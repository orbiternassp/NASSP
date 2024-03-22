/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Digital Command System (Header)

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

#define DCSUPLINK_SWITCH_SELECTOR			0
#define DCSUPLINK_TIMEBASE_UPDATE			1
#define DCSUPLINK_LM_ABORT					2
#define DCSUPLINK_TDE_ENABLE				3
#define DCSUPLINK_RESTART_MANEUVER_ENABLE	4
#define DCSUPLINK_TIMEBASE_8_ENABLE			5
#define DCSUPLINK_EVASIVE_MANEUVER_ENABLE	6
#define DCSUPLINK_EXECUTE_COMM_MANEUVER		7
#define DCSUPLINK_SIVBIU_LUNAR_IMPACT		8
#define DCSUPLINK_REMOVE_INHIBIT_MANEUVER4	9
#define DCSUPLINK_GENERALIZED_MANEUVER		10
//From here on not available in the PAMFD
#define DCSUPLINK_SATURNIB_LAUNCH_TARGETING	11
#define DCSUPLINK_SLV_NAVIGATION_UPDATE 	12
#define DCSUPLINK_SLV_TLI_TARGETING_UPDATE	13

#define DCS_START_STRING	"DCS_BEGIN"
#define DCS_END_STRING		"DCS_END"

//Generalized Switch Selector uplink
struct DCSSWITSEL
{
	int stage;
	int channel;
};

//Timebase update
struct DCSTBUPDATE
{
	double dt;
};

//SIVB/IU Lunar Impact
struct DCSLUNARIMPACT
{
	double tig;
	double dt;
	double pitch;
	double yaw;
};

//Saturn IB Launch Targeting Update
struct DCSLAUNCHTARGET
{
	double V_T;
	double R_T;
	double theta_T;
	double i;
	double lambda_0;
	double lambda_dot;
	double T_GRR0;
};

//SLV Navigation Update
struct DCSSLVNAVUPDATE
{
	double NUPTIM = 0.0;
	VECTOR3 PosS = _V(0, 0, 0);
	VECTOR3 DotS = _V(0, 0, 0);
};

//SLV TLI Target Update
struct DCSSLVTLITARGET
{
	double T_RP = 0.0;
	double C_3 = 0.0;
	double Inclination = 0.0;
	double e = 0.0;
	double alpha_D = 0.0;
	double f = 0.0;
	double theta_N = 0.0;
};

//SIB Generalized Attitude Maneuver
struct DCSGENMANEUVER
{
	double T = 0.0;
	double X = 0.0;
	double Y = 0.0;
	double Z = 0.0;
	int Type = 0;
};

class IU;

class DCS
{
public:
	DCS(IU *i);
	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

	virtual bool Uplink(int type, void *upl);

	bool IsCommandSystemEnabled();
protected:
	IU *iu;
};