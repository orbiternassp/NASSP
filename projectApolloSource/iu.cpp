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
  *	Revision 1.10  2006/10/30 18:37:58  tschachim
  *	Bugfix IUToLVCommandConnector::GetApDist
  *	
  *	Revision 1.9  2006/07/09 16:09:38  movieman523
  *	Added Prog 59 for SIVb venting.
  *	
  *	Revision 1.8  2006/07/09 00:07:07  movieman523
  *	Initial tidy-up of connector code.
  *	
  *	Revision 1.7  2006/07/07 19:44:58  movieman523
  *	First version of connector support.
  *	
  *	Revision 1.6  2006/06/23 11:56:48  tschachim
  *	New Project Apollo MFD for TLI burn control.
  *	
  *	Revision 1.5  2006/06/08 15:27:59  tschachim
  *	SIVB is can be controlled manually in the Virtual AGC case
  *	until we'll have a smarter IU.
  *	
  *	Revision 1.4  2006/04/25 13:41:11  tschachim
  *	Removed GetXXXSwitchState.
  *	
  *	Revision 1.3  2006/02/22 18:47:35  tschachim
  *	Bugfixes for Apollo 4-6.
  *	
  *	Revision 1.2  2006/02/21 12:19:52  tschachim
  *	Moved TLI sequence to the IU.
  *	
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

#include "connector.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"

#include "saturn.h"
#include "sivb.h"

IU::IU() 

{
	TLICapable = false;
	TLIBurnStart = false;
	TLIBurnDone = false;
	State = 0;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;
	FirstTimeStepDone = false;

	Realism = REALISM_DEFAULT;
	Crewed = true;
	VesselISP = 0;
	VesselThrust = 0;

	SIVBBurn = false;
	SIVBBurnStart = 0;
	SIVBApogee = 0;

	//
	// Generic thrust decay value. This still needs tweaking.
	//

	TLIBurnState = 0;
	TLIThrustDecayDV = 6.1;
	TLIBurnTime = 0.0;
	TLIBurnStartTime = 0.0;
	TLIBurnEndTime = 0.0;
	TLIBurnDeltaV = 0.0;
	TLICutOffVel = 0.0;
	TLILastAltitude = 0.0;

	commandConnector.SetIU(this);
}

IU::~IU()

{
	// Nothing for now.
}

void IU::SetVesselStats(double ISP, double Thrust)

{
	VesselISP = ISP;
	VesselThrust = Thrust;
}

void IU::GetVesselStats(double &ISP, double &Thrust)

{
	ISP = VesselISP;
	Thrust = VesselThrust;
}

void IU::SetMissionInfo(bool tlicapable, bool crewed, int realism, double sivbburnstart, double sivbapogee)
{
	TLICapable = tlicapable;
	Crewed = crewed;
	Realism = realism;

	SIVBBurnStart = sivbburnstart;
	SIVBApogee = sivbapogee;

	if (!Crewed && (SIVBApogee > 0.0) && (SIVBBurnStart > 0) && (lvCommandConnector.GetStage() < CSM_LEM_STAGE))
	{
		SIVBBurn = true;
	}
}

void IU::Timestep(double simt, double simdt)

