/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Saturn Instrument Unit header

  Project Apollo is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Project Apollo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Project Apollo; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  See http://nassp.sourceforge.net/license/ for more details.

  **************************** Revision History ****************************
  *	$Log$
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"

#include "soundlib.h"
#include "nasspsound.h"
#include "nasspdefs.h"

#include "ioChannels.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"

#include "saturn.h"


IU::IU(SoundLib &s, CSMcomputer &cmc) : soundlib(s), agc(cmc)

{
	TLIBurnTime = 0.0;
	TLIBurnDeltaV = 0.0;
	TLICapable = false;
	TLIBurnStart = false;
	TLIBurnDone = false;
	State = 0;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;
	FirstTimeStepDone = false;

	OurVessel = 0;
	th_SIVB = 0;
	ph_SIVB = 0;
	thg_aps = 0;
	Realism = REALISM_DEFAULT;
	Crewed = true;

	SIVBBurn = false;
	SIVBBurnStart = 0;
	SIVBApogee = 0;
}

IU::~IU()

{
	// Nothing for now.
}

void IU::SetMissionInfo(bool tlicapable, bool crewed, int realism, THRUSTER_HANDLE *th, PROPELLANT_HANDLE *ph, 
						THGROUP_HANDLE *thg, double sivbburnstart, double sivbapogee)
{
	TLICapable = tlicapable;
	Crewed = crewed;
	Realism = realism;
	th_SIVB = th;
	ph_SIVB = ph;
	thg_aps = thg;

	SIVBBurnStart = sivbburnstart;
	SIVBApogee = sivbapogee;

	if (!Crewed && (SIVBApogee > 0.0) && (SIVBBurnStart > 0) && (OurVessel->GetStage() < CSM_LEM_STAGE)) {
		SIVBBurn = true;
	}

	if (!TLIBurnDone) { 
		soundlib.LoadMissionSound(STLI, GO_FOR_TLI_SOUND, NULL);
		soundlib.LoadMissionSound(STLIStart, TLI_START_SOUND, NULL);
		soundlib.LoadMissionSound(SecoSound, SECO_SOUND, SECO_SOUND);
		soundlib.LoadSound(Scount, COUNT10_SOUND);
		soundlib.LoadSound(SepS, SEPMOTOR_SOUND, INTERNAL_ONLY);
	}
}

void IU::Timestep(double simt, double simdt)

