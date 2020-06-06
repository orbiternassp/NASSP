/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Coupling Data Unit

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
#include "apolloguidance.h"
#include "cdu.h"
#include "papi.h"

#include <stdlib.h>
#include <math.h>

// The feedback resistors for most of the summing op amp's are 25 kOhm
static double default_resistor = 25.0;

// Both coarse and fine bits are produced by the
// Read Counter Module, on Card Box 462 part1, page 1567

//     B15 B14 B13 B12 B11 B10  B9     B15 B14 B13
// DC1   0   1   1                       1   0   0
// DC2   1   0   1                       0   1   0
// DC3   1   1   1                       0   0   0
// DC4   1   1   0                       0   0   1
// DC5   0   1   1                       0   0   0
// DC6   0   1   0                       0   0   1
// DC7   1   1   1                       1   0   0
// DC8   1   1   0                       1   0   1
// DC9               0
// DC10                  1
// DC11                      1
// DC12                          1
// RSTB  1   0   0

//coarse_bits[i][0] : first mask for DC_{i+1}
//coarse_bits[i][1] : second mask for DC_{i+1}
//coarse_bits[i][2] : first bits for DC_{i+1}
//coarse_bits[i][3] : second bits for DC_{i+1}

static int coarse_bits[13][4] = {
	{ 0x70, 0x70, 0x30, 0x40 },
	{ 0x70, 0x70, 0x50, 0x20 },
	{ 0x70, 0x70, 0x70, 0x00 },
	{ 0x70, 0x70, 0x60, 0x10 },
	{ 0x70, 0x70, 0x30, 0x00 },
	{ 0x70, 0x70, 0x20, 0x10 },
	{ 0x70, 0x70, 0x70, 0x40 },
	{ 0x70, 0x70, 0x60, 0x50 },
	{ 0x08, 0x00, 0x00, 0x00 },
	{ 0x04, 0x00, 0x04, 0x00 },
	{ 0x02, 0x00, 0x02, 0x00 },
	{ 0x01, 0x00, 0x01, 0x00 },
	{ 0x70, 0x00, 0x40, 0x00 }
};

// These resistance values are from Coarse System Electronic
// schematic in Card Box 463, page 709

static double coarse_summing_resistors[4] = {
	27.06, 65.33, 128.15
};

static double coarse_T_resistors[2][3] = {
	{ 4.0,  100, 2.6 }, //R51, R65, R53
	{ 8.0,  100, 2.0 }  //R52, R66, R54
};


// The Schmitt-Trigger limits are taken from the
// CDU specfication in Card Box 461 part1, page 53

static double coarse_ternary_limit = 0.146;  // sin(8.4 degrees)

static double coarse_ambig_cos_limit = -0.5; //cos(120 degrees)

//    B11 B10  B9  B8  B7     B11 B10  B9  B8
// D1       0   0   0               1   1   1
// D2       0   0   1               1   1   0
// D3       1   0   1               0   1   0
// D4       0   1   1               1   0   0
// D5   0   0                   1   1
// D6   0   1                   1   0
// D7   1
// D8   0
// D9       1           0
// D10      1           1
// D11      0
// D12      0           0
// D13      0           1
// D14      1

//fine_bits[i][0] : first mask for S_{i+1}
//fine_bits[i][1] : second mask for S_{i+1}
//fine_bits[i][2] : first bits for S_{i+1}
//fine_bits[i][3] : second bits for S_{i+1}
static int fine_bits[14][4] = {
	{ 0x0E, 0x0E, 0x00, 0x0E },
	{ 0x0E, 0x0E, 0x02, 0x0C },
	{ 0x0E, 0x0E, 0x0A, 0x04 },
	{ 0x0E, 0x0E, 0x06, 0x08 },
	{ 0x18, 0x18, 0x00, 0x18 },
	{ 0x18, 0x18, 0x08, 0x10 },
	{ 0x10, 0x00, 0x10, 0x00 },
	{ 0x10, 0x00, 0x00, 0x00 },
	{ 0x09, 0x00, 0x08, 0x00 },
	{ 0x09, 0x00, 0x09, 0x00 },
	{ 0x08, 0x00, 0x00, 0x00 },
	{ 0x09, 0x00, 0x00, 0x00 },
	{ 0x09, 0x00, 0x01, 0x00 },
	{ 0x08, 0x00, 0x08, 0x00 }
};

