#include "ARCore.h"

#include "soundlib.h"
#include "apolloguidance.h"
#include "CSMcomputer.h"
#include "IMU.h"
#include "saturn.h"
#include "saturnv.h"
#include "iu.h"
#include "LVDC.h"
#include "LEM.h"
#include "sivb.h"
#include "mccvessel.h"
#include "mcc.h"
#include "TLMCC.h"
#include "ApolloGeneralizedOpticsProgram.h"
#include "rtcc.h"
#include "nassputils.h"

using namespace nassp;

static WSADATA wsaData;
static SOCKET m_socket;
static sockaddr_in clientService;
static SOCKET close_Socket = INVALID_SOCKET;
static char debugString[100];
static char debugStringBuffer[100];
static char debugWinsock[100];

AR_GCore::AR_GCore(VESSEL* v)
{
	MissionPlanningActive = false;
	mptInitError = 0;

	AGOP_Page = 1;
	AGOP_Option = 1;
	AGOP_Mode = 1;
	AGOP_AdditionalOption = 0;
	AGOP_StartTime = 0.0;
	AGOP_StopTime = 0.0;
	AGOP_TimeStep = 0.0;
	AGOP_CSM_REFSMMAT = RTCC_REFSMMAT_TYPE_CUR;
	AGOP_LM_REFSMMAT = RTCC_REFSMMAT_TYPE_CUR;
	AGOP_Stars[0] = 1;
	AGOP_Stars[1] = 2;
	AGOP_Lat = 0.0;
	AGOP_Lng = 0.0;
	AGOP_Alt = 0.0;
	AGOP_Attitudes[0] = _V(0, 0, 0);
	AGOP_Attitudes[1] = _V(0, 0, 0);
	AGOP_AttIsCSM = true;
	AGOP_HeadsUp = true;
	AGOP_AntennaPitch = 0.0;
	AGOP_AntennaYaw = 0.0;
	AGOP_Instrument = 0;
	AGOP_LMCOASAxis = false;
	AGOP_LMAOTDetent = 2;
	AGOP_InstrumentAngles1[0] = 0.0;
	AGOP_InstrumentAngles1[1] = 0.0;
	AGOP_InstrumentAngles2[0] = 0.0;
	AGOP_InstrumentAngles2[1] = 0.0;
	AGOP_REFSMMAT = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);
	AGOP_REFSMMAT_Vehicle = 0;

	tlipad.TB6P = 0.0;
	tlipad.BurnTime = 0.0;
	tlipad.dVC = 0.0;
	tlipad.VI = 0.0;
	tlipad.SepATT = _V(0.0, 0.0, 0.0);
	tlipad.IgnATT = _V(0.0, 0.0, 0.0);
	tlipad.ExtATT = _V(0.0, 0.0, 0.0);

	pdipad.Att = _V(0, 0, 0);
	pdipad.CR = 0.0;
	pdipad.DEDA231 = 0.0;
	pdipad.GETI = 0.0;
	pdipad.t_go = 0.0;

	earthentrypad.Att400K[0] = _V(0, 0, 0);
	earthentrypad.BankAN[0] = 0;
	earthentrypad.DRE[0] = 0;
	earthentrypad.dVTO[0] = 0;
	earthentrypad.Lat[0] = 0;
	earthentrypad.Lng[0] = 0;
	earthentrypad.PB_BankAN[0] = 0;
	earthentrypad.PB_DRE[0] = 0;
	earthentrypad.PB_R400K[0] = 0;
	earthentrypad.PB_Ret05[0] = 0;
	earthentrypad.PB_Ret2[0] = 0;
	earthentrypad.PB_RetBBO[0] = 0;
	earthentrypad.PB_RetDrog[0] = 0;
	earthentrypad.PB_RetEBO[0] = 0;
	earthentrypad.PB_RetRB[0] = 0;
	earthentrypad.PB_RTGO[0] = 0;
	earthentrypad.PB_VIO[0] = 0;
	earthentrypad.Ret05[0] = 0;
	earthentrypad.Ret2[0] = 0;
	earthentrypad.RetBBO[0] = 0;
	earthentrypad.RetDrog[0] = 0;
	earthentrypad.RetEBO[0] = 0;
	earthentrypad.RetRB[0] = 0;
	earthentrypad.RTGO[0] = 0;
	earthentrypad.VIO[0] = 0;

	lunarentrypad.Att05[0] = _V(0, 0, 0);
	lunarentrypad.BSS[0] = 0;
	lunarentrypad.DO[0] = 0.0;
	lunarentrypad.Gamma400K[0] = 0.0;
	lunarentrypad.GETHorCheck[0] = 0.0;
	lunarentrypad.Lat[0] = 0.0;
	lunarentrypad.LiftVector[0][0] = 0;
	lunarentrypad.Lng[0] = 0.0;
	lunarentrypad.MaxG[0] = 0.0;
	lunarentrypad.PitchHorCheck[0] = 0.0;
	lunarentrypad.RET05[0] = 0.0;
	lunarentrypad.RRT[0] = 0.0;
	lunarentrypad.RTGO[0] = 0.0;
	lunarentrypad.SFT[0] = 0.0;
	lunarentrypad.SPA[0] = 0.0;
	lunarentrypad.SXP[0] = 0.0;
	lunarentrypad.SXTS[0] = 0;
	lunarentrypad.TRN[0] = 0;
	lunarentrypad.V400K[0] = 0.0;
	lunarentrypad.VIO[0] = 0.0;
	lunarentrypad.RETBBO[0] = 0.0;
	lunarentrypad.RETEBO[0] = 0.0;
	lunarentrypad.RETDRO[0] = 0.0;
	lunarentrypad.RETVCirc[0] = 0.0;
	lunarentrypad.DLMax[0] = 0.0;
	lunarentrypad.DLMin[0] = 0.0;
	lunarentrypad.VLMax[0] = 0.0;
	lunarentrypad.VLMin[0] = 0.0;

	entrypadopt = 0;
	EntryPADSxtStarCheckAttOpt = true;

	LmkLat = 0;
	LmkLng = 0;
	LmkTime = 0;
	LmkElevation = 35.0*RAD;
	landmarkpad.T1[0] = 0;
	landmarkpad.T2[0] = 0;
	landmarkpad.CRDist[0] = 0;
	landmarkpad.Alt[0] = 0;
	landmarkpad.Lat[0] = 0;
	landmarkpad.Lng05[0] = 0;

	int mission = 0;

	if (strcmp(v->GetName(), "AS-205") == 0)
	{
		mission = 7;
	}
	else if (strcmp(v->GetName(), "AS-503") == 0)
	{
		mission = 8;
	}
	else if (strcmp(v->GetName(), "AS-504") == 0 || strcmp(v->GetName(), "Gumdrop") == 0 || strcmp(v->GetName(), "Spider") == 0)
	{
		mission = 9;
	}
	else if (strcmp(v->GetName(), "AS-505") == 0 || strcmp(v->GetName(), "Charlie-Brown") == 0 || strcmp(v->GetName(), "Snoopy") == 0)
	{
		mission = 10;
	}
	else if (strcmp(v->GetName(), "AS-506") == 0 || strcmp(v->GetName(), "Columbia") == 0 || strcmp(v->GetName(), "Eagle") == 0)
	{
		mission = 11;
	}
	else if (strcmp(v->GetName(), "Yankee-Clipper") == 0 || strcmp(v->GetName(), "Intrepid") == 0)
	{
		mission = 12;
	}
	else if (strcmp(v->GetName(), "Odyssey") == 0 || strcmp(v->GetName(), "Aquarius") == 0)
	{
		mission = 13;
	}
	else if (strcmp(v->GetName(), "Kitty-Hawk") == 0 || strcmp(v->GetName(), "Antares") == 0)
	{
		mission = 14;
	}
	else if (strcmp(v->GetName(), "Endeavour") == 0 || strcmp(v->GetName(), "Falcon") == 0)
	{
		mission = 15;
	}
	else if (strcmp(v->GetName(), "Casper") == 0 || strcmp(v->GetName(), "Orion") == 0)
	{
		mission = 16;
	}
	else if (strcmp(v->GetName(), "America") == 0 || strcmp(v->GetName(), "Challenger") == 0)
	{
		mission = 17;
	}

	REFSMMAT_PTC_MJD = 0.0;

	if (mission == 10)
	{
		REFSMMAT_PTC_MJD = 40365.25560140741; //133:19:04 GET of nominal mission
	}
	else if (mission == 11)
	{
		REFSMMAT_PTC_MJD = 40426.71589131481; //195:38:53 GET of nominal mission
	}
	else if (mission == 12)
	{
		REFSMMAT_PTC_MJD = 40547.30729122223; //183:00:30 GET of nominal mission
	}
	else if (mission == 13)
	{
		REFSMMAT_PTC_MJD = 40695.238194; //178:30:00 GET of nominal mission
	}
	else if (mission == 14)
	{
		REFSMMAT_PTC_MJD = 40989.77326433333; //166:10:30 GET of nominal mission
	}
	else if (mission == 15)
	{
		REFSMMAT_PTC_MJD = 41168.15486133334; //230:09:00 GET of nominal mission
	}
	else if (mission == 16)
	{
		REFSMMAT_PTC_MJD = 41430.66446425925;  //166:02:50 GET of nominal mission
	}
	else if (mission == 17)
	{
		REFSMMAT_PTC_MJD = 41668.18229177778;  //241:29:30 GET of nominal mission
	}
	else
	{
		REFSMMAT_PTC_MJD = oapiGetSimMJD(); //Near current time usually gives a good PTC REFSMMAT, too
	}

	//Get a pointer to the RTCC. If the MCC vessel doesn't exist yet, create it
	OBJHANDLE hMCC = oapiGetVesselByName("MCC");
	if (hMCC == NULL)
	{
		VESSELSTATUS2 vs;
		memset(&vs, 0, sizeof(vs));
		vs.version = 2;
		vs.status = 1;
		vs.surf_lng = -95.08833333*RAD;
		vs.surf_lat = 29.55805556*RAD;
		vs.surf_hdg = 270.0*RAD;
		vs.rbody = oapiGetObjectByName("Earth");

		hMCC = oapiCreateVesselEx("MCC", "ProjectApollo/MCC", &vs);
		
	}

	VESSEL *pMCC = oapiGetVesselInterface(hMCC);
	MCCVessel *pMCCVessel = static_cast<MCCVessel*>(pMCC);
	rtcc = pMCCVessel->rtcc;

	
	//If the GMTBase hasn't been loaded into the RTCC we can assume it hasn't been properly initialized yet
	if (rtcc->GetGMTBase() == 0)
	{
		SetMissionSpecificParameters(mission);
	}
}

AR_GCore::~AR_GCore()
{

}

void AR_GCore::SetMissionSpecificParameters(int mission)
{
	if (mission == 7)
	{
		rtcc->SystemParametersFile = "Apollo 7 Constants";
		rtcc->LoadMissionFiles();
		rtcc->LoadLaunchDaySpecificParameters(1968, 10, 11);
		rtcc->GMGMED("P80,1,CSM,10,11,1968;");
		rtcc->GMGMED("P10,CSM,15:02:45;");
		rtcc->GMGMED("P12,CSM,15:02:45,72.0;");
		rtcc->GMGMED("P12,IU1,15:02:28,72.0;");
		rtcc->GMGMED("P15,AGC,15:02:45;");
		rtcc->GMGMED("P15,LGC,15:02:45;");
	}
	else if (mission == 8)
	{
		rtcc->SystemParametersFile = "Apollo 8 Constants";
		rtcc->TLIFile = "Apollo 8 TLI";
		rtcc->SFPFile = "Apollo 8 SFP";
		rtcc->LoadMissionFiles();
		rtcc->LoadLaunchDaySpecificParameters(1968, 12, 21);
		rtcc->GMGMED("P80,1,CSM,12,21,1968;");
		rtcc->GMGMED("P10,CSM,12:51:0;");
		rtcc->GMGMED("P12,CSM,12:51:0,72.124;");
		rtcc->GMGMED("P12,IU1,12:50:43,72.124;");
		rtcc->GMGMED("P15,AGC,12:51:0;");
		rtcc->GMGMED("P15,LGC,12:51:0;");
	}
	else if (mission == 9)
	{
		rtcc->SystemParametersFile = "Apollo 7 Constants";
		rtcc->LoadMissionFiles();
		rtcc->LoadLaunchDaySpecificParameters(1969, 3, 3);
		rtcc->GMGMED("P80,1,CSM,3,3,1969;");
		rtcc->GMGMED("P10,CSM,16:00:00;");
		rtcc->GMGMED("P12,CSM,16:00:00,72.0;");
		rtcc->GMGMED("P12,IU1,15:59:43,72.0;");
		rtcc->GMGMED("P15,AGC,16:00:00;");
		rtcc->GMGMED("P15,LGC,16:00:00;");
		rtcc->GMGMED("P15,AGS,,40:00:00;");
	}
	else if (mission == 10)
	{
		rtcc->SystemParametersFile = "Apollo 10 Constants";
		rtcc->TLIFile = "Apollo 10 TLI";
		rtcc->SFPFile = "Apollo 10 SFP";
		rtcc->LoadMissionFiles();
		rtcc->LoadLaunchDaySpecificParameters(1969, 5, 18);
		rtcc->GMGMED("P80,1,CSM,5,18,1969;");
		rtcc->GMGMED("P10,CSM,16:49:00;");
		rtcc->GMGMED("P12,CSM,16:49:00,72.028;");
		rtcc->GMGMED("P12,IU1,16:48:43,72.028;");
		rtcc->GMGMED("P15,AGC,16:49:00;");
		rtcc->GMGMED("P15,LGC,16:49:00;");
		rtcc->GMGMED("P15,AGS,,90:00:00;");
	}
	else if (mission == 11) // July 16th Launch
	{
		rtcc->SystemParametersFile = "Apollo 11 Constants";
		rtcc->TLIFile = "Apollo 11 TLI";
		rtcc->SFPFile = "Apollo 11 SFP";
		rtcc->LoadMissionFiles();
		rtcc->LoadLaunchDaySpecificParameters(1969, 7, 16);
		rtcc->GMGMED("P80,1,CSM,7,16,1969;");
		rtcc->GMGMED("P10,CSM,13:32:00;");
		rtcc->GMGMED("P12,CSM,13:32:00,72.058;");
		rtcc->GMGMED("P12,IU1,13:31:43,72.058;");
		rtcc->GMGMED("P15,AGC,13:32:00;");
		rtcc->GMGMED("P15,LGC,13:32:00;");
		rtcc->GMGMED("P15,AGS,,90:00:00;");

		//July 18 launch
		//rtcc->LoadLaunchDaySpecificParameters(1969, 7, 18);
		//rtcc->GMGMED("P80,1,CSM,7,18,1969;");
		//rtcc->GMGMED("P10,CSM,15:32:00;");
		//rtcc->GMGMED("P12,CSM,15:32:00,89.295;");
		//rtcc->GMGMED("P12,IU1,15:31:43,89.295;");
		//rtcc->GMGMED("P15,AGC,15:32:00;");
		//rtcc->GMGMED("P15,LGC,15:32:00;");
		//rtcc->GMGMED("P15,AGS,,90:00:00;");

		//July 21 launch
		//rtcc->LoadLaunchDaySpecificParameters(1969, 7, 21);
		//rtcc->GMGMED("P80,1,CSM,7,21,1969;");
		//rtcc->GMGMED("P10,CSM,16:09:00;");
		//rtcc->GMGMED("P12,CSM,16:09:00,94.6775;");
		//rtcc->GMGMED("P12,IU1,16:08:43,94.6775;");
		//rtcc->GMGMED("P15,AGC,16:09:00;");
		//rtcc->GMGMED("P15,LGC,16:09:00;");
		//rtcc->GMGMED("P15,AGS,,90:00:00;");
	}
	else if (mission == 12)
	{
		rtcc->SystemParametersFile = "Apollo 12 Constants";
		rtcc->TLIFile = "Apollo 12 TLI";
		rtcc->SFPFile = "Apollo 12 SFP";
		rtcc->LoadMissionFiles();
		rtcc->LoadLaunchDaySpecificParameters(1969, 11, 14);
		rtcc->GMGMED("P80,1,CSM,11,14,1969;");
		rtcc->GMGMED("P10,CSM,16:22:00;");
		rtcc->GMGMED("P12,CSM,16:22:00,72.029;");
		rtcc->GMGMED("P12,IU1,16:21:43,72.029;");
		rtcc->GMGMED("P15,AGC,16:22:00;");
		rtcc->GMGMED("P15,LGC,16:22:00;");
		rtcc->GMGMED("P15,AGS,,100:00:00;");
	}
	else if (mission == 13)
	{
		rtcc->SystemParametersFile = "Apollo 13 Constants";
		rtcc->TLIFile = "Apollo 13 TLI";
		rtcc->SFPFile = "Apollo 13 SFP";
		rtcc->LoadMissionFiles();
		rtcc->LoadLaunchDaySpecificParameters(1970, 4, 11);
		rtcc->GMGMED("P80,1,CSM,4,11,1970;");
		rtcc->GMGMED("P10,CSM,19:13:00;");
		rtcc->GMGMED("P12,CSM,19:13:00,72.043;");
		rtcc->GMGMED("P12,IU1,19:12:43,72.043;");
		rtcc->GMGMED("P15,AGC,19:13:00;");
		rtcc->GMGMED("P15,LGC,19:13:00;");
		rtcc->GMGMED("P15,AGS,,90:00:00;");
	}
	else if (mission == 14)
	{
		rtcc->SystemParametersFile = "Apollo 14 Constants";
		rtcc->TLIFile = "Apollo 14 TLI";
		rtcc->SFPFile = "Apollo 14 SFP";
		rtcc->LoadMissionFiles();
		rtcc->LoadLaunchDaySpecificParameters(1971, 1, 31);
		rtcc->GMGMED("P80,1,CSM,1,31,1971;");
		rtcc->GMGMED("P10,CSM,20:23:00;");
		rtcc->GMGMED("P12,CSM,20:23:00,72.067;");
		rtcc->GMGMED("P12,IU1,20:22:43,72.067;");
		rtcc->GMGMED("P15,AGC,20:23:00;");
		rtcc->GMGMED("P15,LGC,20:23:00;");
		rtcc->GMGMED("P15,AGS,,100:00:00;");
	}
	else if (mission == 15)
	{
		rtcc->SystemParametersFile = "Apollo 15 Constants";
		rtcc->TLIFile = "Apollo 15 TLI";
		rtcc->SFPFile = "Apollo 15 SFP";
		rtcc->LoadMissionFiles();
		rtcc->LoadLaunchDaySpecificParameters(1971, 7, 26);
		rtcc->GMGMED("P80,1,CSM,7,26,1971;");
		rtcc->GMGMED("P10,CSM,13:34:00;");
		rtcc->GMGMED("P12,CSM,13:34:00,80.088;");
		rtcc->GMGMED("P12,IU1,13:33:43,80.088;");
		rtcc->GMGMED("P15,AGC,13:34:00;");
		rtcc->GMGMED("P15,LGC,13:34:00;");
		rtcc->GMGMED("P15,AGS,,100:00:00;");
	}
	else if (mission == 16)
	{
		rtcc->SystemParametersFile = "Apollo 16 Constants";
		rtcc->TLIFile = "Apollo 16 TLI";
		rtcc->SFPFile = "Apollo 16 SFP";
		rtcc->LoadMissionFiles();
		rtcc->LoadLaunchDaySpecificParameters(1972, 4, 16);
		rtcc->GMGMED("P80,1,CSM,4,16,1972;");
		rtcc->GMGMED("P10,CSM,17:54:00;");
		rtcc->GMGMED("P12,CSM,17:54:00,72.034;");
		rtcc->GMGMED("P12,IU1,17:53:43,72.034;");
		rtcc->GMGMED("P15,AGC,17:54:00;");
		rtcc->GMGMED("P15,LGC,17:54:00;");
		rtcc->GMGMED("P15,AGS,,90:00:00;");
	}
	else if (mission == 17)
	{
		rtcc->SystemParametersFile = "Apollo 17 Constants";
		rtcc->TLIFile = "Apollo 17 TLI";
		rtcc->SFPFile = "Apollo 17 SFP";
		rtcc->LoadMissionFiles();
		rtcc->LoadLaunchDaySpecificParameters(1972, 12, 7);
		rtcc->GMGMED("P80,1,CSM,12,7,1972;");
		rtcc->GMGMED("P10,CSM,02:53:00;");
		rtcc->GMGMED("P12,CSM,02:53:00,72.141;");
		rtcc->GMGMED("P12,IU1,02:52:43,72.141;");
		rtcc->GMGMED("P15,AGC,02:53:00;");
		rtcc->GMGMED("P15,LGC,02:53:00;");
		rtcc->GMGMED("P15,AGS,,110:00:00;");
	}
}

int AR_GCore::MPTTrajectoryUpdate(VESSEL *ves, bool csm)
{
	if (ves == NULL) return 1;

	bool landed = ves->GroundContact();

	//CSM state vector can't be landed of course...
	if (csm && landed) return 1;

	EphemerisData sv = rtcc->StateVectorCalcEphem(ves);

	int id;
	char letter;
	if (csm)
	{
		id = 5;
		letter = 'C';
	}
	else
	{
		id = 11;
		letter = 'L';
	}

	if (rtcc->BZUSEVEC.data[id].ID < 0)
	{
		rtcc->BZUSEVEC.data[id].ID = 0;
	}
	rtcc->BZUSEVEC.data[id].ID++;
	rtcc->BZUSEVEC.data[id].Vector = sv;
	if (landed)
	{
		rtcc->BZUSEVEC.data[id].LandingSiteIndicator = true;
	}
	else
	{
		rtcc->BZUSEVEC.data[id].LandingSiteIndicator = false;
	}
	char Buff[16];
	sprintf_s(Buff, "API%c%03d", letter, rtcc->BZUSEVEC.data[id].ID);
	rtcc->BZUSEVEC.data[id].VectorCode.assign(Buff);
	return 0;
}

void AR_GCore::MPTMassUpdate()
{
	//Mass Update
	VESSEL *v;

	if (rtcc->med_m49.Table == RTCC_MPT_CSM)
	{
		v = rtcc->pCSM;
	}
	else
	{
		v = rtcc->pLM;
	}

	if (v == NULL) return;

	rtcc->MPTMassUpdate(v, rtcc->med_m50, rtcc->med_m55, rtcc->med_m49);
}

bool AR_GCore::AGOP_CSM_REFSMMAT_Required()
{
	bool GetCSMREFSMMAT = false;

	if (AGOP_Option == 1 || AGOP_Option == 5 || AGOP_Option == 6) GetCSMREFSMMAT = true;
	else if (AGOP_Option == 4)
	{
		if (AGOP_Mode == 1 || AGOP_Mode == 4) GetCSMREFSMMAT = true;
		else if (AGOP_AttIsCSM) GetCSMREFSMMAT = true;
	}
	else if (AGOP_Option == 7)
	{
		if (AGOP_Mode == 2)
		{
			if (AGOP_AttIsCSM) GetCSMREFSMMAT = true;
		}
		else if (AGOP_Mode == 4 && AGOP_AdditionalOption < 3)
		{
			GetCSMREFSMMAT = true;
		}
		else if (AGOP_Mode == 5) GetCSMREFSMMAT = true;
		else if (AGOP_Mode == 6) GetCSMREFSMMAT = true;
	}

	return GetCSMREFSMMAT;
}