{
	double MainLevel;

	// Only SIVB in orbit for now
	if (OurVessel->GetStage() != STAGE_ORBIT_SIVB) return;

	// Initialization
	if (!FirstTimeStepDone) {

		//
		// Disable the engines if we're waiting for
		// the user to start the TLI burn or if it's been done.
		//
		if (Realism && (State <= 107 || State >= 202)) {
			OurVessel->SetThrusterResource(*th_SIVB, NULL);
		
		} else {
			OurVessel->SetThrusterResource(*th_SIVB, *ph_SIVB);
		}
		
		FirstTimeStepDone = true;
		return;
	}

	// Switches to inhibit TLI
	// TODO: Introduce event handling when the new SPSDK arrives.
	bool XLunarSwitch = (OurVessel->GetSwitchState("TLIEnableSwitch") == TOGGLESWITCH_UP);
	bool SIISIVBSepSwitch = (OurVessel->GetSwitchState("SIISIVBSepSwitch") == TOGGLESWITCH_UP);

	if (TLICapable) {
		switch (State) {

		case 0:
			if (Crewed || !SIVBBurn) {
				State = 100;
			}
			else {
				NextMissionEventTime = SIVBBurnStart - 100.0;
				State++;
			}
			break;

		case 1:
			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				OurVessel->SlowIfDesired();
				OurVessel->ActivateS4RCS();
				State++;
			}
			break;

		case 2:
			OurVessel->ActivateNavmode(NAVMODE_PROGRADE);
			NextMissionEventTime = SIVBBurnStart;
			State++;
			break;

		case 3:
			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				OurVessel->ActivateNavmode(NAVMODE_PROGRADE);
				SIVBStart();
				State = 100;
			}
			break;

		case 100:
			//
			// Fuel boiloff every ten seconds.
			//

			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				SIVBBoiloff();

				NextMissionEventTime = OurVessel->GetMissionTime() + 10.0;
			}

			//
			// Detect start signal.
			//

			if (TLIBurnStart) { 
				
				// TLI Inhibit
				if (!XLunarSwitch) {

					TLIBurnStart = false;
				
				} else {
				
					//
					// Start signal is sent 9:38 before SIVB ignition.
					//

					OurVessel->SetSIISep();

					//
					// For now we'll enable prograde autopilot.
					//
					OurVessel->ActivateNavmode(NAVMODE_PROGRADE);

					// Reset time acceleration to normal.
					oapiSetTimeAcceleration(1);

					NextMissionEventTime = OurVessel->GetMissionTime() + 10.0;
					State++;
				}
			}
			break;

		case 101:
			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				OurVessel->ClearSIISep();

				// Next event is 100s before ignition
				NextMissionEventTime = OurVessel->GetMissionTime() + ((9.0 * 60.0) + 38.0 - 10.0 - 100.0); 
				State++;
			}

			// TLI inhibit
			if (!XLunarSwitch) {
				OurVessel->ClearSIISep();
				TLIInhibit();
			}				
			break;

		case 102:
			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				
				// Reset time acceleration to normal.
				oapiSetTimeAcceleration(1);

				STLI.play();

				// Next event is 84s before ignition
				NextMissionEventTime = OurVessel->GetMissionTime() + 16.0;
				State++;
			}

			// TLI inhibit
			if (!XLunarSwitch) {
				TLIInhibit();
			}				
			break;

		case 103:
			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				OurVessel->SetSIISep();

				// Next event is 82s before ignition
				NextMissionEventTime = OurVessel->GetMissionTime() + 2.0;
				State++;
			}

			// TLI inhibit
			if (!XLunarSwitch) {
				TLIInhibit();
			}				
			break;

		case 104:
			//
			// Start ullage engines 
			//

			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				OurVessel->SetAttitudeLinLevel(2, 1);

				OurVessel->SetThrusterGroupLevel(*thg_aps, 1.0);
				SepS.play(LOOP, 130);

				// Next event is 18s before ignition
				NextMissionEventTime += 64.0;
				State++;
			}

			// TLI inhibit
			if (!XLunarSwitch) {
				OurVessel->ClearSIISep();
				TLIInhibit();
			}				
			break;

		case 105:
			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				OurVessel->ClearSIISep();

				// Next event is 10.9s before ignition
				NextMissionEventTime = OurVessel->GetMissionTime() + 7.1;
				State++;
			}

			// TLI inhibit
			if (!XLunarSwitch) {
				OurVessel->ClearSIISep();

				OurVessel->SetAttitudeLinLevel(2, 0);
				OurVessel->SetThrusterGroupLevel(*thg_aps, 0.0);
				SepS.stop();

				TLIInhibit();
			}				
			break;

		case 106:
			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				// Reset time acceleration to normal.
				oapiSetTimeAcceleration(1);

				// And play the countdown.
				Scount.play(NOLOOP,245);

				// Next event is 5s before ignition
				NextMissionEventTime = OurVessel->GetMissionTime() + 5.9;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSepSwitch) {
				OurVessel->SetAttitudeLinLevel(2, 0);
				OurVessel->SetThrusterGroupLevel(*thg_aps, 0.0);
				SepS.stop();

				TLIInhibit();
			}				
			break;

		case 107:
			//
			// Ullage cutoff
			//

			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				OurVessel->SetThrusterGroupLevel(*thg_aps, 0.0);
				SepS.stop();

				// Next event is 1s before ignition
				NextMissionEventTime += 4.0;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSepSwitch) {
				OurVessel->SetAttitudeLinLevel(2, 0);
				OurVessel->SetThrusterGroupLevel(*thg_aps, 0.0);
				SepS.stop();

				Scount.stop();

				TLIInhibit();
			}				
			break;

		case 108:
			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				if (Realism)
					OurVessel->SetThrusterResource(*th_SIVB, *ph_SIVB);

				OurVessel->SetEngineIndicator(1);
				OurVessel->ActivateNavmode(NAVMODE_PROGRADE);

				// Next event is ignition
				LastMissionEventTime = NextMissionEventTime;
				NextMissionEventTime += 1.0;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSepSwitch) {
				OurVessel->SetAttitudeLinLevel(2, 0);
				Scount.stop();

				TLIInhibit();
			}				
			break;

		case 109:
			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				// IGNITION
				
				LastMissionEventTime = NextMissionEventTime;
				NextMissionEventTime += 1.0;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSepSwitch) {
				OurVessel->SetAttitudeLinLevel(2, 0);
				OurVessel->ClearEngineIndicator(1);
				Scount.stop();

				TLIInhibit();
			}				
			break;

		case 110:
			// Engine ramps up to 90% thrust.
			if (OurVessel->GetMissionTime() < NextMissionEventTime) {
				double deltat = (OurVessel->GetMissionTime() - LastMissionEventTime);
				OurVessel->SetThrusterLevel(*th_SIVB, 0.9 * deltat);
			}
			else {				
				LastMissionEventTime = NextMissionEventTime;
				NextMissionEventTime += 0.5;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSepSwitch) {
				OurVessel->SetAttitudeLinLevel(2, 0);
				OurVessel->ClearEngineIndicator(1);

				TLIInhibit();
			}				
			break;

		case 111:
			// Then up to 100%.
			if (OurVessel->GetMissionTime() < NextMissionEventTime) {
				double deltat = (OurVessel->GetMissionTime() - LastMissionEventTime);
				OurVessel->SetThrusterLevel(*th_SIVB, 0.9 + (deltat * 0.2));
			}
			else {
				NextMissionEventTime += 0.5;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSepSwitch) {
				OurVessel->SetAttitudeLinLevel(2, 0);
				OurVessel->ClearEngineIndicator(1);

				TLIInhibit();
			}				
			break;

		case 112:
			//
			// Engine will be at 100% thrust
			//

			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				OurVessel->SetThrusterLevel(*th_SIVB, 1.0);
				OurVessel->SetAttitudeLinLevel(2, 0);
				OurVessel->ClearEngineIndicator(1);

				STLIStart.play();

				if (!SIVBBurn || Crewed) {
					State = 200;
				}
				else {
					State = 150;
				}
			}

			// TLI inhibit
			if (SIISIVBSepSwitch) {
				OurVessel->SetAttitudeLinLevel(2, 0);
				OurVessel->ClearEngineIndicator(1);

				TLIInhibit();
			}				
			break;

		//
		// Wait for the right apogee.
		//

		case 150:
			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				OBJHANDLE hPlanet = OurVessel->GetGravityRef();
				double prad = oapiGetSize(hPlanet);
				double ap;
				OurVessel->GetApDist(ap);

				OurVessel->ActivateNavmode(NAVMODE_PROGRADE);

				//
				// Burn until the orbit is about right or we're out of fuel.
				//

				if ((ap >= (prad + (SIVBApogee * 1000.0))) || (((OurVessel->GetFuelMass() * 100.0) / OurVessel->GetMaxFuelMass()) <= 0.1)) {
					State = 201;
					SIVBBurn = false;
					OurVessel->DeactivateNavmode(NAVMODE_PROGRADE);
					OurVessel->DeactivateS4RCS();
				}

				NextMissionEventTime = OurVessel->GetMissionTime() + 0.25;
			}
			break;

		case 200:

			//
			// Wait for shutdown.
			//

			OurVessel->ActivateNavmode(NAVMODE_PROGRADE);

			if (TLIBurnDone || OurVessel->GetPropellantMass(*ph_SIVB) == 0) {
				State++;
			}

			// TLI inhibit
			if (SIISIVBSepSwitch) {
				if (OurVessel->GetMissionTime() >= NextMissionEventTime + 10.0) {	// non-permanent inhibit only until T+00:12, NextMissionEventTime is ignition + 2s
					State++;

				} else {
					TLIInhibit();
				}
			}				
			break;

		case 201:

			//
			// Thrust decay.
			//

			OurVessel->SetEngineIndicator(1);

			MainLevel = OurVessel->GetThrusterLevel(*th_SIVB);
			MainLevel -= (simdt * 1.2);
			OurVessel->SetThrusterLevel(*th_SIVB, MainLevel);

			if (MainLevel <= 0.0) {
				SIVBStop();
				NextMissionEventTime = OurVessel->GetMissionTime() + 10.0 - 1.0 / 1.2;
				State++;
			}
			break;

		case 202:
			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				OurVessel->DeactivateNavmode(NAVMODE_PROGRADE);
				OurVessel->ClearEngineIndicator(1);

				if (Realism < 2)
					State = 100;
				else
					State++;
			}
			break;

		case 203:

			//
			// Engine is now dead. Just boil off the remaining
			// fuel.
			//

			if (OurVessel->GetMissionTime() >= NextMissionEventTime) {
				SIVBBoiloff();
				NextMissionEventTime = OurVessel->GetMissionTime() + 10.0;
			}
			break;
		}
	}
	else {
		if (OurVessel->GetEngineLevel(ENGINE_MAIN) <= 0) {
			if (Realism)
				OurVessel->SetThrusterResource(*th_SIVB, NULL);
		}
	}
}