{
	double MainLevel;

	// Only SIVB in orbit for now
	if (lvCommandConnector.GetStage() != STAGE_ORBIT_SIVB) return;

	//
	// Update mission time.
	//
	MissionTime = simt;

	// Initialization
	if (!FirstTimeStepDone) {

		//
		// Disable the engines if we're waiting for
		// the user to start the TLI burn or if it's been done.
		//
		if (Realism && !commandConnector.IsVirtualAGC() && (State <= 107 || State >= 202))
		{
			lvCommandConnector.EnableDisableJ2(false);
		}
		else
		{
			lvCommandConnector.EnableDisableJ2(true);
		}
		
		FirstTimeStepDone = true;
		return;
	}

	// Switches to inhibit TLI
	bool XLunar = (commandConnector.TLIEnableSwitchState() == TOGGLESWITCH_UP);
	bool SIISIVBSep = (commandConnector.SIISIVbSwitchState() == TOGGLESWITCH_UP);

	// TLI burn calculations
	switch (TLIBurnState) {

		case 0:
			// Disabled
			break;

		case 1:
			// Waiting for sequence start
			DoTLICalcs();
			// Burn sequence begins 9:38 min before ignition
			if (TLIBurnStartTime - MissionTime <= (9.0 * 60.0 + 38.0)) {
				if (SIVBStart()) 
				{
					TLIBurnState++;
				} 
				else 
				{
					TLIBurnState = 0;
				}
			}
			// TLI inhibit
			if (!XLunar) {
				TLIBurnState = 0;
			}				
			break;

		case 2:
			// Waiting for ignition
			DoTLICalcs();
			if (lvCommandConnector.GetJ2ThrustLevel() >= 1.0) {
				TLILastAltitude = lvCommandConnector.GetAltitude();
				TLIBurnState++;
			}
			// Inhibited?
			if (!TLIBurnStart)
				TLIBurnState = 0;
			break;

		case 3:
			// Monitor burn... 
			UpdateTLICalcs();

			// ...and cut off the engine at the appropriate velocity.
			VESSELSTATUS status;
			lvCommandConnector.GetStatus(status);
			if (length(status.rvel) >= TLICutOffVel) {
				if (TLIBurnStart) 
					TLIBurnDone = true;

				TLIBurnState = 0;
			}
			// Inhibited?
			if (!TLIBurnStart)
				TLIBurnState = 0;
			break;
		
	}

	if (TLICapable) {
		switch (State) {

		case 0:
			if (Crewed || !SIVBBurn) {
				State = 100;
			}
			else {
				NextMissionEventTime = SIVBBurnStart - (9.0 * 60.0) - 38.0 - 10.0;
				State++;
			}
			break;

		case 1:
			if (MissionTime >= NextMissionEventTime) {
				commandConnector.SlowIfDesired();
				lvCommandConnector.ActivateS4RCS();
				State++;
			}
			break;

		case 2:
			NextMissionEventTime = SIVBBurnStart - (9.0 * 60.0) - 38.0 ;
			State++;
			break;

		case 3:
			if (MissionTime >= NextMissionEventTime) {
				SIVBStart();
				State = 100;
			}
			break;

		case 100:

			//
			// Detect start signal.
			//

			if (TLIBurnStart) 
			{ 
				// TLI Inhibit
				if (!XLunar)
				{
					TLIBurnStart = false;
				} 
				else 
				{
				
					//
					// Start signal is sent 9:38 before SIVB ignition.
					//

					commandConnector.SetSIISep();

					//
					// For now we'll enable prograde autopilot.
					//
					lvCommandConnector.ActivateNavmode(NAVMODE_PROGRADE);

					// Reset time acceleration to normal.
					oapiSetTimeAcceleration(1);

					NextMissionEventTime = MissionTime + 38.0;
					State++;
				}
			}
			break;

		case 101:
			if (MissionTime >= NextMissionEventTime) {
				commandConnector.ClearSIISep();

				// Next event is 100s before ignition
				NextMissionEventTime = MissionTime + ((9.0 * 60.0) - 100.0); 
				State++;
			}

			// TLI inhibit
			if (!XLunar) {
				commandConnector.ClearSIISep();
				TLIInhibit();
			}				
			break;

		case 102:
			if (MissionTime >= NextMissionEventTime) {
				
				// Reset time acceleration to normal.
				oapiSetTimeAcceleration(1);

				commandConnector.PlayTLISound(true);

				// Next event is 84s before ignition
				NextMissionEventTime = MissionTime + 16.0;
				State++;
			}

			// TLI inhibit
			if (!XLunar) {
				TLIInhibit();
			}				
			break;

		case 103:
			if (MissionTime >= NextMissionEventTime) {
				commandConnector.SetSIISep();

				// Next event is 82s before ignition
				NextMissionEventTime = MissionTime + 2.0;
				State++;
			}

			// TLI inhibit
			if (!XLunar) {
				TLIInhibit();
			}				
			break;

		case 104:
			//
			// Start ullage engines 
			//

			if (MissionTime >= NextMissionEventTime) {
				lvCommandConnector.SetAttitudeLinLevel(2, 1);
				lvCommandConnector.SetAPSThrustLevel(1.0);
				commandConnector.PlaySepsSound(true);

				// Next event is 18s before ignition
				NextMissionEventTime += 64.0;
				State++;
			}

			// TLI inhibit
			if (!XLunar)
			{
				commandConnector.ClearSIISep();
				TLIInhibit();
			}				
			break;

		case 105:
			if (MissionTime >= NextMissionEventTime)
			{
				commandConnector.ClearSIISep();

				// Next event is 10.9s before ignition
				NextMissionEventTime = MissionTime + 7.1;
				State++;
			}

			// TLI inhibit
			if (!XLunar)
			{
				commandConnector.ClearSIISep();

				lvCommandConnector.SetAttitudeLinLevel(2, 0);
				lvCommandConnector.SetAPSThrustLevel(0.0);
				commandConnector.PlaySepsSound(false);

				TLIInhibit();
			}				
			break;

		case 106:
			if (MissionTime >= NextMissionEventTime) {
				// Reset time acceleration to normal.
				oapiSetTimeAcceleration(1);

				// And play the countdown.
				commandConnector.PlayCountSound(true);

				// Next event is 5s before ignition
				NextMissionEventTime = MissionTime + 5.9;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSep) {
				lvCommandConnector.SetAttitudeLinLevel(2, 0);
				lvCommandConnector.SetAPSThrustLevel(0.0);
				commandConnector.PlaySepsSound(false);

				TLIInhibit();
			}				
			break;

		case 107:
			//
			// Ullage cutoff
			//

			if (MissionTime >= NextMissionEventTime) {
				lvCommandConnector.SetAPSThrustLevel(0.0);
				commandConnector.PlaySepsSound(false);

				// Next event is 1s before ignition
				NextMissionEventTime += 4.0;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSep) {
				lvCommandConnector.SetAttitudeLinLevel(2, 0);
				lvCommandConnector.SetAPSThrustLevel(0.0);

				commandConnector.PlaySepsSound(false);
				commandConnector.PlayCountSound(false);

				TLIInhibit();
			}				
			break;

		case 108:
			if (MissionTime >= NextMissionEventTime) {
				if (Realism)
					lvCommandConnector.EnableDisableJ2(true);

				commandConnector.SetEngineIndicator(1);
				lvCommandConnector.ActivateNavmode(NAVMODE_PROGRADE);

				// Next event is ignition
				LastMissionEventTime = NextMissionEventTime;
				NextMissionEventTime += 1.0;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSep) {
				lvCommandConnector.SetAttitudeLinLevel(2, 0);
				commandConnector.PlayCountSound(false);

				TLIInhibit();
			}				
			break;

		case 109:
			if (MissionTime >= NextMissionEventTime) {
				// IGNITION
				
				LastMissionEventTime = NextMissionEventTime;
				NextMissionEventTime += 1.0;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSep) {
				lvCommandConnector.SetAttitudeLinLevel(2, 0);
				commandConnector.ClearEngineIndicator(1);
				commandConnector.PlayCountSound(false);

				TLIInhibit();
			}				
			break;

		case 110:
			// Engine ramps up to 90% thrust.
			if (MissionTime < NextMissionEventTime) {
				double deltat = (MissionTime - LastMissionEventTime);
				lvCommandConnector.SetJ2ThrustLevel(0.9 * deltat);
			}
			else {				
				LastMissionEventTime = NextMissionEventTime;
				NextMissionEventTime += 0.5;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSep) {
				lvCommandConnector.SetAttitudeLinLevel(2, 0);
				commandConnector.ClearEngineIndicator(1);

				TLIInhibit();
			}				
			break;

		case 111:
			// Then up to 100%.
			if (MissionTime < NextMissionEventTime) {
				double deltat = (MissionTime - LastMissionEventTime);
				lvCommandConnector.SetJ2ThrustLevel(0.9 + (deltat * 0.2));
			}
			else {
				NextMissionEventTime += 0.5;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSep) {
				lvCommandConnector.SetAttitudeLinLevel(2, 0);
				commandConnector.ClearEngineIndicator(1);

				TLIInhibit();
			}				
			break;

		case 112:
			//
			// Engine will be at 100% thrust
			//

			if (MissionTime >= NextMissionEventTime)
			{
				lvCommandConnector.SetJ2ThrustLevel(1.0);
				lvCommandConnector.SetAttitudeLinLevel(2, 0);
				commandConnector.ClearEngineIndicator(1);

				commandConnector.PlayTLIStartSound(true);

				if (!SIVBBurn || Crewed)
				{
					State = 200;
				}
				else
				{
					State = 150;
				}
			}

			// TLI inhibit
			if (SIISIVBSep) {
				lvCommandConnector.SetAttitudeLinLevel(2, 0);
				commandConnector.ClearEngineIndicator(1);

				TLIInhibit();
			}				
			break;

		//
		// Wait for the right apogee.
		//

		case 150:
			if (MissionTime >= NextMissionEventTime) {
				OBJHANDLE hPlanet = lvCommandConnector.GetGravityRef();
				double prad = oapiGetSize(hPlanet);
				double ap;
				lvCommandConnector.GetApDist(ap);

				lvCommandConnector.ActivateNavmode(NAVMODE_PROGRADE);

				//
				// Burn until the orbit is about right or we're out of fuel.
				//

				if ((ap >= (prad + (SIVBApogee * 1000.0))) || (((lvCommandConnector.GetPropellantMass() * 100.0) / lvCommandConnector.GetMaxFuelMass()) <= 0.1)) {
					State = 201;
					SIVBBurn = false;
					lvCommandConnector.DeactivateNavmode(NAVMODE_PROGRADE);
					lvCommandConnector.DeactivateS4RCS();
				}

				NextMissionEventTime = MissionTime + 0.25;
			}
			break;

		case 200:

			//
			// Wait for shutdown.
			//

			lvCommandConnector.ActivateNavmode(NAVMODE_PROGRADE);

			if (TLIBurnDone || lvCommandConnector.GetPropellantMass() < 0.001)
			{
				State++;
			}

			// TLI inhibit
			if (SIISIVBSep) {
				if (MissionTime >= NextMissionEventTime + 10.0) {	// non-permanent inhibit only until T+00:12, NextMissionEventTime is ignition + 2s
					State++;
				}
				else
				{
					TLIInhibit();
				}
			}				
			break;

		case 201:

			//
			// Thrust decay.
			//

			commandConnector.SetEngineIndicator(1);

			MainLevel = lvCommandConnector.GetJ2ThrustLevel();
			MainLevel -= (simdt * 1.2);
			lvCommandConnector.SetJ2ThrustLevel(MainLevel);

			if (MainLevel <= 0.0) {
				SIVBStop();
				NextMissionEventTime = MissionTime + 10.0 - 1.0 / 1.2;
				State++;
			}
			break;

		case 202:
			if (MissionTime >= NextMissionEventTime) {
				lvCommandConnector.DeactivateNavmode(NAVMODE_PROGRADE);
				commandConnector.ClearEngineIndicator(1);

				if (Realism < 2)
					State = 100;
				else
					State++;
			}
			break;

		case 203:

			//
			// Engine is now dead.
			//
			break;
		}
	}
	else {
		if (lvCommandConnector.GetJ2ThrustLevel() <= 0) {
			if (Realism && !commandConnector.IsVirtualAGC())
			{
				lvCommandConnector.EnableDisableJ2(false);
			}
		}
	}
}

