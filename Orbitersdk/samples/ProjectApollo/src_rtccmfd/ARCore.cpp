#include "ARCore.h"

#include "soundlib.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "saturn.h"
#include "saturnv.h"
#include "iu.h"
#include "LVDC.h"
#include "LEM.h"
#include "mcc.h"
#include "rtcc.h"

static WSADATA wsaData;
static SOCKET m_socket;
static sockaddr_in clientService;
static SOCKET close_Socket = INVALID_SOCKET;
static char debugString[100];
static char debugStringBuffer[100];
static char debugWinsock[100];

static DWORD WINAPI RTCCMFD_Trampoline(LPVOID ptr) {
	ARCore *core = (ARCore *)ptr;
	return(core->subThread());
}

AR_GCore::AR_GCore(VESSEL* v)
{
	MissionPlanningActive = false;

	mission = 0;
	GETbase = LaunchMJD[0];
	LSLat = 0.0;
	LSLng = 0.0;
	LSAlt = 0.0;
	t_Land = 0.0;
	LOIazi = 0.0;
	TLCCFreeReturnEMPLat = 0.0;
	TLCCNonFreeReturnEMPLat = 0.0;
	TLCCPeriGET = 0.0;
	LOIapo = 170.0*1852.0;
	LOIperi = 60.0*1852.0;
	LOIEllipseRotation = 0;
	TLCCLAHPeriAlt = 60.0*1852.0;
	TLCCFlybyPeriAlt = 60.0*1852.0;
	TLCCNodeLat = 0.0;
	TLCCNodeLng = 0.0;
	TLCCNodeAlt = 0.0;
	TLCCNodeGET = 0.0;
	DOI_N = 0;
	DOI_option = 0;
	DOI_PeriAng = 15.0*RAD;
	DOI_alt = 50000.0*0.3048;
	RTEMaxReturnInclination = 40.0*RAD;
	RTERangeOverrideNM = 0.0;

	if (strcmp(v->GetName(), "AS-205") == 0)
	{
		mission = 7;
	}
	else if (strcmp(v->GetName(), "AS-503") == 0)
	{
		mission = 8;
	}
	else if (strcmp(v->GetName(), "AS-504") == 0 || strcmp(v->GetName(), "Spider") == 0)
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

	if (mission >= 7)
	{
		GETbase = LaunchMJD[mission - 7];
	}

	if (mission == 8)
	{
		LSLat = 2.6317*RAD;
		LSLng = 34.0253*RAD;
		LSAlt = -0.82*1852.0;
		LOIazi = -78.0*RAD;
		TLCCFreeReturnEMPLat = TLCCNonFreeReturnEMPLat = -5.67822*RAD;
		TLCCPeriGET = OrbMech::HHMMSSToSS(69.0, 9.0, 29.4);
		t_Land = OrbMech::HHMMSSToSS(82.0, 8.0, 26.0);
		RTERangeOverrideNM = 1350.0;
	}
	else if (mission == 10)
	{
		LSLat = 0.732*RAD;
		LSLng = 23.647*RAD;
		LSAlt = -1.66*1852.0;
		LOIazi = -91.0*RAD;
		TLCCFreeReturnEMPLat = TLCCNonFreeReturnEMPLat = -4.933294*RAD;
		TLCCPeriGET = OrbMech::HHMMSSToSS(75.0, 49.0, 40.2);
		t_Land = OrbMech::HHMMSSToSS(100.0, 46.0, 19.0);
		RTERangeOverrideNM = 1285.0;
	}
	else if (mission == 11)
	{
		LSLat = 0.71388888*RAD;
		LSLng = 23.7077777*RAD;
		LSAlt = -3073.263;
		LOIazi = -91.0*RAD;
		LOIapo = 169.8*1852.0;
		LOIperi = 59.2*1852.0;
		TLCCFreeReturnEMPLat = TLCCNonFreeReturnEMPLat = 0.279074*RAD;
		TLCCPeriGET = OrbMech::HHMMSSToSS(75.0, 53.0, 35.0);
		t_Land = OrbMech::HHMMSSToSS(102.0, 47.0, 11.0);
		RTERangeOverrideNM = 1285.0;
	}
	else if (mission == 12)
	{
		LSLat = -2.9425*RAD;
		LSLng = -23.44333*RAD;
		LSAlt = -1.19*1852.0;
		LOIazi = -75.0*RAD;
		LOIapo = 168.9*1852.0;
		LOIperi = 58.7*1852.0;
		LOIEllipseRotation = 1;
		TLCCFreeReturnEMPLat = -1.962929*RAD;
		TLCCNonFreeReturnEMPLat = 3.35412*RAD;
		TLCCPeriGET = TLCCNodeGET = OrbMech::HHMMSSToSS(83.0, 28.0, 45.0);
		TLCCFlybyPeriAlt = 1851.7*1852.0;
		TLCCNodeLat = 0.49*RAD;
		TLCCNodeLng = 162.54*RAD;
		TLCCNodeAlt = 59.9*1852.0;
		TLCCLAHPeriAlt = TLCCNodeAlt;
		t_Land = OrbMech::HHMMSSToSS(110.0, 31.0, 19.0);
		RTERangeOverrideNM = 1250.0;
	}
	else if (mission == 13)
	{
		LSLat = -3.6686*RAD;
		LSLng = -17.4842*RAD;
		LSAlt = -0.76*1852.0;
		LOIazi = -93.88*RAD;
		LOIapo = 170.0*1852.0;
		LOIperi = 60.0*1852.0;
		LOIEllipseRotation = 1;
		TLCCFreeReturnEMPLat = 0.0*RAD;
		TLCCNonFreeReturnEMPLat = -0.20553*RAD;
		TLCCPeriGET = TLCCNodeGET = OrbMech::HHMMSSToSS(77.0, 28.0, 16.0);
		TLCCFlybyPeriAlt = 210 * 1852.0;
		TLCCNodeLat = 3.7*RAD;
		TLCCNodeLng = 176.6*RAD;
		TLCCNodeAlt = 60.0*1852.0;
		TLCCLAHPeriAlt = TLCCNodeAlt;
		t_Land = OrbMech::HHMMSSToSS(103.0, 42.0, 02.0);
		DOI_option = 1;
		DOI_N = 11;
		RTERangeOverrideNM = 1250.0;
	}
	else if (mission == 14)
	{
		LSLat = -3.672*RAD;
		LSLng = -17.463*RAD;
		LSAlt = -0.76*1852.0;
		LOIazi = -76.31*RAD;
		LOIapo = 170.0*1852.0;
		LOIperi = 59.6*1852.0;
		LOIEllipseRotation = 1;
		TLCCFreeReturnEMPLat = -0.048722*RAD;
		TLCCNonFreeReturnEMPLat = -4.66089*RAD;
		TLCCPeriGET = TLCCNodeGET = OrbMech::HHMMSSToSS(82.0, 40.0, 45.0);
		TLCCFlybyPeriAlt = 2030.9*1852.0;
		TLCCNodeLat = 2.16*RAD;
		TLCCNodeLng = 170.88*RAD;
		TLCCNodeAlt = 58.8*1852.0;
		TLCCLAHPeriAlt = TLCCNodeAlt;
		t_Land = OrbMech::HHMMSSToSS(108.0, 53.0, 32.6);
		DOI_option = 1;
		DOI_N = 11;
		RTERangeOverrideNM = 1250.0;
	}
	else if (mission == 15)
	{
		LSLat = 26.0739*RAD;
		LSLng = 3.6539*RAD;
		LSAlt = -1.92*1852.0;
		LOIazi = -91.0*RAD;
		LOIapo = 170.0*1852.0;
		LOIperi = 60.0*1852.0;
		LOIEllipseRotation = 1;
		TLCCFreeReturnEMPLat = TLCCNonFreeReturnEMPLat = -16.90093*RAD;
		TLCCPeriGET = TLCCNodeGET = OrbMech::HHMMSSToSS(78.0, 35.0, 0.0);
		TLCCFlybyPeriAlt = TLCCNodeAlt = 67.88*1852.0;
		TLCCNodeLat = -23.28*RAD;
		TLCCNodeLng = 171.57*RAD;
		TLCCLAHPeriAlt = TLCCNodeAlt;
		t_Land = OrbMech::HHMMSSToSS(104.0, 40.0, 57.0);
		DOI_PeriAng = 16.0*RAD;
		DOI_option = 1;
		DOI_N = 11;
		RTEMaxReturnInclination = 40.0*RAD;
		RTERangeOverrideNM = 1190.0;
	}
	else if (mission == 16)
	{
		LSLat = -9.00028*RAD;
		LSLng = 15.51639*RAD;
		LSAlt = -0.1405*1852.0;
		LOIazi = -90.0*RAD;
		LOIapo = 170.0*1852.0;
		LOIperi = 60.0*1852.0;
		LOIEllipseRotation = 1;
		TLCCFreeReturnEMPLat = TLCCNonFreeReturnEMPLat = 4.88464*RAD;
		TLCCPeriGET = TLCCNodeGET = OrbMech::HHMMSSToSS(74.0, 32.0, 24.0);
		TLCCFlybyPeriAlt = TLCCNodeAlt = 72.76*1852.0;
		TLCCNodeLat = 7.45*RAD;
		TLCCNodeLng = 173.90*RAD;
		TLCCLAHPeriAlt = TLCCNodeAlt;
		t_Land = OrbMech::HHMMSSToSS(98.0, 46.0, 42.4);
		DOI_PeriAng = 16.0*RAD;
		DOI_option = 1;
		DOI_N = 10;
		DOI_alt = 52500.0*0.3048;
		RTEMaxReturnInclination = 80.0*RAD;
		RTERangeOverrideNM = 1190.0;
	}
	else if (mission == 17)
	{
		LSLat = 20.164*RAD;
		LSLng = 30.750*RAD;
		LSAlt = -1.95*1852.0;
		LOIazi = -90.0*RAD;
		LOIapo = 170.0*1852.0;
		LOIperi = 52.8*1852.0;
		LOIEllipseRotation = 1;
		TLCCFreeReturnEMPLat = TLCCNonFreeReturnEMPLat = -11.11101*RAD;
		TLCCPeriGET = TLCCNodeGET = OrbMech::HHMMSSToSS(88.0, 58.0, 53.0);
		TLCCFlybyPeriAlt = TLCCNodeAlt = 49.35*1852.0;
		TLCCNodeLat = -9.52*RAD;
		TLCCNodeLng = 161.21*RAD;
		TLCCLAHPeriAlt = TLCCNodeAlt;
		t_Land = OrbMech::HHMMSSToSS(113.0, 01.0, 38.4);
		DOI_PeriAng = -10.0*RAD;
		DOI_option = 1;
		DOI_N = 10;
		DOI_alt = 84000.0*0.3048;
		RTEMaxReturnInclination = 80.0*RAD;
		RTERangeOverrideNM = 1190.0;
	}

	rtcc = new RTCC();
}

AR_GCore::~AR_GCore()
{
	delete rtcc;
}

