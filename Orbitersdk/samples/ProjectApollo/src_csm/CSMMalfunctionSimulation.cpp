/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

CSM Malfunction Simulation

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

#include "CSMMalfunctionSimulation.h"
#include "Malfunction.h"
#include "Orbitersdk.h"
#include "soundlib.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "iu.h"
#include "saturn.h"
#include "nassputils.h"

using namespace nassp;

CSMMalfunctionSimulation::CSMMalfunctionSimulation(Saturn *s) : sat(s)
{
	Add(new Malfunction("LET AutoJet Fail"));
	Add(new Malfunction("LES Jet Motor Fail"));
	Add(new Malfunction("Liftoff Circuit A Fail"));
	Add(new Malfunction("Liftoff Circuit B Fail"));
	Add(new Malfunction("Auto Abort Enable Fail"));
	Add(new Malfunction("Tower Jett 1 Fail"));
	Add(new Malfunction("Tower Jett 2 Fail"));
	Add(new Malfunction("SM Jett 1 Fail"));
	Add(new Malfunction("SM Jett 2 Fail"));
	Add(new Malfunction("Apex Cover Fail"));
	Add(new Malfunction("Drogue Chute Fail"));
	Add(new Malfunction("Main Chute Fail"));
	Add(new Malfunction("S-I Engine 1 Fail"));
	Add(new Malfunction("S-I Engine 2 Fail"));
	Add(new Malfunction("S-I Engine 3 Fail"));
	Add(new Malfunction("S-I Engine 4 Fail"));
	Add(new Malfunction("S-I Engine 5 Fail"));
	Add(new Malfunction("S-I Engine 6 Fail"));
	Add(new Malfunction("S-I Engine 7 Fail"));
	Add(new Malfunction("S-I Engine 8 Fail"));
	Add(new Malfunction("S-II Auto Sep Fail")); // Stage two will fail to seperate automatically from stage one.
	Add(new Malfunction("S-II Engine 1 Fail"));
	Add(new Malfunction("S-II Engine 2 Fail"));
	Add(new Malfunction("S-II Engine 3 Fail"));
	Add(new Malfunction("S-II Engine 4 Fail"));
	Add(new Malfunction("S-II Engine 5 Fail"));
	Add(new Malfunction("IU Platform Fail"));
	Add(new Malfunction("S-IVB Engine Fail"));
	Add(new Malfunction("Fuel Cell 1 Discon."));
	Add(new Malfunction("Fuel Cell 2 Discon."));
	Add(new Malfunction("Fuel Cell 3 Discon."));
	Add(new Malfunction("CSM LV Separation Fail"));
	Add(new Malfunction("S4B O2/H2 Burner Fail"));

	//Add 60 CWS light failures, as each needs to be handled separately
	char Buffer[128];
	for (int i = 0; i < 60; i++)
	{
		sprintf(Buffer, "CWS Light %d Fail", i + 1);
		Add(new Malfunction(Buffer));
	}
}

double CSMMalfunctionSimulation::GetTimeReference(int i)
{
	if (i == 1) //1 = simulation time
	{
		return oapiGetSimTime();
	}
	else if (i == 2) //2 = time since S-I/S-II (or S-IVB) staging
	{
		if (sat->eventControl.SECOND_STAGE_STAGING == MINUS_INFINITY)
		{
			return MINUS_INFINITY;
		}
		return sat->GetMissionTime() - sat->eventControl.SECOND_STAGE_STAGING;
	}
	else if (i == 3) //3 = time since S-II/S-IVB staging
	{
		if (sat->eventControl.SIVB_STAGE_STAGING == MINUS_INFINITY)
		{
			return MINUS_INFINITY;
		}
		return sat->GetMissionTime() - sat->eventControl.SIVB_STAGE_STAGING;
	}
	else if (i == 4) //4 = time since Earth Orbit Insertion
	{
		if (sat->eventControl.EARTH_ORBIT_INSERTION == MINUS_INFINITY)
		{
			return MINUS_INFINITY;
		}
		return sat->GetMissionTime() - sat->eventControl.EARTH_ORBIT_INSERTION;
	}
	else if (i == 5) //5 = time since Timebase 6
	{
		if (sat->eventControl.TLI == MINUS_INFINITY)
		{
			return MINUS_INFINITY;
		}
		return sat->GetMissionTime() - sat->eventControl.TLI;
	}
	else if (i == 6) //6 = time since TLI cutoff
	{
		if (sat->eventControl.TLI_DONE == MINUS_INFINITY)
		{
			return MINUS_INFINITY;
		}
		return sat->GetMissionTime() - sat->eventControl.TLI_DONE;
	}

	//0 = mission time
	return sat->GetMissionTime();
}

