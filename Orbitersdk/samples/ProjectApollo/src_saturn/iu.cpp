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

  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"

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
#include "papi.h"


IU::IU()

{
	State = 0;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;
	TLICapable = false;
	TLIBurnState = 0;
	TLIBurnStart = false;
	TLIBurnDone = false;
	FirstTimeStepDone = false;

	Realism = REALISM_DEFAULT;
	Crewed = true;
	VesselISP = 0;
	VesselThrust = 0;

	SIVBBurn = false;
	SIVBBurnStart = 0;
	SIVBApogee = 0;

	commandConnector.SetIU(this);
	GNC.Configure(&lvCommandConnector, 0);
	ExternalGNC = false;

	AttitudeHold = false;
	AttitudeToHold = _V(0, 0, 0);
	AttitudeToHold2 = _V(0, 0, 0);
}

IU::~IU()

{
	// Nothing for now.
}

void IU::SetVesselStats(double ISP, double Thrust)

{
	VesselISP = ISP;
	VesselThrust = Thrust;
	GNC.SetThrusterForce(VesselThrust, VesselISP, 0);
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

void IU::Timestep(double simt, double simdt, double mjd)

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
		if (Realism && (State <= 107 || State >= 202))	{
			lvCommandConnector.EnableDisableJ2(false);
		} else {
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
			// Burn sequence begins 9:38 min before ignition
			if (((GNC.Get_IgnMJD() - mjd) * 24. * 3600.) <= (9.0 * 60.0 + 38.0)) {
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
			// Waiting for full thrust
			if (lvCommandConnector.GetJ2ThrustLevel() >= 1.0) {	
				TLIBurnState++;
			}
			// Inhibited?
			if (!TLIBurnStart)
				TLIBurnState = 0;
			break;

		case 3:
			// Waiting for cut off
			if (!GNC.IsEngineOn()) {
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
				ExternalGNC = true;
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
				} else {				
					//
					// Start signal is sent 9:38 before SIVB ignition.
					//

					commandConnector.SetSIISep();
		
					// Reset time acceleration to normal.
					oapiSetTimeAcceleration(1);

					NextMissionEventTime = MissionTime + 38.0;
					if (!ExternalGNC) {
						NextMissionEventTime += ((GNC.Get_IgnMJD() - mjd) * 24. * 3600.) - (9.0 * 60.0 + 38.0);
					}						
					State++;
				}
			}
			break;

		case 101:
			if (MissionTime >= NextMissionEventTime) {
				commandConnector.ClearSIISep();

				// Next event is 100s before ignition
				NextMissionEventTime += ((9.0 * 60.0) - 100.0); 
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
				NextMissionEventTime += 16.0;
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
				NextMissionEventTime += 2.0;
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
				NextMissionEventTime += 7.1;
				State++;
			}

			// TLI inhibit
			if (!XLunar)
			{
				commandConnector.ClearSIISep();

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
				NextMissionEventTime += 5.9;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSep) {
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

				// Next event is ignition
				LastMissionEventTime = NextMissionEventTime;

				// Start the ignition sequence 0.255s early in order to compensate the thrust buildup/tail off
				NextMissionEventTime += 0.745;
				
				State++;
			}

			// TLI inhibit
			if (SIISIVBSep) {
				commandConnector.PlayCountSound(false);

				TLIInhibit();
			}				
			break;

		case 109:
			if (MissionTime >= NextMissionEventTime) { 
				
				//
				// IGNITION
				//

				LastMissionEventTime = NextMissionEventTime;
				NextMissionEventTime += 1.0;
				State++;
			}

			// TLI inhibit
			if (SIISIVBSep) {
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

				//
				// Burn until the orbit is about right or we're out of fuel.
				//

				if ((ap >= (prad + (SIVBApogee * 1000.0))) || (((lvCommandConnector.GetPropellantMass() * 100.0) / lvCommandConnector.GetMaxFuelMass()) <= 0.1)) {
					State = 201;
					SIVBBurn = false;
					TLIBurnDone = true;			
					ExternalGNC = false;
					lvCommandConnector.DeactivateS4RCS();
				}

				NextMissionEventTime = MissionTime + 0.25;
			}
			break;

		case 200:

			//
			// Wait for shutdown.
			//
			if ((!ExternalGNC && GNC.Get_tGO() < 1.2) || TLIBurnDone || lvCommandConnector.GetPropellantMass() < 0.001)	{
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
			if (TLIBurnDone) {
				MainLevel = 0;
			} else {
				MainLevel -= (simdt / 2.4);
				MainLevel = max(MainLevel, 0.05);
			}
			lvCommandConnector.SetJ2ThrustLevel(MainLevel);

			if (MainLevel <= 0.0) {
				SIVBStop();
				NextMissionEventTime = MissionTime + 10.0 - 1.0 / 1.2;
				State++;
			}
			break;

		case 202:
			if (MissionTime >= NextMissionEventTime) {
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
		switch (State) {

		case 0:
			lvCommandConnector.SetJ2ThrustLevel(0.0);
			lvCommandConnector.SetVentingThruster();
		
			if (Realism) lvCommandConnector.EnableDisableJ2(false);

			//
			// Engine is now dead.
			//
			State = 203; 
			break;
		}
	}

	// Guidance
	GNC.PreStep(mjd, simdt);

	// Attitude control
	VECTOR3 vel;
	OBJHANDLE hbody = lvCommandConnector.GetGravityRef();
	lvCommandConnector.GetRelativeVel(hbody, vel);
	if (ExternalGNC) { 
		if (State >= 101 && State <= 200) {			
			OrientAxis(Normalize(vel), 2, 0, (State == 200 ? 5 : 1), _V(0, 0, 0));
		} else if (State >= 201 && State <= 202) {
			lvCommandConnector.SetAttitudeRotLevel(_V(0, 0, 0));
		} 
	} else {
		if (State >= 101 && State < 200) {
			OrientAxis(GNC.Get_uTD(), 2, 0, 1, _V(0, 0, 0));
		} else if (State == 200) {
			if (GNC.Get_tGO() > 5) {
				OrientAxis(GNC.Get_uTD(), 2, 0, 1, _V(0, 0, 0));
			} else {
				lvCommandConnector.SetAttitudeRotLevel(_V(0, 0, 0));
			}
		} else if (State >= 201 && State <= 202) {			
			lvCommandConnector.SetAttitudeRotLevel(_V(0, 0, 0));
		} 
	}
	//sprintf(oapiDebugString(), "TLIBurnState %d State %d IgnMJD %.12f tGO %f vG x %f y %f z %f l %f Th %f", TLIBurnState, State, GNC.Get_IgnMJD(), GNC.Get_tGO(), GNC.Get_vG().x, GNC.Get_vG().y, GNC.Get_vG().z, length(GNC.Get_vG()), lvCommandConnector.GetJ2ThrustLevel()); 
}

void IU::PostStep(double simt, double simdt, double mjd) {

	GNC.PostStep(mjd, simdt);

	// Shutdown the engine in the same time step
	if (State == 200 && !ExternalGNC && !GNC.IsEngineOn()) {
		lvCommandConnector.SetJ2ThrustLevel(0);
		TLIBurnDone = true;
	}
}

void IU::TLIInhibit()

{
	lvCommandConnector.SetJ2ThrustLevel(0.0);
	lvCommandConnector.SetAttitudeRotLevel(_V(0, 0, 0));
	if (Realism) lvCommandConnector.EnableDisableJ2(false);

	TLIBurnStart = false;
	State = 100;
}

void IU::ChannelOutput(int address, int value)

{
	ChannelValue12 val12;

 	if (address == 012) {
    	val12.Value = value;
		if (val12.Bits.SIVBIgnitionSequenceStart) {
			ExternalGNC = true;
			SIVBStart();

			val12.Bits.SIVBIgnitionSequenceStart = false;			
			commandConnector.SetAGCOutputChannel(012, val12.Value);
		}

		if (val12.Bits.SIVBCutoff) {
			if (TLIBurnStart) { 
				TLIBurnDone = true;
			}
			ExternalGNC = false;
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
	commandConnector.TLIBegun();

	TLIBurnStart = true;
	return true;
}

void IU::SIVBStop()

{
	if (lvCommandConnector.GetStage() != STAGE_ORBIT_SIVB)
		return;

	lvCommandConnector.SetJ2ThrustLevel(0.0);
	lvCommandConnector.SetVentingThruster();

	if (Realism) lvCommandConnector.EnableDisableJ2(false);

	commandConnector.PlaySecoSound(true);
	commandConnector.ClearTLISounds();

	TLIBurnStart = false;
	TLIBurnDone = true;
}

void IU::SetLVLHAttitude(VECTOR3 v)
{
	AttitudeHold = false;
	if (State <= 100 || State >= 203) {
		OrientAxis(v, 2, 1, 1, _V(0, -1, 0));
	}
}

void IU::HoldAttitude()
{
	if (!AttitudeHold) {
		VECTOR3 zerogl, zgl, ygl;	
		// Store current attitude
		lvCommandConnector.Local2Global(_V(0.0, 0.0, 0.0), zerogl);
		lvCommandConnector.Local2Global(_V(0.0, 0.0, 1.0), zgl);
		lvCommandConnector.Local2Global(_V(0.0, 1.0, 0.0), ygl);
		AttitudeToHold = zgl - zerogl;
		AttitudeToHold2 = ygl - zerogl;
		AttitudeHold = true;
	}

	if (State <= 100 || State >= 203) {
		OrientAxis(AttitudeToHold, 2, 0, 1, AttitudeToHold2);
	}
}

bool IU::StartTLIBurn(VECTOR3 RIgn, VECTOR3 VIgn, VECTOR3 dV, double MJDIgn) 

{
	if (TLIBurnState == 1 || TLIBurnState == 2) 
		return GNC.ActivateP30(RIgn, VIgn, dV, MJDIgn);
	
	if (!TLICapable || TLIBurnStart || TLIBurnDone)
		return false;

	if (lvCommandConnector.GetStage() != STAGE_ORBIT_SIVB)
		return false;
		 
	if (TLIBurnState != 0) 
		return false;

	// Burn sequence begins 9:38 min before ignition
	if (((MJDIgn - oapiGetSimMJD()) * 24. * 3600.) <= (9.0 * 60.0 + 38.0))
		return false;

	if (!GNC.ActivateP30(RIgn, VIgn, dV, MJDIgn))
		return false;

	TLIBurnState = 1;
	return true;
}

VECTOR3 IU::OrientAxis(VECTOR3 &vec, int axis, int ref, double gainFactor, VECTOR3 &vec2)
{
	//axis=0=x, 1=y, 2=z
	//ref =0 body coordinates 1=local vertical
	//orients a vessel axis with a body-relative normalized vector
	//allows rotation about the axis being oriented for axis = x or y
	//for axis = z vec2 defines the roll angle w.r.t the y axis
	const double RATE_MAX = RAD*(0.5);
	const double DEADBAND_LOW = RAD*(0.01);
	const double RATE_FINE = RAD*(0.005);
	const double RATE_NULL = RAD*(0.0001);

	VECTOR3 PMI, Level, Drate, delatt, Rate, zerogl, xgl, ygl, zgl, norm, pos, vel, up,
		left, forward;
	double Mass, Size, MaxThrust, Thrust, Rdead, factor, xa, ya, za, rmax, denom;

	VESSELSTATUS status2;
	lvCommandConnector.GetStatus(status2);
	lvCommandConnector.GetPMI(PMI); 
	Mass = lvCommandConnector.GetMass();
	Size = lvCommandConnector.GetSize();
	rmax = RATE_MAX;
	denom = 3.0;

	/// \todo Docked CSM for Apollo to Venus

	MaxThrust = lvCommandConnector.GetMaxThrust(ENGINE_ATTITUDE);
	factor = gainFactor;
	
	lvCommandConnector.Local2Global(_V(0.0, 0.0, 0.0), zerogl);
	lvCommandConnector.Local2Global(_V(1.0, 0.0, 0.0), xgl);
	lvCommandConnector.Local2Global(_V(0.0, 1.0, 0.0), ygl);
	lvCommandConnector.Local2Global(_V(0.0, 0.0, 1.0), zgl);
	xgl = xgl - zerogl;
	ygl = ygl - zerogl;
	zgl = zgl - zerogl;
	norm = Normalize(vec);
	VECTOR3 norm2 = Normalize(vec2);
	if(ref == 1) {
		// vec is in local vertical reference, change to body rel coords
		// vec.x=forward component
		// vec.y=up component
		// vec.z=left component
		OBJHANDLE hbody = lvCommandConnector.GetGravityRef();
		lvCommandConnector.GetRelativePos(hbody, pos);
		lvCommandConnector.GetRelativeVel(hbody, vel);
		up = Normalize(pos);
		left = Normalize(CrossProduct(pos, vel));
		forward = Normalize(CrossProduct(left, up));
		norm = forward * vec.x + up * vec.y + left * vec.z;	
		norm2 = forward * vec2.x + up * vec2.y + left * vec2.z;	
	}
	if(axis == 0) {
		xa=norm*xgl;
		xa=xa/fabs(xa);
		ya=asin(norm*ygl);
		za=asin(norm*zgl);
		if(xa < 0.0) {
			ya=(ya/fabs(ya))*PI-ya;
			za=(za/fabs(za))*PI-za;
		}
		delatt.x=0.0;
		delatt.y=za;
		delatt.z=-ya;
	}
	if(axis == 1) {
		xa=asin(norm*xgl);
		ya=norm*ygl;
		ya=ya/fabs(ya);
		za=asin(norm*zgl);
		if(ya < 0.0) {
			xa=(xa/fabs(xa))*PI-xa;
			za=(za/fabs(za))*PI-za;
		}
		delatt.x=-za;
		delatt.y=0.0;
		delatt.z=xa;
//		fprintf(outstr, "delatt=%.3f %.3f %.3f xyz=%.3f %.3f %.3f\n", delatt*DEG, 
//			xa*DEG, ya*DEG, za*DEG);
	}
	if(axis == 2) {
		xa=asin(norm*xgl);
		ya=asin(norm*ygl);
		za=norm*zgl;
		za=za/fabs(za);
		if(za < 0.0) {
			xa=(xa/fabs(xa))*PI-xa;
			ya=(ya/fabs(ya))*PI-ya;
		}
		delatt.x=ya;
		delatt.y=-xa;

		xa=asin(norm2*xgl);
		ya=norm2*ygl;
		ya=ya/fabs(ya);
		if(ya < 0.0) {
			xa=(xa/fabs(xa))*PI-xa;
		}
		delatt.z=xa;

		// sprintf(oapiDebugString(), "delatt=%.3f %.3f %.3f", delatt.x*DEG, delatt.y*DEG, delatt.z*DEG);
	}

	// sprintf(oapiDebugString(), "norm=%.6f %.6f %.6f x=%.6f y=%.6f z=%.6f", 
	// 	norm.x, norm.y, norm.z, zgl.x, zgl.y, zgl.z);


//X axis
	if (fabs(delatt.x) < DEADBAND_LOW) {
		if(fabs(status2.vrot.x) < RATE_NULL) {
		// set level to zero
			Level.x=0;
		} else {
		// null the rate
			Thrust=-(Mass*PMI.x*status2.vrot.x)/Size;
			Level.x = min((Thrust/MaxThrust), 1);
		}
	} else {
		Rate.x=fabs(delatt.x)/denom;
		if(Rate.x < RATE_FINE) Rate.x=RATE_FINE;
		if (Rate.x > rmax ) Rate.x=rmax;
		Rdead=min(Rate.x/2,RATE_FINE);
		if(delatt.x < 0) {
			Rate.x=-Rate.x;
			Rdead=-Rdead;
		}
		Drate.x=Rate.x-status2.vrot.x;
		Thrust=factor*(Mass*PMI.x*Drate.x)/Size;
		if(delatt.x > 0) {
			if(Drate.x > Rdead) {
				Level.x=min((Thrust/MaxThrust),1);
			} else if (Drate.x < -Rdead) {
				Level.x=max((Thrust/MaxThrust),-1);
			} else {
				Level.x=0;
			}
		} else {
			if(Drate.x < Rdead) {
				Level.x=max((Thrust/MaxThrust),-1);
			} else if (Drate.x > -Rdead) {
				Level.x=min((Thrust/MaxThrust),1);
			} else {
				Level.x=0;
			}
		}
	}
//	fprintf(outstr, "del=%.5f rate=%.5f drate=%.5f lvl=%.5f vrot=%.5f\n",
//		delatt.x*DEG, Rate.x*DEG, Drate.x*DEG, Level.x, status2.vrot.x*DEG);

//	sprintf(oapiDebugString(), "del=%.5f rate=%.5f drate=%.5f lvl=%.5f pmi=%.5f th=%.3f vr=%.3f",
//		delatt.x*DEG, Rate.x*DEG, Drate.x*DEG, Level.x, PMI.x, Thrust, status2.vrot.x*DEG);

//Y-axis
	if (fabs(delatt.y) < DEADBAND_LOW) {
		if(fabs(status2.vrot.y) < RATE_NULL) {
		// set level to zero
			Level.y=0;
		} else {
		// null the rate
			Thrust=-(Mass*PMI.y*status2.vrot.y)/Size;
			Level.y = min((Thrust/MaxThrust), 1);
		}
//		sprintf(oapiDebugString(),"yrate=%.5f level%.5f", status2.vrot.y, Level.y);
	} else {
		Rate.y=fabs(delatt.y)/denom;
		if(Rate.y < RATE_FINE) Rate.y=RATE_FINE;
		if (Rate.y > rmax ) Rate.y=rmax;
		Rdead=min(Rate.y/2,RATE_FINE);
		if(delatt.y < 0) {
			Rate.y=-Rate.y;
			Rdead=-Rdead;
		}
		Drate.y=Rate.y-status2.vrot.y;
		Thrust=factor*(Mass*PMI.y*Drate.y)/Size;
		if(delatt.y > 0) {
			if(Drate.y > Rdead) {
				Level.y=min((Thrust/MaxThrust),1);
			} else if (Drate.y < -Rdead) {
				Level.y=max((Thrust/MaxThrust),-1);
			} else {
				Level.y=0;
			}
		} else {
			if(Drate.y < Rdead) {
				Level.y=max((Thrust/MaxThrust),-1);
			} else if (Drate.y > -Rdead) {
				Level.y=min((Thrust/MaxThrust),1);
			} else {
				Level.y=0;
			}
		}
	}

//Z axis
	if (fabs(delatt.z) < DEADBAND_LOW) {
		if(fabs(status2.vrot.z) < RATE_NULL) {
		// set level to zero
			Level.z=0;
		} else {
		// null the rate
			Thrust=-(Mass*PMI.z*status2.vrot.z)/Size;
			Level.z = min((Thrust/MaxThrust), 1);
		}
	} else {
		Rate.z=fabs(delatt.z)/denom;
		if(Rate.z < RATE_FINE) Rate.z=RATE_FINE;
		if (Rate.z > rmax ) Rate.z=rmax;
		Rdead=min(Rate.z/2,RATE_FINE);
		if(delatt.z< 0) {
			Rate.z=-Rate.z;
			Rdead=-Rdead;
		}
		Drate.z=Rate.z-status2.vrot.z;
		Thrust=factor*(Mass*PMI.z*Drate.z)/Size;
		if(delatt.z > 0) {
			if(Drate.z > Rdead) {
				Level.z=min((Thrust/MaxThrust),1);
			} else if (Drate.z < -Rdead) {
				Level.z=max((Thrust/MaxThrust),-1);
			} else {
				Level.z=0;
			}
		} else {
			if(Drate.z < Rdead) {
				Level.z=max((Thrust/MaxThrust),-1);
			} else if (Drate.z > -Rdead) {
				Level.z=min((Thrust/MaxThrust),1);
			} else {
				Level.z=0;
			}
		}
	}
	lvCommandConnector.SetAttitudeRotLevel(Level);	

	// sprintf(oapiDebugString(),"IU Level x %.10lf y %.10lf z %.10lf Rate x %lf y %lf z %lf", 
	//	Level.x, Level.y, Level.z, status2.vrot.x*DEG, status2.vrot.y*DEG, status2.vrot.z*DEG);

	return Level;
}

void IU::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, IU_START_STRING);

	oapiWriteScenario_int(scn, "STATE", State);
	oapiWriteScenario_int(scn, "TLIBURNSTATE", TLIBurnState);
	papiWriteScenario_bool(scn, "TLIBURNSTART", TLIBurnStart);
	papiWriteScenario_bool(scn, "TLIBURNDONE", TLIBurnDone);
	papiWriteScenario_double(scn, "NEXTMISSIONEVENTTIME", NextMissionEventTime);
	papiWriteScenario_double(scn, "LASTMISSIONEVENTTIME", LastMissionEventTime);
	papiWriteScenario_bool(scn, "EXTERNALGNC", ExternalGNC);
	papiWriteScenario_bool(scn, "ATTITUDEHOLD", AttitudeHold);
	papiWriteScenario_vec(scn, "ATTITUDETOHOLD", AttitudeToHold);
	papiWriteScenario_vec(scn, "ATTITUDETOHOLD2", AttitudeToHold2);
	GNC.SaveState(scn);

	oapiWriteLine(scn, IU_END_STRING);
}

void IU::LoadState(FILEHANDLE scn)

{
	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, IU_END_STRING, sizeof(IU_END_STRING)))
			return;

		if (papiReadScenario_int(line, "TLIBURNSTATE", TLIBurnState)); 
		else if (papiReadScenario_int(line, "STATE", State)); 
		else if (papiReadScenario_bool(line, "TLIBURNSTART", TLIBurnStart)); 
		else if (papiReadScenario_bool(line, "TLIBURNDONE", TLIBurnDone)); 
		else if (papiReadScenario_double(line, "NEXTMISSIONEVENTTIME", NextMissionEventTime)); 
		else if (papiReadScenario_double(line, "LASTMISSIONEVENTTIME", LastMissionEventTime)); 
		else if (papiReadScenario_bool(line, "EXTERNALGNC", ExternalGNC)); 
		else if (papiReadScenario_bool(line, "ATTITUDEHOLD", AttitudeHold)); 
		else if (papiReadScenario_vec(line, "ATTITUDETOHOLD", AttitudeToHold)); 
		else if (papiReadScenario_vec(line, "ATTITUDETOHOLD2", AttitudeToHold2)); 
		else if (!strnicmp(line, IUGNC_START_STRING, sizeof(IUGNC_START_STRING))) {
			GNC.LoadState(scn);
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
void IUToCSMCommandConnector::TLIBegun()
{
	ConnectorMessage cm;
	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_TLI_BEGUN;

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
			ourIU->StartTLIBurn(m.val1.vValue, m.val2.vValue, m.val3.vValue, m.val4.dValue);
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

void IUToLVCommandConnector::SetAttitudeRotLevel (VECTOR3 th)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_ATTITUDE_ROT_LEVEL;
	cm.val1.vValue = th;

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

double IUToLVCommandConnector::GetSize()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SIZE;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetMaxThrust(ENGINETYPE eng)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_MAXTHRUST;
	cm.val1.iValue = eng;

	if (SendMessage(cm))
	{
		return cm.val2.dValue;
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

void IUToLVCommandConnector::GetPMI(VECTOR3 &pmi)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_PMI;
	cm.val1.pValue = &pmi;

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

void IUToLVCommandConnector::Local2Global(VECTOR3 &local, VECTOR3 &global)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_LOCAL2GLOBAL;
	cm.val1.pValue = &local;
	cm.val2.pValue = &global;

	SendMessage(cm);
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

void IUToLVCommandConnector::GetGlobalVel(VECTOR3 &v)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_GLOBAL_VEL;
	cm.val1.pValue = &v;

	SendMessage(cm);
}

OBJHANDLE IUToLVCommandConnector::GetElements(ELEMENTS &el, double &mjd_ref)

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

bool IUToLVCommandConnector::GetWeightVector(VECTOR3 &w)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_WEIGHTVECTOR;
	cm.val1.pValue = &w;

	if (SendMessage(cm))
	{		
		return cm.val2.bValue; 
	}

	return false;
}

bool IUToLVCommandConnector::GetForceVector(VECTOR3 &f)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_FORCEVECTOR;
	cm.val1.pValue = &f;

	if (SendMessage(cm))
	{		
		return cm.val2.bValue; 
	}

	return false;
}

void IUToLVCommandConnector::GetRotationMatrix(MATRIX3 &rot)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_ROTATIONMATRIX;
	cm.val1.pValue = &rot;

	SendMessage(cm);
}