ARCore::ARCore(VESSEL* v, AR_GCore* gcin)
{
	GC = gcin;

	T1 = 0;
	T2 = 0;
	SPQMode = 0;
	CSItime = 0.0;
	CDHtime = 0.0;
	SPQTIG = 0.0;
	CDHtimemode = 0;
	DH = 0;
	N = 0;
	this->vessel = v;
	t_TPI = 0.0;

	spqresults.DH = 0.0;
	spqresults.dV_CDH = _V(0.0, 0.0, 0.0);
	spqresults.dV_CSI = _V(0.0, 0.0, 0.0);
	spqresults.t_CDH = 0.0;
	spqresults.t_TPI = 0.0;

	lambertelev = 26.6*RAD;
	TwoImpulse_TIG = 0.0;
	LambertdeltaV = _V(0, 0, 0);
	lambertopt = 0;
	twoimpulsemode = 0;
	TwoImpulse_TPI = 0.0;
	TwoImpulse_PhaseAngle = 0.0;
	lambertElevOpt = 0;
	lambertTPFOpt = 0;
	lambertDT = 0.0;

	SPQDeltaV = _V(0, 0, 0);
	target = NULL;
	offvec = _V(0, 0, 0);
	//screen = 0;
	angdeg = 0;
	targetnumber = -1;
	AGCEpoch = 40221.525;
	AGCEphemTEphemZero = 40038.0;
	REFSMMAT = REFSMMAT_BRCS = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	REFSMMATTime = 0.0;
	REFSMMATopt = 4;
	REFSMMATcur = 4;
	REFSMMATupl = 0;
	manpadopt = 0;
	vesseltype = 0;
	lemdescentstage = true;

	for (int i = 0; i < 20; i++)
	{
		REFSMMAToct[i] = 0;
	}

	if (strcmp(v->GetName(), "AS-205") == 0)
	{
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.5217969*RAD, -80.5612465*RAD, LaunchMJD[GC->mission - 7], 72.0*RAD);
	}
	else if (strcmp(v->GetName(), "AS-503") == 0)
	{
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[GC->mission - 7], 72.124*RAD);
	}
	else if (strcmp(v->GetName(), "AS-504") == 0)
	{
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[GC->mission - 7], 72.0*RAD);
	}
	else if (strcmp(v->GetName(), "Spider") == 0)
	{
		vesseltype = 2;
	}
	else if (strcmp(v->GetName(), "AS-505") == 0 || strcmp(v->GetName(), "Charlie-Brown") == 0)
	{
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.626530*RAD, -80.620629*RAD, LaunchMJD[GC->mission - 7], 72.0*RAD);
	}
	else if (strcmp(v->GetName(), "Snoopy") == 0)
	{
		vesseltype = 2;
	}
	else if (strcmp(v->GetName(), "AS-506") == 0 || strcmp(v->GetName(), "Columbia") == 0)
	{
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[GC->mission - 7], 72.0*RAD);
		AGCEpoch = 40586.767239;
		AGCEphemTEphemZero = 40403.0;
	}
	else if (strcmp(v->GetName(), "Eagle") == 0)
	{
		vesseltype = 2;
		AGCEpoch = 40586.767239;
		AGCEphemTEphemZero = 40403.0;
	}
	else if (strcmp(v->GetName(), "Yankee-Clipper") == 0)
	{
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[GC->mission - 7], 72.0*RAD);
		AGCEpoch = 40586.767239;
		AGCEphemTEphemZero = 40403.0;
	}
	else if (strcmp(v->GetName(), "Intrepid") == 0)
	{
		vesseltype = 2;
		AGCEpoch = 40586.767239;
		AGCEphemTEphemZero = 40403.0;
	}
	else if (strcmp(v->GetName(), "Odyssey") == 0)
	{
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[GC->mission - 7], 72.0*RAD);
		AGCEpoch = 40586.767239;
		AGCEphemTEphemZero = 40403.0;
	}
	else if (strcmp(v->GetName(), "Aquarius") == 0)
	{
		vesseltype = 2;
		AGCEpoch = 40586.767239;
		AGCEphemTEphemZero = 40403.0;
	}
	else if (strcmp(v->GetName(), "Kitty-Hawk") == 0)
	{
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[GC->mission - 7], 72.067*RAD);
		AGCEpoch = 40952.009432;
		AGCEphemTEphemZero = 40768.0;
	}
	else if (strcmp(v->GetName(), "Antares") == 0)
	{
		vesseltype = 2;
		AGCEpoch = 40952.009432;
		AGCEphemTEphemZero = 40768.0;
	}
	else if (strcmp(v->GetName(), "Endeavour") == 0)
	{
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[GC->mission - 7], 80.088*RAD);
		AGCEpoch = 41317.251625;
		AGCEphemTEphemZero = 41133.0;
	}
	else if (strcmp(v->GetName(), "Falcon") == 0)
	{
		vesseltype = 2;
		AGCEpoch = 41317.251625;
		AGCEphemTEphemZero = 41133.0;
	}
	else if (strcmp(v->GetName(), "Casper") == 0)
	{
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[GC->mission - 7], 72.0*RAD);
		AGCEpoch = 41317.251625;
		AGCEphemTEphemZero = 41133.0;
	}
	else if (strcmp(v->GetName(), "Orion") == 0)
	{
		vesseltype = 2;
		AGCEpoch = 41317.251625;
		AGCEphemTEphemZero = 41133.0;
	}
	else if (strcmp(v->GetName(), "America") == 0)
	{
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[GC->mission - 7], 72.0*RAD);
		AGCEpoch = 41317.251625;
		AGCEphemTEphemZero = 41133.0;
	}
	else if (strcmp(v->GetName(), "Challenger") == 0)
	{
		vesseltype = 2;
		AGCEpoch = 41317.251625;
		AGCEphemTEphemZero = 41133.0;
	}

	if (vessel->DockingStatus(0) == 1)
	{
		vesseltype++;
	}

	REFSMMATHeadsUp = true;

	GMPManeuverCode = 0;
	GMPManeuverPoint = 0;
	GMPManeuverType = 0;
	OrbAdjAltRef = true;
	OrbAdjDVX = _V(0, 0, 0);
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
	GMPResults.A = 0.0;
	GMPResults.Del_G = 0.0;
	GMPResults.E = 0.0;
	GMPResults.GET_A = 0.0;
	GMPResults.GET_P = 0.0;
	GMPResults.HA = 0.0;
	GMPResults.HP = 0.0;
	GMPResults.H_Man = 0.0;
	GMPResults.I = 0.0;
	GMPResults.lat_A = 0.0;
	GMPResults.lat_Man = 0.0;
	GMPResults.lat_P = 0.0;
	GMPResults.long_A = 0.0;
	GMPResults.long_Man = 0.0;
	GMPResults.long_P = 0.0;
	GMPResults.Node_Ang = 0.0;
	GMPResults.Pitch_Man = 0.0;
	GMPResults.Yaw_Man = 0.0;

	g_Data.uplinkBufferSimt = 0;
	g_Data.connStatus = 0;
	g_Data.uplinkState = 0;
	if (vesseltype < 2)
	{
		g_Data.uplinkLEM = 0;
	}
	else
	{
		g_Data.uplinkLEM = 1;

		if (!stricmp(vessel->GetClassName(), "ProjectApollo\\LEM") ||
			!stricmp(vessel->GetClassName(), "ProjectApollo/LEM") ||
			!stricmp(vessel->GetClassName(), "ProjectApollo\\LEMSaturn") ||
			!stricmp(vessel->GetClassName(), "ProjectApollo/LEMSaturn")) {
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
	}
	for (int i = 0; i < 24; i++)
	{
		g_Data.emem[i] = 0;
	}
	ZeroMemory(&g_Data.burnData, sizeof(IMFD_BURN_DATA));
	g_Data.isRequesting = false;
	g_Data.progVessel = (Saturn *)vessel;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		sprintf(debugWinsock, "ERROR AT WSAStartup()");
	}
	else {
		sprintf(debugWinsock, "DISCONNECTED");
	}
	P30TIG = 0;
	dV_LVLH = _V(0.0, 0.0, 0.0);
	for (int i = 0;i < 012;i++)
	{
		P30Octals[i] = 0;
	}
	for (int i = 0;i < 016;i++)
	{
		RetrofireEXDVOctals[i] = 0;
	}

	EntryTIG = 0.0;
	EntryLat = 0.0;
	EntryLng = 0.0;
	EntryTIGcor = 0.0;
	EntryLatcor = 0.0;
	EntryLngcor = 0.0;
	EntryAng = 0.0;
	EntryAngcor = 0.0;
	Entry_DV = _V(0.0, 0.0, 0.0);
	entrycritical = 1;
	RTEReentryTime = 0.0;
	entrynominal = 1;
	entryrange = 0.0;
	EntryRTGO = 0.0;
	FlybyPeriAlt = 0.0;
	EntryDesiredInclination = 0.0;
	RTECalcMode = 1;
	RTEReturnInclination = 0.0;
	RTEMaxReentrySpeed = 36323.0*0.3048;

	SVSlot = true; //true = CSM; false = Other
	SVDesiredGET = 0.0;
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
	sprintf(lmmanpad.remarks, "");
	entrypadopt = 0;
	csmenginetype = 0;
	lemenginetype = 1;
	TPIPAD_AZ = 0.0;
	TPIPAD_dH = 0.0;
	TPIPAD_dV_LOS = _V(0.0, 0.0, 0.0);
	TPIPAD_ELmin5 = 0.0;
	TPIPAD_R = 0.0;
	TPIPAD_Rdot = 0.0;
	TPIPAD_ddH = 0.0;
	TPIPAD_BT = _V(0.0, 0.0, 0.0);
	sxtstardtime = 0.0;
	P37GET400K = 0.0;
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
	inhibUplLOS = false;
	PADSolGood = true;
	svtarget = NULL;
	svtargetnumber = -1;

	for (int i = 0;i < 021;i++)
	{
		SVOctals[i] = 0;
	}
	RLSUplink = _V(0, 0, 0);
	for (int i = 0;i < 010;i++)
	{
		RLSOctals[i] = 0;
	}

	lambertmultiaxis = 1;
	entrylongmanual = true;
	landingzone = 0;
	entryprecision = -1;

	LOImaneuver = 0;
	LOIOption = 0;
	LOI_dV_LVLH = _V(0.0, 0.0, 0.0);
	LOI_TIG = 0.0;
	LOI2Alt = 60.0*1852.0;
	LOI2_EarliestGET = 0.0;

	TLCCmaneuver = 2;
	TLCC_GET = 0.0;
	TLCCEMPLatcor = 0.0;
	TLCC_dV_LVLH = _V(0.0, 0.0, 0.0);
	TLCCPeriGETcor = 0.0;
	TLCC_TIG = 0.0;
	TLCCReentryGET = 0.0;
	TLCCSolGood = true;
	TLCCFRIncl = 0.0;
	TLCCFRLat = 0.0;
	TLCCFRLng = 0.0;
	TLCCAscendingNode = true;
	TLCCFRDesiredInclination = 0.0;
	TLCCIterationStep = 0;
	TLCCRev2MeridianGET = 0.0;
	TLCCPostDOIApoAlt = 0.0;
	TLCCPostDOIPeriAlt = 0.0;
	
	tlipad.TB6P = 0.0;
	tlipad.BurnTime = 0.0;
	tlipad.dVC = 0.0;
	tlipad.VI = 0.0;
	tlipad.SepATT = _V(0.0, 0.0, 0.0);
	tlipad.IgnATT = _V(0.0, 0.0, 0.0);
	tlipad.ExtATT = _V(0.0, 0.0, 0.0);
	R_TLI = _V(0, 0, 0);
	V_TLI = _V(0, 0, 0);

	pdipad.Att = _V(0, 0, 0);
	pdipad.CR = 0.0;
	pdipad.DEDA231 = 0.0;
	pdipad.GETI = 0.0;
	pdipad.t_go = 0.0;

	subThreadMode = 0;
	subThreadStatus = 0;

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
	DOI_TIG = 0.0;
	DOI_t_PDI = 0.0;
	DOI_CR = 0.0;
	DOIGET = 0.0;

	AGSKFactor = 90.0*3600.0;

	DKI_Profile = 0;
	DKI_TPI_Mode = 0;
	DKI_Radial_DV = false;
	DKI_TIG = 0.0;
	DKI_DV = _V(0, 0, 0);
	DKI_Maneuver_Line = true;
	DKI_dt_TPI_sunrise = 16.0*60.0;
	DKI_N_HC = 1;
	DKI_N_PB = 1;
	DKI_dt_PBH = DKI_dt_BHAM = DKI_dt_HAMH = 3600.0;
	dkiresult.DV_Phasing = _V(0, 0, 0);
	dkiresult.t_CDH = 0.0;
	dkiresult.dv_CSI = 0.0;
	dkiresult.t_CSI = 0.0;
	dkiresult.t_TPI = 0.0;
	dkiresult.DV_CDH = _V(0, 0, 0);
	dkiresult.t_Boost = 0.0;
	dkiresult.dv_Boost = 0.0;
	dkiresult.t_HAM = 0.0;

	if (GC->mission == 9)
	{
		AGSKFactor = 40.0*3600.0;
	}
	else if (GC->mission == 12)
	{
		AGSKFactor = 100.0*3600.0;
		//For PTC REFSMMAT
		REFSMMATTime = OrbMech::HHMMSSToSS(183, 0, 30);
	}
	else if (GC->mission == 13)
	{
		//For PTC REFSMMAT
		REFSMMATTime = OrbMech::HHMMSSToSS(178, 30, 0);
	}
	else if (GC->mission == 14)
	{
		AGSKFactor = 100.0*3600.0;
		//For PTC REFSMMAT
		REFSMMATTime = OrbMech::HHMMSSToSS(166, 10, 30);
	}
	else if (GC->mission == 15)
	{
		AGSKFactor = 100.0*3600.0;
		//For PTC REFSMMAT
		REFSMMATTime = OrbMech::HHMMSSToSS(230, 9, 0);
	}
	else if (GC->mission == 16)
	{
		//For PTC REFSMMAT
		REFSMMATTime = OrbMech::HHMMSSToSS(166, 2, 50);
	}
	else if (GC->mission == 17)
	{
		AGSKFactor = 110.0*3600.0;
		//For PTC REFSMMAT
		REFSMMATTime = OrbMech::HHMMSSToSS(241, 29, 30);
	}

	Skylabmaneuver = 0;
	SkylabTPIGuess = 0.0;
	Skylab_n_C = 1.5;
	SkylabDH1 = 20.0*1852.0;
	SkylabDH2 = 10.0*1852.0;
	Skylab_E_L = 27.0*RAD;
	SkylabSolGood = true;
	Skylab_dV_NSR = Skylab_dV_NCC = _V(0, 0, 0);
	Skylab_dH_NC2 = Skylab_dv_NC2 = Skylab_t_NC1 = Skylab_t_NC2 = Skylab_dv_NCC = Skylab_t_NCC = Skylab_t_NSR = Skylab_dt_TPM = 0.0;
	Skylab_NPCOption = Skylab_PCManeuver = false;

	PCAlignGET = 0.0;
	PClanded = true;
	PC_dV_LVLH = _V(0, 0, 0);
	PC_TIG = 0;
	PCEarliestGET = 0;

	TMLat = 0.0;
	TMLng = 0.0;
	TMAzi = 0.0;
	TMDistance = 600000.0*0.3048;
	TMStepSize = 100.0*0.3048;
	TMAlt = 0.0;

	LunarLiftoffTimeOption = 0;
	t_TPIguess = 0.0;
	DT_Ins_TPI = 40.0*60.0;
	t_Liftoff_guess = 0.0;
	LunarLiftoffInsVelInput = false;
	LunarLiftoffRes.t_CDH = 0.0;
	LunarLiftoffRes.t_CSI = 0.0;
	LunarLiftoffRes.t_Ins = 0.0;
	LunarLiftoffRes.t_L = 0.0;
	LunarLiftoffRes.t_TPI = 0.0;
	LunarLiftoffRes.t_TPF = 0.0;
	LunarLiftoffRes.v_LH = 5509.5*0.3048;
	LunarLiftoffRes.v_LV = 19.5*0.3048;
	LunarLiftoffRes.DV_CDH = 0.0;
	LunarLiftoffRes.DV_CSI = 0.0;
	LunarLiftoffRes.DV_T = 0.0;
	LunarLiftoffRes.DV_TPF = 0.0;
	LunarLiftoffRes.DV_TPI = 0.0;

	LAP_Theta = 10.0*RAD;
	LAP_DT = 7.0*60.0 + 15.0;

	EMPUplinkType = 0;
	EMPUplinkNumber = 0;

	LVDCLaunchAzimuth = 0.0;

	AGCEphemOption = 0;
	AGCEphemBRCSEpoch = AGCEpoch;
	AGCEphemTIMEM0 = floor(GC->GETbase) + 6.75;
	AGCEphemTEPHEM = GC->GETbase;
	AGCEphemTLAND = GC->t_Land;
	AGCEphemMission = GC->mission;
	AGCEphemIsCMC = vesseltype < 2;

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
	PDAP_J1 = 0.0;
	PDAP_J2 = 0.0;
	PDAP_K1 = 0.0;
	PDAP_K2 = 0.0;
	PDAP_Theta_LIM = 0.0;
	PDAP_R_amin = 0.0;

	nextstatcont_lunar = false;
	predsiteacq_GET = 0.0;
	predsiteacq_DT = 0.0;
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
			//g_Data.uplinkDataReady = 0;
			//g_Data.updateClockReady = 0;
			g_Data.connStatus = 0;
			closesocket(m_socket);
		}
		//else if (g_Data.connStatus == 2 && g_Data.updateClockReady == 2) {
		//	UpdateClock();
		//}
	}
	if (g_Data.isRequesting && g_Data.progVessel->GetIMFDClient()->IsBurnDataValid()) {
		g_Data.burnData = g_Data.progVessel->GetIMFDClient()->GetBurnData();
		StopIMFDRequest();
		if (g_Data.burnData.p30mode && !g_Data.burnData.impulsive) {
			//g_Data.errorMessage = "IMFD not in Off-Axis, P30 Mode";
			//g_Data.progState = PROGSTATE_TLI_ERROR;
			dV_LVLH = g_Data.burnData._dV_LVLH;
			P30TIG = (g_Data.burnData.IgnMJD - GC->GETbase)*24.0*3600.0;

			double *EarthPos;
			EarthPos = new double[12];
			VECTOR3 EarthVec, EarthVecVel;
			CELBODY *cEarth;
			OBJHANDLE hEarth = oapiGetObjectByName("Earth");

			cEarth = oapiGetCelbodyInterface(hEarth);
			cEarth->clbkEphemeris(g_Data.burnData.IgnMJD + g_Data.burnData.BT / 24.0 / 3600.0, EPHEM_BARYPOS | EPHEM_BARYVEL, EarthPos);

			EarthVec = _V(EarthPos[0], EarthPos[1], EarthPos[2]);
			EarthVecVel = _V(EarthPos[3], EarthPos[4], EarthPos[5]);

			R_TLI = g_Data.burnData._RCut - EarthVec;
			R_TLI = _V(R_TLI.x, R_TLI.z, R_TLI.y);
			V_TLI = g_Data.burnData._VCut - EarthVecVel;
			V_TLI = _V(V_TLI.x, V_TLI.z, V_TLI.y);
		}
		else {
			//g_Data.progState = PROGSTATE_TLI_WAITING;
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

void ARCore::DOICalc()
{
	startSubthread(10);
}

void ARCore::SkylabCalc()
{
	startSubthread(12);
}

void ARCore::PCCalc()
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
	GC->rtcc->EntryUpdateCalc(sv0, GC->GETbase, entryrange, true, &res);

	EntryLatcor = res.latitude;
	EntryLngcor = res.longitude;
	EntryRTGO = res.RTGO;
}

void ARCore::EntryCalc()
{
	startSubthread(7);
}

void ARCore::DeorbitCalc()
{
	startSubthread(17);
}

void ARCore::MoonRTECalc()
{
	startSubthread(11);
}

void ARCore::CDHcalc()			//Calculates the required DV vector of a coelliptic burn
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

void ARCore::UpdateFIDOOrbitDigitals()
{
	startSubthread(23);
}

void ARCore::CycleFIDOOrbitDigitals()
{
	if (subThreadStatus == 0 && fidoorbitsv.gravref != NULL)
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->GETbase);
		if (GET > fidoorbit.GET + 12.0)
		{
			startSubthread(24);
		}
		else if ((GET > fidoorbit.GETA && fidoorbit.E < 1.0) || GET > fidoorbit.GETP)
		{
			startSubthread(25);
		}
	}
}

