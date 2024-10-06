/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

Lunar Module Electrical Power Subsystem

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
#include "papi.h"
#include "LEM.h"
#include "lm_eps.h"

LEM_DeadfaceRelayBox::LEM_DeadfaceRelayBox()
{
	DFR = false;
	lem = NULL;
}

void LEM_DeadfaceRelayBox::Init(LEM *l, bool dfrstate)
{
	lem = l;
	DFR = dfrstate;
}

void LEM_DeadfaceRelayBox::Timestep()
{
	if (lem->AscentECAContFeeder.Voltage() > SP_MIN_DCVOLTAGE && !DFR && lem->DSCBattFeedSwitch.IsUp())
	{
		DFR = true;
		CheckStatus();
	}
	else if (lem->rjb.GetHVLVOffSignalAbort() || (lem->AscentECAContFeeder.Voltage() > SP_MIN_DCVOLTAGE && DFR && lem->DSCBattFeedSwitch.IsDown()))
	{
		DFR = false;
		CheckStatus();
	}
}

void LEM_DeadfaceRelayBox::SaveState(FILEHANDLE scn)
{
	char buffer[1000];

	sprintf(buffer, "%d", DFR);
	oapiWriteScenario_string(scn, "DEADFACERELAYBOX", buffer);
}

void LEM_DeadfaceRelayBox::LoadState(char *line)
{
	int var;
	sscanf(line + 16, "%d", &var);
	DFR = (var != 0);

	CheckStatus();
}

void LEM_DeadfaceRelayBox::CheckStatus()
{
	if (DFR && lem->stage < 2)
	{
		// Reconnect ECA outputs
		lem->DES_CDRs28VBusA.WireTo(&lem->ECA_2.ECAChannelA);
		lem->DES_CDRs28VBusB.WireTo(&lem->ECA_2.ECAChannelB);
	}
	else
	{
		lem->DES_CDRs28VBusA.Disconnect();
		lem->DES_CDRs28VBusB.Disconnect();
	}
}

LEM_RelayJunctionBox::LEM_RelayJunctionBox()
{
	K1 = false;
	K2 = false;
	DFR = false;
	LDA = false;
	LDR = false;
	HVLVOffSignalCM = false;
	HVLVOffSignalAbort = false;
	lem = NULL;
}

void LEM_RelayJunctionBox::Init(LEM *l, bool dfrstate)
{
	lem = l;
	DFR = dfrstate;
}

void LEM_RelayJunctionBox::Timestep()
{
	if (lem->stage < 2 && lem->LMPDesECAMainCB.IsPowered() && lem->scca1.GetK10())
	{
		K1 = true;
	}
	else
	{
		K1 = false;
	}
	if (lem->stage < 2 && lem->CDRDesECAMainCB.IsPowered() && lem->scca1.GetK20())
	{
		K2 = true;
	}
	else
	{
		K2 = false;
	}
	//Signal to switch off descent ECAs
	HVLVOffSignalCM = CalcHVLVOffSignalCM();
	HVLVOffSignalAbort = CalcHVLVOffSignalAbort();

	//Deadface relay
	if (lem->AscentECAContFeeder.Voltage() > SP_MIN_DCVOLTAGE && !DFR && lem->DSCBattFeedSwitch.IsUp())
	{
		DFR = true;
		CheckStatus();
	}
	else if (HVLVOffSignalAbort ||(lem->AscentECAContFeeder.Voltage() > SP_MIN_DCVOLTAGE && DFR && lem->DSCBattFeedSwitch.IsDown()))
	{
		DFR = false;
		CheckStatus();
	}

	//DES BATTS TB
	if (DFR && lem->drb.GetDeadFaceRelay())
	{
		lem->DSCBattFeedTB.SetState(1);
	}
	else
	{
		lem->DSCBattFeedTB.SetState(0);
	}

	//TBD: GSE relays
}

bool LEM_RelayJunctionBox::GetHVLVOffSignal()
{
	return HVLVOffSignalCM || HVLVOffSignalAbort;
}

bool LEM_RelayJunctionBox::GetHVLVOffSignalAbort()
{
	return HVLVOffSignalAbort;
}

bool LEM_RelayJunctionBox::GetLVOnSignal()
{
	return lem->CSMToLEMPowerConnector.GetBatteriesLVOn(); //TBD: Also from GSE
}

bool LEM_RelayJunctionBox::CalcHVLVOffSignalCM()
{
	if ((lem->CSMToLEMPowerConnector.GetBatteriesLVHVOffA() || lem->CSMToLEMPowerConnector.GetBatteriesLVHVOffB()) && !lem->CMPowerToCDRBusRelayA && !lem->CMPowerToCDRBusRelayB)
	{
		return true;
	}
	return false;
}

bool LEM_RelayJunctionBox::CalcHVLVOffSignalAbort()
{
	if ((K2 && lem->CDRDesECAContCB.IsPowered() && (lem->ECA_4.GetBattAFCOn() || lem->ECA_4.GetBattMFCOn())) || ((K1 && lem->LMPDesECAContCB.IsPowered() && (lem->ECA_3.GetBattAFCOn() || lem->ECA_3.GetBattMFCOn()))))
	{
		return true;
	}
	return false;
}

bool LEM_RelayJunctionBox::GetAscentECAOn()
{
	return (K1 || K2);
}

void LEM_RelayJunctionBox::CheckStatus()
{
	if (DFR && lem->stage < 2)
	{
		// Reconnect ECA outputs
		lem->DES_LMPs28VBusA.WireTo(&lem->ECA_1.ECAChannelA);
		lem->DES_LMPs28VBusB.WireTo(&lem->ECA_1.ECAChannelB);
	}
	else
	{
		lem->DES_LMPs28VBusA.Disconnect();
		lem->DES_LMPs28VBusB.Disconnect();
	}
}

void LEM_RelayJunctionBox::SaveState(FILEHANDLE scn)
{
	char buffer[1000];

	sprintf(buffer, "%d %d %d %d %d", K1, K2, DFR, LDA, LDR);
	oapiWriteScenario_string(scn, "RELAYJUNCTIONBOX", buffer);
}

