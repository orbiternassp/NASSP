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
	sv1 = pRTCC->coast(opt->sv_in, dt1);

	//Convert to AEG
	aeg = pRTCC->SVToAEG(sv1);

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
		if (opt->AltRef == 0)
		{
		R_E = 6371.0e3;
		}
		else
		{
		R_E = OrbMech::R_Earth;
		}

		mu = OrbMech::mu_Earth;
	}
	else
	{
	if (opt->AltRef == 0)
	{
		R_E = OrbMech::R_Moon;
	}
	else
	{
		R_E = pRTCC->BZLAND.rad[RTCC_LMPOS_BEST];
	}

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
	VECTOR3 DR = R_A - R_B;
	sprintf_s(oapiDebugString(), 128, "%lf %lf %lf", DR.x, DR.y, DR.z);

	if (aeg.Header.AEGInd == BODY_EARTH)
	{
		//For now, back to ecliptic
		R_B = tmul(pRTCC->SystemParameters.MAT_J2000_BRCS, R_B);
		V_B = tmul(pRTCC->SystemParameters.MAT_J2000_BRCS, V_B);
		V_A = tmul(pRTCC->SystemParameters.MAT_J2000_BRCS, V_A);
	}

	pRTCC->PZGPMELM.code = "GPM";
	pRTCC->PZGPMELM.SV_before.R = R_B;
	pRTCC->PZGPMELM.SV_before.V = V_B;
	pRTCC->PZGPMELM.SV_before.GMT = sv_b_apo.TS;
	pRTCC->PZGPMELM.SV_before.RBI = aeg.Header.AEGInd;
	pRTCC->PZGPMELM.V_after = V_A;

	pRTCC->PZGPMDIS.GET_A = pRTCC->GETfromGMT(INFO[0]);
	pRTCC->PZGPMDIS.lat_A = INFO[2];
	pRTCC->PZGPMDIS.long_A = INFO[3];
	pRTCC->PZGPMDIS.HA = INFO[4];

	pRTCC->PZGPMDIS.GET_P = pRTCC->GETfromGMT(INFO[5]);
	pRTCC->PZGPMDIS.lat_P = INFO[7];
	pRTCC->PZGPMDIS.long_P = INFO[8];
	pRTCC->PZGPMDIS.HP = INFO[9];

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
		code == RTCC_GMP_CRA || code == RTCC_GMP_CPA || code == RTCC_GMP_CNA || code == RTCC_GMP_SAA)
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
		if (err != 0)
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
		double f_D, u_D;

		f_D = 0.5*opt->dLOA;
		u_D = f_D + aeg.Data.coe_osc.g;
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
		pRTCC->PMMAEGS(aeg.Header, aeg.Data, sv_b);
		if (aeg.Header.ErrorInd)
		{
			ErrorIndicator = 4;
			return;
		}
	}
	else if (ManeuverPoint == 9)
	{
		ApsidesPlacementNRevsLater();
	}

	if (ErrorIndicator) return;

	//Save elements before maneuver
	sv_b_apo = sv_a = sv_b;

	//Compute maneuver point quantities
	H_man = sv_b.R - R_E;
	if (aeg.Header.AEGInd == BODY_EARTH)
	{
		lat_man = asin(sin(sv_b.U)*sin(sv_b.coe_osc.i));
		double DELTA = pRTCC->GLQATN(sin(sv_b.U)*cos(sv_b.coe_osc.i), cos(sv_b.U));
		lng_man = sv_b.coe_osc.h + DELTA - pRTCC->SystemParameters.MCLAMD - OrbMech::w_Earth*sv_b.TS;
		NormalizeAngle(lng_man);
	}
	else
	{
		double i_temp, g_temp, h_temp, u_temp;
		GetSelenographicElements(sv_b, u_temp, i_temp, g_temp, h_temp);

		lat_man = asin(sin(u_temp)*sin(i_temp));
		double DELTA = pRTCC->GLQATN(sin(u_temp)*cos(i_temp), cos(u_temp));
		lng_man = h_temp + DELTA;
		NormalizeAngle(lng_man);
	}
	if (lng_man > PI)
	{
		lng_man -= PI2;
	}

	//Flight controller input
	if (ManeuverType == 0)
	{
		double f_a;
		sv_a.coe_osc = FlightControllerInput(sv_b.coe_osc, sv_b.R, sv_b.f, sv_b.U, opt->dV, opt->Pitch, opt->Yaw, f_a, DW);
		sv_a.coe_osc = PlaneChange(sv_a.coe_osc, f_a, sv_b.U, DW);
	}
	//Plane change
	else if (ManeuverType == 1)
	{
		DW = opt->dW;
		sv_a.coe_osc = PlaneChange(sv_b.coe_osc, sv_b.f, sv_b.U, DW);
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
		int itemp = OptimumApsidesChange();
		if (itemp == 1)
		{
			//Switch to height maneuver logic
			HeightManeuver(false);
		}
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
		ApseLineShiftToLongitude();
		DW = 0.0;
	}
	//Plane change and height maneuver
	else if (ManeuverType == 10)
	{
		sv_a.coe_osc = PlaneChange(sv_b.coe_osc, sv_b.f, sv_b.U, DW);
		sv_b = sv_a;
		HeightManeuver(false);
	}
	//Circularization and plane change
	else if (ManeuverType == 11)
	{
		sv_a.coe_osc = PlaneChange(sv_b.coe_osc, sv_b.f, sv_b.U, DW);
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

		sv_a.TIMA = 1;
		sv_a.Item8 = PI;
		sv_a.Item10 = (double)opt->N;
		pRTCC->PMMAEGS(aeg.Header, sv_a, sv_temp);
		pRTCC->PMMAPD(aeg.Header, sv_temp, 0, 0, INFO, &sv_AP, &sv_PE);
		K3 = 2;
	}

	if (ErrorIndicator) return;

	//Calculate pitch and yaw for output
	double x_b, r_b_dot, r_a_dot, v_H_a, dv_H_OP, dv_H_IP, dr_dot, v_H_b, x_a;
	x_b = sqrt(mu*(sv_b_apo.coe_osc.a)*(1.0 - sv_b_apo.coe_osc.e*sv_b_apo.coe_osc.e));
	r_b_dot = (mu*sv_b_apo.coe_osc.e*sin(sv_b_apo.f)) / x_b;
	v_H_b = x_b / sv_b_apo.R;
	x_a = sqrt(mu*(sv_a.coe_osc.a)*(1.0 - sv_a.coe_osc.e*sv_a.coe_osc.e));
	r_a_dot = (mu*sv_a.coe_osc.e*sin(sv_a.f)) / x_b;
	v_H_a = x_a / sv_b.R;

	dv_H_OP = -v_H_a * sin(DW);
	dv_H_IP = v_H_a * cos(DW) - v_H_b;
	dr_dot = r_a_dot - r_b_dot;
	DV_Vector = _V(dv_H_IP, dv_H_OP, dr_dot);
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
		Node_Ang = 0.0;
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
	OrbMech::PIVECT(sv.coe_osc.i, sv.coe_osc.g, sv.coe_osc.h, P, W);
	if (pRTCC->PLEFEM(1, sv.TS / 3600.0, 0, L))
	{
		ErrorIndicator = 5;
		return;
	}
	P_apo = tmul(L, P);
	W_apo = tmul(L, W);
	OrbMech::PIVECT(P_apo, W_apo, i, g, h);
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
	OrbMech::PIVECT(i, g, h, P, W);
	if (pRTCC->PLEFEM(1, sv.TS / 3600.0, 0, L))
	{
		ErrorIndicator = 5;
		return;
	}
	P_apo = mul(L, P);
	W_apo = mul(L, W);
	OrbMech::PIVECT(P_apo, W_apo, sv.coe_osc.i, sv.coe_osc.g, sv.coe_osc.h);
	sv.U = sv.coe_osc.g + sv.f;
	if (sv.U >= PI2)
	{
		sv.U -= PI2;
	}
}

