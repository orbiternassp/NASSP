/****************************************************************************
This file is part of Project Apollo - NASSP

Two Impulse Processor, RTCC Module PMSTICN

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

#include "TwoImpulseProcessor.h"
#include "rtcc.h"

TwoImpulseOpt::TwoImpulseOpt()
{
	ChaserVehicle = 1;
	RequestIndicator = 0;
	ChaserVectorTime = TargetVectorTime = 0.0;
	T1 = T2 = 0.0;

	DH_min = DH_max = DH_inc = T2_min = T2_max = dt_inc = 0.0;

	TimeStep = 10.0;
	TimeRange = 0.0;

	DH = 0.0;
	PhaseAngle = 0.0;
	WT = 0.0;
	Elev = 0.0;

	TwoImpulseTableIndicator = 1;
	PlanNumber = 1;
}

TwoImpulseSingleSolutionTable::TwoImpulseSingleSolutionTable()
{
	LM_GMTTH = 0.0;
	CSM_GMTTH = 0.0;
	MAN_VEH = 1;
	PointingMode = 1;
	PlanNumber = 0;
	TwoImpulseTableIndicator = 1;
}

TwoImpulseProcessor::TwoImpulseProcessor(RTCC *r) : RTCCModule(r)
{
	T1 = T2 = 0.0;
}

void TwoImpulseProcessor::PMSTICN(const TwoImpulseOpt &opt, TwoImpulseResuls &res)
{
	//Initialization
	this->opt = opt;

	if (opt.mode == 1 || opt.mode == 2)
	{
		//CC or MS request
		//Load chaser and target state vectors, area, weight, drag, STAID
		sv_C1 = opt.sv_C;
		sv_T1 = opt.sv_T;
	}

	switch (opt.mode)
	{
	case 1: //Corrective Combination
		CorrectiveCombination();
		break;
	case 2: //Multiple Solution
		MultipleSolution();
		break;
	case 3: //Single Solution
	case 4: //Transfer Plan
		SingleSolutionTransferPlan();
		break;
	case 5: //External request
		ExternalRequest(res);
		break;
	}
}

void TwoImpulseProcessor::CorrectiveCombination()
{

}

void TwoImpulseProcessor::MultipleSolution()
{
	TwoImpulseMultipleSolutionTableEntry entry[13];
	VECTOR3 DV_LVLH;
	double DVT, TMAX, DV_opt, T_c, T_c_apo, DT_Light;
	int soln, err, OPCASE, PMMDAN_ERR;

	soln = 0;
	DV_opt = 10000000.0;
	OPCASE = 1;

	//Load MED request parameters
	T1 = opt.T1;
	T2 = opt.T2;
	//Get requested height and phase offsets, elevation angle and target terminal transfer angle
	DH = pRTCC->GZGENCSN.TIDeltaH;
	PhaseAngle = pRTCC->GZGENCSN.TIPhaseAngle;
	Elev = pRTCC->GZGENCSN.TIElevationAngle;
	WT = pRTCC->GZGENCSN.TITravelAngle;
	//Set TI Table to updating condition
	pRTCC->PZTIPREG.Updating = true;
	//Load TI table area with necessary MED quantities
	if (DH != 0.0 && PhaseAngle != 0.0 && Elev != 0.0 && WT != 0.0 && T1 > 0 && T2 > 0)
	{
		pRTCC->PZTIPREG.showTPI = true;
	}
	else
	{
		pRTCC->PZTIPREG.showTPI = false;
	}	
	//Was time of 1st maneuver input?
	if (T1 < 0.0)
	{
		//Use elevation angle search routine
		if (ElevationAngleSearch(sv_C1, sv_T1, Elev, sv_C1, sv_T1, T1))
		{
			//Error
			pRTCC->PMXSPT("PMSTICN", 30);
			return;
		}
		//Is time of first maneuver greater than time of second maneuver?
		if (T1 > T2)
		{
			T2 = T2Search();
		}
	}
	//Was time of 2nd maneuver input?
	if (T2 < 0.0)
	{
		//Find T2
		T2 = T2Search();
	}
	//Calculate maximum time
	if (opt.RequestIndicator == 1)
	{
		TMAX = T2 + opt.TimeRange;
	}
	else if (opt.RequestIndicator == 2)
	{
		TMAX = T1 + opt.TimeRange;
	}
	else
	{
		TMAX = 0.0;
	}
	do
	{
		//Advance to T1
		if (coast(sv_C1, T1 - sv_C1.sv.GMT, sv_C1))
		{
			//Error: Reentered
			pRTCC->PMXSPT("PMSTICN", 82);
		}
		else
		{
			if (coast(sv_T1, T1 - sv_T1.sv.GMT, sv_T1))
			{
				//Error: Reentered
				pRTCC->PMXSPT("PMSTICN", 82);
			}
			else
			{
				//Compute two impulse solution
				if (soln == 0)
				{
					//Save state vector
					pRTCC->PZMYSAVE.SV_mult[0] = sv_C1;
					pRTCC->PZMYSAVE.SV_mult[1] = sv_T1;
				}
				err = PMMTIS(sv_C1, sv_T1, T2 - T1, DH, PhaseAngle, sv_C1_apo, sv_C2, sv_C2_apo);
				//Was a solution available?
				if (err == 0)
				{
					//Store DVs and times for data table
					entry[soln].DELV1 = length(sv_C1_apo.sv.V - sv_C1.sv.V);
					entry[soln].DELV2 = length(sv_C2_apo.sv.V - sv_C2.sv.V);
					entry[soln].Time1 = sv_C1.sv.GMT;
					entry[soln].Time2 = sv_C2.sv.GMT;
					PMSTICN_PY(sv_C1_apo.sv.R, sv_C1_apo.sv.V, sv_C1.sv.R, sv_C1.sv.V, entry[soln].PITCH1, entry[soln].YAW1, DV_LVLH);
					PMSTICN_PY(sv_C2_apo.sv.R, sv_C2_apo.sv.V, sv_C2.sv.R, sv_C2.sv.V, entry[soln].PITCH2, entry[soln].YAW2, DV_LVLH);
					DVT = length(sv_C2_apo.sv.V - sv_C2.sv.V) + length(sv_C1_apo.sv.V - sv_C1.sv.V);

					//Is this the 1st solution computed?
					if (soln == 0)
					{
						//Compute next environment change following frozen maneuver of first plan
						VehicleDataBlock sv_temp = opt.RequestIndicator == 2 ? sv_C2_apo : sv_C1_apo;
						pRTCC->PMMDAN(sv_temp, 1, PMMDAN_ERR, T_c, T_c_apo);
						if (PMMDAN_ERR)
						{
							DT_Light = 0.0;
						}
						else
						{
							DT_Light = abs(T_c) - sv_temp.sv.GMT;
							if (T_c < 0.0)
							{
								DT_Light = -DT_Light;
							}
						}
					}
					//Compute next environment change following variable maneuver of this plan
					pRTCC->PMMDAN(opt.RequestIndicator == 2 ? sv_C1_apo : sv_C2_apo, 1, PMMDAN_ERR, T_c, T_c_apo);
					if (PMMDAN_ERR)
					{
						//With error just set it to false
						entry[soln].L = false;
					}
					else
					{
						if (T_c < 0.0)
						{
							//Upcoming darkness, so currently light
							entry[soln].L = true;
						}
						else
						{
							//Upcoming daylight, so currently dark
							entry[soln].L = false;
						}
					}

					//Compute TPI time under special conditions
					entry[soln].T_TPI = 0.0;
					if (pRTCC->PZTIPREG.showTPI)
					{
						VehicleDataBlock sv_C_temp, sv_T_temp;
						double T3;

						if (ElevationAngleSearch(sv_C2_apo, sv_T1, Elev, sv_C_temp, sv_T_temp, T3) == 0)
						{
							entry[soln].T_TPI = T3;
						}
					}
					soln++;
				}
			}
		}
		//Determine next T1 and T2
		if (opt.RequestIndicator < 1)
		{
			//Only one solution requested
			break;
		}
		else if (opt.RequestIndicator == 1)
		{
			//Increment time of 2nd maneuver
			T2 += opt.TimeStep;
		}
		else
		{
			//Increment time of 1st maneuver
			T1 += opt.TimeStep;
			//Is 1st maneuver time past 2nd maneuver time?
			if (T1 >= T2)
			{
				//Yes
				break;
			}
		}
		//Has the number of requested cases been computed?
		if ((opt.RequestIndicator == 1 && T2 > TMAX) || (opt.RequestIndicator == 2 && T1 > TMAX))
		{
			break;
		}
		//Was a solution available for the last case?
		if (err == 0)
		{
			//Yes
			//Did the last solution have the lowest fuel expenditure so far?
			if (DVT < DV_opt)
			{
				OPCASE = soln;
				DV_opt = DVT;
			}
			//Have 13 solutions been computed?
			if (soln >= 13)
			{
				//Is the best solution one of the first seven?
				if (OPCASE > 7)
				{
					//Set number of solutions to 12 and delete the 1st solution of present 13 available
					for (int i = 0; i < 12; i++)
					{
						entry[i] = entry[i + 1];
					}
					OPCASE--;
					soln = 12;
				}
			}
		}
	} while (soln < 13);
	//Write output
	for (int i = 0; i < soln; i++)
	{
		pRTCC->PZTIPREG.data[i] = entry[i];
	}
	pRTCC->PZTIPREG.Solutions = soln;
	pRTCC->PZTIPREG.IVFLAG = opt.RequestIndicator;
	pRTCC->PZTIPREG.MAN_VEH = opt.ChaserVehicle;
	pRTCC->PZTIPREG.DT_Light = DT_Light;
	pRTCC->PZTIPREG.Updating = false;
	if (opt.ChaserVehicle == RTCC_MPT_CSM)
	{
		pRTCC->PZTIPREG.CSMSTAID = opt.ChaserStationID;
		pRTCC->PZTIPREG.LMSTAID = opt.TargetStationID;
		pRTCC->PZTIPREG.CSM_GMTTH = opt.ChaserVectorTime;
		pRTCC->PZTIPREG.LM_GMTTH = opt.TargetVectorTime;
	}
	else
	{
		pRTCC->PZTIPREG.CSMSTAID = opt.TargetStationID;
		pRTCC->PZTIPREG.LMSTAID = opt.ChaserStationID;
		pRTCC->PZTIPREG.CSM_GMTTH = opt.TargetVectorTime;
		pRTCC->PZTIPREG.LM_GMTTH = opt.ChaserVectorTime;
	}
	pRTCC->EMSNAP(0, 63);
}

void TwoImpulseProcessor::SingleSolutionTransferPlan()
{
	int err;
	bool found;

	//Read desired block of desired table
	found = false;
	if (opt.TwoImpulseTableIndicator == 1)
	{
		//Multiple
		if (opt.PlanNumber <= pRTCC->PZTIPREG.Solutions)
		{
			found = true;
		}
	}
	else
	{
		//Corrective Combination
		if (opt.PlanNumber <= pRTCC->PZTIPCCD.Solutions)
		{
			found = true;
		}
	}
	//Is requested solution available?
	if (found == false)
	{
		//No
		pRTCC->PMXSPT("PMSTICN", 29);
		return;
	}

	StationIDArr LMSTAID, CSMSTAID;
	double LM_GMTTH, CSM_GMTTH;
	int MAN_VEH;

	//Get saved block for requested solution
	if (opt.TwoImpulseTableIndicator == 1)
	{
		sv_C1 = pRTCC->PZMYSAVE.SV_mult[0];
		sv_T1 = pRTCC->PZMYSAVE.SV_mult[1];
		//Set up STAIDs, maneuver and threshold times and desired offsets for re-computation of desired solution
		LMSTAID = pRTCC->PZTIPREG.LMSTAID;
		CSMSTAID = pRTCC->PZTIPREG.CSMSTAID;
		LM_GMTTH = pRTCC->PZTIPREG.LM_GMTTH;
		CSM_GMTTH = pRTCC->PZTIPREG.CSM_GMTTH;
		MAN_VEH = pRTCC->PZTIPREG.MAN_VEH;
		T1 = pRTCC->PZTIPREG.data[opt.PlanNumber - 1].Time1;
		T2 = pRTCC->PZTIPREG.data[opt.PlanNumber - 1].Time2;
		DH = pRTCC->GZGENCSN.TIDeltaH;
		PhaseAngle = pRTCC->GZGENCSN.TIPhaseAngle;
	}
	else
	{
		sv_C1 = pRTCC->PZMYSAVE.SV_CC[0];
		sv_T1 = pRTCC->PZMYSAVE.SV_CC[1];
		//Set up STAIDs, maneuver and threshold times and desired offsets for re-computation of corrective combination desired solution
		//T1 = pRTCC->PZTIPCCD.data[opt.PlanNumber - 1].Time1;
		//T2 = pRTCC->PZTIPCCD.data[opt.PlanNumber - 1].Time2;
		DH = pRTCC->GZGENCSN.TINSRNominalDeltaH;
		PhaseAngle = pRTCC->GZGENCSN.TINSRNominalPhaseAngle;
	}

	//Advance to T1
	if (coast(sv_C1, T1 - sv_C1.sv.GMT, sv_C1))
	{
		//Error: Reentered
		pRTCC->PMXSPT("PMSTICN", 82);
		return;
	}
	if (coast(sv_T1, T1 - sv_T1.sv.GMT, sv_T1))
	{
		//Error: Reentered
		pRTCC->PMXSPT("PMSTICN", 82);
		return;
	}
	//Calculate solution
	err = PMMTIS(sv_C1, sv_T1, T2 - T1, DH, PhaseAngle, sv_C1_apo, sv_C2, sv_C2_apo);
	if (err)
	{
		return;
	}
	if (opt.mode == 3)
	{
		VECTOR3 DV_LVLH1, DV_LVLH2;
		double T_c1, T_c2, T_c_apo, PITCH1, YAW1, PITCH2, YAW2;
		int PMMDAN_ERR;

		//Compute impulsive pitch, yaw and VX, VY, VZ components for 1st maneuver
		PMSTICN_PY(sv_C1_apo.sv.R, sv_C1_apo.sv.V, sv_C1.sv.R, sv_C1.sv.V, PITCH1, YAW1, DV_LVLH1);
		//Compute impulsive pitch, yaw and VX, VY, VZ components for 2nd maneuver
		PMSTICN_PY(sv_C2_apo.sv.R, sv_C2_apo.sv.V, sv_C2.sv.R, sv_C2.sv.V, PITCH2, YAW2, DV_LVLH2);
		//Compute next environment change following each maneuver
		pRTCC->PMMDAN(sv_C1_apo, 1, PMMDAN_ERR, T_c1, T_c_apo);
		pRTCC->PMMDAN(sv_C2_apo, 1, PMMDAN_ERR, T_c2, T_c_apo);
		//Put data in table
		pRTCC->PZTIPSS.LMSTAID = LMSTAID;
		pRTCC->PZTIPSS.CSMSTAID = CSMSTAID;
		pRTCC->PZTIPSS.LM_GMTTH = LM_GMTTH;
		pRTCC->PZTIPSS.CSM_GMTTH = CSM_GMTTH;
		pRTCC->PZTIPSS.MAN_VEH = MAN_VEH;
		pRTCC->PZTIPSS.PointingMode = opt.LOSMode;
		pRTCC->PZTIPSS.PlanNumber = opt.PlanNumber;
		pRTCC->PZTIPSS.TwoImpulseTableIndicator = opt.TwoImpulseTableIndicator;
		pRTCC->PZTIPSS.ActualWT = opt.UllageQuads ? 1.0 : 0.0;
		pRTCC->PZTIPSS.DeltaPitch = opt.DeltaPitch;
		pRTCC->PZTIPSS.man[0].TIG = opt.RelMoTimeStep;
		pRTCC->PZTIPSS.man[0].DV_LVLH = DV_LVLH1;
		pRTCC->PZTIPSS.man[1].DV_LVLH = DV_LVLH2;
		pRTCC->PZTIPSS.man[0].MinEnvironChange = T_c1 / 60.0;
		pRTCC->PZTIPSS.man[1].MinEnvironChange = T_c2 / 60.0;
		//Single Solution computation
		SingleSolution(pRTCC->PZTIPSS);
		//Set up display queue for single solution
		pRTCC->EMSNAP(0, 65);
	}
	else
	{
		TransferPlan();
	}
}

void LOS_PITCH_YAW(VECTOR3 R_C, VECTOR3 V_C, VECTOR3 R_T, double &pitch, double &yaw)
{
	VECTOR3 R4 = mul(OrbMech::LVLH_Matrix(R_C, V_C), R_T - R_C);
	//Change to different LVLH definition
	R4 = _V(R4.x, R4.z, -R4.y);
	pitch = atan(-R4.y / sqrt(R4.x*R4.x + R4.z*R4.z));
	yaw = atan2(-R4.z, R4.x);
}

void TwoImpulseProcessor::SingleSolution(TwoImpulseSingleSolutionTable &tab)
{
	//Load module PMMTISS

	VECTOR3 DV[2], R_C[2], V_C[2], R_T[2], V_T[2];
	double RelMoTimeStep, l_dot_T, g_dot_T, h_dot_T, eps, R_E, R[2], WT_BEF[2], WT_AFT[2], T, isp, WDOT;
	int UllageQuads;

	if (sv_C1.sv.RBI == BODY_EARTH)
	{
		R_E = OrbMech::R_Earth;
	}
	else
	{
		R_E = pRTCC->BZLAND.rad[0];
	}
	R_C[0] = sv_C1.sv.R; R_C[1] = sv_C2.sv.R;
	V_C[0] = sv_C1.sv.V; V_C[1] = sv_C2.sv.V;
	R_T[0] = sv_T1.sv.R; R_T[1] = sv_T2.sv.R;
	V_T[0] = sv_T1.sv.V; V_T[1] = sv_T2.sv.V;
	R[0] = length(sv_C1.sv.R); R[1] = length(sv_C2.sv.R);
	WT_BEF[0] = sv_C1.Weight; WT_BEF[1] = sv_C2.Weight;
	WT_AFT[0] = sv_C1_apo.Weight; WT_AFT[1] = sv_C2_apo.Weight;

	if (coast(sv_T1, T2 - sv_C1.sv.GMT, sv_T2))
	{
		//Error: Reentered
		pRTCC->PMXSPT("PMSTICN", 82);
		return;
	}
	//Recover temporarily stored parameters
	RelMoTimeStep = tab.man[0].TIG;
	UllageQuads = tab.ActualWT != 0.0 ? 2 : 1;
	SecularRates(sv_T1.sv, l_dot_T, g_dot_T, h_dot_T);

	//Store times of both maneuvers in single solution table
	tab.man[0].TIG = sv_C1.sv.GMT;
	tab.man[1].TIG = sv_C2.sv.GMT;
	tab.ActualWT = (tab.man[1].TIG - tab.man[0].TIG)*(l_dot_T + g_dot_T);

	//Convert external DV coordinates to components necessary for maneuver execution function
	for (int i = 0; i < 2; i++)
	{
		DV[i] = _V(tab.man[i].DV_LVLH.x, tab.man[i].DV_LVLH.z, -tab.man[i].DV_LVLH.y);
	}
	
	//Is CSM the chaser vehicle?
	if (tab.MAN_VEH == 1)
	{
		eps = 7.25*RAD;
		T = pRTCC->SystemParameters.MCTCT1;
		isp = T / pRTCC->SystemParameters.MCTCW1;
	}
	else
	{
		eps = 0.0;
		T = pRTCC->SystemParameters.MCTLT1;
		isp = T / pRTCC->SystemParameters.MCTLW1;
	}
	WDOT = T * ((double)UllageQuads) / isp;

	VehicleDataBlock sv_C_temp, sv_T_temp;
	VECTOR3 DR, U_R, R4;
	double phi, psi, V_B0, V_B1, V_B2, X_dot, Y_dot, Z_dot, X_BR, Y_BR, Z_BR, a_LAT, DELTA, DT_B0, DT_B1, DT_B2, T_APP, RC, RT, DPHI;
	TwoImpulseSingleSolutionTableApproachData *app;

	for (int i = 0; i < 2; i++)
	{
		//Pointer to approach data table
		app = i == 0 ? tab.app1 : tab.app2;
		//Store DV
		tab.man[i].DV = length(DV[i]);
		//Re-compute impulsive pitch and yaw for each maneuver of the requested solution
		tab.man[i].Pitch = atan(-DV[i].y / sqrt(DV[i].x*DV[i].x + DV[i].z*DV[i].z));
		tab.man[i].Yaw = atan2(-DV[i].z, DV[i].x);
		//Compute pitch and yaw angles to LOS option (target or horizon)
		tab.man[i].E_HOR = asin(R_E / R[i]) - PI05;
		if (tab.PointingMode == 1)
		{
			//Target
			//Compute relative coordinates of first vehicle in second vehicle's coordinate system
			LOS_PITCH_YAW(R_C[i], V_C[i], R_T[i], tab.man[i].Pitch_LOS, tab.man[i].Yaw_LOS);
		}
		else
		{
			//Horizon
			tab.man[i].Pitch_LOS = tab.man[i].E_HOR;
			tab.man[i].Yaw_LOS = 0.0;
		}
		//Compute DV increments in body coordinates with vehicle pitched to view either the target of the horizon. Also compute burn times of each thruster for first maneuver
		phi = tab.man[i].Pitch_LOS;
		psi = tab.man[i].Yaw_LOS;
		V_B0 = DV[i].x*cos(psi)*cos(phi) - DV[i].z*sin(psi)*cos(phi) - DV[i].y*sin(phi);
		V_B1 = -DV[i].x*sin(psi) - DV[i].z*cos(psi);
		V_B2 = DV[i].x*cos(psi)*sin(phi) - DV[i].z*sin(psi)*sin(phi) + DV[i].y*cos(phi);
		//Compute DV increments in body coordinates with vehicle pitch to view either target or horizon
		X_dot = V_B0 * cos(tab.DeltaPitch) - V_B2 * sin(tab.DeltaPitch);
		Y_dot = V_B1;
		Z_dot = V_B2 * cos(tab.DeltaPitch) - V_B0 * sin(tab.DeltaPitch);
		//Add MED pitch angle for astronaut to view target or horizon to computed LOS pitch
		tab.man[i].Pitch_LOS += tab.DeltaPitch;
		//DV increments in body coordinates rotated through offset of thruster
		X_BR = X_dot;
		Y_BR = Y_dot * cos(eps) - Z_dot * sin(eps);
		Z_BR = Z_dot * cos(eps) + Y_dot * sin(eps);
		//Compute acceleration of lateral thrusters
		a_LAT = 2.0*T / WT_BEF[i];
		//Compute burn times with lateral thrusters
		DT_B1 = abs(Y_BR) / a_LAT;
		DT_B2 = abs(Z_BR) / a_LAT;
		//Compute burn times with forward thrusters
		DELTA = WT_BEF[i] / exp(abs(X_BR) / isp);
		DT_B0 = (WT_BEF[i] - DELTA) / WDOT;
		//Store
		tab.man[i].DV_LOS = _V(X_BR, Y_BR, Z_BR);
		tab.man[i].BT_LOS = _V(DT_B0, DT_B1, DT_B2);
		//Three sets of data for the first maneuver, four sets for the second
		for (int j = 0; j < (i == 0 ? 3 : 4); j++)
		{
			if (j == 0)
			{
				//First approach time is the time of the maneuver
				T_APP = tab.man[i].TIG;
				sv_C_temp = i == 0 ? sv_C1 : sv_C2;
				sv_T_temp = i == 0 ? sv_T1 : sv_T2;
			}
			else
			{
				//Advance to next approach time
				T_APP -= RelMoTimeStep;
				coast(sv_C_temp, T_APP - sv_C_temp.sv.GMT, sv_C_temp);
				coast(sv_T_temp, T_APP - sv_T_temp.sv.GMT, sv_T_temp);
			}

			//Range
			DR = sv_T_temp.sv.R - sv_C_temp.sv.R;
			U_R = unit(DR);
			app[j].Range = length(DR);
			//Range rate
			app[j].RangeRate = dotp(sv_T_temp.sv.V - sv_C_temp.sv.V, U_R);
			//Pitch and yaw
			LOS_PITCH_YAW(sv_C_temp.sv.R, sv_C_temp.sv.V, sv_T_temp.sv.R, app[j].Elev, app[j].Azi);
			//Offset
			R4 = mul(OrbMech::LVLH_Matrix(sv_T_temp.sv.R, sv_T_temp.sv.V), sv_C_temp.sv.R - sv_T_temp.sv.R);
			//Compute phase angle between chaser and target in target vehicle plane - DPHI is positive if chaser is leading
			RC = length(sv_C_temp.sv.R);
			RT = length(sv_T_temp.sv.R);
			DPHI = atan2(R4.x, RT - R4.y);
			//Compute curvilinear coordinates of active (chaser) from passive (target)
			app[j].DX = _V(RT * DPHI, RT - RC, -R4.z);
		}
	}
}

void TwoImpulseProcessor::TransferPlan()
{

}

void TwoImpulseProcessor::ExternalRequest(TwoImpulseResuls &res)
{
	VehicleDataBlock sv_C1_apo, sv_C2, sv_C2_apo;
	int err;

	T1 = opt.T1;
	T2 = opt.T2;
	DH = opt.DH;
	PhaseAngle = opt.PhaseAngle;
	Elev = opt.Elev;
	WT = opt.WT;

	sv_C1 = opt.sv_C;
	sv_T1 = opt.sv_T;

	//Was time of 1st maneuver input?
	if (T1 < 0.0)
	{
		//Use elevation angle search routine
		if (ElevationAngleSearch(sv_C1, sv_T1, Elev, sv_C1, sv_T1, T1))
		{
			//Error
			res.SolutionFound = false;
			return;
		}
	}
	//Was time of 2nd maneuver input?
	if (T2 < 0.0)
	{
		//Use target terminal phase transfer angle to compute time
		T2 = T2Search();
	}
	//Advance to T1
	if (coast(sv_C1, T1 - sv_C1.sv.GMT, sv_C1))
	{
		res.SolutionFound = false;
		return;
	}
	if (coast(sv_T1, T1 - sv_T1.sv.GMT, sv_T1))
	{
		res.SolutionFound = false;
		return;
	}
	//Compute two impulse solution
	err = PMMTIS(sv_C1, sv_T1, T2 - T1, DH, PhaseAngle, sv_C1_apo, sv_C2, sv_C2_apo);
	if (err)
	{
		//DKI error return
		res.SolutionFound = false;
	}
	else
	{
		//Pass solution back
		res.sv_tig = sv_C1.sv;
		res.sv_tig_apo = sv_C1_apo.sv;
		res.sv_tig2 = sv_C2.sv;
		res.sv_tig2_apo = sv_C2_apo.sv;
		res.dV = sv_C1_apo.sv.V - sv_C1.sv.V;
		res.dV2 = sv_C2_apo.sv.V - sv_C2.sv.V;
		res.dV_LVLH = mul(OrbMech::LVLH_Matrix(sv_C1.sv.R, sv_C1.sv.V), res.dV);
		res.dV_LVLH2 = mul(OrbMech::LVLH_Matrix(sv_C2.sv.R, sv_C2.sv.V), res.dV2);
		res.T1 = pRTCC->GETfromGMT(T1);
		res.T2 = pRTCC->GETfromGMT(T2);
		res.SolutionFound = true;
	}
}

int TwoImpulseProcessor::PMMTIS(VehicleDataBlock sv_A1, VehicleDataBlock sv_P1, double dt, double DH, double theta, VehicleDataBlock &sv_A1_apo, VehicleDataBlock &sv_A2, VehicleDataBlock &sv_A2_apo) const
{
	//INPUTS:
	//sv_A1: Chaser at T1
	//sv_P1: Target at T1

	VehicleDataBlock sv_P2;
	CELEMENTS elem_C, elem_CE, elem_T;
	VECTOR3 RP2off, VP2off, R3, VA1_apo, DR;
	double mu, RPLIM, l_dot_C, g_dot_C, h_dot_C, l_dot_T, g_dot_T, h_dot_T, theta_0, df_3, WT_3, f_T, f_CE, u_T, u_CE, R_CE, R_T, dv, isp;
	int IC, N;
	bool prograde;

	if (sv_A1.sv.RBI == BODY_EARTH)
	{
		prograde = true;
		mu = OrbMech::mu_Earth;
		RPLIM = OrbMech::R_Earth + 10.0*1852.0;
	}
	else
	{
		prograde = false;
		mu = OrbMech::mu_Moon;
		//Don't enforce periapsis limit for external request, because the calling function might iterate on T1/T2
		if (opt.mode == 5)
		{
			RPLIM = 0.0;
		}
		else
		{
			RPLIM = pRTCC->BZLAND.rad[RTCC_LMPOS_BEST];
		}
	}
	//Get initial orbital elements and secular rates
	elem_C = OrbMech::GIMIKC(sv_A1.sv.R, sv_A1.sv.V, mu);
	SecularRates(sv_A1.sv, l_dot_C, g_dot_C, h_dot_C);
	elem_T = OrbMech::GIMIKC(sv_P1.sv.R, sv_P1.sv.V, mu);
	SecularRates(sv_P1.sv, l_dot_T, g_dot_T, h_dot_T);
	theta_0 = OrbMech::PHSANG(sv_P1.sv.R, sv_P1.sv.V, sv_A1.sv.R);

	//Rough estimate of specific impulse for weight update
	isp = 3000.0;
	//Initialize iteration counter
	IC = 0;
	//Estimate travel angle from T1 to T2
	df_3 = dt * (l_dot_T + g_dot_T);
	WT_3 = df_3 + theta_0 - theta;

	//Calculate number of revs
	N = (int)(WT_3 / PI2);

	//Take target to T2
	if (coast(sv_P1, dt, sv_P2))
	{
		//Error
		return 1;
	}
	//Target elements at T2
	elem_T = OrbMech::GIMIKC(sv_P2.sv.R, sv_P2.sv.V, mu);
	f_T = OrbMech::MeanToTrueAnomaly(elem_T.l, elem_T.e);
	u_T = elem_T.g + f_T;
	OrbMech::normalizeAngle(u_T);
	R_T = length(sv_P2.sv.R);

	//Calculate offset position
	if (theta != 0 || DH != 0)
	{
		elem_CE.a = elem_T.a - DH;
		elem_CE.e = elem_T.e*elem_T.a / elem_CE.a;
		f_CE = f_T - theta;
		if (f_CE >= PI2)
		{
			f_CE -= PI2;
		}
		if (f_CE < 0)
		{
			f_CE += PI2;
		}
		u_CE = u_T - theta;
		if (u_CE >= PI2)
		{
			u_CE -= PI2;
		}
		if (u_CE < 0)
		{
			u_CE += PI2;
		}
		R_CE = elem_CE.a*(1.0 - elem_CE.e*elem_CE.e) / (1.0 + elem_CE.e*cos(f_CE));
		elem_CE.l = OrbMech::TrueToMeanAnomaly(f_CE, elem_CE.e);
	}
	else
	{
		elem_CE.a = elem_T.a;
		elem_CE.e = elem_T.e;
		f_CE = f_T;
		u_CE = u_T;
		R_CE = R_T;
		elem_CE.l = elem_T.l;
	}

	elem_CE.i = elem_T.i;
	elem_CE.g = elem_T.g;
	elem_CE.h = elem_T.h;
	//Calculate offset state
	OrbMech::GIMKIC(elem_CE, mu, RP2off, VP2off);
	//State after NCC
	sv_A1_apo = sv_A1;
	//Target position
	R3 = RP2off;
	do
	{
		//Out of iterations?
		if (IC > 15)
		{
			return 1;
		}
		//Calculate Lambert solution
		VA1_apo = OrbMech::elegant_lambert(sv_A1.sv.R, sv_A1.sv.V, R3, dt, N, prograde, mu);
		//No solution?
		if (length(VA1_apo) == 0.0)
		{
			return 1;
		}
		//Assign velocity to post maneuver state
		sv_A1_apo.sv.V = VA1_apo;
		//Check if periapsis constraint is violated
		elem_C = OrbMech::GIMIKC(sv_A1_apo.sv.R, sv_A1_apo.sv.V, mu);
		if (elem_C.a*(1.0 - elem_C.e) < RPLIM)
		{
			return 1;
		}
		//Update post maneuver weight
		dv = length(sv_A1_apo.sv.V - sv_A1.sv.V);
		sv_A1_apo.Weight = sv_A1.Weight * exp(-dv / isp);
		//Take chaser to T2
		if (coast(sv_A1_apo, dt, sv_A2))
		{
			//Error
			return 1;
		}
		//Calculate position error
		DR = sv_A2.sv.R - RP2off;
		//Calculate new fake target
		R3 = R3 - DR;
		IC++;
	} while (length(DR) > 100.0*0.3048);

	//Output data
	sv_A2_apo = sv_A2;
	sv_A2_apo.sv.V = VP2off;
	//Update post maneuver weight
	dv = length(sv_A2_apo.sv.V - sv_A2.sv.V);
	sv_A2_apo.Weight = sv_A2.Weight * exp(-dv / isp);
	return 0;
}

bool TwoImpulseProcessor::ElevationAngleSearch(VehicleDataBlock sv_A0, VehicleDataBlock sv_P0, double Elev, VehicleDataBlock &sv_A1, VehicleDataBlock &sv_P1, double &T1) const
{
	//INPUTS:
	//sv_A0: Chaser state vector
	//sv_P0: Target state vector
	//Elev: Desired elevation angle
	//OUTPUTS:
	//sv_A1: Chaser state vector at T1
	//sv_P1: Target state vector at T1
	//T1: Time of elevation angle
	VECTOR3 I_LOS, I, I_H;
	double e_LN, eps_elev, dt_max, r_C, r_T, C, e, e0, p, C2, T, eps_dh;
	int C1, s_FAIL;

	//Elevation angle tolerance (0.057296 deg)
	eps_elev = 0.001;
	//Maximum integration time (900 seconds)
	dt_max = 900.0;
	eps_dh = 50.0;
	C = e = e0 = p = 0.0;
	C1 = s_FAIL = 0;

	//Initial time of chaser as first iteration time
	T1 = sv_A0.sv.GMT;
	sv_A1 = sv_A0;
	sv_P1 = sv_P0;

	do
	{
		//Update both vehicles to T1
		if (coast(sv_A1, T1 - sv_A1.sv.GMT, sv_A1)) return true;
		if (coast(sv_P1, T1 - sv_P1.sv.GMT, sv_P1)) return true;
		//Calculate current elevation angle
		I_LOS = unit(sv_P1.sv.R - sv_A1.sv.R);
		r_C = length(sv_A1.sv.R);
		r_T = length(sv_P1.sv.R);
		I = unit(I_LOS - sv_A1.sv.R*dotp(I_LOS, sv_A1.sv.R) / (r_C*r_C));
		I_H = unit(crossp(crossp(sv_A1.sv.R, sv_A1.sv.V), sv_A1.sv.R));
		e_LN = acos(dotp(I_LOS, I*OrbMech::sign(dotp(I, I_H))));
		if (dotp(I_LOS, sv_A1.sv.R) < 0.0)
		{
			e_LN = PI2 - e_LN;
		}
		if (C1 == 0)
		{
			if ((Elev - PI)*(r_C - r_T) < 0.0)
			{
				e = r_T - r_C + OrbMech::sign(r_T - r_C)*eps_dh;
			}
			else
			{
				C1 = 1;
				if (C != 0.0)
				{
					C = 0.0;
					C2 = OrbMech::sign(T1 - T);
					T = T1;
					T1 = T1 + 10.0*C2;
					continue;
				}
				else
				{
					e = Elev - e_LN;
					if (abs(e) <= eps_elev)
					{
						//Converged
						break;
					}
				}
			}
		}
		else
		{
			e = Elev - e_LN;
			if (abs(e) <= eps_elev)
			{
				//Converged
				break;
			}
		}
		//Iterate on time
		OrbMech::ITER(C, s_FAIL, e, p, T1, e0, T, -100.0);
		//Out of iterations?
		if (s_FAIL) return true;
		//Updated time exceeds dt_max?
		if (abs(T - T1) > dt_max)
		{
			T1 = T + dt_max * OrbMech::sign(T1 - T);
		}
	} while (s_FAIL == false);
	return false;
}

double TwoImpulseProcessor::T2Search()
{
	//Use sv_T1, assumes it is at T1
	double l_dot, g_dot, h_dot;

	SecularRates(sv_T1.sv, l_dot, g_dot, h_dot);
	return T1 + WT / (l_dot + g_dot);
}

void TwoImpulseProcessor::PMSTICN_PY(VECTOR3 R_A, VECTOR3 V_A, VECTOR3 R_B, VECTOR3 V_B, double &Pitch, double &Yaw, VECTOR3 &DV_LVLH) const
{
	VECTOR3 H_A, H_B;
	double r_B, sin_delta, r_A_dot, r_B_dot, dr_dot, DV, h_A, h_B, VH_A, VH_B, DV_H;

	H_A = crossp(R_A, V_A);
	h_A = length(H_A);
	H_A = unit(H_A);
	H_B = crossp(R_B, V_B);
	h_B = length(H_B);
	H_B = unit(H_B);
	r_B = length(R_B);
	sin_delta = dotp(crossp(H_A, H_B), R_B) / r_B;
	r_B_dot = dotp(R_B, V_B) / r_B;
	r_A_dot = dotp(R_A, V_A) / r_B;
	dr_dot = r_A_dot - r_B_dot;
	DV = length(V_B - V_A);
	Pitch = asin(dr_dot / DV);
	VH_B = h_B / r_B;
	VH_A = h_A / r_B;
	DV_H = DV * cos(Pitch);
	Yaw = asin(VH_A*sin_delta / DV_H);
	if (VH_A*sqrt(1.0 - sin_delta * sin_delta) < VH_B)
	{
		Yaw = PI - Yaw;
		if (Yaw > PI)
		{
			Yaw -= PI2;
		}
	}
	DV_LVLH.z = -dr_dot;
	DV_LVLH.x = DV_H * cos(Yaw);
	DV_LVLH.y = DV_H * sin(Yaw);
}

bool TwoImpulseProcessor::coast(VehicleDataBlock sv0, double dt, VehicleDataBlock &sv1) const
{
	sv1 = sv0;
	sv1.sv = pRTCC->coast(sv0.sv, dt, sv0.Weight, sv0.Area, sv0.KFactor, false);
	//Error condition for reentering the atmosphere
	if (sv1.sv.RBI == BODY_EARTH)
	{
		if (sv1.KFactor > 0.0)
		{
			if (length(sv1.sv.R) < OrbMech::R_Earth + 50.0*1852.0)
			{
				return true;
			}
		}
	}
	return false;
}

void TwoImpulseProcessor::SecularRates(EphemerisData sv0, double &l_dot, double &g_dot, double &h_dot) const
{
	EphemerisData sv_true;
	double mu;

	int err = pRTCC->ELVCNV(sv0, sv0.RBI == BODY_EARTH ? 1 : 3, sv_true);
	if (err) sv_true = sv0;

	mu = sv0.RBI == BODY_EARTH ? OrbMech::mu_Earth : OrbMech::mu_Moon;

	CELEMENTS coe = OrbMech::GIMIKC(sv_true.R, sv_true.V, mu);
	OrbMech::BrouwerSecularRates(coe, coe, sv0.RBI, l_dot, g_dot, h_dot);
}