void LEM_RelayJunctionBox::LoadState(char *line)
{
	int var[5];
	sscanf(line + 16, "%d %d %d %d %d", &var[0], &var[1], &var[2], &var[3], &var[4]);
	K1 = (var[0] != 0);
	K2 = (var[1] != 0);
	DFR = (var[2] != 0);
	LDA = (var[3] != 0);
	LDR = (var[4] != 0);

	CheckStatus();
}

// ELECTRICAL CONTROL ASSEMBLY SUBCHANNEL
LEM_ECAch::LEM_ECAch() {
	lem = NULL;
}

void LEM_ECAch::Init(LEM *s, e_object *src) {
	lem = s;
	dc_source = src;
	Volts = 24;
}

void LEM_ECAch::DrawPower(double watts) {
	power_load += watts;
};

LEM_AscentECAch::LEM_AscentECAch()
{
	relay = NULL;
}

void LEM_AscentECAch::Init(LEM *s, e_object *src, bool *r)
{
	LEM_ECAch::Init(s, src);
	relay = r;
}

void LEM_AscentECAch::UpdateFlow(double dt)
{
	if ((*relay) && dc_source != NULL)
	{
		dc_source->DrawPower(power_load);

		Volts = dc_source->Voltage();
		Amperes = dc_source->Current();
	}
	else
	{
		Volts = 0;
		Amperes = 0;
	}

	// Reset for next pass.
	e_object::UpdateFlow(dt);
}

LEM_DescentECAch::LEM_DescentECAch()
{
	eca = NULL;
}

void LEM_DescentECAch::Init(LEM *s, e_object *src, LEM_DescentECASector *e)
{
	LEM_ECAch::Init(s, src);
	eca = e;
}

void LEM_DescentECAch::UpdateFlow(double dt)
{
	// ECA INPUTS CAN BE PARALLELED, BUT NOT IN THE SAME CHANNEL
	// That is, Battery 1 and 2 can be on at the same time.
	// Draw power from the source, and retake voltage, etc.

	// Take power. It shouldn't be possible for HV and LV to be on at the same time
	if (eca->GetHVOn())
	{
		if (dc_source != NULL) {
			dc_source->DrawPower(power_load); // Draw 1:1
			Volts = dc_source->Voltage();
			Amperes = dc_source->Current();
		}
	}
	else if (eca->GetLVOn())
	{
		if (dc_source != NULL) {
			dc_source->DrawPower(power_load*1.06); // Draw 6% more (Why is this drawing 6% more??)
			Volts = (dc_source->Voltage()*0.85);
			Amperes = dc_source->Current();
		}
	}
	else
	{
		Volts = 0;
		Amperes = 0;
	}

	// Reset for next pass.
	e_object::UpdateFlow(dt);
}

LEM_DescentECASector::LEM_DescentECASector()
{
	LV = false;
	HV = false;
	AUX = false;
	LOR = false;
	OC = false;
	RC = false;
	lem = NULL;
}

void LEM_DescentECASector::Init(LEM *l, ThreePosSwitch *hvs, ThreePosSwitch *lvs, Battery *b, int inp)
{
	lem = l;
	HV_SW = hvs;
	LV_SW = lvs;
	batt = b;

	if (inp == 1)
	{
		HV = true;
		AUX = true;
	}
	else if (inp == 2)
	{
		LV = true;
		AUX = true;
	}
}

void LEM_DescentECASector::Timestep()
{
	POWER1 = lem->DescentECAContFeeder.Voltage() > SP_MIN_DCVOLTAGE;
	POWER2 = lem->DescentECAMainFeeder.Voltage() > SP_MIN_DCVOLTAGE;

	SIG1 = POWER1 && HV_SW->IsUp(); //HV on signal
	SIG2 = (POWER1 && HV_SW->IsCenter() && LV_SW->IsUp()) || lem->rjb.GetLVOnSignal(); //LV on signal
	SIG3 = (POWER1 && (HV_SW->IsDown() || LV_SW->IsDown())) || lem->rjb.GetHVLVOffSignal(); //Off

	TEMP1 = POWER2 && OC;
	TEMP2 = SIG3 || TEMP1;

	LOR = TEMP2;

	HV_SIG = SIG1 && !LOR && !LV;
	LV_SIG = SIG2 && !LOR && !HV;

	if (HV_SIG && !HV)
	{
		HV = true;
	}
	else if (TEMP2 && HV)
	{
		HV = false;
	}

	if (LV_SIG && !LV)
	{
		LV = true;
	}
	else if (TEMP2 && LV)
	{
		LV = false;
	}

	if (HV_SIG || LV_SIG)
	{
		AUX = true;
	}
	else if (TEMP2)
	{
		AUX = false;
	}

	TEMP1 = POWER2 && (LV || HV);
	POWER_SUPPLY = SIG3 || TEMP1;

	if (POWER_SUPPLY && batt->Current() > 150.0)
	{
		OC = true;
	}
	else if (SIG3)
	{
		OC = false;
	}
	if (POWER_SUPPLY)
	{
		if (!RC && batt->Current() < -10.0)
		{
			RC = true;
		}
		else if (RC && batt->Current() > -4.0)
		{
			RC = false;
		}
	}
}

bool LEM_DescentECASector::GetMalfunction()
{
	if (AUX && batt->GetTemp() > 335.928) //145°F
	{
		return true;
	}
	else if (OC || RC)
	{
		return true;
	}
	return false;
}

void LEM_DescentECASector::SaveState(FILEHANDLE scn, char *name_str)
{
	char buffer[100];

	sprintf(buffer, "%d %d %d %d %d %d", AUX, HV, LV, LOR, OC, RC);
	oapiWriteScenario_string(scn, name_str, buffer);
}

void LEM_DescentECASector::LoadState(char *line, int strlen)
{
	int var[6];
	sscanf(line + strlen + 1, "%i %i %i %i %i %i", &var[0], &var[1], &var[2], &var[3], &var[4], &var[5]);

	AUX = (var[0] != 0);
	HV = (var[1] != 0);
	LV = (var[2] != 0);
	LOR = (var[3] != 0);
	OC = (var[4] != 0);
	RC = (var[5] != 0);
}

LEM_AscentECASector::LEM_AscentECASector()
{
	MFC = false;
	AFC = false;
	LOR = false;
	OC = false;
	RC = false;
	lem = NULL;
}

