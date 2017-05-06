/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  Docking probe

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
#include "powersource.h"
#include "dockingprobe.h"
#include "nasspdefs.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"

#include "saturn.h"
#include "papi.h"

//
// Added matrix math stuff... might need cleaning or moving or both...
//

inline MATRIX3 operator- (const MATRIX3 &a, const MATRIX3 &b)
{
	MATRIX3 c;
	c.m11 = a.m11-b.m11;c.m12 = a.m12-b.m12;c.m13 = a.m13-b.m13;
	c.m21 = a.m21-b.m21;c.m22 = a.m22-b.m22;c.m23 = a.m23-b.m23;
	c.m31 = a.m31-b.m31;c.m32 = a.m32-b.m32;c.m33 = a.m33-b.m33;
	return c;
}

inline MATRIX3 mul (const VECTOR3 &A, const VECTOR3 &B) // Outerproduct of two vectors is MATRIX
{
	MATRIX3 mat = {
		A.x*B.x, A.x*B.y, A.x*B.z,
		A.y*B.x, A.y*B.y, A.y*B.z,
		A.z*B.x, A.z*B.y, A.z*B.z
	};
	return mat;
}

inline MATRIX3 eyemat(double i)               //Basic Matrix to be multiplied by
{
	MATRIX3 mat = {
		i,0,0,
		0,i,0,
		0,0,i
	};
	return mat;
}
inline MATRIX3 eyemat() {return eyemat(1.0);}  //Identity Matrix

DockingProbe::DockingProbe(int port, Sound &capturesound, Sound &latchsound, Sound &extendsound, 
						   Sound &undocksound, Sound &dockfailedsound, PanelSDK &p) : 
	                       CaptureSound(capturesound), LatchSound(latchsound), ExtendSound(extendsound), 
						   UndockSound(undocksound), DockFailedSound(dockfailedsound), DCPower(0, p)
{
	Enabled = false;
	Status = DOCKINGPROBE_STATUS_RETRACTED;
	ExtendingRetracting = 0;
	Docked = false;
	FirstTimeStepDone = false;
	UndockNextTimestep = false;
	IgnoreNextDockEvent = 0;
	DockingMethod = MODIFIEDORBITER;
	ourPort = port;
	Dockproc = DOCKINGPROBE_PROC_UNDOCKED;
	Dockparam[0] = Dockparam[1] = Dockparam[2] = _V(0, 0, 0);
	RetractChargesUsed = 0;
}

DockingProbe::~DockingProbe()

{
	// Nothing for now.
}

void DockingProbe::Extend()

{
	if (!Enabled) return;

	if (Status != DOCKINGPROBE_STATUS_EXTENDED) {
		ExtendingRetracting = 1;
		if (Docked) {
			Dockproc = DOCKINGPROBE_PROC_UNDOCKED;
			OurVessel->Undock(ourPort);
			UndockSound.play();
		
		} else {
			ExtendSound.play(); 
		}
	}
}

bool DockingProbe::IsHardDocked()

{
	return (Docked && (Status == DOCKINGPROBE_STATUS_RETRACTED));
}

void DockingProbe::Retract()

{
	if (!Enabled) return;

	if (Status != DOCKINGPROBE_STATUS_RETRACTED) {
		ExtendingRetracting = -1;
		if (Docked) {
			LatchSound.play();
		} else {
			// Use the same sound for retracting until we have proper sounds 
			ExtendSound.play(); 
		}
	}	
}

void DockingProbe::DockEvent(int dock, OBJHANDLE connected) 

{
	if (!FirstTimeStepDone) return;
	
	if (IgnoreNextDockEvent > 0) {
		Docked = (connected != NULL);
		IgnoreNextDockEvent--;
		return;
	}

	///
	/// Advanced Orbiter Handling submitted by Artlav
	///		~More realistic capture and retract
	///

	if (connected == NULL) {
		Docked = false;
		Dockproc = DOCKINGPROBE_PROC_UNDOCKED;
		DOCKHANDLE dock = OurVessel->GetDockHandle(ourPort);
		OurVessel->SetDockParams(dock, Dockparam[0], Dockparam[1], Dockparam[2]);
	} else {
		Docked = true;
		DOCKHANDLE dock = OurVessel->GetDockHandle(ourPort);
		OurVessel->GetDockParams(dock, Dockparam[0], Dockparam[1], Dockparam[2]);
		if (!Enabled || Status != DOCKINGPROBE_STATUS_EXTENDED) {
			DockFailedSound.play(NOLOOP, 200);
			UndockNextTimestep = true;
		} else {
			Status = 0.9;
			CaptureSound.play();			
			Dockproc = DOCKINGPROBE_PROC_SOFTDOCKED;
		}
	}
}

