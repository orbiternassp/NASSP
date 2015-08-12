#include "ARCore.h"

static WSADATA wsaData;
static SOCKET m_socket;
static sockaddr_in clientService;
static SOCKET close_Socket = INVALID_SOCKET;
static char debugString[100];
static char debugStringBuffer[100];
static char debugWinsock[100];


ARCore::ARCore(VESSEL* v)
{
	time_mode = 0;
	T1 = 0;	
	T2 = 0;
	CDHtime = 0;
	CDHtime_cor = 0;
	CDHtimemode = 1;
	DH = 0;
	N = 0;
	uni = 0;
	offsetuni = 0;
	orient = 1;
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
	iterator = 0;
	IterStage = 0;
	dvdisplay = 1;

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
	EntryLatPred = 0.0;
	EntryLngPred = 0.0;
	EntryAng = 0.0;
	EntryAngcor = 0.0;
	Entry_DV = _V(0.0, 0.0, 0.0);
	entrycalcmode = 0; //0=LEO mode with angle and longitude, 1=Entry Prediction
	entrycritical = 1; //0 = Fuel critical, 1 = time critical, 2 = Abort
	entrycalcstate = 0;
	SVSlot = true; //true = CSM; false = Other
	BRCSPos = _V(0.0, 0.0, 0.0);
	BRCSVel = _V(0.0, 0.0, 0.0);
	BRCSGET = 0.0;
	Mantrunnion = 0.0;
	Manshaft = 0.0;
	Manstaroct = 0;
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
	entrypadopt = 0; //0 = Earth Entry Update, 1 = Lunar Entry
	EntryPADdirect = false; //false = Entry PAD with MCC/Deorbit burn, true = Direct Entry
	EntryPADRRT = 0.0;
	EntryPADLng = 0.0;
	EntryPADLat = 0.0;
	EntryPADGMax = 0.0;
	ManPADSPS = true;
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
}

void ARCore::MinorCycle(double SimT, double SimDT, double mjd)
{
	if (iterator == 1)
	{
		OrbitAdjustCalc();
	}
	else if (iterator == 2)
	{
		//lambertcalc();
	}
	if (entrycalcstate == 1)
	{
		bool endi;
		endi = entry->EntryIter();
		if (endi)
		{
			EntryLatcor = entry->EntryLatcor;
			EntryLngcor = entry->EntryLngcor;
			
			EntryRET05 = entry->EntryRET;
			EntryRTGO = entry->EntryRTGO;
			EntryVIO = entry->EntryVIO;
			EntryAngcor = entry->EntryAng;
			Entry_DV = entry->Entry_DV;
			maneuverplanet = oapiGetObjectByName("Earth");
			P37GET400K = entry->t2;

			VECTOR3 R0, V0, R, V, DV, Llambda, UX, UY, UZ, V_G, DV_P, DV_C;
			MATRIX3 Rot, mat;
			double SVMJD,theta_T,t_slip;


			vessel->GetRelativePos(gravref, R0);
			vessel->GetRelativeVel(gravref, V0);
			SVMJD = oapiGetSimMJD();

			Rot = OrbMech::J2000EclToBRCS(40222.525);

			R0 = mul(Rot, _V(R0.x, R0.z, R0.y));
			V0 = mul(Rot, _V(V0.x, V0.z, V0.y));

			OrbMech::oneclickcoast(R0, V0, SVMJD, entry->EntryTIGcor - (SVMJD - GETbase) * 24.0 * 60.0 * 60.0, R, V, gravref,gravref);

			UY = unit(crossp(V, R));
			UZ = unit(-R);
			UX = crossp(UY, UZ);

			DV = UX*Entry_DV.x + UY*Entry_DV.y + UZ*Entry_DV.z;

			mat = _M(UX.x, UY.x, UZ.x, UX.y, UY.y, UZ.y, UX.z, UY.z, UZ.z);
			DV = mul(mat,Entry_DV);

			impulsive(R, V, gravref, vessel->GetGroupThruster(THGROUP_MAIN, 0), DV, Llambda, t_slip);

			EntryTIGcor = entry->EntryTIGcor+t_slip;
			P30TIG = EntryTIGcor;
			dV_LVLH = tmul(mat, Llambda);
			entrycalcstate = 0;
			delete entry;

			/*DV_P = UX*dV_LVLH.x + UZ*dV_LVLH.z;
			theta_T = length(crossp(R, V))*length(dV_LVLH)*vessel->GetMass() / power(length(R), 2.0) / 92100.0;
			DV_C = (unit(DV_P)*cos(-theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(-theta_T / 2.0))*length(DV_P);
			V_G = DV_C + UY*dV_LVLH.y;
			dV_LVLH = tmul(mat, V_G);*/


			/*double v_e,F,t_b;
			v_e = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
			F = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
			t_b = v_e / F*vessel->GetMass()*(1.0 - exp(-length(Entry_DV) / v_e));
			EntryTIGcor = entry->EntryTIGcor-0.5*t_b;
			P30TIG = EntryTIGcor;
			dV_LVLH = Entry_DV;
			entrycalcstate = 0;
			delete entry;*/
		}
	}
	else if (entrycalcstate == 2)
	{
		entry->EntryUpdateCalc();
		EntryLatPred = entry->EntryLatPred;
		EntryLngPred = entry->EntryLngPred;
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
}

void ARCore::CDHcalc()			//Calculates the required DV vector of a coelliptic burn
{
	double mu, DH_met;
	double SVMJD, SVtime, dt, dt2, c1, c2, theta, SW, dh_CDH, VPV, dt2_apo;
	VECTOR3 RA0_orb, VA0_orb, RP0_orb, VP0_orb;
	VECTOR3 RA0, VA0, RP0, VP0, RA2_alt, VA2_alt, V_A2_apo;
	VECTOR3 u, RPC, VPC, i, j, k;
	double epsilon, a_P, a_A, V_AH, V_AV;
	MATRIX3 Q_Xx;
	bool stop;
	MATRIX3 obli;
	VECTOR3 RA2, VA2, RP2, VP2;

	maneuverplanet = gravref;

	mu = GGRAV*oapiGetMass(gravref);

		DH_met = DH*1852.0;							//Calculates the desired delta height of the coellitpic orbit in metric units

		vessel->GetRelativePos(gravref, RA0_orb);	//vessel position vector
		vessel->GetRelativeVel(gravref, VA0_orb);	//vessel velocity vector
		target->GetRelativePos(gravref, RP0_orb);	//target position vector
		target->GetRelativeVel(gravref, VP0_orb);	//target velocity vector
		SVtime = oapiGetSimTime();					//The time mark of the state vectors

		oapiGetPlanetObliquityMatrix(gravref, &obli);

		RA0_orb = mul(OrbMech::inverse(obli), RA0_orb);
		VA0_orb = mul(OrbMech::inverse(obli), VA0_orb);
		RP0_orb = mul(OrbMech::inverse(obli), RP0_orb);
		VP0_orb = mul(OrbMech::inverse(obli), VP0_orb);

		RA0 = _V(RA0_orb.x, RA0_orb.z, RA0_orb.y);	//Coordinate system nightmare. My calculation methods use another system
		VA0 = _V(VA0_orb.x, VA0_orb.z, VA0_orb.y);
		RP0 = _V(RP0_orb.x, RP0_orb.z, RP0_orb.y);
		VP0 = _V(VP0_orb.x, VP0_orb.z, VP0_orb.y);

		SVMJD = oapiTime2MJD(SVtime);				//Calculates the MJD of the state vector

		if (CDHtimemode == 0)
		{
			CDHtime_cor = CDHtime;
			dt2 = CDHtime - (SVMJD - GETbase) * 24 * 60 * 60;
		}
		else
		{
			if (time_mode == 0)							//If the time mode is GET, calculate the dt in seconds
			{
				dt = CDHtime - (SVMJD - GETbase) * 24 * 60 * 60;
			}
			else if (time_mode == 1)					//If the time mode is MJD, calculate the dt in seconds
			{
				dt = (CDHtime - SVMJD) * 24 * 60 * 60;
			}
			else										////If the time mode is sim time, calculate the dt in seconds
			{
				dt = CDHtime - SVtime;
			}

			dt2 = dt + 10.0;							//A secant search method is used to find the time, when the desired delta height is reached. Other values might work better.

			while (abs(dt2 - dt) > 0.1)					//0.1 seconds accuracy should be enough
			{
				c1 = OrbMech::NSRsecant(RA0, VA0, RP0, VP0, SVMJD, dt, DH_met, gravref);		//c is the difference between desired and actual DH
				c2 = OrbMech::NSRsecant(RA0, VA0, RP0, VP0, SVMJD, dt2, DH_met, gravref);

				dt2_apo = dt2 - (dt2 - dt) / (c2 - c1)*c2;						//secant method
				dt = dt2;
				dt2 = dt2_apo;
			}

			CDHtime_cor = dt2 + (SVMJD - GETbase) * 24 * 60 * 60;		//the new, calculated CDH time

		}

		//OrbMech::rv_from_r0v0(RA0, VA0, dt2, RA2, VA2, mu);
		//OrbMech::rv_from_r0v0(RP0, VP0, dt2, RP2, VP2, mu);

		stop = false;
		coast = new CoastIntegrator(RA0, VA0, SVMJD, dt2, gravref, gravref); //Coasting Integration, active vessel state to CDH time
		while (stop == false)								//Not a single method, so that it could be used in framerate friendly iterations
		{
			stop = coast->iteration();
		}
		RA2 = coast->R2;
		VA2 = coast->V2;
		stop = false;
		delete coast;

		coast = new CoastIntegrator(RP0, VP0, SVMJD, dt2, gravref, gravref);
		while (stop == false)
		{
			stop = coast->iteration();
		}
		RP2 = coast->R2;
		VP2 = coast->V2;
		delete coast;

		//This block of equations is taken from the GSOP for Luminary document: 
		//http://www.ibiblio.org/apollo/NARA-SW/R-567-sec5-rev8-5.4-5.5.pdf
		//Pre-CDH Program, page 5.4-18
		u = unit(crossp(RP2, VP2));
		RA2_alt = RA2;
		VA2_alt = VA2;
		RA2 = unit(RA2 - u*dotp(RA2, u))*length(RA2);
		VA2 = unit(VA2 - u*dotp(VA2, u))*length(VA2);

		theta = acos(dotp(RA2, RP2) / length(RA2) / length(RP2));
		SW = OrbMech::sign(dotp(u, crossp(RP2, RA2)));
		theta = SW*theta;
		OrbMech::rv_from_r0v0_ta(RP2, VP2, theta, RPC, VPC, mu);
		dh_CDH = length(RPC) - length(RA2);

		if (CDHtimemode == 0)
		{
			DH = dh_CDH/1852.0;
		}

		VPV = dotp(VPC, RA2 / length(RA2));

		epsilon = (length(VPC)*length(VPC)) / 2 - mu / length(RPC);
		a_P = -mu / (2.0 * epsilon);
		a_A = a_P - dh_CDH;
		V_AV = VPV*OrbMech::power((a_P / a_A), 1.5);
		V_AH = sqrt(mu*(2.0 / length(RA2) - 1.0 / a_A) - (V_AV*V_AV));
		V_A2_apo = unit(crossp(u, RA2))*V_AH + unit(RA2)*V_AV;	//The desired velocity vector at CDH time
		//dV2 = V_A2_apo - VA2_alt;
		//Z = unit(-RA2);
		//Y = -u;
		//X = unit(crossp(Y, Z));
		//dV_CDHLV = mul(_M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z),dV2);

		//OrbMech::rv_from_r0v0(RA2, VA2, -30.0, RA2, VA2, mu);	//According to GSOP for Colossus Section 2 the uplinked DV vector is in LVLH coordinates 30 second before the TIG

		if (orient == 0)
		{
			i = RA2 / length(RA2);
			j = VA2 / length(VA2);
			k = crossp(i, j);
			Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);
		}
		else
		{
			k = -RA2 / length(RA2);
			j = crossp(VA2, RA2) / length(RA2) / length(VA2);
			i = crossp(j, k);
			Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);
		}
		CDHdeltaV = mul(Q_Xx, V_A2_apo - VA2_alt); //Calculates the desired P30 LVLH DV Vector

		VECTOR3 Llambda;double t_slip;
		impulsive(RA2, VA2_alt, gravref, vessel->GetGroupThruster(THGROUP_MAIN, 0), V_A2_apo - VA2_alt, Llambda, t_slip);
		CDHdeltaV = mul(Q_Xx, Llambda); //Calculates the desired P30 LVLH DV Vector
		P30TIG = CDHtime_cor + t_slip;
		dV_LVLH = CDHdeltaV;

		/*dV_LVLH = CDHdeltaV;
		P30TIG = CDHtime_cor;*/
}

