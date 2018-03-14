#include "ARCore.h"

#include "soundlib.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "saturn.h"
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

ARCore::ARCore(VESSEL* v)
{
	T1 = 0;
	T2 = 0;
	CDHtime = 0;
	CDHtime_cor = 0;
	CDHtimemode = 1;
	DH = 0;
	N = 0;
	this->vessel = v;
	lambertelev = 0.0;
	LambertdeltaV = _V(0, 0, 0);
	lambertopt = 0;
	CDHdeltaV = _V(0, 0, 0);
	target = NULL;
	offvec = _V(0, 0, 0);
	//screen = 0;
	angdeg = 0;
	targetnumber = -1;
	mission = 0;
	GETbase = LaunchMJD[0];
	AGCEpoch = 40221.525;
	REFSMMAT = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	REFSMMATTime = 0.0;
	REFSMMATopt = 4;
	REFSMMATcur = 4;
	REFSMMATupl = 0;
	LSLat = 0.0;
	LSLng = 0.0;
	LSAlt = 0.0;
	manpadopt = 0;
	vesseltype = 0;

	for (int i = 0; i < 20; i++)
	{
		REFSMMAToct[i] = 0;
	}

	if (strcmp(v->GetName(), "AS-205") == 0)
	{
		mission = 7;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.5217969*RAD, -80.5612465*RAD, LaunchMJD[mission - 7], 72.0*RAD);
	}
	else if (strcmp(v->GetName(), "AS-503") == 0)
	{
		mission = 8;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 72.124*RAD);
	}
	else if (strcmp(v->GetName(), "AS-504") == 0)
	{
		mission = 9;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 72.0*RAD);
	}
	else if (strcmp(v->GetName(), "Spider") == 0)
	{
		mission = 9;
		vesseltype = 2;
	}
	else if (strcmp(v->GetName(), "AS-505") == 0 || strcmp(v->GetName(), "Charlie Brown") == 0)
	{
		mission = 10;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.626530*RAD, -80.620629*RAD, LaunchMJD[mission - 7], 72.0*RAD);
	}
	else if (strcmp(v->GetName(), "Snoopy") == 0)
	{
		mission = 10;
		vesseltype = 2;
	}
	else if (strcmp(v->GetName(), "AS-506") == 0 || strcmp(v->GetName(), "Columbia") == 0)
	{
		mission = 11;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 72.0*RAD);
		AGCEpoch = 40586.767239;
	}
	else if (strcmp(v->GetName(), "Eagle") == 0)
	{
		mission = 11;
		vesseltype = 2;
		AGCEpoch = 40586.767239;
	}
	else if (strcmp(v->GetName(), "Yankee Clipper") == 0)
	{
		mission = 12;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 72.0*RAD);
		AGCEpoch = 40586.767239;
	}
	else if (strcmp(v->GetName(), "Intrepid") == 0)
	{
		mission = 12;
		vesseltype = 2;
		AGCEpoch = 40586.767239;
	}
	else if (strcmp(v->GetName(), "Odyssey") == 0)
	{
		mission = 13;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 72.0*RAD);
		AGCEpoch = 40586.767239;
	}
	else if (strcmp(v->GetName(), "Aquarius") == 0)
	{
		mission = 13;
		vesseltype = 2;
		AGCEpoch = 40586.767239;
	}
	else if (strcmp(v->GetName(), "Kitty Hawk") == 0)
	{
		mission = 14;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 72.067*RAD);
		AGCEpoch = 40952.009432;
	}
	else if (strcmp(v->GetName(), "Antares") == 0)
	{
		mission = 14;
		vesseltype = 2;
		AGCEpoch = 40952.009432;
	}
	else if (strcmp(v->GetName(), "Endeavour") == 0)
	{
		mission = 15;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 80.088*RAD);
		AGCEpoch = 41317.251625;
	}
	else if (strcmp(v->GetName(), "Falcon") == 0)
	{
		mission = 15;
		vesseltype = 2;
		AGCEpoch = 41317.251625;
	}
	else if (strcmp(v->GetName(), "Casper") == 0)
	{
		mission = 16;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 72.0*RAD);
		AGCEpoch = 41317.251625;
	}
	else if (strcmp(v->GetName(), "Orion") == 0)
	{
		mission = 16;
		vesseltype = 2;
		AGCEpoch = 41317.251625;
	}
	else if (strcmp(v->GetName(), "America") == 0)
	{
		mission = 17;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 72.0*RAD);
		AGCEpoch = 41317.251625;
	}
	else if (strcmp(v->GetName(), "Challenger") == 0)
	{
		mission = 17;
		vesseltype = 2;
		AGCEpoch = 41317.251625;
	}
	GETbase = LaunchMJD[mission - 7];

	if (vessel->DockingStatus(0) == 1)
	{
		vesseltype++;
	}

	MATRIX3 a;
	a = mul(REFSMMAT, OrbMech::transpose_matrix(OrbMech::J2000EclToBRCS(AGCEpoch)));

	REFSMMAToct[0] = 24;
	REFSMMAToct[1] = 306;
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

	REFSMMATdirect = true;
	REFSMMATHeadsUp = true;

	GMPType = 0;
	OrbAdjAltRef = true;
	OrbAdjDVX = _V(0, 0, 0);
	SPSGET = 0.0;
	apo_desnm = 0;
	peri_desnm = 0;
	incdeg = 0;
	GMPRotationAngle = 0.0;
	GMPLongitude = 0.0;
	GMPTOA = 0.0;
	GMPRevs = 0;

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
	returnspeed = 1;
	FlybyType = 0;
	entrynominal = 1;
	entryrange = 0.0;
	EntryRTGO = 0.0;
	DeorbitEngineOpt = 0;

	SVSlot = true; //true = CSM; false = Other
	J2000Pos = _V(0.0, 0.0, 0.0);
	J2000Vel = _V(0.0, 0.0, 0.0);
	J2000GET = 0.0;
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
	EntryPADdirect = false; //false = Entry PAD with MCC/Deorbit burn, true = Direct Entry
	ManPADSPS = 0;
	TPIPAD_AZ = 0.0;
	TPIPAD_dH = 0.0;
	TPIPAD_dV_LOS = _V(0.0, 0.0, 0.0);
	TPIPAD_ELmin5 = 0.0;
	TPIPAD_R = 0.0;
	TPIPAD_Rdot = 0.0;
	TPIPAD_ddH = 0.0;
	TPIPAD_BT = _V(0.0, 0.0, 0.0);
	sxtstardtime = 0.0;
	ManPADdirect = true;
	P37GET400K = 0.0;
	mapupdate.LOSGET = 0.0;
	mapupdate.AOSGET = 0.0;
	mapupdate.SSGET = 0.0;
	mapupdate.SRGET = 0.0;
	mapupdate.PMGET = 0.0;
	mappage = 1;
	mapgs = 0;
	GSAOSGET = 0.0;
	GSLOSGET = 0.0;
	inhibUplLOS = false;
	PADSolGood = true;
	svtarget = NULL;
	svtargetnumber = -1;
	svtimemode = 0;
	svmode = 0;
	lambertmultiaxis = 1;
	entrylongmanual = true;
	landingzone = 0;
	entryprecision = -1;

	rtcc = new RTCC();

	LOImaneuver = 0;
	LOIOption = 0;
	LOIapo = 170.0*1852.0;
	LOIperi = 60.0*1852.0;
	LOIazi = 0.0;
	LOI_dV_LVLH = _V(0.0, 0.0, 0.0);
	LOI_TIG = 0.0;
	LOI2Alt = 60.0*1852.0;

	TLCCmaneuver = 2;
	TLCC_GET = 0.0;
	TLCCNodeLat = 0.0;
	TLCCFreeReturnEMPLat = 0.0;
	TLCCNonFreeReturnEMPLat = 0.0;
	TLCCEMPLatcor = 0.0;
	TLCCNodeLng = 0.0;
	TLCC_dV_LVLH = _V(0.0, 0.0, 0.0);
	TLCCLAHPeriAlt = 60.0*1852.0;
	TLCCFlybyPeriAlt = 60.0*1852.0;
	TLCCNodeAlt = 0.0;
	TLCCNodeGET = 0.0;
	TLCCPeriGET = 0.0;
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
	landmarkpad.T1 = 0;
	landmarkpad.T2 = 0;
	landmarkpad.CRDist = 0;
	landmarkpad.Alt = 0;
	landmarkpad.Lat = 0;
	landmarkpad.Lng05 = 0;

	VECdirection = 0;
	VECbody = NULL;
	VECangles = _V(0, 0, 0);

	DOI_N = 0;
	DOI_dV_LVLH = _V(0, 0, 0);
	DOI_TIG = 0.0;
	DOI_t_PDI = 0.0;
	t_Land = 0.0;
	DOI_CR = 0.0;
	DOIGET = 0.0;
	DOI_PeriAng = 15.0*RAD;
	DOI_option = 0;

	AGSKFactor = 90.0*3600.0;

	if (mission == 8)
	{
		LSLat = 2.6317*RAD;
		LSLng = 34.0253*RAD;
		LSAlt = -0.82*1852.0;
		LOIazi = -78.0*RAD;
		TLCCFreeReturnEMPLat = TLCCNonFreeReturnEMPLat  = -5.67822*RAD;
		TLCCPeriGET = OrbMech::HHMMSSToSS(69.0, 9.0, 29.4);
		t_Land = OrbMech::HHMMSSToSS(82.0, 8.0, 26.0);
	}
	else if (mission == 9)
	{
		AGSKFactor = 40.0*3600.0;
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
	}
	else if (mission == 12)
	{
		LSLat = -2.9425*RAD;
		LSLng = -23.44333*RAD;
		LSAlt = -1.19*1852.0;
		LOIazi = -75.0*RAD;
		LOIapo = 168.9*1852.0;
		LOIperi = 58.7*1852.0;
		TLCCFreeReturnEMPLat = -1.962929*RAD;
		TLCCNonFreeReturnEMPLat = 3.800381*RAD;
		TLCCPeriGET = TLCCNodeGET = OrbMech::HHMMSSToSS(83.0, 32.0, 15.0);
		TLCCFlybyPeriAlt = 1851.7*1852.0;
		TLCCNodeLat = 1.23*RAD;
		TLCCNodeLng = 165.23*RAD;
		TLCCNodeAlt = 59.9*1852.0;
		TLCCLAHPeriAlt = TLCCNodeAlt;
		t_Land = OrbMech::HHMMSSToSS(110.0, 31.0, 19.0);
		AGSKFactor = 100.0*3600.0;
	}
	else if (mission == 13)
	{
		LSLat = -3.6686*RAD;
		LSLng = -17.4842*RAD;
		LSAlt = -0.76*1852.0;
		LOIazi = -93.9*RAD;
		LOIapo = 168.3*1852.0;
		LOIperi = 57.0*1852.0;		
		TLCCFreeReturnEMPLat = 0.108105*RAD;
		TLCCNonFreeReturnEMPLat = -0.203331*RAD;
		TLCCPeriGET = TLCCNodeGET = OrbMech::HHMMSSToSS(77.0, 29.0, 30.0);
		TLCCFlybyPeriAlt = 210*1852.0;
		TLCCNodeLat = 3.69*RAD;
		TLCCNodeLng = 176.76*RAD;
		TLCCNodeAlt = 58.0*1852.0;
		TLCCLAHPeriAlt = TLCCNodeAlt;
		t_Land = OrbMech::HHMMSSToSS(103.0, 42.0, 02.0);
	}
	else if (mission == 14)
	{
		LSLat = -3.672*RAD;
		LSLng = -17.463*RAD;
		LSAlt = -0.76*1852.0;
		LOIazi = -76.31*RAD;
		LOIapo = 170.0*1852.0;
		LOIperi = 57.1*1852.0;
		TLCCFreeReturnEMPLat = -0.048722*RAD;
		TLCCNonFreeReturnEMPLat = -3.672093*RAD;
		TLCCPeriGET = TLCCNodeGET = OrbMech::HHMMSSToSS(82.0, 41.0, 14.0);
		TLCCFlybyPeriAlt = 2030.9*1852.0;
		TLCCNodeLat = 3.15*RAD;
		TLCCNodeLng = 174.82*RAD;
		TLCCNodeAlt = 57.1*1852.0;
		TLCCLAHPeriAlt = TLCCNodeAlt;
		t_Land = OrbMech::HHMMSSToSS(108.0, 53.0, 32.6);
		AGSKFactor = 100.0*3600.0;
	}
	else if (mission == 15)
	{
		LSLat = 26.0739*RAD;
		LSLng = 3.6539*RAD;
		LSAlt = -1.92*1852.0;
		LOIazi = -91.0*RAD;
		LOIapo = 170.0*1852.0;
		LOIperi = 58.3*1852.0;
		TLCCFreeReturnEMPLat = TLCCNonFreeReturnEMPLat = -16.904842*RAD;
		TLCCPeriGET = TLCCNodeGET = OrbMech::HHMMSSToSS(78.0, 35.0, 00.5);
		TLCCFlybyPeriAlt = TLCCNodeAlt = 68.1*1852.0;
		TLCCNodeLat = -23.3*RAD;
		TLCCNodeLng = 171.7*RAD;
		TLCCLAHPeriAlt = TLCCNodeAlt;
		t_Land = OrbMech::HHMMSSToSS(104.0, 40.0, 57.0);
		AGSKFactor = 100.0*3600.0;
		DOI_PeriAng = 16.0*RAD;
	}
	else if (mission == 16)
	{
		LSLat = -9.00028*RAD;
		LSLng = 15.51639*RAD;
		LSAlt = -0.1405*1852.0;
		LOIazi = -91.0*RAD;
		LOIapo = 170.6*1852.0;
		LOIperi = 58.5*1852.0;
		TLCCFreeReturnEMPLat = TLCCNonFreeReturnEMPLat = 5.529042*RAD;
		TLCCPeriGET = TLCCNodeGET = OrbMech::HHMMSSToSS(74.0, 32.0, 13.4);
		TLCCFlybyPeriAlt = TLCCNodeAlt = 71.4*1852.0;
		TLCCNodeLat = 7.8*RAD;
		TLCCNodeLng = 176.8*RAD;
		TLCCLAHPeriAlt = TLCCNodeAlt;
		t_Land = OrbMech::HHMMSSToSS(98.0, 46.0, 42.4);
		DOI_PeriAng = 16.0*RAD;
	}
	else if (mission == 17)
	{
		LSLat = 20.164*RAD;
		LSLng = 30.750*RAD;
		LSAlt = -1.95*1852.0;
		LOIazi = -90.0*RAD;
		LOIapo = 170.8*1852.0;
		LOIperi = 51.4*1852.0;
		TLCCFreeReturnEMPLat = TLCCNonFreeReturnEMPLat = -12.695335*RAD;
		TLCCPeriGET = TLCCNodeGET = OrbMech::HHMMSSToSS(88.0, 59.0, 25.0);
		TLCCFlybyPeriAlt = TLCCNodeAlt = 51.3*1852.0;
		TLCCNodeLat = -10.68*RAD;
		TLCCNodeLng = 165.01*RAD;
		TLCCLAHPeriAlt = TLCCNodeAlt;
		t_Land = OrbMech::HHMMSSToSS(113.0, 01.0, 38.4);
		AGSKFactor = 110.0*3600.0;
		DOI_PeriAng = -10.0*RAD;
	}

	Skylabmaneuver = 0;
	SkylabTPIGuess = 0.0;
	Skylab_n_C = 0;
	SkylabDH1 = 0.0;
	SkylabDH2 = 0.0;
	Skylab_E_L = 0.0;
	SkylabSolGood = true;
	Skylab_dV_NSR = Skylab_dV_NCC = _V(0, 0, 0);
	Skylab_dH_NC2 = Skylab_dv_NC2 = Skylab_t_NC1 = Skylab_t_NC2 = Skylab_dv_NCC = Skylab_t_NCC = Skylab_t_NSR = Skylab_t_TPI = Skylab_dt_TPM = 0.0;
	Skylab_NPCOption = Skylab_PCManeuver = false;

	PCAlignGET = 0.0;
	PClanded = true;
	PC_dV_LVLH = _V(0, 0, 0);
	PC_TIG = 0;
	PCEarliestGET = 0;

	TMLat = 0.0;
	TMLng = 0.0;
	TMAzi = 0.0;
	TMDistance = 0.0;
	TMStepSize = 0.0;
	TMAlt = 0.0;

	LunarLiftoffTimeOption = 0;
	t_TPIguess = 0.0;
	LunarLiftoffTimes.t_CDH = 0.0;
	LunarLiftoffTimes.t_CSI = 0.0;
	LunarLiftoffTimes.t_Ins = 0.0;
	LunarLiftoffTimes.t_L = 0.0;
	LunarLiftoffTimes.t_TPI = 0.0;
	LunarLiftoffTimes.t_TPF = 0.0;
	LunarLiftoffTimes.v_LH = 0.0;
	LunarLiftoffTimes.v_LV = 0.0;

	EMPUplinkType = 0;
	EMPUplinkNumber = 0;

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
}

