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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "lmresource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "LM_VC_Resource.h"
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
	{-0.072168, 0.359422, 1.67335}
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
	{0.052147, 0.353926, 1.67237}, {0.243652, 0.400778, 1.67901}, {0.243665, 0.327141, 1.6687}, {0.356315, 0.364088, 1.67391}
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
	{-0.34068, 0.15311, 1.56197}, {-0.19343, 0.163151, 1.57026}, {0.063658, 0.163679, 1.57046}, {0.27805, 0.151635, 1.56925}, {0.31772, 0.220734, 1.61811}
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
{-0.5854, -0.0136, 1.4165}, {-0.4982, -0.0136, 1.4164}, {-0.4304, -0.0136, 1.4164}
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
{0.9876, 0.0424, 1.0606}, {0.9828, 0.0409, 0.9785}, {0.9941, 0.0442, 0.9123}, {1.0780, 0.0693, 0.9122}
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
{1.0065, 0.3466, 1.2119}
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

// Subtracted from total material count to find L01 location.
const int mat_L01 = 40;

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

void LEM::SetView() {

	VECTOR3 v;
	//
	// Set camera offset
	//
	if (InVC) {
		switch (viewpos) {
		case LMVIEW_CDR:
			if (stage == 2) {
				v = _V(-0.45, -0.07, 1.25);
			}
			else {
				v = _V(-0.45, 1.68, 1.25);
			}
			SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			break;

		case LMVIEW_LMP:
			if (stage == 2) {
				v = _V(0.45, -0.07, 1.25);
			}
			else {
				v = _V(0.45, 1.68, 1.25);
			}
			SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			break;

		case LMVIEW_LPD:
			if (stage == 2) {
				v = _V(-0.58, -0.15, 1.40);
			}
			else {
				v = _V(-0.58, 1.60, 1.40);
			}
			SetCameraDefaultDirection(_V(0.0, -sin(VIEWANGLE * RAD), cos(VIEWANGLE * RAD)));
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
					if (stage == 2) {
						v =_V(0, 0.15, 1.26);
					}
					else {
						v =_V(0, 1.90, 1.26);
					}
					break;
				case LMPANEL_RIGHTWINDOW:
					if (stage == 2) {
						v =_V(0.576, 0.15, 1.26);
					}
					else {
						v =_V(0.576, 1.90, 1.26);
					}
					break;
				case LMPANEL_LEFTWINDOW:
					if (stage == 2) {
						v =_V(-0.576, 0.15, 1.26);
					}
					else {
						v =_V(-0.576, 1.90, 1.26);
					}
					break;
				case LMPANEL_LPDWINDOW:
					if (stage == 2) {
						v = _V(-0.58, -0.15, 1.40);
					}
					else {
						v = _V(-0.58, 1.60, 1.40);
					}
					break;
				case LMPANEL_RNDZWINDOW:
					if (stage == 2) {
						v =_V(-0.598, 0.15, 1.106);
					}
					else {
						v =_V(-0.598, 1.90, 1.106);
					}
					break;
				case LMPANEL_LEFTPANEL:
					if (stage == 2) {
						v =_V(-0.576, 0.15, 1.26);
					}
					else {
						v =_V(-0.576, 1.90, 1.26);
					}
					break;
				case LMPANEL_AOTVIEW:
					if (stage == 2) {
						v =_V(0, 1.13, 1.26);
					}
					else {
						v =_V(0, 2.88, 1.26);
					}
					break;
				case LMPANEL_AOTZOOM:
					if (stage == 2) {
						v =_V(0, 1.13, 1.26);
					}
					else {
						v =_V(0, 2.88, 1.26);
					}
					break;
				case LMPANEL_DOCKVIEW:
					if (stage == 2) {
						v =_V(-0.598, 0.15, 1.106);
					}
					else {
						v =_V(-0.598, 1.90, 1.106);
					}
					break;
				
				case LMPANEL_LEFTZOOM:
					if (stage == 2) {
						v =_V(-0.576, 0.15, 1.26);
					}
					else {
						v =_V(-0.576, 1.90, 1.26);
					}
					break;
				case LMPANEL_UPPERHATCH:
					if (stage == 2) {
						v =_V(0, -0.55, 0);
					}
					else {
						v =_V(0, 1.20, 0);
					}
					SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
					oapiCameraSetCockpitDir(180 * RAD, 0);
					break;
				case LMPANEL_FWDHATCH:
					if (stage == 2) {
						v =_V(0, -1.4, 1.5);
					}
					else {
						v =_V(0, 0.35, 1.5);
					}
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
	RegisterActiveAreas();

	switch (id) {
	case LMVIEW_CDR:
		viewpos = LMVIEW_CDR;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(-1, LMVIEW_LMP, -1, LMVIEW_LPD);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();

		return true;

	case LMVIEW_LMP:
		viewpos = LMVIEW_LMP;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.1, 0.0, 0.1), 0, 0, _V(-0.1, 0.0, 0.0), 0, 0, _V(0.1, -0.15, 0.0), -90.0 * RAD, 0);
		oapiVCSetNeighbours(LMVIEW_CDR, -1, -1, -1);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();

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

		return true;

	default:
		return false;
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

	SURFHANDLE MainPanelTex1 = oapiGetTextureHandle(hLMVC, 2);
	SURFHANDLE MainPanelTex2 = oapiGetTextureHandle(hLMVC, 8);

	// Panel 1
	for (i = 0; i < P1_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P1_01 + i, P1_TOGGLE_POS[i] + P1_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P1_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P1_01 + i, P1_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_LM_CWS_LEFT, _R(238, 27, 559, 153), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MISSION_CLOCK, _R(60, 259, 202, 281), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_EVENT_TIMER, _R(276, 259, 357, 281), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RANGE_TAPE, _R(431, 633, 475, 796), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RATE_TAPE, _R(482, 633, 517, 796), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LEM_MA_LEFT, _R(30, 593, 77, 636), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCSetAreaClickmode_Spherical(AID_VC_LEM_MA_LEFT, _V(-0.415919, 0.599307, 1.70252) + ofs, 0.008);
	oapiVCRegisterArea(AID_VC_MPS_REG_CONTROLS_LEFT, _R(341, 891, 377, 1098), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MPS_REG_CONTROLS_RIGHT, _R(415, 891, 451, 1098), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MPS_OXID_QUANTITY_INDICATOR, _R(445, 218, 484, 239), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MPS_FUEL_QUANTITY_INDICATOR, _R(445, 270, 484, 292), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MPS_HELIUM_PRESS_INDICATOR, _R(577, 259, 658, 281), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	
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
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P2_01 + i, P2_TOGGLE_POS[i] + P2_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P2_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P2_01 + i, P2_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_LM_CWS_RIGHT, _R(1075, 27, 1375, 153), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_ASC_FEED_TALKBACKS, _R(794, 413, 1031, 436), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LGC_CMD_ENABLE_14_TALKBACKS, _R(794, 562, 1031, 585), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LGC_CMD_ENABLE_23_TALKBACKS, _R(794, 688, 1031, 711), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_XFEED_TALKBACK, _R(795, 844, 818, 867), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MAIN_SOV_TALKBACKS, _R(934, 844, 1027, 867), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LEM_MA_RIGHT, _R(1384, 593, 1431, 636), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCSetAreaClickmode_Spherical(AID_VC_LEM_MA_RIGHT, _V(0.414751, 0.59891, 1.70247) + ofs, 0.008);

	oapiVCRegisterArea(AID_VC_PANEL2_COMPLIGHTS, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PWRFAIL_LIGHTS_P2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_XPOINTERLMP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FDAI_RIGHT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	for (i = 0; i < P3_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P3_01 + i, P3_TOGGLE_POS[i] + P3_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P3_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P3_01 + i, P3_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_RR_NOTRACK, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_CONTACTLIGHT2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_RR_SLEW_SWITCH, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_RR_SLEW_SWITCH, _V(-0.27271, 0.157539, 1.57055) + ofs, _V(-0.255647, 0.157539, 1.57055) + ofs, _V(-0.27271, 0.143561, 1.56076) + ofs, _V(-0.255647, 0.143561, 1.56076) + ofs);

	// Panel 4
	for (i = 0; i < P4_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P4_01 + i, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P4_01 + i, P4_PUSHB_POS[i] + ofs, 0.008);
	}

	for (i = 0; i < P4_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P4_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P4_01 + i, P4_TOGGLE_POS[i] + P4_CLICK + ofs, 0.006);
	}

	oapiVCRegisterArea(AID_VC_DSKY_DISPLAY, _R(309, 1520, 414, 1696), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_DSKY_LIGHTS,  _R(165, 1525, 267, 1694), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	// Panel 5

	for (i = 0; i < P5_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P5_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P5_01 + i, P5_TOGGLE_POS[i] + P5_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P5_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P5_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P5_01 + i, P5_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_START_BUTTON, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_START_BUTTON, _V(-0.644677, 0.073766, 1.48271) + ofs, 0.01);
	oapiVCRegisterArea(AID_VC_STOP_BUTTON_CDR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_STOP_BUTTON_CDR, _V(-0.694894, 0.085711, 1.53811) + ofs, 0.01);
	oapiVCRegisterArea(AID_VC_PLUSX_BUTTON, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_PLUSX_BUTTON, _V(-0.65603, -0.017321, 1.41484) + ofs, 0.01);

	// Panel 6

	for (i = 0; i < P6_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P6_01 + i, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P6_01 + i, P6_PUSHB_POS[i] + ofs, 0.008);
	}

	for (i = 0; i < P6_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P6_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P6_01 + i, P6_TOGGLE_POS[i] + P6_CLICK + ofs, 0.006);
	}

	oapiVCRegisterArea(AID_VC_STOP_BUTTON_LMP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_STOP_BUTTON_LMP, _V(0.421597, 0.077646, 1.50924) + ofs, 0.01);

	oapiVCRegisterArea(AID_VC_LM_DEDA_DISP, _R(1803, 98, 1937, 120), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LM_DEDA_ADR, _R(1827, 53, 1885, 75), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LM_DEDA_LIGHTS, _R(1740, 96, 1787, 122), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	// Panel 8

	for (i = 0; i < P8_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P8_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P8_01 + i, P8_TOGGLE_POS[i] + P8_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P8_TWCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_TW_P8_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P8_01 + i, P8_TW_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_SEQ_LIGHTS, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PANEL8_TALKBACKS, _R(1389, 8, 1772, 149), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

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

	for (i = 0; i < P12_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P12_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P12_01 + i, P12_TOGGLE_POS[i] + P12_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P12_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P12_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P12_01 + i, P12_ROT_POS[i] + ofs, 0.02);
	}

	for (i = 0; i < P12_TWCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_TW_P12_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P12_01 + i, P12_TW_POS[i] + ofs, 0.02);
	}

	//oapiVCRegisterArea(AID_VC_RECORDER_TALKBACK, _R(806, 1460, 829, 1483), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	// Panel 14

	for (i = 0; i < P14_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P14_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P14_01 + i, P14_TOGGLE_POS[i] + P14_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P14_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P14_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P14_01 + i, P14_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_PANEL14_COMPLIGHTS, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_DSC_BATTERY_TALKBACKS, _R(550, 1002, 865, 1025), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_ASC_BATTERY_TALKBACKS, _R(932, 1027, 1176, 1050), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

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

	//
	// Initialize surfaces and switches
	//
	InitPanelVC();

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
		case AID_VC_LEM_MA_LEFT:
		case AID_VC_LEM_MA_RIGHT:
			return CWEA.CheckMasterAlarmMouseClick(event);
	}
	return MainPanelVC.VCMouseEvent(id, event, p);
	//return false;
}

bool LEM::clbkVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	switch (id) {

	case AID_VC_LM_CWS_LEFT:
		CWEA.RedrawLeft(surf, srf[SFR_VC_CW_LIGHTS]);
		return true;

	case AID_VC_LM_CWS_RIGHT:
		CWEA.RedrawRight(surf, srf[SFR_VC_CW_LIGHTS]);
		return true;

	case AID_VC_MISSION_CLOCK:
		MissionTimerDisplay.Render(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_EVENT_TIMER:
		EventTimerDisplay.Render(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[SRF_VC_DIGITALDISP], srf[SRF_VC_DSKYDISP]);
		return true;

	case AID_VC_DSKY_LIGHTS:
		if (pMission->GetLMDSKYVersion() == 3)
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS], 0, 0, true, true);
		}
		else if (pMission->GetLMDSKYVersion() == 2)
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS]);
		}
		else
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS], 0, 0, false);
		}
		return true;

	case AID_VC_LM_DEDA_LIGHTS:
		deda.RenderOprErr(surf, srf[SRF_DEDA_LIGHTSVC]);
		return true;

	case AID_VC_LM_DEDA_DISP:
		deda.RenderData(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_LM_DEDA_ADR:
		deda.RenderAdr(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_RANGE_TAPE:
		RadarTape.RenderRangeVC(surf, srf[SRF_VC_RADAR_TAPEA], srf[SRF_VC_RADAR_TAPEB], srf[SRF_VC_RADAR_TAPE2]);
		return true;

	case AID_VC_RATE_TAPE:
		RadarTape.RenderRateVC(surf, srf[SRF_VC_RADAR_TAPEA], srf[SRF_VC_RADAR_TAPEB]);
		return true;

	case AID_VC_XPOINTERCDR:
		crossPointerLeft.DrawSwitchVC(id, event, surf);
		return true;

	case AID_VC_XPOINTERLMP:
		crossPointerRight.DrawSwitchVC(id, event, surf);
		return true;

	case AID_VC_CONTACTLIGHT1:
		if (SCS_ENG_CONT_CB.IsPowered() && (scca3.GetContactLightLogic() || LampToneTestRotary.GetState() == 6)) {
			SetContactLight(0, true); // Light On
		}
		else
		{
			SetContactLight(0, false); // Light On
		}
		return true;

	case AID_VC_CONTACTLIGHT2:
		if (SCS_ATCA_CB.IsPowered() && (scca3.GetContactLightLogic() || LampToneTestRotary.GetState() == 6)) {
			SetContactLight(1, true); // Light On
		}
		else
		{
			SetContactLight(1, false); // Light On
		}
		return true;

	case AID_VC_FDAI_LEFT:
		if (fdaiLeft.IsPowered()) {
			VECTOR3 attitude;
			VECTOR3 errors;
			VECTOR3 rates;
			int no_att = 0;

			if (AttitudeMonSwitch.IsUp())	//PGNS
			{
				attitude = gasta.GetTotalAttitude();
			}
			else							//AGS
			{
				attitude = aea.GetTotalAttitude();
			}

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

			// ORDEAL
			if (!no_att) {
				attitude.y += ordeal.GetFDAI1PitchAngle();
				if (attitude.y >= TWO_PI) attitude.y -= TWO_PI;
			}

			if (RateScaleSwitch.IsUp())
			{
				rates = rga.GetRates() / (25.0*RAD);
			}
			else
			{
				rates = rga.GetRates() / (5.0*RAD);
			}

			AnimateFDAI(attitude, rates, errors, anim_fdaiR_cdr, anim_fdaiP_cdr, anim_fdaiY_cdr, anim_fdaiRerror_cdr, anim_fdaiPerror_cdr, anim_fdaiYerror_cdr, anim_fdaiRrate_cdr, anim_fdaiPrate_cdr, anim_fdaiYrate_cdr);
			SetAnimation(anim_attflag_cdr, 0.0);
		}
		else
		{
			SetAnimation(anim_attflag_cdr, 1.0);
		}
		return true;

	case AID_VC_FDAI_RIGHT:
		if (fdaiRight.IsPowered()) {
			VECTOR3 attitude;
			VECTOR3 errors;
			VECTOR3 rates;
			int no_att = 0;

			if (RightAttitudeMonSwitch.IsUp())	//PGNS
			{
				attitude = gasta.GetTotalAttitude();
			}
			else							//AGS
			{
				attitude = aea.GetTotalAttitude();
			}

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

			// ORDEAL
			if (!no_att) {
				attitude.y += ordeal.GetFDAI2PitchAngle();
				if (attitude.y >= TWO_PI) attitude.y -= TWO_PI;
			}

			if (RateScaleSwitch.IsUp())
			{
				rates = rga.GetRates() / (25.0*RAD);
			}
			else
			{
				rates = rga.GetRates() / (5.0*RAD);
			}

			AnimateFDAI(attitude, rates, errors, anim_fdaiR_lmp, anim_fdaiP_lmp, anim_fdaiY_lmp, anim_fdaiRerror_lmp, anim_fdaiPerror_lmp, anim_fdaiYerror_lmp, anim_fdaiRrate_lmp, anim_fdaiPrate_lmp, anim_fdaiYrate_lmp);
			SetAnimation(anim_attflag_lmp, 0.0);
		}
		else
		{
			SetAnimation(anim_attflag_lmp, 1.0);
		}
		return true;

	case AID_VC_RR_NOTRACK:
		if (lca.GetAnnunVoltage() > 2.25 && (RR.GetNoTrackSignal() || LampToneTestRotary.GetState() == 6)) { // The AC side is only needed for the transmitter
			SetCompLight(LM_VC_COMP_LIGHT_1, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_1, false); // Light Off
		}
		return true;

	case AID_VC_PANEL2_COMPLIGHTS:
		if (lca.GetAnnunVoltage() > 2.25 && (scera2.GetSwitch(12, 2)->IsClosed() || PrimGlycolPumpController.GetPressureSwitch() == true || LampToneTestRotary.GetState() == 6)) {
			SetCompLight(LM_VC_COMP_LIGHT_2, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_2, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25 && (SuitFanDPSensor.GetSuitFanFail() == true || LampToneTestRotary.GetState() == 6)) {
			SetCompLight(LM_VC_COMP_LIGHT_3, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_3, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25) {
			if (INST_CWEA_CB.IsPowered() && ECS_CO2_SENSOR_CB.IsPowered() && (scera1.GetVoltage(5, 2) >= (7.6 / 6))) {
				SetCompLight(LM_VC_COMP_LIGHT_4, true); // Light On
			}
			else if (CO2CanisterSelectSwitch.GetState() == 0 || LampToneTestRotary.GetState() == 6) {
				SetCompLight(LM_VC_COMP_LIGHT_4, true); // Light On
			}
			else {
				SetCompLight(LM_VC_COMP_LIGHT_4, false); // Light Off
			}
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_4, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25 && INST_CWEA_CB.IsPowered() && (scera1.GetVoltage(5, 3) < (792.5 / 720.0) || LampToneTestRotary.GetState() == 6)) {
			SetCompLight(LM_VC_COMP_LIGHT_5, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_5, false); // Light Off
		}
		return true;

	case AID_VC_PANEL14_COMPLIGHTS:
		if (lca.GetCompDockVoltage() > 2.25 && (LampToneTestRotary.GetState() == 6)) {
			SetCompLight(LM_VC_COMP_LIGHT_6, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_6, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25 && (LampToneTestRotary.GetState() == 6)) {
			SetCompLight(LM_VC_COMP_LIGHT_7, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_7, false); // Light Off
		}
		return true;

	case AID_VC_SEQ_LIGHTS:
		if (lca.GetCompDockVoltage() > 2.25 && (scera1.GetVoltage(12, 11) > 2.5 && stage < 2 || LampToneTestRotary.GetState() == 6)) {
			SetStageSeqRelayLight(LM_VC_STG_SEQ_A, true); // Light On
		}
		else {
			SetStageSeqRelayLight(LM_VC_STG_SEQ_A, false); // Light Off
		}

		if (lca.GetCompDockVoltage() > 2.25 && (scera1.GetVoltage(12, 12) > 2.5 || LampToneTestRotary.GetState() == 6)) {
			SetStageSeqRelayLight(LM_VC_STG_SEQ_B, true); // Light On
		}
		else {
			SetStageSeqRelayLight(LM_VC_STG_SEQ_B, false); // Light Off
		}
		return true;

	case AID_VC_PWRFAIL_LIGHTS_P1:
		if (!pfira.GetCDRXPointerRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_1, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_1, false); // Light Off
		}

		if (!pfira.GetThrustIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_2, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_2, false); // Light Off
		}

		if (!pfira.GetPropPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_3, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_3, false); // Light Off
		}
		return true;

	case AID_VC_PWRFAIL_LIGHTS_P2:

		if (!pfira.GetRCSPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_4, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_4, false); // Light Off
		}

		if (!pfira.GetRCSQtyIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_5, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_5, false); // Light Off
		}

		if (!pfira.GetSuitCabinPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_6, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_6, false); // Light Off
		}

		if (!pfira.GetGlyTempPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_7, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_7, false); // Light Off
		}

		if (!pfira.GetO2H2OQtyIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_8, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_8, false); // Light Off
		}

		if (!pfira.GetLMPXPointerRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_9, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_9, false); // Light Off
		}
		return true;

	case AID_VC_LEM_MA_LEFT:
	case AID_VC_LEM_MA_RIGHT:
		CWEA.RenderMasterAlarm(surf, srf[SRF_LEM_MASTERALARMVC], NULL);
		return true;

	/*case AID_VC_RECORDER_TALKBACK:
		TapeRecorderTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;*/
	}

	return MainPanelVC.VCRedrawEvent(id, event, surf);
	//return false;
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

	VECTOR3 xvector = { 0.00, 0.0625*cos(P1_TILT), 0.0625*sin(P1_TILT) };

	MainPanelVC.AddSwitch(&EngineThrustInd);
	EngineThrustInd.SetDirection(xvector);
	EngineThrustInd.DefineMeshGroup(VC_GRP_Needle_P1_01);

	MainPanelVC.AddSwitch(&CommandedThrustInd);
	CommandedThrustInd.SetDirection(xvector);
	CommandedThrustInd.DefineMeshGroup(VC_GRP_Needle_P1_02);

	MainPanelVC.AddSwitch(&MainFuelTempInd);
	MainFuelTempInd.SetDirection(xvector);
	MainFuelTempInd.DefineMeshGroup(VC_GRP_Needle_P1_03);

	MainPanelVC.AddSwitch(&MainOxidizerTempInd);
	MainOxidizerTempInd.SetDirection(xvector);
	MainOxidizerTempInd.DefineMeshGroup(VC_GRP_Needle_P1_04);

	MainPanelVC.AddSwitch(&MainFuelPressInd);
	MainFuelPressInd.SetDirection(xvector);
	MainFuelPressInd.DefineMeshGroup(VC_GRP_Needle_P1_05);

	MainPanelVC.AddSwitch(&MainOxidizerPressInd);
	MainOxidizerPressInd.SetDirection(xvector);
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

	xvector = { 0.00, 0.075*cos(P1_TILT), 0.075*sin(P1_TILT) };
	VECTOR3 yvector = { 0.075, 0, 0 };
	crossPointerLeft.SetDirection(xvector, yvector);
	crossPointerLeft.DefineMeshGroup(VC_GRP_XpointerX_cdr, VC_GRP_XpointerY_cdr);

	crossPointerRight.SetDirection(xvector, yvector);
	crossPointerRight.DefineMeshGroup(VC_GRP_XpointerX_lmp, VC_GRP_XpointerY_lmp);

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

	xvector = _V(0.0, 0.0625*cos(P2_TILT), 0.0625*sin(P2_TILT));

	MainPanelVC.AddSwitch(&LMRCSATempInd);
	LMRCSATempInd.SetDirection(xvector);
	LMRCSATempInd.DefineMeshGroup(VC_GRP_Needle_P2_01);

	MainPanelVC.AddSwitch(&LMRCSBTempInd);
	LMRCSBTempInd.SetDirection(xvector);
	LMRCSBTempInd.DefineMeshGroup(VC_GRP_Needle_P2_02);

	MainPanelVC.AddSwitch(&LMRCSAPressInd);
	LMRCSAPressInd.SetDirection(xvector);
	LMRCSAPressInd.DefineMeshGroup(VC_GRP_Needle_P2_03);

	MainPanelVC.AddSwitch(&LMRCSBPressInd);
	LMRCSBPressInd.SetDirection(xvector);
	LMRCSBPressInd.DefineMeshGroup(VC_GRP_Needle_P2_04);

	MainPanelVC.AddSwitch(&LMRCSAQtyInd);
	LMRCSAQtyInd.SetDirection(xvector);
	LMRCSAQtyInd.DefineMeshGroup(VC_GRP_Needle_P2_05);

	MainPanelVC.AddSwitch(&LMRCSBQtyInd);
	LMRCSBQtyInd.SetDirection(xvector);
	LMRCSBQtyInd.DefineMeshGroup(VC_GRP_Needle_P2_06);

	MainPanelVC.AddSwitch(&LMSuitTempMeter);
	LMSuitTempMeter.SetDirection(xvector);
	LMSuitTempMeter.DefineMeshGroup(VC_GRP_Needle_P2_07);

	MainPanelVC.AddSwitch(&LMCabinTempMeter);
	LMCabinTempMeter.SetDirection(xvector);
	LMCabinTempMeter.DefineMeshGroup(VC_GRP_Needle_P2_08);

	MainPanelVC.AddSwitch(&LMSuitPressMeter);
	LMSuitPressMeter.SetDirection(xvector);
	LMSuitPressMeter.DefineMeshGroup(VC_GRP_Needle_P2_09);

	MainPanelVC.AddSwitch(&LMCabinPressMeter);
	LMCabinPressMeter.SetDirection(xvector);
	LMCabinPressMeter.DefineMeshGroup(VC_GRP_Needle_P2_10);

	MainPanelVC.AddSwitch(&LMGlycolTempMeter);
	LMGlycolTempMeter.SetDirection(xvector);
	LMGlycolTempMeter.DefineMeshGroup(VC_GRP_Needle_P2_11);

	MainPanelVC.AddSwitch(&LMGlycolPressMeter);
	LMGlycolPressMeter.SetDirection(xvector);
	LMGlycolPressMeter.DefineMeshGroup(VC_GRP_Needle_P2_12);

	MainPanelVC.AddSwitch(&LMOxygenQtyMeter);
	LMOxygenQtyMeter.SetDirection(xvector);
	LMOxygenQtyMeter.DefineMeshGroup(VC_GRP_Needle_P2_13);

	MainPanelVC.AddSwitch(&LMWaterQtyMeter);
	LMWaterQtyMeter.SetDirection(xvector);
	LMWaterQtyMeter.DefineMeshGroup(VC_GRP_Needle_P2_14);

	MainPanelVC.AddSwitch(&LMCO2Meter);
	LMCO2Meter.SetDirection(xvector);
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

	MainPanelVC.AddSwitch(&TempMonitorInd);
	TempMonitorInd.SetDirection(_V(0.00, 0.0625*cos(P3_TILT), 0.0625*sin(P3_TILT)));
	TempMonitorInd.DefineMeshGroup(VC_GRP_Needle_P3_01);

	MainPanelVC.AddSwitch(&RadarSignalStrengthMeter);
	RadarSignalStrengthMeter.SetReference(_V(-0.264141, 0.235696, 1.62835), P3_ROT_AXIS);
	RadarSignalStrengthMeter.DefineMeshGroup(VC_GRP_Needle_Radar);

	MainPanelVC.AddSwitch(&RadarSlewSwitch, AID_VC_RR_SLEW_SWITCH);
	RadarSlewSwitch.SetReference(_V(-0.264179, 0.149389, 1.56749));
	RadarSlewSwitch.SetDirection(_V(-0.00, -sin(P3_TILT + (90.0 * RAD)), cos(P3_TILT + (90.0 * RAD))), _V(1, 0, 0));
	RadarSlewSwitch.DefineMeshGroup(VC_GRP_RR_slew_switch);

	//Panel 4
	MainPanelVC.AddSwitch(&DskySwitchVerb, AID_VC_PUSHB_P4_01);
	MainPanelVC.AddSwitch(&DskySwitchNoun, AID_VC_PUSHB_P4_02);
	MainPanelVC.AddSwitch(&DskySwitchPlus, AID_VC_PUSHB_P4_03);
	MainPanelVC.AddSwitch(&DskySwitchMinus, AID_VC_PUSHB_P4_04);
	MainPanelVC.AddSwitch(&DskySwitchZero, AID_VC_PUSHB_P4_05);
	MainPanelVC.AddSwitch(&DskySwitchOne, AID_VC_PUSHB_P4_06);
	MainPanelVC.AddSwitch(&DskySwitchTwo, AID_VC_PUSHB_P4_07);
	MainPanelVC.AddSwitch(&DskySwitchThree, AID_VC_PUSHB_P4_08);
	MainPanelVC.AddSwitch(&DskySwitchFour, AID_VC_PUSHB_P4_09);
	MainPanelVC.AddSwitch(&DskySwitchFive, AID_VC_PUSHB_P4_10);
	MainPanelVC.AddSwitch(&DskySwitchSix, AID_VC_PUSHB_P4_11);
	MainPanelVC.AddSwitch(&DskySwitchSeven, AID_VC_PUSHB_P4_12);
	MainPanelVC.AddSwitch(&DskySwitchEight, AID_VC_PUSHB_P4_13);
	MainPanelVC.AddSwitch(&DskySwitchNine, AID_VC_PUSHB_P4_14);
	MainPanelVC.AddSwitch(&DskySwitchClear, AID_VC_PUSHB_P4_15);
	MainPanelVC.AddSwitch(&DskySwitchProg, AID_VC_PUSHB_P4_16);
	MainPanelVC.AddSwitch(&DskySwitchKeyRel, AID_VC_PUSHB_P4_17);
	MainPanelVC.AddSwitch(&DskySwitchEnter, AID_VC_PUSHB_P4_18);
	MainPanelVC.AddSwitch(&DskySwitchReset, AID_VC_PUSHB_P4_19);

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

	MainPanelVC.AddSwitch(&LtgAnunNumKnob, AID_VC_ROT_P5_02);
	LtgAnunNumKnob.SetReference(P5_ROT_POS[1], P5_ROT_AXIS);
	LtgAnunNumKnob.DefineMeshGroup(VC_GRP_Rot_P5_02);

	MainPanelVC.AddSwitch(&LtgIntegralKnob, AID_VC_ROT_P5_03);
	LtgIntegralKnob.SetReference(P5_ROT_POS[2], P5_ROT_AXIS);
	LtgIntegralKnob.DefineMeshGroup(VC_GRP_Rot_P5_03);

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

	MainPanelVC.AddSwitch(&DedaSwitchPlus, AID_VC_PUSHB_P6_01);
	MainPanelVC.AddSwitch(&DedaSwitchMinus, AID_VC_PUSHB_P6_02);
	MainPanelVC.AddSwitch(&DedaSwitchZero, AID_VC_PUSHB_P6_03);
	MainPanelVC.AddSwitch(&DedaSwitchOne, AID_VC_PUSHB_P6_04);
	MainPanelVC.AddSwitch(&DedaSwitchTwo, AID_VC_PUSHB_P6_05);
	MainPanelVC.AddSwitch(&DedaSwitchThree, AID_VC_PUSHB_P6_06);
	MainPanelVC.AddSwitch(&DedaSwitchFour, AID_VC_PUSHB_P6_07);
	MainPanelVC.AddSwitch(&DedaSwitchFive, AID_VC_PUSHB_P6_08);
	MainPanelVC.AddSwitch(&DedaSwitchSix, AID_VC_PUSHB_P6_09);
	MainPanelVC.AddSwitch(&DedaSwitchSeven, AID_VC_PUSHB_P6_10);
	MainPanelVC.AddSwitch(&DedaSwitchEight, AID_VC_PUSHB_P6_11);
	MainPanelVC.AddSwitch(&DedaSwitchNine, AID_VC_PUSHB_P6_12);
	MainPanelVC.AddSwitch(&DedaSwitchClear, AID_VC_PUSHB_P6_13);
	MainPanelVC.AddSwitch(&DedaSwitchReadOut, AID_VC_PUSHB_P6_14);
	MainPanelVC.AddSwitch(&DedaSwitchEnter, AID_VC_PUSHB_P6_15);
	MainPanelVC.AddSwitch(&DedaSwitchHold, AID_VC_PUSHB_P6_16);

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

	CircuitBrakerSwitch* breakerrow1[P11R1_CBCOUNT] = { &SE_WND_HTR_AC_CB, &HE_PQGS_PROP_DISP_AC_CB, &SBD_ANT_AC_CB, &ORDEAL_AC_CB, &AGS_AC_CB, &AOT_LAMP_ACB_CB, &LMP_FDAI_AC_CB, &NUM_LTG_AC_CB, &AC_B_INV_1_FEED_CB,
		&AC_B_INV_2_FEED_CB, &AC_A_INV_1_FEED_CB, &AC_A_INV_2_FEED_CB, &AC_A_BUS_VOLT_CB, &CDR_WND_HTR_AC_CB, &TAPE_RCDR_AC_CB, &AOT_LAMP_ACA_CB, &RDZ_RDR_AC_CB, &DECA_GMBL_AC_CB, &INTGL_LTG_AC_CB };

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

	MainPanelVC.AddSwitch(&Panel12AntYawKnob, AID_VC_ROT_P12_02);
	Panel12AntYawKnob.SetReference(P12_ROT_POS[1], P12_ROT_AXIS);
	Panel12AntYawKnob.DefineMeshGroup(VC_GRP_Rot_P12_02);

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

	MainPanelVC.DefineVCAnimations(vcidx);
	crossPointerLeft.DefineVCAnimations(vcidx);
	crossPointerRight.DefineVCAnimations(vcidx);

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

	int lightmat = VC_NMAT - mat_L01 + 12;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, lightmat + m);

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

	oapiSetMaterial(vcmesh, lightmat + m, mat);
}