void LEM_AscentECASector::Init(LEM *l, ThreePosSwitch *mfs, ThreePosSwitch *afs, Battery *b)
{
	lem = l;
	MF_SW = mfs;
	AF_SW = afs;
	batt = b;
}

void LEM_AscentECASector::Timestep()
{
	POWER1 = lem->AscentECAContFeeder.Voltage() > SP_MIN_DCVOLTAGE;
	POWER2 = lem->AscentECAMainFeeder.Voltage() > SP_MIN_DCVOLTAGE;
	POWER3 = batt->Voltage() > 5.0; //TBD

	LOR = POWER1 && MF_SW->IsDown();

	TEMP1 = lem->rjb.GetAscentECAOn() && !AFC;
	TEMP2 = MF_SW->IsUp();
	RESET1 = POWER1 && MF_SW->IsDown();
	RESET2 = POWER1 && AF_SW->IsDown();

	if (POWER3 && !MFC && !LOR && (TEMP1 || TEMP2))
	{
		MFC = true;
	}
	else if (MFC && RESET1)
	{
		MFC = false;
	}
	if (POWER3 && !AFC && AF_SW->IsUp())
	{
		AFC = true;
	}
	else if (AFC && RESET2)
	{
		AFC = false;
	}

	POWER4 = POWER2 && (AFC || MFC);
	POWER_SUPPLY = POWER4 || RESET1 || RESET2;

	if (POWER_SUPPLY && batt->Current() > 150.0)
	{
		OC = true;
	}
	else if (MF_SW->IsDown())
	{
		OC = false;
	}
	if (POWER_SUPPLY)
	{
		if (!RC && batt->Current() < -10.0)
		{
			RC = true;
		}
		else if (RC && batt->Current() > -4.0)
		{
			RC = false;
		}
	}
}

bool LEM_AscentECASector::GetMalfunction()
{
	if (batt->GetTemp() > 335.928) //145°F
	{
		return true;
	}
	else if (OC || RC)
	{
		return true;
	}
	return false;
}

void LEM_AscentECASector::SaveState(FILEHANDLE scn, char *name_str)
{
	char buffer[100];

	sprintf(buffer, "%d %d %d %d %d", MFC, AFC, LOR, OC, RC);
	oapiWriteScenario_string(scn, name_str, buffer);
}

void LEM_AscentECASector::LoadState(char *line, int strlen)
{
	int var[5];
	sscanf(line + strlen + 1, "%i %i %i %i %i", &var[0], &var[1], &var[2], &var[3], &var[4]);

	MFC = (var[0] != 0);
	AFC = (var[1] != 0);
	LOR = (var[2] != 0);
	OC = (var[3] != 0);
	RC = (var[4] != 0);
}

LEM_ECA::LEM_ECA()
{

}

LEM_DescentECA::LEM_DescentECA()
{

}

void LEM_DescentECA::Init(LEM *l, Battery *b1, Battery *b2, ThreePosSwitch *hv_sw1, ThreePosSwitch *lv_sw1, ThreePosSwitch *hv_sw2, ThreePosSwitch *lv_sw2, int inp)
{
	SectA.Init(l, hv_sw1, lv_sw1, b1, inp);
	SectB.Init(l, hv_sw2, lv_sw2, b2, inp);

	ECAChannelA.Init(l, b1, &SectA);
	ECAChannelB.Init(l, b2, &SectB);
}

void LEM_DescentECA::Timestep(double dt)
{
	SectA.Timestep();
	SectB.Timestep();
}

void LEM_DescentECA::SystemTimestep(double dt)
{

}

void LEM_DescentECA::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	oapiWriteLine(scn, start_str);
	SectA.SaveState(scn, "BAT1");
	SectB.SaveState(scn, "BAT2");
	oapiWriteLine(scn, end_str);
}

void LEM_DescentECA::LoadState(FILEHANDLE scn, char *end_str)
{
	char *line;
	int dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp(line, "BAT1", 4)) {
			SectA.LoadState(line, 4);
		}
		else if (!strnicmp(line, "BAT2", 4)) {
			SectB.LoadState(line, 4);
		}
	}
}

LEM_AscentECA::LEM_AscentECA()
{

}
void LEM_AscentECA::Init(LEM *l, ThreePosSwitch *mfs, ThreePosSwitch *afs, Battery *b)
{
	SectA.Init(l, mfs, afs, b);
	MFChannel.Init(l, b, SectA.GetMFCPointer());
	AFChannel.Init(l, b, SectA.GetAFCPointer());
}

void LEM_AscentECA::Timestep(double dt)
{
	SectA.Timestep();
}

void LEM_AscentECA::SystemTimestep(double dt)
{

}

void LEM_AscentECA::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	oapiWriteLine(scn, start_str);
	SectA.SaveState(scn, "BAT1");
	oapiWriteLine(scn, end_str);
}

void LEM_AscentECA::LoadState(FILEHANDLE scn, char *end_str)
{
	char *line;
	int dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp(line, "BAT1", 4)) {
			SectA.LoadState(line, 4);
		}
	}
}

// BUS TIE BLOCK

LEM_BusFeed::LEM_BusFeed() {
	lem = NULL;
	dc_source_a = NULL;
	dc_source_b = NULL;
}

void LEM_BusFeed::Init(LEM *s, e_object *sra, e_object *srb) {
	lem = s;
	dc_source_a = sra;
	dc_source_b = srb;
	Volts = 0;
}

void LEM_BusFeed::DrawPower(double watts)
{
	power_load += watts;
};

