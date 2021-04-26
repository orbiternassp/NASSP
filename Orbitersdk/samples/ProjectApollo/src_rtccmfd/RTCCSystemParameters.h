/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC System Parameters

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

struct RTCCDensityTables
{
	RTCCDensityTables()
	{
		r0 = 6.373338e6; //Adjusted for Orbiter
		C2 = 401.8743008658905; //1.4*8.31432/(28.9644/1000)
		CSMAX = 269.4412597;

		H_b[0] = 79000.0;
		H_b[1] = 61000.0;
		H_b[2] = 52000.0;
		H_b[3] = 47000.0;
		H_b[4] = 32000.0;
		H_b[5] = 20000.0;
		H_b[6] = 11000.0;
		H_b[7] = 0.0;

		ln_rho_b[0] = -10.819278;
		ln_rho_b[1] = -8.289699;
		ln_rho_b[2] = -7.182942;
		ln_rho_b[3] = -6.551831;
		ln_rho_b[4] = -4.325646;
		ln_rho_b[5] = -2.430021;
		ln_rho_b[6] = -1.010821;
		ln_rho_b[7] = 0.202941;

		L_M_B[0] = 0.0;
		L_M_B[1] = -4.0 / 1000.0;
		L_M_B[2] = -2.0 / 1000.0;
		L_M_B[3] = 0.0;
		L_M_B[4] = 2.8 / 1000.0;
		L_M_B[5] = 1.0 / 1000.0;
		L_M_B[6] = 0.0;
		L_M_B[7] = -6.5 / 1000.0;

		T_M_B[0] = 180.65;
		T_M_B[1] = 252.65;
		T_M_B[2] = 270.65;
		T_M_B[3] = 270.65;
		T_M_B[4] = 228.65;
		T_M_B[5] = 216.65;
		T_M_B[6] = 216.65;
		T_M_B[7] = 288.15;

		A[0] = 0.000000e+00;
		A[1] = -7.540799e+00;
		A[2] = -1.608160e+01;
		A[3] = 0.000000e+00;
		A[4] = 1.320114e+01;
		A[5] = 3.516319e+01;
		A[6] = 0.000000e+00;
		A[7] = -4.255876e+00;

		B[0] = 1.891126e-04;
		B[1] = -1.583218e-05;
		B[2] = -7.389618e-06;
		B[3] = 1.262265e-04;
		B[4] = 1.224579e-05;
		B[5] = 4.615740e-06;
		B[6] = 1.576884e-04;
		B[7] = -2.255770e-05;

		r_b[0] = r0 + 600.0*1000.0;
		r_b[1] = r0 + 500.0*1000.0;
		r_b[2] = r0 + 400.0*1000.0;
		r_b[3] = r0 + 300.0*1000.0;
		r_b[4] = r0 + 230.0*1000.0;
		r_b[5] = r0 + 190.0*1000.0;
		r_b[6] = r0 + 170.0*1000.0;
		r_b[7] = r0 + 160.0*1000.0;
		r_b[8] = r0 + 150.0*1000.0;
		r_b[9] = r0 + 120.0*1000.0;
		r_b[10] = r0 + 110.0*1000.0;
		r_b[11] = r0 + 100.0*1000.0;
		r_b[12] = r0 + 90.0*1000.0;

		L_M[0] = 1.1 / 1000.0;
		L_M[1] = 1.7 / 1000.0;
		L_M[2] = 2.6 / 1000.0;
		L_M[3] = 3.3 / 1000.0;
		L_M[4] = 4.0 / 1000.0;
		L_M[5] = 5.0 / 1000.0;
		L_M[6] = 7.0 / 1000.0;
		L_M[7] = 10.0 / 1000.0;
		L_M[8] = 15.0 / 1000.0;
		L_M[9] = 20.0 / 1000.0;
		L_M[10] = 10.0 / 1000.0;
		L_M[11] = 5.0 / 1000.0;
		L_M[12] = 3.0 / 1000.0;

		T_M_b[0] = 2590.65;
		T_M_b[1] = 2420.65;
		T_M_b[2] = 2160.65;
		T_M_b[3] = 1830.65;
		T_M_b[4] = 1550.65;
		T_M_b[5] = 1350.65;
		T_M_b[6] = 1210.65;
		T_M_b[7] = 1110.65;
		T_M_b[8] = 960.65;
		T_M_b[9] = 360.65;
		T_M_b[10] = 260.65;
		T_M_b[11] = 210.65;
		T_M_b[12] = 180.65;

		A7[0] = 1.509968e+00;
		A7[1] = 1.261296e+00;
		A7[2] = 1.139848e+00;
		A7[3] = 1.090665e+00;
		A7[4] = 1.062369e+00;
		A7[5] = 1.042924e+00;
		A7[6] = 1.027149e+00;
		A7[7] = 1.017294e+00;
		A7[8] = 1.009915e+00;
		A7[9] = 1.002785e+00;
		A7[10] = 1.004037e+00;
		A7[11] = 1.006551e+00;
		A7[12] = 1.009404e+00;

		B7[0] = 3.917293e+01;
		B7[1] = 2.179340e+01;
		B7[2] = 1.326051e+01;
		B7[3] = 1.029873e+01;
		B7[4] = 8.452409e+00;
		B7[5] = 6.719323e+00;
		B7[6] = 4.755860e+00;
		B7[7] = 3.307261e+00;
		B7[8] = 2.195564e+00;
		B7[9] = 1.650191e+00;
		B7[10] = 3.314703e+00;
		B7[11] = 6.666556e+00;
		B7[12] = 1.117693e+01;

		ln_rho_b_T_M_b[0] = -20.539228;
		ln_rho_b_T_M_b[1] = -19.383705;
		ln_rho_b_T_M_b[2] = -18.081362;
		ln_rho_b_T_M_b[3] = -16.539251;
		ln_rho_b_T_M_b[4] = -15.232175;
		ln_rho_b_T_M_b[5] = -14.348024;
		ln_rho_b_T_M_b[6] = -13.843007;
		ln_rho_b_T_M_b[7] = -13.563008;
		ln_rho_b_T_M_b[8] = -13.248066;
		ln_rho_b_T_M_b[9] = -11.642415;
		ln_rho_b_T_M_b[10] = -10.572165;
		ln_rho_b_T_M_b[11] = -9.163673;
		ln_rho_b_T_M_b[12] = -7.465218;
	}

