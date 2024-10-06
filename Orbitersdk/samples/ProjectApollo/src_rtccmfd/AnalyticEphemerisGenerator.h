/****************************************************************************
This file is part of Project Apollo - NASSP

Analytic Ephemeris Generator (Header)

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

#include "OrbMech.h"
#include "RTCCModule.h"

struct AEGHeader
{
	//0 = Earth, 1 = Lunar
	int AEGInd = 0;
	int ErrorInd = 0;
	int NumBlocks = 1;
	//int Spare;
};

struct AEGDataBlock
{
	AEGDataBlock();

	//Item 1, 11 is Keplerian to Keplerian (only used in AEG service routine)
	int InputOutputInd;
	//Item 2, initialize/update indicator (0 = Osculating elements provided, 1 = osc and mean elements provided, use low e form., 2 = same as 1, but high e)
	int ENTRY;
	//Item 3, update option indicator (0 = to time, 1 = to mean anomaly, 2 = to argument of latitude, 3 = to maneuver counter line...)
	int TIMA;
	//Item 4, 0 for J2 and J4, 1 for J2, J3, J4 and -1 for J2 and J3 (only 1 should be used in the real time system)
	int HarmonicsInd;
	//Item 5, drag indicator. If nonzero it is the K-Factor
	double ICSUBD;
	//Item 6, vehicle area
	double VehArea;
	//Item 7, vehicle weight for Earth AEG
	double Item7;
	//Item 8, Input: Mean anomaly (option 1), argument of latitude (option 2-3), Output: DH (options 5-6) otherwise same as input
	double Item8;
	//Item 9, crossing time of the reference counter line (input), time lag (options 5-6), otherwise same as input (output)
	double Item9;
	//Item 10, number of times the AEG will cross the reference line (input), phase angle for options 4-6 (output)
	double Item10;
	CELEMENTS coe_osc;
	CELEMENTS coe_mean;
	//Item 23, time associated with elements 11-16 (in GMT)
	double TS;
	double l_dot;
	double g_dot;
	double h_dot;
	//Item 27, time to which to update elements (in GMT)
	double TE;
	double f;
	double U;
	double R;
};

struct AEGBlock
{
	AEGHeader Header;
	AEGDataBlock Data;
};

class PMMAEG
{
public:
	PMMAEG();
	void CALL(AEGHeader &header, AEGDataBlock &in, AEGDataBlock &out);
protected:
	AEGDataBlock CurrentBlock;
};

class PMMLAEG : public RTCCModule
{
public:
	PMMLAEG(RTCC *r);
	void CALL(AEGHeader &header, AEGDataBlock &in, AEGDataBlock &out);
protected:
	AEGDataBlock CurrentBlock;
};