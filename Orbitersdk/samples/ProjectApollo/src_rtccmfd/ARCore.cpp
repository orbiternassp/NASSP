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
	vesseltype = 0;
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
		vesseltype = 2;
	}
	else if (strcmp(v->GetName(), "Yankee-Clipper") == 0)
	{
		mission = 12;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 72.0*RAD);
	}
	else if (strcmp(v->GetName(), "Kitty-Hawk") == 0)
	{
		mission = 14;
		REFSMMAT = OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, LaunchMJD[mission - 7], 75.558*RAD);
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
	if (vesseltype == 0)
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
	returnspeed = 1;
	TEItype = 0;
	TEIfail = false;
	entrynominal = 1;
	entrycalcstate = 0;
	entryrange = 0.0;
	EntryRTGO = 0.0;
	SVSlot = true; //true = CSM; false = Other
	BRCSPos = _V(0.0, 0.0, 0.0);
	BRCSVel = _V(0.0, 0.0, 0.0);
	BRCSGET = 0.0;
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
	TimeTag = 0.0;
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

	rtcc = new RTCC();

	LOImaneuver = 4;
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

	LmkLat = 0;
	LmkLng = 0;
	LmkTime = 0;
	LmkT1 = 0;
	LmkT2 = 0;
	LmkRange = 0;
	LmkN89Alt = 0;
	LmkN89Lat = 0;

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
		}
		else {
			//g_Data.progState = PROGSTATE_TLI_WAITING;
		}
	}
}