	//Radius of Earth at latitude 45° 33' 33''
	double r0;
	//K*R/M0
	double C2;
	//Speed of sound at 90km
	double CSMAX;

	//Tables for 90km and below
	//Base altitude
	double H_b[8];
	//Molecular scale temperature
	double T_M_B[8];
	double ln_rho_b[8];
	double L_M_B[8];
	double A[8];
	double B[8];

	//Tables for 90km and above
	//Base radius
	double r_b[13];
	//Gradient of the molecular-scale temperature in degrees Kelvin per meter
	double L_M[13];
	//Molecular scale temperature in Kelvon
	double T_M_b[13];
	//Coefficents
	double A7[13], B7[13], ln_rho_b_T_M_b[13];
};

struct RTCCSystemParameters
{
	RTCCSystemParameters()
	{
		//DEFAULT VALUES

		//Epoch of NBY 1969 (Apollo 7-10)
		AGCEpoch = 40221.525;

		//These are calculated with R_E = 6.373338e6
		MCGMUM = 2.454405845045305e-01;
		MCEMUU = 19.95468740240253;
		MCSRMU = 4.954196044814240e-01;
		MCERMU = 4.467066979842873;

		//Time from launch to EOI, seconds
		MDLIEV[0] = 0.76673814e3;
		MDLIEV[1] = -0.18916781e1;
		MDLIEV[2] = 0.10202785e-1;
		MDLIEV[3] = 0.16441395e-5;
		//Declination of EOI, deg
		MDLIEV[4] = 0.60994221e2;
		MDLIEV[5] = -0.36710496e0;
		MDLIEV[6] = -0.68781953e-3;
		MDLIEV[7] = 0.44783353e-5;
		//Longitude of EOI, deg
		MDLIEV[8] = -0.7232328e2;
		MDLIEV[9] = 0.55465072e0;
		MDLIEV[10] = -0.48610649e-2;
		MDLIEV[11] = 0.90988410e-5;
		//Inertial azimuth of EOI, deg
		MDLIEV[12] = 0.12344699e2;
		MDLIEV[13] = 0.13104156e1;
		MDLIEV[14] = -0.46697509e-2;
		MDLIEV[15] = 0.14584433e-4;
		//Radius of EOI in NM
		MDLEIC[0] = 3.5439336e3;
		//Flight path angle of EOI in deg
		MDLEIC[1] = 0.0;
		//Inertial velocity of EOI, fps
		MDLEIC[2] = 25567.72868;

		MCVLMC = 0.0;
		MCVCMC = 0.5;
		MCCNMC = 3443.93359;
		MCSMLR = 1738090.0;
		MCCRPD = 0.01745329251; // PI/180
		MCEBAS = 1.0;
		MCEASQ = 4.0619437e13;
		MCEBSQ = 4.0619437e13;
		MCERTS = 0.26251614528012;
		MCGHZA = 0.55326937288; //31.7°
		MCGSMA = 1.5;
		MCGECC = 0.5;
		MGGPHS = 0;
		MDVCCC[0] = 0.0;
		MDVCCC[1] = 0.0;
		MDVACC[0] = 0.0;
		MDVACC[1] = 0.0;
		MDVDCC[0] = 0.0;
		MDVDCC[1] = 0.0;
		MCGVNT = 2.0 / 3600.0;
		MCGVEN = 15.0 / 60.0;
		for (int i = 0;i < 2;i++)
		{
			for (int j = 0;j < 10;j++)
			{
				MDTVTV[i][j] = 0.0;
			}
		}
		MCTVSP = 1000.0;
		MCLABN = MCLCBN = MCLSBN = 0.0;

		//Apollo 15 RTCC Report
		MHACLD[0] = 29.5; MHACLD[1] = 10.0; MHACLD[2] = 4.0; MHACLD[3] = 3.0; MHACLD[4] = 2.4; MHACLD[5] = 2.0; MHACLD[6] = 1.65; MHACLD[7] = 1.35;
		MHACLD[8] = 1.2; MHACLD[9] = 1.1; MHACLD[10] = 0.9; MHACLD[11] = 0.7; MHACLD[12] = 0.4; MHACLD[13] = 0.2; MHACLD[14] = 0.0;

		MHACLD[25] = 1.30622; MHACLD[26] = 1.24360; MHACLD[27] = 1.23308; MHACLD[28] = 1.24174; MHACLD[29] = 1.26414; MHACLD[30] = 1.29352; MHACLD[31] = 1.27779; MHACLD[32] = 1.28926;
		MHACLD[33] = 1.16740; MHACLD[34] = 1.18187; MHACLD[35] = 1.07058; MHACLD[36] = 0.98990; MHACLD[37] = 0.85380; MHACLD[38] = 0.82403; MHACLD[39] = 0.0;

		MHACLD[50] = 0.37544; MHACLD[51] = 0.41763; MHACLD[52] = 0.43006; MHACLD[53] = 0.46687; MHACLD[54] = 0.49523; MHACLD[55] = 0.52171; MHACLD[56] = 0.53170; MHACLD[57] = 0.54654;
		MHACLD[58] = 0.46397; MHACLD[59] = 0.47753; MHACLD[60] = 0.31096; MHACLD[61] = 0.25664; MHACLD[62] = 0.24004; MHACLD[63] = 0.23952; MHACLD[64] = 0.0;

		MHACLD[75] = 160.96*RAD; MHACLD[76] = 157.73*RAD; MHACLD[77] = 157.12*RAD; MHACLD[78] = 155.34*RAD; MHACLD[79] = 154.87*RAD; MHACLD[80] = 154.29*RAD; MHACLD[81] = 154.19*RAD; MHACLD[82] = 154.89*RAD;
		MHACLD[83] = 156.06*RAD; MHACLD[84] = 155.9*RAD; MHACLD[85] = 162.38*RAD; MHACLD[86] = 164.97*RAD; MHACLD[87] = 167.36*RAD; MHACLD[88] = 170.96*RAD; MHACLD[89] = 0.0;

		for (int i = 0;i < 103;i++)
		{
			MHALLD[i] = MHACLD[i];
		}
		MCADRG = 0.1; //Should be 2.0

		MCTJD1 = 570.0;
		MCTJD3 = 2.5;
		MCTJDS = 1.0;
		MCTJD5 = 99999.0;
		MCTJD6 = 1.8;

		MCTJT1 = 38.8 * 4.4482216152605;
		MCTJT2 = 5889.0 * 4.4482216152605;
		MCTJT3 = 110900.0 * 4.4482216152605;
		MCTJT4 = 178147.0 * 4.4482216152605;
		MCTJTL = 202097.0 * 4.4482216152605;
		MCTJTH = 202256.0 * 4.4482216152605;
		MCTJT5 = 202256.0 * 4.4482216152605;
		MCTJT6 = 27784.0 * 4.4482216152605;

		MCTJW1 = 0.111 * 0.45359237;
		MCTJW2 = 0.75 * 0.45359237;
		MCTJW3 = 157.6 * 0.45359237;
		MCTJW4 = 412.167 * 0.45359237;
		MCTJWL = 472.121 * 0.45359237;
		MCTJWH = 472.18 * 0.45359237;
		MCTJW5 = 472.18* 0.45359237;
		MCTJW6 = 61.7 * 0.45359237;

		MCTSAV = 202097.0 * 4.4482216152605;
		MCTWAV = 472.121 * 0.45359237;

		MCVIGM = 584.0;
		MCVWMN = 10000.0; //Minimum S-IVB weight
		MCVKPC = 0.0;
		MCVCPQ = 10.0;
		MCVEP1 = 1000.0;
		MCVEP2 = 30.0;
		MCVEP3 = 30.0;
		MCVEP4 = 3.0;
		MCVYMX = 45.0*RAD;
		MCVPDL = 1.0*RAD;
		MCVYDL = 1.0*RAD;
		MCVTGQ = 300.0;
		MCVRQV = -0.4;
		MCVRQT = 1.0;
		MCTIND = 0;
		MCVVX3 = 4198.1678;
		MCVWD3 = 472.159 * 0.45359237;
		MCVTB2 = 1.0;
		MCVDTM = 2.0;
		MCCRAM = 0.5;

		MCTCT1 = 441.5*2.0;
		MCTCT2 = 441.5*4.0;
		MCTCT3 = 441.5*2.0;
		MCTCT4 = 441.5*4.0;
		MCTCT5 = 874.37837;
		MCTCT6 = 874.37837*2.0;
		MCTCW1 = 0.31038;
		MCTCW2 = 0.62076;
		MCTCW3 = 0.31038;
		MCTCW4 = 0.62076;
		MCTLT1 = 441.5*2.0;
		MCTLT2 = 441.5*4.0;
		MCTLT3 = 441.5*2.0;
		MCTLT4 = 441.5*4.0;
		MCTLT5 = 889.6;
		MCTLT6 = 1779.2;
		MCTLW1 = 0.31038;
		MCTLW2 = 0.62076;
		MCTLW3 = 0.31038;
		MCTLW4 = 0.62076;
		MCTAT1 = 15297.43;
		MCTAT2 = 693.0329277;
		MCTAT4 = 15297.43;
		MCTAT9 = 15569.0;
		MCTAW1 = 5.049776716;
		MCTAW2 = 0.2290767466;
		MCTAW4 = 5.049776716;
		MCTAD2 = 0.308;
		MCTAD3 = 0.192;
		MCTAD9 = 0.5;
		MCTDT1 = 9712.5 * 4.4482216152605;
		MCTDT2 = 1.0 * 4.4482216152605;
		MCTDT3 = 467.7 * 4.4482216152605;
		MCTDT4 = 1180.0 * 4.4482216152605;
		MCTDT5 = 4326.5 * 4.4482216152605;
		MCTDT6 = 10500.0 * 4.4482216152605;
		MCTDT9 = 43670.0;
		MCTDW1 = 14.63626597;
		MCTDW2 = 0.000126;
		MCTDW3 = 0.7048;
		MCTDW4 = 1.778203048;
		MCTDW5 = 6.519823328;
		MCTDW6 = 15.82299024;
		MCTDD2 = 0.5;
		MCTDD3 = 3.1;
		MCTDD4 = 26.0;
		MCTDD5 = 0.4;
		MCTDD6 = 95.0;
		MCTDD9 = 0.5;
		MCTST1 = 91188.544;
		MCTST2 = 424.015 * 4.4482216152605;
		MCTST4 = 91188.544;
		MCTST9 = 91188.544;
		MCTSW1 = 29.60667013;
		MCTSW2 = 0.6107646462;
		MCTSW4 = 29.60667013;
		MCTSD2 = 0.0;
		MCTSD3 = 0.0;
		MCTSD9 = 1.0;
		MCTAK1 = 12455.0;
		MCTAK2 = 31138.0;
		MCTAK3 = 15569.0;
		MCTAK4 = 6181.0;
		MCTDTF = 0.925;

		MCTSPP = -2.15*RAD;
		MCTSYP = 0.95*RAD;
		MCTSPG = 0.0;
		MCTSYG = 0.0;
		MCTAPG = 0.0;
		MCTARG = 0.0;
		MCTDPG = 0.0;
		MCTDRG = 0.0;
	}

