#include "ARCore.h"

#include "soundlib.h"
#include "toggleswitch.h"
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
	REFSMMAT = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	REFSMMATTime = 0.0;
	REFSMMATopt = 4; 
	REFSMMATcur = 4;
	REFSMMATupl = 0;
	LSLat = 0.0;
	LSLng = 0.0;
	manpadopt = 0;
	vesseltype = CSM;
	gravref = oapiGetObjectByName("Earth");
	VECTOR3 rsph;
	vessel->GetRelativePos(oapiGetObjectByName("Moon"), rsph);
	if (length(rsph) < 64373760.0)
	{
		gravref = oapiGetObjectByName("Moon");
	}
	//mech = new OrbMech(v, gravref);
	for (int i = 0; i < 20; i++)
	{
		REFSMMAToct[i] = 0;
	}
	if (strcmp(v->GetName(), "AS-205") == 0)
	{
		mission = 7;
		REFSMMAT = A7REFSMMAT;
	}
	else if (strcmp(v->GetName(), "AS-503")==0)
	{
		mission = 8;
		REFSMMAT = A8REFSMMAT;
	}
	else if (strcmp(v->GetName(), "AS-504") == 0 || strcmp(v->GetName(), "Gumdrop") == 0)
	{
		mission = 9;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 72.0*RAD);
	}
	else if (strcmp(v->GetName(), "AS-505") == 0 || strcmp(v->GetName(), "Charlie Brown") == 0)
	{
		mission = 10;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.626530*RAD, -80.620629*RAD, LaunchMJD[mission - 7], 72.0*RAD);
	}
	else if (strcmp(v->GetName(), "AS-506") == 0 || strcmp(v->GetName(), "Columbia") == 0)
	{
		mission = 11;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 72.0*RAD);
	}
	else if (strcmp(v->GetName(), "Eagle") == 0)
	{
		mission = 11;
		vesseltype = LM;
	}
	else if (strcmp(v->GetName(), "Yankee-Clipper") == 0)
	{
		mission = 12;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 72.0*RAD);
	}
	else if (strcmp(v->GetName(), "Endeavour") == 0)
	{
		mission = 15;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission-7], 72.0*RAD);
	}
	GETbase = LaunchMJD[mission - 7];
	MATRIX3 a;
	a = REFSMMAT;

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
	OrbAdjDVX = _V(0, 0, 0);
	SPSGET = (oapiGetSimMJD()-GETbase)*24*60*60;
	apo_desnm = 0;
	peri_desnm = 0;
	incdeg = 0;

	g_Data.uplinkBufferSimt = 0;
	g_Data.connStatus = 0;
	g_Data.uplinkState = 0;
	if (vesseltype == CSM)
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
	entrycalcmode = 0;
	entrycritical = 1; 
	entrynominal = 1;
	entrycalcstate = 0;
	entryrange = 0.0;
	EntryRTGO = 0.0;
	SVSlot = true; //true = CSM; false = Other
	BRCSPos = _V(0.0, 0.0, 0.0);
	BRCSVel = _V(0.0, 0.0, 0.0);
	BRCSGET = 0.0;
	Mantrunnion = 0.0;
	Manshaft = 0.0;
	Manstaroct = 0;
	ManCOASstaroct = 0;
	ManBSSpitch = 0.0;
	ManBSSXPos = 0.0;
	Entrytrunnion = 0.0;
	Entryshaft = 0.0;
	Entrystaroct = 0;
	IMUangles = _V(0, 0, 0);
	GDCangles = _V(0, 0, 0);
	GDCset = 0;
	HeadsUp = false;
	ManPADApo = 0.0;
	ManPADPeri = 0.0;
	ManPADWeight = 0.0;
	ManPADBurnTime = 0.0;
	ManPADDVC = 0.0;
	ManPADPTrim = 0.0;
	ManPADYTrim = 0.0;
	ManPADLMWeight = 0.0;
	ManPADVeh = 0;
	EntryPADRTGO = 0.0;
	EntryPADVIO = 0.0;
	EntryPADRET05Earth = 0.0;
	EntryPADRET05Lunar = 0.0;
	EntryPADV400k = 0.0;
	EntryPADgamma400k = 0.0;
	EntryPADDO = 0.0;
	EntryPADHorChkGET = 0.0;
	EntryPADLift = true;
	EntryPADHorChkPit = 0.0;
	EIangles = _V(0.0, 0.0, 0.0);
	TimeTag = 0.0;
	entrypadopt = 0;
	EntryPADdirect = false; //false = Entry PAD with MCC/Deorbit burn, true = Direct Entry
	EntryPADRRT = 0.0;
	EntryPADLng = 0.0;
	EntryPADLat = 0.0;
	EntryPADGMax = 0.0;
	ManPADSPS = 0;
	TPIPAD_AZ = 0.0;
	TPIPAD_dH = 0.0;
	TPIPAD_dV_LOS = _V(0.0, 0.0, 0.0);
	TPIPAD_ELmin5 = 0.0;
	TPIPAD_R = 0.0;
	TPIPAD_Rdot = 0.0;
	TPIPAD_ddH = 0.0;
	TPIPAD_BT = _V(0.0, 0.0, 0.0);
	maneuverplanet = gravref;
	sxtstardtime = 0.0;
	P37GET400K = 0.0;
	LOSGET = 0.0;
	AOSGET = 0.0;
	SSGET = 0.0;
	SRGET = 0.0;
	PMGET = 0.0;
	mappage = 1;
	mapgs = 0;
	GSAOSGET = 0.0;
	GSLOSGET = 0.0;
	inhibUplLOS = false;
	svtarget = NULL;
	svtargetnumber = -1;
	svtimemode = 0;
	lambertmultiaxis = 1;
	entrylongmanual = true;
	landingzone = 0;
	entryprecision = -1;
	EntryPADdVTO = 0.0;

	rtcc = new RTCC();

	EntryPADPB_RTGO = 0.0;
	EntryPADPB_R400K = 0.0;
	EntryPADPB_Ret05 = 0.0;
	EntryPADPB_VIO = 0.0;

	LOImaneuver = 0;
	LOIGET = 0.0;
	LOIPeriGET = 0.0;
	LOILat = 0.0;
	LOILng = 0.0;
	LOIapo = 0.0;
	LOIperi = 0.0;
	LOIinc = 0.0;
	TLCC_dV_LVLH = _V(0.0, 0.0, 0.0);
	LOI_dV_LVLH = _V(0.0, 0.0, 0.0);
	TLCC_TIG = 0.0;
	LOI_TIG = 0.0;
	tlipad.TB6P = 0.0;
	tlipad.BurnTime = 0.0;
	tlipad.dVC = 0.0;
	tlipad.VI = 0.0;
	tlipad.SepATT = _V(0.0, 0.0, 0.0);
	tlipad.IgnATT = _V(0.0, 0.0, 0.0);

	subThreadMode = 0;
	subThreadStatus = 0;
}