void IU::TLIInhibit()

{
	lvCommandConnector.SetJ2ThrustLevel(0.0);
	if (Realism&& !commandConnector.IsVirtualAGC())
	{
		lvCommandConnector.EnableDisableJ2(false);
	}

	lvCommandConnector.DeactivateNavmode(NAVMODE_PROGRADE);

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
			commandConnector.SetAGCOutputChannel(012, val12.Value);
		}

		if (val12.Bits.SIVBCutoff) {
			if (TLIBurnStart) 
				TLIBurnDone = true;

			val12.Bits.SIVBCutoff = false;			
			commandConnector.SetAGCOutputChannel(012, val12.Value);
		}
	}
}

bool IU::SIVBStart()

{
	if (!TLICapable || TLIBurnStart || TLIBurnDone)
		return false;

	if (lvCommandConnector.GetStage() != STAGE_ORBIT_SIVB)
		return false;

	commandConnector.LoadTLISounds();

	TLIBurnStart = true;
	return true;
}

void IU::SIVBStop()

{
	if (lvCommandConnector.GetStage() != STAGE_ORBIT_SIVB)
		return;

	lvCommandConnector.SetJ2ThrustLevel(0.0);
	lvCommandConnector.SetVentingThruster();

	if (Realism && !commandConnector.IsVirtualAGC())
	{
		lvCommandConnector.EnableDisableJ2(false);
	}

	commandConnector.PlaySecoSound(true);
	commandConnector.ClearTLISounds();

	TLIBurnStart = false;
	TLIBurnDone = true;
}