double IUToLVCommandConnector::GetPitch()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_PITCH;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetBank()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_BANK;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetSlipAngle()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SLIP_ANGLE;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}


//
//	S-IVB IU GNC
//

IUGNC::IUGNC()
{	
	Reset();
}

IUGNC::~IUGNC()
{	

}

// f = Thruster Force in newtons 
// i = ISP (m/s)
void IUGNC::SetThrusterForce(double f, double i, double tail)
{
	ISP = i;
	Thrust = f;
	TailOff = tail;	/// \todo Kinda time to go offset???
}

// Unit Thrust Direction Vector
VECTOR3 IUGNC::Get_uTD()
{
	return _uTD;
}

// Time To Go (Time to engine shutdown)
double IUGNC::Get_tGO()
{
	return tGO;
}

// Remaining Delta-v in P30 LVLH system
VECTOR3 IUGNC::Get_dV()
{
	return GlobalToP30_LVLH(_vG, _ri, _vi, Mass, Thrust);
}


void IUGNC::Reset()
{
	Thrust = 0;
	TailOff = 0;
	ISP = 0;

	// Planet configuration
	Ref = 0;
	RefHandle = NULL;
	RefMu = 0;
	Mass = 0;

	// ** VECTORS **
	_PIPA = _V(0,0,0);
	_uTD = _V(0,0,0);
	_vG = _V(0,0,0);
	_r1 = _V(0,0,0);
	_v1 = _V(0,0,0);
	_ri = _V(0,0,0);
	_vi = _V(0,0,0);
	_r2 = _V(0,0,0);
	_lastWeight = _V(0,0,0);
	_uTDInit = _V(0,0,0);

	//** FLAGS **
	ExtDV = false;
	ready = false;
	engine = false;

	// ** SCALARS **
	tGO = 0;
	tBurn = 0;
	IgnMJD = 0;
	TInMJD = 0;
	tD = 0;
	CutMJD = 0;
}

