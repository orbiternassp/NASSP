/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

RTCC Calculations for Mission B

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
#include "LEM.h"
#include "../src_rtccmfd/OrbMech.h"
#include "mcc.h"
#include "rtcc.h"

bool RTCC::CalculationMTP_B(int fcn, LPVOID &pad, char * upString, char * upDesc, char * upMessage)
{
	char uplinkdata[1024 * 3];

	MATRIX3 REFSMMAT = _M(0.749669954748883, -0.141831590016531, 0.646435425251580, 0.318362144838044, 0.933611208066774, -0.16436435, -0.580207293715727, 0.329019622888181, 0.74505405);

	switch (fcn)
	{
	case 1: //MISSION INITIALIZATION
	{
		char Buff[128];

		//P80 MED: mission initialization
		sprintf_s(Buff, "P80,1,LEM,%d,%d,%d;", GZGENCSN.MonthofLiftoff, GZGENCSN.DayofLiftoff, GZGENCSN.Year);
		GMGMED(Buff);

		//P10 MED: Enter actual liftoff time
		int hh, mm;
		double ss;

		hh = 22;
		mm = 48;
		ss = 9.0;

		sprintf_s(Buff, "P10,CSM,%d:%d:%.2lf;", hh, mm, ss); //TBD: Should be LEM
		GMGMED(Buff);

		//P15: CMC, LGC and AGS clock zero
		sprintf_s(Buff, "P15,LGC,%d:%d:%.2lf;", hh, mm, ss);
		GMGMED(Buff);

		//P12: IU GRR and Azimuth
		double Azi = 72.0;
		hh = 22;
		mm = 47;
		ss = 52.0;
		sprintf_s(Buff, "P12,IU1,%d:%d:%.2lf,%.2lf;", hh, mm, ss, Azi);
		GMGMED(Buff);
	}
	break;
	case 2: //RCS BURN 1 ATTITUDE
	{
		AP11LMManPADOpt opt;
		AP11LMMNV manpad;
		char buffer1[1000];

		double GET, SVMJD;
		SVMJD = oapiGetSimMJD();
		GET = mcc->lm->GetMissionTime();

		opt.TIG = OrbMech::HHMMSSToSS(8, 52, 44);
		opt.dV_LVLH = _V(1.0, 0.0, -1.0);
		opt.enginetype = RTCC_ENGINETYPE_LMRCSPLUS4;
		opt.HeadsUp = true;
		opt.REFSMMAT = REFSMMAT;
		opt.RV_MCC = StateVectorCalcEphem(mcc->lm);
		opt.WeightsTable = GetWeightsTable(mcc->lm, false, false);

		AP11LMManeuverPAD(opt, manpad);
		SunburstAttitudeManeuver(buffer1, manpad.IMUAtt);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Attitude for RCS Burn #1");
		}
	}
	break;
	case 3: //CONFIGURE FOR SYSTEM B DEPLETION
	{
		char buffer1[1000];
		char buffer2[1000];

		SunburstLMPCommand(buffer1, 168);
		SunburstLMPCommand(buffer2, 232);

		sprintf(uplinkdata, "%sKKKKKKKKKK%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Configure for RCS system B depletion");
		}
	}
	break;
	case 4: //PLUS X TRANSLATION ON
	{
		char buffer1[1000];

		SunburstLMPCommand(buffer1, 128);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "RCS +X translation on");
		}
	}
	break;
	case 5: //PLUS X TRANSLATION OFF
	{
		char buffer1[1000];

		SunburstLMPCommand(buffer1, 129);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "RCS +X translation off");
		}
	}
	break;
	case 6: //CONFIGURE FOR NORMAL RCS A OPERATION
	{
		char buffer1[1000];
		char buffer2[1000];

		SunburstLMPCommand(buffer1, 152);
		SunburstLMPCommand(buffer2, 216);

		sprintf(uplinkdata, "%sKKKKKKKKKK%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Configure for normal RCS system A operation");
		}
	}
	break;
	case 7: //RCS BURN 2 ATTITUDE
	{
		AP11LMManPADOpt opt;
		AP11LMMNV manpad;
		char buffer1[1000];

		double GET, SVMJD;
		SVMJD = oapiGetSimMJD();
		GET = mcc->lm->GetMissionTime();

		opt.TIG = OrbMech::HHMMSSToSS(9, 40, 20);
		opt.dV_LVLH = _V(0.0, 1.0, 0.0);
		opt.enginetype = RTCC_ENGINETYPE_LMRCSPLUS4;
		opt.HeadsUp = true;
		opt.REFSMMAT = REFSMMAT;
		opt.RV_MCC = StateVectorCalcEphem(mcc->lm);
		opt.WeightsTable = GetWeightsTable(mcc->lm, false, false);

		AP11LMManeuverPAD(opt, manpad);
		SunburstAttitudeManeuver(buffer1, manpad.IMUAtt);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Attitude for RCS Burn #2");
		}
	}
	break;
	case 8: //X-FEED OPEN
	{
		char buffer1[1000];
		SunburstLMPCommand(buffer1, 252);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "X-Feed Open");
		}
	}
	break;
	case 9: //ERRONEOUS LM WEIGHT
	{
		char buffer1[1000];
		SunburstMassUpdate(buffer1, 4716.0);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Update LM mass with erroneous value");
		}
	}
	break;
	case 10: //RCS BURN 5 ATTITUDE
	{
		AP11LMManPADOpt opt;
		AP11LMMNV manpad;
		char buffer1[1000];

		double GET, SVMJD;
		SVMJD = oapiGetSimMJD();
		GET = mcc->lm->GetMissionTime();

		opt.TIG = OrbMech::HHMMSSToSS(12, 52, 18);
		opt.dV_LVLH = _V(1.0, 0.0, 0.0);
		opt.enginetype = RTCC_ENGINETYPE_LMRCSPLUS4;
		opt.HeadsUp = true;
		opt.REFSMMAT = REFSMMAT;
		opt.RV_MCC = StateVectorCalcEphem(mcc->lm);
		opt.WeightsTable = GetWeightsTable(mcc->lm, false, false);

		AP11LMManeuverPAD(opt, manpad);
		SunburstAttitudeManeuver(buffer1, manpad.IMUAtt);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Attitude for RCS Burn #5");
		}
	}
	break;
	}

	return false;
}