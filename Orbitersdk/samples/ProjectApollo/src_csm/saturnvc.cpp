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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "soundlib.h"
#include "resource.h"

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

#include "nasspdefs.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "saturnv.h"
#include "tracer.h"
#include "papi.h"
#include "CM_VC_Resource.h"

// ==============================================================
// VC Constants
// ==============================================================

// Panel tilt
const double P1_3_TILT = 18.4 * RAD;

// Number of switches on each panel
const int	P1_SWITCHCOUNT = 49;
const int	P2_SWITCHCOUNT = 90;
const int	P3_SWITCHCOUNT = 64;

// Number of push buttons
const int   P1_PUSHBCOUNT = 11;
const int   P2_PUSHBCOUNT = 19;

// Number of switch covers
const int   P1_SWITCHCOVERCOUNT = 15;
const int   P2_SWITCHCOVERCOUNT = 12;

// Number of rotaries
const int	 P1_ROTCOUNT = 1;
const int	 P2_ROTCOUNT = 4;
const int	 P3_ROTCOUNT = 5;

// Number of thumbwheels
const int	 P1_TWCOUNT = 2;
const int	 P2_TWCOUNT = 1;
const int	 P3_TWCOUNT = 2;

// Number of needles
const int	 P1_NEEDLECOUNT = 5;

// Switch clickspot offset
const VECTOR3	P1_3_CLICK = { 0.00, 0.009*cos(P1_3_TILT - (90.0 * RAD)), 0.009*sin(P1_3_TILT - (90.0 * RAD)) };
const VECTOR3	P1_3_TWCLICK = { 0.00, 0.024*cos(P1_3_TILT - (90.0 * RAD)), 0.024*sin(P1_3_TILT - (90.0 * RAD)) };

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
	{-0.6834, 0.8195, 0.3925}
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
	{0.0297, 0.7256, 0.3614}, { 0.0297, 0.6477, 0.3354 }, { 0.3195, 0.4008, 0.2527 }, { 0.3974, 0.4010, 0.2527 }
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
	{0.5139, 0.8741, 0.4111}, {0.7741, 0.6223, 0.3268}, {0.8658, 0.4693, 0.2756}, {0.9596, 0.4691, 0.2755}, {0.9894, 0.2936, 0.2169}
};