void ARCore::MinorCycle(double SimT, double SimDT, double mjd)
{
	if (g_Data.connStatus > 0 && g_Data.uplinkBuffer.size() > 0) {
		if (SimT > g_Data.uplinkBufferSimt + 0.05) {
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
			P30TIG = (g_Data.burnData.IgnMJD - GETbase)*24.0*3600.0;

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
	LMARKTRKPADOpt opt;

	opt.GETbase = GETbase;
	opt.lat = LmkLat;
	opt.LmkTime = LmkTime;
	opt.lng = LmkLng;
	opt.vessel = vessel;

	rtcc->LandmarkTrackingPAD(&opt, landmarkpad);
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

	sv0 = rtcc->StateVectorCalc(vessel);
	rtcc->EntryUpdateCalc(sv0, GETbase, entryrange, true, &res);

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

void ARCore::TEICalc()
{
	startSubthread(11);
}

void ARCore::RTEFlybyCalc()
{
	startSubthread(18);
}

void ARCore::CDHcalc()			//Calculates the required DV vector of a coelliptic burn
{
	startSubthread(2);
}

void ARCore::lambertcalc()
{
	startSubthread(1);
}

void ARCore::OrbitAdjustCalc()	//Calculates the optimal velocity change to reach an orbit specified by apoapsis, periapsis and inclination
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

void ARCore::EntryPAD()
{
	OBJHANDLE hEarth;
	double mu;

	hEarth = oapiGetObjectByName("Earth");
	mu = GGRAV*oapiGetMass(hEarth);

	if (entrypadopt == 0)
	{
		EarthEntryPADOpt opt;

		opt.dV_LVLH = Entry_DV;
		opt.GETbase = GETbase;
		opt.P30TIG = EntryTIGcor;
		opt.REFSMMAT = REFSMMAT;
		opt.vessel = vessel;

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
		OBJHANDLE gravref = rtcc->AGCGravityRef(vessel);
		vessel->GetRelativePos(gravref, R);
		vessel->GetRelativeVel(gravref, V);
		OrbMech::periapo(R, V, mu, apo, peri);
		if (peri < oapiGetSize(gravref) + 50 * 1852.0)
		{
			opt.preburn = false;
			rtcc->EarthOrbitEntry(&opt, earthentrypad);
		}
		else
		{
			opt.preburn = true;
			rtcc->EarthOrbitEntry(&opt, earthentrypad);
		}


		return;
	}
	else
	{
		LunarEntryPADOpt opt;

		opt.direct = EntryPADdirect;
		opt.dV_LVLH = dV_LVLH;
		opt.GETbase = GETbase;
		
		if (EntryLatcor == 0)
		{
			//EntryPADLat = entry->EntryLatPred;
			//EntryPADLng = entry->EntryLngPred;
		}
		else
		{
			//EntryPADLat = EntryLatcor;
			//EntryPADLng = EntryLngcor;
			opt.lat = EntryLatcor;
			opt.lng = EntryLngcor;
			opt.P30TIG = P30TIG;
			opt.REFSMMAT = REFSMMAT;
			opt.vessel = vessel;

			rtcc->LunarEntryPAD(&opt, lunarentrypad);
		}
	}
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
	SV sv0 = rtcc->StateVectorCalc(vessel);

	if (mappage == 0)
	{
		int gstat;
		double ttoGSAOS, ttoGSLOS;

		gstat = OrbMech::findNextAOS(sv0.R, sv0.V, sv0.MJD, sv0.gravref);

		OrbMech::groundstation(sv0.R, sv0.V, sv0.MJD, sv0.gravref, groundstations[gstat][0], groundstations[gstat][1], 1, ttoGSAOS);
		OrbMech::groundstation(sv0.R, sv0.V, sv0.MJD, sv0.gravref, groundstations[gstat][0], groundstations[gstat][1], 0, ttoGSLOS);
		GSAOSGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoGSAOS;
		GSLOSGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoGSLOS;
		mapgs = gstat;
	}
	else
	{
		rtcc->LunarOrbitMapUpdate(sv0, GETbase, mapupdate);
	}
}

void ARCore::NavCheckPAD()
{
	SV sv;

	sv = rtcc->StateVectorCalc(vessel);

	rtcc->NavCheckPAD(sv, navcheckpad, GETbase, navcheckpad.NavChk[0]);
}

void ARCore::LandingSiteUpdate()
{
	double lat, lng, rad;
	svtarget->GetEquPos(lng, lat, rad);

	LSLat = lat;
	LSLng = lng;
	LSAlt = rad - oapiGetSize(svtarget->GetGravityRef());
}

void ARCore::LandingSiteUplink()
{
	VECTOR3 R_P, R;
	double r_0;

	R_P = unit(_V(cos(LSLng)*cos(LSLat), sin(LSLng)*cos(LSLat), sin(LSLat)));
	r_0 = oapiGetSize(oapiGetObjectByName("Moon"));

	R = R_P*(r_0 + LSAlt);

	g_Data.emem[0] = 10;

	if (vesseltype < 2)
	{
		g_Data.emem[1] = 2025;
	}
	else
	{
		if (mission < 14)
		{
			g_Data.emem[1] = 2022;
		}
		else
		{
			g_Data.emem[1] = 2020;
		}
	}

	g_Data.emem[2] = OrbMech::DoubleToBuffer(R.x, 27, 1);
	g_Data.emem[3] = OrbMech::DoubleToBuffer(R.x, 27, 0);
	g_Data.emem[4] = OrbMech::DoubleToBuffer(R.y, 27, 1);
	g_Data.emem[5] = OrbMech::DoubleToBuffer(R.y, 27, 0);
	g_Data.emem[6] = OrbMech::DoubleToBuffer(R.z, 27, 1);
	g_Data.emem[7] = OrbMech::DoubleToBuffer(R.z, 27, 0);

	UplinkData(); // Go for uplink
}

void ARCore::StateVectorCalc()
{
	VECTOR3 R, V,R0B,V0B,R1B,V1B;
	//MATRIX3 Rot;
	double SVMJD,dt;
	OBJHANDLE gravref = rtcc->AGCGravityRef(vessel);
	svtarget->GetRelativePos(gravref, R);
	svtarget->GetRelativeVel(gravref, V);
	SVMJD = oapiGetSimMJD();

	R0B = _V(R.x, R.z, R.y);
	V0B = _V(V.x, V.z, V.y);

	if (svtimemode)
	{
		dt = J2000GET - (SVMJD - GETbase)*24.0*3600.0;
		OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, gravref);
	}
	else
	{
		J2000GET = (SVMJD - GETbase)*24.0*3600.0;
		R1B = R0B;
		V1B = V0B;
	}

	//oapiGetPlanetObliquityMatrix(gravref, &obl);

	//convmat = mul(_M(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0),mul(transpose_matrix(obl),_M(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0)));

	J2000Pos = R1B;
	J2000Vel = V1B;
}

void ARCore::AGSStateVectorCalc()
{
	AGSSVOpt opt;
	SV sv;

	sv = rtcc->StateVectorCalc(svtarget);

	opt.AGSbase = AGSKFactor;
	opt.csm = SVSlot;
	opt.GETbase = GETbase;
	opt.REFSMMAT = REFSMMAT;
	opt.sv = sv;

	rtcc->AGSStateVectorPAD(&opt, agssvpad);
}

void ARCore::StateVectorUplink()
{

	OBJHANDLE hMoon = oapiGetGbodyByName("Moon");
	OBJHANDLE hEarth = oapiGetGbodyByName("Earth");
	OBJHANDLE gravref = rtcc->AGCGravityRef(vessel);
	VECTOR3 vel,pos;
	double get;
	MATRIX3 Rot;

	Rot = OrbMech::J2000EclToBRCS(AGCEpoch);

	pos = mul(Rot, J2000Pos);
	vel = mul(Rot, J2000Vel)*0.01;
	get = J2000GET;

	if (gravref == hMoon) {

		g_Data.emem[0] = 21;
		g_Data.emem[1] = 1501;

		//if (g_Data.vessel->GetHandle()==oapiGetFocusObject()) 
		if (SVSlot)
		{
			g_Data.emem[2] = 2;
		}
		else
		{
			g_Data.emem[2] = 77775;	// Octal coded decimal
		}

		g_Data.emem[3] = OrbMech::DoubleToBuffer(pos.x, 27, 1);
		g_Data.emem[4] = OrbMech::DoubleToBuffer(pos.x, 27, 0);
		g_Data.emem[5] = OrbMech::DoubleToBuffer(pos.y, 27, 1);
		g_Data.emem[6] = OrbMech::DoubleToBuffer(pos.y, 27, 0);
		g_Data.emem[7] = OrbMech::DoubleToBuffer(pos.z, 27, 1);
		g_Data.emem[8] = OrbMech::DoubleToBuffer(pos.z, 27, 0);
		g_Data.emem[9] = OrbMech::DoubleToBuffer(vel.x, 5, 1);
		g_Data.emem[10] = OrbMech::DoubleToBuffer(vel.x, 5, 0);
		g_Data.emem[11] = OrbMech::DoubleToBuffer(vel.y, 5, 1);
		g_Data.emem[12] = OrbMech::DoubleToBuffer(vel.y, 5, 0);
		g_Data.emem[13] = OrbMech::DoubleToBuffer(vel.z, 5, 1);
		g_Data.emem[14] = OrbMech::DoubleToBuffer(vel.z, 5, 0);
		g_Data.emem[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		g_Data.emem[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);

		//g_Data.uplinkDataReady = 2;
		UplinkData(); // Go for uplink
		return;
	}

	if (gravref == hEarth) {

		g_Data.emem[0] = 21;
		g_Data.emem[1] = 1501;

		//if (g_Data.vessel->GetHandle()==oapiGetFocusObject()) 
		if (SVSlot)
		{
			g_Data.emem[2] = 1;
		}
		else
		{
			g_Data.emem[2] = 77776;	// Octal coded decimal
		}

		g_Data.emem[3] = OrbMech::DoubleToBuffer(pos.x, 29, 1);
		g_Data.emem[4] = OrbMech::DoubleToBuffer(pos.x, 29, 0);
		g_Data.emem[5] = OrbMech::DoubleToBuffer(pos.y, 29, 1);
		g_Data.emem[6] = OrbMech::DoubleToBuffer(pos.y, 29, 0);
		g_Data.emem[7] = OrbMech::DoubleToBuffer(pos.z, 29, 1);
		g_Data.emem[8] = OrbMech::DoubleToBuffer(pos.z, 29, 0);
		g_Data.emem[9] = OrbMech::DoubleToBuffer(vel.x, 7, 1);
		g_Data.emem[10] = OrbMech::DoubleToBuffer(vel.x, 7, 0);
		g_Data.emem[11] = OrbMech::DoubleToBuffer(vel.y, 7, 1);
		g_Data.emem[12] = OrbMech::DoubleToBuffer(vel.y, 7, 0);
		g_Data.emem[13] = OrbMech::DoubleToBuffer(vel.z, 7, 1);
		g_Data.emem[14] = OrbMech::DoubleToBuffer(vel.z, 7, 0);
		g_Data.emem[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		g_Data.emem[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);

		//g_Data.uplinkDataReady = 2;
		UplinkData(); // Go for uplink
		return;
	}
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
		if (mission < 11)
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

		if (mission < 14)
		{
			g_Data.emem[1] = 2400;
			g_Data.emem[3] = 2401;
		}
		else
		{
			g_Data.emem[1] = 2026;
			g_Data.emem[3] = 2027;
		}
		g_Data.emem[2] = OrbMech::DoubleToBuffer(t_Land*100.0, 28, 1);
		g_Data.emem[4] = OrbMech::DoubleToBuffer(t_Land*100.0, 28, 0);

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
	OBJHANDLE gravref = rtcc->AGCGravityRef(vessel);
	vessel->GetRelativePos(gravref, R);
	vessel->GetRelativeVel(gravref, V);
	MJD = oapiGetSimMJD();

	return OrbMech::vesselinLOS(R, V, MJD, gravref);
}

void ARCore::VecPointCalc()
{
	VECTOR3 vPos, pPos, relvec, UX, UY, UZ, loc;
	MATRIX3 M, M_R;
	double p_T, y_T;
	OBJHANDLE gravref = rtcc->AGCGravityRef(vessel);

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

	VECangles = OrbMech::CALCGAR(REFSMMAT, mul(OrbMech::transpose_matrix(M), M_R));
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

	axis = mul(OrbMech::transpose_matrix(Rot3), mul(Rot4, _V(0.0, 1.0, 0.0)));


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
	subThreadStatus = 2; // Running
	switch (subThreadMode) {
	case 0: // Test
		Sleep(5000); // Waste 5 seconds
		Result = 0;  // Success (negative = error)
		break;
	case 1: //Lambert Targeting
	{
		LambertMan opt;
		opt.axis = !lambertmultiaxis;
		opt.GETbase = GETbase;
		opt.impulsive = RTCC_NONIMPULSIVE;
		opt.N = N;
		opt.Offset = offvec;
		opt.Perturbation = lambertopt;
		opt.PhaseAngle = 0.0;
		opt.T1 = T1;
		opt.T2 = T2;
		opt.target = target;
		opt.vessel = vessel;

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
		rtcc->LambertTargeting(&opt, dV_LVLH, P30TIG);
		LambertdeltaV = dV_LVLH;

		Result = 0;
	}
	break;
	case 2:	//CDH Targeting
	{
		double dH_CDH;
		CDHOpt opt;

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

		opt.CDHtimemode = CDHtimemode;
		opt.DH = DH*1852.0;
		opt.GETbase = GETbase;
		opt.impulsive = RTCC_NONIMPULSIVE;
		opt.target = target;
		opt.vessel = vessel;
		opt.TIG = CDHtime;

		dH_CDH = rtcc->CDHcalc(&opt, CDHdeltaV, CDHtime_cor);

		DH = dH_CDH / 1852.0;

		P30TIG = CDHtime_cor;
		dV_LVLH = CDHdeltaV;

		Result = 0;
	}
	break;
	case 3:	//Orbital Adjustment Targeting
	{
		GMPOpt opt;
		OBJHANDLE gravref = rtcc->AGCGravityRef(vessel);

		opt.type = GMPType;
		opt.GETbase = GETbase;
		opt.h_apo = apo_desnm * 1852.0;
		opt.h_peri = peri_desnm * 1852.0;
		opt.impulsive = RTCC_NONIMPULSIVE;
		opt.inc = incdeg*RAD;
		opt.TIG_GET = SPSGET;
		opt.vessel = vessel;
		opt.useSV = false;
		opt.AltRef = OrbAdjAltRef;
		opt.LSAlt = LSAlt;
		opt.rot_ang = GMPRotationAngle;
		opt.lng = GMPLongitude;
		opt.N = GMPRevs;

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

		rtcc->GeneralManeuverProcessor(&opt, OrbAdjDVX, P30TIG, GMPTOA);

		dV_LVLH = OrbAdjDVX;

		Result = 0;
	}
	break;
	case 4:	//REFSMMAT Calculation
	{
		MATRIX3 a;
		REFSMMATOpt opt;

		opt.dV_LVLH = dV_LVLH;
		opt.dV_LVLH2 = LOI_dV_LVLH;
		opt.GETbase = GETbase;
		opt.LSAzi = LOIazi;
		opt.LSLat = LSLat;
		opt.LSLng = LSLng;
		opt.mission = mission;
		opt.P30TIG = P30TIG;
		opt.P30TIG2 = LOI_TIG;
		opt.REFSMMATdirect = REFSMMATdirect;
		opt.REFSMMATopt = REFSMMATopt;

		if (REFSMMATopt == 5 || REFSMMATopt == 8)
		{
			opt.REFSMMATTime = t_Land;
		}
		else
		{
			opt.REFSMMATTime = REFSMMATTime;
		}

		opt.vessel = vessel;
		opt.vesseltype = vesseltype;
		opt.HeadsUp = REFSMMATHeadsUp;

		if (vesseltype == 0 || vesseltype == 2)
		{
			opt.csmlmdocked = false;
		}
		else
		{
			opt.csmlmdocked = true;
		}

		REFSMMAT = rtcc->REFSMMATCalc(&opt);

		//sprintf(oapiDebugString(), "%f, %f, %f, %f, %f, %f, %f, %f, %f", REFSMMAT.m11, REFSMMAT.m12, REFSMMAT.m13, REFSMMAT.m21, REFSMMAT.m22, REFSMMAT.m23, REFSMMAT.m31, REFSMMAT.m32, REFSMMAT.m33);

		a = mul(REFSMMAT, OrbMech::transpose_matrix(OrbMech::J2000EclToBRCS(AGCEpoch)));

		if (REFSMMATupl == 0)
		{
			if (vesseltype < 2)
			{
				REFSMMAToct[1] = 306;
			}
			else
			{
				REFSMMAToct[1] = 3606;
			}
		}
		else
		{
			REFSMMAToct[1] = REFSMMAT_Address();

			if (vesseltype < 2)
			{
				REFSMMAToct[1] = 1735;
			}
			else
			{
				REFSMMAToct[1] = 1733;
			}

			if (mission >= 14)	//Luminary 210 and Artemis 072 both have the REFSMMAT two addresses earlier
			{
				REFSMMAToct[1] -= 2;
			}
		}

		REFSMMAToct[0] = 24;
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

		REFSMMATcur = REFSMMATopt;

		Result = 0;
	}
	break;
	case 5: //LOI Targeting
	{
		
		if (LOImaneuver == 0 || LOImaneuver == 1)
		{
			LOIMan loiopt;

			loiopt.GETbase = GETbase;
			loiopt.h_apo = LOIapo;
			loiopt.h_peri = LOIperi;
			loiopt.lat = LSLat;
			loiopt.lng = LSLng;
			loiopt.alt = LSAlt;
			loiopt.t_land = t_Land;
			loiopt.azi = LOIazi;
			loiopt.vessel = vessel;
			loiopt.type = LOIOption;

			if (vesseltype < 2)
			{
				loiopt.vesseltype = 0;
			}
			else
			{
				loiopt.vesseltype = 1;
			}

			if (vesseltype == 0 || vesseltype == 2)
			{
				loiopt.csmlmdocked = false;
			}
			else
			{
				loiopt.csmlmdocked = true;
			}

			if (LOImaneuver == 0)
			{
				SV RV1, RV2;

				RV1 = rtcc->StateVectorCalc(vessel);
				RV2 = rtcc->ExecuteManeuver(vessel, GETbase, TLCC_TIG, TLCC_dV_LVLH, RV1, 0);

				loiopt.useSV = true;
				loiopt.RV_MCC = RV2;
			}

			rtcc->LOITargeting(&loiopt, LOI_dV_LVLH, LOI_TIG);
			P30TIG = LOI_TIG;
			dV_LVLH = LOI_dV_LVLH;

		}
		else if (LOImaneuver == 2)
		{
			LOI2Man opt;

			opt.alt = LSAlt;
			opt.GETbase = GETbase;
			opt.h_circ = LOI2Alt;
			opt.vessel = vessel;
			opt.useSV = false;

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

			rtcc->LOI2Targeting(&opt, LOI_dV_LVLH, LOI_TIG);
			P30TIG = LOI_TIG;
			dV_LVLH = LOI_dV_LVLH;
		}


		Result = 0;
	}
	break;
	case 6: //TPI PAD
	{
		AP7TPIPADOpt opt;
		AP7TPI pad;

		opt.dV_LVLH = dV_LVLH;
		opt.GETbase = GETbase;
		opt.target = target;
		opt.vessel = vessel;
		opt.TIG = P30TIG;

		rtcc->AP7TPIPAD(&opt, pad);

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

		if (entrylongmanual)
		{
			opt.lng = EntryLng;
		}
		else
		{
			opt.lng = (double)landingzone;
		}

		opt.GETbase = GETbase;
		opt.impulsive = RTCC_NONIMPULSIVE;
		opt.entrylongmanual = entrylongmanual;
		opt.ReA = EntryAng;
		opt.TIGguess = EntryTIG;
		opt.vessel = vessel;
		opt.type = entrycritical;

		rtcc->EntryTargeting(&opt, &res);

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
	case 8: //TLI PAD
	{
		TLIPADOpt opt;
		opt.dV_LVLH = dV_LVLH;
		opt.GETbase = GETbase;
		opt.REFSMMAT = REFSMMAT;
		opt.TIG = P30TIG;
		opt.vessel = vessel;
		opt.uselvdc = false;
		opt.SeparationAttitude = _V(PI, 120.0*RAD, 0.0);
		rtcc->TLI_PAD(&opt, tlipad);
		Result = 0;
	}
	break;
	case 9: //Maneuver PAD
	{
		if (vesseltype < 2)
		{
			AP11ManPADOpt opt;

			opt.dV_LVLH = dV_LVLH;
			opt.engopt = ManPADSPS;
			opt.GETbase = GETbase;
			opt.HeadsUp = HeadsUp;
			opt.REFSMMAT = REFSMMAT;
			opt.sxtstardtime = sxtstardtime;
			opt.TIG = P30TIG;
			opt.vessel = vessel;
			opt.vesseltype = vesseltype;
			opt.alt = LSAlt;

			rtcc->AP11ManeuverPAD(&opt, manpad);
		}
		else
		{
			AP11LMManPADOpt opt;

			opt.dV_LVLH = dV_LVLH;
			opt.engopt = ManPADSPS;
			opt.GETbase = GETbase;
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
			opt.alt = LSAlt;

			rtcc->AP11LMManeuverPAD(&opt, lmmanpad);
		}

		Result = 0;
	}
	break;
	case 10:	//DOI Targeting
	{
		DOIMan opt;
		VECTOR3 DOI_DV_imp;
		double DOI_TIG_imp;

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
		opt.EarliestGET = DOIGET;
		opt.GETbase = GETbase;
		opt.lat = LSLat;
		opt.lng = LSLng;
		opt.alt = LSAlt;
		opt.vessel = vessel;
		opt.N = DOI_N;
		opt.PeriAng = DOI_PeriAng;
		opt.opt = DOI_option;

		rtcc->DOITargeting(&opt, DOI_DV_imp, DOI_TIG_imp, DOI_dV_LVLH, DOI_TIG, DOI_t_PDI, t_Land, DOI_CR);

		P30TIG = DOI_TIG;
		dV_LVLH = DOI_dV_LVLH;

		Result = 0;
	}
	break;
	case 11: //TEI Targeting
	{
		TEIOpt opt;
		EntryResults res;

		entryprecision = 1;

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
		
		if (entrylongmanual)
		{
			opt.EntryLng = EntryLng;
		}
		else
		{
			opt.EntryLng = (double)landingzone;
		}
		opt.GETbase = GETbase;
		opt.returnspeed = returnspeed;
		opt.RevsTillTEI = 0;
		opt.vessel = vessel;
		opt.entrylongmanual = entrylongmanual;
		opt.TIGguess = EntryTIG;

		rtcc->TEITargeting(&opt, &res);//Entry_DV, EntryTIGcor, EntryLatcor, EntryLngcor, P37GET400K, EntryRTGO, EntryVIO, EntryAngcor);

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
	case 12: //Skylab Rendezvous Targeting
	{
		SkyRendOpt opt;
		SkylabRendezvousResults res;

		opt.E_L = Skylab_E_L;
		opt.GETbase = GETbase;
		opt.man = Skylabmaneuver;
		opt.DH1 = SkylabDH1;
		opt.DH2 = SkylabDH2;
		opt.n_C = Skylab_n_C;
		opt.target = target;
		opt.t_TPI = Skylab_t_TPI;
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
			opt.t_C = Skylab_t_TPI;
		}
		else if (Skylabmaneuver == 6)
		{
			opt.t_C = Skylab_t_TPI + Skylab_dt_TPM;
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

		SkylabSolGood = rtcc->SkylabRendezvous(&opt, &res);

		if (SkylabSolGood)
		{
			if (Skylabmaneuver == 0)
			{
				Skylab_t_TPI = res.t_TPI;
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
		opt.EarliestGET = PCEarliestGET;
		opt.GETbase = GETbase;
		opt.lat = LSLat;
		opt.lng = LSLng;
		opt.alt = LSAlt;
		opt.vessel = vessel;
		opt.target = target;
		opt.landed = PClanded;
		opt.t_A = PCAlignGET;

		rtcc->PlaneChangeTargeting(&opt, PC_dV_LVLH, PC_TIG);

		P30TIG = PC_TIG;
		dV_LVLH = PC_dV_LVLH;

		Result = 0;
	}
	break;
	case 14: //TLI/MCC Targeting
	{
		if (TLCCmaneuver == 0)
		{
			TLIManNode opt;
			double MJDcut;

			opt.GETbase = GETbase;
			opt.h_peri = TLCCNodeAlt;
			opt.lat = TLCCNodeLat;
			opt.lng = TLCCNodeLng;
			opt.TLI_TIG = TLCC_GET;
			opt.PeriGET = TLCCNodeGET;
			opt.vessel = vessel;
			opt.useSV = false;

			rtcc->TranslunarInjectionProcessorNodal(&opt, TLCC_dV_LVLH, TLCC_TIG, R_TLI, V_TLI, MJDcut);
			P30TIG = TLCC_TIG;
			dV_LVLH = TLCC_dV_LVLH;
		}
		else if (TLCCmaneuver == 1)
		{
			TLIManFR opt;
			TLMCCResults res;
			double MJDcut;

			opt.GETbase = GETbase;
			opt.h_peri = TLCCFlybyPeriAlt;
			opt.lat = TLCCFreeReturnEMPLat;
			opt.TLI_TIG = TLCC_GET;
			opt.PeriGET = TLCCPeriGET;
			opt.vessel = vessel;
			opt.useSV = false;

			rtcc->TranslunarInjectionProcessorFreeReturn(&opt, &res, R_TLI, V_TLI, MJDcut);

			TLCC_dV_LVLH = res.dV_LVLH;
			TLCC_TIG = res.P30TIG;
			TLCCPeriGETcor = res.PericynthionGET;
			TLCCReentryGET = res.EntryInterfaceGET;
			TLCCFRIncl = res.FRInclination;
			P30TIG = TLCC_TIG;
			dV_LVLH = TLCC_dV_LVLH;
			TLCCFRLat = res.SplashdownLat;
			TLCCFRLng = res.SplashdownLng;
		}
		else if (TLCCmaneuver == 2)
		{
			MCCNodeMan opt;

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

			opt.GETbase = GETbase;
			opt.h_node = TLCCNodeAlt;
			opt.lat = TLCCNodeLat;
			opt.lng = TLCCNodeLng;
			opt.MCCGET = TLCC_GET;
			opt.NodeGET = TLCCNodeGET;
			opt.vessel = vessel;

			rtcc->TranslunarMidcourseCorrectionTargetingNodal(&opt, TLCC_dV_LVLH, TLCC_TIG);
			P30TIG = TLCC_TIG;
			dV_LVLH = TLCC_dV_LVLH;
		}
		else if (TLCCmaneuver == 3 || TLCCmaneuver == 4)
		{
			MCCFRMan opt;
			TLMCCResults res;

			if (TLCCmaneuver == 3)
			{
				opt.type = 0;
			}
			else
			{
				opt.type = 1;
			}

			opt.GETbase = GETbase;
			opt.lat = TLCCFreeReturnEMPLat;
			opt.PeriGET = TLCCPeriGET;
			opt.MCCGET = TLCC_GET;
			opt.vessel = vessel;

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

			opt.LOIh_apo = LOIapo;
			opt.LOIh_peri = LOIperi;
			opt.LSlat = LSLat;
			opt.LSlng = LSLng;
			opt.alt = LSAlt;
			opt.t_land = t_Land;
			opt.azi = LOIazi;
			opt.h_peri = TLCCLAHPeriAlt;

			TLCCSolGood = rtcc->TranslunarMidcourseCorrectionTargetingFreeReturn(&opt, &res);

			if (TLCCSolGood)
			{
				TLCC_dV_LVLH = res.dV_LVLH;
				TLCC_TIG = res.P30TIG;
				TLCCPeriGETcor = res.PericynthionGET;
				TLCCReentryGET = res.EntryInterfaceGET;
				TLCCNodeLat = res.NodeLat;
				TLCCNodeLng = res.NodeLng;
				TLCCNodeAlt = res.NodeAlt;
				TLCCNodeGET = res.NodeGET;
				TLCCFRIncl = res.FRInclination;
				TLCCEMPLatcor = res.EMPLatitude;
				TLCCFRLat = res.SplashdownLat;
				TLCCFRLng = res.SplashdownLng;

				P30TIG = TLCC_TIG;
				dV_LVLH = TLCC_dV_LVLH;
			}
		}
		else if (TLCCmaneuver == 5 || TLCCmaneuver == 6)
		{
			MCCNFRMan opt;
			TLMCCResults res;

			if (TLCCmaneuver == 5)
			{
				opt.type = 0;
			}
			else
			{
				opt.type = 1;
			}

			opt.GETbase = GETbase;
			opt.lat = TLCCNonFreeReturnEMPLat;
			opt.PeriGET = TLCCPeriGET;
			opt.MCCGET = TLCC_GET;
			opt.vessel = vessel;

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

			opt.LOIh_apo = LOIapo;
			opt.LOIh_peri = LOIperi;
			opt.LSlat = LSLat;
			opt.LSlng = LSLng;
			opt.alt = LSAlt;
			opt.t_land = t_Land;
			opt.azi = LOIazi;
			opt.h_peri = TLCCLAHPeriAlt;

			TLCCSolGood = rtcc->TranslunarMidcourseCorrectionTargetingNonFreeReturn(&opt, &res);

			if (TLCCSolGood)
			{
				TLCC_dV_LVLH = res.dV_LVLH;
				TLCC_TIG = res.P30TIG;
				TLCCNodeLat = res.NodeLat;
				TLCCNodeLng = res.NodeLng;
				TLCCNodeAlt = res.NodeAlt;
				TLCCNodeGET = res.NodeGET;
				TLCCEMPLatcor = res.EMPLatitude;

				P30TIG = TLCC_TIG;
				dV_LVLH = TLCC_dV_LVLH;
			}
		}
		else if (TLCCmaneuver == 7)
		{
			MCCFlybyMan opt;
			TLMCCResults res;

			opt.GETbase = GETbase;
			opt.lat = TLCCFreeReturnEMPLat;
			opt.PeriGET = TLCCPeriGET;
			opt.MCCGET = TLCC_GET;
			opt.vessel = vessel;
			opt.h_peri = TLCCFlybyPeriAlt;

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

			TLCCSolGood = rtcc->TranslunarMidcourseCorrectionTargetingFlyby(&opt, &res);

			if (TLCCSolGood)
			{
				TLCC_dV_LVLH = res.dV_LVLH;
				TLCC_TIG = res.P30TIG;
				TLCCPeriGETcor = res.PericynthionGET;
				TLCCReentryGET = res.EntryInterfaceGET;
				TLCCFRIncl = res.FRInclination;
				TLCCFRLat = res.SplashdownLat;
				TLCCFRLng = res.SplashdownLng;

				P30TIG = TLCC_TIG;
				dV_LVLH = TLCC_dV_LVLH;
			}
		}
		else if (TLCCmaneuver == 8)
		{
			MCCSPSLunarFlybyMan opt;
			TLMCCResults res;

			opt.GETbase = GETbase;
			opt.lat = TLCCFreeReturnEMPLat;
			opt.PeriGET = TLCCPeriGET;
			opt.MCCGET = TLCC_GET;
			opt.vessel = vessel;
			opt.h_peri = TLCCFlybyPeriAlt;
			opt.AscendingNode = TLCCAscendingNode;
			opt.FRInclination = TLCCFRDesiredInclination;

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

			TLCCSolGood = rtcc->TranslunarMidcourseCorrectionTargetingSPSLunarFlyby(&opt, &res, TLCCIterationStep);

			if (TLCCSolGood)
			{
				TLCC_dV_LVLH = res.dV_LVLH;
				TLCC_TIG = res.P30TIG;
				TLCCPeriGETcor = res.PericynthionGET;
				TLCCReentryGET = res.EntryInterfaceGET;
				TLCCFRIncl = res.FRInclination;
				TLCCFRLat = res.SplashdownLat;
				TLCCFRLng = res.SplashdownLng;
				TLCCEMPLatcor = res.EMPLatitude;

				P30TIG = TLCC_TIG;
				dV_LVLH = TLCC_dV_LVLH;
			}
		}
		Result = 0;
	}
	break;
	case 15:	//Lunar Liftoff Time Prediction
	{
		LunarLiftoffTimeOpt opt;

		opt.GETbase = GETbase;
		opt.opt = LunarLiftoffTimeOption;
		opt.target = target;
		opt.t_TPIguess = t_TPIguess;
		opt.vessel = vessel;

		rtcc->LaunchTimePredictionProcessor(&opt, &LunarLiftoffTimes);

		Result = 0;
	}
	break;
	case 16: //PDI PAD
	{
		PDIPADOpt opt;
		AP11PDIPAD temppdipad;

		double rad = oapiGetSize(oapiGetObjectByName("Moon"));

		opt.direct = ManPADdirect;
		opt.GETbase = GETbase;
		opt.HeadsUp = HeadsUp;
		opt.REFSMMAT = REFSMMAT;
		opt.R_LS = OrbMech::r_from_latlong(LSLat, LSLng, LSAlt + rad);
		opt.t_land = t_Land;
		opt.vessel = vessel;
		opt.P30TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;

		PADSolGood = rtcc->PDI_PAD(&opt, temppdipad);

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

		if (entrylongmanual)
		{
			opt.lng = EntryLng;
		}
		else
		{
			opt.lng = (double)landingzone;
		}
		
		opt.GETbase = GETbase;

		if (DeorbitEngineOpt == 0)
		{
			opt.impulsive = RTCC_NONIMPULSIVE;
		}
		else
		{
			opt.impulsive = RTCC_NONIMPULSIVERCS;
		}
		
		opt.entrylongmanual = entrylongmanual;
		opt.ReA = EntryAng;
		opt.TIGguess = EntryTIG;
		opt.vessel = vessel;
		opt.nominal = entrynominal;

		rtcc->BlockDataProcessor(&opt, &res);

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
	case 18: //RTE Flyby Targeting
	{
		RTEFlybyOpt opt;
		EntryResults res;
		double SVMJD;

		SVMJD = oapiGetSimMJD();

		entryprecision = 1;

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

		if (entrylongmanual)
		{
			opt.EntryLng = EntryLng;
		}
		else
		{
			opt.EntryLng = (double)landingzone;
		}
		opt.GETbase = GETbase;
		opt.returnspeed = returnspeed;
		opt.FlybyType = FlybyType;
		opt.vessel = vessel;
		opt.entrylongmanual = entrylongmanual;
		opt.TIGguess = EntryTIG;

		rtcc->RTEFlybyTargeting(&opt, &res);//Entry_DV, EntryTIGcor, EntryLatcor, EntryLngcor, P37GET400K, EntryRTGO, EntryVIO, EntryAngcor);

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

int ARCore::REFSMMAT_Address()
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