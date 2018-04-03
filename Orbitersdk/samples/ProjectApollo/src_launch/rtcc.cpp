/****************************************************************************
This file is part of Project Apollo - NASSP

Real-Time Computer Complex (RTCC)

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

// To force orbitersdk.h to use <fstream> in any compiler version
//#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "soundlib.h"
#include "ioChannels.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "papi.h"
#include "saturn.h"
#include "LEMcomputer.h"
#include "LEM.h"
#include "sivb.h"
#include "../src_rtccmfd/OrbMech.h"
#include "../src_rtccmfd/EntryCalculations.h"
#include "mcc.h"
#include "rtcc.h"

// SCENARIO FILE MACROLOGY
#define SAVE_BOOL(KEY,VALUE) oapiWriteScenario_int(scn, KEY, VALUE)
#define SAVE_INT(KEY,VALUE) oapiWriteScenario_int(scn, KEY, VALUE)
#define SAVE_DOUBLE(KEY,VALUE) papiWriteScenario_double(scn, KEY, VALUE)
#define SAVE_V3(KEY,VALUE) papiWriteScenario_vec(scn, KEY, VALUE)
#define SAVE_M3(KEY,VALUE) papiWriteScenario_mx(scn, KEY, VALUE)
#define SAVE_STRING(KEY,VALUE) oapiWriteScenario_string(scn, KEY, VALUE)
#define LOAD_BOOL(KEY,VALUE) if(strnicmp(line, KEY, strlen(KEY)) == 0){ sscanf(line + strlen(KEY), "%i", &tmp); if (tmp == 1) { VALUE = true; } else { VALUE = false; } }
#define LOAD_INT(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%i",&VALUE); }
#define LOAD_DOUBLE(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf",&VALUE); }
#define LOAD_V3(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf %lf %lf",&VALUE.x,&VALUE.y,&VALUE.z); }
#define LOAD_M3(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf %lf %lf %lf %lf %lf %lf %lf %lf",&VALUE.m11,&VALUE.m12,&VALUE.m13,&VALUE.m21,&VALUE.m22,&VALUE.m23,&VALUE.m31,&VALUE.m32,&VALUE.m33); }
#define LOAD_STRING(KEY,VALUE,LEN) if(strnicmp(line,KEY,strlen(KEY))==0){ strncpy(VALUE, line + (strlen(KEY)+1), LEN); }

RTCC::RTCC()
{
	mcc = NULL;
	TimeofIgnition = 0.0;
	SplashLatitude = 0.0;
	SplashLongitude = 0.0;
	DeltaV_LVLH = _V(0.0, 0.0, 0.0);
	calcParams.EI = 0.0;
	calcParams.TEI = 0.0;
	calcParams.TLI = 0.0;
	calcParams.R_TLI = _V(0,0,0);
	calcParams.V_TLI = _V(0,0,0);
	calcParams.alt_node = 0.0;
	calcParams.GET_node = 0.0;
	calcParams.lat_node = 0.0;
	calcParams.lng_node = 0.0;
	calcParams.LOI = 0.0;
	calcParams.DOI = 0.0;
	calcParams.PDI = 0.0;
	calcParams.TLAND = 0.0;
	calcParams.Insertion = 0.0;
	calcParams.Phasing = 0.0;
	calcParams.CSI = 0.0;
	calcParams.TPI = 0.0;
	calcParams.src = NULL;
	calcParams.tgt = NULL;
	REFSMMATType = 0;
}

void RTCC::Init(MCC *ptr)
{
	mcc = ptr;
}
bool RTCC::Calculation(int mission, int fcn, LPVOID &pad, char * upString, char * upDesc)
{
	bool scrubbed = false;

	switch (mission)
	{
	case MTP_B:
		scrubbed = CalculationMTP_B(fcn, pad, upString, upDesc);
		break;
	case MTP_C:
		scrubbed = CalculationMTP_C(fcn, pad, upString, upDesc);
		break;
	case MTP_C_PRIME:
		scrubbed = CalculationMTP_C_PRIME(fcn, pad, upString, upDesc);
		break;
	case MTP_D:
		scrubbed = CalculationMTP_D(fcn, pad, upString, upDesc);
		break;
	case MTP_F:
		scrubbed = CalculationMTP_F(fcn, pad, upString, upDesc);
		break;
	}

	return scrubbed;
}

bool RTCC::CalculationMTP_B(int fcn, LPVOID &pad, char * upString, char * upDesc)
{
	char* uplinkdata = new char[1000];

	double AGCEpoch = 39856.0;
	MATRIX3 REFSMMAT = _M(0.749669954748883, -0.141831590016531, 0.646435425251580, 0.318362144838044, 0.933611208066774, -0.16436435, -0.580207293715727, 0.329019622888181, 0.74505405);
	REFSMMAT = mul(REFSMMAT, OrbMech::J2000EclToBRCS(AGCEpoch));

	switch (fcn)
	{
	case 1: //RCS BURN 1 ATTITUDE
	{
		AP11LMManPADOpt opt;
		AP11LMMNV manpad;

		double GET, SVMJD;
		SVMJD = oapiGetSimMJD();
		GET = mcc->lm->GetMissionTime();
		opt.GETbase = SVMJD - GET / 24.0 / 3600.0;

		opt.dV_LVLH = _V(1.0, 0.0, -1.0);
		opt.enginetype = RTCC_ENGINETYPE_RCS;
		opt.HeadsUp = true;
		opt.REFSMMAT = REFSMMAT;
		opt.TIG = OrbMech::HHMMSSToSS(8, 52, 44);
		opt.vessel = mcc->lm;
		opt.csmlmdocked = 0;

		AP11LMManeuverPAD(&opt, manpad);

		sprintf(uplinkdata, "%s", SunburstAttitudeManeuver(manpad.IMUAtt));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Attitude for RCS Burn #1");
		}
	}
	break;
	case 2: //CONFIGURE FOR SYSTEM B DEPLETION
	{
		sprintf(uplinkdata, "%sKKKKKKKKKK%s", SunburstLMPCommand(168), SunburstLMPCommand(232));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Configure for RCS system B depletion");
		}
	}
	break;
	case 3: //PLUS X TRANSLATION ON
	{
		sprintf(uplinkdata, "%s", SunburstLMPCommand(128));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "RCS +X translation on");
		}
	}
	break;
	case 4: //PLUS X TRANSLATION OFF
	{
		sprintf(uplinkdata, "%s", SunburstLMPCommand(129));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "RCS +X translation off");
		}
	}
	break;
	case 5: //CONFIGURE FOR NORMAL RCS A OPERATION
	{
		sprintf(uplinkdata, "%sKKKKKKKKKK%s", SunburstLMPCommand(152), SunburstLMPCommand(216));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Configure for normal RCS system A operation");
		}
	}
	break;
	case 6: //RCS BURN 2 ATTITUDE
	{
		AP11LMManPADOpt opt;
		AP11LMMNV manpad;

		double GET, SVMJD;
		SVMJD = oapiGetSimMJD();
		GET = mcc->lm->GetMissionTime();
		opt.GETbase = SVMJD - GET / 24.0 / 3600.0;

		opt.dV_LVLH = _V(0.0, 1.0, 0.0);
		opt.enginetype = RTCC_ENGINETYPE_RCS;
		opt.HeadsUp = true;
		opt.REFSMMAT = REFSMMAT;
		opt.TIG = OrbMech::HHMMSSToSS(9, 40, 20);
		opt.vessel = mcc->lm;
		opt.csmlmdocked = 0;

		AP11LMManeuverPAD(&opt, manpad);

		sprintf(uplinkdata, "%s", SunburstAttitudeManeuver(manpad.IMUAtt));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Attitude for RCS Burn #2");
		}
	}
	break;
	case 7: //X-FEED OPEN
	{
		sprintf(uplinkdata, "%s", SunburstLMPCommand(252));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "X-Feed Open");
		}
	}
	break;
	case 8: //ERRONEOUS LM WEIGHT
	{
		sprintf(uplinkdata, "%s", SunburstMassUpdate(4716.0));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Update LM mass with erroneous value");
		}
	}
	break;
	case 9: //RCS BURN 5 ATTITUDE
	{
		AP11LMManPADOpt opt;
		AP11LMMNV manpad;

		double GET, SVMJD;
		SVMJD = oapiGetSimMJD();
		GET = mcc->lm->GetMissionTime();
		opt.GETbase = SVMJD - GET / 24.0 / 3600.0;

		opt.dV_LVLH = _V(1.0, 0.0, 0.0);
		opt.enginetype = RTCC_ENGINETYPE_RCS;
		opt.HeadsUp = true;
		opt.REFSMMAT = REFSMMAT;
		opt.TIG = OrbMech::HHMMSSToSS(12, 52, 18);
		opt.vessel = mcc->lm;
		opt.csmlmdocked = 0;

		AP11LMManeuverPAD(&opt, manpad);

		sprintf(uplinkdata, "%s", SunburstAttitudeManeuver(manpad.IMUAtt));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Attitude for RCS Burn #5");
		}
	}
	break;
	}

	return false;
}

bool RTCC::CalculationMTP_D(int fcn, LPVOID &pad, char * upString, char * upDesc)
{
	char* uplinkdata = new char[1000];
	bool preliminary = true;
	bool scrubbed = false;

	double AGCEpoch = 40221.525;

	switch (fcn) {
	case 1: //GENERIC CSM STATE VECTOR UPDATE
	{
		SV sv;
		double GETbase;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();

		sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 2: //CSM STATE VECTOR UPDATE AND NAV CHECK PAD
	{
		AP7NAV * form = (AP7NAV *)pad;

		SV sv;
		double GETbase;

		GETbase = getGETBase();
		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		NavCheckPAD(sv, *form, GETbase);

		sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 3: //DAYLIGHT STAR CHECK
	{
		STARCHKPAD * form = (STARCHKPAD *)pad;

		form->Att[0] = _V(68.0, 291.1, 330.2);
		form->GET[0] = OrbMech::HHMMSSToSS(6.0, 49.0, 45.0);
		form->TAlign[0] = 0.0;
	}
	break;
	case 10:	//SPS-1
	{
		AP7MNV * form = (AP7MNV *)pad;

		AP7ManPADOpt opt;
		double GETbase, P30TIG, NomTIG, sv0GET, dt1, TIGMJD;
		VECTOR3 dV_LVLH;
		SV sv0, sv1;

		sv0 = StateVectorCalc(calcParams.src); //State vector for uplink

		GETbase = getGETBase();
		NomTIG = OrbMech::HHMMSSToSS(6.0, 1.0, 40.0);
		sv0GET = (sv0.MJD - GETbase)*24.0*3600.0;
		dt1 = NomTIG - sv0GET;
		sv1 = coast(sv0, dt1 - 10.0*60.0);

		TIGMJD = OrbMech::P29TimeOfLongitude(sv1.R, sv1.V, sv1.MJD, sv1.gravref, -167.675*RAD);	//find 167.675°W
		P30TIG = round((TIGMJD - GETbase)*24.0*3600.0 / 60.0)*60.0;	//Round to next minute
		dV_LVLH = _V(36.8, 0.0, 0.0)*0.3048;

		opt.GETbase = GETbase;
		opt.vessel = calcParams.src;
		opt.TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.HeadsUp = true;
		opt.sxtstardtime = -30.0*60.0;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.navcheckGET = 0;
		opt.vesseltype = 1;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "SPS-1");
		sprintf(form->remarks, "Gimbal angles with pad REFSMMAT");

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv0, true, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, target load");
		}
	}
	break;
	case 11: //BLOCK DATA 2
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		const int n = 6;
		double lng[n] = { 145.0*RAD, -161.7*RAD, -23.0*RAD, -32.1*RAD, -26.4*RAD, -27.9*RAD };
		double GETI[n] = { OrbMech::HHMMSSToSS(13,14,31),OrbMech::HHMMSSToSS(15,02,51),OrbMech::HHMMSSToSS(15,40,24),OrbMech::HHMMSSToSS(17,13,49),OrbMech::HHMMSSToSS(18,50,57),OrbMech::HHMMSSToSS(20,24,40) };
		char area[n][10] = { "009-3B", "010-CC", "011-AC", "012-AC", "013-2A", "014-AC" };
		char **test;
		test = new char*[n];
		for (int i = 0;i < n;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 12: //BLOCK DATA 3
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		const int n = 6;
		double lng[n] = { -67.0*RAD, -67.0*RAD, -67.0*RAD, -66.3*RAD, -69.2*RAD, -165.5*RAD };
		double GETI[n] = { OrbMech::HHMMSSToSS(21,52,49),OrbMech::HHMMSSToSS(23,28,3),OrbMech::HHMMSSToSS(25,2,25),OrbMech::HHMMSSToSS(26,37,58),OrbMech::HHMMSSToSS(28,11,50),OrbMech::HHMMSSToSS(31,7,17) };
		char area[n][10] = { "015-1B", "016-1B", "017-1B", "018-1B", "019-1B", "020-4A" };
		char **test;
		test = new char*[n];
		for (int i = 0;i < n;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 13: //SPS-2
	{
		AP7MNV * form = (AP7MNV *)pad;

		AP7ManPADOpt opt;
		REFSMMATOpt refsopt;
		GMPOpt gmpopt;
		double GETbase, P30TIG, NomTIG, sv0GET, dt1, TIGMJD, TIG;
		VECTOR3 dV_LVLH;
		MATRIX3 REFSMMAT;
		SV sv0, sv1;

		sv0 = StateVectorCalc(calcParams.src); //State vector for uplink

		GETbase = getGETBase();
		NomTIG = OrbMech::HHMMSSToSS(22.0, 12.0, 0.0);
		sv0GET = (sv0.MJD - GETbase)*24.0*3600.0;
		dt1 = NomTIG - sv0GET;
		sv1 = coast(sv0, dt1 - 10.0*60.0);

		TIGMJD = OrbMech::P29TimeOfLongitude(sv1.R, sv1.V, sv1.MJD, sv1.gravref, -64.783*RAD);	//find 64.783°W
		TIG = (TIGMJD - GETbase)*24.0*3600.0;

		gmpopt.csmlmdocked = true;
		gmpopt.GETbase = GETbase;
		gmpopt.h_apo = 192.0*1852.0;
		gmpopt.impulsive = RTCC_NONIMPULSIVE;
		gmpopt.rot_ang = 1.89*RAD;
		gmpopt.TIG_GET = TIG;
		gmpopt.type = 5;
		gmpopt.vessel = calcParams.src;

		GeneralManeuverProcessor(&gmpopt, dV_LVLH, P30TIG);

		refsopt.csmlmdocked = true;
		refsopt.dV_LVLH = dV_LVLH;
		refsopt.GETbase = GETbase;
		refsopt.HeadsUp = false;
		refsopt.P30TIG = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;
		refsopt.vesseltype = 1;

		REFSMMAT = REFSMMATCalc(&refsopt);

		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.navcheckGET = P30TIG - 30.0*60.0;
		opt.REFSMMAT = REFSMMAT;
		opt.sxtstardtime = -30.0*60.0;
		opt.TIG = P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 1;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "SPS-2");

		//Bias pitch trim gimbal angle by 0.5° to induce transient at ignition. Gets mentioned in pre-mission documents, but wasn't actually done during the mission?!
		form->pTrim += 0.5;

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv0, true, AGCEpoch, GETbase, true), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, Verb 66, target load");
		}
	}
	break;
	case 14: //SPS-3
	{
		AP7MNV * form = (AP7MNV *)pad;

		AP7ManPADOpt opt;
		REFSMMATOpt refsopt;
		GMPOpt gmpopt;
		double GETbase, P30TIG, NomTIG, sv0GET, dt1, TIGMJD, TIG;
		VECTOR3 dV_LVLH;
		MATRIX3 REFSMMAT;
		SV sv0, sv1;

		sv0 = StateVectorCalc(calcParams.src); //State vector for uplink

		GETbase = getGETBase();
		NomTIG = OrbMech::HHMMSSToSS(25.0, 18.0, 30.0);
		sv0GET = (sv0.MJD - GETbase)*24.0*3600.0;
		dt1 = NomTIG - sv0GET;
		sv1 = coast(sv0, dt1 - 10.0*60.0);

		TIGMJD = OrbMech::P29TimeOfLongitude(sv1.R, sv1.V, sv1.MJD, sv1.gravref, -70.0*RAD);	//find 70.0°W
		TIG = (TIGMJD - GETbase)*24.0*3600.0;

		gmpopt.csmlmdocked = true;
		gmpopt.GETbase = GETbase;
		gmpopt.h_apo = 272.0*1852.0;
		gmpopt.impulsive = RTCC_NONIMPULSIVE;
		gmpopt.rot_ang = 5.7*RAD;
		gmpopt.TIG_GET = TIG;
		gmpopt.type = 5;
		gmpopt.vessel = calcParams.src;

		GeneralManeuverProcessor(&gmpopt, dV_LVLH, P30TIG);

		refsopt.csmlmdocked = true;
		refsopt.dV_LVLH = dV_LVLH;
		refsopt.GETbase = GETbase;
		refsopt.HeadsUp = false;
		refsopt.P30TIG = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;
		refsopt.vesseltype = 1;

		REFSMMAT = REFSMMATCalc(&refsopt);

		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.navcheckGET = P30TIG - 30.0*60.0;
		opt.REFSMMAT = REFSMMAT;
		opt.sxtstardtime = -30.0*60.0;
		opt.TIG = P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 1;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "SPS-3");

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv0, true, AGCEpoch, GETbase, true), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, Verb 66, target load");
		}
	}
	break;
	case 15: //SPS-4
	{
		AP7MNV * form = (AP7MNV *)pad;

		AP7ManPADOpt opt;
		REFSMMATOpt refsopt;
		GMPOpt gmpopt;
		double GETbase, P30TIG, NomTIG, sv0GET, dt1, TIGMJD, TIG;
		VECTOR3 dV_LVLH;
		MATRIX3 REFSMMAT;
		SV sv0, sv1;

		sv0 = StateVectorCalc(calcParams.src); //State vector for uplink

		GETbase = getGETBase();
		NomTIG = OrbMech::HHMMSSToSS(28.0, 28.0, 20.0);
		sv0GET = (sv0.MJD - GETbase)*24.0*3600.0;
		dt1 = NomTIG - sv0GET;
		sv1 = coast(sv0, dt1 - 10.0*60.0);

		TIGMJD = OrbMech::P29TimeOfLongitude(sv1.R, sv1.V, sv1.MJD, sv1.gravref, -110.0*RAD);	//find 110.0°W
		TIG = (TIGMJD - GETbase)*24.0*3600.0;

		gmpopt.csmlmdocked = true;
		gmpopt.GETbase = GETbase;
		gmpopt.h_apo = 272.0*1852.0;
		gmpopt.impulsive = RTCC_NONIMPULSIVE;
		gmpopt.rot_ang = 0.67*RAD;
		gmpopt.TIG_GET = TIG;
		gmpopt.type = 5;
		gmpopt.vessel = calcParams.src;

		GeneralManeuverProcessor(&gmpopt, dV_LVLH, P30TIG);

		refsopt.csmlmdocked = true;
		refsopt.dV_LVLH = dV_LVLH;
		refsopt.GETbase = GETbase;
		refsopt.HeadsUp = false;
		refsopt.P30TIG = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;
		refsopt.vesseltype = 1;

		REFSMMAT = REFSMMATCalc(&refsopt);

		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.navcheckGET = P30TIG - 30.0*60.0;
		opt.REFSMMAT = REFSMMAT;
		opt.sxtstardtime = -30.0*60.0;
		opt.TIG = P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 1;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "SPS-4");

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv0, true, AGCEpoch, GETbase, true), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, Verb 66, target load");
		}
	}
	break;
	case 16: //BLOCK DATA 4
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		const int n = 6;
		double lng[n] = { -161.0*RAD, -161.0*RAD, -162.5*RAD, -7.0*RAD, -162.0*RAD, -26.0*RAD };
		double GETI[n] = { OrbMech::HHMMSSToSS(32,44,34),OrbMech::HHMMSSToSS(34,19,1),OrbMech::HHMMSSToSS(35,56,3),OrbMech::HHMMSSToSS(36,24,11),OrbMech::HHMMSSToSS(39,13,13),OrbMech::HHMMSSToSS(39,33,59) };
		char area[n][10] = { "021-4A", "022-4C", "023-4C", "024-AC", "025-4C", "026-AC" };
		char **test;
		test = new char*[n];
		for (int i = 0;i < n;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 17: //BLOCK DATA 5
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		const int n = 6;
		double lng[n] = { -31.0*RAD, -26.4*RAD, -28.5*RAD, -29.0*RAD, -32.0*RAD, -33.0*RAD };
		double GETI[n] = { OrbMech::HHMMSSToSS(41,16,3),OrbMech::HHMMSSToSS(43,2,57),OrbMech::HHMMSSToSS(44,46,10),OrbMech::HHMMSSToSS(46,24,14),OrbMech::HHMMSSToSS(47,58,31),OrbMech::HHMMSSToSS(49,34,33) };
		char area[n][10] = { "027-AC", "028-2A", "029-AC", "030-2C", "031-2C", "032-2B" };
		char **test;
		test = new char*[n];
		for (int i = 0;i < n;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	}

	return scrubbed;
}

bool RTCC::CalculationMTP_C_PRIME(int fcn, LPVOID &pad, char * upString, char * upDesc)
{
	char* uplinkdata = new char[1000];
	bool preliminary = true;
	bool scrubbed = false;

	//Hardcoded for now, better solution at some point...
	double AGCEpoch = 40221.525;
	double LSLat = 2.6317*RAD;
	double LSLng = 34.0253*RAD;
	double LSAlt = -0.82*1852.0;
	double LSAzi = -78.0*RAD;
	double t_land = OrbMech::HHMMSSToSS(82.0, 8.0, 26.0);

	switch (fcn) {
	case 2: //TLI+90 PAD + State Vector
	case 3: //TLI+4 PAD
	{
		EntryOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase, TLIplus, CSMmass;
		SV sv, sv1, sv2;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		if (fcn == 2)
		{
			TLIplus = 90.0*60.0;
			entopt.lng = -30.0*RAD;
		}
		else
		{
			TLIplus = 4.0*3600.0;
			entopt.lng = -165.0 * RAD;
		}

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		CSMmass = 28862.0;

		sv1.mass = CSMmass;
		sv1.gravref = AGCGravityRef(calcParams.src);
		sv1.MJD = GETbase + calcParams.TLI / 24.0 / 3600.0;
		sv1.R = calcParams.R_TLI;
		sv1.V = calcParams.V_TLI;
		sv2 = coast(sv1, TLIplus);// -(sv1.MJD - GETbase)*24.0*3600.0);

		entopt.entrylongmanual = true;
		entopt.GETbase = GETbase;
		entopt.impulsive = RTCC_NONIMPULSIVE;
		entopt.ReA = 0;
		entopt.TIGguess = TimeofIgnition + TLIplus;//(TIGMJD - GETbase)*24.0*3600.0 + TLIplus;
		entopt.type = RTCC_ENTRY_ABORT;
		entopt.vessel = calcParams.src;
		entopt.useSV = true;
		entopt.RV_MCC = sv2;

		EntryTargeting(&entopt, &res); //Target Load for uplink

		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;
		opt.useSV = true;
		opt.RV_MCC = sv2;

		AP11ManeuverPAD(&opt, *form);
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->Weight = CSMmass/ 0.45359237;
		form->GET05G = res.GET05G;

		if (fcn == 2)
		{
			sprintf(form->purpose, "TLI+90");
			sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM and LM state vectors");
			}
		}
		else
		{
			sprintf(form->purpose, "TLI+4");

			//Save parameters for further use
			SplashLatitude = res.latitude;
			SplashLongitude = res.longitude;
			calcParams.TEI = res.P30TIG;
			calcParams.EI = res.GET400K;
		}
	}
	break;
	case 4: //TLI PAD
	{
		TLIPADOpt opt;
		double GETbase;

		TLIPAD * form = (TLIPAD *)pad;

		GETbase = getGETBase();

		opt.dV_LVLH = DeltaV_LVLH;
		opt.GETbase = GETbase;
		opt.R_TLI = calcParams.R_TLI;
		opt.V_TLI = calcParams.V_TLI;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = TimeofIgnition;
		opt.TLI = calcParams.TLI;
		opt.vessel = calcParams.src;
		opt.SeparationAttitude = _V(PI, 120.0*RAD, 0.0);
		opt.uselvdc = true;

		TLI_PAD(&opt, *form);

		//calcParams.TLI = TimeofIgnition + form->BurnTime;// ceil(TimeofIgnition / 3600.0)*3600.0;	//Round up to nominally 3 hours, timebase for MCCs and TLC Aborts
	}
	break;
	case 10: //MISSION CP BLOCK DATA 1
	case 11: //MISSION CP BLOCK DATA 2
	case 12: //MISSION CP BLOCK DATA 3
	case 13: //MISSION CP BLOCK DATA 4
	{
		EntryOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double TLIplus, GETbase;
		char manname[8];

		AP11MNV * form = (AP11MNV *)pad;
		
		if (fcn == 10)
		{
			TLIplus = calcParams.TLI + 11.0*3600.0;
			sprintf(manname, "TLI+11");
		}
		else if (fcn == 11)
		{
			TLIplus = calcParams.TLI + 25.0*3600.0;
			sprintf(manname, "TLI+25");
		}
		else if (fcn == 12)
		{
			TLIplus = calcParams.TLI + 35.0*3600.0;
			sprintf(manname, "TLI+35");
		}
		else if (fcn == 13)
		{
			TLIplus = calcParams.TLI + 44.0*3600.0;
			sprintf(manname, "TLI+44");
		}

		GETbase = getGETBase();

		entopt.entrylongmanual = true;
		entopt.GETbase = GETbase;
		entopt.impulsive = RTCC_NONIMPULSIVE;
		entopt.lng = -165.0*RAD;
		entopt.ReA = 0;
		entopt.TIGguess = TLIplus;
		entopt.type = RTCC_ENTRY_ABORT;
		entopt.vessel = calcParams.src;

		EntryTargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, ReA, prec); //Target Load for uplink

		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);
		sprintf(form->purpose, manname);
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->GET05G = res.GET05G;

		//Save parameters for further use
		SplashLatitude = res.latitude;
		SplashLongitude = res.longitude;
		calcParams.TEI = res.P30TIG;
		calcParams.EI = res.GET400K;
	}
	break;
	case 20: // MISSION CP MCC1
	case 21: // MISSION CP MCC2
	case 22: // MISSION CP MCC3
	case 23: // MISSION CP MCC4
	{
		AP11ManPADOpt manopt;
		double GETbase, P30TIG, MCCGET;
		VECTOR3 dV_LVLH;
		MATRIX3 REFSMMAT;
		SV sv;
		char manname[8];

		if (calcParams.TLI == 0)
		{
			calcParams.TLI = OrbMech::HHMMSSToSS(3, 0, 0);
		}

		if (calcParams.LOI == 0)
		{
			calcParams.LOI = OrbMech::HHMMSSToSS(69.0, 9.0, 29.4);
		}

		if (fcn == 20)
		{
			MCCGET = calcParams.TLI + 6.0*3600.0;
			sprintf(manname, "MCC1");
			REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		}
		else if (fcn == 21)
		{
			MCCGET = calcParams.TLI + 25.0*3600.0;
			sprintf(manname, "MCC2");
			REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		}
		else if (fcn == 22)
		{
			MCCGET = calcParams.LOI - 22.0*3600.0;
			sprintf(manname, "MCC3");
			REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		}
		else
		{
			MCCGET = calcParams.LOI - 8.0*3600.0;
			sprintf(manname, "MCC4");
		}

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		//Free Return - Best Adaptive Path
		if (fcn == 20 || fcn == 21)
		{
			MCCFRMan opt;
			TLMCCResults res;

			opt.type = 0;
			opt.lat = -5.67822*RAD;
			opt.PeriGET = calcParams.LOI;
			opt.h_peri = 60.0 * 1852.0;
			opt.alt = LSAlt;
			opt.azi = LSAzi;
			opt.csmlmdocked = false;
			opt.GETbase = GETbase;
			opt.MCCGET = MCCGET;

			opt.LOIh_apo = 170.0*1852.0;
			opt.LOIh_peri = 60.0*1852.0;
			opt.LSlat = LSLat;
			opt.LSlng = LSLng;
			opt.PeriGET = calcParams.LOI;
			opt.t_land = t_land;
			opt.vessel = calcParams.src;

			if (TranslunarMidcourseCorrectionTargetingFreeReturn(&opt, &res))
			{
				calcParams.alt_node = res.NodeAlt;
				calcParams.lat_node = res.NodeLat;
				calcParams.lng_node = res.NodeLng;
				calcParams.GET_node = res.NodeGET;
				calcParams.LOI = res.PericynthionGET;
				dV_LVLH = res.dV_LVLH;
				P30TIG = res.P30TIG;
			}
		}
		else //Nodal Targeting
		{
			MCCNodeMan opt;

			opt.lat = calcParams.lat_node;
			opt.lng = calcParams.lng_node;
			opt.NodeGET = calcParams.GET_node;
			opt.h_node = calcParams.alt_node;
			opt.csmlmdocked = false;
			opt.GETbase = GETbase;
			opt.MCCGET = MCCGET;
			opt.vessel = calcParams.src;

			TranslunarMidcourseCorrectionTargetingNodal(&opt, dV_LVLH, P30TIG);
		}

		if (fcn != 23)
		{
			if (length(dV_LVLH) < 5.0*0.3048)
			{
				scrubbed = true;
			}
		}

		if (scrubbed)
		{
			sprintf(upDesc, "%s has been scrubbed.", manname);
		}
		else
		{
			if (fcn == 23)
			{
				LOIMan opt2;
				REFSMMATOpt refsopt;
				double P30TIG_LOI;
				VECTOR3 dV_LVLH_LOI;

				sv.mass = calcParams.src->GetMass();

				opt2.csmlmdocked = false;
				opt2.GETbase = GETbase;
				opt2.h_apo = 170.0*1852.0;
				opt2.h_peri = 60.0*1852.0;
				opt2.alt = LSAlt;
				opt2.azi = LSAzi;
				opt2.lat = LSLat;
				opt2.lng = LSLng;
				opt2.useSV = true;
				opt2.vessel = calcParams.src;
				opt2.t_land = t_land;
				opt2.RV_MCC = ExecuteManeuver(calcParams.src, GETbase, P30TIG, dV_LVLH, sv, 0);

				LOITargeting(&opt2, dV_LVLH_LOI, P30TIG_LOI);

				refsopt.dV_LVLH = dV_LVLH;
				refsopt.dV_LVLH2 = dV_LVLH_LOI;
				refsopt.GETbase = GETbase;
				refsopt.P30TIG = P30TIG;
				refsopt.P30TIG2 = P30TIG_LOI;
				refsopt.REFSMMATopt = 7;
				refsopt.vessel = calcParams.src;

				REFSMMAT = REFSMMATCalc(&refsopt);
			}

			manopt.dV_LVLH = dV_LVLH;
			manopt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), dV_LVLH);
			manopt.GETbase = GETbase;
			manopt.HeadsUp = false;
			manopt.REFSMMAT = REFSMMAT;
			manopt.TIG = P30TIG;
			manopt.vessel = calcParams.src;
			manopt.vesseltype = 0;

			AP11ManeuverPAD(&manopt, *form);
			sprintf(form->purpose, manname);

			if (fcn == 23)
			{
				sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH), CMCDesiredREFSMMATUpdate(REFSMMAT, AGCEpoch));
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "LM state vector, target load, LOI-2 REFSMMAT");
				}
			}
			else
			{
				sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "LM state vector, target load");
				}
			}
		}
	}
	break;
	case 30:	// MISSION CP PRELIMINARY LOI-1 MANEUVER
	case 31:	// MISSION CP LOI-1 MANEUVER
	{
		LOIMan opt;
		AP11ManPADOpt manopt;
		double GETbase, P30TIG;
		VECTOR3 dV_LVLH;
		SV sv;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		opt.csmlmdocked = false;
		opt.GETbase = GETbase;
		opt.h_apo = 170.0*1852.0;
		opt.h_peri = 60.0*1852.0;
		opt.alt = LSAlt;
		opt.azi = LSAzi;
		opt.lat = LSLat;
		opt.lng = LSLng;
		opt.t_land = t_land;
		opt.vessel = calcParams.src;

		LOITargeting(&opt, dV_LVLH, P30TIG);

		manopt.alt = LSAlt;
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.vesseltype = 0;

		AP11ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "LOI-1");

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

		if (fcn == 31)
		{
			sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load");
			}
		}
	}
	break;
	case 40:	// MISSION CP FLYBY MANEUVER
	{
		RTEFlybyOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		entopt.EntryLng = -165.0*RAD;
		entopt.GETbase = GETbase;
		entopt.returnspeed = 0;
		entopt.FlybyType = 0;
		entopt.TIGguess = calcParams.LOI - 8.0*3600.0;
		entopt.vessel = calcParams.src;

		RTEFlybyTargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, EntryAng);

		opt.alt = LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);

		if (!REFSMMATDecision(form->Att*RAD))
		{
			REFSMMATOpt refsopt;
			MATRIX3 REFSMMAT;

			refsopt.dV_LVLH = res.dV_LVLH;
			refsopt.GETbase = GETbase;
			refsopt.P30TIG = res.P30TIG;
			refsopt.REFSMMATopt = 0;
			refsopt.vessel = calcParams.src;

			REFSMMAT = REFSMMATCalc(&refsopt);

			opt.HeadsUp = true;
			opt.REFSMMAT = REFSMMAT;
			AP11ManeuverPAD(&opt, *form);

			sprintf(form->remarks, "Requires realignment to preferred REFSMMAT");
		}

		sprintf(form->purpose, "Flyby");
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->GET05G = res.GET05G;

		//Save parameters for further use
		SplashLatitude = res.latitude;
		SplashLongitude = res.longitude;
		calcParams.TEI = res.P30TIG;
		calcParams.EI = res.GET400K;
	}
	break;
	case 41:	// MISSION CP PC+2 MANEUVER
	case 42:	// MISSION CP FAST PC+2 MANEUVER
	{
		RTEFlybyOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase;
		char manname[32];

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		if (fcn == 41)
		{
			entopt.EntryLng = -25.0*RAD;
			entopt.returnspeed = 1;
			sprintf(manname, "PC+2");
		}
		else
		{
			entopt.EntryLng = 65.0*RAD;
			entopt.returnspeed = 2;
			sprintf(manname, "PC+2 fast return");
		}
		entopt.GETbase = GETbase;
		entopt.FlybyType = 1;
		entopt.vessel = calcParams.src;

		RTEFlybyTargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, EntryAng);

		opt.alt = LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);

		if (!REFSMMATDecision(form->Att*RAD))
		{
			REFSMMATOpt refsopt;
			MATRIX3 REFSMMAT;

			refsopt.dV_LVLH = res.dV_LVLH;
			refsopt.GETbase = GETbase;
			refsopt.P30TIG = res.P30TIG;
			refsopt.REFSMMATopt = 0;
			refsopt.vessel = calcParams.src;

			REFSMMAT = REFSMMATCalc(&refsopt);

			opt.HeadsUp = true;
			opt.REFSMMAT = REFSMMAT;
			AP11ManeuverPAD(&opt, *form);

			sprintf(form->remarks, "Requires realignment to preferred REFSMMAT");
		}
		sprintf(form->purpose, manname);
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->GET05G = res.GET05G;

		//Save parameters for further use
		SplashLatitude = res.latitude;
		SplashLongitude = res.longitude;
		calcParams.TEI = res.P30TIG;
		calcParams.EI = res.GET400K;
	}
	break;
	case 50:	// MISSION CP TEI-1 (Pre LOI)
	{
		TEIOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase;
		SV sv0, sv1;
		char manname[8];

		AP11MNV * form = (AP11MNV *)pad;

		//entopt.TIGguess = OrbMech::HHMMSSToSS(71, 25, 4);
		sprintf(manname, "TEI-1");

		GETbase = getGETBase();

		sv0 = StateVectorCalc(calcParams.src); //State vector for uplink

		sv1 = ExecuteManeuver(calcParams.src, GETbase, TimeofIgnition, DeltaV_LVLH, sv0, 0);

		entopt.EntryLng = -165.0*RAD;
		entopt.GETbase = GETbase;
		entopt.returnspeed = 1;
		entopt.RV_MCC = sv1;
		entopt.useSV = true;
		entopt.vessel = calcParams.src;

		TEITargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, EntryAng);

		opt.alt = LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.RV_MCC = sv1;
		opt.TIG = res.P30TIG;
		opt.useSV = true;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);
		sprintf(form->purpose, manname);
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->GET05G = res.GET05G;

		//Save parameters for further use
		SplashLatitude = res.latitude;
		SplashLongitude = res.longitude;
		calcParams.TEI = res.P30TIG;
		calcParams.EI = res.GET400K;
	}
	break;
	case 102:	// MISSION CP LOI-2 MANEUVER
	{
		LOI2Man opt;
		AP11ManPADOpt manopt;
		double GETbase, P30TIG;
		VECTOR3 dV_LVLH;
		SV sv;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		opt.alt = LSAlt;
		opt.csmlmdocked = false;
		opt.GETbase = GETbase;
		opt.h_circ = 60.0*1852.0;
		opt.vessel = calcParams.src;

		LOI2Targeting(&opt, dV_LVLH, P30TIG);

		manopt.alt = LSAlt;
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.vesseltype = 0;

		AP11ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "LOI-2");

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, target load");
		}
	}
	break;
	case 103: //GENERIC CSM STATE VECTOR UPDATE TO LM SLOT
	{
		SV sv;
		double GETbase;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();

		sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector");
		}
	}
	break;
	case 104: //TEI-1
	case 105: //TEI-2
	case 106: //TEI-3
	case 107: //TEI-4
	case 108: //TEI-5
	case 109: //TEI-6
	case 110: //TEI-7
	case 111: //TEI-8
	case 112: //TEI-9
	case 113: //Preliminary TEI-10
	case 200: //TEI-10
	case 201: //TEI-11
	{
		TEIOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase;
		SV sv;
		char manname[8];

		AP11MNV * form = (AP11MNV *)pad;

		entopt.RevsTillTEI = 1;

		if (fcn == 104) //TEI-1
		{
			entopt.RevsTillTEI = 0;
			sprintf(manname, "TEI-1");
		}
		if (fcn == 105) //TEI-2
		{
			sprintf(manname, "TEI-2");
		}
		if (fcn == 106) //TEI-3
		{
			sprintf(manname, "TEI-3");
			sprintf(form->remarks, "Assumes no LOI-2");
		}
		else if (fcn == 107) //TEI-4
		{
			sprintf(manname, "TEI-4");
		}
		else if (fcn == 108) //TEI-5
		{
			sprintf(manname, "TEI-5");
		}
		else if (fcn == 109) //TEI-6
		{
			sprintf(manname, "TEI-6");
		}
		else if (fcn == 110) //TEI-7
		{
			sprintf(manname, "TEI-7");
		}
		else if (fcn == 111) //TEI-8
		{
			sprintf(manname, "TEI-8");
		}
		else if (fcn == 112) //TEI-9
		{
			sprintf(manname, "TEI-9");
		}
		else if (fcn == 113) //Preliminary TEI-10
		{
			sprintf(manname, "TEI-10");
		}
		else if (fcn == 200) //TEI-10
		{
			entopt.RevsTillTEI = 0;
			sprintf(manname, "TEI-10");
		}
		else if (fcn == 201) //TEI-11
		{
			sprintf(manname, "TEI-11");
		}

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.TIGguess = 0.0;
		entopt.EntryLng = -165.0*RAD;
		entopt.GETbase = GETbase;
		entopt.vessel = calcParams.src;

		entopt.returnspeed = 1;

		TEITargeting(&entopt, &res);

		opt.alt = LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);
		sprintf(form->purpose, manname);
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->GET05G = res.GET05G;

		if (fcn == 109 || fcn == 110 || fcn == 111 || fcn == 113)
		{
			sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "LM state vector");
			}
		}
		else if (fcn == 200)
		{
			sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase), CMCRetrofireExternalDeltaVUpdate(res.latitude, res.longitude, res.P30TIG, res.dV_LVLH));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM and LM state vectors, target load");
			}
		}

		if (fcn != 201)	//Don't save it for TEI-11
		{
			//Save parameters for further use
			SplashLatitude = res.latitude;
			SplashLongitude = res.longitude;
			calcParams.TEI = res.P30TIG;
			calcParams.EI = res.GET400K;
		}
	}
	break;
	case 202: //Entry REFSMMAT
	{
		REFSMMATOpt refsopt;
		MATRIX3 REFSMMAT;

		refsopt.GETbase = getGETBase();
		refsopt.REFSMMATopt = 3;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		sprintf(uplinkdata, "%s", CMCDesiredREFSMMATUpdate(REFSMMAT, AGCEpoch));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Entry REFSMMAT");
		}
	}
	break;
	case 203: //MISSION CP MCC5
	case 204: //MISSION CP MCC6
	case 205: //MISSION CP PRELIMINARY MCC7
	case 206: //MISSION CP MCC7
	case 300: //MISSION CP Generic MCC
	{
		EntryOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase, MCCtime;
		MATRIX3 REFSMMAT;
		char manname[8];
		SV sv;

		AP11MNV * form = (AP11MNV *)pad;

		//Just so things don't break
		if (calcParams.TEI == 0)
		{
			calcParams.TEI = OrbMech::HHMMSSToSS(89, 15, 7);
		}
		if (calcParams.EI == 0)
		{
			calcParams.EI = OrbMech::HHMMSSToSS(146, 50, 0);
		}

		if (fcn == 203)
		{
			MCCtime = calcParams.TEI + 15.0*3600.0;
			sprintf(manname, "MCC-5");
		}
		else if (fcn == 204)
		{
			MCCtime = calcParams.TEI + 33.0*3600.0;
			sprintf(manname, "MCC-6");
		}
		else if (fcn == 205 || fcn == 206)
		{
			MCCtime = calcParams.EI - 2.0*3600.0;
			sprintf(manname, "MCC-7");
		}
		else
		{
			MCCtime = calcParams.TEI + 5.0*3600.0;
			sprintf(manname, "MCC");
		}

		//Only corridor control after EI-24h
		if (MCCtime > calcParams.EI - 24.0*3600.0)
		{
			entopt.type = RTCC_ENTRY_CORRIDOR;
		}
		else
		{
			entopt.type = RTCC_ENTRY_MCC;
		}

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.entrylongmanual = true;
		entopt.GETbase = GETbase;
		entopt.impulsive = RTCC_NONIMPULSIVE;
		entopt.lng = -165.0*RAD;
		entopt.ReA = 0;
		entopt.TIGguess = MCCtime;
		entopt.vessel = calcParams.src;

		EntryTargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, ReA, prec); //Target Load for uplink

		//Apollo 8 Mission Rules
		if (MCCtime > calcParams.EI - 50.0*3600.0)
		{
			if (length(res.dV_LVLH) < 1.0*0.3048)
			{
				scrubbed = true;
			}
		}
		else
		{
			if (length(res.dV_LVLH) < 2.0*0.3048)
			{
				scrubbed = true;
			}
		}

		if (scrubbed)
		{
			sprintf(upDesc, "%s has been scrubbed.", manname);

			//Entry prediction without maneuver
			EntryUpdateCalc(sv, entopt.GETbase, 0, true, &res);

			res.dV_LVLH = _V(0, 0, 0);
			res.P30TIG = entopt.TIGguess;
		}
		else
		{
			if (fcn == 203 || fcn == 206)
			{
				REFSMMATOpt refsopt;
				refsopt.GETbase = GETbase;
				refsopt.dV_LVLH = res.dV_LVLH;
				refsopt.P30TIG = res.P30TIG;
				refsopt.REFSMMATdirect = false;
				refsopt.REFSMMATopt = 3;
				refsopt.vessel = calcParams.src;

				REFSMMAT = REFSMMATCalc(&refsopt);
			}
			else
			{
				REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			}

			opt.dV_LVLH = res.dV_LVLH;
			opt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), res.dV_LVLH);;
			opt.GETbase = GETbase;
			opt.HeadsUp = false;
			opt.REFSMMAT = REFSMMAT;
			opt.TIG = res.P30TIG;
			opt.vessel = calcParams.src;
			opt.vesseltype = 0;

			AP11ManeuverPAD(&opt, *form);
			sprintf(form->purpose, manname);
			form->lat = res.latitude*DEG;
			form->lng = res.longitude*DEG;
			form->RTGO = res.RTGO;
			form->VI0 = res.VIO / 0.3048;
			form->GET05G = res.GET05G;

			if (fcn == 203)//MCC5
			{
				sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase), CMCRetrofireExternalDeltaVUpdate(res.latitude, res.longitude, res.P30TIG, res.dV_LVLH), AGCREFSMMATUpdate(REFSMMAT, AGCEpoch));
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "LM state vector, target load, Entry REFSMMAT");
				}
			}
			else if (fcn == 204)//MCC6
			{
				sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase), CMCRetrofireExternalDeltaVUpdate(res.latitude, res.longitude, res.P30TIG, res.dV_LVLH));
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "LM state vector, target load");
				}
			}
			else if (fcn == 205)//Prel. MCC7
			{
				sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase));
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "LM state vector");
				}
			}
			else if (fcn == 206)//MCC7
			{
				sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), CMCRetrofireExternalDeltaVUpdate(res.latitude, res.longitude, res.P30TIG, res.dV_LVLH), AGCREFSMMATUpdate(REFSMMAT, AGCEpoch));
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM state vector, target load, Entry REFSMMAT");
				}
			}
			else if (fcn == 300)//generic MCC
			{
				sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), CMCRetrofireExternalDeltaVUpdate(res.latitude, res.longitude, res.P30TIG, res.dV_LVLH), CMCDesiredREFSMMATUpdate(REFSMMAT, AGCEpoch));
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM state vector, target load, Desired Entry REFSMMAT");
				}
			}
		}

		//Save for further use
		calcParams.EI = res.GET400K;
		DeltaV_LVLH = res.dV_LVLH;
		TimeofIgnition = res.P30TIG;
		SplashLatitude = res.latitude;
		SplashLongitude = res.longitude;
	}
	break;
	case 207: //MISSION CP PRELIMINARY ENTRY PAD
	{
		AP11ENT * form = (AP11ENT *)pad;

		LunarEntryPADOpt entopt;
		double GETbase;

		GETbase = getGETBase();

		if (length(DeltaV_LVLH) != 0.0)
		{
			entopt.direct = false;
		}
		else
		{
			entopt.direct = true;
		}
		entopt.dV_LVLH = DeltaV_LVLH;
		entopt.GETbase = GETbase;
		entopt.lat = SplashLatitude;
		entopt.lng = SplashLongitude;
		entopt.P30TIG = TimeofIgnition;
		entopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		entopt.vessel = calcParams.src;

		LunarEntryPAD(&entopt, *form);
		sprintf(form->Area[0], "MIDPAC");
		if (entopt.direct == false)
		{
			sprintf(form->remarks[0], "Assumes MCC7");
		}
	}
	break;
	case 208: //MISSION CP FINAL ENTRY PAD AND STATE VECTORS
	{
		AP11ENT * form = (AP11ENT *)pad;
		SV sv;

		LunarEntryPADOpt entopt;
		double GETbase;

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.direct = true;
		entopt.GETbase = GETbase;
		entopt.lat = SplashLatitude;
		entopt.lng = SplashLongitude;
		entopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		entopt.vessel = calcParams.src;

		LunarEntryPAD(&entopt, *form);
		sprintf(form->Area[0], "MIDPAC");

		sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase), CMCEntryUpdate(SplashLatitude, SplashLongitude));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM and LM state vectors, entry target");
		}
	}
	break;
	}

	return scrubbed;
}

bool RTCC::CalculationMTP_C(int fcn, LPVOID &pad, char * upString, char * upDesc)
{
	char* uplinkdata = new char[1000];
	bool preliminary = true;
	bool scrubbed = false;

	double AGCEpoch = 40221.525;

	switch (fcn) {
	case 1: // MISSION C PHASING BURN
	{
		LambertMan lambert;
		AP7ManPADOpt opt;
		SV sv_A, sv_P;
		double GET_TIG;
		VECTOR3 dV, dV_LVLH;

		AP7MNV * form = (AP7MNV *)pad;

		sv_A = StateVectorCalc(calcParams.src);
		sv_P = StateVectorCalc(calcParams.tgt);

		GET_TIG = OrbMech::HHMMSSToSS(3, 20, 0);

		lambert = set_lambertoptions(sv_A, sv_P, getGETBase(), GET_TIG, OrbMech::HHMMSSToSS(26, 25, 0), 15, RTCC_LAMBERT_XAXIS, RTCC_LAMBERT_SPHERICAL, _V(76.5 * 1852, 0, 0), 0);
		LambertTargeting(&lambert, dV, dV_LVLH);

		opt.GETbase = getGETBase();
		opt.vessel = calcParams.src;
		opt.TIG = GET_TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_RCS;
		opt.directiontype = RTCC_DIRECTIONTYPE_MINUSX;
		opt.HeadsUp = false;
		opt.sxtstardtime = 0;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.navcheckGET = 0;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "PHASING BURN");
		sprintf(form->remarks, "heads down, retrograde, -X thrusters");
	}
	break;
	case 2: // MISSION C CONTINGENCY DEORBIT (6-4) TARGETING
	{
		AP7MNV * form = (AP7MNV *)pad;

		MATRIX3 REFSMMAT;
		double GETbase;
		EarthEntryOpt entopt;
		EntryResults res;
		AP7ManPADOpt opt;
		REFSMMATOpt refsopt;
		SV sv;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();

		entopt.vessel = calcParams.src;
		entopt.GETbase = GETbase;
		entopt.impulsive = RTCC_NONIMPULSIVE;
		entopt.lng = -163.0*RAD;
		entopt.nominal = RTCC_ENTRY_NOMINAL;
		entopt.ReA = 0;
		entopt.TIGguess = 8 * 60 * 60 + 55 * 60;
		entopt.entrylongmanual = true;

		BlockDataProcessor(&entopt, &res); //Target Load for uplink

		refsopt.vessel = calcParams.src;
		refsopt.GETbase = GETbase;
		refsopt.dV_LVLH = res.dV_LVLH;
		refsopt.P30TIG = res.P30TIG;
		refsopt.REFSMMATopt = 1;

		REFSMMAT = REFSMMATCalc(&refsopt); //REFSMMAT for uplink

		opt.vessel = calcParams.src;
		opt.GETbase = GETbase;
		opt.TIG = res.P30TIG;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.HeadsUp = true;
		opt.sxtstardtime = -25 * 60;
		opt.REFSMMAT = REFSMMAT;
		opt.navcheckGET = 8 * 60 * 60 + 17 * 60;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "6-4 DEORBIT");

		sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), CMCRetrofireExternalDeltaVUpdate(res.latitude, res.longitude, res.P30TIG, res.dV_LVLH), CMCDesiredREFSMMATUpdate(REFSMMAT, AGCEpoch));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, target load, REFSMMAT");
		}
	}
	break;
	case 3: //MISSION C BLOCK DATA UPDATE 2
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { 136.7*RAD, -16.2*RAD, -22.0*RAD, -33.0*RAD, -28.2*RAD, -62.0*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(13,29,36),OrbMech::HHMMSSToSS(14,19,12),OrbMech::HHMMSSToSS(15,54,48),OrbMech::HHMMSSToSS(17,28,48),OrbMech::HHMMSSToSS(19,8,6),OrbMech::HHMMSSToSS(20,34,3) };
		char area[6][10] = { "009-3B", "010-AC", "011-AC", "012-AC", "013-2A", "014-1B" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;
		
		AP7BlockData(&opt, *form);
	}
	break;
	case 4: //MISSION C 2ND PHASING MANEUVER
	{
		AP7ManPADOpt opt;
		LambertMan lambert;
		SV sv_A, sv_P;
		double GET_TIG;
		VECTOR3 dV, dV_LVLH;

		AP7MNV * form = (AP7MNV *)pad;

		sv_A = StateVectorCalc(calcParams.src);
		sv_P = StateVectorCalc(calcParams.tgt);

		GET_TIG = OrbMech::HHMMSSToSS(15, 52, 0);

		lambert = set_lambertoptions(sv_A, sv_P, getGETBase(), OrbMech::HHMMSSToSS(15, 52, 0), OrbMech::HHMMSSToSS(26, 25, 0), 7, RTCC_LAMBERT_XAXIS, RTCC_LAMBERT_SPHERICAL, _V(76.5 * 1852, 0, 0), 0);

		LambertTargeting(&lambert, dV, dV_LVLH);

		if (length(dV_LVLH) < 1.0*0.3048)
		{
			scrubbed = true;
		}

		if (scrubbed)
		{
			sprintf(upDesc, "Second Phasing Maneuver not necessary.");
		}
		else
		{
			opt.GETbase = getGETBase();
			opt.vessel = calcParams.src;
			opt.TIG = GET_TIG;
			opt.dV_LVLH = dV_LVLH;
			opt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), dV_LVLH);
			opt.HeadsUp = true;
			opt.sxtstardtime = 0;
			opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			opt.navcheckGET = 0;

			AP7ManeuverPAD(&opt, *form);
			sprintf(form->purpose, "PHASING BURN");
			sprintf(form->remarks, "heads up, +X thrusters");
		}
	}
	break;
	case 5: //MISSION C BLOCK DATA UPDATE 3
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[7] = { -62.9*RAD, -63.0*RAD, -62.9*RAD, -68.5*RAD, -162.4*RAD, -162.3*RAD, -163.3*RAD };
		double GETI[7] = { OrbMech::HHMMSSToSS(22,10,42),OrbMech::HHMMSSToSS(23,46,41),OrbMech::HHMMSSToSS(25,22,18),OrbMech::HHMMSSToSS(26,56,28),OrbMech::HHMMSSToSS(29,43,42),OrbMech::HHMMSSToSS(31,18,29), OrbMech::HHMMSSToSS(32,53,56) };
		char area[7][10] = { "015-1A", "016-1B", "017-1A", "018-1A", "019-4A", "020-4A", "021-4A" };
		char **test;
		test = new char*[7];
		for (int i = 0;i < 7;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 7;

		AP7BlockData(&opt, *form);
	}
	break;
	case 7://MISSION C FINAL NCC1 MANEUVER
	{
		preliminary = false;
	}
	case 6: //MISSION C PRELIMINARY NCC1 MANEUVER
	{
		LambertMan lambert;
		AP7ManPADOpt opt;
		double GET_TIG_imp, P30TIG, GETBase;
		VECTOR3 dV, dV_LVLH;
		SV sv_A, sv_P;

		AP7MNV * form = (AP7MNV *)pad;

		GETBase = getGETBase();

		sv_A = StateVectorCalc(calcParams.src);
		sv_P = StateVectorCalc(calcParams.tgt);

		GET_TIG_imp = OrbMech::HHMMSSToSS(26, 25, 0);

		lambert = set_lambertoptions(sv_A, sv_P, GETBase, GET_TIG_imp, OrbMech::HHMMSSToSS(28, 0, 0), 1, RTCC_LAMBERT_MULTIAXIS, RTCC_LAMBERT_PERTURBED, _V(0, 0, 8 * 1852), -1.32*RAD);

		LambertTargeting(&lambert, dV);
		PoweredFlightProcessor(sv_A, GETBase, GET_TIG_imp, RTCC_VESSELTYPE_CSM, RTCC_ENGINETYPE_SPSDPS, 0.0, dV, P30TIG, dV_LVLH);

		opt.GETbase = GETBase;
		opt.vessel = calcParams.src;
		opt.TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.HeadsUp = true;
		opt.sxtstardtime = -30 * 60;
		if (preliminary)
		{
			REFSMMATOpt refsopt;

			refsopt.GETbase = GETBase;
			refsopt.REFSMMATopt = 2;
			refsopt.REFSMMATTime = 23 * 60 * 60 + 24 * 60 + 8;
			refsopt.vessel = calcParams.src;

			opt.REFSMMAT = REFSMMATCalc(&refsopt);
			opt.navcheckGET = 25 * 60 * 60 + 41 * 60 + 55;
		}
		else
		{
			opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			opt.navcheckGET = 25 * 60 * 60 + 42 * 60;
			sprintf(form->remarks, "posigrade, heads up");
		}

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "NCC1");

		sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv_A, true, AGCEpoch, GETBase), AGCStateVectorUpdate(sv_P, false, AGCEpoch, GETBase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM and LM state vectors, target load");
		}
	}
	break;
	case 8: //MISSION C NCC2 MANEUVER
	{
		LambertMan lambert;
		AP7ManPADOpt opt;
		SV sv_A, sv_P;
		double GETBase, GET_TIG_imp, P30TIG;
		VECTOR3 dV;

		AP7MNV * form = (AP7MNV *)pad;

		sv_A = StateVectorCalc(calcParams.src);
		sv_P = StateVectorCalc(calcParams.tgt);

		GETBase = getGETBase();
		GET_TIG_imp = OrbMech::HHMMSSToSS(27, 30, 0);
		
		lambert = set_lambertoptions(sv_A, sv_P, GETBase, GET_TIG_imp, OrbMech::HHMMSSToSS(28, 0, 0), 0, RTCC_LAMBERT_MULTIAXIS, RTCC_LAMBERT_PERTURBED, _V(0, 0, 8 * 1852), -1.32*RAD);
		LambertTargeting(&lambert, dV);

		if (length(dV) < 10.0*0.3048)
		{
			scrubbed = true;
		}

		if (scrubbed)
		{
			sprintf(upDesc, "NCC2 has been scrubbed.");
		}
		else
		{
			VECTOR3 dV_LVLH;
			int enginetype;
			enginetype = SPSRCSDecision(SPS_THRUST / sv_A.mass, dV);

			PoweredFlightProcessor(sv_A, GETBase, GET_TIG_imp, RTCC_VESSELTYPE_CSM, enginetype, 0.0, dV, P30TIG, dV_LVLH);

			opt.GETbase = GETBase;
			opt.vessel = calcParams.src;
			opt.TIG = P30TIG;
			opt.dV_LVLH = dV_LVLH;
			opt.enginetype = enginetype;
			opt.HeadsUp = false;
			opt.sxtstardtime = 0;
			opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			opt.navcheckGET = 0;

			AP7ManeuverPAD(&opt, *form);
			sprintf(form->purpose, "NCC2");

			sprintf(uplinkdata, "%s", AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "Target load");
			}
		}
	}
	break;
	case 9: //MISSION C NSR MANEUVER
	{
		CDHOpt cdhopt;
		AP7ManPADOpt opt;
		double P30TIG, GETbase;
		VECTOR3 dV_LVLH;
		SV sv_A, sv_P;

		sv_A = StateVectorCalc(calcParams.src); //State vector for uplink
		sv_P = StateVectorCalc(calcParams.tgt); //State vector for uplink
		GETbase = getGETBase();

		AP7MNV * form = (AP7MNV *)pad;

		cdhopt.GETbase = GETbase;
		cdhopt.impulsive = RTCC_NONIMPULSIVE;
		cdhopt.CDHtimemode = 1;
		cdhopt.DH = 8 * 1852;
		cdhopt.TIG = 28 * 60 * 60;
		cdhopt.target = calcParams.tgt;
		cdhopt.vessel = calcParams.src;

		CDHcalc(&cdhopt, dV_LVLH, P30TIG);

		opt.GETbase = GETbase;
		opt.vessel = calcParams.src;
		opt.TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.HeadsUp = false;
		opt.sxtstardtime = 0;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.navcheckGET = 27 * 60 * 60 + 17 * 60;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "NSR");
		sprintf(form->remarks, "heads down, retrograde");

		sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv_A, true, AGCEpoch, GETbase), AGCStateVectorUpdate(sv_P, false, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM and LM state vectors, target load");
		}
	}
	break;
	case 10: //MISSION C TPI MANEUVER
	{
		LambertMan lambert;
		AP7TPIPADOpt opt;
		SV sv_A, sv_P;
		double T1, T2, GETbase;
		VECTOR3 dV, dV_LVLH;

		AP7TPI * form = (AP7TPI *)pad;

		sv_A = StateVectorCalc(calcParams.src);
		sv_P = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();
		T1 = lambertelev(calcParams.src, calcParams.tgt, GETbase, 27.45*RAD);
		T2 = T1 + 35.0*60.0;

		lambert = set_lambertoptions(sv_A, sv_P, GETbase, T1, T2, 0, RTCC_LAMBERT_MULTIAXIS, RTCC_LAMBERT_PERTURBED, _V(0, 0, 0), 0);
	
		LambertTargeting(&lambert, dV, dV_LVLH);

		opt.dV_LVLH = dV_LVLH;
		opt.GETbase = GETbase;
		opt.target = calcParams.tgt;
		opt.TIG = T1;
		opt.vessel = calcParams.src;

		AP7TPIPAD(&opt, *form);
	}
	break;
	case 11: //MISSION C FINAL SEPARATION MANEUVER
	{
		AP7ManPADOpt opt;

		AP7MNV * form = (AP7MNV *)pad;

		opt.dV_LVLH = _V(2.0*0.3048, 0.0, 0.0);
		opt.enginetype = RTCC_ENGINETYPE_RCS;
		opt.directiontype = RTCC_DIRECTIONTYPE_MINUSX;
		opt.GETbase = getGETBase();
		opt.HeadsUp = false;
		opt.navcheckGET = 0;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.sxtstardtime = 0;
		opt.TIG = OrbMech::HHMMSSToSS(30.0, 20.0, 0.0);
		opt.vessel = calcParams.src;	

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "PHASING BURN");
		sprintf(form->remarks, "posigrade, heads down, -X Thrusters");
	}
	break;
	case 12: //MISSION C BLOCK DATA 4
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[7] = { -163.3*RAD, 138.8*RAD, 138.5*RAD, 135.6*RAD, -18.0*RAD, -24.0*RAD, -25.4*RAD };
		double GETI[7] = { OrbMech::HHMMSSToSS(32,53,42),OrbMech::HHMMSSToSS(34,13,54),OrbMech::HHMMSSToSS(35,49,27),OrbMech::HHMMSSToSS(37,24,28),OrbMech::HHMMSSToSS(38,14,11),OrbMech::HHMMSSToSS(39,49,27), OrbMech::HHMMSSToSS(41,26,13) };
		char area[7][10] = { "021-4A", "022-3B", "023-3A", "024-3B", "025-AC", "026-AC", "027-2B" };
		char **test;
		test = new char*[7];
		for (int i = 0;i < 7;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 7;

		AP7BlockData(&opt, *form);
	}
	break;
	case 13: //MISSION C BLOCK DATA 5
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -25.4*RAD, -28.8*RAD, -63.3*RAD, -66.2*RAD, -66.2*RAD, -66.2*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(41,26,13),OrbMech::HHMMSSToSS(43,2,5),OrbMech::HHMMSSToSS(44,27,52),OrbMech::HHMMSSToSS(46,3,34),OrbMech::HHMMSSToSS(47,39,29),OrbMech::HHMMSSToSS(49,15,9) };
		char area[6][10] = { "027-2B", "028-2B", "029-1B", "030-1A", "031-1B", "032-1A"};
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 14: //MISSION C BLOCK DATA 6
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -145.0*RAD, 150.0*RAD, 139.0*RAD, -165.9*RAD, 139.0*RAD, 137.4*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(52,5,9),OrbMech::HHMMSSToSS(53,21,42),OrbMech::HHMMSSToSS(54,55,7),OrbMech::HHMMSSToSS(56,46,40),OrbMech::HHMMSSToSS(58,7,17),OrbMech::HHMMSSToSS(59,42,35) };
		char area[6][10] = { "033-4C", "034-3C", "035-3B", "036-4A", "037-3A", "038-3A" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 15: //MISSION C BLOCK DATA 7
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { 134.5*RAD, -19.9*RAD, -22.9*RAD, -26.4*RAD, -54.9*RAD, -64.9*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(61,17,53),OrbMech::HHMMSSToSS(62,7,40),OrbMech::HHMMSSToSS(63,43,46),OrbMech::HHMMSSToSS(65,19,43),OrbMech::HHMMSSToSS(66,47,22),OrbMech::HHMMSSToSS(68,20,59) };
		char area[6][10] = { "039-3B", "040-AC", "041-AC", "042-2A", "043-1C", "044-1A" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 16: //MISSION C BLOCK DATA 8
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -63.8*RAD, -63.8*RAD, -64.9*RAD, -165.0*RAD, -165.0*RAD, -137.1*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(69,57,34),OrbMech::HHMMSSToSS(71,33,18),OrbMech::HHMMSSToSS(73,8,47),OrbMech::HHMMSSToSS(75,52,32),OrbMech::HHMMSSToSS(77,28,29),OrbMech::HHMMSSToSS(78,47,51) };
		char area[6][10] = { "045-1A", "046-1A", "047-1A", "048-4A", "049-4B", "050-3A" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 17: //MISSION C SPS-3: SCS MANEUVER AND SLOSH DAMPING TEST
	{
		AP7MNV * form = (AP7MNV *)pad;

		GMPOpt orbopt;
		REFSMMATOpt refsopt;
		AP7ManPADOpt manopt;
		VECTOR3 dV_LVLH;
		double P30TIG, GETbase;
		OBJHANDLE gravref;
		MATRIX3 REFSMMAT;
		SV sv;

		GETbase = getGETBase();
		gravref = AGCGravityRef(calcParams.src);

		sv = StateVectorCalc(calcParams.src);

		orbopt.GETbase = GETbase;
		orbopt.h_apo = 160.1*1852.0;
		orbopt.h_peri = 90.3*1852.0;
		orbopt.inc = 31.26*RAD;
		orbopt.TIG_GET = OrbMech::HHMMSSToSS(75, 48, 0);
		orbopt.useSV = false;
		orbopt.vessel = calcParams.src;
		orbopt.impulsive = RTCC_NONIMPULSIVE;
		orbopt.csmlmdocked = false;

		GeneralManeuverProcessor(&orbopt, dV_LVLH, P30TIG);

		refsopt.dV_LVLH = dV_LVLH;
		refsopt.GETbase = GETbase;
		refsopt.P30TIG = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = true;
		manopt.navcheckGET = OrbMech::HHMMSSToSS(75, 5, 0);
		manopt.REFSMMAT = REFSMMAT;
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.sxtstardtime = 0.0;

		AP7ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "SPS-3");

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, target load");
		}
	}
	break;
	case 18: //MISSION C BLOCK DATA 9
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { 138.0*RAD, 138.0*RAD, 137.0*RAD, -15.0*RAD, -22.0*RAD, -30.0*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(80,23,36),OrbMech::HHMMSSToSS(82,0,15),OrbMech::HHMMSSToSS(83,36,27),OrbMech::HHMMSSToSS(84,22,7),OrbMech::HHMMSSToSS(85,55,7),OrbMech::HHMMSSToSS(87,28,31) };
		char area[6][10] = { "051-3B", "052-3B", "053-3A", "054-AC", "55-AC", "056-AC" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 19: //MISSION C BLOCK DATA 10
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -27.0*RAD, -60.0*RAD, -64.0*RAD, -64.4*RAD, -64.5*RAD, -64.0*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(89,6,20),OrbMech::HHMMSSToSS(90,30,41),OrbMech::HHMMSSToSS(92,6,54),OrbMech::HHMMSSToSS(93,43,29),OrbMech::HHMMSSToSS(95,20,0),OrbMech::HHMMSSToSS(96,52,38) };
		char area[6][10] = { "057-2A", "058-1C", "059-1A", "060-1A", "61-1A", "062-1A" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 20: //MISSION C BLOCK DATA 11
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -159.9*RAD, -160.0*RAD, -160.0*RAD, 136.3*RAD, 136.2*RAD, 134.4*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(99,36,59),OrbMech::HHMMSSToSS(101,13,24),OrbMech::HHMMSSToSS(102,46,4),OrbMech::HHMMSSToSS(104,4,38),OrbMech::HHMMSSToSS(105,41,4),OrbMech::HHMMSSToSS(107,13,10) };
		char area[6][10] = { "063-4A", "064-4A", "065-4A", "066-3A", "067-3A", "068-3B" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 21: //MISSION C BLOCK DATA 12
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { 130.0*RAD, -23.0*RAD, -32.0*RAD, -27.0*RAD, -61.5*RAD, -64.5*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(108,47,28),OrbMech::HHMMSSToSS(109,37,43),OrbMech::HHMMSSToSS(111,10,33),OrbMech::HHMMSSToSS(112,48,12),OrbMech::HHMMSSToSS(114,13,4),OrbMech::HHMMSSToSS(115,48,12) };
		char area[6][10] = { "069-3C", "070-AC", "071-AC", "072-2A", "073-1B", "074-1B" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 22: //MISSION C BLOCK DATA 13
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -65.0*RAD, -65.0*RAD, -63.0*RAD, -160.0*RAD, -160.0*RAD, -161.1*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(117,24,4),OrbMech::HHMMSSToSS(119,0,11),OrbMech::HHMMSSToSS(120,33,36),OrbMech::HHMMSSToSS(123,17,25),OrbMech::HHMMSSToSS(124,53,43),OrbMech::HHMMSSToSS(126,27,32) };
		char area[6][10] = { "075-1A", "076-1A", "077-1A", "078-4A", "079-4A", "080-4A" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 23: //MISSION C SPS-4: MINIMUM IMPULSE
	{
		AP7MNV * form = (AP7MNV *)pad;
		AP7ManPADOpt opt;
		REFSMMATOpt refsopt;
		double t_burn, F, m, dv, GETbase, P30TIG;
		VECTOR3 dV_LVLH;
		MATRIX3 REFSMMAT;
		OBJHANDLE gravref;
		SV sv;

		GETbase = getGETBase();
		F = SPS_THRUST;
		t_burn = 0.5;
		m = calcParams.src->GetMass();
		dv = F / m*t_burn;
		gravref = AGCGravityRef(calcParams.src);

		sv = StateVectorCalc(calcParams.src);

		dV_LVLH = _V(dv, 0.0, 0.0);
		GETbase = getGETBase();
		P30TIG = OrbMech::HHMMSSToSS(120, 43, 0);

		refsopt.GETbase = GETbase;
		refsopt.REFSMMATopt = 2;
		refsopt.REFSMMATTime = OrbMech::HHMMSSToSS(120, 43, 0);
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.navcheckGET = 120.0*3600.0;
		opt.REFSMMAT = REFSMMAT;
		opt.sxtstardtime = -25.0*3600.0;
		opt.TIG = P30TIG;
		opt.vessel = calcParams.src;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "SPS-4");

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, target load");
		}
	}
	break;
	case 24: //MISSION C BLOCK DATA 14
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { 136.0*RAD, 136.0*RAD, 134.0*RAD, 170.0*RAD, -22.0*RAD, -25.0*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(127,45,11),OrbMech::HHMMSSToSS(129,21,34),OrbMech::HHMMSSToSS(130,53,56),OrbMech::HHMMSSToSS(132,33,15),OrbMech::HHMMSSToSS(133,19,17),OrbMech::HHMMSSToSS(134,53,55) };
		char area[6][10] = { "081-3A", "082-3A", "083-3B", "084-CC", "085-AC", "086-2C" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 25: //MISSION C BLOCK DATA 15
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -27.0*RAD, -60.0*RAD, -62.2*RAD, -62.0*RAD, -62.2*RAD, -63.0*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(136,29,19),OrbMech::HHMMSSToSS(137,54,53),OrbMech::HHMMSSToSS(139,30,6),OrbMech::HHMMSSToSS(141,6,7),OrbMech::HHMMSSToSS(142,42,26),OrbMech::HHMMSSToSS(144,16,25) };
		char area[6][10] = { "087-2A", "088-1B", "089-1A", "090-1B", "091-1A", "092-1A" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 26: //MISSION C BLOCK DATA 16
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -162.0*RAD, -161.9*RAD, -163.0*RAD, 133.9*RAD, 133.9*RAD, 141.9*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(146,58,14),OrbMech::HHMMSSToSS(148,34,16),OrbMech::HHMMSSToSS(150,9,20),OrbMech::HHMMSSToSS(151,25,41),OrbMech::HHMMSSToSS(153,1,35),OrbMech::HHMMSSToSS(154,38,44) };
		char area[6][10] = { "093-4A", "094-4A", "095-4A", "096-3A", "097-3A", "098-3C" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 27: //MISSION C BLOCK DATA 17
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -18.0*RAD, -24.0*RAD, -23.9*RAD, -27.0*RAD, -61.6*RAD, -62.7*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(155,27,54),OrbMech::HHMMSSToSS(157,0,51),OrbMech::HHMMSSToSS(158,35,56),OrbMech::HHMMSSToSS(160,10,26),OrbMech::HHMMSSToSS(161,35,40),OrbMech::HHMMSSToSS(163,10,40) };
		char area[6][10] = { "099-AC", "100-AC", "101-2C", "102-2A", "103-1B", "104-1A" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 28: //MISSION C SPS-5: PUGS TEST AND MTVC
	{
		AP7MNV * form = (AP7MNV *)pad;

		GMPOpt orbopt;
		REFSMMATOpt refsopt;
		AP7ManPADOpt manopt;
		VECTOR3 dV_LVLH, Rlat, Vlat;
		double P30TIG, GETbase, TIGguess;
		OBJHANDLE gravref;
		MATRIX3 REFSMMAT;
		SV sv, sv1;

		GETbase = getGETBase();
		gravref = AGCGravityRef(calcParams.src);
		TIGguess = OrbMech::HHMMSSToSS(165, 0, 0);

		sv = StateVectorCalc(calcParams.src);

		sv1 = coast(sv, TIGguess - (sv.MJD - GETbase)*24.0*3600.0);

		orbopt.GETbase = GETbase;
		orbopt.h_apo = 240.6*1852.0;
		orbopt.h_peri = 89.8*1852.0;
		orbopt.inc = 30.09*RAD;
		orbopt.TIG_GET = TIGguess + OrbMech::findlatitude(sv1.R, sv1.V, GETbase + TIGguess/24.0/3600.0, gravref, 28.7319*RAD, true, Rlat, Vlat);//OrbMech::HHMMSSToSS(165, 0, 0);
		orbopt.vessel = calcParams.src;
		orbopt.impulsive = RTCC_IMPULSIVE;
		orbopt.csmlmdocked = false;

		GeneralManeuverProcessor(&orbopt, dV_LVLH, P30TIG);

		refsopt.dV_LVLH = dV_LVLH;
		refsopt.GETbase = GETbase;
		refsopt.P30TIG = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = true;
		manopt.navcheckGET = OrbMech::HHMMSSToSS(164, 18, 0);
		manopt.REFSMMAT = REFSMMAT;
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.sxtstardtime = -20.0*60.0;

		AP7ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "SPS-5");

		form->Vc += 100.0;
		sprintf(form->remarks, "MTVC takeover at TIG+%.0f seconds, manual cutoff at DV counter equal 100 ft/s.", form->burntime-30.0);

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, target load");
		}
	}
	break;
	case 29: //MISSION C BLOCK DATA 18
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -62.7*RAD, -63.1*RAD, -162.5*RAD, -162.5*RAD, -162.5*RAD, 139.0*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(164,46,6),OrbMech::HHMMSSToSS(166,21,55),OrbMech::HHMMSSToSS(168,59,3),OrbMech::HHMMSSToSS(170,40,38),OrbMech::HHMMSSToSS(172,22,48),OrbMech::HHMMSSToSS(173,34,54) };
		char area[6][10] = { "105-1A", "106-1A", "107-4A", "108-4A", "109-4A", "110-3A" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 30: //MISSION C BLOCK DATA 19
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { 138.9*RAD, 152.0*RAD, -9.0*RAD, -23.9*RAD, -31.0*RAD, -26.9*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(175,17,19),OrbMech::HHMMSSToSS(177,0,44),OrbMech::HHMMSSToSS(177,42,42),OrbMech::HHMMSSToSS(179,14,47),OrbMech::HHMMSSToSS(180,48,41),OrbMech::HHMMSSToSS(182,26,21) };
		char area[6][10] = { "111-3A", "112-3C", "113-AC", "114-AC", "115-AC", "116-2A" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 31: //MISSION C BLOCK DATA 20
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -55.2*RAD, -60.0*RAD, -60.0*RAD, -70.2*RAD, -63.0*RAD, -162.0*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(183,54,59),OrbMech::HHMMSSToSS(185,31,45),OrbMech::HHMMSSToSS(187,12,18),OrbMech::HHMMSSToSS(188,54,8),OrbMech::HHMMSSToSS(190,35,19),OrbMech::HHMMSSToSS(193,9,9) };
		char area[6][10] = { "117-1C", "118-1A", "119-1B", "120-1A", "121-1A", "122-4A" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 32: //MISSION C BLOCK DATA 21
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -162.0*RAD, -163.5*RAD, -166.0*RAD, -10.0*RAD, -12.0*RAD, -18.0*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(194,50,14),OrbMech::HHMMSSToSS(196,31,45),OrbMech::HHMMSSToSS(198,9,57),OrbMech::HHMMSSToSS(198,43,50),OrbMech::HHMMSSToSS(200,17,18),OrbMech::HHMMSSToSS(201,50,35) };
		char area[6][10] = { "123-4A", "124-4A", "125-CC", "126-AC", "127-AC", "128-AC" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 33: //MISSION C BLOCK DATA 22
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -25.0*RAD, -27.0*RAD, -27.1*RAD, -62.0*RAD, -60.0*RAD, -60.0*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(203,23,55),OrbMech::HHMMSSToSS(204,58,45),OrbMech::HHMMSSToSS(206,35,31),OrbMech::HHMMSSToSS(208,2,22),OrbMech::HHMMSSToSS(209,40,53),OrbMech::HHMMSSToSS(211,20,47) };
		char area[6][10] = { "129-AC", "130-2A", "131-2C", "132-1C", "133-1A", "134-1A" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 34: //MISSION C SPS-6: MINIMUM IMPULSE
	{
		AP7MNV * form = (AP7MNV *)pad;
		AP7ManPADOpt opt;
		REFSMMATOpt refsopt;
		double t_burn, F, m, dv, GETbase, P30TIG;
		VECTOR3 dV_LVLH;
		MATRIX3 REFSMMAT;
		OBJHANDLE gravref;
		SV sv;

		GETbase = getGETBase();
		F = SPS_THRUST;
		t_burn = 0.5;
		m = calcParams.src->GetMass();
		dv = F / m*t_burn;
		gravref = AGCGravityRef(calcParams.src);

		sv = StateVectorCalc(calcParams.src);

		dV_LVLH = _V(0.0, dv, 0.0);
		GETbase = getGETBase();
		P30TIG = OrbMech::HHMMSSToSS(210, 8, 0);

		refsopt.dV_LVLH = dV_LVLH;
		refsopt.GETbase = GETbase;
		refsopt.P30TIG = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.navcheckGET = OrbMech::HHMMSSToSS(209, 20, 0);
		opt.REFSMMAT = REFSMMAT;
		opt.sxtstardtime = 0.0;
		opt.TIG = P30TIG;
		opt.vessel = calcParams.src;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "SPS-6");

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, target load");
		}
	}
	break;
	case 35: //MISSION C BLOCK DATA 23
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -63.0*RAD, -161.8*RAD, -162.0*RAD, -161.7*RAD, -164.0*RAD, -5.0*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(213,0,32),OrbMech::HHMMSSToSS(215,38,45),OrbMech::HHMMSSToSS(217,17,27),OrbMech::HHMMSSToSS(218,57,54),OrbMech::HHMMSSToSS(220,39,3),OrbMech::HHMMSSToSS(221,19,6) };
		char area[6][10] = { "135-1A", "136-4A", "137-4B", "138-4A", "139-4B", "140-AC" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 36: //MISSION C SV PAD
	{
		P27PAD * form = (P27PAD *)pad;
		P27Opt opt;

		opt.GETbase = getGETBase();
		opt.navcheckGET = OrbMech::HHMMSSToSS(215, 44, 0);
		opt.SVGET = OrbMech::HHMMSSToSS(216, 14, 0);
		opt.vessel = calcParams.src;

		P27PADCalc(&opt, AGCEpoch, *form);
	}
	break;
	case 37: //MISSION C BLOCK DATA 24
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -10.0*RAD, -8.0*RAD, -20.0*RAD, -31.0*RAD, -27.0*RAD, -27.0*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(222,51,52),OrbMech::HHMMSSToSS(224,26,0),OrbMech::HHMMSSToSS(225,58,13),OrbMech::HHMMSSToSS(227,30,42),OrbMech::HHMMSSToSS(229,6,36),OrbMech::HHMMSSToSS(230,43,18) };
		char area[6][10] = { "141-AC", "142-AC", "143-AC", "144-AC", "145-2A", "146-2C" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 38: //MISSION C BLOCK DATA 25
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -63.0*RAD, -64.5*RAD, -64.4*RAD, -63.0*RAD, -161.5*RAD, -161.5*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(232,9,47),OrbMech::HHMMSSToSS(233,46,42),OrbMech::HHMMSSToSS(235,25,39),OrbMech::HHMMSSToSS(237,7,5),OrbMech::HHMMSSToSS(239,48,35),OrbMech::HHMMSSToSS(241,29,11) };
		char area[6][10] = { "147-1B", "148-1A", "149-1A", "150-1A", "151-4A", "152-4A" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 39: //MISSION C SPS-7: SCS MANEUVER
	{
		AP7MNV * form = (AP7MNV *)pad;

		GMPOpt orbopt;
		REFSMMATOpt refsopt;
		AP7ManPADOpt manopt;
		VECTOR3 dV_LVLH;
		double P30TIG, GETbase;
		OBJHANDLE gravref;
		MATRIX3 REFSMMAT;
		SV sv;

		GETbase = getGETBase();
		gravref = AGCGravityRef(calcParams.src);

		sv = StateVectorCalc(calcParams.src);

		orbopt.GETbase = GETbase;
		orbopt.h_apo = 230.3*1852.0;
		orbopt.h_peri = 90.0*1852.0;
		orbopt.inc = 29.88*RAD;
		orbopt.TIG_GET = OrbMech::HHMMSSToSS(239, 6, 12);
		orbopt.useSV = false;
		orbopt.vessel = calcParams.src;
		orbopt.impulsive = RTCC_NONIMPULSIVE;
		orbopt.csmlmdocked = false;

		GeneralManeuverProcessor(&orbopt, dV_LVLH, P30TIG);

		refsopt.dV_LVLH = dV_LVLH;
		refsopt.GETbase = GETbase;
		refsopt.P30TIG = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = true;
		manopt.navcheckGET = OrbMech::HHMMSSToSS(238, 24, 0);
		manopt.REFSMMAT = REFSMMAT;
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.sxtstardtime = 0.0;

		AP7ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "SPS-7");

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, target load");
		}
	}
	break;
	case 40: //MISSION C BLOCK DATA 26
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[6] = { -161.0*RAD, -161.0*RAD, -10.0*RAD, -11.0*RAD, -17.0*RAD, -25.0*RAD };
		double GETI[6] = { OrbMech::HHMMSSToSS(243,11,5),OrbMech::HHMMSSToSS(244,47,45),OrbMech::HHMMSSToSS(245,22,27),OrbMech::HHMMSSToSS(246,55,49),OrbMech::HHMMSSToSS(248,28,57),OrbMech::HHMMSSToSS(250,2,0) };
		char area[6][10] = { "153-4A", "154-4C", "155-AC", "156-AC", "157-AC", "158-AC" };
		char **test;
		test = new char*[6];
		for (int i = 0;i < 6;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 6;

		AP7BlockData(&opt, *form);
	}
	break;
	case 41: //MISSION C BLOCK DATA 27
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		double lng[7] = { -33.0*RAD, -26.5*RAD, -62.0*RAD, -64.2*RAD, -64.5*RAD, -64.2*RAD, -67.0*RAD };
		double GETI[7] = { OrbMech::HHMMSSToSS(251,35,18),OrbMech::HHMMSSToSS(253,13,19),OrbMech::HHMMSSToSS(254,39,51),OrbMech::HHMMSSToSS(256,16,31),OrbMech::HHMMSSToSS(257,55,28),OrbMech::HHMMSSToSS(259,39,18),OrbMech::HHMMSSToSS(261,16,45) };
		char area[7][10] = { "159-AC", "160-2A", "161-1B", "162-1A", "163-1A", "164-1A", "165-1B" };
		char **test;
		test = new char*[7];
		for (int i = 0;i < 7;i++)
		{
			test[i] = new char[10];
			test[i] = &area[i][0];
		}

		opt.area = test;
		opt.GETI = GETI;
		opt.lng = lng;
		opt.n = 7;

		AP7BlockData(&opt, *form);
	}
	break;
	case 42: //MISSION C NOMINAL DEORBIT MANEUVER PAD
	{
		AP7MNV * form = (AP7MNV *)pad;

		EarthEntryOpt entopt;
		EntryResults res;
		AP7ManPADOpt opt;
		REFSMMATOpt refsopt;
		MATRIX3 REFSMMAT;
		double GETbase;
		SV sv;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();

		entopt.vessel = calcParams.src;
		entopt.GETbase = GETbase;
		entopt.impulsive = RTCC_NONIMPULSIVE;
		entopt.lng = -64.17*RAD;
		entopt.nominal = RTCC_ENTRY_NOMINAL;
		entopt.ReA = -2.062*RAD;
		entopt.TIGguess = OrbMech::HHMMSSToSS(259, 39, 16);
		entopt.entrylongmanual = true;
		entopt.useSV = false;

		BlockDataProcessor(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, ReA, prec); //Target Load for uplink

		TimeofIgnition = res.P30TIG;
		SplashLatitude = res.latitude;
		SplashLongitude = res.longitude;
		DeltaV_LVLH = res.dV_LVLH;

		refsopt.vessel = calcParams.src;
		refsopt.GETbase = GETbase;
		refsopt.dV_LVLH = res.dV_LVLH;
		refsopt.P30TIG = res.P30TIG;
		refsopt.REFSMMATopt = 1;

		REFSMMAT = REFSMMATCalc(&refsopt); //REFSMMAT for uplink

		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.navcheckGET = res.P30TIG - 40.0*60.0;
		opt.REFSMMAT = REFSMMAT;
		opt.sxtstardtime = -20.0*60.0;
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "164-1A RETROFIRE");

		sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), CMCRetrofireExternalDeltaVUpdate(res.latitude, res.longitude, res.P30TIG, res.dV_LVLH), CMCDesiredREFSMMATUpdate(REFSMMAT, AGCEpoch));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, target load, Retrofire REFSMMAT");
		}
	}
	break;
	case 43: //MISSION C NOMINAL DEORBIT ENTRY PAD
	{
		AP7ENT * form = (AP7ENT *)pad;

		EarthEntryPADOpt opt;
		REFSMMATOpt refsopt;
		MATRIX3 REFSMMAT;

		refsopt.vessel = calcParams.src;
		refsopt.GETbase = getGETBase();
		refsopt.dV_LVLH = DeltaV_LVLH;
		refsopt.P30TIG = TimeofIgnition;
		refsopt.REFSMMATopt = 1;

		REFSMMAT = REFSMMATCalc(&refsopt);
	
		opt.dV_LVLH = DeltaV_LVLH;
		opt.GETbase = getGETBase();
		opt.P30TIG = TimeofIgnition;
		opt.REFSMMAT = REFSMMAT;
		opt.vessel = calcParams.src;
		opt.preburn = true;
		opt.lat = SplashLatitude;
		opt.lng = SplashLongitude;

		EarthOrbitEntry(&opt, *form);
		sprintf(form->Area[0], "164-1A");
		form->Lat[0] = SplashLatitude*DEG;
		form->Lng[0] = SplashLongitude*DEG;
	}
	break;
	case 44: //MISSION C POSTBURN ENTRY PAD
	{
		AP7ENT * form = (AP7ENT *)pad;

		EarthEntryPADOpt opt;

		opt.dV_LVLH = DeltaV_LVLH;
		opt.GETbase = getGETBase();
		opt.lat = SplashLatitude;
		opt.lng = SplashLongitude;
		opt.P30TIG = TimeofIgnition;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.preburn = false;
		opt.vessel = calcParams.src;

		EarthOrbitEntry(&opt, *form);
	}
	break;
	case 50: //GENERIC CSM STATE VECTOR UPDATE
	{
		SV sv;
		double GETbase;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();

		sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 51: //GENERIC CSM AND TARGET STATE VECTOR UPDATE
	{
		SV sv_A, sv_P;
		double GETbase;

		sv_A = StateVectorCalc(calcParams.src); //State vector for uplink
		sv_P = StateVectorCalc(calcParams.tgt); //State vector for uplink
		GETbase = getGETBase();

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv_A, true, AGCEpoch, GETbase), AGCStateVectorUpdate(sv_P, false, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM and LM state vectors");
		}
	}
	break;
	case 52: //CSM STATE VECTOR UPDATE AND NAV CHECK PAD
	{
		AP7NAV * form = (AP7NAV *)pad;

		SV sv;
		double GETbase;

		GETbase = getGETBase();
		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		NavCheckPAD(sv, *form, GETbase);

		sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 53: //GENERIC CSM AND TARGET STATE VECTOR UPDATE AND CSM NAV CHECK PAD
	{
		AP7NAV * form = (AP7NAV *)pad;

		SV sv_A, sv_P;
		double GETbase;

		GETbase = getGETBase();

		sv_A = StateVectorCalc(calcParams.src); //State vector for uplink
		sv_P = StateVectorCalc(calcParams.tgt); //State vector for uplink

		NavCheckPAD(sv_A, *form, GETbase);

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv_A, true, AGCEpoch, GETbase), AGCStateVectorUpdate(sv_P, false, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM and LM state vectors");
		}
	}
	break;
	case 54: //GENERIC SV PAD
	{
		P27PAD * form = (P27PAD *)pad;
		P27Opt opt;

		opt.GETbase = getGETBase();
		opt.SVGET = (oapiGetSimMJD() - opt.GETbase)*24.0*3600.0;
		opt.navcheckGET = opt.SVGET + 30 * 60;
		opt.vessel = calcParams.src;

		P27PADCalc(&opt, AGCEpoch, *form);
	}
	break;
	}

	return scrubbed;
}

bool RTCC::CalculationMTP_F(int fcn, LPVOID &pad, char * upString, char * upDesc)
{
	char uplinkdata[1000];
	bool preliminary = false;
	bool scrubbed = false;

	//Hardcoded for now, better solution at some point...
	double AGCEpoch = 40221.525;
	double LSLat = 0.732*RAD;
	double LSLng = 23.647*RAD;
	double LSAlt = -3073.263;
	double LSAzi = -91.0*RAD;
	double EMPLat = -4.933294*RAD;

	int LGCREFSAddrOffs = -2;
	int LGCDeltaVAddr = 3431;

	switch (fcn) {
	case 1: //TLI+90 PAD + State Vector
	{
		EntryOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase, TLIBase, TIG, CSMmass;
		SV sv, sv1;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		TLIBase = floor((TimeofIgnition / 1800.0) + 0.5)*1800.0; //Round to next half hour
		TIG = TLIBase + 90.0*60.0;
		entopt.lng = -25.0*RAD;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		CSMmass = 28862.0;

		sv1.mass = CSMmass;
		sv1.gravref = AGCGravityRef(calcParams.src);
		sv1.MJD = GETbase + calcParams.TLI / 24.0 / 3600.0;
		sv1.R = calcParams.R_TLI;
		sv1.V = calcParams.V_TLI;

		entopt.entrylongmanual = true;
		entopt.GETbase = GETbase;
		entopt.impulsive = RTCC_NONIMPULSIVE;
		entopt.ReA = 0;
		entopt.TIGguess = TIG;
		entopt.type = RTCC_ENTRY_ABORT;
		entopt.vessel = calcParams.src;
		entopt.useSV = true;
		entopt.RV_MCC = sv1;

		EntryTargeting(&entopt, &res); //Target Load for uplink

		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;
		opt.useSV = true;
		opt.RV_MCC = sv1;

		AP11ManeuverPAD(&opt, *form);
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->Weight = CSMmass / 0.45359237;
		form->GET05G = res.GET05G;

		sprintf(form->purpose, "TLI+90");
		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM and LM state vectors");
		}
	}
	break;
	case 2: //TLI+4 PAD
	{
		AP11BLKOpt opt;
		double CSMmass, GETbase;
		SV sv1;

		P37PAD * form = (P37PAD *)pad;

		GETbase = getGETBase();

		CSMmass = 28862.0;

		sv1.mass = CSMmass;
		sv1.gravref = AGCGravityRef(calcParams.src);
		sv1.MJD = GETbase + calcParams.TLI / 24.0 / 3600.0;
		sv1.R = calcParams.R_TLI;
		sv1.V = calcParams.V_TLI;

		opt.n = 1;
		opt.GETI = new double[opt.n];
		opt.lng = new double[opt.n];

		opt.GETI[0] = OrbMech::HHMMSSToSS(6, 30, 0);
		opt.lng[0] = -165.0*RAD;
		opt.useSV = true;
		opt.RV_MCC = sv1;

		AP11BlockData(&opt, *form);

		delete opt.GETI;
		delete opt.lng;
	}
	break;
	case 3: //TLI PAD
	{
		TLIPADOpt opt;
		double GETbase;

		TLIPAD * form = (TLIPAD *)pad;

		GETbase = getGETBase();

		opt.dV_LVLH = DeltaV_LVLH;
		opt.GETbase = GETbase;
		opt.R_TLI = calcParams.R_TLI;
		opt.V_TLI = calcParams.V_TLI;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = TimeofIgnition;
		opt.TLI = calcParams.TLI;
		opt.vessel = calcParams.src;
		opt.SeparationAttitude = _V(PI, 120.0*RAD, 40.0*RAD);
		opt.uselvdc = true;

		TLI_PAD(&opt, *form);
	}
	break;
	case 4: //Evasive Maneuver
	{
		AP11ManPADOpt opt;

		AP11MNV * form = (AP11MNV *)pad;

		opt.dV_LVLH = _V(5.1, 0.0, 19.0)*0.3048;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = getGETBase();
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = calcParams.TLI + 2.0*3600.0;
		opt.vessel= calcParams.src;
		opt.vesseltype = 1;

		AP11ManeuverPAD(&opt, *form);

		sprintf(form->purpose, "Evasive");
	}
	break;
	case 5: //Block Data 1
	case 6: //Block Data 2
	{
		AP11BLKOpt opt;
		double GETbase;

		P37PAD * form = (P37PAD *)pad;

		GETbase = getGETBase();

		double TLIbase = floor((calcParams.TLI / 1800.0) + 0.5)*1800.0; //Round to next half hour

		if (fcn == 5)
		{
			opt.n = 3;

			opt.GETI = new double[opt.n];
			opt.lng = new double[opt.n];

			opt.GETI[0] = TLIbase + 11.0*3600.0;
			opt.lng[0] = -165.0*RAD;
			opt.GETI[1] = TLIbase + 25.0*3600.0;
			opt.lng[1] = -165.0*RAD;
			opt.GETI[2] = TLIbase + 35.0*3600.0;
			opt.lng[2] = -165.0*RAD;
		}
		else
		{
			opt.n = 4;

			opt.GETI = new double[opt.n];
			opt.lng = new double[opt.n];

			opt.GETI[0] = TLIbase + 25.0*3600.0;
			opt.lng[0] = -165.0*RAD;
			opt.GETI[1] = TLIbase + 35.0*3600.0;
			opt.lng[1] = -165.0*RAD;
			opt.GETI[2] = TLIbase + 44.0*3600.0;
			opt.lng[2] = -165.0*RAD;
			opt.GETI[3] = TLIbase + 53.0*3600.0;
			opt.lng[3] = -165.0*RAD;
		}

		AP11BlockData(&opt, *form);

		delete opt.GETI;
		delete opt.lng;
	}
	break;
	case 7: //PTC REFSMMAT
	{
		 MATRIX3 REFSMMAT = _M(-0.5, -0.8660254, 0.0, -0.79453912, 0.45872741, 0.39784005, -0.34453959, 0.19892003, -0.91745479);

		sprintf(uplinkdata, "%s", CMCDesiredREFSMMATUpdate(REFSMMAT, AGCEpoch, true));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "PTC REFSMMAT");
		}
	}
	break;
	case 10: //MCC-1
	case 11: //MCC-2
	{
		AP11ManPADOpt manopt;
		double GETbase, P30TIG, MCC1GET, MCC2GET, MCC3GET;
		VECTOR3 dV_LVLH;
		SV sv;
		char manname[8];

		if (fcn == 10)
		{
			sprintf(manname, "MCC1");
		}
		else if (fcn == 11)
		{
			sprintf(manname, "MCC2");
		}

		if (calcParams.LOI == 0)
		{
			calcParams.LOI = OrbMech::HHMMSSToSS(75.0, 49.0, 40.2);
		}

		if (calcParams.TLAND == 0)
		{
			calcParams.TLAND = OrbMech::HHMMSSToSS(102.0, 47.0, 11.0);
		}

		double TLIbase = floor((TimeofIgnition / 60.0) + 0.5)*60.0; //Round TLI ignition time to next minute

		MCC1GET= TLIbase + 9.0*3600.0;
		MCC2GET = TLIbase + 25.0*3600.0;
		MCC3GET = calcParams.LOI - 22.0*3600.0;

		AP11MNV * form = (AP11MNV *)pad;
		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		MCCFRMan opt;
		TLMCCResults res;

		opt.type = 0;
		opt.lat = EMPLat;
		opt.PeriGET = calcParams.LOI;
		opt.h_peri = 60.0 * 1852.0;
		opt.alt = LSAlt;
		opt.azi = LSAzi;
		opt.csmlmdocked = true;
		opt.GETbase = GETbase;

		opt.LOIh_apo = 170.0*1852.0;
		opt.LOIh_peri = 60.0*1852.0;
		opt.LSlat = LSLat;
		opt.LSlng = LSLng;
		opt.PeriGET = calcParams.LOI;
		opt.t_land = calcParams.TLAND;
		opt.vessel = calcParams.src;

		//Evaluate MCC-3 DV
		opt.MCCGET = MCC3GET;
		TranslunarMidcourseCorrectionTargetingFreeReturn(&opt, &res);

		if (length(res.dV_LVLH) < 25.0*0.3048)
		{
			scrubbed = true;
		}
		else
		{
			if (fcn == 10)
			{
				opt.MCCGET = MCC1GET;
			}
			else
			{
				opt.MCCGET = MCC2GET;
			}

			TranslunarMidcourseCorrectionTargetingFreeReturn(&opt, &res);

			//Scrub MCC-1 if DV is less than 50 ft/s
			if (fcn == 10 && length(res.dV_LVLH) < 50.0*0.3048)
			{
				scrubbed = true;
			}
			else
			{
				calcParams.alt_node = res.NodeAlt;
				calcParams.lat_node = res.NodeLat;
				calcParams.lng_node = res.NodeLng;
				calcParams.GET_node = res.NodeGET;
				calcParams.LOI = res.PericynthionGET;
				dV_LVLH = res.dV_LVLH;
				P30TIG = res.P30TIG;
			}
		}

		if (scrubbed)
		{
			sprintf(upDesc, "%s has been scrubbed. CSM state vector.", manname);

			sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
			}
		}
		else
		{
			manopt.dV_LVLH = dV_LVLH;
			manopt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), dV_LVLH);
			manopt.GETbase = GETbase;
			manopt.HeadsUp = false;
			manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			manopt.TIG = P30TIG;
			manopt.vessel = calcParams.src;
			manopt.vesseltype = 1;

			AP11ManeuverPAD(&manopt, *form);
			sprintf(form->purpose, manname);

			sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load");
			}
		}
	}
	break;
	case 12: //Lunar Flyby PAD
	{
		RTEFlybyOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		SV sv;
		double GETbase;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.EntryLng = -165.0*RAD;
		entopt.GETbase = GETbase;
		entopt.returnspeed = 0;
		entopt.FlybyType = 0;
		entopt.TIGguess = calcParams.LOI - 5.0*3600.0;
		entopt.vessel = calcParams.src;

		RTEFlybyTargeting(&entopt, &res);

		SV sv_peri = FindPericynthion(res.sv_postburn);
		double h_peri = length(sv_peri.R) - oapiGetSize(oapiGetObjectByName("Moon"));

		opt.alt = LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 1;

		AP11ManeuverPAD(&opt, *form);

		sprintf(form->purpose, "Flyby");
		sprintf(form->remarks, "Height of pericynthion is %.0f NM", h_peri / 1852.0);
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->GET05G = res.GET05G;

		//Save parameters for further use
		SplashLatitude = res.latitude;
		SplashLongitude = res.longitude;
		calcParams.TEI = res.P30TIG;
		calcParams.EI = res.GET400K;

		sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 13: //MCC-3
	{
		AP11ManPADOpt manopt;
		LOIMan loiopt;
		MCCNodeMan opt;
		VECTOR3 dV_LVLH, dV_LOI;
		SV sv, sv_peri, sv_node;
		double GETbase, MCCGET, P30TIG, r_M, TIG_LOI, h_peri, h_node;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();
		r_M = oapiGetSize(oapiGetObjectByName("Moon"));
		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		loiopt.alt = LSAlt;
		loiopt.azi = LSAzi;
		loiopt.csmlmdocked = true;
		loiopt.GETbase = GETbase;
		loiopt.h_apo = 170.0*1852.0;
		loiopt.h_peri = 60.0*1852.0;
		loiopt.impulsive = RTCC_IMPULSIVE;
		loiopt.lat = LSLat;
		loiopt.lng = LSLng;
		loiopt.t_land = calcParams.TLAND;
		loiopt.vessel = calcParams.src;

		LOITargeting(&loiopt, dV_LOI, TIG_LOI, sv_node);

		sv_peri = FindPericynthion(sv);

		h_peri = length(sv_peri.R) - r_M;
		h_node = length(sv_node.R) - r_M;

		//Maneuver execution criteria
		if (h_peri > 50.0*1852.0 && h_peri < 70.0*1852.0)
		{
			if (h_node > 50.0*1852.0 && h_node < 75.0*1852.0)
			{
				scrubbed = true;
			}
		}

		if (scrubbed)
		{
			sprintf(upDesc, "MCC-3 has been scrubbed. CSM state vector.");

			sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
			}
		}
		else
		{
			MCCGET = calcParams.LOI - 22.0*3600.0;

			opt.lat = calcParams.lat_node;
			opt.lng = calcParams.lng_node;
			opt.NodeGET = calcParams.GET_node;
			opt.h_node = calcParams.alt_node;
			opt.csmlmdocked = true;
			opt.GETbase = GETbase;
			opt.MCCGET = MCCGET;
			opt.vessel = calcParams.src;

			TranslunarMidcourseCorrectionTargetingNodal(&opt, dV_LVLH, P30TIG);

			manopt.dV_LVLH = dV_LVLH;
			manopt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), dV_LVLH);
			manopt.GETbase = GETbase;
			manopt.HeadsUp = false;
			manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			manopt.TIG = P30TIG;
			manopt.vessel = calcParams.src;
			manopt.vesseltype = 1;

			AP11ManeuverPAD(&manopt, *form);
			sprintf(form->purpose, "MCC-3");

			sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load");
			}
		}
	}
	break;
	case 14: //MCC-4
	{
		LOIMan loiopt;
		REFSMMATOpt refsopt;
		VECTOR3 dV_LVLH, dV_LOI;
		SV sv, sv_peri, sv_node;
		MATRIX3 REFSMMAT;
		double GETbase, MCCGET, P30TIG, r_M, TIG_LOI, h_peri, h_node;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();
		r_M = oapiGetSize(oapiGetObjectByName("Moon"));
		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		loiopt.alt = LSAlt;
		loiopt.azi = LSAzi;
		loiopt.csmlmdocked = true;
		loiopt.GETbase = GETbase;
		loiopt.h_apo = 170.0*1852.0;
		loiopt.h_peri = 60.0*1852.0;
		loiopt.impulsive = RTCC_IMPULSIVE;
		loiopt.lat = LSLat;
		loiopt.lng = LSLng;
		loiopt.t_land = calcParams.TLAND;
		loiopt.vessel = calcParams.src;

		LOITargeting(&loiopt, dV_LOI, TIG_LOI, sv_node);

		sv_peri = FindPericynthion(sv);

		h_peri = length(sv_peri.R) - r_M;
		h_node = length(sv_node.R) - r_M;

		//Maneuver execution criteria
		if (h_peri > 50.0*1852.0 && h_peri < 70.0*1852.0)
		{
			if (h_node > 50.0*1852.0 && h_node < 75.0*1852.0)
			{
				scrubbed = true;
			}
		}

		//REFSMMAT calculation
		refsopt.GETbase = GETbase;
		refsopt.LSAzi = LSAzi;
		refsopt.LSLat = LSLat;
		refsopt.LSLng = LSLng;
		refsopt.REFSMMATopt = 8;
		refsopt.REFSMMATTime = calcParams.TLAND;

		REFSMMAT = REFSMMATCalc(&refsopt);

		if (scrubbed)
		{
			sprintf(upDesc, "MCC-4 has been scrubbed. CSM state vector, Landing Site REFSMMAT.");

			sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), CMCDesiredREFSMMATUpdate(REFSMMAT, AGCEpoch));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
			}
		}
		else
		{
			AP11ManPADOpt manopt;
			MCCNodeMan opt;

			MCCGET = calcParams.LOI - 5.0*3600.0;

			opt.lat = calcParams.lat_node;
			opt.lng = calcParams.lng_node;
			opt.NodeGET = calcParams.GET_node;
			opt.h_node = calcParams.alt_node;
			opt.csmlmdocked = true;
			opt.GETbase = GETbase;
			opt.MCCGET = MCCGET;
			opt.vessel = calcParams.src;

			TranslunarMidcourseCorrectionTargetingNodal(&opt, dV_LVLH, P30TIG);

			manopt.dV_LVLH = dV_LVLH;
			manopt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), dV_LVLH);
			manopt.GETbase = GETbase;
			manopt.HeadsUp = false;
			manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			manopt.TIG = P30TIG;
			manopt.vessel = calcParams.src;
			manopt.vesseltype = 1;

			AP11ManeuverPAD(&manopt, *form);
			sprintf(form->purpose, "MCC-4");

			sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH), CMCDesiredREFSMMATUpdate(REFSMMAT, AGCEpoch));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load, Landing Site REFSMMAT");
			}
		}

	}
	break;
	case 15: //PC+2 UPDATE
	{
		RTEFlybyOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		entopt.EntryLng = -165.0*RAD;
		entopt.returnspeed = 2;
		entopt.GETbase = GETbase;
		entopt.FlybyType = 1;
		entopt.vessel = calcParams.src;

		RTEFlybyTargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, EntryAng);

		opt.alt = LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);

		if (!REFSMMATDecision(form->Att*RAD))
		{
			REFSMMATOpt refsopt;
			MATRIX3 REFSMMAT;

			refsopt.dV_LVLH = res.dV_LVLH;
			refsopt.GETbase = GETbase;
			refsopt.P30TIG = res.P30TIG;
			refsopt.REFSMMATopt = 0;
			refsopt.vessel = calcParams.src;

			REFSMMAT = REFSMMATCalc(&refsopt);

			opt.HeadsUp = true;
			opt.REFSMMAT = REFSMMAT;
			AP11ManeuverPAD(&opt, *form);

			sprintf(form->remarks, "Requires realignment to preferred REFSMMAT");
		}
		sprintf(form->purpose, "PC+2");
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->GET05G = res.GET05G;

		//Save parameters for further use
		SplashLatitude = res.latitude;
		SplashLongitude = res.longitude;
		calcParams.TEI = res.P30TIG;
		calcParams.EI = res.GET400K;
	}
	break;
	case 20:	// MISSION F PRELIMINARY LOI-1 MANEUVER
	case 21:	// MISSION F LOI-1 MANEUVER
	{
		LOIMan opt;
		AP11ManPADOpt manopt;
		double GETbase, P30TIG;
		VECTOR3 dV_LVLH;
		SV sv;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		opt.csmlmdocked = true;
		opt.GETbase = GETbase;
		opt.h_apo = 170.0*1852.0;
		opt.h_peri = 60.0*1852.0;
		opt.alt = LSAlt;
		opt.azi = LSAzi;
		opt.lat = LSLat;
		opt.lng = LSLng;
		opt.t_land = calcParams.TLAND;
		opt.vessel = calcParams.src;

		LOITargeting(&opt, dV_LVLH, P30TIG);

		manopt.alt = LSAlt;
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.vesseltype = 1;

		AP11ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "LOI-1");
		sprintf(form->remarks, "LM weight is %.0f.", form->LMWeight);

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

		if (fcn == 21)
		{
			sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load");
			}
		}
	}
	break;
	case 22:	// MISSION F LOI-2 MANEUVER
	{
		LOI2Man opt;
		AP11ManPADOpt manopt;
		double GETbase, P30TIG;
		VECTOR3 dV_LVLH;
		SV sv;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		opt.alt = LSAlt;
		opt.csmlmdocked = true;
		opt.GETbase = GETbase;
		opt.h_circ = 60.0*1852.0;
		opt.vessel = calcParams.src;

		LOI2Targeting(&opt, dV_LVLH, P30TIG);

		manopt.alt = LSAlt;
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.vesseltype = 1;

		AP11ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "LOI-2");

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(P30TIG, dV_LVLH));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, target load");
		}
	}
	break;
	case 30: //TEI-1 UPDATE (PRE LOI-1)
	case 31: //TEI-4 UPDATE (PRE LOI-1)
	case 32: //TEI-5 UPDATE (PRE LOI-2)
	case 33: //TEI-10 UPDATE
	case 34: //TEI-22 UPDATE
	case 35: //TEI-23 UPDATE
	case 36: //TEI-24 UPDATE
	case 37: //TEI-25 UPDATE
	case 38: //TEI-26 UPDATE
	case 39: //TEI-27 UPDATE
	case 130: //TEI-29 UPDATE
	case 131: //TEI-30 UPDATE
	case 132: //PRELIMINARY TEI-31 UPDATE
	case 133: //FINAL TEI-31 UPDATE
	case 134: //TEI-32 UPDATE
	{
		TEIOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase;
		SV sv0, sv1, sv2;
		char manname[8];

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();
		sv0 = StateVectorCalc(calcParams.src); //State vector for uplink

		if (fcn == 30 || fcn == 31 || fcn == 32)
		{
			sv1 = ExecuteManeuver(calcParams.src, GETbase, TimeofIgnition, DeltaV_LVLH, sv0, GetDockedVesselMass(calcParams.src));
		}
		else
		{
			sv1 = sv0;
		}

		if (fcn == 30)
		{
			sprintf(manname, "TEI-1");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 31)
		{
			sprintf(manname, "TEI-4");
			sv2 = coast(sv1, 3.5*2.0*3600.0);
		}
		else if (fcn == 32)
		{
			sprintf(manname, "TEI-5");
			sv2 = coast(sv1, 2.5*2.0*3600.0);
		}
		else if (fcn == 33)
		{
			sprintf(manname, "TEI-10");
			sv2 = coast(sv1, 5.5*2.0*3600.0);
		}
		else if (fcn == 34)
		{
			sprintf(manname, "TEI-22");
			sv2 = coast(sv1, 5.5*2.0*3600.0);
		}
		else if (fcn == 35)
		{
			sprintf(manname, "TEI-23");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 36)
		{
			sprintf(manname, "TEI-24");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 37)
		{
			sprintf(manname, "TEI-25");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 38)
		{
			sprintf(manname, "TEI-26");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 39)
		{
			sprintf(manname, "TEI-27");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 130)
		{
			sprintf(manname, "TEI-29");
			sv2 = coast(sv1, 1.5*2.0*3600.0);
		}
		else if (fcn == 131)
		{
			sprintf(manname, "TEI-30");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 132)
		{
			sprintf(manname, "TEI-31");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 133)
		{
			sprintf(manname, "TEI-31");
			sv2 = sv1;
		}
		else if (fcn == 134)
		{
			sprintf(manname, "TEI-32");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}

		entopt.EntryLng = -165.0*RAD;
		entopt.GETbase = GETbase;
		entopt.returnspeed = 1;
		entopt.RV_MCC = sv2;
		entopt.useSV = true;
		entopt.vessel = calcParams.src;

		TEITargeting(&entopt, &res);

		opt.alt = LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.RV_MCC = sv1;
		opt.TIG = res.P30TIG;
		opt.useSV = true;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);
		sprintf(form->purpose, manname);
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->GET05G = res.GET05G;

		if (fcn != 134)
		{
			//Save parameters for further use
			SplashLatitude = res.latitude;
			SplashLongitude = res.longitude;
			calcParams.TEI = res.P30TIG;
			calcParams.EI = res.GET400K;
		}

		if (fcn == 133)
		{
			TimeofIgnition = res.P30TIG;
			DeltaV_LVLH = res.dV_LVLH;

			sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv0, true, AGCEpoch, GETbase, true), AGCExternalDeltaVUpdate(TimeofIgnition, DeltaV_LVLH));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "State vectors, target load");
			}
		}
	}
	break;
	case 40: //REV 1 MAP UPDATE
	{
		SV sv0, sv1, sv2;
		AP10MAPUPDATE upd_hyper, upd_ellip;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc(calcParams.src);
		LunarOrbitMapUpdate(sv0, getGETBase(), upd_hyper);
		
		sv1 = ExecuteManeuver(calcParams.src, getGETBase(), TimeofIgnition, DeltaV_LVLH, sv0, GetDockedVesselMass(calcParams.src));
		sv2 = coast(sv1, -30.0*60.0);
		LunarOrbitMapUpdate(sv2, getGETBase(), upd_ellip);

		form->Rev = 1;
		form->AOSGET = upd_ellip.AOSGET;
		form->LOSGET = upd_hyper.LOSGET;
		form->PMGET = upd_hyper.PMGET;
	}
	break;
	case 41: //REV 2 MAP UPDATE
	case 43: //REV 4 MAP UPDATE
	case 44: //REV 11 MAP UPDATE
	case 45: //REV 22 MAP UPDATE
	case 46: //REV 23 MAP UPDATE
	case 47: //REV 24 MAP UPDATE
	case 48: //REV 25 MAP UPDATE
	case 49: //REV 26 MAP UPDATE
	case 140: //REV 27 MAP UPDATE
	case 141: //REV 29 MAP UPDATE
	case 142: //REV 30 MAP UPDATE
	case 143: //REV 31 MAP UPDATE
	{
		SV sv0, sv1;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc(calcParams.src);

		if (fcn == 45)
		{
			sv1 = coast(sv0, 2.0*4.0*3600.0);
		}
		else if (fcn == 141)
		{
			sv1 = coast(sv0, 1.0*2.0*3600.0);
		}
		else
		{
			sv1 = sv0;
		}

		LunarOrbitMapUpdate(sv1, getGETBase(), *form);

		if (fcn == 41)
		{
			form->Rev = 2;
		}
		else if (fcn == 43)
		{
			form->Rev = 4;
		}
		else if (fcn == 44)
		{
			form->Rev = 11;
		}
		else if (fcn == 45 || fcn == 46 || fcn == 47 || fcn == 48 || fcn == 49)
		{
			form->Rev = fcn - 23;
		}
		else if (fcn == 140)
		{
			form->Rev = 27;
		}
		else if (fcn == 141)
		{
			form->Rev = 29;
		}
		else if (fcn == 142)
		{
			form->Rev = 30;
		}
		else if (fcn == 143)
		{
			form->Rev = 31;
		}
	}
	break;
	case 42: //REV 3 MAP UPDATE
	{
		SV sv0, sv1;
		AP10MAPUPDATE upd_preloi, upd_postloi;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc(calcParams.src);
		LunarOrbitMapUpdate(sv0, getGETBase(), upd_preloi);

		sv1 = ExecuteManeuver(calcParams.src, getGETBase(), TimeofIgnition, DeltaV_LVLH, sv0, GetDockedVesselMass(calcParams.src));
		LunarOrbitMapUpdate(sv0, getGETBase(), upd_postloi);

		form->Rev = 3;
		form->AOSGET = upd_postloi.AOSGET;
		form->LOSGET = upd_preloi.LOSGET;
		form->PMGET = upd_preloi.PMGET;
	}
	break;
	case 144: //TEI MAP UPDATE
	{
		SV sv0, sv1;
		AP10MAPUPDATE upd_pretei, upd_posttei;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc(calcParams.src);
		LunarOrbitMapUpdate(sv0, getGETBase(), upd_pretei);

		sv1 = ExecuteManeuver(calcParams.src, getGETBase(), TimeofIgnition, DeltaV_LVLH, sv0, GetDockedVesselMass(calcParams.src));
		LunarOrbitMapUpdate(sv0, getGETBase(), upd_posttei);

		form->Rev = 32;
		form->AOSGET = upd_posttei.AOSGET;
		form->LOSGET = upd_pretei.LOSGET;
		form->PMGET = upd_pretei.PMGET;
	}
	break;
	case 50: //REV 4 LANDMARK TRACKING PAD F-1
	case 51: //REV 4 LANDMARK TRACKING PAD B-1
	case 52: //REV 11 LANDMARK TRACKING PAD LLS-2
	case 53: //REV 24 LANDMARK TRACKING PADs
	case 54: //REV 25 LANDMARK TRACKING PADs
	case 55: //REV 26 LANDMARK TRACKING PADs
	case 56: //REV 27 LANDMARK TRACKING PADs
	case 57: //REV 30 LANDMARK TRACKING PADs
	{
		LMARKTRKPADOpt opt;

		AP11LMARKTRKPAD * form = (AP11LMARKTRKPAD *)pad;

		opt.GETbase = getGETBase();
		opt.vessel = calcParams.src;

		if (fcn == 50)
		{
			sprintf(form->LmkID[0], "F-1");
			opt.alt[0] = 0;
			opt.lat[0] = 1.6*RAD;
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(82, 27, 0);
			opt.lng[0] = 86.88*RAD;
			opt.entries = 1;
		}
		else if (fcn == 51)
		{
			sprintf(form->LmkID[0], "B-1");
			opt.alt[0] = -1.54*1852.0;
			opt.lat[0] = 2.522*RAD;
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(82, 45, 0);
			opt.lng[0] = 35.036*RAD;
			opt.entries = 1;
		}
		else if (fcn == 52)
		{
			sprintf(form->LmkID[0], "130");
			opt.alt[0] = -1.73*1852.0;
			opt.lat[0] = 1.266*RAD;
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(96, 35, 0);
			opt.lng[0] = 23.678*RAD;
			opt.entries = 1;
		}
		else if (fcn == 53 || fcn == 54 || fcn == 55 || fcn == 56)
		{
			if (fcn == 53)
			{
				opt.LmkTime[0] = OrbMech::HHMMSSToSS(121, 26, 0);
				opt.LmkTime[1] = OrbMech::HHMMSSToSS(121, 41, 0);
				opt.LmkTime[2] = OrbMech::HHMMSSToSS(121, 54, 0);
				opt.LmkTime[3] = OrbMech::HHMMSSToSS(122, 15, 0);
			}
			else if (fcn == 54)
			{
				opt.LmkTime[0] = OrbMech::HHMMSSToSS(123, 24, 0);
				opt.LmkTime[1] = OrbMech::HHMMSSToSS(123, 39, 0);
				opt.LmkTime[2] = OrbMech::HHMMSSToSS(123, 52, 0);
				opt.LmkTime[3] = OrbMech::HHMMSSToSS(124, 13, 0);
			}
			else if (fcn == 55)
			{
				opt.LmkTime[0] = OrbMech::HHMMSSToSS(125, 22, 0);
				opt.LmkTime[1] = OrbMech::HHMMSSToSS(125, 37, 0);
				opt.LmkTime[2] = OrbMech::HHMMSSToSS(125, 50, 0);
				opt.LmkTime[3] = OrbMech::HHMMSSToSS(126, 11, 0);
			}
			else if (fcn == 56)
			{
				opt.LmkTime[0] = OrbMech::HHMMSSToSS(127, 20, 0);
				opt.LmkTime[1] = OrbMech::HHMMSSToSS(127, 35, 0);
				opt.LmkTime[2] = OrbMech::HHMMSSToSS(127, 48, 0);
				opt.LmkTime[3] = OrbMech::HHMMSSToSS(128, 9, 0);
			}

			sprintf(form->LmkID[0], "CP-1");
			opt.alt[0] = 0.0;
			opt.lat[0] = 0.875*RAD;
			opt.lng[0] = 170.146*RAD;

			sprintf(form->LmkID[1], "CP-2");
			opt.alt[1] = 0.0;
			opt.lat[1] = 1.0*RAD;
			opt.lng[1] = 127.4*RAD;

			sprintf(form->LmkID[2], "F-1");
			opt.alt[2] = 0.0;
			opt.lat[2] = 1.6*RAD;
			opt.lng[2] = 86.88*RAD;

			sprintf(form->LmkID[3], "130");
			opt.alt[3] = -1.73*1852.0;
			opt.lat[3] = 1.266*RAD;
			opt.lng[3] = 23.678*RAD;

			opt.entries = 4;
		}
		else if (fcn == 57)
		{
			sprintf(form->LmkID[0], "B-1");
			opt.alt[0] = -1.54*1852.0;
			opt.lat[0] = 2.522*RAD;
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(134, 0, 0);
			opt.lng[0] = 35.036*RAD;

			sprintf(form->LmkID[1], "150");
			opt.alt[1] = -1.05*1852.0;
			opt.lat[1] = 0.283*RAD;
			opt.LmkTime[1] = OrbMech::HHMMSSToSS(134, 12, 0);
			opt.lng[1] = -1.428*RAD;

			opt.entries = 2;
		}

		LandmarkTrackingPAD(&opt, *form);
	}
	break;
	case 60: //STATE VETOR and LLS 2 REFSMMAT UPLINK
	{
		MATRIX3 REFSMMAT;
		VECTOR3 dV_LVLH_imp;
		double GETbase, t_PDI, t_land, CR, t_DOI_imp;
		SV sv;
		REFSMMATOpt opt;
		DOIMan doiopt;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();

		doiopt.alt = LSAlt;
		doiopt.csmlmdocked = false;
		doiopt.EarliestGET = OrbMech::HHMMSSToSS(99, 0, 0);
		doiopt.GETbase = GETbase;
		doiopt.lat = LSLat;
		doiopt.lng = LSLng;
		doiopt.N = 0;
		doiopt.opt = 0;
		doiopt.vessel = calcParams.tgt;
		doiopt.vesseltype = 1;

		DOITargeting(&doiopt, dV_LVLH_imp, t_DOI_imp, DeltaV_LVLH, TimeofIgnition, t_PDI, t_land, CR);

		calcParams.DOI = t_DOI_imp;
		calcParams.TLAND = t_land;

		opt.GETbase = GETbase;
		opt.LSLat = LSLat;
		opt.LSLng = LSLng;
		opt.REFSMMATopt = 5;
		opt.REFSMMATTime = calcParams.TLAND;
		opt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&opt);

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), CMCDesiredREFSMMATUpdate(REFSMMAT, AGCEpoch));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, LLS2 REFSMMAT");
		}
	}
	break;
	case 61: //CSM DAP DATA
	{
		AP10DAPDATA * form = (AP10DAPDATA *)pad;

		CSMDAPUpdate(calcParams.src, *form);
	}
	break;
	case 62: //LM DAP DATA
	{
		AP10DAPDATA * form = (AP10DAPDATA *)pad;

		LMDAPUpdate(calcParams.tgt, *form);
	}
	break;
	case 63: //GYRO TORQUING ANGLES
	{
		GENERICPAD * form = (GENERICPAD *)pad;
		LEM *lem = (LEM *)calcParams.tgt;

		VECTOR3 lmn20, csmn20, V42angles;

		csmn20.x = calcParams.src->imu.Gimbal.X;
		csmn20.y = calcParams.src->imu.Gimbal.Y;
		csmn20.z = calcParams.src->imu.Gimbal.Z;

		lmn20.x = lem->imu.Gimbal.X;
		lmn20.y = lem->imu.Gimbal.Y;
		lmn20.z = lem->imu.Gimbal.Z;

		V42angles = OrbMech::finealignLMtoCSM(lmn20, csmn20);

		sprintf(form->paddata, "GYRO TORQUING ANGLES\nX %+07.3f\nY %+07.3f\n Z %+07.3f", V42angles.x*DEG, V42angles.y*DEG, V42angles.z*DEG);
	}
	break;
	case 64: //LGC ACTIVATION UPDATE
	{
		SV sv;
		REFSMMATOpt opt;
		MATRIX3 REFSMMAT;
		double GETbase, TEPHEM0, tephem, t_AGC, t_actual, deltaT;
		LEM *lem;
		char clockupdate[128];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();
		lem = (LEM *)calcParams.tgt;
		TEPHEM0 = 40038.;

		tephem = GetTEPHEMFromAGC(&lem->agc.vagc);
		t_AGC = GetClockTimeFromAGC(&lem->agc.vagc);

		tephem = (tephem / 8640000.) + TEPHEM0;
		t_actual = (oapiGetSimMJD() - tephem) * 8640000.;
		deltaT = t_actual - t_AGC;

		IncrementAGCTime(clockupdate, deltaT);

		opt.GETbase = GETbase;
		opt.LSLat = LSLat;
		opt.LSLng = LSLng;
		opt.REFSMMATopt = 5;
		opt.REFSMMATTime = calcParams.TLAND;
		opt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&opt);

		sprintf(uplinkdata, "%s%s%s%s", clockupdate, AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase), AGCREFSMMATUpdate(REFSMMAT, AGCEpoch, LGCREFSAddrOffs));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "clock update, state vectors, LS REFSMMAT");
		}
	}
	break;
	case 70: //CSM SEPARATION BURN
	{
		AP11ManPADOpt opt;
		SV sv;
		VECTOR3 dV_LVLH;
		double GETbase, t_P, mu, t_Sep;

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();
		mu = GGRAV * oapiGetMass(sv.gravref);

		t_P = OrbMech::period(sv.R, sv.V, mu);
		t_Sep = floor(calcParams.DOI - t_P / 2.0);
		dV_LVLH = _V(0, 0, -2.5)*0.3048;

		opt.alt = LSAlt;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_RCS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = t_Sep;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "state vectors");
		}
	}
	break;
	case 71: //DESCENT ORBIT INSERTION
	{
		AP11LMManPADOpt opt;

		VECTOR3 dV_LVLH_imp;
		double GETbase, t_PDI, t_land, CR, t_DOI_imp, t_TPI_guess;
		SV sv_CSM, sv, sv_DOI;
		DOIMan doiopt;
		char GETbuffer[64];
		char TLANDbuffer[64];

		AP11LMMNV * form = (AP11LMMNV *)pad;

		sv_CSM = StateVectorCalc(calcParams.src);
		sv = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();

		doiopt.alt = LSAlt;
		doiopt.csmlmdocked = false;
		doiopt.EarliestGET = OrbMech::HHMMSSToSS(99, 0, 0);
		doiopt.GETbase = GETbase;
		doiopt.lat = LSLat;
		doiopt.lng = LSLng;
		doiopt.N = 0;
		doiopt.opt = 0;
		doiopt.vessel = calcParams.tgt;
		doiopt.vesseltype = 1;

		DOITargeting(&doiopt, dV_LVLH_imp, t_DOI_imp, DeltaV_LVLH, TimeofIgnition, t_PDI, t_land, CR);

		calcParams.DOI = t_DOI_imp;
		calcParams.TLAND = t_land;

		opt.alt = LSAlt;
		opt.csmlmdocked = false;
		opt.dV_LVLH = DeltaV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.TIG = TimeofIgnition;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);
		sprintf(form->purpose, "DOI");

		//Rendezvous Plan
		double MJD_Phasing;

		sv_DOI = ExecuteManeuver(calcParams.tgt, GETbase, TimeofIgnition, DeltaV_LVLH, sv, 0.0);
		MJD_Phasing = OrbMech::P29TimeOfLongitude(sv_DOI.R, sv_DOI.V, sv_DOI.MJD, sv_DOI.gravref, -12.5*RAD);
		calcParams.Phasing = (MJD_Phasing - GETbase)*24.0*3600.0;

		t_TPI_guess = OrbMech::HHMMSSToSS(105, 9, 0);
		calcParams.TPI = FindOrbitalMidnight(sv_CSM, GETbase, t_TPI_guess);

		RendezvousPlanner(calcParams.tgt, calcParams.src, sv_DOI, GETbase, calcParams.Phasing, calcParams.TPI, calcParams.Insertion, calcParams.CSI);

		OrbMech::format_time_HHMMSS(GETbuffer, calcParams.CSI);
		sprintf(form->remarks, "CSI time: %s, ", GETbuffer);
		OrbMech::format_time_HHMMSS(GETbuffer, calcParams.TPI);
		sprintf(form->remarks, "%sTPI time: %s, N equal to 1", form->remarks, GETbuffer);

		TLANDUpdate(TLANDbuffer, calcParams.TLAND, 2400);

		sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase), AGCExternalDeltaVUpdate(TimeofIgnition, DeltaV_LVLH, LGCDeltaVAddr), TLANDbuffer);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector, DOI target load");
		}
	}
	break;
	case 72: //PRELIMINARY PHASING MANEUVER
		preliminary = true;
	case 73: //PHASING MANEUVER
	{
		AP11LMManPADOpt opt;
		LambertMan lamopt;
		SV sv_CSM, sv_LM, sv_DOI;
		VECTOR3 dV, dV_LVLH;
		double GETbase, MJD_LS, t_LS, P30TIG, MJD_100E, t_100E;
		char GETbuffer[64];
		char GETbuffer2[64];

		AP11LMMNV * form = (AP11LMMNV *)pad;
		GETbase = getGETBase();

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);

		if (preliminary)
		{
			sv_DOI = ExecuteManeuver(calcParams.tgt, GETbase, TimeofIgnition, DeltaV_LVLH, sv_LM, 0.0);
		}
		else
		{
			sv_DOI = sv_LM;
		}

		lamopt.axis = RTCC_LAMBERT_MULTIAXIS;
		lamopt.GETbase = GETbase;
		lamopt.N = 0;
		lamopt.Offset = _V(-270.0*1852.0, 0.0, 60.0*1852.0 - 60000.0*0.3048);
		lamopt.Perturbation = RTCC_LAMBERT_PERTURBED;
		lamopt.sv_A = sv_DOI;
		lamopt.sv_P = sv_CSM;
		lamopt.T1 = calcParams.Phasing;
		lamopt.T2 = calcParams.Insertion;

		LambertTargeting(&lamopt, dV);
		PoweredFlightProcessor(sv_DOI, GETbase, lamopt.T1, RTCC_VESSELTYPE_LM, RTCC_ENGINETYPE_SPSDPS, 0.0, dV, P30TIG, dV_LVLH);

		opt.alt = LSAlt;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.RV_MCC = sv_DOI;
		opt.TIG = P30TIG;
		opt.useSV = true;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);
		sprintf(form->purpose, "Phasing");

		if (preliminary)
		{
			MJD_LS = OrbMech::P29TimeOfLongitude(sv_DOI.R, sv_DOI.V, sv_DOI.MJD, sv_DOI.gravref, LSLng);
			t_LS = (MJD_LS - GETbase)*24.0*3600.0;
			MJD_100E = OrbMech::P29TimeOfLongitude(sv_DOI.R, sv_DOI.V, sv_DOI.MJD, sv_DOI.gravref, 100.0*RAD);
			t_100E = (MJD_100E - GETbase)*24.0*3600.0;

			OrbMech::format_time_MMSS(GETbuffer, P30TIG - t_100E);
			OrbMech::format_time_MMSS(GETbuffer2, P30TIG - t_LS);
			sprintf(form->remarks, "100-degree east time is %s. Site 2 time is %s", GETbuffer, GETbuffer2);
		}
	}
	break;
	case 74: //PDI ABORT MANEUVER
	{
		AP11LMManPADOpt opt;
		DKIOpt dkiopt;
		SV sv_LM, sv_CSM, sv_DOI, sv_Phasing;
		VECTOR3 DV_Phasing, dV_LVLH;
		double GETbase, dt_peri, t_Abort, t_TPI_guess, t_TPI_Abort, t_CSI, P30TIG;
		char GETbuffer[64], GETbuffer2[64];

		AP11LMMNV * form = (AP11LMMNV *)pad;
		GETbase = getGETBase();

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);
		sv_DOI = ExecuteManeuver(calcParams.tgt, GETbase, TimeofIgnition, DeltaV_LVLH, sv_LM, 0.0);

		dt_peri = OrbMech::timetoperi_integ(sv_DOI.R, sv_DOI.V, sv_DOI.MJD, sv_DOI.gravref, sv_DOI.gravref);
		t_Abort = (sv_DOI.MJD - GETbase)*24.0*3600.0 + dt_peri;

		t_TPI_guess = OrbMech::HHMMSSToSS(103, 9, 0);
		t_TPI_Abort = FindOrbitalMidnight(sv_CSM, GETbase, t_TPI_guess);

		dkiopt.DH = 15.0*1852.0;
		dkiopt.E = 26.6*RAD;
		dkiopt.GETbase = GETbase;
		dkiopt.sv_A = sv_DOI;
		dkiopt.sv_P = sv_CSM;
		dkiopt.t_TIG = t_Abort;
		dkiopt.t_TPI = t_TPI_Abort;

		DockingInitiationProcessor(&dkiopt, DV_Phasing, t_CSI);
		PoweredFlightProcessor(sv_DOI, GETbase, t_Abort, 1, 1, 0.0, DV_Phasing, P30TIG, dV_LVLH);

		opt.alt = LSAlt;
		opt.csmlmdocked = false;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.TIG = P30TIG;
		opt.vessel = calcParams.tgt;
		opt.useSV = true;
		opt.RV_MCC = sv_DOI;

		AP11LMManeuverPAD(&opt, *form);
		sprintf(form->purpose, "PDI Abort");

		OrbMech::format_time_HHMMSS(GETbuffer, t_CSI);
		OrbMech::format_time_HHMMSS(GETbuffer2, t_TPI_Abort);
		sprintf(form->remarks, "CSI time: %s, TPI time: %s, N equal to 1", GETbuffer, GETbuffer2);
	}
	break;
	case 75: //PRELIMINARY CSM BACKUP INSERTION UPDATE
		preliminary = true;
	case 76: //CSM BACKUP INSERTION UPDATE
	{
		AP11ManPADOpt opt;
		LambertMan lamopt;
		SV sv_CSM, sv_LM, sv_Ins;
		VECTOR3 dV, dV_LVLH;
		double GETbase, P30TIG;

		AP11MNV * form = (AP11MNV *)pad;

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();

		lamopt.axis = RTCC_LAMBERT_MULTIAXIS;
		lamopt.GETbase = GETbase;
		lamopt.N = 0;
		lamopt.Offset = -_V(-110.0*1852.0, 0.0, 14.7*1852.0);
		lamopt.Perturbation = RTCC_LAMBERT_PERTURBED;
		lamopt.sv_A = sv_CSM;
		lamopt.sv_P = sv_LM;
		lamopt.T1 = calcParams.Insertion + 3.0*60.0;
		lamopt.T2 = calcParams.CSI;

		LambertTargeting(&lamopt, dV);
		PoweredFlightProcessor(sv_CSM, GETbase, lamopt.T1, RTCC_VESSELTYPE_CSM, RTCC_ENGINETYPE_SPSDPS, 0.0, dV, P30TIG, dV_LVLH);

		opt.alt = LSAlt;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "Backup Insertion");

		sv_Ins = ExecuteManeuver(calcParams.src, GETbase, P30TIG, dV_LVLH, sv_CSM, 0.0);

		SPQOpt coeopt;
		VECTOR3 dV_CSI;
		double t_TPI;
		char GETbuffer[64], GETbuffer2[64];

		coeopt.DH = -15.0*1852.0;
		coeopt.E = 208.3*RAD;
		coeopt.GETbase = GETbase;
		coeopt.maneuver = 0;
		coeopt.sv_A = sv_Ins;
		coeopt.sv_P = sv_LM;
		coeopt.type = 1;
		coeopt.t_TIG = calcParams.CSI;

		ConcentricRendezvousProcessor(&coeopt, dV_CSI, t_TPI);

		OrbMech::format_time_HHMMSS(GETbuffer, calcParams.CSI);
		OrbMech::format_time_HHMMSS(GETbuffer2, t_TPI);
		sprintf(form->remarks, "CSI: %s, TPI: %s, N equals 1", GETbuffer, GETbuffer2);

		if (preliminary == false)
		{
			sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv_CSM, true, AGCEpoch, GETbase));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector");
			}
		}
	}
	break;
	case 77: //PRELIMINARY LM INSERTION UPDATE
		preliminary = true;
	case 78: //LM INSERTION UPDATE
	{
		AP11LMManPADOpt opt;
		LambertMan lamopt;
		SV sv_CSM, sv_LM;
		VECTOR3 dV, dV_LVLH;
		double GETbase, P30TIG;

		AP11LMMNV * form = (AP11LMMNV *)pad;

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();

		//Without descent stage
		sv_LM.mass -= 2224.0 + 8000.0;

		lamopt.axis = RTCC_LAMBERT_MULTIAXIS;
		lamopt.GETbase = GETbase;
		lamopt.N = 0;
		lamopt.Offset = _V(-147.0*1852.0, 0.0, 14.7*1852.0);
		lamopt.Perturbation = RTCC_LAMBERT_PERTURBED;
		lamopt.sv_A = sv_LM;
		lamopt.sv_P = sv_CSM;
		lamopt.T1 = calcParams.Insertion;
		lamopt.T2 = calcParams.CSI;

		LambertTargeting(&lamopt, dV);
		PoweredFlightProcessor(sv_LM, GETbase, lamopt.T1, RTCC_VESSELTYPE_LM, RTCC_ENGINETYPE_APS, 0.0, dV, P30TIG, dV_LVLH);

		opt.alt = LSAlt;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_APS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.RV_MCC = sv_LM;
		opt.TIG = P30TIG;
		opt.useSV = true;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);
		sprintf(form->purpose, "Insertion");

		if (preliminary == false)
		{
			sprintf(form->remarks, "LM ascent stage weight is %.0lf", form->LMWeight);

			sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv_CSM, true, AGCEpoch, GETbase));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector");
			}
		}
	}
	break;
	case 79: //CSI UPDATE
	{
		AP10CSIPADOpt manopt;
		SPQOpt opt;
		SV sv_CSM, sv_LM, sv_CSI;
		MATRIX3 Q_Xx;
		VECTOR3 dV, dV_LVLH;
		double GETbase, t_TPI, dt_apo, mu;

		AP10CSI * form = (AP10CSI *)pad;

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();

		//CSI at apolune
		mu = GGRAV * oapiGetMass(sv_LM.gravref);
		dt_apo = OrbMech::timetoapo(sv_LM.R, sv_LM.V, mu);
		calcParams.CSI = OrbMech::GETfromMJD(sv_LM.MJD, GETbase) + dt_apo;

		opt.DH = 15.0*1852.0;
		opt.E = 26.6*RAD;
		opt.GETbase = GETbase;
		opt.maneuver = 0;
		opt.sv_A = sv_LM;
		opt.sv_P = sv_CSM;
		opt.type = 0;
		opt.t_TIG = calcParams.CSI;
		opt.t_TPI = calcParams.TPI;

		ConcentricRendezvousProcessor(&opt, dV, t_TPI);

		sv_CSI = coast(sv_LM, opt.t_TIG - OrbMech::GETfromMJD(sv_LM.MJD, GETbase));
		Q_Xx = OrbMech::LVLH_Matrix(sv_CSI.R, sv_CSI.V);
		dV_LVLH = mul(Q_Xx, dV);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_APS;
		manopt.GETbase = GETbase;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		manopt.sv0 = sv_LM;
		manopt.t_CSI = calcParams.CSI;
		manopt.t_TPI = calcParams.TPI;

		AP10CSIPAD(&manopt, *form);
	}
	break;
	case 80: //APS DEPLETION UPDATE
	{
		AP11LMManPADOpt opt;
		SV sv, sv1, sv2;
		MATRIX3 Q_Xx;
		VECTOR3 UX, UY, UZ, DV, DV_P, DV_C, V_G, dV_LVLH;
		double GETbase, MJD_depletion, t_Depletion_guess, t_Depletion, dv, theta_T;

		AP11LMMNV * form = (AP11LMMNV *)pad;

		GETbase = getGETBase();
		t_Depletion_guess = OrbMech::HHMMSSToSS(108, 0, 0);
		dv = 4600.0*0.3048;

		sv = StateVectorCalc(calcParams.tgt);
		sv1 = coast(sv, t_Depletion_guess - OrbMech::GETfromMJD(sv.MJD, GETbase));

		MJD_depletion = OrbMech::P29TimeOfLongitude(sv1.R, sv1.V, sv1.MJD, sv1.gravref, 0.0);
		t_Depletion = OrbMech::GETfromMJD(MJD_depletion, GETbase);
		sv2 = coast(sv1, t_Depletion - t_Depletion_guess);

		UY = unit(crossp(sv2.V, sv2.R));
		UZ = unit(-sv2.R);
		UX = crossp(UY, UZ);
		Q_Xx = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		DV = UX * dv;
		DV_P = UX * dv;

		theta_T = -length(crossp(sv2.R, sv2.V))*dv*sv2.mass / OrbMech::power(length(sv2.R), 2.0) / APS_THRUST;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C;
		dV_LVLH = mul(Q_Xx, V_G);

		opt.alt = LSAlt;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_APS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT= GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.TIG = t_Depletion;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);

		sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector");
		}
	}
	break;
	case 90: //MCC-5 UPDATE
	case 91: //PRELIMINARY MCC-6 UPDATE
	case 92: //MCC-6 UPDATE
	case 93: //MCC-7 DECISION
	case 94: //MCC-7 UPDATE
	{
		EntryOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase, MCCtime;
		MATRIX3 REFSMMAT;
		char manname[8];
		SV sv;

		AP11MNV * form = (AP11MNV *)pad;

		//Just so things don't break
		if (calcParams.TEI == 0)
		{
			calcParams.TEI = OrbMech::HHMMSSToSS(137, 20, 0);
		}
		if (calcParams.EI == 0)
		{
			calcParams.EI = OrbMech::HHMMSSToSS(191, 50, 0);
		}

		if (fcn == 90)
		{
			MCCtime = calcParams.TEI + 15.0*3600.0;
			sprintf(manname, "MCC-5");
		}
		else if (fcn == 91 || fcn == 92)
		{
			MCCtime = calcParams.EI - 15.0*3600.0;
			sprintf(manname, "MCC-6");
		}
		else if (fcn == 93 || fcn == 94)
		{
			MCCtime = calcParams.EI - 2.0*3600.0;
			sprintf(manname, "MCC-7");
		}

		//Only corridor control after EI-24h
		if (MCCtime > calcParams.EI - 24.0*3600.0)
		{
			entopt.type = RTCC_ENTRY_CORRIDOR;
		}
		else
		{
			entopt.type = RTCC_ENTRY_MCC;
		}

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.entrylongmanual = true;
		entopt.GETbase = GETbase;
		entopt.impulsive = RTCC_NONIMPULSIVE;
		entopt.lng = -165.0*RAD;
		entopt.ReA = 0;
		entopt.TIGguess = MCCtime;
		entopt.vessel = calcParams.src;

		EntryTargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, ReA, prec); //Target Load for uplink

		//Apollo 10 Mission Rules
		if (MCCtime > calcParams.EI - 50.0*3600.0)
		{
			if (length(res.dV_LVLH) < 1.0*0.3048)
			{
				scrubbed = true;
			}
		}
		else
		{
			if (length(res.dV_LVLH) < 2.0*0.3048)
			{
				scrubbed = true;
			}
		}

		if (fcn == 94)
		{
			REFSMMATOpt refsopt;
			refsopt.GETbase = GETbase;
			refsopt.dV_LVLH = res.dV_LVLH;
			refsopt.P30TIG = res.P30TIG;
			refsopt.REFSMMATdirect = false;
			refsopt.REFSMMATopt = 3;
			refsopt.vessel = calcParams.src;

			REFSMMAT = REFSMMATCalc(&refsopt);
		}
		else
		{
			REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		}

		if (scrubbed)
		{
			sprintf(upDesc, "%s has been scrubbed.", manname);

			//Entry prediction without maneuver
			EntryUpdateCalc(sv, entopt.GETbase, 0, true, &res);

			res.dV_LVLH = _V(0, 0, 0);
			res.P30TIG = entopt.TIGguess;
		}
		else
		{
			opt.dV_LVLH = res.dV_LVLH;
			opt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), res.dV_LVLH);
			opt.GETbase = GETbase;
			opt.HeadsUp = false;
			opt.REFSMMAT = REFSMMAT;
			opt.TIG = res.P30TIG;
			opt.vessel = calcParams.src;
			opt.vesseltype = 0;

			AP11ManeuverPAD(&opt, *form);
			sprintf(form->purpose, manname);
			form->lat = res.latitude*DEG;
			form->lng = res.longitude*DEG;
			form->RTGO = res.RTGO;
			form->VI0 = res.VIO / 0.3048;
			form->GET05G = res.GET05G;
		}

		if (scrubbed)
		{
			//Scrubbed MCC-5 and MCC-6
			if (fcn == 90 || fcn == 91 || fcn == 92)
			{
				sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), CMCEntryUpdate(res.latitude, res.longitude));
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM state vector, entry target");
				}
			}
			//Scrubbed MCC-7
			else if (fcn == 94)
			{
				sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), CMCEntryUpdate(res.latitude, res.longitude), CMCDesiredREFSMMATUpdate(REFSMMAT, AGCEpoch));
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM state vector, entry target, Entry REFSMMAT");
				}
			}
		}
		else
		{
			//MCC-5 and MCC-6
			if (fcn == 90 || fcn == 91 || fcn == 92)
			{
				sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), CMCRetrofireExternalDeltaVUpdate(res.latitude, res.longitude, res.P30TIG, res.dV_LVLH));
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM state vector, target load");
				}
			}
			//MCC-7
			else if (fcn == 94)
			{
				sprintf(uplinkdata, "%s%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase), CMCRetrofireExternalDeltaVUpdate(res.latitude, res.longitude, res.P30TIG, res.dV_LVLH), CMCDesiredREFSMMATUpdate(REFSMMAT, AGCEpoch));
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM state vector, target load, Entry REFSMMAT");
				}
			}
		}

		//Save for further use
		calcParams.EI = res.GET400K;
		DeltaV_LVLH = res.dV_LVLH;
		TimeofIgnition = res.P30TIG;
		SplashLatitude = res.latitude;
		SplashLongitude = res.longitude;
	}
	break;
	case 96: //ENTRY PAD (ASSUMES NO MCC-6, but MCC-7)
	case 97: //ENTRY PAD (ASSUMES MCC-6)
	case 98: //ENTRY PAD (ASSUMES MCC-7)
	case 99: //FINAL LUNAR ENTRY PAD
	{
		AP11ENT * form = (AP11ENT *)pad;

		SV sv;
		LunarEntryPADOpt entopt;
		MATRIX3 REFSMMAT;
		double GETbase;

		GETbase = getGETBase();
		sv = StateVectorCalc(calcParams.src);

		if (length(DeltaV_LVLH) != 0.0 && fcn != 99)
		{
			entopt.direct = false;
		}
		else
		{
			entopt.direct = true;
		}

		if (fcn == 99)
		{
			REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		}
		else
		{
			REFSMMATOpt refsopt;
			refsopt.GETbase = GETbase;
			refsopt.dV_LVLH = DeltaV_LVLH;
			refsopt.P30TIG = TimeofIgnition;
			refsopt.REFSMMATdirect = false;
			refsopt.REFSMMATopt = 3;
			refsopt.vessel = calcParams.src;

			REFSMMAT = REFSMMATCalc(&refsopt);
		}

		entopt.dV_LVLH = DeltaV_LVLH;
		entopt.GETbase = GETbase;
		entopt.lat = SplashLatitude;
		entopt.lng = SplashLongitude;
		entopt.P30TIG = TimeofIgnition;
		entopt.REFSMMAT = REFSMMAT;
		entopt.vessel = calcParams.src;

		LunarEntryPAD(&entopt, *form);
		sprintf(form->Area[0], "MIDPAC");
		if (entopt.direct == false)
		{
			if (fcn == 97)
			{
				sprintf(form->remarks[0], "Assumes MCC6");
			}
			else if (fcn == 98)
			{
				sprintf(form->remarks[0], "Assumes MCC7");
			}
		}

		if (fcn == 99)
		{
			sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase, true), CMCEntryUpdate(SplashLatitude, SplashLongitude));
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load, Entry REFSMMAT");
			}
		}
	}
	break;
	case 100: //GENERIC CSM STATE VECTOR UPDATE
	{
		SV sv;
		double GETbase;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();

		sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv, true, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 101: //GENERIC CSM AND LM STATE VECTOR UPDATE
	{
		SV sv_CSM, sv_LM;
		double GETbase;

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();

		sprintf(uplinkdata, "%s%s", AGCStateVectorUpdate(sv_CSM, true, AGCEpoch, GETbase), AGCStateVectorUpdate(sv_LM, false, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM and LM state vectors");
		}
	}
	break;
	case 102: //GENERIC LM STATE VECTOR UPDATE
	{
		SV sv;
		double GETbase;

		sv = StateVectorCalc(calcParams.tgt); //State vector for uplink
		GETbase = getGETBase();

		sprintf(uplinkdata, "%s", AGCStateVectorUpdate(sv, false, AGCEpoch, GETbase));
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector");
		}
	}
	break;
	}

	return scrubbed;
}

void RTCC::AP7BlockData(AP7BLKOpt *opt, AP7BLK &pad)
{
	EarthEntryOpt entopt;
	EntryResults res;
	double v_e, m1, Vc;

	char weather[10] = "GOOD";

	v_e = calcParams.src->GetThrusterIsp0(calcParams.src->GetGroupThruster(THGROUP_MAIN, 0));

	entopt.vessel = calcParams.src;
	entopt.GETbase = getGETBase();
	entopt.impulsive = RTCC_NONIMPULSIVE;
	entopt.nominal = RTCC_ENTRY_NOMINAL;
	entopt.ReA = 0;
	entopt.entrylongmanual = true;

	for (int i = 0;i < opt->n;i++)
	{
		entopt.lng = opt->lng[i];
		entopt.TIGguess = opt->GETI[i];

		BlockDataProcessor(&entopt, &res);

		m1 = calcParams.src->GetMass()*exp(-length(res.dV_LVLH) / v_e);
		Vc = length(res.dV_LVLH)*cos(-2.15*RAD)*cos(0.95*RAD);// -60832.18 / m1;

		sprintf(pad.Area[i], opt->area[i]);
		sprintf(pad.Wx[i], weather);

		pad.dVC[i] = Vc/0.3048;
		pad.GETI[i] = res.P30TIG;
		pad.Lat[i] = res.latitude*DEG;
		pad.Lng[i] = res.longitude*DEG;
	}
}

void RTCC::AP11BlockData(AP11BLKOpt *opt, P37PAD &pad)
{
	EntryOpt entopt;
	EntryResults res;

	entopt.entrylongmanual = true;
	entopt.GETbase = getGETBase();
	entopt.impulsive = RTCC_NONIMPULSIVE;
	entopt.ReA = 0;
	entopt.type = RTCC_ENTRY_ABORT;
	entopt.vessel = calcParams.src;

	if (opt->useSV)
	{
		entopt.useSV = true;
		entopt.RV_MCC = opt->RV_MCC;
	}

	for (int i = 0;i < opt->n;i++)
	{
		entopt.TIGguess = opt->GETI[i];
		entopt.lng = opt->lng[i];

		EntryTargeting(&entopt, &res);

		pad.dVT[i] = length(res.dV_LVLH) / 0.3048;
		pad.GET400K[i] = res.GET05G;
		pad.GETI[i] = opt->GETI[i];
		pad.lng[i] = opt->lng[i] * DEG;
	}
}

void RTCC::BlockDataProcessor(EarthEntryOpt *opt, EntryResults *res)
{
	EarthEntry* entry;
	double GET;
	bool stop;
	SV sv;

	stop = false;

	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}
	GET = (sv.MJD - opt->GETbase)*24.0*3600.0;

	entry = new EarthEntry(sv.R, sv.V, sv.MJD, sv.gravref, opt->GETbase, opt->TIGguess, opt->ReA, opt->lng, opt->nominal, opt->entrylongmanual);

	while (!stop)
	{
		stop = entry->EntryIter();
	}

	res->dV_LVLH = entry->Entry_DV;
	res->P30TIG = entry->EntryTIGcor;
	res->latitude = entry->EntryLatcor;
	res->longitude = entry->EntryLngcor;
	res->GET400K = entry->t2;
	res->GET05G = entry->EntryRET;
	res->RTGO = entry->EntryRTGO;
	res->VIO = entry->EntryVIO;
	res->ReA = entry->EntryAng;
	res->precision = entry->precision;

	delete entry;

	if (opt->impulsive > 0 )
	{
		bool main;
		double t_slip;
		VECTOR3 UX, UY, UZ, DV, Llambda, RA1_cor, VA1_cor;
		MATRIX3 Q_Xx;
		SV sv1;

		if (opt->impulsive == RTCC_NONIMPULSIVE)
		{
			main = true;
		}
		else
		{
			main = false;
		}

		sv1 = coast(sv, res->P30TIG - GET);

		UY = unit(crossp(sv1.V, sv1.R));
		UZ = unit(-sv1.R);
		UX = crossp(UY, UZ);

		DV = UX*res->dV_LVLH.x + UY*res->dV_LVLH.y + UZ*res->dV_LVLH.z;

		FiniteBurntimeCompensation(0, sv1, 0.0, DV, main, Llambda, t_slip);

		OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD + (res->P30TIG - GET) / 24.0 / 3600.0, t_slip, RA1_cor, VA1_cor, sv1.gravref, sv1.gravref);

		Q_Xx = OrbMech::LVLH_Matrix(RA1_cor, VA1_cor);

		res->dV_LVLH = mul(Q_Xx, Llambda);
		res->P30TIG += t_slip;
	}
}

void RTCC::EntryTargeting(EntryOpt *opt, EntryResults *res)
{
	Entry* entry;
	double GET;
	bool stop;
	SV sv;

	stop = false;

	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}
	GET = (sv.MJD - opt->GETbase)*24.0*3600.0;

	entry = new Entry(sv.R, sv.V, sv.MJD, sv.gravref, opt->GETbase, opt->TIGguess, opt->ReA, opt->lng, opt->type, opt->entrylongmanual);

	while (!stop)
	{
		stop = entry->EntryIter();
	}

	res->dV_LVLH = entry->Entry_DV;
	res->P30TIG = entry->EntryTIGcor;
	res->latitude = entry->EntryLatcor;
	res->longitude = entry->EntryLngcor;
	res->GET400K = entry->t2;
	res->GET05G = entry->EntryRET;
	res->RTGO = entry->EntryRTGO;
	res->VIO = entry->EntryVIO;
	res->ReA = entry->EntryAng;
	res->precision = entry->precision;

	delete entry;

	if (opt->impulsive == RTCC_NONIMPULSIVE)
	{
		double t_slip, LMmass;
		VECTOR3 UX, UY, UZ, DV, Llambda, RA1_cor, VA1_cor;
		MATRIX3 Q_Xx;
		SV sv1;

		sv1 = coast(sv, res->P30TIG - GET);

		UY = unit(crossp(sv1.V, sv1.R));
		UZ = unit(-sv1.R);
		UX = crossp(UY, UZ);

		DV = UX*res->dV_LVLH.x + UY*res->dV_LVLH.y + UZ*res->dV_LVLH.z;

		if (opt->csmlmdocked)
		{
			LMmass = GetDockedVesselMass(opt->vessel);
		}
		else
		{
			LMmass = 0.0;
		}

		FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DV, true, Llambda, t_slip);

		OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD + (res->P30TIG - GET) / 24.0 / 3600.0, t_slip, RA1_cor, VA1_cor, sv1.gravref, sv1.gravref);

		Q_Xx = OrbMech::LVLH_Matrix(RA1_cor, VA1_cor);

		res->dV_LVLH = mul(Q_Xx, Llambda);
		res->P30TIG += t_slip;
	}
}

void RTCC::LambertTargeting(LambertMan *lambert, VECTOR3 &dV)
{
	VECTOR3 dV_LVLH;

	LambertTargeting(lambert, dV, dV_LVLH);
}

void RTCC::LambertTargeting(LambertMan *lambert, VECTOR3 &dV, VECTOR3 &dV_LVLH)
{
	SV sv_A1, sv_P2;
	double dt1, dt1_apo, dt2, mu;
	OBJHANDLE gravref;
	bool prograde;

	gravref = lambert->sv_A.gravref;

	if (gravref == oapiGetObjectByName("Earth"))	//Hardcoded: Always prograde for Earth, always retrograde for Moon
	{
		prograde = true;
	}
	else
	{
		prograde = false;
	}

	mu = GGRAV*oapiGetMass(gravref);

	dt1 = lambert->T1 - (lambert->sv_A.MJD - lambert->GETbase) * 24.0 * 60.0 * 60.0;
	dt1_apo = lambert->T1 - (lambert->sv_P.MJD - lambert->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = lambert->T2 - lambert->T1;

	if (lambert->Perturbation == 1)
	{
		sv_A1 = coast(lambert->sv_A, dt1);
		sv_P2 = coast(lambert->sv_P, dt1_apo + dt2);
	}
	else
	{
		OrbMech::rv_from_r0v0(lambert->sv_A.R, lambert->sv_A.V, dt1, sv_A1.R, sv_A1.V, mu);
		OrbMech::rv_from_r0v0(lambert->sv_P.R, lambert->sv_P.V, dt1_apo + dt2, sv_P2.R, sv_P2.V, mu);
	}

	VECTOR3 RP2off, VP2off;
	double angle;

	if (lambert->PhaseAngle != 0.0)
	{
		angle = lambert->PhaseAngle;
	}
	else
	{
		angle = lambert->Offset.x / length(sv_P2.R);
	}

	OrbMech::rv_from_r0v0_ta(sv_P2.R, sv_P2.V, angle, RP2off, VP2off, mu);

	VECTOR3 i, j, k, VA1_apo;
	MATRIX3 Q_Xx2, Q_Xx;

	k = -unit(RP2off);
	j = unit(crossp(VP2off, RP2off));
	i = crossp(j, k);
	Q_Xx2 = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

	RP2off = RP2off + tmul(Q_Xx2, _V(0.0, lambert->Offset.y, lambert->Offset.z));

	if (lambert->Perturbation == RTCC_LAMBERT_PERTURBED)
	{
		VA1_apo = OrbMech::Vinti(sv_A1.R, sv_A1.V, RP2off, sv_A1.MJD, dt2, lambert->N, prograde, gravref, gravref, gravref, _V(0.0, 0.0, 0.0)); //Vinti Targeting: For non-spherical gravity
	}
	else
	{
		if (lambert->axis == RTCC_LAMBERT_MULTIAXIS)
		{
			VA1_apo = OrbMech::elegant_lambert(sv_A1.R, sv_A1.V, RP2off, dt2, lambert->N, prograde, mu);	//Lambert Targeting
		}
		else
		{
			OrbMech::xaxislambert(sv_A1.R, sv_A1.V, RP2off, dt2, lambert->N, prograde, mu, VA1_apo, lambert->Offset.z);	//Lambert Targeting
		}
	}

	Q_Xx = OrbMech::LVLH_Matrix(sv_A1.R, sv_A1.V);
	dV = VA1_apo - sv_A1.V;
	dV_LVLH = mul(Q_Xx, dV);

	if (lambert->axis == RTCC_LAMBERT_XAXIS)
	{
		dV_LVLH.y = 0.0;
	}
}

void RTCC::LMThrottleProgram(double F, double v_e, double mass, double dV_LVLH, double &F_average, double &ManPADBurnTime, double &bt_var, int &step)
{
	double bt, dv_10p, mass40p, bt_40p, dv_40p, mass100p, bt_100p;

	bt = v_e / (0.1*F) *(mass)*(1.0 - exp(-dV_LVLH / v_e)); //burn time at 10% thrust

	if (bt > 5.0 + 16.0)	//Burn longer than 5+16 seconds at 10%
	{

		dv_10p = v_e*log((mass) / (mass - 0.1*F / v_e*5.0)); //Five seconds at 10%
		mass40p = mass - 0.1*F / v_e*5.0;
		bt_40p = v_e / (0.4*F) *(mass40p)*(1.0 - exp(-(dV_LVLH - dv_10p) / v_e)); // rest burn time at 40% thrust

		if (bt_40p > 21.0 + 10.0)	//Burn at 40% longer than 21+10 seconds
		{
			dv_40p = v_e*log((mass40p) / (mass40p - 0.4*F / v_e*21.0)); //21 seconds at 40%
			mass100p = mass40p - 0.4*F / v_e*21.0;
			bt_100p = v_e / F *(mass100p)*(1.0 - exp(-(dV_LVLH - dv_10p - dv_40p) / v_e)); // rest burn time at 100% thrust

			F_average = (0.1 * 5.0 + 0.4*21.0 + bt_100p) / (21.0 + 5.0 + bt_100p)*F;
			ManPADBurnTime = bt_100p + 26.0;
			bt_var = bt_100p;

			step = 2;
		}
		else
		{
			F_average = (0.1 * 5.0 + 0.4*bt_40p) / (bt_40p + 5.0)*F;
			ManPADBurnTime = bt_40p + 5.0;
			bt_var = bt_40p;

			step = 1;
		}
	}
	else
	{
		F_average = 0.1*F;	//Full burn at 10%
		ManPADBurnTime = bt;
		bt_var = bt;

		step = 0;
	}
}

void RTCC::AP10CSIPAD(AP10CSIPADOpt *opt, AP10CSI &pad)
{
	SV sv1, sv2;
	MATRIX3 Q_Xx, M, M_R;
	VECTOR3 V_G, UX, UY, UZ, IMUangles, FDAIangles;
	double dt, v_e, F, F_average, ManPADBurnTime, bt;
	int step;

	dt = opt->t_CSI - (opt->sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	sv1 = coast(opt->sv0, dt);

	//Engine parameters
	if (opt->enginetype == RTCC_ENGINETYPE_RCS)
	{
		v_e = 2706.64;
		F = 400 * 4.448222;
	}
	else if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		v_e = DPS_ISP;
		F = DPS_THRUST;
	}
	else
	{
		v_e = APS_ISP;
		F = APS_THRUST;
	}

	if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		//Estimates for average thrust (relevant for finite burntime compensation), complete and variable burntime
		LMThrottleProgram(F, v_e, sv1.mass, length(opt->dV_LVLH), F_average, ManPADBurnTime, bt, step);
	}
	else
	{
		F_average = F;
	}

	sv2 = ExecuteManeuver(NULL, opt->GETbase, opt->t_CSI, opt->dV_LVLH, sv1, 0.0, Q_Xx, V_G, F_average, v_e);

	UX = unit(V_G);

	if (abs(dotp(UX, unit(sv1.R))) < cos(0.01*RAD))
	{
		UY = unit(crossp(UX, sv1.R));
	}
	else
	{
		UY = unit(crossp(UX, sv1.V));
	}
	UZ = unit(crossp(UX, UY));

	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::transpose_matrix(M), M_R));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.PLM_FDAI = FDAIangles.y*DEG;
	pad.t_CSI = opt->t_CSI;
	pad.t_TPI = opt->t_TPI;
	pad.dV_LVLH = opt->dV_LVLH / 0.3048;
	pad.dV_AGS = mul(Q_Xx, V_G) / 0.3048;
}

void RTCC::AP11LMManeuverPAD(AP11LMManPADOpt *opt, AP11LMMNV &pad)
{
	MATRIX3 M_R, M, Q_Xx;
	VECTOR3 V_G, X_B, UX, UY, UZ, IMUangles, FDAIangles;
	double dt, mu, CSMmass, F, v_e, F_average, ManPADBurnTime, bt, apo, peri, ManPADApo, ManPADPeri, ManPADDVR, ManBSSpitch, ManBSSXPos, R_E, headsswitch;
	int step, ManCOASstaroct;
	SV sv, sv1, sv2;

	//State vector from PAD options or get a new one
	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}

	//Coast until TIG
	dt = opt->TIG - (sv.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	sv1 = coast(sv, dt);

	//Docked mass
	if (opt->csmlmdocked == true)
	{
		CSMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		CSMmass = 0.0;
	}

	//Engine parameters
	if (opt->enginetype == RTCC_ENGINETYPE_RCS)
	{
		v_e = 2706.64;
		F = 400 * 4.448222;
	}
	else if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		v_e = DPS_ISP;
		F = DPS_THRUST;
	}
	else
	{
		v_e = APS_ISP;
		F = APS_THRUST;
	}

	if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		//Estimates for average thrust (relevant for finite burntime compensation), complete and variable burntime
		LMThrottleProgram(F, v_e, CSMmass + sv1.mass, length(opt->dV_LVLH), F_average, ManPADBurnTime, bt, step);
	}
	else
	{
		F_average = F;
		ManPADBurnTime = v_e / F * (sv1.mass + CSMmass)*(1.0 - exp(-length(opt->dV_LVLH) / v_e));
	}

	//Execute maneuver, output state vector at cutoff
	sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->TIG, opt->dV_LVLH, sv1, CSMmass, Q_Xx, V_G, F_average, v_e);

	mu = GGRAV*oapiGetMass(sv1.gravref);
	//Only use landing site radius for the Moon
	if (sv1.gravref == oapiGetObjectByName("Moon"))
	{
		R_E = oapiGetSize(sv1.gravref) + opt->alt;
	}
	else
	{
		R_E = oapiGetSize(sv1.gravref);
	}

	OrbMech::periapo(sv2.R, sv2.V, mu, apo, peri);
	ManPADApo = apo - R_E;
	ManPADPeri = peri - R_E;

	if (opt->HeadsUp)
	{
		headsswitch = -1.0;
	}
	else
	{
		headsswitch = 1.0;
	}

	X_B = unit(V_G);
	if (opt->enginetype == RTCC_ENGINETYPE_RCS && opt->directiontype == RTCC_DIRECTIONTYPE_MINUSX)
	{
		UX = -X_B;
	}
	else
	{
		UX = X_B;
	}
	if (abs(dotp(UX, unit(sv1.R))) < cos(0.01*RAD))
	{
		UY = unit(crossp(UX, sv1.R*headsswitch));
	}
	else
	{
		UY = unit(crossp(UX, sv1.V));
	}
	UZ = unit(crossp(UX, UY));


	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);


	ManPADDVR = length(opt->dV_LVLH);

	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::transpose_matrix(M), M_R));
	pad.IMUAtt = IMUangles;

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	VECTOR3 Rsxt, Vsxt;

	OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD, opt->sxtstardtime*60.0, Rsxt, Vsxt, sv1.gravref, sv1.gravref);

	OrbMech::coascheckstar(opt->REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD), Rsxt, oapiGetSize(sv1.gravref), ManCOASstaroct, ManBSSpitch, ManBSSXPos);
	
	pad.Att = _V(OrbMech::imulimit(FDAIangles.x*DEG), OrbMech::imulimit(FDAIangles.y*DEG), OrbMech::imulimit(FDAIangles.z*DEG));
	pad.BSSStar = ManCOASstaroct;
	pad.burntime = ManPADBurnTime;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.dV_AGS = mul(Q_Xx, V_G) / 0.3048;
	pad.GETI = opt->TIG;
	pad.HA = min(9999.9, ManPADApo / 1852.0);
	pad.HP = ManPADPeri / 1852.0;

	pad.SPA = ManBSSpitch*DEG;
	pad.SXP = ManBSSXPos*DEG;
	pad.dVR = ManPADDVR / 0.3048;
	pad.CSMWeight = CSMmass / 0.45359237;
	pad.LMWeight = sv1.mass / 0.45359237;
}

void RTCC::AP11ManeuverPAD(AP11ManPADOpt *opt, AP11MNV &pad)
{
	MATRIX3 Q_Xx, M_R, M, M_RTM;
	VECTOR3 V_G, X_B, UX, UY, UZ, IMUangles, GDCangles;
	double dt, LMmass, F, v_e, headsswitch, mu, apo, peri, ManPADApo, ManPADPeri, ManPADPTrim, ManPADYTrim, p_T, y_T, ManPADDVC, ManPADBurnTime;
	double Mantrunnion, Manshaft, ManBSSpitch, ManBSSXPos, R_E;
	int GDCset, Manstaroct, ManCOASstaroct;
	SV sv, sv1, sv2;

	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}

	dt = opt->TIG - (sv.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;

	sv1 = coast(sv, dt);

	if (opt->vesseltype == 1)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		v_e = SPS_ISP;
		F = SPS_THRUST;
	}
	else
	{
		v_e = 2706.64;
		F = 400 * 4.448222;
	}

	//Execute maneuver, output state vector at cutoff
	sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->TIG, opt->dV_LVLH, sv1, LMmass, Q_Xx, V_G, F, v_e);

	mu = GGRAV*oapiGetMass(sv1.gravref);

	//Only use landing site radius for the Moon
	if (sv1.gravref == oapiGetObjectByName("Moon"))
	{
		R_E = oapiGetSize(sv1.gravref) + opt->alt;
	}
	else
	{
		R_E = oapiGetSize(sv1.gravref);
	}

	if (opt->HeadsUp)
	{
		headsswitch = 1.0;
	}
	else
	{
		headsswitch = -1.0;
	}

	OrbMech::periapo(sv2.R, sv2.V, mu, apo, peri);
	ManPADApo = apo - R_E;
	ManPADPeri = peri - R_E;

	if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		CalcSPSGimbalTrimAngles(sv1.mass, LMmass, p_T, y_T);

		ManPADPTrim = p_T + 2.15*RAD;
		ManPADYTrim = y_T - 0.95*RAD;

		X_B = unit(V_G);
		UX = X_B;
		UY = unit(crossp(X_B, sv1.R*headsswitch));
		UZ = unit(crossp(X_B, crossp(X_B, sv1.R*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
		M_RTM = mul(OrbMech::transpose_matrix(M_R), M);

		ManPADDVC = length(opt->dV_LVLH)*cos(p_T)*cos(y_T);// -60832.18 / m1;
	}
	else
	{
		ManPADPTrim = 0.0;
		ManPADYTrim = 0.0;

		X_B = unit(V_G);
		if (opt->directiontype == RTCC_DIRECTIONTYPE_PLUSX)
		{
			UX = X_B;
			ManPADDVC = length(opt->dV_LVLH);
		}
		else
		{
			UX = -X_B;
			ManPADDVC = -length(opt->dV_LVLH);
		}
		UY = unit(crossp(UX, sv1.R*headsswitch));
		UZ = unit(crossp(UX, crossp(UX, sv1.R*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	}

	ManPADBurnTime = v_e / F *(sv1.mass + LMmass)*(1.0 - exp(-length(opt->dV_LVLH) / v_e));

	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::transpose_matrix(M), M_R));
	//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);

	GDCangles = OrbMech::backupgdcalignment(opt->REFSMMAT, sv1.R, oapiGetSize(sv1.gravref), GDCset);

	VECTOR3 Rsxt, Vsxt;

	OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD, opt->sxtstardtime*60.0, Rsxt, Vsxt, sv1.gravref, sv1.gravref);

	OrbMech::checkstar(opt->REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD), Rsxt, oapiGetSize(sv1.gravref), Manstaroct, Mantrunnion, Manshaft);

	OrbMech::coascheckstar(opt->REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD), Rsxt, oapiGetSize(sv1.gravref), ManCOASstaroct, ManBSSpitch, ManBSSXPos);

	pad.Att = _V(OrbMech::imulimit(IMUangles.x*DEG), OrbMech::imulimit(IMUangles.y*DEG), OrbMech::imulimit(IMUangles.z*DEG));
	pad.BSSStar = ManCOASstaroct;
	pad.burntime = ManPADBurnTime;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.GDCangles = _V(OrbMech::imulimit(GDCangles.x*DEG), OrbMech::imulimit(GDCangles.y*DEG), OrbMech::imulimit(GDCangles.z*DEG));
	pad.GETI = opt->TIG;
	pad.HA = min(9999.9, ManPADApo / 1852.0);
	pad.HP = ManPADPeri / 1852.0;

	if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		sprintf(pad.PropGuid, "SPS/G&N");
	}
	else
	{
		sprintf(pad.PropGuid, "RCS/G&N");
	}

	pad.pTrim = ManPADPTrim*DEG;
	pad.yTrim = ManPADYTrim*DEG;

	if (length(GDCangles) == 0.0)
	{
		sprintf(pad.SetStars, "N/A");
	}
	else
	{
		if (GDCset == 0)
		{
			sprintf(pad.SetStars, "Vega, Deneb");
		}
		else if (GDCset == 1)
		{
			sprintf(pad.SetStars, "Navi, Polaris");
		}
		else
		{
			sprintf(pad.SetStars, "Acrux, Atria");
		}
	}
	pad.Shaft = Manshaft*DEG;
	pad.SPA = ManBSSpitch*DEG;
	pad.Star = Manstaroct;
	pad.SXP = ManBSSXPos*DEG;
	pad.Trun = Mantrunnion*DEG;
	pad.Vc = ManPADDVC / 0.3048;
	pad.Vt = length(opt->dV_LVLH) / 0.3048;
	pad.Weight = sv1.mass / 0.45359237;
	pad.LMWeight = LMmass / 0.45359237;

	pad.GET05G = 0;
	pad.lat = 0;
	pad.lng = 0;
	pad.RTGO = 0;
}

void RTCC::AP7ManeuverPAD(AP7ManPADOpt *opt, AP7MNV &pad)
{
	MATRIX3 Q_Xx, M, M_R, M_RTM;
	VECTOR3 V_G, X_B, UX, UY, UZ, Att;
	double dt, LMmass, v_e, F, mu, headsswitch, apo, peri, ManPADApo, ManPADPeri, ManPADPTrim, ManPADYTrim, y_T, p_T;
	SV sv, sv1, sv2;

	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}

	dt = opt->TIG - (sv.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	sv1 = coast(sv, dt);

	if (opt->vesseltype == 1)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		v_e = SPS_ISP;
		F = SPS_THRUST;
	}
	else
	{
		v_e = 2706.64;
		F = 400 * 4.448222;
	}

	//Execute maneuver, output state vector at cutoff
	sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->TIG, opt->dV_LVLH, sv1, LMmass, Q_Xx, V_G, F, v_e);

	mu = GGRAV*oapiGetMass(sv1.gravref);

	if (opt->HeadsUp)
	{
		headsswitch = 1.0;
	}
	else
	{
		headsswitch = -1.0;
	}

	OrbMech::periapo(sv2.R, sv2.V, mu, apo, peri);
	ManPADApo = apo - oapiGetSize(sv2.gravref);
	ManPADPeri = peri - oapiGetSize(sv2.gravref);

	pad.Weight = sv1.mass / 0.45359237;

	pad.burntime = v_e / F *(sv1.mass + LMmass)*(1.0 - exp(-length(opt->dV_LVLH) / v_e));

	if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		CalcSPSGimbalTrimAngles(sv1.mass, LMmass, p_T, y_T);

		ManPADPTrim = p_T + 2.15*RAD;
		ManPADYTrim = y_T - 0.95*RAD;

		X_B = unit(V_G);
		UX = X_B;
		UY = unit(crossp(X_B, sv1.R*headsswitch));
		UZ = unit(crossp(X_B, UY));

		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
		M_RTM = mul(OrbMech::transpose_matrix(M_R), M);

		pad.Vc = length(opt->dV_LVLH)*cos(p_T)*cos(y_T);// -60832.18 / m1;
	}
	else
	{
		ManPADPTrim = 0.0;
		ManPADYTrim = 0.0;

		X_B = unit(V_G);
		if (opt->directiontype == RTCC_DIRECTIONTYPE_PLUSX)
		{
			UX = X_B;
			pad.Vc = length(opt->dV_LVLH);
		}
		else
		{
			UX = -X_B;
			pad.Vc = -length(opt->dV_LVLH);
		}
		UY = unit(crossp(UX, sv1.R*headsswitch));
		UZ = unit(crossp(UX, crossp(UX, sv1.R*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	}
	Att = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::transpose_matrix(M), M_R));
	//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);

	//GDCangles = OrbMech::backupgdcalignment(REFSMMAT, R1B, oapiGetSize(gravref), GDCset);

	VECTOR3 Rsxt, Vsxt;

	OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD, opt->sxtstardtime, Rsxt, Vsxt, sv1.gravref, sv1.gravref);

	OrbMech::checkstar(opt->REFSMMAT, _V(round(Att.x*DEG)*RAD, round(Att.y*DEG)*RAD, round(Att.z*DEG)*RAD), Rsxt, oapiGetSize(sv1.gravref), pad.Star, pad.Trun, pad.Shaft);

	if (opt->navcheckGET != 0.0)
	{
		VECTOR3 Rnav, Vnav;
		double alt, lat, lng;
		OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD, opt->navcheckGET - opt->TIG, Rnav, Vnav, sv1.gravref, sv1.gravref);
		navcheck(Rnav, Vnav, opt->GETbase + opt->navcheckGET / 24.0 / 3600.0, sv1.gravref, lat, lng, alt);

		pad.NavChk = opt->navcheckGET;
		pad.lat = lat*DEG;
		pad.lng = lng*DEG;
		pad.alt = alt / 1852;
	}

	pad.Att = _V(OrbMech::imulimit(Att.x*DEG), OrbMech::imulimit(Att.y*DEG), OrbMech::imulimit(Att.z*DEG));

	pad.GETI = opt->TIG;
	pad.pTrim = ManPADPTrim*DEG;
	pad.yTrim = ManPADYTrim*DEG;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.Vc /= 0.3048;
	pad.Shaft *= DEG;
	pad.Trun *= DEG;
	pad.HA = min(9999.9, ManPADApo / 1852.0);
	pad.HP = ManPADPeri / 1852.0;
}

void RTCC::AP7TPIPAD(AP7TPIPADOpt *opt, AP7TPI &pad)
{
	double mu, dt, SVMJD;
	VECTOR3 R_A, V_A, R0B, V0B, RA3, VA3, R_P, V_P, RP0B, VP0B, RP3, VP3, u, U_L, UX, UY, UZ, U_R, U_R2, RA2, VA2, RP2, VP2, U_P, TPIPAD_BT, TPIPAD_dV_LOS;
	MATRIX3 Rot1, Rot2;
	double TPIPAD_AZ, TPIPAD_R, TPIPAD_Rdot, TPIPAD_ELmin5,TPIPAD_dH, TPIPAD_ddH;
	VECTOR3 U_F, LOS, U_LOS, NN;

	OBJHANDLE gravref;

	gravref = AGCGravityRef(opt->vessel);

	mu = GGRAV*oapiGetMass(gravref);

	opt->vessel->GetRelativePos(gravref, R_A);
	opt->vessel->GetRelativeVel(gravref, V_A);
	opt->target->GetRelativePos(gravref, R_P);
	opt->target->GetRelativeVel(gravref, V_P);
	SVMJD = oapiGetSimMJD();

	dt = opt->TIG - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;

	R0B = _V(R_A.x, R_A.z, R_A.y);
	V0B = _V(V_A.x, V_A.z, V_A.y);
	RP0B = _V(R_P.x, R_P.z, R_P.y);
	VP0B = _V(V_P.x, V_P.z, V_P.y);

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

	TPIPAD_dV_LOS = mul(Rot2, mul(Rot1, opt->dV_LVLH));
	//TPIPAD_dH = abs(length(RP3) - length(RA3));
	double mass, F;

	mass = opt->vessel->GetMass();
	F = 200.0 * 4.448222;
	TPIPAD_BT = _V(abs(0.5*TPIPAD_dV_LOS.x), abs(TPIPAD_dV_LOS.y), abs(TPIPAD_dV_LOS.z))*mass / F;

	VECTOR3 i, j, k, dr, dv, dr0, dv0, Omega;
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
	t2 = 1.0 / n*(atan(-dv0.x / (3.0*n*dr0.x + 2.0 * dv0.y)) + PI);
	dxmax = 4.0 * dr0.x + 2.0 / n*dv0.y + dv0.x / n*sin(n*t1) - (3.0 * dr0.x + 2.0 / n*dv0.y)*cos(n*t1);
	dxmin = 4.0 * dr0.x + 2.0 / n*dv0.y + dv0.x / n*sin(n*t2) - (3.0 * dr0.x + 2.0 / n*dv0.y)*cos(n*t2);

	OrbMech::oneclickcoast(RA3, VA3, SVMJD + dt / 24.0 / 3600.0, -5.0*60.0, RA2, VA2, gravref, gravref);
	OrbMech::oneclickcoast(RP3, VP3, SVMJD + dt / 24.0 / 3600.0, -5.0*60.0, RP2, VP2, gravref, gravref);

	U_R2 = unit(RP2 - RA2);

	TPIPAD_dH = -dr0.x;
	TPIPAD_ddH = abs(dxmax - dxmin);
	TPIPAD_R = abs(length(RP2 - RA2));
	TPIPAD_Rdot = dotp(VP2 - VA2, U_R2);

	U_L = unit(RP2 - RA2);
	U_P = unit(U_L - RA2*dotp(U_L, RA2) / length(RA2) / length(RA2));

	TPIPAD_ELmin5 = acos(dotp(U_L, U_P*OrbMech::sign(dotp(U_P, crossp(u, RA2)))));

	U_F = unit(crossp(crossp(RA2, VA2), RA2));
	U_R = unit(RA2);
	LOS = RP2 - RA2;
	U_LOS = unit(LOS - U_R*dotp(LOS, U_R));
	TPIPAD_AZ = acos(dotp(U_LOS, U_F));//atan2(-TPIPAD_dV_LOS.z, TPIPAD_dV_LOS.x);
	NN = crossp(U_LOS, U_F);
	if (dotp(NN, RA2) < 0)
	{
		TPIPAD_AZ = PI2 - TPIPAD_AZ;
	}

	pad.AZ = TPIPAD_AZ*DEG;
	pad.Backup_bT = TPIPAD_BT;
	pad.Backup_dV = TPIPAD_dV_LOS / 0.3048;
	pad.R = TPIPAD_R / 1852.0;
	pad.Rdot = TPIPAD_Rdot / 0.3048;
	pad.EL = TPIPAD_ELmin5*DEG;
	pad.GETI = opt->TIG;
	pad.Vg = opt->dV_LVLH / 0.3048;
	pad.dH_TPI = TPIPAD_dH / 1852.0;
	pad.dH_Max = TPIPAD_ddH / 1852.0;
}

void RTCC::CSMDAPUpdate(VESSEL *v, AP10DAPDATA &pad)
{
	double CSMmass, LMmass, p_T, y_T;

	CSMmass = v->GetMass();
	LMmass = GetDockedVesselMass(v);

	CalcSPSGimbalTrimAngles(CSMmass, LMmass, p_T, y_T);

	pad.ThisVehicleWeight = CSMmass / 0.45359237;
	pad.OtherVehicleWeight = LMmass / 0.45359237;
	pad.PitchTrim = p_T * DEG + 2.15;
	pad.YawTrim = y_T * DEG - 0.95;
}

void RTCC::LMDAPUpdate(VESSEL *v, AP10DAPDATA &pad)
{
	double CSMmass, LMmass;

	LMmass = v->GetMass();
	CSMmass = GetDockedVesselMass(v);

	pad.ThisVehicleWeight = LMmass / 0.45359237;
	pad.OtherVehicleWeight = CSMmass / 0.45359237;
	pad.PitchTrim = 6.0;
	pad.YawTrim = 6.0;
}

void RTCC::EarthOrbitEntry(EarthEntryPADOpt *opt, AP7ENT &pad)
{
	double mu, SVMJD, GET, EMSAlt, theta_T, m1,v_e, EIAlt, lat, lng, KTETA;
	double dt;//from SV time to deorbit maneuver
	double t_go; //from deorbit TIG to shutdown
	double dt2; //from shutdown to EI
	double dt3; //from EI to 300k
	double dt4; //from 300k to 0.05g
	VECTOR3 R_A, V_A, R0B, V0B, R1B, V1B, UX, UY, UZ, DV_P, DV_C, V_G, R2B, V2B, R05G, V05G, EIangles, REI, VEI, R300K, V300K;
	VECTOR3 UXD, UYD, UZD;
	MATRIX3 M_R;
	OBJHANDLE gravref;

	double ALFATRIM = -20.0*RAD;

	gravref = AGCGravityRef(opt->vessel);
	mu = GGRAV*oapiGetMass(gravref);

	opt->vessel->GetRelativePos(gravref, R_A);
	opt->vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();
	GET = (SVMJD - opt->GETbase)*24.0*3600.0;

	R0B = _V(R_A.x, R_A.z, R_A.y);
	V0B = _V(V_A.x, V_A.z, V_A.y);

	EMSAlt = 284643.0*0.3048;
	EIAlt = 400000.0*0.3048;

	KTETA = 1000.0;

	if (opt->preburn)
		{
		double F, m_cut, EntryRTGO, EntryVIO, EntryRET;

		F = SPS_THRUST;
		v_e = SPS_ISP;

		dt = opt->P30TIG - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
		OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, gravref);

		UY = unit(crossp(V1B, R1B));
		UZ = unit(-R1B);
		UX = crossp(UY, UZ);

		DV_P = UX*opt->dV_LVLH.x + UZ*opt->dV_LVLH.z;
		theta_T = length(crossp(R1B, V1B))*length(opt->dV_LVLH)*opt->vessel->GetMass() / OrbMech::power(length(R1B), 2.0) / 92100.0;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + UY*opt->dV_LVLH.y;
		OrbMech::poweredflight(R1B, V1B, SVMJD + dt / 24.0 / 3600.0, gravref, F, v_e, opt->vessel->GetMass(), V_G, R2B, V2B, m_cut, t_go);

		dt2 = OrbMech::time_radius_integ(R2B, V2B, SVMJD + (dt + t_go) / 3600.0 / 24.0, oapiGetSize(gravref) + EIAlt, -1, gravref, gravref, REI, VEI);
		dt3 = OrbMech::time_radius_integ(REI, VEI, SVMJD + (dt + t_go + dt2) / 3600.0 / 24.0, oapiGetSize(gravref) + 300000.0*0.3048, -1, gravref, gravref, R300K, V300K);
		dt4 = OrbMech::time_radius_integ(R300K, V300K, SVMJD + (dt + t_go + dt2 + dt3) / 3600.0 / 24.0, oapiGetSize(gravref) + EMSAlt, -1, gravref, gravref, R05G, V05G);

		EntryCalculations::Reentry(REI, VEI, SVMJD + (dt + t_go + dt2) / 3600.0 / 24.0, false, lat, lng, EntryRTGO, EntryVIO, EntryRET);

		UX = unit(-V05G);
		UY = unit(crossp(UX, -R05G));
		UZ = unit(crossp(UX, crossp(UX, -R05G)));

		UYD = UY;
		UXD = unit(crossp(UYD, UX))*sin(ALFATRIM) + UX*cos(ALFATRIM);
		UZD = crossp(UXD, UYD);

		M_R = _M(UXD.x, UXD.y, UXD.z, UYD.x, UYD.y, UYD.z, UZD.x, UZD.y, UZD.z);
		EIangles = OrbMech::CALCGAR(opt->REFSMMAT, M_R);

		m1 = opt->vessel->GetMass()*exp(-length(opt->dV_LVLH) / v_e);

		double WIE, WT, theta_rad, LSMJD;
		VECTOR3 RTE, UTR, urh, URT0, URT, R_LS, R_P;
		MATRIX3 Rot2;
		LSMJD = (dt + t_go + dt2 + dt3 + dt4) / 24.0 / 3600.0 + SVMJD;
		R_P = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)));
		Rot2 = OrbMech::GetRotationMatrix(gravref, LSMJD);
		R_LS = mul(Rot2, R_P);
		//R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));
		R_LS = _V(R_LS.x, R_LS.z, R_LS.y);
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

		pad.Att400K[0] = _V(OrbMech::imulimit(EIangles.x*DEG), OrbMech::imulimit(EIangles.y*DEG), OrbMech::imulimit(EIangles.z*DEG));
		pad.dVTO[0] = 0.0;//-60832.18 / m1 / 0.3048;
		if (opt->lat == 0)
		{
			pad.Lat[0] = lat*DEG;
			pad.Lng[0] = lng*DEG;
		}
		else
		{
			pad.Lat[0] = opt->lat*DEG;
			pad.Lng[0] = opt->lng*DEG;
		}
		pad.Ret05[0] = t_go + dt2 + dt3 + dt4;
		pad.RTGO[0] = theta_rad*3437.7468;//entry->EntryRTGO;
		pad.VIO[0] = EntryVIO / 0.3048;
	}
	else
	{
		double EMSTime, LSMJD, dt5, theta_rad, theta_nm;
		VECTOR3 R_P, R_LS;
		MATRIX3 Rot2;

		dt = opt->P30TIG - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
		dt2 = OrbMech::time_radius_integ(R0B, V0B, SVMJD, oapiGetSize(gravref) + EIAlt, -1, gravref, gravref, REI, VEI);
		dt3 = OrbMech::time_radius_integ(REI, VEI, SVMJD + dt2 / 24.0 / 3600.0, oapiGetSize(gravref) + 300000.0*0.3048, -1, gravref, gravref, R300K, V300K);
		dt4 = OrbMech::time_radius_integ(R300K, V300K, SVMJD + (dt2 + dt3) / 24.0 / 3600.0, oapiGetSize(gravref) + EMSAlt, -1, gravref, gravref, R05G, V05G);

		UX = unit(-V05G);
		UY = unit(crossp(UX, -R05G));
		UZ = unit(crossp(UX, crossp(UX, -R05G)));

		UYD = UY;
		UXD = unit(crossp(UYD, UX))*sin(ALFATRIM) + UX*cos(ALFATRIM);
		UZD = crossp(UXD, UYD);

		M_R = _M(UXD.x, UXD.y, UXD.z, UYD.x, UYD.y, UYD.z, UZD.x, UZD.y, UZD.z);
		EIangles = OrbMech::CALCGAR(opt->REFSMMAT, M_R);

		dt5 = 500.0;
		EMSTime = dt2 + dt3 + dt4 + (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;

		/*for (int i = 0;i < 10;i++)
		{
			LSMJD = (EMSTime + dt5) / 24.0 / 3600.0 + opt->GETbase;
			R_P = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)));
			Rot2 = OrbMech::GetRotationMatrix2(gravref, LSMJD);
			R_LS = mul(Rot2, R_P);
			R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));
			theta_rad = acos(dotp(R_LS, unit(R300K)));
			theta_nm = 3437.7468*theta_rad;
			if (length(V300K) >= 26000.0*0.3048)
			{
				dt5 = theta_nm / 3.0;
			}
			else
			{
				dt5 = 8660.0*theta_nm / (length(V300K) / 0.3048);
			}
		}*/
		double WIE, WT;
		VECTOR3 RTE, UTR, urh, URT0, URT;
		LSMJD = EMSTime / 24.0 / 3600.0 + opt->GETbase;
		R_P = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)));
		Rot2 = OrbMech::GetRotationMatrix(gravref, LSMJD);
		R_LS = mul(Rot2, R_P);
		//R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));
		R_LS = _V(R_LS.x, R_LS.z, R_LS.y);
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

		pad.PB_RTGO[0] = theta_nm;//-3437.7468*acos(dotp(unit(R300K), unit(R05G)));
		pad.PB_R400K[0] = EIangles.x*DEG;
		pad.PB_Ret05[0] = dt2 + dt3 + dt4 - dt;
		pad.PB_VIO[0] = length(V05G) / 0.3048;
	}
}

void RTCC::LunarEntryPAD(LunarEntryPADOpt *opt, AP11ENT &pad)
{
	VECTOR3 R_A, V_A, R0B, V0B, R_P, R_LS, URT0, UUZ, RTE, UTR, urh, URT, UX, UY, UZ, EIangles, UREI;
	MATRIX3 M_R, Rot2;
	double SVMJD, dt, dt2, dt3, EIAlt, Alt300K, EMSAlt, S_FPA, g_T, V_T, v_BAR, RET05, liftline, EntryPADV400k, EntryPADVIO, mu_M;
	double WIE, WT, LSMJD, theta_rad, theta_nm, EntryPADDO, EntryPADGMax, EntryPADgamma400k, EntryPADHorChkGET, EIGET, EntryPADHorChkPit, KTETA;
	OBJHANDLE gravref, hEarth, hMoon;
	SV sv0;		// "Now"
	SV sv1;		// "Now" or just after the maneuver
	SV svEI;	// EI/400K
	SV sv300K;  // 300K
	SV sv05G;   // EMS Altitude / 0.05G

	gravref = AGCGravityRef(opt->vessel);
	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_M = GGRAV*oapiGetMass(hMoon);

	EIAlt = 400000.0*0.3048;
	Alt300K = 300000.0*0.3048;
	EMSAlt = 297431.0*0.3048;

	opt->vessel->GetRelativePos(gravref, R_A);
	opt->vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();

	R0B = _V(R_A.x, R_A.z, R_A.y);
	V0B = _V(V_A.x, V_A.z, V_A.y);

	sv0.gravref = gravref;
	sv0.MJD = SVMJD;
	sv0.R = R0B;
	sv0.V = V0B;
	sv0.mass = opt->vessel->GetMass();

	if (opt->direct || length(opt->dV_LVLH) == 0.0)	//Check against a DV of 0
	{
		sv1 = sv0;
	}
	else
	{
		sv1 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->P30TIG, opt->dV_LVLH, sv0, 0);
	}

	if (sv1.gravref == hMoon)
	{
		double dt_r = OrbMech::time_radius(sv1.R, sv1.V, 64373760.0, 1.0, mu_M);
		sv1 = coast(sv1, dt_r);
	}

	dt = OrbMech::time_radius_integ(sv1.R, sv1.V, sv1.MJD, oapiGetSize(hEarth) + EIAlt, -1, sv1.gravref, hEarth, svEI.R, svEI.V);
	svEI.gravref = hEarth;
	svEI.mass = sv1.mass;
	svEI.MJD = sv1.MJD + dt / 24.0 / 3600.0;

	double EntryRET, lat, lng, rtgo, vio;

	EntryCalculations::Reentry(svEI.R, svEI.V, svEI.MJD, true, lat, lng, rtgo, vio, EntryRET);

	LSMJD = svEI.MJD + EntryRET / 24.0 / 3600.0;

	dt2 = OrbMech::time_radius_integ(svEI.R, svEI.V, svEI.MJD, oapiGetSize(hEarth) + Alt300K, -1, hEarth, hEarth, sv300K.R, sv300K.V);
	sv300K.gravref = hEarth;
	sv300K.mass = svEI.mass;
	sv300K.MJD = svEI.MJD + dt2 / 24.0 / 3600.0;

	dt3 = OrbMech::time_radius_integ(sv300K.R, sv300K.V, sv300K.MJD, oapiGetSize(hEarth) + EMSAlt, -1, hEarth, hEarth, sv05G.R, sv05G.V);
	sv05G.gravref = hEarth;
	sv05G.mass = sv300K.mass;
	sv05G.MJD = sv300K.MJD + dt3 / 24.0 / 3600.0;

	EntryPADVIO = length(sv05G.V);

	R_P = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)));
	Rot2 = OrbMech::GetRotationMatrix(hEarth, LSMJD);
	R_LS = mul(Rot2, R_P);
	//R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));
	R_LS = _V(R_LS.x, R_LS.z, R_LS.y);
	URT0 = R_LS;
	WIE = 72.9211505e-6;
	KTETA = 1000.0;
	UUZ = _V(0, 0, 1);
	RTE = crossp(UUZ, URT0);
	UTR = crossp(RTE, UUZ);
	urh = unit(sv05G.R);//unit(r)*cos(theta) + crossp(unit(r), -unit(h_apo))*sin(theta);
	theta_rad = acos(dotp(URT0, urh));
	for (int i = 0;i < 10;i++)
	{
		WT = WIE*(KTETA*theta_rad);
		URT = URT0 + UTR*(cos(WT) - 1.0) + RTE*sin(WT);
		theta_rad = acos(dotp(URT, urh));
	}
	theta_nm = theta_rad*3437.7468;

	UX = unit(-sv05G.V);
	UY = unit(crossp(UX, -sv05G.R));
	UZ = unit(crossp(UX, crossp(UX, -sv05G.R)));

	double ALFATRIM = -20.0*RAD;
	VECTOR3 UXD, UYD, UZD;

	UYD = UY;
	UXD = unit(crossp(UYD, UX))*sin(ALFATRIM) + UX*cos(ALFATRIM);
	UZD = crossp(UXD, UYD);

	M_R = _M(UXD.x, UXD.y, UXD.z, UYD.x, UYD.y, UYD.z, UZD.x, UZD.y, UZD.z);
	EIangles = OrbMech::CALCGAR(opt->REFSMMAT, M_R);

	S_FPA = dotp(unit(sv300K.R), sv300K.V) / length(sv300K.V);
	g_T = asin(S_FPA);
	V_T = length(sv300K.V);
	v_BAR = (V_T / 0.3048 - 36000.0) / 20000.0;
	EntryPADGMax = 4.0 / (1.0 + 4.8*v_BAR*v_BAR)*(abs(g_T)*DEG - 6.05 - 2.4*v_BAR*v_BAR) + 10.0;

	UREI = unit(svEI.R);
	EntryPADV400k = length(svEI.V);
	S_FPA = dotp(UREI, svEI.V) / EntryPADV400k;
	EntryPADgamma400k = asin(S_FPA);
	EIGET = (svEI.MJD - opt->GETbase)*24.0*3600.0;
	RET05 = (sv05G.MJD - svEI.MJD)*24.0*3600.0;

	double vei;
	vei = length(svEI.V) / 0.3048;

	EntryPADDO = 4.2317708e-9*vei*vei + 1.4322917e-6*vei - 1.600664062;

	liftline = 4.055351e-10*vei*vei - 3.149125e-5*vei + 0.503280635;
	if (S_FPA > atan(liftline))
	{
		sprintf(pad.LiftVector[0], "DN");
	}
	else
	{
		sprintf(pad.LiftVector[0], "UP");
	}

	SV svHorCheck, svSxtCheck;
	svHorCheck = coast(svEI, -17.0*60.0);
	svSxtCheck = coast(svEI, -60.0*60.0);

	EntryPADHorChkGET = EIGET - 17.0*60.0;

	double Entrytrunnion, Entryshaft, EntryBSSpitch, EntryBSSXPos;
	int Entrystaroct, EntryCOASstaroct;
	OrbMech::checkstar(opt->REFSMMAT, _V(OrbMech::round(EIangles.x*DEG)*RAD, OrbMech::round(EIangles.y*DEG)*RAD, OrbMech::round(EIangles.z*DEG)*RAD), svSxtCheck.R, oapiGetSize(hEarth), Entrystaroct, Entrytrunnion, Entryshaft);
	OrbMech::coascheckstar(opt->REFSMMAT, _V(OrbMech::round(EIangles.x*DEG)*RAD, OrbMech::round(EIangles.y*DEG)*RAD, OrbMech::round(EIangles.z*DEG)*RAD), svSxtCheck.R, oapiGetSize(hEarth), EntryCOASstaroct, EntryBSSpitch, EntryBSSXPos);

	double horang, coastang, IGA, cosIGA, sinIGA;
	VECTOR3 X_NB, Y_NB, Z_NB, X_SM, Y_SM, Z_SM, A_MG;

	horang = asin(oapiGetSize(gravref) / length(svHorCheck.R));
	coastang = dotp(unit(svEI.R), unit(svHorCheck.R));

	Z_NB = unit(-svEI.R);
	Y_NB = unit(crossp(svEI.V, svEI.R));
	X_NB = crossp(Y_NB, Z_NB);

	X_SM = _V(opt->REFSMMAT.m11, opt->REFSMMAT.m12, opt->REFSMMAT.m13);
	Y_SM = _V(opt->REFSMMAT.m21, opt->REFSMMAT.m22, opt->REFSMMAT.m23);
	Z_SM = _V(opt->REFSMMAT.m31, opt->REFSMMAT.m32, opt->REFSMMAT.m33);
	A_MG = unit(crossp(X_NB, Y_SM));
	cosIGA = dotp(A_MG, Z_SM);
	sinIGA = dotp(A_MG, X_SM);
	IGA = atan2(sinIGA, cosIGA);

	EntryPADHorChkPit = PI2 - (horang + coastang + 0.0*31.7*RAD) + IGA;	//Currently COAS, not 31.7° line

	pad.Att05[0] = _V(OrbMech::imulimit(EIangles.x*DEG), OrbMech::imulimit(EIangles.y*DEG), OrbMech::imulimit(EIangles.z*DEG));
	pad.BSS[0] = EntryCOASstaroct;
	pad.DLMax[0] = 0.0;
	pad.DLMin[0] = 0.0;
	pad.DO[0] = EntryPADDO;
	pad.Gamma400K[0] = EntryPADgamma400k*DEG;
	pad.GETHorCheck[0] = EntryPADHorChkGET;
	pad.Lat[0] = opt->lat*DEG;
	pad.Lng[0] = opt->lng*DEG;
	pad.MaxG[0] = EntryPADGMax;
	pad.PitchHorCheck[0] = OrbMech::imulimit(EntryPADHorChkPit*DEG);
	pad.RET05[0] = RET05;
	pad.RETBBO[0] = 0.0;
	pad.RETDRO[0] = 0.0;
	pad.RETEBO[0] = 0.0;
	pad.RETVCirc[0] = 0.0;
	pad.RRT[0] = EIGET;
	pad.RTGO[0] = theta_nm;
	pad.SXTS[0] = Entrystaroct;
	pad.SFT[0] = Entryshaft*DEG;
	pad.TRN[0] = Entrytrunnion*DEG;
	pad.SPA[0] = EntryBSSpitch*DEG;
	pad.SXP[0] = EntryBSSXPos*DEG;
	pad.V400K[0] = EntryPADV400k / 0.3048;
	pad.VIO[0] = EntryPADVIO / 0.3048;
	pad.VLMax[0] = 0.0;
	pad.VLMin[0] = 0.0;
}

MATRIX3 RTCC::GetREFSMMATfromAGC(agc_t *agc, double AGCEpoch, int addroff)
{
	MATRIX3 REFSMMAT;
	char Buffer[100];
	int REFSMMAToct[20];

	int REFSaddr = 01735 + addroff;

	unsigned short REFSoct[20];
	REFSoct[2] = agc->Erasable[0][REFSaddr];
	REFSoct[3] = agc->Erasable[0][REFSaddr + 1];
	REFSoct[4] = agc->Erasable[0][REFSaddr + 2];
	REFSoct[5] = agc->Erasable[0][REFSaddr + 3];
	REFSoct[6] = agc->Erasable[0][REFSaddr + 4];
	REFSoct[7] = agc->Erasable[0][REFSaddr + 5];
	REFSoct[8] = agc->Erasable[0][REFSaddr + 6];
	REFSoct[9] = agc->Erasable[0][REFSaddr + 7];
	REFSoct[10] = agc->Erasable[0][REFSaddr + 8];
	REFSoct[11] = agc->Erasable[0][REFSaddr + 9];
	REFSoct[12] = agc->Erasable[0][REFSaddr + 10];
	REFSoct[13] = agc->Erasable[0][REFSaddr + 11];
	REFSoct[14] = agc->Erasable[0][REFSaddr + 12];
	REFSoct[15] = agc->Erasable[0][REFSaddr + 13];
	REFSoct[16] = agc->Erasable[0][REFSaddr + 14];
	REFSoct[17] = agc->Erasable[0][REFSaddr + 15];
	REFSoct[18] = agc->Erasable[0][REFSaddr + 16];
	REFSoct[19] = agc->Erasable[0][REFSaddr + 17];
	for (int i = 2; i < 20; i++)
	{
		sprintf(Buffer, "%05o", REFSoct[i]);
		REFSMMAToct[i] = atoi(Buffer);
	}
	
	REFSMMAT.m11 = OrbMech::DecToDouble(REFSoct[2], REFSoct[3])*2.0;
	REFSMMAT.m12 = OrbMech::DecToDouble(REFSoct[4], REFSoct[5])*2.0;
	REFSMMAT.m13 = OrbMech::DecToDouble(REFSoct[6], REFSoct[7])*2.0;
	REFSMMAT.m21 = OrbMech::DecToDouble(REFSoct[8], REFSoct[9])*2.0;
	REFSMMAT.m22 = OrbMech::DecToDouble(REFSoct[10], REFSoct[11])*2.0;
	REFSMMAT.m23 = OrbMech::DecToDouble(REFSoct[12], REFSoct[13])*2.0;
	REFSMMAT.m31 = OrbMech::DecToDouble(REFSoct[14], REFSoct[15])*2.0;
	REFSMMAT.m32 = OrbMech::DecToDouble(REFSoct[16], REFSoct[17])*2.0;
	REFSMMAT.m33 = OrbMech::DecToDouble(REFSoct[18], REFSoct[19])*2.0;
	
	return mul(REFSMMAT, OrbMech::J2000EclToBRCS(AGCEpoch));
}

double RTCC::GetClockTimeFromAGC(agc_t *agc)
{
	return agc->Erasable[AGC_BANK(025)][AGC_ADDR(025)] + agc->Erasable[AGC_BANK(024)][AGC_ADDR(024)] * pow((double) 2., (double) 14.);
}

double RTCC::GetTEPHEMFromAGC(agc_t *agc)
{
	return agc->Erasable[AGC_BANK(01710)][AGC_ADDR(01710)] +
		agc->Erasable[AGC_BANK(01707)][AGC_ADDR(01707)] * pow((double) 2., (double) 14.) +
		agc->Erasable[AGC_BANK(01706)][AGC_ADDR(01706)] * pow((double) 2., (double) 28.);
}

void RTCC::navcheck(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double &lat, double &lng, double &alt)
{
	//R and V in the J2000

	MATRIX3 Rot2;
	VECTOR3 Requ, Recl, u;
	double sinl, a, b, gamma,r_0;

	a = 6378166;
	b = 6356784;

	Rot2 = OrbMech::GetRotationMatrix(gravref, MJD);

	Recl = _V(R.x, R.z, R.y);

	Requ = tmul(Rot2, Recl);
	Requ = _V(Requ.x, Requ.z, Requ.y);

	u = unit(Requ);
	sinl = u.z;

	if (gravref == oapiGetObjectByName("Earth"))
	{
		gamma = b*b / a / a;
	}
	else
	{
		gamma = 1;
	}
	r_0 = oapiGetSize(gravref);

	lat = atan(u.z/(gamma*sqrt(u.x*u.x + u.y*u.y)));
	lng = atan2(u.y, u.x);
	alt = length(Requ) - r_0;
}

SV RTCC::StateVectorCalc(VESSEL *vessel, double SVMJD)
{
	VECTOR3 R, V;
	double dt;
	OBJHANDLE gravref;
	SV sv, sv1;

	gravref = AGCGravityRef(vessel);

	vessel->GetRelativePos(gravref, R);
	vessel->GetRelativeVel(gravref, V);
	sv.MJD = oapiGetSimMJD();

	sv.R = _V(R.x, R.z, R.y);
	sv.V = _V(V.x, V.z, V.y);

	sv.gravref = gravref;
	sv.mass = vessel->GetMass();

	if (SVMJD != 0.0)
	{
		dt = (SVMJD - sv.MJD)*24.0*3600.0;
		sv1 = coast(sv, dt);
	}
	else
	{
		sv1 = sv;
	}

	return sv1;
}

OBJHANDLE RTCC::AGCGravityRef(VESSEL *vessel)
{
	OBJHANDLE gravref;
	VECTOR3 rsph;

	gravref = oapiGetObjectByName("Moon");
	vessel->GetRelativePos(gravref, rsph);
	if (length(rsph) > 64373760.0)
	{
		gravref = oapiGetObjectByName("Earth");
	}
	return gravref;
}

double RTCC::getGETBase()
{
	double GET, SVMJD;
	SVMJD = oapiGetSimMJD();
	GET = mcc->cm->GetMissionTime();
	return SVMJD - GET / 24.0 / 3600.0;
}

MATRIX3 RTCC::REFSMMATCalc(REFSMMATOpt *opt)
{
	VECTOR3 UX, UY, UZ;
	OBJHANDLE hMoon, hEarth;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");

	//Here the options that don't require a state vector or thrust parameters
	if (opt->REFSMMATopt == 4)
	{
		//For now a default LC-39A, 72° launch
		return OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, opt->GETbase, 72 * RAD);
	}
	else if (opt->REFSMMATopt == 6)
	{
		double *MoonPos;
		double PTCMJD;
		VECTOR3 R_ME;

		MoonPos = new double[12];

		PTCMJD = opt->REFSMMATTime / 24.0 / 3600.0 + opt->GETbase;

		CELBODY *cMoon = oapiGetCelbodyInterface(hMoon);

		cMoon->clbkEphemeris(PTCMJD, EPHEM_TRUEPOS, MoonPos);

		R_ME = -_V(MoonPos[0], MoonPos[1], MoonPos[2]);

		UX = unit(crossp(_V(0.0, 1.0, 0.0), unit(R_ME)));
		//UX = mul(Rot, _V(UX.x, UX.z, UX.y));
		UX = _V(UX.x, UX.z, UX.y);

		UZ = _V(0.0, 0.0, -1.0);
		UY = crossp(UZ, UX);
		return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}
	else if (opt->REFSMMATopt == 8)
	{
		MATRIX3 Rot2;
		VECTOR3 R_P, R_LS, R_LPO, V_LPO, H_LPO, axis, H_C;
		double LSMJD;

		R_P = unit(_V(cos(opt->LSLng)*cos(opt->LSLat), sin(opt->LSLat), sin(opt->LSLng)*cos(opt->LSLat)));
		LSMJD = opt->REFSMMATTime / 24.0 / 3600.0 + opt->GETbase;
		Rot2 = OrbMech::GetRotationMatrix(oapiGetObjectByName("Moon"), LSMJD);

		R_LS = mul(Rot2, R_P);
		R_LS = _V(R_LS.x, R_LS.z, R_LS.y);

		OrbMech::adbar_from_rv(1.0, 1.0, opt->LSLng, opt->LSLat, PI05, opt->LSAzi, R_LPO, V_LPO);
		H_LPO = crossp(R_LPO, V_LPO);
		axis = unit(H_LPO);
		H_C = unit(rhmul(Rot2, axis));

		UX = unit(R_LS);
		UZ = unit(crossp(H_C, R_LS));
		UY = crossp(UZ, UX);

		return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}

	double SVMJD, dt, LMmass;
	VECTOR3 R_A, V_A, X_B;
	VECTOR3 DV_P, DV_C, V_G, X_SM, Y_SM, Z_SM;
	double theta_T;
	OBJHANDLE gravref;
	SV sv0, sv2;
	THGROUP_TYPE th_main;

	if (opt->vessel->GetGroupThruster(THGROUP_MAIN, 0) == NULL)
	{
		th_main = THGROUP_HOVER;
	}
	else
	{
		th_main = THGROUP_MAIN;
	}

	gravref = AGCGravityRef(opt->vessel);

	opt->vessel->GetRelativePos(gravref, R_A);
	opt->vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();

	R_A = _V(R_A.x, R_A.z, R_A.y);
	V_A = _V(V_A.x, V_A.z, V_A.y);

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	sv0.gravref = gravref;
	sv0.mass = opt->vessel->GetMass();
	sv0.MJD = SVMJD;
	sv0.R = R_A;
	sv0.V = V_A;

	if (opt->REFSMMATopt == 7)
	{
		SV sv1;

		sv1 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->P30TIG, opt->dV_LVLH, sv0, LMmass);
		sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->P30TIG2, opt->dV_LVLH2, sv1, LMmass);
	}
	else if (opt->REFSMMATdirect == false && length(opt->dV_LVLH) != 0.0 )	//Check against DV = 0, some calculations could break down
	{
		sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->P30TIG, opt->dV_LVLH, sv0, LMmass);
	}
	else
	{
		sv2 = sv0;
	}

	if (opt->REFSMMATopt == 5)
	{
		double LSMJD;
		VECTOR3 R_P, R_LS, H_C;
		MATRIX3 Rot2;
		SV sv3;

		LSMJD = opt->REFSMMATTime / 24.0 / 3600.0 + opt->GETbase;

		R_P = unit(_V(cos(opt->LSLng)*cos(opt->LSLat), sin(opt->LSLat), sin(opt->LSLng)*cos(opt->LSLat)));

		Rot2 = OrbMech::GetRotationMatrix(oapiGetObjectByName("Moon"), LSMJD);

		R_LS = mul(Rot2, R_P);
		R_LS = _V(R_LS.x, R_LS.z, R_LS.y);

		OrbMech::oneclickcoast(sv2.R, sv2.V, sv2.MJD, (LSMJD - sv2.MJD)*24.0*3600.0, sv3.R, sv3.V, sv2.gravref, hMoon);
		sv3.gravref = hMoon;
		sv3.mass = sv2.mass;
		H_C = crossp(sv3.R, sv3.V);

		UX = unit(R_LS);
		UZ = unit(crossp(H_C, R_LS));
		UY = crossp(UZ, UX);

		return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}
	else
	{
		SV sv4;

		if (opt->REFSMMATopt == 2)
		{
			dt = opt->REFSMMATTime - (sv2.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
			sv4 = coast(sv2, dt);
		}
		else if (opt->REFSMMATopt == 7)
		{
			double t_p, mu;
			SV sv3;

			mu = GGRAV*oapiGetMass(hMoon);
			t_p = OrbMech::period(sv2.R, sv2.V, mu);
			OrbMech::oneclickcoast(sv2.R, sv2.V, sv2.MJD, 1.5*t_p, sv3.R, sv3.V, sv2.gravref, hMoon);
			sv3.gravref = hMoon;
			sv3.mass = sv2.mass;
			sv3.MJD = sv2.MJD + 1.5*t_p / 24.0 / 3600.0;

			OrbMech::time_radius_integ(sv3.R, sv3.V, sv3.MJD, oapiGetSize(hMoon) + 60.0*1852.0, -1, hMoon, hMoon, sv4.R, sv4.V);
		}
		else if (opt->REFSMMATopt == 0 || opt->REFSMMATopt == 1)
		{
			dt = opt->P30TIG - (sv2.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
			sv4 = coast(sv2, dt);
		}
		else
		{
			dt = OrbMech::time_radius_integ(sv2.R, sv2.V, sv2.MJD, oapiGetSize(hEarth) + 400000.0*0.3048, -1, sv2.gravref, hEarth, sv4.R, sv4.V);
		}

		UY = unit(crossp(sv4.V, sv4.R));
		UZ = unit(-sv4.R);
		UX = crossp(UY, UZ);

		if (opt->REFSMMATopt == 0 || opt->REFSMMATopt == 1)
		{
			double headsswitch;

			if (opt->HeadsUp)
			{
				headsswitch = 1.0;
			}
			else
			{
				headsswitch = -1.0;
			}

			DV_P = UX*opt->dV_LVLH.x + UZ*opt->dV_LVLH.z;
			if (length(DV_P) != 0.0)
			{
				theta_T = length(crossp(sv4.R, sv4.V))*length(opt->dV_LVLH)*(sv4.mass + LMmass) / OrbMech::power(length(sv4.R), 2.0) / 92100.0;
				DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
				V_G = DV_C + UY*opt->dV_LVLH.y;
			}
			else
			{
				V_G = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;
			}

			if (opt->vesseltype < 2)
			{
				MATRIX3 M, M_R, M_RTM;
				double p_T, y_T;

				CalcSPSGimbalTrimAngles(sv4.mass, LMmass, p_T, y_T);

				if (opt->REFSMMATopt == 0)
				{
					X_B = unit(V_G);
				}
				else
				{
					headsswitch = 1.0;
					p_T = 2.15*RAD;
					X_B = -unit(V_G);
				}
				UX = X_B;
				UY = unit(crossp(UX, sv4.R*headsswitch));
				UZ = unit(crossp(UX, UY));

				M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
				M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
				M_RTM = mul(OrbMech::transpose_matrix(M_R), M);
				X_SM = mul(M_RTM, _V(1.0, 0.0, 0.0));
				Y_SM = mul(M_RTM, _V(0.0, 1.0, 0.0));
				Z_SM = mul(M_RTM, _V(0.0, 0.0, 1.0));
			}
			else
			{
				VECTOR3 U_FDI;

				U_FDI = unit(V_G);

				if (opt->REFSMMATopt == 0)
				{
					X_SM = U_FDI;
				}
				else
				{
					X_SM = -U_FDI;
				}
				if (abs(dotp(X_SM, unit(sv4.R))) < cos(0.01*RAD))
				{
					Y_SM = unit(crossp(X_SM, -sv4.R*headsswitch));
				}
				else
				{
					Y_SM = unit(crossp(X_SM, sv4.V));
				}
				Z_SM = unit(crossp(X_SM, Y_SM));
			}

			return _M(X_SM.x, X_SM.y, X_SM.z, Y_SM.x, Y_SM.y, Y_SM.z, Z_SM.x, Z_SM.y, Z_SM.z);

			//IMUangles = OrbMech::CALCGAR(REFSMMAT, mul(transpose_matrix(M), M_R));
			//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);
		}
		else
		{
			return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		}
		//
	}
}

double RTCC::CDHcalc(CDHOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG)			//Calculates the required DV vector of a coelliptic burn
{
	double mu;
	double SVMJD, SVtime, dt, dt2, c1, c2, theta, SW, dh_CDH, VPV, dt2_apo, CDHtime_cor, mass;
	VECTOR3 RA0_orb, VA0_orb, RP0_orb, VP0_orb;
	VECTOR3 RA0, VA0, RP0, VP0, RA2_alt, VA2_alt, V_A2_apo, CDHdeltaV;
	VECTOR3 u, RPC, VPC;
	double epsilon, a_P, a_A, V_AH, V_AV;
	MATRIX3 Q_Xx;
	//MATRIX3 obli;
	VECTOR3 RA2, VA2, RP2, VP2;
	OBJHANDLE gravref;

	gravref = AGCGravityRef(opt->vessel);

	mu = GGRAV*oapiGetMass(gravref);

	//DH_met = DH*1852.0;							//Calculates the desired delta height of the coellitpic orbit in metric units

	opt->vessel->GetRelativePos(gravref, RA0_orb);	//vessel position vector
	opt->vessel->GetRelativeVel(gravref, VA0_orb);	//vessel velocity vector
	opt->target->GetRelativePos(gravref, RP0_orb);	//target position vector
	opt->target->GetRelativeVel(gravref, VP0_orb);	//target velocity vector
	SVtime = oapiGetSimTime();					//The time mark of the state vectors

	//oapiGetPlanetObliquityMatrix(gravref, &obli);

	//RA0_orb = mul(OrbMech::inverse(obli), RA0_orb);
	//VA0_orb = mul(OrbMech::inverse(obli), VA0_orb);
	//RP0_orb = mul(OrbMech::inverse(obli), RP0_orb);
	//VP0_orb = mul(OrbMech::inverse(obli), VP0_orb);

	RA0 = _V(RA0_orb.x, RA0_orb.z, RA0_orb.y);	//Coordinate system nightmare. My calculation methods use another system
	VA0 = _V(VA0_orb.x, VA0_orb.z, VA0_orb.y);
	RP0 = _V(RP0_orb.x, RP0_orb.z, RP0_orb.y);
	VP0 = _V(VP0_orb.x, VP0_orb.z, VP0_orb.y);

	SVMJD = oapiTime2MJD(SVtime);				//Calculates the MJD of the state vector

	mass = opt->vessel->GetMass();

	if (opt->CDHtimemode == 0)
	{
		CDHtime_cor = opt->TIG;
		dt2 = opt->TIG - (SVMJD - opt->GETbase) * 24 * 60 * 60;
	}
	else
	{
		int n = 0;

		dt = opt->TIG - (SVMJD - opt->GETbase) * 24 * 60 * 60;

		dt2 = dt + 10.0;							//A secant search method is used to find the time, when the desired delta height is reached. Other values might work better.

		while (abs(dt2 - dt) > 0.1 && n <= 20)					//0.1 seconds accuracy should be enough
		{
			c1 = OrbMech::NSRsecant(RA0, VA0, RP0, VP0, SVMJD, dt, opt->DH, gravref);		//c is the difference between desired and actual DH
			c2 = OrbMech::NSRsecant(RA0, VA0, RP0, VP0, SVMJD, dt2, opt->DH, gravref);

			dt2_apo = dt2 - (dt2 - dt) / (c2 - c1)*c2;						//secant method
			dt = dt2;
			dt2 = dt2_apo;
			n++;
		}

		CDHtime_cor = dt2 + (SVMJD - opt->GETbase) * 24 * 60 * 60;		//the new, calculated CDH time

	}

	//OrbMech::rv_from_r0v0(RA0, VA0, dt2, RA2, VA2, mu);
	//OrbMech::rv_from_r0v0(RP0, VP0, dt2, RP2, VP2, mu);
	OrbMech::oneclickcoast(RA0, VA0, SVMJD, dt2, RA2, VA2, gravref, gravref);//Coasting Integration, active vessel state to CDH time
	OrbMech::oneclickcoast(RP0, VP0, SVMJD, dt2, RP2, VP2, gravref, gravref);//Coasting Integration, passive vessel state to CDH time

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

	if (opt->CDHtimemode == 0)
	{
		//DH = dh_CDH / 1852.0;
	}

	VPV = dotp(VPC, RA2 / length(RA2));

	epsilon = (length(VPC)*length(VPC)) / 2 - mu / length(RPC);
	a_P = -mu / (2.0 * epsilon);
	a_A = a_P - dh_CDH;
	V_AV = VPV*OrbMech::power((a_P / a_A), 1.5);
	V_AH = sqrt(mu*(2.0 / length(RA2) - 1.0 / a_A) - (V_AV*V_AV));
	V_A2_apo = unit(crossp(u, RA2))*V_AH + unit(RA2)*V_AV;	//The desired velocity vector at CDH time													
	
	if (opt->impulsive == RTCC_IMPULSIVE)
	{
		Q_Xx = OrbMech::LVLH_Matrix(RA2, VA2);

		CDHdeltaV = mul(Q_Xx, V_A2_apo - VA2_alt); //Calculates the desired P30 LVLH DV Vector

		P30TIG = CDHtime_cor;
		dV_LVLH = CDHdeltaV;
	}
	else
	{
		VECTOR3 Llambda, RA2_cor, VA2_cor;double t_slip;

		SV sv_tig;

		sv_tig.gravref = gravref;
		sv_tig.mass = mass;
		sv_tig.MJD = opt->GETbase + CDHtime_cor / 24.0 / 3600.0;
		sv_tig.R = RA2;
		sv_tig.V = VA2_alt;

		FiniteBurntimeCompensation(opt->vesseltype, sv_tig, 0.0, V_A2_apo - VA2_alt, true, Llambda, t_slip);

		OrbMech::rv_from_r0v0(RA2, VA2_alt, t_slip, RA2_cor, VA2_cor, mu);

		Q_Xx = OrbMech::LVLH_Matrix(RA2_cor, VA2_cor);

		CDHdeltaV = mul(Q_Xx, Llambda); //Calculates the desired P30 LVLH DV Vector
		P30TIG = CDHtime_cor + t_slip;
		dV_LVLH = CDHdeltaV;

		/*dV_LVLH = CDHdeltaV;
		P30TIG = CDHtime_cor;*/
	}

	return dh_CDH;
}

LambertMan RTCC::set_lambertoptions(SV sv_A, SV sv_P, double GETbase, double T1, double T2, int N, int axis, int Perturbation, VECTOR3 Offset, double PhaseAngle)
{
	LambertMan opt;

	opt.axis = axis;
	opt.GETbase = GETbase;
	opt.N = N;
	opt.Offset = Offset;
	opt.Perturbation = Perturbation;
	opt.PhaseAngle = PhaseAngle;
	opt.sv_A = sv_A;
	opt.sv_P = sv_P;
	opt.T1 = T1;
	opt.T2 = T2;
	
	return opt;
}

double RTCC::lambertelev(VESSEL* vessel, VESSEL* target, double GETbase, double elev)
{
	double SVMJD, dt1;
	VECTOR3 RA0_orb, VA0_orb, RP0_orb, VP0_orb, RA0, VA0, RP0, VP0;
	OBJHANDLE gravref;

	gravref = AGCGravityRef(vessel);

	vessel->GetRelativePos(gravref, RA0_orb);
	vessel->GetRelativeVel(gravref, VA0_orb);
	target->GetRelativePos(gravref, RP0_orb);
	target->GetRelativeVel(gravref, VP0_orb);
	SVMJD = oapiGetSimMJD();

	RA0 = _V(RA0_orb.x, RA0_orb.z, RA0_orb.y);
	VA0 = _V(VA0_orb.x, VA0_orb.z, VA0_orb.y);
	RP0 = _V(RP0_orb.x, RP0_orb.z, RP0_orb.y);
	VP0 = _V(VP0_orb.x, VP0_orb.z, VP0_orb.y);

	dt1 = OrbMech::findelev(RA0, VA0, RP0, VP0, SVMJD, elev, gravref);

	return dt1 + (SVMJD - GETbase) * 24.0 * 60.0 * 60.0;
}

void RTCC::DOITargeting(DOIMan *opt, VECTOR3 &dV_LVLH_imp, double &P30TIG_imp, VECTOR3 &dV_LVLH, double &P30TIG, double &t_PDI, double &t_L, double &CR)
{
	double SVMJD, GET, mass, LMmass, h_DP, theta_F, t_F, mu, t_DOI, t_slip, CSMmass;
	VECTOR3 R_A, V_A, R0B, V0B, R_LSA, DV_DOI, RA2, VA2, Llambda, R2_cor, V2_cor;
	MATRIX3 Q_Xx;
	OBJHANDLE hMoon, gravref;

	hMoon = oapiGetObjectByName("Moon");

	if (opt->useSV)
	{
		R0B = opt->RV_MCC.R;
		V0B = opt->RV_MCC.V;
		SVMJD = opt->RV_MCC.MJD;
		gravref = opt->RV_MCC.gravref;
		CSMmass = opt->RV_MCC.mass;
	}
	else
	{
		gravref = AGCGravityRef(opt->vessel);
		opt->vessel->GetRelativePos(gravref, R_A);
		opt->vessel->GetRelativeVel(gravref, V_A);
		SVMJD = oapiGetSimMJD();
		R0B = _V(R_A.x, R_A.z, R_A.y);
		V0B = _V(V_A.x, V_A.z, V_A.y);
		CSMmass = opt->vessel->GetMass();
	}

	GET = (SVMJD - opt->GETbase)*24.0*3600.0;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}
	mass = LMmass + CSMmass;


	R_LSA = _V(cos(opt->lng)*cos(opt->lat), sin(opt->lng)*cos(opt->lat), sin(opt->lat))*(oapiGetSize(hMoon) + opt->alt);
	h_DP = 50000.0*0.3048;
	theta_F = opt->PeriAng;
	t_F = 718.0;
	mu = GGRAV*oapiGetMass(hMoon);

	if (opt->opt == 0)
	{
		OrbMech::LunarLandingPrediction(R0B, V0B, GET, opt->EarliestGET, R_LSA, h_DP, theta_F, t_F, hMoon, opt->GETbase, mu, opt->N, t_DOI, t_PDI, t_L, DV_DOI, CR);
	}
	else
	{
		OrbMech::LunarLandingPrediction2(R0B, V0B, GET, opt->EarliestGET, R_LSA, h_DP, 1.0, theta_F, t_F, hMoon, opt->GETbase, mu, opt->N, t_DOI, t_PDI, t_L, DV_DOI, CR);
	}

	OrbMech::oneclickcoast(R0B, V0B, SVMJD, t_DOI - GET, RA2, VA2, hMoon, hMoon);

	SV sv_tig;

	sv_tig.gravref = hMoon;
	sv_tig.mass = CSMmass;
	sv_tig.MJD = opt->GETbase + t_DOI / 24.0 / 3600.0;
	sv_tig.R = RA2;
	sv_tig.V = VA2;

	FiniteBurntimeCompensation(opt->vesseltype, sv_tig, LMmass, DV_DOI, true, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver

	OrbMech::rv_from_r0v0(RA2, VA2, t_slip, R2_cor, V2_cor, mu);//Calculate the state vector at the corrected ignition time

	Q_Xx = OrbMech::LVLH_Matrix(RA2, VA2);
	dV_LVLH_imp = mul(Q_Xx, DV_DOI);
	P30TIG_imp = t_DOI;

	Q_Xx = OrbMech::LVLH_Matrix(R2_cor, V2_cor);
	dV_LVLH = mul(Q_Xx, Llambda);
	P30TIG = t_DOI + t_slip;
}

void RTCC::PlaneChangeTargeting(PCMan *opt, VECTOR3 &dV_LVLH, double &P30TIG)
{
	double SVMJD, cosA, sinA, A, dt, MJD_A, lng, lat, rad, GET, mu, LMmass, mass, t_slip;
	MATRIX3 Rot, Q_Xx;
	VECTOR3 R_A, V_A, RLS, n1, u, n2, V_PC2, DV, loc, Llambda;
	SV sv0, sv1, sv_PC, sv_cor;

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0.gravref = AGCGravityRef(opt->vessel);
		opt->vessel->GetRelativePos(sv0.gravref, R_A);
		opt->vessel->GetRelativeVel(sv0.gravref, V_A);
		SVMJD = oapiGetSimMJD();
		sv0.R = _V(R_A.x, R_A.z, R_A.y);
		sv0.V = _V(V_A.x, V_A.z, V_A.y);
		sv0.mass = opt->vessel->GetMass();
	}

	GET = (SVMJD - opt->GETbase)*24.0*3600.0;
	MJD_A = opt->GETbase + opt->t_A / 24.0 / 3600.0;
	Rot = OrbMech::GetRotationMatrix(sv0.gravref, MJD_A);
	mu = GGRAV*oapiGetMass(sv0.gravref);

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	if (opt->landed)
	{
		opt->target->GetEquPos(lng, lat, rad);
		loc = unit(_V(cos(lng)*cos(lat), sin(lat), sin(lng)*cos(lat)))*rad;
	}
	else
	{
		loc = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)))*oapiGetSize(sv0.gravref);
	}

	RLS = mul(Rot, loc);
	RLS = _V(RLS.x, RLS.z, RLS.y);
	sv1 = coast(sv0, opt->EarliestGET - GET);

	n1 = unit(crossp(sv1.R, sv1.V));
	u = unit(crossp(-RLS, n1));
	cosA = dotp(u, unit(sv1.R));
	sinA = sqrt(1.0 - cosA*cosA);
	A = atan2(sinA, cosA);
	dt = OrbMech::time_theta(sv1.R, sv1.V, A, mu);
	sv_PC = coast(sv1, dt);
	n2 = unit(crossp(sv_PC.R, -RLS));
	V_PC2 = unit(crossp(n2, sv_PC.R))*length(sv_PC.V);

	DV = V_PC2 - sv_PC.V;

	FiniteBurntimeCompensation(opt->vesseltype, sv_PC, LMmass, DV, true, Llambda, t_slip);

	sv_cor = coast(sv_PC, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_cor.R, sv_cor.V);

	dV_LVLH = mul(Q_Xx, Llambda);

	P30TIG = opt->EarliestGET + dt + t_slip;
}

void RTCC::LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG)
{
	SV sv_node;

	LOITargeting(opt, dV_LVLH, P30TIG, sv_node);
}

void RTCC::LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_node)
{
	SV sv0, sv1;
	VECTOR3 axis, u_LPO, H, u_LAH, u_node, p, q, H_LPO;
	MATRIX3 Rot;
	double MJD_LAND, mu, dt, dt_node, R_M, apo, peri, a, e, h, mass, LMmass, GET, r, GET_node;
	OBJHANDLE hMoon;

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	GET = (sv0.MJD - opt->GETbase)*24.0*3600.0;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	hMoon = oapiGetObjectByName("Moon");
	mu = GGRAV*oapiGetMass(hMoon);
	sv_node.gravref = hMoon;

	MJD_LAND = opt->GETbase + opt->t_land / 24.0 / 3600.0;
	Rot = OrbMech::GetRotationMatrix(hMoon, MJD_LAND);

	//Lunar radius above LLS
	R_M = oapiGetSize(hMoon) + opt->alt;

	//Initial guess for the node, at pericynthion
	dt = OrbMech::timetoperi_integ(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hMoon, sv1.R, sv1.V);
	sv1 = coast(sv0, dt);
	H = crossp(sv1.R, sv1.V);

	//Lunar Approach Hyperbola orientation
	u_LAH = unit(H);

	VECTOR3 R_LPO, V_LPO;
	double r_LPO, v_LPO;

	r_LPO = R_M + 60.0*1852.0;
	v_LPO = sqrt(mu / r_LPO);

	OrbMech::adbar_from_rv(r_LPO, v_LPO, opt->lng, opt->lat, PI05, opt->azi, R_LPO, V_LPO);

	if (opt->type == 0)
	{
		H_LPO = crossp(R_LPO, V_LPO);

		axis = unit(H_LPO);

		//Lunar Parking Orbit orientation
		u_LPO = unit(rhmul(Rot, axis));

		//Node unit vector
		u_node = unit(crossp(u_LPO, u_LAH));

		//Periapsis vector and node should be within 90°, if not the actual intersection is in the other direction
		if (dotp(u_node, unit(sv1.R)) < 0.0)
		{
			u_node = -u_node;
		}

		//state vector at the node
		dt_node = OrbMech::timetonode_integ(sv1.R, sv1.V, sv1.MJD, hMoon, u_node, sv_node.R, sv_node.V);
	}
	else
	{
		axis = unit(rhmul(Rot, R_LPO));

		//Lunar Parking Orbit orientation
		u_LPO = unit(crossp(axis, unit(sv1.R)));

		if (dotp(u_LPO, u_LAH) < 0.0)
		{
			u_LPO = -u_LPO;
		}

		u_node = unit(sv1.R);
		sv_node.R = sv1.R;
		sv_node.V = sv1.V;
		dt_node = 0.0;
	}

	//Time at node
	GET_node = GET + dt + dt_node;
	sv_node.MJD = opt->GETbase + GET_node / 24.0 / 3600.0;

	apo = R_M + opt->h_apo;
	peri = R_M + opt->h_peri;
	a = (apo + peri) / 2.0;
	e = (apo - peri) / (apo + peri);
	r = length(sv_node.R);

	if (r > apo)													//If the maneuver radius is higher than the desired apoapsis, then we would get no solution
	{
		apo = r;													//sets the desired apoapsis to the current radius, so that we can calculate a maneuver
	}
	else if (r < peri)												//If the maneuver radius is lower than the desired periapsis, then we would also get no solution
	{
		peri = r;													//sets the desired periapsis to the current radius, so that we can calculate a maneuver
	}

	h = sqrt(mu*a*(1.0 - e*e));

	double ta[2];
	double f_T, isp, t_slip, MJD_cut, m_cut;
	VECTOR3 Llambda, R_cut, V_cut, R_cor, V_cor, DV[2], R_ref[2], V_ref[2];
	MATRIX3 Q_Xx;
	int sol;

	ta[0] = acos(min(1.0, max(-1.0, (a / r*(1.0 - e*e) - 1.0) / e)));	//The true anomaly of the desired orbit, min and max just to make sure this value isn't out of bounds for acos
	ta[1] = PI2 - ta[0];												//Calculates the second possible true anomaly of the desired orbit

	if (opt->vesseltype == 0)
	{
		f_T = SPS_THRUST;
		isp = SPS_ISP;
	}
	else
	{
		f_T = DPS_THRUST;
		isp = DPS_ISP;
	}

	for (int ii = 0;ii < 2;ii++)
	{

		p = OrbMech::RotateVector(u_LPO, -ta[ii], u_node);
		q = unit(crossp(u_LPO, p));

		R_ref[ii] = (p*cos(ta[ii]) + q*sin(ta[ii]))*h*h / mu / (1.0 + e*cos(ta[ii]));
		V_ref[ii] = (-p*sin(ta[ii]) + q*(e + cos(ta[ii])))*mu / h;

		DV[ii] = V_ref[ii] - sv_node.V;
	}

	if (length(DV[1]) < length(DV[0]))
	{
		sol = 1;
	}
	else
	{
		sol = 0;
	}


	if (opt->impulsive == RTCC_NONIMPULSIVE)
	{
		//FiniteBurntimeCompensation(opt->vesseltype, sv_node, attachedMass,)


		OrbMech::impulsive(sv_node.R, sv_node.V, sv_node.MJD, hMoon, f_T, isp, mass, R_ref[sol], V_ref[sol], Llambda, t_slip, R_cut, V_cut, MJD_cut, m_cut);

		OrbMech::oneclickcoast(sv_node.R, sv_node.V, sv_node.MJD, t_slip, R_cor, V_cor, hMoon, hMoon); //Calculate the state vector at the corrected ignition time

		Q_Xx = OrbMech::LVLH_Matrix(R_cor, V_cor);

		dV_LVLH = mul(Q_Xx, Llambda);
		P30TIG = GET_node + t_slip;
	}
	else
	{
		Q_Xx = OrbMech::LVLH_Matrix(sv_node.R, sv_node.V);

		dV_LVLH = mul(Q_Xx, DV[sol]);
		P30TIG = GET_node;
	}

}

void RTCC::LOI2Targeting(LOI2Man *opt, VECTOR3 &dV_LVLH, double &P30TIG)
{
	SV sv0, sv1;
	double GET, LMmass, mass, mu;
	OBJHANDLE hMoon;

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	GET = (sv0.MJD - opt->GETbase)*24.0*3600.0;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	hMoon = oapiGetObjectByName("Moon");
	mu = GGRAV*oapiGetMass(hMoon);

	double a, dt2, LOIGET, v_circ;
	VECTOR3 U_H, U_hor, VA2_apo, DVX;
	MATRIX3 Q_Xx;

	mu = GGRAV*oapiGetMass(hMoon);
	a = oapiGetSize(hMoon) + opt->h_circ + opt->alt;

	dt2 = OrbMech::time_radius_integ(sv0.R, sv0.V, sv0.MJD, a, -1, hMoon, hMoon, sv1.R, sv1.V);
	sv1 = coast(sv0, dt2);
	LOIGET = dt2 + (sv0.MJD - opt->GETbase) * 24 * 60 * 60;

	U_H = unit(crossp(sv1.R, sv1.V));
	U_hor = unit(crossp(U_H, unit(sv1.R)));
	v_circ = sqrt(mu*(2.0 / length(sv1.R) - 1.0 / a));
	VA2_apo = U_hor*v_circ;

	DVX = VA2_apo - sv1.V;

	VECTOR3 Llambda;
	double t_slip;
	SV sv_tig;

	FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DVX, true, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver
	sv_tig = coast(sv1, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	dV_LVLH = mul(Q_Xx, Llambda);

	P30TIG = LOIGET + t_slip;
}

void RTCC::TranslunarInjectionProcessorNodal(TLIManNode *opt, VECTOR3 &dV_LVLH, double &P30TIG, VECTOR3 &Rcut, VECTOR3 &Vcut, double &MJDcut)
{
	double GET;
	OBJHANDLE hMoon;
	SV sv0, sv1;

	hMoon = oapiGetObjectByName("Moon");

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	GET = (sv0.MJD - opt->GETbase)*24.0*3600.0;

	double TIGMJD, PeriMJD, dt1, dt2, TIGguess, dTIG;
	VECTOR3 R_peri, VA1_apo, V_peri, RA2, DVX;
	MATRIX3 Q_Xx;
	OBJHANDLE hEarth = oapiGetObjectByName("Earth");
	int ii;

	PeriMJD = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	TIGguess = opt->TLI_TIG + 6.0*60.0;
	dTIG = -60.0;
	ii = 0;

	double TIGvar[3], dv[3];
	VECTOR3 Llambda, R2_cor, V2_cor, dVLVLH; double t_slip, f_T, isp, boil, m1, m0, TIG, mcut;

	f_T = opt->vessel->GetThrusterMax0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));
	isp = opt->vessel->GetThrusterIsp0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));
	boil = (1.0 - 0.99998193) / 10.0;
	m0 = opt->vessel->GetEmptyMass();

	while (abs(dTIG) > 1.0)
	{
		TIGMJD = TIGguess / 24.0 / 3600.0 + opt->GETbase;
		dt1 = TIGguess - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
		dt2 = (PeriMJD - TIGMJD)*24.0*3600.0;

		sv1 = coast(sv0, dt1);

		double mu_E, mu_M;
		double *MoonPos;
		CELBODY *cMoon;
		MoonPos = new double[12];
		VECTOR3 R_I_star, delta_I_star, delta_I_star_dot, R_P, R_m, V_m;
		MATRIX3 Rot2;
		R_I_star = delta_I_star = delta_I_star_dot = _V(0.0, 0.0, 0.0);

		cMoon = oapiGetCelbodyInterface(hMoon);
		mu_E = GGRAV*oapiGetMass(hEarth);
		mu_M = GGRAV*oapiGetMass(hMoon);

		R_P = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)));
		Rot2 = OrbMech::GetRotationMatrix(hMoon, PeriMJD);

		R_peri = mul(Rot2, R_P);
		//R_peri = unit(mul(Rot, _V(R_peri.x, R_peri.z, R_peri.y)))*(oapiGetSize(hMoon) + opt->h_peri);
		R_peri = unit(_V(R_peri.x, R_peri.z, R_peri.y))*(oapiGetSize(hMoon) + opt->h_peri);

		cMoon->clbkEphemeris(PeriMJD, EPHEM_TRUEPOS | EPHEM_TRUEVEL, MoonPos);
		//R_m = mul(Rot, _V(MoonPos[0], MoonPos[2], MoonPos[1]));
		//V_m = mul(Rot, _V(MoonPos[3], MoonPos[5], MoonPos[4]));
		R_m = _V(MoonPos[0], MoonPos[2], MoonPos[1]);
		V_m = _V(MoonPos[3], MoonPos[5], MoonPos[4]);

		//Initial trajectory, only accurate to 1000 meters
		V_peri = OrbMech::ThreeBodyLambert(PeriMJD, TIGMJD, R_peri, V_m, sv1.R, R_m, V_m, 24.0*oapiGetSize(hEarth), mu_E, mu_M, R_I_star, delta_I_star, delta_I_star_dot);
		//OrbMech::oneclickcoast(R_peri, V_peri, PeriMJD, -dt2, RA2, VA1_apo, hMoon, hEarth);

		//Precise backwards targeting
		V_peri = OrbMech::Vinti(R_peri, V_peri, sv1.R, PeriMJD, -dt2, 0, false, hMoon, hMoon, hEarth, V_peri);

		//Finding the new ignition state
		OrbMech::oneclickcoast(R_peri, V_peri, PeriMJD, (sv1.MJD - PeriMJD)*24.0*3600.0, RA2, VA1_apo, hMoon, hEarth);

		//Final pass through the targeting, forwards and precise
		//VA1_apo = OrbMech::Vinti(RA1, VA1, R_peri, TIGMJD, dt2, 0, true, hEarth, hEarth, hMoon, VA1_apo);

		DVX = VA1_apo - sv1.V;

		if (ii < 2)
		{
			TIGvar[ii + 1] = (TIGMJD - sv0.MJD)*24.0*3600.0;
			dv[ii + 1] = length(DVX);
		}
		else
		{
			dv[0] = dv[1];
			dv[1] = dv[2];
			dv[2] = length(DVX);
			TIGvar[0] = TIGvar[1];
			TIGvar[1] = TIGvar[2];
			TIGvar[2] = (TIGMJD - sv0.MJD)*24.0*3600.0;

			dTIG = OrbMech::quadratic(TIGvar, dv) + (sv0.MJD - TIGMJD)*24.0*3600.0;

			if (abs(dTIG) > 10.0*60.0)
			{
				dTIG = OrbMech::sign(dTIG)*10.0*60.0;
			}
		}

		TIGguess += dTIG;
		ii++;
	}

	m1 = (sv0.mass - m0)*exp(-boil*dt1);

	OrbMech::impulsive(sv1.R, sv1.V, TIGMJD, hEarth, f_T, isp, m0 + m1, DVX, Llambda, t_slip, Rcut, Vcut, MJDcut, mcut); //Calculate the impulsive equivalent of the maneuver

	OrbMech::oneclickcoast(sv1.R, sv1.V, TIGMJD, t_slip, R2_cor, V2_cor, hEarth, hEarth);//Calculate the state vector at the corrected ignition time

	Q_Xx = OrbMech::LVLH_Matrix(R2_cor, V2_cor);

	dVLVLH = mul(Q_Xx, Llambda);
	TIG = TIGguess + t_slip;

	P30TIG = TIG;
	dV_LVLH = dVLVLH;
}

void RTCC::TranslunarInjectionProcessorFreeReturn(TLIManFR *opt, TLMCCResults *res, VECTOR3 &Rcut, VECTOR3 &Vcut, double &MJDcut)
{
	//state vector "now"
	SV sv0;
	//state vector at initial guess for ignition
	SV sv1;
	//state vector at current ignition time
	SV sv2;
	//state vector at periapsis
	SV sv_peri;
	//state vector at reentry
	SV sv_reentry;
	MATRIX3 Q_Xx;
	VECTOR3 DV, DV_guess, Llambda, R2_cor, V2_cor, dVLVLH, var_conv;
	double PeriMJDguess, dTIG, TIGguess, dt0, dt1, TIGMJD, f_T, isp, boil, m0, m1, t_slip, mcut, TIG, mu_E;
	double TIGvar[3], dv[3];
	int ii;
	OBJHANDLE hEarth;

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	dTIG = -60.0;
	ii = 0;
	hEarth = oapiGetObjectByName("Earth");
	mu_E = GGRAV*oapiGetMass(hEarth);

	PeriMJDguess = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;
	TIGguess = opt->TLI_TIG + 6.0*60.0;
	dt0 = TIGguess - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	sv1 = coast(sv0, dt0);

	TLIFirstGuessConic(sv1, opt->lat, opt->h_peri, PeriMJDguess, DV_guess, var_conv);
	IntegratedTLMC(sv1, opt->lat, opt->h_peri, 0.0, PeriMJDguess, var_conv, DV_guess, var_conv, sv_peri);

	while (abs(dTIG) > 1.0)
	{
		TIGMJD = TIGguess / 24.0 / 3600.0 + opt->GETbase;
		dt1 = (TIGMJD - sv1.MJD) * 24.0 * 60.0 * 60.0;
		sv2 = coast(sv1, dt1);

		TLIFlyby(sv2, opt->lat, opt->h_peri, sv_peri, DV, sv_peri, sv_reentry);

		if (ii < 2)
		{
			TIGvar[ii + 1] = (TIGMJD - sv0.MJD)*24.0*3600.0;
			dv[ii + 1] = length(DV);
		}
		else
		{
			dv[0] = dv[1];
			dv[1] = dv[2];
			dv[2] = length(DV);
			TIGvar[0] = TIGvar[1];
			TIGvar[1] = TIGvar[2];
			TIGvar[2] = (TIGMJD - sv0.MJD)*24.0*3600.0;

			dTIG = OrbMech::quadratic(TIGvar, dv) + (sv0.MJD - TIGMJD)*24.0*3600.0;

			if (abs(dTIG) > 10.0*60.0)
			{
				dTIG = OrbMech::sign(dTIG)*10.0*60.0;
			}
		}

		TIGguess += dTIG;
		ii++;
	}

	f_T = opt->vessel->GetThrusterMax0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));
	isp = opt->vessel->GetThrusterIsp0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));
	boil = (1.0 - 0.99998193) / 10.0;
	m0 = opt->vessel->GetEmptyMass();

	m1 = (sv0.mass - m0)*exp(-boil*dt1);

	OrbMech::impulsive(sv2.R, sv2.V, TIGMJD, hEarth, f_T, isp, m0 + m1, DV, Llambda, t_slip, Rcut, Vcut, MJDcut, mcut); //Calculate the impulsive equivalent of the maneuver

	OrbMech::oneclickcoast(sv2.R, sv2.V, TIGMJD, t_slip, R2_cor, V2_cor, hEarth, hEarth);//Calculate the state vector at the corrected ignition time

	Q_Xx = OrbMech::LVLH_Matrix(R2_cor, V2_cor);

	dVLVLH = mul(Q_Xx, Llambda);
	TIG = TIGguess + t_slip;

	res->P30TIG = TIG;
	res->dV_LVLH = dVLVLH;
	res->PericynthionGET = (sv_peri.MJD - opt->GETbase)*24.0*3600.0;
	res->EntryInterfaceGET = (sv_reentry.MJD - opt->GETbase)*24.0*3600.0;

	//Calculate Reentry Parameters
	MATRIX3 Rot;
	VECTOR3 R_geo, V_geo, H_geo;

	Rot = OrbMech::GetRotationMatrix(hEarth, sv_reentry.MJD);
	R_geo = rhtmul(Rot, sv_reentry.R);
	V_geo = rhtmul(Rot, sv_reentry.V);
	H_geo = crossp(R_geo, V_geo);
	res->FRInclination = acos(H_geo.z / length(H_geo));

	double rtgo, vio, ret, r_EI, dt_EI;
	VECTOR3 R_EI, V_EI;

	r_EI = oapiGetSize(hEarth) + 400000.0*0.3048;
	dt_EI = OrbMech::time_radius(sv_reentry.R, -sv_reentry.V, r_EI, 1.0, mu_E);
	OrbMech::oneclickcoast(sv_reentry.R, sv_reentry.V, sv_reentry.MJD, -dt_EI, R_EI, V_EI, hEarth, hEarth);

	EntryCalculations::Reentry(R_EI, V_EI, sv_reentry.MJD - dt_EI / 24.0 / 3600.0, true, res->SplashdownLat, res->SplashdownLng, rtgo, vio, ret);
}

void RTCC::TranslunarMidcourseCorrectionTargetingNodal(MCCNodeMan *opt, VECTOR3 &dV_LVLH, double &P30TIG)
{
	MATRIX3 Q_Xx, Rot, M_EMP;
	VECTOR3 R_selen, R, R_EMP, DV1, DV2, var_conv1, var_conv2, DV3, Llambda;
	double LMmass, mass, dt1, dt2, NodeMJD, t_slip;
	double lat_EMP, lng_EMP;
	OBJHANDLE hMoon;
	SV sv0, sv1, sv_tig, sv_peri2;

	hMoon = oapiGetObjectByName("Moon");

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->NodeGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);

	NodeMJD = opt->NodeGET / 24.0 / 3600.0 + opt->GETbase;

	//Convert selenographic coordinates to EMP latitude and longitude

	R_selen = OrbMech::r_from_latlong(opt->lat, opt->lng);
	Rot = OrbMech::GetRotationMatrix(hMoon, NodeMJD);
	R = rhmul(Rot, R_selen);
	M_EMP = OrbMech::EMPMatrix(NodeMJD);
	R_EMP = mul(M_EMP, R);
	OrbMech::latlong_from_r(R_EMP, lat_EMP, lng_EMP);

	if (lng_EMP < 0.0)
	{
		lng_EMP += PI2;
	}

	//Step 1: Converge TLMC to the desired end conditions in order to provide good first guesses.
	TLMCFirstGuessConic(sv1, lat_EMP, opt->h_node, 0.0, NodeMJD, DV1, var_conv1);

	//Step 2: Converge integrated TLMC
	IntegratedTLMC(sv1, lat_EMP, opt->h_node, 0.0, NodeMJD, var_conv1, DV2, var_conv2, sv_peri2);

	//Step 3: Converge integrated XYZ and T trajectory
	TLMCIntegratedXYZT(sv1, lat_EMP, lng_EMP, opt->h_node, NodeMJD, DV2, DV3);

	FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DV3, true, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver
	sv_tig = coast(sv1, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	dV_LVLH = mul(Q_Xx, Llambda);
	P30TIG = opt->MCCGET + t_slip;
}

bool RTCC::TranslunarMidcourseCorrectionTargetingFreeReturn(MCCFRMan *opt, TLMCCResults *res)
{
	SV sv0, sv1, sv_peri2, sv_reentry2, sv_peri3, sv_reentry3, sv_peri, sv_reentry, sv_peri5, sv_node3, sv_node;
	double mass, LMmass, dt1, dt2, PeriMJDguess, mu_E, lat_EMP3;
	double r_peri3, v_peri3, lng_peri3, lat_peri3, fpav_peri3, azi_peri3;
	OBJHANDLE hMoon, hEarth;
	VECTOR3 Llambda, DV, DV1, var_conv1, DV2, DV3, R_EMP3, V_EMP3, DV5, var_converged5;
	MATRIX3 Q_Xx;
	double t_slip;
	SV sv_tig;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	mu_E = GGRAV*oapiGetMass(hEarth);

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->PeriGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);
	PeriMJDguess = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	//Step 1: Converge conic TLMC
	TLMCFirstGuessConic(sv1, opt->lat, opt->h_peri, 0.0, PeriMJDguess, DV1, var_conv1);

	//Step 2: Converge conic, free-return trajectory
	TLMCFlybyConic(sv1, opt->lat, opt->h_peri, DV1, DV2, sv_peri2, sv_reentry2);

	//Step 3: Converge conic full mission
	if (!TLMCConic_BAP_FR_LPO(opt, sv1, opt->lat, opt->h_peri, DV2, DV3, sv_peri3, sv_node3, sv_reentry3, lat_EMP3))
	{
		return false;
	}

	OrbMech::EclToEMP(sv_peri3.R, sv_peri3.V, sv_peri3.MJD, R_EMP3, V_EMP3);
	OrbMech::rv_from_adbar(R_EMP3, V_EMP3, r_peri3, v_peri3, lng_peri3, lat_peri3, fpav_peri3, azi_peri3);

	//Step 5: Converge integrated TLMC to new targets
	IntegratedTLMC(sv1, lat_EMP3, opt->h_peri, 0.0, sv_peri3.MJD, _V(v_peri3, azi_peri3, lng_peri3), DV5, var_converged5, sv_peri5);

	//Step 6: Converge integrated full mission
	if (!TLMC_BAP_FR_LPO(opt, sv1, lat_EMP3, opt->h_peri, DV5, DV, sv_peri, sv_node, sv_reentry, res->EMPLatitude))
	{
		return false;
	}

	res->PericynthionGET = (sv_peri.MJD - opt->GETbase)*24.0*3600.0;
	res->EntryInterfaceGET = (sv_reentry.MJD - opt->GETbase)*24.0*3600.0;

	FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DV, true, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver
	sv_tig = coast(sv1, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	res->dV_LVLH = mul(Q_Xx, Llambda);
	res->P30TIG = opt->MCCGET + t_slip;

	//Calculate nodal target
	OrbMech::latlong_from_J2000(sv_node.R, sv_node.MJD, sv_node.gravref, res->NodeLat, res->NodeLng);
	res->NodeAlt = length(sv_node.R) - oapiGetSize(hMoon);
	res->NodeGET = (sv_node.MJD - opt->GETbase)*24.0*3600.0;

	//Calculate Reentry Parameters
	MATRIX3 Rot_reentry;
	VECTOR3 R_geo, V_geo, H_geo;

	Rot_reentry = OrbMech::GetRotationMatrix(hEarth, sv_reentry.MJD);
	R_geo = rhtmul(Rot_reentry, sv_reentry.R);
	V_geo = rhtmul(Rot_reentry, sv_reentry.V);
	H_geo = crossp(R_geo, V_geo);
	res->FRInclination = acos(H_geo.z / length(H_geo));

	double rtgo, vio, ret, r_EI, dt_EI;
	VECTOR3 R_EI, V_EI;

	r_EI = oapiGetSize(hEarth) + 400000.0*0.3048;
	dt_EI = OrbMech::time_radius(sv_reentry.R, -sv_reentry.V, r_EI, 1.0, mu_E);
	OrbMech::oneclickcoast(sv_reentry.R, sv_reentry.V, sv_reentry.MJD, -dt_EI, R_EI, V_EI, hEarth, hEarth);

	EntryCalculations::Reentry(R_EI, V_EI, sv_reentry.MJD - dt_EI / 24.0 / 3600.0, true, res->SplashdownLat, res->SplashdownLng, rtgo, vio, ret);

	return true;
}

bool RTCC::TranslunarMidcourseCorrectionTargetingNonFreeReturn(MCCNFRMan *opt, TLMCCResults *res)
{
	SV sv0, sv1, sv_peri2, sv_node4, sv_node2;
	double mass, LMmass, dt1, dt2, PeriMJD, lat_EMP2, r_nd2, v_nd2, fpa_nd2, azi_nd2;
	double h_nd2, lat_nd2, lng_nd2, lat_nd4, lng_nd4, h_nd4;
	OBJHANDLE hMoon;
	VECTOR3 Llambda, DV1, var_conv1, DV2, DV4, R_EMP2, V_EMP2, var_conv4, DV5, R_EMP4, V_EMP4;
	MATRIX3 Q_Xx;
	double t_slip;
	SV sv_tig;

	hMoon = oapiGetObjectByName("Moon");

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->PeriGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);

	PeriMJD = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	//Step 1: Converge conic TLMC
	TLMCFirstGuessConic(sv1, opt->lat, opt->h_peri, 0.0, PeriMJD, DV1, var_conv1);

	//Step 2: Converge a conic non-free select full mission
	if (!TLMCConic_BAP_NFR_LPO(opt, sv1, opt->lat, opt->h_peri, PeriMJD, DV1, DV2, sv_peri2, sv_node2, lat_EMP2))
	{
		return false;
	}

	OrbMech::EclToEMP(sv_node2.R, sv_node2.V, sv_node2.MJD, R_EMP2, V_EMP2);

	OrbMech::rv_from_adbar(R_EMP2, V_EMP2, r_nd2, v_nd2, lng_nd2, lat_nd2, fpa_nd2, azi_nd2);
	h_nd2 = r_nd2 - oapiGetSize(hMoon);

	//Step 4: Converge integrated TLMC
	IntegratedTLMC(sv1, lat_nd2, h_nd2, fpa_nd2 - PI05, sv_node2.MJD, _V(v_nd2, azi_nd2, lng_nd2), DV4, var_conv4, sv_node4);

	OrbMech::EclToEMP(sv_node4.R, sv_node4.V, sv_node4.MJD, R_EMP4, V_EMP4);
	OrbMech::latlong_from_r(R_EMP4, lat_nd4, lng_nd4);

	h_nd4 = length(R_EMP4) - oapiGetSize(hMoon);

	//Step 5: Converge precision trajectory to optimized LOI conditions
	TLMCIntegratedXYZT(sv1, lat_nd4, lng_nd4, h_nd4, sv_node4.MJD, DV4, DV5);

	//Calculate nodal target
	OrbMech::latlong_from_J2000(sv_node4.R, sv_node4.MJD, sv_node4.gravref, res->NodeLat, res->NodeLng);
	res->NodeAlt = length(sv_node4.R) - oapiGetSize(hMoon);
	res->NodeGET = (sv_node4.MJD - opt->GETbase)*24.0*3600.0;

	FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DV5, true, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver
	sv_tig = coast(sv1, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	res->dV_LVLH = mul(Q_Xx, Llambda);
	res->P30TIG = opt->MCCGET + t_slip;

	res->EMPLatitude = lat_nd4;

	return true;
}

bool RTCC::TranslunarMidcourseCorrectionTargetingFlyby(MCCFlybyMan *opt, TLMCCResults *res)
{
	SV sv0, sv1, sv_peri2, sv_peri3, sv_reentry3;
	double mass, LMmass, dt1, dt2, PeriMJDguess, mu_E;
	OBJHANDLE hMoon, hEarth;
	VECTOR3 Llambda, DV1, var_conv1, DV2, var_conv2, DV3;
	MATRIX3 Q_Xx;
	double t_slip;
	SV sv_tig;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->PeriGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);

	PeriMJDguess = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	//Step 1: Converge conic TLMC
	TLMCFirstGuessConic(sv1, opt->lat, opt->h_peri, 0.0, PeriMJDguess, DV1, var_conv1);

	//Step 2: Converge integrated TLMC
	IntegratedTLMC(sv1, opt->lat, opt->h_peri, 0.0, PeriMJDguess, var_conv1, DV2, var_conv2, sv_peri2);

	//Step 3: Converge integrated free-return trajectory
	if (!TLMCFlyby(sv1, opt->lat, opt->h_peri, DV2, DV3, sv_peri3, sv_reentry3))
	{
		return false;
	}

	res->PericynthionGET = (sv_peri3.MJD - opt->GETbase)*24.0*3600.0;
	res->EntryInterfaceGET = (sv_reentry3.MJD - opt->GETbase)*24.0*3600.0;

	FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DV3, true, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver
	sv_tig = coast(sv1, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	res->dV_LVLH = mul(Q_Xx, Llambda);
	res->P30TIG = opt->MCCGET + t_slip;

	//Calculate Reentry Parameters
	MATRIX3 Rot;
	VECTOR3 R_geo, V_geo, H_geo;

	Rot = OrbMech::GetRotationMatrix(hEarth, sv_reentry3.MJD);
	R_geo = rhtmul(Rot, sv_reentry3.R);
	V_geo = rhtmul(Rot, sv_reentry3.V);
	H_geo = crossp(R_geo, V_geo);
	res->FRInclination = acos(H_geo.z / length(H_geo));

	double rtgo, vio, ret, r_EI, dt_EI;
	VECTOR3 R_EI, V_EI;

	r_EI = oapiGetSize(hEarth) + 400000.0*0.3048;
	dt_EI = OrbMech::time_radius(sv_reentry3.R, -sv_reentry3.V, r_EI, 1.0, mu_E);
	OrbMech::oneclickcoast(sv_reentry3.R, sv_reentry3.V, sv_reentry3.MJD, -dt_EI, R_EI, V_EI, hEarth, hEarth);

	EntryCalculations::Reentry(R_EI, V_EI, sv_reentry3.MJD - dt_EI / 24.0 / 3600.0, true, res->SplashdownLat, res->SplashdownLng, rtgo, vio, ret);

	return true;
}

bool RTCC::TranslunarMidcourseCorrectionTargetingSPSLunarFlyby(MCCSPSLunarFlybyMan *opt, TLMCCResults *res, int &step)
{
	SV sv0, sv1, sv_peri, sv_reentry;
	double mass, LMmass, dt1, dt2, PeriMJDguess, mu_E;
	OBJHANDLE hMoon, hEarth;
	VECTOR3 Llambda, DV, var_converged;
	MATRIX3 Q_Xx;
	double t_slip;
	SV sv_tig;

	step = 0;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->PeriGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);

	PeriMJDguess = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	SV sv_p1, sv_p2, sv_r2, sv_p3, sv_r3, sv_p3B, S1, S2C, sv_p4, sv_r4, sv_p5, sv_r5, S_apo, sv_p6, sv_r6;
	VECTOR3 DV1, DV2, DV3, DV3B, DV4, DV5, DV6_guess, DV6, DV7_guess;
	double h_peri_fg, LunarRightAscension, LunarDeclination, LunarDistance, InclFRGuess, lat_EMP_SPLIT, lat_EMP_Target3, lat_EMP_Target6, lattemp;
	double lat_pc_max, lat_pc_min;

	h_peri_fg = 80.0*1852.0;

	//TBD: the 80NM first guess for the perilune height could be a manual input. 2000+ NM flybys don't work very well with this initial guess.
	//So for now limit the difference between the heights to 1000NM.
	if (opt->h_peri - h_peri_fg > 1000.0*1852.0)
	{
		h_peri_fg += opt->h_peri - h_peri_fg - 1000.0*1852.0;
	}

	step++;

	//Step 1: Conic TLMC First Guess
	TLMCFirstGuessConic(sv1, opt->lat, h_peri_fg, 0.0, PeriMJDguess, DV1, var_converged);
	step++;

	//Step 2: Conic Flyby to minimum inclination flyby

	OrbMech::GetLunarEquatorialCoordinates(PeriMJDguess, LunarRightAscension, LunarDeclination, LunarDistance);

	//User input constraint. Inclination has to be lunar declination at pericynthion passage plus 5 degrees
	if (opt->FRInclination < abs(LunarDeclination) + 5.0*RAD)
	{
		return false;
	}

	InclFRGuess = abs(LunarDeclination) + 2.0*RAD;

	if (!TLMCConicFlybyToInclinationSubprocessor(sv1, h_peri_fg, InclFRGuess, DV1, DV2, sv_p2, sv_r2, lat_EMP_SPLIT))
	{
		return false;
	}

	step++;

	//Step 3: Execute conic flyby to the biased latitude of perilune
	if (opt->AscendingNode)
	{
		lat_EMP_Target3 = lat_EMP_SPLIT + 2.0*RAD;
	}
	else
	{
		lat_EMP_Target3 = lat_EMP_SPLIT - 2.0*RAD;
	}

	if (!TLMCFlybyConic(sv1, lat_EMP_Target3, h_peri_fg, DV2, DV3, sv_p3, sv_r3))
	{
		return false;
	}

	S1 = sv1;
	S2C = sv1;
	S2C.V += DV3;

	//Step 3B: Integrated first guess with new latitude
	TLMCFirstGuess(sv1, lat_EMP_Target3, h_peri_fg, sv_p3.MJD, DV3B, sv_p3B);
	step++;

	//Step 4: Execute integrated flyby to the same latitude of perilune as step 3
	if (!TLMCFlyby(sv1, lat_EMP_Target3, h_peri_fg, DV3B, DV4, sv_p4, sv_r4))
	{
		return false;
	}

	step++;

	//Step 5: Execute integrated flyby to same inclination of free return and height as step 2
	if (!TLMCIntegratedFlybyToInclinationSubprocessor(sv1, h_peri_fg, InclFRGuess, DV4, DV5, sv_p5, sv_r5, lattemp))
	{
		return false;
	}

	S_apo = S2C;
	S_apo.V -= DV5;
	DV6_guess = S2C.V - S_apo.V;

	if (opt->AscendingNode)
	{
		lat_pc_max = 20.0*RAD;
		lat_pc_min = lat_EMP_SPLIT - 0.5*RAD;
	}
	else
	{
		lat_pc_max = lat_EMP_SPLIT + 0.5*RAD;
		lat_pc_min = -20.0*RAD;
	}

	step++;

	//Step 6: Converge a conic free return (select mode using S_apo as the state vector)
	if (!TLMCConicFlybyToInclinationSubprocessor(S_apo, opt->h_peri, opt->FRInclination, DV6_guess, DV6, sv_p6, sv_r6, lat_EMP_Target6))
	{
		return false;
	}

	step++;

	S2C = S_apo;
	S2C.V += DV6;
	DV7_guess = S2C.V - S_apo.V;

	//Step 7: Converge integrated flyby to specified inclination of free return and height using state S1
	if (!TLMCIntegratedFlybyToInclinationSubprocessor(sv1, opt->h_peri, opt->FRInclination, DV7_guess, DV, sv_peri, sv_reentry, res->EMPLatitude))
	{
		return false;
	}

	step++;

	FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DV, true, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver
	sv_tig = coast(sv1, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	res->dV_LVLH = mul(Q_Xx, Llambda);
	res->P30TIG = opt->MCCGET + t_slip;
	res->PericynthionGET = (sv_peri.MJD - opt->GETbase)*24.0*3600.0;
	res->EntryInterfaceGET = (sv_reentry.MJD - opt->GETbase)*24.0*3600.0;

	//Calculate Reentry Parameters
	MATRIX3 Rot;
	VECTOR3 R_geo, V_geo, H_geo;

	Rot = OrbMech::GetRotationMatrix(hEarth, sv_reentry.MJD);
	R_geo = rhtmul(Rot, sv_reentry.R);
	V_geo = rhtmul(Rot, sv_reentry.V);
	H_geo = crossp(R_geo, V_geo);
	res->FRInclination = acos(H_geo.z / length(H_geo));

	double rtgo, vio, ret, r_EI, dt_EI;
	VECTOR3 R_EI, V_EI;

	r_EI = oapiGetSize(hEarth) + 400000.0*0.3048;
	dt_EI = OrbMech::time_radius(sv_reentry.R, -sv_reentry.V, r_EI, 1.0, mu_E);
	OrbMech::oneclickcoast(sv_reentry.R, sv_reentry.V, sv_reentry.MJD, -dt_EI, R_EI, V_EI, hEarth, hEarth);

	EntryCalculations::Reentry(R_EI, V_EI, sv_reentry.MJD - dt_EI / 24.0 / 3600.0, true, res->SplashdownLat, res->SplashdownLng, rtgo, vio, ret);

	return true;
}

void RTCC::GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG)
{
	double TOA;
	GeneralManeuverProcessor(opt, dV_LVLH, P30TIG, TOA);
}

void RTCC::GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG, double &TOA)
{
	SV sv0, sv1, sv_tig_imp;
	VECTOR3 DV;
	double dt1, LMmass, mass, R_E, mu;

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	dt1 = opt->TIG_GET + (opt->GETbase - sv0.MJD)*24.0*3600.0;
	sv1 = coast(sv0, dt1);

	//Body radius
	if (sv1.gravref == oapiGetObjectByName("Earth"))
	{
		if (opt->AltRef == 0)
		{
			R_E = 6371.0e3;
		}
		else
		{
			R_E = oapiGetSize(sv1.gravref);
		}
	}
	else
	{
		if (opt->AltRef == 0)
		{
			R_E = oapiGetSize(sv1.gravref);
		}
		else
		{
			R_E = oapiGetSize(sv1.gravref) + opt->LSAlt;
		}
	}

	mu = GGRAV*oapiGetMass(sv1.gravref);

	if (opt->type == 0)
	{
		sv_tig_imp = sv1;
		OrbitAdjustCalc(sv_tig_imp, R_E + opt->h_apo, R_E + opt->h_peri, opt->inc, DV);
	}
	else if (opt->type == 1 || opt->type == 2 || opt->type == 3 || opt->type == 5)
	{
		dt1 = opt->TIG_GET + (opt->GETbase - sv0.MJD)*24.0*3600.0;
		sv1 = coast(sv0, dt1);
		sv_tig_imp = sv1;

		if (opt->type == 1)
		{
			DV = OrbMech::AdjustApoapsis(sv_tig_imp.R, sv_tig_imp.V, mu, R_E + opt->h_apo);
		}
		else if (opt->type == 2)
		{
			DV = OrbMech::AdjustPeriapsis(sv_tig_imp.R, sv_tig_imp.V, mu, R_E + opt->h_peri);
		}
		else if (opt->type == 3)
		{
			DV = OrbMech::CircularOrbitDV(sv_tig_imp.R, sv_tig_imp.V, mu);
		}
		else if (opt->type == 5)
		{
			VECTOR3 DV2, DV3;
			DV2 = OrbMech::RotateVelocityVector(sv_tig_imp.R, sv_tig_imp.V, opt->rot_ang);
			DV3 = OrbMech::AdjustApoapsis(sv_tig_imp.R, sv_tig_imp.V + DV2, mu, R_E + opt->h_apo);
			DV = DV2 + DV3;
		}
	}
	else if (opt->type == 4)
	{
		VECTOR3 U_H, U_hor, V_apo;
		double a, dt2, dt21, dt22, v_circ;

		dt1 = opt->TIG_GET + (opt->GETbase - sv0.MJD)*24.0*3600.0;
		sv1 = coast(sv0, dt1);

		a = opt->h_peri + R_E;

		dt21 = OrbMech::time_radius_integ(sv1.R, sv1.V, sv1.MJD, a, -1.0, sv1.gravref, sv1.gravref);
		dt22 = OrbMech::time_radius_integ(sv1.R, sv1.V, sv1.MJD, a, 1.0, sv1.gravref, sv1.gravref);

		if (dt21 < dt22)
		{
			dt2 = dt21;
		}
		else
		{
			dt2 = dt22;
		}

		sv_tig_imp = coast(sv1, dt2);

		U_H = unit(crossp(sv_tig_imp.R, sv_tig_imp.V));
		U_hor = unit(crossp(U_H, unit(sv_tig_imp.R)));
		v_circ = sqrt(mu*(2.0 / length(sv_tig_imp.R) - 1.0 / a));
		V_apo = U_hor*v_circ;

		DV = V_apo - sv_tig_imp.V;
	}
	else if (opt->type == 6)
	{
		double dt2, dt3, dv, dTIG;
		SV sv2;

		dt2 = OrbMech::timetoperi_integ(sv1.R, sv1.V,sv1.MJD, sv1.gravref, sv1.gravref);
		sv2 = coast(sv1, dt2);

		OrbMech::RotatePerigeeToSpecifiedLongitude(sv2.R, sv2.V, sv2.MJD, sv2.gravref, opt->lng, opt->N, mu, dv, dTIG, dt3);

		TOA = (sv2.MJD - opt->GETbase)*24.0*3600.0 + dTIG + dt3;

		sv_tig_imp = coast(sv2, dTIG);

		DV = unit(sv_tig_imp.R)*dv;
	}

	MATRIX3 Q_Xx;

	if (opt->impulsive == RTCC_IMPULSIVE || length(DV) < 0.001)
	{
		Q_Xx = OrbMech::LVLH_Matrix(sv_tig_imp.R, sv_tig_imp.V);
		dV_LVLH = mul(Q_Xx, DV);
		P30TIG = (sv_tig_imp.MJD - opt->GETbase)*24.0*3600.0;
	}
	else
	{
		VECTOR3 Llambda;
		double t_slip;
		bool main;
		SV sv_tig_cor;

		if (opt->impulsive == RTCC_NONIMPULSIVE)
		{
			main = true;
		}
		else
		{
			main = false;
		}

		FiniteBurntimeCompensation(opt->vesseltype, sv_tig_imp, LMmass, DV, main, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver

		sv_tig_cor = coast(sv_tig_imp, t_slip);	//Calculate the state vector at the corrected ignition time

		Q_Xx = OrbMech::LVLH_Matrix(sv_tig_cor.R, sv_tig_cor.V);

		dV_LVLH = mul(Q_Xx, Llambda);
		P30TIG = (sv_tig_imp.MJD - opt->GETbase)*24.0*3600.0 + t_slip;
	}
}

void RTCC::OrbitAdjustCalc(SV sv_tig, double r_apo, double r_peri, double inc, VECTOR3 &DV)
{
	double mu, r, phi, lambda, apo, peri, a, e, theta1, theta2, beta1, beta2, ll1, ll2, h, w11, w12, w21, w22, dlambda1, dlambda2, lambda11, lambda12;
	VECTOR3 u, R3, V3, DVX;
	MATRIX3 obli;
	VECTOR3 VXvec[4], DVXvec[4];

	mu = GGRAV*oapiGetMass(sv_tig.gravref);									//Standard gravitational parameter GM

	obli = OrbMech::GetObliquityMatrix(sv_tig.gravref, sv_tig.MJD);

	R3 = rhtmul(obli, sv_tig.R);
	V3 = rhtmul(obli, sv_tig.V);

	//OrbMech::local_to_equ(R2, r, phi, lambda);							//Calculates the radius, latitude and longitude of the maneuver position
	u = unit(R3);
	r = length(R3);
	phi = atan(u.z / sqrt(u.x*u.x + u.y*u.y));
	lambda = atan2(u.y, u.x);

	apo = r_apo;													//calculates the apoapsis radius in the metric system
	peri = r_peri;													//calculates the periapsis radius in the metric system

	if (r > apo)													//If the maneuver radius is higher than the desired apoapsis, then we would get no solution
	{
		apo = r;													//sets the desired apoapsis to the current radius, so that we can calculate a maneuver
	}
	else if (r < peri)												//If the maneuver radius is lower than the desired periapsis, then we would also get no solution
	{
		peri = r;													//sets the desired periapsis to the current radius, so that we can calculate a maneuver
	}

	a = (apo + peri) / 2.0;											//The semi-major axis of the desired orbit
	e = (apo - peri) / (apo + peri);								//The eccentricity of the desired orbit

	theta1 = acos(min(1.0, max(-1.0, (a / r*(1.0 - e*e) - 1.0) / e)));	//The true anomaly of the desired orbit, min and max just to make sure this value isn't out of bounds for acos
	theta2 = PI2 - theta1;											//Calculates the second possible true anomaly of the desired orbit

	beta1 = asin(min(1.0, max(-1.0,cos(inc) / cos(phi))));				//Calculates the azimuth heading of the desired orbit at the current position. TODO: if phi > inc we get no solution
	beta2 = PI - beta1;													//The second possible azimuth heading

	ll1 = atan2(tan(phi), cos(beta1));    //angular distance between the ascending node and the current position (beta1)
	ll2 = atan2(tan(phi), cos(beta2));    //angular distance between the ascending node and the current position (beta2)	

	h = sqrt(r*mu*(1.0 + e*cos(theta1)));    //Specific relative angular momentum (theta1)

	w11 = ll1 - theta1;                     //argument of periapsis (beta1, theta1)
	w12 = ll1 - theta2;                     //argument of periapsis (beta1, theta2)
	w21 = ll2 - theta1;                     //argument of periapsis (beta2, theta1)
	w22 = ll2 - theta2;                     //argument of periapsis (beta2, theta2)

	dlambda1 = atan(tan(beta1)*sin(phi));			//angular distance between the ascending node and the current position measured in the equatorial plane (beta1)
	dlambda2 = atan(tan(beta2)*sin(phi)) + PI;		//angular distance between the ascending node and the current position measured in the equatorial plane (beta2)

	lambda11 = lambda - dlambda1;               //longitude at the ascending node (beta1)
	lambda12 = lambda - dlambda2;               //longitude at the ascending node (beta2)

	VECTOR3 RX1, RX2, RX3, RX4;

	OrbMech::perifocal(h, mu, e, theta1, inc, lambda11, w11, RX1, VXvec[0]); //The required velocity vector for the desired orbit (beta1, theta1)
	OrbMech::perifocal(h, mu, e, theta2, inc, lambda11, w12, RX2, VXvec[1]); //The required velocity vector for the desired orbit (beta1, theta2)
	OrbMech::perifocal(h, mu, e, theta1, inc, lambda12, w21, RX3, VXvec[2]); //The required velocity vector for the desired orbit (beta2, theta1)
	OrbMech::perifocal(h, mu, e, theta2, inc, lambda12, w22, RX4, VXvec[3]); //The required velocity vector for the desired orbit (beta2, theta2)

	DVXvec[0] = VXvec[0] - V3;									//Calculates the DV vectors to achieve the desired orbit
	DVXvec[1] = VXvec[1] - V3;
	DVXvec[2] = VXvec[2] - V3;
	DVXvec[3] = VXvec[3] - V3;

	DVX = DVXvec[0];
	for (int ii = 1;ii < 4;ii++)				//The lowest DV vector is selected. TODO: Let the user choose it.
	{
		if (length(DVXvec[ii]) < length(DVX))
		{
			DVX = DVXvec[ii];
		}
	}

	DVX = _V(DVX.x, DVX.z, DVX.y);	//DVX is in local coordinates, convert back to ecliptic
	DVX = mul(obli, DVX);
	DVX = _V(DVX.x, DVX.z, DVX.y);

	DV = DVX;
}

void RTCC::TLI_PAD(TLIPADOpt* opt, TLIPAD &pad)
{
	MATRIX3 M_R, M, M_RTM;
	VECTOR3 R_A, V_A, R0, V0, UX, UY, UZ, DV_P, DV_C, V_G, U_TD, X_B, IgnAtt, SepATT, ExtATT;
	double boil, SVMJD, m0, mass, dt, t_go, F, v_e, theta_T, dVC;
	SV sv0, sv1, sv2, sv3;

	boil = (1.0 - 0.99998193) / 10.0;

	sv0.gravref = AGCGravityRef(opt->vessel);

	opt->vessel->GetRelativePos(sv0.gravref, R_A);
	opt->vessel->GetRelativeVel(sv0.gravref, V_A);
	SVMJD = oapiGetSimMJD();

	dt = opt->TIG - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;

	R0 = _V(R_A.x, R_A.z, R_A.y);
	V0 = _V(V_A.x, V_A.z, V_A.y);

	m0 = opt->vessel->GetEmptyMass();
	mass = opt->vessel->GetMass();

	sv0.MJD = SVMJD;
	sv0.R = R0;
	sv0.V = V0;
	sv0.mass = mass;

	dt = opt->TIG - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;

	sv1 = coast(sv0, dt);
	sv1.mass = m0 + (mass - m0)*exp(-boil*dt);

	if (opt->uselvdc)
	{
		t_go = opt->TLI - opt->TIG;

		UY = unit(crossp(sv1.V, -sv1.R));
		UZ = unit(sv1.R);
		UX = crossp(UY, UZ);

		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		IgnAtt = OrbMech::CALCGAR(opt->REFSMMAT, M_R);

		sv2 = sv1;
		sv2.R = opt->R_TLI;
		sv2.V = opt->V_TLI;
		sv2.MJD += t_go / 24.0 / 3600.0;

		sv3 = coast(sv2, 900.0);
	}
	else
	{

		sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->TIG, opt->dV_LVLH, sv1, 0);

		sv3 = coast(sv2, 900.0);

		t_go = (sv2.MJD - sv1.MJD)*24.0*3600.0;

		UY = unit(crossp(sv1.V, sv1.R));
		UZ = unit(-sv1.R);
		UX = crossp(UY, UZ);

		v_e = opt->vessel->GetThrusterIsp0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));
		F = opt->vessel->GetThrusterMax0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));

		DV_P = UX*opt->dV_LVLH.x + UZ*opt->dV_LVLH.z;
		if (length(DV_P) != 0.0)
		{
			theta_T = length(crossp(sv1.R, sv1.V))*length(opt->dV_LVLH)*opt->vessel->GetMass() / OrbMech::power(length(sv1.R), 2.0) / F;
			DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
			V_G = DV_C + UY*opt->dV_LVLH.y;
		}
		else
		{
			V_G = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;
		}

		U_TD = unit(V_G);
		X_B = unit(V_G);

		UX = X_B;
		UY = unit(crossp(X_B, -sv1.R));
		UZ = unit(crossp(X_B, crossp(X_B, -sv1.R)));

		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);

		IgnAtt = OrbMech::CALCGAR(opt->REFSMMAT, M_R);
	}

	UY = unit(crossp(sv3.V, sv3.R));
	UZ = unit(-sv3.R);
	UX = crossp(UY, UZ);
	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = OrbMech::CALCSMSC(_V(PI - opt->SeparationAttitude.x, opt->SeparationAttitude.y, opt->SeparationAttitude.z));
	M_RTM = mul(M, M_R);

	SepATT = OrbMech::CALCGAR(opt->REFSMMAT, M_RTM);
	ExtATT = _V(300.0*RAD - SepATT.x, SepATT.y + PI, PI2 - SepATT.z);

	dVC = length(opt->dV_LVLH);

	pad.BurnTime = t_go;
	pad.dVC = dVC / 0.3048;
	pad.IgnATT = _V(OrbMech::imulimit(IgnAtt.x*DEG), OrbMech::imulimit(IgnAtt.y*DEG), OrbMech::imulimit(IgnAtt.z*DEG));
	pad.SepATT = _V(OrbMech::imulimit(SepATT.x*DEG), OrbMech::imulimit(SepATT.y*DEG), OrbMech::imulimit(SepATT.z*DEG));
	pad.ExtATT = _V(OrbMech::imulimit(ExtATT.x*DEG), OrbMech::imulimit(ExtATT.y*DEG), OrbMech::imulimit(ExtATT.z*DEG));
	pad.TB6P = opt->TIG - 577.6;
	pad.VI = length(sv2.V) / 0.3048;
}

bool RTCC::PDI_PAD(PDIPADOpt* opt, AP11PDIPAD &pad)
{
	SV sv0, sv1, sv2, sv_I;
	OBJHANDLE hMoon;
	MATRIX3 C_GP, Rot;
	VECTOR3 U_FDP, dV_TrP, R_LSI, R_LSP, W_I, W_P, R_I, V_I, R_P, V_P, R_G, V_SURFP, V_G, G_P, A_FDP, R_TG, A_G, V_TG, A_TG, C_XGP, C_YGP, C_ZGP;
	double GUIDDURN, AF_TRIM, DELTTRIM, TTT, t_pip, t_2, t_I, t_pipold, PIPTIME, q, dt_I, mu, TTT_P, eps, LEADTIME, dTTT, J_TZG, A_TZG, V_TZG, R_TZG;
	double v_IGG, r_IGXG, r_IGZG, K_X, K_Y, K_V, t_IG, C_R, w_M, FRAC, m, TEM;
	int n1, n2, COUNT_TTT;

	hMoon = oapiGetObjectByName("Moon");
	mu = GGRAV*oapiGetMass(hMoon);

	GUIDDURN = 664.4;
	AF_TRIM = 0.350133;
	DELTTRIM = 26.0;
	n1 = 40;
	n2 = 2;
	COUNT_TTT = 0;
	U_FDP = dV_TrP = _V(0, 0, 0);
	C_GP = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	TTT = 0.0;
	t_pip = opt->t_land;
	dt_I = 1.0;
	FRAC = 43455.0;
	m = 16000.0;

	v_IGG = 1690.256208;
	r_IGXG = -39782.453328;
	r_IGZG = -436655.657;
	K_X = -0.617631;
	K_Y = -2.4770341207e-6;
	K_V = -410.0;
	J_TZG = -0.01882677*0.3048;// / 8.0;
	A_TZG = -54.6264*0.3048 / 6.0;
	V_TZG = -18.72*0.3048 / 3.0;
	R_TG = _V(52.375308, 0.0, -3254.836061);
	R_TZG = R_TG.z;
	V_TG = _V(-105.876, 0.0, -1.04)*0.3048;
	A_TG = _V(0.6241, 0.0, -9.1044)*0.3048;
	LEADTIME = 2.2;
	w_M = 2.66169948e-6;

	sv0 = StateVectorCalc(opt->vessel);

	if (opt->direct)
	{
		sv2 = sv0;
	}
	else
	{
		sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->P30TIG, opt->dV_LVLH, sv0, 0.0);
	}

	t_2 = OrbMech::GETfromMJD(sv2.MJD, opt->GETbase);
	Rot = OrbMech::GetRotationMatrix(hMoon, opt->GETbase + opt->t_land / 24.0 / 3600.0);
	R_LSI = rhmul(Rot, opt->R_LS);
	R_LSP = mul(opt->REFSMMAT, R_LSI);
	t_I = opt->t_land - GUIDDURN;
	W_I = rhmul(Rot, _V(0.0, 0.0, 1.0));
	W_P = mul(opt->REFSMMAT, W_I)*w_M;
	sv_I = coast(sv2, t_I - t_2);
	R_I = sv_I.R;
	V_I = sv_I.V;

	while (abs(dt_I) > 0.08 && n1>0)
	{
		PIPTIME = t_I;
		R_P = mul(opt->REFSMMAT, R_I);
		G_P = -R_P / pow(length(R_P), 3.0)*mu;

		n2 = 2;

		t_pipold = t_pip;
		t_pip = PIPTIME;
		t_2 = PIPTIME;
		R_LSP = unit(R_LSP + crossp(W_P, R_LSP)*(t_pip - t_pipold))*length(opt->R_LS);
		TTT = TTT + t_pip - t_pipold;

		while (n2 > 0)
		{
			V_P = mul(opt->REFSMMAT, V_I) + dV_TrP;

			R_G = mul(C_GP, R_P - R_LSP);
			V_SURFP = V_P - crossp(W_P, R_P);
			V_G = mul(C_GP, V_SURFP);

			COUNT_TTT = 0;
			eps = abs(TTT / 128.0);
			do
			{
				dTTT = -(J_TZG*pow(TTT, 3.0) + 6.0*A_TZG*TTT*TTT + (18.0*V_TZG + 6.0*V_G.z)*TTT + 24.0*(R_TZG - R_G.z)) / (3.0*J_TZG*TTT*TTT + 12.0*A_TZG*TTT + 18.0*V_TZG + 6.0*V_G.z);
				TTT += dTTT;
				COUNT_TTT++;
			} while (abs(dTTT) > eps && COUNT_TTT < 8);

			if (COUNT_TTT == 8)
			{
				return false;
			}

			TTT_P = TTT + LEADTIME;
			A_G = (R_TG - R_G)*(-24.0*TTT_P / pow(TTT, 3.0) + 36.0*TTT_P*TTT_P / pow(TTT, 4.0));
			A_G += V_TG*(-18.0*TTT_P / TTT / TTT + 24.0*TTT_P*TTT_P / pow(TTT, 3.0));
			A_G += V_G*(-6.0*TTT_P / TTT / TTT + 12.0*TTT_P*TTT_P / pow(TTT, 3.0));
			A_G += A_TG*(6.0*TTT_P*TTT_P / TTT / TTT - 6.0*TTT_P / TTT + 1.0);

			A_FDP = tmul(C_GP, A_G) - G_P;
			//s = length(A_FDP);
			TEM = FRAC*FRAC / m / m - A_FDP.x*A_FDP.x - A_FDP.y*A_FDP.y;
			if (TEM < 0.0)
			{
				TEM = 0.0;
			}
			if (sqrt(TEM) + A_FDP.z < 0.0)
			{
				A_FDP.z = -sqrt(TEM);
			}
			U_FDP = A_FDP;

			C_XGP = unit(R_LSP);
			C_YGP = unit(crossp(unit(V_SURFP*TTT / 4.0 + R_LSP - R_P), R_LSP));
			C_ZGP = crossp(C_XGP, C_YGP);
			C_GP = _M(C_XGP.x, C_YGP.x, C_ZGP.x, C_XGP.y, C_YGP.y, C_ZGP.y, C_XGP.z, C_YGP.z, C_ZGP.z);

			dV_TrP = unit(U_FDP)*DELTTRIM*AF_TRIM;

			n2--;
		}

		n1--;
		q = K_X*(R_G.x - r_IGXG) + K_Y*R_G.y*R_G.y + R_G.z - r_IGZG + K_V*(length(V_G) - v_IGG);
		dt_I = -q / (V_G.z + K_X*V_G.x);
		t_I += dt_I;

		if (abs(dt_I) > 0.08)
		{
			OrbMech::rv_from_r0v0(R_I, V_I, dt_I, R_I, V_I, mu);
		}
	}

	if (n1 == 0)
	{
		return false;
	}

	t_IG = t_I - DELTTRIM;
	C_R = dotp(unit(crossp(V_P, R_P)), R_LSP);

	VECTOR3 X_B, UX, UY, UZ, IMUangles, FDAIangles;
	MATRIX3 M, M_R;
	double headsswitch;

	if (opt->HeadsUp)
	{
		headsswitch = -1.0;
	}
	else
	{
		headsswitch = 1.0;
	}

	X_B = tmul(opt->REFSMMAT, unit(U_FDP));
	UX = X_B;
	if (abs(dotp(UX, unit(R_I))) < cos(0.01*RAD))
	{
		UY = unit(crossp(UX, R_I*headsswitch));
	}
	else
	{
		UY = unit(crossp(UX, V_I));
	}
	UZ = unit(crossp(UX, UY));

	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::transpose_matrix(M), M_R));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.Att = _V(OrbMech::imulimit(FDAIangles.x*DEG), OrbMech::imulimit(FDAIangles.y*DEG), OrbMech::imulimit(FDAIangles.z*DEG));
	pad.CR = C_R / 1852.0;
	pad.DEDA231 = length(opt->R_LS) / 0.3048 / 100.0;
	pad.GETI = t_IG;
	pad.t_go = -TTT;

	return true;
}

char* RTCC::AGCExternalDeltaVUpdate(double P30TIG, VECTOR3 dV_LVLH, int DVAddr)
{
	char* str = new char[1000];
	double getign = P30TIG;
	int emem[24];

	emem[0] = 12;
	emem[1] = DVAddr;
	emem[2] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 1);
	emem[3] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 0);
	emem[4] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 1);
	emem[5] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 0);
	emem[6] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 1);
	emem[7] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 0);
	emem[8] = OrbMech::DoubleToBuffer(getign*100.0, 28, 1);
	emem[9] = OrbMech::DoubleToBuffer(getign*100.0, 28, 0);

	str = V71Update(emem, 10);
	return str;
}

char* RTCC::AGCStateVectorUpdate(SV sv, bool csm, double AGCEpoch, double GETbase, bool v66)
{
	OBJHANDLE hMoon = oapiGetGbodyByName("Moon");
	OBJHANDLE hEarth = oapiGetGbodyByName("Earth");

	MATRIX3 Rot = OrbMech::J2000EclToBRCS(AGCEpoch);

	VECTOR3 vel, pos;
	double get;
	int emem[24];
	char* str = new char[1000];

	pos = mul(Rot, sv.R);
	vel = mul(Rot, sv.V)*0.01;
	get = (sv.MJD - GETbase)*24.0*3600.0;

	if (sv.gravref == hMoon) {

		emem[0] = 21;
		emem[1] = 1501;

		//if (g_Data.vessel->GetHandle()==oapiGetFocusObject()) 
		if (csm)
		{
			emem[2] = 2;
		}
		else
		{
			emem[2] = 77775;	// Octal coded decimal
		}

		emem[3] = OrbMech::DoubleToBuffer(pos.x, 27, 1);
		emem[4] = OrbMech::DoubleToBuffer(pos.x, 27, 0);
		emem[5] = OrbMech::DoubleToBuffer(pos.y, 27, 1);
		emem[6] = OrbMech::DoubleToBuffer(pos.y, 27, 0);
		emem[7] = OrbMech::DoubleToBuffer(pos.z, 27, 1);
		emem[8] = OrbMech::DoubleToBuffer(pos.z, 27, 0);
		emem[9] = OrbMech::DoubleToBuffer(vel.x, 5, 1);
		emem[10] = OrbMech::DoubleToBuffer(vel.x, 5, 0);
		emem[11] = OrbMech::DoubleToBuffer(vel.y, 5, 1);
		emem[12] = OrbMech::DoubleToBuffer(vel.y, 5, 0);
		emem[13] = OrbMech::DoubleToBuffer(vel.z, 5, 1);
		emem[14] = OrbMech::DoubleToBuffer(vel.z, 5, 0);
		emem[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		emem[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}
	else if (sv.gravref == hEarth) {

		emem[0] = 21;
		emem[1] = 1501;

		if (csm)
		{
			emem[2] = 1;
		}
		else
		{
			emem[2] = 77776;	// Octal coded decimal
		}

		emem[3] = OrbMech::DoubleToBuffer(pos.x, 29, 1);
		emem[4] = OrbMech::DoubleToBuffer(pos.x, 29, 0);
		emem[5] = OrbMech::DoubleToBuffer(pos.y, 29, 1);
		emem[6] = OrbMech::DoubleToBuffer(pos.y, 29, 0);
		emem[7] = OrbMech::DoubleToBuffer(pos.z, 29, 1);
		emem[8] = OrbMech::DoubleToBuffer(pos.z, 29, 0);
		emem[9] = OrbMech::DoubleToBuffer(vel.x, 7, 1);
		emem[10] = OrbMech::DoubleToBuffer(vel.x, 7, 0);
		emem[11] = OrbMech::DoubleToBuffer(vel.y, 7, 1);
		emem[12] = OrbMech::DoubleToBuffer(vel.y, 7, 0);
		emem[13] = OrbMech::DoubleToBuffer(vel.z, 7, 1);
		emem[14] = OrbMech::DoubleToBuffer(vel.z, 7, 0);
		emem[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		emem[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}
	str = V71Update(emem, 17);
	if (v66 && csm)
	{
		sprintf(str, "%sV66E", str);
	}
	return str;
}

void RTCC::IncrementAGCTime(char *list, double dt)
{
	int emem[2];

	sprintf(list, "V73E");

	emem[0] = OrbMech::DoubleToBuffer(dt, 28, 1);
	emem[1] = OrbMech::DoubleToBuffer(dt, 28, 0);

	sprintf(list, "%s%dE", list, emem[0]);
	sprintf(list, "%s%dE", list, emem[1]);
	sprintf(list, "%sV33E", list);
}

char* RTCC::V71Update(int *emem, int n)
{
	char* list = new char[1000];
	sprintf(list, "V71E%dE", emem[0]);
	for (int i = 1;i < n;i++)
	{
		sprintf(list, "%s%dE", list, emem[i]);
	}
	sprintf(list, "%sV33E", list);
	return list;
}

void RTCC::TLANDUpdate(char *list, double t_land, int tlandaddr)
{
	int emem[5];
	emem[0] = 5;

	emem[1] = tlandaddr;
	emem[3] = tlandaddr + 1;

	emem[2] = OrbMech::DoubleToBuffer(t_land*100.0, 28, 1);
	emem[4] = OrbMech::DoubleToBuffer(t_land*100.0, 28, 0);

	V72Update(emem, 5, list);
}

void RTCC::V72Update(int *emem, int n, char* list)
{
	sprintf(list, "V72E%dE", emem[0]);
	for (int i = 1;i < n;i++)
	{
		sprintf(list, "%s%dE", list, emem[i]);
	}
	sprintf(list, "%sV33E", list);
}

char* RTCC::SunburstAttitudeManeuver(VECTOR3 imuangles)
{
	int emem[3];
	char* list = new char[1000];

	if (imuangles.x > PI)
	{
		imuangles.x -= PI2;
	}
	if (imuangles.y > PI)
	{
		imuangles.y -= PI2;
	}
	if (imuangles.z > PI)
	{
		imuangles.z -= PI2;
	}

	emem[0] = OrbMech::DoubleToBuffer(imuangles.x / PI, 0, 1);
	emem[1] = OrbMech::DoubleToBuffer(imuangles.y / PI, 0, 1);
	emem[2] = OrbMech::DoubleToBuffer(imuangles.z / PI, 0, 1);

	sprintf(list, "V21N1E372E0EV25N1E1631E%dE%dE%dEV25N26E20001E2067E70063EV30ER", emem[0], emem[1], emem[2]);

	return list;
}

char* RTCC::SunburstLMPCommand(int code)
{
	char* list = new char[1000];

	sprintf(list, "V67E%oEV33ER", code);

	return list;
}

char* RTCC::SunburstMassUpdate(double masskg)
{
	int emem[2];
	char* list = new char[1000];

	emem[0] = OrbMech::DoubleToBuffer(masskg, 15, 1);
	emem[1] = OrbMech::DoubleToBuffer(masskg, 15, 0);

	sprintf(list, "V24N1E1320E%dE%dER", emem[0], emem[1]);

	return list;
}

char* RTCC::CMCDesiredREFSMMATUpdate(MATRIX3 REFSMMAT, double AGCEpoch, bool AGCCoordSystem)
{
	MATRIX3 a;
	int emem[24];
	char* str = new char[1000];

	if (AGCCoordSystem)
	{
		a = REFSMMAT;
	}
	else
	{
		a = mul(REFSMMAT, OrbMech::transpose_matrix(OrbMech::J2000EclToBRCS(AGCEpoch)));
	}

	emem[0] = 24;
	emem[1] = 306;
	emem[2] = OrbMech::DoubleToBuffer(a.m11, 1, 1);
	emem[3] = OrbMech::DoubleToBuffer(a.m11, 1, 0);
	emem[4] = OrbMech::DoubleToBuffer(a.m12, 1, 1);
	emem[5] = OrbMech::DoubleToBuffer(a.m12, 1, 0);
	emem[6] = OrbMech::DoubleToBuffer(a.m13, 1, 1);
	emem[7] = OrbMech::DoubleToBuffer(a.m13, 1, 0);
	emem[8] = OrbMech::DoubleToBuffer(a.m21, 1, 1);
	emem[9] = OrbMech::DoubleToBuffer(a.m21, 1, 0);
	emem[10] = OrbMech::DoubleToBuffer(a.m22, 1, 1);
	emem[11] = OrbMech::DoubleToBuffer(a.m22, 1, 0);
	emem[12] = OrbMech::DoubleToBuffer(a.m23, 1, 1);
	emem[13] = OrbMech::DoubleToBuffer(a.m23, 1, 0);
	emem[14] = OrbMech::DoubleToBuffer(a.m31, 1, 1);
	emem[15] = OrbMech::DoubleToBuffer(a.m31, 1, 0);
	emem[16] = OrbMech::DoubleToBuffer(a.m32, 1, 1);
	emem[17] = OrbMech::DoubleToBuffer(a.m32, 1, 0);
	emem[18] = OrbMech::DoubleToBuffer(a.m33, 1, 1);
	emem[19] = OrbMech::DoubleToBuffer(a.m33, 1, 0);

	str = V71Update(emem, 20);
	return str;
}

char* RTCC::AGCREFSMMATUpdate(MATRIX3 REFSMMAT, double AGCEpoch, int offset, bool AGCCoordSystem)
{
	MATRIX3 a;
	int emem[24];
	char* str = new char[1000];

	if (AGCCoordSystem)
	{
		a = REFSMMAT;
	}
	else
	{
		a = mul(REFSMMAT, OrbMech::transpose_matrix(OrbMech::J2000EclToBRCS(AGCEpoch)));
	}

	emem[0] = 24;
	emem[1] = 1735 + offset;
	emem[2] = OrbMech::DoubleToBuffer(a.m11, 1, 1);
	emem[3] = OrbMech::DoubleToBuffer(a.m11, 1, 0);
	emem[4] = OrbMech::DoubleToBuffer(a.m12, 1, 1);
	emem[5] = OrbMech::DoubleToBuffer(a.m12, 1, 0);
	emem[6] = OrbMech::DoubleToBuffer(a.m13, 1, 1);
	emem[7] = OrbMech::DoubleToBuffer(a.m13, 1, 0);
	emem[8] = OrbMech::DoubleToBuffer(a.m21, 1, 1);
	emem[9] = OrbMech::DoubleToBuffer(a.m21, 1, 0);
	emem[10] = OrbMech::DoubleToBuffer(a.m22, 1, 1);
	emem[11] = OrbMech::DoubleToBuffer(a.m22, 1, 0);
	emem[12] = OrbMech::DoubleToBuffer(a.m23, 1, 1);
	emem[13] = OrbMech::DoubleToBuffer(a.m23, 1, 0);
	emem[14] = OrbMech::DoubleToBuffer(a.m31, 1, 1);
	emem[15] = OrbMech::DoubleToBuffer(a.m31, 1, 0);
	emem[16] = OrbMech::DoubleToBuffer(a.m32, 1, 1);
	emem[17] = OrbMech::DoubleToBuffer(a.m32, 1, 0);
	emem[18] = OrbMech::DoubleToBuffer(a.m33, 1, 1);
	emem[19] = OrbMech::DoubleToBuffer(a.m33, 1, 0);

	str = V71Update(emem, 20);
	return str;
}

char* RTCC::CMCRetrofireExternalDeltaVUpdate(double LatSPL, double LngSPL, double P30TIG, VECTOR3 dV_LVLH)
{
	int emem[24];
	char* str = new char[1000];
	double getign = P30TIG;

	emem[0] = 16;
	emem[1] = 3400;
	emem[2] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 1);
	emem[3] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 0);
	emem[4] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 1);
	emem[5] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 0);
	emem[6] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 1);
	emem[7] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 0);
	emem[8] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 1);
	emem[9] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 0);
	emem[10] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 1);
	emem[11] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 0);
	emem[12] = OrbMech::DoubleToBuffer(getign*100.0, 28, 1);
	emem[13] = OrbMech::DoubleToBuffer(getign*100.0, 28, 0);

	str = V71Update(emem, 14);
	return str;
}

char* RTCC::CMCEntryUpdate(double LatSPL, double LngSPL)
{
	int emem[24];
	char* str = new char[1000];

	emem[0] = 06;
	emem[1] = 3400;
	emem[2] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 1);
	emem[3] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 0);
	emem[4] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 1);
	emem[5] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 0);

	str = V71Update(emem, 6);
	return str;
}

void RTCC::AGSStateVectorPAD(AGSSVOpt *opt, AP11AGSSVPAD &pad)
{
	SV sv1;
	VECTOR3 R, V;
	double AGSEpochTime, AGSEpochTime2, AGSEpochTime3, dt, scalR, scalV;

	AGSEpochTime = (opt->sv.MJD - opt->GETbase)*24.0*3600.0;
	AGSEpochTime2 = ceil(AGSEpochTime / 6.0)*6.0;

	dt = AGSEpochTime2 - AGSEpochTime;

	sv1 = coast(opt->sv, dt);

	R = mul(opt->REFSMMAT, sv1.R);
	V = mul(opt->REFSMMAT, sv1.V);

	AGSEpochTime3 = AGSEpochTime2 - opt->AGSbase;

	if (sv1.gravref == oapiGetObjectByName("Earth"))
	{
		scalR = 1000.0;
		scalV = 1.0;
	}
	else
	{
		scalR = 100.0;
		scalV = 0.1;
	}

	if (opt->csm)
	{
		pad.DEDA244 = R.x / 0.3048 / scalR;
		pad.DEDA245 = R.y / 0.3048 / scalR;
		pad.DEDA246 = R.z / 0.3048 / scalR;
		pad.DEDA264 = V.x / 0.3048 / scalV;
		pad.DEDA265 = V.y / 0.3048 / scalV;
		pad.DEDA266 = V.z / 0.3048 / scalV;
		pad.DEDA272 = AGSEpochTime3 / 60.0;
	}
	else
	{
		pad.DEDA240 = R.x / 0.3048 / scalR;
		pad.DEDA241 = R.y / 0.3048 / scalR;
		pad.DEDA242 = R.z / 0.3048 / scalR;
		pad.DEDA260 = V.x / 0.3048 / scalV;
		pad.DEDA261 = V.y / 0.3048 / scalV;
		pad.DEDA262 = V.z / 0.3048 / scalV;
		pad.DEDA254 = AGSEpochTime3 / 60.0;
	}
}

void RTCC::NavCheckPAD(SV sv, AP7NAV &pad, double GETbase, double GET)
{
	double lat, lng, alt, navcheckdt;
	VECTOR3 R1, V1;
	OBJHANDLE outgrav = NULL;
	
	if (GET == 0.0)
	{
		navcheckdt = 30 * 60;
		pad.NavChk[0] = (sv.MJD - GETbase)*24.0*3600.0 + navcheckdt;
	}
	else
	{
		navcheckdt = GET - (sv.MJD - GETbase)*24.0*3600.0;
		pad.NavChk[0] = GET;
	}

	OrbMech::oneclickcoast(sv.R, sv.V, sv.MJD, navcheckdt, R1, V1, sv.gravref, outgrav);
	navcheck(R1, V1, sv.MJD + navcheckdt/24.0/3600.0, outgrav, lat, lng, alt);

	pad.alt[0] = alt / 1852.0;
	pad.lat[0] = lat*DEG;
	pad.lng[0] = lng*DEG;
}

void RTCC::P27PADCalc(P27Opt *opt, double AGCEpoch, P27PAD &pad)
{
	double lat, lng, alt, get, SVMJD;
	VECTOR3 pos, vel;
	OBJHANDLE hMoon = oapiGetGbodyByName("Moon");
	OBJHANDLE hEarth = oapiGetGbodyByName("Earth");
	bool csm = true;
	SV sv, sv1;

	SVMJD = opt->GETbase + opt->SVGET / 24.0 / 3600.0;

	sv = StateVectorCalc(opt->vessel, SVMJD);
	sv1 = coast(sv, opt->navcheckGET - opt->SVGET);

	navcheck(sv1.R, sv1.V, sv1.MJD, sv1.gravref, lat, lng, alt);
	
	sprintf(pad.Purpose[0], "SV");
	pad.GET[0] = opt->SVGET;
	pad.alt = alt / 1852.0;
	pad.lat = lat*DEG;
	pad.lng = lng*DEG;
	pad.NavChk = opt->navcheckGET;
	pad.Index[0] = 21;
	pad.Verb[0] = 71;

	pos = mul(OrbMech::J2000EclToBRCS(AGCEpoch), sv.R);
	vel = mul(OrbMech::J2000EclToBRCS(AGCEpoch), sv.V)*0.01;
	get = opt->SVGET;

	if (sv.gravref == hMoon) {

		pad.Data[0][0] = 1501;

		//if (g_Data.vessel->GetHandle()==oapiGetFocusObject()) 
		if (csm)
		{
			pad.Data[0][1] = 2;
		}
		else
		{
			pad.Data[0][1] = 77775;	// Octal coded decimal
		}

		pad.Data[0][2] = OrbMech::DoubleToBuffer(pos.x, 27, 1);
		pad.Data[0][3] = OrbMech::DoubleToBuffer(pos.x, 27, 0);
		pad.Data[0][4] = OrbMech::DoubleToBuffer(pos.y, 27, 1);
		pad.Data[0][5] = OrbMech::DoubleToBuffer(pos.y, 27, 0);
		pad.Data[0][6] = OrbMech::DoubleToBuffer(pos.z, 27, 1);
		pad.Data[0][7] = OrbMech::DoubleToBuffer(pos.z, 27, 0);
		pad.Data[0][8] = OrbMech::DoubleToBuffer(vel.x, 5, 1);
		pad.Data[0][9] = OrbMech::DoubleToBuffer(vel.x, 5, 0);
		pad.Data[0][10] = OrbMech::DoubleToBuffer(vel.y, 5, 1);
		pad.Data[0][11] = OrbMech::DoubleToBuffer(vel.y, 5, 0);
		pad.Data[0][12] = OrbMech::DoubleToBuffer(vel.z, 5, 1);
		pad.Data[0][13] = OrbMech::DoubleToBuffer(vel.z, 5, 0);
		pad.Data[0][14] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		pad.Data[0][15] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}

	if (sv.gravref == hEarth) {

		pad.Data[0][0] = 1501;

		if (csm)
		{
			pad.Data[0][1] = 1;
		}
		else
		{
			pad.Data[0][1] = 77776;	// Octal coded decimal
		}

		pad.Data[0][2] = OrbMech::DoubleToBuffer(pos.x, 29, 1);
		pad.Data[0][3] = OrbMech::DoubleToBuffer(pos.x, 29, 0);
		pad.Data[0][4] = OrbMech::DoubleToBuffer(pos.y, 29, 1);
		pad.Data[0][5] = OrbMech::DoubleToBuffer(pos.y, 29, 0);
		pad.Data[0][6] = OrbMech::DoubleToBuffer(pos.z, 29, 1);
		pad.Data[0][7] = OrbMech::DoubleToBuffer(pos.z, 29, 0);
		pad.Data[0][8] = OrbMech::DoubleToBuffer(vel.x, 7, 1);
		pad.Data[0][9] = OrbMech::DoubleToBuffer(vel.x, 7, 0);
		pad.Data[0][10] = OrbMech::DoubleToBuffer(vel.y, 7, 1);
		pad.Data[0][11] = OrbMech::DoubleToBuffer(vel.y, 7, 0);
		pad.Data[0][12] = OrbMech::DoubleToBuffer(vel.z, 7, 1);
		pad.Data[0][13] = OrbMech::DoubleToBuffer(vel.z, 7, 0);
		pad.Data[0][14] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		pad.Data[0][15] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}

}

void RTCC::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, RTCC_START_STRING);
	// Booleans
	// Integers
	SAVE_INT("RTCC_REFSMMATType", REFSMMATType);
	// Floats
	SAVE_DOUBLE("RTCC_P30TIG", TimeofIgnition);
	SAVE_DOUBLE("RTCC_SplLat", SplashLatitude);
	SAVE_DOUBLE("RTCC_SplLng", SplashLongitude);
	SAVE_DOUBLE("RTCC_TEI", calcParams.TEI);
	SAVE_DOUBLE("RTCC_EI", calcParams.EI);
	SAVE_DOUBLE("RTCC_TLI", calcParams.TLI);
	SAVE_DOUBLE("RTCC_LOI", calcParams.LOI);
	SAVE_DOUBLE("RTCC_DOI", calcParams.DOI);
	SAVE_DOUBLE("RTCC_PDI", calcParams.PDI);
	SAVE_DOUBLE("RTCC_TLAND", calcParams.TLAND);
	SAVE_DOUBLE("RTCC_Insertion", calcParams.Insertion);
	SAVE_DOUBLE("RTCC_Phasing", calcParams.Phasing);
	SAVE_DOUBLE("RTCC_CSI", calcParams.CSI);
	SAVE_DOUBLE("RTCC_TPI", calcParams.TPI);
	SAVE_DOUBLE("RTCC_alt_node", calcParams.alt_node);
	SAVE_DOUBLE("RTCC_GET_node", calcParams.GET_node);
	SAVE_DOUBLE("RTCC_lat_node", calcParams.lat_node);
	SAVE_DOUBLE("RTCC_lng_node", calcParams.lng_node);
	// Strings
	// Vectors
	SAVE_V3("RTCC_DVLVLH", DeltaV_LVLH);
	SAVE_V3("RTCC_R_TLI", calcParams.R_TLI);
	SAVE_V3("RTCC_V_TLI", calcParams.V_TLI);
	oapiWriteLine(scn, RTCC_END_STRING);
}

// Load State
void RTCC::LoadState(FILEHANDLE scn) {
	char *line;
	int tmp = 0; // Used in boolean type loader

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, RTCC_END_STRING, sizeof(RTCC_END_STRING))) {
			break;
		}
		LOAD_INT("RTCC_REFSMMATType", REFSMMATType);
		LOAD_DOUBLE("RTCC_P30TIG", TimeofIgnition);
		LOAD_DOUBLE("RTCC_SplLat", SplashLatitude);
		LOAD_DOUBLE("RTCC_SplLng", SplashLongitude);
		LOAD_DOUBLE("RTCC_TEI", calcParams.TEI);
		LOAD_DOUBLE("RTCC_EI", calcParams.EI);
		LOAD_DOUBLE("RTCC_TLI", calcParams.TLI);
		LOAD_DOUBLE("RTCC_LOI", calcParams.LOI);
		LOAD_DOUBLE("RTCC_DOI", calcParams.DOI);
		LOAD_DOUBLE("RTCC_PDI", calcParams.PDI);
		LOAD_DOUBLE("RTCC_TLAND", calcParams.TLAND);
		LOAD_DOUBLE("RTCC_Insertion", calcParams.Insertion);
		LOAD_DOUBLE("RTCC_Phasing", calcParams.Phasing);
		LOAD_DOUBLE("RTCC_CSI", calcParams.CSI);
		LOAD_DOUBLE("RTCC_TPI", calcParams.TPI);
		LOAD_DOUBLE("RTCC_alt_node", calcParams.alt_node);
		LOAD_DOUBLE("RTCC_GET_node", calcParams.GET_node);
		LOAD_DOUBLE("RTCC_lat_node", calcParams.lat_node);
		LOAD_DOUBLE("RTCC_lng_node", calcParams.lng_node);
		LOAD_V3("RTCC_DVLVLH", DeltaV_LVLH);
		LOAD_V3("RTCC_R_TLI", calcParams.R_TLI);
		LOAD_V3("RTCC_V_TLI", calcParams.V_TLI);
	}
	return;
}

void RTCC::SetManeuverData(double TIG, VECTOR3 DV)
{
	TimeofIgnition = (TIG - getGETBase())*24.0*3600.0;
	DeltaV_LVLH = DV;
}

int RTCC::SPSRCSDecision(double a, VECTOR3 dV_LVLH)
{
	double t;

	t = length(dV_LVLH) / a;

	if (t > 0.5)
	{
		return RTCC_ENGINETYPE_SPSDPS;
	}
	else
	{
		return RTCC_ENGINETYPE_RCS;
	}
}

SV RTCC::ExecuteManeuver(VESSEL* vessel, double GETbase, double P30TIG, VECTOR3 dV_LVLH, SV sv, double attachedMass, double F, double isp)
{
	//INPUT:
	//vessel: vessel interface
	//GETbase: usually launch MJD
	//P30TIG: Time of ignition in seconds relative to GETbase
	//dV_LVLH: DV Vector in LVLH coordinates
	//sv: state vector on trajectory before the maneuver, doesn't have to be at TIG
	//attachedMass: mass of the attached vessel, 0 if no vessel present
	//optional:
	//F: thrust in Newton
	//isp: specific impulse in m/s
	//OUTPUT:
	//sv3: state vector at cutoff

	if (length(dV_LVLH) == 0.0)
	{
		return sv;
	}

	double t_go, theta_T;
	VECTOR3 UX, UY, UZ, DV, DV_P, V_G, DV_C;
	SV sv2, sv3;

	if (vessel->GetGroupThruster(THGROUP_MAIN, 0))
	{
		if (F == 0.0)
		{
			F = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
		}

		if (isp == 0.0)
		{
			isp = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
		}
	}
	else
	{
		if (F == 0.0)
		{
			F = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_HOVER, 0));
		}

		if (isp == 0.0)
		{
			isp = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_HOVER, 0));
		}
	}

	OrbMech::oneclickcoast(sv.R, sv.V, sv.MJD, P30TIG - (sv.MJD - GETbase)*24.0*3600.0, sv2.R, sv2.V, sv.gravref, sv2.gravref);
	sv2.mass = sv.mass;
	sv2.MJD = GETbase + P30TIG / 24.0 / 3600.0;

	UY = unit(crossp(sv2.V, sv2.R));
	UZ = unit(-sv2.R);
	UX = crossp(UY, UZ);
	DV = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
	DV_P = UX*dV_LVLH.x + UZ*dV_LVLH.z;

	if (length(DV_P) != 0.0)
	{
		theta_T = length(crossp(sv2.R, sv2.V))*length(dV_LVLH)*(sv2.mass + attachedMass) / OrbMech::power(length(sv2.R), 2.0) / F;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + UY*dV_LVLH.y;
	}
	else
	{
		V_G = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
	}

	OrbMech::poweredflight(sv2.R, sv2.V, sv2.MJD, sv2.gravref, F, isp, sv2.mass + attachedMass, V_G, sv3.R, sv3.V, sv3.mass, t_go);
	sv3.gravref = sv2.gravref;
	sv3.MJD = sv2.MJD + t_go / 24.0 / 3600.0;
	sv3.mass -= attachedMass;

	return sv3;
}

SV RTCC::ExecuteManeuver(VESSEL* vessel, double GETbase, double P30TIG, VECTOR3 dV_LVLH, SV sv, double attachedMass, MATRIX3 &Q_Xx, VECTOR3 &V_G, double F, double isp)
{
	//INPUT:
	//vessel: vessel interface
	//GETbase: usually launch MJD
	//P30TIG: Time of ignition in seconds relative to GETbase
	//dV_LVLH: DV Vector in LVLH coordinates
	//sv: state vector on trajectory before the maneuver, doesn't have to be at TIG
	//attachedMass: mass of the attached vessel, 0 if no vessel present
	//optional:
	//F: thrust in Newton
	//isp: specific impulse in m/s
	//OUTPUT:
	//sv3: state vector at cutoff

	double t_go, theta_T;
	VECTOR3 UX, UY, UZ, DV, DV_P, DV_C;
	SV sv2, sv3;

	if (vessel)
	{
		if (vessel->GetGroupThruster(THGROUP_MAIN, 0))
		{
			if (F == 0.0)
			{
				F = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
			}

			if (isp == 0.0)
			{
				isp = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
			}
		}
		else
		{
			if (F == 0.0)
			{
				F = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_HOVER, 0));
			}

			if (isp == 0.0)
			{
				isp = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_HOVER, 0));
			}
		}
	}

	OrbMech::oneclickcoast(sv.R, sv.V, sv.MJD, P30TIG - (sv.MJD - GETbase)*24.0*3600.0, sv2.R, sv2.V, sv.gravref, sv2.gravref);
	sv2.mass = sv.mass;
	sv2.MJD = GETbase + P30TIG / 24.0 / 3600.0;

	UY = unit(crossp(sv2.V, sv2.R));
	UZ = unit(-sv2.R);
	UX = crossp(UY, UZ);
	Q_Xx = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	DV = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
	DV_P = UX*dV_LVLH.x + UZ*dV_LVLH.z;

	if (length(DV_P) != 0.0)
	{
		theta_T = length(crossp(sv2.R, sv2.V))*length(dV_LVLH)*(sv2.mass + attachedMass) / OrbMech::power(length(sv2.R), 2.0) / F;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + UY*dV_LVLH.y;
	}
	else
	{
		V_G = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
	}

	OrbMech::poweredflight(sv2.R, sv2.V, sv2.MJD, sv2.gravref, F, isp, sv2.mass + attachedMass, V_G, sv3.R, sv3.V, sv3.mass, t_go);
	sv3.gravref = sv2.gravref;
	sv3.MJD = sv2.MJD + t_go / 24.0 / 3600.0;
	sv3.mass -= attachedMass;

	return sv3;
}

void RTCC::RTEFlybyTargeting(RTEFlybyOpt *opt, EntryResults *res)
{
	Flyby* flybycalc;
	bool endi = false;
	double EMSAlt, dt22, mu_E, MJDguess, CSMmass, LMmass;
	VECTOR3 R05G, V05G, R_A, V_A, R0M, V0M;
	VECTOR3 Llambda, R_cor, V_cor;
	double t_slip, SVMJD;
	MATRIX3 Q_Xx;
	OBJHANDLE hEarth = oapiGetObjectByName("Earth");
	OBJHANDLE hMoon = oapiGetObjectByName("Moon");
	OBJHANDLE gravref = AGCGravityRef(opt->vessel);

	EMSAlt = 297431.0*0.3048;
	mu_E = GGRAV*oapiGetMass(hEarth);

	if (opt->useSV)
	{
		R0M = opt->RV_MCC.R;
		V0M = opt->RV_MCC.V;
		SVMJD = opt->RV_MCC.MJD;
		gravref = opt->RV_MCC.gravref;
		CSMmass = opt->RV_MCC.mass;
	}
	else
	{
		gravref = AGCGravityRef(opt->vessel);
		opt->vessel->GetRelativePos(gravref, R_A);
		opt->vessel->GetRelativeVel(gravref, V_A);
		SVMJD = oapiGetSimMJD();
		R0M = _V(R_A.x, R_A.z, R_A.y);
		V0M = _V(V_A.x, V_A.z, V_A.y);

		CSMmass = opt->vessel->GetMass();
	}


	if (opt->TIGguess == 0.0)
	{
		//TEI targeting uses "now" as the initial guess
		MJDguess = SVMJD;
	}
	else
	{
		MJDguess = opt->GETbase + opt->TIGguess / 24.0 / 3600.0;
	}

	flybycalc = new Flyby(R0M, V0M, SVMJD, gravref, MJDguess, opt->EntryLng, opt->entrylongmanual, opt->returnspeed, opt->FlybyType);

	while (!endi)
	{
		endi = flybycalc->Flybyiter();
	}

	dt22 = OrbMech::time_radius(flybycalc->R_EI, flybycalc->V_EI, oapiGetSize(hEarth) + EMSAlt, -1, mu_E);
	OrbMech::rv_from_r0v0(flybycalc->R_EI, flybycalc->V_EI, dt22, R05G, V05G, mu_E); //Entry Interface to 0.05g

	res->latitude = flybycalc->EntryLatcor;
	res->longitude = flybycalc->EntryLngcor;
	res->P30TIG = (flybycalc->TIG - opt->GETbase)*24.0*3600.0;
	res->dV_LVLH = flybycalc->Entry_DV;
	res->ReA = flybycalc->EntryAng;
	res->GET400K = (flybycalc->EIMJD - opt->GETbase)*24.0*3600.0;
	res->GET05G = res->GET400K + dt22;
	res->RTGO = 1285.0 - 3437.7468*acos(dotp(unit(flybycalc->R_EI), unit(R05G)));
	res->VIO = length(V05G);
	res->precision = flybycalc->precision;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0;
	}

	SV sv_tig;

	sv_tig.gravref = hMoon;
	sv_tig.mass = CSMmass;
	sv_tig.MJD = flybycalc->TIG;
	sv_tig.R = flybycalc->Rig;
	sv_tig.V = flybycalc->Vig;

	res->sv_postburn = sv_tig;
	res->sv_postburn.V = flybycalc->Vig_apo;

	FiniteBurntimeCompensation(opt->vesseltype, sv_tig, LMmass, flybycalc->Vig_apo - flybycalc->Vig, true, Llambda, t_slip);

	OrbMech::oneclickcoast(flybycalc->Rig, flybycalc->Vig, flybycalc->TIG, t_slip, R_cor, V_cor, hMoon, hMoon);

	Q_Xx = OrbMech::LVLH_Matrix(R_cor, V_cor);

	//EntryTIGcor = teicalc->TIG_imp + t_slip;
	res->P30TIG = (flybycalc->TIG - opt->GETbase)*24.0*3600.0 + t_slip;
	res->dV_LVLH = mul(Q_Xx, Llambda);

	delete flybycalc;
}

void RTCC::TEITargeting(TEIOpt *opt, EntryResults *res)
{
	TEI* teicalc;
	SV sv0;
	bool endi = false;
	double EMSAlt, dt22, mu_E, MJDguess, LMmass;
	VECTOR3 R05G, V05G;
	VECTOR3 Llambda, R_cor, V_cor;
	double t_slip;
	MATRIX3 Q_Xx;
	OBJHANDLE hEarth = oapiGetObjectByName("Earth");
	OBJHANDLE hMoon = oapiGetObjectByName("Moon");

	EMSAlt = 297431.0*0.3048;
	mu_E = GGRAV*oapiGetMass(hEarth);

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}


	if (opt->TIGguess == 0.0)
	{
		//TEI targeting uses "now" as the initial guess
		MJDguess = sv0.MJD;
	}
	else
	{
		MJDguess = opt->GETbase + opt->TIGguess / 24.0 / 3600.0;
	}

	teicalc = new TEI(sv0.R, sv0.V, sv0.MJD, sv0.gravref, MJDguess, opt->EntryLng, opt->entrylongmanual, opt->returnspeed, opt->RevsTillTEI);

	while (!endi)
	{
		endi = teicalc->TEIiter();
	}

	dt22 = OrbMech::time_radius(teicalc->R_EI, teicalc->V_EI, oapiGetSize(hEarth) + EMSAlt, -1, mu_E);
	OrbMech::rv_from_r0v0(teicalc->R_EI, teicalc->V_EI, dt22, R05G, V05G, mu_E); //Entry Interface to 0.05g

	res->latitude = teicalc->EntryLatcor;
	res->longitude = teicalc->EntryLngcor;
	res->P30TIG = (teicalc->TIG - opt->GETbase)*24.0*3600.0;
	res->dV_LVLH = teicalc->Entry_DV;
	res->ReA = teicalc->EntryAng;
	res->GET400K = (teicalc->EIMJD - opt->GETbase)*24.0*3600.0;
	res->GET05G = res->GET400K + dt22;
	res->RTGO = 1285.0 - 3437.7468*acos(dotp(unit(teicalc->R_EI), unit(R05G)));
	res->VIO = length(V05G);
	res->precision = teicalc->precision;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0;
	}

	SV sv_tig;

	sv_tig.gravref = hMoon;
	sv_tig.mass = sv0.mass;
	sv_tig.MJD = teicalc->TIG;
	sv_tig.R = teicalc->Rig;
	sv_tig.V = teicalc->Vig;

	FiniteBurntimeCompensation(opt->vesseltype, sv_tig, LMmass, teicalc->Vig_apo - teicalc->Vig, 1, Llambda, t_slip);

	OrbMech::oneclickcoast(teicalc->Rig, teicalc->Vig, teicalc->TIG, t_slip, R_cor, V_cor, hMoon, hMoon);

	Q_Xx = OrbMech::LVLH_Matrix(R_cor, V_cor);

	//EntryTIGcor = teicalc->TIG_imp + t_slip;
	res->P30TIG = (teicalc->TIG - opt->GETbase)*24.0*3600.0 + t_slip;
	res->dV_LVLH = mul(Q_Xx, Llambda);

	delete teicalc;
}

void RTCC::LunarOrbitMapUpdate(SV sv0, double GETbase, AP10MAPUPDATE &pad)
{
	double ttoLOS, ttoAOS, ttoSS, ttoSR, ttoPM;
	OBJHANDLE hEarth, hSun;

	double t_lng;

	hEarth = oapiGetObjectByName("Earth");
	hSun = oapiGetObjectByName("Sun");

	ttoLOS = OrbMech::sunrise(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hEarth, 0, 0, true);
	ttoAOS = OrbMech::sunrise(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hEarth, 1, 0, true);

	pad.LOSGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoLOS;
	pad.AOSGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoAOS;

	ttoSS = OrbMech::sunrise(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hSun, 0, 0, true);
	ttoSR = OrbMech::sunrise(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hSun, 1, 0, true);

	pad.SSGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoSS;
	pad.SRGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoSR;

	t_lng = OrbMech::P29TimeOfLongitude(sv0.R, sv0.V, sv0.MJD, sv0.gravref, -150.0*RAD);
	ttoPM = (t_lng - sv0.MJD)*24.0 * 3600.0;
	pad.PMGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoPM;
}

void RTCC::LandmarkTrackingPAD(LMARKTRKPADOpt *opt, AP11LMARKTRKPAD &pad)
{
	VECTOR3 RA0_orb, VA0_orb, RA0, VA0, R_P, RA1, VA1, u;
	double SVMJD, dt1, dt2, get, MJDguess, sinl, gamma, r_0, LmkRange;
	OBJHANDLE hEarth, hMoon, gravref;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	gravref = AGCGravityRef(opt->vessel);

	opt->vessel->GetRelativePos(gravref, RA0_orb);
	opt->vessel->GetRelativeVel(gravref, VA0_orb);
	SVMJD = oapiGetSimMJD();
	get = (SVMJD - opt->GETbase)*24.0*3600.0;
	RA0 = _V(RA0_orb.x, RA0_orb.z, RA0_orb.y);
	VA0 = _V(VA0_orb.x, VA0_orb.z, VA0_orb.y);

	for (int i = 0;i < opt->entries;i++)
	{
		MJDguess = opt->GETbase + opt->LmkTime[i] / 24.0 / 3600.0;

		R_P = unit(_V(cos(opt->lng[i])*cos(opt->lat[i]), sin(opt->lat[i]), sin(opt->lng[i])*cos(opt->lat[i])))*(oapiGetSize(gravref) + opt->alt[i]);

		OrbMech::oneclickcoast(RA0, VA0, SVMJD, opt->LmkTime[i] - get, RA1, VA1, gravref, gravref);

		dt1 = OrbMech::findelev_gs(RA1, VA1, R_P, MJDguess, 180.0*RAD, gravref, LmkRange);
		dt2 = OrbMech::findelev_gs(RA1, VA1, R_P, MJDguess, 145.0*RAD, gravref, LmkRange);

		pad.T1[i] = dt1 + (MJDguess - opt->GETbase) * 24.0 * 60.0 * 60.0;
		pad.T2[i] = dt2 + (MJDguess - opt->GETbase) * 24.0 * 60.0 * 60.0;

		u = unit(_V(R_P.x, R_P.z, R_P.y));
		sinl = u.z;

		if (gravref == hEarth)
		{
			double a, b, r_F;
			a = 6378166;
			b = 6356784;
			gamma = b * b / a / a;
			r_F = sqrt(b*b / (1.0 - (1.0 - b * b / a / a)*(1.0 - sinl * sinl)));
			r_0 = r_F;
		}
		else
		{
			gamma = 1.0;
			r_0 = oapiGetSize(gravref);
		}

		pad.Alt[i] = (length(R_P) - r_0) / 1852.0;
		pad.CRDist[i] = LmkRange / 1852.0;

		pad.Lat[i] = atan2(u.z, gamma*sqrt(u.x*u.x + u.y*u.y))*DEG;
		pad.Lng05[i] = opt->lng[i] / 2.0*DEG;
	}

	pad.entries = opt->entries;
}

SV RTCC::coast(SV sv0, double dt)
{
	SV sv1;
	OBJHANDLE gravout = NULL;

	OrbMech::oneclickcoast(sv0.R, sv0.V, sv0.MJD, dt, sv1.R, sv1.V, sv0.gravref, gravout);
	sv1.gravref = gravout;
	sv1.mass = sv0.mass;
	sv1.MJD = sv0.MJD + dt / 24.0 / 3600.0;

	return sv1;
}

void RTCC::GetTLIParameters(VECTOR3 &RIgn_global, VECTOR3 &VIgn_global, VECTOR3 &dV_LVLH, double &IgnMJD)
{
	VECTOR3 RIgn, VIgn;
	double GETbase;
	SV sv, sv2;

	GETbase = getGETBase();

	IgnMJD = GETbase + TimeofIgnition / 24.0 / 3600.0;

	sv = StateVectorCalc(calcParams.src);

	sv2 = coast(sv, (IgnMJD - sv.MJD)*24.0*3600.0);

	//RIgn = tmul(Rot, sv2.R);
	//VIgn = tmul(Rot, sv2.V);
	RIgn = sv2.R;
	VIgn = sv2.V;

	RIgn_global = _V(RIgn.x, RIgn.z, RIgn.y);
	VIgn_global = _V(VIgn.x, VIgn.z, VIgn.y);
	dV_LVLH = DeltaV_LVLH;
}

bool RTCC::REFSMMATDecision(VECTOR3 Att)
{
	if (cos(Att.z) > 0.5) //Yaw between 300° and 60°
	{
		return true;
	}

	return false;
}

SevenParameterUpdate RTCC::TLICutoffToLVDCParameters(VECTOR3 R_TLI, VECTOR3 V_TLI, double GETbase, double P30TIG, double TB5, double mu, double T_RG)
{
	//Inputs:
	//
	//R_TLI: TLI cutoff position vector, right-handed, ecliptic coordinate system
	//V_TLI: TLI cutoff velocity vector, right-handed, ecliptic coordinate system

	MATRIX3 Rot;
	VECTOR3 R_TLI2, V_TLI2;
	double T_RP, tb5start;
	SevenParameterUpdate param;
	OELEMENTS coe;

	tb5start = TB5 - 17.0;
	T_RP = P30TIG - tb5start - T_RG;

	Rot = OrbMech::GetRotationMatrix(oapiGetObjectByName("Earth"), GETbase - 17.0 / 24.0 / 3600.0);

	R_TLI2 = tmul(Rot, _V(R_TLI.x, R_TLI.z, R_TLI.y));
	V_TLI2 = tmul(Rot, _V(V_TLI.x, V_TLI.z, V_TLI.y));

	R_TLI2 = _V(R_TLI2.x, R_TLI2.z, R_TLI2.y);
	V_TLI2 = _V(V_TLI2.x, V_TLI2.z, V_TLI2.y);

	coe = OrbMech::coe_from_PACSS4(R_TLI2, V_TLI2, mu);

	param.alpha_D = coe.w;
	param.C3 = coe.h;
	param.e = coe.e;
	param.f = coe.TA;
	param.Inclination = coe.i;
	param.theta_N = coe.RA;
	param.T_RP = T_RP;

	return param;
}

void RTCC::LVDCTLIPredict(LVDCTLIparam lvdc, VESSEL* vessel, double GETbase, VECTOR3 &dV_LVLH, double &P30TIG, VECTOR3 &R_TLI, VECTOR3 &V_TLI, double &T_TLI)
{
	double SVMJD, day, theta_E, MJD_GRR, mu_E, MJD_TST, dt, cos_psiT, sin_psiT, Inclination, X_1, X_2, theta_N, p_N, T_M, R, e, p, alpha_D;
	double MJD_TIG, R_T, V_T, K_5, G_T, gamma_T, boil;
	VECTOR3 R_A, V_A, PosS, DotS, T_P, N, PosP, Sbar, DotP, Sbardot, R0, V0, R1, V1, Sbar_1, Cbar_1, R2, V2;
	MATRIX3 mat, MX_EPH, MX_B, MX_G;
	OBJHANDLE gravref;
	OELEMENTS coe;

	//Constants
	gravref = AGCGravityRef(vessel);
	mu_E = GGRAV*oapiGetMass(gravref);
	boil = (1.0 - 0.99998193) / 10.0;

	//State Vector
	modf(oapiGetSimMJD(), &day);
	MJD_GRR = day + lvdc.T_L / 24.0 / 3600.0;
	mat = OrbMech::Orbiter2PACSS13(MJD_GRR, lvdc.phi_L, -80.6041140*RAD, lvdc.Azimuth);
	vessel->GetRelativePos(gravref, R_A);
	vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();
	R0 = _V(R_A.x, R_A.z, R_A.y);
	V0 = _V(V_A.x, V_A.z, V_A.y);

	//Find TB6 start
	//Determination of S-bar and S-bar-dot
	theta_E = lvdc.theta_EO + lvdc.omega_E*lvdc.t_D;
	MX_EPH = mul(OrbMech::transpose_matrix(lvdc.MX_A), _M(cos(theta_E), sin(theta_E), 0, 0, 0, -1, -sin(theta_E), cos(theta_E), 0));
	T_P = mul(MX_EPH, unit(lvdc.TargetVector));

	MJD_TST = MJD_GRR + (lvdc.TB5 + lvdc.T_ST) / 24.0 / 3600.0;
	OrbMech::oneclickcoast(R0, V0, SVMJD, (MJD_TST - SVMJD) * 24.0 * 3600.0, R1, V1, gravref, gravref);

	dt = 0;

	do
	{
		OrbMech::oneclickcoast(R1, V1, MJD_TST, dt, R2, V2, gravref, gravref);
		//R2 = tmul(Rot, R2);
		//V2 = tmul(Rot, V2);
		R2 = _V(R2.x, R2.z, R2.y);
		V2 = _V(V2.x, V2.z, V2.y);
		PosS = mul(mat, R2);
		DotS = mul(mat, V2);
		//OrbMech::rv_from_r0v0(R1, V1, dt, PosS, DotS, mu_E);
		N = unit(crossp(PosS, DotS));
		PosP = crossp(N, unit(PosS));
		Sbar = unit(PosS)*cos(lvdc.beta) + PosP*sin(lvdc.beta);
		DotP = crossp(N, DotS / length(PosS));

		Sbardot = DotS / length(PosS)*cos(lvdc.beta) + DotP*sin(lvdc.beta);
		dt += 1.0;

	} while (!((dotp(Sbardot, T_P) < 0 && dotp(Sbar, T_P) <= cos(lvdc.alpha_TS))));

	//Advance to Ignition State
	OrbMech::rv_from_r0v0(PosS, DotS, lvdc.T_RG, PosS, DotS, mu_E);

	cos_psiT = dotp(Sbar,T_P);
	sin_psiT = sqrt(1.0 - pow(cos_psiT, 2));
	Sbar_1 = (Sbar*cos_psiT - T_P)*(1.0 / sin_psiT);
	Cbar_1 = crossp(Sbar_1, Sbar);
	Inclination = acos(dotp(_V(lvdc.MX_A.m21, lvdc.MX_A.m22, lvdc.MX_A.m23),Cbar_1));
	X_1 = dotp(_V(lvdc.MX_A.m31, lvdc.MX_A.m32, lvdc.MX_A.m33), crossp(Cbar_1, _V(lvdc.MX_A.m21, lvdc.MX_A.m22, lvdc.MX_A.m23)));
	X_2 = dotp(_V(lvdc.MX_A.m11, lvdc.MX_A.m12, lvdc.MX_A.m13), crossp(Cbar_1, _V(lvdc.MX_A.m21, lvdc.MX_A.m22, lvdc.MX_A.m23)));
	theta_N = atan2(X_1, X_2);
	p_N = lvdc.mu / lvdc.C_3*(pow(lvdc.e_N, 2) - 1.0);
	T_M = p_N / (1.0 - lvdc.e_N*lvdc.cos_sigma);
	R = length(PosS);
	e = R / lvdc.R_N*(lvdc.e_N - 1.0) + 1.0;
	p = lvdc.mu / lvdc.C_3*(pow(e, 2) - 1.0);

	alpha_D = acos(dotp(Sbar, T_P)) - acos((1.0 - p / T_M) / e) + atan2(dotp(Sbar_1, crossp(Cbar_1, _V(lvdc.MX_A.m21, lvdc.MX_A.m22, lvdc.MX_A.m23))), dotp(Sbar, crossp(Cbar_1, _V(lvdc.MX_A.m21, lvdc.MX_A.m22, lvdc.MX_A.m23))));

	MJD_TIG = MJD_TST + (dt + lvdc.T_RG) / 24.0 / 3600.0;
	P30TIG = (MJD_TIG - GETbase) * 24.0 * 3600.0;

	MX_B = _M(cos(theta_N), 0, sin(theta_N), sin(theta_N)*sin(Inclination), cos(Inclination), -cos(theta_N)*sin(Inclination),
		-sin(theta_N)*cos(Inclination), sin(Inclination), cos(theta_N)*cos(Inclination));
	MX_G = mul(MX_B, lvdc.MX_A);
	R_T = p / (1.0 + e*cos(lvdc.f));
	K_5 = sqrt(lvdc.mu / p);
	V_T = K_5*sqrt(1.0 + 2.0 * e*cos(lvdc.f) + pow(e, 2));
	gamma_T = atan((e*sin(lvdc.f)) / (1.0 + cos(lvdc.f)));
	G_T = -lvdc.mu / pow(R_T, 2);

	double Fs, V_ex, mass, a_T, tau, t_go, V, sin_gam, cos_gam, dot_phi_1, dot_phi_T, phi_T, xi_T, dot_zeta_T, dot_xi_T, ddot_zeta_GT, ddot_xi_GT, m0, m1, dt1;
	double dot_dxit, dot_detat, dot_dzetat, dV, dT, f, L, dL;
	VECTOR3 Pos4, PosXEZ, DotXEZ, ddotG_act, DDotXEZ_G;
	MATRIX3 MX_phi_T, MX_K;

	Fs = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
	V_ex = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
	mass = vessel->GetMass();
	m0 = vessel->GetEmptyMass();
	dt1 = dt + (MJD_TST - SVMJD) * 24.0 * 3600.0;
	m1 = (mass - m0)*exp(-boil*dt1);

	a_T = Fs / (m0 + m1);
	tau = V_ex / a_T;
	t_go = 305.0;//Initial estimate
	dT = 1.0;

	while (abs(dT) > 0.01)
	{
		//IGM
		ddotG_act = -PosS*mu_E / OrbMech::power(length(PosS), 3.0);
		Pos4 = mul(MX_G, PosS);


		L = V_ex * log(tau / (tau - t_go));

		V = length(DotS);
		R = length(PosS);
		sin_gam = ((PosS.x*DotS.x) + (PosS.y*DotS.y) + (PosS.z*DotS.z)) / (R*V);
		cos_gam = pow(1.0 - pow(sin_gam, 2), 0.5);
		dot_phi_1 = (V*cos_gam) / R;
		dot_phi_T = (V_T*cos(gamma_T)) / R_T;
		phi_T = atan2(Pos4.z, Pos4.x) + (((dot_phi_1 + dot_phi_T) / 2.0)*t_go);

		f = phi_T + alpha_D;
		R_T = p / (1 + ((e*(cos(f)))));
		V_T = K_5 * pow(1 + ((2 * e)*(cos(f))) + pow(e, 2), 0.5);
		gamma_T = atan2((e*(sin(f))), (1 + (e*(cos(f)))));
		G_T = -mu_E / pow(R_T, 2);

		xi_T = R_T*cos(gamma_T);
		dot_zeta_T = V_T;
		dot_xi_T = 0.0;
		ddot_zeta_GT = G_T*sin(gamma_T);
		ddot_xi_GT = G_T*cos(gamma_T);
		phi_T = phi_T - gamma_T;

		MX_phi_T.m11 = (cos(phi_T));    MX_phi_T.m12 = 0; MX_phi_T.m13 = ((sin(phi_T)));
		MX_phi_T.m21 = 0;               MX_phi_T.m22 = 1; MX_phi_T.m23 = 0;
		MX_phi_T.m31 = (-sin(phi_T)); MX_phi_T.m32 = 0; MX_phi_T.m33 = (cos(phi_T));
		MX_K = mul(MX_phi_T, MX_G);
		PosXEZ = mul(MX_K, PosS);
		DotXEZ = mul(MX_K, DotS);
		VECTOR3 RTT_T1, RTT_T2;
		RTT_T1.x = ddot_xi_GT; RTT_T1.y = 0;        RTT_T1.z = ddot_zeta_GT;
		RTT_T2 = ddotG_act;
		RTT_T2 = mul(MX_K, RTT_T2);
		RTT_T1 = RTT_T1 + RTT_T2;
		DDotXEZ_G = _V(0.5*RTT_T1.x, 0.5*RTT_T1.y, 0.5*RTT_T1.z);


		dot_dxit = dot_xi_T - DotXEZ.x - (DDotXEZ_G.x*t_go);
		dot_detat = -DotXEZ.y - (DDotXEZ_G.y * t_go);
		dot_dzetat = dot_zeta_T - DotXEZ.z - (DDotXEZ_G.z * t_go);

		dV = pow((pow(dot_dxit, 2) + pow(dot_detat, 2) + pow(dot_dzetat, 2)), 0.5);
		dL = (((pow(dot_dxit, 2) + pow(dot_detat, 2) + pow(dot_dzetat, 2)) / L) - L) / 2;


		dT = (dL*(tau - t_go)) / V_ex;
		t_go = t_go + dT;

		/*// TARGET PARAMETER UPDATE
		if (!(UP > 0)) {
			UP = 1;
			L = L + dL;
			J = J + (dL*t_go);
			goto gtupdate; // Recycle. 
		}*/
	}

	coe.e = e;
	coe.h = lvdc.C_3;
	coe.i = Inclination;
	coe.RA = theta_N;
	coe.TA = f;
	coe.w = alpha_D;

	OrbMech::PACSS13_from_coe(coe, lvdc.phi_L, lvdc.Azimuth, mu_E, R_TLI, V_TLI);

	R_TLI = tmul(mat, R_TLI);
	V_TLI = tmul(mat, V_TLI);
	R_TLI = _V(R_TLI.x, R_TLI.z, R_TLI.y);
	V_TLI = _V(V_TLI.x, V_TLI.z, V_TLI.y);

	T_TLI = P30TIG + t_go;
	dV_LVLH = _V(1.0, 0.0, 0.0)*L;
}

void RTCC::FiniteBurntimeCompensation(int vesseltype, SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, SV &sv_out)
{
	//For CSM, LM or CSM/LM docked maneuvers
	//INPUT:
	//vesseltype: thrusting vessel, 0 = CSM, 1 = LM
	//sv: State vector at impulsive TIG
	//attachedMass: mass of the attached vessel, 0 if no vessel present
	//DV: Delta velocity of impulsive thrusting maneuver
	//engine: 0 = use RCS, 1 = use SPS/DPS engine, 2 = use APS engine
	//
	//OUTPUT:
	//DV_imp: non-impulsive delta velocity
	//t_slip: time in seconds the maneuver has slipped to compensate for finite burntime; should always be negative
	//sv_out: complete state vector at actual cutoff

	double f_T, isp, F_average;

	sv_out.gravref = sv.gravref;

	if (engine == 0)
	{
		isp = 2706.64;
		F_average = 400 * 4.448222;
	}
	else
	{
		if (vesseltype == 0)
		{
			f_T = SPS_THRUST;
			isp = SPS_ISP;

			F_average = f_T;
		}
		else
		{
			double bt, bt_var;
			int step;

			if (engine == 1)
			{
				f_T = DPS_THRUST;
				isp = DPS_ISP;

				LMThrottleProgram(f_T, isp, sv.mass + attachedMass, length(DV), F_average, bt, bt_var, step);
			}
			else
			{
				f_T = APS_THRUST;
				isp = APS_ISP;

				F_average = f_T;
			}
		}
	}

	OrbMech::impulsive(sv.R, sv.V, sv.MJD, sv.gravref, F_average, isp, sv.mass + attachedMass, DV, DV_imp, t_slip, sv_out.R, sv_out.V, sv_out.MJD, sv_out.mass);

	sv_out.mass -= attachedMass;
}

void RTCC::FiniteBurntimeCompensation(int vesseltype, SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip)
{
	SV sv_out;

	FiniteBurntimeCompensation(vesseltype, sv, attachedMass, DV, engine, DV_imp, t_slip, sv_out);
}

void RTCC::PoweredFlightProcessor(SV sv0, double GETbase, double GET_TIG_imp, int vesseltype, int enginetype, double attachedMass, VECTOR3 DV, double &GET_TIG, VECTOR3 &dV_LVLH)
{
	SV sv_pre, sv_tig, sv_post;
	MATRIX3 Q_Xx;
	VECTOR3 Llambda;
	double t_slip;

	sv_pre = coast(sv0, GET_TIG_imp - OrbMech::GETfromMJD(sv0.MJD, GETbase));

	FiniteBurntimeCompensation(vesseltype, sv_pre, attachedMass, DV, enginetype, Llambda, t_slip, sv_post);

	sv_tig = coast(sv_pre, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);
	GET_TIG = GET_TIG_imp + t_slip;
	dV_LVLH = mul(Q_Xx, Llambda);
}

double RTCC::GetDockedVesselMass(VESSEL *vessel)
{
	//INPUT:
	//vessel: primary vessel
	//OUTPUT:
	//mass of the docked vessel, 0 if no vessel docked

	DOCKHANDLE dock;
	OBJHANDLE hLM;
	VESSEL *lm;
	double LMmass;

	if (vessel->DockingStatus(0) == 1)
	{
		dock = vessel->GetDockHandle(0);
		hLM = vessel->GetDockStatus(dock);
		lm = oapiGetVesselInterface(hLM);

		//Special case: S-IVB, but we want the LM mass
		if (!stricmp(lm->GetClassName(), "ProjectApollo\\sat5stg3") ||
			!stricmp(lm->GetClassName(), "ProjectApollo/sat5stg3"))
		{
			SIVB *sivb = (SIVB *)lm;
			LMmass = sivb->GetPayloadMass();
		}
		else
		{
			LMmass = lm->GetMass();
		}
	}
	else
	{
		LMmass = 0.0;
	}

	return LMmass;
}

double RTCC::PericynthionTime(VESSEL *vessel)
{
	OBJHANDLE gravref;
	VECTOR3 R_A, V_A, R0, V0;
	double SVMJD, mu, dt, GETbase;

	GETbase = getGETBase();
	gravref = AGCGravityRef(vessel);

	mu = GGRAV*oapiGetMass(gravref);
	vessel->GetRelativePos(gravref, R_A);
	vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();
	R0 = _V(R_A.x, R_A.z, R_A.y);
	V0 = _V(V_A.x, V_A.z, V_A.y);

	dt = OrbMech::timetoperi(R0, V0, mu);
	return (SVMJD-GETbase)*24.0*3600.0 + dt;
}

SV RTCC::FindPericynthion(SV sv0)
{
	OBJHANDLE hMoon = oapiGetObjectByName("Moon");
	double dt = OrbMech::timetoperi_integ(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hMoon);
	return coast(sv0, dt);
}

double RTCC::FindOrbitalMidnight(SV sv, double GETbase, double t_TPI_guess)
{
	SV sv1;
	double GET_SV, dt, ttoMidnight;

	OBJHANDLE hSun = oapiGetObjectByName("Sun");

	GET_SV = OrbMech::GETfromMJD(sv.MJD, GETbase);
	dt = t_TPI_guess - GET_SV;

	sv1 = coast(sv, dt);

	ttoMidnight = OrbMech::sunrise(sv1.R, sv1.V, sv1.MJD, sv1.gravref, hSun, 1, 1, false);
	return t_TPI_guess + ttoMidnight;
}

bool RTCC::NC1NC2Program(SV sv_C, SV sv_W, double GETbase, double E_L, double t_C, double dt, double t_F, double dh_F, double n_H1, int s, double dh, double n_C, VECTOR3 &dV_NC1_LVLH, double &dh_NC2, double &dv_NC2, double &t_NC2, VECTOR3 &dV_NC2_LVLH, double &dv_NCC, double &t_NCC, double &t_NSR, VECTOR3 &dV_NSR, bool NPC)
{
	int s_F;
	double t_N, dv_C, dv_H1, dv_H2, c, theta, mu, eps1, pi, t, t_H1, t_H2, e_H1, eps2, c_I, t_S, e_H2, e_P, c_F, dvo_C, eo_P;
	double d, MJD, p_H1, p_H2, p_C, e_H1o, e_H2o, dv_H1o, dv_H2o, tt, t_WC;
	VECTOR3 R_WS, V_WS, R_CC, V_CC, R_WC, V_WC, am, ur_WD, R_WF, V_WF, R_WJ, V_WJ, R_CFD, R_CH1, V_CH1, R_CH2, V_CH2, V_CCF, V_CH1F;
	VECTOR3 R_WH2, V_WH2, V_CH2F, R_CS, V_CS, R_CF, V_CF, R_CSF, V_CSF,R_WAF, V_WAF, X, Y, Z, dV_LVLH, dV_NSR_LVLH;
	OBJHANDLE gravref;

	gravref = sv_C.gravref;
	mu = GGRAV*oapiGetMass(gravref);
	eps1 = 0.000001;
	eps2 = 0.01;
	p_C = 1;
	MJD = GETbase + t_C / 24.0 / 3600.0;

	s_F = 0;
	p_H1 = p_H2 = c_F = c_I = 0.0;
	t_N = t_C;
	R_WS = sv_W.R;
	V_WS = sv_W.V;
	pi = PI;
	dv_C = 225.0*0.3048;
	dv_H1 = 165.0*0.3048;
	dv_H2 = 25.0*0.3048;

	if (s == 2)
	{
		n_C = n_H1;
		dv_C = dv_H1;
	}

	OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t_C, R_CC, V_CC, gravref, gravref);
	OrbMech::rv_from_r0v0(sv_W.R, sv_W.V, (GETbase - sv_W.MJD)*24.0*3600.0 + t_C, R_WC, V_WC, mu);
	t_WC = t_C;
	do
	{
		am = unit(crossp(R_WC, V_WC));
		ur_WD = unit(crossp(crossp(crossp(R_CC, V_CC), R_CC), am));
		d = OrbMech::sign(n_C - 18.0);
		c = OrbMech::sign(dotp(crossp(ur_WD, R_WC), am));
		theta = pi - c*(pi - acos(dotp(ur_WD, unit(R_WC)))) + (d + 1.0)*(c + 1.0)*pi*0.5;

		if (abs(theta) >= eps1)
		{
			if (c_I == 10)
			{
				return false;
			}
			t = OrbMech::time_theta(R_WC, V_WC, theta, mu);
			if (pi != 0.0)
			{
				pi = 0.0;
				R_WC = R_WS;
				V_WC = V_WS;
				t_WC = (sv_W.MJD - GETbase)*24.0*3600.0;
			}
			c_I++;
			t_N -= t;
			OrbMech::oneclickcoast(R_WC, V_WC, MJD, t_N - t_WC, R_WC, V_WC, gravref, gravref);
			t_WC = t_N;
		}
	} while (abs(theta) >= eps1);

	R_CC = unit(R_CC - am*dotp(R_CC, am))*length(R_CC);
	V_CC = unit(V_CC - am*dotp(V_CC, am))*length(V_CC);

	OrbMech::rv_from_r0v0(R_WC, V_WC, t_F - t_N, R_WF, V_WF, mu);

	if (!OrbMech::QDRTPI(R_WF, V_WF, MJD, gravref, mu, dh_F, E_L, 0, R_WJ, V_WJ))
	{
		return false;
	}

	R_CFD = R_WJ - unit(R_WJ)*dh_F;

	do
	{
		V_CCF = V_CC + unit(crossp(am, R_CC))*dv_C;
		OrbMech::REVUP(R_CC, V_CCF, n_C, mu, R_CH1, V_CH1, tt);
		t_H1 = t_C + tt;

		if (s == 1)
		{
			if (c_F == 0)
			{
				c_I = 0;
			}
			else
			{
				c_I = 0.5;
			}
			do
			{
				V_CH1F = V_CH1 + unit(crossp(am, R_CH1))*dv_H1;
				OrbMech::REVUP(R_CH1, V_CH1F, n_H1, mu, R_CH2, V_CH2, tt);
				t_H2 = t_H1 + tt;
				OrbMech::RADUP(R_WC, V_WC, R_CH2, mu, R_WH2, V_WH2);
				e_H1 = length(R_WH2) - length(R_CH2) - dh;
				if (p_H1 == 0 || abs(e_H1) >= eps2)
				{
					OrbMech::ITER(c_I, s_F, e_H1, p_H1, dv_H1, e_H1o, dv_H1o);
					if (s_F == 1)
					{
						return false;
					}
				}
			} while (abs(e_H1) >= eps2);
		}
		else
		{
			t_H2 = t_H1;
			R_CH2 = R_CH1;
			V_CH2 = V_CH1;
		}
		if (c_F == 0)
		{
			c_I = 0;
		}
		else
		{
			c_I = 0.5;
		}
		t_S = t_H2 + dt;
		do
		{
			V_CH2F = V_CH2 + unit(crossp(am, R_CH2))*dv_H2;
			OrbMech::rv_from_r0v0(R_CH2, V_CH2F, dt, R_CS, V_CS, mu);
			OrbMech::RADUP(R_WC, V_WC, R_CS, mu, R_WS, V_WS);
			OrbMech::COE(R_WS, V_WS, length(R_WS) - length(R_CS), mu, R_CSF, V_CSF);
			OrbMech::rv_from_r0v0(R_CS, V_CSF, t_F - t_S, R_CF, V_CF, mu);
			OrbMech::RADUP(R_WC, V_WC, R_CF, mu, R_WAF, V_WAF);
			e_H2 = length(R_WAF) - length(R_CF) - dh_F;

			if (p_H2 == 0 || abs(e_H2) >= eps2)
			{
				OrbMech::ITER(c_I, s_F, e_H2, p_H2, dv_H2, e_H2o, dv_H2o);
				if (s_F == 1)
				{
					return false;
				}
			}
		} while (abs(e_H2) >= eps2);
		e_P = OrbMech::sign(dotp(crossp(R_CF, R_CFD), am))*acos(dotp(R_CFD / length(R_CFD), R_CF / length(R_CF)));
		if (abs(e_P) >= 4.0*eps1)
		{
			OrbMech::ITER(c_F, s_F, e_P, p_C, dv_C, eo_P, dvo_C);
			if (s_F == 1)
			{
				return false;
			}
		}
	} while (abs(e_P) >= 4.0*eps1);

	if (NPC)
	{
		double dt_PC, dv_PC, Y_C;
		VECTOR3 u2, R_WCC, V_WCC, R_CCC, V_CCC, dV_NC, R_WPC, V_WPC, R_CPC, V_CPC, XX, YY, ZZ, V_CCC_apo;
		double c_PC, p_PC, eo_PC, dvo_PC;
		int s_PC;

		c_PC = 0.0;
		p_PC = 0.0;

		OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t_C, R_CCC, V_CCC, gravref, gravref);
		OrbMech::oneclickcoast(sv_W.R, sv_W.V, sv_W.MJD, (GETbase - sv_W.MJD)*24.0*3600.0 + t_C, R_WCC, V_WCC, gravref, gravref);

		u2 = unit(crossp(V_WCC, R_WCC));
		dv_PC = -dotp(V_CCC, u2);

		ZZ = -unit(R_CCC);
		YY = unit(crossp(V_CCC, R_CCC));
		XX = crossp(YY, ZZ);

		do
		{
			dV_LVLH = _V(dv_C, dv_PC, 0);
			dV_NC = tmul(_M(XX.x, XX.y, XX.z, YY.x, YY.y, YY.z, ZZ.x, ZZ.y, ZZ.z), dV_LVLH);
			V_CCC_apo = V_CCC + dV_NC;

			dt_PC = OrbMech::time_theta(R_CCC, V_CCC_apo, PI05, mu);

			OrbMech::oneclickcoast(R_CCC, V_CCC_apo, MJD, dt_PC, R_CPC, V_CPC, gravref, gravref);
			OrbMech::oneclickcoast(R_WCC, V_WCC, MJD, dt_PC, R_WPC, V_WPC, gravref, gravref);
			u2 = unit(crossp(V_WPC, R_WPC));

			Y_C = dotp(R_CPC, u2);
			if (p_PC == 0 || abs(Y_C) >= eps2)
			{
				OrbMech::ITER(c_PC, s_PC, Y_C, p_PC, dv_PC, eo_PC, dvo_PC);
				if (s_PC == 1)
				{
					return false;
				}
			}
		} while (abs(Y_C) >= eps2);
	}
	else
	{
		dV_LVLH = _V(dv_C, 0, 0);
	}
	if (s == 1)
	{
		OrbMech::RADUP(R_WC, V_WC, R_CH1, mu, R_WF, V_WF);
		dh_NC2 = length(R_WF) - length(R_CH1);
		dv_NC2 = dv_H1;
		dV_NC1_LVLH = dV_LVLH;
		t_NC2 = t_H1;
	}
	else
	{
		dV_NC2_LVLH = dV_LVLH;
	}
	dv_NCC = dv_H2;
	dV_NSR = V_CSF - V_CS;
	t_NCC = t_H2;
	t_NSR = t_S;
	Z = -unit(R_CS);
	Y = -am;
	X = crossp(Y, Z);
	dV_NSR_LVLH = mul(_M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z), dV_NSR);

	return true;
}

void RTCC::NCCProgram(SV sv_C, SV sv_W, double GETbase, double E_L, double t_C, double dt, double t_F, double dh, VECTOR3 &dV_NCC_LVLH, double &t_NSR, VECTOR3 &dV_NSR_LVLH)
{
	VECTOR3 R_CC, V_CC, R_WF, V_WF, R_WJ, V_WJ, R_CF, V_CF, R_CT, V_CT, V_CCF, R_CTF, V_CTF, X, Y, Z, dV_NCC, dV_NSR;
	double MJD, mu;
	OBJHANDLE gravref;

	gravref = sv_C.gravref;
	mu = GGRAV*oapiGetMass(gravref);
	MJD = GETbase + t_C / 24.0 / 3600.0;

	OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t_C, R_CC, V_CC, gravref, gravref);
	OrbMech::oneclickcoast(sv_W.R, sv_W.V, sv_W.MJD, (GETbase - sv_W.MJD)*24.0*3600.0 + t_F, R_WF, V_WF, gravref, gravref);
	OrbMech::QDRTPI(R_WF, V_WF, MJD, gravref, mu, dh, E_L, 1, R_WJ, V_WJ);
	OrbMech::COE(R_WJ, V_WJ, dh, mu, R_CF, V_CF);
	OrbMech::oneclickcoast(R_CF, V_CF, MJD, t_C + dt - t_F, R_CT, V_CT, gravref, gravref);
	V_CCF = OrbMech::Vinti(R_CC, V_CC, R_CT, MJD, dt, 0, true, gravref, gravref, gravref, _V(0, 0, 0));
	OrbMech::oneclickcoast(R_CC, V_CCF, sv_C.MJD, dt, R_CTF, V_CTF, gravref, gravref);

	dV_NCC = V_CCF - V_CC;
	dV_NSR = V_CT - V_CTF;

	Z = -unit(R_CTF);
	Y = unit(crossp(V_CTF, R_CTF));
	X = crossp(Y, Z);
	dV_NSR_LVLH = mul(_M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z), dV_NSR);

	Z = -unit(R_CC);
	Y = unit(crossp(V_CC, R_CC));
	X = crossp(Y, Z);
	dV_NCC_LVLH = mul(_M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z), dV_NCC);
}

void RTCC::NSRProgram(SV sv_C, SV sv_W, double GETbase, double E_L, double t2, double t3, VECTOR3 &dV_NSR_LVLH)
{
	VECTOR3 R_C2, V_C2, R_W2, V_W2, u1, u2, u, R_WC, V_WC, dV_2, X, Y, Z, V_C2_apo;
	double Y_C, Y_C_dot, Y_W_dot, cos_phi, sin_phi, SW, phi, dh_NSR, v_WV, a_W, a_C, v_CV, v_CH, mu, dt;
	OBJHANDLE gravref;

	gravref = sv_C.gravref;
	mu = GGRAV*oapiGetMass(gravref);

	OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t2, R_C2, V_C2, gravref, gravref);
	OrbMech::oneclickcoast(sv_W.R, sv_W.V, sv_W.MJD, (GETbase - sv_W.MJD)*24.0*3600.0 + t2, R_W2, V_W2, gravref, gravref);

	u1 = unit(crossp(V_C2, R_C2));
	u2 = unit(crossp(V_W2, R_W2));
	Y_C = dotp(R_C2, u2);
	Y_C_dot = dotp(V_C2, u2);
	Y_W_dot = dotp(V_W2, u1);
	
	u = unit(crossp(R_W2, V_W2));
	R_C2 = unit(R_C2 - u*dotp(R_C2, u))*length(R_C2);
	V_C2 = unit(V_C2 - u*dotp(V_C2, u))*length(V_C2);

	cos_phi = dotp(unit(R_C2), unit(R_W2));
	sin_phi = sqrt(1.0 - cos_phi*cos_phi);
	SW = dotp(u, crossp(R_W2, R_C2));

	phi = atan2(OrbMech::sign(SW)*sin_phi, cos_phi);
	dt = OrbMech::time_theta(R_W2, V_W2, phi, mu);
	OrbMech::rv_from_r0v0(R_W2, V_W2, dt, R_WC, V_WC, mu);

	dh_NSR = length(R_WC) - length(R_C2);
	v_WV = dotp(V_WC, unit(R_C2));
	a_W =  1.0 / (2.0 / length(R_W2) - dotp(V_W2, V_W2) / mu);
	a_C = a_W - dh_NSR;
	v_CV = v_WV*OrbMech::power(a_W / a_C, 1.5);
	v_CH = sqrt(mu*(2.0 / length(R_C2) - 1.0 / a_C) - v_CV*v_CV);
	V_C2_apo = unit(crossp(u, R_C2))*v_CH + unit(R_C2)*v_CV;
	dV_2 = V_C2_apo - V_C2;
	Z = unit(-R_C2);
	Y = -u;
	X = unit(crossp(Y, Z));
	
	dV_NSR_LVLH = mul(_M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z), dV_2) + _V(0.0, -Y_C_dot, 0.0);
}

void RTCC::NPCProgram(SV sv_C, SV sv_W, double GETbase, double t, double &t_NPC, VECTOR3 &dV_NPC_LVLH)
{
	VECTOR3 R_C2, V_C2, R_C3, V_C3, R_W3, V_W3, u1, u2;
	double mu, dt, Y_C, Y_C_dot, Y_W_dot, T_P;
	OBJHANDLE gravref;

	gravref = sv_C.gravref;
	mu = GGRAV*oapiGetMass(gravref);

	OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t, R_C2, V_C2, gravref, gravref);
	dt = OrbMech::time_theta(R_C2, V_C2, PI05*3.0, mu);
	T_P = OrbMech::period(R_C2, V_C2, mu);

	if (dt < 0)
	{
		dt = T_P + dt;
	}

	t_NPC = t + dt;

	OrbMech::oneclickcoast(R_C2, V_C2, sv_C.MJD, dt, R_C3, V_C3, gravref, gravref);
	OrbMech::oneclickcoast(sv_W.R, sv_W.V, sv_W.MJD, (GETbase - sv_W.MJD)*24.0*3600.0 + t_NPC, R_W3, V_W3, gravref, gravref);

	u1 = unit(crossp(V_C3, R_C3));
	u2 = unit(crossp(V_W3, R_W3));
	Y_C = dotp(R_C3, u2);
	Y_C_dot = dotp(V_C3, u2);
	Y_W_dot = dotp(V_W3, u1);

	dV_NPC_LVLH = _V(0, -Y_C_dot, abs(Y_C_dot) / tan(70.0*RAD));
}

bool RTCC::SkylabRendezvous(SkyRendOpt *opt, SkylabRendezvousResults *res)
{
	SV sv_C, sv_W, sv_tig;
	VECTOR3 dV_NC1, dV_LVLH, dV_NC2, dV_NCC, dV_NSR, dV_NPC;
	double LMmass, P30TIG, t_NPC;
	OBJHANDLE gravref;
	bool SolutionGood = true;	//True = Calculation successful, False = Calculation failed

	if (opt->useSV)
	{
		sv_C = opt->RV_MCC;
	}
	else
	{
		sv_C = StateVectorCalc(opt->vessel);
	}

	sv_W = StateVectorCalc(opt->target);
	gravref = sv_C.gravref;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0;
	}

	if (opt->man == 0)
	{
		SV sv_W1;
		double ttoMidnight;
		OBJHANDLE hSun;

		hSun = oapiGetObjectByName("Sun");

		sv_W1 = coast(sv_W, (opt->GETbase - sv_W.MJD)*24.0*3600.0 + opt->TPIGuess);

		ttoMidnight = OrbMech::sunrise(sv_W1.R, sv_W1.V, sv_W1.MJD, gravref, hSun, 1, 1, false);
		res->t_TPI = opt->TPIGuess + ttoMidnight;

		return true;
	}
	else if (opt->man == 1)	//NC1
	{
		SolutionGood = NC1NC2Program(sv_C, sv_W, opt->GETbase, opt->E_L, opt->t_C, 37.0*60.0, opt->t_TPI, opt->DH2, 0.5, 1, opt->DH1, opt->n_C, dV_NC1, res->dH_NC2, res->dv_NC2, res->t_NC2, dV_NC2, res->dv_NCC, res->t_NCC, res->t_NSR, res->dV_NSR, opt->NPCOption);
		
		if (SolutionGood)
		{
			dV_LVLH = dV_NC1;
			P30TIG = opt->t_C;
		}
	}
	else if (opt->man == 2)	//NC2
	{
		SolutionGood = NC1NC2Program(sv_C, sv_W, opt->GETbase, opt->E_L, opt->t_C, 37.0*60.0, opt->t_TPI, opt->DH2, 0.5, 2, opt->DH1, opt->n_C, dV_NC1, res->dH_NC2, res->dv_NC2, res->t_NC2, dV_NC2, res->dv_NCC, res->t_NCC, res->t_NSR, res->dV_NSR, opt->NPCOption);
		if (SolutionGood)
		{
			dV_LVLH = dV_NC2;
			P30TIG = opt->t_C;
		}
	}
	else if (opt->man == 3)	//NCC
	{
		NCCProgram(sv_C, sv_W, opt->GETbase, opt->E_L, opt->t_C, 37.0*60.0, opt->t_TPI, opt->DH2, dV_NCC, res->t_NSR, res->dV_NSR);
		
		dV_LVLH = dV_NCC;
		P30TIG = opt->t_C;
	}
	else if (opt->man == 4)	//NSR
	{
		NSRProgram(sv_C, sv_W, opt->GETbase, opt->E_L, opt->t_C, opt->t_TPI, dV_NSR);

		dV_LVLH = dV_NSR;
		P30TIG = opt->t_C;
	}
	else if (opt->man == 5)	//TPI
	{
		LambertMan lambert;
		VECTOR3 dV;
		double dt;

		dt = OrbMech::time_theta(sv_W.R, sv_W.V, 140.0*RAD, GGRAV*oapiGetMass(gravref));
		lambert = set_lambertoptions(sv_C, sv_W, opt->GETbase, opt->t_C, opt->t_C + dt, 0, RTCC_LAMBERT_MULTIAXIS, RTCC_LAMBERT_PERTURBED, _V(0, 0, 0), 0);
		LambertTargeting(&lambert, dV, dV_LVLH);

		res->dV_LVLH = dV_LVLH;
		res->P30TIG = opt->t_C;

		return true;
	}
	else if (opt->man == 6)	//TPM
	{
		LambertMan lambert;
		VECTOR3 dV;
		double dt;

		dt = OrbMech::time_theta(sv_W.R, sv_W.V, 140.0*RAD, GGRAV*oapiGetMass(gravref));
		lambert = set_lambertoptions(sv_C, sv_W, opt->GETbase, opt->t_C, opt->t_TPI + dt, 0, RTCC_LAMBERT_MULTIAXIS, RTCC_LAMBERT_PERTURBED, _V(0, 0, 0), 0);
		LambertTargeting(&lambert, dV, dV_LVLH);

		res->dV_LVLH = dV_LVLH;
		res->P30TIG = opt->t_C;

		return true;
	}
	else if (opt->man == 7)	//NPC
	{
		NPCProgram(sv_C, sv_W, opt->GETbase, opt->t_NC, t_NPC, dV_NPC);

		dV_LVLH = dV_NPC;
		P30TIG = t_NPC;
	}

	if (SolutionGood == false)
	{
		return false;
	}

	VECTOR3 R_cor, V_cor, Llambda, DVX;
	MATRIX3 Q_Xx;
	double t_slip;

	sv_tig = coast(sv_C, (opt->GETbase - sv_C.MJD)*24.0*3600.0 + P30TIG);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	DVX = tmul(Q_Xx, dV_LVLH);

	FiniteBurntimeCompensation(RTCC_VESSELTYPE_CSM, sv_tig, LMmass, DVX, 1, Llambda, t_slip);

	OrbMech::oneclickcoast(sv_tig.R, sv_tig.V, sv_tig.MJD, t_slip, R_cor, V_cor, gravref, gravref);

	Q_Xx = OrbMech::LVLH_Matrix(R_cor, V_cor);

	res->P30TIG = P30TIG + t_slip;
	res->dV_LVLH = mul(Q_Xx, Llambda);

	return true;
}

void RTCC::TLIFirstGuessConic(SV sv_mcc, double lat_EMP, double h_peri, double MJD_P, VECTOR3 &DV, VECTOR3 &var_converged)
{
	SV sv_p;
	MATRIX3 M_EMP;
	VECTOR3 R_EMP, V_EMP, var_fg, V_MCC_apo;
	double r_peri, mu, ddt, R_E, lng_peri, azi_peri, v_peri;
	OBJHANDLE hMoon, hEarth;
	OELEMENTS coe;
	double rtest, lngtest, lattest, fpatest;

	R_E = 6378.137e3;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	mu = GGRAV*oapiGetMass(hMoon);

	r_peri = oapiGetSize(hMoon) + h_peri;
	sv_p.MJD = MJD_P;
	sv_p.gravref = hMoon;

	//Initial guess
	var_fg = TLMCEmpiricalFirstGuess(r_peri, 0.0);
	lng_peri = var_fg.z;
	azi_peri = var_fg.y;
	v_peri = var_fg.x;

	//Conic first guess seems to break down above 1500NM pericynthion altitude
	if (r_peri > 4.5e6)
	{
		var_converged = var_fg;
		return;
	}

	do
	{
		//Position vector in EMP Coordinates
		OrbMech::adbar_from_rv(r_peri, v_peri, lng_peri, lat_EMP, PI05, azi_peri, R_EMP, V_EMP);

		//EMP Matrix
		M_EMP = OrbMech::EMPMatrix(sv_p.MJD);

		//Convert EMP position to ecliptic
		sv_p.R = tmul(M_EMP, R_EMP);
		sv_p.V = tmul(M_EMP, V_EMP);

		//Calculate pericynthion velocity
		OrbMech::ThirdBodyConic(sv_p.R, hMoon, sv_mcc.R, hEarth, sv_p.MJD, (sv_mcc.MJD - sv_p.MJD)*24.0*3600.0, sv_p.V, sv_p.V, V_MCC_apo, 100.0);

		//save azi and vmag as new initial guess
		V_EMP = mul(M_EMP, sv_p.V);
		OrbMech::rv_from_adbar(R_EMP, V_EMP, rtest, v_peri, lngtest, lattest, fpatest, azi_peri);

		coe = OrbMech::coe_from_sv(sv_p.R, sv_p.V, mu);
		ddt = OrbMech::timetoperi(sv_p.R, sv_p.V, mu);

		if (coe.TA > PI)
		{
			lng_peri += coe.TA - PI2;
		}
		else
		{
			lng_peri += coe.TA;
		}
	} while (abs(ddt) > 0.01);

	DV = V_MCC_apo - sv_mcc.V;

	var_converged = _V(v_peri, azi_peri, lng_peri);
}

VECTOR3 RTCC::TLMCEmpiricalFirstGuess(double r, double dt)
{
	double lambda, psi, V, R_E;

	R_E = 6378.137e3;

	lambda = PI - 0.025*dt*3600.0;
	psi = 270.0*RAD;
	V = sqrt(0.184 + 0.553 / (r / R_E))*R_E / 3600.0 - 0.0022*dt*3600.0;

	return _V(V, psi, lambda);
}

bool RTCC::TLIFlyby(SV sv_TLI, double lat_EMP, double h_peri, SV sv_peri_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry)
{
	SV sv_TLI_apo, sv_p, sv_r;
	MATRIX3 M_EMP;
	VECTOR3 R_EMP, V_EMP;
	double dt, mu, ddt, VacPeri, R_E, e_H, e_Ho, c_I, p_H, eps2, dto, r_peri, lngtest, lattest, fpatest, rtest, v_peri, lng_EMP, azi_peri;
	int s_F;
	OBJHANDLE hMoon, hEarth;
	OELEMENTS coe;

	c_I = p_H = dto = 0.0;
	eps2 = 0.1;
	s_F = 0;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu = GGRAV*oapiGetMass(hMoon);
	R_E = oapiGetSize(hEarth);

	sv_p.gravref = hMoon;
	sv_r.gravref = hEarth;

	r_peri = oapiGetSize(hMoon) + h_peri;
	dt = (sv_peri_guess.MJD - sv_TLI.MJD)*24.0*3600.0;

	M_EMP = OrbMech::EMPMatrix(sv_peri_guess.MJD);
	R_EMP = mul(M_EMP, sv_peri_guess.R);
	V_EMP = mul(M_EMP, sv_peri_guess.V);
	OrbMech::rv_from_adbar(R_EMP, V_EMP, rtest, v_peri, lng_EMP, lattest, fpatest, azi_peri);

	do
	{
		sv_p.MJD = sv_TLI.MJD + dt / 24.0 / 3600.0;

		do
		{
			//Position vector in EMP Coordinates
			OrbMech::adbar_from_rv(r_peri, v_peri, lng_EMP, lat_EMP, PI05, azi_peri, R_EMP, V_EMP);

			//EMP Matrix
			M_EMP = OrbMech::EMPMatrix(sv_p.MJD);

			//Convert EMP position to ecliptic
			sv_p.R = tmul(M_EMP, R_EMP);
			sv_p.V = tmul(M_EMP, V_EMP);

			//Calculate pericynthion velocity
			sv_p.V = OrbMech::Vinti(sv_p.R, _V(0.0, 0.0, 0.0), sv_TLI.R, sv_p.MJD, -dt, 0, false, hMoon, hMoon, sv_TLI.gravref, sv_p.V);

			//save azi and vmag as new initial guess
			V_EMP = mul(M_EMP, sv_p.V);
			OrbMech::rv_from_adbar(R_EMP, V_EMP, rtest, v_peri, lngtest, lattest, fpatest, azi_peri);

			coe = OrbMech::coe_from_sv(sv_p.R, sv_p.V, mu);
			ddt = OrbMech::timetoperi(sv_p.R, sv_p.V, mu);

			if (coe.TA > PI)
			{
				lng_EMP += coe.TA - PI2;
			}
			else
			{
				lng_EMP += coe.TA;
			}
		} while (abs(ddt) > 0.01);

		OrbMech::ReturnPerigee(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, 1.0, sv_r.MJD, sv_r.R, sv_r.V);
		VacPeri = length(sv_r.R);

		//20NM vacuum perigee
		e_H = VacPeri - R_E - 20.0*1852.0;

		if (p_H == 0 || abs(dt - dto) >= eps2)
		{
			OrbMech::ITER(c_I, s_F, e_H, p_H, dt, e_Ho, dto);
			if (s_F == 1)
			{
				return false;
			}
		}
	} while (abs(dt - dto) >= eps2);

	sv_TLI_apo = coast(sv_p, -dt);

	DV = sv_TLI_apo.V - sv_TLI.V;
	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

bool RTCC::TLMCFlyby(SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry)
{
	SV sv_p, sv_r;
	OBJHANDLE hMoon, hEarth;
	VECTOR3 step, target;
	double dt1;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	target = _V(h_peri, lat_EMP, 20.0*1852.0);
	step = _V(10e-3, 10e-3, 10e-3);

	sv_p.gravref = hMoon;
	sv_r.gravref = hEarth;

	DV = OrbMech::TLMCIntegratedFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, target, step);

	dt1 = OrbMech::timetoperi_integ(sv_mcc.R, sv_mcc.V + DV, sv_mcc.MJD, sv_mcc.gravref, hMoon, sv_p.R, sv_p.V);
	sv_p.MJD = sv_mcc.MJD + dt1 / 24.0 / 3600.0;
	OrbMech::ReturnPerigee(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, 1.0, sv_r.MJD, sv_r.R, sv_r.V);

	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

bool RTCC::TLMCConic_BAP_FR_LPO(MCCFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, SV &sv_reentry, double &lat_EMPcor)
{
	SV sv_p, sv_n, sv_r;
	LOIMan loiopt;
	VECTOR3 target, DV_LOI, DV_MCC, step;
	double TIG_LOI, dV_T, lat_iter, mu_E, mu_M;
	double latarr[3], dvarr[3];
	int s_F, c_I;
	OBJHANDLE hEarth, hMoon;
	double dt1, dt2;
	VECTOR3 R_patch, V_patch;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);
	mu_M = GGRAV*oapiGetMass(hMoon);

	loiopt.alt = opt->alt;
	loiopt.azi = opt->azi;
	loiopt.csmlmdocked = opt->csmlmdocked;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.t_land = opt->t_land;
	loiopt.useSV = true;
	loiopt.vessel = opt->vessel;
	loiopt.impulsive = RTCC_IMPULSIVE;
	loiopt.type = opt->type;

	sv_p.gravref = hMoon;
	sv_p.mass = sv_mcc.mass;

	DV_MCC = DV_guess;
	lat_iter = lat_EMP;

	step = _V(10e-3, 10e-3, 10e-3);
	dvarr[0] = dvarr[1] = dvarr[2] = 0.0;
	c_I = s_F = 0;

	do
	{
		target = _V(h_peri, lat_iter, 20.0*1852.0);
		DV_MCC = OrbMech::TLMCConicFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_MCC, target, step);

		if (sv_mcc.gravref == hEarth)
		{
			dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV_MCC, sv_mcc.MJD, mu_E, mu_M, R_patch, V_patch);
		}
		else
		{
			R_patch = sv_mcc.R;
			V_patch = sv_mcc.V + DV_MCC;
			dt1 = 0.0;
		}

		dt2 = OrbMech::timetoperi(R_patch, V_patch, mu_M);
		OrbMech::rv_from_r0v0(R_patch, V_patch, dt2, sv_p.R, sv_p.V, mu_M);
		sv_p.MJD = sv_mcc.MJD + (dt1 + dt2) / 24.0 / 3600.0;

		loiopt.RV_MCC = sv_p;
		LOITargeting(&loiopt, DV_LOI, TIG_LOI, sv_n);

		dV_T = length(DV_LOI) + length(DV_MCC);
		OrbMech::QuadraticIterator(c_I, s_F, lat_iter, latarr, dvarr, dV_T, 0.02*RAD, 0.2*RAD);
		if (s_F == 1)
		{
			return false;
		}

	} while (c_I < 3 || abs(dvarr[2] - dvarr[1]) > 0.01);

	DV = DV_MCC;
	lat_EMPcor = lat_iter;
	sv_peri = sv_p;
	sv_node = sv_n;

	return true;
}

bool RTCC::TLMC_BAP_FR_LPO(MCCFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, SV &sv_reentry, double &lat_EMPcor)
{
	SV sv_p, sv_r, sv_n;
	LOIMan loiopt;
	VECTOR3 target, DV_LOI, DV_MCC, step;
	double TIG_LOI, dV_T, lat_iter, mu_E, mu_M;
	double latarr[3], dvarr[3];
	int s_F, c_I;
	OBJHANDLE hEarth, hMoon;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);
	mu_M = GGRAV*oapiGetMass(hMoon);

	loiopt.alt = opt->alt;
	loiopt.azi = opt->azi;
	loiopt.csmlmdocked = opt->csmlmdocked;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.t_land = opt->t_land;
	loiopt.useSV = true;
	loiopt.vessel = opt->vessel;
	loiopt.impulsive = RTCC_IMPULSIVE;
	loiopt.type = opt->type;

	loiopt.RV_MCC.gravref = sv_mcc.gravref;
	loiopt.RV_MCC.R = sv_mcc.R;
	loiopt.RV_MCC.mass = sv_mcc.mass;
	loiopt.RV_MCC.MJD = sv_mcc.MJD;

	DV_MCC = DV_guess;
	lat_iter = lat_EMP;

	step = _V(10e-3, 10e-3, 10e-3);
	dvarr[0] = dvarr[1] = dvarr[2] = 0.0;
	c_I = s_F = 0;

	do
	{
		target = _V(h_peri, lat_iter, 20.0*1852.0);
		DV_MCC = OrbMech::TLMCIntegratedFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_MCC, target, step);

		loiopt.RV_MCC.V = sv_mcc.V + DV_MCC;
		LOITargeting(&loiopt, DV_LOI, TIG_LOI, sv_n);

		dV_T = length(DV_LOI) + length(DV_MCC);
		OrbMech::QuadraticIterator(c_I, s_F, lat_iter, latarr, dvarr, dV_T, 0.02*RAD, 0.2*RAD);
		if (s_F == 1)
		{
			return false;
		}

	} while (c_I < 3 || abs(dvarr[2] - dvarr[1]) > 0.01);

	double dt1;

	dt1 = OrbMech::timetoperi_integ(sv_mcc.R, sv_mcc.V + DV_MCC, sv_mcc.MJD, sv_mcc.gravref, hMoon, sv_p.R, sv_p.V);
	sv_p.MJD = sv_mcc.MJD + dt1 / 24.0 / 3600.0;
	sv_p.gravref = hMoon;

	OrbMech::ReturnPerigee(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, 1.0, sv_r.MJD, sv_r.R, sv_r.V);
	sv_p.gravref = hEarth;

	DV = DV_MCC;
	lat_EMPcor = lat_iter;
	sv_peri = sv_p;
	sv_reentry = sv_r;
	sv_node = sv_n;

	return true;
}

bool RTCC::TLMCConic_BAP_NFR_LPO(MCCNFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, double MJD_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, double &lat_EMPcor)
{
	SV sv_p, sv_mcc_apo, sv_n;
	VECTOR3 R_EMP, V_EMP, DV_LOI, var_converged, RP_M, VP_M, var_guess, DV_MCC, step;
	double lng_peri_guess, dt, dt1, mu_E, mu_M, TIG_LOI, dV_T, r_peri, v_peri_guess, azi_peri_guess, v_peri_conv, azi_peri_conv, lng_peri_conv;
	double rtemp, lattemp, fpatemp, lat_iter;
	double dLat, dV_To;
	int c_I;
	OBJHANDLE hMoon, hEarth;
	LOIMan loiopt;

	loiopt.alt = opt->alt;
	loiopt.azi = opt->azi;
	loiopt.csmlmdocked = opt->csmlmdocked;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.t_land = opt->t_land;
	loiopt.useSV = true;
	loiopt.vessel = opt->vessel;
	loiopt.impulsive = RTCC_IMPULSIVE;
	loiopt.type = opt->type;

	c_I = 0;
	dLat = 0.0;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	mu_M = GGRAV*oapiGetMass(hMoon);
	mu_E = GGRAV*oapiGetMass(hEarth);

	sv_p.mass = sv_mcc.mass;
	sv_p.gravref = hMoon;

	sv_mcc_apo = sv_mcc;
	sv_mcc_apo.V = sv_mcc.V + DV_guess;

	r_peri = oapiGetSize(hMoon) + h_peri;
	lat_iter = lat_EMP;
	step = _V(10e-3, 10e-3, 10e-3);

	dt = (MJD_peri - sv_mcc.MJD)*24.0*3600.0;

	dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV_guess, sv_mcc.MJD, mu_E, mu_M, RP_M, VP_M);
	OrbMech::rv_from_r0v0(RP_M, VP_M, dt - dt1, sv_p.R, sv_p.V, mu_M);
	sv_p.MJD = sv_mcc.MJD + dt / 24.0 / 3600.0;

	OrbMech::EclToEMP(sv_p.R, sv_p.V, sv_p.MJD, R_EMP, V_EMP);
	OrbMech::rv_from_adbar(R_EMP, V_EMP, rtemp, v_peri_guess, lng_peri_guess, lattemp, fpatemp, azi_peri_guess);

	var_guess = _V(v_peri_guess, azi_peri_guess, lng_peri_guess);

	do
	{
		var_converged = OrbMech::TLMCConicFirstGuessIterator(r_peri, lat_iter, 0.0, var_guess, sv_mcc.R, step, MJD_peri, -dt, hMoon, sv_mcc.gravref);

		v_peri_conv = var_converged.x;
		azi_peri_conv = var_converged.y;
		lng_peri_conv = var_converged.z;

		var_guess = var_converged;

		//Position and velocity vector in EMP Coordinates
		OrbMech::adbar_from_rv(r_peri, v_peri_conv, lng_peri_conv, lat_iter, PI05, azi_peri_conv, R_EMP, V_EMP);

		//Convert EMP position to ecliptic
		OrbMech::EMPToEcl(R_EMP, V_EMP, sv_p.MJD, sv_p.R, sv_p.V);

		OrbMech::rv_from_r0v0_tb(sv_p.R, sv_p.V, sv_p.MJD, hMoon, sv_mcc.gravref, -dt, sv_mcc_apo.R, sv_mcc_apo.V);

		DV_MCC = sv_mcc_apo.V - sv_mcc.V;

		loiopt.RV_MCC = sv_p;

		LOITargeting(&loiopt, DV_LOI, TIG_LOI, sv_n);

		dV_T = length(DV_LOI) + length(DV_MCC);

		if (c_I == 0)
		{
			dLat = -1.0*RAD;
		}
		else
		{
			if (dV_T > dV_To)
			{
				dLat = -dLat / 2.0;
			}
		}

		lat_iter += dLat;
		dV_To = dV_T;
		c_I++;

		//dvar = OrbMech::QuadraticIterator(c_I, s_F, lat_EMPcor, latarr, dvarr, dV_T, 0.1*RAD, 1.0*RAD);
		//if (s_F == 1)
		//{
		//	return false;
		//}
		//} while (c_I < 3 || abs(dvarr[2] - dvarr[1]) > 0.01);
	} while (abs(dLat) > 0.01*RAD);

	DV = DV_MCC;
	lat_EMPcor = lat_iter;
	sv_peri = sv_p;
	sv_node = sv_n;

	return true;
}

bool RTCC::TLMC_BAP_NFR_LPO(MCCNFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, double MJD_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, double &lat_EMPcor)
{
	SV sv_p, sv_mcc_apo, sv_n;
	VECTOR3 R_EMP, V_EMP, DV_LOI, var_converged, var_guess, DV_MCC, step;
	double lng_peri_guess, dt, mu_E, mu_M, TIG_LOI, dV_T, r_peri, v_peri_guess, azi_peri_guess, v_peri_conv, azi_peri_conv, lng_peri_conv;
	double rtemp, lattemp, fpatemp, lat_iter;
	double dLat, dV_To;
	int c_I;
	OBJHANDLE hMoon, hEarth;
	LOIMan loiopt;

	loiopt.alt = opt->alt;
	loiopt.azi = opt->azi;
	loiopt.csmlmdocked = opt->csmlmdocked;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.t_land = opt->t_land;
	loiopt.useSV = true;
	loiopt.vessel = opt->vessel;
	loiopt.impulsive = RTCC_IMPULSIVE;
	loiopt.type = opt->type;

	c_I = 0;
	dLat = 0.0;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	mu_M = GGRAV*oapiGetMass(hMoon);
	mu_E = GGRAV*oapiGetMass(hEarth);

	loiopt.RV_MCC.mass = sv_mcc.mass;
	loiopt.RV_MCC.gravref = hMoon;

	sv_mcc_apo = sv_mcc;
	sv_mcc_apo.V = sv_mcc.V + DV_guess;

	r_peri = oapiGetSize(hMoon) + h_peri;
	lat_iter = lat_EMP;
	step = _V(10e-3, 10e-3, 10e-3);

	dt = (MJD_peri - sv_mcc.MJD)*24.0*3600.0;

	sv_p = coast(sv_mcc_apo, dt);

	//dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV_guess, sv_mcc.MJD, mu_E, mu_M, RP_M, VP_M);
	//OrbMech::rv_from_r0v0(RP_M, VP_M, dt - dt1, sv_p.R, sv_p.V, mu_M);
	//sv_p.MJD = sv_mcc.MJD + dt / 24.0 / 3600.0;

	OrbMech::EclToEMP(sv_p.R, sv_p.V, sv_p.MJD, R_EMP, V_EMP);
	OrbMech::rv_from_adbar(R_EMP, V_EMP, rtemp, v_peri_guess, lng_peri_guess, lattemp, fpatemp, azi_peri_guess);

	var_guess = _V(v_peri_guess, azi_peri_guess, lng_peri_guess);

	do
	{
		var_converged = OrbMech::IntegratedTLMCIterator(r_peri, lat_iter, PI05, var_guess, sv_mcc.R, step, MJD_peri, -dt, hMoon, sv_mcc.gravref);

		v_peri_conv = var_converged.x;
		azi_peri_conv = var_converged.y;
		lng_peri_conv = var_converged.z;

		var_guess = var_converged;

		//Position and velocity vector in EMP Coordinates
		OrbMech::adbar_from_rv(r_peri, v_peri_conv, lng_peri_conv, lat_iter, PI05, azi_peri_conv, R_EMP, V_EMP);

		//Convert EMP position to ecliptic
		OrbMech::EMPToEcl(R_EMP, V_EMP, sv_p.MJD, sv_p.R, sv_p.V);

		sv_mcc_apo = coast(sv_p, -dt);
		DV_MCC = sv_mcc_apo.V - sv_mcc.V;

		loiopt.RV_MCC = sv_p;

		LOITargeting(&loiopt, DV_LOI, TIG_LOI, sv_n);

		dV_T = length(DV_LOI) + length(DV_MCC);

		if (c_I == 0)
		{
			dLat = -1.0*RAD;
		}
		else
		{
			if (dV_T > dV_To)
			{
				dLat = -dLat / 2.0;
			}
		}

		lat_EMPcor += dLat;
		dV_To = dV_T;
		c_I++;

		//dvar = OrbMech::QuadraticIterator(c_I, s_F, lat_EMPcor, latarr, dvarr, dV_T, 0.1*RAD, 1.0*RAD);
		//if (s_F == 1)
		//{
		//	return false;
		//}
		//} while (c_I < 3 || abs(dvarr[2] - dvarr[1]) > 0.01);
	} while (abs(dLat) > 0.01*RAD);

	DV = DV_MCC;
	lat_EMPcor = lat_iter;
	sv_peri = sv_p;
	sv_node = sv_n;

	return true;
}

void RTCC::TLMCFirstGuess(SV sv_mcc, double lat_EMP, double h_peri, double MJD_P, VECTOR3 &DV, SV &sv_peri)
{
	VECTOR3 var_guess, var_converged;
	double r_peri;
	OBJHANDLE hMoon;

	hMoon = oapiGetObjectByName("Moon");
	r_peri = oapiGetSize(hMoon) + h_peri;
	var_guess = TLMCEmpiricalFirstGuess(r_peri, 0.0);

	IntegratedTLMC(sv_mcc, lat_EMP, h_peri, 0.0, MJD_P, var_guess, DV, var_converged, sv_peri);
}

void RTCC::IntegratedTLMC(SV sv_mcc, double lat, double h, double gamma, double MJD, VECTOR3 var_guess, VECTOR3 &DV, VECTOR3 &var_converged, SV &sv_node)
{
	SV sv_n, sv_mcc_apo;
	OBJHANDLE hMoon;
	VECTOR3 R_EMP, V_EMP, step;
	double r_peri, dt, v_peri_conv, azi_peri_conv, lng_peri_conv;

	hMoon = oapiGetObjectByName("Moon");
	r_peri = oapiGetSize(hMoon) + h;
	sv_n.MJD = MJD;
	sv_n.gravref = hMoon;
	dt = (sv_n.MJD - sv_mcc.MJD)*24.0*3600.0;
	step = _V(10e-3, 10e-3, 10e-3);//_V(pow(2.0, -19.0)*1769.7, pow(2.0, -19.0)*RAD, pow(2.0, -19.0));

	var_converged = OrbMech::IntegratedTLMCIterator(r_peri, lat, gamma, var_guess, sv_mcc.R, step, MJD, -dt, hMoon, sv_mcc.gravref);

	v_peri_conv = var_converged.x;
	azi_peri_conv = var_converged.y;
	lng_peri_conv = var_converged.z;

	OrbMech::adbar_from_rv(r_peri, v_peri_conv, lng_peri_conv, lat, gamma + PI05, azi_peri_conv, R_EMP, V_EMP);
	OrbMech::EMPToEcl(R_EMP, V_EMP, sv_n.MJD, sv_n.R, sv_n.V);
	sv_mcc_apo = coast(sv_n, -dt);

	DV = sv_mcc_apo.V - sv_mcc.V;

	sv_node = sv_n;
}

void RTCC::TLMCIntegratedXYZT(SV sv_mcc, double lat_node, double lng_node, double h_node, double MJD_node, VECTOR3 DV_guess, VECTOR3 &DV)
{
	SV sv_p;
	VECTOR3 step;
	double dt;

	dt = (MJD_node - sv_mcc.MJD)*24.0*3600.0;
	step = _V(10e-3, 10e-3, 10e-3);

	DV = OrbMech::TLMCIntegratedXYZTIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, _V(h_node, lat_node, lng_node), step, dt);
}

void RTCC::TLMCFirstGuessConic(SV sv_mcc, double lat, double h, double gamma, double MJD_P, VECTOR3 &DV, VECTOR3 &var_converged)
{
	SV sv_p;
	VECTOR3 R_EMP, V_EMP, R_MCC, V_MCC_apo, var_guess, step;
	double r_peri, R_E, lng_peri_conv, azi_peri_conv, v_peri_conv, dt;
	OBJHANDLE hMoon, hEarth;

	R_E = 6378.137e3;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	//mu = GGRAV*oapiGetMass(hMoon);

	r_peri = oapiGetSize(hMoon) + h;
	sv_p.MJD = MJD_P;
	dt = (MJD_P - sv_mcc.MJD)*24.0*3600.0;
	step = _V(10e-3, 10e-3, 10e-3);

	//Initial guess
	var_guess = TLMCEmpiricalFirstGuess(r_peri, 0.0);

	var_converged = OrbMech::TLMCConicFirstGuessIterator(r_peri, lat, gamma, var_guess, sv_mcc.R, step, MJD_P, -dt, hMoon, sv_mcc.gravref);

	v_peri_conv = var_converged.x;
	azi_peri_conv = var_converged.y;
	lng_peri_conv = var_converged.z;

	OrbMech::adbar_from_rv(r_peri, v_peri_conv, lng_peri_conv, lat, gamma + PI05, azi_peri_conv, R_EMP, V_EMP);
	OrbMech::EMPToEcl(R_EMP, V_EMP, sv_p.MJD, sv_p.R, sv_p.V);
	OrbMech::rv_from_r0v0_tb(sv_p.R, sv_p.V, sv_p.MJD, hMoon, sv_mcc.gravref, -dt, R_MCC, V_MCC_apo);

	DV = V_MCC_apo - sv_mcc.V;
}

bool RTCC::TLMCFlybyConic(SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry)
{
	SV sv_p, sv_r;
	VECTOR3 target, R_patch, V_patch, step;
	double mu_M, mu_E, dt1, dt2;
	OBJHANDLE hEarth, hMoon;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);
	mu_M = GGRAV*oapiGetMass(hMoon);

	target = _V(h_peri, lat_EMP, 20.0*1852.0);
	step = _V(10e-3, 10e-3, 10e-3);

	DV = OrbMech::TLMCConicFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, target, step);

	if (sv_mcc.gravref == hEarth)
	{
		dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV, sv_mcc.MJD, mu_E, mu_M, R_patch, V_patch);
	}
	else
	{
		R_patch = sv_mcc.R;
		V_patch = sv_mcc.V + DV;
		dt1 = 0.0;
	}

	dt2 = OrbMech::timetoperi(R_patch, V_patch, mu_M);
	OrbMech::rv_from_r0v0(R_patch, V_patch, dt2, sv_p.R, sv_p.V, mu_M);
	sv_p.MJD = sv_mcc.MJD + (dt1 + dt2) / 24.0 / 3600.0;

	OrbMech::ReturnPerigeeConic(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, sv_r.MJD, sv_r.R, sv_r.V);

	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

bool RTCC::TLMCConicFlybyToInclinationSubprocessor(SV sv_mcc, double h_peri, double inc_fr_des, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry, double &lat_EMP)
{
	SV sv_p, sv_r;
	VECTOR3 target, R_patch, V_patch, R_EMP, V_EMP, step;
	double mu_M, mu_E, dt1, dt2, lng_EMP;
	OBJHANDLE hEarth, hMoon;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);
	mu_M = GGRAV*oapiGetMass(hMoon);

	sv_p.gravref = hMoon;
	sv_r.gravref = hEarth;

	target = _V(h_peri, 20.0*1852.0, inc_fr_des);
	step = _V(10e-3, 10e-3, 10e-3);

	DV = OrbMech::TLMCConicSPSLunarFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, target, step);

	if (sv_mcc.gravref == hEarth)
	{
		dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV, sv_mcc.MJD, mu_E, mu_M, R_patch, V_patch);
	}
	else
	{
		R_patch = sv_mcc.R;
		V_patch = sv_mcc.V + DV;
		dt1 = 0.0;
	}

	dt2 = OrbMech::timetoperi(R_patch, V_patch, mu_M);
	OrbMech::rv_from_r0v0(R_patch, V_patch, dt2, sv_p.R, sv_p.V, mu_M);

	sv_p.MJD = sv_mcc.MJD + (dt1 + dt2) / 24.0 / 3600.0;

	OrbMech::EclToEMP(sv_p.R, sv_p.V, sv_p.MJD, R_EMP, V_EMP);
	OrbMech::latlong_from_r(R_EMP, lat_EMP, lng_EMP);

	OrbMech::ReturnPerigeeConic(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, sv_r.MJD, sv_r.R, sv_r.V);

	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

bool RTCC::TLMCIntegratedFlybyToInclinationSubprocessor(SV sv_mcc, double h_peri, double inc_fr_des, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry, double &lat_EMP)
{
	SV sv_p, sv_r;
	VECTOR3 target, R_EMP, V_EMP, step;
	double mu_M, mu_E, dt1, lng_EMP;
	OBJHANDLE hEarth, hMoon;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);
	mu_M = GGRAV*oapiGetMass(hMoon);

	sv_p.gravref = hMoon;
	sv_r.gravref = hEarth;

	target = _V(h_peri, 20.0*1852.0, inc_fr_des);
	step = _V(10e-3, 10e-3, 10e-3);

	DV = OrbMech::TLMCIntegratedSPSLunarFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, target, step);

	dt1 = OrbMech::timetoperi_integ(sv_mcc.R, sv_mcc.V + DV, sv_mcc.MJD, sv_mcc.gravref, hMoon, sv_p.R, sv_p.V);
	sv_p.MJD = sv_mcc.MJD + dt1 / 24.0 / 3600.0;

	OrbMech::EclToEMP(sv_p.R, sv_p.V, sv_p.MJD, R_EMP, V_EMP);
	OrbMech::latlong_from_r(R_EMP, lat_EMP, lng_EMP);

	OrbMech::ReturnPerigee(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, 1.0, sv_r.MJD, sv_r.R, sv_r.V);

	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

void RTCC::LaunchTimePredictionProcessor(LunarLiftoffTimeOpt *opt, LunarLiftoffResults *res)
{
	VECTOR3 R_LS;
	double lat, lng, r, dt_1, h_1, theta_1, theta_Ins, v_LV, v_LH, DH, E, theta_F, t_TPI, t_IG, t_CSI, t_CDH, t_TPF;
	SV sv_P, sv_TPI;
	OBJHANDLE hMoon;

	hMoon = oapiGetObjectByName("Moon");

	if (opt->useSV)
	{
		sv_P = opt->RV_MCC;
	}
	else
	{
		sv_P = StateVectorCalc(opt->target);
	}

	theta_F = 130.0*RAD;
	dt_1 = 7.0*60.0 + 15.0;
	h_1 = 60000.0*0.3048;
	theta_1 = 10.0*RAD;
	theta_Ins = 17.0*RAD;
	DH = 15.0*1852.0;
	E = 26.6*RAD;

	t_CSI = 0;
	t_CDH = 0;

	opt->vessel->GetEquPos(lng, lat, r);

	R_LS = OrbMech::r_from_latlong(lat, lng, r);

	if (opt->opt == 0)
	{
		double ttoMidnight;
		OBJHANDLE hSun;

		hSun = oapiGetObjectByName("Sun");

		sv_TPI = coast(sv_P, (opt->GETbase - sv_P.MJD)*24.0*3600.0 + opt->t_TPIguess);

		ttoMidnight = OrbMech::sunrise(sv_TPI.R, sv_TPI.V, sv_TPI.MJD, hMoon, hSun, 1, 1, false);
		t_TPI = opt->t_TPIguess + ttoMidnight;

		OrbMech::LunarLiftoffTimePredictionCFP(R_LS, sv_P.R, sv_P.V, sv_P.MJD, opt->GETbase, hMoon, dt_1, h_1, theta_1, theta_Ins, DH, E, t_TPI, theta_F, t_IG, t_CSI, t_CDH, t_TPF, v_LH, v_LV);
	}
	else
	{
		t_TPI = opt->t_TPIguess;

		OrbMech::LunarLiftoffTimePredictionDT(R_LS, sv_P.R, sv_P.V, sv_P.MJD, opt->GETbase, hMoon, dt_1, h_1, theta_1, theta_Ins, DH, E, t_TPI, theta_F, t_IG, t_TPF, v_LH, v_LV);
	}
	res->t_L = t_IG;
	res->t_Ins = t_IG + dt_1;
	res->t_CSI = t_CSI;
	res->t_CDH = t_CDH;
	res->t_TPI = t_TPI;
	res->t_TPF = t_TPF;
	res->v_LH = v_LH;
	res->v_LV = v_LV;
}

void RTCC::EntryUpdateCalc(SV sv0, double GETbase, double entryrange, bool highspeed, EntryResults *res)
{
	VECTOR3 REI, VEI, R3, V3, UR3, U_H3, U_LS, LSEF;
	double RCON, mu, dt2, t32, v3, S_FPA, gammaE, phie, te, t_LS, Sphie, Cphie, tLSMJD, l, m, n, phi, lambda2, EntryInterface;
	MATRIX3 R;
	OBJHANDLE hEarth;
	VECTOR3 R05G, V05G, UREI;
	double dt22, t2, EMSAlt, vEI;
	int rangeiter;

	if (entryrange == 0)
	{
		rangeiter = 1;
	}
	else
	{
		rangeiter = 2;
	}

	if (highspeed)
	{
		EMSAlt = 297431.0*0.3048;
	}
	else
	{
		EMSAlt = 284643.0*0.3048;
	}

	hEarth = oapiGetObjectByName("Earth");

	EntryInterface = 400000.0 * 0.3048;
	RCON = oapiGetSize(hEarth) + EntryInterface;
	mu = GGRAV*oapiGetMass(hEarth);

	dt2 = OrbMech::time_radius_integ(sv0.R, sv0.V, sv0.MJD, RCON, -1, sv0.gravref, hEarth, REI, VEI);

	UREI = unit(REI);
	vEI = length(VEI);
	res->ReA = asin(dotp(UREI, VEI) / vEI);

	t32 = OrbMech::time_radius(REI, VEI, RCON - 30480.0, -1, mu);
	OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, mu);
	UR3 = unit(R3);
	v3 = length(V3);
	S_FPA = dotp(UR3, V3) / v3;
	gammaE = asin(S_FPA);
	EntryCalculations::augekugel(v3, gammaE, phie, te);

	for (int iii = 0;iii < rangeiter;iii++)
	{
		t2 = (sv0.MJD - GETbase) * 24.0 * 3600.0 + dt2;	//EI time in seconds from launch
		t_LS = t2 + t32 + te;
		Sphie = sin(0.00029088821*phie);
		Cphie = cos(0.00029088821*phie);
		U_H3 = unit(crossp(crossp(R3, V3), R3));
		U_LS = UR3*Cphie + U_H3*Sphie;

		tLSMJD = GETbase + t_LS / 24.0 / 3600.0;
		//U_LS = tmul(Rot, U_LS);
		U_LS = _V(U_LS.x, U_LS.z, U_LS.y);
		R = OrbMech::GetRotationMatrix(hEarth, tLSMJD);
		LSEF = tmul(R, U_LS);
		l = LSEF.x;
		m = LSEF.z;
		n = LSEF.y;
		phi = asin(n);
		if (m > 0)
		{
			lambda2 = acos(l / cos(phi));
		}
		else
		{
			lambda2 = PI2 - acos(l / cos(phi));
		}
		if (lambda2 > PI) { lambda2 -= PI2; }

		res->latitude = phi;
		res->longitude = lambda2;


		dt22 = OrbMech::time_radius(R3, V3, length(R3) - (300000.0 * 0.3048 - EMSAlt), -1, mu);
		OrbMech::rv_from_r0v0(R3, V3, dt22, R05G, V05G, mu);

		res->RTGO = phie - 3437.7468*acos(dotp(unit(R3), unit(R05G)));
		if (entryrange != 0)
		{
			te *= entryrange / res->RTGO;
			phie = entryrange + 3437.7468*acos(dotp(unit(R3), unit(R05G)));
		}
	}
	res->VIO = length(V05G);
	res->GET400K = t2;
	res->GET05G = t2 + t32 + dt22;	
}

void RTCC::CalcSPSGimbalTrimAngles(double CSMmass, double LMmass, double &p_T, double &y_T)
{
	double x1;

	x1 = LMmass / (CSMmass + LMmass)*6.2;
	p_T = atan2(-2.15 * RAD * 5.0, 5.0 + x1);
	y_T = atan2(0.95 * RAD * 5.0, 5.0 + x1);
}

void RTCC::RendezvousPlanner(VESSEL *chaser, VESSEL *target, SV sv_A0, double GETbase, double t_TIG, double t_TPI, double &t_Ins, double &CSI)
{
	//Plan 1: Phasing (fixed TIG), Insertion, CSI at apolune, CDH, TPI at midnight (Apollo 10)

	LambertMan lamopt, lamopt2;
	double t_sv0, t_Phasing, t_Insertion, dt, t_CSI, dt2, mu, ddt, ddt2, T_P, DH, dv_CSI, t_CDH, dt_TPI, t_TPI_apo;
	VECTOR3 dV_Phasing, dV_Insertion, dV_CDH, DVX;
	MATRIX3 Q_Xx;
	SV sv_P0, sv_P_CSI, sv_Phasing, sv_Phasing_apo, sv_Insertion, sv_Insertion_apo, sv_CSI, sv_CSI_apo, sv_CDH, sv_CDH_apo, sv_P_CDH;

	mu = GGRAV * oapiGetMass(sv_A0.gravref);

	t_Phasing = t_TIG;
	dt = 7017.0;
	dt2 = 3028.0;
	dv_CSI = 50.0*0.3048;
	DH = 15.0*1852.0;
	ddt = 10.0;

	sv_P0 = StateVectorCalc(target);

	lamopt.GETbase = GETbase;
	lamopt.N = 0;
	lamopt.Offset = _V(-270.0*1852.0, 0.0, 60.0*1852.0 - 60000.0*0.3048);
	lamopt.Perturbation = RTCC_LAMBERT_PERTURBED;
	lamopt.T1 = t_Phasing;
	lamopt.sv_P = sv_P0;

	lamopt2 = lamopt;
	lamopt2.Offset = _V(-147.0*1852.0, 0.0, 14.7*1852.0);

	t_sv0 = OrbMech::GETfromMJD(sv_A0.MJD, GETbase);
	sv_Phasing = coast(sv_A0, t_Phasing - t_sv0);

	//Loop
	while (abs(ddt) > 1.0)
	{
		t_Insertion = t_Phasing + dt;
		
		lamopt.T2 = t_Insertion;
		lamopt.sv_A = sv_Phasing;

		LambertTargeting(&lamopt, dV_Phasing);
		sv_Phasing_apo = sv_Phasing;
		sv_Phasing_apo.V += dV_Phasing;

		ddt2 = 1.0;

		//Loop
		while (abs(ddt2) > 0.1)
		{
			t_CSI = t_Insertion + dt2;

			lamopt2.T1 = t_Insertion;
			lamopt2.T2 = t_CSI;
			lamopt2.sv_A = sv_Phasing_apo;

			LambertTargeting(&lamopt2, dV_Insertion);

			sv_Insertion = coast(sv_Phasing_apo, t_Insertion - t_Phasing);
			sv_Insertion_apo = sv_Insertion;
			sv_Insertion_apo.V += dV_Insertion;

			sv_CSI = coast(sv_Insertion_apo, t_CSI - t_Insertion);
			T_P = OrbMech::period(sv_CSI.R, sv_CSI.V, mu);
			ddt2 = OrbMech::timetoapo(sv_CSI.R, sv_CSI.V, mu);

			if (ddt2 > T_P / 2.0)
			{
				ddt2 = ddt2 - T_P;
			}
			dt2 += ddt2;
		}

		//CSI Targeting
		sv_P_CSI = coast(sv_P0, t_CSI - OrbMech::GETfromMJD(sv_P0.MJD, GETbase));
		OrbMech::CSIToDH(sv_CSI.R, sv_CSI.V, sv_P_CSI.R, sv_P_CSI.V, DH, mu, dv_CSI);
		sv_CSI_apo = sv_CSI;
		sv_CSI_apo.V = sv_CSI.V + OrbMech::ApplyHorizontalDV(sv_CSI.R, sv_CSI.V, dv_CSI);

		//CDH Targeting
		T_P = OrbMech::period(sv_CSI_apo.R, sv_CSI_apo.V, mu);
		t_CDH = t_CSI + T_P / 2.0;
		NSRProgram(sv_CSI_apo, sv_P_CSI, GETbase, 0.0, t_CDH, 0.0, dV_CDH);
		sv_CDH = coast(sv_CSI_apo, t_CDH - t_CSI);
		Q_Xx = OrbMech::LVLH_Matrix(sv_CDH.R, sv_CDH.V);
		DVX = tmul(Q_Xx, dV_CDH);
		sv_CDH_apo = sv_CDH;
		sv_CDH_apo.V += DVX;
		sv_P_CDH = coast(sv_P_CSI, t_CDH - t_CSI);

		//Find TPI time and recycle
		dt_TPI = OrbMech::findelev(sv_CDH_apo.R, sv_CDH_apo.V, sv_P_CDH.R, sv_P_CDH.V, sv_CDH_apo.MJD, 26.6*RAD, sv_CDH_apo.gravref);
		t_TPI_apo = t_CDH + dt_TPI;
		ddt = t_TPI - t_TPI_apo;
		dt += ddt;
	}

	t_Ins = t_Insertion;
	CSI = t_CSI;
}

void RTCC::DockingInitiationProcessor(DKIOpt *opt, VECTOR3 &DV_Phasing, double &t_CSI)
{
	SV sv_AP, sv_TPI, sv_PC;
	VECTOR3 u, R_AP, V_AP, V_APF, R_AH, V_AH, V_AHF, R_AC, V_AC, R_PC, V_PC, V_ACF, R_PJ, V_PJ, R_AFD, R_AF, V_AF;
	double mu, dv_P, p_P, dt_PH, c_P, eps_P, dv_H, dt_HC, t_H, t_C, dv_Po, e_Po, e_P;
	int s_P;

	eps_P = 0.000004;	//radians
	s_P = 0;
	p_P = c_P = 0.0;
	dv_P = 100.0*0.3048;
	dv_H = 0.0;
	mu = GGRAV * oapiGetMass(opt->sv_A.gravref);

	sv_AP = coast(opt->sv_A, opt->t_TIG - OrbMech::GETfromMJD(opt->sv_A.MJD, opt->GETbase));
	sv_TPI = coast(opt->sv_P, opt->t_TPI - OrbMech::GETfromMJD(opt->sv_P.MJD, opt->GETbase));

	OrbMech::QDRTPI(sv_TPI.R, sv_TPI.V, sv_TPI.MJD, sv_TPI.gravref, mu, opt->DH, opt->E, 0, R_PJ, V_PJ);
	R_AFD = R_PJ - unit(R_PJ)*opt->DH;

	u = unit(crossp(sv_TPI.R, sv_TPI.V));

	R_AP = unit(sv_AP.R - u * dotp(sv_AP.R, u))*length(sv_AP.R);
	V_AP = unit(sv_AP.V - u * dotp(sv_AP.V, u))*length(sv_AP.V);

	do
	{
		V_APF = V_AP + unit(crossp(u, R_AP))*dv_P;
		OrbMech::REVUP(R_AP, V_APF, 0.5, mu, R_AH, V_AH, dt_PH);
		t_H = opt->t_TIG + dt_PH;

		OrbMech::CSIToDH(R_AH, V_AH, sv_TPI.R, sv_TPI.V, opt->DH, mu, dv_H);
		V_AHF = V_AH + unit(crossp(u, R_AH))*dv_H;
		OrbMech::REVUP(R_AH, V_AHF, 0.5, mu, R_AC, V_AC, dt_HC);
		t_C = t_H + dt_HC;
		OrbMech::RADUP(sv_TPI.R, sv_TPI.V, R_AC, mu, R_PC, V_PC);

		V_ACF = OrbMech::CoellipticDV(R_AC, R_PC, V_PC, mu);
		OrbMech::rv_from_r0v0(R_AC, V_ACF, opt->t_TPI - t_C, R_AF, V_AF, mu);
		e_P = OrbMech::sign(dotp(crossp(R_AF, R_AFD), u))*acos(dotp(R_AFD / length(R_AFD), R_AF / length(R_AF)));

		if (p_P == 0 || abs(e_P) >= eps_P)
		{
			OrbMech::ITER(c_P, s_P, e_P, p_P, dv_P, e_Po, dv_Po);
		}
	} while (abs(e_P) >= eps_P);

	DV_Phasing = OrbMech::ApplyHorizontalDV(sv_AP.R, sv_AP.V, dv_P);
	t_CSI = t_H;
}

void RTCC::ConcentricRendezvousProcessor(SPQOpt *opt, VECTOR3 &DV_coe, double &t_TPI)
{
	SV sv_A1, sv_P1;
	VECTOR3 u, R_A1, V_A1, V_A1F, R_A2, V_A2, R_P2, V_P2, R_PC, V_PC, V_A2F;
	double dv_CSI, mu, dt_1, t_CDH, dt_TPI;

	dv_CSI = 0.0;

	mu = GGRAV * oapiGetMass(opt->sv_A.gravref);
	sv_A1 = coast(opt->sv_A, opt->t_TIG - OrbMech::GETfromMJD(opt->sv_A.MJD, opt->GETbase));
	sv_P1 = coast(opt->sv_P, opt->t_TIG - OrbMech::GETfromMJD(opt->sv_P.MJD, opt->GETbase));

	u = unit(crossp(sv_P1.R, sv_P1.V));

	R_A1 = unit(sv_A1.R - u * dotp(sv_A1.R, u))*length(sv_A1.R);
	V_A1 = unit(sv_A1.V - u * dotp(sv_A1.V, u))*length(sv_A1.V);

	//CDH calculation
	if (opt->maneuver == 1)
	{
		DV_coe = OrbMech::CoellipticDV(R_A1, sv_P1.R, sv_P1.V, mu) - V_A1;
		t_TPI = opt->t_TPI;
		return;
	}

	if (opt->type == 0)
	{
		VECTOR3 R_P3, V_P3, R_PJ, V_PJ, R_AF, V_AF, R_AFD;
		double p_C, c_C, eps_C, e_C, e_Co, dv_CSIo;
		int s_C;

		eps_C = 0.000004;	//radians
		p_C = c_C = 0.0;
		s_C = 0;
		dv_CSI = 10.0*0.3048;

		OrbMech::rv_from_r0v0(sv_P1.R, sv_P1.V, opt->t_TPI - opt->t_TIG, R_P3, V_P3, mu);
		OrbMech::QDRTPI(R_P3, V_P3, opt->GETbase + opt->t_TPI / 24.0 / 3600.0, sv_P1.gravref, mu, opt->DH, opt->E, 0, R_PJ, V_PJ);
		R_AFD = R_PJ - unit(R_PJ)*opt->DH;

		do
		{
			V_A1F = V_A1 + unit(crossp(u, R_A1))*dv_CSI;
			OrbMech::REVUP(R_A1, V_A1F, 0.5, mu, R_A2, V_A2, dt_1);
			t_CDH = opt->t_TIG + dt_1;
			OrbMech::RADUP(R_P3, V_P3, R_A2, mu, R_PC, V_PC);
			V_A2F = OrbMech::CoellipticDV(R_A2, R_PC, V_PC, mu);
			OrbMech::rv_from_r0v0(R_A2, V_A2F, opt->t_TPI - t_CDH, R_AF, V_AF, mu);

			e_C = OrbMech::sign(dotp(crossp(R_AF, R_AFD), u))*acos(dotp(R_AFD / length(R_AFD), R_AF / length(R_AF)));

			if (p_C == 0 || abs(e_C) >= eps_C)
			{
				OrbMech::ITER(c_C, s_C, e_C, p_C, dv_CSI, e_Co, dv_CSIo);
			}
		} while (abs(e_C) >= eps_C);

		DV_coe = V_A1F - V_A1;
		t_TPI = opt->t_TPI;
	}
	else
	{
		OrbMech::CSIToDH(R_A1, V_A1, sv_P1.R, sv_P1.V, opt->DH, mu, dv_CSI);
		V_A1F = V_A1 + unit(crossp(u, R_A1))*dv_CSI;
		OrbMech::REVUP(R_A1, V_A1F, 0.5, mu, R_A2, V_A2, dt_1);
		t_CDH = opt->t_TIG + dt_1;
		OrbMech::RADUP(sv_P1.R, sv_P1.V, R_A2, mu, R_PC, V_PC);
		V_A2F = OrbMech::CoellipticDV(R_A2, R_PC, V_PC, mu);
		OrbMech::rv_from_r0v0(sv_P1.R, sv_P1.V, t_CDH - opt->t_TIG, R_P2, V_P2, mu);
		dt_TPI = OrbMech::findelev_conic(R_A2, V_A2F, R_P2, V_P2, opt->E, mu);

		DV_coe = V_A1F - V_A1;
		t_TPI = t_CDH + dt_TPI;
	}
}