/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Q-Ball

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

#include "saturn.h"
#include "papi.h"

#include "qball.h"

QBall::QBall()
{
	aoa = 0.0;
}

void QBall::Init(Saturn *vessel) {

	sat = vessel;
}

double QBall::GetAOA()
{
	aoa = 0.0;

	if (sat->LETAttached() && sat->GetQBallPower())
	{
		if (sat->GetQBallSimulateCmd())
		{
			aoa = 10.0*RAD;
		}
		else {
			/* There is no definite source on how to calculate the pressure
			   difference measured by Q-ball, but we can safely assume that it
			   has the form of deltap = mu*q*aoa, where mu is the unknown
			   "gain" of the Q-ball. For each Saturn V mission where was Q-ball
			   present (AS501-AS512), the Launch Vehicle Flight Evaluation
			   Report gives the maximum measured deltap value and the time of
			   the maximum as well. There are also diagrams with dynamic
			   pressure and AOA, in the reports, so the mu value could be
			   determined for these missions:
			   AS501   0.0523
			   AS502   0.0588
			   AS503   0.0998
			   AS504   0.0573
			   AS505   0.0562
			   AS506   0.0507
			   AS507   0.0555
			   AS508   0.0553
			   AS509   0.0679
			   AS510   0.0560
			   AS511   0.0904
			   AS512   0.0589
			   The median of these values is 0.0568 1/degree, that is used as
			   mu in this function. The indicator displayed the deltap in the
			   percentage of Manual Abort Limit, defined as 3.2 psid =
			   2.21 N/cm2. That gives the constant in this function as
			   0.0568*10/22100 = 0.0000257 1/Pa
			*/
			double dynpress = sat->GetDynPressure();
			if(dynpress > 100.0)
			{
				double aoa1 = sat->GetAOA();
				double aoa2 = sat->GetSlipAngle();
				aoa = 0.0000257*dynpress*sqrt(aoa1*aoa1 + aoa2*aoa2);
			}
		}
	}

	return aoa;
}

void QBall::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_double(scn, "AOA", aoa);

	oapiWriteLine(scn, end_str);
}

void QBall::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_double(line, "AOA", aoa);

	}
}