CELEMENTS RTCCGeneralPurposeManeuverProcessor::PlaneChange(CELEMENTS coe, double f_b, double u_b, double dw)
{
	CELEMENTS out;
	double sin_dw, cos_dw, sin_dh, cos_dh, dh, sin_u_b, cos_u_b, sin_u_a, cos_u_a, u_a;

	sin_dw = sin(dw);
	cos_dw = cos(dw);
	sin_u_b = sin(u_b);
	cos_u_b = cos(u_b);
	out.i = acos(cos(coe.i)*cos_dw - sin(coe.i)*sin_dw*cos_u_b);
	sin_dh = sin_dw * sin_u_b / sin(out.i);
	cos_dh = (cos_dw - cos(coe.i)*cos(out.i)) / (sin(coe.i)*sin(out.i));
	dh = atan(sin_dh / cos_dh);
	out.h = coe.h + dh;
	out.h = fmod(out.h, PI2);
	sin_u_a = sin_u_b * sin(coe.i) / sin(out.i);
	cos_u_a = cos_u_b * cos_dh + sin_u_b * sin_dh*cos(coe.i);
	u_a = atan2(sin_u_a, cos_u_a);
	out.g = u_a - f_b;
	if (out.g < 0)
	{
		out.g += PI2;
	}

	out.a = coe.a;
	out.e = coe.e;
	out.l = coe.l;
	return out;
}

