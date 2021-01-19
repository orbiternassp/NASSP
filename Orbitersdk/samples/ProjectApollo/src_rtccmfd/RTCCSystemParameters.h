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

struct RTCCSystemParameters
{
	RTCCSystemParameters()
	{
		//DEFAULT VALUES

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
};