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
#include "saturnv.h"
#include "iu.h"
#include "LVDC.h"
#include "LEM.h"
#include "../src_rtccmfd/OrbMech.h"
#include "mcc.h"
#include "rtcc.h"

bool RTCC::CalculationMTP_D(int fcn, LPVOID &pad, char * upString, char * upDesc, char * upMessage)
{
	char uplinkdata[1024 * 3];
	bool preliminary = true;
	bool scrubbed = false;

	switch (fcn) {
	case 1: //GENERIC CSM STATE VECTOR UPDATE
	{
		EphemerisData sv;
		char buffer1[1000];

		sv = StateVectorCalcEphem(calcParams.src); //State vector for uplink
		AGCStateVectorUpdate(buffer1, 1, 1, sv);

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
		char buffer1[1000];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		NavCheckPAD(sv, *form);
		AGCStateVectorUpdate(buffer1, sv, true);

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
		char buffer1[1000];
		char buffer2[1000];

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);

		AGCStateVectorUpdate(buffer1, sv_CSM, true);
		AGCStateVectorUpdate(buffer2, sv_LM, false);

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
		char buffer1[1000];

		sv = StateVectorCalc(calcParams.tgt); //State vector for uplink

		AGCStateVectorUpdate(buffer1, sv, false);

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

		CSMDAPUpdate(calcParams.src, *form, false);
	}
	break;
	case 6: //LM DAP DATA
	{
		AP10DAPDATA * form = (AP10DAPDATA *)pad;

		LMDAPUpdate(calcParams.tgt, *form, false);
	}
	break;
	case 8: //LM DAP DATA (DOCKED)
	{
		AP10DAPDATA * form = (AP10DAPDATA *)pad;

		LMDAPUpdate(calcParams.tgt, *form, true);
	}
	break;
	case 7: //MISSION INITIALIZATION
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

		//P15: CMC, LGC and AGS clock zero
		sprintf_s(Buff, "P15,AGC,%d:%d:%.2lf;", hh, mm, ss);
		GMGMED(Buff);
		sprintf_s(Buff, "P15,LGC,%d:%d:%.2lf;", hh, mm, ss);
		GMGMED(Buff);
		GMGMED("P15,AGS,,40:00:00;");

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
		med_m55.ConfigCode = "CL";
		PMMWTC(55);

		med_m55.Table = RTCC_MPT_LM;
		med_m55.ConfigCode = "L";
		PMMWTC(55);

		med_m51.Table = RTCC_MPT_CSM;
		med_m51.CSMArea = 129.4*0.3048*0.3048;
		med_m51.LMAscentArea = med_m51.LMDescentArea = 129.4*0.3048*0.3048;
		med_m51.SIVBArea = 0.0;
		med_m51.KFactor = 1.0;
		PMMWTC(51);

		med_m51.Table = RTCC_MPT_LM;
		med_m51.LMAscentArea = med_m51.LMDescentArea = 129.4*0.3048*0.3048;
		med_m51.SIVBArea = med_m51.CSMArea = 0.0;
		med_m51.KFactor = 1.0;
		PMMWTC(51);

		med_m50.Table = RTCC_MPT_CSM;
		med_m50.CSMWT = 58962.0* 0.453592;
		med_m50.LMWT = 30029.0* 0.453592;
		med_m50.LMASCWT = 10106.0* 0.453592;
		med_m50.SIVBWT = 0.0;
		PMMWTC(50);

		med_m50.Table = RTCC_MPT_LM;
		med_m50.CSMWT = med_m50.SIVBWT = 0.0;
		PMMWTC(50);

		//And propellants
		med_m49.Table = RTCC_MPT_CSM;
		med_m49.SPSFuelRemaining = 35970.0* 0.453592;
		med_m49.CSMRCSFuelRemaining = 1362.4* 0.453592;
		med_m49.LMAPSFuelRemaining = 4136.0* 0.453592;
		med_m49.LMDPSFuelRemaining = 17944.0* 0.453592;
		med_m49.LMRCSFuelRemaining = 633.0* 0.453592;
		med_m49.SIVBFuelRemaining = 0.0;
		PMMWTC(49);

		med_m49.Table = RTCC_MPT_LM;
		med_m49.SPSFuelRemaining = 0.0;
		med_m49.CSMRCSFuelRemaining = 0.0;
		med_m49.SIVBFuelRemaining = 0.0;
		PMMWTC(49);
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
		double P30TIG;
		VECTOR3 dV_LVLH;
		EphemerisData sv0;
		char buffer1[1000];

		sv0 = StateVectorCalcEphem(calcParams.src); //State vector for uplink

		med_m50.CSMWT = calcParams.src->GetMass();
		med_m50.LMWT = calcParams.tgt->GetMass();

		P30TIG = OrbMech::HHMMSSToSS(5, 59, 0);
		dV_LVLH = _V(36.8, 0.0, 0.0)*0.3048;

		opt.TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = true;
		opt.sxtstardtime = -30.0*60.0;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		opt.navcheckGET = 0;
		opt.sv0 = sv0;
		opt.CSMMass = med_m50.CSMWT;
		opt.LMMass = med_m50.LMWT;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "SPS-1");
		sprintf(form->remarks, "Gimbal angles with pad REFSMMAT");

		CMCExternalDeltaVUpdate(buffer1, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Target load");
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
	case 13: //SPS-2 CALCULATION
	case 14: //SPS-3 CALCULATION
	case 15: //SPS-4 CALCULATION
	case 23: //SPS-5 CALCULATION
	{
		GENERICPAD * form = NULL;

		if (fcn == 14 || fcn == 15)
		{
			form = (GENERICPAD *)pad;
		}

		PMMMPTInput in;
		REFSMMATOpt refsopt;
		GMPOpt gmpopt;
		double TIG_imp;
		VECTOR3 dV_imp;
		MATRIX3 REFSMMAT, CurrentREFSMMAT;
		EphemerisData sv0;

		//Get data
		sv0 = StateVectorCalcEphem(calcParams.src);
		med_m50.CSMWT = calcParams.src->GetMass();
		med_m50.LMWT = calcParams.tgt->GetMass();
		CurrentREFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);

		gmpopt.sv_in = sv0;
		gmpopt.Area = PZMPTCSM.ConfigurationArea;
		gmpopt.Weight = med_m50.CSMWT + med_m50.LMWT;
		gmpopt.KFactor = PZMPTCSM.KFactor;

		if (fcn == 13)
		{
			gmpopt.dH_D = 64.0*1852.0;	//Increase in HA, from SCOT 69-FM-17
			gmpopt.dLAN = 3.0263*RAD; //From trajectory parameters table in mission report
			gmpopt.long_D = -60.28*RAD; //Midpoint of burn in mission report
			gmpopt.ManeuverCode = RTCC_GMP_HNL;
			gmpopt.TIG_GET = OrbMech::HHMMSSToSS(21.0, 40.0, 0.0);

			in.DETU = 0.0;
		}
		else if (fcn == 14)
		{
			gmpopt.dH_D = 78.0*1852.0;
			gmpopt.dLAN = 10.2*RAD;		//From trajectory parameters table in mission report
			gmpopt.long_D = -70.0*RAD; //Midpoint of burn in mission report
			gmpopt.ManeuverCode = RTCC_GMP_HNL;
			gmpopt.TIG_GET = OrbMech::HHMMSSToSS(24.0, 50.0, 0.0);

			in.DETU = 0.0;
		}
		else if (fcn == 15)
		{
			gmpopt.dLAN = 1.2*RAD;	//From trajectory parameters table in mission report
			gmpopt.ManeuverCode = RTCC_GMP_NSO;	//Maneuver was at highest possible latitude, so probably the optimum node shift option
			gmpopt.TIG_GET = OrbMech::HHMMSSToSS(28, 0, 0);

			in.DETU = 18.0;
		}
		else
		{
			gmpopt.H_D = 133.0*1852.0;	//Mission report says 130, referenced to ellipsoid? 133 might be relative to pad radius, which we have in Orbiter
			gmpopt.ManeuverCode = RTCC_GMP_CPH;
			gmpopt.dW = 0.8*RAD;	//To give more than 40 seconds burn time
			gmpopt.TIG_GET = OrbMech::HHMMSSToSS(54, 10, 0); //Using 54:00:00 was finding the wrong solution

			in.DETU = 18.0;
		}

		GeneralManeuverProcessor(&gmpopt, dV_imp, TIG_imp);

		in.CONFIG = 13; //CSM+LM
		in.CSMWeight = med_m50.CSMWT;
		in.LMWeight = med_m50.LMWT;
		in.sv_before = PZGPMELM.SV_before;
		in.V_aft = PZGPMELM.V_after;
		in.UT = false; //2 jets
		in.IgnitionTimeOption = false;
		in.IterationFlag = true;
		in.Thruster = RTCC_ENGINETYPE_CSMSPS;
		in.VC = RTCC_MANVEHICLE_CSM;
		in.VehicleArea = PZMPTCSM.ConfigurationArea;

		double GMT_TIG;
		PoweredFlightProcessor(in, GMT_TIG, DeltaV_LVLH);
		TimeofIgnition = GETfromGMT(GMT_TIG);

		refsopt.csmlmdocked = true;
		refsopt.dV_LVLH = DeltaV_LVLH;
		refsopt.REFSMMATTime = TimeofIgnition;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;
		refsopt.vesseltype = 1;

		REFSMMAT = REFSMMATCalc(&refsopt);

		//Store
		EMGSTSTM(RTCC_MPT_CSM, REFSMMAT, RTCC_REFSMMAT_TYPE_CUR, GMTfromGET(TimeofIgnition));

		if (fcn == 14 || fcn == 15)
		{
			VECTOR3 GimbalAngles = OrbMech::GimbalAngleConversion(CurrentREFSMMAT, _V(0, 0, 0), REFSMMAT, true);
			sprintf(form->paddata, "Attitude for next burn: %03.0lf %03.0lf %03.0lf", OrbMech::imulimit(GimbalAngles.x*DEG), OrbMech::imulimit(GimbalAngles.y*DEG), OrbMech::imulimit(GimbalAngles.z*DEG));
		}
	}
	break;
	case 100: //SPS-2 MANEUVER PAD
	case 101: //SPS-3 MANEUVER PAD
	case 102: //SPS-4 MANEUVER PAD
	case 103: //SPS-5 MANEUVER PAD
	{
		AP7MNV * form = (AP7MNV *)pad;

		AP7ManPADOpt opt;
		EphemerisData sv0;
		char buffer1[1000];
		char buffer2[1000];

		sv0 = StateVectorCalcEphem(calcParams.src);
		med_m50.CSMWT = calcParams.src->GetMass();
		med_m50.LMWT = calcParams.tgt->GetMass();

		opt.dV_LVLH = DeltaV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = true;
		opt.navcheckGET = TimeofIgnition - 30.0*60.0;
		opt.REFSMMAT = EZJGMTX1.data[0].REFSMMAT;
		opt.TIG = TimeofIgnition;
		opt.sv0 = sv0;
		opt.CSMMass = med_m50.CSMWT;
		opt.LMMass = med_m50.LMWT;

		if (fcn == 100)
		{
			opt.UllageDT = 0.0;
			opt.sxtstardtime = -30.0*60.0;
		}
		else if (fcn == 101)
		{
			opt.UllageDT = 0.0;
			opt.sxtstardtime = -30.0*60.0;
		}
		else if (fcn == 102)
		{
			opt.UllageDT = 18.0;
			opt.sxtstardtime = -30.0*60.0;
		}
		else
		{
			opt.UllageDT = 18.0;
			opt.sxtstardtime = -50.0*60.0;
		}

		AP7ManeuverPAD(&opt, *form);

		if (fcn == 100)
		{
			sprintf(form->purpose, "SPS-2");
			//Bias pitch trim gimbal angle by 0.5° to induce transient at ignition. Gets mentioned in pre-mission documents, but wasn't actually done during the mission?!
			form->pTrim += 0.5;
		}
		else if (fcn == 101)
		{
			sprintf(form->purpose, "SPS-3");
		}
		else if (fcn == 102)
		{
			sprintf(form->purpose, "SPS-4");
		}
		else
		{
			AP10DAPDATA dappad;

			sprintf(form->purpose, "SPS-5");

			CSMDAPUpdate(calcParams.src, dappad, false);
			sprintf(form->remarks, "LM weight is %.0f", dappad.OtherVehicleWeight);
		}

		AGCStateVectorUpdate(buffer1, 1, 1, sv0, true);
		CMCExternalDeltaVUpdate(buffer2, TimeofIgnition, DeltaV_LVLH);

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
	{
		PMMMPTInput in;
		GMPOpt gmpopt;
		REFSMMATOpt refsopt;
		MATRIX3 REFSMMAT;
		EphemerisData sv0;
		VECTOR3 dV_imp;
		double TIG_imp;

		sv0 = StateVectorCalcEphem(calcParams.src); //State vector for uplink
		med_m50.CSMWT = calcParams.src->GetMass();
		med_m50.LMWT = calcParams.tgt->GetMass();

		gmpopt.dLAN = 6.9*RAD;
		gmpopt.ManeuverCode = RTCC_GMP_NSO; //Gives TIG at northernmost latitude
		gmpopt.sv_in = sv0;
		gmpopt.TIG_GET = OrbMech::HHMMSSToSS(49, 10, 0);
		gmpopt.Area = PZMPTCSM.ConfigurationArea;
		gmpopt.Weight = med_m50.CSMWT + med_m50.LMWT;
		gmpopt.KFactor = PZMPTCSM.KFactor;

		GeneralManeuverProcessor(&gmpopt, dV_imp, TIG_imp);

		in.CONFIG = 13; //CSM+LM
		in.CSMWeight = med_m50.CSMWT;
		in.LMWeight = med_m50.LMWT;
		in.sv_before = PZGPMELM.SV_before;
		in.V_aft = PZGPMELM.V_after;
		in.UT = false; //2 jets
		in.DETU = 10.0;
		in.IgnitionTimeOption = false;
		in.IterationFlag = true;
		in.Thruster = RTCC_ENGINETYPE_LMDPS;
		in.VC = RTCC_MANVEHICLE_LM;
		in.VehicleArea = PZMPTCSM.ConfigurationArea;
		in.DPSScaleFactor = 0.925; //Should this be lowered to account for the throttle program at the end of the burn?
		in.DT_10PCT = 19.0; //Roughly gives the same as 5 sec at 10% and 21 sec at 40%

		double GMT_TIG;
		PoweredFlightProcessor(in, GMT_TIG, DeltaV_LVLH);
		TimeofIgnition = GETfromGMT(GMT_TIG);

		//Add 10.4 ft/s for the 4 second early cutoff
		DeltaV_LVLH = DeltaV_LVLH + unit(DeltaV_LVLH)*10.4*0.3048;

		refsopt.REFSMMATTime = TimeofIgnition;
		refsopt.dV_LVLH = DeltaV_LVLH;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.tgt;
		refsopt.vesseltype = 3;
		refsopt.csmlmdocked = true;
		refsopt.HeadsUp = false;

		REFSMMAT = REFSMMATCalc(&refsopt);
		//Store for both CSM and LM
		EMGSTSTM(RTCC_MPT_CSM, REFSMMAT, RTCC_REFSMMAT_TYPE_CUR, GMTfromGET(TimeofIgnition));
		EMGSTSTM(RTCC_MPT_LM, REFSMMAT, RTCC_REFSMMAT_TYPE_CUR, GMTfromGET(TimeofIgnition));

		char buffer1[1000];
		char buffer2[1000];

		AGCStateVectorUpdate(buffer1, 1, 1, sv0, true);
		AGCDesiredREFSMMATUpdate(buffer2, REFSMMAT);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, Verb 66, Desired REFSMMAT");
		}
	}
	break;
	case 19: //LM AOT STAR OBSERVATION PAD
	{
		AP9AOTSTARPAD * form = (AP9AOTSTARPAD *)pad;

		//form->CSMAtt = _V(79.0, 358.0, 309.0);
		form->Detent = 2;
		form->GET = OrbMech::HHMMSSToSS(43, 55, 0);
		form->Star = 015;

		MATRIX3 REFSMMAT = EZJGMTX1.data[0].REFSMMAT;
		EphemerisData sv = StateVectorCalcEphem(calcParams.src);

		VECTOR3 GA = PointAOTWithCSM(REFSMMAT, sv, form->Detent, form->Star, 0.0);
		form->CSMAtt = GA * DEG;
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
	case 21: //DOCKED DPS BURN - REFSMMAT AND SV FOR LGC
	{
		AP11LMMNV * form = (AP11LMMNV *)pad;

		AP10DAPDATA dappad;
		AP11LMManPADOpt manopt;
		EphemerisData sv0;
		char buffer1[1000];
		char buffer2[1000];

		sv0 = StateVectorCalcEphem(calcParams.src); //State vector for uplink

		manopt.csmlmdocked = true;
		manopt.dV_LVLH = DeltaV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_LMDPS;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = EZJGMTX3.data[0].REFSMMAT;
		manopt.TIG = TimeofIgnition;
		manopt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&manopt, *form);
		LMDAPUpdate(calcParams.tgt, dappad, true);

		sprintf(form->purpose, "Docked DPS");

		AGCStateVectorUpdate(buffer1, 2, 3, sv0, true);
		AGCREFSMMATUpdate(buffer2, EZJGMTX3.data[0].REFSMMAT, false);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector, V66, REFSMMAT");
		}
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
		EphemerisData sv;
		char buffer1[1000];
		char buffer2[1000];

		sv = StateVectorCalcEphem(calcParams.src);
		E = { 1,0,0, 0,1,0, 0,0,1 };

		GA = HatchOpenThermalControl(sv.GMT, E);

		opt.IMUAngles = GA;
		opt.PresentREFSMMAT = E;
		opt.REFSMMATopt = 7;
		REFSMMAT = REFSMMATCalc(&opt);

		AGCStateVectorUpdate(buffer1, 1, 1, sv, true);
		AGCDesiredREFSMMATUpdate(buffer2, REFSMMAT);

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

		REFSMMATOpt opt;
		MATRIX3 REFSMMAT, A;
		EphemerisData sv0, sv1;
		double t_TPI0;
		sv0 = StateVectorCalcEphem(calcParams.src);

		//Coast to 90h GET (desired is Sep-12 min, but CMC software limitations prevent that)
		med_m50.CSMWT = calcParams.src->GetMass();
		med_m50.LMWT = calcParams.tgt->GetMass();

		PZMPTLEM.ConfigurationArea = 200.57*0.3048*0.3048;

		//Coast to uplink state vector time with total weight and LM area
		sv1 = coast(sv0, GMTfromGET(90.0*3600.0) - sv0.GMT, med_m50.CSMWT + med_m50.LMWT, PZMPTLEM.ConfigurationArea, 1.0, false);

		DMissionRendezvousPlan(ConvertEphemDatatoSV(sv1), t_TPI0);

		//Calculate LM REFSMMAT
		opt.REFSMMATopt = 2;
		opt.REFSMMATTime = calcParams.TPI;
		opt.vessel = calcParams.tgt;
		opt.vesseltype = 3;

		REFSMMAT = REFSMMATCalc(&opt);

		//Store
		EMGSTSTM(RTCC_MPT_LM, REFSMMAT, RTCC_REFSMMAT_TYPE_CUR, opt.REFSMMATTime);

		//Convert LM REFSMMAT to CSM REFSMMAT
		A = REFSMMAT;
		REFSMMAT = _M(A.m31, A.m32, A.m33, A.m21, A.m22, A.m23, -A.m11, -A.m12, -A.m13);

		//Store
		EMGSTSTM(RTCC_MPT_CSM, REFSMMAT, RTCC_REFSMMAT_TYPE_CUR, opt.REFSMMATTime);

		char buffer1[1000];
		char buffer2[1000];

		AGCStateVectorUpdate(buffer1, 1, 1, sv1, true);
		AGCDesiredREFSMMATUpdate(buffer2, EZJGMTX1.data[0].REFSMMAT);

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
		EphemerisData sv0, sv_uplink;

		AP7NAV * form = (AP7NAV *)pad;

		sv0 = StateVectorCalcEphem(calcParams.src);
		med_m50.CSMWT = calcParams.src->GetMass();
		med_m50.LMWT = calcParams.tgt->GetMass();
		REFSMMAT = EZJGMTX3.data[0].REFSMMAT;

		//Generate state vector for uplink
		sv_uplink = coast(sv0, GMTfromGET(92.0*3600.0) - sv0.GMT, med_m50.CSMWT + med_m50.LMWT, PZMPTLEM.ConfigurationArea, 1.0, false);

		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		AGCStateVectorUpdate(buffer1, 2, RTCC_MPT_CSM, sv_uplink);
		AGCStateVectorUpdate(buffer2, 2, RTCC_MPT_LM, sv_uplink);
		AGCREFSMMATUpdate(buffer3, REFSMMAT, false);

		NavCheckPAD(ConvertEphemDatatoSV(sv_uplink), *form, 92.0*3600.0);

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

		PMMMPTInput in;
		AP11LMManPADOpt opt;
		EphemerisData sv, sv1;
		MATRIX3 Q_Xx;
		VECTOR3 dV_LVLH, DV;
		double eps, a, n0, n1, n2, dt1, dt2, dh1, dh2, dphase, dv, dphase_bias, t_TPI0, P30TIG, t_Sep;

		//about 10NM less phasing in the coelliptic orbit above
		dphase_bias = 0.0027987178;

		sv = StateVectorCalcEphem(calcParams.src);
		med_m50.LMWT = calcParams.tgt->GetMass();

		DMissionRendezvousPlan(ConvertEphemDatatoSV(sv), t_TPI0);
		//Store the TPI0 time here, the nominal TPI time is already stored in calcParams.TPI
		TimeofIgnition = t_TPI0;

		eps = length(sv.V)*length(sv.V) / 2.0 - OrbMech::mu_Earth / length(sv.R);
		a = -OrbMech::mu_Earth / (2.0*eps);
		n0 = sqrt(OrbMech::mu_Earth / pow(a, 3));

		dt1 = calcParams.CSI - calcParams.Insertion;
		dt2 = calcParams.TPI - calcParams.CDH;

		dh2 = 10.0*1852.0;
		n2 = sqrt(OrbMech::mu_Earth / pow((a - dh2), 3));
		dphase = (n2 - n0)*dt2 - dphase_bias;

		n1 = -dphase / dt1 + n0;
		dh1 = OrbMech::power(OrbMech::mu_Earth / (n1*n1), 1.0 / 3.0) - a;
		dv = dh1 * n0;

		sv1 = coast(sv, GMTfromGET(calcParams.Phasing) - sv.GMT);
		Q_Xx = OrbMech::LVLH_Matrix(sv1.R, sv1.V);
		DV = tmul(Q_Xx, _V(0, 0, -dv));

		in.CONFIG = 12; //LM
		in.CSMWeight = 0.0;
		in.DETU = 8.0;
		in.DPSScaleFactor = 0.4;
		in.DT_10PCT = 15.0;
		in.HeadsUpIndicator = false;
		in.IgnitionTimeOption = false;
		in.IterationFlag = false; //Could be true
		in.LMWeight = med_m50.LMWT;
		in.sv_before = sv1;
		in.Thruster = RTCC_ENGINETYPE_LMDPS;
		in.UT = false; //2 Jets
		in.VC = RTCC_MANVEHICLE_LM;
		in.VehicleArea = PZMPTLEM.ConfigurationArea;
		in.VehicleWeight = med_m50.LMWT;
		in.V_aft = sv1.V + DV;

		double GMT_TIG;
		PoweredFlightProcessor(in, GMT_TIG, dV_LVLH);
		P30TIG = GETfromGMT(GMT_TIG);

		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_LMDPS;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
		opt.TIG = P30TIG;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);

		t_Sep = calcParams.Phasing - 44.5*60.0;

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

		EphemerisData sv_A, sv_P;
		TwoImpulseOpt opt;
		TwoImpulseResuls res;
		AP9LMTPIPADOpt manopt;

		sv_P = StateVectorCalcEphem(calcParams.src);
		sv_A = StateVectorCalcEphem(calcParams.tgt);

		opt.mode = 5;
		opt.T1 = GMTfromGET(TimeofIgnition);
		opt.T2 = -1.0;
		opt.ChaserVehicle = RTCC_MPT_LM;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.WT = 130.0*RAD;

		PMSTICN(opt, res);

		manopt.dV_LVLH = res.dV_LVLH;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
		manopt.sv_A = sv_A;
		manopt.sv_P = sv_P;
		manopt.GMT_TIG = opt.T1;

		AP9LMTPIPAD(&manopt, *form);
	}
	break;
	case 34: //INSERTION MANEUVER
	{
		AP11LMMNV * form = (AP11LMMNV *)pad;

		SPQOpt opt;
		SPQResults res;
		AP11LMManPADOpt manopt;
		SV sv_A, sv_P;
		VECTOR3 dV_LVLH;
		double TIG, P30TIG;

		sv_A = StateVectorCalc(calcParams.tgt);
		sv_P = StateVectorCalc(calcParams.src);

		TIG = calcParams.Insertion;

		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.t_CSI = -1;
		opt.t_CDH = TIG;

		ConcentricRendezvousProcessor(opt, res);
		PoweredFlightProcessor(sv_A, TIG, RTCC_ENGINETYPE_LMDPS, 0.0, res.dV_CDH, true, P30TIG, dV_LVLH);

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_LMDPS;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
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
		SPQResults res;
		SV sv_A, sv_P, sv_CSI;
		MATRIX3 Q_Xx;
		VECTOR3 dV_LVLH;

		sv_A = StateVectorCalc(calcParams.tgt);
		sv_P = StateVectorCalc(calcParams.src);

		opt.E = 27.5*RAD;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.K_CDH = 0;
		opt.t_CSI = calcParams.CSI;
		opt.t_TPI = calcParams.TPI;

		ConcentricRendezvousProcessor(opt, res);
		sv_CSI = coast(sv_A, opt.t_CSI - OrbMech::GETfromMJD(sv_A.MJD, CalcGETBase()));
		Q_Xx = OrbMech::LVLH_Matrix(sv_CSI.R, sv_CSI.V);
		dV_LVLH = res.dV_CSI;

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_LMAPS;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
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
		SPQResults res;
		SV sv_A, sv_P, sv_CDH;
		VECTOR3 dV_LVLH;
		double T_CDH;

		sv_A = StateVectorCalc(calcParams.tgt);
		sv_P = StateVectorCalc(calcParams.src);

		calcParams.CDH = calcParams.CSI + OrbMech::period(sv_A.R, sv_A.V, OrbMech::mu_Earth) / 2.0;

		//Get CDH TIG from LGC memory (EMEM3370 and 3371)
		T_CDH = (mcc->lm->agc.vagc.Erasable[6][0371] + mcc->lm->agc.vagc.Erasable[6][0370] * pow((double) 2., (double) 14.)) / 100.0;

		//Reasonability test
		if (abs(T_CDH - calcParams.CDH) < 5.0*60.0)
		{
			//Update with onboard value
			calcParams.CDH = T_CDH;
		}

		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.t_CSI = -1;
		opt.t_CDH = calcParams.CDH;

		ConcentricRendezvousProcessor(opt, res);
		sv_CDH = coast(sv_A, opt.t_CDH - OrbMech::GETfromMJD(sv_A.MJD, CalcGETBase()));
		dV_LVLH = res.dV_CDH;

		manopt.dV_LVLH = dV_LVLH;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
		manopt.sv_A = sv_A;
		manopt.TIG = calcParams.CDH;

		AP9LMCDHPAD(&manopt, *form);
	}
	break;
	case 37: //TPI MANEUVER
	{
		AP9LMTPI * form = (AP9LMTPI *)pad;

		EphemerisData sv_A, sv_P;
		TwoImpulseOpt opt;
		AP9LMTPIPADOpt manopt;
		TwoImpulseResuls res;

		sv_P = StateVectorCalcEphem(calcParams.src);
		sv_A = StateVectorCalcEphem(calcParams.tgt);

		opt.mode = 5;
		opt.T1 = -1.0;
		opt.T2 = -1.0;
		opt.ChaserVehicle = RTCC_MPT_LM;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.Elev = 27.5*RAD;
		opt.WT = 130.0*RAD;

		PMSTICN(opt, res);

		calcParams.TPI = res.T1;

		manopt.dV_LVLH = res.dV_LVLH;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
		manopt.sv_A = sv_A;
		manopt.sv_P = sv_P;
		manopt.GMT_TIG = res.sv_tig.GMT;

		AP9LMTPIPAD(&manopt, *form);
	}
	break;
	case 38: //LM DOCKED P52 PAD (STAR 15)
	case 39: //LM DOCKED P52 PAD (STAR 25)
	{
		AP9AOTSTARPAD * form = (AP9AOTSTARPAD *)pad;

		EphemerisData sv;
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

		REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		sv = StateVectorCalcEphem(calcParams.src);

		GA = PointAOTWithCSM(REFSMMAT, sv, form->Detent, form->Star, 0.0);
		form->CSMAtt = GA * DEG;
	}
	break;
	case 40: //LM BURN TO DEPLETION UPDATE
	{
		AP11LMMNV * form = (AP11LMMNV *)pad;

		AP10DAPDATA dappad;
		AP11LMManPADOpt opt;
		GMPOpt gmpopt;
		SV sv0;
		VECTOR3 dV_LVLH, dV_imp;
		double P30TIG, TIG_imp;
		char buffer1[1000];

		sv0 = StateVectorCalc(calcParams.tgt);

		gmpopt.dV = 7427.5*0.3048;
		gmpopt.long_D = -95.0*RAD;
		gmpopt.ManeuverCode = RTCC_GMP_FCL;
		gmpopt.Pitch = 0.0;
		gmpopt.sv_in = ConvertSVtoEphemData(sv0);
		gmpopt.TIG_GET = OrbMech::HHMMSSToSS(101, 30, 0);
		gmpopt.Yaw = -45.0*RAD;

		GeneralManeuverProcessor(&gmpopt, dV_imp, TIG_imp);
		PoweredFlightProcessor(sv0, TIG_imp, RTCC_ENGINETYPE_LMAPS, 0.0, dV_imp, false, P30TIG, dV_LVLH);

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_LMAPS;
		opt.REFSMMAT= GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
		opt.TIG = P30TIG;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);

		LMDAPUpdate(calcParams.tgt, dappad, false);
		sprintf(form->purpose, "APS Depletion");
		sprintf(form->remarks, "LM weight is %.0f", dappad.ThisVehicleWeight);

		AGCStateVectorUpdate(buffer1, sv0, false);

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

		opt.dV_LVLH = DeltaV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_LMAPS;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
		opt.TIG = TimeofIgnition;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, manpad);

		DockAlignOpt dockopt;

		dockopt.LM_REFSMMAT = EZJGMTX3.data[0].REFSMMAT;
		dockopt.CSM_REFSMMAT = EZJGMTX1.data[0].REFSMMAT;
		dockopt.LMAngles = manpad.IMUAtt;
		dockopt.type = 3;

		DockingAlignmentProcessor(dockopt);

		char buff1[64];
		OrbMech::format_time_HHMMSS(buff1, TimeofIgnition - 30.0*60.0);

		sprintf(form->paddata, "LM Jettison at %s. Roll %.1f, pitch %.1f, yaw %.1f", buff1, dockopt.CSMAngles.x*DEG, dockopt.CSMAngles.y*DEG, dockopt.CSMAngles.z*DEG);
	}
	break;
	case 42: //BLOCK DATA 11
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 4;
		double lng[] = { 144.6*RAD, 148.5*RAD, 144.6*RAD, 139.0*RAD};
		double GETI[] = { OrbMech::HHMMSSToSS(104,20,28),OrbMech::HHMMSSToSS(105,54,57),OrbMech::HHMMSSToSS(107,27,50),OrbMech::HHMMSSToSS(109,0,44) };
		std::string area[] = { "066-3A", "067-3B", "068-3A", "069-CC"};

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 75: //POST JETTISON SEPARATION MANEUVER
	{
		GENERICPAD * form = (GENERICPAD *)pad;

		char buff1[64];
		AP7ManPADOpt opt;
		AP7MNV manpad;
		EphemerisData sv0;

		//Change config to CSM only
		GMGMED("M55,CSM,C;");

		sv0 = StateVectorCalcEphem(calcParams.src);

		opt.CSMMass = 10000.0; //Doesn't matter
		opt.dV_LVLH = _V(+1.0, -1.1, -2.6)*0.3048; //60° pitch, 45° yaw
		opt.enginetype = RTCC_ENGINETYPE_CSMRCSMINUS4;
		opt.HeadsUp = true;
		opt.LMMass = 0.0;
		opt.REFSMMAT = EZJGMTX1.data[0].REFSMMAT;
		opt.sv0 = sv0;
		opt.TIG = TimeofIgnition - 20.0*60.0;

		AP7ManeuverPAD(&opt, manpad);

		OrbMech::format_time_HHMMSS(buff1, TimeofIgnition - 20.0*60.0);

		sprintf(form->paddata, "Separation at %s. Roll %.1f, pitch %.1f, yaw %.1f", buff1, manpad.Att.x, manpad.Att.y, manpad.Att.z);
	}
	break;
	case 43: //BLOCK DATA 12
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 5;
		double lng[] = { -160.0*RAD, -160.0*RAD, -33.0*RAD, -31.0*RAD, -32.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(110,53,53),OrbMech::HHMMSSToSS(112,27,57),OrbMech::HHMMSSToSS(113,9,29),OrbMech::HHMMSSToSS(114,39,6), OrbMech::HHMMSSToSS(116, 12, 55) };
		std::string area[] = { "070-CC", "071-CC", "72-AC", "73-2A", "74-AC" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 44: //BLOCK DATA 13
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 8;
		double lng[] = { -68.2*RAD, -33.0*RAD, -32.9*RAD, -69.0*RAD, -170.5*RAD, -170.5*RAD, -170.5*RAD, -160.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(117,39,36),OrbMech::HHMMSSToSS(119,17,43),OrbMech::HHMMSSToSS(120,52,15),OrbMech::HHMMSSToSS(122,17,41), 
			OrbMech::HHMMSSToSS(125, 3, 53), OrbMech::HHMMSSToSS(126, 36, 9), OrbMech::HHMMSSToSS(128, 9, 44), OrbMech::HHMMSSToSS(129, 46, 43) };
		std::string area[] = { "075-1A", "076-2B", "077-2B", "078-1A", "079-4A", "080-4B", "081-4A", "082-DC" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 45: //SPS-6
	case 57: //SPS-7
	{
		AP7MNV * form = (AP7MNV *)pad;

		PMMMPTInput in;
		AP7ManPADOpt opt;
		REFSMMATOpt refsopt;
		GMPOpt gmpopt;
		double P30TIG, TIG_imp;
		VECTOR3 dV_LVLH, dV_imp;
		MATRIX3 REFSMMAT;
		EphemerisData sv0;
		char buffer1[1000];
		char buffer2[1000];

		sv0 = StateVectorCalcEphem(calcParams.src); //State vector for uplink
		med_m50.CSMWT = calcParams.src->GetMass();

		if (fcn == 45)
		{
			gmpopt.H_A = 130.0*1852.0;
			gmpopt.H_P = 97.0*1852.0;	//Should be 95, but ensures that even with residuals and drag the perigee won't fall below 95 at SPS-7, which makes the calculation much more reliable
			gmpopt.long_D = 122.3*RAD;
			gmpopt.ManeuverCode = RTCC_GMP_HBL;
			gmpopt.sv_in = sv0;
			gmpopt.TIG_GET = OrbMech::HHMMSSToSS(121, 30, 0);

			opt.sxtstardtime = -20.0*60.0;
		}
		else
		{
			gmpopt.H_A = 210.0*1852.0;
			gmpopt.H_P = 95.0*1852.0;	//Should be 97, but, see above
			gmpopt.long_D = -45.0*RAD;
			gmpopt.ManeuverCode = RTCC_GMP_HAS;
			gmpopt.sv_in = sv0;
			gmpopt.TIG_GET = OrbMech::HHMMSSToSS(169, 10, 0);
			gmpopt.N = 46;

			opt.sxtstardtime = -40.0*60.0;
		}

		GeneralManeuverProcessor(&gmpopt, dV_imp, TIG_imp);

		in.CONFIG = 1; //CSM
		in.CSMWeight = med_m50.CSMWT;
		in.sv_before = PZGPMELM.SV_before;
		in.V_aft = PZGPMELM.V_after;
		in.DETU = 18.0; //Ullage
		in.UT = false; //2 jets
		in.IgnitionTimeOption = false;
		in.IterationFlag = false;
		in.LMWeight = 0.0;
		in.Thruster = RTCC_ENGINETYPE_CSMSPS;
		in.VC = RTCC_MANVEHICLE_CSM;
		in.VehicleArea = PZMPTCSM.ConfigurationArea;
		in.HeadsUpIndicator = true;

		double GMT_TIG;
		PoweredFlightProcessor(in, GMT_TIG, dV_LVLH);
		P30TIG = GETfromGMT(GMT_TIG);

		refsopt.dV_LVLH = dV_LVLH;
		refsopt.REFSMMATTime = P30TIG;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;
		refsopt.vesseltype = 0;

		REFSMMAT = REFSMMATCalc(&refsopt);

		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = true;
		opt.navcheckGET = P30TIG - 30.0*60.0;
		opt.REFSMMAT = REFSMMAT;
		opt.TIG = P30TIG;
		opt.UllageThrusterOpt = false;
		opt.UllageDT = 18.0;
		opt.sv0 = sv0;
		opt.CSMMass = med_m50.CSMWT;

		AP7ManeuverPAD(&opt, *form);

		if (fcn == 45)
		{
			sprintf(form->purpose, "SPS-6");
		}
		else
		{
			sprintf(form->purpose, "SPS-7");
		}

		AGCStateVectorUpdate(buffer1, 1, 1, sv0, true);
		CMCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, Verb 66, target load");
		}
	}
	break;
	case 46: //S065 PHOTOGRAPHY UPDATE 1
	case 47: //S065 PHOTOGRAPHY UPDATE 2
	case 58: //S065 PHOTOGRAPHY UPDATE 3
	case 59: //S065 PHOTOGRAPHY UPDATE 4
	case 63: //S065 PHOTOGRAPHY UPDATE 5
	case 64: //S065 PHOTOGRAPHY UPDATE 6
	case 68: //S065 PHOTOGRAPHY UPDATE 7
	case 69: //S065 PHOTOGRAPHY UPDATE 8
	{
		S065UPDATE * form = (S065UPDATE *)pad;

		SV sv0, sv1, sv2;
		MATRIX3 REFSMMAT, M_R;
		VECTOR3 Att, UX, UY, UZ;
		double GETbase, GET_AOS, GET_LOS, dt;


		for (int i = 0;i < 4;i++)
		{
			strcpy(form->Area[i], "N/A");
			form->FDAIAngles[i] = _V(0, 0, 0);
			form->GETStart[i] = 0.0;
			form->TAlign[i] = 0.0;
			form->ExposureInterval[i] = 0.0;
			form->ExposureNum[i] = 0;
			form->OrbRate[i] = false;
		}

		sv0 = StateVectorCalc(calcParams.src);
		GETbase = CalcGETBase();
		REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);

		if (fcn == 46)
		{
			dt = OrbMech::HHMMSSToSS(124, 0, 0) - OrbMech::GETfromMJD(sv0.MJD, GETbase);
			sv1 = coast(sv0, dt);

			FindRadarAOSLOS(sv1, 31.0*RAD, -115.5*RAD, GET_AOS, GET_LOS);
			form->GETStart[0] = GET_AOS - 5.0*60.0;
			form->OrbRate[0] = true;
			form->OrbRate[2] = true;

			sprintf(form->Area[1], "SW US");
			form->GETStart[1] = GET_AOS;
			form->ExposureInterval[1] = 6.0;
			form->ExposureNum[1] = 25;

			FindRadarAOSLOS(sv1, 29.6667*RAD, -95.1667*RAD, GET_AOS, GET_LOS);
			sprintf(form->Area[2], "Houston");
			form->GETStart[2] = GET_AOS;
			form->ExposureInterval[2] = 6.0;
			form->ExposureNum[2] = 3;
		}
		else if (fcn == 47)
		{
			dt = OrbMech::HHMMSSToSS(125, 40, 0) - OrbMech::GETfromMJD(sv0.MJD, GETbase);
			sv1 = coast(sv0, dt);

			FindRadarAOSLOS(sv1, 19.3*RAD, -99.6667*RAD, GET_AOS, GET_LOS);
			form->GETStart[0] = GET_AOS - 5.0*60.0;
			form->OrbRate[0] = true;
			form->OrbRate[2] = true;

			sprintf(form->Area[1], "Mexico");
			form->GETStart[1] = GET_AOS;
			form->ExposureInterval[1] = 6.0;
			form->ExposureNum[1] = 15;
		}
		else if (fcn == 58)
		{
			dt = OrbMech::HHMMSSToSS(171, 10, 0) - OrbMech::GETfromMJD(sv0.MJD, GETbase);
			sv1 = coast(sv0, dt);

			FindRadarAOSLOS(sv1, 31.0*RAD, -115.5*RAD, GET_AOS, GET_LOS);
			form->GETStart[0] = GET_AOS - 5.0*60.0;
			form->OrbRate[0] = true;
			form->OrbRate[2] = true;

			sprintf(form->Area[1], "SW US");
			form->GETStart[1] = GET_AOS;
			form->ExposureInterval[1] = 6.0;
			form->ExposureNum[1] = 25;
		}
		else if (fcn == 59)
		{
			dt = OrbMech::HHMMSSToSS(172, 45, 0) - OrbMech::GETfromMJD(sv0.MJD, GETbase);
			sv1 = coast(sv0, dt);

			FindRadarAOSLOS(sv1, 19.3*RAD, -99.666*RAD, GET_AOS, GET_LOS);
			form->GETStart[0] = GET_AOS - 5.0*60.0;
			form->OrbRate[0] = true;
			form->OrbRate[2] = true;

			sprintf(form->Area[1], "Mexico");
			form->GETStart[1] = GET_AOS;
			form->ExposureInterval[1] = 6.0;
			form->ExposureNum[1] = 25;

			FindRadarAOSLOS(sv1, -19.81666*RAD, -43.3666*RAD, GET_AOS, GET_LOS);
			sprintf(form->Area[2], "Brazil");
			form->GETStart[2] = GET_AOS;
			form->ExposureInterval[2] = 6.0;
			form->ExposureNum[2] = 3;
		}
		else if (fcn == 63)
		{
			dt = OrbMech::HHMMSSToSS(190, 25, 0) - OrbMech::GETfromMJD(sv0.MJD, GETbase);
			sv1 = coast(sv0, dt);

			FindRadarAOSLOS(sv1, 31.91666*RAD, -105.0*RAD, GET_AOS, GET_LOS);
			form->GETStart[0] = GET_AOS - 5.0*60.0;
			form->OrbRate[0] = true;
			form->OrbRate[2] = true;

			sprintf(form->Area[1], "North Mexico");
			form->GETStart[1] = GET_AOS;
			form->ExposureInterval[1] = 6.0;
			form->ExposureNum[1] = 6;

			FindRadarAOSLOS(sv1, 29.1667*RAD, -89.333*RAD, GET_AOS, GET_LOS);
			sprintf(form->Area[2], "Southeast US");
			form->GETStart[2] = GET_AOS;
			form->ExposureInterval[2] = 6.0;
			form->ExposureNum[2] = 6;

			FindRadarAOSLOS(sv1, 17.0*RAD, -15.61667*RAD, GET_AOS, GET_LOS);
			sprintf(form->Area[3], "Africa");
			form->GETStart[3] = GET_AOS;
			form->ExposureInterval[3] = 12.0;
			form->ExposureNum[3] = 48;
		}
		else if (fcn == 64)
		{
			dt = OrbMech::HHMMSSToSS(192, 0, 0) - OrbMech::GETfromMJD(sv0.MJD, GETbase);
			sv1 = coast(sv0, dt);

			FindRadarAOSLOS(sv1, 31.0*RAD, -115.5*RAD, GET_AOS, GET_LOS);
			form->GETStart[0] = GET_AOS - 5.0*60.0;
			form->OrbRate[0] = true;
			form->OrbRate[2] = true;

			sprintf(form->Area[1], "Southwest US");
			form->GETStart[1] = GET_AOS;
			form->ExposureInterval[1] = 6.0;
			form->ExposureNum[1] = 3;
		}
		else if (fcn == 68)
		{
			dt = OrbMech::HHMMSSToSS(214, 30, 0) - OrbMech::GETfromMJD(sv0.MJD, GETbase);
			sv1 = coast(sv0, dt);

			//Wilmington, North Carolina
			FindRadarAOSLOS(sv1, 34.22333*RAD, -77.91222*RAD, GET_AOS, GET_LOS);
			form->GETStart[0] = GET_AOS - 5.0*60.0;
			form->OrbRate[0] = true;
			form->OrbRate[2] = true;

			sprintf(form->Area[1], "Wilmington");
			form->GETStart[1] = GET_AOS;
			form->ExposureInterval[1] = 20;
			form->ExposureNum[1] = 3;

			FindRadarAOSLOS(sv1, -17.3667*RAD, 37.95*RAD, GET_AOS, GET_LOS);
			sprintf(form->Area[2], "Mozambique");
			form->GETStart[2] = GET_AOS;
			form->ExposureInterval[2] = 12.0;
			form->ExposureNum[2] = 6;
		}
		else if (fcn == 69)
		{
			dt = OrbMech::HHMMSSToSS(215, 55, 0) - OrbMech::GETfromMJD(sv0.MJD, GETbase);
			sv1 = coast(sv0, dt);

			FindRadarAOSLOS(sv1, 32.333*RAD, -107.667*RAD, GET_AOS, GET_LOS);
			form->GETStart[0] = GET_AOS - 5.0*60.0;
			form->OrbRate[0] = true;
			form->OrbRate[2] = true;

			sprintf(form->Area[1], "New Mexico");
			form->GETStart[1] = GET_AOS;
			form->ExposureInterval[1] = 20;
			form->ExposureNum[1] = 6;
		}

		sv2 = coast(sv1, form->GETStart[0] - OrbMech::GETfromMJD(sv1.MJD, GETbase));

		UY = unit(crossp(sv2.V, -sv2.R));
		UZ = unit(sv2.R);
		UX = crossp(UY, UZ);

		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		Att = OrbMech::CALCGAR(REFSMMAT, M_R);

		form->FDAIAngles[0] = Att * DEG;
	}
	break;
	case 48: //BLOCK DATA 14
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 8;
		double lng[] = { 148.0*RAD, 138.0*RAD, -161.0*RAD, -28.0*RAD, -32.0*RAD, -30.0*RAD, -30.0*RAD, -66.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(131,8,49),OrbMech::HHMMSSToSS(132,40,27),OrbMech::HHMMSSToSS(134,32,19),OrbMech::HHMMSSToSS(135,5,33),
			OrbMech::HHMMSSToSS(136, 40, 9), OrbMech::HHMMSSToSS(138, 15, 36), OrbMech::HHMMSSToSS(139, 49, 30), OrbMech::HHMMSSToSS(141, 14, 42) };
		std::string area[] = { "083-CC", "084-CC", "085-CC", "086-AC", "087-AC", "088-2A", "089-2B", "090-1B" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 49: //BLOCK DATA 15
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 8;
		double lng[] = { -68.0*RAD, -62.5*RAD, -68.0*RAD, -164.9*RAD, -164.0*RAD, -165.0*RAD, -161.0*RAD, -171.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(142,44,15),OrbMech::HHMMSSToSS(144,19,36),OrbMech::HHMMSSToSS(145,52,18),OrbMech::HHMMSSToSS(148,36,40),
			OrbMech::HHMMSSToSS(150, 10, 27), OrbMech::HHMMSSToSS(151, 44, 0), OrbMech::HHMMSSToSS(153, 19, 44), OrbMech::HHMMSSToSS(154, 51, 55) };
		std::string area[] = { "091-1B", "092-1B", "093-1A", "094-4B", "095-4B", "096-4A", "097-CC", "098-CC" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 50: //LANDMARK TRACKING T ALIGN
	{
		GENERICPAD * form = (GENERICPAD *)pad;

		SV sv0, sv1;
		OBJHANDLE hSun;
		double GETbase, dt, t_align;
		char buff[64], buffer1[1000];

		GETbase = CalcGETBase();
		sv0 = StateVectorCalc(calcParams.src);
		sv1 = coast(sv0, OrbMech::HHMMSSToSS(142, 50, 0) - OrbMech::GETfromMJD(sv0.MJD, GETbase));
		hSun = oapiGetObjectByName("Sun");

		dt = OrbMech::sunrise(SystemParameters.MAT_J2000_BRCS, sv1.R, sv1.V, sv1.MJD, sv1.gravref, hSun, true, false);
		t_align = OrbMech::GETfromMJD(sv1.MJD, GETbase) + dt;

		OrbMech::format_time_HHMMSS(buff, t_align);
		sprintf(form->paddata, "T Align is %s GET", buff);

		AGCStateVectorUpdate(buffer1, sv0, true, true);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, Verb 66");
		}
	}
	break;
	case 51: //LANDMARK TRACKING UPDATE 1
	case 52: //LANDMARK TRACKING UPDATE 2
	case 53: //LANDMARK TRACKING UPDATE 3
	case 54: //LANDMARK TRACKING UPDATE 4
	{
		LMARKTRKPADOpt opt;
		SV sv0;

		AP11LMARKTRKPAD * form = (AP11LMARKTRKPAD *)pad;

		sv0 = StateVectorCalc(calcParams.src);

		opt.sv0 = sv0;

		if (fcn == 51)
		{
			sprintf(form->LmkID[0], "021");
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(142, 30, 0);
			opt.alt[0] = 0.0;
			opt.lat[0] = 27.689*RAD;
			opt.lng[0] = -97.243*RAD;

			sprintf(form->LmkID[1], "207");
			opt.LmkTime[1] = OrbMech::HHMMSSToSS(143, 0, 0);
			opt.alt[1] = 0.0;
			opt.lat[1] = 23.617*RAD;
			opt.lng[1] = -15.985*RAD;

			sprintf(form->LmkID[2], "255");
			opt.LmkTime[2] = OrbMech::HHMMSSToSS(143, 10, 0);
			opt.alt[2] = 0.42*1852.0;
			opt.lat[2] = -3.362*RAD;
			opt.lng[2] = 29.281*RAD;

			opt.entries = 3;
		}
		else if (fcn == 52)
		{
			sprintf(form->LmkID[0], "010");
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(144, 10, 0);
			opt.alt[0] = 0.0;
			opt.lat[0] = 28.876*RAD;
			opt.lng[0] = -112.584*RAD;

			sprintf(form->LmkID[1], "042");
			opt.LmkTime[1] = OrbMech::HHMMSSToSS(144, 15, 0);
			opt.alt[1] = 0.0;
			opt.lat[1] = 33.840*RAD;
			opt.lng[1] = -77.962*RAD;

			sprintf(form->LmkID[2], "212");
			opt.LmkTime[2] = OrbMech::HHMMSSToSS(144, 20, 0);
			opt.alt[2] = 0.0;
			opt.lat[2] = 9.451*RAD;
			opt.lng[2] = -13.831*RAD;

			sprintf(form->LmkID[3], "222");
			opt.LmkTime[3] = OrbMech::HHMMSSToSS(144, 25, 0);
			opt.alt[3] = 0.0;
			opt.lat[3] = -8.758*RAD;
			opt.lng[3] = 13.266*RAD;

			opt.entries = 4;
		}
		else if (fcn == 53)
		{
			sprintf(form->LmkID[0], "002");
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(145, 30, 0);
			opt.alt[0] = 0.0;
			opt.lat[0] = 21.161*RAD;
			opt.lng[0] = -156.713*RAD;

			sprintf(form->LmkID[1], "005");
			opt.LmkTime[1] = OrbMech::HHMMSSToSS(145, 45, 0);
			opt.alt[1] = 0.01*1852.0;
			opt.lat[1] = 33.479*RAD;
			opt.lng[1] = -118.606*RAD;

			sprintf(form->LmkID[2], "044");
			opt.LmkTime[2] = OrbMech::HHMMSSToSS(145, 50, 0);
			opt.alt[2] = 0.0;
			opt.lat[2] = 31.150*RAD;
			opt.lng[2] = -81.395*RAD;

			/*sprintf(form->LmkID[3], "049");
			opt.LmkTime[3] = OrbMech::HHMMSSToSS(145, 55, 0);
			opt.alt[3] = 0.0;
			opt.lat[3] = 26.694*RAD;
			opt.lng[3] = -78.978*RAD;*/

			opt.entries = 3;
		}
		else if (fcn == 54)
		{
			sprintf(form->LmkID[0], "006");
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(147, 0, 0);
			opt.alt[0] = 0.01*1852.0;
			opt.lat[0] = 32.665*RAD;
			opt.lng[0] = -117.242*RAD;

			sprintf(form->LmkID[1], "020");
			opt.LmkTime[1] = OrbMech::HHMMSSToSS(147, 5, 0);
			opt.alt[1] = 0.0;
			opt.lat[1] = 29.263*RAD;
			opt.lng[1] = -94.859*RAD;

			sprintf(form->LmkID[2], "067");
			opt.LmkTime[2] = OrbMech::HHMMSSToSS(147, 10, 0);
			opt.alt[2] = 0.0;
			opt.lat[2] = 17.812*RAD;
			opt.lng[2] = -71.654*RAD;

			sprintf(form->LmkID[3], "139");
			opt.LmkTime[3] = OrbMech::HHMMSSToSS(147, 15, 0);
			opt.alt[3] = 0.0;
			opt.lat[3] = -3.699*RAD;
			opt.lng[3] = -38.482*RAD;

			opt.entries = 4;
		}

		LandmarkTrackingPAD(&opt, *form);
	}
	break;
	case 55: //BLOCK DATA 16
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -143.0*RAD, -161.0*RAD, -30.0*RAD, -32.0*RAD, -30.0*RAD, -59.5*RAD};
		double GETI[] = { OrbMech::HHMMSSToSS(156, 15, 41),OrbMech::HHMMSSToSS(158, 6, 17),OrbMech::HHMMSSToSS(158, 40, 36),OrbMech::HHMMSSToSS(160, 15, 37),
			OrbMech::HHMMSSToSS(161, 50, 48), OrbMech::HHMMSSToSS(163, 17, 18)};
		std::string area[] = { "099-CC", "100-CC", "101-AC", "102-AC", "103-2A", "104-1B" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 56: //BLOCK DATA 17
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -29.0*RAD, -29.0*RAD, -34.0*RAD, -68.0*RAD, -159.0*RAD, -160.9*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(164, 54, 2),OrbMech::HHMMSSToSS(166, 27, 38),OrbMech::HHMMSSToSS(168, 1, 3),OrbMech::HHMMSSToSS(169, 26, 8),
			OrbMech::HHMMSSToSS(172, 18, 34), OrbMech::HHMMSSToSS(173, 56, 15) };
		std::string area[] = { "105-2B", "106-2B", "107-AC", "108-1A", "109-4C", "110-4B" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 60: //BLOCK DATA 18
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 8;
		double lng[] = { -160.0*RAD, -148.5*RAD, -144.0*RAD, -141.0*RAD, -161.0*RAD, -32.0*RAD, -27.0*RAD, -30.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(175, 29, 41),OrbMech::HHMMSSToSS(176, 53, 9),OrbMech::HHMMSSToSS(178, 32, 27),OrbMech::HHMMSSToSS(180, 4, 48),
			OrbMech::HHMMSSToSS(182, 1, 49), OrbMech::HHMMSSToSS(182, 21, 1), OrbMech::HHMMSSToSS(183, 59, 15), OrbMech::HHMMSSToSS(185, 37, 27) };
		std::string area[] = { "111-4A", "112-3B", "113-3A", "114-3C", "115-CC", "116-AC", "117-2C", "118-2A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 61: //BLOCK DATA 19
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 8;
		double lng[] = { -64.0*RAD, -68.0*RAD, -66.3*RAD, -66.0*RAD, -163.2*RAD, -163.0*RAD, -163.2*RAD, 149.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(187, 3, 40),OrbMech::HHMMSSToSS(188, 42, 36),OrbMech::HHMMSSToSS(190, 25, 20),OrbMech::HHMMSSToSS(192, 7, 2),
			OrbMech::HHMMSSToSS(194, 43, 50), OrbMech::HHMMSSToSS(196, 25, 35), OrbMech::HHMMSSToSS(198, 7, 6), OrbMech::HHMMSSToSS(199, 25, 49) };
		std::string area[] = { "119-1B", "120-1B", "121-1B", "122-1A", "123-4A", "124-4B", "125-4A", "126-3B" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 62: //S065 T ALIGN
	{
		GENERICPAD * form = (GENERICPAD *)pad;

		SV sv0, sv1;
		char buff[64];
		double t_align, dt, GET_AOS, GET_LOS;
		char buffer1[1000];

		sv0 = StateVectorCalc(calcParams.src);
		dt = OrbMech::HHMMSSToSS(190, 25, 0) - OrbMech::GETfromMJD(sv0.MJD, CalcGETBase());
		sv1 = coast(sv0, dt);

		//Northern Mexico
		FindRadarAOSLOS(sv1, 31.91666*RAD, -105.0*RAD, GET_AOS, GET_LOS);
		t_align = GET_AOS - 5.0*60.0;

		OrbMech::format_time_HHMMSS(buff, t_align);
		sprintf(form->paddata, "T Align is %s GET", buff);

		AGCStateVectorUpdate(buffer1, sv0, true, true);
		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, Verb 66");
		}
	}
	break;
	case 65: //BLOCK DATA 20
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 8;
		double lng[] = { 148.5*RAD, 145.0*RAD, -160.0*RAD, -27.0*RAD, -32.5*RAD, -28.0*RAD, -58.9*RAD, -68.0*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(201, 2, 9),OrbMech::HHMMSSToSS(202, 42, 15),OrbMech::HHMMSSToSS(204, 30, 30),OrbMech::HHMMSSToSS(204, 55, 37),
			OrbMech::HHMMSSToSS(206, 25, 7), OrbMech::HHMMSSToSS(208, 3, 15), OrbMech::HHMMSSToSS(209, 31, 36), OrbMech::HHMMSSToSS(211, 6, 48) };
		std::string area[] = { "127-3A", "128-3B", "129-DC", "130-AC", "131-AC", "132-2A", "133-AC", "134-1A" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 66: //BLOCK DATA 21
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 6;
		double lng[] = { -27.0*RAD, -33.0*RAD, -68.0*RAD, -162.4*RAD, -162.4*RAD, -164.0*RAD};
		double GETI[] = { OrbMech::HHMMSSToSS(213, 1, 11),OrbMech::HHMMSSToSS(214, 38, 0),OrbMech::HHMMSSToSS(216, 4, 52),OrbMech::HHMMSSToSS(218, 43, 21),
			OrbMech::HHMMSSToSS(220, 24, 20), OrbMech::HHMMSSToSS(222, 5, 10)};
		std::string area[] = { "135-2B", "136-2B", "137-1A", "138-4A", "139-4A", "140-4B"};

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 67: //S065 T ALIGN
	{
		GENERICPAD * form = (GENERICPAD *)pad;

		SV sv0, sv1;
		char buff[64];
		double t_align, t_guess;
		char buffer1[1000];

		sv0 = StateVectorCalc(calcParams.src);
		t_guess = OrbMech::HHMMSSToSS(214, 21, 0);

		t_align = FindOrbitalSunrise(sv0, t_guess);

		OrbMech::format_time_HHMMSS(buff, t_align);
		sprintf(form->paddata, "T Align is %s GET", buff);

		AGCStateVectorUpdate(buffer1, sv0, true, true);
		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, Verb 66");
		}
	}
	break;
	case 70: //BLOCK DATA 22
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 7;
		double lng[] = { -162.0*RAD, -169.0*RAD, 145.0*RAD, -162.0*RAD, -32.0*RAD, -30.1*RAD, -30.0*RAD};
		double GETI[] = { OrbMech::HHMMSSToSS(223, 37, 43),OrbMech::HHMMSSToSS(225, 12, 55),OrbMech::HHMMSSToSS(226, 41, 6),OrbMech::HHMMSSToSS(228, 31, 8),
			OrbMech::HHMMSSToSS(228, 53, 7), OrbMech::HHMMSSToSS(230, 29, 7), OrbMech::HHMMSSToSS(232, 6, 14)};
		std::string area[] = { "141-CC", "142-CC", "143-CC", "144-CC", "145-AC", "146-AC", "147-2A"};

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 71: //BLOCK DATA 23
	{
		AP7BLK * form = (AP7BLK *)pad;
		AP7BLKOpt opt;

		int n = 8;
		double lng[] = { -64.0*RAD, -68.0*RAD, -31.0*RAD, -67.0*RAD, -68.0*RAD, -161.0*RAD, -160.0*RAD, -159.4*RAD };
		double GETI[] = { OrbMech::HHMMSSToSS(233, 28, 37),OrbMech::HHMMSSToSS(235, 5, 22),OrbMech::HHMMSSToSS(237, 2, 7),OrbMech::HHMMSSToSS(238, 26, 15),
			OrbMech::HHMMSSToSS(240, 7, 58), OrbMech::HHMMSSToSS(242, 46, 54), OrbMech::HHMMSSToSS(244, 27, 53), OrbMech::HHMMSSToSS(246, 10, 9) };
		std::string area[] = { "148-1B", "149-1C", "150-2B", "151-1C", "152-1A", "153-4B", "154-4B", "155-4B" };

		opt.area.assign(area, area + n);
		opt.GETI.assign(GETI, GETI + n);
		opt.lng.assign(lng, lng + n);
		opt.n = n;

		AP7BlockData(&opt, *form);
	}
	break;
	case 72: //NOMINAL SPS DEORBIT
	{
		AP7MNV * form = (AP7MNV *)pad;

		AP7ManPADOpt opt;
		double get_guess, lng_des, gmt_guess, gmt_min, gmt_max;
		EphemerisData sv0, sv_preTIG;
		EMSMISSInputTable intab;
		EphemerisDataTable2 tab;
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		//Get state vector and mass
		sv0 = StateVectorCalcEphem(calcParams.src);
		PZMPTCSM.TotalInitMass = PZMPTCSM.CommonBlock.CSMMass = calcParams.src->GetMass();

		//GET and longitude for deorbit and splashdown
		get_guess = OrbMech::HHMMSSToSS(238, 11, 47);
		lng_des = -59.9*RAD;

		//Generate epehemeris for recovery target selection
		gmt_guess = GMTfromGET(get_guess);
		gmt_min = gmt_guess;
		gmt_max = gmt_guess + 2.75*60.0*60.0;

		intab.AnchorVector = sv0;
		intab.EphemerisBuildIndicator = true;
		intab.ECIEphemerisIndicator = true;
		intab.ECIEphemTableIndicator = &tab;
		intab.EphemerisLeftLimitGMT = gmt_min;
		intab.EphemerisRightLimitGMT = gmt_max;
		intab.ManCutoffIndicator = false;
		intab.VehicleCode = RTCC_MPT_CSM;

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
		RZJCTTC.R31_REFSMMAT = RTCC_REFSMMAT_TYPE_CUR;
		RZJCTTC.R31_GimbalIndicator = -1;
		RZJCTTC.R31_InitialBankAngle = 0.0;
		RZJCTTC.R31_GLevel = 0.2;
		RZJCTTC.R31_FinalBankAngle = 55.0*RAD;

		RMSDBMP(sv0, PZMPTCSM.TotalInitMass);

		//Save data
		TimeofIgnition = RZRFDP.data[2].GETI;
		SplashLatitude = RZRFDP.data[2].lat_T*RAD;
		SplashLongitude = RZRFDP.data[2].lng_T*RAD;
		DeltaV_LVLH = RZRFTT.Manual.DeltaV;

		//Save REFSMMAT in DOD slot
		GMGMED("G11,CSM,DOM;");
		//Move REFSMMAT to current
		GMGMED("G00,CSM,DOD,CSM,CUR;");

		//Uplinked state vector accurate at GETI minus 12 minutes
		sv_preTIG = coast(sv0, GMTfromGET(TimeofIgnition - 12.0*60.0) - sv0.GMT, RTCC_MPT_CSM);

		opt.dV_LVLH = DeltaV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = true;
		opt.navcheckGET = TimeofIgnition - 40.0*60.0;
		opt.REFSMMAT = EZJGMTX1.data[0].REFSMMAT;
		opt.sxtstardtime = -30.0*60.0;
		opt.TIG = TimeofIgnition;
		opt.UllageDT = 15.0;
		opt.UllageThrusterOpt = true;
		opt.sv0 = sv0;
		opt.CSMMass = PZMPTCSM.TotalInitMass;

		AP7ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "151-1A RETROFIRE");

		AGCStateVectorUpdate(buffer1, 1, 1, sv_preTIG, true);
		CMCRetrofireExternalDeltaVUpdate(buffer2, SplashLatitude, SplashLongitude, TimeofIgnition, DeltaV_LVLH);
		AGCDesiredREFSMMATUpdate(buffer3, EZJGMTX1.data[0].REFSMMAT);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, V66, target load, Retrofire REFSMMAT");
		}
	}
	break;
	case 73: //NOMINAL DEORBIT ENTRY PAD
	{
		AP7ENT * form = (AP7ENT *)pad;

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
		sprintf(form->Area[0], "151-1A");
		form->Lat[0] = SplashLatitude * DEG;
		form->Lng[0] = SplashLongitude * DEG;
	}
	break;
	case 74: //POSTBURN ENTRY PAD
	{
		AP7ENT * form = (AP7ENT *)pad;

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
	case 80: //HGA Test REFSMMAT
	{
		GENERICPAD * form = (GENERICPAD *)pad;

		REFSMMATOpt refsopt;
		MATRIX3 REFSMMAT;
		SV sv0, sv1;
		double dt, GET_AOS, GET_LOS, GET_AOS_HAW, GET_LOS_HAW;

		sv0 = StateVectorCalc(calcParams.src);

		dt = OrbMech::HHMMSSToSS(193, 0, 0) - OrbMech::GETfromMJD(sv0.MJD, CalcGETBase());
		sv1 = coast(sv0, dt);

		//Carnarvon
		FindRadarAOSLOS(sv1, -24.90619*RAD, 113.72595*RAD, GET_AOS, GET_LOS);
		//Hawaii
		FindRadarAOSLOS(sv1, 21.44719*RAD, -157.76307*RAD, GET_AOS_HAW, GET_LOS_HAW);

		refsopt.csmlmdocked = false;
		refsopt.dV_LVLH = _V(0, -1.0, 0.0); //Pointing north
		refsopt.REFSMMATTime = GET_AOS;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;
		refsopt.vesseltype = 1;

		REFSMMAT = REFSMMATCalc(&refsopt);

		AGCDesiredREFSMMATUpdate(uplinkdata, REFSMMAT);

		char buff1[64], buff2[64], buff3[64], buff4[64];
		OrbMech::format_time_HHMMSS(buff1, GET_AOS);
		OrbMech::format_time_HHMMSS(buff2, GET_LOS);
		OrbMech::format_time_HHMMSS(buff3, GET_AOS_HAW);
		OrbMech::format_time_HHMMSS(buff4, GET_LOS_HAW);

		sprintf(form->paddata, "Carnarvon AOS %s LOS %s, Hawaii AOS %s LOS %s", buff1, buff2, buff3, buff4);

		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "High Gain Antenna Test REFSMMAT");
		}
	}
	break;
	}

	return scrubbed;
}