bool AR_GCore::AGOP_LM_REFSMMAT_Required()
{
	bool GetLMREFSMMAT = false;

	if (AGOP_Option == 4)
	{
		if (AGOP_Mode != 1 && AGOP_Mode != 4)
		{
			GetLMREFSMMAT = true;
		}
		else if (!AGOP_AttIsCSM)
		{
			GetLMREFSMMAT = true;
		}
	}
	else if (AGOP_Option == 7)
	{
		if (AGOP_Mode == 1) GetLMREFSMMAT = true;
		else if (AGOP_Mode == 2 && AGOP_AttIsCSM == false) GetLMREFSMMAT = true;
		else if (AGOP_Mode == 4)
		{
			if (AGOP_AdditionalOption > 0) GetLMREFSMMAT = true;
		}
		else if (AGOP_Mode == 6) GetLMREFSMMAT = true; //But is actually a CSM REFSMMAT
	}

	return GetLMREFSMMAT;
}

void AR_GCore::DFLBackgroundSlide(oapi::Sketchpad *skp, DWORD W, DWORD H, unsigned display)
{
	BackgroundSlides.Print(skp, W, H, display);
}

ARCore::ARCore(VESSEL* v, AR_GCore* gcin)
{
	GC = gcin;

	SPQMode = 0;
	CSItime = 0.0;
	CDHtime = 0.0;
	SPQTIG = 0.0;
	CDHtimemode = 0;
	t_TPI = 0.0;

	SPQDeltaV = _V(0, 0, 0);
	//screen = 0;
	REFSMMAT_LVLH_Time = 0.0;
	REFSMMATopt = 4;
	REFSMMATcur = 4;
	manpadopt = 0;
	lemdescentstage = true;
	mptinitmode = 3;

	vesselisdocked = false;
	//For now, CSM or LM being docked will set this flag
	if (GC->rtcc->pCSM)
	{
		if (GC->rtcc->pCSM->DockingStatus(0) == 1)
		{
			vesselisdocked = true;
		}
	}
	if (GC->rtcc->pLM)
	{
		if (GC->rtcc->pLM->DockingStatus(0) == 1)
		{
			vesselisdocked = true;
		}
	}

	REFSMMATHeadsUp = true;

	GMPManeuverCode = 0;
	GMPManeuverPoint = 0;
	GMPManeuverType = 0;
	SPSGET = 0.0;
	GMPApogeeHeight = 0;
	GMPPerigeeHeight = 0;
	GMPWedgeAngle = 0.0;
	GMPManeuverLongitude = 0.0;
	GMPManeuverHeight = 0.0;
	GMPHeightChange = 0.0;
	GMPNodeShiftAngle = 0.0;
	GMPDeltaVInput = 0.0;
	GMPPitch = 0.0;
	GMPYaw = 0.0;
	GMPRevs = 0;
	GMPApseLineRotAngle = 0.0;

	RTEASTType = 0;

	g_Data.uplinkBufferSimt = 0;
	g_Data.connStatus = 0;
	g_Data.uplinkState = 0;
	if (GC->rtcc->pLM)
	{
		if (utils::IsVessel(GC->rtcc->pLM, utils::LEM))
		{
			LEM *lem = (LEM *)GC->rtcc->pLM;
			if (lem->GetStage() < 2)
			{
				lemdescentstage = true;
			}
			else
			{
				lemdescentstage = false;
			}
		}
	}
	for (int i = 0; i < 24; i++)
	{
		g_Data.emem[i] = 0;
	}
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		sprintf(debugWinsock, "ERROR AT WSAStartup()");
	}
	else {
		sprintf(debugWinsock, "DISCONNECTED");
	}
	P30TIG = 0;
	dV_LVLH = _V(0.0, 0.0, 0.0);

	RTEReentryTime = 0.0;
	entryrange = GC->rtcc->PZREAP.RRBIAS;
	RTECalcMode = 1;
	RTETradeoffMode = 0;
	RTEASTType = 76;

	SVDesiredGET = -1;
	HeadsUp = false;

	manpadenginetype = RTCC_ENGINETYPE_CSMSPS;
	sxtstardtime = -30.0*60.0;
	manpad_ullage_dt = 0.0;
	manpad_ullage_opt = true;
	ManPADMPT = 1;
	ManPADMPTManeuver = 1;
	TLIPAD_StudyAid = false;

	mapupdate.LOSGET = 0.0;
	mapupdate.AOSGET = 0.0;
	mapupdate.SSGET = 0.0;
	mapupdate.SRGET = 0.0;
	mapupdate.PMGET = 0.0;
	mappage = 1;
	mapgs = 0;
	mapUpdateGET = 0.0;
	mapUpdatePM = false;
	GSAOSGET = 0.0;
	GSLOSGET = 0.0;
	PADSolGood = true;
	Rendezvous_Target = NULL;
	iuvessel = NULL;
	TLCCSolGood = true;

	landingzone = 0;
	entryprecision = -1;

	subThreadMode = 0;
	subThreadStatus = DONE;
	IsCSMCalculation = false;

	VECoption = 0;
	VECdirection = 0;
	VECbody = NULL;
	VECangles = _V(0, 0, 0);
	VECBodyVector = _V(0, 0, 0);

	TPI_Mode = 0;
	dt_TPI_sunrise = 16.0*60.0;

	PDAPEngine = 0;
	PDAPTwoSegment = false;
	PDAPABTCOF[0] = 0.0;
	PDAPABTCOF[1] = 0.0;
	PDAPABTCOF[2] = 0.0;
	PDAPABTCOF[3] = 0.0;
	PDAPABTCOF[4] = 0.0;
	PDAPABTCOF[5] = 0.0;
	PDAPABTCOF[6] = 0.0;
	PDAPABTCOF[7] = 0.0;
	DEDA224 = 0.0;
	DEDA225 = 0.0;
	DEDA226 = 0.0;
	DEDA227 = 0;

	PDAP_J1 = 6.0325675e6*0.3048;
	PDAP_K1 = -6.2726125e5*0.3048;
	PDAP_J2 = 6.03047e6*0.3048;
	PDAP_K2 = -3.1835146e5*0.3048;
	PDAP_Theta_LIM = 8.384852304*RAD;
	PDAP_R_amin = 5.8768997e6*0.3048;

	TMLat = 0.0;
	TMLng = 0.0;
	TMAzi = -90.0*RAD;
	TMDistance = 600000.0*0.3048;
	TMStepSize = 100.0*0.3048;
	TMAlt = 0.0;

	t_LunarLiftoff = 0.0;
	LAP_Phase = 0.0;
	LAP_CR = 0.0;
	AscentPADVersion = 0;
	t_TPIguess = 0.0;

	EMPUplinkNumber = 1;
	EMPUplinkMaxNumber = 0;

	LVDCLaunchAzimuth = 0.0;

	navcheckpad.alt[0] = 0.0;
	navcheckpad.lat[0] = 0.0;
	navcheckpad.lng[0] = 0.0;
	navcheckpad.NavChk[0] = 0.0;

	agssvpad.DEDA240 = 0.0;
	agssvpad.DEDA241 = 0.0;
	agssvpad.DEDA242 = 0.0;
	agssvpad.DEDA244 = 0.0;
	agssvpad.DEDA245 = 0.0;
	agssvpad.DEDA246 = 0.0;
	agssvpad.DEDA254 = 0.0;
	agssvpad.DEDA260 = 0.0;
	agssvpad.DEDA261 = 0.0;
	agssvpad.DEDA262 = 0.0;
	agssvpad.DEDA264 = 0.0;
	agssvpad.DEDA265 = 0.0;
	agssvpad.DEDA266 = 0.0;
	agssvpad.DEDA272 = 0.0;

	DAP_PAD.OtherVehicleWeight = 0.0;
	DAP_PAD.PitchTrim = 0.0;
	DAP_PAD.ThisVehicleWeight = 0.0;
	DAP_PAD.YawTrim = 0.0;

	lmascentpad.CR = 0.0;
	lmascentpad.DEDA047 = 0;
	lmascentpad.DEDA053 = 0;
	lmascentpad.DEDA225_226 = 0.0;
	lmascentpad.DEDA231 = 0.0;
	sprintf(lmascentpad.remarks, "");
	lmascentpad.TIG = 0.0;
	lmascentpad.V_hor = 0.0;
	lmascentpad.V_vert = 0.0;

	NodeConvOpt = true;
	NodeConvLat = 0.0;
	NodeConvLng = 0.0;
	NodeConvHeight = 0.0;
	NodeConvGET = 0.0;
	NodeConvResLat = 0.0;
	NodeConvResLng = 0.0;

	SpaceDigitalsOption = 1;
	SpaceDigitalsGET = 0.0;

	for (int i = 0;i < 2;i++)
	{
		AGCClockTime[i] = 0.0;
		RTCCClockTime[i] = 0.0;
		DeltaClockTime[i] = 0.0;
		DesiredRTCCLiftoffTime[i] = 0.0;
	}
	iuUplinkResult = DONE;

	LUNTAR_lat = 0.0;
	LUNTAR_lng = 0.0;
	LUNTAR_bt_guess = 0.0;
	LUNTAR_pitch_guess = 0.0;
	LUNTAR_yaw_guess = 0.0;
	LUNTAR_TIG = 0.0;

	DebugIMUTorquingAngles = _V(0, 0, 0);

	for (int i = 0; i < 6; i++)
	{
		GravVec[i] = 0;
	}
	IMUParkingAngles = _V(0, 0, 0);
}

ARCore::~ARCore()
{
}

void ARCore::MinorCycle(double SimT, double SimDT, double mjd)
{
	if (g_Data.connStatus > 0 && g_Data.uplinkBuffer.size() > 0) {
		if (SimT > g_Data.uplinkBufferSimt + 0.1) {
			unsigned char data = g_Data.uplinkBuffer.front();
			send(m_socket, (char *)&data, 1, 0);
			g_Data.uplinkBuffer.pop();
			g_Data.uplinkBufferSimt = SimT;
		}
	}
	else if (g_Data.connStatus > 0 && g_Data.uplinkBuffer.size() == 0) {
		if (g_Data.connStatus == 1)	{
			sprintf(debugWinsock, "DISCONNECTED");
			g_Data.connStatus = 0;
			closesocket(m_socket);
		}
	}
}

void ARCore::LmkCalc()
{
	startSubthread(33);
}

void ARCore::LOICalc()
{
	startSubthread(5);
}

void ARCore::LDPPalc()
{
	startSubthread(10);
}

void ARCore::LunarLaunchTargetingCalc()
{
	startSubthread(13);
}

void ARCore::LunarLiftoffCalc()
{
	startSubthread(15);
}

void ARCore::EntryUpdateCalc()
{
	VESSEL *v = GC->rtcc->pCSM;

	if (v == NULL) return;

	SV sv0;
	EntryResults res;

	sv0 = GC->rtcc->StateVectorCalc(v);
	GC->rtcc->EntryUpdateCalc(sv0, entryrange, true, &res);

	GC->rtcc->RZDBSC1.lat_T = res.latitude;
	GC->rtcc->RZDBSC1.lng_T = res.longitude;
}

void ARCore::EntryCalc()
{
	startSubthread(7);
}

void ARCore::DeorbitCalc()
{
	startSubthread(17);
}

void ARCore::TLI_PAD()
{
	startSubthread(8);
}

void ARCore::TLCCCalc()
{
	startSubthread(14);
}

void ARCore::PDI_PAD()
{
	startSubthread(16);
}

void ARCore::LAPCalc()
{
	startSubthread(20);
}

void ARCore::AscentPADCalc()
{
	startSubthread(21);
}

void ARCore::PDAPCalc()
{
	startSubthread(22);
}

void ARCore::CycleVectorPanelSummary()
{
	if (subThreadStatus == DONE)
	{
		if (GC->rtcc->RTCCPresentTimeGMT() > GC->rtcc->VectorPanelSummaryBuffer.gmt + 6.0)
		{
			GC->rtcc->VectorPanelSummaryBuffer.gmt = GC->rtcc->RTCCPresentTimeGMT();
			startSubthread(34);
		}
	}
}

void ARCore::CycleFIDOOrbitDigitals1()
{
	if (subThreadStatus == DONE)
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->rtcc->CalcGETBase());
		if (GET > GC->rtcc->EZSAVCSM.GET + 12.0)
		{
			startSubthread(24);
		}
	}
}

void ARCore::CycleFIDOOrbitDigitals2()
{
	if (subThreadStatus == DONE)
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->rtcc->CalcGETBase());
		if (GET > GC->rtcc->EZSAVLEM.GET + 12.0)
		{
			startSubthread(26);
		}
	}
}

void ARCore::CycleSpaceDigitals()
{
	if (subThreadStatus == DONE)
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->rtcc->CalcGETBase());
		if (GET > GC->rtcc->EZSPACE.GET + 12.0)
		{
			startSubthread(29);
		}
	}
}

void ARCore::SpaceDigitalsMSKRequest()
{
	if (subThreadStatus == DONE)
	{
		startSubthread(30);
	}
}

void ARCore::GenerateSpaceDigitalsNoMPT()
{
	startSubthread(11);
}

void ARCore::TLIProcessorCalc()
{
	startSubthread(12);
}

void ARCore::SaturnVTLITargetUplink()
{
	startSubthread(57);
}

void ARCore::LUNTARCalc()
{
	startSubthread(50);
}

void ARCore::CycleNextStationContactsDisplay()
{
	if (subThreadStatus == DONE)
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->rtcc->CalcGETBase());
		if (GET > GC->rtcc->NextStationContactsBuffer.GET + 12.0)
		{
			startSubthread(36);
		}
	}
}

void ARCore::RecoveryTargetSelectionCalc()
{
	startSubthread(37);
}

void ARCore::SLVNavigationUpdateCalc()
{
	startSubthread(27);
}

void ARCore::SLVNavigationUpdateUplink()
{
	startSubthread(28);
}

void ARCore::RTETradeoffDisplayCalc()
{
	startSubthread(52);
}

void ARCore::GeneralMEDRequest()
{
	startSubthread(53);
}

void ARCore::SkylabSaturnIBLaunchCalc()
{
	startSubthread(54);
}

void ARCore::SkylabSaturnIBLaunchUplink()
{
	startSubthread(55);
}

void ARCore::PerigeeAdjustCalc()
{
	startSubthread(56);
}

void ARCore::GetAGSKFactor()
{
	startSubthread(35);
}

void ARCore::TransferTIToMPT()
{
	startSubthread(38);
}

void ARCore::TransferSPQToMPT()
{
	startSubthread(39);
}

void ARCore::TransferDKIToMPT()
{
	startSubthread(40);
}

void ARCore::MPTDirectInputCalc()
{
	startSubthread(41);
}

void ARCore::TransferDescentPlanToMPT()
{
	startSubthread(42);
}

void ARCore::TransferPoweredDescentToMPT()
{
	startSubthread(43);
}

void ARCore::TransferPoweredAscentToMPT()
{
	startSubthread(44);
}

void ARCore::TransferGPMToMPT()
{
	startSubthread(45);
}

void ARCore::MPTTLIDirectInput()
{
	startSubthread(46);
}

void ARCore::AbortScanTableCalc()
{
	startSubthread(47);
}

void ARCore::TransferLOIorMCCtoMPT()
{
	startSubthread(48);
}

void ARCore::TransferRTEToMPT()
{
	startSubthread(49);
}

void ARCore::DAPPADCalc(bool IsCSM)
{
	VESSEL*v;

	if (IsCSM)
	{
		v = GC->rtcc->pCSM;
	}
	else
	{
		v = GC->rtcc->pLM;
	}

	if (v == NULL) return;

	if (IsCSM)
	{
		GC->rtcc->CSMDAPUpdate(v, DAP_PAD, vesselisdocked);
	}
	else
	{
		GC->rtcc->LMDAPUpdate(v, DAP_PAD, vesselisdocked, lemdescentstage == false);
	}
}

void ARCore::EntryPAD()
{
	startSubthread(31);
}

void ARCore::ManeuverPAD(bool IsCSM)
{
	startSubthread(9, IsCSM);
}

void ARCore::TPIPAD()
{
	startSubthread(6);
}

void ARCore::MapUpdate(bool IsCSM)
{
	startSubthread(32, IsCSM);
}

void ARCore::CalculateTPITime()
{
	startSubthread(23);
}

void ARCore::VectorCompareDisplayCalc()
{
	startSubthread(25);
}

void ARCore::UpdateGRRTime(VESSEL *v)
{
	if (v == NULL) return;

	bool isSaturnV;
	double T_L, Azi;
	LVDC *lvdc;

	if (utils::IsVessel(v,utils::SaturnV))
	{
		Saturn *iuv = (Saturn *)v;

		if (iuv->GetStage() >= CSM_LEM_STAGE) return;

		lvdc = iuv->GetIU()->GetLVDC();
		isSaturnV = true;
	}
	else if (utils::IsVessel(v, utils::SaturnIB))
	{
		Saturn *iuv = (Saturn *)v;

		if (iuv->GetStage() >= CSM_LEM_STAGE) return;

		lvdc = iuv->GetIU()->GetLVDC();
		isSaturnV = false;
	}
	else if (utils::IsVessel(v, utils::SaturnV_SIVB))
	{
		SIVB *iuv = (SIVB *)v;
		lvdc = iuv->GetIU()->GetLVDC();
		isSaturnV = true;
	}
	else if (utils::IsVessel(v, utils::SaturnIB_SIVB))
	{
		SIVB *iuv = (SIVB *)v;
		lvdc = iuv->GetIU()->GetLVDC();
		isSaturnV = false;
	}
	else
	{
		return;
	}

	if (isSaturnV)
	{
		LVDCSV *l = (LVDCSV*)lvdc;
		T_L = l->T_L;
		Azi = l->Azimuth;
	}
	else
	{
		LVDC1B *l = (LVDC1B*)lvdc;
		T_L = l->T_GRR;
		Azi = l->Azimuth;
	}

	int hh, mm;
	double ss;
	char Buff[128];
	OrbMech::SStoHHMMSS(T_L, hh, mm, ss, 0.01);
	sprintf_s(Buff, "P12,IU1,%d:%d:%.2lf,%.3lf;", hh, mm, ss, Azi*DEG);
	GC->rtcc->GMGMED(Buff);
}

void ARCore::GetStateVectorFromIU()
{
	bool isSaturnV;
	IU* iu;
	VESSEL *v;

	//For now only Saturn class
	v = GC->rtcc->pCSM;

	if (v == NULL) return;

	if (utils::IsVessel(v, utils::SaturnV))
	{
		Saturn *iuv = (Saturn *)v;
		iu = iuv->GetIU();
		isSaturnV = true;
	}
	else if (utils::IsVessel(v, utils::SaturnIB))
	{
		Saturn *iuv = (Saturn *)v;
		iu = iuv->GetIU();
		isSaturnV = false;
	}
	else if (utils::IsVessel(v, utils::SaturnV_SIVB))
	{
		SIVB *iuv = (SIVB *)v;
		iu = iuv->GetIU();
		isSaturnV = true;
	}
	else if (utils::IsVessel(v, utils::SaturnIB_SIVB))
	{
		SIVB *iuv = (SIVB *)v;
		iu = iuv->GetIU();
		isSaturnV = false;
	}
	else
	{
		return;
	}

	if (iu == NULL)
	{
		return;
	}

	EphemerisData sv;
	VECTOR3 R, V;
	double TAS;

	if (isSaturnV == false)
	{
		LVDC1B *lvdc = (LVDC1B*)iu->GetLVDC();

		R = lvdc->PosS;
		V = lvdc->DotS;
		TAS = lvdc->TAS;
	}
	else
	{
		LVDCSV *lvdc = (LVDCSV*)iu->GetLVDC();

		R = lvdc->PosS;
		V = lvdc->DotS;
		TAS = lvdc->TAS;
	}
	sv.R = tmul(GC->rtcc->GZLTRA.IU1_REFSMMAT, R);
	sv.V = tmul(GC->rtcc->GZLTRA.IU1_REFSMMAT, V);
	sv.GMT = TAS + GC->rtcc->GetIUClockZero();
	sv.RBI = BODY_EARTH;

	GC->rtcc->BZSTLM.HighSpeedIUVector = sv;
}

void ARCore::GetStateVectorsFromAGS()
{
	VESSEL *v = GC->rtcc->pLM;

	if (v == NULL) return;

	//0-6: pos and vel
	int csmvecoct[6], lmvecoct[6];
	int timeoct[2];

	LEM *lem = (LEM *)v;

	//Get Data
	lmvecoct[0] = lem->aea.vags.Memory[0340];
	lmvecoct[1] = lem->aea.vags.Memory[0341];
	lmvecoct[2] = lem->aea.vags.Memory[0342];
	lmvecoct[3] = lem->aea.vags.Memory[0360];
	lmvecoct[4] = lem->aea.vags.Memory[0361];
	lmvecoct[5] = lem->aea.vags.Memory[0362];

	csmvecoct[0] = lem->aea.vags.Memory[0344];
	csmvecoct[1] = lem->aea.vags.Memory[0345];
	csmvecoct[2] = lem->aea.vags.Memory[0346];
	csmvecoct[3] = lem->aea.vags.Memory[0364];
	csmvecoct[4] = lem->aea.vags.Memory[0365];
	csmvecoct[5] = lem->aea.vags.Memory[0366];

	timeoct[0] = lem->aea.vags.Memory[0377];
	timeoct[1] = lem->aea.vags.Memory[0353];

	//From twos complement
	for (int i = 0;i < 6;i++)
	{
		if (lmvecoct[i] >= 0400000)
		{
			lmvecoct[i] = lmvecoct[i] - 01000000;
		}
	}
	for (int i = 0;i < 6;i++)
	{
		if (csmvecoct[i] >= 0400000)
		{
			csmvecoct[i] = csmvecoct[i] - 01000000;
		}
	}

	VECTOR3 R_CSM, V_CSM, R_LM, V_LM;
	double T_SV;

	R_LM = _V(lmvecoct[0], lmvecoct[1], lmvecoct[2]);
	V_LM = _V(lmvecoct[3], lmvecoct[4], lmvecoct[5]);
	R_CSM = _V(csmvecoct[0], csmvecoct[1], csmvecoct[2]);
	V_CSM = _V(csmvecoct[3], csmvecoct[4], csmvecoct[5]);

	//Scale
	R_LM *= pow(2, 6)*0.3048;
	V_LM *= pow(2, -4)*0.3048;
	R_CSM *= pow(2, 6)*0.3048;
	V_CSM *= pow(2, -4)*0.3048;

	T_SV = (double)(timeoct[0])*2.0 + (double)(timeoct[1]) *pow(2, -16);

	//Convert to RTCC coordinates
	EphemerisData sv_CSM, sv_LM;
	MATRIX3 Rot = GC->rtcc->EZJGMTX3.data[RTCC_REFSMMAT_TYPE_AGS - 1].REFSMMAT;
	sv_LM.R = tmul(Rot, R_LM);
	sv_LM.V = tmul(Rot, V_LM);
	sv_CSM.R = tmul(Rot, R_CSM);
	sv_CSM.V = tmul(Rot, V_CSM);
	sv_CSM.GMT = sv_LM.GMT = T_SV + GC->rtcc->GetAGSClockZero();

	if (GC->rtcc->AGCGravityRef(v) == oapiGetObjectByName("Moon"))
	{
		sv_CSM.RBI = sv_LM.RBI = BODY_MOON;
	}
	else
	{
		//Scale to Earth units
		sv_CSM.R *= 10.0;
		sv_CSM.V *= 10.0;
		sv_LM.R *= 10.0;
		sv_LM.V *= 10.0;
		sv_CSM.RBI = sv_LM.RBI = BODY_EARTH;
	}

	//Save in telemetry table
	GC->rtcc->BZSTLM.HighSpeedAGSCSMVector = sv_CSM;
	GC->rtcc->BZSTLM.HighSpeedAGSLEMVector = sv_LM;
}

