/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: generic Saturn base class
  Saturn mesh code

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
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "CSMcomputer.h"
#include "ioChannels.h"

#include "saturn.h"
#include "tracer.h"
#include "sivb.h"

#include "LES.h"
#include "Mission.h"

MESHHANDLE hSM;
MESHHANDLE hSMRCS;
MESHHANDLE hSMRCSLow;
MESHHANDLE hSMSPS;
MESHHANDLE hSMPanel1;
MESHHANDLE hSMPanel2;
MESHHANDLE hSMPanel3;
MESHHANDLE hSMPanel4;
MESHHANDLE hSMPanel5;
MESHHANDLE hSMPanel6;
MESHHANDLE hSMhga;
MESHHANDLE hSMCRYO;
MESHHANDLE hSMSIMBAY;
MESHHANDLE hCM;
MESHHANDLE hCMnh;
MESHHANDLE hCM2;
MESHHANDLE hCMP;
MESHHANDLE hCMInt;
MESHHANDLE hCMVC;
MESHHANDLE hCREW;
MESHHANDLE hFHO;
MESHHANDLE hFHC;
MESHHANDLE hFHF;
MESHHANDLE hCM2B;
MESHHANDLE hdockring;
MESHHANDLE hprobe;
MESHHANDLE hprobeext;
//MESHHANDLE hCMBALLOON;
MESHHANDLE hCRB;
MESHHANDLE hApollochute;
MESHHANDLE hCMB;
MESHHANDLE hChute30;
MESHHANDLE hChute31;
MESHHANDLE hChute32;
MESHHANDLE hFHC2;
MESHHANDLE hsat5tower;
MESHHANDLE hFHO2;
MESHHANDLE hCMPEVA;
MESHHANDLE hopticscover;
MESHHANDLE hcmdocktgt;
MESHHANDLE hcmseatsfolded;
MESHHANDLE hcmseatsunfolded;
MESHHANDLE hcmWindowShades[5];
MESHHANDLE hcmCOAScdr;
MESHHANDLE hcmCOAScdrreticle;

#define LOAD_MESH(var, name) var = oapiLoadMeshGlobal(name);

