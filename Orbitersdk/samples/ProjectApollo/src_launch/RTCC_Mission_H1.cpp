/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2022

RTCC Calculations for Mission H1

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

bool RTCC::CalculationMTP_H1(int fcn, LPVOID &pad, char * upString, char * upDesc, char * upMessage)
{
	char uplinkdata[1024 * 3];
	bool preliminary = false;
	bool scrubbed = false;

	switch (fcn) {
	case 1: //GENERIC CMC CSM STATE VECTOR UPDATE
	case 3: //GENERIC LGC CSM STATE VECTOR UPDATE
	{
		EphemerisData sv;
		char buffer1[1000];
		int veh;

		sv = StateVectorCalcEphem(calcParams.src); //State vector for uplink
		if (fcn == 1)
		{
			veh = RTCC_MPT_CSM;
		}
		else
		{
			veh = RTCC_MPT_LM;
		}

		AGCStateVectorUpdate(buffer1, veh, RTCC_MPT_CSM, sv);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 2: //GENERIC CMC LM STATE VECTOR UPDATE
	case 4: //GENERIC LGC LM STATE VECTOR UPDATE
	{
		EphemerisData sv;
		char buffer1[1000];
		int veh;

		sv = StateVectorCalcEphem(calcParams.tgt); //State vector for uplink
		if (fcn == 2)
		{
			veh = RTCC_MPT_CSM;
		}
		else
		{
			veh = RTCC_MPT_LM;
		}

		AGCStateVectorUpdate(buffer1, veh, RTCC_MPT_LM, sv);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector");
		}
	}
	break;
	case 5: //GENERIC CMC CSM STATE VECTOR UPDATE WITH V66
	case 6: //GENERIC LGC LM STATE VECTOR UPDATE WITH V66
	{
		EphemerisData sv;
		char buffer1[1000];
		int veh, vehs;

		if (fcn == 5)
		{
			sv = StateVectorCalcEphem(calcParams.src); //State vector for uplink
			veh = RTCC_MPT_CSM;
			vehs = RTCC_MPT_CSM;
		}
		else
		{
			sv = StateVectorCalcEphem(calcParams.tgt); //State vector for uplink
			veh = RTCC_MPT_LM;
			vehs = RTCC_MPT_LM;
		}

		AGCStateVectorUpdate(buffer1, veh, vehs, sv, true);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			if (fcn == 5)
			{
				sprintf(upDesc, "CSM state vector and V66");
			}
			else
			{
				sprintf(upDesc, "LM state vector and V66");
			}
		}
	}
	break;
	case 7: //CSM DAP DATA
	{
		AP10DAPDATA * form = (AP10DAPDATA *)pad;

		CSMDAPUpdate(calcParams.src, *form, false);
	}
	break;
	case 8: //LM DAP DATA (ASCENT STAGE)
	case 9: //LM DAP DATA WITH V42
	{
		if (fcn == 9)
		{
			LMACTDATA * form = (LMACTDATA *)pad;

			AP10DAPDATA dap;

			LMDAPUpdate(calcParams.tgt, dap, false);

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
		else
		{
			AP10DAPDATA * form = (AP10DAPDATA *)pad;

			LMDAPUpdate(calcParams.tgt, *form, false, true);
		}
	}
	break;
	case 10: //GROUND LIFTOFF TIME UPDATE
	{
		char Buff[128];

		//P80 MED: mission initialization
		sprintf_s(Buff, "P80,1,CSM,%d,%d,%d;", GZGENCSN.MonthofLiftoff, GZGENCSN.DayofLiftoff, GZGENCSN.Year);
		GMGMED(Buff);

		//P10 MED: Enter actual liftoff time
		double TEPHEM0, tephem_scal;
		Saturn *cm = (Saturn *)calcParams.src;

		//Get TEPHEM
		TEPHEM0 = 40403.;
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
		GMGMED("P15,AGS,,100:00:00;");

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
	case 11: //TLI SIMULATION
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
		med_m66.Att = _V(48.6*RAD, -130.9*RAD, -139.1*RAD); //Make this launch day specific?
		med_m66.ConfigChangeInd = RTCC_CONFIGCHANGE_UNDOCKING;
		med_m66.FinalConfig = "CL";

		//Dummy data
		std::vector<std::string> str;
		PMMMED("66", str);

		//Save TLI time
		TimeofIgnition = GETfromGMT(PZMPTCSM.mantable[0].GMT_BI);
		calcParams.TLI = GETfromGMT(PZMPTCSM.mantable[0].GMT_BO);
	}
	break;
	case 12: //TLI+90 MANEUVER PAD
	{
		EntryOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double TLIBase, TIG, GMTSV;
		EphemerisData sv, sv_uplink;
		SV sv1;
		char buffer1[1000];

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalcEphem(calcParams.src);

		TLIBase = calcParams.TLI;
		TIG = TLIBase + 90.0*60.0;
		entopt.ATPLine = 2; //AOL
		entopt.r_rbias = PZREAP.RRBIAS;

		sv1.mass = PZMPTCSM.mantable[1].CommonBlock.CSMMass;
		sv1.gravref = hEarth;
		sv1.MJD = OrbMech::MJDfromGET(PZMPTCSM.mantable[1].GMT_BO, SystemParameters.GMTBASE);
		sv1.R = PZMPTCSM.mantable[1].R_BO;
		sv1.V = PZMPTCSM.mantable[1].V_BO;

		entopt.entrylongmanual = false;
		entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		entopt.TIGguess = TIG;
		entopt.t_Z = OrbMech::HHMMSSToSS(16.0, 46.0, 0.0);
		entopt.type = 1;
		entopt.vessel = calcParams.src;
		entopt.RV_MCC = sv1;

		EntryTargeting(&entopt, &res); //Target Load for uplink

		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
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
		form->Weight = PZMPTCSM.mantable[1].CommonBlock.CSMMass / 0.45359237;
		form->GET05G = res.GET05G;

		sprintf(form->purpose, "TLI+90");
		sprintf(form->remarks, "No ullage, undocked");

		GMTSV = PZMPTCSM.TimeToBeginManeuver[0] - 10.0*60.0; //10 minutes before TB6
		sv_uplink = coast(sv, GMTSV - sv.GMT, RTCC_MPT_CSM); //Coast with venting and drag taken into account

		AGCStateVectorUpdate(buffer1, RTCC_MPT_CSM, RTCC_MPT_CSM, sv_uplink, true);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector and V66");
		}
	}
	break;
	case 13: //LIFTOFF+8 PAD
	{
		EntryOpt entopt;
		EntryResults res;
		SV sv1;

		P37PAD * form = (P37PAD *)pad;

		sv1.mass = PZMPTCSM.mantable[1].CommonBlock.CSMMass;
		sv1.gravref = hEarth;
		sv1.MJD = OrbMech::MJDfromGET(PZMPTCSM.mantable[1].GMT_BO, SystemParameters.GMTBASE);
		sv1.R = PZMPTCSM.mantable[1].R_BO;
		sv1.V = PZMPTCSM.mantable[1].V_BO;

		entopt.entrylongmanual = false;
		entopt.ATPLine = 0; //MPL
		entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		entopt.type = 1;
		entopt.vessel = calcParams.src;
		entopt.r_rbias = PZREAP.RRBIAS;

		entopt.TIGguess = form->GETI[0] = OrbMech::HHMMSSToSS(8, 0, 0);
		entopt.t_Z = OrbMech::HHMMSSToSS(25.0, 43.0, 0.0);
		entopt.RV_MCC = sv1;
		EntryTargeting(&entopt, &res);
		form->dVT[0] = length(res.dV_LVLH) / 0.3048;
		form->GET400K[0] = res.GET05G;
		form->lng[0] = round(res.longitude*DEG);
	}
	break;
	case 14: //TLI PAD
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
		form->type = 2;

		//DMT for sep maneuver
		GMGMED("U20,CSM,2;");
		form->SepATT = DMTBuffer[0].IMUAtt;
		form->ExtATT = OrbMech::imulimit(_V(300.0 - form->SepATT.x, form->SepATT.y + 180.0, 360.0 - form->SepATT.z));

		//Delete maneuvers from MPT
		GMGMED("M62,CSM,1,D;");

		//Set anchor vector time to 0, so that no trajectory updates are done anymore
		EZANCHR1.AnchorVectors[9].Vector.GMT = 0.0;
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
	case 16: //Block Data 1
	case 17: //Block Data 2
	{
		EntryOpt entopt;
		EntryResults res;
		SV sv1, sv2;

		P37PAD * form = (P37PAD *)pad;

		sv1 = StateVectorCalc(calcParams.src);

		if (length(DeltaV_LVLH) > 0.0)
		{
			sv2 = ExecuteManeuver(sv1, TimeofIgnition, DeltaV_LVLH, GetDockedVesselMass(calcParams.src), RTCC_ENGINETYPE_CSMSPS);
		}
		else
		{
			sv2 = sv1;
		}

		entopt.entrylongmanual = false;
		entopt.ATPLine = 0; //MPL
		entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		entopt.type = 1;
		entopt.vessel = calcParams.src;
		entopt.r_rbias = PZREAP.RRBIAS;

		if (fcn == 16)
		{
			entopt.TIGguess = form->GETI[0] = OrbMech::HHMMSSToSS(15, 0, 0);
			entopt.t_Z = OrbMech::HHMMSSToSS(50.0, 6.0, 0.0);
			entopt.RV_MCC = sv2;
			EntryTargeting(&entopt, &res);
			form->dVT[0] = length(res.dV_LVLH) / 0.3048;
			form->GET400K[0] = res.GET05G;
			form->lng[0] = round(res.longitude*DEG);
		}
		else
		{
			entopt.TIGguess = form->GETI[0] = OrbMech::HHMMSSToSS(25, 0, 0);
			entopt.t_Z = OrbMech::HHMMSSToSS(74.0, 12.0, 0.0);
			entopt.RV_MCC = sv1;
			EntryTargeting(&entopt, &res);
			form->dVT[0] = length(res.dV_LVLH) / 0.3048;
			form->GET400K[0] = res.GET05G;
			form->lng[0] = round(res.longitude*DEG);

			entopt.TIGguess = form->GETI[1] = OrbMech::HHMMSSToSS(35, 0, 0);
			entopt.t_Z = OrbMech::HHMMSSToSS(73.0, 39.0, 0.0);
			entopt.RV_MCC = sv2;
			EntryTargeting(&entopt, &res);
			form->dVT[1] = length(res.dV_LVLH) / 0.3048;
			form->GET400K[1] = res.GET05G;
			form->lng[1] = round(res.longitude*DEG);

			entopt.TIGguess = form->GETI[2] = OrbMech::HHMMSSToSS(45, 0, 0);
			entopt.t_Z = OrbMech::HHMMSSToSS(97.0, 58.0, 0.0);
			entopt.RV_MCC = sv2;
			EntryTargeting(&entopt, &res);
			form->dVT[2] = length(res.dV_LVLH) / 0.3048;
			form->GET400K[2] = res.GET05G;
			form->lng[2] = round(res.longitude*DEG);

			entopt.TIGguess = form->GETI[3] = OrbMech::HHMMSSToSS(60, 0, 0);
			entopt.t_Z = OrbMech::HHMMSSToSS(122.0, 01.0, 0.0);
			entopt.RV_MCC = sv2;
			EntryTargeting(&entopt, &res);
			form->dVT[3] = length(res.dV_LVLH) / 0.3048;
			form->GET400K[3] = res.GET05G;
			form->lng[3] = round(res.longitude*DEG);
		}
	}
	break;
	case 18: //PTC REFSMMAT
	{
		char buffer[1000];
		REFSMMATOpt refsopt;
		MATRIX3 REFSMMAT;

		refsopt.REFSMMATopt = 6;
		refsopt.REFSMMATTime = 40547.30729122223; //183:00:30 GET of nominal mission

		REFSMMAT = REFSMMATCalc(&refsopt);
		AGCDesiredREFSMMATUpdate(buffer, REFSMMAT);
		sprintf(uplinkdata, "%s", buffer);

		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "PTC REFSMMAT");
		}
	}
	break;
	case 19: //MCC-1 EVALUATION
	case 21: //MCC-1 CALCULATION AND UPDATE
	case 20: //MCC-2 EVALUATION
	case 22: //MCC-2 CALCULATION AND UPDATE
	{
		double P30TIG, MCC1GET, MCC2GET, CSMmass, LMmass, F23time;
		int engine, mccnum;
		VECTOR3 dV_LVLH;
		EphemerisData sv;
		char Buff[128];

		int hh, mm;
		double ss;

		double TLIbase = calcParams.TLI - 5.0*60.0 - 20.0; //Approximate TLI ignition
		double LOIFP = OrbMech::HHMMSSToSS(83.0, 25.0, 18.2); //Flight plan LOI TIG

		bool IterateNodeGET = false;
		if (SystemParameters.MCLABN < 77.0*RAD && calcParams.TLI < OrbMech::HHMMSSToSS(3, 0, 0))
		{
			IterateNodeGET = true;
		}

		MCC1GET = TLIbase + 9.0*3600.0;
		MCC2GET = TLIbase + 28.0*3600.0;

		sv = StateVectorCalcEphem(calcParams.src);

		PZMCCPLN.MidcourseGET = MCC2GET;
		PZMCCPLN.Config = true;
		PZMCCPLN.Column = 1;
		PZMCCPLN.SFPBlockNum = 1;
		PZMCCPLN.Mode = 5;
		CSMmass = calcParams.src->GetMass();
		LMmass = calcParams.tgt->GetMass();

		sprintf_s(Buff, "F23,0.0:0.0:0.0,0.0:0.0:0.0;");
		GMGMED(Buff);

		TranslunarMidcourseCorrectionProcessor(sv, CSMmass, LMmass);

		if (IterateNodeGET)
		{
			//Iterate node GET for MCC-2
			bool init = true;
			F23time = LOIFP - 11.0*60.0;
			while (PZMCCDIS.data[0].GET_LOI < LOIFP - 5.0 || init)
			{
				OrbMech::SStoHHMMSS(F23time, hh, mm, ss, 0.01);
				sprintf_s(Buff, "F23,%d:%d:%.2lf,%d:%d:%.2lf;", hh, mm, ss, hh, mm + 10, ss);
				GMGMED(Buff);
				TranslunarMidcourseCorrectionProcessor(sv, CSMmass, LMmass);
				F23time = F23time + 5.0;
				if (init) init = false;
			}
		}

		if (fcn == 19 || fcn == 21) // MCC-1
		{
			mccnum = 1;

			if (length(PZMCCDIS.data[0].DV_MCC) < 120.0*0.3048) // Can we wait until MCC-2?
			{
				scrubbed = true;
			}
			else
			{
				PZMCCPLN.MidcourseGET = MCC1GET;

				TranslunarMidcourseCorrectionProcessor(sv, CSMmass, LMmass);

				if (IterateNodeGET)
				{
					//Re-iterate node GET for MCC-1
					bool init = true;
					F23time = LOIFP - 11.0*60.0;
					while (PZMCCDIS.data[0].GET_LOI < LOIFP - 5.0 || init)
					{
						OrbMech::SStoHHMMSS(F23time, hh, mm, ss, 0.01);
						sprintf_s(Buff, "F23,%d:%d:%.2lf,%d:%d:%.2lf;", hh, mm, ss, hh, mm + 10, ss);
						GMGMED(Buff);
						TranslunarMidcourseCorrectionProcessor(sv, CSMmass, LMmass);
						F23time = F23time + 5.0;
						if (init) init = false;
					}
				}
			}
		}
		else // MCC-2
		{
			mccnum = 2;

			if (length(PZMCCDIS.data[0].DV_MCC) < 1.0*0.3048)
			{
				scrubbed = true;
			}
		}

		calcParams.LOI = PZMCCDIS.data[0].GET_LOI;
		//Transfer MCC plan to skeleton flight plan table
		GMGMED("F30,1;");

		if (!scrubbed)
		{
			engine = SPSRCSDecision(SPS_THRUST / (CSMmass + LMmass), PZMCCDIS.data[0].DV_MCC);
			PoweredFlightProcessor(sv, CSMmass, PZMCCPLN.MidcourseGET, engine, LMmass, PZMCCXFR.V_man_after[0] - PZMCCXFR.sv_man_bef[0].V, false, P30TIG, dV_LVLH);

			//Save burn data for block data calculation
			TimeofIgnition = P30TIG;
			DeltaV_LVLH = dV_LVLH;
		}
		else
		{
			DeltaV_LVLH = _V(0.0,0.0,0.0);
		}

		if (fcn > 20)
		{
			if (scrubbed)
			{
				char buffer1[1000];

				sprintf(upMessage, "MCC-%d has been scrubbed.", mccnum);
				sprintf(upDesc, "CSM state vector and V66");

				AGCStateVectorUpdate(buffer1, RTCC_MPT_CSM, RTCC_MPT_CSM, sv, true);

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

				manopt.dV_LVLH = dV_LVLH;
				manopt.enginetype = SPSRCSDecision(SPS_THRUST / (CSMmass + LMmass), dV_LVLH);
				manopt.HeadsUp = true;
				manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
				manopt.TIG = P30TIG;
				manopt.vessel = calcParams.src;
				manopt.vesseltype = 1;

				AP11ManeuverPAD(&manopt, *form);
				sprintf(form->purpose, "MCC-%d", mccnum);
				sprintf(form->remarks, "LM weight is %.0f.", form->LMWeight);

				AGCStateVectorUpdate(buffer1, RTCC_MPT_CSM, RTCC_MPT_CSM, sv, true);
				CMCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

				sprintf(uplinkdata, "%s%s", buffer1, buffer2);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM state vector and V66, target load");
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
		char buffer1[1000];

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.SMODE = 14;
		entopt.RV_MCC = sv;
		entopt.TIGguess = calcParams.LOI - 5.0*3600.0;
		entopt.vessel = calcParams.src;
		entopt.t_zmin = 145.0*3600.0;
		entopt.entrylongmanual = false;
		entopt.ATPLine = 0; //MPL

		RTEMoonTargeting(&entopt, &res);

		opt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = SPSRCSDecision(SPS_THRUST / (calcParams.src->GetMass() + calcParams.tgt->GetMass()), res.dV_LVLH);
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 1;

		AP11ManeuverPAD(&opt, *form);

		sprintf(form->purpose, "Flyby");
		sprintf(form->remarks, "Height of pericynthion is %.0f NM", res.FlybyAlt / 1852.0);
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->GET05G = res.GET05G;

		AGCStateVectorUpdate(buffer1, sv, true, true);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector and V66");
		}
	}
	break;
	case 24: //MCC-3
	{
		AP11ManPADOpt manopt;
		VECTOR3 dV_LVLH, dv;
		EphemerisData sv;
		double P30TIG, tig, CSMmass, LMmass;
		int engine;

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalcEphem(calcParams.src);

		//Calculate MCC-3
		PZMCCPLN.MidcourseGET = calcParams.LOI - 22.0*3600.0;
		PZMCCPLN.Config = true;
		PZMCCPLN.Column = 1;
		PZMCCPLN.SFPBlockNum = 2;
		PZMCCPLN.Mode = 1;
		CSMmass = calcParams.src->GetMass();
		LMmass = calcParams.tgt->GetMass();

		TranslunarMidcourseCorrectionProcessor(sv, CSMmass, LMmass);

		tig = GETfromGMT(PZMCCXFR.sv_man_bef[0].GMT);
		dv = PZMCCXFR.V_man_after[0] - PZMCCXFR.sv_man_bef[0].V;

		//DV smaller than 3 ft/s? This is a simplified logic. The 3 ft/s check should be for MCC-4 and even if it is larger than that MCC-3 might not have been done
		if (length(dv) < 3.0*0.3048)
		{
			scrubbed = true;
		}

		if (scrubbed)
		{
			char buffer1[1000];

			sprintf(upMessage, "MCC-3 has been scrubbed");
			sprintf(upDesc, "CSM state vector and V66");

			AGCStateVectorUpdate(buffer1, RTCC_MPT_CSM, RTCC_MPT_CSM, sv, true);

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

			calcParams.LOI = PZMCCDIS.data[0].GET_LOI;

			engine = SPSRCSDecision(SPS_THRUST / (CSMmass + LMmass), dv);
			PoweredFlightProcessor(sv, CSMmass, tig, engine, LMmass, dv, false, P30TIG, dV_LVLH);

			manopt.dV_LVLH = dV_LVLH;
			manopt.enginetype = engine;
			manopt.HeadsUp = false;
			manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
			manopt.TIG = P30TIG;
			manopt.vessel = calcParams.src;
			manopt.vesseltype = 1;

			AP11ManeuverPAD(&manopt, *form);
			sprintf(form->purpose, "MCC-3");

			AGCStateVectorUpdate(buffer1, RTCC_MPT_CSM, RTCC_MPT_CSM, sv, true);
			CMCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector and V66, target load");
			}
		}
	}
	break;
	case 25: //MCC-4 Evaluation & LS REFSMMAT calculation
	case 26: //MCC-4
	{
		REFSMMATOpt refsopt;
		VECTOR3 dV_LVLH;
		EphemerisData sv;
		MATRIX3 REFSMMAT;
		double P30TIG, h_peri, h_node, CSMmass, LMmass, ApsidRot;
		int engine, loisol;

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalcEphem(calcParams.src);

		if (fcn == 25)
		{
			bool loierr = PMMLRBTI(sv);

			if (loierr == false)
			{
				//Choose solution with the lowest LOI-1 DV
				if (PZLRBTI.sol[6].DVLOI1 > PZLRBTI.sol[7].DVLOI1)
				{
					loisol = 7;
				}
				else
				{
					loisol = 6;
				}

				h_peri = PZLRBTI.h_pc;
				h_node = PZLRBTI.sol[loisol].H_ND;
				ApsidRot = PZLRBTI.sol[loisol].f_ND_E - PZLRBTI.sol[loisol].f_ND_H;
				while (ApsidRot > 180.0)
				{
					ApsidRot -= 360.0;
				}
				while (ApsidRot < -180.0)
				{
					ApsidRot += 360.0;
				}

				//Maneuver execution criteria
				if (h_peri > 50.0 && h_peri < 70.0)
				{
					if (h_node > 50.0 && h_node < 75.0)
					{
						if (abs(ApsidRot) < 45.0)
						{
							scrubbed = true;
						}
					}
				}
			}

			//REFSMMAT calculation
			refsopt.LSAzi = calcParams.LSAzi;
			refsopt.LSLat = BZLAND.lat[RTCC_LMPOS_BEST];
			refsopt.LSLng = BZLAND.lng[RTCC_LMPOS_BEST];
			refsopt.REFSMMATopt = 8;
			refsopt.REFSMMATTime = CZTDTGTU.GETTD;

			REFSMMAT = REFSMMATCalc(&refsopt);

			//Store as LM LLD matrix
			EMGSTSTM(RTCC_MPT_LM, REFSMMAT, RTCC_REFSMMAT_TYPE_LLD, RTCCPresentTimeGMT());
			//Also move to CSM LCV
			GMGMED("G00,LEM,LLD,CSM,LCV;");

			if (scrubbed)
			{
				sprintf(upMessage, "MCC-4 has been scrubbed");
			}
			else
			{
				sprintf(upMessage, "MCC-4 will have to be executed");
			}
		}
		else
		{
			AP11ManPADOpt manopt;
			char buffer1[1000];
			char buffer2[1000];
			char buffer3[1000];
			double tig;
			VECTOR3 dv;

			PZMCCPLN.MidcourseGET = calcParams.LOI - 5.0*3600.0;
			PZMCCPLN.Config = true;
			PZMCCPLN.Column = 1;
			PZMCCPLN.SFPBlockNum = 2;
			PZMCCPLN.Mode = 1;
			CSMmass = calcParams.src->GetMass();
			LMmass = calcParams.tgt->GetMass();

			TranslunarMidcourseCorrectionProcessor(sv, CSMmass, LMmass);
			calcParams.LOI = PZMCCDIS.data[0].GET_LOI;

			tig = GETfromGMT(PZMCCXFR.sv_man_bef[0].GMT);
			dv = PZMCCXFR.V_man_after[0] - PZMCCXFR.sv_man_bef[0].V;

			engine = SPSRCSDecision(SPS_THRUST / (CSMmass + LMmass), dv);
			PoweredFlightProcessor(sv, CSMmass, tig, engine, LMmass, dv, false, P30TIG, dV_LVLH);

			manopt.dV_LVLH = dV_LVLH;
			manopt.enginetype = engine;
			manopt.HeadsUp = false;
			manopt.REFSMMAT = EZJGMTX1.data[RTCC_REFSMMAT_TYPE_LCV - 1].REFSMMAT;
			manopt.TIG = P30TIG;
			manopt.vessel = calcParams.src;
			manopt.vesseltype = 1;

			AP11ManeuverPAD(&manopt, *form);
			sprintf(form->purpose, "MCC-4");

			AGCStateVectorUpdate(buffer1, RTCC_MPT_CSM, RTCC_MPT_CSM, sv, true);
			CMCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

			REFSMMAT = EZJGMTX1.data[RTCC_REFSMMAT_TYPE_LCV - 1].REFSMMAT;
			AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT);

			sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector and V66, target load, Landing Site REFSMMAT");
			}

			//Save burn data for PC+2 calculation
			TimeofIgnition = P30TIG;
			DeltaV_LVLH = dV_LVLH;
		}
	}
	break;
	case 27: //PC+2 UPDATE after MCC-4 UPDATE
	case 28: //PC+2 UPDATE
	{
		RTEMoonOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		SV sv, sv1;

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalc(calcParams.src);

		//Simulate MCC-4 if not scrubbed
		if (fcn == 27)
		{
			sv1 = ExecuteManeuver(sv, TimeofIgnition, DeltaV_LVLH, GetDockedVesselMass(calcParams.src), RTCC_ENGINETYPE_CSMSPS);
		}
		else
		{
			sv1 = sv;
		}

		entopt.returnspeed = 2;
		entopt.SMODE = 14;
		entopt.RV_MCC = sv1;
		entopt.vessel = calcParams.src;
		entopt.TIGguess = calcParams.LOI + 2.0*3600.0;
		PZREAP.VRMAX = 37500.0;
		entopt.entrylongmanual = false;
		entopt.ATPLine = 0; //MPL

		RTEMoonTargeting(&entopt, &res);

		//Reset to default
		PZREAP.VRMAX = 36323.0;

		opt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = false;
		//LS REFSMMAT
		opt.REFSMMAT = EZJGMTX1.data[RTCC_REFSMMAT_TYPE_LCV - 1].REFSMMAT;
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
		form->type = 2;
	}
	break;
	case 29:    // LOI-1 MANEUVER with LS REFSMMAT uplink
	case 30:	// LOI-1 MANEUVER
	{
		AP11ManPADOpt manopt;
		double P30TIG, tig, CSMmass, LMmass;
		VECTOR3 dV_LVLH, dv;
		int loisol;
		EphemerisData sv;

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalcEphem(calcParams.src); //State vector for uplink
		CSMmass = calcParams.src->GetMass();
		LMmass = calcParams.tgt->GetMass();

		PMMLRBTI(sv);

		//Choose solution with the lowest LOI-1 DV
		if (PZLRBTI.sol[6].DVLOI1 > PZLRBTI.sol[7].DVLOI1)
		{
			loisol = 7;
		}
		else
		{
			loisol = 6;
		}

		tig = GETfromGMT(PZLRBELM.sv_man_bef[loisol].GMT);
		dv = PZLRBELM.V_man_after[loisol] - PZLRBELM.sv_man_bef[loisol].V;
		PoweredFlightProcessor(sv, CSMmass, tig, RTCC_ENGINETYPE_CSMSPS, LMmass, dv, false, P30TIG, dV_LVLH);

		manopt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = false;
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.vesseltype = 1;

		if (fcn == 29)
		{
			manopt.REFSMMAT = EZJGMTX1.data[RTCC_REFSMMAT_TYPE_LCV - 1].REFSMMAT;
		}
		else
		{
			manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		}

		AP11ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "LOI-1");
		sprintf(form->remarks, "LM weight is %.0f", form->LMWeight);

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

		char buffer1[1000];
		char buffer2[1000];

		AGCStateVectorUpdate(buffer1, RTCC_MPT_CSM, RTCC_MPT_CSM, sv, true);
		CMCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		if (fcn == 29)
		{
			MATRIX3 REFSMMAT;
			char buffer3[1000];

			REFSMMAT = EZJGMTX1.data[RTCC_REFSMMAT_TYPE_LCV - 1].REFSMMAT;

			AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT);

			sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector and V66, target load, Landing Site REFSMMAT");
			}
		}
		else
		{
			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector and V66, target load");
			}
		}
	}
	break;
	case 31: //LOI-2 MANEUVER
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

		manopt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		manopt.sxtstardtime = -40.0*60.0;
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.vesseltype = 1;

		AP11ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "LOI-2");
		sprintf(form->remarks, "Two-jet ullage for 19 seconds");

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

		AGCStateVectorUpdate(buffer1, sv, true, true);
		CMCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector and V66, target load");
		}
	}
	break;
	case 32: //STATE VECTOR and LS REFSMMAT UPLINK
	{
		AP11LMARKTRKPAD * form = (AP11LMARKTRKPAD *)pad;

		LMARKTRKPADOpt landmarkopt;
		MATRIX3 REFSMMAT;
		SV sv;
		REFSMMATOpt opt;
		char buffer1[1000];
		char buffer2[1000];

		sv = StateVectorCalc(calcParams.tgt); //State vector for uplink

		//Updated landing site vector after processing landmark tracking data
		BZLAND.rad[RTCC_LMPOS_BEST] = 937.72*1852.0;
		BZLAND.lat[RTCC_LMPOS_BEST] = -3.016*RAD;
		BZLAND.lng[RTCC_LMPOS_BEST] = -23.42*RAD;

		//MED K17
		GZGENCSN.LDPPPoweredDescentSimFlag = false;
		GZGENCSN.LDPPDwellOrbits = 0;
		//MED K16
		med_k16.Mode = 4;
		med_k16.Sequence = 1;
		med_k16.GETTH1 = med_k16.GETTH2 = med_k16.GETTH3 = med_k16.GETTH4 = calcParams.LOI + 25.5*3600.0;

		LunarDescentPlanningProcessor(sv);

		calcParams.DOI = GETfromGMT(PZLDPELM.sv_man_bef[0].GMT);
		calcParams.PDI = PZLDPDIS.PD_GETIG;
		CZTDTGTU.GETTD = PZLDPDIS.PD_GETTD;

		PoweredFlightProcessor(sv, calcParams.DOI, RTCC_ENGINETYPE_LMDPS, 0.0, PZLDPELM.V_man_after[0] - PZLDPELM.sv_man_bef[0].V, false, TimeofIgnition, DeltaV_LVLH);

		opt.LSLat = BZLAND.lat[RTCC_LMPOS_BEST];
		opt.LSLng = BZLAND.lng[RTCC_LMPOS_BEST];
		opt.REFSMMATopt = 5;
		opt.REFSMMATTime = CZTDTGTU.GETTD;
		opt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&opt);
		//Store as LM LLD
		EMGSTSTM(RTCC_MPT_LM, REFSMMAT, RTCC_REFSMMAT_TYPE_LLD, RTCCPresentTimeGMT());
		//Also move to CSM LCV
		GMGMED("G00,LEM,LLD,CSM,LCV;");

		AGCStateVectorUpdate(buffer1, sv, true, true);
		AGCDesiredREFSMMATUpdate(buffer2, REFSMMAT);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector and V66, LS REFSMMAT");
		}
	}
	break;
	case 33: //LOI-1 EVALUATION
	{
		SV sv;
		OELEMENTS coe;

		sv = StateVectorCalc(calcParams.src);

		coe = OrbMech::coe_from_sv(sv.R, sv.V, OrbMech::mu_Moon);

		if (coe.e > 0.7)
		{
			sprintf(upMessage, "LOI-1 scrubbed, burn PC+2 PAD");
			scrubbed = true;
			mcc->AbortMode = 6;
		}
	}
	break;
	case 35: //LGC ACTIVATION UPDATE
	{
		SV sv;
		MATRIX3 REFSMMAT;
		char buffer1[1000];
		char buffer2[1000];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		REFSMMAT = EZJGMTX3.data[RTCC_REFSMMAT_TYPE_LLD - 1].REFSMMAT;

		AGCStateVectorUpdate(buffer1, sv, false, true);
		AGCREFSMMATUpdate(buffer2, REFSMMAT, false);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector and V66, LS REFSMMAT");
		}
	}
	break;
	case 36: //AGS ACTIVATION UPDATE
	{
		AP11AGSACT *form = (AP11AGSACT*)pad;

		SV sv1, sv2, sv_INP;
		double t_sunrise, t_TPI, KFactor;
		int emem[14];
		char buffer1[1000];

		PDAPOpt opt;
		PDAPResults res;

		LEM *l = (LEM*)calcParams.tgt;

		sv1 = StateVectorCalc(calcParams.tgt);
		sv2 = StateVectorCalc(calcParams.src);

		sv_INP = ExecuteManeuver(sv1, TimeofIgnition, DeltaV_LVLH, 0.0, RTCC_ENGINETYPE_LMDPS);

		t_sunrise = calcParams.PDI + 3.0*3600.0;
		t_TPI = FindOrbitalSunrise(sv2, t_sunrise) - 23.0*60.0;

		bool res_k = CalculateAGSKFactor(&l->agc.vagc, &l->aea.vags, KFactor);
		if (res_k)
		{
			SystemParameters.MCGZSS = SystemParameters.MCGZSL + KFactor / 3600.0;
		}

		opt.dt_stage = 999999.9;
		opt.W_TAPS = 4711.0;
		opt.W_TDRY = 6874.3;
		opt.dt_step = 20.0;
		opt.t_TPI = t_TPI;
		opt.IsTwoSegment = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
		opt.R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.rad[RTCC_LMPOS_BEST]);
		opt.sv_A = sv_INP;
		opt.sv_P = sv2;
		opt.TLAND = CZTDTGTU.GETTD;

		PoweredDescentAbortProgram(opt, res);

		form->KFactor = GETfromGMT(GetAGSClockZero());
		form->DEDA224 = (int)(res.DEDA224 / 0.3048 / 100.0);
		form->DEDA225 = (int)(res.DEDA225 / 0.3048 / 100.0);
		form->DEDA226 = (int)(res.DEDA226 / 0.3048 / 100.0);
		form->DEDA227 = OrbMech::DoubleToDEDA(res.DEDA227 / 0.3048*pow(2, -20), 14);

		/* Pad-load:
		form->DEDA224 = 60326;
		form->DEDA225 = 58158;
		form->DEDA226 = 70312;
		form->DEDA227 = -50181;*/

		emem[0] = 16;
		emem[1] = 2550;
		emem[2] = OrbMech::DoubleToBuffer(res.J1, 23, 1);
		emem[3] = OrbMech::DoubleToBuffer(res.J1, 23, 0);
		emem[4] = OrbMech::DoubleToBuffer(res.K1*PI2, 23, 1);
		emem[5] = OrbMech::DoubleToBuffer(res.K1*PI2, 23, 0);
		emem[6] = OrbMech::DoubleToBuffer(res.J2, 23, 1);
		emem[7] = OrbMech::DoubleToBuffer(res.J2, 23, 0);
		emem[8] = OrbMech::DoubleToBuffer(res.K2*PI2, 23, 1);
		emem[9] = OrbMech::DoubleToBuffer(res.K2*PI2, 23, 0);
		emem[10] = OrbMech::DoubleToBuffer(res.Theta_LIM / PI2, 0, 1);
		emem[11] = OrbMech::DoubleToBuffer(res.Theta_LIM / PI2, 0, 0);
		emem[12] = OrbMech::DoubleToBuffer(res.R_amin, 24, 1);
		emem[13] = OrbMech::DoubleToBuffer(res.R_amin, 24, 0);

		V7XUpdate(71, buffer1, emem, 14);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LGC abort constants");
		}
	}
	break;
	case 37: //SEPARATION MANEUVER
	{
		AP11ManPADOpt opt;
		SV sv;
		VECTOR3 dV_LVLH;
		double t_P, t_Sep;

		AP12SEPPAD * form = (AP12SEPPAD *)pad;
		AP11MNV manpad;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		t_P = OrbMech::period(sv.R, sv.V, OrbMech::mu_Moon);
		t_Sep = floor(calcParams.DOI - t_P / 2.0);
		calcParams.SEP = t_Sep;
		dV_LVLH = _V(2.5, 0, 0)*0.3048;

		opt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMRCSPLUS4;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		opt.TIG = t_Sep;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, manpad);

		form->t_Undock = t_Sep - 30.0*60.0;
		form->t_Separation = t_Sep;
		form->Att_Undock = manpad.Att;
	}
	break;
	case 38: //DESCENT ORBIT INSERTION
	{
		AP11LMManPADOpt opt;
		SV sv, sv_upl;

		char TLANDbuffer[64];
		char buffer1[1000];
		char buffer2[1000];

		AP11LMMNV * form = (AP11LMMNV *)pad;

		sv = StateVectorCalc(calcParams.tgt);

		//MED K17
		GZGENCSN.LDPPPoweredDescentSimFlag = false;
		GZGENCSN.LDPPDwellOrbits = 0;
		//MED K16
		med_k16.Mode = 4;
		med_k16.Sequence = 1;
		med_k16.GETTH1 = med_k16.GETTH2 = med_k16.GETTH3 = med_k16.GETTH4 = calcParams.LOI + 25.5*3600.0;

		LunarDescentPlanningProcessor(sv);

		calcParams.DOI = GETfromGMT(PZLDPELM.sv_man_bef[0].GMT);
		calcParams.PDI = PZLDPDIS.PD_GETIG;
		CZTDTGTU.GETTD = PZLDPDIS.PD_GETTD;

		PoweredFlightProcessor(sv, calcParams.DOI, RTCC_ENGINETYPE_LMDPS, 0.0, PZLDPELM.V_man_after[0] - PZLDPELM.sv_man_bef[0].V, false, TimeofIgnition, DeltaV_LVLH);

		opt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		opt.csmlmdocked = false;
		opt.dV_LVLH = DeltaV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_LMDPS;
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
		opt.TIG = TimeofIgnition;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);
		sprintf(form->purpose, "DOI");

		sv_upl = coast(sv, calcParams.DOI - 10.0*60.0 - OrbMech::GETfromMJD(sv.MJD, CalcGETBase()));
		AGCStateVectorUpdate(buffer1, sv_upl, false);
		LGCExternalDeltaVUpdate(buffer2, TimeofIgnition, DeltaV_LVLH);
		TLANDUpdate(TLANDbuffer, CZTDTGTU.GETTD);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, TLANDbuffer);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector, DOI target load, descent target load");
		}
	}
	break;
	case 40: //TEI-1 UPDATE (PRE LOI-1)
	case 41: //TEI-4 UPDATE (PRE LOI-1)
	case 42: //TEI-5 UPDATE (PRE LOI-2)
	case 43: //TEI-11 UPDATE
	case 44: //TEI-34 UPDATE
	case 45: //TEI-39 UPDATE
	case 46: //TEI-41 UPDATE (PRE PC-2)
	case 47: //TEI-43 UPDATE
	case 48: //TEI-45 UPDATE (BLOCK DATA)
	case 49: //TEI-45 UPDATE (PRELIM)
	case 50: //TEI-45 UPDATE (FINAL)
	case 51: //TEI-46 UPDATE
	{
		AP11ManPADOpt opt;
		double GET_SV1, returnspeed, DT_TEI_EI, AbortGuess, MoonRevTEI;
		SV sv0, sv1;
		char manname[8];
		EphemerisData sv_e;

		AP11MNV * form = (AP11MNV *)pad;

		sv0 = StateVectorCalc(calcParams.src); //State vector for uplink

		GMGMED("F79,0;"); //Clear abort scan table

		//Simulate the maneuver preceeding TEI (LOI-1 or LOI-2 or PC-2)
		if (fcn == 40 || fcn == 41 || fcn == 42 || fcn == 46)
		{
			sv1 = ExecuteManeuver(sv0, TimeofIgnition, DeltaV_LVLH, GetDockedVesselMass(calcParams.src), RTCC_ENGINETYPE_CSMSPS);
		}
		else
		{
			sv1 = sv0;
		}

		GET_SV1 = OrbMech::GETfromMJD(sv1.MJD, CalcGETBase());

		sv_e.GMT = GMTfromGET(GET_SV1);
		sv_e.R = sv1.R;
		sv_e.V = sv1.V;
		sv_e.RBI = BODY_MOON;

		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		returnspeed = 1;

		if (fcn == 40)
		{
			sprintf(manname, "TEI-1");
			MoonRevTEI = 1.0;
			opt.REFSMMAT = EZJGMTX1.data[RTCC_REFSMMAT_TYPE_LCV - 1].REFSMMAT; //Landing site REFSMMAT
		}
		else if (fcn == 41)
		{
			sprintf(manname, "TEI-4");
			MoonRevTEI = 4.25;
			opt.REFSMMAT = EZJGMTX1.data[RTCC_REFSMMAT_TYPE_LCV - 1].REFSMMAT; //Landing site REFSMMAT
		}
		else if (fcn == 42)
		{
			sprintf(manname, "TEI-5");
			MoonRevTEI = 5.1;
		}
		else if (fcn == 43)
		{
			sprintf(manname, "TEI-11");
			MoonRevTEI = 11.0;
		}
		else if (fcn == 44)
		{
			sprintf(manname, "TEI-34");
			MoonRevTEI = 33.63;
		}
		else if (fcn == 45)
		{
			sprintf(manname, "TEI-39");
			MoonRevTEI = 38.63;
		}
		else if (fcn == 46)
		{
			sprintf(manname, "TEI-41");
			MoonRevTEI = 40.63;
			opt.REFSMMAT = EZJGMTX1.data[RTCC_REFSMMAT_TYPE_LCV - 1].REFSMMAT; //Photography REFSMMAT
		}
		else if (fcn == 47)
		{
			sprintf(manname, "TEI-43");
			MoonRevTEI = 42.5;
			returnspeed = 0;
		}
		else if (fcn == 48) //Block data
		{
			sprintf(manname, "TEI-45");
			MoonRevTEI = 44.5;
		}
		else if (fcn == 49) //Preliminary
		{
			sprintf(manname, "TEI-45");
			MoonRevTEI = 44.5;
		}
		else if (fcn == 50) //Final
		{
			sprintf(manname, "TEI-%d", mcc->MoonRev);
			MoonRevTEI = ((double)mcc->MoonRev) - 0.5;
		}
		else if (fcn == 51)
		{
			sprintf(manname, "TEI-%d", mcc->MoonRev + 1);
			MoonRevTEI = ((double)mcc->MoonRev + 1) - 0.5;
		}

		AbortGuess = calcParams.LOI + (MoonRevTEI * 2.0*3600.0);

		DT_TEI_EI = 62.0*3600.0;

		if (returnspeed == 0)
		{
			DT_TEI_EI += 24.0*3600.0;
		}
		else if (returnspeed == 2)
		{
			DT_TEI_EI -= 24.0*3600.0;
		}

		VEHDATABUF.csmmass = calcParams.src->GetMass();
		VEHDATABUF.lmascmass = 0.0;
		VEHDATABUF.lmdscmass = 0.0;
		VEHDATABUF.sv = sv_e;
		VEHDATABUF.config = "C";

		med_f75_f77.T_0_min = AbortGuess;
		med_f75_f77.T_Z = AbortGuess + DT_TEI_EI;

		DetermineRTESite(med_f77.Site);

		PZREAP.RTEVectorTime = GMTfromGET(med_f75_f77.T_V) / 3600.0;
		PZREAP.RTET0Min = GMTfromGET(med_f75_f77.T_0_min) / 3600.0;
		PZREAP.RTETimeOfLanding = GMTfromGET(med_f75_f77.T_Z) / 3600.0;
		PZREAP.RTEPTPMissDistance = med_f77.MissDistance;

		PMMREAST(77, &sv_e);

		med_f80.ASTCode = PZREAP.AbortScanTableData[0].ASTCode;
		med_f80.ManeuverCode = "CSUX";
		med_f80.REFSMMAT = "TEI";
		med_f80.HeadsUp = false;
		med_f80.NumQuads = 4;
		med_f80.UllageDT = 11.0;

		PMMREDIG(false);

		if (fcn == 49)
		{
			GMGMED("G11,CSM,REP;");
			GMGMED("G00,CSM,DOD,CSM,LCV;");

			opt.REFSMMAT = EZJGMTX1.data[RTCC_REFSMMAT_TYPE_LCV - 1].REFSMMAT;
		}

		opt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		opt.dV_LVLH = PZREAP.RTEDTable[0].DV_XDV;;
		opt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		opt.HeadsUp = false;
		opt.RV_MCC = sv1;
		opt.TIG = PZREAP.RTEDTable[0].GETI;
		opt.useSV = true;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);

		RMMYNIInputTable entin;
		RMMYNIOutputTable entout;
		EphemerisData2 sv_EI_ECT;

		ELVCNV(PZREAP.AbortScanTableData[0].sv_EI, 0, 1, sv_EI_ECT);

		entin.R0 = sv_EI_ECT.R;
		entin.V0 = sv_EI_ECT.V;
		entin.GMT0 = sv_EI_ECT.GMT;
		entin.lat_T = PZREAP.RTEDTable[0].lat_imp_tgt;
		entin.lng_T = PZREAP.RTEDTable[0].lng_imp_tgt;
		entin.KSWCH = 3;

		RMMYNI(entin, entout);

		sprintf(form->purpose, manname);
		form->lat = PZREAP.RTEDTable[0].lat_imp_tgt*DEG;
		form->lng = PZREAP.RTEDTable[0].lng_imp_tgt*DEG;
		form->RTGO = entout.R_EMS / 1852.0;
		form->VI0 = entout.V_EMS / 0.3048;
		form->GET05G = GETfromGMT(entout.t_05g);
		form->type = 2;

		sprintf(form->remarks, "Four-jet ullage for 11 seconds");

		if (fcn == 40 || fcn == 43)
		{
			sprintf(form->remarks, "%s,\nUndocked", form->remarks);
		}
		else if (fcn == 41)
		{
			sprintf(form->remarks, "%s,\nUndocked, assumes no LOI-2", form->remarks);
		}
		else if (fcn == 42)
		{
			sprintf(form->remarks, "%s,\nUndocked, assumes LOI-2", form->remarks);
		}
		else if (fcn == 45)
		{
			sprintf(form->remarks, "%s,\nAssumes no PC-2", form->remarks);
		}
		else if (fcn == 46)
		{
			sprintf(form->remarks, "%s,\nAssumes PC-2", form->remarks);
		}
		else if (fcn == 49 || fcn == 50)
		{
			form->type = 1;
		}

		if (fcn != 51)
		{
			//Save parameters for further use
			SplashLatitude = PZREAP.RTEDTable[0].lat_imp_tgt;
			SplashLongitude = PZREAP.RTEDTable[0].lng_imp_tgt;
			calcParams.TEI = PZREAP.RTEDTable[0].GETI;
			calcParams.EI = PZREAP.RTEDTable[0].ReentryPET;
		}

		if (fcn == 50)
		{
			char buffer1[1000], buffer2[1000];
			TimeofIgnition = PZREAP.RTEDTable[0].GETI;
			DeltaV_LVLH = PZREAP.RTEDTable[0].DV_XDV;

			AGCStateVectorUpdate(buffer1, sv1, true, true);
			CMCExternalDeltaVUpdate(buffer2, TimeofIgnition, DeltaV_LVLH);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector and V66, target load");
			}
		}
	}
	break;
	case 58: //LM P22 ACQUISITION TIME PRE-PDI
	case 59: //LM P22 ACQUISITION TIME PRE-ASCENT
	case 74: //CSM SV AND RLS UPDATE AND P22 ACQUISTION TIME
	{
		LMP22ACQPAD * form = (LMP22ACQPAD*)pad;

		SV sv0, sv1;
		VECTOR3 R_P;
		double GETbase, alt, lat, lng, dt2, LmkTime, MJDguess, LmkRange;

		GETbase = CalcGETBase();
		sv0 = StateVectorCalc(calcParams.src);

		if (fcn == 58 || fcn == 74)
		{
			LmkTime = calcParams.PDI + 2.0 * 3600.0 + 5.0*60.0;
		}
		else
		{
			LmkTime = calcParams.LunarLiftoff - 3.0 * 3600.0 - 30.0*60.0;
		}
		alt = BZLAND.rad[RTCC_LMPOS_BEST] - OrbMech::R_Moon;;
		lat = BZLAND.lat[RTCC_LMPOS_BEST];
		lng = BZLAND.lng[RTCC_LMPOS_BEST];
		MJDguess = GETbase + LmkTime / 24.0 / 3600.0;
		sv1 = coast(sv0, (MJDguess - sv0.MJD)*24.0*3600.0);
		R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)))*(oapiGetSize(sv1.gravref) + alt);
		dt2 = OrbMech::findelev_gs(SystemParameters.AGCEpoch, SystemParameters.MAT_J2000_BRCS, sv1.R, sv1.V, R_P, MJDguess, 152.0*RAD, sv1.gravref, LmkRange);

		form->P22_ACQ_GET = dt2 + (MJDguess - GETbase) * 24.0 * 60.0 * 60.0;
		//Round
		form->P22_ACQ_GET = round(form->P22_ACQ_GET);

		if (fcn == 58)
		{
			form->Octals[0] = form->Octals[1] = 0;
		}
		else
		{
			form->Octals[0] = OrbMech::DoubleToBuffer(form->P22_ACQ_GET*100.0, 28, 1);
			form->Octals[1] = OrbMech::DoubleToBuffer(form->P22_ACQ_GET*100.0, 28, 0);
		}

		if (fcn == 74)
		{
			EphemerisData sv;
			char buffer1[1000];
			char buffer2[100];

			sv = StateVectorCalcEphem(calcParams.src);

			AGCStateVectorUpdate(buffer1, 2, RTCC_MPT_CSM, sv);
			LandingSiteUplink(buffer2, RTCC_MPT_LM);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, RLS");
			}
		}
	}
	break;
	case 60: //REV 1 MAP UPDATE
	{
		SV2 sv0, sv1, sv2;
		AP10MAPUPDATE upd_hyper, upd_ellip;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc2(calcParams.src);
		LunarOrbitMapUpdate(sv0.sv, upd_hyper);

		sv1 = ExecuteManeuver(sv0, TimeofIgnition, DeltaV_LVLH, RTCC_ENGINETYPE_CSMSPS);
		sv2.sv = coast(sv1.sv, -30.0*60.0);
		LunarOrbitMapUpdate(sv2.sv, upd_ellip);

		form->Rev = 1;
		form->type = 2;
		form->AOSGET = upd_hyper.AOSGET;
		form->LOSGET = upd_hyper.LOSGET;
		form->PMGET = upd_ellip.AOSGET;
	}
	break;
	case 61: //REV 4 LANDMARK TRACKING PAD H-1
	case 62: //REV 12 LANDMARK TRACKING PAD 193
	case 63: //LANDMARK TRACKING PAD 193
	case 64: //LANDMARK TRACKING PAD LANSBERG A
	case 65: //LANDMARK TRACKING PAD CP-1/CP-2/DE-1/FM-1
	case 66: //LM ACQUISITION TIME
	{
		LMARKTRKPADOpt opt;
		SV sv0;
		double GET_SV;

		AP11LMARKTRKPAD * form = (AP11LMARKTRKPAD *)pad;

		sv0 = StateVectorCalc(calcParams.src);

		opt.sv0 = sv0;

		GET_SV = OrbMech::GETfromMJD(sv0.MJD, CalcGETBase());

		if (fcn == 61)
		{
			sprintf(form->LmkID[0], "H-1");
			opt.alt[0] = -1.94*1852.0;
			opt.lat[0] = -1.517*RAD;
			opt.LmkTime[0] = calcParams.LOI + 6.5*3600.0;
			opt.lng[0] = -15.25*RAD;
			opt.entries = 1;
		}
		else if (fcn == 62 || fcn == 63)
		{
		sprintf(form->LmkID[0], "193");
		opt.alt[0] = -1.37*1852.0;
		opt.lat[0] = -3.437*RAD;
		if (fcn == 62)
		{
			opt.LmkTime[0] = calcParams.LOI + 23.0*3600.0;
		}
		else
		{
			opt.LmkTime[0] = GET_SV + 15.0*60.0;
		}
		opt.lng[0] = -23.228*RAD;
		opt.entries = 1;
		}
		else if (fcn == 64)
		{
			sprintf(form->LmkID[0], "LANSBERG A");
			opt.alt[0] = -0.54*1852.0;
			opt.lat[0] = 0.15*RAD;
			opt.LmkTime[0] = GET_SV + 35.0*60.0;
			opt.lng[0] = -31.15*RAD;
			opt.entries = 1;
		}
		else if (fcn == 65)
		{
			opt.entries = 4;

			sprintf(form->LmkID[0], "CP-1");
			opt.alt[0] = 0.0;
			opt.lat[0] = -5.667*RAD;
			opt.LmkTime[0] = GET_SV + 1.0*3600.0 + 5.0*60.0;
			opt.lng[0] = 112.0*RAD;

			sprintf(form->LmkID[1], "CP-2");
			opt.alt[1] = -0.81*1852.0;
			opt.lat[1] = -10.25*RAD;
			opt.LmkTime[1] = opt.LmkTime[0] + 15*60.0;
			opt.lng[1] = 56.182*RAD;

			sprintf(form->LmkID[2], "DE-1");
			opt.alt[2] = -1.7*1852.0;
			opt.lat[2] = -8.883*RAD;
			opt.LmkTime[2] = opt.LmkTime[1] + 13*60.0;
			opt.lng[2] = 15.55*RAD;

			sprintf(form->LmkID[3], "FM-1");
			opt.alt[3] = -1.56*1852.0;
			opt.lat[3] = -3.228*RAD;
			opt.LmkTime[3] = opt.LmkTime[2] + 13*60.0;
			opt.lng[3] = -17.33*RAD;
		}
		else if (fcn == 66)
		{
			//Update landing site
			calcParams.tgt->GetEquPos(BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.rad[RTCC_LMPOS_BEST]);

			sprintf(form->LmkID[0], "Lunar Module");
			opt.alt[0] = BZLAND.rad[RTCC_LMPOS_BEST] - OrbMech::R_Moon;
			opt.lat[0] = BZLAND.lat[RTCC_LMPOS_BEST];
			opt.LmkTime[0] = GET_SV + 15.0*60.0;
			opt.lng[0] = BZLAND.lng[RTCC_LMPOS_BEST];
			opt.entries = 1;
		}

		LandmarkTrackingPAD(&opt, *form);
	}
	break;
	case 170: //PDI2 PAD
	{
		//Recalculate PDI and landing times
		SV sv1, sv2;
		double GETbase, GET_SV1, t_sunrise, t_TPI;
		int emem[14];
		char buffer1[1000];
		char TLANDbuffer[64];

		PDAPOpt opt;
		PDAPResults res;

		AP11AGSACT *form = (AP11AGSACT*)pad;

		sv1 = StateVectorCalc(calcParams.tgt);
		sv2 = StateVectorCalc(calcParams.src);
		GETbase = CalcGETBase();

		GET_SV1 = OrbMech::GETfromMJD(sv1.MJD, GETbase);

		calcParams.SEP = calcParams.PDI + 25 * 60.0;

		//MED K17
		GZGENCSN.LDPPPoweredDescentSimFlag = false;
		GZGENCSN.LDPPDwellOrbits = 0;
		//MED K16
		med_k16.Mode = 4;
		med_k16.Sequence = 1;
		med_k16.GETTH1 = med_k16.GETTH2 = med_k16.GETTH3 = med_k16.GETTH4 = GET_SV1 + 0.5*3600.0;

		LunarDescentPlanningProcessor(sv1);

		calcParams.PDI = PZLDPDIS.PD_GETIG;
		CZTDTGTU.GETTD = PZLDPDIS.PD_GETTD;

		//Update abort constants
		t_sunrise = calcParams.PDI + 3.0*3600.0;
		t_TPI = FindOrbitalSunrise(sv2, t_sunrise) - 23.0*60.0;

		opt.dt_stage = 999999.9;
		opt.W_TAPS = 4711.0;
		opt.W_TDRY = 6874.3;
		opt.dt_step = 20.0;
		opt.t_TPI = t_TPI;
		opt.IsTwoSegment = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
		opt.R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.rad[RTCC_LMPOS_BEST]);
		opt.sv_A = sv1;
		opt.sv_P = sv2;
		opt.TLAND = CZTDTGTU.GETTD;
		//opt.dt_2CSI = 0.0;
		//opt.dt_2TPI = 0.0;

		PoweredDescentAbortProgram(opt, res);

		form->KFactor = GETfromGMT(GetAGSClockZero());
		form->DEDA224 = (int)(res.DEDA224 / 0.3048 / 100.0);
		form->DEDA225 = (int)(res.DEDA225 / 0.3048 / 100.0);
		form->DEDA226 = (int)(res.DEDA226 / 0.3048 / 100.0);
		form->DEDA227 = OrbMech::DoubleToDEDA(res.DEDA227 / 0.3048*pow(2, -20), 14);

		emem[0] = 16;
		emem[1] = 2550;
		emem[2] = OrbMech::DoubleToBuffer(res.J1, 23, 1);
		emem[3] = OrbMech::DoubleToBuffer(res.J1, 23, 0);
		emem[4] = OrbMech::DoubleToBuffer(res.K1*PI2, 23, 1);
		emem[5] = OrbMech::DoubleToBuffer(res.K1*PI2, 23, 0);
		emem[6] = OrbMech::DoubleToBuffer(res.J2, 23, 1);
		emem[7] = OrbMech::DoubleToBuffer(res.J2, 23, 0);
		emem[8] = OrbMech::DoubleToBuffer(res.K2*PI2, 23, 1);
		emem[9] = OrbMech::DoubleToBuffer(res.K2*PI2, 23, 0);
		emem[10] = OrbMech::DoubleToBuffer(res.Theta_LIM / PI2, 0, 1);
		emem[11] = OrbMech::DoubleToBuffer(res.Theta_LIM / PI2, 0, 0);
		emem[12] = OrbMech::DoubleToBuffer(res.R_amin, 24, 1);
		emem[13] = OrbMech::DoubleToBuffer(res.R_amin, 24, 0);

		V7XUpdate(71, buffer1, emem, 14);
		TLANDUpdate(TLANDbuffer, CZTDTGTU.GETTD);

		sprintf(uplinkdata, "%s%s", TLANDbuffer, buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Descent target load, LGC abort constants");
		}
	}
	break;
	case 70: //PDI PAD
	{
		AP11PDIPAD * form = (AP11PDIPAD *)pad;

		PDIPADOpt opt;
		SV sv;

		sv = StateVectorCalc(calcParams.tgt);

		if (mcc->MoonRev >= 14)
		{
			opt.direct = true;
		}
		else
		{
			opt.direct = false;
			opt.dV_LVLH = DeltaV_LVLH;
			opt.P30TIG = TimeofIgnition;
		}
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
		opt.R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.rad[RTCC_LMPOS_BEST]);
		opt.sv0 = sv;
		opt.t_land = CZTDTGTU.GETTD;

		PDI_PAD(&opt, *form);
	}
	break;
	case 71: //PDI ABORT PAD
	{
		AP12PDIABORTPAD * form = (AP12PDIABORTPAD *)pad;

		SV sv;
		double t_sunrise1, t_sunrise2, t_TPI;

		sv = StateVectorCalc(calcParams.src);

		t_sunrise1 = calcParams.PDI + 3.0*3600.0;
		t_sunrise2 = calcParams.PDI + 5.0*3600.0;

		if (mcc->MoonRev >= 14)
		{
			//Find one TPI opportunity (PDI-2)
			t_TPI = FindOrbitalSunrise(sv, t_sunrise1) - 23.0*60.0;
			form->T_TPI_Pre10Min = form->T_TPI_Post10Min = round(t_TPI);
		}
		else
		{
			//Find two TPI opportunities (PDI-1)
			t_TPI = FindOrbitalSunrise(sv, t_sunrise1) - 23.0*60.0;
			form->T_TPI_Pre10Min = round(t_TPI);
			t_TPI = FindOrbitalSunrise(sv, t_sunrise2) - 23.0*60.0;
			form->T_TPI_Post10Min = round(t_TPI);
		}
	}
	break;
	case 72: //No PDI+12 PAD
	{
		AP11LMMNV * form = (AP11LMMNV*)pad;

		LambertMan opt;
		AP11LMManPADOpt manopt;
		TwoImpulseResuls res;
		SV sv_LM, sv_INP, sv_CSM;
		VECTOR3 dV_LVLH;
		double t_sunrise, t_CSI, t_TPI, dt, P30TIG, t_P;

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);

		t_sunrise = calcParams.PDI + 3.0*3600.0;
		t_TPI = FindOrbitalSunrise(sv_CSM, t_sunrise) - 23.0*60.0;

		GZGENCSN.TIElevationAngle = 26.6*RAD;

		if (mcc->MoonRev >= 14)
		{
			sv_INP = sv_LM;
		}
		else
		{
			sv_INP = ExecuteManeuver(sv_LM, TimeofIgnition, DeltaV_LVLH, 0.0, RTCC_ENGINETYPE_LMDPS);
		}
		opt.mode = 1;
		opt.axis = RTCC_LAMBERT_MULTIAXIS;
		opt.DH = 15.0*1852.0;
		opt.ElevationAngle = 26.6*RAD;
		opt.N = 0;
		opt.Perturbation = RTCC_LAMBERT_PERTURBED;
		//Angle confirmed by Apollo 11 FIDO loop (finally!)
		opt.PhaseAngle = 4.475*RAD;
		opt.sv_A = sv_INP;
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

		t_P = OrbMech::period(sv_INP.R, sv_INP.V + res.dV, OrbMech::mu_Moon);
		t_CSI = opt.T2 - t_P / 2.0;

		PoweredFlightProcessor(sv_INP, opt.T1, RTCC_ENGINETYPE_LMDPS, 0.0, res.dV, false, P30TIG, dV_LVLH);
		//Store for P76 PAD
		calcParams.TIGSTORE1 = P30TIG;
		calcParams.DVSTORE1 = dV_LVLH;

		manopt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_LMDPS;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
		manopt.RV_MCC = sv_INP;
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
		AP12LunarSurfaceDataCard * form = (AP12LunarSurfaceDataCard*)pad;

		SV sv_CSM, sv_Ins, sv_IG;
		VECTOR3 R_LS, R_C1, V_C1, u, V_C1F, R_CSI1, V_CSI1;
		double T2, GETbase, m0, v_LH, v_LV, theta, dt_asc, t_C1, dt1, dt2, t_CSI1, t_sunrise, t_TPI, dv;

		GETbase = CalcGETBase();
		sv_CSM = StateVectorCalc(calcParams.src);
		
		LEM *l = (LEM*)calcParams.tgt;
		m0 = l->GetAscentStageMass();

		v_LH = 5513.5*0.3048;
		v_LV = 19.5*0.3048;

		R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.rad[RTCC_LMPOS_BEST]);

		if (mcc->MoonRev >= 14)
		{
			T2 = calcParams.PDI + 21.0*60.0;
			t_sunrise = calcParams.PDI + 5.0*3600.0;
		}
		else
		{
			T2 = calcParams.PDI + 21.0*60.0 + 22.0;
			t_sunrise = calcParams.PDI + 7.0*3600.0;
		}

		LunarAscentProcessor(R_LS, m0, sv_CSM, T2, v_LH, v_LV, theta, dt_asc, dv, sv_IG, sv_Ins);
		dt1 = OrbMech::timetoapo(sv_Ins.R, sv_Ins.V, OrbMech::mu_Moon);
		OrbMech::rv_from_r0v0(sv_Ins.R, sv_Ins.V, dt1, R_C1, V_C1, OrbMech::mu_Moon);
		t_C1 = T2 + dt_asc + dt1;
		u = unit(crossp(R_C1, V_C1));
		V_C1F = V_C1 + unit(crossp(u, V_C1))*10.0*0.3048;
		OrbMech::REVUP(R_C1, V_C1F, 1.5, OrbMech::mu_Moon, R_CSI1, V_CSI1, dt2);
		t_CSI1 = t_C1 + dt2;

		t_TPI = FindOrbitalSunrise(sv_CSM, t_sunrise) - 23.0*60.0;
		//Round to next 30 seconds
		t_TPI = round(t_TPI / 30.0)*30.0;

		form->T2_TIG = T2;
		form->T2_t_TPI = round(t_TPI);

		//T3
		LunarLiftoffTimeOpt opt;
		SV sv_CSM2, sv_CSM_over;
		double theta_1, dt_1, t_L;

		//Calculate TPI time for T3
		t_sunrise = calcParams.PDI + 5.0*3600.0;
		t_TPI = FindOrbitalSunrise(sv_CSM, t_sunrise) - 23.0*60.0;
		//Round to next 30 seconds
		t_TPI = round(t_TPI / 30.0)*30.0;

		//Set all the options
		opt.I_BURN = 1;
		opt.I_TPI = 2;
		opt.I_CDH = 1;
		opt.I_SRCH = 0;
		opt.I_OS = 0;
		opt.M = 2;
		opt.P = 1;
		opt.L_DH = 3;
		opt.t_max = 9.0*3600.0;
		opt.H_S = 5.0*1852.0;
		opt.DV_MAX[0] = opt.DV_MAX[1] = 430.0*0.3048;
		opt.DH[0] = 10.0*1852.0;
		opt.DH[1] = 15.0*1852.0;
		opt.DH[2] = 20.0*1852.0;
		opt.v_LH = 5535.6*0.3048;
		opt.v_LV = 32.0*0.3048;
		opt.h_BO = 60000.0*0.3048;
		opt.Y_S = 0.5*RAD;
		opt.DT_B = 50.0*60.0;
		opt.theta_F = 130.0*RAD;
		opt.E = 26.6*RAD;
		opt.DH_OFF = 0.0;
		opt.dTheta_OFF = 0.0;
		opt.t_hole = GMTfromGET(t_TPI);
		opt.lat = BZLAND.lat[0];
		opt.lng = BZLAND.lng[0];
		opt.R_LLS = BZLAND.rad[0];
		opt.sv_CSM = sv_CSM;
		opt.theta_1 = 9.9588*RAD;
		opt.dt_1 = 447.0;

		//Initial pass through the processor
		LunarLaunchWindowProcessor(opt);
		//Refine ascent parameters
		LunarAscentProcessor(R_LS, m0, sv_CSM, PZLRPT.data[1].GETLO, opt.v_LH, opt.v_LV, theta_1, dt_1, dv, sv_IG, sv_Ins);
		opt.theta_1 = theta_1;
		opt.dt_1 = dt_1;
		//Final pass through
		LunarLaunchWindowProcessor(opt);

		t_L = PZLRPT.data[1].GETLO;
		form->T3_TIG = round(t_L);
	}
	break;
    case 75: //LM SV AND RLS UPDATE
	{
		EphemerisData sv;
		char buffer1[1000];
		char buffer2[100];

		sv = StateVectorCalcEphem(calcParams.tgt);

		AGCStateVectorUpdate(buffer1, 2, RTCC_MPT_LM, sv);
		LandingSiteUplink(buffer2, RTCC_MPT_LM);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector, RLS");
		}
	}
	break;
	case 400: //LGC CSM STATE VECTOR UPDATE WITH TIME TAG DOI-10 MINUTES
	case 401: //LGC CSM STATE VECTOR UPDATE WITH TIME TAG AT LIFTOFF
	{
		EphemerisData sv, sv_upl;
		char buffer1[1000];
		double t_Tag;

		sv = StateVectorCalcEphem(calcParams.src); //State vector for uplink

		if (fcn == 400)
		{
			t_Tag = calcParams.DOI - 10.0*60.0;
		}
		else
		{
			t_Tag = calcParams.LunarLiftoff;
		}

		//Make sure no SV in the past is uplinked
		if (t_Tag < GETfromGMT(RTCCPresentTimeGMT()))
		{
			sv_upl = sv;
		}
		else
		{
			sv_upl = coast(sv, t_Tag - GETfromGMT(sv.GMT));
		}

		AGCStateVectorUpdate(buffer1, RTCC_MPT_LM, RTCC_MPT_CSM, sv_upl);

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

		SV sv_CSM, sv_LM, sv_CSM_upl, sv_LM_upl;
		double GETbase;

		char buffer1[1000];
		char buffer2[1000];

		GETbase = CalcGETBase();

		//State vectors
		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);

		//Coast to DOI-10min
		if (mcc->MoonRev >= 14)
		{
			sv_CSM_upl = sv_CSM;
			sv_LM_upl = sv_LM;
		}
		else
		{
			sv_CSM_upl = coast(sv_CSM, calcParams.DOI - 10.0*60.0 - OrbMech::GETfromMJD(sv_CSM.MJD, GETbase));
			sv_LM_upl = coast(sv_LM, calcParams.DOI - 10.0*60.0 - OrbMech::GETfromMJD(sv_LM.MJD, GETbase));
		}

		form->entries = 2;
		sprintf(form->purpose[0], "DOI");
		form->TIG[0] = TimeofIgnition;
		form->DV[0] = DeltaV_LVLH / 0.3048;
		sprintf(form->purpose[1], "No PDI +12");
		form->TIG[1] = calcParams.TIGSTORE1;
		form->DV[1] = calcParams.DVSTORE1 / 0.3048;

		AGCStateVectorUpdate(buffer1, sv_CSM_upl, true);
		AGCStateVectorUpdate(buffer2, sv_LM_upl, false);

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
		sprintf(upMessage, "Good landing, Pete! Outstanding, man!");
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

		//Update landing site
		calcParams.tgt->GetEquPos(BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.rad[RTCC_LMPOS_BEST]);
	}
	break;
	case 85: //LIFTOFF TIME UPDATE REV 16 THRU 19
	case 86: //LIFTOFF TIME UPDATE REV 20 THRU 24
	case 87: //LIFTOFF TIME UPDATE REV 25 THRU 28
	case 88: //LIFTOFF TIME UPDATE REV 29 & 30
	{
		LIFTOFFTIMES * form = (LIFTOFFTIMES*)pad;

		LunarLiftoffTimeOpt opt;
		SV sv_CSM, sv_CSM_upl, sv_Ins, sv_IG;
		VECTOR3 R_LS;
		double GETbase, m0, theta_1, dt_1, dv, t_TPI_guess, GET_CSM;

		GETbase = CalcGETBase();
		sv_CSM = StateVectorCalc(calcParams.src);

		LEM *l = (LEM*)calcParams.tgt;
		m0 = l->GetAscentStageMass();

		GET_CSM = OrbMech::GETfromMJD(sv_CSM.MJD, GETbase);

		//Set all the options
		opt.I_BURN = 1;
		opt.I_TPI = 2;
		opt.I_CDH = 1;
		opt.I_SRCH = 0;
		opt.I_OS = 0;
		opt.M = 2;
		opt.P = 1;
		opt.L_DH = 3;
		opt.t_max = 9.0*3600.0;
		opt.H_S = 5.0*1852.0;
		opt.DV_MAX[0] = opt.DV_MAX[1] = 430.0*0.3048;
		opt.DH[0] = 10.0*1852.0;
		opt.DH[1] = 15.0*1852.0;
		opt.DH[2] = 20.0*1852.0;
		opt.v_LH = 5535.6*0.3048;
		opt.v_LV = 32.0*0.3048;
		opt.h_BO = 60000.0*0.3048;
		opt.Y_S = 0.5*RAD;
		opt.DT_B = 50.0*60.0;
		opt.theta_F = 130.0*RAD;
		opt.E = 26.6*RAD;
		opt.DH_OFF = 0.0;
		opt.dTheta_OFF = 0.0;
		opt.lat = BZLAND.lat[0];
		opt.lng = BZLAND.lng[0];
		opt.R_LLS = BZLAND.rad[0];
		opt.sv_CSM = sv_CSM;
		opt.theta_1 = 9.9588*RAD;
		opt.dt_1 = 447.0;

		if (fcn == 85)
		{
			t_TPI_guess = GET_CSM + 5.0*3600.0;
			form->entries = 4;
			form->startdigit = 4;
		}
		else if (fcn == 86)
		{
			t_TPI_guess = GET_CSM + 5.5*3600.0;
			form->entries = 5;
			form->startdigit = 8;
		}
		else if (fcn == 87)
		{
			t_TPI_guess = GET_CSM + 4.75*3600.0;
			form->entries = 4;
			form->startdigit = 13;
		}
		else if (fcn == 88)
		{
			t_TPI_guess = GET_CSM + 4.5*3600.0;
			form->entries = 2;
			form->startdigit = 17;
		}

		R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.rad[RTCC_LMPOS_BEST]);

		double t_TPI;
		t_TPI = FindOrbitalSunrise(sv_CSM, t_TPI_guess) - 23.0*60.0;
		//Round to next 30 seconds
		t_TPI = round(t_TPI / 30.0)*30.0;
		opt.t_hole = GMTfromGET(t_TPI);

		//Initial pass through the processor
		LunarLaunchWindowProcessor(opt);
		//Refine ascent parameters
		LunarAscentProcessor(R_LS, m0, sv_CSM, PZLRPT.data[1].GETLO, opt.v_LH, opt.v_LV, theta_1, dt_1, dv, sv_IG, sv_Ins);
		opt.theta_1 = theta_1;
		opt.dt_1 = dt_1;

		for (int i = 0;i < form->entries;i++)
		{
			LunarLaunchWindowProcessor(opt);
			form->TIG[i] = PZLRPT.data[1].GETLO;

			t_TPI_guess += 2.0*3600.0;
			t_TPI = FindOrbitalSunrise(sv_CSM, t_TPI_guess) - 23.0*60.0;
			//Round to next 30 seconds
			t_TPI = round(t_TPI / 30.0)*30.0;
			opt.t_hole = GMTfromGET(t_TPI);
		}
	}
	break;
	case 93: //PLANE CHANGE 1 EVALUATION
	{
		SV sv_CSM, sv_Liftoff;
		VECTOR3 R_LS;
		double TIG_nom, GETbase, MJD_TIG_nom, dt1, LmkRange;

		sv_CSM = StateVectorCalc(calcParams.src);
		GETbase = CalcGETBase();

		//Initial guess for liftoff time
		calcParams.LunarLiftoff = calcParams.LOI + 58.6*3600.0;
		TIG_nom = calcParams.LunarLiftoff;
		MJD_TIG_nom = OrbMech::MJDfromGET(TIG_nom, GETbase);
		sv_Liftoff = coast(sv_CSM, (MJD_TIG_nom - sv_CSM.MJD)*24.0*3600.0);

		R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.rad[RTCC_LMPOS_BEST]);

		dt1 = OrbMech::findelev_gs(SystemParameters.AGCEpoch, SystemParameters.MAT_J2000_BRCS, sv_Liftoff.R, sv_Liftoff.V, R_LS, MJD_TIG_nom, 180.0*RAD, sv_Liftoff.gravref, LmkRange);

		if (abs(LmkRange) < 8.0*1852.0)
		{
			sprintf(upMessage, "Plane Change has been scrubbed");
			scrubbed = true;
		}
	}
	break;
	case 94: //PLANE CHANGE 1 TARGETING
	case 95: //PLANE CHANGE 2 TARGETING
	{
		SV sv;
		double GET_SV, THT;

		AP11MNV * form = (AP11MNV*)pad;

		AP11ManPADOpt manopt;

		REFSMMATOpt refsopt;
		MATRIX3 REFSMMAT;

		char buffer1[1000], buffer2[1000], buffer3[1000], updesc[128];

		sv = StateVectorCalc(calcParams.src);

		GET_SV = OrbMech::GETfromMJD(sv.MJD, CalcGETBase());

		med_k16.GETTH1 = GET_SV + 1.0*3600.0;
		if (fcn == 95)
		{
			//Transcript (AP12FJ) 163:47:14 Carr: "Delta Echo 1 is the next one; 165:24:21, 165:29:17, and it's on track."
			//Descartes coordinates (Delta Echo 1)
			BZLAND.lat[RTCC_LMPOS_BEST] = -8.883*RAD;
			BZLAND.lng[RTCC_LMPOS_BEST] = 15.55*RAD;

			THT = med_k16.GETTH1 + 6.0*3600.0;
		}
		else
		{
			THT = calcParams.LunarLiftoff - 0.5*3600.0;
		}
		med_k16.GETTH2 = med_k16.GETTH3 = med_k16.GETTH4 = THT;
		med_k16.Mode = 7;
		med_k16.Sequence = 1;
		med_k16.Vehicle = RTCC_MPT_CSM;

		GZGENCSN.LDPPAzimuth = 0.0;

		LunarDescentPlanningProcessor(sv);

		PoweredFlightProcessor(sv, PZLDPDIS.GETIG[0], RTCC_ENGINETYPE_CSMSPS, 0.0, PZLDPDIS.DVVector[0] * 0.3048, true, TimeofIgnition, DeltaV_LVLH);

		refsopt.dV_LVLH = DeltaV_LVLH;
		refsopt.HeadsUp = true;
		refsopt.REFSMMATTime = TimeofIgnition;
		refsopt.REFSMMATopt = 0;
		refsopt.vessel = calcParams.src;
		refsopt.vesseltype = 0;

		REFSMMAT = REFSMMATCalc(&refsopt);

		manopt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		manopt.dV_LVLH = DeltaV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = true;
		manopt.REFSMMAT = REFSMMAT;
		manopt.TIG = TimeofIgnition;
		manopt.vessel = calcParams.src;
		manopt.vesseltype = 0;

		if (fcn == 94)
		{
			manopt.UllageThrusterOpt = false;
			manopt.UllageDT = 15.0;
			manopt.sxtstardtime = -35.0*60.0;
		}
		else
		{
			manopt.UllageThrusterOpt = true;
			manopt.UllageDT = 11.0;
			manopt.sxtstardtime = -25.0*60.0;
		}

		AP11ManeuverPAD(&manopt, *form);

		if (fcn == 94)
		{
			sprintf(form->purpose, "PLANE CHANGE 1");
			sprintf(form->remarks, "Ullage: 2 jets, 15 seconds");
			sprintf(updesc, "CSM state vector, target load, PC REFSMMAT");
			AGCStateVectorUpdate(buffer1, sv, true);
		}
		else
		{
			sprintf(form->purpose, "PLANE CHANGE 2");
			sprintf(form->remarks, "Ullage: 4 jets, 11 seconds");
			sprintf(updesc, "CSM state vector and V66, target load, PC REFSMMAT");
			AGCStateVectorUpdate(buffer1, sv, true, true);
		}

		CMCExternalDeltaVUpdate(buffer2, TimeofIgnition, DeltaV_LVLH);
		AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);

		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, updesc);
		}
	}
	break;
	case 96: //CMC LUNAR LIFTOFF REFSMMAT UPLINK
	{
		REFSMMATOpt opt;
		MATRIX3 REFSMMAT;
		char buffer[1000];

		opt.LSLat = BZLAND.lat[RTCC_LMPOS_BEST];
		opt.LSLng = BZLAND.lng[RTCC_LMPOS_BEST];
		opt.REFSMMATopt = 5;
		opt.REFSMMATTime = calcParams.LunarLiftoff;
		opt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&opt);
		AGCDesiredREFSMMATUpdate(buffer, REFSMMAT);

		sprintf(uplinkdata, "%s", buffer);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Liftoff REFSMMAT");
		}
	}
	break;
	case 100: //NOMINAL INSERTION TARGET + CMC LM SV UPDATE
	{
		LunarLiftoffTimeOpt opt;
		REFSMMATOpt refsopt;
		SV sv_CSM, sv_LM_upl, sv_Ins, sv_IG;
		MATRIX3 REFSMMAT;
		VECTOR3 R_LS;
		char buffer1[64], buffer2[100];
		double GETbase, m0, theta_1, dt_1, dv, t_TPI_guess, t_TPI;

		GETbase = CalcGETBase();
		sv_CSM = StateVectorCalc(calcParams.src);

		LEM *l = (LEM*)calcParams.tgt;
		m0 = l->GetAscentStageMass();

		//Rev 29 is the nominal for this update. Nominal TPI time is about 34.25 hours after PDI
		t_TPI_guess = calcParams.PDI + 34.25*3600.0 + 23.0*60.0 + 2.0*3600.0*(double)(mcc->MoonRev - 29);
		t_TPI = FindOrbitalSunrise(sv_CSM, t_TPI_guess) - 23.0*60.0;
		//Round to next 30 seconds
		t_TPI = round(t_TPI / 30.0)*30.0;

		//Set all the options
		opt.I_BURN = 1;
		opt.I_TPI = 2;
		opt.I_CDH = 1;
		opt.I_SRCH = 0;
		opt.I_OS = 0;
		opt.M = 2;
		opt.P = 1;
		opt.L_DH = 3;
		opt.t_max = 9.0*3600.0;
		opt.H_S = 5.0*1852.0;
		opt.DV_MAX[0] = opt.DV_MAX[1] = 430.0*0.3048;
		opt.DH[0] = 10.0*1852.0;
		opt.DH[1] = 15.0*1852.0;
		opt.DH[2] = 20.0*1852.0;
		opt.v_LH = 5533.9*0.3048;
		opt.v_LV = 34.4*0.3048;
		opt.h_BO = 60000.0*0.3048;
		opt.Y_S = 0.5*RAD;
		opt.DT_B = 50.0*60.0;
		opt.theta_F = 130.0*RAD;
		opt.E = 26.6*RAD;
		opt.DH_OFF = 0.0;
		opt.dTheta_OFF = 0.0;
		opt.lat = BZLAND.lat[0];
		opt.lng = BZLAND.lng[0];
		opt.R_LLS = BZLAND.rad[0];
		opt.sv_CSM = sv_CSM;
		opt.theta_1 = 9.9588*RAD;
		opt.dt_1 = 447.0;
		opt.t_hole = GMTfromGET(t_TPI);

		R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.rad[RTCC_LMPOS_BEST]);

		//Initial pass through the processor
		LunarLaunchWindowProcessor(opt);
		//Refine ascent parameters
		LunarAscentProcessor(R_LS, m0, sv_CSM, PZLRPT.data[1].GETLO, opt.v_LH, opt.v_LV, theta_1, dt_1, dv, sv_IG, sv_Ins);
		opt.theta_1 = theta_1;
		opt.dt_1 = dt_1;
		//Final pass through
		LunarLaunchWindowProcessor(opt);

		calcParams.LunarLiftoff = PZLRPT.data[1].GETLO;
		calcParams.Insertion = PZLRPT.data[1].T_INS;
		calcParams.CSI = PZLRPT.data[1].T_CSI;
		calcParams.CDH = PZLRPT.data[1].T_CDH;
		calcParams.TPI = PZLRPT.data[1].T_TPI;

		//Calculate Liftoff REFSMMAT
		refsopt.LSLat = BZLAND.lat[RTCC_LMPOS_BEST];
		refsopt.LSLng = BZLAND.lng[RTCC_LMPOS_BEST];
		refsopt.REFSMMATopt = 5;
		refsopt.REFSMMATTime = calcParams.LunarLiftoff;
		refsopt.vessel = calcParams.src;
		REFSMMAT = REFSMMATCalc(&refsopt);

		//Store as LM LLD
		EMGSTSTM(RTCC_MPT_LM, REFSMMAT, RTCC_REFSMMAT_TYPE_LLD, RTCCPresentTimeGMT());
		//Also move to CSM LCV
		GMGMED("G00,LEM,LLD,CSM,LCV;");

		//Store for Ascent PAD
		DeltaV_LVLH = _V(opt.v_LH, opt.v_LV, 0);
		//Store for CSI PAD
		calcParams.DVSTORE1 = _V(PZLRPT.data[1].DVCSI*0.3048, 0, 0);
		calcParams.SVSTORE1 = sv_Ins;

		sv_LM_upl = coast(sv_Ins, PZLRPT.data[1].T_INS + 18.0*60.0 - OrbMech::GETfromMJD(sv_Ins.MJD, GETbase));

		sprintf(buffer1, "V45E");
		AGCStateVectorUpdate(buffer2, sv_LM_upl, false);

		//Calculate T19
		opt.t_hole += 2.0*3600.0;
		LunarLaunchWindowProcessor(opt);
		//Store for Ascent PAD
		TimeofIgnition = PZLRPT.data[1].GETLO;

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Reset surface flag, LM state vector");
		}
	}
	break;
	case 101: //CMC INSERTION STATE VECTORS
	case 102: //LGC CSM STATE VECTORS + RLS
	{
		SV sv_LM_upl, sv_CSM, sv_CSM_upl;
		double GETbase;
		char buffer1[100], buffer2[100];

		GETbase = CalcGETBase();

		sv_CSM = StateVectorCalc(calcParams.src);

		sv_CSM_upl = coast(sv_CSM, calcParams.Insertion + 18.0*60.0 - OrbMech::GETfromMJD(sv_CSM.MJD, GETbase));
		sv_LM_upl = coast(calcParams.SVSTORE1, calcParams.Insertion + 18.0*60.0 - OrbMech::GETfromMJD(calcParams.SVSTORE1.MJD, GETbase));

		AGCStateVectorUpdate(buffer1, sv_CSM_upl, true);
		AGCStateVectorUpdate(buffer2, sv_LM_upl, false);

		if (fcn == 101)
		{
			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM and LM state vectors");
			}
		}
		else
		{
			char buffer3[100];
			LandingSiteUplink(buffer3, RTCC_MPT_LM);

			sprintf(uplinkdata, "%s%s", buffer1, buffer3);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, RLS");
			}
		}
	}
	break;
	case 105: //NOMINAL LM ASCENT PAD
	{
		AP12LMASCPAD * form = (AP12LMASCPAD*)pad;

		ASCPADOpt opt;
		double m0, m1;

		//Definitions
		opt.TIG = calcParams.LunarLiftoff;
		opt.v_LH = DeltaV_LVLH.x;
		opt.v_LV = DeltaV_LVLH.y;

		opt.sv_CSM = StateVectorCalcEphem(calcParams.src);
		opt.R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.rad[RTCC_LMPOS_BEST]);

		LEM *l = (LEM*)calcParams.tgt;
		m0 = l->GetAscentStageMass();
		m1 = calcParams.src->GetMass();

		opt.Rot_VL = OrbMech::GetVesselToLocalRotMatrix(calcParams.tgt);

		// Calculate ascent PAD
		double KFactor;

		int hh, mm;
		double ss;

		bool res_k = CalculateAGSKFactor(&l->agc.vagc, &l->aea.vags, KFactor);
		if (res_k)
		{
			SystemParameters.MCGZSS = SystemParameters.MCGZSL + KFactor / 3600.0;
		}

		OrbMech::SStoHHMMSS(GETfromGMT(GetAGSClockZero()), hh, mm, ss, 0.01);

		AP11LMASCPAD pad2;

		LunarAscentPAD(opt, pad2);

		form->CR = pad2.CR;
		form->TIG = pad2.TIG;
		form->V_hor = pad2.V_hor;
		form->V_vert = form->DEDA465 = pad2.V_vert;
		form->DEDA047 = pad2.DEDA047;
		form->DEDA053 = pad2.DEDA053;
		form->DEDA225_226 = pad2.DEDA225_226;
		form->DEDA231 = pad2.DEDA231;
		form->TIG_2 = TimeofIgnition;
		form->LMWeight = m0 / 0.45359237;
		form->CSMWeight = m1 / 0.45359237;
		sprintf(form->remarks, "K-Factor is %d:%02d:%05.2f", hh, mm, ss);
	}
	break;
	case 106: //NOMINAL CSI PAD
	{
		AP10CSI * form = (AP10CSI*)pad;

		AP10CSIPADOpt opt;

		opt.dV_LVLH = calcParams.DVSTORE1;
		opt.enginetype = RTCC_ENGINETYPE_LMRCSPLUS4;
		opt.KFactor = GETfromGMT(GetAGSClockZero());
		opt.REFSMMAT = EZJGMTX3.data[RTCC_REFSMMAT_TYPE_LLD - 1].REFSMMAT;
		opt.sv0 = calcParams.SVSTORE1;
		opt.t_CSI = calcParams.CSI;
		opt.t_TPI = calcParams.TPI;

		AP10CSIPAD(&opt, *form);
		form->type = 1;
	}
	break;
	case 107: //LM LIFTOFF EVALUATION
	{
		if (calcParams.tgt->GroundContact())
		{
			sprintf(upMessage, "Launch scrubbed, recycle for next rev");
			scrubbed = true;
		}
	}
	break;
	case 110: //SEP BURN & LM JETT PAD + CMC SV UPLINK
	{
		AP11ManPADOpt opt;
		SV sv_CSM, sv_LM;
		VECTOR3 dV_LVLH;
		double t_Sep;
		char buffer1[100], buffer2[100];

		int hh, mm;
		double ss;

		AP11MNV * form = (AP11MNV *)pad;

		t_Sep = calcParams.LunarLiftoff + OrbMech::HHMMSSToSS(6, 0, 42);
		calcParams.SEP = t_Sep;
		dV_LVLH = _V(0, 0, -1.0)*0.3048;

		opt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_CSMRCSPLUS4;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		opt.TIG = t_Sep;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "SEP BURN");
		OrbMech::SStoHHMMSS(form->GETI - 5.0*60.0, hh, mm, ss, 0.01);
		sprintf(form->remarks, "Jettison PAD: GET %d:%d:%.2lf R 219 P 358 Y 342\nSep burn is Z-axis, retrograde", hh, mm, ss);
		form->type = 2;

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
	case 111: //LM IMPACT BURN PAD + UPLINK
	{
		AP11LMManPADOpt opt;

		SV sv, sv2;
		char buffer1[1000];
		char buffer2[1000];

		AP11LMMNV * form = (AP11LMMNV *)pad;

		sv = StateVectorCalc(calcParams.tgt);

		TimeofIgnition = calcParams.LunarLiftoff + OrbMech::HHMMSSToSS(7, 23, 23);
		DeltaV_LVLH = _V(-181.2, 60.3, -1.5)*0.3048;

		opt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
		opt.csmlmdocked = false;
		opt.dV_LVLH = DeltaV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_LMRCSPLUS4;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, false);
		opt.TIG = TimeofIgnition;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);
		sprintf(form->purpose, "IMPACT PAD");

		//Save cutoff time for MCC
		calcParams.TIGSTORE1 = TimeofIgnition + form->burntime - 1.0;

		AGCStateVectorUpdate(buffer1, sv, false);
		LGCExternalDeltaVUpdate(buffer2, TimeofIgnition, DeltaV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector, Impact burn target load");
		}
	}
	break;
	case 112: //CSM P76 PAD
	{
		AP11P76PAD * form = (AP11P76PAD*)pad;

		form->entries = 1;
		sprintf(form->purpose[0], "CSM P76");
		form->TIG[0] = TimeofIgnition;
		form->DV[0] = DeltaV_LVLH / 0.3048;
	}
	break;
	case 120: //P42 GROUND COMMAND
	case 121: //PRO
	case 122: //ENTER
	case 123: //COMMAND ULLAGE OFF
	{
		char updesc[128];

		if (fcn == 120)
		{
			sprintf(uplinkdata, "V37E42ER");
			sprintf(updesc, "LM: P42 APS thrusting");
		}
		else if (fcn == 121)
		{
			sprintf(uplinkdata, "V33ER");
			sprintf(updesc, "");
		}
		else if (fcn == 122)
		{
			sprintf(uplinkdata, "ER");
			sprintf(updesc, "");
		}
		else if (fcn == 123)
		{
			sprintf(uplinkdata, "ER");
			sprintf(updesc, "LM: Command ullage off");
		}

		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, updesc);
		}
	}
	break;
	case 130: //PHOTOGRAPHY REFSMMAT CACLULATION
	case 131: //PHOTOGRAPHY REFSMMAT UPLINK
	{
		MATRIX3 REFSMMAT;

		if (fcn == 130)
		{
			//Hard-coded from page 59 of "Final operational spacecraft attitude sequence for Apollo 12" (69-FM-304)
			//TBD: Calculate this dynamically. It may have been a landing site orientation at one of the photo sites, more research required.
			REFSMMAT = _M(-0.99920070, -0.00131239, -0.03995296, -0.03280089, -0.54435646, 0.83821248, -0.02284871, 0.83885300, 0.54387832);

			//Store as LM LCV matrix
			EMGSTSTM(RTCC_MPT_CSM, REFSMMAT, RTCC_REFSMMAT_TYPE_LCV, RTCCPresentTimeGMT());
		}
		else
		{
			char buffer1[1000];

			REFSMMAT = EZJGMTX1.data[RTCC_REFSMMAT_TYPE_LCV - 1].REFSMMAT;

			AGCDesiredREFSMMATUpdate(buffer1, REFSMMAT, true);
			sprintf(uplinkdata, "%s", buffer1);

			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "Photography REFSMMAT");
			}
		}
	}
	break;
	case 132: //TEI REFSMMAT UPLINK
	{
		MATRIX3 REFSMMAT;

		char buffer1[1000];

		REFSMMAT = EZJGMTX1.data[RTCC_REFSMMAT_TYPE_LCV - 1].REFSMMAT;

		AGCDesiredREFSMMATUpdate(buffer1, REFSMMAT, true);
		sprintf(uplinkdata, "%s", buffer1);

		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "TEI REFSMMAT");
		}
	}
	break;
	case 200: //TEI EVALUATION
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
	case 205: //EI Evaluation
	case 210: //MCC-5 UPDATE
	case 212: //MCC-6 UPDATE
	case 213: //MCC-7 DECISION
	case 214: //MCC-7 UPDATE
	case 300: //GENERIC MCC
	{
		EntryOpt entopt;
		EntryResults res;
		double MCCtime;
		char manname[8];
		SV sv;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		if (fcn == 210)
		{
			MCCtime = calcParams.TEI + 15.0*3600.0;
			sprintf(manname, "MCC-5");
		}
		else if (fcn == 211 || fcn == 212)
		{
			MCCtime = calcParams.EI - 22.0*3600.0;
			sprintf(manname, "MCC-6");
		}
		else if (fcn == 213 || fcn == 214)
		{
			MCCtime = calcParams.EI - 3.0*3600.0;
			sprintf(manname, "MCC-7");
		}
		else if (fcn == 300)
		{
			MCCtime = calcParams.TEI + 5.0*3600.0;
			sprintf(manname, "MCC");
		}
		else
		{
			if (AGCGravityRef(calcParams.src) == oapiGetObjectByName("Moon"))
			{
				sv = coast(sv, 20.0*3600.0);
			}

			MCCtime = OrbMech::GETfromMJD(sv.MJD, CalcGETBase());
		}

		entopt.entrylongmanual = false;
		entopt.ATPLine = 0; //MPL
		entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		entopt.RV_MCC = sv;
		entopt.TIGguess = MCCtime;
		entopt.vessel = calcParams.src;
		entopt.r_rbias = PZREAP.RRBIAS;

		if (calcParams.src->DockingStatus(0) == 1)
		{
			entopt.csmlmdocked = true;
		}
		else
		{
			entopt.csmlmdocked = false;
		}

		entopt.type = 3;
		EntryTargeting(&entopt, &res);

		if (fcn != 205)
		{
			AP11ManPADOpt opt;
			MATRIX3 REFSMMAT;

			AP11MNV * form = (AP11MNV *)pad;

			//Longitude control before EI-24h
			if (MCCtime < res.GET400K - 24.0*3600.0)
			{
				entopt.type = 1;
				entopt.t_Z = res.GET400K;
				EntryTargeting(&entopt, &res);
			}

			//Apollo 11 Mission Rules
			if (MCCtime > res.GET400K - 50.0*3600.0)
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

			if (fcn != 213)
			{
				if (fcn == 214)
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

				if (scrubbed)
				{
					//Entry prediction without maneuver
					EntryUpdateCalc(sv, PZREAP.RRBIAS, true, &res);

					res.dV_LVLH = _V(0, 0, 0);
					res.P30TIG = entopt.TIGguess;
				}
				else
				{
					opt.dV_LVLH = res.dV_LVLH;
					opt.enginetype = SPSRCSDecision(SPS_THRUST / (calcParams.src->GetMass() + GetDockedVesselMass(calcParams.src)), res.dV_LVLH);
					opt.HeadsUp = false;
					opt.REFSMMAT = REFSMMAT;
					opt.TIG = res.P30TIG;
					opt.vessel = calcParams.src;

					if (calcParams.src->DockingStatus(0) == 1)
					{
						opt.vesseltype = 1;
					}
					else
					{
						opt.vesseltype = 0;
					}

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
				if (fcn == 210 || fcn == 212)
				{
					char buffer1[1000];
					char buffer2[1000];

					sprintf(upMessage, "%s has been scrubbed", manname);
					sprintf(upDesc, "CSM state vector and V66, entry target");

					AGCStateVectorUpdate(buffer1, sv, true, true);
					CMCEntryUpdate(buffer2, res.latitude, res.longitude);

					sprintf(uplinkdata, "%s%s", buffer1, buffer2);
					if (upString != NULL) {
						// give to mcc
						strncpy(upString, uplinkdata, 1024 * 3);
					}
				}
				//MCC-6 and 7 decision
				else if (fcn == 211 || fcn == 213 || fcn == 300)
				{
					sprintf(upMessage, "%s has been scrubbed", manname);
				}
				//Scrubbed MCC-7
				else if (fcn == 214)
				{
					char buffer1[1000];
					char buffer2[1000];
					char buffer3[1000];

					sprintf(upMessage, "%s has been scrubbed", manname);
					sprintf(upDesc, "CSM state vector and V66, entry target, Entry REFSMMAT");

					AGCStateVectorUpdate(buffer1, sv, true, true);
					CMCEntryUpdate(buffer2, res.latitude, res.longitude);
					AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT);

					sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
					if (upString != NULL) {
						// give to mcc
						strncpy(upString, uplinkdata, 1024 * 3);
					}
				}
			}
			else
			{
				//MCC-5 or MCC-6 or Generic MCC
				if (fcn == 210 || fcn == 212 || fcn == 300)
				{
					char buffer1[1000];
					char buffer2[1000];

					AGCStateVectorUpdate(buffer1, sv, true, true);
					CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);

					sprintf(uplinkdata, "%s%s", buffer1, buffer2);
					if (upString != NULL) {
						// give to mcc
						strncpy(upString, uplinkdata, 1024 * 3);
						sprintf(upDesc, "CSM state vector and V66, target load");
					}
				}
				//MCC-6 and 7 decision
				else if (fcn == 211 || fcn == 213)
				{
					sprintf(upMessage, "%s will be executed", manname);
				}
				//MCC-7
				else if (fcn == 214)
				{
					char buffer1[1000];
					char buffer2[1000];
					char buffer3[1000];

					AGCStateVectorUpdate(buffer1, sv, true, true);
					CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);
					AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT);

					sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
					if (upString != NULL) {
						// give to mcc
						strncpy(upString, uplinkdata, 1024 * 3);
						sprintf(upDesc, "CSM state vector and V66, target load, Entry REFSMMAT");
					}
				}
			}

			//Save for further use
			DeltaV_LVLH = res.dV_LVLH;
			TimeofIgnition = res.P30TIG;
			SplashLatitude = res.latitude;
			SplashLongitude = res.longitude;
			calcParams.SVSTORE1 = res.sv_postburn;
		}

		calcParams.EI = res.GET400K;
	}
	break;
	case 216: //ENTRY PAD (ASSUMES MCC-6)
	case 217: //ENTRY PAD (ASSUMES MCC-7)
	case 218: //FINAL LUNAR ENTRY PAD
	{
		AP11ENT * form = (AP11ENT *)pad;

		SV sv;
		LunarEntryPADOpt entopt;
		MATRIX3 REFSMMAT;

		sv = StateVectorCalc(calcParams.src);

		//Burn scrubbed or not
		if (length(DeltaV_LVLH) != 0.0 && fcn != 218)
		{
			entopt.direct = false;
		}
		else
		{
			entopt.direct = true;
		}

		if (fcn == 218)
		{
			REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		}
		else
		{
			REFSMMATOpt refsopt;
			refsopt.REFSMMATopt = 3;
			refsopt.vessel = calcParams.src;
			refsopt.useSV = true;
			refsopt.RV_MCC = calcParams.SVSTORE1;

			REFSMMAT = REFSMMATCalc(&refsopt);
		}

		entopt.dV_LVLH = DeltaV_LVLH;
		entopt.lat = SplashLatitude;
		entopt.lng = SplashLongitude;
		entopt.P30TIG = TimeofIgnition;
		entopt.REFSMMAT = REFSMMAT;
		entopt.sv0 = sv;

		LunarEntryPAD(&entopt, *form);
		sprintf(form->Area[0], "MIDPAC");
		if (entopt.direct == false)
		{
			if (fcn == 216 && length(DeltaV_LVLH) != 0.0)
			{
				sprintf(form->remarks[0], "Assumes MCC6");
			}
			else if (fcn == 217 && length(DeltaV_LVLH) != 0.0)
			{
				sprintf(form->remarks[0], "Assumes MCC7");
			}
		}

		if (fcn == 218)
		{
			char buffer1[1000];
			char buffer2[1000];

			AGCStateVectorUpdate(buffer1, sv, true, true);
			CMCEntryUpdate(buffer2, SplashLatitude, SplashLongitude);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector and V66, entry update");
			}
		}
	}
	break;
	}

	return scrubbed;
}