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
	struct CueCardConfig
	{
		unsigned loc = 0;
		std::string meshname;
		VECTOR3 ofs = _V(0, 0, 0);
	};

	struct GroundStationData
	{
		int Num;			 //0 = crate new station, >=1 = Overload existing station

		char Name[64];		 // Station name
		char Code[8];		 // Station ID code
		double Position[2];  // Latitude, Longitude
		bool Active;         // This entry is valid and active
		int TrackingCaps;	 // Tracking capabilities
		int USBCaps;        // Unified S-Band Capabilities
		int SBandAntenna;   // S-Band Antenna Type
		int TelemetryCaps;  // Telemetry Handling Capabilities
		int CommCaps;		 // Radio/Ground Communications Capabilities
		bool HasRadar;       // Has radar capability
		bool HasAcqAid;      // Has target acquisition aid
		int DownTlmCaps;    // Downtelemetry Capabilities
		int UpTlmCaps;      // Command Capabilities
		int StationType;    // Station Type
		int  StationPurpose; // Station Purpose
	};

	struct GroundStationPosition
	{
		int Num;			 //Station number
		double Position[2];  // Latitude, Longitude
	};

	struct GroundStationActive
	{
		int Num;			 //Station number
		bool Active;         // This entry is valid and active
	};

	class Mission
	{
	public:
		Mission(const std::string& strMission);
		virtual ~Mission();

		virtual bool LoadMission(const int iMission);
		virtual bool LoadMission(const std::string& strMission);

		const virtual std::string& GetMissionName(void) const;

		//1 = Block I and pre Apollo 13, 2 = Apollo 13 and later
		virtual int GetSMJCVersion() const;
		//false = any other CSM, true = J-type mission CSM (for all systems and panels common to CSM-112 to 114)
		virtual bool IsJMission() const;
		//0 = none, 1 = J-type mission, 2 = Skylab
		virtual int GetPanel277Version() const;
		//1 = pre Apollo 15, 2 = Apollo 15-16, 3 = Apollo 17, 4 = Skylab, 5 = ASTP
		virtual int GetPanel278Version() const;
		//1 = No lights in the bottom two rows, 2 = ALT + VEL lights, 3 = ALT, VEL, PRIO DISP, NO DAP lights
		virtual int GetLMDSKYVersion() const;
		//false = LM has no Programer, true = LM has programer
		virtual bool HasLMProgramer() const;
		//false = LM has no abort electronics assembly, true = LM has abort electronics assembly
		virtual bool HasAEA() const;
		//false = LM has no ascent engine arming assembly, = true = LM has ascent engine arming assembly
		virtual bool LMHasAscEngArmAssy() const;
		//false = LM has no legs, true = LM has legs
		virtual bool LMHasLegs() const;
		//false = LM has no deflectors, true = LM has deflectors
		virtual bool LMHasDeflectors() const;
		//false = CSM has no HGA, true = CSM has a HGA
		virtual bool CSMHasHGA() const;
		//false = CSM has no VHF Ranging System, true = CSM has VHF Ranging System
		virtual bool CSMHasVHFRanging() const;
		//Name of CMC software
		virtual const std::string& GetCMCVersion() const;
		//Name of LGC software
		virtual const std::string& GetLGCVersion() const;
		//Name of AEA software
		virtual const std::string& GetAEAVersion() const;
		//false = LM stage verify bit normal, true = inverted
		bool IsLMStageBitInverted() const;
		//Value of adjustable gain in pulse ratio modulator of the ATCA in the LM. 0.3 used for LM-4 and later, 0.1 for LM-3 and before
		double GetATCA_PRM_Factor() const;
		//Get matrix with coefficients for calculating the LM center of gravity as a quadratic function of mass
		MATRIX3 GetLMCGCoefficients() const;
		//CM to LM power connection version. 0 = connection doesn't work with LM staged, 1 = LM has a CB to bypass circuit to descent stage, 2 = circuit bypassed automatically at staging
		int GetCMtoLMPowerConnectionVersion() const;
		//Get CG of the empty SM (but including SM RCS) in inches
		VECTOR3 GetCGOfEmptySM() const;
		//false = Optics mode switch is not bypassed for CMC to optics commands, true = optics mode switch is bypassed for CMC to optics commands (ECP 792)
		bool HasRateAidedOptics() const;
		//
		MATRIX3 GetCM_IMU_Drift() const;
		//
		MATRIX3 GetLM_IMU_Drift() const;
		//
		VECTOR3 GetCM_PIPA_Bias() const;
		//
		VECTOR3 GetLM_PIPA_Bias() const;
		//
		VECTOR3 GetCM_PIPA_Scale() const;
		//
		VECTOR3 GetLM_PIPA_Scale() const;
		//false = Normal polarity (Apollo 14 and earlier), Lateral axis for PGNS and LR input has switched polarity (Apollo 15 and later)
		bool GetCrossPointerReversePolarity() const;
		//false = No shades (Apollo 15 and earlier), Shades (Apollo 16 & 17)
		bool GetCrossPointerShades() const;
		//Get time reference of AGC for CMC clock initialization. The value is usually the MJD of midnight July 1st that preceeds the launch
		double GetTEPHEM0() const;
		//Get LM number for mission specific EPS, ECS and CWEA versions. In the future this will also decide which systems config file will be loaded
		int GetLMNumber() const;
		//Get cue cards
		bool GetCSMCueCards(unsigned &counter, unsigned &loc, std::string &meshname, VECTOR3 &ofs);
		//Name of CDR
		virtual const std::string& GetCDRName() const;
		//Name of CMP
		virtual const std::string& GetCMPName() const;
		//Name of LMP
		virtual const std::string& GetLMPName() const;
		//Name of CDR to print on suit
		virtual const std::string& GetCDRSuitName() const;
		//Name of CMP to print on suit
		virtual const std::string& GetCMPSuitName() const;
		//Name of LMP to print on suit
		virtual const std::string& GetLMPSuitName() const;
		//false = LM event timer continues to count down through zero, true = when reaching zero it starts counting up
		virtual bool IsLMEventTimerReversingAtZero() const;
		//Get additional ground station data
		std::vector<GroundStationData> GetGroundStationData() const;
		//Overload ground station position
		std::vector<GroundStationPosition> GetGroundStationPosition() const;
		//Set ground station active/inactive
		std::vector<GroundStationActive> GetGroundStationActive() const;
		//Run special Apollo 13 failure and audio code
		virtual bool DoApollo13Failures() const;
	protected:
		bool GetCueCards(const std::vector<CueCardConfig> &cue, unsigned &counter, unsigned &loc, std::string &meshname, VECTOR3 &ofs);

		void AddCueCard(int vehicle, unsigned location, std::string meshname, VECTOR3 ofs);

		void AddCSMCueCard(unsigned location, std::string meshname, VECTOR3 ofs = _V(0, 0, 0));
		void AddLMCueCard(unsigned location, std::string meshname, VECTOR3 ofs = _V(0, 0, 0));

		void ReadCueCardLine(char *line, int vehicle);

		void UpdateTEPHEM0();

		void ReadGroundStationLine(char *line);
		void ReadGroundStationPostionLine(char *line);
		void ReadGroundStationActiveLine(char *line);

		std::string strFileName;
		std::string strMissionName;
		std::string strCMCVersion;
		std::string strLGCVersion;
		std::string strAEAVersion;

		std::string strCDRName;
		std::string strCMPName;
		std::string strLMPName;
		std::string strCDRSuitName;
		std::string strCMPSuitName;
		std::string strLMPSuitName;

		int iSMJCVersion;
		bool bJMission;
		int iPanel277Version;
		int iPanel278Version;
		int iLMDSKYVersion;
		bool bHasLMProgramer;
		bool bHasAEA;
		bool bLMHasAscEngArmAssy;
		bool bLMHasLegs;
		bool bLMHasDeflectors;
		bool bCSMHasHGA;
		bool bCSMHasVHFRanging;
		bool bInvertLMStageBit;
		double dATCA_PRM_Factor;
		MATRIX3 LM_CG_Coefficients;
		int iCMtoLMPowerConnectionVersion;
		VECTOR3 EmptySMCG;
		bool bHasRateAidedOptics;
		bool bCrossPointerReversePolarity;
		bool bCrossPointerShades;
		std::vector<CueCardConfig> CSMCueCards;
		std::vector<CueCardConfig> LMCueCards;
		double dTEPHEM0;
		int iLMNumber;
		bool bLMEventTimerReverseAtZero;
		std::vector<GroundStationData> AdditionalGroundStations;
		std::vector<GroundStationPosition> GroundStationsPositions;
		std::vector<GroundStationActive> GroundStationsActive;
		bool bApollo13Failures;

		MATRIX3 CM_IMUDriftRates;
		VECTOR3 CM_PIPABias;
		VECTOR3 CM_PIPAScale;

		MATRIX3 LM_IMUDriftRates;
		VECTOR3 LM_PIPABias;
		VECTOR3 LM_PIPAScale;

		void SetDefaultValues();
	};
}

DLLCLBK mission::Mission* paGetDefaultMission();
DLLCLBK mission::Mission* paGetMission(const std::string& filename);
void ClearMissionManagementMemory();
void InitMissionManagementMemory();
