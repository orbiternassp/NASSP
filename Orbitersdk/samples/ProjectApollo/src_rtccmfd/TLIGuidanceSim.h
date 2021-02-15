/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC S-IVB TLI Numerical Integrator and IGM Guidance (Header)

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
#include "RTCCTables.h"
#include "RTCCModule.h"

//Module PMMSIU
class TLIGuidanceSim : public RTCCModule
{
public:
	TLIGuidanceSim(RTCC *r, RTCCNIInputTable TABLIN, int &IRETN, EphemerisDataTable *EPHEM, RTCCNIAuxOutputTable *AUX, MATRIX3 *adramat, std::vector<double> *WTABL = NULL);
	void PCMTRL();
private:
	//Thrust Subroutine
	void PCMTH();
	//Ephemeris Storage Subroutine
	void PCMEP();
	//TLI Guidance Subroutine
	void PCMGN();
	//Runge Kutta Integration Subroutine
	void PCMRK();
	//Second Derivative Subroutine
	VECTOR3 PCMDC();

	//Current time in hr.
	double T;
	//Previous time in hr.
	double TPR;
	//Thrust phase indicator (0 = No Thrust, 1 = initial phase, -1 = non-initial phase thrust)
	int KTHSWT;
	//Maneuver phase indicator
	int MPHASE;
	//Current weight
	double WT;
	//Current thrust
	double THRUST;
	//Current weight loss rate
	double WTLRT;
	//Pitch gimbal
	double PITCHG;
	//Yaw gimbal
	double YAWG;
	//Error indicator
	int IERR;
	int IERRE;
	//Chilldown phase indicator
	int IPUTIG;
	//Full thrust phase indicator
	int IPHFUL;
	//Tailoff phase indicator
	int IPHTOF;
	//Accumulated DV increment due to pure ullage
	double DVULL;
	//Accumulated DV increment along X-body axis excluding tailoff
	double DVX;
	//Accumulated DV increment along X-body axis during tailoff
	double DVXTO;
	//Accumulated DV increment during tailoff
	double DVTO;
	//Accumulate DV increment of maneuver
	double DVMAG;
	//Thrust acceleration vector
	VECTOR3 RDDTH;
	//Gravity vector
	VECTOR3 PVEC;
	//Unit thrust vector
	VECTOR3 UT;
	//Some large number
	double TLARGE;
	//Some small number
	double EPS;
	//Drag factor
	double CDFACT;
	//Area over mass
	double AOM;
	//IGM initiated flag
	int IGMSKP;
	//Time for storing the next ephemeris/weight
	double TNEXT;
	//Earth's potential function computation indicator
	int K3SWT;
	//Double integration indicator
	int IDOUBL;
	//Current integration interval
	double DT;
	//Ephemeris position vector
	VECTOR3 RE;
	//Ephemeris velocity vector
	VECTOR3 VE;
	//Vehicle position vector
	VECTOR3 RV;
	//Vehicle velocity vector
	VECTOR3 VV;
	//Working position vector
	VECTOR3 R;
	//Working velocity vector
	VECTOR3 V;
	//Perturbed position vector
	VECTOR3 RP;
	//Perturbed velocity vector
	VECTOR3 VP;
	//Previous DT
	double DTPREV;
	//Previous T
	double TPREV;
	//Indicator for double integration
	int INPASS;
	//End of current integration step
	double TAU;
	//Ephemeris storage counter
	unsigned IEPHCT;
	//mu of Earth
	double EMU;
	//IGM time to end maneuver
	double TCO;
	double TCOF;
	//Unit thrust vector to be used for middle of the integration step (in integrating system coordinates)
	VECTOR3 THRSTM;
	//Unit thrust vector to be used at end of the integration step (in integrating system coordinates)
	VECTOR3 THRSTR;
	//Target parameter update (0 = done, 1 = update not done yet)
	int ITUP;
	//Next event
	double TI;
	//Next time
	double TE;
	//MRS indicator (-1 before, 0 = after, 1 = during MRS)
	int IMRS;
	//Absolute value of ephemeris storage indicator
	int KEHOP;
	//End flag
	int IEND;
	double TEND;
	double TENDF;
	double VEX3;
	double WDOT3;
	double TAU2;
	//Transition time for SIVB MRS
	double TB2;
	//Time from S4B MRS
	double TB4;
	//Initial or non-initial pass indicator for IGM (=0, or !=0 respectively)
	int IFIRST;
	//Ephemeris storage step length
	double STEP;
	//Initial attitude indicator
	int INITL;
	//Time of last ephemeris or weight storage
	double TLOP;
	//Velocity cutoff storage
	double VSB0, VSB1, VSB2;
	//Weight at maneuver initiation
	double WBM;
	//Cutoff indicator (1 = cutoff time has been calculated)
	int ICO;
	//Last time IGM calculations were done
	double TLAST;
	//New pitch angle
	double PITN;
	//New yaw angle
	double YAWN;
	//Old pitch angle
	double PITO;
	//Old yaw angle
	double YAWO;
	//Ephemeris storage index
	int JT;
	//Weight table storage index
	int KWT;
	//Number of items to be stored
	int NITEMS;
	//Time to initiate IGM
	double TIGM;
	//Time of ignition (TB6 start)
	double TIG;
	//MRS timer
	double PC;
	//Artificial tau timer
	double CPR;
	MATRIX3 G, GG, PLMB, MX_K;
	//Epsilon to control range angle computation
	double EPSL1;
	//Epsilon to terminate computation of K1, K2, K3, and K4
	double EPSL2;
	//Epsilon to terminate recomputation of terminal values
	double EPSL3;
	//Epsilon to allow cut-off computations to be sensed
	double EPSL4;
	VECTOR3 PLPS;
	//Intermediate variables for cutoff computation
	double DT1P, DT2P;
	//Thrust integral values that need to be global
	double XL2, XK1, XK2, XK3, XK4, XJ2, S2, P2, Q2, U2;
	double CYP, SYP;
	double H, H2, H4, H6;
	double VTEST;

