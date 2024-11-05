/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: LEM virtual cockpit code

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

// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "lmresource.h"

#include "nasspdefs.h"
#include "nassputils.h"

#ifdef _OPENORBITER
#include <gcCoreAPI.h>
#else
#include <gcConst.h>
#endif

#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "LM_VC_Resource.h"
#include "EmissionListLMVC.h"
#include "Mission.h"

#include "LEM.h"

// ==============================================================
// VC Constants
// ==============================================================

// VC Comp light index
#define LM_VC_COMP_LIGHT_1		0   // RR No track light
#define LM_VC_COMP_LIGHT_2		1   // Glycol light
#define LM_VC_COMP_LIGHT_3		2   // Suit fan light
#define LM_VC_COMP_LIGHT_4		3   // CO2 light
#define LM_VC_COMP_LIGHT_5		4   // H2O sep light
#define LM_VC_COMP_LIGHT_6		5   // DC Bus fault light
#define LM_VC_COMP_LIGHT_7		6   // Bat fault light

// Stage sequence relay lights
#define LM_VC_STG_SEQ_A		    0   // Stage seq relay A
#define LM_VC_STG_SEQ_B		    1   // Stage seq relay B

// VC power failure light index
#define LM_VC_PWRFAIL_LIGHT_1		0   // X-pointer left
#define LM_VC_PWRFAIL_LIGHT_2  	    1   // Thrust
#define LM_VC_PWRFAIL_LIGHT_3		2   // DPS press
#define LM_VC_PWRFAIL_LIGHT_4		3   // RCS press
#define LM_VC_PWRFAIL_LIGHT_5		4   // RCS quantity
#define LM_VC_PWRFAIL_LIGHT_6		5   // ECS press
#define LM_VC_PWRFAIL_LIGHT_7		6   // Glycol
#define LM_VC_PWRFAIL_LIGHT_8		7   // ECS quantity
#define LM_VC_PWRFAIL_LIGHT_9		8   // X-pointer right
#define LM_VC_PWRFAIL_LIGHT_10		20   // Range Rate Tape

// Panel tilt
const double P1_TILT = 7.95581 * RAD;
const double P2_TILT = 7.95581 * RAD;
const double P3_TILT = 35.2509 * RAD;
const double P4_TILT = 45.0663 * RAD;
const double P5_TILT = 80 * RAD;
const double P6_TILT = 80 * RAD;
const double P8_TILT = 71.6 * RAD;
const double P11R1_TILT = 326.84 * RAD;
const double P11R2_TILT = 347.15 * RAD;
const double P11R3_TILT = 0;
const double P11R4_TILT = 7.75 * RAD;
const double P11R5_TILT = 18.44 * RAD;
const double P12_TILT = 73.43 * RAD;
const double P14_TILT = 50.1 * RAD;
const double P16R1_TILT = 28.81 * RAD;
const double P16R2_TILT = 23.59 * RAD;
const double P16R3_TILT = 5.57 * RAD;
const double P16R4_TILT = 0;

// Number of switches on each panel
const int	P1_SWITCHCOUNT = 20;
const int	P2_SWITCHCOUNT = 18;
const int	P3_SWITCHCOUNT = 26;
const int	P4_SWITCHCOUNT = 4;
const int	P5_SWITCHCOUNT = 8;
const int	P6_SWITCHCOUNT = 1;
const int	P8_SWITCHCOUNT = 19;
const int	P12_SWITCHCOUNT = 22;
const int	P14_SWITCHCOUNT = 16;
const int	ORDEAL_SWITCHCOUNT = 6;

// Number of rotaries
const int	 P1_ROTCOUNT = 1;
const int	 P2_ROTCOUNT = 4;
const int	 P3_ROTCOUNT = 5;
const int	 P5_ROTCOUNT = 3;
//const int	 P8_ROTCOUNT = 0;
const int	 P12_ROTCOUNT = 4;
const int	 P14_ROTCOUNT = 1;

// Number of push buttons
const int   P4_PUSHBCOUNT = 19;
const int   P6_PUSHBCOUNT = 16;

// Number of thumbwheels
const int P8_TWCOUNT = 6;
const int P12_TWCOUNT = 8;

// Rotary/Needle rotation axises
const VECTOR3	P1_ROT_AXIS = { 0.00, sin(P1_TILT),-cos(P1_TILT) };
const VECTOR3	P2_ROT_AXIS = { 0.00, sin(P2_TILT),-cos(P2_TILT) };
const VECTOR3	P3_ROT_AXIS = { 0.00, sin(P3_TILT),-cos(P3_TILT) };
const VECTOR3	P5_ROT_AXIS = { 0.00, sin(P5_TILT),-cos(P5_TILT) };
const VECTOR3	P12_ROT_AXIS = { -sin((90 * RAD) - P12_TILT), cos((90 * RAD) - P12_TILT), 0.00 };
const VECTOR3	P14_ROT_AXIS = { -sin((90 * RAD) - P14_TILT), cos((90 * RAD) - P14_TILT), 0.00 };

// Clickspot radius
const double SWITCH = 0.015;
const double ROT = 0.02;

// Switch clickspot offset
const VECTOR3	P1_CLICK = { 0, 0.0011, -0.0078 };
const VECTOR3	P2_CLICK = { 0, 0.0011, -0.0078 };
const VECTOR3	P3_CLICK = { 0, 0.0045, -0.0065 };
const VECTOR3	P4_CLICK = { 0, 0.0056, -0.0056 };
const VECTOR3	P5_CLICK = { 0, 0.0015, -0.0002 };
const VECTOR3	P6_CLICK = { 0, 0.0030, -0.0004 };
const VECTOR3	P8_CLICK = { 0.0005, 0.0015, 0 };
const VECTOR3	P12_CLICK = { -0.0005, 0.0016, 0 };
const VECTOR3	P14_CLICK = { -0.001, 0.0013, 0 };
const VECTOR3	ORDEAL_CLICK = { 0, 0, 0 };
const VECTOR3   RRGYRO_CLICK = { 0.0003, -0.004, -0.0023 };
const VECTOR3   UTIL_LTS_CLICK = { 0.0, -0.0088, 0.0 };

// Number of circuit breakers
const int P11R1_CBCOUNT = 19;
const int P11R2_CBCOUNT = 19;
const int P11R3_CBCOUNT = 19;
const int P11R4_CBCOUNT = 21;
const int P11R5_CBCOUNT = 11;
const int P16R1_CBCOUNT = 16;
const int P16R2_CBCOUNT = 18;
const int P16R3_CBCOUNT = 18;
const int P16R4_CBCOUNT = 19;

// Number of indicator needles (round)
const int P12_NEEDLECOUNT = 3;
const int P14_NEEDLECOUNT = 2;

// Panel 1 switches
const VECTOR3 P1_TOGGLE_POS[P1_SWITCHCOUNT] = {
	{-0.414641, 0.546586, 1.699172}, {-0.414770, 0.496147, 1.692049}, {-0.243260, 0.666534, 1.715854}, {-0.040785, 0.595291, 1.705927}, {-0.040025, 0.547691, 1.699348},
	{-0.039420, 0.496879, 1.692121}, {-0.361350, 0.417043, 1.681146}, {-0.316536, 0.417075, 1.681118}, {-0.274721, 0.417141, 1.681057}, {-0.317088, 0.365357, 1.673783},
	{-0.274707, 0.365293, 1.673774}, {-0.344825, 0.321379, 1.667756}, {-0.304150, 0.321385, 1.667736}, {-0.270978, 0.321355, 1.667717}, {-0.228567, 0.395044, 1.677975},
	{-0.183200, 0.394991, 1.677967}, {-0.227949, 0.313448, 1.666632}, {-0.182563, 0.313463, 1.666498}, {-0.148754, 0.376823, 1.675450}, {-0.131680, 0.320759, 1.667584}
};

// Panel 1 rotaries
const VECTOR3 P1_ROT_POS[P1_ROTCOUNT] = {
	{-0.0721, 0.3602, 1.6730}
};

// Panel 2 switches
const VECTOR3 P2_TOGGLE_POS[P2_SWITCHCOUNT] = {
	{0.045885, 0.676289, 1.717328}, {0.088232, 0.676319, 1.717332}, {0.134322, 0.676356, 1.717329}, {0.176470, 0.676407, 1.717335}, {0.045959, 0.585804, 1.704680},
	{0.088331, 0.585888, 1.704689}, {0.134278, 0.585964, 1.704702}, {0.176613, 0.585796, 1.704680}, {0.045969, 0.509331, 1.693991}, {0.088249, 0.509365, 1.693995},
	{0.134309, 0.509290, 1.693981}, {0.176591, 0.509346, 1.693988}, {0.045998, 0.416488, 1.681014}, {0.131341, 0.415909, 1.680932}, {0.174630, 0.415624, 1.680883},
	{0.174120, 0.361105, 1.673267}, {0.413899, 0.546290, 1.699150}, {0.413992, 0.495853, 1.692103}
};

// Panel 2 rotaries
const VECTOR3 P2_ROT_POS[P2_ROTCOUNT] = {
	{0.0520, 0.3542, 1.6720}, {0.2438, 0.4007, 1.6786}, {0.2439, 0.3275, 1.6683}, {0.3568, 0.3643, 1.6735}
};

// Panel 3 switches
const VECTOR3 P3_TOGGLE_POS[P3_SWITCHCOUNT] = {
	{-0.364743, 0.249154, 1.638}, {-0.371207, 0.202654, 1.60513}, {-0.323886, 0.249078, 1.63795}, {-0.322623, 0.202607, 1.6051}, {-0.263987, 0.184704, 1.59245},
	{-0.113911, 0.251605, 1.63973}, {-0.059532, 0.251537, 1.63968}, {-0.002303, 0.251443, 1.63961}, {-0.113839, 0.204921, 1.60674}, {-0.059438, 0.204979, 1.60678},
	{-0.002292, 0.204943, 1.60675}, {-0.113877, 0.152865, 1.56994}, {-0.05957, 0.152744, 1.56986}, {-0.002788, 0.153104, 1.57011}, {0.111952, 0.248089, 1.63724},
	{0.143863, 0.248164, 1.6373}, {0.176712, 0.248192, 1.63732}, {0.210488, 0.248188, 1.63731}, {0.144625, 0.200656, 1.60372}, {0.178421, 0.200655, 1.60372},
	{0.144484, 0.154658, 1.57121}, {0.178357, 0.154589, 1.57116}, {0.250217, 0.253498, 1.64107}, {0.25025, 0.220473, 1.61772}, {0.381213, 0.206949, 1.60817},
	{0.369014, 0.151772, 1.56917}
};

// Panel 3 rotaries
const VECTOR3 P3_ROT_POS[P3_ROTCOUNT] = {
	{-0.3410, 0.1495, 1.5669},{-0.1935, 0.1602, 1.5751},{0.0633, 0.1604, 1.5751},{0.2777, 0.1519, 1.5688},{0.3172, 0.2211, 1.6177}
};

// Panel 4 switches
const VECTOR3 P4_TOGGLE_POS[P4_SWITCHCOUNT] = {
	{-0.132938, 0.070816, 1.49986}, {-0.132898, 0.015721, 1.44464}, {0.131333, 0.070756, 1.4998}, {0.131208, 0.015729, 1.44465}
};

// Panel 4 push-buttons
const VECTOR3 P4_PUSHB_POS[P4_PUSHBCOUNT] = {
	{-0.068631, 0.008015, 1.4369}, {-0.068244, -0.007669, 1.42117}, {-0.045774, 0.015927, 1.44483}, {-0.045737, 0.000295, 1.4291}, {-0.045635, -0.01525, 1.41354},
	{-0.023046, -0.015110, 1.413551}, {-0.000177, -0.015110, 1.413551}, {0.022441,-0.015110,1.413551}, {-0.022966, 0.000311, 1.429174}, {-0.000208, 0.000311, 1.429174},
	{0.022667, 0.000311, 1.429174}, {-0.023022, 0.015839, 1.444753}, {-0.000197, 0.015839, 1.444753}, {0.022350, 0.015839, 1.444753}, {0.045282, 0.015839, 1.444753},
	{0.045019, 0.000180, 1.428975}, {0.045087, -0.015076, 1.413737}, {0.067903, 0.007901, 1.436722}, {0.067771, -0.007522, 1.421295}
};

// Panel 5 switches
const VECTOR3 P5_TOGGLE_POS[P5_SWITCHCOUNT] = {
{-0.5664, 0.0055, 1.5272}, {-0.5120, 0.0055, 1.5272}, {-0.4727, 0.0055, 1.5272}, {-0.4334, 0.0055, 1.5272}, {-0.5172, -0.0044, 1.4713}, {-0.486466, -0.0044, 1.4713},
{-0.455454, -0.0044, 1.4713}, {-0.424065, -0.0044, 1.4713}
};

// Panel 5 rotaries
const VECTOR3 P5_ROT_POS[P5_ROTCOUNT] = {
	{-0.5856, -0.0153, 1.4168}, {-0.4983, -0.0153, 1.4167}, {-0.4306, -0.0153, 1.4168}
};

// Panel 6 switches
const VECTOR3 P6_TOGGLE_POS[P6_SWITCHCOUNT] = {
{0.478506, -0.016735, 1.41034}
};

// Panel 6 push-buttons
const VECTOR3 P6_PUSHB_POS[P6_PUSHBCOUNT] = {
{0.5432, 0.0012, 1.4746}, {0.5428, -0.0028, 1.4513}, {0.5428, -0.0070, 1.4279}, {0.5670, -0.0070, 1.4281}, {0.5910, -0.0070, 1.4281}, {0.6150, -0.0071, 1.4275},
{0.5669, -0.0029, 1.4512}, {0.5911, -0.0029, 1.4511}, {0.6149, -0.0029, 1.4511}, {0.5668, 0.0014, 1.4751}, {0.5911, 0.0015, 1.4750}, {0.6148, 0.0017, 1.4748},
{0.6391, 0.0012, 1.4747}, {0.6385, -0.0029, 1.4509}, {0.6389, -0.0070, 1.4280}, {0.5914, -0.0112, 1.4039}
};

// Panel 8 switches
const VECTOR3 P8_TOGGLE_POS[P8_SWITCHCOUNT] = {
{-1.0669, 0.0697, 0.9742}, {-1.0669, 0.0697, 1.0143}, {-1.08451, 0.075547, 1.0709}, {-1.08451, 0.075547, 1.1904}, {-1.08451, 0.075547, 1.2312}, {-1.08451, 0.075547, 1.2680},
{-1.03188, 0.058115, 1.1481}, {-1.03188, 0.058115, 1.1904}, {-1.03188, 0.058115, 1.2312}, {-1.03188, 0.058115, 1.2681}, {-0.977557, 0.040107, 1.2681}, {-1.1026, 0.0816, 1.3838},
{-1.1026, 0.0816, 1.4388}, {-1.0147, 0.0525, 1.3839}, {-1.0147, 0.0525, 1.4385}, {-1.10563, 0.08257, 1.4974}, {-1.0668, 0.0697, 1.4967}, {-1.0668, 0.0697, 1.5436},
{-0.977514, 0.040111, 1.5528}
};

// Panel 8 thumbwheels
const VECTOR3 P8_TW_POS[P8_TWCOUNT] = {
{-1.0666, 0.0476, 1.3820}, {-1.0666, 0.0476, 1.4368}, {-1.0311, 0.0361, 1.4949}, {-0.9791, 0.0185, 1.3820}, {-0.9791, 0.0185, 1.4367}, {-0.9791, 0.0185, 1.4949}
};

// Panel 11 circuit breakers
// Row 1
const VECTOR3 P11R1_CB_POS[P11R1_CBCOUNT] = {
{-0.781182, 0.69943, 0.720146}, {-0.781178, 0.699452, 0.751413}, {-0.78121, 0.699379, 0.782766}, {-0.781228, 0.699417, 0.813862}, {-0.7821, 0.7000, 0.8738},
{-0.7821, 0.7000, 0.9051}, {-0.7821, 0.7000, 0.9364}, {-0.7821, 0.7000, 0.9675}, {-0.7821, 0.7000, 0.9991}, {-0.7821, 0.7000, 1.0304},
{-0.7821, 0.7000, 1.0617}, {-0.7821, 0.7000, 1.0928}, {-0.7821, 0.7000, 1.1240}, {-0.7821, 0.7000, 1.1554}, {-0.7821, 0.7000, 1.1867},
{-0.7821, 0.7000, 1.2180}, {-0.7821, 0.7000, 1.2490}, {-0.7821, 0.7000, 1.2805}, {-0.7821, 0.7000, 1.3121}
};

// Row 2
const VECTOR3 P11R2_CB_POS[P11R2_CBCOUNT] = {
{-0.8326, 0.6399, 0.7206}, {-0.8326, 0.6399, 0.7519}, {-0.8326, 0.6399, 0.7831}, {-0.8326, 0.6399, 0.8142}, {-0.8326, 0.6399, 0.8746}, {-0.8326, 0.6399, 0.9057}, {-0.8326, 0.6399, 0.9369},
{-0.8326, 0.6399, 0.9682}, {-0.8326, 0.6399, 0.9994}, {-0.8326, 0.6399, 1.0305}, {-0.8326, 0.6399, 1.0617}, {-0.8326, 0.6399, 1.0927}, {-0.8326, 0.6399, 1.1242},
{-0.8326, 0.6399, 1.1554}, {-0.8326, 0.6399, 1.1865}, {-0.8326, 0.6399, 1.2177}, {-0.8326, 0.6399, 1.2490}, {-0.8326, 0.6399, 1.2802}, {-0.8326, 0.6399, 1.3113}
};

// Row 3
const VECTOR3 P11R3_CB_POS[P11R3_CBCOUNT] = {
{-0.8770, 0.5710, 0.7206},{-0.8770, 0.5710, 0.7519}, {-0.8770, 0.5710, 0.7833}, {-0.8770, 0.5710, 0.8142}, {-0.8770, 0.5710, 0.8744}, {-0.8770, 0.5710, 0.9055},
{-0.8770, 0.5710, 0.9368}, {-0.8770, 0.5710, 0.9680}, {-0.8770, 0.5710, 0.9993}, {-0.8770, 0.5710, 1.0306}, {-0.8770, 0.5710, 1.0617}, {-0.8770, 0.5710, 1.0931},
{-0.8770, 0.5710, 1.1242}, {-0.8770, 0.5710, 1.1554}, {-0.8770, 0.5710, 1.1865}, {-0.8770, 0.5710, 1.2178}, {-0.8770, 0.5710, 1.2490},
{-0.8770, 0.5710, 1.2801}, {-0.8770, 0.5710, 1.3113}
};

// Row 4
const VECTOR3 P11R4_CB_POS[P11R4_CBCOUNT] = {
{-0.9263, 0.4833, 0.7204}, {-0.9263, 0.4833, 0.7517}, {-0.9263, 0.4833, 0.7826}, {-0.9263, 0.4833, 0.8138}, {-0.9263, 0.4833, 0.8737}, {-0.9263, 0.4833, 0.9049},
{-0.9263, 0.4833, 0.9362}, {-0.9263, 0.4833, 0.9672}, {-0.9263, 0.4833, 0.9984}, {-0.9263, 0.4833, 1.0296}, {-0.9263, 0.4833, 1.0608}, {-0.9263, 0.4833, 1.0920},
{-0.9263, 0.4833, 1.1233}, {-0.9263, 0.4833, 1.1545}, {-0.9263, 0.4833, 1.1854}, {-0.9263, 0.4833, 1.2168}, {-0.9263, 0.4833, 1.2479}, {-0.9263, 0.4833, 1.2790},
{-0.9263, 0.4833, 1.3100}, {-0.9263, 0.4833, 1.3415}, {-0.9263, 0.4833, 1.3724}
};

// Row 5
const VECTOR3 P11R5_CB_POS[P11R5_CBCOUNT] = {
{-0.9558, 0.3950, 0.7223}, {-0.9558, 0.3950, 0.7534}, {-0.9558, 0.3950, 0.7844}, {-0.9558, 0.3950, 0.8154}, {-0.9558, 0.3950, 0.8752}, {-0.9558, 0.3950, 0.9062},
{-0.9558, 0.3950, 0.9373}, {-0.9558, 0.3950, 0.9684}, {-0.9558, 0.3950, 0.9995}, {-0.9558, 0.3950, 1.0304}, {-0.9558, 0.3950, 1.0616}
};

// Panel 12 switches
const VECTOR3 P12_TOGGLE_POS[P12_SWITCHCOUNT] = {
{1.0613, 0.0639, 1.5407}, {0.9870, 0.0419, 1.5557}, {1.1029, 0.0763, 1.4994}, {1.1027, 0.0763, 1.4439}, {1.1027, 0.0763, 1.3904}, {1.0209, 0.0519, 1.5002},
{1.0207, 0.0519, 1.4448}, {1.0695, 0.0663, 1.3905}, {1.0919, 0.0730, 1.3526}, {1.0919, 0.0730, 1.3026}, {1.0919, 0.0730, 1.2570}, {1.0919, 0.0730, 1.2091},
{1.0919, 0.0730, 1.1599}, {1.0919, 0.0730, 1.1195}, {1.0395, 0.0574, 1.2910}, {1.0395, 0.0574, 1.2556}, {1.0395, 0.0574, 1.2213}, {1.0395, 0.0574, 1.1862},
{1.0395, 0.0574, 1.1519}, {1.0395, 0.0574, 1.1169}, {0.9806, 0.0399, 1.1178}, {1.0416, 0.0581, 1.0728}
};

// Panel 12 rotaries
const VECTOR3 P12_ROT_POS[P12_ROTCOUNT] = {
{0.9879, 0.0405, 1.0601}, {0.9839, 0.0393, 0.9782}, {0.9948, 0.0425, 0.9119}, {1.0787, 0.0675, 0.9119}
};

// Panel 12 needles
const VECTOR3 P12_NEEDLE_POS[P12_NEEDLECOUNT] = {
{1.09359, 0.072091, 1.05354}, {1.09359, 0.072091, 0.989483}, {1.0338, 0.054291, 1.01673}
};

// Panel 12 thumbwheels
const VECTOR3 P12_TW_POS[P12_TWCOUNT] = {
{1.0691, 0.0429, 1.5005}, {1.0689, 0.0430, 1.4452}, {1.0362, 0.0334, 1.3907}, {0.9843, 0.0186, 1.5001}, {0.9848, 0.0185, 1.4452}, {0.9848, 0.0182, 1.3907},
{0.9842, 0.0186, 1.2568}, {0.9844, 0.0184, 1.1842}
};

// Panel 14 switches
const VECTOR3 P14_TOGGLE_POS[P14_SWITCHCOUNT] = {
{0.9641, 0.3106, 1.2910}, {0.9641, 0.3106, 1.2118}, {0.9960, 0.3373, 1.1471}, {0.9960, 0.3373, 1.1073}, {0.9960, 0.3373, 1.0597}, {0.9960, 0.3373, 1.0200},
{0.9645, 0.3109, 1.1471}, {0.9645, 0.3109, 1.1074}, {0.9645, 0.3109, 1.0598}, {0.9645, 0.3109, 1.0201}, {0.9960, 0.3373, 0.9789}, {0.9831, 0.3265, 0.9271},
{0.9831, 0.3265, 0.8872}, {0.9831, 0.3265, 0.8473}, {0.9831, 0.3265, 0.7995}, {0.9831, 0.3265, 0.7567}
};

// Panel 14 rotaries
const VECTOR3 P14_ROT_POS[P14_ROTCOUNT] = {
{1.0075, 0.3452, 1.2114}
};

// Panel 14 needles
const VECTOR3 P14_NEEDLE_POS[P14_NEEDLECOUNT] = {
{1.02176, 0.357098, 1.39294}, {0.972098, 0.31565, 1.39296}
};

// Panel 16 circuit breakers
// Row 1
const VECTOR3 P16R1_CB_POS[P16R1_CBCOUNT] = {
{0.7723, 0.7006, 1.3166}, {0.7723, 0.7006, 1.2856}, {0.7723, 0.7006, 1.2546}, {0.7723, 0.7006, 1.2235}, {0.7723, 0.7006, 1.1922}, {0.7723, 0.7006, 1.1612},
{0.7723, 0.7006, 1.1301}, {0.7723, 0.7006, 1.0990}, {0.7723, 0.7006, 1.0679}, {0.7723, 0.7006, 1.0368}, {0.7723, 0.7006, 1.0056}, {0.7723, 0.7006, 0.9745},
{0.7723, 0.7006, 0.9434}, {0.7723, 0.7006, 0.9121}, {0.7723, 0.7006, 0.8811}, {0.7723, 0.7006, 0.8213}
};

// Row 2
const VECTOR3 P16R2_CB_POS[P16R2_CBCOUNT] = {
{0.8321, 0.6372, 1.3176}, {0.8321, 0.6372, 1.2864}, {0.8321, 0.6372, 1.2552}, {0.8321, 0.6372, 1.2239}, {0.8321, 0.6372, 1.1927}, {0.8321, 0.6372, 1.1615},
{0.8321, 0.6372, 1.1304}, {0.8321, 0.6372, 1.0992}, {0.8321, 0.6372, 1.0680}, {0.8321, 0.6372, 1.0369}, {0.8321, 0.6372, 1.0057}, {0.8321, 0.6372, 0.9744},
{0.8321, 0.6372, 0.9433}, {0.8321, 0.6372, 0.9120}, {0.8321, 0.6372, 0.8809}, {0.8321, 0.6372, 0.8212}, {0.8321, 0.6372, 0.7899}, {0.8321, 0.6372, 0.7588}
};

// Row 3
const VECTOR3 P16R3_CB_POS[P16R3_CBCOUNT] = {
{0.8730, 0.5736, 1.3179}, {0.8730, 0.5736, 1.2865}, {0.8730, 0.5736, 1.2554}, {0.8730, 0.5736, 1.2242}, {0.8730, 0.5736, 1.1931}, {0.8730, 0.5736, 1.1619},
{0.8730, 0.5736, 1.1307}, {0.8730, 0.5736, 1.0995}, {0.8730, 0.5736, 1.0682}, {0.8730, 0.5736, 1.0370}, {0.8730, 0.5736, 1.0059}, {0.8730, 0.5736, 0.9436},
{0.8730, 0.5736, 0.9124}, {0.8730, 0.5732, 0.8812}, {0.8730, 0.5736, 0.8212}, {0.8730, 0.5736, 0.7901}, {0.8730, 0.5736, 0.7588}, {0.8730, 0.5736, 0.7278}
};