// These values are from the schematic of MSA&QR 2010058 revP
// page 40-42 on https://archive.org/stream/apertureCardBox462Part2NARASW_images

//On page 49: R67-R70, R82-R79 (decreasing order)
//On page 40: R48-R51, R55-R52 (decreasing order)
static double fine_quadrant_resistor[4] = {
	128.146, 44.999, 30.067, 25.490
};

static double fine_intercept_resistor = 125.842;

// The fb resistor of the ladder op amp is slightly differing
// from the usual 25kOhm feedback
static double fine_ladder_feedback_resistor = 25.708;

static double fine_ladder_resistor[9][3] = {
	// BIAS addition is wired in with a similar same 3-res "T-circuit"
	{ 10, 240, 0.051 },

	//Page 40
	{ 4,  100, 2.602700 }, //R32, R60, R40
	{ 8,  100, 1.963800 }, //R33, R61, R41
	{ 8,  100, 0.866900 }, // ....
	{ 8,  100, 0.409500 },
	{ 16, 100, 0.399300 },
	{ 16, 100, 0.196800 },
	{ 16, 100, 0.097700 },
	{ 32, 100, 0.097410 }
};

static double fine_ternary_limit_norm = 0.002286;      // sin(0.131 degs)
static double fine_ternary_limit_trun = 0.001134; // sin(0.065 degs) for optics trunnion

static double high_ternary_limit_norm = 0.0384;        // sin(2.2 degs)
static double high_ternary_limit_trun = 0.0192;   // sin(1.1 degs) for optics trunnion

// 1/R of a T-circuit of resistors
// ---==--+--==--
//    R1  |  R2
//        ‖
//        ‖R3
//        |
//  ------+------
//
//  rarr[i][0]=R1,rarr[i][1]=R2, rarr[i][2]=R3
static inline double T_Circuit(double rarr[][3], int i) {
	return (rarr[i][2] / (
		rarr[i][0] * rarr[i][1] +
		rarr[i][1] * rarr[i][2] +
		rarr[i][2] * rarr[i][0]));
}




/*
* flgCsm != 0 : CDU belongs to CSM's IMU or Optics
* flgCsm == 0 : CDU belongs to LM's IMU or RR
* Idx== 0 : IMU inner
* Idx== 1 : IMU middle
* Idx== 2 : IMU outer
* Idx== 3 : Optics or RR trunnion
* Idx== 4 : Optics or RR shaft
*/

