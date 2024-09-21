/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2021

  RTCC Numerical Integration Control Element - EMSMISS

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

#include "RTCC_EMSMISS.h"
#include "CSMLMGuidanceSim.h"
#include "TLIGuidanceSim.h"
#include "LMGuidanceSim.h"
#include "rtcc.h"

RTCC_EMSMISS::RTCC_EMSMISS(RTCC *r) : RTCCModule(r)
{
	for (int ii = 0;ii < 4;ii++)
	{
		EphemerisIndicatorList[ii] = false;
		EphemerisTableIndicatorList[ii] = NULL;
	}

	nierror = 0;
	ErrorCode = 0;
	min_ephem_dt = 0.001;
}

void RTCC_EMSMISS::Call(EMSMISSInputTable *in)
{
	//Get initial state

	intab = in;
	mpt = pRTCC->GetMPTPointer(intab->VehicleCode);

	if (in->DensityMultOverrideIndicator)
	{
		DensityMultiplier = in->DensityMultiplier;
	}
	else
	{
		DensityMultiplier = 1.0;
	}

	if (in->useInputWeights)
	{
		CurrentWeightsTable = *in->WeightsTable;
	}
	else
	{
		PLAWDTInput plawdtin;
		plawdtin.T_UP = in->AnchorVector.GMT;
		plawdtin.Num = in->IgnoreManueverNumber;
		plawdtin.KFactorOpt = true;
		plawdtin.TableCode = in->VehicleCode;
		plawdtin.VentingOpt = true;
		pRTCC->PLAWDT(plawdtin, CurrentWeightsTable);

		if (CurrentWeightsTable.Err)
		{
			//TBD
		}
	}
	InitialWeightsTable = CurrentWeightsTable;

	//Assign initial state
	state.StateVector = sv0 = in->AnchorVector;
	state.WeightsTable = CurrentWeightsTable;
	state.isLanded = in->landed;

	//Determine time limit
	double IntegDirSign = intab->IsForwardIntegration / abs(intab->IsForwardIntegration);
	gmt_lim = state.StateVector.GMT + in->MaxIntegTime*IntegDirSign;

	//Initial state of lunar stay times
	LunarStayBeginGMT = -1.0;
	LunarStayEndGMT = -1.0;

	EphemerisBuildOn = false;

	//Determine first maneuver
	i = in->IgnoreManueverNumber;
	if (i > mpt->ManeuverNum)
	{
		i = mpt->ManeuverNum;
	}

	LastManeuver = 0;

	//Determine if there is a TLI
	bool tli = false;
	unsigned tlinum;
	for (unsigned i = 0;i < mpt->ManeuverNum;i++)
	{
		if (mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
		{
			tli = true;
			tlinum = i;
		}
	}
	//Do we have a TLI?
	if (tli)
	{
		//Yes, end time for venting is TLI plus MCGVNT
		T_NV[1] = mpt->TimeToEndManeuver[tlinum] + pRTCC->SystemParameters.MCGVNT*3600.0;
	}
	else
	{
		//Don't stop venting
		T_NV[1] = 999999999.9;
	}
	T_NV[0] = pRTCC->GMTfromGET(mpt->SIVBVentingBeginGET);

	//Ephemeris or cutoff mode
	if (intab->EphemerisBuildIndicator)
	{
		EphemerisModeLogic();
	}
	else
	{
		CutoffModeLogic();
	}
	//Write output data
	WriteNIAuxOutputTable();

	//Output weights table
	if (intab->useInputWeights)
	{
		*intab->WeightsTable = state.WeightsTable;
	}

	//Maneuver times table
	if (intab->ManTimesIndicator)
	{
		ManTimesTable.TUP = mpt->CommonBlock.TUP;
		*intab->ManTimesIndicator = ManTimesTable;
	}
	//Maneuver integrator table
	if (intab->AuxTableIndicator)
	{
		*intab->AuxTableIndicator = AuxTableIndicator;
	}
	if (intab->CutoffIndicator == 5)
	{
		//TBD: Reference switch table
	}
	if (intab->EphemerisBuildIndicator)
	{
		WriteEphemerisHeaders();
	}
}

void RTCC_EMSMISS::CutoffModeLogic()
{
	double gmt;
	
RTCC_EMSMISS_CutoffModeLogic_START:
	manflag = NextManeuverTime(gmt);

	//Maneuver error
	if (nierror)
	{
		ErrorCode = nierror;
		return;
	}

	if (manflag)
	{
		gmt_coast = NextEventTime(gmt);
	}
	else
	{
		gmt_coast = gmt_lim;
	}

	if (state.isLanded)
	{
		LunarStayPhase();
	}
	else
	{
		CallCoastIntegrator();
		//Correct weights for venting
		UpdateWeightsTableAndSVAfterCoast();
	}

	if (EphemerisBuildOn)
	{
		AddCoastOrSurfaceEphemeris();
	}

	//Coasting integration didn't end on time, so go to end directly
	if (TerminationCode != 1)
	{
		return;
	}

	//Do we have to take maneuvers into account?
	if (manflag)
	{

		if (intab->ManCutoffIndicator == 0)
		{
			//Stop
			TerminationCode = 6;
			return;
		}

		CallManeuverIntegrator();

		//Maneuver error
		if (nierror)
		{
			ErrorCode = nierror;
			return;
		}

		if (EphemerisBuildOn)
		{
			AddManeuverEphemeris();
		}

		if (intab->ManCutoffIndicator == 1 || (mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_PGNS_DESCENT && intab->DescentBurnIndicator))
		{
			//Stop
			TerminationCode = 7;
			return;
		}

		i++;
		goto RTCC_EMSMISS_CutoffModeLogic_START;
	}
}

void RTCC_EMSMISS::EphemerisModeLogic()
{
	bool initerr = InitEphemTables();	
	if (initerr)
	{
		ErrorCode = 99;
		return;
	}

	//First get to the initial time of ephemeris storing
	gmt_lim = intab->EphemerisLeftLimitGMT;

	//Propagate state to EphemerisLeftLimitGMT
	CutoffModeLogic();

	if (ErrorCode) return;

	//Switch on ephemeris writing
	EphemerisBuildOn = true;

	//Restore cutoff conditions
	gmt_lim = intab->EphemerisRightLimitGMT;

	//Now generate the ephemeris
	CutoffModeLogic();
}

void RTCC_EMSMISS::WriteNIAuxOutputTable()
{
	intab->NIAuxOutputTable.sv_cutoff = state.StateVector;
	intab->NIAuxOutputTable.landed = state.isLanded;
	intab->NIAuxOutputTable.InputArea = InitialWeightsTable.ConfigArea;
	intab->NIAuxOutputTable.InputWeight = InitialWeightsTable.ConfigWeight;
	intab->NIAuxOutputTable.CutoffArea = state.WeightsTable.ConfigArea;
	intab->NIAuxOutputTable.CutoffWeight = state.WeightsTable.ConfigWeight;
	intab->NIAuxOutputTable.ErrorCode = ErrorCode;
	intab->NIAuxOutputTable.TerminationCode = TerminationCode;
	intab->NIAuxOutputTable.ManeuverNumber = LastManeuver;
	intab->NIAuxOutputTable.LunarStayBeginGMT = LunarStayBeginGMT;
	intab->NIAuxOutputTable.LunarStayEndGMT = LunarStayEndGMT;
}

void RTCC_EMSMISS::UpdateWeightsTableAndSVAfterCoast()
{
	PLAWDTInput plawdtin;

	plawdtin.T_UP = svtemp.GMT;
	plawdtin.Num = (int)state.WeightsTable.CC.to_ulong();
	plawdtin.TableCode = -intab->VehicleCode;
	plawdtin.VentingOpt = true;
	plawdtin.CSMArea = state.WeightsTable.CSMArea;
	plawdtin.LMAscArea = state.WeightsTable.LMAscArea;
	plawdtin.LMDscArea = state.WeightsTable.LMDscArea;
	plawdtin.SIVBArea = state.WeightsTable.SIVBArea;
	plawdtin.CSMWeight = state.WeightsTable.CSMWeight;
	plawdtin.LMAscWeight = state.WeightsTable.LMAscWeight;
	plawdtin.LMDscWeight = state.WeightsTable.LMDscWeight;
	plawdtin.SIVBWeight = state.WeightsTable.SIVBWeight;
	plawdtin.T_IN = state.StateVector.GMT;

	pRTCC->PLAWDT(plawdtin, CurrentWeightsTable);

	if (CurrentWeightsTable.Err)
	{
		//TBD
	}

	//Update state
	state.StateVector = svtemp;
	state.WeightsTable = CurrentWeightsTable;
}

void RTCC_EMSMISS::UpdateWeightsTableAndSVAfterManeuver()
{
	state.StateVector.R = AuxTableIndicator.R_BO;
	state.StateVector.V = AuxTableIndicator.V_BO;
	state.StateVector.GMT = AuxTableIndicator.GMT_BO;
	state.StateVector.RBI = AuxTableIndicator.RBI;

	//TBD: Docking maneuver

	double dmass = AuxTableIndicator.MainFuelUsed + AuxTableIndicator.RCSFuelUsed;
	switch (mpt->mantable[i].TVC)
	{
	case RTCC_MANVEHICLE_CSM:
		state.WeightsTable.CSMWeight -= dmass;
		break;
	case RTCC_MANVEHICLE_SIVB:
		state.WeightsTable.SIVBWeight -= dmass;
		break;
	case RTCC_MANVEHICLE_LM:
		if (mpt->mantable[i].Thruster == RTCC_ENGINETYPE_LMDPS)
		{
			state.WeightsTable.LMDscWeight -= dmass;
		}
		else
		{
			state.WeightsTable.LMAscWeight -= dmass;
		}
		break;
	}

	state.WeightsTable.ConfigWeight = state.WeightsTable.CSMWeight + state.WeightsTable.SIVBWeight + state.WeightsTable.LMAscWeight + state.WeightsTable.LMDscWeight;

	MANTIMESData data;

	data.ManData[0] = AuxTableIndicator.GMT_1;
	data.ManData[1] = AuxTableIndicator.GMT_BO;

	ManTimesTable.Table.push_back(data);
}

double RTCC_EMSMISS::NextEventTime(double gmt)
{
	if (gmt_lim - state.StateVector.GMT >= 0.0)
	{
		if (gmt >= gmt_lim)
		{
			manflag = false;
			return gmt_lim;
		}
		else
		{
			return gmt;
		}
	}
	else
	{
		if (gmt < gmt_lim)
		{
			return gmt_lim;
		}
		else
		{
			return gmt;
		}
	}
}

bool RTCC_EMSMISS::NextManeuverTime(double &gmt)
{
	if (intab->ManeuverIndicator == false) return false;
	if (gmt_lim - state.StateVector.GMT < 0.0) return false;

	double t_right;

	for (unsigned j = i;j < mpt->mantable.size();j++)
	{
		t_right = mpt->TimeToBeginManeuver[j];
		if (state.StateVector.GMT <= t_right)
		{
			//Found an upcoming maneuver
			/*
			//Special PDI logic. This is probably not the right way to do it.
			if (mpt->mantable[j].AttitudeCode == RTCC_ATTITUDE_PGNS_DESCENT)
			{
				PMMLDPInput tigin;

				tigin.CurrentManeuver = j;
				tigin.HeadsUpDownInd = mpt->mantable[j].HeadsUpDownInd;
				tigin.mpt = mpt;

				tigin.sv.R = state.StateVector.R;
				tigin.sv.V = state.StateVector.V;
				tigin.sv.MJD = OrbMech::MJDfromGET(state.StateVector.GMT, pRTCC->SystemParameters.GMTBASE);
				tigin.sv.gravref = pRTCC->GetGravref(state.StateVector.RBI);

				tigin.sv.mass = state.WeightsTable.ConfigWeight;
				tigin.TLAND = pRTCC->GETfromGMT(mpt->mantable[j].GMT_BO);
				tigin.TrimAngleInd = mpt->mantable[j].TrimAngleInd;

				nierror = pRTCC->PMMLDP(tigin, mpt->mantable[j]);
			}*/

			i = j;
			gmt = t_right;
			return true;
		}
	}

	return false;
}

void RTCC_EMSMISS::LunarStayPhase()
{
	LunarStayBeginGMT = state.StateVector.GMT + 0.1;
	LunarStayEndGMT = gmt_coast;

	//Move only GMT of state vector to current time. Actual state vector shouldn't matter
	state.StateVector.GMT = gmt_coast;
	//Lunar stay phase always ends on time
	TerminationCode = 1;

	if (EphemerisBuildOn)
	{
		EMSLSFInputTable emslsfin;

		emslsfin.ECIEphemerisIndicator = intab->ECIEphemerisIndicator;
		emslsfin.ECIEphemTableIndicator = &tempcoastephemtable[0];
		emslsfin.ECTEphemerisIndicator = intab->ECTEphemerisIndicator;
		emslsfin.ECTEphemTableIndicator = &tempcoastephemtable[1];
		emslsfin.MCIEphemerisIndicator = intab->MCIEphemerisIndicator;
		emslsfin.MCIEphemTableIndicator = &tempcoastephemtable[2];
		emslsfin.MCTEphemerisIndicator = intab->MCTEphemerisIndicator;
		emslsfin.MCTEphemTableIndicator = &tempcoastephemtable[3];
		emslsfin.EphemerisLeftLimitGMT = LunarStayBeginGMT;
		emslsfin.EphemerisRightLimitGMT = LunarStayEndGMT;
		emslsfin.LunarEphemDT = intab->LunarEphemDT;

		pRTCC->EMSLSF(emslsfin);
	}
}

void RTCC_EMSMISS::CallCoastIntegrator()
{
	emmeniin.AnchorVector = state.StateVector;
	emmeniin.Area = state.WeightsTable.ConfigArea;
	emmeniin.CutoffIndicator = intab->CutoffIndicator;
	emmeniin.DensityMultiplier = DensityMultiplier;
	emmeniin.EarthRelStopParam = intab->EarthRelStopParam;
	emmeniin.MaxIntegTime = gmt_coast - state.StateVector.GMT;
	if (emmeniin.MaxIntegTime >= 0)
	{
		emmeniin.IsForwardIntegration = abs(intab->IsForwardIntegration);
	}
	else
	{
		emmeniin.IsForwardIntegration = -abs(intab->IsForwardIntegration);
		emmeniin.MaxIntegTime = abs(emmeniin.MaxIntegTime);
	}
	emmeniin.MoonRelStopParam = intab->MoonRelStopParam;
	emmeniin.StopParamRefFrame = intab->StopParamRefFrame;
	emmeniin.Weight = state.WeightsTable.ConfigWeight;
	emmeniin.MinEphemDT = intab->MinEphemDT;
	emmeniin.UseFixedStepLength = false;

	emmeniin.EphemerisBuildIndicator = EphemerisBuildOn;
	emmeniin.ECIEphemerisIndicator = intab->ECIEphemerisIndicator;
	emmeniin.ECIEphemTableIndicator = &tempcoastephemtable[0];
	emmeniin.ECTEphemerisIndicator = intab->ECTEphemerisIndicator;
	emmeniin.ECTEphemTableIndicator = &tempcoastephemtable[1];
	emmeniin.MCIEphemerisIndicator = intab->MCIEphemerisIndicator;
	emmeniin.MCIEphemTableIndicator = &tempcoastephemtable[2];
	emmeniin.MCTEphemerisIndicator = intab->MCTEphemerisIndicator;
	emmeniin.MCTEphemTableIndicator = &tempcoastephemtable[3];

	if (CurrentWeightsTable.CC[RTCC_CONFIG_S] && (state.StateVector.GMT > T_NV[0]) && (state.StateVector.GMT < T_NV[1]))
	{
		emmeniin.VentPerturbationFactor = 1.0;
	}
	else
	{
		emmeniin.VentPerturbationFactor = -1.0;
	}

	for (int ii = 0; ii < 2; ii++)
	{
		//Clear temporary ephemeris tables
		for (int j = 0; j < 4; j++)
		{
			tempcoastephemtable[j].table.clear();
		}

		//Call Encke integrator
		pRTCC->EMMENI(emmeniin);

		//No need for the checks on the second pass through
		if (ii > 0) break;

		if (emmeniin.EphemerisBuildIndicator && intab->MinNumEphemPoints > 0U)
		{
			//If ephemeris is being built and the minimum number of desired points haven't been generated, repeat EMMENI with fixed step length

			bool enough = true;

			if (emmeniin.ECIEphemerisIndicator)
			{
				if (intab->MinNumEphemPoints > emmeniin.ECIEphemTableIndicator->Header.NumVec) enough = false;
			}
			if (emmeniin.ECTEphemerisIndicator)
			{
				if (intab->MinNumEphemPoints > emmeniin.ECTEphemTableIndicator->Header.NumVec) enough = false;
			}
			if (emmeniin.MCIEphemerisIndicator)
			{
				if (intab->MinNumEphemPoints > emmeniin.MCIEphemTableIndicator->Header.NumVec) enough = false;
			}
			if (emmeniin.MCTEphemerisIndicator)
			{
				if (intab->MinNumEphemPoints > emmeniin.MCTEphemTableIndicator->Header.NumVec) enough = false;
			}

			if (enough == false)
			{
				double interval;

				//Repeat EMMENI with step length giving desired number of ephemeris points

				//Turn this off so it doesn't get in the way
				emmeniin.MinEphemDT = 0.0;
				//Turn on fixed step integration
				emmeniin.UseFixedStepLength = true;
				//Interval is output GMT minus anchor vector GMT, as integration might have not stopped at time but e.g. altitude
				interval = emmeniin.sv_cutoff.GMT - emmeniin.AnchorVector.GMT;
				//Step length adjusted to give desired number of ephemeris points
				emmeniin.FixedStepLength = interval / ((double)(intab->MinNumEphemPoints - 1U));
			}
			else
			{
				//Enough state vectors
				break;
			}
		}
		else
		{
			//Otherwise we are finished
			break;
		}
	}
	svtemp = emmeniin.sv_cutoff;
	TerminationCode = emmeniin.TerminationCode;
}

bool RTCC_EMSMISS::InitEphemTables()
{
	if (intab->ECIEphemerisIndicator)
	{
		if (intab->ECIEphemTableIndicator == NULL)
		{
			//Table not provided, error
			return true;
		}
		EphemerisIndicatorList[0] = intab->ECIEphemerisIndicator;
		EphemerisTableIndicatorList[0] = intab->ECIEphemTableIndicator;
		intab->ECIEphemTableIndicator->table.clear();
	}
	if (intab->ECTEphemerisIndicator)
	{
		if (intab->ECTEphemTableIndicator == NULL)
		{
			//Table not provided, error
			return true;
		}
		EphemerisIndicatorList[1] = intab->ECTEphemerisIndicator;
		EphemerisTableIndicatorList[1] = intab->ECTEphemTableIndicator;
		intab->ECTEphemTableIndicator->table.clear();
	}
	if (intab->MCIEphemerisIndicator)
	{
		if (intab->MCIEphemTableIndicator == NULL)
		{
			//Table not provided, error
			return true;
		}
		EphemerisIndicatorList[2] = intab->MCIEphemerisIndicator;
		EphemerisTableIndicatorList[2] = intab->MCIEphemTableIndicator;
		intab->MCIEphemTableIndicator->table.clear();
	}
	if (intab->MCTEphemerisIndicator)
	{
		if (intab->MCTEphemTableIndicator == NULL)
		{
			//Table not provided, error
			return true;
		}
		EphemerisIndicatorList[3] = intab->MCTEphemerisIndicator;
		EphemerisTableIndicatorList[3] = intab->MCTEphemTableIndicator;
		intab->MCTEphemTableIndicator->table.clear();
	}

	return false;
}

void RTCC_EMSMISS::WriteEphemerisHeaders()
{
	for (int ii = 0;ii < 4;ii++)
	{
		if (EphemerisIndicatorList[ii])
		{
			EphemerisTableIndicatorList[ii]->Header.CSI = ii;
			EphemerisTableIndicatorList[ii]->Header.NumVec = EphemerisTableIndicatorList[ii]->table.size();
			EphemerisTableIndicatorList[ii]->Header.Offset = 0;
			EphemerisTableIndicatorList[ii]->Header.Status = 0;
			EphemerisTableIndicatorList[ii]->Header.TL = EphemerisTableIndicatorList[ii]->table.front().GMT;
			EphemerisTableIndicatorList[ii]->Header.TR = EphemerisTableIndicatorList[ii]->table.back().GMT;
			EphemerisTableIndicatorList[ii]->Header.TUP = mpt->CommonBlock.TUP;
			EphemerisTableIndicatorList[ii]->Header.VEH = intab->VehicleCode;
		}
	}
}

void RTCC_EMSMISS::AddCoastOrSurfaceEphemeris()
{
	double dt;
	for (unsigned int ii = 0;ii < 4;ii++)
	{
		if (EphemerisIndicatorList[ii])
		{
			//Add ephemeris data from temporary storage to final storage
			for (unsigned int jj = 0;jj < tempcoastephemtable[ii].table.size();jj++)
			{
				//If the GMT is identical to the last GMT in the table then don't write the same state vector again
				if (EphemerisTableIndicatorList[ii]->table.size() > 0)
				{
					dt = EphemerisTableIndicatorList[ii]->table.back().GMT - tempcoastephemtable[ii].table[jj].GMT;
					if (abs(dt) < min_ephem_dt)
					{
						continue;
					}
				}

				EphemerisTableIndicatorList[ii]->table.push_back(tempcoastephemtable[ii].table[jj]);
			}
			//Clear table
			tempcoastephemtable[ii].table.clear();
		}
	}
}

void RTCC_EMSMISS::AddManeuverEphemeris()
{
	EphemerisData2 sv_out;
	double dt;
	int in, converr;

	if (tempephemtable.Header.CSI == BODY_EARTH)
	{
		in = 0;
	}
	else
	{
		in = 2;
	}

	for (unsigned int ii = 0;ii < 4;ii++)
	{
		if (EphemerisIndicatorList[ii])
		{
			//Add ephemeris data from temporary storage to final storage
			for (unsigned int jj = 0;jj < tempephemtable.table.size();jj++)
			{
				//If the GMT is identical to the last GMT in the table then don't write the same state vector again
				if (EphemerisTableIndicatorList[ii]->table.size() > 0)
				{
					dt = EphemerisTableIndicatorList[ii]->table.back().GMT - tempephemtable.table[jj].GMT;

					if (abs(dt) < min_ephem_dt)
					{
						continue;
					}
				}

				//Convert to desired coordinate system
				converr = pRTCC->ELVCNV(tempephemtable.table[jj], in, ii, sv_out);

				//Put on table
				EphemerisTableIndicatorList[ii]->table.push_back(sv_out);
			}
			tempephemtable.table.clear();
		}
	}
}

void RTCC_EMSMISS::CallManeuverIntegrator()
{
	//Update weights to beginning of maneuver
	WeightsAtManeuverBegin();

	if (mpt->mantable[i].AttitudeCode <= 5)
	{
		CallCSMLMIntegrator();
	}
	else if (mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
	{
		CallSIVBIntegrator();
	}
	else if (mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_PGNS_DESCENT)
	{
		CallDescentIntegrator();
		state.isLanded = true;
	}
	else
	{
		CallAscentIntegrator();
		state.isLanded = false;
	}

	LastManeuver = i + 1;

	if (nierror)
	{
		pRTCC->RTCCONLINEMON.IntBuffer[0] = nierror;
		pRTCC->RTCCONLINEMON.IntBuffer[1] = i + 1;
		if (intab->VehicleCode == RTCC_MPT_CSM)
		{
			pRTCC->RTCCONLINEMON.TextBuffer[0] = "CSM";
		}
		else
		{
			pRTCC->RTCCONLINEMON.TextBuffer[0] = "LEM";
		}

		pRTCC->EMGPRINT("EMSMISS", 17);
		return;
	}

	//Update weights table and SV
	UpdateWeightsTableAndSVAfterManeuver();
}

void RTCC_EMSMISS::WeightsAtManeuverBegin()
{
	if (mpt->mantable[i].CommonBlock.ConfigChangeInd == RTCC_CONFIGCHANGE_UNDOCKING)
	{
		CurrentWeightsTable.CC = mpt->mantable[i].CommonBlock.ConfigCode;

		CurrentWeightsTable.ConfigWeight = CurrentWeightsTable.CSMWeight = CurrentWeightsTable.LMAscWeight = CurrentWeightsTable.LMDscWeight = CurrentWeightsTable.SIVBWeight = 0.0;
		CurrentWeightsTable.ConfigArea = CurrentWeightsTable.CSMArea = CurrentWeightsTable.LMAscArea = CurrentWeightsTable.LMDscArea = 0.0;

		if (mpt->mantable[i].CommonBlock.ConfigCode[RTCC_CONFIG_C])
		{
			CurrentWeightsTable.CSMWeight = state.WeightsTable.CSMWeight;
			CurrentWeightsTable.ConfigWeight += CurrentWeightsTable.CSMWeight;
			if (state.WeightsTable.CSMArea > CurrentWeightsTable.ConfigArea)
			{
				CurrentWeightsTable.ConfigArea = state.WeightsTable.CSMArea;
			}
		}
		if (mpt->mantable[i].CommonBlock.ConfigCode[RTCC_CONFIG_S])
		{
			CurrentWeightsTable.SIVBWeight = state.WeightsTable.SIVBWeight;
			CurrentWeightsTable.ConfigWeight += CurrentWeightsTable.SIVBWeight;
			if (state.WeightsTable.SIVBArea > CurrentWeightsTable.ConfigArea)
			{
				CurrentWeightsTable.ConfigArea = state.WeightsTable.SIVBArea;
			}
		}
		if (mpt->mantable[i].CommonBlock.ConfigCode[RTCC_CONFIG_A])
		{
			CurrentWeightsTable.LMAscWeight = state.WeightsTable.LMAscWeight;
			CurrentWeightsTable.ConfigWeight += CurrentWeightsTable.LMAscWeight;
			if (state.WeightsTable.LMAscArea > CurrentWeightsTable.ConfigArea)
			{
				CurrentWeightsTable.ConfigArea = state.WeightsTable.LMAscArea;
			}
		}

		if (mpt->mantable[i].CommonBlock.ConfigCode[RTCC_CONFIG_D])
		{
			CurrentWeightsTable.LMDscWeight = state.WeightsTable.LMDscWeight;
			CurrentWeightsTable.ConfigWeight += CurrentWeightsTable.LMDscWeight;
			if (state.WeightsTable.LMDscArea > CurrentWeightsTable.ConfigArea)
			{
				CurrentWeightsTable.ConfigArea = state.WeightsTable.LMDscArea;
			}
		}

		state.WeightsTable = CurrentWeightsTable;
	}
	else if (mpt->mantable[i].CommonBlock.ConfigChangeInd == RTCC_CONFIGCHANGE_DOCKING)
	{
		//TBD
	}
}

void RTCC_EMSMISS::CallCSMLMIntegrator()
{
	PMMRKJInputArray integin;

	integin.AT = mpt->mantable[i].A_T;
	integin.DENSMULT = DensityMultiplier;
	integin.DOCKANG = mpt->mantable[i].DockingAngle;
	integin.DPSScale = mpt->mantable[i].DPSScaleFactor;
	integin.DTMAN = mpt->mantable[i].dt;
	integin.DTOUT = 10.0;
	integin.DTPS10 = mpt->mantable[i].DT_10PCT;
	integin.DTU = mpt->mantable[i].dt_ullage;
	integin.DVMAN = mpt->mantable[i].dv;
	integin.HeadsUpDownInd = mpt->mantable[i].HeadsUpDownInd;
	integin.IC = state.WeightsTable.CC.to_ulong();
	integin.KAUXOP = 1;
	if (EphemerisBuildOn)
	{
		integin.KEPHOP = 1;
	}
	else
	{
		integin.KEPHOP = 0;
	}
	integin.KTRIMOP = mpt->mantable[i].TrimAngleInd;
	if (state.WeightsTable.CC[RTCC_CONFIG_D] == true)
	{
		integin.LMDESCJETT = 1e70;
	}
	else
	{
		integin.LMDESCJETT = 0;
	}

	if (mpt->mantable[i].AttitudesInput)
	{
		integin.MANOP = -mpt->mantable[i].AttitudeCode;
	}
	else
	{
		integin.MANOP = mpt->mantable[i].AttitudeCode;
	}

	integin.sv0 = state.StateVector;
	integin.ThrusterCode = mpt->mantable[i].Thruster;
	integin.TVC = mpt->mantable[i].TVC;
	integin.UllageOption = mpt->mantable[i].UllageThrusterOpt;
	integin.ExtDVCoordInd = mpt->mantable[i].Word67i[0];
	if (mpt->mantable[i].Word67i[0])
	{
		integin.VG = mpt->mantable[i].dV_LVLH;
	}
	else
	{
		integin.VG = mpt->mantable[i].dV_inertial;
	}

	integin.WDMULT = 1.0;
	integin.XB = mpt->mantable[i].X_B;
	integin.YB = mpt->mantable[i].Y_B;
	integin.ZB = mpt->mantable[i].Z_B;

	integin.CAPWT = state.WeightsTable.ConfigWeight;
	integin.A = state.WeightsTable.ConfigArea;
	integin.CSMWT = state.WeightsTable.CSMWeight;
	integin.SIVBWT = state.WeightsTable.SIVBWeight;
	integin.LMAWT = state.WeightsTable.LMAscWeight;
	integin.LMDWT = state.WeightsTable.LMDscWeight;

	CSMLMPoweredFlightIntegration numin(pRTCC, integin, nierror, &tempephemtable, &AuxTableIndicator);
	numin.PMMRKJ();
}

void RTCC_EMSMISS::CallSIVBIntegrator()
{
	RTCCNIInputTable integin;

	integin.R = svtemp.R;
	integin.V = svtemp.V;
	integin.DTOUT = intab->ManEphemDT;
	integin.WDMULT = 1.0;
	integin.DENSMULT = DensityMultiplier;
	if (EphemerisBuildOn)
	{
		integin.IEPHOP = 1;
	}
	else
	{
		integin.IEPHOP = 0;
	}
	integin.KAUXOP = true;
	integin.MAXSTO = 1000;

	integin.CAPWT = state.WeightsTable.ConfigWeight;
	integin.Area = state.WeightsTable.ConfigArea;
	integin.CSMWT = state.WeightsTable.CSMWeight;
	integin.SIVBWT = state.WeightsTable.SIVBWeight;
	integin.LMAWT = state.WeightsTable.LMAscWeight;
	integin.LMDWT = state.WeightsTable.LMDscWeight;

	integin.MANOP = mpt->mantable[i].AttitudeCode;
	integin.ThrusterCode = mpt->mantable[i].Thruster;
	integin.IC = mpt->mantable[i].ConfigCodeBefore.to_ulong();
	integin.MVC = mpt->mantable[i].TVC;
	integin.IFROZN = mpt->mantable[i].FrozenManeuverInd;
	integin.IREF = mpt->mantable[i].RefBodyInd;
	integin.ICOORD = mpt->mantable[i].CoordSysInd;
	integin.GMTBASE = pRTCC->SystemParameters.GMTBASE;
	integin.GMTI = mpt->mantable[i].GMTMAN;
	integin.DTINP = mpt->mantable[i].dt;
	integin.R_frozen = mpt->mantable[i].FrozenManeuverVector.R;
	integin.V_frozen = mpt->mantable[i].FrozenManeuverVector.V;
	integin.GMTI_frozen = mpt->mantable[i].FrozenManeuverVector.GMT;
	//e_N
	integin.Params[0] = mpt->mantable[i].dV_inertial.z;
	//C3
	integin.Params[1] = mpt->mantable[i].dV_LVLH.x;
	//alpha_D
	integin.Params[2] = mpt->mantable[i].dV_LVLH.y;
	//f
	integin.Params[3] = mpt->mantable[i].dV_LVLH.z;
	//P
	integin.Params[4] = mpt->mantable[i].Word67d;
	//K5
	integin.Params[5] = mpt->mantable[i].Word68;
	//R_T or T_M
	integin.Params[6] = mpt->mantable[i].Word69;
	//V_T
	integin.Params[7] = mpt->mantable[i].Word70;
	//gamma_T
	integin.Params[8] = mpt->mantable[i].Word71;
	//G_T
	integin.Params[9] = mpt->mantable[i].Word72;
	integin.Params[10] = mpt->mantable[i].Word73;
	integin.Params[11] = mpt->mantable[i].Word74;
	integin.Params[13] = mpt->mantable[i].Word76;
	integin.Params[14] = mpt->mantable[i].Word77;
	integin.Word68i[0] = mpt->mantable[i].Word78i[0];
	integin.Word68i[1] = mpt->mantable[i].Word78i[1];
	integin.Params2[0] = mpt->mantable[i].Word79;
	integin.Params2[1] = mpt->mantable[i].Word80;
	integin.Params2[2] = mpt->mantable[i].Word81;
	integin.Params2[3] = mpt->mantable[i].Word82;
	integin.Params2[4] = mpt->mantable[i].Word83;
	integin.Params2[5] = mpt->mantable[i].Word84;

	//Link to TLI matrix table
	MATRIX3 ADRMAT[3];
	if (intab->VehicleCode == RTCC_MPT_CSM)
	{
		ADRMAT[0] = pRTCC->PZMATCSM.EPH;
		ADRMAT[1] = pRTCC->PZMATCSM.GG;
		ADRMAT[2] = pRTCC->PZMATCSM.G;
	}
	else
	{
		ADRMAT[0] = pRTCC->PZMATLEM.EPH;
		ADRMAT[1] = pRTCC->PZMATLEM.GG;
		ADRMAT[2] = pRTCC->PZMATLEM.G;
	}

	TLIGuidanceSim numin(pRTCC, integin, nierror, &tempephemtable, &AuxTableIndicator, ADRMAT);
	numin.PCMTRL();
}

void RTCC_EMSMISS::CallDescentIntegrator()
{
	PMMLDIInput integin;

	integin.sv.R = svtemp.R;
	integin.sv.V = svtemp.V;
	integin.sv.MJD = OrbMech::MJDfromGET(svtemp.GMT, pRTCC->SystemParameters.GMTBASE);
	integin.sv.gravref = pRTCC->GetGravref(svtemp.RBI);

	integin.TLAND = pRTCC->GETfromGMT(mpt->mantable[i].GMT_BO);

	integin.sv.mass = state.WeightsTable.ConfigWeight;
	integin.W_LMA = state.WeightsTable.LMAscWeight;
	integin.W_LMD = state.WeightsTable.LMDscWeight;

	nierror = 0;
	pRTCC->PMMLDI(integin, AuxTableIndicator, &tempephemtable);
}

void RTCC_EMSMISS::CallAscentIntegrator()
{
	PMMLAIInput integin;

	integin.m0 = state.WeightsTable.ConfigWeight;

	if (mpt->mantable[i].FrozenManeuverInd)
	{
		integin.sv_CSM.R.x = mpt->mantable[i].Word73;
		integin.sv_CSM.R.y = mpt->mantable[i].Word74;
		integin.sv_CSM.R.z = mpt->mantable[i].Word75;
		integin.sv_CSM.V.x = mpt->mantable[i].Word76;
		integin.sv_CSM.V.y = mpt->mantable[i].Word77;
		integin.sv_CSM.V.z = mpt->mantable[i].Word78d;
		integin.sv_CSM.GMT = mpt->mantable[i].Word79;
		integin.sv_CSM.RBI = BODY_MOON;
	}
	else
	{
		EphemerisData SV;
		double gmt_sv;
		//If desired GMT is before start of CSM ephemeris use start of CSM ephemeris instead
		if (state.StateVector.GMT < pRTCC->EZEPH1.EPHEM.Header.TL)
		{
			gmt_sv = pRTCC->EZEPH1.EPHEM.Header.TL;
		}
		else
		{
			gmt_sv = state.StateVector.GMT;
		}
		if (pRTCC->ELFECH(gmt_sv, RTCC_MPT_CSM, SV))
		{
			nierror = 1;
			return;
		}

		integin.sv_CSM = SV;
	}

	integin.t_liftoff = mpt->mantable[i].GMTMAN;
	integin.v_LH = mpt->mantable[i].dV_LVLH.z;
	integin.v_LV = mpt->mantable[i].dV_LVLH.x;

	nierror = 0;
	pRTCC->PMMLAI(integin, AuxTableIndicator, &tempephemtable);
}