// Reference:
// R = 0  (Earth)
// R = 1  (Moon)
void IUGNC::Configure(IUToLVCommandConnector *lvc, int R)
{
	lvCommandConnector = lvc;
	Ref = R;	// Reference 0=Earth  1=Moon

	OBJHANDLE Earth = oapiGetGbodyByName("Earth");
	OBJHANDLE Moon  = oapiGetGbodyByName("Moon");

	if (Ref==0) RefHandle = Earth;
	else        RefHandle = Moon;
	
	RefMu = oapiGetMass(RefHandle) * GGRAV;
}

// P-30, External dV program
// IMJD = MJD Of Ignition
// _dv = Deltavelocity in Local Vertical
bool IUGNC::ActivateP30(VECTOR3 _rign, VECTOR3 _vign, VECTOR3 _dv, double IMJD)
{	
	if (RefHandle==NULL) return false;
	
	IgnMJD = IMJD;
	Mass  = GetMass();	
	_r1 = _ri = _rign;  // Ignition State Vectors
	_v1 = _vi = _vign;

	VECTOR3 _X = Normalize(crossp(crossp(_r1, _v1), _r1)); 
	VECTOR3 _Y = Normalize(crossp(_r1, _v1));
	VECTOR3 _Z = -Normalize(_r1);

	// In Plane Component
	VECTOR3 _vP	= _X * _dv.x + _Z * _dv.z;
	
	// Central Angle
	VECTOR3 _vC = _V(0, 0, 0);
	if (length(_vP) != 0) {
		double ca   = ( length(crossp(_r1, _v1))*length(_dv)*Mass ) / ( dotp(_r1, _r1) * Thrust );		
		_vC = ( Normalize(_vP) * cos(ca/2.0) - Normalize(crossp(_vP, _Y)) * sin(ca/2.0) ) * length(_vP); 
	}
	_vG  = _vC + _Y * _dv.y;

	_uTD = Normalize(_vG);
	_uTDInit = _vG;
	double vG = length(_vG);	
	tGO  = (1.0 - exp(-vG/ISP)) * Mass*ISP/Thrust - TailOff;

	tBurn = 0;
	CutMJD  = 0;
	ExtDV   = true;  // External dV mode (P-30)
	ready   = true;  // Go for Average G at T-30

	return true;
}

