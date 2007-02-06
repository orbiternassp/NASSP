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
  *	Revision 1.7  2006/07/28 02:06:57  movieman523
  *	Now need to hard dock to get the connectors connected.
  *	
  *	Revision 1.6  2006/05/19 13:48:28  tschachim
  *	Fixed a lot of devices and power consumptions.
  *	DirectO2 valve added.
  *	
  *	Revision 1.5  2006/05/06 06:00:35  jasonims
  *	No more venting our Astronauts into space...and no more LRV popping out of an Astronauts pocket....well sorta.
  *	
  *	Revision 1.4  2006/02/01 18:27:11  tschachim
  *	Pyros and secs logic necessary for retraction.
  *	Automatic retraction if REALISM 0.
  *	
  *	Revision 1.3  2006/01/14 20:58:16  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
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

class Saturn;

class DockingProbe {

public:
	DockingProbe(Sound &capturesound, Sound &latchsound, Sound &extendsound, 
		         Sound &undocksound, Sound &dockfailedsound, PanelSDK &p);
	virtual ~DockingProbe();

	double GetStatus() { return Status; }
	bool GetDocked() { return Docked; }
	bool IsHardDocked();
	bool IsExtended() { return (Status == DOCKINGPROBE_STATUS_EXTENDED); };
	void SetEnabled(bool e) { Enabled = e; }
	void Extend();
	void Retract();
	void SetIgnoreNextDockEvent() { IgnoreNextDockEvent = true; };
	void DockEvent(int dock, OBJHANDLE connected);
	void TimeStep(double simt, double simdt);
	void SystemTimestep(double simdt);
	void RegisterVessel(Saturn *v) { OurVessel = v; };
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	void WireTo(e_object *a, e_object *b) { DCPower.WireToBuses(a, b); };
	bool IsPowered() { return DCPower.Voltage() > SP_MIN_DCVOLTAGE; };
	void SetRealism(int r) { Realism = r; };

protected:
	void DoFirstTimeStep();

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
	bool IgnoreNextDockEvent;
	PowerMerge DCPower;
	int Realism;
};

//
// Strings for state saving.
//

#define DOCKINGPROBE_START_STRING "DOCKINGPROBE_BEGIN"
#define DOCKINGPROBE_END_STRING   "DOCKINGPROBE_END"

#endif
