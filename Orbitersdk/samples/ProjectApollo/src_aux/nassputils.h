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
			SIVB
		};

		bool IsVessel(VESSEL *v, ClassNames name)
		{
			switch (name)
			{
			case SaturnIB:
				if (!stricmp(v->GetClassName(), "ProjectApollo\\Saturn1b") || !stricmp(v->GetClassName(), "ProjectApollo/Saturn1b")) return true;
				return false;
			case SaturnV:
				if (!stricmp(v->GetClassName(), "ProjectApollo\\Saturn5") || !stricmp(v->GetClassName(), "ProjectApollo/Saturn5")) return true;
				return false;
			case LEM:
				if (!stricmp(v->GetClassName(), "ProjectApollo\\LEM") || !stricmp(v->GetClassName(), "ProjectApollo/LEM")) return true;
				return false;
			case MCC:
				if (!stricmp(v->GetClassName(), "ProjectApollo\\MCC") || !stricmp(v->GetClassName(), "ProjectApollo/MCC")) return true;
				return false;
			case SaturnIB_SIVB:
				if (!stricmp(v->GetClassName(), "ProjectApollo\\nsat1stg2") || !stricmp(v->GetClassName(), "ProjectApollo/nsat1stg2")) return true;
				return false;
			case SaturnV_SIVB:
				if (!stricmp(v->GetClassName(), "ProjectApollo\\sat5stg3") || !stricmp(v->GetClassName(), "ProjectApollo/sat5stg3")) return true;
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