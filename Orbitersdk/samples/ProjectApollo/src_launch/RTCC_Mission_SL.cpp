/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Calculations for Skylab missions

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
#include "iu.h"
#include "LVDC.h"
#include "mcc.h"
#include "rtcc.h"
#include "../src_skylab/skylab.h"

bool RTCC::CalculationMTP_SL(int fcn, LPVOID &pad, char * upString, char * upDesc, char * upMessage)
{
	char uplinkdata[1024 * 3];
	char Buff[128];
	bool preliminary = false;
	bool scrubbed = false;

	switch (fcn)
	{
	case 1: //MISSION INITIALIZATION
	{
		//P80 MED: mission initialization
		sprintf_s(Buff, "P80,1,CSM,%d,%d,%d;", GZGENCSN.MonthofLiftoff, GZGENCSN.DayofLiftoff, GZGENCSN.Year);
		GMGMED(Buff);

		//P10 MED: Predicted liftoff time

		double iptr, gmt, ss;
		int hh, mm;
		gmt = modf(oapiGetSimMJD(), &iptr)*24.0*3600.0 + 4.0*3600.0; //TBD: Not sure this is good

		OrbMech::SStoHHMMSS(gmt, hh, mm, ss, 0.01);

		sprintf_s(Buff, "P10,CSM,%d:%d:%.2lf;", hh, mm, ss);
		GMGMED(Buff);

		//P15: CMC, clock zero
		sprintf_s(Buff, "P15,AGC,%d:%d:%.2lf;", hh, mm, ss);
		GMGMED(Buff);
	}
	break;
	case 2: //PRELAUNCH TARGETING UPDATE
	{
		//TBD
		sprintf(upMessage, "TBD: PRELAUNCH PAD");

		EphemerisData sv_SWS = StateVectorCalcEphem(calcParams.tgt);
		EphemerisData sv_SWS_upl = coast(sv_SWS, GMTfromGET(20.0*60.0) - sv_SWS.GMT);

		AGCStateVectorUpdate(uplinkdata, 1, RTCC_MPT_LM, sv_SWS_upl);

		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "SWS state vector");
		}
	}
	break;
	case 10: //ON-ORBIT INITIALIZATION
	{
		//P10 MED: Enter actual liftoff time
		double tephem_scal;
		Saturn *cm = (Saturn *)calcParams.src;

		//Get TEPHEM
		tephem_scal = GetTEPHEMFromAGC(&cm->agc.vagc, true);
		double LaunchMJD = (tephem_scal / 8640000.) + SystemParameters.TEPHEM0;
		LaunchMJD = (LaunchMJD - SystemParameters.GMTBASE)*24.0;

		int hh, mm;
		double ss;

		OrbMech::SStoHHMMSS(LaunchMJD*3600.0, hh, mm, ss, 0.01);

		sprintf_s(Buff, "P10,CSM,%d:%d:%.2lf;", hh, mm, ss);
		GMGMED(Buff);

		//P12: CSM GRR and Azimuth
		Saturn1b *Sat1b = (Saturn1b*)cm;
		LVDC1B *lvdc = (LVDC1B*)Sat1b->iu->GetLVDC();
		double Azi = lvdc->Azimuth*DEG;
		double T_GRR = lvdc->T_GRR;

		sprintf_s(Buff, "P12,CSM,%d:%d:%.2lf,%.2lf;", hh, mm, ss, Azi);
		GMGMED(Buff);

		//P15: CMC, clock zero
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
	}
	break;
	case 11: //RENDEZVOUS PLANNING
	{
		DKIOpt opt;

		opt.sv_CSM = StateVectorCalcEphem(calcParams.src);
		opt.sv_LM = StateVectorCalcEphem(calcParams.tgt);
		opt.MV = 1; //CSM maneuvers

		opt.I4 = true;
		opt.DHNCC = 20.0*1852.0;
		opt.DHSR = 10.0*1852.0;
		opt.MI = 5.0; //M = 5 rendezvous
		opt.NH = opt.MI - 1.5;
		opt.NC1 = 2.0;
		opt.NCC = opt.MI - 1.0;
		opt.dt_NCC_NSR = 37.0*60.0;
		opt.NPC = -1.0;
		opt.IPUTNA = 2; //Maneuver line at apogee
		opt.PUTNA = 1.0;
		opt.WT = 130.0*RAD;
		opt.Elev = 27.0*RAD;
		opt.K46 = 4; //TPI at X minutes into light
		opt.TIMLIT = -23.0;

		DockingInitiationProcessor(opt);

		calcParams.Phasing = PZREDT.GET[0]; //NC1 time
		calcParams.Insertion = PZREDT.GET[1]; //NC2 time
		calcParams.CSI = PZREDT.GET[2]; //NCC time
		calcParams.CDH = PZREDT.GET[3]; //NSR time
		calcParams.TPI = PZRPDT.data[0].GETTPI; //TPI time

		char Buff1[64], Buff2[64];
		OrbMech::format_time_HHMMSS(Buff1, calcParams.Phasing);
		OrbMech::format_time_HHMMSS(Buff2, calcParams.TPI);

		sprintf(upMessage, "NC1: %s, TPI: %s", Buff1, Buff2);
	}
	break;
	case 12: //NC1 preliminary update
		preliminary = true;
	case 13: //NC1 final update
	{
		AP7MNV * form = (AP7MNV *)pad;
		AP7ManPADOpt manopt;
		DKIOpt opt;

		opt.sv_CSM = StateVectorCalcEphem(calcParams.src);
		opt.sv_LM = StateVectorCalcEphem(calcParams.tgt);
		opt.MV = 1; //CSM maneuvers

		opt.I4 = true;
		opt.DHNCC = 20.0*1852.0;
		opt.DHSR = 10.0*1852.0;
		opt.MI = 5.0; //M = 5 rendezvous
		opt.NH = opt.MI - 1.5;
		opt.NC1 = 2.0;
		opt.NCC = opt.MI - 1.0;
		opt.dt_NCC_NSR = 37.0*60.0;
		opt.NPC = -1.0;
		opt.IPUTNA = 1; //Maneuver line at input time
		opt.PUTNA = 2.0;
		opt.PUTTNA = GMTfromGET(calcParams.Phasing);
		opt.WT = 130.0*RAD;
		opt.Elev = 27.0*RAD;
		opt.K46 = 1; //Input TPI time
		opt.TTPI = GMTfromGET(calcParams.TPI);

		DockingInitiationProcessor(opt);

		calcParams.Insertion = PZREDT.GET[1]; //NC2 time
		calcParams.CSI = PZREDT.GET[2]; //NCC time
		calcParams.CDH = PZREDT.GET[3]; //NSR time

		manopt.TIG = calcParams.Phasing;
		manopt.dV_LVLH = PZREDT.DVVector[0] * 0.3048;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		manopt.navcheckGET = 0.0;
		manopt.sxtstardtime = 0.0;
		manopt.UllageDT = 20.0;
		manopt.UllageThrusterOpt = false;
		manopt.sv0 = opt.sv_CSM;
		manopt.CSMMass = calcParams.src->GetMass();

		AP7ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "NC1");

		if (preliminary == false)
		{
			char buffer1[1000];
			char buffer2[1000];

			AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, opt.sv_CSM);
			AGCStateVectorUpdate(buffer2, 1, RTCC_MPT_LM, opt.sv_LM);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM & SWS state vectors");
			}
		}
	}
	break;
	case 14: //NC2 preliminary update
		preliminary = true;
	case 15: //NC2 final update
	{
		AP7MNV * form = (AP7MNV *)pad;
		AP7ManPADOpt manopt;
		DKIOpt opt;
		double CSMMass;

		opt.sv_CSM = StateVectorCalcEphem(calcParams.src);
		CSMMass = calcParams.src->GetMass();
		opt.sv_LM = StateVectorCalcEphem(calcParams.tgt);
		opt.MV = 1; //CSM maneuvers

		opt.DHSR = 10.0*1852.0;
		opt.MI = 5.0; //M = 5 rendezvous
		opt.NC1 = opt.MI - 1.5; //NC2 maneuver becomes NC1
		opt.NH = opt.NC1 + 0.5; //NCC maneuver becomes NH
		opt.NSR = opt.NH + 0.4029;
		opt.NPC = -1.0;
		opt.IPUTNA = 1; //Maneuver line at input time
		opt.PUTNA = opt.NC1; //NC2
		opt.PUTTNA = GMTfromGET(calcParams.Insertion);
		opt.WT = 130.0*RAD;
		opt.Elev = 27.0*RAD;
		opt.K46 = 1; //Input TPI time
		opt.TTPI = GMTfromGET(calcParams.TPI);

		DockingInitiationProcessor(opt);

		calcParams.CSI = PZREDT.GET[1]; //NCC time
		calcParams.CDH = calcParams.CSI + 37.0*60.0; //NSR time
		calcParams.LunarLiftoff = PZREDT.PHASE[2] * RAD; //Phase angle at NSR
		calcParams.SVSTORE1 = ConvertEphemDatatoSV(PZDKIELM.Block[0].SV_before[1], CSMMass); //State vector before NCC

		manopt.TIG = calcParams.Insertion;
		manopt.dV_LVLH = PZREDT.DVVector[0] * 0.3048;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		manopt.navcheckGET = 0.0;
		manopt.sxtstardtime = 0.0;
		manopt.UllageDT = 20.0;
		manopt.UllageThrusterOpt = false;
		manopt.sv0 = opt.sv_CSM;
		manopt.CSMMass = CSMMass;

		AP7ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "NC2");
	}
	break;
	case 16: //NCC preliminary update
		preliminary = true;
	case 17: //NCC final update
	{
		AP7MNV * form = (AP7MNV *)pad;
		AP7ManPADOpt manopt;
		TwoImpulseOpt opt;
		TwoImpulseResuls res;
		double CSMMass;

		opt.mode = 5; 
		opt.T1 = GMTfromGET(calcParams.CSI);
		opt.T2 = GMTfromGET(calcParams.CDH);

		if (preliminary)
		{
			opt.sv_A = ConvertSVtoEphemData(calcParams.SVSTORE1);
			CSMMass = calcParams.SVSTORE1.mass;
		}
		else
		{
			opt.sv_A = StateVectorCalcEphem(calcParams.src);
			CSMMass = calcParams.src->GetMass();
		}

		opt.sv_P = StateVectorCalcEphem(calcParams.tgt);
		opt.DH = 10.0*1852.0;
		opt.PhaseAngle = calcParams.LunarLiftoff; //Angle was stored here

		PMSTICN(opt, res);

		//Store for NSR PAD
		calcParams.DVSTORE1 = res.dV_LVLH2;

		if (preliminary == false)
		{
			//State vector after NCC
			EphemerisData sv_NCC = res.sv_tig;
			sv_NCC.V = res.sv_tig.V + res.dV;

			calcParams.SVSTORE1 = ConvertEphemDatatoSV(sv_NCC, CSMMass);
		}

		manopt.TIG = calcParams.CSI;
		manopt.dV_LVLH = res.dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		manopt.navcheckGET = 0.0;
		manopt.sxtstardtime = 0.0;
		manopt.UllageDT = 20.0;
		manopt.UllageThrusterOpt = false;
		manopt.sv0 = opt.sv_A;
		manopt.CSMMass = CSMMass;

		AP7ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "NCC");
	}
	break;
	case 18: //NSR preliminary update
		preliminary = true;
	case 19: //NSR final update
	{
		AP7MNV * form = (AP7MNV *)pad;
		AP7ManPADOpt manopt;

		manopt.TIG = calcParams.CDH;
		manopt.dV_LVLH = calcParams.DVSTORE1; //Was calculated for NCC PAD
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		manopt.navcheckGET = 0.0;
		manopt.sxtstardtime = 0.0;
		manopt.UllageDT = 20.0;
		manopt.UllageThrusterOpt = false;
		manopt.sv0 = ConvertSVtoEphemData(calcParams.SVSTORE1);
		manopt.CSMMass = calcParams.SVSTORE1.mass;

		AP7ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "NSR");
	}
	break;
	case 20: //TPI preliminary update
		preliminary = true;
	case 21: //TPI final update
	{
		AP7MNV * form = (AP7MNV *)pad;
		AP7ManPADOpt manopt;
		TwoImpulseOpt opt;
		TwoImpulseResuls res;
		double CSMMass;

		opt.mode = 5;
		opt.T1 = -1.0;
		opt.T2 = -1.0;
		opt.sv_A = StateVectorCalcEphem(calcParams.src);
		CSMMass = calcParams.src->GetMass();
		opt.sv_P = StateVectorCalcEphem(calcParams.tgt);
		opt.DH = opt.PhaseAngle = 0.0;
		opt.Elev = 27.0*RAD;
		opt.WT = 130.0*RAD;

		PMSTICN(opt, res);

		calcParams.TPI = res.T1; //Update TPI time

		manopt.TIG = res.T1;
		manopt.dV_LVLH = res.dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = true;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		manopt.navcheckGET = 0.0;
		manopt.sxtstardtime = 0.0;
		manopt.UllageDT = 15.0;
		manopt.UllageThrusterOpt = true;
		manopt.sv0 = opt.sv_A;
		manopt.CSMMass = CSMMass;

		AP7ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "TPI");
	}
	break;
	case 22: //Docking Attitude PAD
	{
		GENERICPAD * form = (GENERICPAD *)pad;

		EphemerisData sv, sv2;
		MATRIX3 REFSMMAT;
		VECTOR3 Att;
		double dt;

		sv = StateVectorCalcEphem(calcParams.tgt);

		//Docking attitude valid at roughly an hour after TPI
		dt = GMTfromGET(calcParams.TPI + 3600.0) - sv.GMT;
		sv2 = coast(sv, dt);

		REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		Att = SkylabDockingAttitude(sv2, REFSMMAT);

		sprintf(form->paddata, "Docking attitude: R %03.0f, P %03.0f, Y %03.0f", Att.x*DEG, Att.y*DEG, Att.z*DEG);
	}
	break;
	case 23: //Command Skylab to Solar Inertial attitude
	{
		Skylab* sl = (Skylab*)calcParams.tgt;

		sl->GetATMDC()->CommandSystem(052016, 050002);

		sprintf(upMessage, "Skylab to Solar Inertial attitude");
	}
	break;
	}

	return scrubbed;
}