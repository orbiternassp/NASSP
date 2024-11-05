/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Saturn 5
  Virtual cockpit code

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
#include <stdio.h>
#include <math.h>
#include "soundlib.h"
#include "resource.h"

#include "nasspdefs.h"
#include "nassputils.h"

#ifdef _OPENORBITER
#include <gcCoreAPI.h>
#else
#include <gcConst.h>
#endif

#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "CSMcomputer.h"
#include "saturn.h"
#include "saturnv.h"
#include "tracer.h"
#include "papi.h"
#include "CM_VC_Resource.h"
#include "CM-VC-SeatsFolded_Resource.h"
#include "CM-VC-SeatsUnfolded_Resource.h"
#include "EmissionListCMVC.h"


// ==============================================================
// VC Constants
// ==============================================================

// Panel tilt
const double P1_3_TILT = 18.4 * RAD;

// Number of switches on each panel
const int	P1_SWITCHCOUNT = 49;
const int	P2_SWITCHCOUNT = 90;
const int	P3_SWITCHCOUNT = 64;
const int	P4_SWITCHCOUNT = 5;
const int	P5_SWITCHCOUNT = 10;
const int	P6_SWITCHCOUNT = 8;
const int	P7_SWITCHCOUNT = 6;
const int	P8_SWITCHCOUNT = 25;
const int	P9_SWITCHCOUNT = 9;
const int	P10_SWITCHCOUNT = 8;
const int	P13_SWITCHCOUNT = 6;
const int	P15_SWITCHCOUNT = 5;
const int	P16_SWITCHCOUNT = 3;
const int	P100_SWITCHCOUNT = 6;
const int	P101_SWITCHCOUNT = 4;
const int	P122_SWITCHCOUNT = 7;
const int	P306_SWITCHCOUNT = 8;

// Number of push buttons
const int   P1_PUSHBCOUNT = 11;
const int   P2_PUSHBCOUNT = 19;
const int   P122_PUSHBCOUNT = 19;
const int   LEB_L_PUSHBCOUNT = 4;

// Number of switch covers
const int   P1_SWITCHCOVERCOUNT = 15;
const int   P2_SWITCHCOVERCOUNT = 12;

// Number of rotaries
const int	 P1_ROTCOUNT = 1;
const int	 P2_ROTCOUNT = 4;
const int	 P3_ROTCOUNT = 5;
const int	 P4_ROTCOUNT = 1;
const int	 P5_ROTCOUNT = 2;
const int	 P7_ROTCOUNT = 5;
const int	 P8_ROTCOUNT = 3;
const int	 P12_ROTCOUNT = 1;
const int	 P100_ROTCOUNT = 3;
const int	 P101_ROTCOUNT = 2;
const int	 P326_ROTCOUNT = 6;
const int	 LEB_R_ROTCOUNT = 3;
const int	 LEB_L_ROTCOUNT = 22;

// Number of thumbwheels
const int	 P1_TWCOUNT = 2;
const int	 P2_TWCOUNT = 1;
const int	 P3_TWCOUNT = 2;
const int	 P6_TWCOUNT = 6;
const int	 P9_TWCOUNT = 6;
const int	 P10_TWCOUNT = 6;

// Number of needles
const int	 P1_NEEDLECOUNT = 5;

// Number of circuit breakers
const int P4_CBCOUNT = 12;
const int P5_CBCOUNT = 69;
const int P8_CBCOUNT = 67;
const int LEB_R1_CBCOUNT = 81;
const int LEB_R2_CBCOUNT = 31;

// Clickspot radius
const double SWITCH = 0.015;
const double PUSHB = 0.008;
const double ROT = 0.02;
const double CB = 0.008;
const double TW = 0.01;

// Switch clickspot offset
const VECTOR3	P1_3_CLICK = { 0.00, 0.009*cos(P1_3_TILT - (90.0 * RAD)), 0.009*sin(P1_3_TILT - (90.0 * RAD)) };
const VECTOR3	P1_3_TWCLICK = { 0.00, 0.024*cos(P1_3_TILT - (90.0 * RAD)), 0.024*sin(P1_3_TILT - (90.0 * RAD)) };
const VECTOR3	P6_TWCLICK = { -0.016793, -0.010398, -0.013661 };
const VECTOR3	P9_TWCLICK = { 0.017689, -0.011461, -0.012926 };
const VECTOR3	P10_TWCLICK = { 0.000035, -0.021956, -0.009651 };
const VECTOR3	P4_CLICK = { -0.00442, 0.008318, -0.002538 };
const VECTOR3	P6_CLICK = { -0.006293, -0.003896, -0.005119 };
const VECTOR3	P5_CLICK = { -0.00828, 0.00004, -0.005173 };
const VECTOR3	P7_CLICK = { 0.00442, 0.008327, -0.002499 };
const VECTOR3	P8_CLICK = { 0.00826, 0.00008, -0.005196 };
const VECTOR3	P9_CLICK = { 0.006978, -0.004521, -0.005098 };
const VECTOR3	P10_CLICK = { 0.000013, -0.008239, -0.003621 };
const VECTOR3	P13_CLICK = { 0.005371, -0.00622, -0.005259 };
const VECTOR3	P15_CLICK = { 0.005634, -0.006241, -0.004942 };
const VECTOR3	P16_CLICK = { -0.006025, -0.005422, -0.005441 };
const VECTOR3	P101_CLICK = { 0, 0.938288430820701 * 0.009, -0.345853756070434 * 0.009 };
const VECTOR3	P306_CLICK = { 0.009, 0, 0 };
const VECTOR3	P325_CLICK = { 0, 0, 0.07 };
const VECTOR3	P600_CLICK = { 0, -0.009, 0 };
const VECTOR3	LEBFLOOR_CLICK = { 0, 0.009, 0 };
const VECTOR3	LEBLEFT_CLICK = { 0.009, 0, 0 };

// Rotary/Needle rotation axises
const VECTOR3	P1_3_ROT_AXIS = { 0.00, sin(P1_3_TILT),-cos(P1_3_TILT) };
const VECTOR3	P1_3_ROT_AXIS_NEG = { 0.00, -sin(P1_3_TILT),cos(P1_3_TILT) };

// Pushbutton vectors
const VECTOR3	P1_3_PB_VECT = { 0.00, 0.001*cos(P1_3_TILT + (90.0 * RAD)), 0.001*sin(P1_3_TILT + (90.0 * RAD)) };

// Panel 1 switches
const VECTOR3 P1_TOGGLE_POS[P1_SWITCHCOUNT] = {
{-0.7016, 0.7637, 0.3847}, {-0.8518, 0.6303, 0.3430}, {-0.8983, 0.5705, 0.3200}, {-0.8724, 0.5705, 0.3200}, {-0.8132, 0.5705, 0.3200},
{-0.7873, 0.5705, 0.3200}, {-0.9430, 0.5160, 0.3017}, {-0.9171, 0.5160, 0.3017}, {-0.8914, 0.5160, 0.3017}, {-0.8656, 0.5160, 0.3017},
{-0.8399, 0.5160, 0.3017}, {-0.8141, 0.5160, 0.3017}, {-0.7885, 0.5160, 0.3017}, {-0.9943, 0.4533, 0.2807}, {-0.9588, 0.4533, 0.2807},
{-0.9237, 0.4533, 0.2807}, {-0.8826, 0.4533, 0.2807}, {-0.8273, 0.4533, 0.2807}, {-0.8014, 0.4533, 0.2807}, {-0.9816, 0.4031, 0.2640},
{-0.9529, 0.4031, 0.2640}, {-0.9224, 0.4031, 0.2640}, {-0.7916, 0.3850, 0.2580}, {-0.7342, 0.3850, 0.2580}, {-0.8293, 0.3254, 0.2379},
{-0.8035, 0.3254, 0.2379}, {-0.7738, 0.3254, 0.2379}, {-0.7446, 0.3254, 0.2379}, {-0.7151, 0.3254, 0.2379}, {-0.6855, 0.3254, 0.2379},
{-0.6361, 0.3254, 0.2379}, {-0.6097, 0.3254, 0.2379}, {-0.5832, 0.3254, 0.2379}, {-0.5573, 0.3254, 0.2379}, {-0.5315, 0.3254, 0.2379},
{-0.5042, 0.3254, 0.2379}, {-0.8827, 0.2762, 0.2214}, {-0.8556, 0.2762, 0.2214}, {-0.8296, 0.2762, 0.2214}, {-0.8035, 0.2762, 0.2214},
{-0.7778, 0.2762, 0.2214}, {-0.7521, 0.2762, 0.2214}, {-0.7264, 0.2762, 0.2214}, {-0.5853, 0.2762, 0.2214}, {-0.5591, 0.2762, 0.2214},
{-0.5339, 0.2762, 0.2214},{-0.5077, 0.2762, 0.2214}, {-0.508397, 0.798359, 0.406069}, {-0.8767, 0.3880, 0.2590}
};

// Panel 1 rotaries
const VECTOR3 P1_ROT_POS[P1_ROTCOUNT] = {
//	{-0.6834, 0.8195, 0.3925}
	{-0.6835, 0.8185, 0.3952}
};

// Panel 1 push-buttons
const VECTOR3 P1_PUSHB_POS[P1_PUSHBCOUNT] = {
	{-0.5622, 0.4689, 0.2789}, {-0.5623, 0.4425, 0.2701}, {-0.5623, 0.4163, 0.2613}, {-0.5623, 0.3901, 0.2526}, {-0.4982, 0.4687, 0.2788},
	{-0.4981, 0.4425, 0.2701}, {-0.4981, 0.4163, 0.2613}, {-0.4982, 0.3903, 0.2526}, {-0.8339, 0.4017, 0.2594}, {-0.8339, 0.3716, 0.2493},
	{-0.8819, 0.3268, 0.2343}
};

// Panel 1 thumbwheels
const VECTOR3 P1_TW_POS[P1_TWCOUNT] = {
	{-0.669255, 0.382236, 0.280385}, {-0.613206, 0.382088, 0.280445}
};

// Panel 2 switches
const VECTOR3 P2_TOGGLE_POS[P2_SWITCHCOUNT] = {
{-0.4057, 0.4478, 0.2790}, {-0.3799, 0.4478, 0.2790}, {-0.3533, 0.4478, 0.2790}, {-0.3271, 0.4478, 0.2790}, {-0.3007, 0.4478, 0.2790},
{-0.2745, 0.4478, 0.2790}, {-0.4323, 0.3955, 0.2615}, {-0.4029, 0.3955, 0.2615}, {-0.3668, 0.3955, 0.2615}, {-0.3324, 0.3955, 0.2615},
{-0.3017, 0.3955, 0.2615}, {-0.4313, 0.3325, 0.2403}, {-0.4027, 0.3325, 0.2403}, {-0.3743, 0.3325, 0.2403}, {-0.4321, 0.2813, 0.2232},
{-0.2246, 0.8341, 0.4082}, {-0.1988, 0.8341, 0.4082}, {-0.1731, 0.8341, 0.4082}, {-0.1473, 0.8341, 0.4082}, {-0.1131, 0.8341, 0.4082},
{-0.0808, 0.8341, 0.4082}, {-0.0430, 0.8341, 0.4082}, {-0.1072, 0.7482, 0.3795}, {-0.0814, 0.7482, 0.3795}, {-0.0556, 0.7482, 0.3795},
{-0.0299, 0.7482, 0.3795}, {-0.2115, 0.6588, 0.3496}, {-0.1854, 0.6588, 0.3496}, {-0.1594, 0.6588, 0.3496}, {-0.1330, 0.6588, 0.3496},
{-0.1071, 0.6588, 0.3496}, {-0.0813, 0.6588, 0.3496}, {-0.0557, 0.6588, 0.3496}, {-0.0298, 0.6588, 0.3496}, {-0.2103, 0.5845, 0.3247},
{-0.1845, 0.5845, 0.3247}, {-0.1587, 0.5845, 0.3247}, {-0.1330, 0.5845, 0.3247}, {-0.1072, 0.5845, 0.3247}, {-0.0814, 0.5845, 0.3247},
{-0.0557, 0.5845, 0.3247}, {-0.0299, 0.5845, 0.3247}, {-0.2103, 0.5106, 0.3000}, {-0.1846, 0.5106, 0.3000}, {-0.1588, 0.5106, 0.3000},
{-0.1330, 0.5106, 0.3000}, {-0.1072, 0.5106, 0.3000}, {-0.0814, 0.5106, 0.3000}, {-0.0556, 0.5106, 0.3000}, {-0.0299, 0.5106, 0.3000},
{0.1531, 0.8225, 0.4044}, {0.1788, 0.8225, 0.4044}, {0.2046, 0.8225, 0.4044}, {0.0907, 0.7652, 0.3852}, {0.1194, 0.7652, 0.3852},
{0.1488, 0.7652, 0.3852}, {0.1746, 0.7652, 0.3852}, {0.2004, 0.7652, 0.3852}, {0.0889, 0.7141, 0.3681}, {0.1243, 0.7141, 0.3681},
{0.1572, 0.7141, 0.3681}, {0.1830, 0.7141, 0.3681}, {0.2088, 0.7141, 0.3681}, {0.2388, 0.7141, 0.3681}, {0.2646, 0.7141, 0.3681},
{0.2982, 0.7141, 0.3681}, {0.3366, 0.7141, 0.3681}, {0.3755, 0.7141, 0.3681}, {0.4133, 0.7141, 0.3681}, {0.0193, 0.5702, 0.3200},
{0.0493, 0.5702, 0.3200}, {0.0793, 0.5702, 0.3200}, {0.1093, 0.5702, 0.3200}, {0.1351, 0.5702, 0.3200}, {0.0139, 0.5104, 0.2999},
{0.0397, 0.5104, 0.2999}, {0.0655, 0.5104, 0.2999}, {0.0912, 0.5104, 0.2999}, {0.1171, 0.5104, 0.2999}, {0.1465, 0.5104, 0.2999},
{0.1758, 0.5104, 0.2999}, {0.2016, 0.5104, 0.2999}, {0.2322, 0.5104, 0.2999}, {0.2604, 0.5104, 0.2999}, {0.2874, 0.5104, 0.2999},
{0.3173, 0.5104, 0.2999}, {0.2526, 0.4565, 0.2819}, {0.2784, 0.4565, 0.2819}, {0.3701, 0.3348, 0.2411}, {0.3959, 0.3348, 0.2411}
};

// Panel 2 push-buttons
const VECTOR3 P2_PUSHB_POS[P2_PUSHBCOUNT] = {
	{-0.4100, 0.5333, 0.3029}, {-0.4101, 0.5117, 0.2957}, {-0.3850, 0.5444, 0.3066}, {-0.3850, 0.5221, 0.2991}, {-0.3850, 0.5000, 0.2917},
	{-0.3614, 0.5001, 0.2918}, {-0.3367, 0.5002, 0.2918}, {-0.3120, 0.5003, 0.2918}, {-0.3615, 0.5222, 0.2992}, {-0.3367, 0.5223, 0.2992},
	{-0.3120, 0.5224, 0.2992}, {-0.3615, 0.5444, 0.3066}, {-0.3368, 0.5444, 0.3066}, {-0.3121, 0.5444, 0.3066}, {-0.2884, 0.5444, 0.3066},
	{-0.2883, 0.5224, 0.2993}, {-0.2881, 0.5003, 0.2919}, {-0.2632, 0.5330, 0.3028}, {-0.2630, 0.5113, 0.2955}
};

// Panel 2 rotaries
const VECTOR3 P2_ROT_POS[P2_ROTCOUNT] = {
//	{0.0297, 0.7256, 0.3614}, { 0.0297, 0.6477, 0.3354 }, { 0.3195, 0.4008, 0.2527 }, { 0.3974, 0.4010, 0.2527 }
	{0.0297, 0.7245, 0.3640}, { 0.0297, 0.6467, 0.3379 }, { 0.3195, 0.3998, 0.2553 }, { 0.3974, 0.4000, 0.2553 }
};

// Panel 2 thumbwheels
const VECTOR3 P2_TW_POS[P2_TWCOUNT] = {
	{0.4012, 0.5060, 0.3160}
};

// Panel 3 switches
const VECTOR3 P3_TOGGLE_POS[P3_SWITCHCOUNT] = {
{0.6307, 0.6477, 0.3458}, {0.6564, 0.6477, 0.3458}, {0.6823, 0.6477, 0.3458}, {0.5695, 0.5877, 0.3287}, {0.6312, 0.5977, 0.3291},
{0.6570, 0.5977, 0.3291}, {0.6828, 0.5977, 0.3291}, {0.5251, 0.5383, 0.3122}, {0.5509, 0.5383, 0.3122}, {0.5766, 0.5383, 0.3122},
{0.6300, 0.5283, 0.3059}, {0.6558, 0.5283, 0.3059}, {0.6816, 0.5283, 0.3059}, {0.7074, 0.5283, 0.3059}, {0.7374, 0.5283, 0.3059},
{0.7679, 0.5283, 0.3059}, {0.7938, 0.5283, 0.3059}, {0.4970, 0.4498, 0.2796}, {0.5227, 0.4498, 0.2796}, {0.5485, 0.4498, 0.2796},
{0.5953, 0.4498, 0.2796}, {0.6211, 0.4498, 0.2796}, {0.6469, 0.4498, 0.2796}, {0.6727, 0.4498, 0.2796}, {0.6984, 0.4498, 0.2796},
{0.7320, 0.4498, 0.2796}, {0.7656, 0.4498, 0.2796}, {0.7992, 0.4498, 0.2796}, {0.4969, 0.3958, 0.2616}, {0.5226, 0.3958, 0.2616},
{0.5485, 0.3958, 0.2616}, {0.5839, 0.3958, 0.2616}, {0.6097, 0.3958, 0.2616}, {0.6354, 0.3958, 0.2616}, {0.6672, 0.3958, 0.2616},
{0.6990, 0.3958, 0.2616}, {0.7308, 0.3958, 0.2616}, {0.7625, 0.3958, 0.2616}, {0.4969, 0.3361, 0.2416}, {0.5227, 0.3361, 0.2416},
{0.6438, 0.3378, 0.2421}, {0.6744, 0.3378, 0.2421}, {0.7050, 0.3378, 0.2421}, {0.7308, 0.3378, 0.2421}, {0.7565, 0.3378, 0.2421},
{0.4970, 0.2814, 0.2233}, {0.5227, 0.2814, 0.2233}, {0.6438, 0.2821, 0.2235}, {0.6696, 0.2821, 0.2235}, {0.6954, 0.2821, 0.2235},
{0.7212, 0.2821, 0.2235}, {0.7470, 0.2821, 0.2235}, {0.7727, 0.2821, 0.2235}, {0.8501, 0.3958, 0.2616}, {0.8879, 0.3958, 0.2616},
{0.9256, 0.3958, 0.2616}, {0.8501, 0.3383, 0.2423}, {0.8759, 0.3383, 0.2423}, {0.9016, 0.3383, 0.2423}, {0.9275, 0.3383, 0.2423},
{0.8501, 0.2821, 0.2235}, {0.8759, 0.2821, 0.2235}, {0.9017, 0.2821, 0.2235}, {0.9274, 0.2821, 0.2235}
};

// Panel 3 rotaries
const VECTOR3 P3_ROT_POS[P3_ROTCOUNT] = {
//	{0.5139, 0.8741, 0.4111}, {0.7741, 0.6223, 0.3268}, {0.8658, 0.4693, 0.2756}, {0.9596, 0.4691, 0.2755}, {0.9894, 0.2936, 0.2169}
	{0.5140, 0.8731, 0.4137}, {0.7741, 0.6212, 0.3294},	{0.8659, 0.4682, 0.2783}, {0.9596, 0.4681, 0.2782},	{0.9895, 0.2925, 0.2195}
};

// Panel 3 thumbwheels
const VECTOR3 P3_TW_POS[P3_TWCOUNT] = {
	{0.5832, 0.3306, 0.2629}, {0.5832, 0.2747, 0.2444}
};

// Panel 4 switches
const VECTOR3 P4_TOGGLE_POS[P4_SWITCHCOUNT] = {
{1.1509, 0.2085, 0.0020}, {1.1266, 0.1798, -0.0481}, {1.1787, 0.2074, -0.0480}, {1.1817, 0.1880, -0.1142}, {1.2203, 0.2086, -0.1140}
};

// Panel 4 rotaries
const VECTOR3 P4_ROT_POS[P4_ROTCOUNT] = {
//	{1.1305, 0.1670, -0.1088}
	{1.1316, 0.1646, -0.1081}
};

// Panel 4 circuit breakers
const VECTOR3 P4_CB_POS[P4_CBCOUNT] = {
{1.1127, 0.1476, -0.1605}, {1.1333, 0.1585, -0.1602}, {1.1541, 0.1695, -0.1600}, {1.1858, 0.1863, -0.1603}, {1.2064, 0.1973, -0.1600},
{1.2271, 0.2083, -0.1598}, {1.1251, 0.1420, -0.1984}, {1.1465, 0.1533, -0.1985}, {1.1679, 0.1647, -0.1985}, {1.1890, 0.1759, -0.1980},
{1.2101, 0.1870, -0.1982}, {1.2311, 0.1982, -0.1979}
};

// Panel 5 switches
const VECTOR3 P5_TOGGLE_POS[P5_SWITCHCOUNT] = {
{1.0635, 0.4726, 0.2254}, {1.0830, 0.4807, 0.1946}, {1.1024, 0.4888, 0.1636}, {1.1218, 0.4970, 0.1328}, {1.1525, 0.5098, 0.0844},
{1.1660, 0.5155, 0.0631}, {1.1794, 0.5212, 0.0417}, {1.1988, 0.5293, 0.0108}, {1.1492, 0.4365, 0.0895}, {1.1627, 0.4422, 0.0682}
};

// Panel 5 rotaries
const VECTOR3 P5_ROT_POS[P5_ROTCOUNT] = {
//	{1.0745, 0.4076, 0.1899}, {1.1143, 0.4244, 0.1267}
	{1.0769, 0.4076, 0.1914}, {1.1168, 0.4243, 0.1282}
};

// Panel 5 circuit breakers
const VECTOR3 P5_CB_POS[P5_CBCOUNT] = {
{1.2131, 0.5463, -0.0392}, {1.2243, 0.5510, -0.0572}, {1.2447, 0.5597, -0.0896}, {1.2749, 0.5723, -0.1374}, {1.2950, 0.5808, -0.1693},
{1.3064, 0.5855, -0.1872}, {1.3179, 0.5903, -0.2054}, {1.2181, 0.5024, -0.0474}, {1.2296, 0.5074, -0.0655}, {1.2410, 0.5122, -0.0836},
{1.2575, 0.5189, -0.1098}, {1.2686, 0.5237, -0.1273}, {1.2796, 0.5283, -0.1448}, {1.2907, 0.5330, -0.1624}, {1.3018, 0.5377, -0.1801},
{1.3129, 0.5423, -0.1974}, {1.3240, 0.5470, -0.2151}, {1.1709, 0.4335, 0.0273}, {1.1824, 0.4385, 0.0091}, {1.1938, 0.4434, -0.0089},
{1.2226, 0.4553, -0.0546}, {1.2337, 0.4601, -0.0721}, {1.2447, 0.4647, -0.0896}, {1.2558, 0.4694, -0.1072}, {1.2669, 0.4741, -0.1249},
{1.2780, 0.4787, -0.1422}, {1.2891, 0.4834, -0.1599}, {1.3134, 0.4935, -0.1984}, {1.0670, 0.3376, 0.1924}, {1.0782, 0.3423, 0.1744},
{1.1066, 0.3542, 0.1293}, {1.1180, 0.3590, 0.1112}, {1.1294, 0.3639, 0.0933}, {1.1408, 0.3687, 0.0752}, {1.1579, 0.3757, 0.0480},
{1.1693, 0.3805, 0.0299}, {1.1807, 0.3854, 0.0120}, {1.1921, 0.3902, -0.0061}, {1.2363, 0.4087, -0.0764}, {1.2477, 0.4135, -0.0944},
{1.2591, 0.4184, -0.1124}, {1.2705, 0.4232, -0.1305}, {1.2871, 0.4300, -0.1569}, {1.2985, 0.4349, -0.1750}, {1.3099, 0.4397, -0.1930},
{1.3213, 0.4445, -0.2110}, {1.0864, 0.2956, 0.1615}, {1.1168, 0.3082, 0.1133}, {1.1420, 0.3187, 0.0734}, {1.1535, 0.3237, 0.0552},
{1.1649, 0.3285, 0.0372}, {1.1926, 0.3401, -0.0069}, {1.2040, 0.3450, -0.0249}, {1.2616, 0.3692, -0.1164}, {1.2727, 0.3739, -0.1339},
{1.2837, 0.3785, -0.1514}, {1.2949, 0.3833, -0.1690}, {1.3060, 0.3879, -0.1867}, {1.3170, 0.3926, -0.2041}, {1.1010, 0.2534, 0.1383},
{1.1182, 0.2605, 0.1111}, {1.1320, 0.2663, 0.0892}, {1.1431, 0.2711, 0.0716}, {1.1541, 0.2757, 0.0542}, {1.1652, 0.2804, 0.0365},
{1.1763, 0.2851, 0.0189}, {1.1874, 0.2897, 0.0015}, {1.1985, 0.2944, -0.0162}, {1.2096, 0.2990, -0.0338}
};

// Panel 6 switches
const VECTOR3 P6_TOGGLE_POS[P6_SWITCHCOUNT] = {
{0.8417, 0.7281, 0.3304}, {0.8871, 0.6781, 0.3139}, {0.9330, 0.6277, 0.2971}, {0.9047, 0.7671, 0.2239}, {0.9521, 0.7151, 0.2064},
{0.9973, 0.6656, 0.1897}, {1.0243, 0.5847, 0.2194}, {1.0351, 0.6043, 0.1910}
};

// Panel 6 thumbwheels
const VECTOR3 P6_TW_POS[P6_TWCOUNT] = {
{0.8661, 0.7540, 0.3201}, {0.9115, 0.7045, 0.3034}, {0.9575, 0.6543, 0.2864}, {0.9112, 0.7617, 0.2602}, {0.9589, 0.7096, 0.2424},
{1.0038, 0.6602, 0.2258}
};

// Panel 7 switches
const VECTOR3 P7_TOGGLE_POS[P7_SWITCHCOUNT] = {
{-1.1340, 0.2070, 0.0421}, {-1.1548, 0.2001, -0.0147}, {-1.1319, 0.1881, -0.0149}, {-1.1432, 0.1790, -0.0628}, {-1.1545, 0.1690, -0.1137},
{-1.1317, 0.1571, -0.1139}
};

// Panel 7 rotaries
const VECTOR3 P7_ROT_POS[P7_ROTCOUNT] = {
//	{-1.1892, 0.2079, -0.0612}, {-1.2288, 0.2103, -0.1199}, {-1.2709, 0.2117, -0.1857}, {-1.2104, 0.1801, -0.1858}, {-1.1565, 0.1613, -0.1646}
	{-1.1904, 0.2054, -0.0606},	{-1.2300, 0.2078, -0.1191},	{-1.2721, 0.2092, -0.1850},	{-1.2116, 0.1776, -0.1852}, {-1.1565, 0.1613, -0.1646}
};

// Panel 8 switches
const VECTOR3 P8_TOGGLE_POS[P8_SWITCHCOUNT] = {
{-1.2784, 0.5453, -0.1002}, {-1.2647, 0.5400, -0.0787}, {-1.2513, 0.5347, -0.0573}, {-1.2377, 0.5294, -0.0359}, {-1.2242, 0.5240, -0.0146},
{-1.2107, 0.5186, 0.0069}, {-1.1972, 0.5133, 0.0282}, {-1.1836, 0.5080, 0.0496}, {-1.1701, 0.5026, 0.0710}, {-1.1566, 0.4973, 0.0923},
{-1.1431, 0.4919, 0.1137}, {-1.1295, 0.4865, 0.1350}, {-1.1160, 0.4813, 0.1565}, {-1.1025, 0.4759, 0.1778}, {-1.0890, 0.4705, 0.1992},
{-1.0754, 0.4652, 0.2207}, {-1.2178, 0.4169, -0.0045}, {-1.2043, 0.4116, 0.0170}, {-1.1748, 0.3641, 0.0531}, {-1.1575, 0.3571, 0.0807},
{-1.1399, 0.3504, 0.1083}, {-1.1258, 0.3447, 0.1307}, {-1.1138, 0.3400, 0.1496}, {-1.1018, 0.3352, 0.1687}, {-1.0860, 0.3290, 0.1938}
};

// Panel 8 rotaries
const VECTOR3 P8_ROT_POS[P8_ROTCOUNT] = {
//	{-1.1575, 0.4264, 0.0725}, {-1.1217, 0.4119, 0.1292}, {-1.0860, 0.3979, 0.1855}
	{-1.1599, 0.4263, 0.0741}, {-1.1241, 0.4118, 0.1307}, {-1.0884, 0.3978, 0.1870}
};

// Panel 8 circuit breakers
const VECTOR3 P8_CB_POS[P8_CBCOUNT] = {
{-1.3346, 0.5696, -0.2170}, {-1.3188, 0.5633, -0.1922}, {-1.2939, 0.5535, -0.1528}, {-1.3348, 0.5092, -0.2175}, {-1.3233, 0.5047, -0.1993},
{-1.3118, 0.5001, -0.1812}, {-1.3005, 0.4957, -0.1632}, {-1.2890, 0.4910, -0.1453}, {-1.2775, 0.4866, -0.1271}, {-1.2662, 0.4821, -0.1091},
{-1.2546, 0.4774, -0.0909}, {-1.2433, 0.4730, -0.0728}, {-1.2317, 0.4684, -0.0547}, {-1.2204, 0.4641, -0.0368}, {-1.2090, 0.4593, -0.0188},
{-1.1975, 0.4550, -0.0007}, {-1.1862, 0.4505, 0.0174}, {-1.1746, 0.4457, 0.0356}, {-1.3342, 0.4614, -0.2165}, {-1.3227, 0.4569, -0.1984},
{-1.3112, 0.4523, -0.1803}, {-1.2999, 0.4480, -0.1623}, {-1.2884, 0.4432, -0.1443}, {-1.2769, 0.4389, -0.1262}, {-1.2656, 0.4344, -0.1082},
{-1.2540, 0.4296, -0.0900}, {-1.2427, 0.4253, -0.0719}, {-1.2312, 0.4207, -0.0538}, {-1.3330, 0.4146, -0.2146}, {-1.3215, 0.4101, -0.1965},
{-1.3100, 0.4055, -0.1784}, {-1.2987, 0.4012, -0.1604}, {-1.2873, 0.3964, -0.1424}, {-1.2757, 0.3921, -0.1243}, {-1.2644, 0.3876, -0.1063},
{-1.2528, 0.3828, -0.0881}, {-1.2415, 0.3785, -0.0700}, {-1.2300, 0.3739, -0.0519}, {-1.2187, 0.3695, -0.0339}, {-1.2072, 0.3648, -0.0160},
{-1.1957, 0.3604, 0.0021}, {-1.1844, 0.3559, 0.0202}, {-1.3349, 0.3672, -0.2176}, {-1.3234, 0.3627, -0.1994}, {-1.3119, 0.3581, -0.1814},
{-1.3006, 0.3537, -0.1634}, {-1.2891, 0.3490, -0.1454}, {-1.2776, 0.3446, -0.1273}, {-1.2663, 0.3401, -0.1092}, {-1.2547, 0.3354, -0.0911},
{-1.2434, 0.3310, -0.0730}, {-1.2318, 0.3264, -0.0549}, {-1.2205, 0.3221, -0.0369}, {-1.2091, 0.3173, -0.0189}, {-1.1673, 0.3008, 0.0474},
{-1.1558, 0.2962, 0.0655}, {-1.1445, 0.2919, 0.0835}, {-1.1331, 0.2871, 0.1014}, {-1.1215, 0.2828, 0.1195}, {-1.1102, 0.2783, 0.1376},
{-1.0986, 0.2735, 0.1558}, {-1.3237, 0.3158, -0.1999}, {-1.3122, 0.3113, -0.1817}, {-1.3007, 0.3067, -0.1636}, {-1.2894, 0.3023, -0.1456},
{-1.2779, 0.2976, -0.1277}, {-1.2664, 0.2933, -0.1096}
};

// Panel 9 switches
const VECTOR3 P9_TOGGLE_POS[P9_SWITCHCOUNT] = {
{-0.9100, 0.7654, 0.2204}, {-0.9548, 0.7162, 0.2031}, {-1.0003, 0.6658, 0.1858}, {-0.8512, 0.7320, 0.3298}, {-0.8983, 0.6797, 0.3119},
{-0.9431, 0.6301, 0.2949}, {-1.0367, 0.6007, 0.1939}, {-1.0256, 0.5790, 0.2280}, {-1.0146, 0.5572, 0.2621}
};

// Panel 9 thumbwheels
const VECTOR3 P9_TW_POS[P9_TWCOUNT] = {
{-0.9182, 0.7617, 0.2566}, {-0.9631, 0.7120, 0.2397}, {-1.0087, 0.6615, 0.2222}, {-0.8749, 0.7584, 0.3179}, {-0.9222, 0.7061, 0.3001},
{-0.9670, 0.6566, 0.2829}
};

// Panel 10 switches
const VECTOR3 P10_TOGGLE_POS[P10_SWITCHCOUNT] = {
{0.0929, 0.3539, 0.5188}, {0.0928, 0.3791, 0.4605}, {0.0930, 0.4047, 0.4013}, {-0.0028, 0.3791, 0.4605}, {-0.0028, 0.4047, 0.4012},
{-0.0986, 0.3540, 0.5188}, {-0.0985, 0.3791, 0.4605}, {-0.0985, 0.4046, 0.4013}
};

// Panel 10 thumbwheels
const VECTOR3 P10_TW_POS[P10_TWCOUNT] = {
{0.0624, 0.3754, 0.5277}, {0.0623, 0.4002, 0.4695}, {0.0625, 0.4257, 0.4105}, {-0.0684, 0.3754, 0.5277}, {-0.0684, 0.4002, 0.4695},
{-0.0685, 0.4257, 0.4105}
};

// Panel 12 rotaries
const VECTOR3 P12_ROT_POS[P12_ROTCOUNT] = {
{-0.3027, 0.1734, 0.5538}
};

// Panel 13 switches
const VECTOR3 P13_TOGGLE_POS[P13_SWITCHCOUNT] = {
{-0.9838, 1.0715, -0.2288}, {-0.9574, 1.0460, -0.1718}, {-0.9352, 1.0243, -0.1233}, {-1.0234, 1.0360, -0.2263}, {-0.9864, 0.9999, -0.1455},
{-0.9749, 0.9889, -0.1208}
};

// Panel 15 switches
const VECTOR3 P15_TOGGLE_POS[P15_SWITCHCOUNT] = {
{-0.8312, 0.9137, 0.2083}, {-0.8075, 0.8896, 0.2649}, {-0.7980, 0.8802, 0.2871}, {-0.7880, 0.8700, 0.3111}, {-0.7708, 0.8527, 0.3520}
};

// Panel 16 switches
const VECTOR3 P16_TOGGLE_POS[P16_SWITCHCOUNT] = {
{0.7430, 0.8487, 0.3613}, {0.7878, 0.8822, 0.2766}, {0.8170, 0.9039, 0.2215}
};

// Panel 100 switches
const VECTOR3 P100_TOGGLE_POS[P100_SWITCHCOUNT] = {
{-0.7336, -0.6991, 0.2216}, {-0.7050, -0.6991, 0.2216}, {-0.6659, -0.6991, 0.2216}, {-0.6138, -0.6991, 0.2216}, {-0.5778, -0.6991, 0.2216},
{-0.5436, -0.6991, 0.2216}
};

// Panel 100 rotaries
const VECTOR3 P100_ROT_POS[P100_ROTCOUNT] = {
//{-0.7232, -0.6952, 0.1361}, {-0.6414, -0.6952, 0.1361}, {-0.5711, -0.6952, 0.1361}
{-0.7232, -0.6982, 0.1359}, {-0.6413, -0.6982, 0.1360}, {-0.5711, -0.6982, 0.1359}
};

// Panel 101 switches
const VECTOR3 P101_TOGGLE_POS[P101_SWITCHCOUNT] = {
{-0.3969, -0.6484, 0.4125}, {-0.4717, -0.6764, 0.3361}, {-0.4250, -0.6764, 0.3361}, {-0.3975, -0.6764, 0.3361}
};

// Panel 101 rotaries
const VECTOR3 P101_ROT_POS[P101_ROTCOUNT] = {
//{-0.4692, -0.6228, 0.4710}, {-0.3974, -0.6228, 0.4710}
{-0.4692, -0.6256, 0.4719}, {-0.3974, -0.6256, 0.4719}
};

// Panel 122 switches
const VECTOR3 P122_TOGGLE_POS[P122_SWITCHCOUNT] = {
{-0.3150, -0.7130, 0.0791}, {-0.2414, -0.7130, 0.0792}, {-0.1800, -0.7130, 0.0791}, {-0.2413, -0.7130, 0.0009}, {-0.1801, -0.7130, 0.0009},
{0.1715, -0.7130, 0.0709}, {0.1716, -0.7130, -0.0079}
};

// Panel 122 push-buttons
const VECTOR3 P122_PUSHB_POS[P122_PUSHBCOUNT] = {
{0.2894, -0.6997, 0.0963}, {0.2894, -0.6997, 0.0741}, {0.3122, -0.6997, 0.1073}, {0.3122, -0.6997, 0.0853}, {0.3122, -0.6997, 0.0630},
{0.3349, -0.6997, 0.0630}, {0.3576, -0.6998, 0.0630}, {0.3805, -0.6998, 0.0630}, {0.3349, -0.6997, 0.0853}, {0.3576, -0.6998, 0.0853},
{0.3805, -0.6998, 0.0853}, {0.3349, -0.6997, 0.1073}, {0.3576, -0.6998, 0.1073}, {0.3805, -0.6998, 0.1073}, {0.4032, -0.6998, 0.1073},
{0.4032, -0.6998, 0.0853}, {0.4032, -0.6998, 0.0630}, {0.4260, -0.6998, 0.0961}, {0.4260, -0.6998, 0.0739}
};

// Panel 306 switches
const VECTOR3 P306_TOGGLE_POS[P306_SWITCHCOUNT] = {
{-0.5233, -0.5086, 0.3536}, {-0.5233, -0.5293, 0.3536}, {-0.5233, -0.5498, 0.3536}, {-0.5233, -0.5704, 0.3536}, {-0.5233, -0.5910, 0.3536},
{-0.5233, -0.6116, 0.3536}, {-0.5233, -0.6323, 0.3536}, {-0.5226, -0.6023, 0.4065}
};

// Panel 325 Levers
const VECTOR3 Cab_Press_Rel_Handle1Location = { -1.2149, 0.7847, -0.2850 };
const VECTOR3 Cab_Press_Rel_Handle2Location = { -1.2024, 0.6799, -0.2992 };

// Panel 326 rotaries
const VECTOR3 P326_ROT_POS[P326_ROTCOUNT] = {
{-1.1659, 0.5990, -0.2336}, {-1.1659, 0.5355, -0.2336}, {-1.1659, 0.4719, -0.2336}, {-1.2942, 0.3346, -0.2336}, {-1.2305, 0.3346, -0.2336},
{-1.1670, 0.3346, -0.2336}
};

// LEB Right Rotaries

const VECTOR3 LEB_R_ROT_POS[LEB_R_ROTCOUNT] = {
	{1.0811, -0.5597, -0.4702}, {1.0811, -0.5536, -0.6556}, {1.0811, -0.4765, -0.6556}
};

// LEB Right wall 1 circuit breakers
const VECTOR3 LEB_R1_CB_POS[LEB_R1_CBCOUNT] = {
{1.0887, 0.0627, 0.2484}, {1.0887, 0.0827, 0.2484}, {1.0887, 0.1026, 0.2484}, {1.0887, 0.1225, 0.2484}, {1.0887, 0.0627, 0.2139},
{1.0887, 0.0812, 0.2139}, {1.0887, 0.0997, 0.2139}, {1.0887, 0.1182, 0.2139}, {1.0887, 0.0627, 0.1804}, {1.0887, 0.0812, 0.1804},
{1.0887, 0.0998, 0.1804}, {1.0887, 0.1182, 0.1804}, {1.0887, 0.0626, 0.1464}, {1.0887, 0.0813, 0.1464}, {1.0887, 0.0997, 0.1464},
{1.0887, 0.1182, 0.1464}, {1.0887, 0.0627, 0.1118}, {1.0887, 0.0812, 0.1118}, {1.0887, 0.0628, 0.0772}, {1.0887, 0.0812, 0.0772},
{1.0887, 0.0627, 0.0106}, {1.0887, 0.0812, 0.0106}, {1.0887, -0.0841, 0.2422}, {1.0887, -0.0356, 0.2422}, {1.0887, -0.1464, 0.2047},
{1.0887, -0.1118, 0.2047}, {1.0887, -0.0773, 0.2047}, {1.0887, -0.0428, 0.2047}, {1.0887, -0.2274, 0.1595}, {1.0887, -0.2076, 0.1595},
{1.0887, -0.1876, 0.1595}, {1.0887, -0.1678, 0.1595}, {1.0887, -0.1489, 0.1595}, {1.0887, -0.1119, 0.1589}, {1.0887, -0.0769, 0.1589},
{1.0887, -0.0429, 0.1589}, {1.0887, -0.2301, 0.1139}, {1.0887, -0.2090, 0.1139}, {1.0887, -0.1881, 0.1139}, {1.0887, -0.1672, 0.1139},
{1.0887, -0.1463, 0.1139}, {1.0887, -0.1118, 0.1139}, {1.0887, -0.0773, 0.1139}, {1.0887, -0.0427, 0.1139}, {1.0887, -0.5147, 0.0014},
{1.0887, -0.4804, 0.0014}, {1.0887, -0.4612, 0.0014}, {1.0887, -0.4311, 0.0014}, {1.0887, -0.4125, 0.0014}, {1.0887, -0.3933, 0.0014},
{1.0887, -0.3748, 0.0014}, {1.0887, -0.5147, -0.0482}, {1.0887, -0.4805, -0.0482}, {1.0887, -0.4611, -0.0482}, {1.0887, -0.4313, -0.0482},
{1.0887, -0.4124, -0.0482}, {1.0887, -0.3933, -0.0482}, {1.0887, -0.3745, -0.0482}, {1.0887, -0.5146, -0.0984}, {1.0887, -0.4804, -0.0984},
{1.0887, -0.4611, -0.0984}, {1.0887, -0.4310, -0.0984}, {1.0887, -0.4124, -0.0984}, {1.0887, -0.3935, -0.0984}, {1.0887, -0.3745, -0.0984},
{1.0887, -0.5207, -0.1431}, {1.0887, -0.5019, -0.1431}, {1.0887, -0.4829, -0.1431}, {1.0887, -0.4639, -0.1431}, {1.0887, -0.4313, -0.1431},
{1.0887, -0.4124, -0.1431}, {1.0887, -0.3934, -0.1431}, {1.0887, -0.3745, -0.1431}, {1.0887, -0.5237, -0.1897}, {1.0887, -0.5028, -0.1897},
{1.0887, -0.4819, -0.1897}, {1.0887, -0.4610, -0.1897}, {1.0887, -0.4353, -0.1897}, {1.0887, -0.4144, -0.1897}, {1.0887, -0.3932, -0.1897},
{1.0887, -0.3725, -0.1897}
};

// LEB Right wall 2 circuit breakers
const VECTOR3 LEB_R2_CB_POS[LEB_R2_CBCOUNT] = {
{1.0866, -0.6567, -0.3024}, {1.0866, -0.6567, -0.3280}, {1.0866, -0.6567, -0.3558}, {1.0866, -0.6567, -0.3814}, {1.0866, -0.6565, -0.4168},
{1.0866, -0.6565, -0.4437}, {1.0866, -0.6565, -0.4705}, {1.0866, -0.6565, -0.4975}, {1.0866, -0.6565, -0.5245}, {1.0866, -0.6565, -0.5468},
{1.0866, 0.1940, -0.3246}, {1.0866, 0.2255, -0.3246}, {1.0866, 0.2491, -0.3246}, {1.0866, 0.2732, -0.3246}, {1.0866, 0.2970, -0.3246},
{1.0866, 0.3283, -0.3246}, {1.0866, 0.3521, -0.3246}, {1.0866, 0.3761, -0.3246}, {1.0866, 0.4073, -0.3246}, {1.0866, 0.4311, -0.3246},
{1.0866, 0.4551, -0.3246}, {1.0866, 0.4789, -0.3246}, {1.0866, 0.5102, -0.3246}, {1.0866, 0.5766, -0.2883}, {1.0866, 0.6237, -0.2883},
{1.0866, 0.5766, -0.3211}, {1.0866, 0.6237, -0.3213}, {1.0866, 0.7718, -0.3005}, {1.0866, 0.7983, -0.3005}, {1.0866, 0.7718, -0.3409},
{1.0866, 0.7983, -0.3409}
};

// LEB Left push-buttons
const VECTOR3 LEB_L_PUSHB_POS[LEB_L_PUSHBCOUNT] = {
{-1.0608, 0.2392, -0.3428}, {-1.0727, -0.5641, -0.3161}, {-1.0727, -0.5997, -0.3164}, {-1.0838, -0.6491, -0.5083}
};

// LEB Left Rotaries

const VECTOR3 LEB_L_ROT_POS[LEB_L_ROTCOUNT] = {
{-1.0826, 1.0052, -0.3451}, {-1.0827, 0.9334, -0.7408}, {-1.0797, 0.6465, -0.8017}, {-1.0810, 0.6287, -0.5564}, {-1.1027, 0.1549, -0.5528},
{-1.1027, 0.1612, -0.6055}, {-1.1027, 0.1569, -0.6560}, {-1.1024, 0.2024, -0.6963}, {-1.1002, 0.2175, -0.7425}, {-1.1002, 0.1567, -0.7425},
{-1.1002, 0.2040, -0.7992}, {-1.1002, 0.1607, -0.7992}, {-1.0794, -0.5441, -0.4012}, {-1.0794, -0.6041, -0.4012}, {-1.0827, -0.6138, -0.5079},
{-1.0802, -0.5950, -0.5845}, {-1.0802, -0.6160, -0.7094}, {-1.0802, -0.6165, -0.7634}, {-1.0802, -0.6165, -0.8171}, {-1.0802, -0.5416, -0.7602},
{-1.0827, 0.1467, -0.3310}, {-1.0740, 0.1364, -0.3000}
};

// Switch Covers
const VECTOR3 P1_COVERS_POS[P1_SWITCHCOVERCOUNT] = {
{-0.523385, 0.837354, 0.401598}, {-0.897739, 0.291494, 0.218991}, {-0.807395, 0.400065, 0.255313}, {-0.807395, 0.400065, 0.255313}, {-0.650851, 0.339981, 0.235209}, 
{-0.650851, 0.339981, 0.235209}, {-0.650851, 0.339981, 0.235209}, {-0.530182, 0.427677, 0.267877}, {-0.530182, 0.427677, 0.267877}, {-0.530182, 0.427677, 0.267877}, 
{-0.530182, 0.427677, 0.267877}, {-0.530182, 0.427677, 0.267877}, {-0.530182, 0.427677, 0.267877}, {-0.530182, 0.427677, 0.267877}, {-0.530182, 0.427677, 0.267877}
};

const VECTOR3 P2_COVERS_POS[P2_SWITCHCOVERCOUNT] = {
{-0.42006, 0.46232, 0.276106}, {-0.42006, 0.46232, 0.276106}, {-0.42006, 0.46232, 0.276106}, {-0.42006, 0.46232, 0.276106}, {-0.42006, 0.46232, 0.276106}, 
{-0.342684, 0.413289, 0.259907}, {-0.342684, 0.413289, 0.259907}, {-0.412789, 0.34966, 0.238427}, {-0.412789, 0.34966, 0.238427}, {-0.447372, 0.295628, 0.220297}, 
{-0.234966, 0.851656, 0.406449}, {-0.169936, 0.675647, 0.347563}
};

// Panel 600
const VECTOR3 Sw_P600_01Location = { 0.1788, 1.4288, -0.3471 };
const VECTOR3 Sw_P600_02Location = { -0.1769, 1.4288, -0.3455 };

// THC Handle
const VECTOR3 THChandleLocation = { -0.9249, 0.2373, 0.0419 };

// Panel 163
const VECTOR3 Switch_P163Location = { -0.9108, -0.6972, -0.4823 };

// PLVC switch
const VECTOR3 Switch_PLVCLocation = { -1.0979, 1.0467, -0.4147 };

extern GDIParams g_Param;

void Saturn::InitVC()
{
	//int i;
	TRACESETUP("Saturn::InitVC");

	ReleaseSurfacesVC();

	DWORD ck = oapiGetColour(255, 0, 255);

	// Load CM VC surfaces

	srf[SRF_VC_DIGITALDISP] = oapiLoadTexture("ProjectApollo/VC/digitaldisp.dds");
	srf[SRF_VC_DIGITALDISP2] = oapiLoadTexture("ProjectApollo/VC/digitaldisp_2.dds");
	srf[SRF_VC_DSKYDISP] = oapiLoadTexture("ProjectApollo/VC/dsky_disp.dds");
	srf[SRF_VC_DSKY_LIGHTS] = oapiLoadTexture("ProjectApollo/VC/dsky_lights.dds");
	srf[SRF_VC_MASTERALARM] = oapiLoadTexture("ProjectApollo/VC/masteralarmbright.dds");
	srf[SRF_VC_CW_LIGHTS] = oapiLoadTexture("ProjectApollo/VC/cw_lights.dds");
	srf[SRF_VC_LVENGLIGHTS] = oapiLoadTexture("ProjectApollo/VC/lv_eng.dds");
	srf[SRF_VC_EVENT_TIMER_DIGITS] = oapiLoadTexture("ProjectApollo/VC/event_timer.dds");
#if 0
	srf[SRF_VC_EMS_SCROLL_LEO] = oapiLoadTexture("ProjectApollo/VC/EMSscroll_LEO.dds", true);
#else
	// As a workaround for the dynamic flag of oapiLoadTexture being lost on the way,
	// we create a texture with oapiCreateTextureSurface which will have the correct flags
	// and then blit the original texture onto it
	SURFHANDLE tmp = oapiLoadTexture("ProjectApollo/VC/EMSscroll_LEO.dds");
	// Another Workaround to enable 8K Textures
	if (TexMul == TEXTURES_8K){
		srf[SRF_VC_EMS_SCROLL_LEO] = oapiCreateTextureSurface(4096 * TexMul/2, 256 * TexMul);
		oapiBlt(srf[SRF_VC_EMS_SCROLL_LEO], tmp, 0, 0, 0, 0, 4096 * TexMul/2, 256 * TexMul);
	}else{
		srf[SRF_VC_EMS_SCROLL_LEO] = oapiCreateTextureSurface(4096 * TexMul, 256 * TexMul);
		oapiBlt(srf[SRF_VC_EMS_SCROLL_LEO], tmp, 0, 0, 0, 0, 4096 * TexMul, 256 * TexMul);
	}
	oapiReleaseTexture(tmp);
#endif
	srf[SRF_VC_EMS_SCROLL_BORDER] = oapiLoadTexture("ProjectApollo/VC/EMSscroll_border.dds");
	srf[SRF_VC_EMS_SCROLL_BUG] = oapiLoadTexture("ProjectApollo/VC/EMSscroll_bug.dds");
	srf[SRF_VC_EMS_LIGHTS] = oapiLoadTexture("ProjectApollo/VC/ems_lights.dds");
	srf[SRF_VC_INDICATOR] = oapiLoadTexture("ProjectApollo/VC/Indicator.dds");
	srf[SRF_VC_ECSINDICATOR] = oapiLoadTexture("ProjectApollo/VC/ECSIndicator.dds");
	srf[SRF_VC_SEQUENCERSWITCHES] = oapiLoadTexture("ProjectApollo/VC/SequencerSwitches.dds");
	srf[SRF_VC_LVENGLIGHTS_S1B] = oapiLoadTexture("ProjectApollo/VC/lv_eng_s1b.dds");
	srf[SRF_VC_SPS_FONT_BLACK] = oapiLoadTexture("ProjectApollo/VC/fonts_black.dds");
	srf[SRF_VC_SPS_FONT_WHITE] = oapiLoadTexture("ProjectApollo/VC/fonts_white.dds");
	srf[SRF_VC_SPS_INJ_VLV] = oapiLoadTexture("ProjectApollo/VC/sps_injector_indicators.dds");
	srf[SRF_VC_SPSMAXINDICATOR] = oapiLoadTexture("ProjectApollo/VC/SPSmax_indicator.dds");
	srf[SRF_VC_SPSMININDICATOR] = oapiLoadTexture("ProjectApollo/VC/SPSmin_indicator.dds");
	srf[SRF_VC_THUMBWHEEL_LARGEFONTSINV] = oapiLoadTexture("ProjectApollo/VC/thumbwheel_large_fonts_inv.dds");
	srf[SRF_VC_CWS_GNLIGHTS] = oapiLoadTexture("ProjectApollo/VC/csm_lower_equip_bay_lights.dds");
	srf[SRF_VC_DIGITAL90] = oapiLoadTexture("ProjectApollo/VC/digitaldisp90.dds");
	srf[SRF_VC_EVENT_TIMER_DIGITS90] = oapiLoadTexture("ProjectApollo/VC/event_timer90.dds");
	srf[SRF_VC_ABORT] = oapiLoadTexture("ProjectApollo/VC/abort.dds");

	// Set Colour Key

	oapiSetSurfaceColourKey(srf[SRF_VC_DIGITALDISP], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_DIGITALDISP2], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_DSKYDISP], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_DSKY_LIGHTS], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_MASTERALARM], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_CW_LIGHTS], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_LVENGLIGHTS], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_EMS_SCROLL_BORDER], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_EMS_SCROLL_BUG], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_EMS_LIGHTS], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_ECSINDICATOR], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_SEQUENCERSWITCHES], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_LVENGLIGHTS_S1B], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_SPS_FONT_BLACK], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_SPS_FONT_WHITE], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_SPS_INJ_VLV], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_SPSMAXINDICATOR], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_SPSMININDICATOR], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_THUMBWHEEL_LARGEFONTSINV], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_CWS_GNLIGHTS], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_DIGITAL90], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_EVENT_TIMER_DIGITS90], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_ABORT], ck);

	//
	// Register active areas for repainting here
	//
		
	SURFHANDLE MainPanelTex1 = oapiGetTextureHandle(hCMVC, VC_TEX_CMVCTex1_dds);
	SURFHANDLE MainPanelTex2 = oapiGetTextureHandle(hCMVC, VC_TEX_CMVCTex2_dds);

	// Panel 1

	oapiVCRegisterArea(AID_VC_MASTER_ALARM, _R(1489*TexMul, 1183*TexMul, 1534*TexMul, 1219*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_LVENG_LIGHTS, _R(1868*TexMul, 1421*TexMul, 1969*TexMul, 1565*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_EVENT_TIMER, _R(1891*TexMul, 1356*TexMul, 1962*TexMul, 1374*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_EMS_SCROLL_LEO, _R(1756*TexMul, 982*TexMul, 1900*TexMul, 1134*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SPS_LIGHT, _R(1840*TexMul, 1142*TexMul, 1881*TexMul, 1158*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_PT05G_LIGHT, _R(1774*TexMul, 1142*TexMul, 1815*TexMul, 1158*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_EMSDVDISPLAY, _R(1768*TexMul, 1204*TexMul, 1925*TexMul, 1225*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_EMS_RSI_BKGRND, _R(1627*TexMul, 1149*TexMul, 1715*TexMul, 1236*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SEQUENCERSWITCHES, _R(1847*TexMul, 1606*TexMul, 1886*TexMul, 1644*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	oapiVCRegisterArea(AID_VC_ASCPDISPLAYROLL, _R(1224*TexMul, 1830*TexMul, 1254*TexMul, 1842*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_ASCPDISPLAYPITCH, _R(1224*TexMul, 1892*TexMul, 1254*TexMul, 1904*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_ASCPDISPLAYYAW, _R(1224*TexMul, 1954*TexMul, 1254*TexMul, 1966*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	oapiVCRegisterArea(AID_VC_ABORT_BUTTON, _R(1887 * TexMul, 1286 * TexMul, 1949 * TexMul, 1317 * TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	// Panel 2
	oapiVCRegisterArea(AID_VC_DSKY_DISPLAY, _R(254*TexMul, 1235*TexMul, 359*TexMul, 1411*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_DSKY_LIGHTS, _R(110*TexMul, 1240*TexMul, 212*TexMul, 1360*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MISSION_CLOCK, _R(849*TexMul, 949*TexMul, 992*TexMul, 972*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_CWS_LIGHTS_LEFT, _R(555*TexMul, 743*TexMul, 767*TexMul, 1851*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_CWS_LIGHTS_RIGHT, _R(810*TexMul, 743*TexMul, 1023*TexMul, 851*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	oapiVCRegisterArea(AID_VC_RCS_HELIUM1_TB, _R(606*TexMul, 1014*TexMul, 756*TexMul, 1037*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_HELIUM2_TB, _R(606*TexMul, 1172*TexMul, 756*TexMul, 1195*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_PROP1_TB, _R(606*TexMul, 1305*TexMul, 756*TexMul, 1328*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_PROP2_TB, _R(518*TexMul, 1438*TexMul, 754*TexMul, 1461*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_ECSRADIATORIND, _R(814*TexMul, 1329*TexMul, 837*TexMul, 1352*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_DOCKINGPROBEIND, _R(411*TexMul, 825*TexMul, 434*TexMul, 875*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	// Panel 3
	oapiVCRegisterArea(AID_VC_MASTER_ALARM2, _R(433*TexMul, 1339*TexMul, 478*TexMul, 1375*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex2);

	oapiVCRegisterArea(AID_VC_FCPHRADTEMPIND, _R(295*TexMul, 1177*TexMul, 492*TexMul, 1200*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_FCRADIATORSIND, _R(295*TexMul, 1226*TexMul, 404*TexMul, 1249*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(ADI_VC_MAINBUSAIND, _R(426*TexMul, 1445*TexMul, 535*TexMul, 1468*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SPSHEVALVEIND, _R(71*TexMul, 1583*TexMul, 137*TexMul, 1606*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_FCREACTANTSIND, _R(296*TexMul, 1580*TexMul, 534*TexMul, 1604*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_TELECOMTB, _R(592*TexMul, 1729*TexMul, 615*TexMul, 1802*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SPS_OXID_PCT, _R(137*TexMul, 1315*TexMul, 175*TexMul, 1328*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SPS_FUEL_PCT, _R(137*TexMul, 1344*TexMul, 175*TexMul, 1357*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SPSOXIDFLOWIND, _R(78*TexMul, 1479*TexMul, 100*TexMul, 1521*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SPS_INJ_VLV, _R(69*TexMul, 1210*TexMul, 247*TexMul, 1244*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	// Lower Equipment Bay
	oapiVCRegisterArea(AID_VC_MASTER_ALARM3, _R(1752*TexMul, 167*TexMul, 1797*TexMul, 203*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex2);

	oapiVCRegisterArea(AID_VC_CWS_GNLIGHTS, _R(1750*TexMul, 56*TexMul, 1803*TexMul, 132*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_OPT_SHAFTDISPLAY, _R(520*TexMul, 144*TexMul, 571*TexMul, 156*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_OPT_TRUNDISPLAY, _R(648*TexMul, 94*TexMul, 699*TexMul, 106*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_DSKY_DISPLAY2, _R(1050*TexMul, 1076*TexMul, 1155*TexMul, 1252*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_DSKY_LIGHTS2, _R(906*TexMul, 1081*TexMul, 1008*TexMul, 1201*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_EVENT_TIMER306, _R(220*TexMul, 149*TexMul, 238*TexMul, 220*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MISSION_CLOCK306, _R(337*TexMul, 129*TexMul, 360*TexMul, 272*TexMul), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	// Integral Lights Panel 8
	oapiVCRegisterArea(AID_VC_INTEGRAL_LIGHT_P8, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FLOOD_LIGHT_P8, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_NUMERICS_LIGHT_P8, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Integral Lights Panel 5
	oapiVCRegisterArea(AID_VC_INTEGRAL_LIGHT_P5, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FLOOD_LIGHT_P5, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Integral Lights LEB
	oapiVCRegisterArea(AID_VC_INTEGRAL_LIGHT_P100, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FLOOD_LIGHT_P100, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_NUMERICS_LIGHT_P100, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Cue Cards Lighting
	oapiVCRegisterArea(AID_VC_CUE_CARDS_LIGHTING, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Initialize surfaces

	SMRCSHelium1ATalkback.InitVC(srf[SRF_VC_INDICATOR]);
	SMRCSHelium1BTalkback.InitVC(srf[SRF_VC_INDICATOR]);
	SMRCSHelium1CTalkback.InitVC(srf[SRF_VC_INDICATOR]);
	SMRCSHelium1DTalkback.InitVC(srf[SRF_VC_INDICATOR]);

	SMRCSHelium2ATalkback.InitVC(srf[SRF_VC_INDICATOR]);
	SMRCSHelium2BTalkback.InitVC(srf[SRF_VC_INDICATOR]);
	SMRCSHelium2CTalkback.InitVC(srf[SRF_VC_INDICATOR]);
	SMRCSHelium2DTalkback.InitVC(srf[SRF_VC_INDICATOR]);

	SMRCSProp1ATalkback.InitVC(srf[SRF_VC_INDICATOR]);
	SMRCSProp1BTalkback.InitVC(srf[SRF_VC_INDICATOR]);
	SMRCSProp1CTalkback.InitVC(srf[SRF_VC_INDICATOR]);
	SMRCSProp1DTalkback.InitVC(srf[SRF_VC_INDICATOR]);

	CMRCSProp1Talkback.InitVC(srf[SRF_VC_INDICATOR]);
	CMRCSProp2Talkback.InitVC(srf[SRF_VC_INDICATOR]);
	SMRCSProp2ATalkback.InitVC(srf[SRF_VC_INDICATOR]);
	SMRCSProp2BTalkback.InitVC(srf[SRF_VC_INDICATOR]);
	SMRCSProp2CTalkback.InitVC(srf[SRF_VC_INDICATOR]);
	SMRCSProp2DTalkback.InitVC(srf[SRF_VC_INDICATOR]);

	EcsRadiatorIndicator.InitVC(srf[SRF_VC_ECSINDICATOR]);

	DockingProbeAIndicator.InitVC(srf[SRF_VC_INDICATOR]);
	DockingProbeBIndicator.InitVC(srf[SRF_VC_INDICATOR]);

	FuelCellPhIndicator.InitVC(srf[SRF_VC_INDICATOR]);
	FuelCellRadTempIndicator.InitVC(srf[SRF_VC_INDICATOR]);

	FuelCellRadiators1Indicator.InitVC(srf[SRF_VC_INDICATOR]);
	FuelCellRadiators2Indicator.InitVC(srf[SRF_VC_INDICATOR]);
	FuelCellRadiators3Indicator.InitVC(srf[SRF_VC_INDICATOR]);

	MainBusAIndicator1.InitVC(srf[SRF_VC_INDICATOR]);
	MainBusAIndicator2.InitVC(srf[SRF_VC_INDICATOR]);
	MainBusAIndicator3.InitVC(srf[SRF_VC_INDICATOR]);

	SPSHeliumValveAIndicator.InitVC(srf[SRF_VC_INDICATOR]);
	SPSHeliumValveBIndicator.InitVC(srf[SRF_VC_INDICATOR]);

	FuelCellReactants1Indicator.InitVC(srf[SRF_VC_INDICATOR]);
	FuelCellReactants2Indicator.InitVC(srf[SRF_VC_INDICATOR]);
	FuelCellReactants3Indicator.InitVC(srf[SRF_VC_INDICATOR]);
	MainBusBIndicator1.InitVC(srf[SRF_VC_INDICATOR]);
	MainBusBIndicator2.InitVC(srf[SRF_VC_INDICATOR]);
	MainBusBIndicator3.InitVC(srf[SRF_VC_INDICATOR]);

	PwrAmplTB.InitVC(srf[SRF_VC_INDICATOR]);
	DseTapeTB.InitVC(srf[SRF_VC_INDICATOR]);

	SPSOxidPercentMeter.InitVC(srf[SRF_VC_SPS_FONT_BLACK], srf[SRF_VC_SPS_FONT_WHITE]);
	SPSFuelPercentMeter.InitVC(srf[SRF_VC_SPS_FONT_BLACK], srf[SRF_VC_SPS_FONT_WHITE]);

	SPSOxidFlowValveMaxIndicator.InitVC(srf[SRF_VC_SPSMAXINDICATOR]);
	SPSOxidFlowValveMinIndicator.InitVC(srf[SRF_VC_SPSMININDICATOR]);

	SPSInjectorValve1Indicator.InitVC(srf[SRF_VC_SPS_INJ_VLV]);
	SPSInjectorValve2Indicator.InitVC(srf[SRF_VC_SPS_INJ_VLV]);
	SPSInjectorValve3Indicator.InitVC(srf[SRF_VC_SPS_INJ_VLV]);
	SPSInjectorValve4Indicator.InitVC(srf[SRF_VC_SPS_INJ_VLV]);
}

void Saturn::ReleaseSurfacesVC()
{
	for (int i = 0; i < nsurfvc; i++)
		if (srf[i]) {
			oapiDestroySurface(srf[i]);
			srf[i] = 0;
		}
}

bool Saturn::clbkLoadVC (int id)
{
	TRACESETUP("Saturn::clbkLoadVC");

	// Set VC view to last saved position
	if (FirstTimestep || !InVC) {
		id = viewpos;
	}

	InVC = true;
	InPanel = false;

	//Reset Clip Radius settings
	SetClipRadius(0.0);

	// Init the 2D panel switches to fix XRSound not giving us switch clicks if we load directly into the VC.
	// Calling InitPanel(SATPANEL_MAIN) also works, since that function calls SetSwitches() as well.
	SetSwitches(SATPANEL_MAIN);	// Use main panel as a placeholder, it doesn't actually matter

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

	case SATVIEW_LEFTSEAT:
		viewpos = SATVIEW_LEFTSEAT;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.0, -0.3, 0.0), 0, 0, _V(-0.3, -0.3, 0.1), 90 * RAD, 0, _V(0.25, -0.19, 0.15), 0, 0);
		oapiVCSetNeighbours(-1, SATVIEW_CENTERSEAT, SATVIEW_LEFTDOCK, SATVIEW_LEBLEFT);

		SetView(true);
		SetCOASMesh();

		RegisterActiveAreas();

		return true;

	case SATVIEW_CENTERSEAT:
		viewpos = SATVIEW_CENTERSEAT;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(-0.0, -0.15, 0.0), 0, 0, _V(-0.3, 0.0, 0.0), 0, 0, _V(0.3, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(SATVIEW_LEFTSEAT, SATVIEW_RIGHTSEAT, SATVIEW_UPPER_CENTER, SATVIEW_LOWER_CENTER);

		SetView(true);

		RegisterActiveAreas();

		return true;

	case SATVIEW_RIGHTSEAT:
		viewpos = SATVIEW_RIGHTSEAT;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(-0.0, -0.3, 0.0), 0, 0, _V(-0.3, 0.0, 0.0), 0, 0, _V(0.3, -0.3, 0.1), -90 * RAD, 0);
		oapiVCSetNeighbours(SATVIEW_CENTERSEAT, -1, SATVIEW_RIGHTDOCK, SATVIEW_LEBRIGHT);

		SetView(true);

		RegisterActiveAreas();

		return true;

	case SATVIEW_GNPANEL:
		viewpos = SATVIEW_GNPANEL;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.8 * PI, 0.4 * PI);
		oapiVCSetNeighbours(SATVIEW_LEBLEFT, SATVIEW_LEBRIGHT, SATVIEW_TUNNEL, -1);
		SetCameraMovement(_V(0.0, -0.2, 0.0), 0, 0, _V(-0.4, -0.2, 0.0), 0, 0, _V(0.4, -0.2, 0.0), 0, 0);

		SetView(true);

		PanelId = SATPANEL_TELESCOPE;

		RegisterActiveAreas();

		return true;

	case SATVIEW_LEFTDOCK:
		viewpos = SATVIEW_LEFTDOCK;
		SetCameraMovement(_V(0.0, 0.0, 0.0), 0, 0, _V(0.0, 0.0, 0.0), 0, 0, _V(0.0, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(-1, SATVIEW_RIGHTDOCK, -1, SATVIEW_LEFTSEAT);

		SetView(true);
		SetCOASMesh();

		RegisterActiveAreas();

		return true;

	case SATVIEW_RIGHTDOCK:
		viewpos = SATVIEW_RIGHTDOCK;
		SetCameraMovement(_V(0.0, 0.0, 0.0), 0, 0, _V(0.0, 0.0, 0.0), 0, 0, _V(0.0, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(SATVIEW_LEFTDOCK, -1, -1, SATVIEW_RIGHTSEAT);

		SetView(true);

		RegisterActiveAreas();

		return true;

	case SATVIEW_LEBLEFT:
		viewpos = SATVIEW_LEBLEFT;
		SetCameraMovement(_V(0.0, 1.4, -0.2), -20 * RAD, 0, _V(-0.1, 0.7, -0.3), -20 * RAD, 0, _V(-0.1, 0.55, 0.2), 0, 20 * RAD);
		oapiVCSetNeighbours(-1, SATVIEW_GNPANEL, SATVIEW_LEFTSEAT, -1);

		SetView(true);

		RegisterActiveAreas();

		return true;

	case SATVIEW_LEBRIGHT:
		viewpos = SATVIEW_LEBRIGHT;
		SetCameraMovement(_V(0.0, 0.4, 0.1), 0, 0, _V(0.0, 0.0, 0.0), 0, 0, _V(0.0, -0.15, -0.35), 0, 0);
		oapiVCSetNeighbours(SATVIEW_GNPANEL, -1, SATVIEW_RIGHTSEAT, -1);

		SetView(true);

		RegisterActiveAreas();

		return true;

	case SATVIEW_UPPER_CENTER:
		viewpos = SATVIEW_UPPER_CENTER;
		SetCameraMovement(_V(0.0, 0.0, 0.0), 0, 0, _V(0.2, -0.03, -0.1), 0, 20 * RAD, _V(-0.2, -0.03, -0.1), 0, 20 * RAD);
		oapiVCSetNeighbours(-1, -1, -1, SATVIEW_CENTERSEAT);

		SetView(true);

		RegisterActiveAreas();

		return true;

	case SATVIEW_LOWER_CENTER:
		viewpos = SATVIEW_LOWER_CENTER;
		SetCameraMovement(_V(0.0, 0.0, 0.0), 0, 0, _V(0.0, 0.0, 0.0), 0, 0, _V(0.0, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(-1, -1, SATVIEW_CENTERSEAT, SATVIEW_TUNNEL);

		SetView(true);

		RegisterActiveAreas();

		return true;

	case SATVIEW_TUNNEL:
		viewpos = SATVIEW_TUNNEL;
		SetCameraMovement(_V(0.0, 0.0, 0.0), 0, 0, _V(-0.2, -0.1, 0.0), 90 * RAD, 0, _V(0.0, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(-1, -1, SATVIEW_LOWER_CENTER, SATVIEW_GNPANEL);

		SetView(true);

		RegisterActiveAreas();

		return true;

	default:
		return false;
	}
}

void Saturn::clbkVisualCreated(VISHANDLE vis, int refcount) {
	this->vis = vis;
	if (vcidx != -1) {
        vcmesh = GetDevMesh(vis, vcidx);
//		seatsunfoldedmesh = GetDevMesh(vis, seatsunfoldedidx);
//		seatsfoldedmesh = GetDevMesh(vis, seatsfoldedidx);
	}
}

void Saturn::clbkVisualDestroyed(VISHANDLE vis, int refcount) {
	if (this->vis == vis) this->vis = NULL;
    vcmesh = NULL;
//	seatsunfoldedmesh = NULL;
//	seatsfoldedmesh = NULL;
}

void Saturn::RegisterActiveAreas() {

	int i = 0;

	//
	// Initialize surfaces
	//
	InitVC();

	VECTOR3 ofs = _V(0.0, 0.0, 0.0);

	if (SaturnType == SAT_SATURNV) {
		if (stage < 12) {
			ofs.z = 43.65;
		} else if (stage < 20) {
			ofs.z = 28.5;
		} else if (stage < 30) {
			ofs.z = 15.25;
		} else if (stage < 40) {
			ofs.z = 2.1;
		} else if (stage < 42) {
			ofs.z = 0.0;
		} else {
			ofs.z = -1.2;
		}
	} else {
		if (stage < 12) {
			ofs.z = 35.15;
		} else if (stage < 30) {
			ofs.z = 15.2;
		} else if (stage < 40) {
			ofs.z = 2.1;
		} else if (stage < 42) {
			ofs.z = 0.0;
		} else {
			ofs.z = -1.2;
		}
	}

	//Apply center of gravity offset here
	ofs -= currentCoG;

	//
	// Register active areas for switches/animations here
	//

	// FDAI's
	oapiVCRegisterArea(AID_VC_FDAI_LEFT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FDAI_RIGHT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Clickspot to cycle seats folded/unfolded
	oapiVCRegisterArea(AID_VC_SEATSCYCLE, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_SEATSCYCLE, _V(-0.9187, 0.200999, -0.258652) + ofs, 0.05);

	// Side Hatch
	if (viewpos != SATVIEW_LOWER_CENTER) {
		const VECTOR3 SideHatchLocation = { 0.2436, 1.1710, 0.1699 };
		const VECTOR3 SideHatch_HandleRot1Location = { 0.3076, 1.3543, -0.1137 };
		const VECTOR3 SideHatch_HandleRot2Location = { 0.2348, 1.2453, 0.0608 };
		const VECTOR3 SideHatch_VentValveLocation = { -0.2637, 1.1932, 0.1462 };
		const VECTOR3 SideHatch_openLocation = { -0.4243, 1.7647, 0.6818 };
		const VECTOR3 SideHatch_HandleRot1_openLocation = { -0.4944, 2.0370, 0.4730 };
		const VECTOR3 SideHatch_HandleRot2_openLocation = { -0.4556, 1.8491, 0.5764 };
		const VECTOR3 SideHatch_VentValve_openLocation = { -0.4557, 1.4090, 0.3188 };

		if (!SideHatch.IsOpen()) {

			oapiVCRegisterArea(AID_VC_SIDEHATCH_HANDLE, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
			oapiVCSetAreaClickmode_Spherical(AID_VC_SIDEHATCH_HANDLE, SideHatchLocation + ofs, 0.1);

			oapiVCRegisterArea(AID_VC_SIDEHATCH_GEARBOX_SEL, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
			oapiVCSetAreaClickmode_Spherical(AID_VC_SIDEHATCH_GEARBOX_SEL, SideHatch_HandleRot1Location + ofs, ROT);

			oapiVCRegisterArea(AID_VC_SIDEHATCH_ACT_HANDLE_SEL, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
			oapiVCSetAreaClickmode_Spherical(AID_VC_SIDEHATCH_ACT_HANDLE_SEL, SideHatch_HandleRot2Location + ofs, ROT);

			oapiVCRegisterArea(AID_VC_SIDEHATCH_VENT_VALVE, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
			oapiVCSetAreaClickmode_Spherical(AID_VC_SIDEHATCH_VENT_VALVE, SideHatch_VentValveLocation + ofs, ROT);

		}
		else {

			oapiVCRegisterArea(AID_VC_SIDEHATCH_HANDLE, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
			oapiVCSetAreaClickmode_Spherical(AID_VC_SIDEHATCH_HANDLE, SideHatch_openLocation + ofs, 0.1);

			oapiVCRegisterArea(AID_VC_SIDEHATCH_GEARBOX_SEL, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
			oapiVCSetAreaClickmode_Spherical(AID_VC_SIDEHATCH_GEARBOX_SEL, SideHatch_HandleRot1_openLocation + ofs, ROT);

			oapiVCRegisterArea(AID_VC_SIDEHATCH_ACT_HANDLE_SEL_OPEN, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
			oapiVCSetAreaClickmode_Spherical(AID_VC_SIDEHATCH_ACT_HANDLE_SEL_OPEN, SideHatch_HandleRot2_openLocation + ofs, ROT);

			oapiVCRegisterArea(AID_VC_SIDEHATCH_VENT_VALVE, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
			oapiVCSetAreaClickmode_Spherical(AID_VC_SIDEHATCH_VENT_VALVE, SideHatch_VentValve_openLocation + ofs, ROT);
		}
	}

	// Forward Hatch
	if (viewpos == SATVIEW_TUNNEL) {
		const VECTOR3 FwdHatch_Location = { -0.1495, 0.0705, 1.0980 };
		const VECTOR3 FwdHatch_Equal_ValveLocation = { 0.0011, -0.0000, 1.0773 };

		oapiVCRegisterArea(AID_VC_FWDHATCH_HANDLE, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_FWDHATCH_HANDLE, FwdHatch_Location + ofs, 0.1);

		if (!ForwardHatch.IsOpen()) {
			oapiVCRegisterArea(AID_VC_FWDHATCH_PRESS_EQU_VLV, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
			oapiVCSetAreaClickmode_Spherical(AID_VC_FWDHATCH_PRESS_EQU_VLV, FwdHatch_Equal_ValveLocation + ofs, ROT);
		}
	}

    // COAS
	const VECTOR3 COASLocation = { -0.5232, 1.1292, 0.1087 };
	oapiVCRegisterArea(AID_VC_COAS, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_COAS, COASLocation + ofs, 0.05);

	// THC Handle
	oapiVCRegisterArea(AID_VC_THC_HANDLE, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_THC_HANDLE, THChandleLocation + ofs, ROT);

	// Panel 600-602

	//Normalized vectors for quadrilateral click mode (re-defined for each panel orientation)
	VECTOR3 UL = _V(0.4666, 0.0041, 0.8844);
	VECTOR3 UR = _V(-0.4692, 0.0040, 0.8830);
	VECTOR3 DL = _V(0.4690, -0.0039, -0.8831);
	VECTOR3 DR = _V(-0.4665, -0.0040, -0.8843);

	const double P600_SWITCH = 0.05;

	oapiVCRegisterArea(AID_VC_SWITCH_P600_01, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P600_01 + i, Sw_P600_01Location + _V(UL.x * P600_SWITCH, UL.y * P600_SWITCH, UL.z * P600_SWITCH) + P600_CLICK + ofs, Sw_P600_01Location + _V(UR.x * P600_SWITCH, UR.y * P600_SWITCH, UR.z * P600_SWITCH) + P600_CLICK + ofs, Sw_P600_01Location + _V(DL.x * P600_SWITCH, DL.y * P600_SWITCH, DL.z * P600_SWITCH) + P600_CLICK + ofs, Sw_P600_01Location + _V(DR.x * P600_SWITCH, DR.y * P600_SWITCH, DR.z * P600_SWITCH) + P600_CLICK + ofs);

	oapiVCRegisterArea(AID_VC_SWITCH_P600_02, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P600_02 + i, Sw_P600_02Location + _V(UL.x * P600_SWITCH, UL.y * P600_SWITCH, UL.z * P600_SWITCH) + P600_CLICK + ofs, Sw_P600_02Location + _V(UR.x * P600_SWITCH, UR.y * P600_SWITCH, UR.z * P600_SWITCH) + P600_CLICK + ofs, Sw_P600_02Location + _V(DL.x * P600_SWITCH, DL.y * P600_SWITCH, DL.z * P600_SWITCH) + P600_CLICK + ofs, Sw_P600_02Location + _V(DR.x * P600_SWITCH, DR.y * P600_SWITCH, DR.z * P600_SWITCH) + P600_CLICK + ofs);

	// ORDEAL Rotary

	oapiVCRegisterArea(AID_VC_ORDEAL_ROT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ORDEAL_ROT, _V(-0.92382, 1.00681, -0.08822) + ofs, _V(-0.905273, 0.989085, -0.048251) + ofs, _V(-0.959113, 0.975162, -0.08578) + ofs, _V(-0.941077, 0.957305, -0.046165) + ofs);

	// Altimeter
	oapiVCRegisterArea(AID_VC_ALTIMETER, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Panel 163

	UL = _V(-0.7893, 0.0001, 0.6139);
	UR = _V(0.7453, -0.0001, 0.6668);
	DL = _V(-0.7451, 0.0001, -0.6667);
	DR = _V(0.7892, -0.0001, -0.6138);

	oapiVCRegisterArea(AID_VC_SWITCH_P163_01, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P163_01 + i, Switch_P163Location + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + LEBFLOOR_CLICK + ofs, Switch_P163Location + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + LEBFLOOR_CLICK + ofs, Switch_P163Location + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + LEBFLOOR_CLICK + ofs, Switch_P163Location + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + LEBFLOOR_CLICK + ofs);

	// PLVC switch

	UL = _V(-0.0068, 0.7357, -0.6772);
	UR = _V(0.0067, 0.7326, 0.6807);
	DL = _V(-0.0070, -0.7324, -0.6805);
	DR = _V(0.0069, -0.7358, 0.6771);

	oapiVCRegisterArea(AID_VC_SWITCH_PLVC, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_PLVC + i, Switch_PLVCLocation + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + LEBLEFT_CLICK + ofs, Switch_PLVCLocation + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + LEBLEFT_CLICK + ofs, Switch_PLVCLocation + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + LEBLEFT_CLICK + ofs, Switch_PLVCLocation + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + LEBLEFT_CLICK + ofs);

	// RSI Needle
	oapiVCRegisterArea(AID_VC_EMS_RSI, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Panel 1

	UL = _V(-0.7744, 0.5996, 0.2017);
	UR = _V(0.7743, 0.5995, 0.2016);
	DL = _V(-0.7742, -0.5994, -0.2015);
	DR = _V(0.7745, -0.5997, -0.2018);

	for (i = 0; i < P1_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P1_01 + i, P1_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P1_3_CLICK + ofs, P1_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P1_3_CLICK + ofs, P1_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P1_3_CLICK + ofs, P1_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P1_3_CLICK + ofs);
	}

	for (i = 0; i < P1_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P1_01 + i, P1_ROT_POS[i] + ofs, ROT);
	}

	for (i = 0; i < P1_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P1_01 + i, P1_PUSHB_POS[i] + ofs, PUSHB);
	}

	oapiVCRegisterArea(AID_VC_TW_P1_01, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_TW_P1_01, _V(-0.6744, 0.4004, 0.2644) + ofs, _V(-0.6641, 0.4004, 0.2644) + ofs, _V(-0.6744, 0.3769, 0.2566) + ofs, _V(-0.6641, 0.3769, 0.2566) + ofs);

	oapiVCRegisterArea(AID_VC_TW_P1_02, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_TW_P1_02, _V(-0.6258, 0.3935, 0.2621) + ofs, _V(-0.6011, 0.3935, 0.2621) + ofs, _V(-0.6258, 0.3838, 0.2588) + ofs, _V(-0.6011, 0.3838, 0.2588) + ofs);

	oapiVCRegisterArea(AID_VC_EMS_DVSET, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_PRESSED | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_EMS_DVSET, _V(-0.521, 0.757, 0.3755) + ofs, _V(-0.4936, 0.757, 0.3755) + ofs, _V(-0.521, 0.7102, 0.3599) + ofs, _V(-0.4936, 0.7102, 0.3599) + ofs);

	oapiVCSetAreaClickmode_Spherical(AID_VC_MASTER_ALARM, _V(-0.775435, 0.709185, 0.361746) + ofs, PUSHB);

	oapiVCRegisterArea(AID_VC_ASCPROLL, PANEL_REDRAW_NEVER, PANEL_MOUSE_PRESSED | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ASCPROLL, _V(-0.993077, 0.360405, 0.251007) + ofs, _V(-0.982356, 0.360405, 0.251007) + ofs, _V(-0.993077, 0.339763, 0.244102) + ofs, _V(-0.982356, 0.339763, 0.244102) + ofs);

	oapiVCRegisterArea(AID_VC_ASCPPITCH, PANEL_REDRAW_NEVER, PANEL_MOUSE_PRESSED | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ASCPPITCH, _V(-0.993077, 0.325064, 0.239184) + ofs, _V(-0.982356, 0.325064, 0.239184) + ofs, _V(-0.993077, 0.303916, 0.232109) + ofs, _V(-0.982356, 0.303916, 0.232109) + ofs);

	oapiVCRegisterArea(AID_VC_ASCPYAW, PANEL_REDRAW_NEVER, PANEL_MOUSE_PRESSED | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ASCPYAW, _V(-0.993077, 0.289921, 0.227431) + ofs, _V(-0.982356, 0.289921, 0.227431) + ofs, _V(-0.993077, 0.268953, 0.220424) + ofs, _V(-0.982356, 0.268953, 0.220424) + ofs);

	//Panel 2

	if (viewpos != SATVIEW_LOWER_CENTER) {
		for (i = 0; i < P2_SWITCHCOUNT; i++)
		{
			oapiVCRegisterArea(AID_VC_SWITCH_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
			oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P2_01 + i, P2_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P1_3_CLICK + ofs, P2_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P1_3_CLICK + ofs, P2_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P1_3_CLICK + ofs, P2_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P1_3_CLICK + ofs);
		}

		for (i = 0; i < 2; i++)
		{
			oapiVCRegisterArea(AID_VC_ROT_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
			oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P2_01 + i, P2_ROT_POS[i] + ofs, ROT);
		}

		for (i = 2; i < 4; i++)
		{
			oapiVCRegisterArea(AID_VC_ROT_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_UP);
			oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ROT_P2_01 + i, P2_ROT_POS[i] + UL * ROT + P1_3_CLICK + ofs, P2_ROT_POS[i] + UR * ROT + P1_3_CLICK + ofs, P2_ROT_POS[i] + DL * ROT + P1_3_CLICK + ofs, P2_ROT_POS[i] + DR * ROT + P1_3_CLICK + ofs);
		}
		
		for (i = 0; i < P2_PUSHBCOUNT; i++)
		{
			oapiVCRegisterArea(AID_VC_PUSHB_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
			oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P2_01 + i, P2_PUSHB_POS[i] + ofs, PUSHB);
		}

		for (i = 0; i < P2_TWCOUNT; i++)
		{
			oapiVCRegisterArea(AID_VC_TW_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
			oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P2_01 + i, P2_TW_POS[i] + P1_3_TWCLICK + ofs, TW);
		}

		oapiVCRegisterArea(AID_VC_POSTLDGVENTHANDLE, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_POSTLDGVENTHANDLE, _V(0.183459, 0.922476, 0.408289) + ofs, 0.02);
	}

	// Panel 3

	for (i = 0; i < P3_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P3_01 + i, P3_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P1_3_CLICK + ofs, P3_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P1_3_CLICK + ofs, P3_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P1_3_CLICK + ofs, P3_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P1_3_CLICK + ofs);
	}

	for (i = 0; i < P3_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P3_01 + i, P3_ROT_POS[i] + ofs, ROT);
	}

	for (i = 0; i < P3_TWCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_TW_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P3_01 + i, P3_TW_POS[i] + P1_3_TWCLICK + ofs, TW);
	}

	oapiVCSetAreaClickmode_Spherical(AID_VC_MASTER_ALARM2, _V(0.720346, 0.621423, 0.332349) + ofs, PUSHB);

	// Panel 4

	UL = _V(-0.7345, -0.1855, 0.6526);
	UR = _V(0.6124, 0.5193, 0.5958);
	DL = _V(-0.6122, -0.5195, -0.5960);
	DR = _V(0.7346, 0.1855, -0.6525);

	for (i = 0; i < P4_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P4_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P4_01 + i, P4_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P4_CLICK + ofs, P4_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P4_CLICK + ofs, P4_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P4_CLICK + ofs, P4_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P4_CLICK + ofs);
	}

	for (i = 0; i < P4_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P4_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P4_01 + i, P4_ROT_POS[i] + ofs, ROT);
	}

	for (i = 0; i < P4_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P4_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P4_01 + i, P4_CB_POS[i] + ofs, CB);
	}

	// Panel 5

	UL = _V(-0.4700, 0.4672, 0.7488);
	UR = _V(0.3115, 0.8008, -0.5110);
	DL = _V(-0.3117, -0.8010, 0.5112);
	DR = _V(0.4701, -0.4672, -0.7486);

	for (i = 0; i < P5_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P5_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P5_01 + i, P5_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P5_CLICK + ofs, P5_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P5_CLICK + ofs, P5_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P5_CLICK + ofs, P5_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P5_CLICK + ofs);
	}

	for (i = 0; i < P5_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P5_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ROT_P5_01 + i, P5_ROT_POS[i] + UL * ROT + P8_CLICK + ofs, P5_ROT_POS[i] + UR * ROT + P8_CLICK + ofs, P5_ROT_POS[i] + DL * ROT + P8_CLICK + ofs, P5_ROT_POS[i] + DR * ROT + P8_CLICK + ofs);
	}

	for (i = 0; i < P5_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P5_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P5_01 + i, P5_CB_POS[i] + ofs, CB);
	}

	// Panel 6

	UL = _V(-0.6539, 0.0493, 0.7551);
	UR = _V(-0.1998, 0.8764, -0.4380);
	DL = _V(0.1998, -0.8762, 0.4382);
	DR = _V(0.6539, -0.0493, -0.7551);

	for (i = 0; i < P6_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P6_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P6_01 + i, P6_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P6_CLICK + ofs, P6_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P6_CLICK + ofs, P6_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P6_CLICK + ofs, P6_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P6_CLICK + ofs);
	}

	for (i = 0; i < P6_TWCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_TW_P6_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P6_01 + i, P6_TW_POS[i] + P6_TWCLICK + ofs, TW);
	}

	// Panel 7

	UL = _V(-0.6015, 0.5030, 0.6202);
	UR = _V(0.7591, -0.2076, 0.6171);
	DL = _V(-0.7589, 0.2074, -0.6169);
	DR = _V(0.6017, -0.5032, -0.6202);

	for (i = 0; i < P7_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P7_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P7_01 + i, P7_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P7_CLICK + ofs, P7_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P7_CLICK + ofs, P7_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P7_CLICK + ofs, P7_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P7_CLICK + ofs);
	}

	for (i = 0; i < P7_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P7_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P7_01 + i, P7_ROT_POS[i] + ofs, ROT);
	}

	// Panel 8

	UL = _V(-0.3310, 0.7798, -0.5312);
	UR = _V(0.4716, 0.4611, 0.7516);
	DL = _V(-0.4718, -0.4609, -0.7514);
	DR = _V(0.3312, -0.7798, 0.5314);

	for (i = 0; i < P8_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P8_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P8_01 + i, P8_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P8_CLICK + ofs, P8_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P8_CLICK + ofs, P8_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P8_CLICK + ofs, P8_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P8_CLICK + ofs);
	}

	for (i = 0; i < P8_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P8_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P8_01 + i, P8_CB_POS[i] + ofs, CB);
	}

	for (i = 0; i < P8_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P8_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ROT_P8_01 + i, P8_ROT_POS[i] + UL * ROT + P8_CLICK + ofs, P8_ROT_POS[i] + UR * ROT + P8_CLICK + ofs, P8_ROT_POS[i] + DL * ROT + P8_CLICK + ofs, P8_ROT_POS[i] + DR * ROT + P8_CLICK + ofs);

	}

	// Panel 9

	UL = _V(0.2232, 0.8634, -0.4522);
	UR = _V(0.6280, 0.0629, 0.7754);
	DL = _V(-0.6282, -0.0627, -0.7756);
	DR = _V(-0.2234, -0.8636, 0.4520);

	for (i = 0; i < P9_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P9_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P9_01 + i, P9_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P9_CLICK + ofs, P9_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P9_CLICK + ofs, P9_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P9_CLICK + ofs, P9_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P9_CLICK + ofs);
	}

	for (i = 0; i < P9_TWCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_TW_P9_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P9_01 + i, P9_TW_POS[i] + P9_TWCLICK + ofs, TW);
	}

	// Panel 10

	UL = _V(0.7582, -0.2587, 0.5984);
	UR = _V(-0.7643, -0.2555, 0.5919);
	DL = _V(0.7645, 0.2555, -0.5917);
	DR = _V(-0.7583, 0.2587, -0.5985);

	if (viewpos == SATVIEW_LOWER_CENTER) {

		for (i = 0; i < P10_SWITCHCOUNT; i++)
		{
			oapiVCRegisterArea(AID_VC_SWITCH_P10_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
			oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P10_01 + i, P10_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P10_CLICK + ofs, P10_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P10_CLICK + ofs, P10_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P10_CLICK + ofs, P10_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P10_CLICK + ofs);
		}

		for (i = 0; i < P10_TWCOUNT; i++)
		{
			oapiVCRegisterArea(AID_VC_TW_P10_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
			oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P10_01 + i, P10_TW_POS[i] + P10_TWCLICK + ofs, TW);
		}

		// Panel 12
		for (i = 0; i < P12_ROTCOUNT; i++)
		{
			oapiVCRegisterArea(AID_VC_ROT_P12_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
			oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P12_01 + i, P12_ROT_POS[i] + ofs, ROT);
		}
	}

	// Panel 13

	UL = _V(0.1911, 0.7208, -0.6662);
	UR = _V(0.7794, 0.1415, 0.6102);
	DL = _V(-0.7792, -0.1416, -0.6104);
	DR = _V(-0.1912, -0.7207, 0.6663);

	for (i = 0; i < P13_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P13_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P13_01 + i, P13_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P13_CLICK + ofs, P13_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P13_CLICK + ofs, P13_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P13_CLICK + ofs, P13_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P13_CLICK + ofs);
	}

	// Panel 15

	UL = _V(0.2051, 0.7189, -0.6643);
	UR = _V(0.7548, 0.1557, 0.6370);
	DL = _V(-0.7546, -0.1559, -0.6372);
	DR = _V(-0.2049, -0.7187, 0.6641);

	for (i = 0; i < P15_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P15_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P15_01 + i, P15_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P15_CLICK + ofs, P15_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P15_CLICK + ofs, P15_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P15_CLICK + ofs, P15_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P15_CLICK + ofs);
	}

	// Panel 16

	UL = _V(-0.7573, 0.2406, 0.6071);
	UR = _V(-0.0903, 0.7413, -0.6647);
	DL = _V(0.0901, -0.7415, 0.6649);
	DR = _V(0.7574, -0.2405, -0.6071);

	for (i = 0; i < P16_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P16_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P16_01 + i, P16_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P16_CLICK + ofs, P16_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P16_CLICK + ofs, P16_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P16_CLICK + ofs, P16_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P16_CLICK + ofs);
	}

	// Panel 100

	UL = _V(-0.7893, 0.0001, 0.6139);
	UR = _V(0.7453, -0.0001, 0.6668);
	DL = _V(-0.7451, 0.0001, -0.6667);
	DR = _V(0.7892, -0.0001, -0.6138);

	for (i = 0; i < P100_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P100_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P100_01 + i, P100_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + LEBFLOOR_CLICK + ofs, P100_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + LEBFLOOR_CLICK + ofs, P100_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + LEBFLOOR_CLICK + ofs, P100_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + LEBFLOOR_CLICK + ofs);
	}

	for (i = 0; i < P100_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P100_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ROT_P100_01 + i, P100_ROT_POS[i] + UL * ROT + LEBFLOOR_CLICK + ofs, P100_ROT_POS[i] + UR * ROT + LEBFLOOR_CLICK + ofs, P100_ROT_POS[i] + DL * ROT + LEBFLOOR_CLICK + ofs, P100_ROT_POS[i] + DR * ROT + LEBFLOOR_CLICK + ofs);
	}

	// Panel 101

	UL = _V(-0.2536, 0.3335, 0.9079);
	UR = _V(0.7609, 0.2271, 0.6076);
	DL = _V(-0.7609, -0.2271, -0.6076);
	DR = _V(0.2536, -0.3335, -0.9079);

	for (i = 0; i < P101_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P101_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P101_01 + i, P101_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P101_CLICK + ofs, P101_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P101_CLICK + ofs, P101_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P101_CLICK + ofs, P101_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P101_CLICK + ofs);
	}

	for (i = 0; i < P101_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P101_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P101_01 + i, P101_ROT_POS[i] + ofs, ROT);
	}

	//Panel 122

	UL = _V(-0.7893, 0.0001, 0.6139);
	UR = _V(0.7453, -0.0001, 0.6668);
	DL = _V(-0.7451, 0.0001, -0.6667);
	DR = _V(0.7892, -0.0001, -0.6138);

	for (i = 0; i < P122_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P122_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P122_01 + i, P122_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + LEBFLOOR_CLICK + ofs, P122_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + LEBFLOOR_CLICK + ofs, P122_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + LEBFLOOR_CLICK + ofs, P122_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + LEBFLOOR_CLICK + ofs);
	}

	for (i = 0; i < P122_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P122_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P122_01 + i, P122_PUSHB_POS[i] + ofs, PUSHB);
	}

	oapiVCSetAreaClickmode_Spherical(AID_VC_MASTER_ALARM3, _V(0.103894, -0.69915, 0.029394) + ofs, PUSHB);

	// Panel 306

	UL = _V(-0.0068, 0.7357, -0.6772);
	UR = _V(0.0067, 0.7326, 0.6807);
	DL = _V(-0.0070, -0.7324, -0.6805);
	DR = _V(0.0069, -0.7358, 0.6771);

	for (i = 0; i < P306_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P306_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Quadrilateral(AID_VC_SWITCH_P306_01 + i, P306_TOGGLE_POS[i] + _V(UL.x * SWITCH, UL.y * SWITCH, UL.z * SWITCH) + P306_CLICK + ofs, P306_TOGGLE_POS[i] + _V(UR.x * SWITCH, UR.y * SWITCH, UR.z * SWITCH) + P306_CLICK + ofs, P306_TOGGLE_POS[i] + _V(DL.x * SWITCH, DL.y * SWITCH, DL.z * SWITCH) + P306_CLICK + ofs, P306_TOGGLE_POS[i] + _V(DR.x * SWITCH, DR.y * SWITCH, DR.z * SWITCH) + P306_CLICK + ofs);
	}

	// Panel 325
	const VECTOR3 Prim_Gly_HandleLocation = { -1.1482, 0.9682, -0.2071 };

	oapiVCRegisterArea(AID_VC_Cab_Press_Rel_Handle1, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_Cab_Press_Rel_Handle1, _V(-1.1716, 0.797558, -0.2383) + ofs, _V(-1.26138, 0.797558, -0.225946) + ofs, _V(-1.17143, 0.749602, -0.2383) + ofs, _V(-1.26138, 0.749602, -0.225946) + ofs);

	oapiVCRegisterArea(AID_VC_Cab_Press_Rel_Handle2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_Cab_Press_Rel_Handle2, _V(-1.10395, 0.716629, -0.2383) + ofs, _V(-1.26248, 0.716629, -0.218411) + ofs, _V(-1.10413, 0.664093, -0.2383) + ofs, _V(-1.26248, 0.664093, -0.218411) + ofs);

	oapiVCRegisterArea(AID_VC_Prim_Gly_Handle, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_Prim_Gly_Handle, Prim_Gly_HandleLocation + ofs, 0.02);

	// Panel 326
	for (i = 0; i < P326_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P326_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P326_01 + i, P326_ROT_POS[i] + ofs, ROT);
	}

	// LEB Right wall

	for (i = 0; i < LEB_R_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_LEB_R_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_LEB_R_01 + i, LEB_R_ROT_POS[i] + ofs, ROT);
	}

	for (i = 0; i < LEB_R1_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_LEB_R1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_LEB_R1_01 + i, LEB_R1_CB_POS[i] + ofs, CB);
	}

	for (i = 0; i < LEB_R2_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_LEB_R2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_LEB_R2_01 + i, LEB_R2_CB_POS[i] + ofs, CB);
	}

	// LEB Left wall

	for (i = 0; i < 21; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_LEB_L_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_LEB_L_01 + i, LEB_L_ROT_POS[i] + ofs, ROT);
	}

	// Rotary 22 (Suit Test) needs a different click-spot then the mesh origin
	oapiVCRegisterArea(AID_VC_ROT_LEB_L_22, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_LEB_L_22, _V(-1.07633, 0.156535, -0.281039) + ofs, ROT);

	for (i = 0; i < LEB_L_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_LEB_L_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_LEB_L_01 + i, LEB_L_PUSHB_POS[i] + ofs, 0.012);
	}

	//Cue Cards

	// Above the DSKY for the DAP Monitor Card
	oapiVCRegisterArea(AID_VC_CUE_CARD_LOCATION_1, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_CUE_CARD_LOCATION_1, _V(-0.329979, 0.682787, 0.352857) + ofs, _V(-0.257461, 0.682787, 0.352857) + ofs, _V(-0.329979, 0.673671, 0.349805) + ofs, _V(-0.257461, 0.673671, 0.349805) + ofs);

	// Left of the DSKY, Boost and TLI Cards
	oapiVCRegisterArea(AID_VC_CUE_CARD_LOCATION_2, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_CUE_CARD_LOCATION_2, _V(-0.345, 0.6657, 0.3439) + ofs, _V(-0.332, 0.6657, 0.3439) + ofs, _V(-0.345, 0.5775, 0.3141) + ofs, _V(-0.332, 0.5775, 0.3141) + ofs);

	// Around the event timer, SPS burn card and others
	oapiVCRegisterArea(AID_VC_CUE_CARD_LOCATION_3, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_CUE_CARD_LOCATION_3, _V(-0.561500, 0.607183, 0.327275) + ofs, _V(-0.511500, 0.607183, 0.327275) + ofs, _V(-0.561500, 0.588217, 0.320925) + ofs, _V(-0.511500, 0.588217, 0.320925) + ofs);

	// Left of FDAI 2 and DSKY (Velcro patch 1)
	oapiVCRegisterArea(AID_VC_CUE_CARD_LOCATION_4A, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_CUE_CARD_LOCATION_4A, _V(-0.454700, 0.785408, 0.378859) + ofs, _V(-0.434700, 0.785408, 0.378859) + ofs,
		_V(-0.454700, 0.766441, 0.372514) + ofs, _V(-0.434700, 0.766441, 0.372514) + ofs);

	// Left of FDAI 2 and DSKY (Velcro patch 2)
	oapiVCRegisterArea(AID_VC_CUE_CARD_LOCATION_4B, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_CUE_CARD_LOCATION_4B, _V(-0.454700, 0.652641, 0.334441) + ofs, _V(-0.434700, 0.652641, 0.334441) + ofs,
		_V(-0.454700, 0.515132, 0.288437) + ofs, _V(-0.434700, 0.515132, 0.288437) + ofs);

	// Above EMS
	oapiVCRegisterArea(AID_VC_CUE_CARD_LOCATION_5, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_CUE_CARD_LOCATION_5, _V(-0.601200, 0.804300, 0.367064) + ofs, _V(-0.561200, 0.804300, 0.367064) + ofs, _V(-0.601200, 0.775850, 0.357546) + ofs, _V(-0.561200, 0.775850, 0.357546) + ofs);

	// Above fuel cell meters
	oapiVCRegisterArea(AID_VC_CUE_CARD_LOCATION_6, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_CUE_CARD_LOCATION_6, _V(0.564600, 0.881316, 0.416091) + ofs, _V(0.614600, 0.881316, 0.416091) + ofs, _V(0.564600, 0.852866, 0.406573) + ofs, _V(0.614600, 0.852866, 0.406573) + ofs);

	// Landing
	oapiVCRegisterArea(AID_VC_CUE_CARD_LOCATION_7, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_CUE_CARD_LOCATION_7, _V(0.692700, 0.821967, 0.386709) + ofs, _V(0.712700, 0.821967, 0.386709) + ofs, _V(0.692700, 0.765066, 0.367673) + ofs, _V(0.712700, 0.765066, 0.367673) + ofs);

	// Right of DC meters
	oapiVCRegisterArea(AID_VC_CUE_CARD_LOCATION_8, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_CUE_CARD_LOCATION_8, _V(0.886400, 0.596067, 0.320209) + ofs, _V(0.906400, 0.596067, 0.320209) + ofs, _V(0.886400, 0.577100, 0.313864) + ofs, _V(0.906400, 0.577100,	0.313864) + ofs);

	// Right of ECS meters
	oapiVCRegisterArea(AID_VC_CUE_CARD_LOCATION_9, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_CUE_CARD_LOCATION_9, _V(0.469300, 0.678658, 0.347814) + ofs, _V(0.489300, 0.678658, 0.347814) + ofs, _V(0.469300, 0.607533, 0.324019) + ofs, _V(0.489300, 0.607533, 0.324019) + ofs);

	// Below ECS meters
	oapiVCRegisterArea(AID_VC_CUE_CARD_LOCATION_10, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_CUE_CARD_LOCATION_10, _V(0.419100, 0.584558, 0.312614) + ofs, _V(0.439100, 0.584558, 0.312614) + ofs, _V(0.419100, 0.494466, 0.282473) + ofs, _V(0.439100, 0.494466, 0.282473) + ofs);

	// Antenna locations
	oapiVCRegisterArea(AID_VC_CUE_CARD_LOCATION_11, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_CUE_CARD_LOCATION_11, _V(0.344100, 0.560850, 0.304682) + ofs, _V(0.364100, 0.560850, 0.304682) + ofs, _V(0.344100, 0.541883, 0.298337) + ofs, _V(0.364100, 0.541883, 0.298337) + ofs);
}

// --------------------------------------------------------------
// Respond to virtual cockpit mouse event
// --------------------------------------------------------------
bool Saturn::clbkVCMouseEvent (int id, int event, VECTOR3 &p)
{
	TRACESETUP("Saturn::clbkVCMouseEvent");
	switch (id) {

	case AID_VC_MASTER_ALARM:
	case AID_VC_MASTER_ALARM2:
	case AID_VC_MASTER_ALARM3:

		return cws.CheckMasterAlarmMouseClick(event);

	case AID_VC_EMS_DVSET:
		return EMSDvSetSwitch.CheckMouseClickVC(event, p);

	case AID_VC_ASCPROLL:
	{
		int mx = 0;
		int my;
		if (p.y > 0.5) {
			my = 19;
		}
		else {
			my = 17;
		}
		ascp.RollClick(event, mx, my);
		return true;
	}

	case AID_VC_ASCPPITCH:
	{
		int mx = 0;
		int my;
		if (p.y > 0.5) {
			my = 19;
		}
		else {
			my = 17;
		}
		ascp.PitchClick(event, mx, my);
		return true;
	}
	case AID_VC_ASCPYAW:
	{
		int mx = 0;
		int my;
		if (p.y > 0.5) {
			my = 19;
		}
		else {
			my = 17;
		}
		ascp.YawClick(event, mx, my);
		return true;
	}
	case AID_VC_SEATSCYCLE:
	{
		if (VCSeatsfolded) {
			VCSeatsfolded = false;
		}
		else {
			VCSeatsfolded = true;
		}

		SwitchClick();
		SetVCSeatsMesh();
		return true;
	}

	case AID_VC_FWDHATCH_HANDLE:
		ForwardHatch.Toggle();
		return true;

	case AID_VC_COAS:
		if (coasEnabled) {
			coasEnabled = false;
		} else {
			coasEnabled = true;
		}
		SwitchClick();
		SetCOASMesh();
		return true;

	case AID_VC_CUE_CARD_LOCATION_4A:
	case AID_VC_CUE_CARD_LOCATION_4B:
		CueCards.CycleCueCard(3);
		return true;
	case AID_VC_CUE_CARD_LOCATION_1:
	case AID_VC_CUE_CARD_LOCATION_2:
	case AID_VC_CUE_CARD_LOCATION_3:
	case AID_VC_CUE_CARD_LOCATION_5:
	case AID_VC_CUE_CARD_LOCATION_6:
	case AID_VC_CUE_CARD_LOCATION_7:
	case AID_VC_CUE_CARD_LOCATION_8:
	case AID_VC_CUE_CARD_LOCATION_9:
	case AID_VC_CUE_CARD_LOCATION_10:
	case AID_VC_CUE_CARD_LOCATION_11:
		CueCards.CycleCueCard(id - AID_VC_CUE_CARD_LOCATION_1);
		return true;
	}

	return MainPanelVC.VCMouseEvent(id, event, p);
	//return false;
}

// --------------------------------------------------------------
// Respond to virtual cockpit area redraw request
// --------------------------------------------------------------
bool Saturn::clbkVCRedrawEvent (int id, int event, SURFHANDLE surf)
{
	TRACESETUP("Saturn::clbkVCRedrawEvent");
	//int i;
	SetCameraCatchAngle(5.0*RAD);

	switch (id) {
	//case areaidentifier
	//	Redraw Panel stuff
	//	return true if dynamic texture modified, false if not
	
#ifdef _OPENORBITER
	case AID_VC_SWITCH_P13_04: // CMVC Ordeal Lighting Switch
        SetCMVCIntegralLight(vcidx, IntegralLights_CMVC_Ordeal, MatProp::Emission, ordeal.LightingPower(), NUM_ELEMENTS(IntegralLights_CMVC_Ordeal));
		ORDEALLightingSwitch.DrawSwitchVC(id, event, surf);
		return true;

	case AID_VC_INTEGRAL_LIGHT_P8:
        SetCMVCIntegralLight(vcidx, IntegralLights_P8, MatProp::Emission, IntegralRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(IntegralLights_P8));
        SetCMVCIntegralLight(vcidx, IntergralLights_P8_NTex, MatProp::Light,IntegralRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(IntergralLights_P8_NTex));
        return true;

	case AID_VC_FLOOD_LIGHT_P8:
        SetCMVCIntegralLight(vcidx, FloodLights_P8, MatProp::Light,FloodRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(FloodLights_P8));
		SetCMVCIntegralLight(seatsunfoldedidx, CMVCSeatsUnFolded, MatProp::Light, FloodRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(CMVCSeatsUnFolded));
		SetCMVCIntegralLight(seatsfoldedidx, CMVCSeatsFolded, MatProp::Light, FloodRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(CMVCSeatsFolded));
		SetCMVCIntegralLight(coascdridx, CMVC_COAS_CDR, MatProp::Light, FloodRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(CMVC_COAS_CDR));

        return true;

	case AID_VC_NUMERICS_LIGHT_P8:
//        SetCMVCIntegralLight(vcidx,NumericLights_P8, MatProp::Light, NumericRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(NumericLights_P8));
        SetCMVCIntegralLight(vcidx, NumericLights_P8_NTex, MatProp::Light, NumericRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(NumericLights_P8_NTex));
        return true;

	case AID_VC_INTEGRAL_LIGHT_P5:
        SetCMVCIntegralLight(vcidx, IntegralLights_P5, MatProp::Emission, RightIntegralRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(IntegralLights_P5));
        return true;

	case AID_VC_FLOOD_LIGHT_P5:
//		SetCMVCIntegralLight(vcidx, FloodLights_P5, MatProp::Light,RightFloodRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(FloodLights_P5));
        return true;

	case AID_VC_INTEGRAL_LIGHT_P100:
        SetCMVCIntegralLight(vcidx, IntegralLights_P100, MatProp::Emission, Panel100IntegralRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(IntegralLights_P100));
        return true;

	case AID_VC_FLOOD_LIGHT_P100:
//		SetCMVCIntegralLight(vcidx, FloodLights_P100, MatProp::Light, Panel100FloodRotarySwitch.GetValue*0.01, NUM_ELEMENTS(FloodLights_P100));
        return true;

	case AID_VC_NUMERICS_LIGHT_P100:
		SetCMVCIntegralLight(vcidx, NumericLights_P100, MatProp::Light, Panel100NumericRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(NumericLights_P100));
		return true;
	case AID_VC_CUE_CARDS_LIGHTING:
	{
		//Get list of mesh indices
		std::vector<UINT> indices;
		CueCards.GetMeshIndexList(indices);
		//Assume cue cards only have material 0
		DWORD ccmat[1] = { 0 };

		for (unsigned i = 0; i < indices.size(); i++)
		{
			SetCMVCIntegralLight(indices[i], ccmat, MatProp::Light, FloodRotarySwitch.GetValue()*0.1, 1);
		}

		return true;
	}
#else
	case AID_VC_SWITCH_P13_04: // CMVC Ordeal Lighting Switch
        SetCMVCIntegralLight(vcidx, IntegralLights_CMVC_Ordeal, MESHM_EMISSION2, ordeal.LightingPower(), NUM_ELEMENTS(IntegralLights_CMVC_Ordeal));
		ORDEALLightingSwitch.DrawSwitchVC(id, event, surf);
		return true;

	case AID_VC_INTEGRAL_LIGHT_P8:
        SetCMVCIntegralLight(vcidx, IntegralLights_P8, MESHM_EMISSION2, IntegralRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(IntegralLights_P8));
		SetCMVCIntegralLight(vcidx, IntergralLights_P8_NTex, MESHM_EMISSION,IntegralRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(IntergralLights_P8_NTex));
        return true;

	case AID_VC_FLOOD_LIGHT_P8:
		SetCMVCIntegralLight(vcidx, FloodLights_P8, MESHM_EMISSION, FloodRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(FloodLights_P8));
		SetCMVCIntegralLight(seatsunfoldedidx, CMVCSeatsUnFolded, MESHM_EMISSION, FloodRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(CMVCSeatsUnFolded));
		SetCMVCIntegralLight(seatsfoldedidx, CMVCSeatsFolded, MESHM_EMISSION, FloodRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(CMVCSeatsFolded));
		SetCMVCIntegralLight(coascdridx, CMVC_COAS_CDR, MESHM_EMISSION, FloodRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(CMVC_COAS_CDR));
        return true;

	case AID_VC_NUMERICS_LIGHT_P8:
//        SetCMVCIntegralLight(vcidx,NumericLights_P8, MESHM_EMISSION,NumericRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(NumericLights_P8));
		SetCMVCIntegralLight(vcidx, NumericLights_P8_NTex, MESHM_EMISSION, NumericRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(NumericLights_P8_NTex));
        return true;

	case AID_VC_INTEGRAL_LIGHT_P5:
        SetCMVCIntegralLight(vcidx, IntegralLights_P5, MESHM_EMISSION2, RightIntegralRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(IntegralLights_P5));
        return true;

	case AID_VC_FLOOD_LIGHT_P5:
//		SetCMVCIntegralLight(vcidx, FloodLights_P5, MESHM_EMISSION, RightFloodRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(FloodLights_P5));
		return true;

	case AID_VC_INTEGRAL_LIGHT_P100:
		SetCMVCIntegralLight(vcidx, IntegralLights_P100, MESHM_EMISSION2, Panel100IntegralRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(IntegralLights_P100));
		return true;

	case AID_VC_FLOOD_LIGHT_P100:
//		SetCMVCIntegralLight(vcidx, FloodLights_P100, MESHM_EMISSION, Panel100FloodRotarySwitch.GetValue()*0.1, NUM_ELEMENTS(FloodLights_P100));
        return true;

	case AID_VC_NUMERICS_LIGHT_P100:
		SetCMVCIntegralLight(vcidx, NumericLights_P100, MESHM_EMISSION, Panel100NumericRotarySwitch.GetValue() / 10.0, NUM_ELEMENTS(NumericLights_P100));
		return true;


	case AID_VC_CUE_CARDS_LIGHTING:
	{
		//Get list of mesh indices
		std::vector<UINT> indices;
		CueCards.GetMeshIndexList(indices);
		//Assume cue cards only have material 0
		DWORD ccmat[1] = { 0 };

		for (unsigned i = 0; i < indices.size(); i++)
		{
			SetCMVCIntegralLight(indices[i], ccmat, MESHM_EMISSION, FloodRotarySwitch.GetValue()*0.1, 1);
		}

		return true;
	}
#endif

	case AID_VC_FDAI_LEFT:
	{
		VECTOR3 euler_rates;
		VECTOR3 errors;

		euler_rates = eda.GetFDAI1AttitudeRate();
		errors = eda.GetFDAI1AttitudeError();

		fdaiLeft.AnimateFDAI(euler_rates, errors, anim_fdaiR_L, anim_fdaiP_L, anim_fdaiY_L, anim_fdaiRerror_L, anim_fdaiPerror_L, anim_fdaiYerror_L, anim_fdaiRrate_L, anim_fdaiPrate_L, anim_fdaiYrate_L);

		return true;
	}

	case AID_VC_FDAI_RIGHT:
	{
		VECTOR3 euler_rates;
		VECTOR3 errors;

		euler_rates = eda.GetFDAI2AttitudeRate();
		errors = eda.GetFDAI2AttitudeError();

		fdaiRight.AnimateFDAI(euler_rates, errors, anim_fdaiR_R, anim_fdaiP_R, anim_fdaiY_R, anim_fdaiRerror_R, anim_fdaiPerror_R, anim_fdaiYerror_R, anim_fdaiRrate_R, anim_fdaiPrate_R, anim_fdaiYrate_R);

		return true;
	}

	case AID_VC_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[SRF_VC_DIGITALDISP], srf[SRF_VC_DSKYDISP], 0, 0, TexMul);
		return true;

	case AID_VC_DSKY_DISPLAY2:
		dsky2.RenderData(surf, srf[SRF_VC_DIGITALDISP], srf[SRF_VC_DSKYDISP], 0, 0, TexMul);
		return true;

	case AID_VC_DSKY_LIGHTS:
		dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS], 0, 0, true, false, TexMul);
		return true;

	case AID_VC_DSKY_LIGHTS2:
		dsky2.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS], 0, 0, true, false, TexMul);
		return true;

	case AID_VC_ABORT_BUTTON:
		if (AbortLightLogic()) {
			oapiBlt(surf, srf[SRF_VC_ABORT], 0, 0, 62 * TexMul, 0, 62 * TexMul, 31 * TexMul);
		}
		else {
			oapiBlt(surf,srf[SRF_VC_ABORT], 0, 0, 0, 0, 62 * TexMul, 31 * TexMul);
		}
		return true;
		
	case AID_VC_MASTER_ALARM:
		cws.RenderMasterAlarm(surf, srf[SRF_VC_MASTERALARM], NULL, CWS_MASTERALARMPOSITION_LEFT, TexMul);
		return true;

	case AID_VC_MASTER_ALARM2:
		cws.RenderMasterAlarm(surf, srf[SRF_VC_MASTERALARM], NULL, CWS_MASTERALARMPOSITION_RIGHT, TexMul);
		return true;

	case AID_VC_MASTER_ALARM3:
		cws.RenderMasterAlarm(surf, srf[SRF_VC_MASTERALARM], NULL, CWS_MASTERALARMPOSITION_NONE, TexMul);
		return true;

	case AID_VC_LVENG_LIGHTS:
	{
		if (SI_EngineNum > 5)
		{
			RenderS1bEngineLight(ENGIND[0], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 64, 54, TexMul);
			RenderS1bEngineLight(ENGIND[1], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 64, 111, TexMul);
			RenderS1bEngineLight(ENGIND[2], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 4, 111, TexMul);
			RenderS1bEngineLight(ENGIND[3], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 4, 54, TexMul);
			RenderS1bEngineLight(ENGIND[4], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 34, 54, TexMul);
			RenderS1bEngineLight(ENGIND[5], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 50, 84, TexMul);
			RenderS1bEngineLight(ENGIND[6], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 34, 111, TexMul);
			RenderS1bEngineLight(ENGIND[7], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 19, 84, TexMul);
		}
		else
		{
			if (ENGIND[0])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 53*TexMul, 47*TexMul, 53*TexMul, 47*TexMul, 27*TexMul, 27*TexMul);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 53*TexMul, 47*TexMul, 155*TexMul, 47*TexMul, 27*TexMul, 27*TexMul);
			}

			if (ENGIND[1])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 53*TexMul, 103*TexMul, 53*TexMul, 103*TexMul, 27*TexMul, 27*TexMul);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 53*TexMul, 103*TexMul, 155*TexMul, 103*TexMul, 27*TexMul, 27*TexMul);
			}
			if (ENGIND[2])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 22*TexMul, 103*TexMul, 22*TexMul, 103*TexMul, 27*TexMul, 27*TexMul);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 22*TexMul, 103*TexMul, 124*TexMul, 103*TexMul, 27*TexMul, 27*TexMul);
			}
			if (ENGIND[3])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 22*TexMul, 47*TexMul, 22*TexMul, 47*TexMul, 27*TexMul, 27*TexMul);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 22*TexMul, 47*TexMul, 124*TexMul, 47*TexMul, 27*TexMul, 27*TexMul);
			}
			if (ENGIND[4])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 37*TexMul, 75*TexMul, 37*TexMul, 75*TexMul, 27*TexMul, 27*TexMul);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 37*TexMul, 75*TexMul, 139*TexMul, 75*TexMul, 27*TexMul, 27*TexMul);
			}
		}
	}

	if (LVRateLight)
	{
		oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 6*TexMul, 5*TexMul, 6*TexMul, 5*TexMul, 27*TexMul, 27*TexMul);
	}
	else
	{
		oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 6*TexMul, 5*TexMul, 108*TexMul, 5*TexMul, 27*TexMul, 27*TexMul);
	}

	//
	// Saturn 1b doesn't have an SII sep light.
	//

	if (SaturnType == SAT_SATURNV)
	{
		if (SIISepState)
		{
			oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 37*TexMul, 5*TexMul, 37*TexMul, 5*TexMul, 27*TexMul, 27*TexMul);
		}
		else
		{
			oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 37*TexMul, 5*TexMul, 139*TexMul, 5*TexMul, 27*TexMul, 27*TexMul);
		}
	}

	if (LVGuidLight)
	{
		oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 68*TexMul, 5*TexMul, 68*TexMul, 5*TexMul, 27*TexMul, 27*TexMul);
	}
	else
	{
		oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 68*TexMul, 5*TexMul, 170*TexMul, 5*TexMul, 27*TexMul, 27*TexMul);
	}
	return true;

	case AID_VC_MISSION_CLOCK:
		MissionTimerDisplay.Render(surf, srf[SRF_VC_DIGITALDISP2], true, TexMul);
		return true;

	case AID_VC_MISSION_CLOCK306:
		MissionTimer306Display.Render90(surf, srf[SRF_VC_DIGITAL90], true, TexMul);
		return true;

	case AID_VC_EVENT_TIMER306:
		EventTimer306Display.Render90(surf, srf[SRF_VC_EVENT_TIMER_DIGITS90], TexMul);
		return true;

	case AID_VC_EVENT_TIMER:
		EventTimerDisplay.Render(surf, srf[SRF_VC_EVENT_TIMER_DIGITS], TexMul);
		return true;

	case AID_VC_CWS_LIGHTS_LEFT:
		cws.RenderLights(surf, srf[SRF_VC_CW_LIGHTS], true, TexMul);
		return true;

	case AID_VC_CWS_LIGHTS_RIGHT:
		cws.RenderLights(surf, srf[SRF_VC_CW_LIGHTS], false, TexMul);
		return true;

	case AID_VC_CWS_GNLIGHTS:
		cws.RenderGNLights(surf, srf[SRF_VC_CWS_GNLIGHTS], TexMul);
		return true;

	case AID_VC_SEQUENCERSWITCHES:
		LiftoffNoAutoAbortSwitch.RepaintSwitchVC(surf, srf[SRF_VC_SEQUENCERSWITCHES], TexMul);
		return true;

	case AID_VC_SPS_LIGHT:
		if (ems.SPSThrustLight()) {
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 0, 0, 41*TexMul, 16*TexMul, 41*TexMul, 16*TexMul);
		}
		else {
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 0, 0, 0, 16*TexMul, 41*TexMul, 16*TexMul);
		}
		return true;

	case AID_VC_PT05G_LIGHT:
		if (ems.pt05GLight()) {
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 0, 0, 41*TexMul, 0, 41*TexMul, 16*TexMul);
		}
		else {
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 0, 0, 0, 0, 41*TexMul, 16*TexMul);
		}
		return true;


	case AID_VC_EMSDVDISPLAY:
		EMSDvDisplay.DoDrawSwitchVC(surf, EMSDvDisplay.QueryValue(), srf[SRF_VC_DIGITALDISP]);
		return true;

	case AID_VC_EMS_SCROLL_LEO:
	{
		oapi::Sketchpad* skp = oapiGetSketchpad(srf[SRF_VC_EMS_SCROLL_LEO]);

		skp->SetBackgroundMode(oapi::Sketchpad::BK_TRANSPARENT);
		skp->SetPen(g_Param.pen[2]);

		skp->Polyline(ems.ScribePntArrayVC, ems.ScribePntCnt);

		oapiReleaseSketchpad(skp);

		oapiBlt(surf, srf[SRF_VC_EMS_SCROLL_LEO], 5*TexMul, 4*TexMul, ems.GetScrollOffset()*TexMul, 0, 132*TexMul, 143*TexMul);
		oapiBlt(surf, srf[SRF_VC_EMS_SCROLL_BUG], 42*TexMul, (ems.GetGScribe() + 2)*TexMul, 0, 0, 5*TexMul, 5*TexMul, SURF_PREDEF_CK);
		oapiBlt(surf, srf[SRF_VC_EMS_SCROLL_BORDER], 0, 0, 0, 0, 142*TexMul, 150*TexMul, SURF_PREDEF_CK);
		return true;
	}

	case AID_VC_EMS_RSI_BKGRND:
	{
		switch (ems.LiftVectLight()) {
		case 1:
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 33*TexMul, 8*TexMul, 82*TexMul, 6*TexMul, 20*TexMul, 6*TexMul);
			break;
		case -1:
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 32*TexMul, 69*TexMul, 82*TexMul, 22*TexMul, 22*TexMul, 10*TexMul);
			break;
		case 0:
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 33*TexMul, 8*TexMul, 82*TexMul, 0, 20*TexMul, 6*TexMul);
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 32*TexMul, 69*TexMul, 82*TexMul, 12*TexMul, 22*TexMul, 10*TexMul);
			break;
		}
	}

	case AID_VC_EMS_RSI:
		ems.DrawSwitchVC(id, event, surf);
		return true;

	case AID_VC_EMS_DVSET:
		EMSDvSetSwitch.DrawSwitchVC(id, event, surf);
		return true;

		// ASCP
	case AID_VC_ASCPDISPLAYROLL:
		ascp.PaintRollDisplay(surf, srf[SRF_VC_THUMBWHEEL_LARGEFONTSINV], TexMul);
		return true;

	case AID_VC_ASCPDISPLAYPITCH:
		ascp.PaintPitchDisplay(surf, srf[SRF_VC_THUMBWHEEL_LARGEFONTSINV], TexMul);
		return true;

	case AID_VC_ASCPDISPLAYYAW:
		ascp.PaintYawDisplay(surf, srf[SRF_VC_THUMBWHEEL_LARGEFONTSINV], TexMul);
		return true;

	case AID_VC_OPT_SHAFTDISPLAY:
		optics.PaintShaftDisplay(surf, srf[SRF_VC_THUMBWHEEL_LARGEFONTSINV], TexMul);
		return true;

	case AID_VC_OPT_TRUNDISPLAY:
		optics.PaintTrunnionDisplay(surf, srf[SRF_VC_THUMBWHEEL_LARGEFONTSINV], TexMul);
		return true;

	case AID_VC_ALTIMETER:
		Altimeter.DrawSwitchVC(id, event, surf);
		return true;

	/*default:
		return false;*/
	}

	return MainPanelVC.VCRedrawEvent(id, event, surf);
	//return false;
}

#define JostleAxis(offset, noiseamp) \
	j = ((double) ((rand() & 32767) - 16384) * noiseamp*sqdt) / 16384.0; \
	offset += j; \
	if(dtmul>1.0) \
		offset = 0.0; \
	else \
		offset = offset-dtmul*offset;

void Saturn::JostleViewpoint(double noiselat, double noiselon, double noisefreq, double dt, double accoffsx, double accoffsy, double accoffsz)
{	
	double j, sqdt=sqrt(dt), dtmul= noisefreq*dt;
	JostleAxis(NoiseOffsetx, noiselat);
	JostleAxis(NoiseOffsety, noiselat);
	JostleAxis(NoiseOffsetz, noiselon);

	ViewOffsetx = NoiseOffsetx + accoffsx;
	ViewOffsety = NoiseOffsety + accoffsy;
	ViewOffsetz = NoiseOffsetz + accoffsz;
	//sprintf(oapiDebugString(), "Offsx:%8.4lf Offsy:%8.4lf Offsz:%8.4lf accoffsz:%8.4lf ", ViewOffsetx, ViewOffsety, ViewOffsetz, accoffsz);

	if (ViewOffsetx > 0.20)
		ViewOffsetx = 0.20;
	if (ViewOffsetx < -0.20)
		ViewOffsetx = -0.20;

	if (ViewOffsety > 0.10)
		ViewOffsety = 0.10;
	if (ViewOffsety < -0.10)
		ViewOffsety = -0.10;

	if (ViewOffsetz > 0.2)
		ViewOffsetz = 0.2;
	if (ViewOffsetz < -0.2)
		ViewOffsetz = -0.2;

	SetView();
}

void Saturn::VCFreeCam(VECTOR3 dir, bool slow)
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

void Saturn::SetView()

{
	SetView(CurrentViewOffset, false);
}

void Saturn::SetView(double offset)

{
	SetView(offset, false);
}

void Saturn::SetView(bool update_direction)

{
	SetView(CurrentViewOffset, update_direction);
}

void Saturn::SetView(double offset, bool update_direction)

{
	VECTOR3 v;
	TRACESETUP("Saturn::SetView");
	CurrentViewOffset = offset;

	// VC Offset
	VECTOR3 ofs_vc = _V(0.0, 0.0, offset);
	if (vcidx != -1) {
		GetMeshOffset(vcidx, ofs_vc);
		ofs_vc.z = ofs_vc.z - 0.15;
	}

	// 
	// 2D panel 
	// Direction/rotation range is in clbkLoadPanel
	//
	if (InPanel) {
		if (PanelId == SATPANEL_LEFT_RNDZ_WINDOW) {
			v = _V(-0.605, 1.045, offset - 3.0); // Adjusted to line up with docking target

		} else if (PanelId == SATPANEL_LEFT_317_WINDOW) {
			v = _V(-0.710051624, 1.305366408, offset - 3.0);

		} else if (PanelId == SATPANEL_RIGHT_RNDZ_WINDOW) {
			v = _V(0.710051624, 1.305366408, offset - 3.0);

		} else if (PanelId == SATPANEL_HATCH_WINDOW) {
			v = _V(0, 0, offset + 1.0);

		} else if (PanelId == SATPANEL_TUNNEL) {
			// Calibrated for best view with forward hatch open
			v = _V(0, 0, offset + 0.4);

		} else {
			// "Calibrated" for optics cover jettison as seen through the scanning telescope 
			v = _V(0, 0, offset + 0.2);
		}
	} 

	//
	// Generic cockpit
	// Direction/rotation range is in clbkLoadGenericCockpit
	//
	else if (!InVC) {		
		v = _V(0, 0, offset - 3.0);
	} 

	//
	// Virtual cockpit
	//
	else {
		switch (viewpos) {
			case SATVIEW_LEFTSEAT:
				v = _V(-0.6, 0.85, ofs_vc.z + 0.1);
				v.x += vcFreeCamx;
				v.y += (cos(P1_3_TILT) * vcFreeCamy) + (-sin(P1_3_TILT) * vcFreeCamz);
				v.z += (sin(P1_3_TILT) * vcFreeCamy) + (cos(P1_3_TILT) * vcFreeCamz);
				break;

			case SATVIEW_CENTERSEAT:
				v = _V(0, 0.85, ofs_vc.z + 0.1);
				v.x += vcFreeCamx;
				v.y += (cos(P1_3_TILT) * vcFreeCamy) + (-sin(P1_3_TILT) * vcFreeCamz);
				v.z += (sin(P1_3_TILT) * vcFreeCamy) + (cos(P1_3_TILT) * vcFreeCamz);
				break;

			case SATVIEW_RIGHTSEAT:
				v = _V(0.6, 0.85, ofs_vc.z + 0.1);
				v.x += vcFreeCamx;
				v.y += (cos(P1_3_TILT) * vcFreeCamy) + (-sin(P1_3_TILT) * vcFreeCamz);
				v.z += (sin(P1_3_TILT) * vcFreeCamy) + (cos(P1_3_TILT) * vcFreeCamz);
				break;

			case SATVIEW_LEFTDOCK:
				v = _V(-0.6, 1.05, 0.1 + ofs_vc.z); // Adjusted to line up with LM docking target
				//v.x += vcFreeCamx;
				//v.y += vcFreeCamy;
				//v.z += vcFreeCamz;
				break;
			
			case SATVIEW_RIGHTDOCK:
				v = _V(0.6, 1.05, 0.1 + ofs_vc.z);
				//v.x += vcFreeCamx;
				//v.y += vcFreeCamy;
				//v.z += vcFreeCamz;
				break;

			case SATVIEW_GNPANEL:
				v = _V(-0.05, -0.15, 0.3 + ofs_vc.z);
				v.x += vcFreeCamx;
				v.y += -vcFreeCamz;
				v.z += vcFreeCamy;
				break;

			case SATVIEW_LEBLEFT:
				v = _V(-0.8, -0.5, ofs_vc.z - 0.4);
				v.x += -vcFreeCamz;
				v.y += vcFreeCamy;
				v.z += vcFreeCamx;
				break;

			case SATVIEW_LEBRIGHT:
				v = _V(0.8, -0.4, ofs_vc.z + 0.1);
				v.x += vcFreeCamz;
				v.y += vcFreeCamy;
				v.z += -vcFreeCamx;
				break;

			case SATVIEW_LOWER_CENTER:
				v = _V(0.0, 0.1, 0.65 + ofs_vc.z);
				//v.x += vcFreeCamx;
				//v.y += vcFreeCamz;
				//v.z += vcFreeCamy;
				break;

			case SATVIEW_UPPER_CENTER:
				v = _V(0, 1.35, ofs_vc.z - 0.0);
				v.x += -vcFreeCamx;
				v.y += vcFreeCamy;
				v.z += -vcFreeCamz;
				break;

			case SATVIEW_TUNNEL:
				v = _V(0.0, 0.0, 0.8 + ofs_vc.z - 0.1);
				v.x += vcFreeCamx;
				v.y += vcFreeCamy;
				v.z += vcFreeCamz;
				break;
		}

		if (update_direction) {
			SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
			if (viewpos == SATVIEW_GNPANEL) {
				SetCameraDefaultDirection(_V(0.0,-1.0, 0.0));
			} else if (viewpos == SATVIEW_LEBRIGHT) {
				SetCameraDefaultDirection(_V(1.0, 0.0, 0.0));
			} else if (viewpos == SATVIEW_LEBLEFT) {
				SetCameraDefaultDirection(_V(-1.0, 0.0, 0.0));
			} else if (viewpos == SATVIEW_LOWER_CENTER) {
				SetCameraDefaultDirection(_V(0.0, 1.0, 0.0), 180 * RAD);
			} else if (viewpos == SATVIEW_UPPER_CENTER) {
				SetCameraDefaultDirection(_V(0.0, 0.0, -1.0));
			} else if (viewpos == SATVIEW_LEFTSEAT || viewpos == SATVIEW_CENTERSEAT || viewpos == SATVIEW_RIGHTSEAT) {
				SetCameraDefaultDirection(_V(0.00, -sin(P1_3_TILT), cos(P1_3_TILT)));
			} else {
				SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			}
			oapiCameraSetCockpitDir(0,0);
		}

		v.x += ViewOffsetx * VibrationVisualizationMultiplier;
		v.y += ViewOffsety * VibrationVisualizationMultiplier;
		v.z += ViewOffsetz * VibrationVisualizationMultiplier;
	}

	SetCameraOffset(v - _V(currentCoG.x, currentCoG.y, 0.0)); //We already use the mesh offset in the z-axis

	//
	// FOV handling
	//

	if (!FovExternal && !GenericFirstTimestep) {
		if (InPanel && PanelId == SATPANEL_SEXTANT) { // Sextant
			if (!FovFixed) {
				FovSave = oapiCameraAperture();
				FovFixed = true;
			}
			oapiCameraSetAperture(atan(0.000029418*PanelPixelHeight)); //1.8deg FOV for 534px; tan(0.9*PI/180)/534 = 2.9418e-5

		}
		else if (InPanel && PanelId == SATPANEL_TELESCOPE) { // Telescope
			if (!FovFixed) {
				FovSave = oapiCameraAperture();
				FovFixed = true;
			}
			oapiCameraSetAperture(atan(0.0010811*PanelPixelHeight)); //60deg FOV for 534px; tan(30*PI/180)/534 = 1.0811e-3

		}
		else {
			if (FovFixed) {
				oapiCameraSetAperture(FovSave);
				FovFixed = false;
			}
		}
	}
}

void Saturn::InitVCAnimations() {

	anim_fdaiR_L = anim_fdaiR_R = -1;
	anim_fdaiP_L = anim_fdaiP_R = -1;
	anim_fdaiY_L = anim_fdaiY_R = -1;
	anim_fdaiRerror_L = anim_fdaiRerror_R = -1;
	anim_fdaiPerror_L = anim_fdaiPerror_R = -1;
	anim_fdaiYerror_L = anim_fdaiYerror_R = -1;
	anim_fdaiRrate_L = anim_fdaiRrate_R = -1;
	anim_fdaiPrate_L = anim_fdaiPrate_R = -1;
	anim_fdaiYrate_L = anim_fdaiYrate_R = -1;
}

void Saturn::DefineVCAnimations()

{
	MainPanelVC.ClearSwitches();

	// Panel 1

	MainPanelVC.AddSwitch(&EMSModeSwitch, AID_VC_SWITCH_P1_01);
	EMSModeSwitch.SetReference(P1_TOGGLE_POS[0], _V(1, 0, 0));
	EMSModeSwitch.DefineMeshGroup(VC_GRP_Sw_P1_01);

	MainPanelVC.AddSwitch(&CMCAttSwitch, AID_VC_SWITCH_P1_02);
	CMCAttSwitch.SetReference(P1_TOGGLE_POS[1], _V(1, 0, 0));
	CMCAttSwitch.DefineMeshGroup(VC_GRP_Sw_P1_02);

	MainPanelVC.AddSwitch(&FDAIScaleSwitch, AID_VC_SWITCH_P1_03);
	FDAIScaleSwitch.SetReference(P1_TOGGLE_POS[2], _V(1, 0, 0));
	FDAIScaleSwitch.DefineMeshGroup(VC_GRP_Sw_P1_03);

	MainPanelVC.AddSwitch(&FDAISelectSwitch, AID_VC_SWITCH_P1_04);
	FDAISelectSwitch.SetReference(P1_TOGGLE_POS[3], _V(1, 0, 0));
	FDAISelectSwitch.DefineMeshGroup(VC_GRP_Sw_P1_04);

	MainPanelVC.AddSwitch(&FDAISourceSwitch, AID_VC_SWITCH_P1_05);
	FDAISourceSwitch.SetReference(P1_TOGGLE_POS[4], _V(1, 0, 0));
	FDAISourceSwitch.DefineMeshGroup(VC_GRP_Sw_P1_05);

	MainPanelVC.AddSwitch(&FDAIAttSetSwitch, AID_VC_SWITCH_P1_06);
	FDAIAttSetSwitch.SetReference(P1_TOGGLE_POS[5], _V(1, 0, 0));
	FDAIAttSetSwitch.DefineMeshGroup(VC_GRP_Sw_P1_06);

	MainPanelVC.AddSwitch(&ManualAttRollSwitch, AID_VC_SWITCH_P1_07);
	ManualAttRollSwitch.SetReference(P1_TOGGLE_POS[6], _V(1, 0, 0));
	ManualAttRollSwitch.DefineMeshGroup(VC_GRP_Sw_P1_07);

	MainPanelVC.AddSwitch(&ManualAttPitchSwitch, AID_VC_SWITCH_P1_08);
	ManualAttPitchSwitch.SetReference(P1_TOGGLE_POS[7], _V(1, 0, 0));
	ManualAttPitchSwitch.DefineMeshGroup(VC_GRP_Sw_P1_08);

	MainPanelVC.AddSwitch(&ManualAttYawSwitch, AID_VC_SWITCH_P1_09);
	ManualAttYawSwitch.SetReference(P1_TOGGLE_POS[8], _V(1, 0, 0));
	ManualAttYawSwitch.DefineMeshGroup(VC_GRP_Sw_P1_09);

	MainPanelVC.AddSwitch(&LimitCycleSwitch, AID_VC_SWITCH_P1_10);
	LimitCycleSwitch.SetReference(P1_TOGGLE_POS[9], _V(1, 0, 0));
	LimitCycleSwitch.DefineMeshGroup(VC_GRP_Sw_P1_10);

	MainPanelVC.AddSwitch(&AttDeadbandSwitch, AID_VC_SWITCH_P1_11);
	AttDeadbandSwitch.SetReference(P1_TOGGLE_POS[10], _V(1, 0, 0));
	AttDeadbandSwitch.DefineMeshGroup(VC_GRP_Sw_P1_11);

	MainPanelVC.AddSwitch(&AttRateSwitch, AID_VC_SWITCH_P1_12);
	AttRateSwitch.SetReference(P1_TOGGLE_POS[11], _V(1, 0, 0));
	AttRateSwitch.DefineMeshGroup(VC_GRP_Sw_P1_12);

	MainPanelVC.AddSwitch(&TransContrSwitch, AID_VC_SWITCH_P1_13);
	TransContrSwitch.SetReference(P1_TOGGLE_POS[12], _V(1, 0, 0));
	TransContrSwitch.DefineMeshGroup(VC_GRP_Sw_P1_13);

	MainPanelVC.AddSwitch(&RotPowerNormal1Switch, AID_VC_SWITCH_P1_14);
	RotPowerNormal1Switch.SetReference(P1_TOGGLE_POS[13], _V(1, 0, 0));
	RotPowerNormal1Switch.DefineMeshGroup(VC_GRP_Sw_P1_14);

	MainPanelVC.AddSwitch(&RotPowerNormal2Switch, AID_VC_SWITCH_P1_15);
	RotPowerNormal2Switch.SetReference(P1_TOGGLE_POS[14], _V(1, 0, 0));
	RotPowerNormal2Switch.DefineMeshGroup(VC_GRP_Sw_P1_15);

	MainPanelVC.AddSwitch(&RotPowerDirect1Switch, AID_VC_SWITCH_P1_16);
	RotPowerDirect1Switch.SetReference(P1_TOGGLE_POS[15], _V(1, 0, 0));
	RotPowerDirect1Switch.DefineMeshGroup(VC_GRP_Sw_P1_16);

	MainPanelVC.AddSwitch(&RotPowerDirect2Switch, AID_VC_SWITCH_P1_17);
	RotPowerDirect2Switch.SetReference(P1_TOGGLE_POS[16], _V(1, 0, 0));
	RotPowerDirect2Switch.DefineMeshGroup(VC_GRP_Sw_P1_17);

	MainPanelVC.AddSwitch(&SCContSwitch, AID_VC_SWITCH_P1_18);
	SCContSwitch.SetReference(P1_TOGGLE_POS[17], _V(1, 0, 0));
	SCContSwitch.DefineMeshGroup(VC_GRP_Sw_P1_18);

	MainPanelVC.AddSwitch(&CMCModeSwitch, AID_VC_SWITCH_P1_19);
	CMCModeSwitch.SetReference(P1_TOGGLE_POS[18], _V(1, 0, 0));
	CMCModeSwitch.DefineMeshGroup(VC_GRP_Sw_P1_19);

	MainPanelVC.AddSwitch(&BMAGRollSwitch, AID_VC_SWITCH_P1_20);
	BMAGRollSwitch.SetReference(P1_TOGGLE_POS[19], _V(1, 0, 0));
	BMAGRollSwitch.DefineMeshGroup(VC_GRP_Sw_P1_20);

	MainPanelVC.AddSwitch(&BMAGPitchSwitch, AID_VC_SWITCH_P1_21);
	BMAGPitchSwitch.SetReference(P1_TOGGLE_POS[20], _V(1, 0, 0));
	BMAGPitchSwitch.DefineMeshGroup(VC_GRP_Sw_P1_21);

	MainPanelVC.AddSwitch(&BMAGYawSwitch, AID_VC_SWITCH_P1_22);
	BMAGYawSwitch.SetReference(P1_TOGGLE_POS[21], _V(1, 0, 0));
	BMAGYawSwitch.DefineMeshGroup(VC_GRP_Sw_P1_22);

	MainPanelVC.AddSwitch(&dVThrust1Switch, AID_VC_SWITCH_P1_23);
	dVThrust1Switch.SetReference(P1_TOGGLE_POS[22], P1_COVERS_POS[2], _V(1, 0, 0), _V(1, 0, 0));
	dVThrust1Switch.DefineMeshGroup(VC_GRP_Sw_P1_23, VC_GRP_SwitchCover_P1_03);

	MainPanelVC.AddSwitch(&dVThrust2Switch, AID_VC_SWITCH_P1_24);
	dVThrust2Switch.SetReference(P1_TOGGLE_POS[23], P1_COVERS_POS[3], _V(1, 0, 0), _V(1, 0, 0));
	dVThrust2Switch.DefineMeshGroup(VC_GRP_Sw_P1_24, VC_GRP_SwitchCover_P1_04);

	MainPanelVC.AddSwitch(&SCSTvcPitchSwitch, AID_VC_SWITCH_P1_25);
	SCSTvcPitchSwitch.SetReference(P1_TOGGLE_POS[24], _V(1, 0, 0));
	SCSTvcPitchSwitch.DefineMeshGroup(VC_GRP_Sw_P1_25);

	MainPanelVC.AddSwitch(&SCSTvcYawSwitch, AID_VC_SWITCH_P1_26);
	SCSTvcYawSwitch.SetReference(P1_TOGGLE_POS[25], _V(1, 0, 0));
	SCSTvcYawSwitch.DefineMeshGroup(VC_GRP_Sw_P1_26);

	MainPanelVC.AddSwitch(&Pitch1Switch, AID_VC_SWITCH_P1_27);
	Pitch1Switch.SetReference(P1_TOGGLE_POS[26], _V(1, 0, 0));
	Pitch1Switch.DefineMeshGroup(VC_GRP_Sw_P1_27);

	MainPanelVC.AddSwitch(&Pitch2Switch, AID_VC_SWITCH_P1_28);
	Pitch2Switch.SetReference(P1_TOGGLE_POS[27], _V(1, 0, 0));
	Pitch2Switch.DefineMeshGroup(VC_GRP_Sw_P1_28);

	MainPanelVC.AddSwitch(&Yaw1Switch, AID_VC_SWITCH_P1_29);
	Yaw1Switch.SetReference(P1_TOGGLE_POS[28], _V(1, 0, 0));
	Yaw1Switch.DefineMeshGroup(VC_GRP_Sw_P1_29);

	MainPanelVC.AddSwitch(&Yaw2Switch, AID_VC_SWITCH_P1_30);
	Yaw2Switch.SetReference(P1_TOGGLE_POS[29], _V(1, 0, 0));
	Yaw2Switch.DefineMeshGroup(VC_GRP_Sw_P1_30);

	MainPanelVC.AddSwitch(&CGSwitch, AID_VC_SWITCH_P1_31);
	CGSwitch.SetReference(P1_TOGGLE_POS[30], _V(1, 0, 0));
	CGSwitch.DefineMeshGroup(VC_GRP_Sw_P1_31);

	MainPanelVC.AddSwitch(&ELSLogicSwitch, AID_VC_SWITCH_P1_32);
	ELSLogicSwitch.SetReference(P1_TOGGLE_POS[31], P1_COVERS_POS[04], _V(1, 0, 0), _V(1, 0, 0));
	ELSLogicSwitch.DefineMeshGroup(VC_GRP_Sw_P1_32, VC_GRP_SwitchCover_P1_05);

	MainPanelVC.AddSwitch(&ELSAutoSwitch, AID_VC_SWITCH_P1_33);
	ELSAutoSwitch.SetReference(P1_TOGGLE_POS[32], _V(1, 0, 0));
	ELSAutoSwitch.DefineMeshGroup(VC_GRP_Sw_P1_33);

	MainPanelVC.AddSwitch(&CMRCSLogicSwitch, AID_VC_SWITCH_P1_34);
	CMRCSLogicSwitch.SetReference(P1_TOGGLE_POS[33], _V(1, 0, 0));
	CMRCSLogicSwitch.DefineMeshGroup(VC_GRP_Sw_P1_34);

	MainPanelVC.AddSwitch(&CMPropDumpSwitch, AID_VC_SWITCH_P1_35);
	CMPropDumpSwitch.SetReference(P1_TOGGLE_POS[34], P1_COVERS_POS[05], _V(1, 0, 0), _V(1, 0, 0));
	CMPropDumpSwitch.DefineMeshGroup(VC_GRP_Sw_P1_35, VC_GRP_SwitchCover_P1_06);

	MainPanelVC.AddSwitch(&CMPropPurgeSwitch, AID_VC_SWITCH_P1_36);
	CMPropPurgeSwitch.SetReference(P1_TOGGLE_POS[35], P1_COVERS_POS[06], _V(1, 0, 0), _V(1, 0, 0));
	CMPropPurgeSwitch.DefineMeshGroup(VC_GRP_Sw_P1_36, VC_GRP_SwitchCover_P1_07);

	MainPanelVC.AddSwitch(&IMUGuardedCageSwitch, AID_VC_SWITCH_P1_37);
	IMUGuardedCageSwitch.SetReference(P1_TOGGLE_POS[36], P1_COVERS_POS[01], _V(1, 0, 0), _V(1, 0, 0));
	IMUGuardedCageSwitch.DefineMeshGroup(VC_GRP_Sw_P1_37, VC_GRP_SwitchCover_P1_02);

	MainPanelVC.AddSwitch(&EMSRollSwitch, AID_VC_SWITCH_P1_38);
	EMSRollSwitch.SetReference(P1_TOGGLE_POS[37], _V(1, 0, 0));
	EMSRollSwitch.DefineMeshGroup(VC_GRP_Sw_P1_38);

	MainPanelVC.AddSwitch(&GSwitch, AID_VC_SWITCH_P1_39);
	GSwitch.SetReference(P1_TOGGLE_POS[38], _V(1, 0, 0));
	GSwitch.DefineMeshGroup(VC_GRP_Sw_P1_39);

	MainPanelVC.AddSwitch(&LVSPSPcIndicatorSwitch, AID_VC_SWITCH_P1_40);
	LVSPSPcIndicatorSwitch.SetReference(P1_TOGGLE_POS[39], _V(1, 0, 0));
	LVSPSPcIndicatorSwitch.DefineMeshGroup(VC_GRP_Sw_P1_40);

	MainPanelVC.AddSwitch(&LVFuelTankPressIndicatorSwitch, AID_VC_SWITCH_P1_41);
	LVFuelTankPressIndicatorSwitch.SetReference(P1_TOGGLE_POS[40], _V(1, 0, 0));
	LVFuelTankPressIndicatorSwitch.DefineMeshGroup(VC_GRP_Sw_P1_41);

	MainPanelVC.AddSwitch(&TVCGimbalDrivePitchSwitch, AID_VC_SWITCH_P1_42);
	TVCGimbalDrivePitchSwitch.SetReference(P1_TOGGLE_POS[41], _V(1, 0, 0));
	TVCGimbalDrivePitchSwitch.DefineMeshGroup(VC_GRP_Sw_P1_42);

	MainPanelVC.AddSwitch(&TVCGimbalDriveYawSwitch, AID_VC_SWITCH_P1_43);
	TVCGimbalDriveYawSwitch.SetReference(P1_TOGGLE_POS[42], _V(1, 0, 0));
	TVCGimbalDriveYawSwitch.DefineMeshGroup(VC_GRP_Sw_P1_43);

	MainPanelVC.AddSwitch(&EventTimerUpDownSwitch, AID_VC_SWITCH_P1_44);
	EventTimerUpDownSwitch.SetReference(P1_TOGGLE_POS[43], _V(1, 0, 0));
	EventTimerUpDownSwitch.DefineMeshGroup(VC_GRP_Sw_P1_44);

	MainPanelVC.AddSwitch(&EventTimerContSwitch, AID_VC_SWITCH_P1_45);
	EventTimerContSwitch.SetReference(P1_TOGGLE_POS[44], _V(1, 0, 0));
	EventTimerContSwitch.DefineMeshGroup(VC_GRP_Sw_P1_45);

	MainPanelVC.AddSwitch(&EventTimerMinutesSwitch, AID_VC_SWITCH_P1_46);
	EventTimerMinutesSwitch.SetReference(P1_TOGGLE_POS[45], _V(1, 0, 0));
	EventTimerMinutesSwitch.DefineMeshGroup(VC_GRP_Sw_P1_46);

	MainPanelVC.AddSwitch(&EventTimerSecondsSwitch, AID_VC_SWITCH_P1_47);
	EventTimerSecondsSwitch.SetReference(P1_TOGGLE_POS[46], _V(1, 0, 0));
	EventTimerSecondsSwitch.DefineMeshGroup(VC_GRP_Sw_P1_47);

	MainPanelVC.AddSwitch(&GTASwitch, AID_VC_SWITCH_P1_48);
	GTASwitch.SetReference(P1_TOGGLE_POS[47], P1_COVERS_POS[0], _V(1, 0, 0), _V(1, 0, 0));
	GTASwitch.DefineMeshGroup(VC_GRP_Sw_P1_48, VC_GRP_SwitchCover_P1_01);

	MainPanelVC.AddSwitch(&SPSswitch, AID_VC_SWITCH_P1_49);
	SPSswitch.SetReference(P1_TOGGLE_POS[48], _V(1, 0, 0));
	SPSswitch.DefineMeshGroup(VC_GRP_Sw_P1_49);

	VECTOR3	SEQS_COVER_AXIS = { -0.00, -sin(P1_3_TILT + (90.0 * RAD)), cos(P1_3_TILT + (90.0 * RAD)) };

	MainPanelVC.AddSwitch(&LiftoffNoAutoAbortSwitch, AID_VC_PUSHB_P1_01);
	LiftoffNoAutoAbortSwitch.SetReference(P1_3_PB_VECT, P1_COVERS_POS[7], SEQS_COVER_AXIS);
	LiftoffNoAutoAbortSwitch.SetCoverRotationAngle(70.0*RAD);
	LiftoffNoAutoAbortSwitch.DefineMeshGroup(VC_GRP_PB_P1_01, VC_GRP_SwitchCover_P1_08);

	MainPanelVC.AddSwitch(&LesMotorFireSwitch, AID_VC_PUSHB_P1_02);
	LesMotorFireSwitch.SetReference(P1_3_PB_VECT, P1_COVERS_POS[8], SEQS_COVER_AXIS);
	LesMotorFireSwitch.SetCoverRotationAngle(70.0*RAD);
	LesMotorFireSwitch.DefineMeshGroup(VC_GRP_PB_P1_02, VC_GRP_SwitchCover_P1_09);

	MainPanelVC.AddSwitch(&CanardDeploySwitch, AID_VC_PUSHB_P1_03);
	CanardDeploySwitch.SetReference(P1_3_PB_VECT, P1_COVERS_POS[9], SEQS_COVER_AXIS);
	CanardDeploySwitch.SetCoverRotationAngle(70.0*RAD);
	CanardDeploySwitch.DefineMeshGroup(VC_GRP_PB_P1_03, VC_GRP_SwitchCover_P1_10);

	MainPanelVC.AddSwitch(&CsmLvSepSwitch, AID_VC_PUSHB_P1_04);
	CsmLvSepSwitch.SetReference(P1_3_PB_VECT, P1_COVERS_POS[10], SEQS_COVER_AXIS);
	CsmLvSepSwitch.SetCoverRotationAngle(70.0*RAD);
	CsmLvSepSwitch.DefineMeshGroup(VC_GRP_PB_P1_04, VC_GRP_SwitchCover_P1_11);

	SEQS_COVER_AXIS = { -0.00, sin(P1_3_TILT + (90.0 * RAD)), -cos(P1_3_TILT + (90.0 * RAD)) };

	MainPanelVC.AddSwitch(&ApexCoverJettSwitch, AID_VC_PUSHB_P1_05);
	ApexCoverJettSwitch.SetReference(P1_3_PB_VECT, P1_COVERS_POS[11], SEQS_COVER_AXIS);
	ApexCoverJettSwitch.SetCoverRotationAngle(70.0*RAD);
	ApexCoverJettSwitch.DefineMeshGroup(VC_GRP_PB_P1_05, VC_GRP_SwitchCover_P1_12);

	MainPanelVC.AddSwitch(&DrogueDeploySwitch, AID_VC_PUSHB_P1_06);
	DrogueDeploySwitch.SetReference(P1_3_PB_VECT, P1_COVERS_POS[12], SEQS_COVER_AXIS);
	DrogueDeploySwitch.SetCoverRotationAngle(70.0*RAD);
	DrogueDeploySwitch.DefineMeshGroup(VC_GRP_PB_P1_06, VC_GRP_SwitchCover_P1_13);

	MainPanelVC.AddSwitch(&MainDeploySwitch, AID_VC_PUSHB_P1_07);
	MainDeploySwitch.SetReference(P1_3_PB_VECT, P1_COVERS_POS[13], SEQS_COVER_AXIS);
	MainDeploySwitch.SetCoverRotationAngle(70.0*RAD);
	MainDeploySwitch.DefineMeshGroup(VC_GRP_PB_P1_07, VC_GRP_SwitchCover_P1_14);

	MainPanelVC.AddSwitch(&CmRcsHeDumpSwitch, AID_VC_PUSHB_P1_08);
	CmRcsHeDumpSwitch.SetReference(P1_3_PB_VECT, P1_COVERS_POS[14], SEQS_COVER_AXIS);
	CmRcsHeDumpSwitch.SetCoverRotationAngle(70.0*RAD);
	CmRcsHeDumpSwitch.DefineMeshGroup(VC_GRP_PB_P1_08, VC_GRP_SwitchCover_P1_15);

	MainPanelVC.AddSwitch(&DirectUllageButton, AID_VC_PUSHB_P1_09);
	DirectUllageButton.SetDirection(P1_3_PB_VECT);
	DirectUllageButton.DefineMeshGroup(VC_GRP_PB_P1_09);

	MainPanelVC.AddSwitch(&ThrustOnButton, AID_VC_PUSHB_P1_10);
	ThrustOnButton.SetDirection(P1_3_PB_VECT);
	ThrustOnButton.DefineMeshGroup(VC_GRP_PB_P1_10);

	MainPanelVC.AddSwitch(&GDCAlignButton, AID_VC_PUSHB_P1_11);
	GDCAlignButton.SetDirection(P1_3_PB_VECT);
	GDCAlignButton.DefineMeshGroup(VC_GRP_PB_P1_11);

	MainPanelVC.AddSwitch(&SPSGimbalPitchThumbwheel, AID_VC_TW_P1_01);
	SPSGimbalPitchThumbwheel.SetReference(P1_TW_POS[0], _V(1, 0, 0));
	SPSGimbalPitchThumbwheel.DefineMeshGroup(VC_GRP_TW_P1_01);
	SPSGimbalPitchThumbwheel.SetInitialAnimState(0.5);

	const VECTOR3	TW_SPSYAW_AXIS = { -0.00, sin(P1_3_TILT + (90.0 * RAD)), -cos(P1_3_TILT + (90.0 * RAD)) };
	MainPanelVC.AddSwitch(&SPSGimbalYawThumbwheel, AID_VC_TW_P1_02);
	SPSGimbalYawThumbwheel.SetReference(P1_TW_POS[1], TW_SPSYAW_AXIS);
	SPSGimbalYawThumbwheel.DefineMeshGroup(VC_GRP_TW_P1_02);
	SPSGimbalYawThumbwheel.SetInitialAnimState(0.5);

	MainPanelVC.AddSwitch(&EMSFunctionSwitch, AID_VC_ROT_P1_01);
	EMSFunctionSwitch.SetReference(P1_ROT_POS[0], P1_3_ROT_AXIS);
	EMSFunctionSwitch.DefineMeshGroup(VC_GRP_Rot_P1_01);

	VECTOR3 NEEDLE_POS = { -0.640937, 0.4098, 0.355623 };

	MainPanelVC.AddSwitch(&GPFPIPitch1Meter);
	GPFPIPitch1Meter.SetReference(NEEDLE_POS);
	GPFPIPitch1Meter.DefineMeshGroup(VC_GRP_Needle_P1_01);

	MainPanelVC.AddSwitch(&GPFPIPitch2Meter);
	GPFPIPitch2Meter.SetReference(NEEDLE_POS);
	GPFPIPitch2Meter.DefineMeshGroup(VC_GRP_Needle_P1_02);

	MainPanelVC.AddSwitch(&GPFPIYaw1Meter);
	GPFPIYaw1Meter.SetReference(NEEDLE_POS);
	GPFPIYaw1Meter.DefineMeshGroup(VC_GRP_Needle_P1_03);

	MainPanelVC.AddSwitch(&GPFPIYaw2Meter);
	GPFPIYaw2Meter.SetReference(NEEDLE_POS);
	GPFPIYaw2Meter.DefineMeshGroup(VC_GRP_Needle_P1_04);

	NEEDLE_POS = { -0.7400, 0.4536, 0.2764 };

	MainPanelVC.AddSwitch(&LVSPSPcMeter);
	LVSPSPcMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	LVSPSPcMeter.SetRotationRange(RAD * 255);
	LVSPSPcMeter.DefineMeshGroup(VC_GRP_Needle_P1_05);

	NEEDLE_POS = { -0.801428, 0.634486, 0.345593 };

	MainPanelVC.AddSwitch(&AccelGMeter);
	AccelGMeter.SetReference(NEEDLE_POS, _V(0, 0, -1));
	AccelGMeter.SetRotationRange(RAD * 240);
	AccelGMeter.DefineMeshGroup(VC_GRP_Needle_P1_06);

	const VECTOR3 rsiref = { -0.680414, 0.71462, 0.365469 };

	ems.SetReference(rsiref, P1_3_ROT_AXIS);
	ems.DefineMeshGroup(VC_GRP_RSI_Indicator);

	const VECTOR3 dvsetref = { -0.507344, 0.732746, 0.370513 };
	EMSDvSetSwitch.Init(this);
	EMSDvSetSwitch.SetReference(dvsetref);
	EMSDvSetSwitch.DefineMeshGroup(VC_GRP_EMSDV_Set_switch);

	// Panel 2

	MainPanelVC.AddSwitch(&EDSSwitch, AID_VC_SWITCH_P2_01);
	EDSSwitch.SetReference(P2_TOGGLE_POS[0], _V(1, 0, 0));
	EDSSwitch.DefineMeshGroup(VC_GRP_Sw_P2_01);

	MainPanelVC.AddSwitch(&CsmLmFinalSep1Switch, AID_VC_SWITCH_P2_02);
	CsmLmFinalSep1Switch.SetReference(P2_TOGGLE_POS[1], P2_COVERS_POS[0], _V(1, 0, 0), _V(1, 0, 0));
	CsmLmFinalSep1Switch.DefineMeshGroup(VC_GRP_Sw_P2_02, VC_GRP_SwitchCover_P2_01);

	MainPanelVC.AddSwitch(&CsmLmFinalSep2Switch, AID_VC_SWITCH_P2_03);
	CsmLmFinalSep2Switch.SetReference(P2_TOGGLE_POS[2], P2_COVERS_POS[1], _V(1, 0, 0), _V(1, 0, 0));
	CsmLmFinalSep2Switch.DefineMeshGroup(VC_GRP_Sw_P2_03, VC_GRP_SwitchCover_P2_02);

	MainPanelVC.AddSwitch(&CmSmSep1Switch, AID_VC_SWITCH_P2_04);
	CmSmSep1Switch.SetReference(P2_TOGGLE_POS[3], P2_COVERS_POS[2], _V(1, 0, 0), _V(1, 0, 0));
	CmSmSep1Switch.DefineMeshGroup(VC_GRP_Sw_P2_04, VC_GRP_SwitchCover_P2_03);

	MainPanelVC.AddSwitch(&CmSmSep2Switch, AID_VC_SWITCH_P2_05);
	CmSmSep2Switch.SetReference(P2_TOGGLE_POS[4], P2_COVERS_POS[3], _V(1, 0, 0), _V(1, 0, 0));
	CmSmSep2Switch.DefineMeshGroup(VC_GRP_Sw_P2_05, VC_GRP_SwitchCover_P2_04);

	MainPanelVC.AddSwitch(&SIVBPayloadSepSwitch, AID_VC_SWITCH_P2_06);
	SIVBPayloadSepSwitch.SetReference(P2_TOGGLE_POS[5], P2_COVERS_POS[4], _V(1, 0, 0), _V(1, 0, 0));
	SIVBPayloadSepSwitch.DefineMeshGroup(VC_GRP_Sw_P2_06, VC_GRP_SwitchCover_P2_05);

	MainPanelVC.AddSwitch(&PropDumpAutoSwitch, AID_VC_SWITCH_P2_07);
	PropDumpAutoSwitch.SetReference(P2_TOGGLE_POS[6], _V(1, 0, 0));
	PropDumpAutoSwitch.DefineMeshGroup(VC_GRP_Sw_P2_07);

	MainPanelVC.AddSwitch(&TwoEngineOutAutoSwitch, AID_VC_SWITCH_P2_08);
	TwoEngineOutAutoSwitch.SetReference(P2_TOGGLE_POS[7], _V(1, 0, 0));
	TwoEngineOutAutoSwitch.DefineMeshGroup(VC_GRP_Sw_P2_08);

	MainPanelVC.AddSwitch(&LVRateAutoSwitch, AID_VC_SWITCH_P2_09);
	LVRateAutoSwitch.SetReference(P2_TOGGLE_POS[8], _V(1, 0, 0));
	LVRateAutoSwitch.DefineMeshGroup(VC_GRP_Sw_P2_09);

	MainPanelVC.AddSwitch(&TowerJett1Switch, AID_VC_SWITCH_P2_10);
	TowerJett1Switch.SetReference(P2_TOGGLE_POS[9], P2_COVERS_POS[5], _V(1, 0, 0), _V(1, 0, 0));
	TowerJett1Switch.DefineMeshGroup(VC_GRP_Sw_P2_10, VC_GRP_SwitchCover_P2_06);

	MainPanelVC.AddSwitch(&TowerJett2Switch, AID_VC_SWITCH_P2_11);
	TowerJett2Switch.SetReference(P2_TOGGLE_POS[10], P2_COVERS_POS[6], _V(1, 0, 0), _V(1, 0, 0));
	TowerJett2Switch.DefineMeshGroup(VC_GRP_Sw_P2_11, VC_GRP_SwitchCover_P2_07);

	MainPanelVC.AddSwitch(&LVGuidanceSwitch, AID_VC_SWITCH_P2_12);
	LVGuidanceSwitch.SetReference(P2_TOGGLE_POS[11], P2_COVERS_POS[7], _V(1, 0, 0), _V(1, 0, 0));
	LVGuidanceSwitch.DefineMeshGroup(VC_GRP_Sw_P2_12, VC_GRP_SwitchCover_P2_08);

	MainPanelVC.AddSwitch(&SIISIVBSepSwitch, AID_VC_SWITCH_P2_13);
	SIISIVBSepSwitch.SetReference(P2_TOGGLE_POS[12], P2_COVERS_POS[8], _V(1, 0, 0), _V(1, 0, 0));
	SIISIVBSepSwitch.DefineMeshGroup(VC_GRP_Sw_P2_13, VC_GRP_SwitchCover_P2_09);

	MainPanelVC.AddSwitch(&TLIEnableSwitch, AID_VC_SWITCH_P2_14);
	TLIEnableSwitch.SetReference(P2_TOGGLE_POS[13], _V(1, 0, 0));
	TLIEnableSwitch.DefineMeshGroup(VC_GRP_Sw_P2_14);

	MainPanelVC.AddSwitch(&MainReleaseSwitch, AID_VC_SWITCH_P2_15);
	MainReleaseSwitch.SetReference(P2_TOGGLE_POS[14], P2_COVERS_POS[9], _V(1, 0, 0), _V(1, 0, 0));
	MainReleaseSwitch.DefineMeshGroup(VC_GRP_Sw_P2_15, VC_GRP_SwitchCover_P2_10);

	MainPanelVC.AddSwitch(&DockingProbeExtdRelSwitch, AID_VC_SWITCH_P2_16);
	DockingProbeExtdRelSwitch.SetReference(P2_TOGGLE_POS[15], P2_COVERS_POS[10], _V(1, 0, 0), _V(1, 0, 0));
	DockingProbeExtdRelSwitch.DefineMeshGroup(VC_GRP_Sw_P2_16, VC_GRP_SwitchCover_P2_11);

	MainPanelVC.AddSwitch(&DockingProbeRetractPrimSwitch, AID_VC_SWITCH_P2_17);
	DockingProbeRetractPrimSwitch.SetReference(P2_TOGGLE_POS[16], _V(1, 0, 0));
	DockingProbeRetractPrimSwitch.DefineMeshGroup(VC_GRP_Sw_P2_17);

	MainPanelVC.AddSwitch(&DockingProbeRetractSecSwitch, AID_VC_SWITCH_P2_18);
	DockingProbeRetractSecSwitch.SetReference(P2_TOGGLE_POS[17], _V(1, 0, 0));
	DockingProbeRetractSecSwitch.DefineMeshGroup(VC_GRP_Sw_P2_18);

	MainPanelVC.AddSwitch(&RunEVALightSwitch, AID_VC_SWITCH_P2_19);
	RunEVALightSwitch.SetReference(P2_TOGGLE_POS[18], _V(1, 0, 0));
	RunEVALightSwitch.DefineMeshGroup(VC_GRP_Sw_P2_19);

	MainPanelVC.AddSwitch(&RndzLightSwitch, AID_VC_SWITCH_P2_20);
	RndzLightSwitch.SetReference(P2_TOGGLE_POS[19], _V(1, 0, 0));
	RndzLightSwitch.DefineMeshGroup(VC_GRP_Sw_P2_20);

	MainPanelVC.AddSwitch(&TunnelLightSwitch, AID_VC_SWITCH_P2_21);
	TunnelLightSwitch.SetReference(P2_TOGGLE_POS[20], _V(1, 0, 0));
	TunnelLightSwitch.DefineMeshGroup(VC_GRP_Sw_P2_21);

	MainPanelVC.AddSwitch(&LMPowerSwitch, AID_VC_SWITCH_P2_22);
	LMPowerSwitch.SetReference(P2_TOGGLE_POS[21], _V(1, 0, 0));
	LMPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P2_22);

	MainPanelVC.AddSwitch(&SMRCSHelium1ASwitch, AID_VC_SWITCH_P2_23);
	SMRCSHelium1ASwitch.SetReference(P2_TOGGLE_POS[22], _V(1, 0, 0));
	SMRCSHelium1ASwitch.DefineMeshGroup(VC_GRP_Sw_P2_23);

	MainPanelVC.AddSwitch(&SMRCSHelium1BSwitch, AID_VC_SWITCH_P2_24);
	SMRCSHelium1BSwitch.SetReference(P2_TOGGLE_POS[23], _V(1, 0, 0));
	SMRCSHelium1BSwitch.DefineMeshGroup(VC_GRP_Sw_P2_24);

	MainPanelVC.AddSwitch(&SMRCSHelium1CSwitch, AID_VC_SWITCH_P2_25);
	SMRCSHelium1CSwitch.SetReference(P2_TOGGLE_POS[24], _V(1, 0, 0));
	SMRCSHelium1CSwitch.DefineMeshGroup(VC_GRP_Sw_P2_25);

	MainPanelVC.AddSwitch(&SMRCSHelium1DSwitch, AID_VC_SWITCH_P2_26);
	SMRCSHelium1DSwitch.SetReference(P2_TOGGLE_POS[25], _V(1, 0, 0));
	SMRCSHelium1DSwitch.DefineMeshGroup(VC_GRP_Sw_P2_26);

	MainPanelVC.AddSwitch(&CMUplinkSwitch, AID_VC_SWITCH_P2_27);
	CMUplinkSwitch.SetReference(P2_TOGGLE_POS[26], _V(1, 0, 0));
	CMUplinkSwitch.DefineMeshGroup(VC_GRP_Sw_P2_27);

	MainPanelVC.AddSwitch(&IUUplinkSwitch, AID_VC_SWITCH_P2_28);
	IUUplinkSwitch.SetReference(P2_TOGGLE_POS[27], _V(1, 0, 0));
	IUUplinkSwitch.DefineMeshGroup(VC_GRP_Sw_P2_28);

	MainPanelVC.AddSwitch(&CMRCSPressSwitch, AID_VC_SWITCH_P2_29);
	CMRCSPressSwitch.SetReference(P2_TOGGLE_POS[28], P2_COVERS_POS[11], _V(1, 0, 0), _V(1, 0, 0));
	CMRCSPressSwitch.DefineMeshGroup(VC_GRP_Sw_P2_29, VC_GRP_SwitchCover_P2_12);

	MainPanelVC.AddSwitch(&SMRCSIndSwitch, AID_VC_SWITCH_P2_30);
	SMRCSIndSwitch.SetReference(P2_TOGGLE_POS[29], _V(1, 0, 0));
	SMRCSIndSwitch.DefineMeshGroup(VC_GRP_Sw_P2_30);

	MainPanelVC.AddSwitch(&SMRCSHelium2ASwitch, AID_VC_SWITCH_P2_31);
	SMRCSHelium2ASwitch.SetReference(P2_TOGGLE_POS[30], _V(1, 0, 0));
	SMRCSHelium2ASwitch.DefineMeshGroup(VC_GRP_Sw_P2_31);

	MainPanelVC.AddSwitch(&SMRCSHelium2BSwitch, AID_VC_SWITCH_P2_32);
	SMRCSHelium2BSwitch.SetReference(P2_TOGGLE_POS[31], _V(1, 0, 0));
	SMRCSHelium2BSwitch.DefineMeshGroup(VC_GRP_Sw_P2_32);

	MainPanelVC.AddSwitch(&SMRCSHelium2CSwitch, AID_VC_SWITCH_P2_33);
	SMRCSHelium2CSwitch.SetReference(P2_TOGGLE_POS[32], _V(1, 0, 0));
	SMRCSHelium2CSwitch.DefineMeshGroup(VC_GRP_Sw_P2_33);

	MainPanelVC.AddSwitch(&SMRCSHelium2DSwitch, AID_VC_SWITCH_P2_34);
	SMRCSHelium2DSwitch.SetReference(P2_TOGGLE_POS[33], _V(1, 0, 0));
	SMRCSHelium2DSwitch.DefineMeshGroup(VC_GRP_Sw_P2_34);

	MainPanelVC.AddSwitch(&SMRCSHeaterASwitch, AID_VC_SWITCH_P2_35);
	SMRCSHeaterASwitch.SetReference(P2_TOGGLE_POS[34], _V(1, 0, 0));
	SMRCSHeaterASwitch.DefineMeshGroup(VC_GRP_Sw_P2_35);

	MainPanelVC.AddSwitch(&SMRCSHeaterBSwitch, AID_VC_SWITCH_P2_36);
	SMRCSHeaterBSwitch.SetReference(P2_TOGGLE_POS[35], _V(1, 0, 0));
	SMRCSHeaterBSwitch.DefineMeshGroup(VC_GRP_Sw_P2_36);

	MainPanelVC.AddSwitch(&SMRCSHeaterCSwitch, AID_VC_SWITCH_P2_37);
	SMRCSHeaterCSwitch.SetReference(P2_TOGGLE_POS[36], _V(1, 0, 0));
	SMRCSHeaterCSwitch.DefineMeshGroup(VC_GRP_Sw_P2_37);

	MainPanelVC.AddSwitch(&SMRCSHeaterDSwitch, AID_VC_SWITCH_P2_38);
	SMRCSHeaterDSwitch.SetReference(P2_TOGGLE_POS[37], _V(1, 0, 0));
	SMRCSHeaterDSwitch.DefineMeshGroup(VC_GRP_Sw_P2_38);

	MainPanelVC.AddSwitch(&SMRCSProp1ASwitch, AID_VC_SWITCH_P2_39);
	SMRCSProp1ASwitch.SetReference(P2_TOGGLE_POS[38], _V(1, 0, 0));
	SMRCSProp1ASwitch.DefineMeshGroup(VC_GRP_Sw_P2_39);

	MainPanelVC.AddSwitch(&SMRCSProp1BSwitch, AID_VC_SWITCH_P2_40);
	SMRCSProp1BSwitch.SetReference(P2_TOGGLE_POS[39], _V(1, 0, 0));
	SMRCSProp1BSwitch.DefineMeshGroup(VC_GRP_Sw_P2_40);

	MainPanelVC.AddSwitch(&SMRCSProp1CSwitch, AID_VC_SWITCH_P2_41);
	SMRCSProp1CSwitch.SetReference(P2_TOGGLE_POS[40], _V(1, 0, 0));
	SMRCSProp1CSwitch.DefineMeshGroup(VC_GRP_Sw_P2_41);

	MainPanelVC.AddSwitch(&SMRCSProp1DSwitch, AID_VC_SWITCH_P2_42);
	SMRCSProp1DSwitch.SetReference(P2_TOGGLE_POS[41], _V(1, 0, 0));
	SMRCSProp1DSwitch.DefineMeshGroup(VC_GRP_Sw_P2_42);

	MainPanelVC.AddSwitch(&RCSCMDSwitch, AID_VC_SWITCH_P2_43);
	RCSCMDSwitch.SetReference(P2_TOGGLE_POS[42], _V(1, 0, 0));
	RCSCMDSwitch.DefineMeshGroup(VC_GRP_Sw_P2_43);

	MainPanelVC.AddSwitch(&RCSTrnfrSwitch, AID_VC_SWITCH_P2_44);
	RCSTrnfrSwitch.SetReference(P2_TOGGLE_POS[43], _V(1, 0, 0));
	RCSTrnfrSwitch.DefineMeshGroup(VC_GRP_Sw_P2_44);

	MainPanelVC.AddSwitch(&CMRCSProp1Switch, AID_VC_SWITCH_P2_45);
	CMRCSProp1Switch.SetReference(P2_TOGGLE_POS[44], _V(1, 0, 0));
	CMRCSProp1Switch.DefineMeshGroup(VC_GRP_Sw_P2_45);

	MainPanelVC.AddSwitch(&CMRCSProp2Switch, AID_VC_SWITCH_P2_46);
	CMRCSProp2Switch.SetReference(P2_TOGGLE_POS[45], _V(1, 0, 0));
	CMRCSProp2Switch.DefineMeshGroup(VC_GRP_Sw_P2_46);

	MainPanelVC.AddSwitch(&SMRCSProp2ASwitch, AID_VC_SWITCH_P2_47);
	SMRCSProp2ASwitch.SetReference(P2_TOGGLE_POS[46], _V(1, 0, 0));
	SMRCSProp2ASwitch.DefineMeshGroup(VC_GRP_Sw_P2_47);

	MainPanelVC.AddSwitch(&SMRCSProp2BSwitch, AID_VC_SWITCH_P2_48);
	SMRCSProp2BSwitch.SetReference(P2_TOGGLE_POS[47], _V(1, 0, 0));
	SMRCSProp2BSwitch.DefineMeshGroup(VC_GRP_Sw_P2_48);

	MainPanelVC.AddSwitch(&SMRCSProp2CSwitch, AID_VC_SWITCH_P2_49);
	SMRCSProp2CSwitch.SetReference(P2_TOGGLE_POS[48], _V(1, 0, 0));
	SMRCSProp2CSwitch.DefineMeshGroup(VC_GRP_Sw_P2_49);

	MainPanelVC.AddSwitch(&SMRCSProp2DSwitch, AID_VC_SWITCH_P2_50);
	SMRCSProp2DSwitch.SetReference(P2_TOGGLE_POS[49], _V(1, 0, 0));
	SMRCSProp2DSwitch.DefineMeshGroup(VC_GRP_Sw_P2_50);

	MainPanelVC.AddSwitch(&MissionTimerHoursSwitch, AID_VC_SWITCH_P2_51);
	MissionTimerHoursSwitch.SetReference(P2_TOGGLE_POS[50], _V(1, 0, 0));
	MissionTimerHoursSwitch.DefineMeshGroup(VC_GRP_Sw_P2_51);

	MainPanelVC.AddSwitch(&MissionTimerMinutesSwitch, AID_VC_SWITCH_P2_52);
	MissionTimerMinutesSwitch.SetReference(P2_TOGGLE_POS[51], _V(1, 0, 0));
	MissionTimerMinutesSwitch.DefineMeshGroup(VC_GRP_Sw_P2_52);

	MainPanelVC.AddSwitch(&MissionTimerSecondsSwitch, AID_VC_SWITCH_P2_53);
	MissionTimerSecondsSwitch.SetReference(P2_TOGGLE_POS[52], _V(1, 0, 0));
	MissionTimerSecondsSwitch.DefineMeshGroup(VC_GRP_Sw_P2_53);

	MainPanelVC.AddSwitch(&CautionWarningModeSwitch, AID_VC_SWITCH_P2_54);
	CautionWarningModeSwitch.SetReference(P2_TOGGLE_POS[53], _V(1, 0, 0));
	CautionWarningModeSwitch.DefineMeshGroup(VC_GRP_Sw_P2_54);

	MainPanelVC.AddSwitch(&CautionWarningCMCSMSwitch, AID_VC_SWITCH_P2_55);
	CautionWarningCMCSMSwitch.SetReference(P2_TOGGLE_POS[54], _V(1, 0, 0));
	CautionWarningCMCSMSwitch.DefineMeshGroup(VC_GRP_Sw_P2_55);

	MainPanelVC.AddSwitch(&CautionWarningPowerSwitch, AID_VC_SWITCH_P2_56);
	CautionWarningPowerSwitch.SetReference(P2_TOGGLE_POS[55], _V(1, 0, 0));
	CautionWarningPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P2_56);

	MainPanelVC.AddSwitch(&CautionWarningLightTestSwitch, AID_VC_SWITCH_P2_57);
	CautionWarningLightTestSwitch.SetReference(P2_TOGGLE_POS[56], _V(1, 0, 0));
	CautionWarningLightTestSwitch.DefineMeshGroup(VC_GRP_Sw_P2_57);

	MainPanelVC.AddSwitch(&MissionTimerSwitch, AID_VC_SWITCH_P2_58);
	MissionTimerSwitch.SetReference(P2_TOGGLE_POS[57], _V(1, 0, 0));
	MissionTimerSwitch.DefineMeshGroup(VC_GRP_Sw_P2_58);

	MainPanelVC.AddSwitch(&CabinFan1Switch, AID_VC_SWITCH_P2_59);
	CabinFan1Switch.SetReference(P2_TOGGLE_POS[58], _V(1, 0, 0));
	CabinFan1Switch.DefineMeshGroup(VC_GRP_Sw_P2_59);

	MainPanelVC.AddSwitch(&CabinFan2Switch, AID_VC_SWITCH_P2_60);
	CabinFan2Switch.SetReference(P2_TOGGLE_POS[59], _V(1, 0, 0));
	CabinFan2Switch.DefineMeshGroup(VC_GRP_Sw_P2_60);

	MainPanelVC.AddSwitch(&H2Heater1Switch, AID_VC_SWITCH_P2_61);
	H2Heater1Switch.SetReference(P2_TOGGLE_POS[60], _V(1, 0, 0));
	H2Heater1Switch.DefineMeshGroup(VC_GRP_Sw_P2_61);

	MainPanelVC.AddSwitch(&H2Heater2Switch, AID_VC_SWITCH_P2_62);
	H2Heater2Switch.SetReference(P2_TOGGLE_POS[61], _V(1, 0, 0));
	H2Heater2Switch.DefineMeshGroup(VC_GRP_Sw_P2_62);

	MainPanelVC.AddSwitch(&O2Heater1Switch, AID_VC_SWITCH_P2_63);
	O2Heater1Switch.SetReference(P2_TOGGLE_POS[62], _V(1, 0, 0));
	O2Heater1Switch.DefineMeshGroup(VC_GRP_Sw_P2_63);

	MainPanelVC.AddSwitch(&O2Heater2Switch, AID_VC_SWITCH_P2_64);
	O2Heater2Switch.SetReference(P2_TOGGLE_POS[63], _V(1, 0, 0));
	O2Heater2Switch.DefineMeshGroup(VC_GRP_Sw_P2_64);

	MainPanelVC.AddSwitch(&O2PressIndSwitch, AID_VC_SWITCH_P2_65);
	O2PressIndSwitch.SetReference(P2_TOGGLE_POS[64], _V(1, 0, 0));
	O2PressIndSwitch.DefineMeshGroup(VC_GRP_Sw_P2_65);

	MainPanelVC.AddSwitch(&H2Fan1Switch, AID_VC_SWITCH_P2_66);
	H2Fan1Switch.SetReference(P2_TOGGLE_POS[65], _V(1, 0, 0));
	H2Fan1Switch.DefineMeshGroup(VC_GRP_Sw_P2_66);

	MainPanelVC.AddSwitch(&H2Fan2Switch, AID_VC_SWITCH_P2_67);
	H2Fan2Switch.SetReference(P2_TOGGLE_POS[66], _V(1, 0, 0));
	H2Fan2Switch.DefineMeshGroup(VC_GRP_Sw_P2_67);

	MainPanelVC.AddSwitch(&O2Fan1Switch, AID_VC_SWITCH_P2_68);
	O2Fan1Switch.SetReference(P2_TOGGLE_POS[67], _V(1, 0, 0));
	O2Fan1Switch.DefineMeshGroup(VC_GRP_Sw_P2_68);

	MainPanelVC.AddSwitch(&O2Fan2Switch, AID_VC_SWITCH_P2_69);
	O2Fan2Switch.SetReference(P2_TOGGLE_POS[68], _V(1, 0, 0));
	O2Fan2Switch.DefineMeshGroup(VC_GRP_Sw_P2_69);

	MainPanelVC.AddSwitch(&EcsRadiatorsFlowContAutoSwitch, AID_VC_SWITCH_P2_70);
	EcsRadiatorsFlowContAutoSwitch.SetReference(P2_TOGGLE_POS[69], _V(1, 0, 0));
	EcsRadiatorsFlowContAutoSwitch.DefineMeshGroup(VC_GRP_Sw_P2_70);

	MainPanelVC.AddSwitch(&EcsRadiatorsFlowContPwrSwitch, AID_VC_SWITCH_P2_71);
	EcsRadiatorsFlowContPwrSwitch.SetReference(P2_TOGGLE_POS[70], _V(1, 0, 0));
	EcsRadiatorsFlowContPwrSwitch.DefineMeshGroup(VC_GRP_Sw_P2_71);

	MainPanelVC.AddSwitch(&EcsRadiatorsManSelSwitch, AID_VC_SWITCH_P2_72);
	EcsRadiatorsManSelSwitch.SetReference(P2_TOGGLE_POS[71], _V(1, 0, 0));
	EcsRadiatorsManSelSwitch.DefineMeshGroup(VC_GRP_Sw_P2_72);

	MainPanelVC.AddSwitch(&EcsRadiatorsHeaterPrimSwitch, AID_VC_SWITCH_P2_73);
	EcsRadiatorsHeaterPrimSwitch.SetReference(P2_TOGGLE_POS[72], _V(1, 0, 0));
	EcsRadiatorsHeaterPrimSwitch.DefineMeshGroup(VC_GRP_Sw_P2_73);

	MainPanelVC.AddSwitch(&EcsRadiatorsHeaterSecSwitch, AID_VC_SWITCH_P2_74);
	EcsRadiatorsHeaterSecSwitch.SetReference(P2_TOGGLE_POS[73], _V(1, 0, 0));
	EcsRadiatorsHeaterSecSwitch.DefineMeshGroup(VC_GRP_Sw_P2_74);

	MainPanelVC.AddSwitch(&PotH2oHtrSwitch, AID_VC_SWITCH_P2_75);
	PotH2oHtrSwitch.SetReference(P2_TOGGLE_POS[74], _V(1, 0, 0));
	PotH2oHtrSwitch.DefineMeshGroup(VC_GRP_Sw_P2_75);

	MainPanelVC.AddSwitch(&SuitCircuitH2oAccumAutoSwitch, AID_VC_SWITCH_P2_76);
	SuitCircuitH2oAccumAutoSwitch.SetReference(P2_TOGGLE_POS[75], _V(1, 0, 0));
	SuitCircuitH2oAccumAutoSwitch.DefineMeshGroup(VC_GRP_Sw_P2_76);

	MainPanelVC.AddSwitch(&SuitCircuitH2oAccumOnSwitch, AID_VC_SWITCH_P2_77);
	SuitCircuitH2oAccumOnSwitch.SetReference(P2_TOGGLE_POS[76], _V(1, 0, 0));
	SuitCircuitH2oAccumOnSwitch.DefineMeshGroup(VC_GRP_Sw_P2_77);

	MainPanelVC.AddSwitch(&SuitCircuitHeatExchSwitch, AID_VC_SWITCH_P2_78);
	SuitCircuitHeatExchSwitch.SetReference(P2_TOGGLE_POS[77], _V(1, 0, 0));
	SuitCircuitHeatExchSwitch.DefineMeshGroup(VC_GRP_Sw_P2_78);

	MainPanelVC.AddSwitch(&SecCoolantLoopEvapSwitch, AID_VC_SWITCH_P2_79);
	SecCoolantLoopEvapSwitch.SetReference(P2_TOGGLE_POS[78], _V(1, 0, 0));
	SecCoolantLoopEvapSwitch.DefineMeshGroup(VC_GRP_Sw_P2_79);

	MainPanelVC.AddSwitch(&SecCoolantLoopPumpSwitch, AID_VC_SWITCH_P2_80);
	SecCoolantLoopPumpSwitch.SetReference(P2_TOGGLE_POS[79], _V(1, 0, 0));
	SecCoolantLoopPumpSwitch.DefineMeshGroup(VC_GRP_Sw_P2_80);

	MainPanelVC.AddSwitch(&H2oQtyIndSwitch, AID_VC_SWITCH_P2_81);
	H2oQtyIndSwitch.SetReference(P2_TOGGLE_POS[80], _V(1, 0, 0));
	H2oQtyIndSwitch.DefineMeshGroup(VC_GRP_Sw_P2_81);

	MainPanelVC.AddSwitch(&GlycolEvapTempInSwitch, AID_VC_SWITCH_P2_82);
	GlycolEvapTempInSwitch.SetReference(P2_TOGGLE_POS[81], _V(1, 0, 0));
	GlycolEvapTempInSwitch.DefineMeshGroup(VC_GRP_Sw_P2_82);

	MainPanelVC.AddSwitch(&GlycolEvapSteamPressAutoManSwitch, AID_VC_SWITCH_P2_83);
	GlycolEvapSteamPressAutoManSwitch.SetReference(P2_TOGGLE_POS[82], _V(1, 0, 0));
	GlycolEvapSteamPressAutoManSwitch.DefineMeshGroup(VC_GRP_Sw_P2_83);

	MainPanelVC.AddSwitch(&GlycolEvapSteamPressIncrDecrSwitch, AID_VC_SWITCH_P2_84);
	GlycolEvapSteamPressIncrDecrSwitch.SetReference(P2_TOGGLE_POS[83], _V(1, 0, 0));
	GlycolEvapSteamPressIncrDecrSwitch.DefineMeshGroup(VC_GRP_Sw_P2_84);

	MainPanelVC.AddSwitch(&GlycolEvapH2oFlowSwitch, AID_VC_SWITCH_P2_85);
	GlycolEvapH2oFlowSwitch.SetReference(P2_TOGGLE_POS[84], _V(1, 0, 0));
	GlycolEvapH2oFlowSwitch.DefineMeshGroup(VC_GRP_Sw_P2_85);

	MainPanelVC.AddSwitch(&CabinTempAutoManSwitch, AID_VC_SWITCH_P2_86);
	CabinTempAutoManSwitch.SetReference(P2_TOGGLE_POS[85], _V(1, 0, 0));
	CabinTempAutoManSwitch.DefineMeshGroup(VC_GRP_Sw_P2_86);

	MainPanelVC.AddSwitch(&GHATrackSwitch, AID_VC_SWITCH_P2_87);
	GHATrackSwitch.SetReference(P2_TOGGLE_POS[86], _V(1, 0, 0));
	GHATrackSwitch.DefineMeshGroup(VC_GRP_Sw_P2_87);

	MainPanelVC.AddSwitch(&GHABeamSwitch, AID_VC_SWITCH_P2_88);
	GHABeamSwitch.SetReference(P2_TOGGLE_POS[87], _V(1, 0, 0));
	GHABeamSwitch.DefineMeshGroup(VC_GRP_Sw_P2_88);

	MainPanelVC.AddSwitch(&GHAPowerSwitch, AID_VC_SWITCH_P2_89);
	GHAPowerSwitch.SetReference(P2_TOGGLE_POS[88], _V(1, 0, 0));
	GHAPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P2_89);

	MainPanelVC.AddSwitch(&GHAServoElecSwitch, AID_VC_SWITCH_P2_90);
	GHAServoElecSwitch.SetReference(P2_TOGGLE_POS[89], _V(1, 0, 0));
	GHAServoElecSwitch.DefineMeshGroup(VC_GRP_Sw_P2_90);

	MainPanelVC.AddSwitch(&DskySwitchVerb, AID_VC_PUSHB_P2_01);
	DskySwitchVerb.SetDirection(P1_3_PB_VECT);
	DskySwitchVerb.DefineMeshGroup(VC_GRP_PB_P2_01);

	MainPanelVC.AddSwitch(&DskySwitchNoun, AID_VC_PUSHB_P2_02);
	DskySwitchNoun.SetDirection(P1_3_PB_VECT);
	DskySwitchNoun.DefineMeshGroup(VC_GRP_PB_P2_02);

	MainPanelVC.AddSwitch(&DskySwitchPlus, AID_VC_PUSHB_P2_03);
	DskySwitchPlus.SetDirection(P1_3_PB_VECT);
	DskySwitchPlus.DefineMeshGroup(VC_GRP_PB_P2_03);

	MainPanelVC.AddSwitch(&DskySwitchMinus, AID_VC_PUSHB_P2_04);
	DskySwitchMinus.SetDirection(P1_3_PB_VECT);
	DskySwitchMinus.DefineMeshGroup(VC_GRP_PB_P2_04);

	MainPanelVC.AddSwitch(&DskySwitchZero, AID_VC_PUSHB_P2_05);
	DskySwitchZero.SetDirection(P1_3_PB_VECT);
	DskySwitchZero.DefineMeshGroup(VC_GRP_PB_P2_05);

	MainPanelVC.AddSwitch(&DskySwitchOne, AID_VC_PUSHB_P2_06);
	DskySwitchOne.SetDirection(P1_3_PB_VECT);
	DskySwitchOne.DefineMeshGroup(VC_GRP_PB_P2_06);

	MainPanelVC.AddSwitch(&DskySwitchTwo, AID_VC_PUSHB_P2_07);
	DskySwitchTwo.SetDirection(P1_3_PB_VECT);
	DskySwitchTwo.DefineMeshGroup(VC_GRP_PB_P2_07);

	MainPanelVC.AddSwitch(&DskySwitchThree, AID_VC_PUSHB_P2_08);
	DskySwitchThree.SetDirection(P1_3_PB_VECT);
	DskySwitchThree.DefineMeshGroup(VC_GRP_PB_P2_08);

	MainPanelVC.AddSwitch(&DskySwitchFour, AID_VC_PUSHB_P2_09);
	DskySwitchFour.SetDirection(P1_3_PB_VECT);
	DskySwitchFour.DefineMeshGroup(VC_GRP_PB_P2_09);

	MainPanelVC.AddSwitch(&DskySwitchFive, AID_VC_PUSHB_P2_10);
	DskySwitchFive.SetDirection(P1_3_PB_VECT);
	DskySwitchFive.DefineMeshGroup(VC_GRP_PB_P2_10);

	MainPanelVC.AddSwitch(&DskySwitchSix, AID_VC_PUSHB_P2_11);
	DskySwitchSix.SetDirection(P1_3_PB_VECT);
	DskySwitchSix.DefineMeshGroup(VC_GRP_PB_P2_11);

	MainPanelVC.AddSwitch(&DskySwitchSeven, AID_VC_PUSHB_P2_12);
	DskySwitchSeven.SetDirection(P1_3_PB_VECT);
	DskySwitchSeven.DefineMeshGroup(VC_GRP_PB_P2_12);

	MainPanelVC.AddSwitch(&DskySwitchEight, AID_VC_PUSHB_P2_13);
	DskySwitchEight.SetDirection(P1_3_PB_VECT);
	DskySwitchEight.DefineMeshGroup(VC_GRP_PB_P2_13);

	MainPanelVC.AddSwitch(&DskySwitchNine, AID_VC_PUSHB_P2_14);
	DskySwitchNine.SetDirection(P1_3_PB_VECT);
	DskySwitchNine.DefineMeshGroup(VC_GRP_PB_P2_14);

	MainPanelVC.AddSwitch(&DskySwitchClear, AID_VC_PUSHB_P2_15);
	DskySwitchClear.SetDirection(P1_3_PB_VECT);
	DskySwitchClear.DefineMeshGroup(VC_GRP_PB_P2_15);

	MainPanelVC.AddSwitch(&DskySwitchProceed, AID_VC_PUSHB_P2_16);
	DskySwitchProceed.SetDirection(P1_3_PB_VECT);
	DskySwitchProceed.DefineMeshGroup(VC_GRP_PB_P2_16);

	MainPanelVC.AddSwitch(&DskySwitchKeyRel, AID_VC_PUSHB_P2_17);
	DskySwitchKeyRel.SetDirection(P1_3_PB_VECT);
	DskySwitchKeyRel.DefineMeshGroup(VC_GRP_PB_P2_17);

	MainPanelVC.AddSwitch(&DskySwitchEnter, AID_VC_PUSHB_P2_18);
	DskySwitchEnter.SetDirection(P1_3_PB_VECT);
	DskySwitchEnter.DefineMeshGroup(VC_GRP_PB_P2_18);

	MainPanelVC.AddSwitch(&DskySwitchReset, AID_VC_PUSHB_P2_19);
	DskySwitchReset.SetDirection(P1_3_PB_VECT);
	DskySwitchReset.DefineMeshGroup(VC_GRP_PB_P2_19);

	const VECTOR3	POSTLDGVENT_VECT = { 0.00, 0.005*cos(P1_3_TILT + (90.0 * RAD)), 0.005*sin(P1_3_TILT + (90.0 * RAD)) };
	MainPanelVC.AddSwitch(&PostLDGVentValveLever, AID_VC_POSTLDGVENTHANDLE);
	PostLDGVentValveLever.SetDirection(POSTLDGVENT_VECT);
	PostLDGVentValveLever.DefineMeshGroup(VC_GRP_PostLandingVentHandle);

	MainPanelVC.AddSwitch(&RCSIndicatorsSwitch, AID_VC_ROT_P2_01);
	RCSIndicatorsSwitch.SetReference(P2_ROT_POS[0], P1_3_ROT_AXIS);
	RCSIndicatorsSwitch.DefineMeshGroup(VC_GRP_Rot_P2_01);

	MainPanelVC.AddSwitch(&ECSIndicatorsSwitch, AID_VC_ROT_P2_02);
	ECSIndicatorsSwitch.SetReference(P2_ROT_POS[1], P1_3_ROT_AXIS);
	ECSIndicatorsSwitch.DefineMeshGroup(VC_GRP_Rot_P2_02);

	MainPanelVC.AddSwitch(&HighGainAntennaPitchPositionSwitch, AID_VC_ROT_P2_03);
	HighGainAntennaPitchPositionSwitch.SetReference(P2_ROT_POS[2], P1_3_ROT_AXIS);
	HighGainAntennaPitchPositionSwitch.DefineMeshGroup(VC_GRP_Rot_P2_03);
	HighGainAntennaPitchPositionSwitch.SetInitialAnimState(0.0);

	MainPanelVC.AddSwitch(&HighGainAntennaYawPositionSwitch, AID_VC_ROT_P2_04);
	HighGainAntennaYawPositionSwitch.SetReference(P2_ROT_POS[3], P1_3_ROT_AXIS);
	HighGainAntennaYawPositionSwitch.DefineMeshGroup(VC_GRP_Rot_P2_04);
	HighGainAntennaYawPositionSwitch.SetInitialAnimState(0.0);

	NEEDLE_POS = { -0.187906, 0.721, 0.455917 };

	MainPanelVC.AddSwitch(&RCSTempMeter);
	RCSTempMeter.SetReference(NEEDLE_POS);
	RCSTempMeter.SetRotationRange(RAD * 38.6);
	RCSTempMeter.DefineMeshGroup(VC_GRP_Needle_P2_01);

	MainPanelVC.AddSwitch(&RCSHeliumPressMeter);
	RCSHeliumPressMeter.SetReference(NEEDLE_POS);
	RCSHeliumPressMeter.SetRotationRange(RAD * 38.4);
	RCSHeliumPressMeter.DefineMeshGroup(VC_GRP_Needle_P2_02);

	MainPanelVC.AddSwitch(&RCSFuelPressMeter);
	RCSFuelPressMeter.SetReference(NEEDLE_POS);
	RCSFuelPressMeter.SetRotationRange(RAD * 39.5);
	RCSFuelPressMeter.DefineMeshGroup(VC_GRP_Needle_P2_03);

	MainPanelVC.AddSwitch(&RCSQuantityMeter);
	RCSQuantityMeter.SetReference(NEEDLE_POS);
	RCSQuantityMeter.SetRotationRange(RAD * 39.5);
	RCSQuantityMeter.DefineMeshGroup(VC_GRP_Needle_P2_04);

	NEEDLE_POS = { 0.33136, 0.761097, 0.468935 };

	MainPanelVC.AddSwitch(&H2Pressure1Meter);
	H2Pressure1Meter.SetReference(NEEDLE_POS);
	H2Pressure1Meter.SetRotationRange(RAD * 39.5);
	H2Pressure1Meter.DefineMeshGroup(VC_GRP_Needle_P2_05);

	MainPanelVC.AddSwitch(&H2Pressure2Meter);
	H2Pressure2Meter.SetReference(NEEDLE_POS);
	H2Pressure2Meter.SetRotationRange(RAD * 39.5);
	H2Pressure2Meter.DefineMeshGroup(VC_GRP_Needle_P2_06);

	MainPanelVC.AddSwitch(&O2Pressure1Meter);
	O2Pressure1Meter.SetReference(NEEDLE_POS);
	O2Pressure1Meter.SetRotationRange(RAD * 40.3);
	O2Pressure1Meter.DefineMeshGroup(VC_GRP_Needle_P2_07);

	MainPanelVC.AddSwitch(&O2Pressure2Meter);
	O2Pressure2Meter.SetReference(NEEDLE_POS);
	O2Pressure2Meter.SetRotationRange(RAD * 40.3);
	O2Pressure2Meter.DefineMeshGroup(VC_GRP_Needle_P2_08);

	MainPanelVC.AddSwitch(&H2Quantity1Meter);
	H2Quantity1Meter.SetReference(NEEDLE_POS);
	H2Quantity1Meter.SetRotationRange(RAD * 40.3);
	H2Quantity1Meter.DefineMeshGroup(VC_GRP_Needle_P2_09);

	MainPanelVC.AddSwitch(&H2Quantity2Meter);
	H2Quantity2Meter.SetReference(NEEDLE_POS);
	H2Quantity2Meter.SetRotationRange(RAD * 40.3);
	H2Quantity2Meter.DefineMeshGroup(VC_GRP_Needle_P2_10);

	MainPanelVC.AddSwitch(&O2Quantity1Meter);
	O2Quantity1Meter.SetReference(NEEDLE_POS);
	O2Quantity1Meter.SetRotationRange(RAD * 40.3);
	O2Quantity1Meter.DefineMeshGroup(VC_GRP_Needle_P2_11);

	MainPanelVC.AddSwitch(&O2Quantity2Meter);
	O2Quantity2Meter.SetReference(NEEDLE_POS);
	O2Quantity2Meter.SetRotationRange(RAD * 40.3);
	O2Quantity2Meter.DefineMeshGroup(VC_GRP_Needle_P2_12);

	NEEDLE_POS = { 0.347613, 0.603087, 0.415891 };

	MainPanelVC.AddSwitch(&SuitTempMeter);
	SuitTempMeter.SetReference(NEEDLE_POS);
	SuitTempMeter.SetRotationRange(RAD * 40.2);
	SuitTempMeter.DefineMeshGroup(VC_GRP_Needle_P2_13);

	MainPanelVC.AddSwitch(&CabinTempMeter);
	CabinTempMeter.SetReference(NEEDLE_POS);
	CabinTempMeter.SetRotationRange(RAD * 40.2);
	CabinTempMeter.DefineMeshGroup(VC_GRP_Needle_P2_14);

	MainPanelVC.AddSwitch(&SuitPressMeter);
	SuitPressMeter.SetReference(NEEDLE_POS);
	SuitPressMeter.SetRotationRange(RAD * 37.5);
	SuitPressMeter.DefineMeshGroup(VC_GRP_Needle_P2_15);

	MainPanelVC.AddSwitch(&CabinPressMeter);
	CabinPressMeter.SetReference(NEEDLE_POS);
	CabinPressMeter.SetRotationRange(RAD * 37.5);
	CabinPressMeter.DefineMeshGroup(VC_GRP_Needle_P2_16);

	MainPanelVC.AddSwitch(&PartPressCO2Meter);
	PartPressCO2Meter.SetReference(NEEDLE_POS);
	PartPressCO2Meter.SetRotationRange(RAD * 38.7);
	PartPressCO2Meter.DefineMeshGroup(VC_GRP_Needle_P2_17);

	NEEDLE_POS = { -0.2005, 0.9289, 0.4364 };

	MainPanelVC.AddSwitch(&SuitCabinDeltaPMeter);
	SuitCabinDeltaPMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS_NEG);
	SuitCabinDeltaPMeter.SetRotationRange(RAD * 110);
	SuitCabinDeltaPMeter.DefineMeshGroup(VC_GRP_Needle_P2_18);

	NEEDLE_POS = { -0.1741, 0.9289, 0.4364 };

	MainPanelVC.AddSwitch(&RightO2FlowMeter);
	RightO2FlowMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	RightO2FlowMeter.SetRotationRange(RAD * 110);
	RightO2FlowMeter.DefineMeshGroup(VC_GRP_Needle_P2_19);

	NEEDLE_POS = { 0.1030, 0.6457, 0.3416 };

	MainPanelVC.AddSwitch(&EcsRadTempInletMeter);
	EcsRadTempInletMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS_NEG);
	EcsRadTempInletMeter.SetRotationRange(RAD * 110);
	EcsRadTempInletMeter.DefineMeshGroup(VC_GRP_Needle_P2_20);

	NEEDLE_POS = { 0.1294, 0.6457, 0.3416 };

	MainPanelVC.AddSwitch(&EcsRadTempPrimOutletMeter);
	EcsRadTempPrimOutletMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	EcsRadTempPrimOutletMeter.SetRotationRange(RAD * 110);
	EcsRadTempPrimOutletMeter.DefineMeshGroup(VC_GRP_Needle_P2_21);

	NEEDLE_POS = { 0.1740, 0.6457, 0.3416 };

	MainPanelVC.AddSwitch(&EcsRadTempSecOutletMeter);
	EcsRadTempSecOutletMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS_NEG);
	EcsRadTempSecOutletMeter.SetRotationRange(RAD * 110);
	EcsRadTempSecOutletMeter.DefineMeshGroup(VC_GRP_Needle_P2_22);

	NEEDLE_POS = { 0.2004, 0.6457, 0.3416 };

	MainPanelVC.AddSwitch(&GlyEvapTempOutletMeter);
	GlyEvapTempOutletMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	GlyEvapTempOutletMeter.SetRotationRange(RAD * 110);
	GlyEvapTempOutletMeter.DefineMeshGroup(VC_GRP_Needle_P2_23);

	NEEDLE_POS = { 0.2438, 0.6457, 0.3416 };

	MainPanelVC.AddSwitch(&GlyEvapSteamPressMeter);
	GlyEvapSteamPressMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS_NEG);
	GlyEvapSteamPressMeter.SetRotationRange(RAD * 110);
	GlyEvapSteamPressMeter.DefineMeshGroup(VC_GRP_Needle_P2_24);

	NEEDLE_POS = { 0.2702, 0.6457, 0.3416 };

	MainPanelVC.AddSwitch(&GlycolDischPressMeter);
	GlycolDischPressMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	GlycolDischPressMeter.SetRotationRange(RAD * 110);
	GlycolDischPressMeter.DefineMeshGroup(VC_GRP_Needle_P2_25);

	NEEDLE_POS = { 0.1737, 0.5763, 0.3183 };

	MainPanelVC.AddSwitch(&LeftO2FlowMeter);
	LeftO2FlowMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS_NEG);
	LeftO2FlowMeter.SetRotationRange(RAD * 110);
	LeftO2FlowMeter.DefineMeshGroup(VC_GRP_Needle_P2_26);

	NEEDLE_POS = { 0.2000, 0.5763, 0.3183 };

	MainPanelVC.AddSwitch(&SuitComprDeltaPMeter);
	SuitComprDeltaPMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	SuitComprDeltaPMeter.SetRotationRange(RAD * 110);
	SuitComprDeltaPMeter.DefineMeshGroup(VC_GRP_Needle_P2_27);

	NEEDLE_POS = { 0.2439, 0.5763, 0.3183 };

	MainPanelVC.AddSwitch(&AccumQuantityMeter);
	AccumQuantityMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS_NEG);
	AccumQuantityMeter.SetRotationRange(RAD * 110);
	AccumQuantityMeter.DefineMeshGroup(VC_GRP_Needle_P2_28);

	NEEDLE_POS = { 0.2702, 0.5763, 0.3183 };

	MainPanelVC.AddSwitch(&H2oQuantityMeter);
	H2oQuantityMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	H2oQuantityMeter.SetRotationRange(RAD * 110);
	H2oQuantityMeter.DefineMeshGroup(VC_GRP_Needle_P2_29);

	NEEDLE_POS = { 0.3197, 0.4567, 0.2790 };

	MainPanelVC.AddSwitch(&HighGainAntennaPitchMeter);
	HighGainAntennaPitchMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS_NEG);
	HighGainAntennaPitchMeter.SetRotationRange(RAD * 180);
	HighGainAntennaPitchMeter.DefineMeshGroup(VC_GRP_Needle_P2_30);

	NEEDLE_POS = { 0.3663, 0.4568, 0.2790 };

	MainPanelVC.AddSwitch(&HighGainAntennaStrengthMeter);
	HighGainAntennaStrengthMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	HighGainAntennaStrengthMeter.SetRotationRange(RAD * 180);
	HighGainAntennaStrengthMeter.DefineMeshGroup(VC_GRP_Needle_P2_31);

	NEEDLE_POS = { 0.4127, 0.4568, 0.2790 };

	MainPanelVC.AddSwitch(&HighGainAntennaYawMeter);
	HighGainAntennaYawMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	HighGainAntennaYawMeter.SetRotationRange(RAD * 255);
	HighGainAntennaYawMeter.DefineMeshGroup(VC_GRP_Needle_P2_32);

	MainPanelVC.AddSwitch(&CabinTempAutoControlSwitch, AID_VC_TW_P2_01);
	CabinTempAutoControlSwitch.SetReference(P2_TW_POS[0], _V(1, 0, 0));
	CabinTempAutoControlSwitch.DefineMeshGroup(VC_GRP_TW_P2_01);

	MainPanelVC.AddSwitch(&SMRCSHelium1ATalkback, AID_VC_RCS_HELIUM1_TB);
	MainPanelVC.AddSwitch(&SMRCSHelium1BTalkback, AID_VC_RCS_HELIUM1_TB);
	MainPanelVC.AddSwitch(&SMRCSHelium1CTalkback, AID_VC_RCS_HELIUM1_TB);
	MainPanelVC.AddSwitch(&SMRCSHelium1DTalkback, AID_VC_RCS_HELIUM1_TB);

	MainPanelVC.AddSwitch(&SMRCSHelium2ATalkback, AID_VC_RCS_HELIUM2_TB);
	MainPanelVC.AddSwitch(&SMRCSHelium2BTalkback, AID_VC_RCS_HELIUM2_TB);
	MainPanelVC.AddSwitch(&SMRCSHelium2CTalkback, AID_VC_RCS_HELIUM2_TB);
	MainPanelVC.AddSwitch(&SMRCSHelium2DTalkback, AID_VC_RCS_HELIUM2_TB);

	MainPanelVC.AddSwitch(&SMRCSProp1ATalkback, AID_VC_RCS_PROP1_TB);
	MainPanelVC.AddSwitch(&SMRCSProp1BTalkback, AID_VC_RCS_PROP1_TB);
	MainPanelVC.AddSwitch(&SMRCSProp1CTalkback, AID_VC_RCS_PROP1_TB);
	MainPanelVC.AddSwitch(&SMRCSProp1DTalkback, AID_VC_RCS_PROP1_TB);

	MainPanelVC.AddSwitch(&CMRCSProp1Talkback, AID_VC_RCS_PROP2_TB);
	MainPanelVC.AddSwitch(&CMRCSProp2Talkback, AID_VC_RCS_PROP2_TB);
	MainPanelVC.AddSwitch(&SMRCSProp2ATalkback, AID_VC_RCS_PROP2_TB);
	MainPanelVC.AddSwitch(&SMRCSProp2BTalkback, AID_VC_RCS_PROP2_TB);
	MainPanelVC.AddSwitch(&SMRCSProp2CTalkback, AID_VC_RCS_PROP2_TB);
	MainPanelVC.AddSwitch(&SMRCSProp2DTalkback, AID_VC_RCS_PROP2_TB);

	MainPanelVC.AddSwitch(&EcsRadiatorIndicator, AID_VC_ECSRADIATORIND);

	MainPanelVC.AddSwitch(&DockingProbeAIndicator, AID_VC_DOCKINGPROBEIND);
	MainPanelVC.AddSwitch(&DockingProbeBIndicator, AID_VC_DOCKINGPROBEIND);

	// Panel 3

	MainPanelVC.AddSwitch(&FuelCellRadiators1Switch, AID_VC_SWITCH_P3_01);
	FuelCellRadiators1Switch.SetReference(P3_TOGGLE_POS[0], _V(1, 0, 0));
	FuelCellRadiators1Switch.DefineMeshGroup(VC_GRP_Sw_P3_01);

	MainPanelVC.AddSwitch(&FuelCellRadiators2Switch, AID_VC_SWITCH_P3_02);
	FuelCellRadiators2Switch.SetReference(P3_TOGGLE_POS[1], _V(1, 0, 0));
	FuelCellRadiators2Switch.DefineMeshGroup(VC_GRP_Sw_P3_02);

	MainPanelVC.AddSwitch(&FuelCellRadiators3Switch, AID_VC_SWITCH_P3_03);
	FuelCellRadiators3Switch.SetReference(P3_TOGGLE_POS[2], _V(1, 0, 0));
	FuelCellRadiators3Switch.DefineMeshGroup(VC_GRP_Sw_P3_03);

	MainPanelVC.AddSwitch(&SPSTestSwitch, AID_VC_SWITCH_P3_04);
	SPSTestSwitch.SetReference(P3_TOGGLE_POS[3], _V(1, 0, 0));
	SPSTestSwitch.DefineMeshGroup(VC_GRP_Sw_P3_04);

	MainPanelVC.AddSwitch(&FuelCellHeater1Switch, AID_VC_SWITCH_P3_05);
	FuelCellHeater1Switch.SetReference(P3_TOGGLE_POS[4], _V(1, 0, 0));
	FuelCellHeater1Switch.DefineMeshGroup(VC_GRP_Sw_P3_05);

	MainPanelVC.AddSwitch(&FuelCellHeater2Switch, AID_VC_SWITCH_P3_06);
	FuelCellHeater2Switch.SetReference(P3_TOGGLE_POS[5], _V(1, 0, 0));
	FuelCellHeater2Switch.DefineMeshGroup(VC_GRP_Sw_P3_06);

	MainPanelVC.AddSwitch(&FuelCellHeater3Switch, AID_VC_SWITCH_P3_07);
	FuelCellHeater3Switch.SetReference(P3_TOGGLE_POS[6], _V(1, 0, 0));
	FuelCellHeater3Switch.DefineMeshGroup(VC_GRP_Sw_P3_07);

	MainPanelVC.AddSwitch(&SPSOxidFlowValveSwitch, AID_VC_SWITCH_P3_08);
	SPSOxidFlowValveSwitch.SetReference(P3_TOGGLE_POS[7], _V(1, 0, 0));
	SPSOxidFlowValveSwitch.DefineMeshGroup(VC_GRP_Sw_P3_08);

	MainPanelVC.AddSwitch(&SPSOxidFlowValveSelectorSwitch, AID_VC_SWITCH_P3_09);
	SPSOxidFlowValveSelectorSwitch.SetReference(P3_TOGGLE_POS[8], _V(1, 0, 0));
	SPSOxidFlowValveSelectorSwitch.DefineMeshGroup(VC_GRP_Sw_P3_09);

	MainPanelVC.AddSwitch(&SPSPugModeSwitch, AID_VC_SWITCH_P3_10);
	SPSPugModeSwitch.SetReference(P3_TOGGLE_POS[9], _V(1, 0, 0));
	SPSPugModeSwitch.DefineMeshGroup(VC_GRP_Sw_P3_10);

	MainPanelVC.AddSwitch(&FuelCellPurge1Switch, AID_VC_SWITCH_P3_11);
	FuelCellPurge1Switch.SetReference(P3_TOGGLE_POS[10], _V(1, 0, 0));
	FuelCellPurge1Switch.DefineMeshGroup(VC_GRP_Sw_P3_11);

	MainPanelVC.AddSwitch(&FuelCellPurge2Switch, AID_VC_SWITCH_P3_12);
	FuelCellPurge2Switch.SetReference(P3_TOGGLE_POS[11], _V(1, 0, 0));
	FuelCellPurge2Switch.DefineMeshGroup(VC_GRP_Sw_P3_12);

	MainPanelVC.AddSwitch(&FuelCellPurge3Switch, AID_VC_SWITCH_P3_13);
	FuelCellPurge3Switch.SetReference(P3_TOGGLE_POS[12], _V(1, 0, 0));
	FuelCellPurge3Switch.DefineMeshGroup(VC_GRP_Sw_P3_13);

	MainPanelVC.AddSwitch(&MainBusASwitch1, AID_VC_SWITCH_P3_14);
	MainBusASwitch1.SetReference(P3_TOGGLE_POS[13], _V(1, 0, 0));
	MainBusASwitch1.DefineMeshGroup(VC_GRP_Sw_P3_14);

	MainPanelVC.AddSwitch(&MainBusASwitch2, AID_VC_SWITCH_P3_15);
	MainBusASwitch2.SetReference(P3_TOGGLE_POS[14], _V(1, 0, 0));
	MainBusASwitch2.DefineMeshGroup(VC_GRP_Sw_P3_15);

	MainPanelVC.AddSwitch(&MainBusASwitch3, AID_VC_SWITCH_P3_16);
	MainBusASwitch3.SetReference(P3_TOGGLE_POS[15], _V(1, 0, 0));
	MainBusASwitch3.DefineMeshGroup(VC_GRP_Sw_P3_16);

	MainPanelVC.AddSwitch(&MainBusAResetSwitch, AID_VC_SWITCH_P3_17);
	MainBusAResetSwitch.SetReference(P3_TOGGLE_POS[16], _V(1, 0, 0));
	MainBusAResetSwitch.DefineMeshGroup(VC_GRP_Sw_P3_17);

	MainPanelVC.AddSwitch(&SPSHeliumValveASwitch, AID_VC_SWITCH_P3_18);
	SPSHeliumValveASwitch.SetReference(P3_TOGGLE_POS[17], _V(1, 0, 0));
	SPSHeliumValveASwitch.DefineMeshGroup(VC_GRP_Sw_P3_18);

	MainPanelVC.AddSwitch(&SPSHeliumValveBSwitch, AID_VC_SWITCH_P3_19);
	SPSHeliumValveBSwitch.SetReference(P3_TOGGLE_POS[18], _V(1, 0, 0));
	SPSHeliumValveBSwitch.DefineMeshGroup(VC_GRP_Sw_P3_19);

	MainPanelVC.AddSwitch(&SPSLineHTRSSwitch, AID_VC_SWITCH_P3_20);
	SPSLineHTRSSwitch.SetReference(P3_TOGGLE_POS[19], _V(1, 0, 0));
	SPSLineHTRSSwitch.DefineMeshGroup(VC_GRP_Sw_P3_20);

	MainPanelVC.AddSwitch(&SPSPressIndSwitch, AID_VC_SWITCH_P3_21);
	SPSPressIndSwitch.SetReference(P3_TOGGLE_POS[20], _V(1, 0, 0));
	SPSPressIndSwitch.DefineMeshGroup(VC_GRP_Sw_P3_21);

	MainPanelVC.AddSwitch(&FuelCellReactants1Switch, AID_VC_SWITCH_P3_22);
	FuelCellReactants1Switch.SetReference(P3_TOGGLE_POS[21], _V(1, 0, 0));
	FuelCellReactants1Switch.DefineMeshGroup(VC_GRP_Sw_P3_22);

	MainPanelVC.AddSwitch(&FuelCellReactants2Switch, AID_VC_SWITCH_P3_23);
	FuelCellReactants2Switch.SetReference(P3_TOGGLE_POS[22], _V(1, 0, 0));
	FuelCellReactants2Switch.DefineMeshGroup(VC_GRP_Sw_P3_23);

	MainPanelVC.AddSwitch(&FuelCellReactants3Switch, AID_VC_SWITCH_P3_24);
	FuelCellReactants3Switch.SetReference(P3_TOGGLE_POS[23], _V(1, 0, 0));
	FuelCellReactants3Switch.DefineMeshGroup(VC_GRP_Sw_P3_24);

	MainPanelVC.AddSwitch(&MainBusBSwitch1, AID_VC_SWITCH_P3_25);
	MainBusBSwitch1.SetReference(P3_TOGGLE_POS[24], _V(1, 0, 0));
	MainBusBSwitch1.DefineMeshGroup(VC_GRP_Sw_P3_25);

	MainPanelVC.AddSwitch(&MainBusBSwitch2, AID_VC_SWITCH_P3_26);
	MainBusBSwitch2.SetReference(P3_TOGGLE_POS[25], _V(1, 0, 0));
	MainBusBSwitch2.DefineMeshGroup(VC_GRP_Sw_P3_26);

	MainPanelVC.AddSwitch(&MainBusBSwitch3, AID_VC_SWITCH_P3_27);
	MainBusBSwitch3.SetReference(P3_TOGGLE_POS[26], _V(1, 0, 0));
	MainBusBSwitch3.DefineMeshGroup(VC_GRP_Sw_P3_27);

	MainPanelVC.AddSwitch(&MainBusBResetSwitch, AID_VC_SWITCH_P3_28);
	MainBusBResetSwitch.SetReference(P3_TOGGLE_POS[27], _V(1, 0, 0));
	MainBusBResetSwitch.DefineMeshGroup(VC_GRP_Sw_P3_28);

	MainPanelVC.AddSwitch(&SBandNormalXPDRSwitch, AID_VC_SWITCH_P3_29);
	SBandNormalXPDRSwitch.SetReference(P3_TOGGLE_POS[28], _V(1, 0, 0));
	SBandNormalXPDRSwitch.DefineMeshGroup(VC_GRP_Sw_P3_29);

	MainPanelVC.AddSwitch(&SBandNormalPwrAmpl1Switch, AID_VC_SWITCH_P3_30);
	SBandNormalPwrAmpl1Switch.SetReference(P3_TOGGLE_POS[29], _V(1, 0, 0));
	SBandNormalPwrAmpl1Switch.DefineMeshGroup(VC_GRP_Sw_P3_30);

	MainPanelVC.AddSwitch(&SBandNormalPwrAmpl2Switch, AID_VC_SWITCH_P3_31);
	SBandNormalPwrAmpl2Switch.SetReference(P3_TOGGLE_POS[30], _V(1, 0, 0));
	SBandNormalPwrAmpl2Switch.DefineMeshGroup(VC_GRP_Sw_P3_31);

	MainPanelVC.AddSwitch(&SBandNormalMode1Switch, AID_VC_SWITCH_P3_32);
	SBandNormalMode1Switch.SetReference(P3_TOGGLE_POS[31], _V(1, 0, 0));
	SBandNormalMode1Switch.DefineMeshGroup(VC_GRP_Sw_P3_32);

	MainPanelVC.AddSwitch(&SBandNormalMode2Switch, AID_VC_SWITCH_P3_33);
	SBandNormalMode2Switch.SetReference(P3_TOGGLE_POS[32], _V(1, 0, 0));
	SBandNormalMode2Switch.DefineMeshGroup(VC_GRP_Sw_P3_33);

	MainPanelVC.AddSwitch(&SBandNormalMode3Switch, AID_VC_SWITCH_P3_34);
	SBandNormalMode3Switch.SetReference(P3_TOGGLE_POS[33], _V(1, 0, 0));
	SBandNormalMode3Switch.DefineMeshGroup(VC_GRP_Sw_P3_34);

	MainPanelVC.AddSwitch(&SBandAuxSwitch1, AID_VC_SWITCH_P3_35);
	SBandAuxSwitch1.SetReference(P3_TOGGLE_POS[34], _V(1, 0, 0));
	SBandAuxSwitch1.DefineMeshGroup(VC_GRP_Sw_P3_35);

	MainPanelVC.AddSwitch(&SBandAuxSwitch2, AID_VC_SWITCH_P3_36);
	SBandAuxSwitch2.SetReference(P3_TOGGLE_POS[35], _V(1, 0, 0));
	SBandAuxSwitch2.DefineMeshGroup(VC_GRP_Sw_P3_36);

	MainPanelVC.AddSwitch(&UPTLMSwitch1, AID_VC_SWITCH_P3_37);
	UPTLMSwitch1.SetReference(P3_TOGGLE_POS[36], _V(1, 0, 0));
	UPTLMSwitch1.DefineMeshGroup(VC_GRP_Sw_P3_37);

	MainPanelVC.AddSwitch(&UPTLMSwitch2, AID_VC_SWITCH_P3_38);
	UPTLMSwitch2.SetReference(P3_TOGGLE_POS[37], _V(1, 0, 0));
	UPTLMSwitch2.DefineMeshGroup(VC_GRP_Sw_P3_38);

	MainPanelVC.AddSwitch(&SBandAntennaSwitch1, AID_VC_SWITCH_P3_39);
	SBandAntennaSwitch1.SetReference(P3_TOGGLE_POS[38], _V(1, 0, 0));
	SBandAntennaSwitch1.DefineMeshGroup(VC_GRP_Sw_P3_39);

	MainPanelVC.AddSwitch(&SBandAntennaSwitch2, AID_VC_SWITCH_P3_40);
	SBandAntennaSwitch2.SetReference(P3_TOGGLE_POS[39], _V(1, 0, 0));
	SBandAntennaSwitch2.DefineMeshGroup(VC_GRP_Sw_P3_40);

	MainPanelVC.AddSwitch(&VHFAMASwitch, AID_VC_SWITCH_P3_41);
	VHFAMASwitch.SetReference(P3_TOGGLE_POS[40], _V(1, 0, 0));
	VHFAMASwitch.DefineMeshGroup(VC_GRP_Sw_P3_41);

	MainPanelVC.AddSwitch(&VHFAMBSwitch, AID_VC_SWITCH_P3_42);
	VHFAMBSwitch.SetReference(P3_TOGGLE_POS[41], _V(1, 0, 0));
	VHFAMBSwitch.DefineMeshGroup(VC_GRP_Sw_P3_42);

	MainPanelVC.AddSwitch(&RCVOnlySwitch, AID_VC_SWITCH_P3_43);
	RCVOnlySwitch.SetReference(P3_TOGGLE_POS[42], _V(1, 0, 0));
	RCVOnlySwitch.DefineMeshGroup(VC_GRP_Sw_P3_43);

	MainPanelVC.AddSwitch(&VHFBeaconSwitch, AID_VC_SWITCH_P3_44);
	VHFBeaconSwitch.SetReference(P3_TOGGLE_POS[43], _V(1, 0, 0));
	VHFBeaconSwitch.DefineMeshGroup(VC_GRP_Sw_P3_44);

	MainPanelVC.AddSwitch(&VHFRangingSwitch, AID_VC_SWITCH_P3_45);
	VHFRangingSwitch.SetReference(P3_TOGGLE_POS[44], _V(1, 0, 0));
	VHFRangingSwitch.DefineMeshGroup(VC_GRP_Sw_P3_45);

	MainPanelVC.AddSwitch(&FCReacsValvesSwitch, AID_VC_SWITCH_P3_46);
	FCReacsValvesSwitch.SetReference(P3_TOGGLE_POS[45], _V(1, 0, 0));
	FCReacsValvesSwitch.DefineMeshGroup(VC_GRP_Sw_P3_46);

	MainPanelVC.AddSwitch(&H2PurgeLineSwitch, AID_VC_SWITCH_P3_47);
	H2PurgeLineSwitch.SetReference(P3_TOGGLE_POS[46], _V(1, 0, 0));
	H2PurgeLineSwitch.DefineMeshGroup(VC_GRP_Sw_P3_47);

	MainPanelVC.AddSwitch(&TapeRecorderPCMSwitch, AID_VC_SWITCH_P3_48);
	TapeRecorderPCMSwitch.SetReference(P3_TOGGLE_POS[47], _V(1, 0, 0));
	TapeRecorderPCMSwitch.DefineMeshGroup(VC_GRP_Sw_P3_48);

	MainPanelVC.AddSwitch(&TapeRecorderRecordSwitch, AID_VC_SWITCH_P3_49);
	TapeRecorderRecordSwitch.SetReference(P3_TOGGLE_POS[48], _V(1, 0, 0));
	TapeRecorderRecordSwitch.DefineMeshGroup(VC_GRP_Sw_P3_49);

	MainPanelVC.AddSwitch(&TapeRecorderForwardSwitch, AID_VC_SWITCH_P3_50);
	TapeRecorderForwardSwitch.SetReference(P3_TOGGLE_POS[49], _V(1, 0, 0));
	TapeRecorderForwardSwitch.DefineMeshGroup(VC_GRP_Sw_P3_50);

	MainPanelVC.AddSwitch(&SCESwitch, AID_VC_SWITCH_P3_51);
	SCESwitch.SetReference(P3_TOGGLE_POS[50], _V(1, 0, 0));
	SCESwitch.DefineMeshGroup(VC_GRP_Sw_P3_51);

	MainPanelVC.AddSwitch(&PMPSwitch, AID_VC_SWITCH_P3_52);
	PMPSwitch.SetReference(P3_TOGGLE_POS[51], _V(1, 0, 0));
	PMPSwitch.DefineMeshGroup(VC_GRP_Sw_P3_52);

	MainPanelVC.AddSwitch(&PCMBitRateSwitch, AID_VC_SWITCH_P3_53);
	PCMBitRateSwitch.SetReference(P3_TOGGLE_POS[52], _V(1, 0, 0));
	PCMBitRateSwitch.DefineMeshGroup(VC_GRP_Sw_P3_53);

	MainPanelVC.AddSwitch(&MnA1Switch, AID_VC_SWITCH_P3_54);
	MnA1Switch.SetReference(P3_TOGGLE_POS[53], _V(1, 0, 0));
	MnA1Switch.DefineMeshGroup(VC_GRP_Sw_P3_54);

	MainPanelVC.AddSwitch(&MnB2Switch, AID_VC_SWITCH_P3_55);
	MnB2Switch.SetReference(P3_TOGGLE_POS[54], _V(1, 0, 0));
	MnB2Switch.DefineMeshGroup(VC_GRP_Sw_P3_55);

	MainPanelVC.AddSwitch(&MnA3Switch, AID_VC_SWITCH_P3_56);
	MnA3Switch.SetReference(P3_TOGGLE_POS[55], _V(1, 0, 0));
	MnA3Switch.DefineMeshGroup(VC_GRP_Sw_P3_56);

	MainPanelVC.AddSwitch(&AcBus1Switch1, AID_VC_SWITCH_P3_57);
	AcBus1Switch1.SetReference(P3_TOGGLE_POS[56], _V(1, 0, 0));
	AcBus1Switch1.DefineMeshGroup(VC_GRP_Sw_P3_57);

	MainPanelVC.AddSwitch(&AcBus1Switch2, AID_VC_SWITCH_P3_58);
	AcBus1Switch2.SetReference(P3_TOGGLE_POS[57], _V(1, 0, 0));
	AcBus1Switch2.DefineMeshGroup(VC_GRP_Sw_P3_58);

	MainPanelVC.AddSwitch(&AcBus1Switch3, AID_VC_SWITCH_P3_59);
	AcBus1Switch3.SetReference(P3_TOGGLE_POS[58], _V(1, 0, 0));
	AcBus1Switch3.DefineMeshGroup(VC_GRP_Sw_P3_59);

	MainPanelVC.AddSwitch(&AcBus1ResetSwitch, AID_VC_SWITCH_P3_60);
	AcBus1ResetSwitch.SetReference(P3_TOGGLE_POS[59], _V(1, 0, 0));
	AcBus1ResetSwitch.DefineMeshGroup(VC_GRP_Sw_P3_60);

	MainPanelVC.AddSwitch(&AcBus2Switch1, AID_VC_SWITCH_P3_61);
	AcBus2Switch1.SetReference(P3_TOGGLE_POS[60], _V(1, 0, 0));
	AcBus2Switch1.DefineMeshGroup(VC_GRP_Sw_P3_61);

	MainPanelVC.AddSwitch(&AcBus2Switch2, AID_VC_SWITCH_P3_62);
	AcBus2Switch2.SetReference(P3_TOGGLE_POS[61], _V(1, 0, 0));
	AcBus2Switch2.DefineMeshGroup(VC_GRP_Sw_P3_62);

	MainPanelVC.AddSwitch(&AcBus2Switch3, AID_VC_SWITCH_P3_63);
	AcBus2Switch3.SetReference(P3_TOGGLE_POS[62], _V(1, 0, 0));
	AcBus2Switch3.DefineMeshGroup(VC_GRP_Sw_P3_63);

	MainPanelVC.AddSwitch(&AcBus2ResetSwitch, AID_VC_SWITCH_P3_64);
	AcBus2ResetSwitch.SetReference(P3_TOGGLE_POS[63], _V(1, 0, 0));
	AcBus2ResetSwitch.DefineMeshGroup(VC_GRP_Sw_P3_64);

	MainPanelVC.AddSwitch(&VHFAntennaRotarySwitch, AID_VC_ROT_P3_01);
	VHFAntennaRotarySwitch.SetReference(P3_ROT_POS[0], P1_3_ROT_AXIS);
	VHFAntennaRotarySwitch.DefineMeshGroup(VC_GRP_Rot_P3_01);

	MainPanelVC.AddSwitch(&FuelCellIndicatorsSwitch, AID_VC_ROT_P3_02);
	FuelCellIndicatorsSwitch.SetReference(P3_ROT_POS[1], P1_3_ROT_AXIS);
	FuelCellIndicatorsSwitch.DefineMeshGroup(VC_GRP_Rot_P3_02);

	MainPanelVC.AddSwitch(&DCIndicatorsRotary, AID_VC_ROT_P3_03);
	DCIndicatorsRotary.SetReference(P3_ROT_POS[2], P1_3_ROT_AXIS);
	DCIndicatorsRotary.DefineMeshGroup(VC_GRP_Rot_P3_03);

	MainPanelVC.AddSwitch(&BatteryChargeRotary, AID_VC_ROT_P3_04);
	BatteryChargeRotary.SetReference(P3_ROT_POS[3], P1_3_ROT_AXIS);
	BatteryChargeRotary.DefineMeshGroup(VC_GRP_Rot_P3_04);

	MainPanelVC.AddSwitch(&ACIndicatorRotary, AID_VC_ROT_P3_05);
	ACIndicatorRotary.SetReference(P3_ROT_POS[4], P1_3_ROT_AXIS);
	ACIndicatorRotary.DefineMeshGroup(VC_GRP_Rot_P3_05);

	NEEDLE_POS = { 0.579603, 0.759254, 0.468032 };

	MainPanelVC.AddSwitch(&SPSTempMeter);
	SPSTempMeter.SetReference(NEEDLE_POS);
	SPSTempMeter.SetRotationRange(RAD * 39.5);
	SPSTempMeter.DefineMeshGroup(VC_GRP_Needle_P3_01);

	MainPanelVC.AddSwitch(&SPSHeliumNitrogenPressMeter);
	SPSHeliumNitrogenPressMeter.SetReference(NEEDLE_POS);
	SPSHeliumNitrogenPressMeter.SetRotationRange(RAD * 39.5);
	SPSHeliumNitrogenPressMeter.DefineMeshGroup(VC_GRP_Needle_P3_02);

	MainPanelVC.AddSwitch(&SPSFuelPressMeter);
	SPSFuelPressMeter.SetReference(NEEDLE_POS);
	SPSFuelPressMeter.SetRotationRange(RAD * 39.5);
	SPSFuelPressMeter.DefineMeshGroup(VC_GRP_Needle_P3_03);

	MainPanelVC.AddSwitch(&SPSOxidPressMeter);
	SPSOxidPressMeter.SetReference(NEEDLE_POS);
	SPSOxidPressMeter.SetRotationRange(RAD * 39.5);
	SPSOxidPressMeter.DefineMeshGroup(VC_GRP_Needle_P3_04);

	MainPanelVC.AddSwitch(&FuelCellH2FlowMeter);
	FuelCellH2FlowMeter.SetReference(NEEDLE_POS);
	FuelCellH2FlowMeter.SetRotationRange(RAD * 40.3);
	FuelCellH2FlowMeter.DefineMeshGroup(VC_GRP_Needle_P3_05);

	MainPanelVC.AddSwitch(&FuelCellO2FlowMeter);
	FuelCellO2FlowMeter.SetReference(NEEDLE_POS);
	FuelCellO2FlowMeter.SetRotationRange(RAD * 40.3);
	FuelCellO2FlowMeter.DefineMeshGroup(VC_GRP_Needle_P3_06);

	MainPanelVC.AddSwitch(&FuelCellTempMeter);
	FuelCellTempMeter.SetReference(NEEDLE_POS);
	FuelCellTempMeter.SetRotationRange(RAD * 40.7);
	FuelCellTempMeter.DefineMeshGroup(VC_GRP_Needle_P3_07);

	MainPanelVC.AddSwitch(&FuelCellCondenserTempMeter);
	FuelCellCondenserTempMeter.SetReference(NEEDLE_POS);
	FuelCellCondenserTempMeter.SetRotationRange(RAD * 40.7);
	FuelCellCondenserTempMeter.DefineMeshGroup(VC_GRP_Needle_P3_08);

	NEEDLE_POS = { 0.8420, 0.6010, 0.3265 };

	MainPanelVC.AddSwitch(&DCAmpMeter);
	DCAmpMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	DCAmpMeter.SetRotationRange(RAD * 240);
	DCAmpMeter.DefineMeshGroup(VC_GRP_Needle_P3_09);

	NEEDLE_POS = { 0.8533, 0.5413, 0.3066 };

	MainPanelVC.AddSwitch(&CSMDCVoltMeter);
	CSMDCVoltMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	CSMDCVoltMeter.SetRotationRange(RAD * 259.2);
	CSMDCVoltMeter.DefineMeshGroup(VC_GRP_Needle_P3_10);

	NEEDLE_POS = { 0.9799, 0.3656, 0.2478 };

	MainPanelVC.AddSwitch(&CSMACVoltMeter);
	CSMACVoltMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	CSMACVoltMeter.SetRotationRange(RAD * 264);
	CSMACVoltMeter.DefineMeshGroup(VC_GRP_Needle_P3_11);

	NEEDLE_POS = { 0.520752, 0.588298, 0.323995 };

	MainPanelVC.AddSwitch(&SPSOxidUnbalMeter);
	SPSOxidUnbalMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	SPSOxidUnbalMeter.SetRotationRange(RAD * 235);
	SPSOxidUnbalMeter.DefineMeshGroup(VC_GRP_Needle_P3_12);

	MainPanelVC.AddSwitch(&SquelchAThumbwheel, AID_VC_TW_P3_01);
	SquelchAThumbwheel.SetReference(P3_TW_POS[0], _V(1, 0, 0));
	SquelchAThumbwheel.DefineMeshGroup(VC_GRP_TW_P3_01);

	MainPanelVC.AddSwitch(&SquelchBThumbwheel, AID_VC_TW_P3_02);
	SquelchBThumbwheel.SetReference(P3_TW_POS[1], _V(1, 0, 0));
	SquelchBThumbwheel.DefineMeshGroup(VC_GRP_TW_P3_02);

	MainPanelVC.AddSwitch(&FuelCellPhIndicator, AID_VC_FCPHRADTEMPIND);
	MainPanelVC.AddSwitch(&FuelCellRadTempIndicator, AID_VC_FCPHRADTEMPIND);

	MainPanelVC.AddSwitch(&FuelCellRadiators1Indicator, AID_VC_FCRADIATORSIND);
	MainPanelVC.AddSwitch(&FuelCellRadiators2Indicator, AID_VC_FCRADIATORSIND);
	MainPanelVC.AddSwitch(&FuelCellRadiators3Indicator, AID_VC_FCRADIATORSIND);

	MainPanelVC.AddSwitch(&MainBusAIndicator1, ADI_VC_MAINBUSAIND);
	MainPanelVC.AddSwitch(&MainBusAIndicator2, ADI_VC_MAINBUSAIND);
	MainPanelVC.AddSwitch(&MainBusAIndicator3, ADI_VC_MAINBUSAIND);

	MainPanelVC.AddSwitch(&SPSHeliumValveAIndicator, AID_VC_SPSHEVALVEIND);
	MainPanelVC.AddSwitch(&SPSHeliumValveBIndicator, AID_VC_SPSHEVALVEIND);

	MainPanelVC.AddSwitch(&FuelCellReactants1Indicator, AID_VC_FCREACTANTSIND);
	MainPanelVC.AddSwitch(&FuelCellReactants2Indicator, AID_VC_FCREACTANTSIND);
	MainPanelVC.AddSwitch(&FuelCellReactants3Indicator, AID_VC_FCREACTANTSIND);
	MainPanelVC.AddSwitch(&MainBusBIndicator1, AID_VC_FCREACTANTSIND);
	MainPanelVC.AddSwitch(&MainBusBIndicator2, AID_VC_FCREACTANTSIND);
	MainPanelVC.AddSwitch(&MainBusBIndicator3, AID_VC_FCREACTANTSIND);

	MainPanelVC.AddSwitch(&PwrAmplTB, AID_VC_TELECOMTB);
	MainPanelVC.AddSwitch(&DseTapeTB, AID_VC_TELECOMTB);

	MainPanelVC.AddSwitch(&SPSOxidPercentMeter, AID_VC_SPS_OXID_PCT);
	MainPanelVC.AddSwitch(&SPSFuelPercentMeter, AID_VC_SPS_FUEL_PCT);

	MainPanelVC.AddSwitch(&SPSOxidFlowValveMaxIndicator, AID_VC_SPSOXIDFLOWIND);
	MainPanelVC.AddSwitch(&SPSOxidFlowValveMinIndicator, AID_VC_SPSOXIDFLOWIND);

	MainPanelVC.AddSwitch(&SPSInjectorValve1Indicator, AID_VC_SPS_INJ_VLV);
	MainPanelVC.AddSwitch(&SPSInjectorValve2Indicator, AID_VC_SPS_INJ_VLV);
	MainPanelVC.AddSwitch(&SPSInjectorValve3Indicator, AID_VC_SPS_INJ_VLV);
	MainPanelVC.AddSwitch(&SPSInjectorValve4Indicator, AID_VC_SPS_INJ_VLV);

	// Panel 4

	const VECTOR3 P4_SW_AXIS = { 0.88699091775187, 0.460460338294568, -0.03497697361619 };

	MainPanelVC.AddSwitch(&SPSGaugingSwitch, AID_VC_SWITCH_P4_01);
	SPSGaugingSwitch.SetReference(P4_TOGGLE_POS[0], P4_SW_AXIS);
	SPSGaugingSwitch.DefineMeshGroup(VC_GRP_Sw_P4_01);

	MainPanelVC.AddSwitch(&TelcomGroup1Switch, AID_VC_SWITCH_P4_02);
	TelcomGroup1Switch.SetReference(P4_TOGGLE_POS[1], P4_SW_AXIS);
	TelcomGroup1Switch.DefineMeshGroup(VC_GRP_Sw_P4_02);

	MainPanelVC.AddSwitch(&TelcomGroup2Switch, AID_VC_SWITCH_P4_03);
	TelcomGroup2Switch.SetReference(P4_TOGGLE_POS[2], P4_SW_AXIS);
	TelcomGroup2Switch.DefineMeshGroup(VC_GRP_Sw_P4_03);

	MainPanelVC.AddSwitch(&SuitCompressor1Switch, AID_VC_SWITCH_P4_04);
	SuitCompressor1Switch.SetReference(P4_TOGGLE_POS[3], P4_SW_AXIS);
	SuitCompressor1Switch.DefineMeshGroup(VC_GRP_Sw_P4_04);

	MainPanelVC.AddSwitch(&SuitCompressor2Switch, AID_VC_SWITCH_P4_05);
	SuitCompressor2Switch.SetReference(P4_TOGGLE_POS[4], P4_SW_AXIS);
	SuitCompressor2Switch.DefineMeshGroup(VC_GRP_Sw_P4_05);

	const VECTOR3	P4_ROT_AXIS = { -0.44660808466206, 0.853175444492311, -0.269504878675383 };

	MainPanelVC.AddSwitch(&EcsGlycolPumpsSwitch, AID_VC_ROT_P4_01);
	EcsGlycolPumpsSwitch.SetReference(P4_ROT_POS[0], P4_ROT_AXIS);
	EcsGlycolPumpsSwitch.DefineMeshGroup(VC_GRP_Rot_P4_01);

	const VECTOR3 cb_P4_vector = { 0.44660808466206 * 0.003, -0.853175444492311 * 0.003, 0.269504878675383 * 0.003 };

	CircuitBrakerSwitch* breakerspanel4[P4_CBCOUNT] = { &SuitCompressorsAc1ACircuitBraker, &SuitCompressorsAc1BCircuitBraker, &SuitCompressorsAc1CCircuitBraker, &SuitCompressorsAc2ACircuitBraker, &SuitCompressorsAc2BCircuitBraker, &SuitCompressorsAc2CCircuitBraker, &ECSGlycolPumpsAc1ACircuitBraker, &ECSGlycolPumpsAc1BCircuitBraker, &ECSGlycolPumpsAc1CCircuitBraker,
		&ECSGlycolPumpsAc2ACircuitBraker, &ECSGlycolPumpsAc2BCircuitBraker, &ECSGlycolPumpsAc2CCircuitBraker };

	for (int i = 0; i < P4_CBCOUNT; i++)
	{
		MainPanelVC.AddSwitch(breakerspanel4[i], AID_VC_CB_P4_01 + i);
		breakerspanel4[i]->SetDirection(cb_P4_vector);
		breakerspanel4[i]->DefineMeshGroup(VC_GRP_CB_P4_01 + i);
	}

	// Panel 5

	const VECTOR3 P5_SW_AXIS = { 0.517790715821878, 0.220780935148721, -0.826528011191105 };

	MainPanelVC.AddSwitch(&FuelCellPumps1Switch, AID_VC_SWITCH_P5_01);
	FuelCellPumps1Switch.SetReference(P5_TOGGLE_POS[0], P5_SW_AXIS);
	FuelCellPumps1Switch.DefineMeshGroup(VC_GRP_Sw_P5_01);

	MainPanelVC.AddSwitch(&FuelCellPumps2Switch, AID_VC_SWITCH_P5_02);
	FuelCellPumps2Switch.SetReference(P5_TOGGLE_POS[1], P5_SW_AXIS);
	FuelCellPumps2Switch.DefineMeshGroup(VC_GRP_Sw_P5_02);

	MainPanelVC.AddSwitch(&FuelCellPumps3Switch, AID_VC_SWITCH_P5_03);
	FuelCellPumps3Switch.SetReference(P5_TOGGLE_POS[2], P5_SW_AXIS);
	FuelCellPumps3Switch.DefineMeshGroup(VC_GRP_Sw_P5_03);

	MainPanelVC.AddSwitch(&GNPowerSwitch, AID_VC_SWITCH_P5_04);
	GNPowerSwitch.SetReference(P5_TOGGLE_POS[3], P5_SW_AXIS);
	GNPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P5_04);

	MainPanelVC.AddSwitch(&MainBusTieBatAcSwitch, AID_VC_SWITCH_P5_05);
	MainBusTieBatAcSwitch.SetReference(P5_TOGGLE_POS[4], P5_SW_AXIS);
	MainBusTieBatAcSwitch.DefineMeshGroup(VC_GRP_Sw_P5_05);

	MainPanelVC.AddSwitch(&MainBusTieBatBcSwitch, AID_VC_SWITCH_P5_06);
	MainBusTieBatBcSwitch.SetReference(P5_TOGGLE_POS[5], P5_SW_AXIS);
	MainBusTieBatBcSwitch.DefineMeshGroup(VC_GRP_Sw_P5_06);

	MainPanelVC.AddSwitch(&BatCHGRSwitch, AID_VC_SWITCH_P5_07);
	BatCHGRSwitch.SetReference(P5_TOGGLE_POS[6], P5_SW_AXIS);
	BatCHGRSwitch.DefineMeshGroup(VC_GRP_Sw_P5_07);

	MainPanelVC.AddSwitch(&NonessBusSwitch, AID_VC_SWITCH_P5_08);
	NonessBusSwitch.SetReference(P5_TOGGLE_POS[7], P5_SW_AXIS);
	NonessBusSwitch.DefineMeshGroup(VC_GRP_Sw_P5_08);

	MainPanelVC.AddSwitch(&InteriorLightsFloodDimSwitch, AID_VC_SWITCH_P5_09);
	InteriorLightsFloodDimSwitch.SetReference(P5_TOGGLE_POS[8], P5_SW_AXIS);
	InteriorLightsFloodDimSwitch.DefineMeshGroup(VC_GRP_Sw_P5_09);

	MainPanelVC.AddSwitch(&InteriorLightsFloodFixedSwitch, AID_VC_SWITCH_P5_10);
	InteriorLightsFloodFixedSwitch.SetReference(P5_TOGGLE_POS[9], P5_SW_AXIS);
	InteriorLightsFloodFixedSwitch.DefineMeshGroup(VC_GRP_Sw_P5_10);

	const VECTOR3	P5_ROT_AXIS = { -0.846089265473375, -0.002307516178545, -0.533036237248285 };

	MainPanelVC.AddSwitch(&RightIntegralRotarySwitch, AID_VC_ROT_P5_01);
	RightIntegralRotarySwitch.SetReference(P5_ROT_POS[0], P5_ROT_AXIS);
	RightIntegralRotarySwitch.DefineMeshGroup(VC_GRP_Rot_P5_01);
	RightIntegralRotarySwitch.SetInitialAnimState(0.5);

	MainPanelVC.AddSwitch(&RightFloodRotarySwitch, AID_VC_ROT_P5_02);
	RightFloodRotarySwitch.SetReference(P5_ROT_POS[1], P5_ROT_AXIS);
	RightFloodRotarySwitch.DefineMeshGroup(VC_GRP_Rot_P5_02);
	RightFloodRotarySwitch.SetInitialAnimState(0.5);

	const VECTOR3 cb_p5_vector = { 0.846089265473375 * 0.003, 0.002307516178545 * 0.003, 0.533036237248285 * 0.003 };

	CircuitBrakerSwitch* breakerspanel5[P5_CBCOUNT] = { &EpsSensorSignalDcMnaCircuitBraker, &EpsSensorSignalDcMnbCircuitBraker, &EpsSensorSignalAc1CircuitBraker, &EpsSensorSignalAc2CircuitBraker, &CWMnaCircuitBraker, &CWMnbCircuitBraker, &MnbLMPWR1CircuitBraker, &InverterControl1CircuitBraker, &InverterControl2CircuitBraker,
		&InverterControl3CircuitBraker, &EPSSensorUnitDcBusACircuitBraker, &EPSSensorUnitDcBusBCircuitBraker, &EPSSensorUnitAcBus1CircuitBraker, &EPSSensorUnitAcBus2CircuitBraker, &BATRLYBusBatACircuitBraker, &BATRLYBusBatBCircuitBraker, &MnbLMPWR2CircuitBraker, &ControllerAc1CircuitBraker, &ControllerAc2CircuitBraker,
		&CONTHTRSMnACircuitBraker, &CONTHTRSMnBCircuitBraker, &HTRSOVLDBatACircuitBraker, &HTRSOVLDBatBCircuitBraker, &BatteryChargerBatACircuitBraker, &BatteryChargerBatBCircuitBraker, &BatteryChargerMnACircuitBraker, &BatteryChargerMnBCircuitBraker, &BatteryChargerAcPwrCircuitBraker, &InstrumentLightingESSMnACircuitBraker,
		&InstrumentLightingESSMnBCircuitBraker, &InstrumentLightingNonESSCircuitBraker, &InstrumentLightingSCIEquipSEP1CircuitBraker, &InstrumentLightingSCIEquipSEP2CircuitBraker, &InstrumentLightingSCIEquipHatchCircuitBraker, &ECSPOTH2OHTRMnACircuitBraker, &ECSPOTH2OHTRMnBCircuitBraker, &ECSH2OAccumMnACircuitBraker,
		&ECSH2OAccumMnBCircuitBraker, &ECSTransducerWastePOTH2OMnACircuitBraker, &ECSTransducerWastePOTH2OMnBCircuitBraker, &ECSTransducerPressGroup1MnACircuitBraker, &ECSTransducerPressGroup1MnBCircuitBraker, &ECSTransducerPressGroup2MnACircuitBraker, &ECSTransducerPressGroup2MnBCircuitBraker, &ECSTransducerTempMnACircuitBraker,
		&ECSTransducerTempMnBCircuitBraker, &ECSSecCoolLoopAc1CircuitBraker, &ECSSecCoolLoopAc2CircuitBraker, &ECSSecCoolLoopRADHTRMnACircuitBraker, &ECSSecCoolLoopXducersMnACircuitBraker, &ECSSecCoolLoopXducersMnBCircuitBraker, &ECSWasteH2OUrineDumpHTRMnACircuitBraker, &ECSWasteH2OUrineDumpHTRMnBCircuitBraker, &ECSCabinFanAC1ACircuitBraker,
		&ECSCabinFanAC1BCircuitBraker, &ECSCabinFanAC1CCircuitBraker, &ECSCabinFanAC2ACircuitBraker, &ECSCabinFanAC2BCircuitBraker, &ECSCabinFanAC2CCircuitBraker, &GNPowerAc1CircuitBraker, &GNPowerAc2CircuitBraker, &GNIMUMnACircuitBraker, &GNIMUMnBCircuitBraker, &GNIMUHTRMnACircuitBraker, &GNIMUHTRMnBCircuitBraker, &GNComputerMnACircuitBraker,
		&GNComputerMnBCircuitBraker, &GNOpticsMnACircuitBraker, &GNOpticsMnBCircuitBraker };

	for (int i = 0; i < P5_CBCOUNT; i++)
	{
		MainPanelVC.AddSwitch(breakerspanel5[i], AID_VC_CB_P5_01 + i);
		breakerspanel5[i]->SetDirection(cb_p5_vector);
		breakerspanel5[i]->DefineMeshGroup(VC_GRP_CB_P5_01 + i);
	}

	// Panel 6

	const VECTOR3 P6_SW_AXIS = { 0.300206537322247, 0.544287959888755, -0.783343252775642 };

	MainPanelVC.AddSwitch(&RightModeIntercomSwitch, AID_VC_SWITCH_P6_01);
	RightModeIntercomSwitch.SetReference(P6_TOGGLE_POS[0], P6_SW_AXIS);
	RightModeIntercomSwitch.DefineMeshGroup(VC_GRP_Sw_P6_01);

	MainPanelVC.AddSwitch(&RightPadCommSwitch, AID_VC_SWITCH_P6_02);
	RightPadCommSwitch.SetReference(P6_TOGGLE_POS[1], P6_SW_AXIS);
	RightPadCommSwitch.DefineMeshGroup(VC_GRP_Sw_P6_02);

	MainPanelVC.AddSwitch(&RightSBandSwitch, AID_VC_SWITCH_P6_03);
	RightSBandSwitch.SetReference(P6_TOGGLE_POS[2], P6_SW_AXIS);
	RightSBandSwitch.DefineMeshGroup(VC_GRP_Sw_P6_03);

	MainPanelVC.AddSwitch(&RightAudioPowerSwitch, AID_VC_SWITCH_P6_04);
	RightAudioPowerSwitch.SetReference(P6_TOGGLE_POS[3], P6_SW_AXIS);
	RightAudioPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P6_04);

	MainPanelVC.AddSwitch(&RightIntercomSwitch, AID_VC_SWITCH_P6_05);
	RightIntercomSwitch.SetReference(P6_TOGGLE_POS[4], P6_SW_AXIS);
	RightIntercomSwitch.DefineMeshGroup(VC_GRP_Sw_P6_05);

	MainPanelVC.AddSwitch(&RightVHFAMSwitch, AID_VC_SWITCH_P6_06);
	RightVHFAMSwitch.SetReference(P6_TOGGLE_POS[5], P6_SW_AXIS);
	RightVHFAMSwitch.DefineMeshGroup(VC_GRP_Sw_P6_06);

	MainPanelVC.AddSwitch(&AudioControlSwitch, AID_VC_SWITCH_P6_07);
	AudioControlSwitch.SetReference(P6_TOGGLE_POS[6], P6_SW_AXIS);
	AudioControlSwitch.DefineMeshGroup(VC_GRP_Sw_P6_07);

	MainPanelVC.AddSwitch(&SuitPowerSwitch, AID_VC_SWITCH_P6_08);
	SuitPowerSwitch.SetReference(P6_TOGGLE_POS[7], P6_SW_AXIS);
	SuitPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P6_08);


	MainPanelVC.AddSwitch(&ModeIntercomVOXSensThumbwheelSwitch, AID_VC_TW_P6_01);
	ModeIntercomVOXSensThumbwheelSwitch.SetReference(P6_TW_POS[0], P6_SW_AXIS);
	ModeIntercomVOXSensThumbwheelSwitch.DefineMeshGroup(VC_GRP_TW_P6_01);

	MainPanelVC.AddSwitch(&PadCommVolumeThumbwheelSwitch, AID_VC_TW_P6_02);
	PadCommVolumeThumbwheelSwitch.SetReference(P6_TW_POS[1], P6_SW_AXIS);
	PadCommVolumeThumbwheelSwitch.DefineMeshGroup(VC_GRP_TW_P6_02);

	MainPanelVC.AddSwitch(&SBandVolumeThumbwheelSwitch, AID_VC_TW_P6_03);
	SBandVolumeThumbwheelSwitch.SetReference(P6_TW_POS[2], P6_SW_AXIS);
	SBandVolumeThumbwheelSwitch.DefineMeshGroup(VC_GRP_TW_P6_03);

	MainPanelVC.AddSwitch(&PowerMasterVolumeThumbwheelSwitch, AID_VC_TW_P6_04);
	PowerMasterVolumeThumbwheelSwitch.SetReference(P6_TW_POS[3], P6_SW_AXIS);
	PowerMasterVolumeThumbwheelSwitch.DefineMeshGroup(VC_GRP_TW_P6_04);

	MainPanelVC.AddSwitch(&IntercomVolumeThumbwheelSwitch, AID_VC_TW_P6_05);
	IntercomVolumeThumbwheelSwitch.SetReference(P6_TW_POS[4], P6_SW_AXIS);
	IntercomVolumeThumbwheelSwitch.DefineMeshGroup(VC_GRP_TW_P6_05);

	MainPanelVC.AddSwitch(&VHFAMVolumeThumbwheelSwitch, AID_VC_TW_P6_06);
	VHFAMVolumeThumbwheelSwitch.SetReference(P6_TW_POS[5], P6_SW_AXIS);
	VHFAMVolumeThumbwheelSwitch.DefineMeshGroup(VC_GRP_TW_P6_06);

	// Panel 7

	const VECTOR3 P7_SW_AXIS = { 0.887943158453019, -0.459835186361606, 0.010418672672886 };

	MainPanelVC.AddSwitch(&EDSPowerSwitch, AID_VC_SWITCH_P7_01);
	EDSPowerSwitch.SetReference(P7_TOGGLE_POS[0], P7_SW_AXIS);
	EDSPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P7_01);

	MainPanelVC.AddSwitch(&TVCServoPower1Switch, AID_VC_SWITCH_P7_02);
	TVCServoPower1Switch.SetReference(P7_TOGGLE_POS[1], P7_SW_AXIS);
	TVCServoPower1Switch.DefineMeshGroup(VC_GRP_Sw_P7_02);

	MainPanelVC.AddSwitch(&TVCServoPower2Switch, AID_VC_SWITCH_P7_03);
	TVCServoPower2Switch.SetReference(P7_TOGGLE_POS[2], P7_SW_AXIS);
	TVCServoPower2Switch.DefineMeshGroup(VC_GRP_Sw_P7_03);

	MainPanelVC.AddSwitch(&LogicPowerSwitch, AID_VC_SWITCH_P7_04);
	LogicPowerSwitch.SetReference(P7_TOGGLE_POS[3], P7_SW_AXIS);
	LogicPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P7_04);

	MainPanelVC.AddSwitch(&SIGCondDriverBiasPower1Switch, AID_VC_SWITCH_P7_05);
	SIGCondDriverBiasPower1Switch.SetReference(P7_TOGGLE_POS[4], P7_SW_AXIS);
	SIGCondDriverBiasPower1Switch.DefineMeshGroup(VC_GRP_Sw_P7_05);

	MainPanelVC.AddSwitch(&SIGCondDriverBiasPower2Switch, AID_VC_SWITCH_P7_06);
	SIGCondDriverBiasPower2Switch.SetReference(P7_TOGGLE_POS[5], P7_SW_AXIS);
	SIGCondDriverBiasPower2Switch.DefineMeshGroup(VC_GRP_Sw_P7_06);

	const VECTOR3	P7_ROT_AXIS = { 0.447544925948547, 0.846191424802758, -0.289246628066756 };

	MainPanelVC.AddSwitch(&FDAIPowerRotarySwitch, AID_VC_ROT_P7_01);
	FDAIPowerRotarySwitch.SetReference(P7_ROT_POS[0], P7_ROT_AXIS);
	FDAIPowerRotarySwitch.DefineMeshGroup(VC_GRP_Rot_P7_01);

	MainPanelVC.AddSwitch(&SCSElectronicsPowerRotarySwitch, AID_VC_ROT_P7_02);
	SCSElectronicsPowerRotarySwitch.SetReference(P7_ROT_POS[1], P7_ROT_AXIS);
	SCSElectronicsPowerRotarySwitch.DefineMeshGroup(VC_GRP_Rot_P7_02);

	MainPanelVC.AddSwitch(&BMAGPowerRotary1Switch, AID_VC_ROT_P7_03);
	BMAGPowerRotary1Switch.SetReference(P7_ROT_POS[2], P7_ROT_AXIS);
	BMAGPowerRotary1Switch.DefineMeshGroup(VC_GRP_Rot_P7_03);

	MainPanelVC.AddSwitch(&BMAGPowerRotary2Switch, AID_VC_ROT_P7_04);
	BMAGPowerRotary2Switch.SetReference(P7_ROT_POS[3], P7_ROT_AXIS);
	BMAGPowerRotary2Switch.DefineMeshGroup(VC_GRP_Rot_P7_04);

	MainPanelVC.AddSwitch(&DirectO2RotarySwitch, AID_VC_ROT_P7_05);
	DirectO2RotarySwitch.SetReference(P7_ROT_POS[4], P7_ROT_AXIS);
	DirectO2RotarySwitch.DefineMeshGroup(VC_GRP_Rot_P7_05);

	// Panel 8

	const VECTOR3 P8_SW_AXIS = { 0.522444235332028, -0.207197823992405, 0.82711612407276 };

	MainPanelVC.AddSwitch(&AcRollA1Switch, AID_VC_SWITCH_P8_01);
	AcRollA1Switch.SetReference(P8_TOGGLE_POS[0], P8_SW_AXIS);
	AcRollA1Switch.DefineMeshGroup(VC_GRP_Sw_P8_01);

	MainPanelVC.AddSwitch(&AcRollC1Switch, AID_VC_SWITCH_P8_02);
	AcRollC1Switch.SetReference(P8_TOGGLE_POS[1], P8_SW_AXIS);
	AcRollC1Switch.DefineMeshGroup(VC_GRP_Sw_P8_02);

	MainPanelVC.AddSwitch(&AcRollA2Switch, AID_VC_SWITCH_P8_03);
	AcRollA2Switch.SetReference(P8_TOGGLE_POS[2], P8_SW_AXIS);
	AcRollA2Switch.DefineMeshGroup(VC_GRP_Sw_P8_03);

	MainPanelVC.AddSwitch(&AcRollC2Switch, AID_VC_SWITCH_P8_04);
	AcRollC2Switch.SetReference(P8_TOGGLE_POS[3], P8_SW_AXIS);
	AcRollC2Switch.DefineMeshGroup(VC_GRP_Sw_P8_04);

	MainPanelVC.AddSwitch(&BdRollB1Switch, AID_VC_SWITCH_P8_05);
	BdRollB1Switch.SetReference(P8_TOGGLE_POS[4], P8_SW_AXIS);
	BdRollB1Switch.DefineMeshGroup(VC_GRP_Sw_P8_05);

	MainPanelVC.AddSwitch(&BdRollD1Switch, AID_VC_SWITCH_P8_06);
	BdRollD1Switch.SetReference(P8_TOGGLE_POS[5], P8_SW_AXIS);
	BdRollD1Switch.DefineMeshGroup(VC_GRP_Sw_P8_06);

	MainPanelVC.AddSwitch(&BdRollB2Switch, AID_VC_SWITCH_P8_07);
	BdRollB2Switch.SetReference(P8_TOGGLE_POS[6], P8_SW_AXIS);
	BdRollB2Switch.DefineMeshGroup(VC_GRP_Sw_P8_07);

	MainPanelVC.AddSwitch(&BdRollD2Switch, AID_VC_SWITCH_P8_08);
	BdRollD2Switch.SetReference(P8_TOGGLE_POS[7], P8_SW_AXIS);
	BdRollD2Switch.DefineMeshGroup(VC_GRP_Sw_P8_08);

	MainPanelVC.AddSwitch(&PitchA3Switch, AID_VC_SWITCH_P8_09);
	PitchA3Switch.SetReference(P8_TOGGLE_POS[8], P8_SW_AXIS);
	PitchA3Switch.DefineMeshGroup(VC_GRP_Sw_P8_09);

	MainPanelVC.AddSwitch(&PitchC3Switch, AID_VC_SWITCH_P8_10);
	PitchC3Switch.SetReference(P8_TOGGLE_POS[9], P8_SW_AXIS);
	PitchC3Switch.DefineMeshGroup(VC_GRP_Sw_P8_10);

	MainPanelVC.AddSwitch(&PitchA4Switch, AID_VC_SWITCH_P8_11);
	PitchA4Switch.SetReference(P8_TOGGLE_POS[10], P8_SW_AXIS);
	PitchA4Switch.DefineMeshGroup(VC_GRP_Sw_P8_11);

	MainPanelVC.AddSwitch(&PitchC4Switch, AID_VC_SWITCH_P8_12);
	PitchC4Switch.SetReference(P8_TOGGLE_POS[11], P8_SW_AXIS);
	PitchC4Switch.DefineMeshGroup(VC_GRP_Sw_P8_12);

	MainPanelVC.AddSwitch(&YawB3Switch, AID_VC_SWITCH_P8_13);
	YawB3Switch.SetReference(P8_TOGGLE_POS[12], P8_SW_AXIS);
	YawB3Switch.DefineMeshGroup(VC_GRP_Sw_P8_13);

	MainPanelVC.AddSwitch(&YawD3Switch, AID_VC_SWITCH_P8_14);
	YawD3Switch.SetReference(P8_TOGGLE_POS[13], P8_SW_AXIS);
	YawD3Switch.DefineMeshGroup(VC_GRP_Sw_P8_14);

	MainPanelVC.AddSwitch(&YawB4Switch, AID_VC_SWITCH_P8_15);
	YawB4Switch.SetReference(P8_TOGGLE_POS[14], P8_SW_AXIS);
	YawB4Switch.DefineMeshGroup(VC_GRP_Sw_P8_15);

	MainPanelVC.AddSwitch(&YawD4Switch, AID_VC_SWITCH_P8_16);
	YawD4Switch.SetReference(P8_TOGGLE_POS[15], P8_SW_AXIS);
	YawD4Switch.DefineMeshGroup(VC_GRP_Sw_P8_16);

	MainPanelVC.AddSwitch(&FloodDimSwitch, AID_VC_SWITCH_P8_17);
	FloodDimSwitch.SetReference(P8_TOGGLE_POS[16], P8_SW_AXIS);
	FloodDimSwitch.DefineMeshGroup(VC_GRP_Sw_P8_17);

	MainPanelVC.AddSwitch(&FloodFixedSwitch, AID_VC_SWITCH_P8_18);
	FloodFixedSwitch.SetReference(P8_TOGGLE_POS[17], P8_SW_AXIS);
	FloodFixedSwitch.DefineMeshGroup(VC_GRP_Sw_P8_18);

	MainPanelVC.AddSwitch(&FloatBagSwitch1, AID_VC_SWITCH_P8_19);
	FloatBagSwitch1.SetReference(P8_TOGGLE_POS[18], P8_SW_AXIS);
	FloatBagSwitch1.DefineMeshGroup(VC_GRP_Sw_P8_19);

	MainPanelVC.AddSwitch(&FloatBagSwitch2, AID_VC_SWITCH_P8_20);
	FloatBagSwitch2.SetReference(P8_TOGGLE_POS[19], P8_SW_AXIS);
	FloatBagSwitch2.DefineMeshGroup(VC_GRP_Sw_P8_20);

	MainPanelVC.AddSwitch(&FloatBagSwitch3, AID_VC_SWITCH_P8_21);
	FloatBagSwitch3.SetReference(P8_TOGGLE_POS[20], P8_SW_AXIS);
	FloatBagSwitch3.DefineMeshGroup(VC_GRP_Sw_P8_21);

	MainPanelVC.AddSwitch(&SECSLogic1Switch, AID_VC_SWITCH_P8_22);
	SECSLogic1Switch.SetReference(P8_TOGGLE_POS[21], P8_SW_AXIS);
	SECSLogic1Switch.DefineMeshGroup(VC_GRP_Sw_P8_22);

	MainPanelVC.AddSwitch(&SECSLogic2Switch, AID_VC_SWITCH_P8_23);
	SECSLogic2Switch.SetReference(P8_TOGGLE_POS[22], P8_SW_AXIS);
	SECSLogic2Switch.DefineMeshGroup(VC_GRP_Sw_P8_23);

	MainPanelVC.AddSwitch(&PyroArmASwitch, AID_VC_SWITCH_P8_24);
	PyroArmASwitch.SetReference(P8_TOGGLE_POS[23], P8_SW_AXIS);
	PyroArmASwitch.DefineMeshGroup(VC_GRP_Sw_P8_24);

	MainPanelVC.AddSwitch(&PyroArmBSwitch, AID_VC_SWITCH_P8_25);
	PyroArmBSwitch.SetReference(P8_TOGGLE_POS[24], P8_SW_AXIS);
	PyroArmBSwitch.DefineMeshGroup(VC_GRP_Sw_P8_25);

	const VECTOR3	P8_ROT_AXIS = { 0.844819075554255, 0.0, -0.535052081184303 };

	MainPanelVC.AddSwitch(&NumericRotarySwitch, AID_VC_ROT_P8_01);
	NumericRotarySwitch.SetReference(P8_ROT_POS[0], P8_ROT_AXIS);
	NumericRotarySwitch.DefineMeshGroup(VC_GRP_Rot_P8_01);
	NumericRotarySwitch.SetInitialAnimState(0.5);

	MainPanelVC.AddSwitch(&FloodRotarySwitch, AID_VC_ROT_P8_02);
	FloodRotarySwitch.SetReference(P8_ROT_POS[1], P8_ROT_AXIS);
	FloodRotarySwitch.DefineMeshGroup(VC_GRP_Rot_P8_02);
	FloodRotarySwitch.SetInitialAnimState(0.5);

	MainPanelVC.AddSwitch(&IntegralRotarySwitch, AID_VC_ROT_P8_03);
	IntegralRotarySwitch.SetReference(P8_ROT_POS[2], P8_ROT_AXIS);
	IntegralRotarySwitch.DefineMeshGroup(VC_GRP_Rot_P8_03);
	IntegralRotarySwitch.SetInitialAnimState(0.5);

	const VECTOR3 cb_p8_vector = { -0.844819075554255 * 0.003, 0.0, 0.535052081184303 * 0.003 };

	CircuitBrakerSwitch* breakerspanel8[P8_CBCOUNT] = { &StabContSystemTVCAc1CircuitBraker, &StabContSystemAc1CircuitBraker, &StabContSystemAc2CircuitBraker, &ECATVCAc2CircuitBraker, &DirectUllMnACircuitBraker, &DirectUllMnBCircuitBraker, &ContrDirectMnA1CircuitBraker, &ContrDirectMnB1CircuitBraker, &ContrDirectMnA2CircuitBraker,
		&ContrDirectMnB2CircuitBraker, &ACRollMnACircuitBraker, &ACRollMnBCircuitBraker, &BDRollMnACircuitBraker, &BDRollMnBCircuitBraker, &PitchMnACircuitBraker, &PitchMnBCircuitBraker, &YawMnACircuitBraker, &YawMnBCircuitBraker, &OrdealAc2CircuitBraker, &OrdealMnBCircuitBraker, &ContrAutoMnACircuitBraker, &ContrAutoMnBCircuitBraker, &LogicBus12MnACircuitBraker,
		&LogicBus34MnACircuitBraker, &LogicBus14MnBCircuitBraker, &LogicBus23MnBCircuitBraker, &SystemMnACircuitBraker, &SystemMnBCircuitBraker, &CMHeater1MnACircuitBraker, &CMHeater2MnBCircuitBraker, &SMHeatersAMnBCircuitBraker, &SMHeatersCMnBCircuitBraker, &SMHeatersBMnACircuitBraker, &SMHeatersDMnACircuitBraker, &PrplntIsolMnACircuitBraker, &PrplntIsolMnBCircuitBraker,
		&RCSLogicMnACircuitBraker, &RCSLogicMnBCircuitBraker, &EMSMnACircuitBraker, &EMSMnBCircuitBraker, &DockProbeMnACircuitBraker, &DockProbeMnBCircuitBraker, &GaugingMnACircuitBraker, &GaugingMnBCircuitBraker, &GaugingAc1CircuitBraker, &GaugingAc2CircuitBraker, &HeValveMnACircuitBraker, &HeValveMnBCircuitBraker, &PitchBatACircuitBraker, &PitchBatBCircuitBraker,
		&YawBatACircuitBraker, &YawBatBCircuitBraker, &PilotValveMnACircuitBraker, &PilotValveMnBCircuitBraker, &FloatBag1BatACircuitBraker, &FloatBag2BatBCircuitBraker, &FloatBag3FLTPLCircuitBraker, &SECSLogicBatACircuitBraker, &SECSLogicBatBCircuitBraker, &SECSArmBatACircuitBraker, &SECSArmBatBCircuitBraker, &EDS1BatACircuitBraker, &EDS2BatCCircuitBraker,
	    &EDS3BatBCircuitBraker, &ELSBatACircuitBraker, &ELSBatBCircuitBraker, &FLTPLCircuitBraker };

	for (int i = 0; i < P8_CBCOUNT; i++)
	{
		MainPanelVC.AddSwitch(breakerspanel8[i], AID_VC_CB_P8_01 + i);
		breakerspanel8[i]->SetDirection(cb_p8_vector);
		breakerspanel8[i]->DefineMeshGroup(VC_GRP_CB_P8_01 + i);
	}

	// Panel 9

	const VECTOR3 P9_SW_AXIS = { 0.263822320021041, -0.523246728773619, 0.810315151213663 };

	MainPanelVC.AddSwitch(&LeftModeIntercomSwitch, AID_VC_SWITCH_P9_01);
	LeftModeIntercomSwitch.SetReference(P9_TOGGLE_POS[0], P9_SW_AXIS);
	LeftModeIntercomSwitch.DefineMeshGroup(VC_GRP_Sw_P9_01);

	MainPanelVC.AddSwitch(&LeftPadCommSwitch, AID_VC_SWITCH_P9_02);
	LeftPadCommSwitch.SetReference(P9_TOGGLE_POS[1], P9_SW_AXIS);
	LeftPadCommSwitch.DefineMeshGroup(VC_GRP_Sw_P9_02);

	MainPanelVC.AddSwitch(&LeftSBandSwitch, AID_VC_SWITCH_P9_03);
	LeftSBandSwitch.SetReference(P9_TOGGLE_POS[2], P9_SW_AXIS);
	LeftSBandSwitch.DefineMeshGroup(VC_GRP_Sw_P9_03);

	MainPanelVC.AddSwitch(&LeftAudioPowerSwitch, AID_VC_SWITCH_P9_04);
	LeftAudioPowerSwitch.SetReference(P9_TOGGLE_POS[3], P9_SW_AXIS);
	LeftAudioPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P9_04);

	MainPanelVC.AddSwitch(&LeftIntercomSwitch, AID_VC_SWITCH_P9_05);
	LeftIntercomSwitch.SetReference(P9_TOGGLE_POS[4], P9_SW_AXIS);
	LeftIntercomSwitch.DefineMeshGroup(VC_GRP_Sw_P9_05);

	MainPanelVC.AddSwitch(&LeftVHFAMSwitch, AID_VC_SWITCH_P9_06);
	LeftVHFAMSwitch.SetReference(P9_TOGGLE_POS[5], P9_SW_AXIS);
	LeftVHFAMSwitch.DefineMeshGroup(VC_GRP_Sw_P9_06);

	MainPanelVC.AddSwitch(&LeftAudioControlSwitch, AID_VC_SWITCH_P9_07);
	LeftAudioControlSwitch.SetReference(P9_TOGGLE_POS[6], P9_SW_AXIS);
	LeftAudioControlSwitch.DefineMeshGroup(VC_GRP_Sw_P9_07);

	MainPanelVC.AddSwitch(&LeftSuitPowerSwitch, AID_VC_SWITCH_P9_08);
	LeftSuitPowerSwitch.SetReference(P9_TOGGLE_POS[7], P9_SW_AXIS);
	LeftSuitPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P9_08);

	MainPanelVC.AddSwitch(&VHFRNGSwitch, AID_VC_SWITCH_P9_09);
	VHFRNGSwitch.SetReference(P9_TOGGLE_POS[8], P9_SW_AXIS);
	VHFRNGSwitch.DefineMeshGroup(VC_GRP_Sw_P9_09);

	MainPanelVC.AddSwitch(&LeftModeIntercomVOXSensThumbwheelSwitch, AID_VC_TW_P9_01);
	LeftModeIntercomVOXSensThumbwheelSwitch.SetReference(P9_TW_POS[0], P9_SW_AXIS);
	LeftModeIntercomVOXSensThumbwheelSwitch.DefineMeshGroup(VC_GRP_TW_P9_01);

	MainPanelVC.AddSwitch(&LeftPadCommVolumeThumbwheelSwitch, AID_VC_TW_P9_02);
	LeftPadCommVolumeThumbwheelSwitch.SetReference(P9_TW_POS[1], P9_SW_AXIS);
	LeftPadCommVolumeThumbwheelSwitch.DefineMeshGroup(VC_GRP_TW_P9_02);

	MainPanelVC.AddSwitch(&LeftSBandVolumeThumbwheelSwitch, AID_VC_TW_P9_03);
	LeftSBandVolumeThumbwheelSwitch.SetReference(P9_TW_POS[2], P9_SW_AXIS);
	LeftSBandVolumeThumbwheelSwitch.DefineMeshGroup(VC_GRP_TW_P9_03);

	MainPanelVC.AddSwitch(&LeftPowerMasterVolumeThumbwheelSwitch, AID_VC_TW_P9_04);
	LeftPowerMasterVolumeThumbwheelSwitch.SetReference(P9_TW_POS[3], P9_SW_AXIS);
	LeftPowerMasterVolumeThumbwheelSwitch.DefineMeshGroup(VC_GRP_TW_P9_04);

	MainPanelVC.AddSwitch(&LeftIntercomVolumeThumbwheelSwitch, AID_VC_TW_P9_05);
	LeftIntercomVolumeThumbwheelSwitch.SetReference(P9_TW_POS[4], P9_SW_AXIS);
	LeftIntercomVolumeThumbwheelSwitch.DefineMeshGroup(VC_GRP_TW_P9_05);

	MainPanelVC.AddSwitch(&LeftVHFAMVolumeThumbwheelSwitch, AID_VC_TW_P9_06);
	LeftVHFAMVolumeThumbwheelSwitch.SetReference(P9_TW_POS[5], P9_SW_AXIS);
	LeftVHFAMVolumeThumbwheelSwitch.DefineMeshGroup(VC_GRP_TW_P9_06);

	// Panel 10

	const VECTOR3 P10_SW_AXIS = { -1, 0, 0 };

	MainPanelVC.AddSwitch(&ModeIntercomSwitch, AID_VC_SWITCH_P10_01);
	ModeIntercomSwitch.SetReference(P10_TOGGLE_POS[0], P10_SW_AXIS);
	ModeIntercomSwitch.DefineMeshGroup(VC_GRP_Sw_P10_01);

	MainPanelVC.AddSwitch(&PadComSwitch, AID_VC_SWITCH_P10_02);
	PadComSwitch.SetReference(P10_TOGGLE_POS[1], P10_SW_AXIS);
	PadComSwitch.DefineMeshGroup(VC_GRP_Sw_P10_02);

	MainPanelVC.AddSwitch(&SBandSwitch, AID_VC_SWITCH_P10_03);
	SBandSwitch.SetReference(P10_TOGGLE_POS[2], P10_SW_AXIS);
	SBandSwitch.DefineMeshGroup(VC_GRP_Sw_P10_03);

	MainPanelVC.AddSwitch(&CenterSuitPowerSwitch, AID_VC_SWITCH_P10_04);
	CenterSuitPowerSwitch.SetReference(P10_TOGGLE_POS[3], P10_SW_AXIS);
	CenterSuitPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P10_04);

	MainPanelVC.AddSwitch(&CenterAudioControlSwitch, AID_VC_SWITCH_P10_05);
	CenterAudioControlSwitch.SetReference(P10_TOGGLE_POS[4], P10_SW_AXIS);
	CenterAudioControlSwitch.DefineMeshGroup(VC_GRP_Sw_P10_05);

	MainPanelVC.AddSwitch(&PowerAudioSwitch, AID_VC_SWITCH_P10_06);
	PowerAudioSwitch.SetReference(P10_TOGGLE_POS[5], P10_SW_AXIS);
	PowerAudioSwitch.DefineMeshGroup(VC_GRP_Sw_P10_06);

	MainPanelVC.AddSwitch(&IntercomSwitch, AID_VC_SWITCH_P10_07);
	IntercomSwitch.SetReference(P10_TOGGLE_POS[6], P10_SW_AXIS);
	IntercomSwitch.DefineMeshGroup(VC_GRP_Sw_P10_07);

	MainPanelVC.AddSwitch(&VHFAMSwitch, AID_VC_SWITCH_P10_08);
	VHFAMSwitch.SetReference(P10_TOGGLE_POS[7], P10_SW_AXIS);
	VHFAMSwitch.DefineMeshGroup(VC_GRP_Sw_P10_08);

	MainPanelVC.AddSwitch(&LeftAudioVOXSensThumbwheel, AID_VC_TW_P10_01);
	LeftAudioVOXSensThumbwheel.SetReference(P10_TW_POS[0], P10_SW_AXIS);
	LeftAudioVOXSensThumbwheel.DefineMeshGroup(VC_GRP_TW_P10_01);

	MainPanelVC.AddSwitch(&LeftAudioPadComVolumeThumbwheel, AID_VC_TW_P10_02);
	LeftAudioPadComVolumeThumbwheel.SetReference(P10_TW_POS[1], P10_SW_AXIS);
	LeftAudioPadComVolumeThumbwheel.DefineMeshGroup(VC_GRP_TW_P10_02);

	MainPanelVC.AddSwitch(&LeftAudioSBandVolumeThumbwheel, AID_VC_TW_P10_03);
	LeftAudioSBandVolumeThumbwheel.SetReference(P10_TW_POS[2], P10_SW_AXIS);
	LeftAudioSBandVolumeThumbwheel.DefineMeshGroup(VC_GRP_TW_P10_03);

	MainPanelVC.AddSwitch(&RightAudioMasterVolumeThumbwheel, AID_VC_TW_P10_04);
	RightAudioMasterVolumeThumbwheel.SetReference(P10_TW_POS[3], P10_SW_AXIS);
	RightAudioMasterVolumeThumbwheel.DefineMeshGroup(VC_GRP_TW_P10_04);

	MainPanelVC.AddSwitch(&RightAudioIntercomVolumeThumbwheel, AID_VC_TW_P10_05);
	RightAudioIntercomVolumeThumbwheel.SetReference(P10_TW_POS[4], P10_SW_AXIS);
	RightAudioIntercomVolumeThumbwheel.DefineMeshGroup(VC_GRP_TW_P10_05);

	MainPanelVC.AddSwitch(&RightAudioVHFAMVolumeThumbwheel, AID_VC_TW_P10_06);
	RightAudioVHFAMVolumeThumbwheel.SetReference(P10_TW_POS[5], P10_SW_AXIS);
	RightAudioVHFAMVolumeThumbwheel.DefineMeshGroup(VC_GRP_TW_P10_06);

	// Panel 12

	const VECTOR3	P12_ROT_AXIS = { 0.690769935975231, -0.667400604807913, -0.278232507545052 };

	MainPanelVC.AddSwitch(&LMTunnelVentValve, AID_VC_ROT_P12_01);
	LMTunnelVentValve.SetReference(P12_ROT_POS[0], P12_ROT_AXIS);
	LMTunnelVentValve.DefineMeshGroup(VC_GRP_Rot_P12_01);

	NEEDLE_POS = { -0.3174, 0.1993, 0.4694 };

	MainPanelVC.AddSwitch(&LMDPGauge);
	LMDPGauge.SetReference(NEEDLE_POS, P12_ROT_AXIS);
	LMDPGauge.SetRotationRange(RAD * 75);
	LMDPGauge.DefineMeshGroup(VC_GRP_Needle_P12_01);

	// Panel 13

	const VECTOR3 P13_SW_AXIS = { 0.385804106483757, -0.378054512596731, 0.841564006434162 };

	MainPanelVC.AddSwitch(&ORDEALFDAI1Switch, AID_VC_SWITCH_P13_01);
	ORDEALFDAI1Switch.SetReference(P13_TOGGLE_POS[0], P13_SW_AXIS);
	ORDEALFDAI1Switch.DefineMeshGroup(VC_GRP_Sw_P13_01);

	MainPanelVC.AddSwitch(&ORDEALFDAI2Switch, AID_VC_SWITCH_P13_02);
	ORDEALFDAI2Switch.SetReference(P13_TOGGLE_POS[1], P13_SW_AXIS);
	ORDEALFDAI2Switch.DefineMeshGroup(VC_GRP_Sw_P13_02);

	MainPanelVC.AddSwitch(&ORDEALEarthSwitch, AID_VC_SWITCH_P13_03);
	ORDEALEarthSwitch.SetReference(P13_TOGGLE_POS[2], P13_SW_AXIS);
	ORDEALEarthSwitch.DefineMeshGroup(VC_GRP_Sw_P13_03);

	MainPanelVC.AddSwitch(&ORDEALLightingSwitch, AID_VC_SWITCH_P13_04);
	ORDEALLightingSwitch.SetReference(P13_TOGGLE_POS[3], P13_SW_AXIS);
	ORDEALLightingSwitch.DefineMeshGroup(VC_GRP_Sw_P13_04);

	MainPanelVC.AddSwitch(&ORDEALModeSwitch, AID_VC_SWITCH_P13_05);
	ORDEALModeSwitch.SetReference(P13_TOGGLE_POS[4], P13_SW_AXIS);
	ORDEALModeSwitch.DefineMeshGroup(VC_GRP_Sw_P13_05);

	MainPanelVC.AddSwitch(&ORDEALSlewSwitch, AID_VC_SWITCH_P13_06);
	ORDEALSlewSwitch.SetReference(P13_TOGGLE_POS[5], P13_SW_AXIS);
	ORDEALSlewSwitch.DefineMeshGroup(VC_GRP_Sw_P13_06);

	const VECTOR3	P13_ROT_AXIS = { 0.54191307344258, -0.645820796385401, -0.53781569314045 };
	const VECTOR3 ORDEAL_RotLocation = { -0.9256, 0.9741, -0.0737 };

	MainPanelVC.AddSwitch(&ORDEALAltSetRotary, AID_VC_ORDEAL_ROT);
	ORDEALAltSetRotary.SetReference(ORDEAL_RotLocation, P13_ROT_AXIS);
	ORDEALAltSetRotary.DefineMeshGroup(VC_GRP_ORDEAL_Rot);
	ORDEALAltSetRotary.SetInitialAnimState(133.0 / 285.0); //133° from 10 NM to 150 NM, 285° total range

	// Panel 15

	const VECTOR3 P15_SW_AXIS = { 0.263822320021041, -0.523246728773619, 0.810315151213663 };
	const VECTOR3 P15_COVER_AXIS = { 0.358290506720357, -0.354736633295221, 0.863591242308791 };

	MainPanelVC.AddSwitch(&LeftCOASPowerSwitch, AID_VC_SWITCH_P15_01);
	LeftCOASPowerSwitch.SetReference(P15_TOGGLE_POS[0], P15_SW_AXIS);
	LeftCOASPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P15_01);

	MainPanelVC.AddSwitch(&LeftUtilityPowerSwitch, AID_VC_SWITCH_P15_02);
	LeftUtilityPowerSwitch.SetReference(P15_TOGGLE_POS[1], P15_SW_AXIS);
	LeftUtilityPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P15_02);

	MainPanelVC.AddSwitch(&PostLandingBCNLTSwitch, AID_VC_SWITCH_P15_03);
	PostLandingBCNLTSwitch.SetReference(P15_TOGGLE_POS[2], P15_SW_AXIS);
	PostLandingBCNLTSwitch.DefineMeshGroup(VC_GRP_Sw_P15_03);

	MainPanelVC.AddSwitch(&PostLandingDYEMarkerSwitch, AID_VC_SWITCH_P15_04);
	PostLandingDYEMarkerSwitch.SetReference(P15_TOGGLE_POS[3], _V(-0.774513, 0.881142, 0.309977), P15_SW_AXIS, P15_COVER_AXIS);
	PostLandingDYEMarkerSwitch.DefineMeshGroup(VC_GRP_Sw_P15_04, VC_GRP_SwitchCover_P15_01);

	MainPanelVC.AddSwitch(&PostLandingVentSwitch, AID_VC_SWITCH_P15_05);
	PostLandingVentSwitch.SetReference(P15_TOGGLE_POS[4], P15_SW_AXIS);
	PostLandingVentSwitch.DefineMeshGroup(VC_GRP_Sw_P15_05);

	const VECTOR3 P16_SW_AXIS = { 0.440928737513677, 0.332607488651675, -0.833639074736436 };

	MainPanelVC.AddSwitch(&RightDockingTargetSwitch, AID_VC_SWITCH_P16_01);
	RightDockingTargetSwitch.SetReference(P16_TOGGLE_POS[0], P16_SW_AXIS);
	RightDockingTargetSwitch.DefineMeshGroup(VC_GRP_Sw_P16_01);

	MainPanelVC.AddSwitch(&RightUtilityPowerSwitch, AID_VC_SWITCH_P16_02);
	RightUtilityPowerSwitch.SetReference(P16_TOGGLE_POS[1], P16_SW_AXIS);
	RightUtilityPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P16_02);

	MainPanelVC.AddSwitch(&RightCOASPowerSwitch, AID_VC_SWITCH_P16_03);
	RightCOASPowerSwitch.SetReference(P16_TOGGLE_POS[2], P16_SW_AXIS);
	RightCOASPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P16_03);

	// Panel 100

	MainPanelVC.AddSwitch(&UtilityPowerSwitch, AID_VC_SWITCH_P100_01);
	UtilityPowerSwitch.SetReference(P100_TOGGLE_POS[0], _V(1, 0, 0));
	UtilityPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P100_01);

	MainPanelVC.AddSwitch(&Panel100FloodDimSwitch, AID_VC_SWITCH_P100_02);
	Panel100FloodDimSwitch.SetReference(P100_TOGGLE_POS[1], _V(1, 0, 0));
	Panel100FloodDimSwitch.DefineMeshGroup(VC_GRP_Sw_P100_02);

	MainPanelVC.AddSwitch(&Panel100FloodFixedSwitch, AID_VC_SWITCH_P100_03);
	Panel100FloodFixedSwitch.SetReference(P100_TOGGLE_POS[2], _V(1, 0, 0));
	Panel100FloodFixedSwitch.DefineMeshGroup(VC_GRP_Sw_P100_03);

	MainPanelVC.AddSwitch(&GNPowerOpticsSwitch, AID_VC_SWITCH_P100_04);
	GNPowerOpticsSwitch.SetReference(P100_TOGGLE_POS[3], _V(1, 0, 0));
	GNPowerOpticsSwitch.DefineMeshGroup(VC_GRP_Sw_P100_04);

	MainPanelVC.AddSwitch(&GNPowerIMUSwitch, AID_VC_SWITCH_P100_05);
	GNPowerIMUSwitch.SetReference(P100_TOGGLE_POS[4], _V(-0.581695, -0.697935, 0.239086), _V(1, 0, 0), _V(1, 0, 0));
	GNPowerIMUSwitch.DefineMeshGroup(VC_GRP_Sw_P100_05, VC_GRP_SwitchCover_P100_01);

	MainPanelVC.AddSwitch(&Panel100RNDZXPDRSwitch, AID_VC_SWITCH_P100_06);
	Panel100RNDZXPDRSwitch.SetReference(P100_TOGGLE_POS[5], _V(1, 0, 0));
	Panel100RNDZXPDRSwitch.DefineMeshGroup(VC_GRP_Sw_P100_06);

	const VECTOR3	P100_ROT_AXIS = { 0, 1, 0 };

	MainPanelVC.AddSwitch(&Panel100NumericRotarySwitch, AID_VC_ROT_P100_01);
	Panel100NumericRotarySwitch.SetReference(P100_ROT_POS[0], P100_ROT_AXIS);
	Panel100NumericRotarySwitch.DefineMeshGroup(VC_GRP_Rot_P100_01);
	Panel100NumericRotarySwitch.SetInitialAnimState(0.5);

	MainPanelVC.AddSwitch(&Panel100FloodRotarySwitch, AID_VC_ROT_P100_02);
	Panel100FloodRotarySwitch.SetReference(P100_ROT_POS[1], P100_ROT_AXIS);
	Panel100FloodRotarySwitch.DefineMeshGroup(VC_GRP_Rot_P100_02);
	Panel100FloodRotarySwitch.SetInitialAnimState(0.5);

	MainPanelVC.AddSwitch(&Panel100IntegralRotarySwitch, AID_VC_ROT_P100_03);
	Panel100IntegralRotarySwitch.SetReference(P100_ROT_POS[2], P100_ROT_AXIS);
	Panel100IntegralRotarySwitch.DefineMeshGroup(VC_GRP_Rot_P100_03);
	Panel100IntegralRotarySwitch.SetInitialAnimState(0.5);

	// Panel 101

	MainPanelVC.AddSwitch(&RNDZXPDRSwitch, AID_VC_SWITCH_P101_01);
	RNDZXPDRSwitch.SetReference(P101_TOGGLE_POS[0], _V(1, 0, 0));
	RNDZXPDRSwitch.DefineMeshGroup(VC_GRP_Sw_P101_01);

	MainPanelVC.AddSwitch(&CMRCSHTRSSwitch, AID_VC_SWITCH_P101_02);
	CMRCSHTRSSwitch.SetReference(P101_TOGGLE_POS[1], _V(1, 0, 0));
	CMRCSHTRSSwitch.DefineMeshGroup(VC_GRP_Sw_P101_02);

	MainPanelVC.AddSwitch(&WasteH2ODumpSwitch, AID_VC_SWITCH_P101_03);
	WasteH2ODumpSwitch.SetReference(P101_TOGGLE_POS[2], _V(1, 0, 0));
	WasteH2ODumpSwitch.DefineMeshGroup(VC_GRP_Sw_P101_03);

	MainPanelVC.AddSwitch(&UrineDumpSwitch, AID_VC_SWITCH_P101_04);
	UrineDumpSwitch.SetReference(P101_TOGGLE_POS[3], _V(1, 0, 0));
	UrineDumpSwitch.DefineMeshGroup(VC_GRP_Sw_P101_04);

	const VECTOR3	P101_ROT_AXIS = { 0, 0.938288430820701, -0.345853756070434 };

	MainPanelVC.AddSwitch(&LeftSystemTestRotarySwitch, AID_VC_ROT_P101_01);
	LeftSystemTestRotarySwitch.SetReference(P101_ROT_POS[0], P101_ROT_AXIS);
	LeftSystemTestRotarySwitch.DefineMeshGroup(VC_GRP_Rot_P101_01);

	MainPanelVC.AddSwitch(&RightSystemTestRotarySwitch, AID_VC_ROT_P101_02);
	RightSystemTestRotarySwitch.SetReference(P101_ROT_POS[1], P101_ROT_AXIS);
	RightSystemTestRotarySwitch.DefineMeshGroup(VC_GRP_Rot_P101_02);

	NEEDLE_POS = { -0.4320, -0.5961, 0.5486 };

	MainPanelVC.AddSwitch(&SystemTestVoltMeter);
	SystemTestVoltMeter.SetReference(NEEDLE_POS, P101_ROT_AXIS);
	SystemTestVoltMeter.SetRotationRange(RAD * 225);
	SystemTestVoltMeter.DefineMeshGroup(VC_GRP_Needle_P101_01);

	// Panel 122

	MainPanelVC.AddSwitch(&OpticsZeroSwitch, AID_VC_SWITCH_P122_01);
	OpticsZeroSwitch.SetReference(P122_TOGGLE_POS[0], _V(1, 0, 0));
	OpticsZeroSwitch.DefineMeshGroup(VC_GRP_Sw_P122_01);

	MainPanelVC.AddSwitch(&ControllerTelescopeTrunnionSwitch, AID_VC_SWITCH_P122_02);
	ControllerTelescopeTrunnionSwitch.SetReference(P122_TOGGLE_POS[1], _V(1, 0, 0));
	ControllerTelescopeTrunnionSwitch.DefineMeshGroup(VC_GRP_Sw_P122_02);

	MainPanelVC.AddSwitch(&ControllerCouplingSwitch, AID_VC_SWITCH_P122_03);
	ControllerCouplingSwitch.SetReference(P122_TOGGLE_POS[2], _V(1, 0, 0));
	ControllerCouplingSwitch.DefineMeshGroup(VC_GRP_Sw_P122_03);

	MainPanelVC.AddSwitch(&OpticsModeSwitch, AID_VC_SWITCH_P122_04);
	OpticsModeSwitch.SetReference(P122_TOGGLE_POS[3], _V(1, 0, 0));
	OpticsModeSwitch.DefineMeshGroup(VC_GRP_Sw_P122_04);

	MainPanelVC.AddSwitch(&ControllerSpeedSwitch, AID_VC_SWITCH_P122_05);
	ControllerSpeedSwitch.SetReference(P122_TOGGLE_POS[4], _V(1, 0, 0));
	ControllerSpeedSwitch.DefineMeshGroup(VC_GRP_Sw_P122_05);

	MainPanelVC.AddSwitch(&ConditionLampsSwitch, AID_VC_SWITCH_P122_06);
	ConditionLampsSwitch.SetReference(P122_TOGGLE_POS[5], _V(1, 0, 0));
	ConditionLampsSwitch.DefineMeshGroup(VC_GRP_Sw_P122_06);

	MainPanelVC.AddSwitch(&UPTLMSwitch, AID_VC_SWITCH_P122_07);
	UPTLMSwitch.SetReference(P122_TOGGLE_POS[6], _V(1, 0, 0));
	UPTLMSwitch.DefineMeshGroup(VC_GRP_Sw_P122_07);

	const VECTOR3 pb_P122_vector = {0 , -0.001, 0 };

	MainPanelVC.AddSwitch(&Dsky2SwitchVerb, AID_VC_PUSHB_P122_01);
	Dsky2SwitchVerb.SetDirection(pb_P122_vector);
	Dsky2SwitchVerb.DefineMeshGroup(VC_GRP_PB_P122_01);

	MainPanelVC.AddSwitch(&Dsky2SwitchNoun, AID_VC_PUSHB_P122_02);
	Dsky2SwitchNoun.SetDirection(pb_P122_vector);
	Dsky2SwitchNoun.DefineMeshGroup(VC_GRP_PB_P122_02);

	MainPanelVC.AddSwitch(&Dsky2SwitchPlus, AID_VC_PUSHB_P122_03);
	Dsky2SwitchPlus.SetDirection(pb_P122_vector);
	Dsky2SwitchPlus.DefineMeshGroup(VC_GRP_PB_P122_03);

	MainPanelVC.AddSwitch(&Dsky2SwitchMinus, AID_VC_PUSHB_P122_04);
	Dsky2SwitchMinus.SetDirection(pb_P122_vector);
	Dsky2SwitchMinus.DefineMeshGroup(VC_GRP_PB_P122_04);

	MainPanelVC.AddSwitch(&Dsky2SwitchZero, AID_VC_PUSHB_P122_05);
	Dsky2SwitchZero.SetDirection(pb_P122_vector);
	Dsky2SwitchZero.DefineMeshGroup(VC_GRP_PB_P122_05);

	MainPanelVC.AddSwitch(&Dsky2SwitchOne, AID_VC_PUSHB_P122_06);
	Dsky2SwitchOne.SetDirection(pb_P122_vector);
	Dsky2SwitchOne.DefineMeshGroup(VC_GRP_PB_P122_06);

	MainPanelVC.AddSwitch(&Dsky2SwitchTwo, AID_VC_PUSHB_P122_07);
	Dsky2SwitchTwo.SetDirection(pb_P122_vector);
	Dsky2SwitchTwo.DefineMeshGroup(VC_GRP_PB_P122_07);

	MainPanelVC.AddSwitch(&Dsky2SwitchThree, AID_VC_PUSHB_P122_08);
	Dsky2SwitchThree.SetDirection(pb_P122_vector);
	Dsky2SwitchThree.DefineMeshGroup(VC_GRP_PB_P122_08);

	MainPanelVC.AddSwitch(&Dsky2SwitchFour, AID_VC_PUSHB_P122_09);
	Dsky2SwitchFour.SetDirection(pb_P122_vector);
	Dsky2SwitchFour.DefineMeshGroup(VC_GRP_PB_P122_09);

	MainPanelVC.AddSwitch(&Dsky2SwitchFive, AID_VC_PUSHB_P122_10);
	Dsky2SwitchFive.SetDirection(pb_P122_vector);
	Dsky2SwitchFive.DefineMeshGroup(VC_GRP_PB_P122_10);

	MainPanelVC.AddSwitch(&Dsky2SwitchSix, AID_VC_PUSHB_P122_11);
	Dsky2SwitchSix.SetDirection(pb_P122_vector);
	Dsky2SwitchSix.DefineMeshGroup(VC_GRP_PB_P122_11);

	MainPanelVC.AddSwitch(&Dsky2SwitchSeven, AID_VC_PUSHB_P122_12);
	Dsky2SwitchSeven.SetDirection(pb_P122_vector);
	Dsky2SwitchSeven.DefineMeshGroup(VC_GRP_PB_P122_12);

	MainPanelVC.AddSwitch(&Dsky2SwitchEight, AID_VC_PUSHB_P122_13);
	Dsky2SwitchEight.SetDirection(pb_P122_vector);
	Dsky2SwitchEight.DefineMeshGroup(VC_GRP_PB_P122_13);

	MainPanelVC.AddSwitch(&Dsky2SwitchNine, AID_VC_PUSHB_P122_14);
	Dsky2SwitchNine.SetDirection(pb_P122_vector);
	Dsky2SwitchNine.DefineMeshGroup(VC_GRP_PB_P122_14);

	MainPanelVC.AddSwitch(&Dsky2SwitchClear, AID_VC_PUSHB_P122_15);
	Dsky2SwitchClear.SetDirection(pb_P122_vector);
	Dsky2SwitchClear.DefineMeshGroup(VC_GRP_PB_P122_15);

	MainPanelVC.AddSwitch(&Dsky2SwitchProceed, AID_VC_PUSHB_P122_16);
	Dsky2SwitchProceed.SetDirection(pb_P122_vector);
	Dsky2SwitchProceed.DefineMeshGroup(VC_GRP_PB_P122_16);

	MainPanelVC.AddSwitch(&Dsky2SwitchKeyRel, AID_VC_PUSHB_P122_17);
	Dsky2SwitchKeyRel.SetDirection(pb_P122_vector);
	Dsky2SwitchKeyRel.DefineMeshGroup(VC_GRP_PB_P122_17);

	MainPanelVC.AddSwitch(&Dsky2SwitchEnter, AID_VC_PUSHB_P122_18);
	Dsky2SwitchEnter.SetDirection(pb_P122_vector);
	Dsky2SwitchEnter.DefineMeshGroup(VC_GRP_PB_P122_18);

	MainPanelVC.AddSwitch(&Dsky2SwitchReset, AID_VC_PUSHB_P122_19);
	Dsky2SwitchReset.SetDirection(pb_P122_vector);
	Dsky2SwitchReset.DefineMeshGroup(VC_GRP_PB_P122_19);

	// Panel 306
	const VECTOR3	P306_SW_AXIS = { 0, -1, 0 };

	MainPanelVC.AddSwitch(&EventTimerUpDown306Switch, AID_VC_SWITCH_P306_01);
	EventTimerUpDown306Switch.SetReference(P306_TOGGLE_POS[0], P306_SW_AXIS);
	EventTimerUpDown306Switch.DefineMeshGroup(VC_GRP_Sw_P306_01);

	MainPanelVC.AddSwitch(&EventTimerControl306Switch, AID_VC_SWITCH_P306_02);
	EventTimerControl306Switch.SetReference(P306_TOGGLE_POS[1], P306_SW_AXIS);
	EventTimerControl306Switch.DefineMeshGroup(VC_GRP_Sw_P306_02);

	MainPanelVC.AddSwitch(&EventTimer306MinutesSwitch, AID_VC_SWITCH_P306_03);
	EventTimer306MinutesSwitch.SetReference(P306_TOGGLE_POS[2], P306_SW_AXIS);
	EventTimer306MinutesSwitch.DefineMeshGroup(VC_GRP_Sw_P306_03);

	MainPanelVC.AddSwitch(&EventTimer306SecondsSwitch, AID_VC_SWITCH_P306_04);
	EventTimer306SecondsSwitch.SetReference(P306_TOGGLE_POS[3], P306_SW_AXIS);
	EventTimer306SecondsSwitch.DefineMeshGroup(VC_GRP_Sw_P306_04);

	MainPanelVC.AddSwitch(&MissionTimer306HoursSwitch, AID_VC_SWITCH_P306_05);
	MissionTimer306HoursSwitch.SetReference(P306_TOGGLE_POS[4], P306_SW_AXIS);
	MissionTimer306HoursSwitch.DefineMeshGroup(VC_GRP_Sw_P306_05);

	MainPanelVC.AddSwitch(&MissionTimer306MinutesSwitch, AID_VC_SWITCH_P306_06);
	MissionTimer306MinutesSwitch.SetReference(P306_TOGGLE_POS[5], P306_SW_AXIS);
	MissionTimer306MinutesSwitch.DefineMeshGroup(VC_GRP_Sw_P306_06);

	MainPanelVC.AddSwitch(&MissionTimer306SecondsSwitch, AID_VC_SWITCH_P306_07);
	MissionTimer306SecondsSwitch.SetReference(P306_TOGGLE_POS[6], P306_SW_AXIS);
	MissionTimer306SecondsSwitch.DefineMeshGroup(VC_GRP_Sw_P306_07);

	MainPanelVC.AddSwitch(&MissionTimer306Switch, AID_VC_SWITCH_P306_08);
	MissionTimer306Switch.SetReference(P306_TOGGLE_POS[7], P306_SW_AXIS);
	MissionTimer306Switch.DefineMeshGroup(VC_GRP_Sw_P306_08);

	// Panel 325
	const VECTOR3	PRIMGLYHANDLE_VECT = { 0, 0, -0.01 };
	const VECTOR3	P325_HANDLE_AXIS = { 0, 1, 0 };

	MainPanelVC.AddSwitch(&GlycolToRadiatorsLever, AID_VC_Prim_Gly_Handle);
	GlycolToRadiatorsLever.SetDirection(PRIMGLYHANDLE_VECT);
	GlycolToRadiatorsLever.DefineMeshGroup(VC_GRP_Prim_Gly_Handle);

	MainPanelVC.AddSwitch(&CabinPressureReliefLever1, AID_VC_Cab_Press_Rel_Handle1);
	CabinPressureReliefLever1.SetReference(Cab_Press_Rel_Handle1Location, P325_HANDLE_AXIS);
	CabinPressureReliefLever1.SetRotationRange(50 * RAD);
	CabinPressureReliefLever1.DefineMeshGroup(VC_GRP_Cab_Press_Rel_Handle1);

	MainPanelVC.AddSwitch(&CabinPressureReliefLever2, AID_VC_Cab_Press_Rel_Handle2);
	CabinPressureReliefLever2.SetReference(Cab_Press_Rel_Handle2Location, P325_HANDLE_AXIS);
	CabinPressureReliefLever2.SetRotationRange(55 * RAD);
	CabinPressureReliefLever2.DefineMeshGroup(VC_GRP_Cab_Press_Rel_Handle2);

	// Panel 326

	const VECTOR3	P326_ROT_AXIS = { 0, 0, 1 };

	MainPanelVC.AddSwitch(&GlycolReservoirInletRotary, AID_VC_ROT_P326_01);
	GlycolReservoirInletRotary.SetReference(P326_ROT_POS[0], P326_ROT_AXIS);
	GlycolReservoirInletRotary.DefineMeshGroup(VC_GRP_Rot_P326_01);

	MainPanelVC.AddSwitch(&GlycolReservoirBypassRotary, AID_VC_ROT_P326_02);
	GlycolReservoirBypassRotary.SetReference(P326_ROT_POS[1], P326_ROT_AXIS);
	GlycolReservoirBypassRotary.DefineMeshGroup(VC_GRP_Rot_P326_02);

	MainPanelVC.AddSwitch(&GlycolReservoirOutletRotary, AID_VC_ROT_P326_03);
	GlycolReservoirOutletRotary.SetReference(P326_ROT_POS[2], P326_ROT_AXIS);
	GlycolReservoirOutletRotary.DefineMeshGroup(VC_GRP_Rot_P326_03);

	MainPanelVC.AddSwitch(&OxygenRepressPackageRotary, AID_VC_ROT_P326_04);
	OxygenRepressPackageRotary.SetReference(P326_ROT_POS[3], P326_ROT_AXIS);
	OxygenRepressPackageRotary.DefineMeshGroup(VC_GRP_Rot_P326_04);

	MainPanelVC.AddSwitch(&OxygenSMSupplyRotary, AID_VC_ROT_P326_05);
	OxygenSMSupplyRotary.SetReference(P326_ROT_POS[4], P326_ROT_AXIS);
	OxygenSMSupplyRotary.DefineMeshGroup(VC_GRP_Rot_P326_05);

	MainPanelVC.AddSwitch(&OxygenSurgeTankRotary, AID_VC_ROT_P326_06);
	OxygenSurgeTankRotary.SetReference(P326_ROT_POS[5], P326_ROT_AXIS);
	OxygenSurgeTankRotary.DefineMeshGroup(VC_GRP_Rot_P326_06);


	// LEB Right wall

	const VECTOR3 rot_leb_r_vector = { -1, 0, 0 };

	MainPanelVC.AddSwitch(&WasteMGMTOvbdDrainDumpRotary, AID_VC_ROT_LEB_R_01);
	WasteMGMTOvbdDrainDumpRotary.SetReference(LEB_R_ROT_POS[0], rot_leb_r_vector);
	WasteMGMTOvbdDrainDumpRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Right_01);

	MainPanelVC.AddSwitch(&WasteMGMTBatteryVentRotary, AID_VC_ROT_LEB_R_02);
	WasteMGMTBatteryVentRotary.SetReference(LEB_R_ROT_POS[1], rot_leb_r_vector);
	WasteMGMTBatteryVentRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Right_02);

	MainPanelVC.AddSwitch(&WasteMGMTStoageVentRotary, AID_VC_ROT_LEB_R_03);
	WasteMGMTStoageVentRotary.SetReference(LEB_R_ROT_POS[2], rot_leb_r_vector);
	WasteMGMTStoageVentRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Right_03);

	const VECTOR3 cb_leb_r_vector = { 0.003, 0.0, 0.0 };

	CircuitBrakerSwitch* breakersleb1[LEB_R1_CBCOUNT] = {

		/*Panel 229*/ &EPSMnBGroup1CircuitBraker, &EPSMnAGroup1CircuitBraker, &TimersMnACircuitBraker, &TimersMnBCircuitBraker, &EPSMnAGroup2CircuitBraker, &EPSMnBGroup2CircuitBraker, &SPSLineHtrsMnACircuitBraker, &SPSLineHtrsMnBCircuitBraker, &EPSMnAGroup3CircuitBraker,
		&EPSMnBGroup3CircuitBraker, &O2VacIonPumpsMnACircuitBraker, &O2VacIonPumpsMnBCircuitBraker, &EPSMnAGroup4CircuitBraker, &EPSMnBGroup4CircuitBraker, &MainReleasePyroACircuitBraker, &MainReleasePyroBCircuitBraker, &EPSMnAGroup5CircuitBraker, &EPSMnBGroup5CircuitBraker, &UtilityCB1, &UtilityCB2, 
		&EPSBatBusACircuitBraker, &EPSBatBusBCircuitBraker,

		/*Panel 225*/ &PCMTLMGroup1CB, &PCMTLMGroup2CB, &FLTBusMNACB, &FLTBusMNBCB, &PMPPowerPrimCB, &PMPPowerAuxCB, &VHFStationAudioLCB, &VHFStationAudioCTRCB, &VHFStationAudioRCB, &UDLCB, &HGAFLTBus1CB, &HGAGroup2CB, &SBandFMXMTRFLTBusCB, &SBandFMXMTRGroup1CB,
		&CentralTimingEquipMNACB, &CentralTimingEquipMNBCB, &RNDZXPNDRFLTBusCB, &SIGCondrFLTBusCB, &SBandPWRAmpl1FLTBusCB, &SBandPWRAmpl1Group1CB, &SBandPWRAmpl2FLTBusCB, &SBandPWRAmpl2Group1CB, 

		/*Panel 226*/ &FuelCell1PumpsACCB, &FuelCell1ReacsCB, &FuelCell1BusContCB, &FuelCell1PurgeCB, &FuelCell1RadCB, &CryogenicH2HTR1CB, &CryogenicH2HTR2CB, &FuelCell2PumpsACCB, &FuelCell2ReacsCB, &FuelCell2BusContCB, &FuelCell2PurgeCB, &FuelCell2RadCB, &CryogenicO2HTR1CB, &CryogenicO2HTR2CB, 
		&FuelCell3PumpsACCB, &FuelCell3ReacsCB, &FuelCell3BusContCB, &FuelCell3PurgeCB, &FuelCell3RadCB, &CryogenicQTYAmpl1CB, &CryogenicQTYAmpl2CB, &CryogenicFanMotorsAC1ACB, &CryogenicFanMotorsAC1BCB, &CryogenicFanMotorsAC1CCB, &CryogenicFanMotorsAC2ACB, &CryogenicFanMotorsAC2BCB, &CryogenicFanMotorsAC2CCB, 
		&LightingRndzMNACB, &LightingRndzMNBCB, &LightingFloodMNACB, &LightingFloodMNBCB, &LightingFloodFLTPLCB, &LightingNumIntLEBCB, &LightingNumIntLMDCCB, &LightingNumIntRMDCCB, &RunEVATRGTAC1CB, &RunEVATRGTAC2CB };

	for (int i = 0; i < LEB_R1_CBCOUNT; i++)
	{
		MainPanelVC.AddSwitch(breakersleb1[i], AID_VC_CB_LEB_R1_01 + i);
		breakersleb1[i]->SetDirection(cb_leb_r_vector);
		breakersleb1[i]->DefineMeshGroup(VC_GRP_CB_LEB_Right1_01 + i);
	}

	CircuitBrakerSwitch* breakersleb2[LEB_R2_CBCOUNT] = {
		/*Panel 250*/ &BatBusAToPyroBusTieCircuitBraker, &PyroASeqACircuitBraker, &BatBusBToPyroBusTieCircuitBraker, &PyroBSeqBCircuitBraker, &BatAPWRCircuitBraker, &BatBPWRCircuitBraker, &BatCPWRCircuitBraker, &BatCtoBatBusACircuitBraker, &BatCtoBatBusBCircuitBraker, &BatCCHRGCircuitBraker,

		/*Panel 275-278*/ &MainABatBusACircuitBraker, &MainABatCCircuitBraker, &MainBBatCCircuitBraker, &MainBBatBusBCircuitBraker, &FlightPostLandingBatBusACircuitBraker, &FlightPostLandingBatBusBCircuitBraker, &FlightPostLandingBatCCircuitBraker, &FlightPostLandingMainACircuitBraker, 
		&FlightPostLandingMainBCircuitBraker, &InverterPower1MainACircuitBraker, &InverterPower2MainBCircuitBraker, &InverterPower3MainACircuitBraker, &InverterPower3MainBCircuitBraker,
		&Panel276CB1, &Panel276CB2, &Panel276CB3, &Panel276CB4, &UprightingSystemCompressor1CircuitBraker, &UprightingSystemCompressor2CircuitBraker, &SIVBLMSepPyroACircuitBraker, &SIVBLMSepPyroBCircuitBraker };

	for (int i = 0; i < LEB_R2_CBCOUNT; i++)
	{
		MainPanelVC.AddSwitch(breakersleb2[i], AID_VC_CB_LEB_R2_01 + i);
		breakersleb2[i]->SetDirection(cb_leb_r_vector);
		breakersleb2[i]->DefineMeshGroup(VC_GRP_CB_LEB_Right2_01 + i);
	}

	// LEB Left Wall

	const VECTOR3 rot_leb_l_vector = { 1, 0, 0 };

	MainPanelVC.AddSwitch(&OxygenSurgeTankValveRotary, AID_VC_ROT_LEB_L_01);
	OxygenSurgeTankValveRotary.SetReference(LEB_L_ROT_POS[0], rot_leb_l_vector);
	OxygenSurgeTankValveRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_01);

	MainPanelVC.AddSwitch(&GlycolToRadiatorsRotary, AID_VC_ROT_LEB_L_02);
	GlycolToRadiatorsRotary.SetReference(LEB_L_ROT_POS[1], rot_leb_l_vector);
	GlycolToRadiatorsRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_02);

	MainPanelVC.AddSwitch(&GlycolRotary, AID_VC_ROT_LEB_L_03);
	GlycolRotary.SetReference(LEB_L_ROT_POS[2], rot_leb_l_vector);
	GlycolRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_03);

	MainPanelVC.AddSwitch(&AccumRotary, AID_VC_ROT_LEB_L_04);
	AccumRotary.SetReference(LEB_L_ROT_POS[3], rot_leb_l_vector);
	AccumRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_04);

	MainPanelVC.AddSwitch(&SuitHeatExchangerPrimaryGlycolRotary, AID_VC_ROT_LEB_L_05);
	SuitHeatExchangerPrimaryGlycolRotary.SetReference(LEB_L_ROT_POS[4], rot_leb_l_vector);
	SuitHeatExchangerPrimaryGlycolRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_05);

	MainPanelVC.AddSwitch(&SuitFlowReliefRotary, AID_VC_ROT_LEB_L_06);
	SuitFlowReliefRotary.SetReference(LEB_L_ROT_POS[5], rot_leb_l_vector);
	SuitFlowReliefRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_06);

	MainPanelVC.AddSwitch(&PrimaryGlycolEvapInletTempRotary, AID_VC_ROT_LEB_L_07);
	PrimaryGlycolEvapInletTempRotary.SetReference(LEB_L_ROT_POS[6], rot_leb_l_vector);
	PrimaryGlycolEvapInletTempRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_07);

	MainPanelVC.AddSwitch(&SuitHeatExchangerSecondaryGlycolRotary, AID_VC_ROT_LEB_L_08);
	SuitHeatExchangerSecondaryGlycolRotary.SetReference(LEB_L_ROT_POS[7], rot_leb_l_vector);
	SuitHeatExchangerSecondaryGlycolRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_08);

	MainPanelVC.AddSwitch(&EvapWaterControlSecondaryRotary, AID_VC_ROT_LEB_L_09);
	EvapWaterControlSecondaryRotary.SetReference(LEB_L_ROT_POS[8], rot_leb_l_vector);
	EvapWaterControlSecondaryRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_09);

	MainPanelVC.AddSwitch(&EvapWaterControlPrimaryRotary, AID_VC_ROT_LEB_L_10);
	EvapWaterControlPrimaryRotary.SetReference(LEB_L_ROT_POS[9], rot_leb_l_vector);
	EvapWaterControlPrimaryRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_10);

	MainPanelVC.AddSwitch(&WaterAccumulator1Rotary, AID_VC_ROT_LEB_L_11);
	WaterAccumulator1Rotary.SetReference(LEB_L_ROT_POS[10], rot_leb_l_vector);
	WaterAccumulator1Rotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_11);

	MainPanelVC.AddSwitch(&WaterAccumulator2Rotary, AID_VC_ROT_LEB_L_12);
	WaterAccumulator2Rotary.SetReference(LEB_L_ROT_POS[11], rot_leb_l_vector);
	WaterAccumulator2Rotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_12);

	MainPanelVC.AddSwitch(&SelectorInletValveRotary, AID_VC_ROT_LEB_L_13);
	SelectorInletValveRotary.SetReference(LEB_L_ROT_POS[12], rot_leb_l_vector);
	SelectorInletValveRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_13);

	MainPanelVC.AddSwitch(&SelectorOutletValveRotary, AID_VC_ROT_LEB_L_14);
	SelectorOutletValveRotary.SetReference(LEB_L_ROT_POS[13], rot_leb_l_vector);
	SelectorOutletValveRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_14);

	MainPanelVC.AddSwitch(&EmergencyCabinPressureRotary, AID_VC_ROT_LEB_L_15);
	EmergencyCabinPressureRotary.SetReference(LEB_L_ROT_POS[14], rot_leb_l_vector);
	EmergencyCabinPressureRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_15);

	MainPanelVC.AddSwitch(&CabinRepressValveRotary, AID_VC_ROT_LEB_L_16);
	CabinRepressValveRotary.SetReference(LEB_L_ROT_POS[15], rot_leb_l_vector);
	CabinRepressValveRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_16);

	MainPanelVC.AddSwitch(&WasteTankServicingRotary, AID_VC_ROT_LEB_L_17);
	WasteTankServicingRotary.SetReference(LEB_L_ROT_POS[16], rot_leb_l_vector);
	WasteTankServicingRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_17);

	MainPanelVC.AddSwitch(&PotableTankInletRotary, AID_VC_ROT_LEB_L_18);
	PotableTankInletRotary.SetReference(LEB_L_ROT_POS[17], rot_leb_l_vector);
	PotableTankInletRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_18);

	MainPanelVC.AddSwitch(&WasteTankInletRotary, AID_VC_ROT_LEB_L_19);
	WasteTankInletRotary.SetReference(LEB_L_ROT_POS[18], rot_leb_l_vector);
	WasteTankInletRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_19);

	MainPanelVC.AddSwitch(&PressureReliefRotary, AID_VC_ROT_LEB_L_20);
	PressureReliefRotary.SetReference(LEB_L_ROT_POS[19], rot_leb_l_vector);
	PressureReliefRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_20);

	MainPanelVC.AddSwitch(&O2DemandRegulatorRotary, AID_VC_ROT_LEB_L_21);
	O2DemandRegulatorRotary.SetReference(LEB_L_ROT_POS[20], rot_leb_l_vector);
	O2DemandRegulatorRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_21);

	MainPanelVC.AddSwitch(&SuitTestRotary, AID_VC_ROT_LEB_L_22);
	SuitTestRotary.SetReference(LEB_L_ROT_POS[21], rot_leb_l_vector);
	SuitTestRotary.DefineMeshGroup(VC_GRP_Rot_LEB_Left_22);

	const VECTOR3 pb_leb_l_vector = { -0.001, 0, 0 };
	const VECTOR3 handle_leb_l_vector = { -0.01, 0, 0 };

	MainPanelVC.AddSwitch(&SuitCircuitReturnValveLever, AID_VC_PUSHB_LEB_L_01);
	SuitCircuitReturnValveLever.SetDirection(handle_leb_l_vector);
	SuitCircuitReturnValveLever.DefineMeshGroup(VC_GRP_PB_LEB_Left_01);

	MainPanelVC.AddSwitch(&O2MainRegulatorASwitch, AID_VC_PUSHB_LEB_L_02);
	O2MainRegulatorASwitch.SetDirection(handle_leb_l_vector);
	O2MainRegulatorASwitch.DefineMeshGroup(VC_GRP_PB_LEB_Left_02);

	MainPanelVC.AddSwitch(&O2MainRegulatorBSwitch, AID_VC_PUSHB_LEB_L_03);
	O2MainRegulatorBSwitch.SetDirection(handle_leb_l_vector);
	O2MainRegulatorBSwitch.DefineMeshGroup(VC_GRP_PB_LEB_Left_03);

	MainPanelVC.AddSwitch(&EmergencyCabinPressureTestSwitch, AID_VC_PUSHB_LEB_L_04);
	EmergencyCabinPressureTestSwitch.SetDirection(pb_leb_l_vector);
	EmergencyCabinPressureTestSwitch.DefineMeshGroup(VC_GRP_PB_LEB_Left_04);

	// Side Hatch
	MainPanelVC.AddSwitch(&HatchToggle, AID_VC_SIDEHATCH_HANDLE);
	MainPanelVC.AddSwitch(&HatchGearBoxSelector, AID_VC_SIDEHATCH_GEARBOX_SEL);
	MainPanelVC.AddSwitch(&HatchActuatorHandleSelector, AID_VC_SIDEHATCH_ACT_HANDLE_SEL);
	MainPanelVC.AddSwitch(&HatchActuatorHandleSelectorOpen, AID_VC_SIDEHATCH_ACT_HANDLE_SEL_OPEN);
	MainPanelVC.AddSwitch(&HatchVentValveRotary, AID_VC_SIDEHATCH_VENT_VALVE);

	// Forward Hatch
	MainPanelVC.AddSwitch(&PressEqualValve, AID_VC_FWDHATCH_PRESS_EQU_VLV);

	// Panel 600
	const VECTOR3	P600_SW_AXIS = { -1, 0, 0 };

	MainPanelVC.AddSwitch(&HatchEmergencyO2ValveSwitch, AID_VC_SWITCH_P600_01);
	HatchEmergencyO2ValveSwitch.SetReference(Sw_P600_01Location, P600_SW_AXIS);
	HatchEmergencyO2ValveSwitch.SetRotationRange(RAD * 90);
	HatchEmergencyO2ValveSwitch.DefineMeshGroup(VC_GRP_Sw_P600_01);

	MainPanelVC.AddSwitch(&HatchRepressO2ValveSwitch, AID_VC_SWITCH_P600_02);
	HatchRepressO2ValveSwitch.SetReference(Sw_P600_02Location, P600_SW_AXIS);
	HatchRepressO2ValveSwitch.SetRotationRange(RAD * 90);
	HatchRepressO2ValveSwitch.DefineMeshGroup(VC_GRP_Sw_P600_02);

	NEEDLE_POS = { 0.2225, 1.3865, -0.5910 };
	const VECTOR3	P600_ROT_AXIS = { 0.0011765750919, 0.019116914303107, 0.999816562804688 };

	MainPanelVC.AddSwitch(&HatchOxygenRepressPressMeter);
	HatchOxygenRepressPressMeter.SetReference(NEEDLE_POS, P600_ROT_AXIS);
	HatchOxygenRepressPressMeter.SetRotationRange(RAD * 58);
	HatchOxygenRepressPressMeter.DefineMeshGroup(VC_GRP_Needle_P600_01);

	// THC Handle
	const VECTOR3 thc_handle_axis = { 0.0, 0.945196207714068, -0.326502877357833 };
	MainPanelVC.AddSwitch(&THCRotary, AID_VC_THC_HANDLE);
	THCRotary.SetReference(THChandleLocation, thc_handle_axis);
	THCRotary.DefineMeshGroup(VC_GRP_THChandle);

	// Panel 163
	MainPanelVC.AddSwitch(&SCIUtilPowerSwitch, AID_VC_SWITCH_P163_01);
	SCIUtilPowerSwitch.SetReference(Switch_P163Location, _V(1, 0, 0));
	SCIUtilPowerSwitch.DefineMeshGroup(VC_GRP_Sw_P163_01);

	// PLVC switch
	MainPanelVC.AddSwitch(&PLVCSwitch, AID_VC_SWITCH_PLVC);
	PLVCSwitch.SetReference(Switch_PLVCLocation, _V(0, -1, 0));
	PLVCSwitch.DefineMeshGroup(VC_GRP_Sw_PLVC);

	ems.DefineVCAnimations(vcidx);
	EMSDvSetSwitch.DefineVCAnimations(vcidx);

	SideHatch.DefineAnimationsVC(vcidx);
	ForwardHatch.DefineAnimationsVC(vcidx);

	Altimeter.DefineVCAnimations(vcidx);

	MainPanelVC.DefineVCAnimations(vcidx);

	InitFDAI(vcidx);
}

void Saturn::InitFDAI(UINT mesh)

{
	// 3D FDAI initialization

	// Constants
	const VECTOR3 fdairollaxis = { -0.00, sin(P1_3_TILT), -cos(P1_3_TILT) };
	const VECTOR3 fdaiyawvaxis = { -0.00, sin(P1_3_TILT + (90.0 * RAD)), -cos(P1_3_TILT + (90.0 * RAD)) };
	const VECTOR3 needlexvector = { 0.00, 0.05*cos(P1_3_TILT), 0.05*sin(P1_3_TILT) };
	const VECTOR3 needleyvector = { 0.05, 0, 0 };
	const VECTOR3 ratexvector = { 0.00, 0.064*cos(P1_3_TILT), 0.064*sin(P1_3_TILT) };
	const VECTOR3 rateyvector = { 0.064, 0, 0 };
	const VECTOR3 FDAI_PIVOT_L = { -0.673236, 0.563893, 0.385934 }; // L FDAI Pivot Point
	const VECTOR3 FDAI_PIVOT_R = { -0.340246, 0.750031, 0.44815 }; // R FDAI Pivot Point

	// L FDAI Ball
	ANIMATIONCOMPONENT_HANDLE	ach_FDAIroll_L, ach_FDAIpitch_L, ach_FDAIyaw_L;
	static UINT meshgroup_Fdai1_L = { VC_GRP_FDAIBall1_L }; // Roll gimbal meshgroup (includes roll indicator)
	static UINT meshgroup_Fdai2_L = { VC_GRP_FDAIBall_L };  // Pitch gimbal meshgroup (visible ball)
	static UINT meshgroup_Fdai3_L = { VC_GRP_FDAIBall2_L }; // Yaw gimbal meshgroup
	static MGROUP_ROTATE mgt_FDAIRoll_L(mesh, &meshgroup_Fdai1_L, 1, FDAI_PIVOT_L, fdairollaxis, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIPitch_L(mesh, &meshgroup_Fdai2_L, 1, FDAI_PIVOT_L, _V(-1, 0, 0), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIYaw_L(mesh, &meshgroup_Fdai3_L, 1, FDAI_PIVOT_L, fdaiyawvaxis, (float)(RAD * 360));
	anim_fdaiR_L = CreateAnimation(0.0);
	anim_fdaiP_L = CreateAnimation(0.0);
	anim_fdaiY_L = CreateAnimation(0.0);
	ach_FDAIroll_L = AddAnimationComponent(anim_fdaiR_L, 0.0f, 1.0f, &mgt_FDAIRoll_L);
	ach_FDAIyaw_L = AddAnimationComponent(anim_fdaiY_L, 0.0f, 1.0f, &mgt_FDAIYaw_L, ach_FDAIroll_L);
	ach_FDAIpitch_L = AddAnimationComponent(anim_fdaiP_L, 0.0f, 1.0f, &mgt_FDAIPitch_L, ach_FDAIyaw_L);

	// L FDAI error needles
	static UINT meshgroup_RollError_L = VC_GRP_FDAI_rollerror_L;
	static UINT meshgroup_PitchError_L = VC_GRP_FDAI_pitcherror_L;
	static UINT meshgroup_YawError_L = VC_GRP_FDAI_yawerror_L;
	static MGROUP_TRANSLATE mgt_rollerror_L(mesh, &meshgroup_RollError_L, 1, needleyvector);
	static MGROUP_TRANSLATE mgt_pitcherror_L(mesh, &meshgroup_PitchError_L, 1, needlexvector);
	static MGROUP_TRANSLATE mgt_yawerror_L(mesh, &meshgroup_YawError_L, 1, needleyvector);
	anim_fdaiRerror_L = CreateAnimation(0.5);
	anim_fdaiPerror_L = CreateAnimation(0.5);
	anim_fdaiYerror_L = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRerror_L, 0.0f, 1.0f, &mgt_rollerror_L);
	AddAnimationComponent(anim_fdaiPerror_L, 0.0f, 1.0f, &mgt_pitcherror_L);
	AddAnimationComponent(anim_fdaiYerror_L, 0.0f, 1.0f, &mgt_yawerror_L);

	// L FDAI rate needles
	static UINT meshgroup_RollRate_L = VC_GRP_FDAI_rateR_L;
	static UINT meshgroup_PitchRate_L = VC_GRP_FDAI_rateP_L;
	static UINT meshgroup_YawRate_L = VC_GRP_FDAI_rateY_L;
	static MGROUP_TRANSLATE mgt_rollrate_L(mesh, &meshgroup_RollRate_L, 1, rateyvector);
	static MGROUP_TRANSLATE mgt_pitchrate_L(mesh, &meshgroup_PitchRate_L, 1, ratexvector);
	static MGROUP_TRANSLATE mgt_yawrate_L(mesh, &meshgroup_YawRate_L, 1, rateyvector);
	anim_fdaiRrate_L = CreateAnimation(0.5);
	anim_fdaiPrate_L = CreateAnimation(0.5);
	anim_fdaiYrate_L = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRrate_L, 0.0f, 1.0f, &mgt_rollrate_L);
	AddAnimationComponent(anim_fdaiPrate_L, 0.0f, 1.0f, &mgt_pitchrate_L);
	AddAnimationComponent(anim_fdaiYrate_L, 0.0f, 1.0f, &mgt_yawrate_L);

	// R FDAI Ball
	ANIMATIONCOMPONENT_HANDLE	ach_FDAIroll_R, ach_FDAIpitch_R, ach_FDAIyaw_R;
	static UINT meshgroup_Fdai1_R = { VC_GRP_FDAIBall1_R }; // Roll gimbal meshgroup (includes roll incicator)
	static UINT meshgroup_Fdai2_R = { VC_GRP_FDAIBall_R };  // Pitch gimbal meshgroup (visible ball)
	static UINT meshgroup_Fdai3_R = { VC_GRP_FDAIBall2_R }; // Yaw gimbal meshgroup
	static MGROUP_ROTATE mgt_FDAIRoll_R(mesh, &meshgroup_Fdai1_R, 1, FDAI_PIVOT_R, fdairollaxis, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIPitch_R(mesh, &meshgroup_Fdai2_R, 1, FDAI_PIVOT_R, _V(-1, 0, 0), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIYaw_R(mesh, &meshgroup_Fdai3_R, 1, FDAI_PIVOT_R, fdaiyawvaxis, (float)(RAD * 360));
	anim_fdaiR_R = CreateAnimation(0.0);
	anim_fdaiP_R = CreateAnimation(0.0);
	anim_fdaiY_R = CreateAnimation(0.0);
	ach_FDAIroll_R = AddAnimationComponent(anim_fdaiR_R, 0.0f, 1.0f, &mgt_FDAIRoll_R);
	ach_FDAIyaw_R = AddAnimationComponent(anim_fdaiY_R, 0.0f, 1.0f, &mgt_FDAIYaw_R, ach_FDAIroll_R);
	ach_FDAIpitch_R = AddAnimationComponent(anim_fdaiP_R, 0.0f, 1.0f, &mgt_FDAIPitch_R, ach_FDAIyaw_R);

	// R FDAI error needles
	static UINT meshgroup_RollError_R = VC_GRP_FDAI_rollerror_R;
	static UINT meshgroup_PitchError_R = VC_GRP_FDAI_pitcherror_R;
	static UINT meshgroup_YawError_R = VC_GRP_FDAI_yawerror_R;
	static MGROUP_TRANSLATE mgt_rollerror_R(mesh, &meshgroup_RollError_R, 1, needleyvector);
	static MGROUP_TRANSLATE mgt_pitcherror_R(mesh, &meshgroup_PitchError_R, 1, needlexvector);
	static MGROUP_TRANSLATE mgt_yawerror_R(mesh, &meshgroup_YawError_R, 1, needleyvector);
	anim_fdaiRerror_R = CreateAnimation(0.5);
	anim_fdaiPerror_R = CreateAnimation(0.5);
	anim_fdaiYerror_R = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRerror_R, 0.0f, 1.0f, &mgt_rollerror_R);
	AddAnimationComponent(anim_fdaiPerror_R, 0.0f, 1.0f, &mgt_pitcherror_R);
	AddAnimationComponent(anim_fdaiYerror_R, 0.0f, 1.0f, &mgt_yawerror_R);

	// R FDAI rate needles
	static UINT meshgroup_RollRate_R = VC_GRP_FDAI_rateR_R;
	static UINT meshgroup_PitchRate_R = VC_GRP_FDAI_rateP_R;
	static UINT meshgroup_YawRate_R = VC_GRP_FDAI_rateY_R;
	static MGROUP_TRANSLATE mgt_rollrate_R(mesh, &meshgroup_RollRate_R, 1, rateyvector);
	static MGROUP_TRANSLATE mgt_pitchrate_R(mesh, &meshgroup_PitchRate_R, 1, ratexvector);
	static MGROUP_TRANSLATE mgt_yawrate_R(mesh, &meshgroup_YawRate_R, 1, rateyvector);
	anim_fdaiRrate_R = CreateAnimation(0.5);
	anim_fdaiPrate_R = CreateAnimation(0.5);
	anim_fdaiYrate_R = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRrate_R, 0.0f, 1.0f, &mgt_rollrate_R);
	AddAnimationComponent(anim_fdaiPrate_R, 0.0f, 1.0f, &mgt_pitchrate_R);
	AddAnimationComponent(anim_fdaiYrate_R, 0.0f, 1.0f, &mgt_yawrate_R);
}

#ifdef _OPENORBITER
void Saturn::SetCMVCIntegralLight(UINT meshidx, DWORD *matList, MatProp EmissionMode, double state, int cnt)
#else
void Saturn::SetCMVCIntegralLight(UINT meshidx, DWORD *matList, int EmissionMode, double state, int cnt)
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

void Saturn::MoveFlashlight()
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

void Saturn::SetFlashlightOn(bool state)
{
	flashlight->Activate(state);
	flashlightOn = state;
}

void Saturn::ToggleFlashlight()
{
	if ((oapiCockpitMode() == COCKPIT_VIRTUAL) && (oapiCameraMode() == CAM_COCKPIT)) {
		SetFlashlightOn(!flashlightOn);
	}
}