agc_t* ARCore::GetAGCPointer(bool cmc) const
{
	VESSEL *v;

	if (cmc)
	{
		v = GC->rtcc->pCSM;
	}
	else
	{
		v = GC->rtcc->pLM;
	}

	if (v == NULL) return NULL;

	agc_t* vagc = NULL;

	if (cmc)
	{
		if (utils::IsVessel(v, utils::ClassNames::Saturn) == false)
		{
			return NULL;
		}
		Saturn *saturn = (Saturn *)v;
		vagc = &saturn->agc.vagc;
	}
	else
	{
		if (utils::IsVessel(v, utils::ClassNames::LEM) == false)
		{
			return NULL;
		}
		LEM *lem = (LEM *)v;
		vagc = &lem->agc.vagc;
	}

	return vagc;
}

void ARCore::GetStateVectorFromAGC(bool csm, bool cmc)
{
	agc_t* vagc = GetAGCPointer(cmc);
	if (vagc == NULL) return;

	unsigned short SVoct[16];
	int SVadd, MoonBit;
	
	if (csm)
	{
		SVadd = 01554;
		MoonBit = 11;
	}
	else
	{
		SVadd = 01626;
		MoonBit = 10;
	}
	
	bool MoonFlag;

	for (int i = 0;i < 14;i++)
	{
		SVoct[i] = vagc->Erasable[0][SVadd + i];
	}
	SVoct[14] = vagc->Erasable[0][SVadd + 38];
	SVoct[15] = vagc->Erasable[0][SVadd + 39];

	MoonFlag = (vagc->Erasable[0][0104] & (1 << MoonBit));

	VECTOR3 R, V;
	double GET;

	R.x = OrbMech::DecToDouble(SVoct[0], SVoct[1]);
	R.y = OrbMech::DecToDouble(SVoct[2], SVoct[3]);
	R.z = OrbMech::DecToDouble(SVoct[4], SVoct[5]);
	V.x = OrbMech::DecToDouble(SVoct[6], SVoct[7])*100.0;
	V.y = OrbMech::DecToDouble(SVoct[8], SVoct[9])*100.0;
	V.z = OrbMech::DecToDouble(SVoct[10], SVoct[11])*100.0;
	GET = (OrbMech::DecToDouble(SVoct[12], SVoct[13]) - OrbMech::DecToDouble(SVoct[14], SVoct[15])) / 100.0*pow(2, 28);

	if (MoonFlag)
	{
		R.x *= pow(2, 27);
		R.y *= pow(2, 27);
		R.z *= pow(2, 27);
		V.x *= pow(2, 5);
		V.y *= pow(2, 5);
		V.z *= pow(2, 5);
	}
	else
	{
		R.x *= pow(2, 29);
		R.y *= pow(2, 29);
		R.z *= pow(2, 29);
		V.x *= pow(2, 7);
		V.y *= pow(2, 7);
		V.z *= pow(2, 7);
	}

	EphemerisData sv;
	sv.R = R;
	sv.V = V;
	if (cmc)
	{
		sv.GMT = GET + GC->rtcc->GetCMCClockZero();
	}
	else
	{
		sv.GMT = GET + GC->rtcc->GetLGCClockZero();
	}
	
	if (MoonFlag)
	{
		sv.RBI = BODY_MOON;
	}
	else
	{
		sv.RBI = BODY_EARTH;
	}

	if (csm)
	{
		if (cmc)
		{
			GC->rtcc->BZSTLM.HighSpeedCMCCSMVector = sv;
		}
		else
		{
			GC->rtcc->BZSTLM.HighSpeedLGCCSMVector = sv;
		}
	}
	else
	{
		if (cmc)
		{
			GC->rtcc->BZSTLM.HighSpeedCMCLEMVector = sv;
		}
		else
		{
			GC->rtcc->BZSTLM.HighSpeedLGCLEMVector = sv;
		}
	}
}

void ARCore::NavCheckPAD(bool IsCSM)
{
	SV sv;

	VESSEL *v;

	if (IsCSM)
	{
		v = GC->rtcc->pCSM;
	}
	else
	{
		v = GC->rtcc->pLM;
	}

	if (v == NULL) return;

	sv = GC->rtcc->StateVectorCalc(v);

	GC->rtcc->NavCheckPAD(sv, navcheckpad, navcheckpad.NavChk[0]);
}

void ARCore::LandingSiteUpdate()
{
	VESSEL *v = GC->rtcc->pLM;

	if (v == NULL) return;

	double lat, lng, rad;
	v->GetEquPos(lng, lat, rad);

	GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST] = lat;
	GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST] = lng;
	GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST] = rad;
}

void ARCore::CSMLSUplinkCalc()
{
	GC->rtcc->CMMCMCLS(RTCC_MPT_CSM);
}

void ARCore::LMLSUplinkCalc()
{
	GC->rtcc->CMMCMCLS(RTCC_MPT_LM);
}

void ARCore::CSMLandingSiteUplink()
{
	for (int i = 0;i < 010;i++)
	{
		g_Data.emem[i] = GC->rtcc->CZLSVECT.CSMLSUpdate.Octals[i];
	}

	UplinkData(true);
}

void ARCore::LMLandingSiteUplink()
{
	for (int i = 0;i < 010;i++)
	{
		g_Data.emem[i] = GC->rtcc->CZLSVECT.LMLSUpdate.Octals[i];
	}

	UplinkData(false);
}

void ARCore::StateVectorCalc(int type)
{
	int uplveh, mptveh;

	if (type == 0 || type == 9)
	{
		uplveh = 1;
	}
	else
	{
		uplveh = 2;
	}

	VESSEL *v;
	if (type == 0 || type == 21)
	{
		mptveh = RTCC_MPT_CSM;
		v = GC->rtcc->pCSM;
	}
	else
	{
		mptveh = RTCC_MPT_LM;
		v = GC->rtcc->pLM;
	}

	//Check on v not being NULL is already in the calling function!

	if (GC->MissionPlanningActive)
	{
		double get;
		if (SVDesiredGET < 0)
		{
			get = GC->rtcc->GETfromGMT(GC->rtcc->RTCCPresentTimeGMT());
		}
		else
		{
			get = SVDesiredGET;
		}

		GC->rtcc->CMMCMNAV(uplveh, mptveh, get, 0); //TBD
	}
	else
	{
		EphemerisData sv0, sv1;
		sv0 = GC->rtcc->StateVectorCalcEphem(v);
		if (SVDesiredGET < 0)
		{
			sv1 = sv0;
		}
		else
		{
			sv1 = GC->rtcc->coast(sv0, SVDesiredGET - GC->rtcc->GETfromGMT(sv0.GMT), mptveh);
		}
		GC->rtcc->CMMCMNAV(uplveh, mptveh, sv1);
	}
}

void ARCore::AGSStateVectorCalc(bool IsCSM)
{
	AGSSVOpt opt;
	EphemerisData sv;
	VESSEL *v;

	if (IsCSM)
	{
		v = GC->rtcc->pCSM;
	}
	else
	{
		v = GC->rtcc->pLM;
	}

	if (v == NULL) return;

	sv = GC->rtcc->StateVectorCalcEphem(v);

	opt.csm = IsCSM;
	opt.REFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
	opt.sv = sv;
	opt.landed = v->GroundContact();

	GC->rtcc->AGSStateVectorPAD(opt, agssvpad);
}

void ARCore::StateVectorUplink(int type)
{
	int *SVOctals;
	bool isCSM;
	if (type == 0)
	{
		SVOctals = GC->rtcc->CZNAVGEN.CMCCSMUpdate.Octals;
		isCSM = true;
	}
	else if (type == 9)
	{
		SVOctals = GC->rtcc->CZNAVGEN.CMCLEMUpdate.Octals;
		isCSM = true;
	}
	else if (type == 21)
	{
		SVOctals = GC->rtcc->CZNAVGEN.LGCCSMUpdate.Octals;
		isCSM = false;
	}
	else
	{
		SVOctals = GC->rtcc->CZNAVGEN.LGCLEMUpdate.Octals;
		isCSM = false;
	}

	for (int i = 0;i < 021;i++)
	{
		g_Data.emem[i] = SVOctals[i];
	}

	UplinkData(isCSM);
}


void ARCore::send_agc_key(char key, bool isCSM)
{
	int bytesXmit = SOCKET_ERROR;
	unsigned char cmdbuf[4];

	if (isCSM == false){
		cmdbuf[0] = 031; // VA,SA for LEM
	}
	else{
		cmdbuf[0] = 043; // VA,SA for CM
	}

	switch (key) {
	case 'V': // 11-000-101 11-010-001										
		cmdbuf[1] = 0305;
		cmdbuf[2] = 0321;
		break;
	case 'N': // 11-111-100 00-011-111
		cmdbuf[1] = 0374;
		cmdbuf[2] = 0037;
		break;
	case 'E': // 11-110-000 01-111-100
		cmdbuf[1] = 0360;
		cmdbuf[2] = 0174;
		break;
	case 'R': // 11-001-001 10-110-010
		cmdbuf[1] = 0311;
		cmdbuf[2] = 0262;
		break;
	case 'C': // 11-111-000 00-111-110
		cmdbuf[1] = 0370;
		cmdbuf[2] = 0076;
		break;
	case 'K': // 11-100-100 11-011-001
		cmdbuf[1] = 0344;
		cmdbuf[2] = 0331;
		break;
	case '+': // 11-101-000 10-111-010
		cmdbuf[1] = 0350;
		cmdbuf[2] = 0272;
		break;
	case '-': // 11-101-100 10-011-011
		cmdbuf[1] = 0354;
		cmdbuf[2] = 0233;
		break;
	case '1': // 10-000-111 11-000-001
		cmdbuf[1] = 0207;
		cmdbuf[2] = 0301;
		break;
	case '2': // 10-001-011 10-100-010
		cmdbuf[1] = 0213;
		cmdbuf[2] = 0242;
		break;
	case '3': // 10-001-111 10-000-011
		cmdbuf[1] = 0217;
		cmdbuf[2] = 0203;
		break;
	case '4': // 10-010-011 01-100-100
		cmdbuf[1] = 0223;
		cmdbuf[2] = 0144;
		break;
	case '5': // 10-010-111 01-000-101
		cmdbuf[1] = 0227;
		cmdbuf[2] = 0105;
		break;
	case '6': // 10-011-011 00-100-110
		cmdbuf[1] = 0233;
		cmdbuf[2] = 0046;
		break;
	case '7': // 10-011-111 00-000-111
		cmdbuf[1] = 0237;
		cmdbuf[2] = 0007;
		break;
	case '8': // 10-100-010 11-101-000
		cmdbuf[1] = 0242;
		cmdbuf[2] = 0350;
		break;
	case '9': // 10-100-110 11-001-001
		cmdbuf[1] = 0246;
		cmdbuf[2] = 0311;
		break;
	case '0': // 11-000-001 11-110-000
		cmdbuf[1] = 0301;
		cmdbuf[2] = 0360;
		break;
	case 'S': // 11-001-101 10-010-011 (code 23)
		cmdbuf[1] = 0315;
		cmdbuf[2] = 0223;
		break;
	case 'T': // 11-010-001 01-110-100 (code 24)
		cmdbuf[1] = 0321;
		cmdbuf[2] = 0164;
		break;
	}
	for (int i = 0; i < 3; i++) {
		g_Data.uplinkBuffer.push(cmdbuf[i]);
	}
}

void ARCore::REFSMMATUplink(bool isCSM)
{
	for (int i = 0; i < 20; i++)
	{
		if (isCSM)
		{
			g_Data.emem[i] = GC->rtcc->CZREFMAT.Block[0].Octals[i];
		}
		else
		{
			g_Data.emem[i] = GC->rtcc->CZREFMAT.Block[1].Octals[i];
		}
	}
	UplinkData(isCSM);
}

void ARCore::P30UplinkCalc(bool isCSM)
{
	if (isCSM)
	{
		GC->rtcc->CMMAXTDV(P30TIG, dV_LVLH);
	}
	else
	{
		GC->rtcc->CMMLXTDV(P30TIG, dV_LVLH);
	}
}

void ARCore::P30Uplink(bool isCSM)
{
	int *P30Octals;
	if (isCSM)
	{
		P30Octals = GC->rtcc->CZAXTRDV.Octals;
	}
	else
	{
		P30Octals = GC->rtcc->CZLXTRDV.Octals;
	}
	for (int i = 0;i < 012;i++)
	{
		g_Data.emem[i] = P30Octals[i];
	}

	UplinkData(isCSM);
}

void ARCore::RetrofireEXDVUplinkCalc(char source, char column)
{
	int s, c;
	if (source == 'T')
	{
		s = 1;
	}
	else if (source == 'R')
	{
		s = 2;
	}
	else
	{
		return;
	}

	if (column == 'P')
	{
		c = 1;
	}
	else if (column == 'M')
	{
		c = 2;
	}
	else
	{
		return;
	}

	GC->rtcc->CMMRXTDV(s, c);
}

void ARCore::RetrofireEXDVUplink()
{
	for (int i = 0;i < 016;i++)
	{
		g_Data.emem[i] = GC->rtcc->CZREXTDV.Octals[i];
	}

	UplinkData(true);
}

void ARCore::EntryUplinkCalc()
{
	GC->rtcc->CMMENTRY(GC->rtcc->RZDBSC1.lat_T, GC->rtcc->RZDBSC1.lng_T);
}

void ARCore::EntryUpdateUplink()
{
	for (int i = 0;i < 6;i++)
	{
		g_Data.emem[i] = GC->rtcc->CZENTRY.Octals[i];
	}

	UplinkData(true);
}

void ARCore::TLANDUplinkCalc(void)
{
	GC->rtcc->CMMDTGTU(GC->rtcc->CZTDTGTU.GETTD);
}

void ARCore::TLANDUplink(void)
{
	for (int i = 0;i < 5;i++)
	{
		g_Data.emem[i] = GC->rtcc->CZTDTGTU.Octals[i];
	}

	UplinkData2(false); // Go for uplink
}


void ARCore::AGCClockIncrementUplink(bool csm)
{
	RTCC::AGCTimeIncrementMakeupTableBlock *block;

	if (csm)
	{
		block = &GC->rtcc->CZTMEINC.Blocks[0];
	}
	else
	{
		block = &GC->rtcc->CZTMEINC.Blocks[1];
	}

	for (int i = 0;i < 2;i++)
	{
		g_Data.emem[i] = block->Octals[i];
	}

	UplinkDataV70V73(false, csm);
}

void ARCore::AGCLiftoffTimeIncrementUplink(bool csm)
{
	RTCC::AGCLiftoffTimeUpdateMakeupTableBlock *block;

	if (csm)
	{
		block = &GC->rtcc->CZLIFTFF.Blocks[0];
	}
	else
	{
		block = &GC->rtcc->CZLIFTFF.Blocks[1];
	}

	for (int i = 0;i < 2;i++)
	{
		g_Data.emem[i] = block->Octals[i];
	}

	UplinkDataV70V73(true, csm);
}

void ARCore::ErasableMemoryFileRead()
{
	//Read description etc. from the file

	EMPDescription = EMPRope = EMPErrorMessage = "";
	EMPUplinkMaxNumber = 0;

	std::ifstream file;
	std::string line;

	file.open(".\\Config\\ProjectApollo\\RTCC\\EMPs\\" + EMPFile + ".txt");

	if (file.is_open() == false)
	{
		EMPErrorMessage = "Error: File not available";
		return;
	}

	//Get description
	std::getline(file, line);
	EMPDescription = line;

	//Get rope name
	std::getline(file, line);
	EMPRope = line;

	//Read remaining number of lines

	int num = 0;

	while (std::getline(file, line))
	{
		num++;
	}

	if (num % 2 != 0)
	{
		EMPErrorMessage = "Error: Invalid loads";
		return;
	}
	EMPUplinkMaxNumber = num / 2;
}

void ARCore::ErasableMemoryFileLoad(int blocknum)
{
	//Read actual load

	EMPErrorMessage = "";

	std::ifstream file;
	std::string line;

	if (EMPUplinkNumber <= 0) return;

	file.open(".\\Config\\ProjectApollo\\RTCC\\EMPs\\" + EMPFile + ".txt");

	if (file.is_open() == false)
	{
		EMPErrorMessage = "Error: File not available";
		return;
	}

	//Skip two lines
	std::getline(file, line);
	std::getline(file, line);

	int linenum = EMPUplinkNumber * 2 - 1;
	int num = 0;

	while (std::getline(file, line))
	{
		num++;

		//Found desired line?
		if (num == linenum) break;
	}
	if (num != linenum)
	{
		EMPErrorMessage = "Error: Load not available";
		file.close();
		return;
	}

	std::vector<int> data;
	int verb, address;

	num = sscanf(line.c_str(), "%o %o", &verb, &address);

	if (verb == 071)
	{
		if (num != 2)
		{
			EMPErrorMessage = "Error: Invalid load";
			file.close();
			return;
		}
		data.push_back(address);
	}
	else if (verb == 072)
	{
		if (num != 1)
		{
			EMPErrorMessage = "Error: Invalid load";
			file.close();
			return;
		}
		data.push_back(0);
	}
	else return;

	//Now look for the data
	if (!std::getline(file, line))
	{
		EMPErrorMessage = "Error: Invalid load";
		file.close();
		return;
	}

	file.close();

	//Initialize uplink
	GC->rtcc->CMMERMEM(blocknum, 0, 0, data);
	data.clear();

	//Get data
	int datatab[18];
	num = sscanf(line.c_str(), "%o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o", &datatab[0], &datatab[1], &datatab[2], &datatab[3], &datatab[4], &datatab[5], &datatab[6], &datatab[7],
		&datatab[8], &datatab[9], &datatab[10], &datatab[11], &datatab[12], &datatab[13], &datatab[14], &datatab[15], &datatab[16], &datatab[17]);

	if (num == 0)
	{
		EMPErrorMessage = "Error: Invalid load";
		return;
	}

	for (int i = 0; i < num; i++)
	{
		data.push_back(datatab[i]);
	}

	int ident;

	if (verb == 071)
	{
		ident = 03;
	}
	else
	{
		ident = 02;
	}
	
	//Call to enter octal data
	GC->rtcc->CMMERMEM(blocknum, 2, ident, data);
}

void ARCore::ErasableMemoryUpdateUplink(int blocknum)
{
	char Buff[128];
	int i, emem[24];
	bool IsCMC;

	for (i = 0; i < 24; i++)
	{
		emem[i] = 0;
	}

	if (blocknum <= 1)
	{
		IsCMC = true;
	}
	else
	{
		IsCMC = false;
	}

	RTCC::AGCErasableMemoryUpdateMakeupBlock *block = &GC->rtcc->CZERAMEM.Blocks[blocknum];

	if (block->Data[0].EndOfDataFlag) return;

	emem[0] = block->Index;
	for (i = 0; i < 19; i++)
	{
		emem[i + 1] = block->Data[i].OctalData;
		if (block->Data[i].EndOfDataFlag) break;
	}

	//Bad octal/decimal conversion
	for (i = 0; i < emem[0]; i++)
	{
		sprintf(Buff, "%o", emem[i]);
		sscanf(Buff, "%d", &g_Data.emem[i]);
	}

	if (block->IsVerb72)
	{
		UplinkData2(IsCMC);
	}
	else
	{
		UplinkData(IsCMC);
	}
}

void ARCore::AP11AbortCoefUplink()
{
	g_Data.emem[0] = 22;
	g_Data.emem[1] = 2550;
	g_Data.emem[2] = OrbMech::DoubleToBuffer(PDAPABTCOF[0] * pow(100.0, -4)*pow(2, 44), 0, 1);
	g_Data.emem[3] = OrbMech::DoubleToBuffer(PDAPABTCOF[0] * pow(100.0, -4)*pow(2, 44), 0, 0);
	g_Data.emem[4] = OrbMech::DoubleToBuffer(PDAPABTCOF[1] * pow(100.0, -3)*pow(2, 27), 0, 1);
	g_Data.emem[5] = OrbMech::DoubleToBuffer(PDAPABTCOF[1] * pow(100.0, -3)*pow(2, 27), 0, 0);
	g_Data.emem[6] = OrbMech::DoubleToBuffer(PDAPABTCOF[2] * pow(100.0, -2)*pow(2, 10), 0, 1);
	g_Data.emem[7] = OrbMech::DoubleToBuffer(PDAPABTCOF[2] * pow(100.0, -2)*pow(2, 10), 0, 0);
	g_Data.emem[8] = OrbMech::DoubleToBuffer(PDAPABTCOF[3] * pow(100.0, -1), 7, 1);
	g_Data.emem[9] = OrbMech::DoubleToBuffer(PDAPABTCOF[3] * pow(100.0, -1), 7, 0);
	g_Data.emem[10] = OrbMech::DoubleToBuffer(PDAPABTCOF[4] * pow(100.0, -4)*pow(2, 44), 0, 1);
	g_Data.emem[11] = OrbMech::DoubleToBuffer(PDAPABTCOF[4] * pow(100.0, -4)*pow(2, 44), 0, 0);
	g_Data.emem[12] = OrbMech::DoubleToBuffer(PDAPABTCOF[5] * pow(100.0, -3)*pow(2, 27), 0, 1);
	g_Data.emem[13] = OrbMech::DoubleToBuffer(PDAPABTCOF[5] * pow(100.0, -3)*pow(2, 27), 0, 0);
	g_Data.emem[14] = OrbMech::DoubleToBuffer(PDAPABTCOF[6] * pow(100.0, -2)*pow(2, 10), 0, 1);
	g_Data.emem[15] = OrbMech::DoubleToBuffer(PDAPABTCOF[6] * pow(100.0, -2)*pow(2, 10), 0, 0);
	g_Data.emem[16] = OrbMech::DoubleToBuffer(PDAPABTCOF[7] * pow(100.0, -1), 7, 1);
	g_Data.emem[17] = OrbMech::DoubleToBuffer(PDAPABTCOF[7] * pow(100.0, -1), 7, 0);

	UplinkData(false); // Go for uplink
}

void ARCore::AP12AbortCoefUplink()
{
	g_Data.emem[0] = 16;
	g_Data.emem[1] = GC->rtcc->SystemParameters.MCLABT;
	g_Data.emem[2] = OrbMech::DoubleToBuffer(PDAP_J1, 23, 1);
	g_Data.emem[3] = OrbMech::DoubleToBuffer(PDAP_J1, 23, 0);
	g_Data.emem[4] = OrbMech::DoubleToBuffer(PDAP_K1*PI2, 23, 1);
	g_Data.emem[5] = OrbMech::DoubleToBuffer(PDAP_K1*PI2, 23, 0);
	g_Data.emem[6] = OrbMech::DoubleToBuffer(PDAP_J2, 23, 1);
	g_Data.emem[7] = OrbMech::DoubleToBuffer(PDAP_J2, 23, 0);
	g_Data.emem[8] = OrbMech::DoubleToBuffer(PDAP_K2*PI2, 23, 1);
	g_Data.emem[9] = OrbMech::DoubleToBuffer(PDAP_K2*PI2, 23, 0);
	g_Data.emem[10] = OrbMech::DoubleToBuffer(PDAP_Theta_LIM / PI2, 0, 1);
	g_Data.emem[11] = OrbMech::DoubleToBuffer(PDAP_Theta_LIM / PI2, 0, 0);
	g_Data.emem[12] = OrbMech::DoubleToBuffer(PDAP_R_amin, 24, 1);
	g_Data.emem[13] = OrbMech::DoubleToBuffer(PDAP_R_amin, 24, 0);

	UplinkData(false); // Go for uplink
}