void LEM::SetContactLight(int m, bool state) {

	if (!vcmesh)
		return;

	int lightmat = VC_NMAT - mat_L01 + 10;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, lightmat + m);

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

	oapiSetMaterial(vcmesh, lightmat + m, mat);
}

void LEM::SetPowerFailureLight(int m, bool state) {

	if (!vcmesh)
		return;

	int lightmat = VC_NMAT - mat_L01 + 1;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, lightmat + m);

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

	oapiSetMaterial(vcmesh, lightmat + m, mat);
}

void LEM::SetStageSeqRelayLight(int m, bool state) {

	if (!vcmesh)
		return;

	int lightmat = VC_NMAT - mat_L01 + 19;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, lightmat + m);

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

	oapiSetMaterial(vcmesh, lightmat + m, mat);
}

void LEM::AnimateFDAI(VECTOR3 attitude, VECTOR3 rates, VECTOR3 errors, UINT animR, UINT animP, UINT animY, UINT errorR, UINT errorP, UINT errorY, UINT rateR, UINT rateP, UINT rateY) {

	double fdai_proc[3];
	double rate_proc[3];

	// Drive FDAI ball
	fdai_proc[0] = -attitude.x / PI2; // 1.0 - attitude.x / PI2;
	fdai_proc[1] = attitude.y / PI2;
	fdai_proc[2] = attitude.z / PI2;
	if (fdai_proc[0] < 0) fdai_proc[0] += 1.0;
	if (fdai_proc[1] < 0) fdai_proc[1] += 1.0;
	if (fdai_proc[2] < 0) fdai_proc[2] += 1.0;
	SetAnimation(animY, fdai_proc[2]);
	SetAnimation(animR, fdai_proc[0]);
	SetAnimation(animP, fdai_proc[1]);

	// Drive error needles
	SetAnimation(errorR, (errors.x + 46) / 92);
	SetAnimation(errorP, (-errors.y + 46) / 92);
	SetAnimation(errorY, (errors.z + 46) / 92);

	// Drive rate needles
	rate_proc[0] = (rates.z + 1) / 2;
	rate_proc[1] = (rates.x + 1) / 2;
	rate_proc[2] = (-rates.y + 1) / 2;
	if (rate_proc[0] < 0) rate_proc[0] = 0;
	if (rate_proc[1] < 0) rate_proc[1] = 0;
	if (rate_proc[2] < 0) rate_proc[2] = 0;
	if (rate_proc[0] > 1) rate_proc[0] = 1;
	if (rate_proc[1] > 1) rate_proc[1] = 1;
	if (rate_proc[2] > 1) rate_proc[1] = 1;
	SetAnimation(rateR, rate_proc[0]);
	SetAnimation(rateP, rate_proc[1]);
	SetAnimation(rateY, rate_proc[2]);
}

