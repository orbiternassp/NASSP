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

bool RTCC::CalculationMTP_SL(int fcn, LPVOID &pad, char * upString, char * upDesc, char * upMessage)
{
	char uplinkdata[1024 * 3];
	char Buff[128];
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

		OrbMech::SStoHHMMSS(gmt, hh, mm, ss);

		sprintf_s(Buff, "P10,CSM,%d:%d:%.2lf;", hh, mm, ss);
		GMGMED(Buff);

		//P12: Predicted CSM GRR and Azimuth
		sprintf_s(Buff, "P12,CSM,%d:%d:%.2lf,44.4;", hh, mm, ss);
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
			sprintf(upDesc, "CSM & SWS state vectors");
		}
	}
	break;
	case 10: //ON-ORBIT INITIALIZATION
	{
		//P10 MED: Enter actual liftoff time
		double tephem_scal;
		Saturn *cm = (Saturn *)calcParams.src;

		//Get TEPHEM
		tephem_scal = GetTEPHEMFromAGC(&cm->agc.vagc); //TBD: Add Skylark TEPHEM address here
		double LaunchMJD = (tephem_scal / 8640000.) + SystemParameters.TEPHEM0;
		LaunchMJD = (LaunchMJD - SystemParameters.GMTBASE)*24.0;

		int hh, mm;
		double ss;

		OrbMech::SStoHHMMSS(LaunchMJD*3600.0, hh, mm, ss);

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
		OrbMech::SStoHHMMSS(T_GRR, hh, mm, ss);
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
		opt.WT = 130.0*RAD;
		opt.Elev = 27.0*RAD;
		opt.K46 = 4; //TPI at X minutes into light
		opt.TIMLIT = -20.0*60.0;

		DockingInitiationProcessor(opt);

		calcParams.Phasing = PZREDT.GET[0]; //NC1 time
		calcParams.CSI = PZREDT.GET[1]; //NC2 time
		calcParams.CDH = PZREDT.GET[3]; //NSR time
		calcParams.TPI = PZRPDT.data[0].GETTPI; //TPI time
	}
	break;
	case 12: //NC1 preliminary update
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
		opt.K46 = 4; //TPI at X minutes into light
		opt.TIMLIT = -20.0*60.0;

		DockingInitiationProcessor(opt);

		calcParams.CSI = PZREDT.GET[1]; //NC2 time
		calcParams.CDH = PZREDT.GET[3]; //NSR time
		calcParams.TPI = PZRPDT.data[0].GETTPI; //TPI time

		manopt.TIG = opt.PUTTNA;
		manopt.dV_LVLH = PZREDT.DVVector[0] * 0.3048;
		manopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
		manopt.HeadsUp = true;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, true);
		manopt.navcheckGET = 0.0;
		manopt.sxtstardtime = -25.0*60.0;
		manopt.UllageDT = 15.0;
		manopt.UllageThrusterOpt = true;
		manopt.sv0 = opt.sv_CSM;
		manopt.CSMMass = calcParams.src->GetMass();

		AP7ManeuverPAD(&manopt, *form);

		if (fcn == 13)
		{
			char buffer1[1000];
			char buffer2[1000];

			AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_CSM, opt.sv_CSM);
			AGCStateVectorUpdate(buffer1, 1, RTCC_MPT_LM, opt.sv_LM);

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
	case 15: //NC2 final update
	{
		DKIOpt opt;

		opt.sv_CSM = StateVectorCalcEphem(calcParams.src);
		opt.sv_LM = StateVectorCalcEphem(calcParams.tgt);
		opt.MV = 1; //CSM maneuvers

		opt.DHSR = 10.0*1852.0;
		opt.MI = 5.0; //M = 5 rendezvous
		opt.NC1 = opt.MI - 1.5; //NC2 maneuver becomes NC1
		opt.NH = opt.NC1 + 0.5; //NCC maneuver becomes NH
		opt.NPC = -1.0;
		opt.IPUTNA = 1; //Maneuver line at input time
		opt.PUTNA = opt.NC1; //NC2
		opt.PUTTNA = GMTfromGET(calcParams.CSI);
		opt.WT = 130.0*RAD;
		opt.Elev = 27.0*RAD;
		opt.K46 = 4; //TPI at X minutes into light
		opt.TIMLIT = -20.0*60.0;

		DockingInitiationProcessor(opt);
	}
	break;
	}

	return scrubbed;
}