bool IUGNC::ActivateP31(VECTOR3 _rign, VECTOR3 _vign, VECTOR3 _lap, double IMJD, double TMJD)
{
	if (RefHandle == NULL) return false;

	IgnMJD = IMJD;
	TInMJD = TMJD;
	Mass  = GetMass();	

	tD = (TInMJD - IgnMJD)*86400.0;  // Transfer Time

	_r2 = _lap;	  // Lambert Aim Point

	_r1 = _ri = _rign;  // Ignition State Vectors
	_v1 = _vi = _vign;

	double   w = cos(10.0 * RAD);	// 10deg cone angle
	VECTOR3 _h = crossp(_r1, _v1);
	double   z = dotp(Normalize(_r1), Normalize(_r2));

	// Rotate Target Position in source plane
	if ( (z+w) < 0 )  _r2 = Normalize(_r2 - _h*dotp(_r2,_h)) * length(_r2);
	
	// Compute Transfer Orbit Normal
	VECTOR3 _n = crossp( _r1, _r2 );

	if (_n.y < 0) {				 // ATTENTION ! ! ! !   This will depend about the system being used	
		 if (Ref = 1) _n = -_n;  // Ref = 1 Lunar Orbit
	}
	else if (Ref = 0) _n = -_n;  // Ref = 0 Earth Orbit 

	// Set SIGN for lampert routine 
	double SG;

	if (dotp(_r2, crossp(_n, _r1))>0) SG = 1.0;
	else SG = -1.0;

	// Compute Lambert Transfer Solution 
	_vG  = Lambert(_r1, _r2, tD, RefMu, SG) - _v1;
	_uTD = Normalize(_vG);

	double vG = length(_vG);	
	tGO  = (1.0 - exp(-vG/ISP)) * Mass*ISP/Thrust - TailOff;

	CutMJD  = 0;
	ExtDV   = false; // External dV mode off
	ready   = true;  // Go for Ignition

	return true;
}

