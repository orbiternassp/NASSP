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
	MPTVesselNumber = -1;
	pMPTVessel = NULL;
	mptInitError = 0;

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
	if (pMPTVessel == NULL) return;

	rtcc->MPTMassUpdate(pMPTVessel, rtcc->med_m50, rtcc->med_m55, rtcc->med_m49);
}

ARCore::ARCore(VESSEL* v, AR_GCore* gcin)
{
	GC = gcin;

	SPQMode = 0;
	CSItime = 0.0;
	CDHtime = 0.0;
	SPQTIG = 0.0;
	CDHtimemode = 0;
	this->vessel = v;
	t_TPI = 0.0;

	SPQDeltaV = _V(0, 0, 0);
	target = NULL;
	//screen = 0;
	targetnumber = -1;
	AGCEphemTEphemZero = 40038.0;
	REFSMMAT_LVLH_Time = 0.0;
	REFSMMATopt = 4;
	REFSMMATcur = 4;
	manpadopt = 0;
	lemdescentstage = true;
	mptinitmode = 3;

	vesseltype = -1;

	if (utils::IsVessel(v,utils::Saturn))
	{
		vesseltype = 0;
	}
	else if (utils::IsVessel(v, utils::LEM))
	{
		vesseltype = 1;
	}
	else if (utils::IsVessel(v, utils::MCC) || utils::IsVessel(v, utils::SaturnIB_SIVB) || utils::IsVessel(v, utils::SaturnV_SIVB))
	{
		vesseltype = 2;
	}

	AGCEphemTEphemZero = GC->rtcc->SystemParameters.TEPHEM0;

	vesselisdocked = false;
	if (vessel->DockingStatus(0) == 1)
	{
		vesselisdocked = true;
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
	if (vesseltype == 1)
	{
		LEM *lem = (LEM *)vessel;
		if (lem->GetStage() < 2)
		{
			lemdescentstage = true;
		}
		else
		{
			lemdescentstage = false;
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

	EntryTIGcor = 0.0;
	EntryLatcor = 0.0;
	EntryLngcor = 0.0;
	Entry_DV = _V(0.0, 0.0, 0.0);
	RTEReentryTime = 0.0;
	entryrange = GC->rtcc->PZREAP.RRBIAS;
	RTECalcMode = 1;
	RTETradeoffMode = 0;
	RTEASTType = 76;

	SVSlot = true; //true = CSM; false = Other
	SVDesiredGET = -1;
	manpad.Trun = 0.0;
	manpad.Shaft = 0.0;
	manpad.Star = 0;
	manpad.BSSStar = 0;
	manpad.SPA = 0.0;
	manpad.SXP = 0.0;
	manpad.Att = _V(0, 0, 0);
	manpad.GDCangles = _V(0, 0, 0);
	//GDCset = 0;
	manpad.SetStars[0] = 0;
	manpad.SetStars[1] = 0;
	HeadsUp = false;
	manpad.HA = 0.0;
	manpad.HP = 0.0;
	manpad.Weight = 0.0;
	manpad.burntime = 0.0;
	manpad.Vt = 0.0;
	manpad.Vc = 0.0;
	manpad.pTrim = 0.0;
	manpad.yTrim = 0.0;
	manpad.LMWeight = 0.0;
	lmmanpad.Att = _V(0, 0, 0);
	lmmanpad.BSSStar = 0;
	lmmanpad.burntime = 0.0;
	lmmanpad.CSMWeight = 0.0;
	lmmanpad.dV = _V(0, 0, 0);
	lmmanpad.dVR = 0.0;
	lmmanpad.dV_AGS = _V(0, 0, 0);
	lmmanpad.GETI = 0.0;
	lmmanpad.HA = 0.0;
	lmmanpad.HP = 0.0;
	lmmanpad.LMWeight = 0.0;
	lmmanpad.SPA = 0.0;
	lmmanpad.SXP = 0.0;
	lmmanpad.IMUAtt = _V(0, 0, 0);
	sprintf(lmmanpad.remarks, "");
	entrypadopt = 0;
	EntryPADSxtStarCheckAttOpt = true;
	manpadenginetype = RTCC_ENGINETYPE_CSMSPS;
	TPI_PAD.AZ = 0.0;
	TPI_PAD.dH_TPI = 0.0;
	TPI_PAD.Backup_dV = _V(0.0, 0.0, 0.0);
	TPI_PAD.EL = 0.0;
	TPI_PAD.R = 0.0;
	TPI_PAD.Rdot = 0.0;
	TPI_PAD.dH_Max = 0.0;
	TPI_PAD.Backup_bT = _V(0.0, 0.0, 0.0);
	sxtstardtime = 0.0;
	manpad_ullage_dt = 0.0;
	manpad_ullage_opt = true;

	mapupdate.LOSGET = 0.0;
	mapupdate.AOSGET = 0.0;
	mapupdate.SSGET = 0.0;
	mapupdate.SRGET = 0.0;
	mapupdate.PMGET = 0.0;
	mappage = 1;
	mapgs = 0;
	mapUpdateGET = 0.0;
	GSAOSGET = 0.0;
	GSLOSGET = 0.0;
	PADSolGood = true;
	svtarget = NULL;
	svtargetnumber = -1;
	TLCCSolGood = true;

	landingzone = 0;
	entryprecision = -1;
	
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

	subThreadMode = 0;
	subThreadStatus = DONE;

	LmkLat = 0;
	LmkLng = 0;
	LmkTime = 0;
	landmarkpad.T1[0] = 0;
	landmarkpad.T2[0] = 0;
	landmarkpad.CRDist[0] = 0;
	landmarkpad.Alt[0] = 0;
	landmarkpad.Lat[0] = 0;
	landmarkpad.Lng05[0] = 0;

	VECoption = 0;
	VECdirection = 0;
	VECbody = NULL;
	VECangles = _V(0, 0, 0);

	DOI_dV_LVLH = _V(0, 0, 0);

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
	AscentPADVersion = 0;
	t_TPIguess = 0.0;

	EMPUplinkNumber = 1;
	EMPUplinkMaxNumber = 0;

	LVDCLaunchAzimuth = 0.0;

	AGCEphemOption = 0;
	AGCEphemBRCSEpoch = GC->rtcc->SystemParameters.AGCEpoch;
	AGCEphemTIMEM0 = floor(GC->rtcc->CalcGETBase()) + 6.75;
	AGCEphemTEPHEM = floor(GC->rtcc->CalcGETBase() - 0.5) + 0.5; //Noon before launch
	AGCEphemTLAND = 4.5;
	AGCEphemMission = 11; // GC->mission; TBD
	AGCEphemIsCMC = vesseltype != 1;

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

	AGOP_Page = 1;
	AGOP_Option = 1;
	AGOP_Mode = 1;
	AGOP_StartTime = 0.0;
	AGOP_StopTime = 0.0;
	AGOP_TimeStep = 0.0;
	AGOP_CSM_REFSMMAT = RTCC_REFSMMAT_TYPE_CUR;
	AGOP_LM_REFSMMAT = RTCC_REFSMMAT_TYPE_CUR;
	AGOP_Star = 1;
	AGOP_Lat = 0.0;
	AGOP_Lng = 0.0;
	AGOP_Alt = 0.0;
	AGOP_Attitude = _V(0, 0, 0);
	AGOP_AttIsCSM = true;
	AGOP_HeadsUp = true;
	AGOP_AntennaPitch = 0.0;
	AGOP_AntennaYaw = 0.0;

	DebugIMUTorquingAngles = _V(0, 0, 0);
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
	SV sv0;
	EntryResults res;

	sv0 = GC->rtcc->StateVectorCalc(vessel);
	GC->rtcc->EntryUpdateCalc(sv0, entryrange, true, &res);

	EntryLatcor = res.latitude;
	EntryLngcor = res.longitude;
}

void ARCore::EntryCalc()
{
	startSubthread(7);
}

void ARCore::DeorbitCalc()
{
	startSubthread(17);
}

void ARCore::SPQcalc()
{
	startSubthread(2);
}

void ARCore::lambertcalc()
{
	startSubthread(1);
}

void ARCore::GPMPCalc()
{
	startSubthread(3);
}

void ARCore::REFSMMATCalc()
{
	startSubthread(4);
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

void ARCore::DKICalc()
{
	startSubthread(19);
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

void ARCore::DAPPADCalc()
{
	if (vesseltype == 0)
	{
		GC->rtcc->CSMDAPUpdate(vessel, DAP_PAD, vesselisdocked);
	}
	else if (vesseltype == 1)
	{
		GC->rtcc->LMDAPUpdate(vessel, DAP_PAD, vesselisdocked, lemdescentstage == false);
	}
}

void ARCore::GenerateAGCEphemeris()
{
	AGCEphemeris(AGCEphemTIMEM0, AGCEphemBRCSEpoch, AGCEphemTEphemZero);
}

void ARCore::GenerateAGCCorrectionVectors()
{
	AGCCorrectionVectors(AGCEphemTEPHEM, -0.5, AGCEphemTLAND, AGCEphemMission, AGCEphemIsCMC);
}

void ARCore::EntryPAD()
{
	startSubthread(31);
}

void ARCore::ManeuverPAD()
{
	startSubthread(9);
}

void ARCore::TPIPAD()
{
	startSubthread(6);
}

void ARCore::MapUpdate()
{
	startSubthread(32);
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
	OrbMech::SStoHHMMSS(T_L, hh, mm, ss);
	sprintf_s(Buff, "P12,IU1,%d:%d:%.2lf,%.3lf;", hh, mm, ss, Azi*DEG);
	GC->rtcc->GMGMED(Buff);
}

void ARCore::GetStateVectorFromIU()
{
	bool isSaturnV;
	IU* iu;

	if (utils::IsVessel(vessel, utils::SaturnV))
	{
		Saturn *iuv = (Saturn *)vessel;
		iu = iuv->GetIU();
		isSaturnV = true;
	}
	else if (utils::IsVessel(vessel, utils::SaturnIB))
	{
		Saturn *iuv = (Saturn *)vessel;
		iu = iuv->GetIU();
		isSaturnV = false;
	}
	else if (utils::IsVessel(vessel, utils::SaturnV_SIVB))
	{
		SIVB *iuv = (SIVB *)vessel;
		iu = iuv->GetIU();
		isSaturnV = true;
	}
	else if (utils::IsVessel(vessel, utils::SaturnIB_SIVB))
	{
		SIVB *iuv = (SIVB *)vessel;
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
	//Are we a LM?
	if (vesseltype != 1) return;

	//0-6: pos and vel
	int csmvecoct[6], lmvecoct[6];
	int timeoct[2];

	LEM *lem = (LEM *)vessel;

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
	if (GC->rtcc->AGCGravityRef(vessel) == oapiGetObjectByName("Moon"))
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

void ARCore::GetStateVectorFromAGC(bool csm)
{
	if (vesseltype < 0 || vesseltype > 1) return;

	agc_t* vagc;

	if (vesseltype == 0)
	{
		Saturn *saturn = (Saturn *)vessel;

		vagc = &saturn->agc.vagc;
	}
	else
	{
		LEM *lem = (LEM *)vessel;

		vagc = &lem->agc.vagc;
	}

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
	if (vesseltype == 0)
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
		if (vesseltype == 0)
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
		if (vesseltype == 0)
		{
			GC->rtcc->BZSTLM.HighSpeedCMCLEMVector = sv;
		}
		else
		{
			GC->rtcc->BZSTLM.HighSpeedLGCLEMVector = sv;
		}
	}
}

void ARCore::NavCheckPAD()
{
	SV sv;

	sv = GC->rtcc->StateVectorCalc(vessel);

	GC->rtcc->NavCheckPAD(sv, navcheckpad, navcheckpad.NavChk[0]);
}

void ARCore::LandingSiteUpdate()
{
	double lat, lng, rad;
	svtarget->GetEquPos(lng, lat, rad);

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
	if (type == 0 || type == 21)
	{
		mptveh = RTCC_MPT_CSM;
	}
	else
	{
		mptveh = RTCC_MPT_LM;
	}

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
		sv0 = GC->rtcc->StateVectorCalcEphem(svtarget);
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

void ARCore::AGSStateVectorCalc()
{
	AGSSVOpt opt;
	EphemerisData sv;

	sv = GC->rtcc->StateVectorCalcEphem(svtarget);

	opt.csm = SVSlot;
	opt.REFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
	opt.sv = sv;
	opt.landed = svtarget->GroundContact();

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
	GC->rtcc->CMMENTRY(EntryLatcor, EntryLngcor);
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
		if (vesseltype != 0) return; //Not a CSM
	}
	else
	{
		IsCMC = false;
		if (vesseltype != 1) return; //Not a LM
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

void ARCore::VecPointCalc()
{
	if (VECoption == 0)
	{
		if (VECbody == NULL) return;

		VECTOR3 vPos, pPos, relvec, UX, UY, UZ, loc;
		MATRIX3 M, M_R;
		double p_T, y_T;
		OBJHANDLE gravref = GC->rtcc->AGCGravityRef(vessel);

		p_T = 0;
		y_T = 0;

		if (VECdirection == 1)
		{
			p_T = PI;
			y_T = 0;
		}
		else if (VECdirection == 2)
		{
			p_T = 0;
			y_T = PI05;
		}
		else if (VECdirection == 3)
		{
			p_T = 0;
			y_T = -PI05;
		}
		else if (VECdirection == 4)
		{
			p_T = -PI05;
			y_T = 0;
		}
		else if (VECdirection == 5)
		{
			p_T = PI05;
			y_T = 0;
		}

		vessel->GetGlobalPos(vPos);
		oapiGetGlobalPos(VECbody, &pPos);
		vessel->GetRelativePos(gravref, loc);

		relvec = unit(pPos - vPos);
		relvec = mul(GC->rtcc->SystemParameters.MAT_J2000_BRCS, _V(relvec.x, relvec.z, relvec.y));
		loc = mul(GC->rtcc->SystemParameters.MAT_J2000_BRCS, _V(loc.x, loc.z, loc.y));

		UX = relvec;
		UY = unit(crossp(UX, -loc));
		UZ = unit(crossp(UX, crossp(UX, -loc)));

		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));

		VECangles = OrbMech::CALCGAR(GC->rtcc->EZJGMTX1.data[0].REFSMMAT, mul(OrbMech::tmat(M), M_R));
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

int ARCore::startSubthread(int fcn) {
	if (IsReady(subThreadStatus)) {
		// Punt thread
		subThreadMode = fcn;
		subThreadStatus = SCHEDULED;
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

	int mptveh;

	if (vesseltype == 0)
	{
		mptveh = RTCC_MPT_CSM;
	}
	else
	{
		mptveh = RTCC_MPT_LM;
	}

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
			sv_A = GC->rtcc->StateVectorCalcEphem(vessel);
			sv_P = GC->rtcc->StateVectorCalcEphem(target);
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
			if (target == NULL)
			{
				Result = DONE;
				break;
			}

			sv_A = GC->rtcc->StateVectorCalc(vessel);
			sv_P = GC->rtcc->StateVectorCalc(target);
		}

		opt.DH = GC->rtcc->GZGENCSN.SPQDeltaH;
		opt.E = GC->rtcc->GZGENCSN.SPQElevationAngle;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.WT = GC->rtcc->GZGENCSN.SPQTerminalPhaseAngle;
		if (GC->MissionPlanningActive)
		{
			opt.ChaserID = GC->rtcc->med_k01.ChaserVehicle;
		}
		else
		{
			if (vesseltype == 0)
			{
				opt.ChaserID = 1;
			}
			else
			{
				opt.ChaserID = 3;
			}
		}
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
			sv0 = GC->rtcc->StateVectorCalcEphem(vessel);
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

		//For LS REFSMMAT use target vessel, if we are not in the CSM
		if (GC->MissionPlanningActive == false && vesseltype != 0 && REFSMMATopt == 5)
		{
			if (target == NULL)
			{
				Result = DONE;
				break;
			}

			opt.vessel = target;
		}
		else
		{
			opt.vessel = vessel;
		}

		if (vesseltype == 0)
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
		if (vesseltype == 0)
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

			if (REFSMMATopt == 0 || REFSMMATopt == 1 || REFSMMATopt == 2 || REFSMMATopt == 5)
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
		if (vesseltype == 0)
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
			SV sv0_apo = GC->rtcc->StateVectorCalc(vessel);
			sv0.R = sv0_apo.R;
			sv0.V = sv0_apo.V;
			sv0.GMT = OrbMech::GETfromMJD(sv0_apo.MJD, GC->rtcc->GetGMTBase());
			if (sv0_apo.gravref == oapiGetObjectByName("Earth"))
			{
				sv0.RBI = BODY_EARTH;
			}
			else
			{
				sv0.RBI = BODY_MOON;
			}
		}

		GC->rtcc->PMMLRBTI(sv0);

		Result = DONE;
	}
	break;
	case 6: //TPI PAD
	{
		AP7TPIPADOpt opt;

		opt.dV_LVLH = dV_LVLH;
		opt.TIG = P30TIG;
		opt.sv_A = GC->rtcc->StateVectorCalcEphem(vessel);
		opt.sv_P = GC->rtcc->StateVectorCalcEphem(target);
		opt.mass = vessel->GetMass();

		GC->rtcc->AP7TPIPAD(opt, TPI_PAD);

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
			MED_M50 med1;
			MED_M55 med2;
			MED_M49 med3;
			//This doesn't work in debug mode (with only RTCC MFD and MCC modules build), so below are some fake masses
			GC->rtcc->MPTMassUpdate(vessel, med1, med2, med3);

			GC->rtcc->VEHDATABUF.csmmass = med1.CSMWT;//vessel->GetMass();//
			GC->rtcc->VEHDATABUF.lmascmass = med1.LMASCWT;//0.0;10000.0*0.453;//
			GC->rtcc->VEHDATABUF.lmdscmass = med1.LMWT - med1.LMASCWT;//0.0;25000.0*0.453;//
			GC->rtcc->VEHDATABUF.sv = GC->rtcc->StateVectorCalcEphem(vessel);
			GC->rtcc->VEHDATABUF.config = med2.ConfigCode;//"CL";//"C";//

			GC->rtcc->PMMREDIG(false);
		}

		Result = DONE;
	}
	break;
	case 8: //TLI PAD
	{
		LVDCSV *lvdc = NULL;
		if (utils::IsVessel(vessel, utils::SaturnV))
		{
			SaturnV * SatV = (SaturnV *)vessel;
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

		opt.ConfigMass = vessel->GetMass();
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
		opt.sv0 = GC->rtcc->StateVectorCalcEphem(vessel);

		GC->rtcc->TLI_PAD(opt, tlipad);

		Result = DONE;
	}
	break;
	case 9: //Maneuver PAD
	{
		SV sv_A;

		if (GC->MissionPlanningActive)
		{
			double GMT = GC->rtcc->GMTfromGET(P30TIG);
			EphemerisData EPHEM;
			if (GC->rtcc->EMSFFV(GMT, mptveh, EPHEM))
			{
				Result = DONE;
				break;
			}
			sv_A.R = EPHEM.R;
			sv_A.V = EPHEM.V;
			sv_A.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv_A.gravref = GC->rtcc->GetGravref(EPHEM.RBI);

			PLAWDTInput pin;
			PLAWDTOutput pout;
			pin.T_UP = GMT;
			pin.TableCode = mptveh;
			GC->rtcc->PLAWDT(pin, pout);

			if (vesseltype == 0)
			{
				sv_A.mass = pout.CSMWeight;
			}
			else
			{
				sv_A.mass = pout.LMAscWeight + pout.LMDscWeight;
			}
		}
		else
		{
			sv_A = GC->rtcc->StateVectorCalc(vessel);
		}

		if (vesseltype == 0)
		{
			AP11ManPADOpt opt;

			opt.dV_LVLH = dV_LVLH;
			opt.enginetype = manpadenginetype;
			opt.HeadsUp = HeadsUp;
			opt.REFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
			opt.sxtstardtime = sxtstardtime;
			opt.TIG = P30TIG;
			opt.vessel = vessel;
			if (vesselisdocked)
			{
				opt.vesseltype = 1;
			}
			else
			{
				opt.vesseltype = 0;
			}
			opt.R_LLS = GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST];
			opt.useSV = true;
			opt.RV_MCC = sv_A;
			opt.UllageDT = manpad_ullage_dt;
			opt.UllageThrusterOpt = manpad_ullage_opt;

			GC->rtcc->AP11ManeuverPAD(&opt, manpad);
		}
		else
		{
			AP11LMManPADOpt opt;

			opt.dV_LVLH = dV_LVLH;
			opt.enginetype = manpadenginetype;
			opt.HeadsUp = HeadsUp;
			opt.REFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
			opt.sxtstardtime = sxtstardtime;
			opt.TIG = P30TIG;
			opt.vessel = vessel;
			opt.csmlmdocked = !GC->MissionPlanningActive && vesselisdocked;
			opt.R_LLS = GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST];
			opt.useSV = true;
			opt.RV_MCC = sv_A;

			GC->rtcc->AP11LMManeuverPAD(&opt, lmmanpad);
		}

		Result = DONE;
	}
	break;
	case 10: //Lunar Descent Planning Processor
	{
		SV sv;

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

			EphemerisData EPHEM;
			if (GC->rtcc->EMSFFV(gmt, GC->rtcc->med_k16.Vehicle, EPHEM))
			{
				Result = DONE;
				break;
			}

			sv.R = EPHEM.R;
			sv.V = EPHEM.V;
			sv.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv.gravref = GC->rtcc->GetGravref(EPHEM.RBI);
		}
		else
		{
			sv = GC->rtcc->StateVectorCalc(vessel);
		}

		if (!GC->rtcc->LunarDescentPlanningProcessor(sv))
		{
			if (GC->rtcc->med_k16.Mode != 7)
			{
				GC->rtcc->CZTDTGTU.GETTD = GC->rtcc->PZLDPDIS.PD_GETTD;
			}
		}

		Result = DONE;
	}
	break;
	case 11: //Space Digitals without MPT
	{
		SV sv0 = GC->rtcc->StateVectorCalc(vessel);
		GC->rtcc->EMDSPACENoMPT(sv0, SpaceDigitalsOption + 2, GC->rtcc->GMTfromGET(SpaceDigitalsGET));

		Result = DONE;
	}
	break;
	case 12: //TLI Processor
	{
		SV2 state;

		state = GC->rtcc->StateVectorCalc2(vessel);

		GC->rtcc->TranslunarInjectionProcessor(state);

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
			sv_CSM = GC->rtcc->StateVectorCalcEphem(target);
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
			sv0 = GC->rtcc->StateVectorCalcEphem(vessel);

			CSMmass = vessel->GetMass();
			//Assume pre CSM separation from the S-IVB
			if (CSMmass > 40000.0)
			{
				CSMmass = 28860.0;
			}
			if (GC->rtcc->PZMCCPLN.Config)
			{
				LMmass = GC->rtcc->GetDockedVesselMass(vessel);
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
			sv_CSM = GC->rtcc->StateVectorCalc(target);
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
			opt.sv0 = GC->rtcc->StateVectorCalc(vessel);
		}

		opt.direct = true;
		opt.HeadsUp = HeadsUp;
		opt.REFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
		opt.R_LS = OrbMech::r_from_latlong(GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST], GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST], GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST]);
		opt.t_land = GC->rtcc->CZTDTGTU.GETTD;

		PADSolGood = GC->rtcc->PDI_PAD(&opt, temppdipad);

		if (PADSolGood)
		{
			pdipad = temppdipad;
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
			sv = GC->rtcc->StateVectorCalcEphem(vessel);
			CSMmass = vessel->GetMass();
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

			EntryLatcor = GC->rtcc->RZRFTT.Manual.entry.lat_T;
			EntryLngcor = GC->rtcc->RZRFTT.Manual.entry.lng_T;
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
			if (target == NULL)
			{
				Result = DONE;
				break;
			}

			EphemerisData sv[2];
			sv[0] = GC->rtcc->StateVectorCalcEphem(vessel);
			sv[1] = GC->rtcc->StateVectorCalcEphem(target);

			if (utils::IsVessel(vessel, utils::Saturn))
			{
				opt.sv_CSM = sv[0];
				opt.sv_LM = sv[1];
			}
			else
			{
				opt.sv_CSM = sv[1];
				opt.sv_LM = sv[0];
			}

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
		SV sv_CSM, sv_Ins, sv_IG;
		VECTOR3 R_LS;
		double theta, dt, m0, dv;

		if (GC->MissionPlanningActive)
		{
			double GMT = GC->rtcc->GMTfromGET(t_LunarLiftoff);
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

			PLAWDTInput pin;
			PLAWDTOutput pout;
			pin.T_UP = GMT;
			pin.TableCode = RTCC_MPT_LM;
			GC->rtcc->PLAWDT(pin, pout);

			m0 = pout.LMAscWeight;
		}
		else
		{
			if (target == NULL)
			{
				Result = DONE;
				break;
			}
			sv_CSM = GC->rtcc->StateVectorCalc(target);
			LEM *l = (LEM *)vessel;
			m0 = l->GetAscentStageMass();
		}

		R_LS = OrbMech::r_from_latlong(GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST], GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST], GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST]);

		GC->rtcc->LunarAscentProcessor(R_LS, m0, sv_CSM, t_LunarLiftoff, GC->rtcc->PZLTRT.InsertionHorizontalVelocity, GC->rtcc->PZLTRT.InsertionRadialVelocity, theta, dt, dv, sv_IG, sv_Ins);

		GC->rtcc->PZLTRT.PoweredFlightArc = theta;
		GC->rtcc->PZLTRT.PoweredFlightTime = dt;

		GC->rtcc->JZLAI.t_launch = t_LunarLiftoff;
		GC->rtcc->JZLAI.R_D = 60000.0*0.3048;
		GC->rtcc->JZLAI.Y_D = 0.0;
		GC->rtcc->JZLAI.R_D_dot = GC->rtcc->PZLTRT.InsertionRadialVelocity;
		GC->rtcc->JZLAI.Y_D_dot = 0.0;
		GC->rtcc->JZLAI.Z_D_dot = GC->rtcc->PZLTRT.InsertionHorizontalVelocity;

		GC->rtcc->JZLAI.sv_Insertion.R = sv_Ins.R;
		GC->rtcc->JZLAI.sv_Insertion.V = sv_Ins.V;
		GC->rtcc->JZLAI.sv_Insertion.GMT = OrbMech::GETfromMJD(sv_Ins.MJD, GC->rtcc->GetGMTBase());
		GC->rtcc->JZLAI.sv_Insertion.RBI = BODY_MOON;

		Result = DONE;
	}
	break;
	case 21: //LM Ascent PAD
	{
		if (target == NULL)
		{
			Result = DONE;
			break;
		}

		ASCPADOpt opt;
		EphemerisData sv_CSM;

		sv_CSM = GC->rtcc->StateVectorCalcEphem(target);

		opt.Rot_VL = OrbMech::GetVesselToLocalRotMatrix(vessel);
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

		LEM *l = (LEM *)vessel;


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
			if (vesseltype != 1 || target == NULL)
			{
				Result = DONE;
				break;
			}

			opt.W_TAPS = l->GetAscentStageMass();
			opt.W_TDRY = opt.sv_A.mass - vessel->GetPropellantMass(vessel->GetPropellantHandleByIndex(0));

			sv_LM = GC->rtcc->StateVectorCalc(vessel);
			sv_CSM = GC->rtcc->StateVectorCalc(target);
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
		if (target == NULL)
		{
			Result = DONE;
			break;
		}
		SV sv0 = GC->rtcc->StateVectorCalc(target);
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
		if (svtarget == NULL)
		{
			Result = DONE;
			break;
		}

		EphemerisData sv = GC->rtcc->StateVectorCalcEphem(svtarget);
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
		if (svtarget == NULL)
		{
			iuUplinkResult = 2;
			Result = DONE;
			break;
		}

		IU *iu = NULL;

		if (utils::IsVessel(svtarget, utils::Saturn))
		{
			Saturn *iuv = (Saturn *)svtarget;
			iu = iuv->GetIU();
		}
		else if (utils::IsVessel(svtarget, utils::SIVB))
		{
			SIVB *iuv = (SIVB *)svtarget;
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
		OBJHANDLE hEarth;
		double mu;

		hEarth = oapiGetObjectByName("Earth");
		mu = GGRAV * oapiGetMass(hEarth);

		if (entrypadopt == 0)
		{
			EarthEntryPADOpt opt;

			opt.dV_LVLH = dV_LVLH;
			opt.P30TIG = P30TIG;
			opt.REFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
			opt.sv0 = GC->rtcc->StateVectorCalc(vessel);
			opt.Thruster = manpadenginetype;
			opt.InitialBank = GC->rtcc->RZC1RCNS.entry.GNInitialBank;
			opt.GLevel = GC->rtcc->RZC1RCNS.entry.GLevel;

			if (EntryLatcor == 0)
			{
				opt.lat = 0;
				opt.lng = 0;
			}
			else
			{
				opt.lat = EntryLatcor;
				opt.lng = EntryLngcor;
			}

			VECTOR3 R, V;
			double apo, peri;
			OBJHANDLE gravref = GC->rtcc->AGCGravityRef(vessel);
			vessel->GetRelativePos(gravref, R);
			vessel->GetRelativeVel(gravref, V);
			OrbMech::periapo(R, V, mu, apo, peri);
			if (peri < oapiGetSize(gravref) + 50 * 1852.0)
			{
				opt.preburn = false;
				GC->rtcc->EarthOrbitEntry(opt, earthentrypad);
			}
			else
			{
				opt.preburn = true;
				GC->rtcc->EarthOrbitEntry(opt, earthentrypad);
			}
		}
		else
		{
			LunarEntryPADOpt opt;

			if (EntryLatcor == 0)
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
						opt.sv0 = GC->rtcc->StateVectorCalc(vessel);
					}
				}
				else
				{
					opt.sv0 = GC->rtcc->StateVectorCalc(vessel);
				}

				//EntryPADLat = EntryLatcor;
				//EntryPADLng = EntryLngcor;
				opt.lat = EntryLatcor;
				opt.lng = EntryLngcor;
				opt.REFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
				opt.SxtStarCheckAttitudeOpt = EntryPADSxtStarCheckAttOpt;

				GC->rtcc->LunarEntryPAD(&opt, lunarentrypad);
			}
		}

		Result = DONE;
	}
	break;
	case 32: //Map Update
	{
		EphemerisData sv0;

		if (GC->MissionPlanningActive && GC->rtcc->MPTHasManeuvers(RTCC_MPT_CSM))
		{
			double gmt;

			if (mapUpdateGET <= 0.0)
			{
				gmt = GC->rtcc->RTCCPresentTimeGMT();
			}
			else
			{
				gmt = GC->rtcc->GMTfromGET(mapUpdateGET);
			}

			if (GC->rtcc->EMSFFV(gmt, RTCC_MPT_CSM, sv0))
			{
				Result = DONE;
				break;
			}
		}
		else
		{
			sv0 = GC->rtcc->StateVectorCalcEphem(vessel);
			if (mapUpdateGET > 0)
			{
				sv0 = GC->rtcc->coast(sv0, GC->rtcc->GMTfromGET(mapUpdateGET) - sv0.GMT);
			}
		}

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
			GC->rtcc->LunarOrbitMapUpdate(sv0, mapupdate);
		}

		Result = DONE;
	}
	break;
	case 33: //Landmark Tracking PAD
	{
		LMARKTRKPADOpt opt;
		SV sv0;

		if (GC->MissionPlanningActive && GC->rtcc->MPTHasManeuvers(RTCC_MPT_CSM))
		{
			if (LmkTime <= 0.0)
			{
				GC->rtcc->NewMPTTrajectory(RTCC_MPT_CSM, sv0);
			}
			else
			{
				EphemerisData sv;
				if (GC->rtcc->EMSFFV(GC->rtcc->GMTfromGET(LmkTime), RTCC_MPT_CSM, sv))
				{
					Result = DONE;
					break;
				}
				sv0.R = sv.R;
				sv0.V = sv.V;
				sv0.MJD = OrbMech::MJDfromGET(sv.GMT, GC->rtcc->GetGMTBase());
				sv0.gravref = GC->rtcc->GetGravref(sv.RBI);
			}
		}
		else
		{
			sv0 = GC->rtcc->StateVectorCalc(vessel);
		}

		opt.lat[0] = LmkLat;
		opt.LmkTime[0] = LmkTime;
		opt.lng[0] = LmkLng;
		opt.sv0 = sv0;
		opt.entries = 1;

		GC->rtcc->LandmarkTrackingPAD(&opt, landmarkpad);

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
		if (vesseltype != 1)
		{
			Result = DONE;
			break;
		}

		LEM *l = (LEM*)vessel;

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
			EphemerisData sv = GC->rtcc->StateVectorCalcEphem(vessel);

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
			if (GetVesselParameters(GC->rtcc->med_m72.Thruster, in.CONFIG, in.VC, in.CSMWeight, in.LMWeight))
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

			SV sv_now = GC->rtcc->StateVectorCalc(vessel);
			sv_tig = GC->rtcc->coast(sv_now, SPQTIG - OrbMech::GETfromMJD(sv_now.MJD, GC->rtcc->CalcGETBase()));

			if (vesselisdocked)
			{
				attachedMass = GC->rtcc->GetDockedVesselMass(vessel);
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
			if (GetVesselParameters(GC->rtcc->med_m70.Thruster, in.CONFIG, in.VC, in.CSMWeight, in.LMWeight))
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

			SV sv_now = GC->rtcc->StateVectorCalc(vessel);
			sv_tig = GC->rtcc->coast(sv_now, GC->rtcc->PZLDPDIS.GETIG[0] - OrbMech::GETfromMJD(sv_now.MJD, GC->rtcc->CalcGETBase()));

			if (vesselisdocked)
			{
				attachedMass = GC->rtcc->GetDockedVesselMass(vessel);
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
			if (GetVesselParameters(GC->rtcc->med_m65.Thruster, in.CONFIG, in.VC, in.CSMWeight, in.LMWeight))
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
				manpad_ullage_dt = GC->rtcc->med_m65.UllageDT;
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
				GC->rtcc->PZREAP.RTETimeOfLanding = GC->rtcc->GMTfromGET(GC->rtcc->med_f75_f77.T_Z) / 3600.0;
				GC->rtcc->PZREAP.RTEPTPMissDistance = GC->rtcc->med_f77.MissDistance;
			}
			EphemerisData sv = GC->rtcc->StateVectorCalcEphem(vessel);
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

			EphemerisData sv_now, sv_tig;
			double mass, dt, attachedMass;
			int ITS;

			sv_now = GC->rtcc->StateVectorCalcEphem(vessel);
			mass = vessel->GetMass();

			//Propagate to TIG
			dt = gmt_tig - sv_now.GMT;
			GC->rtcc->PMMCEN(sv_now, 0.0, 0.0, 1, abs(dt), dt >= 0.0 ? 1.0 : -1.0, sv_tig, ITS);

			if (vesselisdocked)
			{
				attachedMass = GC->rtcc->GetDockedVesselMass(vessel);
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
		if (target == NULL)
		{
			Result = DONE;
			break;
		}

		IU *iu = NULL;
		LVDCSV * lvdc = NULL;

		bool uplinkaccepted = false;

		if (utils::IsVessel(target, utils::SaturnV))
		{
			Saturn *iuv = (Saturn *)target;

			iu = iuv->GetIU();
		}
		else if (utils::IsVessel(target, utils::SaturnV_SIVB))
		{
			SIVB *iuv = (SIVB *)target;

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
		
		in.sv_in = GC->rtcc->StateVectorCalcEphem(target);
		in.mass = target->GetMass();
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
		EphemerisData sv;

		in.Option = AGOP_Option;
		in.Mode = AGOP_Mode;

		//Get ephemeris
		if (AGOP_Option != 3)
		{
			double GMT, GMT_Stop;

			GMT = GC->rtcc->GMTfromGET(AGOP_StartTime);
			GMT_Stop = GC->rtcc->GMTfromGET(AGOP_StopTime);
			sv = GC->rtcc->StateVectorCalcEphem(vessel);

			in.sv_arr.clear();

			do
			{
				in.sv_arr.push_back(GC->rtcc->coast(sv, GMT - sv.GMT));

				GMT += AGOP_TimeStep * 60.0;
				if (in.sv_arr.size() >= 10) break;

				sv = in.sv_arr.back();
			} while (GMT_Stop > GMT);
		}

		//Logic to get required REFSMMATs
		bool GetCSMREFSMMAT = false, GetLMREFSMMAT = false;

		if (AGOP_Option == 1 || AGOP_Option == 5 || AGOP_Option == 6) GetCSMREFSMMAT = true;
		else if (AGOP_Option == 4)
		{
			if (AGOP_Mode == 1 || AGOP_Mode == 4) GetCSMREFSMMAT = true;
			else if (AGOP_AttIsCSM) GetCSMREFSMMAT = true;
		}

		if (AGOP_Option == 4)
		{
			if (AGOP_Mode != 1 && AGOP_Mode != 4) GetLMREFSMMAT = true;
			else if (!AGOP_AttIsCSM) GetLMREFSMMAT = true;
		}

		if (GetCSMREFSMMAT)
		{
			REFSMMATData refs = GC->rtcc->EZJGMTX1.data[AGOP_CSM_REFSMMAT - 1];

			if (refs.ID == 0)
			{
				AGOP_Output.clear();
				AGOP_Error = "REFSMMAT NOT AVAILABLE";
				Result = DONE;
				break;
			}
			in.CSM_REFSMMAT = refs.REFSMMAT;
		}

		if (GetLMREFSMMAT)
		{
			REFSMMATData refs = GC->rtcc->EZJGMTX3.data[AGOP_LM_REFSMMAT - 1];

			if (refs.ID == 0)
			{
				AGOP_Output.clear();
				AGOP_Error = "REFSMMAT NOT AVAILABLE";
				Result = DONE;
				break;
			}
			in.LM_REFSMMAT = refs.REFSMMAT;
		}

		in.startable = GC->rtcc->EZJGSTAR;
		in.NumStars = 1;
		in.StarIDs[0] = AGOP_Star;

		in.AttIsCSM = AGOP_AttIsCSM;
		in.IMUAttitude[0] = AGOP_Attitude;
		in.HeadsUp = AGOP_HeadsUp;
		in.AntennaPitch = AGOP_AntennaPitch;
		in.AntennaYaw = AGOP_AntennaYaw;

		//For now, always input landmark
		in.GroundStationID = "";
		in.lmk_lat = AGOP_Lat;
		in.lmk_lng = AGOP_Lng;
		in.lmk_alt = AGOP_Alt;

		AGOP agop(GC->rtcc);

		agop.Calc(in, out);

		AGOP_Output = out.output_text;
		AGOP_Error = out.errormessage;

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
		if (target == NULL || GC->rtcc->GetGMTBase() == 0.0)
		{
			Result = DONE;
			break;
		}
		
		EphemerisData sv, sv_ECT;

		sv = GC->rtcc->StateVectorCalcEphem(target);

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

		if (utils::IsVessel(vessel, utils::SaturnIB))
		{
			Saturn *iuv = (Saturn *)vessel;
			iu = iuv->GetIU();
		}
		else if (utils::IsVessel(vessel, utils::SaturnIB_SIVB))
		{
			SIVB *iuv = (SIVB *)vessel;
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
			sv0 = GC->rtcc->StateVectorCalcEphem(vessel);
			mass = vessel->GetMass();
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

		if (utils::IsVessel(vessel, utils::SaturnV))
		{
			Saturn *iuv = (Saturn *)vessel;
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

void ARCore::AGCCorrectionVectors(double mjd_launchday, double dt_UNITW, double dt_504LM, int mission, bool isCMC)
{
	//INPUTS:
	//mjd_launchday: MJD of noon (GMT) preceeding the launch
	//dt_UNITW: //Time in days from mjd_launchday to calculate the Earth correction vector UNITW
	//dt_504LM: //Time in days from mjd_launchday to calculate the Moon correction vector 504LM

	MATRIX3 R, Rot2, J2000, R2, R3, M, M_AGC;
	VECTOR3 UNITW;
	double mjd_UNITW, mjd_504LM, brcsmjd, w_E, t0, B_0, Omega_I0, F_0, B_dot, Omega_I_dot, F_dot, cosI, sinI;
	double A_Z, A_Z0;
	int mem, epoch;
	char AGC[64];

	mjd_UNITW = mjd_launchday + dt_UNITW;
	mjd_504LM = mjd_launchday + dt_504LM;

	Rot2 = _M(1., 0., 0., 0., 0., 1., 0., 1., 0.);
	R = OrbMech::GetRotationMatrix(BODY_EARTH, mjd_UNITW);

	if (isCMC)
	{
		if (mission < 11)
		{
			epoch = 1969;    //Nearest Besselian Year 1969
			w_E = 7.29211515e-5;
			B_0 = 0.409164173;              
			Omega_I0 = -6.03249419;
			F_0 = 2.61379488;
			B_dot = -7.19756666e-14;
			Omega_I_dot = -1.07047016e-8;
			F_dot = 2.67240019e-6;
			cosI = 0.99964115;
			sinI = 0.02678760;
			t0 = 40038;
			if (mission < 9)
			{
				sprintf(AGC, "Colossus237");
			}
			else if (mission < 10)
			{
				sprintf(AGC, "Colossus249");
			}
			else
			{
				sprintf(AGC, "Comanche044");
			}
		}
		else if (mission < 14)
		{
			epoch = 1970;				//Nearest Besselian Year 1970
			w_E = 7.29211319606104e-5;	//Comanche 055 (Apollo 11 CM AGC)
			B_0 = 0.40916190299;
			Omega_I0 = 6.1965366255107;
			F_0 = 5.20932947411685;
			B_dot = -7.19757301e-14;
			Omega_I_dot = -1.07047011e-8;
			F_dot = 2.67240410e-6;
			cosI = 0.99964173;
			sinI = 0.02676579;
			t0 = 40403;
			sprintf(AGC, "Comanche055");
		}
		//Use this when we get Comanche 67:
		/*
		else if (mission < 14)
		{
			epoch = 1970;			//Nearest Besselian Year 1970
			w_E = 7.292115145489943e-05;
			B_0 = 0.4091619030;
			Omega_I0 = 6.196536640;
			F_0 = 5.209327056;
			B_dot = -7.197573418e-14;
			Omega_I_dot = -1.070470170e-8;
			F_dot = 2.672404256e-6;
			cosI = 0.9996417320;
			sinI = 0.02676579050;
			t0 = 40403;
			if (mission < 13)
			{
				sprintf(AGC, "Comanche067");
			}
			else
			{
				sprintf(AGC, "Comanche072");
			}
		}
		*/
		else if (mission < 15)
		{
			epoch = 1971;			//Nearest Besselian Year 1971
			w_E = 7.292115147e-5;	//Comanche 108 (Apollo 14 CM AGC)
			B_0 = 0.40915963316;
			Omega_I0 = 5.859196887;
			F_0 = 1.5216749598;
			B_dot = -7.1975797907e-14;
			Omega_I_dot = -1.070470151e-8;
			F_dot = 2.6724042552e-6;
			cosI = 0.999641732;
			sinI = 0.0267657905;
			t0 = 40768;
			sprintf(AGC, "Comanche108");
		}
		else
		{
			epoch = 1972;			//Nearest Besselian Year 1972
			w_E = 7.29211514667e-5; //Artemis 072 (Apollo 15 CM AGC)
			B_0 = 0.409157363336;
			Omega_I0 = 5.52185714700;
			F_0 = 4.11720655556;
			B_dot = -7.19758599677e-14;
			Omega_I_dot = -1.07047013100e-8;
			F_dot = 2.67240425480e-6;
			cosI = 0.999641732;
			sinI = 0.0267657905;
			t0 = 41133;
			sprintf(AGC, "Artemis072");
		}
	}
	else
	{
		if (mission < 11)
		{
			epoch = 1969;		    //Nearest Besselian Year 1969
			w_E = 7.29211515e-5;    //Luminary 069 (Apollo 10 LM AGC)
			B_0 = 0.409164173;
			Omega_I0 = -6.03249419;
			F_0 = 2.61379488;
			B_dot = -7.19756666e-14;
			Omega_I_dot = -1.07047016e-8;
			F_dot = 2.67240019e-6;
			cosI = 0.99964115;
			sinI = 0.02678760;
			t0 = 40038;
			sprintf(AGC, "Luminary069");
		}
		else if (mission < 12)
		{
			epoch = 1970;				//Nearest Besselian Year 1970
			w_E = 7.29211319606104e-5;  //Luminary 099 (Apollo 11 LM AGC)
			B_0 = 0.40916190299;
			Omega_I0 = 6.1965366255107;
			F_0 = 5.20932947411685;
			B_dot = -7.19757301e-14;
			Omega_I_dot = -1.07047011e-8;
			F_dot = 2.67240410e-6;
			cosI = 0.99964173;
			sinI = 0.02676579;
			t0 = 40403;
			sprintf(AGC, "Luminary099");
		}
		else if (mission < 14)
		{
			epoch = 1970;			//Nearest Besselian Year 1970
			w_E = 7.292115145489943e-05;
			B_0 = 0.4091619030;
			Omega_I0 = 6.196536640;
			F_0 = 5.209327056;
			B_dot = -7.197573418e-14;
			Omega_I_dot = -1.070470170e-8;
			F_dot = 2.672404256e-6;
			cosI = 0.9996417320;
			sinI = 0.02676579050;
			t0 = 40403;
			if (mission < 13)
			{
				sprintf(AGC, "Luminary116");
			}
			else
			{
				sprintf(AGC, "Luminary131");
			}
		}
		else if (mission < 15)
		{
			epoch = 1971;			//Nearest Besselian Year 1971
			w_E = 7.292115147e-5;	//Luminary 178 (Apollo 14 LM AGC)
			B_0 = 0.40915963316;
			Omega_I0 = 5.859196887;
			F_0 = 1.5216749598;
			B_dot = -7.1975797907e-14;
			Omega_I_dot = -1.070470151e-8;
			F_dot = 2.6724042552e-6;
			cosI = 0.999641732;
			sinI = 0.0267657905;
			t0 = 40768;
			sprintf(AGC, "Luminary178");
		}
		else
		{
			epoch = 1972;			//Nearest Besselian Year 1972
			w_E = 7.29211514667e-5; //Luminary 210 (Apollo 15 LM AGC)
			B_0 = 0.409157363336;
			Omega_I0 = 5.52185714700;
			F_0 = 4.11720655556;
			B_dot = -7.19758599677e-14;
			Omega_I_dot = -1.07047013100e-8;
			F_dot = 2.67240425480e-6;
			cosI = 0.999641732;
			sinI = 0.0267657905;
			t0 = 41133;
			sprintf(AGC, "Luminary210");
		}
	}

	//EARTH ROTATIONS
	brcsmjd = OrbMech::MJDOfNBYEpoch(epoch);
	J2000 = OrbMech::J2000EclToBRCSMJD(brcsmjd);
	R2 = mul(OrbMech::tmat(Rot2), mul(R, Rot2));
	R3 = mul(J2000, R2);

	UNITW = mul(R3, _V(0, 0, 1));

	A_Z = atan2(R3.m21, R3.m11);
	if (mission < 14)
	{
		A_Z0 = fmod((A_Z - w_E * (mjd_UNITW - t0) * 24.0 * 3600.0), PI2);  //AZ0 for mission
		if (A_Z0 < 0) A_Z0 += PI2;
	}
	else if (mission < 15)
	{
		A_Z0 = 4.8631512705;   //Hardcoded in Comanche 108 and Luminary 178
	}
	else
	{
		A_Z0 = 4.85898502016;  //Hardcoded in Artemis 072 and Luminary 210
	}

	M = _M(cos(A_Z), sin(A_Z), 0., -sin(A_Z), cos(A_Z), 0., 0., 0., 1.);
	M_AGC = mul(R3, M);

	mem = 01711;

	//TBD: Print stuff here
	FILE *file = fopen("PrecessionData.txt", "w");
	fprintf(file, "------- AGC Correction Vectors for Apollo %d using %s -------\n", mission, AGC);
	fprintf(file, "Epoch   = %d (Year) Epoch of Basic Reference Coordinate System\n", epoch);
	fprintf(file, "Epoch   = %6.6f (MJD) Epoch of Basic Reference Coordinate System\n", brcsmjd);
	fprintf(file, "TEphem0 = %6.6f (MJD) Ephemeris Time Zero\n", t0);
	fprintf(file, "T0      = %6.6f (MJD) Mission start time\n", mjd_launchday);
	fprintf(file, "UNITW computed to %+.2lf Days\n", dt_UNITW);
	fprintf(file, "504LM computed to %+.2lf Days\n\n", dt_504LM);
	fprintf(file, "------- Earth Orientation -------\n");
	if (mission < 14)
	{
		fprintf(file, "AZO %.10f DEG\n", A_Z0*DEG);
	}
	fprintf(file, "-AYO %.10f DEG\n", UNITW.x*DEG);
	fprintf(file, "AXO %.10f DEG\n", UNITW.y*DEG);
	if (mission < 14)
	{
		fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(A_Z0 / PI2, 0, 1)); mem++;
		fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(A_Z0 / PI2, 0, 0)); mem++;
	}	
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(UNITW.x, 0, 1)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(UNITW.x, 0, 0)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(UNITW.y, 0, 1)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(UNITW.y, 0, 0)); mem++;
	if (isCMC)
	{
		fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(UNITW.z, 0, 1)); mem++;
		fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(UNITW.z, 0, 0)); mem++;
	}

	//MOON ROTATIONS
	MATRIX3 M1, M2, M3, M4, MM, M_AGC_M, RM, R2M, R3M;
	VECTOR3 lm;
	double t_M, B, Omega_I, F;

	t_M = (mjd_504LM - t0) * 24.0 * 3600.0;
	RM = OrbMech::GetRotationMatrix(BODY_MOON, mjd_504LM);

	B = B_0 + B_dot * t_M;
	Omega_I = Omega_I0 + Omega_I_dot * t_M;
	F = F_0 + F_dot * t_M;
	M1 = _M(1., 0., 0., 0., cos(B), sin(B), 0., -sin(B), cos(B));
	M2 = _M(cos(Omega_I), sin(Omega_I), 0., -sin(Omega_I), cos(Omega_I), 0., 0., 0., 1.);
	M3 = _M(1., 0., 0., 0., cosI, -sinI, 0., sinI, cosI);
	M4 = _M(-cos(F), -sin(F), 0., sin(F), -cos(F), 0., 0., 0., 1.);
	MM = mul(M4, mul(M3, mul(M2, M1)));
	R2M = mul(OrbMech::tmat(Rot2), mul(RM, Rot2));
	R3M = mul(J2000, R2M);
	M_AGC_M = mul(MM, R3M);

	lm.x = atan2(M_AGC_M.m32, M_AGC_M.m33);
	lm.y = atan2(-M_AGC_M.m31, sqrt(M_AGC_M.m32*M_AGC_M.m32 + M_AGC_M.m33*M_AGC_M.m33));
	lm.z = atan2(M_AGC_M.m21, M_AGC_M.m11);

	if (isCMC)
	{
		mem = 02011;
	}
	else
	{
		mem = 02012;
	}
	fprintf(file, "------- Moon Orientation -------\n");
	fprintf(file, "504LM %.10f DEG\n", lm.x*DEG);
	fprintf(file, "504LM+2 %.10f DEG\n", lm.y*DEG);
	fprintf(file, "504LM+4 %.10f DEG\n", lm.z*DEG);
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(lm.x, 0, 1)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(lm.x, 0, 0)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(lm.y, 0, 1)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(lm.y, 0, 0)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(lm.z, 0, 1)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(lm.z, 0, 0)); mem++;

	if (file) fclose(file);
}


