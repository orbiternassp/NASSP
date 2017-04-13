/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: CSM caution and warning system code.

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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "soundlib.h"

#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"

#include "saturn.h"
#include "papi.h"

CSMCautionWarningSystem::CSMCautionWarningSystem(Sound &mastersound, Sound &buttonsound, PanelSDK &p) : 
	CautionWarningSystem(mastersound, buttonsound, p)

{
	NextUpdateTime = MINUS_INFINITY;

	NextO2FlowCheckTime = MINUS_INFINITY;
	LastO2FlowCheckHigh = false;
	SPSPressCheckCount = 0;
	CryoPressCheckCount = 0;
	GlycolTempCheckCount = 0;
	for (int i = 0; i < 4; i++)
		FuelCellCheckCount[i] = 0;

	TimeStepCount = 0;

	ACBus1Alarm = false;
	ACBus2Alarm = false;
	ACBus1Reset = false;
	ACBus2Reset = false;

	GNLampState = 1;	// on
	GNPGNSAlarm = false;
}

//
// Check status of a fuel cell.
//

bool CSMCautionWarningSystem::FuelCellBad(FuelCellStatus &fc, int index)

{
	bool bad = false;
	
	//
	// Various conditions, see Apollo Operations Handbook 2.10.4.2
	//

	if (fc.H2FlowLBH > 0.161) bad = true;
	if (fc.O2FlowLBH > 1.276) bad = true;

	// pH > 9 not simulated at the moment

	if (fc.TempF < 360.0) bad = true;
	if (fc.TempF > 475.0) bad = true;

	if (fc.CondenserTempF < 150.0) bad = true;
	if (fc.CondenserTempF > 175.0) bad = true;

	if (fc.CoolingTempF < -30.0) bad = true;

	//
	// To avoid spurious alarms because of fluctuation at high time accelerations
	// the "bad" condition has to last for a few check counts.
	// This is similar to the shutdown handling in FCell.refresh
	//
	if (bad) {
		if (FuelCellCheckCount[index] < 10)
			FuelCellCheckCount[index]++;
		else
			return true;
	} else {
		FuelCellCheckCount[index] = 0;
	}
	return false;
}

//
// Check status of a SM RCS quad.
//

bool CSMCautionWarningSystem::SMRCSBad(SMRCSPropellantSource* smrcs)

{
	bool bad = false;
	
	//
	// Various conditions, see Apollo Operations Handbook 2.10.4.2
	//

	if (smrcs->GetPackageTempF() < 75) bad = true;
	if (smrcs->GetPackageTempF() > 205) bad = true;

	if (smrcs->GetPropellantPressurePSI() < 145) bad = true;
	if (smrcs->GetPropellantPressurePSI() > 215) bad = true;
	return bad;
}

//
// Check status of the AC inverter overload flags.
//

bool CSMCautionWarningSystem::ACOverloaded(int bus)

{
	Saturn *sat = (Saturn *) OurVessel;

	switch(bus){
		case 1:
			// sprintf(oapiDebugString(),"AC1 AMPS = %f",sat->ACBus1PhaseA.Current()+sat->ACBus1PhaseB.Current()+sat->ACBus1PhaseC.Current());
			if(sat->AcBus1Switch1.GetState() == TOGGLESWITCH_UP && sat->Inverter1->Overloaded()){
				return true;
			}
			if(sat->AcBus1Switch2.GetState() == TOGGLESWITCH_UP && sat->Inverter2->Overloaded()){
				return true;
			}
			if(sat->AcBus1Switch3.GetState() == TOGGLESWITCH_UP && sat->Inverter3->Overloaded()){
				return true;
			}
			return false;
		case 2:
			if(sat->AcBus2Switch1.GetState() == TOGGLESWITCH_UP && sat->Inverter1->Overloaded()){
				return true;
			}
			if(sat->AcBus2Switch2.GetState() == TOGGLESWITCH_UP && sat->Inverter2->Overloaded()){
				return true;
			}
			if(sat->AcBus2Switch3.GetState() == TOGGLESWITCH_UP && sat->Inverter3->Overloaded()){
				return true;
			}
			return false;
	}
	return false;
}