void IUGNC::PreStep(double sim_mjd, double dt)
{	
	if (ready == false)    return;
	if (sim_mjd < IgnMJD)  return;

	// Nothing for now
}

void IUGNC::PostStep(double sim_mjd, double dt)
{
	if (RefHandle==NULL) return;

	VECTOR3 w;
	MATRIX3 rot;
	lvCommandConnector->GetWeightVector(w);
	lvCommandConnector->GetRotationMatrix(rot);
	w = mul(rot, w) / GetMass();

	if (ready == false || sim_mjd < IgnMJD) {
		_lastWeight = w;
		return;
	}
	
	// Engine On/Off Control
	if (engine == false) EngineOn();
	tBurn += dt;

	// Simulation of PIPA (Pulsed Integrating Pendulous Accelerometers)
	// Acceleration calculation, see IMU
	VECTOR3 f;
	lvCommandConnector->GetForceVector(f);
	f = mul(rot, f) / GetMass();
	VECTOR3 dw1 = w - f;
	VECTOR3 dw2 = _lastWeight - f;
	_lastWeight = w;
	_PIPA = -(dw1 + dw2) / 2.0 * dt;

	lvCommandConnector->GetRelativePos(RefHandle, _r1);
	lvCommandConnector->GetRelativeVel(RefHandle, _v1);

	// P30 Code Section
	if (ExtDV) {
		VECTOR3 vnew = _vG - _PIPA;
		if (length(vnew) >= length(_vG) && tBurn > 1) {
			ready = false;
			EngineOff();
		}

		_vG -= _PIPA;
		_uTD = Normalize(_vG);
	}

	// P31 Code Section
	if (ExtDV==false) {	

		 double   w = cos(10.0 * RAD);
		 VECTOR3 _h = crossp(_r1, _v1);
		 double   z = dotp(Normalize(_r1), Normalize(_r2));

		// Rotate Target Position in source plane
		if ( (z+w) < 0 )  _r2 = Normalize(_r2 - _h*dotp(_r2,_h)) * length(_r2);
		
		// Compute Transfer Orbit Normal
		VECTOR3 _n = crossp( _r1, _r2 );

		if (_n.y < 0) {				 // ATTENTION ! ! ! !   This will depend about the system being used	
			 if (Ref = 1) _n = -_n;  // Ref = 1 Lunar Orbit
		}
		else if (Ref = 0) _n = -_n;  // Ref = 0 Earth Orbit 

		// Set SIGN for lampert routine 
		double SG;

		if (dotp(_r2, crossp(_n, _r1))>0) SG = 1.0;
		else SG = -1.0;

		// Compute Lambert Transfer Solution 
		_vG  = Lambert(_r1, _r2, tD, RefMu, SG) - _v1;
		_uTD = Normalize(_vG);
	}

	// tGO Calculation 
	if (CutMJD) tGO = (CutMJD-oapiGetSimMJD())*86400.0;
	else {
		double vG = length(_vG);
		Mass  = GetMass();		
		tGO  = (1.0 - exp(-vG/ISP)) * Mass*ISP/Thrust - TailOff;
		if (tGO < 2.0) 
			CutMJD = oapiGetSimMJD() + tGO/86400.0;
	}
}

