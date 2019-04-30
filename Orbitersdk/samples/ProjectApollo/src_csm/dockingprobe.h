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

#if !defined(_PA_DOCKINGPROBE_H)
#define _PA_DOCKINGPROBE_H

#define DOCKINGPROBE_STATUS_RETRACTED 0
#define DOCKINGPROBE_STATUS_EXTENDED  1

#define DOCKINGPROBE_PROC_UNDOCKED 0
#define DOCKINGPROBE_PROC_SOFTDOCKED 1
#define DOCKINGPROBE_PROC_HARDDOCKED 2

#define DOCKINGPROBE_CHARGE_PRIM1 2
#define DOCKINGPROBE_CHARGE_PRIM2 4
#define DOCKINGPROBE_CHARGE_SEC1 8
#define DOCKINGPROBE_CHARGE_SEC2 16

// Tolerances for detection for collision and capture
const double COLLISION_DETECT_RANGE = 1;
const double CAPTURE_DETECT_RANGE = 0.1;

// Mathematical/Physical description
const double DOCKINGPROBE_PROBE_LENGTH = 0.7; // NEED actual values for this!
const double DOCKINGPROBE_DROGUE_DEPTH = 0.5; // NEED actual values for this!
const double DOCKINGPROBE_DROGUE_ANGLE = 45.0;  // Angle of DROGUE cone NEED actual value in degrees

class Saturn;


///
/// This class simulates the docking probe attached to the CM.
/// \ingroup InternalSystems
/// \brief Docking probe simulation.
///
class DockingProbe {

public:
	DockingProbe(int port, Sound &capturesound, Sound &latchsound, Sound &extendsound, 
		         Sound &undocksound, Sound &dockfailedsound, PanelSDK &p);
	virtual ~DockingProbe();

	double GetStatus() { return Status; }
	bool IsDocked() { return Docked; }
	bool IsHardDocked();
	bool IsExtended() { return (Status == DOCKINGPROBE_STATUS_EXTENDED); };
	bool IsRetracted() { return (Status == DOCKINGPROBE_STATUS_RETRACTED); };
	void SetEnabled(bool e) { Enabled = e; }
	bool IsEnabled() { return Enabled; };
	void DockEvent(int dock, OBJHANDLE connected);
	void TimeStep(double simt, double simdt);
	void SystemTimestep(double simdt);
	void RegisterVessel(Saturn *v) { OurVessel = v; };
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	void WireTo(e_object *a, e_object *b) { DCPower.WireToBuses(a, b); };

protected:
	void DoFirstTimeStep();
	void Extend();
	void Retract();
	void UpdatePort(VECTOR3 off, double simdt);
	double CollisionDetection(VECTOR3 prbP, VECTOR3 prbD, VECTOR3 drgV, VECTOR3 drgA);
	bool IsPowered() { return DCPower.Voltage() > SP_MIN_DCVOLTAGE; };

	bool Enabled;
	double Status;
	int ExtendingRetracting;
	bool Docked;
	Sound &CaptureSound;
	Sound &LatchSound;
	Sound &ExtendSound;
	Sound &UndockSound;;
	Sound &DockFailedSound;
	Saturn *OurVessel;
	bool FirstTimeStepDone;
	bool UndockNextTimestep;

	///
	/// Number of docking events to ignore. This is required in special cases, such as LEM creation, and
	/// the state is not saved; the intention is that the count will only be set in the same timestep when
	/// the docking events occur, so it doesn't need to be maintained over a longer period.
	///
	int IgnoreNextDockEvent;
	PowerMerge DCPower;

	///
	/// Flag for different docking methods:
	///    0 - Use Standard Orbiter docking handling
	///    1 - Use Modified Orbiter by Artlav
	///    11 - Use Advanced Capture Method with no Physics by Swatch
	///    12 - Use Advanced Capture Method with Collision Physics by Swatch
	///
	enum enumDockMethod{
		STANDARDORBITER = 0,
		MODIFIEDORBITER = 1,
		ADVANCED = 10,         // Only should be used internally to check if advanced methods to be used
		ADVANCEDCAPTURE = 11,
		ADVANCEDPHYSICS = 12
	} DockingMethod;

	///
	/// Docking port this probe is connected to.
	///
	int ourPort;

	int Dockproc;
	VECTOR3 Dockparam[3];	 

	ATTACHMENTHANDLE hattPROBE;

	int RetractChargesUsed; //
};

//
// Strings for state saving.
//

#define DOCKINGPROBE_START_STRING "DOCKINGPROBE_BEGIN"
#define DOCKINGPROBE_END_STRING   "DOCKINGPROBE_END"

#endif