bool CSMCautionWarningSystem::ACUndervoltage(ACBusStatus &as)

{
	if (as.Phase1Voltage < 96 || as.Phase2Voltage < 96 || as.Phase3Voltage < 96)
		return true;

	return false;

}


bool CSMCautionWarningSystem::ACOvervoltage(ACBusStatus &as)

{
	if (as.Phase1Voltage > 130 || as.Phase2Voltage > 130 || as.Phase3Voltage > 130)
		return true;

	return false;

}

//
// We'll only check the internal systems five times a second (x time acceleration). That should cut the overhead to
// a minimum.
//

void CSMCautionWarningSystem::TimeStep(double simt)

{
	CautionWarningSystem::TimeStep(simt);

	//
	// Skip the first few timesteps and simulation time seconds to give the internal systems time to
	// settle down after a scenario load. This should avoid spurious
	// warnings.
	//

	if (TimeStepCount < 5) {
		if (TimeStepCount == 0) NextUpdateTime = simt + 0.1;
		TimeStepCount++;
		return;
	}

	//
	// Check systems.
	//
	Saturn *sat = (Saturn *) OurVessel;

	//
	// If we don't have power to the CWS, light the CWS light and return.
	//
	if (!IsPowered()) {

		// Turn off all lights
		SetLightStates(LeftLights, 0);
		SetLightStates(RightLights, 0);

		// Turn on CWS light
		SetLight(CSM_CWS_CWS_POWER, true);
		return;
	}

	SetLight(CSM_CWS_CWS_POWER, false);

	//
	// Check AGC warnings
	//

	AGCWarningStatus aws;
	sat->GetAGCWarningStatus(aws);

	//
	// ISS warning
	//

	if (aws.ISSWarning)
		SetLight(CSM_CWS_ISS_LIGHT, true);
	else
		SetLight(CSM_CWS_ISS_LIGHT, false);

	//
	// CMC warning
	//
    // See http://www.ibiblio.org/apollo/Documents/BwCsmHandbookGncSection_112-114.pdf
	// Drawing 8.1, square L4 and others.
	//

	if (aws.CMCWarning || aws.TestAlarms) { // TestAlarms must be removed after yaAGC implements control over CMC light!
		SetLight(CSM_CWS_CMC_LIGHT, true);
	}
	else
		SetLight(CSM_CWS_CMC_LIGHT, false);

	//
	// PGNS warning, does NOT trigger master alarm 
	// CSM Systems HandbooK (NTRS 19730060780) pg. 278 
	//

	GNPGNSAlarm = aws.PGNSWarning;

	//
	// CREW ALERT
	//

	if ((UplinkTestState & 010) != 0) {
		SetLight(CSM_CWS_CREW_ALERT,true);		
	} else {
		SetLight(CSM_CWS_CREW_ALERT,false);
	}

	//
	// Do some checks not every timestep
	//
	if (simt > NextUpdateTime) {
		//
		// Some systems only apply when we're in CSM mode.
		//

		if (Source == CWS_SOURCE_CSM) {

			//
			// Check fuel cells
			//
			FuelCellStatus fc1, fc2, fc3;

			sat->GetFuelCellStatus(1, fc1);
			sat->GetFuelCellStatus(2, fc2);
			sat->GetFuelCellStatus(3, fc3);

			SetLight(CSM_CWS_FC1_LIGHT, FuelCellBad(fc1, 1));
			SetLight(CSM_CWS_FC2_LIGHT, FuelCellBad(fc2, 2));
			SetLight(CSM_CWS_FC3_LIGHT, FuelCellBad(fc3, 3));

			//
			// LOX/LH2: "The caution and warning system will activate on alarm when oxygen pressure 
			// in either tank exceeds 950 psia or falls below 800 psia or when the hydrogen system 
			// pressure exceeds 270 psia or drops below 220 psia."
			//

			bool LightCryo = false;
			TankPressures press;
			sat->GetTankPressures(press);

			if (press.H2Tank1PressurePSI < 220.0 || press.H2Tank2PressurePSI < 220.0) {
				LightCryo = true;
			}
			else if (press.H2Tank1PressurePSI > 270.0 || press.H2Tank2PressurePSI > 270.0) {
				LightCryo = true;
			}
			else if (press.O2Tank1PressurePSI < 800.0 || press.O2Tank2PressurePSI < 800.0) {
				LightCryo = true;
			}
			else if (press.O2Tank1PressurePSI > 950.0 || press.O2Tank2PressurePSI > 950.0) {
				LightCryo = true;
			}

			if (LightCryo) {
				if (CryoPressCheckCount > 20) {			
					SetLight(CSM_CWS_CRYO_PRESS_LIGHT, true);
				} else {
					CryoPressCheckCount++;
				}
			} else {
				SetLight(CSM_CWS_CRYO_PRESS_LIGHT, false);
				CryoPressCheckCount = 0;
			}

			//
			// SPS PRESS
			// Fuel and oxidizer have the same pressure for now.
			// See AOH C+W
			//

			double spsPress = sat->GetSPSPropellant()->GetPropellantPressurePSI();
			if (spsPress < 157.0 || spsPress > 200.0) {
				if (SPSPressCheckCount > 10) {			
					SetLight(CSM_CWS_SPS_PRESS, true);
				} else {
					SPSPressCheckCount++;
				}
			} else {
				SetLight(CSM_CWS_SPS_PRESS, false);
				SPSPressCheckCount = 0;
			}

			//
			// SM RCS
			//

			SetLight(CSM_CWS_SM_RCS_A, SMRCSBad(&sat->SMQuadARCS));
			SetLight(CSM_CWS_SM_RCS_B, SMRCSBad(&sat->SMQuadBRCS));
			SetLight(CSM_CWS_SM_RCS_C, SMRCSBad(&sat->SMQuadCRCS));
			SetLight(CSM_CWS_SM_RCS_D, SMRCSBad(&sat->SMQuadDRCS));
		}
		else {
			SetLight(CSM_CWS_FC1_LIGHT, false);
			SetLight(CSM_CWS_FC2_LIGHT, false);
			SetLight(CSM_CWS_FC3_LIGHT, false);

			SetLight(CSM_CWS_SM_RCS_A, false);
			SetLight(CSM_CWS_SM_RCS_B, false);
			SetLight(CSM_CWS_SM_RCS_C, false);
			SetLight(CSM_CWS_SM_RCS_D, false);

			SetLight(CSM_CWS_CRYO_PRESS_LIGHT, false);
			SetLight(CSM_CWS_SPS_PRESS, false);
		}

		//BMAG1
		if (sat->bmag1.GetTempF() > 172.0 || sat->bmag1.GetTempF() < 168.0)
		{
			SetLight(CSM_CWS_BMAG_1_TEMP, true);
		}
		else
		{
			SetLight(CSM_CWS_BMAG_1_TEMP, false);
		}

		//BMAG2
		if (sat->bmag2.GetTempF() > 172.0 || sat->bmag2.GetTempF() < 168.0)
		{
			SetLight(CSM_CWS_BMAG_2_TEMP, true);
		}
		else
		{
			SetLight(CSM_CWS_BMAG_2_TEMP, false);
		}

		AtmosStatus atm;
		DisplayedAtmosStatus datm;
		sat->GetAtmosStatus(atm);
		sat->GetDisplayedAtmosStatus(datm);

		//
		// Glycol temperature of the EcsRadTempPrimOutletMeter lower than -30°F
		// Use displayed value instead of the PanelSDK to make use of the "damping" 
		// of the SuitComprDeltaPMeter to pervent alarms because of the fluctuations during 
		// high time acceleration.
		//
		
		if (datm.DisplayedEcsRadTempPrimOutletMeterTemperatureF < -30.0) {
			if (GlycolTempCheckCount > 20) {			
				SetLight(CSM_CWS_GLYCOL_TEMP_LOW, true);
			} else {
				GlycolTempCheckCount++;
			}
		} else {
			SetLight(CSM_CWS_GLYCOL_TEMP_LOW, false);
			GlycolTempCheckCount = 0;
		}

		//
		// Inverter: "A temperature sensor with a range of 32 degrees to 248 degrees F is installed 
		// in each inverter and will illuminate a light in the caution and warning system at an 
		// inverter overtemperature of 190 degrees F"
		//

		//
		// Power Bus: "If voltage drops below 26.25 volts dc, the applicable dc undervoltage light 
		// on the caution and warning panel will illuminate."
		//

		MainBusStatus ms;
		sat->GetMainBusStatus(ms);

		if (ms.Enabled_DC_A_CWS) {
			if (!ms.Reset_DC_A_CWS) {
				if (ms.MainBusAVoltage < 26.25) {
					SetLight(CSM_CWS_BUS_A_UNDERVOLT, true); 
				}
			} else {
				SetLight(CSM_CWS_BUS_A_UNDERVOLT, false);
			}
		}
		
		if (ms.Enabled_DC_B_CWS) {
			if (!ms.Reset_DC_B_CWS) {
				if (ms.MainBusBVoltage < 26.25) {
					SetLight(CSM_CWS_BUS_B_UNDERVOLT, true); 
				}
			} else { 
				SetLight(CSM_CWS_BUS_B_UNDERVOLT, false);
			}
		}
		

		SetLight(CSM_CWS_FC_BUS_DISCONNECT, ms.Fc_Disconnected && (Source == CWS_SOURCE_CSM));

		//
		// AC bus: lights come on to indicate under or overvoltage in the AC bus.
		//
		// AC bus overload: overload light will come on if total output exceeds 250% of rated current, or a
		// single phase output exceeds 300% of rated current, which is 27.7 and 11 amps respectively.
		// Since the flag has to be set for at least 5 seconds, I want the inverter to set this.
		//
		// AC bus undervoltage: AC_BUS1/2 light will come on if voltage on any phase is
		// < 96 volts and reset switch is center. Lights only come off if reset switch is reset (up).
		//
		// AC bus overvoltage: AC_BUS1/2 light will come on and disconnected the inverter from the bus
		// if voltage on any phase is > 130 volts and reset switch is reset (up).

		ACBusStatus as;

		sat->GetACBusStatus(as, 1);
		if (as.Enabled_AC_CWS) {
			if (!as.Reset_AC_CWS) {
				if (ACOverloaded(1)) {
					SetLight(CSM_CWS_AC_BUS1_OVERLOAD, true);
					SetLight(CSM_CWS_AC_BUS1_LIGHT, true); 
				}
				if (ACUndervoltage(as)) 
					SetLight(CSM_CWS_AC_BUS1_LIGHT, true);
				if (ACOvervoltage(as)) {
					SetLight(CSM_CWS_AC_BUS1_LIGHT, true);
					sat->DisconnectInverter(true,1); 
				}

				//
				// I'm not really sure if this is correct, I did it because of the master alarm
				// during the standby inverter check during prelaunch, see AOH part 2 or standard 
				// mode checklists for more informations
				//

				if (ACBus1Reset) {
					ACBus1Reset = false;
					SetMasterAlarm(true);
				}
			}
			else {
				SetLight(CSM_CWS_AC_BUS1_OVERLOAD, false);
				SetLight(CSM_CWS_AC_BUS1_LIGHT, false);
				sat->DisconnectInverter(false, 1);

				if (ACBus1Alarm) {
					ACBus1Alarm = false;
					ACBus1Reset = true;
				}
			}
		} else {
			if (ACOverloaded(1) || ACUndervoltage(as) || ACOvervoltage(as)) {
				ACBus1Alarm = true;
			}
		}
		
		sat->GetACBusStatus(as, 2);
		if (as.Enabled_AC_CWS) {
			if (!as.Reset_AC_CWS) {
				if (ACOverloaded(2)) {
					SetLight(CSM_CWS_AC_BUS2_OVERLOAD, true);
					SetLight(CSM_CWS_AC_BUS2_LIGHT, true); 
				}
				if (ACUndervoltage(as)) 
					SetLight(CSM_CWS_AC_BUS2_LIGHT, true);
				if (ACOvervoltage(as)) {
					SetLight(CSM_CWS_AC_BUS2_LIGHT, true);
					sat->DisconnectInverter(true,2); 
				}

				//
				// See AC bus 1
				//

				if (ACBus2Reset) {
					ACBus2Reset = false;
					SetMasterAlarm(true);
				}
			}
			else {
				SetLight(CSM_CWS_AC_BUS2_OVERLOAD, false);
				SetLight(CSM_CWS_AC_BUS2_LIGHT, false);
				sat->DisconnectInverter(false,2);

				if (ACBus2Alarm) {
					ACBus2Alarm = false;
					ACBus2Reset = true;
				}
			}
		} else {
			if (ACOverloaded(2) || ACUndervoltage(as) || ACOvervoltage(as)) {
				ACBus2Alarm = true;
			}
		}

		//
		// Oxygen flow: "Flow rates of 1 pound per hour or more with a duration in excess of 16.5 
		// seconds will illuminate a light on the caution and warning panel to alert the crew to 
		// the fact that the oxygen flow rate is greater than is normally required."
		//

		//
		// Use displyed value instead of the PanelSDK to make use of the "damping" 
		// of the O2 flow meter to pervent alarms because of the fluctuations during 
		// high time acceleration.
		//
		double cf = datm.DisplayedO2FlowLBH;
		bool LightO2Warning = false;

		if (cf > 1.0) {
			if (LastO2FlowCheckHigh) {
				if (simt > NextO2FlowCheckTime) {
					LightO2Warning = true;
				}
			} else {
				LastO2FlowCheckHigh = true;
				NextO2FlowCheckTime = simt + 16.5;
			}
		} else {
			LastO2FlowCheckHigh = false;
		}
		SetLight(CSM_CWS_O2_FLOW_HIGH_LIGHT, LightO2Warning);

		//
		// CO2: "A carbon dioxide sensor is connected between the suit inlet and return manifold. It 
		// is connected to an indicator on the main display console, to telemetry, and to the caution 
		// and warning system and will activate a warning if the carbon dioxide partial pressure 
		// reaches 7.6 millimeters of mercury."
		//

		SetLight(CSM_CWS_CO2_LIGHT, (atm.SuitCO2MMHG >= 7.6));

		//
		// Suit compressor delta pressure below 0.22 psi
		// Use displayed value instead of the PanelSDK to make use of the "damping" 
		// of the SuitComprDeltaPMeter to pervent alarms because of the fluctuations during 
		// high time acceleration. 
		//
		
		SetLight(CSM_CWS_SUIT_COMPRESSOR, (datm.DisplayedSuitComprDeltaPressurePSI < 0.22));

		//
		// CM RCS warning lights if pressure is below 260psi or above 330psi (AOH RCS 2.5-46),
		// however, AOH 2-10.6 says that the CM RCS lights are only active in CM mode.
		//

		if (Source == CWS_SOURCE_CM)
		{
			SetLight(CSM_CWS_CM_RCS_1, !(sat->CMRCS1.GetPropellantPressurePSI() <= 330.0 && sat->CMRCS1.GetPropellantPressurePSI() >= 260.0));
			SetLight(CSM_CWS_CM_RCS_2, !(sat->CMRCS2.GetPropellantPressurePSI() <= 330.0 && sat->CMRCS2.GetPropellantPressurePSI() >= 260.0));
		}
		else
		{
			SetLight(CSM_CWS_CM_RCS_1, false);
			SetLight(CSM_CWS_CM_RCS_2, false);
		}
		NextUpdateTime = simt + 0.1;
	}
}

