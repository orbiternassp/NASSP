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
	double t_land = OrbMech::HHMMSSToSS(82.0, 8.0, 26.0);

	switch (fcn) {
	case 1: //MISSION INITIALIZATION GROUND LIFTOFF TIME UPDATE
	{
		char Buff[128];

		//P80 MED: mission initialization
		sprintf_s(Buff, "P80,1,CSM,%d,%d,%d;", GZGENCSN.MonthofLiftoff, GZGENCSN.DayofLiftoff, GZGENCSN.Year);
		GMGMED(Buff);

		//P10 MED: Enter actual liftoff time
		double TEPHEM0, tephem_scal;
		Saturn *cm = (Saturn *)calcParams.src;

		//Get TEPHEM
		TEPHEM0 = 40038.;
		tephem_scal = GetTEPHEMFromAGC(&cm->agc.vagc, true);
		double LaunchMJD = (tephem_scal / 8640000.) + TEPHEM0;
		LaunchMJD = (LaunchMJD - SystemParameters.GMTBASE)*24.0;

		int hh, mm;
		double ss;

		OrbMech::SStoHHMMSS(LaunchMJD*3600.0, hh, mm, ss, 0.01);

		sprintf_s(Buff, "P10,CSM,%d:%d:%.2lf;", hh, mm, ss);
		GMGMED(Buff);

		//P12: CSM GRR and Azimuth
		SaturnV *SatV = (SaturnV*)cm;
		LVDCSV *lvdc = (LVDCSV*)SatV->iu->GetLVDC();
		double Azi = lvdc->Azimuth*DEG;
		double T_GRR = lvdc->T_L;

		sprintf_s(Buff, "P12,CSM,%d:%d:%.2lf,%.2lf;", hh, mm, ss, Azi);
		GMGMED(Buff);

		//P15: CMC clock zero
		sprintf_s(Buff, "P15,AGC,%d:%d:%.2lf;", hh, mm, ss);
		GMGMED(Buff);

		//P12: IU GRR and Azimuth
		OrbMech::SStoHHMMSS(T_GRR, hh, mm, ss, 0.01);
		sprintf_s(Buff, "P12,IU1,%d:%d:%.2lf,%.2lf;", hh, mm, ss, Azi);
		GMGMED(Buff);

		//Get actual liftoff REFSMMAT from telemetry
		BZSTLM.CMC_REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		BZSTLM.CMCRefsPresent = true;
		EMSGSUPP(1, 1);
		//Make telemetry matrix current
		GMGMED("G00,CSM,TLM,CSM,CUR;");

		//F62: Interpolate SFP
		sprintf_s(Buff, "F62,,1,%.3lf;", Azi);
		GMGMED(Buff);
	}
	break;
	case 2: //TLI SIMULATION
	{
		if (PZMPTCSM.ManeuverNum > 0)
		{
			//Delete maneuvers from MPT
			GMGMED("M62,CSM,1,D;");
		}

		//Config and mass update
		med_m55.Table = RTCC_MPT_CSM;
		MPTMassUpdate(calcParams.src, med_m50, med_m55, med_m49);
		PMMWTC(55);
		med_m50.Table = RTCC_MPT_CSM;
		med_m50.WeightGET = GETfromGMT(RTCCPresentTimeGMT());
		PMMWTC(50);

		//Trajectory Update
		StateVectorTableEntry sv0;
		sv0.Vector = StateVectorCalcEphem(calcParams.src);
		sv0.LandingSiteIndicator = false;
		sv0.VectorCode = "APIC001";

		PMSVCT(4, RTCC_MPT_CSM, sv0);

		//Add TLI to MPT
		if (GETEval2(3.0*3600.0))
		{
			//Second opportunity
			GMGMED("M68,CSM,2;");
		}
		else
		{
			//First opportunity
			GMGMED("M68,CSM,1;");
		}

		//Add separation maneuver to MPT
		GMGMED("M40,P1,0.0;");
		med_m66.Table = RTCC_MPT_CSM;
		med_m66.ReplaceCode = 0; //Don't replace
		med_m66.GETBI = GETfromGMT(PZMPTCSM.mantable[0].GMT_BO) + 15.0*60.0;
		med_m66.Thruster = RTCC_ENGINETYPE_CSMRCSPLUS4;
		med_m66.AttitudeOpt = RTCC_ATTITUDE_INERTIAL;
		med_m66.BurnParamNo = 1;
		med_m66.CoordInd = 0; //LVLH
		med_m66.Att = _V(60.0*RAD, PI, PI);
		med_m66.ConfigChangeInd = RTCC_CONFIGCHANGE_UNDOCKING;
		med_m66.FinalConfig = "C";

		//Dummy data
		std::vector<std::string> str;
		PMMMED("66", str);

		//Save TLI time
		calcParams.TLI = GETfromGMT(PZMPTCSM.mantable[0].GMT_BO);
	}
	break;
	case 3: //TLI+90 PAD + State Vector
	case 4: //TLI+4 PAD
	{
		EntryOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double TLIplus;
		SV sv, sv1, sv2;

		AP11MNV * form = (AP11MNV *)pad;

		if (fcn == 3)
		{
			TLIplus = 90.0*60.0;
			entopt.lng = -30.0*RAD;
			entopt.t_Z = 17.0*3600.0 + 47.0*60.0;
		}
		else
		{
			TLIplus = 4.0*3600.0;
			entopt.lng = -165.0 * RAD;
			entopt.t_Z = 26.0*3600.0 + 43.0*60.0;
		}

		sv1.mass = PZMPTCSM.mantable[0].CommonBlock.CSMMass;
		sv1.gravref = hEarth;
		sv1.MJD = OrbMech::MJDfromGET(PZMPTCSM.mantable[0].GMT_BO, SystemParameters.GMTBASE);
		sv1.R = PZMPTCSM.mantable[0].R_BO;
		sv1.V = PZMPTCSM.mantable[0].V_BO;

		sv2 = coast(sv1, TLIplus);

		entopt.entrylongmanual = true;
		entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		entopt.TIGguess = calcParams.TLI + TLIplus;
		entopt.type = 1;
		entopt.vessel = calcParams.src;
		entopt.RV_MCC = sv2;
		entopt.r_rbias = PZREAP.RRBIAS;
		entopt.dv_max = 7000.0*0.3048;

		EntryTargeting(&entopt, &res); //Target Load for uplink

		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = true;
		opt.REFSMMAT = EZJGMTX1.data[0].REFSMMAT;
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
		form->Weight = PZMPTCSM.mantable[0].CommonBlock.CSMMass / 0.45359237;
		form->GET05G = res.GET05G;

		if (fcn == 3)
		{
			EphemerisData sv, sv_uplink;
			double GMTSV;

			GMTSV = PZMPTCSM.TimeToBeginManeuver[0] - 10.0*60.0; //10 minutes before TB6
			
			sv = StateVectorCalcEphem(calcParams.src);
			sv_uplink = coast(sv, GMTSV - sv.GMT, RTCC_MPT_CSM); //Coast with venting and drag taken into account

			char buffer1[1000];
			char buffer2[1000];

			AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv_uplink);
			AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv_uplink);

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
		SV sv;

		TLIPAD * form = (TLIPAD *)pad;

		//DMT for TLI
		GMGMED("U20,CSM,1;");

		form->TB6P = DMTBuffer[0].GETI - 9.0*60.0 - 38.0;
		form->IgnATT = DMTBuffer[0].IMUAtt;
		form->BurnTime = DMTBuffer[0].DT_B;
		form->dVC = DMTBuffer[0].DVC;
		form->VI = length(PZMPTCSM.mantable[0].V_BO) / 0.3048;
		form->type = 1; //Don't show extraction angles

		//DMT for sep maneuver
		GMGMED("U20,CSM,2;");
		form->SepATT = DMTBuffer[0].IMUAtt;

		//Delete maneuvers from MPT
		GMGMED("M62,CSM,1,D;");

		//Set anchor vector time to 0, so that no trajectory updates are done anymore
		EZANCHR1.AnchorVectors[9].Vector.GMT = 0.0;
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
		double TLIplus;
		char manname[8];

		AP11MNV * form = (AP11MNV *)pad;

		if (fcn == 10)
		{
			TLIplus = calcParams.TLI + 11.0*3600.0;
			sprintf(manname, "TLI+11");
			entopt.t_Z = OrbMech::HHMMSSToSS(50.0, 4.0, 0.0);
		}
		else if (fcn == 11)
		{
			TLIplus = calcParams.TLI + 25.0*3600.0;
			sprintf(manname, "TLI+25");
			entopt.t_Z = OrbMech::HHMMSSToSS(74.0, 38.0, 0.0);
		}
		else if (fcn == 12)
		{
			TLIplus = calcParams.TLI + 35.0*3600.0;
			sprintf(manname, "TLI+35");
			entopt.t_Z = OrbMech::HHMMSSToSS(98.0, 12.0, 0.0);
		}
		else if (fcn == 13)
		{
			TLIplus = calcParams.TLI + 44.0*3600.0;
			sprintf(manname, "TLI+44");
			entopt.t_Z = OrbMech::HHMMSSToSS(98.0, 12.0, 0.0);
		}

		sv0 = StateVectorCalc(calcParams.src);

		entopt.entrylongmanual = true;
		entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		entopt.lng = -165.0*RAD;
		entopt.RV_MCC = sv0;
		entopt.TIGguess = TLIplus;
		entopt.type = 1;
		entopt.vessel = calcParams.src;
		entopt.r_rbias = PZREAP.RRBIAS;
		entopt.dv_max = 7000.0*0.3048;

		EntryTargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, ReA, prec); //Target Load for uplink

		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = true;
		opt.REFSMMAT = EZJGMTX1.data[0].REFSMMAT;
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
		EphemerisData sv_ephem;
		SV sv, sv_ig1, sv_cut1;
		double P30TIG, MCCGET, tig, dv_thres;
		int engine;
		MATRIX3 REFSMMAT;
		VECTOR3 dV_LVLH, dv;
		char manname[8];

		AP11MNV * form = (AP11MNV *)pad;

		if (fcn == 20)
		{
			MCCGET = calcParams.TLI + 6.0*3600.0;
			sprintf(manname, "MCC1");
			REFSMMAT = EZJGMTX1.data[0].REFSMMAT;
			PZMCCPLN.SFPBlockNum = 1;
			dv_thres = 5.0*0.3048;
		}
		else if (fcn == 21)
		{
			MCCGET = calcParams.TLI + 25.0*3600.0;
			sprintf(manname, "MCC2");
			REFSMMAT = EZJGMTX1.data[0].REFSMMAT;
			PZMCCPLN.SFPBlockNum = 2;
			dv_thres = 5.0*0.3048;
		}
		else if (fcn == 22)
		{
			MCCGET = calcParams.LOI - 22.0*3600.0;
			sprintf(manname, "MCC3");
			REFSMMAT = EZJGMTX1.data[0].REFSMMAT;
			PZMCCPLN.SFPBlockNum = 2;
			dv_thres = 1.0*0.3048;
		}
		else
		{
			MCCGET = calcParams.LOI - 8.0*3600.0;
			sprintf(manname, "MCC4");
			PZMCCPLN.SFPBlockNum = 2;
			dv_thres = 0.2*0.3048; //Mission techniques say "MCC-4 will be performed regardless of its magnitude and residuals will also be nulled to 0.2 foot per second."
		}

		//Yeah, this is bad
		sv = StateVectorCalc(calcParams.src);
		sv_ephem = StateVectorCalcEphem(calcParams.src);

		PZMCCPLN.LOPC_M = 1;
		PZMCCPLN.LOPC_N = 0;
		PZMCCPLN.REVS2 = 4;
		PZMCCPLN.MidcourseGET = MCCGET;
		PZMCCPLN.Config = false;
		PZMCCPLN.Column = 1;

		//Free Return - Best Adaptive Path
		if (fcn == 20 || fcn == 21)
		{
			PZMCCPLN.Mode = 3;
			TranslunarMidcourseCorrectionProcessor(sv_ephem, sv.mass, 0.0);

			tig = GETfromGMT(PZMCCXFR.sv_man_bef[0].GMT);
			dv = PZMCCXFR.V_man_after[0] - PZMCCXFR.sv_man_bef[0].V;

			engine = SPSRCSDecision(SPS_THRUST / sv.mass, dv);
			PoweredFlightProcessor(sv, tig, engine, 0.0, dv, false, P30TIG, dV_LVLH);
		}
		else //Nodal Targeting
		{
			PZMCCPLN.Mode = 1;
			TranslunarMidcourseCorrectionProcessor(sv_ephem, sv.mass, 0.0);

			tig = GETfromGMT(PZMCCXFR.sv_man_bef[0].GMT);
			dv = PZMCCXFR.V_man_after[0] - PZMCCXFR.sv_man_bef[0].V;

			engine = SPSRCSDecision(SPS_THRUST / sv.mass, dv);
			PoweredFlightProcessor(sv, tig, engine, 0.0, dv, false, P30TIG, dV_LVLH, sv_ig1, sv_cut1);
		}

		if (length(dV_LVLH) < dv_thres)
		{
			scrubbed = true;
		}

		//Message for scrubbed maneuvers
		if (scrubbed)
		{
			sprintf(upMessage, "%s has been scrubbed.", manname);
		}

		char buffer1[1000];

		//MCC-1 through 3
		if (fcn != 23)
		{
			//Store data from MCC calculation. Always for MCC-1, for MCC-2 only if the maneuver will be done
			if (fcn == 20 || (fcn == 21 && scrubbed == false))
			{
				//Store new LOI time
				calcParams.LOI = PZMCCDIS.data[0].GET_LOI;
				//Transfer MCC plan to skeleton flight plan table
				GMGMED("F30,1;");
			}

			//LM state vector
			AGCStateVectorUpdate(buffer1, sv, false);

			if (scrubbed)
			{
				sprintf(uplinkdata, "%s", buffer1);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "LM state vector");
				}
			}
			else
			{
				manopt.dV_LVLH = dV_LVLH;
				manopt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), dV_LVLH);
				manopt.HeadsUp = false;
				manopt.REFSMMAT = REFSMMAT;
				manopt.TIG = P30TIG;
				manopt.vessel = calcParams.src;
				manopt.vesseltype = 0;

				AP11ManeuverPAD(&manopt, *form);
				sprintf(form->purpose, manname);

				char buffer2[1000];
				CMCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

				sprintf(uplinkdata, "%s%s", buffer1, buffer2);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "LM state vector, target load");
				}
			}
		}
		//MCC-4
		else
		{

			REFSMMATOpt refsopt;
			SV sv_node, sv_ig2, sv_cut2;
			double P30TIG_LOI2;
			char buffer2[1000];

			//Step 1: Calculate LOI-1 with MCC-4 burnout vector

			//If maneuver scrubbed, use state vector without maneuver
			if (scrubbed)
			{
				sv_cut1 = sv;
			}

			PZLOIPLN.dh_bias = 0.0;
			PZLOIPLN.DW = -15.0;
			PZLOIPLN.eta_1 = 0.0;
			PZLOIPLN.HA_LLS = 60.0;
			PZLOIPLN.HP_LLS = 60.0;
			PZLOIPLN.PlaneSolnForInterSoln = true;
			PZLOIPLN.REVS1 = 2.0;
			PZLOIPLN.REVS2 = 4;

			med_k18.HALOI1 = 170.0;
			med_k18.HPLOI1 = 60.0;
			med_k18.DVMAXp = 10000.0;
			med_k18.DVMAXm = 10000.0;
			med_k18.psi_DS = 282.0;
			med_k18.psi_MN = med_k18.psi_DS - 1.0;
			med_k18.psi_MX = med_k18.psi_DS + 1.0;

			EphemerisData sv_cut1b;

			sv_cut1b.R = sv_cut1.R;
			sv_cut1b.V = sv_cut1.V;
			sv_cut1b.GMT = OrbMech::GETfromMJD(sv_cut1.MJD, SystemParameters.GMTBASE);
			sv_cut1b.RBI = BODY_MOON;

			PMMLRBTI(sv_cut1b);

			sv_cut2.R = PZLRBELM.sv_man_bef[6].R;
			sv_cut2.V = PZLRBELM.V_man_after[6];
			sv_cut2.MJD = OrbMech::MJDfromGET(PZLRBELM.sv_man_bef[6].GMT, SystemParameters.GMTBASE);
			sv_cut2.gravref = hMoon;

			//Step 2: Calculate LOI-2 to get the TIG
			med_k16.Mode = 2;
			med_k16.Sequence = 3;
			med_k16.GETTH1 = GETfromGMT(PZLRBELM.sv_man_bef[6].GMT) + 3.5*3600.0;
			med_k16.GETTH2 = med_k16.GETTH3 = med_k16.GETTH4 = med_k16.GETTH1;
			med_k16.DesiredHeight = 60.0*1852.0;

			LunarDescentPlanningProcessor(sv_cut2);
			P30TIG_LOI2 = PZLDPDIS.GETIG[0];

			//Step 3: Calculate LVLH REFSMMAT at LOI-2 TIG taking into account the trajectory leading up to that point
			refsopt.REFSMMATopt = 2;
			refsopt.REFSMMATTime = P30TIG_LOI2;
			refsopt.vessel = calcParams.src;
			refsopt.useSV = true;
			refsopt.RV_MCC = coast(sv_cut2, refsopt.REFSMMATTime - OrbMech::GETfromMJD(sv_cut2.MJD, CalcGETBase()));

			REFSMMAT = REFSMMATCalc(&refsopt);

			//Step 4: Store SV for use with PC+2
			calcParams.SVSTORE1 = sv_cut1;

			//CSM state vector with V66
			AGCStateVectorUpdate(buffer1, sv, true, true);

			if (scrubbed)
			{
				AGCDesiredREFSMMATUpdate(buffer2, REFSMMAT);

				sprintf(uplinkdata, "%s%s", buffer1, buffer2);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM+LM state vectors, LOI-2 REFSMMAT");
				}
			}
			else
			{
				manopt.dV_LVLH = dV_LVLH;
				manopt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), dV_LVLH);
				manopt.HeadsUp = false;
				manopt.REFSMMAT = REFSMMAT;
				manopt.TIG = P30TIG;
				manopt.vessel = calcParams.src;
				manopt.vesseltype = 0;

				AP11ManeuverPAD(&manopt, *form);
				sprintf(form->purpose, manname);

				char buffer3[1000];

				CMCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);
				AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT);

				sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM+LM state vectors, target load, LOI-2 REFSMMAT");
				}
			}
		}
	}
	break;
	case 30:	// MISSION CP PRELIMINARY LOI-1 MANEUVER
	case 31:	// MISSION CP LOI-1 MANEUVER
	{
		AP11ManPADOpt manopt;
		double P30TIG, tig;
		VECTOR3 dV_LVLH, dv;
		EphemerisData sv_ephem;
		SV sv;

		AP11MNV * form = (AP11MNV *)pad;

		sv_ephem = StateVectorCalcEphem(calcParams.src);

		PZLOIPLN.dh_bias = 0.0;
		PZLOIPLN.DW = -15.0;
		PZLOIPLN.eta_1 = 0.0;
		PZLOIPLN.HA_LLS = 60.0;
		PZLOIPLN.HP_LLS = 60.0;
		PZLOIPLN.PlaneSolnForInterSoln = true;
		PZLOIPLN.REVS1 = 2.0;
		PZLOIPLN.REVS2 = 4;

		med_k18.HALOI1 = 170.0;
		med_k18.HPLOI1 = 60.0;
		med_k18.DVMAXp = 10000.0;
		med_k18.DVMAXm = 10000.0;
		med_k18.psi_DS = 282.0;
		med_k18.psi_MN = med_k18.psi_DS - 1.0;
		med_k18.psi_MX = med_k18.psi_DS + 1.0;

		PMMLRBTI(sv_ephem);

		sv = StateVectorCalc(calcParams.src);
		tig = GETfromGMT(PZLRBELM.sv_man_bef[6].GMT);
		dv = PZLRBELM.V_man_after[6] - PZLRBELM.sv_man_bef[6].V;

		PoweredFlightProcessor(sv, tig, RTCC_ENGINETYPE_CSMSPS, 0.0, dv, false, P30TIG, dV_LVLH);

		manopt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
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

			AGCStateVectorUpdate(buffer1, sv, true);
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
	case 40:	// MISSION CP FLYBY MANEUVER
	{
		RTEMoonOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		SV sv;

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalc(calcParams.src);

		entopt.EntryLng = -165.0*RAD;
		entopt.returnspeed = 0;
		entopt.RV_MCC = sv;
		entopt.TIGguess = calcParams.LOI - 8.0*3600.0;
		entopt.vessel = calcParams.src;
		entopt.SMODE = 14;

		RTEMoonTargeting(&entopt, &res);

		opt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
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
		char manname[32];

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalc(calcParams.src);

		if (fcn == 41)
		{
			//AOL
			entopt.entrylongmanual = true;
			entopt.EntryLng = -25.0*RAD;
			entopt.returnspeed = 1;
			entopt.enginetype = RTCC_ENGINETYPE_CSMRCSPLUS4;
			sprintf(manname, "PC+2");
		}
		else
		{
			//IOL
			entopt.entrylongmanual = false;
			entopt.ATPLine = 3;
			entopt.returnspeed = 2;
			sprintf(manname, "PC+2 fast return");
			PZREAP.VRMAX = 37500.0;
		}

		entopt.RV_MCC = calcParams.SVSTORE1;
		entopt.TIGguess = calcParams.LOI + 2.0*3600.0;
		entopt.vessel = calcParams.src;
		entopt.SMODE = 14;

		RTEMoonTargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, EntryAng);

		//Reset to default
		PZREAP.VRMAX = 36323.0;

		opt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		opt.dV_LVLH = res.dV_LVLH;
		if (fcn == 41)
		{
			opt.enginetype = RTCC_ENGINETYPE_CSMRCSPLUS4;
		}
		else
		{
			opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		}
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);

		if (!REFSMMATDecision(form->Att*RAD))
		{
			REFSMMATOpt refsopt;
			MATRIX3 REFSMMAT;

			refsopt.dV_LVLH = res.dV_LVLH;
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
		SV sv0, sv1;
		char manname[8];

		AP11MNV * form = (AP11MNV *)pad;

		sv0 = StateVectorCalc(calcParams.src); //State vector for uplink

		sv1 = ExecuteManeuver(sv0, TimeofIgnition, DeltaV_LVLH, 0, RTCC_ENGINETYPE_CSMSPS);

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
		entopt.RV_MCC = sv1;
		entopt.vessel = calcParams.src;

		RTEMoonTargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, EntryAng);

		opt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
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
		SV2 sv0, sv1, sv2, sv3;
		AP10MAPUPDATE upd_hyper, upd_ellip, upd_ellip2;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc2(calcParams.src);
		LunarOrbitMapUpdate(sv0.sv, upd_hyper);

		sv1 = ExecuteManeuver(sv0, TimeofIgnition, DeltaV_LVLH, RTCC_ENGINETYPE_CSMSPS);
		sv2.sv = coast(sv1.sv, -30.0*60.0);
		LunarOrbitMapUpdate(sv2.sv, upd_ellip);
		sv3.sv = coast(sv2.sv, 2.0*3600.0);
		LunarOrbitMapUpdate(sv3.sv, upd_ellip2);

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
		SV2 sv0, sv1;
		AP10MAPUPDATE upd_ellip, upd_ellip2;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc2(calcParams.src);
		LunarOrbitMapUpdate(sv0.sv, upd_ellip);

		sv1 = ExecuteManeuver(sv0, TimeofIgnition, DeltaV_LVLH, RTCC_ENGINETYPE_CSMSPS);
		LunarOrbitMapUpdate(sv1.sv, upd_ellip2);

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
		EphemerisData sv0, sv1;
		AP10MAPUPDATE upd_ellip;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalcEphem(calcParams.src);

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

		LunarOrbitMapUpdate(sv1, upd_ellip);

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
		SV2 sv0, sv1;
		AP10MAPUPDATE upd_ellip, upd_hyper;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc2(calcParams.src);
		LunarOrbitMapUpdate(sv0.sv, upd_ellip);

		sv1 = ExecuteManeuver(sv0, TimeofIgnition, DeltaV_LVLH, RTCC_ENGINETYPE_CSMSPS);
		LunarOrbitMapUpdate(sv1.sv, upd_hyper);

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
		AP11ManPADOpt manopt;
		double P30TIG;
		VECTOR3 dV_LVLH;
		SV sv;
		char buffer1[1000];
		char buffer2[1000];

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		med_k16.Mode = 2;
		med_k16.Sequence = 3;
		med_k16.GETTH1 = calcParams.LOI + 3.5*3600.0;
		med_k16.GETTH2 = med_k16.GETTH3 = med_k16.GETTH4 = med_k16.GETTH1;
		med_k16.DesiredHeight = 60.0*1852.0;

		LunarDescentPlanningProcessor(sv);

		PoweredFlightProcessor(sv, PZLDPDIS.GETIG[0], RTCC_ENGINETYPE_CSMSPS, 0.0, PZLDPDIS.DVVector[0] * 0.3048, true, P30TIG, dV_LVLH);

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

		manopt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.vesseltype = 0;

		AP11ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "LOI-2");

		AGCStateVectorUpdate(buffer1, sv, true);
		CMCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

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
		char buffer1[1000];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		AGCStateVectorUpdate(buffer1, sv, false);

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

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.TIGguess = 0.0;
		entopt.EntryLng = -165.0*RAD;
		entopt.vessel = calcParams.src;
		entopt.returnspeed = 1;
		entopt.RV_MCC = sv;

		RTEMoonTargeting(&entopt, &res);

		opt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
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

			AGCStateVectorUpdate(buffer1, sv, false);

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

			AGCStateVectorUpdate(buffer1, sv, true);
			AGCStateVectorUpdate(buffer2, sv, false);
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

			//Calculate nominal entry REFSMMAT
			REFSMMATOpt refsopt;

			refsopt.REFSMMATopt = 3;
			refsopt.vessel = calcParams.src;
			refsopt.useSV = true;
			refsopt.RV_MCC = res.sv_postburn;

			//Save as LCV REFSMMAT
			int id = RTCC_REFSMMAT_TYPE_LCV - 1;
			EZJGMTX1.data[id].ID = 1;
			EZJGMTX1.data[id].GMT = GMTfromGET(res.GET400K);
			EZJGMTX1.data[id].REFSMMAT = REFSMMATCalc(&refsopt);
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
		char buffer1[1000];

		AGCDesiredREFSMMATUpdate(buffer1, EZJGMTX1.data[RTCC_REFSMMAT_TYPE_LCV - 1].REFSMMAT);

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
		double MCCtime;
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

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.entrylongmanual = true;
		entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		entopt.lng = -165.0*RAD;
		entopt.RV_MCC = sv;
		entopt.TIGguess = MCCtime;
		entopt.vessel = calcParams.src;
		entopt.r_rbias = PZREAP.RRBIAS;
		entopt.type = 3;

		//Calculate corridor control burn
		EntryTargeting(&entopt, &res);

		//If time to EI is more than 24 hours and the splashdown longitude is not within 2° of desired, then perform a longitude control burn
		if (MCCtime < calcParams.EI - 24.0*3600.0 && abs(res.longitude - entopt.lng) > 2.0*RAD)
		{
			entopt.type = 1;
			entopt.t_Z = res.GET400K;

			EntryTargeting(&entopt, &res);
		}

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
			EntryUpdateCalc(sv, PZREAP.RRBIAS, true, &res);

			res.dV_LVLH = _V(0, 0, 0);
			res.P30TIG = entopt.TIGguess;

			char buffer1[1000];

			//Every update except final MCC-7 gets a LM state vector
			if (fcn != 206)
			{
				AGCStateVectorUpdate(buffer1, sv, false);
				sprintf(upDesc, "LM state vector");
			}
			else
			{
				AGCStateVectorUpdate(buffer1, sv, true, true);
				sprintf(upDesc, "CSM+LM state vector");
			}

			sprintf(uplinkdata, "%s", buffer1);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
			}
		}
		else
		{
			if (fcn == 203 || fcn == 206)
			{
				REFSMMATOpt refsopt;
				refsopt.REFSMMATopt = 3;
				refsopt.vessel = calcParams.src;
				refsopt.useSV = true;
				refsopt.RV_MCC = res.sv_postburn;

				REFSMMAT = REFSMMATCalc(&refsopt);
			}
			else
			{
				REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
			}

			opt.dV_LVLH = res.dV_LVLH;
			opt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), res.dV_LVLH);
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

				AGCStateVectorUpdate(buffer1, sv, false);
				CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);
				AGCREFSMMATUpdate(buffer3, REFSMMAT, true);

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

				AGCStateVectorUpdate(buffer1, sv, false);
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

				AGCStateVectorUpdate(buffer1, sv, false);

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

				AGCStateVectorUpdate(buffer1, sv, true, true);
				CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);
				AGCREFSMMATUpdate(buffer3, REFSMMAT, true);

				sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM+LM state vector, target load, Entry REFSMMAT");
				}
			}
			else if (fcn == 300)//generic MCC
			{
				char buffer1[1000];
				char buffer2[1000];
				char buffer3[1000];

				AGCStateVectorUpdate(buffer1, sv, true);
				CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);
				AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT);

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
		entopt.lat = SplashLatitude;
		entopt.lng = SplashLongitude;
		entopt.P30TIG = TimeofIgnition;
		entopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		entopt.sv0 = sv;

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
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.lat = SplashLatitude;
		entopt.lng = SplashLongitude;
		entopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		entopt.sv0 = sv;

		LunarEntryPAD(&entopt, *form);
		sprintf(form->Area[0], "MIDPAC");

		AGCStateVectorUpdate(buffer1, sv, true);
		AGCStateVectorUpdate(buffer2, sv, false);
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