// Panel 3 thumbwheels
const VECTOR3 P3_TW_POS[P3_TWCOUNT] = {
	{0.5832, 0.3306, 0.2629}, {0.5832, 0.2747, 0.2444}
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
	srf[SRF_VC_EMS_SCROLL_LEO] = oapiLoadTexture("ProjectApollo/VC/EMSscroll_LEO.dds");
	srf[SRF_VC_EMS_SCROLL_BORDER] = oapiLoadTexture("ProjectApollo/VC/EMSscroll_border.dds");
	srf[SRF_VC_EMS_SCROLL_BUG] = oapiLoadTexture("ProjectApollo/VC/EMSscroll_bug.dds");
	srf[SRF_VC_EMS_LIGHTS] = oapiLoadTexture("ProjectApollo/VC/ems_lights.dds");
	srf[SRF_VC_INDICATOR] = oapiLoadTexture("ProjectApollo/VC/Indicator.dds");
	srf[SRF_VC_ECSINDICATOR] = oapiLoadTexture("ProjectApollo/VC/ECSIndicator.dds");
	srf[SRF_VC_SEQUENCERSWITCHES] = oapiLoadTexture("ProjectApollo/VC/SequencerSwitches.dds");
	srf[SRF_VC_SPS_FONT_BLACK] = oapiLoadTexture("ProjectApollo/VC/fonts_black.dds");
	srf[SRF_VC_SPS_FONT_WHITE] = oapiLoadTexture("ProjectApollo/VC/fonts_white.dds");
	srf[SRF_VC_SPS_INJ_VLV] = oapiLoadTexture("ProjectApollo/VC/sps_injector_indicators.dds");
	srf[SRF_VC_SPSMAXINDICATOR] = oapiLoadTexture("ProjectApollo/VC/SPSmax_indicator.dds");
	srf[SRF_VC_SPSMININDICATOR] = oapiLoadTexture("ProjectApollo/VC/SPSmin_indicator.dds");
	srf[SRF_VC_THUMBWHEEL_LARGEFONTSINV] = oapiLoadTexture("ProjectApollo/VC/thumbwheel_large_fonts_inv.dds");

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
	oapiSetSurfaceColourKey(srf[SRF_VC_SPS_FONT_BLACK], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_SPS_FONT_WHITE], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_SPS_INJ_VLV], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_SPSMAXINDICATOR], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_SPSMININDICATOR], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_THUMBWHEEL_LARGEFONTSINV], ck);

	//
	// Register active areas for repainting here
	//
		
	SURFHANDLE MainPanelTex1 = oapiGetTextureHandle(hCMVC, 12);
	SURFHANDLE MainPanelTex2 = oapiGetTextureHandle(hCMVC, 1);

	// Panel 1

	oapiVCRegisterArea(AID_VC_MASTER_ALARM, _R(1489, 1183, 1534, 1219), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_LVENG_LIGHTS, _R(1868, 1421, 1969, 1565), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_EVENT_TIMER, _R(1891, 1356, 1962, 1374), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_EMS_SCROLL_LEO, _R(1756, 982, 1900, 1134), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SPS_LIGHT, _R(1840, 1142, 1881, 1158), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_PT05G_LIGHT, _R(1774, 1142, 1815, 1158), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_EMSDVDISPLAY, _R(1768, 1204, 1925, 1225), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_EMS_RSI_BKGRND, _R(1627, 1149, 1715, 1236), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SEQUENCERSWITCHES, _R(1847, 1606, 1886, 1644), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	oapiVCRegisterArea(AID_VC_ASCPDISPLAYROLL, _R(1224, 1830, 1254, 1842), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_ASCPDISPLAYPITCH, _R(1224, 1892, 1254, 1904), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_ASCPDISPLAYYAW, _R(1224, 1954, 1254, 1966), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);


	//oapiRegisterPanelArea(AID_ASCPDISPLAYROLL, _R(199, 1144, 229, 1156), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN, PANEL_MAP_BACKGROUND);
	//oapiRegisterPanelArea(AID_ASCPDISPLAYPITCH, _R(199, 1206, 229, 1218), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN, PANEL_MAP_BACKGROUND);
	//oapiRegisterPanelArea(AID_ASCPDISPLAYYAW, _R(199, 1268, 229, 1280), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN, PANEL_MAP_BACKGROUND);

	// Panel 2
	oapiVCRegisterArea(AID_VC_DSKY_DISPLAY, _R(254, 1235, 359, 1411), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_DSKY_LIGHTS, _R(110, 1240, 212, 1360), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MISSION_CLOCK, _R(849, 949, 992, 972), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_CWS_LIGHTS_LEFT, _R(555, 743, 767, 1851), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_CWS_LIGHTS_RIGHT, _R(810, 743, 1023, 851), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	oapiVCRegisterArea(AID_VC_RCS_HELIUM1_TB, _R(606, 1014, 756, 1037), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_HELIUM2_TB, _R(606, 1172, 756, 1195), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_PROP1_TB, _R(606, 1305, 756, 1328), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_PROP2_TB, _R(518, 1438, 754, 1461), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_ECSRADIATORIND, _R(814, 1329, 837, 1352), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_DOCKINGPROBEIND, _R(411, 825, 434, 875), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	// Panel 3
	oapiVCRegisterArea(AID_VC_MASTER_ALARM2, _R(433, 1339, 478, 1375), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex2);

	oapiVCRegisterArea(AID_VC_FCPHRADTEMPIND, _R(295, 1177, 492, 1200), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_FCRADIATORSIND, _R(295, 1226, 404, 1249), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(ADI_VC_MAINBUSAIND, _R(426, 1445, 535, 1468), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SPSHEVALVEIND, _R(71, 1583, 137, 1606), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_FCREACTANTSIND, _R(296, 1580, 534, 1604), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_TELECOMTB, _R(592, 1729, 615, 1802), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SPS_OXID_PCT, _R(137, 1315, 175, 1328), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SPS_FUEL_PCT, _R(137, 1344, 175, 1357), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SPSOXIDFLOWIND, _R(78, 1479, 100, 1521), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SPS_INJ_VLV, _R(69, 1210, 247, 1244), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

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

	InVC = true;
	InPanel = false;

	//if ((viewpos >= SATVIEW_ENG1) && (viewpos <= SATVIEW_ENG6))
	//	return true;

	// Test stuff
	//SURFHANDLE tex# = oapiGetTextureHandle (vcmeshidentifier, meshgroup#);
	//int i;
	//SetCameraDefaultDirection (_V(0,0,1));
	//default camera direction: forward
	//SetCameraShiftRange (_V(#,#,#), _V(#,#,#), _V(#,#,#));
	// leaning forward/left/right

	InitVC();
	RegisterActiveAreas();

	switch (id) {

	case SATVIEW_LEFTSEAT:
		viewpos = SATVIEW_LEFTSEAT;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);

		//SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(-1, SATVIEW_CENTERSEAT, -1, -1);
		SetView(true);


		return true;

	case SATVIEW_CENTERSEAT:
		viewpos = SATVIEW_CENTERSEAT;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);

		//SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(SATVIEW_LEFTSEAT, SATVIEW_RIGHTSEAT, -1, -1);
		SetView(true);


		return true;

	case SATVIEW_RIGHTSEAT:
		viewpos = SATVIEW_RIGHTSEAT;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);

		//SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(SATVIEW_CENTERSEAT, -1, -1, -1);
		SetView(true);


		return true;

	case SATVIEW_GNPANEL:
		viewpos = SATVIEW_GNPANEL;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.8 * PI, 0.4 * PI);
		SetCameraShiftRange(_V(-0.4, 0, 0), _V(0, 0, 0), _V(0, 0, 0));

		//SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		//oapiVCSetNeighbours(-1, LMVIEW_LMP, -1, LMVIEW_LPD);



		return true;

	case SATVIEW_LEFTDOCK:
		viewpos = SATVIEW_LEFTDOCK;
		SetCameraRotationRange(0, 0, 0, 0);

		//SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		//oapiVCSetNeighbours(-1, LMVIEW_LMP, -1, LMVIEW_LPD);



		return true;

	case SATVIEW_RIGHTDOCK: // Limited eye movement
		viewpos = SATVIEW_RIGHTDOCK;
		SetCameraRotationRange(0, 0, 0, 0);

		//SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		//oapiVCSetNeighbours(-1, LMVIEW_LMP, -1, LMVIEW_LPD);

		return true;

	default:
		return false;
	}
}

