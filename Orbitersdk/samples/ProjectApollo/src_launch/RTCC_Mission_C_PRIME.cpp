/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

RTCC Calculations for Mission C Prime

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

bool RTCC::CalculationMTP_C_PRIME(int fcn, LPVOID &pad, char * upString, char * upDesc, char * upMessage)
{
	char uplinkdata[1024 * 3];
	bool preliminary = true;
	bool scrubbed = false;

	//Hardcoded for now, better solution at some point...
	double AGCEpoch = 40221.525;
	double LSLat = 2.6317*RAD;
	double LSLng = 34.0253*RAD;
	double LSAlt = -0.82*1852.0;
	double LSAzi = -78.0*RAD;
	double t_land = OrbMech::HHMMSSToSS(82.0, 8.0, 26.0);

	//For old scenarios
	if (calcParams.TEPHEM == 0.0)
	{
		calcParams.TEPHEM = 40211.535417;
	}

	switch (fcn) {
	case 1: //GROUND LIFTOFF TIME UPDATE
	{
		double TEPHEM0, tephem_scal;

		Saturn *cm = (Saturn *)calcParams.src;

		//Get TEPHEM
		TEPHEM0 = 40038.;
		tephem_scal = GetTEPHEMFromAGC(&cm->agc.vagc);
		calcParams.TEPHEM = (tephem_scal / 8640000.) + TEPHEM0;
	}
	break;
	case 2: //TLI SIMULATION
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
		tliparam.t_clock = lvdc->t_clock;

		LVDCTLIPredict(tliparam, calcParams.src, sv, calcParams.TEPHEM, DeltaV_LVLH, TimeofIgnition, sv_IG, sv_TLI);

		calcParams.R_TLI = sv_TLI.R;
		calcParams.V_TLI = sv_TLI.V;
		calcParams.TLI = OrbMech::GETfromMJD(sv_TLI.MJD, calcParams.TEPHEM);
	}
	break;
	case 3: //TLI+90 PAD + State Vector
	case 4: //TLI+4 PAD
	{
		EntryOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase, TLIplus, CSMmass;
		SV sv, sv1, sv2;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;

		if (fcn == 3)
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
		entopt.RV_MCC = sv2;
		entopt.r_rbias = 1350.0;
		entopt.dv_max = 7000.0*0.3048;

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
		form->Weight = CSMmass / 0.45359237;
		form->GET05G = res.GET05G;

		if (fcn == 3)
		{
			char buffer1[1000];
			char buffer2[1000];

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
			AGCStateVectorUpdate(buffer2, sv, false, AGCEpoch, GETbase);

			sprintf(form->purpose, "TLI+90");
			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
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
	case 5: //TLI PAD
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
		opt.SeparationAttitude = _V(PI, 120.0*RAD, 0.0);
		opt.sv0 = sv;
		opt.uselvdc = true;

		TLI_PAD(&opt, *form);

		//calcParams.TLI = TimeofIgnition + form->BurnTime;// ceil(TimeofIgnition / 3600.0)*3600.0;	//Round up to nominally 3 hours, timebase for MCCs and TLC Aborts
	}
	break;
	case 6: //TLI Evaluation
	{
		SaturnV *SatV = (SaturnV*)calcParams.src;
		LVDCSV *lvdc = (LVDCSV*)SatV->iu->GetLVDC();

		if (lvdc->LVDC_Timebase == 5)
		{
			scrubbed = true;
		}
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
		SV sv0;
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

		GETbase = calcParams.TEPHEM;
		sv0 = StateVectorCalc(calcParams.src);

		entopt.entrylongmanual = true;
		entopt.GETbase = GETbase;
		entopt.impulsive = RTCC_NONIMPULSIVE;
		entopt.lng = -165.0*RAD;
		entopt.ReA = 0;
		entopt.RV_MCC = sv0;
		entopt.TIGguess = TLIplus;
		entopt.type = RTCC_ENTRY_ABORT;
		entopt.vessel = calcParams.src;
		entopt.r_rbias = 1350.0;
		entopt.dv_max = 7000.0*0.3048;

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
		SV sv, sv_ig1, sv_cut1;
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

		GETbase = calcParams.TEPHEM;

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
			opt.RV_MCC = sv;
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
			TLMCCResults res;

			opt.lat = calcParams.lat_node;
			opt.lng = calcParams.lng_node;
			opt.NodeGET = calcParams.GET_node;
			opt.h_node = calcParams.alt_node;
			opt.csmlmdocked = false;
			opt.GETbase = GETbase;
			opt.MCCGET = MCCGET;
			opt.RV_MCC = sv;
			opt.vessel = calcParams.src;

			TranslunarMidcourseCorrectionTargetingNodal(opt, res);
			P30TIG = res.P30TIG;
			dV_LVLH = res.dV_LVLH;
			sv_ig1 = res.sv_pre;
			sv_cut1 = res.sv_post;
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
			sprintf(upMessage, "%s has been scrubbed.", manname);
		}
		else
		{
			if (fcn == 23)
			{
				MPTable mpt;

				//Step 1: Add MCC-4 to the mission plan table
				MPTAddManeuver(mpt, sv_ig1, sv_cut1, "MCC", 0.0, 0.0, 2, false);

				LOIMan opt2;
				LOI2Man opt3;
				REFSMMATOpt refsopt;
				SV sv_node, sv_ig2, sv_cut2;
				double P30TIG_LOI, P30TIG_LOI2;
				VECTOR3 dV_LVLH_LOI, dV_LVLH_LOI2;

				//Step 2: Calculate LOI-1 and add it to the mission plan
				opt2.csmlmdocked = false;
				opt2.GETbase = GETbase;
				opt2.h_apo = 170.0*1852.0;
				opt2.h_peri = 60.0*1852.0;
				opt2.alt = LSAlt;
				opt2.azi = LSAzi;
				opt2.lat = LSLat;
				opt2.lng = LSLng;
				opt2.vessel = calcParams.src;
				opt2.t_land = t_land;
				MPTTrajectory(mpt, opt2.RV_MCC, 2);
				//opt2.RV_MCC = ExecuteManeuver(calcParams.src, GETbase, P30TIG, dV_LVLH, sv, 0);

				LOITargeting(&opt2, dV_LVLH_LOI, P30TIG_LOI, sv_node, sv_ig2, sv_cut2);
				MPTAddManeuver(mpt, sv_ig2, sv_cut2, "LOI", 0.0, 0.0, 2, false);

				//Step 3: Calculate LOI-2 to get the TIG
				opt3.alt = LSAlt;
				opt3.csmlmdocked = false;
				opt3.EarliestGET = P30TIG_LOI + 3.5*3600.0;
				opt3.GETbase = GETbase;
				opt3.h_circ = 60.0*1852.0;
				MPTTrajectory(mpt, opt3.RV_MCC, 2);
				opt3.vessel = calcParams.src;
				opt3.vesseltype = 0;

				LOI2Targeting(&opt3, dV_LVLH_LOI2, P30TIG_LOI2);

				//Step 4: Calculate LVLH REFSMMAT at LOI-2 TIG taking into account the trajectory leading up to that point
				refsopt.GETbase = GETbase;
				refsopt.REFSMMATopt = 2;
				refsopt.REFSMMATTime = P30TIG_LOI2;
				refsopt.vessel = calcParams.src;
				refsopt.useSV = true;
				MPTTrajectory(mpt, refsopt.REFSMMATTime, GETbase, refsopt.RV_MCC, 2);

				REFSMMAT = REFSMMATCalc(&refsopt);

				//Step 5: Store SV for use with PC+2
				calcParams.SVSTORE1 = sv_cut1;
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
				char buffer1[1000];
				char buffer2[1000];
				char buffer3[1000];

				AGCStateVectorUpdate(buffer1, sv, false, AGCEpoch, GETbase);
				AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);
				AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch);

				sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "LM state vector, target load, LOI-2 REFSMMAT");
				}
			}
			else
			{
				char buffer1[1000];
				char buffer2[1000];

				AGCStateVectorUpdate(buffer1, sv, false, AGCEpoch, GETbase);
				AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

				sprintf(uplinkdata, "%s%s", buffer1, buffer2);
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

		GETbase = calcParams.TEPHEM;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		opt.csmlmdocked = false;
		opt.GETbase = GETbase;
		opt.h_apo = 170.0*1852.0;
		opt.h_peri = 60.0*1852.0;
		opt.alt = LSAlt;
		opt.azi = LSAzi;
		opt.lat = LSLat;
		opt.lng = LSLng;
		opt.RV_MCC = sv;
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
	}
	break;
	case 40:	// MISSION CP FLYBY MANEUVER
	{
		RTEMoonOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		SV sv;
		double GETbase;

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalc(calcParams.src);
		GETbase = calcParams.TEPHEM;

		entopt.EntryLng = -165.0*RAD;
		entopt.GETbase = GETbase;
		entopt.returnspeed = 0;
		entopt.RV_MCC = sv;
		entopt.TIGguess = calcParams.LOI - 8.0*3600.0;
		entopt.vessel = calcParams.src;
		entopt.SMODE = 14;
		entopt.r_rbias = 1350.0;

		RTEMoonTargeting(&entopt, &res);

		opt.alt = LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		//Save for use with PC+2
		calcParams.SVSTORE1 = res.sv_postburn;

		AP11ManeuverPAD(&opt, *form);

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
		RTEMoonOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		SV sv;
		double GETbase;
		char manname[32];

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalc(calcParams.src);
		GETbase = calcParams.TEPHEM;

		if (fcn == 41)
		{
			//AOL
			entopt.entrylongmanual = true;
			entopt.EntryLng = -25.0*RAD;
			entopt.returnspeed = 1;
			entopt.enginetype = RTCC_ENGINETYPE_RCS;
			sprintf(manname, "PC+2");
		}
		else
		{
			//IOL
			entopt.entrylongmanual = false;
			entopt.EntryLng = 3.0;
			entopt.returnspeed = 2;
			sprintf(manname, "PC+2 fast return");
			entopt.u_rmax = 37500.0*0.3048;
		}

		entopt.RV_MCC = calcParams.SVSTORE1;
		entopt.GETbase = GETbase;
		entopt.TIGguess = calcParams.LOI + 2.0*3600.0;
		entopt.vessel = calcParams.src;
		entopt.SMODE = 14;
		entopt.r_rbias = 1350.0;

		RTEMoonTargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, EntryAng);

		opt.alt = LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		if (fcn == 41)
		{
			opt.enginetype = RTCC_ENGINETYPE_RCS;
		}
		else
		{
			opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		}
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
			refsopt.REFSMMATTime = res.P30TIG;
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
	case 51:	// MISSION CP TEI-2 (Pre LOI)
	{
		RTEMoonOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase;
		SV sv0, sv1;
		char manname[8];

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = calcParams.TEPHEM;
		sv0 = StateVectorCalc(calcParams.src); //State vector for uplink

		sv1 = ExecuteManeuver(calcParams.src, GETbase, TimeofIgnition, DeltaV_LVLH, sv0, 0);

		if (fcn == 50)
		{
			sprintf(manname, "TEI-1");
			entopt.t_zmin = 122.0*3600.0;
		}
		else
		{
			sprintf(manname, "TEI-2");
			entopt.t_zmin = 146.0*3600.0;
			sv1 = coast(sv1, 2.5*3600.0);
		}	

		entopt.EntryLng = -165.0*RAD;
		entopt.GETbase = GETbase;
		entopt.RV_MCC = sv1;
		entopt.vessel = calcParams.src;
		entopt.r_rbias = 1350.0;

		RTEMoonTargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, EntryAng);

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
	case 60: //Map Update Rev 1/2
	{
		SV sv0, sv1, sv2, sv3;
		AP10MAPUPDATE upd_hyper, upd_ellip, upd_ellip2;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc(calcParams.src);
		LunarOrbitMapUpdate(sv0, calcParams.TEPHEM, upd_hyper);

		sv1 = ExecuteManeuver(calcParams.src, calcParams.TEPHEM, TimeofIgnition, DeltaV_LVLH, sv0, 0.0);
		sv2 = coast(sv1, -30.0*60.0);
		LunarOrbitMapUpdate(sv2, calcParams.TEPHEM, upd_ellip);
		sv3 = coast(sv2, 2.0*3600.0);
		LunarOrbitMapUpdate(sv3, calcParams.TEPHEM, upd_ellip2);

		form->type = 3;
		form->Rev = 1;
		form->LOSGET = upd_hyper.LOSGET;
		form->PMGET = upd_hyper.PMGET;
		form->AOSGET = upd_ellip.AOSGET;		
		form->SSGET = upd_ellip.SSGET;
		form->LOSGET2 = upd_ellip2.LOSGET;
		form->SRGET = upd_ellip2.SRGET;
		form->PMGET2 = upd_ellip2.PMGET;
		form->AOSGET2 = upd_ellip2.AOSGET;
		form->SSGET2 = upd_ellip2.SSGET;
	}
	break;
	case 61: //Map Update Rev 2/3
	{
		SV sv0, sv1;
		AP10MAPUPDATE upd_ellip, upd_ellip2;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc(calcParams.src);
		LunarOrbitMapUpdate(sv0, calcParams.TEPHEM, upd_ellip);

		sv1 = ExecuteManeuver(calcParams.src, calcParams.TEPHEM, TimeofIgnition, DeltaV_LVLH, sv0, 0.0);
		LunarOrbitMapUpdate(sv1, calcParams.TEPHEM, upd_ellip2);

		form->type = 1;
		form->Rev = 2;
		form->LOSGET = upd_ellip.LOSGET;
		form->SRGET = upd_ellip.SRGET;
		form->PMGET = upd_ellip.PMGET;
		form->AOSGET = upd_ellip2.AOSGET;
		form->SSGET = upd_ellip2.SSGET;
	}
	break;
	case 62: //Map Update Rev 3/4
	case 63: //Map Update Rev 4/5
	case 64: //Map Update Rev 5/6
	case 65: //Map Update Rev 6/7
	case 66: //Map Update Rev 7/8
	case 67: //Map Update Rev 8/9
	case 68: //Map Update Rev 9/10
	{
		SV sv0, sv1;
		AP10MAPUPDATE upd_ellip;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc(calcParams.src);

		if (fcn == 62)
		{
			form->Rev = 3;
			sv1 = sv0;
		}
		else if (fcn == 63)
		{
			form->Rev = 4;
			sv1 = coast(sv0, 15.0*60.0);
		}
		else if (fcn == 64)
		{
			form->Rev = 5;
			sv1 = sv0;
		}
		else if (fcn == 65)
		{
			form->Rev = 6;
			sv1 = sv0;
		}
		else if (fcn == 66)
		{
			form->Rev = 7;
			sv1 = sv0;
		}
		else if (fcn == 67)
		{
			form->Rev = 8;
			sv1 = sv0;
		}
		else if (fcn == 68)
		{
			form->Rev = 9;
			sv1 = coast(sv0, 35.0*60.0);
		}

		LunarOrbitMapUpdate(sv1, calcParams.TEPHEM, upd_ellip);

		form->type = 1;
		form->LOSGET = upd_ellip.LOSGET;
		form->SRGET = upd_ellip.SRGET;
		form->PMGET = upd_ellip.PMGET;
		form->AOSGET = upd_ellip.AOSGET;
		form->SSGET = upd_ellip.SSGET;
	}
	break;
	case 69: //Map Update TEI
	{
		SV sv0, sv1;
		AP10MAPUPDATE upd_ellip, upd_hyper;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc(calcParams.src);
		LunarOrbitMapUpdate(sv0, calcParams.TEPHEM, upd_ellip);

		sv1 = ExecuteManeuver(calcParams.src, calcParams.TEPHEM, TimeofIgnition, DeltaV_LVLH, sv0, 0.0);
		LunarOrbitMapUpdate(sv1, calcParams.TEPHEM, upd_hyper);

		form->type = 1;
		form->Rev = 10;
		form->LOSGET = upd_ellip.LOSGET;
		form->SRGET = upd_ellip.SRGET;
		form->PMGET = upd_ellip.PMGET;
		form->AOSGET = upd_hyper.AOSGET;
		form->SSGET = 0.0;
	}
	break;
	case 102:	// MISSION CP LOI-2 MANEUVER
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

		opt.alt = LSAlt;
		opt.csmlmdocked = false;
		opt.GETbase = GETbase;
		opt.h_circ = 60.0*1852.0;
		opt.RV_MCC = sv;
		opt.vessel = calcParams.src;

		LOI2Targeting(&opt, dV_LVLH, P30TIG);

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

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
	case 103: //GENERIC CSM STATE VECTOR UPDATE TO LM SLOT
	{
		SV sv;
		double GETbase;
		char buffer1[1000];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
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
		RTEMoonOpt entopt;
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

		GETbase = calcParams.TEPHEM;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.TIGguess = 0.0;
		entopt.EntryLng = -165.0*RAD;
		entopt.GETbase = GETbase;
		entopt.vessel = calcParams.src;
		entopt.returnspeed = 1;
		entopt.RV_MCC = sv;
		entopt.r_rbias = 1350.0;

		RTEMoonTargeting(&entopt, &res);

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
			char buffer1[1000];

			AGCStateVectorUpdate(buffer1, sv, false, AGCEpoch, GETbase);

			sprintf(uplinkdata, "%s", buffer1);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "LM state vector");
			}
		}
		else if (fcn == 200)
		{
			char buffer1[1000];
			char buffer2[1000];
			char buffer3[1000];

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
			AGCStateVectorUpdate(buffer2, sv, false, AGCEpoch, GETbase);
			CMCRetrofireExternalDeltaVUpdate(buffer3, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);

			sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM and LM state vectors, target load");
			}
		}

		if (fcn == 200) //Save only for TEI-10
		{
			TimeofIgnition = res.P30TIG;
			DeltaV_LVLH = res.dV_LVLH;
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
		char buffer1[1000];

		refsopt.GETbase = calcParams.TEPHEM;
		refsopt.REFSMMATopt = 3;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);
		AGCDesiredREFSMMATUpdate(buffer1, REFSMMAT, AGCEpoch);

		sprintf(uplinkdata, "%s", buffer1);
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

		GETbase = calcParams.TEPHEM;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.entrylongmanual = true;
		entopt.GETbase = GETbase;
		entopt.impulsive = RTCC_NONIMPULSIVE;
		entopt.lng = -165.0*RAD;
		entopt.ReA = 0;
		entopt.RV_MCC = sv;
		entopt.TIGguess = MCCtime;
		entopt.vessel = calcParams.src;
		entopt.r_rbias = 1350.0;

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
			sprintf(upMessage, "%s has been scrubbed.", manname);

			//Entry prediction without maneuver
			EntryUpdateCalc(sv, entopt.GETbase, 1350.0, true, &res);

			res.dV_LVLH = _V(0, 0, 0);
			res.P30TIG = entopt.TIGguess;
		}
		else
		{
			if (fcn == 203 || fcn == 206)
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
				char buffer1[1000];
				char buffer2[1000];
				char buffer3[1000];

				AGCStateVectorUpdate(buffer1, sv, false, AGCEpoch, GETbase);
				CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);
				AGCREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch);

				sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "LM state vector, target load, Entry REFSMMAT");
				}
			}
			else if (fcn == 204)//MCC6
			{
				char buffer1[1000];
				char buffer2[1000];

				AGCStateVectorUpdate(buffer1, sv, false, AGCEpoch, GETbase);
				CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);

				sprintf(uplinkdata, "%s%s", buffer1, buffer2);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "LM state vector, target load");
				}
			}
			else if (fcn == 205)//Prel. MCC7
			{
				char buffer1[1000];

				AGCStateVectorUpdate(buffer1, sv, false, AGCEpoch, GETbase);

				sprintf(uplinkdata, "%s", buffer1);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "LM state vector");
				}
			}
			else if (fcn == 206)//MCC7
			{
				char buffer1[1000];
				char buffer2[1000];
				char buffer3[1000];

				AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
				CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);
				AGCREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch);

				sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM state vector, target load, Entry REFSMMAT");
				}
			}
			else if (fcn == 300)//generic MCC
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
		SV sv;
		double GETbase;

		GETbase = calcParams.TEPHEM;
		sv = StateVectorCalc(calcParams.src);

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
		entopt.sv0 = sv;
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
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		GETbase = calcParams.TEPHEM;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.GETbase = GETbase;
		entopt.lat = SplashLatitude;
		entopt.lng = SplashLongitude;
		entopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		entopt.sv0 = sv;
		entopt.vessel = calcParams.src;

		LunarEntryPAD(&entopt, *form);
		sprintf(form->Area[0], "MIDPAC");

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		AGCStateVectorUpdate(buffer2, sv, false, AGCEpoch, GETbase);
		CMCEntryUpdate(buffer3, SplashLatitude, SplashLongitude);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
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