void ARCore::UplinkData(bool isCSM)
{
	if (g_Data.connStatus == 0) {
		int bytesRecv = SOCKET_ERROR;
		char addr[256];
		char buffer[8];
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == INVALID_SOCKET) {
			//g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "ERROR AT SOCKET(): %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(addr, "127.0.0.1");
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr);
		if (isCSM)
		{
			clientService.sin_port = htons(14242);
		}
		else
		{
			clientService.sin_port = htons(14243);
		}
		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
			//g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "FAILED TO CONNECT, ERROR %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "CONNECTED");
		g_Data.uplinkState = 0;
		send_agc_key('V', isCSM);
		send_agc_key('7', isCSM);
		send_agc_key('1', isCSM);
		send_agc_key('E', isCSM);

		int cnt2 = (g_Data.emem[0] / 10);
		int cnt = (g_Data.emem[0] - (cnt2 * 10)) + cnt2 * 8;

		while (g_Data.uplinkState < cnt && cnt <= 20 && cnt >= 3)
			{
				sprintf(buffer, "%ld", g_Data.emem[g_Data.uplinkState]);
				uplink_word(buffer, isCSM);
				g_Data.uplinkState++;
			}
		send_agc_key('V', isCSM);
		send_agc_key('3', isCSM);
		send_agc_key('3', isCSM);
		send_agc_key('E', isCSM);
		g_Data.connStatus = 1;
		g_Data.uplinkState = 0;
		//.uplinkBufferSimt = oapiGetSimTime() + 5.0; //5 second delay
	}
}

void ARCore::UplinkData2(bool isCSM)
{
	if (g_Data.connStatus == 0) {
		int bytesRecv = SOCKET_ERROR;
		char addr[256];
		char buffer[8];
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == INVALID_SOCKET) {
			//g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "ERROR AT SOCKET(): %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(addr, "127.0.0.1");
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr);
		if (isCSM)
		{
			clientService.sin_port = htons(14242);
		}
		else
		{
			clientService.sin_port = htons(14243);
		}
		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
			//g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "FAILED TO CONNECT, ERROR %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "CONNECTED");
		g_Data.uplinkState = 0;
		send_agc_key('V', isCSM);
		send_agc_key('7', isCSM);
		send_agc_key('2', isCSM);
		send_agc_key('E', isCSM);

		int cnt2 = (g_Data.emem[0] / 10);
		int cnt = (g_Data.emem[0] - (cnt2 * 10)) + cnt2 * 8;

		while (g_Data.uplinkState < cnt && cnt <= 20 && cnt >= 3)
		{
			sprintf(buffer, "%ld", g_Data.emem[g_Data.uplinkState]);
			uplink_word(buffer, isCSM);
			g_Data.uplinkState++;
		}
		send_agc_key('V', isCSM);
		send_agc_key('3', isCSM);
		send_agc_key('3', isCSM);
		send_agc_key('E', isCSM);
		g_Data.connStatus = 1;
		g_Data.uplinkState = 0;
		//g_Data.uplinkBufferSimt = oapiGetSimTime() + 5.0; //6 second delay
	}
}

void ARCore::UplinkDataV70V73(bool v70, bool isCSM)
{
	if (g_Data.connStatus == 0) {
		int bytesRecv = SOCKET_ERROR;
		char addr[256];
		char buffer[8];
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == INVALID_SOCKET) {
			//g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "ERROR AT SOCKET(): %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(addr, "127.0.0.1");
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr);
		if (isCSM)
		{
			clientService.sin_port = htons(14242);
		}
		else
		{
			clientService.sin_port = htons(14243);
		}
		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
			//g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "FAILED TO CONNECT, ERROR %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "CONNECTED");
		g_Data.uplinkState = 0;
		send_agc_key('V', isCSM);
		send_agc_key('7', isCSM);
		if (v70)
		{
			send_agc_key('0', isCSM);
		}
		else
		{
			send_agc_key('3', isCSM);
		}
		send_agc_key('E', isCSM);

		while (g_Data.uplinkState < 2)
		{
			sprintf(buffer, "%ld", g_Data.emem[g_Data.uplinkState]);
			uplink_word(buffer, isCSM);
			g_Data.uplinkState++;
		}
		send_agc_key('V', isCSM);
		send_agc_key('3', isCSM);
		send_agc_key('3', isCSM);
		send_agc_key('E', isCSM);
		g_Data.connStatus = 1;
		g_Data.uplinkState = 0;
		//g_Data.uplinkBufferSimt = oapiGetSimTime() + 5.0; //6 second delay
	}
}

void ARCore::uplink_word(char *data, bool isCSM)
{
	int i;
	for (i = 5; i > (int)strlen(data); i--) {
		send_agc_key('0', isCSM);
	}
	for (i = 0; i < (int)strlen(data); i++) {
		send_agc_key(data[i], isCSM);
	}
	send_agc_key('E', isCSM);
}

void ARCore::VecPointCalc(bool IsCSM)
{
	if (VECoption == 0)
	{
		if (VECbody == NULL) return;

		VESSEL *v;
		if (IsCSM)
		{
			v = GC->rtcc->pCSM;
		}
		else
		{
			v = GC->rtcc->pLM;
		}

		if (v == NULL) return;

		EphemerisData sv;
		MATRIX3 MAT, REFSMMAT;
		VECTOR3 POINTVSM, vPos, pPos, dPos, U_LOS, UNITY, SCAXIS, UTSA, UTSB, UTSAP, UTSBP;
		double UTPIT, UTYAW, OMICRON;

		if (IsCSM)
		{
			REFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
		}
		else
		{
			REFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
		}

		switch (VECdirection)
		{
		case 0: //+X
			UTYAW = 0;
			UTPIT = 0;
			break;
		case 1: //-X
			UTYAW = 0;
			UTPIT = PI;
			break;
		case 2: //Optics
			UTYAW = 0;
			UTPIT = -(PI05 - 0.5676353234);
			break;
		case 3: //SIM Bay
			UTYAW = PI05;
			UTPIT = 52.25*RAD;
			break;
		default: //Selectable
			UTYAW = VECBodyVector.x;
			UTPIT = VECBodyVector.y;
			break;
		}

		//State vector used in calculation
		sv = GC->rtcc->StateVectorCalcEphem(v);
		POINTVSM = unit(crossp(sv.V, sv.R));

		//Pointing vector
		v->GetGlobalPos(vPos);
		oapiGetGlobalPos(VECbody, &pPos);
		dPos = pPos - vPos;
		dPos = mul(GC->rtcc->SystemParameters.MAT_J2000_BRCS, _V(dPos.x, dPos.z, dPos.y));
		U_LOS = unit(dPos);

		//Artemis calculations
		OMICRON = VECBodyVector.z;
		UNITY = _V(0, 1, 0);
		SCAXIS = _V(cos(UTYAW)*cos(UTPIT), sin(UTYAW)*cos(UTPIT), -sin(UTPIT));

		UTSAP = crossp(SCAXIS, UNITY);
		if (length(UTSAP) == 0.0) return;
		UTSAP = unit(UTSAP);

		POINTVSM = unit(crossp(U_LOS, POINTVSM));

		UTSA = POINTVSM * cos(OMICRON) + unit(crossp(U_LOS, POINTVSM))*sin(OMICRON);
		UTSB = U_LOS;
		UTSBP = SCAXIS;

		MAT = OrbMech::AXISGEN(UTSAP, UTSBP, UTSA, UTSB);
		VECangles = OrbMech::CALCGAR(REFSMMAT, MAT);
	}
	else if (VECoption == 1)
	{
		VECangles = GC->rtcc->HatchOpenThermalControl(GC->rtcc->RTCCPresentTimeGMT(), GC->rtcc->EZJGMTX1.data[0].REFSMMAT);
	}
}

void ARCore::TerrainModelCalc()
{
	MATRIX3 Rot3, Rot4;
	VECTOR3 R_P, UX10, UY10, UZ10, axis, R_loc;
	double ang, r_0, anginc, dist, lat, lng, alt;
	OBJHANDLE hMoon;

	hMoon = oapiGetObjectByName("Moon");
	ang = 0.0;
	dist = 0.0;

	R_P = unit(_V(cos(TMLng)*cos(TMLat), sin(TMLng)*cos(TMLat), sin(TMLat)));

	TMAlt = oapiSurfaceElevation(hMoon, TMLng, TMLat);
	r_0 = TMAlt + oapiGetSize(hMoon);
	anginc = TMStepSize / r_0;

	UX10 = R_P;
	UY10 = unit(crossp(_V(0.0, 0.0, 1.0), UX10));
	UZ10 = crossp(UX10, UY10);

	Rot3 = _M(UX10.x, UX10.y, UX10.z, UY10.x, UY10.y, UY10.z, UZ10.x, UZ10.y, UZ10.z);
	Rot4 = _M(1.0, 0.0, 0.0, 0.0, cos(TMAzi), -sin(TMAzi), 0.0, sin(TMAzi), cos(TMAzi));

	axis = mul(OrbMech::tmat(Rot3), mul(Rot4, _V(0.0, 1.0, 0.0)));


	FILE *file = fopen("TerrainModel.txt", "w");

	fprintf(file, "Lunar Terrain Model\n");
	fprintf(file, "Lat: %.4lf deg;Lng: %.4lf deg;Elev: %.0lf m;Azimuth: %.2lf deg\n", TMLat*DEG, TMLng*DEG, TMAlt, TMAzi*DEG);
	fprintf(file, "Distance in meters;Elevation in meters\n");
	fprintf(file, "%f;%f\n", -dist, 0.0);

	while (dist < TMDistance)
	{
		ang += anginc;
		dist += TMStepSize;

		R_loc = OrbMech::RotateVector(axis, -ang, R_P);
		R_loc = unit(R_loc);

		lat = atan2(R_loc.z, sqrt(R_loc.x*R_loc.x + R_loc.y*R_loc.y));
		lng = atan2(R_loc.y, R_loc.x);

		alt = oapiSurfaceElevation(hMoon, lng, lat);

		fprintf(file, "%f;%f\n", -dist, alt - TMAlt);
	}

	if (file) fclose(file);
}

void ARCore::NodeConvCalc()
{
	MATRIX3 Rot;
	VECTOR3 R_EMP, R_selen;


	//Get rotation matrix from MCT to EMP
	int err = GC->rtcc->ELVCNV(GC->rtcc->GMTfromGET(NodeConvGET), 3, 4, Rot);

	if (err) return;

	if (NodeConvOpt)
	{
		R_selen = OrbMech::r_from_latlong(NodeConvLat, NodeConvLng);
		R_EMP = mul(Rot, R_selen);
		OrbMech::latlong_from_r(R_EMP, NodeConvResLat, NodeConvResLng);
	}
	else
	{
		R_EMP = OrbMech::r_from_latlong(NodeConvLat, NodeConvLng);
		R_selen = tmul(Rot, R_EMP);
		OrbMech::latlong_from_r(R_selen, NodeConvResLat, NodeConvResLng);
	}
	if (NodeConvResLng < 0)
	{
		NodeConvResLng += PI2;
	}
}

void ARCore::SendNodeToSFP()
{
	//If SFP block 2 hasn't been generated yet, copy it over from block 1 and then write the nodal target to it
	if (GC->rtcc->PZSFPTAB.blocks[1].GMTTimeFlag == 0.0)
	{
		GC->rtcc->PZSFPTAB.blocks[1] = GC->rtcc->PZSFPTAB.blocks[0];
	}
	GC->rtcc->PZSFPTAB.blocks[1].GMT_nd = GC->rtcc->GMTfromGET(NodeConvGET);
	GC->rtcc->PZSFPTAB.blocks[1].lat_nd = NodeConvResLat;
	GC->rtcc->PZSFPTAB.blocks[1].lng_nd = NodeConvResLng;
	GC->rtcc->PZSFPTAB.blocks[1].h_nd = NodeConvHeight;
}

int ARCore::startSubthread(int fcn, bool IsCSM) {
	if (IsReady(subThreadStatus)) {
		// Punt thread
		subThreadMode = fcn;
		subThreadStatus = SCHEDULED;
		IsCSMCalculation = IsCSM;
		subThreadWorker.Start([this] { subThread(); });
	}
	else {
		subThreadWorker.Kill();
		subThreadStatus = DONE;
		return(-1);
	}
	return(0);
}

