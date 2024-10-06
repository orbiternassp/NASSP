/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

CSM Malfunction Simulation (Header)

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

#include "MalfunctionSimulation.h"

class Saturn;

#define CSMFailures_LET_AutoJet_Fail 0
#define CSMFailures_LES_Jet_Motor_Fail 1
#define CSMFailures_Liftoff_Circuit_A_Failure 2
#define CSMFailures_Liftoff_Circuit_B_Failure 3
#define CSMFailures_Auto_Abort_Enable_Fail 4
#define CSMFailures_Tower_Jett1_Fail 5
#define CSMFailures_Tower_Jett2_Fail 6
#define CSMFailures_SM_Jett1_Fail 7
#define CSMFailures_SM_Jett2_Fail 8
#define CSMFailures_Apex_Cover_Fail 9
#define CSMFailures_Drogue_Chute_Fail 10
#define CSMFailures_Main_Chute_Fail 11
#define CSMFailures_SI_Engine_1_Failure 12
#define CSMFailures_SI_Engine_2_Failure 13
#define CSMFailures_SI_Engine_3_Failure 14
#define CSMFailures_SI_Engine_4_Failure 15
#define CSMFailures_SI_Engine_5_Failure 16
#define CSMFailures_SI_Engine_6_Failure 17
#define CSMFailures_SI_Engine_7_Failure 18
#define CSMFailures_SI_Engine_8_Failure 19
#define CSMFailures_SII_Auto_Sep_Fail 20
#define CSMFailures_SII_Engine_1_Failure 21
#define CSMFailures_SII_Engine_2_Failure 22
#define CSMFailures_SII_Engine_3_Failure 23
#define CSMFailures_SII_Engine_4_Failure 24
#define CSMFailures_SII_Engine_5_Failure 25
#define CSMFailures_IU_Platform_Failure 26
#define CSMFailures_SIVB_Engine_Failure 27
#define CSMFailures_Fuel_Cell_1_Disconnect 28
#define CSMFailures_Fuel_Cell_2_Disconnect 29
#define CSMFailures_Fuel_Cell_3_Disconnect 30
#define CSMFailures_CSM_LV_Separation_Failure 31
#define CSMFailures_SIVB_O2_H2_Burner_Failure 32
#define CSMFailures_CWS_Light_Failure 33 //Plus 60

class CSMMalfunctionSimulation : public MalfunctionSimulation
{
public:
	CSMMalfunctionSimulation(Saturn *s);

	virtual void SetRandomFailures(double FailureMultiplier);

protected:
	double GetTimeReference(int i);
	void SetFailure(unsigned i);
	void ResetFailure(unsigned i);
	void SetSwitchFailure(unsigned i, bool set);

	int GetDamageModel(); //Check if failures are to be simulated

	Saturn *sat;
};