//CDU::CDU(ApolloGuidance &comp, int l, int err, bool isicdu) : agc(comp)
CDU::CDU(ApolloGuidance &comp, bool flgCSM, int Idx, int err) :
	agc(comp)
{
	//if (idx<0 || idx>4)
	//	return 0;

	flg_csm = flgCSM;
	idx = Idx;

	loc = RegCDUX + idx;
	err_channel = err;
	if (idx < 3) {
		flg_ICDU = true;
		CDUZeroBit = 4;
		ErrorCounterBit = 5;
	}
	else {
		flg_ICDU = false;
		CDUZeroBit = 0;
		ErrorCounterBit = 1;
	}

	// For most of the CDUs, the fine resolver has 
	// a multipler of 16. However in case of 
	// optics's trunnion CDU the multiplier is 64.
    // To add to the complications, the optics
	// trunnion angle is divided with the factor
	// of 2 before measuring it with the resolver
	// I think this halving is explained by optical
	// reasons: the trunnion LOS angle is doubled
	// by the reflection on the trunnion mirror
	// and we can directly measure the mirror's
	// angle only. Anyway, we make no harm if we
	// use here 64/2=32 for opt trun.

	if (flg_ICDU || !(flg_csm)) {
		flg_coarse_detect = true;
		flg_opttrun = false;
		resolv_mul = 16;
	}
	else {
		flg_coarse_detect = false; //No coarse detection for optics
		flg_opttrun = (idx==3);
		resolv_mul = flg_opttrun ? 32 : 16;
	}

	if (flg_opttrun) {
		fine_ternary_limit = fine_ternary_limit_trun;
		high_ternary_limit = high_ternary_limit_trun;
		read_counter_mask = 3;
	}
	else {
		fine_ternary_limit = fine_ternary_limit_norm;
		high_ternary_limit = high_ternary_limit_norm;
		read_counter_mask = 1;
	}

	int i, k;
	int bits[14];

	//Coarse coeffs
	for (i = 0; i<128; i++) {
		for (k = 0; k<13; k++) {
			bits[k] = ((i & coarse_bits[k][0]) == coarse_bits[k][2]);
			if (coarse_bits[k][1])
				bits[k] = bits[k] ||
				((i & coarse_bits[k][1]) == coarse_bits[k][3]);
			//printf("i=%#04x B%d=%#04x\n", i, k, bits[k]);
		}
		double sCoarse, cCoarse, iceptCoarse;

		if (bits[0])      sCoarse = -1.0 / coarse_summing_resistors[0];
		else if (bits[1]) sCoarse = -1.0 / coarse_summing_resistors[1];
		else if (bits[2]) sCoarse = 1.0 / coarse_summing_resistors[0];
		else if (bits[3]) sCoarse = 1.0 / coarse_summing_resistors[1];

		if (bits[4])      cCoarse = -1.0 / coarse_summing_resistors[1];
		else if (bits[5]) cCoarse = -1.0 / coarse_summing_resistors[0];
		else if (bits[6]) cCoarse = 1.0 / coarse_summing_resistors[1];
		else if (bits[7]) cCoarse = 1.0 / coarse_summing_resistors[0];

		iceptCoarse = 0.0;
		if (bits[8])  iceptCoarse += 1.0 / coarse_summing_resistors[1];
		if (bits[9])  iceptCoarse += -1.0 / coarse_summing_resistors[2];
		if (bits[10]) iceptCoarse += -T_Circuit(coarse_T_resistors, 0);
		if (bits[11]) iceptCoarse += -T_Circuit(coarse_T_resistors, 1);

		coarse_coeffs[i][0] = default_resistor*sCoarse;
		coarse_coeffs[i][1] = default_resistor*cCoarse;
		coarse_coeffs[i][2] = default_resistor*iceptCoarse;

		coarse_rstb[i] = bits[12];
	}

	//Fine coeffs
	for (i = 0; i<32; i++) {
		for (k = 0; k<14; k++) {
			bits[k] = ((i & fine_bits[k][0]) == fine_bits[k][2]);
			if (fine_bits[k][1])
				bits[k] = bits[k] ||
				((i & fine_bits[k][1]) == fine_bits[k][3]);
			//printf("i=%#04x B%d=%#04x Masked1=%#04x\n", i, k, bits[k], i & fine_bits[k][0]);
		}

		int quadidx = 0;
		for (k = 0; k<4; k++)
			if (bits[k]) quadidx = k; //From S1-S4 one and only one is 1

		int ssign = bits[4] ? -1 : 1;  //S5 inverts sin(th)
		int csign = bits[6] ? -1 : 1;  //S7 inverts cos(th)

		double sFineQuad = fine_quadrant_resistor[quadidx];
		double cFineQuad = fine_quadrant_resistor[3 - quadidx];

		//Output of Op Amp A5 is sin(th)*sAmp5+cos(th)*cAmp5
		double sAmp5 = -ssign*default_resistor / sFineQuad;
		double cAmp5 = bits[13] * csign*default_resistor / cFineQuad;

		//Output of Op Amp A6 is sin(th)*sAmp6+cos(th)*cAmp6
		double sAmp6 = bits[10] * ssign*default_resistor / sFineQuad;
		double cAmp6 = -csign*default_resistor / cFineQuad;

		//Output of ladder amp is sin(th)*sLadderAmp+cos(th)*cLadderAmp
		double sLadderAmp = (-bits[10] * sAmp6 - bits[13] * sAmp5)*
			fine_ladder_feedback_resistor / default_resistor;
		double cLadderAmp = (-bits[10] * cAmp6 - bits[13] * cAmp5)*
			fine_ladder_feedback_resistor / default_resistor;


		double sBias = T_Circuit(fine_ladder_resistor, 0)*
			(bits[9] * sAmp5 + bits[12] * sAmp6);
		double cBias = T_Circuit(fine_ladder_resistor, 0)*
			(bits[9] * cAmp5 + bits[12] * cAmp6);
		double sIcept = (bits[8] * sAmp5 + bits[11] * sAmp6) / fine_intercept_resistor;
		double cIcept = (bits[8] * cAmp5 + bits[11] * cAmp6) / fine_intercept_resistor;

		//sin(th) component of MSA w/o ladder addition
		fine_msa_coeffs[i][0] = -default_resistor*(sIcept + sBias + ssign / cFineQuad);
		//cos(th) component of MSA w/o ladder addition
		fine_msa_coeffs[i][1] = -default_resistor*(cIcept + cBias + csign / sFineQuad);

		//sin(th) component of MSA ladder multiplier
		fine_msa_coeffs[i][2] = -default_resistor*sLadderAmp;
		//cos(th) component of MSA ladder multiplier
		fine_msa_coeffs[i][3] = -default_resistor*cLadderAmp;
	}

	for (i = 0; i<256; i++) {
		int rem = i;
		for (k = 8; k>0; k--) {
			if (rem & 1)
				fine_ladder_coeffs[i] +=
				T_Circuit(fine_ladder_resistor, k);
			rem = rem >> 1;
		}
	}

	read_counter = 0;
	pulse_counter = 0;
	cg = 0.0;
	sg = 0.0;
	cg_fine = 0.0;
	sg_fine = 0.0;
	fast_cnt = 0;
	slow_cnt = 0;
	ambig_flipflop = 0;
	flg_ca = 0;
	flg_phase_bug = 0;

	error_counter = 0;
	driver_speed = 0.0;
	flg_zero_CDU = false;
	flg_ca = false;
	flg_eec = false;
	flg_drc = false;
	flg_feed_rc_to_ec = false;
	flg_enable_drive = false;
	flg_enable_alt = false;
}