void DockingProbe::TimeStep(double simt, double simdt)

{
	if (!FirstTimeStepDone) {
		DoFirstTimeStep();
		FirstTimeStepDone = true;
		return;
	}

	if (UndockNextTimestep) {
		UpdatePort(Dockparam[1] * 0.5, simdt);
		OurVessel->Undock(ourPort);
		UndockNextTimestep = false;
	}

	if (ExtendingRetracting > 0) {
		if (Status >= DOCKINGPROBE_STATUS_EXTENDED) {
			Status = DOCKINGPROBE_STATUS_EXTENDED;
			ExtendingRetracting = 0;
			Dockproc = DOCKINGPROBE_PROC_UNDOCKED;
			OurVessel->Undocking(ourPort);
			OurVessel->SetDockingProbeMesh();
		} else {
			Status += 0.33 * simdt;
		}
	} else if (ExtendingRetracting < 0) {
		if (Status <= DOCKINGPROBE_STATUS_RETRACTED) {
			Status = DOCKINGPROBE_STATUS_RETRACTED;
			ExtendingRetracting = 0;
			OurVessel->HaveHardDocked(ourPort);		
			OurVessel->SetDockingProbeMesh();
		} else {
			Status -= 0.33 * simdt;
		}	
	}

	if (Dockproc == DOCKINGPROBE_PROC_SOFTDOCKED) {
		UpdatePort(Dockparam[1] * 0.5, simdt);
		Dockproc = DOCKINGPROBE_PROC_HARDDOCKED;
	} else if (Dockproc == DOCKINGPROBE_PROC_HARDDOCKED) {
		if (Status > DOCKINGPROBE_STATUS_RETRACTED) {
			UpdatePort(Dockparam[1] * 0.5 * Status / 0.9, simdt);
		} else {
			UpdatePort(_V(0,0,0), simdt);
			Dockproc = DOCKINGPROBE_PROC_UNDOCKED;
		}
	}
	// sprintf(oapiDebugString(), "Docked %d Status %.3f Dockproc %d  ExtendingRetracting %d", (Docked ? 1 : 0), Status, Dockproc, ExtendingRetracting); 

	// Switching logic
	if (OurVessel->DockingProbeExtdRelSwitch.IsUp() && IsPowered()) {
		Extend();

	} else if (OurVessel->DockingProbeExtdRelSwitch.IsDown()) {
		if ((!OurVessel->DockingProbeRetractPrimSwitch.IsCenter() && OurVessel->DockProbeMnACircuitBraker.IsPowered() && OurVessel->PyroBusA.Voltage() > SP_MIN_DCVOLTAGE) ||
			(!OurVessel->DockingProbeRetractSecSwitch.IsCenter()  && OurVessel->DockProbeMnBCircuitBraker.IsPowered() && OurVessel->PyroBusB.Voltage() > SP_MIN_DCVOLTAGE)) {

			int ActiveCharges = 0;

			if (OurVessel->DockingProbeRetractPrimSwitch.IsUp()) ActiveCharges = ActiveCharges | DOCKINGPROBE_CHARGE_PRIM1;
			if (OurVessel->DockingProbeRetractPrimSwitch.IsDown()) ActiveCharges = ActiveCharges | DOCKINGPROBE_CHARGE_PRIM2;
			if (OurVessel->DockingProbeRetractSecSwitch.IsUp()) ActiveCharges = ActiveCharges | DOCKINGPROBE_CHARGE_SEC1;
			if (OurVessel->DockingProbeRetractSecSwitch.IsDown()) ActiveCharges = ActiveCharges | DOCKINGPROBE_CHARGE_SEC2;

			if ((ActiveCharges & RetractChargesUsed)!= ActiveCharges) Retract();

			RetractChargesUsed = RetractChargesUsed | ActiveCharges;

			// sprintf(oapiDebugString(), "Charge Used: P1%d P2%d S1%d S2%d", RetractChargesUsed & DOCKINGPROBE_CHARGE_PRIM1 , RetractChargesUsed & DOCKINGPROBE_CHARGE_PRIM2 , RetractChargesUsed & DOCKINGPROBE_CHARGE_SEC1 , RetractChargesUsed & DOCKINGPROBE_CHARGE_SEC2); 
		}
	}

	///
	/// Begin Advanced Docking Code
	///
	if (DockingMethod > ADVANCED){
		// Code that follows is largely lifted from Atlantis...
		// Goal is to handle close proximity docking between a probe and drogue

		VECTOR3 gdrgPos, gdrgDir, gprbPos, gprbDir, gvslPos, rvel, pos, dir, rot;
		OurVessel->Local2Global (Dockparam[0],gprbPos);  //converts probe location to global
		OurVessel->GlobalRot (Dockparam[1],gprbDir);     //rotates probe direction to global

		// Search the complete vessel list for a grappling candidate.
		// Not very scalable ...
		for (DWORD i = 0; i < oapiGetVesselCount(); i++) {
			OBJHANDLE hV = oapiGetVesselByIndex (i);
			if (hV == OurVessel->GetHandle()) continue; // we don't want to grapple ourselves ...
			oapiGetGlobalPos (hV, &gvslPos);
			if (dist (gvslPos, gprbPos) < oapiGetSize (hV)) { // in range
				VESSEL *v = oapiGetVesselInterface (hV);
				DWORD nAttach = v->AttachmentCount (true);
				for (DWORD j = 0; j < nAttach; j++) { // now scan all attachment points of the candidate
					ATTACHMENTHANDLE hAtt = v->GetAttachmentHandle (true, j);
					const char *id = v->GetAttachmentId (hAtt);
					if (strncmp (id, "PADROGUE", 8)) continue; // attachment point not compatible
					v->GetAttachmentParams (hAtt, pos, dir, rot);
					v->Local2Global (pos, gdrgPos);  // converts found drogue position to global
					v->GlobalRot (dir, gdrgDir);     // rotates found drogue direction to global
					if (dist (gdrgPos, gprbPos) < COLLISION_DETECT_RANGE && DockingMethod == ADVANCEDPHYSICS) { // found one less than a meter away!
						//  Detect if collision has happend, if so, t will return intersection point along the probe line X(t) = gprbPos + t * gprbDir
						double t = CollisionDetection(gprbPos, gprbDir, gdrgPos, gdrgDir);	
						//  Calculate time of penetration according to current velocity
						OurVessel->GetRelativeVel(hV, rvel);
						//  Determine resultant force

						//APPLY rforce to DockingProbe Vessel, and APPLY -rforce to Drogue Vessel
						return;
					} 
					if (dist(gdrgPos, gprbPos) < CAPTURE_DETECT_RANGE && DockingMethod > ADVANCED) {
						// If we're within capture range, set docking port to attachment so docking can take place
						// Originally, I would have used the Attachment features to soft dock and move the LM during retract
						// but Artlav's docking method does this better and uses the docking port itself.
						// Attachment is being used as a placeholder for the docking port and to identify its orientation.
						OurVessel->GetAttachmentParams(hattPROBE, pos, dir, rot);
						DOCKHANDLE dock = OurVessel->GetDockHandle(ourPort);
						OurVessel->SetDockParams(dock, pos, dir, rot);
					}
				}//for nAttach
			}//if inRange
		}//for nVessel
	}
}

