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
	double R_LLS = calcParams.LSAlt + OrbMech::R_Moon;

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
	case 2: //GENERIC LM STATE VECTOR UPDATE
	{
		SV sv;
		double GETbase;
		char buffer1[1000];

		sv = StateVectorCalc(calcParams.tgt); //State vector for uplink
		GETbase = calcParams.TEPHEM;

		AGCStateVectorUpdate(buffer1, sv, false, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector");
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
		LVDCSV *lvdc = (LVDCSV*)SatV->iu->GetLVDC();

		SV sv, sv_IG, sv_TLI;
		sv = StateVectorCalc(calcParams.src);

		LVDCTLIparam tliparam;

		tliparam.alpha_TS = lvdc->alpha_TS;
		tliparam.Azimuth = lvdc->Azimuth;
		tliparam.beta = lvdc->beta;
		tliparam.cos_sigma = lvdc->cos_sigma;
		tliparam.C_3 = lvdc->C_3;
		tliparam.DEC = lvdc->DEC;
		tliparam.e_N = lvdc->e_N;
		tliparam.f = lvdc->f;
		tliparam.mu = lvdc->mu;
		tliparam.omega_E = lvdc->omega_E;
		tliparam.phi_L = lvdc->PHI;
		tliparam.RA = lvdc->RAS;
		tliparam.R_N = lvdc->R_N;
		tliparam.T_2R = lvdc->T_2R;
		tliparam.theta_EO = lvdc->theta_EO;
		tliparam.t_D = lvdc->t_D;
		tliparam.T_L = lvdc->T_L;
		tliparam.T_RG = 578.6;
		tliparam.T_ST = lvdc->T_ST;
		tliparam.Tt_3R = lvdc->Tt_3R;
		tliparam.t_clock = lvdc->t_clock;

		double m0 = calcParams.src->GetEmptyMass();
		LVDCTLIPredict(tliparam, m0, sv, calcParams.TEPHEM, DeltaV_LVLH, TimeofIgnition, sv_IG, sv_TLI);

		calcParams.R_TLI = sv_TLI.R;
		calcParams.V_TLI = sv_TLI.V;
		calcParams.TLI = OrbMech::GETfromMJD(sv_TLI.MJD, calcParams.TEPHEM);
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
		entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		//Code for Atlantic Ocean Line
		entopt.lng = 2.0;
		entopt.ReA = 0;
		entopt.TIGguess = TIG;
		entopt.type = RTCC_ENTRY_ABORT;
		entopt.vessel = calcParams.src;
		entopt.RV_MCC = sv1;

		EntryTargeting(&entopt, &res); //Target Load for uplink

		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
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
		SV sv;
		double GETbase;

		TLIPAD * form = (TLIPAD *)pad;

		sv = StateVectorCalc(calcParams.src);
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
		opt.sv0 = sv;
		opt.uselvdc = true;

		TLI_PAD(&opt, *form);

		sprintf(form->remarks, "TLI 10-minute abort pitch, 223°");
	}
	break;
	case 15: //TLI Evaluation
	{
		SaturnV *SatV = (SaturnV*)calcParams.src;
		LVDCSV *lvdc = (LVDCSV*)SatV->iu->GetLVDC();

		if (lvdc->LVDC_Timebase == 5)
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
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
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
		int engine;
		SV sv, sv_ig1, sv_cut1;
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
		opt.R_LLS = R_LLS;
		opt.azi = calcParams.LSAzi;
		opt.GETbase = GETbase;

		opt.LOIh_apo = 170.0*1852.0;
		opt.LOIh_peri = 60.0*1852.0;
		opt.LSlat = BZLAND.lat[RTCC_LMPOS_BEST];
		opt.LSlng = BZLAND.lng[RTCC_LMPOS_BEST];
		opt.RV_MCC = sv;
		opt.t_land = calcParams.TLAND;

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

		if (length(res.DV) < 25.0*0.3048)
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
			if (fcn == 20 && length(res.DV) < 50.0*0.3048)
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

				engine = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), res.DV);
				PoweredFlightProcessor(sv, GETbase, res.TIG, engine, GetDockedVesselMass(calcParams.src), res.DV, false, TimeofIgnition, DeltaV_LVLH, sv_ig1, sv_cut1);
			}
		}

		//MCC-1 Evaluation
		if (fcn == 20)
		{
			if (scrubbed)
			{
				sprintf(upMessage, "%s has been scrubbed.", manname);
			}
			else
			{
				sprintf(upMessage, "%s will have to be executed.", manname);
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
				CMCExternalDeltaVUpdate(buffer2, TimeofIgnition, DeltaV_LVLH);

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
		RTEMoonOpt entopt;
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
		entopt.SMODE = 14;
		entopt.RV_MCC = sv;
		entopt.TIGguess = calcParams.LOI - 5.0*3600.0;
		entopt.vessel = calcParams.src;
		PZREAP.RRBIAS = 1285.0;
		entopt.t_zmin = 145.0*3600.0;

		RTEMoonTargeting(&entopt, &res);

		SV sv_peri = FindPericynthion(res.sv_postburn);
		double h_peri = length(sv_peri.R) - OrbMech::R_Moon;

		opt.R_LLS = R_LLS;
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
		SV sv, sv_peri, sv_node, sv_ig1, sv_cut1;
		double GETbase, MCCGET, P30TIG, TIG_LOI, h_peri, h_node;
		int engine;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;
		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		loiopt.R_LLS = R_LLS;
		loiopt.azi = calcParams.LSAzi;
		loiopt.csmlmdocked = true;
		loiopt.GETbase = GETbase;
		loiopt.h_apo = 170.0*1852.0;
		loiopt.h_peri = 60.0*1852.0;
		loiopt.impulsive = 1;
		loiopt.lat = BZLAND.lat[RTCC_LMPOS_BEST];
		loiopt.lng = BZLAND.lng[RTCC_LMPOS_BEST];
		loiopt.RV_MCC = sv;
		loiopt.t_land = calcParams.TLAND;
		loiopt.vessel = calcParams.src;

		LOITargeting(&loiopt, dV_LOI, TIG_LOI, sv_node);

		sv_peri = FindPericynthion(sv);

		h_peri = length(sv_peri.R) - OrbMech::R_Moon;
		h_node = length(sv_node.R) - OrbMech::R_Moon;

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
			TLMCCResults res;
			char buffer1[1000];
			char buffer2[1000];

			MCCGET = calcParams.LOI - 22.0*3600.0;

			opt.lat = calcParams.lat_node;
			opt.lng = calcParams.lng_node;
			opt.NodeGET = calcParams.GET_node;
			opt.h_node = calcParams.alt_node;
			opt.GETbase = GETbase;
			opt.MCCGET = MCCGET;
			opt.RV_MCC = sv;

			TranslunarMidcourseCorrectionTargetingNodal(opt, res);

			engine = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), res.DV);
			PoweredFlightProcessor(sv, GETbase, res.TIG, engine, GetDockedVesselMass(calcParams.src), res.DV, false, P30TIG, dV_LVLH, sv_ig1, sv_cut1);

			manopt.dV_LVLH = dV_LVLH;
			manopt.enginetype = engine;
			manopt.GETbase = GETbase;
			manopt.HeadsUp = false;
			manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			manopt.TIG = P30TIG;
			manopt.vessel = calcParams.src;
			manopt.vesseltype = 1;

			AP11ManeuverPAD(&manopt, *form);
			sprintf(form->purpose, "MCC-3");

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
			CMCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

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
		SV sv, sv_peri, sv_node, sv_ig1, sv_cut1;
		MATRIX3 REFSMMAT;
		double GETbase, MCCGET, P30TIG, TIG_LOI, h_peri, h_node;
		int engine;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;
		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		loiopt.R_LLS = R_LLS;
		loiopt.azi = calcParams.LSAzi;
		loiopt.csmlmdocked = true;
		loiopt.GETbase = GETbase;
		loiopt.h_apo = 170.0*1852.0;
		loiopt.h_peri = 60.0*1852.0;
		loiopt.impulsive = 1;
		loiopt.lat = BZLAND.lat[RTCC_LMPOS_BEST];
		loiopt.lng = BZLAND.lng[RTCC_LMPOS_BEST];
		loiopt.RV_MCC = sv;
		loiopt.t_land = calcParams.TLAND;
		loiopt.vessel = calcParams.src;

		LOITargeting(&loiopt, dV_LOI, TIG_LOI, sv_node);

		sv_peri = FindPericynthion(sv);

		h_peri = length(sv_peri.R) - OrbMech::R_Moon;
		h_node = length(sv_node.R) - OrbMech::R_Moon;

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
		refsopt.LSLat = BZLAND.lat[RTCC_LMPOS_BEST];
		refsopt.LSLng = BZLAND.lng[RTCC_LMPOS_BEST];
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
			TLMCCResults res;
			char buffer1[1000];
			char buffer2[1000];
			char buffer3[1000];

			MCCGET = calcParams.LOI - 5.0*3600.0;

			opt.lat = calcParams.lat_node;
			opt.lng = calcParams.lng_node;
			opt.NodeGET = calcParams.GET_node;
			opt.h_node = calcParams.alt_node;
			opt.GETbase = GETbase;
			opt.MCCGET = MCCGET;
			opt.RV_MCC = sv;

			TranslunarMidcourseCorrectionTargetingNodal(opt, res);

			engine = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), res.DV);
			PoweredFlightProcessor(sv, GETbase, res.TIG, engine, GetDockedVesselMass(calcParams.src), res.DV, false, P30TIG, dV_LVLH, sv_ig1, sv_cut1);

			manopt.dV_LVLH = dV_LVLH;
			manopt.enginetype = engine;
			manopt.GETbase = GETbase;
			manopt.HeadsUp = false;
			manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			manopt.TIG = P30TIG;
			manopt.vessel = calcParams.src;
			manopt.vesseltype = 1;

			AP11ManeuverPAD(&manopt, *form);
			sprintf(form->purpose, "MCC-4");

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
			CMCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);
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
		RTEMoonOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		SV sv;
		double GETbase;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;
		sv = StateVectorCalc(calcParams.src);

		entopt.EntryLng = -165.0*RAD;
		entopt.returnspeed = 2;
		entopt.GETbase = GETbase;
		entopt.SMODE = 14;
		entopt.RV_MCC = sv;
		entopt.vessel = calcParams.src;
		entopt.TIGguess = calcParams.LOI + 2.0*3600.0;
		PZREAP.RRBIAS = 1285.0;
		PZREAP.VRMAX = 37500.0;

		RTEMoonTargeting(&entopt, &res);

		//Reset to default
		PZREAP.VRMAX = 36323.0;

		opt.R_LLS = R_LLS;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
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
			refsopt.REFSMMATTime = res.P30TIG;
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
		SV sv, sv_n;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		opt.csmlmdocked = true;
		opt.GETbase = GETbase;
		opt.h_apo = 170.0*1852.0;
		opt.h_peri = 60.0*1852.0;
		opt.R_LLS = R_LLS;
		opt.azi = calcParams.LSAzi;
		opt.lat = BZLAND.lat[RTCC_LMPOS_BEST];
		opt.lng = BZLAND.lng[RTCC_LMPOS_BEST];
		opt.RV_MCC = sv;
		opt.t_land = calcParams.TLAND;
		opt.vessel = calcParams.src;

		LOITargeting(&opt, dV_LVLH, P30TIG, sv_n);

		manopt.R_LLS = R_LLS;
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
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
		CMCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

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
		AP11ManPADOpt manopt;
		double GETbase, P30TIG;
		VECTOR3 dV_LVLH;
		SV sv;
		char buffer1[1000];
		char buffer2[1000];

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;
		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		med_k16.Mode = 2;
		med_k16.Sequence = 3;
		med_k16.GETTH1 = calcParams.LOI + 3.5*3600.0;
		med_k16.GETTH2 = med_k16.GETTH3 = med_k16.GETTH4 = med_k16.GETTH1;
		med_k16.DesiredHeight = 60.0*1852.0;

		LunarDescentPlanningTable table;
		LunarDescentPlanningProcessor(sv, GETbase, BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], R_LLS, table);

		PoweredFlightProcessor(sv, GETbase, table.GETIG[0], RTCC_ENGINETYPE_CSMSPS, 0.0, table.DVVector[0] * 0.3048, true, P30TIG, dV_LVLH);

		manopt.R_LLS = R_LLS;
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
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
		CMCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, target load");
		}
	}
	break;
	case 32: //STATE VECTOR and LS REFSMMAT UPLINK
	{
		AP11LMARKTRKPAD * form = (AP11LMARKTRKPAD *)pad;

		LMARKTRKPADOpt landmarkopt;
		MATRIX3 REFSMMAT;
		VECTOR3 DV;
		double GETbase, t_DOI, t_PDI, t_land, CR;
		SV sv;
		REFSMMATOpt opt;
		DOIMan doiopt;
		char buffer1[1000];
		char buffer2[1000];

		sv = StateVectorCalc(calcParams.tgt); //State vector for uplink
		GETbase = calcParams.TEPHEM;

		doiopt.R_LLS = R_LLS;
		doiopt.EarliestGET = OrbMech::HHMMSSToSS(101, 0, 0);
		doiopt.GETbase = GETbase;
		doiopt.lat = BZLAND.lat[RTCC_LMPOS_BEST];
		doiopt.lng = BZLAND.lng[RTCC_LMPOS_BEST];
		doiopt.N = 0;
		doiopt.opt = 0;
		doiopt.sv0 = sv;

		DOITargeting(&doiopt, DV, t_DOI, t_PDI, t_land, CR);
		calcParams.DOI = t_DOI;
		calcParams.PDI = t_PDI;
		calcParams.TLAND = t_land;

		PoweredFlightProcessor(sv, GETbase, t_DOI, RTCC_ENGINETYPE_LMDPS, 0.0, DV, false, TimeofIgnition, DeltaV_LVLH);

		opt.GETbase = GETbase;
		opt.LSLat = BZLAND.lat[RTCC_LMPOS_BEST];
		opt.LSLng = BZLAND.lng[RTCC_LMPOS_BEST];
		opt.REFSMMATopt = 5;
		opt.REFSMMATTime = calcParams.TLAND;
		opt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&opt);
		calcParams.StoredREFSMMAT = REFSMMAT;

		sprintf(form->LmkID[0], "130");
		landmarkopt.alt[0] = -1.46*1852.0;
		landmarkopt.GETbase = calcParams.TEPHEM;
		landmarkopt.lat[0] = 1.243*RAD;
		landmarkopt.LmkTime[0] = OrbMech::HHMMSSToSS(98, 30, 0);
		landmarkopt.lng[0] = 23.688*RAD;
		landmarkopt.sv0 = sv;
		landmarkopt.entries = 1;

		LandmarkTrackingPAD(&landmarkopt, *form);

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		AGCDesiredREFSMMATUpdate(buffer2, REFSMMAT, AGCEpoch);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, LS REFSMMAT");
		}
	}
	break;
	case 33: //CSM DAP DATA
	{
		AP10DAPDATA * form = (AP10DAPDATA *)pad;

		CSMDAPUpdate(calcParams.src, *form);
	}
	break;
	case 34: //LM DAP DATA
	{
		LMACTDATA * form = (LMACTDATA *)pad;

		AP10DAPDATA dap;

		LMDAPUpdate(calcParams.tgt, dap);
		
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

		form->CSMWeight = dap.OtherVehicleWeight;
		form->LMWeight = dap.ThisVehicleWeight;
		form->PitchTrim = dap.PitchTrim;
		form->RollTrim = dap.YawTrim;
	}
	break;
	case 35: //LGC ACTIVATION UPDATE
	{
		SV sv;
		MATRIX3 REFSMMAT;
		double GETbase;
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = calcParams.TEPHEM;

		REFSMMAT = calcParams.StoredREFSMMAT;

		AGCStateVectorUpdate(buffer1, sv, false, AGCEpoch, GETbase);
		AGCStateVectorUpdate(buffer2, sv, true, AGCEpoch, GETbase);
		AGCREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch, LGCREFSAddrOffs);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "State vectors, LS REFSMMAT");
		}
	}
	break;
	case 36: //AGS ACTIVATION UPDATE
	{
		AP11AGSACT *form = (AP11AGSACT*)pad;

		form->KFactor = 90.0*3600.0;
		form->DEDA224 = 60267;
		form->DEDA225 = 58148;
		form->DEDA226 = 70312;
		form->DEDA227 = -50031;		
	}
	break;
	case 37: //SEPARATION MANEUVER
	{
		AP11ManPADOpt opt;
		SV sv;
		VECTOR3 dV_LVLH;
		double GETbase, t_P, t_Sep;
		char buffer1[1000];
		char buffer2[1000];

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = calcParams.TEPHEM;

		t_P = OrbMech::period(sv.R, sv.V, OrbMech::mu_Moon);
		t_Sep = floor(calcParams.DOI - t_P / 2.0);
		calcParams.SEP = t_Sep;
		dV_LVLH = _V(0, 0, -2.5)*0.3048;

		opt.R_LLS = R_LLS;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMRCSPLUS4;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = t_Sep;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		AGCStateVectorUpdate(buffer2, sv, false, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "State vectors");
		}
	}
	break;
	case 38: //DESCENT ORBIT INSERTION
	{
		AP11LMManPADOpt opt;

		VECTOR3 DV;
		double GETbase, t_PDI, t_land, CR, t_DOI_imp;
		SV sv;
		DOIMan doiopt;
		char TLANDbuffer[64];
		char buffer1[1000];
		char buffer2[1000];

		AP11LMMNV * form = (AP11LMMNV *)pad;

		sv = StateVectorCalc(calcParams.tgt);
		GETbase = calcParams.TEPHEM;

		doiopt.R_LLS = R_LLS;
		doiopt.EarliestGET = OrbMech::HHMMSSToSS(101, 0, 0);
		doiopt.GETbase = GETbase;
		doiopt.lat = BZLAND.lat[RTCC_LMPOS_BEST];
		doiopt.lng = BZLAND.lng[RTCC_LMPOS_BEST];
		doiopt.N = 0;
		doiopt.opt = 0;
		doiopt.sv0 = sv;

		DOITargeting(&doiopt, DV, t_DOI_imp, t_PDI, t_land, CR);

		calcParams.DOI = t_DOI_imp;
		calcParams.PDI = t_PDI;
		calcParams.TLAND = t_land;

		PoweredFlightProcessor(sv, GETbase, t_DOI_imp, RTCC_ENGINETYPE_LMDPS, 0.0, DV, false, TimeofIgnition, DeltaV_LVLH);

		opt.R_LLS = R_LLS;
		opt.csmlmdocked = false;
		opt.dV_LVLH = DeltaV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_LMDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.TIG = TimeofIgnition;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);
		sprintf(form->purpose, "DOI");

		AGCStateVectorUpdate(buffer1, sv, false, AGCEpoch, GETbase);
		LGCExternalDeltaVUpdate(buffer2, TimeofIgnition, DeltaV_LVLH);
		TLANDUpdate(TLANDbuffer, calcParams.TLAND, 2400);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, TLANDbuffer);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector, DOI target load");
		}
	}
	break;
	case 40: //TEI-1 UPDATE (PRE LOI-1)
	case 41: //TEI-4 UPDATE (PRE LOI-1)
	case 42: //TEI-5 UPDATE (PRE LOI-2)
	case 43: //TEI-11 UPDATE
	case 44: //TEI-30 UPDATE (PDI DAY)
	case 45: //TEI-30 UPDATE (PRELIM)
	case 46: //TEI-30 UPDATE (FINAL)
	case 47: //TEI-31 UPDATE
	{
		RTEMoonOpt entopt;
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
			sv1 = ExecuteManeuver(sv0, GETbase, TimeofIgnition, DeltaV_LVLH, GetDockedVesselMass(calcParams.src), RTCC_ENGINETYPE_CSMSPS);
		}
		else
		{
			sv1 = sv0;
		}

		//Mid Pacific Contingency Landing Area
		entopt.EntryLng = -165.0*RAD;

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
		else if (fcn == 45)
		{
			sprintf(manname, "TEI-30");
			sv2 = coast(sv1, 1.5*2.0*3600.0);
			//Nominal EOM area
			entopt.EntryLng = -172.37*RAD;
		}
		else if (fcn == 46)
		{
			sprintf(manname, "TEI-%d", mcc->MoonRev);
			sv2 = coast(sv1, 0.5*3600.0);
			//Nominal EOM area
			entopt.EntryLng = -172.37*RAD;
		}
		else if (fcn == 47)
		{
			sprintf(manname, "TEI-%d", mcc->MoonRev + 1);
			sv2 = coast(sv1, 1.5*2.0*3600.0);
		}

		entopt.GETbase = GETbase;
		entopt.Inclination = -40.0*RAD;
		entopt.returnspeed = 1;
		entopt.RV_MCC = sv2;
		entopt.vessel = calcParams.src;
		PZREAP.RRBIAS = 1285.0;

		RTEMoonTargeting(&entopt, &res);

		opt.R_LLS = R_LLS;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
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
		else if (fcn == 45 || fcn == 46)
		{
			sprintf(form->remarks, "Two-jet ullage for 16 seconds");
		}

		if (fcn != 47)
		{
			//Save parameters for further use
			SplashLatitude = res.latitude;
			SplashLongitude = res.longitude;
			calcParams.TEI = res.P30TIG;
			calcParams.EI = res.GET400K;
		}

		if (fcn == 46)
		{
			char buffer1[1000], buffer2[1000];
			TimeofIgnition = res.P30TIG;
			DeltaV_LVLH = res.dV_LVLH;

			AGCStateVectorUpdate(buffer1, sv2, true, AGCEpoch, GETbase);
			CMCExternalDeltaVUpdate(buffer2, res.P30TIG, res.dV_LVLH);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load");
			}
		}
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

		sv1 = ExecuteManeuver(sv0, GETbase, TimeofIgnition, DeltaV_LVLH, GetDockedVesselMass(calcParams.src), RTCC_ENGINETYPE_CSMSPS);
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
	case 63: //LM TRACKING PAD
	case 64: //LM ACQUISITION TIME
	{
		LMARKTRKPADOpt opt;
		SV sv0;

		AP11LMARKTRKPAD * form = (AP11LMARKTRKPAD *)pad;

		sv0 = StateVectorCalc(calcParams.src);

		opt.GETbase = calcParams.TEPHEM;
		opt.sv0 = sv0;

		if (fcn == 61)
		{
			sprintf(form->LmkID[0], "A-1");
			opt.alt[0] = 0;
			opt.lat[0] = 2.0*RAD;
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(82, 40, 0);
			opt.lng[0] = 65.5*RAD;
			opt.entries = 1;
		}
		else if (fcn == 63 || fcn == 64)
		{
			double LSRad, R_M;
			R_M = 1.73809e6;

			//Update landing site
			calcParams.tgt->GetEquPos(BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.lat[RTCC_LMPOS_BEST], LSRad);
			calcParams.LSAlt = LSRad - R_M;

			sprintf(form->LmkID[0], "Lunar Module");
			opt.alt[0] = calcParams.LSAlt;
			opt.lat[0] = BZLAND.lat[RTCC_LMPOS_BEST];
			if (fcn == 63)
			{
				opt.LmkTime[0] = OrbMech::HHMMSSToSS(104, 30, 0);
			}
			else
			{
				opt.LmkTime[0] = OrbMech::HHMMSSToSS(114, 20, 0);
			}
			opt.lng[0] = BZLAND.lng[RTCC_LMPOS_BEST];
			opt.entries = 1;
		}

		LandmarkTrackingPAD(&opt, *form);
	}
	break;
	case 170: //PDI2 PAD
	{
		//Recalculate PDI and landing times
		DOIMan doiopt;
		SV sv;
		VECTOR3 DV;
		double GETbase, t_DOI_imp, t_PDI, t_land, CR;

		sv = StateVectorCalc(calcParams.tgt);
		GETbase = calcParams.TEPHEM;

		doiopt.R_LLS = R_LLS;
		doiopt.EarliestGET = OrbMech::HHMMSSToSS(101, 0, 0);
		doiopt.GETbase = GETbase;
		doiopt.lat = BZLAND.lat[RTCC_LMPOS_BEST];
		doiopt.lng = BZLAND.lng[RTCC_LMPOS_BEST];
		doiopt.N = 0;
		doiopt.opt = 0;
		doiopt.sv0 = sv;

		DOITargeting(&doiopt, DV, t_DOI_imp, t_PDI, t_land, CR);

		calcParams.PDI = t_PDI;
		calcParams.TLAND = t_land;
		//Fall into PDI PAD calculation
	}
	case 70: //PDI PAD
	{
		AP11PDIPAD * form = (AP11PDIPAD *)pad;

		PDIPADOpt opt;
		SV sv;
		double GETbase;

		sv = StateVectorCalc(calcParams.tgt);
		GETbase = calcParams.TEPHEM;

		opt.direct = false;
		opt.dV_LVLH = DeltaV_LVLH;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.P30TIG = TimeofIgnition;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.R_LS = RLS_from_latlng(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], calcParams.LSAlt);
		opt.sv0 = sv;
		opt.t_land = calcParams.TLAND;
		opt.vessel = calcParams.tgt;

		PDI_PAD(&opt, *form);
	}
	break;
	case 71: //PDI ABORT PAD
	case 75: //CSM RESCUE PAD
	{
		PDIABORTPAD * form = (PDIABORTPAD *)pad;

		SV sv;
		double GETbase, t_sunrise1, t_sunrise2, t_TPI;

		sv = StateVectorCalc(calcParams.src);
		GETbase = calcParams.TEPHEM;

		t_sunrise1 = calcParams.PDI + 3.0*3600.0;
		t_sunrise2 = calcParams.PDI + 5.0*3600.0;

		//Find two TPI opportunities
		t_TPI = FindOrbitalSunrise(sv, GETbase, t_sunrise1) - 23.0*60.0;
		form->T_TPI_Pre10Min = round(t_TPI);
		t_TPI = FindOrbitalSunrise(sv, GETbase, t_sunrise2) - 23.0*60.0;
		form->T_TPI_Post10Min = round(t_TPI);

		//Phasing 67 minutes after PDI
		form->T_Phasing = round(calcParams.PDI + 67.0*60.0);

		if (fcn == 75)
		{
			form->type = 1;
		}
	}
	break;
	case 72: //No PDI+12 PAD
	{
		AP11LMMNV * form = (AP11LMMNV*)pad;

		LambertMan opt;
		AP11LMManPADOpt manopt;
		TwoImpulseResuls res;
		SV sv_LM, sv_DOI, sv_CSM;
		VECTOR3 dV_LVLH;
		double GETbase, t_sunrise, t_CSI, t_TPI, dt, P30TIG, t_P;

		GETbase = calcParams.TEPHEM;
		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);

		sv_DOI = ExecuteManeuver(sv_LM, GETbase, TimeofIgnition, DeltaV_LVLH, 0.0, RTCC_ENGINETYPE_LMDPS);

		t_sunrise = calcParams.PDI + 3.0*3600.0;
		t_TPI = FindOrbitalSunrise(sv_CSM, GETbase, t_sunrise) - 23.0*60.0;

		GZGENCSN.TIElevationAngle = 26.6*RAD;

		opt.mode = 1;
		opt.axis = RTCC_LAMBERT_MULTIAXIS;
		opt.DH = 15.0*1852.0;
		opt.ElevationAngle = 26.6*RAD;
		opt.GETbase = GETbase;
		opt.N = 0;
		opt.Perturbation = RTCC_LAMBERT_PERTURBED;
		//Angle confirmed by Apollo 11 FIDO loop (finally!)
		opt.PhaseAngle = 4.475*RAD;
		opt.sv_A = sv_DOI;
		opt.sv_P = sv_CSM;
		//PDI+12
		opt.T1 = calcParams.PDI + 12.0*60.0;
		//Estimate for T2
		opt.T2 = opt.T1 + 3600.0 + 46.0*60.0;

		for (int i = 0;i < 2;i++)
		{
			LambertTargeting(&opt, res);
			dt = t_TPI - res.t_TPI;
			opt.T2 += dt;
		}

		t_P = OrbMech::period(sv_DOI.R, sv_DOI.V + res.dV, OrbMech::mu_Moon);
		t_CSI = opt.T2 - t_P / 2.0;

		PoweredFlightProcessor(sv_DOI, GETbase, opt.T1, RTCC_ENGINETYPE_LMDPS, 0.0, res.dV, false, P30TIG, dV_LVLH);
		//Store for P76 PAD
		calcParams.TIGSTORE1 = P30TIG;
		calcParams.DVSTORE1 = dV_LVLH;

		manopt.R_LLS = R_LLS;
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_LMDPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		manopt.RV_MCC = sv_DOI;
		manopt.TIG = P30TIG;
		manopt.useSV = true;
		manopt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "No PDI +12 ABORT");

		form->type = 1;
		form->t_CSI = round(t_CSI);
		form->t_TPI = round(t_TPI);
	}
	break;
	case 73: //LUNAR SURFACE DATA CARD
	{
		AP11LunarSurfaceDataCard * form = (AP11LunarSurfaceDataCard*)pad;

		SV sv_CSM, sv_Ins, sv_IG;
		VECTOR3 R_LS, R_C1, V_C1, u, V_C1F, R_CSI1, V_CSI1;
		double T2, GETbase, m0, v_LH, v_LV, theta, dt_asc, t_C1, dt1, dt2, t_CSI1, t_sunrise, t_TPI, dv;
		char buffer1[1000];

		GETbase = calcParams.TEPHEM;
		sv_CSM = StateVectorCalc(calcParams.src);
		
		LEM *l = (LEM*)calcParams.tgt;
		m0 = l->GetAscentStageMass();

		v_LH = 5515.2*0.3048;
		v_LV = 19.6*0.3048;

		T2 = calcParams.PDI + 21.0*60.0 + 24.0;
		R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], calcParams.LSAlt + OrbMech::R_Moon);

		LunarAscentProcessor(R_LS, m0, sv_CSM, GETbase, T2, v_LH, v_LV, theta, dt_asc, dv, sv_IG, sv_Ins);
		dt1 = OrbMech::timetoapo(sv_Ins.R, sv_Ins.V, OrbMech::mu_Moon);
		OrbMech::rv_from_r0v0(sv_Ins.R, sv_Ins.V, dt1, R_C1, V_C1, OrbMech::mu_Moon);
		t_C1 = T2 + dt_asc + dt1;
		u = unit(crossp(R_C1, V_C1));
		V_C1F = V_C1 + unit(crossp(u, V_C1))*10.0*0.3048;
		OrbMech::REVUP(R_C1, V_C1F, 1.5, OrbMech::mu_Moon, R_CSI1, V_CSI1, dt2);
		t_CSI1 = t_C1 + dt2;

		t_sunrise = calcParams.PDI + 7.0*3600.0;
		t_TPI = FindOrbitalSunrise(sv_CSM, GETbase, t_sunrise) - 23.0*60.0;

		form->T2_TIG = T2;
		form->T2_t_CSI1 = round(t_CSI1);
		form->T2_t_Phasing = round(t_C1);
		form->T2_t_TPI = round(t_TPI);

		LunarLiftoffTimeOpt opt;
		LunarLiftoffResults res;
		SV sv_CSM2, sv_CSM_over;
		double MJD_over, t_P, t_PPlusDT, theta_1, dt_1;

		opt.R_LLS = R_LLS;
		opt.GETbase = GETbase;
		opt.lat = BZLAND.lat[RTCC_LMPOS_BEST];
		opt.lng = BZLAND.lng[RTCC_LMPOS_BEST];
		opt.sv_CSM = sv_CSM;
		opt.t_hole = calcParams.PDI + 1.5*3600.0;

		//Initial pass through the processor
		LaunchTimePredictionProcessor(opt, res);
		//Refine ascent parameters
		LunarAscentProcessor(R_LS, m0, sv_CSM, GETbase, res.t_L, res.v_LH, res.v_LV, theta_1, dt_1, dv, sv_IG, sv_Ins);
		opt.theta_1 = theta_1;
		opt.dt_1 = dt_1;
		//Final pass through
		LaunchTimePredictionProcessor(opt, res);

		sv_CSM2 = coast(sv_CSM, calcParams.PDI - OrbMech::GETfromMJD(sv_CSM.MJD, GETbase));
		MJD_over = OrbMech::P29TimeOfLongitude(sv_CSM2.R, sv_CSM2.V, sv_CSM2.MJD, sv_CSM2.gravref, BZLAND.lng[RTCC_LMPOS_BEST]);
		sv_CSM_over = coast(sv_CSM2, (MJD_over - sv_CSM2.MJD)*24.0*3600.0);

		t_P = OrbMech::period(sv_CSM_over.R, sv_CSM_over.V, OrbMech::mu_Moon);
		t_PPlusDT = res.t_L - OrbMech::GETfromMJD(sv_CSM_over.MJD, GETbase);

		form->T3_TIG = round(res.t_L);
		form->T3_t_CSI = round(res.t_CSI);
		form->T3_t_Period = t_P;
		form->T3_t_PPlusDT = t_PPlusDT;
		form->T3_t_TPI = round(res.t_TPI);

		AGCStateVectorUpdate(buffer1, sv_CSM, true, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 76: //P76 PAD for DOI AND NO PDI+12
	{
		AP11P76PAD * form = (AP11P76PAD*)pad;

		SV sv_CSM, sv_LM;
		double GETbase;
		char buffer1[1000];
		char buffer2[1000];

		GETbase = calcParams.TEPHEM;

		//State vectors
		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);

		form->entries = 2;
		sprintf(form->purpose[0], "DOI");
		form->TIG[0] = TimeofIgnition;
		form->DV[0] = DeltaV_LVLH / 0.3048;
		sprintf(form->purpose[1], "PDI1 +12");
		form->TIG[1] = calcParams.TIGSTORE1;
		form->DV[1] = calcParams.DVSTORE1 / 0.3048;

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
	case 77: //DOI EVALUATION
	{
		sprintf(upMessage, "Go for PDI");
	}
	break;
	case 78: //PDI EVALUATION
	{
		SV sv;
		double apo, peri;

		sv = StateVectorCalc(calcParams.tgt);

		OrbMech::periapo(sv.R, sv.V, OrbMech::mu_Moon, apo, peri);

		if (peri - OrbMech::R_Moon > 0.)
		{
			sprintf(upMessage, "Recycle to next PDI attempt");
			scrubbed = true;
		}
	}
	break;
	case 79: //LANDING EVALUATION
	{
		sprintf(upMessage, "We copy you down, Eagle");
	}
	break;
	case 80: //STAY FOR T1
	{
		sprintf(upMessage, "Stay for T1");
	}
	break;
	case 81: //STAY FOR T2
	{
		sprintf(upMessage, "Stay for T2");
	}
	break;
	case 90: //LM ASCENT PAD FOR T3
	{
		AP11LMASCPAD * form = (AP11LMASCPAD*)pad;

		ASCPADOpt ascopt;
		LunarLiftoffTimeOpt opt;
		LunarLiftoffResults res;
		SV sv_CSM, sv_Ins, sv_IG;
		MATRIX3 Rot, Rot2;
		VECTOR3 R_LS;
		char buffer1[100], buffer2[1000];
		double GETbase, m0, theta_1, dt_1, dv;

		GETbase = calcParams.TEPHEM;
		sv_CSM = StateVectorCalc(calcParams.src);

		LEM *l = (LEM*)calcParams.tgt;
		m0 = l->GetAscentStageMass();
		calcParams.tgt->GetRotationMatrix(Rot);
		oapiGetRotationMatrix(sv_CSM.gravref, &Rot2);

		opt.R_LLS = R_LLS;
		opt.GETbase = GETbase;
		opt.lat = BZLAND.lat[RTCC_LMPOS_BEST];
		opt.lng = BZLAND.lng[RTCC_LMPOS_BEST];
		opt.sv_CSM = sv_CSM;
		opt.t_hole = calcParams.PDI + 1.5*3600.0;

		R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], OrbMech::R_Moon + calcParams.LSAlt);

		//Initial pass through the processor
		LaunchTimePredictionProcessor(opt, res);
		//Refine ascent parameters
		LunarAscentProcessor(R_LS, m0, sv_CSM, GETbase, res.t_L, res.v_LH, res.v_LV, theta_1, dt_1, dv, sv_IG, sv_Ins);
		opt.theta_1 = theta_1;
		opt.dt_1 = dt_1;
		//Final pass through
		LaunchTimePredictionProcessor(opt, res);

		calcParams.LunarLiftoff = res.t_L;
		calcParams.Insertion = res.t_Ins;
		calcParams.CSI = res.t_CSI;
		calcParams.CDH = res.t_CDH;
		calcParams.TPI = res.t_TPI;

		ascopt.GETbase = GETbase;
		ascopt.R_LS = R_LS;
		ascopt.sv_CSM = sv_CSM;
		ascopt.TIG = res.t_L;
		ascopt.v_LH = res.v_LH;
		ascopt.v_LV = res.v_LV;
		ascopt.Rot_VL = OrbMech::GetVesselToLocalRotMatrix(Rot, Rot2);

		LunarAscentPAD(ascopt, *form);

		//Store for CSI PAD
		calcParams.DVSTORE1 = _V(res.DV_CSI, 0, 0);
		calcParams.SVSTORE1 = sv_Ins;

		LandingSiteUplink(buffer1, BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], calcParams.LSAlt, 2022);
		AGCStateVectorUpdate(buffer2, sv_CSM, true, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "RLS, CSM state vector");
		}
	}
	break;
	case 91: //CSI PAD FOR T3
	{
		AP10CSI * form = (AP10CSI*)pad;

		AP10CSIPADOpt opt;
		double GETbase;

		GETbase = calcParams.TEPHEM;

		opt.dV_LVLH = calcParams.DVSTORE1;
		opt.enginetype = RTCC_ENGINETYPE_LMRCSPLUS4;
		opt.GETbase = GETbase;
		opt.KFactor = 90.0*3600.0;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.sv0 = calcParams.SVSTORE1;
		opt.t_CSI = calcParams.CSI;
		opt.t_TPI = calcParams.TPI;

		AP10CSIPAD(&opt, *form);
		form->type = 1;
	}
	break;
	case 92: //LIFTOFF TIME UPDATE T4 to T7
	case 97: //LIFTOFF TIME UPDATE T7 to T10
	case 98: //LIFTOFF TIME UPDATE T10 to T13
	{
		LIFTOFFTIMES * form = (LIFTOFFTIMES*)pad;

		LunarLiftoffTimeOpt opt;
		LunarLiftoffResults res;
		SV sv_CSM, sv_CSM_upl, sv_Ins, sv_IG;
		VECTOR3 R_LS;
		double GETbase, m0, theta_1, dt_1, dv;

		GETbase = calcParams.TEPHEM;
		sv_CSM = StateVectorCalc(calcParams.src);

		LEM *l = (LEM*)calcParams.tgt;
		m0 = l->GetAscentStageMass();

		opt.R_LLS = R_LLS;
		opt.GETbase = GETbase;
		opt.lat = BZLAND.lat[RTCC_LMPOS_BEST];
		opt.lng = BZLAND.lng[RTCC_LMPOS_BEST];
		opt.sv_CSM = sv_CSM;
		if (fcn == 92)
		{
			opt.t_hole = calcParams.PDI + 3.5*3600.0;
			form->entries = 4;
			form->startdigit = 4;
		}
		else if (fcn == 97)
		{
			opt.t_hole = calcParams.PDI + 9.5*3600.0;
			form->entries = 4;
			form->startdigit = 7;
		}
		else if (fcn == 98)
		{
			opt.t_hole = calcParams.PDI + 15.0*3600.0;
			form->entries = 4;
			form->startdigit = 10;
		}

		R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], OrbMech::R_Moon + calcParams.LSAlt);

		//Initial pass through the processor
		LaunchTimePredictionProcessor(opt, res);
		//Refine ascent parameters
		LunarAscentProcessor(R_LS, m0, sv_CSM, GETbase, res.t_L, res.v_LH, res.v_LV, theta_1, dt_1, dv, sv_IG, sv_Ins);
		opt.theta_1 = theta_1;
		opt.dt_1 = dt_1;


		for (int i = 0;i < form->entries;i++)
		{
			LaunchTimePredictionProcessor(opt, res);
			form->TIG[i] = res.t_L;
			opt.t_hole += 2.0*3600.0;
		}
	}
	break;
	case 93: //PLANE CHANGE EVALUATION
	{
		SV sv_CSM, sv_Liftoff;
		VECTOR3 R_LS;
		double TIG_nom, GETbase, MJD_TIG_nom, dt1, LmkRange;

		sv_CSM = StateVectorCalc(calcParams.src);
		GETbase = calcParams.TEPHEM;

		//Initial guess for liftoff time
		calcParams.LunarLiftoff = OrbMech::HHMMSSToSS(124, 23, 26);
		TIG_nom = calcParams.LunarLiftoff;
		MJD_TIG_nom = OrbMech::MJDfromGET(TIG_nom, GETbase);
		sv_Liftoff = coast(sv_CSM, (MJD_TIG_nom - sv_CSM.MJD)*24.0*3600.0);

		R_LS = RLS_from_latlng(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], calcParams.LSAlt);

		dt1 = OrbMech::findelev_gs(sv_Liftoff.R, sv_Liftoff.V, R_LS, MJD_TIG_nom, 180.0*RAD, sv_Liftoff.gravref, LmkRange);

		if (abs(LmkRange) < 4.0*1852.0)
		{
			sprintf(upMessage, "Plane Change has been scrubbed");
			scrubbed = true;
		}
	}
	break;
	case 94: //PLANE CHANGE TARGETING (FOR REFSMMAT)
	case 95: //PLANE CHANGE TARGETING (FOR BURN)
	{
		SV sv;

		sv = StateVectorCalc(calcParams.src);

		med_k16.GETTH1 = OrbMech::HHMMSSToSS(106, 30, 0);
		med_k16.GETTH2 = med_k16.GETTH3 = med_k16.GETTH4 = OrbMech::HHMMSSToSS(124, 0, 0);
		med_k16.Mode = 7;
		med_k16.Sequence = 1;
		med_k16.Vehicle = RTCC_MPT_CSM;

		GZGENCSN.LDPPAzimuth = 0.0;

		LunarDescentPlanningTable table;
		LunarDescentPlanningProcessor(sv, calcParams.TEPHEM, BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], R_LLS, table);

		PoweredFlightProcessor(sv, calcParams.TEPHEM, table.GETIG[0], RTCC_ENGINETYPE_CSMSPS, 0.0, table.DVVector[0] * 0.3048, true, TimeofIgnition, DeltaV_LVLH);

		if (fcn == 94)
		{
			REFSMMATOpt refsopt;
			MATRIX3 REFSMMAT;
			char buffer[1000];

			refsopt.dV_LVLH = DeltaV_LVLH;
			refsopt.GETbase = calcParams.TEPHEM;
			refsopt.HeadsUp = false;
			refsopt.REFSMMATTime = TimeofIgnition;
			refsopt.REFSMMATopt = 0;
			refsopt.vessel = calcParams.src;
			refsopt.vesseltype = 0;

			REFSMMAT = REFSMMATCalc(&refsopt);
			AGCDesiredREFSMMATUpdate(buffer, REFSMMAT, AGCEpoch);

			sprintf(uplinkdata, "%s", buffer);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "PC REFSMMAT");
			}
		}
		else
		{
			AP11MNV * form = (AP11MNV*)pad;

			AP11ManPADOpt manopt;
			SV sv_CSM;
			char buffer1[1000], buffer2[1000];

			sv_CSM = StateVectorCalc(calcParams.src);

			manopt.R_LLS = R_LLS;
			manopt.dV_LVLH = DeltaV_LVLH;
			manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
			manopt.GETbase = calcParams.TEPHEM;
			manopt.HeadsUp = false;
			manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			manopt.TIG = TimeofIgnition;
			manopt.vessel = calcParams.src;
			manopt.vesseltype = 0;

			AP11ManeuverPAD(&manopt, *form);
			sprintf(form->purpose, "PLANE CHANGE");

			AGCStateVectorUpdate(buffer1, sv_CSM, true, AGCEpoch, calcParams.TEPHEM);
			CMCExternalDeltaVUpdate(buffer2, TimeofIgnition, DeltaV_LVLH);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load");
			}
		}
	}
	break;
	case 96: //CMC LUNAR LIFTOFF REFSMMAT UPLINK
	{
		REFSMMATOpt opt;
		MATRIX3 REFSMMAT;
		double GETbase;
		char buffer[1000];

		GETbase = calcParams.TEPHEM;

		opt.GETbase = GETbase;
		opt.LSLat = BZLAND.lat[RTCC_LMPOS_BEST];
		opt.LSLng = BZLAND.lng[RTCC_LMPOS_BEST];
		opt.REFSMMATopt = 5;
		opt.REFSMMATTime = calcParams.LunarLiftoff;
		opt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&opt);
		AGCDesiredREFSMMATUpdate(buffer, REFSMMAT, AGCEpoch);

		sprintf(uplinkdata, "%s", buffer);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Liftoff REFSMMAT");
		}
	}
	break;
	case 100: //NOMINAL INSERTION TARGET + LGC SV UPDATE
	{
		LunarLiftoffTimeOpt opt;
		REFSMMATOpt refsopt;
		LunarLiftoffResults res;
		SV sv_CSM, sv_CSM_upl, sv_Ins, sv_IG;
		VECTOR3 R_LS;
		char buffer1[100];
		double GETbase, m0, theta_1, dt_1, dv;

		GETbase = calcParams.TEPHEM;
		sv_CSM = StateVectorCalc(calcParams.src);

		LEM *l = (LEM*)calcParams.tgt;
		m0 = l->GetAscentStageMass();

		opt.R_LLS = R_LLS;
		opt.GETbase = GETbase;
		opt.lat = BZLAND.lat[RTCC_LMPOS_BEST];
		opt.lng = BZLAND.lng[RTCC_LMPOS_BEST];
		opt.sv_CSM = sv_CSM;
		//1.5 hours from "now"
		opt.t_hole = OrbMech::GETfromMJD(sv_CSM.MJD, GETbase) + 1.5*3600.0;

		R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], OrbMech::R_Moon + calcParams.LSAlt);

		//Initial pass through the processor
		LaunchTimePredictionProcessor(opt, res);
		//Refine ascent parameters
		LunarAscentProcessor(R_LS, m0, sv_CSM, GETbase, res.t_L, res.v_LH, res.v_LV, theta_1, dt_1, dv, sv_IG, sv_Ins);
		opt.theta_1 = theta_1;
		opt.dt_1 = dt_1;
		//Final pass through
		LaunchTimePredictionProcessor(opt, res);

		calcParams.LunarLiftoff = res.t_L;
		calcParams.Insertion = res.t_Ins;
		calcParams.CSI = res.t_CSI;
		calcParams.CDH = res.t_CDH;
		calcParams.TPI = res.t_TPI;

		//Calculate T14
		opt.t_hole += 2.0*3600.0;
		LaunchTimePredictionProcessor(opt, res);
		//Store for Ascent PAD
		TimeofIgnition = res.t_L;

		//Calculate Liftoff REFSMMAT
		refsopt.GETbase = GETbase;
		refsopt.LSLat = BZLAND.lat[RTCC_LMPOS_BEST];
		refsopt.LSLng = BZLAND.lng[RTCC_LMPOS_BEST];
		refsopt.REFSMMATopt = 5;
		refsopt.REFSMMATTime = calcParams.LunarLiftoff;
		refsopt.vessel = calcParams.src;
		calcParams.StoredREFSMMAT = REFSMMATCalc(&refsopt);

		//Store for Ascent PAD
		DeltaV_LVLH = _V(res.v_LH, res.v_LV, 0);
		//Store for CSI PAD
		calcParams.DVSTORE1 = _V(res.DV_CSI, 0, 0);
		calcParams.SVSTORE1 = sv_Ins;

		sv_CSM_upl = coast(sv_CSM, res.t_Ins + 18.0*60.0 - OrbMech::GETfromMJD(sv_CSM.MJD, GETbase));
		AGCStateVectorUpdate(buffer1, sv_CSM_upl, true, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 101: //CMC INSERTION STATE VECTORS
	{
		SV sv_LM_upl, sv_CSM_upl;
		double GETbase;
		char buffer1[100], buffer2[100];

		GETbase = calcParams.TEPHEM;

		sv_CSM_upl = StateVectorCalc(calcParams.src);
		sv_LM_upl = coast(calcParams.SVSTORE1, calcParams.Insertion + 18.0*60.0 - OrbMech::GETfromMJD(calcParams.SVSTORE1.MJD, GETbase));

		AGCStateVectorUpdate(buffer1, sv_CSM_upl, true, AGCEpoch, GETbase);
		AGCStateVectorUpdate(buffer2, sv_LM_upl, false, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM and LM state vectors");
		}
	}
	break;
	case 102: //NOMINAL LM ASCENT PAD
	{
		AP11LMASCPAD * form = (AP11LMASCPAD*)pad;

		ASCPADOpt ascopt;
		SV sv_CSM;
		MATRIX3 Rot, Rot2;
		VECTOR3 R_LS;
		double GETbase, m0;
		char buffer1[128];

		GETbase = calcParams.TEPHEM;
		sv_CSM = StateVectorCalc(calcParams.src);
		R_LS = RLS_from_latlng(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], calcParams.LSAlt);

		LEM *l = (LEM*)calcParams.tgt;
		m0 = l->GetAscentStageMass();
		calcParams.tgt->GetRotationMatrix(Rot);
		oapiGetRotationMatrix(sv_CSM.gravref, &Rot2);

		ascopt.GETbase = GETbase;
		ascopt.R_LS = R_LS;
		ascopt.sv_CSM = sv_CSM;
		ascopt.TIG = calcParams.LunarLiftoff;
		ascopt.v_LH = DeltaV_LVLH.x;
		ascopt.v_LV = DeltaV_LVLH.y;
		ascopt.Rot_VL = OrbMech::GetVesselToLocalRotMatrix(Rot, Rot2);

		LunarAscentPAD(ascopt, *form);

		OrbMech::format_time_HHMMSS(buffer1, TimeofIgnition);
		sprintf(form->remarks, "LM weight is %.0f, T%d is %s", m0 / 0.45359237, mcc->MoonRev - 10, buffer1);
	}
	break;
	case 103: //NOMINAL CSI PAD
	{
		AP10CSI * form = (AP10CSI*)pad;

		AP10CSIPADOpt opt;
		double GETbase;

		GETbase = calcParams.TEPHEM;

		opt.dV_LVLH = calcParams.DVSTORE1;
		opt.enginetype = RTCC_ENGINETYPE_LMRCSPLUS4;
		opt.GETbase = GETbase;
		opt.KFactor = 120.0*3600.0;
		opt.REFSMMAT = calcParams.StoredREFSMMAT;
		opt.sv0 = calcParams.SVSTORE1;
		opt.t_CSI = calcParams.CSI;
		opt.t_TPI = calcParams.TPI;

		AP10CSIPAD(&opt, *form);
		form->type = 1;
	}
	break;
	case 104: //LM LIFTOFF EVALUATION
	{
		if (calcParams.tgt->GroundContact())
		{
			sprintf(upMessage, "Launch scrubbed, recycle for next rev");
			scrubbed = true;
		}
	}
	break;
	case 105: //TEI EVALUATION
	{
		SV sv;
		OELEMENTS coe;

		sv = StateVectorCalc(calcParams.src);
		coe = OrbMech::coe_from_sv(sv.R, sv.V, OrbMech::mu_Moon);

		if (coe.e < 0.5)
		{
			scrubbed = true;
		}
	}
	break;
	case 110: //MCC-5 UPDATE
	case 111: //PRELIMINARY MCC-6 UPDATE
	case 112: //MCC-6 UPDATE
	case 113: //MCC-7 DECISION
	case 114: //MCC-7 UPDATE
	{
		EntryOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase, MCCtime;
		MATRIX3 REFSMMAT;
		char manname[8];
		SV sv;

		AP11MNV * form = (AP11MNV *)pad;

		if (fcn == 110)
		{
			MCCtime = calcParams.TEI + 15.0*3600.0;
			sprintf(manname, "MCC-5");
		}
		else if (fcn == 111 || fcn == 112)
		{
			MCCtime = calcParams.EI - 23.0*3600.0;
			sprintf(manname, "MCC-6");
		}
		else if (fcn == 113 || fcn == 114)
		{
			MCCtime = calcParams.EI - 3.0*3600.0;
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

		GETbase = calcParams.TEPHEM;
		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.entrylongmanual = true;
		entopt.GETbase = GETbase;
		entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		entopt.lng = -172.37*RAD;
		entopt.ReA = 0;
		entopt.RV_MCC = sv;
		entopt.TIGguess = MCCtime;
		entopt.vessel = calcParams.src;

		EntryTargeting(&entopt, &res);

		//Apollo 11 Mission Rules
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

		if (fcn != 113)
		{
			if (fcn == 114)
			{
				REFSMMATOpt refsopt;
				refsopt.GETbase = GETbase;
				refsopt.REFSMMATopt = 3;
				refsopt.vessel = calcParams.src;
				refsopt.useSV = true;
				refsopt.RV_MCC = res.sv_postburn;

				REFSMMAT = REFSMMATCalc(&refsopt);
			}
			else
			{
				REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			}

			if (scrubbed)
			{
				//Entry prediction without maneuver
				EntryUpdateCalc(sv, entopt.GETbase, 1285.0, true, &res);

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
		}

		if (scrubbed)
		{
			//Scrubbed MCC-5 and MCC-6
			if (fcn == 110 || fcn == 111 || fcn == 112)
			{
				char buffer1[1000];
				char buffer2[1000];

				sprintf(upMessage, "%s has been scrubbed", manname);
				sprintf(upDesc, "CSM state vector, entry target");

				AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
				CMCEntryUpdate(buffer2, res.latitude, res.longitude);

				sprintf(uplinkdata, "%s%s", buffer1, buffer2);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
				}
			}
			//MCC-7 decision
			else if (fcn == 113)
			{
				sprintf(upMessage, "%s has been scrubbed", manname);
			}
			//Scrubbed MCC-7
			else if (fcn == 114)
			{
				char buffer1[1000];
				char buffer2[1000];
				char buffer3[1000];

				sprintf(upMessage, "%s has been scrubbed", manname);
				sprintf(upDesc, "CSM state vector, entry target, Entry REFSMMAT");

				AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
				CMCEntryUpdate(buffer2, res.latitude, res.longitude);
				AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch);

				sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
				}
			}
		}
		else
		{
			//MCC-5 and MCC-6
			if (fcn == 110 || fcn == 111 || fcn == 112)
			{
				char buffer1[1000];
				char buffer2[1000];

				AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
				CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);

				sprintf(uplinkdata, "%s%s", buffer1, buffer2);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM state vector, target load");
				}
			}
			//MCC-7 decision
			else if (fcn == 113)
			{
				sprintf(upMessage, "%s will be executed", manname);
			}
			//MCC-7
			else if (fcn == 114)
			{
				char buffer1[1000];
				char buffer2[1000];
				char buffer3[1000];

				AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
				CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);
				AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch);

				sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
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
		calcParams.SVSTORE1 = res.sv_postburn;
	}
	break;
	case 116: //ENTRY PAD (ASSUMES MCC-6)
	case 117: //ENTRY PAD (ASSUMES MCC-7)
	case 118: //FINAL LUNAR ENTRY PAD
	{
		AP11ENT * form = (AP11ENT *)pad;

		SV sv;
		LunarEntryPADOpt entopt;
		MATRIX3 REFSMMAT;
		double GETbase;

		GETbase = calcParams.TEPHEM;
		sv = StateVectorCalc(calcParams.src);

		//Burn scrubbed or not
		if (length(DeltaV_LVLH) != 0.0 && fcn != 118)
		{
			entopt.direct = false;
		}
		else
		{
			entopt.direct = true;
		}

		if (fcn == 118)
		{
			REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		}
		else
		{
			REFSMMATOpt refsopt;
			refsopt.GETbase = GETbase;
			refsopt.REFSMMATopt = 3;
			refsopt.vessel = calcParams.src;
			refsopt.useSV = true;
			refsopt.RV_MCC = calcParams.SVSTORE1;

			REFSMMAT = REFSMMATCalc(&refsopt);
		}

		entopt.dV_LVLH = DeltaV_LVLH;
		entopt.GETbase = GETbase;
		entopt.lat = SplashLatitude;
		entopt.lng = SplashLongitude;
		entopt.P30TIG = TimeofIgnition;
		entopt.REFSMMAT = REFSMMAT;
		entopt.sv0 = sv;

		LunarEntryPAD(&entopt, *form);
		sprintf(form->Area[0], "MIDPAC");
		if (entopt.direct == false)
		{
			if (fcn == 116 && length(DeltaV_LVLH) != 0.0)
			{
				sprintf(form->remarks[0], "Assumes MCC6");
			}
			else if (fcn == 117 && length(DeltaV_LVLH) != 0.0)
			{
				sprintf(form->remarks[0], "Assumes MCC7");
			}
		}

		if (fcn == 118)
		{
			char buffer1[1000];
			char buffer2[1000];

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase, true);
			CMCEntryUpdate(buffer2, SplashLatitude, SplashLongitude);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "State vectors, entry update");
			}
		}
	}
	break;
	}

	return scrubbed;
}