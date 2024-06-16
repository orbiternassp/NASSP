/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Translunar Injection Processor (Header)

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

#pragma once

#include "TLTrajectoryComputers.h"

struct TLIMEDQuantities
{
	//1 = S-IVB hypersurface solution, 2 = integrated free-return, 3 = hybrid ellipse, 4 = E-type mission ellipse
	int Mode;
	EphemerisData state;
	PLAWDTOutput WeightsTable;

	//Mode1
	double GMT_TIG;
	double h_ap;
};

struct SevenParameterUpdate
{
	double GMT_TIG = 0.0;
	double C3 = 0.0;
	double Inclination = 0.0;
	double e = 0.0;
	double alpha_D = 0.0;
	double f = 0.0;
	double theta_N = 0.0;
};

struct TLIOutputData
{
	int ErrorIndicator = 0;
	bool IsSevenParameters = true;
	EphemerisData sv_TLI_ign, sv_TLI_cut;
	SevenParameterUpdate uplink_data;
	double dv_TLI;
};

class TLIProcessor : public TLTrajectoryComputers
{
public:
	TLIProcessor(RTCC *r);

	void Main(TLIOutputData &out);
	void Init(TLIMEDQuantities med, TLMCCMissionConstants constants, double GMTBase);
protected:

	void Option1();

	bool ConicTLIIEllipse(double C3_guess, double h_ap);
	bool IntegratedTLIIEllipse(double C3_guess_ER, double h_ap);

	OELEMENTS LVTAR(OELEMENTS coe, double lng_PAD, double RAGL) const;

	TLIMEDQuantities MEDQuantities;
	int ErrorIndicator;
};