void LEM_BusFeed::UpdateFlow(double dt) {
	//sprintf(oapiDebugString(),"BTO Input = %d Voltage %f Load %f",input,Volts,power_load);
	// Draw power from the source, and retake voltage, etc.

	int csrc = 0;                             // Current Sources Operational
	double PowerDrawPerSource;              // Current to draw, per source
	double A_Volts = 0;
	double A_Amperes = 0;
	double B_Volts = 0;
	double B_Amperes = 0;

	if (dc_source_a != NULL)
	{
		A_Volts = dc_source_a->Voltage();
	}
	if (dc_source_b != NULL)
	{
		B_Volts = dc_source_b->Voltage();
	}

	// Find active sources
	if (A_Volts > 0) {
		csrc++;
	}
	if (B_Volts > 0) {
		csrc++;
	}
	// Compute draw
	if (csrc > 1) {
		PowerDrawPerSource = power_load / 2;
	}
	else {
		PowerDrawPerSource = power_load;
	}

	// Now take power
	if (dc_source_a != NULL && A_Volts > 0) {
		dc_source_a->DrawPower(PowerDrawPerSource);
	}
	if (dc_source_b != NULL && B_Volts > 0) {
		dc_source_b->DrawPower(PowerDrawPerSource);
	}

	// Resupply from source
	if (dc_source_a != NULL) {
		A_Volts = dc_source_a->Voltage();
		A_Amperes = dc_source_a->Current();
	}
	if (dc_source_b != NULL) {
		B_Volts = dc_source_b->Voltage();
		B_Amperes = dc_source_b->Current();
	}
	// Final output
	switch (csrc) {
	case 2: // DUAL
		Volts = (A_Volts + B_Volts) / 2;
		Amperes = A_Amperes + B_Amperes;
		break;
	case 1: // SINGLE
		if (A_Volts > 0) { // Only one (or no) input
			Volts = A_Volts;
			Amperes = A_Amperes;
		}
		else {
			Volts = B_Volts;
			Amperes = B_Amperes;
		}
		break;
	default: // OFF OR OTHER
		Volts = 0;
		Amperes = 0;
		break;
	}

	// if(this == &lem->BTB_CDR_D){ sprintf(oapiDebugString(),"LM_BTO: = Voltages %f %f | Load %f PS %f Output %f V",A_Volts,B_Volts,power_load,PowerDrawPerSource,Volts); }

	// Reset for next pass.
	power_load = 0;
}

// XLUNAR BUS MANAGER OUTPUT SOURCE
LEM_XLBSource::LEM_XLBSource() {
	Volts = 0;
	enabled = true;
}

void LEM_XLBSource::SetVoltage(double v) {
	Volts = v;
}

void LEM_XLBSource::DrawPower(double watts)
{
	power_load += watts;
}

// XLUNAR BUS MANAGER
LEM_XLBControl::LEM_XLBControl() {
	lem = NULL;
}

void LEM_XLBControl::Init(LEM *s) {
	lem = s;
	dc_output.SetVoltage(0);
}

// Depreciated - Don't tie directly
void LEM_XLBControl::DrawPower(double watts)
{
	power_load += watts;
};

void LEM_XLBControl::UpdateFlow(double dt) {
	// If we have no LEM, punt
	if (lem == NULL) { return; }
	// Do we have power from the other side?
	double sVoltage = lem->CSMToLEMPowerSource.Voltage();
	// Is the CSM Power relay latched?
	if (lem->CMPowerToCDRBusRelayA || lem->CMPowerToCDRBusRelayB) { //TBD: two separate power lines
		// Yes, we can put voltage on the CDR bus
		dc_output.SetVoltage(sVoltage);
	}
	else {
		// No, we have no return path, so we have no voltage.
		dc_output.SetVoltage(0);
	}
	// So then, do we have xlunar voltage?
	if (dc_output.Voltage() > 0) {
		// Process load at our feed point
		lem->CSMToLEMPowerSource.DrawPower(dc_output.PowerLoad());
		// sprintf(oapiDebugString(),"Drawing %f watts from CSM",dc_output.PowerLoad());
		dc_output.UpdateFlow(dt); // Shouldn't touch voltage since it has no SRC
	}

};

// CROSS-TIE BALANCER OUTPUT SOURCE
LEM_BCTSource::LEM_BCTSource() {
	Volts = 0;
}

void LEM_BCTSource::SetVoltage(double v) {
	Volts = v;
}

// BUS CROSS-TIE BALANCER
LEM_BusCrossTie::LEM_BusCrossTie() {
	lem = NULL;
	dc_bus_lmp = NULL;
	dc_bus_cdr = NULL;
	lmp_bal_cb = NULL;	lmp_bus_cb = NULL;
	cdr_bal_cb = NULL;	cdr_bus_cb = NULL;
	last_cdr_ld = 0;
	last_lmp_ld = 0;
}

void LEM_BusCrossTie::Init(LEM *s, DCbus *sra, DCbus *srb, CircuitBrakerSwitch *cb1, CircuitBrakerSwitch *cb2, CircuitBrakerSwitch *cb3, CircuitBrakerSwitch *cb4) {
	lem = s;
	dc_bus_lmp = sra;
	dc_bus_cdr = srb;
	lmp_bal_cb = cb1;	lmp_bus_cb = cb2;
	cdr_bal_cb = cb3;	cdr_bus_cb = cb4;
	dc_output_lmp.SetVoltage(0);
	dc_output_cdr.SetVoltage(0);
	last_cdr_ld = 0;
	last_lmp_ld = 0;
}

// Depreciated - Don't tie directly
void LEM_BusCrossTie::DrawPower(double watts)
{
	power_load += watts;
};