double DockingProbe::CollisionDetection(VECTOR3 prbP, VECTOR3 prbD, VECTOR3 drgV, VECTOR3 drgA)
{
	//This Function will check to see if collision has occured and 
	//return a vector of resultant forces if collision has occured
	//If returns 0's, no collision occured.

	double t = 0.0;

	VECTOR3 Del;
	double c0,c1,c2;
	MATRIX3 M;

	M = mul(drgA, drgA) - eyemat(pow(cos(DOCKINGPROBE_DROGUE_ANGLE*PI/180),2));

	Del = prbP - drgV;

	c0 = dotp(mul(M,Del),Del);
	c1 = dotp(mul(M,prbD),Del);
	c2 = dotp(mul(M,prbD),prbD);
	
	if (c2 != 0){
		double delta = c1*c1 - c0*c2;
		if (delta < 0) {  // no real roots
			return 0.0;
		} else if (delta == 0) {  // tangent to cone
			return 0.0;
		} else { // two distinct real roots!
			double t1 = (-c1 + pow(delta,0.5))/c2;
			double t2 = (-c1 - pow(delta,0.5))/c2;
			if (dotp(drgA, ((prbP + prbD * t1) - drgV)) > 0) {  // Check for proper cone solution
				t = t1;
			} else {
				t = t2;
			}
		}
	}

	// TODO: make sure this is checking properly
	if (t <= 0 && t >= DOCKINGPROBE_PROBE_LENGTH) return 0.0;  // if intersection does not occur in actual probe, do nothing

	return t;
}

