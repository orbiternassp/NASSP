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

	DH_min = DH_max = DH_inc = T2_min = T2_max = 0.0;
	dt_TPI_slip = 0.0;

	TimeStep = 10.0;
	TimeRange = 0.0;

	DH = 0.0;
	PhaseAngle = 0.0;
	WT = 0.0;
	Elev = 0.0;

	TwoImpulseTableIndicator = 1;
	PlanNumber = 1;
	UllageQuads = true;
	LOSMode = 1;
	DeltaPitch = 0.0;
}

TwoImpulseResuls::TwoImpulseResuls()
{
	dV = dV2 = dV_LVLH = dV_LVLH2 = _V(0, 0, 0);
	T1 = T2 = 0.0;
	SolutionFound = false;
}

TwoImpulseSingleSolutionTable::TwoImpulseSingleSolutionTable()
{
	LM_GMTTH = 0.0;
	CSM_GMTTH = 0.0;
	MAN_VEH = 1;
	PointingMode = 1;
	PlanNumber = 0;
	TwoImpulseTableIndicator = 1;
	ActualPhase = 0.0;
	ActualDH = 0.0;
	ActualWT = 0.0;
	DeltaPitch = 0.0;
}

TwoImpulseProcessor::TwoImpulseProcessor(RTCC *r) : RTCCModule(r)
{
	T1 = T2 = 0.0;
}