void LEM_BusCrossTie::UpdateFlow(double dt) {
	// Voltage, load, load-share-difference
	double cdr_v, cdr_l, cdr_ld;
	double lmp_v, lmp_l, lmp_ld;
	double loadshare;

	lmp_v = lem->BTB_LMP_A.Voltage(); // Measure bus voltages at their A tie point, so we don't get our own output 
	cdr_v = lem->BTB_CDR_A.Voltage();
	lmp_l = dc_bus_lmp->PowerLoad();
	cdr_l = dc_bus_cdr->PowerLoad();

	// If both busses are dead or both CBs on either side are out, the output is dead.
	if ((cdr_v == 0 && lmp_v == 0) ||
		(lmp_bus_cb->GetState() == 0 && lmp_bal_cb->GetState() == 0) ||
		(cdr_bus_cb->GetState() == 0 && cdr_bal_cb->GetState() == 0)) {
		dc_output_lmp.SetVoltage(0);
		dc_output_cdr.SetVoltage(0);
		lem->CDRs28VBus.UpdateFlow(dt);
		lem->LMPs28VBus.UpdateFlow(dt);
		return;
	}

	// Compute load-share and differences.
	if (lmp_v == 0 || cdr_v == 0) {
		// We lost power on one or both busses. Reset the stored load split.
		last_cdr_ld = 0;
		last_lmp_ld = 0;
		// If one bus is powered, but the other is not,
		// we feed the dead bus from the live one.
		lem->CDRs28VBus.UpdateFlow(dt);
		lem->LMPs28VBus.UpdateFlow(dt);
		if (cdr_v == 0) {
			// Draw CDR load from LMP side and equalize voltage
			dc_output_cdr.SetVoltage(lmp_v);
			dc_output_lmp.SetVoltage(0);
			dc_bus_lmp->DrawPower(cdr_l);
			double Draw = cdr_l / lmp_v;
			if (lmp_bus_cb->GetState() > 0) {
				if (Draw > 100) {
					lmp_bus_cb->SetState(0);
				}
				if (lmp_bal_cb->GetState() > 0 && Draw > 60) {
					lmp_bal_cb->SetState(0);
				}
			}
			else {
				if (lmp_bal_cb->GetState() > 0 && Draw > 30) {
					lmp_bal_cb->SetState(0);
				}
			}
		}
		else {
			// Draw LMP load from CDR side and equalize voltage
			dc_output_lmp.SetVoltage(cdr_v);
			dc_output_cdr.SetVoltage(0);
			dc_bus_cdr->DrawPower(lmp_l);
			double Draw = lmp_l / cdr_v;
			if (cdr_bus_cb->GetState() > 0) {
				if (Draw > 100) {
					cdr_bus_cb->SetState(0);
				}
				if (cdr_bal_cb->GetState() > 0 && Draw > 60) {
					cdr_bal_cb->SetState(0);
				}
			}
			else {
				if (cdr_bal_cb->GetState() > 0 && Draw > 30) {
					cdr_bal_cb->SetState(0);
				}
			}
		}
		return;
	}
	else {
		// If both sides are powered, then one side is going to have a higher load
		// than the other. We draw power from the low-load side to feed the high-load side.
		// The higher-load side will probably have the lower voltage.
		loadshare = (lmp_l + cdr_l) / 2;
		cdr_ld = loadshare - cdr_l;
		lmp_ld = loadshare - lmp_l;
	}

	// Are we within tolerance already?
	if ((cdr_ld < 0.000001 && cdr_ld > -0.000001) && (lmp_ld < 0.000001 && lmp_ld > -0.000001)) {
		// In this case, the busses are already balanced.
		// Use whatever numbers we used last time.
		cdr_ld = last_cdr_ld;
		lmp_ld = last_lmp_ld;
		// sprintf(oapiDebugString(),"BCT L: LMP/CDR V %f %f L %f %f | LS %f | DF %f %f",lmp_v,cdr_v,lmp_l,cdr_l,loadshare,lmp_ld,cdr_ld);
	}
	else {
		// Include what we did before
		cdr_ld += last_cdr_ld;
		lmp_ld += last_lmp_ld;
		// Save this for later abuse
		last_cdr_ld = cdr_ld;
		last_lmp_ld = lmp_ld;
		// sprintf(oapiDebugString(),"BCT N: LMP/CDR V %f %f L %f %f | LS %f | DF %f %f",lmp_v,cdr_v,lmp_l,cdr_l,loadshare,lmp_ld,cdr_ld);
	}

	// If this works the load on both sides should be equal, with each bus having half the total load.
	// sprintf(oapiDebugString(),"BCT: LMP/CDR V %f %f L %f %f | LS %f | D %f %f",lmp_v,cdr_v,lmp_l,cdr_l,loadshare,lmp_ld,cdr_ld);

	lem->CDRs28VBus.UpdateFlow(dt);
	lem->LMPs28VBus.UpdateFlow(dt);

	// Transfer power from the higher-voltage side

	// Balance voltage
	// dc_output_cdr.SetVoltage((cdr_v+lmp_v)/2);	
	// dc_output_lmp.SetVoltage((cdr_v+lmp_v)/2);		

	// Transfer load (works both ways)
	dc_bus_cdr->DrawPower(cdr_ld);
	dc_bus_lmp->DrawPower(lmp_ld);
	// Last thing we do is blow CBs on overcurrent.
	// BUS TIE blows at 100 amps, BUS BAL blows at 30 amps, or 60 amps if the TIE breaker is also closed.
	if (cdr_ld > 0) {
		double Draw = cdr_ld / cdr_v;
		if (cdr_bus_cb->GetState() > 0) {
			if (Draw > 100) {
				cdr_bus_cb->SetState(0);
			}
			if (cdr_bal_cb->GetState() > 0 && Draw > 60) {
				cdr_bal_cb->SetState(0);
			}
		}
		else {
			if (cdr_bal_cb->GetState() > 0 && Draw > 30) {
				cdr_bal_cb->SetState(0);
			}
		}
	}
	if (lmp_ld > 0) {
		double Draw = lmp_ld / lmp_v;
		if (lmp_bus_cb->GetState() > 0) {
			if (Draw > 100) {
				lmp_bus_cb->SetState(0);
			}
			if (lmp_bal_cb->GetState() > 0 && Draw > 60) {
				lmp_bal_cb->SetState(0);
			}
		}
		else {
			if (lmp_bal_cb->GetState() > 0 && Draw > 30) {
				lmp_bal_cb->SetState(0);
			}
		}
	}
}


// AC INVERTER

LEM_INV::LEM_INV() {
	lem = NULL;
	dc_input = NULL;
	InvHeat = 0;
	heatloss = 0.0;

	BASE_HLPW[0] = 40.0;	//0W AC output
	BASE_HLPW[1] = 41.0;	//20W
	BASE_HLPW[2] = 42.5;	//40W
	BASE_HLPW[3] = 45.0;	//60W
	BASE_HLPW[4] = 48.0;	//80W
	BASE_HLPW[5] = 51.0;	//100W
	BASE_HLPW[6] = 54.5;	//120W
	BASE_HLPW[7] = 58.5;	//140W
	BASE_HLPW[8] = 63.0;	//160W
	BASE_HLPW[9] = 68.0;	//180W
	BASE_HLPW[10] = 73.0;	//200W
	BASE_HLPW[11] = 78.0;	//220W
	BASE_HLPW[12] = 83.5;	//240W
	BASE_HLPW[13] = 89.0;	//260W
	BASE_HLPW[14] = 94.5;	//280W
	BASE_HLPW[15] = 100.0;	//300W
	BASE_HLPW[16] = 105.5;	//320W
	BASE_HLPW[17] = 111.5;	//340W
	BASE_HLPW[18] = 117.5;	//360W
}