CELEMENTS RTCCGeneralPurposeManeuverProcessor::FlightControllerInput(CELEMENTS coe, double r_b, double f_b, double u_b, double dv, double p, double y, double &f_a, double &dw)
{
	CELEMENTS out;
	double dv_H_IP, dv_H_OP, dr_dot, x, r_b_dot, v_H_b, r_a_dot, V_a2, E_a, x_a;
	double v_H_a_IP, v_H_a_OP, v_H_a2, e_a2, x_a2;

	out = coe;

	dv_H_IP = dv*cos(p)*cos(y);
	dv_H_OP = dv*cos(p)*sin(y);
	dr_dot = dv*sin(p);
	
	x = sqrt(mu*coe.a*(1.0 - coe.e*coe.e));
	r_b_dot = (mu*coe.e*sin(f_b)) / x;
	v_H_b = x / r_b;
	v_H_a_IP = v_H_b + dv_H_IP;
	v_H_a_OP = dv_H_OP;
	v_H_a2 = v_H_a_IP * v_H_a_IP + v_H_a_OP * v_H_a_OP;
	r_a_dot = r_b_dot + dr_dot;
	V_a2 = r_a_dot * r_a_dot + v_H_a2;
	out.a = mu * r_b / (2.0*mu - r_b*V_a2);
	e_a2 = 1.0 - (r_b*r_b*v_H_a2) / mu / out.a;
	out.e = sqrt(e_a2);
	E_a = pRTCC->GLQATN(r_b*r_a_dot*sqrt(out.a), (out.a - r_b)*sqrt(mu));
	out.l = E_a - out.e*sin(E_a);
	x_a2 = mu * out.a*(1.0 - e_a2);
	x_a = sqrt(x_a2);
	f_a = atan(x_a*r_a_dot*r_b / (x_a2 - mu * r_b));
	dw = atan2(-v_H_a_OP, v_H_a_IP);
	return out;
}

void RTCCGeneralPurposeManeuverProcessor::HeightManeuver(bool circ)
{
	double DH, dv_h, V_a2, x_a, E_a, V_a, r_b_dot, V_H_a, r_D;

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
		if (abs(DH) < eps1 || I > 5)
		{
			break;
		}
		dv_h = mu * DH / (4.0*pow(sv_a.coe_osc.a, 2)*V_a);
		V_H_a = V_H_a + dv_h;
		V_a2 = V_H_a * V_H_a + r_b_dot * r_b_dot;
		V_a = sqrt(V_a2);
		sv_a.coe_osc.a = mu * sv_b.R / (2.0*mu - sv_b.R*V_a2);
		x_a = sv_a.R*sv_a.R*V_H_a*V_H_a;
		sv_a.coe_osc.e = sqrt(1.0 - x_a / mu / sv_a.coe_osc.a);
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
	} while (I < 5);
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
	sv_a.U = atan2(sin_u_a , cos_u_a);
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

