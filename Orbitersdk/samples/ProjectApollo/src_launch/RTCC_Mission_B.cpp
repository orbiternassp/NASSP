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

	double AGCEpoch = 39856.0;
	MATRIX3 REFSMMAT = _M(0.749669954748883, -0.141831590016531, 0.646435425251580, 0.318362144838044, 0.933611208066774, -0.16436435, -0.580207293715727, 0.329019622888181, 0.74505405);
	REFSMMAT = mul(REFSMMAT, OrbMech::J2000EclToBRCS(AGCEpoch));

	switch (fcn)
	{
	case 1: //RCS BURN 1 ATTITUDE
	{
		AP11LMManPADOpt opt;
		AP11LMMNV manpad;
		char buffer1[1000];

		double GET, SVMJD;
		SVMJD = oapiGetSimMJD();
		GET = mcc->lm->GetMissionTime();
		opt.GETbase = SVMJD - GET / 24.0 / 3600.0;

		opt.dV_LVLH = _V(1.0, 0.0, -1.0);
		opt.enginetype = RTCC_ENGINETYPE_LMRCSPLUS4;
		opt.HeadsUp = true;
		opt.REFSMMAT = REFSMMAT;
		opt.TIG = OrbMech::HHMMSSToSS(8, 52, 44);
		opt.vessel = mcc->lm;
		opt.csmlmdocked = 0;

		AP11LMManeuverPAD(&opt, manpad);
		SunburstAttitudeManeuver(buffer1, manpad.IMUAtt);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Attitude for RCS Burn #1");
		}
	}
	break;
	case 2: //CONFIGURE FOR SYSTEM B DEPLETION
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
	case 3: //PLUS X TRANSLATION ON
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
	case 4: //PLUS X TRANSLATION OFF
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
	case 5: //CONFIGURE FOR NORMAL RCS A OPERATION
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
	case 6: //RCS BURN 2 ATTITUDE
	{
		AP11LMManPADOpt opt;
		AP11LMMNV manpad;
		char buffer1[1000];

		double GET, SVMJD;
		SVMJD = oapiGetSimMJD();
		GET = mcc->lm->GetMissionTime();
		opt.GETbase = SVMJD - GET / 24.0 / 3600.0;

		opt.dV_LVLH = _V(0.0, 1.0, 0.0);
		opt.enginetype = RTCC_ENGINETYPE_LMRCSPLUS4;
		opt.HeadsUp = true;
		opt.REFSMMAT = REFSMMAT;
		opt.TIG = OrbMech::HHMMSSToSS(9, 40, 20);
		opt.vessel = mcc->lm;
		opt.csmlmdocked = 0;

		AP11LMManeuverPAD(&opt, manpad);
		SunburstAttitudeManeuver(buffer1, manpad.IMUAtt);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "Attitude for RCS Burn #2");
		}
	}
	break;
	case 7: //X-FEED OPEN
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
	case 8: //ERRONEOUS LM WEIGHT
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
	case 9: //RCS BURN 5 ATTITUDE
	{
		AP11LMManPADOpt opt;
		AP11LMMNV manpad;
		char buffer1[1000];

		double GET, SVMJD;
		SVMJD = oapiGetSimMJD();
		GET = mcc->lm->GetMissionTime();
		opt.GETbase = SVMJD - GET / 24.0 / 3600.0;

		opt.dV_LVLH = _V(1.0, 0.0, 0.0);
		opt.enginetype = RTCC_ENGINETYPE_LMRCSPLUS4;
		opt.HeadsUp = true;
		opt.REFSMMAT = REFSMMAT;
		opt.TIG = OrbMech::HHMMSSToSS(12, 52, 18);
		opt.vessel = mcc->lm;
		opt.csmlmdocked = 0;

		AP11LMManeuverPAD(&opt, manpad);
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