void ARCore::lambertcalc()
{

	VECTOR3 RA0_orb, VA0_orb, RP0_orb, VP0_orb;
	VECTOR3 RA0, VA0, RP0, VP0, RA1, VA1, RP2, VP2, RP2off, H_P;
	double SVMJD, dt1, dt2;
	double mu,i_P;
	VECTOR3 VA1_apo;
	VECTOR3 i, j, k;
	MATRIX3 Q_Xx, Q_Xx2, obli;
	bool tgtprograde;

	maneuverplanet = gravref;

	mu = GGRAV*oapiGetMass(gravref);

	//if (IterStage == 0)
	//{

	oapiGetPlanetObliquityMatrix(gravref, &obli);//oapiGetRotationMatrix(gravref, &obli);

		vessel->GetRelativePos(gravref, RA0_orb);
		vessel->GetRelativeVel(gravref, VA0_orb);
		target->GetRelativePos(gravref, RP0_orb);
		target->GetRelativeVel(gravref, VP0_orb);

		//SVtime = oapiGetSimTime();
		SVMJD = oapiGetSimMJD();

		if (lambertopt == 1)//vessel->NonsphericalGravityEnabled() == true && N < 16)	//Revolution values higher than 15 have not successfully lead to convergence
		{
			RA0_orb = mul(OrbMech::inverse(obli), RA0_orb);	//Calculates the equatorial state vector from the ecliptic state vector
			VA0_orb = mul(OrbMech::inverse(obli), VA0_orb);
			RP0_orb = mul(OrbMech::inverse(obli), RP0_orb);
			VP0_orb = mul(OrbMech::inverse(obli), VP0_orb);
		}

		RA0 = _V(RA0_orb.x, RA0_orb.z, RA0_orb.y);	//The following equations use another coordinate system than Orbiter
		VA0 = _V(VA0_orb.x, VA0_orb.z, VA0_orb.y);
		RP0 = _V(RP0_orb.x, RP0_orb.z, RP0_orb.y);
		VP0 = _V(VP0_orb.x, VP0_orb.z, VP0_orb.y);

		if (time_mode == 0)	//GET time mode
		{
			dt1 = T1 - (SVMJD - GETbase) * 24.0 * 60.0 * 60.0;
			dt2 = T2 - T1;
		}
		else if (time_mode == 1)
		{
			dt1 = (T1 - SVMJD) * 24 * 60 * 60;
			dt2 = (T2 - T1) * 24 * 60 * 60;
		}
		else
		{
			//dt1 = T1 - SVtime; BROKEN BROKEN
			dt2 = T2 - T1;
		}

		if (lambertopt == 1)//vessel->NonsphericalGravityEnabled() == true && N < 16)
		{
			//OrbMech::rungeinteg(RA0, VA0, dt1, RA1, VA1, mu);
			OrbMech::oneclickcoast(RA0, VA0, SVMJD, dt1, RA1, VA1, gravref, gravref);
			//coast = new CoastIntegrator(RA0, VA0, SVMJD, dt1, gravref);
			//IterStage++;
			//}
			//else if (IterStage == 1)
			//{
			//bool stop;
			//stop = false;
			//while (stop == false)
			//{
			//	stop = coast->iteration();
			//}
			//RA1 = coast->R2;
			//VA1 = coast->V2;
			//sprintf(oapiDebugString(), "x %0.1f, y %0.1f, z %0.1f, x %0.1f, y %0.1f, z %0.1f", RA1.x, RA1.y, RA1.z, VA1.x, VA1.y, VA1.z);

			//delete coast;
			//OrbMech::rungeinteg(RP0, VP0, dt1+dt2, RP2, VP2, mu);
			OrbMech::oneclickcoast(RP0, VP0, SVMJD, dt1 + dt2, RP2, VP2, gravref, gravref);
			//coast = new CoastIntegrator(RP0, VP0, SVMJD, dt1 + dt2, gravref);
			//stop = false;
			//while (stop == false)
			//{
			//	stop = coast->iteration();
			//}
			//RP2 = coast->R2;
			//VP2 = coast->V2;
			//delete coast;
			//}
		}

	//if (vessel->NonsphericalGravityEnabled() == true && N < 15)
	//{
	//	OrbMech::rv_from_r0v0_obla(RA0, VA0, dt1, RA1, VA1, mu);
	//	OrbMech::rv_from_r0v0_obla(RP0, VP0, dt1 + dt2, RP2, VP2, mu);
	//}
		else
		{
			OrbMech::rv_from_r0v0(RA0, VA0, dt1, RA1, VA1, mu);
			OrbMech::rv_from_r0v0(RP0, VP0, dt1 + dt2, RP2, VP2, mu);
		}

	if (orient == 0)
	{
		i = RP2 / length(RP2);
		j = VP2 / length(VP2);
		k = crossp(i, j);
		Q_Xx2 = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);
		RP2off = RP2 + tmul(Q_Xx2, offvec);
	}
	else
	{
		k = -RP2 / length(RP2);
		j = crossp(VP2, RP2) / length(RP2) / length(VP2);
		i = crossp(j, k);
		Q_Xx2 = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);
		double angle;
		VECTOR3 yvec;
		//dhdiff = length(RP2) - sqrt(length(RP2)*length(RP2) - offvec.x*offvec.x);
		RP2off = RP2 + tmul(Q_Xx2, _V(0.0,offvec.y, offvec.z));
		angle = offvec.x / length(RP2);
		yvec = _V(Q_Xx2.m21, Q_Xx2.m22, Q_Xx2.m23);
		RP2off = OrbMech::RotateVector(yvec, -angle, RP2off);
	}

	H_P = crossp(RP0, VP0);
	i_P = acos(H_P.z / length(H_P));
	if (i_P < PI05)
	{
		tgtprograde = true;
	}
	else
	{
		tgtprograde = false;
	}

	if (lambertopt == 1)//vessel->NonsphericalGravityEnabled() == true && N < 16)
	{
		VA1_apo = OrbMech::Vinti(RA1, VA1, RP2off, SVMJD + dt1 / 24.0 / 3600.0, dt2, N, tgtprograde, gravref); //Vinti Targeting: For non-spherical gravity
	}
	else
	{
		VA1_apo = OrbMech::elegant_lambert(RA1, VA1, RP2off, dt2, N, tgtprograde, mu);	//Lambert Targeting
	}
	//OrbMech::rv_from_r0v0(RA1, VA1, -30.0, RA1, VA1, mu);	//According to GSOP for Colossus Section 2 the uplinked DV vector is in LVLH coordinates 30 second before the TIG
	if (orient == 0)
	{
		i = RA1 / length(RA1);
		j = VA1 / length(VA1);
		k = crossp(i, j);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);
	}
	else
	{
		k = -RA1 / length(RA1);
		j = crossp(VA1, RA1) / length(RA1) / length(VA1);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);
	}
	VECTOR3 Llambda;
	double t_slip;
	impulsive(RA1, VA1, gravref, vessel->GetGroupThruster(THGROUP_MAIN, 0), VA1_apo - VA1, Llambda,t_slip);

	LambertdeltaV = mul(Q_Xx,Llambda);
	dV_LVLH = LambertdeltaV;
	P30TIG = T1+t_slip;
	/*LambertdeltaV = mul(Q_Xx, VA1_apo - VA1);
	dV_LVLH = LambertdeltaV;
	P30TIG = T1;*/

}

