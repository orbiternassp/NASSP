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
	case 5: //CSM DAP DATA
	{
		AP10DAPDATA * form = (AP10DAPDATA *)pad;

		CSMDAPUpdate(calcParams.src, *form);
	}
	break;
	case 6: //LM DAP DATA
	{
		AP10DAPDATA * form = (AP10DAPDATA *)pad;

		LMDAPUpdate(calcParams.tgt, *form);
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

		V42angles = OrbMech::LMDockedFineAlignment(lmn20, csmn20);

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
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -62.9*RAD, -62.0*RAD, -62.0*RAD, -64.0*RAD, -161.9*RAD, -162.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(70,12,33),OrbMech::HHMMSSToSS(71,45,43),OrbMech::HHMMSSToSS(73,20,28),OrbMech::HHMMSSToSS(74,54,9),OrbMech::HHMMSSToSS(77,40,30),OrbMech::HHMMSSToSS(79,14,13) };
		std::string area[] = { "045-1B", "046-1B", "047-1B", "048-1A", "049-4B", "050-4B" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 26: //EVA REFSMMAT
	{
		REFSMMATOpt opt;
		MATRIX3 REFSMMAT, E;
		VECTOR3 GA;
		double GETbase;
		SV sv;
		char buffer1[1000];
		char buffer2[1000];

		sv = StateVectorCalc(calcParams.src);
		GETbase = getGETBase();
		E = { 1,0,0, 0,1,0, 0,0,1 };

		GA = HatchOpenThermalControl(calcParams.src, E);

		opt.IMUAngles = GA;
		opt.PresentREFSMMAT = E;
		opt.REFSMMATopt = 9;
		REFSMMAT = REFSMMATCalc(&opt);

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
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -161.9*RAD, 148.5*RAD, 148.5*RAD, 145.0*RAD, -162.0*RAD, -24.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(80,49,10),OrbMech::HHMMSSToSS(82,12,23),OrbMech::HHMMSSToSS(83,46,6),OrbMech::HHMMSSToSS(85,19,30),OrbMech::HHMMSSToSS(87,11,8),OrbMech::HHMMSSToSS(87,47,6) };
		std::string area[] = { "051-4A", "052-3B", "053-3A", "054-3B", "055-CC", "056-AC" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 28: //BLOCK DATA 10
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 7;
		double lng[] = { -31.9*RAD, -27.0*RAD, -27.9*RAD, -62.9*RAD, -62.9*RAD, -62.5*RAD, -63.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(89,19,18),OrbMech::HHMMSSToSS(90,55,37),OrbMech::HHMMSSToSS(92,29,25),OrbMech::HHMMSSToSS(93,55,38),OrbMech::HHMMSSToSS(95,29,25),OrbMech::HHMMSSToSS(97,3,12),OrbMech::HHMMSSToSS(98,37,15) };
		std::string area[] = { "057-AC", "058-2A", "059-AC", "060-1A", "061-1B", "062-1B", "63-1A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 29: //CSM Rendezvous REFSMMAT Update
	{
		//Rendezvous Plan

		SV sv0, sv1, sv2;
		double GETbase, t_TPI0;
		sv0 = StateVectorCalc(calcParams.src);
		GETbase = getGETBase();

		DMissionRendezvousPlan(sv0, GETbase, t_TPI0);

		//Calculate LM REFSMMAT
		REFSMMATOpt opt;
		MATRIX3 REFSMMAT, A;

		opt.GETbase = GETbase;
		opt.REFSMMATopt = 2;
		opt.REFSMMATTime = calcParams.TPI;
		opt.vessel = calcParams.tgt;
		opt.vesseltype = 3;

		calcParams.StoredREFSMMAT = REFSMMATCalc(&opt);

		//Convert LM REFSMMAT to CSM REFSMMAT
		A = calcParams.StoredREFSMMAT;
		REFSMMAT = _M(A.m31, A.m32, A.m33, A.m21, A.m22, A.m23, -A.m11, -A.m12, -A.m13);

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
	break;
	case 30: //LM Rendezvous REFSMMAT Update
	{
		MATRIX3 REFSMMAT;
		SV sv0;
		double GETbase;

		AP7NAV * form = (AP7NAV *)pad;

		sv0 = StateVectorCalc(calcParams.src);
		GETbase = getGETBase();

		REFSMMAT = calcParams.StoredREFSMMAT;

		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		AGCStateVectorUpdate(buffer1, sv0, true, AGCEpoch, GETbase);
		AGCStateVectorUpdate(buffer2, sv0, false, AGCEpoch, GETbase);
		AGCREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch, LGCREFSAddrOffs);

		NavCheckPAD(sv0, *form, GETbase);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector, CSM state vector, REFSMMAT");
		}
	}
	break;
	case 31: //LM GYRO TORQUING ANGLES
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

		V42angles = OrbMech::LMDockedFineAlignment(lmn20, csmn20, false);

		form->V42Angles.x = V42angles.x*DEG;
		form->V42Angles.y = V42angles.y*DEG;
		form->V42Angles.z = V42angles.z*DEG;
	}
	break;
	case 32: //LM PHASING MANEUVER
	{
		AP11LMMNV * form = (AP11LMMNV *)pad;

		AP11LMManPADOpt opt;
		SV sv, sv1;
		MATRIX3 Q_Xx;
		VECTOR3 dV_LVLH, DV;
		double mu, eps, a, n0, n1, n2, dt1, dt2, dh1, dh2, dphase, dv, dphase_bias, t_TPI0, GETbase, P30TIG, t_Sep;

		//about 10NM less phasing in the coelliptic orbit above
		dphase_bias = 0.0027987178;

		sv = StateVectorCalc(calcParams.src);
		GETbase = getGETBase();
		mu = GGRAV * oapiGetMass(sv.gravref);

		DMissionRendezvousPlan(sv, GETbase, t_TPI0);
		//Store the TPI0 time here, the nominal TPI time is already stored in calcParams.TPI
		TimeofIgnition = t_TPI0;

		eps = length(sv.V)*length(sv.V) / 2.0 - mu / length(sv.R);
		a = -mu / (2.0*eps);
		n0 = sqrt(mu / pow(a, 3));

		dt1 = calcParams.CSI - calcParams.Insertion;
		dt2 = calcParams.TPI - calcParams.CDH;

		dh2 = 10.0*1852.0;
		n2 = sqrt(mu / pow((a - dh2), 3));
		dphase = (n2 - n0)*dt2 - dphase_bias;

		n1 = -dphase / dt1 + n0;
		dh1 = OrbMech::power(mu / (n1*n1), 1.0 / 3.0) - a;
		dv = dh1 * n0;

		sv1 = coast(sv, calcParams.Phasing - OrbMech::GETfromMJD(sv.MJD, GETbase));
		Q_Xx = OrbMech::LVLH_Matrix(sv1.R, sv1.V);
		DV = tmul(Q_Xx, _V(0, 0, -dv));

		PoweredFlightProcessor(sv, GETbase, calcParams.Phasing, RTCC_VESSELTYPE_LM, RTCC_ENGINETYPE_SPSDPS, 0.0, DV, P30TIG, dV_LVLH);

		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.TIG = P30TIG;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);

		t_Sep = calcParams.Phasing - 45.0*60.0;

		char GETbuffer1[64];
		char GETbuffer2[64];

		OrbMech::format_time_HHMMSS(GETbuffer1, t_Sep);
		OrbMech::format_time_HHMMSS(GETbuffer2, t_TPI0);

		sprintf(form->purpose, "Phasing");
		sprintf(form->remarks, "Your SEP time: %s, TPI0: %s", GETbuffer1, GETbuffer2);
	}
	break;
	case 33: //TPI0 MANEUVER
	{
		AP9LMTPI * form = (AP9LMTPI *)pad;

		SV sv_A, sv_P;
		LambertMan opt;
		AP9LMTPIPADOpt manopt;
		TwoImpulseResuls res;
		double GETbase, mu;

		sv_P = StateVectorCalc(calcParams.src);
		sv_A = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();
		mu = GGRAV * oapiGetMass(sv_P.gravref);

		opt.GETbase = GETbase;
		opt.N = 0;
		opt.Offset = _V(0, 0, 0);
		opt.Perturbation = RTCC_LAMBERT_PERTURBED;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.T1 = TimeofIgnition;
		opt.T2 = TimeofIgnition + OrbMech::time_theta(sv_P.R, sv_P.V, 130.0*RAD, mu);

		LambertTargeting(&opt, res);

		manopt.dV_LVLH = res.dV_LVLH;
		manopt.GETbase = GETbase;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		manopt.sv_A = sv_A;
		manopt.sv_P = sv_P;
		manopt.TIG = opt.T1;

		AP9LMTPIPAD(&manopt, *form);
	}
	break;
	case 34: //INSERTION MANEUVER
	{
		AP11LMMNV * form = (AP11LMMNV *)pad;

		CDHOpt opt;
		AP11LMManPADOpt manopt;
		SV sv0;
		VECTOR3 dV_LVLH;
		double TIG, GETbase, P30TIG;

		sv0 = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();

		TIG = calcParams.Insertion;

		opt.CDHtimemode = 0;
		opt.GETbase = GETbase;
		opt.impulsive = RTCC_NONIMPULSIVE;
		opt.target = calcParams.src;
		opt.TIG = TIG;
		opt.vessel = calcParams.tgt;
		opt.vesseltype = 1;

		CDHcalc(&opt, dV_LVLH, P30TIG);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&manopt, *form);

		sprintf(form->purpose, "Insertion");
	}
	break;
	case 35: //CSI UPDATE
	{
		AP10CSI * form = (AP10CSI *)pad;

		AP10CSIPADOpt manopt;
		SPQOpt opt;
		SV sv_A, sv_P, sv_CSI;
		MATRIX3 Q_Xx;
		VECTOR3 dV, dV_LVLH;
		double GETbase, t_TPI;

		GETbase = getGETBase();
		sv_A = StateVectorCalc(calcParams.tgt);
		sv_P = StateVectorCalc(calcParams.src);

		opt.E = 27.5*RAD;
		opt.GETbase = GETbase;
		opt.maneuver = 0;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.type = 0;
		opt.t_TIG = calcParams.CSI;
		opt.t_TPI = calcParams.TPI;

		ConcentricRendezvousProcessor(&opt, dV, t_TPI);
		sv_CSI = coast(sv_A, opt.t_TIG - OrbMech::GETfromMJD(sv_A.MJD, GETbase));
		Q_Xx = OrbMech::LVLH_Matrix(sv_CSI.R, sv_CSI.V);
		dV_LVLH = mul(Q_Xx, dV);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_APS;
		manopt.GETbase = GETbase;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		manopt.sv0 = sv_A;
		manopt.t_CSI = calcParams.CSI;
		manopt.t_TPI = calcParams.TPI;

		AP10CSIPAD(&manopt, *form);
	}
	break;
	case 36: //CDH UPDATE
	{
		AP9LMCDH * form = (AP9LMCDH *)pad;

		AP9LMCDHPADOpt manopt;
		SPQOpt opt;
		SV sv_A, sv_P, sv_CDH;
		MATRIX3 Q_Xx;
		VECTOR3 dV, dV_LVLH;
		double GETbase, t_TPI, mu;

		GETbase = getGETBase();
		sv_A = StateVectorCalc(calcParams.tgt);
		sv_P = StateVectorCalc(calcParams.src);

		mu = GGRAV * oapiGetMass(sv_A.gravref);
		calcParams.CDH = calcParams.CSI + OrbMech::period(sv_A.R, sv_A.V, mu) / 2.0;

		opt.GETbase = GETbase;
		opt.maneuver = 1;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.type = 0;
		opt.t_TIG = calcParams.CDH;

		ConcentricRendezvousProcessor(&opt, dV, t_TPI);
		sv_CDH = coast(sv_A, opt.t_TIG - OrbMech::GETfromMJD(sv_A.MJD, GETbase));
		Q_Xx = OrbMech::LVLH_Matrix(sv_CDH.R, sv_CDH.V);
		dV_LVLH = mul(Q_Xx, dV);

		manopt.dV_LVLH = dV_LVLH;
		manopt.GETbase = GETbase;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		manopt.sv_A = sv_A;
		manopt.TIG = calcParams.CDH;

		AP9LMCDHPAD(&manopt, *form);
	}
	break;
	case 37: //TPI MANEUVER
	{
		AP9LMTPI * form = (AP9LMTPI *)pad;

		SV sv_A, sv_P, sv_A1, sv_P1;
		LambertMan opt;
		AP9LMTPIPADOpt manopt;
		TwoImpulseResuls res;
		double GETbase, mu, dt;

		sv_P = StateVectorCalc(calcParams.src);
		sv_A = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();
		mu = GGRAV * oapiGetMass(sv_P.gravref);

		sv_A1 = coast(sv_A, calcParams.TPI - OrbMech::GETfromMJD(sv_A.MJD, GETbase));
		sv_P1 = coast(sv_P, calcParams.TPI - OrbMech::GETfromMJD(sv_P.MJD, GETbase));

		dt = OrbMech::findelev(sv_A1.R, sv_A1.V, sv_P1.R, sv_P1.V, sv_A1.MJD, 27.5*RAD, sv_A1.gravref);
		calcParams.TPI += dt;

		opt.GETbase = GETbase;
		opt.N = 0;
		opt.Offset = _V(0, 0, 0);
		opt.Perturbation = RTCC_LAMBERT_PERTURBED;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.T1 = calcParams.TPI;
		opt.T2 = opt.T1 + OrbMech::time_theta(sv_P.R, sv_P.V, 130.0*RAD, mu);

		LambertTargeting(&opt, res);

		manopt.dV_LVLH = res.dV_LVLH;
		manopt.GETbase = GETbase;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		manopt.sv_A = sv_A;
		manopt.sv_P = sv_P;
		manopt.TIG = opt.T1;

		AP9LMTPIPAD(&manopt, *form);
	}
	break;
	case 38: //LM DOCKED P52 PAD (STAR 15)
	case 39: //LM DOCKED P52 PAD (STAR 25)
	{
		AP9AOTSTARPAD * form = (AP9AOTSTARPAD *)pad;

		SV sv;
		MATRIX3 REFSMMAT;
		VECTOR3 GA;

		form->Detent = 2;
		form->GET = OrbMech::HHMMSSToSS(99, 30, 0);
		if (fcn == 38)
		{
			form->Star = 015;
		}
		else
		{
			form->Star = 025;
		}

		REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		sv = StateVectorCalc(calcParams.src);

		GA = PointAOTWithCSM(REFSMMAT, sv, form->Detent, form->Star, 0.0);
		form->CSMAtt = GA * DEG;
	}
	break;
	case 40: //LM BURN TO DEPLETION UPDATE
	{
		AP11LMMNV * form = (AP11LMMNV *)pad;

		AP10DAPDATA dappad;
		AP11LMManPADOpt opt;
		SV sv0, sv1, sv_TIG;
		MATRIX3 Rot;
		VECTOR3 dV_LVLH, DV, dV_LVLH_imp;
		double GETbase, TIG, P30TIG, dv;
		char buffer1[1000];

		GETbase = getGETBase();
		sv0 = StateVectorCalc(calcParams.tgt);
		sv1 = coast(sv0, OrbMech::HHMMSSToSS(102, 0, 0) - OrbMech::GETfromMJD(sv0.MJD, GETbase));

		//Mid Pass Texas
		double lat_TEX, lng_TEX;
		lat_TEX = groundstations[10][0];
		lng_TEX = groundstations[10][1];
		FindRadarMidPass(sv1, GETbase, lat_TEX, lng_TEX, TIG);

		dv = 7427.5*0.3048;
		dV_LVLH_imp = _V(dv*cos(45.0*RAD), -dv*sin(45.0*RAD), 0.0);
		sv_TIG = coast(sv0, TIG - OrbMech::GETfromMJD(sv0.MJD, GETbase));
		Rot = OrbMech::LVLH_Matrix(sv_TIG.R, sv_TIG.V);
		DV = tmul(Rot, dV_LVLH_imp);

		PoweredFlightProcessor(sv0, GETbase, TIG, RTCC_VESSELTYPE_LM, RTCC_ENGINETYPE_APS, 0.0, DV, P30TIG, dV_LVLH);

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_APS;
		opt.GETbase = GETbase;
		opt.REFSMMAT= GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.TIG = P30TIG;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);

		LMDAPUpdate(calcParams.tgt, dappad);
		sprintf(form->purpose, "APS Depletion");
		sprintf(form->remarks, "LM weight is %.0f", dappad.ThisVehicleWeight);

		AGCStateVectorUpdate(buffer1, sv0, false, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector");
		}
	}
	break;
	case 41: //LM JETTISON ATTITUDE
	{
		GENERICPAD * form = (GENERICPAD *)pad;

		AP11LMMNV manpad;
		AP11LMManPADOpt opt;
		MATRIX3 A;
		double GETbase;

		GETbase = getGETBase();

		opt.dV_LVLH = DeltaV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_APS;
		opt.GETbase = GETbase;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.TIG = TimeofIgnition;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, manpad);

		DockAlignOpt dockopt;

		A = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);
		dockopt.LM_REFSMMAT = A;
		dockopt.CSM_REFSMMAT = _M(A.m31, A.m32, A.m33, A.m21, A.m22, A.m23, -A.m11, -A.m12, -A.m13);
		dockopt.LMAngles = manpad.IMUAtt;
		dockopt.type = 3;

		DockingAlignmentProcessor(dockopt);

		sprintf(form->paddata, "CSM Jettison Attitude: roll %.1f, pitch %.1f, yaw %.1f", dockopt.CSMAngles.x*DEG, dockopt.CSMAngles.y*DEG, dockopt.CSMAngles.z*DEG);
	}
	break;
	}

	return scrubbed;
}

