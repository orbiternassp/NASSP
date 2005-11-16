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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.12  2005/11/16 23:14:01  movieman523
  *	Initial support for wiring in the circuit breakers.
  *	
  *	Revision 1.11  2005/11/16 00:18:49  movieman523
  *	Added beginnings of really basic IU emulation. Added random failures of caution and warning lights on non-historical missions. Added initial support for Skylab CM and SM. Added LEM Name option in scenario file.
  *	
  *	Revision 1.10  2005/10/19 11:25:14  tschachim
  *	Bugfixes for high time accelerations.
  *	
  *	Revision 1.9  2005/10/12 19:41:08  tschachim
  *	Reduced suit compressor alarm pressure with higher time accelerations,
  *	TODO better solution.
  *	
  *	Revision 1.8  2005/10/11 16:31:50  tschachim
  *	Added more alarms.
  *	
  *	Revision 1.7  2005/09/30 11:21:28  tschachim
  *	Changed fuel cell and O2 flow handling.
  *	
  *	Revision 1.6  2005/08/21 16:23:32  movieman523
  *	Added more alarms.
  *	
  *	Revision 1.5  2005/08/21 13:13:43  movieman523
  *	Wired in a few caution and warning lights.
  *	
  *	Revision 1.4  2005/08/21 11:51:59  movieman523
  *	Initial version of CSM caution and warning lights: light test switch now works.
  *	
  *	Revision 1.3  2005/08/13 23:48:57  movieman523
  *	Added some documentation on caution and warning checks that I found on the web.
  *	
  *	Revision 1.2  2005/08/13 16:41:15  movieman523
  *	Fully wired up the CSM caution and warning switches.
  *	
  *	Revision 1.1  2005/08/13 14:59:24  movieman523
  *	Added initial null implementation of CSM caution and warning system, and removed 'master alarm' flag from Saturn class.
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"

#include "saturn.h"

CSMCautionWarningSystem::CSMCautionWarningSystem(Sound &mastersound, Sound &buttonsound) : CautionWarningSystem(mastersound, buttonsound)

{
	NextUpdateTime = MINUS_INFINITY;

	NextO2FlowCheckTime = MINUS_INFINITY;
	LastO2FlowCheckHigh = false;
	O2FlowCheckCount = 0;
	for (int i = 0; i < 4; i++)
		FuelCellCheckCount[i] = 0;

	TimeStepCount = 0;
}

//
// Check status of a fuel cell.
//

bool CSMCautionWarningSystem::FuelCellBad(FuelCellStatus fc, int index)

{
	bool bad = false;
	
	//
	// Various conditions, see Apollo Operations Handbook 2.10.4.2
	//

	if (fc.H2FlowLBH > 0.161) bad = true;
	if (fc.O2FlowLBH > 1.276) bad = true;

	// pH > 9 not simulated at the moment

	// The alarm conditions gets reduced with a time acceleration
	// factor, this "dirty hack" should be removed if we find a better solution	
	if (fc.TempF < 360.0 - 0.02 * oapiGetTimeAcceleration()) bad = true;
	if (fc.TempF > 475.0 + 0.02 * oapiGetTimeAcceleration()) bad = true;

	if (fc.CondenserTempF < 150.0 - 0.02 * oapiGetTimeAcceleration()) bad = true;
	if (fc.CondenserTempF > 175.0 + 0.02 * oapiGetTimeAcceleration()) bad = true;

	if (fc.CoolingTempF < -30.0 - 0.02 * oapiGetTimeAcceleration()) bad = true;

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

	if (TimeStepCount < 100) {
		if (TimeStepCount == 0) NextUpdateTime = simt + 20.0;
		TimeStepCount++;
		return;
	}

	if (simt > NextUpdateTime) {

		Saturn *sat = (Saturn *) OurVessel;

		//
		// Check systems.
		//

		//
		// If we don't have power to the CWS, light the CWS light and return.
		//

		if (!IsPowered()) {
			SetLight(CSM_CWS_CWS_POWER, true);
			return;
		}

		SetLight(CSM_CWS_CWS_POWER, false);

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

			SetLight(CSM_CWS_CRYO_PRESS_LIGHT, LightCryo);
		}
		else {
			SetLight(CSM_CWS_FC1_LIGHT, false);
			SetLight(CSM_CWS_FC2_LIGHT, false);
			SetLight(CSM_CWS_FC3_LIGHT, false);
			SetLight(CSM_CWS_CRYO_PRESS_LIGHT, false);
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
		
		SetLight(CSM_CWS_GLYCOL_TEMP_LOW, (datm.DisplayedEcsRadTempPrimOutletMeterTemperatureF < -30.0));

		//
		// Inverter: "A temperature sensor with a range of 32 degrees to 248 degrees F is installed 
		// in each inverter and will illuminate a light in the caution and warning system at an 
		// inverter overtemperature of 190 degrees F"
		//

		//
		// Power Bus: "If voltage drops below 26.25 volts dc, the applicable dc undervoltage light 
		// on the caution and warning panel will illuminate."
		//

		//
		// Oxygen flow: "Flow rates of 1 pound per hour or more with a duration in excess of 16.5 
		// seconds will illuminate a light on the caution and warning panel to alert the crew to 
		// the fact that the oxygen flow rate is greater than is normally required."
		//

		if (simt > NextO2FlowCheckTime) {
			//
			// Use displyed value instead of the PanelSDK to make use of the "damping" 
			// of the O2 flow meter to pervent alarms because of the fluctuations during 
			// high time acceleration.
			//
			double cf = datm.DisplayedO2FlowLBH;
			bool LightO2Warning = false;

			//
			// Skip the first few flow checks to give the panel SDK time to stabilise when
			// starting a scenario.
			//

			if (O2FlowCheckCount > 10) {
				if (cf > 1.0) {
					if (LastO2FlowCheckHigh) {
						LightO2Warning = true;
					}
					LastO2FlowCheckHigh = true;
				}
				else {
					LastO2FlowCheckHigh = false;
				}
			}
			else {
				O2FlowCheckCount++;
			}

			SetLight(CSM_CWS_O2_FLOW_HIGH_LIGHT, LightO2Warning);

			NextO2FlowCheckTime = simt + 16.5;
		}

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
		// high time acceleration. The alarm pressure gets reduced with a time acceleration
		// factor, this "dirty hack" should be removed if we find a better solution
		//
		
		double minPressure = 0.22 - 0.001 * oapiGetTimeAcceleration();
		SetLight(CSM_CWS_SUIT_COMPRESSOR, (datm.DisplayedSuitComprDeltaPressurePSI < minPressure));

		NextUpdateTime = simt + (0.2 * oapiGetTimeAcceleration());
	}
}

void CSMCautionWarningSystem::RenderLights(SURFHANDLE surf, SURFHANDLE lightsurf, bool leftpanel)

{
	if (leftpanel) {
		RenderLightPanel(surf, lightsurf, LeftLights, TestState == CWS_TEST_LIGHTS_LEFT, 6, 122, 0);
	}
	else {
		RenderLightPanel(surf, lightsurf, RightLights, TestState == CWS_TEST_LIGHTS_RIGHT, 261, 122, 30);
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
	case CSM_CWS_HIGAIN_LIMIT:
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