int ARCore::GetVesselParameters(int Thruster, int &Config, int &TVC, double &CSMMass, double &LMMass)
{
	//Error checking
	if (Thruster == RTCC_ENGINETYPE_CSMSPS || Thruster == RTCC_ENGINETYPE_CSMRCSMINUS2 || Thruster == RTCC_ENGINETYPE_CSMRCSPLUS2 || Thruster == RTCC_ENGINETYPE_CSMRCSMINUS4 || Thruster == RTCC_ENGINETYPE_CSMRCSPLUS4)
	{
		//CSM thruster
		if (vesseltype != 0) return 1;

		TVC = 1;
	}
	else
	{
		//LM thruster
		if (vesseltype != 1) return 1;

		TVC = 3;
	}

	MED_M50 m50;
	MED_M55 m55;
	MED_M49 m49;

	GC->rtcc->MPTMassUpdate(vessel, m50, m55, m49, vesselisdocked);

	std::bitset<4> cfg;
	GC->rtcc->MPTGetConfigFromString(m55.ConfigCode, cfg);

	Config = cfg.to_ulong();
	CSMMass = m50.CSMWT;
	LMMass = m50.LMWT;

	return 0;
}

void ARCore::menuCalculateIMUComparison()
{
	MATRIX3 M_BRCS_SM; //BRCS to stable member, right handed
	MATRIX3 M_NB_ECL; //Local vessel to global ecliptic
	MATRIX3 M_SM_NB_est; //Stable member to navigation base, estimated
	MATRIX3 M_SM_NB_act; //Stable member to navigation base, actual
	MATRIX3 M_ECL_BRCS; //Ecliptic to BRCS
	VECTOR3 IMUAngles;

	if (vesseltype == 0)
	{
		M_BRCS_SM = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
		IMUAngles = ((Saturn*)vessel)->imu.GetTotalAttitude();

		//Get actual orientation (left handed)
		vessel->GetRotationMatrix(M_NB_ECL);
		//Convert to right-handed CSM coordinates
		M_NB_ECL = mul(MatrixRH_LH(M_NB_ECL), _M(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0));
	}
	else if (vesseltype == 1)
	{
		M_BRCS_SM = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
		IMUAngles = ((LEM*)vessel)->imu.GetTotalAttitude();

		//Get actual orientation (left handed)
		vessel->GetRotationMatrix(M_NB_ECL);
		//Convert to right-handed LM coordinates
		M_NB_ECL = mul(MatrixRH_LH(M_NB_ECL), _M(0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0));
	}
	else return;

	//Stable member to navigation base conversion with current IMU alignment
	M_SM_NB_est = OrbMech::CALCSMSC(IMUAngles);
	//Get ecliptic to BRCS rotation matrix from RTCC system parameters
	M_ECL_BRCS = GC->rtcc->SystemParameters.MAT_J2000_BRCS;
	//Actual stable member to navigation base conversion
	M_SM_NB_act = OrbMech::tmat(mul(M_BRCS_SM, mul(M_ECL_BRCS, M_NB_ECL)));
	//Torquing angles that would be required
	DebugIMUTorquingAngles = OrbMech::CALCGTA(mul(OrbMech::tmat(M_SM_NB_act), M_SM_NB_est));
}