int RTCCGeneralPurposeManeuverProcessor::ApsidesChange()
{
	double r_AD, r_PD, dr_a_max, dr_p_max, a_D, e_D, cos_theta_A, theta_A, r_a, r_p, cos_E_a;
	double dr_ap0, dr_ap1, dr_p0, dr_p1, dr_ap_c, dr_p_c, ddr_ap, ddr_p, E_a;

	pRTCC->PMMAPD(aeg.Header, sv_b, 0, 0, INFO, &sv_AP, &sv_PE);

	r_AD = R_E + opt->H_A;
	r_PD = R_E + opt->H_P;

	if (r_AD == r_PD)
	{
		r_PD = r_PD - eps1;
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
			if (I > 3)
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
		pRTCC->PMMAPD(aeg.Header, sv_a, 0, 0, INFO, &sv_AP, &sv_PE);

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

	if (r_AD < sv_PE.R || r_PD > sv_AP.R)
	{
		return 1;
	}

	if (r_PD > sv_AP.R && r_PD > sv_PE.R)
	{
		K1 = -1;
	}
	else if (r_PD < sv_AP.R && r_PD < sv_PE.R)
	{
		K1 = -1;
	}
	else
	{
		K1 = 1;
	}

	dr_a_max = (sv_PE.coe_osc.a*(1.0 + sv_PE.coe_osc.e) - sv_AP.R)*(r_AD / sv_AP.R);
	dr_p_max = (sv_AP.coe_osc.a*(1.0 - sv_AP.coe_osc.e) - sv_PE.R)*(r_PD / sv_PE.R);
	a_d = (r_AD + r_PD) / 2.0;
	e_d = abs((a_d - r_PD) / a_d);

	I = 1;
	R_MD_apo = 1.0;

	do
	{
		p_d = a_d * (1.0 - e_d * e_d);
		p_b = aeg.Data.coe_osc.a * (1.0 - aeg.Data.coe_osc.e * aeg.Data.coe_osc.e);

		if (K1 < 0)
		{
			R_MD = 2.0*(p_d - p_b)*a_d*aeg.Data.coe_osc.a / (p_d*a_d - p_b * aeg.Data.coe_osc.a);
		}
		else
		{
			R_MD = (aeg.Data.coe_osc.e*p_d - e_d * p_b) / (aeg.Data.coe_osc.e - e_d);
		}

		if (abs(R_MD - R_MD_apo) < eps1 || I > 5)
		{
			break;
		}
		I++;
		R_MD_apo = R_MD;
		err = pRTCC->PITCIR(aeg.Header, aeg.Data, R_MD_apo, aeg.Data);
		//Some errors acceptable
		if (err == 1 || err == 2)
		{
			return 1;
		}

		cos_theta = (a_d*(1.0 - e_d) - R_MD_apo) / (e_d*R_MD_apo);
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

	sv_b = aeg.Data;
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

void RTCCGeneralPurposeManeuverProcessor::ApseLineShiftToLongitude()
{
	double eps, dLOA, ddLOA, lng_p;
	int n, nmax;

	n = 0;
	nmax = 20;
	eps = 0.01*RAD;
	dLOA = 0.0;
	ddLOA = 1.0;

	while (abs(ddLOA) > eps && n < nmax)
	{
		pRTCC->PMMAPD(aeg.Header, sv_a, -1, 0, INFO, NULL, &sv_PE);
		lng_p = INFO[8];

		ddLOA = OrbMech::calculateDifferenceBetweenAngles(lng_p, opt->long_D);
		dLOA += ddLOA;

		ApseLineShift(dLOA);
		if (ErrorIndicator) return;
		n++;
	}

	if (n == nmax)
	{
		ErrorIndicator = 1;
	}
}

void RTCCGeneralPurposeManeuverProcessor::ApsidesPlacementNRevsLater()
{
	double r_AD, r_PD, lng_p, dlng, dt, ddt, dlng_apo, eps, w_E, dt_max;
	int n, nmax;

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

	while (abs(dlng) > eps && n < nmax && dt <= dt_max)
	{
		aeg.Data.TE = aeg.Data.TS + dt;
		pRTCC->PMMAEGS(aeg.Header, aeg.Data, sv_b);
		if (ErrorIndicator) return;
		sv_b_apo = sv_a = sv_b;
		if (ApsidesChange() == 0)
		{
			//DV = ApoapsisPeriapsisChangeInteg(sv2, r_AD, r_PD);
			//sv2_apo = sv2;
			//sv2_apo.V += DV;
			//T_P = OrbMech::REVTIM(sv2_apo.R, sv2_apo.V, sv2_apo.MJD, body, true);

			//pRTCC->PMMAPD(aeg.Header,sv_a,)
			//OrbMech::latlong_from_J2000(sv_p.R, sv_p.MJD, sv_p.gravref, lat_p, lng_p);
			//lng_p += -T_P * w_E*(double)opt->N;
			//lng_p = fmod(lng_p, PI2);

			//Longitude of periapsis following maneuver
			lng_p = INFO[8];
			lng_p += -PI2 / (sv_a.l_dot + sv_a.g_dot + sv_a.h_dot)*w_E*(double)opt->N;
			NormalizeAngle(lng_p);

			dlng = OrbMech::calculateDifferenceBetweenAngles(lng_p, opt->long_D);

			if (n > 0 && dlng*dlng_apo < 0 && abs(dlng_apo) < PI05)
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