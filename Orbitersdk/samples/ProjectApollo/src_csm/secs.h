/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Sequential Events Controller simulation.

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

  **************************** Revision History ****************************/

#if !defined(_PA_SECS_H)
#define _PA_SECS_H

class Saturn;
class FloatBag;

///
/// This class simulates the Sequential Events Control System in the CM.
/// \ingroup InternalSystems
/// \brief SECS simulation.
///
class SECS { 

public:
	SECS();
	virtual ~SECS();

	void ControlVessel(Saturn *v);
	void Timestep(double simt, double simdt);

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

protected:
	bool IsLogicPoweredAndArmedA();
	bool IsLogicPoweredAndArmedB();
	
	int State;
	double NextMissionEventTime;
	double LastMissionEventTime;

	bool PyroBusAMotor;
	bool PyroBusBMotor;

	Saturn *Sat;
};

///
/// This class simulates the Earth Landing System in the CM.
/// \ingroup InternalSystems
/// \brief ELS simulation.
///
class ELS { 

public:
	ELS();
	virtual ~ELS();

	void ControlVessel(Saturn *v);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);
	double *GetDyeMarkerLevelRef() { return &DyeMarkerLevel; }


protected:
	double NewFloatBagSize(double size, ThreePosSwitch *sw, CircuitBrakerSwitch *cb, double simdt);

	int State;
	double NextMissionEventTime;
	double LastMissionEventTime;

	double FloatBag1Size;
	double FloatBag2Size;
	double FloatBag3Size;
	double DyeMarkerLevel;
	double DyeMarkerTime;

	Saturn *Sat;
	FloatBag *FloatBagVessel;
};

//
// Strings for state saving.
//

#define SECS_START_STRING		"SECS_BEGIN"
#define SECS_END_STRING			"SECS_END"

#define ELS_START_STRING		"ELS_BEGIN"
#define ELS_END_STRING			"ELS_END"

#endif // _PA_SECS_H
