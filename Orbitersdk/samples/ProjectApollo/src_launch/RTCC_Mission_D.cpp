/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

RTCC Calculations for Mission D

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

#include "Orbitersdk.h"
#include "soundlib.h"
#include "apolloguidance.h"
#include "saturn.h"
#include "LEM.h"
#include "../src_rtccmfd/OrbMech.h"
#include "mcc.h"
#include "rtcc.h"

bool RTCC::CalculationMTP_D(int fcn, LPVOID &pad, char * upString, char * upDesc, char * upMessage)
{
	char uplinkdata[1024 * 3];
	bool preliminary = true;
	bool scrubbed = false;

	double AGCEpoch = 40221.525;
	int LGCREFSAddrOffs = -2;

	switch (fcn) {
	case 1: //GENERIC CSM STATE VECTOR UPDATE
	{
		SV sv;
		double GETbase;
		char buffer1[1000];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();
		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s", buffer1);
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
		char buffer1[1000];

		GETbase = getGETBase();
		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		NavCheckPAD(sv, *form, GETbase);
		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 3: //GENERIC CSM AND LM STATE VECTOR UPDATE
	{
		SV sv_CSM, sv_LM;
		double GETbase;
		char buffer1[1000];
		char buffer2[1000];

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();

		AGCStateVectorUpdate(buffer1, sv_CSM, true, AGCEpoch, GETbase);
		AGCStateVectorUpdate(buffer2, sv_LM, false, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM and LM state vectors");
		}
	}
	break;
	case 4: //GENERIC LM STATE VECTOR UPDATE
	{
		SV sv;
		double GETbase;
		char buffer1[1000];

		sv = StateVectorCalc(calcParams.tgt); //State vector for uplink
		GETbase = getGETBase();

		AGCStateVectorUpdate(buffer1, sv, false, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector");
		}
	}
	break;
	case 9: //DAYLIGHT STAR CHECK
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
		double GETbase, P30TIG;
		VECTOR3 dV_LVLH;
		SV sv0;
		char buffer1[1000];
		char buffer2[1000];

		sv0 = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();

		P30TIG = OrbMech::HHMMSSToSS(5, 59, 0);
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

		AGCStateVectorUpdate(buffer1, sv0, true, AGCEpoch, GETbase);
		AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
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

		int n = 6;
		double lng[] = { 145.0*RAD, -161.7*RAD, -23.0*RAD, -32.1*RAD, -26.4*RAD, -27.9*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(13,14,31),OrbMech::HHMMSSToSS(15,02,51),OrbMech::HHMMSSToSS(15,40,24),OrbMech::HHMMSSToSS(17,13,49),OrbMech::HHMMSSToSS(18,50,57),OrbMech::HHMMSSToSS(20,24,40) };
		std::string area[] = { "009-3B", "010-CC", "011-AC", "012-AC", "013-2A", "014-AC" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 12: //BLOCK DATA 3
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -67.0*RAD, -67.0*RAD, -67.0*RAD, -66.3*RAD, -69.2*RAD, -165.5*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(21,52,49),OrbMech::HHMMSSToSS(23,28,3),OrbMech::HHMMSSToSS(25,2,25),OrbMech::HHMMSSToSS(26,37,58),OrbMech::HHMMSSToSS(28,11,50),OrbMech::HHMMSSToSS(31,7,17) };
		std::string area[] = { "015-1B", "016-1B", "017-1B", "018-1B", "019-1B", "020-4A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
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
		char buffer1[1000];
		char buffer2[1000];

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

		AGCStateVectorUpdate(buffer1, sv0, true, AGCEpoch, GETbase, true);
		AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
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
		char buffer1[1000];
		char buffer2[1000];

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

		AGCStateVectorUpdate(buffer1, sv0, true, AGCEpoch, GETbase, true);
		AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
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
		char buffer1[1000];
		char buffer2[1000];

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

		AGCStateVectorUpdate(buffer1, sv0, true, AGCEpoch, GETbase, true);
		AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
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

		int n = 6;
		double lng[] = { -161.0*RAD, -161.0*RAD, -162.5*RAD, -7.0*RAD, -162.0*RAD, -26.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(32,44,34),OrbMech::HHMMSSToSS(34,19,1),OrbMech::HHMMSSToSS(35,56,3),OrbMech::HHMMSSToSS(36,24,11),OrbMech::HHMMSSToSS(39,13,13),OrbMech::HHMMSSToSS(39,33,59) };
		std::string area[] = { "021-4A", "022-4C", "023-4C", "024-AC", "025-4C", "026-AC" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 17: //BLOCK DATA 5
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -31.0*RAD, -26.4*RAD, -28.5*RAD, -29.0*RAD, -32.0*RAD, -33.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(41,16,3),OrbMech::HHMMSSToSS(43,2,57),OrbMech::HHMMSSToSS(44,46,10),OrbMech::HHMMSSToSS(46,24,14),OrbMech::HHMMSSToSS(47,58,31),OrbMech::HHMMSSToSS(49,34,33) };
		std::string area[] = { "027-AC", "028-2A", "029-AC", "030-2C", "031-2C", "032-2B" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 18: //DOCKED DPS BURN - REFSMMAT AND SV FOR CMC
	case 21: //DOCKED DPS BURN - REFSMMAT AND SV FOR LGC
	{
		GMPOpt gmpopt;
		REFSMMATOpt refsopt;
		MATRIX3 REFSMMAT;
		SV sv0;
		VECTOR3 dV_LVLH;
		double GETbase, P30TIG;

		GETbase = getGETBase();
		sv0 = StateVectorCalc(calcParams.src); //State vector for uplink

		gmpopt.csmlmdocked = true;
		gmpopt.GETbase = GETbase;
		gmpopt.impulsive = RTCC_NONIMPULSIVE;
		gmpopt.rot_ang = 6.9*RAD;
		gmpopt.TIG_GET = OrbMech::HHMMSSToSS(49, 42, 0);
		gmpopt.type = 7;
		gmpopt.vessel = calcParams.tgt;

		GeneralManeuverProcessor(&gmpopt, dV_LVLH, P30TIG);

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

		refsopt.P30TIG = P30TIG;
		refsopt.dV_LVLH = dV_LVLH;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.tgt;
		refsopt.vesseltype = 3;
		refsopt.csmlmdocked = true;
		refsopt.GETbase = GETbase;
		refsopt.HeadsUp = false;

		REFSMMAT = REFSMMATCalc(&refsopt);

		//CMC UPDATE
		if (fcn == 18)
		{
			char buffer1[1000];
			char buffer2[1000];

			AGCStateVectorUpdate(buffer1, sv0, true, AGCEpoch, GETbase, true);
			AGCDesiredREFSMMATUpdate(buffer2, REFSMMAT, AGCEpoch);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, Verb 66, Desired REFSMMAT");
			}
		}
		//LGC UPDATE
		else
		{
			AP11LMMNV * form = (AP11LMMNV *)pad;

			AP10DAPDATA dappad;
			AP11LMManPADOpt manopt;
			LEM *lem;
			double TEPHEM0, tephem, t_AGC, t_actual, deltaT;
			char buffer1[1000];
			char buffer2[1000];
			char clockupdate[128];

			lem = (LEM *)calcParams.tgt;
			TEPHEM0 = 40038.;

			tephem = GetTEPHEMFromAGC(&lem->agc.vagc);
			t_AGC = GetClockTimeFromAGC(&lem->agc.vagc);

			tephem = (tephem / 8640000.) + TEPHEM0;
			t_actual = (oapiGetSimMJD() - tephem) * 8640000.;
			deltaT = t_actual - t_AGC;

			IncrementAGCTime(clockupdate, deltaT);

			manopt.csmlmdocked = true;
			manopt.dV_LVLH = dV_LVLH;
			manopt.enginetype = 1;
			manopt.GETbase = GETbase;
			manopt.HeadsUp = false;
			manopt.REFSMMAT = REFSMMAT;
			manopt.TIG = P30TIG;
			manopt.vessel = calcParams.tgt;

			AP11LMManeuverPAD(&manopt, *form);
			LMDAPUpdate(calcParams.tgt, dappad);

			sprintf(form->remarks, "LM weight is %.0f, CSM weight is %.0f", dappad.ThisVehicleWeight, dappad.OtherVehicleWeight);

			AGCStateVectorUpdate(buffer1, sv0, false, AGCEpoch, GETbase);
			AGCREFSMMATUpdate(buffer2, REFSMMAT, AGCEpoch, LGCREFSAddrOffs);

			sprintf(uplinkdata, "%s%s%s", clockupdate, buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "Clock update, LM state vector, REFSMMAT");
			}
		}
	}
	break;
	case 19: //LM AOT STAR OBSERVATION PAD
	{
		AP9AOTSTARPAD * form = (AP9AOTSTARPAD *)pad;

		form->CSMAtt = _V(79.0, 358.0, 309.0);
		form->Detent = 2;
		form->GET = OrbMech::HHMMSSToSS(43, 55, 0);
		form->Star = 015;
	}
	break;
	case 20: //BLOCK DATA 6
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -62.1*RAD, -157.9*RAD, -157.9*RAD, -145.0*RAD, 161.9*RAD, 150.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(51,4,32),OrbMech::HHMMSSToSS(53,58,9),OrbMech::HHMMSSToSS(55,29,8),OrbMech::HHMMSSToSS(56,53,16),OrbMech::HHMMSSToSS(58,39,31),OrbMech::HHMMSSToSS(60,2,28) };
		std::string area[] = { "033-1A", "034-4A", "035-4A", "036-3A", "037-4A", "038-3B" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 22: //GYRO TORQUING ANGLES
	{
		TORQANG * form = (TORQANG *)pad;
		LEM *lem = (LEM *)calcParams.tgt;

		VECTOR3 lmn20, csmn20, V42angles;

		csmn20.x = calcParams.src->imu.Gimbal.X;
		csmn20.y = calcParams.src->imu.Gimbal.Y;
		csmn20.z = calcParams.src->imu.Gimbal.Z;

		lmn20.x = lem->imu.Gimbal.X;
		lmn20.y = lem->imu.Gimbal.Y;
		lmn20.z = lem->imu.Gimbal.Z;

		V42angles = OrbMech::finealignLMtoCSM(lmn20, csmn20);

		form->V42Angles.x = V42angles.x*DEG;
		form->V42Angles.y = V42angles.y*DEG;
		form->V42Angles.z = V42angles.z*DEG;
	}
	break;
	case 23: //SPS-5
	{
		AP7MNV * form = (AP7MNV *)pad;

		AP10DAPDATA dappad;
		GMPOpt opt;
		AP7ManPADOpt manopt;
		REFSMMATOpt refsopt;
		SV sv;
		MATRIX3 REFSMMAT;
		VECTOR3 dV_LVLH;
		double P30TIG, GETbase, dv_T;
		char buffer1[1000];
		char buffer2[1000];

		sv = StateVectorCalc(calcParams.src);
		GETbase = getGETBase();

		opt.csmlmdocked = true;
		opt.GETbase = GETbase;
		opt.h_peri = 133.0*1852.0;
		opt.impulsive = RTCC_NONIMPULSIVE;
		opt.TIG_GET = OrbMech::HHMMSSToSS(54, 25, 16);
		opt.type = 4;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		GeneralManeuverProcessor(&opt, dV_LVLH, P30TIG);

		//ensure 40+ seconds burntime
		dv_T = OrbMech::DVFromBurnTime(41.4, SPS_THRUST, SPS_ISP, calcParams.src->GetMass() + calcParams.tgt->GetMass());
		dV_LVLH.y = -sqrt(max(0, dv_T * dv_T - dV_LVLH.x*dV_LVLH.x - dV_LVLH.z*dV_LVLH.z));

		refsopt.csmlmdocked = true;
		refsopt.dV_LVLH = dV_LVLH;
		refsopt.GETbase = GETbase;
		refsopt.HeadsUp = false;
		refsopt.P30TIG = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;
		refsopt.vesseltype = 1;

		REFSMMAT = REFSMMATCalc(&refsopt);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = REFSMMAT;
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.vesseltype = 1;

		AP7ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "SPS-5");

		CSMDAPUpdate(calcParams.src, dappad);
		sprintf(form->remarks, "LM weight is %.0f", dappad.OtherVehicleWeight);

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase, true);
		AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, Verb 66, target load");
		}
	}
	break;
	case 24: //BLOCK DATA 7
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { 145.0*RAD, -12.0*RAD, -23.0*RAD, -32.0*RAD, -27.0*RAD, -29.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(61,35,8),OrbMech::HHMMSSToSS(62,29,34),OrbMech::HHMMSSToSS(64,2,26),OrbMech::HHMMSSToSS(65,35,55),OrbMech::HHMMSSToSS(67,12,51),OrbMech::HHMMSSToSS(68,46,52) };
		std::string area[] = { "039-3A", "040-AC", "041-AC", "042-AC", "043-2A", "044-AC" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 25: //BLOCK DATA 8
	{

	}
	break;
	case 26: //EVA REFSMMAT
	{
		MATRIX3 REFSMMAT, MX, MY;
		VECTOR3 R_SC, UX, UY, UZ;
		double MJD, GETbase;
		OBJHANDLE hSun;
		SV sv;
		char buffer1[1000];
		char buffer2[1000];

		sv = StateVectorCalc(calcParams.src);
		GETbase = getGETBase();

		MJD = oapiGetSimMJD();
		hSun = oapiGetObjectByName("Sun");
		calcParams.src->GetRelativePos(hSun, R_SC);

		UZ = -unit(R_SC);
		UY = unit(crossp(UZ, _V(0.0, 0.0, 1.0)));
		UX = crossp(UY, UZ);

		MY = OrbMech::_MRy(-15.0*RAD);
		MX = OrbMech::_MRx(-80.0*RAD);
		REFSMMAT = mul(MX, mul(MY, _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z)));

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase, true);
		AGCDesiredREFSMMATUpdate(buffer2, REFSMMAT, AGCEpoch);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, Verb 66, EVA REFSMMAT");
		}
	}
	break;
	case 27: //BLOCK DATA 9
	{

	}
	break;
	case 28: //BLOCK DATA 10
	{

	}
	break;
	}

	return scrubbed;
}