// Engine On/Off functions
// Called at engine ignition
void IUGNC::EngineOn()
{
	engine=true;

}

// Called at engine cutoff
void IUGNC::EngineOff()
{
	engine=false;

}

// Subroutines
VECTOR3 IUGNC::create_vector(VECTOR3 normal,VECTOR3 zero,double angle)
{
	zero=Normalize(zero);
	normal=Normalize(normal);
	zero=Normalize(zero-(zero*dotp(zero,normal)));
	VECTOR3 per=Normalize(crossp(normal,zero));
	VECTOR3 vec=zero*cos(angle) + per*sin(angle);
	return vec;
}


VECTOR3 IUGNC::GlobalToLV(VECTOR3 _in, VECTOR3 _pos, VECTOR3 _vel)
{
	VECTOR3 _out;
	VECTOR3 _prod = crossp(_pos,_vel);
	VECTOR3 _horz = crossp(_prod,_pos);
	
	_out.x = dotp(Normalize(_horz), _in);
	_out.z = dotp(Normalize(_pos),  _in);
	_out.y = dotp(Normalize(_prod), _in);

	return _out;
}

VECTOR3 IUGNC::GlobalToP30_LVLH(VECTOR3 _in, VECTOR3 _r, VECTOR3 _v, double mass, double thrust)
{ 
	VECTOR3 _h  = crossp(_r, _v);			// Angular Momentum
	double   h  = length(_h);
	VECTOR3 _o  = _h * dotp(_h, _in)/(h*h);	// Out of plane velocity
	VECTOR3 _i  = _in - _o;					// Inplane Velocity
	double in   = length(_in);
	double ca   = (in * mass / thrust) * (h / dotp(_r, _r));  // Travelled Aprx. central angle during burn

	VECTOR3 _V  = _o + create_vector(_h, _i, ca/2.0) * length(_i);
	VECTOR3 _LV = GlobalToLV(_V, _r, _v);
	
	_LV.z = -_LV.z; // Invert Z-Axis.  Positive Towards Planet

	return _LV; 
}

