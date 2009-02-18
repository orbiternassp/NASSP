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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.7  2008/04/11 12:19:21  tschachim
  *	New SM and CM RCS.
  *	Improved abort handling.
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.6  2007/07/17 14:33:10  tschachim
  *	Added entry and post landing stuff.
  *	
  *	Revision 1.5  2006/08/11 21:16:22  movieman523
  *	Dummy ELS implementation.
  *	
  *	Revision 1.4  2006/08/11 18:44:56  movieman523
  *	Beginnings of SECS implementation.
  *	
  *	Revision 1.3  2006/07/01 23:49:13  movieman523
  *	Updated more documentation.
  *	
  *	Revision 1.2  2006/06/25 21:19:45  movieman523
  *	Lots of Doxygen updates.
  *	
  *	Revision 1.1  2006/01/14 18:57:49  movieman523
  *	First stages of pyro and SECS simulation.
  *	
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