void LEM_INV::Init(LEM *s, h_HeatLoad *invh) {
	lem = s;
	InvHeat = invh;
}

void LEM_INV::DrawPower(double watts)

{
	power_load += watts;
};

void LEM_INV::UpdateFlow(double dt) {

	// Reset these before pass
	Volts = 0;
	Amperes = 0;
	Hertz = 0;
	heatloss = 0;

	if (dc_input != NULL)
	{
		double SourceVoltage = dc_input->Voltage();

		//Rien ne va plus
		if (SourceVoltage < 10.0)	//TBD: Find actual voltage for this
		{
			power_load = 0.0;
			return;
		}

		// First calculate heat loss
		heatloss = get_hlpw(power_load);
		// Then take power from source
		dc_input->DrawPower(power_load + heatloss);  // Add inefficiency
													 // Then supply the bus
		if (SourceVoltage > 24) {				  // Above 24V input
			Volts = 115.0;                    // Regulator supplies 115V
		}
		else {                                // Otherwise
			Volts = SourceVoltage * 4.8;		  // Falls out of regulation
		}                                     // until the load trips the CB
		Amperes = power_load / Volts;           // AC load amps
		Hertz = 400.0;
	}

	// Debug
	/*
	if(dc_input->Voltage() > 0){
	sprintf(oapiDebugString(),"INV: DC V = %f A = %f | AC LOAD = %f V = %f A = %f",
	dc_input->Voltage(),(power_load/dc_input->Voltage()*2.5),power_load,Volts,Amperes);
	}else{
	sprintf(oapiDebugString(),"INV: INPUT V = %f LOAD = %f",dc_input->Voltage(),power_load);
	}
	*/
	// Reset for next pass
	e_object::UpdateFlow(dt);
}

double LEM_INV::get_hlpw(double base_hlpw_factor)
{
	if (base_hlpw_factor < 20.0) return(calc_hlpw_util(base_hlpw_factor, 0));
	if (base_hlpw_factor < 40.0) return(calc_hlpw_util(base_hlpw_factor, 1));
	if (base_hlpw_factor < 60.0) return(calc_hlpw_util(base_hlpw_factor, 2));
	if (base_hlpw_factor < 80.0) return(calc_hlpw_util(base_hlpw_factor, 3));
	if (base_hlpw_factor < 100.0) return(calc_hlpw_util(base_hlpw_factor, 4));
	if (base_hlpw_factor < 120.0) return(calc_hlpw_util(base_hlpw_factor, 5));
	if (base_hlpw_factor < 140.0) return(calc_hlpw_util(base_hlpw_factor, 6));
	if (base_hlpw_factor < 160.0) return(calc_hlpw_util(base_hlpw_factor, 7));
	if (base_hlpw_factor < 180.0) return(calc_hlpw_util(base_hlpw_factor, 8));
	if (base_hlpw_factor < 200.0) return(calc_hlpw_util(base_hlpw_factor, 9));
	if (base_hlpw_factor < 220.0) return(calc_hlpw_util(base_hlpw_factor, 10));
	if (base_hlpw_factor < 240.0) return(calc_hlpw_util(base_hlpw_factor, 11));
	if (base_hlpw_factor < 260.0) return(calc_hlpw_util(base_hlpw_factor, 12));
	if (base_hlpw_factor < 280.0) return(calc_hlpw_util(base_hlpw_factor, 13));
	if (base_hlpw_factor < 300.0) return(calc_hlpw_util(base_hlpw_factor, 14));
	if (base_hlpw_factor < 320.0) return(calc_hlpw_util(base_hlpw_factor, 15));
	if (base_hlpw_factor < 340.0) return(calc_hlpw_util(base_hlpw_factor, 16));

	//340W and higher uses the same data points
	return calc_hlpw_util(base_hlpw_factor, 17);
}

double LEM_INV::calc_hlpw_util(double maxw, int index)
{
	return (BASE_HLPW[index + 1] - BASE_HLPW[index]) / 20.0*(maxw - 20.0*(double)index) + BASE_HLPW[index];
}

void LEM_INV::SystemTimestep(double simdt)
{
	InvHeat->GenerateHeat(heatloss);
}

//Tracking Light Electronics
LEM_TLE::LEM_TLE()
{
	lem = NULL;
	TrackCB = NULL;
	TrackSwitch = NULL;
	TLEHeat = 0;

}

void LEM_TLE::Init(LEM *l, e_object *trk_cb, ThreePosSwitch *tracksw, h_HeatLoad *tleh)
{
	lem = l;
	TrackCB = trk_cb;
	TrackSwitch = tracksw;
	TLEHeat = tleh;
}

bool LEM_TLE::IsPowered()
{
	if (TrackCB->Voltage() > SP_MIN_DCVOLTAGE && TrackSwitch->GetState() == THREEPOSSWITCH_DOWN) {
		return true;
	}
	return false;
}

void LEM_TLE::Timestep(double simdt)
{
	if (IsPowered()) {
		lem->trackLight.active = true;
	}
	else {
		lem->trackLight.active = false;
	}
}

void LEM_TLE::SystemTimestep(double simdt)
{
	if (IsPowered()) {
		TrackCB->DrawPower(120.0);
		TLEHeat->GenerateHeat(120.0);
	}
}

//Docking Lights
LEM_DockLights::LEM_DockLights()
{
	lem = NULL;
	DockSwitch = NULL;
}

void LEM_DockLights::Init(LEM *l, ThreePosSwitch *docksw)
{
	lem = l;
	DockSwitch = docksw;
}

bool LEM_DockLights::IsPowered()
{
	if (lem->lca.GetCompDockVoltage() > 2.0 && DockSwitch->GetState() == THREEPOSSWITCH_UP) {
		return true;
	}
	return false;
}

