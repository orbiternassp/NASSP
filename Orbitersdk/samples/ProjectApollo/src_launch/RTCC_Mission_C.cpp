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
#include "saturn1b.h"
#include "sivb.h"
#include "iu.h"
#include "LVDC.h"
#include "../src_rtccmfd/OrbMech.h"
#include "mcc.h"
#include "rtcc.h"

bool RTCC::CalculationMTP_C(int fcn, LPVOID &pad, char *upString, char *upDesc, char *upMessage)
{
	char uplinkdata[1024 * 3];
	bool preliminary = true;
	bool scrubbed = false;

	switch (fcn) {
	case 100: //MISSION INITIALIZATION
	{
		char Buff[128];

		//P80 MED: mission initialization
		mcc->mcc_calcs.PrelaunchMissionInitialization();

		//P10 MED: Enter actual liftoff time
		double tephem_cs;
		Saturn *cm = (Saturn *)calcParams.src;

		//Get TEPHEM from CMC
		tephem_cs = GetTEPHEMFromAGC(&cm->agc.vagc, true);
		double LaunchMJD = (tephem_cs / 8640000.) + SystemParameters.TEPHEM0;
		double LaunchGMT = (LaunchMJD - SystemParameters.GMTBASE)*24.0*3600.0;

		int hh, mm;
		double ss;

		OrbMech::SStoHHMMSS(LaunchGMT, hh, mm, ss, 0.01);

		sprintf_s(Buff, "P10,CSM,%d:%d:%.2lf;", hh, mm, ss);
		GMGMED(Buff);

		//P12: CSM GRR and Azimuth
		Saturn1b *Sat1b = (Saturn1b *)cm;
		LVDC1B *lvdc = (LVDC1B *)Sat1b->iu->GetLVDC();
		double Azi = lvdc->Azimuth*DEG;
		double T_GRR = lvdc->T_GRR;

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

		//Initialize config, areas and weights
		med_m55.Table = RTCC_MPT_CSM;
		med_m55.ConfigCode = "C";
		PMMWTC(55);

		med_m55.Table = RTCC_MPT_LM;
		med_m55.ConfigCode = "S";
		PMMWTC(55);

		med_m51.Table = RTCC_MPT_CSM;
		med_m51.CSMArea = 129.4*0.3048*0.3048;
		med_m51.LMAscentArea = med_m51.LMDescentArea = med_m51.SIVBArea = 0.0;
		med_m51.KFactor = 1.0;
		PMMWTC(51);

		med_m51.Table = RTCC_MPT_LM;
		med_m51.SIVBArea = 365.0*0.3048*0.3048;
		med_m51.LMAscentArea = med_m51.LMDescentArea = med_m51.CSMArea = 0.0;
		med_m51.KFactor = 1.0;
		PMMWTC(51);

		med_m50.Table = RTCC_MPT_CSM;
		med_m50.CSMWT = 36300.0* 0.453592;
		PMMWTC(50);

		med_m50.Table = RTCC_MPT_LM;
		med_m50.SIVBWT = 30029.0* 0.453592;
		PMMWTC(50);

		//And propellants
		med_m49.Table = RTCC_MPT_CSM;
		med_m49.SPSFuelRemaining = 4430;
		PMMWTC(49);

		med_m49.Table = RTCC_MPT_LM;
		med_m49.SPSFuelRemaining = -1;
		med_m49.SIVBFuelRemaining = 0;
		PMMWTC(49);
	}
	break;
	case 1: //MISSION C PHASING BURN
	{
		AP7ManPADOpt opt;
		EphemerisData sv_A;
		PLAWDTOutput WeightsTable;
		double GET_TIG;
		double GRRTIME;

		char RETRO[32];

		AP7MNV *form = (AP7MNV *)pad;

		GRRTIME = GETfromGMT(GetIUClockZero());

		OrbMech::format_time_XXHMMSS(RETRO, GRRTIME + 11820.0);

		sv_A = StateVectorCalcEphem(calcParams.src);
		WeightsTable = GetWeightsTable(calcParams.src, true, false);

		GET_TIG = OrbMech::HHMMSSToSS(3, 20, 0);

		opt.TIG = GET_TIG;
		opt.dV_LVLH = _V(-1.8, 0.0, 0.0)*0.3048; //TBD: Change to -5.7 when full drag is simulated
		opt.enginetype = RTCC_ENGINETYPE_CSMRCSMINUS4;
		opt.HeadsUp = false;
		opt.sxtstardtime = 0;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		opt.navcheckGET = 0;
		opt.sv0 = sv_A;
		opt.WeightsTable = WeightsTable;

		AP7ManeuverPAD(opt, *form);
		sprintf(form->purpose, "PHASING");
		sprintf(form->remarks, "Heads down, Retrograde, -X thrusters  Be in retro attitude by 3:16:30  GET of S-IVB Retro Maneuver: %s", RETRO);
	}
	break;
	case 101: //S-IVB STATE VECTOR UPLINK
	{
		void *uplink = NULL;
		DCSSLVNAVUPDATE upl;
		VehicleDataBlock sv1, sv2;

		SIVB *iuv = (SIVB *)calcParams.tgt;
		IU *iu = iuv->GetIU();

		//Get S-IVB state vector, using the area previously stored in the LM MPT
		sv1 = StateVectorCalcDataBlock(calcParams.tgt, PZMPTLEM.ConfigurationArea, PZMPTLEM.KFactor);

		sv2 = coast(sv1, GMTfromGET(17460.0) - sv1.sv.GMT); //4:51:00 GET
		CMMSLVNAV(1, sv2.sv.R, sv2.sv.V, sv2.sv.GMT);

		upl.PosS = CZNAVSLV.PosS;
		upl.DotS = CZNAVSLV.DotS;
		upl.NUPTIM = CZNAVSLV.NUPTIM;

		uplink = &upl;
		bool uplinkaccepted = iu->DCSUplink(DCSUPLINK_SLV_NAVIGATION_UPDATE, uplink);

		sprintf(upMessage, "S-IVB Navigation Update");
	}
	break;
	case 2:  //MISSION C CONTINGENCY DEORBIT (6-4) TARGETING
	case 42: //MISSION C NOMINAL DEORBIT MANEUVER PAD
	{
		AP7MNV *form = (AP7MNV *)pad;

		MATRIX3 REFSMMAT;
		double get_guess, lng_des, gmt_guess, gmt_min, gmt_max, NavGET;
		AP7ManPADOpt opt;
		VehicleDataBlock sv, sv_upl;
		PLAWDTOutput WeightsTable;
		EMSMISSInputTable intab;
		EphemerisDataTable2 tab;
		VECTOR3 GDCangles;
		char SetStars[64];
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];
		char buffer4[1000];
		char alignstars[256];
		char starsettime[256];

		//Get state vector and mass
		sv = StateVectorCalcDataBlock(calcParams.src);
		WeightsTable = GetWeightsTable(calcParams.src, true, false);

		if (fcn == 2)
		{
			get_guess = OrbMech::HHMMSSToSS(8, 55, 0);
			lng_des = -163.0*RAD;
		}
		else
		{
			get_guess = OrbMech::HHMMSSToSS(259, 40, 0);
			lng_des = -64.17*RAD;
		}

		//Generate epehemeris for recovery target selection
		gmt_guess = GMTfromGET(get_guess);
		gmt_min = gmt_guess;
		gmt_max = gmt_guess + 2.75*60.0*60.0;

		intab.AnchorVector = sv.sv;
		intab.EphemerisBuildIndicator = true;
		intab.ECIEphemerisIndicator = true;
		intab.ECIEphemTableIndicator = &tab;
		intab.EphemerisLeftLimitGMT = gmt_min;
		intab.EphemerisRightLimitGMT = gmt_max;
		intab.ManCutoffIndicator = false;
		intab.VehicleCode = RTCC_MPT_CSM;
		intab.WeightsTable = &WeightsTable;
		intab.useInputWeights = true;

		EMSMISS(&intab);
		tab.Header.TUP = 1;

		//Run recovery target selection
		RMDRTSD(tab, 1, gmt_guess, lng_des);

		//Select first entry
		RZJCTTC.R32_lat_T = RZDRTSD.table[0].Latitude*RAD;
		RZJCTTC.R32_lng_T = RZDRTSD.table[0].Longitude*RAD;
		RZJCTTC.R32_GETI = RZDRTSD.table[0].GET - 20.0*60.0;

		//MEDs
		RZJCTTC.R32_Code = 1;
		RZJCTTC.Type = 1;

		RZJCTTC.R31_Thruster = RTCC_ENGINETYPE_CSMSPS;
		RZJCTTC.R31_GuidanceMode = 4;
		RZJCTTC.R31_BurnMode = 3;
		RZJCTTC.R31_dt = 0.0;
		RZJCTTC.R31_dv = 0.0;
		RZJCTTC.R31_AttitudeMode = 1;
		RZJCTTC.R31_LVLHAttitude = _V(0.0, -48.5*RAD, PI);
		RZJCTTC.R31_UllageTime = 15.0;
		RZJCTTC.R31_Use4UllageThrusters = true;
		RZJCTTC.R31_REFSMMAT = 9;
		RZJCTTC.R31_GimbalIndicator = -1;
		RZJCTTC.R31_InitialBankAngle = 0.0;
		RZJCTTC.R31_GLevel = 0.2;
		RZJCTTC.R31_FinalBankAngle = 55.0*RAD;

		RMSDBMP(sv.sv, WeightsTable.ConfigWeight);

		//Save data
		TimeofIgnition = RZRFDP.data[2].GETI;
		SplashLatitude = RZRFDP.data[2].lat_T*RAD;
		SplashLongitude = RZRFDP.data[2].lng_T*RAD;
		DeltaV_LVLH = RZRFTT.Manual.DeltaV;

		REFSMMAT = RZRFDP.data[2].REFSMMAT; //REFSMMAT for uplink

		//Save REFSMMAT in DOD slot
		GMGMED("G11,CSM,DOM;");
		//Move REFSMMAT to current
		GMGMED("G00,CSM,DOD,CSM,CUR;");

		int hh, mm;
		double ss;
		if (fcn == 2)
		{
			//Nav Check Calculation (T -42m)
			OrbMech::SStoHHMMSS(abs(TimeofIgnition - 42.0*60.0), hh, mm, ss, 60.0);
			NavGET = OrbMech::HHMMSSToSS(hh, mm, 0);
		}
		else
		{
			//Nav Check Calculation (T -40m)
			OrbMech::SStoHHMMSS(abs(TimeofIgnition - 40.0*60.0), hh, mm, ss, 60.0);
			NavGET = OrbMech::HHMMSSToSS(hh, mm, 0);
		}

		opt.TIG = TimeofIgnition;
		opt.dV_LVLH = DeltaV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = true;
		opt.REFSMMAT = REFSMMAT;
		opt.navcheckGET = NavGET;
		opt.sxtstardtime = -25.0*60.0;
		opt.UllageDT = 15.0;
		opt.UllageThrusterOpt = true;
		opt.sv0 = sv.sv;
		opt.WeightsTable = WeightsTable;

		mcc->mcc_calcs.BackupGDCAlignment(sv_upl, opt.sxtstardtime, REFSMMAT, 1, GDCangles, SetStars);

		sprintf(alignstars, "SET STARS: %s  RALIGN %03.0f  PALIGN %03.0f  YALIGN %03.0f", SetStars, GDCangles.x, GDCangles.y, GDCangles.z);

		AP7ManeuverPAD(opt, *form);
		OrbMech::format_time_HHHMM(starsettime, TimeofIgnition - 19.0*60.0); //Need to find actual star set time, -19m used as guess from actual pads

		if (fcn == 2)
		{
			sprintf(form->purpose, "6-4 DEORBIT");
			sv_upl = coast(sv, GMTfromGET(TimeofIgnition - 12.0*60.0) - sv.sv.GMT);
			sprintf(form->remarks, "Ullage: 4 jet, 15 seconds  Star check not available after %s", starsettime);
		}
		else
		{
			sprintf(form->purpose, "164-1A RETROFIRE");
			sv_upl = coast(sv, GMTfromGET(TimeofIgnition - 12.0*60.0) - sv.sv.GMT);
			sprintf(form->remarks, "Ullage: 4 jet, 15 seconds  %s  Star check not available after %s", alignstars, starsettime);
		}

		AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv_upl.sv);
		AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv_upl.sv);
		CMCRetrofireExternalDeltaVUpdate(buffer3, SplashLatitude, SplashLongitude, TimeofIgnition, DeltaV_LVLH);
		AGCDesiredREFSMMATUpdate(buffer4, REFSMMAT);

		sprintf(uplinkdata, "%s%s%s%s", buffer1, buffer2, buffer3, buffer4);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, Target load, Entry REFSMMAT");
		}
	}
	break;
	case 102: //MANUAL RETRO ATTITUDE ORIENTATION TEST
	{
		AP7RETRORIENTPAD *form = (AP7RETRORIENTPAD *)pad;

		RTACFGOSTInput in;
		RTACFGOSTOutput out;

		in.get = OrbMech::HHMMSSToSS(6, 10, 0);
		in.LVLHRoll = 0.0;
		in.LVLHYaw = PI;
		in.option = 4;
		in.REFSMMAT = EZJGMTX1.data[0].REFSMMAT;
		in.sv = StateVectorCalcEphem(calcParams.src);
		in.Weight = calcParams.src->GetMass();

		RTACFGuidanceOpticsSupportTable(in, out);

		form->GET_Day = in.get;
		form->RetroAtt_Day = out.IMUAtt*DEG;
		form->RetroAtt_Day.x = round(form->RetroAtt_Day.x);
		form->RetroAtt_Day.y = round(form->RetroAtt_Day.y);
		form->RetroAtt_Day.z = round(form->RetroAtt_Day.z);

		in.get = OrbMech::HHMMSSToSS(6, 50, 0);
		RTACFGuidanceOpticsSupportTable(in, out);

		form->GET_Night = in.get;
		form->RetroAtt_Night = out.IMUAtt*DEG;
		form->RetroAtt_Night.x = round(form->RetroAtt_Night.x);
		form->RetroAtt_Night.y = round(form->RetroAtt_Night.y);
		form->RetroAtt_Night.z = round(form->RetroAtt_Night.z);
	}
	break;
	case 3: //MISSION C BLOCK DATA UPDATE 2
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 2;
	}
	break;
	case 4: //MISSION C 2ND PHASING MANEUVER
	{
		VehicleDataBlock sv_A, sv_P, sv_A1, sv_A1_apo, sv_A2, sv_P2;
		double GET_TIG, GMT1, GMT2;
		MATRIX3 Q_Xx;
		VECTOR3 R_c_u, R_t_u, H_t_u, dV_LVLH;
		double r_t, theta1, DR, DR_act, dv, erro, dvo, err;
		double c_I, p_I, tol;
		int s_I;

		AP7MNV *form = (AP7MNV *)pad;

		//Update S-IVB area on MPT to 1400 ft^2 (tumbling S-IVB area)
		med_m51.Table = RTCC_MPT_LM;
		med_m51.SIVBArea = 1400.0*pow(0.3048, 2);
		med_m51.LMAscentArea = med_m51.LMDescentArea = med_m51.CSMArea = 0.0;
		med_m51.KFactor = 1.0;
		PMMWTC(51);

		//Get CSM state vector
		sv_A = StateVectorCalcDataBlock(calcParams.src, PZMPTCSM.ConfigurationArea, PZMPTCSM.KFactor);
		//Get S-IVB state vector
		sv_P = StateVectorCalcDataBlock(calcParams.tgt, PZMPTLEM.ConfigurationArea, PZMPTLEM.KFactor);

		//Iterator variable
		c_I = 0.0;
		s_I = 0;

		//TIG GET
		GET_TIG = OrbMech::HHMMSSToSS(15, 52, 0);
		//TIG GMT
		GMT1 = GMTfromGET(GET_TIG);
		//Time of NCC1 maneuver
		GMT2 = GMTfromGET(OrbMech::HHMMSSToSS(26, 25, 0));
		//Desired leading distance at NCC1
		DR = 76.5 * 1852;
		//Initial guess for DV
		dv = 0.0;
		//Tolerance
		tol = 10.0;

		//Coast to TIG
		sv_A1 = coast(sv_A, GMT1 - sv_A.sv.GMT);
		sv_A1_apo = sv_A1;
		Q_Xx = OrbMech::LVLH_Matrix(sv_A1.sv.R, sv_A1.sv.V);

		//Coast to NCC1
		sv_P2 = coast(sv_P, GMT2 - sv_P.sv.GMT);
		r_t = length(sv_P2.sv.R);
		R_t_u = unit(sv_P2.sv.R);
		H_t_u = unit(crossp(sv_P2.sv.R, sv_P2.sv.V));

		do
		{
			//Simulate maneuver
			dV_LVLH = _V(dv, 0, 0);
			sv_A1_apo.sv.V = sv_A1.sv.V + tmul(Q_Xx, dV_LVLH);

			//Coast to NCC1
			sv_A2 = coast(sv_A1_apo, GMT2 - sv_A1.sv.GMT);

			//Calculate actual downrange distance
			R_c_u = unit(sv_A2.sv.R);
			theta1 = atan2(dotp(H_t_u, crossp(R_t_u, R_c_u)), dotp(R_c_u, R_t_u));
			DR_act = r_t * theta1;

			//Calculate err
			err = DR_act - DR;

			//Iterate
			if (abs(err) > tol)
			{
				OrbMech::ITER(c_I, s_I, err, p_I, dv, erro, dvo);
			}
		} while (abs(err) > tol);

		if (length(dV_LVLH) < 1.0*0.3048)
		{
			scrubbed = true;
		}

		if (scrubbed)
		{
			sprintf(upMessage, "Second Phasing Maneuver Scrubbed");
		}
		else
		{
			AP7ManPADOpt opt;

			opt.TIG = GET_TIG;
			opt.dV_LVLH = dV_LVLH;
			opt.enginetype = mcc->mcc_calcs.SPSRCSDecision(SystemParameters.MCTST1 / calcParams.src->GetMass(), dV_LVLH);
			opt.HeadsUp = true;
			opt.sxtstardtime = 0;
			opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
			opt.navcheckGET = 0;
			opt.UllageDT = 15.0;
			opt.UllageThrusterOpt = true;
			opt.sv0 = sv_A.sv;
			opt.WeightsTable = GetWeightsTable(calcParams.src, true, false);

			AP7ManeuverPAD(opt, *form);
			sprintf(form->purpose, "PHASING 2");

			if (opt.enginetype == RTCC_ENGINETYPE_CSMRCSPLUS4)
			{
				sprintf(form->remarks, "Heads up, +X thrusters");
			}
			else
			{
				sprintf(form->remarks, "Heads up,  Ullage: 4 jet, 15 seconds");
			}
		}
	}
	break;
	case 5: //MISSION C BLOCK DATA UPDATE 3
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 3;
	}
	break;
	case 7://MISSION C FINAL NCC1 MANEUVER
	{
		preliminary = false;
	}
	case 6: //MISSION C PRELIMINARY NCC1 MANEUVER
	{
		TwoImpulseOpt lambert;
		TwoImpulseResuls res;
		AP7ManPADOpt opt;
		PMMMPTInput in;
		VECTOR3 dV_LVLH;
		double GET_TIG_imp, GMT_TIG_imp, P30TIG, NavGET;
		VehicleDataBlock sv_A, sv_P, sv_A1, sv_P1;
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];
		char T_ALIGN_HMS[24];

		AP7MNV *form = (AP7MNV *)pad;

		//TIG in GET and GMT
		GET_TIG_imp = OrbMech::HHMMSSToSS(26, 25, 0);
		GMT_TIG_imp = GMTfromGET(GET_TIG_imp);

		//Get state vectors
		sv_A = StateVectorCalcDataBlock(calcParams.src, PZMPTCSM.ConfigurationArea, PZMPTCSM.KFactor);
		sv_P = StateVectorCalcDataBlock(calcParams.tgt, PZMPTLEM.ConfigurationArea, PZMPTLEM.KFactor);

		//Propagate to time tags
		sv_A1 = coast(sv_A, GMT_TIG_imp - 12.0*60.0 - sv_A.sv.GMT);
		sv_P1 = coast(sv_P, GMT_TIG_imp + 12.0*60.0 - sv_P.sv.GMT);

		lambert.mode = 5;
		lambert.T1 = GMT_TIG_imp;
		lambert.T2 = GMTfromGET(OrbMech::HHMMSSToSS(28, 1, 0));
		lambert.ChaserVehicle = RTCC_MPT_CSM;
		lambert.sv_C = sv_A;
		lambert.sv_T = sv_P;
		lambert.DH = 8.0*1852.0;
		lambert.PhaseAngle = 1.32*RAD;

		PMSTICN(lambert, res);

		in.CONFIG = 1; //CSM
		in.CSMWeight = sv_A.Weight;
		in.sv_before = res.sv_tig.sv;
		in.V_aft = res.sv_tig.sv.V + res.dV;
		in.DETU = 15.0; //Ullage
		in.UT = true; //4 jets
		in.IgnitionTimeOption = false;
		in.IterationFlag = true;
		in.LMWeight = 0.0;
		in.Thruster = RTCC_ENGINETYPE_CSMSPS;
		in.VC = RTCC_MANVEHICLE_CSM;
		in.VehicleArea = PZMPTCSM.ConfigurationArea;

		double GMT_TIG;
		double T_Align;
		PoweredFlightProcessor(in, GMT_TIG, dV_LVLH);
		P30TIG = GETfromGMT(GMT_TIG);
		T_Align = OrbMech::HHMMSSToSS(23, 24, 8); //T-Align 23:24:08 from transcript

		opt.TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = true;
		opt.sxtstardtime = -30.0*60.0;
		opt.UllageDT = 15.0;
		opt.UllageThrusterOpt = true;
		opt.sv0 = sv_A.sv;
		opt.WeightsTable = GetWeightsTable(calcParams.src, true, false);

		int hh, mm;
		double ss;
		if (preliminary)
		{
			REFSMMATOpt refsopt;

			refsopt.REFSMMATopt = 2;
			refsopt.REFSMMATTime = T_Align;
			refsopt.vessel = calcParams.src;

			opt.REFSMMAT = REFSMMATCalc(&refsopt);

			//Nav Check Calculation (T -42m) HHMMSS
			OrbMech::SStoHHMMSS(abs(P30TIG - 42.0*60.0), hh, mm, ss, 1.0);

			NavGET = OrbMech::HHMMSSToSS(hh, mm, int(ss));
			opt.navcheckGET = NavGET;
		}
		else
		{
			opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);

			//Nav Check Calculation (T -42m)
			OrbMech::SStoHHMMSS(abs(P30TIG - 42.0*60.0), hh, mm, ss, 60.0);
			NavGET = OrbMech::HHMMSSToSS(hh, mm, 0);
			opt.navcheckGET = NavGET;
		}

		AP7ManeuverPAD(opt, *form);

		OrbMech::format_time_XXHMMSS(T_ALIGN_HMS, T_Align);

		if (preliminary)
		{
			sprintf(form->purpose, "Preliminary NCC-1");
			sprintf(form->remarks, "T-Align: %s", T_ALIGN_HMS);
		}
		else
		{
			sprintf(form->purpose, "NCC-1");
			sprintf(form->remarks, "Ullage: 4 jet, 15 seconds  Posigrade, Pitch down 70 degrees, Heads up");
		}

		AGCStateVectorUpdate(buffer1, 1, 1, sv_A.sv);
		AGCStateVectorUpdate(buffer2, 1, 3, sv_P.sv);
		CMCExternalDeltaVUpdate(buffer3, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM and S-IVB state vectors, Target load");
		}
	}
	break;
	case 8: //MISSION C NCC2 MANEUVER
	{
		TwoImpulseOpt lambert;
		TwoImpulseResuls res;
		AP7ManPADOpt opt;
		VehicleDataBlock sv_A, sv_P, sv_A1, sv_P1;
		PLAWDTOutput WeightsTable;
		VECTOR3 dV_LVLH;
		double GET_TIG_imp, GMT_TIG_imp, P30TIG;

		AP7MNV *form = (AP7MNV *)pad;

		WeightsTable = GetWeightsTable(calcParams.src, true, false);

		//Get state vectors
		sv_A = StateVectorCalcDataBlock(calcParams.src, PZMPTCSM.ConfigurationArea, PZMPTCSM.KFactor);
		sv_P = StateVectorCalcDataBlock(calcParams.tgt, PZMPTLEM.ConfigurationArea, PZMPTLEM.KFactor);

		GET_TIG_imp = OrbMech::HHMMSSToSS(27, 30, 0);
		GMT_TIG_imp = GMTfromGET(GET_TIG_imp);

		lambert.mode = 5;
		lambert.T1 = GMTfromGET(GET_TIG_imp);
		lambert.T2 = GMTfromGET(OrbMech::HHMMSSToSS(28, 1, 0));
		lambert.ChaserVehicle = RTCC_MPT_CSM;
		lambert.sv_C = sv_A;
		lambert.sv_T = sv_P;
		lambert.DH = 8.0*1852.0;
		lambert.PhaseAngle = 1.32*RAD;

		PMSTICN(lambert, res);

		if (length(res.dV) < 10.0*0.3048) //10 fps
		{
			scrubbed = true;
		}

		if (scrubbed)
		{
			DeltaV_LVLH = _V(0, 0, 0);
			sprintf(upMessage, "NCC-2 has been scrubbed.");
		}
		else
		{
			PMMMPTInput in;
			char buffer1[1000];
			char buffer2[1000];
			char buffer3[1000];
			int enginetype;
			enginetype = mcc->mcc_calcs.SPSRCSDecision(SystemParameters.MCTST1 / WeightsTable.ConfigWeight, res.dV);

			mcc->mcc_calcs.StoreStateVector(res.sv_tig2);

			in.CONFIG = 1; //CSM
			in.CSMWeight = WeightsTable.CSMWeight;
			in.sv_before = res.sv_tig.sv;
			in.V_aft = res.sv_tig.sv.V + res.dV;
			in.DETU = 15.0; //Ullage
			in.UT = true; //4 jets
			in.IgnitionTimeOption = false;
			in.IterationFlag = true;
			in.LMWeight = 0.0;
			in.Thruster = enginetype;
			in.VC = RTCC_MANVEHICLE_CSM;
			in.VehicleArea = WeightsTable.ConfigArea;

			double GMT_TIG;
			PoweredFlightProcessor(in, GMT_TIG, dV_LVLH);
			P30TIG = GETfromGMT(GMT_TIG);
			DeltaV_LVLH = dV_LVLH;

			opt.TIG = P30TIG;
			opt.dV_LVLH = dV_LVLH;
			opt.enginetype = enginetype;
			opt.HeadsUp = false;
			opt.sxtstardtime = 0;
			opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
			opt.navcheckGET = 0;
			opt.UllageDT = 15.0;
			opt.UllageThrusterOpt = true;
			opt.sv0 = sv_A.sv;
			opt.WeightsTable = WeightsTable;

			//Propagate to time tags
			sv_A1 = coast(sv_A, GMT_TIG_imp - (12.0 * 60.0) - sv_A.sv.GMT);
			sv_P1 = coast(sv_P, GMT_TIG_imp + (12.0 * 60.0) - sv_P.sv.GMT);

			AP7ManeuverPAD(opt, *form);
			sprintf(form->purpose, "NCC-2");

			if (enginetype == RTCC_ENGINETYPE_CSMSPS)
			{
				sprintf(form->remarks, "Heads down, P40 SPS");
			}
			else
			{
				sprintf(form->remarks, "Heads down, P41 RCS 4 jet +X thrusters");
			}

			AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv_A1.sv);
			AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv_P1.sv);
			CMCExternalDeltaVUpdate(buffer3, P30TIG, dV_LVLH);

			sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM and S-IVB state vectors, Target load");
			}
		}
	}
	break;
	case 9: //MISSION C NSR MANEUVER
	{
		SPQOpt spqopt;
		AP7ManPADOpt opt;
		PMMMPTInput in;
		SPQResults res;
		double P30TIG, NavGET;
		bool NCC2scrubbed;
		VehicleDataBlock sv_A, sv_P, sv_A1, sv_P1;
		VECTOR3 dV_LVLH;
		PLAWDTOutput WeightsTable;
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		if (length(DeltaV_LVLH) != 0.0)
		{
			mcc->mcc_calcs.RestoreStateVector(sv_A);
			NCC2scrubbed = false;
		}
		else
		{
			sv_A = StateVectorCalcDataBlock(calcParams.src);
			NCC2scrubbed = true;
		}

		sv_P = StateVectorCalcDataBlock(calcParams.tgt, PZMPTLEM.ConfigurationArea, PZMPTLEM.KFactor);

		WeightsTable = GetWeightsTable(calcParams.src, true, false);

		AP7MNV *form = (AP7MNV *)pad;

		spqopt.E = 27.45*RAD;
		spqopt.sv_A = sv_A;
		spqopt.sv_P = sv_P;
		spqopt.GMT_CSI = -1;
		spqopt.GMT_CDH = FindDH(sv_A, sv_P, GMTfromGET(28.0*3600.0 + 1.0*60.0), 8.0*1852.0);

		ConcentricRendezvousProcessor(spqopt, res);

		in.CONFIG = 1; //CSM
		in.CSMWeight = sv_A.Weight;
		in.sv_before = res.sv_C[0].sv;
		in.V_aft = res.sv_C_apo[0].sv.V;
		in.DETU = 15.0; //Ullage
		in.UT = true; //4 jets
		in.IgnitionTimeOption = false;
		in.IterationFlag = false;
		in.LMWeight = 0.0;
		in.Thruster = RTCC_ENGINETYPE_CSMSPS;
		in.VC = RTCC_MANVEHICLE_CSM;
		in.VehicleArea = WeightsTable.ConfigArea;
		in.HeadsUpIndicator = false;

		double GMT_TIG;
		PoweredFlightProcessor(in, GMT_TIG, dV_LVLH);
		P30TIG = GETfromGMT(GMT_TIG);

		int hh, mm;
		double ss;
		//Nav Check Calculation (T -42m)
		OrbMech::SStoHHMMSS(abs(P30TIG - 42.0*60.0), hh, mm, ss, 60.0);
		NavGET = OrbMech::HHMMSSToSS(hh, mm, 0);

		opt.TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = false;
		opt.sxtstardtime = -10.0 * 60.0;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		opt.navcheckGET = NavGET;
		opt.UllageDT = 15.0;
		opt.UllageThrusterOpt = true;
		opt.sv0 = sv_A.sv;
		opt.WeightsTable = WeightsTable;

		AP7ManeuverPAD(opt, *form);
		sprintf(form->purpose, "NSR");
		sprintf(form->remarks, "Ullage: 4 jet, 15 seconds  Heads down, Retrograde");

		if (NCC2scrubbed)
		{
			//Propagate to time tags
			sv_A1 = coast(sv_A, GMT_TIG - (12.0 * 60.0) - sv_A.sv.GMT);
			sv_P1 = coast(sv_P, GMT_TIG + (12.0 * 60.0) - sv_P.sv.GMT);

			AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv_A1.sv);
			AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv_P1.sv);
			CMCExternalDeltaVUpdate(buffer3, P30TIG, dV_LVLH);

			sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM and S-IVB state vectors, Target load");
			}
		}
	}
	break;
	case 10: //MISSION C TPI MANEUVER
	{
		TwoImpulseOpt lambert;
		TwoImpulseResuls res;
		AP7TPIPADOpt opt;
		VehicleDataBlock sv_A, sv_P;

		AP7TPI *form = (AP7TPI *)pad;

		sv_A = StateVectorCalcDataBlock(calcParams.src, PZMPTCSM.ConfigurationArea, PZMPTCSM.KFactor);
		sv_P = StateVectorCalcDataBlock(calcParams.tgt, PZMPTLEM.ConfigurationArea, PZMPTLEM.KFactor);

		lambert.mode = 5;
		lambert.T1 = -1.0;
		lambert.T2 = -1.0;
		lambert.ChaserVehicle = RTCC_MPT_CSM;
		lambert.sv_C = sv_A;
		lambert.sv_T = sv_P;
		lambert.Elev = 27.45*RAD;
		lambert.WT = 140.0*RAD;

		PMSTICN(lambert, res);

		opt.dV_LVLH = res.dV_LVLH;
		opt.TIG = res.T1;
		opt.sv_A = sv_A.sv;
		opt.sv_P = sv_P.sv;
		opt.mass = calcParams.src->GetMass();

		AP7TPIPAD(opt, *form);
	}
	break;
	case 11: //MISSION C FINAL SEPARATION MANEUVER
	{
		AP7ManPADOpt opt;

		AP7MNV *form = (AP7MNV *)pad;

		EphemerisData sv;
		PLAWDTOutput WeightsTable;

		sv = StateVectorCalcEphem(calcParams.src);
		WeightsTable = GetWeightsTable(calcParams.src, true, false);

		opt.dV_LVLH = _V(2.0*0.3048, 0.0, 0.0);
		opt.enginetype = RTCC_ENGINETYPE_CSMRCSMINUS4;
		opt.HeadsUp = false;
		opt.navcheckGET = 0;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		opt.sxtstardtime = 0;
		opt.TIG = OrbMech::HHMMSSToSS(30.0, 20.0, 0.0);
		opt.sv0 = sv;
		opt.WeightsTable = WeightsTable;

		AP7ManeuverPAD(opt, *form);
		sprintf(form->purpose, "SEPARATION");
		sprintf(form->remarks, "Posigrade, Heads down, -X thrusters  Burn will take place in front of booster");
	}
	break;
	case 12: //MISSION C BLOCK DATA 4
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 4;
	}
	break;
	case 13: //MISSION C BLOCK DATA 5
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 5;
	}
	break;
	case 14: //MISSION C BLOCK DATA 6
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 6;
	}
	break;
	case 15: //MISSION C BLOCK DATA 7
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 7;
	}
	break;
	case 16: //MISSION C BLOCK DATA 8
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 8;
	}
	break;
	case 17: //MISSION C SPS-3: SCS MANEUVER AND SLOSH DAMPING TEST
	{
		AP7MNV *form = (AP7MNV *)pad;

		PMMMPTInput in;
		GMPOpt orbopt;
		REFSMMATOpt refsopt;
		AP7ManPADOpt manopt;
		VECTOR3 dV_LVLH, dV_imp;
		double P30TIG, TIG_imp, LOA, GET_TH, NavGET;
		MATRIX3 REFSMMAT;
		EphemerisData sv, sv1, sv2;
		PLAWDTOutput WeightsTable;
		AEGBlock aeg;
		AEGDataBlock sv_A;
		int KAOP, KE;
		double INFO[10], H_A;
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		sv = StateVectorCalcEphem(calcParams.src);
		WeightsTable = GetWeightsTable(calcParams.src, true, false);

		//Threshold time
		GET_TH = OrbMech::HHMMSSToSS(75, 18, 0);

		//Take state vector to threshold
		sv1 = coast(sv, GMTfromGET(GET_TH) - sv.GMT, RTCC_MPT_CSM);

		//Convert to AEG and initialize
		aeg = SVToAEG(sv1, WeightsTable.ConfigArea, WeightsTable.ConfigWeight, PZMPTCSM.KFactor);
		PMMAEGS(aeg.Header, aeg.Data, aeg.Data);

		//Calculate required apsidal shift
		LOA = 120.0*RAD - aeg.Data.coe_mean.g;

		//Calculate current apogee
		KAOP = 1; //Perigee only
		KE = 0; //ECI
		PMMAPD(aeg.Header, aeg.Data, KAOP, KE, INFO, &sv_A, NULL);

		//Use this apogee height, add 0.1 NM just to make sure it works
		H_A = INFO[4] + 0.1*1852.0;
		//Raise to 160 NM if it is currently lower
		if (H_A < 160.0*1852.0)
		{
			H_A = 160.0*1852.0;
		}

		//Iterate on TIG to find solution that gives us the desired apsidal shift
		orbopt.ManeuverCode = RTCC_GMP_HBT;
		orbopt.sv_in = sv;
		orbopt.H_A = H_A;
		orbopt.H_P = 90.0*1852.0;
		orbopt.TIG_GET = GET_TH;

		int n = 0, nmax = 100;
		double dloa_apo, dloa, ddt;

		ddt = 2.0*60.0;

		//Make sure it doesn't iterate forever
		while (abs(ddt) > 1.0 && orbopt.TIG_GET < GET_TH + 2.0*3600.0 && n < nmax)
		{
			GeneralManeuverProcessor(&orbopt, dV_imp, TIG_imp);

			dloa = OrbMech::calculateDifferenceBetweenAngles(LOA, PZGPMDIS.Del_G);

			//Don't switch sign on ddt on first step. Change when dloa also switches sign, but only when passing through zero
			if (n > 0 && dloa*dloa_apo < 0 && abs(dloa - dloa_apo) < PI05)
			{
				ddt = -ddt / 2.0;
			}

			orbopt.TIG_GET += ddt;
			dloa_apo = dloa;
			n++;
		}

		//Finally, add out-of-plane DV
		PZGPMELM.V_after += tmul(OrbMech::LVLH_Matrix(PZGPMELM.SV_before.R, PZGPMELM.SV_before.V), _V(-0.8, 200.0, 0.0)*0.3048);

		in.CONFIG = 1; //CSM
		in.CSMWeight = WeightsTable.CSMWeight;
		in.sv_before = PZGPMELM.SV_before;
		in.V_aft = PZGPMELM.V_after;
		in.DETU = 15.0; //Ullage
		in.UT = true; //4 jets
		in.IgnitionTimeOption = false;
		in.IterationFlag = false;
		in.LMWeight = 0.0;
		in.Thruster = RTCC_ENGINETYPE_CSMSPS;
		in.VC = RTCC_MANVEHICLE_CSM;
		in.VehicleArea = WeightsTable.ConfigArea;
		in.HeadsUpIndicator = true;

		double GMT_TIG;
		PoweredFlightProcessor(in, GMT_TIG, dV_LVLH);
		P30TIG = GETfromGMT(GMT_TIG);

		refsopt.dV_LVLH = dV_LVLH;
		refsopt.REFSMMATTime = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		int hh, mm;
		double ss;
		//Nav Check Calculation (T -42m)
		OrbMech::SStoHHMMSS(abs(P30TIG - 42.0*60.0), hh, mm, ss, 60.0);
		NavGET = OrbMech::HHMMSSToSS(hh, mm, 0);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = true;
		manopt.navcheckGET = NavGET;
		manopt.REFSMMAT = REFSMMAT;
		manopt.TIG = P30TIG;
		manopt.sxtstardtime = -25.0*60.0;
		manopt.sv0 = sv;
		manopt.WeightsTable = WeightsTable;

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

		sv2 = coast(sv, GMTfromGET(P30TIG - 12.0*60.0) - sv.GMT); //Time tag to TIG -12m

		AP7ManeuverPAD(manopt, *form);
		sprintf(form->purpose, "SPS-3");
		sprintf(form->remarks, "SCS control, Ullage: 4 jet, 15 seconds  Out of plane south, Slightly retrograde, Slightly pitched down");

		AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv2);
		AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv2);
		CMCExternalDeltaVUpdate(buffer3, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vectors, Target load");
		}
	}
	break;
	case 18: //MISSION C BLOCK DATA 9
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 9;
	}
	break;
	case 19: //MISSION C BLOCK DATA 10
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 10;
	}
	break;
	case 20: //MISSION C BLOCK DATA 11
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 11;
	}
	break;
	case 21: //MISSION C BLOCK DATA 12
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 12;
	}
	break;
	case 22: //MISSION C BLOCK DATA 13
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 13;
	}
	break;
	case 23: //MISSION C SPS-4: MINIMUM IMPULSE
	case 34: //MISSION C SPS-6: MINIMUM IMPULSE
	{
		AP7MNV *form = (AP7MNV *)pad;
		AP7ManPADOpt opt;
		REFSMMATOpt refsopt;
		double t_burn, F, dv, P30TIG, NavGET;
		VECTOR3 dV_LVLH;
		MATRIX3 REFSMMAT;
		VehicleDataBlock sv, sv_1;
		PLAWDTOutput WeightsTable;
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		F = SystemParameters.MCTST1;
		t_burn = 0.5;

		WeightsTable = GetWeightsTable(calcParams.src, true, false);

		dv = F / WeightsTable.ConfigWeight * t_burn + SystemParameters.MCTCT1 / WeightsTable.ConfigWeight * 20.0;

		sv = StateVectorCalcDataBlock(calcParams.src);

		if (fcn == 23)
		{
			dV_LVLH = _V(dv, 0.0, 0.0);
			P30TIG = OrbMech::HHMMSSToSS(120, 43, 0);
		}
		else
		{
			dV_LVLH = _V(0.0, dv, 0.0);
			P30TIG = OrbMech::HHMMSSToSS(210, 8, 0);
		}

		refsopt.dV_LVLH = dV_LVLH;
		refsopt.REFSMMATTime = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		int hh, mm;
		double ss;
		if (fcn == 23)
		{
			//Nav Check Calculation (T -42m)
			OrbMech::SStoHHMMSS(abs(P30TIG - 42.0*60.0), hh, mm, ss, 60.0);
			NavGET = OrbMech::HHMMSSToSS(hh, mm, 0);

			opt.sxtstardtime = -30.0*60.0;
		}
		else
		{
			//Nav Check Calculation (T -42m)
			OrbMech::SStoHHMMSS(abs(P30TIG - 42.0*60.0), hh, mm, ss, 60.0);
			NavGET = OrbMech::HHMMSSToSS(hh, mm, 0);

			opt.sxtstardtime = -10.0*60.0;
		}

		opt.navcheckGET = NavGET;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = true;
		opt.REFSMMAT = REFSMMAT;
		opt.TIG = P30TIG;
		opt.UllageThrusterOpt = false;
		opt.UllageDT = 20.0;
		opt.sv0 = sv.sv;
		opt.WeightsTable = WeightsTable;

		//Time tagged SV
		sv_1 = coast(sv, GMTfromGET(P30TIG - 12.0*60.0) - sv.sv.GMT); //Time tag to TIG -12m

		//Store Postburn SV
		EphemerisData sv_cut;
		PLAWDTOutput WeightsTableOut;
		ExecuteManeuver(sv.sv, WeightsTable, P30TIG, dV_LVLH, RTCC_ENGINETYPE_CSMSPS, sv_cut, WeightsTableOut);
		mcc->mcc_calcs.StoreStateVector(sv_cut, WeightsTableOut.ConfigWeight);

		AP7ManeuverPAD(opt, *form);

		if (fcn == 23)
		{
			sprintf(form->purpose, "SPS-4, MINIMUM IMPULSE");
			sprintf(form->remarks, "Heads up, Posigrade,  Ullage: 2 jet (B/D), 20 seconds");
		}
		else
		{
			sprintf(form->purpose, "SPS-6, MINIMUM IMPULSE");
			sprintf(form->remarks, "Heads up, Out of plane south,  Ullage: 2 jet (B/D), 20 seconds");
		}

		AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv_1.sv);
		AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv_1.sv);
		CMCExternalDeltaVUpdate(buffer3, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, Target load");
		}
	}
	break;
	case 55: //Rev 77 Landmark Tracking Update
	case 56: //Rev 90 Landmark Tracking Update
	case 57: //Rev 91 Landmark Tracking Update
	case 58: //Rev 92 Landmark Tracking Update
	case 59: //Rev 135 Landmark Tracking Update
	case 60: //Rev 136 Landmark Tracking Update
	{
		LMARKTRKPADOpt opt;
		EphemerisData sv0;

		AP11LMARKTRKPAD *form = (AP11LMARKTRKPAD *)pad;

		sv0 = StateVectorCalcEphem(calcParams.src);

		opt.sv0 = sv0;
		form->type = 0;
		if (fcn == 55)
		{
			sprintf(form->LmkID[0], "010");
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(122, 0, 0);
			opt.alt[0] = 18.52;
			opt.lat[0] = 28.876*RAD;
			opt.lng[0] = -112.584*RAD;

			sprintf(form->LmkID[1], "067");
			opt.LmkTime[1] = OrbMech::HHMMSSToSS(122, 10, 0);
			opt.alt[1] = -18.52;
			opt.lat[1] = 17.812*RAD;
			opt.lng[1] = -71.654*RAD;

			sprintf(form->LmkID[2], "141");
			opt.LmkTime[2] = OrbMech::HHMMSSToSS(122, 20, 0);
			opt.alt[2] = 55.56;
			opt.lat[2] = -5.91*RAD;
			opt.lng[2] = -35.245*RAD;

			opt.entries = 3;
		}
		else if (fcn == 56)
		{
			sprintf(form->LmkID[0], "008");
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(142, 30, 0);
			opt.alt[0] = 0.0;
			opt.lat[0] = 27.852*RAD;
			opt.lng[0] = -115.081*RAD;

			sprintf(form->LmkID[1], "037");
			opt.LmkTime[1] = OrbMech::HHMMSSToSS(142, 40, 0);
			opt.alt[1] = 0.0;
			opt.lat[1] = 30.077*RAD;
			opt.lng[1] = -85.581*RAD;

			sprintf(form->LmkID[2], "209");
			opt.LmkTime[2] = OrbMech::HHMMSSToSS(142, 50, 0);
			opt.alt[2] = 2833.56;
			opt.lat[2] = 14.947*RAD;
			opt.lng[2] = -24.353*RAD;

			opt.entries = 3;
		}
		else if (fcn == 57)
		{
			sprintf(form->LmkID[0], "018");
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(144, 10, 0);
			opt.alt[0] = 1185.28;
			opt.lat[0] = 31.855*RAD;
			opt.lng[0] = -106.374*RAD;

			sprintf(form->LmkID[1], "225");
			opt.LmkTime[1] = OrbMech::HHMMSSToSS(144, 20, 0);
			opt.alt[1] = 0.0;
			opt.lat[1] = -22.882*RAD;
			opt.lng[1] = 14.445*RAD;

			opt.entries = 2;
		}
		else if (fcn == 58)
		{
			sprintf(form->LmkID[0], "010");
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(145, 30, 0);
			opt.alt[0] = 18.52;
			opt.lat[0] = 28.876*RAD;
			opt.lng[0] = -112.584*RAD;

			sprintf(form->LmkID[1], "142");
			opt.LmkTime[1] = OrbMech::HHMMSSToSS(145, 50, 0);
			opt.alt[1] = 0.0;
			opt.lat[1] = -6.966*RAD;
			opt.lng[1] = -34.843*RAD;

			opt.entries = 2;
		}
		else if (fcn == 59)
		{
			sprintf(form->LmkID[0], "048");
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(214, 40, 0);
			opt.alt[0] = 0.0;
			opt.lat[0] = 25.666*RAD;
			opt.lng[0] = -80.158*RAD;

			sprintf(form->LmkID[1], "225");
			opt.LmkTime[1] = OrbMech::HHMMSSToSS(215, 0, 0);
			opt.alt[1] = 0.0;
			opt.lat[1] = -22.882*RAD;
			opt.lng[1] = 14.445*RAD;

			opt.entries = 2;
		}
		else if (fcn == 60)
		{
			sprintf(form->LmkID[0], "011");
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(216, 10, 0);
			opt.alt[0] = 0.0;
			opt.lat[0] = 27.839*RAD;
			opt.lng[0] = -110.885*RAD;

			sprintf(form->LmkID[1], "128");
			opt.LmkTime[1] = OrbMech::HHMMSSToSS(216, 20, 0);
			opt.alt[1] = 18.52;
			opt.lat[1] = 10.576*RAD;
			opt.lng[1] = -71.538*RAD;

			sprintf(form->LmkID[2], "144");
			opt.LmkTime[2] = OrbMech::HHMMSSToSS(216, 30, 0);
			opt.alt[2] = 0.0;
			opt.lat[2] = -10.506*RAD;
			opt.lng[2] = -36.419*RAD;

			sprintf(form->LmkID[3], "227");
			opt.LmkTime[3] = OrbMech::HHMMSSToSS(216, 40, 0);
			opt.alt[3] = 0.0;
			opt.lat[3] = -28.637*RAD;
			opt.lng[3] = 16.46*RAD;

			opt.entries = 4;
		}

		LandmarkTrackingPAD(opt, *form);
	}
	break;
	case 24: //MISSION C BLOCK DATA 14
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 14;
	}
	break;
	case 25: //MISSION C BLOCK DATA 15
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 15;
	}
	break;
	case 26: //MISSION C BLOCK DATA 16
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 16;
	}
	break;
	case 27: //MISSION C BLOCK DATA 17
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 17;
	}
	break;
	case 28: //MISSION C SPS-5: PUGS TEST AND MTVC
	{
		AP7MNV *form = (AP7MNV *)pad;

		PMMMPTInput in;
		GMPOpt orbopt;
		REFSMMATOpt refsopt;
		AP7ManPADOpt manopt;
		VECTOR3 dV_LVLH, dV_imp;
		double P30TIG, TIG_imp, NavGET;
		MATRIX3 REFSMMAT;
		VehicleDataBlock sv, sv_1;
		PLAWDTOutput WeightsTable;
		VECTOR3 GDCangles;
		char SetStars[64];
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];
		char alignstars[1000];

		sv = StateVectorCalcDataBlock(calcParams.src);
		WeightsTable = GetWeightsTable(calcParams.src, true, false);

		orbopt.H_A = 240.0*1852.0;
		orbopt.H_P = 90.0*1852.0;
		//Eastern Test Range
		orbopt.long_D = -88.455*RAD;
		orbopt.dLAN = -6.7*RAD;
		orbopt.ManeuverCode = RTCC_GMP_NHL;
		orbopt.sv_in = sv.sv;
		orbopt.TIG_GET = OrbMech::HHMMSSToSS(164, 30, 0);

		GeneralManeuverProcessor(&orbopt, dV_imp, TIG_imp);

		in.CONFIG = 1; //CSM
		in.CSMWeight = WeightsTable.ConfigWeight;
		in.sv_before = PZGPMELM.SV_before;
		in.V_aft = PZGPMELM.V_after;
		in.DETU = 20.0; //Ullage
		in.UT = false; //2 jets
		in.IgnitionTimeOption = false;
		in.IterationFlag = true;
		in.LMWeight = 0.0;
		in.Thruster = RTCC_ENGINETYPE_CSMSPS;
		in.VC = RTCC_MANVEHICLE_CSM;
		in.VehicleArea = WeightsTable.ConfigArea;
		in.HeadsUpIndicator = true;

		double GMT_TIG;
		PoweredFlightProcessor(in, GMT_TIG, dV_LVLH);

		P30TIG = GETfromGMT(GMT_TIG);

		refsopt.dV_LVLH = dV_LVLH;
		refsopt.REFSMMATTime = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		int hh, mm;
		double ss;
		//Nav Check Calculation (T -42m)
		OrbMech::SStoHHMMSS(abs(P30TIG - 42.0*60.0), hh, mm, ss, 60.0);
		NavGET = OrbMech::HHMMSSToSS(hh, mm, 0);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = true;
		manopt.navcheckGET = NavGET;
		manopt.REFSMMAT = REFSMMAT;
		manopt.TIG = P30TIG;
		manopt.sxtstardtime = -25.0*60.0;
		manopt.UllageThrusterOpt = false;
		manopt.UllageDT = 20.0;
		manopt.sv0 = sv.sv;
		manopt.WeightsTable = WeightsTable;

		//Time tagged SV
		sv_1 = coast(sv, GMTfromGET(P30TIG - 12.0*60.0) - sv.sv.GMT); //Time tag to TIG -12m

		//Store Postburn SV
		EphemerisData sv_cut;
		PLAWDTOutput WeightsTableOut;
		ExecuteManeuver(sv.sv, WeightsTable, P30TIG, dV_LVLH, RTCC_ENGINETYPE_CSMSPS, sv_cut, WeightsTableOut);
		mcc->mcc_calcs.StoreStateVector(sv_cut, WeightsTableOut.ConfigWeight);

		mcc->mcc_calcs.BackupGDCAlignment(sv_1, manopt.sxtstardtime, REFSMMAT, 1, GDCangles, SetStars);
		sprintf(alignstars, "SET STARS: %s  RALIGN %03.0f  PALIGN %03.0f  YALIGN %03.0f", SetStars, GDCangles.x, GDCangles.y, GDCangles.z);

		AP7ManeuverPAD(manopt, *form);
		sprintf(form->purpose, "SPS-5 / PUGS");

		form->Vc += 100.0;

		sprintf(form->remarks, "Ullage: 2 jet (B/D), 20 seconds, Out of plane south, Heads up  MTVC takeover at TIG+%.0f seconds  Manual cutoff at DV counter equal 100 ft/s  %s", form->burntime - 30.0, alignstars);

		AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv_1.sv);
		AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv_1.sv);
		CMCExternalDeltaVUpdate(buffer3, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, Target load");
		}
	}
	break;
	case 29: //MISSION C BLOCK DATA 18
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 18;
	}
	break;
	case 30: //MISSION C BLOCK DATA 19
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 19;
	}
	break;
	case 31: //MISSION C BLOCK DATA 20
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 20;
	}
	break;
	case 32: //MISSION C BLOCK DATA 21
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 21;
	}
	break;
	case 33: //MISSION C BLOCK DATA 22
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 22;
	}
	break;
	case 35: //MISSION C BLOCK DATA 23
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 23;
	}
	break;
	case 36: //MISSION C SV PAD (122:00:00)
	case 92: //MISSION C SV PAD w/ Uplink (142:13:00)
	case 93: //MISSION C SV PAD (215:44:00)
	{
		VehicleDataBlock sv, sv_1;
		double NavGET, NavGMT;
		char buffer1[1000];
		char buffer2[1000];

		P27PAD *form = (P27PAD *)pad;
		P27Opt opt;

		sv = StateVectorCalcDataBlock(calcParams.src);

		if (fcn == 36)
		{
			NavGET = OrbMech::HHMMSSToSS(122, 0, 0);  //Nav Check GET
		}
		else if (fcn == 92)
		{
			NavGET = OrbMech::HHMMSSToSS(142, 13, 0);  //Nav Check GET
		}
		else if (fcn == 93)
		{
			NavGET = OrbMech::HHMMSSToSS(215, 44, 0);  //Nav Check GET
		}

		opt.navcheckGET = NavGET;
		opt.SVGET = NavGET + (30.0 * 60.0);
		opt.sv0 = sv.sv;

		P27PADCalc(opt, *form);

		NavGMT = GMTfromGET(NavGET + (30.0 * 60.0));

		sv_1 = coast(sv, NavGMT - sv.sv.GMT); //Time tag to Nav Check time + 30m

		if (fcn == 90)
		{
			AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv_1.sv);
			AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv_1.sv);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vectors");
			}
		}
	}
	break;
	case 37: //MISSION C BLOCK DATA 24
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 24;
	}
	break;
	case 38: //MISSION C BLOCK DATA 25
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 25;
	}
	break;
	case 39: //MISSION C SPS-7: SCS MANEUVER
	{
		AP7MNV *form = (AP7MNV *)pad;

		AP11MNV  tempPAD;
		AP11ManPADOpt tempopt;

		PMMMPTInput in;
		GMPOpt orbopt;
		REFSMMATOpt refsopt;
		AP7ManPADOpt manopt;
		VECTOR3 dV_LVLH, dV_imp;
		double P30TIG, TIG_imp, NavGET;
		MATRIX3 REFSMMAT;
		VehicleDataBlock sv, sv0, sv1, sv2;
		PLAWDTOutput WeightsTable;
		VECTOR3 GDCangles;
		char SetStars[64];
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];
		char alignstars[1000];

		AEGBlock aeg;
		AEGDataBlock sv_P;
		double INFO[10];
		int KAOP, KE;

		sv = StateVectorCalcDataBlock(calcParams.src);
		WeightsTable = GetWeightsTable(calcParams.src, true, false);

		sv0 = sv;
		orbopt.dLOA = 0.0;
		orbopt.TIG_GET = OrbMech::HHMMSSToSS(238, 40, 0);
		orbopt.ManeuverCode = RTCC_GMP_SAO;
		orbopt.sv_in = sv.sv;

		//Do this three times to converge properly
		for (int i = 0; i < 3; i++)
		{
			//Take state vector to estimated time of rev 164 crossing
			sv1 = coast(sv0, GMTfromGET(260.0*3600.0) - sv0.sv.GMT);
			//Convert to AEG and initialize
			aeg = SVToAEG(sv1.sv, WeightsTable.ConfigArea, WeightsTable.ConfigWeight, PZMPTCSM.KFactor);
			PMMAEGS(aeg.Header, aeg.Data, aeg.Data);
			KAOP = -1; //Perigee only
			KE = 0; //ECI
			PMMAPD(aeg.Header, aeg.Data, KAOP, KE, INFO, NULL, &sv_P);
			//Place perigee at 30°W (actual Apollo 7 mission)
			orbopt.dLOA += OrbMech::calculateDifferenceBetweenAngles(INFO[8], -30.0*RAD);

			GeneralManeuverProcessor(&orbopt, dV_imp, TIG_imp);

			sv0.sv = PZGPMELM.SV_before;
			sv0.sv.V = PZGPMELM.V_after;
		}

		in.CONFIG = 1; //CSM
		in.CSMWeight = WeightsTable.ConfigWeight;
		in.sv_before = PZGPMELM.SV_before;
		in.V_aft = PZGPMELM.V_after;
		in.DETU = 15.0; //Ullage
		in.UT = true; //4 jets
		in.IgnitionTimeOption = false;
		in.IterationFlag = false;
		in.LMWeight = 0.0;
		in.Thruster = RTCC_ENGINETYPE_CSMSPS;
		in.VC = RTCC_MANVEHICLE_CSM;
		in.VehicleArea = WeightsTable.ConfigArea;
		in.HeadsUpIndicator = true;

		double GMT_TIG;
		PoweredFlightProcessor(in, GMT_TIG, dV_LVLH);
		P30TIG = GETfromGMT(GMT_TIG);

		//If the maneuver is too short, add some out-of-plane DV
		if (length(dV_LVLH) < 220.0*0.3048)
		{
			dV_LVLH.y = -100.0*0.3048;
		}

		refsopt.dV_LVLH = dV_LVLH;
		refsopt.REFSMMATTime = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		int hh, mm;
		double ss;
		//Nav Check Calculation (T -42m)
		OrbMech::SStoHHMMSS(abs(P30TIG - 42.0*60.0), hh, mm, ss, 60.0);
		NavGET = OrbMech::HHMMSSToSS(hh, mm, 0);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = true;
		manopt.navcheckGET = NavGET;
		manopt.REFSMMAT = REFSMMAT;
		manopt.TIG = P30TIG;
		manopt.sxtstardtime = -30.0 * 60.0;
		manopt.UllageThrusterOpt = true;
		manopt.UllageDT = 15.0;
		manopt.sv0 = sv.sv;
		manopt.WeightsTable = WeightsTable;

		//Time tagged SV
		sv2 = coast(sv, GMTfromGET(P30TIG - 12.0*60.0) - sv.sv.GMT); //Time tag to TIG -12m

		mcc->mcc_calcs.BackupGDCAlignment(sv2, manopt.sxtstardtime, REFSMMAT, 1, GDCangles, SetStars);
		sprintf(alignstars, "SET STARS: %s  RALIGN %03.0f  PALIGN %03.0f  YALIGN %03.0f", SetStars, GDCangles.x, GDCangles.y, GDCangles.z);

		AP7ManeuverPAD(manopt, *form);
		sprintf(form->purpose, "SPS-7");

		sprintf(form->remarks, "Ullage: 4 jet, 15 seconds, SCS AUTO  %s", alignstars);

		AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv2.sv);
		AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv2.sv);
		CMCExternalDeltaVUpdate(buffer3, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vectors, Target load");
		}
	}
	break;
	case 40: //MISSION C BLOCK DATA 26
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 26;
	}
	break;
	case 41: //MISSION C BLOCK DATA 27
	{
		AP7BLK *form = (AP7BLK *)pad;
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
		form->Num = 27;
	}
	break;
	case 43: //MISSION C NOMINAL DEORBIT ENTRY PAD
	{
		AP7ENT *form = (AP7ENT *)pad;

		EarthEntryPADOpt opt;
		REFSMMATOpt refsopt;
		MATRIX3 REFSMMAT;

		refsopt.vessel = calcParams.src;
		refsopt.dV_LVLH = DeltaV_LVLH;
		refsopt.REFSMMATTime = TimeofIgnition;
		refsopt.REFSMMATopt = 1;

		REFSMMAT = REFSMMATCalc(&refsopt);

		opt.dV_LVLH = DeltaV_LVLH;
		opt.P30TIG = TimeofIgnition;
		opt.REFSMMAT = REFSMMAT;
		opt.sv0 = StateVectorCalc(calcParams.src);
		opt.preburn = true;
		opt.lat = SplashLatitude;
		opt.lng = SplashLongitude;

		EarthOrbitEntry(opt, *form);
		if (mcc->AbortMode == 0)
		{
			sprintf(form->Area[0], "164-1A");
		}
		else
		{
			sprintf(form->Area[0], "Abort");
		}
		form->Lat[0] = SplashLatitude * DEG;
		form->Lng[0] = SplashLongitude * DEG;
	}
	break;
	case 44: //MISSION C POSTBURN ENTRY PAD
	{
		AP7ENT *form = (AP7ENT *)pad;

		EarthEntryPADOpt opt;

		opt.dV_LVLH = DeltaV_LVLH;
		opt.lat = SplashLatitude;
		opt.lng = SplashLongitude;
		opt.P30TIG = TimeofIgnition;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		opt.preburn = false;
		opt.sv0 = StateVectorCalc(calcParams.src);

		EarthOrbitEntry(opt, *form);
	}
	break;
	case 51: //GENERIC CSM AND TARGET STATE VECTOR UPDATE
	{
		EphemerisData sv_A, sv_P;
		char buffer1[1000];
		char buffer2[1000];

		sv_A = StateVectorCalcEphem(calcParams.src); //State vector for uplink
		sv_P = StateVectorCalcEphem(calcParams.tgt); //State vector for uplink

		AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv_A);
		AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv_P);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM and S-IVB state vectors");
		}
	}
	break;
	case 52: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (SV GET)
	case 75: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (92:05:00)
	case 76: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (94:15:00)
	case 77: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (102:30:00)
	case 78: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (128:30:00)
	case 79: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (143:47:00)
	case 83: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (144:50:00)
	case 84: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (154:30:00)
	case 85: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (175:30:00)
	case 86: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (193:10:00)
	case 87: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (199:30:00)
	case 88: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (214:20:00)
	case 89: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (221:30:00)
	case 90: //CSM STATE VECTOR UPDATE (BOTH SLOTS) AND NAV CHECK PAD (246:30:00)
	{
		VehicleDataBlock sv, sv_1;
		char buffer1[1000];
		char buffer2[1000];
		double NavGET, SVGMT;

		AP7NAV *form = (AP7NAV *)pad;

		sv = StateVectorCalcDataBlock(calcParams.src); //State vector for uplink

		if (fcn == 75)
		{
			NavGET = OrbMech::HHMMSSToSS(92, 5, 0);  //Nav Check GET
		}
		else if (fcn == 76)
		{
			NavGET = OrbMech::HHMMSSToSS(94, 15, 0);  //Nav Check GET
		}
		else if (fcn == 77)
		{
			NavGET = OrbMech::HHMMSSToSS(102, 30, 0);  //Nav Check GET
		}
		else if (fcn == 78)
		{
			NavGET = OrbMech::HHMMSSToSS(128, 30, 0);  //Nav Check GET
		}
		else if (fcn == 79)
		{
			NavGET = OrbMech::HHMMSSToSS(143, 47, 0);  //Nav Check GET
		}
		else if (fcn == 83)
		{
			NavGET = OrbMech::HHMMSSToSS(144, 50, 0);  //Nav Check GET
		}
		else if (fcn == 84)
		{
			NavGET = OrbMech::HHMMSSToSS(154, 30, 0);  //Nav Check GET
		}
		else if (fcn == 85)
		{
			NavGET = OrbMech::HHMMSSToSS(175, 30, 0);  //Nav Check GET
		}
		else if (fcn == 86)
		{
			NavGET = OrbMech::HHMMSSToSS(193, 10, 0);  //Nav Check GET
		}
		else if (fcn == 87)
		{
			NavGET = OrbMech::HHMMSSToSS(199, 30, 0);  //Nav Check GET
		}
		else if (fcn == 88)
		{
			NavGET = OrbMech::HHMMSSToSS(214, 20, 0);  //Nav Check GET
		}
		else if (fcn == 89)
		{
			NavGET = OrbMech::HHMMSSToSS(221, 30, 0);  //Nav Check GET
		}
		else if (fcn == 90)
		{
			NavGET = OrbMech::HHMMSSToSS(246, 30, 0);  //Nav Check GET
		}
		else
		{
			NavGET = GETfromGMT(sv.sv.GMT) - 30.0 * 60.0; //Nav Check GET as SV time - 30m
		}

		SVGMT = GMTfromGET(NavGET + 30.0 * 60.0);

		sv_1 = coast(sv, SVGMT - sv.sv.GMT); //Time tag to Nav Check time

		NavCheckPAD(sv_1, *form, NavGET);

		AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv_1.sv);
		AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv_1.sv);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vectors");
		}
	}
	break;
	case 61: //W-MATRIX UPDATE (9x9)
	{
		char buffer1[1000];
		int emem[5];

		//W-Matrix update
		emem[0] = 5;	//Size
		emem[1] = 2004; //Address
		emem[2] = 2;	//WORBPOS = 200 ft
		emem[3] = 12;	//WORBVEL = 0.2 ft/s
		emem[4] = 5;	//S22WSUBL = 500 ft

		V7XUpdate(71, buffer1, emem, 5);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "W-Matrix update (9 x 9)");
		}
	}
	break;
	case 62: //SV WITH LOX DUMP TIMES
	{
		EphemerisData sv;
		double INSTIME;

		char buffer1[1000];
		char buffer2[1000];
		char LOXSTART[32];
		char LOXEND[32];

		GENERICPAD *form = (GENERICPAD *)pad;

		sv = StateVectorCalcEphem(calcParams.src); //State vector for uplink

		INSTIME = mcc->mcc_calcs.GetLVDCOrbitalInsertionTime(calcParams.src);

		OrbMech::format_time_XXHMMSS(LOXSTART, INSTIME + 5052.0);
		OrbMech::format_time_XXHMMSS(LOXEND, INSTIME + 5773.0);

		AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv);
		AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv);

		sprintf(form->paddata, "LOX Dump Start: %s  LOX Dump End: %s", LOXSTART, LOXEND);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 63: //P52 NAV STAR PAD
	{
		GENERICPAD *form = (GENERICPAD *)pad;

		sprintf(form->paddata, "P52 Nav Stars:  Star 2 and Star 4");
	}
	break;
	case 64: //SIVB PITCHDOWN UPDATE
	{
		double GRRTIME;

		char PITCHDWN[32];
		char INERTIAL[32];

		GENERICPAD *form = (GENERICPAD *)pad;

		GRRTIME = GETfromGMT(GetIUClockZero());

		OrbMech::format_time_XXHMMSS(PITCHDWN, GRRTIME + 9780.0);
		OrbMech::format_time_XXHMMSS(INERTIAL, GRRTIME + 10275.0);

		sprintf(form->paddata, "GET of S-IVB Pitchdown: %s  GET of S-IVB Inertial Attitude: %s", PITCHDWN, INERTIAL);
	}
	break;
	case 66: //CSM AND TARGET STATE VECTOR UPDATE SR TIME TAGS AND CSM NAV CHECK PAD (40:10:00)
	case 67: //CSM AND TARGET STATE VECTOR UPDATE SR TIME TAGS AND CSM NAV CHECK PAD (46:05:00)
	case 68: //CSM AND TARGET STATE VECTOR UPDATE SR TIME TAGS AND CSM NAV CHECK PAD (52:00:00)
	{
		AP7NAV *form = (AP7NAV *)pad;

		VehicleDataBlock sv_A, sv_P, sv_A1, sv_P1;
		double SR_guess, GET_SR_A, GMT_SR_A, GET_SR_P, GMT_SR_P, NavGET;
		char buffer1[1000];
		char buffer2[1000];

		//Get state vectors
		sv_A = StateVectorCalcDataBlock(calcParams.src);
		sv_P = StateVectorCalcDataBlock(calcParams.tgt, PZMPTLEM.ConfigurationArea, PZMPTLEM.KFactor);
		if (fcn == 66)
		{
			SR_guess = OrbMech::HHMMSSToSS(40, 10, 0);
		}
		else if (fcn == 67)
		{
			SR_guess = OrbMech::HHMMSSToSS(46, 05, 0);
		}
		else
		{
			SR_guess = OrbMech::HHMMSSToSS(52, 0, 0);
		}

		GET_SR_A = mcc->mcc_calcs.FindOrbitalSunrise(sv_A, SR_guess);
		GET_SR_P = mcc->mcc_calcs.FindOrbitalSunrise(sv_P, SR_guess);

		GMT_SR_A = GMTfromGET(GET_SR_A);
		GMT_SR_P = GMTfromGET(GET_SR_P);

		NavGET = GET_SR_A - 30.0 * 60.0;
		NavCheckPAD(sv_A, *form, NavGET);

		//Propagate to time tags (SR)
		sv_A1 = coast(sv_A, GMT_SR_A - sv_A.sv.GMT);
		sv_P1 = coast(sv_P, GMT_SR_P - sv_P.sv.GMT);

		AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv_A1.sv);	//time tagged to tracking sunrise
		AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv_P1.sv);	//time tagged to tracking sunrise

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM and S-IVB state vectors");
		}
	}
	break;
	case 69: //SCT STAR COUNT PAD (53h30m)
	case 70: //SCT STAR COUNT PAD (98h)
	case 71: //SXT STAR COUNT PAD (147h)
	case 72: //SCT STAR COUNT PAD (147h)
	{
		VehicleDataBlock sv_A;
		double SR_guess;

		AP7STRCNTPAD *form = (AP7STRCNTPAD *)pad;

		//Get state vector
		sv_A = StateVectorCalcDataBlock(calcParams.src);

		if (fcn == 69)
		{
			form->type = 0; //SCT
			SR_guess = OrbMech::HHMMSSToSS(53, 30, 0);
			sprintf(form->Mode, "A");
			form->TAlign = mcc->mcc_calcs.FindOrbitalSunset(sv_A, SR_guess - 3600.0);
			form->AttSR = _V(4.0, 92.0, 359.0); //TBD compute attitude
			form->AttSS_12 = _V(184.0, 97.0, 359.0); //TBD compute attitude
		}
		else if (fcn == 70)
		{
			form->type = 0; //SCT
			SR_guess = OrbMech::HHMMSSToSS(98, 0, 0);
			sprintf(form->Mode, "B");
			form->TAlign = mcc->mcc_calcs.FindOrbitalSunrise(sv_A, SR_guess);
			form->AttSR = _V(0.0, 97.0, 0.0); //TBD compute attitude
			form->AttSS_12 = _V(0.0, 327.0, 0.0); //TBD compute attitude
		}
		else if (fcn == 71)
		{
			form->type = 1; //SXT
			form->TAlign = OrbMech::HHMMSSToSS(147, 31, 0); //Use this form entry for start time
			form->AttSR = _V(352.0, 41.0, 6.0); //Use this form entry for attitude
			form->Star1 = 23;
			form->Star2 = 31;
		}
		else
		{
			form->type = 0; //SCT
			SR_guess = OrbMech::HHMMSSToSS(147, 0, 0);
			sprintf(form->Mode, "A");
			form->TAlign = mcc->mcc_calcs.FindOrbitalSunset(sv_A, SR_guess - 3600.0);
			form->AttSR = _V(4.0, 92.0, 359.0);
			form->AttSS_12 = _V(184.0, 97.0, 359.0);
		}

		form->GETSR = mcc->mcc_calcs.FindOrbitalSunrise(sv_A, SR_guess);
		form->GETSS_12 = (mcc->mcc_calcs.FindOrbitalSunset(sv_A, SR_guess + 3600.0)) - 12.0 * 60.0;
	}
	break;
	case 80: //NAV CHECK, CSM STATE VECTOR UPDATE BOTH SLOTS (TIME TAG TCA)
	case 81: //WSMR PAD 1
	case 82: //WSMR PAD 2
	{
		VehicleDataBlock sv_A, sv_A1;
		double GMTtimetag, TCA, AOS, NavGET;
		char buffer1[1000];
		char buffer2[1000];

		AP11LMARKTRKPAD WSMRtempPAD;
		LMARKTRKPADOpt landmarkoptWSMR;

		//Get state vector
		sv_A = StateVectorCalcDataBlock(calcParams.src);

		landmarkoptWSMR.entries = 2;
		landmarkoptWSMR.Elevation = 90.0*RAD;

		//WSMR
		landmarkoptWSMR.lat[0] = 32.35637*RAD;
		landmarkoptWSMR.lng[0] = -106.37826*RAD;
		landmarkoptWSMR.alt[0] = 0.666*1852.0;

		//RR
		landmarkoptWSMR.lat[1] = 32.433333*RAD;
		landmarkoptWSMR.lng[1] = -106.366667*RAD;
		landmarkoptWSMR.alt[1] = 0.666*1852.0;

		if (fcn != 82)
		{
			landmarkoptWSMR.sv0 = sv_A.sv;
			landmarkoptWSMR.LmkTime[0] = OrbMech::HHMMSSToSS(71, 0, 0);
			landmarkoptWSMR.LmkTime[1] = OrbMech::HHMMSSToSS(71, 0, 0);
		}
		else
		{
			if (DeltaV_LVLH == _V(0, 0, 0))
			{
				landmarkoptWSMR.sv0 = sv_A.sv;
			}
			else
			{
				VehicleDataBlock sv2 = ExecuteManeuver(sv_A, TimeofIgnition, DeltaV_LVLH, 0, RTCC_ENGINETYPE_CSMSPS);
				landmarkoptWSMR.sv0 = sv2.sv;
			}

			landmarkoptWSMR.LmkTime[0] = OrbMech::HHMMSSToSS(76, 0, 0);
			landmarkoptWSMR.LmkTime[1] = OrbMech::HHMMSSToSS(76, 0, 0);
		}

		LandmarkTrackingPAD(landmarkoptWSMR, WSMRtempPAD);

		AOS = WSMRtempPAD.T1[1];
		TCA = WSMRtempPAD.T2[0];
		GMTtimetag = GMTfromGET(TCA); // Time tag to TCA

		if (fcn == 80)
		{
			AP7NAV *form = (AP7NAV *)pad;

			NavGET = TCA - 30.0 * 60.0; //Nav Check GET

			//Use SV for Nav Check
			NavCheckPAD(sv_A, *form, NavGET);

			//Time tagged SV
			sv_A1 = coast(sv_A, GMTtimetag - sv_A.sv.GMT);

			AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, sv_A1.sv);
			AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, sv_A1.sv);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vectors");
			}
		}
		else if (fcn == 81)
		{
			AP7WSMRPAD  *form = (AP7WSMRPAD *)pad;

			form->TAlign = TCA - (45.0 * 60.0);
			form->GETAOS = AOS;
			form->GETRR = TCA;
			form->AttAOS = _V(159.0, 55.0, 17.0); // TBD compute attitude
		}
		else
		{
			AP7WSMRPAD  *form = (AP7WSMRPAD *)pad;

			form->GETAOS = AOS;
			form->GETRR = TCA;
			form->AttAOS = _V(349.3, 305.8, 60.8); // TBD compute attitude
		}
	}
	break;
	case 95: //P23 PAD (122:10)
	case 96: //P23 PAD (146:20)
	{
		VehicleDataBlock sv_A;
		double GET_guess_1, GET_guess_2;

		AP7P23PAD *form = (AP7P23PAD *)pad;

		sv_A = StateVectorCalcDataBlock(calcParams.src);

		if (fcn == 95)
		{
			form->entries = 2;
			GET_guess_1 = OrbMech::HHMMSSToSS(123, 52, 0); //TBD compute star rise time
			GET_guess_2 = OrbMech::HHMMSSToSS(124, 4, 0); //TBD compute star rise time
			form->GET[0] = GET_guess_1;
			form->GET[1] = GET_guess_2;
			form->Star[0] = 37;
			form->Star[1] = 45;
			form->Att[0] = _V(0.0, 356.0, 1.0); //TBD compute attitude
			form->Att[1] = _V(1.0, 306.0, 1.0); //TBD compute attitude
			form->Shaft[0] = 19.0; //TBD compute shaft angles
			form->Shaft[1] = 355.0; //TBD compute shaft angles
			form->Trun[0] = 18.0; //TBD compute trunnion angles
			form->Trun[1] = 14.0; //TBD compute trunnion angles
		}
		else if (fcn == 96)
		{
			form->entries = 1;
			GET_guess_1 = OrbMech::HHMMSSToSS(147, 0, 0); //TBD compute star rise time
			form->GET[0] = GET_guess_1;
			form->Att[0] = _V(347.0, 97.0, 11.0); //TBD compute attitude
			form->Star[0] = 21;
			form->Ldmk[0] = 5;
		}
	}
	break;
	case 105: //PTC PAD (167:00 Guess)
	case 106: //PTC PAD (212:00 Guess)
	{
		VehicleDataBlock sv, sv_1;
		double GMT_guess, GETAlign;
		REFSMMATOpt refsopt;
		MATRIX3 REFSMMAT;

		AP7PTCPAD *form = (AP7PTCPAD *)pad;

		mcc->mcc_calcs.RestoreStateVector(sv);

		//Compute T0+26 as time crossing 200nmi
		if (fcn == 105)
		{
			GMT_guess = GMTfromGET(OrbMech::HHMMSSToSS(167, 0, 0)); //Initial guess time
		}
		else
		{
			GMT_guess = GMTfromGET(OrbMech::HHMMSSToSS(212, 0, 0)); //Initial guess time
		}

		sv_1 = coast(sv, GMT_guess - sv.sv.GMT); //Time tag to find altitude from initial guess

		//Compute GET crossing 200 nmi altitude
		int stop;
		EphemerisData sv_stop;
		PMMCEN(sv_1.sv, 0.0, (60.0 * 60.0), 3, ((OrbMech::R_Earth)+(200.0 * 1852)), 1.0, sv_stop, stop);
		GETAlign = GETfromGMT(sv_stop.GMT);

		//Compute and store REFSMMAT 
		refsopt.REFSMMATTime = GETAlign;
		refsopt.REFSMMATopt = 2;
		refsopt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&refsopt);

		calcParams.StoredREFSMMAT = REFSMMAT;

		form->type = 0;
		form->GET = GETAlign - (26.0 * 60.0);
		form->TAlign = GETAlign;
		form->Att = _V(0.0, 0.0, 0.0);
	}
	break;
	case 109: //SPS COLD SOAK PAD  (167:40 Guess)
	{
		VehicleDataBlock sv, sv_1;
		double SR_guess, GET;
		MATRIX3 REFSMMAT;

		AP7PTCPAD *form = (AP7PTCPAD *)pad;

		sv = StateVectorCalcDataBlock(calcParams.src);

		REFSMMAT = calcParams.StoredREFSMMAT;

		SR_guess = OrbMech::HHMMSSToSS(167, 40, 0); //Initial guess time

		GET = mcc->mcc_calcs.FindOrbitalSunrise(sv, SR_guess);

		sv_1 = coast(sv, GMTfromGET(GET) - sv.sv.GMT); //Time tag to sunrise

		form->type = 1;
		form->GET = GET;
		form->Att = _V(4.0, 183.0, 20.0); //TBD compute attitude to place +X to sun
	}
	break;
	case 110: //DAP UPDATE
	{
		AP10DAPDATA * form = (AP10DAPDATA *)pad;

		CSMDAPUpdate(calcParams.src, *form, false);
	}
	break;
	}

	return scrubbed;
}