void Saturn::RegisterActiveAreas() {

	int i = 0;

	VECTOR3 ofs = _V(0.0, 0.0, 0.0);
	//if (vcidx != -1) GetMeshOffset(vcidx, ofs);

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

	//sprintf(oapiDebugString(), "OFS %lf", ofs.z);

	//
	// Register active areas for switches/animations here
	//

	// FDAI's
	oapiVCRegisterArea(AID_VC_FDAI_LEFT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FDAI_RIGHT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Panel 1
	for (i = 0; i < P1_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P1_01 + i, P1_TOGGLE_POS[i] + P1_3_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P1_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P1_01 + i, P1_ROT_POS[i] + ofs, 0.02);
	}

	for (i = 0; i < P1_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P1_01 + i, P1_PUSHB_POS[i] + ofs, 0.008);
	}

	for (i = 0; i < P1_TWCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_TW_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P1_01 + i, P1_TW_POS[i] + P1_3_TWCLICK + ofs, 0.01);
	}

	oapiVCRegisterArea(AID_VC_EMS_DVSET, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_PRESSED | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_EMS_DVSET, _V(-0.507344, 0.732746, 0.370513) + ofs, 0.025);

	oapiVCSetAreaClickmode_Spherical(AID_VC_MASTER_ALARM, _V(-0.775435, 0.709185, 0.361746) + ofs, 0.008);

	oapiVCRegisterArea(AID_VC_ASCPROLL, PANEL_REDRAW_NEVER, PANEL_MOUSE_PRESSED | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ASCPROLL, _V(-0.993077, 0.360405, 0.251007) + ofs, _V(-0.982356, 0.360405, 0.251007) + ofs, _V(-0.993077, 0.339763, 0.244102) + ofs, _V(-0.982356, 0.339763, 0.244102) + ofs);

	oapiVCRegisterArea(AID_VC_ASCPPITCH, PANEL_REDRAW_NEVER, PANEL_MOUSE_PRESSED | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ASCPPITCH, _V(-0.993077, 0.325064, 0.239184) + ofs, _V(-0.982356, 0.325064, 0.239184) + ofs, _V(-0.993077, 0.303916, 0.232109) + ofs, _V(-0.982356, 0.303916, 0.232109) + ofs);

	oapiVCRegisterArea(AID_VC_ASCPYAW, PANEL_REDRAW_NEVER, PANEL_MOUSE_PRESSED | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_ASCPYAW, _V(-0.993077, 0.289921, 0.227431) + ofs, _V(-0.982356, 0.289921, 0.227431) + ofs, _V(-0.993077, 0.268953, 0.220424) + ofs, _V(-0.982356, 0.268953, 0.220424) + ofs);

	oapiVCRegisterArea(AID_VC_POSTLDGVENTHANDLE, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_POSTLDGVENTHANDLE, _V(0.183459, 0.922476, 0.408289) + ofs, 0.02);

	//Panel 2

	for (i = 0; i < P2_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P2_01 + i, P2_TOGGLE_POS[i] + P1_3_CLICK + ofs, 0.008);
	}

	for (i = 0; i < P2_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P2_01 + i, P2_ROT_POS[i] + ofs, 0.02);
	}

	for (i = 0; i < P2_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P2_01 + i, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P2_01 + i, P2_PUSHB_POS[i] + ofs, 0.008);
	}

	for (i = 0; i < P2_TWCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_TW_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P2_01 + i, P2_TW_POS[i] + P1_3_TWCLICK + ofs, 0.01);
	}

	// Panel 3

	for (i = 0; i < P3_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P3_01 + i, P3_TOGGLE_POS[i] + P1_3_CLICK + ofs, 0.008);
	}

	for (i = 0; i < P3_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P3_01 + i, P3_ROT_POS[i] + ofs, 0.02);
	}

	for (i = 0; i < P3_TWCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_TW_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P3_01 + i, P3_TW_POS[i] + P1_3_TWCLICK + ofs, 0.01);
	}

	oapiVCSetAreaClickmode_Spherical(AID_VC_MASTER_ALARM2, _V(0.720346, 0.621423, 0.332349) + ofs, 0.008);

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
	//case AID_VC_MASTER_ALARM3:

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

	switch (id) {
	//case areaidentifier
	//	Redraw Panel stuff
	//	return true if dynamic texture modified, false if not

	case AID_VC_FDAI_LEFT:
	{
		VECTOR3 euler_rates;
		VECTOR3 attitude;
		VECTOR3 errors;

		euler_rates = eda.GetFDAI1AttitudeRate();
		attitude = eda.GetFDAI1Attitude();
		errors = eda.GetFDAI1AttitudeError();

		AnimateFDAI(attitude, euler_rates, errors, anim_fdaiR_L, anim_fdaiP_L, anim_fdaiY_L, anim_fdaiRerror_L, anim_fdaiPerror_L, anim_fdaiYerror_L, anim_fdaiRrate_L, anim_fdaiPrate_L, anim_fdaiYrate_L);

		return true;
	}

	case AID_VC_FDAI_RIGHT:
	{
		VECTOR3 euler_rates;
		VECTOR3 attitude;
		VECTOR3 errors;

		euler_rates = eda.GetFDAI2AttitudeRate();
		attitude = eda.GetFDAI2Attitude();
		errors = eda.GetFDAI2AttitudeError();

		AnimateFDAI(attitude, euler_rates, errors, anim_fdaiR_R, anim_fdaiP_R, anim_fdaiY_R, anim_fdaiRerror_R, anim_fdaiPerror_R, anim_fdaiYerror_R, anim_fdaiRrate_R, anim_fdaiPrate_R, anim_fdaiYrate_R);

		return true;
	}

	case AID_VC_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[SRF_VC_DIGITALDISP], srf[SRF_VC_DSKYDISP]);
		return true;

	case AID_VC_DSKY_LIGHTS:
		dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS]);
		return true;

	/*case AID_ABORT_BUTTON:
		if (ABORT_IND) {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 62, 0, 62, 31);
		}
		else {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 0, 0, 62, 31);
		}
		return true;*/
		
	case AID_VC_MASTER_ALARM:
		cws.RenderMasterAlarm(surf, srf[SRF_VC_MASTERALARM], NULL, CWS_MASTERALARMPOSITION_LEFT);
		return true;

	case AID_VC_MASTER_ALARM2:
		cws.RenderMasterAlarm(surf, srf[SRF_VC_MASTERALARM], NULL, CWS_MASTERALARMPOSITION_RIGHT);
		return true;

	case AID_VC_LVENG_LIGHTS:
	{
		if (SI_EngineNum > 5)
		{
			/*RenderS1bEngineLight(ENGIND[0], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 64, 42);
			RenderS1bEngineLight(ENGIND[1], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 64, 98);
			RenderS1bEngineLight(ENGIND[2], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 8, 98);
			RenderS1bEngineLight(ENGIND[3], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 7, 43);
			RenderS1bEngineLight(ENGIND[4], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 36, 41);
			RenderS1bEngineLight(ENGIND[5], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 51, 69);
			RenderS1bEngineLight(ENGIND[6], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 36, 98);
			RenderS1bEngineLight(ENGIND[7], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 22, 69);*/
		}
		else
		{
			if (ENGIND[0])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 55, 44, 55, 44, 27, 27);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 55, 44, 157, 44, 27, 27);
			}

			if (ENGIND[1])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 55, 98, 55, 98, 27, 27);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 55, 98, 157, 98, 27, 27);
			}
			if (ENGIND[2])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 20, 98, 20, 98, 27, 27);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 20, 98, 122, 98, 27, 27);
			}
			if (ENGIND[3])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 20, 44, 20, 44, 27, 27);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 20, 44, 122, 44, 27, 27);
			}
			if (ENGIND[4])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 37, 71, 37, 71, 27, 27);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 37, 71, 140, 71, 27, 27);
			}
		}
	}

	if (LVRateLight)
	{
		oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 6, 4, 6, 4, 27, 27);
	}
	else
	{
		oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 6, 4, 108, 4, 27, 27);
	}

	//
	// Saturn 1b doesn't have an SII sep light.
	//

	if (SaturnType == SAT_SATURNV)
	{
		if (SIISepState)
		{
			oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 37, 4, 37, 4, 27, 27);
		}
		else
		{
			oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 37, 4, 139, 4, 27, 27);
		}
	}

	if (LVGuidLight)
	{
		oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 69, 4, 69, 4, 27, 27);
	}
	else
	{
		oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 69, 4, 171, 4, 27, 27);
	}
	return true;

	case AID_VC_MISSION_CLOCK:
		MissionTimerDisplay.Render(surf, srf[SRF_VC_DIGITALDISP2], true);
		return true;

	/*case AID_MISSION_CLOCK306:
		MissionTimer306Display.Render90(surf, srf[SRF_DIGITAL90], true);
		return true;*/

	case AID_VC_EVENT_TIMER:
		EventTimerDisplay.Render(surf, srf[SRF_VC_EVENT_TIMER_DIGITS]);
		return true;

	case AID_VC_CWS_LIGHTS_LEFT:
		cws.RenderLights(surf, srf[SRF_VC_CW_LIGHTS], true);
		return true;

	case AID_VC_CWS_LIGHTS_RIGHT:
		cws.RenderLights(surf, srf[SRF_VC_CW_LIGHTS], false);
		return true;

	case AID_VC_SEQUENCERSWITCHES:
		LiftoffNoAutoAbortSwitch.RepaintSwitchVC(surf, srf[SRF_VC_SEQUENCERSWITCHES]);
		return true;

	case AID_VC_SPS_LIGHT:
		if (ems.SPSThrustLight()) {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 41, 16, 41, 16);
		}
		else {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 0, 16, 41, 16);
		}
		return true;

	case AID_VC_PT05G_LIGHT:
		if (ems.pt05GLight()) {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 41, 0, 41, 16);
		}
		else {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 0, 0, 41, 16);
		}
		return true;


	case AID_VC_EMSDVDISPLAY:
		EMSDvDisplay.DoDrawSwitchVC(surf, EMSDvDisplay.QueryValue(), srf[SRF_VC_DIGITALDISP]);
		return true;

	case AID_VC_EMS_SCROLL_LEO:
	{

		HDC hDC;

		hDC = oapiGetDC(srf[SRF_VC_EMS_SCROLL_LEO]);

		SetBkMode(hDC, TRANSPARENT);
		HGDIOBJ oldObj = SelectObject(hDC, g_Param.pen[2]);

		Polyline(hDC, ems.ScribePntArray, ems.ScribePntCnt);

		SelectObject(hDC, oldObj);
		oapiReleaseDC(srf[SRF_VC_EMS_SCROLL_LEO], hDC);

		oapiBlt(surf, srf[SRF_VC_EMS_SCROLL_LEO], 5, 4, ems.GetScrollOffset(), 0, 132, 143);
		oapiBlt(surf, srf[SRF_VC_EMS_SCROLL_BUG], 42, ems.GetGScribe() + 2, 0, 0, 5, 5, SURF_PREDEF_CK);
		oapiBlt(surf, srf[SRF_VC_EMS_SCROLL_BORDER], 0, 0, 0, 0, 142, 150, SURF_PREDEF_CK);
		return true;
	}

	case AID_VC_EMS_RSI_BKGRND:
	{
		switch (ems.LiftVectLight()) {
		case 1:
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 33, 8, 82, 6, 20, 6);
			break;
		case -1:
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 32, 69, 82, 22, 22, 10);
			break;
		case 0:
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 33, 8, 82, 0, 20, 6);
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 32, 69, 82, 12, 22, 10);
			break;
		}

		ems.DrawSwitchVC(id, event, surf);
		return true;
	}

	case AID_VC_EMS_DVSET:
		EMSDvSetSwitch.DrawSwitchVC(id, event, surf);
		return true;

		// ASCP
	case AID_VC_ASCPDISPLAYROLL:
		ascp.PaintRollDisplay(surf, srf[SRF_VC_THUMBWHEEL_LARGEFONTSINV]);
		return true;

	case AID_VC_ASCPDISPLAYPITCH:
		ascp.PaintPitchDisplay(surf, srf[SRF_VC_THUMBWHEEL_LARGEFONTSINV]);
		return true;

	case AID_VC_ASCPDISPLAYYAW:
		ascp.PaintYawDisplay(surf, srf[SRF_VC_THUMBWHEEL_LARGEFONTSINV]);
		return true;

	/*case AID_MASTER_ALARM3:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], CWS_MASTERALARMPOSITION_NONE);
		return true;*/

	/*default:
		return false;*/
	}

	return MainPanelVC.VCRedrawEvent(id, event, surf);
	//return false;
}