void IU::TLIInhibit()

{
	OurVessel->SetThrusterLevel(*th_SIVB, 0);
	if (Realism)
		OurVessel->SetThrusterResource(*th_SIVB, NULL);

	OurVessel->DeactivateNavmode(NAVMODE_PROGRADE);

	TLIBurnStart = false;
	State = 100;
}

void IU::ChannelOutput(int address, int value)

{
	ChannelValue12 val12;

 	if (address == 012) {
    	val12.Value = value;
		if (val12.Bits.SIVBIgnitionSequenceStart) {
			SIVBStart();

			val12.Bits.SIVBIgnitionSequenceStart = false;			
			agc.SetOutputChannel(012, val12.Value);
		}

		if (val12.Bits.SIVBCutoff) {
			if (TLIBurnStart) 
				TLIBurnDone = true;

			val12.Bits.SIVBCutoff = false;			
			agc.SetOutputChannel(012, val12.Value);
		}
	}
}

bool IU::SIVBStart()

{
	if (!TLICapable || TLIBurnStart || TLIBurnDone)
		return false;

	if (OurVessel->GetStage() != STAGE_ORBIT_SIVB)
		return false;

	TLIBurnStart = true;
	return true;
}

void IU::SIVBStop()

{
	if (OurVessel->GetStage() != STAGE_ORBIT_SIVB)
		return;

	OurVessel->SetEngineLevel(ENGINE_MAIN, 0);
	if (Realism)
		OurVessel->SetThrusterResource(*th_SIVB, NULL);

	SecoSound.play();

	TLIBurnStart = false;
	TLIBurnDone = true;

	Scount.done();
	STLI.done();
	STLIStart.done();
	SecoSound.done();
	SepS.done();
}