void ARCore::FIDOOrbitDigitalsCalculateLongitude()
{
	if (subThreadStatus == 0 && fidoorbitsv.gravref != NULL)
	{
		startSubthread(26);
	}
}

void ARCore::FIDOOrbitDigitalsCalculateGETL()
{
	if (subThreadStatus == 0 && fidoorbitsv.gravref != NULL)
	{
		startSubthread(27);
	}
}

void ARCore::FIDOOrbitDigitalsApoPeriRequest()
{
	if (subThreadStatus == 0 && fidoorbitsv.gravref != NULL)
	{
		startSubthread(34);
	}
}

void ARCore::UpdateSpaceDigitals()
{
	startSubthread(28);
}

void ARCore::CycleSpaceDigitals()
{
	if (subThreadStatus == 0 && spacedigitalssv.gravref != NULL)
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->GETbase);
		if (GET > spacedigit.GET + 12.0)
		{
			startSubthread(29);
		}
	}
}

void ARCore::SpaceDigitalsGET()
{
	if (subThreadStatus == 0 && spacedigitalssv.gravref != NULL)
	{
		startSubthread(30);
	}
}

void ARCore::CycleNextStationContactsDisplay()
{
	if (subThreadStatus == 0)
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->GETbase);
		if (GET > orbitstatconttable.GET + 12.0)
		{
			startSubthread(35);
		}
		else if (GET > nextstatconttable.GET + 12.0)
		{
			startSubthread(36);
		}
	}
}

void ARCore::CyclePredictedSiteAcquisitionDisplay()
{
	if (subThreadStatus == 0)
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->GETbase);
		if (GET > orbitstatconttable.GET + 12.0)
		{
			startSubthread(35);
		}
	}
}

void ARCore::CalculatePredictedSiteAcquisitionDisplay()
{
	if (subThreadStatus == 0)
	{
		startSubthread(37);
	}
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

void ARCore::DAPPADCalc()
{
	if (vesseltype < 2)
	{
		GC->rtcc->CSMDAPUpdate(vessel, DAP_PAD);
	}
	else
	{
		GC->rtcc->LMDAPUpdate(vessel, DAP_PAD, lemdescentstage == false);
	}
}

void ARCore::GenerateAGCEphemeris()
{
	AGCEphemeris(AGCEphemTIMEM0, AGCEphemBRCSEpoch, AGCEphemTEphemZero);
}

void ARCore::GenerateAGCCorrectionVectors()
{
	AGCCorrectionVectors(AGCEphemTEPHEM, AGCEphemTLAND, AGCEphemMission, AGCEphemIsCMC);
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

void ARCore::NavCheckPAD()
{
	SV sv;

	sv = GC->rtcc->StateVectorCalc(vessel);

	GC->rtcc->NavCheckPAD(sv, navcheckpad, GC->GETbase, navcheckpad.NavChk[0]);
}

void ARCore::LandingSiteUpdate()
{
	double lat, lng, rad;
	svtarget->GetEquPos(lng, lat, rad);

	GC->LSLat = lat;
	GC->LSLng = lng;
	GC->LSAlt = rad - oapiGetSize(svtarget->GetGravityRef());
}

void ARCore::LSUplinkCalc()
{
	VECTOR3 R_P;
	double r_0;

	R_P = unit(_V(cos(GC->LSLng)*cos(GC->LSLat), sin(GC->LSLng)*cos(GC->LSLat), sin(GC->LSLat)));
	r_0 = oapiGetSize(oapiGetObjectByName("Moon"));

	RLSUplink = R_P * (r_0 + GC->LSAlt);

	RLSOctals[0] = 10;

	if (vesseltype < 2)
	{
		RLSOctals[1] = 2025;
	}
	else
	{
		if (GC->mission < 14)
		{
			RLSOctals[1] = 2022;
		}
		else
		{
			RLSOctals[1] = 2020;
		}
	}

	RLSOctals[2] = OrbMech::DoubleToBuffer(RLSUplink.x, 27, 1);
	RLSOctals[3] = OrbMech::DoubleToBuffer(RLSUplink.x, 27, 0);
	RLSOctals[4] = OrbMech::DoubleToBuffer(RLSUplink.y, 27, 1);
	RLSOctals[5] = OrbMech::DoubleToBuffer(RLSUplink.y, 27, 0);
	RLSOctals[6] = OrbMech::DoubleToBuffer(RLSUplink.z, 27, 1);
	RLSOctals[7] = OrbMech::DoubleToBuffer(RLSUplink.z, 27, 0);
}

void ARCore::LandingSiteUplink()
{
	for (int i = 0;i < 010;i++)
	{
		g_Data.emem[i] = RLSOctals[i];
	}

	UplinkData();
}

void ARCore::StateVectorCalc()
{
	SV sv0, sv1;
	MATRIX3 Rot;
	VECTOR3 pos, vel;
	double get;
	OBJHANDLE hMoon = oapiGetGbodyByName("Moon");
	OBJHANDLE hEarth = oapiGetGbodyByName("Earth");

	int mptveh;

	if (SVSlot)
	{
		mptveh = 2;
	}
	else
	{
		mptveh = 1;
	}

	if (!GC->MissionPlanningActive || !GC->rtcc->MPTTrajectory(GC->mptable, SVDesiredGET, GC->GETbase, sv0, mptveh))
	{
		sv0 = GC->rtcc->StateVectorCalc(svtarget);
		sv1 = GC->rtcc->GeneralTrajectoryPropagation(sv0, 0, OrbMech::MJDfromGET(SVDesiredGET, GC->GETbase));
	}
	else
	{
		sv1 = sv0;
	}

	UplinkSV = sv1;

	Rot = OrbMech::J2000EclToBRCS(AGCEpoch);

	UplinkSV.R = mul(Rot, UplinkSV.R);
	UplinkSV.V = mul(Rot, UplinkSV.V);
	UplinkSV.MJD = OrbMech::GETfromMJD(UplinkSV.MJD, GC->GETbase);

	pos = UplinkSV.R;
	vel = UplinkSV.V*0.01;
	get = UplinkSV.MJD;

	SVOctals[0] = 21;
	SVOctals[1] = 1501;

	if (sv1.gravref == hMoon)
	{
		if (SVSlot)
		{
			SVOctals[2] = 2;
		}
		else
		{
			SVOctals[2] = 77775;	// Octal coded decimal
		}

		SVOctals[3] = OrbMech::DoubleToBuffer(pos.x, 27, 1);
		SVOctals[4] = OrbMech::DoubleToBuffer(pos.x, 27, 0);
		SVOctals[5] = OrbMech::DoubleToBuffer(pos.y, 27, 1);
		SVOctals[6] = OrbMech::DoubleToBuffer(pos.y, 27, 0);
		SVOctals[7] = OrbMech::DoubleToBuffer(pos.z, 27, 1);
		SVOctals[8] = OrbMech::DoubleToBuffer(pos.z, 27, 0);
		SVOctals[9] = OrbMech::DoubleToBuffer(vel.x, 5, 1);
		SVOctals[10] = OrbMech::DoubleToBuffer(vel.x, 5, 0);
		SVOctals[11] = OrbMech::DoubleToBuffer(vel.y, 5, 1);
		SVOctals[12] = OrbMech::DoubleToBuffer(vel.y, 5, 0);
		SVOctals[13] = OrbMech::DoubleToBuffer(vel.z, 5, 1);
		SVOctals[14] = OrbMech::DoubleToBuffer(vel.z, 5, 0);
		SVOctals[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		SVOctals[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
		return;
	}

	if (sv1.gravref == hEarth)
	{
		if (SVSlot)
		{
			SVOctals[2] = 1;
		}
		else
		{
			SVOctals[2] = 77776;	// Octal coded decimal
		}

		SVOctals[3] = OrbMech::DoubleToBuffer(pos.x, 29, 1);
		SVOctals[4] = OrbMech::DoubleToBuffer(pos.x, 29, 0);
		SVOctals[5] = OrbMech::DoubleToBuffer(pos.y, 29, 1);
		SVOctals[6] = OrbMech::DoubleToBuffer(pos.y, 29, 0);
		SVOctals[7] = OrbMech::DoubleToBuffer(pos.z, 29, 1);
		SVOctals[8] = OrbMech::DoubleToBuffer(pos.z, 29, 0);
		SVOctals[9] = OrbMech::DoubleToBuffer(vel.x, 7, 1);
		SVOctals[10] = OrbMech::DoubleToBuffer(vel.x, 7, 0);
		SVOctals[11] = OrbMech::DoubleToBuffer(vel.y, 7, 1);
		SVOctals[12] = OrbMech::DoubleToBuffer(vel.y, 7, 0);
		SVOctals[13] = OrbMech::DoubleToBuffer(vel.z, 7, 1);
		SVOctals[14] = OrbMech::DoubleToBuffer(vel.z, 7, 0);
		SVOctals[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		SVOctals[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
		return;
	}
}

void ARCore::AGSStateVectorCalc()
{
	AGSSVOpt opt;
	SV sv;

	sv = GC->rtcc->StateVectorCalc(svtarget);

	opt.AGSbase = AGSKFactor;
	opt.csm = SVSlot;
	opt.GETbase = GC->GETbase;
	opt.REFSMMAT = REFSMMAT;
	opt.sv = sv;

	GC->rtcc->AGSStateVectorPAD(&opt, agssvpad);
}

void ARCore::StateVectorUplink()
{
	for (int i = 0;i < 021;i++)
	{
		g_Data.emem[i] = SVOctals[i];
	}

	UplinkData();
}


void ARCore::send_agc_key(char key)	{

	int bytesXmit = SOCKET_ERROR;
	unsigned char cmdbuf[4];

	if (vesseltype > 1){
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

void ARCore::REFSMMATUplink(void)
{
	for (int i = 0; i < 20; i++)
	{
		g_Data.emem[i] = REFSMMAToct[i];
	}
	UplinkData();
}

void ARCore::REFSMMATUplinkCalc()
{
	MATRIX3 a = mul(REFSMMAT, OrbMech::tmat(OrbMech::J2000EclToBRCS(AGCEpoch)));
	REFSMMAT_BRCS = a;

	//sprintf(oapiDebugString(), "%f, %f, %f, %f, %f, %f, %f, %f, %f", a.m11, a.m12, a.m13, a.m21, a.m22, a.m23, a.m31, a.m32, a.m33);

	REFSMMAToct[0] = 24;
	REFSMMAToct[1] = REFSMMATUplinkAddress();
	REFSMMAToct[2] = OrbMech::DoubleToBuffer(a.m11, 1, 1);
	REFSMMAToct[3] = OrbMech::DoubleToBuffer(a.m11, 1, 0);
	REFSMMAToct[4] = OrbMech::DoubleToBuffer(a.m12, 1, 1);
	REFSMMAToct[5] = OrbMech::DoubleToBuffer(a.m12, 1, 0);
	REFSMMAToct[6] = OrbMech::DoubleToBuffer(a.m13, 1, 1);
	REFSMMAToct[7] = OrbMech::DoubleToBuffer(a.m13, 1, 0);
	REFSMMAToct[8] = OrbMech::DoubleToBuffer(a.m21, 1, 1);
	REFSMMAToct[9] = OrbMech::DoubleToBuffer(a.m21, 1, 0);
	REFSMMAToct[10] = OrbMech::DoubleToBuffer(a.m22, 1, 1);
	REFSMMAToct[11] = OrbMech::DoubleToBuffer(a.m22, 1, 0);
	REFSMMAToct[12] = OrbMech::DoubleToBuffer(a.m23, 1, 1);
	REFSMMAToct[13] = OrbMech::DoubleToBuffer(a.m23, 1, 0);
	REFSMMAToct[14] = OrbMech::DoubleToBuffer(a.m31, 1, 1);
	REFSMMAToct[15] = OrbMech::DoubleToBuffer(a.m31, 1, 0);
	REFSMMAToct[16] = OrbMech::DoubleToBuffer(a.m32, 1, 1);
	REFSMMAToct[17] = OrbMech::DoubleToBuffer(a.m32, 1, 0);
	REFSMMAToct[18] = OrbMech::DoubleToBuffer(a.m33, 1, 1);
	REFSMMAToct[19] = OrbMech::DoubleToBuffer(a.m33, 1, 0);
}

void ARCore::P30Uplink(void)
{

	//if (saturn){ get = fabs(saturn->GetMissionTime()); }
	//if (crawler){ get = fabs(crawler->GetMissionTime()); }
	//if (lem){ get = fabs(lem->GetMissionTime()); }

	//double liftoff = oapiGetSimMJD() - get / 86400.0;
	double getign = P30TIG;// (pbd->IgnMJD - liftoff) * 86400.0;

	g_Data.emem[0] = 12;
	if (vesseltype < 2)
	{
		g_Data.emem[1] = 3404;
	}
	else
	{
		if (GC->mission < 11)
		{
			g_Data.emem[1] = 3431;
		}
		else
		{
			g_Data.emem[1] = 3433;
		}
	}
	g_Data.emem[2] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 1);
	g_Data.emem[3] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 0);
	g_Data.emem[4] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 1);
	g_Data.emem[5] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 0);
	g_Data.emem[6] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 1);
	g_Data.emem[7] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 0);
	g_Data.emem[8] = OrbMech::DoubleToBuffer(getign*100.0, 28, 1);
	g_Data.emem[9] = OrbMech::DoubleToBuffer(getign*100.0, 28, 0);

	//g_Data.uplinkDataReady = 2;
	UplinkData(); // Go for uplink
}

void ARCore::P30UplinkCalc()
{
	double getign = P30TIG;

	P30Octals[0] = 12;
	if (vesseltype < 2)
	{
		P30Octals[1] = 3404;
	}
	else
	{
		if (GC->mission < 11)
		{
			P30Octals[1] = 3431;
		}
		else
		{
			P30Octals[1] = 3433;
		}
	}
	P30Octals[2] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 1);
	P30Octals[3] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 0);
	P30Octals[4] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 1);
	P30Octals[5] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 0);
	P30Octals[6] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 1);
	P30Octals[7] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 0);
	P30Octals[8] = OrbMech::DoubleToBuffer(getign*100.0, 28, 1);
	P30Octals[9] = OrbMech::DoubleToBuffer(getign*100.0, 28, 0);
}

void ARCore::P30UplinkNew()
{
	for (int i = 0;i < 012;i++)
	{
		g_Data.emem[i] = P30Octals[i];
	}

	UplinkData();
}

void ARCore::RetrofireEXDVUplinkCalc()
{
	double getign = P30TIG;

	RetrofireEXDVOctals[0] = 16;
	RetrofireEXDVOctals[1] = 3400;
	RetrofireEXDVOctals[2] = OrbMech::DoubleToBuffer(EntryLatcor / PI2, 0, 1);
	RetrofireEXDVOctals[3] = OrbMech::DoubleToBuffer(EntryLatcor / PI2, 0, 0);
	RetrofireEXDVOctals[4] = OrbMech::DoubleToBuffer(EntryLngcor / PI2, 0, 1);
	RetrofireEXDVOctals[5] = OrbMech::DoubleToBuffer(EntryLngcor / PI2, 0, 0);
	RetrofireEXDVOctals[6] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 1);
	RetrofireEXDVOctals[7] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 0);
	RetrofireEXDVOctals[8] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 1);
	RetrofireEXDVOctals[9] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 0);
	RetrofireEXDVOctals[10] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 1);
	RetrofireEXDVOctals[11] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 0);
	RetrofireEXDVOctals[12] = OrbMech::DoubleToBuffer(getign*100.0, 28, 1);
	RetrofireEXDVOctals[13] = OrbMech::DoubleToBuffer(getign*100.0, 28, 0);
}

void ARCore::RetrofireEXDVUplinkNew()
{
	for (int i = 0;i < 016;i++)
	{
		g_Data.emem[i] = RetrofireEXDVOctals[i];
	}

	UplinkData();
}