bool IU::StartTLIBurn(double timeToEjection, double dV) 

{
	if (!TLICapable || TLIBurnStart || TLIBurnDone)
		return false;

	if (lvCommandConnector.GetStage() != STAGE_ORBIT_SIVB)
		return false;

	if (TLIBurnState != 0) 
		return false;

	TLIBurnDeltaV = dV;
	TLIBurnTime = MissionTime + timeToEjection;

	if (!DoTLICalcs())
		return false;

	// Burn sequence begins 9:38 min before ignition
	if (TLIBurnStartTime - MissionTime <= (9.0 * 60.0 + 38.0))
		return false;

	TLIBurnState = 1;
	return true;
}

bool IU::DoTLICalcs()

{
	extern void PredictPosVelVectors(const VECTOR3 &Pos, const VECTOR3 &Vel, double a, double Mu,
						  double Time, VECTOR3 &NewPos, VECTOR3 &NewVel, double &NewVelMag);
	//
	// We know the expected deltaV, so we need to calculate the time for the burn,
	// and the expected end velocity.
	//

	double mass = lvCommandConnector.GetMass();
	double isp = VesselISP;
	double fuelmass = lvCommandConnector.GetPropellantMass();
	double thrust = VesselThrust;

	double massrequired = mass * (1.0 - exp(-((TLIBurnDeltaV - TLIThrustDecayDV) / isp)));

	if (massrequired > fuelmass) {
		return false;
	}

	double deltaT = massrequired / (thrust / isp);

	//
	// Start the burn early, subtracting half the burn length.
	//

	TLIBurnStartTime = TLIBurnTime - (deltaT / 2.0);
	TLIBurnEndTime = TLIBurnTime + (deltaT / 2.0);

	//
	// Now calculate the cutoff velocity.
	//

	VECTOR3 Pos, Vel;
	OBJHANDLE GravityRef = lvCommandConnector.GetGravityRef();
	lvCommandConnector.GetRelativePos(GravityRef, Pos);
	lvCommandConnector.GetRelativeVel(GravityRef, Vel);

	ELEMENTS el;
	double mjd_ref;
	lvCommandConnector.GetElements(el, mjd_ref);

	VECTOR3 NewPos, NewVel;
	double NewVelMag;

	PredictPosVelVectors(Pos, Vel, el.a, 3.986e14,
						  TLIBurnTime - MissionTime, NewPos, NewVel, NewVelMag);

	TLICutOffVel = NewVelMag + TLIBurnDeltaV - TLIThrustDecayDV;
	return true;
}