void TwoImpulseProcessor::PMSTICN(const TwoImpulseOpt &opt, TwoImpulseResuls &res)
{
	//Initialization
	this->opt = opt;

	switch (opt.mode)
	{
	case 1: //Corrective Combination
	case 2: //Multiple Solution
		CCAndML();
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

void TwoImpulseProcessor::CCAndML()
{
	//Load chaser and target state vectors, area, weight, drag, STAID
	sv_C1 = opt.sv_C;
	sv_T1 = opt.sv_T;

	if (opt.mode == 1)
	{
		CorrectiveCombination();
	}
	else
	{
		MultipleSolution();
	}
}

void TwoImpulseProcessor::CorrectiveCombination()
{
	CorrectiveCombinationSolutionTable tab;
	double DV_opt, K, T_WSR, DH_WSR, theta_WSR, theta_T_min, HthetaR, du_dot, DV_TP, D[4], theta_max, theta_min, DVT, DH_OPTH, TIME_OPTH, theta_OPTH, T_SLIP;
	int err;
	bool coast_err;

	tab.Solutions = 0;
	if (opt.sv_C.sv.RBI == BODY_EARTH)
	{
		K = 5.9853114 / 3600.0 / OrbMech::R_Earth;
		//Curve fit for terminal phase (TPI and TPF)
		D[0] = 7.252527071926806e-03;
		D[1] = -5.702884489551252e-03;
		D[2] = 1.593340751920636e-03;
		D[3] = -1.441405564950315e-04;
	}
	else
	{
		K = 16.38140036 / 3600.0 / OrbMech::R_Earth;
		//Curve fit for terminal phase (TPI and TPF)
		D[0] = 5.515531567368726e-03;
		D[1] = -4.337031646358014e-03;
		D[2] = 1.211732286900968e-03;
		D[3] = -1.096185897107027e-04;
	}

	//Load MED request quantities
	T1 = opt.T1;
	//Set C.C. Table to updating condition
	pRTCC->PZTIPCCD.Updating = true;
	//DTREAD: GZGENCSN Blks. 12-15
	WT = pRTCC->GZGENCSN.TITravelAngle;
	T_WSR = pRTCC->GZGENCSN.TINSRNominalTime;
	DH_WSR = pRTCC->GZGENCSN.TINSRNominalDeltaH;
	theta_WSR = pRTCC->GZGENCSN.TINSRNominalPhaseAngle;
	//Build first block of corrective combination table: Station IDs, threshold times and time of 1st maneuver (NCC)
	tab.MAN_VEH = opt.ChaserVehicle;
	tab.T_NCC = T1;
	if (opt.ChaserVehicle == 1)
	{
		tab.LMSTAID = opt.TargetStationID;
		tab.CSMSTAID = opt.ChaserStationID;
		tab.LM_GMTTH = opt.TargetVectorTime;
		tab.CSM_GMTTH = opt.ChaserVectorTime;
	}
	else
	{
		tab.LMSTAID = opt.ChaserStationID;
		tab.CSMSTAID = opt.TargetStationID;
		tab.LM_GMTTH = opt.ChaserVectorTime;
		tab.CSM_GMTTH = opt.TargetVectorTime;
	}
	//Advance chaser and target to the time of the NCC maneuver
	coast_err = coast(sv_C1, T1 - sv_C1.sv.GMT, sv_C1);
	if (coast_err == false)
	{
		coast_err = coast(sv_T1, T1 - sv_T1.sv.GMT, sv_T1);
	}
	if (coast_err)
	{
		//Error: Reentered
		pRTCC->PMXSPT("PMSTICN", 82);
		//Write available solutions
		pRTCC->PZTIPCCD = tab;
		//Update display
		pRTCC->EMSNAP(0, 64);
		return;
	}
	//Save initial elements for regeneration
	pRTCC->PZMYSAVE.SV_CC[0] = sv_C1;
	pRTCC->PZMYSAVE.SV_CC[1] = sv_T1;
	//Initialize optimum DV to max, height offset and time of NSR maneuver to MED request minimum
	DV_opt = 10000000000.0;
	DH = opt.DH_min;
	T2 = opt.T2_min;
	theta_T_min = theta_WSR - (opt.T2_min - T_WSR)*K*DH_WSR;
	if (theta_T_min == 0.0 || DH_WSR == 0.0)
	{
		//Error
		pRTCC->PMXSPT("PMSTICN", 28);
		//Write available solutions
		pRTCC->PZTIPCCD = tab;
		//Update display
		pRTCC->EMSNAP(0, 64);
		return;
	}
	//Compute phase angle for 1st case
	HthetaR = DH_WSR / theta_T_min;
	PhaseAngle = opt.DH_min / HthetaR;
	//Outer, DH loop
	do
	{
		//Compute catchup rate at this height offset
		du_dot = DH * K;
		//Compute terminal phase approximation for this height
		DV_TP = abs((D[0] + WT * (D[1] + WT * (D[2] + WT * D[3]))) * DH);
		//Is this a slip time option request?
		if (opt.RequestIndicator == 1)
		{
			theta_max = PhaseAngle - opt.dt_TPI_slip * du_dot;
			theta_min = PhaseAngle + opt.dt_TPI_slip * du_dot;
			PhaseAngle = theta_min;
		}
		else
		{
			T2 = opt.T2_min;
		}
		//Inner, phase angle loop
		do
		{
			//Calculate solution
			err = PMMTIS(sv_C1, sv_T1, T2 - T1, DH, PhaseAngle, sv_C1_apo, sv_C2, sv_C2_apo);
			//Was a solution available?
			if (err == 0)
			{
				//Compute total dv for this solution
				DVT = length(sv_C1_apo.sv.V - sv_C1.sv.V) + length(sv_C2_apo.sv.V - sv_C2.sv.V) + DV_TP;
				//Is this the best solution for this DH?
				if (DVT < DV_opt)
				{
					//Save DV, DH, time and phase for this case
					DV_opt = DVT;
					DH_OPTH = DH;
					TIME_OPTH = T2;
					theta_OPTH = PhaseAngle;
				}
			}
			//Compute phase angle for next solution
			PhaseAngle = PhaseAngle - du_dot * opt.TimeStep;
			//Is this a slip time option request?
			if (opt.RequestIndicator == 0)
			{
				//No
				//Compute next NSR time at this height
				T2 = T2 + opt.TimeStep;
				//Is the new NSR time past the MED maximum?
				if (T2 >= opt.T2_max)
				{
					break;
				}
			}
			else
			{
				//Have all phase angles been tried at this height?
				if (PhaseAngle <= theta_max)
				{
					//Yes
					break;
				}
			}
		} while (true);
		//Was a solution obtained for this DH?
		if (DV_opt < 10000000.0)
		{
			//Yes
			//Is this a slip time option?
			if (opt.RequestIndicator == 1)
			{
				//Yes
				T_SLIP = (theta_min - opt.dt_TPI_slip * du_dot - theta_OPTH) / du_dot;
			}
			else
			{
				//No
				T_SLIP = 0.0;
			}
			//Store solution here?
			tab.data[tab.Solutions].GMT_NSR = TIME_OPTH;
			tab.data[tab.Solutions].DV_T = DV_opt;
			tab.data[tab.Solutions].DH = DH_OPTH;
			tab.data[tab.Solutions].PhaseAngle = theta_OPTH;
			tab.data[tab.Solutions].T_SLIP = T_SLIP;
			tab.Solutions++;
			//Reset optimum to maximum
			DV_opt = 10000000000.0;
			//Are there 13 solutions yet?
			if (tab.Solutions >= 13) break;
		}
		DH += opt.DH_inc;
		//Has last MED height request been attempted?
		if (DH > opt.DH_max)
		{
			//Yes
			break;
		}
		PhaseAngle = DH / HthetaR;
	} while (true);
	//Write available solutions
	pRTCC->PZTIPCCD = tab;
	//Update display
	pRTCC->EMSNAP(0, 64);
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
	pRTCC->PZTIPREG.DH = DH;
	pRTCC->PZTIPREG.PhaseAngle = PhaseAngle;
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

	std::string LMSTAID, CSMSTAID;
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
		DH = pRTCC->PZTIPREG.DH;
		PhaseAngle = pRTCC->PZTIPREG.PhaseAngle;
	}
	else
	{
		sv_C1 = pRTCC->PZMYSAVE.SV_CC[0];
		sv_T1 = pRTCC->PZMYSAVE.SV_CC[1];
		//Set up STAIDs, maneuver and threshold times and desired offsets for re-computation of corrective combination desired solution
		LMSTAID = pRTCC->PZTIPCCD.LMSTAID;
		CSMSTAID = pRTCC->PZTIPCCD.CSMSTAID;
		LM_GMTTH = pRTCC->PZTIPCCD.LM_GMTTH;
		CSM_GMTTH = pRTCC->PZTIPCCD.CSM_GMTTH;
		T1 = pRTCC->PZTIPCCD.T_NCC;
		T2 = pRTCC->PZTIPCCD.data[opt.PlanNumber - 1].GMT_NSR;
		DH = pRTCC->PZTIPCCD.data[opt.PlanNumber - 1].DH;
		PhaseAngle = pRTCC->PZTIPCCD.data[opt.PlanNumber - 1].PhaseAngle;
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
		pRTCC->PZTIPSS.man[0].TIG = opt.TimeStep;
		pRTCC->PZTIPSS.man[0].DV_LVLH = DV_LVLH1;
		pRTCC->PZTIPSS.man[1].DV_LVLH = DV_LVLH2;
		pRTCC->PZTIPSS.man[0].MinEnvironChange = T_c1;
		pRTCC->PZTIPSS.man[1].MinEnvironChange = T_c2;
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

	VehicleDataBlock sv_C[2], sv_C_apo[2], sv_T[2];
	VECTOR3 DV[2];
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

	//Take target to T2
	if (coast(sv_T1, T2 - sv_C1.sv.GMT, sv_T2))
	{
		//Error: Reentered
		pRTCC->PMXSPT("PMSTICN", 82);
		return;
	}
	//Save data in tabular form
	sv_C[0] = sv_C1; sv_C_apo[0] = sv_C1_apo;
	sv_C[1] = sv_C2; sv_C_apo[1] = sv_C2_apo;
	sv_T[0] = sv_T1; sv_T[1] = sv_T2;
	R[0] = length(sv_C1.sv.R); R[1] = length(sv_C2.sv.R);
	WT_BEF[0] = sv_C1.Weight; WT_BEF[1] = sv_C2.Weight;
	WT_AFT[0] = sv_C1_apo.Weight; WT_AFT[1] = sv_C2_apo.Weight;

	//Recover temporarily stored parameters
	RelMoTimeStep = tab.man[0].TIG;
	UllageQuads = tab.ActualWT != 0.0 ? 2 : 1;
	SecularRates(sv_T1.sv, l_dot_T, g_dot_T, h_dot_T);

	//Store times of both maneuvers in single solution table
	tab.man[0].TIG = sv_C1.sv.GMT;
	tab.man[1].TIG = sv_C2.sv.GMT;
	tab.ActualWT = (tab.man[1].TIG - tab.man[0].TIG)*(l_dot_T + g_dot_T);
	AEGBlock aeg_C = pRTCC->SVToAEG(sv_C2_apo.sv, sv_C2_apo.Area, sv_C2_apo.Weight, sv_C2_apo.KFactor);
	AEGBlock aeg_T = pRTCC->SVToAEG(sv_T2.sv, sv_T2.Area, sv_T2.Weight, sv_T2.KFactor);
	//Initialize
	pRTCC->PMMAEGS(aeg_C.Header, aeg_C.Data, aeg_C.Data);
	//Initialize and get DH/phase
	aeg_T.Data.TIMA = 6;
	pRTCC->PMMAEGS(aeg_T.Header, aeg_T.Data, aeg_T.Data);
	//Get actual phase and DH
	tab.ActualDH = aeg_T.Data.Item8;
	tab.ActualPhase = aeg_T.Data.Item10;

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
	double INFO[10];
	int k, jmax;
	AEGBlock aeg;

	for (int i = 0; i < 2; i++)
	{
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
			LOS_PITCH_YAW(sv_C[i].sv.R, sv_C[i].sv.V, sv_T[i].sv.R, tab.man[i].Pitch_LOS, tab.man[i].Yaw_LOS);
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
		tab.man[i].BT_LOS = _V(DT_B0*OrbMech::sign(X_BR), DT_B1*OrbMech::sign(Y_BR), DT_B2*OrbMech::sign(Z_BR));
		//Three sets of data for the first maneuver, four sets for the second
		jmax = i == 0 ? 3 : 4;
		for (int j = 0; j < jmax; j++)
		{
			k = i * 3 + (jmax - j - 1); //Changes the order
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
			//Time
			tab.app[k].GMT = T_APP;
			//Range
			DR = sv_T_temp.sv.R - sv_C_temp.sv.R;
			U_R = unit(DR);
			tab.app[k].Range = length(DR);
			//Range rate
			tab.app[k].RangeRate = dotp(sv_T_temp.sv.V - sv_C_temp.sv.V, U_R);
			//Pitch and yaw
			LOS_PITCH_YAW(sv_C_temp.sv.R, sv_C_temp.sv.V, sv_T_temp.sv.R, tab.app[k].Elev, tab.app[k].Azi);
			//Offset
			R4 = mul(OrbMech::LVLH_Matrix(sv_T_temp.sv.R, sv_T_temp.sv.V), sv_C_temp.sv.R - sv_T_temp.sv.R);
			//Change to different LVLH definition
			R4 = _V(R4.x, R4.z, -R4.y);
			//Compute phase angle between chaser and target in target vehicle plane - DPHI is positive if chaser is leading
			RC = length(sv_C_temp.sv.R);
			RT = length(sv_T_temp.sv.R);
			DPHI = atan2(R4.x, RT - R4.y);
			//Compute curvilinear coordinates of active (chaser) from passive (target)
			tab.app[k].DX = _V(RT * DPHI, -R4.z, RT - RC);
		}
		//HA and HP
		aeg = pRTCC->SVToAEG(sv_C_apo[i].sv, sv_C_apo[i].Area, sv_C_apo[i].Weight, sv_C_apo[i].KFactor);
		pRTCC->PMMAPD(aeg.Header, aeg.Data, 0, 0, INFO, NULL, NULL);
		tab.man[i].HA = INFO[4];
		tab.man[i].HP = INFO[9];
	}
}

void TwoImpulseProcessor::TransferPlan()
{
	pRTCC->PZMYSAVE.SV_before[0] = sv_C1;
	pRTCC->PZMYSAVE.V_after[0] = sv_C1_apo.sv.V;
	pRTCC->PZMYSAVE.SV_before[1] = sv_C2;
	pRTCC->PZMYSAVE.V_after[1] = sv_C2_apo.sv.V;
	pRTCC->PZMYSAVE.code[0] = "C1";
	pRTCC->PZMYSAVE.code[1] = "C2";
	if (opt.TwoImpulseTableIndicator == 1)
	{
		pRTCC->PZMYSAVE.plan[0] = pRTCC->PZTIPREG.MAN_VEH;
		pRTCC->PZMYSAVE.plan[1] = pRTCC->PZTIPREG.MAN_VEH;
	}
	else
	{
		pRTCC->PZMYSAVE.plan[0] = pRTCC->PZTIPCCD.MAN_VEH;
		pRTCC->PZMYSAVE.plan[1] = pRTCC->PZTIPCCD.MAN_VEH;
	}

	std::vector<std::string> str;
	pRTCC->PMMMED("72", str);
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

void TwoImpulseProcessor::PMDTIMP()
{
	//Pointers to tables
	TwoImpulseMultipleSolutionTable *intab = &pRTCC->PZTIPREG;
	TwoImpulseMultipleSolutionDisplay *outtab = &pRTCC->TwoImpMultDispBuffer;

	*outtab = TwoImpulseMultipleSolutionDisplay();
	outtab->ErrorMessage = "";
	if (intab->Solutions == 0)
	{
		if (intab->Updating)
		{
			outtab->ErrorMessage = "TABLE BEING UPDATED";
			return;
		}
		else
		{
			outtab->ErrorMessage = "NO TWO IMPULSE PLANS AVAILABLE";
			return;
		}
	}

	outtab->CSMSTAID = intab->CSMSTAID;
	outtab->LMSTAID = intab->LMSTAID;
	outtab->GETTH_CSM = pRTCC->GETfromGMT(intab->CSM_GMTTH);
	outtab->GETTH_LM = pRTCC->GETfromGMT(intab->LM_GMTTH);
	if (intab->MAN_VEH == 1)
	{
		outtab->MAN_VEH = "CSM";
	}
	else
	{
		outtab->MAN_VEH = "LEM";
	}

	if (intab->IVFLAG == 2)
	{
		outtab->GETFRZ = '2';
		outtab->GMTFRZ = '2';
		outtab->GETVAR = '1';
		outtab->GET1 = pRTCC->GETfromGMT(intab->data[0].Time2);
		outtab->GMT1 = intab->data[0].Time2;
	}
	else
	{
		outtab->GETFRZ = '1';
		outtab->GMTFRZ = '1';
		outtab->GETVAR = '2';
		outtab->GET1 = pRTCC->GETfromGMT(intab->data[0].Time1);
		outtab->GMT1 = intab->data[0].Time1;
	}

	if (intab->IVFLAG == 0)
	{
		outtab->OPTION = "BOTH FIXED";
	}
	else if (intab->IVFLAG == 1)
	{
		outtab->OPTION = "FIRST FIXED";
	}
	else
	{
		outtab->OPTION = "SECOND FIXED";
	}
	std::string temp;
	if (intab->DT_Light >= 0.0)
	{
		temp = "DAYLIGHT";
	}
	else
	{
		temp = "DARKNESS";
	}
	char Buffer[128];
	sprintf_s(Buffer, "%.0lf MIN UNTIL %s", abs(intab->DT_Light) / 60.0, temp.c_str());
	outtab->MinutesUntil.assign(Buffer);
	outtab->WT = pRTCC->GZGENCSN.TITravelAngle*DEG;
	outtab->PHASE = pRTCC->PZTIPREG.PhaseAngle*DEG;
	outtab->DH = pRTCC->PZTIPREG.DH / 1852.0;

	outtab->Solutions = intab->Solutions;
	outtab->showTPI = intab->showTPI;
	for (int i = 0; i < intab->Solutions; i++)
	{
		outtab->data[i].DELV1 = intab->data[i].DELV1 / 0.3048;
		outtab->data[i].YAW1 = intab->data[i].YAW1*DEG;
		outtab->data[i].PITCH1 = intab->data[i].PITCH1*DEG;
		if (intab->IVFLAG == 2)
		{
			outtab->data[i].Time2 = pRTCC->GETfromGMT(intab->data[i].Time1);
		}
		else
		{
			outtab->data[i].Time2 = pRTCC->GETfromGMT(intab->data[i].Time2);
		}
		outtab->data[i].DELV2 = intab->data[i].DELV2 / 0.3048;
		outtab->data[i].YAW2 = intab->data[i].YAW2*DEG;
		outtab->data[i].PITCH2 = intab->data[i].PITCH2*DEG;
		if (outtab->showTPI)
		{
			outtab->data[i].T_TPI = pRTCC->GETfromGMT(intab->data[i].T_TPI);
		}
		outtab->data[i].L = intab->data[i].L ? 'D' : 'N';
		outtab->data[i].C = i + 1;
	}
}

void TwoImpulseProcessor::PMDDTVCC()
{
	//Pointers to tables
	CorrectiveCombinationSolutionTable *intab = &pRTCC->PZTIPCCD;
	TwoImpulseCorrectiveCombinationDisplay *outtab = &pRTCC->TwoImpCCDispBuffer;

	*outtab = TwoImpulseCorrectiveCombinationDisplay();
	outtab->ErrorMessage = "";
	if (intab->Solutions == 0)
	{
		if (intab->Updating)
		{
			outtab->ErrorMessage = "TABLE BEING UPDATED";
			return;
		}
		else
		{
			outtab->ErrorMessage = "NO CORRECTIVE COMBINATION PLANS AVAILABLE";
			return;
		}
	}

	outtab->CSMSTAID = intab->CSMSTAID;
	outtab->LMSTAID = intab->LMSTAID;
	outtab->GETTH_CSM = pRTCC->GETfromGMT(intab->CSM_GMTTH);
	outtab->GETTH_LM = pRTCC->GETfromGMT(intab->LM_GMTTH);
	if (intab->MAN_VEH == 1)
	{
		outtab->MAN_VEH = "CSM";
	}
	else
	{
		outtab->MAN_VEH = "LEM";
	}
	outtab->GMT_NCC = intab->T_NCC;
	outtab->GET_NCC = pRTCC->GETfromGMT(intab->T_NCC);
	outtab->Solutions = intab->Solutions;
	for (int i = 0; i < intab->Solutions; i++)
	{
		outtab->data[i].Code = i + 1;
		outtab->data[i].GET_NSR = pRTCC->GETfromGMT(intab->data[i].GMT_NSR);
		outtab->data[i].GMT_NSR = intab->data[i].GMT_NSR;
		outtab->data[i].DVT = intab->data[i].DV_T / 0.3048;
		outtab->data[i].DH = intab->data[i].DH / 1852.0;
		outtab->data[i].PhaseAngle = intab->data[i].PhaseAngle*DEG;
		outtab->data[i].DT = (intab->data[i].GMT_NSR - intab->T_NCC) / 60.0;
		outtab->data[i].TSLIP = intab->data[i].T_SLIP / 60.0;
	}
}

void TwoImpulseProcessor::PMDTIPSS()
{
	//Pointers to tables
	const TwoImpulseSingleSolutionTable *intab = &pRTCC->PZTIPSS;
	TwoImpulseSingleSolutionDisplay *outtab = &pRTCC->TwoImpSingleDispBuffer;

	*outtab = TwoImpulseSingleSolutionDisplay();
	outtab->ErrorMessage = "";

	if (intab->man[0].TIG == 0.0)
	{
		outtab->ErrorMessage = "NO INFORMATION AVAILABLE AT THIS TIME";
		return;
	}
	outtab->LMSTAID = intab->LMSTAID;
	outtab->CSMSTAID = intab->LMSTAID;
	outtab->LM_GETTH = pRTCC->GETfromGMT(intab->LM_GMTTH);
	outtab->CSM_GETTH = pRTCC->GETfromGMT(intab->CSM_GMTTH);
	if (intab->MAN_VEH == 1)
	{
		outtab->MAN_VEH = "CSM";
	}
	else
	{
		outtab->MAN_VEH = "LEM";
	}
	if (intab->PointingMode == 1)
	{
		outtab->PointingMode = "TGT";
	}
	else
	{
		outtab->PointingMode = "HOR";
	}
	if (intab->TwoImpulseTableIndicator == 1)
	{
		outtab->TwoImpulseTableIndicator = "MS";
	}
	else
	{
		outtab->TwoImpulseTableIndicator = "CC";
	}
	outtab->ID = intab->PlanNumber;
	outtab->DTR = intab->man[1].TIG - intab->man[0].TIG;
	outtab->WT = intab->ActualWT*DEG;
	outtab->PHASE = intab->ActualPhase*DEG;
	outtab->DELH = intab->ActualDH / 1852.0;
	outtab->DELPITCH = intab->DeltaPitch*DEG;

	int k;
	for (int i = 0; i < 2; i++)
	{
		outtab->man[i].GET = pRTCC->GETfromGMT(intab->man[i].TIG);
		outtab->man[i].E_HOR = intab->man[i].E_HOR*DEG;
		outtab->man[i].GMT = intab->man[i].TIG;
		outtab->man[i].DV = intab->man[i].DV / 0.3048;
		outtab->man[i].Yaw = intab->man[i].Yaw*DEG;
		outtab->man[i].Pitch = intab->man[i].Pitch*DEG;
		outtab->man[i].DV_LVLH = intab->man[i].DV_LVLH / 0.3048;
		outtab->man[i].Yaw_LOS = intab->man[i].Yaw_LOS*DEG;
		outtab->man[i].Pitch_LOS = intab->man[i].Pitch_LOS*DEG;
		outtab->man[i].DV_LOS = intab->man[i].DV_LOS / 0.3048;
		outtab->man[i].BT_LOS = intab->man[i].BT_LOS;
		if (outtab->man[i].BT_LOS.x >= 0.0) outtab->man[i].BT_LOS_DIR[0] = 'F';
		else outtab->man[i].BT_LOS_DIR[0] = 'A';
		if (outtab->man[i].BT_LOS.y >= 0.0) outtab->man[i].BT_LOS_DIR[1] = 'R';
		else outtab->man[i].BT_LOS_DIR[1] = 'L';
		if (outtab->man[i].BT_LOS.z >= 0.0) outtab->man[i].BT_LOS_DIR[2] = 'D';
		else outtab->man[i].BT_LOS_DIR[2] = 'U';
		outtab->man[i].HA = intab->man[i].HA / 1852.0;
		outtab->man[i].HP = intab->man[i].HP / 1852.0;
		outtab->man[i].MinEnvironChange = (abs(intab->man[i].MinEnvironChange) - intab->man[i].TIG) / 60.0;
		if (intab->man[i].MinEnvironChange >= 0.0)
		{
			outtab->man[i].Condition = "DAYLIGHT";
		}
		else
		{
			outtab->man[i].Condition = "DARKNESS";
		}

		for (int j = 0; j < (i == 0 ? 3 : 4); j++)
		{
			k = i * 3 + j;
			outtab->app[k].GET = pRTCC->GETfromGMT(intab->app[k].GMT);
			outtab->app[k].TGT_AZ = intab->app[k].Azi*DEG;
			if (outtab->app[k].TGT_AZ < 0.0) outtab->app[k].TGT_AZ_DIR = 'L';
			else outtab->app[k].TGT_AZ_DIR = 'R';
			outtab->app[k].TGT_EL = intab->app[k].Elev*DEG;
			if (outtab->app[k].TGT_EL < 0.0) outtab->app[k].TGT_EL_DIR = 'D';
			else outtab->app[k].TGT_EL_DIR = 'U';
			outtab->app[k].RANGE = intab->app[k].Range / 1852.0;
			outtab->app[k].RDOT = intab->app[k].RangeRate / 0.3048;
			outtab->app[k].OFF = intab->app[k].DX / 1852.0;
			if (outtab->app[k].OFF.x < 0.0) outtab->app[k].X = 'T';
			else outtab->app[k].X = 'L';
			if (outtab->app[k].OFF.z < 0.0) outtab->app[k].Z = 'A';
			else outtab->app[k].Z = 'B';
			if (outtab->app[k].OFF.y < 0.0) outtab->app[k].Y = 'L';
			else outtab->app[k].Y = 'R';
		}
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

double TwoImpulseProcessor::T2Search() const
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