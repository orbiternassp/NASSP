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
  *	Revision 1.4  2012/09/05 21:16:26  tschachim
  *	PGNS warning does NOT trigger master alarm
  *	
  *	Revision 1.3  2012/01/14 22:34:48  tschachim
  *	GN CWS lights
  *	
  *	Revision 1.2  2009/12/22 18:14:47  tschachim
  *	More bugfixes related to the prelaunch/launch checklists.
  *	
  *	Revision 1.1  2009/02/18 23:20:56  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.39  2008/04/11 12:19:00  tschachim
  *	New SM and CM RCS.
  *	Improved abort handling.
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.38  2007/10/18 00:23:18  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.37  2007/08/13 16:06:09  tschachim
  *	Moved bitmaps to subdirectory.
  *	New VAGC mission time pad load handling.
  *	New telescope and sextant panels.
  *	Fixed CSM/LV separation speed.
  *	
  *	Revision 1.36  2007/07/17 14:33:06  tschachim
  *	Added entry and post landing stuff.
  *	
  *	Revision 1.35  2007/06/06 15:02:11  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.34  2007/02/18 01:35:29  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.33  2007/01/14 13:02:42  dseagrav
  *	CM AC bus feed reworked. Inverter efficiency now varies, AC busses are 3-phase all the way to the inverter, inverter switching logic implemented to match the CM motor-switch lockouts. Original AC bus feeds deleted. Inverter overload detection enabled and correct.
  *	
  *	Revision 1.32  2007/01/06 04:44:48  dseagrav
  *	Corrected CREW ALARM command behavior, PCM downtelemetry generator now draws power
  *	
  *	Revision 1.31  2006/12/26 12:58:47  dseagrav
  *	CMC C/W lamp on restart and altered restart to compensate.
  *	
  *	Revision 1.30  2006/11/20 16:38:27  tschachim
  *	Bugfix CWS CM/SM separation.
  *	
  *	Revision 1.29  2006/11/13 14:47:30  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.28  2006/10/26 18:48:50  movieman523
  *	Fixed up CM RCS 1 and 2 warning lights to make the 'C&WS Operational Check' work.
  *	
  *	Revision 1.27  2006/09/04 11:37:53  tschachim
  *	Fixed CMC C/W light in case of test alarm.
  *	
  *	Revision 1.26  2006/06/10 23:27:41  movieman523
  *	Updated abort code.
  *	
  *	Revision 1.25  2006/05/30 14:40:21  tschachim
  *	Fixed fuel cell - dc bus connectivity, added battery charger
  *	
  *	Revision 1.24  2006/05/19 11:05:43  tschachim
  *	Bugfix infinite power consumption.
  *	
  *	Revision 1.23  2006/02/28 20:40:32  quetalsi
  *	Bugfix and added CWS FC BUS DISCONNECT. Reset DC switches now work.
  *	
  *	Revision 1.22  2006/02/23 22:46:41  quetalsi
  *	Added AC ovevoltage control and Bugfix
  *	
  *	Revision 1.21  2006/02/22 20:14:45  quetalsi
  *	C&W  AC_BUS1/2 light and AC RESET SWITCH now woks.
  *	
  *	Revision 1.20  2006/02/18 21:38:55  tschachim
  *	Bugfix
  *	
  *	Revision 1.19  2006/02/13 21:29:00  tschachim
  *	ISS light.
  *	
  *	Revision 1.18  2006/01/20 19:13:02  movieman523
  *	Added info on RCS warning lights.
  *	
  *	Revision 1.17  2006/01/14 20:58:15  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
  *	Revision 1.16  2006/01/07 01:34:08  movieman523
  *	Added AC bus overvoltage and main bus undervolt lights.
  *	
  *	Revision 1.15  2005/12/19 16:48:50  tschachim
  *	Bugfix, Removed "dirty hacks".
  *	
  *	Revision 1.14  2005/11/18 02:40:55  movieman523
  *	Major revamp of PanelSDK electrical code, and various modifications to run off fuel cells.
  *	
  *	Revision 1.13  2005/11/16 23:50:31  movieman523
  *	More updates to CWS operation. Still not completely correct, but closer.
  *	
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
  *	Todo better solution.
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

	if (aws.ISSWarning) {
		SetLight(CSM_CWS_ISS_LIGHT, true);
		// No Master Alarm during lamp test
		if (aws.TestAlarms) 
			SetMasterAlarm(false);
	} 
	else {
		SetLight(CSM_CWS_ISS_LIGHT, false);
	}

	//
	// CMC warning
	//
    // See http://www.ibiblio.org/apollo/Documents/BwCsmHandbookGncSection_112-114.pdf
	// Drawing 8.1, square L4 and others.
	//

	if (aws.TestAlarms) {
		SetLight(CSM_CWS_CMC_LIGHT, true);
		// No Master Alarm during lamp test
		SetMasterAlarm(false);
	} else {
		if (aws.CMCWarning) {
			SetLight(CSM_CWS_CMC_LIGHT, true); // The real thing
		} else {
			SetLight(CSM_CWS_CMC_LIGHT, false);
		}
	}

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