void IU::SIVBBoiloff()

{
	if (Realism < 2)
		return;

	//
	// The SIVB stage boils off a small amount of fuel while in orbit.
	//
	// For the time being we'll ignore any thrust created by the venting
	// of this fuel.
	//

	double FuelMass = OurVessel->GetPropellantMass(*ph_SIVB) * 0.99998193;
	OurVessel->SetPropellantMass(*ph_SIVB, FuelMass);
}

void IU::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, IU_START_STRING);

	oapiWriteScenario_int(scn, "STATE", State);
	oapiWriteScenario_int(scn, "TLIBURNSTART", TLIBurnStart);
	oapiWriteScenario_int(scn, "TLIBURNDONE", TLIBurnDone);
	oapiWriteScenario_float(scn, "NEXTMISSIONEVENTTIME", NextMissionEventTime);
	oapiWriteScenario_float(scn, "LASTMISSIONEVENTTIME", LastMissionEventTime);

	oapiWriteScenario_float(scn, "TLITIME", TLIBurnTime);
	oapiWriteScenario_float(scn, "TLIDV", TLIBurnDeltaV);

	oapiWriteLine(scn, IU_END_STRING);
}

void IU::LoadState(FILEHANDLE scn)

{
	char *line;
	float flt = 0;
	int i = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, IU_END_STRING, sizeof(IU_END_STRING)))
			return;

		if (!strnicmp (line, "TLITIME", 7)) {
			sscanf(line + 7, "%f", &flt);
			TLIBurnTime = flt;
		}
		else if (!strnicmp (line, "TLIDV", 5)) {
			sscanf(line + 5, "%f", &flt);
			TLIBurnDeltaV = flt;
		}
		else if (!strnicmp (line, "STATE", 5)) {
			sscanf (line + 5, "%d", &State);
		}
		else if (!strnicmp (line, "TLIBURNSTART", 12)) {
			sscanf (line + 12, "%d", &i);
			TLIBurnStart = (i != 0);
		}
		else if (!strnicmp (line, "TLIBURNDONE", 11)) {
			sscanf (line + 11, "%d", &i);
			TLIBurnDone = (i != 0);
		}
		else if (!strnicmp (line, "NEXTMISSIONEVENTTIME", 20)) {
			sscanf(line + 20, "%f", &flt);
			NextMissionEventTime = flt;
		}
		else if (!strnicmp (line, "LASTMISSIONEVENTTIME", 20)) {
			sscanf(line + 20, "%f", &flt);
			LastMissionEventTime = flt;
		}
	}
}