// "O2 venting" particle streams
PARTICLESTREAMSPEC o2_venting_spec = {
	0,		// flag
	0.3,	// size
	30,		// rate
	2,	    // velocity
	0.5,    // velocity distribution
	2,		// lifetime
	0.2,	// growthrate
	0.5,    // atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

static PARTICLESTREAMSPEC lem_exhaust = {
	0,		// flag
	0.5,	// size
	100.0, 	// rate
	25.0,	// velocity
	0.1,	// velocity distribution
	0.3, 	// lifetime
	3.0,	// growthrate
	0.0,	// atmslowdown
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC dyemarker_spec = {
	0,		// flag
	0.15,	// size
	15,	    // rate
	1,	    // velocity
	0.3,    // velocity distribution
	3,		// lifetime
	0.2,	// growthrate
	0.2,    // atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC wastewaterdump_spec = {
	0,		// flag
	0.005,	// size
	1000,	// rate
	1.5,    // velocity
	0.2,    // velocity distribution
	100,	// lifetime
	0.001,	// growthrate
	0,      // atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC urinedump_spec = {
	0,		// flag
	0.005,	// size
	1000,	// rate
	1.5,    // velocity
	0.2,    // velocity distribution
	100,	// lifetime
	0.001,	// growthrate
	0,		// atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};


double BilinearInterpolation(double q11, double q12, double q21, double q22, double x1, double x2, double y1, double y2, double x, double y)
{
	double x2x1, y2y1, x2x, y2y, yy1, xx1;
	x2x1 = x2 - x1;
	y2y1 = y2 - y1;
	x2x = x2 - x;
	y2y = y2 - y;
	yy1 = y - y1;
	xx1 = x - x1;
	return 1.0 / (x2x1 * y2y1) * (q11 * x2x * y2y + q21 * xx1 * y2y + q12 * x2x * yy1 + q22 * xx1 * yy1);
}

void CMContinuumFlowAero(double alpha, double Mach, double &C_M, double &C_A, double &C_NY)
{

	//Parameters from the "CSM Data Book" (110 to 180 deg) and the "Mission Evaluation 103 (Mission D) Presimulation Report Part II Simulator Description" from North American 

	static const int Mach_arr_size = 16;
	static const int Alpha_arr_size = 26;

	static const double C_N_arr[Mach_arr_size][Alpha_arr_size] = { {0,0.148,0.267,0.368,0.448,0.438,0.179,0.058,0.221,0.266,0.252,0.1926,0.1694,
	0.1291,0.0759,0.0167,-0.0394,-0.0865,-0.1317,-0.1718,-0.1969,-0.194,-0.18,-0.112,-0.04,-0.002},
	{0,0.148,0.267,0.368,0.448,0.438,0.179,0.058,0.221,0.266,0.252,0.1926,0.1694,
	0.1291,0.0759,0.0167,-0.0394,-0.0865,-0.1317,-0.1718,-0.1969,-0.194,-0.18,-0.112,-0.04,-0.002},
	{0,0.148,0.267,0.368,0.448,0.438,0.179,0.058,0.221,0.266,0.252,0.2099,0.1796,
	0.1491,0.1088,0.0686,0.0284,-0.0118,-0.0569,-0.092,-0.1021,-0.0921,-0.0621,-0.0321,-0.0171,-0.0021},
	{0,0.16,0.296,0.409,0.5,0.58,0.55,0.323,0.296,0.315,0.318,0.2699,0.2345,
	0.1941,0.1437,0.0984,0.0531,0.028,0.0178,0.0227,0.0327,0.0276,0.0076,-0.0224,-0.0274,-0.0023},
	{0,0.14,0.289,0.439,0.575,0.668,0.624,0.466,0.467,0.485,0.469,0.4177,0.3322,
	0.2889,0.2185,0.1632,0.1119,0.0848,0.0756,0.0675,0.0524,0.0354,0.0173,-0.0027,-0.0126,-0.0026},
	{0,0.125,0.272,0.417,0.565,0.702,0.74,0.668,0.66,0.655,0.613,0.5453,0.501,
	0.4536,0.3931,0.3187,0.2304,0.1462,0.0871,0.065,0.049,0.0349,0.0219,0.0019,-0.0131,-0.0031},
	{0,0.117,0.24,0.395,0.58,0.705,0.762,0.705,0.696,0.67,0.62,0.5543,0.5119,
	0.4615,0.4021,0.3207,0.2334,0.1542,0.0871,0.0621,0.052,0.037,0.0219,0.0069,-0.0011,-0.0032},
	{0,0.127,0.261,0.408,0.579,0.701,0.76,0.72,0.705,0.685,0.64,0.5742,0.5339,
	0.4815,0.4231,0.3577,0.2824,0.2071,0.1319,0.0717,0.0517,0.0367,0.0216,0.0096,0.0066,-0.0034},
	{0,0.1646,0.326,0.467,0.591,0.688,0.712,0.694,0.671,0.651,0.612,0.5383,0.4989,
	0.4485,0.3862,0.3278,0.2645,0.2042,0.144,0.1018,0.0667,0.0416,0.0246,0.0126,0.0026,-0.0035},
	{0,0.211,0.375,0.515,0.622,0.69,0.725,0.725,0.695,0.65,0.598,0.5093,0.4639,
	0.4136,0.3583,0.306,0.2536,0.2024,0.1591,0.1218,0.0847,0.0466,0.0185,0.0065,-0.0015,-0.0035},
	{0,0.232,0.416,0.545,0.63,0.68,0.71,0.7,0.675,0.63,0.57,0.4743,0.424,
	0.3717,0.3283,0.283,0.2427,0.1974,0.1592,0.122,0.0868,0.0587,0.0366,0.0185,0.0065,-0.0035},
	{0,0.238,0.43,0.553,0.625,0.672,0.688,0.68,0.645,0.595,0.53,0.4394,0.3841,
	0.3388,0.2984,0.2581,0.2228,0.1875,0.1573,0.1221,0.0949,0.0697,0.0476,0.0315,0.0145,-0.0035},
	{0,0.235,0.439,0.553,0.613,0.636,0.647,0.632,0.592,0.541,0.475,0.3935,0.3482,
	0.3068,0.2685,0.2322,0.2039,0.1727,0.1464,0.1202,0.093,0.0693,0.0479,0.0291,0.0116,-0.0035},
	{0,0.244,0.463,0.576,0.638,0.65,0.652,0.638,0.597,0.543,0.459,0.3686,0.3182,
	0.2709,0.2225,0.1983,0.1819,0.1687,0.1424,0.1202,0.096,0.0748,0.0526,0.0305,0.0115,-0.0035},
	{0,0.244,0.463,0.576,0.638,0.65,0.652,0.638,0.597,0.543,0.459,0.3686,0.3182,
	0.2709,0.2225,0.1983,0.1819,0.1687,0.1424,0.1202,0.096,0.0748,0.0526,0.0305,0.0115,-0.0035},
	{0,0.244,0.463,0.576,0.638,0.65,0.652,0.638,0.597,0.543,0.459,0.3686,0.3182,
	0.2709,0.2225,0.1983,0.1819,0.1687,0.1424,0.1202,0.096,0.0748,0.0526,0.0305,0.0115,-0.0035} };

	static const double C_A_arr[Mach_arr_size][Alpha_arr_size] = { {0.548,0.51,0.42,0.31,0.2,0.11,0.21,0.12,0.3,0.23,0.1,-0.1705,-0.2654,
	-0.3753,-0.4622,-0.53,-0.5849,-0.6498,-0.7197,-0.7686,-0.7995,-0.8245,-0.8446,-0.8517,-0.8519,-0.85},
	{0.548,0.51,0.42,0.31,0.2,0.11,0.21,0.12,0.3,0.23,0.1,-0.1705,-0.2654,
	-0.3753,-0.4622,-0.53,-0.5849,-0.6498,-0.7197,-0.7686,-0.7995,-0.8245,-0.8446,-0.8517,-0.8519,-0.85},
	{0.548,0.51,0.42,0.31,0.2,0.11,0.21,0.12,0.3,0.23,0.1,-0.0405,-0.1604,
	-0.3604,-0.5003,-0.5902,-0.6601,-0.74,-0.7999,-0.8548,-0.8998,-0.8998,-0.8899,-0.8799,-0.87,-0.87},
	{0.646,0.615,0.504,0.358,0.234,0.106,0.19,0.59,0.51,0.34,0.17,-0.0606,-0.2006,
	-0.3805,-0.5503,-0.6702,-0.7901,-0.8601,-0.91,-0.9501,-0.9801,-1.0001,-1.022,-1.0199,-1.0099,-0.98},
	{0.719,0.698,0.595,0.456,0.311,0.212,0.218,0.541,0.57,0.395,0.155,-0.11,-0.3168,
	-0.4507,-0.6305,-0.7604,-0.8703,-0.9352,-0.9902,-1.0452,-1.0751,-1.1051,-1.12,-1.115,-1.101,-1.09},
	{1.015,0.999,0.918,0.72,0.497,0.408,0.403,0.561,0.491,0.23,-0.022,-0.2963,-0.4412,
	-0.6011,-0.7909,-0.9488,-1.0816,-1.1684,-1.2102,-1.2402,-1.2701,-1.2901,-1.3051,-1.315,-1.315,-1.305},
	{1.05,1.028,0.939,0.764,0.581,0.472,0.449,0.577,0.465,0.22,-0.03,-0.3113,-0.4722,
	-0.6331,-0.8,-0.9608,-1.0866,-1.1614,-1.2072,-1.2302,-1.2501,-1.2801,-1.3001,-1.31,-1.32,-1.325},
	{1.092,1.05,0.946,0.785,0.649,0.558,0.526,0.546,0.435,0.2,-0.04,-0.3214,-0.4713,
	-0.6311,-0.801,-0.9609,-1.1007,-1.2205,-1.3103,-1.3702,-1.4001,-1.4051,-1.4101,-1.415,-1.42,-1.42},
	{1.093,1.02,0.926,0.833,0.744,0.655,0.613,0.539,0.401,0.2,-0.037,-0.3113,-0.4612,
	-0.6131,-0.7709,-0.9208,-1.0606,-1.1805,-1.2703,-1.3502,-1.3902,-1.4201,-1.4301,-1.44,-1.442,-1.45},
	{0.95,0.917,0.863,0.793,0.733,0.672,0.606,0.531,0.406,0.217,-0.023,-0.2932,-0.4451,
	-0.587,-0.7269,-0.8607,-0.9896,-1.1055,-1.2294,-1.3253,-1.4022,-1.4361,-1.453,-1.467,-1.471,-1.472},
	{0.862,0.853,0.817,0.771,0.698,0.651,0.595,0.51,0.39,0.22,-0,-0.2811,-0.411,
	-0.5609,-0.7008,-0.8307,-0.9606,-1.0805,-1.1904,-1.2803,-1.3582,-1.4051,-1.4401,-1.455,-1.465,-1.47},
	{0.77,0.76,0.715,0.685,0.66,0.625,0.57,0.49,0.38,0.23,0.02,-0.251,-0.3909,
	-0.5208,-0.6607,-0.8006,-0.9205,-1.0304,-1.1404,-1.2303,-1.3202,-1.3802,-1.4201,-1.4501,-1.47,-1.475},
	{0.7,0.693,0.67,0.644,0.62,0.588,0.539,0.475,0.388,0.254,0.057,-0.2069,-0.3528,
	-0.4887,-0.6266,-0.7556,-0.8765,-0.9864,-1.0924,-1.1903,-1.2752,-1.3412,-1.3961,-1.4341,-1.454,-1.46},
	{0.65,0.67,0.643,0.622,0.624,0.591,0.544,0.469,0.383,0.263,0.06,-0.1879,-0.3308,
	-0.4706,-0.6095,-0.7335,-0.8634,-0.9794,-1.0828,-1.1763,-1.2672,-1.3452,-1.4138,-1.4611,-1.484,-1.49},
	{0.65,0.67,0.643,0.622,0.624,0.591,0.544,0.469,0.383,0.263,0.06,-0.1879,-0.3308,
	-0.4706,-0.6095,-0.7335,-0.8634,-0.9794,-1.0828,-1.1763,-1.2672,-1.3452,-1.4138,-1.4611,-1.484,-1.49},
	{0.65,0.67,0.643,0.622,0.624,0.591,0.544,0.469,0.383,0.263,0.06,-0.1879,-0.3308,
	-0.4706,-0.6095,-0.7335,-0.8634,-0.9794,-1.0828,-1.1763,-1.2672,-1.3452,-1.4138,-1.4611,-1.484,-1.49} };

	static const double C_M_arr[Mach_arr_size][Alpha_arr_size] = { {0,-0.0953,-0.1736,-0.2455,-0.3007,-0.2961,-0.1696,-0.1304,-0.2364,-0.2297,-0.1656,-0.0976,-0.0609,
	-0.0181,0.0209,0.0598,0.0977,0.1355,0.1643,0.1969,0.2083,0.2054,0.1823,0.1095,0.0367,0.0033},
	{0,-0.0953,-0.1736,-0.2455,-0.3007,-0.2961,-0.1696,-0.1304,-0.2364,-0.2297,-0.1656,-0.0976,-0.0609,
	-0.0181,0.0209,0.0598,0.0977,0.1355,0.1643,0.1969,0.2083,0.2054,0.1823,0.1095,0.0367,0.0033},
	{0,-0.0953,-0.1736,-0.2455,-0.3007,-0.2961,-0.1696,-0.1304,-0.2364,-0.2297,-0.1656,-0.1041,-0.0699,
	-0.0327,0.0067,0.0409,0.0728,0.1049,0.1362,0.1557,0.1534,0.1196,0.0813,0.048,0.025,0.0034},
	{0,-0.1066,-0.1972,-0.2741,-0.3344,-0.377,-0.3852,-0.3035,-0.303,-0.2534,-0.2094,-0.1382,-0.0983,
	-0.0544,-0.0073,0.0312,0.0678,0.0859,0.0834,0.0603,0.0385,0.0304,0.0333,0.0419,0.0327,0.0038},
	{0,-0.065,-0.1638,-0.2982,-0.3857,-0.4379,-0.4329,-0.3745,-0.3669,-0.3588,-0.3008,-0.2366,-0.1585,
	-0.1155,-0.0544,-0.0129,0.0216,0.0313,0.0245,0.021,0.0212,0.0233,0.0246,0.0271,0.0171,0.0042},
	{0,-0.0906,-0.1904,-0.2869,-0.3874,-0.4835,-0.5209,-0.5005,-0.4829,-0.4401,-0.3878,-0.3336,-0.2961,
	-0.2547,-0.1994,-0.1355,-0.0686,-0.0142,0.016,0.0161,0.017,0.0189,0.0178,0.0212,0.0155,0.005},
	{0,-0.0819,-0.1664,-0.2727,-0.4011,-0.4893,-0.5385,-0.5235,-0.5028,-0.4537,-0.3956,-0.3418,-0.3038,
	-0.2589,-0.2061,-0.1387,-0.0743,-0.0232,0.0149,0.0185,0.0147,0.0164,0.0175,0.0184,0.0154,0.0051},
	{0,-0.0875,-0.1786,-0.2811,-0.4024,-0.4916,-0.5417,-0.5298,-0.5046,-0.4662,-0.4111,-0.356,-0.319,
	-0.2731,-0.221,-0.1671,-0.1112,-0.0615,-0.0177,0.0149,0.0181,0.0177,0.0184,0.017,0.0094,0.0054},
	{0,-0.1111,-0.2229,-0.3244,-0.4171,-0.4886,-0.5143,-0.5043,-0.4796,-0.4482,-0.3978,-0.3323,-0.295,
	-0.25,-0.1984,-0.153,-0.1068,-0.0659,-0.0294,-0.0075,0.0061,0.0132,0.0146,0.0135,0.011,0.0056},
	{0,-0.144,-0.2604,-0.3585,-0.4375,-0.4906,-0.5222,-0.5233,-0.4981,-0.451,-0.3921,-0.313,-0.2704,
	-0.225,-0.1799,-0.1396,-0.1027,-0.0683,-0.0424,-0.0223,-0.005,0.0108,0.0197,0.0188,0.0136,0.0057},
	{0,-0.1674,-0.2909,-0.3816,-0.4423,-0.4829,-0.5109,-0.5046,-0.4816,-0.4391,-0.375,-0.2871,-0.241,
	-0.1942,-0.1573,-0.1222,-0.0939,-0.0644,-0.0433,-0.0237,-0.0067,0.0026,0.0079,0.0088,0.0072,0.0057},
	{0,-0.172,-0.3059,-0.3886,-0.4382,-0.4765,-0.4945,-0.4899,-0.4607,-0.416,-0.3551,-0.2629,-0.2112,
	-0.17,-0.1347,-0.1028,-0.0787,-0.0576,-0.0416,-0.0243,-0.0126,-0.0036,0.0006,0.001,0.0025,0.0057},
	{0,-0.1695,-0.3128,-0.3915,-0.4322,-0.4519,-0.465,-0.4565,-0.4253,-0.3809,-0.314,-0.234,-0.1883,
	-0.1495,-0.1146,-0.085,-0.0648,-0.0454,-0.0319,-0.0199,-0.009,-0.0023,0.0013,0.0027,0.0033,0.0057},
	{0,-0.1758,-0.3315,-0.4093,-0.4529,-0.4631,-0.4686,-0.4602,-0.4284,-0.3827,-0.3034,-0.2179,-0.169,
	-0.1258,-0.085,-0.0627,-0.049,-0.0408,-0.0274,-0.0183,-0.0096,-0.0047,0.0003,0.0034,0.0049,0.0057},
	{0,-0.1758,-0.3315,-0.4093,-0.4529,-0.4631,-0.4686,-0.4602,-0.4284,-0.3827,-0.3034,-0.2099,-0.161,
	-0.1178,-0.077,-0.0547,-0.041,-0.0328,-0.0194,-0.0103,-0.0016,0.0033,0.0083,0.0114,0.0129,0.0137},
	{0,-0.1758,-0.3315,-0.4093,-0.4529,-0.4631,-0.4686,-0.4602,-0.4284,-0.3827,-0.3034,-0.2099,-0.161,
	-0.1178,-0.077,-0.0547,-0.041,-0.0328,-0.0194,-0.0103,-0.0016,0.0033,0.0083,0.0114,0.0129,0.0137} };

	static const double Mach_arr[Mach_arr_size] = { 0.0, 0.2, 0.4, 0.7, 0.9, 1.1, 1.2, 1.35, 1.65, 2.0, 2.4, 3.0, 4.0, 10.0, 29.5, 50.0 };
	static const double alpha_arr[Alpha_arr_size] = { 0, 10.0* RAD, 20.0 * RAD, 30.0 * RAD, 40.0 * RAD, 50.0 * RAD, 60.0 * RAD, 70.0 * RAD, 80.0 * RAD, 90.0 * RAD, 100.0 * RAD, 110.1365*RAD, 115.1365*RAD,
		120.1365*RAD, 125.1365*RAD, 130.1365*RAD, 135.1365*RAD, 140.1365*RAD, 145.1365*RAD, 150.1365*RAD, 155.1365*RAD, 160.1365*RAD, 165.1365*RAD, 170.1365*RAD, 175.1365*RAD, 180.1365*RAD };

	int i, j;

	for (i = 0; (i < Mach_arr_size - 2) && (Mach_arr[i + 1] < Mach); i++);
	for (j = 0; (j < Alpha_arr_size - 2) && (alpha_arr[j + 1] < alpha); j++);

	C_M = BilinearInterpolation(C_M_arr[i][j], C_M_arr[i][j + 1], C_M_arr[i + 1][j], C_M_arr[i + 1][j + 1], Mach_arr[i], Mach_arr[i + 1], alpha_arr[j], alpha_arr[j + 1], Mach, alpha);
	C_A = BilinearInterpolation(C_A_arr[i][j], C_A_arr[i][j + 1], C_A_arr[i + 1][j], C_A_arr[i + 1][j + 1], Mach_arr[i], Mach_arr[i + 1], alpha_arr[j], alpha_arr[j + 1], Mach, alpha);
	C_NY = BilinearInterpolation(C_N_arr[i][j], C_N_arr[i][j + 1], C_N_arr[i + 1][j], C_N_arr[i + 1][j + 1], Mach_arr[i], Mach_arr[i + 1], alpha_arr[j], alpha_arr[j + 1], Mach, alpha);

	//sprintf(oapiDebugString(), "Mach %lf alpha %lf i %d j %d C_M %lf C_A %lf C_NY %lf", Mach, alpha*DEG, i, j, C_M, C_A, C_NY);
}

void CMFreeFlowAero(double alpha, double &C_M, double &C_A, double &C_NY)
{
	//Numbers from CSM Data Book, Table 5-4

	static const double alpha_arr[] = { 0, 10 * RAD, 20 * RAD, 30 * RAD, 40 * RAD, 50 * RAD, 60 * RAD, 70 * RAD, 80 * RAD, 90 * RAD, 100 * RAD, 110 * RAD, 120 * RAD, 130 * RAD, 140 * RAD, 150 * RAD, 160 * RAD, 170 * RAD, 180 * RAD };
	static const double C_N_free[] = { 0, 0.34, 0.65, 0.88, 1.06, 1.21, 1.30, 1.32, 1.30, 1.29, 1.30, 1.31, 1.29, 1.20, 1.05, 0.88, 0.65, 0.34, 0.0 };
	static const double C_A_free[] = { 2.01, 1.95, 1.78, 1.53, 1.26, 1.02, 0.75, 0.48, 0.23, 0.0, -0.23, -0.48, -0.75, -1.02, -1.27, -1.54, -1.79, -1.95, -2.02 };
	static const double C_M_A_free[] = { 0, -0.27, -0.51,  0.69, -0.80, -0.86, -0.87, -0.83, -0.78, -0.76, -0.78, -0.83, -0.87, -0.86, -0.80, -0.69, -0.51, -0.27, 0.0 };

	int i;
	for (i = 0; i < 18 && alpha_arr[i + 1] < alpha; i++);
	double f = (alpha - alpha_arr[i]) / (alpha_arr[i + 1] - alpha_arr[i]);

	C_M = C_M_A_free[i] + (C_M_A_free[i + 1] - C_M_A_free[i]) * f;
	C_A = C_A_free[i] + (C_A_free[i + 1] - C_A_free[i]) * f;
	C_NY = C_N_free[i] + (C_N_free[i + 1] - C_N_free[i]) * f;
}

void CMVertCoeffFunc(VESSEL *v, double aoa, double M, double Re, void *context, double *cl, double *cm, double *cd)
{
	double Kn = M / Re * 1.482941286; //Knudsen number. Factor is sqrt(1.4*pi/2). Describes the flow regime.

	//Get unit airspeed vector
	VECTOR3 vec;
	v->GetAirspeedVector(FRAME_LOCAL, vec);
	vec = unit(vec);

	double C_M_A, C_A, C_NY, fact, C_L, C_D, cos_alpha, sin_alpha, alpha, phi_A, cos_phi_A;

	//Calculate total angle of attack (alpha) and roll angle (phi_A)
	cos_alpha = vec.z;
	alpha = acos(cos_alpha);
	sin_alpha = sin(alpha);
	phi_A = atan2(vec.x, -vec.y);
	cos_phi_A = cos(phi_A);

	//Get body axis coefficients based on Knudsen number
	if (Kn > 10.0)
	{
		CMFreeFlowAero(alpha, C_M_A, C_A, C_NY);
	}
	else if (Kn < 0.01)
	{
		CMContinuumFlowAero(alpha, M, C_M_A, C_A, C_NY);
	}
	else
	{
		double g = (Kn - 0.01) / 9.99;
		double C_M_A1, C_A1, C_NY1, C_M_A2, C_A2, C_NY2;
		CMFreeFlowAero(alpha, C_M_A1, C_A1, C_NY1);
		CMContinuumFlowAero(alpha, M, C_M_A2, C_A2, C_NY2);

		C_A = g * C_A1 + (1.0 - g)*C_A2;
		C_NY = g * C_NY1 + (1.0 - g)*C_NY2;
		C_M_A = g * C_M_A1 + (1.0 - g)*C_M_A2;
	}

	//Calculate total lift and drag coefficients
	C_L = cos_alpha * C_NY - sin_alpha * C_A;
	C_D = sin_alpha * C_NY + cos_alpha * C_A;

	//Project coefficients into coordinates used by Orbiter
	fact = sqrt(vec.y*vec.y + vec.z*vec.z);
	*cl = C_L * cos_phi_A*fact;
	*cd = C_D;
	*cm = cos_phi_A * C_M_A;

	//sprintf(oapiDebugString(), "aoa %lf alpha_T %lf phi_A %lf Mach %Lf Kn %lf cl %lf cd %lf cm %lf LD %lf", aoa*DEG, alpha*DEG, phi_A*DEG, M, Kn, *cl, *cd, *cm, (*cl)/(*cd));
}

void CMHorizCoeffFunc(VESSEL *v, double beta, double M, double Re, void *context, double *cl, double *cm, double *cd)

{
	double Kn = M / Re * 1.482941286; //Knudsen number. Factor is sqrt(1.4*pi/2). Describes the flow regime.

	//Get unit airspeed vector
	VECTOR3 vec;
	v->GetAirspeedVector(FRAME_LOCAL, vec);
	vec = unit(vec);

	double C_M_A, C_A, C_NY, fact, C_L, cos_alpha, sin_alpha, alpha, phi_A, sin_phi_A;

	//Calculate total angle of attack (alpha) and roll angle (phi_A)
	cos_alpha = vec.z;
	alpha = acos(cos_alpha);
	sin_alpha = sin(alpha);
	phi_A = atan2(vec.x, -vec.y);
	sin_phi_A = sin(phi_A);

	//Get body axis coefficients based on Knudsen number
	if (Kn > 10.0)
	{
		CMFreeFlowAero(alpha, C_M_A, C_A, C_NY);
	}
	else if (Kn < 0.01)
	{
		CMContinuumFlowAero(alpha, M, C_M_A, C_A, C_NY);
	}
	else
	{
		double g = (Kn - 0.01) / 9.99;
		double C_M_A1, C_A1, C_NY1, C_M_A2, C_A2, C_NY2;
		CMFreeFlowAero(alpha, C_M_A1, C_A1, C_NY1);
		CMContinuumFlowAero(alpha, M, C_M_A2, C_A2, C_NY2);

		C_A = g * C_A1 + (1.0 - g)*C_A2;
		C_NY = g * C_NY1 + (1.0 - g)*C_NY2;
		C_M_A = g * C_M_A1 + (1.0 - g)*C_M_A2;
	}

	//Calculate total lift coefficient
	C_L = cos_alpha * C_NY - sin_alpha * C_A;

	//Project coefficients into coordinates used by Orbiter
	fact = sqrt(vec.x*vec.x + vec.z*vec.z);
	*cl = -C_L * sin_phi_A*fact;
	*cd = 0.0;
	*cm = sin_phi_A * C_M_A;

	//sprintf(oapiDebugString(), "beta %lf alpha_T %lf phi_A %lf Mach %Lf Kn %lf cl %lf cd %lf cm %lf", beta*DEG, alpha*DEG, phi_A*DEG, M, Kn, *cl, *cd, *cm);
}

void CMLETVertCoeffFunc(double aoa, double M, double Re, double *cl, double *cm, double *cd)

{
	const int nlift = 19;
	double factor, frac, drag, lift;
	static const double AOA[nlift] =
	{ -180.*RAD,-160.*RAD,-150.*RAD,-120.*RAD,-90.*RAD,-40.*RAD,-30.*RAD,-20.*RAD,-10.*RAD,0 * RAD,10.*RAD,20.*RAD,30.*RAD,40.*RAD,90.*RAD,120.*RAD,150.*RAD,160.*RAD,180.*RAD };
	static const double Mach[17] = { 0.0,0.7,0.9,1.1,1.2,1.35,1.65,2.0,3.0,5.0,8.0,10.5,13.5,18.2,21.5,31.0,50.0 };
	static const double LFactor[17] = { 0.3,0.392,0.466,0.607,0.641,0.488,0.446,0.435,0.416,0.415,0.405,0.400,0.385,0.385,0.375,0.35,0.33 };
	static const double DFactor[17] = { 0.9,0.944,0.991,1.068,1.044,1.270,1.28,1.267,1.213,1.134,1.15,1.158,1.18,1.18,1.193,1.224,1.25 };
	static const double CL[nlift] = { 0.0,-0.9,-1.1,-0.5,0.0,-0.316196,-0.239658,-0.193466,-0.110798,0.0,0.110798,0.193466,0.239658,0.316196,0.0,0.5,1.1,0.9,0.0 };
	static const double CM[nlift] = { 0.0,-0.02,-0.03,-0.06,-0.075,0.08,0.1,0.11,0.09,0.0,-0.09,-0.11,-0.1,-0.08,0.075,0.06,0.03,0.02,0. };
	static const double CD[nlift] = { 1.143,1.0,1.0,0.8,0.8,0.72946,0.65157,0.63798,0.65136,0.5778,0.65136,0.63798,0.65157,0.72946,0.8,0.8,1.0,1.0,1.143 };
	int j;
	factor = 2.0;
	for (j = 0; (j < 16) && (Mach[j + 1] < M); j++);
	frac = (M - Mach[j]) / (Mach[j + 1] - Mach[j]);
	drag = (frac*DFactor[j + 1] + (1.0 - frac)*DFactor[j]);
	lift = drag * (frac*LFactor[j + 1] + (1.0 - frac)*LFactor[j]);
	for (j = 0; (j < nlift - 1) && (AOA[j + 1] < aoa); j++);
	frac = (aoa - AOA[j]) / (AOA[j + 1] - AOA[j]);
	*cd = drag * (frac*CD[j + 1] + (1.0 - frac)*CD[j]);
	*cl = lift * (frac*CL[j + 1] + (1.0 - frac)*CL[j]);
	*cm = factor * (frac*CM[j + 1] + (1.0 - frac)*CM[j]);
}

void CMLETCanardVertCoeffFunc(double aoa, double M, double Re, double *cl, double *cm, double *cd)

{
	const int nlift = 19;
	double factor, frac, drag, lift;
	static const double AOA[nlift] =
	{ -180.*RAD,-160.*RAD,-150.*RAD,-120.*RAD,-90.*RAD,-40.*RAD,-30.*RAD,-20.*RAD,-10.*RAD,0 * RAD,10.*RAD,20.*RAD,30.*RAD,40.*RAD,90.*RAD,120.*RAD,150.*RAD,160.*RAD,180.*RAD };
	static const double Mach[17] = { 0.0,0.7,0.9,1.1,1.2,1.35,1.65,2.0,3.0,5.0,8.0,10.5,13.5,18.2,21.5,31.0,50.0 };
	static const double LFactor[17] = { 0.3,0.392,0.466,0.607,0.641,0.488,0.446,0.435,0.416,0.415,0.405,0.400,0.385,0.385,0.375,0.35,0.33 };
	static const double DFactor[17] = { 0.9,0.944,0.991,1.068,1.044,1.270,1.28,1.267,1.213,1.134,1.15,1.158,1.18,1.18,1.193,1.224,1.25 };
	static const double CL[nlift] = { 0.0,-0.9,-1.1,-0.5,0.0,-0.316196,-0.239658,-0.193466,-0.110798,0.0,0.110798,0.193466,0.239658,0.316196,0.0,0.5,1.1,0.9,0.0 };
	static const double CM[nlift] = { -0.05,-0.375,-0.425,-0.35,-0.25,-0.1,0.0,0.1,0.2,0.3,0.2,0.15,0.15,0.2,0.375,0.325,0.325,0.05,-0.05 };
	static const double CD[nlift] = { 1.143,1.0,1.0,0.8,0.8,0.72946,0.65157,0.63798,0.65136,0.5778,0.65136,0.63798,0.65157,0.72946,0.8,0.8,1.0,1.0,1.143 };
	int j;
	factor = 2.0;
	for (j = 0; (j < 16) && (Mach[j + 1] < M); j++);
	frac = (M - Mach[j]) / (Mach[j + 1] - Mach[j]);
	drag = (frac*DFactor[j + 1] + (1.0 - frac)*DFactor[j]);
	lift = drag * (frac*LFactor[j + 1] + (1.0 - frac)*LFactor[j]);
	for (j = 0; (j < nlift - 1) && (AOA[j + 1] < aoa); j++);
	frac = (aoa - AOA[j]) / (AOA[j + 1] - AOA[j]);
	*cd = drag * (frac*CD[j + 1] + (1.0 - frac)*CD[j]);
	*cl = lift * (frac*CL[j + 1] + (1.0 - frac)*CL[j]);
	*cm = factor * (frac*CM[j + 1] + (1.0 - frac)*CM[j]);
}

void CMLETHoriCoeffFunc(double aoa, double M, double Re, double *cl, double *cm, double *cd)

{
	const int nlift = 19;
	double factor, frac, drag, lift;
	static const double AOA[nlift] =
	{ -180.*RAD,-160.*RAD,-150.*RAD,-120.*RAD,-90.*RAD,-40.*RAD,-30.*RAD,-20.*RAD,-10.*RAD,0 * RAD,10.*RAD,20.*RAD,30.*RAD,40.*RAD,90.*RAD,120.*RAD,150.*RAD,160.*RAD,180.*RAD };
	static const double Mach[17] = { 0.0,0.7,0.9,1.1,1.2,1.35,1.65,2.0,3.0,5.0,8.0,10.5,13.5,18.2,21.5,31.0,50.0 };
	static const double LFactor[17] = { 0.3,0.392,0.466,0.607,0.641,0.488,0.446,0.435,0.416,0.415,0.405,0.400,0.385,0.385,0.375,0.35,0.33 };
	static const double DFactor[17] = { 0.9,0.944,0.991,1.068,1.044,1.270,1.28,1.267,1.213,1.134,1.15,1.158,1.18,1.18,1.193,1.224,1.25 };
	static const double CL[nlift] = { 0.0,-0.9,-1.1,-0.5,0.0,-0.316196,-0.239658,-0.193466,-0.110798,0.0,0.110798,0.193466,0.239658,0.316196,0.0,0.5,1.1,0.9,0.0 };
	static const double CM[nlift] = { 0.0,0.02,0.03,0.06,0.075,-0.08,-0.1,-0.11,-0.09,0.0,0.09,0.11,0.1,0.08,-0.075,-0.06,-0.03,-0.02,0. };
	static const double CD[nlift] = { 1.143,1.0,1.0,0.8,0.8,0.72946,0.65157,0.63798,0.65136,0.5778,0.65136,0.63798,0.65157,0.72946,0.8,0.8,1.0,1.0,1.143 };
	int j;
	factor = 2.0;
	for (j = 0; (j < 16) && (Mach[j + 1] < M); j++);
	frac = (M - Mach[j]) / (Mach[j + 1] - Mach[j]);
	drag = (frac*DFactor[j + 1] + (1.0 - frac)*DFactor[j]);
	lift = drag * (frac*LFactor[j + 1] + (1.0 - frac)*LFactor[j]);
	for (j = 0; (j < nlift - 1) && (AOA[j + 1] < aoa); j++);
	frac = (aoa - AOA[j]) / (AOA[j + 1] - AOA[j]);
	*cd = drag * (frac*CD[j + 1] + (1.0 - frac)*CD[j]);
	*cl = lift * (frac*CL[j + 1] + (1.0 - frac)*CL[j]);
	*cm = factor * (frac*CM[j + 1] + (1.0 - frac)*CM[j]);
}

void CSMAeroAxisSymmetricalCoeff(double aoa_T, double Kn, double &C_M, double &C_A, double &C_NY)
{
	//From the CSM Data Book, table 5-6.
	static const double alpha_arr[19] = { 0, 10 * RAD, 20 * RAD, 30 * RAD, 40 * RAD, 50 * RAD, 60 * RAD, 70 * RAD, 80 * RAD, 90 * RAD, 100 * RAD, 110 * RAD, 120 * RAD, 130 * RAD, 140 * RAD, 150 * RAD, 160 * RAD, 170 * RAD, 180 * RAD };
	static const double C_A_free[19] = { 2.41,2.72,2.97,3.08,3.06,2.84,2.4,1.72,0.88,0.01,-0.84,-1.3,-1.6,-2.08,-2.31,-2.36,-2.26,-2.05,-1.96 };
	static const double C_N_free[19] = { 0,0.43,1.06,1.84,2.68,3.51,4.23,4.73,4.97,5.01,5.07,4.86,4.39,3.7,2.89,2.08,1.29,0.49,0 };
	static const double C_A_cont[19] = { 0.54,0.55,0.57,0.61,0.66,0.69,0.69,0.65,0.56,0.46,0.28,0.01,-0.31,-0.65,-0.97,-1.22,-1.41,-1.52,-1.69 };
	static const double C_N_cont[19] = { 0,0.28,0.69,1.17,1.66,2.1,2.48,2.72,2.81,2.78,2.55,2.2,1.77,1.32,0.89,0.54,0.24,0.06,0 };

	//Calculated for CG location X=950, Y=0, Z=0 inches
	static const double C_M_cg_free[19] = { 0,0.0440, -0.1436, -0.4349, -0.7318, -0.9010, -1.0068, -0.9859, -0.8778, -0.7682, -0.9437, -1.0544, -1.0481, -0.9350, -0.6710, -0.2369,  0.3820, -0.0115, 0 };
	static const double C_M_cg_cont[19] = { 0,0.0977,  0.1069,  0.0230, -0.1085, -0.2520, -0.3901, -0.5021, -0.5803, -0.6476, -0.6132, -0.5379, -0.4219, -0.2707, -0.1247,   0.0206, 0.0981,  0.0145, 0 };

	int i;
	for (i = 0; i < 18 && alpha_arr[i + 1] < aoa_T; i++);
	double f = (aoa_T - alpha_arr[i]) / (alpha_arr[i + 1] - alpha_arr[i]);

	if (Kn > 10.0)
	{
		//Free flow
		C_A = C_A_free[i] + (C_A_free[i + 1] - C_A_free[i]) * f;
		C_NY = C_N_free[i] + (C_N_free[i + 1] - C_N_free[i]) * f;
		C_M = C_M_cg_free[i] + (C_M_cg_free[i + 1] - C_M_cg_free[i]) * f;
	}
	else if (Kn < 0.01)
	{
		//Continuum flow
		C_A = C_A_cont[i] + (C_A_cont[i + 1] - C_A_cont[i]) * f;
		C_NY = C_N_cont[i] + (C_N_cont[i + 1] - C_N_cont[i]) * f;
		C_M = C_M_cg_cont[i] + (C_M_cg_cont[i + 1] - C_M_cg_cont[i]) * f;
	}
	else
	{
		//Mix
		double g = (Kn - 0.01) / 9.99;
		C_A = g * (C_A_free[i] + (C_A_free[i + 1] - C_A_free[i]) * f) + (1.0 - g)*(C_A_cont[i] + (C_A_cont[i + 1] - C_A_cont[i]) * f);
		C_NY = g * (C_N_free[i] + (C_N_free[i + 1] - C_N_free[i]) * f) + (1.0 - g)*(C_N_cont[i] + (C_N_cont[i + 1] - C_N_cont[i]) * f);
		C_M = g * (C_M_cg_free[i] + (C_M_cg_free[i + 1] - C_M_cg_free[i]) * f) + (1.0 - g)*(C_M_cg_cont[i] + (C_M_cg_cont[i + 1] - C_M_cg_cont[i]) * f);
	}

	//sprintf(oapiDebugString(), "aoa_T %lf Kn %lf C_M %lf C_A %lf C_NY %lf i %d", aoa_T*DEG, Kn, C_M, C_A, C_NY, i);
}

void CSMAeroVertCoeff(VESSEL *v, double aoa, double M, double Re, void *context, double *cl, double *cm, double *cd)
{
	//Calculate unit airspeed vector
	VECTOR3 vec;
	v->GetAirspeedVector(FRAME_LOCAL, vec);
	vec = unit(vec);

	//Calculate axis symmetric angles
	double cos_alpha, sin_alpha, alpha, phi_A, C_L, C_D, cos_phi_A;

	cos_alpha = vec.z;
	alpha = acos(vec.z);
	sin_alpha = sin(alpha);
	phi_A = atan2(vec.x, -vec.y);
	cos_phi_A = cos(phi_A);

	//Calculate Knudsen number
	double Kn = M / Re * 1.482941286; //Knudsen number. Factor is sqrt(1.4*pi/2)

	double C_A, C_NY, C_M;
	CSMAeroAxisSymmetricalCoeff(alpha, Kn, C_M, C_A, C_NY);

	//Calculate total lift and drag coefficients
	C_L = cos_alpha * C_NY - sin_alpha * C_A;
	C_D = sin_alpha * C_NY + cos_alpha * C_A;

	double fact = sqrt(vec.y*vec.y + vec.z*vec.z);
	*cl = 0.0; //C_L * cos_phi_A*fact; //TBD: Use lift when full drag gets used, too
	*cd = C_D;
	*cm = C_M * cos_phi_A;

	//TBD: Remove when RTCC takes drag into account properly
	*cd = (*cd)*0.05;
	
	//sprintf(oapiDebugString(), "C_L %lf C_D %lf cos_phi_A %lf fact %lf cl %lf cd %lf cm %lf", C_L, C_D, cos_phi_A, fact, *cl, *cd, *cm);
	//sprintf(oapiDebugString(), "Vertical: aoa %lf aoa_T %lf phi_A %Lf M %lf Re %lf Kn %lf CD %lf CL %lf CM %lf", aoa*DEG, alpha*DEG, phi_A*DEG, M, Re, Kn, *cd, *cl, *cm);
}

void CSMAeroHorizCoeff(VESSEL *v, double aoa, double M, double Re, void *context, double *cl, double *cm, double *cd)
{
	//Calculate unit airspeed vector
	VECTOR3 vec;
	v->GetAirspeedVector(FRAME_LOCAL, vec);
	vec = unit(vec);

	//Calculate axis symmetric angles
	double cos_alpha, sin_alpha, alpha, phi_A, C_L, sin_phi_A;

	cos_alpha = vec.z;
	alpha = acos(vec.z);
	sin_alpha = sin(alpha);
	phi_A = atan2(vec.x, -vec.y);
	sin_phi_A = sin(phi_A);

	//Calculate Knudsen number
	double Kn = M / Re * 1.482941286; //Knudsen number. Factor is sqrt(1.4*pi/2)

	double C_A, C_NY, C_M;
	CSMAeroAxisSymmetricalCoeff(alpha, Kn, C_M, C_A, C_NY);

	//Calculate total lift coefficient
	C_L = cos_alpha * C_NY - sin_alpha * C_A;

	double fact = sqrt(vec.x*vec.x + vec.z*vec.z);
	*cl = 0.0;	//-C_L * sin_phi_A*fact; //TBD: Use lift when full drag gets used, too
	*cd = 0;	//Total drag is applied in other airfoil function
	*cm = C_M * sin_phi_A;

	//sprintf(oapiDebugString(), "C_L %lf sin_phi_A %lf fact %lf cl %lf cm %lf", C_L, sin_phi_A, fact, *cl, *cm);
	//sprintf(oapiDebugString(), "Horizontal: beta %lf aoa_T %lf phi_A %lf M %lf Re %lf Kn %lf CD %lf CL %lf CM %lf", aoa*DEG, alpha*DEG, phi_A*DEG, M, Re, Kn, *cd, *cl, *cm);
}

void SaturnInitMeshes()

{
	LOAD_MESH(hSM, "ProjectApollo/SM-core");
	LOAD_MESH(hSMRCS, "ProjectApollo/SM-RCSHI");
	LOAD_MESH(hSMRCSLow, "ProjectApollo/SM-RCSLO");
	LOAD_MESH(hSMSPS, "ProjectApollo/SM-SPS");
	LOAD_MESH(hSMPanel1, "ProjectApollo/SM-Panel1");
	LOAD_MESH(hSMPanel2, "ProjectApollo/SM-Panel2");
	LOAD_MESH(hSMPanel3, "ProjectApollo/SM-Panel3");
	LOAD_MESH(hSMPanel4, "ProjectApollo/SM-Panel4");
	LOAD_MESH(hSMPanel5, "ProjectApollo/SM-Panel5");
	LOAD_MESH(hSMPanel6, "ProjectApollo/SM-Panel6");
	LOAD_MESH(hSMhga, "ProjectApollo/SM-HGA");
	LOAD_MESH(hSMCRYO, "ProjectApollo/SM-CRYO");
	LOAD_MESH(hSMSIMBAY, "ProjectApollo/SM-SIMBAY");
	LOAD_MESH(hCM, "ProjectApollo/CM");
	LOAD_MESH(hCMnh, "ProjectApollo/CM-Nohatch");
	LOAD_MESH(hCM2, "ProjectApollo/CM-Recov");
	LOAD_MESH(hCMP, "ProjectApollo/CM-CMP");
	LOAD_MESH(hCMInt, "ProjectApollo/CM-Interior");
	LOAD_MESH(hCMVC, "ProjectApollo/CM-VC");
	LOAD_MESH(hCREW, "ProjectApollo/CM-CREW");
	LOAD_MESH(hFHC, "ProjectApollo/CM-HatchC");
	LOAD_MESH(hFHO, "ProjectApollo/CM-HatchO");
	LOAD_MESH(hFHF, "ProjectApollo/CM-HatchF");
	LOAD_MESH(hCM2B, "ProjectApollo/CMB-Recov");
	LOAD_MESH(hdockring, "ProjectApollo/CM-DockRing");
	LOAD_MESH(hprobe, "ProjectApollo/CM-ProbeRetracted");
	LOAD_MESH(hprobeext, "ProjectApollo/CM-ProbeExtended");
	LOAD_MESH(hCRB, "ProjectApollo/CM-CrewRecovery");
	LOAD_MESH(hCMB, "ProjectApollo/CMB");
	LOAD_MESH(hChute30, "ProjectApollo/Apollo_2chute");
	LOAD_MESH(hChute31, "ProjectApollo/Apollo_3chuteEX");
	LOAD_MESH(hChute32, "ProjectApollo/Apollo_3chuteHD");
	LOAD_MESH(hApollochute, "ProjectApollo/Apollo_3chute");
	LOAD_MESH(hFHC2, "ProjectApollo/CMB-HatchC");
	LOAD_MESH(hsat5tower, "ProjectApollo/BoostCover");
	LOAD_MESH(hFHO2, "ProjectApollo/CMB-HatchO");
	LOAD_MESH(hCMPEVA, "ProjectApollo/CM-CMPEVA");
	LOAD_MESH(hopticscover, "ProjectApollo/CM-OpticsCover");
	LOAD_MESH(hcmdocktgt, "ProjectApollo/CM-Docktgt");
	LOAD_MESH(hcmseatsfolded, "ProjectApollo/CM-VC-SeatsFolded");
	LOAD_MESH(hcmseatsunfolded, "ProjectApollo/CM-VC-SeatsUnfolded");
	LOAD_MESH(hcmWindowShades[0], "ProjectApollo/CM-VC_Shade1");
	LOAD_MESH(hcmWindowShades[1], "ProjectApollo/CM-VC_Shade2");
	LOAD_MESH(hcmWindowShades[2], "ProjectApollo/CM-VC_Shade3");
	LOAD_MESH(hcmWindowShades[3], "ProjectApollo/CM-VC_Shade4");
	LOAD_MESH(hcmWindowShades[4], "ProjectApollo/CM-VC_Shade5");
	LOAD_MESH(hcmCOAScdr, "ProjectApollo/CM-COAS-CDR");
	LOAD_MESH(hcmCOAScdrreticle, "ProjectApollo/CM-COAS-CDR_Reticle");

	SURFHANDLE contrail_tex = oapiRegisterParticleTexture("Contrail2");
	lem_exhaust.tex = contrail_tex;
}

void Saturn::AddSM(double offset, bool showSPS)

{
	VECTOR3 mesh_dir=_V(0, SMVO, offset);

	AddMesh (hSM, &mesh_dir);

	if (LowRes)
		AddMesh(hSMRCSLow, &mesh_dir);
	else
		AddMesh (hSMRCS, &mesh_dir);

	if (!SIMBayPanelJett)
	{
		simbaypanelidx = AddMesh(hSMPanel1, &mesh_dir);
	}

	AddMesh (hSMPanel2, &mesh_dir);
	AddMesh (hSMPanel3, &mesh_dir);

	if (!ApolloExploded)
		AddMesh (hSMPanel4, &mesh_dir);
	else
		AddMesh (hSMCRYO, &mesh_dir);

	AddMesh (hSMPanel5, &mesh_dir);
	AddMesh (hSMPanel6, &mesh_dir);
	AddMesh (hSMSIMBAY, &mesh_dir);

	if (showSPS) {
		mesh_dir = _V(0, SMVO, offset - 1.654);
		SPSidx = AddMesh(hSMSPS, &mesh_dir);
		SPSEngine.DefineAnimations(SPSidx);
	}
}




///\todo needs to be redesigned

/*
void Saturn::ToggleEVA()

{
	UINT meshidx;

	//
	// EVA does nothing if we're unmanned.
	//

	if (!Crewed)
		return;

	ToggleEva = false;

	if (EVA_IP){
		EVA_IP =false;

		ClearMeshes();
		VECTOR3 mesh_dir=_V(0,SMVO,30.25-12.25-21.5);
		AddMesh (hSM, &mesh_dir);

		//
		// Skylab SM and Apollo 7 have no HGA.
		//
		if (!NoHGA) {
			mesh_dir=_V(-1.308,-1.18,29.042-12.25-21.5);
			AddMesh (hSMhga, &mesh_dir);
		}

		mesh_dir=_V(0,0,34.4-12.25-21.5);

		// And the Crew
		if (Crewed) {
			cmpidx = AddMesh (hCMP, &mesh_dir);
			crewidx = AddMesh (hCREW, &mesh_dir);
			SetCrewMesh();
		} else {
			cmpidx = -1;
			crewidx = -1;
		}

		meshidx = AddMesh (hCMInt, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_EXTERNAL);

		meshidx = AddMesh (hCMVC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VC);
		VCMeshOffset = mesh_dir;

		meshidx = AddMesh (hCM, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		//Don't Forget the Hatch
		meshidx = AddMesh (hFHO, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		HatchOpen = true;
		if (HasProbe) {
			probeidx = AddMesh(hprobe, &mesh_dir);
			probeextidx = AddMesh(hprobeext, &mesh_dir);
			SetDockingProbeMesh();
		} else {
			probeidx = -1;
			probeextidx = -1;
		}
	}
	else 
	{
		EVA_IP = true;

		ClearMeshes();
		VECTOR3 mesh_dir=_V(0,SMVO,30.25-12.25-21.5);
		AddMesh (hSM, &mesh_dir);
		mesh_dir=_V(-1.308,-1.18,29.042-12.25-21.5);
		AddMesh (hSMhga, &mesh_dir);

		mesh_dir=_V(0,0,34.4-12.25-21.5);

		// And the Crew, CMP is outside
		if (Crewed) {
			crewidx = AddMesh (hCREW, &mesh_dir);
			SetCrewMesh();
		} else {
			crewidx = -1;
		}
		cmpidx = -1;

		meshidx = AddMesh (hCMInt, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_EXTERNAL);

		meshidx = AddMesh (hCMVC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VC);
		VCMeshOffset = mesh_dir;

		meshidx = AddMesh (hCM, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		//Don't Forget the Hatch
		meshidx = AddMesh (hFHO, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		HatchOpen= true;

		if (HasProbe) {
			probeidx = AddMesh(hprobe, &mesh_dir);
			probeextidx = AddMesh(hprobeext, &mesh_dir);
			SetDockingProbeMesh();
		} else {
			probeidx = -1;
			probeextidx = -1;
		}

		VESSELSTATUS vs1;
		GetStatus(vs1);
		VECTOR3 ofs1 = _V(0,0.15,34.25-12.25-21.5);
		VECTOR3 vel1 = _V(0,0,0);
		VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};
		Local2Rel (ofs1, vs1.rpos);
		GlobalRot (vel1, rofs1);
		vs1.rvel.x = rvel1.x+rofs1.x;
		vs1.rvel.y = rvel1.y+rofs1.y;
		vs1.rvel.z = rvel1.z+rofs1.z;
		char VName[256]="";
		strcpy (VName, GetName()); strcat (VName, "-EVA");
		hEVA = oapiCreateVessel(VName,"ProjectApollo/EVA",vs1);
		oapiSetFocusObject(hEVA);
	}
}

void Saturn::SetupEVA()

{
	UINT meshidx;

	if (EVA_IP){
		EVA_IP =true;

		ClearMeshes();
		VECTOR3 mesh_dir=_V(0,SMVO,30.25-12.25-21.5);
		AddMesh (hSM, &mesh_dir);

		//
		// Skylab SM and Apollo 7 have no HGA.
		//
		if (!NoHGA) {
			mesh_dir=_V(-1.308,-1.18,29.042-12.25-21.5);
			AddMesh (hSMhga, &mesh_dir);
		}

		mesh_dir=_V(0,0,34.4-12.25-21.5);

		// And the Crew, CMP is outside
		if (Crewed) {
			crewidx = AddMesh (hCREW, &mesh_dir);
			SetCrewMesh();
		} else {
			crewidx = -1;
		}
		cmpidx = -1;

		meshidx = AddMesh (hCMInt, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_EXTERNAL);

		meshidx = AddMesh (hCMVC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VC);
		VCMeshOffset = mesh_dir;

		meshidx = AddMesh (hCM, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		//Don't Forget the Hatch
		meshidx = AddMesh (hFHO, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		HatchOpen = true;

		if (HasProbe) {
			probeidx = AddMesh(hprobe, &mesh_dir);
			probeextidx = AddMesh(hprobeext, &mesh_dir);
			SetDockingProbeMesh();
		} else {
			probeidx = -1;
			probeextidx = -1;
		}
	}
}
*/


void Saturn::SetCSMStage (VECTOR3 cg_ofs)
{
	ClearMeshes();
    ClearThrusterDefinitions();
	ClearAirfoilDefinitions();
	ClearEngineIndicators();
	ClearLVGuidLight();
	ClearLVRateLight();
	ClearSIISep();
	agc.SetInputChannelBit(030, LiftOff, false);
	agc.SetInputChannelBit(030, GuidanceReferenceRelease, false);
	agc.SetInputChannelBit(030, UllageThrust, false);

	// Because all meshes are getting reloaded, we have to shift CG back to the center of the mesh, and then re-apply the offset CG on the next timestep
	// Only necessary because of LET jettison function reloading all meshes.
	ShiftCG(-currentCoG + cg_ofs);
	currentCoG = _V(0, 0, 0);
	LastFuelWeight = numeric_limits<double>::infinity();

	//
	// Delete any dangling propellant resources.
	//

	if (ph_ullage1)
	{
		DelPropellantResource(ph_ullage1);
		ph_ullage1 = 0;
	}

	if (ph_ullage2)
	{
		DelPropellantResource(ph_ullage2);
		ph_ullage2 = 0;
	}

	if (ph_ullage3)
	{
		DelPropellantResource(ph_ullage3);
		ph_ullage3 = 0;
	}

	if (ph_1st) {
		DelPropellantResource(ph_1st);
		ph_1st = 0;
	}

	if (ph_2nd) {
		DelPropellantResource(ph_2nd);
		ph_2nd = 0;
	}

	if (ph_3rd) {
		DelPropellantResource(ph_3rd);
		ph_3rd = 0;
	}

	if (ph_sep) {
		DelPropellantResource(ph_sep);
		ph_sep = 0;
	}

	if (ph_sep2) {
		DelPropellantResource(ph_sep2);
		ph_sep2 = 0;
	}

	if (ph_aps1) {
		DelPropellantResource(ph_aps1);
		ph_aps1 = 0;
	}

	if (ph_aps2) {
		DelPropellantResource(ph_aps2);
		ph_aps2 = 0;
	}

	SetSize(10);
	SetCOG_elev(3.5);
	SetEmptyMass(CM_EmptyMass + SM_EmptyMass + (LESAttached ? Abort_Mass : 0.0));

	// ************************* propellant specs **********************************

	if (!ph_sps) {
		ph_sps = CreatePropellantResource(SM_FuelMass, SM_FuelMass); //SPS stage propellant
	}

	SetDefaultPropellantResource (ph_sps); // display SPS stage propellant level in generic HUD

	// *********************** thruster definitions ********************************

	th_sps[0] = CreateThruster(_V(0.0, 0.0, -3.37472), _V(0, 0, 1), SPS_THRUST, ph_sps, SPS_ISP);
	thg_sps = CreateThrusterGroup(th_sps, 1, THGROUP_USER);

	VECTOR3 spspos0 = _V(0.0, 0.0, -5);
	EXHAUSTSPEC es_sps[1] = {
		{ th_sps[0], NULL, &spspos0, NULL, 20.0, 2.25, 0, 0.1, SMExhaustTex, EXHAUST_CONSTANTPOS }
	};

	AddExhaust(es_sps);
	//SetPMI(_V(12, 12, 7));
	SetPMI(_V(4.3972, 4.6879, 1.6220));
	SetCrossSections(_V(26.0,26.0,12.02));
	SetCW(2.41*2.0, 1.96*2.0, 2.36*2.0, 2.36*2.0);
	CreateAirfoil3(LIFT_VERTICAL, _V(0, 0, 0), CSMAeroVertCoeff, NULL, 154.0*0.0254, 129.4*0.3048*0.3048, 0.1);
	CreateAirfoil3(LIFT_HORIZONTAL, _V(0, 0, 0), CSMAeroHorizCoeff, NULL, 154.0*0.0254, 129.4*0.3048*0.3048, 0.1);
	SetRotDrag(_V(0.7,0.7,0.3));
	SetPitchMomentScale(0);
	SetYawMomentScale(0);
	SetLiftCoeffFunc(0);

	const double CGOffset = 12.25+21.5-1.8+0.35;
	AddSM(30.25 - CGOffset, true);

	ConfigTouchdownPoints();

	VECTOR3 mesh_dir;

	//
	// Skylab SM and Apollo 7 have no HGA.
	//
	if (pMission->CSMHasHGA()) {
		UINT HGAidx;
		mesh_dir=_V(-1.308,-1.18,29.042-CGOffset);
		HGAidx = AddMesh (hSMhga, &mesh_dir);
		hga.DefineAnimations(HGAidx);
	}

	mesh_dir=_V(0, 0, 34.4 - CGOffset);

	UINT meshidx;
	meshidx = AddMesh (hCMnh, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	if (LESAttached) {
		TowerOffset = 4.95;
		VECTOR3 mesh_dir_tower = mesh_dir + _V(0, 0, TowerOffset);
		TowerOffset += 2.1; //Additional offset for CSM stage

		meshidx = AddMesh(hsat5tower, &mesh_dir_tower);
		SetMeshVisibilityMode(meshidx, MESHVIS_VCEXTERNAL);
	}

	// And the Crew
	if (Crewed) {
		cmpidx = AddMesh (hCMP, &mesh_dir);
		crewidx = AddMesh (hCREW, &mesh_dir);
		SetCrewMesh();
	} else {
		cmpidx = -1;
		crewidx = -1;
	}

	//CM docking target
	VECTOR3 dt_dir = _V(0.66, 1.07, 2.1);
	cmdocktgtidx = AddMesh(hcmdocktgt, &dt_dir);
	SetCMdocktgtMesh();

	//Don't Forget the Hatch
	sidehatchidx = AddMesh (hFHC, &mesh_dir);
	sidehatchopenidx = AddMesh (hFHO, &mesh_dir);
	SetSideHatchMesh();

	//Forward Hatch
	fwdhatchidx = AddMesh(hFHF, &mesh_dir);
	SetFwdHatchMesh();

	// VC
	UpdateVC(mesh_dir);
	seatsfoldedidx = AddMesh(hcmseatsfolded, &mesh_dir);
	seatsunfoldedidx = AddMesh(hcmseatsunfolded, &mesh_dir);
	SetVCSeatsMesh();
	windowshadesidx[0] = AddMesh(hcmWindowShades[0], &mesh_dir);
	windowshadesidx[1] = AddMesh(hcmWindowShades[1], &mesh_dir);
	windowshadesidx[2] = AddMesh(hcmWindowShades[2], &mesh_dir);
	windowshadesidx[3] = AddMesh(hcmWindowShades[3], &mesh_dir);
	windowshadesidx[4] = AddMesh(hcmWindowShades[4], &mesh_dir);
	SetWindowShades();
	coascdrreticleidx = AddMesh(hcmCOAScdrreticle, &mesh_dir);
	coascdridx = AddMesh(hcmCOAScdr, &mesh_dir);
	SetCOASMesh();

	//Interior
	meshidx = AddMesh(hCMInt, &mesh_dir);
	SetMeshVisibilityMode(meshidx, MESHVIS_EXTERNAL);

	//Add CM meshes. More to be added here...
	AddCMMeshes(mesh_dir);

	// Docking port
	VECTOR3 dockpos = {0,0,35.90-CGOffset};
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = {0,1,0};
	SetDockParams(dockpos, dockdir, dockrot);

	//
	// SM RCS
	//
	AddRCSJets(-0.18, SM_RCS_THRUST);

	//
	// CM RCS
	//
	AddRCS_CM(CM_RCS_THRUST, 34.4 - CGOffset, false);

	//
	// Waste dump streams
	//
	SetWaterDumpParticleStreams(currentCoG + _V(0, 0, 32.3));

	//
	// Apollo 13 special handling
	//

	if (ApolloExploded && !ph_o2_vent) {

		double tank_mass = CSM_O2TANK_CAPACITY / 1000.0;

		ph_o2_vent = CreatePropellantResource(tank_mass, tank_mass); //"Thruster" created by O2 venting

		SetPropellantMass(ph_o2_vent, O2Tanks[0]->mass / 1E3);

	}
	
	if (ApolloExploded) {
		VECTOR3 vent_pos = {0, 1.5, 30.25 - CGOffset};
		VECTOR3 vent_dir = {0.5, 1, 0};

		th_o2_vent = CreateThruster (vent_pos, vent_dir, 30.0, ph_o2_vent, 300.0);
		AddExhaustStream(th_o2_vent, &o2_venting_spec);
	}


	SetView(0.4 + 1.8 - 0.35);

	// **************************** NAV radios *************************************
	InitNavRadios (4);
	EnableTransponder (true);
	OrbiterAttitudeToggle.SetActive(true);
}

void Saturn::CreateSIVBStage(char *config, VESSELSTATUS &vs1, bool SaturnVStage)

{
	char VName[256]="";

	GetApolloName(VName); strcat (VName, "-S4BSTG");
	OBJHANDLE hs4bM = oapiCreateVessel(VName, config, vs1);

	SIVBSettings S4Config;

	//
	// For now we'll only seperate the panels on ASTP.
	//

	S4Config.SettingsType.word = 0;
	S4Config.SettingsType.SIVB_SETTINGS_FUEL = 1;
	S4Config.SettingsType.SIVB_SETTINGS_GENERAL = 1;
	S4Config.SettingsType.SIVB_SETTINGS_MASS = 1;
	S4Config.SettingsType.SIVB_SETTINGS_PAYLOAD = 1;
	S4Config.SettingsType.SIVB_SETTINGS_ENGINES = 1;
	S4Config.SettingsType.SIVB_SETTINGS_PAYLOAD_INFO = 1;
	S4Config.Payload = SIVBPayload;
	S4Config.VehicleNo = VehicleNo;
	S4Config.EmptyMass = S4B_EmptyMass;
	S4Config.MainFuelKg = GetPropellantMass(ph_3rd);
	S4Config.MainFuelMaxKg = GetPropellantMaxMass(ph_3rd);
	S4Config.ApsFuel1Kg = GetPropellantMass(ph_aps1);
	S4Config.ApsFuel2Kg = GetPropellantMass(ph_aps2);
	S4Config.PayloadMass = S4PL_Mass;
	S4Config.SaturnVStage = SaturnVStage;
	S4Config.IUSCContPermanentEnabled = IUSCContPermanentEnabled;
	S4Config.MissionNo = ApolloNo;
	S4Config.MissionTime = MissionTime;
	S4Config.LowRes = LowRes;
	S4Config.ISP_VAC = ISP_THIRD_VAC;
	S4Config.THRUST_VAC = THRUST_THIRD_VAC;
	S4Config.PanelsHinged = !SLAWillSeparate;
	S4Config.SLARotationLimit = (double) SLARotationLimit;
	S4Config.PanelProcess = 0.0;

	GetPayloadName(S4Config.PayloadName);
	strncpy(S4Config.CSMName, GetName(), 63);
	S4Config.Crewed = Crewed;

	S4Config.LMAscentFuelMassKg = LMAscentFuelMassKg;
	S4Config.LMDescentFuelMassKg = LMDescentFuelMassKg;
	S4Config.LMAscentEmptyMassKg = LMAscentEmptyMassKg;
	S4Config.LMDescentEmptyMassKg = LMDescentEmptyMassKg;
	S4Config.LMPad = LMPad;
	S4Config.LMPadCount = LMPadCount;
	S4Config.AEAPad = AEAPad;
	S4Config.AEAPadCount = AEAPadCount;
	sprintf(S4Config.LEMCheck, LEMCheck);

	S4Config.iu_pointer = iu;
	S4Config.sivb_pointer = sivb;
	DontDeleteIU = true;

	SIVB *SIVBVessel = static_cast<SIVB *> (oapiGetVesselInterface(hs4bM));
	SIVBVessel->SetState(S4Config);

	PayloadDataTransfer = true;

}

void Saturn::SetDockingProbeMesh() {

	if (probeidx == -1 || probeextidx == -1 || dockringidx == -1)
		return;

	if (HasProbe) {
		SetMeshVisibilityMode(dockringidx, MESHVIS_VCEXTERNAL);
	} else {
		SetMeshVisibilityMode(dockringidx, MESHVIS_NEVER);
	}

	if (HasProbe && !ForwardHatch.IsOpen()) {
		if (!dockingprobe.IsRetracted()) {
			SetMeshVisibilityMode(probeidx, MESHVIS_NEVER);
			SetMeshVisibilityMode(probeextidx, MESHVIS_VCEXTERNAL);
		} else {
			SetMeshVisibilityMode(probeidx, MESHVIS_VCEXTERNAL);
			SetMeshVisibilityMode(probeextidx, MESHVIS_NEVER);
		}
	} else {
		SetMeshVisibilityMode(probeidx, MESHVIS_NEVER);
		SetMeshVisibilityMode(probeextidx, MESHVIS_NEVER);
	}
}

void Saturn::SetSIMBayPanelMesh()
{
	if (simbaypanelidx == -1)
		return;

	if (!SIMBayPanelJett) {
		SetMeshVisibilityMode(simbaypanelidx, MESHVIS_EXTERNAL);
	}
	else {
		SetMeshVisibilityMode(simbaypanelidx, MESHVIS_NEVER);
	}
}

void Saturn::SetSideHatchMesh() {

	if (sidehatchidx == -1 || sidehatchopenidx == -1)
		return;

	if (SideHatch.IsOpen()) {
		SetMeshVisibilityMode(sidehatchidx, MESHVIS_NEVER);
		SetMeshVisibilityMode(sidehatchopenidx, MESHVIS_EXTERNAL);
	} else {
		SetMeshVisibilityMode(sidehatchidx, MESHVIS_EXTERNAL);
		SetMeshVisibilityMode(sidehatchopenidx, MESHVIS_NEVER);
	}

	if (sidehatchburnedidx == -1 || sidehatchburnedopenidx == -1)
		return;

	if (!Burned) {
		SetMeshVisibilityMode(sidehatchburnedidx, MESHVIS_NEVER);
		SetMeshVisibilityMode(sidehatchburnedopenidx, MESHVIS_NEVER);
		return;
	}

	SetMeshVisibilityMode(sidehatchidx, MESHVIS_NEVER);
	SetMeshVisibilityMode(sidehatchopenidx, MESHVIS_NEVER);

	if (SideHatch.IsOpen()) {
		SetMeshVisibilityMode(sidehatchburnedidx, MESHVIS_NEVER);
		SetMeshVisibilityMode(sidehatchburnedopenidx, MESHVIS_EXTERNAL);
	} else {
		SetMeshVisibilityMode(sidehatchburnedidx, MESHVIS_EXTERNAL);
		SetMeshVisibilityMode(sidehatchburnedopenidx, MESHVIS_NEVER);
	}
}

void Saturn::SetFwdHatchMesh() {

	if (fwdhatchidx == -1)
		return;

	if (ForwardHatch.IsOpen()) {
		SetMeshVisibilityMode(fwdhatchidx, MESHVIS_NEVER);
	}
	else {
		SetMeshVisibilityMode(fwdhatchidx, MESHVIS_EXTERNAL);
	}
}

void Saturn::SetCrewMesh() {

	if (cmpidx != -1) {
		if (Crewed && (Crew->number == 1 || Crew->number >= 3)) {
			SetMeshVisibilityMode(cmpidx, MESHVIS_EXTERNAL);
		} else {
			SetMeshVisibilityMode(cmpidx, MESHVIS_NEVER);
		}
	}
	if (crewidx != -1) {
		if (Crewed && Crew->number >= 2) {
			SetMeshVisibilityMode(crewidx, MESHVIS_EXTERNAL);
		} else {
			SetMeshVisibilityMode(crewidx, MESHVIS_NEVER);
		}
	}
}

void Saturn::SetOpticsCoverMesh() {

	if (opticscoveridx == -1)
		return;
	
	if (optics.OpticsCovered) {
		SetMeshVisibilityMode(opticscoveridx, MESHVIS_EXTERNAL);
	} else {
		SetMeshVisibilityMode(opticscoveridx, MESHVIS_NEVER);
	}
}

void Saturn::SetCMdocktgtMesh() {

	if (cmdocktgtidx == -1)
		return;

	if (CMdocktgt && ApexCoverAttached) {
		SetMeshVisibilityMode(cmdocktgtidx, MESHVIS_VCEXTERNAL);
	}
	else {
		SetMeshVisibilityMode(cmdocktgtidx, MESHVIS_NEVER);
	}
}

void Saturn::SetVCSeatsMesh() {

	if (seatsfoldedidx == -1 || seatsunfoldedidx == -1)
		return;

	if (VCSeatsfolded) {
		SetMeshVisibilityMode(seatsfoldedidx, MESHVIS_VC);
		SetMeshVisibilityMode(seatsunfoldedidx, MESHVIS_NEVER);
	} else {
		SetMeshVisibilityMode(seatsfoldedidx, MESHVIS_NEVER);
		SetMeshVisibilityMode(seatsunfoldedidx, MESHVIS_VC);
	}
}

void Saturn::SetWindowShades() {

	if (!hcmWindowShades[0])
		return;
	
	if (CSMWindowShades[0]) { SetMeshVisibilityMode(windowshadesidx[0], MESHVIS_VC); }
	else { SetMeshVisibilityMode(windowshadesidx[0], MESHVIS_NEVER); }

	if (CSMWindowShades[1]) { SetMeshVisibilityMode(windowshadesidx[1], MESHVIS_VC); }
	else { SetMeshVisibilityMode(windowshadesidx[1], MESHVIS_NEVER); }

	if (CSMWindowShades[2]) { SetMeshVisibilityMode(windowshadesidx[2], MESHVIS_VC); }
	else { SetMeshVisibilityMode(windowshadesidx[2], MESHVIS_NEVER); }

	if (CSMWindowShades[3]) { SetMeshVisibilityMode(windowshadesidx[3], MESHVIS_VC); }
	else { SetMeshVisibilityMode(windowshadesidx[3], MESHVIS_NEVER); }

	if (CSMWindowShades[4]) { SetMeshVisibilityMode(windowshadesidx[4], MESHVIS_VC); }
	else { SetMeshVisibilityMode(windowshadesidx[4], MESHVIS_NEVER); }
}

void Saturn::SetCOASMesh() {

	if (coascdridx == -1 || coascdrreticleidx == -1)
		return;

	if (coasEnabled) {
		SetMeshVisibilityMode(coascdridx, MESHVIS_VC);
		if (InVC && oapiCameraInternal() && viewpos == SATVIEW_LEFTDOCK && COASreticlevisible) {
			SetMeshVisibilityMode(coascdrreticleidx, MESHVIS_VC);
		} else {
			SetMeshVisibilityMode(coascdrreticleidx, MESHVIS_NEVER);
		}
	} else {
		SetMeshVisibilityMode(coascdridx, MESHVIS_NEVER);
		SetMeshVisibilityMode(coascdrreticleidx, MESHVIS_NEVER);
	}
}

void Saturn::SetReentryStage (VECTOR3 cg_ofs)

{
    ClearThrusters();
	ClearPropellants();
	ClearAirfoilDefinitions();
	ClearVariableDragElements();
	ClearEngineIndicators();
	ClearLVGuidLight();
	ClearLVRateLight();
	ClearSIISep();
	agc.SetInputChannelBit(030, LiftOff, false);
	agc.SetInputChannelBit(030, GuidanceReferenceRelease, false);
	agc.SetInputChannelBit(030, UllageThrust, false);

	ShiftCG(-currentCoG + cg_ofs);
	currentCoG = _V(0, 0, 0);

	DefineCMAttachments();

	hga.DeleteAnimations();
	SPSEngine.DeleteAnimations();
	double EmptyMass = CM_EmptyMass + (LESAttached ? 2000.0 : 0.0);
	SetSize(6.0);
	SetEmptyMass(EmptyMass);

	double td_mass = 5430.0;
	double td_width = 2.0;
	double td_tdph = -2.5;
	if (ApexCoverAttached) {
		td_tdph = -1.3;
	}
	double td_height = 5.0;

	ConfigTouchdownPoints(td_mass, td_width, td_tdph, td_height);

	if (LESAttached)
	{
		SetPMI(_V(15.0, 15.0, 1.5));
		SetRotDrag(_V(1.5, 1.5, 0.003));
	}
	else
	{
		SetPMI(_V(1.25411, 1.11318, 1.41524)); //Calculated from CSM-109 Mass Properties at CM/SM Separation
		SetRotDrag(_V(0.07, 0.07, 0.002));
	}
	SetCrossSections (_V(9.17,7.13,7.0));
	SetCW(1.5, 1.5, 1.2, 1.2);
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1) {
		if (LESAttached)
		{
			if (canard.IsDeployed())
			{
				CMLETCanardAirfoilConfig();
			}
			else
			{
				CMLETAirfoilConfig();
			}
		}
		else
		{
			//TBD: make CG variable. 1141.25 inches is reference for moment coefficients, 1040.9 is current hardcoded CG
			CreateAirfoil3(LIFT_VERTICAL, _V(0.0, 5.8*0.0254, (1141.25 - 1040.9)*0.0254), CMVertCoeffFunc, NULL, 154.0*0.0254, 129.4*0.3048*0.3048, 1.0);
			CreateAirfoil3(LIFT_HORIZONTAL, _V(0.0, 5.8*0.0254, (1141.25 - 1040.9)*0.0254), CMHorizCoeffFunc, NULL, 154.0*0.0254, 129.4*0.3048*0.3048, 1.0);
		}
    }

	SetReentryMeshes();
	if (ApexCoverAttached) {
		SetView(-0.15);
	} else {
		SetView(-1.35);
	}
	if (CMTex) SetReentryTexture(CMTex, 1e6, 5, 0.7);

	// CM RCS
	double CGOffset = 34.4;
	if (ApexCoverAttached) {
		AddRCS_CM(CM_RCS_THRUST);
	} else {
		AddRCS_CM(CM_RCS_THRUST, -1.2);
		CGOffset += 1.2;
	}

	if (LESAttached) {
		//if (!ph_tjm)
		//	ph_tjm  = CreatePropellantResource(93.318);
		if (!ph_lem)
			ph_lem = CreatePropellantResource(1425.138);
		if (!ph_pcm)
			ph_pcm = CreatePropellantResource(4.07247);

		SetDefaultPropellantResource (ph_lem); // display LEM propellant level in generic HUD

		//
		// *********************** thruster definitions ********************************
		//

		VECTOR3 m_exhaust_pos1 = _V(0.0, -0.5, TowerOffset-2.2);
		VECTOR3 m_exhaust_pos2 = _V(0.0, 0.5, TowerOffset-2.2);
		VECTOR3 m_exhaust_pos3 = _V(-0.5, 0.0, TowerOffset-2.2);
		VECTOR3 m_exhaust_pos4 = _V(0.5, 0.0, TowerOffset-2.2);

		//
		// Main thrusters.
		//

		th_lem[0] = CreateThruster (m_exhaust_pos1, _V(0.0, sin(35.0*RAD), cos(35.0*RAD)), THRUST_VAC_LEM, ph_lem, ISP_LEM_VAC, ISP_LEM_SL);
		th_lem[1] = CreateThruster (m_exhaust_pos2, _V(0.0, -sin(35.0*RAD), cos(35.0*RAD)), THRUST_VAC_LEM, ph_lem, ISP_LEM_VAC, ISP_LEM_SL);
		th_lem[2] = CreateThruster (m_exhaust_pos3, _V(sin(35.0*RAD), 0.0, cos(35.0*RAD)), THRUST_VAC_LEM, ph_lem, ISP_LEM_VAC, ISP_LEM_SL);
		th_lem[3] = CreateThruster (m_exhaust_pos4, _V(-sin(35.0*RAD), 0.0, cos(35.0*RAD)), THRUST_VAC_LEM, ph_lem, ISP_LEM_VAC, ISP_LEM_SL);

		//th_tjm[0] = CreateThruster(_V(0.0, -0.5, TowerOffset), _V(0.030524, 0.49907, 0.8660254), THRUST_VAC_TJM, ph_tjm, ISP_TJM_VAC, ISP_TJM_SL);
		//th_tjm[1] = CreateThruster(_V(0.0, 0.5, TowerOffset), _V(0.030524, -0.49907, 0.8660254), THRUST_VAC_TJM, ph_tjm, ISP_TJM_VAC, ISP_TJM_SL);

		th_pcm = CreateThruster(_V(0.0, 0.0, TowerOffset + 4.5), _V(0.0, 1.0, 0.0), THRUST_VAC_PCM, ph_pcm, ISP_PCM_VAC, ISP_PCM_SL);

		//
		// Add exhausts
		//

		int i;
		for (i = 0; i < 4; i++)
		{
			AddExhaust (th_lem[i], 8.0, 0.5, SIVBRCSTex);
			AddExhaustStream (th_lem[i], &lem_exhaust);
		}
		//for (i = 0; i < 2; i++)
		//{
		//	AddExhaust(th_tjm[i], 8.0, 0.5, SIVBRCSTex);
		//	AddExhaustStream(th_tjm[i], &lem_exhaust);
		//}
		AddExhaust(th_pcm, 8.0, 0.5, SIVBRCSTex);
		AddExhaustStream(th_pcm, &lem_exhaust);

		thg_lem = CreateThrusterGroup (th_lem, 4, THGROUP_USER);
		//thg_tjm = CreateThrusterGroup(th_tjm, 2, THGROUP_USER);
	}

	VECTOR3 dockpos = {0, 0, 1.5};
	VECTOR3 dockdir = {0, 0, 1};
	VECTOR3 dockrot = {0, 1, 0};
	SetDockParams(dockpos, dockdir, dockrot);

	if (!DrogueS.isValid())
		soundlib.LoadMissionSound(DrogueS, DROGUES_SOUND);

	//
	// Waste dump streams
	//

	SetWaterDumpParticleStreams(_V(0, 0, CGOffset));
}

void Saturn::SetReentryMeshes() {

	ClearMeshes();

	UINT meshidx;
	VECTOR3 mesh_dir=_V(0,0,0);
	if (Burned)	{
		if (ApexCoverAttached) {
			meshidx = AddMesh (hCMB, &mesh_dir);
		} else {
			mesh_dir=_V(0, 0, -1.2);
			meshidx = AddMesh (hCM2B, &mesh_dir);
		}
	} else {
		if (ApexCoverAttached) {
			meshidx = AddMesh (hCMnh, &mesh_dir);
		} else {
			mesh_dir=_V(0, 0, -1.2);
			meshidx = AddMesh (hCM2, &mesh_dir);
		}
	}
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	if (LESAttached) {
		TowerOffset = 4.95;
		VECTOR3 mesh_dir_tower = mesh_dir + _V(0, 0, TowerOffset);

		meshidx = AddMesh (hsat5tower, &mesh_dir_tower);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	// And the Crew
	if (Crewed) {		
		cmpidx = AddMesh (hCMP, &mesh_dir);
		crewidx = AddMesh (hCREW, &mesh_dir);
		SetCrewMesh();
	} else {
		cmpidx = -1;
		crewidx = -1;
	}
	
	//CM docking target
	VECTOR3 dt_dir = _V(0.66, 1.07, 0);
	cmdocktgtidx = AddMesh(hcmdocktgt, &dt_dir);
	SetCMdocktgtMesh();

	// Hatch
	sidehatchidx = AddMesh (hFHC, &mesh_dir);
	sidehatchopenidx = AddMesh (hFHO, &mesh_dir);
	sidehatchburnedidx = AddMesh (hFHC2, &mesh_dir);
	sidehatchburnedopenidx = AddMesh (hFHO2, &mesh_dir);
	SetSideHatchMesh();

	//Forward Hatch
	if (ApexCoverAttached) {
		fwdhatchidx = AddMesh(hFHF, &mesh_dir);
		SetFwdHatchMesh();
	}

	//Interior
	meshidx = AddMesh (hCMInt, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_EXTERNAL);

	// VC
	UpdateVC(mesh_dir);
	seatsfoldedidx = AddMesh(hcmseatsfolded, &mesh_dir);
	seatsunfoldedidx = AddMesh(hcmseatsunfolded, &mesh_dir);
	SetVCSeatsMesh();
	windowshadesidx[0] = AddMesh(hcmWindowShades[0], &mesh_dir);
	windowshadesidx[1] = AddMesh(hcmWindowShades[1], &mesh_dir);
	windowshadesidx[2] = AddMesh(hcmWindowShades[2], &mesh_dir);
	windowshadesidx[3] = AddMesh(hcmWindowShades[3], &mesh_dir);
	windowshadesidx[4] = AddMesh(hcmWindowShades[4], &mesh_dir);
	SetWindowShades();
	coascdrreticleidx = AddMesh(hcmCOAScdrreticle, &mesh_dir);
	coascdridx = AddMesh(hcmCOAScdr, &mesh_dir);
	SetCOASMesh();

	//Add CM meshes. More to be added here...
	AddCMMeshes(mesh_dir);
}

void Saturn::StageSeven(double simt)

{
	if (!Crewed)
	{
		switch (StageState) {
		case 0:
			if (GetAltitude() < 350000) {
				SlowIfDesired();
				ActivateNavmode(NAVMODE_RETROGRADE);
				StageState++;
			}
			break;
		}
	}

	// Entry heat according to Orbiter reference manual
	double entryHeat = 0.5 * GetAtmDensity() * pow(GetAirspeed(), 3);
	if (entryHeat > 2e7 ) { // We 're looking wether the CM has burned or not
		Burned = true;
		SetReentryMeshes();

		ClearThrusters();
		AddRCS_CM(CM_RCS_THRUST);

		SetStage(CM_ENTRY_STAGE);
		SetView(-0.15);
	}
}

void Saturn::StageEight(double simt)

{
	ConfigTouchdownPoints(CM_EmptyMass, 2.0, -2.5, 5.0);

	// Mark apex as detached
	ApexCoverAttached = false;
	SetReentryMeshes();

	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);

	DefineCMAttachments();

	SetView(-1.35);

	if (!Crewed)
	{
		switch (StageState) {
		case 0:
			if (GetAltitude() < 50000) {
				SlowIfDesired();
				DeactivateNavmode(NAVMODE_RETROGRADE);
				StageState++;
			}
			break;
		}
	}

	//
	// Create the apex cover vessel
	//
	VECTOR3 posOffset = _V(0, 0, 0);
	VECTOR3 velOffset = _V(0, 0, 3);

	VESSELSTATUS vs;
	GetStatus(vs);
	Local2Rel(posOffset, vs.rpos);
	VECTOR3 vog;
	GlobalRot(velOffset, vog);
	vs.rvel += vog;

	char VName[256]="";
	GetApolloName(VName);
	strcat(VName, "-APEX");
	if (Burned) {
		hApex = oapiCreateVessel(VName,"ProjectApollo/CMBapex", vs);
	} else {
		hApex = oapiCreateVessel(VName,"ProjectApollo/CMapex", vs);
	}

	// New stage
	SetStage(CM_ENTRY_STAGE_TWO);
}

void Saturn::SetChuteStage1()
{
	SetSize(15);
	SetCOG_elev(2.2);
	SetEmptyMass(CM_EmptyMass);
	ConfigTouchdownPoints(CM_EmptyMass, 2.0, -2.5, 5.0);
	ClearAirfoilDefinitions();
	SetPMI(_V(20,20,12));
	SetCrossSections(_V(2.8,2.8,80.0));
	SetCW(1.0, 1.5, 1.4, 1.4);
	SetRotDrag(_V(0.7,0.7,1.2));
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale(-5e-3);
		SetYawMomentScale(-5e-3);
	}
	SetLiftCoeffFunc(0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	SetReentryMeshes();
	
	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);
	SetView(-1.35);

	DeactivateNavmode(NAVMODE_KILLROT);
}

void Saturn::SetChuteStage2()
{
	SetSize(22);
	SetCOG_elev(2.2);
	SetEmptyMass (CM_EmptyMass);
	ConfigTouchdownPoints(CM_EmptyMass, 2.0, -2.5, 5.0);
	SetPMI (_V(20,20,12));
	SetCrossSections (_V(2.8,2.8,140.0));
	SetCW (1.0, 1.5, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale (-5e-3);
		SetYawMomentScale (-5e-3);
	}
	SetLiftCoeffFunc(0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	SetReentryMeshes();

	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);
	SetView(-1.35);
}

void Saturn::SetChuteStage3()
{
	SetSize(22);
	SetCOG_elev(2.2);
	SetEmptyMass (CM_EmptyMass);
	ConfigTouchdownPoints(CM_EmptyMass, 2.0, -2.5, 5.0);
	SetPMI(_V(20,20,12));
	SetCrossSections(_V(2.8,2.8,480.0));
	SetCW(0.7, 1.5, 1.4, 1.4);
	SetRotDrag(_V(0.7,0.7,1.2));
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale(-5e-3);
		SetYawMomentScale(-5e-3);
	}
	SetLiftCoeffFunc (0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	SetReentryMeshes();

	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);
	SetView(-1.35);
}

void Saturn::SetChuteStage4()
{
	SetSize(22);
	SetCOG_elev(2.2);
	SetEmptyMass(CM_EmptyMass);
	ConfigTouchdownPoints(CM_EmptyMass, 2.0, -2.5, 5.0);
	SetPMI(_V(20,20,12));
	SetCrossSections (_V(2.8,2.8,3280.0));
	SetCW (0.7, 1.5, 1.4, 1.4);
	SetRotDrag(_V(0.7, 0.7, 1.2));
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale (-5e-3);
		SetYawMomentScale (-5e-3);
	}
	SetLiftCoeffFunc(0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	SetReentryMeshes();

	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);
	SetView(-1.35);
}

