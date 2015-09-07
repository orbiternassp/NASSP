/****************************************************************************
This file is part of Project Apollo - NASSP

Real-Time Computer Complex (RTCC)

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

#if !defined(_PA_RTCC_H)
#define _PA_RTCC_H

#define RTCC_LAMBERT_MULTIAXIS 0
#define RTCC_LAMBERT_XAXIS 1

#define RTCC_LAMBERT_SPHERICAL 0
#define RTCC_LAMBERT_PERTURBED 1

#define RTCC_LAMBERT_RETROGRADE 0
#define RTCC_LAMBERT_PROGRADE 1

#define RTCC_IMPULSIVE 0
#define RTCC_NONIMPULSIVE 1

#define RTCC_ENTRY_DEORBIT 0
#define RTCC_ENTRY_MCC 1
#define RTCC_ENTRY_ABORT 2

#define RTCC_ENTRY_MINDV 0
#define RTCC_ENTRY_NOMINAL 1

struct LambertMan //Data for Lambert targeting
{
	VESSEL* target; //Target vessel
	double T1;	//GET of the maneuver
	double T2;	// GET of the arrival
	int N;		//number of revolutions
	int axis;	//Multi-axis or horizontal burn
	int Perturbation; //Spherical or non-spherical gravity
	VECTOR3 Offset; //Offset vector
	double PhaseAngle; //Phase angle to target, will overwrite offset
	OBJHANDLE gravref; //Gravity reference of the maneuver
	bool prograde; //Prograde or retrograde solution
	int impulsive; //Calculated with nonimpulsive maneuver compensation or without
};

struct AP7ManPADOpt
{
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int engopt; //0 = SPS, 1 = RCS+X, 2 = RCS-X
	bool HeadsUp; //Orientation during the maneuver
	MATRIX3 REFSMMAT;//REFSMMAT during the maneuver
	double sxtstardtime; //time delay for the sextant star check (in case no star is available during the maneuver)
	double navcheckGET; //Time for the navcheck. 0 = no nav check
};

struct EntryOpt
{
	VESSEL* vessel; //Reentry vessel
	double TIGguess; //Initial estimate for the TIG or baseline TIG for abort and MCC maneuvers
	int type; //Type of reentry maneuver
	double ReA; //Reentry angle at entry interface, 0 starts iteration to find reentry angle
	double lng; //Longitude of the desired splashdown coordinates
	double Range;  //Desired range from 0.05g to splashdown, 0 uses AUGEKUGEL function to determine range
	bool nominal; //Calculates minimum DV deorbit or nominal 31.7° line deorbit
	int impulsive; //Calculated with nonimpulsive maneuver compensation or without
};

class RTCC {
public:
	RTCC();
	void Init(MCC *ptr);
	void Calculation(LPVOID &pad);

	MCC *mcc;
private:
	void LambertTargeting(LambertMan *lambert, VECTOR3 &dV_LVLH, double &P30TIG);
	void AP7ManeuverPAD(AP7ManPADOpt *opt, AP7MNV &pad);
	MATRIX3 GetREFSMMATfromAGC();
	void navcheck(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double &lat, double &lng, double &alt);
	void StateVectorCalc(VESSEL *vessel, double &SVGET, VECTOR3 &BRCSPos, VECTOR3 &BRCSVel);
	OBJHANDLE AGCGravityRef(VESSEL* vessel); // A sun referenced state vector wouldn't be much of a help for the AGC...
	void EntryTargeting(EntryOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG);
};


#endif