int ARCore::subThread()
{
	ThreadStatus Result = DONE;

	subThreadStatus = RUNNING;
	switch (subThreadMode) {
	case 0: // Test
		Sleep(5000); // Waste 5 seconds
		Result = DONE;  // Success (negative = error)
		break;
	case 1: //Lambert Targeting
	{
		TwoImpulseOpt opt;
		TwoImpulseResuls res;
		EphemerisData sv_A, sv_P;

		if (GC->MissionPlanningActive)
		{
			EphemerisData EPHEM;

			double GMT;
			
			if (GC->rtcc->med_k30.ChaserVectorTime > 0)
			{
				GMT = GC->rtcc->GMTfromGET(GC->rtcc->med_k30.ChaserVectorTime);
			}
			else
			{
				GMT = GC->rtcc->RTCCPresentTimeGMT();
			}

			if (GC->rtcc->EMSFFV(GMT, GC->rtcc->med_k30.Vehicle, EPHEM))
			{
				Result = DONE;
				break;
			}

			sv_A = EPHEM;

			if (GC->rtcc->med_k30.TargetVectorTime > 0)
			{
				GMT = GC->rtcc->GMTfromGET(GC->rtcc->med_k30.TargetVectorTime);
			}
			else
			{
				GMT = GC->rtcc->RTCCPresentTimeGMT();
			}

			if (GC->rtcc->EMSFFV(GMT, 4 - GC->rtcc->med_k30.Vehicle, EPHEM))
			{
				Result = DONE;
				break;
			}
			sv_P = EPHEM;
		}
		else
		{
			if (GC->rtcc->pCSM == NULL || GC->rtcc->pLM == NULL)
			{
				Result = DONE;
				break;
			}

			VESSEL *chaser, *tgt;
			if (GC->rtcc->med_k30.Vehicle == 1)
			{
				chaser = GC->rtcc->pCSM;
				tgt = GC->rtcc->pLM;
			}
			else
			{
				chaser = GC->rtcc->pLM;
				tgt = GC->rtcc->pCSM;
			}

			sv_A = GC->rtcc->StateVectorCalcEphem(chaser);
			sv_P = GC->rtcc->StateVectorCalcEphem(tgt);
		}

		opt.mode = 2;

		if (GC->rtcc->med_k30.StartTime < 0)
		{
			opt.T1 = -1;
		}
		else
		{
			opt.T1 = GC->rtcc->GMTfromGET(GC->rtcc->med_k30.StartTime);
		}
		if (GC->rtcc->med_k30.EndTime < 0)
		{
			opt.T2 = -1;
		}
		else
		{
			opt.T2 = GC->rtcc->GMTfromGET(GC->rtcc->med_k30.EndTime);
		}
		
		opt.TimeStep = GC->rtcc->med_k30.TimeStep;
		opt.TimeRange = GC->rtcc->med_k30.TimeRange;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.IVFLAG = GC->rtcc->med_k30.IVFlag;
		opt.ChaserVehicle = GC->rtcc->med_k30.Vehicle;

		GC->rtcc->PMSTICN(opt, res);

		Result = DONE;
	}
	break;
	case 2:	//Concentric Rendezvous Processor
	{
		SPQOpt opt;
		SPQResults res;
		SV sv_A, sv_P, sv_pre, sv_post;

		if (GC->MissionPlanningActive)
		{
			int err;
			double GMT_C, GMT_T;

			if (GC->rtcc->med_k01.ChaserThresholdGET < 0)
			{
				GMT_C = GC->rtcc->RTCCPresentTimeGMT();
			}
			else
			{
				GMT_C = GC->rtcc->GMTfromGET(GC->rtcc->med_k01.ChaserThresholdGET);
			}
			if (GC->rtcc->med_k01.TargetThresholdGET < 0)
			{
				GMT_T = GC->rtcc->RTCCPresentTimeGMT();
			}
			else
			{
				GMT_T = GC->rtcc->GMTfromGET(GC->rtcc->med_k01.TargetThresholdGET);
			}

			EphemerisData EPHEM;
			err = GC->rtcc->EMSFFV(GMT_C, GC->rtcc->med_k01.ChaserVehicle, EPHEM);
			if (err)
			{
				Result = DONE;
				break;
			}
			sv_A.R = EPHEM.R;
			sv_A.V = EPHEM.V;
			sv_A.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv_A.gravref = GC->rtcc->GetGravref(EPHEM.RBI);

			err = GC->rtcc->EMSFFV(GMT_T, 4 - GC->rtcc->med_k01.ChaserVehicle, EPHEM);
			if (err)
			{
				Result = DONE;
				break;
			}
			sv_P.R = EPHEM.R;
			sv_P.V = EPHEM.V;
			sv_P.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv_P.gravref = GC->rtcc->GetGravref(EPHEM.RBI);
		}
		else
		{
			if (GC->rtcc->pCSM == NULL || GC->rtcc->pLM == NULL)
			{
				Result = DONE;
				break;
			}

			VESSEL *chaser, *tgt;
			if (GC->rtcc->med_k01.ChaserVehicle == RTCC_MPT_CSM)
			{
				chaser = GC->rtcc->pCSM;
				tgt = GC->rtcc->pLM;
			}
			else
			{
				chaser = GC->rtcc->pLM;
				tgt = GC->rtcc->pCSM;
			}

			sv_A = GC->rtcc->StateVectorCalc(chaser);
			sv_P = GC->rtcc->StateVectorCalc(tgt);
		}

		opt.DH = GC->rtcc->GZGENCSN.SPQDeltaH;
		opt.E = GC->rtcc->GZGENCSN.SPQElevationAngle;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.WT = GC->rtcc->GZGENCSN.SPQTerminalPhaseAngle;
		opt.ChaserID = GC->rtcc->med_k01.ChaserVehicle;

		if (SPQMode != 1)
		{
			opt.t_CSI = CSItime;
			
			if (SPQMode == 2)
			{
				opt.K_CDH = 1;
				opt.OptimumCSI = true;
			}
			else
			{
				opt.K_CDH = CDHtimemode;
				opt.OptimumCSI = false;
			}
		}
		else
		{
			opt.t_CSI = -1;
			if (CDHtimemode == 0)
			{
				opt.t_CDH = CDHtime;
			}
			else
			{
				opt.t_CDH = GC->rtcc->FindDH(sv_A, sv_P, CDHtime, GC->rtcc->GZGENCSN.SPQDeltaH);
			}
		}
		opt.t_TPI = GC->rtcc->GZGENCSN.TPIDefinitionValue;
		opt.I_CDH = GC->rtcc->med_k01.I_CDH;
		opt.DU_D = GC->rtcc->med_k01.CDH_Angle;

		GC->rtcc->PMMDKI(opt, res);

		if (SPQMode != 1)
		{
			SPQTIG = res.t_CSI;
		}
		else
		{
			SPQTIG = res.t_CDH;
		}

		if (SPQMode != 1)
		{
			CDHtime = res.t_CDH;
			SPQDeltaV = res.dV_CSI;
		}
		else
		{
			SPQDeltaV = res.dV_CDH;
		}

		Result = DONE;
	}
	break;
	case 3:	//Orbital Adjustment Targeting
	{
		GMPOpt opt;
		EphemerisData sv0;
		SV sv_pre, sv_post;

		if (GC->MissionPlanningActive)
		{
			double GMT = GC->rtcc->GMTfromGET(SPSGET);
			if (GC->rtcc->EMSFFV(GMT, GC->rtcc->med_k20.Vehicle, sv0))
			{
				Result = DONE;
				break;
			}
		}
		else
		{
			VESSEL *v;
			if (GC->rtcc->med_k20.Vehicle == RTCC_MPT_CSM)
			{
				v = GC->rtcc->pCSM;
			}
			else
			{
				v = GC->rtcc->pLM;
			}

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			sv0 = GC->rtcc->StateVectorCalcEphem(v);
		}

		opt.ManeuverCode = GMPManeuverCode;
		opt.H_A = GMPApogeeHeight;
		opt.H_P = GMPPerigeeHeight;
		opt.dH_D = GMPHeightChange;
		opt.TIG_GET = SPSGET;
		opt.dLAN = GMPNodeShiftAngle;
		opt.dW = GMPWedgeAngle;
		opt.long_D = GMPManeuverLongitude;
		opt.H_D = GMPManeuverHeight;
		opt.dV = GMPDeltaVInput;
		opt.Pitch = GMPPitch;
		opt.Yaw = GMPYaw;
		opt.dLOA = GMPApseLineRotAngle;
		opt.N = GMPRevs;
		opt.sv_in = sv0;

		VECTOR3 OrbAdjDVX;
		double GPM_TIG;
		GC->rtcc->GeneralManeuverProcessor(&opt, OrbAdjDVX, GPM_TIG);

		Result = DONE;
	}
	break;
	case 4:	//REFSMMAT Calculation
	{
		REFSMMATOpt opt;
		int mptveh;

		if (IsCSMCalculation)
		{
			mptveh = RTCC_MPT_CSM;
		}
		else
		{
			mptveh = RTCC_MPT_LM;
		}

		opt.dV_LVLH = dV_LVLH;
		opt.LSAzi = GC->rtcc->med_k18.psi_DS*RAD;
		opt.LSLat = GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST];
		opt.LSLng = GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST];
		opt.REFSMMATopt = REFSMMATopt;

		if (REFSMMATopt == 0 || REFSMMATopt == 1)
		{
			opt.REFSMMATTime = P30TIG;
		}
		else if (REFSMMATopt == 2)
		{
			opt.REFSMMATTime = REFSMMAT_LVLH_Time;
		}
		else if (REFSMMATopt == 5 || REFSMMATopt == 8)
		{
			opt.REFSMMATTime = GC->rtcc->CZTDTGTU.GETTD;
		}
		else
		{
			opt.REFSMMATTime = GC->REFSMMAT_PTC_MJD;
		}

		//For LS REFSMMAT use CSM vessel
		if (GC->MissionPlanningActive == false && REFSMMATopt == 5)
		{
			opt.vessel = GC->rtcc->pCSM;
		}
		else
		{
			if (IsCSMCalculation)
			{
				opt.vessel = GC->rtcc->pCSM;
			}
			else
			{
				opt.vessel = GC->rtcc->pLM;
			}
		}

		//Error check on vessel
		if (opt.vessel == NULL)
		{
			Result = DONE;
			break;
		}

		if (IsCSMCalculation)
		{
			if (vesselisdocked)
			{
				opt.vesseltype = 1;
			}
			else
			{
				opt.vesseltype = 0;
			}
		}
		else
		{
			if (vesselisdocked)
			{
				opt.vesseltype = 3;
			}
			else
			{
				opt.vesseltype = 2;
			}
		}

		opt.HeadsUp = REFSMMATHeadsUp;
		if (IsCSMCalculation)
		{
			opt.PresentREFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
		}
		else
		{
			opt.PresentREFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
		}

		opt.IMUAngles = VECangles;
		opt.csmlmdocked = !GC->MissionPlanningActive && vesselisdocked;

		if (GC->MissionPlanningActive && GC->rtcc->MPTHasManeuvers(mptveh))
		{
			opt.useSV = true;

			if (REFSMMATopt == 0 || REFSMMATopt == 1 || REFSMMATopt == 2)
			{
				//SV at specified time
				double GMT = GC->rtcc->GMTfromGET(opt.REFSMMATTime);
				EphemerisData EPHEM;
				if (GC->rtcc->EMSFFV(GMT, mptveh, EPHEM))
				{
					Result = DONE;
					break;
				}
				opt.RV_MCC.R = EPHEM.R;
				opt.RV_MCC.V = EPHEM.V;
				opt.RV_MCC.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
				opt.RV_MCC.gravref = GC->rtcc->GetGravref(EPHEM.RBI);

				PLAWDTInput pin;
				PLAWDTOutput pout;
				pin.T_UP = GMT;
				pin.TableCode = mptveh;
				GC->rtcc->PLAWDT(pin, pout);
				opt.RV_MCC.mass = pout.ConfigWeight;
			}
			else if (REFSMMATopt == 3)
			{
				//Last SV in the table
				MissionPlanTable *tab;
				if (mptveh == RTCC_MPT_CSM)
				{
					tab = &GC->rtcc->PZMPTCSM;
				}
				else
				{
					tab = &GC->rtcc->PZMPTLEM;
				}

				opt.RV_MCC.R = tab->mantable.back().R_BO;
				opt.RV_MCC.V = tab->mantable.back().V_BO;
				opt.RV_MCC.MJD = OrbMech::MJDfromGET(tab->mantable.back().GMT_BO, GC->rtcc->GetGMTBase());
				opt.RV_MCC.gravref = GC->rtcc->GetGravref(tab->mantable.back().RefBodyInd);

				PLAWDTInput pin;
				PLAWDTOutput pout;
				pin.T_UP = tab->mantable.back().GMT_BO;
				pin.TableCode = mptveh;
				GC->rtcc->PLAWDT(pin, pout);
				opt.RV_MCC.mass = pout.ConfigWeight;
			}
			else if (REFSMMATopt == 5)
			{
				//Landing site
				double GMT = GC->rtcc->GMTfromGET(opt.REFSMMATTime);
				EphemerisData EPHEM;
				if (GC->rtcc->EMSFFV(GMT, RTCC_MPT_CSM, EPHEM))
				{
					Result = DONE;
					break;
				}

				opt.RV_MCC.R = EPHEM.R;
				opt.RV_MCC.V = EPHEM.V;
				opt.RV_MCC.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
				opt.RV_MCC.gravref = GC->rtcc->GetGravref(EPHEM.RBI);
			}
			else
			{
				opt.useSV = false;
			}
		}
		else
		{
			opt.useSV = false;
		}

		MATRIX3 REFSMMAT = GC->rtcc->REFSMMATCalc(&opt);
		if (IsCSMCalculation)
		{
			GC->rtcc->EMGSTSTM(1, REFSMMAT, RTCC_REFSMMAT_TYPE_CUR, GC->rtcc->RTCCPresentTimeGMT());
		}
		else
		{
			GC->rtcc->EMGSTSTM(3, REFSMMAT, RTCC_REFSMMAT_TYPE_CUR, GC->rtcc->RTCCPresentTimeGMT());
		}

		//sprintf(oapiDebugString(), "%f, %f, %f, %f, %f, %f, %f, %f, %f", REFSMMAT.m11, REFSMMAT.m12, REFSMMAT.m13, REFSMMAT.m21, REFSMMAT.m22, REFSMMAT.m23, REFSMMAT.m31, REFSMMAT.m32, REFSMMAT.m33);

		REFSMMATcur = REFSMMATopt;

		Result = DONE;
	}
	break;
	case 5: //LOI Targeting
	{
		EphemerisData sv0;

		if (GC->MissionPlanningActive)
		{
			double gmt;
			if (GC->rtcc->med_k18.VectorTime != 0.0)
			{
				gmt = GC->rtcc->GMTfromGET(GC->rtcc->med_k18.VectorTime);
			}
			else
			{
				gmt = GC->rtcc->RTCCPresentTimeGMT();
				GC->rtcc->med_k18.VectorTime = GC->rtcc->GETfromGMT(gmt);
			}

			if (GC->rtcc->EMSFFV(gmt, RTCC_MPT_CSM, sv0))
			{
				Result = DONE;
				break;
			}
		}
		else
		{
			VESSEL *v = GC->rtcc->pCSM;

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			sv0 =  GC->rtcc->StateVectorCalcEphem(v);
		}

		GC->rtcc->PMMLRBTI(sv0);

		Result = DONE;
	}
	break;
	case 6: //TPI PAD
	{
		if (GC->rtcc->pCSM == NULL || GC->rtcc->pLM == NULL)
		{
			Result = DONE;
			break;
		}

		AP7TPIPADOpt opt;

		opt.dV_LVLH = dV_LVLH;
		opt.TIG = P30TIG;
		opt.sv_A = GC->rtcc->StateVectorCalcEphem(GC->rtcc->pCSM);
		opt.sv_P = GC->rtcc->StateVectorCalcEphem(GC->rtcc->pLM);
		opt.mass = GC->rtcc->pCSM->GetMass();

		GC->rtcc->AP7TPIPAD(opt, GC->TPI_PAD);

		Result = DONE;
	}
	break;
	case 7:	//Return to Earth
	{
		if (GC->MissionPlanningActive)
		{
			GC->rtcc->GMGMED("F80;");
		}
		else
		{
			VESSEL *v = GC->rtcc->pCSM;

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			MED_M50 med1;
			MED_M55 med2;
			MED_M49 med3;
			//This doesn't work in debug mode (with only RTCC MFD and MCC modules build), so below are some fake masses
			GC->rtcc->MPTMassUpdate(v, med1, med2, med3);

			GC->rtcc->VEHDATABUF.csmmass = med1.CSMWT;//GC->rtcc->pCSM->GetMass();//
			GC->rtcc->VEHDATABUF.lmascmass = med1.LMASCWT;//0.0;10000.0*0.453;//
			GC->rtcc->VEHDATABUF.lmdscmass = med1.LMWT - med1.LMASCWT;//0.0;25000.0*0.453;//
			GC->rtcc->VEHDATABUF.sv = GC->rtcc->StateVectorCalcEphem(v);
			GC->rtcc->VEHDATABUF.config = med2.ConfigCode;//"CL";//"C";//

			GC->rtcc->PMMREDIG(false);
		}

		Result = DONE;
	}
	break;
	case 8: //TLI PAD
	{
		if (GC->rtcc->pCSM == NULL)
		{
			Result = DONE;
			break;
		}

		LVDCSV *lvdc = NULL;
		if (utils::IsVessel(GC->rtcc->pCSM, utils::SaturnV))
		{
			SaturnV * SatV = (SaturnV *)GC->rtcc->pCSM;
			if (SatV->iu)
			{
				lvdc = (LVDCSV*)SatV->iu->GetLVDC();
			}
		}
		if (lvdc == NULL)
		{
			Result = DONE;
			break;
		}

		TLIPADOpt opt;

		opt.ConfigMass = GC->rtcc->pCSM->GetMass();
		if (lvdc->first_op)
		{
			opt.InjOpp = 1;
		}
		else
		{
			opt.InjOpp = 2;
		}
		opt.REFSMMAT= GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
		opt.SeparationAttitude = lvdc->XLunarAttitude;
		opt.sv0 = GC->rtcc->StateVectorCalcEphem(GC->rtcc->pCSM);
		opt.StudyAid = TLIPAD_StudyAid;

		GC->rtcc->TLI_PAD(opt, GC->tlipad);

		Result = DONE;
	}
	break;
	case 9: //Maneuver PAD
	{
		EphemerisData sv_A;
		PLAWDTOutput WeightsTable;

		if (GC->MissionPlanningActive)
		{
			//Get pointer to MPT maneuver
			MissionPlanTable *mpt = GC->rtcc->GetMPTPointer(ManPADMPT);

			if (mpt == NULL)
			{
				Result = DONE;
				break;
			}

			unsigned num = (unsigned)(ManPADMPTManeuver - 1);

			//Does the maneuver exist?
			if (num >= mpt->mantable.size())
			{
				Result = DONE;
				break;
			}

			MPTManeuver *man = &mpt->mantable[num];

			if (IsCSMCalculation)
			{
				//Is CSM maneuver?
				if (man->TVC != RTCC_MPT_CSM)
				{
					Result = DONE;
					break;
				}
			}
			else
			{
				//Is LM maneuver?
				if (man->TVC != RTCC_MPT_LM)
				{
					Result = DONE;
					break;
				}
			}

			//Also only allow External DV burns
			if (man->AttitudeCode != RTCC_ATTITUDE_PGNS_EXDV)
			{
				Result = DONE;
				break;
			}

			MPTVehicleDataBlock *CommonBlock;

			//Load data
			sv_A.R = man->R_1;
			sv_A.V = man->V_1;
			sv_A.GMT = man->GMT_1;
			sv_A.RBI = man->RefBodyInd;

			if (num == 0)
			{
				CommonBlock = &mpt->CommonBlock;
			}
			else
			{
				CommonBlock = &mpt->mantable[num - 1].CommonBlock;
			}

			WeightsTable.CC = CommonBlock->ConfigCode;
			WeightsTable.CSMArea = CommonBlock->CSMArea;
			WeightsTable.CSMWeight = CommonBlock->CSMMass;
			WeightsTable.KFactor = mpt->KFactor;
			WeightsTable.LMAscArea = CommonBlock->LMAscentArea;
			WeightsTable.LMAscWeight = CommonBlock->LMAscentMass;
			WeightsTable.LMDscArea = CommonBlock->LMDescentArea;
			WeightsTable.LMDscWeight = CommonBlock->LMDescentMass;
			WeightsTable.SIVBArea = CommonBlock->SIVBArea;
			WeightsTable.SIVBWeight = CommonBlock->SIVBMass;

			if (num == 0)
			{
				WeightsTable.ConfigArea = mpt->ConfigurationArea;
				WeightsTable.ConfigWeight = mpt->TotalInitMass;
			}
			else
			{
				WeightsTable.ConfigArea = mpt->mantable[num - 1].TotalAreaAfter;
				WeightsTable.ConfigWeight = mpt->mantable[num - 1].TotalMassAfter;
			}

			P30TIG = GC->rtcc->GETfromGMT(man->GMT_BI);
			dV_LVLH = man->dV_LVLH;
			manpadenginetype = man->Thruster;
			HeadsUp = man->HeadsUpDownInd;
			manpad_ullage_dt = man->dt_ullage;
			manpad_ullage_opt = man->UllageThrusterOpt;

			//Save maneuver code under remarks
			if (IsCSMCalculation)
			{
				sprintf(GC->manpad.remarks, "%s", man->code.c_str());
			}
			else
			{
				sprintf(GC->lmmanpad.remarks, "%s", man->code.c_str());
			}
		}
		else
		{
			VESSEL *v;
			if (IsCSMCalculation)
			{
				v = GC->rtcc->pCSM;
			}
			else
			{
				v = GC->rtcc->pLM;
			}

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			sv_A = GC->rtcc->StateVectorCalcEphem(v);
			WeightsTable = GC->rtcc->GetWeightsTable(v, IsCSMCalculation, vesselisdocked);
		}

		if (IsCSMCalculation)
		{
			AP11ManPADOpt opt;

			opt.TIG = P30TIG;
			opt.dV_LVLH = dV_LVLH;
			opt.enginetype = manpadenginetype;
			opt.HeadsUp = HeadsUp;
			opt.REFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
			opt.sxtstardtime = sxtstardtime;
			opt.RV_MCC = sv_A;
			opt.WeightsTable = WeightsTable;
			opt.UllageDT = manpad_ullage_dt;
			opt.UllageThrusterOpt = manpad_ullage_opt;

			GC->rtcc->AP11ManeuverPAD(opt, GC->manpad);
		}
		else
		{
			AP11LMManPADOpt opt;

			opt.TIG = P30TIG;
			opt.dV_LVLH = dV_LVLH;
			opt.enginetype = manpadenginetype;
			opt.HeadsUp = HeadsUp;
			opt.REFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
			opt.sxtstardtime = sxtstardtime;
			opt.RV_MCC = sv_A;
			opt.WeightsTable = WeightsTable;

			GC->rtcc->AP11LMManeuverPAD(opt, GC->lmmanpad);
		}

		Result = DONE;
	}
	break;
	case 10: //Lunar Descent Planning Processor
	{
		EphemerisData sv;
		double W_LM;

		if (GC->MissionPlanningActive)
		{
			double gmt;

			if (GC->rtcc->med_k16.VectorTime != 0.0)
			{
				gmt = GC->rtcc->GMTfromGET(GC->rtcc->med_k16.VectorTime);
			}
			else
			{
				gmt = GC->rtcc->RTCCPresentTimeGMT();
			}

			if (GC->rtcc->EMSFFV(gmt, GC->rtcc->med_k16.Vehicle, sv))
			{
				Result = DONE;
				break;
			}
			W_LM = 0.0; //TBD
		}
		else
		{
			VESSEL *v;

			if (GC->rtcc->med_k16.Vehicle == RTCC_MPT_CSM)
			{
				v = GC->rtcc->pCSM;
			}
			else
			{
				v = GC->rtcc->pLM;
			}

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			sv = GC->rtcc->StateVectorCalcEphem(v);
			if (GC->rtcc->pLM)
			{
				W_LM = GC->rtcc->pLM->GetMass();
			}
			else
			{
				W_LM = 0.0;
			}
		}

		GC->rtcc->LunarDescentPlanningProcessor(sv, W_LM);

		Result = DONE;
	}
	break;
	case 11: //Space Digitals without MPT
	{
		VESSEL *v;

		if (GC->rtcc->EZETVMED.SpaceDigVehID == RTCC_MPT_CSM)
		{
			v = GC->rtcc->pCSM;
		}
		else
		{
			v = GC->rtcc->pLM;
		}

		if (v == NULL)
		{
			Result = DONE;
			break;
		}

		SV sv0 = GC->rtcc->StateVectorCalc(v);
		GC->rtcc->EMDSPACENoMPT(sv0, SpaceDigitalsOption + 2, GC->rtcc->GMTfromGET(SpaceDigitalsGET));

		Result = DONE;
	}
	break;
	case 12: //TLI Processor
	{
		EphemerisData state;
		PLAWDTOutput WeightsTable;

		if (GC->MissionPlanningActive)
		{
			GC->rtcc->TranslunarInjectionProcessor(true);
		}
		else
		{
			if (iuvessel == NULL)
			{
				Result = DONE;
				break;
			}

			state = GC->rtcc->StateVectorCalcEphem(iuvessel);
			WeightsTable = GC->rtcc->GetWeightsTable(iuvessel, true, false);

			GC->rtcc->TranslunarInjectionProcessor(false, &state, &WeightsTable);
		}

		Result = DONE;
	}
	break;
	case 13: //Lunar Launch Targeting Processor (Short Rendezvous Profile)
	{
		LLTPOpt opt;
		EphemerisData sv_CSM;

		if (GC->MissionPlanningActive)
		{
			if (GC->rtcc->EMSFFV(GC->rtcc->GMTfromGET(GC->rtcc->med_k50.GETV), RTCC_MPT_CSM, sv_CSM))
			{
				Result = DONE;
				break;
			}
		}
		else
		{
			VESSEL *v = GC->rtcc->pCSM;

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			sv_CSM = GC->rtcc->StateVectorCalcEphem(v);
		}

		opt.sv_CSM = sv_CSM;
		opt.Y_S = GC->rtcc->PZLTRT.YawSteerCap;
		opt.V_Z_NOM = 32.0*0.3048;
		opt.T_TH = GC->rtcc->GMTfromGET(GC->rtcc->med_k50.GETTH);
		opt.R_LS = GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST];
		opt.lat_LS = GC->rtcc->BZLAND.lat[0];
		opt.lng_LS = GC->rtcc->BZLAND.lng[0];
		opt.alpha_PF = GC->rtcc->PZLTRT.PoweredFlightArc;
		opt.dt_PF = GC->rtcc->PZLTRT.PoweredFlightTime;
		opt.dt_INS_TPI = GC->rtcc->PZLTRT.DT_Ins_TPI;
		opt.h_INS = GC->rtcc->PZLTRT.InsertionHeight;
		opt.DH_TPI = GC->rtcc->PZLTRT.DT_DH;
		opt.dTheta_TPI = GC->rtcc->PZLTRT.DT_Theta_i;
		opt.WT = GC->rtcc->PZLTRT.TerminalPhaseTravelAngle;

		if (GC->rtcc->LunarLiftoffTimePredictionDT(opt, GC->rtcc->PZLLTT))
		{
			t_LunarLiftoff = GC->rtcc->PZLLTT.GETLOR;
			GC->rtcc->PZLTRT.InsertionHorizontalVelocity = GC->rtcc->PZLLTT.VH;
		}

		Result = DONE;
	}
	break;
	case 14: //MCC Targeting
	{
		EphemerisData sv0;
		double CSMmass, LMmass;

		if (GC->MissionPlanningActive)
		{
			double GMT = GC->rtcc->GMTfromGET(GC->rtcc->PZMCCPLN.VectorGET);
			EphemerisData EPHEM;
			if (GC->rtcc->EMSFFV(GMT, RTCC_MPT_CSM, EPHEM))
			{
				Result = DONE;
				break;
			}

			sv0 = EPHEM;

			PLAWDTInput pin;
			PLAWDTOutput pout;
			pin.T_UP = GMT;
			pin.TableCode = RTCC_MPT_CSM;
			GC->rtcc->PLAWDT(pin, pout);

			CSMmass = pout.CSMWeight;
			LMmass = pout.LMAscWeight + pout.LMDscWeight;
		}
		else
		{
			VESSEL *v = GC->rtcc->pCSM;

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			sv0 = GC->rtcc->StateVectorCalcEphem(v);

			CSMmass = v->GetMass();
			//Assume pre CSM separation from the S-IVB
			if (CSMmass > 40000.0)
			{
				CSMmass = 28860.0;
			}
			if (GC->rtcc->PZMCCPLN.Config)
			{
				LMmass = GC->rtcc->GetDockedVesselMass(v);
			}
			else
			{
				LMmass = 0.0;
			}
		}

		GC->rtcc->TranslunarMidcourseCorrectionProcessor(sv0, CSMmass, LMmass);

		Result = DONE;
	}
	break;
	case 15:	//Lunar Launch Window Processor
	{
		LunarLiftoffTimeOpt opt;
		SV sv_CSM;

		if (GC->MissionPlanningActive)
		{
			double GMT = GC->rtcc->GMTfromGET(GC->rtcc->med_k15.CSMVectorTime);
			EphemerisData EPHEM;
			if (GC->rtcc->EMSFFV(GMT, RTCC_MPT_CSM, EPHEM))
			{
				Result = DONE;
				break;
			}
			sv_CSM.R = EPHEM.R;
			sv_CSM.V = EPHEM.V;
			sv_CSM.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv_CSM.gravref = GC->rtcc->GetGravref(EPHEM.RBI);
		}
		else
		{
			VESSEL *v = GC->rtcc->pCSM;

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			sv_CSM = GC->rtcc->StateVectorCalc(v);
		}

		if (GC->rtcc->med_k15.CSI_Flag == 0)
		{
			opt.I_BURN = 0;
		}
		else if (GC->rtcc->med_k15.CSI_Flag < 0)
		{
			opt.I_BURN = 2;
		}
		else
		{
			opt.I_BURN = 1;
			opt.DT_B = GC->rtcc->med_k15.CSI_Flag;
		}
		opt.I_TPI = GC->rtcc->med_k15.TPIDefinition;
		opt.I_CDH = GC->rtcc->med_k15.CDH_Flag;
		opt.t_BASE = GC->rtcc->PZLTRT.dt_bias;
		if (GC->rtcc->med_k15.DeltaHTFlag < 0)
		{
			opt.I_SRCH = 0;
		}
		else
		{
			opt.I_SRCH = 1;
			opt.N_CURV = GC->rtcc->med_k15.DeltaHTFlag;
		}
		opt.L_DH = 3;
		opt.t_max = GC->rtcc->PZLTRT.MaxAscLifetime;
		opt.H_S = GC->rtcc->PZLTRT.MinSafeHeight;
		opt.DV_MAX[0] = GC->rtcc->PZLTRT.CSMMaxDeltaV;
		opt.DV_MAX[1] = GC->rtcc->PZLTRT.LMMaxDeltaV;
		opt.DH[0] = GC->rtcc->med_k15.DH1;
		opt.DH[1] = GC->rtcc->med_k15.DH2;
		opt.DH[2] = GC->rtcc->med_k15.DH3;
		opt.theta_1 = GC->rtcc->PZLTRT.PoweredFlightArc;
		opt.dt_1 = GC->rtcc->PZLTRT.PoweredFlightTime;
		opt.v_LH = GC->rtcc->PZLTRT.InsertionHorizontalVelocity;
		opt.v_LV = GC->rtcc->PZLTRT.InsertionRadialVelocity;
		opt.h_BO = GC->rtcc->PZLTRT.InsertionHeight;
		opt.Y_S = GC->rtcc->PZLTRT.YawSteerCap;
		opt.DH_SRCH = GC->rtcc->PZLTRT.Height_Diff_Begin;
		opt.DH_STEP = GC->rtcc->PZLTRT.Height_Diff_Incr;
		opt.theta_F = GC->rtcc->PZLTRT.TerminalPhaseTravelAngle;
		opt.E = GC->rtcc->PZLTRT.ElevationAngle;
		opt.DH_OFF = GC->rtcc->PZLTRT.TPF_Height_Offset;
		opt.dTheta_OFF = GC->rtcc->PZLTRT.TPF_Phase_Offset;
		opt.t_hole = GC->rtcc->GMTfromGET(GC->rtcc->med_k15.ThresholdTime);
		opt.lat = GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST];
		opt.lng = GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST];
		opt.R_LLS = GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST];
		opt.lng_TPI = GC->rtcc->med_k15.TPIValue;
		opt.sv_CSM = sv_CSM;
		if (GC->rtcc->med_k15.Chaser == 1)
		{
			opt.M = 1;
			opt.P = 2;
		}
		else
		{
			opt.M = 2;
			opt.P = 1;
		}

		GC->rtcc->LunarLaunchWindowProcessor(opt);

		Result = DONE;
	}
	break;
	case 16: //PDI PAD
	{
		PDIPADOpt opt;
		AP11PDIPAD temppdipad;

		if (GC->MissionPlanningActive)
		{
			if (GC->rtcc->NewMPTTrajectory(RTCC_MPT_LM, opt.sv0))
			{
				Result = DONE;
				break;
			}
		}
		else
		{
			if (GC->rtcc->pLM == NULL)
			{
				Result = DONE;
				break;
			}

			opt.sv0 = GC->rtcc->StateVectorCalc(GC->rtcc->pLM);
		}

		opt.direct = true;
		opt.HeadsUp = HeadsUp;
		opt.REFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
		opt.R_LS = OrbMech::r_from_latlong(GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST], GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST], GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST]);
		opt.t_land = GC->rtcc->CZTDTGTU.GETTD;

		PADSolGood = GC->rtcc->PDI_PAD(opt, temppdipad);

		if (PADSolGood)
		{
			GC->pdipad = temppdipad;
		}

		Result = DONE;
	}
	break;
	case 17: //Deorbit Maneuver
	{
		EphemerisData sv;
		double CSMmass;

		if (GC->MissionPlanningActive)
		{
			double GMT = GC->rtcc->GMTfromGET(GC->rtcc->RZJCTTC.R32_GETI);
			int err = GC->rtcc->EMSFFV(GMT, RTCC_MPT_CSM, sv);
			if (err)
			{
				Result = DONE;
				break;
			}

			PLAWDTInput pin;
			PLAWDTOutput pout;
			pin.T_UP = GMT;
			pin.TableCode = RTCC_MPT_CSM;
			GC->rtcc->PLAWDT(pin, pout);
			CSMmass = pout.CSMWeight;
		}
		else
		{
			VESSEL *v = GC->rtcc->pCSM;

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			sv = GC->rtcc->StateVectorCalcEphem(v);
			CSMmass = v->GetMass();
			//Assume pre CSM separation from the S-IVB
			if (CSMmass > 30000.0)
			{
				CSMmass = 28860.0;
			}
		}

		GC->rtcc->RMSDBMP(sv, CSMmass);

		if (GC->rtcc->RZRFDP.data[2].Indicator == 0)
		{
			P30TIG = GC->rtcc->RZRFDP.data[2].GETI;
			dV_LVLH = GC->rtcc->RZRFTT.Manual.DeltaV;

			GC->rtcc->RZC1RCNS.entry = GC->rtcc->RZRFTT.Manual.entry;

			GC->rtcc->RZDBSC1.lat_T = GC->rtcc->RZRFTT.Manual.entry.lat_T;
			GC->rtcc->RZDBSC1.lng_T = GC->rtcc->RZRFTT.Manual.entry.lng_T;
			manpadenginetype = GC->rtcc->RZRFTT.Manual.Thruster;
		}

		Result = DONE;
	}
	break;
	case 19: //Docking Initiation Processor
	{
		DKIOpt opt;
		double GMT;

		if (GC->rtcc->med_k10.MLDTime == 0.0)
		{
			GMT = GC->rtcc->RTCCPresentTimeGMT();
		}
		else
		{
			GMT = GC->rtcc->GMTfromGET(GC->rtcc->med_k10.MLDTime);
		}

		if (GC->MissionPlanningActive)
		{
			EphemerisData EPHEM;

			int err = GC->rtcc->EMSFFV(GMT, RTCC_MPT_CSM, EPHEM);
			if (err)
			{
				Result = DONE;
				break;
			}
			opt.sv_CSM = EPHEM;

			err = GC->rtcc->EMSFFV(GMT, RTCC_MPT_LM, EPHEM);
			if (err)
			{
				Result = DONE;
				break;
			}
			opt.sv_LM = EPHEM;
		}
		else
		{
			if (GC->rtcc->pCSM == NULL || GC->rtcc->pLM == NULL)
			{
				Result = DONE;
				break;
			}

			opt.sv_CSM = GC->rtcc->StateVectorCalcEphem(GC->rtcc->pCSM);
			opt.sv_LM = GC->rtcc->StateVectorCalcEphem(GC->rtcc->pLM);

			//Coast to threshold time
			opt.sv_CSM = GC->rtcc->coast(opt.sv_CSM, GMT - opt.sv_CSM.GMT);
			opt.sv_LM = GC->rtcc->coast(opt.sv_LM, GMT - opt.sv_LM.GMT);
		}		

		opt.IPUTNA = GC->rtcc->med_k10.MLDOption;
		opt.PUTNA = GC->rtcc->med_k10.MLDValue;
		opt.PUTTNA = GC->rtcc->GMTfromGET(GC->rtcc->med_k10.MLDTime);

		if (GC->rtcc->GZGENCSN.DKI_TP_Definition == 0)
		{
			opt.KCOSR = true;
			opt.COSR = GC->rtcc->GZGENCSN.DKI_TPDefinitionValue;
		}
		else
		{
			opt.KCOSR = false;
			opt.K46 = GC->rtcc->GZGENCSN.DKI_TP_Definition;
			if (GC->rtcc->GZGENCSN.DKI_TP_Definition == 1)
			{
				opt.TTPI = GC->rtcc->GMTfromGET(GC->rtcc->GZGENCSN.DKI_TPDefinitionValue);
			}
			else if (GC->rtcc->GZGENCSN.DKI_TP_Definition == 2)
			{
				opt.TTPF = GC->rtcc->GMTfromGET(GC->rtcc->GZGENCSN.DKI_TPDefinitionValue);
			}
			else
			{
				opt.TIMLIT = GC->rtcc->GZGENCSN.DKI_TPDefinitionValue;
			}
		}

		opt.DHNCC = GC->rtcc->GZGENCSN.DKIDeltaH_NCC;
		opt.DHSR = GC->rtcc->GZGENCSN.DKIDeltaH_NSR;
		opt.DTSR = 10.0*60.0;
		opt.dt_NCC_NSR = GC->rtcc->med_k00.dt_NCC_NSR;
		opt.Elev = GC->rtcc->GZGENCSN.DKIElevationAngle;
		opt.I4 = GC->rtcc->med_k00.I4;
		//TBD: opt.IHALF
		opt.NC1 = GC->rtcc->med_k00.NC1;
		opt.NH = GC->rtcc->med_k00.NH;
		opt.NCC = GC->rtcc->med_k00.NCC;
		opt.NSR = GC->rtcc->med_k00.NSR;
		opt.NPC = GC->rtcc->med_k00.NPC;
		opt.MI = GC->rtcc->med_k00.MI;
		if (GC->rtcc->med_k00.ChaserVehicle == RTCC_MPT_CSM)
		{
			opt.MV = 1;
		}
		else
		{
			opt.MV = 2;
		}
		opt.WT = GC->rtcc->GZGENCSN.DKITerminalPhaseAngle;
		opt.KRAP = GC->rtcc->GZGENCSN.DKIPhaseAngleSetting;

		GC->rtcc->DockingInitiationProcessor(opt);

		Result = DONE;
	}
	break;
	case 20: //Lunar Ascent Processor
	{
		RTCC::LunarAscentProcessorInputs asc_in;
		RTCC::LunarAscentProcessorOutputs asc_out;

		if (GC->MissionPlanningActive)
		{
			double GMT = GC->rtcc->GMTfromGET(t_LunarLiftoff);
			EphemerisData EPHEM;
			if (GC->rtcc->EMSFFV(GMT, RTCC_MPT_CSM, EPHEM))
			{
				Result = DONE;
				break;
			}

			asc_in.sv_CSM = EPHEM;

			PLAWDTInput pin;
			PLAWDTOutput pout;
			pin.T_UP = GMT;
			pin.TableCode = RTCC_MPT_LM;
			GC->rtcc->PLAWDT(pin, pout);

			asc_in.m0 = pout.LMAscWeight;
		}
		else
		{
			if (GC->rtcc->pCSM == NULL || GC->rtcc->pLM == NULL)
			{
				Result = DONE;
				break;
			}
			asc_in.sv_CSM = GC->rtcc->StateVectorCalcEphem(GC->rtcc->pCSM);
			LEM *l = (LEM *)GC->rtcc->pLM;
			asc_in.m0 = l->GetAscentStageMass();
		}

		asc_in.R_LS = OrbMech::r_from_latlong(GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST], GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST], GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST]);
		asc_in.t_liftoff = GC->rtcc->GMTfromGET(t_LunarLiftoff);
		asc_in.v_LH = GC->rtcc->PZLTRT.InsertionHorizontalVelocity;
		asc_in.v_LV = GC->rtcc->PZLTRT.InsertionRadialVelocity;

		GC->rtcc->LunarAscentProcessor(asc_in, asc_out);

		GC->rtcc->PZLTRT.PoweredFlightArc = asc_out.theta;
		GC->rtcc->PZLTRT.PoweredFlightTime = asc_out.dt_asc;

		GC->rtcc->JZLAI.t_launch = t_LunarLiftoff;
		GC->rtcc->JZLAI.R_D = 60000.0*0.3048;
		GC->rtcc->JZLAI.Y_D = 0.0;
		GC->rtcc->JZLAI.R_D_dot = GC->rtcc->PZLTRT.InsertionRadialVelocity;
		GC->rtcc->JZLAI.Y_D_dot = 0.0;
		GC->rtcc->JZLAI.Z_D_dot = GC->rtcc->PZLTRT.InsertionHorizontalVelocity;

		GC->rtcc->JZLAI.sv_Insertion = asc_out.sv_Ins;

		LAP_Phase = asc_out.phase;
		LAP_CR = asc_out.CR;

		Result = DONE;
	}
	break;
	case 21: //LM Ascent PAD
	{
		//TBD: MPT compatibility

		if (GC->rtcc->pCSM == NULL || GC->rtcc->pLM == NULL)
		{
			Result = DONE;
			break;
		}

		ASCPADOpt opt;
		EphemerisData sv_CSM;

		sv_CSM = GC->rtcc->StateVectorCalcEphem(GC->rtcc->pCSM);

		opt.Rot_VL = OrbMech::GetVesselToLocalRotMatrix(GC->rtcc->pLM);
		opt.R_LS = OrbMech::r_from_latlong(GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST], GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST], GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST]);
		opt.sv_CSM = sv_CSM;
		opt.TIG = t_LunarLiftoff;
		opt.v_LH = GC->rtcc->PZLTRT.InsertionHorizontalVelocity;
		opt.v_LV = GC->rtcc->PZLTRT.InsertionRadialVelocity;

		GC->rtcc->LunarAscentPAD(opt, lmascentpad);

		Result = DONE;
	}
	break;
	case 22: //Powered Descent Abort Program
	{
		PDAPOpt opt;
		PDAPResults res;
		SV sv_LM, sv_CSM;

		if (GC->rtcc->pCSM == NULL || GC->rtcc->pLM == NULL)
		{
			Result = DONE;
			break;
		}

		if (utils::IsVessel(GC->rtcc->pLM, utils::LEM) == false)
		{
			Result = DONE;
			break;
		}

		LEM *l = (LEM *)GC->rtcc->pLM;

		if (GC->MissionPlanningActive)
		{
			if (GC->rtcc->NewMPTTrajectory(RTCC_MPT_LM, sv_LM))
			{
				Result = DONE;
				break;
			}
			if (GC->rtcc->NewMPTTrajectory(RTCC_MPT_CSM, sv_CSM))
			{
				Result = DONE;
				break;
			}

			opt.W_TAPS = 0.0;
			opt.W_TDRY = 0.0;
		}
		else
		{
			opt.W_TAPS = l->GetAscentStageMass();
			opt.W_TDRY = opt.sv_A.mass - l->GetPropellantMass(l->GetPropellantHandleByIndex(0));

			sv_LM = GC->rtcc->StateVectorCalc(GC->rtcc->pLM);
			sv_CSM = GC->rtcc->StateVectorCalc(GC->rtcc->pCSM);
		}

		if (PDAPEngine == 0)
		{
			opt.dt_stage = 999999.9;
		}
		else
		{
			opt.dt_stage = 0.0;

		}

		opt.IsTwoSegment = PDAPTwoSegment;
		opt.REFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
		opt.R_LS = OrbMech::r_from_latlong(GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST], GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST], GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST]);
		opt.sv_A = sv_LM;
		opt.sv_P = sv_CSM;
		opt.TLAND = GC->rtcc->CZTDTGTU.GETTD;
		opt.t_TPI = t_TPI;
		if (opt.IsTwoSegment)
		{
			opt.dt_step = 20.0;
		}
		else
		{
			opt.dt_step = 120.0;
		}

		if (PADSolGood = GC->rtcc->PoweredDescentAbortProgram(opt, res))
		{
			if (opt.IsTwoSegment == false)
			{
				if (PDAPEngine == 0)
				{
					PDAPABTCOF[0] = res.ABTCOF1;
					PDAPABTCOF[1] = res.ABTCOF2;
					PDAPABTCOF[2] = res.ABTCOF3;
					PDAPABTCOF[3] = res.ABTCOF4;
				}
				else
				{
					PDAPABTCOF[4] = res.ABTCOF1;
					PDAPABTCOF[5] = res.ABTCOF2;
					PDAPABTCOF[6] = res.ABTCOF3;
					PDAPABTCOF[7] = res.ABTCOF4;
				}
			}
			else
			{
				PDAP_J1 = res.J1;
				PDAP_J2 = res.J2;
				PDAP_K1 = res.K1;
				PDAP_K2 = res.K2;
				PDAP_Theta_LIM = res.Theta_LIM;
				PDAP_R_amin = res.R_amin;
			}

			DEDA224 = res.DEDA224;
			DEDA225 = res.DEDA225;
			DEDA226 = res.DEDA226;
			DEDA227 = OrbMech::DoubleToDEDA(res.DEDA227 / 0.3048*pow(2, -20), 14);
		}

		Result = DONE;
	}
	break;
	case 23: //Calculate TPI times
	{
		if (Rendezvous_Target == NULL)
		{
			Result = DONE;
			break;
		}
		SV sv0 = GC->rtcc->StateVectorCalc(Rendezvous_Target);
		t_TPI = GC->rtcc->CalculateTPITimes(sv0, TPI_Mode, t_TPIguess, dt_TPI_sunrise);

		Result = DONE;
	}
	break;
	case 24: //FIDO Orbit Digitals No 1 Cycle
	{
		GC->rtcc->EMMDYNMC(1, 1);
		Result = DONE;
	}
	break;
	case 25: //Vector Compare Display
	{
		GC->rtcc->BMSVEC();
		Result = DONE;
	}
	break;
	case 26: //FIDO Orbit Digitals No 1 Cycle
	{
		GC->rtcc->EMMDYNMC(2, 1);
		Result = DONE;
	}
	break;
	case 27: //SLV Navigation Update Calculation
	{
		if (iuvessel == NULL)
		{
			Result = DONE;
			break;
		}

		EphemerisData sv = GC->rtcc->StateVectorCalcEphem(iuvessel);
		EphemerisData sv2;
		if (SVDesiredGET < 0)
		{
			sv2 = sv;
		}
		else
		{
			sv2 = GC->rtcc->coast(sv, GC->rtcc->GMTfromGET(SVDesiredGET) - sv.GMT, RTCC_MPT_CSM);
		}

		GC->rtcc->CMMSLVNAV(sv2.R, sv2.V, sv2.GMT);

		Result = DONE;
	}
	break;
	case 28: //SLV Navigation Update Uplink
	{
		iuUplinkResult = DONE;

		if (GC->rtcc->CZNAVSLV.NUPTIM == 0.0)
		{
			iuUplinkResult = 4;
			Result = DONE;
			break;
		}
		if (iuvessel == NULL)
		{
			iuUplinkResult = 2;
			Result = DONE;
			break;
		}

		IU *iu = NULL;

		if (utils::IsVessel(iuvessel, utils::Saturn))
		{
			Saturn *iuv = (Saturn *)iuvessel;
			iu = iuv->GetIU();
		}
		else if (utils::IsVessel(iuvessel, utils::SIVB))
		{
			SIVB *iuv = (SIVB *)iuvessel;
			iu = iuv->GetIU();
		}
		else
		{
			iuUplinkResult = 2;
			Result = DONE;
			break;
		}

		void *uplink = NULL;
		DCSSLVNAVUPDATE upl;

		upl.PosS = GC->rtcc->CZNAVSLV.PosS;
		upl.DotS = GC->rtcc->CZNAVSLV.DotS;
		upl.NUPTIM = GC->rtcc->CZNAVSLV.NUPTIM;

		uplink = &upl;
		bool uplinkaccepted = iu->DCSUplink(DCSUPLINK_SLV_NAVIGATION_UPDATE, uplink);

		if (uplinkaccepted)
		{
			iuUplinkResult = 1;
		}
		else
		{
			iuUplinkResult = 3;
		}

		Result = DONE;
	}
	break;
	case 29: //FIDO Space Digitals Cycle
	{
		GC->rtcc->EMDSPACE(1);
		Result = DONE;
	}
	break;
	case 30: //FIDO Space Digitals MSK Request
	{
		GC->rtcc->EMDSPACE(6);
		Result = DONE;
	}
	break;
	case 31: //Entry PAD
	{
		VESSEL *v = GC->rtcc->pCSM;

		if (v == NULL)
		{
			Result = DONE;
			break;
		}

		OBJHANDLE hEarth;
		double mu;

		hEarth = oapiGetObjectByName("Earth");
		mu = GGRAV * oapiGetMass(hEarth);

		if (GC->entrypadopt == 0)
		{
			EarthEntryPADOpt opt;

			opt.dV_LVLH = dV_LVLH;
			opt.P30TIG = P30TIG;
			opt.REFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
			opt.sv0 = GC->rtcc->StateVectorCalc(v);
			opt.Thruster = manpadenginetype;
			opt.InitialBank = GC->rtcc->RZC1RCNS.entry.GNInitialBank;
			opt.GLevel = GC->rtcc->RZC1RCNS.entry.GLevel;

			if (GC->rtcc->RZDBSC1.lat_T == 0)
			{
				opt.lat = 0;
				opt.lng = 0;
			}
			else
			{
				opt.lat = GC->rtcc->RZDBSC1.lat_T;
				opt.lng = GC->rtcc->RZDBSC1.lng_T;
			}

			VECTOR3 R, V;
			double apo, peri;
			OBJHANDLE gravref = GC->rtcc->AGCGravityRef(v);
			v->GetRelativePos(gravref, R);
			v->GetRelativeVel(gravref, V);
			OrbMech::periapo(R, V, mu, apo, peri);

			if (peri < oapiGetSize(gravref) + 50 * 1852.0)
			{
				opt.preburn = false;
				GC->rtcc->EarthOrbitEntry(opt, GC->earthentrypad);
			}
			else
			{
				opt.preburn = true;
				GC->rtcc->EarthOrbitEntry(opt, GC->earthentrypad);
			}
		}
		else
		{
			LunarEntryPADOpt opt;

			if (GC->rtcc->RZDBSC1.lat_T == 0)
			{
				//EntryPADLat = entry->EntryLatPred;
				//EntryPADLng = entry->EntryLngPred;
			}
			else
			{
				if (GC->MissionPlanningActive)
				{
					if (GC->rtcc->NewMPTTrajectory(RTCC_MPT_CSM, opt.sv0))
					{
						opt.sv0 = GC->rtcc->StateVectorCalc(v);
					}
				}
				else
				{
					opt.sv0 = GC->rtcc->StateVectorCalc(v);
				}

				//EntryPADLat = EntryLatcor;
				//EntryPADLng = EntryLngcor;
				opt.lat = GC->rtcc->RZDBSC1.lat_T;
				opt.lng = GC->rtcc->RZDBSC1.lng_T;
				opt.REFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
				opt.SxtStarCheckAttitudeOpt = GC->EntryPADSxtStarCheckAttOpt;

				GC->rtcc->LunarEntryPAD(opt, GC->lunarentrypad);
			}
		}

		Result = DONE;
	}
	break;
	case 32: //Map Update
	{
		EphemerisData sv0;
		double gmt;

		if (mapUpdateGET <= 0.0)
		{
			gmt = GC->rtcc->RTCCPresentTimeGMT();
		}
		else
		{
			gmt = GC->rtcc->GMTfromGET(mapUpdateGET);
		}

		if (GC->MissionPlanningActive)
		{
			if (GC->rtcc->EMSFFV(gmt, RTCC_MPT_CSM, sv0))
			{
				Result = DONE;
				break;
			}
		}
		else
		{
			VESSEL *v;

			if (IsCSMCalculation)
			{
				v = GC->rtcc->pCSM;
			}
			else
			{
				v = GC->rtcc->pLM;
			}
			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			sv0 = GC->rtcc->StateVectorCalcEphem(v);
		}

		//Coast to desired GMT
		sv0 = GC->rtcc->coast(sv0, gmt - sv0.GMT);

		if (mappage == 0)
		{
			int gstat;
			double ttoGSAOS, ttoGSLOS, MJD;
			OBJHANDLE gravref;

			MJD = OrbMech::MJDfromGET(sv0.GMT, GC->rtcc->GetGMTBase());
			gravref = GC->rtcc->GetGravref(sv0.RBI);

			gstat = OrbMech::findNextAOS(GC->rtcc->SystemParameters.MAT_J2000_BRCS, sv0.R, sv0.V, MJD, gravref);

			OrbMech::groundstation(GC->rtcc->SystemParameters.MAT_J2000_BRCS, sv0.R, sv0.V, MJD, gravref, groundstations[gstat][0], groundstations[gstat][1], 1, ttoGSAOS);
			OrbMech::groundstation(GC->rtcc->SystemParameters.MAT_J2000_BRCS, sv0.R, sv0.V, MJD, gravref, groundstations[gstat][0], groundstations[gstat][1], 0, ttoGSLOS);
			GSAOSGET = (MJD - GC->rtcc->CalcGETBase())*24.0*3600.0 + ttoGSAOS;
			GSLOSGET = (MJD - GC->rtcc->CalcGETBase())*24.0*3600.0 + ttoGSLOS;
			mapgs = gstat;
		}
		else
		{
			double pm;

			if (mapUpdatePM)
			{
				pm = -180.0*RAD;
			}
			else
			{
				pm = -150.0*RAD;
			}

			GC->rtcc->LunarOrbitMapUpdate(sv0, mapupdate, pm);
		}

		Result = DONE;
	}
	break;
	case 33: //Landmark Tracking PAD
	{
		LMARKTRKPADOpt opt;
		EphemerisData sv0, sv1;

		double get, gmt;

		if (GC->LmkTime <= 0.0)
		{
			gmt = GC->rtcc->RTCCPresentTimeGMT();
			get = GC->rtcc->GETfromGMT(gmt);
		}
		else
		{
			get = GC->LmkTime;
			gmt = GC->rtcc->GMTfromGET(get);
		}

		if (GC->MissionPlanningActive)
		{
			EphemerisData sv;
			if (GC->rtcc->EMSFFV(gmt, RTCC_MPT_CSM, sv0))
			{
				Result = DONE;
				break;
			}
		}
		else
		{
			VESSEL *v = GC->rtcc->pCSM;

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			sv0 = GC->rtcc->StateVectorCalcEphem(v);
		}

		opt.lat[0] = GC->LmkLat;
		opt.LmkTime[0] = get;
		opt.lng[0] = GC->LmkLng;
		opt.sv0 = sv0;
		opt.Elevation = GC->LmkElevation;
		opt.entries = 1;

		GC->rtcc->LandmarkTrackingPAD(opt, GC->landmarkpad);

		Result = DONE;
	}
	break;
	case 34: //Vector Panel Summary Display
	{
		GC->rtcc->BMDVPS();
		Result = DONE;
	}
	break;
	case 35: //AGS Clock Sync
	{
		VESSEL *v = GC->rtcc->pLM;

		if (v == NULL || utils::IsVessel(v, utils::LEM) == false)
		{
			Result = DONE;
			break;
		}

		LEM *l = (LEM*)v;

		double KFactor;
		bool res = GC->rtcc->CalculateAGSKFactor(&l->agc.vagc, &l->aea.vags, KFactor);
		if (res)
		{
			//TBD: Use MED P15 instead
			GC->rtcc->SystemParameters.MCGZSS = GC->rtcc->SystemParameters.MCGZSL + KFactor / 3600.0;
		}

		Result = DONE;
	}
	break;
	case 36: //Next Station Contacts Display
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->rtcc->CalcGETBase());
		GC->rtcc->EMDSTAC();

		Result = DONE;
	}
	break;
	case 37: //Recovery Target Selection Display
	{
		EphemerisDataTable2 tab;
		EphemerisDataTable2 *tab2;
		double gmt_guess, gmt_min, gmt_max;
		
		gmt_guess = GC->rtcc->GMTfromGET(GC->rtcc->RZJCTTC.R20_GET);
		gmt_min = gmt_guess;
		gmt_max = gmt_guess + 2.75*60.0*60.0;

		if (GC->MissionPlanningActive)
		{
			unsigned int NumVec;
			int TUP;
			ManeuverTimesTable MANTIMES;
			LunarStayTimesTable LUNSTAY;

			GC->rtcc->ELNMVC(gmt_min, gmt_max, RTCC_MPT_CSM, NumVec, TUP);
			GC->rtcc->ELFECH(gmt_min, NumVec, 0, RTCC_MPT_CSM, tab, MANTIMES, LUNSTAY);

			if (tab.Header.NumVec < 9 || GC->rtcc->DetermineSVBody(tab.table[0]) != BODY_EARTH)
			{
				Result = DONE;
				break;
			}
		}
		else
		{
			VESSEL *v;

			if (GC->rtcc->RZJCTTC.R20_VEH == RTCC_MPT_CSM)
			{
				v = GC->rtcc->pCSM;
			}
			else
			{
				v = GC->rtcc->pLM;
			}
			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			EphemerisData sv = GC->rtcc->StateVectorCalcEphem(v);

			if (sv.RBI != BODY_EARTH)
			{
				Result = DONE;
				break;
			}

			EMSMISSInputTable intab;

			intab.AnchorVector = sv;
			intab.EphemerisBuildIndicator = true;
			intab.ECIEphemerisIndicator = true;
			intab.ECIEphemTableIndicator = &tab;
			intab.EphemerisLeftLimitGMT = gmt_min;
			intab.EphemerisRightLimitGMT = gmt_max;
			intab.ManCutoffIndicator = false;
			intab.VehicleCode = RTCC_MPT_CSM;

			GC->rtcc->EMSMISS(&intab);
			tab.Header.TUP = 1;
		}
		tab2 = &tab;
		GC->rtcc->RMDRTSD(*tab2, 1, gmt_guess, GC->rtcc->RZJCTTC.R20_lng);

		Result = DONE;
	}
	break;
	case 38: //Transfer Two-Impulse Solution to MPT
	{
		TwoImpulseOpt opt;
		TwoImpulseResuls res;

		if (GC->MissionPlanningActive)
		{
			opt.mode = 4;
			opt.SingSolNum = GC->rtcc->med_m72.Plan;
			opt.SingSolTable = GC->rtcc->med_m72.Table;
			GC->rtcc->PMSTICN(opt, res);
		}
		else
		{
			if (GC->rtcc->med_m72.Table == 1)
			{
				if (GC->rtcc->med_m72.Plan > GC->rtcc->PZTIPREG.Solutions)
				{
					Result = DONE;
					break;
				}
				opt.sv_A = GC->rtcc->PZMYSAVE.SV_mult[0];
				opt.sv_P = GC->rtcc->PZMYSAVE.SV_mult[1];
				opt.DH = GC->rtcc->GZGENCSN.TIDeltaH;
				opt.PhaseAngle = GC->rtcc->GZGENCSN.TIPhaseAngle;
				opt.T1 = GC->rtcc->PZTIPREG.data[GC->rtcc->med_m72.Plan - 1].Time1;
				opt.T2 = GC->rtcc->PZTIPREG.data[GC->rtcc->med_m72.Plan - 1].Time2;
			}
			else
			{
				//TBD
				Result = DONE;
				break;
			}

			opt.mode = 5;
			GC->rtcc->PMSTICN(opt, res);

			PMMMPTInput in;

			//Get all required data for PMMMPT and error checking
			if (GetVesselParameters(GC->rtcc->PZTIPREG.MAN_VEH == RTCC_MPT_CSM, vesselisdocked, GC->rtcc->med_m72.Thruster, in.CONFIG, in.VC, in.CSMWeight, in.LMWeight))
			{
				//Error
				Result = DONE;
				break;
			}

			in.VehicleArea = 0.0;
			in.IterationFlag = GC->rtcc->med_m72.Iteration;
			in.IgnitionTimeOption = GC->rtcc->med_m72.TimeFlag;
			in.Thruster = GC->rtcc->med_m72.Thruster;

			in.sv_before = res.sv_tig;
			in.V_aft = res.sv_tig.V + res.dV;
			in.DETU = GC->rtcc->med_m72.UllageDT;
			in.UT = GC->rtcc->med_m72.UllageQuads;
			in.DT_10PCT = GC->rtcc->med_m72.TenPercentDT;
			in.DPSScaleFactor = GC->rtcc->med_m72.DPSThrustFactor;

			double GMT_TIG;
			VECTOR3 DV;
			if (GC->rtcc->PoweredFlightProcessor(in, GMT_TIG, DV) == 0)
			{
				//Save for Maneuver PAD and uplink
				P30TIG = GC->rtcc->GETfromGMT(GMT_TIG);
				dV_LVLH = DV;
				manpadenginetype = GC->rtcc->med_m72.Thruster;
				HeadsUp = true;
				manpad_ullage_dt = GC->rtcc->med_m72.UllageDT;
				manpad_ullage_opt = GC->rtcc->med_m72.UllageQuads;
			}
		}

		Result = DONE;
	}
	break;
	case 39: //Transfer SPQ to MPT
	{
		if (GC->MissionPlanningActive)
		{
			std::vector<std::string> str;
			GC->rtcc->PMMMED("70", str);
		}
		else
		{
			SV sv_pre, sv_post, sv_tig;
			double attachedMass = 0.0;

			//Was CSM or LM the chaser vehicle?
			VESSEL *v;
			if (GC->rtcc->PZDKIT.Block[0].Display[0].VEH == RTCC_MPT_CSM)
			{
				v = GC->rtcc->pCSM;
			}
			else
			{
				v = GC->rtcc->pLM;
			}

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			SV sv_now = GC->rtcc->StateVectorCalc(v);
			sv_tig = GC->rtcc->coast(sv_now, SPQTIG - OrbMech::GETfromMJD(sv_now.MJD, GC->rtcc->CalcGETBase()));

			if (vesselisdocked)
			{
				attachedMass = GC->rtcc->GetDockedVesselMass(v);
			}
			else
			{
				attachedMass = 0.0;
			}
			GC->rtcc->PoweredFlightProcessor(sv_tig, SPQTIG, GC->rtcc->med_m70.Thruster, 0.0, SPQDeltaV, true, P30TIG, dV_LVLH, sv_pre, sv_post);
		}

		Result = DONE;
	}
	break;
	case 40: //Transfer DKI to MPT
	{
		if (GC->MissionPlanningActive)
		{
			std::vector<std::string> str;
			GC->rtcc->PMMMED("70", str);
		}
		else
		{
			PMMMPTInput in;

			//Get all required data for PMMMPT and error checking
			if (GetVesselParameters(GC->rtcc->PZDKIT.Block[0].Display[0].VEH == RTCC_MPT_CSM, vesselisdocked, GC->rtcc->med_m70.Thruster, in.CONFIG, in.VC, in.CSMWeight, in.LMWeight))
			{
				//Error
				Result = DONE;
				break;
			}

			in.VehicleArea = 0.0;
			in.IterationFlag = GC->rtcc->med_m70.Iteration;
			in.IgnitionTimeOption = GC->rtcc->med_m70.TimeFlag;
			in.Thruster = GC->rtcc->med_m70.Thruster;

			in.sv_before = GC->rtcc->PZDKIELM.Block[0].SV_before[0];
			in.V_aft = GC->rtcc->PZDKIELM.Block[0].V_after[0];
			if (GC->rtcc->med_m70.UllageDT < 0)
			{
				in.DETU = GC->rtcc->SystemParameters.MCTNDU;
			}
			else
			{
				in.DETU = GC->rtcc->med_m70.UllageDT;
			}
			in.UT = GC->rtcc->med_m70.UllageQuads;
			in.DT_10PCT = GC->rtcc->med_m70.TenPercentDT;
			in.DPSScaleFactor = GC->rtcc->med_m70.DPSThrustFactor;

			double GMT_TIG;
			VECTOR3 DV;
			if (GC->rtcc->PoweredFlightProcessor(in, GMT_TIG, DV) == 0)
			{
				//Save for Maneuver PAD and uplink
				P30TIG = GC->rtcc->GETfromGMT(GMT_TIG);
				dV_LVLH = DV;
				manpadenginetype = GC->rtcc->med_m70.Thruster;
				HeadsUp = true;
				manpad_ullage_dt = GC->rtcc->med_m70.UllageDT;
				manpad_ullage_opt = GC->rtcc->med_m70.UllageQuads;
			}
		}

		Result = DONE;
	}
	break;
	case 41: //Direct Input to the MPT
	{
		//Dummy data
		std::vector<std::string> str;
		GC->rtcc->PMMMED("66", str);

		Result = DONE;
	}
	break;
	case 42: //Transfer Descent Plan to MPT
	{
		if (GC->MissionPlanningActive)
		{
			std::vector<std::string> str;
			GC->rtcc->PMMMED("70", str);
		}
		else
		{
			SV sv_pre, sv_post, sv_tig;
			double attachedMass = 0.0;

			VESSEL *v;
			if (GC->rtcc->PZLDPELM.plan[0] == RTCC_MPT_CSM)
			{
				v = GC->rtcc->pCSM;
			}
			else
			{
				v = GC->rtcc->pLM;
			}

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			SV sv_now = GC->rtcc->StateVectorCalc(v);
			sv_tig = GC->rtcc->coast(sv_now, GC->rtcc->PZLDPDIS.GETIG[0] - OrbMech::GETfromMJD(sv_now.MJD, GC->rtcc->CalcGETBase()));

			if (vesselisdocked)
			{
				attachedMass = GC->rtcc->GetDockedVesselMass(v);
			}

			GC->rtcc->PoweredFlightProcessor(sv_tig, GC->rtcc->PZLDPDIS.GETIG[0], GC->rtcc->med_m70.Thruster, attachedMass, GC->rtcc->PZLDPDIS.DVVector[0] * 0.3048, true, P30TIG, dV_LVLH, sv_pre, sv_post);
		}

		Result = DONE;
	}
	break;
	case 43: //Direct Input of Lunar Descent Maneuver
	{
		if (GC->MissionPlanningActive)
		{
			//Temporary
			GC->rtcc->med_m86.Time = GC->rtcc->CZTDTGTU.GETTD;

			std::vector<std::string> str;
			GC->rtcc->PMMMED("86", str);
		}

		Result = DONE;
	}
	break;
	case 44: //Transfer ascent maneuver to MPT from lunar targeting
	{
		if (GC->MissionPlanningActive)
		{
			std::vector<std::string> str;
			GC->rtcc->PMMMED("85", str);
		}

		Result = DONE;
	}
	break;
	case 45: //Transfer GPM to the MPT
	{
		if (GC->MissionPlanningActive)
		{
			std::vector<std::string> str;
			GC->rtcc->PMMMED("65", str);
		}
		else
		{
			if (GC->rtcc->PZGPMELM.SV_before.GMT == 0.0)
			{
				//No data
				Result = DONE;
				break;
			}

			PMMMPTInput in;

			//Get all required data for PMMMPT and error checking
			if (GetVesselParameters(GC->rtcc->med_m65.Table == 1, vesselisdocked, GC->rtcc->med_m65.Thruster, in.CONFIG, in.VC, in.CSMWeight, in.LMWeight))
			{
				//Error
				Result = DONE;
				break;
			}

			in.VehicleArea = 129.4*pow(0.3048, 2); //TBD
			in.IterationFlag = GC->rtcc->med_m65.Iteration;
			in.IgnitionTimeOption = GC->rtcc->med_m65.TimeFlag;
			in.Thruster = GC->rtcc->med_m65.Thruster;

			in.sv_before = GC->rtcc->PZGPMELM.SV_before;
			in.V_aft = GC->rtcc->PZGPMELM.V_after;
			if (GC->rtcc->med_m65.UllageDT < 0)
			{
				in.DETU = GC->rtcc->SystemParameters.MCTNDU;
			}
			else
			{
				in.DETU = GC->rtcc->med_m65.UllageDT;
			}
			in.UT = GC->rtcc->med_m65.UllageQuads;
			in.DT_10PCT = GC->rtcc->med_m65.TenPercentDT;
			in.DPSScaleFactor = GC->rtcc->med_m65.DPSThrustFactor;

			double GMT_TIG;
			VECTOR3 DV;
			if (GC->rtcc->PoweredFlightProcessor(in, GMT_TIG, DV) == 0)
			{
				//Save for Maneuver PAD and uplink
				P30TIG = GC->rtcc->GETfromGMT(GMT_TIG);
				dV_LVLH = DV;
				manpadenginetype = GC->rtcc->med_m65.Thruster;
				HeadsUp = true;
				manpad_ullage_dt = in.DETU;
				manpad_ullage_opt = GC->rtcc->med_m65.UllageQuads;
			}
		}

		Result = DONE;
	}
	break;
	case 46: //TLI Direct Input
	{
		if (!GC->MissionPlanningActive)
		{
			Result = DONE;
			break;
		}

		//UpdateTLITargetTable();

		//MED string was previously saved
		GC->rtcc->GMGMED(GC->rtcc->RTCCMEDBUFFER);

		Result = DONE;
	}
	break;
	case 47: //Abort Scan Table
	{
		if (GC->MissionPlanningActive)
		{
			if (RTEASTType == 75)
			{
				GC->rtcc->GMGMED("F75;");
			}
			else if (RTEASTType == 76)
			{
				GC->rtcc->GMGMED("F76;");
			}
			else
			{
				GC->rtcc->GMGMED("F77;");
			}
		}
		else
		{
			VESSEL *v = GC->rtcc->pCSM;

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			if (RTEASTType == 75)
			{
				GC->rtcc->PZREAP.RTET0Min = GC->rtcc->GMTfromGET(GC->rtcc->med_f75_f77.T_0_min) / 3600.0;
			}
			else if (RTEASTType == 76)
			{
				bool found = GC->rtcc->DetermineRTESite(GC->rtcc->med_f76.Site);

				if (found == false)
				{
					Result = DONE;
					break;
				}

				//Check vector time
				//TBD: T_V greater than present time
				GC->rtcc->PZREAP.RTET0Min = GC->rtcc->GMTfromGET(GC->rtcc->med_f75_f77.T_0_min) / 3600.0;
				GC->rtcc->PZREAP.RTETimeOfLanding = GC->rtcc->GMTfromGET(GC->rtcc->med_f75_f77.T_Z) / 3600.0;
				GC->rtcc->PZREAP.RTEPTPMissDistance = GC->rtcc->med_f76.MissDistance;
			}
			else
			{
				if (GC->rtcc->med_f77.Site != "FCUA")
				{
					bool found = GC->rtcc->DetermineRTESite(GC->rtcc->med_f77.Site);

					if (found == false)
					{
						Result = DONE;
						break;
					}
				}

				//Check vector time
				//TBD: T_V greater than present time
				GC->rtcc->PZREAP.RTEVectorTime = GC->rtcc->GMTfromGET(GC->rtcc->med_f75_f77.T_V) / 3600.0;
				GC->rtcc->PZREAP.RTET0Min = GC->rtcc->GMTfromGET(GC->rtcc->med_f75_f77.T_0_min) / 3600.0;
				GC->rtcc->PZREAP.RTET0Max = GC->rtcc->GMTfromGET(GC->rtcc->med_f77.T_max) / 3600.0;
				GC->rtcc->PZREAP.RTETimeOfLanding = GC->rtcc->GMTfromGET(GC->rtcc->med_f75_f77.T_Z) / 3600.0;
				GC->rtcc->PZREAP.RTEPTPMissDistance = GC->rtcc->med_f77.MissDistance;
			}
			EphemerisData sv = GC->rtcc->StateVectorCalcEphem(v);
			GC->rtcc->PZREAP.RTEVectorTime = sv.GMT / 3600.0;
			GC->rtcc->PMMREAST(RTEASTType, &sv);
		}

		Result = DONE;
	}
	break;
	case 48: //LOI and MCC Transfer
	{
		if (GC->MissionPlanningActive)
		{
			//With the MPT, just call the MED function
			std::vector<std::string> data;
			GC->rtcc->PMMMED("78", data);
		}
		else
		{
			//Without the MPT, get the TIG and DV from the MCC or LOI table

			VECTOR3 dv;
			double gmt_tig;

			int num = GC->rtcc->med_m78.ManeuverNumber;

			if (GC->rtcc->med_m78.Type)
			{
				//LOI
				if (num < 1 || num > 8)
				{
					Result = DONE;
					break;
				}
				gmt_tig = GC->rtcc->PZLRBELM.sv_man_bef[num - 1].GMT;
				dv = GC->rtcc->PZLRBELM.V_man_after[num - 1] - GC->rtcc->PZLRBELM.sv_man_bef[num - 1].V;
			}
			else
			{
				//MCC
				if (num < 1 || num > 4)
				{
					Result = DONE;
					break;
				}
				gmt_tig = GC->rtcc->PZMCCXFR.sv_man_bef[num - 1].GMT;
				dv = GC->rtcc->PZMCCXFR.V_man_after[num - 1] - GC->rtcc->PZMCCXFR.sv_man_bef[num - 1].V;
			}

			VESSEL *v;
			EphemerisData sv_now, sv_tig;
			double mass, dt, attachedMass;
			int ITS;

			if (GC->rtcc->med_m78.Table == RTCC_MPT_CSM)
			{
				v = GC->rtcc->pCSM;
			}
			else
			{
				v = GC->rtcc->pLM;
			}

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			sv_now = GC->rtcc->StateVectorCalcEphem(v);
			mass = v->GetMass();

			//Propagate to TIG
			dt = gmt_tig - sv_now.GMT;
			GC->rtcc->PMMCEN(sv_now, 0.0, 0.0, 1, abs(dt), dt >= 0.0 ? 1.0 : -1.0, sv_tig, ITS);

			if (vesselisdocked)
			{
				attachedMass = GC->rtcc->GetDockedVesselMass(v);
			}
			else
			{
				attachedMass = 0.0;
			}
			GC->rtcc->PoweredFlightProcessor(sv_tig, mass, GC->rtcc->GETfromGMT(gmt_tig), GC->rtcc->med_m78.Thruster, attachedMass, dv, false, P30TIG, dV_LVLH);
		}

		Result = DONE;
	}
	break;
	case 49: //Transfer Maneuver to MPT from TTF, SCS, RTE
	{
		GC->rtcc->GMGMED(GC->rtcc->RTCCMEDBUFFER);

		Result = DONE;
	}
	break;
	case 50: //Lunar Targeting Program (S-IVB Lunar Impact)
	{
		if (iuvessel == NULL)
		{
			Result = DONE;
			break;
		}

		IU *iu = NULL;
		LVDCSV * lvdc = NULL;

		bool uplinkaccepted = false;

		if (utils::IsVessel(iuvessel, utils::SaturnV))
		{
			Saturn *iuv = (Saturn *)iuvessel;

			iu = iuv->GetIU();
		}
		else if (utils::IsVessel(iuvessel, utils::SaturnV_SIVB))
		{
			SIVB *iuv = (SIVB *)iuvessel;

			iu = iuv->GetIU();
		}
		if (iu == NULL)
		{
			Result = DONE;
			break;
		}
		lvdc = (LVDCSV*)((IUSV*)iu)->GetLVDC();

		if (lvdc == NULL)
		{
			Result = DONE;
			break;
		}

		if (lvdc->LVDC_Timebase != 8)
		{
			//TB8 not enabled yet
			LUNTAR_Output.err = 3;
			Result = DONE;
			break;
		}

		LunarTargetingProgramInput in;
		
		in.sv_in = GC->rtcc->StateVectorCalcEphem(iuvessel);
		in.mass = iuvessel->GetMass();
		in.lat_tgt = LUNTAR_lat;
		in.lng_tgt = LUNTAR_lng;
		in.bt_guess = LUNTAR_bt_guess;
		in.pitch_guess = LUNTAR_pitch_guess;
		in.yaw_guess = LUNTAR_yaw_guess;
		in.tig_guess = LUNTAR_TIG;
		in.TB8 = lvdc->TB8;

		LunarTargetingProgram luntar(GC->rtcc);
		luntar.Call(in, LUNTAR_Output);

		Result = DONE;
	}
	break;

	case 51: //Apollo Generalized Optics Program (RTACF)
	{
		AGOPInputs in;
		AGOPOutputs out;

		in.Option = GC->AGOP_Option;
		in.Mode = GC->AGOP_Mode;
		in.AdditionalOption = GC->AGOP_AdditionalOption;
		in.DeltaT = GC->AGOP_TimeStep;

		bool statevectorrequired = true;

		if (GC->AGOP_Option == 3) statevectorrequired = false;
		else if (GC->AGOP_Option == 7)
		{
			if (GC->AGOP_Mode == 3) statevectorrequired = false;
			else if (GC->AGOP_Mode == 4) statevectorrequired = false;
			else if (GC->AGOP_Mode == 6) statevectorrequired = false;
		}

		//Get ephemeris
		if (statevectorrequired)
		{
			EphemerisData sv;
			bool TimesNotRequired;

			VESSEL *v;
			if (GC->AGOP_AttIsCSM)
			{
				v = GC->rtcc->pCSM;
			}
			else
			{
				v = GC->rtcc->pLM;
			}

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			sv = GC->rtcc->StateVectorCalcEphem(v);

			TimesNotRequired = false;

			if (GC->AGOP_Option == 2 && GC->AGOP_Mode == 1) TimesNotRequired = true;
			if (GC->AGOP_Option == 7)
			{
				if (GC->AGOP_Mode == 1) TimesNotRequired = true;
				else if (GC->AGOP_Mode == 5) TimesNotRequired = true;
			}

			double GMT_Start, GMT_Stop;

			GMT_Start = GC->rtcc->GMTfromGET(GC->AGOP_StartTime);
			GMT_Stop = GC->rtcc->GMTfromGET(GC->AGOP_StopTime);

			//Get state vector to GMT_Start
			sv = GC->rtcc->coast(sv, GMT_Start - sv.GMT);

			if (TimesNotRequired)
			{
				//Only a single state vector required
				EphemerisData2 sv2;

				in.ephem.Header.CSI = sv.RBI == BODY_EARTH ? 0 : 2;

				sv2.R = sv.R;
				sv2.V = sv.V;
				sv2.GMT = sv.GMT;

				in.ephem.table.push_back(sv2);
			}
			else
			{
				//Write ephemeris

				EMSMISSInputTable intab;

				intab.AnchorVector = sv;
				intab.EphemerisBuildIndicator = true;

				if (sv.RBI == BODY_EARTH)
				{
					intab.ECIEphemerisIndicator = true;
					intab.ECIEphemTableIndicator = &in.ephem;
				}
				else
				{
					intab.MCIEphemerisIndicator = true;
					intab.MCIEphemTableIndicator = &in.ephem;
				}
				intab.EphemerisLeftLimitGMT = GMT_Start;
				intab.EphemerisRightLimitGMT = GMT_Stop;
				intab.DensityMultOverrideIndicator = 0.0;
				intab.ManeuverIndicator = false;
				intab.VehicleCode = GC->AGOP_AttIsCSM ? RTCC_MPT_CSM : RTCC_MPT_LM;

				GC->rtcc->EMSMISS(&intab);
			}

		}

		//Logic to get required REFSMMATs
		bool GetCSMREFSMMAT, GetLMREFSMMAT;

		GetCSMREFSMMAT = GC->AGOP_CSM_REFSMMAT_Required();
		GetLMREFSMMAT = GC->AGOP_LM_REFSMMAT_Required();

		if (GetCSMREFSMMAT)
		{
			REFSMMATData refs;
			
			if (GC->AGOP_Option == 7 && GC->AGOP_Mode == 6)
			{
				//Special case REFSMMAT to REFSMMAT calculation
				if (GC->AGOP_AttIsCSM)
				{
					refs = GC->rtcc->EZJGMTX1.data[GC->AGOP_CSM_REFSMMAT - 1];
				}
				else
				{
					refs = GC->rtcc->EZJGMTX3.data[GC->AGOP_CSM_REFSMMAT - 1];
				}
			}
			else
			{
				refs = GC->rtcc->EZJGMTX1.data[GC->AGOP_CSM_REFSMMAT - 1];
			}

			if (refs.ID == 0)
			{
				GC->AGOP_Output.clear();
				GC->AGOP_Error = "REFSMMAT NOT AVAILABLE";
				Result = DONE;
				break;
			}
			in.CSM_REFSMMAT = refs.REFSMMAT;
		}

		if (GetLMREFSMMAT)
		{
			REFSMMATData refs;
			
			if (GC->AGOP_Option == 7 && GC->AGOP_Mode == 6)
			{
				//Special case REFSMMAT to REFSMMAT calculation
				if (GC->AGOP_AttIsCSM)
				{
					refs = GC->rtcc->EZJGMTX1.data[GC->AGOP_LM_REFSMMAT - 1];
				}
				else
				{
					refs = GC->rtcc->EZJGMTX3.data[GC->AGOP_LM_REFSMMAT - 1];
				}
			}
			else
			{
				refs = GC->rtcc->EZJGMTX3.data[GC->AGOP_LM_REFSMMAT - 1];
			}
			

			if (refs.ID == 0)
			{
				GC->AGOP_Output.clear();
				GC->AGOP_Error = "REFSMMAT NOT AVAILABLE";
				Result = DONE;
				break;
			}
			in.LM_REFSMMAT = refs.REFSMMAT;
		}

		in.startable = GC->rtcc->EZJGSTAR;
		in.NumStars = 1;
		in.StarIDs[0] = GC->AGOP_Stars[0];
		in.StarIDs[1] = GC->AGOP_Stars[1];

		in.AttIsCSM = GC->AGOP_AttIsCSM;
		in.IMUAttitude[0] = GC->AGOP_Attitudes[0];
		in.IMUAttitude[1] = GC->AGOP_Attitudes[1];
		in.HeadsUp = GC->AGOP_HeadsUp;
		in.Instrument = GC->AGOP_Instrument;
		in.LMCOASAxis = GC->AGOP_LMCOASAxis;
		in.AOTDetent = GC->AGOP_LMAOTDetent - 1;

		if (in.Instrument == 0)
		{
			in.SextantShaftAngles[0] = GC->AGOP_InstrumentAngles1[0];
			in.SextantTrunnionAngles[0] = GC->AGOP_InstrumentAngles1[1];
			in.SextantShaftAngles[1] = GC->AGOP_InstrumentAngles2[0];
			in.SextantTrunnionAngles[1] = GC->AGOP_InstrumentAngles2[1];
		}
		else if (in.Instrument == 1 || in.Instrument == 3)
		{
			in.COASElevationAngle[0] = GC->AGOP_InstrumentAngles1[0];
			in.COASPositionAngle[0] = GC->AGOP_InstrumentAngles1[1];
			in.COASElevationAngle[1] = GC->AGOP_InstrumentAngles2[0];
			in.COASPositionAngle[1] = GC->AGOP_InstrumentAngles2[1];
		}
		else
		{
			in.AOTReticleAngle[0] = GC->AGOP_InstrumentAngles1[0];
			in.AOTSpiraleAngle[0] = GC->AGOP_InstrumentAngles1[1];
			in.AOTReticleAngle[1] = GC->AGOP_InstrumentAngles2[0];
			in.AOTSpiraleAngle[1] = GC->AGOP_InstrumentAngles2[1];
		}

		in.AntennaPitch = GC->AGOP_AntennaPitch;
		in.AntennaYaw = GC->AGOP_AntennaYaw;

		//For now, always input landmark
		in.GroundStationID = "";
		in.lmk_lat = GC->AGOP_Lat;
		in.lmk_lng = GC->AGOP_Lng;
		in.lmk_alt = GC->AGOP_Alt;

		AGOP agop(GC->rtcc);

		agop.Calc(in, out);

		GC->AGOP_Output = out.output_text;
		GC->AGOP_Error = out.errormessage;

		if (out.REFSMMAT_Vehicle != 0)
		{
			//Save REFSMMAT
			GC->AGOP_REFSMMAT = out.REFSMMAT;
			GC->AGOP_REFSMMAT_Vehicle = out.REFSMMAT_Vehicle;
		}

		Result = DONE;
	}
	break;
	case 52: //RTE Tradeoff Display
	{
		std::string mode;

		if (RTETradeoffMode == 0)
		{
			mode = "70";
		}
		else
		{
			mode = "71";
		}

		GC->rtcc->PMQAFMED(mode);

		Result = DONE;
	}
	break;
	case 53: //Central Manual Entry Device Decoder
	{
		GC->rtcc->GMGMED(GC->rtcc->RTCCMEDBUFFER);

		Result = DONE;
	}
	break;
	case 54: //Skylab Saturn IB Launch Targeting
	{
		if (Rendezvous_Target == NULL || GC->rtcc->GetGMTBase() == 0.0)
		{
			Result = DONE;
			break;
		}
		
		EphemerisData sv, sv_ECT;

		sv = GC->rtcc->StateVectorCalcEphem(Rendezvous_Target);

		GC->rtcc->ELVCNV(sv, 1, sv_ECT);

		GC->rtcc->PMMPAR(sv_ECT.R, sv_ECT.V, sv_ECT.GMT);

		Result = DONE;
	}
	break;
	case 55: //SLV Target Update Uplink
	{
		iuUplinkResult = DONE;

		if (GC->rtcc->PZSLVTAR.VIGM == 0.0)
		{
			iuUplinkResult = 4;
			Result = DONE;
			break;
		}

		IU *iu;

		if (utils::IsVessel(iuvessel, utils::SaturnIB))
		{
			Saturn *iuv = (Saturn *)iuvessel;
			iu = iuv->GetIU();
		}
		else if (utils::IsVessel(iuvessel, utils::SaturnIB_SIVB))
		{
			SIVB *iuv = (SIVB *)iuvessel;
			iu = iuv->GetIU();
		}
		else
		{
			iuUplinkResult = 2;
			Result = DONE;
			break;
		}

		void *uplink = NULL;
		DCSLAUNCHTARGET upl;

		upl.i = GC->rtcc->PZSLVTAR.IIGM;
		upl.lambda_0 = GC->rtcc->PZSLVTAR.TIGM;
		upl.lambda_dot = GC->rtcc->PZSLVTAR.TDIGM;
		upl.R_T = GC->rtcc->PZSLVTAR.RIGM;
		upl.theta_T = GC->rtcc->PZSLVTAR.GIGM*RAD;
		upl.T_GRR0 = GC->rtcc->PZSLVTAR.TGRR;
		upl.V_T = GC->rtcc->PZSLVTAR.VIGM;

		uplink = &upl;
		bool uplinkaccepted = iu->DCSUplink(DCSUPLINK_SATURNIB_LAUNCH_TARGETING, uplink);

		if (uplinkaccepted)
		{
			iuUplinkResult = 1;
		}
		else
		{
			iuUplinkResult = 3;
		}

		Result = DONE;
	}
	break;
	case 56: //Perigee Adjust
	{
		EphemerisData sv0;
		double mass, THT, dt, H_P, DPSScaleFactor;
		int Thruster;

		if (GC->MissionPlanningActive)
		{
			//TBD
			Result = DONE;
			break;
		}
		else
		{
			VESSEL *v;
			if (GC->rtcc->med_k28.VEH == RTCC_MPT_CSM)
			{
				v = GC->rtcc->pCSM;
			}
			else
			{
				v = GC->rtcc->pLM;
			}

			if (v == NULL)
			{
				Result = DONE;
				break;
			}

			sv0 = GC->rtcc->StateVectorCalcEphem(v);
			mass = v->GetMass();
		}

		THT = GC->rtcc->GMTfromGET(GC->rtcc->med_k28.ThresholdTime);
		dt = GC->rtcc->med_k28.TimeIncrement;
		H_P = GC->rtcc->med_k28.H_P*1852.0;
		Thruster = GC->rtcc->med_k28.Thruster;
		DPSScaleFactor = GC->rtcc->med_k28.DPSScaleFactor;

		AEGBlock sv1 = GC->rtcc->SVToAEG(sv0, 0.0, 1.0, 1.0); //TBD

		GC->rtcc->PMMPAD(sv1, mass, THT, dt, H_P, Thruster, DPSScaleFactor);
		GC->rtcc->PMDPAD();

		Result = DONE;
	}
	break;
	case 57: //Saturn V TLI Targeting Update
	{
		iuUplinkResult = DONE;

		if (GC->rtcc->PZTTLIPL.DataIndicator == 0)
		{
			iuUplinkResult = 4;
			Result = DONE;
			break;
		}

		IU *iu;

		if (utils::IsVessel(iuvessel, utils::SaturnV))
		{
			Saturn *iuv = (Saturn *)iuvessel;
			iu = iuv->GetIU();
		}
		else
		{
			iuUplinkResult = 2;
			Result = DONE;
			break;
		}

		void *uplink = NULL;
		DCSSLVTLITARGET upl;

		LVDCSV *lvdc = (LVDCSV*)iu->GetLVDC();

		SevenParameterUpdate coe = GC->rtcc->PZTTLIPL.elem;

		//Calculate time of restart preparation in TB5
		double GMT_TB5 = GC->rtcc->SystemParameters.MCGRIC*3600.0 + lvdc->TB5;
		double TIG_TB5 = coe.GMT_TIG - GMT_TB5;
		double T_RP_TB5 = TIG_TB5 - GC->rtcc->SystemParameters.MDVSTP.DTIG;

		upl.T_RP = T_RP_TB5;
		upl.alpha_D = coe.alpha_D;
		upl.C_3 = coe.C3;
		upl.e = coe.e;
		upl.f = coe.f;
		upl.Inclination = coe.Inclination;
		upl.theta_N = coe.theta_N;

		uplink = &upl;
		bool uplinkaccepted = iu->DCSUplink(DCSUPLINK_SLV_TLI_TARGETING_UPDATE, uplink);

		if (uplinkaccepted)
		{
			iuUplinkResult = 1;
		}
		else
		{
			iuUplinkResult = 3;
		}

		Result = DONE;
	}
	break;
	case 58: //Coelliptic ARM display
	{
		GC->rtcc->PZMARM.t_Calc_ARM = GC->rtcc->RTCCPresentTimeGMT();

		if (GC->rtcc->pCSM == NULL || GC->rtcc->pLM == NULL)
		{
			Result = DONE;
			break;
		}

		EphemerisData sv_CSM, sv_LM;

		sv_CSM = GC->rtcc->StateVectorCalcEphem(GC->rtcc->pCSM);
		sv_LM = GC->rtcc->StateVectorCalcEphem(GC->rtcc->pLM);

		GC->rtcc->PMDARM(sv_CSM, sv_LM);
	}
	break;
	case 59: //Short ARM display
	{
		GC->rtcc->PZMARM.t_Calc_ShortARM = GC->rtcc->RTCCPresentTimeGMT();

		if (GC->rtcc->pCSM == NULL || GC->rtcc->pLM == NULL)
		{
			Result = DONE;
			break;
		}

		EphemerisData sv_CSM, sv_LM;

		sv_CSM = GC->rtcc->StateVectorCalcEphem(GC->rtcc->pCSM);
		sv_LM = GC->rtcc->StateVectorCalcEphem(GC->rtcc->pLM);

		GC->rtcc->PMDSARM(sv_CSM, sv_LM);
	}
	break;
	}

	subThreadStatus = Result;

	return(0);
}