void Saturn::SetSplashStage()
{
	SetSize(6.0);
	SetCOG_elev(2.2);
	SetEmptyMass(CM_EmptyMass);
	ConfigTouchdownPoints(CM_EmptyMass, 2.0, -2.5, 5.0);
	SetPMI(_V(20,20,12));
	SetCrossSections(_V(2.8,2.8,7.0));
	SetCW(0.5, 1.5, 1.4, 1.4);
	SetRotDrag(_V(0.7,0.7,1.2));
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale (-5e-3);
		SetYawMomentScale (-5e-3);
	}
	SetLiftCoeffFunc(0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	SetReentryMeshes();

	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);

	dyemarker_spec.tex = oapiRegisterParticleTexture("ProjectApollo/Dyemarker");
	if (dyemarker) DelExhaustStream(dyemarker);
	dyemarker = AddParticleStream(&dyemarker_spec, _V(-0.5, 1.5, -2), _V(-0.8660254, 0.5, 0), els.GetDyeMarkerLevelRef());

	SetView(-1.35);
}

void Saturn::SetRecovery()

{
	SetSize(10.0);
	SetCOG_elev(2.2);
	ConfigTouchdownPoints(CM_EmptyMass, 2.0, -2.5, 5.0);
	SetEmptyMass(CM_EmptyMass);
	SetPMI(_V(20,20,12));
	SetCrossSections(_V(2.8,2.8,7.0));
	SetCW(0.5, 1.5, 1.4, 1.4);
	SetRotDrag(_V(0.7,0.7,1.2));
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale (-5e-3);
		SetYawMomentScale (-5e-3);
	}
	SetLiftCoeffFunc(0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	// Meshes
	ClearMeshes();

	UINT meshidx;
	VECTOR3 mesh_dir=_V(0,0,-1.2);
	if (Burned)	{
		meshidx = AddMesh (hCM2B, &mesh_dir);
	} else {
		meshidx = AddMesh (hCM2, &mesh_dir);
	}
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// Hatch
	sidehatchidx = AddMesh (hFHC, &mesh_dir);
	sidehatchopenidx = AddMesh (hFHO, &mesh_dir);
	sidehatchburnedidx = AddMesh (hFHC2, &mesh_dir);
	sidehatchburnedopenidx = AddMesh (hFHO2, &mesh_dir);
	SetSideHatchMesh();

	meshidx = AddMesh (hCMInt, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_EXTERNAL);

	// VC
	UpdateVC(mesh_dir);
	seatsfoldedidx = AddMesh(hcmseatsfolded, &mesh_dir);
	seatsunfoldedidx = AddMesh(hcmseatsunfolded, &mesh_dir);
	SetVCSeatsMesh();
	windowshadesidx[0] = AddMesh(hcmWindowShades[0], &mesh_dir);
	windowshadesidx[1] = AddMesh(hcmWindowShades[1], &mesh_dir);
	windowshadesidx[2] = AddMesh(hcmWindowShades[2], &mesh_dir);
	windowshadesidx[3] = AddMesh(hcmWindowShades[3], &mesh_dir);
	windowshadesidx[4] = AddMesh(hcmWindowShades[4], &mesh_dir);
	SetWindowShades();
	coascdrreticleidx = AddMesh(hcmCOAScdrreticle, &mesh_dir);
	coascdridx = AddMesh(hcmCOAScdr, &mesh_dir);
	SetCOASMesh();

	if (Crewed) {
		mesh_dir =_V(2.7,1.8,-1.5);
		meshidx = AddMesh (hCRB, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);

	dyemarker_spec.tex = oapiRegisterParticleTexture("ProjectApollo/Dyemarker");
	if (dyemarker) DelExhaustStream(dyemarker);
	dyemarker = AddParticleStream(&dyemarker_spec, _V(-0.5, 1.5, -2), _V(-0.8660254, 0.5, 0), els.GetDyeMarkerLevelRef());

	SetView(-1.35);
}

bool Saturn::clbkLoadGenericCockpit ()

{
	TRACESETUP("Saturn::clbkLoadGenericCockpit");

	//
	// VC-only in engineering camera view.
	//

	if (viewpos == SATVIEW_ENG1 || viewpos == SATVIEW_ENG2 || viewpos == SATVIEW_ENG3)
		return false;

	SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
	oapiCameraSetCockpitDir(0,0);
	InVC = false;
	InPanel = false;

	SetView();
	return true;
}

//
// Generic function to jettison the escape tower.
//

void Saturn::JettisonLET(bool AbortJettison)

{		
	//
	// Don't do anything if the tower isn't attached!
	//
	if (!LESAttached || !LESLegsCut)
		return;

	//
	// Otherwise jettison the LES.
	//
	VECTOR3 ofs1 = _V(0.0, 0.0, TowerOffset) - currentCoG;
	VECTOR3 vel1 = _V(0.0,0.0,0.5);

	VESSELSTATUS vs1;
	GetStatus (vs1);

	vs1.eng_main = vs1.eng_hovr = 0.0;

	//
	// We must set status to zero to ensure the LET is in 'free flight'. Otherwise if we jettison
	// on the pad, the LET thinks it's on the ground!
	//

	vs1.status = 0;

	VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};

	Local2Rel (ofs1, vs1.rpos);

	GlobalRot (vel1, rofs1);

	vs1.rvel.x = rvel1.x+rofs1.x;
	vs1.rvel.y = rvel1.y+rofs1.y;
	vs1.rvel.z = rvel1.z+rofs1.z;

	TowerJS.play();
	TowerJS.done();

	char VName[256];

	GetApolloName(VName);
	strcat (VName, "-TWR");

	hesc1 = oapiCreateVessel(VName, "ProjectApollo/LES", vs1);
	LESAttached = false;

	LESSettings LESConfig;

	LESConfig.SettingsType.word = 0;
	LESConfig.SettingsType.LES_SETTINGS_GENERAL = 1;
	LESConfig.SettingsType.LES_SETTINGS_ENGINES = 1;

	LESConfig.FireLEM = FireLEM;
	LESConfig.FireTJM = FireTJM;
	LESConfig.FirePCM = FirePCM;

	LESConfig.LowRes = LowRes;
	LESConfig.ProbeAttached = AbortJettison && HasProbe;

	if (ph_lem)
	{
		LESConfig.LaunchEscapeFuelKg = GetPropellantMass(ph_lem);
		LESConfig.SettingsType.LES_SETTINGS_MFUEL = 1;
	}
	//if (ph_tjm)
	//{
		//LESConfig.JettisonFuelKg = GetPropellantMass(ph_tjm);
		//LESConfig.SettingsType.LES_SETTINGS_MFUEL = 1;
	//}
	if (ph_pcm)
	{
		LESConfig.PitchControlFuelKg = GetPropellantMass(ph_pcm);
		LESConfig.SettingsType.LES_SETTINGS_PFUEL = 1;
	}

	LES *les_vessel = (LES *) oapiGetVesselInterface(hesc1);
	les_vessel->SetState(LESConfig);

	//
	// AOH SECS page 2.9-8 says that in the case of an abort, the docking probe is pulled away
	// from the CM by the LES when it's jettisoned.
	//
	if (AbortJettison)
	{
		dockingprobe.SetEnabled(false);
		HasProbe = false;
	}
	else
	{
		//
		// Enable docking probe because the tower is gone
		//
		dockingprobe.SetEnabled(HasProbe);			
	}

	ConfigureStageMeshes(stage);

	if (Crewed)
	{
		SwindowS.play();
	}
	SwindowS.done();

	//
	// Event management
	//

	if (eventControl.TOWER_JETTISON == MINUS_INFINITY)
		eventControl.TOWER_JETTISON = MissionTime;
}

