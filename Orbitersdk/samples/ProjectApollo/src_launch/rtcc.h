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

const MATRIX3 A7REFSMMAT = _M(-0.097435921, -0.957429007, 0.271727726, -0.516196772, -0.184815392, -0.836291939, 0.850909933, -0.221749939, -0.476214282);
const MATRIX3 A8REFSMMAT = _M(0.496776313, -0.82489121, 0.269755125, -0.303982571, -0.456513584, -0.836175814, 0.812900983, 0.333391495, -0.477537684);

const double LaunchMJD[11] = {//Launch MJD of Apollo missions
	40140.62691,
	40211.535417,
	40283.666667,
	40359.700694,
	40418.563889,
	40539.68194,
	40687.80069,
	40982.87711,
	41128.56529,
	41423.74583,
	41658.23125
};

struct LambertMan //Data for Lambert targeting
{
	VESSEL* vessel; //Vessel executing the burn
	VESSEL* target; //Target vessel
	double GETbase; //usually MJD at launch
	double T1;	//GET of the maneuver
	double T2;	// GET of the arrival
	int N;		//number of revolutions
	int axis;	//Multi-axis or horizontal burn
	int Perturbation; //Spherical or non-spherical gravity
	VECTOR3 Offset; //Offset vector
	double PhaseAngle; //Phase angle to target, will overwrite offset
	bool prograde; //Prograde or retrograde solution
	int impulsive; //Calculated with nonimpulsive maneuver compensation or without
};

struct AP7ManPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int engopt; //0 = SPS, 1 = RCS+X, 2 = RCS-X
	bool HeadsUp; //Orientation during the maneuver
	MATRIX3 REFSMMAT;//REFSMMAT during the maneuver
	double sxtstardtime; //time delay for the sextant star check (in case no star is available during the maneuver)
	double navcheckGET; //Time for the navcheck. 0 = no nav check
};

struct AP7TPIPADOpt
{
	VESSEL* vessel; //vessel
	VESSEL* target; //Target vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
};

struct EntryOpt
{
	VESSEL* vessel; //Reentry vessel
	double GETbase; //usually MJD at launch
	double TIGguess; //Initial estimate for the TIG or baseline TIG for abort and MCC maneuvers
	int type; //Type of reentry maneuver
	double ReA; //Reentry angle at entry interface, 0 starts iteration to find reentry angle
	double lng; //Longitude of the desired splashdown coordinates
	double Range;  //Desired range from 0.05g to splashdown, 0 uses AUGEKUGEL function to determine range
	bool nominal; //Calculates minimum DV deorbit or nominal 31.7° line deorbit
	int impulsive; //Calculated with nonimpulsive maneuver compensation or without
	bool entrylongmanual; //Targeting a landing zone or a manual landing longitude
};

struct REFSMMATOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	bool REFSMMATdirect; //if false, there is a maneuver between "now" and the relevant time of the REFSMMAT calculation
	double P30TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	OBJHANDLE maneuverplanet; //The gravity reference of the maneuver might be different than the gravity reference now!
	int REFSMMATopt; //REFSMMAT options: 0 = P30 Maneuver, 1 = P30 Retro, 2= LVLH, 3= Lunar Entry, 4 = Launch, 5 = Landing Site, 6 = PTC
	double REFSMMATTime; //Time for the REFSMMAT calculation
	double LSLng; //longitude for the landing site REFSMMAT
	double LSLat; //latitude for the landign site REFSMMAT
	int mission; //Just for the launch REFSMMAT
};

struct CDHOpt
{
	VESSEL* vessel; //Vessel executing the burn
	VESSEL* target; //Target vessel
	double GETbase; //usually MJD at launch
	double DH; //Delta Height
	int CDHtimemode; //0 = Fixed Time, 1 = Find GETI
	double TIG; // (Estimated) Time of Ignition
	int impulsive; //Calculated with nonimpulsive maneuver compensation or without
};

struct AP7BLKOpt
{
	int n; //number of PAD entries
	double *lng; //pointer to splashdown longitudes
	double *GETI; //pointer to ignition times
	char **area; //pointer to splashdown areas
};

struct EarthEntryPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double P30TIG; //Time of Ignition (deorbit maneuver)
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates (deorbit maneuver)
	MATRIX3 REFSMMAT;
	bool preburn; //
	double lat; //splashdown latitude
	double lng; //splashdown longitude
};

struct LOIMan
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	int man; //0 = last MCC, 1 = LOI-1, 2 = LOI-2
	double MCCGET; //GET for the last MCC
	double lat; //target for MCC
	double lng; //target for MCC
	double PeriGET; //time of periapsis (for MCC)
	double h_apo;	//for LOI-1
	double h_peri;	//for MCC and LOI-1, circular orbit for LOI-2
	double inc;		//Inclination (equatorial) for LOI-1
};

struct OrbAdjOpt
{
	VESSEL* vessel;
	OBJHANDLE gravref;
	double SPSGET;
	double GETbase; //usually MJD at launch
	double h_apo;	//
	double h_peri;	//
	double inc;		//
};

// Parameter block for Calculation(). Expand as needed.
struct calculationParameters {
	VESSEL *src;	// Our ship
	VESSEL *tgt;	// Target ship
};

class RTCC {
public:
	RTCC();
	void Init(MCC *ptr);
	void Calculation(int fcn,LPVOID &pad);

	void AP7TPIPAD(AP7TPIPADOpt *opt, AP7TPI &pad);
	void EarthOrbitEntry(EarthEntryPADOpt *opt, AP7ENT &pad);
	void LambertTargeting(LambertMan *lambert, VECTOR3 &dV_LVLH, double &P30TIG);
	double CDHcalc(CDHOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	MATRIX3 REFSMMATCalc(REFSMMATOpt *opt);
	void LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	void OrbitAdjustCalc(OrbAdjOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG);

	MCC *mcc;
	struct calculationParameters calcParams;
private:
	void AP7ManeuverPAD(AP7ManPADOpt *opt, AP7MNV &pad);
	MATRIX3 GetREFSMMATfromAGC();
	void navcheck(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double &lat, double &lng, double &alt);
	void StateVectorCalc(VESSEL *vessel, double &SVGET, VECTOR3 &BRCSPos, VECTOR3 &BRCSVel);
	OBJHANDLE AGCGravityRef(VESSEL* vessel); // A sun referenced state vector wouldn't be much of a help for the AGC...
	void EntryTargeting(EntryOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG, double &latitude, double &longitude);
	double getGETBase();
	void AP7BlockData(AP7BLKOpt *opt, AP7BLK &pad);
	LambertMan set_lambertoptions(VESSEL* vessel, VESSEL* target, double GETbase, double T1, double T2, int N, int axis, int Perturbation, VECTOR3 Offset, double PhaseAngle,bool prograde, int impulsive);
	double lambertelev(VESSEL* vessel, VESSEL* target, double GETbase, double elev);
};


#endif