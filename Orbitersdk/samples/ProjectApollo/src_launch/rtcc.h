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

#define RTCC_LAMBERT_IMPULSIVE 0
#define RTCC_LAMBERT_NONIMPULSIVE 1

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
	int prograde; //Prograde or retrograde solution
	int impulsive;
};

struct AP7ManPADOpt
{
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int engopt; //0 = SPS, 1 = RCS+X, 2 = RCS-X
	bool HeadsUp; //Orientation during the maneuver
	MATRIX3 REFSMMAT;//REFSMMAT during the maneuver
	double sxtstardtime; //time delay for the sextant star check (in case no star is available during the maneuver)
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
};


#endif