void LEM_DockLights::Timestep(double simdt)
{
	int i;

	if (IsPowered()) {
		for (i = 0; i < 5; i++) lem->dockingLights[i].active = true;
	}
	else {
		for (i = 0; i < 5; i++) lem->dockingLights[i].active = false;
	}
}

void LEM_DockLights::SystemTimestep(double simdt)
{
	//This will need power draw through and heat generated to the LCA
}

//LIGHTING CONTROL ASSEMBLY

LEM_LCA::LEM_LCA()
{
	lem = NULL;
	CDRAnnunDockCompCB = NULL;
	LMPAnnunDockCompCB = NULL;
	HasDCPower = false;
	LCAHeat = 0;
	AC_power_load = 0;
}

void LEM_LCA::Init(LEM *l, e_object *cdrcb, e_object *lmpcb, h_HeatLoad *lca_h)
{
	lem = l;
	CDRAnnunDockCompCB = cdrcb;
	LMPAnnunDockCompCB = lmpcb;
	LCAHeat = lca_h;
}

void LEM_LCA::DrawDCPower(double watts)
{
	power_load += watts;
};

void LEM_LCA::DrawACPower(double watts)
{
	AC_power_load += watts;
};

void LEM_LCA::UpdateFlow(double dt)
{
	int csrc = 0;
	double PowerDrawPerSource;
	double CDR_Volts = 0;
	double LMP_Volts = 0;

	HasDCPower = false;

	if (!lem->CMPowerToCDRBusRelayA && !lem->CMPowerToCDRBusRelayB && CDRAnnunDockCompCB->Voltage() > SP_MIN_DCVOLTAGE) //TBD: LM/SLA pressure switch
	{
		CDR_Volts = CDRAnnunDockCompCB->Voltage();
		HasDCPower = true;
		csrc++;
	}

	if (LMPAnnunDockCompCB->Voltage() > SP_MIN_DCVOLTAGE)
	{
		LMP_Volts = LMPAnnunDockCompCB->Voltage();
		HasDCPower = true;
		csrc++;
	}

	// Compute draw
	if (csrc > 1) {
		PowerDrawPerSource = power_load / 2.0;
	}
	else {
		PowerDrawPerSource = power_load;
	}

	//sprintf(oapiDebugString(), "%f %f", power_load, AC_power_load);

	if (CDR_Volts > 0) {
		CDRAnnunDockCompCB->DrawPower(PowerDrawPerSource);
	}
	if (LMP_Volts > 0) {
		LMPAnnunDockCompCB->DrawPower(PowerDrawPerSource);
	}

	power_load = 0.0;

	if (lem->NUM_LTG_AC_CB.Voltage() > SP_MIN_ACVOLTAGE)
	{
		lem->NUM_LTG_AC_CB.DrawPower(AC_power_load);
	}

	AC_power_load = 0.0;
}


void LEM_LCA::SystemTimestep(double simdt)
{
	//LCA Heating to be added
}

double LEM_LCA::GetCompDockVoltage()
{
	if (HasDCPower)
	{
		return 5.5;
	}

	return 0.0;
}

double LEM_LCA::GetAnnunVoltage()
{
	if (HasDCPower)
	{
		if (lem->LtgORideAnunSwitch.IsUp())
		{
			return 5.0;
		}
		else
		{
			//2-5V
			return (3.0 / 8.0*lem->LtgAnunNumKnob.GetValue() + 2.0);
		}
	}

	return 0.0;
}

double LEM_LCA::GetAnnunDimPct()
{
	if (GetAnnunVoltage() > 2.0)
	{
		return GetAnnunVoltage() / 5.0;
	}
	return 0.0;
}

double LEM_LCA::GetNumericVoltage()
{
	if (lem->NUM_LTG_AC_CB.Voltage() > SP_MIN_ACVOLTAGE)
	{
		if (lem->LtgORideNumSwitch.IsUp())
		{
			return 115.0;
		}
		else
		{
			//20-110V
			return (90.0 / 8.0*lem->LtgAnunNumKnob.GetValue() + 20.0);
		}
	}

	return 0.0;
}

double LEM_LCA::GetIntegralVoltage()
{
	if (lem->INTGL_LTG_AC_CB.Voltage() > SP_MIN_ACVOLTAGE)
	{
		if (lem->LtgORideIntegralSwitch.IsUp())
		{
			return 75.0;
		}
		else
		{
			//15-75V
			return (60.0 / 8.0*lem->LtgIntegralKnob.GetValue() + 15.0);
		}
	}

	return 0.0;
}

void LEM_LCA::SaveState(FILEHANDLE scn, char *start_str, char *end_str)

{
	oapiWriteLine(scn, start_str);
	papiWriteScenario_bool(scn, "HASDCPOWER", HasDCPower);
	oapiWriteLine(scn, end_str);
}

void LEM_LCA::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	int dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;

		papiReadScenario_bool(line, "HASDCPOWER", HasDCPower);
	}
}

//Utility Lights

LEM_UtilLights::LEM_UtilLights()
{
lem = NULL;
UtlCB = NULL;
CDRSwitch = NULL;
LMPSwitch = NULL;
UtlLtgHeat = 0;
}

void LEM_UtilLights::Init(LEM *l, e_object *utl_cb, ThreePosSwitch *cdr_sw, ThreePosSwitch *lmp_sw, h_HeatLoad *util_h)
{
lem = l;
UtlCB = utl_cb;
CDRSwitch = cdr_sw;
LMPSwitch = lmp_sw;
UtlLtgHeat = util_h;
}

bool LEM_UtilLights::IsPowered()
{
if (UtlCB->Voltage() > SP_MIN_DCVOLTAGE) {
return true;
}
return false;
}

void LEM_UtilLights::Timestep(double simdt)
{
//Can be used to draw lit UTIL Lights
}