void ARCore::MinorCycle(double SimT, double SimDT, double mjd)
{
	if (entrycalcstate == 1)
	{
		startSubthread(7);
	}
	else if (entrycalcstate == 2)
	{
		entry->EntryUpdateCalc();
		EntryLatcor = entry->EntryLatPred;
		EntryLngcor = entry->EntryLngPred;
		EntryRTGO = entry->EntryRTGO;
		entrycalcstate = 0;
		delete entry;
	}
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
			//tlipad.BurnTime = g_Data.burnData.BT;
			//tlipad.TB6P = P30TIG - 578.0;
			//tlipad.IgnATT
		}
		else {
			//g_Data.progState = PROGSTATE_TLI_WAITING;
		}
	}
}

void ARCore::LOICalc()
{
	startSubthread(5);
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

void ARCore::EntryPAD()
{
	OBJHANDLE hEarth;
	double mu;

	hEarth = oapiGetObjectByName("Earth");
	mu = GGRAV*oapiGetMass(hEarth);

	if (entrypadopt == 0)
	{
		AP7ENT pad;
		EarthEntryPADOpt opt;

		opt.dV_LVLH = dV_LVLH;
		opt.GETbase = GETbase;
		opt.P30TIG = P30TIG;
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
		vessel->GetRelativePos(gravref, R);
		vessel->GetRelativeVel(gravref, V);
		OrbMech::periapo(R, V, mu, apo, peri);
		if (peri < oapiGetSize(gravref) + 50 * 1852.0)
		{
			opt.preburn = false;
			rtcc->EarthOrbitEntry(&opt, pad);

			EntryPADPB_RTGO = pad.PB_RTGO[0];
			EntryPADPB_R400K = pad.PB_R400K[0];
			EntryPADPB_Ret05 = pad.PB_Ret05[0];
			EntryPADPB_VIO = pad.PB_VIO[0];
		}
		else
		{
			opt.preburn = true;
			rtcc->EarthOrbitEntry(&opt, pad);
			EIangles = pad.Att400K[0];
			EntryPADdVTO = pad.dVTO[0];
			EntryPADRTGO = pad.RTGO[0];
			EntryPADVIO = pad.VIO[0];
			EntryPADRET05Earth = pad.Ret05[0];
			EntryPADLat = pad.Lat[0];
			EntryPADLng = pad.Lng[0];
		}


		return;
	}

	VECTOR3 DV, R_A, V_A, R0B, V0B, R1B, V1B, UX, UY, UZ, R05G, V05G, R3, V3;
	MATRIX3 Rot, M_R;
	double SVMJD, dt, EMSAlt, dt2, dt3, S_FPA, g_T, V_T, v_BAR;
	double WIE, WT, LSMJD, theta_rad, theta_nm;
	VECTOR3 RTE, UTR, urh, URT0, URT, R_P, R_LS;
	MATRIX3 Rot2;

	dt2 = 0;

	vessel->GetRelativePos(gravref, R_A);
	vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	R0B = mul(Rot, _V(R_A.x, R_A.z, R_A.y));
	V0B = mul(Rot, _V(V_A.x, V_A.z, V_A.y));

	if (entrycritical == 0)
	{
		EMSAlt = 284643.0*0.3048;
	}
	else
	{
		EMSAlt = 297431.0*0.3048;
	}

	if (EntryPADdirect)
	{
		dt = OrbMech::time_radius_integ(R0B, V0B, SVMJD, oapiGetSize(hEarth) + EMSAlt, -1, gravref, hEarth, R05G, V05G);
		//OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R05G, V05G, mu);
		entry = new Entry(vessel, GETbase, EntryTIG, EntryAng, EntryLng, entrycritical, entryrange, 0, true);
		entry->EntryUpdateCalc();
		EntryPADRTGO = entry->EntryRTGO;
		EntryPADVIO = entry->EntryVIO;
		

		if (EntryLatcor == 0)
		{
			EntryPADLat = entry->EntryLatPred;
			EntryPADLng = entry->EntryLngPred;
		}
		else
		{
			EntryPADLat = EntryLatcor;
			EntryPADLng = EntryLngcor;
		}

		delete entry;
		LSMJD = SVMJD + dt / 24.0 / 3600.0;
	}
	else
	{
		VECTOR3 DV_P, DV_C, V_G, R2B, V2B;
		double theta_T, t_go, m0;

		EntryPADLng = EntryLngcor;
		EntryPADLat = EntryLatcor;

		DV = Entry_DV;
		dt = EntryTIGcor - (SVMJD - GETbase) * 24.0 * 60.0 * 60.0;


		OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, maneuverplanet);

		UY = unit(crossp(V1B, R1B));
		UZ = unit(-R1B);
		UX = crossp(UY, UZ);

		//DV = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
		//V2 = V1B + DV;

		DV_P = UX*dV_LVLH.x + UZ*dV_LVLH.z;
		theta_T = length(crossp(R1B, V1B))*length(dV_LVLH)*vessel->GetMass() / OrbMech::power(length(R1B), 2.0) / 92100.0;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + UY*dV_LVLH.y;

		m0 = vessel->GetMass();

		OrbMech::poweredflight(vessel, R1B, V1B, maneuverplanet, vessel->GetGroupThruster(THGROUP_MAIN, 0), m0, V_G, R2B, V2B, t_go);
		OrbMech::oneclickcoast(R2B, V2B, SVMJD + dt/3600.0/24.0, 0.0, R2B, V2B, maneuverplanet, hEarth);

		dt2 = OrbMech::time_radius_integ(R2B, V2B, SVMJD + (dt + t_go) / 3600.0 / 24.0, oapiGetSize(hEarth) + EMSAlt, -1, hEarth, hEarth, R05G, V05G);
		dt2 += t_go;
		//OrbMech::oneclickcoast(R1B, V2, SVMJD + dt / 3600.0 / 24.0, dt2, R05G, V05G, mu);
		//dt22 = OrbMech::time_radius(R05G, -V05G, oapiGetSize(gravref) + 400000.0*0.3048, 1,mu);
		//rv_from_r0v0(R05G, -V05G, dt22, REI, VEI, mu);
		//VEI = -VEI;
		//EntryPADRTGO = EntryRTGO;
		EntryPADVIO = EntryVIO;

		LSMJD = SVMJD + (dt + dt2) / 24.0 / 3600.0;
	}

	R_P = unit(_V(cos(EntryPADLng)*cos(EntryPADLat), sin(EntryPADLat), sin(EntryPADLng)*cos(EntryPADLat)));
	Rot2 = OrbMech::GetRotationMatrix2(hEarth, LSMJD);
	R_LS = mul(Rot2, R_P);
	R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));
	URT0 = R_LS;
	WIE = 72.9211505e-6;
	UZ = _V(0, 0, 1);
	RTE = crossp(UZ, URT0);
	UTR = crossp(RTE, UZ);
	urh = unit(R05G);//unit(r)*cos(theta) + crossp(unit(r), -unit(h_apo))*sin(theta);
	theta_rad = acos(dotp(URT0, urh));
	for (int i = 0;i < 10;i++)
	{
		WT = WIE*(KTETA*theta_rad);
		URT = URT0 + UTR*(cos(WT) - 1.0) + RTE*sin(WT);
		theta_rad = acos(dotp(URT, urh));
	}
	theta_nm = theta_rad*3437.7468;
	EntryPADRTGO = theta_nm;

	UX = unit(V05G);
	UY = unit(crossp(UX, R05G));
	UZ = unit(crossp(UX, crossp(UX, R05G)));

	double aoa = -157.0*RAD;
	VECTOR3 vunit = _V(0.0, 1.0, 0.0);
	MATRIX3 Rotaoa;

	Rotaoa = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0)*cos(aoa) + OrbMech::skew(vunit)*sin(aoa) + outerp(vunit, vunit)*(1.0 - cos(aoa));

	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	EIangles = OrbMech::CALCGAR(REFSMMAT, mul(Rotaoa, M_R));


	dt3 = OrbMech::time_radius(R05G, -V05G, oapiGetSize(hEarth) + 300000.0*0.3048, 1, mu);
	OrbMech::rv_from_r0v0(R05G, -V05G, dt3, R3, V3, mu);
	V3 = -V3;
	S_FPA = dotp(unit(R3), V3) / length(V3);
	g_T = asin(S_FPA);
	V_T = length(V3);
	v_BAR = (V_T / 0.3048 - 36000.0) / 20000.0;
	EntryPADGMax = 4.0 / (1.0 + 4.8*v_BAR*v_BAR)*(abs(g_T)*DEG - 6.05 - 2.4*v_BAR*v_BAR) + 10.0;

	double dt22, vei, liftline, horang, coastang, cosIGA, sinIGA, IGA;
	VECTOR3 REI, VEI, UREI, REI17, VEI17, X_NB, Y_NB, Z_NB, X_SM, Y_SM, Z_SM, A_MG, Rcheck, Vcheck;

	dt22 = OrbMech::time_radius(R05G, -V05G, oapiGetSize(hEarth) + 400000.0*0.3048, 1, mu);
	OrbMech::rv_from_r0v0(R05G, -V05G, dt22, REI, VEI, mu);
	VEI = -VEI;
	UREI = unit(REI);
	EntryPADV400k = length(VEI);
	S_FPA = dotp(UREI, VEI) / EntryPADV400k;
	EntryPADgamma400k = asin(S_FPA);
	EntryPADRET05Lunar = dt22;
	EntryPADRRT = dt + dt2 + (SVMJD - GETbase)*24.0*3600.0 - dt22;

	vei = length(VEI) / 0.3048;

	EntryPADDO = 4.2317708e-9*vei*vei + 1.4322917e-6*vei - 1.600664062;

	liftline = 4.055351e-10*vei*vei - 3.149125e-5*vei + 0.503280635;
	if (S_FPA > atan(liftline))
	{
		EntryPADLift = false;
	}
	else
	{
		EntryPADLift = true;
	}

	OrbMech::oneclickcoast(REI, -VEI, EntryPADRRT / 24.0 / 3600.0 + GETbase, 17.0*60.0, REI17, VEI17, hEarth, hEarth);

	OrbMech::oneclickcoast(REI17, VEI17, EntryPADRRT / 24.0 / 3600.0 + GETbase - 1.0 / 24.0, 60.0*60.0, Rcheck, Vcheck, hEarth, hEarth);

	VEI17 = -VEI17;

	EntryPADHorChkGET = EntryPADRRT - 17.0*60.0;

	OrbMech::checkstar(REFSMMAT, _V(OrbMech::round(EIangles.x*DEG)*RAD, OrbMech::round(EIangles.y*DEG)*RAD, OrbMech::round(EIangles.z*DEG)*RAD), Rcheck, oapiGetSize(hEarth), Entrystaroct, Entrytrunnion, Entryshaft);

	horang = asin(oapiGetSize(gravref) / length(REI17));
	coastang = dotp(unit(REI), unit(REI17));

	Z_NB = unit(-REI);
	Y_NB = unit(crossp(VEI, REI));
	X_NB = crossp(Y_NB, Z_NB);

	X_SM = _V(REFSMMAT.m11, REFSMMAT.m12, REFSMMAT.m13);
	Y_SM = _V(REFSMMAT.m21, REFSMMAT.m22, REFSMMAT.m23);
	Z_SM = _V(REFSMMAT.m31, REFSMMAT.m32, REFSMMAT.m33);
	A_MG = unit(crossp(X_NB, Y_SM));
	cosIGA = dotp(A_MG, Z_SM);
	sinIGA = dotp(A_MG, X_SM);
	IGA = atan2(sinIGA, cosIGA);

	EntryPADHorChkPit = PI2 - (horang + coastang + 0.0*31.7*RAD) + IGA;
}