void CSMMalfunctionSimulation::SetFailure(unsigned i)
{
	switch (i)
	{
	case CSMFailures_Liftoff_Circuit_A_Failure:
		if (sat->GetIU()) sat->GetIU()->GetEDS()->SetLiftoffCircuitAFailure(true);
		break;
	case CSMFailures_Liftoff_Circuit_B_Failure:
		if (sat->GetIU()) sat->GetIU()->GetEDS()->SetLiftoffCircuitBFailure(true);
		break;
	case CSMFailures_Tower_Jett1_Fail:
		sat->TowerJett1Switch.SetFailed(true);
		break;
	case CSMFailures_Tower_Jett2_Fail:
		sat->TowerJett2Switch.SetFailed(true);
		break;
	case CSMFailures_SM_Jett1_Fail:
		sat->CmSmSep1Switch.SetFailed(true);
		break;
	case CSMFailures_SM_Jett2_Fail:
		sat->CmSmSep2Switch.SetFailed(true);
		break;
	case CSMFailures_SI_Engine_1_Failure:
	case CSMFailures_SI_Engine_2_Failure:
	case CSMFailures_SI_Engine_3_Failure:
	case CSMFailures_SI_Engine_4_Failure:
	case CSMFailures_SI_Engine_5_Failure:
	case CSMFailures_SI_Engine_6_Failure:
	case CSMFailures_SI_Engine_7_Failure:
	case CSMFailures_SI_Engine_8_Failure:
	case CSMFailures_SII_Engine_1_Failure:
	case CSMFailures_SII_Engine_2_Failure:
	case CSMFailures_SII_Engine_3_Failure:
	case CSMFailures_SII_Engine_4_Failure:
	case CSMFailures_SII_Engine_5_Failure:
	case CSMFailures_SIVB_Engine_Failure:
	case CSMFailures_SIVB_O2_H2_Burner_Failure:
		sat->SetFailure(i, true);
		break;
	case CSMFailures_IU_Platform_Failure:
		if(sat->GetIU()) sat->GetIU()->GetLVIMU()->SetFailed();
		break;
	case CSMFailures_Fuel_Cell_1_Disconnect:
		sat->MainBusAController.ConnectFuelCell(1, false);
		sat->MainBusBController.ConnectFuelCell(1, false);
		break;
	case CSMFailures_Fuel_Cell_2_Disconnect:
		sat->MainBusAController.ConnectFuelCell(2, false);
		sat->MainBusBController.ConnectFuelCell(2, false);
		break;
	case CSMFailures_Fuel_Cell_3_Disconnect:
		sat->MainBusAController.ConnectFuelCell(3, false);
		sat->MainBusBController.ConnectFuelCell(3, false);
		break;
	default:
		if (i >= CSMFailures_CWS_Light_Failure && i < CSMFailures_CWS_Light_Failure + 60)
		{
			sat->cws.FailLight(i - CSMFailures_CWS_Light_Failure, true);
		}
		break;
	}
}

void CSMMalfunctionSimulation::ResetFailure(unsigned i)
{
	switch (i)
	{
	case CSMFailures_Liftoff_Circuit_A_Failure:
		if (sat->GetIU()) sat->GetIU()->GetEDS()->SetLiftoffCircuitAFailure(false);
		break;
	case CSMFailures_Liftoff_Circuit_B_Failure:
		if (sat->GetIU()) sat->GetIU()->GetEDS()->SetLiftoffCircuitBFailure(false);
		break;
	case CSMFailures_Tower_Jett1_Fail:
		sat->TowerJett1Switch.SetFailed(false);
		break;
	case CSMFailures_Tower_Jett2_Fail:
		sat->TowerJett2Switch.SetFailed(false);
		break;
	case CSMFailures_SM_Jett1_Fail:
		sat->CmSmSep1Switch.SetFailed(false);
		break;
	case CSMFailures_SM_Jett2_Fail:
		sat->CmSmSep2Switch.SetFailed(false);
		break;
	case CSMFailures_SIVB_O2_H2_Burner_Failure:
		sat->SetFailure(i, false);
		break;
	default:
		if (i >= CSMFailures_CWS_Light_Failure && i < CSMFailures_CWS_Light_Failure + 60)
		{
			sat->cws.FailLight(i - CSMFailures_CWS_Light_Failure, false);
		}
		break;
	}
}

