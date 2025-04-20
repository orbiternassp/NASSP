/***************************************************************************
  This file is part of Project Apollo - NASSP

  NASSP utility functions

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

#include "Orbitersdk.h"

#define NUM_ELEMENTS(x) (sizeof((x))/sizeof((x)[0]))

#ifdef _OPENORBITER
#define MAT_EMISSION MatProp::Emission
#define MAT_LIGHT MatProp::Light
#else
#define MAT_EMISSION MESHM_EMISSION2
#define MAT_LIGHT MESHM_EMISSION
#endif


namespace nassp
{
	namespace utils
	{
		enum ClassNames
		{
			SaturnIB,
			SaturnV,
			Saturn,
			LEM,
			MCC,
			SaturnIB_SIVB,
			SaturnV_SIVB,
			SIVB,
			ML,
			Crawler,
			MainChute,
			DrogueChute
		};

		static inline bool IsVessel(VESSEL *v, ClassNames name)
		{
			const char *classname = v->GetClassName();
			switch (name)
			{
			case SaturnIB:
				if (!_stricmp(classname, "ProjectApollo\\Saturn1b") || !_stricmp(classname, "ProjectApollo/Saturn1b")) return true;
				return false;
			case SaturnV:
				if (!_stricmp(classname, "ProjectApollo\\Saturn5") || !_stricmp(classname, "ProjectApollo/Saturn5")) return true;
				return false;
			case LEM:
				if (!_stricmp(classname, "ProjectApollo\\LEM") || !_stricmp(classname, "ProjectApollo/LEM")) return true;
				return false;
			case MCC:
				if (!_stricmp(classname, "ProjectApollo\\MCC") || !_stricmp(classname, "ProjectApollo/MCC")) return true;
				return false;
			case SaturnIB_SIVB:
				if (!_stricmp(classname, "ProjectApollo\\nsat1stg2") || !_stricmp(classname, "ProjectApollo/nsat1stg2")) return true;
				return false;
			case SaturnV_SIVB:
				if (!_stricmp(classname, "ProjectApollo\\sat5stg3") || !_stricmp(classname, "ProjectApollo/sat5stg3")) return true;
				return false;
			case ML:
				if (!_stricmp(classname, "ProjectApollo\\ML") || !_stricmp(classname, "ProjectApollo/ML")) return true;
				return false;
			case Crawler:
				if (!_stricmp(classname, "ProjectApollo\\Crawler") || !_stricmp(classname, "ProjectApollo/Crawler")) return true;
				return false;
			case MainChute:
				if (!_stricmp(classname, "ProjectApollo\\MainChute") || !_stricmp(classname, "ProjectApollo/MainChute")) return true;
				return false;
			case DrogueChute:
				if (!_stricmp(classname, "ProjectApollo\\DrogueChute") || !_stricmp(classname, "ProjectApollo/DrogueChute")) return true;
				return false;
			case Saturn:
				return (IsVessel(v, SaturnIB) || IsVessel(v, SaturnV));
			case SIVB:
				return (IsVessel(v, SaturnIB_SIVB) || IsVessel(v, SaturnV_SIVB));
			}
			return false;
		}
	}
}