void CDU::SetAngleDevice(double *pAngle) {
	pangle = pAngle;
}

void CDU::UpdateAngle() {
	double angle = *pangle;
	if (flg_coarse_detect) {
		cg = cos(angle);
		sg = sin(angle);
	}

	cg_fine = cos(angle*resolv_mul);
	sg_fine = sin(angle*resolv_mul);
}


inline int incErrCnt(int errcnt) {
	return errcnt < 0600 ? ++errcnt : 0600;
}

inline int decErrCnt(int errcnt) {
	return errcnt > -0600 ? --errcnt : -0600;
}

// Output of the Main Summing Amp, the result is pratically the fine error
// We need this both for normal read counter management
// and as feedback signal during IMU coarse alignment
inline double CDU::calc_fine_msa() { 
	int fine_highbits, fine_lowbits;

	if (flg_opttrun) {// For optics trun we use one additional bit
		fine_highbits = (read_counter & 0x1F00) >> 8;
		fine_lowbits = read_counter & 0xFF;
	}
	else {
		fine_highbits = (read_counter & 0xF80) >> 7;
		fine_lowbits = (read_counter & 0x7F) << 1;
	}

	double msa_s = fine_msa_coeffs[fine_highbits][0];
	double msa_lad_s = fine_msa_coeffs[fine_highbits][2];
	double msa_c = fine_msa_coeffs[fine_highbits][1];
	double msa_lad_c = fine_msa_coeffs[fine_highbits][3];

	double ladder_coeff = fine_ladder_coeffs[fine_lowbits];
	return
		(msa_s + ladder_coeff*msa_lad_s)*sg_fine +
		(msa_c + ladder_coeff*msa_lad_c)*cg_fine;
}