void IU::UpdateTLICalcs()

{
	double MaxE = TLICutOffVel * TLICutOffVel;

	OBJHANDLE hbody = lvCommandConnector.GetGravityRef();
	double alt = lvCommandConnector.GetAltitude();
	double bradius = oapiGetSize(hbody);
	double dist = bradius + alt;
	double g = (G * bradius * bradius) / (dist * dist);
	double deltaE = 2.0 * (g * (alt - TLILastAltitude));

	TLILastAltitude = alt;
	TLICutOffVel = sqrt(MaxE - deltaE);
}

void IU::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, IU_START_STRING);

	oapiWriteScenario_int(scn, "STATE", State);
	oapiWriteScenario_int(scn, "TLIBURNSTART", TLIBurnStart);
	oapiWriteScenario_int(scn, "TLIBURNDONE", TLIBurnDone);
	oapiWriteScenario_float(scn, "NEXTMISSIONEVENTTIME", NextMissionEventTime);
	oapiWriteScenario_float(scn, "LASTMISSIONEVENTTIME", LastMissionEventTime);

	oapiWriteScenario_int(scn, "TLIBURNSTATE", TLIBurnState);
	oapiWriteScenario_float(scn, "TLITIME", TLIBurnTime);
	oapiWriteScenario_float(scn, "TLIDV", TLIBurnDeltaV);
	oapiWriteScenario_float(scn, "TLIBURNSTARTTIME", TLIBurnStartTime);
	oapiWriteScenario_float(scn, "TLIBURNENDTIME", TLIBurnEndTime);
	oapiWriteScenario_float(scn, "TLICUTOFFVEL", TLICutOffVel);
	oapiWriteScenario_float(scn, "TLILASTALTITUDE", TLILastAltitude);

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
		else if (!strnicmp (line, "TLIBURNSTATE", 12)) {
			sscanf (line + 12, "%d", &TLIBurnState);
		}
		else if (!strnicmp (line, "TLIDV", 5)) {
			sscanf(line + 5, "%f", &flt);
			TLIBurnDeltaV = flt;
		}
		else if (!strnicmp (line, "TLIBURNSTARTTIME", 16)) {
			sscanf(line + 16, "%f", &flt);
			TLIBurnStartTime = flt;
		}
		else if (!strnicmp (line, "TLIBURNENDTIME", 14)) {
			sscanf(line + 14, "%f", &flt);
			TLIBurnEndTime = flt;
		}
		else if (!strnicmp (line, "TLICUTOFFVEL", 12)) {
			sscanf(line + 12, "%f", &flt);
			TLICutOffVel = flt;
		}
		else if (!strnicmp (line, "TLILASTALTITUDE", 15)) {
			sscanf(line + 15, "%f", &flt);
			TLILastAltitude = flt;
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

void IU::ConnectToCSM(Connector *csmConnector)

{
	commandConnector.ConnectTo(csmConnector);
}

void IU::ConnectToMultiConnector(MultiConnector *csmConnector)

{
	csmConnector->AddTo(&commandConnector);
}

void IU::ConnectToLV(Connector *CommandConnector)

{
	lvCommandConnector.ConnectTo(CommandConnector);
}

double IU::GetMass()

{
	return lvCommandConnector.GetMass();
}

double IU::GetFuelMass()

{
	return lvCommandConnector.GetPropellantMass();
}

IUToCSMCommandConnector::IUToCSMCommandConnector()

{
	type = CSM_IU_COMMAND;
	ourIU = 0;
}

IUToCSMCommandConnector::~IUToCSMCommandConnector()

{
}

void IUToCSMCommandConnector::SetAGCOutputChannel(int channel, int value)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_OUTPUT_CHANNEL;
	cm.val1.iValue = channel;
	cm.val2.iValue = value;

	SendMessage(cm);
}