void CSMCautionWarningSystem::RenderLights(SURFHANDLE surf, SURFHANDLE lightsurf, bool leftpanel)

{
	if (leftpanel) {
		RenderLightPanel(surf, lightsurf, LeftLights, TestState == CWS_TEST_LIGHTS_LEFT, 6, 122, 0);
	}
	else {
		RenderLightPanel(surf, lightsurf, RightLights, TestState == CWS_TEST_LIGHTS_RIGHT, 261, 122, CWS_LIGHTS_PER_PANEL);
	}
}

void CSMCautionWarningSystem::RenderGNLights(SURFHANDLE surf, SURFHANDLE lightsurf)

{
	if (!LightsPowered() || GNLampState == 0)
		return;

	// PGNS
	if (GNLampState == 2 || GNPGNSAlarm) {
		oapiBlt(surf, lightsurf, 0, 0, 54, 2, 49, 21);
	}
	// CMC
	if (GNLampState == 2 || RightLights[CSM_CWS_CMC_LIGHT - CWS_LIGHTS_PER_PANEL]) {
		oapiBlt(surf, lightsurf, 0, 25, 54, 27, 49, 21);
	}
	// ISS
	if (GNLampState == 2 || RightLights[CSM_CWS_ISS_LIGHT - CWS_LIGHTS_PER_PANEL]) {
		oapiBlt(surf, lightsurf, 0, 50, 54, 52, 49, 21);
	}
}

