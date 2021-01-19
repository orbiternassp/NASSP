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
	//Nautical miles per Earth radii
	double MCCNMC = 3443.93359;
	//MJD of epoch
	double AGCEpoch;
	//Mean lunar radius
	double MCSMLR = 1738090.0;
	//Radians per degree
	double MCCRPD = 0.01745329251; // PI/180
	//Geodetic Earth constant B²/A²
	double MCEBAS = 1.0;
	//Geodetic Earth constant A²
	double MCEASQ = 4.0619437e13;
	//Geodetic Earth constant B²
	double MCEBSQ = 4.0619437e13;
	//Rotational rate of the Earth (radians/hr.)
	double MCERTS = 0.26251614528012;
	//Burnout launch azimuth (rad.)
	double MCLABN;
	//Sine of burnout launch azimuth
	double MCLSBN;
	//Cosine of burnout launch azimuth
	double MCLCBN;
	//Pitch angle from horizon (rad.)
	double MCGHZA = 0.55326937288; //31.7°
	//Semimajor axis for integration limit (Er.)
	double MCGSMA = 1.5;
	//Eccentricity for integration limit
	double MCGECC = 0.5;
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
};