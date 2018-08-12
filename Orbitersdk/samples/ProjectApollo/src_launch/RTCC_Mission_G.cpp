/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

RTCC Calculations for Mission G

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
#include "saturnv.h"
#include "LVDC.h"
#include "iu.h"
#include "LEM.h"
#include "../src_rtccmfd/OrbMech.h"
#include "mcc.h"
#include "rtcc.h"

bool RTCC::CalculationMTP_G(int fcn, LPVOID &pad, char * upString, char * upDesc, char * upMessage)
{
	char uplinkdata[1024 * 3];
	bool preliminary = false;
	bool scrubbed = false;

	//Mission Constants
	double AGCEpoch = 40586.767239;
	int LGCREFSAddrOffs = -2;
	int LGCDeltaVAddr = 3433;

	switch (fcn) {
	case 1: //GENERIC CSM STATE VECTOR UPDATE
	{
		SV sv;
		double GETbase;
		char buffer1[1000];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = calcParams.TEPHEM;

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 10: //GROUND LIFTOFF TIME UPDATE
	{
		double TEPHEM0, tephem_scal;

		Saturn *cm = (Saturn *)calcParams.src;

		//Get TEPHEM
		TEPHEM0 = 40403.;
		tephem_scal = GetTEPHEMFromAGC(&cm->agc.vagc);
		calcParams.TEPHEM = (tephem_scal / 8640000.) + TEPHEM0;
	}
	break;
	case 11: //TLI SIMULATION
	{
		SaturnV *SatV = (SaturnV*)calcParams.src;
		LVDCSV *lvdc = (LVDCSV*)SatV->iu->lvdc;

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

		LVDCTLIPredict(tliparam, calcParams.src, getGETBase(), DeltaV_LVLH, TimeofIgnition, calcParams.R_TLI, calcParams.V_TLI, calcParams.TLI);
	}
	break;
	case 12: //TLI+90 MANEUVER PAD
	{
		EntryOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase, TLIBase, TIG, CSMmass;
		SV sv, sv1;
		char buffer1[1000];

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;

		TLIBase = TimeofIgnition;
		TIG = TLIBase + 90.0*60.0;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		CSMmass = 28862.0;

		sv1.mass = CSMmass;
		sv1.gravref = AGCGravityRef(calcParams.src);
		sv1.MJD = GETbase + calcParams.TLI / 24.0 / 3600.0;
		sv1.R = calcParams.R_TLI;
		sv1.V = calcParams.V_TLI;

		entopt.entrylongmanual = false;
		entopt.GETbase = GETbase;
		entopt.impulsive = RTCC_NONIMPULSIVE;
		//Code for Atlantic Ocean Line
		entopt.lng = 2.0;
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
		sprintf(form->remarks, "No ullage, undocked");

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, calcParams.TEPHEM);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 13: //TLI+5 PAD
	{
		AP11BLKOpt opt;
		double CSMmass, GETbase;
		SV sv1;

		P37PAD * form = (P37PAD *)pad;

		GETbase = calcParams.TEPHEM;

		CSMmass = 28862.0;

		sv1.mass = CSMmass;
		sv1.gravref = AGCGravityRef(calcParams.src);
		sv1.MJD = GETbase + calcParams.TLI / 24.0 / 3600.0;
		sv1.R = calcParams.R_TLI;
		sv1.V = calcParams.V_TLI;

		opt.n = 1;

		opt.GETI.push_back(TimeofIgnition + 5.0*3600.0);
		opt.lng.push_back(-165.0*RAD);
		opt.useSV = true;
		opt.RV_MCC = sv1;

		AP11BlockData(&opt, *form);
	}
	break;
	case 14: //TLI PAD
	{
		TLIPADOpt opt;
		double GETbase;

		TLIPAD * form = (TLIPAD *)pad;

		GETbase = calcParams.TEPHEM;

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

		sprintf(form->remarks, "TLI 10-minute abort pitch, 223°");
	}
	break;
	case 15: //TLI Evaluation
	{
		SaturnV *SatV = (SaturnV*)calcParams.src;
		LVDCSV *lvdc = (LVDCSV*)SatV->iu->lvdc;

		if (lvdc->first_op == false)
		{
			scrubbed = true;
		}
	}
	break;
	case 16: //Evasive Maneuver
	{
		AP11ManPADOpt opt;

		AP11MNV * form = (AP11MNV *)pad;

		opt.dV_LVLH = _V(5.1, 0.0, 19.0)*0.3048;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = calcParams.TEPHEM;
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = calcParams.TLI + 1.0*3600.0 + 50.0*60.0;
		opt.vessel = calcParams.src;
		opt.vesseltype = 1;

		AP11ManeuverPAD(&opt, *form);

		sprintf(form->purpose, "Evasive");
	}
	break;
	case 17: //Block Data 1
	case 18: //Block Data 2
	{
		AP11BLKOpt opt;
		double GETbase;

		P37PAD * form = (P37PAD *)pad;

		GETbase = calcParams.TEPHEM;

		double TLIbase = calcParams.TLI - 5.0*60.0 - 20.0; //Approximate TLI ignition

		if (fcn == 17)
		{
			opt.n = 1;

			opt.GETI.push_back(TLIbase + 11.0*3600.0);
			opt.lng.push_back(-165.0*RAD);
		}
		else
		{
			opt.n = 4;

			opt.GETI.push_back(TLIbase + 25.0*3600.0);
			opt.lng.push_back(-165.0*RAD);
			opt.GETI.push_back(TLIbase + 35.0*3600.0);
			opt.lng.push_back(-165.0*RAD);
			opt.GETI.push_back(TLIbase + 44.0*3600.0);
			opt.lng.push_back(-165.0*RAD);
			opt.GETI.push_back(TLIbase + 53.0*3600.0);
			opt.lng.push_back(-165.0*RAD);
		}

		AP11BlockData(&opt, *form);
	}
	break;
	case 19: //PTC REFSMMAT
	{
		char buffer1[1000];

		MATRIX3 REFSMMAT = _M(0.866025404, -0.45872739, -0.19892002, -0.5, -0.79453916, -0.34453958, 0.0, 0.39784004, -0.91745480);

		AGCDesiredREFSMMATUpdate(buffer1, REFSMMAT, AGCEpoch, true, true);
		sprintf(uplinkdata, "%s", buffer1);

		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "PTC REFSMMAT");
		}
	}
	break;
	case 20: //MCC-1 EVALUATION
	case 21: //MCC-1 CALCULATION AND UPDATE
	case 22: //MCC-2 CALCULATION AND UPDATE
	{
		double GETbase, MCC1GET, MCC2GET, MCC3GET;
		SV sv;
		char manname[8];

		DeltaV_LVLH = _V(0, 0, 0);
		TimeofIgnition = 0.0;

		if (fcn == 20 || fcn == 21)
		{
			sprintf(manname, "MCC1");
		}
		else if (fcn == 22)
		{
			sprintf(manname, "MCC2");
		}

		double TLIbase = calcParams.TLI - 5.0*60.0 - 20.0; //Approximate TLI ignition

		MCC1GET = TLIbase + 9.0*3600.0;
		MCC2GET = TLIbase + 24.0*3600.0;
		MCC3GET = calcParams.LOI - 22.0*3600.0;

		sv = StateVectorCalc(calcParams.src);
		GETbase = calcParams.TEPHEM;

		MCCFRMan opt;
		TLMCCResults res;

		opt.type = 0;
		opt.lat = calcParams.PericynthionLatitude;
		opt.PeriGET = calcParams.LOI;
		opt.h_peri = 60.0 * 1852.0;
		opt.alt = calcParams.LSAlt;
		opt.azi = calcParams.LSAzi;
		opt.csmlmdocked = true;
		opt.GETbase = GETbase;

		opt.LOIh_apo = 170.0*1852.0;
		opt.LOIh_peri = 60.0*1852.0;
		opt.LSlat = calcParams.LSLat;
		opt.LSlng = calcParams.LSLng;
		opt.t_land = calcParams.TLAND;
		opt.vessel = calcParams.src;

		//Evaluate MCC-3 DV
		opt.MCCGET = MCC3GET;
		if (TranslunarMidcourseCorrectionTargetingFreeReturn(&opt, &res))
		{
			calcParams.alt_node = res.NodeAlt;
			calcParams.lat_node = res.NodeLat;
			calcParams.lng_node = res.NodeLng;
			calcParams.GET_node = res.NodeGET;
			calcParams.LOI = res.PericynthionGET;
		}

		if (length(res.dV_LVLH) < 25.0*0.3048)
		{
			scrubbed = true;
		}
		else
		{
			if (fcn == 20 || fcn == 21)
			{
				opt.MCCGET = MCC1GET;
			}
			else
			{
				opt.MCCGET = MCC2GET;
			}

			TranslunarMidcourseCorrectionTargetingFreeReturn(&opt, &res);

			//Scrub MCC-1 if DV is less than 50 ft/s
			if (fcn == 20 && length(res.dV_LVLH) < 50.0*0.3048)
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
				DeltaV_LVLH = res.dV_LVLH;
				TimeofIgnition = res.P30TIG;
			}
		}

		//MCC-1 Evaluation
		if (fcn == 20)
		{
			if (scrubbed)
			{
				sprintf(upMessage, "%s has been scrubbed.", manname);
			}
		}
		//MCC-1 and MCC-2 Update
		else
		{
			if (scrubbed)
			{
				char buffer1[1000];

				sprintf(upMessage, "%s has been scrubbed.", manname);
				sprintf(upDesc, "CSM state vector");

				AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);

				sprintf(uplinkdata, "%s", buffer1);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
				}
			}
			else
			{
				char buffer1[1000];
				char buffer2[1000];
				AP11ManPADOpt manopt;

				AP11MNV * form = (AP11MNV *)pad;

				manopt.dV_LVLH = DeltaV_LVLH;
				manopt.enginetype = SPSRCSDecision(SPS_THRUST / (calcParams.src->GetMass() + calcParams.tgt->GetMass()), DeltaV_LVLH);
				manopt.GETbase = GETbase;
				manopt.HeadsUp = false;
				manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
				manopt.TIG = TimeofIgnition;
				manopt.vessel = calcParams.src;
				manopt.vesseltype = 1;

				AP11ManeuverPAD(&manopt, *form);
				sprintf(form->purpose, manname);
				sprintf(form->remarks, "LM weight is %.0f.", form->LMWeight);

				AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
				AGCExternalDeltaVUpdate(buffer2, TimeofIgnition, DeltaV_LVLH);

				sprintf(uplinkdata, "%s%s", buffer1, buffer2);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM state vector, target load");
				}
			}
		}
	}
	break;
	case 23: //Lunar Flyby PAD
	{
		RTEFlybyOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		SV sv;
		double GETbase;
		char buffer1[1000];

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.EntryLng = -165.0*RAD;
		entopt.GETbase = GETbase;
		entopt.returnspeed = 1;
		entopt.FlybyType = 0;
		entopt.TIGguess = calcParams.LOI - 5.0*3600.0;
		entopt.vessel = calcParams.src;

		RTEFlybyTargeting(&entopt, &res);

		SV sv_peri = FindPericynthion(res.sv_postburn);
		double h_peri = length(sv_peri.R) - oapiGetSize(oapiGetObjectByName("Moon"));

		opt.alt = calcParams.LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = SPSRCSDecision(SPS_THRUST / (calcParams.src->GetMass() + calcParams.tgt->GetMass()), res.dV_LVLH);
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

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 24: //MCC-3
	{
		AP11ManPADOpt manopt;
		LOIMan loiopt;
		MCCNodeMan opt;
		VECTOR3 dV_LVLH, dV_LOI;
		SV sv, sv_peri, sv_node, sv_postLOI;
		double GETbase, MCCGET, P30TIG, r_M, TIG_LOI, h_peri, h_node;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;
		r_M = oapiGetSize(oapiGetObjectByName("Moon"));
		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		loiopt.alt = calcParams.LSAlt;
		loiopt.azi = calcParams.LSAzi;
		loiopt.csmlmdocked = true;
		loiopt.GETbase = GETbase;
		loiopt.h_apo = 170.0*1852.0;
		loiopt.h_peri = 60.0*1852.0;
		loiopt.impulsive = RTCC_IMPULSIVE;
		loiopt.lat = calcParams.LSLat;
		loiopt.lng = calcParams.LSLng;
		loiopt.t_land = calcParams.TLAND;
		loiopt.vessel = calcParams.src;

		LOITargeting(&loiopt, dV_LOI, TIG_LOI, sv_node, sv_postLOI);

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
			char buffer1[1000];

			sprintf(upMessage, "MCC-3 has been scrubbed");
			sprintf(upDesc, "CSM state vector");

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);

			sprintf(uplinkdata, "%s", buffer1);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
			}
		}
		else
		{
			char buffer1[1000];
			char buffer2[1000];

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

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
			AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load");
			}
		}
	}
	break;
	case 25: //MCC-4
	{
		LOIMan loiopt;
		REFSMMATOpt refsopt;
		VECTOR3 dV_LVLH, dV_LOI;
		SV sv, sv_peri, sv_node, sv_postLOI;
		MATRIX3 REFSMMAT;
		double GETbase, MCCGET, P30TIG, r_M, TIG_LOI, h_peri, h_node;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;
		r_M = oapiGetSize(oapiGetObjectByName("Moon"));
		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		loiopt.alt = calcParams.LSAlt;
		loiopt.azi = calcParams.LSAzi;
		loiopt.csmlmdocked = true;
		loiopt.GETbase = GETbase;
		loiopt.h_apo = 170.0*1852.0;
		loiopt.h_peri = 60.0*1852.0;
		loiopt.impulsive = RTCC_IMPULSIVE;
		loiopt.lat = calcParams.LSLat;
		loiopt.lng = calcParams.LSLng;
		loiopt.t_land = calcParams.TLAND;
		loiopt.vessel = calcParams.src;

		LOITargeting(&loiopt, dV_LOI, TIG_LOI, sv_node, sv_postLOI);

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
		refsopt.LSAzi = calcParams.LSAzi;
		refsopt.LSLat = calcParams.LSLat;
		refsopt.LSLng = calcParams.LSLng;
		refsopt.REFSMMATopt = 8;
		refsopt.REFSMMATTime = calcParams.TLAND;

		REFSMMAT = REFSMMATCalc(&refsopt);
		calcParams.StoredREFSMMAT = REFSMMAT;

		if (scrubbed)
		{
			char buffer1[1000];
			char buffer2[1000];

			sprintf(upMessage, "MCC-4 has been scrubbed");
			sprintf(upDesc, "CSM state vector, Landing Site REFSMMAT");

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
			AGCDesiredREFSMMATUpdate(buffer2, REFSMMAT, AGCEpoch);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
			}
		}
		else
		{
			AP11ManPADOpt manopt;
			MCCNodeMan opt;
			char buffer1[1000];
			char buffer2[1000];
			char buffer3[1000];

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

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
			AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);
			AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch);

			sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load, Landing Site REFSMMAT");
			}
		}

	}
	break;
	case 26: //PC+2 UPDATE
	{
		RTEFlybyOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;

		entopt.EntryLng = -165.0*RAD;
		entopt.returnspeed = 2;
		entopt.GETbase = GETbase;
		entopt.FlybyType = 1;
		entopt.vessel = calcParams.src;

		RTEFlybyTargeting(&entopt, &res);

		opt.alt = calcParams.LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		//LS REFSMMAT
		opt.REFSMMAT = calcParams.StoredREFSMMAT;
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 1;

		AP11ManeuverPAD(&opt, *form);
		sprintf(form->remarks, "Assumes LS REFSMMAT and docked");

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

			sprintf(form->remarks, "Docked, requires realignment to preferred REFSMMAT");
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
	case 30:	// LOI-1 MANEUVER
	{
		LOIMan opt;
		AP11ManPADOpt manopt;
		double GETbase, P30TIG;
		VECTOR3 dV_LVLH;
		SV sv, sv_n, sv_postLOI;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		opt.csmlmdocked = true;
		opt.GETbase = GETbase;
		opt.h_apo = 170.0*1852.0;
		opt.h_peri = 60.0*1852.0;
		opt.alt = calcParams.LSAlt;
		opt.azi = calcParams.LSAzi;
		opt.lat = calcParams.LSLat;
		opt.lng = calcParams.LSLng;
		opt.t_land = calcParams.TLAND;
		opt.vessel = calcParams.src;

		LOITargeting(&opt, dV_LVLH, P30TIG, sv_n, sv_postLOI);

		manopt.alt = calcParams.LSAlt;
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
		sprintf(form->remarks, "LM weight is %.0f", form->LMWeight);

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

		char buffer1[1000];
		char buffer2[1000];

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
	case 31: //LOI-2 MANEUVER
	{
		LOI2Man opt;
		AP11ManPADOpt manopt;
		double GETbase, P30TIG;
		VECTOR3 dV_LVLH;
		SV sv;
		char buffer1[1000];
		char buffer2[1000];

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		opt.alt = calcParams.LSAlt;
		opt.csmlmdocked = true;
		opt.GETbase = GETbase;
		opt.h_circ = 60.0*1852.0;
		opt.vessel = calcParams.src;

		LOI2Targeting(&opt, dV_LVLH, P30TIG);

		manopt.alt = calcParams.LSAlt;
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		manopt.sxtstardtime = -40.0*60.0;
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.vesseltype = 1;

		AP11ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "LOI-2");

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

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
	case 40: //TEI-1 UPDATE (PRE LOI-1)
	case 41: //TEI-4 UPDATE (PRE LOI-1)
	case 42: //TEI-5 UPDATE (PRE LOI-2)
	case 43: //TEI-11 UPDATE
	case 44: //TEI-30 UPDATE
	{
		TEIOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase;
		SV sv0, sv1, sv2;
		char manname[8];

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;
		sv0 = StateVectorCalc(calcParams.src); //State vector for uplink

		//Simulate the maneuver preceeding TEI (LOI-1 or LOI-2)
		if (fcn == 40 || fcn == 41 || fcn == 42)
		{
			sv1 = ExecuteManeuver(calcParams.src, GETbase, TimeofIgnition, DeltaV_LVLH, sv0, GetDockedVesselMass(calcParams.src));
		}
		else
		{
			sv1 = sv0;
		}

		if (fcn == 40)
		{
			sprintf(manname, "TEI-1");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 41)
		{
			sprintf(manname, "TEI-4");
			sv2 = coast(sv1, 3.5*2.0*3600.0);
		}
		else if (fcn == 42)
		{
			sprintf(manname, "TEI-5");
			sv2 = coast(sv1, 2.5*2.0*3600.0);
		}
		else if (fcn == 43)
		{
			sprintf(manname, "TEI-11");
			sv2 = coast(sv1, 6.5*2.0*3600.0);
		}
		else if (fcn == 44)
		{
			sprintf(manname, "TEI-30");
			sv2 = coast(sv1, 19.5*2.0*3600.0);
		}

		entopt.EntryLng = -165.0*RAD;
		entopt.GETbase = GETbase;
		entopt.Inclination = 40.0*RAD;
		entopt.Ascending = true;
		entopt.returnspeed = 1;
		entopt.RV_MCC = sv2;
		entopt.useSV = true;
		entopt.vessel = calcParams.src;

		TEITargeting(&entopt, &res);

		opt.alt = calcParams.LSAlt;
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

		if (fcn == 40)
		{
			sprintf(form->remarks, "Undocked");
		}
		else if (fcn == 41)
		{
			sprintf(form->remarks, "Undocked, assumes no LOI-2");
		}
		else if (fcn == 42)
		{
			sprintf(form->remarks, "Undocked, assumes LOI-2");
		}

		//Save parameters for further use
		SplashLatitude = res.latitude;
		SplashLongitude = res.longitude;
		calcParams.TEI = res.P30TIG;
		calcParams.EI = res.GET400K;
	}
	break;
	case 60: //REV 1 MAP UPDATE
	{
		SV sv0, sv1, sv2;
		AP10MAPUPDATE upd_hyper, upd_ellip;
		double GETbase;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		GETbase = calcParams.TEPHEM;
		sv0 = StateVectorCalc(calcParams.src);
		LunarOrbitMapUpdate(sv0, GETbase, upd_hyper);

		sv1 = ExecuteManeuver(calcParams.src, GETbase, TimeofIgnition, DeltaV_LVLH, sv0, GetDockedVesselMass(calcParams.src));
		sv2 = coast(sv1, -30.0*60.0);
		LunarOrbitMapUpdate(sv2, GETbase, upd_ellip);

		form->Rev = 1;
		form->type = 2;
		form->AOSGET = upd_hyper.AOSGET;
		form->LOSGET = upd_hyper.LOSGET;
		form->PMGET = upd_ellip.AOSGET;
	}
	break;
	case 61: //REV 4 LANDMARK TRACKING PAD A-1
	{
		LMARKTRKPADOpt opt;

		AP11LMARKTRKPAD * form = (AP11LMARKTRKPAD *)pad;

		opt.GETbase = calcParams.TEPHEM;
		opt.vessel = calcParams.src;

		if (fcn == 50)
		{
			sprintf(form->LmkID[0], "A-1");
			opt.alt[0] = 0;
			opt.lat[0] = 2.0*RAD;
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(82, 40, 0);
			opt.lng[0] = 65.5*RAD;
			opt.entries = 1;
		}

		LandmarkTrackingPAD(&opt, *form);
	}
	break;
	}

	return scrubbed;
}