void RTCC::DMissionRendezvousPlan(SV sv_A0, double GETbase, double &t_TPI0)
{
	SV sv2;

	//Step 1: Find TPI0 time (25.5 minutes before sunrise)
	double TPI0_guess, TPI0_sunrise_guess, TPI0_sunrise, dt_sunrise;
	dt_sunrise = 25.5*60.0;
	TPI0_guess = OrbMech::HHMMSSToSS(95, 0, 0);
	TPI0_sunrise_guess = TPI0_guess + dt_sunrise;
	TPI0_sunrise = FindOrbitalSunrise(sv_A0, GETbase, TPI0_sunrise_guess);
	t_TPI0 = TPI0_sunrise - dt_sunrise;

	//Step 2: Phasing is 70 minutes before TPI0
	calcParams.Phasing = t_TPI0 - 70.0*60.0;

	//Step 3: Insertion is 111:42 minutes after Phasing
	calcParams.Insertion = calcParams.Phasing + 111.0*60.0 + 42.0;

	//Step 4: CSI is two minutes (rounded) into after 5° AOS of the TAN pass
	double CSI_guess, lat_TAN, lng_TAN, AOS_TAN, LOS_TAN;
	lat_TAN = groundstations[13][0];
	lng_TAN = groundstations[13][1];
	CSI_guess = calcParams.Insertion + 40.0*60.0;
	sv2 = coast(sv_A0, CSI_guess - OrbMech::GETfromMJD(sv_A0.MJD, GETbase));
	FindRadarAOSLOS(sv2, GETbase, lat_TAN, lng_TAN, AOS_TAN, LOS_TAN);
	calcParams.CSI = (floor(AOS_TAN / 60.0) + 2.0)*60.0;

	//Step 5: CDH is placed 44.4 minutes after CSI
	calcParams.CDH = calcParams.CSI + 44.4*60.0;

	//Step 6: Find TPI0 time (25.5 minutes before sunrise)
	double TPI_guess, TPI_sunrise_guess, TPI_sunrise;
	TPI_guess = OrbMech::HHMMSSToSS(98, 0, 0);
	TPI_sunrise_guess = TPI_guess + dt_sunrise;
	TPI_sunrise = FindOrbitalSunrise(sv_A0, GETbase, TPI_sunrise_guess);
	calcParams.TPI = TPI_sunrise - dt_sunrise;
}