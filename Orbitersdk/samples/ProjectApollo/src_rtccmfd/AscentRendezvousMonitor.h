/****************************************************************************
This file is part of Project Apollo - NASSP

Ascent Rendezvous Monitor Displays (Header)

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

#include "Orbitersdk.h"
#include "RTCCModule.h"
#include "RTCCTables.h"

struct ARMInputs
{
	//CSM state vector
	EphemerisData sv_CSM;
	//LM state vector (MSFN, PGNS, AGS)
	EphemerisData sv_LM[3];
	//Terminal phase travel angle
	double WT;
	//Elevation angle
	double E;
	//CSI: If zero CSI performed at first apolune after insertion. non-zero parameter used as delta time from insertion to CSI.
	double CSIFlag;
	//CDH Indicator (+N: N apsis crossings from CSI to CDH, -N: N/2 revs from CSI to CDH (N must be odd)
	int CDHIndicator;
	//TPI time (GMT)
	double t_TPI;
	//Minimum safe perilune
	double h_min;
	//Desired Delta H
	double DH;
	//Time of insertion (FIDO thrust switch from event to no event)
	double t_Ins;
};

struct ARMDisplayQuantities
{
	//Perilune after tweak maneuver
	double HP;
	//GET of tweak
	double GETI;
	//Delta velocity of tweak maneuver
	double DV_Tweak;
	//GET of CSI
	double GET_CSI;
	//DV of CSI
	double DV_CSI;
	//GET of CDH
	double GET_CDH;
	//DV of CDH
	double DV_CDH;
	//GET of apolune
	double GET_Kick;
	//Delta velocity of apolune maneuver
	double DV_Kick;
	//Tweak maneuver required
	bool TweakRequired;
};

struct ARMDisplay
{
	ARMDisplayQuantities tab[3];
};

//Tweak burn for coelliptic rendezvous profile
class AscentRendezvousMonitor : public RTCCModule
{
public:
	AscentRendezvousMonitor(RTCC *r);

	int Calc(const ARMInputs &in, ARMDisplay &out);
protected:
	int CalcTweak(const ARMInputs &in, ARMDisplay &out);
	void ApoluneKick(const ARMInputs &in, ARMDisplay &out);
};

struct ShortARMInputs
{
	//CSM state vector
	EphemerisData sv_CSM;
	//LM state vector (MSFN, PGNS, AGS)
	EphemerisData sv_LM[3];
	//false = time of tweak DT from insertion, true = input time of tweat
	bool ITWEAK;
	//false = time of TPI is DTPI from insertion, true = input time of TPI
	bool ITPI;
	//Time of tweak
	double t_tweak;
	//Time of TPI
	double t_TPI;
	//Delta time of tweak
	double DT;
	//Delta time of TPI
	double DTPI;
	//Phase and height offsets at TPI
	double DTHETA, DH;
	//Terminal phase transfer angle
	double WT;
	//IMU gimbal angles
	VECTOR3 IMUAngles;
	//body axis reference. false = gimbal angles, true = Axhor
	bool IREF;
	//Angle between line of sight to the forward horizon and the spacecraft X-axis
	double Axhor;
	MATRIX3 REFSMMAT;
	//Time of insertion (FIDO thrust switch from event to no event)
	double t_Ins;
};

struct ShortARMDisplayQuantities
{
	//Perilune after tweak maneuver
	double HP;
	//GET of tweak
	double GETI;
	//Delta velocity of tweak in body coordinates
	VECTOR3 DV_B;
	//IMU attitude
	VECTOR3 Att;
	//GET of TPI maneuver
	double GETTPI;
	//Delta velocity of TPI maneuver
	double DV_TPI;
	//LVLH yaw of tweak maneuver
	double Y_H;
	//LVLH pitch of tweak maneuver
	double P_H;
	//If tweak DV exceeds 60 ft/s, recommend a bailout instead
	bool DoBailout;
};

struct ShortARMDisplay
{
	ShortARMDisplayQuantities tab[3];
};

//Tweak burn for short rendezvous profile
class ShortAscentRendezvousMonitor : public RTCCModule
{
public:
	ShortAscentRendezvousMonitor(RTCC *r);

	int Calc(const ShortARMInputs &in, ShortARMDisplay &out);
};