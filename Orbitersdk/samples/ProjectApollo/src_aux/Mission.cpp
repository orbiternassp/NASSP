/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  Mission File Handling

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

#include "Mission.h"
#include "OrbiterAPI.h"
#include <map>

std::map<std::string, mission::Mission*> pa_mission_hashmap;

DLLCLBK mission::Mission* paGetDefaultMission()
{
	return paGetMission("");
}

DLLCLBK mission::Mission* paGetMission(const std::string& mission_name)
{
	if (pa_mission_hashmap.find(mission_name) != pa_mission_hashmap.end())
	{
		oapiWriteLog("(Mission File Management) Found loaded mission file.");
		return pa_mission_hashmap[mission_name];
	}
	else {
		oapiWriteLog("(Mission File Management) Load mission file into shared memory.");
		mission::Mission* new_mission = new mission::Mission(mission_name);
		pa_mission_hashmap.insert(std::make_pair(mission_name, new_mission));
		return new_mission;
	}
}

void InitMissionManagementMemory()
{
	pa_mission_hashmap.clear();
}

void ClearMissionManagementMemory()
{
	std::map<std::string, mission::Mission*>::iterator iter
		= pa_mission_hashmap.begin();
	while (iter != pa_mission_hashmap.end())
	{
		if (iter->second)
		{
			delete iter->second;
			iter->second = NULL;
		}
		iter++;
	}
}

namespace mission {

	Mission::Mission(const std::string& strMission)
	{
		SetDefaultValues();
		if (!strMission.empty())
		{
			if (!LoadMission(strMission))
			{
				oapiWriteLog("(Mission) failed creating mission from loaded file.");
			}
		}
	}

	Mission::~Mission()
	{
	}

	void Mission::SetDefaultValues()
	{
		iSMJCVersion = 1;
		bJMission = false;
		iPanel277Version = 0;
		iPanel278Version = 1;
		iLMDSKYVersion = 2;
		bHasLMProgramer = false;
		bHasAEA = true;
		bLMHasAscEngArmAssy = false;
		bLMHasLegs = true;
		bCSMHasHGA = true;
		bCSMHasVHFRanging = true;
		strCMCVersion = "Artemis072";
		strLGCVersion = "Luminary210";
		strAEAVersion = "FP8";
		bInvertLMStageBit = false;
		dATCA_PRM_Factor = 0.3;
		//LM-7 data from Operational Data Book
		LM_CG_Coefficients = _M(8.7719e-08, -7.9329e-04, 7.9773e+00, 2.1488e-10, -2.5485e-06, 1.2769e-02, 6.1788e-09, -6.9019e-05, 2.5186e-01);
		iCMtoLMPowerConnectionVersion = 0;
		EmptySMCG = _V(914.5916, -6.6712, 12.2940); //Includes: empty SM and SLA ring, but no SM RCS
		bHasRateAidedOptics = false;

		AddCSMCueCard(0, "CUECARD_DAP");
		AddCSMCueCard(2, "SPS_BURN");
		AddCSMCueCard(2, "SPS_BURN_CONTINUED");
		AddCSMCueCard(2, "T_AND_D");
		AddCSMCueCard(2, "T_AND_D_CONTINUED");
		AddCSMCueCard(2, "ENTRY");
		AddCSMCueCard(2, "PRE-ENTRY_ATTITUDE_TIMELINE");
		AddCSMCueCard(2, "CM_EVA");
		AddCSMCueCard(2, "CM_EVA_CONTD");
		AddCSMCueCard(2, "CONTINGENCY_EVA");
		AddCSMCueCard(2, "CONTINGENCY_EVA_CONTINUED");
		AddCSMCueCard(3, "CDR_BOOST-ABORTS");
		AddCSMCueCard(3, "TLI");
		AddCSMCueCard(4, "POWER_LOSS");
		AddCSMCueCard(4, "UNDOCK_SEP");
		AddCSMCueCard(4, "EMER_CAB_REPRESS");
		AddCSMCueCard(4, "TPF");
		AddCSMCueCard(4, "TPF_CONTINUED");
		AddCSMCueCard(4, "VAC_XFER_TO_ECS");
		AddCSMCueCard(5, "EPS-ECS_ABORTS");
		AddCSMCueCard(5, "SPS_BURN_RULES");
		AddCSMCueCard(6, "LANDING");
		AddCSMCueCard(7, "AC_PWR");
		AddCSMCueCard(7, "LOSS_OF_COMM");
		AddCSMCueCard(8, "LMP_BOOST-ABORTS");
		AddCSMCueCard(9, "LOI_LIMITS");
		AddCSMCueCard(10, "CSM_ANTENNA_LOCATIONS");
	}