void ARCore::EntryUplink(void)
{

	double getign = P30TIG;

	g_Data.emem[0] = 16;
	g_Data.emem[1] = 3400;
	g_Data.emem[2] = OrbMech::DoubleToBuffer(EntryLatcor / PI2, 0, 1);
	g_Data.emem[3] = OrbMech::DoubleToBuffer(EntryLatcor / PI2, 0, 0);
	g_Data.emem[4] = OrbMech::DoubleToBuffer(EntryLngcor / PI2, 0, 1);
	g_Data.emem[5] = OrbMech::DoubleToBuffer(EntryLngcor / PI2, 0, 0);
	g_Data.emem[6] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 1);
	g_Data.emem[7] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 0);
	g_Data.emem[8] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 1);
	g_Data.emem[9] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 0);
	g_Data.emem[10] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 1);
	g_Data.emem[11] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 0);
	g_Data.emem[12] = OrbMech::DoubleToBuffer(getign*100.0, 28, 1);
	g_Data.emem[13] = OrbMech::DoubleToBuffer(getign*100.0, 28, 0);

	//g_Data.uplinkDataReady = 2;
	UplinkData(); // Go for uplink
}

void ARCore::EntryUpdateUplink(void)
{
	g_Data.emem[0] = 06;
	g_Data.emem[1] = 3400;
	g_Data.emem[2] = OrbMech::DoubleToBuffer(EntryLatcor / PI2, 0, 1);
	g_Data.emem[3] = OrbMech::DoubleToBuffer(EntryLatcor / PI2, 0, 0);
	g_Data.emem[4] = OrbMech::DoubleToBuffer(EntryLngcor / PI2, 0, 1);
	g_Data.emem[5] = OrbMech::DoubleToBuffer(EntryLngcor / PI2, 0, 0);

	//g_Data.uplinkDataReady = 2;
	UplinkData(); // Go for uplink
}

void ARCore::TLANDUplink(void)
{
	if (vesseltype > 1)
	{
		g_Data.emem[0] = 5;

		if (GC->mission < 14)
		{
			g_Data.emem[1] = 2400;
			g_Data.emem[3] = 2401;
		}
		else
		{
			g_Data.emem[1] = 2026;
			g_Data.emem[3] = 2027;
		}
		g_Data.emem[2] = OrbMech::DoubleToBuffer(GC->t_Land*100.0, 28, 1);
		g_Data.emem[4] = OrbMech::DoubleToBuffer(GC->t_Land*100.0, 28, 0);

		UplinkData2(); // Go for uplink
	}
}

void ARCore::EMPP99Uplink(int i)
{
	if (vesseltype > 1)
	{
		if (i == 0)
		{
			g_Data.emem[0] = 24;
			g_Data.emem[1] = 3404;
			g_Data.emem[2] = 1450;
			g_Data.emem[3] = 12324;
			g_Data.emem[4] = 5520;
			g_Data.emem[5] = 161;
			g_Data.emem[6] = 1400;
			g_Data.emem[7] = 12150;
			g_Data.emem[8] = 5656;
			g_Data.emem[9] = 3667;
			g_Data.emem[10] = 74066;
			g_Data.emem[11] = 12404;
			g_Data.emem[12] = 12433;
			g_Data.emem[13] = 1406;
			g_Data.emem[14] = 5313;
			g_Data.emem[15] = 143;
			g_Data.emem[16] = 36266;
			g_Data.emem[17] = 54333;
			g_Data.emem[18] = 6060;
			g_Data.emem[19] = 77634;

			UplinkData(); // Go for uplink
		}
		else if(i == 1)
		{
			g_Data.emem[0] = 12;
			g_Data.emem[1] = 3734;
			g_Data.emem[2] = 26;
			g_Data.emem[3] = 30605;
			g_Data.emem[4] = 151;
			g_Data.emem[5] = 5214;
			g_Data.emem[6] = 0;
			g_Data.emem[7] = 0;
			g_Data.emem[8] = 15400;
			g_Data.emem[9] = 0;

			UplinkData(); // Go for uplink
		}
		else if (i == 2)
		{
			g_Data.emem[0] = 17;
			g_Data.emem[1] = 3400;
			g_Data.emem[2] = 5520;
			g_Data.emem[3] = 3401;
			g_Data.emem[4] = 312;
			g_Data.emem[5] = 3402;
			g_Data.emem[6] = 5263;
			g_Data.emem[7] = 3426;
			g_Data.emem[8] = 10636;
			g_Data.emem[9] = 3427;
			g_Data.emem[10] = 56246;
			g_Data.emem[11] = 3430;
			g_Data.emem[12] = 77650;
			g_Data.emem[13] = 3431;
			g_Data.emem[14] = 75202;

			UplinkData2(); // Go for uplink
		}
		else if (i == 3)
		{
			g_Data.emem[0] = 15;
			g_Data.emem[1] = 3455;
			g_Data.emem[2] = 1404;
			g_Data.emem[3] = 1250;
			g_Data.emem[4] = 0;
			g_Data.emem[5] = 3515;
			g_Data.emem[6] = 4;
			g_Data.emem[7] = 2371;
			g_Data.emem[8] = 13001;
			g_Data.emem[9] = 2372;
			g_Data.emem[10] = 1420;
			g_Data.emem[11] = 2373;
			g_Data.emem[12] = 12067;

			UplinkData2(); // Go for uplink
		}
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

	UplinkData(); // Go for uplink
}

void ARCore::UplinkData()
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
		if (g_Data.uplinkLEM > 0){ clientService.sin_port = htons(14243); }
		else{ clientService.sin_port = htons(14242); }
		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
			//g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "FAILED TO CONNECT, ERROR %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "CONNECTED");
		g_Data.uplinkState = 0;
		send_agc_key('V');
		send_agc_key('7');
		send_agc_key('1');
		send_agc_key('E');

		int cnt2 = (g_Data.emem[0] / 10);
		int cnt = (g_Data.emem[0] - (cnt2 * 10)) + cnt2 * 8;

		while (g_Data.uplinkState < cnt && cnt <= 20 && cnt >= 3)
			{
				sprintf(buffer, "%ld", g_Data.emem[g_Data.uplinkState]);
				uplink_word(buffer);
				g_Data.uplinkState++;
			}
		send_agc_key('V');
		send_agc_key('3');
		send_agc_key('3');
		send_agc_key('E');
		g_Data.connStatus = 1;
		g_Data.uplinkState = 0;
	}
}

void ARCore::UplinkData2()
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
		if (g_Data.uplinkLEM > 0){ clientService.sin_port = htons(14243); }
		else{ clientService.sin_port = htons(14242); }
		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
			//g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "FAILED TO CONNECT, ERROR %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "CONNECTED");
		g_Data.uplinkState = 0;
		send_agc_key('V');
		send_agc_key('7');
		send_agc_key('2');
		send_agc_key('E');

		int cnt2 = (g_Data.emem[0] / 10);
		int cnt = (g_Data.emem[0] - (cnt2 * 10)) + cnt2 * 8;

		while (g_Data.uplinkState < cnt && cnt <= 20 && cnt >= 3)
		{
			sprintf(buffer, "%ld", g_Data.emem[g_Data.uplinkState]);
			uplink_word(buffer);
			g_Data.uplinkState++;
		}
		send_agc_key('V');
		send_agc_key('3');
		send_agc_key('3');
		send_agc_key('E');
		g_Data.connStatus = 1;
		g_Data.uplinkState = 0;
	}
}

void ARCore::uplink_word(char *data)
{
	int i;
	for (i = 5; i > (int)strlen(data); i--) {
		send_agc_key('0');
	}
	for (i = 0; i < (int)strlen(data); i++) {
		send_agc_key(data[i]);
	}
	send_agc_key('E');
}