bool IUToCSMCommandConnector::IsVirtualAGC()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_IS_VIRTUAL_AGC;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

void IUToCSMCommandConnector::SetSIISep()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_SII_SEP_LIGHT;
	cm.val1.bValue = true;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearSIISep()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_SII_SEP_LIGHT;
	cm.val1.bValue = false;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SetEngineIndicator(int eng)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_ENGINE_INDICATOR;

	cm.val1.iValue = eng;
	cm.val2.bValue = true;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearEngineIndicator(int eng)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_ENGINE_INDICATOR;

	cm.val1.iValue = eng;
	cm.val2.bValue = false;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SlowIfDesired()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SLOW_IF_DESIRED;

	SendMessage(cm);
}

int IUToCSMCommandConnector::TLIEnableSwitchState()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_TLI_ENABLE_SWITCH_STATE;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return -1;
}

int IUToCSMCommandConnector::SIISIVbSwitchState()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_SIISIVBSEP_SWITCH_STATE;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return -1;
}

void IUToCSMCommandConnector::LoadTLISounds()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_LOAD_TLI_SOUNDS;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearTLISounds()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_CLEAR_TLI_SOUNDS;

	SendMessage(cm);
}

void IUToCSMCommandConnector::PlayCountSound(bool StartStop)

{
	PlayStopSound(IUCSM_PLAY_COUNT_SOUND, StartStop);
}

void IUToCSMCommandConnector::PlaySecoSound(bool StartStop)

{
	PlayStopSound(IUCSM_PLAY_SECO_SOUND, StartStop);
}

void IUToCSMCommandConnector::PlaySepsSound(bool StartStop)

{
	PlayStopSound(IUCSM_PLAY_SEPS_SOUND, StartStop);
}

void IUToCSMCommandConnector::PlayTLISound(bool StartStop)

{
	PlayStopSound(IUCSM_PLAY_TLI_SOUND, StartStop);
}

void IUToCSMCommandConnector::PlayTLIStartSound(bool StartStop)

{
	PlayStopSound(IUCSM_PLAY_TLISTART_SOUND, StartStop);
}

void IUToCSMCommandConnector::PlayStopSound(IUCSMMessageType sound, bool StartStop)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = sound;
	cm.val1.bValue = StartStop;

	SendMessage(cm);
}

//
// Process incoming messages from the CSM.
//