void ARCore::LmkCalc()
{
	VECTOR3 RA0_orb, VA0_orb, RA0, VA0, R_P, RA1, VA1, u;
	MATRIX3 Rot;
	double SVMJD, dt1, dt2, get, MJDguess, sinl, gamma, r_0;
	OBJHANDLE hEarth, hMoon;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	vessel->GetRelativePos(gravref, RA0_orb);
	vessel->GetRelativeVel(gravref, VA0_orb);
	SVMJD = oapiGetSimMJD();
	get = (SVMJD - GETbase)*24.0*3600.0;
	MJDguess = GETbase + LmkTime / 24.0 / 3600.0;

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	RA0 = mul(Rot, _V(RA0_orb.x, RA0_orb.z, RA0_orb.y));
	VA0 = mul(Rot, _V(VA0_orb.x, VA0_orb.z, VA0_orb.y));

	R_P = unit(_V(cos(LmkLng)*cos(LmkLat), sin(LmkLat), sin(LmkLng)*cos(LmkLat)))*oapiGetSize(gravref);

	OrbMech::oneclickcoast(RA0, VA0, SVMJD, LmkTime - get, RA1, VA1, gravref, gravref);

	dt1 = OrbMech::findelev_gs(RA1, VA1, R_P, MJDguess, 180.0*RAD, gravref, LmkRange);
	dt2 = OrbMech::findelev_gs(RA1, VA1, R_P, MJDguess, 145.0*RAD, gravref, LmkRange);

	LmkT1 = dt1 + (MJDguess - GETbase) * 24.0 * 60.0 * 60.0;
	LmkT2 = dt2 + (MJDguess - GETbase) * 24.0 * 60.0 * 60.0;

	u = unit(_V(R_P.x, R_P.z, R_P.y));
	sinl = u.z;
	
	if (gravref == hEarth)
	{
		double a, b, r_F;
		a = 6378166;
		b = 6356784;
		gamma = b*b / a / a;
		r_F = sqrt(b*b / (1.0 - (1.0 - b*b / a / a)*(1.0 - sinl*sinl)));
		r_0 = r_F;
	}
	else
	{
		gamma = 1.0;
		r_0 = oapiGetSize(gravref);
	}
	LmkN89Lat = atan2(u.z, gamma*sqrt(u.x*u.x + u.y*u.y));
	LmkN89Alt = length(R_P) - r_0;
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

		MATRIX3 Rot;
		VECTOR3 R0B, V0B;
		double t_lng;

		hEarth = oapiGetObjectByName("Earth");
		hSun = oapiGetObjectByName("Sun");

		ttoLOS = OrbMech::sunrise(R, V, MJD, gravref, hEarth, 0, 0, true);
		ttoAOS = OrbMech::sunrise(R, V, MJD, gravref, hEarth, 1, 0, true);

		LOSGET = (MJD - GETbase)*24.0*3600.0 + ttoLOS;
		AOSGET = (MJD - GETbase)*24.0*3600.0 + ttoAOS;

		ttoSS = OrbMech::sunrise(R, V, MJD, gravref, hSun, 0, 0, true);
		ttoSR = OrbMech::sunrise(R, V, MJD, gravref, hSun, 1, 0, true);

		SSGET = (MJD - GETbase)*24.0*3600.0 + ttoSS;
		SRGET = (MJD - GETbase)*24.0*3600.0 + ttoSR;

		Rot = OrbMech::J2000EclToBRCS(40222.525);

		R0B = mul(Rot, _V(R.x, R.z, R.y));
		V0B = mul(Rot, _V(V.x, V.z, V.y));

		t_lng = OrbMech::P29TimeOfLongitude(R0B, V0B, MJD, gravref, -150.0*RAD);
		ttoPM = (t_lng - MJD)*24.0 * 3600.0;
		//ttoPM = OrbMech::findlongitude(R, V, MJD, gravref, -150.0 * RAD);
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
		OrbAdjOpt opt;

		opt.GETbase = GETbase;
		opt.gravref = gravref;
		opt.h_apo = apo_desnm * 1852.0;
		opt.h_peri = peri_desnm * 1852.0;
		opt.impulsive = RTCC_NONIMPULSIVE;
		opt.inc = incdeg*RAD;
		opt.SPSGET = SPSGET;
		opt.vessel = vessel;
		opt.useSV = false;
		if (vesseltype == 0)
		{
			opt.csmlmdocked = 0;
		}
		else if (vesseltype == 1)
		{
			opt.csmlmdocked = 1;
		}

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
			if (vesseltype < 2)
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
		double MJDcut;

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

		if (vesseltype == 0)
		{
			opt.csmlmdocked = 0;
		}
		else if (vesseltype == 1)
		{
			opt.csmlmdocked = 1;
		}

		if (LOImaneuver == 0 || LOImaneuver == 4)
		{
			rtcc->LOITargeting(&opt, TLCC_dV_LVLH, TLCC_TIG, R_TLI, V_TLI, MJDcut);
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

			rtcc->LOITargeting(&opt, LOI_dV_LVLH, LOI_TIG, R_TLI, V_TLI, MJDcut);
		}
		else
		{
			rtcc->LOITargeting(&opt, LOI_dV_LVLH, LOI_TIG, R_TLI, V_TLI, MJDcut);
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

		if (entrycalcmode == 3)
		{
			VECTOR3 Llambda, R_cor, V_cor, i, j, k, Rcut, Vcut;
			MATRIX3 Q_Xx;
			double t_slip, mu, f_T, isp, tcut;

			while (!endi)
			{
				endi = teicalc->TEIiter();
			}
			entryprecision = teicalc->precision;
			EntryLatcor = teicalc->EntryLatcor;
			EntryLngcor = teicalc->EntryLngcor;
			//Entry_DV = teicalc->Entry_DV;
			maneuverplanet = oapiGetObjectByName("Moon");
			P37GET400K = (teicalc->EIMJD - GETbase)*24.0*3600.0;//teicalc->TIG_imp + teicalc->dt;
			EntryAngcor = teicalc->EntryAng;

			f_T = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
			isp = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_MAIN, 0));

			//OrbMech::impulsive(vessel, teicalc->Rguess, teicalc->Vguess, GETbase + P30TIG / 24.0 / 3600.0, gravref, f_T, isp, vessel->GetMass(), teicalc->V1B_apo - teicalc->Vguess, Llambda, t_slip);
			OrbMech::impulsive(teicalc->Rig, teicalc->Vig, teicalc->TIG, gravref, f_T, isp, vessel->GetMass(), teicalc->Vig_apo - teicalc->Vig, Llambda, t_slip, Rcut, Vcut, tcut);

			mu = GGRAV*oapiGetMass(gravref);
			//OrbMech::rv_from_r0v0(teicalc->Rguess, teicalc->Vguess, t_slip, R_cor, V_cor, mu);
			OrbMech::rv_from_r0v0(teicalc->Rig, teicalc->Vig, t_slip, R_cor, V_cor, mu);

			j = unit(crossp(V_cor, R_cor));
			k = unit(-R_cor);
			i = crossp(j, k);
			Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

			//EntryTIGcor = teicalc->TIG_imp + t_slip;
			EntryTIGcor = (teicalc->TIG - GETbase)*24.0*3600.0 + t_slip;
			P30TIG = EntryTIGcor;
			dV_LVLH = mul(Q_Xx, Llambda);
			Entry_DV = dV_LVLH;

			entrycalcstate = 0;
			delete teicalc;
		}
		else
		{
			while (!endi)
			{
				endi = entry->EntryIter();
			}

			entryprecision = entry->precision;

			if (entryprecision != 9)
			{

				EntryLatcor = entry->EntryLatcor;
				EntryLngcor = entry->EntryLngcor;

				//EntryRET05 = entry->EntryRET;
				EntryRTGO = entry->EntryRTGO;
				//EntryVIO = entry->EntryVIO;
				EntryAngcor = entry->EntryAng;
				Entry_DV = entry->Entry_DV;
				maneuverplanet = entry->SOIplan;//oapiGetObjectByName("Earth");
				P37GET400K = entry->t2;


				VECTOR3 R0, V0, R, V, DV, Llambda, UX, UY, UZ, R_cor, V_cor, i, j, k, Rcut, Vcut;
				MATRIX3 Rot, mat, Q_Xx;
				double SVMJD, t_slip, mu, f_T, isp, tcut;


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

				f_T = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
				isp = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_MAIN, 0));

				OrbMech::impulsive(R, V, GETbase + entry->EntryTIGcor / 24.0 / 3600.0, gravref, f_T, isp, vessel->GetMass(), DV, Llambda, t_slip, Rcut, Vcut, tcut);

				mu = GGRAV*oapiGetMass(gravref);
				OrbMech::rv_from_r0v0(R, V, t_slip, R_cor, V_cor, mu);

				j = unit(crossp(V_cor, R_cor));
				k = unit(-R_cor);
				i = crossp(j, k);
				Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

				EntryTIGcor = entry->EntryTIGcor + t_slip;
				P30TIG = EntryTIGcor;
				dV_LVLH = mul(Q_Xx, Llambda);
				Entry_DV = dV_LVLH;
				//dV_LVLH = tmul(mat, Llambda);
			}
			entrycalcstate = 0;
			delete entry;
		}

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
		opt.SeparationAttitude = _V(0.0*RAD, -120.0*RAD, 0.0);
		rtcc->TLI_PAD(&opt, tlipad);
		Result = 0;
	}
	break;
	case 9: //Maneuver PAD
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

		rtcc->AP11ManeuverPAD(&opt, manpad);
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