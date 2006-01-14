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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.2  2006/01/09 17:55:26  tschachim
  *	Connected the dockingprobe to the EPS.
  *	
  *	Revision 1.1  2006/01/05 12:08:15  tschachim
  *	Initial version
  *	
  **************************************************************************/

#if !defined(_PA_DOCKINGPROBE_H)
#define _PA_DOCKINGPROBE_H

#define DOCKINGPROBE_STATUS_RETRACTED 0
#define DOCKINGPROBE_STATUS_EXTENDED  1

class DockingProbe {

public:
	DockingProbe(Sound &capturesound, Sound &latchsound, Sound &extendsound, Sound &undocksound, Sound &dockfailedsound, PanelSDK &p);
	virtual ~DockingProbe();

	double GetStatus() { return Status; }
	bool GetDocked() { return Docked; }
	void SetEnabled(bool e) { Enabled = e; }
	void Extend();
	void Retract();
	void SetIgnoreNextDockEvent() { IgnoreNextDockEvent = true; };
	void DockEvent(int dock, OBJHANDLE connected);
	void TimeStep(double simt, double simdt);
	void RegisterVessel(VESSEL *v) { OurVessel = v; };
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	void WireTo(e_object *a, e_object *b) { DCPower.WireToBuses(a, b); };
	bool IsPowered() { return DCPower.Voltage() > 20.0; };

protected:

	bool Enabled;
	double Status;
	int ExtendingRetracting;
	bool Docked;
	Sound &CaptureSound;
	Sound &LatchSound;
	Sound &ExtendSound;
	Sound &UndockSound;;
	Sound &DockFailedSound;
	VESSEL *OurVessel;
	bool FirstTimeStepDone;
	bool UndockNextTimestep;
	bool IgnoreNextDockEvent;
	PowerMerge DCPower;

	void DoFirstTimeStep();
};

//
// Strings for state saving.
//

#define DOCKINGPROBE_START_STRING "DOCKINGPROBE_BEGIN"
#define DOCKINGPROBE_END_STRING   "DOCKINGPROBE_END"

#endif
