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

///
/// This class simulates the Sequential Events Control System in the CM.
/// \ingroup InternalSystems
/// \brief SECS simulation.
///
class SECS : public e_object {

public:
	SECS();
	virtual ~SECS();

	void ControlVessel(Saturn *v);

	void Timestep(double simt, double simdt);
	bool IsPowered();

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

protected:
	int State;

	double NextMissionEventTime;
	double LastMissionEventTime;

	Saturn *OurVessel;
};

///
/// This class simulates the Earth Landing System in the CM.
/// \ingroup InternalSystems
/// \brief ELS simulation.
///
class ELS : public e_object {

public:
	ELS();
	virtual ~ELS();

	void ControlVessel(Saturn *v);

	void Timestep(double simt, double simdt);
	bool IsPowered();

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

protected:
	int State;

	double NextMissionEventTime;
	double LastMissionEventTime;

	Saturn *OurVessel;
};

//
// Strings for state saving.
//

#define SECS_START_STRING		"SECS_BEGIN"
#define SECS_END_STRING			"SECS_END"

#define ELS_START_STRING		"ELS_BEGIN"
#define ELS_END_STRING			"ELS_END"

#endif // _PA_SECS_H