void LEM_UtilLights::SystemTimestep(double simdt)
{
	//CDR Utility Lights Dim
	if (IsPowered() && CDRSwitch->GetState() == THREEPOSSWITCH_CENTER) {
		UtlCB->DrawPower(2.2);
		UtlLtgHeat->GenerateHeat(2.178);
	}
	//CDR Utility Lights Bright
	else if (IsPowered() && CDRSwitch->GetState() == THREEPOSSWITCH_DOWN) {
		UtlCB->DrawPower(6.15);
		UtlLtgHeat->GenerateHeat(6.1);
	}	

	//LMP Utility Lights Dim
	if (IsPowered() && LMPSwitch->GetState() == THREEPOSSWITCH_CENTER) {
		UtlCB->DrawPower(1.76);
		UtlLtgHeat->GenerateHeat(1.74);
	}
	//LMP Utility Lights Bright
	else if (IsPowered() && LMPSwitch->GetState() == THREEPOSSWITCH_DOWN) {
		UtlCB->DrawPower(3.3);
		UtlLtgHeat->GenerateHeat(3.267); 
	}
}

//COAS Lights
LEM_COASLights::LEM_COASLights()
{
	lem = NULL;
	COASCB = NULL;
	COASSwitch = NULL;
	COASHeat = 0;
}

void LEM_COASLights::Init(LEM *l, e_object *coas_cb, ThreePosSwitch *coas_sw, h_HeatLoad *coas_h)
{
	lem = l;
	COASCB = coas_cb;
	COASSwitch = coas_sw;
	COASHeat = coas_h;
}

bool LEM_COASLights::IsPowered()
{
	if (COASCB->Voltage() > SP_MIN_DCVOLTAGE) {
		return true;
	}
	return false;
}

void LEM_COASLights::Timestep(double simdt)
{
	//Can be used to draw lit COAS
}

void LEM_COASLights::SystemTimestep(double simdt)
{
	if (IsPowered() && COASSwitch->GetState() != THREEPOSSWITCH_CENTER) {
		COASCB->DrawPower(8.4);
		COASHeat->GenerateHeat(8.4);
	}
}

//Flood Lights
LEM_FloodLights::LEM_FloodLights()
{
	lem = NULL;
	FloodCB = NULL;
	FloodSwitch = NULL;
	LMPRotary = NULL;
	CDRRotary = NULL;
	FloodHeat = 0;
}

void LEM_FloodLights::Init(LEM *l, e_object *flood_cb, ThreePosSwitch *flood_sw, ContinuousRotationalSwitch *pnl_3_rty, ContinuousRotationalSwitch *pnl_5_rty, h_HeatLoad *flood_h)
{
	lem = l;
	FloodCB = flood_cb;
	FloodSwitch = flood_sw;
	LMPRotary = pnl_3_rty;
	CDRRotary = pnl_5_rty;
	FloodHeat = flood_h;
}

bool LEM_FloodLights::IsPowered()
{
	if (FloodCB->Voltage() > SP_MIN_DCVOLTAGE) {
		return true;
	}
	return false;
}

bool LEM_FloodLights::IsHatchOpen()
{
	if (lem->OverheadHatch.IsOpen()) {
		return true;
	}
	return false;
}

double LEM_FloodLights::GetLMPRotaryVoltage()
{
	if (IsPowered() && (IsHatchOpen() || FloodSwitch->GetState() != THREEPOSSWITCH_CENTER))
	{
		return (LMPRotary->GetValue() + 0.6154) / 0.3077;	//Returns 2V-28V, need to check if max dim is actually 2V
	}
	return 0.0;
}

double LEM_FloodLights::GetCDRRotaryVoltage()
{
	if (IsPowered() && (IsHatchOpen() || FloodSwitch->GetState() != THREEPOSSWITCH_CENTER))
	{
		return (CDRRotary->GetValue() + 0.6154) / 0.3077;	//Returns 2V-28V, need to check if max dim is actually 2V
	}
	return 0.0;
}

double LEM_FloodLights::GetALLPowerDraw()	//These lamps are not dimmable
{
	if (IsPowered() && FloodSwitch->GetState() == THREEPOSSWITCH_DOWN)
	{
		return 50.626;  //34 lamps at 1.489W/lamp 
	}
	return 0.0;
}

double LEM_FloodLights::GetOVHDFWDPowerDraw()	//Dimmable CDR and LMP lamps
{
	return (GetLMPRotaryVoltage() + GetCDRRotaryVoltage()) * 0.319;  //Assumes linear scaling, 12 lamps at 1.489W/lamp
}

double LEM_FloodLights::GetPowerDraw()
{
	return (GetOVHDFWDPowerDraw() + GetALLPowerDraw());
}

void LEM_FloodLights::Timestep(double simdt)
{
	//Can be used to light floods
}

void LEM_FloodLights::SystemTimestep(double simdt)
{
	FloodCB->DrawPower(GetPowerDraw());

	//LM8 Handbook Flood heat listed at 24.4W, this needs to be checked
	FloodHeat->GenerateHeat(GetPowerDraw()*0.356);	//Assumes linear relationship between heat and power draw based on maximum at 28V.
}

LEM_PFIRA::LEM_PFIRA()
{
	lem = NULL;
	K1 = false;
	K2 = false;
	K3 = false;
	K4 = false;
	K5 = false;
	K6 = false;
	K7 = false;
	K8 = false;
	K9 = false;
}

void LEM_PFIRA::Init(LEM *l)
{
	lem = l;
}

void LEM_PFIRA::Timestep(double simdt)
{
	if (lem == NULL) return;

	if (lem->PROP_DISP_ENG_OVRD_LOGIC_CB.IsPowered())
	{
		K1 = true;
	}
	else
	{
		K1 = false;
	}
	if (lem->CDR_XPTR_CB.IsPowered())
	{
		K2 = true;
	}
	else
	{
		K2 = false;
	}
	if (lem->THRUST_DISP_CB.IsPowered())
	{
		K3 = true;
	}
	else
	{
		K3 = false;
	}
	if (lem->ECS_DISP_CB.IsPowered())
	{
		K4 = true;
		K5 = true;
		K6 = true;
	}
	else
	{
		K4 = false;
		K5 = false;
		K6 = false;
	}
	if (lem->RCS_B_TEMP_PRESS_DISP_FLAGS_CB.IsPowered())
	{
		K7 = true;
	}
	else
	{
		K7 = false;
	}
	if (lem->RCS_B_PQGS_DISP_CB.IsPowered())
	{
		K8 = true;
	}
	else
	{
		K8 = false;
	}
	if (lem->SE_XPTR_DC_CB.IsPowered())
	{
		K9 = true;
	}
	else
	{
		K9 = false;
	}
}