void ARCore::ManeuverPAD()
{
	VECTOR3 DV_P, DV_C, V_G;
	double SVMJD, dt, mu, theta_T,t_go, CSMmass, LMmass;
	VECTOR3 R_A, V_A, R0B, V0B, R1B, V1B, UX, UY, UZ, X_B, DV,U_TD,R2B,V2B;
	MATRIX3 Rot, M, M_R, M_RTM;
	double p_T, y_T, peri, apo, m1;
	bool stop;
	double headsswitch;

	mu = GGRAV*oapiGetMass(gravref);

	vessel->GetRelativePos(gravref, R_A);
	vessel->GetRelativeVel(gravref, V_A);

	SVMJD = oapiGetSimMJD();

	//double ddtt = OrbMech::sunrise(R_A, V_A, SVMJD, gravref,true);
	//TimeTag = (SVMJD - GETbase)*24.0*3600.0 + ddtt;
	//TimeTagUplink();

	dt = P30TIG - (SVMJD - GETbase) * 24.0 * 60.0 * 60.0;

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	R0B = mul(Rot, _V(R_A.x, R_A.z, R_A.y));
	V0B = mul(Rot, _V(V_A.x, V_A.z, V_A.y));

	coast = new CoastIntegrator(R0B, V0B, SVMJD, dt, gravref, gravref);
	stop = false;
	while (stop == false)
	{
		stop = coast->iteration();
	}
	R1B = coast->R2;
	V1B = coast->V2;
	delete coast;

	UY = unit(crossp(V1B, R1B));
	UZ = unit(-R1B);
	UX = crossp(UY, UZ);

	DV = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
	

	CSMmass = vessel->GetMass();

	if (ManPADVeh == 1)
	{
		DOCKHANDLE dock;
		OBJHANDLE hLM;
		VESSEL* lm;
		if (vessel->DockingStatus(0) == 1)
		{
			dock = vessel->GetDockHandle(0);
			hLM = vessel->GetDockStatus(dock);
			lm = oapiGetVesselInterface(hLM);
			LMmass = lm->GetMass();
		}
		else
		{
			LMmass = 0.0;
		}
	}
	else
	{
		LMmass = 0.0;
	}

	double v_e, F;
	if (ManPADSPS == 0)
	{
		v_e = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
		F = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
	}
	else
	{
		v_e = 2706.64;
		F = 400 * 4.448222;
	}

	ManPADBurnTime = v_e / F *(CSMmass + LMmass)*(1.0 - exp(-length(dV_LVLH) / v_e));

	if (HeadsUp)
	{
		headsswitch = 1.0;
	}
	else
	{
		headsswitch = -1.0;
	}

	if (ManPADSPS == 0)
	{
		DV_P = UX*dV_LVLH.x + UZ*dV_LVLH.z;
		if (length(DV_P) != 0.0)
		{
			theta_T = length(crossp(R1B, V1B))*length(dV_LVLH)*(CSMmass + LMmass) / OrbMech::power(length(R1B), 2.0) / 92100.0;
			DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
			V_G = DV_C + UY*dV_LVLH.y;
		}
		else
		{
			V_G = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
		}

		U_TD = unit(V_G);

		OrbMech::poweredflight(vessel, R1B, V1B, gravref, vessel->GetGroupThruster(THGROUP_MAIN, 0), CSMmass + LMmass, V_G, R2B, V2B, t_go);

		OrbMech::periapo(R2B, V2B, mu, apo, peri);
		ManPADApo = apo - oapiGetSize(gravref);
		ManPADPeri = peri - oapiGetSize(gravref);

		double x1 = LMmass / (CSMmass + LMmass)*6.2;
		ManPADPTrim = atan2(SPS_PITCH_OFFSET * RAD * 5.0, 5.0 + x1) + 2.15*RAD;
		ManPADYTrim = atan2(SPS_YAW_OFFSET * RAD * 5.0, 5.0 + x1) - 0.95*RAD;

		p_T = -2.15*RAD + ManPADPTrim;
		y_T = 0.95*RAD + ManPADYTrim;

		X_B = unit(V_G);
		UX = X_B;
		UY = unit(crossp(X_B, R1B*headsswitch));
		UZ = unit(crossp(X_B, crossp(X_B, R1B*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
		M_RTM = mul(OrbMech::transpose_matrix(M_R), M);

		m1 = (CSMmass + LMmass)*exp(-length(dV_LVLH) / v_e);
		ManPADDVC = length(dV_LVLH)*cos(p_T)*cos(y_T);// -60832.18 / m1;
	}
	else
	{
		OrbMech::periapo(R1B, V1B+DV, mu, apo, peri);
		ManPADApo = apo - oapiGetSize(gravref);
		ManPADPeri = peri - oapiGetSize(gravref);

		V_G = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
		X_B = unit(V_G);
		if (ManPADSPS == 1)
		{
			UX = X_B;
		}
		else
		{
			UX = -X_B;
		}
		UY = unit(crossp(UX, R1B*headsswitch));
		UZ = unit(crossp(UX, crossp(UX, R1B*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

		ManPADDVC = length(dV_LVLH);
	}
	IMUangles = OrbMech::CALCGAR(REFSMMAT, mul(OrbMech::transpose_matrix(M), M_R));
	//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);

	GDCangles = OrbMech::backupgdcalignment(REFSMMAT, R1B, oapiGetSize(gravref), GDCset);

	VECTOR3 Rsxt, Vsxt;

	OrbMech::oneclickcoast(R1B, V1B, SVMJD + dt / 24.0 / 3600.0, sxtstardtime*60.0, Rsxt, Vsxt, gravref, gravref);

	OrbMech::checkstar(REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD),Rsxt, oapiGetSize(maneuverplanet), Manstaroct, Mantrunnion, Manshaft);

	OrbMech::coascheckstar(REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD), Rsxt, oapiGetSize(maneuverplanet), ManCOASstaroct, ManBSSpitch, ManBSSXPos);

	ManPADWeight = CSMmass / 0.45359237;
	ManPADLMWeight = LMmass / 0.45359237;
}

void ARCore::TPIPAD()
{
	startSubthread(6);
}

void ARCore::MapUpdate()
{
	VECTOR3 R, V;
	double MJD;
	
	vessel->GetRelativePos(gravref, R);
	vessel->GetRelativeVel(gravref, V);
	MJD = oapiGetSimMJD();
	if (mappage == 0)
	{
		int gstat;
		double ttoGSAOS, ttoGSLOS;

		gstat = OrbMech::findNextAOS(R, V, MJD, gravref);

		OrbMech::groundstation(R, V, MJD, gravref, groundstations[gstat][0], groundstations[gstat][1], 1, ttoGSAOS);
		OrbMech::groundstation(R, V, MJD, gravref, groundstations[gstat][0], groundstations[gstat][1], 0, ttoGSLOS);
		GSAOSGET = (MJD - GETbase)*24.0*3600.0 + ttoGSAOS;
		GSLOSGET = (MJD - GETbase)*24.0*3600.0 + ttoGSLOS;
		mapgs = gstat;
	}
	else
	{
		double ttoLOS, ttoAOS, ttoSS, ttoSR, ttoPM;
		OBJHANDLE hEarth, hSun;

		hEarth = oapiGetObjectByName("Earth");
		hSun = oapiGetObjectByName("Sun");

		ttoLOS = OrbMech::sunrise(R, V, MJD, gravref, hEarth, 0, 0);
		ttoAOS = OrbMech::sunrise(R, V, MJD, gravref, hEarth, 1, 0);

		LOSGET = (MJD - GETbase)*24.0*3600.0 + ttoLOS;
		AOSGET = (MJD - GETbase)*24.0*3600.0 + ttoAOS;

		ttoSS = OrbMech::sunrise(R, V, MJD, gravref, hSun, 0, 0);
		ttoSR = OrbMech::sunrise(R, V, MJD, gravref, hSun, 1, 0);

		SSGET = (MJD - GETbase)*24.0*3600.0 + ttoSS;
		SRGET = (MJD - GETbase)*24.0*3600.0 + ttoSR;

		ttoPM = OrbMech::findlongitude(R, V, MJD, gravref, -150.0 * RAD);
		PMGET = (MJD - GETbase)*24.0*3600.0 + ttoPM;
	}
}

void ARCore::StateVectorCalc()
{
	VECTOR3 R, V,R0B,V0B,R1B,V1B;
	MATRIX3 Rot;
	double SVMJD,dt;

	svtarget->GetRelativePos(gravref, R);
	svtarget->GetRelativeVel(gravref, V);
	SVMJD = oapiGetSimMJD();

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	R0B = mul(Rot, _V(R.x, R.z, R.y));
	V0B = mul(Rot, _V(V.x, V.z, V.y));

	if (svtimemode)
	{
		dt = BRCSGET - (SVMJD - GETbase)*24.0*3600.0;
		OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, gravref);
	}
	else
	{
		BRCSGET = (SVMJD - GETbase)*24.0*3600.0;
		R1B = R0B;
		V1B = V0B;
	}

	//oapiGetPlanetObliquityMatrix(gravref, &obl);

	//convmat = mul(_M(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0),mul(transpose_matrix(obl),_M(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0)));

	BRCSPos = R1B;
	BRCSVel = V1B;
}

void ARCore::StateVectorUplink()
{

	OBJHANDLE hMoon = oapiGetGbodyByName("Moon");
	OBJHANDLE hEarth = oapiGetGbodyByName("Earth");

	VECTOR3 vel,pos;
	double get;

	pos = BRCSPos;
	vel = BRCSVel*0.01;
	get = BRCSGET;

	if (gravref == hMoon) {

		g_Data.emem[0] = 21;
		g_Data.emem[1] = 1501;

		//if (g_Data.vessel->GetHandle()==oapiGetFocusObject()) 
		if (SVSlot == 0)
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

	if (vesseltype == LM){
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
	g_Data.emem[1] = 3404;
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

void ARCore::TimeTagUplink(void)
{
	g_Data.emem[0] = 05;
	g_Data.emem[1] = 1045;
	g_Data.emem[2] = OrbMech::DoubleToBuffer(TimeTag*100, 28, 1);
	g_Data.emem[3] = 1046;
	g_Data.emem[4] = OrbMech::DoubleToBuffer(TimeTag*100, 28, 0);

	//g_Data.uplinkDataReady = 2;
	UplinkData2(); // Go for uplink
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

	vessel->GetRelativePos(gravref, R);
	vessel->GetRelativeVel(gravref, V);
	MJD = oapiGetSimMJD();

	return OrbMech::vesselinLOS(R, V, MJD, gravref);
}

VECTOR3 ARCore::finealignLMtoCSM(VECTOR3 lmn20, VECTOR3 csmn20) //LM noun 20 and CSM noun 20
{
	MATRIX3 lmmat, csmmat,summat,expmat;

	lmmat = OrbMech::CALCSMSC(lmn20);
	csmmat = OrbMech::CALCSMSC(csmn20);
	summat = OrbMech::CALCSMSC(_V(300.0*RAD, PI, 0.0));
	expmat = mul(summat, csmmat);
	return OrbMech::CALCSGTA(mul(lmmat, OrbMech::transpose_matrix(expmat)));
}

int ARCore::startSubthread(int fcn) {
	if (subThreadStatus < 1) {
		// Punt thread
		subThreadMode = fcn;
		subThreadStatus = 1; // Busy
		DWORD id = 0;
		HANDLE h = CreateThread(NULL, 0, RTCCMFD_Trampoline, this, 0, &id);
		if (h != NULL) { CloseHandle(h); }
	}
	else {
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
		opt.prograde = true;
		opt.T1 = T1;
		opt.T2 = T2;
		opt.target = target;
		opt.vessel = vessel;
		rtcc->LambertTargeting(&opt, dV_LVLH, P30TIG);
		LambertdeltaV = dV_LVLH;

		Result = 0;
	}
	break;
	case 2:	//CDH Targeting
	{
		double dH_CDH;
		CDHOpt opt;

		opt.CDHtimemode = CDHtimemode;
		opt.DH = DH*1852.0;
		opt.GETbase = GETbase;
		opt.impulsive = RTCC_NONIMPULSIVE;
		opt.target = target;
		opt.vessel = vessel;
		opt.TIG = CDHtime;

		dH_CDH = rtcc->CDHcalc(&opt, CDHdeltaV, CDHtime_cor);

		if (CDHtimemode == 0)
		{
			DH = dH_CDH / 1852.0;
		}

		P30TIG = CDHtime_cor;
		dV_LVLH = CDHdeltaV;

		Result = 0;
	}
	break;
	case 3:	//Orbital Adjustment Targeting
	{
		maneuverplanet = gravref;

		OrbAdjOpt opt;

		opt.GETbase = GETbase;
		opt.gravref = gravref;
		opt.h_apo = apo_desnm * 1852.0;
		opt.h_peri = peri_desnm * 1852.0;
		opt.inc = incdeg*RAD;
		opt.SPSGET = SPSGET;
		opt.vessel = vessel;
		opt.useSV = false;

		rtcc->OrbitAdjustCalc(&opt, OrbAdjDVX, P30TIG);

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
		opt.LSLat = LSLat;
		opt.LSLng = LSLng;
		opt.maneuverplanet = maneuverplanet;
		opt.mission = mission;
		opt.P30TIG = P30TIG;
		opt.P30TIG2 = LOI_TIG;
		opt.REFSMMATdirect = REFSMMATdirect;
		opt.REFSMMATopt = REFSMMATopt;
		opt.REFSMMATTime = REFSMMATTime;
		opt.vessel = vessel;

		REFSMMAT = rtcc->REFSMMATCalc(&opt);

		a = REFSMMAT;

		if (REFSMMATupl == 0)
		{
			if (vesseltype == CSM)
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
			if (vesseltype == CSM)
			{
				REFSMMAToct[1] = 1735;
			}
			else
			{
				REFSMMAToct[1] = 1733;
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
		LOIMan opt;

		opt.GETbase = GETbase;
		opt.h_apo = LOIapo;
		opt.h_peri = LOIperi;
		opt.inc = LOIinc;
		opt.lat = LOILat;
		opt.lng = LOILng;
		opt.man = LOImaneuver;
		opt.MCCGET = LOIGET;
		opt.PeriGET = LOIPeriGET;
		opt.vessel = vessel;
		opt.useSV = false;

		if (LOImaneuver == 0)
		{
			rtcc->LOITargeting(&opt, TLCC_dV_LVLH, TLCC_TIG);
			P30TIG = TLCC_TIG;
			dV_LVLH = TLCC_dV_LVLH;
		}
		else if (LOImaneuver == 1)
		{
			OBJHANDLE hMoon;
			VECTOR3 R_A, V_A, R0B, V0B, UX, UY, UZ, DV, V2;
			double SVMJD;
			MATRIX3 Rot;
			SV RV1;

			hMoon = oapiGetObjectByName("Moon");
			Rot = OrbMech::J2000EclToBRCS(40222.525);

			vessel->GetRelativePos(hMoon, R_A);
			vessel->GetRelativeVel(hMoon, V_A);
			SVMJD = oapiGetSimMJD();
			R0B = mul(Rot, _V(R_A.x, R_A.z, R_A.y));
			V0B = mul(Rot, _V(V_A.x, V_A.z, V_A.y));

			OrbMech::oneclickcoast(R0B, V0B, SVMJD, TLCC_TIG - (SVMJD - GETbase)*24.0*3600.0, RV1.R, RV1.V, gravref, hMoon);
			RV1.gravref = hMoon;
			RV1.MJD = GETbase + TLCC_TIG / 24.0 / 3600.0;
			opt.useSV = true;

			UY = unit(crossp(RV1.V, RV1.R));
			UZ = unit(-RV1.R);
			UX = crossp(UY, UZ);

			DV = UX*TLCC_dV_LVLH.x + UY*TLCC_dV_LVLH.y + UZ*TLCC_dV_LVLH.z;
			V2 = RV1.V + DV;

			opt.RV_MCC = RV1;
			opt.RV_MCC.V = V2;

			rtcc->LOITargeting(&opt, LOI_dV_LVLH, LOI_TIG);
		}
		else
		{
			rtcc->LOITargeting(&opt, LOI_dV_LVLH, LOI_TIG);
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
		bool endi = false;

		while (!endi)
		{
			endi = entry->EntryIter();
		}

		entryprecision = entry->precision;

		if (entryprecision != 9)
		{

			EntryLatcor = entry->EntryLatcor;
			EntryLngcor = entry->EntryLngcor;

			EntryRET05 = entry->EntryRET;
			EntryRTGO = entry->EntryRTGO;
			EntryVIO = entry->EntryVIO;
			EntryAngcor = entry->EntryAng;
			Entry_DV = entry->Entry_DV;
			maneuverplanet = entry->SOIplan;//oapiGetObjectByName("Earth");
			P37GET400K = entry->t2;


			VECTOR3 R0, V0, R, V, DV, Llambda, UX, UY, UZ, R_cor, V_cor, i, j, k;
			MATRIX3 Rot, mat, Q_Xx;
			double SVMJD, t_slip, mu;


			vessel->GetRelativePos(gravref, R0);
			vessel->GetRelativeVel(gravref, V0);
			SVMJD = oapiGetSimMJD();

			Rot = OrbMech::J2000EclToBRCS(40222.525);

			R0 = mul(Rot, _V(R0.x, R0.z, R0.y));
			V0 = mul(Rot, _V(V0.x, V0.z, V0.y));

			OrbMech::oneclickcoast(R0, V0, SVMJD, entry->EntryTIGcor - (SVMJD - GETbase) * 24.0 * 60.0 * 60.0, R, V, gravref, gravref);

			UY = unit(crossp(V, R));
			UZ = unit(-R);
			UX = crossp(UY, UZ);

			DV = UX*Entry_DV.x + UY*Entry_DV.y + UZ*Entry_DV.z;

			mat = _M(UX.x, UY.x, UZ.x, UX.y, UY.y, UZ.y, UX.z, UY.z, UZ.z);
			DV = mul(mat, Entry_DV);

			OrbMech::impulsive(vessel, R, V, GETbase + entry->EntryTIGcor/24.0/3600.0, gravref, vessel->GetGroupThruster(THGROUP_MAIN, 0), DV, Llambda, t_slip);

			mu = GGRAV*oapiGetMass(gravref);
			OrbMech::rv_from_r0v0(R, V, t_slip, R_cor, V_cor, mu);

			j = unit(crossp(V_cor, R_cor));
			k = unit(-R_cor);
			i = crossp(j, k);
			Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

			EntryTIGcor = entry->EntryTIGcor + t_slip;
			P30TIG = EntryTIGcor;
			dV_LVLH = mul(Q_Xx, Llambda);
			//dV_LVLH = tmul(mat, Llambda);
		}
		entrycalcstate = 0;
		delete entry;

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
		rtcc->TLI_PAD(&opt, tlipad);
		Result = 0;
	}
	break;
	}
	subThreadStatus = Result;
	// Printing messages from the subthread is not safe, but this is just for testing.
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