double IUGNC::GetMass()
{

	double mass = lvCommandConnector->GetMass();

	/// \todo Docked CSM for Apollo to Venus
	/*
	int count = v->DockCount();

	DOCKHANDLE dock;

	if (count)
	for (int i=0;i<count;i++) {
		dock = v->GetDockHandle(i);
		if (dock) {
			OBJHANDLE vessel = v->GetDockStatus(dock); 
			if (vessel)	mass += oapiGetMass(vessel);
		}
	}
	*/
	return mass;
}

VECTOR3 IUGNC::Lambert(VECTOR3 init, VECTOR3 rad, double time, double mu, double dm, VECTOR3 *tv)
{
	int i;
	double y, x, dt;
	double th = acos(dotp(init, rad) / (length(rad) * length(init)));
	double r0 = length(init);
	double r1 = length(rad);

	double A = dm * sqrt(r0*r1*(1+cos(th)));

	if (th==0 || A==0) return _V(0,0,0);

	double C=0.5, S=1.0/6.0;
	double zu = 4.0*PI*PI;
	double zl = -4.0*PI*PI;
	double z  = (zl+zu)/2.0;
	

	// Main iteration
	
	for(i=0;i<32;i++) {

		y  = r0 + r1 + A*(z*S-1.0)/sqrt(C);
		
		if (y<0) zl = z;
		else {
			x  = sqrt(y/C);
			
			dt = (x*x*x*S + A*sqrt(y)) / sqrt(mu); 

			if ((fabs(dt-time)/time)<10e-6) break;		

			if (dt<=time) zl=z;
			else zu=z;
		}

		z = (zl+zu)/2.0;

		if (z>10e-5) {
			C=(1.0-cos(sqrt(z)))/z;
			S=(sqrt(z)-sin(sqrt(z)))/sqrt(z*z*z);
		}
		else if (z<-10e-5) {
			C=(1.0-cosh(sqrt(-z)))/z;
			S=(sinh(sqrt(-z))-sqrt(-z))/sqrt(-z*z*z);
		}
		else {
			C = 1/2.0 - z/24.0  + z*z/720.0  - z*z*z/40320.0;
			S = 1/6.0 - z/120.0 + z*z/5040.0 - z*z*z/362880.0;
		}
	}

	double f=1.0 - y/r0;
	double g=1.0/(A*sqrt(y/mu));

	VECTOR3 V = (rad - init*f)*g;

	f = 1.0 - y/r1;

	if (tv) *tv = (rad*f - init)*g;

	return V;
}