bool IUToCSMCommandConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	IUCSMMessageType messageType;

	messageType = (IUCSMMessageType) m.messageType;

	switch (messageType)
	{
	case CSMIU_SET_VESSEL_STATS:
		if (ourIU)
		{
			ourIU->SetVesselStats(m.val1.dValue, m.val2.dValue);
		}
		return true;

	case CSMIU_START_TLI_BURN:
		if (ourIU)
		{
			ourIU->StartTLIBurn(m.val1.dValue, m.val2.dValue);
		}
		return true;

	case CSMIU_IS_TLI_CAPABLE:
		if (ourIU)
		{
			m.val1.bValue = ourIU->IsTLICapable();
			return true;
		}
		break;

	case CSMIU_CHANNEL_OUTPUT:
		if (ourIU)
		{
			ourIU->ChannelOutput(m.val1.iValue, m.val2.iValue);
			return true;
		}
		break;

	case CSMIU_GET_VESSEL_STATS:
		if (ourIU)
		{
			ourIU->GetVesselStats(m.val1.dValue, m.val2.dValue);
			return true;
		}
		break;

	case CSMIU_GET_VESSEL_MASS:
		if (ourIU)
		{
			m.val1.dValue = ourIU->GetMass();
			return true;
		}
		break;

	case CSMIU_GET_VESSEL_FUEL:
		if (ourIU)
		{
			m.val1.dValue = ourIU->GetFuelMass();
			return true;
		}
		break;
	}

	return false;
}

IUToLVCommandConnector::IUToLVCommandConnector()

{
	type = LV_IU_COMMAND;
}

IUToLVCommandConnector::~IUToLVCommandConnector()

{
}

void IUToLVCommandConnector::EnableDisableJ2(bool Enable)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_ENABLE_J2;
	cm.val1.bValue = Enable;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetJ2ThrustLevel(double thrust)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_J2_THRUST_LEVEL;
	cm.val1.dValue = thrust;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetVentingThruster()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_J2_DONE;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetAPSThrustLevel(double thrust)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_APS_THRUST_LEVEL;
	cm.val1.dValue = thrust;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetAttitudeLinLevel(int a1, int a2)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_ATTITUDE_LIN_LEVEL;
	cm.val1.iValue = a1;
	cm.val2.iValue = a2;

	SendMessage(cm);
}

void IUToLVCommandConnector::ActivateNavmode(int mode)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_ACTIVATE_NAVMODE;
	cm.val1.iValue = mode;

	SendMessage(cm);
}

void IUToLVCommandConnector::DeactivateNavmode(int mode)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_DEACTIVATE_NAVMODE;
	cm.val1.iValue = mode;

	SendMessage(cm);
}

void IUToLVCommandConnector::DeactivateS4RCS()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_DEACTIVATE_S4RCS;

	SendMessage(cm);
}

void IUToLVCommandConnector::ActivateS4RCS()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_ACTIVATE_S4RCS;

	SendMessage(cm);
}

double IUToLVCommandConnector::GetMass()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_MASS;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}


int IUToLVCommandConnector::GetStage()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_STAGE;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return NULL_STAGE;
}

double IUToLVCommandConnector::GetJ2ThrustLevel()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_J2_THRUST_LEVEL;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetAltitude()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_ALTITUDE;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetPropellantMass()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_PROPELLANT_MASS;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetMaxFuelMass()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_MAX_FUEL_MASS;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

void IUToLVCommandConnector::GetStatus(VESSELSTATUS &status)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_STATUS;
	cm.val1.pValue = &status;

	SendMessage(cm);
}

OBJHANDLE IUToLVCommandConnector::GetGravityRef()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_GRAVITY_REF;

	if (SendMessage(cm))
	{
		return cm.val1.hValue;
	}

	return 0;
}

void IUToLVCommandConnector::GetApDist(double &d)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_AP_DIST;

	if (SendMessage(cm))
	{
		d = cm.val1.dValue;
		return;
	}

	d = 0.0;
}

void IUToLVCommandConnector::GetRelativePos(OBJHANDLE ref, VECTOR3 &v)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_RELATIVE_POS;
	cm.val1.hValue = ref;
	cm.val2.pValue = &v;

	SendMessage(cm);
}

void IUToLVCommandConnector::GetRelativeVel(OBJHANDLE ref, VECTOR3 &v)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_RELATIVE_VEL;
	cm.val1.hValue = ref;
	cm.val2.pValue = &v;

	SendMessage(cm);
}

OBJHANDLE IUToLVCommandConnector::GetElements (ELEMENTS &el, double &mjd_ref)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_ELEMENTS;
	cm.val1.pValue = &el;

	if (SendMessage(cm))
	{
		mjd_ref = cm.val2.dValue;
		return cm.val3.hValue;
	}

	return 0;
}