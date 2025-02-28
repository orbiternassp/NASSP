/***************************************************************************
  This file is part of Project Apollo - NASSP

  MCC Calculations

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

#include "MCC_Calculations.h"
#include "rtcc.h"

MCC_Calculations::MCC_Calculations(RTCC *r) : RTCCModule(r)
{

}

bool MCC_Calculations::CreateEphemeris(EphemerisData sv, double EphemerisLeftLimitGMT, double EphemerisRightLimitGMT, EphemerisDataTable2 &ephem)
{
	EMSMISSInputTable in;
	PLAWDTOutput weights;

	in.AnchorVector = sv;
	in.EphemerisLeftLimitGMT = EphemerisLeftLimitGMT;
	in.EphemerisRightLimitGMT = EphemerisRightLimitGMT;

	in.EphemerisBuildIndicator = true;
	if (sv.RBI == BODY_EARTH)
	{
		in.ECIEphemerisIndicator = true;
		in.ECIEphemTableIndicator = &ephem;
	}
	else
	{
		in.MCIEphemerisIndicator = true;
		in.MCIEphemTableIndicator = &ephem;
	}
	in.useInputWeights = true;

	//TBD
	weights.KFactor = 0.0;
	weights.CC.set(0);
	weights.CSMWeight = 1.0;

	in.DensityMultiplier = 0.0;
	in.WeightsTable = &weights;
	in.VehicleCode = RTCC_MPT_CSM; //Not used

	pRTCC->EMSMISS(&in);

	ephem.Header.TUP = 1; //Only has to be non-zero

	return (in.NIAuxOutputTable.ErrorCode != 0);
}
double MCC_Calculations::EnvironmentChange(EphemerisDataTable2 &ephem, double gmt_estimate, int option, bool present, bool terminator)
{
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNRSTAY;
	RTCC::EMMENVInputTable in;
	RTCC::EMMENVOutputTable out;

	in.GMT = gmt_estimate;
	in.option = option;
	in.present = present;
	in.terminator = terminator;

	pRTCC->EMMENV(ephem, MANTIMES, &LUNRSTAY, in, out);

	return out.T_Change;
}

double MCC_Calculations::Sunrise(EphemerisDataTable2 &ephem, double gmt_estimate)
{
	return EnvironmentChange(ephem, gmt_estimate, 0, true, false);
}

double MCC_Calculations::TerminatorRise(EphemerisDataTable2 &ephem, double gmt_estimate)
{
	return EnvironmentChange(ephem, gmt_estimate, 0, true, true);
}

bool MCC_Calculations::LongitudeCrossing(EphemerisDataTable2 &ephem, double lng, double gmt_estimate, double &gmt_cross)
{
	ManeuverTimesTable MANTIMES;
	EphemerisDataTable2 ephem_true;
	EphemerisData2 sv;
	double dErr;
	int in, out, iErr;

	gmt_cross = 0.0;

	//Convert to ECT or MCT table
	ephem_true = ephem;

	in = ephem.Header.CSI;
	out = in + 1;

	iErr = pRTCC->ELVCNV(ephem.table, in, out, ephem_true.table);
	if (iErr) return true;

	ephem_true.Header.CSI = out;

	//Longitude crossing
	dErr = pRTCC->RLMTLC(ephem_true, MANTIMES, lng, gmt_estimate, gmt_cross, sv);

	return (dErr < 0.0);
}

bool MCC_Calculations::GETEval(double get)
{
	if (OrbMech::GETfromMJD(oapiGetSimMJD(), pRTCC->CalcGETBase()) > get)
	{
		return true;
	}

	return false;
}

double MCC_Calculations::FindOrbitalSunrise(SV sv, double t_sunrise_guess)
{
	SV sv1;
	double GET_SV, dt, ttoSunrise;

	OBJHANDLE hSun = oapiGetObjectByName("Sun");

	GET_SV = OrbMech::GETfromMJD(sv.MJD, pRTCC->CalcGETBase());
	dt = t_sunrise_guess - GET_SV;

	sv1 = pRTCC->coast(sv, dt);

	ttoSunrise = OrbMech::sunrise(pRTCC->SystemParameters.MAT_J2000_BRCS, sv1.R, sv1.V, sv1.MJD, sv1.gravref, hSun, true, false, false);
	return t_sunrise_guess + ttoSunrise;
}

double MCC_Calculations::FindOrbitalMidnight(SV sv, double t_TPI_guess)
{
	SV sv1;
	double GET_SV, dt, ttoMidnight;

	OBJHANDLE hSun = oapiGetObjectByName("Sun");

	GET_SV = OrbMech::GETfromMJD(sv.MJD, pRTCC->CalcGETBase());
	dt = t_TPI_guess - GET_SV;

	sv1 = pRTCC->coast(sv, dt);

	ttoMidnight = OrbMech::sunrise(pRTCC->SystemParameters.MAT_J2000_BRCS, sv1.R, sv1.V, sv1.MJD, sv1.gravref, hSun, 1, 1, false);
	return t_TPI_guess + ttoMidnight;
}

void MCC_Calculations::FindRadarAOSLOS(SV sv, double lat, double lng, double &GET_AOS, double &GET_LOS)
{
	VECTOR3 R_P;
	double LmkRange, dt1, dt2;

	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)))*oapiGetSize(sv.gravref);

	dt1 = OrbMech::findelev_gs(pRTCC->SystemParameters.AGCEpoch, pRTCC->SystemParameters.MAT_J2000_BRCS, sv.R, sv.V, R_P, sv.MJD, 175.0*RAD, sv.gravref, LmkRange);
	dt2 = OrbMech::findelev_gs(pRTCC->SystemParameters.AGCEpoch, pRTCC->SystemParameters.MAT_J2000_BRCS, sv.R, sv.V, R_P, sv.MJD, 5.0*RAD, sv.gravref, LmkRange);

	GET_AOS = OrbMech::GETfromMJD(sv.MJD, pRTCC->CalcGETBase()) + dt1;
	GET_LOS = OrbMech::GETfromMJD(sv.MJD, pRTCC->CalcGETBase()) + dt2;
}

int MCC_Calculations::SPSRCSDecision(double a, VECTOR3 dV_LVLH)
{
	double t;

	t = length(dV_LVLH) / a;

	if (t > 0.5)
	{
		return RTCC_ENGINETYPE_CSMSPS;
	}
	else
	{
		return RTCC_ENGINETYPE_CSMRCSPLUS4;
	}
}

bool MCC_Calculations::REFSMMATDecision(VECTOR3 Att)
{
	if (cos(Att.z) > 0.5) //Yaw between 300° and 60°
	{
		return true;
	}

	return false;
}

void MCC_Calculations::PrelaunchMissionInitialization()
{
	//Assumes mission file has been loaded. Also GZGENCSN.MonthofLiftoff, GZGENCSN.DayofLiftoff, GZGENCSN.Year in the scenario.

	char Buff[128];

	//P80 MED: mission initialization
	sprintf_s(Buff, "P80,1,CSM,%d,%d,%d;", pRTCC->GZGENCSN.MonthofLiftoff, pRTCC->GZGENCSN.DayofLiftoff, pRTCC->GZGENCSN.Year);
	pRTCC->GMGMED(Buff);
}

void MCC_Calculations::DMissionRendezvousPlan(SV sv_A0, double &t_TPI0)
{
	SV sv2;

	//Step 1: Find TPI0 time (25 minutes before sunrise)
	double TPI0_guess, TPI0_sunrise_guess, TPI0_sunrise, dt_sunrise;
	dt_sunrise = 25.0*60.0;
	TPI0_guess = OrbMech::HHMMSSToSS(95, 0, 0);
	TPI0_sunrise_guess = TPI0_guess + dt_sunrise;
	TPI0_sunrise = FindOrbitalSunrise(sv_A0, TPI0_sunrise_guess);
	t_TPI0 = TPI0_sunrise - dt_sunrise;

	//Step 2: Phasing is 70 minutes before TPI0
	pRTCC->calcParams.Phasing = t_TPI0 - 70.0*60.0;

	//Step 3: Insertion is 111:42 minutes after Phasing
	pRTCC->calcParams.Insertion = pRTCC->calcParams.Phasing + 111.0*60.0 + 42.0;

	//Step 4: CSI is two minutes (rounded) after 5° AOS of the TAN pass
	double CSI_guess, lat_TAN, lng_TAN, AOS_TAN, LOS_TAN;
	lat_TAN = groundstations[13][0];
	lng_TAN = groundstations[13][1];
	CSI_guess = pRTCC->calcParams.Insertion + 40.0*60.0;
	sv2 = pRTCC->coast(sv_A0, CSI_guess - OrbMech::GETfromMJD(sv_A0.MJD, pRTCC->CalcGETBase()));
	FindRadarAOSLOS(sv2, lat_TAN, lng_TAN, AOS_TAN, LOS_TAN);
	pRTCC->calcParams.CSI = (floor(AOS_TAN / 60.0) + 2.0)*60.0;

	//Step 5: CDH is placed 44.4 minutes after CSI
	pRTCC->calcParams.CDH = pRTCC->calcParams.CSI + 44.4*60.0;

	//Step 6: Find TPI0 time (25 minutes before sunrise)
	double TPI_guess, TPI_sunrise_guess, TPI_sunrise;
	TPI_guess = OrbMech::HHMMSSToSS(98, 0, 0);
	TPI_sunrise_guess = TPI_guess + dt_sunrise;
	TPI_sunrise = FindOrbitalSunrise(sv_A0, TPI_sunrise_guess);
	pRTCC->calcParams.TPI = TPI_sunrise - dt_sunrise;
}

void MCC_Calculations::FMissionRendezvousPlan(VESSEL *chaser, VESSEL *target, SV sv_A0, double t_TIG, double t_TPI, double &t_Ins, double &CSI)
{
	//Plan: Phasing (fixed TIG), Insertion, CSI 50 minutes after Insertion, CDH, TPI at orbital midnight (Apollo 10)

	TwoImpulseOpt lamopt, lamopt2;
	TwoImpulseResuls lamres;
	double GETbase, t_sv0, t_Phasing, t_Insertion, dt, t_CSI, ddt, T_P, dv_CSI, t_CDH, dt_TPI, t_TPI_apo;
	VECTOR3 dV_Phasing, dV_Insertion, R_P_CDH1, V_P_CDH1;
	SV sv_P0, sv_P_CSI, sv_Phasing, sv_Phasing_apo, sv_Insertion, sv_Insertion_apo, sv_CSI, sv_CSI_apo, sv_CDH, sv_CDH_apo, sv_P_CDH;

	//Constants
	const double dt2 = 50.0*60.0; //Insertion to CSI
	const double DH = 15.0*1852.0;

	GETbase = pRTCC->CalcGETBase();
	t_Phasing = t_TIG;
	dt = 7017.0; //Phasing to Insertion
	dv_CSI = 50.0*0.3048;
	ddt = 10.0;

	sv_P0 = pRTCC->StateVectorCalc(target);

	lamopt.mode = 5; //External request
	lamopt.DH = 60.0*1852.0 - 60000.0*0.3048; //Aiming for 60000 ft altitude
	lamopt.PhaseAngle = 15.509*RAD; //270 NM behind CSM
	lamopt.T1 = pRTCC->GMTfromGET(t_Phasing);
	lamopt.sv_P = pRTCC->ConvertSVtoEphemData(sv_P0);

	lamopt2 = lamopt;
	lamopt2.DH = 14.7*1852.0; //14.7 NM
	lamopt2.PhaseAngle = 8.4436*RAD; //147 NM behind

	t_sv0 = OrbMech::GETfromMJD(sv_A0.MJD, GETbase);
	sv_Phasing = pRTCC->coast(sv_A0, t_Phasing - t_sv0);

	//Loop
	while (abs(ddt) > 1.0)
	{
		//Phasing Targeting
		t_Insertion = t_Phasing + dt;

		lamopt.T2 = pRTCC->GMTfromGET(t_Insertion);
		lamopt.sv_A = pRTCC->ConvertSVtoEphemData(sv_Phasing);

		pRTCC->PMSTICN(lamopt, lamres);
		dV_Phasing = lamres.dV;

		sv_Phasing_apo = sv_Phasing;
		sv_Phasing_apo.V += dV_Phasing;

		//Insertion Targeting
		t_CSI = t_Insertion + dt2;

		lamopt2.T1 = pRTCC->GMTfromGET(t_Insertion);
		lamopt2.T2 = pRTCC->GMTfromGET(t_CSI);
		lamopt2.sv_A = pRTCC->ConvertSVtoEphemData(sv_Phasing_apo);

		pRTCC->PMSTICN(lamopt2, lamres);
		dV_Insertion = lamres.dV;

		sv_Insertion = pRTCC->coast(sv_Phasing_apo, t_Insertion - t_Phasing);
		sv_Insertion_apo = sv_Insertion;
		sv_Insertion_apo.V += dV_Insertion;

		sv_CSI = pRTCC->coast(sv_Insertion_apo, t_CSI - t_Insertion);

		//CSI Targeting
		sv_P_CSI = pRTCC->coast(sv_P0, t_CSI - OrbMech::GETfromMJD(sv_P0.MJD, GETbase));
		OrbMech::CSIToDH(sv_CSI.R, sv_CSI.V, sv_P_CSI.R, sv_P_CSI.V, DH, OrbMech::mu_Moon, dv_CSI);
		sv_CSI_apo = sv_CSI;
		sv_CSI_apo.V = sv_CSI.V + OrbMech::ApplyHorizontalDV(sv_CSI.R, sv_CSI.V, dv_CSI);

		//CDH Targeting
		T_P = OrbMech::period(sv_CSI_apo.R, sv_CSI_apo.V, OrbMech::mu_Moon);
		t_CDH = t_CSI + T_P / 2.0;
		sv_CDH = pRTCC->coast(sv_CSI_apo, t_CDH - t_CSI);
		sv_CDH_apo = sv_CDH;
		sv_P_CDH = pRTCC->coast(sv_P_CSI, t_CDH - t_CSI);
		OrbMech::RADUP(sv_P_CDH.R, sv_P_CDH.V, sv_CDH.R, OrbMech::mu_Moon, R_P_CDH1, V_P_CDH1);
		sv_CDH_apo.V = OrbMech::CoellipticDV(sv_CDH.R, R_P_CDH1, V_P_CDH1, OrbMech::mu_Moon);

		//Find TPI time and recycle
		dt_TPI = OrbMech::findelev(pRTCC->SystemParameters.AGCEpoch, sv_CDH_apo.R, sv_CDH_apo.V, sv_P_CDH.R, sv_P_CDH.V, sv_CDH_apo.MJD, 26.6*RAD, sv_CDH_apo.gravref);
		t_TPI_apo = t_CDH + dt_TPI;
		ddt = t_TPI - t_TPI_apo;
		dt += ddt;
	}

	t_Ins = t_Insertion;
	CSI = t_CSI;

	/*
	//Debug prints
	SV sv_before, sv_after;
	MATRIX3 Rot;
	VECTOR3 DV_LVLH;
	double tig, r_apo, r_peri, h_apo, h_peri;
	char Buffer[128], Buffer2[128];

	for (int i = 0; i < 4; i++)
	{
		if (i == 0)
		{
			sprintf(Buffer, "Phasing");
			tig = t_TIG;
			sv_before = sv_Phasing;
			sv_after = sv_Phasing_apo;
		}
		else if (i == 1)
		{
			sprintf(Buffer, "Insertion");
			tig = t_Insertion;
			sv_before = sv_Insertion;
			sv_after = sv_Insertion_apo;
		}
		else if (i == 2)
		{
			sprintf(Buffer, "CSI");
			tig = t_CSI;
			sv_before = sv_CSI;
			sv_after = sv_CSI_apo;
		}
		else if (i == 3)
		{
			sprintf(Buffer, "CDH");
			tig = t_CDH;
			sv_before = sv_CDH;
			sv_after = sv_CDH_apo;
		}

		oapiWriteLog(Buffer);

		OrbMech::format_time_HHMMSS(Buffer2, tig);
		Rot = OrbMech::LVLH_Matrix(sv_before.R, sv_before.V);
		DV_LVLH = mul(Rot, sv_after.V - sv_before.V) / 0.3048;

		sprintf(Buffer, "TIG %s DV %.1lf %.1lf %.1lf", Buffer2, DV_LVLH.x, DV_LVLH.y, DV_LVLH.z);
		oapiWriteLog(Buffer);
		OrbMech::periapo(sv_after.R, sv_after.V, OrbMech::mu_Moon, r_apo, r_peri);
		h_apo = r_apo - pRTCC->BZLAND.rad[0];
		h_peri = r_peri - pRTCC->BZLAND.rad[0];
		sprintf(Buffer, "HA %.2lf HP %.2lf", h_apo / 1852.0, h_peri / 1852.0);
		oapiWriteLog(Buffer);
	}

	oapiWriteLog("TPI");
	OrbMech::format_time_HHMMSS(Buffer2, t_TPI);
	sprintf(Buffer, "TIG %s", Buffer2);
	oapiWriteLog(Buffer);
	*/
}