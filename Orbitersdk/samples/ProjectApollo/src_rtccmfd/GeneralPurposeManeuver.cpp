/****************************************************************************
This file is part of Project Apollo - NASSP

General Purpose Maneuver Processor, RTCC Module PMMGPM

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

#include "rtcc.h"
#include "GeneralPurposeManeuver.h"

RTCCGeneralPurposeManeuverProcessor::RTCCGeneralPurposeManeuverProcessor(RTCC *r) : RTCCModule(r)
{

}

int RTCCGeneralPurposeManeuverProcessor::PCMGPM(const GMPOpt &IOPT)
{
	opt = &IOPT;
	code = opt->ManeuverCode;
	ErrorIndicator = 0;

	//Determine maneuver point
	DetermineManeuverPoint();
	DetermineManeuverType();

	//Coast to threshold time
	EphemerisData sv1;
	double TIG_GMT, dt1;

	TIG_GMT = pRTCC->GMTfromGET(opt->TIG_GET);
	dt1 = TIG_GMT - opt->sv_in.GMT;
	sv1 = pRTCC->coast(opt->sv_in, dt1, opt->Weight, opt->Area, opt->KFactor, false);

	//Convert to AEG
	aeg = pRTCC->SVToAEG(sv1, opt->Area, opt->Weight, opt->KFactor);

	//Initialize AEG
	pRTCC->PMMAEGS(aeg.Header, aeg.Data, aeg.Data);
	if (aeg.Header.ErrorInd == -3)
	{
		//AEG failed to converge
		return 3;
	}
	else if (aeg.Header.ErrorInd)
	{
		//AEG error
		return 4;
	}

	if (sv1.RBI == BODY_EARTH)
	{
		R_E = pRTCC->SystemParameters.MCECAP;
		mu = OrbMech::mu_Earth;
	}
	else
	{
		R_E = pRTCC->BZLAND.rad[RTCC_LMPOS_BEST];
		mu = OrbMech::mu_Moon;
	}

	//Calculate
	PCGPMP();

	if (ErrorIndicator)
	{
		return ErrorIndicator;
	}

	//Output
	VECTOR3 R_A, V_A, R_B, V_B;
	OrbMech::GIMKIC(sv_b_apo.coe_osc, mu, R_B, V_B);
	OrbMech::GIMKIC(sv_a.coe_osc, mu, R_A, V_A);

	//Debug
	//VECTOR3 DR = R_A - R_B;
	//sprintf_s(oapiDebugString(), 128, "%lf %lf %lf", DR.x, DR.y, DR.z);

	//Special HAS logic: show apogee/perigee data in N revs, and not the next one
	if (opt->ManeuverCode == RTCC_GMP_HAS)
	{
		INFO[0] = INFO_HAS[0]; INFO[2] = INFO_HAS[2]; INFO[3] = INFO_HAS[3];
		INFO[5] = INFO_HAS[5]; INFO[7] = INFO_HAS[7]; INFO[8] = INFO_HAS[8];
	}

	pRTCC->PZGPMELM.code = "GP";
	pRTCC->PZGPMELM.SV_before.R = R_B;
	pRTCC->PZGPMELM.SV_before.V = V_B;
	pRTCC->PZGPMELM.SV_before.GMT = sv_b_apo.TS;
	pRTCC->PZGPMELM.SV_before.RBI = aeg.Header.AEGInd;
	pRTCC->PZGPMELM.V_after = V_A;

	pRTCC->PZGPMDIS.GET_A = pRTCC->GETfromGMT(INFO[0]);
	pRTCC->PZGPMDIS.lat_A = INFO[2];
	pRTCC->PZGPMDIS.long_A = INFO[3];
	pRTCC->PZGPMDIS.HA = INFO[4];

	//Impacting trajectory?
	if (INFO[9] < 0.0)
	{
		//Find impact location

		int err = pRTCC->PITCIR(aeg.Header, sv_a, R_E, sv_temp);

		//Are some errors ok?
		if (err == 2 || err == 4)
		{
			//Error
			ErrorIndicator = 2;
			return ErrorIndicator;
		}

		pRTCC->PZGPMDIS.GET_P = pRTCC->GETfromGMT(sv_temp.TS);
		GetLatLongHeight(sv_temp, pRTCC->PZGPMDIS.HP, pRTCC->PZGPMDIS.lat_P, pRTCC->PZGPMDIS.long_P);
		pRTCC->PZGPMDIS.ShowImpact = true;
	}
	else
	{
		//No, show periapsis data
		pRTCC->PZGPMDIS.GET_P = pRTCC->GETfromGMT(INFO[5]);
		pRTCC->PZGPMDIS.lat_P = INFO[7];
		pRTCC->PZGPMDIS.long_P = INFO[8];
		pRTCC->PZGPMDIS.HP = INFO[9];
		pRTCC->PZGPMDIS.ShowImpact = false;
	}

	pRTCC->PZGPMDIS.A = sv_a.coe_osc.a;
	pRTCC->PZGPMDIS.E = sv_a.coe_osc.e;
	
	if (aeg.Header.AEGInd == BODY_MOON)
	{
		double u, g, h;
		GetSelenographicElements(sv_a, u, pRTCC->PZGPMDIS.I, g, h);
		if (ErrorIndicator) return ErrorIndicator;
	}
	else
	{
		pRTCC->PZGPMDIS.I = sv_a.coe_osc.i;
	}
	pRTCC->PZGPMDIS.Node_Ang = Node_Ang;
	pRTCC->PZGPMDIS.Del_G = Del_G;
	pRTCC->PZGPMDIS.Pitch_Man = Pitch_Man;
	pRTCC->PZGPMDIS.Yaw_Man = Yaw_Man;
	pRTCC->PZGPMDIS.dv = DV;
	pRTCC->PZGPMDIS.DV = DV_Vector;
	pRTCC->PZGPMDIS.GET_TIG = pRTCC->GETfromGMT(sv_b_apo.TS);
	pRTCC->PZGPMDIS.H_Man = H_man;
	pRTCC->PZGPMDIS.lat_Man = lat_man;
	pRTCC->PZGPMDIS.long_Man = lng_man;

	return 0;
}

void RTCCGeneralPurposeManeuverProcessor::DetermineManeuverPoint()
{
	//Time
	if (code == RTCC_GMP_PCT || code == RTCC_GMP_HOT || code == RTCC_GMP_NST || code == RTCC_GMP_HBT || code == RTCC_GMP_FCT ||
		code == RTCC_GMP_NHT || code == RTCC_GMP_PHT || code == RTCC_GMP_SAT || code == RTCC_GMP_HNT || code == RTCC_GMP_CRT ||
		code == RTCC_GMP_CPT || code == RTCC_GMP_CNT)
	{
		ManeuverPoint = 0;
	}
	//Apogee
	else if (code == RTCC_GMP_HAO || code == RTCC_GMP_FCA || code == RTCC_GMP_PHA || code == RTCC_GMP_HNA ||
		code == RTCC_GMP_CRA || code == RTCC_GMP_CPA || code == RTCC_GMP_CNA)
	{
		ManeuverPoint = 1;
	}
	//Perigee
	else if (code == RTCC_GMP_HPO || code == RTCC_GMP_FCP || code == RTCC_GMP_PHP || code == RTCC_GMP_HNP ||
		code == RTCC_GMP_CRP || code == RTCC_GMP_CPP || code == RTCC_GMP_CNP)
	{
		ManeuverPoint = 2;
	}
	//Equatorial crossing
	else if (code == RTCC_GMP_PCE || code == RTCC_GMP_FCE)
	{
		ManeuverPoint = 3;
	}
	//Longitude crossing
	else if (code == RTCC_GMP_PCL || code == RTCC_GMP_CRL || code == RTCC_GMP_HOL || code == RTCC_GMP_FCL ||
		code == RTCC_GMP_SAL || code == RTCC_GMP_PHL || code == RTCC_GMP_CPL || code == RTCC_GMP_HBL ||
		code == RTCC_GMP_CNL || code == RTCC_GMP_HNL || code == RTCC_GMP_NSL || code == RTCC_GMP_NHL)
	{
		ManeuverPoint = 4;
	}
	//Height
	else if (code == RTCC_GMP_CRH || code == RTCC_GMP_HBH || code == RTCC_GMP_FCH || code == RTCC_GMP_CPH ||
		code == RTCC_GMP_PCH || code == RTCC_GMP_NSH || code == RTCC_GMP_HOH || code == RTCC_GMP_CNH)
	{
		ManeuverPoint = 5;
	}
	else if (code == RTCC_GMP_NSO)
	{
		ManeuverPoint = 6;
	}
	else if (code == RTCC_GMP_HBO)
	{
		ManeuverPoint = 7;
	}
	else if (code == RTCC_GMP_SAO)
	{
		ManeuverPoint = 8;
	}
	else if (code == RTCC_GMP_HAS)
	{
		ManeuverPoint = 9;
	}
	else if (code == RTCC_GMP_SAA)
	{
		ManeuverPoint = 10;
	}
}

void RTCCGeneralPurposeManeuverProcessor::DetermineManeuverType()
{
	if (code == RTCC_GMP_FCT || code == RTCC_GMP_FCL || code == RTCC_GMP_FCH || code == RTCC_GMP_FCA || code == RTCC_GMP_FCP || code == RTCC_GMP_FCE)
	{
		ManeuverType = 0;
	}
	else if (code == RTCC_GMP_PCE || code == RTCC_GMP_PCL || code == RTCC_GMP_PCT || code == RTCC_GMP_PCH)
	{
		ManeuverType = 1;
	}
	else if (code == RTCC_GMP_CRL || code == RTCC_GMP_CRH || code == RTCC_GMP_CRT || code == RTCC_GMP_CRA || code == RTCC_GMP_CRP)
	{
		ManeuverType = 2;
	}
	else if (code == RTCC_GMP_HAO || code == RTCC_GMP_HPO)
	{
		ManeuverType = 3;
	}
	else if (code == RTCC_GMP_HOL || code == RTCC_GMP_HOT || code == RTCC_GMP_HOH)
	{
		ManeuverType = 4;
	}
	else if (code == RTCC_GMP_NSL || code == RTCC_GMP_NST || code == RTCC_GMP_NSH || code == RTCC_GMP_NSO)
	{
		ManeuverType = 5;
	}
	else if (code == RTCC_GMP_HBT || code == RTCC_GMP_HBH || code == RTCC_GMP_HBO || code == RTCC_GMP_HBL)
	{
		ManeuverType = 6;
	}
	else if (code == RTCC_GMP_NHT || code == RTCC_GMP_NHL)
	{
		ManeuverType = 7;
	}
	else if (code == RTCC_GMP_SAT || code == RTCC_GMP_SAO || code == RTCC_GMP_SAL)
	{
		ManeuverType = 8;
	}
	else if (code == RTCC_GMP_SAA)
	{
		ManeuverType = 9;
	}
	else if (code == RTCC_GMP_PHL || code == RTCC_GMP_PHT || code == RTCC_GMP_PHA || code == RTCC_GMP_PHP)
	{
		ManeuverType = 10;
	}
	else if (code == RTCC_GMP_CPL || code == RTCC_GMP_CPH || code == RTCC_GMP_CPT || code == RTCC_GMP_CPA || code == RTCC_GMP_CPP)
	{
		ManeuverType = 11;
	}
	else if (code == RTCC_GMP_CNL || code == RTCC_GMP_CNH || code == RTCC_GMP_CNT || code == RTCC_GMP_CNA || code == RTCC_GMP_CNP)
	{
		ManeuverType = 12;
	}
	else if (code == RTCC_GMP_HNL || code == RTCC_GMP_HNT || code == RTCC_GMP_HNA || code == RTCC_GMP_HNP)
	{
		ManeuverType = 13;
	}
	else if (code == RTCC_GMP_HAS)
	{
		ManeuverType = 14;
	}
}

void RTCCGeneralPurposeManeuverProcessor::PCGPMP()
{
	//Initialize
	K3 = K5 = 0;

	//Time
	if (ManeuverPoint == 0)
	{
		sv_b = aeg.Data;
	}
	//Apogee
	else if (ManeuverPoint == 1)
	{
		K3 = 1;
		pRTCC->PMMAPD(aeg.Header, aeg.Data, 1, 0, INFO, &sv_b, NULL);
	}
	//Perigee
	else if (ManeuverPoint == 2)
	{
		K3 = -1;
		pRTCC->PMMAPD(aeg.Header, aeg.Data, -1, 0, INFO, NULL, &sv_b);
	}
	//Equatorial crossing
	else if (ManeuverPoint == 3)
	{
		double U, isg, gsg, hsg;

		if (aeg.Header.AEGInd == BODY_EARTH)
		{
			U = aeg.Data.U;
		}
		else
		{
			GetSelenographicElements(aeg.Data, U, isg, gsg, hsg);
			if (ErrorIndicator) return;
		}

		//Which equatorial crossing?
		if (U < PI)
		{
			aeg.Data.Item8 = PI;
			aeg.Data.Item10 = 0.0;
		}
		else
		{
			aeg.Data.Item8 = 0.0;
			aeg.Data.Item10 = 1.0;
		}

		//Calculate equatorial crossing
		if (aeg.Header.AEGInd == BODY_EARTH)
		{
			aeg.Data.TIMA = 2;
			pRTCC->PMMAEGS(aeg.Header, aeg.Data, sv_b);
		}
		else
		{
			pRTCC->PIATSU(aeg.Data, sv_b, isg, gsg, hsg);
		}
	}
	//Longitude
	else if (ManeuverPoint == 4)
	{
		int K;
		pRTCC->PMMTLC(aeg.Header, aeg.Data, sv_b, opt->long_D, K, 0);
		if (K < 0)
		{
			//Error
			ErrorIndicator = 2;
			return;
		}
	}
	//Height
	else if (ManeuverPoint == 5)
	{
		int err = pRTCC->PITCIR(aeg.Header, aeg.Data, R_E + opt->H_D, sv_b);

		//Are some errors ok?
		if (err == 2 || err == 4)
		{
			//Error
			ErrorIndicator = 2;
			return;
		}
	}
	//Optimum nodal shift
	else if (ManeuverPoint == 6)
	{
		double u_temp, i_temp, g_temp, h_temp;
		if (aeg.Header.AEGInd == BODY_EARTH)
		{
			i_temp = aeg.Data.coe_osc.i;
			u_temp = aeg.Data.U;
		}
		else
		{
			GetSelenographicElements(aeg.Data, u_temp, i_temp, g_temp, h_temp);
			if (ErrorIndicator) return;
		}

		aeg.Data.Item8 = pRTCC->GLQATN(1.0 + cos(opt->dLAN), -sin(opt->dLAN*cos(i_temp)));
		if (u_temp < aeg.Data.Item8)
		{
			aeg.Data.Item10 = 0.0;
		}
		else
		{
			if (u_temp < aeg.Data.Item8 + PI)
			{
				aeg.Data.Item8 += PI;
				aeg.Data.Item10 = 0.0;
			}
			else
			{
				aeg.Data.Item10 = 1.0;
			}
		}
		if (aeg.Header.AEGInd == BODY_EARTH)
		{
			aeg.Data.TIMA = 2;
			pRTCC->PMMAEGS(aeg.Header, aeg.Data, sv_b);
			if (aeg.Header.ErrorInd)
			{
				ErrorIndicator = 4;
				return;
			}
		}
		else
		{
			if (pRTCC->PIATSU(aeg.Data, sv_b, i_temp, g_temp, h_temp))
			{
				ErrorIndicator = 2;
				return;
			}
		}
	}
	//Optimum apsides change
	else if (ManeuverPoint == 7)
	{
		OptimumPointForApsidesChange();
	}
	//Optimum apse line shift
	else if (ManeuverPoint == 8)
	{
		OptimumApseLineShift(opt->dLOA);
	}
	else if (ManeuverPoint == 9)
	{
		ApsidesPlacementNRevsLater();
	}
	else if (ManeuverPoint == 10)
	{
		pRTCC->PMMAPD(aeg.Header, aeg.Data, -1, 0, INFO, NULL, &sv_b);
		dLOA_temp = OrbMech::calculateDifferenceBetweenAngles(INFO[8], opt->long_D);
		OptimumApseLineShift(dLOA_temp);
	}

	if (ErrorIndicator) return;

	//Save elements before maneuver
	sv_b_apo = sv_a = sv_b;

	//Compute maneuver point quantities
	GetLatLongHeight(sv_b, H_man, lat_man, lng_man);

	//Flight controller input
	if (ManeuverType == 0)
	{
		FlightControllerInput();
		PlaneChange();
	}
	//Plane change
	else if (ManeuverType == 1)
	{
		DW = opt->dW;
		PlaneChange();
	}
	//Circularization
	else if (ManeuverType == 2)
	{
		HeightManeuver(true);
		DW = 0.0;
	}
	//Optimum apogee perigee change
	else if (ManeuverType == 3)
	{
		//OptimumApsidesChange gives bad results if maneuver is not exactly at apogee or perigee
		//int itemp = OptimumApsidesChange();
		//if (itemp == 1)
		//{
			//Switch to height maneuver logic
			HeightManeuver(false);
		//}
		DW = 0.0;
	}
	else if (ManeuverType == 4)
	{
		HeightManeuver(false);
		DW = 0.0;
	}
	else if (ManeuverType == 5)
	{
		NodeShift();
	}
	else if (ManeuverType == 6)
	{
		ApsidesChange();
		DW = 0.0;
	}
	//Node Shift + Apsides Change
	else if (ManeuverType == 7)
	{
		NodeShift();
		sv_b = sv_a;
		ApsidesChange();
	}
	//Line of apsides shift
	else if (ManeuverType == 8)
	{
		ApseLineShift(opt->dLOA);
		DW = 0.0;
	}
	//Line of apsides shift to longitude
	else if (ManeuverType == 9)
	{
		ApseLineShift(dLOA_temp);
		DW = 0.0;
	}
	//Plane change and height maneuver
	else if (ManeuverType == 10)
	{
		DW = opt->dW;
		PlaneChange();
		sv_b = sv_a;
		HeightManeuver(false);
	}
	//Circularization and plane change
	else if (ManeuverType == 11)
	{
		DW = opt->dW;
		PlaneChange();
		sv_b = sv_a;
		HeightManeuver(true);
	}
	//Circularization and node shift
	else if (ManeuverType == 12)
	{
		NodeShift();
		sv_b = sv_a;
		HeightManeuver(true);
	}
	//Node shift and height maneuver
	else if (ManeuverType == 13)
	{
		NodeShift();
		sv_b = sv_a;
		HeightManeuver(false);
	}
	//Apsides change and shift to longitude N revs later
	else if (ManeuverType == 14)
	{
		ApsidesChange();
		if (ErrorIndicator) return;
		DW = 0.0;
	}

	if (ErrorIndicator) return;

	//Calculate pitch and yaw for output
	double x_b, r_b_dot, r_a_dot, v_H_a, dv_H_OP, dv_H_IP, dr_dot, v_H_b, x_a;
	x_b = sqrt(mu*(sv_b_apo.coe_osc.a)*(1.0 - sv_b_apo.coe_osc.e*sv_b_apo.coe_osc.e));
	r_b_dot = (mu*sv_b_apo.coe_osc.e*sin(sv_b_apo.f)) / x_b;
	v_H_b = x_b / sv_b_apo.R;
	x_a = sqrt(mu*(sv_a.coe_osc.a)*(1.0 - sv_a.coe_osc.e*sv_a.coe_osc.e));
	r_a_dot = (mu*sv_a.coe_osc.e*sin(sv_a.f)) / x_a;
	v_H_a = x_a / sv_b.R;

	dv_H_OP = -v_H_a * sin(DW);
	dv_H_IP = v_H_a * cos(DW) - v_H_b;
	dr_dot = r_a_dot - r_b_dot;
	DV_Vector = _V(dv_H_IP, dv_H_OP, -dr_dot);
	DV = sqrt(dr_dot*dr_dot + dv_H_IP * dv_H_IP + dv_H_OP * dv_H_OP);
	Pitch_Man = asin(dr_dot / DV);

	if (abs(abs(Pitch_Man) - PI05) <= 0.0017)
	{
		Yaw_Man = 0.0;
	}
	else
	{
		Yaw_Man = atan2(dv_H_OP, dv_H_IP);
	}

	//Calculate apogee/perigee for output
	if (K3 != 2)
	{
		sv_a.ENTRY = 0;
		sv_a.TIMA = 0;
		sv_a.TE = sv_a.TS;
		if (pRTCC->PMMAPD(aeg.Header, sv_a, 0, 0, INFO, &sv_AP, &sv_PE))
		{
			ErrorIndicator = 8;
			return;
		}
	}

	//Advance to next ascending node
	sv_a.Item8 = 0.0;
	sv_a.Item10 = 1.0;
	if (aeg.Header.AEGInd == BODY_EARTH)
	{
		sv_a.TIMA = 2;
		pRTCC->PMMAEGS(aeg.Header, sv_a, sv_temp);
		Node_Ang = fmod(sv_temp.coe_osc.h - pRTCC->SystemParameters.MCLAMD - OrbMech::w_Earth*sv_temp.TS, PI2);
	}
	else
	{
		double isg, gsg, hsg;
		pRTCC->PIATSU(sv_a, sv_temp, isg, gsg, hsg);
		Node_Ang = hsg;
	}
	if (Node_Ang < 0)
	{
		Node_Ang += PI2;
	}
	Del_G = sv_a.coe_osc.g - sv_b_apo.coe_osc.g;
	if (Del_G < -PI)
	{
		Del_G += PI2;
	}

}

void RTCCGeneralPurposeManeuverProcessor::GetSelenographicElements(const AEGDataBlock &sv, double &u, double &i, double &g, double &h)
{
	MATRIX3 L;
	VECTOR3 P, W, P_apo, W_apo;
	pRTCC->PIVECT(sv.coe_osc.i, sv.coe_osc.g, sv.coe_osc.h, P, W);
	if (pRTCC->PLEFEM(5, sv.TS / 3600.0, 0, NULL, NULL, NULL, &L))
	{
		ErrorIndicator = 5;
		return;
	}
	P_apo = mul(L, P);
	W_apo = mul(L, W);
	pRTCC->PIVECT(P_apo, W_apo, i, g, h);
	u = g + sv.f;
	if (u > PI2)
	{
		u -= PI2;
	}
}

void RTCCGeneralPurposeManeuverProcessor::GetSelenocentricElements(double i, double g, double h, AEGDataBlock &sv)
{
	MATRIX3 L;
	VECTOR3 P, W, P_apo, W_apo;
	pRTCC->PIVECT(i, g, h, P, W);
	if (pRTCC->PLEFEM(5, sv.TS / 3600.0, 0, NULL, NULL, NULL, &L))
	{
		ErrorIndicator = 5;
		return;
	}
	P_apo = tmul(L, P);
	W_apo = tmul(L, W);
	pRTCC->PIVECT(P_apo, W_apo, sv.coe_osc.i, sv.coe_osc.g, sv.coe_osc.h);
	sv.U = sv.coe_osc.g + sv.f;
	if (sv.U >= PI2)
	{
		sv.U -= PI2;
	}
}

void RTCCGeneralPurposeManeuverProcessor::PlaneChange()
{
	double sin_dw, cos_dw, sin_dh, cos_dh, dh, sin_u_b, cos_u_b, sin_u_a, cos_u_a;

	sin_dw = sin(DW);
	cos_dw = cos(DW);
	sin_u_b = sin(sv_b.U);
	cos_u_b = cos(sv_b.U);
	sv_a.coe_osc.i = acos(cos(sv_b.coe_osc.i)*cos_dw - sin(sv_b.coe_osc.i)*sin_dw*cos_u_b);
	sin_dh = sin_dw * sin_u_b / sin(sv_a.coe_osc.i);
	cos_dh = (cos_dw - cos(sv_b.coe_osc.i)*cos(sv_a.coe_osc.i)) / (sin(sv_b.coe_osc.i)*sin(sv_a.coe_osc.i));
	dh = atan(sin_dh / cos_dh);
	sv_a.coe_osc.h = sv_b.coe_osc.h + dh;
	NormalizeAngle(sv_a.coe_osc.h);
	sin_u_a = sin_u_b * sin(sv_b.coe_osc.i) / sin(sv_a.coe_osc.i);
	cos_u_a = cos_u_b * cos_dh + sin_u_b * sin_dh*cos(sv_b.coe_osc.i);
	sv_a.U = atan2(sin_u_a, cos_u_a);
	NormalizeAngle(sv_a.U);
	sv_a.coe_osc.g = sv_a.U - sv_a.f;
	NormalizeAngle(sv_a.coe_osc.g);
}

void RTCCGeneralPurposeManeuverProcessor::FlightControllerInput()
{
	double dv_H_IP, dv_H_OP, dr_dot, x, r_b_dot, v_H_b, r_a_dot, V_a2, E_a, x_a;
	double v_H_a_IP, v_H_a_OP, v_H_a2, e_a2, x_a2;

	dv_H_IP = opt->dV*cos(opt->Pitch)*cos(opt->Yaw);
	dv_H_OP = opt->dV*cos(opt->Pitch)*sin(opt->Yaw);
	dr_dot = opt->dV*sin(opt->Pitch);
	
	x = sqrt(mu*sv_b.coe_osc.a*(1.0 - sv_b.coe_osc.e*sv_b.coe_osc.e));
	r_b_dot = (mu*sv_b.coe_osc.e*sin(sv_b.f)) / x;
	v_H_b = x / sv_b.R;
	v_H_a_IP = v_H_b + dv_H_IP;
	v_H_a_OP = dv_H_OP;
	v_H_a2 = v_H_a_IP * v_H_a_IP + v_H_a_OP * v_H_a_OP;
	r_a_dot = r_b_dot + dr_dot;
	V_a2 = r_a_dot * r_a_dot + v_H_a2;
	sv_a.coe_osc.a = mu * sv_b.R / (2.0*mu - sv_b.R*V_a2);
	e_a2 = 1.0 - (sv_b.R*sv_b.R*v_H_a2) / mu / sv_a.coe_osc.a;
	sv_a.coe_osc.e = sqrt(e_a2);
	E_a = pRTCC->GLQATN(sv_b.R*r_a_dot*sqrt(sv_a.coe_osc.a), (sv_a.coe_osc.a - sv_b.R)*sqrt(mu));
	sv_a.coe_osc.l = E_a - sv_a.coe_osc.e*sin(E_a);
	x_a2 = mu * sv_a.coe_osc.a*(1.0 - e_a2);
	x_a = sqrt(x_a2);
	sv_a.f = pRTCC->GLQATN(x_a*r_a_dot*sv_b.R, x_a2 - mu * sv_b.R);
	DW = atan2(-v_H_a_OP, v_H_a_IP);
}

void RTCCGeneralPurposeManeuverProcessor::HeightManeuver(bool circ)
{
	double DH, dv_h, V_a2, x_a, E_a, V_a, r_b_dot, V_H_a, r_D;
	int IMAX;

	IMAX = 10;

	if (circ)
	{
		sv_a.coe_osc.a = sv_b.R;
		V_a = sqrt(mu / sv_a.coe_osc.a);
		sv_a.coe_osc.e = 0.0;
		sv_a.coe_osc.i = sv_b.coe_osc.i;
		sv_a.coe_osc.l = PI;
		sv_a.coe_osc.h = sv_b.coe_osc.h;
		r_b_dot = 0.0;
		V_H_a = V_a;
		r_D = sv_b.R;
		I = 1;
		sv_a.ENTRY = 0;
		sv_a.coe_osc.g = sv_b.U - sv_a.coe_osc.l;
		if (sv_a.coe_osc.g < 0)
		{
			sv_a.coe_osc.g += PI2;
		}
	}
	else
	{
		I = 0;
		sv_a.coe_osc.a = sv_b.coe_osc.a;
		V_a = sqrt(mu*(2.0 / sv_b.R - 1.0 / sv_a.coe_osc.a));
		double x = sqrt(mu*sv_b.coe_osc.a*(1.0 - sv_b.coe_osc.e*sv_b.coe_osc.e));
		r_b_dot = mu * sv_b.coe_osc.e*sin(sv_b.f) / x;
		V_H_a = x / sv_b.R;
	}

	sv_a.TIMA = 2;
	sv_a.Item8 = sv_b.U + PI;
	if (sv_a.Item8 >= PI2)
	{
		sv_a.Item10 = 1.0;
		sv_a.Item8 -= PI2;
	}
	else
	{
		sv_a.Item10 = 0.0;
	}
	do
	{
		pRTCC->PMMAEGS(aeg.Header, sv_a, sv_temp);
		if (aeg.Header.ErrorInd)
		{
			ErrorIndicator = 3;
			return;
		}
		if (I == 0)
		{
			r_D = sv_temp.R + opt->dH_D;
			DH = opt->dH_D;
		}
		else
		{
			DH = r_D - sv_temp.R;
		}
		if (abs(DH) < eps1 || I > IMAX)
		{
			break;
		}
		dv_h = mu * DH / (4.0*pow(sv_a.coe_osc.a, 2)*V_a);
		//Limit change in DV. Is equation above even correct?
		if (abs(dv_h) > 1000.0)
		{
			if (dv_h > 0.0)
			{
				dv_h = 1000.0;
			}
			else
			{
				dv_h = -1000.0;
			}
		}

		V_H_a = V_H_a + dv_h;
		V_a2 = V_H_a * V_H_a + r_b_dot * r_b_dot;
		V_a = sqrt(V_a2);
		sv_a.coe_osc.a = mu * sv_b.R / (2.0*mu - sv_b.R*V_a2);
		x_a = sv_a.R*sv_a.R*V_H_a*V_H_a;
		sv_a.coe_osc.e = sqrt(1.0 - x_a / mu / sv_a.coe_osc.a);
		//Orbit too eccentric
		if (sv_a.coe_osc.e >= 1.0)
		{
			ErrorIndicator = 9;
			return;
		}

		E_a = pRTCC->GLQATN(sv_b.R*r_b_dot*sqrt(sv_a.coe_osc.a), sqrt(mu)*(sv_a.coe_osc.a - sv_b.R));
		sv_a.coe_osc.l = E_a - sv_a.coe_osc.e*sin(E_a);
		sv_a.f = pRTCC->GLQATN(sv_b.R*r_b_dot*sqrt(x_a), x_a - mu * sv_b.R);
		sv_a.coe_osc.g = sv_b.U - sv_a.f;
		sv_a.ENTRY = 0;
		I++;
		sv_a.coe_osc.i = sv_b.coe_osc.i;
		sv_a.coe_osc.h = sv_b.coe_osc.h;
		if (sv_a.coe_osc.g < 0)
		{
			sv_a.coe_osc.g += PI2;
		}
	} while (I < IMAX);
}

int RTCCGeneralPurposeManeuverProcessor::OptimumApsidesChange()
{
	double r_D, r, DH, V_a;
	int K13;

	K13 = K3;

	I = 0;
	sv_a.f = sv_b.f;
	V_a = sqrt(mu*(2.0 / sv_b.R - 1.0 / sv_b.coe_osc.a));
	sv_a.coe_osc.a = sv_b.coe_osc.a;
	do
	{
		pRTCC->PMMAPD(aeg.Header, sv_a, -K13, 0, INFO, &sv_AP, &sv_PE);
		if (K13 > 0)
		{
			r = sv_PE.R;
		}
		else
		{
			r = sv_AP.R;
		}
		if (I == 0)
		{
			r_D = r + opt->dH_D;
			DH = opt->dH_D;
			//Switch to height logic, if nearly circular
			if (abs(r - sv_b.R) < 0.007*OrbMech::R_Earth)
			{
				return 1;
			}
			if (abs(r_D - sv_b.R) < 0.007*OrbMech::R_Earth)
			{
				return 1;
			}
			if ((code == RTCC_GMP_HPO && r_D < sv_b.R) || (code == RTCC_GMP_HAO && r_D > sv_b.R))
			{
				K13 = -K13;
				sv_a.f = sv_b.f + PI;
				if (sv_a.f > PI2) { sv_a.f -= PI2; }
				sv_a.coe_osc.l = sv_b.coe_osc.l + PI;
				if (sv_a.coe_osc.l > PI2) { sv_a.coe_osc.l -= PI2; }
				sv_a.coe_osc.g = sv_b.coe_osc.g + PI;
				if (sv_a.coe_osc.g > PI2) { sv_a.coe_osc.g -= PI2; }
			}
		}
		else
		{
			DH = r_D - r;
		}
		if (abs(DH) < eps1 || I > 5)
		{
			return 0;
		}
		V_a = V_a + mu * DH / (4.0*pow(sv_a.coe_osc.a, 2)*V_a);
		sv_a.coe_osc.a = mu * sv_b.R / (2.0*mu - sv_b.R*V_a*V_a);
		sv_a.coe_osc.e = sqrt(1.0 - sv_b.R*sv_b.R*V_a*V_a / mu / sv_a.coe_osc.a);
		//sv_a.coe_osc.i = sv_b.coe_osc.i;
		//sv_a.coe_osc.l = sv_b.coe_osc.l;
		//sv_a.coe_osc.g = sv_b.coe_osc.g;
		//sv_a.coe_osc.h = sv_b.coe_osc.h;
		I++;
		sv_a.ENTRY = 0;
	} while (I < 6);

	return 0;
}

void RTCCGeneralPurposeManeuverProcessor::NodeShift()
{
	double i_temp, g_temp, h_temp, u_temp, cos_u_a, sin_u_a, cos_dw, sin_dw;

	if (aeg.Header.AEGInd == BODY_EARTH)
	{
		i_temp = sv_b.coe_osc.i;
		g_temp = sv_b.coe_osc.g;
		h_temp = sv_b.coe_osc.h;
		u_temp = sv_b.U;
	}
	else
	{
		GetSelenographicElements(sv_b, u_temp, i_temp, g_temp, h_temp);
		if (ErrorIndicator) return;
	}

	cos_u_a = cos(opt->dLAN)*cos(u_temp) + sin(opt->dLAN)*sin(u_temp)*cos(i_temp);
	sin_u_a = sqrt(1.0 - pow(cos_u_a, 2));

	if (u_temp > PI)
	{
		sin_u_a = -sin_u_a;
	}
	sv_a.U = pRTCC->GLQATN(sin_u_a , cos_u_a);
	cos_dw = (cos(opt->dLAN) - cos(u_temp)*cos(sv_a.U)) / (sin(u_temp)*sin(sv_a.U));
	sin_dw = sin(opt->dLAN)*sin(i_temp) / sin_u_a;
	DW = atan2(sin_dw, cos_dw);
	sv_a.coe_osc.i = acos(cos(i_temp)*cos_dw - sin(i_temp)*sin_dw*cos(u_temp));
	sv_a.coe_osc.h = h_temp + opt->dLAN;
	sv_a.coe_osc.h = fmod(sv_a.coe_osc.h, PI2);
	sv_a.coe_osc.g = sv_a.U - sv_a.f;
	if (sv_a.coe_osc.h < 0)
	{
		sv_a.coe_osc.h += PI2;
	}
	if (sv_a.coe_osc.g < 0)
	{
		sv_a.coe_osc.g += PI2;
	}
	if (aeg.Header.AEGInd == BODY_MOON)
	{
		GetSelenocentricElements(sv_a.coe_osc.i, sv_a.coe_osc.g, sv_a.coe_osc.h, sv_a);
	}
}

int RTCCGeneralPurposeManeuverProcessor::ApsidesChange(bool limit)
{
	double r_AD, r_PD, dr_a_max, dr_p_max, a_D, e_D, cos_theta_A, theta_A, r_a, r_p, cos_E_a;
	double dr_ap0, dr_ap1, dr_p0, dr_p1, dr_ap_c, dr_p_c, ddr_ap, ddr_p, E_a;
	int err;

	pRTCC->PMMAPD(aeg.Header, sv_b, 0, 0, INFO, &sv_AP, &sv_PE);

	r_AD = R_E + opt->H_A;
	r_PD = R_E + opt->H_P;

	if (r_AD == r_PD)
	{
		r_PD = r_PD - eps1;
	}

	//Limit apoapsis and periapsis to current radius, but only for non multi rev cases
	if (limit)
	{
		if (sv_b.R > r_AD)
		{
			r_AD = sv_b.R;
		}
		if (sv_b.R < r_PD)
		{
			r_PD = sv_b.R;
		}
	}

	//Can maneuver be performed?
	//if (sv_b.R > r_AD || sv_b.R < r_PD)
	//{
	//	return 1;
	//}

	dr_a_max = (sv_PE.coe_osc.a*(1.0 + sv_PE.coe_osc.e) - sv_AP.R)*(r_AD / sv_AP.R);
	dr_p_max = (sv_AP.coe_osc.a*(1.0 - sv_AP.coe_osc.e) - sv_PE.R)*(r_PD / sv_PE.R);
	a_D = (r_AD + r_PD) / 2.0;
	e_D = abs((a_D - r_PD) / a_D);
	cos_theta_A = (a_D*(1.0 - e_D * e_D) - sv_b.R) / (e_D* sv_b.R);
	if (abs(cos_theta_A) > 1.0)
	{
		if (cos_theta_A < 0)
		{
			cos_theta_A = -1.0;
		}
		else
		{
			cos_theta_A = 1.0;
		}
	}
	theta_A = acos(cos_theta_A);
	r_a = r_AD + (PI - theta_A) / PI * dr_a_max;
	r_p = r_PD + theta_A / PI * dr_p_max;
	I = 1;
	do
	{
		sv_a.coe_osc.a = (r_a + r_p) / 2.0;
		sv_a.coe_osc.e = abs((sv_a.coe_osc.a - r_p) / sv_a.coe_osc.a);
		cos_E_a = (sv_a.coe_osc.a - sv_b.R) / (sv_a.coe_osc.a*sv_a.coe_osc.e);

		if (abs(cos_E_a) > 1.0)
		{
			//Maneuver type HAS uses multi rev, don't fail for that case
			if (limit && I > 3)
			{
				//Maneuver cannot be performed
				return 1;
			}

			if (cos_E_a < 0)
			{
				cos_E_a = -1.0;
			}
			else
			{
				cos_E_a = 1.0;
			}
			sv_a.coe_osc.a = sv_b.R / (1.0 - sv_a.coe_osc.e * cos_E_a);
		}

		E_a = acos(cos_E_a);
		sv_a.f = acos2((sv_a.coe_osc.a*(1.0 - sv_a.coe_osc.e * sv_a.coe_osc.e) - sv_b.R) / (sv_a.coe_osc.e*sv_b.R));
		if (sv_b.coe_osc.l > PI)
		{
			E_a = PI2 - E_a;
			sv_a.f = PI2 - sv_a.f;
		}
		sv_a.coe_osc.l = E_a - sv_a.coe_osc.e * sin(E_a);
		sv_a.coe_osc.g = sv_b.U - sv_a.f;
		sv_a.coe_osc.i = sv_b.coe_osc.i;
		sv_a.coe_osc.h = sv_b.coe_osc.h;
		if (sv_a.coe_osc.g < 0)
		{
			sv_a.coe_osc.g += PI2;
		}

		sv_a.ENTRY = 0;

		err = pRTCC->PMMAPD(aeg.Header, sv_a, 0, 0, INFO, &sv_AP, &sv_PE);

		if (aeg.Header.ErrorInd || err)
		{
			ErrorIndicator = 4;
			return 1;
		}

		if (I > 5)
		{
			break;
		}
		if (abs(r_AD - sv_AP.R) < eps1 && abs(r_PD - sv_PE.R) < eps1)
		{
			K3 = 2;
			return 0;
		}

		if (I == 1)
		{
			dr_ap0 = r_AD - sv_AP.R;
			dr_p0 = r_PD - sv_PE.R;
			dr_ap_c = dr_ap0;
			dr_p_c = dr_p0;
		}
		else
		{
			dr_ap1 = r_AD - sv_AP.R;
			dr_p1 = r_PD - sv_PE.R;
			ddr_ap = dr_ap0 - dr_ap1;
			ddr_p = dr_p0 - dr_p1;
			if (abs(ddr_ap) < 0.1)
			{
				dr_ap_c = 0.0;
			}
			else
			{
				dr_ap_c = dr_ap1 * dr_ap_c / ddr_ap;
			}

			if (abs(ddr_p) < 0.1)
			{
				dr_p_c = 0.0;
			}
			else
			{
				dr_p_c = dr_p1 * dr_p_c / ddr_p;
			}

			dr_ap0 = dr_ap1;
			dr_p0 = dr_p1;
		}

		r_a = r_a + dr_ap_c;
		r_p = r_p + dr_p_c;
		I++;

	} while (I <= 5);

	return 1;
}

int RTCCGeneralPurposeManeuverProcessor::OptimumPointForApsidesChange()
{
	double r_AD, r_PD, a_d, e_d, dr_a_max, dr_p_max, R_MD_apo, R_MD, p_d, p_b, cos_theta, theta, r_a_apo, r_p_apo;
	int K1, err;

	pRTCC->PMMAPD(aeg.Header, aeg.Data, 0, 0, INFO, &sv_AP, &sv_PE);

	r_AD = R_E + opt->H_A;
	r_PD = R_E + opt->H_P;

	//If desired apogee is equal to perigee, bias perigee to prevent zero eccentricity
	if (r_AD <= r_PD)
	{
		r_PD -= 0.1*1852.0;
	}

	r_AD = R_E + opt->H_A;
	r_PD = R_E + opt->H_P;

	//Determine whether the maneuver is theoretically possible or not
	if (r_AD < sv_PE.R || r_PD > sv_AP.R)
	{
		return 1;
	}

	//Determine which assumption to use to compute the optimum point
	if (r_AD > sv_AP.R && r_PD > sv_PE.R)
	{
		//Raise both apogee and perigee, optimum is along velocity vector
		K1 = -1;
	}
	else if (r_AD < sv_AP.R && r_PD < sv_PE.R)
	{
		//Lower both apogee and perigee, optimum is along velocity vector
		K1 = -1;
	}
	else
	{
		//Otherwise, change a and e without shifting the line-of-apsides (true anomaly stays the same)
		K1 = 1;
	}

	dr_a_max = (sv_PE.coe_osc.a*(1.0 + sv_PE.coe_osc.e) - sv_AP.R)*(r_AD / sv_AP.R);
	dr_p_max = (sv_AP.coe_osc.a*(1.0 - sv_AP.coe_osc.e) - sv_PE.R)*(r_PD / sv_PE.R);
	a_d = (r_AD + r_PD) / 2.0;
	e_d = abs((a_d - r_PD) / a_d);

	I = 1;
	R_MD_apo = 1.0;
	sv_b = aeg.Data;

	do
	{
		p_d = a_d * (1.0 - e_d * e_d);
		p_b = sv_b.coe_osc.a * (1.0 - sv_b.coe_osc.e * sv_b.coe_osc.e);

		if (K1 < 0)
		{
			R_MD = 2.0*(p_d - p_b)*a_d*sv_b.coe_osc.a / (p_d*a_d - p_b * sv_b.coe_osc.a);
		}
		else
		{
			R_MD = (sv_b.coe_osc.e*p_d - e_d * p_b) / (sv_b.coe_osc.e - e_d);
		}

		if (abs(R_MD - R_MD_apo) < eps1 || I > 5)
		{
			break;
		}
		I++;
		R_MD_apo = R_MD;
		err = pRTCC->PITCIR(aeg.Header, aeg.Data, R_MD_apo, sv_b);
		//Some errors acceptable
		if (err == 1 || err == 2)
		{
			return 1;
		}

		cos_theta = (a_d*(1.0 - e_d* e_d) - R_MD_apo) / (e_d*R_MD_apo);
		if (abs(cos_theta) > 1.0)
		{
			if (cos_theta <= 0)
			{
				cos_theta = -1.0;
			}
			else
			{
				cos_theta = 1.0;
			}
		}

		theta = acos(cos_theta);
		r_a_apo = r_AD + (PI - theta) / PI * dr_a_max;
		r_p_apo = r_PD + theta / PI * dr_p_max;
		a_d = (r_a_apo + r_p_apo) / 2.0;
		e_d = abs((a_d - r_p_apo) / a_d);
	} while (I <= 5);

	return 0;
}

void RTCCGeneralPurposeManeuverProcessor::ApseLineShift(double dang)
{
	double E_a, cos_f, C2, RC;

	sv_a.coe_osc.a = sv_b.coe_osc.a;
	sv_a.f = sv_b.f - dang;
	NormalizeAngle(sv_a.f);

	//Calculate e
	cos_f = cos(sv_a.f);
	C2 = sv_b.R*cos_f;
	RC = C2 * C2 - 4.0*sv_a.coe_osc.a*(sv_b.R - sv_a.coe_osc.a);
	if (RC < 0)
	{
		ErrorIndicator = 1;
		return;
	}
	RC = sqrt(RC);
	if (cos_f < 0)
	{
		RC = -RC;
	}
	sv_a.coe_osc.e = (-C2 + RC) / (2.0*sv_a.coe_osc.a);

	if (sv_a.coe_osc.e < 0)
	{
		ErrorIndicator = 1;
		return;
	}
	sv_a.coe_osc.i = sv_b.coe_osc.i;
	sv_a.coe_osc.h = sv_b.coe_osc.h;
	E_a = 2.0*atan(sqrt((1.0 - sv_a.coe_osc.e) / (1.0 + sv_a.coe_osc.e))*tan(sv_a.f / 2.0));
	NormalizeAngle(E_a);
	sv_a.coe_osc.l = E_a - sv_a.coe_osc.e*sin(E_a);
	sv_a.coe_osc.g = sv_b.U - sv_a.f;
	NormalizeAngle(sv_a.coe_osc.g);
	sv_a.ENTRY = 0;
}

void RTCCGeneralPurposeManeuverProcessor::NormalizeAngle(double &ang)
{
	ang = fmod(ang, PI2);
	if (ang < 0)
	{
		ang += PI2;
	}
}

void RTCCGeneralPurposeManeuverProcessor::ApsidesPlacementNRevsLater()
{
	double r_AD, r_PD, lng_p, dlng, dt, ddt, dlng_apo, eps, w_E, dt_max, l_initial;
	int n, nmax, err;

	ddt = 5.0*60.0;
	dt = 0.0;
	n = 0;
	nmax = 100;
	eps = 0.01*RAD;
	dlng = 1.0;

	r_AD = R_E + opt->H_A;
	r_PD = R_E + opt->H_P;

	if (aeg.Header.AEGInd == BODY_EARTH)
	{
		w_E = OrbMech::w_Earth;
	}
	else
	{
		w_E = OrbMech::w_Moon;
	}

	dt_max = PI2 / aeg.Data.l_dot + 100.0;

	aeg.Data.TIMA = 0;

	//Same initial mean anomaly
	l_initial = aeg.Data.coe_osc.l;

	while (abs(dlng) > eps && n < nmax && dt <= dt_max)
	{
		aeg.Data.TE = aeg.Data.TS + dt;
		pRTCC->PMMAEGS(aeg.Header, aeg.Data, sv_b);
		if (ErrorIndicator) return;
		sv_b_apo = sv_a = sv_b;
		if (ApsidesChange(false) == 0)
		{
			//Find same mean anomaly N revs from now
			sv_a.TIMA = 1;
			sv_a.Item8 = sv_a.coe_osc.l;
			sv_a.Item10 = 1.0 + (double)opt->N;

			//If mean anomaly at maneuver point is smaller than initial mean anomaly, we are in the next rev
			if (sv_b.coe_osc.l < l_initial)
			{
				sv_a.Item10 -= 1.0;
			}

			pRTCC->PMMAEGS(aeg.Header, sv_a, sv_temp);
			//Then find apsides
			err = pRTCC->PMMAPD(aeg.Header, sv_temp, 0, 0, INFO_HAS, &sv_AP, &sv_PE);
			if (err)
			{
				ErrorIndicator = 8;
				return;
			}

			//Longitude of periapsis following maneuver
			lng_p = INFO_HAS[8];
			dlng = OrbMech::calculateDifferenceBetweenAngles(lng_p, opt->long_D);

			//Don't switch sign on ddt on first step. Change when dlng also switches sign, but only when passing through zero
			if (n > 0 && dlng*dlng_apo < 0 && abs(dlng - dlng_apo) < PI05)
			{
				ddt = -ddt / 2.0;
			}
		}

		dt += ddt;
		dlng_apo = dlng;
		n++;
	}

	if (n == nmax || dt > dt_max)
	{
		ErrorIndicator = 2;
	}
}

void RTCCGeneralPurposeManeuverProcessor::OptimumApseLineShift(double dang)
{
	double f_D, u_D;

	f_D = 0.5*opt->dLOA;
	u_D = f_D + aeg.Data.coe_mean.g;
	NormalizeAngle(u_D);
	if (aeg.Data.U < u_D)
	{
		aeg.Data.Item10 = 0.0;
	}
	else
	{
		if (aeg.Data.U < u_D + PI)
		{
			u_D += PI;
			aeg.Data.Item10 = 0.0;
		}
		else
		{
			aeg.Data.Item10 = 1.0;
		}
	}
	aeg.Data.TIMA = 2;
	aeg.Data.Item8 = u_D;
	pRTCC->PMMAEGS(aeg.Header, aeg.Data, sv_b);
	if (aeg.Header.ErrorInd)
	{
		ErrorIndicator = 4;
		return;
	}
}

void RTCCGeneralPurposeManeuverProcessor::GetLatLongHeight(const AEGDataBlock &sv, double &alt, double &lat, double &lng)
{
	alt = sv.R - R_E;
	if (aeg.Header.AEGInd == BODY_EARTH)
	{
		lat = asin(sin(sv.U)*sin(sv.coe_osc.i));
		double DELTA = pRTCC->GLQATN(sin(sv.U)*cos(sv.coe_osc.i), cos(sv.U));
		lng = sv.coe_osc.h + DELTA - pRTCC->SystemParameters.MCLAMD - OrbMech::w_Earth*sv.TS;
		NormalizeAngle(lng);
	}
	else
	{
		double i_temp, g_temp, h_temp, u_temp;
		GetSelenographicElements(sv, u_temp, i_temp, g_temp, h_temp);

		lat = asin(sin(u_temp)*sin(i_temp));
		double DELTA = pRTCC->GLQATN(sin(u_temp)*cos(i_temp), cos(u_temp));
		lng = h_temp + DELTA;
		NormalizeAngle(lng);
	}
	if (lng > PI)
	{
		lng -= PI2;
	}
}