/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Translunar Injection Processor

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

#include "GeneralizedIterator.h"
#include "rtcc.h"
#include "TLIProcessor.h"

TLIProcessor::TLIProcessor(RTCC *r) : TLTrajectoryComputers(r)
{

}

void TLIProcessor::Init(TLIMEDQuantities med, TLMCCMissionConstants constants, double GMTBase)
{
	TLTrajectoryComputers::Init(constants, GMTBase);

	MEDQuantities = med;
}

void TLIProcessor::Main(TLIOutputData &out)
{
	ErrorIndicator = 0;

	if (MEDQuantities.Mode == 4)
	{
		Option1();
	}

	out.ErrorIndicator = ErrorIndicator;
	if (out.ErrorIndicator) return;

	//Convert to LVDC parameters
	if (MEDQuantities.Mode == 4)
	{
		//7 parameters


		//Convert to ECT at GRR

		EphemerisData2 sv, sv_ECT;

		sv.R = outarray.sv_tli_cut.R;
		sv.V = outarray.sv_tli_cut.V;
		sv.GMT = pRTCC->SystemParameters.MCGRIC*3600.0;
		pRTCC->ELVCNV(sv, 0, 1, sv_ECT);

		OELEMENTS coe = OrbMech::coe_from_sv(sv_ECT.R, sv_ECT.V, mu_E);
		coe.h = dotp(sv_ECT.V, sv_ECT.V) - 2.0*mu_E / length(sv_ECT.R);

		OELEMENTS coe2 = LVTAR(coe, pRTCC->SystemParameters.MCLGRA, pRTCC->SystemParameters.MCERTS*pRTCC->SystemParameters.MCGRIC);

		out.uplink_data.Inclination = coe2.i;
		out.uplink_data.theta_N = coe2.RA;
		out.uplink_data.e = coe2.e;
		out.uplink_data.C3 = coe2.h;
		out.uplink_data.alpha_D = coe2.w;
		out.uplink_data.f = coe2.TA;
		out.uplink_data.GMT_TIG = outarray.sv_tli_ign.GMT;
	}
	else
	{
		//10 parameters
	}

	out.dv_TLI = outarray.dv_TLI;
}

void TLIProcessor::Option1()
{
	//Propagate state to TIG
	EMSMISSInputTable in;

	in.AnchorVector = MEDQuantities.state;
	in.MaxIntegTime = MEDQuantities.GMT_TIG - MEDQuantities.state.GMT;
	if (in.MaxIntegTime < 0)
	{
		in.MaxIntegTime = abs(in.MaxIntegTime);
		in.IsForwardIntegration = false;
	}
	in.VehicleCode = RTCC_MPT_CSM;
	in.useInputWeights = true;
	in.WeightsTable = &MEDQuantities.WeightsTable;

	pRTCC->EMSMISS(&in);

	if (in.NIAuxOutputTable.ErrorCode)
	{
		ErrorIndicator = 1;
		return;
	}

	outarray.sv0 = in.NIAuxOutputTable.sv_cutoff;
	outarray.M_i = in.NIAuxOutputTable.CutoffWeight;

	//Calculate initial guess for C3
	double R_C, a_C, V_C, C3_guess;

	R_C = length(outarray.sv0.R);
	a_C = (R_C + R_E + MEDQuantities.h_ap) / 2.0;
	V_C = sqrt(mu_E*(2.0 / R_C - 1.0 / a_C));
	C3_guess = V_C * V_C - 2.0*mu_E / R_C;

	bool err = ConicTLIIEllipse(C3_guess, MEDQuantities.h_ap);
	if (err)
	{
		ErrorIndicator = 2;
		return;
	}
	err = IntegratedTLIIEllipse(outarray.C3_TLI, MEDQuantities.h_ap);
	if (err)
	{
		ErrorIndicator = 2;
		return;
	}
}

bool TLIProcessor::ConicTLIIEllipse(double C3_guess, double h_ap)
{
	void *constPtr;
	outarray.MidcourseCorrectionIndicator = false;
	outarray.TLIIndicator = true;
	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &ConicMissionComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[10] = true;

	block.IndVarGuess[10] = C3_guess / pow(R_E / 3600.0, 2);
	block.IndVarGuess[11] = MissionConstants.delta;
	block.IndVarGuess[12] = MissionConstants.sigma;

	block.IndVarStep[10] = pow(2, -23);

	block.IndVarWeight[10] = 4.0;

	block.DepVarSwitch[21] = true;

	block.DepVarLowerLimit[21] = (h_ap - 1.0*1852.0) / R_E;

	block.DepVarUpperLimit[21] = (h_ap + 1.0*1852.0) / R_E;

	block.DepVarClass[21] = 1;

	std::vector<double> result;
	std::vector<double> y_vals;
	return GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

bool TLIProcessor::IntegratedTLIIEllipse(double C3_guess_ER, double h_ap)
{
	void *constPtr;
	outarray.TLIIndicator = true;
	outarray.EllipticalCaseIndicator = true;
	outarray.sigma_TLI = MissionConstants.sigma;
	constPtr = &outarray;

	bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &IntegratedTrajectoryComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[4] = true;

	block.IndVarGuess[4] = C3_guess_ER;
	block.IndVarGuess[5] = 0.0; //dt_EPO
	block.IndVarGuess[6] = MissionConstants.delta;

	block.IndVarStep[4] = pow(2, -23);
	block.IndVarWeight[4] = 4.0;

	block.DepVarSwitch[8] = true;
	block.DepVarLowerLimit[8] = (h_ap - 1.0*1852.0) / R_E;
	block.DepVarUpperLimit[8] = (h_ap + 1.0*1852.0) / R_E;
	block.DepVarClass[8] = 1;

	std::vector<double> result;
	std::vector<double> y_vals;
	return GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

OELEMENTS TLIProcessor::LVTAR(OELEMENTS coe, double lng_PAD, double RAGL) const
{
	//lng_PAD: longitude of the launch pad
	//RAGL: Greenwich hour angle at GRR

	OELEMENTS coe2;
	double theta_e;

	theta_e = RAGL + lng_PAD;
	coe2.RA = coe.RA + PI - theta_e;

	if (coe2.RA >= PI2) coe2.RA -= PI2;
	if (coe2.RA < 0) coe2.RA += PI2;

	coe2.i = coe.i;
	coe2.e = coe.e;
	coe2.h = coe.h; //C3

	coe2.w = PI - coe.w; //alpha
	if (coe2.w < 0) coe2.w += PI2;

	coe2.TA = coe.TA;

	return coe2;
}