void IUGNC::SaveState(FILEHANDLE scn)
{
	oapiWriteLine(scn, IUGNC_START_STRING);

	papiWriteScenario_vec(scn, "PIPA", _PIPA);
	papiWriteScenario_vec(scn, "UTD", _uTD);
	papiWriteScenario_vec(scn, "VG", _vG);
	papiWriteScenario_vec(scn, "R1", _r1);
	papiWriteScenario_vec(scn, "V1", _v1);
	papiWriteScenario_vec(scn, "RI", _ri);
	papiWriteScenario_vec(scn, "VI", _vi);
	papiWriteScenario_vec(scn, "R2", _r2);
	papiWriteScenario_vec(scn, "LASTWEIGHT", _lastWeight);
	papiWriteScenario_vec(scn, "INITUTD", _uTDInit);

	papiWriteScenario_bool(scn, "EXTDV", ExtDV);
	papiWriteScenario_bool(scn, "READY", ready);
	papiWriteScenario_bool(scn, "ENGINE", engine);

	papiWriteScenario_double(scn, "TGO", tGO); 
	papiWriteScenario_double(scn, "TBURN", tBurn); 
	papiWriteScenario_double(scn, "IGNMJD", IgnMJD); 
	papiWriteScenario_double(scn, "TINMJD", TInMJD); 
	papiWriteScenario_double(scn, "TD", tD); 
	papiWriteScenario_double(scn, "CUTMJD", CutMJD); 

	oapiWriteLine(scn, IUGNC_END_STRING);
}

void IUGNC::LoadState(FILEHANDLE scn)
{
	char *line;
	float flt = 0;
	int i = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, IUGNC_END_STRING, sizeof(IUGNC_END_STRING)))
			return;

		papiReadScenario_vec(line, "PIPA", _PIPA);
		papiReadScenario_vec(line, "UTD", _uTD);
		papiReadScenario_vec(line, "VG", _vG);
		papiReadScenario_vec(line, "R1", _r1);
		papiReadScenario_vec(line, "V1", _v1);
		papiReadScenario_vec(line, "RI", _ri);
		papiReadScenario_vec(line, "VI", _vi);
		papiReadScenario_vec(line, "R2", _r2);
		papiReadScenario_vec(line, "LASTWEIGHT", _lastWeight);
		papiReadScenario_vec(line, "INITUTD", _uTDInit);

		papiReadScenario_bool(line, "EXTDV", ExtDV);
		papiReadScenario_bool(line, "READY", ready);
		papiReadScenario_bool(line, "ENGINE", engine);

		papiReadScenario_double(line, "TGO", tGO); 
		papiReadScenario_double(line, "TBURN", tBurn); 
		papiReadScenario_double(line, "IGNMJD", IgnMJD); 
		papiReadScenario_double(line, "TINMJD", TInMJD); 
		papiReadScenario_double(line, "TD", tD); 
		papiReadScenario_double(line, "CUTMJD", CutMJD); 
	}
}
