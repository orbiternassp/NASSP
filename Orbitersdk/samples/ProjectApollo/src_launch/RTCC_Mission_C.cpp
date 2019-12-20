/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

RTCC Calculations for Mission C

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
#include "../src_rtccmfd/OrbMech.h"
#include "mcc.h"
#include "rtcc.h"

bool RTCC::CalculationMTP_C(int fcn, LPVOID &pad, char * upString, char * upDesc, char * upMessage)
{
	char uplinkdata[1024 * 3];
	bool preliminary = true;
	bool scrubbed = false;

	double AGCEpoch = 40221.525;

	switch (fcn) {
	case 1: // MISSION C PHASING BURN
	{
		LambertMan lambert;
		TwoImpulseResuls res;
		AP7ManPADOpt opt;
		SV sv_A, sv_P;
		double GET_TIG;

		AP7MNV * form = (AP7MNV *)pad;

		sv_A = StateVectorCalc(calcParams.src);
		sv_P = StateVectorCalc(calcParams.tgt);

		GET_TIG = OrbMech::HHMMSSToSS(3, 20, 0);

		lambert = set_lambertoptions(sv_A, sv_P, getGETBase(), GET_TIG, OrbMech::HHMMSSToSS(26, 25, 0), 15, RTCC_LAMBERT_XAXIS, RTCC_LAMBERT_SPHERICAL, _V(76.5 * 1852, 0, 0), 0);
		LambertTargeting(&lambert, res);

		opt.GETbase = getGETBase();
		opt.vessel = calcParams.src;
		opt.TIG = GET_TIG;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMRCSMINUS4;
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
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();

		entopt.vessel = calcParams.src;
		entopt.GETbase = GETbase;
		entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		entopt.lng = -163.0*RAD;
		entopt.nominal = RTCC_ENTRY_NOMINAL;
		entopt.ReA = 0;
		entopt.TIGguess = 8 * 60 * 60 + 55 * 60;
		entopt.entrylongmanual = true;

		BlockDataProcessor(&entopt, &res); //Target Load for uplink

		refsopt.vessel = calcParams.src;
		refsopt.GETbase = GETbase;
		refsopt.dV_LVLH = res.dV_LVLH;
		refsopt.REFSMMATTime = res.P30TIG;
		refsopt.REFSMMATopt = 1;

		REFSMMAT = REFSMMATCalc(&refsopt); //REFSMMAT for uplink

		opt.vessel = calcParams.src;
		opt.GETbase = GETbase;
		opt.TIG = res.P30TIG;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = true;
		opt.sxtstardtime = -25 * 60;
		opt.REFSMMAT = REFSMMAT;
		opt.navcheckGET = 8 * 60 * 60 + 17 * 60;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "6-4 DEORBIT");

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);
		AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
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

		int n = 6;
		double lng[] = { 136.7*RAD, -16.2*RAD, -22.0*RAD, -33.0*RAD, -28.2*RAD, -62.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(13,29,36),OrbMech::HHMMSSToSS(14,19,12),OrbMech::HHMMSSToSS(15,54,48),OrbMech::HHMMSSToSS(17,28,48),OrbMech::HHMMSSToSS(19,8,6),OrbMech::HHMMSSToSS(20,34,3) };
		std::string area[] = { "009-3B", "010-AC", "011-AC", "012-AC", "013-2A", "014-1B" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 4: //MISSION C 2ND PHASING MANEUVER
	{
		AP7ManPADOpt opt;
		LambertMan lambert;
		TwoImpulseResuls res;
		SV sv_A, sv_P;
		double GET_TIG;

		AP7MNV * form = (AP7MNV *)pad;

		sv_A = StateVectorCalc(calcParams.src);
		sv_P = StateVectorCalc(calcParams.tgt);

		GET_TIG = OrbMech::HHMMSSToSS(15, 52, 0);

		lambert = set_lambertoptions(sv_A, sv_P, getGETBase(), OrbMech::HHMMSSToSS(15, 52, 0), OrbMech::HHMMSSToSS(26, 25, 0), 7, RTCC_LAMBERT_XAXIS, RTCC_LAMBERT_SPHERICAL, _V(76.5 * 1852, 0, 0), 0);

		LambertTargeting(&lambert, res);

		if (length(res.dV_LVLH) < 1.0*0.3048)
		{
			scrubbed = true;
		}

		if (scrubbed)
		{
			sprintf(upMessage, "Second Phasing Maneuver not necessary.");
		}
		else
		{
			opt.GETbase = getGETBase();
			opt.vessel = calcParams.src;
			opt.TIG = GET_TIG;
			opt.dV_LVLH = res.dV_LVLH;
			opt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), res.dV_LVLH);
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

		int n = 7;
		double lng[] = { -62.9*RAD, -63.0*RAD, -62.9*RAD, -68.5*RAD, -162.4*RAD, -162.3*RAD, -163.3*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(22,10,42),OrbMech::HHMMSSToSS(23,46,41),OrbMech::HHMMSSToSS(25,22,18),OrbMech::HHMMSSToSS(26,56,28),OrbMech::HHMMSSToSS(29,43,42),OrbMech::HHMMSSToSS(31,18,29), OrbMech::HHMMSSToSS(32,53,56) };
		std::string area[] = { "015-1A", "016-1B", "017-1A", "018-1A", "019-4A", "020-4A", "021-4A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

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
		TwoImpulseResuls res;
		AP7ManPADOpt opt;
		VECTOR3 dV_LVLH;
		double GET_TIG_imp, P30TIG, GETBase;
		SV sv_A, sv_P;
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		AP7MNV * form = (AP7MNV *)pad;

		GETBase = getGETBase();

		sv_A = StateVectorCalc(calcParams.src);
		sv_P = StateVectorCalc(calcParams.tgt);

		GET_TIG_imp = OrbMech::HHMMSSToSS(26, 25, 0);

		lambert.axis = RTCC_LAMBERT_MULTIAXIS;
		lambert.DH = 8.0*1852.0;
		lambert.Elevation = 27.45*RAD;
		lambert.GETbase = GETBase;
		lambert.N = 1;
		lambert.NCC_NSR_Flag = true;
		lambert.Perturbation = RTCC_LAMBERT_PERTURBED;
		lambert.PhaseAngle = -1.32*RAD;
		lambert.sv_A = sv_A;
		lambert.sv_P = sv_P;
		lambert.T1 = GET_TIG_imp;
		lambert.T2 = OrbMech::HHMMSSToSS(28, 0, 0);
		lambert.use_XYZ_Offset = false;

		LambertTargeting(&lambert, res);
		PoweredFlightProcessor(sv_A, GETBase, GET_TIG_imp, RTCC_ENGINETYPE_CSMSPS, 0.0, res.dV, false, P30TIG, dV_LVLH);

		opt.GETbase = GETBase;
		opt.vessel = calcParams.src;
		opt.TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
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

		AGCStateVectorUpdate(buffer1, sv_A, true, AGCEpoch, GETBase);
		AGCStateVectorUpdate(buffer2, sv_P, false, AGCEpoch, GETBase);
		AGCExternalDeltaVUpdate(buffer3, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
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
		TwoImpulseResuls res;
		AP7ManPADOpt opt;
		SV sv_A, sv_P;
		double GETBase, GET_TIG_imp, P30TIG;

		AP7MNV * form = (AP7MNV *)pad;

		sv_A = StateVectorCalc(calcParams.src);
		sv_P = StateVectorCalc(calcParams.tgt);

		GETBase = getGETBase();
		GET_TIG_imp = OrbMech::HHMMSSToSS(27, 30, 0);

		lambert.axis = RTCC_LAMBERT_MULTIAXIS;
		lambert.DH = 8.0*1852.0;
		lambert.Elevation = 27.45*RAD;
		lambert.GETbase = GETBase;
		lambert.N = 0;
		lambert.NCC_NSR_Flag = true;
		lambert.Perturbation = RTCC_LAMBERT_PERTURBED;
		lambert.PhaseAngle = -1.32*RAD;
		lambert.sv_A = sv_A;
		lambert.sv_P = sv_P;
		lambert.T1 = GET_TIG_imp;
		lambert.T2 = OrbMech::HHMMSSToSS(28, 0, 0);
		lambert.use_XYZ_Offset = false;

		LambertTargeting(&lambert, res);

		if (length(res.dV) < 10.0*0.3048)
		{
			scrubbed = true;
		}

		if (scrubbed)
		{
			sprintf(upMessage, "NCC2 has been scrubbed.");
		}
		else
		{
			char buffer1[1000];
			VECTOR3 dV_LVLH;
			int enginetype;
			enginetype = SPSRCSDecision(SPS_THRUST / sv_A.mass, res.dV);

			PoweredFlightProcessor(sv_A, GETBase, GET_TIG_imp, enginetype, 0.0, res.dV, false, P30TIG, dV_LVLH);

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

			AGCExternalDeltaVUpdate(buffer1, P30TIG, dV_LVLH);

			sprintf(uplinkdata, "%s", buffer1);
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
		SPQOpt spqopt;
		AP7ManPADOpt opt;
		SPQResults res;
		double P30TIG, GETbase;
		VECTOR3 dV_LVLH;
		SV sv_A, sv_P;
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		sv_A = StateVectorCalc(calcParams.src); //State vector for uplink
		sv_P = StateVectorCalc(calcParams.tgt); //State vector for uplink
		GETbase = getGETBase();

		AP7MNV * form = (AP7MNV *)pad;

		spqopt.E = 27.45*RAD;
		spqopt.GETbase = GETbase;
		spqopt.sv_A = sv_A;
		spqopt.sv_P = sv_P;
		spqopt.t_CSI = -1;
		spqopt.t_CDH = FindDH(sv_A, sv_P, GETbase, 28.0*3600.0, 8.0*1852.0);
		
		ConcentricRendezvousProcessor(spqopt, res);
		PoweredFlightProcessor(sv_A, GETbase, res.t_CDH, RTCC_ENGINETYPE_CSMSPS, 0.0, res.dV_CDH, true, P30TIG, dV_LVLH);

		opt.GETbase = GETbase;
		opt.vessel = calcParams.src;
		opt.TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = false;
		opt.sxtstardtime = 0;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.navcheckGET = 27 * 60 * 60 + 17 * 60;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "NSR");
		sprintf(form->remarks, "heads down, retrograde");

		AGCStateVectorUpdate(buffer1, sv_A, true, AGCEpoch, GETbase);
		AGCStateVectorUpdate(buffer2, sv_P, false, AGCEpoch, GETbase);
		AGCExternalDeltaVUpdate(buffer3, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
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
		TwoImpulseResuls res;
		AP7TPIPADOpt opt;
		SV sv_A, sv_P;
		double GETbase;

		AP7TPI * form = (AP7TPI *)pad;

		sv_A = StateVectorCalc(calcParams.src);
		sv_P = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();

		lambert.axis = RTCC_LAMBERT_MULTIAXIS;
		lambert.DT = 35.0*60.0;
		lambert.Elevation = 27.45*RAD;
		lambert.elevOpt = 1;
		lambert.GETbase = GETbase;
		lambert.N = 0;
		lambert.Perturbation = RTCC_LAMBERT_PERTURBED;
		lambert.sv_A = sv_A;
		lambert.sv_P = sv_P;
		lambert.TPFOpt = 1;

		LambertTargeting(&lambert, res);

		opt.dV_LVLH = res.dV_LVLH;
		opt.GETbase = GETbase;
		opt.TIG = res.T1;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;

		AP7TPIPAD(opt, *form);
	}
	break;
	case 11: //MISSION C FINAL SEPARATION MANEUVER
	{
		AP7ManPADOpt opt;

		AP7MNV * form = (AP7MNV *)pad;

		opt.dV_LVLH = _V(2.0*0.3048, 0.0, 0.0);
		opt.enginetype = RTCC_ENGINETYPE_CSMRCSMINUS4;
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

		int n = 7;
		double lng[] = { -163.3*RAD, 138.8*RAD, 138.5*RAD, 135.6*RAD, -18.0*RAD, -24.0*RAD, -25.4*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(32,53,42),OrbMech::HHMMSSToSS(34,13,54),OrbMech::HHMMSSToSS(35,49,27),OrbMech::HHMMSSToSS(37,24,28),OrbMech::HHMMSSToSS(38,14,11),OrbMech::HHMMSSToSS(39,49,27), OrbMech::HHMMSSToSS(41,26,13) };
		std::string area[] = { "021-4A", "022-3B", "023-3A", "024-3B", "025-AC", "026-AC", "027-2B" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 13: //MISSION C BLOCK DATA 5
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -25.4*RAD, -28.8*RAD, -63.3*RAD, -66.2*RAD, -66.2*RAD, -66.2*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(41,26,13),OrbMech::HHMMSSToSS(43,2,5),OrbMech::HHMMSSToSS(44,27,52),OrbMech::HHMMSSToSS(46,3,34),OrbMech::HHMMSSToSS(47,39,29),OrbMech::HHMMSSToSS(49,15,9) };
		std::string area[] = { "027-2B", "028-2B", "029-1B", "030-1A", "031-1B", "032-1A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 14: //MISSION C BLOCK DATA 6
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -145.0*RAD, 150.0*RAD, 139.0*RAD, -165.9*RAD, 139.0*RAD, 137.4*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(52,5,9),OrbMech::HHMMSSToSS(53,21,42),OrbMech::HHMMSSToSS(54,55,7),OrbMech::HHMMSSToSS(56,46,40),OrbMech::HHMMSSToSS(58,7,17),OrbMech::HHMMSSToSS(59,42,35) };
		std::string area[] = { "033-4C", "034-3C", "035-3B", "036-4A", "037-3A", "038-3A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 15: //MISSION C BLOCK DATA 7
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { 134.5*RAD, -19.9*RAD, -22.9*RAD, -26.4*RAD, -54.9*RAD, -64.9*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(61,17,53),OrbMech::HHMMSSToSS(62,7,40),OrbMech::HHMMSSToSS(63,43,46),OrbMech::HHMMSSToSS(65,19,43),OrbMech::HHMMSSToSS(66,47,22),OrbMech::HHMMSSToSS(68,20,59) };
		std::string area[] = { "039-3B", "040-AC", "041-AC", "042-2A", "043-1C", "044-1A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 16: //MISSION C BLOCK DATA 8
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -63.8*RAD, -63.8*RAD, -64.9*RAD, -165.0*RAD, -165.0*RAD, -137.1*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(69,57,34),OrbMech::HHMMSSToSS(71,33,18),OrbMech::HHMMSSToSS(73,8,47),OrbMech::HHMMSSToSS(75,52,32),OrbMech::HHMMSSToSS(77,28,29),OrbMech::HHMMSSToSS(78,47,51) };
		std::string area[] = { "045-1A", "046-1A", "047-1A", "048-4A", "049-4B", "050-3A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 17: //MISSION C SPS-3: SCS MANEUVER AND SLOSH DAMPING TEST
	{
		AP7MNV * form = (AP7MNV *)pad;

		GMPOpt orbopt;
		REFSMMATOpt refsopt;
		AP7ManPADOpt manopt;
		VECTOR3 dV_LVLH, dV_imp;
		double P30TIG, GETbase, TIG_imp;
		MATRIX3 REFSMMAT;
		SV sv;
		char buffer1[1000];
		char buffer2[1000];

		GETbase = getGETBase();
		sv = StateVectorCalc(calcParams.src);

		orbopt.AltRef = 1;
		orbopt.GETbase = GETbase;
		orbopt.H_A = 160.1*1852.0;
		orbopt.H_P = 90.3*1852.0;
		//Over Carnarvon
		orbopt.long_D = 113.72595*RAD;
		orbopt.ManeuverCode = RTCC_GMP_NHL;
		//Gives DV we had previously, needs fixing eventually
		orbopt.dLAN = 0.3*RAD;
		orbopt.RV_MCC = sv;
		orbopt.TIG_GET= OrbMech::HHMMSSToSS(75, 18, 0);

		GeneralManeuverProcessor(&orbopt, dV_imp, TIG_imp);
		PoweredFlightProcessor(sv, GETbase, TIG_imp, RTCC_ENGINETYPE_CSMSPS, 0.0, dV_imp, false, P30TIG, dV_LVLH);

		refsopt.dV_LVLH = dV_LVLH;
		refsopt.GETbase = GETbase;
		refsopt.REFSMMATTime = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = true;
		manopt.navcheckGET = OrbMech::HHMMSSToSS(75, 5, 0);
		manopt.REFSMMAT = REFSMMAT;
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.sxtstardtime = 0.0;

		AP7ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "SPS-3");

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
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

		int n = 6;
		double lng[] = { 138.0*RAD, 138.0*RAD, 137.0*RAD, -15.0*RAD, -22.0*RAD, -30.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(80,23,36),OrbMech::HHMMSSToSS(82,0,15),OrbMech::HHMMSSToSS(83,36,27),OrbMech::HHMMSSToSS(84,22,7),OrbMech::HHMMSSToSS(85,55,7),OrbMech::HHMMSSToSS(87,28,31) };
		std::string area[] = { "051-3B", "052-3B", "053-3A", "054-AC", "55-AC", "056-AC" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 19: //MISSION C BLOCK DATA 10
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -27.0*RAD, -60.0*RAD, -64.0*RAD, -64.4*RAD, -64.5*RAD, -64.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(89,6,20),OrbMech::HHMMSSToSS(90,30,41),OrbMech::HHMMSSToSS(92,6,54),OrbMech::HHMMSSToSS(93,43,29),OrbMech::HHMMSSToSS(95,20,0),OrbMech::HHMMSSToSS(96,52,38) };
		std::string area[] = { "057-2A", "058-1C", "059-1A", "060-1A", "61-1A", "062-1A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 20: //MISSION C BLOCK DATA 11
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -159.9*RAD, -160.0*RAD, -160.0*RAD, 136.3*RAD, 136.2*RAD, 134.4*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(99,36,59),OrbMech::HHMMSSToSS(101,13,24),OrbMech::HHMMSSToSS(102,46,4),OrbMech::HHMMSSToSS(104,4,38),OrbMech::HHMMSSToSS(105,41,4),OrbMech::HHMMSSToSS(107,13,10) };
		std::string area[] = { "063-4A", "064-4A", "065-4A", "066-3A", "067-3A", "068-3B" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 21: //MISSION C BLOCK DATA 12
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { 130.0*RAD, -23.0*RAD, -32.0*RAD, -27.0*RAD, -61.5*RAD, -64.5*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(108,47,28),OrbMech::HHMMSSToSS(109,37,43),OrbMech::HHMMSSToSS(111,10,33),OrbMech::HHMMSSToSS(112,48,12),OrbMech::HHMMSSToSS(114,13,4),OrbMech::HHMMSSToSS(115,48,12) };
		std::string area[] = { "069-3C", "070-AC", "071-AC", "072-2A", "073-1B", "074-1B" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 22: //MISSION C BLOCK DATA 13
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -65.0*RAD, -65.0*RAD, -63.0*RAD, -160.0*RAD, -160.0*RAD, -161.1*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(117,24,4),OrbMech::HHMMSSToSS(119,0,11),OrbMech::HHMMSSToSS(120,33,36),OrbMech::HHMMSSToSS(123,17,25),OrbMech::HHMMSSToSS(124,53,43),OrbMech::HHMMSSToSS(126,27,32) };
		std::string area[] = { "075-1A", "076-1A", "077-1A", "078-4A", "079-4A", "080-4A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

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
		SV sv;
		char buffer1[1000];
		char buffer2[1000];

		GETbase = getGETBase();
		F = SPS_THRUST;
		t_burn = 0.5;
		m = calcParams.src->GetMass();
		dv = F / m * t_burn;

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
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.navcheckGET = 120.0*3600.0;
		opt.REFSMMAT = REFSMMAT;
		opt.sxtstardtime = -25.0*60.0;
		opt.TIG = P30TIG;
		opt.vessel = calcParams.src;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "SPS-4");

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
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

		int n = 6;
		double lng[] = { 136.0*RAD, 136.0*RAD, 134.0*RAD, 170.0*RAD, -22.0*RAD, -25.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(127,45,11),OrbMech::HHMMSSToSS(129,21,34),OrbMech::HHMMSSToSS(130,53,56),OrbMech::HHMMSSToSS(132,33,15),OrbMech::HHMMSSToSS(133,19,17),OrbMech::HHMMSSToSS(134,53,55) };
		std::string area[] = { "081-3A", "082-3A", "083-3B", "084-CC", "085-AC", "086-2C" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 25: //MISSION C BLOCK DATA 15
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -27.0*RAD, -60.0*RAD, -62.2*RAD, -62.0*RAD, -62.2*RAD, -63.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(136,29,19),OrbMech::HHMMSSToSS(137,54,53),OrbMech::HHMMSSToSS(139,30,6),OrbMech::HHMMSSToSS(141,6,7),OrbMech::HHMMSSToSS(142,42,26),OrbMech::HHMMSSToSS(144,16,25) };
		std::string area[] = { "087-2A", "088-1B", "089-1A", "090-1B", "091-1A", "092-1A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 26: //MISSION C BLOCK DATA 16
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -162.0*RAD, -161.9*RAD, -163.0*RAD, 133.9*RAD, 133.9*RAD, 141.9*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(146,58,14),OrbMech::HHMMSSToSS(148,34,16),OrbMech::HHMMSSToSS(150,9,20),OrbMech::HHMMSSToSS(151,25,41),OrbMech::HHMMSSToSS(153,1,35),OrbMech::HHMMSSToSS(154,38,44) };
		std::string area[] = { "093-4A", "094-4A", "095-4A", "096-3A", "097-3A", "098-3C" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 27: //MISSION C BLOCK DATA 17
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -18.0*RAD, -24.0*RAD, -23.9*RAD, -27.0*RAD, -61.6*RAD, -62.7*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(155,27,54),OrbMech::HHMMSSToSS(157,0,51),OrbMech::HHMMSSToSS(158,35,56),OrbMech::HHMMSSToSS(160,10,26),OrbMech::HHMMSSToSS(161,35,40),OrbMech::HHMMSSToSS(163,10,40) };
		std::string area[] = { "099-AC", "100-AC", "101-2C", "102-2A", "103-1B", "104-1A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 28: //MISSION C SPS-5: PUGS TEST AND MTVC
	{
		AP7MNV * form = (AP7MNV *)pad;

		GMPOpt orbopt;
		REFSMMATOpt refsopt;
		AP7ManPADOpt manopt;
		VECTOR3 dV_LVLH, dV_imp;
		double P30TIG, GETbase, TIG_imp;
		MATRIX3 REFSMMAT;
		SV sv;
		char buffer1[1000];
		char buffer2[1000];

		GETbase = getGETBase();
		sv = StateVectorCalc(calcParams.src);

		orbopt.AltRef = 1;
		orbopt.GETbase = GETbase;
		orbopt.H_A = 240.6*1852.0;
		orbopt.H_P = 89.8*1852.0;
		//Eastern Test Range
		orbopt.long_D = -88.455*RAD;
		orbopt.dLAN = -6.7*RAD;
		orbopt.ManeuverCode = RTCC_GMP_NHL;
		orbopt.RV_MCC = sv;
		orbopt.TIG_GET = OrbMech::HHMMSSToSS(164, 30, 0);

		GeneralManeuverProcessor(&orbopt, dV_imp, TIG_imp);
		PoweredFlightProcessor(sv, GETbase, TIG_imp, RTCC_ENGINETYPE_CSMSPS, 0.0, dV_imp, false, P30TIG, dV_LVLH);

		refsopt.dV_LVLH = dV_LVLH;
		refsopt.GETbase = GETbase;
		refsopt.REFSMMATTime = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
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
		sprintf(form->remarks, "MTVC takeover at TIG+%.0f seconds, manual cutoff at DV counter equal 100 ft/s.", form->burntime - 30.0);

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
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

		int n = 6;
		double lng[] = { -62.7*RAD, -63.1*RAD, -162.5*RAD, -162.5*RAD, -162.5*RAD, 139.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(164,46,6),OrbMech::HHMMSSToSS(166,21,55),OrbMech::HHMMSSToSS(168,59,3),OrbMech::HHMMSSToSS(170,40,38),OrbMech::HHMMSSToSS(172,22,48),OrbMech::HHMMSSToSS(173,34,54) };
		std::string area[] = { "105-1A", "106-1A", "107-4A", "108-4A", "109-4A", "110-3A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 30: //MISSION C BLOCK DATA 19
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { 138.9*RAD, 152.0*RAD, -9.0*RAD, -23.9*RAD, -31.0*RAD, -26.9*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(175,17,19),OrbMech::HHMMSSToSS(177,0,44),OrbMech::HHMMSSToSS(177,42,42),OrbMech::HHMMSSToSS(179,14,47),OrbMech::HHMMSSToSS(180,48,41),OrbMech::HHMMSSToSS(182,26,21) };
		std::string area[] = { "111-3A", "112-3C", "113-AC", "114-AC", "115-AC", "116-2A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 31: //MISSION C BLOCK DATA 20
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -55.2*RAD, -60.0*RAD, -60.0*RAD, -70.2*RAD, -63.0*RAD, -162.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(183,54,59),OrbMech::HHMMSSToSS(185,31,45),OrbMech::HHMMSSToSS(187,12,18),OrbMech::HHMMSSToSS(188,54,8),OrbMech::HHMMSSToSS(190,35,19),OrbMech::HHMMSSToSS(193,9,9) };
		std::string area[] = { "117-1C", "118-1A", "119-1B", "120-1A", "121-1A", "122-4A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 32: //MISSION C BLOCK DATA 21
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -162.0*RAD, -163.5*RAD, -166.0*RAD, -10.0*RAD, -12.0*RAD, -18.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(194,50,14),OrbMech::HHMMSSToSS(196,31,45),OrbMech::HHMMSSToSS(198,9,57),OrbMech::HHMMSSToSS(198,43,50),OrbMech::HHMMSSToSS(200,17,18),OrbMech::HHMMSSToSS(201,50,35) };
		std::string area[] = { "123-4A", "124-4A", "125-CC", "126-AC", "127-AC", "128-AC" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 33: //MISSION C BLOCK DATA 22
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -25.0*RAD, -27.0*RAD, -27.1*RAD, -62.0*RAD, -60.0*RAD, -60.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(203,23,55),OrbMech::HHMMSSToSS(204,58,45),OrbMech::HHMMSSToSS(206,35,31),OrbMech::HHMMSSToSS(208,2,22),OrbMech::HHMMSSToSS(209,40,53),OrbMech::HHMMSSToSS(211,20,47) };
		std::string area[] = { "129-AC", "130-2A", "131-2C", "132-1C", "133-1A", "134-1A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

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
		SV sv;
		char buffer1[1000];
		char buffer2[1000];

		GETbase = getGETBase();
		F = SPS_THRUST;
		t_burn = 0.5;
		m = calcParams.src->GetMass();
		dv = F / m * t_burn;

		sv = StateVectorCalc(calcParams.src);
		dV_LVLH = _V(0.0, dv, 0.0);
		GETbase = getGETBase();
		P30TIG = OrbMech::HHMMSSToSS(210, 8, 0);

		refsopt.dV_LVLH = dV_LVLH;
		refsopt.GETbase = GETbase;
		refsopt.REFSMMATTime = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.navcheckGET = OrbMech::HHMMSSToSS(209, 20, 0);
		opt.REFSMMAT = REFSMMAT;
		opt.sxtstardtime = 0.0;
		opt.TIG = P30TIG;
		opt.vessel = calcParams.src;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "SPS-6");

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
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

		int n = 6;
		double lng[] = { -63.0*RAD, -161.8*RAD, -162.0*RAD, -161.7*RAD, -164.0*RAD, -5.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(213,0,32),OrbMech::HHMMSSToSS(215,38,45),OrbMech::HHMMSSToSS(217,17,27),OrbMech::HHMMSSToSS(218,57,54),OrbMech::HHMMSSToSS(220,39,3),OrbMech::HHMMSSToSS(221,19,6) };
		std::string area[] = { "135-1A", "136-4A", "137-4B", "138-4A", "139-4B", "140-AC" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

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

		int n = 6;
		double lng[] = { -10.0*RAD, -8.0*RAD, -20.0*RAD, -31.0*RAD, -27.0*RAD, -27.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(222,51,52),OrbMech::HHMMSSToSS(224,26,0),OrbMech::HHMMSSToSS(225,58,13),OrbMech::HHMMSSToSS(227,30,42),OrbMech::HHMMSSToSS(229,6,36),OrbMech::HHMMSSToSS(230,43,18) };
		std::string area[] = { "141-AC", "142-AC", "143-AC", "144-AC", "145-2A", "146-2C" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 38: //MISSION C BLOCK DATA 25
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -63.0*RAD, -64.5*RAD, -64.4*RAD, -63.0*RAD, -161.5*RAD, -161.5*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(232,9,47),OrbMech::HHMMSSToSS(233,46,42),OrbMech::HHMMSSToSS(235,25,39),OrbMech::HHMMSSToSS(237,7,5),OrbMech::HHMMSSToSS(239,48,35),OrbMech::HHMMSSToSS(241,29,11) };
		std::string area[] = { "147-1B", "148-1A", "149-1A", "150-1A", "151-4A", "152-4A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 39: //MISSION C SPS-7: SCS MANEUVER
	{
		AP7MNV * form = (AP7MNV *)pad;

		GMPOpt orbopt;
		REFSMMATOpt refsopt;
		AP7ManPADOpt manopt;
		VECTOR3 dV_LVLH, dV_imp;
		double P30TIG, GETbase, TIG_imp;
		MATRIX3 REFSMMAT;
		SV sv;
		char buffer1[1000];
		char buffer2[1000];

		GETbase = getGETBase();
		sv = StateVectorCalc(calcParams.src);

		orbopt.dLOA = -22.5*RAD;
		orbopt.GETbase = GETbase;
		orbopt.ManeuverCode = RTCC_GMP_SAO;
		orbopt.RV_MCC = sv;
		orbopt.TIG_GET = OrbMech::HHMMSSToSS(238, 35, 0);

		GeneralManeuverProcessor(&orbopt, dV_imp, TIG_imp);
		PoweredFlightProcessor(sv, GETbase, TIG_imp, RTCC_ENGINETYPE_CSMSPS, 0.0, dV_imp, false, P30TIG, dV_LVLH);
		dV_LVLH.y = -100.0*0.3048;

		refsopt.dV_LVLH = dV_LVLH;
		refsopt.GETbase = GETbase;
		refsopt.REFSMMATTime = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = true;
		manopt.navcheckGET = OrbMech::HHMMSSToSS(238, 24, 0);
		manopt.REFSMMAT = REFSMMAT;
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.sxtstardtime = 0.0;

		AP7ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "SPS-7");

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
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

		int n = 6;
		double lng[] = { -161.0*RAD, -161.0*RAD, -10.0*RAD, -11.0*RAD, -17.0*RAD, -25.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(243,11,5),OrbMech::HHMMSSToSS(244,47,45),OrbMech::HHMMSSToSS(245,22,27),OrbMech::HHMMSSToSS(246,55,49),OrbMech::HHMMSSToSS(248,28,57),OrbMech::HHMMSSToSS(250,2,0) };
		std::string area[] = { "153-4A", "154-4C", "155-AC", "156-AC", "157-AC", "158-AC" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 41: //MISSION C BLOCK DATA 27
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 7;
		double lng[] = { -33.0*RAD, -26.5*RAD, -62.0*RAD, -64.2*RAD, -64.5*RAD, -64.2*RAD, -67.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(251,35,18),OrbMech::HHMMSSToSS(253,13,19),OrbMech::HHMMSSToSS(254,39,51),OrbMech::HHMMSSToSS(256,16,31),OrbMech::HHMMSSToSS(257,55,28),OrbMech::HHMMSSToSS(259,39,18),OrbMech::HHMMSSToSS(261,16,45) };
		std::string area[] = { "159-AC", "160-2A", "161-1B", "162-1A", "163-1A", "164-1A", "165-1B" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

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
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();

		entopt.vessel = calcParams.src;
		entopt.GETbase = GETbase;
		entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
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
		refsopt.REFSMMATTime = res.P30TIG;
		refsopt.REFSMMATopt = 1;

		REFSMMAT = REFSMMATCalc(&refsopt); //REFSMMAT for uplink

		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.navcheckGET = res.P30TIG - 40.0*60.0;
		opt.REFSMMAT = REFSMMAT;
		opt.sxtstardtime = -20.0*60.0;
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "164-1A RETROFIRE");

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);
		AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
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
		refsopt.REFSMMATTime = TimeofIgnition;
		refsopt.REFSMMATopt = 1;

		REFSMMAT = REFSMMATCalc(&refsopt);

		opt.dV_LVLH = DeltaV_LVLH;
		opt.GETbase = getGETBase();
		opt.P30TIG = TimeofIgnition;
		opt.REFSMMAT = REFSMMAT;
		opt.sv0 = StateVectorCalc(calcParams.src);
		opt.preburn = true;
		opt.lat = SplashLatitude;
		opt.lng = SplashLongitude;

		EarthOrbitEntry(opt, *form);
		sprintf(form->Area[0], "164-1A");
		form->Lat[0] = SplashLatitude * DEG;
		form->Lng[0] = SplashLongitude * DEG;
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
		opt.sv0 = StateVectorCalc(calcParams.src);

		EarthOrbitEntry(opt, *form);
	}
	break;
	case 50: //GENERIC CSM STATE VECTOR UPDATE
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
	case 51: //GENERIC CSM AND TARGET STATE VECTOR UPDATE
	{
		SV sv_A, sv_P;
		double GETbase;
		char buffer1[1000];
		char buffer2[1000];

		sv_A = StateVectorCalc(calcParams.src); //State vector for uplink
		sv_P = StateVectorCalc(calcParams.tgt); //State vector for uplink
		GETbase = getGETBase();

		AGCStateVectorUpdate(buffer1, sv_A, true, AGCEpoch, GETbase);
		AGCStateVectorUpdate(buffer2, sv_P, false, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
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
	case 53: //GENERIC CSM AND TARGET STATE VECTOR UPDATE AND CSM NAV CHECK PAD
	{
		AP7NAV * form = (AP7NAV *)pad;

		SV sv_A, sv_P;
		double GETbase;
		char buffer1[1000];
		char buffer2[1000];

		GETbase = getGETBase();

		sv_A = StateVectorCalc(calcParams.src); //State vector for uplink
		sv_P = StateVectorCalc(calcParams.tgt); //State vector for uplink

		NavCheckPAD(sv_A, *form, GETbase);
		AGCStateVectorUpdate(buffer1, sv_A, true, AGCEpoch, GETbase);
		AGCStateVectorUpdate(buffer2, sv_P, false, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
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