void ARCore::OrbitAdjustCalc()	//Calculates the optimal velocity change to reach an orbit specified by apoapsis, periapsis and inclination
{
	double R_E, SPSMJD, SVMJD, mu, r, phi, lambda, apo, peri, inc, a, e, theta1, theta2, beta1, beta2, ll1, ll2, h, w11, w12, w21, w22, dlambda1, dlambda2, lambda11, lambda12;
	VECTOR3 u, RPOS, RVEL, Requ, Vequ, R2, V2,VX1,VX2,VX3,VX4,k,i,j,DVX1,DVX2,DVX3,DVX4,DVX;
	MATRIX3 obli,Q_Xx;

	maneuverplanet = gravref;

	obli = OrbMech::J2000EclToBRCS(40222.525);
	mu = GGRAV*oapiGetMass(gravref);									//Standard gravitational parameter GM
	SPSMJD = GETbase + SPSGET / 24.0 / 60.0 / 60.0;					//The MJD of the maneuver


	if (IterStage == 0)												//The calculation has 3 stages: setup, coasting integration and DV calculation
	{
		vessel->GetRelativePos(gravref, RPOS);							//The current position vecotr of the vessel in the ecliptic frame
		vessel->GetRelativeVel(gravref, RVEL);							//The current velocity vector of the vessel in the ecliptic frame
		SVMJD = oapiGetSimMJD();										//The time mark for this state vector

		Requ = mul(obli, _V(RPOS.x, RPOS.z, RPOS.y));
		Vequ = mul(obli, _V(RVEL.x, RVEL.z, RVEL.y));
		//oapiGetPlanetObliquityMatrix(gravref, &obli);							//Gets the Obliquity Matrix of the Earth. NOTE: Any resulting longitudes are in the fixed, non-rotating frame
		//Requ = mul(OrbMech::inverse(obli), RPOS);								//Calculate the equatorial position vector from the ecliptic state vector
		//Vequ = mul(OrbMech::inverse(obli), RVEL);								//Calculate the equatorial velocity vector from the ecliptic state vector

		//Requ = _V(Requ.x, Requ.z, Requ.y);								//Coordinate system nightmare, don't even ask
		//Vequ = _V(Vequ.x, Vequ.z, Vequ.y);								//More coordinate system nightmare
		coast = new CoastIntegrator(Requ, Vequ, SVMJD, (SPSMJD- SVMJD)*24.0*60.0*60.0, gravref, gravref);		//Creates a new coast integration object
		IterStage++;													//Stage 0 done, next iteration is stage 1

	}
	else if (IterStage == 1)
	{
		bool stop;														
		stop = coast->iteration();										//iteration of the class CoastIntegrator2 is an implementation of Nystrom's method
		if (stop)														//The iteration method will set bool to true, once the iteration is finished
		{
			IterStage++;												//Iteration done
		}
	}
	else if (IterStage == 2)
	{
		R2 = coast->R2;													//The resulting position vector of the coasting routine
		V2 = coast->V2;													//The resulting velocity vector of the coasting routine	
		delete coast;													//We have everything from the coasting routine we wanted, so let's delete it

		if (gravref == oapiGetObjectByName("Earth"))
		{
			R_E = 6373338.0;// OrbMech::fischer_ellipsoid(R2);				//The radius of the Earth according to the AGC. This is the radius at launch?
		}
		else
		{
			R_E = oapiGetSize(gravref);

			MATRIX3 Rot2;
			Rot2 = OrbMech::GetRotationMatrix2(oapiGetObjectByName("Moon"), SPSMJD);

			R2 = tmul(obli, R2);
			V2 = tmul(obli,V2);
			R2 = _V(R2.x, R2.z, R2.y);
			V2 = _V(V2.x, V2.z, V2.y);
			R2 = tmul(Rot2, R2);
			V2 = tmul(Rot2, V2);
			R2 = _V(R2.x, R2.z, R2.y);
			V2 = _V(V2.x, V2.z, V2.y);
		}

		//OrbMech::local_to_equ(R2, r, phi, lambda);							//Calculates the radius, latitude and longitude of the maneuver position
		u = unit(R2);
		r = length(R2);
		phi = atan(u.z / sqrt(u.x*u.x + u.y*u.y));
		lambda = atan2(u.y, u.x);

		apo = R_E + apo_desnm * 1852.0;									//calculates the apoapsis radius in the metric system
		peri = R_E + peri_desnm * 1852.0;								//calculates the periapsis radius in the metric system

		if (r > apo)													//If the maneuver radius is higher than the desired apoapsis, then we would get no solution
		{
			apo = r;													//sets the desired apoapsis to the current radius, so that we can calculate a maneuver
		}
		else if (r < peri)												//If the maneuver radius is lower than the desired periapsis, then we would also get no solution
		{
			peri = r;													//sets the desired periapsis to the current radius, so that we can calculate a maneuver
		}

		inc = incdeg*PI / 180.0;										//Calculates the desired equatorial inclination in radian
		a = (apo + peri) / 2.0;											//The semi-major axis of the desired orbit
		e = (apo - peri) / (apo + peri);								//The eccentricity of the desired orbit

		theta1 = acos(min(1.0, max(-1.0, (a / r*(1.0 - e*e) - 1.0) / e)));	//The true anomaly of the desired orbit, min and max just to make sure this value isn't out of bounds for acos
		theta2 = PI2 - theta1;											//Calculates the second possible true anomaly of the desired orbit

		beta1 = asin(cos(inc) / cos(phi));									//Calculates the azimuth heading of the desired orbit at the current position. TODO: if phi > inc we get no solution
		beta2 = PI - beta1;													//The second possible azimuth heading

		ll1 = atan2(tan(phi),cos(beta1));    //angular distance between the ascending node and the current position (beta1)
		ll2 = atan2(tan(phi),cos(beta2));    //angular distance between the ascending node and the current position (beta2)	

		h = sqrt(r*mu*(1.0 + e*cos(theta1)));    //Specific relative angular momentum (theta1)
		//h2 = sqrt(r*mu*(1.0 + e*cos(theta2)));    //Specific relative angular momentum (theta2)

		w11 = ll1 - theta1;                     //argument of periapsis (beta1, theta1)
		w12 = ll1 - theta2;                     //argument of periapsis (beta1, theta2)
		w21 = ll2 - theta1;                     //argument of periapsis (beta2, theta1)
		w22 = ll2 - theta2;                     //argument of periapsis (beta2, theta2)
		//w = w1;

		//dlambda1 = atan2(sin(phi),1.0/tan(beta1));   //angular distance between the ascending node and the current position measured in the equatorial plane (beta1)
		//dlambda2 = atan2(sin(phi),1.0/tan(beta2));   //angular distance between the ascending node and the current position measured in the equatorial plane (beta2)
		dlambda1 = atan(tan(beta1)*sin(phi));
		dlambda2 = atan(tan(beta2)*sin(phi))+PI;

		//dlambda1 = atan2(tan(beta1), 1.0 / sin(phi));
		//dlambda2 = atan2(tan(beta2), 1.0 / sin(phi));

		lambda11 = lambda - dlambda1;               //longitude at the ascending node (beta1)
		lambda12 = lambda - dlambda2;               //longitude at the ascending node (beta2)

		VECTOR3 RX1, RX2, RX3, RX4;

		OrbMech::perifocal(h, mu, e, theta1, inc, lambda11, w11, RX1, VX1); //The required velocity vector for the desired orbit (beta1, theta1)
		OrbMech::perifocal(h, mu, e, theta2, inc, lambda11, w12, RX2, VX2); //The required velocity vector for the desired orbit (beta1, theta2)
		OrbMech::perifocal(h, mu, e, theta1, inc, lambda12, w21, RX3, VX3); //The required velocity vector for the desired orbit (beta2, theta1)
		OrbMech::perifocal(h, mu, e, theta2, inc, lambda12, w22, RX4, VX4); //The required velocity vector for the desired orbit (beta2, theta2)

		//OrbMech::rv_from_r0v0(R2, V2, -30.0, R2, V2, mu);	//According to GSOP for Colossus Section 2 the uplinked DV vector is in LVLH coordinates 30 second before the TIG

		k = -R2 / length(R2);											
		j = crossp(V2, R2) / length(R2) / length(V2);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);			//Creates the rotation matrix from the geocentric equatorial frame to the vessel-centered P30 LVLH frame

		DVX1 = VX1 - V2;									//Calculates the DV vectors to achieve the desired orbit
		DVX2 = VX2 - V2;
		DVX3 = VX3 - V2;
		DVX4 = VX4 - V2;

		if (length(DVX1) <= length(DVX2) && length(DVX1) <= length(DVX3) && length(DVX1) <= length(DVX4))		//The lowest DV vector is selected. TODO: Let the user choose it.
		{
			DVX = DVX1;
		}
		else if (length(DVX2) < length(DVX1) && length(DVX2) < length(DVX3) && length(DVX2) < length(DVX4))
		{
			DVX = DVX2;
		}
		else if (length(DVX3) <= length(DVX1) && length(DVX3) <= length(DVX2) && length(DVX3) <= length(DVX4))
		{
			DVX = DVX3;
		}
		else if (length(DVX4) < length(DVX1) && length(DVX4) < length(DVX2) && length(DVX4) < length(DVX3))
		{
			DVX = DVX4;
		}

		VECTOR3 Llambda; double t_slip;
		impulsive(R2, V2, gravref, vessel->GetGroupThruster(THGROUP_MAIN, 0), DVX, Llambda, t_slip);

		OrbAdjDVX = mul(Q_Xx, Llambda);		//The lowest DV vector is saved in the displayed DV vector
		iterator = 0;		//tells the program that the iteration is over
		IterStage = 0;		//resets the Orbit Adjustment method stage

		dV_LVLH = OrbAdjDVX;
		P30TIG = SPSGET+t_slip;
	}
	
}