void Saturn::JostleViewpoint(double amount)

{
	double j = ((double) ((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetx += j;

	j = ((double) ((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsety += j;

	j = ((double) ((rand() & 65535) - 32768) * amount) / 3276800.0;
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

	//
	// Engineering cameras
	//
	/*if (viewpos >= SATVIEW_ENG1)
	{
		VECTOR3 e1 = _V(0, 0, 0), e2 = _V(0, 0, 0), e3 = _V(0, 0, 0), e4 = _V(0, 0, 0), e5 = _V(0, 0, 0), e6 = _V(0, 0, 0);	
		VECTOR3 v1 = _V(0, 0, 0), v2 = _V(0, 0, 0), v3 = _V(0, 0, 0), v4 = _V(0, 0, 0), v5 = _V(0, 0, 0), v6 = _V(0, 0, 0);
		VECTOR3 cd;

		//
		// We really need different cameras for Saturn V and 1b.
		//

		switch (stage) {
		case PRELAUNCH_STAGE:
			e3 = _V(0.0, 7.5, -10.0+STG0O);
			v3 = _V(0.0, -0.1, -1.0);
			e4 = _V(7.5, 0.0, -10.0+STG0O);
			v4 = _V(-0.1, 0.0, -1.0);
			e5 = _V(0.0, -7.5, -10.0+STG0O);
			v5 = _V(0.0, 0.1, -1.0);
			e6 = _V(-7.5, 0.0, -10.0+STG0O);
			v6 = _V(0.1, 0.0, -1.0);
			break;

		case LAUNCH_STAGE_ONE:
			e1 = _V(4.0, 0.0, -39.0+STG0O);
			v1 = _V(-0.15, 0, 1.0);
			e2 = _V(3.5, 0.0, -31.0+STG0O);
			v2 = _V(-0.15, 0, -1.0);
			e3 = _V(0.0, 7.5, -10.0+STG0O);
			v3 = _V(0.0, -0.1, -1.0);
			e4 = _V(7.5, 0.0, -10.0+STG0O);
			v4 = _V(-0.1, 0.0, -1.0);
			e5 = _V(0.0, -7.5, -10.0+STG0O);
			v5 = _V(0.0, 0.1, -1.0);
			e6 = _V(-7.5, 0.0, -10.0+STG0O);
			v6 = _V(0.1, 0.0, -1.0);
			break;

		case LAUNCH_STAGE_TWO:
		case LAUNCH_STAGE_TWO_ISTG_JET:
			e2 = _V(3.5, 0.0, -31.0-STG1O);
			v2 = _V(-0.15, 0, -1.0);
			e3 = _V(0.0, 7.5, -10.0-STG1O);
			v3 = _V(0.0, -0.1, -1.0);
			e4 = _V(7.5, 0.0, -10.0-STG1O);
			v4 = _V(-0.1, 0.0, -1.0);
			e5 = _V(0.0, -7.5, -10.0-STG1O);
			v5 = _V(0.0, 0.1, -1.0);
			e6 = _V(-7.5, 0.0, -10.0-STG1O);
			v6 = _V(0.1, 0.0, -1.0);
			break;

		//
		// Switch back to commander view if we're past the point where we can
		// display anything useful.
		//

		case LAUNCH_STAGE_SIVB:
			viewpos = SATVIEW_LEFTSEAT;
			SetView(offset, true);
			return;
		}

		switch (viewpos) {
		case SATVIEW_ENG1:
			v = e1;
			cd = v1;
			break;

		case SATVIEW_ENG2:
			v = e2;
			cd = v2;
			break;

		case SATVIEW_ENG3:
			v = e3;
			cd = v3;
			break;

		case SATVIEW_ENG4:
			v = e4;
			cd = v4;
			break;

		case SATVIEW_ENG5:
			v = e5;
			cd = v5;
			break;

		case SATVIEW_ENG6:
			v = e6;
			cd = v6;
			break;
		}

		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
		SetCameraDefaultDirection(cd);
		oapiCameraSetCockpitDir(0,0);
	}

	// 
	// 2D panel 
	// Direction/rotation range is in clbkLoadPanel
	//
	else*/ if (InPanel) {
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
				v = _V(-0.6, 0.9, offset);
				break;

			case SATVIEW_CENTERSEAT:
				v = _V(0, 0.9, offset);
				break;

			case SATVIEW_RIGHTSEAT:
				v = _V(0.6, 0.9, offset);
				break;

			case SATVIEW_LEFTDOCK:
				v = _V(-0.65, 1.05, 0.25 + offset);
				break;
			
			case SATVIEW_RIGHTDOCK:
				v = _V(0.65, 1.05, 0.25 + offset);
				break;

			case SATVIEW_GNPANEL:
				v = _V(0.0, -0.15, 0.5 + offset);
				break;
		}

		if (update_direction) {
			SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
			if (viewpos == SATVIEW_GNPANEL) {
				SetCameraDefaultDirection(_V(0.0,-1.0, 0.0));
			} else {
				SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			}
			oapiCameraSetCockpitDir(0,0);
		}

		v.x += ViewOffsetx;
		v.y += ViewOffsety;
		v.z += ViewOffsetz;
	}
	SetCameraOffset(v);

	VCCameraOffset.x = v.x - VCMeshOffset.x;
	VCCameraOffset.y = v.y - VCMeshOffset.y;
	VCCameraOffset.z = v.z - VCMeshOffset.z;

	//
	// FOV handling
	//

	if (!FovExternal && !GenericFirstTimestep) {
		if (InPanel && PanelId == SATPANEL_SEXTANT) { // Sextant
			if (!FovFixed) {
				FovSave = oapiCameraAperture();
				FovFixed = true;
			}
			oapiCameraSetAperture(1 * RAD);

		}
		else if (InPanel && PanelId == SATPANEL_TELESCOPE) { // Telescope
			if (!FovFixed) {
				FovSave = oapiCameraAperture();
				FovFixed = true;
			}
			oapiCameraSetAperture(30 * RAD);
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
	SPSGimbalPitchThumbwheel.SetRotationRange(RAD * 293);
	SPSGimbalPitchThumbwheel.DefineMeshGroup(VC_GRP_TW_P1_01);
	SPSGimbalPitchThumbwheel.SetInitialAnimState(0.5);

	const VECTOR3	TW_SPSYAW_AXIS = { -0.00, sin(P1_3_TILT + (90.0 * RAD)), -cos(P1_3_TILT + (90.0 * RAD)) };
	MainPanelVC.AddSwitch(&SPSGimbalYawThumbwheel, AID_VC_TW_P1_02);
	SPSGimbalYawThumbwheel.SetReference(P1_TW_POS[1], TW_SPSYAW_AXIS);
	SPSGimbalYawThumbwheel.SetRotationRange(RAD * 293);
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
	MainPanelVC.AddSwitch(&DskySwitchNoun, AID_VC_PUSHB_P2_02);
	MainPanelVC.AddSwitch(&DskySwitchPlus, AID_VC_PUSHB_P2_03);
	MainPanelVC.AddSwitch(&DskySwitchMinus, AID_VC_PUSHB_P2_04);
	MainPanelVC.AddSwitch(&DskySwitchZero, AID_VC_PUSHB_P2_05);
	MainPanelVC.AddSwitch(&DskySwitchOne, AID_VC_PUSHB_P2_06);
	MainPanelVC.AddSwitch(&DskySwitchTwo, AID_VC_PUSHB_P2_07);
	MainPanelVC.AddSwitch(&DskySwitchThree, AID_VC_PUSHB_P2_08);
	MainPanelVC.AddSwitch(&DskySwitchFour, AID_VC_PUSHB_P2_09);
	MainPanelVC.AddSwitch(&DskySwitchFive, AID_VC_PUSHB_P2_10);
	MainPanelVC.AddSwitch(&DskySwitchSix, AID_VC_PUSHB_P2_11);
	MainPanelVC.AddSwitch(&DskySwitchSeven, AID_VC_PUSHB_P2_12);
	MainPanelVC.AddSwitch(&DskySwitchEight, AID_VC_PUSHB_P2_13);
	MainPanelVC.AddSwitch(&DskySwitchNine, AID_VC_PUSHB_P2_14);
	MainPanelVC.AddSwitch(&DskySwitchClear, AID_VC_PUSHB_P2_15);
	MainPanelVC.AddSwitch(&DskySwitchProg, AID_VC_PUSHB_P2_16);
	MainPanelVC.AddSwitch(&DskySwitchKeyRel, AID_VC_PUSHB_P2_17);
	MainPanelVC.AddSwitch(&DskySwitchEnter, AID_VC_PUSHB_P2_18);
	MainPanelVC.AddSwitch(&DskySwitchReset, AID_VC_PUSHB_P2_19);

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

	MainPanelVC.AddSwitch(&HighGainAntennaYawPositionSwitch, AID_VC_ROT_P2_04);
	HighGainAntennaYawPositionSwitch.SetReference(P2_ROT_POS[3], P1_3_ROT_AXIS);
	HighGainAntennaYawPositionSwitch.DefineMeshGroup(VC_GRP_Rot_P2_04);

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
	SuitPressMeter.SetRotationRange(RAD * 39.3);
	SuitPressMeter.DefineMeshGroup(VC_GRP_Needle_P2_15);

	MainPanelVC.AddSwitch(&CabinPressMeter);
	CabinPressMeter.SetReference(NEEDLE_POS);
	CabinPressMeter.SetRotationRange(RAD * 39.3);
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
	DCAmpMeter.SetRotationRange(RAD * 225);
	DCAmpMeter.DefineMeshGroup(VC_GRP_Needle_P3_09);

	NEEDLE_POS = { 0.8533, 0.5413, 0.3066 };

	MainPanelVC.AddSwitch(&CSMDCVoltMeter);
	CSMDCVoltMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
	CSMDCVoltMeter.DefineMeshGroup(VC_GRP_Needle_P3_10);

	NEEDLE_POS = { 0.9799, 0.3656, 0.2478 };

	MainPanelVC.AddSwitch(&CSMACVoltMeter);
	CSMACVoltMeter.SetReference(NEEDLE_POS, P1_3_ROT_AXIS);
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


    MainPanelVC.DefineVCAnimations(vcidx);

	ems.DefineVCAnimations(vcidx);
	EMSDvSetSwitch.DefineVCAnimations(vcidx);

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
	const VECTOR3 ratexvector = { 0.00, 0.062*cos(P1_3_TILT), 0.062*sin(P1_3_TILT) };
	const VECTOR3 rateyvector = { 0.062, 0, 0 };
	const VECTOR3 FDAI_PIVOT_L = { -0.673236, 0.563893, 0.385934 }; // L FDAI Pivot Point
	const VECTOR3 FDAI_PIVOT_R = { -0.340246, 0.750031, 0.44815 }; // R FDAI Pivot Point

	// L FDAI Ball
	ANIMATIONCOMPONENT_HANDLE	ach_FDAIroll_L, ach_FDAIpitch_L, ach_FDAIyaw_L;
	static UINT meshgroup_Fdai1_L = { VC_GRP_FDAIBall1_L }; // Roll gimbal meshgroup (includes roll incicator)
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

void Saturn::AnimateFDAI(VECTOR3 attitude, VECTOR3 rates, VECTOR3 errors, UINT animR, UINT animP, UINT animY, UINT errorR, UINT errorP, UINT errorY, UINT rateR, UINT rateP, UINT rateY) {

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