	//DEFINITIONS

	//Nautical miles per Earth radii
	double MCCNMC;
	//MJD of epoch
	double AGCEpoch;
	//Mean lunar radius
	double MCSMLR;
	//Radians per degree
	double MCCRPD;
	//Geodetic Earth constant B²/A²
	double MCEBAS;
	//Geodetic Earth constant A²
	double MCEASQ;
	//Geodetic Earth constant B²
	double MCEBSQ;
	//Rotational rate of the Earth (radians/hr.)
	double MCERTS;
	//Burnout launch azimuth (rad.)
	double MCLABN;
	//Sine of burnout launch azimuth
	double MCLSBN;
	//Cosine of burnout launch azimuth
	double MCLCBN;
	//Pitch angle from horizon (rad.)
	double MCGHZA;
	//Semimajor axis for integration limit (Er.)
	double MCGSMA;
	//Eccentricity for integration limit
	double MCGECC;
	//Phase reference time - GET (hrs.)
	double MCGREF = 0.0;
	//Venting scale factor
	double MCTVEN = 0.0;
	//Lambda Zero
	double MCLAMD = 0.0;
	//P80 first launch vehicle
	std::string MCGPRM;
	//L/O time first vehicle (hrs.)
	double MCGMTL = 0.0;
	//L/O time second vehicle (hrs.)
	double MCGMTS = 0.0;
	//L/O time first vehicle (centisec.)
	double MGLGMT = 0.0;
	//L/O time second vehicle (centisec.)
	double MGGGMT = 0.0;
	//CSM GMTGRR (hrs.)
	double MCGRAG = 0.0;
	//IU1 GMTGRR (hrs.)
	double MCGRIC = 0.0;
	//IU2 GMTGRR (hrs.)
	double MCGRIL = 0.0;
	//AGC GMTZS (hrs.)
	double MCGZSA = 0.0;
	//LGC GMTZS (hrs.)
	double MCGZSL = 0.0;
	//AGS GMTZS (hrs.)
	double MCGZSS = 0.0;
	//MJD of launch day (days)
	double GMTBASE = 0.0;
	//Number of hours from January 0 to midnight before launch
	double MCCBES = 0.0;
	//Sine of the geodetic latitude of the launch pad
	double MCLSDA = sin(28.608202*RAD); //LC-39A
	//Cosine of the geodetic latitude of the launch pad
	double MCLCDA = cos(28.608202*RAD); //LC-39A
	//Longitude of launch pad
	double MCLGRA = -80.604133*RAD;	//LC-39A
	//CMC address for external DV uplink
	int MCCCEX = 3404;
	//LGC address for external DV uplink
	int MCCLEX = 3433;
	//CMC address for REFSMMAT uplink (and downlink)
	int MCCCRF = 1735;
	int MCCCRF_DL = 01735;
	//CMC address for desired REFSMMAT uplink
	int MCCCXS = 306;
	//LGC address for REFSMMAT uplink (and downlink)
	int MCCLRF = 1733;
	int MCCLRF_DL = 01733;
	//LGC address for desired REFSMMAT uplink
	int MCCLXS = 3606;
	//CMC address for landing site uplink
	int MCCRLS = 2025;
	//LGC address for landing site uplink
	int MCLRLS = 2022;
	//LGC address for time of landing
	int MCLTTD = 2400;
	//Suppress C-band station contacts generation (0 = suppressed, 1 = unsuppressed)
	int MGRTAG = 1;
	//Gravitational constant of the Moon (Er^3/hr^2)
	double MCGMUM;
	//Gravitational constant of the Earth (Er^3/hr^2)
	double MCEMUU;
	//Square root of gravitational constant of the Moon (Er^3/hr^2)^1/2
	double MCSRMU;
	//Square root of gravitational constant of the Earth (Er^3/hr^2)^1/2
	double MCERMU;
	//Polynomial coefficients for insertion conditions
	double MDLIEV[16];
	//Earth orbit insertion constants
	double MDLEIC[3];
	//Nominal LM cross-product steering constant
	double MCVLMC;
	//Nominal CSM cross-product steering constant
	double MCVCMC;
	//Phase indicator system parameter
	// 0 = No Phase, 1 = Prelaunch
	// 2, 3 = Condition for Launch and Condition for Launch Sim?
	// 13 = Orbit (likely, but what about lunar orbit)
	// 17 = Lunar Descent (?)
	// 18 = Translunar (likely)
	// 20 = Lunar Stay
	// 26 = Transearth(likely)
	// 14, 19, 21, 27 must be high speed
	int MGGPHS;
	//Linear weight loss coefficients for CSM
	double MDVCCC[2];
	//Linear weight loss coefficients for LM ascent
	double MDVACC[2];
	//Linear weight loss coefficients for LM descent
	double MDVDCC[2];