bool ARCore::vesselinLOS()
{
	VECTOR3 R, V;
	double MJD;
	OBJHANDLE gravref = GC->rtcc->AGCGravityRef(vessel);
	vessel->GetRelativePos(gravref, R);
	vessel->GetRelativeVel(gravref, V);
	MJD = oapiGetSimMJD();

	return OrbMech::vesselinLOS(R, V, MJD);
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
		relvec = _V(relvec.x, relvec.z, relvec.y);
		loc = _V(loc.x, loc.z, loc.y);

		UX = relvec;
		UY = unit(crossp(UX, -loc));
		UZ = unit(crossp(UX, crossp(UX, -loc)));

		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));

		VECangles = OrbMech::CALCGAR(REFSMMAT, mul(OrbMech::tmat(M), M_R));
	}
	else if (VECoption == 1)
	{
		VECangles = GC->rtcc->HatchOpenThermalControl(vessel, REFSMMAT);
	}
	else
	{
		SV sv;

		GC->rtcc->PointAOTWithCSM(REFSMMAT, sv, 2, 1, 0.0);
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

int ARCore::startSubthread(int fcn) {
	if (subThreadStatus < 1) {
		// Punt thread
		subThreadMode = fcn;
		subThreadStatus = 1; // Busy
		DWORD id = 0;
		hThread = CreateThread(NULL, 0, RTCCMFD_Trampoline, this, 0, &id);
	}
	else {
		//Kill thread
		DWORD exitcode = 0;
		if (TerminateThread(hThread, exitcode))
		{
			subThreadStatus = 0;
			if (hThread != NULL) { CloseHandle(hThread); }
		}
		return(-1);
	}
	return(0);
}

int ARCore::subThread()
{
	int Result = 0;

	int poweredenginetype, mptveh, docked, mptotherveh;

	if (vesseltype < 2)
	{
		mptveh = 2;
		mptotherveh = 1;
	}
	else
	{
		mptveh = 1;
		mptotherveh = 2;
	}

	if (vesseltype >= 2)
	{
		poweredenginetype = LEMToEngineType();
	}
	else
	{
		poweredenginetype = CSMToEngineType();
	}

	if (vesseltype == 1 || vesseltype == 3)
	{
		docked = true;
	}
	else
	{
		docked = false;
	}

	if (GC->MissionPlanningActive)
	{
		double thismass, othermass, cmass, lmass;

		thismass = vessel->GetMass();
		othermass = GC->rtcc->GetDockedVesselMass(vessel);

		if (vesseltype < 2)
		{
			cmass = thismass;
			lmass = othermass;
		}
		else
		{
			lmass = thismass;
			cmass = othermass;
		}

		GC->rtcc->MPTMassInit(GC->mptable, cmass, lmass);
	}

	subThreadStatus = 2; // Running
	switch (subThreadMode) {
	case 0: // Test
		Sleep(5000); // Waste 5 seconds
		Result = 0;  // Success (negative = error)
		break;
	case 1: //Lambert Targeting
	{
		LambertMan opt;
		TwoImpulseResuls res;
		SV sv_A, sv_P, sv_pre, sv_post;
		double attachedMass;

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, sv_A, mptveh))
			{
				sv_A = GC->rtcc->StateVectorCalc(vessel);
			}
			if (!GC->rtcc->MPTTrajectory(GC->mptable, sv_P, mptotherveh))
			{
				sv_P = GC->rtcc->StateVectorCalc(target);
			}
		}
		else
		{
			sv_A = GC->rtcc->StateVectorCalc(vessel);
			sv_P = GC->rtcc->StateVectorCalc(target);
		}

		opt.axis = !lambertmultiaxis;
		opt.Elevation = lambertelev;
		opt.GETbase = GC->GETbase;
		opt.N = N;
		opt.NCC_NSR_Flag = (twoimpulsemode == 1);
		opt.use_XYZ_Offset = (twoimpulsemode != 1);
		opt.Offset = offvec;
		opt.DH = DH;
		opt.Perturbation = lambertopt;
		opt.PhaseAngle = TwoImpulse_PhaseAngle;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.T1 = T1;
		opt.T2 = T2;	
		opt.elevOpt = lambertElevOpt;
		opt.TPFOpt = lambertTPFOpt;
		opt.DT = lambertDT;
		opt.WT = lambertWT;

		if (vesseltype == 0 || vesseltype == 2)
		{
			attachedMass = 0.0;
		}
		else
		{
			attachedMass = GC->rtcc->GetDockedVesselMass(vessel);
		}
		GC->rtcc->LambertTargeting(&opt, res);

		TwoImpulse_SV = sv_A;
		TwoImpulse_TIG = res.T1;
		LambertdeltaV = res.dV_LVLH;

		if (twoimpulsemode == 1)
		{
			TwoImpulse_TPI = res.t_TPI;
		}

		if (lambertElevOpt > 0)
		{
			T1 = res.T1;
		}
		if (lambertTPFOpt > 0)
		{
			T2 = res.T2;
		}

		//Reset these to display times
		lambertElevOpt = 0;
		lambertTPFOpt = 0;

		Result = 0;
	}
	break;
	case 2:	//Concentric Rendezvous Processor
	{
		SPQOpt opt;
		SPQResults res;
		SV sv_A, sv_P, sv_pre, sv_post;

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, sv_A, mptveh))
			{
				sv_A = GC->rtcc->StateVectorCalc(vessel);
			}
			if (!GC->rtcc->MPTTrajectory(GC->mptable, sv_P, mptotherveh))
			{
				sv_P = GC->rtcc->StateVectorCalc(target);
			}
		}
		else
		{
			sv_A = GC->rtcc->StateVectorCalc(vessel);
			sv_P = GC->rtcc->StateVectorCalc(target);
		}

		opt.DH = DH;
		opt.E = lambertelev;
		opt.GETbase = GC->GETbase;
		opt.K_CSI = (SPQMode == 0);
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		if (SPQMode == 0)
		{
			opt.t_TIG = CSItime;
			opt.K_CDH = CDHtimemode;
		}
		else
		{
			if (CDHtimemode == 0)
			{
				opt.t_TIG = CDHtime;
			}
			else
			{
				opt.t_TIG = GC->rtcc->FindDH(sv_A, sv_P, GC->GETbase, CDHtime, DH);
			}
		}
		opt.t_TPI = t_TPI;

		GC->rtcc->ConcentricRendezvousProcessor(opt, res);
		spqresults = res;

		SPQTIG = opt.t_TIG;

		if (SPQMode == 0)
		{
			CDHtime = res.t_CDH;
			SPQDeltaV = res.dV_CSI;
		}
		else
		{
			SPQDeltaV = res.dV_CDH;
		}

		Result = 0;
	}
	break;
	case 3:	//Orbital Adjustment Targeting
	{
		GMPOpt opt;
		SV sv0, sv_pre, sv_post;
		double TIG_imp, attachedMass;
		VECTOR3 dV_imp;

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, sv0, mptveh))
			{
				sv0 = GC->rtcc->StateVectorCalc(vessel);
			}
		}
		else
		{
			sv0 = GC->rtcc->StateVectorCalc(vessel);
		}

		opt.ManeuverCode = GMPManeuverCode;
		opt.GETbase = GC->GETbase;
		opt.H_A = GMPApogeeHeight;
		opt.H_P = GMPPerigeeHeight;
		opt.dH_D = GMPHeightChange;
		opt.TIG_GET = SPSGET;
		opt.AltRef = OrbAdjAltRef;
		opt.dLAN = GMPNodeShiftAngle;
		opt.LSAlt = GC->LSAlt;
		opt.dW = GMPWedgeAngle;
		opt.long_D = GMPManeuverLongitude;
		opt.H_D = GMPManeuverHeight;
		opt.dV = GMPDeltaVInput;
		opt.Pitch = GMPPitch;
		opt.Yaw = GMPYaw;
		opt.dLOA = GMPApseLineRotAngle;
		opt.N = GMPRevs;
		opt.RV_MCC = sv0;

		if (vesseltype == 0 || vesseltype == 2)
		{
			attachedMass = 0.0;
		}
		else
		{
			attachedMass = GC->rtcc->GetDockedVesselMass(vessel);
		}

		GC->rtcc->GeneralManeuverProcessor(&opt, dV_imp, TIG_imp, GMPResults);
		GC->rtcc->PoweredFlightProcessor(sv0, GC->GETbase, TIG_imp, poweredenginetype, attachedMass, dV_imp, false, P30TIG, OrbAdjDVX, sv_pre, sv_post);

		dV_LVLH = OrbAdjDVX;

		if (GC->MissionPlanningActive)
		{
			char code[64];

			sprintf(code, "GPM");

			GC->rtcc->MPTAddManeuver(GC->mptable, sv_pre, sv_post, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
		}

		Result = 0;
	}
	break;
	case 4:	//REFSMMAT Calculation
	{
		REFSMMATOpt opt;

		opt.dV_LVLH = dV_LVLH;
		opt.GETbase = GC->GETbase;
		opt.LSAzi = GC->LOIazi;
		opt.LSLat = GC->LSLat;
		opt.LSLng = GC->LSLng;
		opt.mission = GC->mission;
		opt.REFSMMATopt = REFSMMATopt;

		if (REFSMMATopt == 0 || REFSMMATopt == 1)
		{
			opt.REFSMMATTime = P30TIG;
		}
		else if (REFSMMATopt == 5 || REFSMMATopt == 8)
		{
			opt.REFSMMATTime = GC->t_Land;
		}
		else
		{
			opt.REFSMMATTime = REFSMMATTime;
		}

		opt.vessel = vessel;
		opt.vesseltype = vesseltype;
		opt.HeadsUp = REFSMMATHeadsUp;
		opt.PresentREFSMMAT = REFSMMAT;
		opt.IMUAngles = VECangles;

		if (vesseltype == 0 || vesseltype == 2)
		{
			opt.csmlmdocked = false;
		}
		else
		{
			opt.csmlmdocked = true;
		}

		if (GC->MissionPlanningActive && GC->rtcc->MPTHasManeuvers(GC->mptable, mptveh))
		{
			opt.useSV = true;

			if (REFSMMATopt == 0 || REFSMMATopt == 1 || REFSMMATopt == 2 || REFSMMATopt == 5)
			{
				//SV at specified time
				GC->rtcc->MPTTrajectory(GC->mptable, opt.REFSMMATTime, GC->GETbase, opt.RV_MCC, mptveh);
			}
			else if (REFSMMATopt == 3)
			{
				//Last SV in the table
				GC->rtcc->MPTTrajectory(GC->mptable, opt.RV_MCC, mptveh);
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

		REFSMMAT = GC->rtcc->REFSMMATCalc(&opt);

		//sprintf(oapiDebugString(), "%f, %f, %f, %f, %f, %f, %f, %f, %f", REFSMMAT.m11, REFSMMAT.m12, REFSMMAT.m13, REFSMMAT.m21, REFSMMAT.m22, REFSMMAT.m23, REFSMMAT.m31, REFSMMAT.m32, REFSMMAT.m33);

		REFSMMATcur = REFSMMATopt;

		Result = 0;
	}
	break;
	case 5: //LOI Targeting
	{
		SV sv0;

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, sv0, mptveh))
			{
				sv0 = GC->rtcc->StateVectorCalc(vessel);
			}
		}
		else
		{
			sv0 = GC->rtcc->StateVectorCalc(vessel);
		}

		if (LOImaneuver == 0)
		{
			LOIMan loiopt;
			SV sv_n, sv_preLOI, sv_postLOI;

			loiopt.GETbase = GC->GETbase;
			loiopt.h_apo = GC->LOIapo;
			loiopt.h_peri = GC->LOIperi;
			loiopt.lat = GC->LSLat;
			loiopt.lng = GC->LSLng;
			loiopt.alt = GC->LSAlt;
			loiopt.t_land = GC->t_Land;
			loiopt.azi = GC->LOIazi;
			loiopt.vessel = vessel;
			loiopt.type = LOIOption;
			loiopt.EllipseRotation = GC->LOIEllipseRotation;
			loiopt.RV_MCC = sv0;
			loiopt.enginetype = poweredenginetype;

			if (vesseltype == 0 || vesseltype == 2)
			{
				loiopt.csmlmdocked = false;
			}
			else
			{
				loiopt.csmlmdocked = true;
			}

			GC->rtcc->LOITargeting(&loiopt, LOI_dV_LVLH, LOI_TIG, sv_n, sv_preLOI, sv_postLOI);
			P30TIG = LOI_TIG;
			dV_LVLH = LOI_dV_LVLH;

			if (GC->MissionPlanningActive)
			{
				char code[64];

				sprintf(code, "LOI");

				GC->rtcc->MPTAddManeuver(GC->mptable, sv_preLOI, sv_postLOI, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
			}
		}
		else if (LOImaneuver == 1)
		{
			LOI2Man opt;

			SV sv_pre, sv_post;

			opt.alt = GC->LSAlt;
			opt.EarliestGET = LOI2_EarliestGET;
			opt.GETbase = GC->GETbase;
			opt.h_circ = LOI2Alt;
			opt.vessel = vessel;
			opt.RV_MCC = sv0;

			if (vesseltype < 2)
			{
				opt.vesseltype = 0;
			}
			else
			{
				opt.vesseltype = 1;
			}

			if (vesseltype == 0 || vesseltype == 2)
			{
				opt.csmlmdocked = false;
			}
			else
			{
				opt.csmlmdocked = true;
			}

			GC->rtcc->LOI2Targeting(&opt, LOI_dV_LVLH, LOI_TIG, sv_pre, sv_post);
			P30TIG = LOI_TIG;
			dV_LVLH = LOI_dV_LVLH;

			if (GC->MissionPlanningActive)
			{
				char code[64];

				sprintf(code, "LO2");

				GC->rtcc->MPTAddManeuver(GC->mptable, sv_pre, sv_post, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
			}
		}

		Result = 0;
	}
	break;
	case 6: //TPI PAD
	{
		AP7TPIPADOpt opt;
		AP7TPI pad;

		opt.dV_LVLH = dV_LVLH;
		opt.GETbase = GC->GETbase;
		opt.TIG = P30TIG;
		opt.sv_A = GC->rtcc->StateVectorCalc(vessel);
		opt.sv_P = GC->rtcc->StateVectorCalc(target);

		GC->rtcc->AP7TPIPAD(opt, pad);

		TPIPAD_AZ = pad.AZ;
		TPIPAD_BT = pad.Backup_bT;
		TPIPAD_ddH = pad.dH_Max;
		TPIPAD_dH = pad.dH_TPI;
		TPIPAD_dV_LOS = pad.Backup_dV;
		TPIPAD_ELmin5 = pad.EL;
		TPIPAD_R = pad.R;
		TPIPAD_Rdot = pad.Rdot;

		Result = 0;
	}
	break;
	case 7:	//Entry Targeting
	{
		EntryResults res;
		EntryOpt opt;

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, EntryTIG, GC->GETbase, opt.RV_MCC, mptveh))
			{
				opt.RV_MCC = GC->rtcc->StateVectorCalc(vessel);
			}
		}
		else
		{
			opt.RV_MCC = GC->rtcc->StateVectorCalc(vessel);
		}

		if (vesseltype == 0 || vesseltype == 2)
		{
			opt.csmlmdocked = false;
		}
		else
		{
			opt.csmlmdocked = true;
		}

		if (entrylongmanual)
		{
			opt.lng = EntryLng;
		}
		else
		{
			opt.lng = (double)landingzone;
		}

		opt.GETbase = GC->GETbase;
		opt.enginetype = poweredenginetype;
		opt.entrylongmanual = entrylongmanual;
		opt.ReA = EntryAng;
		opt.TIGguess = EntryTIG;
		opt.vessel = vessel;
		opt.type = entrycritical;
		opt.r_rbias = GC->RTERangeOverrideNM;

		GC->rtcc->EntryTargeting(&opt, &res);

		Entry_DV = res.dV_LVLH;
		EntryTIGcor = res.P30TIG;
		EntryLatcor = res.latitude;
		EntryLngcor = res.longitude;
		P37GET400K = res.GET05G;
		EntryRTGO = res.RTGO;
		EntryAngcor = res.ReA;
		P30TIG = EntryTIGcor;
		dV_LVLH = Entry_DV;
		entryprecision = res.precision;

		if (GC->MissionPlanningActive)
		{
			char code[64];

			if (entrycritical == 2)
			{
				sprintf(code, "DAM");
			}
			else
			{
				sprintf(code, "TEMCC");
			}

			GC->rtcc->MPTAddManeuver(GC->mptable, res.sv_preburn, res.sv_postburn, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
		}

		Result = 0;
	}
	break;
	case 8: //TLI PAD
	{
		SaturnV *SatV = (SaturnV*)g_Data.progVessel;
		LVDCSV *lvdc = (LVDCSV*)SatV->iu->GetLVDC();

		TLIPADOpt opt;

		SV sv_A, sv_IG, sv_TLI;
		sv_A = GC->rtcc->StateVectorCalc(vessel);

		if (lvdc->TU)
		{
			opt.dV_LVLH = dV_LVLH;
			opt.GETbase = GC->GETbase;
			opt.REFSMMAT = REFSMMAT;
			opt.TIG = P30TIG;
			opt.vessel = vessel;
			opt.uselvdc = false;
			opt.SeparationAttitude = _V(PI, 120.0*RAD, 0.0);
		}
		else
		{
			LVDCTLIparam tliparam;

			tliparam.alpha_TS = lvdc->alpha_TS;
			tliparam.Azimuth = lvdc->Azimuth;
			tliparam.beta = lvdc->beta;
			tliparam.cos_sigma = lvdc->cos_sigma;
			tliparam.C_3 = lvdc->C_3;
			tliparam.e_N = lvdc->e_N;
			tliparam.f = lvdc->f;
			tliparam.mu = lvdc->mu;
			tliparam.MX_A = lvdc->MX_A;
			tliparam.omega_E = lvdc->omega_E;
			tliparam.phi_L = lvdc->PHI;
			tliparam.R_N = lvdc->R_N;
			tliparam.T_2R = lvdc->T_2R;
			tliparam.TargetVector = lvdc->TargetVector;
			tliparam.TB5 = lvdc->TB5;
			tliparam.theta_EO = lvdc->theta_EO;
			tliparam.t_D = lvdc->t_D;
			tliparam.T_L = lvdc->T_L;
			tliparam.T_RG = lvdc->T_RG;
			tliparam.T_ST = lvdc->T_ST;
			tliparam.Tt_3R = lvdc->Tt_3R;
			tliparam.t_clock = lvdc->t_clock;

			GC->rtcc->LVDCTLIPredict(tliparam, vessel, sv_A, GC->GETbase, dV_LVLH, P30TIG, sv_IG, sv_TLI);

			R_TLI = sv_TLI.R;
			V_TLI = sv_TLI.V;

			if (GC->MissionPlanningActive)
			{
				GC->rtcc->MPTAddTLI(GC->mptable, sv_IG, sv_TLI, length(dV_LVLH));
			}

			opt.dV_LVLH = dV_LVLH;
			opt.GETbase = GC->GETbase;
			opt.REFSMMAT = REFSMMAT;
			opt.TIG = P30TIG;
			opt.vessel = vessel;
			opt.SeparationAttitude = lvdc->XLunarAttitude;
			opt.TLI = OrbMech::GETfromMJD(sv_TLI.MJD, GC->GETbase);
			opt.R_TLI = R_TLI;
			opt.V_TLI = V_TLI;
			opt.uselvdc = true;
		}

		opt.sv0 = sv_A;

		GC->rtcc->TLI_PAD(&opt, tlipad);

		Result = 0;
	}
	break;
	case 9: //Maneuver PAD
	{
		SV sv_A;

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, P30TIG, GC->GETbase, sv_A, mptveh))
			{
				sv_A = GC->rtcc->StateVectorCalc(vessel);
			}
		}
		else
		{
			sv_A = GC->rtcc->StateVectorCalc(vessel);
		}

		if (vesseltype < 2)
		{
			AP11ManPADOpt opt;

			opt.dV_LVLH = dV_LVLH;
			opt.enginetype = poweredenginetype;
			opt.GETbase = GC->GETbase;
			opt.HeadsUp = HeadsUp;
			opt.REFSMMAT = REFSMMAT;
			opt.sxtstardtime = sxtstardtime;
			opt.TIG = P30TIG;
			opt.vessel = vessel;
			opt.vesseltype = vesseltype;
			opt.alt = GC->LSAlt;
			opt.useSV = true;
			opt.RV_MCC = sv_A;

			GC->rtcc->AP11ManeuverPAD(&opt, manpad);
		}
		else
		{
			AP11LMManPADOpt opt;

			opt.dV_LVLH = dV_LVLH;
			opt.enginetype = poweredenginetype;
			opt.GETbase = GC->GETbase;
			opt.HeadsUp = HeadsUp;
			opt.REFSMMAT = REFSMMAT;
			opt.sxtstardtime = sxtstardtime;
			opt.TIG = P30TIG;
			opt.vessel = vessel;
			if (vesseltype == 2)
			{
				opt.csmlmdocked = false;
			}
			else
			{
				opt.csmlmdocked = true;
			}
			opt.alt = GC->LSAlt;
			opt.useSV = true;
			opt.RV_MCC = sv_A;

			GC->rtcc->AP11LMManeuverPAD(&opt, lmmanpad);
		}

		Result = 0;
	}
	break;
	case 10:	//DOI Targeting
	{
		SV sv, sv_pre, sv_post;
		DOIMan opt;
		VECTOR3 DOI_DV_imp;
		double DOI_TIG_imp, attachedMass;

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, sv, mptveh))
			{
				sv = GC->rtcc->StateVectorCalc(vessel);
			}
		}
		else
		{
			sv = GC->rtcc->StateVectorCalc(vessel);
		}

		opt.EarliestGET = DOIGET;
		opt.GETbase = GC->GETbase;
		opt.lat = GC->LSLat;
		opt.lng = GC->LSLng;
		opt.alt = GC->LSAlt;
		opt.N = GC->DOI_N;
		opt.PeriAng = GC->DOI_PeriAng;
		opt.opt = GC->DOI_option;
		opt.sv0 = sv;
		opt.PeriAlt = GC->DOI_alt;

		if (vesseltype == 0 || vesseltype == 2)
		{
			attachedMass = 0.0;
		}
		else
		{
			attachedMass = GC->rtcc->GetDockedVesselMass(vessel);
		}

		GC->rtcc->DOITargeting(&opt, DOI_DV_imp, DOI_TIG_imp, DOI_t_PDI, GC->t_Land, DOI_CR);
		GC->rtcc->PoweredFlightProcessor(sv, GC->GETbase, DOI_TIG_imp, poweredenginetype, attachedMass, DOI_DV_imp, false, DOI_TIG, DOI_dV_LVLH, sv_pre, sv_post);

		P30TIG = DOI_TIG;
		dV_LVLH = DOI_dV_LVLH;

		if (GC->MissionPlanningActive)
		{
			char code[64];

			sprintf(code, "DOI");

			GC->rtcc->MPTAddManeuver(GC->mptable, sv_pre, sv_post, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
		}

		Result = 0;
	}
	break;
	case 11: //TEI Targeting
	{
		RTEMoonOpt opt;
		EntryResults res;

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, opt.RV_MCC, mptveh))
			{
				opt.RV_MCC = GC->rtcc->StateVectorCalc(vessel);
			}
		}
		else
		{
			opt.RV_MCC = GC->rtcc->StateVectorCalc(vessel);
		}

		entryprecision = 1;

		if (vesseltype == 0 || vesseltype == 2)
		{
			opt.csmlmdocked = false;
		}
		else
		{
			opt.csmlmdocked = true;
		}
		
		if (entrylongmanual)
		{
			opt.EntryLng = EntryLng;
		}
		else
		{
			opt.EntryLng = (double)landingzone;
		}
		if (RTECalcMode == 1)
		{
			opt.SMODE = 34;
		}
		else if (RTECalcMode == 2)
		{
			opt.SMODE = 14;
		}
		else if (RTECalcMode == 3)
		{
			opt.SMODE = 36;
		}
		else
		{
			opt.SMODE = 16;
		}

		opt.GETbase = GC->GETbase;
		opt.RevsTillTEI = 0;
		opt.vessel = vessel;
		opt.entrylongmanual = entrylongmanual;
		opt.TIGguess = EntryTIG;
		opt.Inclination = EntryDesiredInclination;
		opt.IRMAX = GC->RTEMaxReturnInclination;
		opt.r_rbias = GC->RTERangeOverrideNM;
		opt.u_rmax = RTEMaxReentrySpeed;
		opt.t_zmin = RTEReentryTime;

		GC->rtcc->RTEMoonTargeting(&opt, &res);
		TLCCSolGood = res.solutionfound;

		if (TLCCSolGood == false)
		{

		}
		else
		{
			Entry_DV = res.dV_LVLH;
			EntryTIGcor = res.P30TIG;
			EntryLatcor = res.latitude;
			EntryLngcor = res.longitude;
			P37GET400K = res.GET05G;
			EntryRTGO = res.RTGO;
			EntryAngcor = res.ReA;
			P30TIG = EntryTIGcor;
			dV_LVLH = Entry_DV;
			entryprecision = res.precision;
			RTEReturnInclination = res.Incl;
			FlybyPeriAlt = res.FlybyAlt;

			if (GC->MissionPlanningActive)
			{
				char code[64];

				sprintf(code, "RTE");

				GC->rtcc->MPTAddManeuver(GC->mptable, res.sv_preburn, res.sv_postburn, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
			}
		}
		
		Result = 0;
	}
	break;
	case 12: //Skylab Rendezvous Targeting
	{
		SkyRendOpt opt;
		SkylabRendezvousResults res;

		opt.E_L = Skylab_E_L;
		opt.GETbase = GC->GETbase;
		opt.man = Skylabmaneuver;
		opt.DH1 = SkylabDH1;
		opt.DH2 = SkylabDH2;
		opt.n_C = Skylab_n_C;
		opt.target = target;
		opt.t_TPI = t_TPI;
		opt.PCManeuver = Skylab_PCManeuver;
		opt.NPCOption = Skylab_NPCOption;
		opt.vessel = vessel;

		if (Skylabmaneuver == 0)
		{
			opt.TPIGuess = SkylabTPIGuess;
		}
		else if (Skylabmaneuver == 1)
		{
			opt.t_C = Skylab_t_NC1;
		}
		else if (Skylabmaneuver == 2)
		{
			opt.t_C = Skylab_t_NC2;
		}
		else if (Skylabmaneuver == 3)
		{
			opt.t_C = Skylab_t_NCC;
		}
		else if (Skylabmaneuver == 4)
		{
			opt.t_C = Skylab_t_NSR;
		}
		else if (Skylabmaneuver == 5)
		{
			opt.t_C = t_TPI;
		}
		else if (Skylabmaneuver == 6)
		{
			opt.t_C = t_TPI + Skylab_dt_TPM;
		}
		else if (Skylabmaneuver == 7)
		{
			if (Skylab_PCManeuver == 0)
			{
				opt.t_NC = Skylab_t_NC1;
			}
			else
			{
				opt.t_NC = Skylab_t_NC2;
			}
		}

		SkylabSolGood = GC->rtcc->SkylabRendezvous(&opt, &res);

		if (SkylabSolGood)
		{
			if (Skylabmaneuver == 0)
			{
				t_TPI = res.t_TPI;
			}
			else
			{
				dV_LVLH = res.dV_LVLH;
				P30TIG = res.P30TIG;
			}
			
			if (Skylabmaneuver == 1)
			{
				Skylab_dH_NC2 = res.dH_NC2;
				Skylab_dv_NC2 = res.dv_NC2;
				Skylab_dv_NCC = res.dv_NCC;
				Skylab_dV_NSR = res.dV_NSR;
				Skylab_t_NC2 = res.t_NC2;
				Skylab_t_NCC = res.t_NCC;
				Skylab_t_NSR = res.t_NSR;
			}
			else if (Skylabmaneuver == 2)
			{
				Skylab_dv_NCC = res.dv_NCC;
				Skylab_dV_NSR = res.dV_NSR;
				Skylab_t_NCC = res.t_NCC;
				Skylab_t_NSR = res.t_NSR;
			}
			else if (Skylabmaneuver == 3)
			{
				Skylab_dV_NSR = res.dV_NSR;
				Skylab_t_NSR = res.t_NSR;
			}
		}

		Result = 0;
	}
	break;
	case 13:	//PC Targeting
	{
		PCMan opt;
		SV sv_pre, sv_post;

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, TLCC_GET, GC->GETbase, opt.RV_MCC, mptveh))
			{
				opt.RV_MCC = GC->rtcc->StateVectorCalc(vessel);
			}
		}
		else
		{
			opt.RV_MCC = GC->rtcc->StateVectorCalc(vessel);
		}

		if (vesseltype == 0 || vesseltype == 2)
		{
			opt.csmlmdocked = false;
		}
		else
		{
			opt.csmlmdocked = true;
		}
		opt.EarliestGET = PCEarliestGET;
		opt.GETbase = GC->GETbase;
		opt.lat = GC->LSLat;
		opt.lng = GC->LSLng;
		opt.alt = GC->LSAlt;
		opt.vessel = vessel;
		opt.target = target;
		opt.landed = PClanded;
		opt.t_A = PCAlignGET;

		GC->rtcc->PlaneChangeTargeting(&opt, PC_dV_LVLH, PC_TIG, sv_pre, sv_post);

		P30TIG = PC_TIG;
		dV_LVLH = PC_dV_LVLH;

		if (GC->MissionPlanningActive)
		{
			char code[64];
			sprintf(code, "PC");
			GC->rtcc->MPTAddManeuver(GC->mptable, sv_pre, sv_post, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
		}

		Result = 0;
	}
	break;
	case 14: //TLI/MCC Targeting
	{
		if (TLCCmaneuver == 0 || TLCCmaneuver == 1)
		{
			SV sv0 = GC->rtcc->StateVectorCalc(vessel);

			if (TLCCmaneuver == 0)
			{
				TLIManNode opt;
				double MJDcut;

				opt.GETbase = GC->GETbase;
				opt.h_peri = GC->TLCCNodeAlt;
				opt.lat = GC->TLCCNodeLat;
				opt.lng = GC->TLCCNodeLng;
				opt.TLI_TIG = TLCC_GET;
				opt.PeriGET = GC->TLCCNodeGET;
				opt.RV_MCC = sv0;
				opt.vessel = vessel;

				GC->rtcc->TranslunarInjectionProcessorNodal(&opt, TLCC_dV_LVLH, TLCC_TIG, R_TLI, V_TLI, MJDcut);
				P30TIG = TLCC_TIG;
				dV_LVLH = TLCC_dV_LVLH;
			}
			else if (TLCCmaneuver == 1)
			{
				TLIManFR opt;
				TLMCCResults res;
				double MJDcut;

				opt.GETbase = GC->GETbase;
				opt.h_peri = GC->TLCCFlybyPeriAlt;
				opt.lat = GC->TLCCFreeReturnEMPLat;
				opt.TLI_TIG = TLCC_GET;
				opt.PeriGET = GC->TLCCPeriGET;
				opt.RV_MCC = sv0;
				opt.vessel = vessel;

				GC->rtcc->TranslunarInjectionProcessorFreeReturn(&opt, &res, R_TLI, V_TLI, MJDcut);

				TLCC_dV_LVLH = res.dV_LVLH;
				TLCC_TIG = res.P30TIG;
				TLCCPeriGETcor = res.PericynthionGET;
				TLCCReentryGET = res.EntryInterfaceGET;
				TLCCFRIncl = res.FRInclination;
				P30TIG = TLCC_TIG;
				dV_LVLH = TLCC_dV_LVLH;
				TLCCFRLat = EntryLatcor = res.SplashdownLat;
				TLCCFRLng = EntryLngcor = res.SplashdownLng;
			}
		}
		else
		{
			TLMCCResults res;
			SV sv0;

			if (GC->MissionPlanningActive)
			{
				if (!GC->rtcc->MPTTrajectory(GC->mptable, TLCC_GET, GC->GETbase, sv0, mptveh))
				{
					sv0 = GC->rtcc->StateVectorCalc(vessel);
				}
			}
			else
			{
				sv0 = GC->rtcc->StateVectorCalc(vessel);
			}

			if (TLCCmaneuver == 2)
			{
				MCCNodeMan opt;

				if (vesseltype == 0 || vesseltype == 2)
				{
					opt.csmlmdocked = false;
				}
				else
				{
					opt.csmlmdocked = true;
				}

				opt.GETbase = GC->GETbase;
				opt.h_node = GC->TLCCNodeAlt;
				opt.lat = GC->TLCCNodeLat;
				opt.lng = GC->TLCCNodeLng;
				opt.MCCGET = TLCC_GET;
				opt.NodeGET = GC->TLCCNodeGET;
				opt.RV_MCC = sv0;
				opt.vessel = vessel;
				opt.enginetype = poweredenginetype;

				GC->rtcc->TranslunarMidcourseCorrectionTargetingNodal(opt, res);
				TLCC_TIG = res.P30TIG;
				TLCC_dV_LVLH = res.dV_LVLH;
				P30TIG = TLCC_TIG;
				dV_LVLH = TLCC_dV_LVLH;

				if (GC->MissionPlanningActive)
				{
					char code[64];
					sprintf(code, "TLM");
					GC->rtcc->MPTAddManeuver(GC->mptable, res.sv_pre, res.sv_post, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
				}
			}
			else if (TLCCmaneuver == 3 || TLCCmaneuver == 4)
			{
				MCCFRMan opt;

				if (TLCCmaneuver == 3)
				{
					opt.type = 0;
				}
				else
				{
					opt.type = 1;
				}

				opt.GETbase = GC->GETbase;
				opt.lat = GC->TLCCFreeReturnEMPLat;
				opt.PeriGET = GC->TLCCPeriGET;
				opt.MCCGET = TLCC_GET;
				opt.vessel = vessel;
				opt.enginetype = poweredenginetype;

				if (vesseltype == 0 || vesseltype == 2)
				{
					opt.csmlmdocked = false;
				}
				else
				{
					opt.csmlmdocked = true;
				}

				opt.LOIh_apo = GC->LOIapo;
				opt.LOIh_peri = GC->LOIperi;
				opt.LSlat = GC->LSLat;
				opt.LSlng = GC->LSLng;
				opt.alt = GC->LSAlt;
				opt.t_land = GC->t_Land;
				opt.azi = GC->LOIazi;
				opt.h_peri = GC->TLCCLAHPeriAlt;
				opt.RV_MCC = sv0;

				TLCCSolGood = GC->rtcc->TranslunarMidcourseCorrectionTargetingFreeReturn(&opt, &res);

				if (TLCCSolGood)
				{
					TLCC_dV_LVLH = res.dV_LVLH;
					TLCC_TIG = res.P30TIG;
					TLCCPeriGETcor = res.PericynthionGET;
					TLCCReentryGET = res.EntryInterfaceGET;
					GC->TLCCNodeLat = res.NodeLat;
					GC->TLCCNodeLng = res.NodeLng;
					GC->TLCCNodeAlt = res.NodeAlt;
					GC->TLCCNodeGET = res.NodeGET;
					TLCCFRIncl = res.FRInclination;
					TLCCEMPLatcor = res.EMPLatitude;
					TLCCFRLat = EntryLatcor = res.SplashdownLat;
					TLCCFRLng = EntryLngcor = res.SplashdownLng;

					P30TIG = TLCC_TIG;
					dV_LVLH = TLCC_dV_LVLH;

					if (GC->MissionPlanningActive)
					{
						char code[64];
						sprintf(code, "TLM");
						GC->rtcc->MPTAddManeuver(GC->mptable, res.sv_pre, res.sv_post, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
					}
				}
			}
			else if (TLCCmaneuver == 5 || TLCCmaneuver == 6)
			{
				MCCNFRMan opt;

				if (TLCCmaneuver == 5)
				{
					opt.type = 0;
				}
				else
				{
					opt.type = 1;
				}

				opt.GETbase = GC->GETbase;
				opt.lat = GC->TLCCNonFreeReturnEMPLat;
				opt.PeriGET = GC->TLCCPeriGET;
				opt.MCCGET = TLCC_GET;
				opt.vessel = vessel;
				opt.enginetype = poweredenginetype;

				if (vesseltype == 0 || vesseltype == 2)
				{
					opt.csmlmdocked = false;
				}
				else
				{
					opt.csmlmdocked = true;
				}

				opt.LOIh_apo = GC->LOIapo;
				opt.LOIh_peri = GC->LOIperi;
				opt.LSlat = GC->LSLat;
				opt.LSlng = GC->LSLng;
				opt.alt = GC->LSAlt;
				opt.t_land = GC->t_Land;
				opt.azi = GC->LOIazi;
				opt.h_peri = GC->TLCCLAHPeriAlt;
				opt.N = GC->DOI_N;
				opt.DOIType = GC->DOI_option;
				opt.DOIPeriAng = GC->DOI_PeriAng;
				opt.LOIEllipseRotation = GC->LOIEllipseRotation;
				opt.DOIPeriAlt = GC->DOI_alt;
				opt.RV_MCC = sv0;

				TLCCSolGood = GC->rtcc->TranslunarMidcourseCorrectionTargetingNonFreeReturn(&opt, &res);

				if (TLCCSolGood)
				{
					TLCC_dV_LVLH = res.dV_LVLH;
					TLCC_TIG = res.P30TIG;
					GC->TLCCNodeLat = res.NodeLat;
					GC->TLCCNodeLng = res.NodeLng;
					GC->TLCCNodeAlt = res.NodeAlt;
					GC->TLCCNodeGET = res.NodeGET;
					TLCCEMPLatcor = res.EMPLatitude;
					TLCCRev2MeridianGET = res.t_Rev2Meridian;
					LOI_dV_LVLH = res.dV_LVLH_LOI;

					if (GC->DOI_option == 1)
					{
						DOI_dV_LVLH = res.dV_LVLH_DOI;
						TLCCPostDOIApoAlt = res.h_apo_postDOI;
						TLCCPostDOIPeriAlt = res.h_peri_postDOI;
					}

					P30TIG = TLCC_TIG;
					dV_LVLH = TLCC_dV_LVLH;

					if (GC->MissionPlanningActive)
					{
						char code[64];
						sprintf(code, "TLM");
						GC->rtcc->MPTAddManeuver(GC->mptable, res.sv_pre, res.sv_post, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
					}
				}
			}
			else if (TLCCmaneuver == 7)
			{
				MCCFlybyMan opt;

				opt.GETbase = GC->GETbase;
				opt.lat = GC->TLCCFreeReturnEMPLat;
				opt.PeriGET = GC->TLCCPeriGET;
				opt.MCCGET = TLCC_GET;
				opt.vessel = vessel;
				opt.h_peri = GC->TLCCFlybyPeriAlt;
				opt.RV_MCC = sv0;
				opt.enginetype = poweredenginetype;

				if (vesseltype == 0 || vesseltype == 2)
				{
					opt.csmlmdocked = false;
				}
				else
				{
					opt.csmlmdocked = true;
				}

				TLCCSolGood = GC->rtcc->TranslunarMidcourseCorrectionTargetingFlyby(&opt, &res);

				if (TLCCSolGood)
				{
					TLCC_dV_LVLH = res.dV_LVLH;
					TLCC_TIG = res.P30TIG;
					TLCCPeriGETcor = res.PericynthionGET;
					TLCCReentryGET = res.EntryInterfaceGET;
					TLCCFRIncl = res.FRInclination;
					TLCCFRLat = EntryLatcor = res.SplashdownLat;
					TLCCFRLng = EntryLngcor = res.SplashdownLng;

					P30TIG = TLCC_TIG;
					dV_LVLH = TLCC_dV_LVLH;

					if (GC->MissionPlanningActive)
					{
						char code[64];
						sprintf(code, "TLM");
						GC->rtcc->MPTAddManeuver(GC->mptable, res.sv_pre, res.sv_post, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
					}
				}
			}
			else if (TLCCmaneuver == 8)
			{
				MCCSPSLunarFlybyMan opt;

				opt.GETbase = GC->GETbase;
				opt.lat = GC->TLCCFreeReturnEMPLat;
				opt.PeriGET = GC->TLCCPeriGET;
				opt.MCCGET = TLCC_GET;
				opt.vessel = vessel;
				opt.h_peri = GC->TLCCFlybyPeriAlt;
				opt.AscendingNode = TLCCAscendingNode;
				opt.FRInclination = TLCCFRDesiredInclination;
				opt.RV_MCC = sv0;
				opt.enginetype = poweredenginetype;

				if (vesseltype == 0 || vesseltype == 2)
				{
					opt.csmlmdocked = false;
				}
				else
				{
					opt.csmlmdocked = true;
				}

				TLCCSolGood = GC->rtcc->TranslunarMidcourseCorrectionTargetingSPSLunarFlyby(&opt, &res, TLCCIterationStep);

				if (TLCCSolGood)
				{
					TLCC_dV_LVLH = res.dV_LVLH;
					TLCC_TIG = res.P30TIG;
					TLCCPeriGETcor = res.PericynthionGET;
					TLCCReentryGET = res.EntryInterfaceGET;
					TLCCFRIncl = res.FRInclination;
					TLCCFRLat = EntryLatcor = res.SplashdownLat;
					TLCCFRLng = EntryLngcor = res.SplashdownLng;
					TLCCEMPLatcor = res.EMPLatitude;

					P30TIG = TLCC_TIG;
					dV_LVLH = TLCC_dV_LVLH;

					if (GC->MissionPlanningActive)
					{
						char code[64];
						sprintf(code, "TLM");
						GC->rtcc->MPTAddManeuver(GC->mptable, res.sv_pre, res.sv_post, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
					}
				}
			}
		}
		Result = 0;
	}
	break;
	case 15:	//Lunar Liftoff Time Prediction
	{
		LunarLiftoffTimeOpt opt;
		SV sv_CSM;

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, sv_CSM, mptotherveh))
			{
				sv_CSM = GC->rtcc->StateVectorCalc(target);
			}
		}
		else
		{
			sv_CSM = GC->rtcc->StateVectorCalc(target);
		}

		opt.GETbase = GC->GETbase;
		opt.opt = LunarLiftoffTimeOption;
		opt.t_hole = t_Liftoff_guess;
		opt.dt_2 = DT_Ins_TPI;
		opt.sv_CSM = sv_CSM;
		opt.dt_1 = LAP_DT;
		opt.theta_1 = LAP_Theta;
		opt.IsInsVelInput = LunarLiftoffInsVelInput;
		opt.v_LH = LunarLiftoffRes.v_LH;
		opt.v_LV = LunarLiftoffRes.v_LV;

		if (vessel->GroundContact())
		{
			double lng, lat, rad;
			vessel->GetEquPos(lng, lat, rad);

			opt.alt = rad - oapiGetSize(oapiGetObjectByName("Moon"));
			opt.lat = lat;
			opt.lng = lng;

		}
		else
		{
			opt.alt = GC->LSAlt;
			opt.lat = GC->LSLat;
			opt.lng = GC->LSLng;
		}

		GC->rtcc->LaunchTimePredictionProcessor(opt, LunarLiftoffRes);
		t_TPI = LunarLiftoffRes.t_TPI;

		Result = 0;
	}
	break;
	case 16: //PDI PAD
	{
		PDIPADOpt opt;
		AP11PDIPAD temppdipad;

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, opt.sv0, mptveh))
			{
				opt.sv0 = GC->rtcc->StateVectorCalc(vessel);
			}
		}
		else
		{
			opt.sv0 = GC->rtcc->StateVectorCalc(vessel);
		}

		double rad = oapiGetSize(oapiGetObjectByName("Moon"));

		opt.direct = true;
		opt.GETbase = GC->GETbase;
		opt.HeadsUp = HeadsUp;
		opt.REFSMMAT = REFSMMAT;
		opt.R_LS = OrbMech::r_from_latlong(GC->LSLat, GC->LSLng, GC->LSAlt + rad);
		opt.t_land = GC->t_Land;
		opt.vessel = vessel;

		PADSolGood = GC->rtcc->PDI_PAD(&opt, temppdipad);

		if (PADSolGood)
		{
			pdipad = temppdipad;
		}

		Result = 0;
	}
	break;
	case 17: //Deorbit Maneuver
	{
		EntryResults res;
		EarthEntryOpt opt;

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, opt.RV_MCC, mptveh))
			{
				opt.RV_MCC = GC->rtcc->StateVectorCalc(vessel);
			}
		}
		else
		{
			opt.RV_MCC = GC->rtcc->StateVectorCalc(vessel);
		}

		if (entrylongmanual)
		{
			opt.lng = EntryLng;
		}
		else
		{
			opt.lng = (double)landingzone;
		}
		
		opt.GETbase = GC->GETbase;
		opt.enginetype = poweredenginetype;
		opt.entrylongmanual = entrylongmanual;
		opt.ReA = EntryAng;
		opt.TIGguess = EntryTIG;
		opt.vessel = vessel;
		opt.nominal = entrynominal;
		opt.useSV = true;

		GC->rtcc->BlockDataProcessor(&opt, &res);

		Entry_DV = res.dV_LVLH;
		EntryTIGcor = res.P30TIG;
		EntryLatcor = res.latitude;
		EntryLngcor = res.longitude;
		P37GET400K = res.GET05G;
		EntryRTGO = res.RTGO;
		EntryAngcor = res.ReA;
		P30TIG = EntryTIGcor;
		dV_LVLH = Entry_DV;
		entryprecision = res.precision;

		Result = 0;
	}
	break;
	case 19: //Docking Initiation Processor
	{
		DKIOpt opt;
		SV sv_A, sv_P, sv_pre, sv_post;

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, sv_A, mptveh))
			{
				sv_A = GC->rtcc->StateVectorCalc(vessel);
			}
			if (!GC->rtcc->MPTTrajectory(GC->mptable, sv_P, mptotherveh))
			{
				sv_P = GC->rtcc->StateVectorCalc(target);
			}
		}
		else
		{
			sv_A = GC->rtcc->StateVectorCalc(vessel);
			sv_P = GC->rtcc->StateVectorCalc(target);
		}		

		opt.DH = DH;
		opt.E = lambertelev;
		opt.GETbase = GC->GETbase;
		opt.maneuverline = DKI_Maneuver_Line;
		opt.N_HC = DKI_N_HC;
		opt.N_PB = DKI_N_PB;
		opt.plan = DKI_Profile;
		opt.radial_dv = DKI_Radial_DV;
		opt.tpimode = DKI_TPI_Mode;
		opt.dt_TPI_sunrise = DKI_dt_TPI_sunrise;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.t_TIG = DKI_TIG;
		opt.t_TPI_guess = t_TPIguess;
		opt.DeltaT_BHAM = DKI_dt_BHAM;
		opt.DeltaT_PBH = DKI_dt_PBH;
		opt.Delta_HAMH = DKI_dt_HAMH;

		GC->rtcc->DockingInitiationProcessor(opt, dkiresult);

		if (DKI_Profile != 3)
		{
			DKI_DV = dkiresult.DV_Phasing;
			DKI_SV = sv_A;
		}

		t_TPI = dkiresult.t_TPI;

		Result = 0;
	}
	break;
	case 20: //Lunar Ascent Processor
	{
		SV sv_CSM, sv_Ins;
		VECTOR3 R_LS;
		double theta, dt, m0;
		double rad = oapiGetSize(oapiGetObjectByName("Moon"));

		sv_CSM = GC->rtcc->StateVectorCalc(target);
		LEM *l = (LEM *)vessel;
		m0 = l->GetAscentStageMass();
		R_LS = OrbMech::r_from_latlong(GC->LSLat, GC->LSLng, GC->LSAlt + rad);

		GC->rtcc->LunarAscentProcessor(R_LS, m0, sv_CSM, GC->GETbase, LunarLiftoffRes.t_L, LunarLiftoffRes.v_LH, LunarLiftoffRes.v_LV, theta, dt, sv_Ins);

		LAP_Theta = theta;
		LAP_DT = dt;
		LAP_SV_Insertion = sv_Ins;

		Result = 0;
	}
	break;
	case 21: //LM Ascent PAD
	{
		ASCPADOpt opt;
		SV sv_CSM;
		MATRIX3 Rot, Rot2;

		sv_CSM = GC->rtcc->StateVectorCalc(target);
		vessel->GetRotationMatrix(Rot);
		oapiGetRotationMatrix(sv_CSM.gravref, &Rot2);

		opt.GETbase = GC->GETbase;
		opt.Rot_VL = OrbMech::GetVesselToLocalRotMatrix(Rot, Rot2);
		opt.R_LS = GC->rtcc->RLS_from_latlng(GC->LSLat, GC->LSLng, GC->LSAlt);
		opt.sv_CSM = sv_CSM;
		opt.TIG = LunarLiftoffRes.t_L;
		opt.v_LH = LunarLiftoffRes.v_LH;
		opt.v_LV = LunarLiftoffRes.v_LV;

		GC->rtcc->LunarAscentPAD(opt, lmascentpad);

		Result = 0;
	}
	break;
	case 22: //Powered Descent Abort Program
	{
		PDAPOpt opt;
		PDAPResults res;
		SV sv_LM;
		double m0;

		LEM *l = (LEM *)vessel;
		m0 = l->GetAscentStageMass();

		if (GC->MissionPlanningActive)
		{
			if (!GC->rtcc->MPTTrajectory(GC->mptable, sv_LM, mptveh))
			{
				sv_LM = GC->rtcc->StateVectorCalc(vessel);
			}
		}
		else
		{
			sv_LM = GC->rtcc->StateVectorCalc(vessel);
		}

		if (PDAPEngine == 0)
		{
			opt.dt_stage = 999999.9;
		}
		else
		{
			opt.dt_stage = 0.0;

		}

		opt.GETbase = GC->GETbase;
		opt.IsTwoSegment = GC->mission > 11;
		opt.REFSMMAT = REFSMMAT;
		opt.R_LS = GC->rtcc->RLS_from_latlng(GC->LSLat, GC->LSLng, GC->LSAlt);
		opt.sv_A = sv_LM;
		opt.sv_P = GC->rtcc->StateVectorCalc(target);
		opt.TLAND = GC->t_Land;
		opt.t_TPI = t_TPI;
		opt.W_TAPS = m0;
		opt.W_TDRY = opt.sv_A.mass - vessel->GetPropellantMass(vessel->GetPropellantHandleByIndex(0));
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

		Result = 0;
	}
	break;
	case 23: //FIDO Orbit Digitals Update
	{
		fidoorbitsv = GC->rtcc->StateVectorCalc(vessel);

		FIDOOrbitDigitalsOpt opt;

		opt.GETbase = GC->GETbase;
		opt.sv_A = fidoorbitsv;

		GC->rtcc->FIDOOrbitDigitalsUpdate(opt, fidoorbit);

		Result = 0;
	}
	break;
	case 24: //FIDO Orbit Digitals Cycle
	{
		double MJD = oapiGetSimMJD();

		FIDOOrbitDigitalsOpt opt;

		opt.GETbase = GC->GETbase;
		opt.MJD = MJD;
		opt.sv_A = fidoorbitsv;

		GC->rtcc->FIDOOrbitDigitalsCycle(opt, fidoorbit);

		Result = 0;
	}
	break;
	case 25: //FIDO Orbit Digitals Apsides Update
	{
		double MJD = oapiGetSimMJD();

		FIDOOrbitDigitalsOpt opt;

		opt.GETbase = GC->GETbase;
		opt.MJD = MJD;
		opt.sv_A = fidoorbitsv;

		GC->rtcc->FIDOOrbitDigitalsApsidesCycle(opt, fidoorbit);

		Result = 0;
	}
	break;
	case 26: //FIDO Orbit Digitals Longitude Calculation
	{
		FIDOOrbitDigitalsOpt opt;

		opt.GETbase = GC->GETbase;
		opt.sv_A = fidoorbitsv;

		GC->rtcc->FIDOOrbitDigitalsCalculateLongitude(opt, fidoorbit);

		Result = 0;
	}
	break;
	case 27: //FIDO Orbit Digitals GETL Calculation
	{
		double MJD = oapiGetSimMJD();

		FIDOOrbitDigitalsOpt opt;

		opt.GETbase = GC->GETbase;
		opt.MJD = MJD;
		opt.sv_A = fidoorbitsv;

		GC->rtcc->FIDOOrbitDigitalsCalculateGETL(opt, fidoorbit);

		Result = 0;
	}
	break;
	case 28: //FIDO Space Digitals Update
	{
		spacedigitalssv = GC->rtcc->StateVectorCalc(vessel);

		SpaceDigitalsOpt opt;

		opt.GETbase = GC->GETbase;
		opt.sv_A = spacedigitalssv;
		opt.LSAlt = GC->LSAlt;
		opt.LSAzi = GC->LOIazi;
		opt.LSLat = GC->LSLat;
		opt.LSLng = GC->LSLng;
		opt.t_land = GC->t_Land;

		GC->rtcc->FIDOSpaceDigitalsUpdate(opt, spacedigit);

		Result = 0;
	}
	break;
	case 29: //FIDO Space Digitals Cycle
	{
		double MJD = oapiGetSimMJD();

		SpaceDigitalsOpt opt;

		opt.GETbase = GC->GETbase;
		opt.LSAlt = GC->LSAlt;
		opt.MJD = MJD;
		opt.sv_A = spacedigitalssv;

		GC->rtcc->FIDOSpaceDigitalsCycle(opt, spacedigit);

		Result = 0;
	}
	break;
	case 30: //FIDO Space Digitals GET
	{
		double MJD = OrbMech::MJDfromGET(spacedigit.GETVector1, GC->GETbase);

		SpaceDigitalsOpt opt;

		opt.GETbase = GC->GETbase;
		opt.LSAlt = GC->LSAlt;
		opt.MJD = MJD;
		opt.sv_A = spacedigitalssv;

		GC->rtcc->FIDOSpaceDigitalsGET(opt, spacedigit);

		Result = 0;
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

			opt.dV_LVLH = Entry_DV;
			opt.GETbase = GC->GETbase;
			opt.P30TIG = EntryTIGcor;
			opt.REFSMMAT = REFSMMAT;
			opt.sv0 = GC->rtcc->StateVectorCalc(vessel);

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

			opt.dV_LVLH = dV_LVLH;
			opt.GETbase = GC->GETbase;

			if (EntryLatcor == 0)
			{
				//EntryPADLat = entry->EntryLatPred;
				//EntryPADLng = entry->EntryLngPred;
			}
			else
			{
				if (GC->MissionPlanningActive)
				{
					if (!GC->rtcc->MPTTrajectory(GC->mptable, opt.sv0, mptveh))
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
				opt.P30TIG = P30TIG;
				opt.REFSMMAT = REFSMMAT;
				opt.vessel = vessel;

				GC->rtcc->LunarEntryPAD(&opt, lunarentrypad);
			}
		}

		Result = 0;
	}
	break;
	case 32: //Map Update
	{
		SV sv0;

		if (GC->MissionPlanningActive && GC->rtcc->MPTHasManeuvers(GC->mptable, mptveh))
		{
			if (mapUpdateGET <= 0.0)
			{
				GC->rtcc->MPTTrajectory(GC->mptable, sv0, mptveh);
			}
			else
			{
				GC->rtcc->MPTTrajectory(GC->mptable, mapUpdateGET, GC->GETbase, sv0, mptveh);
			}
		}
		else
		{
			sv0 = GC->rtcc->StateVectorCalc(vessel);
		}

		if (mappage == 0)
		{
			int gstat;
			double ttoGSAOS, ttoGSLOS;

			gstat = OrbMech::findNextAOS(sv0.R, sv0.V, sv0.MJD, sv0.gravref);

			OrbMech::groundstation(sv0.R, sv0.V, sv0.MJD, sv0.gravref, groundstations[gstat][0], groundstations[gstat][1], 1, ttoGSAOS);
			OrbMech::groundstation(sv0.R, sv0.V, sv0.MJD, sv0.gravref, groundstations[gstat][0], groundstations[gstat][1], 0, ttoGSLOS);
			GSAOSGET = (sv0.MJD - GC->GETbase)*24.0*3600.0 + ttoGSAOS;
			GSLOSGET = (sv0.MJD - GC->GETbase)*24.0*3600.0 + ttoGSLOS;
			mapgs = gstat;
		}
		else
		{
			GC->rtcc->LunarOrbitMapUpdate(sv0, GC->GETbase, mapupdate);
		}

		Result = 0;
	}
	break;
	case 33: //Landmark Tracking PAD
	{
		LMARKTRKPADOpt opt;
		SV sv0;

		if (GC->MissionPlanningActive && GC->rtcc->MPTHasManeuvers(GC->mptable, mptveh))
		{
			GC->rtcc->MPTTrajectory(GC->mptable, LmkTime, GC->GETbase, sv0, mptveh);
		}
		else
		{
			sv0 = GC->rtcc->StateVectorCalc(vessel);
		}

		opt.GETbase = GC->GETbase;
		opt.lat[0] = LmkLat;
		opt.LmkTime[0] = LmkTime;
		opt.lng[0] = LmkLng;
		opt.sv0 = sv0;
		opt.entries = 1;

		GC->rtcc->LandmarkTrackingPAD(&opt, landmarkpad);

		Result = 0;
	}
	break;
	case 34:	//FIDO Orbit Digitals Apogee/Perigee Request
	{
		FIDOOrbitDigitalsOpt opt;

		opt.GETbase = GC->GETbase;

		if (GC->MissionPlanningActive && GC->rtcc->MPTHasManeuvers(GC->mptable, mptveh))
		{
			GC->rtcc->MPTTrajectory(GC->mptable, fidoorbit.GETBV, GC->GETbase, opt.sv_A, mptveh);
		}
		else
		{
			opt.sv_A = fidoorbitsv;
		}		

		GC->rtcc->FIDOOrbitDigitalsCalculateGETBV(opt, fidoorbit);

		Result = 0;
	}
	break;
	case 35: //Orbit Station Contacts Display
	{
		OrbitStationContactsOpt opt;

		opt.GETbase = GC->GETbase;
		opt.sv_A = GC->rtcc->StateVectorCalc(vessel);
		opt.lunar = nextstatcont_lunar;

		GC->rtcc->OrbitStationContactsDisplay(opt, orbitstatconttable);

		Result = 0;
	}
	break;
	case 36: //Next Station Contacts Display
	{
		GC->rtcc->EMDSTAC(orbitstatconttable, nextstatconttable);

		nextstatconttable.GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->GETbase);

		Result = 0;
	}
	break;
	case 37: //Predicted Site Acquisition Display
	{
		PredictedSiteAcquisitionOpt opt;

		opt.dt = predsiteacq_DT;
		opt.GET = predsiteacq_GET;

		GC->rtcc->EMDPESAD(opt, orbitstatconttable, predsiteacqtable);

		Result = 0;
	}
	break;
	case 38: //Transfer Two-Impulse Solution to MPT
	{
		SV sv_pre, sv_post;

		GC->rtcc->PoweredFlightProcessor(TwoImpulse_SV, GC->GETbase, TwoImpulse_TIG, GC->rtcc->med_m72.Thruster, 0.0, LambertdeltaV, true, P30TIG, dV_LVLH, sv_pre, sv_post, GC->rtcc->med_m72.Attitude == 1);

		if (GC->MissionPlanningActive)
		{
			char code[64];

			if (twoimpulsemode == 0)
			{
				sprintf(code, "LAM");
			}
			else if (twoimpulsemode == 1)
			{
				sprintf(code, "NCC");
			}
			else
			{
				sprintf(code, "TPI");
			}

			GC->rtcc->MPTAddManeuver(GC->mptable, sv_pre, sv_post, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
		}

		Result = 0;
	}
	break;
	case 39: //Transfer SPQ to MPT
	{
		SV sv_pre, sv_post;

		GC->rtcc->PoweredFlightProcessor(SPQ_SV, GC->GETbase, SPQTIG, GC->rtcc->med_m70.Thruster, 0.0, SPQDeltaV, true, P30TIG, dV_LVLH, sv_pre, sv_post);

		if (GC->MissionPlanningActive)
		{
			char code[64];

			if (SPQMode == 0)
			{
				sprintf(code, "CSI");
			}
			else
			{
				sprintf(code, "CDH");
			}

			GC->rtcc->MPTAddManeuver(GC->mptable, sv_pre, sv_post, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
		}

		Result = 0;
	}
	break;
	case 40: //Transfer DKI to MPT
	{
		SV sv_pre, sv_post;

		GC->rtcc->PoweredFlightProcessor(DKI_SV, GC->GETbase, DKI_TIG, GC->rtcc->med_m70.Thruster, 0.0, DKI_DV, true, P30TIG, dV_LVLH, sv_pre, sv_post);

		if (GC->MissionPlanningActive)
		{
			char code[64];

			if (DKI_Profile == 0 || DKI_Profile == 1)
			{
				sprintf(code, "NC");
			}
			else
			{
				sprintf(code, "NH");
			}

			GC->rtcc->MPTAddManeuver(GC->mptable, sv_pre, sv_post, code, GC->LSAlt, length(dV_LVLH), mptveh, docked);
		}

		Result = 0;
	}
	break;
	case 41: //Direct Input to the MPT
	{
		if (GC->MissionPlanningActive)
		{
			SV sv_A, sv_tig;
			double attachedMass;

			if (GC->rtcc->MPTHasManeuvers(GC->mptable, mptveh))
			{
				GC->rtcc->MPTTrajectory(GC->mptable, GC->rtcc->med_m66.GETBI, GC->GETbase, sv_A, mptveh);
				sv_tig = sv_A;
			}
			else
			{
				sv_A = GC->rtcc->StateVectorCalc(vessel);
				sv_tig = GC->rtcc->GeneralTrajectoryPropagation(sv_A, 0, OrbMech::MJDfromGET(GC->rtcc->med_m66.GETBI, GC->GETbase));
			}

			if (vesseltype == 0 || vesseltype == 2)
			{
				attachedMass = 0.0;
			}
			else
			{
				attachedMass = GC->rtcc->GetDockedVesselMass(vessel);
			}

			if (vesseltype < 2)
			{
				GC->rtcc->med_m66.Table = 2;
			}
			else
			{
				GC->rtcc->med_m66.Table = 1;
			}

			GC->rtcc->MPTDirectInput(GC->mptable, sv_tig, GC->GETbase, GC->LSAlt, attachedMass);
		}
		Result = 0;
	}
	break;
	}

	subThreadStatus = Result;
	if (hThread != NULL) { CloseHandle(hThread); }

	return(0);
}

