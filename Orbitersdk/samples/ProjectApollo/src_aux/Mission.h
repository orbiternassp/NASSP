/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  Mission File Handling (Header)

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

#include <OrbiterAPI.h>
#include <string>

namespace mission
{
	class Mission
	{
	public:
		Mission(const std::string& strMission);
		virtual ~Mission();

		virtual bool LoadMission(const int iMission);
		virtual bool LoadMission(const std::string& strMission);

		virtual int GetSMJCVersion() const;
	protected:
		std::string strFileName;
		std::string strMissionName;

		//1 = Block I and pre Apollo 12, 2 = Apollo 12 and later
		int iSMJCVersion;

		void SetDefaultValues();
	};
}

DLLCLBK mission::Mission* paGetDefaultMission();
DLLCLBK mission::Mission* paGetMission(const std::string& filename);
void ClearMissionManagementMemory();
void InitMissionManagementMemory();