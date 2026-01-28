/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2025

  CSM SIMBay instrumentation

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
#pragma once

#include "RF_calc.h"

class Saturn;

class SIMBay
{
public:
	SIMBay();
	void Init(Saturn* vessel);					// Initialization
	void DefineAnimations(UINT idx);
	void SystemTimestep(double simdt);   // TimeStep
	void MappingCameraLoadState(char* line);
	void MappingCameraSaveState(FILEHANDLE scn);
	void GammaBayLoadState(char* line);
	void GammaBaySaveState(FILEHANDLE scn);
	void MassSpectrometerLoadState(char* line);
	void MassSpectrometerSaveState(FILEHANDLE scn);

	double GetMappingCameraAnimState() const { return MappingCameraAnimState; }
	double GetGammaBayAnimState() const { return GammaBayAnimState; }
	double GetMassSpectrometerAnimState() const { return MassSpectrometerAnimState; }

	Saturn* sat;								// Ship we're installed in

	//CSM 112&114 Common Animations
	UINT MappingCameraCoverAnim;
	double MappingCameraCoverAnimState;
	UINT MappingCameraAnim;
	double MappingCameraAnimState;
	UINT PanoramicCameraAnim;
	double PanoramicCameraAnimState;

	//CSM 112 Specific Animations
	UINT GammaBayAnim;
	double GammaBayAnimState;
	UINT GammaBayJettAnim;
	double GammaBayJettAnimState;
	UINT MassSpectrometerAnim;
	double MassSpectrometerAnimState;
	UINT MassSpectrometerJettAnim;
	double MassSpectrometerJettAnimState;
	UINT SubSatCoverAnim;
	double SubSatCoverAnimState;
	UINT XRayCoverAnim;
	double XRayCoverAnimState;

	//CSM 114 Specific Animations
	UINT IRCoverAnim;
	double IRCoverAnimState;
	UINT UVCoverAnim;
	double UVCoverAnimState;
};