void ARCore::DetermineGMPCode()
{
	int code = 0;

	if (GMPManeuverType == 0)
	{
		if (GMPManeuverPoint == 1)
		{
			//PCE: Plane Change at equatorial crossing
			code = 1;
		}
		else if (GMPManeuverPoint == 3)
		{
			//PCL: Plane change at a specified longitude
			code = 2;
		}
		else if (GMPManeuverPoint == 4)
		{
			//PCH: Plane change at a height
			code = 49;
		}
		else if (GMPManeuverPoint == 5)
		{
			//PCT: Plane change at a specified time
			code = 3;
		}
	}
	else if (GMPManeuverType == 1)
	{
		if (GMPManeuverPoint == 0)
		{
			//CRA: Circularization maneuver at apogee
			code = 41;
		}
		else if (GMPManeuverPoint == 2)
		{
			//CRA: Circularization maneuver at perigee
			code = 42;
		}
		else if (GMPManeuverPoint == 3)
		{
			//CRL: Circularization maneuver at a specified longitude
			code = 4;
		}
		else if (GMPManeuverPoint == 4)
		{
			//CRH: Circularization maneuver at a specified height
			code = 5;
		}
		else if (GMPManeuverPoint == 5)
		{
			//CRT: Circularization maneuver at a specified time
			code = 40;
		}
	}
	else if (GMPManeuverType == 2)
	{
		if (GMPManeuverPoint == 0)
		{
			//HAO: Height maneuver at apogee
			code = 8;
		}
		else if (GMPManeuverPoint == 2)
		{
			//HPO: Height maneuver at perigee
			code = 9;
		}
		else if (GMPManeuverPoint == 3)
		{
			//HOL: Height maneuver at a specified longitude
			code = 6;
		}
		else if (GMPManeuverPoint == 4)
		{
			//HOH: Height maneuver at a height
			code = 52;
		}
		else if (GMPManeuverPoint == 5)
		{
			//HOL: Height maneuver at a specified time
			code = 7;
		}
	}
	else if (GMPManeuverType == 3)
	{
		if (GMPManeuverPoint == 4)
		{
			//NSH: Node shift maneuver at a height
			code = 50;
		}
		else if (GMPManeuverPoint == 3)
		{
			//NSL: Node shift maneuver at a longitude
			code = 51;
		}
		else if (GMPManeuverPoint == 5)
		{
			//NST: Node shift maneuver at a specified time
			code = 10;
		}
		else if (GMPManeuverPoint == 6)
		{
			//NSO: Optimum node shift maneuver
			code = 11;
		}
	}
	else if (GMPManeuverType == 4)
	{
		if (GMPManeuverPoint == 3)
		{
			//HBL: Maneuver to change both apogee and perigee at a specified longitude
			code = 33;
		}
		else if (GMPManeuverPoint == 4)
		{
			//HBH: Maneuver to change both apogee and perigee at a specified height
			code = 13;
		}
		else if (GMPManeuverPoint == 5)
		{
			//HBT: Maneuver to change both apogee and perigee at a specified time
			code = 12;
		}
		else if (GMPManeuverPoint == 6)
		{
			//HBO: Optimum maneuver to change both apogee and perigee
			code = 14;
		}
	}
	else if (GMPManeuverType == 5)
	{
		if (GMPManeuverPoint == 0)
		{
			//FCA: Input maneuver at an apogee
			code = 18;
		}
		else if (GMPManeuverPoint == 1)
		{
			//FCE: Input maneuver at an equatorial crossing
			code = 20;
		}
		else if (GMPManeuverPoint == 2)
		{
			//FCP: Input maneuver at an perigee
			code = 19;
		}
		else if (GMPManeuverPoint == 3)
		{
			//FCL: Input maneuver at a specified longitude
			code = 16;
		}
		else if (GMPManeuverPoint == 4)
		{
			//FCH: Input maneuver at a specified height
			code = 17;
		}
		else if (GMPManeuverPoint == 5)
		{
			//FCT: Input maneuver at a specified time
			code = 15;
		}
	}
	else if (GMPManeuverType == 6)
	{
		if (GMPManeuverPoint == 3)
		{
			//NHL: Combination maneuver to change both apogee and perigee and shift the node at a specified longitude
			code = 22;
		}
		else if (GMPManeuverPoint == 5)
		{
			//NHT: Combination maneuver to change both apogee and perigee and shift the node at a specified time
			code = 21;
		}
	}
	else if (GMPManeuverType == 7)
	{
		if (GMPManeuverPoint == 0)
		{
			code = RTCC_GMP_SAA;
		}
		else if (GMPManeuverPoint == 3)
		{
			code = RTCC_GMP_SAL;
		}
		else if (GMPManeuverPoint == 5)
		{
			//SAT: Maneuver to shift line-of-apsides some angle at a specified time
			code = 31;
		}
		else if (GMPManeuverPoint == 6)
		{
			//SAO: Maneuver to shift line-of-apsides some angle and keep the same apogee and perigee altitudes
			code = 32;
		}
	}
	else if (GMPManeuverType == 8)
	{
		if (GMPManeuverPoint == 0)
		{
			//PHA: Combination height maneuver and a plane change at an apogee
			code = 27;
		}
		else if (GMPManeuverPoint == 2)
		{
			//PHP: Combination height maneuver and a plane change at an perigee
			code = 28;
		}
		else if (GMPManeuverPoint == 3)
		{
			//PHL: Combination height maneuver and a plane change at a specified longitude
			code = 25;
		}
		else if (GMPManeuverPoint == 5)
		{
			//PHT: Combination height maneuver and a plane change at a specified time
			code = 26;
		}
	}
	else if (GMPManeuverType == 9)
	{
		if (GMPManeuverPoint == 0)
		{
			//CPA: Combination circularization maneuver and a plane change at apogee
			code = 44;
		}
		else if (GMPManeuverPoint == 2)
		{
			//CPP: Combination circularization maneuver and a plane change at perigee
			code = 45;
		}
		else if (GMPManeuverPoint == 3)
		{
			//CPL: Combination circularization maneuver and a plane change at a specified longitude
			code = 29;
		}
		else if (GMPManeuverPoint == 4)
		{
			//CPH: Combination circularization maneuver and a plane change at a specified altitude
			code = 30;
		}
		else if (GMPManeuverPoint == 5)
		{
			//CPT: Combination circularization maneuver and a plane change at a specified time
			code = 43;
		}
	}
	else if (GMPManeuverType == 10)
	{
		if (GMPManeuverPoint == 0)
		{
			//CNA: Circularization and node shift at apogee
			code = 47;
		}
		else if (GMPManeuverPoint == 2)
		{
			//CNP: Circularization and node shift at perigee
			code = 48;
		}
		else if (GMPManeuverPoint == 3)
		{
			//CNL: Circularization and node shift at a specified longitude
			code = 34;
		}
		else if (GMPManeuverPoint == 4)
		{
			//CNH: Circularization and node shift at a specified height
			code = 35;
		}
		else if (GMPManeuverPoint == 5)
		{
			//CNT: Circularization and node shift maneuver at a specified time
			code = 46;
		}
	}
	else if (GMPManeuverType == 11)
	{
		if (GMPManeuverPoint == 0)
		{
			//HNA: Height maneuver and node shift at apogee
			code = 38;
		}
		else if (GMPManeuverPoint == 2)
		{
			//HNP: Height maneuver and node shift at perigee
			code = 39;
		}
		else if (GMPManeuverPoint == 3)
		{
			//HNL: Height maneuver and node shift at a specified longitude
			code = 36;
		}
		else if (GMPManeuverPoint == 5)
		{
			//HNT: Height maneuver and node shift at a specified time
			code = 37;
		}
	}
	else if (GMPManeuverType == 12)
	{
		if (GMPManeuverPoint == 6)
		{
			//HAS
			code = RTCC_GMP_HAS;
		}
	}

	GMPManeuverCode = code;
}