	bool Mission::LoadMission(const int iMission)
	{
		return LoadMission("Apollo " + std::to_string(iMission));
	}

	bool Mission::LoadMission(const std::string& strMission)
	{
		char buffer[256];
		char buffer2[128];
		std::string filename;
		filename = "Missions\\ProjectApollo\\" + strMission + ".cfg";
		strFileName = strMission;

		sprintf_s(buffer, 255, "(Mission) Loading mission %s from file %s",
			strMission.c_str(), filename.c_str());
		oapiWriteLog(buffer);
		FILEHANDLE hFile = oapiOpenFile(filename.c_str(), FILE_IN);

		if (hFile == NULL)
		{
			oapiWriteLog("(Mission) ERROR: Can't open file.");
			return false;
		}

		VECTOR3 vtemp;

		if (oapiReadItem_string(hFile, "Name", buffer))
		{
			strMissionName = buffer;
		}
		oapiReadItem_int(hFile, "SMJCVersion", iSMJCVersion);
		oapiReadItem_bool(hFile, "JMission", bJMission);
		oapiReadItem_int(hFile, "Panel277Version", iPanel277Version);
		oapiReadItem_int(hFile, "Panel278Version", iPanel278Version);
		oapiReadItem_int(hFile, "LMDSKYVersion", iLMDSKYVersion);
		oapiReadItem_bool(hFile, "HasLMProgramer", bHasLMProgramer);
		oapiReadItem_bool(hFile, "HasAEA", bHasAEA);
		oapiReadItem_bool(hFile, "LMHasAscEngArmAssy", bLMHasAscEngArmAssy);
		oapiReadItem_bool(hFile, "LMHasLegs", bLMHasLegs);
		oapiReadItem_bool(hFile, "CSMHasHGA", bCSMHasHGA);
		oapiReadItem_bool(hFile, "CSMHasVHFRanging", bCSMHasVHFRanging);
		if (oapiReadItem_string(hFile, "CMCVersion", buffer))
		{
			strCMCVersion = buffer;
		}
		if (oapiReadItem_string(hFile, "LGCVersion", buffer))
		{
			strLGCVersion = buffer;
		}
		if (oapiReadItem_string(hFile, "AEAVersion", buffer))
		{
			strAEAVersion = buffer;
		}
		oapiReadItem_bool(hFile, "InvertLMStageBit", bInvertLMStageBit);
		oapiReadItem_float(hFile, "ATCA_PRM_Factor", dATCA_PRM_Factor);
		if (oapiReadItem_vec(hFile, "LM_CGX_Coefficients", vtemp))
		{
			LM_CG_Coefficients.m11 = vtemp.x; LM_CG_Coefficients.m12 = vtemp.y; LM_CG_Coefficients.m13 = vtemp.z;
		}
		if (oapiReadItem_vec(hFile, "LM_CGY_Coefficients", vtemp))
		{
			LM_CG_Coefficients.m21 = vtemp.x; LM_CG_Coefficients.m22 = vtemp.y; LM_CG_Coefficients.m23 = vtemp.z;
		}
		if (oapiReadItem_vec(hFile, "LM_CGZ_Coefficients", vtemp))
		{
			LM_CG_Coefficients.m31 = vtemp.x; LM_CG_Coefficients.m32 = vtemp.y; LM_CG_Coefficients.m33 = vtemp.z;
		}
		oapiReadItem_int(hFile, "CMtoLMPowerConnectionVersion", iCMtoLMPowerConnectionVersion);
		if (oapiReadItem_vec(hFile, "EmptySMCG", vtemp))
		{
			EmptySMCG = vtemp;
		}
		oapiReadItem_bool(hFile, "HasRateAidedOptics", bHasRateAidedOptics);
		for (int i = 0; i < 16; i++) //TBD: this is limiting how many cue cards you can load from the mission file
		{
			sprintf_s(buffer2, "CSMCueCard%d", i + 1);
			if (oapiReadItem_string(hFile, buffer2, buffer))
			{
				unsigned loc = 0;
				std::string meshname;
				VECTOR3 ofs = _V(0, 0, 0);

				sscanf(buffer, "%u %s %lf %lf %lf", &loc, buffer2, &ofs.x, &ofs.y, &ofs.z);
				meshname = buffer2;

				AddCSMCueCard(loc, meshname, ofs);
			}
		}
		oapiCloseFile(hFile, FILE_IN);
		return true;
	}