void Saturn::JettisonDockingProbe() 

{
	char VName[256];

	// Use VC offset to calculate the docking probe offset
	VECTOR3 ofs = _V(0, 0, CurrentViewOffset + 0.25);
	VECTOR3 vel = {0.0, 0.0, 2.5};
	VESSELSTATUS vs4b;
	GetStatus (vs4b);
	StageTransform(this, &vs4b, ofs - currentCoG, vel);
	vs4b.vrot.x = 0.0;
	vs4b.vrot.y = 0.0;
	vs4b.vrot.z = 0.0;
	GetApolloName(VName); 
	strcat (VName, "-DCKPRB");
	hPROBE = oapiCreateVessel(VName, "ProjectApollo/CMprobe", vs4b);
}

void Saturn::JettisonSIMBayPanel()
{
	//TBD: Correct offset and velocity

	//
	// Blow off Panel 1.
	//

	VESSELSTATUS vs1;

	const double CGOffset = 12.25 + 21.5 - 1.8 + 0.35;

	VECTOR3 vel1 = _V(cos(52.25*RAD), sin(52.25*RAD), 0.0)*13.7*0.3048;
	VECTOR3 ofs1 = { 0, 0, 30.25 - CGOffset };

	GetStatus(vs1);

	VECTOR3 rofs1, rvel1 = { vs1.rvel.x, vs1.rvel.y, vs1.rvel.z };

	Local2Rel(ofs1 - currentCoG, vs1.rpos);
	GlobalRot(vel1, rofs1);

	vs1.rvel.x = rvel1.x + rofs1.x;
	vs1.rvel.y = rvel1.y + rofs1.y;
	vs1.rvel.z = rvel1.z + rofs1.z;
	vs1.vrot.x = 0;
	vs1.vrot.y = 0;
	vs1.vrot.z = 0;

	char VName[256];

	GetApolloName(VName);
	strcat(VName, "-PANEL1");

	oapiCreateVessel(VName, "ProjectApollo/SM-Panel1", vs1);
}