void ARCore::REFSMMATCalc()
{
	double SVMJD, dt, mu;
	VECTOR3 R_A, V_A, R0B, V0B, R1B, V1B, UX, UY, UZ, X_B, DV, V2;
	MATRIX3 Rot, a;
	VECTOR3 DV_P, DV_C, V_G, X_SM, Y_SM, Z_SM;
	double theta_T,t_go;
	OBJHANDLE hMoon, hEarth;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");

	vessel->GetRelativePos(gravref, R_A);
	vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	R_A = mul(Rot, _V(R_A.x, R_A.z, R_A.y));
	V_A = mul(Rot, _V(V_A.x, V_A.z, V_A.y));

	if (REFSMMATdirect == false)
	{
		OrbMech::oneclickcoast(R_A, V_A, SVMJD, P30TIG - (SVMJD - GETbase) * 24.0 * 60.0 * 60.0, R0B, V0B, gravref, maneuverplanet);

		UY = unit(crossp(V0B, R0B));
		UZ = unit(-R0B);
		UX = crossp(UY, UZ);

		DV_P = UX*dV_LVLH.x + UZ*dV_LVLH.z;
		theta_T = length(crossp(R0B, V0B))*length(dV_LVLH)*vessel->GetMass() / OrbMech::power(length(R0B), 2.0) / 92100.0;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + UY*dV_LVLH.y;

		poweredflight(R0B, V0B, maneuverplanet, vessel->GetGroupThruster(THGROUP_MAIN, 0), V_G, R0B, V0B, t_go);

		//DV = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
		//V0B = V0B + DV;
		SVMJD += (t_go+P30TIG) / 24.0 / 60.0 / 60.0 - (SVMJD - GETbase);
	}
	else
	{
		R0B = R_A;
		V0B = V_A;
	}

	if (REFSMMATopt == 4)
	{
		if (mission == 7)
		{
			REFSMMAT = A7REFSMMAT;
		}
		else if (mission == 8)
		{
			REFSMMAT = A8REFSMMAT;
		}
	}
	else if (REFSMMATopt == 5)
	{
		double LSMJD;
		VECTOR3 R_P,R_LS, H_C;
		MATRIX3 Rot2;
		
		LSMJD = REFSMMATTime/24.0/3600.0 + GETbase;

		R_P = unit(_V(cos(LSLng)*cos(LSLat), sin(LSLat), sin(LSLng)*cos(LSLat)));

		Rot2 = OrbMech::GetRotationMatrix2(oapiGetObjectByName("Moon"), LSMJD);

		R_LS = mul(Rot2, R_P);
		R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));

		OrbMech::oneclickcoast(R0B, V0B, SVMJD, (LSMJD - SVMJD)*24.0*3600.0, R1B, V1B, gravref, hMoon);

		H_C = crossp(R1B, V1B);

		UX = unit(R_LS);
		UZ = unit(crossp(H_C, R_LS));
		UY = crossp(UZ, UX);

		REFSMMAT = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}
	else if (REFSMMATopt == 6)
	{
		double *MoonPos;
		double PTCMJD;
		VECTOR3 R_ME;

		MoonPos = new double[12];

		PTCMJD = REFSMMATTime / 24.0 / 3600.0 + GETbase;

		OBJHANDLE hMoon = oapiGetObjectByName("Moon");
		CELBODY *cMoon = oapiGetCelbodyInterface(hMoon);

		cMoon->clbkEphemeris(PTCMJD, EPHEM_TRUEPOS, MoonPos);

		R_ME = -_V(MoonPos[0], MoonPos[1], MoonPos[2]);

		UX = unit(crossp(_V(0.0, 1.0, 0.0), unit(R_ME)));
		UX = mul(Rot, _V(UX.x, UX.z, UX.y));

		UZ = unit(mul(Rot, _V(0.0, 0.0, -1.0)));
		UY = crossp(UZ, UX);
		REFSMMAT = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}
	else
	{
		mu = GGRAV*oapiGetMass(gravref);

		if (REFSMMATopt == 2)
		{
			dt = REFSMMATTime - (SVMJD - GETbase) * 24.0 * 60.0 * 60.0;
			OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, gravref);
		}
		else if (REFSMMATopt == 0 || REFSMMATopt == 1)
		{
			dt = P30TIG - (SVMJD - GETbase) * 24.0 * 60.0 * 60.0;
			OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, gravref);
		}
		else
		{
			dt = OrbMech::time_radius_integ(R0B, V0B, SVMJD, oapiGetSize(hEarth) + 400000.0*0.3048, -1, hEarth, hEarth, R1B, V1B);
		}

		UY = unit(crossp(V1B, R1B));
		UZ = unit(-R1B);
		UX = crossp(UY, UZ);



		if (REFSMMATopt == 0 || REFSMMATopt == 1)
		{
			MATRIX3 M, M_R, M_RTM;
			double p_T, y_T;

			DV_P = UX*dV_LVLH.x + UZ*dV_LVLH.z;
			if (length(DV_P) != 0.0)
			{
				theta_T = length(crossp(R1B, V1B))*length(dV_LVLH)*vessel->GetMass() / OrbMech::power(length(R1B), 2.0) / 92100.0;
				DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
				V_G = DV_C + UY*dV_LVLH.y;
			}
			else
			{
				V_G = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
			}
			if (REFSMMATopt == 0)
			{
				p_T = -2.15*RAD;
				X_B = unit(V_G);// tmul(REFSMMAT, dV_LVLH));
			}
			else
			{
				p_T = 2.15*RAD;
				X_B = -unit(V_G);// tmul(REFSMMAT, dV_LVLH));
			}
			UX = X_B;
			UY = unit(crossp(X_B, R1B));
			UZ = unit(crossp(X_B, crossp(X_B, R1B)));


			y_T = 0.95*RAD;

			M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
			M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
			M_RTM = mul(OrbMech::transpose_matrix(M_R), M);
			X_SM = mul(M_RTM, _V(1.0, 0.0, 0.0));
			Y_SM = mul(M_RTM, _V(0.0, 1.0, 0.0));
			Z_SM = mul(M_RTM, _V(0.0, 0.0, 1.0));
			REFSMMAT = _M(X_SM.x, X_SM.y, X_SM.z, Y_SM.x, Y_SM.y, Y_SM.z, Z_SM.x, Z_SM.y, Z_SM.z);

			//IMUangles = OrbMech::CALCGAR(REFSMMAT, mul(transpose_matrix(M), M_R));
			//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);
		}
		else
		{
			REFSMMAT = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		}
		//
	}

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
}