	int Mission::GetSMJCVersion() const
	{
		return iSMJCVersion;
	}

	bool Mission::IsJMission() const
	{
		return bJMission;
	}

	int Mission::GetPanel277Version() const
	{
		return iPanel277Version;
	}

	int Mission::GetPanel278Version() const
	{
		return iPanel278Version;
	}

	int Mission::GetLMDSKYVersion() const
	{
		return iLMDSKYVersion;
	}

	bool Mission::HasLMProgramer() const
	{
		return bHasLMProgramer;
	}

	bool Mission::HasAEA() const
	{
		return bHasAEA;
	}

	bool Mission::LMHasAscEngArmAssy() const
	{
		return bLMHasAscEngArmAssy;
	}

	bool Mission::LMHasLegs() const
	{
		return bLMHasLegs;
	}

	bool Mission::CSMHasHGA() const
	{
		return bCSMHasHGA;
	}

	bool Mission::CSMHasVHFRanging() const
	{
		return bCSMHasVHFRanging;
	}

	const std::string& Mission::GetCMCVersion() const
	{
		return strCMCVersion;
	}

	const std::string& Mission::GetLGCVersion() const
	{
		return strLGCVersion;
	}

	const std::string& Mission::GetAEAVersion() const
	{
		return strAEAVersion;
	}

	bool Mission::IsLMStageBitInverted() const
	{
		return bInvertLMStageBit;
	}

	double Mission::GetATCA_PRM_Factor() const
	{
		return dATCA_PRM_Factor;
	}

	MATRIX3 Mission::GetLMCGCoefficients() const
	{
		return LM_CG_Coefficients;
	}

	int Mission::GetCMtoLMPowerConnectionVersion() const
	{
		return iCMtoLMPowerConnectionVersion;
	}

	VECTOR3 Mission::GetCGOfEmptySM() const
	{
		return EmptySMCG;
	}

	bool Mission::HasRateAidedOptics() const
	{
		return bHasRateAidedOptics;
	}

	bool Mission::GetCSMCueCards(unsigned &counter, unsigned &loc, std::string &meshname, VECTOR3 &ofs)
	{
		return GetCueCards(CSMCueCards, counter, loc, meshname, ofs);
	}

	bool Mission::GetCueCards(const std::vector<CueCardConfig> &cue, unsigned &counter, unsigned &loc, std::string &meshname, VECTOR3 &ofs)
	{
		while (counter < CSMCueCards.size())
		{
			if (cue[counter].meshname != "" && cue[counter].meshname != "None")
			{
				loc = cue[counter].loc;
				meshname = cue[counter].meshname;
				ofs = cue[counter].ofs;
				counter++;
				return false;
			}

			counter++;
		}
		return true;
	}

	void Mission::AddCSMCueCard(unsigned location, std::string meshname, VECTOR3 ofs)
	{
		CueCardConfig cfg;

		cfg.loc = location;
		cfg.meshname = "ProjectApollo/CueCards/" + meshname;
		cfg.ofs = ofs;

		CSMCueCards.push_back(cfg);
	}
}