void CSMMalfunctionSimulation::SetSwitchFailure(unsigned i, bool set)
{
	if (i >= switchmalfunctions.size()) return;

	sat->MainPanel.SetFailedState(switchmalfunctions[i]->GetSwitchName().c_str(), set, switchmalfunctions[i]->GetFailState());
}

void CSMMalfunctionSimulation::SetRandomFailures(double FailureMultiplier)
{
	double temptime;

	RandomizedFailure(CSMFailures_LET_AutoJet_Fail, 127.0 / FailureMultiplier);
	RandomizedFailure(CSMFailures_LES_Jet_Motor_Fail, 255.0 / FailureMultiplier);
	RandomizedFailure(CSMFailures_Liftoff_Circuit_A_Failure, 255.0 / FailureMultiplier);
	RandomizedFailure(CSMFailures_Liftoff_Circuit_B_Failure, 255.0 / FailureMultiplier);
	RandomizedFailure(CSMFailures_Auto_Abort_Enable_Fail, 255.0 / FailureMultiplier);
	RandomizedFailure(CSMFailures_Tower_Jett1_Fail, 127.0 / FailureMultiplier);
	RandomizedFailure(CSMFailures_Tower_Jett2_Fail, 127.0 / FailureMultiplier);
	RandomizedFailure(CSMFailures_SM_Jett1_Fail, 127.0 / FailureMultiplier);
	RandomizedFailure(CSMFailures_SM_Jett2_Fail, 127.0 / FailureMultiplier);
	RandomizedFailure(CSMFailures_Apex_Cover_Fail, 127.0 / FailureMultiplier);
	RandomizedFailure(CSMFailures_Drogue_Chute_Fail, 127.0 / FailureMultiplier);
	RandomizedFailure(CSMFailures_Main_Chute_Fail, 127.0 / FailureMultiplier);

	if (sat->GetStage() < CSM_LEM_STAGE)
	{
		if (!(rand() & (int)(127.0 / FailureMultiplier)))
		{
			temptime = 20.0 + ((double)(rand() & 1023) / 2.0);
			ArmFailure(CSMFailures_IU_Platform_Failure, 0, temptime);
		}
		else
		{
			ResetFailureArm(CSMFailures_IU_Platform_Failure);
		}
	}

	if (utils::IsVessel(sat, utils::SaturnIB))
	{
		//S-IB Engines
		if (sat->GetStage() < STAGE_ORBIT_SIVB)
		{
			for (unsigned i = 0; i < 8; i++)
			{
				if (!(rand() & (int)(127.0 / FailureMultiplier)))
				{
					temptime = 20.0 + ((double)(rand() & 1023) / 10.0);
					ArmFailure(CSMFailures_SI_Engine_1_Failure + i, 0, temptime);
				}
				else
				{
					ResetFailureArm(CSMFailures_SI_Engine_1_Failure + i);
				}
			}
		}
	}
	else
	{
		//S-IC Engines
		if (sat->GetStage() < LAUNCH_STAGE_TWO)
		{
			for (unsigned i = 0; i < 5; i++)
			{
				if (!(rand() & (int)(127.0 / FailureMultiplier)))
				{
					temptime = 20.0 + ((double)(rand() & 1023) / 10.0);
					ArmFailure(CSMFailures_SI_Engine_1_Failure + i, 0, temptime);
				}
				else
				{
					ResetFailureArm(CSMFailures_SI_Engine_1_Failure + i);
				}
			}

			RandomizedFailure(CSMFailures_SII_Auto_Sep_Fail, 127.0 / FailureMultiplier);
		}

		if (sat->GetStage() < STAGE_ORBIT_SIVB)
		{
			for (unsigned i = 0; i < 5; i++)
			{
				if (!(rand() & (int)(127.0 / FailureMultiplier)))
				{
					temptime = 20.0 + ((double)(rand() & 1023) / 10.0);
					ArmFailure(CSMFailures_SII_Engine_1_Failure + i, 2, temptime);
				}
				else
				{
					ResetFailureArm(CSMFailures_SII_Engine_1_Failure + i);
				}
			}
		}
	}

	//
// Random CWS light failures.
//
	for (int i = 0; i < 60; i++)
	{
		sat->cws.FailLight(i, false);
	}

	if (!(rand() & (int)(15.0 / FailureMultiplier)))
	{
		int i, n = (rand() & 7) + 1;

		for (i = 0; i < n; i++)
		{
			sat->cws.FailLight(rand() & 63, true);
		}
	}
}

int CSMMalfunctionSimulation::GetDamageModel()
{
	return sat->GetDamageModel();
}