	//DT from end of TLI maneuver to the end of venting weight loss (hrs.)
	double MCGVNT;
	//Reference GET for MDTVTV (Venting thrust table) (hrs.)
	double MCGVEN;
	//Venting thrust table
	double MDTVTV[2][10];
	//Venting specific impulse
	double MCTVSP;
	//Density tables
	RTCCDensityTables MHGDEN;
	//Lift/Drag coefficients (Words: 1-25 Mach Number, 26-50 Coefficients of drag, 51-75 Coefficients of lift, 76-100: Trim angle, 101 DX, 102 DZ, 103 DD)
	double MHACLD[103], MHALLD[103];
	//Coefficient of drag for Earth orbit
	double MCADRG;

	//Phase 1 burn time for S-IVB TLI maneuver (vent and ullage)
	double MCTJD1;
	//Phase 3 burn time for S-IVB TLI maneuver (buildup)
	double MCTJD3;
	//Phase 4 burn time for S-IVB TLI maneuver (main burn)
	double MCTJD4;
	//Phase 5 burn time for S-IVB TLI maneuver (during MRS)
	double MCTJDS;
	//Phase 6 burn time for S-IVB TLI maneuver (main burn after MRS)
	double MCTJD5;
	//Phase 7 burn time for S-IVB TLI maneuver (tailoff)
	double MCTJD6;
	//Thrust level of first S-IVB thrust phase (vent and ullage)
	double MCTJT1;
	//Thrust level of second S-IVB thrust phase (chilldown)
	double MCTJT2;
	//Thrust level of third S-IVB thrust phase (buildup)
	double MCTJT3;
	//Thrust level of third S-IVB thrust phase (pre MRS)
	double MCTJT4;
	//Thrust level of fifth S-IVB thrust phase (high)
	double MCTJTH;
	//Thrust level of fifth S-IVB thrust phase (low)
	double MCTJTL;
	//Thrust level of sixth S-IVB thrust phase (main burn after MRS)
	double MCTJT5;
	//Thrust level of sevent S-IVB thrust phase (tailoff)
	double MCTJT6;
	//Weight flow rate for first S-IVB thrust phase
	double MCTJW1;
	//Weight flow rate for second S-IVB thrust phase
	double MCTJW2;
	//Weight flow rate for third S-IVB thrust phase
	double MCTJW3;
	//Weight flow rate for fourth S-IVB thrust phase
	double MCTJW4;
	//Weight flow rate for fifth S-IVB thrust phase (high)
	double MCTJWH;
	//Weight flow rate for fifth S-IVB thrust phase (low)
	double MCTJWL;
	//Weight flow rate for sixth S-IVB thrust phase
	double MCTJW5;
	//Weight flow rate for seventh S-IVB thrust phase
	double MCTJW6;
	//Time from ignition to start IGM guidance
	double MCVIGM;
	//Minimum allowable S-IVB weight
	double MCVWMN;
	//Fixed time increment to force MRS to occur
	double MCVKPC;
	//Duration of artificial tau for beginning of burn
	double MCVCPQ;
	//Epsilon to control range angle computation
	double MCVEP1;
	//Epsilon to terminate computation of K1, K2, K3, and K4
	double MCVEP2;
	//Epsilon to terminate recomputation of terminal values
	double MCVEP3;
	//Epsilon to allow cut-off computations to be sensed
	double MCVEP4;
	//Maximum allowable total yaw angle (IGM)
	double MCVYMX;
	//Maximum pitch rate (IGM)
	double MCVPDL;
	//Maximum yaw rate (IGM)
	double MCVYDL;
	//Time-to-go reference velocity increment
	double MCVTGQ;
	//IGM range angle constant
	double MCVRQV;
	//Terminal valus rotation indicator
	double MCVRQT;
	//High or low thrust indicator
	int MCTIND;
	//Second stage exhaust velocity
	double MCVVX3;
	//Second stage flow rate
	double MCVWD3;
	//Transition time for mixture ratio shift (MRS)
	double MCVTB2;
	//S-IVB thrust level
	double MCTSAV;
	//S-IVB weight loss rate
	double MCTWAV;
	//Nominal integration cycle for PMMSIU
	double MCVDTM;
	//Converts RHO*AREA/MASS to .5 RHO*AREA/MASS in Er. (eventually in Er at least...)
	double MCCRAM;