// Row 4
const VECTOR3 P16R4_CB_POS[P16R4_CBCOUNT] = {
{0.9333, 0.4903, 1.3197}, {0.9333, 0.4903, 1.2884}, {0.9333, 0.4903, 1.2571}, {0.9333, 0.4903, 1.2259}, {0.9333, 0.4903, 1.1948}, {0.9333, 0.4903, 1.1634},
{0.9333, 0.4903, 1.1321}, {0.9333, 0.4903, 1.1009}, {0.9333, 0.4903, 1.0696}, {0.9333, 0.4903, 1.0385}, {0.9333, 0.4903, 1.0071}, {0.9333, 0.4903, 0.9760},
{0.9333, 0.4903, 0.9447}, {0.9333, 0.4903, 0.9135}, {0.9333, 0.4903, 0.8822}, {0.9333, 0.4903, 0.8223}, {0.9333, 0.4903, 0.7910}, {0.9333, 0.4903, 0.7598},
{0.9333, 0.4903, 0.7285}
};

// ORDEAL

const VECTOR3 ORDEAL_TOGGLE_POS[ORDEAL_SWITCHCOUNT] = {
{-1.0594, 0.2094, 0.7269}, {-1.0594, 0.2095, 0.7876}, {-1.0594, 0.2094, 0.8391}, {-1.0141, 0.1945, 0.7268}, {-1.0140, 0.1944, 0.8128}, {-1.0140, 0.1944, 0.8390}
};

const VECTOR3 ORDEAL_RotLocation = { -1.0319, 0.2112, 0.8968 };

//ECS Panels

const VECTOR3 H20SepHandleLocation = { 0.3153, 0.2264, -0.2932 };
const VECTOR3 PB_Vent1Location = { 0.3382, 0.1455, 0.2815 };
const VECTOR3 PB_Vent2Location = { 0.3382, 0.2936, -0.2550 };
const VECTOR3 Rot_AscO2_1Location = {0.5708, 0.2834, 0.5777};
const VECTOR3 Rot_AscO2_2Location = {0.5004, 0.2868, 0.5682};
const VECTOR3 Rot_CabinGasReturnLocation = {0.3689, 0.5241, 0.0807};
const VECTOR3 Rot_CabinRepressLocation = {0.6029, 0.3542, 0.5816};
const VECTOR3 Rot_DesO2Location = {0.6461, 0.2772, 0.5873};
const VECTOR3 Rot_PlssFillLocation = {0.5022, 0.3589, 0.5686};
const VECTOR3 Rot_PressRegALocation = {0.4255, 0.3470, 0.5588};
const VECTOR3 Rot_PressRegBLocation = {0.4226, 0.2565, 0.5585};
const VECTOR3 Rot_SuitCircuitReliefLocation = {0.3686, 0.5016, 0.2690};
const VECTOR3 SuitGasDiverterHandleLocation = { 0.4473, 0.6010, 0.5533 };
const VECTOR3 Sw_CanisterSelLocation = { 0.3544, 0.3645, 0.1312 };
const VECTOR3 Rot_SuitIsolCdrLocation = { 0.3457, 0.1616, 0.6088 };
const VECTOR3 Rot_SuitIsolLmpLocation = { 0.6174, 0.1719, 0.5947 };
const VECTOR3 Sw_ActOvrdCdrLocation = { 0.3653, 0.1552, 0.5451 };
const VECTOR3 Sw_ActOvrdLmpLocation = { 0.5787, 0.1753, 0.5402 };
const VECTOR3 Rot_Canister1Location = { 0.3077, 0.1679, 0.1246 };
const VECTOR3 Rot_Canister2Location = { 0.3105, 0.2269, -0.1261 };
const VECTOR3 Rot_DesH2OLocation = {0.4417, -0.1269, 0.6370};
const VECTOR3 Rot_LGCLocation = {0.2611, -0.2251, 0.6134};
const VECTOR3 Rot_PriEvapFlow1Location = {0.4584, -0.2097, 0.6406};
const VECTOR3 Rot_PriEvapFlow2Location = {0.4652, -0.0533, 0.6232};
const VECTOR3 Rot_SecEvapFlowLocation = {0.5511, -0.0462, 0.6229};
const VECTOR3 Rot_SuitTempLocation = {0.4533, -0.4028, 0.6826};
const VECTOR3 Rot_WaterTankSelectorLocation = { 0.5189, -0.2687, 0.6631 };
const VECTOR3 Rot_AscH2OLocation = {0.3876, -0.0204, 0.5144};

// Hatches
const VECTOR3 UpperHatchLocation = { -0.0013, 0.9817, 0.0000 };
const VECTOR3 UpperHatchHandleLocation = { -0.0021, 0.9917, 0.3372 };
const VECTOR3 UpperHatchReliefValveLocation = { 0.1467, 1.0035, 0.1677 };

const VECTOR3 FwdHatchHandleLocation = { -0.3440, -0.5710, 1.5847 };
const VECTOR3 FwdHatchReliefValveLocation = { 0.2370, -0.5113, 1.5987 };
const VECTOR3 FwdHatchInnerLocation = { -0.0164, -0.5784, 1.6599 };

// Window Shades
const VECTOR3 WindowShadesLocation = { -0.48, 0.69, 1.8 };

// Utility Lights
const VECTOR3 UtilityLights_CDRLocation = { 0.0162, 1.0318, 0.8908 };
const VECTOR3 UtilityLights_LMPLocation = { 0.1030, 1.0318, 0.8908 };

// AOT
const VECTOR3 Sw_RRGyroLocation = { -0.1557, 0.7949, 1.3874 };
const VECTOR3 AOT_ShaftSelectorLocation = { 0.0640, 0.8800, 1.4792 };

// Subtracted from total material count to find L01 location.

void LEM::JostleViewpoint(double amount)

{
	double j = ((double)((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetx += j;

	j = ((double)((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsety += j;

	j = ((double)((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetz += j;

	if (ViewOffsetx > 0.10)
		ViewOffsetx = 0.10;
	if (ViewOffsetx < -0.10)
		ViewOffsetx = -0.10;

	if (ViewOffsety > 0.10)
		ViewOffsety = 0.10;
	if (ViewOffsety < -0.10)
		ViewOffsety = -0.10;

	if (ViewOffsetz > 0.05)
		ViewOffsetz = 0.05;
	if (ViewOffsetz < -0.05)
		ViewOffsetz = -0.05;

	SetView();
}

void LEM::VCFreeCam(VECTOR3 dir, bool slow)
{
	//dir is always in Orbiter's vessel XYZ reference frame
	//in SetView() the shift is adjusted to local viewpoint reference frame to make is seem 'natural' from the observer's viewpoint

	double simdt = oapiGetSimStep() / oapiGetTimeAcceleration();

	if (slow == false) {
		vcFreeCamx += dir.x * vcFreeCamSpeed * simdt;
		vcFreeCamy += dir.y * vcFreeCamSpeed * simdt;
		vcFreeCamz += dir.z * vcFreeCamSpeed * simdt;
	}
	else {
		vcFreeCamx += dir.x * vcFreeCamSpeed * simdt * 0.25;
		vcFreeCamy += dir.y * vcFreeCamSpeed * simdt * 0.25;
		vcFreeCamz += dir.z * vcFreeCamSpeed * simdt * 0.25;
	}

	//Make sure the camera isn't offset to far
	if (vcFreeCamx > vcFreeCamMaxOffset) { vcFreeCamx = vcFreeCamMaxOffset; }
	else if (vcFreeCamx < -vcFreeCamMaxOffset) { vcFreeCamx = -vcFreeCamMaxOffset; }

	if (vcFreeCamy > vcFreeCamMaxOffset) { vcFreeCamy = vcFreeCamMaxOffset; }
	else if (vcFreeCamy < -vcFreeCamMaxOffset) { vcFreeCamy = -vcFreeCamMaxOffset; }

	if (vcFreeCamz > vcFreeCamMaxOffset) { vcFreeCamz = vcFreeCamMaxOffset; }
	else if (vcFreeCamz < -vcFreeCamMaxOffset) { vcFreeCamz = -vcFreeCamMaxOffset; }

	SetView();
}

void LEM::SetView() {

	VECTOR3 v;

	VECTOR3 ofs;
	if (stage == 2) {
		ofs = { 0.0, 0.0, 0.0 };
	} else {
		ofs = { 0.0, 1.75, 0.0 };
	}

	//
	// Set camera offset
	//
	if (InVC) {
		switch (viewpos) {
		case LMVIEW_CDR:
			v = _V(-0.45, -0.07, 1.25) + ofs;
			SetCameraDefaultDirection(_V(0.00, -sin(P1_TILT), cos(P1_TILT)));
			v.x += vcFreeCamx;
			v.y += (cos(P1_TILT) * vcFreeCamy) + (-sin(P1_TILT) * vcFreeCamz);
			v.z += (sin(P1_TILT) * vcFreeCamy) + (cos(P1_TILT) * vcFreeCamz);
			break;

		case LMVIEW_LMP:
			v = _V(0.45, -0.07, 1.25) + ofs;
			SetCameraDefaultDirection(_V(0.00, -sin(P2_TILT), cos(P2_TILT)));
			v.x += vcFreeCamx;
			v.y += (cos(P2_TILT) * vcFreeCamy) + (-sin(P2_TILT) * vcFreeCamz);
			v.z += (sin(P2_TILT) * vcFreeCamy) + (cos(P2_TILT) * vcFreeCamz);
			break;

		case LMVIEW_LPD:
			v = _V(-0.58, -0.15, 1.40) + ofs;
			SetCameraDefaultDirection(_V(0.0, -sin(VIEWANGLE * RAD), cos(VIEWANGLE * RAD)));
			//v.x += vcFreeCamx;
			//v.y += (cos(VIEWANGLE * RAD) * vcFreeCamy) + (-sin(VIEWANGLE * RAD) * vcFreeCamz);
			//v.z += (sin(VIEWANGLE * RAD) * vcFreeCamy) + (cos(VIEWANGLE * RAD) * vcFreeCamz);
			break;

		case LMVIEW_DSKY:
			v = _V(0.0, -0.4, 1.2) + ofs;
			SetCameraDefaultDirection(_V(0.00, -sin(P3_TILT), cos(P3_TILT)));
			v.x += vcFreeCamx;
			v.y += (cos(P3_TILT) * vcFreeCamy) + (-sin(P3_TILT) * vcFreeCamz);
			v.z += (sin(P3_TILT) * vcFreeCamy) + (cos(P3_TILT) * vcFreeCamz);
			break;

		case LMVIEW_CBLEFT:
			v = _V(-0.55, -0.22, 1.05) + ofs;
			SetCameraDefaultDirection(_V(-1.0, 0.0, 0.0));
			v.x += -vcFreeCamz;
			v.y += vcFreeCamy;
			v.z += vcFreeCamx;
			break;

		case LMVIEW_CBRIGHT:
			v = _V(0.55, -0.22, 1.05) + ofs;
			SetCameraDefaultDirection(_V(1.0, 0.0, 0.0));
			v.x += vcFreeCamz;
			v.y += vcFreeCamy;
			v.z += -vcFreeCamx;
			break;

		case LMVIEW_AOT:
			v = _V(0.0, 0.05, 1.12) + ofs;
			SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			v.x += vcFreeCamx;
			v.y += vcFreeCamy;
			v.z += vcFreeCamz;
			break;

		case LMVIEW_ECS:
			v = _V(-0.15, -0.42, 0.10) + ofs;
			SetCameraDefaultDirection(_V(1.0, 0.0, 0.0));
			v.x += vcFreeCamz;
			v.y += vcFreeCamy;
			v.z += -vcFreeCamx;
			break;

		case LMVIEW_ECS2:
			v = _V(0.45, -0.35, 1.0) + ofs;
			SetCameraDefaultDirection(_V(0.0, 0.0, -1.0));
			v.x += -vcFreeCamx;
			v.y += vcFreeCamy;
			v.z += -vcFreeCamz;
			break;

		case LMVIEW_FWDHATCH:
			v = _V(0.0, -1.3, 1.1) + ofs;
			SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			v.x += vcFreeCamx;
			v.y += vcFreeCamy;
			v.z += vcFreeCamz;
			break;

		case LMVIEW_OVHDHATCH:
			v = _V(0.0, -0.3, 0.1) + ofs;
			SetCameraDefaultDirection(_V(0.0, 1.0, 0.0), 180 * RAD);
			v.x += -vcFreeCamx;
			v.y += vcFreeCamz;
			v.z += vcFreeCamy;
			break;

		case LMVIEW_RDVZWIN:
			v = _V(-0.58, -0.05, 1.004) + ofs;
			SetCameraDefaultDirection(_V(0.0, 1.0, 0.0));
			v.x += vcFreeCamx;
			v.y += vcFreeCamz;
			v.z += -vcFreeCamy;
			break;

		}

		v.x += ViewOffsetx;
		v.y += ViewOffsety;
		v.z += ViewOffsetz;

	} else {

		if(InPanel)
		{
			switch(PanelId)
			{
				case LMPANEL_MAIN:
					v =_V(0, 0.15, 1.26) + ofs;
					break;

				case LMPANEL_RIGHTWINDOW:
					v =_V(0.576, 0.15, 1.26) + ofs;
					break;

				case LMPANEL_LEFTWINDOW:
					v =_V(-0.576, 0.15, 1.26) + ofs;
					break;

				case LMPANEL_LPDWINDOW:
					v = _V(-0.58, -0.15, 1.40) + ofs;
					break;

				case LMPANEL_RNDZWINDOW:
					v =_V(-0.598, 0.15, 1.106) + ofs;
					break;

				case LMPANEL_LEFTPANEL:
					v =_V(-0.576, 0.15, 1.26) + ofs;
					break;

				case LMPANEL_AOTVIEW:
					v =_V(0, 1.13, 1.26) + ofs;
					break;

				case LMPANEL_AOTZOOM:
					v =_V(0, 1.13, 1.26) + ofs;
					break;

				case LMPANEL_DOCKVIEW:
					v =_V(-0.598, 0.15, 1.106) + ofs;
					break;

				case LMPANEL_LEFTZOOM:
					v =_V(-0.576, 0.15, 1.26) + ofs;
					break;

				case LMPANEL_UPPERHATCH:
					v =_V(0, -0.55, 0) + ofs;
					SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
					oapiCameraSetCockpitDir(180 * RAD, 0);
					break;

				case LMPANEL_FWDHATCH:
					v =_V(0, -1.4, 1.5) + ofs;
					break;
			}
		}
		else
		{
			v =_V(0, 0, 0);
		}
	}

	SetCameraOffset(v - currentCoG);

	//
	// Change FOV for the LPD window and AOT zoom
	//
	if (InPanel && PanelId == LMPANEL_LPDWINDOW) {
	   // if this is the first time we've been here, save the current FOV
		if (InFOV) {
			SaveFOV = oapiCameraAperture();
			InFOV = false;
		}
		//set FOV to 60 degrees (except for lower resolutions)
		DWORD w, h;
		oapiGetViewportSize(&w, &h);
		oapiCameraSetAperture(atan(tan(RAD*30.0)*min(h / 1080.0, 1.0)));
	}
	else if (InPanel && PanelId == LMPANEL_AOTZOOM) {
		// if this is the first time we've been here, save the current FOV
		if (InFOV) {
			SaveFOV = oapiCameraAperture();
			InFOV = false;
		}
		//set FOV to 60 degrees (except for lower resolutions)
		DWORD w, h;
		oapiGetViewportSize(&w, &h);
		oapiCameraSetAperture(atan(tan(RAD*30.0)*min(h / 1050.0, 1.0)));
	}
    else {
		if(InFOV == false) {
			oapiCameraSetAperture(SaveFOV);
			InFOV = true;
		}
	}
}

bool LEM::clbkLoadVC (int id)
{
	// Set VC view to last saved position
	if (FirstTimestep || !InVC) {
		id = viewpos;
	}

	//Reset Clip Radius settings
	SetClipRadius(0.0);

	// Init the 2D panel switches to fix XRSound not giving us switch clicks if we load directly into the VC.
	// Calling InitPanel(LMPANEL_MAIN) also works, since that function calls SetSwitches() as well.
	SetSwitches(LMPANEL_MAIN);	// Use main panel as a placeholder, it doesn't actually matter

	//Reset VC free camera to default
	vcFreeCamx = 0;
	vcFreeCamy = 0;
	vcFreeCamz = 0;

	//Flashlight
	DelLightEmitter(flashlight);
	flashlight = (::SpotLight*)AddSpotLight(flashlightPos, flashlightDirLocal, 3, 0, 0, 3, 0, RAD * 45, flashlightColor, flashlightColor, flashlightColor2);
	flashlight->SetVisibility(LightEmitter::VIS_COCKPIT);
	flashlight->Activate(flashlightOn);

	switch (id) {
	case LMVIEW_CDR:
		viewpos = LMVIEW_CDR;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(-0.1, -0.0612, 0.25), 0, P1_TILT, _V(-0.1, 0.0, 0.0), 0, 0, _V(0.1, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(LMVIEW_CBLEFT, LMVIEW_DSKY, LMVIEW_RDVZWIN, LMVIEW_LPD);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();
		SetCOAS();

		RegisterActiveAreas();

		return true;

	case LMVIEW_LMP:
		viewpos = LMVIEW_LMP;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.1, 0.0, 0.1), 0, 0, _V(-0.1, 0.0, 0.0), 0, 0, _V(0.1, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(LMVIEW_DSKY, LMVIEW_CBRIGHT, -1, -1 );
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();
		SetCOAS();

		RegisterActiveAreas();

		return true;

	case LMVIEW_LPD:
		viewpos = LMVIEW_LPD;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.13, 0.0, 0.0), 0, 15 * RAD, _V(0.0, 0.0, 0.0), 0, 0, _V(0.58, -0.4, -0.1), 0, -10 * RAD);
		oapiCameraSetAperture(30 * RAD);
		oapiVCSetNeighbours(-1, -1, LMVIEW_CDR, -1);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();
		SetCOAS();

		RegisterActiveAreas();

		return true;

	case LMVIEW_DSKY:
		viewpos = LMVIEW_DSKY;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.0, 0.0, 0.0), 0, 0, _V(-0.56, 0.0, 0.2), 0, -35 * RAD, _V(0.56, 0.0, 0.2), 0, -35 * RAD);
		oapiVCSetNeighbours(LMVIEW_CDR, LMVIEW_LMP, LMVIEW_AOT, LMVIEW_FWDHATCH);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();
		SetCOAS();

		RegisterActiveAreas();

		return true;

	case LMVIEW_CBLEFT:
		viewpos = LMVIEW_CBLEFT;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.0, 0.1, 0.0), 0, 0, _V(0.0, 0.0, -0.1), 0, 0, _V(0.0, 0.0, 0.1), 0, 0);
		oapiVCSetNeighbours(LMVIEW_ECS, LMVIEW_CDR, -1, -1);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();
		SetCOAS();

		RegisterActiveAreas();

		return true;

	case LMVIEW_CBRIGHT:
		viewpos = LMVIEW_CBRIGHT;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.0, 0.1, 0.0), 0, 0, _V(0.0, 0.0, 0.1), 0, 0, _V(0.0, 0.0, -0.1), 0, 0);
		oapiVCSetNeighbours(LMVIEW_LMP, LMVIEW_ECS2, -1, -1);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();
		SetCOAS();

		RegisterActiveAreas();

		return true;

	case LMVIEW_AOT:
		viewpos = LMVIEW_AOT;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.05, 0.0, -0.05), 0, 90 * RAD, _V(-0.1, 0.0, 0.0), 0, 0, _V(0.1, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(-1, -1, -1, LMVIEW_DSKY);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();
		SetCOAS();

		PanelId = LMPANEL_AOTZOOM;

		RegisterActiveAreas();

		return true;

	case LMVIEW_ECS:
		viewpos = LMVIEW_ECS;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.0, 0.0, 0.0), 0, 0, _V(0.0, 0.0, 0.3), 0, -20 * RAD, _V(0.0, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(LMVIEW_ECS2, LMVIEW_CBLEFT, LMVIEW_OVHDHATCH, -1);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();
		SetCOAS();

		RegisterActiveAreas();

		return true;

	case LMVIEW_ECS2:
		viewpos = LMVIEW_ECS2;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(-0.3, -0.1, -0.3), 55 * RAD, 0, _V(0.0, -0.5, 0.0), 0, -7 * RAD, _V(-0.3, -0.8, 0.0), 20 * RAD, 10 * RAD);
		oapiVCSetNeighbours(LMVIEW_CBRIGHT, LMVIEW_ECS, -1, -1);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();

		RegisterActiveAreas();

		return true;

	case LMVIEW_FWDHATCH:
		viewpos = LMVIEW_FWDHATCH;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.0, 0.0, 0.0), 0, 0, _V(0.0, 0.0, 0.0), 0, 0, _V(0.0, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(-1, -1, LMVIEW_DSKY, -1);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();

		RegisterActiveAreas();

		return true;

	case LMVIEW_OVHDHATCH:
		viewpos = LMVIEW_OVHDHATCH;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.0, 0.0, 0.0), 0, 0, _V(0.0, 0.0, 0.0), 0, 0, _V(0.0, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(-1, -1, -1, LMVIEW_ECS);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();

		RegisterActiveAreas();

		return true;

	case LMVIEW_RDVZWIN:
		viewpos = LMVIEW_RDVZWIN;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.0, 0.0, 0.0), 0, 0, _V(0.0, 0.0, 0.0), 0, 0, _V(0.0, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(-1, -1, -1, LMVIEW_CDR);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();
		SetCOAS();

		RegisterActiveAreas();

		return true;

	default:
		return false;
	}
}

void LEM::InitPanelVC() {

	DWORD ck = oapiGetColour(255, 0, 255);

	// LM VC surfaces

	srf[SRF_VC_DIGITALDISP] = oapiLoadTexture("ProjectApollo/VC/digitaldisp.dds");
	srf[SRF_VC_DIGITALDISP2] = oapiLoadTexture("ProjectApollo/VC/digitaldisp_2.dds");
	srf[SRF_VC_DSKYDISP] = oapiLoadTexture("ProjectApollo/VC/dsky_disp.dds");
	srf[SRF_VC_DSKY_LIGHTS] = oapiLoadTexture("ProjectApollo/VC/dsky_lights.dds");
	srf[SRF_VC_RADAR_TAPEA] = oapiLoadTexture("ProjectApollo/VC/lm_range_rate_indicator_scales_a.dds");
	srf[SRF_VC_RADAR_TAPEB] = oapiLoadTexture("ProjectApollo/VC/lm_range_rate_indicator_scales_b.dds");
	srf[SRF_VC_RADAR_TAPE2] = oapiLoadTexture("ProjectApollo/VC/lm_range_rate_indicator_scales2.dds");
	srf[SFR_VC_CW_LIGHTS] = oapiLoadTexture("ProjectApollo/VC/lem_cw_lights.dds");
	srf[SRF_INDICATORVC] = oapiLoadTexture("ProjectApollo/VC/Indicator.dds");
	srf[SRF_INDICATORREDVC] = oapiLoadTexture("ProjectApollo/VC/IndicatorRed.dds");
	srf[SRF_LEM_MASTERALARMVC] = oapiLoadTexture("ProjectApollo/VC/lem_master_alarm.dds");
	srf[SRF_DEDA_LIGHTSVC] = oapiLoadTexture("ProjectApollo/VC/ags_lights.dds");
	srf[SRF_AOTFONT_VC] = oapiLoadTexture("ProjectApollo/VC/aot_font.dds");
	srf[SRF_ENGSTARTSTOP_VC] = oapiLoadTexture("ProjectApollo/VC/LMEngStartStop.dds");

	oapiSetSurfaceColourKey(srf[SRF_VC_DIGITALDISP], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_DIGITALDISP2], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_DSKYDISP], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_DSKY_LIGHTS], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_RADAR_TAPEA], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_RADAR_TAPEB], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_RADAR_TAPE2], ck);
	oapiSetSurfaceColourKey(srf[SFR_VC_CW_LIGHTS], ck);
	oapiSetSurfaceColourKey(srf[SRF_AOTFONT_VC], ck);
	oapiSetSurfaceColourKey(srf[SRF_ENGSTARTSTOP_VC], ck);
}

void LEM::ReleaseSurfacesVC()
{
	for (int i = 0; i < nsurfvc; i++)
		if (srf[i]) {
			oapiDestroySurface(srf[i]);
			srf[i] = 0;
		}
}