int CDU::step_12800_pps() {
	if (flg_zero_CDU) {
		read_counter = 0;
		if (cg < coarse_ambig_cos_limit)
			ambig_flipflop = 1;
		return 0;
	}
	
	//int fastcnt = 0, slowcnt = 0;

	int ret = 0;
	if (!flg_drc) {
		if (pulse_counter == 0) {
			fast_cnt = 0;
			slow_cnt = 0;
			if (flg_coarse_detect) { // We do the coarse detection
				int coarseidx = (read_counter & 0xFE00) >> 9;
				if (ambig_flipflop) {//In case of ambiguity when zeroing CDU
					if (coarse_rstb[coarseidx])
						ambig_flipflop = 0;
					else
						fast_cnt = -1;
				}

				if (!fast_cnt) {
					double csa =
						coarse_coeffs[coarseidx][0] * sg +
						coarse_coeffs[coarseidx][1] * cg;
					double iceptcoarse = coarse_coeffs[coarseidx][2];
					if (flg_phase_bug) {
						csa = sqrt(csa*csa + iceptcoarse*iceptcoarse);
						if (iceptcoarse < 0)
							csa = -csa;
					}
					else
						csa = csa + iceptcoarse;

					if (csa > coarse_ternary_limit)
						fast_cnt = 1;
					else if (csa < -coarse_ternary_limit)
						fast_cnt = -1;
					//printf("Coarse idx=%d csa=%lf fastcnt=%d ", coarseidx, csa, fastcnt);
					//if ((idx<3) && !flg_csm) {
					//	char buf[256];
					//	sprintf(buf, "CDU IDX:%d CSM:%s Ang:%7.4lf (%6.2lf deg) Coarse RC:%5d (%6.2lf deg) CSA:%7.4lf CL:%6.4lf fastcnt:%d",
					//		idx,
					//		flg_csm ? "T" : "F",
					//		(*pangle),
					//		(*pangle) / RAD,
					//		read_counter,
					//		read_counter*360.0 / 32768.0,
					//		csa,
					//		coarse_ternary_limit,
					//		fast_cnt
					//	);
					//	oapiWriteLog(buf);
					//}
				}

			}

			if (!fast_cnt) {
				double msa_amp = calc_fine_msa();

				if (msa_amp > high_ternary_limit)
					fast_cnt = 1;
				else if (msa_amp < -high_ternary_limit)
					fast_cnt = -1;
				else if (msa_amp > fine_ternary_limit)
					slow_cnt = 1;
				else if (msa_amp < -fine_ternary_limit)
					slow_cnt = -1;

				//printf("Fine msa=%lf fastcnt=%d slowcnt=%d", msa_amp, fast_cnt, slow_cnt);
			}
			//printf("\n");

		}

		if ((fast_cnt && !flg_ca) ||
			(fast_cnt && flg_ca && ((pulse_counter & 1) == 0)) ||
			(slow_cnt && (pulse_counter == 0))) {
			int cntdir = slow_cnt + fast_cnt;
			read_counter += cntdir;
			if ((cntdir > 0) && ((read_counter & 1) == 0)) {
				setCntrReqP(&agc.vagc, idx + 6);
				ret = 1;
				if (flg_feed_rc_to_ec && ((read_counter & 7) == 0))
					error_counter=decErrCnt(error_counter);
			}
			else if ((cntdir < 0) && ((read_counter & 1) == 1)) {
				setCntrReqM(&agc.vagc, idx + 6);
				ret = -1;
				if (flg_feed_rc_to_ec && ((read_counter & 7) == 7))
					error_counter=incErrCnt(error_counter);
			}

		}
	}

	if (flg_ca && flg_enable_drive) {
		if (flg_ICDU) {			
			driver_speed+=(0.333*error_counter*0.0132 - calc_fine_msa()*46.9/8.0)*57600.0/(12800.0*12800.0);
		}
		else
			driver_speed = driver_speed_gain*error_counter;

		if (driver_speed > driver_speed_limit)
			driver_speed = driver_speed_limit;
		else if (driver_speed < -driver_speed_limit)
			driver_speed = -driver_speed_limit;

		*pangle = (*pangle) + driver_speed;
		UpdateAngle();
	}

	pulse_counter++;
	pulse_counter = pulse_counter & 0xF;
	
	return ret;
}