void Saturn::JettisonOpticsCover() 

{
	char VName[256];

	// Use VC offset to calculate the optics cover offset
	VECTOR3 ofs = _V(0, 0, CurrentViewOffset + 0.25);
	VECTOR3 vel = {0.0, -0.16, 0.1};
	VESSELSTATUS vs4b;
	GetStatus (vs4b);
	StageTransform(this, &vs4b, ofs - currentCoG, vel);
	vs4b.vrot.x = 0.05;
	vs4b.vrot.y = 0.0;
	vs4b.vrot.z = 0.0;
	GetApolloName(VName); 
	strcat (VName, "-OPTICSCOVER");
	hOpticsCover = oapiCreateVessel(VName, "ProjectApollo/CMOpticsCover", vs4b);
}

void Saturn::DeployCanard()
{
	if (!LESAttached) return;
	if (canard.IsDeployed()) return;

	canard.Deploy();

	CMLETCanardAirfoilConfig();
}

void Saturn::CMLETAirfoilConfig()
{
	ClearAirfoilDefinitions();

	CreateAirfoil(LIFT_VERTICAL, _V(0.0, 0.0, 1.12), CMLETVertCoeffFunc, 3.5, 11.95 / 2.0, 1.0);
	CreateAirfoil(LIFT_HORIZONTAL, _V(0.0, 0.0, 1.12), CMLETHoriCoeffFunc, 3.5, 11.95 / 2.0, 1.0);
}