void LEM::RegisterActiveAreas()
{
	int i = 0;

	VECTOR3 ofs;
	if (stage > 1) { ofs = _V(0.00, -0.76, 0.00); }
	else { ofs = _V(0.00, 0.99, 0.00); }

	//Apply center of gravity offset here
	ofs -= currentCoG;

	//
	// Release all surfaces
	//
	ReleaseSurfacesVC();


	SURFHANDLE MainPanelTex1 = oapiGetTextureHandle(hLMVC, VC_TEX_LMVC_dds);
	SURFHANDLE MainPanelTex2 = oapiGetTextureHandle(hLMVC, VC_TEX_LMVC_2_dds);

	// Panel 1

	//Normalized vectors for quadrilateral click mode (re-defined for each panel orientation)
	VECTOR3 UL = _V(-0.7355, 0.6709, 0.0944);
	VECTOR3 UR = _V(0.7354, 0.6710, 0.0945);
	VECTOR3 DL = _V(-0.7356, -0.6708, -0.0946);
	VECTOR3 DR = _V(-0.7355, -0.6709, -0.0944);

	for (i = 0; i < P1_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P1_01 + i, P1_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P1_CLICK + ofs, P1_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P1_CLICK + ofs, P1_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P1_CLICK + ofs, P1_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P1_CLICK + ofs);
	}

	for (i = 0; i < P1_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P1_01 + i, P1_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_LMVC_INTEGRAL_LIGHT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_LMVC_FLOOD_LIGHT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_LMVC_NUMERICS_LIGHT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	oapiVCRegisterArea(AID_VC_LM_CWS_LEFT, _R(238*TexMul, 27*TexMul, 559*TexMul, 153*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MISSION_CLOCK, _R(60*TexMul, 259*TexMul, 202*TexMul, 281*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_EVENT_TIMER, _R(276*TexMul, 259*TexMul, 357*TexMul, 281*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RANGE_TAPE, _R(431*TexMul, 633*TexMul, 475*TexMul, 796*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RATE_TAPE, _R(482*TexMul, 633*TexMul, 517*TexMul, 796*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LEM_MA_LEFT, _R(30*TexMul, 593*TexMul, 77*TexMul, 636*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCSetAreaClickmode_Spherical(AID_VC_LEM_MA_LEFT, _V(-0.415919, 0.599307, 1.70252) + ofs, 0.008);
	oapiVCRegisterArea(AID_VC_MPS_REG_CONTROLS_LEFT, _R(341*TexMul, 891*TexMul, 377*TexMul, 1098*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MPS_REG_CONTROLS_RIGHT, _R(415*TexMul, 891*TexMul, 451*TexMul, 1098*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MPS_OXID_QUANTITY_INDICATOR, _R(445*TexMul, 218*TexMul, 484*TexMul, 239*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MPS_FUEL_QUANTITY_INDICATOR, _R(445*TexMul, 270*TexMul, 484*TexMul, 292*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MPS_HELIUM_PRESS_INDICATOR, _R(577*TexMul, 259*TexMul, 658*TexMul, 281*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	
	oapiVCRegisterArea(AID_VC_XPOINTERCDR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FDAI_LEFT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_CONTACTLIGHT1, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PWRFAIL_LIGHTS_P1, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	oapiVCRegisterArea(AID_VC_ABORT_BUTTON, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ABORT_BUTTON, _V(-0.10018, 0.436067, 1.68518) + ofs, 0.01);
	oapiVCRegisterArea(AID_VC_ABORTSTAGE_BUTTON, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ABORTSTAGE_BUTTON, _V(-0.047192, 0.437682, 1.68536) + ofs, 0.01);

	// Panel 2
	for (i = 0; i < P2_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P2_01 + i, P2_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P2_CLICK + ofs, P2_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P2_CLICK + ofs, P2_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P2_CLICK + ofs, P2_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P2_CLICK + ofs);
	}

	for (i = 0; i < P2_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P2_01 + i, P2_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_LM_CWS_RIGHT, _R(1075*TexMul, 27*TexMul, 1375*TexMul, 153*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_ASC_FEED_TALKBACKS, _R(794*TexMul, 413*TexMul, 1031*TexMul, 436*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LGC_CMD_ENABLE_14_TALKBACKS, _R(794*TexMul, 562*TexMul, 1031*TexMul, 585*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LGC_CMD_ENABLE_23_TALKBACKS, _R(794*TexMul, 688*TexMul, 1031*TexMul, 711*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_XFEED_TALKBACK, _R(795*TexMul, 844*TexMul, 818*TexMul, 867*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MAIN_SOV_TALKBACKS, _R(934*TexMul, 844*TexMul, 1027*TexMul, 867*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LEM_MA_RIGHT, _R(1384*TexMul, 593*TexMul, 1431*TexMul, 636*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCSetAreaClickmode_Spherical(AID_VC_LEM_MA_RIGHT, _V(0.414751, 0.59891, 1.70247) + ofs, 0.008);

	oapiVCRegisterArea(AID_VC_PANEL2_COMPLIGHTS, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PWRFAIL_LIGHTS_P2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_XPOINTERLMP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FDAI_RIGHT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Panel 3

	UL = _V(-0.6896, 0.5913, 0.4179);
	UR = _V(0.6895, 0.5912, 0.4182);
	DL = _V(-0.6897, -0.5910, -0.4183);
	DR = _V(0.6896, -0.5914, -0.4178);

	for (i = 0; i < P3_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P3_01 + i, P3_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P3_CLICK + ofs, P3_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P3_CLICK + ofs, P3_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P3_CLICK + ofs, P3_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P3_CLICK + ofs);
	}

	for (i = 0; i < 4; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P3_01 + i, P3_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_ROT_P3_05, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ROT_P3_05, P3_ROT_POS[4] + UL * ROT + P3_CLICK + ofs, P3_ROT_POS[4] + UR * ROT + P3_CLICK + ofs, P3_ROT_POS[4] + DL * ROT + P3_CLICK + ofs, P3_ROT_POS[4] + DR * ROT + P3_CLICK + ofs);

	oapiVCRegisterArea(AID_VC_RR_NOTRACK, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_CONTACTLIGHT2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_RR_SLEW_SWITCH, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_RR_SLEW_SWITCH, _V(-0.27271, 0.157539, 1.57055) + ofs, _V(-0.255647, 0.157539, 1.57055) + ofs, _V(-0.27271, 0.143561, 1.56076) + ofs, _V(-0.255647, 0.143561, 1.56076) + ofs);

	// Panel 4

	UL = _V(-0.7017, 0.5037, 0.5036);
	UR = _V(0.7016, 0.5036, 0.5038);
	DL = _V(-0.7018, -0.5038, -0.5037);
	DR = _V(0.7017, -0.5037, -0.5036);

	for (i = 0; i < P4_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P4_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P4_01 + i, P4_PUSHB_POS[i] + ofs, 0.008);
	}

	for (i = 0; i < P4_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P4_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P4_01 + i, P4_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P4_CLICK + ofs, P4_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P4_CLICK + ofs, P4_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P4_CLICK + ofs, P4_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P4_CLICK + ofs);
	}

	oapiVCRegisterArea(AID_VC_DSKY_DISPLAY, _R(309*TexMul, 1520*TexMul, 414*TexMul, 1696*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_DSKY_LIGHTS,  _R(165*TexMul, 1525*TexMul, 267*TexMul, 1694*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	// Panel 5

	UL = _V(-0.7619, 0.1117, 0.6379);
	UR = _V(0.7608, 0.1135, 0.6391);
	DL = _V(-0.7606, -0.1137, -0.6389);
	DR = _V(0.7618, -0.1116, -0.6378);

	for (i = 0; i < P5_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P5_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P5_01 + i, P5_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P5_CLICK + ofs, P5_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P5_CLICK + ofs, P5_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P5_CLICK + ofs, P5_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P5_CLICK + ofs);
	}

	for (i = 0; i < P5_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P5_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ROT_P5_01 + i, P5_ROT_POS[i] + UL * ROT + P5_CLICK + ofs, P5_ROT_POS[i] + UR * ROT + P5_CLICK + ofs, P5_ROT_POS[i] + DL * ROT + P5_CLICK + ofs, P5_ROT_POS[i] + DR * ROT + P5_CLICK + ofs);
	}

	oapiVCRegisterArea(AID_VC_START_BUTTON, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_START_BUTTON, _V(-0.644677, 0.073766, 1.48271) + ofs, 0.01);
	oapiVCRegisterArea(AID_VC_STOP_BUTTON_CDR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_STOP_BUTTON_CDR, _V(-0.694894, 0.085711, 1.53811) + ofs, 0.01);
	oapiVCRegisterArea(AID_VC_PLUSX_BUTTON, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_PLUSX_BUTTON, _V(-0.65603, -0.017321, 1.41484) + ofs, 0.01);

	oapiVCRegisterArea(AID_VC_START_BUTTON_RED, _R(894*TexMul, 1958*TexMul, 962*TexMul, 2027*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	// Panel 6

	for (i = 0; i < P6_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P6_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P6_01 + i, P6_PUSHB_POS[i] + ofs, 0.008);
	}

	for (i = 0; i < P6_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P6_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P6_01 + i, P6_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P6_CLICK + ofs, P6_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P6_CLICK + ofs, P6_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P6_CLICK + ofs, P6_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P6_CLICK + ofs);
	}

	oapiVCRegisterArea(AID_VC_STOP_BUTTON_LMP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_STOP_BUTTON_LMP, _V(0.421597, 0.077646, 1.50924) + ofs, 0.01);

	oapiVCRegisterArea(AID_VC_LM_DEDA_DISP, _R(1803*TexMul, 98*TexMul, 1937*TexMul, 120*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LM_DEDA_ADR, _R(1827*TexMul, 53*TexMul, 1885*TexMul, 75*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LM_DEDA_LIGHTS, _R(1740*TexMul, 96*TexMul, 1787*TexMul, 122*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	// Panel 8

	UL = _V(-0.6746, 0.2187, -0.7050);
	UR = _V(-0.6747, 0.2186, 0.7051);
	DL = _V(0.6745, -0.2188, -0.7049);
	DR = _V(0.6746, -0.2187, 0.7050);

	for (i = 0; i < P8_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P8_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P8_01 + i, P8_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P8_CLICK + ofs, P8_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P8_CLICK + ofs, P8_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P8_CLICK + ofs, P8_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P8_CLICK + ofs);
	}

	for (i = 0; i < P8_TWCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_TW_P8_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P8_01 + i, P8_TW_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_SEQ_LIGHTS, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PANEL8_TALKBACKS, _R(1389*TexMul, 8*TexMul, 1772*TexMul, 149*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	oapiVCRegisterArea(AID_VC_STAGE_SWITCH, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_STAGE_SWITCH, _V(-0.981771, 0.039868, 1.14749) + (P8_CLICK * 2) + ofs, 0.01);

	// Panel 11
	// Row 1
	for (i = 0; i < P11R1_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P11R1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P11R1_01 + i, P11R1_CB_POS[i] + ofs, 0.008);
	}
	// Row 2
	for (i = 0; i < P11R2_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P11R2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P11R2_01 + i, P11R2_CB_POS[i] + ofs, 0.008);
	}
	// Row 3
	for (i = 0; i < P11R3_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P11R3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P11R3_01 + i, P11R3_CB_POS[i] + ofs, 0.008);
	}
	// Row 4
	for (i = 0; i < P11R4_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P11R4_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P11R4_01 + i, P11R4_CB_POS[i] + ofs, 0.008);
	}
	// Row 5
	for (i = 0; i < P11R5_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P11R5_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P11R5_01 + i, P11R5_CB_POS[i] + ofs, 0.008);
	}

	// Panel 12

	UL = _V(0.6775, 0.1996, 0.7078);
	UR = _V(0.6774, 0.1995, -0.7077);
	DL = _V(-0.6776, -0.1997, 0.7079);
	DR = _V(-0.6775, -0.1996, -0.7078);

	for (i = 0; i < P12_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P12_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P12_01 + i, P12_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P12_CLICK + ofs, P12_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P12_CLICK + ofs, P12_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P12_CLICK + ofs, P12_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P12_CLICK + ofs);
	}

	for (i = 0; i < 2; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P12_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ROT_P12_01 + i, P12_ROT_POS[i] + UL * ROT + P12_CLICK + ofs, P12_ROT_POS[i] + UR * ROT + P12_CLICK + ofs, P12_ROT_POS[i] + DL * ROT + P12_CLICK + ofs, P12_ROT_POS[i] + DR * ROT + P12_CLICK + ofs);
	}

	for (i = 2; i < P12_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P12_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P12_01 + i, P12_ROT_POS[i] + ofs, 0.02);
	}

	for (i = 0; i < P12_TWCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_TW_P12_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P12_01 + i, P12_TW_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_RECORDER_TALKBACK, _R(601*TexMul, 1435*TexMul, 829*TexMul, 1483*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	// Panel 14

	UL = _V(0.5467, 0.4536, 0.7037);
	UR = _V(0.5466, 0.4535, -0.7036);
	DL = _V(-0.5468, -0.4537, 0.7038);
	DR = _V(-0.5467, -0.4536, -0.7037);

	for (i = 0; i < P14_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P14_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P14_01 + i, P14_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P14_CLICK + ofs, P14_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P14_CLICK + ofs, P14_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P14_CLICK + ofs, P14_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P14_CLICK + ofs);
	}

	for (i = 0; i < P14_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P14_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P14_01 + i, P14_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_PANEL14_COMPLIGHTS, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_DSC_BATTERY_TALKBACKS, _R(550*TexMul, 1002*TexMul, 865*TexMul, 1025*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_ASC_BATTERY_TALKBACKS, _R(932*TexMul, 1027*TexMul, 1176*TexMul, 1050*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	// Panel 16
	// Row 1
	for (i = 0; i < P16R1_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P16R1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P16R1_01 + i, P16R1_CB_POS[i] + ofs, 0.008);
	}

	// Row 2
	for (i = 0; i < P16R2_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P16R2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P16R2_01 + i, P16R2_CB_POS[i] + ofs, 0.008);
	}

	// Row 3
	for (i = 0; i < P16R3_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P16R3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P16R3_01 + i, P16R3_CB_POS[i] + ofs, 0.008);
	}

	// Row 4
	for (i = 0; i < P16R4_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P16R4_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P16R4_01 + i, P16R4_CB_POS[i] + ofs, 0.008);
	}

	// ORDEAL

	UL = _V(-0.6746, 0.2187, -0.7050);
	UR = _V(-0.6747, 0.2186, 0.7051);
	DL = _V(0.6745, -0.2188, -0.7049);
	DR = _V(0.6746, -0.2187, 0.7050);

	for (i = 0; i < ORDEAL_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_ORDEAL_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_ORDEAL_01 + i, ORDEAL_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + ORDEAL_CLICK + ofs, ORDEAL_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + ORDEAL_CLICK + ofs, ORDEAL_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + ORDEAL_CLICK + ofs, ORDEAL_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + ORDEAL_CLICK + ofs);
	}

	oapiVCRegisterArea(AID_VC_ROT_ORDEAL_01, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ROT_ORDEAL_01, _V(-1.05598, 0.207146, 0.875066) + ofs, _V(-1.05538, 0.206944, 0.918372) + ofs, _V(-1.01508, 0.193584, 0.875066) + ofs, _V(-1.01508, 0.193608, 0.918372) + ofs);

	// ECS Panels

	oapiVCRegisterArea(AID_VC_ROT_SUITCIRCUITRELIEF, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_SUITCIRCUITRELIEF, Rot_SuitCircuitReliefLocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_CABINGASRETURN, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_CABINGASRETURN, Rot_CabinGasReturnLocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_CANISTER1, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_CANISTER1, Rot_Canister1Location + ofs, 0.1);

	oapiVCRegisterArea(AID_VC_ROT_CANISTER2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_CANISTER2, Rot_Canister2Location + ofs, 0.07);

	oapiVCRegisterArea(AID_VC_ROT_CABINREPRESS, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_CABINREPRESS, Rot_CabinRepressLocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_PLSSFILL, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_PLSSFILL, Rot_PlssFillLocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_PRESSREGA, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_PRESSREGA, Rot_PressRegALocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_PRESSREGB, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_PRESSREGB, Rot_PressRegBLocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_DESO2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_DESO2, Rot_DesO2Location + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_ASCO2_1, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_ASCO2_1, Rot_AscO2_1Location + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_ASCO2_2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_ASCO2_2, Rot_AscO2_2Location + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_SECEVAPFLOW, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_SECEVAPFLOW, Rot_SecEvapFlowLocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_PRIEVAPFLOW_2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_PRIEVAPFLOW_2, Rot_PriEvapFlow2Location + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_DESH2O, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_DESH2O, Rot_DesH2OLocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_PRIEVAPFLOW_1, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_PRIEVAPFLOW_1, Rot_PriEvapFlow1Location + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_SUITTEMP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_SUITTEMP, Rot_SuitTempLocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_WATERTANKSEL, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_WATERTANKSEL, Rot_WaterTankSelectorLocation + ofs, 0.05);

	oapiVCRegisterArea(AID_VC_ROT_LGC, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_LGC, Rot_LGCLocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_SUITISOLCDR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_SUITISOLCDR, Rot_SuitIsolCdrLocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_SUITISOLLMP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_SUITISOLLMP, Rot_SuitIsolLmpLocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ROT_ASCH2O, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_ASCH2O, Rot_AscH2OLocation + ofs, 0.02);


	oapiVCRegisterArea(AID_VC_SUITGASDIVERTER, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_SUITGASDIVERTER, SuitGasDiverterHandleLocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_H2OSEP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_H2OSEP, H20SepHandleLocation + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_CANISTERSEL, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_CANISTERSEL, _V(0.3571, 0.4668, 0.1878) + ofs, _V(0.3572, 0.4670, 0.0753) + ofs, _V(0.3570, 0.3545, 0.1876) + ofs, _V(0.3569, 0.3546, 0.0753) + ofs);

	oapiVCRegisterArea(AID_VC_VENT1, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_VENT1, PB_Vent1Location + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_VENT2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_VENT2, PB_Vent2Location + ofs, 0.02);

	oapiVCRegisterArea(AID_VC_ACTOVRDCDR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ACTOVRDCDR, _V(0.342716, 0.154725, 0.539607) + ofs, 0.008);

	oapiVCRegisterArea(AID_VC_ACTOVRDLMP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ACTOVRDLMP, _V(0.560577, 0.172482, 0.554511) + ofs, 0.008);

	// Window Shades
	oapiVCRegisterArea(AID_VC_WINDOWSHADES, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_WINDOWSHADES, WindowShadesLocation + ofs, 0.05);

	// Hatches
	oapiVCRegisterArea(AID_VC_OVERHEADHATCH, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_OVERHEADHATCH, UpperHatchLocation + ofs, 0.1);

	oapiVCRegisterArea(AID_VC_OVERHEADHATCHHANDLE, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_OVERHEADHATCHHANDLE, _V(0.1029, 0.9761, 0.3830) + ofs, _V(-0.03721, 0.9762, 0.3831) + ofs, _V(0.1028, 0.9760, 0.2427) + ofs, _V(-0.0370, 0.9763, 0.2429) + ofs);

	oapiVCRegisterArea(AID_VC_OVERHEADHATCHRELIEFVALVE, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_OVERHEADHATCHRELIEFVALVE, _V(0.2085, 1.0112, 0.1652) + ofs, _V(0.1385, 1.0111, 0.1653) + ofs, _V(0.2084, 0.9411, 0.1651) + ofs, _V(0.1383, 0.9409, 0.1654) + ofs);

	if (viewpos == LMVIEW_FWDHATCH) { // To avoid that my DSKY clicks plays with the forward dump valve as well
		oapiVCRegisterArea(AID_VC_FORWARDHATCH, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_FORWARDHATCH, FwdHatchInnerLocation + ofs, 0.1);

		oapiVCRegisterArea(AID_VC_FORWARDHATCHHANDLE, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_FORWARDHATCHHANDLE, _V(-0.3796, -0.6482, 1.5752) + ofs, _V(-0.2240, -0.6481, 1.5751) + ofs, _V(-0.3797, -0.4924, 1.5752) + ofs, _V(-0.2238, -0.4926, 1.5750) + ofs);

		oapiVCRegisterArea(AID_VC_FORWARDHATCHRELIEFVALVE, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_FORWARDHATCHRELIEFVALVE, _V(0.2532, -0.4597, 1.5752) + ofs, _V(0.1735, -0.4598, 1.5751) + ofs, _V(0.2531, -0.5396, 1.5751) + ofs, _V(0.1734, -0.5395, 1.5750) + ofs);
	}

	// Utility Lights

	UL = _V(-0.7071, 0.0052, -0.7068);
	UR = _V(0.7072, 0.0053, -0.7072);
	DL = _V(-0.7069, -0.0054, 0.7071);
	DR = _V(0.7068, -0.0055, 0.7070);

	oapiVCRegisterArea(AID_VC_UTILITYLIGHTCDR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_UTILITYLIGHTCDR, UtilityLights_CDRLocation + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + UTIL_LTS_CLICK + ofs, UtilityLights_CDRLocation + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + UTIL_LTS_CLICK + ofs, UtilityLights_CDRLocation + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + UTIL_LTS_CLICK + ofs, UtilityLights_CDRLocation + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + UTIL_LTS_CLICK + ofs);

	oapiVCRegisterArea(AID_VC_UTILITYLIGHTLMP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_UTILITYLIGHTLMP, UtilityLights_LMPLocation + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + UTIL_LTS_CLICK + ofs, UtilityLights_LMPLocation + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + UTIL_LTS_CLICK + ofs, UtilityLights_LMPLocation + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + UTIL_LTS_CLICK + ofs, UtilityLights_LMPLocation + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + UTIL_LTS_CLICK + ofs);

	// AOT

	UL = _V(0.7391, 0.3895, -0.5493);
	UR = _V(0.6599, -0.3398, 0.6698);
	DL = _V(-0.6598, 0.3399, -0.6699);
	DR = _V(-0.7392, -0.3894, 0.5494);

	oapiVCRegisterArea(AID_VC_SW_RRGYRO, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SW_RRGYRO, Sw_RRGyroLocation + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + RRGYRO_CLICK + ofs, Sw_RRGyroLocation + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + RRGYRO_CLICK + ofs, Sw_RRGyroLocation + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + RRGYRO_CLICK + ofs, Sw_RRGyroLocation + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + RRGYRO_CLICK + ofs);

	oapiVCRegisterArea(AID_VC_RETICLEDISP, _R(1068*TexMul, 1891*TexMul, 1188*TexMul, 1927*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	// COAS FWD
	const VECTOR3 COAS2Location = { -0.5612, 0.7323, 1.6805 };
	oapiVCRegisterArea(AID_VC_COAS2, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_COAS2, COAS2Location + ofs, 0.05);

	// COAS OVHD
	const VECTOR3 COAS1Location = { -0.475804, 1.0044, 1.0043 };
	oapiVCRegisterArea(AID_VC_COAS1, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_COAS1, COAS1Location + ofs, 0.05);

	//
	// Initialize surfaces and switches
	//
	InitPanelVC();

	LeftMasterAlarmSwitch.InitVC(srf[SRF_LEM_MASTERALARMVC]);
	RightMasterAlarmSwitch.InitVC(srf[SRF_LEM_MASTERALARMVC]);

	ASCHeReg1TB.InitVC(srf[SRF_INDICATORVC]);
	DESHeReg1TB.InitVC(srf[SRF_INDICATORVC]);
	ASCHeReg2TB.InitVC(srf[SRF_INDICATORVC]);
	DESHeReg2TB.InitVC(srf[SRF_INDICATORVC]);
	DPSOxidPercentMeter.InitVC(srf[SRF_VC_DIGITALDISP2]);
	DPSFuelPercentMeter.InitVC(srf[SRF_VC_DIGITALDISP2]);
	MainHeliumPressureMeter.InitVC(srf[SRF_VC_DIGITALDISP2]);

	RCSAscFeed1ATB.InitVC(srf[SRF_INDICATORVC]);
	RCSAscFeed2ATB.InitVC(srf[SRF_INDICATORVC]);
	RCSAscFeed1BTB.InitVC(srf[SRF_INDICATORVC]);
	RCSAscFeed2BTB.InitVC(srf[SRF_INDICATORVC]);

	RCSQuad1ACmdEnableTB.InitVC(srf[SRF_INDICATORREDVC]);
	RCSQuad4ACmdEnableTB.InitVC(srf[SRF_INDICATORREDVC]);
	RCSQuad1BCmdEnableTB.InitVC(srf[SRF_INDICATORREDVC]);
	RCSQuad4BCmdEnableTB.InitVC(srf[SRF_INDICATORREDVC]);

	RCSQuad2ACmdEnableTB.InitVC(srf[SRF_INDICATORREDVC]);
	RCSQuad3ACmdEnableTB.InitVC(srf[SRF_INDICATORREDVC]);
	RCSQuad2BCmdEnableTB.InitVC(srf[SRF_INDICATORREDVC]);
	RCSQuad3BCmdEnableTB.InitVC(srf[SRF_INDICATORREDVC]);

	RCSXFeedTB.InitVC(srf[SRF_INDICATORVC]);

	RCSMainSovATB.InitVC(srf[SRF_INDICATORVC]);
	RCSMainSovBTB.InitVC(srf[SRF_INDICATORVC]);

	EDDesFuelVentTB.InitVC(srf[SRF_INDICATORVC]);
	EDDesOxidVentTB.InitVC(srf[SRF_INDICATORVC]);
	EDLGTB.InitVC(srf[SRF_INDICATORVC]);

	TapeRecorderTB.InitVC(srf[SRF_INDICATORVC]);

	DSCBattery1TB.InitVC(srf[SRF_INDICATORVC]);
	DSCBattery2TB.InitVC(srf[SRF_INDICATORVC]);
	DSCBattery3TB.InitVC(srf[SRF_INDICATORVC]);
	DSCBattery4TB.InitVC(srf[SRF_INDICATORVC]);
	DSCBattFeedTB.InitVC(srf[SRF_INDICATORVC]);

	ASCBattery5ATB.InitVC(srf[SRF_INDICATORVC]);
	ASCBattery5BTB.InitVC(srf[SRF_INDICATORVC]);
	ASCBattery6ATB.InitVC(srf[SRF_INDICATORVC]);
	ASCBattery6BTB.InitVC(srf[SRF_INDICATORVC]);
}

bool LEM::clbkVCMouseEvent(int id, int event, VECTOR3 &p)
{
	switch (id) {
		case AID_VC_OVERHEADHATCH:
			OverheadHatch.Toggle();
			return true;

		case AID_VC_FORWARDHATCH:
			ForwardHatch.Toggle();
			return true;

		case AID_VC_WINDOWSHADES:
			if (LEMWindowShades)
			{
				LEMWindowShades = false;
			}
			else
			{
				LEMWindowShades = true;
			}
			//Maybe define a custom sound to play here?
			//SwitchClick();
			SetWindowShades();
			return true;

		case AID_VC_COAS1:
			if (LEMCoas1Enabled)
			{
				LEMCoas1Enabled = false;
			}
			else
			{
				LEMCoas1Enabled = true;
				LEMCoas2Enabled = false;
			}
			SwitchClick();
			SetCOAS();
			return true;

		case AID_VC_COAS2:
			if (LEMCoas2Enabled)
			{
				LEMCoas2Enabled = false;
			}
			else
			{
				LEMCoas2Enabled = true;
				LEMCoas1Enabled = false;
			}
			SwitchClick();
			SetCOAS();
			return true;
	}
	return MainPanelVC.VCMouseEvent(id, event, p);
	//return false;
}

bool LEM::clbkVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	switch (id) {

#ifdef _OPENORBITER
	case AID_LMVC_INTEGRAL_LIGHT:
        SetLMVCIntegralLight(vcidx, IntegralLights_LMVC, MatProp::Emission, lca.GetIntegralVoltage() / 100.0, NUM_ELEMENTS(IntegralLights_LMVC));
//		SetLMVCIntegralLight(vcidx, IntegralLights_LMVC_NoTex, MatProp::Light, lca.GetIntegralVoltage() / 100.0, NUM_ELEMENTS(IntegralLights_LMVC_NoTex)));
		SetLMVCIntegralLight(vcidx, IntegralLights_LMVC_NoTex, MatProp::Light, ( (lca.GetIntegralVoltage() / 100.0) + (FloodLights.GetCDRRotaryVoltage() / 28.0) )/2.0, NUM_ELEMENTS(IntegralLights_LMVC_NoTex));
//		SetLMVCIntegralLight(vcidx, IntegralLights_LMVC_NoTex, MatProp::Emission, lca.GetIntegralVoltage() / 100.0, NUM_ELEMENTS(IntegralLights_LMVC_NoTex));
        return true;

	case AID_LMVC_FLOOD_LIGHT:
        SetLMVCIntegralLight(vcidx, FloodLights_LMVC, MatProp::Light, FloodLights.GetCDRRotaryVoltage() / 28.0, NUM_ELEMENTS(FloodLights_LMVC));
        SetLMVCIntegralLight(xpointershadesidx, FloodLights_XPointer_Shades, MatProp::Light, FloodLights.GetCDRRotaryVoltage() / 28.0, NUM_ELEMENTS(FloodLights_XPointer_Shades));
        return true;

	case AID_LMVC_NUMERICS_LIGHT:
		SetLMVCIntegralLight(vcidx, NumericLights_LMVC, MatProp::Light, ( (lca.GetNumericVoltage() / 115.0) + (FloodLights.GetCDRRotaryVoltage() / 28.0) )/2, NUM_ELEMENTS(NumericLights_LMVC));
		return true;

#else
	case AID_LMVC_INTEGRAL_LIGHT:
        SetLMVCIntegralLight(vcidx, IntegralLights_LMVC, MESHM_EMISSION2, lca.GetIntegralVoltage() / 75.0, NUM_ELEMENTS(IntegralLights_LMVC));
		SetLMVCIntegralLight(vcidx, IntegralLights_LMVC_NoTex, MESHM_EMISSION, lca.GetIntegralVoltage() / 75.0, NUM_ELEMENTS(IntegralLights_LMVC_NoTex));
        SetLMVCIntegralLight(vcidx, IntegralLights_LMVC_NoTex, MESHM_EMISSION2, lca.GetIntegralVoltage() / 75.0, NUM_ELEMENTS(IntegralLights_LMVC_NoTex));
		return true;

	case AID_LMVC_FLOOD_LIGHT:
        SetLMVCIntegralLight(vcidx, FloodLights_LMVC, MESHM_EMISSION, FloodLights.GetCDRRotaryVoltage() / 28.0, NUM_ELEMENTS(FloodLights_LMVC));
        SetLMVCIntegralLight(xpointershadesidx, FloodLights_XPointer_Shades, MESHM_EMISSION, FloodLights.GetCDRRotaryVoltage() / 28.0, NUM_ELEMENTS(FloodLights_XPointer_Shades));
        return true;

	case AID_LMVC_NUMERICS_LIGHT:
		SetLMVCIntegralLight(vcidx, NumericLights_LMVC, MESHM_EMISSION, ( (lca.GetNumericVoltage() / 115.0) + (FloodLights.GetCDRRotaryVoltage() / 28.0) )/2, NUM_ELEMENTS(NumericLights_LMVC));

//		sprintf(oapiDebugString(), "Numerics Voltage = %lf", lca.GetNumericVoltage());
		
        return true;

#endif

	case AID_VC_LM_CWS_LEFT:
		CWEA.RedrawLeft(surf, srf[SFR_VC_CW_LIGHTS], TexMul);
		return true;

	case AID_VC_LM_CWS_RIGHT:
		CWEA.RedrawRight(surf, srf[SFR_VC_CW_LIGHTS], TexMul);
		return true;

	case AID_VC_MISSION_CLOCK:
		MissionTimerDisplay.Render(surf, srf[SRF_VC_DIGITALDISP2], false, TexMul);
		return true;

	case AID_VC_EVENT_TIMER:
		EventTimerDisplay.Render(surf, srf[SRF_VC_DIGITALDISP2], TexMul);
		return true;

	case AID_VC_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[SRF_VC_DIGITALDISP], srf[SRF_VC_DSKYDISP], 0, 0, TexMul);
		return true;

	case AID_VC_DSKY_LIGHTS:
		if (pMission->GetLMDSKYVersion() == 3)
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS], 0, 0, true, true, TexMul);
		}
		else if (pMission->GetLMDSKYVersion() == 2)
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS], 0, 0, true, false, TexMul );
		}
		else
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS], 0, 0, false, false, TexMul);
		}
		return true;

	case AID_VC_LM_DEDA_LIGHTS:
		deda.RenderOprErr(surf, srf[SRF_DEDA_LIGHTSVC], TexMul);
		return true;

	case AID_VC_LM_DEDA_DISP:
		deda.RenderData(surf, srf[SRF_VC_DIGITALDISP2], 0, 0, TexMul);
		return true;

	case AID_VC_LM_DEDA_ADR:
		deda.RenderAdr(surf, srf[SRF_VC_DIGITALDISP2], 0, 0, TexMul);
		return true;

	case AID_VC_RANGE_TAPE:
		RadarTape.RenderRangeVC(surf, srf[SRF_VC_RADAR_TAPEA], srf[SRF_VC_RADAR_TAPEB], srf[SRF_VC_RADAR_TAPE2], TexMul);
		return true;

	case AID_VC_RATE_TAPE:
		RadarTape.RenderRateVC(surf, srf[SRF_VC_RADAR_TAPEA], srf[SRF_VC_RADAR_TAPEB], TexMul);
		return true;

	case AID_VC_XPOINTERCDR:
		crossPointerLeft.DrawSwitchVC(id, event, surf);
		return true;

	case AID_VC_XPOINTERLMP:
		crossPointerRight.DrawSwitchVC(id, event, surf);
		return true;

	case AID_VC_CONTACTLIGHT1:
		if (SCS_ENG_CONT_CB.IsPowered() && (scca3.GetContactLightLogic() || LampToneTestRotary.GetState() == 6)) {
			SetContactLight(VC_MAT_L10_ContactLight1, true); // Light On
		}
		else
		{
			SetContactLight(VC_MAT_L10_ContactLight1, false); // Light On
		}
		return true;

	case AID_VC_CONTACTLIGHT2:
		if (SCS_ATCA_CB.IsPowered() && (scca3.GetContactLightLogic() || LampToneTestRotary.GetState() == 6)) {
			SetContactLight(VC_MAT_L11_ContactLight2, true); // Light On
		}
		else
		{
			SetContactLight(VC_MAT_L11_ContactLight2, false); // Light On
		}
		return true;

	case AID_VC_FDAI_LEFT:
		if (fdaiLeft.IsPowered()) {
			VECTOR3 errors;
			VECTOR3 rates;

			if (RateErrorMonSwitch.GetState() == 1)
			{
				if (RR.IsPowered()) {
					if (ShiftTruSwitch.IsUp())
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (50 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (50 * RAD);
						errors.x = 0.0;
					}
					else
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (5 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (5 * RAD);
						errors.x = 0.0;
					}
				}
				else
				{
					errors = _V(0, 0, 0);
				}
			}
			else
			{
				if (AttitudeMonSwitch.IsUp())	//PGNS
				{
					errors = _V(atca.lgc_err_x, atca.lgc_err_y, atca.lgc_err_z);
				}
				else							//AGS
				{
					VECTOR3 aeaerror = aea.GetAttitudeError();
					errors = _V(aeaerror.x, -aeaerror.y, -aeaerror.z)*DEG*41.0 / 15.0;

					if (DeadBandSwitch.IsUp())
					{
						errors *= 15.0 / 14.4;
					}
					else
					{
						errors *= 15.0 / 1.7;
					}
				}
			}

			if (RateScaleSwitch.IsUp())
			{
				rates = rga.GetRates() / (25.0*RAD);
			}
			else
			{
				rates = rga.GetRates() / (5.0*RAD);
			}

			fdaiLeft.AnimateFDAI(rates, errors, anim_fdaiR_cdr, anim_fdaiP_cdr, anim_fdaiY_cdr, anim_fdaiRerror_cdr, anim_fdaiPerror_cdr, anim_fdaiYerror_cdr, anim_fdaiRrate_cdr, anim_fdaiPrate_cdr, anim_fdaiYrate_cdr);
			SetAnimation(anim_attflag_cdr, 0.0);
		}
		else
		{
			SetAnimation(anim_attflag_cdr, 1.0);
		}
		return true;

	case AID_VC_FDAI_RIGHT:
		if (fdaiRight.IsPowered()) {
			VECTOR3 errors;
			VECTOR3 rates;

			if (RightRateErrorMonSwitch.GetState() == 1)
			{
				if (RR.IsPowered()) {
					if (ShiftTruSwitch.IsUp())
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (50 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (50 * RAD);
						errors.x = 0.0;
					}
					else
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (5 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (5 * RAD);
						errors.x = 0.0;
					}
				}
				else
				{
					errors = _V(0, 0, 0);
				}
			}
			else
			{
				if (RightAttitudeMonSwitch.IsUp())	//PGNS
				{
					errors = _V(atca.lgc_err_x, atca.lgc_err_y, atca.lgc_err_z);
				}
				else							//AGS
				{
					VECTOR3 aeaerror = aea.GetAttitudeError();
					errors = _V(aeaerror.x, -aeaerror.y, -aeaerror.z)*DEG*41.0 / 15.0;

					if (DeadBandSwitch.IsUp())
					{
						errors *= 15.0 / 14.4;
					}
					else
					{
						errors *= 15.0 / 1.7;
					}
				}
			}

			if (RateScaleSwitch.IsUp())
			{
				rates = rga.GetRates() / (25.0*RAD);
			}
			else
			{
				rates = rga.GetRates() / (5.0*RAD);
			}

			fdaiRight.AnimateFDAI(rates, errors, anim_fdaiR_lmp, anim_fdaiP_lmp, anim_fdaiY_lmp, anim_fdaiRerror_lmp, anim_fdaiPerror_lmp, anim_fdaiYerror_lmp, anim_fdaiRrate_lmp, anim_fdaiPrate_lmp, anim_fdaiYrate_lmp);
			SetAnimation(anim_attflag_lmp, 0.0);
		}
		else
		{
			SetAnimation(anim_attflag_lmp, 1.0);
		}
		return true;

	case AID_VC_RR_NOTRACK:
		if (lca.GetAnnunVoltage() > 2.25 && (RR.GetNoTrackSignal() || LampToneTestRotary.GetState() == 6)) { // The AC side is only needed for the transmitter
			SetCompLight(VC_MAT_L12_CompLight1_RRnottrack, true); // Light On
		}
		else {
			SetCompLight(VC_MAT_L12_CompLight1_RRnottrack, false); // Light Off
		}
		return true;

	case AID_VC_PANEL2_COMPLIGHTS:
		if (lca.GetAnnunVoltage() > 2.25 && (scera2.GetSwitch(12, 2)->IsClosed() || PrimGlycolPumpController.GetPressureSwitch() == true || LampToneTestRotary.GetState() == 6)) {
			SetCompLight(VC_MAT_L13_CompLight2_Glycol, true); // Light On
		}
		else {
			SetCompLight(VC_MAT_L13_CompLight2_Glycol, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25 && (SuitFanDPSensor.GetSuitFanFail() == true || LampToneTestRotary.GetState() == 6)) {
			SetCompLight(VC_MAT_L14_CompLight3_SuitFan, true); // Light On
		}
		else {
			SetCompLight(VC_MAT_L14_CompLight3_SuitFan, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25) {
			if (INST_CWEA_CB.IsPowered() && ECS_CO2_SENSOR_CB.IsPowered() && (scera1.GetVoltage(5, 2) >= (7.6 / 6))) {
				SetCompLight(VC_MAT_L15_CompLight4_CO2, true); // Light On
			}
			else if (CO2CanisterSelectSwitch.GetState() == 0 || LampToneTestRotary.GetState() == 6) {
				SetCompLight(VC_MAT_L15_CompLight4_CO2, true); // Light On
			}
			else {
				SetCompLight(VC_MAT_L15_CompLight4_CO2, false); // Light Off
			}
		}
		else {
			SetCompLight(VC_MAT_L15_CompLight4_CO2, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25 && INST_CWEA_CB.IsPowered() && (scera1.GetVoltage(5, 3) < (792.5 / 720.0) || LampToneTestRotary.GetState() == 6)) {
			SetCompLight(VC_MAT_L16_CompLight5_H2Osep, true); // Light On
		}
		else {
			SetCompLight(VC_MAT_L16_CompLight5_H2Osep, false); // Light Off
		}
		return true;

	case AID_VC_PANEL14_COMPLIGHTS:
		if (lca.GetCompDockVoltage() > 2.25 && (LampToneTestRotary.GetState() == 6)) {
			SetCompLight(VC_MAT_L17_CompLight6_DCBus, true); // Light On
		}
		else {
			SetCompLight(VC_MAT_L17_CompLight6_DCBus, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25 && (LampToneTestRotary.GetState() == 6)) {
			SetCompLight(VC_MAT_L18_CompLight7_BatFault, true); // Light On
		}
		else {
			SetCompLight(VC_MAT_L18_CompLight7_BatFault, false); // Light Off
		}
		return true;

	case AID_VC_SEQ_LIGHTS:
		if (lca.GetCompDockVoltage() > 2.25 && (scera1.GetVoltage(12, 11) > 2.5 && stage < 2 || LampToneTestRotary.GetState() == 6)) {
			SetStageSeqRelayLight(VC_MAT_L19_StageSeq_SysA, true); // Light On
		}
		else {
			SetStageSeqRelayLight(VC_MAT_L19_StageSeq_SysA, false); // Light Off
		}

		if (lca.GetCompDockVoltage() > 2.25 && (scera1.GetVoltage(12, 12) > 2.5 || LampToneTestRotary.GetState() == 6)) {
			SetStageSeqRelayLight(VC_MAT_L20_StageSeq_SysB, true); // Light On
		}
		else {
			SetStageSeqRelayLight(VC_MAT_L20_StageSeq_SysB, false); // Light Off
		}
		return true;

	case AID_VC_PWRFAIL_LIGHTS_P1:
		if (!pfira.GetCDRXPointerRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(VC_MAT_L01_PwrFail_XpointerL, true); // Light On
		}
		else {
			SetPowerFailureLight(VC_MAT_L01_PwrFail_XpointerL, false); // Light Off
		}

		if (!pfira.GetThrustIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(VC_MAT_L02_PwrFail_Thrust, true); // Light On
		}
		else {
			SetPowerFailureLight(VC_MAT_L02_PwrFail_Thrust, false); // Light Off
		}

		if (!pfira.GetPropPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(VC_MAT_L03_PwrFail_DPSpress, true); // Light On
		}
		else {
			SetPowerFailureLight(VC_MAT_L03_PwrFail_DPSpress, false); // Light Off
		}

		if (RadarTape.PowerSignalMonOn() == true) {
			SetPowerFailureLight(VC_MAT_L21_PwrFail_RangeRate, true); // Light On
		}
		else {
			SetPowerFailureLight(VC_MAT_L21_PwrFail_RangeRate, false); // Light Off
		}
		return true;

	case AID_VC_PWRFAIL_LIGHTS_P2:

		if (!pfira.GetRCSPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(VC_MAT_L04_PwrFail_RCSpress, true); // Light On
		}
		else {
			SetPowerFailureLight(VC_MAT_L04_PwrFail_RCSpress, false); // Light Off
		}

		if (!pfira.GetRCSQtyIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(VC_MAT_L05_PwrFail_RCSquan, true); // Light On
		}
		else {
			SetPowerFailureLight(VC_MAT_L05_PwrFail_RCSquan, false); // Light Off
		}

		if (!pfira.GetSuitCabinPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(VC_MAT_L06_PwrFail_ECSpress, true); // Light On
		}
		else {
			SetPowerFailureLight(VC_MAT_L06_PwrFail_ECSpress, false); // Light Off
		}

		if (!pfira.GetGlyTempPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(VC_MAT_L07_PwrFail_Glycol, true); // Light On
		}
		else {
			SetPowerFailureLight(VC_MAT_L07_PwrFail_Glycol, false); // Light Off
		}

		if (!pfira.GetO2H2OQtyIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(VC_MAT_L08_PwrFail_ECSquan, true); // Light On
		}
		else {
			SetPowerFailureLight(VC_MAT_L08_PwrFail_ECSquan, false); // Light Off
		}

		if (!pfira.GetLMPXPointerRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(VC_MAT_L09_PwrFail_XpointerR, true); // Light On
		}
		else {
			SetPowerFailureLight(VC_MAT_L09_PwrFail_XpointerR, false); // Light Off
		}
		return true;

	case AID_VC_RETICLEDISP:
		optics.PaintReticleAngle(surf, srf[SRF_AOTFONT_VC]);
		return true;

	case AID_VC_START_BUTTON_RED:
		ManualEngineStart.DoDrawSwitchVC(surf, srf[SRF_ENGSTARTSTOP_VC], TexMul);
		return true;
	}

	return MainPanelVC.VCRedrawEvent(id, event, surf);
	//return false;
}

void LEM::InitVCAnimations() {

	anim_fdaiR_cdr = anim_fdaiR_lmp = -1;
	anim_fdaiP_cdr = anim_fdaiP_lmp = -1;
	anim_fdaiY_cdr = anim_fdaiY_lmp = -1;
	anim_fdaiRerror_cdr = anim_fdaiRerror_lmp = -1;
	anim_fdaiPerror_cdr = anim_fdaiPerror_lmp = -1;
	anim_fdaiYerror_cdr = anim_fdaiYerror_lmp = -1;
	anim_fdaiRrate_cdr = anim_fdaiRrate_lmp = -1;
	anim_fdaiPrate_cdr = anim_fdaiPrate_lmp = -1;
	anim_fdaiYrate_cdr = anim_fdaiYrate_lmp = -1;
}

void LEM::DefineVCAnimations()
{
	MainPanelVC.ClearSwitches();

	//Panel 1

	MainPanelVC.AddSwitch(&RateErrorMonSwitch, AID_VC_SWITCH_P1_01);
	RateErrorMonSwitch.SetReference(P1_TOGGLE_POS[0], _V(1, 0, 0));
	RateErrorMonSwitch.DefineMeshGroup(VC_GRP_Sw_P1_01);

	MainPanelVC.AddSwitch(&AttitudeMonSwitch, AID_VC_SWITCH_P1_02);
	AttitudeMonSwitch.SetReference(P1_TOGGLE_POS[1], _V(1, 0, 0));
	AttitudeMonSwitch.DefineMeshGroup(VC_GRP_Sw_P1_02);

	MainPanelVC.AddSwitch(&LeftXPointerSwitch, AID_VC_SWITCH_P1_03);
	LeftXPointerSwitch.SetReference(P1_TOGGLE_POS[2], _V(1, 0, 0));
	LeftXPointerSwitch.DefineMeshGroup(VC_GRP_Sw_P1_03);

	MainPanelVC.AddSwitch(&GuidContSwitch, AID_VC_SWITCH_P1_04);
	GuidContSwitch.SetReference(P1_TOGGLE_POS[3], _V(1, 0, 0));
	GuidContSwitch.DefineMeshGroup(VC_GRP_Sw_P1_04);

	MainPanelVC.AddSwitch(&ModeSelSwitch, AID_VC_SWITCH_P1_05);
	ModeSelSwitch.SetReference(P1_TOGGLE_POS[4], _V(1, 0, 0));
	ModeSelSwitch.DefineMeshGroup(VC_GRP_Sw_P1_05);

	MainPanelVC.AddSwitch(&AltRngMonSwitch, AID_VC_SWITCH_P1_06);
	AltRngMonSwitch.SetReference(P1_TOGGLE_POS[5], _V(1, 0, 0));
	AltRngMonSwitch.DefineMeshGroup(VC_GRP_Sw_P1_06);

	MainPanelVC.AddSwitch(&ShiftTruSwitch, AID_VC_SWITCH_P1_07);
	ShiftTruSwitch.SetReference(P1_TOGGLE_POS[6], _V(1, 0, 0));
	ShiftTruSwitch.DefineMeshGroup(VC_GRP_Sw_P1_07);

	MainPanelVC.AddSwitch(&RateScaleSwitch, AID_VC_SWITCH_P1_08);
	RateScaleSwitch.SetReference(P1_TOGGLE_POS[7], _V(1, 0, 0));
	RateScaleSwitch.DefineMeshGroup(VC_GRP_Sw_P1_08);

	MainPanelVC.AddSwitch(&ACAPropSwitch, AID_VC_SWITCH_P1_09);
	ACAPropSwitch.SetReference(P1_TOGGLE_POS[8], _V(1, 0, 0));
	ACAPropSwitch.DefineMeshGroup(VC_GRP_Sw_P1_09);

	MainPanelVC.AddSwitch(&THRContSwitch, AID_VC_SWITCH_P1_10);
	THRContSwitch.SetReference(P1_TOGGLE_POS[9], _V(1, 0, 0));
	THRContSwitch.DefineMeshGroup(VC_GRP_Sw_P1_10);

	MainPanelVC.AddSwitch(&MANThrotSwitch, AID_VC_SWITCH_P1_11);
	MANThrotSwitch.SetReference(P1_TOGGLE_POS[10], _V(1, 0, 0));
	MANThrotSwitch.DefineMeshGroup(VC_GRP_Sw_P1_11);

	MainPanelVC.AddSwitch(&EngineArmSwitch, AID_VC_SWITCH_P1_12);
	EngineArmSwitch.SetReference(P1_TOGGLE_POS[11], _V(1, 0, 0));
	EngineArmSwitch.DefineMeshGroup(VC_GRP_Sw_P1_12);

	MainPanelVC.AddSwitch(&ATTTranslSwitch, AID_VC_SWITCH_P1_13);
	ATTTranslSwitch.SetReference(P1_TOGGLE_POS[12], _V(1, 0, 0));
	ATTTranslSwitch.DefineMeshGroup(VC_GRP_Sw_P1_13);

	MainPanelVC.AddSwitch(&BALCPLSwitch, AID_VC_SWITCH_P1_14);
	BALCPLSwitch.SetReference(P1_TOGGLE_POS[13], _V(1, 0, 0));
	BALCPLSwitch.DefineMeshGroup(VC_GRP_Sw_P1_14);

	MainPanelVC.AddSwitch(&ASCHeReg1Switch, AID_VC_SWITCH_P1_15);
	ASCHeReg1Switch.SetReference(P1_TOGGLE_POS[14], _V(1, 0, 0));
	ASCHeReg1Switch.DefineMeshGroup(VC_GRP_Sw_P1_15);

	MainPanelVC.AddSwitch(&ASCHeReg2Switch, AID_VC_SWITCH_P1_16);
	ASCHeReg2Switch.SetReference(P1_TOGGLE_POS[15], _V(1, 0, 0));
	ASCHeReg2Switch.DefineMeshGroup(VC_GRP_Sw_P1_16);

	MainPanelVC.AddSwitch(&DESHeReg1Switch, AID_VC_SWITCH_P1_17);
	DESHeReg1Switch.SetReference(P1_TOGGLE_POS[16], _V(1, 0, 0));
	DESHeReg1Switch.DefineMeshGroup(VC_GRP_Sw_P1_17);

	MainPanelVC.AddSwitch(&DESHeReg2Switch, AID_VC_SWITCH_P1_18);
	DESHeReg2Switch.SetReference(P1_TOGGLE_POS[17], _V(1, 0, 0));
	DESHeReg2Switch.DefineMeshGroup(VC_GRP_Sw_P1_18);

	MainPanelVC.AddSwitch(&QTYMonSwitch, AID_VC_SWITCH_P1_19);
	QTYMonSwitch.SetReference(P1_TOGGLE_POS[18], _V(1, 0, 0));
	QTYMonSwitch.DefineMeshGroup(VC_GRP_Sw_P1_19);

	MainPanelVC.AddSwitch(&TempPressMonSwitch, AID_VC_SWITCH_P1_20);
	TempPressMonSwitch.SetReference(P1_TOGGLE_POS[19], _V(1, 0, 0));
	TempPressMonSwitch.DefineMeshGroup(VC_GRP_Sw_P1_20);

	MainPanelVC.AddSwitch(&HeliumMonRotary, AID_VC_ROT_P1_01);
	HeliumMonRotary.SetReference(P1_ROT_POS[0], P1_ROT_AXIS);
	HeliumMonRotary.DefineMeshGroup(VC_GRP_Rot_P1_01);

	VECTOR3 NEEDLE_POS = { -0.1278, 0.6781, 1.8072 };

	MainPanelVC.AddSwitch(&EngineThrustInd);
	EngineThrustInd.SetReference(NEEDLE_POS);
	EngineThrustInd.SetRotationRange(RAD * 38.6);
	EngineThrustInd.DefineMeshGroup(VC_GRP_Needle_P1_01);

	MainPanelVC.AddSwitch(&CommandedThrustInd);
	CommandedThrustInd.SetReference(NEEDLE_POS);
	CommandedThrustInd.SetRotationRange(RAD * 38.6);
	CommandedThrustInd.DefineMeshGroup(VC_GRP_Needle_P1_02);

	MainPanelVC.AddSwitch(&MainFuelTempInd);
	MainFuelTempInd.SetReference(NEEDLE_POS);
	MainFuelTempInd.SetRotationRange(RAD * 38.6);
	MainFuelTempInd.DefineMeshGroup(VC_GRP_Needle_P1_03);

	MainPanelVC.AddSwitch(&MainOxidizerTempInd);
	MainOxidizerTempInd.SetReference(NEEDLE_POS);
	MainOxidizerTempInd.SetRotationRange(RAD * 38.6);
	MainOxidizerTempInd.DefineMeshGroup(VC_GRP_Needle_P1_04);

	MainPanelVC.AddSwitch(&MainFuelPressInd);
	MainFuelPressInd.SetReference(NEEDLE_POS);
	MainFuelPressInd.SetRotationRange(RAD * 38.6);
	MainFuelPressInd.DefineMeshGroup(VC_GRP_Needle_P1_05);

	MainPanelVC.AddSwitch(&MainOxidizerPressInd);
	MainOxidizerPressInd.SetReference(NEEDLE_POS);
	MainOxidizerPressInd.SetRotationRange(RAD * 38.6);
	MainOxidizerPressInd.DefineMeshGroup(VC_GRP_Needle_P1_06);

	MainPanelVC.AddSwitch(&ThrustWeightInd);
	ThrustWeightInd.SetDirection(_V(0.00, 0.0925*cos(P1_TILT), 0.0925*sin(P1_TILT)));
	ThrustWeightInd.DefineMeshGroup(VC_GRP_Needle_P1_07);

	MainPanelVC.AddSwitch(&ASCHeReg1TB, AID_VC_MPS_REG_CONTROLS_LEFT);
	MainPanelVC.AddSwitch(&DESHeReg1TB, AID_VC_MPS_REG_CONTROLS_LEFT);
	MainPanelVC.AddSwitch(&ASCHeReg2TB, AID_VC_MPS_REG_CONTROLS_RIGHT);
	MainPanelVC.AddSwitch(&DESHeReg2TB, AID_VC_MPS_REG_CONTROLS_RIGHT);

	MainPanelVC.AddSwitch(&DPSOxidPercentMeter, AID_VC_MPS_OXID_QUANTITY_INDICATOR);
	MainPanelVC.AddSwitch(&DPSFuelPercentMeter, AID_VC_MPS_FUEL_QUANTITY_INDICATOR);
	MainPanelVC.AddSwitch(&MainHeliumPressureMeter, AID_VC_MPS_HELIUM_PRESS_INDICATOR);

	const VECTOR3 abortbuttonvector = _V(0.0, 0.004*cos(P1_TILT - (90.0 * RAD)), 0.004*sin(P1_TILT - (90.0 * RAD)));
	MainPanelVC.AddSwitch(&AbortSwitch, AID_VC_ABORT_BUTTON);
	AbortSwitch.SetDirection(abortbuttonvector);
	AbortSwitch.DefineMeshGroup(VC_GRP_AbortButton);

	MainPanelVC.AddSwitch(&AbortStageSwitch, AID_VC_ABORTSTAGE_BUTTON);
	AbortStageSwitch.SetReference(abortbuttonvector, _V(-0.045187, 0.468451, 1.68831), _V(1, 0, 0));
	AbortStageSwitch.SetCoverRotationAngle(100.0*RAD);
	AbortStageSwitch.DefineMeshGroup(VC_GRP_AbortStageButton, VC_GRP_AbortStageCover);
	AbortStageSwitch.SetInitialAnimState(1.0);

	VECTOR3 xvector = { 0.00, 0.075*cos(P1_TILT), 0.075*sin(P1_TILT) };
	VECTOR3 yvector = { 0.075, 0, 0 };
	crossPointerLeft.SetDirection(xvector, yvector);
	crossPointerLeft.DefineMeshGroup(VC_GRP_XpointerX_cdr, VC_GRP_XpointerY_cdr);

	crossPointerRight.SetDirection(xvector, yvector);
	crossPointerRight.DefineMeshGroup(VC_GRP_XpointerX_lmp, VC_GRP_XpointerY_lmp);

	MainPanelVC.AddSwitch(&LeftMasterAlarmSwitch, AID_VC_LEM_MA_LEFT);

	//Panel 2

	MainPanelVC.AddSwitch(&RCSAscFeed1ASwitch, AID_VC_SWITCH_P2_01);
	RCSAscFeed1ASwitch.SetReference(P2_TOGGLE_POS[0], _V(1, 0, 0));
	RCSAscFeed1ASwitch.DefineMeshGroup(VC_GRP_Sw_P2_01);

	MainPanelVC.AddSwitch(&RCSAscFeed2ASwitch, AID_VC_SWITCH_P2_02);
	RCSAscFeed2ASwitch.SetReference(P2_TOGGLE_POS[1], _V(1, 0, 0));
	RCSAscFeed2ASwitch.DefineMeshGroup(VC_GRP_Sw_P2_02);

	MainPanelVC.AddSwitch(&RCSAscFeed1BSwitch, AID_VC_SWITCH_P2_03);
	RCSAscFeed1BSwitch.SetReference(P2_TOGGLE_POS[2], _V(1, 0, 0));
	RCSAscFeed1BSwitch.DefineMeshGroup(VC_GRP_Sw_P2_03);

	MainPanelVC.AddSwitch(&RCSAscFeed2BSwitch, AID_VC_SWITCH_P2_04);
	RCSAscFeed2BSwitch.SetReference(P2_TOGGLE_POS[3], _V(1, 0, 0));
	RCSAscFeed2BSwitch.DefineMeshGroup(VC_GRP_Sw_P2_04);

	MainPanelVC.AddSwitch(&RCSQuad1ACmdEnableSwitch, AID_VC_SWITCH_P2_05);
	RCSQuad1ACmdEnableSwitch.SetReference(P2_TOGGLE_POS[4], _V(1, 0, 0));
	RCSQuad1ACmdEnableSwitch.DefineMeshGroup(VC_GRP_Sw_P2_05);

	MainPanelVC.AddSwitch(&RCSQuad4ACmdEnableSwitch, AID_VC_SWITCH_P2_06);
	RCSQuad4ACmdEnableSwitch.SetReference(P2_TOGGLE_POS[5], _V(1, 0, 0));
	RCSQuad4ACmdEnableSwitch.DefineMeshGroup(VC_GRP_Sw_P2_06);

	MainPanelVC.AddSwitch(&RCSQuad1BCmdEnableSwitch, AID_VC_SWITCH_P2_07);
	RCSQuad1BCmdEnableSwitch.SetReference(P2_TOGGLE_POS[6], _V(1, 0, 0));
	RCSQuad1BCmdEnableSwitch.DefineMeshGroup(VC_GRP_Sw_P2_07);

	MainPanelVC.AddSwitch(&RCSQuad4BCmdEnableSwitch, AID_VC_SWITCH_P2_08);
	RCSQuad4BCmdEnableSwitch.SetReference(P2_TOGGLE_POS[7], _V(1, 0, 0));
	RCSQuad4BCmdEnableSwitch.DefineMeshGroup(VC_GRP_Sw_P2_08);

	MainPanelVC.AddSwitch(&RCSQuad2ACmdEnableSwitch, AID_VC_SWITCH_P2_09);
	RCSQuad2ACmdEnableSwitch.SetReference(P2_TOGGLE_POS[8], _V(1, 0, 0));
	RCSQuad2ACmdEnableSwitch.DefineMeshGroup(VC_GRP_Sw_P2_09);

	MainPanelVC.AddSwitch(&RCSQuad3ACmdEnableSwitch, AID_VC_SWITCH_P2_10);
	RCSQuad3ACmdEnableSwitch.SetReference(P2_TOGGLE_POS[9], _V(1, 0, 0));
	RCSQuad3ACmdEnableSwitch.DefineMeshGroup(VC_GRP_Sw_P2_10);

	MainPanelVC.AddSwitch(&RCSQuad2BCmdEnableSwitch, AID_VC_SWITCH_P2_11);
	RCSQuad2BCmdEnableSwitch.SetReference(P2_TOGGLE_POS[10], _V(1, 0, 0));
	RCSQuad2BCmdEnableSwitch.DefineMeshGroup(VC_GRP_Sw_P2_11);

	MainPanelVC.AddSwitch(&RCSQuad3BCmdEnableSwitch, AID_VC_SWITCH_P2_12);
	RCSQuad3BCmdEnableSwitch.SetReference(P2_TOGGLE_POS[11], _V(1, 0, 0));
	RCSQuad3BCmdEnableSwitch.DefineMeshGroup(VC_GRP_Sw_P2_12);

	MainPanelVC.AddSwitch(&RCSXFeedSwitch, AID_VC_SWITCH_P2_13);
	RCSXFeedSwitch.SetReference(P2_TOGGLE_POS[12], _V(1, 0, 0));
	RCSXFeedSwitch.DefineMeshGroup(VC_GRP_Sw_P2_13);

	MainPanelVC.AddSwitch(&RCSMainSovASwitch, AID_VC_SWITCH_P2_14);
	RCSMainSovASwitch.SetReference(P2_TOGGLE_POS[13], _V(1, 0, 0));
	RCSMainSovASwitch.DefineMeshGroup(VC_GRP_Sw_P2_14);

	MainPanelVC.AddSwitch(&RCSMainSovBSwitch, AID_VC_SWITCH_P2_15);
	RCSMainSovBSwitch.SetReference(P2_TOGGLE_POS[14], _V(1, 0, 0));
	RCSMainSovBSwitch.DefineMeshGroup(VC_GRP_Sw_P2_15);

	MainPanelVC.AddSwitch(&RightACAPropSwitch, AID_VC_SWITCH_P2_16);
	RightACAPropSwitch.SetReference(P2_TOGGLE_POS[15], _V(1, 0, 0));
	RightACAPropSwitch.DefineMeshGroup(VC_GRP_Sw_P2_16);

	MainPanelVC.AddSwitch(&RightRateErrorMonSwitch, AID_VC_SWITCH_P2_17);
	RightRateErrorMonSwitch.SetReference(P2_TOGGLE_POS[16], _V(1, 0, 0));
	RightRateErrorMonSwitch.DefineMeshGroup(VC_GRP_Sw_P2_17);

	MainPanelVC.AddSwitch(&RightAttitudeMonSwitch, AID_VC_SWITCH_P2_18);
	RightAttitudeMonSwitch.SetReference(P2_TOGGLE_POS[17], _V(1, 0, 0));
	RightAttitudeMonSwitch.DefineMeshGroup(VC_GRP_Sw_P2_18);

	MainPanelVC.AddSwitch(&TempPressMonRotary, AID_VC_ROT_P2_01);
	TempPressMonRotary.SetReference(P2_ROT_POS[0], P2_ROT_AXIS);
	TempPressMonRotary.DefineMeshGroup(VC_GRP_Rot_P2_01);

	MainPanelVC.AddSwitch(&GlycolRotary, AID_VC_ROT_P2_02);
	GlycolRotary.SetReference(P2_ROT_POS[1], P2_ROT_AXIS);
	GlycolRotary.DefineMeshGroup(VC_GRP_Rot_P2_02);

	MainPanelVC.AddSwitch(&SuitFanRotary, AID_VC_ROT_P2_03);
	SuitFanRotary.SetReference(P2_ROT_POS[2], P2_ROT_AXIS);
	SuitFanRotary.DefineMeshGroup(VC_GRP_Rot_P2_03);

	MainPanelVC.AddSwitch(&QtyMonRotary, AID_VC_ROT_P2_04);
	QtyMonRotary.SetReference(P2_ROT_POS[3], P2_ROT_AXIS);
	QtyMonRotary.DefineMeshGroup(VC_GRP_Rot_P2_04);

	NEEDLE_POS = { 0.1862, 0.7887, 1.8224 };

	MainPanelVC.AddSwitch(&LMRCSATempInd);
	LMRCSATempInd.SetReference(NEEDLE_POS);
	LMRCSATempInd.SetRotationRange(RAD * 38.6);
	LMRCSATempInd.DefineMeshGroup(VC_GRP_Needle_P2_01);

	MainPanelVC.AddSwitch(&LMRCSBTempInd);
	LMRCSBTempInd.SetReference(NEEDLE_POS);
	LMRCSBTempInd.SetRotationRange(RAD * 38.6);
	LMRCSBTempInd.DefineMeshGroup(VC_GRP_Needle_P2_02);

	MainPanelVC.AddSwitch(&LMRCSAPressInd);
	LMRCSAPressInd.SetReference(NEEDLE_POS);
	LMRCSAPressInd.SetRotationRange(RAD * 38.6);
	LMRCSAPressInd.DefineMeshGroup(VC_GRP_Needle_P2_03);

	MainPanelVC.AddSwitch(&LMRCSBPressInd);
	LMRCSBPressInd.SetReference(NEEDLE_POS);
	LMRCSBPressInd.SetRotationRange(RAD * 38.6);
	LMRCSBPressInd.DefineMeshGroup(VC_GRP_Needle_P2_04);

	MainPanelVC.AddSwitch(&LMRCSAQtyInd);
	LMRCSAQtyInd.SetReference(NEEDLE_POS);
	LMRCSAQtyInd.SetRotationRange(RAD * 38.6);
	LMRCSAQtyInd.DefineMeshGroup(VC_GRP_Needle_P2_05);

	MainPanelVC.AddSwitch(&LMRCSBQtyInd);
	LMRCSBQtyInd.SetReference(NEEDLE_POS);
	LMRCSBQtyInd.SetRotationRange(RAD * 38.6);
	LMRCSBQtyInd.DefineMeshGroup(VC_GRP_Needle_P2_06);

	MainPanelVC.AddSwitch(&LMSuitTempMeter);
	LMSuitTempMeter.SetReference(NEEDLE_POS);
	LMSuitTempMeter.SetRotationRange(RAD * 38.6);
	LMSuitTempMeter.DefineMeshGroup(VC_GRP_Needle_P2_07);

	MainPanelVC.AddSwitch(&LMCabinTempMeter);
	LMCabinTempMeter.SetReference(NEEDLE_POS);
	LMCabinTempMeter.SetRotationRange(RAD * 38.6);
	LMCabinTempMeter.DefineMeshGroup(VC_GRP_Needle_P2_08);

	MainPanelVC.AddSwitch(&LMSuitPressMeter);
	LMSuitPressMeter.SetReference(NEEDLE_POS);
	LMSuitPressMeter.SetRotationRange(RAD * 38.6);
	LMSuitPressMeter.DefineMeshGroup(VC_GRP_Needle_P2_09);

	MainPanelVC.AddSwitch(&LMCabinPressMeter);
	LMCabinPressMeter.SetReference(NEEDLE_POS);
	LMCabinPressMeter.SetRotationRange(RAD * 38.6);
	LMCabinPressMeter.DefineMeshGroup(VC_GRP_Needle_P2_10);

	NEEDLE_POS = { 0.2785, 0.6710, 1.8063 };

	MainPanelVC.AddSwitch(&LMGlycolTempMeter);
	LMGlycolTempMeter.SetReference(NEEDLE_POS);
	LMGlycolTempMeter.SetRotationRange(RAD * 38.6);
	LMGlycolTempMeter.DefineMeshGroup(VC_GRP_Needle_P2_11);

	MainPanelVC.AddSwitch(&LMGlycolPressMeter);
	LMGlycolPressMeter.SetReference(NEEDLE_POS);
	LMGlycolPressMeter.SetRotationRange(RAD * 38.6);
	LMGlycolPressMeter.DefineMeshGroup(VC_GRP_Needle_P2_12);

	MainPanelVC.AddSwitch(&LMOxygenQtyMeter);
	LMOxygenQtyMeter.SetReference(NEEDLE_POS);
	LMOxygenQtyMeter.SetRotationRange(RAD * 38.6);
	LMOxygenQtyMeter.DefineMeshGroup(VC_GRP_Needle_P2_13);

	MainPanelVC.AddSwitch(&LMWaterQtyMeter);
	LMWaterQtyMeter.SetReference(NEEDLE_POS);
	LMWaterQtyMeter.SetRotationRange(RAD * 38.6);
	LMWaterQtyMeter.DefineMeshGroup(VC_GRP_Needle_P2_14);

	NEEDLE_POS = { 0.1862, 0.7887, 1.8224 };

	MainPanelVC.AddSwitch(&LMCO2Meter);
	LMCO2Meter.SetReference(NEEDLE_POS);
	LMCO2Meter.SetRotationRange(RAD * 38.6);
	LMCO2Meter.DefineMeshGroup(VC_GRP_Needle_P2_15);

	MainPanelVC.AddSwitch(&RCSAscFeed1ATB, AID_VC_RCS_ASC_FEED_TALKBACKS);
	MainPanelVC.AddSwitch(&RCSAscFeed2ATB, AID_VC_RCS_ASC_FEED_TALKBACKS);
	MainPanelVC.AddSwitch(&RCSAscFeed1BTB, AID_VC_RCS_ASC_FEED_TALKBACKS);
	MainPanelVC.AddSwitch(&RCSAscFeed2BTB, AID_VC_RCS_ASC_FEED_TALKBACKS);

	MainPanelVC.AddSwitch(&RCSQuad1ACmdEnableTB, AID_VC_LGC_CMD_ENABLE_14_TALKBACKS);
	MainPanelVC.AddSwitch(&RCSQuad4ACmdEnableTB, AID_VC_LGC_CMD_ENABLE_14_TALKBACKS);
	MainPanelVC.AddSwitch(&RCSQuad1BCmdEnableTB, AID_VC_LGC_CMD_ENABLE_14_TALKBACKS);
	MainPanelVC.AddSwitch(&RCSQuad4BCmdEnableTB, AID_VC_LGC_CMD_ENABLE_14_TALKBACKS);

	MainPanelVC.AddSwitch(&RCSQuad2ACmdEnableTB, AID_VC_LGC_CMD_ENABLE_23_TALKBACKS);
	MainPanelVC.AddSwitch(&RCSQuad3ACmdEnableTB, AID_VC_LGC_CMD_ENABLE_23_TALKBACKS);
	MainPanelVC.AddSwitch(&RCSQuad2BCmdEnableTB, AID_VC_LGC_CMD_ENABLE_23_TALKBACKS);
	MainPanelVC.AddSwitch(&RCSQuad3BCmdEnableTB, AID_VC_LGC_CMD_ENABLE_23_TALKBACKS);

	MainPanelVC.AddSwitch(&RCSXFeedTB, AID_VC_RCS_XFEED_TALKBACK);

	MainPanelVC.AddSwitch(&RCSMainSovATB, AID_VC_MAIN_SOV_TALKBACKS);
	MainPanelVC.AddSwitch(&RCSMainSovBTB, AID_VC_MAIN_SOV_TALKBACKS);

	MainPanelVC.AddSwitch(&RightMasterAlarmSwitch, AID_VC_LEM_MA_RIGHT);

	//Panel 3

	MainPanelVC.AddSwitch(&EngGimbalEnableSwitch, AID_VC_SWITCH_P3_01);
	EngGimbalEnableSwitch.SetReference(P3_TOGGLE_POS[0], _V(1, 0, 0));
	EngGimbalEnableSwitch.DefineMeshGroup(VC_GRP_Sw_P3_01);

	MainPanelVC.AddSwitch(&EngineDescentCommandOverrideSwitch, AID_VC_SWITCH_P3_02);
	EngineDescentCommandOverrideSwitch.SetReference(P3_TOGGLE_POS[1], _V(1, 0, 0));
	EngineDescentCommandOverrideSwitch.DefineMeshGroup(VC_GRP_Sw_P3_02);

	MainPanelVC.AddSwitch(&LandingAntSwitch, AID_VC_SWITCH_P3_03);
	LandingAntSwitch.SetReference(P3_TOGGLE_POS[2], _V(1, 0, 0));
	LandingAntSwitch.DefineMeshGroup(VC_GRP_Sw_P3_03);

	MainPanelVC.AddSwitch(&RadarTestSwitch, AID_VC_SWITCH_P3_04);
	RadarTestSwitch.SetReference(P3_TOGGLE_POS[3], _V(1, 0, 0));
	RadarTestSwitch.DefineMeshGroup(VC_GRP_Sw_P3_04);

	MainPanelVC.AddSwitch(&SlewRateSwitch, AID_VC_SWITCH_P3_05);
	SlewRateSwitch.SetReference(P3_TOGGLE_POS[4], _V(1, 0, 0));
	SlewRateSwitch.DefineMeshGroup(VC_GRP_Sw_P3_05);

	MainPanelVC.AddSwitch(&DeadBandSwitch, AID_VC_SWITCH_P3_06);
	DeadBandSwitch.SetReference(P3_TOGGLE_POS[5], _V(1, 0, 0));
	DeadBandSwitch.DefineMeshGroup(VC_GRP_Sw_P3_06);

	MainPanelVC.AddSwitch(&GyroTestLeftSwitch, AID_VC_SWITCH_P3_07);
	GyroTestLeftSwitch.SetReference(P3_TOGGLE_POS[6], _V(1, 0, 0));
	GyroTestLeftSwitch.DefineMeshGroup(VC_GRP_Sw_P3_07);

	MainPanelVC.AddSwitch(&GyroTestRightSwitch, AID_VC_SWITCH_P3_08);
	GyroTestRightSwitch.SetReference(P3_TOGGLE_POS[7], _V(1, 0, 0));
	GyroTestRightSwitch.DefineMeshGroup(VC_GRP_Sw_P3_08);

	MainPanelVC.AddSwitch(&RollSwitch, AID_VC_SWITCH_P3_09);
	RollSwitch.SetReference(P3_TOGGLE_POS[8], _V(1, 0, 0));
	RollSwitch.DefineMeshGroup(VC_GRP_Sw_P3_09);

	MainPanelVC.AddSwitch(&PitchSwitch, AID_VC_SWITCH_P3_10);
	PitchSwitch.SetReference(P3_TOGGLE_POS[9], _V(1, 0, 0));
	PitchSwitch.DefineMeshGroup(VC_GRP_Sw_P3_10);

	MainPanelVC.AddSwitch(&YawSwitch, AID_VC_SWITCH_P3_11);
	YawSwitch.SetReference(P3_TOGGLE_POS[10], _V(1, 0, 0));
	YawSwitch.DefineMeshGroup(VC_GRP_Sw_P3_11);

	MainPanelVC.AddSwitch(&ModeControlPGNSSwitch, AID_VC_SWITCH_P3_12);
	ModeControlPGNSSwitch.SetReference(P3_TOGGLE_POS[11], _V(1, 0, 0));
	ModeControlPGNSSwitch.DefineMeshGroup(VC_GRP_Sw_P3_12);

	MainPanelVC.AddSwitch(&ModeControlAGSSwitch, AID_VC_SWITCH_P3_13);
	ModeControlAGSSwitch.SetReference(P3_TOGGLE_POS[12], _V(1, 0, 0));
	ModeControlAGSSwitch.DefineMeshGroup(VC_GRP_Sw_P3_13);

	MainPanelVC.AddSwitch(&IMUCageSwitch, AID_VC_SWITCH_P3_14);
	IMUCageSwitch.SetReference(P3_TOGGLE_POS[13], _V(1, 0, 0));
	IMUCageSwitch.DefineMeshGroup(VC_GRP_Sw_P3_14);

	MainPanelVC.AddSwitch(&EventTimerCtlSwitch, AID_VC_SWITCH_P3_15);
	EventTimerCtlSwitch.SetReference(P3_TOGGLE_POS[14], _V(1, 0, 0));
	EventTimerCtlSwitch.DefineMeshGroup(VC_GRP_Sw_P3_15);

	MainPanelVC.AddSwitch(&EventTimerStartSwitch, AID_VC_SWITCH_P3_16);
	EventTimerStartSwitch.SetReference(P3_TOGGLE_POS[15], _V(1, 0, 0));
	EventTimerStartSwitch.DefineMeshGroup(VC_GRP_Sw_P3_16);

	MainPanelVC.AddSwitch(&EventTimerMinuteSwitch, AID_VC_SWITCH_P3_17);
	EventTimerMinuteSwitch.SetReference(P3_TOGGLE_POS[16], _V(1, 0, 0));
	EventTimerMinuteSwitch.DefineMeshGroup(VC_GRP_Sw_P3_17);

	MainPanelVC.AddSwitch(&EventTimerSecondSwitch, AID_VC_SWITCH_P3_18);
	EventTimerSecondSwitch.SetReference(P3_TOGGLE_POS[17], _V(1, 0, 0));
	EventTimerSecondSwitch.DefineMeshGroup(VC_GRP_Sw_P3_18);

	MainPanelVC.AddSwitch(&RCSSysQuad1Switch, AID_VC_SWITCH_P3_19);
	RCSSysQuad1Switch.SetReference(P3_TOGGLE_POS[18], _V(1, 0, 0));
	RCSSysQuad1Switch.DefineMeshGroup(VC_GRP_Sw_P3_19);

	MainPanelVC.AddSwitch(&RCSSysQuad4Switch, AID_VC_SWITCH_P3_20);
	RCSSysQuad4Switch.SetReference(P3_TOGGLE_POS[19], _V(1, 0, 0));
	RCSSysQuad4Switch.DefineMeshGroup(VC_GRP_Sw_P3_20);

	MainPanelVC.AddSwitch(&RCSSysQuad2Switch, AID_VC_SWITCH_P3_21);
	RCSSysQuad2Switch.SetReference(P3_TOGGLE_POS[20], _V(1, 0, 0));
	RCSSysQuad2Switch.DefineMeshGroup(VC_GRP_Sw_P3_21);

	MainPanelVC.AddSwitch(&RCSSysQuad3Switch, AID_VC_SWITCH_P3_22);
	RCSSysQuad3Switch.SetReference(P3_TOGGLE_POS[21], _V(1, 0, 0));
	RCSSysQuad3Switch.DefineMeshGroup(VC_GRP_Sw_P3_22);

	MainPanelVC.AddSwitch(&SidePanelsSwitch, AID_VC_SWITCH_P3_23);
	SidePanelsSwitch.SetReference(P3_TOGGLE_POS[22], _V(1, 0, 0));
	SidePanelsSwitch.DefineMeshGroup(VC_GRP_Sw_P3_23);

	MainPanelVC.AddSwitch(&FloodSwitch, AID_VC_SWITCH_P3_24);
	FloodSwitch.SetReference(P3_TOGGLE_POS[23], _V(1, 0, 0));
	FloodSwitch.DefineMeshGroup(VC_GRP_Sw_P3_24);

	MainPanelVC.AddSwitch(&RightXPointerSwitch, AID_VC_SWITCH_P3_25);
	RightXPointerSwitch.SetReference(P3_TOGGLE_POS[24], _V(1, 0, 0));
	RightXPointerSwitch.DefineMeshGroup(VC_GRP_Sw_P3_25);

	MainPanelVC.AddSwitch(&ExteriorLTGSwitch, AID_VC_SWITCH_P3_26);
	ExteriorLTGSwitch.SetReference(P3_TOGGLE_POS[25], _V(1, 0, 0));
	ExteriorLTGSwitch.DefineMeshGroup(VC_GRP_Sw_P3_26);

	MainPanelVC.AddSwitch(&TestMonitorRotary, AID_VC_ROT_P3_01);
	TestMonitorRotary.SetReference(P3_ROT_POS[0], P3_ROT_AXIS);
	TestMonitorRotary.DefineMeshGroup(VC_GRP_Rot_P3_01);

	MainPanelVC.AddSwitch(&RendezvousRadarRotary, AID_VC_ROT_P3_02);
	RendezvousRadarRotary.SetReference(P3_ROT_POS[1], P3_ROT_AXIS);
	RendezvousRadarRotary.DefineMeshGroup(VC_GRP_Rot_P3_02);

	MainPanelVC.AddSwitch(&TempMonitorRotary, AID_VC_ROT_P3_03);
	TempMonitorRotary.SetReference(P3_ROT_POS[2], P3_ROT_AXIS);
	TempMonitorRotary.DefineMeshGroup(VC_GRP_Rot_P3_03);

	MainPanelVC.AddSwitch(&LampToneTestRotary, AID_VC_ROT_P3_04);
	LampToneTestRotary.SetReference(P3_ROT_POS[3], P3_ROT_AXIS);
	LampToneTestRotary.DefineMeshGroup(VC_GRP_Rot_P3_04);

	MainPanelVC.AddSwitch(&FloodRotary, AID_VC_ROT_P3_05);
	FloodRotary.SetReference(P3_ROT_POS[4], P3_ROT_AXIS);
	FloodRotary.DefineMeshGroup(VC_GRP_Rot_P3_05);
	FloodRotary.SetInitialAnimState(0.5);

	NEEDLE_POS = {0,0.179865,1.699588};

	MainPanelVC.AddSwitch(&TempMonitorInd);
	//TempMonitorInd.SetDirection(_V(0.00, 0.0625 * cos(P3_TILT), 0.0625 * sin(P3_TILT)));
	//TempMonitorInd.DefineMeshGroup(VC_GRP_Needle_P3_01);
	TempMonitorInd.SetReference(NEEDLE_POS);
	TempMonitorInd.SetRotationRange(RAD * 38.6);
	TempMonitorInd.DefineMeshGroup(VC_GRP_Needle_P3_01);

	MainPanelVC.AddSwitch(&RadarSignalStrengthMeter);
	RadarSignalStrengthMeter.SetReference(_V(-0.264141, 0.235696, 1.62835), P3_ROT_AXIS);
	RadarSignalStrengthMeter.DefineMeshGroup(VC_GRP_Needle_Radar);

	MainPanelVC.AddSwitch(&RadarSlewSwitch, AID_VC_RR_SLEW_SWITCH);
	RadarSlewSwitch.SetReference(_V(-0.264179, 0.149389, 1.56749));
	RadarSlewSwitch.SetDirection(_V(-0.00, -sin(P3_TILT + (90.0 * RAD)), cos(P3_TILT + (90.0 * RAD))), _V(1, 0, 0));
	RadarSlewSwitch.DefineMeshGroup(VC_GRP_RR_slew_switch);

	//Panel 4

	const VECTOR3	P4_PB_VECT = { 0.00,  0.001*-sin(P4_TILT), 0.001*cos(P4_TILT) };

	MainPanelVC.AddSwitch(&DskySwitchVerb, AID_VC_PUSHB_P4_01);
	DskySwitchVerb.SetDirection(P4_PB_VECT);
	DskySwitchVerb.DefineMeshGroup(VC_GRP_PB_P4_01);

	MainPanelVC.AddSwitch(&DskySwitchNoun, AID_VC_PUSHB_P4_02);
	DskySwitchNoun.SetDirection(P4_PB_VECT);
	DskySwitchNoun.DefineMeshGroup(VC_GRP_PB_P4_02);

	MainPanelVC.AddSwitch(&DskySwitchPlus, AID_VC_PUSHB_P4_03);
	DskySwitchPlus.SetDirection(P4_PB_VECT);
	DskySwitchPlus.DefineMeshGroup(VC_GRP_PB_P4_03);

	MainPanelVC.AddSwitch(&DskySwitchMinus, AID_VC_PUSHB_P4_04);
	DskySwitchMinus.SetDirection(P4_PB_VECT);
	DskySwitchMinus.DefineMeshGroup(VC_GRP_PB_P4_04);

	MainPanelVC.AddSwitch(&DskySwitchZero, AID_VC_PUSHB_P4_05);
	DskySwitchZero.SetDirection(P4_PB_VECT);
	DskySwitchZero.DefineMeshGroup(VC_GRP_PB_P4_05);

	MainPanelVC.AddSwitch(&DskySwitchOne, AID_VC_PUSHB_P4_06);
	DskySwitchOne.SetDirection(P4_PB_VECT);
	DskySwitchOne.DefineMeshGroup(VC_GRP_PB_P4_06);

	MainPanelVC.AddSwitch(&DskySwitchTwo, AID_VC_PUSHB_P4_07);
	DskySwitchTwo.SetDirection(P4_PB_VECT);
	DskySwitchTwo.DefineMeshGroup(VC_GRP_PB_P4_07);

	MainPanelVC.AddSwitch(&DskySwitchThree, AID_VC_PUSHB_P4_08);
	DskySwitchThree.SetDirection(P4_PB_VECT);
	DskySwitchThree.DefineMeshGroup(VC_GRP_PB_P4_08);

	MainPanelVC.AddSwitch(&DskySwitchFour, AID_VC_PUSHB_P4_09);
	DskySwitchFour.SetDirection(P4_PB_VECT);
	DskySwitchFour.DefineMeshGroup(VC_GRP_PB_P4_09);

	MainPanelVC.AddSwitch(&DskySwitchFive, AID_VC_PUSHB_P4_10);
	DskySwitchFive.SetDirection(P4_PB_VECT);
	DskySwitchFive.DefineMeshGroup(VC_GRP_PB_P4_10);

	MainPanelVC.AddSwitch(&DskySwitchSix, AID_VC_PUSHB_P4_11);
	DskySwitchSix.SetDirection(P4_PB_VECT);
	DskySwitchSix.DefineMeshGroup(VC_GRP_PB_P4_11);

	MainPanelVC.AddSwitch(&DskySwitchSeven, AID_VC_PUSHB_P4_12);
	DskySwitchSeven.SetDirection(P4_PB_VECT);
	DskySwitchSeven.DefineMeshGroup(VC_GRP_PB_P4_12);

	MainPanelVC.AddSwitch(&DskySwitchEight, AID_VC_PUSHB_P4_13);
	DskySwitchEight.SetDirection(P4_PB_VECT);
	DskySwitchEight.DefineMeshGroup(VC_GRP_PB_P4_13);

	MainPanelVC.AddSwitch(&DskySwitchNine, AID_VC_PUSHB_P4_14);
	DskySwitchNine.SetDirection(P4_PB_VECT);
	DskySwitchNine.DefineMeshGroup(VC_GRP_PB_P4_14);

	MainPanelVC.AddSwitch(&DskySwitchClear, AID_VC_PUSHB_P4_15);
	DskySwitchClear.SetDirection(P4_PB_VECT);
	DskySwitchClear.DefineMeshGroup(VC_GRP_PB_P4_15);

	MainPanelVC.AddSwitch(&DskySwitchProceed, AID_VC_PUSHB_P4_16);
	DskySwitchProceed.SetDirection(P4_PB_VECT);
	DskySwitchProceed.DefineMeshGroup(VC_GRP_PB_P4_16);

	MainPanelVC.AddSwitch(&DskySwitchKeyRel, AID_VC_PUSHB_P4_17);
	DskySwitchKeyRel.SetDirection(P4_PB_VECT);
	DskySwitchKeyRel.DefineMeshGroup(VC_GRP_PB_P4_17);

	MainPanelVC.AddSwitch(&DskySwitchEnter, AID_VC_PUSHB_P4_18);
	DskySwitchEnter.SetDirection(P4_PB_VECT);
	DskySwitchEnter.DefineMeshGroup(VC_GRP_PB_P4_18);

	MainPanelVC.AddSwitch(&DskySwitchReset, AID_VC_PUSHB_P4_19);
	DskySwitchReset.SetDirection(P4_PB_VECT);
	DskySwitchReset.DefineMeshGroup(VC_GRP_PB_P4_19);

	MainPanelVC.AddSwitch(&LeftACA4JetSwitch, AID_VC_SWITCH_P4_01);
	LeftACA4JetSwitch.SetReference(P4_TOGGLE_POS[0], _V(1, 0, 0));
	LeftACA4JetSwitch.DefineMeshGroup(VC_GRP_Sw_P4_01);

	MainPanelVC.AddSwitch(&LeftTTCATranslSwitch, AID_VC_SWITCH_P4_02);
	LeftTTCATranslSwitch.SetReference(P4_TOGGLE_POS[1], _V(1, 0, 0));
	LeftTTCATranslSwitch.DefineMeshGroup(VC_GRP_Sw_P4_02);

	MainPanelVC.AddSwitch(&RightACA4JetSwitch, AID_VC_SWITCH_P4_03);
	RightACA4JetSwitch.SetReference(P4_TOGGLE_POS[2], _V(1, 0, 0));
	RightACA4JetSwitch.DefineMeshGroup(VC_GRP_Sw_P4_03);

	MainPanelVC.AddSwitch(&RightTTCATranslSwitch, AID_VC_SWITCH_P4_04);
	RightTTCATranslSwitch.SetReference(P4_TOGGLE_POS[3], _V(1, 0, 0));
	RightTTCATranslSwitch.DefineMeshGroup(VC_GRP_Sw_P4_04);

	//Panel 5

	MainPanelVC.AddSwitch(&TimerContSwitch, AID_VC_SWITCH_P5_01);
	TimerContSwitch.SetReference(P5_TOGGLE_POS[0] - P5_CLICK, _V(1, 0, 0));
	TimerContSwitch.DefineMeshGroup(VC_GRP_Sw_P5_01);

	MainPanelVC.AddSwitch(&TimerSlewHours, AID_VC_SWITCH_P5_02);
	TimerSlewHours.SetReference(P5_TOGGLE_POS[1] - P5_CLICK, _V(1, 0, 0));
	TimerSlewHours.DefineMeshGroup(VC_GRP_Sw_P5_02);

	MainPanelVC.AddSwitch(&TimerSlewMinutes, AID_VC_SWITCH_P5_03);
	TimerSlewMinutes.SetReference(P5_TOGGLE_POS[2] - P5_CLICK, _V(1, 0, 0));
	TimerSlewMinutes.DefineMeshGroup(VC_GRP_Sw_P5_03);

	MainPanelVC.AddSwitch(&TimerSlewSeconds, AID_VC_SWITCH_P5_04);
	TimerSlewSeconds.SetReference(P5_TOGGLE_POS[3] - P5_CLICK, _V(1, 0, 0));
	TimerSlewSeconds.DefineMeshGroup(VC_GRP_Sw_P5_04);

	MainPanelVC.AddSwitch(&LtgORideAnunSwitch, AID_VC_SWITCH_P5_05);
	LtgORideAnunSwitch.SetReference(P5_TOGGLE_POS[4] - P5_CLICK, _V(1, 0, 0));
	LtgORideAnunSwitch.DefineMeshGroup(VC_GRP_Sw_P5_05);

	MainPanelVC.AddSwitch(&LtgORideNumSwitch, AID_VC_SWITCH_P5_06);
	LtgORideNumSwitch.SetReference(P5_TOGGLE_POS[5] - P5_CLICK, _V(1, 0, 0));
	LtgORideNumSwitch.DefineMeshGroup(VC_GRP_Sw_P5_06);

	MainPanelVC.AddSwitch(&LtgORideIntegralSwitch, AID_VC_SWITCH_P5_07);
	LtgORideIntegralSwitch.SetReference(P5_TOGGLE_POS[6] - P5_CLICK, _V(1, 0, 0));
	LtgORideIntegralSwitch.DefineMeshGroup(VC_GRP_Sw_P5_07);

	MainPanelVC.AddSwitch(&LtgSidePanelsSwitch, AID_VC_SWITCH_P5_08);
	LtgSidePanelsSwitch.SetReference(P5_TOGGLE_POS[7] - P5_CLICK, _V(1, 0, 0));
	LtgSidePanelsSwitch.DefineMeshGroup(VC_GRP_Sw_P5_08);

	MainPanelVC.AddSwitch(&LtgFloodOhdFwdKnob, AID_VC_ROT_P5_01);
	LtgFloodOhdFwdKnob.SetReference(P5_ROT_POS[0], P5_ROT_AXIS);
	LtgFloodOhdFwdKnob.DefineMeshGroup(VC_GRP_Rot_P5_01);
	LtgFloodOhdFwdKnob.SetInitialAnimState(0.5);

	MainPanelVC.AddSwitch(&LtgAnunNumKnob, AID_VC_ROT_P5_02);
	LtgAnunNumKnob.SetReference(P5_ROT_POS[1], P5_ROT_AXIS);
	LtgAnunNumKnob.DefineMeshGroup(VC_GRP_Rot_P5_02);
	LtgAnunNumKnob.SetInitialAnimState(0.5);

	MainPanelVC.AddSwitch(&LtgIntegralKnob, AID_VC_ROT_P5_03);
	LtgIntegralKnob.SetReference(P5_ROT_POS[2], P5_ROT_AXIS);
	LtgIntegralKnob.DefineMeshGroup(VC_GRP_Rot_P5_03);
	LtgIntegralKnob.SetInitialAnimState(0.5);

	MainPanelVC.AddSwitch(&ManualEngineStart, AID_VC_START_BUTTON);
	ManualEngineStart.SetDirection(_V(0.00, -0.004*cos(P5_TILT - (90.0 * RAD)), -0.004*sin(P5_TILT - (90.0 * RAD))));
	ManualEngineStart.DefineMeshGroup(VC_GRP_StartButton);

	MainPanelVC.AddSwitch(&CDRManualEngineStop, AID_VC_STOP_BUTTON_CDR);
	CDRManualEngineStop.SetReference(_V(-0.703027, 0.102277, 1.53733), _V(-0.00, sin(P5_TILT + (90.0 * RAD)), -cos(P5_TILT + (90.0 * RAD))));
	CDRManualEngineStop.DefineMeshGroup(VC_GRP_StopButtonCDR);
	CDRManualEngineStop.SetRotationRange(5.0*RAD);

	MainPanelVC.AddSwitch(&PlusXTranslationButton, AID_VC_PLUSX_BUTTON);
	PlusXTranslationButton.SetDirection(_V(0.00, -0.004*cos(P5_TILT - (90.0 * RAD)), -0.004*sin(P5_TILT - (90.0 * RAD))));
	PlusXTranslationButton.DefineMeshGroup(VC_GRP_PlusXButton);

	//Panel 6

	const VECTOR3	P6_PB_VECT = { 0.00,  0.001*-sin(P6_TILT), 0.001*cos(P6_TILT) };

	MainPanelVC.AddSwitch(&DedaSwitchPlus, AID_VC_PUSHB_P6_01);
	DedaSwitchPlus.SetDirection(P6_PB_VECT);
	DedaSwitchPlus.DefineMeshGroup(VC_GRP_PB_P5_6_01);

	MainPanelVC.AddSwitch(&DedaSwitchMinus, AID_VC_PUSHB_P6_02);
	DedaSwitchMinus.SetDirection(P6_PB_VECT);
	DedaSwitchMinus.DefineMeshGroup(VC_GRP_PB_P5_6_02);

	MainPanelVC.AddSwitch(&DedaSwitchZero, AID_VC_PUSHB_P6_03);
	DedaSwitchZero.SetDirection(P6_PB_VECT);
	DedaSwitchZero.DefineMeshGroup(VC_GRP_PB_P5_6_03);

	MainPanelVC.AddSwitch(&DedaSwitchOne, AID_VC_PUSHB_P6_04);
	DedaSwitchOne.SetDirection(P6_PB_VECT);
	DedaSwitchOne.DefineMeshGroup(VC_GRP_PB_P5_6_04);

	MainPanelVC.AddSwitch(&DedaSwitchTwo, AID_VC_PUSHB_P6_05);
	DedaSwitchTwo.SetDirection(P6_PB_VECT);
	DedaSwitchTwo.DefineMeshGroup(VC_GRP_PB_P5_6_05);

	MainPanelVC.AddSwitch(&DedaSwitchThree, AID_VC_PUSHB_P6_06);
	DedaSwitchThree.SetDirection(P6_PB_VECT);
	DedaSwitchThree.DefineMeshGroup(VC_GRP_PB_P5_6_06);

	MainPanelVC.AddSwitch(&DedaSwitchFour, AID_VC_PUSHB_P6_07);
	DedaSwitchFour.SetDirection(P6_PB_VECT);
	DedaSwitchFour.DefineMeshGroup(VC_GRP_PB_P5_6_07);

	MainPanelVC.AddSwitch(&DedaSwitchFive, AID_VC_PUSHB_P6_08);
	DedaSwitchFive.SetDirection(P6_PB_VECT);
	DedaSwitchFive.DefineMeshGroup(VC_GRP_PB_P5_6_08);

	MainPanelVC.AddSwitch(&DedaSwitchSix, AID_VC_PUSHB_P6_09);
	DedaSwitchSix.SetDirection(P6_PB_VECT);
	DedaSwitchSix.DefineMeshGroup(VC_GRP_PB_P5_6_09);

	MainPanelVC.AddSwitch(&DedaSwitchSeven, AID_VC_PUSHB_P6_10);
	DedaSwitchSeven.SetDirection(P6_PB_VECT);
	DedaSwitchSeven.DefineMeshGroup(VC_GRP_PB_P5_6_10);

	MainPanelVC.AddSwitch(&DedaSwitchEight, AID_VC_PUSHB_P6_11);
	DedaSwitchEight.SetDirection(P6_PB_VECT);
	DedaSwitchEight.DefineMeshGroup(VC_GRP_PB_P5_6_11);

	MainPanelVC.AddSwitch(&DedaSwitchNine, AID_VC_PUSHB_P6_12);
	DedaSwitchNine.SetDirection(P6_PB_VECT);
	DedaSwitchNine.DefineMeshGroup(VC_GRP_PB_P5_6_12);

	MainPanelVC.AddSwitch(&DedaSwitchClear, AID_VC_PUSHB_P6_13);
	DedaSwitchClear.SetDirection(P6_PB_VECT);
	DedaSwitchClear.DefineMeshGroup(VC_GRP_PB_P5_6_13);

	MainPanelVC.AddSwitch(&DedaSwitchReadOut, AID_VC_PUSHB_P6_14);
	DedaSwitchReadOut.SetDirection(P6_PB_VECT);
	DedaSwitchReadOut.DefineMeshGroup(VC_GRP_PB_P5_6_14);

	MainPanelVC.AddSwitch(&DedaSwitchEnter, AID_VC_PUSHB_P6_15);
	DedaSwitchEnter.SetDirection(P6_PB_VECT);
	DedaSwitchEnter.DefineMeshGroup(VC_GRP_PB_P5_6_15);

	MainPanelVC.AddSwitch(&DedaSwitchHold, AID_VC_PUSHB_P6_16);
	DedaSwitchHold.SetDirection(P6_PB_VECT);
	DedaSwitchHold.DefineMeshGroup(VC_GRP_PB_P5_6_16);

	MainPanelVC.AddSwitch(&AGSOperateSwitch, AID_VC_SWITCH_P6_01);
	AGSOperateSwitch.SetReference(P6_TOGGLE_POS[0], _V(1, 0, 0));
	AGSOperateSwitch.DefineMeshGroup(VC_GRP_Sw_P6_01);

	MainPanelVC.AddSwitch(&LMPManualEngineStop, AID_VC_STOP_BUTTON_LMP);
	LMPManualEngineStop.SetReference(_V(0.413931, 0.098204, 1.50665), _V(-0.00, sin(P6_TILT + (90.0 * RAD)), -cos(P6_TILT + (90.0 * RAD))));
	LMPManualEngineStop.DefineMeshGroup(VC_GRP_StopButtonLMP);
	LMPManualEngineStop.SetRotationRange(5.0*RAD);

	//Panel 8

	MainPanelVC.AddSwitch(&EDDesFuelVent, AID_VC_SWITCH_P8_01);
	EDDesFuelVent.SetReference(P8_TOGGLE_POS[0] - P8_CLICK, _V(0, 0, 1));
	EDDesFuelVent.DefineMeshGroup(VC_GRP_Sw_P8_01);

	MainPanelVC.AddSwitch(&EDDesOxidVent, AID_VC_SWITCH_P8_02);
	EDDesOxidVent.SetReference(P8_TOGGLE_POS[1] - P8_CLICK, _V(0, 0, 1));
	EDDesOxidVent.DefineMeshGroup(VC_GRP_Sw_P8_02);

	MainPanelVC.AddSwitch(&EDDesPrpIsol, AID_VC_SWITCH_P8_03);
	EDDesPrpIsol.SetReference(P8_TOGGLE_POS[2] - P8_CLICK, _V(0, 0, 1));
	EDDesPrpIsol.DefineMeshGroup(VC_GRP_Sw_P8_03);

	MainPanelVC.AddSwitch(&EDMasterArm, AID_VC_SWITCH_P8_04);
	EDMasterArm.SetReference(P8_TOGGLE_POS[3] - P8_CLICK, _V(0, 0, 1));
	EDMasterArm.DefineMeshGroup(VC_GRP_Sw_P8_04);

	MainPanelVC.AddSwitch(&EDDesVent, AID_VC_SWITCH_P8_05);
	EDDesVent.SetReference(P8_TOGGLE_POS[4] - P8_CLICK, _V(0, 0, 1));
	EDDesVent.DefineMeshGroup(VC_GRP_Sw_P8_05);

	MainPanelVC.AddSwitch(&EDASCHeSel, AID_VC_SWITCH_P8_06);
	EDASCHeSel.SetReference(P8_TOGGLE_POS[5] - P8_CLICK, _V(0, 0, 1));
	EDASCHeSel.DefineMeshGroup(VC_GRP_Sw_P8_06);

	MainPanelVC.AddSwitch(&EDLGDeploy, AID_VC_SWITCH_P8_07);
	EDLGDeploy.SetReference(P8_TOGGLE_POS[6] - P8_CLICK, _V(0, 0, 1));
	EDLGDeploy.DefineMeshGroup(VC_GRP_Sw_P8_07);

	MainPanelVC.AddSwitch(&EDHePressRCS, AID_VC_SWITCH_P8_08);
	EDHePressRCS.SetReference(P8_TOGGLE_POS[7] - P8_CLICK, _V(0, 0, 1));
	EDHePressRCS.DefineMeshGroup(VC_GRP_Sw_P8_08);

	MainPanelVC.AddSwitch(&EDHePressDesStart, AID_VC_SWITCH_P8_09);
	EDHePressDesStart.SetReference(P8_TOGGLE_POS[8] - P8_CLICK, _V(0, 0, 1));
	EDHePressDesStart.DefineMeshGroup(VC_GRP_Sw_P8_09);

	MainPanelVC.AddSwitch(&EDHePressASC, AID_VC_SWITCH_P8_10);
	EDHePressASC.SetReference(P8_TOGGLE_POS[9] - P8_CLICK, _V(0, 0, 1));
	EDHePressASC.DefineMeshGroup(VC_GRP_Sw_P8_10);

	MainPanelVC.AddSwitch(&EDStageRelay, AID_VC_SWITCH_P8_11);
	EDStageRelay.SetReference(P8_TOGGLE_POS[10] - P8_CLICK, _V(0, 0, 1));
	EDStageRelay.DefineMeshGroup(VC_GRP_Sw_P8_11);

	MainPanelVC.AddSwitch(&CDRAudSBandSwitch, AID_VC_SWITCH_P8_12);
	CDRAudSBandSwitch.SetReference(P8_TOGGLE_POS[11] - P8_CLICK, _V(0, 0, 1));
	CDRAudSBandSwitch.DefineMeshGroup(VC_GRP_Sw_P8_12);

	MainPanelVC.AddSwitch(&CDRAudICSSwitch, AID_VC_SWITCH_P8_13);
	CDRAudICSSwitch.SetReference(P8_TOGGLE_POS[12] - P8_CLICK, _V(0, 0, 1));
	CDRAudICSSwitch.DefineMeshGroup(VC_GRP_Sw_P8_13);

	MainPanelVC.AddSwitch(&CDRAudVHFASwitch, AID_VC_SWITCH_P8_14);
	CDRAudVHFASwitch.SetReference(P8_TOGGLE_POS[13] - P8_CLICK, _V(0, 0, 1));
	CDRAudVHFASwitch.DefineMeshGroup(VC_GRP_Sw_P8_14);

	MainPanelVC.AddSwitch(&CDRAudVHFBSwitch, AID_VC_SWITCH_P8_15);
	CDRAudVHFBSwitch.SetReference(P8_TOGGLE_POS[14] - P8_CLICK, _V(0, 0, 1));
	CDRAudVHFBSwitch.DefineMeshGroup(VC_GRP_Sw_P8_15);

	MainPanelVC.AddSwitch(&CDRAudRelaySwitch, AID_VC_SWITCH_P8_16);
	CDRAudRelaySwitch.SetReference(P8_TOGGLE_POS[15] - P8_CLICK, _V(0, 0, 1));
	CDRAudRelaySwitch.DefineMeshGroup(VC_GRP_Sw_P8_16);

	MainPanelVC.AddSwitch(&CDRAudVOXSwitch, AID_VC_SWITCH_P8_17);
	CDRAudVOXSwitch.SetReference(P8_TOGGLE_POS[16] - P8_CLICK, _V(0, 0, 1));
	CDRAudVOXSwitch.DefineMeshGroup(VC_GRP_Sw_P8_17);

	MainPanelVC.AddSwitch(&CDRAudioControlSwitch, AID_VC_SWITCH_P8_18);
	CDRAudioControlSwitch.SetReference(P8_TOGGLE_POS[17] - P8_CLICK, _V(0, 0, 1));
	CDRAudioControlSwitch.DefineMeshGroup(VC_GRP_Sw_P8_18);

	MainPanelVC.AddSwitch(&CDRCOASSwitch, AID_VC_SWITCH_P8_19);
	CDRCOASSwitch.SetReference(P8_TOGGLE_POS[18] - P8_CLICK, _V(0, 0, 1));
	CDRCOASSwitch.DefineMeshGroup(VC_GRP_Sw_P8_19);

	MainPanelVC.AddSwitch(&EDStage, AID_VC_STAGE_SWITCH);
	EDStage.SetReference(_V(-0.981771, 0.039868, 1.14749), _V(-0.990451, 0.042666, 1.12664), _V(0, 0, 1), _V(sin((180 * RAD) + P8_TILT), -cos((180 * RAD) + P8_TILT), 0.00));
	EDStage.DefineMeshGroup(VC_GRP_StageSwitch, VC_GRP_StageCover);

	MainPanelVC.AddSwitch(&CDRAudSBandVol, AID_VC_TW_P8_01);
	CDRAudSBandVol.SetReference(P8_TW_POS[0], _V(0, 0, 1));
	CDRAudSBandVol.DefineMeshGroup(VC_GRP_TW_P8_01);

	MainPanelVC.AddSwitch(&CDRAudICSVol, AID_VC_TW_P8_02);
	CDRAudICSVol.SetReference(P8_TW_POS[1], _V(0, 0, 1));
	CDRAudICSVol.DefineMeshGroup(VC_GRP_TW_P8_02);

	MainPanelVC.AddSwitch(&CDRAudVOXSens, AID_VC_TW_P8_03);
	CDRAudVOXSens.SetReference(P8_TW_POS[2], _V(0, 0, 1));
	CDRAudVOXSens.DefineMeshGroup(VC_GRP_TW_P8_03);

	MainPanelVC.AddSwitch(&CDRAudVHFAVol, AID_VC_TW_P8_04);
	CDRAudVHFAVol.SetReference(P8_TW_POS[3], _V(0, 0, 1));
	CDRAudVHFAVol.DefineMeshGroup(VC_GRP_TW_P8_04);

	MainPanelVC.AddSwitch(&CDRAudVHFBVol, AID_VC_TW_P8_05);
	CDRAudVHFBVol.SetReference(P8_TW_POS[4], _V(0, 0, 1));
	CDRAudVHFBVol.DefineMeshGroup(VC_GRP_TW_P8_05);

	MainPanelVC.AddSwitch(&CDRAudMasterVol, AID_VC_TW_P8_06);
	CDRAudMasterVol.SetReference(P8_TW_POS[5], _V(0, 0, 1));
	CDRAudMasterVol.DefineMeshGroup(VC_GRP_TW_P8_06);

	MainPanelVC.AddSwitch(&EDDesFuelVentTB, AID_VC_PANEL8_TALKBACKS);
	MainPanelVC.AddSwitch(&EDDesOxidVentTB, AID_VC_PANEL8_TALKBACKS);
	MainPanelVC.AddSwitch(&EDLGTB, AID_VC_PANEL8_TALKBACKS);

	//Panel 11

	const VECTOR3 p11row1_vector = { 0.003 * sin(P11R1_TILT - (90.0 * RAD)), 0.003 * -cos(P11R1_TILT - (90.0 * RAD)), 0.0 };
	const VECTOR3 p11row2_vector = { 0.003 * sin(P11R2_TILT - (90.0 * RAD)), 0.003 * -cos(P11R2_TILT - (90.0 * RAD)), 0.0 };
	const VECTOR3 p11row3_vector = { 0.003 * sin(P11R3_TILT - (90.0 * RAD)), 0.003 * -cos(P11R3_TILT - (90.0 * RAD)), 0.0 };
	const VECTOR3 p11row4_vector = { 0.003 * sin(P11R4_TILT - (90.0 * RAD)), 0.003 * -cos(P11R4_TILT - (90.0 * RAD)), 0.0 };
	const VECTOR3 p11row5_vector = { 0.003 * sin(P11R5_TILT - (90.0 * RAD)), 0.003 * -cos(P11R5_TILT - (90.0 * RAD)), 0.0 };

	CircuitBrakerSwitch* breakerrow1[P11R1_CBCOUNT] = { &SE_WND_HTR_AC_CB, &HE_PQGS_PROP_DISP_AC_CB, &SBD_ANT_AC_CB, &ORDEAL_AC_CB, &AGS_AC_CB, &AOT_LAMP_ACB_CB, &LMP_FDAI_AC_CB, &NUM_LTG_AC_CB, &AC_B_INV_2_FEED_CB,
		&AC_B_INV_1_FEED_CB, &AC_A_INV_2_FEED_CB, &AC_A_INV_1_FEED_CB, &AC_A_BUS_VOLT_CB, &CDR_WND_HTR_AC_CB, &TAPE_RCDR_AC_CB, &AOT_LAMP_ACA_CB, &RDZ_RDR_AC_CB, &DECA_GMBL_AC_CB, &INTGL_LTG_AC_CB };

	for (int i = 0;i < P11R1_CBCOUNT;i++)
	{
		MainPanelVC.AddSwitch(breakerrow1[i], AID_VC_CB_P11R1_01 + i);
		breakerrow1[i]->SetDirection(p11row1_vector);
		breakerrow1[i]->DefineMeshGroup(VC_GRP_CB_P11R1_01 + i);
	}

	CircuitBrakerSwitch* breakerrow2[P11R2_CBCOUNT] = { &RCS_A_MAIN_SOV_CB, &RCS_A_QUAD4_TCA_CB, &RCS_A_QUAD3_TCA_CB, &RCS_A_QUAD2_TCA_CB, &RCS_A_QUAD1_TCA_CB, &RCS_A_ISOL_VLV_CB, &RCS_A_ASC_FEED_2_CB,
		&RCS_A_ASC_FEED_1_CB, &THRUST_DISP_CB, &MISSION_TIMER_CB, &CDR_XPTR_CB, &RNG_RT_ALT_RT_DC_CB, &GASTA_DC_CB, &CDR_FDAI_DC_CB, &COAS_DC_CB, &ORDEAL_DC_CB, &RNG_RT_ALT_RT_AC_CB, &GASTA_AC_CB,
		&CDR_FDAI_AC_CB };

	for (int i = 0;i < P11R2_CBCOUNT;i++)
	{
		MainPanelVC.AddSwitch(breakerrow2[i], AID_VC_CB_P11R2_01 + i);
		breakerrow2[i]->SetDirection(p11row2_vector);
		breakerrow2[i]->DefineMeshGroup(VC_GRP_CB_P11R2_01 + i);
	}

	CircuitBrakerSwitch* breakerrow3[P11R3_CBCOUNT] = { &PROP_DES_HE_REG_VENT_CB, &HTR_RR_STBY_CB, &HTR_RR_OPR_CB, &HTR_LR_CB, &HTR_DOCK_WINDOW_CB, &HTR_AOT_CB, &INST_SIG_CONDR_1_CB, &CDR_SCS_AEA_CB,
		&CDR_SCS_ABORT_STAGE_CB, &CDR_SCS_ATCA_CB, &CDR_SCS_AELD_CB, &SCS_ENG_CONT_CB, &SCS_ATT_DIR_CONT_CB, &SCS_ENG_START_OVRD_CB, &SCS_DECA_PWR_CB, &EDS_CB_LG_FLAG, &EDS_CB_LOGIC_A, &CDR_LTG_UTIL_CB,
			   &CDR_LTG_ANUN_DOCK_COMPNT_CB };

	for (int i = 0;i < P11R3_CBCOUNT;i++)
	{
		MainPanelVC.AddSwitch(breakerrow3[i], AID_VC_CB_P11R3_01 + i);
		breakerrow3[i]->SetDirection(p11row3_vector);
		breakerrow3[i]->DefineMeshGroup(VC_GRP_CB_P11R3_01 + i);
	}

	CircuitBrakerSwitch* breakerrow4[P11R4_CBCOUNT] = { &RCS_QUAD_4_CDR_HTR_CB, &RCS_QUAD_3_CDR_HTR_CB, &RCS_QUAD_2_CDR_HTR_CB,	&RCS_QUAD_1_CDR_HTR_CB,	&ECS_SUIT_FAN_1_CB,	&ECS_CABIN_FAN_1_CB,
		&ECS_GLYCOL_PUMP_2_CB, &ECS_GLYCOL_PUMP_1_CB, &ECS_GLYCOL_PUMP_AUTO_XFER_CB, &COMM_UP_DATA_LINK_CB, &COMM_SEC_SBAND_XCVR_CB, &COMM_SEC_SBAND_PA_CB, &COMM_VHF_XMTR_B_CB, &COMM_VHF_RCVR_A_CB,
		&COMM_CDR_AUDIO_CB, &PGNS_SIG_STR_DISP_CB, &PGNS_LDG_RDR_CB, &PGNS_RNDZ_RDR_CB, &LGC_DSKY_CB, &IMU_SBY_CB, &IMU_OPR_CB };

	for (int i = 0;i < P11R4_CBCOUNT;i++)
	{
		MainPanelVC.AddSwitch(breakerrow4[i], AID_VC_CB_P11R4_01 + i);
		breakerrow4[i]->SetDirection(p11row4_vector);
		breakerrow4[i]->DefineMeshGroup(VC_GRP_CB_P11R4_01 + i);
	}

	CircuitBrakerSwitch* breakerrow5[P11R5_CBCOUNT] = { &CDRBatteryFeedTieCB1, &CDRBatteryFeedTieCB2, &CDRCrossTieBalCB, &CDRCrossTieBusCB,&CDRXLunarBusTieCB,&CDRDesECAContCB,&CDRDesECAMainCB,
	&CDRAscECAContCB, &CDRAscECAMainCB, &CDRInverter1CB, &CDRDCBusVoltCB };

	for (int i = 0;i < P11R5_CBCOUNT;i++)
	{
		MainPanelVC.AddSwitch(breakerrow5[i], AID_VC_CB_P11R5_01 + i);
		breakerrow5[i]->SetDirection(p11row5_vector);
		breakerrow5[i]->DefineMeshGroup(VC_GRP_CB_P11R5_01 + i);
	}

	//Panel 12

	TwoPositionSwitch *panel12sw[P12_SWITCHCOUNT] = { &Panel12AudioCtlSwitch, &Panel12UpdataLinkSwitch, &LMPAudSBandSwitch, &LMPAudICSSwitch, &LMPAudRelaySwitch, &LMPAudVHFASwitch, &LMPAudVHFBSwitch,
		&LMPAudVOXSwitch, &SBandModulateSwitch, &SBandXCvrSelSwitch, &SBandPASelSwitch, &SBandVoiceSwitch, &SBandPCMSwitch, &SBandRangeSwitch, &VHFAVoiceSwitch, &VHFARcvrSwtich, &VHFBVoiceSwitch,
		&VHFBRcvrSwtich, &TLMBiomedSwtich, &TLMBitrateSwitch, &TapeRecorderSwitch, &Panel12AntTrackModeSwitch };

	for (int i = 0;i < P12_SWITCHCOUNT;i++)
	{
		MainPanelVC.AddSwitch(panel12sw[i], AID_VC_SWITCH_P12_01 + i);
		panel12sw[i]->SetReference(P12_TOGGLE_POS[i] - P12_CLICK, _V(0, 0, -1));
		panel12sw[i]->DefineMeshGroup(VC_GRP_Sw_P12_01 + i);
	}

	MainPanelVC.AddSwitch(&Panel12AntPitchKnob, AID_VC_ROT_P12_01);
	Panel12AntPitchKnob.SetReference(P12_ROT_POS[0], P12_ROT_AXIS);
	Panel12AntPitchKnob.DefineMeshGroup(VC_GRP_Rot_P12_01);
	Panel12AntPitchKnob.SetInitialAnimState(0.5);

	MainPanelVC.AddSwitch(&Panel12AntYawKnob, AID_VC_ROT_P12_02);
	Panel12AntYawKnob.SetReference(P12_ROT_POS[1], P12_ROT_AXIS);
	Panel12AntYawKnob.DefineMeshGroup(VC_GRP_Rot_P12_02);
	Panel12AntYawKnob.SetInitialAnimState(0.5);

	MainPanelVC.AddSwitch(&Panel12SBandAntSelKnob, AID_VC_ROT_P12_03);
	Panel12SBandAntSelKnob.SetReference(P12_ROT_POS[2], P12_ROT_AXIS);
	Panel12SBandAntSelKnob.DefineMeshGroup(VC_GRP_Rot_P12_03);

	MainPanelVC.AddSwitch(&Panel12VHFAntSelKnob, AID_VC_ROT_P12_04);
	Panel12VHFAntSelKnob.SetReference(P12_ROT_POS[3], P12_ROT_AXIS);
	Panel12VHFAntSelKnob.DefineMeshGroup(VC_GRP_Rot_P12_04);

	ThumbwheelSwitch *p12thumbw[P12_TWCOUNT] = { &LMPAudSBandVol, &LMPAudICSVol, &LMPAudVOXSens, &LMPAudVHFAVol, &LMPAudVHFBVol, &LMPAudMasterVol, &VHFASquelch, &VHFBSquelch };

	for (int i = 0;i < P12_TWCOUNT;i++)
	{
		MainPanelVC.AddSwitch(p12thumbw[i], AID_VC_TW_P12_01 + i);
		p12thumbw[i]->SetReference(P12_TW_POS[i], _V(0, 0, -1));
		p12thumbw[i]->DefineMeshGroup(VC_GRP_TW_P12_01 + i);
	}

	MainPanelVC.AddSwitch(&ComPitchMeter);
	ComPitchMeter.SetReference(P12_NEEDLE_POS[0], P12_ROT_AXIS);
	ComPitchMeter.DefineMeshGroup(VC_GRP_Needle_P12_01);

	MainPanelVC.AddSwitch(&ComYawMeter);
	ComYawMeter.SetReference(P12_NEEDLE_POS[1], P12_ROT_AXIS);
	ComYawMeter.DefineMeshGroup(VC_GRP_Needle_P12_02);

	MainPanelVC.AddSwitch(&Panel12SignalStrengthMeter);
	Panel12SignalStrengthMeter.SetReference(P12_NEEDLE_POS[2], P12_ROT_AXIS);
	Panel12SignalStrengthMeter.DefineMeshGroup(VC_GRP_Needle_P12_03);

	MainPanelVC.AddSwitch(&TapeRecorderTB, AID_VC_RECORDER_TALKBACK);

	//Panel 14

	TwoPositionSwitch *p14sw[P14_SWITCHCOUNT] = { &EPSEDVoltSelect, &EPSInverterSwitch, &DSCSEBat1HVSwitch, &DSCSEBat2HVSwitch, &DSCCDRBat3HVSwitch, &DSCCDRBat4HVSwitch, &DSCSEBat1LVSwitch,
		&DSCSEBat2LVSwitch, &DSCCDRBat3LVSwitch, &DSCCDRBat4LVSwitch,  &DSCBattFeedSwitch, &ASCBat5SESwitch, &ASCBat5CDRSwitch, &ASCBat6CDRSwitch, &ASCBat6SESwitch, &UpDataSquelchSwitch };

	for (int i = 0;i < P14_SWITCHCOUNT;i++)
	{
		MainPanelVC.AddSwitch(p14sw[i], AID_VC_SWITCH_P14_01 + i);
		p14sw[i]->SetReference(P14_TOGGLE_POS[i] - P14_CLICK, _V(0, 0, -1));
		p14sw[i]->DefineMeshGroup(VC_GRP_Sw_P14_01 + i);
	}

	MainPanelVC.AddSwitch(&EPSMonitorSelectRotary, AID_VC_ROT_P14_01);
	EPSMonitorSelectRotary.SetReference(P14_ROT_POS[0], P14_ROT_AXIS);
	EPSMonitorSelectRotary.DefineMeshGroup(VC_GRP_Rot_P14_01);


	MainPanelVC.AddSwitch(&EPSDCVoltMeter);
	EPSDCVoltMeter.SetReference(P14_NEEDLE_POS[0], P14_ROT_AXIS);
	EPSDCVoltMeter.DefineMeshGroup(VC_GRP_Needle_P14_01);

	MainPanelVC.AddSwitch(&EPSDCAmMeter);
	EPSDCAmMeter.SetReference(P14_NEEDLE_POS[1], P14_ROT_AXIS);
	EPSDCAmMeter.DefineMeshGroup(VC_GRP_Needle_P14_02);

	MainPanelVC.AddSwitch(&DSCBattery1TB, AID_VC_DSC_BATTERY_TALKBACKS);
	MainPanelVC.AddSwitch(&DSCBattery2TB, AID_VC_DSC_BATTERY_TALKBACKS);
	MainPanelVC.AddSwitch(&DSCBattery3TB, AID_VC_DSC_BATTERY_TALKBACKS);
	MainPanelVC.AddSwitch(&DSCBattery4TB, AID_VC_DSC_BATTERY_TALKBACKS);
	MainPanelVC.AddSwitch(&DSCBattFeedTB, AID_VC_DSC_BATTERY_TALKBACKS);
	MainPanelVC.AddSwitch(&ASCBattery5ATB, AID_VC_ASC_BATTERY_TALKBACKS);
	MainPanelVC.AddSwitch(&ASCBattery5BTB, AID_VC_ASC_BATTERY_TALKBACKS);
	MainPanelVC.AddSwitch(&ASCBattery6ATB, AID_VC_ASC_BATTERY_TALKBACKS);
	MainPanelVC.AddSwitch(&ASCBattery6BTB, AID_VC_ASC_BATTERY_TALKBACKS);

	//Panel 16

	const VECTOR3 p16row1_vector = { 0.003 * -sin(P16R1_TILT - (90.0 * RAD)), 0.003 * cos(P16R1_TILT - (90.0 * RAD)), 0.0 };
	const VECTOR3 p16row2_vector = { 0.003 * -sin(P16R2_TILT - (90.0 * RAD)), 0.003 * cos(P16R2_TILT - (90.0 * RAD)), 0.0 };
	const VECTOR3 p16row3_vector = { 0.003 * -sin(P16R3_TILT - (90.0 * RAD)), 0.003 * cos(P16R3_TILT - (90.0 * RAD)), 0.0 };
	const VECTOR3 p16row4_vector = { 0.003 * -sin(P16R4_TILT - (90.0 * RAD)), 0.003 * cos(P16R4_TILT - (90.0 * RAD)), 0.0 };

	CircuitBrakerSwitch* breakerrowp161[P16R1_CBCOUNT] = { &LMP_EVT_TMR_FDAI_DC_CB, &SE_XPTR_DC_CB, &RCS_B_ASC_FEED_1_CB, &RCS_B_ASC_FEED_2_CB, &RCS_B_ISOL_VLV_CB, &RCS_B_QUAD1_TCA_CB, &RCS_B_QUAD2_TCA_CB, 
		&RCS_B_QUAD3_TCA_CB, &RCS_B_QUAD4_TCA_CB, &RCS_B_CRSFD_CB, &RCS_B_TEMP_PRESS_DISP_FLAGS_CB, &RCS_B_PQGS_DISP_CB, &RCS_B_MAIN_SOV_CB, &PROP_DISP_ENG_OVRD_LOGIC_CB, &PROP_PQGS_CB, &PROP_ASC_HE_REG_CB };

	for (int i = 0;i < P16R1_CBCOUNT;i++)
	{
		MainPanelVC.AddSwitch(breakerrowp161[i], AID_VC_CB_P16R1_01 + i);
		breakerrowp161[i]->SetDirection(p16row1_vector);
		breakerrowp161[i]->DefineMeshGroup(VC_GRP_CB_P16R1_01 + i);
	}
	
	CircuitBrakerSwitch* breakerrowp162[P16R2_CBCOUNT] = { &LTG_FLOOD_CB, &LTG_TRACK_CB, &LTG_ANUN_DOCK_COMPNT_CB, &LTG_MASTER_ALARM_CB, &EDS_CB_LOGIC_B, &SCS_AEA_CB, &SCS_ENG_ARM_CB, &SCS_ASA_CB, 
		&SCS_AELD_CB, &SCS_ATCA_CB, &SCS_ABORT_STAGE_CB, &SCS_ATCA_AGS_CB, &SCS_DES_ENG_OVRD_CB, &INST_CWEA_CB, &INST_SIG_SENSOR_CB, &INST_PCMTEA_CB, &INST_SIG_CONDR_2_CB, &ECS_SUIT_FLOW_CONT_CB };

	for (int i = 0;i < P16R2_CBCOUNT;i++)
	{
		MainPanelVC.AddSwitch(breakerrowp162[i], AID_VC_CB_P16R2_01 + i);
		breakerrowp162[i]->SetDirection(p16row2_vector);
		breakerrowp162[i]->DefineMeshGroup(VC_GRP_CB_P16R2_01 + i);
	}

	CircuitBrakerSwitch* breakerrowp163[P16R3_CBCOUNT] = { &COMM_DISP_CB, &COMM_SE_AUDIO_CB, &COMM_VHF_XMTR_A_CB, &COMM_VHF_RCVR_B_CB, &COMM_PRIM_SBAND_PA_CB, &COMM_PRIM_SBAND_XCVR_CB, &COMM_SBAND_ANT_CB, 
		&COMM_PMP_CB, &COMM_TV_CB, &ECS_DISP_CB, &ECS_GLYCOL_PUMP_SEC_CB, &ECS_LGC_PUMP_CB, &ECS_CABIN_FAN_CONT_CB, &ECS_CABIN_REPRESS_CB, &ECS_SUIT_FAN_2_CB, &ECS_SUIT_FAN_DP_CB, &ECS_DIVERT_VLV_CB, 
		&ECS_CO2_SENSOR_CB };

	for (int i = 0;i < P16R3_CBCOUNT;i++)
	{
		MainPanelVC.AddSwitch(breakerrowp163[i], AID_VC_CB_P16R3_01 + i);
		breakerrowp163[i]->SetDirection(p16row3_vector);
		breakerrowp163[i]->DefineMeshGroup(VC_GRP_CB_P16R3_01 + i);
	}

	CircuitBrakerSwitch* breakerrowp164[P16R4_CBCOUNT] = { &RCS_QUAD_1_LMP_HTR_CB, &RCS_QUAD_2_LMP_HTR_CB, &RCS_QUAD_3_LMP_HTR_CB, &RCS_QUAD_4_LMP_HTR_CB, &HTR_DISP_CB, &HTR_SBD_ANT_CB, &CAMR_SEQ_CB,
		&EPS_DISP_CB, &LMPDCBusVoltCB, &LMPInverter2CB, &LMPAscECAMainCB, &LMPAscECAContCB, &LMPDesECAMainCB, &LMPDesECAContCB, &LMPXLunarBusTieCB, &LMPCrossTieBusCB, &LMPCrossTieBalCB,
		&LMPBatteryFeedTieCB1, &LMPBatteryFeedTieCB2 };

	for (int i = 0;i < P16R4_CBCOUNT;i++)
	{
		MainPanelVC.AddSwitch(breakerrowp164[i], AID_VC_CB_P16R4_01 + i);
		breakerrowp164[i]->SetDirection(p16row4_vector);
		breakerrowp164[i]->DefineMeshGroup(VC_GRP_CB_P16R4_01 + i);
	}

	// ORDEAL

	const VECTOR3 ORDEAL_SW_AXIS = { 0, 0, 1 };

	MainPanelVC.AddSwitch(&ORDEALFDAI1Switch, AID_VC_SWITCH_ORDEAL_01);
	ORDEALFDAI1Switch.SetReference(ORDEAL_TOGGLE_POS[0], ORDEAL_SW_AXIS);
	ORDEALFDAI1Switch.DefineMeshGroup(VC_GRP_Sw_ORDEAL_01);

	MainPanelVC.AddSwitch(&ORDEALFDAI2Switch, AID_VC_SWITCH_ORDEAL_02);
	ORDEALFDAI2Switch.SetReference(ORDEAL_TOGGLE_POS[1], ORDEAL_SW_AXIS);
	ORDEALFDAI2Switch.DefineMeshGroup(VC_GRP_Sw_ORDEAL_02);

	MainPanelVC.AddSwitch(&ORDEALEarthSwitch, AID_VC_SWITCH_ORDEAL_03);
	ORDEALEarthSwitch.SetReference(ORDEAL_TOGGLE_POS[2], ORDEAL_SW_AXIS);
	ORDEALEarthSwitch.DefineMeshGroup(VC_GRP_Sw_ORDEAL_03);

	MainPanelVC.AddSwitch(&ORDEALLightingSwitch, AID_VC_SWITCH_ORDEAL_04);
	ORDEALLightingSwitch.SetReference(ORDEAL_TOGGLE_POS[3], ORDEAL_SW_AXIS);
	ORDEALLightingSwitch.DefineMeshGroup(VC_GRP_Sw_ORDEAL_04);

	MainPanelVC.AddSwitch(&ORDEALModeSwitch, AID_VC_SWITCH_ORDEAL_05);
	ORDEALModeSwitch.SetReference(ORDEAL_TOGGLE_POS[4], ORDEAL_SW_AXIS);
	ORDEALModeSwitch.DefineMeshGroup(VC_GRP_Sw_ORDEAL_05);

	MainPanelVC.AddSwitch(&ORDEALSlewSwitch, AID_VC_SWITCH_ORDEAL_06);
	ORDEALSlewSwitch.SetReference(ORDEAL_TOGGLE_POS[5], ORDEAL_SW_AXIS);
	ORDEALSlewSwitch.DefineMeshGroup(VC_GRP_Sw_ORDEAL_06);

	const VECTOR3	ORDEAL_ROT_AXIS = { 0.318638153340006, 0.947872639959537, 0.002680606686833 };

	MainPanelVC.AddSwitch(&ORDEALAltSetRotary, AID_VC_ROT_ORDEAL_01);
	ORDEALAltSetRotary.SetReference(ORDEAL_RotLocation, ORDEAL_ROT_AXIS);
	ORDEALAltSetRotary.DefineMeshGroup(VC_GRP_ORDEAL_Rot);
	ORDEALAltSetRotary.SetInitialAnimState(133.0 / 285.0); //133° from 10 NM to 150 NM, 285° total range

	// ECS Panels
	const VECTOR3 ECSRotAxisOCM = { -0.12642632210213, 0.0, 0.991976000253902 };
	const VECTOR3 ECSRotAxisWCM = { 0.00021370205398, 0.167627891144257, 0.985850315434327 };
	const VECTOR3 ECSRotAxisSSA = { -1, 0, 0 };
	const VECTOR3 ECSVectorSSA = { 0.003, 0, 0 };

	MainPanelVC.AddSwitch(&SuitCircuitReliefValveSwitch, AID_VC_ROT_SUITCIRCUITRELIEF);
	SuitCircuitReliefValveSwitch.SetReference(Rot_SuitCircuitReliefLocation, ECSRotAxisSSA);
	SuitCircuitReliefValveSwitch.DefineMeshGroup(VC_GRP_Rot_SuitCircuitRelief);

	MainPanelVC.AddSwitch(&CabinGasReturnValveSwitch, AID_VC_ROT_CABINGASRETURN);
	CabinGasReturnValveSwitch.SetReference(Rot_CabinGasReturnLocation, ECSRotAxisSSA);
	CabinGasReturnValveSwitch.DefineMeshGroup(VC_GRP_Rot_CabinGasReturn);

	MainPanelVC.AddSwitch(&CO2CanisterPrimValve, AID_VC_ROT_CANISTER1);
	CO2CanisterPrimValve.SetReference(Rot_Canister1Location, ECSRotAxisSSA);
	CO2CanisterPrimValve.DefineMeshGroup(VC_GRP_Rot_Canister1);

	MainPanelVC.AddSwitch(&CO2CanisterSecValve, AID_VC_ROT_CANISTER2);
	CO2CanisterSecValve.SetReference(Rot_Canister2Location, ECSRotAxisSSA);
	CO2CanisterSecValve.DefineMeshGroup(VC_GRP_Rot_Canister2);

	MainPanelVC.AddSwitch(&CabinRepressValveSwitch, AID_VC_ROT_CABINREPRESS);
	CabinRepressValveSwitch.SetReference(Rot_CabinRepressLocation, ECSRotAxisOCM);
	CabinRepressValveSwitch.DefineMeshGroup(VC_GRP_Rot_CabinRepress);

	MainPanelVC.AddSwitch(&PLSSFillValve, AID_VC_ROT_PLSSFILL);
	PLSSFillValve.SetReference(Rot_PlssFillLocation, ECSRotAxisOCM);
	PLSSFillValve.DefineMeshGroup(VC_GRP_Rot_PlssFill);

	MainPanelVC.AddSwitch(&PressRegAValve, AID_VC_ROT_PRESSREGA);
	PressRegAValve.SetReference(Rot_PressRegALocation, ECSRotAxisOCM);
	PressRegAValve.DefineMeshGroup(VC_GRP_Rot_PressRegA);

	MainPanelVC.AddSwitch(&PressRegBValve, AID_VC_ROT_PRESSREGB);
	PressRegBValve.SetReference(Rot_PressRegBLocation, ECSRotAxisOCM);
	PressRegBValve.DefineMeshGroup(VC_GRP_Rot_PressRegB);

	MainPanelVC.AddSwitch(&DESO2Valve, AID_VC_ROT_DESO2);
	DESO2Valve.SetReference(Rot_DesO2Location, ECSRotAxisOCM);
	DESO2Valve.DefineMeshGroup(VC_GRP_Rot_DesO2);

	MainPanelVC.AddSwitch(&ASCO2Valve1, AID_VC_ROT_ASCO2_1);
	ASCO2Valve1.SetReference(Rot_AscO2_1Location, ECSRotAxisOCM);
	ASCO2Valve1.DefineMeshGroup(VC_GRP_Rot_AscO2_1);

	MainPanelVC.AddSwitch(&ASCO2Valve2, AID_VC_ROT_ASCO2_2);
	ASCO2Valve2.SetReference(Rot_AscO2_2Location, ECSRotAxisOCM);
	ASCO2Valve2.DefineMeshGroup(VC_GRP_Rot_AscO2_2);

	MainPanelVC.AddSwitch(&SecEvapFlowValve, AID_VC_ROT_SECEVAPFLOW);
	SecEvapFlowValve.SetReference(Rot_SecEvapFlowLocation, ECSRotAxisWCM);
	SecEvapFlowValve.DefineMeshGroup(VC_GRP_Rot_SecEvapFlow);

	MainPanelVC.AddSwitch(&PrimEvap2FlowValve, AID_VC_ROT_PRIEVAPFLOW_2);
	PrimEvap2FlowValve.SetReference(Rot_PriEvapFlow2Location, ECSRotAxisWCM);
	PrimEvap2FlowValve.DefineMeshGroup(VC_GRP_Rot_PriEvapFlow2);

	MainPanelVC.AddSwitch(&DESH2OValve, AID_VC_ROT_DESH2O);
	DESH2OValve.SetReference(Rot_DesH2OLocation, ECSRotAxisWCM);
	DESH2OValve.DefineMeshGroup(VC_GRP_Rot_DesH2O);

	MainPanelVC.AddSwitch(&PrimEvap1FlowValve, AID_VC_ROT_PRIEVAPFLOW_1);
	PrimEvap1FlowValve.SetReference(Rot_PriEvapFlow1Location, ECSRotAxisWCM);
	PrimEvap1FlowValve.DefineMeshGroup(VC_GRP_Rot_PriEvapFlow1);

	MainPanelVC.AddSwitch(&SuitTempValve, AID_VC_ROT_SUITTEMP);
	SuitTempValve.SetReference(Rot_SuitTempLocation, ECSRotAxisWCM);
	SuitTempValve.DefineMeshGroup(VC_GRP_Rot_SuitTemp);

	MainPanelVC.AddSwitch(&WaterTankSelectValve, AID_VC_ROT_WATERTANKSEL);
	WaterTankSelectValve.SetReference(Rot_WaterTankSelectorLocation, ECSRotAxisWCM);
	WaterTankSelectValve.DefineMeshGroup(VC_GRP_Rot_WaterTankSelector);

	MainPanelVC.AddSwitch(&LiquidGarmentCoolingValve, AID_VC_ROT_LGC);
	LiquidGarmentCoolingValve.SetReference(Rot_LGCLocation, _V(-0.31982572373671, 0.006794476680294, 0.947452025974366));
	LiquidGarmentCoolingValve.DefineMeshGroup(VC_GRP_Rot_LGC);

	MainPanelVC.AddSwitch(&CDRSuitIsolValve, AID_VC_ROT_SUITISOLCDR);
	CDRSuitIsolValve.SetReference(Rot_SuitIsolCdrLocation, _V(-0.369365282731093, 0.86920601516413, 0.328709888983394));
	CDRSuitIsolValve.DefineMeshGroup(VC_GRP_Rot_SuitIsolCdr);

	MainPanelVC.AddSwitch(&LMPSuitIsolValve, AID_VC_ROT_SUITISOLLMP);
	LMPSuitIsolValve.SetReference(Rot_SuitIsolLmpLocation, _V(0.043971612425928, 0.769550296267906, 0.637070513220194));
	LMPSuitIsolValve.DefineMeshGroup(VC_GRP_Rot_SuitIsolLmp);

	MainPanelVC.AddSwitch(&ASCH2OValve, AID_VC_ROT_ASCH2O);
	ASCH2OValve.SetReference(Rot_AscH2OLocation, ECSRotAxisSSA);
	ASCH2OValve.DefineMeshGroup(VC_GRP_Rot_AscH2O);

	MainPanelVC.AddSwitch(&SuitGasDiverterSwitch, AID_VC_SUITGASDIVERTER);
	SuitGasDiverterSwitch.SetReference(SuitGasDiverterHandleLocation, _V(0, 0, -0.005));
	SuitGasDiverterSwitch.DefineMeshGroup(VC_GRP_SuitGasDiverterHandle);

	MainPanelVC.AddSwitch(&WaterSepSelectSwitch, AID_VC_H2OSEP);
	WaterSepSelectSwitch.SetReference(H20SepHandleLocation, ECSVectorSSA);
	WaterSepSelectSwitch.DefineMeshGroup(VC_GRP_H20SepHandle);

	MainPanelVC.AddSwitch(&CO2CanisterSelectSwitch, AID_VC_CANISTERSEL);
	CO2CanisterSelectSwitch.SetReference(Sw_CanisterSelLocation, ECSRotAxisSSA);
	CO2CanisterSelectSwitch.DefineMeshGroup(VC_GRP_Sw_CanisterSel);
	CO2CanisterSelectSwitch.SetRotationRange(RAD * 30.0);

	MainPanelVC.AddSwitch(&CO2CanisterPrimVent, AID_VC_VENT1);
	CO2CanisterPrimVent.SetReference(PB_Vent1Location, ECSVectorSSA);
	CO2CanisterPrimVent.DefineMeshGroup(VC_GRP_PB_Vent1);

	MainPanelVC.AddSwitch(&CO2CanisterSecVent, AID_VC_VENT2);
	CO2CanisterSecVent.SetReference(PB_Vent2Location, ECSVectorSSA);
	CO2CanisterSecVent.DefineMeshGroup(VC_GRP_PB_Vent2);

	MainPanelVC.AddSwitch(&CDRActuatorOvrd, AID_VC_ACTOVRDCDR);
	CDRActuatorOvrd.SetReference(Sw_ActOvrdCdrLocation, _V(0.211247926629971, 0.424097733272292, -0.880633536794871));
	CDRActuatorOvrd.DefineMeshGroup(VC_GRP_Sw_ActOvrdCdr);
	CDRActuatorOvrd.SetRotationRange(RAD * 20.0);

	MainPanelVC.AddSwitch(&LMPActuatorOvrd, AID_VC_ACTOVRDLMP);
	LMPActuatorOvrd.SetReference(Sw_ActOvrdLmpLocation, _V(-0.567684017188507, 0.544856511735439, -0.617143612336876));
	LMPActuatorOvrd.DefineMeshGroup(VC_GRP_Sw_ActOvrdLmp);
	LMPActuatorOvrd.SetRotationRange(RAD * 20.0);

	// Hatches
	MainPanelVC.AddSwitch(&UpperHatchHandle, AID_VC_OVERHEADHATCHHANDLE);
	MainPanelVC.AddSwitch(&UpperHatchReliefValve, AID_VC_OVERHEADHATCHRELIEFVALVE);

	MainPanelVC.AddSwitch(&ForwardHatchHandle, AID_VC_FORWARDHATCHHANDLE);
	MainPanelVC.AddSwitch(&ForwardHatchReliefValve, AID_VC_FORWARDHATCHRELIEFVALVE);

	// Utility Lights
	MainPanelVC.AddSwitch(&UtilityLightSwitchCDR, AID_VC_UTILITYLIGHTCDR);
	UtilityLightSwitchCDR.SetReference(UtilityLights_CDRLocation, _V(1, 0, 0));
	UtilityLightSwitchCDR.DefineMeshGroup(VC_GRP_Sw_UtilityLightsCDR);

	MainPanelVC.AddSwitch(&UtilityLightSwitchLMP, AID_VC_UTILITYLIGHTLMP);
	UtilityLightSwitchLMP.SetReference(UtilityLights_LMPLocation, _V(1, 0, 0));
	UtilityLightSwitchLMP.DefineMeshGroup(VC_GRP_Sw_UtilityLightsLMP);

	// AOT
	MainPanelVC.AddSwitch(&RRGyroSelSwitch, AID_VC_SW_RRGYRO);
	RRGyroSelSwitch.SetReference(Sw_RRGyroLocation, _V(-0.048633374944462, -0.519162328382934, 0.853290848204481));
	RRGyroSelSwitch.DefineMeshGroup(VC_GRP_Sw_RRGyro);

	MainPanelVC.DefineVCAnimations(vcidx);
	crossPointerLeft.DefineVCAnimations(vcidx, true);
	crossPointerRight.DefineVCAnimations(vcidx, false);

	InitFDAI(vcidx);
}

void LEM::InitFDAI(UINT mesh) {

	// 3D FDAI initialization

	// Constants
	const VECTOR3 fdairollaxis = { -0.00, sin(P1_TILT), -cos(P1_TILT) };
	const VECTOR3 fdaiyawvaxis = { -0.00, sin(P1_TILT + (90.0 * RAD)), -cos(P1_TILT + (90.0 * RAD)) };
	const VECTOR3 needlexvector = { 0.00, 0.05*cos(P1_TILT), 0.05*sin(P1_TILT) };
	const VECTOR3 needleyvector = { 0.05, 0, 0 };
	const VECTOR3 ratexvector = { 0.00, 0.062*cos(P1_TILT), 0.062*sin(P1_TILT) };
	const VECTOR3 rateyvector = { 0.062, 0, 0 };
	const VECTOR3 FDAI_PIVOTCDR = { -0.297851, 0.525802, 1.75639 }; // CDR FDAI Pivot Point
	const VECTOR3 FDAI_PIVOTLMP = { 0.297069, 0.525802, 1.75565 }; // LMP FDAI Pivot Point
	const VECTOR3 attflagvector = { 0.01, 0, 0 };

	// CDR FDAI Ball
	ANIMATIONCOMPONENT_HANDLE	ach_FDAIroll_cdr, ach_FDAIpitch_cdr, ach_FDAIyaw_cdr;
	static UINT meshgroup_Fdai1_cdr = { VC_GRP_FDAIBall1_cdr }; // Roll gimbal meshgroup (includes roll incicator)
	static UINT meshgroup_Fdai2_cdr = { VC_GRP_FDAIBall_cdr };  // Pitch gimbal meshgroup (visible ball)
	static UINT meshgroup_Fdai3_cdr = { VC_GRP_FDAIBall2_cdr }; // Yaw gimbal meshgroup
	static MGROUP_ROTATE mgt_FDAIRoll_cdr(mesh, &meshgroup_Fdai1_cdr, 1, FDAI_PIVOTCDR, fdairollaxis, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIPitch_cdr(mesh, &meshgroup_Fdai2_cdr, 1, FDAI_PIVOTCDR, _V(-1, 0, 0), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIYaw_cdr(mesh, &meshgroup_Fdai3_cdr, 1, FDAI_PIVOTCDR, fdaiyawvaxis, (float)(RAD * 360));
	anim_fdaiR_cdr = CreateAnimation(0.0);
	anim_fdaiP_cdr = CreateAnimation(0.0);
	anim_fdaiY_cdr = CreateAnimation(0.0);
	ach_FDAIroll_cdr = AddAnimationComponent(anim_fdaiR_cdr, 0.0f, 1.0f, &mgt_FDAIRoll_cdr);
	ach_FDAIyaw_cdr = AddAnimationComponent(anim_fdaiY_cdr, 0.0f, 1.0f, &mgt_FDAIYaw_cdr, ach_FDAIroll_cdr);
	ach_FDAIpitch_cdr = AddAnimationComponent(anim_fdaiP_cdr, 0.0f, 1.0f, &mgt_FDAIPitch_cdr, ach_FDAIyaw_cdr);

	// CDR FDAI error needles
	static UINT meshgroup_RollError_cdr = VC_GRP_FDAI_rollerror_cdr;
	static UINT meshgroup_PitchError_cdr = VC_GRP_FDAI_pitcherror_cdr;
	static UINT meshgroup_YawError_cdr = VC_GRP_FDAI_yawerror_cdr;
	static MGROUP_TRANSLATE mgt_rollerror_cdr(mesh, &meshgroup_RollError_cdr, 1, needleyvector);
	static MGROUP_TRANSLATE mgt_pitcherror_cdr(mesh, &meshgroup_PitchError_cdr, 1, needlexvector);
	static MGROUP_TRANSLATE mgt_yawerror_cdr(mesh, &meshgroup_YawError_cdr, 1, needleyvector);
	anim_fdaiRerror_cdr = CreateAnimation(0.5);
	anim_fdaiPerror_cdr = CreateAnimation(0.5);
	anim_fdaiYerror_cdr = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRerror_cdr, 0.0f, 1.0f, &mgt_rollerror_cdr);
	AddAnimationComponent(anim_fdaiPerror_cdr, 0.0f, 1.0f, &mgt_pitcherror_cdr);
	AddAnimationComponent(anim_fdaiYerror_cdr, 0.0f, 1.0f, &mgt_yawerror_cdr);

	// CDR FDAI rate needles
	static UINT meshgroup_RollRate_cdr = VC_GRP_FDAI_rateR_cdr;
	static UINT meshgroup_PitchRate_cdr = VC_GRP_FDAI_rateP_cdr;
	static UINT meshgroup_YawRate_cdr = VC_GRP_FDAI_rateY_cdr;
	static MGROUP_TRANSLATE mgt_rollrate_cdr(mesh, &meshgroup_RollRate_cdr, 1, rateyvector);
	static MGROUP_TRANSLATE mgt_pitchrate_cdr(mesh, &meshgroup_PitchRate_cdr, 1, ratexvector);
	static MGROUP_TRANSLATE mgt_yawrate_cdr(mesh, &meshgroup_YawRate_cdr, 1, rateyvector);
	anim_fdaiRrate_cdr = CreateAnimation(0.5);
	anim_fdaiPrate_cdr = CreateAnimation(0.5);
	anim_fdaiYrate_cdr = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRrate_cdr, 0.0f, 1.0f, &mgt_rollrate_cdr);
	AddAnimationComponent(anim_fdaiPrate_cdr, 0.0f, 1.0f, &mgt_pitchrate_cdr);
	AddAnimationComponent(anim_fdaiYrate_cdr, 0.0f, 1.0f, &mgt_yawrate_cdr);

	// CDR FDAI no attitude flag
	static UINT meshgroup_attflag_cdr = VC_GRP_FDAIflag_cdr;
	static MGROUP_TRANSLATE mgt_attflag_cdr(mesh, &meshgroup_attflag_cdr, 1, attflagvector);
	anim_attflag_cdr = CreateAnimation(1.0);
	AddAnimationComponent(anim_attflag_cdr, 0.0f, 1.0f, &mgt_attflag_cdr);

	// LMP FDAI Ball
	ANIMATIONCOMPONENT_HANDLE	ach_FDAIroll_lmp, ach_FDAIpitch_lmp, ach_FDAIyaw_lmp;
	static UINT meshgroup_Fdai1_lmp = { VC_GRP_FDAIBall1_lmp }; // Roll gimbal meshgroup (includes roll incicator)
	static UINT meshgroup_Fdai2_lmp = { VC_GRP_FDAIBall_lmp };  // Pitch gimbal meshgroup (visible ball)
	static UINT meshgroup_Fdai3_lmp = { VC_GRP_FDAIBall2_lmp }; // Yaw gimbal meshgroup
	static MGROUP_ROTATE mgt_FDAIRoll_lmp(mesh, &meshgroup_Fdai1_lmp, 1, FDAI_PIVOTLMP, fdairollaxis, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIPitch_lmp(mesh, &meshgroup_Fdai2_lmp, 1, FDAI_PIVOTLMP, _V(-1, 0, 0), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIYaw_lmp(mesh, &meshgroup_Fdai3_lmp, 1, FDAI_PIVOTLMP, fdaiyawvaxis, (float)(RAD * 360));
	anim_fdaiR_lmp = CreateAnimation(0.0);
	anim_fdaiP_lmp = CreateAnimation(0.0);
	anim_fdaiY_lmp = CreateAnimation(0.0);
	ach_FDAIroll_lmp = AddAnimationComponent(anim_fdaiR_lmp, 0.0f, 1.0f, &mgt_FDAIRoll_lmp);
	ach_FDAIyaw_lmp = AddAnimationComponent(anim_fdaiY_lmp, 0.0f, 1.0f, &mgt_FDAIYaw_lmp, ach_FDAIroll_lmp);
	ach_FDAIpitch_lmp = AddAnimationComponent(anim_fdaiP_lmp, 0.0f, 1.0f, &mgt_FDAIPitch_lmp, ach_FDAIyaw_lmp);

	// LMP FDAI error needles
	static UINT meshgroup_RollError_lmp = VC_GRP_FDAI_rollerror_lmp;
	static UINT meshgroup_PitchError_lmp = VC_GRP_FDAI_pitcherror_lmp;
	static UINT meshgroup_YawError_lmp = VC_GRP_FDAI_yawerror_lmp;
	static MGROUP_TRANSLATE mgt_rollerror_lmp(mesh, &meshgroup_RollError_lmp, 1, needleyvector);
	static MGROUP_TRANSLATE mgt_pitcherror_lmp(mesh, &meshgroup_PitchError_lmp, 1, needlexvector);
	static MGROUP_TRANSLATE mgt_yawerror_lmp(mesh, &meshgroup_YawError_lmp, 1, needleyvector);
	anim_fdaiRerror_lmp = CreateAnimation(0.5);
	anim_fdaiPerror_lmp = CreateAnimation(0.5);
	anim_fdaiYerror_lmp = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRerror_lmp, 0.0f, 1.0f, &mgt_rollerror_lmp);
	AddAnimationComponent(anim_fdaiPerror_lmp, 0.0f, 1.0f, &mgt_pitcherror_lmp);
	AddAnimationComponent(anim_fdaiYerror_lmp, 0.0f, 1.0f, &mgt_yawerror_lmp);

	// LMP FDAI rate needles
	static UINT meshgroup_RollRate_lmp = VC_GRP_FDAI_rateR_lmp;
	static UINT meshgroup_PitchRate_lmp = VC_GRP_FDAI_rateP_lmp;
	static UINT meshgroup_YawRate_lmp = VC_GRP_FDAI_rateY_lmp;
	static MGROUP_TRANSLATE mgt_rollrate_lmp(mesh, &meshgroup_RollRate_lmp, 1, rateyvector);
	static MGROUP_TRANSLATE mgt_pitchrate_lmp(mesh, &meshgroup_PitchRate_lmp, 1, ratexvector);
	static MGROUP_TRANSLATE mgt_yawrate_lmp(mesh, &meshgroup_YawRate_lmp, 1, rateyvector);
	anim_fdaiRrate_lmp = CreateAnimation(0.5);
	anim_fdaiPrate_lmp = CreateAnimation(0.5);
	anim_fdaiYrate_lmp = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRrate_lmp, 0.0f, 1.0f, &mgt_rollrate_lmp);
	AddAnimationComponent(anim_fdaiPrate_lmp, 0.0f, 1.0f, &mgt_pitchrate_lmp);
	AddAnimationComponent(anim_fdaiYrate_lmp, 0.0f, 1.0f, &mgt_yawrate_lmp);

	// LMP FDAI no attitude flag
	static UINT meshgroup_attflag_lmp = VC_GRP_FDAIflag_lmp;
	static MGROUP_TRANSLATE mgt_attflag_lmp(mesh, &meshgroup_attflag_lmp, 1, attflagvector);
	anim_attflag_lmp = CreateAnimation(1.0);
	AddAnimationComponent(anim_attflag_lmp, 0.0f, 1.0f, &mgt_attflag_lmp);
}

void LEM::SetCompLight(int m, bool state) {

	if (!vcmesh)
		return;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, m);

	if (state == true)
	{   // ON
		mat->emissive.r = 1;
		mat->emissive.g = 0.878f;
		mat->emissive.b = 0.506f;
		mat->emissive.a = 1;
	}
	else
	{   // OFF
		mat->emissive.r = 0.125f;
		mat->emissive.g = 0.11f;
		mat->emissive.b = 0.064f;
		mat->emissive.a = 1;
	}

	oapiSetMaterial(vcmesh, m, mat);
}

void LEM::SetContactLight(int m, bool state) {

	if (!vcmesh)
		return;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, m);

	if (state == true)
	{   // ON
		mat->emissive.r = 0;
		mat->emissive.g = 0.541f;
		mat->emissive.b = 1;
		mat->emissive.a = 1;
	}
	else
	{   // OFF
		mat->emissive.r = 0;
		mat->emissive.g = 0.068f;
		mat->emissive.b = 0.125f;
		mat->emissive.a = 1;
	}

	oapiSetMaterial(vcmesh, m, mat);
}

void LEM::SetPowerFailureLight(int m, bool state) {

	if (!vcmesh)
		return;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, m);

	if (state == true)
	{   // ON
		mat->emissive.r = 1;
		mat->emissive.g = 0;
		mat->emissive.b = 0;
		mat->emissive.a = 1;
	}
	else
	{   // OFF
		mat->emissive.r = 0.125f;
		mat->emissive.g = 0;
		mat->emissive.b = 0;
		mat->emissive.a = 1;
	}

	oapiSetMaterial(vcmesh, m, mat);
}

void LEM::SetStageSeqRelayLight(int m, bool state) {

	if (!vcmesh)
		return;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, m);

	if (state == true)
	{   // ON
		mat->emissive.r = 1;
		mat->emissive.g = 1;
		mat->emissive.b = 1;
		mat->emissive.a = 1;
	}
	else
	{   // OFF
		mat->emissive.r = 0.125f;
		mat->emissive.g = 0.125f;
		mat->emissive.b = 0.125f;
		mat->emissive.a = 1;
	}

	oapiSetMaterial(vcmesh, m, mat);
}

#ifdef _OPENORBITER
void LEM::SetLMVCIntegralLight(UINT meshidx, DWORD *matList, MatProp EmissionMode, double state, int cnt)
#else
void LEM::SetLMVCIntegralLight(UINT meshidx, DWORD *matList, int EmissionMode, double state, int cnt)
#endif

{
	if (vis == NULL || meshidx == -1) return;
	DEVMESHHANDLE hMesh = GetDevMesh(vis, meshidx);

    if (!hMesh)
        return;

	for (int i = 0; i < cnt; i++)
	{
		gcCore *pCore = gcGetCoreInterface();
		if (pCore) {
			FVECTOR4 value;
			value.r = (float)state;
			value.g = (float)state;
			value.b = (float)state;
			value.a = 1.0;
#ifdef _OPENORBITER
			pCore->SetMeshMaterial(hMesh, matList[i], EmissionMode, &value);
#else
			pCore->MeshMaterial(hMesh, matList[i], EmissionMode, &value, true);
#endif
		}
	}
    //sprintf(oapiDebugString(), "%d %lf", m, state);
}

void LEM::MoveFlashlight()
{
	if (flashlightOn) { //Only move the light emmitter if the flashlight is on
		//Huge thanks the Jordan64 for helping get the direction stuff working! :)
		GetCameraOffset(flashlightPos);
		oapiCameraGlobalDir(&flashlightDirGlobal);
		GetGlobalPos(vesselPosGlobal);
		Global2Local(vesselPosGlobal + flashlightDirGlobal, flashlightDirLocal);
		normalise(flashlightDirLocal);

		flashlight->SetPosition(flashlightPos);
		flashlight->SetDirection(flashlightDirLocal);
	}
}

void LEM::SetFlashlightOn(bool state)
{
	flashlight->Activate(state);
	flashlightOn = state;
}

void LEM::ToggleFlashlight()
{
	if ((oapiCockpitMode() == COCKPIT_VIRTUAL) && (oapiCameraMode() == CAM_COCKPIT)) {
		SetFlashlightOn(!flashlightOn);
	}
}