/*void CDU::Timestep(double simdt)
{
	ChannelValue val12;

	val12 = agc.GetOutputChannel(012);
	if (ZeroCDU == false)
	{
		double delta;

		delta = NewReadCounter - ReadCounter;

		if (delta < 0) {
			while (fabs(fabs(NewReadCounter) - fabs(ReadCounter)) >= CDU_STEP) {
				agc.vagc.Erasable[0][loc]--;
				agc.vagc.Erasable[0][loc] &= 077777;
				ReadCounter -= CDU_STEP;
			}
		}
		if (delta > 0) {
			while (fabs(fabs(NewReadCounter) - fabs(ReadCounter)) >= CDU_STEP) {
				agc.vagc.Erasable[0][loc]++;
				agc.vagc.Erasable[0][loc] &= 077777;
				ReadCounter += CDU_STEP;
			}
		}
	}
	

	//if (idx == 3)
	//	sprintf(oapiDebugString(), "ReadCounter: %hu fastcnt: %d Reg035: %hu", read_counter, fast_cnt, agc.vagc.Erasable[0][035]);

	//sprintf(oapiDebugString(), "ReadCounter %f NewReadCounter %f ZeroCDU %d CDUZeroBit %d", ReadCounter*DEG, NewReadCounter*DEG, ZeroCDU, CDUZeroBit);
	//sprintf(oapiDebugString(), "ReadCounter %f ErrorCounter %d ErrorCounterEnabled %d", ReadCounter*DEG, ErrorCounter, ErrorCounterEnabled);
}*/

void CDU::ProcessChannel12(ChannelValue val)
{
	if (val[CDUZeroBit] == 1)
		flg_zero_CDU = true;
	else
		flg_zero_CDU = false;

	if (val[ErrorCounterBit] == 1) {
		if (!flg_eec) {
			error_counter = 0;
			flg_eec = true;
		}
	}
	else {
		error_counter = 0;
		flg_eec = false;
	}

	if (flg_ICDU) {
		flg_enable_alt = ((val[8] == 1) && flg_csm);
		if (val[3]) {
			if (!flg_ca) {
				flg_ca = true;
				driver_speed = 0.0;
			}
		}
		else {
			if (flg_ca) {
				flg_ca = false;
				driver_speed = 0.0;
			}
		}
		flg_feed_rc_to_ec = flg_ca;
		flg_drc = (!flg_eec && flg_ca) | flg_zero_CDU;
	}
	else {
		flg_enable_alt = (val[7] == 1);
		flg_ca = (flg_eec && (!flg_enable_alt));
		flg_feed_rc_to_ec = flg_csm ? flg_ca : false;  // No read counter feed for RR
		flg_drc = false;
	}
}

void CDU::ProcessErrorChannel(ChannelValue val){
	//char buf[256];
	//int ival = val.to_ulong();
	//sprintf(buf, "CDU IDX:%d Val:%06o Errcnt:%4d eec:%s drc:%s ca:%s",
	//	idx, ival, error_counter,
	//	flg_eec ? "true" : "false",
	//	flg_drc ? "true" : "false",
	//	flg_ca ? "true" : "false");
	// oapiWriteLog(buf);
	if (flg_eec) {
		int delta = val.to_ulong();
		switch (delta) {
		case 015:
			error_counter = incErrCnt(error_counter);
			break;
		case 016:
			error_counter = decErrCnt(error_counter);
			break;
		default:
			break;
		}
	}
	//sprintf(oapiDebugString(), "ReadCounter %f ErrorCounter %o Bit %d", ReadCounter*DEG, ErrorCounter, val12[ErrorCounterBit] == 1);
}
int CDU::GetReadCounter() {
	int ret = read_counter;
	return ret;
}
int CDU::GetErrorCounter()
{ 
	if (flg_eec)
		return error_counter;
	else
		return 0;
}