int ARCore::GetVesselParameters(bool IsCSM, int docked, int Thruster, int &Config, int &TVC, double &CSMMass, double &LMMass)
{
	//Select vessel type
	VESSEL *v;
	int vestype;

	if (IsCSM)
	{
		v = GC->rtcc->pCSM;
		vestype = 0;
	}
	else
	{
		v = GC->rtcc->pLM;
		vestype = 1;
	}

	if (v == NULL) return 1;

	//Error checking
	if (Thruster == RTCC_ENGINETYPE_CSMSPS || Thruster == RTCC_ENGINETYPE_CSMRCSMINUS2 || Thruster == RTCC_ENGINETYPE_CSMRCSPLUS2 || Thruster == RTCC_ENGINETYPE_CSMRCSMINUS4 || Thruster == RTCC_ENGINETYPE_CSMRCSPLUS4)
	{
		//CSM thruster
		if (vestype != 0) return 1;

		TVC = 1;
	}
	else
	{
		//LM thruster
		if (vestype != 1) return 1;

		TVC = 3;
	}

	MED_M50 m50;
	MED_M55 m55;
	MED_M49 m49;

	GC->rtcc->MPTMassUpdate(v, m50, m55, m49, docked);

	std::bitset<4> cfg;
	GC->rtcc->MPTGetConfigFromString(m55.ConfigCode, cfg);

	Config = cfg.to_ulong();
	CSMMass = m50.CSMWT;
	LMMass = m50.LMWT;

	return 0;
}