void Saturn::CMLETCanardAirfoilConfig()
{
	ClearAirfoilDefinitions();

	CreateAirfoil(LIFT_VERTICAL, _V(0.0, 0.0, 1.12), CMLETCanardVertCoeffFunc, 3.5, 11.95 / 2.0, 1.0);
	CreateAirfoil(LIFT_HORIZONTAL, _V(0.0, 0.0, 1.12), CMLETHoriCoeffFunc, 3.5, 11.95 / 2.0, 1.0);
}

void Saturn::ConfigTouchdownPoints()
{
	double td_mass, td_width, td_tdph, td_height;

	switch (stage)
	{
	case CSM_LEM_STAGE:
		td_mass = CM_EmptyMass + SM_EmptyMass + (SM_FuelMass / 2);
		td_width = 4.0;
		td_tdph = -6.0;
		td_height = 5.5;

		ConfigTouchdownPoints(td_mass, td_width, td_tdph, td_height, -0.1);
		break;
	}
}

void Saturn::ConfigTouchdownPoints(double mass, double ro, double tdph, double height, double x_target)
{

	TOUCHDOWNVTX td[4];
	double stiffness = (-1)*(mass*9.80655) / (3 * x_target);
	double damping = 0.9*(2 * sqrt(mass*stiffness));
	for (int i = 0; i < 4; i++) {
		td[i].damping = damping;
		td[i].mu = 3;
		td[i].mu_lng = 3;
		td[i].stiffness = stiffness;
	}
	td[0].pos.x = -cos(30 * RAD)*ro;
	td[0].pos.y = -sin(30 * RAD)*ro;
	td[0].pos.z = tdph;
	td[1].pos.x = 0;
	td[1].pos.y = 1 * ro;
	td[1].pos.z = tdph;
	td[2].pos.x = cos(30 * RAD)*ro;
	td[2].pos.y = -sin(30 * RAD)*ro;
	td[2].pos.z = tdph;
	td[3].pos.x = 0;
	td[3].pos.y = 0;
	td[3].pos.z = tdph + height;

	for (int i = 0;i < 4;i++)
	{
		td[i].pos -= currentCoG;
	}

	SetTouchdownPoints(td, 4);
}

