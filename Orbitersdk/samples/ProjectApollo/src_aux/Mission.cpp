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
		bLMHasDeflectors = true;
		bCSMHasHGA = true;
		bCSMHasVHFRanging = true;
		strCMCVersion = "Artemis072";
		dTEPHEM0 = 41133.;
		strLGCVersion = "Luminary210";
		strAEAVersion = "FP8";
		bInvertLMStageBit = false;
		dATCA_PRM_Factor = 0.3;
		//LM-7 data from Operational Data Book
		LM_CG_Coefficients = _M(8.7719e-08, -7.9329e-04, 7.9773e+00, 2.1488e-10, -2.5485e-06, 1.2769e-02, 6.1788e-09, -6.9019e-05, 2.5186e-01);
		iCMtoLMPowerConnectionVersion = 0;
		EmptySMCG = _V(914.5916, -6.6712, 12.2940); //Includes: empty SM and SLA ring, but no SM RCS
		bHasRateAidedOptics = false;

		CM_IMUDriftRates = _M(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		CM_PIPABias = _V(0.0, 0.0, 0.0);
		CM_PIPAScale = _V(0.0, 0.0, 0.0);

		LM_IMUDriftRates = _M(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		LM_PIPABias = _V(0.0, 0.0, 0.0);;
		LM_PIPAScale = _V(0.0, 0.0, 0.0);
		bCrossPointerReversePolarity = false;
		bCrossPointerShades = false;
		iLMNumber = 5; //LM-5
		bLMEventTimerReverseAtZero = false;
		bApollo13Failures = false;
		strCDRName = "CDR";
		strCMPName = "CMP";
		strLMPName = "LMP";
		strCDRSuitName = "CDR";
		strCMPSuitName = "CMP";
		strLMPSuitName = "LMP";
		AddCSMCueCard(0, "CUECARD_DAP");
		AddCSMCueCard(2, "SPS_BURN");
		AddCSMCueCard(2, "SPS_BURN_CONTINUED");
		AddCSMCueCard(2, "T_AND_D");
		AddCSMCueCard(2, "T_AND_D_CONTINUED");
		AddCSMCueCard(2, "ENTRY");
		AddCSMCueCard(2, "CM_EVA");
		AddCSMCueCard(2, "CM_EVA_CONTD");
		AddCSMCueCard(2, "CONTINGENCY_EVA");
		AddCSMCueCard(2, "CONTINGENCY_EVA_CONTINUED");
		AddCSMCueCard(3, "CDR_BOOST-ABORTS");
		AddCSMCueCard(3, "TLI");
		AddCSMCueCard(3, "VAC_XFER_TO_ECS");
		AddCSMCueCard(4, "POWER_LOSS");
		AddCSMCueCard(4, "UNDOCK_SEP");
		AddCSMCueCard(4, "EMER_CAB_REPRESS");
		AddCSMCueCard(4, "TPF");
		AddCSMCueCard(4, "TPF_CONTINUED");
		AddCSMCueCard(4, "CDR_ABORTS_III_&_IV");
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
		std::string filename;
		filename = "Missions\\ProjectApollo\\" + strMission + ".cfg";
		strFileName = strMission;

		sprintf_s(buffer, 255, "(Mission) Loading mission %s from file %s",
			strMission.c_str(), filename.c_str());
		oapiWriteLog(buffer);

		std::ifstream hFile(filename.c_str());

		if (!hFile.is_open())
		{
			oapiWriteLog("(Mission) ERROR: Can't open file.");
			return false;
		}

		char line[256];

		AdditionalGroundStations.clear();
		GroundStationsPositions.clear();
		GroundStationsActive.clear();

		while (hFile.getline(line, sizeof line))
		{
			if (!_strnicmp(line, "Name=", 5)) {
				strncpy(buffer, line + 5, 255);
				strMissionName.assign(buffer);
			}
			else if (!_strnicmp(line, "SMJCVersion=", 12)) {
				sscanf(line + 12, "%d", &iSMJCVersion);
			}
			else if (!_strnicmp(line, "JMission=", 9)) {
				strncpy(buffer, line + 9, 255);
				bJMission = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "Panel277Version=", 16)) {
				sscanf(line + 16, "%d", &iPanel277Version);
			}
			else if (!_strnicmp(line, "Panel278Version=", 16)) {
				sscanf(line + 16, "%d", &iPanel278Version);
			}
			else if (!_strnicmp(line, "LMDSKYVersion=", 14)) {
				sscanf(line + 14, "%d", &iLMDSKYVersion);
			}
			else if (!_strnicmp(line, "HasLMProgramer=", 15)) {
				strncpy(buffer, line + 15, 255);
				bHasLMProgramer = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "HasAEA=", 7)) {
				strncpy(buffer, line + 7, 255);
				bHasAEA = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "LMHasAscEngArmAssy=", 19)) {
				strncpy(buffer, line + 19, 255);
				bLMHasAscEngArmAssy = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "LMHasLegs=", 10)) {
				strncpy(buffer, line + 10, 255);
				bLMHasLegs = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "LMHasDeflectors=", 16)) {
				strncpy(buffer, line + 16, 255);
				bLMHasDeflectors = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "CSMHasHGA=", 10)) {
				strncpy(buffer, line + 10, 255);
				bCSMHasHGA = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "CSMHasVHFRanging=", 17)) {
				strncpy(buffer, line + 17, 255);
				bCSMHasVHFRanging = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "CMCVersion=", 11)) {
				strncpy(buffer, line + 11, 255);
				strCMCVersion.assign(buffer);
				UpdateTEPHEM0();
			}
			else if (!_strnicmp(line, "LGCVersion=", 11)) {
				strncpy(buffer, line + 11, 255);
				strLGCVersion.assign(buffer);
			}
			else if (!_strnicmp(line, "AEAVersion=", 11)) {
				strncpy(buffer, line + 11, 255);
				strAEAVersion.assign(buffer);
			}
			else if (!_strnicmp(line, "InvertLMStageBit=", 17)) {
				strncpy(buffer, line + 17, 255);
				bInvertLMStageBit = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "ATCA_PRM_Factor=", 16)) {
				sscanf(line + 16, "%lf", &dATCA_PRM_Factor);
			}
			else if (!_strnicmp(line, "LM_CGX_Coefficients=", 20)) {
				sscanf(line + 20, "%lf %lf %lf", &LM_CG_Coefficients.m11, &LM_CG_Coefficients.m12, &LM_CG_Coefficients.m13);
			}
			else if (!_strnicmp(line, "LM_CGY_Coefficients=", 20)) {
				sscanf(line + 20, "%lf %lf %lf", &LM_CG_Coefficients.m21, &LM_CG_Coefficients.m22, &LM_CG_Coefficients.m23);
			}
			else if (!_strnicmp(line, "LM_CGZ_Coefficients=", 20)) {
				sscanf(line + 20, "%lf %lf %lf", &LM_CG_Coefficients.m31, &LM_CG_Coefficients.m32, &LM_CG_Coefficients.m33);
			}
			else if (!_strnicmp(line, "CMtoLMPowerConnectionVersion=", 29)) {
				sscanf(line + 29, "%d", &iCMtoLMPowerConnectionVersion);
			}
			else if (!_strnicmp(line, "EmptySMCG=", 10)) {
				sscanf(line + 10, "%lf %lf %lf", &EmptySMCG.x, &EmptySMCG.y, &EmptySMCG.z);
			}
			else if (!_strnicmp(line, "HasRateAidedOptics=", 19)) {
				strncpy(buffer, line + 19, 255);
				bHasRateAidedOptics = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "CrossPointerReversePolarity=", 28)) {
				strncpy(buffer, line + 28, 255);
				bCrossPointerReversePolarity = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "CrossPointerShades=", 19)) {
				strncpy(buffer, line + 19, 255);
				bCrossPointerShades = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "TEPHEM0=", 8)) {
				sscanf(line + 8, "%lf", &dTEPHEM0);
			}
			else if (!_strnicmp(line, "LMNumber=", 9)) {
				sscanf(line + 9, "%d", &iLMNumber);
			}
			else if (!_strnicmp(line, "LMEventTimerReverseAtZero=", 26)) {
				strncpy(buffer, line + 26, 255);
				bLMEventTimerReverseAtZero = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "Apollo13Failures=", 17)) {
				strncpy(buffer, line + 17, 255);
				bApollo13Failures = !_strnicmp(buffer, "TRUE", 4);
			}
			else if (!_strnicmp(line, "CDRVesselName=", 14)) {
				strncpy(buffer, line + 14, 255);
				strCDRName.assign(buffer);
			}
			else if (!_strnicmp(line, "CMPVesselName=", 14)) {
				strncpy(buffer, line + 14, 255);
				strCMPName.assign(buffer);
			}
			else if (!_strnicmp(line, "LMPVesselName=", 14)) {
				strncpy(buffer, line + 14, 255);
				strLMPName.assign(buffer);
			}
			else if (!_strnicmp(line, "CDRSuitName=", 12)) {
				strncpy(buffer, line + 12, 255);
				strCDRSuitName.assign(buffer);
			}
			else if (!_strnicmp(line, "CMPSuitName=", 12)) {
				strncpy(buffer, line + 12, 255);
				strCMPSuitName.assign(buffer);
			}
			else if (!_strnicmp(line, "LMPSuitName=", 12)) {
				strncpy(buffer, line + 12, 255);
				strLMPSuitName.assign(buffer);
			}
			else if (!_strnicmp(line, "CSMCueCard=", 11)) {
				ReadCueCardLine(line + 11, 0);
			}
			else if (!_strnicmp(line, "LMCueCard=", 10)) {
				ReadCueCardLine(line + 10, 1);
			}
			else if (!_strnicmp(line, "CMNBDX=", 7)) {
				sscanf(line + 7, "%lf", &CM_IMUDriftRates.m11);
			}
			else if (!_strnicmp(line, "CMNBDY=", 7)) {
				sscanf(line + 7, "%lf", &CM_IMUDriftRates.m12);
			}
			else if (!_strnicmp(line, "CMNBDZ=", 7)) {
				sscanf(line + 7, "%lf", &CM_IMUDriftRates.m13);
			}
			else if (!_strnicmp(line, "CMADSRAX=", 9)) {
				sscanf(line + 9, "%lf", &CM_IMUDriftRates.m21);
			}
			else if (!_strnicmp(line, "CMADSRAY=", 9)) {
				sscanf(line + 9, "%lf", &CM_IMUDriftRates.m22);
			}
			else if (!_strnicmp(line, "CMADSRAZ=", 9)) {
				sscanf(line + 9, "%lf", &CM_IMUDriftRates.m23);
			}
			else if (!_strnicmp(line, "CMADIAX=", 8)) {
				sscanf(line + 8, "%lf", &CM_IMUDriftRates.m31);
			}
			else if (!_strnicmp(line, "CMADIAY=", 8)) {
				sscanf(line + 8, "%lf", &CM_IMUDriftRates.m32);
			}
			else if (!_strnicmp(line, "CMADIAZ=", 8)) {
				sscanf(line + 8, "%lf", &CM_IMUDriftRates.m33);
			}
			else if (!_strnicmp(line, "CMPIPABIASX=", 12)) {
				sscanf(line + 12, "%lf", &CM_PIPABias.x);
			}
			else if (!_strnicmp(line, "CMPIPABIASY=", 12)) {
				sscanf(line + 12, "%lf", &CM_PIPABias.y);
			}
			else if (!_strnicmp(line, "CMPIPABIASZ=", 12)) {
				sscanf(line + 12, "%lf", &CM_PIPABias.z);
			}
			else if (!_strnicmp(line, "CMPIPASCALEX=", 13)) {
				sscanf(line + 13, "%lf", &CM_PIPAScale.x);
			}
			else if (!_strnicmp(line, "CMPIPASCALEY=", 13)) {
				sscanf(line + 13, "%lf", &CM_PIPAScale.y);
			}
			else if (!_strnicmp(line, "CMPIPASCALEZ=", 13)) {
				sscanf(line + 13, "%lf", &CM_PIPAScale.z);
			}
			else if (!_strnicmp(line, "LMNBDX=", 7)) {
				sscanf(line + 7, "%lf", &LM_IMUDriftRates.m11);
			}
			else if (!_strnicmp(line, "LMNBDY=", 7)) {
				sscanf(line + 7, "%lf", &LM_IMUDriftRates.m12);
			}
			else if (!_strnicmp(line, "LMNBDZ=", 7)) {
				sscanf(line + 7, "%lf", &LM_IMUDriftRates.m13);
			}
			else if (!_strnicmp(line, "LMADSRAX=", 9)) {
				sscanf(line + 9, "%lf", &LM_IMUDriftRates.m21);
			}
			else if (!_strnicmp(line, "LMADSRAY=", 9)) {
				sscanf(line + 9, "%lf", &LM_IMUDriftRates.m22);
			}
			else if (!_strnicmp(line, "LMADSRAZ=", 9)) {
				sscanf(line + 9, "%lf", &LM_IMUDriftRates.m23);
			}
			else if (!_strnicmp(line, "LMADIAX=", 8)) {
				sscanf(line + 8, "%lf", &LM_IMUDriftRates.m31);
			}
			else if (!_strnicmp(line, "LMADIAY=", 8)) {
				sscanf(line + 8, "%lf", &LM_IMUDriftRates.m32);
			}
			else if (!_strnicmp(line, "LMADIAZ=", 8)) {
				sscanf(line + 8, "%lf", &LM_IMUDriftRates.m33);
			}
			else if (!_strnicmp(line, "LMPIPABIASX=", 12)) {
				sscanf(line + 12, "%lf", &LM_PIPABias.x);
			}
			else if (!_strnicmp(line, "LMPIPABIASY=", 12)) {
				sscanf(line + 12, "%lf", &LM_PIPABias.y);
			}
			else if (!_strnicmp(line, "LMPIPABIASZ=", 12)) {
				sscanf(line + 12, "%lf", &LM_PIPABias.z);
			}
			else if (!_strnicmp(line, "LMPIPASCALEX=", 13)) {
				sscanf(line + 13, "%lf", &LM_PIPAScale.x);
			}
			else if (!_strnicmp(line, "LMPIPASCALEY=", 13)) {
				sscanf(line + 13, "%lf", &LM_PIPAScale.y);
			}
			else if (!_strnicmp(line, "LMPIPASCALEZ=", 13)) {
				sscanf(line + 13, "%lf", &LM_PIPAScale.z);
			}
			else if (!_strnicmp(line, "GroundStation=", 14)) {
				ReadGroundStationLine(line + 14);
			}
			else if (!_strnicmp(line, "GroundStationPosition=", 22)) {
				ReadGroundStationPostionLine(line + 22);
			}
			else if (!_strnicmp(line, "GroundStationActive=", 20)) {
				ReadGroundStationActiveLine(line + 20);
			}
		}
		hFile.close();

		return true;
	}

	MATRIX3 Mission::GetCM_IMU_Drift() const {
		return CM_IMUDriftRates;
	}

	MATRIX3 Mission::GetLM_IMU_Drift() const {
		return LM_IMUDriftRates;
	}
	
	VECTOR3 Mission::GetCM_PIPA_Bias() const {
		return CM_PIPABias;
	}
	
	VECTOR3 Mission::GetLM_PIPA_Bias() const {
		return LM_PIPABias;
	}
	
	VECTOR3 Mission::GetCM_PIPA_Scale() const {
		return CM_PIPAScale;
	}
	
	VECTOR3 Mission::GetLM_PIPA_Scale() const {
		return LM_PIPAScale;
	}

	const std::string& Mission::GetMissionName(void) const
	{
		return strFileName;
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

	bool Mission::LMHasDeflectors() const
	{
		return bLMHasDeflectors;
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

	bool Mission::GetCrossPointerReversePolarity() const
	{
		return bCrossPointerReversePolarity;
	}

	bool Mission::GetCrossPointerShades() const
	{
		return bCrossPointerShades;
	}

	double Mission::GetTEPHEM0() const
	{
		return dTEPHEM0;
	}

	int Mission::GetLMNumber() const
	{
		return iLMNumber;
	}

	void Mission::ReadCueCardLine(char *line, int vehicle)
	{
		char buffer[128];
		unsigned loc = 0;
		std::string meshname;
		VECTOR3 ofs = _V(0, 0, 0);

		if (sscanf(line, "%u %s %lf %lf %lf", &loc, buffer, &ofs.x, &ofs.y, &ofs.z) >= 2)
		{
			meshname = buffer;
			AddCueCard(vehicle, loc, meshname, ofs);
		}
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

	void Mission::AddCueCard(int vehicle, unsigned location, std::string meshname, VECTOR3 ofs)
	{
		CueCardConfig cfg;

		cfg.loc = location;
		cfg.meshname = "ProjectApollo/CueCards/" + meshname;
		cfg.ofs = ofs;

		if (vehicle == 0)
		{
			CSMCueCards.push_back(cfg);
		}
		else
		{
			LMCueCards.push_back(cfg);
		}
	}

	void Mission::AddCSMCueCard(unsigned location, std::string meshname, VECTOR3 ofs)
	{
		AddCueCard(0, location, meshname, ofs);
	}

	void Mission::AddLMCueCard(unsigned location, std::string meshname, VECTOR3 ofs)
	{
		AddCueCard(1, location, meshname, ofs);
	}

	void Mission::UpdateTEPHEM0()
	{
		if (strCMCVersion == "Colossus237" || strCMCVersion == "Colossus249" || strCMCVersion == "Manche45R2")
		{
			dTEPHEM0 = 40038.;
		}
		else if (strCMCVersion == "Comanche055" || strCMCVersion == "Comanche067" || strCMCVersion == "Manche72R3")
		{
			dTEPHEM0 = 40403.;
		}
		else if (strCMCVersion == "Artemis072NBY71")
		{
			dTEPHEM0 = 40768.;
		}
		else if (strCMCVersion == "Artemis072")
		{
			dTEPHEM0 = 41133.;
		}
	}

	const std::string& Mission::GetCDRName() const
	{
		return strCDRName;
	}

	const std::string& Mission::GetCMPName() const
	{
		return strCMPName;
	}

	const std::string& Mission::GetLMPName() const
	{
		return strLMPName;
	}

	const std::string& Mission::GetCDRSuitName() const
	{
		return strCDRSuitName;
	}

	const std::string& Mission::GetCMPSuitName() const
	{
		return strCMPSuitName;
	}

	const std::string& Mission::GetLMPSuitName() const
	{
		return strLMPSuitName;
	}

	bool Mission::IsLMEventTimerReversingAtZero() const
	{
		return bLMEventTimerReverseAtZero;
	}

	std::vector<GroundStationData> Mission::GetGroundStationData() const
	{
		return AdditionalGroundStations;
	}

	std::vector<GroundStationPosition> Mission::GetGroundStationPosition() const
	{
		return GroundStationsPositions;
	}

	std::vector<GroundStationActive> Mission::GetGroundStationActive() const
	{
		return GroundStationsActive;
	}

	void Mission::ReadGroundStationLine(char *line)
	{
		GroundStationData temp;

		int itemp[3];

		if (sscanf(line, "%d %s %s %lf %lf %d %d %d %d %d %d %d %d %d %d %d %d",
			&temp.Num, temp.Name, temp.Code, &temp.Position[0], &temp.Position[1], &itemp[0], &temp.TrackingCaps, &temp.USBCaps, &temp.SBandAntenna, &temp.TelemetryCaps,
			&temp.CommCaps, &itemp[1], &itemp[2], &temp.DownTlmCaps, &temp.UpTlmCaps, &temp.StationType, &temp.StationPurpose) == 17)
		{
			temp.Active = (itemp[0] != 0);
			temp.HasRadar = (itemp[1] != 0);
			temp.HasAcqAid = (itemp[2] != 0);

			AdditionalGroundStations.push_back(temp);
		}
	}

	void Mission::ReadGroundStationPostionLine(char *line)
	{
		GroundStationPosition temp;

		if (sscanf(line, "%d %lf %lf", &temp.Num, &temp.Position[0], &temp.Position[1]) == 3)
		{
			GroundStationsPositions.push_back(temp);
		}
	}

	void Mission::ReadGroundStationActiveLine(char *line)
	{
		GroundStationActive temp;

		int itemp;

		if (sscanf(line, "%d %d", &temp.Num, &itemp) == 2)
		{
			temp.Active = (itemp != 0);

			GroundStationsActive.push_back(temp);
		}
	}

	bool Mission::DoApollo13Failures() const
	{
		return bApollo13Failures;
	}
}