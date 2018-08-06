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
		tliparam.TargetVector = lvdc->TargetVector;
		tliparam.TB5 = lvdc->TB5;
		tliparam.theta_EO = lvdc->theta_EO;
		tliparam.t_D = lvdc->t_D;
		tliparam.T_L = lvdc->T_L;
		tliparam.T_RG = lvdc->T_RG;
		tliparam.T_ST = lvdc->T_ST;

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

		TLIBase = floor((TimeofIgnition / 1800.0) + 0.5)*1800.0; //Round to next half hour
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

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, calcParams.TEPHEM, true);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector and V66");
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
		opt.TIG = calcParams.TLI + 2.0*3600.0;
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

		AGCDesiredREFSMMATUpdate(buffer1, REFSMMAT, AGCEpoch, true);
		sprintf(uplinkdata, "%s", buffer1);

		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "PTC REFSMMAT");
		}
	}
	break;
	case 20: //MCC-1 CALCULATION
	case 21: //MCC-2 CALCULATION AND UPDATE
	{
		double GETbase, MCC1GET, MCC2GET, MCC3GET;
		SV sv;
		char manname[8];

		DeltaV_LVLH = _V(0, 0, 0);
		TimeofIgnition = 0.0;

		if (fcn == 20)
		{
			sprintf(manname, "MCC1");
		}
		else if (fcn == 21)
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
			if (fcn == 20)
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

		//MCC-1 Update
		if (fcn == 20)
		{
			if (scrubbed)
			{
				sprintf(upMessage, "%s has been scrubbed.", manname);
			}
		}
		//MCC-2 Update
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
				manopt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), DeltaV_LVLH);
				manopt.GETbase = GETbase;
				manopt.HeadsUp = true;
				manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
				manopt.TIG = TimeofIgnition;
				manopt.vessel = calcParams.src;
				manopt.vesseltype = 1;

				AP11ManeuverPAD(&manopt, *form);
				sprintf(form->purpose, manname);

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
	case 22: //MCC-1 UPDATE
	{
		AP11ManPADOpt manopt;
		SV sv;
		char buffer1[1000];
		char buffer2[1000];
		char manname[8];

		AP11MNV * form = (AP11MNV *)pad;

		if (fcn == 22)
		{
			sprintf(manname, "MCC1");
		}
		else if (fcn == 23)
		{
			sprintf(manname, "MCC2");
		}

		sv = StateVectorCalc(calcParams.src);

		manopt.dV_LVLH = DeltaV_LVLH;
		manopt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), DeltaV_LVLH);
		manopt.GETbase = calcParams.TEPHEM;
		manopt.HeadsUp = true;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		manopt.TIG = TimeofIgnition;
		manopt.vessel = calcParams.src;
		manopt.vesseltype = 1;

		AP11ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, manname);

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, calcParams.TEPHEM);
		AGCExternalDeltaVUpdate(buffer2, TimeofIgnition, DeltaV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, target load");
		}
	}
	break;
	}

	return scrubbed;
}