	//Targeting parameters

	//Eccentricity of the desired cut-off ellipse
	double E;
	//Desired terminal energy
	double C3;
	//True anomaly of descending node of desired cut-off ellipse
	double ALPHD;
	//Estimate of true anomaly at cutoff
	double F;
	//Semilatus rectum of the desired cut-off ellipse (can also be RN)
	double P;
	//Constant sqrt(mu/P)
	double XK5;
	//Desired terminal position radius magnitude
	double RT;
	//Desired terminal velocity
	double VT;
	//Desired terminal flight-path angle
	double GAMT;
	//Vehicle pitch and yaw angles at restart preparation
	double PRP, YRP;
	//Remaining burn time
	double TGO;
	//Maneuver duration before mixture ration shift (MRS) occurs
	double T2;
	//Second stage "IGM" exhaust velocity
	double VEX2;
	//Second stage "IGM" mass flow rate
	double WDOT2;
	//Approximation of maneuver duration after MRS has occured
	double T3;
	//Total time required to exhaust vehicle mass after MRS has occured
	double TAU3;
	//Cut-off velocity bias
	double DVBIAS;
	//Estimated nominal second stage burn to mass depletion
	double TAU2N;
	//Nominal radius at ignition 
	double RN;
	//Estimate gravitation acceleration at maneuver burnout
	double GT;

	//Phase 1: Vent and ullage
	//Phase 2: Chilldown
	//Phase 3: Buildup
	//Phase 4: Main Burn
	//Phase 5: MRS
	//Phase 6: After MRS
	//Phase 7: Tailoff
	double DTPHASE[7];
	double FORCE[7];
	double WTFLO[7];

	RTCCNIInputTable TABLIN;
	int &IRETN;
	EphemerisDataTable *EPHEM;
	RTCCNIAuxOutputTable *AUX;
	std::vector<double> *WTABL;
	MATRIX3 *ADRMAT;

	OBJHANDLE hEarth;
};