void Saturn::SetWaterDumpParticleStreams(VECTOR3 ofs)
{
	wastewaterdump_spec.tex = oapiRegisterParticleTexture("ProjectApollo/WaterDump");
	if (wastewaterdump) DelExhaustStream(wastewaterdump);
	wastewaterdump = AddParticleStream(&wastewaterdump_spec, _V(-1.258, 1.282, 33.69) - ofs, _V(-0.57, 0.57, 0.59), WaterController.GetWasteWaterDumpLevelRef());

	urinedump_spec.tex = oapiRegisterParticleTexture("ProjectApollo/UrineDump");
	if (urinedump) DelExhaustStream(urinedump);
	urinedump = AddParticleStream(&urinedump_spec, _V(-1.358, 1.192, 33.69) - ofs, _V(-0.57, 0.57, 0.59), WaterController.GetUrineDumpLevelRef());
}

void Saturn::LoadVC()
{
	vcidx = AddMesh(hCMVC);
	SetMeshVisibilityMode(vcidx, MESHVIS_VC);
	DefineVCAnimations();
}

void Saturn::UpdateVC(VECTOR3 meshdir)
{
	if (vcidx == -1)
		return;

	VECTOR3 ofs;
	GetMeshOffset(vcidx, ofs);
	ShiftMesh(vcidx, meshdir - ofs);
}