int CDU::GetAltOutput()
{
	if (flg_enable_alt)
		return error_counter;
	else
		return 0;
}

void CDU::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	int irc = read_counter;
	oapiWriteScenario_int(scn, "  READCOUNTER", irc);
	oapiWriteScenario_int(scn, "  PULSECOUNTER", pulse_counter);
	///papiWriteScenario_double(scn, "NEWREADCOUNTER", NewReadCounter);
	papiWriteScenario_double(scn, "  COSINEANGLE", cg);
	papiWriteScenario_double(scn, "  SINEANGLE", sg);
	papiWriteScenario_double(scn, "  FINECOSINEANGLE", cg_fine);
	papiWriteScenario_double(scn, "  FINESINEANGLE", sg_fine);
	oapiWriteScenario_int(scn, "  FASTCOUNT", fast_cnt);
	oapiWriteScenario_int(scn, "  SLOWCOUNT", slow_cnt);
	oapiWriteScenario_int(scn, "  AMBIGFLIPFLOP", ambig_flipflop);
	papiWriteScenario_bool(scn, "  COARSEALIGN", flg_ca);
	papiWriteScenario_bool(scn, "  ZEROCDU", flg_zero_CDU);
	papiWriteScenario_bool(scn, "  PHASEBUG", flg_phase_bug);

	oapiWriteScenario_int(scn, "  ERRORCOUNTER", error_counter);
	papiWriteScenario_double(scn, "  DRIVERSPEED", driver_speed);
	//	papiWriteScenario_bool(scn, "ZEROCDU", ZeroCDU);
	papiWriteScenario_bool(scn, "  ENABLEERRORCOUNTER", flg_eec);
	papiWriteScenario_bool(scn, "  DISABLEREADCOUNTER", flg_drc);
	papiWriteScenario_bool(scn, "  FEEDRCTOEC", flg_feed_rc_to_ec);
	papiWriteScenario_bool(scn, "  ENABLEDRIVE", flg_enable_drive);
	papiWriteScenario_bool(scn, "  ENABLEALT", flg_enable_alt);

	oapiWriteLine(scn, end_str);
}

void CDU::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		int irc;
		if(papiReadScenario_int(line, "READCOUNTER", irc))
			read_counter = irc;
		papiReadScenario_int(line, "PULSECOUNTER", pulse_counter);
		papiReadScenario_double(line, "COSINEANGLE", cg);
		papiReadScenario_double(line, "SINEANGLE", sg);
		papiReadScenario_double(line, "FINECOSINEANGLE", cg_fine);
		papiReadScenario_double(line, "FINESINEANGLE", sg_fine);
		papiReadScenario_int(line, "FASTCOUNT", fast_cnt);
		papiReadScenario_int(line, "SLOWCOUNT", slow_cnt);
		papiReadScenario_int(line, "AMBIGFLIPFLOP", ambig_flipflop);
		papiReadScenario_bool(line, "COARSEALIGN", flg_ca);
		papiReadScenario_bool(line, "ZEROCDU", flg_zero_CDU);
		papiReadScenario_bool(line, "PHASEBUG", flg_phase_bug);

		papiReadScenario_int(line, "ERRORCOUNTER", error_counter);
		papiReadScenario_double(line, "DRIVERSPEED", driver_speed);
		papiReadScenario_bool(line, "ENABLEERRORCOUNTER", flg_eec);
		papiReadScenario_bool(line, "DISABLEREADCOUNTER", flg_drc);
		papiReadScenario_bool(line, "FEEDRCTOEC", flg_feed_rc_to_ec);
		papiReadScenario_bool(line, "ENABLEDRIVE", flg_enable_drive);
		papiReadScenario_bool(line, "ENABLEALT", flg_enable_alt);
	}
}