void DockingProbe::UpdatePort(VECTOR3 off,double simdt)
{
	VESSELSTATUS vs;	
	OurVessel->GetStatus(vs);

 	DOCKHANDLE dock = OurVessel->GetDockHandle(ourPort);
	OBJHANDLE v = OurVessel->GetDockStatus(dock);
	IgnoreNextDockEvent = 1;
	bool b = OurVessel->Undock(ourPort); 
	OurVessel->SetDockParams(dock, Dockparam[0] + off, Dockparam[1], Dockparam[2]);
	IgnoreNextDockEvent = 1;
	if (v != NULL) {
		OurVessel->Dock(v, ourPort, 0, 1);	///\todo Port of the docked vessel is assumed 0
	}
	OurVessel->DefSetState(&vs);
}  


void DockingProbe::SystemTimestep(double simdt) 

{
	if (ExtendingRetracting) {
		DCPower.DrawPower(100.0);	// The real power consumption is unknown yet
	}
}

void DockingProbe::DoFirstTimeStep() 

{
	Docked = false;

	DOCKHANDLE d = OurVessel->GetDockHandle(ourPort);
	if (d) {
		if (OurVessel->GetDockStatus(d) != NULL) {
			Docked = true;
		}
	}

	if (IsHardDocked())
	{
		OurVessel->HaveHardDocked(ourPort);
	}
	else
	{
		OurVessel->Undocking(ourPort);

		/// jasonims:
		///    Not sure this is where it goes, but seems logical
		/// If advanced docking method is to be used, must prevent docking until 
		/// it is correct.
		if (DockingMethod > ADVANCED) {
			// Set Attachment at same spot as DockParams
			DOCKHANDLE dock = OurVessel->GetDockHandle(ourPort);
			// Get current Docking Port parameters
			OurVessel->GetDockParams(dock, Dockparam[0], Dockparam[1], Dockparam[2]);
			// Set actual dockingport direction reversed, effectively cancelling possiblity for docking accidentally.
			OurVessel->SetDockParams(dock, Dockparam[0], -Dockparam[1], Dockparam[2]); 
			// Using docking parameters set up an Attachment Point at docking port location
			hattPROBE = OurVessel->CreateAttachment(false, Dockparam[0], Dockparam[1], Dockparam[2],"PAPROBE");
		}
	}
}

void DockingProbe::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, DOCKINGPROBE_START_STRING);
	oapiWriteScenario_int(scn, "ENABLED", Enabled);
	oapiWriteScenario_float(scn, "STATUS", Status);
	oapiWriteScenario_int(scn, "EXTENDINGRETRACTING", ExtendingRetracting);
	oapiWriteScenario_int(scn, "DOCKPROC", Dockproc);
	oapiWriteScenario_int(scn, "CHARGESUSED", RetractChargesUsed);
	papiWriteScenario_vec(scn, "DOCKPARAM0", Dockparam[0]);
	papiWriteScenario_vec(scn, "DOCKPARAM1", Dockparam[1]);
	papiWriteScenario_vec(scn, "DOCKPARAM2", Dockparam[2]);
	oapiWriteLine(scn, DOCKINGPROBE_END_STRING);
}


void DockingProbe::LoadState(FILEHANDLE scn)

{
	char *line;
	int i;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, DOCKINGPROBE_END_STRING, sizeof(DOCKINGPROBE_END_STRING)))
			return;
		else if (!strnicmp (line, "ENABLED", 7)) {
			sscanf (line+7, "%d", &i);
			Enabled = (i != 0);
		}
		else if (!strnicmp (line, "STATUS", 6)) {
			sscanf (line+6, "%lf", &Status);
		}
		else if (!strnicmp (line, "EXTENDINGRETRACTING", 19)) {
			sscanf (line+19, "%d", &ExtendingRetracting);
		}
		else if (papiReadScenario_int(line, "DOCKPROC", Dockproc));
		else if (papiReadScenario_int(line, "CHARGESUSED", RetractChargesUsed));
		else if (papiReadScenario_vec(line, "DOCKPARAM0", Dockparam[0]));
		else if (papiReadScenario_vec(line, "DOCKPARAM1", Dockparam[1]));
		else papiReadScenario_vec(line, "DOCKPARAM2", Dockparam[2]);
	}
}