	//Thrust of CSM RCS+X (2 quads)
	double MCTCT1;
	//Thrust of CSM RCS+X (4 quads)
	double MCTCT2;
	//Thrust of CSM RCS-X (2 quads)
	double MCTCT3;
	//Thrust of CSM RCS-X (4 quads)
	double MCTCT4;
	//On-board computer thrust level for CSM RCS+/-X thruster (2 quads)
	double MCTCT5;
	//On-board computer thrust level for CSM RCS+/-X thruster (4 quads)
	double MCTCT6;
	//Weight loss rate CSM RCS+X (2 quads)
	double MCTCW1;
	//Weight loss rate CSM RCS+X (4 quads)
	double MCTCW2;
	//Weight loss rate CSM RCS-X (2 quads)
	double MCTCW3;
	//Weight loss rate CSM RCS-X (4 quads)
	double MCTCW4;
	//Thrust of LM RCS+X (2 quads)
	double MCTLT1;
	//Thrust of LM RCS+X (4 quads)
	double MCTLT2;
	//Thrust of LM RCS-X (2 quads)
	double MCTLT3;
	//Thrust of LM RCS-X (4 quads)
	double MCTLT4;
	//On-board computer thrust level for LM RCS+/-X thruster (2 quads)
	double MCTLT5;
	//On-board computer thrust level for LM RCS+/-X thruster (4 quads)
	double MCTLT6;
	//Weight loss rate LM RCS+X (2 quads)
	double MCTLW1;
	//Weight loss rate LM RCS+X (4 quads)
	double MCTLW2;
	//Weight loss rate LM RCS-X (2 quads)
	double MCTLW3;
	//Weight loss rate LM RCS-X (4 quads)
	double MCTLW4;
	//APS thrust level
	double MCTAT1;
	//APS buildup thrust level
	double MCTAT2;
	//APS full load thrust level
	double MCTAT4;
	//On-board computer thrust level for LM APS thruster
	double MCTAT9;
	//APS weight loss rate
	double MCTAW1;
	//APS weight loss rate for buildup
	double MCTAW2;
	//APS weight loss rate for full load
	double MCTAW4;
	//APS phase two burn time
	double MCTAD2;
	//APS phase three burn time
	double MCTAD3;
	//Total APS ullage overlap
	double MCTAD9;
	//DPS thrust level
	double MCTDT1;
	//DPS phase 2 thrust level
	double MCTDT2;
	//DPS phase 3 thrust level
	double MCTDT3;
	//DPS 10% thrust level
	double MCTDT4;
	//DPS thrust for buildup from 10% to maximum
	double MCTDT5;
	//DPS full load thrust level
	double MCTDT6;
	//On-board computer thrust level for LM DPS thruster
	double MCTDT9;
	//DPS weight loss rate
	double MCTDW1;
	//DPS phase 2 weight loss rate
	double MCTDW2;
	//DPS phase 3 weight loss rate
	double MCTDW3;
	//DPS 10% weight loss rate
	double MCTDW4;
	//DPS weight loss rate for build from 10% to max.
	double MCTDW5;
	//DPS full load weight loss rate
	double MCTDW6;
	//DPS phase 2 burn time
	double MCTDD2;
	//DPS phase 3 burn time
	double MCTDD3;
	//Nominal dt of 10% DPS thrust
	double MCTDD4;
	//DPS burn time for building from 10% to max
	double MCTDD5;
	//DPS burn time used to determine if 20% thrust level is to be maintained throughout the burn
	double MCTDD6;
	//Total DPS ullage overlap
	double MCTDD9;
	//SPS thrust level
	double MCTST1;
	//SPS thrust level for buildup
	double MCTST2;
	//SPS thrust level for full load
	double MCTST4;
	//On-board computer thrust level for CSM SPS thruster
	double MCTST9;
	//SPS weight loss rate
	double MCTSW1;
	//SPS weight loss rate for buildup
	double MCTSW2;
	//SPS weight loss rate for full load
	double MCTSW4;
	//SPS phase two burn time
	double MCTSD2;
	//SPS phase three burn time
	double MCTSD3;
	//Total SPS ullage overlap
	double MCTSD9;

	//Impulse in one second SPS / APS / DPS burn
	double MCTAK1;
	//Initial value of SPS/APS/DPS minimum impulse curve
	double MCTAK2;
	//Slope of SPS/APS/DPS curve
	double MCTAK3;
	//LM RCS impulse due to 7 second, 2 jet ullage
	double MCTAK4;

	//SPS pitch electronic null parameter
	double MCTSPP;
	//SPS yaw electronic null parameter
	double MCTSYP;
	//SPS pitch gimbal system parameter
	double MCTSPG;
	//SPS yaw gimbal system parameter
	double MCTSYG;
	//APS pitch engine cant system parameter
	double MCTAPG;
	//APS roll engine cant system parameter
	double MCTARG;
	//DPS pitch gimbal system parameter
	double MCTDPG;
	//DPS roll gimbal system parameter
	double MCTDRG;
	//Nominal scaling factor for main DPS thrust level and weight loss rate
	double MCTDTF;
};