void ARCore::EntryPAD()
{
	VECTOR3 DV, R_A, V_A, R0B, V0B, R1B, V1B, UX, UY, UZ, V2, R05G, V05G,R3,V3;
	MATRIX3 Rot, M_R;
	double SVMJD, dt, mu, EMSAlt,dt2,dt3,S_FPA,g_T,V_T,v_BAR;
	OBJHANDLE hEarth;
	dt2 = 0;

	hEarth = oapiGetObjectByName("Earth");

	mu = GGRAV*oapiGetMass(hEarth);

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
		entry = new Entry(vessel, gravref, GETbase, EntryTIG, EntryAng, EntryLng, entrycritical);
		entry->EntryUpdateCalc();
		EntryPADLat = entry->EntryLatPred;
		EntryPADLng = entry->EntryLngPred;
		EntryPADRTGO = entry->EntryRTGO;
		EntryPADVIO = entry->EntryVIO;
		delete entry;
	}
	else
	{
		VECTOR3 DV_P, DV_C, V_G,R2B,V2B;
		double theta_T,t_go;

		EntryPADLng = EntryLngcor;
		EntryPADLat = EntryLatcor;

		DV = Entry_DV;
		dt = EntryTIGcor - (SVMJD - GETbase) * 24.0 * 60.0 * 60.0;


		OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, hEarth);

		UY = unit(crossp(V1B, R1B));
		UZ = unit(-R1B);
		UX = crossp(UY, UZ);

		//DV = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
		//V2 = V1B + DV;

		DV_P = UX*dV_LVLH.x + UZ*dV_LVLH.z;
		theta_T = length(crossp(R1B, V1B))*length(dV_LVLH)*vessel->GetMass() / OrbMech::power(length(R1B), 2.0) / 92100.0;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + UY*dV_LVLH.y;

		poweredflight(R1B, V1B, hEarth, vessel->GetGroupThruster(THGROUP_MAIN, 0), V_G, R2B, V2B, t_go);

		dt2 = OrbMech::time_radius_integ(R2B, V2B, SVMJD + dt / 3600.0 / 24.0, oapiGetSize(hEarth) + EMSAlt, -1, hEarth, hEarth, R05G, V05G);
		dt2 += t_go;
		//OrbMech::oneclickcoast(R1B, V2, SVMJD + dt / 3600.0 / 24.0, dt2, R05G, V05G, mu);
		//dt22 = OrbMech::time_radius(R05G, -V05G, oapiGetSize(gravref) + 400000.0*0.3048, 1,mu);
		//rv_from_r0v0(R05G, -V05G, dt22, REI, VEI, mu);
		//VEI = -VEI;
		EntryPADRTGO = EntryRTGO;
		EntryPADVIO = EntryVIO;
	}

	UX = unit(V05G);
	UY = unit(crossp(UX, R05G));
	UZ = unit(crossp(UX, crossp(UX, R05G)));

	double aoa = -157.0*RAD;
	VECTOR3 vunit = _V(0.0, 1.0, 0.0);
	MATRIX3 Rotaoa;

	Rotaoa = _M(1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0)*cos(aoa) + OrbMech::skew(vunit)*sin(aoa) + outerp(vunit,vunit)*(1.0 - cos(aoa));
	
	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	EIangles = OrbMech::CALCGAR(REFSMMAT, mul(Rotaoa,M_R));


	if (entrypadopt == 1)
	{
		dt3 = OrbMech::time_radius(R05G, -V05G, oapiGetSize(hEarth) + 300000.0*0.3048, 1, mu);
		OrbMech::rv_from_r0v0(R05G, -V05G, dt3, R3, V3, mu);
		V3 = -V3;
		S_FPA = dotp(unit(R3), V3) / length(V3);
		g_T = asin(S_FPA);
		V_T = length(V3);
		v_BAR = (V_T / 0.3048 - 36000.0) / 20000.0;
		EntryPADGMax = 4.0 / (1.0 + 4.8*v_BAR*v_BAR)*(abs(g_T)*DEG - 6.05 - 2.4*v_BAR*v_BAR) + 10.0;

		double dt22, S_FPA,vei,liftline,horang,coastang,cosIGA,sinIGA,IGA;
		VECTOR3 REI, VEI, UREI,REI17,VEI17,X_NB,Y_NB,Z_NB,X_SM,Y_SM,Z_SM,A_MG,Rcheck,Vcheck;

		dt22 = OrbMech::time_radius(R05G, -V05G, oapiGetSize(hEarth) + 400000.0*0.3048, 1, mu);
		OrbMech::rv_from_r0v0(R05G, -V05G, dt22, REI, VEI, mu);
		VEI = -VEI;
		UREI = unit(REI);
		EntryPADV400k = length(VEI);
		S_FPA = dotp(UREI, VEI) / EntryPADV400k;
		EntryPADgamma400k = asin(S_FPA);
		EntryPADRET05Lunar = dt22;
		EntryPADRRT = dt+dt2+(SVMJD-GETbase)*24.0*3600.0 - dt22;

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

		checkstar(REFSMMAT, _V(OrbMech::round(EIangles.x*DEG)*RAD, OrbMech::round(EIangles.y*DEG)*RAD, OrbMech::round(EIangles.z*DEG)*RAD), Rcheck, oapiGetSize(hEarth), Entrystaroct, Entrytrunnion, Entryshaft);

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

		EntryPADHorChkPit = PI2-(horang + coastang+0.0*31.7*RAD)+IGA;
	}
	else
	{
		EntryPADRET05Earth = EntryRET05;
	}
}

