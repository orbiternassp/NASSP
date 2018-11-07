/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Signal-Conditioning Electronics Assembly

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

#include "Orbitersdk.h"
#include "soundlib.h"
#include "toggleswitch.h"
#include "lm_channels.h"
#include "LEM.h"
#include "lm_scea.h"

SCEA_SolidStateSwitch::SCEA_SolidStateSwitch()
{
	Reset();
}

void SCEA_SolidStateSwitch::Reset()
{
	isClosed = false;
}

void SCEA_SolidStateSwitch::SetState(bool closed)
{
	isClosed = closed;
}

SCEA_SA_5011::SCEA_SA_5011()
{
	Reset();
}

void SCEA_SA_5011::Reset()
{
	for (int i = 0;i < 4;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5022::SCEA_SA_5022()
{
	Reset();
}

void SCEA_SA_5022::Reset()
{
	for (int i = 0;i < 4;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5032::SCEA_SA_5032()
{
	Reset();
}

void SCEA_SA_5032::Reset()
{
	for (int i = 0;i < 3;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5041::SCEA_SA_5041()
{
	Reset();
}

void SCEA_SA_5041::Reset()
{
	for (int i = 0;i < 4;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5042::SCEA_SA_5042()
{
	Reset();
}

void SCEA_SA_5042::Reset()
{
	for (int i = 0;i < 10;i++)
	{
		Output[i] = 0.0;
	}
}

void SCEA_SA_5042::SetOutput(int n, bool val)
{
	Output[n - 1] = val ? 5.0 : 0.0;
}

SCEA_SA_5043::SCEA_SA_5043()
{
	Reset();
}

void SCEA_SA_5043::Reset()
{
	for (int i = 0;i < 12;i++)
	{
		Output[i] = 0.0;
	}
}

void SCEA_SA_5043::SetOutput(int n, bool val)
{
	Output[n - 1] = val ? 5.0 : 0.0;
}

SCEA_SA_5044::SCEA_SA_5044()
{
	Reset();
}

void SCEA_SA_5044::Reset()
{
	for (int i = 0;i < 12;i++)
	{
		Output[i] = 0.0;
		SolidStateSwitch[i].Reset();
	}
}

void SCEA_SA_5044::SetOutput(int n, bool val)
{
	Output[n - 1] = val ? 5.0 : 0.0;
	SolidStateSwitch[n - 1].SetState(val);
}

SCEA_SA_5045::SCEA_SA_5045()
{
	Reset();
}

void SCEA_SA_5045::Reset()
{
	for (int i = 0;i < 12;i++)
	{
		Output[i] = 0.0;
	}
}

void SCEA_SA_5045::SetOutput(int n, bool val)
{
	Output[n - 1] = val ? 5.0 : 0.0;
}

SCEA_SA_5051::SCEA_SA_5051()
{
	Reset();
}

void SCEA_SA_5051::Reset()
{
	for (int i = 0;i < 3;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5062::SCEA_SA_5062()
{
	Reset();
}

void SCEA_SA_5062::Reset()
{
	for (int i = 0;i < 4;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5071::SCEA_SA_5071()
{
	Reset();
}

void SCEA_SA_5071::Reset()
{
	for (int i = 0;i < 4;i++)
	{
		Output[i] = 0.0;
	}
}

SCERA::SCERA()
{
	lem = NULL;
	dcpower = NULL;
	SCERAHeat = 0;
	SCERASECHeat = 0;
	Operate = false;
}

void SCERA::Init(LEM *l, e_object *dc, h_HeatLoad *hl, h_HeatLoad *sechl)
{
	lem = l;
	dcpower = dc;
	SCERAHeat = hl;
	SCERASECHeat = sechl;
}

bool SCERA::IsPowered()
{
	if (dcpower->Voltage() < SP_MIN_DCVOLTAGE) { return false; }
	return true;
}

double SCERA::scale_data(double data, double low, double high)
{
	double step = 0;

	// First eliminate cases outside of the scales
	if (data >= high) { return 5.0; }
	if (data <= low) { return 0.0; }

	// Now figure step value
	step = ((high - low) / 5.0);
	// and return result
	return (data - low) / step;
}

SCERA1::SCERA1()
{

}

void SCERA1::Reset()
{
	SA2.Reset();
	SA3.Reset();
	SA4.Reset();
	SA5.Reset();
	SA6.Reset();
	SA7.Reset();
	SA8.Reset();
	SA9.Reset();
	SA10.Reset();
	SA11.Reset();
	SA12.Reset();
	SA13.Reset();
	SA14.Reset();
	SA15.Reset();
	SA16.Reset();
	SA17.Reset();
	SA18.Reset();
	SA19.Reset();
	SA20.Reset();
	SA21.Reset();
}

void SCERA1::Timestep()
{
	if (!Operate) {
		if (IsPowered())
			Operate = true;
		else
			return;
	}
	else if (!IsPowered()) {
		Reset();
		return;
	}

	bool bval = false;
	double dval = 0.0;

	//Jet Driver B2U Output (GH1426V)
	SA2.SetOutput(1, lem->atca.jet_request[LMRCS_B2U] == 1);
	//Jet Driver A2D Output (GH1427V)
	SA2.SetOutput(2, lem->atca.jet_request[LMRCS_A2D] == 1);
	//Jet Driver A2A Output (GH1428V)
	SA2.SetOutput(3, lem->atca.jet_request[LMRCS_A2A] == 1);
	//Jet Driver B2L Output (GH1429V)
	SA2.SetOutput(4, lem->atca.jet_request[LMRCS_B2L] == 1);
	//Jet Driver A1U Output (GH1430V)
	SA2.SetOutput(5, lem->atca.jet_request[LMRCS_A1U] == 1);
	//Jet Driver B1D Output (GH1431V)
	SA2.SetOutput(6, lem->atca.jet_request[LMRCS_B1D] == 1);
	//Jet Driver B1L Output (GH1433V)
	SA2.SetOutput(7, lem->atca.jet_request[LMRCS_B1L] == 1);
	//Jet Driver A1F Output (GH1432V)
	SA2.SetOutput(8, lem->atca.jet_request[LMRCS_A1F] == 1);
	//Abort Command (GY0050X)
	SA2.SetOutput(9, lem->AbortSwitch.GetState() == 0 && lem->SCS_ENG_CONT_CB.IsPowered());
	//Open

	//Deadband select (wide) (GH1603)
	SA3.SetOutput(1, lem->DeadBandSwitch.IsUp() && !lem->scca1.GetK15() && !lem->scca1.GetK203() && !lem->scca1.GetK204());
	//Ascent engine arm (GH1230)
	SA3.SetOutput(2, lem->EngineArmSwitch.IsUp() && lem->SCS_ENG_ARM_CB.IsPowered());
	//RCS thrust chamber pressure A2A (GR5041)
	SA3.SetOutput(3, lem->GetRCSThrusterLevel(LMRCS_A2A) > 0.5);
	//RCS thrust chamber pressure B2L (GR5042)
	SA3.SetOutput(4, lem->GetRCSThrusterLevel(LMRCS_B2L) > 0.5);
	//RCS thrust chamber pressure A1U (GR5043)
	SA3.SetOutput(5, lem->GetRCSThrusterLevel(LMRCS_A1U) > 0.5);
	//RCS thrust chamber pressure B1D (GR5044)
	SA3.SetOutput(6, lem->GetRCSThrusterLevel(LMRCS_B1D) > 0.5);
	//RCS thrust chamber pressure A1F (GR5045)
	SA3.SetOutput(7, lem->GetRCSThrusterLevel(LMRCS_A1F) > 0.5);
	//RCS thrust chamber pressure B1L (GR5046)
	SA3.SetOutput(8, lem->GetRCSThrusterLevel(LMRCS_B1L) > 0.5);
	//DPS Arm (GH1348X)
	SA3.SetOutput(9, lem->deca.GetEngArm());
	//X-translation override (GH1893)
	SA3.SetOutput(10, lem->PlusXTranslationButton.GetState() == 0 && lem->SCS_ATT_DIR_CONT_CB.IsPowered());

	//AEA Test mode fail (GI3232X)
	SA4.SetOutput(1, lem->aea.GetTestModeFailure());
	//Open
	//Jet Driver B4U Output (GH1418V)
	SA4.SetOutput(3, lem->atca.jet_request[LMRCS_B4U] == 1);
	//Jet Driver B4F Output (GH1420V)
	SA4.SetOutput(4, lem->atca.jet_request[LMRCS_B4F] == 1);
	//Jet Driver A4D Output (GH1419V)
	SA4.SetOutput(5, lem->atca.jet_request[LMRCS_A4D] == 1);
	//Jet Driver A4R Output (GH1421V)
	SA4.SetOutput(6, lem->atca.jet_request[LMRCS_A4R] == 1);
	//Jet Driver A3U Output (GH1422V)
	SA4.SetOutput(7, lem->atca.jet_request[LMRCS_A3U] == 1);
	//Jet Driver B3D Output (GH1423V)
	SA4.SetOutput(8, lem->atca.jet_request[LMRCS_B3D] == 1);
	//Jet Driver B3A Output (GH1424V)
	SA4.SetOutput(9, lem->atca.jet_request[LMRCS_B3A] == 1);
	//Jet Driver A3R Output (GH1425V)
	SA4.SetOutput(10, lem->atca.jet_request[LMRCS_A3R] == 1);

	//Suit outlet pressure (GF1301)
	SA5.SetOutput(1, scale_data(lem->ecs.GetSuitPressurePSI(), 0.0, 10.0));
	//CO2 partial pressure (GF1521)
	SA5.SetOutput(2, scale_data(lem->ecs.GetSensorCO2MMHg(), 0.0, 30.0));
	//Water separator no. 1 and 2 (GF9999)
	SA5.SetOutput(3, scale_data(lem->ecs.GetWaterSeparatorRPM(), 0.0, 3600.0));
	//S-band reciever signal (GT0994V)
	SA5.SetOutput(4, scale_data(lem->SBand.rcvr_agc_voltage, 0.0, 100.0));	// Needs to be preconditioned before entering SCEA

	//Helium pressure tank A (GR1101)
	SA6.SetOutput(1, scale_data(lem->RCSA.GetRCSHeliumPressPSI(), 0.0, 3500.0));
	//Helium pressure tank B (GR1102)
	SA6.SetOutput(2, scale_data(lem->RCSB.GetRCSHeliumPressPSI(), 0.0, 3500.0));
	//Helium regulator pressure system A (GR1201)
	SA6.SetOutput(3, scale_data(lem->RCSA.GetRCSRegulatorPressPSI(), 0.0, 350.0));
	//Helium regulator pressure system B (GR1202)
	SA6.SetOutput(4, scale_data(lem->RCSB.GetRCSRegulatorPressPSI(), 0.0, 350.0));

	//Ascent oxygen tank no. 1 pressure (GF3582)
	SA7.SetOutput(1, scale_data(lem->ecs.AscentOxyTank1PressurePSI(), 0.0, 1000.0));
	//Ascent oxygen tank no. 2 pressure (GF3583)
	SA7.SetOutput(2, scale_data(lem->ecs.AscentOxyTank2PressurePSI(), 0.0, 1000.0));
	//Descent tank water quantity (GF4500)
	SA7.SetOutput(3, scale_data(lem->ecs.DescentWaterTankQuantity(), 0.0, 1.0));
	//Selected ED battery voltage (GY0703U)
	SA7.SetOutput(4, scale_data(lem->EPSEDVoltSelect.Voltage(), 0.0, 40.0));	// Needs to be preconditioned before entering SCEA

	//Ascent tank no. 1 water quantity (GF4582)
	SA8.SetOutput(1, scale_data(lem->ecs.AscentWaterTank1Quantity(), 0.0, 1.0));
	//Ascent tank no. 2 water quantity (GF4583)
	SA8.SetOutput(2, scale_data(lem->ecs.AscentWaterTank2Quantity(), 0.0, 1.0));
	//APS regulator outlet manifold pressure (GP0025)
	SA8.SetOutput(3, scale_data(lem->APSPropellant.GetHeliumRegulator1OutletPressurePSI(), 0.0, 300.0));
	//APS helium tank no. 1 pressure (GP0001)
	SA8.SetOutput(4, scale_data(lem->APSPropellant.GetAscentHelium1PressPSI(), 0.0, 4000.0));

	//DPS fuel tank no. 1 fuel bulk temperature (GQ3718)
	SA9.SetOutput(1, scale_data(lem->DPSPropellant.GetFuelTank1BulkTempF(), 20.0, 120.0));
	//DPS fuel tank no. 2 fuel bulk temperature (GQ3718)
	SA9.SetOutput(2, scale_data(lem->DPSPropellant.GetFuelTank2BulkTempF(), 20.0, 120.0));
	//APS fuel tank, fuel bulk temperature (GP0718)
	SA9.SetOutput(3, scale_data(lem->APSPropellant.GetFuelTankBulkTempF(), 20.0, 120.0));
	//APS oxidizer tank, oxidizer bulk temperature (GP1218)
	SA9.SetOutput(4, scale_data(lem->APSPropellant.GetOxidizerTankBulkTempF(), 20.0, 120.0));

	//Water sublimator coolant outlet temperature (GF9998)
	SA10.SetOutput(1, scale_data(lem->ecs.GetSelectedGlycolTempF(), 20.0, 120.0));
	//ASA Temperature (GI3301T)
	SA10.SetOutput(2, scale_data(lem->asa.GetASATempF(), 20.0, 200.0));
	//DPS oxidizer tank no. 1 fuel bulk temperature (GQ4218)
	SA10.SetOutput(3, scale_data(lem->DPSPropellant.GetOxidizerTank1BulkTempF(), 20.0, 120.0));
	//DPS oxidizer tank no. 2 fuel bulk temperature (GQ4219)
	SA10.SetOutput(4, scale_data(lem->DPSPropellant.GetOxidizerTank2BulkTempF(), 20.0, 120.0));

	//RCS thrust chamber pressure B4U (GR5031)
	SA11.SetOutput(1, lem->GetRCSThrusterLevel(LMRCS_B4U) > 0.5);
	//RCS thrust chamber pressure A4D (GR5032)
	SA11.SetOutput(2, lem->GetRCSThrusterLevel(LMRCS_A4D) > 0.5);
	//RCS thrust chamber pressure B4F (GR5033)
	SA11.SetOutput(3, lem->GetRCSThrusterLevel(LMRCS_B4F) > 0.5);
	//RCS thrust chamber pressure A4R (GR5034)
	SA11.SetOutput(4, lem->GetRCSThrusterLevel(LMRCS_A4R) > 0.5);
	//RCS thrust chamber pressure A3U (GR5035)
	SA11.SetOutput(5, lem->GetRCSThrusterLevel(LMRCS_A3U) > 0.5);
	//RCS thrust chamber pressure B3D (GR5036)
	SA11.SetOutput(6, lem->GetRCSThrusterLevel(LMRCS_B3D) > 0.5);
	//RCS thrust chamber pressure B3A (GR5037)
	SA11.SetOutput(7, lem->GetRCSThrusterLevel(LMRCS_B3A) > 0.5);
	//RCS thrust chamber pressure A3R (GR5038)
	SA11.SetOutput(8, lem->GetRCSThrusterLevel(LMRCS_A3R) > 0.5);
	//RCS thrust chamber pressure B2U (GR5039)
	SA11.SetOutput(9, lem->GetRCSThrusterLevel(LMRCS_B2U) > 0.5);
	//RCS thrust chamber pressure A2D (GR5040)
	SA11.SetOutput(10, lem->GetRCSThrusterLevel(LMRCS_A2D) > 0.5);

	//Main shutoff valves closed, system A (GR9609)
	SA12.SetOutput(1, !lem->RCSA.GetMainShutoffValve()->IsOpen());
	//Main shutoff valves closed, system B (GR9610)
	SA12.SetOutput(2, !lem->RCSB.GetMainShutoffValve()->IsOpen());
	//System A oxidizer interconnect valves open (GR9641)
	bval = lem->RCSA.GetPrimOxidInterconnectValve()->IsOpen() && lem->RCSA.GetSecOxidInterconnectValve()->IsOpen();
	SA12.SetOutput(3, bval);
	//System B oxidizer interconnect valves open (GR9642)
	bval = lem->RCSB.GetPrimOxidInterconnectValve()->IsOpen() && lem->RCSB.GetSecOxidInterconnectValve()->IsOpen();
	SA12.SetOutput(4, bval);
	//Open
	//APS helium primary line solenoid valve closed (GP0318)
	SA12.SetOutput(6, !lem->APSPropellant.GetHeliumValve1()->IsOpen());
	//APS helium secondary line solenoid valve closed (GP0320)
	SA12.SetOutput(7, !lem->APSPropellant.GetHeliumValve2()->IsOpen());
	//RCS main propulsion valve A closed (GR9609)
	SA12.SetOutput(8, !lem->RCSA.GetMainShutoffValve()->IsOpen());
	//Open
	//RCS main propulsion valve B closed (GR9610)
	SA12.SetOutput(10, !lem->RCSB.GetMainShutoffValve()->IsOpen());
	//ED Relay A K1-K6 (GY0201X)
	SA12.SetOutput(11, lem->stage < 2 && lem->eds.RelayBoxA.GetStageRelayMonitor());
	//ED Relay B K1-K6 (GY0202X)
	SA12.SetOutput(12, lem->eds.RelayBoxB.GetStageRelayMonitor());

	//Thrust chamber assembly solenoid valve A4 closed (GR9661)
	SA13.SetOutput(1, !lem->RCSA.GetQuad4IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve B4 closed (GR9662)
	SA13.SetOutput(2, !lem->RCSB.GetQuad4IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve A3 closed (GR9663)
	SA13.SetOutput(3, !lem->RCSA.GetQuad3IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve B3 closed (GR9664)
	SA13.SetOutput(4, !lem->RCSB.GetQuad3IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve A2 closed (GR9665)
	SA13.SetOutput(5, !lem->RCSA.GetQuad2IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve B2 closed (GR9666)
	SA13.SetOutput(6, !lem->RCSB.GetQuad2IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve A1 closed (GR9667)
	SA13.SetOutput(7, !lem->RCSA.GetQuad1IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve B1 closed (GR9668)
	SA13.SetOutput(8, !lem->RCSB.GetQuad1IsolationValve()->IsOpen());
	//Landing gear legs deployed (GM5000)
	SA13.SetOutput(9, lem->stage < 2 && lem->eds.GetLGDeployed());
	//RCS/ASC interconnect A not closed (GR9631)
	bval = lem->RCSA.GetPrimFuelInterconnectValve()->IsOpen() && lem->RCSA.GetSecFuelInterconnectValve()->IsOpen();
	SA13.SetOutput(10, bval);
	//RCS/ASC interconnect B not closed (GR9632)
	bval = lem->RCSB.GetPrimFuelInterconnectValve()->IsOpen() && lem->RCSB.GetSecFuelInterconnectValve()->IsOpen();
	SA13.SetOutput(11, bval);
	//RCS A/B crossfeed open
	bval = lem->RCSB.GetFuelCrossfeedValve()->IsOpen() && lem->RCSB.GetOxidCrossfeedValve()->IsOpen();
	SA13.SetOutput(12, bval);

	//Thrust chamber assembly solenoid valve A4 closed (GR9661)
	SA14.SetOutput(1, !lem->RCSA.GetQuad4IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve B4 closed (GR9662)
	SA14.SetOutput(2, !lem->RCSB.GetQuad4IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve A3 closed (GR9663)
	SA14.SetOutput(3, !lem->RCSA.GetQuad3IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve B3 closed (GR9664)
	SA14.SetOutput(4, !lem->RCSB.GetQuad3IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve A2 closed (GR9665)
	SA14.SetOutput(5, !lem->RCSA.GetQuad2IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve B2 closed (GR9666)
	SA14.SetOutput(6, !lem->RCSB.GetQuad2IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve A1 closed (GR9667)
	SA14.SetOutput(7, !lem->RCSA.GetQuad1IsolationValve()->IsOpen());
	//Thrust chamber assembly solenoid valve B1 closed (GR9668)
	SA14.SetOutput(8, !lem->RCSB.GetQuad1IsolationValve()->IsOpen());
	//ED Relay A K1-K6 (GY0201X)
	SA14.SetOutput(11, lem->stage < 2 && lem->eds.RelayBoxA.GetStageRelayMonitor());
	//ED Relay B K1-K6 (GY0202X)
	SA14.SetOutput(12, lem->eds.RelayBoxB.GetStageRelayMonitor());

	//Automatic thrust command voltage (GH1331)
	SA15.SetOutput(1, scale_data(lem->deca.GetAutoThrustVoltage(), 0.0, 12.0));
	//Manual thrust command voltage (GH1311)
	SA15.SetOutput(2, scale_data(lem->deca.GetManualThrustVoltage(), 0.0, 14.6));
	//Commander's bus voltage (GC0301)
	SA15.SetOutput(3, scale_data(lem->CDRDCBusVoltCB.Voltage(), 0.0, 40.0));
	//Abort sensor assembly +12VDC (GI3215V)
	SA15.SetOutput(4, scale_data(lem->asa.GetASA12V(), 0.0, 14.0));

	//Inverter bus frequency (GC0155F)
	SA16.SetOutput(1, scale_data(lem->AC_A_BUS_VOLT_CB.Frequency(), 380.0, 420.0));
	//Abort sensor assembly frequency (GI3233F)
	SA16.SetOutput(2, scale_data(lem->asa.GetASAFreq(), 380.0, 420.0));

	//Inverter bus voltage (GC0071V)
	SA17.SetOutput(1, scale_data(lem->AC_A_BUS_VOLT_CB.Voltage(), 0.0, 125.0));
	//Rate gyro assembly pickoff excitation (0.8kc) voltage (GH1405V)
	SA17.SetOutput(3, scale_data(lem->atca.GetRGAPickoffExcitationVoltage(), 0.0, 31.0));

	//Frequency inverter bus (GC0155F)
	SA18.SetOutput(1, scale_data(lem->AC_A_BUS_VOLT_CB.Frequency(), 380.0, 420.0));
	//Inverter bus voltage (GC0071V)
	SA18.SetOutput(2, scale_data(lem->AC_A_BUS_VOLT_CB.Voltage(), 0.0, 125.0));
	//Commander's bus voltage (GC0301)
	SA18.SetOutput(3, SA15.GetVoltage(3));

	//APS helium tank no. 2 pressure (GP0002)
	SA19.SetOutput(1, scale_data(lem->APSPropellant.GetAscentHelium2PressPSI(), 0.0, 4000.0));
	//DPS helium regulator output manifold pressure (GQ3018)
	SA19.SetOutput(2, scale_data(lem->DPSPropellant.GetHeliumRegulatorManifoldPressurePSI(), 0.0, 300.0));
	//APS fuel bipropellant valve inlet pressure (GP1501)
	SA19.SetOutput(3, scale_data(lem->APSPropellant.GetFuelTrimOrificeOutletPressurePSI(), 0.0, 250.0));
	//APS oxidizer bipropellant valve inlet pressure (GP1503)
	SA19.SetOutput(4, scale_data(lem->APSPropellant.GetOxidTrimOrificeOutletPressurePSI(), 0.0, 250.0));

	//Quad 4 temperature (GR6001T)
	SA20.SetOutput(1, scale_data(lem->GetRCSQuadTempF(3), 20.0, 200.0));
	//Quad 3 temperature (GR6002T)
	SA20.SetOutput(2, scale_data(lem->GetRCSQuadTempF(2), 20.0, 200.0));
	//Quad 2 temperature (GR6003T)
	SA20.SetOutput(3, scale_data(lem->GetRCSQuadTempF(1), 20.0, 200.0));
	//Quad 1 temperature (GR6004T)
	SA20.SetOutput(4, scale_data(lem->GetRCSQuadTempF(0), 20.0, 200.0));

	//Suit inlet temperature (GF1281)
	SA21.SetOutput(1, scale_data(lem->ecs.GetSuitTempF(), 20.0, 120.0));
	//Cabin temperature (GF1651)
	SA21.SetOutput(2, scale_data(lem->ecs.GetCabinTempF(), 20.0, 120.0));
	//Landing radar antenna temperature (GN7563T)
	SA21.SetOutput(3, scale_data(lem->LR.GetAntennaTempF(), -200.0, 200.0));
	//Rendezvous radar antenna temperature (GN7723T)
	SA21.SetOutput(4, scale_data(lem->RR.GetAntennaTempF(), -200.0, 200.0));

}

double SCERA1::GetVoltage(int sa, int chan)
{
	if (sa == 2)
	{
		return SA2.GetVoltage(chan);
	}
	else if (sa == 3)
	{
		return SA3.GetVoltage(chan);
	}
	else if (sa == 4)
	{
		return SA4.GetVoltage(chan);
	}
	else if (sa == 5)
	{
		return SA5.GetVoltage(chan);
	}
	else if (sa == 6)
	{
		return SA6.GetVoltage(chan);
	}
	else if (sa == 7)
	{
		return SA7.GetVoltage(chan);
	}
	else if (sa == 8)
	{
		return SA8.GetVoltage(chan);
	}
	else if (sa == 9)
	{
		return SA9.GetVoltage(chan);
	}
	else if (sa == 10)
	{
		return SA10.GetVoltage(chan);
	}
	else if (sa == 11)
	{
		return SA11.GetVoltage(chan);
	}
	else if (sa == 12)
	{
		return SA12.GetVoltage(chan);
	}
	else if (sa == 13)
	{
		return SA13.GetVoltage(chan);
	}
	else if (sa == 14)
	{
		return SA14.GetVoltage(chan);
	}
	else if (sa == 15)
	{
		return SA15.GetVoltage(chan);
	}
	else if (sa == 16)
	{
		return SA16.GetVoltage(chan);
	}
	else if (sa == 17)
	{
		return SA17.GetVoltage(chan);
	}
	else if (sa == 18)
	{
		return SA18.GetVoltage(chan);
	}
	else if (sa == 19)
	{
		return SA19.GetVoltage(chan);
	}
	else if (sa == 20)
	{
		return SA20.GetVoltage(chan);
	}
	else if (sa == 21)
	{
		return SA21.GetVoltage(chan);
	}

	return 0.0;
}

void SCERA1::SystemTimestep(double simdt)
{
	if (Operate)
	{
		dcpower->DrawPower(12.6);
		SCERAHeat->GenerateHeat(6.3);
		SCERASECHeat->GenerateHeat(6.3);
	}
}

SCEA_SolidStateSwitch* SCERA1::GetSwitch(int sa, int chan)
{
	if (sa == 12)
	{
		return SA12.GetSwitch(chan);
	}
	else if (sa == 13)
	{
		return SA13.GetSwitch(chan);
	}

	return NULL;
}

SCERA2::SCERA2()
{

}

void SCERA2::Reset()
{
	SA2.Reset();
	SA3.Reset();
	SA4.Reset();
	SA5.Reset();
	SA6.Reset();
	SA7.Reset();
	SA8.Reset();
	SA9.Reset();
	SA10.Reset();
	SA12.Reset();
	SA13.Reset();
	SA14.Reset();
	SA15.Reset();
	SA16.Reset();
	SA17.Reset();
	SA18.Reset();
	SA19.Reset();
	SA20.Reset();
	SA21.Reset();
}

void SCERA2::Timestep()
{
	ChannelValue val11;
	ChannelValue val163;

	if (!Operate) {
		if (IsPowered())
			Operate = true;
		else
			return;
	}
	else if (!IsPowered()) {
		Reset();
		return;
	}

	val11 = lem->agc.GetOutputChannel(011);
	val163 = lem->agc.GetOutputChannel(0163);

	//Rendezvous Radar No Track (GN7621X)
	SA2.SetOutput(1, lem->RR.GetNoTrackSignal());
	//APS fuel tank low level (GP0908X)
	SA2.SetOutput(6, lem->APSPropellant.GetFuelLowLevel());
	//APS oxidizer tank low level (GP1408X)
	SA2.SetOutput(7, lem->APSPropellant.GetOxidLowLevel());
	//AGS Warmup (GI3305X)
	SA2.SetOutput(11, lem->AGSOperateSwitch.GetState() == THREEPOSSWITCH_CENTER);

	//Suit fan 1 fail (GF1083X)
	SA3.SetOutput(2, lem->ecs.GetSuitFan1Failure());
	//Primary Glycol LLS (GF2041X) & Secondary Glycol LLS (GF2042X)
	SA3.SetOutput(3, lem->ecs.GetPrimGlycolLowLevel() || lem->ecs.GetSecGlycolLowLevel());
	//AGS Selected (GH1621X)
	SA3.SetOutput(5, lem->scca2.GetK7());
	//Suit fan 2 fail (GF1084X)
	SA3.SetOutput(6, lem->ecs.GetSuitFan2Failure());
	//Emergency oxygen valve electrically open (GF3572)
	SA3.SetOutput(8, lem->CabinRepressValve.GetEmergencyCabinRepressRelay());
	//Pitch Trim Fail (GH1323X)
	SA3.SetOutput(9, lem->deca.GetK21());
	//Roll Trim Fail (GH1330X)
	SA3.SetOutput(10, lem->deca.GetK22());
	//LGC Warning (GG9001X)
	SA3.SetOutput(11, val163[Ch163DSKYWarn]);
	//ISS Warning (GG9002X)
	SA3.SetOutput(12, val11[ISSWarning]);

	//Battery 1 High Tap (GC4361X)
	SA4.SetOutput(1, lem->stage < 2 && lem->ECA_1a.input == 1);
	//Battery 1 Low Tap (GC4362X)
	SA4.SetOutput(2, lem->stage < 2 && lem->ECA_1a.input == 2);
	//Battery 2 High Tap (GC4363X)
	SA4.SetOutput(3, lem->stage < 2 && lem->ECA_1b.input == 1);
	//Battery 2 Low Tap (GC43664X)
	SA4.SetOutput(4, lem->stage < 2 && lem->ECA_1b.input == 2);
	//Battery 3 High Tap (GC4365X)
	SA4.SetOutput(5, lem->stage < 2 && lem->ECA_2a.input == 1);
	//Battery 3 Low Tap (GC4366X)
	SA4.SetOutput(6, lem->stage < 2 && lem->ECA_2a.input == 2);
	//Battery 4 High Tap (GC4367X)
	SA4.SetOutput(7, lem->stage < 2 && lem->ECA_2b.input == 1);
	//Battery 4 Low Tap (GC4368X)
	SA4.SetOutput(8, lem->stage < 2 && lem->ECA_2b.input == 2);
	//Battery 5 Backup Feed (GC4369X)
	SA4.SetOutput(9, lem->ECA_3b.input == 1);
	//Battery 6 Normal Feed (GC4370X)
	SA4.SetOutput(10, lem->ECA_4a.input == 1);
	//Battery 5 Normal Feed (GC4371X)
	SA4.SetOutput(11, lem->ECA_3a.input == 1);
	//Battery 6 Backup Feed (GC4372X)
	SA4.SetOutput(12, lem->ECA_4b.input == 1);

	//CO2 secondary cartridge (GF1241)
	SA5.SetOutput(1, lem->CO2CanisterSelectSwitch.GetState() == 0);
	//Suit diverter valve position indicator closed (GF1221)
	SA5.SetOutput(2, lem->SuitGasDiverterSwitch.GetState() == 0);
	//Suit pressure relief valve position indicator closed (GF1211)
	SA5.SetOutput(3, !lem->ecs.IsSuitCircuitReliefValveOpen());
	//Suit pressure relief valve position indicator open (GF1212)
	SA5.SetOutput(4, lem->ecs.IsSuitCircuitReliefValveOpen());
	//Oxygen regulator valve A locked closed (GF3071)
	SA5.SetOutput(5, lem->PressRegAValve.GetState() == 3);
	//Oxygen regulator valve B locked closed (GF3073)
	SA5.SetOutput(7, lem->PressRegBValve.GetState() == 3);
	//Cabin gas return valve position indicator closed (GF1231)
	SA5.SetOutput(9, !lem->ecs.IsCabinGasReturnValveOpen());
	//Cabin gas return valve position indicator open (GF1232)
	SA5.SetOutput(10, lem->ecs.IsCabinGasReturnValveOpen());
	//Suit inlet valve position indicator no. 1 closed (GF1201)
	SA5.SetOutput(11, lem->CDRSuitIsolValve.GetState() == 1);
	//Suit inlet valve position indicator no. 2 closed (GF1202)
	SA5.SetOutput(12, lem->LMPSuitIsolValve.GetState() == 1);

	//Rate gyro assembly spin motor line A-B voltage (GH1401V)
	SA7.SetOutput(1, scale_data(lem->atca.GetRGASpinMotorVoltage(), 0.0, 30.0));
	//Rate gyro assembly spin motor line B-C voltage (GH1402V)
	SA7.SetOutput(2, scale_data(lem->atca.GetRGASpinMotorVoltage(), 0.0, 30.0));
	//Rate gyro assembly spin motor line C-A voltage (GH1403V)
	SA7.SetOutput(3, scale_data(lem->atca.GetRGASpinMotorVoltage(), 0.0, 30.0));

	//Descent oxygen tank pressure (GF3584)
	SA8.SetOutput(1, scale_data(lem->ecs.DescentOxyTankPressurePSI(), 0.0, 304.0));
	SA8.SetOutput(2, scale_data(lem->ecs.DescentOxyTankPressurePSI(), 0.0, 3000.0));
	//4.3 vdc supply (GH1408V)
	SA8.SetOutput(3, scale_data(lem->atca.GetPlus6VDCSupplyVoltage(), 0.0, 5.0));
	//LMP bus voltage (GC0302V)
	SA8.SetOutput(4, SA15.GetVoltage(4));

	//Cooling pump no. 1 failure (GF2936)
	SA12.SetOutput(2, lem->PrimGlycolPumpController.GetGlycolPumpFailRelay());
	//Descent propellant tanks (liquid level low) (GQ4455X)
	SA12.SetOutput(3, lem->DPSPropellant.PropellantLevelLow());

	//Coolant pump no. 2 failure (GF2935)
	SA13.SetOutput(3, lem->ecs.GetGlycolPump2Failure());
	//AGS Standby (GI3306X)
	SA13.SetOutput(5, lem->AGSOperateSwitch.GetState() == THREEPOSSWITCH_CENTER);

	//Prim -4.7VDC (GH1488V)
	SA15.SetOutput(1, scale_data(lem->atca.GetPrimPowerVoltage(), -9.4169, -3.3929));
	//Backup -4.7VDC (GH1489V)
	SA15.SetOutput(2, scale_data(lem->atca.GetBackupPowerVoltage(), -9.4169, -3.3929));
	//Abort sensor assembly +28VDC (GI3214V)
	SA15.SetOutput(3, scale_data(lem->asa.GetASA28V(), 0.0, 40.0));
	//LMP bus voltage (GC0302V)
	SA15.SetOutput(4, scale_data(lem->LMPDCBusVoltCB.Voltage(), 0.0, 40.0));

	//Battery 1 voltage (GC0101V)
	SA16.SetOutput(1, scale_data(lem->Battery1->Voltage(), 0.0, 40.0));
	//Battery 2 voltage (GC0102V)
	SA16.SetOutput(2, scale_data(lem->Battery2->Voltage(), 0.0, 40.0));
	//Battery 3 voltage (GC0103V)
	SA16.SetOutput(3, scale_data(lem->Battery3->Voltage(), 0.0, 40.0));
	//Battery 4 voltage (GC0104V)
	SA16.SetOutput(4, scale_data(lem->Battery4->Voltage(), 0.0, 40.0));

	//Battery 5 voltage (GC0105V)
	SA17.SetOutput(1, scale_data(lem->Battery5->Voltage(), 0.0, 40.0));

	//Battery 6 voltage (GC0106V)
	SA18.SetOutput(1, scale_data(lem->Battery6->Voltage(), 0.0, 40.0));

	//+15-vdc supply (GH1406V)
	SA19.SetOutput(1, scale_data(lem->atca.GetPlus15VDCSupplyVoltage(), 0.0, 17.0));
	//-15-vdc supply (GH1407V)
	SA19.SetOutput(2, scale_data(-lem->atca.GetMinus15VDCSupplyVoltage(), 0.0, 17.0));
	//+6-vdc supply (GH1493V)
	SA19.SetOutput(3, scale_data(lem->atca.GetPlus6VDCSupplyVoltage(), 0.0, 7.0));
	//-6-vdc supply (GH1494V)
	SA19.SetOutput(4, scale_data(-lem->atca.GetMinus6VDCSupplyVoltage(), 0.0, 7.0));

	//RCS Fuel tank A temperature (GR2121)
	SA20.SetOutput(2, scale_data(lem->RCSA.GetFuelTankTempF(), 20.0, 120.0));
	//RCS Fuel tank B temperature (GR2122)
	SA20.SetOutput(3, scale_data(lem->RCSB.GetFuelTankTempF(), 20.0, 120.0));

	//S-Band antenna temperature (GT0454T)
	SA21.SetOutput(1, scale_data(lem->SBandSteerable.GetAntennaTempF(), -200.0, 200.0));
	//Ascent water line no. 1 temperature (GF4585T)
	SA21.SetOutput(3, scale_data(lem->ecs.GetAscWaterTank1TempF(), -200.0, 200.0));
	//Ascent water line no. 2 temperature (GF4586T)
	SA21.SetOutput(4, scale_data(lem->ecs.GetAscWaterTank2TempF(), -200.0, 200.0));

	//sprintf(oapiDebugString(), "LGCV %lf ISSV %lf", lem->scera2.GetVoltage(3, 11), lem->scera2.GetVoltage(3, 12));

}

void SCERA2::SystemTimestep(double simdt)
{
	if (Operate)
	{
		dcpower->DrawPower(10.36);
		SCERAHeat->GenerateHeat(5.18);
		SCERASECHeat->GenerateHeat(5.18);
	}
}

double SCERA2::GetVoltage(int sa, int chan)
{
	if (sa == 2)
	{
		return SA2.GetVoltage(chan);
	}
	else if (sa == 3)
	{
		return SA3.GetVoltage(chan);
	}
	else if (sa == 4)
	{
		return SA4.GetVoltage(chan);
	}
	else if (sa == 5)
	{
		return SA5.GetVoltage(chan);
	}
	else if (sa == 6)
	{
		return SA6.GetVoltage(chan);
	}
	else if (sa == 7)
	{
		return SA7.GetVoltage(chan);
	}
	else if (sa == 8)
	{
		return SA8.GetVoltage(chan);
	}
	else if (sa == 9)
	{
		return SA9.GetVoltage(chan);
	}
	else if (sa == 10)
	{
		return SA10.GetVoltage(chan);
	}
	else if (sa == 12)
	{
		return SA12.GetVoltage(chan);
	}
	else if (sa == 13)
	{
		return SA13.GetVoltage(chan);
	}
	else if (sa == 14)
	{
		return SA14.GetVoltage(chan);
	}
	else if (sa == 15)
	{
		return SA15.GetVoltage(chan);
	}
	else if (sa == 16)
	{
		return SA16.GetVoltage(chan);
	}
	else if (sa == 17)
	{
		return SA17.GetVoltage(chan);
	}
	else if (sa == 18)
	{
		return SA18.GetVoltage(chan);
	}
	else if (sa == 19)
	{
		return SA19.GetVoltage(chan);
	}
	else if (sa == 20)
	{
		return SA20.GetVoltage(chan);
	}
	else if (sa == 21)
	{
		return SA21.GetVoltage(chan);
	}

	return 0.0;
}

SCEA_SolidStateSwitch* SCERA2::GetSwitch(int sa, int chan)
{
	if (sa == 4)
	{
		return SA4.GetSwitch(chan);
	}
	else if (sa == 12)
	{
		return SA12.GetSwitch(chan);
	}

	return NULL;
}