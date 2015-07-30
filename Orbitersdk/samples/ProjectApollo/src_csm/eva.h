/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  EVA.h - astronaut code

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

///
/// \ingroup AstronautSettings
/// \brief EVA settings.
///
typedef struct {

	int MissionNo;			///< Apollo mission number.
	int Realism;			///< Realism level.

} EVASettings;

///
/// \ingroup Astronauts
/// \brief Orbital EVA astronaut.
///
class EVA: public VESSEL2
{
public:
	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	EVA (OBJHANDLE hObj, int fmodel);
	virtual ~EVA();

	///
	/// \brief Initialise state.
	///
	void init();

	///
	/// \brief Orbiter timestep function.
	/// \param SimT Current simulation time, in seconds since Orbiter was started.
	/// \param SimDT Time in seconds since last timestep.
	/// \param mjd Current MJD.
	///
	void clbkPreStep (double SimT, double SimDT, double mjd);

	///
	/// \brief Orbiter keyboard input function.
	/// \param kstate Key state.
	///
	int clbkConsumeDirectKey(char *kstate);

	///
	/// \brief Orbiter class configuration function.
	/// \param cfg File to load configuration defaults from.
	///
	void clbkSetClassCaps (FILEHANDLE cfg);

	///
	/// \brief Orbiter state saving function.
	/// \param scn Scenario file to save to.
	///
	void clbkSaveState (FILEHANDLE scn);

	///
	/// \brief Orbiter state loading function.
	/// \param scn Scenario file to load from.
	/// \param status Pointer to current vessel status.
	///
	void clbkLoadStateEx (FILEHANDLE scn, void *status);

protected:

	bool GoDock1;

};