void Saturn::ClearMeshes() {
	// Clear all meshes EXCEPT the VC mesh (idx = 0) in order to not screw up the VC animations when a ClearMeshes() is called i.e. staging
	// This should not be needed once a better way to handle staging is implemented (docked stages)
	int meshcount = GetMeshCount();

	if (buildstatus < 6) {
		for (int i = 0; i < meshcount; i++)
		{
			DelMesh(i);
		}
	} else {
		for (int i = 1; i < meshcount; i++)
		{
			DelMesh(i);
		}
	}

	ResetDynamicMeshIndizes();
}

void Saturn::DefineCMAttachments()
{
	//Reset attachment points
	ATTACHMENTHANDLE ah = GetAttachmentHandle(false, 0);
	SetAttachmentParams(ah, _V(0, 0, 0), _V(0, 0, 1), _V(1, 0, 0)); //FloatBag
	ah = GetAttachmentHandle(false, 1);
	SetAttachmentParams(ah, _V(0, 0, 0), _V(0, 0, 1), _V(1, 0, 0)); //Chute
}

void Saturn::ResetDynamicMeshIndizes()
{
	CueCards.ResetMeshIndizes();
}

void Saturn::AddCMMeshes(const VECTOR3 &mesh_dir)
{
	// Docking probe
	if (HasProbe) {
		dockringidx = AddMesh(hdockring, &mesh_dir);
		probeidx = AddMesh(hprobe, &mesh_dir);
		probeextidx = AddMesh(hprobeext, &mesh_dir);
		SetDockingProbeMesh();
	}
	else {
		dockringidx = -1;
		probeidx = -1;
		probeextidx = -1;
	}

	// Optics Cover
	opticscoveridx = AddMesh(hopticscover, &mesh_dir);
	SetOpticsCoverMesh();

	//Reload cue cards, if required
	CueCards.ResetCueCards();
}