void ARCore::ManeuverPAD()
{
	VECTOR3 DV_P, DV_C, V_G;
	double SVMJD, dt, mu, theta_T,t_go;
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
	

	ManPADWeight = vessel->GetMass() / 0.45359237;

	double v_e, F;
	if (ManPADSPS)
	{
		v_e = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
		F = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
	}
	else
	{
		v_e = 2706.64;
		F = 400 * 4.448222;
	}

	ManPADBurnTime = v_e/F *vessel->GetMass()*(1.0-exp(-length(dV_LVLH)/v_e));

	if (HeadsUp)
	{
		headsswitch = 1.0;
	}
	else
	{
		headsswitch = -1.0;
	}

	if (ManPADSPS)
	{
		DV_P = UX*dV_LVLH.x + UZ*dV_LVLH.z;
		if (length(DV_P) != 0.0)
		{
			theta_T = length(crossp(R1B, V1B))*length(dV_LVLH)*vessel->GetMass() / OrbMech::power(length(R1B), 2.0) / 92100.0;
			DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
			V_G = DV_C + UY*dV_LVLH.y;
		}
		else
		{
			V_G = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
		}

		U_TD = unit(V_G);

		poweredflight(R1B, V1B, gravref, vessel->GetGroupThruster(THGROUP_MAIN, 0), V_G, R2B, V2B, t_go);

		OrbMech::periapo(R2B, V2B, mu, apo, peri);
		ManPADApo = apo - oapiGetSize(gravref);
		ManPADPeri = peri - oapiGetSize(gravref);

		p_T = -2.15*RAD;
		y_T = 0.95*RAD;

		X_B = unit(V_G);
		UX = X_B;
		UY = unit(crossp(X_B, R1B*headsswitch));
		UZ = unit(crossp(X_B, crossp(X_B, R1B*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
		M_RTM = mul(OrbMech::transpose_matrix(M_R), M);

		m1 = vessel->GetMass()*exp(-length(dV_LVLH) / v_e);
		ManPADDVC = length(dV_LVLH)*cos(-2.15*RAD)*cos(0.95*RAD) - 60832.18 / m1;
	}
	else
	{
		OrbMech::periapo(R1B, V1B+DV, mu, apo, peri);
		ManPADApo = apo - oapiGetSize(gravref);
		ManPADPeri = peri - oapiGetSize(gravref);

		V_G = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
		X_B = unit(V_G);
		UX = X_B;
		UY = unit(crossp(X_B, R1B*headsswitch));
		UZ = unit(crossp(X_B, crossp(X_B, R1B*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

		ManPADDVC = length(dV_LVLH);
	}
	IMUangles = OrbMech::CALCGAR(REFSMMAT, mul(OrbMech::transpose_matrix(M), M_R));
	//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);

	GDCangles = OrbMech::backupgdcalignment(REFSMMAT, R1B, oapiGetSize(gravref), GDCset);

	VECTOR3 Rsxt, Vsxt;

	OrbMech::oneclickcoast(R1B, V1B, SVMJD + dt / 24.0 / 3600.0, sxtstardtime*60.0, Rsxt, Vsxt, gravref, gravref);

	checkstar(REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD),Rsxt, oapiGetSize(gravref), Manstaroct, Mantrunnion, Manshaft);
}

void ARCore::TPIPAD()
{
	double mu, dt, SVMJD;
	VECTOR3 R_A, V_A, R0B, V0B, RA3, VA3, R_P, V_P, RP0B, VP0B, RP3, VP3,u,U_L, UX,UY, UZ, U_R,RA2,VA2,RP2,VP2,U_P;
	MATRIX3 Rot, Rot1, Rot2;

	mu = GGRAV*oapiGetMass(gravref);

	vessel->GetRelativePos(gravref, R_A);
	vessel->GetRelativeVel(gravref, V_A);
	target->GetRelativePos(gravref, R_P);
	target->GetRelativeVel(gravref, V_P);


	SVMJD = oapiGetSimMJD();

	//double ddtt = OrbMech::sunrise(R_A, V_A, SVMJD, gravref,true);
	//TimeTag = (SVMJD - GETbase)*24.0*3600.0 + ddtt;
	//TimeTagUplink();

	dt = P30TIG - (SVMJD - GETbase) * 24.0 * 60.0 * 60.0;

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	R0B = mul(Rot, _V(R_A.x, R_A.z, R_A.y));
	V0B = mul(Rot, _V(V_A.x, V_A.z, V_A.y));
	RP0B = mul(Rot, _V(R_P.x, R_P.z, R_P.y));
	VP0B = mul(Rot, _V(V_P.x, V_P.z, V_P.y));

	OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, RA3, VA3, gravref, gravref);
	OrbMech::oneclickcoast(RP0B, VP0B, SVMJD, dt, RP3, VP3, gravref, gravref);

	UY = unit(crossp(VA3, RA3));
	UZ = unit(-RA3);
	UX = crossp(UY, UZ);

	Rot1 = _M(UX.x, UY.x, UZ.x, UX.y, UY.y, UZ.y, UX.z, UY.z, UZ.z);

	u = unit(crossp(RA3, VA3));
	U_L = unit(RP3 - RA3);
	UX = U_L;
	UY = unit(crossp(crossp(u, UX), UX));
	UZ = crossp(UX, UY);

	Rot2 = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);

	U_R = unit(RP3-RA3);

	TPIPAD_dV_LOS = mul(Rot2, mul(Rot1, dV_LVLH));
	//TPIPAD_dH = abs(length(RP3) - length(RA3));
	double mass,F;

	mass = vessel->GetMass();
	F = 200.0 * 4.448222;
	TPIPAD_BT = _V(abs(0.5*TPIPAD_dV_LOS.x), abs(TPIPAD_dV_LOS.y), abs(TPIPAD_dV_LOS.z))*mass / F;

	VECTOR3 i, j, k, dr, dv, dr0, dv0,Omega;
	MATRIX3 Q_Xx;
	double t1, t2, dxmin, n, dxmax;

	j = unit(VP3);
	k = unit(crossp(RP3, j));
	i = crossp(j, k);
	Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);
	dr = RA3 - RP3;
	n = length(VP3) / length(RP3);
	Omega = k*n;
	dv = VA3 - VP3 - crossp(Omega, dr);
	dr0 = mul(Q_Xx, dr);
	dv0 = mul(Q_Xx, dv);
	t1 = 1.0 / n*atan(-dv0.x / (3.0*n*dr0.x + 2.0 * dv0.y));
	t2 = 1.0 / n*(atan(-dv0.x / (3.0*n*dr0.x + 2.0 * dv0.y))+PI);
	dxmax = 4.0 * dr0.x + 2.0 / n*dv0.y + dv0.x / n*sin(n*t1) - (3.0 * dr0.x + 2.0 / n*dv0.y)*cos(n*t1);
	dxmin = 4.0 * dr0.x + 2.0 / n*dv0.y + dv0.x / n*sin(n*t2) - (3.0 * dr0.x + 2.0 / n*dv0.y)*cos(n*t2);

	TPIPAD_dH = -dr0.x;
	TPIPAD_ddH = abs(dxmax - dxmin);
	TPIPAD_R = abs(length(RP3 - RA3));
	TPIPAD_Rdot = dotp(VP3 - VA3, U_R);

	OrbMech::oneclickcoast(RA3, VA3, SVMJD, -5.0*60.0, RA2, VA2, gravref, gravref);
	OrbMech::oneclickcoast(RP3, VP3, SVMJD, -5.0*60.0, RP2, VP2, gravref, gravref);

	U_L = unit(RP2 - RA2);
	U_P = unit(U_L - RA2*dotp(U_L, RA2) / length(RA2) / length(RA2));

	TPIPAD_ELmin5 = acos(dotp(U_L, U_P*OrbMech::sign(dotp(U_P, crossp(u, RA2)))));

	TPIPAD_AZ = atan2(-TPIPAD_dV_LOS.z, TPIPAD_dV_LOS.x);

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

	if (SVSlot || target == NULL)
	{
		vessel->GetRelativePos(gravref, R);
		vessel->GetRelativeVel(gravref, V);
	}
	else
	{
		target->GetRelativePos(gravref, R);
		target->GetRelativeVel(gravref, V);
	}
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
	g_Data.emem[2] = OrbMech::DoubleToBuffer(EntryLatcor/PI2, 0, 1);
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
	g_Data.emem[2] = OrbMech::DoubleToBuffer(EntryLatPred / PI2, 0, 1);
	g_Data.emem[3] = OrbMech::DoubleToBuffer(EntryLatPred / PI2, 0, 0);
	g_Data.emem[4] = OrbMech::DoubleToBuffer(EntryLngPred / PI2, 0, 1);
	g_Data.emem[5] = OrbMech::DoubleToBuffer(EntryLngPred / PI2, 0, 0);

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

void ARCore::checkstar(MATRIX3 REFSM, VECTOR3 IMU, VECTOR3 R_C, double R_E, int &staroct, double &trunnion, double &shaft)
{
	MATRIX3 SMNB,Q1,Q2,Q3, NBSB, SBNB;
	double OGA, IGA, MGA,a,cosSA,TA,SA, sinSA;
	VECTOR3 U_LOS, USTAR,Y_SB, Z_SB, S_SM, S_SB, U_TPA, X_SB;
	int star;

	/*if (REFSMMATopt == 0)
	{
		OGA = 0;
		IGA = 0;
		MGA = 0;
	}
	else
	{
		OGA = PI; //ROLL
		IGA = PI; //PITCH
		MGA = 0;  //YAW
	}*/

	OGA = IMU.x;
	IGA = IMU.y;
	MGA = IMU.z;

	Q1 = OrbMech::_MRy(IGA);
	Q2 = OrbMech::_MRz(MGA);
	Q3 = OrbMech::_MRx(OGA);

	SMNB = mul(Q3, mul(Q2, Q1));

	U_LOS = OrbMech::ULOS(REFSMMAT, SMNB);
	star = OrbMech::FindNearestStar(U_LOS, R_C, R_E);

	if (star == -1)
	{
		staroct = 0;

		trunnion = 0;
		shaft = 0;
	}
	else
	{

		USTAR = navstars[star];

		X_SB = _V(1.0, 0.0, 0.0);
		Z_SB = _V(0.0, 0.0, 1.0);
		Y_SB = _V(0.0, 1.0, 0.0);

		a = -0.5676353234;
		SBNB = _M(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));
		NBSB = OrbMech::transpose_matrix(SBNB);

		S_SM = mul(REFSMMAT, USTAR);
		S_SB = mul(NBSB, mul(SMNB, S_SM));
		U_TPA = unit(crossp(Z_SB, S_SB));
		sinSA = dotp(U_TPA, -X_SB);
		cosSA = dotp(U_TPA, Y_SB);
		SA = OrbMech::atan3(sinSA, cosSA);
		TA = acos(dotp(Z_SB, S_SB));

		staroct = OrbMech::decimal_octal(star + 1);

		trunnion = TA;
		shaft = SA;
	}

	//sprintf(oapiDebugString(), "%d, %f, %f", staroct, SA*DEG, TA*DEG);
}

void ARCore::impulsive(VECTOR3 R, VECTOR3 V, OBJHANDLE gravref, THRUSTER_HANDLE thruster, VECTOR3 DV, VECTOR3 &Llambda, double &t_slip)
{
	double mu, v_ex, f_T, m, a_T, tau, L,t_go,t_go_old,J,S,dr_z,v_goz,dt_go,r_goz, phi;
	VECTOR3 lambda, Slambda,R3,V3,V_apo,R_ref,V_ref,R_thrust, V_thrust, R_bias, V_bias, V_go,R_grav,R_go,R_d,i_z,i_y;
	VECTOR3 dR_c, dV_c,R_c1,V_c1,R_c2,V_c2,V_grav,R_p,V_d,V_go_apo,dV_go,R_ig,V_ig;
	int n, nmax;

	VECTOR3 X, Y, Z, dV_LV, DV_P, DV_C, V_G,R_goxy;
	MATRIX3 Q_Xx;

	t_slip = 0;
	dt_go = 1;
	mu = GGRAV*oapiGetMass(gravref);

	v_ex = vessel->GetThrusterIsp0(thruster);
	f_T = vessel->GetThrusterMax0(thruster);
	m = vessel->GetMass();
	a_T = f_T / m;
	tau = v_ex / a_T;
	V_apo = V + DV;
	R_ref = R;
	V_ref = V_apo;

	while (abs(dt_go) > 0.01)
	{
		OrbMech::rv_from_r0v0(R, V, t_slip, R_ig, V_ig, mu);

		n = 0;
		nmax = 100;

		V_go = DV;
		R_d = R_ig;

		R_bias = _V(0, 0, 0);
		R_grav = -R_ig*0.5 * mu / OrbMech::power(length(R_ig), 3.0);
		i_y = -unit(crossp(R_ref, V_ref));
		t_go = v_ex / f_T*m*(1.0 - exp(-length(V_go) / v_ex));
		dV_go = _V(1.0, 1.0, 1.0);

		while ((length(dV_go) > 0.01 || n < 2) && n <= nmax)
		{
			lambda = unit(V_go);
			L = length(V_go);
			t_go_old = t_go;
			t_go = tau*(1.0 - exp(-L / v_ex));

			J = tau*L - v_ex*t_go;
			S = -J + t_go*L;

			R_grav = R_grav*OrbMech::power(t_go / t_go_old, 2.0);
			R_go = R_d - (R_ig + V_ig*t_go + R_grav);
			i_z = unit(crossp(R_d, i_y));
			R_goxy = R_go - i_z*dotp(i_z, R_go);
			r_goz = (S - dotp(lambda, R_goxy)) / dotp(lambda, i_z);
			R_go = R_goxy + i_z*r_goz + R_bias;

			V_thrust = lambda*L;
			R_thrust = lambda*S;

			/*X = unit(crossp(crossp(R_ig, V_ig), R_ig));
			Y = unit(crossp(V_ig, R_ig));
			Z = -unit(R_ig);
			Q_Xx = _M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z);
			dV_LV = mul(Q_Xx, V_go);
			DV_P = X*dV_LV.x + Z*dV_LV.z;
			phi = length(crossp(R_ig, V_ig))*length(dV_LV)*m / OrbMech::power(length(R_ig), 2.0) / f_T;
			DV_C = (unit(DV_P)*cos(phi / 2.0) + unit(crossp(DV_P, Y))*sin(phi / 2.0))*length(DV_P);
			V_go = DV_C + Y*dV_LV.y;*/

			V_bias = V_go - V_thrust;
			R_bias = R_go - R_thrust;
			dR_c = -R_thrust*1.0 / 10.0 - V_thrust*t_go * 1.0 / 30.0;
			dV_c = R_thrust*6.0 / 5.0 / t_go - V_thrust*1.0 / 10.0;
			R_c1 = R_ig + dR_c;
			V_c1 = V_ig + dV_c;
			OrbMech::rv_from_r0v0(R_c1, V_c1, t_go, R_c2, V_c2, mu);
			V_grav = V_c2 - V_c1;
			R_grav = R_c2 - R_c1 - V_c1*t_go;
			R_p = R_ig + V_ig*t_go + R_grav + R_thrust;
			//V_d = V_ig + V_grav + V_thrust;
			OrbMech::rv_from_r0v0(R_ref, V_ref, t_go+t_slip, R_d, V_d, mu);
			dr_z = dotp(i_z, R_d - R_p);
			v_goz = dotp(i_z, V_go);
			dt_go = -2.0 * dr_z / v_goz;
			V_go_apo = V_d - V_ig - V_grav + V_bias;
			dV_go = V_go_apo - V_go;
			V_go = V_go + dV_go;
			n++;
		}
		t_slip+= dt_go*0.1;
	}


	//VECTOR3 X,Y,Z,dV_LV,DV_P,DV_C,V_G;
	
	//MATRIX3 Q_Xx;
	double theta_T;

	X = unit(crossp(crossp(R_ig, V_ig), R_ig));
	Y = unit(crossp(V_ig, R_ig));
	Z = -unit(R_ig);

	Q_Xx = _M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z);
	dV_LV = mul(Q_Xx, V_go);
	DV_P = X*dV_LV.x + Z*dV_LV.z;
	if (length(DV_P) != 0.0)
	{
		theta_T = -length(crossp(R_ig, V_ig))*length(dV_LV)*m / OrbMech::power(length(R_ig), 2.0) / f_T;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, Y))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + Y*dV_LV.y;
	}
	else
	{
		V_G = X*dV_LV.x + Y*dV_LV.y + Z*dV_LV.z;
	}
	Llambda = V_go;
}