void CSMCautionWarningSystem::GNLampSwitchToggled(PanelSwitchItem *s) {
	if (s->GetState() == THREEPOSSWITCH_CENTER) {
		GNLampState = 0;	// off
	} else if (s->GetState() == THREEPOSSWITCH_UP) {
		GNLampState = 1;	// on
	} else if (s->GetState() == THREEPOSSWITCH_DOWN) {
		GNLampState = 2;	// test
	}
}

bool CSMCautionWarningSystem::LightPowered(int i)

{
	if (!LightsPowered())
		return false;

	if (Source == CWS_SOURCE_CSM)
		return true;

	//
	// Disable SM lights when source is set to CM.
	//

	switch (i) {
	case CSM_CWS_PITCH_GIMBAL1:
	case CSM_CWS_PITCH_GIMBAL2:
	case CSM_CWS_YAW_GIMBAL1:
	case CSM_CWS_YAW_GIMBAL2:
	case CSM_CWS_FC1_LIGHT:
	case CSM_CWS_FC2_LIGHT:
	case CSM_CWS_FC3_LIGHT:
	case CSM_CWS_SM_RCS_A:
	case CSM_CWS_SM_RCS_B:
	case CSM_CWS_SM_RCS_C:
	case CSM_CWS_SM_RCS_D:
	case CSM_CWS_SPS_PRESS:
	case CSM_CWS_CRYO_PRESS_LIGHT:
	case CSM_CWS_GLYCOL_TEMP_LOW:
	case CSM_CWS_FC_BUS_DISCONNECT:
		return false;
	}

	return true;
}