int ARCore::menuCalculateIMUComparison(bool IsCSM)
{
	MATRIX3 M_BRCS_SM; //BRCS to stable member, right handed
	MATRIX3 M_NB_ECL; //Local vessel to global ecliptic
	MATRIX3 M_SM_NB_est; //Stable member to navigation base, estimated
	MATRIX3 M_SM_NB_act; //Stable member to navigation base, actual
	MATRIX3 M_ECL_BRCS; //Ecliptic to BRCS
	VECTOR3 IMUAngles;
	VESSEL *v;

	if (IsCSM)
	{
		v = GC->rtcc->pCSM;
	}
	else
	{
		v = GC->rtcc->pLM;
	}

	if (v == NULL) return 1;

	if (IsCSM)
	{
		if (utils::IsVessel(v, utils::Saturn) == false) return 2;

		M_BRCS_SM = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
		IMUAngles = ((Saturn*)v)->imu.GetTotalAttitude();

		//Get actual orientation (left handed)
		v->GetRotationMatrix(M_NB_ECL);
		//Convert to right-handed CSM coordinates
		M_NB_ECL = mul(MatrixRH_LH(M_NB_ECL), _M(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0));
	}
	else
	{
		if (utils::IsVessel(v, utils::LEM) == false) return 2;

		M_BRCS_SM = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
		IMUAngles = ((LEM*)v)->imu.GetTotalAttitude();

		//Get actual orientation (left handed)
		v->GetRotationMatrix(M_NB_ECL);
		//Convert to right-handed LM coordinates
		M_NB_ECL = mul(MatrixRH_LH(M_NB_ECL), _M(0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0));
	}

	//Stable member to navigation base conversion with current IMU alignment
	M_SM_NB_est = OrbMech::CALCSMSC(IMUAngles);
	//Get ecliptic to BRCS rotation matrix from RTCC system parameters
	M_ECL_BRCS = GC->rtcc->SystemParameters.MAT_J2000_BRCS;
	//Actual stable member to navigation base conversion
	M_SM_NB_act = OrbMech::tmat(mul(M_BRCS_SM, mul(M_ECL_BRCS, M_NB_ECL)));
	//Torquing angles that would be required
	DebugIMUTorquingAngles = OrbMech::CALCGTA(mul(OrbMech::tmat(M_SM_NB_act), M_SM_NB_est));

	return 0;
}

void ARCore::menuCalculateIMUParkingAngles(agc_t* agc)
{
	//Hardcoded address for Luminary1E
	int GravVecAddr = 145;
	double GravVecDec[3];
	double IG, MG, OG;

	for (int i = 0; i < 6; i++)
	{
		GravVec[i] = agc->Erasable[4][GravVecAddr + i];
	}

	//Convert 6 signed octal values into 3 decimal values
	for (int i = 0; i < 3; i++)
	{
		if (GravVec[2 * i] & 040000)
		{
			GravVecDec[i] = -((((GravVec[2 * i] ^ 077777) << 14) | (GravVec[2 * i + 1] ^ 077777)) / pow(2, 28)) * 2;
		}
		else {
			GravVecDec[i] = (((GravVec[2 * i] << 14) | GravVec[2 * i + 1]) / pow(2, 28)) * 2;
		}

	}

	IG = 0.0;
	MG = asin(GravVecDec[0]);
	OG = fmod(atan2(GravVecDec[1] / cos(MG), (-GravVecDec[2]) / cos(MG)) + 2 * PI, 2 * PI); //fmod needed to keep range between 0 and 360 deg

	IMUParkingAngles = _V(OG, IG, MG);
}