void ARCore::StartIMFDRequest() {

	g_Data.isRequesting = true;
	if (!g_Data.progVessel->GetIMFDClient()->IsBurnDataRequesting())
		g_Data.progVessel->GetIMFDClient()->StartBurnDataRequests();
}

void ARCore::StopIMFDRequest() {

	g_Data.isRequesting = false;
		g_Data.progVessel->GetIMFDClient()->StopBurnDataRequests();
}

int ARCore::REFSMMATOctalAddress()
{
	int addr;

	if (vesseltype < 2)
	{
		addr = 01735;
	}
	else
	{
		addr = 01733;
	}
	
	if (AGCEpoch > 40768.0)	//Luminary 210 and Artemis 072 both have the REFSMMAT two addresses earlier
	{
		addr -= 02;
	}
	return addr;
}

int ARCore::REFSMMATUplinkAddress()
{
	int addr;

	if (REFSMMATupl == 0)
	{
		if (vesseltype < 2)
		{
			addr = 306;
		}
		else
		{
			addr = 3606;
		}
	}
	else
	{
		if (vesseltype < 2)
		{
			addr = 1735;
		}
		else
		{
			addr = 1733;
		}

		if (GC->mission >= 14)	//Luminary 210 and Artemis 072 both have the REFSMMAT two addresses earlier
		{
			addr -= 2;
		}
	}

	return addr;
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

void ARCore::AGCCorrectionVectors(double mjd_launch, double t_land, int mission, bool isCMC)
{
	OBJHANDLE hEarth, hMoon;
	MATRIX3 R, Rot2, J2000, R2, R3, M, M_AGC;
	VECTOR3 l;
	double mjd_mid, brcsmjd, w_E, t0, B_0, Omega_I0, F_0, B_dot, Omega_I_dot, F_dot, cosI, sinI;
	double A_Z, A_Z0, A_X, minA_Y, mjd_land;
	int mem;
	char AGC[64];

	mjd_mid = mjd_launch + 7.0;
	mjd_land = mjd_launch + t_land / 24.0 / 3600.0;
	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	Rot2 = _M(1., 0., 0., 0., 0., 1., 0., 1., 0.);
	R = OrbMech::GetRotationMatrix(BODY_EARTH, mjd_mid);

	if (isCMC)
	{
		if (mission < 11)
		{
			brcsmjd = 40221.525;    //Nearest Besselian Year 1969
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
			brcsmjd = 40586.767239; //Nearest Besselian Year 1970
			w_E = 7.29211494e-5;    //Comanche 055 (Apollo 11 CM AGC)
			B_0 = 0.40916190299;
			Omega_I0 = 6.19653663041;
			F_0 = 5.20932947829;
			B_dot = -7.19757301e-14;
			Omega_I_dot = -1.07047011e-8;
			F_dot = 2.67240410e-6;
			cosI = 0.99964173;
			sinI = 0.02676579;
			t0 = 40403;
			sprintf(AGC, "Comanche055");
		}
		else if (mission < 15)
		{
			brcsmjd = 40952.009432; //Nearest Besselian Year 1971
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
			brcsmjd = 41317.251625; //Nearest Besselian Year 1972
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
			brcsmjd = 40221.525;    //Nearest Besselian Year 1969
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
			brcsmjd = 40586.767239;		//Nearest Besselian Year 1970
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
		else if (mission < 13)
		{
			brcsmjd = 40586.767239; //Nearest Besselian Year 1970
			w_E = 7.29211494e-5;    //Luminary 116 (Apollo 12 LM AGC)
			B_0 = 0.4091619030;
			Omega_I0 = 6.196536640;
			F_0 = 5.209327056;
			B_dot = -7.197573418e-14;
			Omega_I_dot = -1.070470170e-8;
			F_dot = 2.672404256e-6;
			cosI = 0.9996417320;
			sinI = 0.02676579050;
			t0 = 40403;
			sprintf(AGC, "Luminary116");
		}
		else if (mission < 14)
		{
			brcsmjd = 40586.767239;		//Nearest Besselian Year 1970
			w_E = 7.292115145489943e-05;//Luminary 131 (Apollo 13 LM AGC)
			B_0 = 0.4091619030;
			Omega_I0 = 6.196536640;
			F_0 = 5.209327056;
			B_dot = -7.197573418e-14;
			Omega_I_dot = -1.070470170e-8;
			F_dot = 2.672404256e-6;
			cosI = 0.9996417320;
			sinI = 0.02676579050;
			t0 = 40403;
			sprintf(AGC, "Luminary131");
		}
		else if (mission < 15)
		{
			brcsmjd = 40952.009432; //Nearest Besselian Year 1971
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
			brcsmjd = 41317.251625; //Nearest Besselian Year 1972
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
	J2000 = OrbMech::J2000EclToBRCS(brcsmjd);
	R2 = mul(OrbMech::tmat(Rot2), mul(R, Rot2));
	R3 = mul(J2000, R2);

	A_Z = atan2(R3.m21, R3.m11);
	if (mission < 14)
	{
		A_Z0 = fmod((A_Z - w_E * (mjd_mid - t0) * 24.0 * 3600.0), PI2);  //AZ0 for mission
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
	A_X = atan2(M_AGC.m32, M_AGC.m33);
	minA_Y = -atan2(-M_AGC.m31, sqrt(M_AGC.m32*M_AGC.m32 + M_AGC.m33*M_AGC.m33));
	l = _V(A_X, -minA_Y, 0.);

	mem = 01711;

	//TBD: Print stuff here
	FILE *file = fopen("PrecessionData.txt", "w");
	fprintf(file, "------- AGC Correction Vectors for Apollo %d using %s -------\n", mission, AGC);
	fprintf(file, "Epoch   = %6.6f (MJD) Epoch of Basic Reference Coordinate System\n", brcsmjd);
	fprintf(file, "TEphem0 = %6.6f (MJD) Ephemeris Time Zero\n", t0);
	fprintf(file, "TEPHEM  = %6.6f (MJD) Mission launch time\n", mjd_launch);
	fprintf(file, "MJDLAND = %6.6f (MJD) Mission lunar landing time\n", mjd_land);
	fprintf(file, "TIMEM0  = %6.6f (MJD) Mission mid-range time\n\n", mjd_mid);
	fprintf(file, "------- Earth Orientation -------\n");
	if (mission < 14)
	{
		fprintf(file, "AZO %.10f DEG\n", A_Z0*DEG);
	}
	fprintf(file, "-AYO %.10f DEG\n", minA_Y*DEG);
	fprintf(file, "AXO %.10f DEG\n", A_X*DEG);
	if (mission < 14)
	{
		fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(A_Z0 / PI2, 0, 1)); mem++;
		fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(A_Z0 / PI2, 0, 0)); mem++;
	}	
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(minA_Y / PI2, 0, 1)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(minA_Y / PI2, 0, 0)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(A_X / PI2, 0, 1)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(A_X / PI2, 0, 0)); mem++;

	//MOON ROTATIONS
	MATRIX3 M1, M2, M3, M4, MM, M_AGC_M, RM, R2M, R3M;
	VECTOR3 lm;
	double t_M, B, Omega_I, F;

	if (isCMC)
	{
		t_M = (mjd_mid - t0) * 24.0 * 3600.0;
		RM = OrbMech::GetRotationMatrix(BODY_MOON, mjd_mid);
	}
	else
	{
		t_M = (mjd_land - t0) * 24.0 * 3600.0;
		RM = OrbMech::GetRotationMatrix(BODY_MOON, mjd_land);
	}

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

int ARCore::CSMToEngineType()
{
	if (csmenginetype == 0)
	{
		return RTCC_ENGINETYPE_SPS;
	}
	else if (csmenginetype == 1)
	{
		return RTCC_ENGINETYPE_RCSPLUS2;
	}
	else if (csmenginetype == 2)
	{
		return RTCC_ENGINETYPE_RCSPLUS4;
	}
	else if (csmenginetype == 3)
	{
		return RTCC_ENGINETYPE_RCSMINUS2;
	}
	else
	{
		return RTCC_ENGINETYPE_RCSMINUS4;
	}
}

int ARCore::LEMToEngineType()
{
	if (lemenginetype == 0)
	{
		return RTCC_ENGINETYPE_APS;
	}
	else if (lemenginetype == 1)
	{
		return RTCC_ENGINETYPE_DPS;
	}
	else if (lemenginetype == 2)
	{
		return RTCC_ENGINETYPE_RCSPLUS2;
	}
	else if (lemenginetype == 3)
	{
		return RTCC_ENGINETYPE_RCSPLUS4;
	}
	else if (lemenginetype == 4)
	{
		return RTCC_ENGINETYPE_RCSMINUS2;
	}
	else
	{
		return RTCC_ENGINETYPE_RCSMINUS4;
	}
}