void RTCC::DMissionRendezvousPlan(SV sv_A0, double &t_TPI0)
{
	SV sv2;

	//Step 1: Find TPI0 time (25 minutes before sunrise)
	double TPI0_guess, TPI0_sunrise_guess, TPI0_sunrise, dt_sunrise;
	dt_sunrise = 25.0*60.0;
	TPI0_guess = OrbMech::HHMMSSToSS(95, 0, 0);
	TPI0_sunrise_guess = TPI0_guess + dt_sunrise;
	TPI0_sunrise = FindOrbitalSunrise(sv_A0, TPI0_sunrise_guess);
	t_TPI0 = TPI0_sunrise - dt_sunrise;

	//Step 2: Phasing is 70 minutes before TPI0
	calcParams.Phasing = t_TPI0 - 70.0*60.0;

	//Step 3: Insertion is 111:42 minutes after Phasing
	calcParams.Insertion = calcParams.Phasing + 111.0*60.0 + 42.0;

	//Step 4: CSI is two minutes (rounded) after 5° AOS of the TAN pass
	double CSI_guess, lat_TAN, lng_TAN, AOS_TAN, LOS_TAN;
	lat_TAN = groundstations[13][0];
	lng_TAN = groundstations[13][1];
	CSI_guess = calcParams.Insertion + 40.0*60.0;
	sv2 = coast(sv_A0, CSI_guess - OrbMech::GETfromMJD(sv_A0.MJD, CalcGETBase()));
	FindRadarAOSLOS(sv2, lat_TAN, lng_TAN, AOS_TAN, LOS_TAN);
	calcParams.CSI = (floor(AOS_TAN / 60.0) + 2.0)*60.0;

	//Step 5: CDH is placed 44.4 minutes after CSI
	calcParams.CDH = calcParams.CSI + 44.4*60.0;

	//Step 6: Find TPI0 time (25 minutes before sunrise)
	double TPI_guess, TPI_sunrise_guess, TPI_sunrise;
	TPI_guess = OrbMech::HHMMSSToSS(98, 0, 0);
	TPI_sunrise_guess = TPI_guess + dt_sunrise;
	TPI_sunrise = FindOrbitalSunrise(sv_A0, TPI_sunrise_guess);
	calcParams.TPI = TPI_sunrise - dt_sunrise;
}