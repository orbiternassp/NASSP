/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Coupling Data Unit (Header)

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

#include "powersource.h"

#define CDU_STEP 0.000191747598876953125 

// Error counter effect on DAC, V/bit
// It is calculated from the specified value 300mV/deg 
// DAC gain (given e.g. in Apollo 15 Delco) and the 
// 0.044 deg/bit resolution of error counter
#define CDU_DAC_GAIN 0.0132 

class ApolloGuidance;
typedef std::bitset<16> ChannelValue;

class CDU
{
	friend class ApolloGuidance;
public:
	CDU(ApolloGuidance &comp, bool flgCSM, int Idx, int err);
	//void Timestep(double simdt);
	int step_12800_pps();
	void ProcessChannel12(ChannelValue val);
	void ProcessErrorChannel(ChannelValue val);
	void SetAngleDevice(double *pAngle);
	void SetDriverSpeedGain(double speed_gain, double speed_limit);
	void SetDriverAccelGain(double accel_gain, double fine_error_gain, double speed_limit);
	//void SetAngle(double Angle);
	void UpdateAngle();
	int GetReadCounter();
	int GetErrorCounter();
	int GetAltOutput();

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	// phase shift between the 800 Hz resolver signal
	// and the reference voltage caused the overload
	// of the LGC and eventually led to the 1201 and
	// 1202 computer errors. If flg_phase_bug is true
	// then the worst case (90 degrees) phase shift
	// will be taken into account
	bool flg_phase_bug;

	// RR or Optics hw device enables AGC to drive
	// it's angle if it wants to.
	bool flg_enable_drive;

	// Angle device driver's limit.
	// driver_speed_gain is given in units of rad/pulse/bit,
	// driver_speed_max is rad/pulse, both given for
	// 12800 pulse/sec. angle_min and angle_max is given
	// in rads.
	double driver_speed_gain, driver_speed_limit;
protected:
	int loc;
	//Channel for error counter commands
	int err_channel;

	//Output channel bit for CDU Zeroing
	int CDUZeroBit;

	//Output channel bit for error counter enabled
	int ErrorCounterBit;

	double coarse_coeffs[128][3];
	int coarse_rstb[128];
	double fine_msa_coeffs[32][4];
	double fine_ladder_coeffs[256];
	bool flg_ICDU, flg_csm, flg_coarse_detect, flg_opttrun;
	int idx, resolv_mul;
	uint16_t read_counter_mask;
	double fine_ternary_limit, high_ternary_limit;
	double *pangle=NULL;

	uint16_t read_counter;
	int pulse_counter; //counts the 12800 pps pulses
	double cg, sg, cg_fine, sg_fine;
	int fast_cnt, slow_cnt;
	int ambig_flipflop;

	int error_counter;
	double driver_speed;
	bool flg_zero_CDU;
	bool flg_ca; //Coarse Align
	bool flg_eec;  //Enable Error Counter
	bool flg_drc;  //Disable Read Counter, Pulse Y=(!EEC & CA) | CDUZ
	bool flg_feed_rc_to_ec;
	bool flg_enable_alt;  //Enable alternative  output (TVC or XPtr cmds)

	ApolloGuidance &agc;
private:
	inline double calc_fine_msa();
};