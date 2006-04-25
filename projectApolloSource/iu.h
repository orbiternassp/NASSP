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
  *	Revision 1.2  2006/02/21 12:01:47  tschachim
  *	Moved TLI sequence to the IU.
  *	
  **************************************************************************/

#if !defined(_PA_IU_H)
#define _PA_IU_H

class Saturn;
class SoundLib;
class XLunarSwitch;

class IU {

public:
	IU(SoundLib &s, CSMcomputer &cmc, GuardedToggleSwitch &siisivsepswitch, XLunarSwitch &tlienableswitch);
	virtual ~IU();
	void Timestep(double simt, double simdt);
	void ChannelOutput(int address, int value);
	void RegisterVessel(Saturn *v) { OurVessel = v; };
	void SetMissionInfo(bool tlicapable, bool crewed, int realism, THRUSTER_HANDLE *th, PROPELLANT_HANDLE *ph, THGROUP_HANDLE *thg, double sivbburnstart, double sivbapogee);
	bool IsTLICapable() { return TLICapable; };

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

protected:
	bool SIVBStart();
	void SIVBStop();
	void SIVBBoiloff();
	void TLIInhibit();

	bool TLIBurnStart;
	bool TLIBurnDone;
	int State;
	double NextMissionEventTime;
	double LastMissionEventTime;
	bool FirstTimeStepDone;

	// Not used for now 
	double TLIBurnTime;
	double TLIBurnDeltaV;

	//
	// Sounds.
	//

	Sound Scount;
	Sound STLI;
	Sound STLIStart;
	Sound SecoSound;
	Sound SepS;

	//
	// Saturn stuff
	//

	Saturn *OurVessel;
	SoundLib &soundlib;
	CSMcomputer &agc;
	GuardedToggleSwitch &SIISIVBSepSwitch;
	XLunarSwitch &TLIEnableSwitch;
	int Realism;
	bool Crewed;
	bool TLICapable;

	// TODO: This stuff should be encapsuled by a "SIVBEngine" class we have a reference to here
	// depending how the engines will be modelled when the new SPSDK arrives.
	THRUSTER_HANDLE *th_SIVB;
	PROPELLANT_HANDLE *ph_SIVB;
	THGROUP_HANDLE *thg_aps;

	//
	// SIVB burn info for unmanned flights.
	//

	bool SIVBBurn;
	double SIVBBurnStart;
	double SIVBApogee;
};

//
// Strings for state saving.
//

#define IU_START_STRING		"IU_BEGIN"
#define IU_END_STRING		"IU_END"

#endif