void ARCore::poweredflight(VECTOR3 R, VECTOR3 V, OBJHANDLE gravref, THRUSTER_HANDLE thruster, VECTOR3 V_G, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &t_go)
{
	double v_ex, f_T,a_T, tau, m,L,S,J, mu,dV;
	VECTOR3 R_thrust, V_thrust, R_c1, V_c1, R_c2, V_c2, R_grav, V_grav,U_TD;

	dV = length(V_G);
	U_TD = unit(V_G);

	mu = GGRAV*oapiGetMass(gravref);

	v_ex = vessel->GetThrusterIsp0(thruster);
	f_T = vessel->GetThrusterMax0(thruster);
	m = vessel->GetMass();
	a_T = f_T / m;
	tau = v_ex / a_T;

	L = dV;
	t_go = tau*(1.0 - exp(-L / v_ex));
	J = L*tau - v_ex*t_go;
	S = -J + t_go*L;

	R_thrust = U_TD*S;
	V_thrust = U_TD*L;

	R_c1 = R - R_thrust*1.0 / 10.0 - V_thrust*1.0 / 30.0*t_go;
	V_c1 = V + R_thrust*6.0 / 5.0 / t_go - V_thrust*1.0 / 10.0;

	OrbMech::rv_from_r0v0(R_c1, V_c1, t_go, R_c2, V_c2, mu);

	V_grav = V_c2 - V_c1;
	R_grav = R_c2 - R_c1 - V_c1*t_go;

	R_cutoff = R + V*t_go + R_grav + R_thrust;
	V_cutoff = V + V_grav + V_thrust;
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