void CSMCautionWarningSystem::RenderLightPanel(SURFHANDLE surf, SURFHANDLE lightsurf, bool *LightState, bool LightTest, int sdx, int sdy, int base)

{
	int i = 0;
	int row, column;

	if (!LightsPowered())
		return;

	for (row = 0; row < 6; row++) {
		for (column = 0; column < 4; column++) {
			if (LightTest || (LightState[i] && (Mode != CWS_MODE_ACK || MasterAlarmPressed))) {
				if (!IsFailed(i + base) && LightPowered(i + base)) {
					oapiBlt(surf, lightsurf, column * 53, row * 18, column * 53 + sdx, row * 18 + sdy, 50, 16);
				}
			}
			i++;
		}
	}
}

void CSMCautionWarningSystem::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, CWS_START_STRING);

	papiWriteScenario_bool(scn, "LASTO2FLOWCHECKHIGH", LastO2FlowCheckHigh);
	papiWriteScenario_double(scn, "NEXTO2FLOWCHECKTIME", NextO2FlowCheckTime);
	oapiWriteScenario_int(scn, "GNLAMPSTATE", GNLampState);
	papiWriteScenario_bool(scn, "GNPGNSALARM", GNPGNSAlarm);

	CautionWarningSystem::SaveState(scn);
	oapiWriteLine(scn, CWS_END_STRING);
}

void CSMCautionWarningSystem::LoadState(FILEHANDLE scn)

{
	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, CWS_END_STRING, sizeof(CWS_END_STRING)))
			return;

		papiReadScenario_bool(line, "LASTO2FLOWCHECKHIGH", LastO2FlowCheckHigh);
		papiReadScenario_double(line, "NEXTO2FLOWCHECKTIME", NextO2FlowCheckTime);
		papiReadScenario_int(line, "GNLAMPSTATE", GNLampState);
		papiReadScenario_bool(line, "GNPGNSALARM", GNPGNSAlarm);

		CautionWarningSystem::LoadState(line);
	}
}

