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
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "lm_channels.h"
#include "LEM.h"
#include "lm_eps.h"

// ELECTRICAL CONTROL ASSEMBLY SUBCHANNEL
LEM_ECAch::LEM_ECAch() {
	lem = NULL;
	dc_source_tb = NULL;
	input = -1; // Flag uninit
}

void LEM_ECAch::Init(LEM *s, e_object *src, int inp) {
	lem = s;
	if (input == -1) { input = inp; }
	dc_source = src;
	Volts = 24;
}

void LEM_ECAch::SaveState(FILEHANDLE scn, char *start_str, char *end_str)

{
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_int(scn, "INPUT", input);
	oapiWriteLine(scn, end_str);
}

void LEM_ECAch::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	int dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp(line, "INPUT", 5)) {
			sscanf(line + 6, "%d", &dec);
			input = dec;
		}
	}
}

void LEM_ECAch::DrawPower(double watts) {
	power_load += watts;
};

void LEM_ECAch::UpdateFlow(double dt) {
	// ECA INPUTS CAN BE PARALLELED, BUT NOT IN THE SAME CHANNEL
	// That is, Battery 1 and 2 can be on at the same time.
	// Draw power from the source, and retake voltage, etc.

	// Take power
	switch (input) {
	case 1: // HI tap
		if (dc_source != NULL) {
			dc_source->DrawPower(power_load); // Draw 1:1
		}
		break;
	case 2: // LO tap
		if (dc_source != NULL) {
			dc_source->DrawPower(power_load*1.06); // Draw 6% more
		}
		break;
	}

	// Resupply from source
	switch (input) {
	case 0: // NULL
		Volts = 0;
		Amperes = 0;
		break;
	case 1: // HV
		if (dc_source != NULL) {
			Volts = dc_source->Voltage();
			Amperes = dc_source->Current();
		}
		break;
	case 2: // LV
		if (dc_source != NULL) {
			Volts = (dc_source->Voltage()*0.85);
			Amperes = dc_source->Current();
		}
		break;
	}

	// Reset for next pass.
	e_object::UpdateFlow(dt);
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
	if (lem->CSMToLEMPowerConnector.csm_power_latch == 1) {
		// Yes, we can put voltage on the CDR bus
		dc_output.SetVoltage(sVoltage);
	}
	else {
		// No, we have no return path, so we have no voltage.
		dc_output.SetVoltage(0);
	}
	// Handle switchery
	switch (lem->CSMToLEMPowerConnector.csm_power_latch) {
	case 1:
		// If the CSM latch is set, keep the descent ECAs off
		lem->ECA_1a.input = 0; lem->ECA_1b.input = 0;
		lem->ECA_2a.input = 0; lem->ECA_2b.input = 0;
		break;
	case -1:
		// If the CSM latch is reset, turn on the LV taps on batteries 1 and 4.
		// And reset the latch to zero
		lem->ECA_1a.input = 2; lem->ECA_1b.input = 0;
		lem->ECA_2a.input = 0; lem->ECA_2b.input = 2;
		lem->CSMToLEMPowerConnector.csm_power_latch = 0;
		break;
	}
	// So then, do we have xlunar voltage?
	if (dc_output.Voltage() > 0) {
		// Process load at our feed point
		lem->CSMToLEMPowerSource.DrawPower(dc_output.PowerLoad());
		// sprintf(oapiDebugString(),"Drawing %f watts from CSM",dc_output.PowerLoad());
		dc_output.UpdateFlow(dt); // Shouldn't touch voltage since it has no SRC
	}

};

void LEM_XLBControl::SaveState(FILEHANDLE scn, char *start_str, char *end_str)

{
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_int(scn, "CSMPOWERLATCH", lem->CSMToLEMPowerConnector.csm_power_latch);
	oapiWriteLine(scn, end_str);
}

void LEM_XLBControl::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	int dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp(line, "CSMPOWERLATCH", 13)) {
			sscanf(line + 14, "%d", &dec);
			lem->CSMToLEMPowerConnector.csm_power_latch = dec;
		}
	}
}

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
	SecInvHeat = 0;
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

void LEM_INV::Init(LEM *s, h_HeatLoad *invh, h_HeatLoad *secinvh) {
	lem = s;
	InvHeat = invh;
	SecInvHeat = secinvh;
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
	InvHeat->GenerateHeat(heatloss / 2.0);
	SecInvHeat->GenerateHeat(heatloss / 2.0);
}

//Tracking Light Electronics

LEM_TLE::LEM_TLE()
{
	lem = NULL;
	TrackCB = NULL;
	TrackSwitch = NULL;
	TLEHeat = 0;
	SecTLEHeat = 0;

}

void LEM_TLE::Init(LEM *l, e_object *trk_cb, ThreePosSwitch *tracksw, h_HeatLoad *tleh, h_HeatLoad *sectleh)
{
	lem = l;
	TrackCB = trk_cb;
	TrackSwitch = tracksw;
	TLEHeat = tleh;
	SecTLEHeat = sectleh;
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
		TLEHeat->GenerateHeat(60.0);
		SecTLEHeat->GenerateHeat(60.0);
	}
}
