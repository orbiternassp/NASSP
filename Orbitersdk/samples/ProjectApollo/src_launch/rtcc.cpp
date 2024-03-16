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

// To force Orbitersdk.h to use <fstream> in any compiler version
//#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "soundlib.h"
#include "ioChannels.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "CSMcomputer.h"
#include "papi.h"
#include "saturn.h"
#include "LEMcomputer.h"
#include "LEM.h"
#include "sivb.h"
#include "../src_rtccmfd/OrbMech.h"
#include "../src_rtccmfd/EntryCalculations.h"
#include "../src_rtccmfd/TLIGuidanceSim.h"
#include "../src_rtccmfd/CSMLMGuidanceSim.h"
#include "../src_rtccmfd/GeneralizedIterator.h"
#include "../src_rtccmfd/EnckeIntegrator.h"
#include "../src_rtccmfd/ReentryNumericalIntegrator.h"
#include "mcc.h"
#include "rtcc.h"
#include "nassputils.h"

using namespace nassp;

// SCENARIO FILE MACROLOGY
#define SAVE_BOOL(KEY,VALUE) oapiWriteScenario_int(scn, KEY, VALUE)
#define SAVE_INT(KEY,VALUE) oapiWriteScenario_int(scn, KEY, VALUE)
#define SAVE_DOUBLE(KEY,VALUE) papiWriteScenario_double(scn, KEY, VALUE)
#define SAVE_DOUBLE2(KEY,VALUE1, VALUE2) papiWriteScenario_double2(scn, KEY, VALUE1, VALUE2)
#define SAVE_V3(KEY,VALUE) papiWriteScenario_vec(scn, KEY, VALUE)
#define SAVE_M3(KEY,VALUE) papiWriteScenario_mx(scn, KEY, VALUE)
#define SAVE_STRING(KEY,VALUE) oapiWriteScenario_string(scn, KEY, VALUE)
#define LOAD_BOOL(KEY,VALUE) if(strnicmp(line, KEY, strlen(KEY)) == 0){ sscanf(line + strlen(KEY), "%i", &tmp); if (tmp == 1) { VALUE = true; } else { VALUE = false; } }
#define LOAD_INT(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%i",&VALUE); }
#define LOAD_DOUBLE(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf",&VALUE); }
#define LOAD_DOUBLE2(KEY,VALUE1, VALUE2) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf %lf",&VALUE1, &VALUE2); }
#define LOAD_V3(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf %lf %lf",&VALUE.x,&VALUE.y,&VALUE.z); }
#define LOAD_M3(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf %lf %lf %lf %lf %lf %lf %lf %lf",&VALUE.m11,&VALUE.m12,&VALUE.m13,&VALUE.m21,&VALUE.m22,&VALUE.m23,&VALUE.m31,&VALUE.m32,&VALUE.m33); }
#define LOAD_STRING(KEY,VALUE,LEN) if(strnicmp(line,KEY,strlen(KEY))==0){ strncpy(VALUE, line + (strlen(KEY)+1), LEN); }

void format_time_rtcc(char *buf, double time) {
	buf[0] = 0; // Clobber
	int hours, minutes, seconds;
	if (time < 0) { return; } // don't do that
	hours = (int)(time / 3600);
	minutes = (int)((time / 60) - (hours * 60));
	seconds = (int)((time - (hours * 3600)) - (minutes * 60));
	sprintf_s(buf, 64, "%03d:%02d:%02d", hours, minutes, seconds);
}

//Ephemeris format 2 to format 1
EphemerisData Eph2ToEph1(EphemerisData2 in, int RBI)
{
	EphemerisData out;

	out.R = in.R;
	out.V = in.V;
	out.GMT = in.GMT;
	out.RBI = RBI;
	return out;
}

void papiWriteScenario_Station(FILEHANDLE scn, char *item, int i, StationData stat)
{

	char buffer[256];

	sprintf(buffer, "  %s %d %s %.12lf %.12lf %.12lf %.12lf", item, i, stat.code.c_str(), stat.H, stat.lat_geod, stat.lng, stat.R_S);
	oapiWriteLine(scn, buffer);
}

bool papiReadScenario_Station(char *line, char *item, StationData *stat)
{

	char buffer[256], name[16];
	StationData v;
	int i;

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			if (sscanf(line, "%s %d %s %lf %lf %lf %lf", buffer, &i, name, &v.H, &v.lat_geod, &v.lng, &v.R_S) == 7) {
				v.code.assign(name);
				stat[i] = v;
				return true;
			}
		}
	}
	return false;
}

void papiWriteScenario_SV(FILEHANDLE scn, char *item, SV sv)
{

	char buffer[256], name[16];

	if (sv.gravref)
	{
		oapiGetObjectName(sv.gravref, name, 16);
	}
	else
	{
		sprintf(name, "None");
	}

	sprintf(buffer, "  %s %s %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf", item, name, sv.mass, sv.MJD, sv.R.x, sv.R.y, sv.R.z, sv.V.x, sv.V.y, sv.V.z);
	oapiWriteLine(scn, buffer);
}

bool papiReadScenario_SV(char *line, char *item, SV &sv)
{

	char buffer[256], name[16];
	SV v;

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			if (sscanf(line, "%s %s %lf %lf %lf %lf %lf %lf %lf %lf", buffer, name, &v.mass, &v.MJD, &v.R.x, &v.R.y, &v.R.z, &v.V.x, &v.V.y, &v.V.z) == 10) {
				v.gravref = oapiGetObjectByName(name);
				sv = v;
				return true;
			}
		}
	}
	return false;
}

void papiWriteScenario_SV(FILEHANDLE scn, char *item, EphemerisData sv) {

	char buffer[256];

	sprintf(buffer, "  %s %d %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf", item, sv.RBI, sv.GMT, sv.R.x, sv.R.y, sv.R.z, sv.V.x, sv.V.y, sv.V.z);
	oapiWriteLine(scn, buffer);
}

void papiWriteScenario_SV(FILEHANDLE scn, char *item, int i, StateVectorTableEntry vec) {

	char buffer[256];

	sprintf(buffer, "  %s %d %s %d %d %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %d", item, i, vec.VectorCode.c_str(), vec.ID, vec.Vector.RBI, vec.Vector.GMT, vec.Vector.R.x, vec.Vector.R.y, vec.Vector.R.z, vec.Vector.V.x, vec.Vector.V.y, vec.Vector.V.z, vec.LandingSiteIndicator);
	oapiWriteLine(scn, buffer);
}

bool papiReadScenario_SV(char *line, char *item, EphemerisData &sv)
{
	char buffer[256];

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			EphemerisData v;
			if (sscanf(line, "%s %d %lf %lf %lf %lf %lf %lf %lf", buffer, &v.RBI, &v.GMT, &v.R.x, &v.R.y, &v.R.z, &v.V.x, &v.V.y, &v.V.z) == 9) {
				sv = v;
				return true;
			}
		}
	}
	return false;
}

bool papiReadScenario_SV(char *line, char *item, StateVectorTableEntry *vec)
{
	char buffer[256];

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			char buffer2[64];
			int i, ID, LSInd;

			EphemerisData v;
			if (sscanf(line, "%s %d %s %d %d %lf %lf %lf %lf %lf %lf %lf %d", buffer, &i, buffer2, &ID, &v.RBI, &v.GMT, &v.R.x, &v.R.y, &v.R.z, &v.V.x, &v.V.y, &v.V.z, &LSInd) == 13) {
				vec[i].ID = ID;
				vec[i].VectorCode.assign(buffer2);
				vec[i].Vector = v;
				vec[i].LandingSiteIndicator = (LSInd != 0);
				return true;
			}
		}
	}
	return false;
}

bool papiReadConfigFile_ATPSite(char *line, char *item, std::string &ATPSite, double *ATPCoordinates, int &num)
{
	char buffer[256];

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			char buffer2[64];
			double CC[10];

			if (sscanf(line, "%s %d %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", buffer, &num, buffer2, &CC[0], &CC[1], &CC[2], &CC[3], &CC[4], &CC[5], &CC[6], &CC[7], &CC[8], &CC[9]) == 13) {
				ATPSite.assign(buffer2);
				for (int i = 0;i < 10;i++)
				{
					if (CC[i] >= 1e9)
					{
						ATPCoordinates[i] = 1e10;
					}
					else
					{
						ATPCoordinates[i] = CC[i] * RAD;
					}
				}
				return true;
			}
		}
	}
	return false;
}

bool papiReadConfigFile_PTPSite(char *line, char *item, std::string &PTPSite, double *PTPCoordinates, int &num)
{
	char buffer[256];

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			char buffer2[64];
			double CC[2];

			if (sscanf(line, "%s %d %s %lf %lf", buffer, &num, buffer2, &CC[0], &CC[1]) == 5) {
				PTPSite.assign(buffer2);
				PTPCoordinates[0] = CC[0] * RAD;
				PTPCoordinates[1] = CC[1] * RAD;
				return true;
			}
		}
	}
	return false;
}

bool papiReadConfigFile_CGTable(char *line, char *item, double *WeightTable, VECTOR3 *CGTable)
{
	char buffer[256];

	if (sscanf(line, "%s", buffer) == 1)
	{
		if (!strcmp(buffer, item))
		{
			int num;
			double W;
			VECTOR3 CG;
			if (sscanf(line, "%s %d %lf %lf %lf %lf", buffer, &num, &W, &CG.x, &CG.y, &CG.z) == 6)
			{
				if (num >= 0 && num <= 39)
				{
					WeightTable[num] = W * 0.453597;
					CGTable[num] = CG * 0.0254;
				}

				return true;
			}
		}
	}
	return false;
}

FIDOOrbitDigitals::FIDOOrbitDigitals()
{
	A = 0.0;
	E = 0.0;
	GAM = 0.0;
	GET = 0.0;
	GETA = 0.0;
	GETCC = 0.0;
	GETID = 0.0;
	GETL = 0.0;
	GETP = 0.0;
	GMTID = 0.0;
	H = 0.0;
	HA = 0.0;
	HP = 0.0;
	I = 0.0;
	K = 0.0;
	L = 0.0;
	LA = 0.0;
	LNPP = 0.0;
	LP = 0.0;
	LPP = 0.0;
	NV1 = 0;
	NV2 = 0;
	ORBWT = 0.0;
	PA = 0.0;
	PP = 0.0;
	PPP = 0.0;
	sprintf(REF, "");
	REV = 0;
	REVL = 0;
	REVR = 0;
	sprintf(STAID, "");
	TAPP = 0.0;
	TO = 0.0;
	V = 0.0;
	sprintf(VEHID, "");
	sprintf(REFR, "");
	GETBV = 0.0;
	HAR = 0.0;
	PAR = 0.0;
	LAR = 0.0;
	GETAR = 0.0;
	HPR = 0.0;
	PPR = 0.0;
	LPR = 0.0;
	GETPR = 0.0;
	Error = 0;
}

SpaceDigitals::SpaceDigitals()
{
	TUP = 0;
	sprintf(VecID, "");
	WEIGHT = 0.0;
	GMTV = 0.0;
	GETV = 0.0;
	GETAxis = 0.0;
	GETR = 0.0;
	GET = 0.0;
	sprintf(REF, "");
	V = 0.0;
	PHI = 0.0;
	H = 0.0;
	ADA = 0.0;
	GAM = 0.0;
	LAM = 0.0;
	PSI = 0.0;
	sprintf(VEHID, "");
	GETVector1 = 0.0;
	sprintf(REF1, "");
	WT = 0.0;
	GETA = 0.0;
	HA = 0.0;
	HP = 0.0;
	H1 = 0.0;
	V1 = 0.0;
	GAM1 = 0.0;
	PSI1 = 0.0;
	PHI1 = 0.0;
	LAM1 = 0.0;
	HS = 0.0;
	HO = 0.0;
	PHIO = 0.0;
	IEMP = 0.0;
	W1 = 0.0;
	OMG = 0.0;
	PRA = 0.0;
	A1 = 0.0;
	L1 = 0.0;
	E1 = 0.0;
	I1 = 0.0;
	GETVector2 = 0.0;
	GETSI = 0.0;
	GETCA = 0.0;
	VCA = 0.0;
	HCA = 0.0;
	PCA = 0.0;
	LCA = 0.0;
	PSICA = 0.0;
	GETMN = 0.0;
	HMN = 0.0;
	PMN = 0.0;
	LMN = 0.0;
	DMN = 0.0;
	GETVector3 = 0.0;
	GETSE = 0.0;
	GETEI = 0.0;
	VEI = 0.0;
	GEI = 0.0;
	PEI = 0.0;
	LEI = 0.0;
	PSIEI = 0.0;
	GETVP = 0.0;
	VVP = 0.0;
	HVP = 0.0;
	PVP = 0.0;
	LVP = 0.0;
	PSIVP = 0.0;
	IE = 0.0;
	LN = 0.0;
	TUN1 = TUN2 = TUN3 = 0;
}

CheckoutMonitor::CheckoutMonitor()
{
	sprintf_s(VEH, "CSM");
	GET = 0.0;
	GMT = 0.0;
	sprintf_s(VID, "");
	Pos = _V(0, 0, 0);
	Vel = _V(0, 0, 0);
	V_i = 0.0;
	gamma_i = 0.0;
	psi = 0.0;
	phi_c = 0.0;
	lambda = 0.0;
	h_s = 0.0;
	h_a = 0.0;
	h_p = 0.0;
	h_o_NM = 0.0;
	h_o_ft = 0.0;
	phi_D = 0.0;
	lambda_D = 0.0;
	a = 0.0;
	e = 0.0;
	i = 0.0;
	omega_p = 0.0;
	Omega = 0.0;
	nu = 0.0;
	m = 0.0;
	R = 0.0;
	R_Day[0] = 0;
	R_Day[1] = 0;
	R_Day[2] = 0;
	A = 0.0;
	K_Fac = 0.0;
	sprintf_s(CFG, "");
	sprintf_s(RF, "ECI");
	WT = 0.0;
	WC = 0.0;
	WL = 0.0;
	SPS = 0.0;
	RCS_C = 0.0;
	APS = 0.0;
	DPS = 0.0;
	RCS_L = 0.0;
	J2 = 0.0;
	LOC = 0.0;
	GRRC = 0.0;
	ZSC = 0.0;
	GRRS = 0.0;
	LAL = 0.0;
	LOL = 0.0;
	ZSL = 0.0;
	ZSA = 0.0;
	EPHB = 0.0;
	EPHE = 0.0;
	UpdateNo = 0;
	NV = 0;
	THT = 0.0;
	RTT = 0.0;
	LSB = 0.0;
	LLS = 0.0;
	deltaL = 0.0;
	VSF = 0.0;
	EB2 = 0.0;
	EE2 = 0.0;
	U_T = _V(-2, 0, 0); //Indicator to blank display (value < -2)
	sprintf_s(Option, "GMT");

	unit = 0;
	TABlank = false;
	LSTBlank = false;
	MABlank = false;
	HOBlank = false;
}

DetailedManeuverTable::DetailedManeuverTable()
{
	sprintf_s(C_STA_ID, "");
	C_GMTV = 0.0;
	C_GETV = 0.0;
	sprintf_s(CODE, "");
	sprintf_s(L_STA_ID, "");
	L_GMTV = 0.0;
	L_GETV = 0.0;
	sprintf_s(REF, "");
	sprintf_s(X_STA_ID, "");
	X_GMTV = 0.0;
	X_GETV = 0.0;
	GETR = 0.0;
	WT = 0.0;
	WC = 0.0;
	WL = 0.0;
	WF = 0.0;
	GETI = 0.0;
	PETI = 0.0;
	DVM = 0.0;
	DVREM = 0.0;
	DVC = 0.0;
	DT_B = 0.0;
	DT_U = 0.0;
	DT_TO = 0.0;
	DV_TO = 0.0;
	sprintf_s(REFSMMAT_Code, "");
	DEL_P = 0.0;
	DEL_Y = 0.0;
	VG = _V(0, 0, 0);
	IMUAtt = _V(0, 0, 0);
	FDAIAtt = _V(0, 0, 0);
	LVLHAtt = _V(0, 0, 0);
	VF = 0.0;
	VS = 0.0;
	VD = 0.0;
	H_BI = 0.0;
	P_BI = 0.0;
	L_BI = 0.0;
	F_BI = 0.0;
	HA = 0.0;
	HP = 0.0;
	L_AN = 0.0;
	E = 0.0;
	I = 0.0;
	WP = 0.0;
	VP = 0.0;
	THETA_P = 0.0;
	DELTA_P = 0.0;
	P_LLS = 0.0;
	L_LLS = 0.0;
	R_LLS = 0.0;
	DH = 0.0;
	PHASE = 0.0;
	PHASE_DOT = 0.0;
	WEDGE_ANG = 0.0;
	YD = 0.0;
	UntilDay = false;
	TimeUntil = 0.0;
	sprintf_s(PGNS_Veh, "");
	sprintf_s(AGS_Veh, "");
	PGNS_GETI = 0.0;
	PGNS_DV = _V(0, 0, 0);
	AGS_GETI = 0.0;
	AGS_DV = _V(0, 0, 0);
	Lam_GETI = 0.0;
	Lam_TF = 0.0;
	Lam_R = _V(0, 0, 0);
	CFP_ID = 0;
	CFP_GETI = 0.0;
	CFP_APSIS = 0;
	CFP_ELEV = 0.0;
	CFP_TPI = 0.0;
	CFP_DT = 0.0;
	sprintf_s(CFP_OPTION, "");
	isCSMTV = true;
	Attitude = 0;
}

TradeoffDataDisplayBuffer::TradeoffDataDisplayBuffer()
{
	curves = 0;
	XLabels[0] = 0;
	XLabels[1] = 0;
	XLabels[2] = 0;
	YLabels[0] = 0;
	YLabels[1] = 0;
	YLabels[2] = 0;
	for (int i = 0;i < 10;i++)
	{
		TZDisplay[i] = 0;
		TZxval[i] = 0.0;
		TZyval[i] = 0.0;
		NumInCurve[i] = 0;
		for (int j = 0;j < 44;j++)
		{
			xval[i][j] = 0.0;
			yval[i][j] = 0.0;
		}
	}
}

void MPTVehicleDataBlock::SaveState(FILEHANDLE scn)
{
	oapiWriteScenario_int(scn, "ConfigCode", ConfigCode.to_ulong());
	oapiWriteScenario_int(scn, "ConfigChangeInd", ConfigChangeInd);
	oapiWriteScenario_int(scn, "TUP", TUP);
	papiWriteScenario_double(scn, "CSMArea", CSMArea);
	papiWriteScenario_double(scn, "SIVBArea", SIVBArea);
	papiWriteScenario_double(scn, "LMAscentArea", LMAscentArea);
	papiWriteScenario_double(scn, "LMDescentArea", LMDescentArea);
	papiWriteScenario_double(scn, "CSMMass", CSMMass);
	papiWriteScenario_double(scn, "SIVBMass", SIVBMass);
	papiWriteScenario_double(scn, "LMAscentMass", LMAscentMass);
	papiWriteScenario_double(scn, "LMDescentMass", LMDescentMass);
	papiWriteScenario_double(scn, "CSMRCSFuelRemaining", CSMRCSFuelRemaining);
	papiWriteScenario_double(scn, "SPSFuelRemaining", SPSFuelRemaining);
	papiWriteScenario_double(scn, "SIVBFuelRemaining", SIVBFuelRemaining);
	papiWriteScenario_double(scn, "LMRCSFuelRemaining", LMRCSFuelRemaining);
	papiWriteScenario_double(scn, "LMAPSFuelRemaining", LMAPSFuelRemaining);
	papiWriteScenario_double(scn, "LMDPSFuelRemaining", LMDPSFuelRemaining);
}
void MPTVehicleDataBlock::LoadState(char *line, int &inttemp)
{
	if (papiReadScenario_int(line, "ConfigCode", inttemp))
	{
		ConfigCode = inttemp;
	}
	papiReadScenario_int(line, "ConfigChangeInd", ConfigChangeInd);
	papiReadScenario_int(line, "TUP", TUP);
	papiReadScenario_double(line, "CSMArea", CSMArea);
	papiReadScenario_double(line, "SIVBArea", SIVBArea);
	papiReadScenario_double(line, "LMAscentArea", LMAscentArea);
	papiReadScenario_double(line, "LMDescentArea", LMDescentArea);
	papiReadScenario_double(line, "CSMMass", CSMMass);
	papiReadScenario_double(line, "SIVBMass", SIVBMass);
	papiReadScenario_double(line, "LMAscentMass", LMAscentMass);
	papiReadScenario_double(line, "LMDescentMass", LMDescentMass);
	papiReadScenario_double(line, "CSMRCSFuelRemaining", CSMRCSFuelRemaining);
	papiReadScenario_double(line, "SPSFuelRemaining", SPSFuelRemaining);
	papiReadScenario_double(line, "SIVBFuelRemaining", SIVBFuelRemaining);
	papiReadScenario_double(line, "LMRCSFuelRemaining", LMRCSFuelRemaining);
	papiReadScenario_double(line, "LMAPSFuelRemaining", LMAPSFuelRemaining);
	papiReadScenario_double(line, "LMDPSFuelRemaining", LMDPSFuelRemaining);
}

MPTManeuver::MPTManeuver()
{
	GMTFrozen = 0.0;
	AttitudeCode = 0;
	Thruster = 0;
	UllageThrusterOpt = false;
	AttitudesInput = false;
	ConfigCodeBefore = 0;
	TVC = 0;
	TrimAngleInd = 0;
	FrozenManeuverInd = false;
	CoordSysInd = 0;
	HeadsUpDownInd = false;
	DockingAngle = 0.0;
	GMTMAN = 0.0;
	dt_ullage = 0.0;
	DT_10PCT = 0.0;
	dt = 0.0;
	dv = 0.0;
	A_T = _V(0, 0, 0);
	X_B = _V(0, 0, 0);
	Y_B = _V(0, 0, 0);
	Z_B = _V(0, 0, 0);
	DPSScaleFactor = 0.0;
	dV_inertial = _V(0, 0, 0);
	dV_LVLH = _V(0, 0, 0);
	Word67d = 0.0;
	Word68 = 0.0;
	Word69 = 0.0;
	Word70 = 0.0;
	Word71 = 0.0;
	Word72 = 0.0;
	Word73 = 0.0;
	Word74 = 0.0;
	Word75 = 0.0;
	Word76 = 0.0;
	Word77 = 0.0;
	Word78d = 0.0;
	Word79 = 0.0;
	Word80 = 0.0;
	Word81 = 0.0;
	Word82 = 0.0;
	Word83 = 0.0;
	Word83 = 0.0;
	Word84 = 0.0;
	GMTI = 0.0;
	TrajDet[0] = 0;
	TrajDet[1] = 0;
	TrajDet[2] = 0;
	R_BI = _V(0, 0, 0);
	V_BI = _V(0, 0, 0);
	GMT_BI = 0.0;
	R_BO = _V(0, 0, 0);
	V_BO = _V(0, 0, 0);
	GMT_BO = 0.0;

	TotalMassAfter = 0.0;
	TotalAreaAfter = 0.0;
	MainEngineFuelUsed = 0.0;
	RCSFuelUsed = 0.0;
	DVREM = 0.0;
	DVC = 0.0;
	DVXBT = 0.0;
	DV_M = 0.0;
	V_F = 0.0, V_S = 0.0, V_D = 0.0;
	P_H = 0.0, Y_H = 0.0, R_H = 0.0;
	dt_BD = 0.0;
	dt_TO = 0.0;
	dv_TO = 0.0;
	P_G = 0.0;
	Y_G = 0.0;
	lat_BI = 0.0;
	lng_BI = 0.0;
	h_BI = 0.0;
	eta_BI = 0.0;
	e_BO = 0.0;
	i_BO = 0.0;
	g_BO = 0.0;
	h_a = 0.0;
	lat_a = 0.0;
	lng_a = 0.0;
	GMT_a = 0.0;
	h_p = 0.0;
	lat_p = 0.0;
	lng_p = 0.0;
	GMT_p = 0.0;
	GMT_AN = 0.0;
	lng_AN = 0.0;
	IMPT = 0.0;
}

void MPTManeuver::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	char Buff[128];

	oapiWriteLine(scn, start_str);
	
	sprintf_s(Buff, code.c_str());
	oapiWriteScenario_string(scn, "code", Buff);
	CommonBlock.SaveState(scn);
	sprintf_s(Buff, StationIDFrozen.c_str());
	oapiWriteScenario_string(scn, "StationIDFrozen", Buff);
	papiWriteScenario_double(scn, "GMTFrozen", GMTFrozen);
	oapiWriteScenario_int(scn, "AttitudeCode", AttitudeCode);
	oapiWriteScenario_int(scn, "Thruster", Thruster);
	oapiWriteScenario_int(scn, "UllageThrusterOpt", UllageThrusterOpt);
	oapiWriteScenario_int(scn, "AttitudesInput", AttitudesInput);
	oapiWriteScenario_int(scn, "ConfigCodeBefore", ConfigCodeBefore.to_ulong());
	oapiWriteScenario_int(scn, "TVC", TVC);
	oapiWriteScenario_int(scn, "TrimAngleInd", TrimAngleInd);
	oapiWriteScenario_int(scn, "FrozenManeuverInd", FrozenManeuverInd);
	oapiWriteScenario_int(scn, "RefBodyInd", RefBodyInd);
	oapiWriteScenario_int(scn, "CoordSysInd", CoordSysInd);
	oapiWriteScenario_int(scn, "HeadsUpDownInd", HeadsUpDownInd);
	papiWriteScenario_double(scn, "DockingAngle", DockingAngle);
	papiWriteScenario_double(scn, "GMTMAN", GMTMAN);
	papiWriteScenario_double(scn, "dt_ullage", dt_ullage);
	papiWriteScenario_double(scn, "DT_10PCT", DT_10PCT);
	papiWriteScenario_double(scn, "dt", dt);
	papiWriteScenario_double(scn, "dv", dv);
	papiWriteScenario_vec(scn, "A_T", A_T);
	papiWriteScenario_vec(scn, "X_B", X_B);
	papiWriteScenario_vec(scn, "Y_B", Y_B);
	papiWriteScenario_vec(scn, "Z_B", Z_B);
	papiWriteScenario_SV(scn, "FrozenManeuverVector", FrozenManeuverVector);
	papiWriteScenario_double(scn, "DPSScaleFactor", DPSScaleFactor);
	papiWriteScenario_vec(scn, "dV_inertial", dV_inertial);
	papiWriteScenario_vec(scn, "dV_LVLH", dV_LVLH);
	if (AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
	{
		papiWriteScenario_double(scn, "Word67d", Word67d);
	}
	else
	{
		papiWriteScenario_intarr(scn, "Word67i", Word67i, 2);
	}
	papiWriteScenario_double(scn, "Word68", Word68);
	papiWriteScenario_double(scn, "Word69", Word69);
	papiWriteScenario_double(scn, "Word70", Word70);
	papiWriteScenario_double(scn, "Word71", Word71);
	papiWriteScenario_double(scn, "Word72", Word72);
	papiWriteScenario_double(scn, "Word73", Word73);
	papiWriteScenario_double(scn, "Word74", Word74);
	papiWriteScenario_double(scn, "Word75", Word75);
	papiWriteScenario_double(scn, "Word76", Word76);
	papiWriteScenario_double(scn, "Word77", Word77);
	if (AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
	{
		papiWriteScenario_intarr(scn, "Word78i", Word78i, 2);
	}
	else
	{
		papiWriteScenario_double(scn, "Word78d", Word78d);
	}
	papiWriteScenario_double(scn, "Word79", Word79);
	papiWriteScenario_double(scn, "Word80", Word80);
	papiWriteScenario_double(scn, "Word81", Word81);
	papiWriteScenario_double(scn, "Word82", Word82);
	papiWriteScenario_double(scn, "Word83", Word83);
	papiWriteScenario_double(scn, "Word84", Word84);
	papiWriteScenario_double(scn, "GMTI", GMTI);
	papiWriteScenario_intarr(scn, "TrajDet", TrajDet, 3);
	papiWriteScenario_vec(scn, "R_BI", R_BI);
	papiWriteScenario_vec(scn, "V_BI", V_BI);
	papiWriteScenario_double(scn, "GMT_BI", GMT_BI);
	papiWriteScenario_vec(scn, "R_BO", R_BO);
	papiWriteScenario_vec(scn, "V_BO", V_BO);
	papiWriteScenario_double(scn, "GMT_BO", GMT_BO);
	papiWriteScenario_vec(scn, "R_1", R_1);
	papiWriteScenario_vec(scn, "V_1", V_1);
	papiWriteScenario_double(scn, "GMT_1", GMT_1);
	papiWriteScenario_double(scn, "TotalMassAfter", TotalMassAfter);
	papiWriteScenario_double(scn, "TotalAreaAfter", TotalAreaAfter);
	papiWriteScenario_double(scn, "MainEngineFuelUsed", MainEngineFuelUsed);
	papiWriteScenario_double(scn, "RCSFuelUsed", RCSFuelUsed);
	papiWriteScenario_double(scn, "DVREM", DVREM);
	papiWriteScenario_double(scn, "DVC", DVC);
	papiWriteScenario_double(scn, "DVXBT", DVXBT);
	papiWriteScenario_double(scn, "DV_M", DV_M);
	papiWriteScenario_double(scn, "V_F", V_F);
	papiWriteScenario_double(scn, "V_S", V_S);
	papiWriteScenario_double(scn, "V_D", V_D);
	papiWriteScenario_double(scn, "P_H", P_H);
	papiWriteScenario_double(scn, "Y_H", Y_H);
	papiWriteScenario_double(scn, "R_H", R_H);
	papiWriteScenario_double(scn, "dt_BD", dt_BD);
	papiWriteScenario_double(scn, "dt_TO", dt_TO);
	papiWriteScenario_double(scn, "dv_TO", dv_TO);
	papiWriteScenario_double(scn, "P_G", P_G);
	papiWriteScenario_double(scn, "Y_G", Y_G);
	papiWriteScenario_double(scn, "lat_BI", lat_BI);
	papiWriteScenario_double(scn, "lng_BI", lng_BI);
	papiWriteScenario_double(scn, "h_BI", h_BI);
	papiWriteScenario_double(scn, "eta_BI", eta_BI);
	papiWriteScenario_double(scn, "e_BO", e_BO);
	papiWriteScenario_double(scn, "i_BO", i_BO);
	papiWriteScenario_double(scn, "g_BO", g_BO);
	papiWriteScenario_double(scn, "h_a", h_a);
	papiWriteScenario_double(scn, "lat_a", lat_a);
	papiWriteScenario_double(scn, "lng_a", lng_a);
	papiWriteScenario_double(scn, "GMT_a", GMT_a);
	papiWriteScenario_double(scn, "h_p", h_p);
	papiWriteScenario_double(scn, "lat_p", lat_p);
	papiWriteScenario_double(scn, "lng_p", lng_p);
	papiWriteScenario_double(scn, "GMT_p", GMT_p);
	papiWriteScenario_double(scn, "GMT_AN", GMT_AN);
	papiWriteScenario_double(scn, "lng_AN", lng_AN);
	papiWriteScenario_double(scn, "IMPT", IMPT);

	oapiWriteLine(scn, end_str);
}

void MPTManeuver::LoadState(FILEHANDLE scn, char *end_str)
{
	char Buff[128];
	char *line;
	int inttemp;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, sizeof(end_str))) {
			break;
		}

		if (papiReadScenario_string(line, "code", Buff))
		{
			code.assign(Buff);
		}
		CommonBlock.LoadState(line, inttemp);
		if (papiReadScenario_string(line, "StationIDFrozen", Buff))
		{
			StationIDFrozen.assign(Buff);
		}
		papiReadScenario_double(line, "GMTFrozen", GMTFrozen);
		papiReadScenario_int(line, "AttitudeCode", AttitudeCode);
		papiReadScenario_int(line, "Thruster", Thruster);
		papiReadScenario_bool(line, "UllageThrusterOpt", UllageThrusterOpt);
		papiReadScenario_bool(line, "AttitudesInput", AttitudesInput);
		if (papiReadScenario_int(line, "ConfigCodeBefore", inttemp))
		{
			ConfigCodeBefore = inttemp;
		}
		papiReadScenario_int(line, "TVC", TVC);
		papiReadScenario_int(line, "TrimAngleInd", TrimAngleInd);
		papiReadScenario_bool(line, "FrozenManeuverInd", FrozenManeuverInd);
		papiReadScenario_int(line, "RefBodyInd", RefBodyInd);
		papiReadScenario_int(line, "CoordSysInd", CoordSysInd);
		papiReadScenario_bool(line, "HeadsUpDownInd", HeadsUpDownInd);
		papiReadScenario_double(line, "DockingAngle", DockingAngle);
		papiReadScenario_double(line, "GMTMAN", GMTMAN);
		papiReadScenario_double(line, "dt_ullage", dt_ullage);
		papiReadScenario_double(line, "DT_10PCT", DT_10PCT);
		papiReadScenario_double(line, "dt", dt);
		papiReadScenario_double(line, "dv", dv);
		papiReadScenario_vec(line, "A_T", A_T);
		papiReadScenario_vec(line, "X_B", X_B);
		papiReadScenario_vec(line, "Y_B", Y_B);
		papiReadScenario_vec(line, "Z_B", Z_B);
		papiReadScenario_SV(line, "FrozenManeuverVector", FrozenManeuverVector);
		papiReadScenario_double(line, "DPSScaleFactor", DPSScaleFactor);
		papiReadScenario_vec(line, "dV_inertial", dV_inertial);
		papiReadScenario_vec(line, "dV_LVLH", dV_LVLH);
		papiReadScenario_double(line, "Word67d", Word67d);
		papiReadScenario_intarr(line, "Word67i", Word67i, 2);
		papiReadScenario_double(line, "Word68", Word68);
		papiReadScenario_double(line, "Word69", Word69);
		papiReadScenario_double(line, "Word70", Word70);
		papiReadScenario_double(line, "Word71", Word71);
		papiReadScenario_double(line, "Word72", Word72);
		papiReadScenario_double(line, "Word73", Word73);
		papiReadScenario_double(line, "Word74", Word74);
		papiReadScenario_double(line, "Word75", Word75);
		papiReadScenario_double(line, "Word76", Word76);
		papiReadScenario_double(line, "Word77", Word77);
		papiReadScenario_double(line, "Word78d", Word78d);
		papiReadScenario_intarr(line, "Word78i", Word78i, 2);
		papiReadScenario_double(line, "Word79", Word79);
		papiReadScenario_double(line, "Word80", Word80);
		papiReadScenario_double(line, "Word81", Word81);
		papiReadScenario_double(line, "Word82", Word82);
		papiReadScenario_double(line, "Word83", Word83);
		papiReadScenario_double(line, "Word84", Word84);
		papiReadScenario_double(line, "GMTI", GMTI);
		papiReadScenario_intarr(line, "TrajDet", TrajDet, 3);
		papiReadScenario_vec(line, "R_BI", R_BI);
		papiReadScenario_vec(line, "V_BI", V_BI);
		papiReadScenario_double(line, "GMT_BI", GMT_BI);
		papiReadScenario_vec(line, "R_BO", R_BO);
		papiReadScenario_vec(line, "V_BO", V_BO);
		papiReadScenario_double(line, "GMT_BO", GMT_BO);
		papiReadScenario_vec(line, "R_1", R_1);
		papiReadScenario_vec(line, "V_1", V_1);
		papiReadScenario_double(line, "GMT_1", GMT_1);
		papiReadScenario_double(line, "TotalMassAfter", TotalMassAfter);
		papiReadScenario_double(line, "TotalAreaAfter", TotalAreaAfter);
		papiReadScenario_double(line, "MainEngineFuelUsed", MainEngineFuelUsed);
		papiReadScenario_double(line, "RCSFuelUsed", RCSFuelUsed);
		papiReadScenario_double(line, "DVREM", DVREM);
		papiReadScenario_double(line, "DVC", DVC);
		papiReadScenario_double(line, "DVXBT", DVXBT);
		papiReadScenario_double(line, "DV_M", DV_M);
		papiReadScenario_double(line, "V_F", V_F);
		papiReadScenario_double(line, "V_S", V_S);
		papiReadScenario_double(line, "V_D", V_D);
		papiReadScenario_double(line, "P_H", P_H);
		papiReadScenario_double(line, "Y_H", Y_H);
		papiReadScenario_double(line, "R_H", R_H);
		papiReadScenario_double(line, "dt_BD", dt_BD);
		papiReadScenario_double(line, "dt_TO", dt_TO);
		papiReadScenario_double(line, "dv_TO", dv_TO);
		papiReadScenario_double(line, "P_G", P_G);
		papiReadScenario_double(line, "Y_G", Y_G);
		papiReadScenario_double(line, "lat_BI", lat_BI);
		papiReadScenario_double(line, "lng_BI", lng_BI);
		papiReadScenario_double(line, "h_BI", h_BI);
		papiReadScenario_double(line, "eta_BI", eta_BI);
		papiReadScenario_double(line, "e_BO", e_BO);
		papiReadScenario_double(line, "i_BO", i_BO);
		papiReadScenario_double(line, "g_BO", g_BO);
		papiReadScenario_double(line, "h_a", h_a);
		papiReadScenario_double(line, "lat_a", lat_a);
		papiReadScenario_double(line, "lng_a", lng_a);
		papiReadScenario_double(line, "GMT_a", GMT_a);
		papiReadScenario_double(line, "h_p", h_p);
		papiReadScenario_double(line, "lat_p", lat_p);
		papiReadScenario_double(line, "lng_p", lng_p);
		papiReadScenario_double(line, "GMT_p", GMT_p);
		papiReadScenario_double(line, "GMT_AN", GMT_AN);
		papiReadScenario_double(line, "lng_AN", lng_AN);
		papiReadScenario_double(line, "IMPT", IMPT);
	}
}

MPTManDisplay::MPTManDisplay()
{
	DELTAV = 0.0;
	DVREM = 0.0;
	HA = 0.0;
	HP = 0.0;
}

StationContact::StationContact()
{
	GMTAOS = 0.0;
	GMTLOS = 0.0;
	GMTEMAX = 0.0;
	MAXELEV = 0.0;
	BestAvailableAOS = false;
	BestAvailableLOS = false;
	BestAvailableEMAX = false;
	REV = 0;
}

bool StationContact::operator<(const StationContact& rhs) const
{
	if (GMTAOS == rhs.GMTAOS)
	{
		return GMTLOS < rhs.GMTLOS;
	}

	return GMTAOS < rhs.GMTAOS;
}

NextStationContactsTable::NextStationContactsTable()
{
	GET = 0.0;
	for (int i = 0;i < 2;i++)
	{
		for (int j = 0;j < 6;j++)
		{
			GETHCA[i][j] = 0.0;
			DTKLOS[i][j] = 0.0;
			EMAX[i][j] = 0.0;
			DTPASS[i][j] = 0.0;
			DTKH[i][j] = 0.0;
			BestAvailableAOS[i][j] = false;
			BestAvailableEMAX[i][j] = false;
		}
	}
}

PredictedSiteAcquisitionTable::PredictedSiteAcquisitionTable()
{
	curpage = 1;
	pages = 1;
	for (int i = 0;i < 2;i++)
	{
		numcontacts[i] = 0;
		for (int j = 0;j < 21;j++)
		{
			REV[i][j] = 0;
			GETHCA[i][j] = 0.0;
			GETHCD[i][j] = 0.0;
			ELMAX[i][j] = 0.0;
			BestAvailableAOS[i][j] = false;
			BestAvailableLOS[i][j] = false;
			BestAvailableEMAX[i][j] = false;
		}
	}
}

ExperimentalSiteAcquisitionTable::ExperimentalSiteAcquisitionTable()
{
	curpage = 1;
	pages = 1;
	for (int i = 0;i < 2;i++)
	{
		numcontacts[i] = 0;
		for (int j = 0;j < 20;j++)
		{
			REV[i][j] = 0;
			GETAOS[i][j] = 0.0;
			GETLOS[i][j] = 0.0;
			GNDRNG[i][j] = 0.0;
			ALT[i][j] = 0.0;
			GETCA[i][j] = 0.0;
			ELMAX[i][j] = 0.0;
			BestAvailableAOS[i][j] = false;
			BestAvailableLOS[i][j] = false;
			BestAvailableEMAX[i][j] = false;
		}
	}
}

LandmarkAcquisitionTable::LandmarkAcquisitionTable()
{
	curpage = 1;
	pages = 1;
	TUN = 0;
	err = 1;
	for (int i = 0;i < 3;i++)
	{
		numcontacts[i] = 0;
		for (int j = 0;j < 20;j++)
		{
			STAID[i][j] = "";
			GETAOS[i][j] = 0.0;
			GETLOS[i][j] = 0.0;
			GETCA[i][j] = 0.0;
			GETSR[i][j] = 0.0;
			GETSS[i][j] = 0.0;
			BestAvailableAOS[i][j] = false;
			BestAvailableLOS[i][j] = false;
			BestAvailableCA[i][j] = false;
			Lambda[i][j] = 0.0;
			h[i][j] = 0.0;
		}
	}
}

CapeCrossingTable::CapeCrossingTable()
{
	ref_body = -1;
	TUP = 0;
	NumRev = 0;
	NumRevFirst = 0;
	NumRevLast = 0;
	GMTEphemFirst = 0.0;
	GMTEphemLast = 0.0;
	GMTCrossPrev = 0.0;
	for (int i = 0;i < 30;i++)
	{
		GMTCross[i] = 0.0;
	}
}

LunarLiftoffTimeOpt::LunarLiftoffTimeOpt()
{
	theta_1 = 17.0*RAD;
	dt_1 = 7.0*60.0 + 15.0;
	DH_SRCH = 15.0*1852.0;
	v_LH = 5509.5*0.3048;
	v_LV = 19.5*0.3048;
	theta_F = 130.0*RAD;
	E = 26.6*RAD;
	I_TPI = 1;
}

LunarDescentPlanningTable::LunarDescentPlanningTable()
{
	LMWT = 0.0;
	GMTV = 0.0;
	GETV = 0.0;
	MODE = 0;
	LAT_LLS = 0.0;
	LONG_LLS = 0.0;
	for (int i = 0;i < 4;i++)
	{
	GETTH[i] = 0.0;
	GETIG[i] = 0.0;
	LIG[i] = 0.0;
	DV[i] = 0.0;
	AC[i] = 0.0;
	HPC[i] = 0.0;
	DEL[i] = 0.0;
	THPC[i] = 0.0;
	DVVector[i] = _V(0, 0, 0);
	}
	PD_ThetaIgn = 0.0;
	PD_PropRem = 0.0;
	PD_GETTH = 0.0;
	PD_GETIG = 0.0;
	PD_GETTD = 0.0;
	sprintf(DescAzMode, "");
	DescAsc = 0.0;
	SN_LK_A = 0.0;
}

MissionPlanTable::MissionPlanTable()
{
	for (int i = 0;i < 15;i++)
	{
		TimeToBeginManeuver[i] = 0.0;
		TimeToEndManeuver[i] = 0.0;
		AreaAfterManeuver[i] = 0.0;
		WeightAfterManeuver[i] = 0.0;
	}
}

void MissionPlanTable::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	char Buff[128], Buff2[128];
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_int(scn, "ManeuverNum", ManeuverNum);
	sprintf_s(Buff, StationID.c_str());
	oapiWriteScenario_string(scn, "StationID", Buff);
	papiWriteScenario_double(scn, "GMTAV", GMTAV);
	papiWriteScenario_double(scn, "KFactor", KFactor);
	papiWriteScenario_double(scn, "LMStagingGMT", LMStagingGMT);
	sprintf(Buff, "  %s %.12g", "UpcomingManeuverGMT", UpcomingManeuverGMT);
	oapiWriteLine(scn, Buff);
	papiWriteScenario_double(scn, "SIVBVentingBeginGET", SIVBVentingBeginGET);
	CommonBlock.SaveState(scn);
	papiWriteScenario_double(scn, "TotalInitMass", TotalInitMass);
	papiWriteScenario_double(scn, "ConfigurationArea", ConfigurationArea);
	papiWriteScenario_double(scn, "DeltaDockingAngle", DeltaDockingAngle);
	if (ManeuverNum > 0)
	{
		papiWriteScenario_doublearr(scn, "TimeToBeginManeuver", TimeToBeginManeuver, ManeuverNum);
		papiWriteScenario_doublearr(scn, "TimeToEndManeuver", TimeToEndManeuver, ManeuverNum);
		papiWriteScenario_doublearr(scn, "AreaAfterManeuver", AreaAfterManeuver, ManeuverNum);
		papiWriteScenario_doublearr(scn, "WeightAfterManeuver", WeightAfterManeuver, ManeuverNum);
	}
	oapiWriteScenario_int(scn, "LastFrozenManeuver", LastFrozenManeuver);
	oapiWriteScenario_int(scn, "LastExecutedManeuver", LastExecutedManeuver);
	for (unsigned i = 0;i < ManeuverNum;i++)
	{
		sprintf_s(Buff, "MAN%d_BEGIN", i + 1);
		sprintf_s(Buff2, "MAN%d_END", i + 1);
		mantable[i].SaveState(scn, Buff, Buff2);
	}
	oapiWriteLine(scn, end_str);
}

void MissionPlanTable::LoadState(FILEHANDLE scn, char *end_str)
{
	char Buff[128], manbuff[16], manbuff2[16];
	char *line;
	int inttemp;
	unsigned int mannum;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, sizeof(end_str))) {
			break;
		}

		if (!strnicmp(line, "ManeuverNum", 11)) {
			sscanf(line + 11, "%d", &ManeuverNum);
			//ManeuverNum should only be 0 to 15
			if (ManeuverNum <= 15)
			{
				mantable.resize(ManeuverNum);
				mannum = 0;
				sprintf_s(manbuff, "MAN%d_BEGIN", mannum + 1);
				sprintf_s(manbuff2, "MAN%d_END", mannum + 1);
			}
			else
			{
				//For safety
				ManeuverNum = 0;
			}
		}
		else if (papiReadScenario_string(line, "StationID", Buff))
		{
			StationID.assign(Buff);
		}

		papiReadScenario_double(line, "GMTAV", GMTAV);
		papiReadScenario_double(line, "KFactor", KFactor);
		papiReadScenario_double(line, "LMStagingGMT", LMStagingGMT);
		papiReadScenario_double(line, "UpcomingManeuverGMT", UpcomingManeuverGMT);
		papiReadScenario_double(line, "SIVBVentingBeginGET", SIVBVentingBeginGET);
		CommonBlock.LoadState(line, inttemp);
		papiReadScenario_double(line, "TotalInitMass", TotalInitMass);
		papiReadScenario_double(line, "ConfigurationArea", ConfigurationArea);
		papiReadScenario_double(line, "DeltaDockingAngle", DeltaDockingAngle);
		if (ManeuverNum > 0)
		{
			papiReadScenario_doublearr(line, "TimeToBeginManeuver", TimeToBeginManeuver, ManeuverNum);
			papiReadScenario_doublearr(line, "TimeToEndManeuver", TimeToEndManeuver, ManeuverNum);
			papiReadScenario_doublearr(line, "AreaAfterManeuver", AreaAfterManeuver, ManeuverNum);
			papiReadScenario_doublearr(line, "WeightAfterManeuver", WeightAfterManeuver, ManeuverNum);
		}
		if (papiReadScenario_int(line, "LastFrozenManeuver", inttemp))
		{
			LastFrozenManeuver = inttemp;
		}
		else if (papiReadScenario_int(line, "LastExecutedManeuver", inttemp))
		{
			LastExecutedManeuver = inttemp;
		}
		if (ManeuverNum > 0)
		{
			if (!strnicmp(line, manbuff, sizeof(manbuff)))
			{
				if (mannum < mantable.size())
				{
					mantable[mannum].LoadState(scn, manbuff2);
					mannum++;
					sprintf_s(manbuff, "MAN%d_BEGIN", mannum + 1);
					sprintf_s(manbuff2, "MAN%d_END", mannum + 1);
				}
			}
		}
	}
}

RTCC::RTEConstraintsTable::RTEConstraintsTable()
{
	DVMAX = 10000.0;
	TZMIN = 0.0;
	TZMAX = 350.0;
	GMAX = 4.0;
	HMINMC = 50.0;
	IRMAX = 40.0;
	RRBIAS = 1285.0;
	VRMAX = 36323.0;
	VECID = 0;
	TGTLN = 1;
	VECTYPE = 7;
	MOTION = 0;
	for (int i = 0;i < 5;i++)
	{
		PTPLatitude[i] = 0.0;
		PTPLongitude[i] = 0.0;
		for (int j = 0;j < 10;j++)
		{
			ATPCoordinates[i][j] = 1e10;
		}
	}

	//0 = Mid Pacific, 1 = East Pacific, 2 = Atlantic Ocean, 3 = Indian Ocean, 4 = West Pacific

	//MPL
	ATPSite[0] = "MPL";
	ATPCoordinates[0][0] = 40.0*RAD;
	ATPCoordinates[0][1] = -175.0*RAD;
	ATPCoordinates[0][2] = 15.0*RAD;
	ATPCoordinates[0][3] = -175.0*RAD;
	ATPCoordinates[0][4] = 0.0*RAD;
	ATPCoordinates[0][5] = -165.0*RAD;
	ATPCoordinates[0][6] = -40.0*RAD;
	ATPCoordinates[0][7] = -165.0*RAD;

	//EPL
	ATPSite[1] = "EPL";
	ATPCoordinates[1][0] = 40.0*RAD;
	ATPCoordinates[1][1] = -135.0*RAD;
	ATPCoordinates[1][2] = 21.0*RAD;
	ATPCoordinates[1][3] = -122.0*RAD;
	ATPCoordinates[1][4] = -11.0*RAD;
	ATPCoordinates[1][5] = -89.0*RAD;
	ATPCoordinates[1][6] = -40.0*RAD;
	ATPCoordinates[1][7] = -83.0*RAD;

	//AOL
	ATPSite[2] = "AOL";
	ATPCoordinates[2][0] = 40.0*RAD;
	ATPCoordinates[2][1] = -30.0*RAD;
	ATPCoordinates[2][2] = 10.0*RAD;
	ATPCoordinates[2][3] = -30.0*RAD;
	ATPCoordinates[2][4] = -5.0*RAD;
	ATPCoordinates[2][5] = -25.0*RAD;
	ATPCoordinates[2][6] = -40.0*RAD;
	ATPCoordinates[2][7] = -25.0*RAD;

	//IOL
	ATPSite[3] = "IOL";
	ATPCoordinates[3][0] = 15.0*RAD;
	ATPCoordinates[3][1] = 65.0*RAD;
	ATPCoordinates[3][2] = -40.0*RAD;
	ATPCoordinates[3][3] = 65.0*RAD;

	//WPL
	ATPSite[4] = "WPL";
	ATPCoordinates[4][0] = 40.0*RAD;
	ATPCoordinates[4][1] = 150.0*RAD;
	ATPCoordinates[4][2] = 10.0*RAD;
	ATPCoordinates[4][3] = 150.0*RAD;
	ATPCoordinates[4][4] = -15.0*RAD;
	ATPCoordinates[4][5] = 170.0*RAD;
	ATPCoordinates[4][6] = -40.0*RAD;
	ATPCoordinates[4][7] = 170.0*RAD;

	sprintf_s(RTEManeuverCode, "CSU");
}

RTCC::RendezvousEvaluationDisplay::RendezvousEvaluationDisplay()
{
	ID = 0;
	M = 0;
	NumMans = 0;
	isDKI = false;
	for (int i = 0;i < 5;i++)
	{
		GET[i] = 0.0;
		if (i > 0)
		{
			DT[i - 1] = 0.0;
		}
		DV[i] = 0.0;
		CODE[i] = 0.0;
		PHASE[i] = 0.0;
		HEIGHT[i] = 0.0;
		HA[i] = 0.0;
		HP[i] = 0.0;
		Pitch[i] = 0.0;
		Yaw[i] = 0.0;
		DVVector[i] = _V(0, 0, 0);
	}
}

RTCC::RendezvousPlanningDisplayData::RendezvousPlanningDisplayData()
{
	NC1 = 0.0;
	NH = 0.0;
	NSR = 0.0;
	NCC = 0.0;
	GETTPI = 0.0;
	NPC = 0.0;
	ID = 0;
	M = 0;
}

SpacecraftSettingTable::SpacecraftSettingTable()
{
	Indicator = 1;
	IsRTE = false;
	lat_T = 0.0;
	lng_T = 0.0;
}

void SpacecraftSettingTable::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	oapiWriteLine(scn, start_str);

	oapiWriteScenario_int(scn, "Indicator", Indicator);
	oapiWriteScenario_int(scn, "IsRTE", IsRTE);
	papiWriteScenario_double(scn, "lat_T", lat_T);
	papiWriteScenario_double(scn, "lng_T", lng_T);

	oapiWriteLine(scn, end_str);
}

void SpacecraftSettingTable::LoadState(FILEHANDLE scn, char *end_str)
{
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, sizeof(end_str))) {
			break;
		}

		papiReadScenario_int(line, "Indicator", Indicator);
		papiReadScenario_bool(line, "IsRTE", IsRTE);
		papiReadScenario_double(line, "lat_T", lat_T);
		papiReadScenario_double(line, "lng_T", lng_T);
	}
}

RTCC::RTCC() :
	pmmlaeg(this)
{
	mcc = NULL;
	RTCC_GreenwichMeanTime = 0.0;
	RTCC_CSM_GroundElapsedTime = 0.0;
	RTCC_LM_GroundElapsedTime = 0.0;
	TimeofIgnition = 0.0;
	SplashLatitude = 0.0;
	SplashLongitude = 0.0;
	DeltaV_LVLH = _V(0.0, 0.0, 0.0);
	calcParams.EI = 0.0;
	calcParams.TEI = 0.0;
	calcParams.TLI = 0.0;
	calcParams.LOI = 0.0;
	calcParams.SEP = 0.0;
	calcParams.DOI = 0.0;
	calcParams.PDI = 0.0;
	calcParams.LSAzi = 0.0;
	calcParams.LunarLiftoff = 0.0;
	calcParams.Insertion = 0.0;
	calcParams.Phasing = 0.0;
	calcParams.CSI = 0.0;
	calcParams.CDH = 0.0;
	calcParams.TPI = 0.0;
	calcParams.src = NULL;
	calcParams.tgt = NULL;
	calcParams.StoredREFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
	calcParams.TIGSTORE1 = 0.0;
	calcParams.DVSTORE1 = _V(0, 0, 0);
	calcParams.SVSTORE1.gravref = NULL;
	calcParams.SVSTORE1.mass = 0.0;
	calcParams.SVSTORE1.MJD = 0.0;
	calcParams.SVSTORE1.R = _V(0, 0, 0);
	calcParams.SVSTORE1.V = _V(0, 0, 0);

	EZJGMTX1.data[RTCC_REFSMMAT_TYPE_CUR - 1].REFSMMAT = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);
	EZJGMTX3.data[RTCC_REFSMMAT_TYPE_CUR - 1].REFSMMAT = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);
	EZJGMTX3.data[RTCC_REFSMMAT_TYPE_AGS - 1].REFSMMAT = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	BZLAND.lat[RTCC_LMPOS_BEST] = 0.0;
	BZLAND.lng[RTCC_LMPOS_BEST] = 0.0;
	BZLAND.rad[RTCC_LMPOS_BEST] = SystemParameters.MCSMLR;

	PZLTRT.DT_Ins_TPI = PZLTRT.DT_Ins_TPI_NOM = 40.0*60.0;
	GZGENCSN.TIPhaseAngle = 0.0;

	//Initialize MPT areas
	PZMPTCSM.CommonBlock.ConfigCode = PZMPTLEM.CommonBlock.ConfigCode = 15;
	PZMPTCSM.CommonBlock.CSMArea = PZMPTLEM.CommonBlock.CSMArea = 129.4*0.3048*0.3048;
	PZMPTCSM.CommonBlock.LMAscentArea = PZMPTLEM.CommonBlock.LMAscentArea = 200.6*0.3048*0.3048;
	PZMPTCSM.CommonBlock.LMDescentArea = PZMPTLEM.CommonBlock.LMDescentArea = 200.6*0.3048*0.3048;
	PZMPTCSM.CommonBlock.SIVBArea = PZMPTLEM.CommonBlock.SIVBArea = 368.7*0.3048*0.3048;
	PZMPTCSM.ConfigurationArea = PZMPTLEM.ConfigurationArea = PZMPTCSM.CommonBlock.SIVBArea;
	PZMPTCSM.KFactor = PZMPTLEM.KFactor = 1.0;

	//Initialize MPT usable propellant
	PZMPTCSM.CommonBlock.SPSFuelRemaining = PZMPTLEM.CommonBlock.SPSFuelRemaining = 40200.0 / (LBS*1000.0);
	PZMPTCSM.CommonBlock.CSMRCSFuelRemaining = PZMPTLEM.CommonBlock.CSMRCSFuelRemaining = 1228.0 / (LBS*1000.0);
	PZMPTCSM.CommonBlock.SIVBFuelRemaining = PZMPTLEM.CommonBlock.SIVBFuelRemaining = 164337.0 / (LBS*1000.0);
	PZMPTCSM.CommonBlock.LMAPSFuelRemaining = PZMPTLEM.CommonBlock.LMAPSFuelRemaining = 5177.0 / (LBS*1000.0);
	PZMPTCSM.CommonBlock.LMDPSFuelRemaining = PZMPTLEM.CommonBlock.LMDPSFuelRemaining = 17921.0 / (LBS*1000.0);
	PZMPTCSM.CommonBlock.LMRCSFuelRemaining = PZMPTLEM.CommonBlock.LMRCSFuelRemaining = 549.0 / (LBS*1000.0);

	//Initialize MPT weights
	PZMPTCSM.CommonBlock.CSMMass = PZMPTLEM.CommonBlock.CSMMass = 29021.0;
	PZMPTCSM.CommonBlock.LMAscentMass = PZMPTLEM.CommonBlock.LMAscentMass = 4944.0;
	PZMPTCSM.CommonBlock.LMDescentMass = PZMPTLEM.CommonBlock.LMDescentMass = 10375.0;
	PZMPTCSM.CommonBlock.SIVBMass = PZMPTLEM.CommonBlock.SIVBMass = 90851.2;
	PZMPTCSM.TotalInitMass = PZMPTLEM.TotalInitMass = PZMPTCSM.CommonBlock.CSMMass + PZMPTCSM.CommonBlock.LMAscentMass + PZMPTCSM.CommonBlock.LMDescentMass + PZMPTCSM.CommonBlock.SIVBMass;

	PZMPTCSM.CommonBlock.ConfigCode = PZMPTLEM.CommonBlock.ConfigCode = 15; //CSM + LM + S-IVB

	//Load station characteristics table
	GZSTCH[0].data.code = "BDA";
	GZSTCH[0].RadarMount = 2;
	GZSTCH[0].SiteType = 13;
	EMGGPCHR(32.3528*RAD, -64.6592*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[0].data);

	GZSTCH[1].data.code = "CYI";
	GZSTCH[1].SiteType = 13;
	EMGGPCHR(27.74055*RAD, -15.60077*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[1].data);

	GZSTCH[2].data.code = "CRO";
	GZSTCH[2].SiteType = 13;
	EMGGPCHR(-24.90619*RAD, 113.72595*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[2].data);

	GZSTCH[3].data.code = "HSK";
	GZSTCH[3].SiteType = 8;
	EMGGPCHR(-35.40282*RAD, 148.98144*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[3].data);

	GZSTCH[4].data.code = "GYM";
	GZSTCH[4].SiteType = 13;
	EMGGPCHR(27.95029*RAD, -110.90846*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[4].data);

	GZSTCH[5].data.code = "HAW";
	GZSTCH[5].SiteType = 13;
	EMGGPCHR(21.44719*RAD, -157.76307*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[5].data);

	GZSTCH[6].data.code = "VAN";
	GZSTCH[6].SiteType = 13;
	EMGGPCHR(32.7*RAD, -48.0*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[6].data);

	GZSTCH[7].data.code = "ASC";
	GZSTCH[7].SiteType = 13;
	EMGGPCHR(-7.94354*RAD, -14.37105*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[7].data);

	GZSTCH[8].data.code = "GWM";
	GZSTCH[8].SiteType = 13;
	EMGGPCHR(13.30929*RAD, 144.73694*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[8].data);

	GZSTCH[9].data.code = "MIL";
	GZSTCH[9].SiteType = 13;
	EMGGPCHR(28.40433*RAD, -80.60192*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[9].data);

	GZSTCH[10].data.code = "TEX";
	GZSTCH[10].SiteType = 13;
	EMGGPCHR(27.65273*RAD, -97.37588*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[10].data);

	GZSTCH[11].data.code = "GBM";
	GZSTCH[11].SiteType = 13;
	EMGGPCHR(26.62022*RAD, -78.35825*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[11].data);

	GZSTCH[12].data.code = "ANG";
	GZSTCH[12].SiteType = 13;
	EMGGPCHR(17.137222*RAD, -61.775833*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[12].data);

	GZSTCH[13].data.code = "TAN";
	GZSTCH[13].SiteType = 13;
	EMGGPCHR(-19.00000*RAD, 47.27556*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[13].data);

	GZSTCH[14].data.code = "MER";
	GZSTCH[14].SiteType = 13;
	EMGGPCHR(25.0*RAD, 125.0*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[14].data);

	GZSTCH[15].data.code = "HTV";
	GZSTCH[15].SiteType = 13;
	EMGGPCHR(25.0*RAD, -136.0*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[15].data);

	GZSTCH[16].data.code = "RED";
	GZSTCH[16].SiteType = 13;
	EMGGPCHR(-24.0*RAD, -118.0*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[16].data);

	GZSTCH[17].data.code = "GDS";
	GZSTCH[17].SiteType = 8;
	EMGGPCHR(35.33820*RAD, -116.87421*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[17].data);

	GZSTCH[18].data.code = "MAD";
	GZSTCH[18].SiteType = 8;
	EMGGPCHR(40.45443*RAD, -4.16990*RAD, 0.0, BODY_EARTH, 0.0, &GZSTCH[18].data);

	SystemParameters.MKRBKS = 19;

	//Set up the yearly coordinate system (default AGCEpoch = 1969)
	InitializeCoordinateSystem();
}

RTCC::~RTCC()
{
}

void RTCC::Init(MCC *ptr)
{
	mcc = ptr;
}

void RTCC::Timestep(double simt, double simdt, double mjd)
{
	//UPDATE TIME KEEPING
	TimeUpdate();

	if (SystemParameters.GMTBASE != 0.0) //Launch day has been initialized
	{
		//MPT Maneuver Execution Logic
		if (PZMPTCSM.ManeuverNum > 0)
		{
			if (RTCC_GreenwichMeanTime >= PZMPTCSM.UpcomingManeuverGMT)
			{
				PMSEXE(RTCC_MPT_CSM, RTCC_GreenwichMeanTime);
			}
		}
		if (PZMPTLEM.ManeuverNum > 0)
		{
			if (RTCC_GreenwichMeanTime >= PZMPTLEM.UpcomingManeuverGMT)
			{
				PMSEXE(RTCC_MPT_LM, RTCC_GreenwichMeanTime);
			}
		}
	}
}

void RTCC::LoadLaunchDaySpecificParameters(int year, int month, int day)
{
	//Mission initialization parameters
	LoadMissionInitParameters(year, month, day);
	//Solar and lunar ephemerides
	QMEPHEM(SystemParameters.AGCEpoch, year, month, day, 0.0);
}

int RTCC::QMSEARCH(std::string file)
{
	//This function loads the skeleton flight plan for the launch month
	//The format is from 69-FM-171 (NTRS ID 19740072570)
	//Each line in the file is equivalent to a punch card
	//For each data set 8 punch cards are used
	//There is a data set for each launch day, launch azimuth, TLI opportunity
	//FORMAT:
	//Card 1: Day (1 to 366), Opportunity (1 or 2), Launch Azimuth (deg), Launch Window (A or P)
	//Card 2: Latitude of TLI pericynthion (deg), Longitude of TLI pericynthion (deg), Height of TLI pericynthion (NM), GET of TLI (hours)
	//Card 3: Latitude of LOI pericynthion (deg), Longitude of LOI pericynthion (deg), Height of LOI pericynthion (NM), spare
	//Card 4: dpsi of LOI (deg), gamma of LOI (deg), Time in lunar orbit (hrs), Time from LOI to landing (hrs)
	//Card 5: Approach azimuth (deg), latitude of the landing site (deg), longitude of the landing site (deg), Radius of the landing site (NM)
	//Card 6: dpsi of TEI (deg), DV of TEI (ft/s), time from TEI to EI (hrs), inclination of free return (deg)
	//Card 7: GMT of TLI pericynthion (hrs), GMT of LOI pericynthion (hrs), GMT of node (hrs), spare
	//Card 8: Latitude of node (deg), Longitude of node (deg), height of node (NM), spare

	char Buff[128];
	int card, day, azimuth, opportunity, err;

	sprintf_s(Buff, ".\\Config\\ProjectApollo\\RTCC\\%s.txt", file.c_str());

	ifstream sfptable(Buff);

	if (sfptable.is_open() == false) return 1;

	std::string line, CardID;
	double dtemp[32];
	int itemp[2];
	unsigned i;

	MasterFlightPlanTable NewTable;
	
	while (getline(sfptable, line))
	{
		//Cards limited to 80 columns
		line = line.substr(0, 80);
		if (line.size() != 80)
		{
			err = 1;
			break;
		}
		//Identification number is the last 11 characters of the card
		CardID = line.substr(69, 11);

		//Get data from identification code
		day = std::stoi(CardID.substr(2, 3));
		opportunity = std::stoi(CardID.substr(5, 1));
		azimuth = std::stoi(CardID.substr(6, 3));
		card = std::stoi(CardID.substr(10, 1));

		switch (card)
		{
		case 1:
			itemp[0] = std::stoi(line.substr(0, 17)); //Day
			itemp[1] = std::stoi(line.substr(17, 17)); //Opportunity
			dtemp[0] = std::stod(line.substr(34, 17)); //Launch azimuth (deg)
			break;
		case 2:
			dtemp[1] = std::stod(line.substr(0, 17)); //Latitude of TLI pericynthion (deg)
			dtemp[2] = std::stod(line.substr(17, 17)); //Longitude of TLI pericynthion (deg)
			dtemp[3] = std::stod(line.substr(34, 17)); //Height of TLI pericynthion (NM)
			dtemp[4] = std::stod(line.substr(51, 17)); //GET of TLI (hrs)
			break;
		case 3:
			dtemp[5] = std::stod(line.substr(0, 17)); //Latitude of LOI pericynthion (deg)
			dtemp[6] = std::stod(line.substr(17, 17)); //Longitude of LOI pericynthion (deg)
			dtemp[7] = std::stod(line.substr(34, 17)); //Height of LOI pericynthion (NM)
			break;
		case 4:
			dtemp[8] = std::stod(line.substr(0, 17)); //dpsi of LOI (deg)
			dtemp[9] = std::stod(line.substr(17, 17)); //gamma of LOI (deg)
			dtemp[10] = std::stod(line.substr(34, 17)); //Time in lunar orbit (hrs)
			dtemp[11] = std::stod(line.substr(51, 17)); //Time from LOI to landing (hrs)
			break;
		case 5:
			dtemp[12] = std::stod(line.substr(0, 17)); //Approach azimuth (deg)
			dtemp[13] = std::stod(line.substr(17, 17)); //latitude of the landing site (deg) 
			dtemp[14] = std::stod(line.substr(34, 17)); //longitude of the landing site (deg)
			dtemp[15] = std::stod(line.substr(51, 17)); //Radius of the landing site (NM)
			break;
		case 6:
			dtemp[16] = std::stod(line.substr(0, 17)); //dpsi of TEI (deg)
			dtemp[17] = std::stod(line.substr(17, 17)); //DV of TEI (ft/s)
			dtemp[18] = std::stod(line.substr(34, 17)); //time from TEI to EI (hrs)
			dtemp[19] = std::stod(line.substr(51, 17)); //inclination of free return (deg)
			break;
		case 7:
			dtemp[20] = std::stod(line.substr(0, 17)); //GMT of TLI pericynthion (hrs)
			dtemp[21] = std::stod(line.substr(17, 17)); //GMT of LOI pericynthion (hrs)
			dtemp[22] = std::stod(line.substr(34, 17)); //GMT of node(hrs)
			break;
		case 8:
			dtemp[23] = std::stod(line.substr(0, 17)); //Latitude of node (deg)
			dtemp[24] = std::stod(line.substr(17, 17)); //Longitude of node (deg)
			dtemp[25] = std::stod(line.substr(34, 17)); //height of node (NM)
			break;
		default:
			err = 2;
			break;
		}

		//All cards read, process data
		if (card == 8)
		{
			bool found;

			//Does day already exist?
			unsigned daynum = 0U;
			found = false;

			if (NewTable.data.size() > 0)
			{
				for (i = 0; i < NewTable.data.size(); i++)
				{
					if (NewTable.data[i].day == day)
					{
						found = true;
						daynum = i;
						break;
					}
				}
			}

			if (found == false)
			{
				NewTable.data.push_back(MasterFlightPlanTableDay());
				NewTable.data.back().day = day;
				daynum = NewTable.data.size() - 1U;
			}

			MasterFlightPlanTableDay *pDay = &NewTable.data[daynum];

			//Does opportunity already exist?
			unsigned oppnum = 0U;
			found = false;

			if (pDay->data.size() > 0)
			{
				for (i = 0; i < pDay->data.size(); i++)
				{
					if (pDay->data[i].Opportunity == opportunity)
					{
						found = true;
						oppnum = i;
						break;
					}
				}
			}

			if (found == false)
			{
				pDay->data.push_back(MasterFlightPlanTableOpportunity());
				pDay->data.back().Opportunity = opportunity;
				oppnum = pDay->data.size() - 1U;
			}

			MasterFlightPlanTableOpportunity *pOpp = &pDay->data[oppnum];

			//Does azimuth already exist?
			unsigned azimuthnum = 0U;
			found = false;

			if (pOpp->data.size() > 0)
			{
				for (i = 0; i < pOpp->data.size(); i++)
				{
					if (pOpp->data[i].iAzimuth == azimuth)
					{
						found = true;
						azimuthnum = i;
						break;
					}
				}
			}

			if (found == false)
			{
				pOpp->data.push_back(MasterFlightPlanTableAzimuth());
				pOpp->data.back().iAzimuth = azimuth;
				pOpp->data.back().dAzimuth = dtemp[0] * RAD;
				azimuthnum = pOpp->data.size() - 1U;
			}

			//Now write data
			TLMCCDataTable *pTab = &pOpp->data[azimuthnum].data;

			pTab->lat_pc1 = dtemp[1] * RAD;
			pTab->lng_pc1 = dtemp[2] * RAD;
			pTab->h_pc1 = dtemp[3] * 1852.0;
			pTab->GET_TLI = dtemp[4] * 3600.0;
			pTab->lat_pc2 = dtemp[5] * RAD;
			pTab->lng_pc2 = dtemp[6] * RAD;
			pTab->h_pc2 = dtemp[7] * 1852.0;
			pTab->dpsi_loi = dtemp[8] * RAD;
			pTab->gamma_loi = dtemp[9] * RAD;
			pTab->T_lo = dtemp[10] * 3600.0;
			pTab->dt_lls = dtemp[11] * 3600.0;
			pTab->psi_lls = dtemp[12] * RAD;
			pTab->lat_lls = dtemp[13] * RAD;
			pTab->lng_lls = dtemp[14] * RAD;
			pTab->rad_lls = dtemp[15] * 1852.0;
			pTab->dpsi_tei = dtemp[16] * RAD;
			pTab->dv_tei = dtemp[17] * 0.3048;
			pTab->T_te = dtemp[18] * 3600.0;
			pTab->incl_fr = dtemp[19] * RAD;
			pTab->GMT_pc1 = dtemp[20] * 3600.0;
			pTab->GMT_pc2 = dtemp[21] * 3600.0;
			pTab->GMT_nd = dtemp[22] * 3600.0;
			pTab->lat_nd = dtemp[23] * RAD;
			pTab->lng_nd = dtemp[24] * RAD;
			pTab->h_nd = dtemp[25] * 1852.0;
		}
	}

	//Lastly, copy over data to master flight plan table
	PZMFPTAB = NewTable;

	return 0;
}

int RTCC::QMMBLD(std::string file)
{
	//This function loads the TLI targeting parameters for the launch day
	//Loaded file has to have the punch card format from MSC memo 69-FM-171 (NTRS ID 19740072570). One punch card = one line
	char Buff[128];
	sprintf_s(Buff, ".\\Config\\ProjectApollo\\RTCC\\%s.txt", file.c_str());

	const double R_Earth = 6378165.0;
	const double ER2HR2ToM2SEC2 = pow(R_Earth / 3600.0, 2);

	ifstream startable(Buff);

	if (startable.is_open() == false) return 1;

	std::string line, CardID;
	double dtemp1, dtemp2, dtemp3, dtemp4;
	int err = 0;
	int CardNum; //1-620
	int day; //0-9
	int cardinday; //0-45 or 0-7
	int cardinopp; //0-23 or 0-4
	int opp; //1 or 2
	int format; //Format of line
	int launchday; //1-366
	int entry; //0-14

	while (getline(startable, line))
	{
		//Cards limited to 80 columns
		line = line.substr(0, 80);
		if (line.size() != 80)
		{
			err = 1;
			break;
		}
		//Identification number is the last 9 characters of the card
		CardID = line.substr(71, 9);
		//Card number is the last three characters of the card ID
		CardNum = std::stoi(CardID.substr(6, 3));

		//Three categories of cards
		if (CardNum <= 0)
		{
			err = 2;
		}
		else if (CardNum < 461)
		{
			day = (CardNum - 1) / 46;
			cardinday = (CardNum - 1) % 46;
			cardinopp = cardinday % 23;
			format = cardinopp + 1;
		}
		else if (CardNum < 541)
		{
			day = (CardNum - 461) / 8;
			cardinday = (CardNum - 461) % 8;
			cardinopp = cardinday % 4;
			format = cardinopp + 24;
		}
		else if (CardNum < 620)
		{
			day = (CardNum - 541) / 8;
			cardinday = (CardNum - 541) % 8;
			format = cardinday + 28;
		}
		else
		{
			err = 2;
		}

		switch (format)
		{
		case 1: //Section 1, line 1 of launch day
			if (sscanf(line.c_str(), "%d %d %lf %lf", &launchday, &opp, &dtemp1, &dtemp2) == 4)
			{
				if (opp < 1 || opp > 2)
				{
					err = 3;
					break;
				}
				if (launchday < 1 || launchday > 366)
				{
					err = 3;
					break;
				}
				PZSTARGP.data[day].Day = launchday;
				PZSTARGP.data[day].t_D[opp - 1][0] = dtemp1 * 3600.0;
				PZSTARGP.data[day].cos_sigma[opp - 1][0] = dtemp2;
			}
			break;
		case 2: //Section 1, line 2 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				PZSTARGP.data[day].C_3[opp - 1][0] = dtemp1 * ER2HR2ToM2SEC2;
				PZSTARGP.data[day].e_N[opp - 1][0] = dtemp2;
				PZSTARGP.data[day].RA[opp - 1][0] = dtemp3;
				PZSTARGP.data[day].DEC[opp - 1][0] = dtemp4;
			}
			break;
		case 3: //Section 1, line 3 of launch day
		case 6: //Section 1, line 6 of launch day
		case 9: //Section 1, line 9 of launch day
		case 12: //Section 1, line 12 of launch day
		case 15: //Section 1, line 15 of launch day
		case 18: //Section 1, line 18 of launch day
		case 21: //Section 1, line 21 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				entry = 2 * (format / 3) - 1;
				PZSTARGP.data[day].t_D[opp - 1][entry] = dtemp1 * 3600.0;
				PZSTARGP.data[day].cos_sigma[opp - 1][entry] = dtemp2;
				PZSTARGP.data[day].C_3[opp - 1][entry] = dtemp3 * ER2HR2ToM2SEC2;
				PZSTARGP.data[day].e_N[opp - 1][entry] = dtemp4;
			}
			break;
		case 4: //Section 1, line 4 of launch day
		case 7: //Section 1, line 7 of launch day
		case 10: //Section 1, line 10 of launch day
		case 13: //Section 1, line 13 of launch day
		case 16: //Section 1, line 16 of launch day
		case 19: //Section 1, line 19 of launch day
		case 22: //Section 1, line 22 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				entry = 2 * (format / 3) - 1;
				PZSTARGP.data[day].RA[opp - 1][entry] = dtemp1;
				PZSTARGP.data[day].DEC[opp - 1][entry] = dtemp2;
				entry++;
				PZSTARGP.data[day].t_D[opp - 1][entry] = dtemp3 * 3600.0;
				PZSTARGP.data[day].cos_sigma[opp - 1][entry] = dtemp4;
			}
			break;
		case 5: //Section 1, line 5 of launch day
		case 8: //Section 1, line 8 of launch day
		case 11: //Section 1, line 11 of launch day
		case 14: //Section 1, line 14 of launch day
		case 17: //Section 1, line 17 of launch day
		case 20: //Section 1, line 20 of launch day
		case 23: //Section 1, line 23 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				entry = 2 * (format / 3);
				PZSTARGP.data[day].C_3[opp - 1][entry] = dtemp1 * ER2HR2ToM2SEC2;
				PZSTARGP.data[day].e_N[opp - 1][entry] = dtemp2;
				PZSTARGP.data[day].RA[opp - 1][entry] = dtemp3;
				PZSTARGP.data[day].DEC[opp - 1][entry] = dtemp4;
			}
			break;
		case 24: //Section 2, line 1 of launch day
			if (sscanf(line.c_str(), "%d %d %lf %lf", &launchday, &opp, &dtemp1, &dtemp2) == 4)
			{
				if (opp < 1 || opp > 2)
				{
					err = 3;
					break;
				}
				PZSTARGP.data[day].T_ST[opp - 1] = dtemp1 * 3600.0;
				PZSTARGP.data[day].beta[opp - 1] = dtemp2;
			}
			break;
		case 25: //Section 2, line 2 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				PZSTARGP.data[day].alpha_TS[opp - 1] = dtemp1;
				PZSTARGP.data[day].f[opp - 1] = dtemp2;
				PZSTARGP.data[day].R_N[opp - 1] = dtemp3 * R_Earth;
				PZSTARGP.data[day].T3_apo[opp - 1] = dtemp4 * 3600.0;
			}
			break;
		case 26: //Section 2, line 3 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				PZSTARGP.data[day].tau3R[opp - 1] = dtemp1 * 3600.0;
				PZSTARGP.data[day].T2[opp - 1] = dtemp2 * 3600.0;
				PZSTARGP.data[day].Vex2[opp - 1] = dtemp3 * R_Earth / 3600.0;
				PZSTARGP.data[day].Mdot2[opp - 1] = dtemp4 / (LBS*1000.0*3600.0);
			}
			break;
		case 27: //Section 2, line 4 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				PZSTARGP.data[day].DV_BR[opp - 1] = dtemp1 * R_Earth / 3600.0;
				PZSTARGP.data[day].tau2N[opp - 1] = dtemp2 * 3600.0;
				PZSTARGP.data[day].KP0[opp - 1] = dtemp3;
				PZSTARGP.data[day].KY0[opp - 1] = dtemp4;
			}
			break;
		case 28: //Section 3, line 1 of launch day
			if (sscanf(line.c_str(), "%d %lf %lf %lf", &launchday, &dtemp1, &dtemp2, &dtemp3) == 4)
			{
				PZSTARGP.data[day].T_LO = dtemp1 * 3600.0;
				PZSTARGP.data[day].theta_EO = dtemp2;
				PZSTARGP.data[day].omega_E = dtemp3 / 3600.0;
			}
			break;
		case 29: //Section 3, line 2 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				PZSTARGP.data[day].K_a1 = dtemp1 / 3600.0;
				PZSTARGP.data[day].K_a2 = dtemp2 / pow(3600.0, 2);
				PZSTARGP.data[day].K_T3 = dtemp3;
				PZSTARGP.data[day].t_DS0 = dtemp4 * 3600.0;
			}
			break;
		case 30: //Section 3, line 3 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				PZSTARGP.data[day].t_DS1 = dtemp1 * 3600.0;
				PZSTARGP.data[day].t_DS2 = dtemp2 * 3600.0;
				PZSTARGP.data[day].t_DS3 = dtemp3 * 3600.0;
				PZSTARGP.data[day].hx[0][0] = dtemp4;
			}
			break;
		case 31: //Section 3, line 4 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				PZSTARGP.data[day].hx[0][1] = dtemp1;
				PZSTARGP.data[day].hx[0][2] = dtemp2;
				PZSTARGP.data[day].hx[0][3] = dtemp3;
				PZSTARGP.data[day].hx[0][4] = dtemp4;
			}
			break;
		case 32: //Section 3, line 5 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				PZSTARGP.data[day].t_D0 = 0.0;
				PZSTARGP.data[day].t_D1 = dtemp1 * 3600.0;
				PZSTARGP.data[day].t_SD1 = dtemp2 * 3600.0;
				PZSTARGP.data[day].hx[1][0] = dtemp3;
				PZSTARGP.data[day].hx[1][1] = dtemp4;
			}
			break;
		case 33: //Section 3, line 6 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				PZSTARGP.data[day].hx[1][2] = dtemp1;
				PZSTARGP.data[day].hx[1][3] = dtemp2;
				PZSTARGP.data[day].hx[1][4] = dtemp3;
				PZSTARGP.data[day].t_D2 = dtemp4 * 3600.0;
			}
			break;
		case 34: //Section 3, line 7 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				PZSTARGP.data[day].t_SD2 = dtemp1 * 3600.0;
				PZSTARGP.data[day].hx[2][0] = dtemp2;
				PZSTARGP.data[day].hx[2][1] = dtemp3;
				PZSTARGP.data[day].hx[2][2] = dtemp4;
			}
			break;
		case 35: //Section 3, line 8 of launch day
			if (sscanf(line.c_str(), "%lf %lf %lf %lf", &dtemp1, &dtemp2, &dtemp3, &dtemp4) == 4)
			{
				PZSTARGP.data[day].hx[2][3] = dtemp1;
				PZSTARGP.data[day].hx[2][4] = dtemp2;
				PZSTARGP.data[day].t_D3 = dtemp3 * 3600.0;
				PZSTARGP.data[day].t_SD3 = dtemp4 * 3600.0;
			}
			break;
		default:
			err = 3;
			break;
		}
	}

	return err;
}

void RTCC::LoadMissionInitParameters(int year, int month, int day)
{
	//This function loads launch day specific parameters that might be updated and might be saved/loaded
	char Buff[128];
	sprintf_s(Buff, ".\\Config\\ProjectApollo\\RTCC\\%d-%02d-%02d Init.txt", year, month, day);

	ifstream startable(Buff);
	if (startable.is_open())
	{
		std::string line, strtemp;
		double dtemp, darrtemp[2];
		int itemp;

		while (getline(startable, line))
		{
			sprintf_s(Buff, line.c_str());

			papiReadScenario_int(Buff, "LDPPDwellOrbits", GZGENCSN.LDPPDwellOrbits);
			papiReadScenario_double(Buff, "PZLOIPLN_HP_LLS", PZLOIPLN.HP_LLS);
			if (papiReadScenario_double(Buff, "LSLat", dtemp))
			{
				BZLAND.lat[RTCC_LMPOS_BEST] = BZLAND.lat[RTCC_LMPOS_MED] = dtemp * RAD;
			}
			else if (papiReadScenario_double(Buff, "LSLng", dtemp))
			{
				BZLAND.lng[RTCC_LMPOS_BEST] = BZLAND.lng[RTCC_LMPOS_MED] = dtemp * RAD;
			}
			else if (papiReadScenario_double(Buff, "LSRad", dtemp))
			{
				BZLAND.rad[RTCC_LMPOS_BEST] = BZLAND.rad[RTCC_LMPOS_MED] = dtemp * 1852.0;
			}
			else if (papiReadScenario_double(Buff, "TLAND", dtemp))
			{
				CZTDTGTU.GETTD = dtemp * 3600.0;
			}
			else if (papiReadScenario_double(Buff, "TLCC_AZ_min", dtemp))
			{
				PZMCCPLN.AZ_min = dtemp * RAD;
			}
			else if (papiReadScenario_double(Buff, "TLCC_AZ_max", dtemp))
			{
				PZMCCPLN.AZ_max = dtemp * RAD;
			}
			else if (papiReadScenario_double(Buff, "TLCC_TLMIN", dtemp))
			{
				PZMCCPLN.TLMIN = dtemp;
			}
			else if (papiReadScenario_double(Buff, "TLCC_TLMAX", dtemp))
			{
				PZMCCPLN.TLMAX = dtemp;
			}
			else if (papiReadScenario_double(Buff, "REVS1", dtemp))
			{
				PZMCCPLN.REVS1 = PZLOIPLN.REVS1 = dtemp;
			}
			else if (papiReadScenario_int(Buff, "REVS2", itemp))
			{
				PZMCCPLN.REVS2 = PZLOIPLN.REVS2 = itemp;
			}
			else if (papiReadScenario_int(Buff, "LOPC_M", itemp))
			{
				PZMCCPLN.LOPC_M = itemp;
			}
			else if (papiReadScenario_int(Buff, "LOPC_N", itemp))
			{
				PZMCCPLN.LOPC_N = itemp;
			}
			else if (papiReadScenario_double(Buff, "LOI_psi_DS", dtemp))
			{
				med_k18.psi_DS = dtemp;
				med_k18.psi_MX = med_k18.psi_DS + 1.0;
				med_k18.psi_MN = med_k18.psi_DS - 1.0;
			}
			else if (papiReadScenario_double(Buff, "eta_1", dtemp))
			{
				PZLOIPLN.eta_1 = dtemp;
				PZMCCPLN.ETA1 = dtemp * RAD;
			}
			else if (papiReadScenario_double(Buff, "H_P_LPO1", dtemp))
			{
				med_k18.HPLOI1 = dtemp;
				PZMCCPLN.H_P_LPO1 = dtemp * 1852.0;
			}
			else if (papiReadScenario_double(Buff, "PZLTRT_DT_Ins_TPI", dtemp))
			{
				PZLTRT.DT_Ins_TPI = PZLTRT.DT_Ins_TPI_NOM = dtemp * 60.0;
			}
			else if (papiReadScenario_double(Buff, "LDPPDescentFlightArc", dtemp))
			{
				GZGENCSN.LDPPDescentFlightArc = dtemp * RAD;
			}
			else if (papiReadScenario_double(Buff, "LDPPHeightofPDI", dtemp))
			{
				GZGENCSN.LDPPHeightofPDI = dtemp * 0.3048;
			}
			else if (papiReadScenario_double(Buff, "SITEROT", dtemp))
			{
				PZLOIPLN.DW = dtemp;
				PZMCCPLN.SITEROT = dtemp * RAD;
			}
			else if (papiReadConfigFile_PTPSite(Buff, "PTPSite", strtemp, darrtemp, itemp))
			{
				PZREAP.PTPSite[itemp] = strtemp;
				PZREAP.PTPLatitude[itemp] = darrtemp[0];
				PZREAP.PTPLongitude[itemp] = darrtemp[1];
			}
		}
	}
}

bool RTCC::LoadMissionFiles()
{
	//Load numbers that presumable would be found on a system parameters tape
	bool found = LoadMissionConstantsFile(SystemParametersFile);
	//Load TLI simulation parameters
	QMMBLD(TLIFile);
	//Load skeleton flight plan master tape
	QMSEARCH(SFPFile);

	return found;
}

bool RTCC::LoadMissionConstantsFile(std::string file)
{
	//This function loads mission specific constants that will rarely be changed or saved/loaded
	char Buff[128];
	sprintf_s(Buff, ".\\Config\\ProjectApollo\\RTCC\\%s.txt", file.c_str());

	ifstream missionfile(Buff);
	if (missionfile.is_open())
	{
		std::string line, strtemp;
		double dtemp;
		VECTOR3 vtemp;
		int itemp;
		double darrtemp[10];

		while (getline(missionfile, line))
		{
			sprintf_s(Buff, line.c_str());

			papiReadScenario_int(Buff, "AGCEpoch", SystemParameters.AGCEpoch);
			papiReadScenario_double(Buff, "TEPHEM0", SystemParameters.TEPHEM0); //Only load for Skylark
			papiReadScenario_double(Buff, "MGVDGD", SystemParameters.MGVDGD);
			papiReadScenario_double(Buff, "MGVSGD", SystemParameters.MGVSGD);
			papiReadScenario_double(Buff, "MGVSTD", SystemParameters.MGVSTD);
			papiReadScenario_int(Buff, "MCCLEX", SystemParameters.MCCLEX);
			papiReadScenario_int(Buff, "MCCCXS", SystemParameters.MCCCXS);
			papiReadScenario_int(Buff, "MCCLXS", SystemParameters.MCCLXS);
			papiReadScenario_int(Buff, "MCLRLS", SystemParameters.MCLRLS);
			papiReadScenario_int(Buff, "MCLTTD", SystemParameters.MCLTTD);
			papiReadScenario_int(Buff, "MCLABT", SystemParameters.MCLABT);
			papiReadScenario_oct(Buff, "MCCTEP", SystemParameters.MCCTEP);
			papiReadScenario_oct(Buff, "MCLTEP", SystemParameters.MCLTEP);
			papiReadScenario_double(Buff, "MCTVEN", SystemParameters.MCTVEN);
			papiReadScenario_doublearr(Buff, "MDLEIC", SystemParameters.MDLEIC, 3);
			if (papiReadScenario_double(Buff, "RRBIAS", dtemp))
			{
				PZREAP.RRBIAS = PZMCCPLN.Reentry_range = dtemp;
			}
			papiReadScenario_double(Buff, "PZREAP_IRMAX", PZREAP.IRMAX);
			papiReadScenario_double(Buff, "PDI_K_X", RTCCPDIIgnitionTargets.K_X);
			papiReadScenario_double(Buff, "PDI_K_V", RTCCPDIIgnitionTargets.K_V);
			if (papiReadScenario_double(Buff, "CSMPadLatitude", dtemp))
			{
				SystemParameters.MCLLTP[0] = SystemParameters.MCLLTP[1] = dtemp*RAD;
				SystemParameters.MCLSDA = sin(SystemParameters.MCLLTP[0]);
				SystemParameters.MCLCDA = cos(SystemParameters.MCLLTP[0]);
				SystemParameters.MDVSTP.PHIL = SystemParameters.MCLLTP[0]; //This assumes the TLI is done with the vehicle from the CSM launchpad
			}
			else if (papiReadScenario_double(Buff, "CSMPadLongitude", dtemp))
			{
				SystemParameters.MCLGRA = dtemp * RAD;
			}
			else if (papiReadScenario_double(Buff, "LMPadLatitude", dtemp))
			{
				SystemParameters.MCLLLP[0] = SystemParameters.MCLLLP[1] = dtemp * RAD;
				SystemParameters.MCLSLL = sin(SystemParameters.MCLLLP[0]);
				SystemParameters.MCLCLL = cos(SystemParameters.MCLLLP[0]);
			}
			else if (papiReadScenario_double(Buff, "LMPadLongitude", dtemp))
			{
				SystemParameters.MCLLPL = dtemp * RAD;
			}
			else if (papiReadScenario_int(Buff, "MCCCRF", SystemParameters.MCCCRF))
			{
				sprintf(Buff, "%d", SystemParameters.MCCCRF);
				sscanf(Buff, "%o", &SystemParameters.MCCCRF_DL);
			}
			else if (papiReadScenario_int(Buff, "MCCLRF", SystemParameters.MCCLRF))
			{
				sprintf(Buff, "%d", SystemParameters.MCCLRF);
				sscanf(Buff, "%o", &SystemParameters.MCCLRF_DL);
			}
			else if (papiReadScenario_double(Buff, "PDI_v_IGG", dtemp))
			{
				RTCCPDIIgnitionTargets.v_IGG = dtemp * 0.3048;
			}
			else if (papiReadScenario_double(Buff, "PDI_r_IGXG", dtemp))
			{
				RTCCPDIIgnitionTargets.r_IGXG = dtemp * 0.3048;
			}
			else if (papiReadScenario_double(Buff, "PDI_r_IGZG", dtemp))
			{
				RTCCPDIIgnitionTargets.r_IGZG = dtemp * 0.3048;
			}
			else if (papiReadScenario_double(Buff, "PDI_K_Y", dtemp))
			{
				RTCCPDIIgnitionTargets.K_Y = dtemp / 0.3048;
			}
			else if (papiReadScenario_vec(Buff, "PDI_RBRFG", vtemp))
			{
				RTCCDescentTargets.RBRFG = vtemp * 0.3048;
			}
			else if (papiReadScenario_vec(Buff, "PDI_VBRFG", vtemp))
			{
				RTCCDescentTargets.VBRFG = vtemp * 0.3048;
			}
			else if (papiReadScenario_vec(Buff, "PDI_ABRFG", vtemp))
			{
				RTCCDescentTargets.ABRFG = vtemp * 0.3048;
			}
			else if (papiReadScenario_double(Buff, "PDI_JBRFGZ", dtemp))
			{
				RTCCDescentTargets.JBRFGZ = dtemp * 0.3048;
			}
			else if (papiReadScenario_vec(Buff, "PDI_RARFG", vtemp))
			{
				RTCCDescentTargets.RARFG = vtemp * 0.3048;
			}
			else if (papiReadScenario_vec(Buff, "PDI_VARFG", vtemp))
			{
				RTCCDescentTargets.VARFG = vtemp * 0.3048;
			}
			else if (papiReadScenario_vec(Buff, "PDI_AARFG", vtemp))
			{
				RTCCDescentTargets.AARFG = vtemp * 0.3048;
			}
			else if (papiReadScenario_double(Buff, "PDI_JARFGZ", dtemp))
			{
				RTCCDescentTargets.JARFGZ = dtemp * 0.3048;
			}
			else if (papiReadConfigFile_ATPSite(Buff, "ATPSite", strtemp, darrtemp, itemp))
			{
				PZREAP.ATPSite[itemp] = strtemp;
				for (int i = 0; i < 10; i++)
				{
					PZREAP.ATPCoordinates[itemp][i] = darrtemp[i];
				}
			}

			papiReadConfigFile_CGTable(Buff, "MHVCCG", SystemParameters.MHVCCG.Weight, SystemParameters.MHVCCG.CG);
			papiReadScenario_int(Buff, "MHVCCG_N", SystemParameters.MHVCCG.N);
			papiReadConfigFile_CGTable(Buff, "MHVLCG", SystemParameters.MHVLCG.Weight, SystemParameters.MHVLCG.CG);
			papiReadScenario_int(Buff, "MHVLCG_N", SystemParameters.MHVLCG.N);
			papiReadConfigFile_CGTable(Buff, "MHVACG", SystemParameters.MHVACG.Weight, SystemParameters.MHVACG.CG);
			papiReadScenario_int(Buff, "MHVACG_N", SystemParameters.MHVACG.N);
		}

		//Anything that is mission, but not launch day specific
		InitializeCoordinateSystem();

		return true;
	}
	return false;
}

void RTCC::InitializeCoordinateSystem()
{
	//J2000 ecliptic to BRCS matrix
	SystemParameters.MAT_J2000_BRCS = OrbMech::J2000EclToBRCS(SystemParameters.AGCEpoch);
	//Calculate TEPHEM0, the zero time for AGC time keeping. Except for 1950 coordinate system (Skylark), which has to be loaded from the mission file
	if (SystemParameters.AGCEpoch != 1950)
	{
		SystemParameters.TEPHEM0 = OrbMech::TJUDAT(SystemParameters.AGCEpoch - 1, 7, 1) - 2400000.5;
	}
	//Convert star table to BRCS
	EMSGSUPP(0, 0);
}

void RTCC::TimeUpdate()
{
	//Updates RTCC GMT and also GET for both CSM and LM

	//Check if mission has been initialized (i.e. GMTBASE not being zero)
	if (SystemParameters.GMTBASE != 0.0)
	{
		//Update times
		double MJD = oapiGetSimMJD();

		RTCC_GreenwichMeanTime = (MJD - SystemParameters.GMTBASE)*24.0*3600.0;
		RTCC_CSM_GroundElapsedTime = RTCC_GreenwichMeanTime - SystemParameters.MCGMTL*3600.0;
		RTCC_LM_GroundElapsedTime = RTCC_GreenwichMeanTime - SystemParameters.MCGMTS*3600.0;
	}
	else
	{
		//Keep times at zero
		RTCC_GreenwichMeanTime = RTCC_CSM_GroundElapsedTime = RTCC_LM_GroundElapsedTime = 0.0;
	}
}

void RTCC::AP7BlockData(AP7BLKOpt *opt, AP7BLK &pad)
{
	EarthEntryOpt entopt;
	EntryResults res;
	double v_e, m1, Vc;

	char weather[] = "GOOD";

	v_e = SystemParameters.MCTST1 / SystemParameters.MCTSW1;

	entopt.vessel = calcParams.src;
	entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
	entopt.nominal = true;
	entopt.ReA = 0;
	entopt.entrylongmanual = true;

	for (int i = 0;i < 8;i++)
	{
		sprintf(pad.Area[i], "N/A");
		sprintf(pad.Wx[i], "N/A");
	}

	for (int i = 0;i < opt->n;i++)
	{
		entopt.lng = opt->lng[i];
		entopt.TIGguess = opt->GETI[i];

		BlockDataProcessor(&entopt, &res);

		m1 = calcParams.src->GetMass()*exp(-length(res.dV_LVLH) / v_e);
		Vc = length(res.dV_LVLH) - 60832.18 / m1; //TBD

		sprintf(pad.Area[i], opt->area[i].c_str());
		sprintf(pad.Wx[i], weather);

		pad.dVC[i] = Vc/0.3048;
		pad.GETI[i] = res.P30TIG;
		pad.Lat[i] = res.latitude*DEG;
		pad.Lng[i] = res.longitude*DEG;
	}
}

void RTCC::AP11BlockData(AP11BLKOpt *opt, P37PAD &pad)
{
	EntryOpt entopt;
	EntryResults res;

	entopt.entrylongmanual = true;
	entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
	entopt.type = 1;
	entopt.vessel = calcParams.src;

	if (opt->useSV)
	{
		entopt.RV_MCC = opt->RV_MCC;
	}
	else
	{
		entopt.RV_MCC = StateVectorCalc(calcParams.src);
	}

	for (int i = 0;i < opt->n;i++)
	{
		entopt.TIGguess = opt->GETI[i];
		entopt.lng = opt->lng[i];
		entopt.t_Z = opt->T_Z[i];

		EntryTargeting(&entopt, &res);

		pad.dVT[i] = length(res.dV_LVLH) / 0.3048;
		pad.GET400K[i] = res.GET05G;
		pad.GETI[i] = opt->GETI[i];
		pad.lng[i] = opt->lng[i] * DEG;
	}
}

void RTCC::BlockDataProcessor(EarthEntryOpt *opt, EntryResults *res)
{
	EarthEntry* entry;
	double GETbase, GET;
	bool stop;
	SV sv;

	GETbase = CalcGETBase();
	stop = false;

	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}
	GET = (sv.MJD - GETbase)*24.0*3600.0;

	entry = new EarthEntry(this, sv.R, sv.V, sv.MJD, sv.gravref, GETbase, opt->TIGguess, opt->ReA, opt->lng, opt->nominal, opt->entrylongmanual);

	while (!stop)
	{
		stop = entry->EntryIter();
	}

	res->dV_LVLH = entry->Entry_DV;
	res->P30TIG = entry->EntryTIGcor;
	res->latitude = entry->EntryLatcor;
	res->longitude = entry->EntryLngcor;
	res->GET400K = entry->t2;
	res->GET05G = entry->EntryRET;
	res->RTGO = entry->EntryRTGO;
	res->VIO = entry->EntryVIO;
	res->ReA = entry->EntryAng;
	res->precision = entry->precision;

	delete entry;

	double TIG_imp;
	VECTOR3 DV_imp;

	TIG_imp = res->P30TIG;
	DV_imp = res->dV_LVLH;

	PoweredFlightProcessor(sv, TIG_imp, opt->enginetype, 0.0, DV_imp, true, res->P30TIG, res->dV_LVLH, res->sv_preburn, res->sv_postburn);

	RZRFTT.Primary.GMTI = GMTfromGET(res->P30TIG);
	RZRFTT.Primary.DeltaV = res->dV_LVLH;
}

void RTCC::EntryTargeting(EntryOpt *opt, EntryResults *res)
{
	RTEEarth* entry;
	double GET, LINE[10];
	bool stop;
	SV sv;

	stop = false;

	sv = opt->RV_MCC;
	GET = (sv.MJD - CalcGETBase())*24.0*3600.0;

	if (opt->entrylongmanual)
	{
		LINE[0] = PI05;
		LINE[1] = opt->lng;
		LINE[2] = -PI05;
		LINE[3] = opt->lng;
		LINE[4] = 1e10;
		LINE[5] = 1e10;
	}
	else
	{
		if (opt->ATPLine < 0 || opt->ATPLine>4)
		{
			return;
		}

		for (int i = 0;i < 10;i++)
		{
			LINE[i] = PZREAP.ATPCoordinates[opt->ATPLine][i];
		}
	}

	entry = new RTEEarth(this, ConvertSVtoEphemData(sv), GetGMTBase(), SystemParameters.MCLAMD, GMTfromGET(opt->TIGguess), GMTfromGET(opt->t_Z), opt->type);
	//Check for error (state vector not in Earth SOI)
	if (entry->errorstate)
	{
		return;
	}
	entry->READ(opt->r_rbias, opt->dv_max, 2, 37500.0*0.3048);
	entry->ATP(LINE);
	while (!stop)
	{
		stop = entry->EntryIter();
	}

	res->dV_LVLH = entry->Entry_DV;
	res->P30TIG = GETfromGMT(entry->sv_ig.GMT);
	res->latitude = entry->EntryLatcor;
	res->longitude = entry->EntryLngcor;
	res->GET400K = GETfromGMT(entry->t2);
	res->GET05G = GETfromGMT(entry->EntryRET);
	res->RTGO = entry->EntryRTGO;
	res->VIO = entry->EntryVIO;
	res->ReA = entry->EntryAng;
	res->precision = entry->precision;

	delete entry;

	double TIG_imp, LMmass;
	VECTOR3 DV_imp;

	TIG_imp = res->P30TIG;
	DV_imp = res->dV_LVLH;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	PoweredFlightProcessor(opt->RV_MCC, TIG_imp, opt->enginetype, LMmass, DV_imp, true, res->P30TIG, res->dV_LVLH, res->sv_preburn, res->sv_postburn);
}

int RTCC::PCZYCF(double R1, double R2, double PHIT, double DELT, double VXI2, double VYI2, double VXF1, double VYF1, double SQRMU, int NREVS, int body, double &a, double &e, double &f_T, double &t_PT)
{
	double S, P, Q, a_lim, delta_lim, SIGN, dt_lim, S_L, S_P, S_Q, eps, delta, TERM, P_P, a_i, AFAIL, da, da_i, a_N, C_N, P_T, sin_f_T, cos_f_T;
	double eps_N, delta_N, C_ep, C_e, C_f, C_em, C_fm, C_fp, a_Ni, da_N, dt_N, da_Ni, a_apo, e_apo, f_T_apo, t_pt_apo, V_T, gamma_T, l_R, f_R, gamma_R, V_R, DV_T, DV_T_apo;
	int i, k, i_N, L;

	if (body == BODY_EARTH)
	{
		AFAIL = 1.02*OrbMech::R_Earth;
	}
	else
	{
		AFAIL = OrbMech::R_Moon;
	}

	S = sqrt(R1*R1 + R2 * R2 - 2.0*R1*R2*cos(PHIT));
	P = R1 + R2 + S;
	Q = R1 + R2 - S;
	a_lim = P / 4.0;
	delta_lim = 2.0*atan(sqrt(Q / (P - Q)));
	i = 0;
	k = 0;
	SIGN = sign(PHIT - PI);
	if (NREVS > 0)
	{
		goto RTCC_PCZYCF_4_1;
	}
	if (DELT <= 1.0 / (6.0*SQRMU)*(pow(P, 1.5) + SIGN * pow(Q, 1.5)))
	{
		return -1;
	}
	//Set semi-major axis to start iteration
	a = a_lim + 10000.0*0.3048;
	dt_lim = pow(a_lim, 1.5) / SQRMU * (PI + SIGN * (delta_lim - sin(delta_lim)));
	if (DELT > dt_lim)
	{
		S_L = SIGN;
	}
	else
	{
		S_L = -SIGN;
	}
RTCC_PCZYCF_2_1:
	i++;
	if (i > 15)
	{
		a = a_i;
	}
	S_P = sqrt(P / (4.0*a - P));
	S_Q = sqrt(Q / (4.0*a - Q));
	eps = 2.0*atan(S_P);
	delta = 2.0*atan(S_Q);
	if (i > 15)
	{
		goto RTCC_PCZYCF_7_1;
	}
	if (i > 1)
	{
		goto RTCC_PCZYCF_3_1;
	}
	if (DELT <= dt_lim || k > 0)
	{
		SIGN = 1.0;
		TERM = PI2 * (double)NREVS;
	}
	else
	{
		SIGN = -1.0;
		TERM = PI2 * (double)(NREVS + 1);
	}
RTCC_PCZYCF_3_1:
	P_P = TERM + SIGN * (eps - sin(eps) - S_L * (delta - sin(delta)));
	a_i = a - (SQRMU*DELT - pow(a, 1.5)*P_P) / (pow(a, 0.5)*(SIGN*((1.0 - cos(eps))*S_P - S_L * (1.0 - cos(delta))*S_Q) - 1.5*P_P));
	//Computed value large enough?
	if (a_i <= a_lim)
	{
		if (a_lim > AFAIL)
		{
			return -1;
		}
		a_i = AFAIL;
	}
	da = abs(a_i - a);
	a = a_i;
	if (50.0*0.3048 > da)
	{
		goto RTCC_PCZYCF_7_1;
	}
	if (i > 1)
	{
		if (da_i <= da)
		{
			goto RTCC_PCZYCF_2_1;
		}
		da_i = da;
		a_i = a;
	}
	else
	{
		if (a > a_lim)
		{
			da_i = da;
			a_i = a;
		}
		else
		{
			a = a_lim + 100.0*0.3048;
			da_i = da;
		}
	}
	goto RTCC_PCZYCF_2_1;
RTCC_PCZYCF_4_1:
	//Set semi-major axis to start multiple rev iteration. Initialize iteration counter
	a_N = a_lim + 50000.0*0.3048;
	i_N = 0;
	C_N = 6.0*PI*(double)(NREVS);
RTCC_PCZYCF_4_2:
	i_N++;
	if (i_N > 25)
	{
		a_N = a_Ni;
		goto RTCC_PCZYCF_5_2;
	}
	eps_N = 2.0*atan(sqrt(P / (4.0*a_N - P)));
	delta_N = 2.0*atan(sqrt(Q / (4.0*a_N - Q)));
	C_e = cos(eps_N);
	C_f = cos(delta_N);
	C_em = 1.0 - C_e;
	C_fm = 1.0 - C_f;
	C_fp = 1.0 + C_f;
	C_ep = 1.0 + C_e;
	a_Ni = a_N - (C_N + 3.0*(eps_N - SIGN * delta_N) - (5.0 + C_e)*sqrt(C_em / C_ep) - SIGN * (5.0 + C_f)*sqrt(C_fm / C_fp)) / (2.0*(pow(C_em, 3.5) / (P*pow(C_ep, 1.5)) + SIGN * pow(C_fm, 3.5) / Q / pow(C_fp, 1.5)));
	da_N = abs(a_Ni - a_N);
	a_N = a_Ni;
	if (da_N <= 10.0*0.3048)
	{
		goto RTCC_PCZYCF_5_2;
	}
	if (i_N > 1)
	{
		if (da_Ni <= da_N)
		{
			goto RTCC_PCZYCF_4_2;
		}
		da_Ni = da_N;
		a_Ni = a_N;
	}
	else
	{
		if (a_N > a_lim)
		{
			da_Ni = da_N;
			a_Ni = a_N;
		}
		else
		{
			a_N = a_lim + 50.0*0.3048;
			da_Ni = da_N;
		}
	}
	goto RTCC_PCZYCF_4_2;
RTCC_PCZYCF_5_2:
	eps_N = 2.0*atan(sqrt(P / (4.0*a_N - P)));
	delta_N = 2.0*atan(sqrt(Q / (4.0*a_N - Q)));
	dt_N = pow(a_N, 1.5) / SQRMU * (PI2*(double)(NREVS)+eps_N-sin(eps_N)+SIGN*(delta_N-sin(delta_N)));
	if (DELT <= dt_N)
	{
		return -1;
	}
	dt_lim = pow(a_lim, 1.5) / SQRMU * (PI*(double)(2 * NREVS + 1) + SIGN * (delta_lim - sin(delta_lim)));
	if (SIGN > 0)
	{
		if (DELT > dt_lim)
		{
			L = 12;
			S_L = 1.0;
		}
		else
		{
			L = 10;
			S_L = -1.0;
		}
	}
	else
	{
		if (DELT > dt_lim)
		{
			L = 11;
			S_L = -1.0;
		}
		else
		{
			L = 9;
			S_L = 1.0;
		}
	}
	a = a_lim + 10000.0*0.3048;
	goto RTCC_PCZYCF_2_1;
RTCC_PCZYCF_7_1:
	e = sqrt(pow((R1 - a - (R2 - a)*cos(eps - delta)) / sin(eps - delta), 2) + pow(a - R2, 2)) / a;
	P_T = a * (1.0 - e * e);
	cos_f_T = (P_T - R2) / e / R2;
	sin_f_T = (cos_f_T*cos(PHIT) - (P_T - R1) / e / R1) / sin(PHIT);
	f_T = atan2(sin_f_T, cos_f_T);
	if (f_T < 0)
	{
		f_T += PI2;
	}
	t_PT = a * sqrt(P_T) / SQRMU * (2.0 / sqrt(1.0 - e * e)*atan(sqrt((1.0 - e) / (1.0 + e))*tan(f_T / 2.0)) - e * sin_f_T / (1.0 + e * cos_f_T));
	if (t_PT < 0)
	{
		t_PT += PI2 * pow(a, 1.5) / SQRMU;
	}
	if (NREVS <= 0)
	{
		return 0;
	}
	V_T = SQRMU * sqrt(2.0 / R1 - 1.0 / a);
	gamma_T = atan(e*sin_f_T / (1.0*e*cos_f_T));
	l_R = pow(t_PT + DELT, -1.5)*SQRMU;
	f_R = OrbMech::MeanToTrueAnomaly(l_R, e);
	gamma_R = atan(e*sin(f_R)/(1.0*e*cos(f_R)));
	V_R = SQRMU * sqrt(2.0 / R1 - 1.0 / a);
	DV_T = sqrt(pow(V_T*cos(gamma_T) - VXI2, 2) + pow(V_T*sin(gamma_T) - VYI2, 2)) + sqrt(pow(VXF1 - V_R * cos(gamma_R), 2) + pow(VYF1 - V_R * sin(gamma_R), 2));
	if (k <= 0)
	{
		a_apo = a;
		e_apo = e;
		f_T_apo = f_T;
		t_pt_apo = t_PT;
		DV_T_apo = DV_T;
		k = 1;
		i = 0;
		if (L == 10 || L == 12)
		{
			S_L = -1.0;
		}
		else
		{
			S_L = 1.0;
		}
		a = a_N + 10000.0*0.3048;
		goto RTCC_PCZYCF_2_1;
	}
	//Output lower DV solution
	if (DV_T_apo < DV_T)
	{
		a = a_apo;
		e = e_apo;
		f_T = f_T_apo;
		t_PT = t_pt_apo;
	}
	return 0;
}

void POSVEL(double A, double B, double C, double D, double *E, double SQRMU)
{
	double gamma;
	if (E[0])
	{
		//Compute velocity
		double a = A;
		double e = B;
		double f = C;
		double R = D;
		gamma = atan(e*sin(f) / (1.0 + e * cos(f)));
		double V = SQRMU * sqrt(2.0 / R - 1.0 / a);
		E[0] = V * cos(gamma);
		E[1] = V * sin(gamma);
	}
	else
	{
		//Compute position
		double i = A;
		double h = B;
		double u = C;
		double R = D;
		gamma = sin(u);
		double alpha = gamma * cos(i);
		double beta = cos(u);
		double eta = cos(h);
		double psi = sin(h);
		E[0] = R * (beta*eta - alpha * psi);
		E[1] = R * (beta*psi + alpha * eta);
		E[2] = R * (gamma*psi);
	}
}

int RTCC::PMMTIS(EphemerisData sv_A1, EphemerisData sv_P1, double dt, double DH, double theta, EphemerisData &sv_A1_apo, EphemerisData &sv_A2, EphemerisData &sv_A2_apo)
{
	EphemerisData sv_P2;
	VECTOR3 RP2off, VP2off, VA1_apo, DR, R3;
	CELEMENTS elem_CE, elem_T;
	double WT3, theta_0, mu, u_CE, R_CE, u_T, R_T, f_T, f_CE, f3, RPLIM;
	CELEMENTS elem_C;
	//VECTOR3 R_i;
	//double sin_u_C, TEMP1, TEMP2, DEN, sin_u3, r3, u3, SIGN, T_PC, sin_h_C, cos_h_C, E[3], f_C, V_Xi, V_Yi, V_Xf, V_Yf, u_C, R_C;
	int N, IC;
	bool prograde;

	if (sv_A1.RBI == BODY_EARTH)
	{
		prograde = true;
		mu = OrbMech::mu_Earth;
		//SIGN = 1.0;
		RPLIM = OrbMech::R_Earth + 10.0*1852.0;
	}
	else
	{
		prograde = false;
		mu = OrbMech::mu_Moon;
		//SIGN = -1.0;
		RPLIM = BZLAND.rad[RTCC_LMPOS_BEST];
	}

	IC = 0;
	/*R_i = sv_A1.R;
	sv_A1_apo = sv_A1;

	//Calculate chaser orbital elements
	elem_C = OrbMech::GIMIKC(sv_A1.R, sv_A1.V, mu);
	f_C = OrbMech::MeanToTrueAnomaly(elem_C.l, elem_C.e);
	u_C = elem_C.g + f_C;
	if (u_C >= PI2)
	{
		u_C -= PI2;
	}
	R_C = length(R_i);
	//Calculate target orbital elements
	elem_T = OrbMech::GIMIKC(sv_P1.R, sv_P1.V, mu);
	f_T = OrbMech::MeanToTrueAnomaly(elem_T.l, elem_T.e);
	u_T = elem_T.g + f_T;
	if (u_T >= PI2)
	{
		u_T -= PI2;
	}
	R_T = length(sv_P1.R);*/

	f_T = 0.0;
	theta_0 = OrbMech::PHSANG(sv_P1.R, sv_P1.V, sv_A1.R);
	f3 = f_T + dt * PI2 / OrbMech::period(sv_P1.R, sv_P1.V, mu);
	//r3 = elem_T.a*(1.0 - elem_T.e*elem_T.e) / (1.0 + elem_T.e*cos(f3)) - DH;
	WT3 = f3 - f_T + theta_0 - theta;
	/*u3 = u_C + WT3;
	while (u3 >= PI2)
	{
		u3 -= PI2;
	}
	while (u3 < 0)
	{
		u3 += PI2;
	}

	E[0] = 0.0;
	POSVEL(elem_T.i, elem_T.h, u3, r3, E, sqrt(mu));
	R3 = _V(E[0], E[1], E[2]);*/

	N = (int)(WT3 / PI2);
	sv_P2 = coast(sv_P1, dt);

	//Calculate target orbital elements at final time
	elem_T = OrbMech::GIMIKC(sv_P2.R, sv_P2.V, mu);
	f_T = OrbMech::MeanToTrueAnomaly(elem_T.l, elem_T.e);
	u_T = elem_T.g + f_T;
	if (u_T >= PI2)
	{
		u_T -= PI2;
	}
	R_T = length(sv_P2.R);

	if (theta != 0 || DH != 0)
	{
		elem_CE.a = elem_T.a - DH;
		elem_CE.e = elem_T.e*elem_T.a / elem_CE.a;
		f_CE = f_T - theta;
		if (f_CE >= PI2)
		{
			f_CE -= PI2;
		}
		if (f_CE < 0)
		{
			f_CE += PI2;
		}
		u_CE = u_T - theta;
		if (u_CE >= PI2)
		{
			u_CE -= PI2;
		}
		if (u_CE < 0)
		{
			u_CE += PI2;
		}
		R_CE = elem_CE.a*(1.0 - elem_CE.e*elem_CE.e) / (1.0 + elem_CE.e*cos(f_CE));
		elem_CE.l = OrbMech::TrueToMeanAnomaly(f_CE, elem_CE.e);
	}
	else
	{
		elem_CE.a = elem_T.a;
		elem_CE.e = elem_T.e;
		f_CE = f_T;
		u_CE = u_T;
		R_CE = R_T;
		elem_CE.l = elem_T.l;
	}

	/*if (N != 0)
	{
		E[0] = 1.0;
		POSVEL(elem_C.a, elem_C.e, f_C, R_C, E, sqrt(mu));
		V_Xi = E[0];
		V_Yi = E[1];
		E[0] = 1.0;
		POSVEL(elem_CE.a, elem_CE.e, f_CE, R_CE, E, sqrt(mu));
		V_Xf = E[0];
		V_Yf = E[1];
	}
	else
	{
		V_Xi = 0.0;
		V_Yi = 0.0;
		V_Xf = 0.0;
		V_Yf = 0.0;
	}*/

	elem_CE.i = elem_T.i;
	elem_CE.g = elem_T.g;
	elem_CE.h = elem_T.h;
	OrbMech::GIMKIC(elem_CE, mu, RP2off, VP2off);
/*RTCC_PMMTIS_3_3:
	WT3 = acos(dotp(unit(R_i), unit(R3)));
	if (SIGN*(R_i.x*R3.y- R_i.y*R3.x) < 0)
	{
		WT3 = PI2 - WT3;
	}

	PCZYCF(R_C, R_CE, WT3, dt, V_Xi, V_Yi, V_Xf, V_Yf, sqrt(mu), N, sv_A1.RBI, elem_C.a, elem_C.e, f_C, T_PC);

	//Periapsis limit
	if (elem_C.a*(1.0 - elem_C.e) < RPLIM)
	{
		return -1;
	}
	u_C = atan(sin(WT3) / (R3.z*length(R_i) / (R_i.z*length(R3)) - cos(WT3)));
	if (u_C > 0)
	{
		if (R_i.z < 0)
		{
			u_C += PI;
		}
	}
	else if (u_C < 0)
	{
		if (R_i.z > 0)
		{
			u_C += PI;
		}
	}
	u3 = u_C + WT3;
	sin_u_C = sin(u_C);
	elem_C.i = atan(abs(R_i.z) / sqrt(pow(length(R_i)*sin_u_C, 2) - R_i.z*R_i.z));
	sin_u3 = sin(u3);
	TEMP1 = length(R3) / sin_u_C;
	TEMP2 = length(R_i) / sin_u_C;
	DEN = cos(u3)*sin_u_C - cos(u_C)*sin_u3;
	sin_h_C = (R3.y / TEMP1 - R_i.y / TEMP2) / DEN;
	cos_h_C = (R3.x / TEMP1 - R_i.x / TEMP2) / DEN;
	elem_C.h = atan2(sin_h_C, cos_h_C);
	if (elem_C.h < 0)
	{
		elem_C.h += PI2;
	}
	elem_C.l = T_PC * pow(elem_C.a, -1.5)*sqrt(mu);
	if (elem_C.l < 0)
	{
		elem_C.l += PI2;
	}
	elem_C.g = u_C - f_C;
	if (elem_C.g < 0)
	{
		elem_C.g += PI2;
	}
	OrbMech::GIMKIC(elem_C, mu, sv_A1_apo.R, sv_A1_apo.V);
	sv_A2 = coast(sv_A1_apo, dt);
	DR = sv_A2.R - RP2off;
	if (length(DR) < 100.0*0.3048)
	{
		//sv_A2_apo
		return 0;
	}
	IC++;
	if (IC >= 15)
	{
		return -1;
	}
	R3 = R3 - DR;
	r3 = length(R3);
	goto RTCC_PMMTIS_3_3;*/

	sv_A1_apo = sv_A1;
	R3 = RP2off;
	do
	{
		VA1_apo = OrbMech::elegant_lambert(sv_A1.R, sv_A1.V, R3, dt, N, prograde, mu);
		if (length(VA1_apo) == 0.0)
		{
			return 1;
		}
		sv_A1_apo.V = VA1_apo;
		elem_C = OrbMech::GIMIKC(sv_A1_apo.R, sv_A1_apo.V, mu);
		if (elem_C.a*(1.0 - elem_C.e) < RPLIM)
		{
			return 1;
		}
		sv_A2 = coast(sv_A1_apo, dt);
		DR = sv_A2.R - RP2off;
		R3 = R3 - DR;
		IC++;
		if (IC > 15)
		{
			return 1;
		}
	} while (length(DR) > 100.0*0.3048);

	//VA1_apo = OrbMech::Vinti(sv_A1.R, sv_A1.V, RP2off, OrbMech::MJDfromGET(sv_A1.GMT,SystemParameters.GMTBASE), dt, N, prograde, sv_A1.RBI, sv_A1.RBI, sv_A1.RBI, _V(0.0, 0.0, 0.0), 100.0*0.3048); //Vinti Targeting: For non-spherical gravity

	sv_A1_apo = sv_A1;
	sv_A1_apo.V = VA1_apo;
	sv_A2 = coast(sv_A1_apo, dt);
	sv_A2_apo = sv_A2;
	sv_A2_apo.V = VP2off;
	return 0;
}

int RTCC::PMSTICN_ELEV(EphemerisData sv_A1, EphemerisData sv_P1, double phi_req, double mu, double &T_ELEV)
{
	EphemerisData sv_A, sv_P, SV_store[2];
	CELEMENTS elem_C;
	double T_S, T, u, X_R_L, Y_R_L, R_C, eps_phi, phi, X_S, Y_S, A, B, C, D;
	int IC;

	eps_phi = 0.001*RAD;
	IC = 0;
	T_S = 0.0;
	T = sv_A1.GMT;
	sv_A = sv_A1;
	sv_P = sv_P1;
PMSTICN_ELEV_18_3:
	SV_store[0] = sv_A;
	SV_store[1] = sv_P;
PMSTICN_ELEV_18_4:
	sv_A = coast(SV_store[0], T - SV_store[0].GMT);
	sv_P = coast(SV_store[1], T - SV_store[1].GMT);
	//TBD: Error
	elem_C = OrbMech::GIMIKC(sv_A.R, sv_A.V, mu);
	u = elem_C.g + OrbMech::MeanToTrueAnomaly(elem_C.l, elem_C.e);
	R_C = length(sv_A.R);
	X_R_L = sv_P.R.x*(-sin(u)*cos(elem_C.h) - sin(elem_C.h)*cos(u)*cos(elem_C.i)) + sv_P.R.y*(-sin(u)*sin(elem_C.h) + cos(elem_C.h)*cos(u)*cos(elem_C.i)) + sv_P.R.z*(cos(u)*sin(elem_C.i));
	Y_R_L = R_C - (sv_P.R.x*(cos(u)*cos(elem_C.h) - sin(u)*cos(elem_C.i)*sin(elem_C.h)) + sv_P.R.y*(cos(u)*sin(elem_C.h) + sin(u)*cos(elem_C.i)*cos(elem_C.h)) + sv_P.R.z*(sin(u)*sin(elem_C.i)));
	phi = atan2(-Y_R_L, X_R_L);
	if (abs(phi - phi_req) <= eps_phi)
	{
		goto PMSTICN_ELEV_21_1;
	}
	if (T_S == 0.0)
	{
		X_S = X_R_L;
		Y_S = Y_R_L;
		T_S = T;
		T = T + 100.0;
		goto PMSTICN_ELEV_18_3;
	}
	else
	{
		A = (X_R_L - X_S) / (T - T_S);
		B = (X_R_L*T_S - X_S * T) / (T_S - T);
		C = (Y_R_L - Y_S) / (T - T_S);
		D = (Y_R_L*T_S - Y_S * T) / (T_S - T);
		X_S = X_R_L;
		Y_S = Y_R_L;
		T_S = T;
		T = (-B * sin(phi_req) - D * cos(phi_req)) / (A*sin(phi_req) + C * cos(phi_req));
		IC++;
		if (IC > 10)
		{
			T_ELEV = sv_A1.GMT;
			return 1;
		}
		goto PMSTICN_ELEV_18_4;
	}
PMSTICN_ELEV_21_1:
	T_ELEV = T;
	return 0;
}

void PMSTICN_PY(VECTOR3 R_A, VECTOR3 V_A, VECTOR3 R_B, VECTOR3 V_B, double &Pitch, double &Yaw)
{
	VECTOR3 H_A, H_B;
	double r_B, sin_delta, r_A_dot, r_B_dot, dr_dot, DV, h_A, h_B, VH_A, VH_B, DV_H;

	H_A = crossp(R_A, V_A);
	h_A = length(H_A);
	H_A = unit(H_A);
	H_B = crossp(R_B, V_B);
	h_B = length(H_B);
	H_B = unit(H_B);
	r_B = length(R_B);
	sin_delta = dotp(crossp(H_A, H_B), R_B) / r_B;
	r_B_dot = dotp(R_B, V_B) / r_B;
	r_A_dot = dotp(R_A, V_A) / r_B;
	dr_dot = r_A_dot - r_B_dot;
	DV = length(V_B - V_A);
	Pitch = asin(dr_dot / DV);
	VH_B = h_B / r_B;
	VH_A = h_A / r_B;
	DV_H = DV * cos(Pitch);
	Yaw = asin(VH_A*sin_delta / DV_H);
	if (VH_A*sqrt(1.0 - sin_delta * sin_delta) < VH_B)
	{
		Yaw = PI - Yaw;
		if (Yaw > PI)
		{
			Yaw -= PI2;
		}
	}
}

void RTCC::PMMTISS()
{

}

void RTCC::PMSTICN(const TwoImpulseOpt &opt, TwoImpulseResuls &res)
{
	TwoImpulseMultipleSolutionTableEntry entry[13];
	EphemerisData sv_A1, sv_P1, sv_A1_apo, sv_A2, sv_A2_apo;
	double DH, PhaseAngle, Elev, WT, T1, T2, mu, DV_opt, DVT, T_WSR, DH_WSR, theta_WSR, TMAX, theta_T_min, HthetaR, theta_max, theta_min, du_dot, DV_TP, T_slip;
	double DV_OPTH, DH_OPTH, theta_OPTH, TIME_OPTH, K, D[4];
	int err, soln = 0, OPCASE, display;
	std::vector<std::string> str;

	//Initialization logic
	if (opt.sv_A.RBI == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
		//ratio of catch-up rate to height difference (roughly at 160NM altitude)
		K = 5.9853114 / 3600.0 / OrbMech::R_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		//ratio of catch-up rate to height difference (roughly at 60NM altitude)
		K = 16.38140036 / 3600.0 / OrbMech::R_Earth;
	}
	DV_opt = 10000000.0;
	//TPI velocity as a function of travel angle
	D[0] = 0.0;
	D[1] = 0.0;
	D[2] = 0.0;
	D[3] = 0.0;

	//What type of request?
	if (opt.mode == 3 || opt.mode == 4)
	{
		goto RTCC_PMSTICN_10_1;
	}
	if (opt.mode == 5)
	{
		goto RTCC_PMSTICN_9_1;
	}
	//Corrective Combination?
	if (opt.mode == 1)
	{
		goto RTCC_PMSTICN_13_1;
	}

	//Load MED request parameters
	T1 = opt.T1;
	T2 = opt.T2;
	//DTREAD: GZGENCSN Blks. 9-12
	DH = GZGENCSN.TIDeltaH;
	PhaseAngle = GZGENCSN.TIPhaseAngle;
	Elev = GZGENCSN.TIElevationAngle;
	WT = GZGENCSN.TITravelAngle;

	//DTWRITE: PZTIPREG (Set T-I Table to updating condition)
	PZTIPREG.Updating = true;
	//Load T-I Table area with necessary MED quantities
	if (DH != 0.0 && PhaseAngle != 0.0 && Elev != 0.0 && WT != 0.0 && T1 > 0 && T2 > 0)
	{
		PZTIPREG.showTPI = true;
	}
	else
	{
		PZTIPREG.showTPI = false;
	}
RTCC_PMSTICN_3_1:
	if (T1 < 0)
	{
		err = PMSTICN_ELEV(opt.sv_A, opt.sv_P, Elev, mu, T1);
		if (err)
		{
			PMXSPT("PMSTICN", 30);
			goto RTCC_PMSTICN_7_1;
		}
	}
	if (T1 >= T2)
	{
		double dt;

		OrbMech::time_theta(opt.sv_P.R, opt.sv_P.V, WT, mu, dt);
		T2 = T1 + dt;
	}
	if (opt.IVFLAG == 1)
	{
		TMAX = T2 + opt.TimeRange;
	}
	else if (opt.IVFLAG == 2)
	{
		TMAX = T1 + opt.TimeRange;
	}
RTCC_PMSTICN_3_3:
	sv_A1 = coast(opt.sv_A, T1 - opt.sv_A.GMT);
	sv_P1 = coast(opt.sv_P, T1 - opt.sv_P.GMT);
	//TBD: Error?
	if (!(opt.mode == 3 || opt.mode == 4 || opt.mode == 5))
	{
		if (soln == 0)
		{
			//Save state vector
			PZMYSAVE.SV_mult[0] = sv_A1;
			PZMYSAVE.SV_mult[1] = sv_P1;
		}
	}
RTCC_PMSTICN_4_2:
	err = PMMTIS(sv_A1, sv_P1, T2 - T1, DH, PhaseAngle, sv_A1_apo, sv_A2, sv_A2_apo);
	if (opt.mode == 5)
	{
		goto RTCC_PMSTICN_9_2;
	}
	if (opt.mode == 3)
	{
		goto RTCC_PMSTICN_11_2;
	}
	if (opt.mode == 4)
	{
		goto RTCC_PMSTICN_12_2;
	}
	if (err)
	{
		goto RTCC_PMSTICN_7_1;
	}
	//Store DVs and times for data table
	entry[soln].DELV1 = length(sv_A1_apo.V - sv_A1.V);
	entry[soln].DELV2 = length(sv_A2_apo.V - sv_A2.V);
	entry[soln].Time1 = sv_A1.GMT;
	entry[soln].Time2 = sv_A2.GMT;
	PMSTICN_PY(sv_A1_apo.R, sv_A1_apo.V, sv_A1.R, sv_A1.V, entry[soln].PITCH1, entry[soln].YAW1);
	PMSTICN_PY(sv_A2_apo.R, sv_A2_apo.V, sv_A2.R, sv_A2.V, entry[soln].PITCH2, entry[soln].YAW2);
	DVT = length(sv_A2_apo.V - sv_A2.V) + length(sv_A1_apo.V - sv_A1.V);
	if (soln == 0)
	{
		//TBD: Compute next environment change following frozen maneuver of first plan
	}
	//TBD: Compute next environment change following variable maneuver of this plan
	
	//Compute TPI time under special conditions
	entry[soln].T_TPI = 0.0;
	if (PZTIPREG.showTPI)
	{
		double T3;

		err = PMSTICN_ELEV(sv_A2_apo, sv_P1, Elev, mu, T3);
		if (err == 0)
		{
			entry[soln].T_TPI = T3;
		}
	}
	soln++;
RTCC_PMSTICN_7_1:
	if (opt.IVFLAG < 1)
	{
		goto RTCC_PMSTICN_8_3;
	}
	if (opt.IVFLAG == 1)
	{
		//Increment time of 2nd maneuver
		T2 += opt.TimeStep;
	}
	else
	{
		//Increment time of 1st maneuver
		T1 += opt.TimeStep;
		if (T1 >= T2)
		{
			goto RTCC_PMSTICN_8_3;
		}
	}
	//Has the number of requested cases been computed?
	if (opt.IVFLAG == 1 && T2 > TMAX)
	{
		goto RTCC_PMSTICN_8_3;
	}
	if (opt.IVFLAG == 2 && T1 > TMAX)
	{
		goto RTCC_PMSTICN_8_3;
	}
	//Was a solution available for the lost case?
	if (1 != 1)
	{
		goto RTCC_PMSTICN_8_2;
	}
	//Did the last solution have the lowest fuel expenditure so far?
	if (DVT < DV_opt)
	{
		OPCASE = soln;
		DV_opt = DVT;
	}
	if (soln >= 13)
	{
		if (OPCASE > 7)
		{
			//Set number of solutions = 12 and delete the 1st solution of present 13 available
			for (int i = 0;i < 12;i++)
			{
				entry[i] = entry[i + 1];
			}
			OPCASE--;
			soln = 12;
		}
		else
		{
			goto RTCC_PMSTICN_8_3;
		}
	}
RTCC_PMSTICN_8_2:
	//Restore elements saved at the time of the first maneuver of the first solution computed
	sv_A1 = PZMYSAVE.SV_mult[0];
	sv_P1 = PZMYSAVE.SV_mult[1];
	if (opt.IVFLAG == 2)
	{
		goto RTCC_PMSTICN_3_3;
	}
	goto RTCC_PMSTICN_4_2;
RTCC_PMSTICN_8_3:
	for (int i = 0;i < soln;i++)
	{
		PZTIPREG.data[i] = entry[i];
	}
	PZTIPREG.Solutions = soln;
	PZTIPREG.IVFLAG = opt.IVFLAG;
	PZTIPREG.MAN_VEH = opt.ChaserVehicle;
	PZTIPREG.Updating = false;
	display = 63;
	goto RTCC_PMSTICN_24_2;
RTCC_PMSTICN_9_1:
	DH = opt.DH;
	PhaseAngle = opt.PhaseAngle;
	Elev = opt.Elev;
	WT = opt.WT;
	T1 = opt.T1;
	T2 = opt.T2;
	//Let the external request go through the full logic
	goto RTCC_PMSTICN_3_1;
	//goto RTCC_PMSTICN_4_2;
RTCC_PMSTICN_9_2:
	if (err)
	{
		//DKI error return
		res.SolutionFound = false;
	}
	else
	{
		//Pass solution back
		res.sv_tig = sv_A1;
		res.dV = sv_A1_apo.V - sv_A1.V;
		res.dV2 = sv_A2_apo.V - sv_A2.V;
		res.dV_LVLH = mul(OrbMech::LVLH_Matrix(sv_A1.R, sv_A1.V), res.dV);
		res.dV_LVLH2 = mul(OrbMech::LVLH_Matrix(sv_A2.R, sv_A2.V), res.dV2);
		res.T1 = GETfromGMT(T1);
		res.T2 = GETfromGMT(T2);
		res.SolutionFound = true;
	}
	goto RTCC_PMSTICN_24_3;
RTCC_PMSTICN_10_1:
	if (opt.SingSolTable == 1)
	{
		if (opt.SingSolNum > PZTIPREG.Solutions)
		{
			PMXSPT("PMSTICN", 29);
			goto RTCC_PMSTICN_24_3;
		}
		T1 = PZTIPREG.data[opt.SingSolNum - 1].Time1;
		T2 = PZTIPREG.data[opt.SingSolNum - 1].Time2;
	}
	else
	{
		if (opt.SingSolNum > PZTIPCCD.Solutions)
		{
			PMXSPT("PMSTICN", 29);
			goto RTCC_PMSTICN_24_3;
		}
	}
	//Get original state vectors for requested solution
	if (opt.SingSolTable == 1)
	{
		sv_A1 = PZMYSAVE.SV_mult[0];
		sv_P1 = PZMYSAVE.SV_mult[1];
	}
	else
	{
		sv_A1 = PZMYSAVE.SV_CC[0];
		sv_P1 = PZMYSAVE.SV_CC[1];
	}
	if (opt.SingSolTable == 2)
	{
		goto RTCC_PMSTICN_11_1;
	}
	//Set up STAIDs, mnvrs, threshold times and desired offsets for re-computation of desired solution
	DH = GZGENCSN.TIDeltaH;
	PhaseAngle = GZGENCSN.TIPhaseAngle;
	//Is same?
	if (T1 == sv_A1.GMT)
	{
		goto RTCC_PMSTICN_4_2;
	}
	else
	{
		goto RTCC_PMSTICN_3_3;
	}
RTCC_PMSTICN_11_1:
	//Set up STAIDs, mnvrs, threshold times and desired offsets for re-computation of desired CC solution
	goto RTCC_PMSTICN_4_2;
RTCC_PMSTICN_11_2:
	if (err)
	{
		goto RTCC_PMSTICN_24_3;
	}
	//TBD: Compute pitch and yaw
	//TBD: PMMDAN twice
	//PMMTISS();
	//Set up single solution display
	goto RTCC_PMSTICN_24_2;
RTCC_PMSTICN_12_2:
	//Transfer plan
	if (err)
	{
		goto RTCC_PMSTICN_24_1;
	}

	PZMYSAVE.SV_before[0] = sv_A1;
	PZMYSAVE.V_after[0] = sv_A1_apo.V;
	PZMYSAVE.SV_before[1] = sv_A2;
	PZMYSAVE.V_after[1] = sv_A2_apo.V;
	PZMYSAVE.code[0] = "NC1";
	PZMYSAVE.code[1] = "NC2";
	if (opt.SingSolTable == 1)
	{
		PZMYSAVE.plan[0] = PZTIPREG.MAN_VEH;
		PZMYSAVE.plan[1] = PZTIPREG.MAN_VEH;
	}
	else
	{
		PZMYSAVE.plan[0] = PZTIPCCD.MAN_VEH;
		PZMYSAVE.plan[1] = PZTIPCCD.MAN_VEH;
	}

	PMMMED("72", str);
	return;
RTCC_PMSTICN_13_1:
	//Corrective Combination
	//Load MED request quantities
	T1 = opt.T1;
	//Set C.C. Table to updating condition
	PZTIPCCD.Updating = true;
	//DTREAD: GZGENCSN Blks. 12-15
	WT = GZGENCSN.TITravelAngle;
	T_WSR = GZGENCSN.TINSRNominalTime;
	DH_WSR = GZGENCSN.TINSRNominalDeltaH;
	theta_WSR = GZGENCSN.TINSRNominalPhaseAngle;
	//Build first block of corrective combination table (Station IDs, threshold times, time of 1st maneuver)
	//Advance chaser and target to the time of the NCC maneuver
	sv_A1 = coast(opt.sv_A, T1 - opt.sv_A.GMT);
	sv_P1 = coast(opt.sv_P, T1 - opt.sv_P.GMT);
	//TBD: AEG error?
	//Save initial elements for regeneration
	PZMYSAVE.SV_CC[0] = sv_A1;
	PZMYSAVE.SV_CC[1] = sv_P1;
	//Initialize optimum DV to max, height offset and time of NSR maneuver to MED request minimum
	DV_opt = 10000000.0;
	DH = opt.DH_min;
	T2 = opt.T2_min;
	theta_T_min = theta_WSR - (opt.T2_min - T_WSR)*K*DH_WSR;
	if (theta_T_min == 0.0 || DH_WSR == 0.0)
	{
		//Error
		goto RTCC_PMSTICN_18_1;
	}
	//Compute phase angle for 1st case
	HthetaR = DH_WSR / theta_T_min;
	PhaseAngle = opt.DH_min / HthetaR;
RTCC_PMSTICN_15_1:
	du_dot = DH * K;
	DV_TP = abs((D[0] + WT * (D[1] + WT * (D[2] + WT * D[3])))*DH);//*CTERMV;
	//Is this a slip time option request?
	if (opt.CCReqInd == 1)
	{
		theta_max = PhaseAngle - opt.TPILimit * du_dot;
		theta_min = PhaseAngle + opt.TPILimit * du_dot;
		PhaseAngle = theta_min;
	}
	else
	{
		T2 = opt.T2_min;
	}
RTCC_PMSTICN_15_2:
	err = PMMTIS(sv_A1, sv_P1, T2 - T1, DH, PhaseAngle, sv_A1_apo, sv_A2, sv_A2_apo);
	if (err == 0)
	{
		DVT = length(sv_A2_apo.V - sv_A2.V) + length(sv_A1_apo.V - sv_A1.V) + DV_TP;
		if (DVT < DV_opt)
		{
			DV_OPTH = DVT;
			DH_OPTH = DH;
			TIME_OPTH = T2;
			theta_OPTH = PhaseAngle;
		}
	}
	//Restore original chaser and target elements for next case
	sv_A1 = PZMYSAVE.SV_CC[0];
	sv_P1 = PZMYSAVE.SV_CC[1];
	//Compute phase angle for next solution
	PhaseAngle = PhaseAngle - du_dot * opt.dt_inc;
	//Is this a slip time option request?
	if (opt.CCReqInd == 1)
	{
		goto RTCC_PMSTICN_17_1;
	}
	//Compute next NSR time at this height
	T2 = T2 + opt.dt_inc;
	//Is the new NSR time past the MED maximum?
	if (T2 >= opt.T2_max)
	{
		goto RTCC_PMSTICN_17_2;
	}
	goto RTCC_PMSTICN_15_2;
RTCC_PMSTICN_17_1:
	//Have all phase angles been tried at this height?
	if (PhaseAngle > theta_max)
	{
		goto RTCC_PMSTICN_15_2;
	}
RTCC_PMSTICN_17_2:
	if (err == 0)
	{
		//Reset optimum to maximum
		DV_opt = 100000000.0;
		if (opt.CCReqInd == 1)
		{
			T_slip = (theta_min - opt.TPILimit*du_dot - theta_OPTH) / du_dot;
		}
		else
		{
			T_slip = 0.0;
		}
		if (soln >= 13)
		{
			goto RTCC_PMSTICN_18_1;
		}
	}
	DH = DH + opt.dh_inc;
	if (DH > opt.DH_max)
	{
		goto RTCC_PMSTICN_18_1;
	}
	PhaseAngle = DH / HthetaR;
	goto RTCC_PMSTICN_15_1;
RTCC_PMSTICN_18_1:
	//Write solutions
	//Set up display
	display = 0;
	goto RTCC_PMSTICN_24_2;
RTCC_PMSTICN_24_1:
	PMXSPT("PMSTICN", 29);
	if (opt.mode == 1)
	{
		goto RTCC_PMSTICN_18_1;
	}
	if (opt.mode == 2)
	{
		goto RTCC_PMSTICN_7_1;
	}
	goto RTCC_PMSTICN_24_3;
RTCC_PMSTICN_24_2:
	EMSNAP(0, display);
RTCC_PMSTICN_24_3:
	return;
}

void RTCC::LambertTargeting(LambertMan *lambert, TwoImpulseResuls &res)
{
	SV sv_A1, sv_A1_apo, sv_A2, sv_P1, sv_P2;
	double GETbase, dt1, dt1_apo, dt2, mu, T1, T2;
	int N;
	OBJHANDLE gravref;
	int body;
	bool prograde;

	GETbase = CalcGETBase();
	gravref = lambert->sv_A.gravref;
	N = lambert->N;

	if (gravref == oapiGetObjectByName("Earth"))	//Hardcoded: Always prograde for Earth, always retrograde for Moon
	{
		prograde = true;
		mu = OrbMech::mu_Earth;
		body = BODY_EARTH;
	}
	else
	{
		prograde = false;
		mu = OrbMech::mu_Moon;
		body = BODY_MOON;
	}

	if (lambert->mode == 2)
	{
		if (lambert->T1 >= 0)
		{
			T1 = lambert->T1;
		}
		else
		{
			T1 = TPISearch(lambert->sv_A, lambert->sv_P, lambert->ElevationAngle);
		}
	}
	else
	{
		T1 = lambert->T1;
	}

	dt1 = T1 - (lambert->sv_A.MJD - GETbase) * 24.0 * 60.0 * 60.0;
	dt1_apo = T1 - (lambert->sv_P.MJD - GETbase) * 24.0 * 60.0 * 60.0;

	if (lambert->Perturbation == 1)
	{
		sv_A1 = coast(lambert->sv_A, dt1);
		sv_P1 = coast(lambert->sv_P, dt1_apo);
	}
	else
	{
		OrbMech::rv_from_r0v0(lambert->sv_A.R, lambert->sv_A.V, dt1, sv_A1.R, sv_A1.V, mu);
		OrbMech::rv_from_r0v0(lambert->sv_P.R, lambert->sv_P.V, dt1_apo, sv_P1.R, sv_P1.V, mu);
	}

	if (lambert->mode == 2)
	{
		if (lambert->T2 >= 0)
		{
			T2 = lambert->T2;
		}
		else
		{
			double dt;

			OrbMech::time_theta(sv_P1.R, sv_P1.V, lambert->TravelAngle, mu, dt);
			T2 = T1 + dt;
		}
	}
	else
	{
		T2 = lambert->T2;
	}

	dt2 = T2 - T1;

	if (lambert->Perturbation == 1)
	{
		sv_P2 = coast(lambert->sv_P, dt1_apo + dt2);
	}
	else
	{
		OrbMech::rv_from_r0v0(lambert->sv_P.R, lambert->sv_P.V, dt1_apo + dt2, sv_P2.R, sv_P2.V, mu);
	}

	MATRIX3 Q_Xx;
	VECTOR3 RP2off, VP2off, VA1_apo;

	if (lambert->mode == 0)
	{
		double angle;
		angle = lambert->Offset.x / length(sv_P2.R);

		OrbMech::rv_from_r0v0_ta(sv_P2.R, sv_P2.V, angle, RP2off, VP2off, mu);

		VECTOR3 i, j, k;
		MATRIX3 Q_Xx2;

		k = -unit(RP2off);
		j = unit(crossp(VP2off, RP2off));
		i = crossp(j, k);
		Q_Xx2 = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

		RP2off = RP2off + tmul(Q_Xx2, _V(0.0, lambert->Offset.y, lambert->Offset.z));
	}
	else
	{
		if (lambert->PhaseAngle != 0 || lambert->DH != 0)
		{
			CELEMENTS elem_T, elem_CE;
			double f_T, f_CE;
			elem_T = OrbMech::GIMIKC(sv_P2.R, sv_P2.V, mu);
			f_T = OrbMech::MeanToTrueAnomaly(elem_T.l, elem_T.e);
			elem_CE.a = elem_T.a - lambert->DH;
			elem_CE.e = elem_T.e*elem_T.a / elem_CE.a;
			f_CE = f_T - lambert->PhaseAngle;
			elem_CE.l = OrbMech::TrueToMeanAnomaly(f_CE, elem_CE.e);
			elem_CE.i = elem_T.i;
			elem_CE.g = elem_T.g;
			elem_CE.h = elem_T.h;
			OrbMech::GIMKIC(elem_CE, mu, RP2off, VP2off);
		}
		else
		{
			RP2off = sv_P2.R;
			VP2off = sv_P2.V;
		}
	}

	if (lambert->Perturbation == RTCC_LAMBERT_PERTURBED)
	{
		VA1_apo = OrbMech::Vinti(SystemParameters.AGCEpoch, sv_A1.R, sv_A1.V, RP2off, sv_A1.MJD, dt2, N, prograde, body, body, body, _V(0.0, 0.0, 0.0), 100.0*0.3048); //Vinti Targeting: For non-spherical gravity
	
		sv_A1_apo = sv_A1;
		sv_A1_apo.V = VA1_apo;
		sv_A2 = coast(sv_A1_apo, dt2);
	}
	else
	{
		if (lambert->axis == RTCC_LAMBERT_MULTIAXIS)
		{
			VA1_apo = OrbMech::elegant_lambert(sv_A1.R, sv_A1.V, RP2off, dt2, N, prograde, mu);	//Lambert Targeting
		}
		else
		{
			OrbMech::xaxislambert(sv_A1.R, sv_A1.V, RP2off, dt2, N, prograde, mu, VA1_apo, lambert->Offset.z);	//Lambert Targeting
		}

		sv_A1_apo = sv_A1;
		sv_A1_apo.V = VA1_apo;
		sv_A2 = sv_A1_apo;
		OrbMech::rv_from_r0v0(sv_A1_apo.R, sv_A1_apo.V, dt2, sv_A2.R, sv_A2.V, mu);
	}

	Q_Xx = OrbMech::LVLH_Matrix(sv_A1.R, sv_A1.V);
	res.dV = VA1_apo - sv_A1.V;
	res.dV2 = sv_P2.V - sv_A2.V;
	res.dV_LVLH = mul(Q_Xx, res.dV);
	res.T1 = T1;
	res.T2 = T2;

	if (lambert->axis == RTCC_LAMBERT_XAXIS)
	{
		res.dV_LVLH.y = 0.0;
	}

	if (lambert->mode == 1)
	{
		SV sv_A2_apo;
		VECTOR3 u, R_A2, V_A2, R_PC, V_PC, DV;
		double dt_TPI;

		sv_A2_apo = sv_A2;

		u = unit(crossp(sv_P2.R, sv_P2.V));

		R_A2 = unit(sv_A2.R - u * dotp(sv_A2.R, u))*length(sv_A2.R);
		V_A2 = unit(sv_A2.V - u * dotp(sv_A2.V, u))*length(sv_A2.V);

		OrbMech::RADUP(sv_P2.R, sv_P2.V, R_A2, mu, R_PC, V_PC);
		DV = OrbMech::CoellipticDV(R_A2, R_PC, V_PC, mu) - V_A2;
		sv_A2_apo.V += DV;

		dt_TPI = OrbMech::findelev(SystemParameters.AGCEpoch, sv_A2_apo.R, sv_A2_apo.V, sv_P2.R, sv_P2.V, sv_P2.MJD, lambert->ElevationAngle, gravref);
		res.t_TPI = OrbMech::GETfromMJD(sv_P2.MJD, GETbase) + dt_TPI;
	}

	if (lambert->storesolns == false) return;

	PZMYSAVE.SV_before[0] = ConvertSVtoEphemData(sv_A1);
	PZMYSAVE.V_after[0] = sv_A1_apo.V;
	PZMYSAVE.plan[0] = lambert->ChaserVehicle;
	if (lambert->mode == 0)
	{
		PZMYSAVE.code[0] = "LAM";
	}
	else if (lambert->mode == 1)
	{
		PZMYSAVE.code[0] = "NCC";
	}
	else
	{
		PZMYSAVE.code[0] = "TPI";
	}

	PZMYSAVE.SV_before[1] = ConvertSVtoEphemData(sv_A2);
	PZMYSAVE.V_after[1] = VP2off;
	PZMYSAVE.plan[1] = lambert->ChaserVehicle;
	if (lambert->mode == 0)
	{
		PZMYSAVE.code[1] = "TAR";
	}
	else if (lambert->mode == 1)
	{
		PZMYSAVE.code[1] = "NSR";
	}
	else
	{
		PZMYSAVE.code[1] = "TPF";
	}
}

void RTCC::LMThrottleProgram(double F, double v_e, double mass, double dV_LVLH, double &F_average, double &ManPADBurnTime, double &bt_var, int &step)
{
	double bt, dv_10p, mass40p, bt_40p, dv_40p, mass100p, bt_100p, time_10p, v_G, F_L, K4, mintime, t_throttleup;
	bool S_TH;

	F_L = 200.0*4.448222;
	K4 = 1050.0*4.448222;

	mintime = 4.0;
	t_throttleup = 26.0;

	v_G = dV_LVLH - 4.0*F_L / mass;
	if (v_G*mass / K4 < 95.0)
	{
		//LGC won't throttle up to 100% at 26 seconds
		S_TH = true;
	}
	else
	{
		//LGC will throttle up to 100% at 26 seconds
		S_TH = false;
	}

	if (mass > 30000.0)
	{
		//heavy docked burn, throttle up to 40% at 5 seconds
		time_10p = 5.0;
	}
	else
	{
		//undocked or light docked burn, throttle up to 40% at 15 seconds
		time_10p = 15.0;
	}

	//Throttle Profile:
	//Basic principle: spend at least 4 seconds at one throttle setting before cutoff
	//Burntime at 10% smaller than 21 seconds: full burn at 10%
	//

	bt = v_e / (0.1*F) *(mass)*(1.0 - exp(-dV_LVLH / v_e)); //burn time at 10% thrust

	if (bt > time_10p + mintime * 0.4 / 0.1)	//Burn longer than 4 seconds at 40%
	{
		dv_10p = v_e * log((mass) / (mass - 0.1*F / v_e * time_10p)); //Five seconds at 10%
		mass40p = mass - 0.1*F / v_e * time_10p;
		bt_40p = v_e / (0.4*F) *(mass40p)*(1.0 - exp(-(dV_LVLH - dv_10p) / v_e)); // rest burn time at 40% thrust

		if (S_TH == false)	//LGC will throttle up to 100% at 26 seconds
		{
			dv_40p = v_e * log((mass40p) / (mass40p - 0.4*F / v_e * (t_throttleup - time_10p))); //21 seconds at 40%
			mass100p = mass40p - 0.4*F / v_e * (t_throttleup - time_10p);
			bt_100p = v_e / F * (mass100p)*(1.0 - exp(-(dV_LVLH - dv_10p - dv_40p) / v_e)); // rest burn time at 100% thrust

			F_average = (0.1 * time_10p + 0.4*(t_throttleup - time_10p) + bt_100p) / (t_throttleup + bt_100p)*F;
			ManPADBurnTime = bt_100p + 26.0;
			bt_var = bt_100p;

			step = 2;
		}
		else
		{
			F_average = (0.1 * time_10p + 0.4*bt_40p) / (bt_40p + time_10p)*F;
			ManPADBurnTime = bt_40p + time_10p;
			bt_var = bt_40p;

			step = 1;
		}
	}
	else
	{
		F_average = 0.1*F;	//Full burn at 10%
		ManPADBurnTime = bt;
		bt_var = bt;

		step = 0;
	}
}

void RTCC::AP10CSIPAD(AP10CSIPADOpt *opt, AP10CSI &pad)
{
	SV sv1, sv2;
	MATRIX3 Q_Xx, M, M_R;
	VECTOR3 V_G, UX, UY, UZ, IMUangles, FDAIangles, dV_AGS;
	double dt, TIG_AGS;

	dt = opt->t_CSI - (opt->sv0.MJD - CalcGETBase()) * 24.0 * 60.0 * 60.0;
	sv1 = coast(opt->sv0, dt);

	PoweredFlightProcessor(sv1, opt->t_CSI, opt->enginetype, 0.0, opt->dV_LVLH, true, TIG_AGS, dV_AGS, false);

	Q_Xx = OrbMech::LVLH_Matrix(sv1.R, sv1.V);
	V_G = tmul(Q_Xx, opt->dV_LVLH);

	UX = unit(V_G);

	if (abs(dotp(UX, unit(sv1.R))) < cos(0.01*RAD))
	{
		UY = unit(crossp(UX, sv1.R));
	}
	else
	{
		UY = unit(crossp(UX, sv1.V));
	}
	UZ = unit(crossp(UX, UY));

	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), M_R));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.DEDA373 = (opt->t_CSI - opt->KFactor) / 60.0;
	pad.DEDA275 = (opt->t_TPI - opt->KFactor) / 60.0;
	pad.PLM_FDAI = FDAIangles.y*DEG;
	pad.t_CSI = opt->t_CSI;
	pad.t_TPI = opt->t_TPI;
	pad.dV_LVLH = opt->dV_LVLH / 0.3048;
	pad.dV_AGS = dV_AGS / 0.3048;
}

void RTCC::AP11LMManeuverPAD(AP11LMManPADOpt *opt, AP11LMMNV &pad)
{
	MATRIX3 M_R, M, Q_Xx;
	VECTOR3 V_G, X_B, UX, UY, UZ, IMUangles, FDAIangles;
	double dt, mu, CSMmass, ManPADBurnTime, apo, peri, ManPADApo, ManPADPeri, ManPADDVR, ManBSSpitch, ManBSSXPos, R_E, headsswitch, GETbase;
	int ManCOASstaroct;
	SV sv, sv1, sv2;

	GETbase = CalcGETBase();

	//State vector from PAD options or get a new one
	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}

	//Coast until TIG
	dt = opt->TIG - (sv.MJD - GETbase) * 24.0 * 60.0 * 60.0;
	sv1 = coast(sv, dt);

	//Docked mass
	if (opt->csmlmdocked == true)
	{
		CSMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		CSMmass = 0.0;
	}

	//Execute maneuver, output state vector at cutoff
	sv2 = ExecuteManeuver(sv1, opt->TIG, opt->dV_LVLH, CSMmass, opt->enginetype, Q_Xx, V_G);
	ManPADBurnTime = (sv2.MJD - sv1.MJD)*24.0*3600.0;

	//Only use landing site radius for the Moon
	if (sv1.gravref == oapiGetObjectByName("Moon"))
	{
		R_E = opt->R_LLS;
		mu = OrbMech::mu_Moon;
	}
	else
	{
		R_E = OrbMech::R_Earth;
		mu = OrbMech::mu_Earth;
	}

	OrbMech::periapo(sv2.R, sv2.V, mu, apo, peri);
	ManPADApo = apo - R_E;
	ManPADPeri = peri - R_E;

	if (opt->HeadsUp)
	{
		headsswitch = -1.0;
	}
	else
	{
		headsswitch = 1.0;
	}

	X_B = unit(V_G);
	if (opt->enginetype == RTCC_ENGINETYPE_LMRCSMINUS2 || opt->enginetype == RTCC_ENGINETYPE_LMRCSMINUS4)
	{
		UX = -X_B;
	}
	else
	{
		UX = X_B;
	}
	if (abs(dotp(UX, unit(sv1.R))) < cos(0.01*RAD))
	{
		UY = unit(crossp(UX, sv1.R*headsswitch));
	}
	else
	{
		UY = unit(crossp(UX, sv1.V));
	}
	UZ = unit(crossp(UX, UY));


	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);


	ManPADDVR = length(opt->dV_LVLH);

	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), M_R));
	pad.IMUAtt = IMUangles;

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	SV sv_sxt = coast(sv1, opt->sxtstardtime);

	OrbMech::coascheckstar(EZJGSTAR, opt->REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD), sv_sxt.R, oapiGetSize(sv1.gravref), ManCOASstaroct, ManBSSpitch, ManBSSXPos);
	
	pad.Att = _V(OrbMech::imulimit(FDAIangles.x*DEG), OrbMech::imulimit(FDAIangles.y*DEG), OrbMech::imulimit(FDAIangles.z*DEG));
	pad.BSSStar = ManCOASstaroct;
	pad.burntime = ManPADBurnTime;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.dV_AGS = mul(Q_Xx, V_G) / 0.3048;
	pad.GETI = opt->TIG;
	pad.HA = min(9999.9, ManPADApo / 1852.0);
	pad.HP = ManPADPeri / 1852.0;

	pad.SPA = ManBSSpitch*DEG;
	pad.SXP = ManBSSXPos*DEG;
	pad.dVR = ManPADDVR / 0.3048;
	pad.CSMWeight = CSMmass / 0.45359237;
	pad.LMWeight = sv1.mass / 0.45359237;
}

void RTCC::AP11ManeuverPAD(AP11ManPADOpt *opt, AP11MNV &pad)
{
	PMMRKJInputArray integin;
	int Ierr;
	RTCCNIAuxOutputTable aux;
	VECTOR3 IMUangles, GDCangles;
	double dt, LMmass, mu, apo, peri, ManPADApo, ManPADPeri, ManPADPTrim, ManPADYTrim;
	double Mantrunnion, Manshaft, ManBSSpitch, ManBSSXPos, R_E;
	int GDCset, Manstaroct, ManCOASstaroct;
	SV sv, sv1, sv2;

	//Get initial state vector
	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}

	//Get LM mass, depending on vesseltype
	if (opt->vesseltype == 1)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	//Calculate time of ullage on for burn simulation
	if (opt->enginetype == RTCC_ENGINETYPE_CSMSPS)
	{
		double dt_ullage_overlap;

		if (opt->UllageDT == 0.0)
		{
			dt_ullage_overlap = 0.0;
		}
		else
		{
			dt_ullage_overlap = SystemParameters.MCTSD9;
		}

		double get_bb = opt->TIG - opt->UllageDT + dt_ullage_overlap; //GET of burn begin (ullage)
		dt = get_bb - GETfromGMT(OrbMech::GETfromMJD(sv.MJD, GetGMTBase()));

		integin.DTU = opt->UllageDT;
	}
	else
	{
		dt = opt->TIG - (sv.MJD - CalcGETBase()) * 24.0 * 60.0 * 60.0;
		integin.DTU = 0.0;
	}
	sv1 = coast(sv, dt);

	//Settings for burn simulation
	integin.sv0 = ConvertSVtoEphemData(sv1);
	integin.CAPWT = sv.mass + LMmass;
	integin.KEPHOP = 0;
	integin.KAUXOP = true;
	integin.CSMWT = sv.mass;
	integin.LMAWT = LMmass;
	integin.MANOP = RTCC_ATTITUDE_PGNS_EXDV;
	integin.ThrusterCode = opt->enginetype;
	integin.UllageOption = opt->UllageThrusterOpt;
	if (opt->vesseltype == 1)
	{
		integin.IC = 13;
	}
	else
	{
		integin.IC = 1;
	}
	integin.TVC = 1;
	integin.KTRIMOP = -1;
	integin.DOCKANG = 0.0;
	integin.VG = opt->dV_LVLH;
	integin.HeadsUpDownInd = opt->HeadsUp;
	integin.ExtDVCoordInd = true;

	//Burn simulation
	CSMLMPoweredFlightIntegration numin(this, integin, Ierr, NULL, &aux);
	numin.PMMRKJ();

	//Store PAD data from inputs
	pad.GETI = opt->TIG;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.Weight = sv1.mass / 0.45359237;
	pad.LMWeight = LMmass / 0.45359237;

	//Store PAD data from burn simulation
	pad.burntime = aux.DT_B;
	pad.Vc = aux.DV_C / 0.3048;
	if (opt->enginetype == RTCC_ENGINETYPE_CSMRCSMINUS2 || opt->enginetype == RTCC_ENGINETYPE_CSMRCSMINUS4)
	{
		pad.Vc = -pad.Vc;
	}
	pad.Vt = length(pad.dV);//aux.DV / 0.3048;//
	
	//Calculate height of periapsis and apoapsis
	if (sv1.gravref == oapiGetObjectByName("Earth"))
	{
		mu = OrbMech::mu_Earth;
		R_E = OrbMech::R_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		R_E = opt->R_LLS;
	}

	OrbMech::periapo(aux.R_BO, aux.V_BO, mu, apo, peri);
	ManPADApo = apo - R_E;
	ManPADPeri = peri - R_E;
	pad.HA = min(9999.9, ManPADApo / 1852.0);
	pad.HP = ManPADPeri / 1852.0;

	//Attitude
	VECTOR3 X_P, Y_P, Z_P;
	X_P = _V(opt->REFSMMAT.m11, opt->REFSMMAT.m12, opt->REFSMMAT.m13);
	Y_P = _V(opt->REFSMMAT.m21, opt->REFSMMAT.m22, opt->REFSMMAT.m23);
	Z_P = _V(opt->REFSMMAT.m31, opt->REFSMMAT.m32, opt->REFSMMAT.m33);

	double MG, OG, IG, C;

	MG = asin(dotp(Y_P, aux.X_B));
	C = abs(MG);

	if (abs(C - PI05) < 0.0017)
	{
		OG = 0.0;
		IG = atan2(dotp(X_P, aux.Z_B), dotp(Z_P, aux.Z_B));
	}
	else
	{
		OG = atan2(-dotp(aux.Z_B, Y_P), dotp(aux.Y_B, Y_P));
		IG = atan2(-dotp(aux.X_B, Z_P), dotp(aux.X_B, X_P));
	}

	IMUangles = _V(OG, IG, MG);

	//Star checks
	GDCangles = OrbMech::backupgdcalignment(EZJGSTAR, opt->REFSMMAT, sv1.R, R_E, GDCset);

	SV sv_sxt = coast(sv1, opt->sxtstardtime);

	OrbMech::checkstar(EZJGSTAR, opt->REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD), sv_sxt.R, R_E, Manstaroct, Mantrunnion, Manshaft);
	OrbMech::coascheckstar(EZJGSTAR, opt->REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD), sv_sxt.R, R_E, ManCOASstaroct, ManBSSpitch, ManBSSXPos);

	pad.Att = _V(OrbMech::imulimit(IMUangles.x*DEG), OrbMech::imulimit(IMUangles.y*DEG), OrbMech::imulimit(IMUangles.z*DEG));
	pad.BSSStar = ManCOASstaroct;
	pad.GDCangles = _V(OrbMech::imulimit(GDCangles.x*DEG), OrbMech::imulimit(GDCangles.y*DEG), OrbMech::imulimit(GDCangles.z*DEG));

	if (opt->enginetype == RTCC_ENGINETYPE_CSMSPS)
	{
		sprintf(pad.PropGuid, "SPS/G&N");
	}
	else
	{
		sprintf(pad.PropGuid, "RCS/G&N");
	}

	//Trim angles
	if (opt->enginetype == RTCC_ENGINETYPE_CSMSPS)
	{
		ManPADPTrim = aux.P_G - SystemParameters.MCTSPP;
		ManPADYTrim = aux.Y_G - SystemParameters.MCTSYP;
		pad.pTrim = ManPADPTrim * DEG;
		pad.yTrim = ManPADYTrim * DEG;
	}
	else
	{
		pad.pTrim = 0.0;
		pad.yTrim = 0.0;
	}

	if (length(GDCangles) == 0.0)
	{
		sprintf(pad.SetStars, "N/A");
	}
	else
	{
		if (GDCset == 0)
		{
			sprintf(pad.SetStars, "Deneb, Vega");
		}
		else if (GDCset == 1)
		{
			sprintf(pad.SetStars, "Navi, Polaris");
		}
		else
		{
			sprintf(pad.SetStars, "Acrux, Atria");
		}
	}
	pad.Shaft = Manshaft*DEG;
	pad.SPA = ManBSSpitch*DEG;
	pad.Star = Manstaroct;
	pad.SXP = ManBSSXPos*DEG;
	pad.Trun = Mantrunnion*DEG;

	pad.GET05G = 0;
	pad.lat = 0;
	pad.lng = 0;
	pad.RTGO = 0;
}

void RTCC::AP7ManeuverPAD(AP7ManPADOpt *opt, AP7MNV &pad)
{
	PMMRKJInputArray integin;
	int Ierr;
	RTCCNIAuxOutputTable aux;
	VECTOR3 IMUangles;
	double dt, mu, apo, peri, ManPADApo, ManPADPeri, ManPADPTrim, ManPADYTrim, GMT_TIG, TotalMass;
	double R_E;
	EphemerisData sv1, sv2;

	GMT_TIG = GMTfromGET(opt->TIG);
	TotalMass = opt->CSMMass + opt->LMMass;

	//Calculate time of ullage on for burn simulation
	if (opt->enginetype == RTCC_ENGINETYPE_CSMSPS)
	{
		double dt_ullage_overlap;

		if (opt->UllageDT == 0.0)
		{
			dt_ullage_overlap = 0.0;
		}
		else
		{
			dt_ullage_overlap = SystemParameters.MCTSD9;
		}

		double gmt_bb = GMT_TIG - opt->UllageDT + dt_ullage_overlap; //GMT of burn begin (ullage)
		dt = gmt_bb - opt->sv0.GMT;

		integin.DTU = opt->UllageDT;
	}
	else
	{
		dt = GMT_TIG - opt->sv0.GMT;
		integin.DTU = 0.0;
	}
	sv1 = coast(opt->sv0, dt, TotalMass, opt->Area, 1.0, false);

	//Settings for burn simulation
	integin.sv0 = sv1;
	integin.CAPWT = TotalMass;
	integin.KEPHOP = 0;
	integin.KAUXOP = true;
	integin.CSMWT = opt->CSMMass;
	integin.LMAWT = opt->LMMass;
	integin.MANOP = RTCC_ATTITUDE_PGNS_EXDV;
	integin.ThrusterCode = opt->enginetype;
	integin.UllageOption = opt->UllageThrusterOpt;
	if (opt->LMMass)
	{
		integin.IC = 13;
	}
	else
	{
		integin.IC = 1;
	}
	integin.TVC = 1;
	integin.KTRIMOP = -1;
	integin.DOCKANG = 0.0;
	integin.VG = opt->dV_LVLH;
	integin.HeadsUpDownInd = opt->HeadsUp;
	integin.ExtDVCoordInd = true;

	//Burn simulation
	CSMLMPoweredFlightIntegration numin(this, integin, Ierr, NULL, &aux);
	numin.PMMRKJ();

	//Store PAD data from inputs
	pad.GETI = opt->TIG;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.Weight = opt->CSMMass / 0.45359237;

	//Store PAD data from burn simulation
	pad.burntime = aux.DT_B;
	pad.Vc = aux.DV_C / 0.3048;
	if (opt->enginetype == RTCC_ENGINETYPE_CSMRCSMINUS2 || opt->enginetype == RTCC_ENGINETYPE_CSMRCSMINUS4)
	{
		pad.Vc = -pad.Vc;
	}

	//Calculate height of periapsis and apoapsis
	if (sv1.RBI == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
		R_E = OrbMech::R_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		R_E = OrbMech::R_Moon;
	}

	OrbMech::periapo(aux.R_BO, aux.V_BO, mu, apo, peri);
	ManPADApo = apo - R_E;
	ManPADPeri = peri - R_E;
	pad.HA = min(9999.9, ManPADApo / 1852.0);
	pad.HP = ManPADPeri / 1852.0;

	//Attitude
	VECTOR3 X_P, Y_P, Z_P;
	X_P = _V(opt->REFSMMAT.m11, opt->REFSMMAT.m12, opt->REFSMMAT.m13);
	Y_P = _V(opt->REFSMMAT.m21, opt->REFSMMAT.m22, opt->REFSMMAT.m23);
	Z_P = _V(opt->REFSMMAT.m31, opt->REFSMMAT.m32, opt->REFSMMAT.m33);

	double MG, OG, IG, C;

	MG = asin(dotp(Y_P, aux.X_B));
	C = abs(MG);

	if (abs(C - PI05) < 0.0017)
	{
		OG = 0.0;
		IG = atan2(dotp(X_P, aux.Z_B), dotp(Z_P, aux.Z_B));
	}
	else
	{
		OG = atan2(-dotp(aux.Z_B, Y_P), dotp(aux.Y_B, Y_P));
		IG = atan2(-dotp(aux.X_B, Z_P), dotp(aux.X_B, X_P));
	}

	IMUangles = _V(OG, IG, MG);
	EphemerisData sv_sxt;

	sv_sxt = coast(sv1, opt->sxtstardtime, TotalMass, opt->Area, 1.0, false);

	OrbMech::checkstar(EZJGSTAR, opt->REFSMMAT, _V(round(IMUangles.x*DEG)*RAD, round(IMUangles.y*DEG)*RAD, round(IMUangles.z*DEG)*RAD), sv_sxt.R, R_E, pad.Star, pad.Trun, pad.Shaft);

	if (opt->navcheckGET != 0.0)
	{
		EphemerisData sv_nav;
		double alt, lat, lng;

		sv_nav = coast(sv1, GMTfromGET(opt->navcheckGET) - sv1.GMT, TotalMass, opt->Area, 1.0, false);

		navcheck(sv_nav.R, sv_nav.GMT, sv1.RBI, lat, lng, alt);

		pad.NavChk = opt->navcheckGET;
		pad.lat = lat*DEG;
		pad.lng = lng*DEG;
		pad.alt = alt / 1852;
	}

	pad.Att = _V(OrbMech::imulimit(IMUangles.x*DEG), OrbMech::imulimit(IMUangles.y*DEG), OrbMech::imulimit(IMUangles.z*DEG));

	//Trim angles
	if (opt->enginetype == RTCC_ENGINETYPE_CSMSPS)
	{
		ManPADPTrim = aux.P_G - SystemParameters.MCTSPP;
		ManPADYTrim = aux.Y_G - SystemParameters.MCTSYP;
		pad.pTrim = ManPADPTrim * DEG;
		pad.yTrim = ManPADYTrim * DEG;
	}
	else
	{
		pad.pTrim = 0.0;
		pad.yTrim = 0.0;
	}

	pad.Shaft *= DEG;
	pad.Trun *= DEG;
}

void RTCC::AP7TPIPAD(const AP7TPIPADOpt &opt, AP7TPI &pad)
{
	EphemerisData sv_A2, sv_P2, sv_A3, sv_P3;
	double dt1, dt2;
	VECTOR3 u, U_L, UX, UY, UZ, U_R, U_R2, U_P, TPIPAD_BT, TPIPAD_dV_LOS;
	MATRIX3 Rot1, Rot2;
	double TPIPAD_AZ, TPIPAD_R, TPIPAD_Rdot, TPIPAD_ELmin5,TPIPAD_dH, TPIPAD_ddH, TIG_GMT;
	VECTOR3 U_F, LOS, U_LOS, NN;

	TIG_GMT = GMTfromGET(opt.TIG);

	dt1 = TIG_GMT - opt.sv_A.GMT;
	dt2 = TIG_GMT - opt.sv_P.GMT;

	sv_A3 = coast(opt.sv_A, dt1);
	sv_P3 = coast(opt.sv_P, dt2);

	UY = unit(crossp(sv_A3.V, sv_A3.R));
	UZ = unit(-sv_A3.R);
	UX = crossp(UY, UZ);

	Rot1 = _M(UX.x, UY.x, UZ.x, UX.y, UY.y, UZ.y, UX.z, UY.z, UZ.z);

	u = unit(crossp(sv_A3.R, sv_A3.V));
	U_L = unit(sv_P3.R - sv_A3.R);
	UX = U_L;
	UY = unit(crossp(crossp(u, UX), UX));
	UZ = crossp(UX, UY);

	Rot2 = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);

	TPIPAD_dV_LOS = mul(Rot2, mul(Rot1, opt.dV_LVLH));
	//TPIPAD_dH = abs(length(RP3) - length(RA3));
	double F;

	F = 200.0 * 4.448222;
	TPIPAD_BT = _V(abs(0.5*TPIPAD_dV_LOS.x), abs(TPIPAD_dV_LOS.y), abs(TPIPAD_dV_LOS.z))*opt.mass / F;

	VECTOR3 i, j, k, dr, dv, dr0, dv0, Omega;
	MATRIX3 Q_Xx;
	double t1, t2, dxmin, n, dxmax;

	j = unit(sv_P3.V);
	k = unit(crossp(sv_P3.R, j));
	i = crossp(j, k);
	Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);
	dr = sv_A3.R - sv_P3.R;
	n = length(sv_P3.V) / length(sv_P3.R);
	Omega = k*n;
	dv = sv_A3.V - sv_P3.V - crossp(Omega, dr);
	dr0 = mul(Q_Xx, dr);
	dv0 = mul(Q_Xx, dv);
	t1 = 1.0 / n*atan(-dv0.x / (3.0*n*dr0.x + 2.0 * dv0.y));
	t2 = 1.0 / n*(atan(-dv0.x / (3.0*n*dr0.x + 2.0 * dv0.y)) + PI);
	dxmax = 4.0 * dr0.x + 2.0 / n*dv0.y + dv0.x / n*sin(n*t1) - (3.0 * dr0.x + 2.0 / n*dv0.y)*cos(n*t1);
	dxmin = 4.0 * dr0.x + 2.0 / n*dv0.y + dv0.x / n*sin(n*t2) - (3.0 * dr0.x + 2.0 / n*dv0.y)*cos(n*t2);

	sv_A2 = coast(sv_A3, -5.0*60.0);
	sv_P2 = coast(sv_P3, -5.0*60.0);

	U_R2 = unit(sv_P2.R - sv_A2.R);

	TPIPAD_dH = -dr0.x;
	TPIPAD_ddH = abs(dxmax - dxmin);
	TPIPAD_R = abs(length(sv_P2.R - sv_A2.R));
	TPIPAD_Rdot = dotp(sv_P2.V - sv_A2.V, U_R2);

	U_L = unit(sv_P2.R - sv_A2.R);
	U_P = unit(U_L - sv_A2.R*dotp(U_L, sv_A2.R) / length(sv_A2.R) / length(sv_A2.R));

	TPIPAD_ELmin5 = acos(dotp(U_L, U_P*OrbMech::sign(dotp(U_P, crossp(u, sv_A2.R)))));

	U_F = unit(crossp(crossp(sv_A2.R, sv_A2.V), sv_A2.R));
	U_R = unit(sv_A2.R);
	LOS = sv_P2.R - sv_A2.R;
	U_LOS = unit(LOS - U_R*dotp(LOS, U_R));
	TPIPAD_AZ = acos(dotp(U_LOS, U_F));//atan2(-TPIPAD_dV_LOS.z, TPIPAD_dV_LOS.x);
	NN = crossp(U_LOS, U_F);
	if (dotp(NN, sv_A2.R) < 0)
	{
		TPIPAD_AZ = PI2 - TPIPAD_AZ;
	}

	pad.AZ = TPIPAD_AZ*DEG;
	pad.Backup_bT = TPIPAD_BT;
	pad.Backup_dV = TPIPAD_dV_LOS / 0.3048;
	pad.R = TPIPAD_R / 1852.0;
	pad.Rdot = TPIPAD_Rdot / 0.3048;
	pad.EL = TPIPAD_ELmin5*DEG;
	pad.GETI = opt.TIG;
	pad.Vg = opt.dV_LVLH / 0.3048;
	pad.dH_TPI = TPIPAD_dH / 1852.0;
	pad.dH_Max = TPIPAD_ddH / 1852.0;
}

void RTCC::AP9LMTPIPAD(AP9LMTPIPADOpt *opt, AP9LMTPI &pad)
{
	EphemerisData sv_A1, sv_P1;
	MATRIX3 Rot1, Rot2, M;
	VECTOR3 u, U_L, dV_LOS, IMUangles, FDAIangles, R1, R2, R3;
	double R, Rdot;

	sv_A1 = coast(opt->sv_A, opt->GMT_TIG - opt->sv_A.GMT);
	sv_P1 = coast(opt->sv_P, opt->GMT_TIG - opt->sv_P.GMT);
	Rot1 = OrbMech::LVLH_Matrix(sv_A1.R, sv_A1.V);

	u = unit(crossp(sv_A1.R, sv_A1.V));
	U_L = unit(sv_P1.R - sv_A1.R);
	R1 = U_L;
	R2 = unit(crossp(crossp(u, R1), R1));
	R3 = crossp(R1, R2);
	Rot2 = _M(R1.x, R1.y, R1.z, R2.x, R2.y, R2.z, R3.x, R3.y, R3.z);

	dV_LOS = mul(Rot2, tmul(Rot1, opt->dV_LVLH));

	R = abs(length(sv_P1.R - sv_A1.R));
	Rdot = dotp(sv_P1.V - sv_A1.V, U_L);

	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), Rot1));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.Att = _V(OrbMech::imulimit(FDAIangles.x*DEG), OrbMech::imulimit(FDAIangles.y*DEG), OrbMech::imulimit(FDAIangles.z*DEG));
	pad.Backup_dV = dV_LOS / 0.3048;
	pad.dVR = length(opt->dV_LVLH) / 0.3048;
	pad.GETI = GETfromGMT(opt->GMT_TIG);
	pad.R = R / 1852.0;
	pad.Rdot = Rdot / 0.3048;
	pad.Vg = opt->dV_LVLH / 0.3048;
}

void RTCC::AP9LMCDHPAD(AP9LMCDHPADOpt *opt, AP9LMCDH &pad)
{
	SV sv_A1;
	MATRIX3 Rot1, M;
	VECTOR3 IMUangles, FDAIangles, V_G;

	sv_A1 = coast(opt->sv_A, opt->TIG - OrbMech::GETfromMJD(opt->sv_A.MJD, CalcGETBase()));
	Rot1 = OrbMech::LVLH_Matrix(sv_A1.R, sv_A1.V);
	V_G = tmul(Rot1, opt->dV_LVLH);

	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), Rot1));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.Pitch = OrbMech::imulimit(FDAIangles.y*DEG);
	pad.Vg_AGS = mul(Rot1, V_G) / 0.3048;
	pad.GETI = opt->TIG;
	pad.Vg = opt->dV_LVLH / 0.3048;
}

void RTCC::CSMDAPUpdate(VESSEL *v, AP10DAPDATA &pad, bool docked)
{
	double CSMmass, LMmass, p_T, y_T;

	CSMmass = v->GetMass();
	LMmass = GetDockedVesselMass(v);

	double T, WDOT;
	unsigned IC;
	if (docked)
	{
		IC = 13; //CSM + LM
	}
	else
	{
		IC = 1; //CSM
	}
	GIMGBL(CSMmass, LMmass, p_T, y_T, T, WDOT, RTCC_ENGINETYPE_CSMSPS, IC, 1, 0, 0.0);

	pad.ThisVehicleWeight = CSMmass / 0.45359237;
	pad.OtherVehicleWeight = LMmass / 0.45359237;
	pad.PitchTrim = (p_T - SystemParameters.MCTSPP)*DEG;
	pad.YawTrim = (y_T - SystemParameters.MCTSYP)*DEG;
}

void ConvertDPSGimbalAnglesToTrim(double P_G, double R_G, double &P_G_trim, double &R_G_trim)
{
	P_G_trim = 6.0*RAD + P_G;
	R_G_trim = 6.0*RAD + R_G;
}

void RTCC::LMDAPUpdate(VESSEL *v, AP10DAPDATA &pad, bool docked, bool asc)
{
	double CSMmass, LMmass;

	if (asc)
	{
		if (utils::IsVessel(v, utils::LEM)) {
			LEM *lem = (LEM *)v;
			LMmass = lem->GetAscentStageMass();
		}
		else
		{
			LMmass = v->GetMass();
		}
	}
	else
	{
		LMmass = v->GetMass();
	}

	CSMmass = GetDockedVesselMass(v);

	if (asc)
	{
		pad.PitchTrim = 6.0;
		pad.YawTrim = 6.0;
	}
	else
	{
		double T, WDOT, p_T, r_T;
		unsigned IC;
		if (docked)
		{
			IC = 13; //CSM + LM
		}
		else
		{
			IC = 12; //LM
		}
		GIMGBL(CSMmass, LMmass, p_T, r_T, T, WDOT, RTCC_ENGINETYPE_LMDPS, IC, 1, 0, 0.0);

		ConvertDPSGimbalAnglesToTrim(p_T, r_T, pad.PitchTrim, pad.YawTrim);

		pad.PitchTrim *= DEG;
		pad.YawTrim *= DEG;
	}

	pad.ThisVehicleWeight = LMmass / 0.45359237;
	pad.OtherVehicleWeight = CSMmass / 0.45359237;
}

void RTCC::EarthOrbitEntry(const EarthEntryPADOpt &opt, AP7ENT &pad)
{
	double r_EMS, r_EI, GMT_TIG;
	double dt2; //from shutdown to EI
	double dt3; //from EI to 0.05g
	VECTOR3 UX, UY, UZ, R05G, V05G, EIangles;
	VECTOR3 UXD, UYD, UZD;
	MATRIX3 M_R;
	RMMYNIInputTable entin;
	RMMYNIOutputTable entout;
	EphemerisData2 sv_EI;

	double ALFATRIM = -20.0*RAD;

	r_EMS = OrbMech::R_Earth + 284643.0*0.3048;
	r_EI = OrbMech::R_Earth + 400000.0*0.3048;

	GMT_TIG = GMTfromGET(opt.P30TIG);

	if (opt.preburn)
	{
		SV sv1;
		
		sv1 = ExecuteManeuver(opt.sv0, opt.P30TIG, opt.dV_LVLH, 0.0, opt.Thruster);

		dt2 = OrbMech::time_radius_integ(SystemParameters.AGCEpoch, sv1.R, sv1.V, sv1.MJD, r_EI, -1, sv1.gravref, sv1.gravref, sv_EI.R, sv_EI.V);
		sv_EI.GMT = OrbMech::GETfromMJD(sv1.MJD + dt2 / 3600.0 / 24.0, GetGMTBase());

		if (opt.Thruster == RTCC_ENGINETYPE_CSMSPS)
		{
			pad.dVTO[0] = -SystemParameters.MCTST5 / sv1.mass*SystemParameters.MCTSD5;
			pad.dVTO[0] /= 0.3048;
		}
		else
		{
			pad.dVTO[0] = 0.0;
		}
		if (opt.lat == 0)
		{
			double EntryRTGO, EntryVIO, EntryRET, lat, lng;
			EntryCalculations::Reentry(SystemParameters.MAT_J2000_BRCS, sv_EI.R, sv_EI.V, sv1.MJD + dt2 / 3600.0 / 24.0, false, lat, lng, EntryRTGO, EntryVIO, EntryRET);

			pad.Lat[0] = lat*DEG;
			pad.Lng[0] = lng*DEG;
		}
		else
		{
			pad.Lat[0] = opt.lat*DEG;
			pad.Lng[0] = opt.lng*DEG;
		}
	}
	else
	{
		dt2 = OrbMech::time_radius_integ(SystemParameters.AGCEpoch, opt.sv0.R, opt.sv0.V, opt.sv0.MJD, r_EI, -1, opt.sv0.gravref, opt.sv0.gravref, sv_EI.R, sv_EI.V);
		sv_EI.GMT = OrbMech::GETfromMJD(opt.sv0.MJD + dt2 / 24.0 / 3600.0, GetGMTBase());
	}

	EphemerisData2 sv_EI_ECT;

	ELVCNV(sv_EI, 0, 1, sv_EI_ECT);

	entin.R0 = sv_EI_ECT.R;
	entin.V0 = sv_EI_ECT.V;
	entin.GMT0 = sv_EI_ECT.GMT;
	entin.lat_T = opt.lat;
	entin.lng_T = opt.lng;
	entin.KSWCH = 3;
	entin.C10 = opt.InitialBank;
	entin.g_c_GN = opt.GLevel;

	RMMYNI(entin, entout);

	dt3 = OrbMech::time_radius_integ(SystemParameters.AGCEpoch, sv_EI.R, sv_EI.V, OrbMech::MJDfromGET(sv_EI.GMT, GetGMTBase()), r_EMS, -1, opt.sv0.gravref, opt.sv0.gravref, R05G, V05G);

	UX = unit(-V05G);
	UY = unit(crossp(UX, -R05G));
	UZ = unit(crossp(UX, crossp(UX, -R05G)));

	UYD = UY * cos(opt.InitialBank) + UZ * sin(opt.InitialBank);
	UXD = unit(crossp(UYD, UX))*sin(ALFATRIM) + UX * cos(ALFATRIM);
	UZD = crossp(UXD, UYD);

	M_R = _M(UXD.x, UXD.y, UXD.z, UYD.x, UYD.y, UYD.z, UZD.x, UZD.y, UZD.z);
	EIangles = OrbMech::CALCGAR(opt.REFSMMAT, M_R);

	if (opt.preburn)
	{
		pad.Ret05[0] = entout.t_05g - GMT_TIG;
		pad.Ret2[0] = entout.t_2g - GMT_TIG;
		pad.DRE[0] = entout.DRE_2g / 1852.0;
		pad.RTGO[0] = entout.R_EMS / 1852.0;
		pad.VIO[0] = entout.V_EMS / 0.3048;
		pad.RetBBO[0] = entout.t_BBO - GMT_TIG;
		pad.RetEBO[0] = entout.t_EBO - GMT_TIG;
		pad.RetDrog[0] = entout.t_drogue - GMT_TIG;
		pad.Att400K[0] = _V(OrbMech::imulimit(EIangles.x*DEG), OrbMech::imulimit(EIangles.y*DEG), OrbMech::imulimit(EIangles.z*DEG));
	}
	else
	{
		pad.PB_RTGO[0] = entout.R_EMS / 1852.0;
		pad.PB_Ret05[0] = entout.t_05g - GMT_TIG;
		pad.PB_Ret2[0] = entout.t_2g - GMT_TIG;
		pad.PB_DRE[0] = entout.DRE_2g / 1852.0;
		pad.PB_VIO[0] = entout.V_EMS / 0.3048;
		pad.PB_RetBBO[0] = entout.t_BBO - GMT_TIG;
		pad.PB_RetEBO[0] = entout.t_EBO - GMT_TIG;
		pad.PB_RetDrog[0] = entout.t_drogue - GMT_TIG;
		pad.PB_R400K[0] = EIangles.x*DEG;
	}
}

void RTCC::LunarEntryPAD(LunarEntryPADOpt *opt, AP11ENT &pad)
{
	VECTOR3 UX, UY, UZ, EIangles, UREI;
	MATRIX3 M_R;
	double dt, dt2, EIAlt, EMSAlt, S_FPA, liftline, EntryPADV400k;
	double EntryPADDO, EntryPADgamma400k, EntryPADHorChkGET, EIGET, EntryPADHorChkPit;

	SV sv1;		// "Now" or just after the maneuver
	SV svEI;	// EI/400K
	SV sv05G;   // EMS Altitude / 0.05G

	EIAlt = 400000.0*0.3048;
	EMSAlt = 297431.0*0.3048;

	if (opt->direct || length(opt->dV_LVLH) == 0.0)	//Check against a DV of 0
	{
		sv1 = opt->sv0;
	}
	else
	{
		sv1 = ExecuteManeuver(opt->sv0, opt->P30TIG, opt->dV_LVLH, 0.0, RTCC_ENGINETYPE_CSMSPS);
	}

	if (sv1.gravref == hMoon)
	{
		double dt_r = OrbMech::time_radius(sv1.R, sv1.V, 64373760.0, 1.0, OrbMech::mu_Moon);
		sv1 = coast(sv1, dt_r);
	}

	dt = OrbMech::time_radius_integ(SystemParameters.AGCEpoch, sv1.R, sv1.V, sv1.MJD, OrbMech::R_Earth + EIAlt, -1, sv1.gravref, hEarth, svEI.R, svEI.V);
	svEI.gravref = hEarth;
	svEI.mass = sv1.mass;
	svEI.MJD = sv1.MJD + dt / 24.0 / 3600.0;

	RMMYNIInputTable entin;
	RMMYNIOutputTable entout;
	EphemerisData2 sv_EI_eph, sv_EI_ECT;

	sv_EI_eph.R = svEI.R;
	sv_EI_eph.V = svEI.V;
	sv_EI_eph.GMT = OrbMech::GETfromMJD(svEI.MJD, GetGMTBase());

	ELVCNV(sv_EI_eph, 0, 1, sv_EI_ECT);

	entin.R0 = sv_EI_ECT.R;
	entin.V0 = sv_EI_ECT.V;
	entin.GMT0 = sv_EI_ECT.GMT;
	entin.lat_T = opt->lat;
	entin.lng_T = opt->lng;
	entin.KSWCH = 3;

	RMMYNI(entin, entout);

	dt2 = OrbMech::time_radius_integ(SystemParameters.AGCEpoch, svEI.R, svEI.V, svEI.MJD, OrbMech::R_Earth + EMSAlt, -1, hEarth, hEarth, sv05G.R, sv05G.V);
	sv05G.gravref = hEarth;
	sv05G.mass = svEI.mass;
	sv05G.MJD = svEI.MJD + dt2 / 24.0 / 3600.0;

	UX = unit(-sv05G.V);
	UY = unit(crossp(UX, -sv05G.R));
	UZ = unit(crossp(UX, crossp(UX, -sv05G.R)));

	double ALFATRIM = -20.0*RAD;
	VECTOR3 UXD, UYD, UZD;

	UYD = UY;
	UXD = unit(crossp(UYD, UX))*sin(ALFATRIM) + UX*cos(ALFATRIM);
	UZD = crossp(UXD, UYD);

	M_R = _M(UXD.x, UXD.y, UXD.z, UYD.x, UYD.y, UYD.z, UZD.x, UZD.y, UZD.z);
	EIangles = OrbMech::CALCGAR(opt->REFSMMAT, M_R);

	UREI = unit(svEI.R);
	EntryPADV400k = length(svEI.V);
	S_FPA = dotp(UREI, svEI.V) / EntryPADV400k;
	EntryPADgamma400k = asin(S_FPA);
	EIGET = GETfromGMT(sv_EI_eph.GMT);

	double vei;
	vei = length(svEI.V) / 0.3048;

	EntryPADDO = 4.2317708e-9*vei*vei + 1.4322917e-6*vei - 1.600664062;

	liftline = 4.055351e-10*vei*vei - 3.149125e-5*vei + 0.503280635;
	if (S_FPA > atan(liftline))
	{
		sprintf(pad.LiftVector[0], "DN");
	}
	else
	{
		sprintf(pad.LiftVector[0], "UP");
	}

	SV svHorCheck, svSxtCheck;
	svHorCheck = coast(svEI, -17.0*60.0);
	svSxtCheck = coast(svEI, -60.0*60.0);

	EntryPADHorChkGET = EIGET - 17.0*60.0;

	double horang, coastang, IGA, cosIGA, sinIGA;
	VECTOR3 X_NB, Y_NB, Z_NB, X_SM, Y_SM, Z_SM, A_MG;

	horang = asin(OrbMech::R_Earth / length(svHorCheck.R));
	coastang = dotp(unit(svEI.R), unit(svHorCheck.R));

	Z_NB = unit(-svEI.R);
	Y_NB = unit(crossp(svEI.V, svEI.R));
	X_NB = crossp(Y_NB, Z_NB);

	X_SM = _V(opt->REFSMMAT.m11, opt->REFSMMAT.m12, opt->REFSMMAT.m13);
	Y_SM = _V(opt->REFSMMAT.m21, opt->REFSMMAT.m22, opt->REFSMMAT.m23);
	Z_SM = _V(opt->REFSMMAT.m31, opt->REFSMMAT.m32, opt->REFSMMAT.m33);
	A_MG = unit(crossp(X_NB, Y_SM));
	cosIGA = dotp(A_MG, Z_SM);
	sinIGA = dotp(A_MG, X_SM);
	IGA = atan2(sinIGA, cosIGA);

	EntryPADHorChkPit = PI2 - (horang + coastang + 31.7*RAD) + IGA;

	VECTOR3 SextantStarCheckAtt;
	double Entrytrunnion, Entryshaft, EntryBSSpitch, EntryBSSXPos;
	int Entrystaroct, EntryCOASstaroct;

	//Sextant star check either at entry attitude or at horizon check attitude
	if (opt->SxtStarCheckAttitudeOpt)
	{
		SextantStarCheckAtt = _V(OrbMech::round(EIangles.x*DEG)*RAD, OrbMech::round(EIangles.y*DEG)*RAD, OrbMech::round(EIangles.z*DEG)*RAD);
	}
	else
	{
		SextantStarCheckAtt = _V(0, OrbMech::round(EntryPADHorChkPit*DEG)*RAD, 0);
	}

	OrbMech::checkstar(EZJGSTAR, opt->REFSMMAT, SextantStarCheckAtt, svSxtCheck.R, OrbMech::R_Earth, Entrystaroct, Entrytrunnion, Entryshaft);
	OrbMech::coascheckstar(EZJGSTAR, opt->REFSMMAT, SextantStarCheckAtt, svSxtCheck.R, OrbMech::R_Earth, EntryCOASstaroct, EntryBSSpitch, EntryBSSXPos);

	pad.Att05[0] = _V(OrbMech::imulimit(EIangles.x*DEG), OrbMech::imulimit(EIangles.y*DEG), OrbMech::imulimit(EIangles.z*DEG));
	pad.BSS[0] = EntryCOASstaroct;
	pad.DLMax[0] = 0.0;
	pad.DLMin[0] = 0.0;
	pad.DO[0] = EntryPADDO;
	pad.Gamma400K[0] = EntryPADgamma400k*DEG;
	pad.GETHorCheck[0] = EntryPADHorChkGET;
	pad.Lat[0] = opt->lat*DEG;
	pad.Lng[0] = opt->lng*DEG;
	pad.MaxG[0] = entout.gmax;
	pad.PitchHorCheck[0] = OrbMech::imulimit(EntryPADHorChkPit*DEG);
	pad.RET05[0] = entout.t_05g - sv_EI_eph.GMT;
	pad.RETBBO[0] = entout.t_BBO - sv_EI_eph.GMT;
	pad.RETDRO[0] = entout.t_drogue - sv_EI_eph.GMT;
	pad.RETEBO[0] = entout.t_EBO - sv_EI_eph.GMT;
	pad.RETVCirc[0] = entout.t_V_Circ - sv_EI_eph.GMT;
	pad.RRT[0] = EIGET;
	pad.RTGO[0] = entout.R_EMS / 1852.0;
	pad.SXTS[0] = Entrystaroct;
	pad.SFT[0] = Entryshaft*DEG;
	pad.TRN[0] = Entrytrunnion*DEG;
	pad.SPA[0] = EntryBSSpitch*DEG;
	pad.SXP[0] = EntryBSSXPos*DEG;
	pad.V400K[0] = EntryPADV400k / 0.3048;
	pad.VIO[0] = entout.V_EMS / 0.3048;
	pad.VLMax[0] = 0.0;
	pad.VLMin[0] = 0.0;
}

MATRIX3 RTCC::GetREFSMMATfromAGC(agc_t *agc, bool cmc)
{
	MATRIX3 REFSMMAT;
	char Buffer[100];
	int REFSMMAToct[20];
	int REFSaddr;

	if (cmc)
	{
		REFSaddr = SystemParameters.MCCCRF_DL;
	}
	else
	{
		REFSaddr = SystemParameters.MCCLRF_DL;
	}

	unsigned short REFSoct[20];
	REFSoct[2] = agc->Erasable[0][REFSaddr];
	REFSoct[3] = agc->Erasable[0][REFSaddr + 1];
	REFSoct[4] = agc->Erasable[0][REFSaddr + 2];
	REFSoct[5] = agc->Erasable[0][REFSaddr + 3];
	REFSoct[6] = agc->Erasable[0][REFSaddr + 4];
	REFSoct[7] = agc->Erasable[0][REFSaddr + 5];
	REFSoct[8] = agc->Erasable[0][REFSaddr + 6];
	REFSoct[9] = agc->Erasable[0][REFSaddr + 7];
	REFSoct[10] = agc->Erasable[0][REFSaddr + 8];
	REFSoct[11] = agc->Erasable[0][REFSaddr + 9];
	REFSoct[12] = agc->Erasable[0][REFSaddr + 10];
	REFSoct[13] = agc->Erasable[0][REFSaddr + 11];
	REFSoct[14] = agc->Erasable[0][REFSaddr + 12];
	REFSoct[15] = agc->Erasable[0][REFSaddr + 13];
	REFSoct[16] = agc->Erasable[0][REFSaddr + 14];
	REFSoct[17] = agc->Erasable[0][REFSaddr + 15];
	REFSoct[18] = agc->Erasable[0][REFSaddr + 16];
	REFSoct[19] = agc->Erasable[0][REFSaddr + 17];
	for (int i = 2; i < 20; i++)
	{
		sprintf(Buffer, "%05o", REFSoct[i]);
		REFSMMAToct[i] = atoi(Buffer);
	}
	
	REFSMMAT.m11 = OrbMech::DecToDouble(REFSoct[2], REFSoct[3])*2.0;
	REFSMMAT.m12 = OrbMech::DecToDouble(REFSoct[4], REFSoct[5])*2.0;
	REFSMMAT.m13 = OrbMech::DecToDouble(REFSoct[6], REFSoct[7])*2.0;
	REFSMMAT.m21 = OrbMech::DecToDouble(REFSoct[8], REFSoct[9])*2.0;
	REFSMMAT.m22 = OrbMech::DecToDouble(REFSoct[10], REFSoct[11])*2.0;
	REFSMMAT.m23 = OrbMech::DecToDouble(REFSoct[12], REFSoct[13])*2.0;
	REFSMMAT.m31 = OrbMech::DecToDouble(REFSoct[14], REFSoct[15])*2.0;
	REFSMMAT.m32 = OrbMech::DecToDouble(REFSoct[16], REFSoct[17])*2.0;
	REFSMMAT.m33 = OrbMech::DecToDouble(REFSoct[18], REFSoct[19])*2.0;
	
	return REFSMMAT;
}

double RTCC::GetClockTimeFromAGC(agc_t *agc)
{
	return agc->Erasable[AGC_BANK(025)][AGC_ADDR(025)] + agc->Erasable[AGC_BANK(024)][AGC_ADDR(024)] * pow((double) 2., (double) 14.);
}

double RTCC::GetTEPHEMFromAGC(agc_t *agc, bool IsCMC)
{
	int tephem_int[3], address;

	//Either CMC or LGC TEPHEM address
	if (IsCMC)
	{
		address = SystemParameters.MCCTEP;
	}
	else
	{
		address = SystemParameters.MCLTEP;
	}

	tephem_int[0] = agc->Erasable[AGC_BANK(address)][AGC_ADDR(address)];
	tephem_int[1] = agc->Erasable[AGC_BANK(address + 1)][AGC_ADDR(address + 1)];
	tephem_int[2] = agc->Erasable[AGC_BANK(address + 2)][AGC_ADDR(address + 2)];

	//Make negative numbers actually negative
	for (int i = 0; i < 3; i++)
	{
		OrbMech::AGCSignedValue(tephem_int[i]);
	}

	//Calculate TEPHEM in centiseconds
	double tephem = tephem_int[2] + tephem_int[1] * pow((double) 2., (double) 14.) + tephem_int[0] * pow((double) 2., (double) 28.);

	return tephem;
}

void RTCC::navcheck(VECTOR3 R, double GMT, int RBI, double &lat, double &lng, double &alt)
{
	MATRIX3 Rot2;
	VECTOR3 Requ, u;
	double sinl, a, b, gamma,r_0, K;

	a = 6378166;
	b = 6356784;

	if (RBI == BODY_EARTH)
	{
		gamma = b * b / a / a;
		r_0 = OrbMech::R_Earth;
		K = 1.0;
		ELVCNV(GMT, 0, 1, Rot2);
	}
	else
	{
		gamma = 1;
		r_0 = BZLAND.rad[RTCC_LMPOS_BEST];
		K = 0.0;
		ELVCNV(GMT, 2, 3, Rot2);
	}

	Requ = mul(Rot2, R);

	u = unit(Requ);
	sinl = u.z;

	lat = atan(u.z/(gamma*sqrt(u.x*u.x + u.y*u.y)));
	lng = atan2(u.y, u.x) - OrbMech::w_Earth*K*GMT;
	OrbMech::normalizeAngle(lng, false);
	alt = length(Requ) - r_0;
}

SV RTCC::StateVectorCalc(VESSEL *vessel, double SVMJD)
{
	SV sv, sv1;
	EphemerisData sv0;
	double dt;
	
	sv0 = StateVectorCalcEphem(vessel);

	sv = ConvertEphemDatatoSV(sv0);
	sv.mass = vessel->GetMass();

	if (SVMJD != 0.0)
	{
		dt = (SVMJD - sv.MJD)*24.0*3600.0;
		sv1 = coast(sv, dt);
	}
	else
	{
		sv1 = sv;
	}

	return sv1;
}

EphemerisData RTCC::StateVectorCalcEphem(VESSEL *vessel)
{
	VECTOR3 R, V;
	double MJD;
	OBJHANDLE gravref;
	EphemerisData sv;

	gravref = AGCGravityRef(vessel);

	vessel->GetRelativePos(gravref, R);
	vessel->GetRelativeVel(gravref, V);
	MJD = oapiGetSimMJD();

	sv.R = _V(R.x, R.z, R.y);
	sv.V = _V(V.x, V.z, V.y);

	//Convert to BRCS
	sv.R = mul(SystemParameters.MAT_J2000_BRCS, sv.R);
	sv.V = mul(SystemParameters.MAT_J2000_BRCS, sv.V);

	sv.GMT = OrbMech::GETfromMJD(MJD, SystemParameters.GMTBASE);
	if (gravref == hEarth)
	{
		sv.RBI = BODY_EARTH;
	}
	else
	{
		sv.RBI = BODY_MOON;
	}
	return sv;
}

SV2 RTCC::StateVectorCalc2(VESSEL *vessel)
{
	SV2 sv;

	sv.sv = StateVectorCalcEphem(vessel);
	sv.Mass = vessel->GetMass();
	sv.AttachedMass = GetDockedVesselMass(vessel);

	return sv;
}

OBJHANDLE RTCC::AGCGravityRef(VESSEL *vessel)
{
	OBJHANDLE gravref;
	VECTOR3 rsph;

	gravref = hMoon;
	vessel->GetRelativePos(gravref, rsph);
	if (length(rsph) > 64373760.0)
	{
		gravref = hEarth;
	}
	return gravref;
}

int RTCC::DetermineSVBody(EphemerisData2 sv)
{
	//Input is ECI from ephemeris

	double r_m; //Radius relative to the Moon

	EphemerisData2 sv_M;
	ELVCNV(sv, 0, 2, sv_M);
	r_m = length(sv_M.R);
	if (r_m > 9.0*OrbMech::R_Earth)
	{
		return BODY_EARTH;
	}
	return BODY_MOON;
}

void RTCC::RotateSVToSOI(EphemerisData &sv)
{
	EphemerisData sv_E, sv_M;

	//Earth or Moon SOI?
	if (sv.RBI == BODY_EARTH)
	{
		//Earth, convert to MCI
		sv_E = sv;
		ELVCNV(sv, 2, sv_M);
	}
	else
	{
		//Already have a MCI vector
		sv_M = sv;
	}
	//SOI criterium: 9 Earth radii
	if (length(sv_M.R) > 9.0*OrbMech::R_Earth)
	{
		//More than 9 Er, state vector should be ECI
		if (sv.RBI == BODY_MOON)
		{
			//If input was MCI, convert to ECI
			ELVCNV(sv, 0, sv_E);
			sv = sv_E;
		}
		sv.RBI = BODY_EARTH;
	}
	else
	{
		//Output MCI vector
		sv = sv_M;
		sv.RBI = BODY_MOON;
	}
}

EphemerisData RTCC::RotateSVToSOI(EphemerisData2 sv)
{
	//Input is ECI
	EphemerisData2 sv_M;
	EphemerisData sv_out;

	//Convert to MCI
	ELVCNV(sv, 0, 2, sv_M);

	//SOI criterium: 9 Earth radii
	if (length(sv_M.R) > 9.0*OrbMech::R_Earth)
	{
		sv_out = Eph2ToEph1(sv, BODY_EARTH);
	}
	else
	{
		sv_out = Eph2ToEph1(sv_M, BODY_MOON);
	}
	return sv_out;
}

double RTCC::CalcGETBase()
{
	return SystemParameters.GMTBASE + SystemParameters.MCGMTL / 24.0;
}

double RTCC::GETfromGMT(double GMT)
{
	return GMT - SystemParameters.MCGMTL * 3600.0;
}

double RTCC::GMTfromGET(double GET)
{
	return GET + SystemParameters.MCGMTL * 3600.0;
}

MATRIX3 RTCC::REFSMMATCalc(REFSMMATOpt *opt)
{
	VECTOR3 UX, UY, UZ;

	//Here the options that don't require a state vector or thrust parameters
	if (opt->REFSMMATopt == 7)
	{
		MATRIX3 M;
		VECTOR3 X_NB, Y_NB, Z_NB, X_NB_apo, Y_NB_apo, Z_NB_apo;

		M = OrbMech::CALCSMSC(opt->IMUAngles);
		X_NB_apo = _V(M.m11, M.m12, M.m13);
		Y_NB_apo = _V(M.m21, M.m22, M.m23);
		Z_NB_apo = _V(M.m31, M.m32, M.m33);
		X_NB = tmul(opt->PresentREFSMMAT, X_NB_apo);
		Y_NB = tmul(opt->PresentREFSMMAT, Y_NB_apo);
		Z_NB = tmul(opt->PresentREFSMMAT, Z_NB_apo);

		return _M(X_NB.x, X_NB.y, X_NB.z, Y_NB.x, Y_NB.y, Y_NB.z, Z_NB.x, Z_NB.y, Z_NB.z);
	}
	else if (opt->REFSMMATopt == 4)
	{
		double phi, DLNG;

		phi = SystemParameters.MCLLTP[0];
		DLNG = SystemParameters.MCLGRA + SystemParameters.MCLAMD + SystemParameters.MCERTS * SystemParameters.MCGMTL;

		return GLMRTM(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), DLNG, 3, -PI05 - phi, 2, SystemParameters.MCLABN, 3);
	}
	else if (opt->REFSMMATopt == 6)
	{
		double MoonPos[12];
		VECTOR3 R_ME;

		CELBODY *cMoon = oapiGetCelbodyInterface(hMoon);

		cMoon->clbkEphemeris(opt->REFSMMATTime, EPHEM_TRUEPOS, MoonPos);

		R_ME = -_V(MoonPos[0], MoonPos[1], MoonPos[2]);

		UX = unit(crossp(_V(0.0, 1.0, 0.0), unit(R_ME)));
		//UX = mul(Rot, _V(UX.x, UX.z, UX.y));
		UX = _V(UX.x, UX.z, UX.y);

		UZ = _V(0.0, 0.0, -1.0);
		UY = crossp(UZ, UX);

		return mul(_M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z), OrbMech::tmat(SystemParameters.MAT_J2000_BRCS));
	}
	else if (opt->REFSMMATopt == 8)
	{
		MATRIX3 Rot2;
		VECTOR3 R_P, R_LS, R_LPO, V_LPO, H_LPO, axis, H_C;

		//Calculate selenographic landing site vector
		R_P = unit(_V(cos(opt->LSLng)*cos(opt->LSLat), sin(opt->LSLng)*cos(opt->LSLat), sin(opt->LSLat)));
		//Get rotation matrix from selenographc to selenocentric
		ELVCNV(GMTfromGET(opt->REFSMMATTime), 3, 2, Rot2);
		//Convert landing site vector to selenocentric
		R_LS = mul(Rot2, R_P);

		//Get orbital plane in selenocentric coordinatrs
		OrbMech::adbar_from_rv(1.0, 1.0, opt->LSLng, opt->LSLat, PI05, opt->LSAzi, R_LPO, V_LPO);
		H_LPO = crossp(R_LPO, V_LPO);
		axis = unit(H_LPO);
		H_C = unit(mul(Rot2, axis));

		//Calculate REFSMMAT
		UX = unit(R_LS);
		UZ = unit(crossp(H_C, R_LS));
		UY = crossp(UZ, UX);

		return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}

	double dt, LMmass;
	VECTOR3 DV_P, DV_C, V_G, X_SM, Y_SM, Z_SM;
	double theta_T;
	SV sv0, sv2;

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	sv2 = sv0;

	if (opt->REFSMMATopt == 5)
	{
		VECTOR3 R_P, R_LS, H_C;
		SV sv3;

		R_P = unit(_V(cos(opt->LSLng)*cos(opt->LSLat), sin(opt->LSLng)*cos(opt->LSLat), sin(opt->LSLat)));

		ELVCNV(R_P, GMTfromGET(opt->REFSMMATTime), 1, 3, 2, R_LS);

		sv3 = coast(sv2, opt->REFSMMATTime - OrbMech::GETfromMJD(sv2.MJD, CalcGETBase()));

		H_C = crossp(sv3.R, sv3.V);

		UX = unit(R_LS);
		UZ = unit(crossp(H_C, R_LS));
		UY = crossp(UZ, UX);

		return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}
	else
	{
		SV sv4;

		if (opt->REFSMMATopt == 0 || opt->REFSMMATopt == 1 || opt->REFSMMATopt == 2)
		{
			dt = opt->REFSMMATTime - (sv2.MJD - CalcGETBase()) * 24.0 * 60.0 * 60.0;
			sv4 = coast(sv2, dt);
		}
		else
		{
			dt = OrbMech::time_radius_integ(SystemParameters.AGCEpoch, sv2.R, sv2.V, sv2.MJD, OrbMech::R_Earth + 400000.0*0.3048, -1, sv2.gravref, hEarth, sv4.R, sv4.V);
		}

		if (opt->REFSMMATopt == 0 || opt->REFSMMATopt == 1)
		{
			UY = unit(crossp(sv4.V, sv4.R));
			UZ = unit(-sv4.R);
			UX = crossp(UY, UZ);

			double headsswitch, F;

			if (opt->HeadsUp)
			{
				headsswitch = 1.0;
			}
			else
			{
				headsswitch = -1.0;
			}
			if (opt->vesseltype < 2)
			{
				F = SystemParameters.MCTST1;
			}
			else
			{
				F = SystemParameters.MCTDT1;
			}

			DV_P = UX*opt->dV_LVLH.x + UZ*opt->dV_LVLH.z;
			if (length(DV_P) != 0.0)
			{
				theta_T = length(crossp(sv4.R, sv4.V))*length(opt->dV_LVLH)*(sv4.mass + LMmass) / OrbMech::power(length(sv4.R), 2.0) / F;
				DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
				V_G = DV_C + UY*opt->dV_LVLH.y;
			}
			else
			{
				V_G = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;
			}

			if (opt->vesseltype < 2)
			{
				VECTOR3 AT, YPH, ZPH;
				double p_T, y_T;

				double T, WDOT;
				unsigned IC;
				if (LMmass > 0)
				{
					IC = 13;
				}
				else
				{
					IC = 1;
				}
				GIMGBL(sv4.mass, LMmass, p_T, y_T, T, WDOT, RTCC_ENGINETYPE_CSMSPS, IC, 1, 0, 0.0);

				AT = unit(V_G);
				YPH = unit(crossp(AT, sv4.R));
				if (opt->REFSMMATopt == 0 && opt->HeadsUp == false)
				{
					YPH = -YPH;
				}
				ZPH = unit(crossp(AT, YPH));
				X_SM = AT * cos(y_T)*cos(p_T) - YPH * sin(y_T)*cos(p_T) + ZPH * sin(p_T);
				Y_SM = AT * sin(y_T) + YPH * cos(y_T);
				Z_SM = unit(crossp(X_SM, Y_SM));

				if (opt->REFSMMATopt != 0)
				{
					//Retro
					X_SM = -X_SM;
					Y_SM = -Y_SM;
				}
			}
			else
			{
				VECTOR3 U_FDI;

				U_FDI = unit(V_G);

				if (opt->REFSMMATopt == 0)
				{
					X_SM = U_FDI;
				}
				else
				{
					X_SM = -U_FDI;
				}
				if (abs(dotp(X_SM, unit(sv4.R))) < cos(0.01*RAD))
				{
					Y_SM = unit(crossp(X_SM, -sv4.R*headsswitch));
				}
				else
				{
					Y_SM = unit(crossp(X_SM, sv4.V));
				}
				Z_SM = unit(crossp(X_SM, Y_SM));
			}

			return _M(X_SM.x, X_SM.y, X_SM.z, Y_SM.x, Y_SM.y, Y_SM.z, Z_SM.x, Z_SM.y, Z_SM.z);

			//IMUangles = OrbMech::CALCGAR(REFSMMAT, mul(transpose_matrix(M), M_R));
			//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);
		}
		else
		{
			if (opt->vesseltype < 2)
			{
				UY = unit(crossp(sv4.V, sv4.R));
				UZ = unit(-sv4.R);
				UX = crossp(UY, UZ);
			}
			else
			{
				UX = unit(sv4.R);
				UY = unit(crossp(sv4.V, sv4.R));
				UZ = unit(crossp(UX, UY));
			}

			return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		}
		//
	}
}

double RTCC::FindDH(SV sv_A, SV sv_P, double TIGguess, double DH)
{
	SV sv_A1, sv_P1;
	double GETbase, dt, dt2, t_A, t_P, SVMJD, c1, c2, dt2_apo, CDHtime_cor;
	int n = 0;

	GETbase = CalcGETBase();
	t_A = OrbMech::GETfromMJD(sv_A.MJD, GETbase);
	t_P = OrbMech::GETfromMJD(sv_P.MJD, GETbase);
	sv_A1 = coast(sv_A, TIGguess - t_A);
	sv_P1 = coast(sv_P, TIGguess - t_P);
	SVMJD = GETbase + TIGguess / 24.0 / 3600.0;

	dt = 0.0;
	dt2 = dt + 10.0;
	
	//A secant search method is used to find the time, when the desired delta height is reached. Other values might work better.
	while (abs(dt2 - dt) > 0.1 && n <= 20)					//0.1 seconds accuracy should be enough
	{
		c1 = OrbMech::NSRsecant(SystemParameters.AGCEpoch, sv_A1.R, sv_A1.V, sv_P1.R, sv_P1.V, SVMJD, dt, DH, sv_A1.gravref);		//c is the difference between desired and actual DH
		c2 = OrbMech::NSRsecant(SystemParameters.AGCEpoch, sv_A1.R, sv_A1.V, sv_P1.R, sv_P1.V, SVMJD, dt2, DH, sv_A1.gravref);

		dt2_apo = dt2 - (dt2 - dt) / (c2 - c1)*c2;						//secant method
		dt = dt2;
		dt2 = dt2_apo;
		n++;
	}

	CDHtime_cor = dt2 + (SVMJD - GETbase) * 24 * 60 * 60;		//the new, calculated CDH time
	return CDHtime_cor;
}

double RTCC::TPISearch(SV sv_A, SV sv_P, double elev)
{
	SV sv_P1;
	double dt;

	sv_P1 = coast(sv_P, (sv_A.MJD - sv_P.MJD)*24.0*3600.0);
	dt = OrbMech::findelev(SystemParameters.AGCEpoch, sv_A.R, sv_A.V, sv_P1.R, sv_P1.V, sv_A.MJD, elev, sv_A.gravref);

	return OrbMech::GETfromMJD(sv_A.MJD + dt / 24.0 / 3600.0, CalcGETBase());
}

int RTCC::LunarDescentPlanningProcessor(SV sv)
{
	LDPPOptions opt;

	opt.azi_nom = GZGENCSN.LDPPAzimuth;
	opt.GETbase = CalcGETBase();
	opt.H_DP = GZGENCSN.LDPPHeightofPDI;
	opt.H_W = med_k16.DesiredHeight;
	opt.IDO = med_k16.Sequence - 2;
	if (GZGENCSN.LDPPAzimuth != 0.0)
	{
		opt.I_AZ = 1;
	}
	else
	{
		opt.I_AZ = 0;
	}
	if (GZGENCSN.LDPPPoweredDescentSimFlag)
	{
		opt.I_PD = 1;
	}
	else
	{
		opt.I_PD = 0;
	}
	opt.I_SP = 0.0;
	opt.I_TPD = 0;
	opt.Lat_LS = BZLAND.lat[RTCC_LMPOS_BEST];
	opt.Lng_LS = BZLAND.lng[RTCC_LMPOS_BEST];
	opt.M = GZGENCSN.LDPPDwellOrbits;
	opt.MODE = med_k16.Mode;
	opt.R_LS = BZLAND.rad[RTCC_LMPOS_BEST];
	opt.sv0 = sv;
	opt.TH[0] = med_k16.GETTH1;
	opt.TH[1] = med_k16.GETTH2;
	opt.TH[2] = med_k16.GETTH3;
	opt.TH[3] = med_k16.GETTH4;
	opt.theta_D = GZGENCSN.LDPPDescentFlightArc;
	opt.t_D = GZGENCSN.LDPPDescentFlightTime;
	opt.T_PD = GZGENCSN.LDPPTimeofPDI;
	opt.W_LM = 0.0;

	//Mode 6 doesn't work yet
	if (opt.MODE == 6) return 1;

	LDPP ldpp(this);
	LDPPResults res;
	ldpp.Init(opt);
	int error = ldpp.LDPPMain(res);

	if (error) return error;

	//Store in PZLDPELM
	for (int i = 0;i < 4;i++)
	{
		PZLDPELM.sv_man_bef[i].R = res.sv_before[i].R;
		PZLDPELM.sv_man_bef[i].V = res.sv_before[i].V;
		PZLDPELM.sv_man_bef[i].GMT = OrbMech::GETfromMJD(res.sv_before[i].MJD, SystemParameters.GMTBASE);
		PZLDPELM.sv_man_bef[i].RBI = BODY_MOON;	
		PZLDPELM.V_man_after[i] = res.V_after[i];
		PZLDPELM.plan[i] = med_k16.Vehicle;
	}
	PZLDPELM.num_man = res.i;

	PMDLDPP(opt, res, PZLDPDIS);

	PZLDPELM.code[0] = PZLDPDIS.MVR[0];
	PZLDPELM.code[1] = PZLDPDIS.MVR[1];
	PZLDPELM.code[2] = PZLDPDIS.MVR[2];
	PZLDPELM.code[3] = PZLDPDIS.MVR[3];

	return 0;
}

void RTCC::TranslunarInjectionProcessor(SV2 state)
{
	TLIMEDQuantities medquant;
	TLMCCMissionConstants mccconst;
	TLIOutputData out;

	medquant.Mode = PZTLIPLN.Mode;
	medquant.state = state;
	medquant.h_ap = PZTLIPLN.h_ap*1852.0;
	medquant.GMT_TIG = GMTfromGET(PZTLIPLN.GET_TLI);

	mccconst.delta = PZTLIPLN.DELTA;
	mccconst.sigma = PZTLIPLN.SIGMA;
	mccconst.Reentry_range = PZMCCPLN.Reentry_range;

	TLIProcessor tli(this);
	tli.Init(medquant, mccconst, GetGMTBase());
	tli.Main(out);

	if (out.ErrorIndicator) return;

	PZTTLIPL.DataIndicator = 1;
	PZTTLIPL.elem = out.uplink_data;

	//Display data
	PZTPDDIS.GET_TIG = GETfromGMT(out.uplink_data.GMT_TIG);
	PZTPDDIS.GET_TB6 = PZTPDDIS.GET_TIG - SystemParameters.MDVSTP.DTIG;
	PZTPDDIS.dv_TLI = out.dv_TLI / 0.3048;
}

void RTCC::TranslunarMidcourseCorrectionProcessor(EphemerisData sv0, double CSMmass, double LMmass)
{
	TLMCCDataTable datatab;
	TLMCCMEDQuantities medquant;
	TLMCCMissionConstants mccconst;
	TLMCCOutputData out;

	datatab = PZSFPTAB.blocks[PZMCCPLN.SFPBlockNum - 1];

	medquant.Mode = PZMCCPLN.Mode;
	medquant.Config = PZMCCPLN.Config;
	medquant.T_MCC = GMTfromGET(PZMCCPLN.MidcourseGET);
	medquant.sv0 = sv0;
	medquant.CSMMass = CSMmass;
	medquant.LMMass = LMmass;
	medquant.H_pl = PZMCCPLN.h_PC;
	medquant.H_pl_mode5 = PZMCCPLN.h_PC_mode5;
	medquant.INCL_fr = PZMCCPLN.incl_fr;
	medquant.H_pl_min = PZMCCPLN.H_PCYN_MIN;
	medquant.H_pl_max = PZMCCPLN.H_PCYN_MAX;
	medquant.AZ_min = PZMCCPLN.AZ_min;
	medquant.AZ_max = PZMCCPLN.AZ_max;
	medquant.H_A_LPO1 = PZMCCPLN.H_A_LPO1;
	medquant.H_P_LPO1 = PZMCCPLN.H_P_LPO1;
	medquant.H_A_LPO2 = PZMCCPLN.H_A_LPO2;
	medquant.H_P_LPO2 = PZMCCPLN.H_P_LPO2;
	medquant.Revs_LPO1 = PZMCCPLN.REVS1;
	medquant.Revs_LPO2 = PZMCCPLN.REVS2;
	medquant.TA_LOI = PZMCCPLN.ETA1;
	medquant.site_rotation_LPO2 = PZMCCPLN.SITEROT;
	medquant.useSPS = true;
	medquant.T_min_sea = PZMCCPLN.TLMIN + SystemParameters.MCGMTL;
	if (PZMCCPLN.TLMAX <= 0)
	{
		medquant.T_max_sea = 1000.0;
	}
	else
	{
		medquant.T_max_sea = PZMCCPLN.TLMAX + SystemParameters.MCGMTL;
	}
	medquant.Revs_circ = 1;
	medquant.H_T_circ = 60.0*1852.0;
	medquant.lat_bias = PZMCCPLN.LATBIAS;

	mccconst.V_pcynlo = 5480.0*0.3048;
	mccconst.H_LPO = 60.0*1852.0;
	mccconst.lambda_IP = 190.0*RAD;
	mccconst.dt_bias = 0.332;
	mccconst.m = PZMCCPLN.LOPC_M;
	mccconst.n = PZMCCPLN.LOPC_N;
	mccconst.T_t1_min_dps = PZMCCPLN.TT1_DPS_MIN;
	mccconst.T_t1_max_dps = PZMCCPLN.TT1_DPS_MAX;
	mccconst.INCL_PR_MAX = PZMCCPLN.INCL_PR_MAX;
	mccconst.Reentry_range = PZMCCPLN.Reentry_range;

	TLMCCProcessor tlmcc(this);
	tlmcc.Init(datatab, medquant, mccconst, GetGMTBase());
	tlmcc.Main(out);

	//Update display data
	PZMCCDIS.data[PZMCCPLN.Column - 1] = out.display;

	//Update MPT transfer table
	PZMCCXFR.sv_man_bef[PZMCCPLN.Column - 1].R = out.R_MCC;
	PZMCCXFR.sv_man_bef[PZMCCPLN.Column - 1].V = out.V_MCC;
	PZMCCXFR.sv_man_bef[PZMCCPLN.Column - 1].GMT = out.GMT_MCC;
	PZMCCXFR.sv_man_bef[PZMCCPLN.Column - 1].RBI = out.RBI;
	PZMCCXFR.V_man_after[PZMCCPLN.Column - 1] = out.V_MCC_apo;

	//Update skeleton flight plan table
	PZMCCSFP.blocks[PZMCCPLN.Column - 1] = out.outtab;
	PZMCCSFP.blocks[PZMCCPLN.Column - 1].GMTTimeFlag = RTCCPresentTimeGMT();
}

bool RTCC::GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_i, double &P30TIG)
{
	RTCCGeneralPurposeManeuverProcessor gpm(this);

	PZGPMDIS.Err = gpm.PCMGPM(*opt);

	switch (PZGPMDIS.Err)
	{
	case 0: //All good
		PMXSPT("PMMGPM", 11);
		break;
	case 1: //Maneuver cannot be performed in this orbit
		PMXSPT("PMMGPM", 12);
		break;
	case 2: //Maneuver cannot be performed at this point in the orbit
		PMXSPT("PMMGPM", 13);
		break;
	case 3: //AEG failed to converge
		PMXSPT("PMMGPM", 14);
		break;
	case 4: //Unrecoverable AEG error
		PMXSPT("PMMGPM", 15);
		break;
	case 5: //Unable to obtain libration matrix
		PMXSPT("PMMGPM", 57);
		break;
	case 6: //Unable to advance to selenographic argument of latitude
		PMXSPT("PMMGPM", 68);
	case 7: //PMMAPD error for current apo/peri
		PMXSPT("PMMGPM", 135);
	case 8: //PMMAPD error for resultant apo/peri
		PMXSPT("PMMGPM", 136);
		break;
	}

	P30TIG = PZGPMDIS.GET_TIG;
	dV_i = PZGPMELM.V_after - PZGPMELM.SV_before.V;

	return true;
}

void RTCC::TLI_PAD(const TLIPADOpt &opt, TLIPAD &pad)
{
	EMMENIInputTable emmeniin;
	PMMSPTInput in;
	MPTManeuver man;
	MissionPlanTable mpt;
	PLAWDTInput plain;
	PLAWDTOutput plaout;
	EphemerisData sv_TH;
	double dt;

	//Take backup of S-IVB matrix table, as it might interfere with MPT
	SIVBTLIMatrixTable ADRMAT = PZMATCSM;

	in.AttitudeMode = RTCC_ATTITUDE_SIVB_IGM;
	in.CC = in.CCMI = 2; //Configuration code doesn't really matter, just has to include S-IVB. 2 = CSM+S-IVB
	in.CCI = RTCC_CONFIGCHANGE_NONE;
	in.CurMan = &man;
	in.dt = 0.0;
	in.EndTimeLimit = 1.e10; //Just needs to be high to not generate failure
	in.GMT = opt.sv0.GMT;
	in.InjOpp = opt.InjOpp;
	in.mpt = &mpt;
	in.PresentGMT = 0.0; //Is this ok?
	in.PrevMan = NULL;
	in.QUEID = 37; //Direct input S-IVB TLI	maneuver
	in.R = opt.sv0.R;
	in.ReplaceCode = 0;
	in.StartTimeLimit = 0.0;
	in.Table = RTCC_MPT_CSM;
	in.ThrusterCode = RTCC_ENGINETYPE_SIVB_MAIN;
	in.TVC = RTCC_MPT_CSM;
	in.T_RP = -1.0;
	in.V = opt.sv0.V;

	int err = PMMSPT(in);

	//Error checking?
	if (err) return;

	//Take SV to threshold
	dt = in.T_RP - opt.sv0.GMT;
	emmeniin.AnchorVector = opt.sv0;
	emmeniin.Area = PZMPTCSM.ConfigurationArea;
	emmeniin.CutoffIndicator = 1;
	emmeniin.DensityMultiplier = 1.0;
	if (dt >= 0)
	{
		emmeniin.IsForwardIntegration = 1.0;
	}
	else
	{
		emmeniin.IsForwardIntegration = -1.0;
	}
	emmeniin.MaxIntegTime = abs(dt);
	emmeniin.Weight = opt.ConfigMass;
	emmeniin.VentPerturbationFactor = 1.0; //Turn venting on
	EMMENI(emmeniin);
	sv_TH = emmeniin.sv_cutoff;

	//Continue processing, when the MPT uses PMMSPT with QUEID = 37 then it returns the threshold time first, before it continues processing with QUEID = 39
	in.QUEID = 39;
	//Input state vector at threshold, MPT would input a state vector from the ephemeris at the threshold time
	in.R = sv_TH.R;
	in.V = sv_TH.V;
	in.GMT = sv_TH.GMT;
	//T_RP reset to zero so that the actual time of restart preparations is calculated
	in.T_RP = 0.0;
	err = PMMSPT(in);

	if (err) return;

	//Take SV to TB6
	dt = man.GMTMAN - opt.sv0.GMT;
	if (dt >= 0)
	{
		emmeniin.IsForwardIntegration = 1.0;
	}
	else
	{
		emmeniin.IsForwardIntegration = -1.0;
	}
	emmeniin.MaxIntegTime = abs(dt);
	emmeniin.AnchorVector = opt.sv0;
	EMMENI(emmeniin);
	EphemerisData sv_TB6 = emmeniin.sv_cutoff;

	//Update mass
	plain.CSMArea = 0.0;
	plain.CSMWeight = 0.0;
	plain.KFactorOpt = false;
	plain.LMAscArea = 0.0;
	plain.LMAscWeight = 0.0;
	plain.LMDscArea = 0.0;
	plain.LMDscWeight = 0.0;
	plain.Num = 2;
	plain.SIVBArea = PZMPTCSM.ConfigurationArea;
	plain.SIVBWeight = opt.ConfigMass;
	plain.TableCode = -1;
	plain.T_IN = opt.sv0.GMT;
	plain.T_UP = sv_TB6.GMT;
	plain.VentingOpt = true; //Take venting mass loss into account

	PLAWDT(plain, plaout);

	//Simulate TLI maneuver
	RTCCNIInputTable integin;

	integin.R = sv_TB6.R;
	integin.V = sv_TB6.V;
	integin.WDMULT = 1.0;
	integin.DENSMULT = 1.0;
	integin.IEPHOP = 0; //No ephemeris storage
	integin.KAUXOP = true;

	integin.CAPWT = plaout.ConfigWeight;
	integin.Area = PZMPTCSM.ConfigurationArea;
	integin.CSMWT = 0.0;
	integin.SIVBWT = plaout.SIVBWeight;
	integin.LMAWT = 0.0;
	integin.LMDWT = 0.0;

	integin.MANOP = in.AttitudeMode;
	integin.ThrusterCode = in.ThrusterCode;
	integin.IC = in.CC;
	integin.MVC = in.TVC;
	integin.IFROZN = false;
	integin.IREF = BODY_EARTH;
	integin.ICOORD = 0;
	integin.GMTBASE = SystemParameters.GMTBASE;
	integin.GMTI = man.GMTMAN;
	integin.DTINP = in.dt;
	//e_N
	integin.Params[0] = man.dV_inertial.z;
	//C3
	integin.Params[1] = man.dV_LVLH.x;
	//alpha_D
	integin.Params[2] = man.dV_LVLH.y;
	//f
	integin.Params[3] = man.dV_LVLH.z;
	//P
	integin.Params[4] = man.Word67d;
	//K5
	integin.Params[5] = man.Word68;
	//R_T or T_M
	integin.Params[6] = man.Word69;
	//V_T
	integin.Params[7] = man.Word70;
	//gamma_T
	integin.Params[8] = man.Word71;
	//G_T
	integin.Params[9] = man.Word72;
	integin.Params[10] = man.Word73;
	integin.Params[11] = man.Word74;
	integin.Params[13] = man.Word76;
	integin.Params[14] = man.Word77;
	integin.Word68i[0] = man.Word78i[0];
	integin.Word68i[1] = man.Word78i[1];
	integin.Params2[0] = man.Word79;
	integin.Params2[1] = man.Word80;
	integin.Params2[2] = man.Word81;
	integin.Params2[3] = man.Word82;
	integin.Params2[4] = man.Word83;
	integin.Params2[5] = man.Word84;

	//Link to TLI matrix table
	MATRIX3 ADRM[3];
	ADRM[0] = PZMATCSM.EPH;
	ADRM[1] = PZMATCSM.GG;
	ADRM[2] = PZMATCSM.G;

	int nierror;
	RTCCNIAuxOutputTable AuxTableIndicator;

	TLIGuidanceSim numin(this, integin, nierror, NULL, &AuxTableIndicator, ADRM);
	numin.PCMTRL();

	//Restore matrix table
	PZMATCSM = ADRMAT;

	//Now all the calculations for the PAD itself
	EphemerisData sv_TLI, sv_TDE;
	MATRIX3 M_R, M, M_RTM;
	VECTOR3 UX, UY, UZ, IgnAtt, SepATT, ExtATT;

	UY = unit(crossp(AuxTableIndicator.V_BI, -AuxTableIndicator.R_BI));
	UZ = unit(AuxTableIndicator.R_BI);
	UX = crossp(UY, UZ);

	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	IgnAtt = OrbMech::CALCGAR(opt.REFSMMAT, M_R);

	sv_TLI.R = AuxTableIndicator.R_BO;
	sv_TLI.V = AuxTableIndicator.V_BO;
	sv_TLI.GMT = AuxTableIndicator.GMT_BO;
	sv_TLI.RBI = BODY_EARTH;

	//Take state vector to time of CSM separation
	sv_TDE = coast(sv_TLI, 900.0);

	UY = unit(crossp(sv_TDE.V, sv_TDE.R));
	UZ = unit(-sv_TDE.R);
	UX = crossp(UY, UZ);
	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = OrbMech::CALCSMSC(_V(PI - opt.SeparationAttitude.x, opt.SeparationAttitude.y, opt.SeparationAttitude.z));
	M_RTM = mul(M, M_R);

	SepATT = OrbMech::CALCGAR(opt.REFSMMAT, M_RTM);
	ExtATT = _V(300.0*RAD - SepATT.x, SepATT.y + PI, PI2 - SepATT.z);

	pad.BurnTime = AuxTableIndicator.DT_B;
	pad.dVC = AuxTableIndicator.DV_C / 0.3048;
	pad.IgnATT = _V(OrbMech::imulimit(IgnAtt.x*DEG), OrbMech::imulimit(IgnAtt.y*DEG), OrbMech::imulimit(IgnAtt.z*DEG));
	pad.SepATT = _V(OrbMech::imulimit(SepATT.x*DEG), OrbMech::imulimit(SepATT.y*DEG), OrbMech::imulimit(SepATT.z*DEG));
	pad.ExtATT = _V(OrbMech::imulimit(ExtATT.x*DEG), OrbMech::imulimit(ExtATT.y*DEG), OrbMech::imulimit(ExtATT.z*DEG));
	pad.TB6P = GETfromGMT(sv_TB6.GMT);
	pad.VI = length(sv_TLI.V) / 0.3048;
}

bool RTCC::PDI_PAD(PDIPADOpt* opt, AP11PDIPAD &pad)
{
	SV sv1, sv2, sv_I;
	MATRIX3 REFSMMAT;
	VECTOR3 U_FDP;
	double GETbase, C_R, TTT, t_IG;

	GETbase = CalcGETBase();

	if (opt->direct)
	{
		sv2 = opt->sv0;
	}
	else
	{
		sv2 = ExecuteManeuver(opt->sv0, opt->P30TIG, opt->dV_LVLH, 0.0, RTCC_ENGINETYPE_LMDPS);
	}

	if (!PDIIgnitionAlgorithm(sv2, opt->R_LS, opt->t_land, sv_I, TTT, C_R, U_FDP, REFSMMAT))
	{
		return false;
	}

	t_IG = OrbMech::GETfromMJD(sv_I.MJD, GETbase);

	VECTOR3 X_B, UX, UY, UZ, IMUangles, FDAIangles;
	MATRIX3 M, M_R;
	double headsswitch;

	if (opt->HeadsUp)
	{
		headsswitch = -1.0;
	}
	else
	{
		headsswitch = 1.0;
	}

	X_B = tmul(opt->REFSMMAT, unit(U_FDP));
	UX = X_B;
	if (abs(dotp(UX, unit(sv_I.R))) < cos(0.01*RAD))
	{
		UY = unit(crossp(UX, sv_I.R*headsswitch));
	}
	else
	{
		UY = unit(crossp(UX, sv_I.V));
	}
	UZ = unit(crossp(UX, UY));

	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), M_R));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.Att = _V(OrbMech::imulimit(FDAIangles.x*DEG), OrbMech::imulimit(FDAIangles.y*DEG), OrbMech::imulimit(FDAIangles.z*DEG));
	pad.CR = C_R / 1852.0;
	pad.DEDA231 = length(opt->R_LS) / 0.3048 / 100.0;
	pad.GETI = t_IG;
	pad.t_go = -TTT;

	return true;
}

void RTCC::LunarAscentPAD(ASCPADOpt opt, AP11LMASCPAD &pad)
{
	EphemerisData sv_Ins, sv_CSM_TIG;
	MATRIX3 Rot_MCT_MCI;
	VECTOR3 R_LSP, Q, U_R, Z_B, X_AGS, Y_AGS, Z_AGS;
	double TIG, CR, SMa, R_D, delta_L, sin_DL, cos_DL;

	TIG = GMTfromGET(opt.TIG);

	sv_CSM_TIG = coast(opt.sv_CSM, TIG - opt.sv_CSM.GMT);

	ELVCNV(TIG, 3, 2, Rot_MCT_MCI);

	R_LSP = mul(Rot_MCT_MCI, opt.R_LS);

	Q = unit(crossp(sv_CSM_TIG.V, sv_CSM_TIG.R));
	U_R = unit(R_LSP);
	R_D = length(R_LSP) + 60000.0*0.3048;
	CR = -R_D * asin(dotp(U_R, Q));

	sv_Ins.R = _V(R_D, 0, 0);
	sv_Ins.V = _V(opt.v_LV, opt.v_LH, 0);
	SMa = OrbMech::GetSemiMajorAxis(sv_Ins.R, sv_Ins.V, OrbMech::mu_Moon);

	//Get Z-axis body vector in inertial coordinates. Use Y unit vector because of LM coordinate system in Orbiter
	Z_B = mul(Rot_MCT_MCI, mul(opt.Rot_VL, _V(0, 1, 0)));
	X_AGS = U_R;
	Z_AGS = unit(crossp(-Q, X_AGS));
	Y_AGS = unit(crossp(Z_AGS, X_AGS));

	delta_L = atan2(dotp(Z_B, Z_AGS), dotp(Z_B, Y_AGS));
	sin_DL = sin(delta_L);
	cos_DL = cos(delta_L);

	pad.CR = CR / 1852.0;
	pad.TIG = opt.TIG;
	pad.V_hor = opt.v_LH / 0.3048;
	pad.V_vert = opt.v_LV / 0.3048;
	pad.DEDA225_226 = SMa / 0.3048 / 100.0;
	pad.DEDA231 = length(opt.R_LS) / 0.3048 / 100.0;
	sprintf(pad.remarks, "");
	pad.DEDA047 = OrbMech::DoubleToDEDA(sin_DL, 14);
	pad.DEDA053 = OrbMech::DoubleToDEDA(cos_DL, 14);
}

void RTCC::CMCExternalDeltaVUpdate(char *str, double P30TIG, VECTOR3 dV_LVLH)
{
	CMMAXTDV(P30TIG, dV_LVLH);
	V7XUpdate(71, str, CZAXTRDV.Octals, 10);
}

void RTCC::LGCExternalDeltaVUpdate(char *str, double P30TIG, VECTOR3 dV_LVLH)
{
	CMMLXTDV(P30TIG, dV_LVLH);
	V7XUpdate(71, str, CZLXTRDV.Octals, 10);
}

void RTCC::CMMCMNAV(int veh, int mpt, double GETSV, int ref)
{
	double GMTSV = GMTfromGET(GETSV);

	EphemerisData sv;
	if (ELFECH(GMTSV, mpt, sv))
	{
		return;
	}
	CMMCMNAV(veh, mpt, sv);
}

void RTCC::CMMCMNAV(int veh, int mpt, EphemerisData sv)
{
	//veh: 1 = CMC, 2 = LGC
	//mpt: 1 = CSM, 3 = LM

	MissionPlanTable *mptab;
	NavUpdateMakeupBuffer *buf;
	VECTOR3 pos, vel;
	double get, tclockzero;

	if (veh == 1)
	{
		if (mpt == RTCC_MPT_CSM)
		{
			buf = &CZNAVGEN.CMCCSMUpdate;
			buf->LoadType = "00";
		}
		else
		{
			buf = &CZNAVGEN.CMCLEMUpdate;
			buf->LoadType = "09";
		}
	}
	else
	{
		if (mpt == RTCC_MPT_CSM)
		{
			buf = &CZNAVGEN.LGCCSMUpdate;
			buf->LoadType = "21";
		}
		else
		{
			buf = &CZNAVGEN.LGCLEMUpdate;
			buf->LoadType = "20";
		}
	}

	if (mpt == RTCC_MPT_CSM)
	{
		mptab = &PZMPTCSM;
		buf->VehicleID = "CSM";
	}
	else
	{
		mptab = &PZMPTLEM;
		buf->VehicleID = "LEM";
	}
	
	if (veh == 1)
	{
		tclockzero = SystemParameters.MCGZSA*3600.0;
	}
	else
	{
		tclockzero = SystemParameters.MCGZSL*3600.0;
	}

	pos = buf->sv.R = sv.R;
	buf->sv.V = sv.V;
	vel = buf->sv.V*0.01;
	get = sv.GMT - tclockzero;

	buf->GETofGeneration = GETfromGMT(RTCCPresentTimeGMT());
	buf->sv.GMT = get;
	buf->sv.RBI = sv.RBI;
	buf->AnchorVectorTime = mptab->GMTAV;
	buf->DCCode = mptab->StationID;

	if (sv.RBI == BODY_MOON) {

		buf->Octals[0] = 21;
		buf->Octals[1] = 1501;

		if (mpt == RTCC_MPT_CSM)
		{
			buf->Octals[2] = 2;
		}
		else
		{
			buf->Octals[2] = 77775;
		}

		buf->Octals[3] = OrbMech::DoubleToBuffer(pos.x, 27, 1);
		buf->Octals[4] = OrbMech::DoubleToBuffer(pos.x, 27, 0);
		buf->Octals[5] = OrbMech::DoubleToBuffer(pos.y, 27, 1);
		buf->Octals[6] = OrbMech::DoubleToBuffer(pos.y, 27, 0);
		buf->Octals[7] = OrbMech::DoubleToBuffer(pos.z, 27, 1);
		buf->Octals[8] = OrbMech::DoubleToBuffer(pos.z, 27, 0);
		buf->Octals[9] = OrbMech::DoubleToBuffer(vel.x, 5, 1);
		buf->Octals[10] = OrbMech::DoubleToBuffer(vel.x, 5, 0);
		buf->Octals[11] = OrbMech::DoubleToBuffer(vel.y, 5, 1);
		buf->Octals[12] = OrbMech::DoubleToBuffer(vel.y, 5, 0);
		buf->Octals[13] = OrbMech::DoubleToBuffer(vel.z, 5, 1);
		buf->Octals[14] = OrbMech::DoubleToBuffer(vel.z, 5, 0);
		buf->Octals[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		buf->Octals[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}
	else
	{
		buf->Octals[0] = 21;
		buf->Octals[1] = 1501;

		if (mpt == RTCC_MPT_CSM)
		{
			buf->Octals[2] = 1;
		}
		else
		{
			buf->Octals[2] = 77776;
		}

		buf->Octals[3] = OrbMech::DoubleToBuffer(pos.x, 29, 1);
		buf->Octals[4] = OrbMech::DoubleToBuffer(pos.x, 29, 0);
		buf->Octals[5] = OrbMech::DoubleToBuffer(pos.y, 29, 1);
		buf->Octals[6] = OrbMech::DoubleToBuffer(pos.y, 29, 0);
		buf->Octals[7] = OrbMech::DoubleToBuffer(pos.z, 29, 1);
		buf->Octals[8] = OrbMech::DoubleToBuffer(pos.z, 29, 0);
		buf->Octals[9] = OrbMech::DoubleToBuffer(vel.x, 7, 1);
		buf->Octals[10] = OrbMech::DoubleToBuffer(vel.x, 7, 0);
		buf->Octals[11] = OrbMech::DoubleToBuffer(vel.y, 7, 1);
		buf->Octals[12] = OrbMech::DoubleToBuffer(vel.y, 7, 0);
		buf->Octals[13] = OrbMech::DoubleToBuffer(vel.z, 7, 1);
		buf->Octals[14] = OrbMech::DoubleToBuffer(vel.z, 7, 0);
		buf->Octals[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		buf->Octals[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}

	if (buf->SequenceNumber == 0)
	{
		if (veh == 1)
		{
			if (mpt == RTCC_MPT_CSM)
			{
				buf->SequenceNumber = 1;
			}
			else
			{
				buf->SequenceNumber = 901;
			}
		}
		else
		{
			if (mpt == RTCC_MPT_CSM)
			{
				buf->SequenceNumber = 2101;
			}
			else
			{
				buf->SequenceNumber = 2001;
			}
		}
	}
	else
	{
		buf->SequenceNumber++;
	}
}

void RTCC::AGCStateVectorUpdate(char *str, int comp, int ves, EphemerisData sv, bool v66)
{
	//comp: 1 = CMC, 2 = LGC
	//ves: 1 = CSM, 3 = LEM
	CMMCMNAV(comp, ves, sv);

	NavUpdateMakeupBuffer *buf;

	if (comp == 1)
	{
		if (ves == 1)
		{
			buf = &CZNAVGEN.CMCCSMUpdate;
		}
		else
		{
			buf = &CZNAVGEN.CMCLEMUpdate;
		}
	}
	else
	{
		if (ves == 1)
		{
			buf = &CZNAVGEN.LGCCSMUpdate;
		}
		else
		{
			buf = &CZNAVGEN.LGCLEMUpdate;
		}
	}
	V7XUpdate(71, str, buf->Octals, 17);
	if (v66)
	{
		sprintf(str, "%sV66ER", str);
	}
}

void RTCC::AGCStateVectorUpdate(char *str, SV sv, bool csm, bool v66)
{
	OBJHANDLE hMoon = oapiGetGbodyByName("Moon");
	OBJHANDLE hEarth = oapiGetGbodyByName("Earth");

	VECTOR3 vel, pos;
	double get;
	int emem[24];

	pos = sv.R;
	vel = sv.V*0.01;
	get = (sv.MJD - CalcGETBase())*24.0*3600.0;

	if (sv.gravref == hMoon) {

		emem[0] = 21;
		emem[1] = 1501;

		//if (g_Data.vessel->GetHandle()==oapiGetFocusObject()) 
		if (csm)
		{
			emem[2] = 2;
		}
		else
		{
			emem[2] = 77775;	// Octal coded decimal
		}

		emem[3] = OrbMech::DoubleToBuffer(pos.x, 27, 1);
		emem[4] = OrbMech::DoubleToBuffer(pos.x, 27, 0);
		emem[5] = OrbMech::DoubleToBuffer(pos.y, 27, 1);
		emem[6] = OrbMech::DoubleToBuffer(pos.y, 27, 0);
		emem[7] = OrbMech::DoubleToBuffer(pos.z, 27, 1);
		emem[8] = OrbMech::DoubleToBuffer(pos.z, 27, 0);
		emem[9] = OrbMech::DoubleToBuffer(vel.x, 5, 1);
		emem[10] = OrbMech::DoubleToBuffer(vel.x, 5, 0);
		emem[11] = OrbMech::DoubleToBuffer(vel.y, 5, 1);
		emem[12] = OrbMech::DoubleToBuffer(vel.y, 5, 0);
		emem[13] = OrbMech::DoubleToBuffer(vel.z, 5, 1);
		emem[14] = OrbMech::DoubleToBuffer(vel.z, 5, 0);
		emem[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		emem[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}
	else if (sv.gravref == hEarth) {

		emem[0] = 21;
		emem[1] = 1501;

		if (csm)
		{
			emem[2] = 1;
		}
		else
		{
			emem[2] = 77776;	// Octal coded decimal
		}

		emem[3] = OrbMech::DoubleToBuffer(pos.x, 29, 1);
		emem[4] = OrbMech::DoubleToBuffer(pos.x, 29, 0);
		emem[5] = OrbMech::DoubleToBuffer(pos.y, 29, 1);
		emem[6] = OrbMech::DoubleToBuffer(pos.y, 29, 0);
		emem[7] = OrbMech::DoubleToBuffer(pos.z, 29, 1);
		emem[8] = OrbMech::DoubleToBuffer(pos.z, 29, 0);
		emem[9] = OrbMech::DoubleToBuffer(vel.x, 7, 1);
		emem[10] = OrbMech::DoubleToBuffer(vel.x, 7, 0);
		emem[11] = OrbMech::DoubleToBuffer(vel.y, 7, 1);
		emem[12] = OrbMech::DoubleToBuffer(vel.y, 7, 0);
		emem[13] = OrbMech::DoubleToBuffer(vel.z, 7, 1);
		emem[14] = OrbMech::DoubleToBuffer(vel.z, 7, 0);
		emem[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		emem[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}
	V7XUpdate(71, str, emem, 17);
	if (v66)
	{
		sprintf(str, "%sV66ER", str);
	}
}

void RTCC::LandingSiteUplink(char *str, int veh)
{
	CMMCMCLS(veh);
	int *octals;
	if (veh == RTCC_MPT_CSM)
	{
		octals = CZLSVECT.CSMLSUpdate.Octals;
	}
	else
	{
		octals = CZLSVECT.LMLSUpdate.Octals;
	}
	V7XUpdate(71, str, octals, 8);
}

void RTCC::IncrementAGCTime(char *list, int veh, double dt)
{
	CMMTMEIN(veh, dt / 3600.0);
	int *octals;
	if (veh == RTCC_MPT_CSM)
	{
		octals = CZTMEINC.Blocks[0].Octals;
	}
	else
	{
		octals = CZTMEINC.Blocks[1].Octals;
	}

	V7XUpdate(73, list, octals, 3);
}

void RTCC::IncrementAGCLiftoffTime(char *list, int veh, double dt)
{
	CMMLIFTF(veh, dt / 3600.0);
	int *octals;
	if (veh == RTCC_MPT_CSM)
	{
		octals = CZLIFTFF.Blocks[0].Octals;
	}
	else
	{
		octals = CZLIFTFF.Blocks[1].Octals;
	}
	V7XUpdate(70, list, octals, 3);
}

void RTCC::V7XUpdate(int verb, char *list, int* emem, int n)
{
	sprintf(list, "V%dE%dE", verb, emem[0]);
	for (int i = 1;i < n;i++)
	{
		sprintf(list, "%s%dE", list, emem[i]);
	}
	sprintf(list, "%sV33E", list);
}

void RTCC::TLANDUpdate(char *list, double t_land)
{
	CMMDTGTU(t_land);
	V7XUpdate(72, list, CZTDTGTU.Octals, 5);
}

void RTCC::SunburstAttitudeManeuver(char *list, VECTOR3 imuangles)
{
	int emem[3];

	if (imuangles.x > PI)
	{
		imuangles.x -= PI2;
	}
	if (imuangles.y > PI)
	{
		imuangles.y -= PI2;
	}
	if (imuangles.z > PI)
	{
		imuangles.z -= PI2;
	}

	emem[0] = OrbMech::DoubleToBuffer(imuangles.x / PI, 0, 1);
	emem[1] = OrbMech::DoubleToBuffer(imuangles.y / PI, 0, 1);
	emem[2] = OrbMech::DoubleToBuffer(imuangles.z / PI, 0, 1);

	sprintf(list, "V21N1E372E0EV25N1E1631E%dE%dE%dEV25N26E4001E2067E70063EV30ER", emem[0], emem[1], emem[2]);
}

void RTCC::SunburstLMPCommand(char *list, int code)
{
	sprintf(list, "V67E%oEV33ER", code);
}

void RTCC::SunburstMassUpdate(char *list, double masskg)
{
	int emem[2];

	emem[0] = OrbMech::DoubleToBuffer(masskg, 15, 1);
	emem[1] = OrbMech::DoubleToBuffer(masskg, 15, 0);

	sprintf(list, "V24N1E1320E%dE%dER", emem[0], emem[1]);
}

void RTCC::AGCDesiredREFSMMATUpdate(char *list, MATRIX3 REFSMMAT, bool cmc)
{
	MATRIX3 a;
	int emem[24];

	a = REFSMMAT;

	emem[0] = 24;
	if (cmc)
	{
		emem[1] = 306;
	}
	else
	{
		emem[1] = 3606;
	}
	emem[2] = OrbMech::DoubleToBuffer(a.m11, 1, 1);
	emem[3] = OrbMech::DoubleToBuffer(a.m11, 1, 0);
	emem[4] = OrbMech::DoubleToBuffer(a.m12, 1, 1);
	emem[5] = OrbMech::DoubleToBuffer(a.m12, 1, 0);
	emem[6] = OrbMech::DoubleToBuffer(a.m13, 1, 1);
	emem[7] = OrbMech::DoubleToBuffer(a.m13, 1, 0);
	emem[8] = OrbMech::DoubleToBuffer(a.m21, 1, 1);
	emem[9] = OrbMech::DoubleToBuffer(a.m21, 1, 0);
	emem[10] = OrbMech::DoubleToBuffer(a.m22, 1, 1);
	emem[11] = OrbMech::DoubleToBuffer(a.m22, 1, 0);
	emem[12] = OrbMech::DoubleToBuffer(a.m23, 1, 1);
	emem[13] = OrbMech::DoubleToBuffer(a.m23, 1, 0);
	emem[14] = OrbMech::DoubleToBuffer(a.m31, 1, 1);
	emem[15] = OrbMech::DoubleToBuffer(a.m31, 1, 0);
	emem[16] = OrbMech::DoubleToBuffer(a.m32, 1, 1);
	emem[17] = OrbMech::DoubleToBuffer(a.m32, 1, 0);
	emem[18] = OrbMech::DoubleToBuffer(a.m33, 1, 1);
	emem[19] = OrbMech::DoubleToBuffer(a.m33, 1, 0);

	V7XUpdate(71, list, emem, 20);
}

void RTCC::AGCREFSMMATUpdate(char *list, MATRIX3 REFSMMAT, bool cmc)
{
	MATRIX3 a;
	int emem[24];
	int address;

	a = REFSMMAT;

	if (cmc)
	{
		address = SystemParameters.MCCCRF;
	}
	else
	{
		address = SystemParameters.MCCLRF;
	}

	emem[0] = 24;
	emem[1] = address;
	emem[2] = OrbMech::DoubleToBuffer(a.m11, 1, 1);
	emem[3] = OrbMech::DoubleToBuffer(a.m11, 1, 0);
	emem[4] = OrbMech::DoubleToBuffer(a.m12, 1, 1);
	emem[5] = OrbMech::DoubleToBuffer(a.m12, 1, 0);
	emem[6] = OrbMech::DoubleToBuffer(a.m13, 1, 1);
	emem[7] = OrbMech::DoubleToBuffer(a.m13, 1, 0);
	emem[8] = OrbMech::DoubleToBuffer(a.m21, 1, 1);
	emem[9] = OrbMech::DoubleToBuffer(a.m21, 1, 0);
	emem[10] = OrbMech::DoubleToBuffer(a.m22, 1, 1);
	emem[11] = OrbMech::DoubleToBuffer(a.m22, 1, 0);
	emem[12] = OrbMech::DoubleToBuffer(a.m23, 1, 1);
	emem[13] = OrbMech::DoubleToBuffer(a.m23, 1, 0);
	emem[14] = OrbMech::DoubleToBuffer(a.m31, 1, 1);
	emem[15] = OrbMech::DoubleToBuffer(a.m31, 1, 0);
	emem[16] = OrbMech::DoubleToBuffer(a.m32, 1, 1);
	emem[17] = OrbMech::DoubleToBuffer(a.m32, 1, 0);
	emem[18] = OrbMech::DoubleToBuffer(a.m33, 1, 1);
	emem[19] = OrbMech::DoubleToBuffer(a.m33, 1, 0);

	V7XUpdate(71, list, emem, 20);
}

void RTCC::CMCRetrofireExternalDeltaVUpdate(char *list, double LatSPL, double LngSPL, double P30TIG, VECTOR3 dV_LVLH)
{
	int emem[24];
	double getign = P30TIG;

	emem[0] = 16;
	emem[1] = 3400;
	emem[2] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 1);
	emem[3] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 0);
	emem[4] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 1);
	emem[5] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 0);
	emem[6] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 1);
	emem[7] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 0);
	emem[8] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 1);
	emem[9] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 0);
	emem[10] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 1);
	emem[11] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 0);
	emem[12] = OrbMech::DoubleToBuffer(getign*100.0, 28, 1);
	emem[13] = OrbMech::DoubleToBuffer(getign*100.0, 28, 0);

	V7XUpdate(71, list, emem, 14);
}

void RTCC::CMCEntryUpdate(char *list, double LatSPL, double LngSPL)
{
	CMMENTRY(LatSPL, LngSPL);
	V7XUpdate(71, list, CZENTRY.Octals, 6);
}

void RTCC::AGSStateVectorPAD(const AGSSVOpt &opt, AP11AGSSVPAD &pad)
{
	EphemerisData sv1;
	VECTOR3 R, V;
	double AGSEpochTime, AGSEpochTime2, AGSEpochTime3, dt, scalR, scalV;

	//Calculate time in GMT
	AGSEpochTime = opt.sv.GMT;
	//Calculate time in AGS time
	AGSEpochTime2 = AGSEpochTime - GetAGSClockZero();
	//Round to the next 6 seconds
	AGSEpochTime2 = ceil(AGSEpochTime2 / 6.0)*6.0;
	//Convert back to GMT for coasting
	AGSEpochTime3 = AGSEpochTime2 + GetAGSClockZero();

	//Determine coasting time
	dt = AGSEpochTime3 - AGSEpochTime;

	if (opt.landed)
	{
		sv1.R = opt.sv.R + opt.sv.V*dt;
		sv1.V = opt.sv.V;
	}
	else
	{
		sv1 = coast(opt.sv, dt);
	}

	R = mul(opt.REFSMMAT, sv1.R);
	V = mul(opt.REFSMMAT, sv1.V);

	if (opt.sv.RBI == BODY_EARTH)
	{
		scalR = 1000.0;
		scalV = 1.0;
	}
	else
	{
		scalR = 100.0;
		scalV = 0.1;
	}

	if (opt.csm)
	{
		pad.DEDA244 = R.x / 0.3048 / scalR;
		pad.DEDA245 = R.y / 0.3048 / scalR;
		pad.DEDA246 = R.z / 0.3048 / scalR;
		pad.DEDA264 = V.x / 0.3048 / scalV;
		pad.DEDA265 = V.y / 0.3048 / scalV;
		pad.DEDA266 = V.z / 0.3048 / scalV;
		pad.DEDA272 = AGSEpochTime2 / 60.0;
	}
	else
	{
		pad.DEDA240 = R.x / 0.3048 / scalR;
		pad.DEDA241 = R.y / 0.3048 / scalR;
		pad.DEDA242 = R.z / 0.3048 / scalR;
		pad.DEDA260 = V.x / 0.3048 / scalV;
		pad.DEDA261 = V.y / 0.3048 / scalV;
		pad.DEDA262 = V.z / 0.3048 / scalV;
		pad.DEDA254 = AGSEpochTime2 / 60.0;
	}
}

void RTCC::NavCheckPAD(SV sv, AP7NAV &pad, double GET)
{
	SV sv1;
	EphemerisData sv2;
	double GETbase, lat, lng, alt, navcheckdt;

	GETbase = CalcGETBase();
	
	if (GET == 0.0)
	{
		navcheckdt = 30 * 60;
		pad.NavChk[0] = (sv.MJD - GETbase)*24.0*3600.0 + navcheckdt;
	}
	else
	{
		navcheckdt = GET - (sv.MJD - GETbase)*24.0*3600.0;
		pad.NavChk[0] = GET;
	}

	sv1 = coast(sv, navcheckdt);
	sv2 = ConvertSVtoEphemData(sv1);
	navcheck(sv2.R, sv2.GMT, sv2.RBI, lat, lng, alt);

	pad.alt[0] = alt / 1852.0;
	pad.lat[0] = lat*DEG;
	pad.lng[0] = lng*DEG;
}

void RTCC::P27PADCalc(const P27Opt &opt, P27PAD &pad)
{
	double lat, lng, alt, get, SVGMT, navcheckGMT;
	VECTOR3 pos, vel;
	bool csm = true;
	EphemerisData sv, sv1;

	//Convert to GMT
	SVGMT = GMTfromGET(opt.SVGET);
	navcheckGMT = GMTfromGET(opt.navcheckGET);

	sv = coast(opt.sv0, SVGMT - opt.sv0.GMT);
	sv1 = coast(opt.sv0, navcheckGMT - opt.sv0.GMT);

	navcheck(sv1.R, sv1.GMT, sv1.RBI, lat, lng, alt);
	
	sprintf(pad.Purpose[0], "SV");
	pad.GET[0] = opt.SVGET;
	pad.alt = alt / 1852.0;
	pad.lat = lat*DEG;
	pad.lng = lng*DEG;
	pad.NavChk = opt.navcheckGET;
	pad.Index[0] = 21;
	pad.Verb[0] = 71;

	pos = sv.R;
	vel = sv.V*0.01;
	get = opt.SVGET;

	pad.Data[0][0] = 1501;

	if (sv.RBI == BODY_MOON)
	{
		if (csm)
		{
			pad.Data[0][1] = 2;
		}
		else
		{
			pad.Data[0][1] = 77775;	// Octal coded decimal
		}

		pad.Data[0][2] = OrbMech::DoubleToBuffer(pos.x, 27, 1);
		pad.Data[0][3] = OrbMech::DoubleToBuffer(pos.x, 27, 0);
		pad.Data[0][4] = OrbMech::DoubleToBuffer(pos.y, 27, 1);
		pad.Data[0][5] = OrbMech::DoubleToBuffer(pos.y, 27, 0);
		pad.Data[0][6] = OrbMech::DoubleToBuffer(pos.z, 27, 1);
		pad.Data[0][7] = OrbMech::DoubleToBuffer(pos.z, 27, 0);
		pad.Data[0][8] = OrbMech::DoubleToBuffer(vel.x, 5, 1);
		pad.Data[0][9] = OrbMech::DoubleToBuffer(vel.x, 5, 0);
		pad.Data[0][10] = OrbMech::DoubleToBuffer(vel.y, 5, 1);
		pad.Data[0][11] = OrbMech::DoubleToBuffer(vel.y, 5, 0);
		pad.Data[0][12] = OrbMech::DoubleToBuffer(vel.z, 5, 1);
		pad.Data[0][13] = OrbMech::DoubleToBuffer(vel.z, 5, 0);
		pad.Data[0][14] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		pad.Data[0][15] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}
	else
	{
		if (csm)
		{
			pad.Data[0][1] = 1;
		}
		else
		{
			pad.Data[0][1] = 77776;	// Octal coded decimal
		}

		pad.Data[0][2] = OrbMech::DoubleToBuffer(pos.x, 29, 1);
		pad.Data[0][3] = OrbMech::DoubleToBuffer(pos.x, 29, 0);
		pad.Data[0][4] = OrbMech::DoubleToBuffer(pos.y, 29, 1);
		pad.Data[0][5] = OrbMech::DoubleToBuffer(pos.y, 29, 0);
		pad.Data[0][6] = OrbMech::DoubleToBuffer(pos.z, 29, 1);
		pad.Data[0][7] = OrbMech::DoubleToBuffer(pos.z, 29, 0);
		pad.Data[0][8] = OrbMech::DoubleToBuffer(vel.x, 7, 1);
		pad.Data[0][9] = OrbMech::DoubleToBuffer(vel.x, 7, 0);
		pad.Data[0][10] = OrbMech::DoubleToBuffer(vel.y, 7, 1);
		pad.Data[0][11] = OrbMech::DoubleToBuffer(vel.y, 7, 0);
		pad.Data[0][12] = OrbMech::DoubleToBuffer(vel.z, 7, 1);
		pad.Data[0][13] = OrbMech::DoubleToBuffer(vel.z, 7, 0);
		pad.Data[0][14] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		pad.Data[0][15] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}

}

void RTCC::SaveState(FILEHANDLE scn) {

	char Buffer[256];
	int i;

	oapiWriteLine(scn, RTCC_START_STRING);

	snprintf(Buffer, 127, "%s", SystemParametersFile.c_str());
	oapiWriteScenario_string(scn, "RTCC_SYSTEMPARAMETERSFILE", Buffer);
	snprintf(Buffer, 127, "%s", TLIFile.c_str());
	oapiWriteScenario_string(scn, "RTCC_TLIFILE", Buffer);
	snprintf(Buffer, 127, "%s", SFPFile.c_str());
	oapiWriteScenario_string(scn, "RTCC_SFPFILE", Buffer);
	// Booleans
	// Integers
	SAVE_INT("RTCC_GZGENCSN_Year", GZGENCSN.Year);
	SAVE_INT("RTCC_GZGENCSN_RefDayOfYear", GZGENCSN.RefDayOfYear);
	SAVE_INT("RTCC_GZGENCSN_DaysInYear", GZGENCSN.DaysInYear);
	SAVE_INT("RTCC_GZGENCSN_MonthofLiftoff", GZGENCSN.MonthofLiftoff);
	SAVE_INT("RTCC_GZGENCSN_DayofLiftoff", GZGENCSN.DayofLiftoff);
	SAVE_INT("RTCC_GZGENCSN_DaysinMonthofLiftoff", GZGENCSN.DaysinMonthofLiftoff);
	SAVE_INT("RTCC_SFP_MODE", PZSFPTAB.blocks[1].mode);
	// Floats
	SAVE_DOUBLE("RTCC_GMTBASE",SystemParameters.GMTBASE);
	SAVE_DOUBLE("RTCC_MCGMTL", SystemParameters.MCGMTL);
	SAVE_DOUBLE("RTCC_MCGZSA", SystemParameters.MCGZSA);
	SAVE_DOUBLE("RTCC_MCGZSL", SystemParameters.MCGZSL);
	SAVE_DOUBLE("RTCC_MCGZSS", SystemParameters.MCGZSS);
	SAVE_DOUBLE("RTCC_MCLABN", SystemParameters.MCLABN);
	SAVE_DOUBLE("RTCC_MCLSBN", SystemParameters.MCLSBN);
	SAVE_DOUBLE("RTCC_MCLCBN", SystemParameters.MCLCBN);
	SAVE_DOUBLE("RTCC_MCGRAG", SystemParameters.MCGRAG);
	SAVE_DOUBLE("RTCC_MCGRIC", SystemParameters.MCGRIC);
	SAVE_DOUBLE("RTCC_MCGRIL", SystemParameters.MCGRIL);
	SAVE_DOUBLE("RTCC_MCGREF", SystemParameters.MCGREF);
	SAVE_DOUBLE("RTCC_MCLAMD", SystemParameters.MCLAMD);
	SAVE_DOUBLE("RTCC_MCTVEN", SystemParameters.MCTVEN);
	SAVE_DOUBLE("RTCC_MDVSTP_T4IG", SystemParameters.MDVSTP.T4IG);
	SAVE_DOUBLE("RTCC_MDVSTP_T4C", SystemParameters.MDVSTP.T4C);
	if (SystemParameters.MGRTAG == 0) { SAVE_INT("MGRTAG", SystemParameters.MGRTAG); }
	
	SAVE_DOUBLE("RTCC_GZGENCSN_DKIELEVATIONANGLE", GZGENCSN.DKIElevationAngle);
	SAVE_DOUBLE("RTCC_GZGENCSN_DKITERMINALPHASEANGLE", GZGENCSN.DKITerminalPhaseAngle);
	SAVE_INT("RTCC_GZGENCSN_DKIPhaseAngleSetting", GZGENCSN.DKIPhaseAngleSetting);
	SAVE_DOUBLE("RTCC_GZGENCSN_TIDELTAH", GZGENCSN.TIDeltaH);
	SAVE_DOUBLE("RTCC_GZGENCSN_TIPHASEANGLE", GZGENCSN.TIPhaseAngle);
	SAVE_DOUBLE("RTCC_GZGENCSN_TIELEVATIONANGLE", GZGENCSN.TIElevationAngle);
	SAVE_DOUBLE("RTCC_GZGENCSN_TITRAVELANGLE", GZGENCSN.TITravelAngle);
	SAVE_DOUBLE("RTCC_GZGENCSN_TINSRNOMINALTIME", GZGENCSN.TINSRNominalTime);
	SAVE_DOUBLE("RTCC_GZGENCSN_TINSRNOMINALDELTAH", GZGENCSN.TINSRNominalDeltaH);
	SAVE_DOUBLE("RTCC_GZGENCSN_TINSRNOMINALPHASEANGLE", GZGENCSN.TINSRNominalPhaseAngle);
	SAVE_DOUBLE("RTCC_GZGENCSN_DKIDELTAH_NCC", GZGENCSN.DKIDeltaH_NCC);
	SAVE_DOUBLE("RTCC_GZGENCSN_DKIDELTAH_NSR", GZGENCSN.DKIDeltaH_NSR);
	SAVE_DOUBLE("RTCC_GZGENCSN_SPQDELTAH", GZGENCSN.SPQDeltaH);
	SAVE_DOUBLE("RTCC_GZGENCSN_SPQELEVATIONANGLE", GZGENCSN.SPQElevationAngle);
	SAVE_DOUBLE("RTCC_GZGENCSN_LDPPAzimuth", GZGENCSN.LDPPAzimuth);
	SAVE_DOUBLE("RTCC_GZGENCSN_LDPPHeightofPDI", GZGENCSN.LDPPHeightofPDI);
	SAVE_INT("RTCC_GZGENCSN_LDPPDwellOrbits", GZGENCSN.LDPPDwellOrbits);
	SAVE_BOOL("RTCC_GZGENCSN_LDPPPoweredDescentSimFlag", GZGENCSN.LDPPPoweredDescentSimFlag);
	SAVE_DOUBLE("RTCC_GZGENCSN_LDPPDescentFlightArc", GZGENCSN.LDPPDescentFlightArc);
	if (EZETVMED.SpaceDigVehID != -1)
	{
		SAVE_INT("EZETVMED_SpaceDigVehID", EZETVMED.SpaceDigVehID);
	}
	if (EZETVMED.SpaceDigCentralBody != -1)
	{
		SAVE_INT("EZETVMED_SpaceDigCentralBody", EZETVMED.SpaceDigCentralBody);
	}
	SAVE_V3("EZJGSTAR_STAR_392", EZJGSTAR[391]);
	SAVE_V3("EZJGSTAR_STAR_393", EZJGSTAR[392]);
	SAVE_V3("EZJGSTAR_STAR_394", EZJGSTAR[393]);
	SAVE_V3("EZJGSTAR_STAR_395", EZJGSTAR[394]);
	SAVE_V3("EZJGSTAR_STAR_396", EZJGSTAR[395]);
	SAVE_V3("EZJGSTAR_STAR_397", EZJGSTAR[396]);
	SAVE_V3("EZJGSTAR_STAR_398", EZJGSTAR[397]);
	SAVE_V3("EZJGSTAR_STAR_399", EZJGSTAR[398]);
	SAVE_V3("EZJGSTAR_STAR_400", EZJGSTAR[399]);
	SAVE_DOUBLE("PZLTRT_DT_Ins_TPI", PZLTRT.DT_Ins_TPI);
	SAVE_DOUBLE("PZLTRT_PoweredFlightArc", PZLTRT.PoweredFlightArc);
	SAVE_DOUBLE("PZLTRT_PoweredFlightTime", PZLTRT.PoweredFlightTime);

	SAVE_DOUBLE("RZC1RCNS_GLevel", RZC1RCNS.entry.GLevel);
	SAVE_DOUBLE("RZC1RCNS_GNInitialBank", RZC1RCNS.entry.GNInitialBank);

	SAVE_DOUBLE("RTCC_TLCCGET", PZMCCPLN.MidcourseGET);
	SAVE_DOUBLE("RTCC_TLCCVectorGET", PZMCCPLN.VectorGET);
	SAVE_DOUBLE("RTCC_TLCC_TLMIN", PZMCCPLN.TLMIN);
	SAVE_DOUBLE("RTCC_TLCC_TLMAX", PZMCCPLN.TLMAX);
	SAVE_DOUBLE("RTCC_TLCC_AZ_min", PZMCCPLN.AZ_min);
	SAVE_DOUBLE("RTCC_TLCC_AZ_max", PZMCCPLN.AZ_max);
	SAVE_DOUBLE("RTCC_TLCC_ETA1", PZMCCPLN.ETA1);
	SAVE_DOUBLE("RTCC_TLCC_REVS1", PZMCCPLN.REVS1);
	SAVE_DOUBLE("LOI_eta_1", PZLOIPLN.eta_1);
	SAVE_DOUBLE("LOI_REVS1", PZLOIPLN.REVS1);

	SAVE_DOUBLE2("RTCC_SFP_DPSI_LOI", PZSFPTAB.blocks[0].dpsi_loi, PZSFPTAB.blocks[1].dpsi_loi);
	SAVE_DOUBLE2("RTCC_SFP_DPSI_TEI", PZSFPTAB.blocks[0].dpsi_tei, PZSFPTAB.blocks[1].dpsi_tei);
	SAVE_DOUBLE2("RTCC_SFP_DT_LLS", PZSFPTAB.blocks[0].dt_lls, PZSFPTAB.blocks[1].dt_lls);
	SAVE_DOUBLE2("RTCC_SFP_DT_UPD_NOM", PZSFPTAB.blocks[0].dt_upd_nom, PZSFPTAB.blocks[1].dt_upd_nom);
	SAVE_DOUBLE2("RTCC_SFP_DV_TEI", PZSFPTAB.blocks[0].dv_tei, PZSFPTAB.blocks[1].dv_tei);
	SAVE_DOUBLE2("RTCC_SFP_GAMMA_LOI", PZSFPTAB.blocks[0].gamma_loi, PZSFPTAB.blocks[1].gamma_loi);
	SAVE_DOUBLE2("RTCC_SFP_GET_TLI", PZSFPTAB.blocks[0].GET_TLI, PZSFPTAB.blocks[1].GET_TLI);
	SAVE_DOUBLE2("RTCC_SFP_GMT_TIME_FLAG", PZSFPTAB.blocks[0].GMTTimeFlag, PZSFPTAB.blocks[1].GMTTimeFlag);
	SAVE_DOUBLE2("RTCC_SFP_GMT_ND", PZSFPTAB.blocks[0].GMT_nd, PZSFPTAB.blocks[1].GMT_nd);
	SAVE_DOUBLE2("RTCC_SFP_GMT_PC1", PZSFPTAB.blocks[0].GMT_pc1, PZSFPTAB.blocks[1].GMT_pc1);
	SAVE_DOUBLE2("RTCC_SFP_GMT_PC2", PZSFPTAB.blocks[0].GMT_pc2, PZSFPTAB.blocks[1].GMT_pc2);
	SAVE_DOUBLE2("RTCC_SFP_H_ND", PZSFPTAB.blocks[0].h_nd, PZSFPTAB.blocks[1].h_nd);
	SAVE_DOUBLE2("RTCC_SFP_H_PC1", PZSFPTAB.blocks[0].h_pc1, PZSFPTAB.blocks[1].h_pc1);
	SAVE_DOUBLE2("RTCC_SFP_H_PC2", PZSFPTAB.blocks[0].h_pc2, PZSFPTAB.blocks[1].h_pc2);
	SAVE_DOUBLE2("RTCC_SFP_INCL_FR", PZSFPTAB.blocks[0].incl_fr, PZSFPTAB.blocks[1].incl_fr);
	SAVE_DOUBLE2("RTCC_SFP_LAT_LLS", PZSFPTAB.blocks[0].lat_lls, PZSFPTAB.blocks[1].lat_lls);
	SAVE_DOUBLE2("RTCC_SFP_LAT_ND", PZSFPTAB.blocks[0].lat_nd, PZSFPTAB.blocks[1].lat_nd);
	SAVE_DOUBLE2("RTCC_SFP_LAT_PC1", PZSFPTAB.blocks[0].lat_pc1, PZSFPTAB.blocks[1].lat_pc1);
	SAVE_DOUBLE2("RTCC_SFP_LAT_PC2", PZSFPTAB.blocks[0].lat_pc2, PZSFPTAB.blocks[1].lat_pc2);
	SAVE_DOUBLE2("RTCC_SFP_LNG_LLS", PZSFPTAB.blocks[0].lng_lls, PZSFPTAB.blocks[1].lng_lls);
	SAVE_DOUBLE2("RTCC_SFP_LNG_ND", PZSFPTAB.blocks[0].lng_nd, PZSFPTAB.blocks[1].lng_nd);
	SAVE_DOUBLE2("RTCC_SFP_LNG_PC1", PZSFPTAB.blocks[0].lng_pc1, PZSFPTAB.blocks[1].lng_pc1);
	SAVE_DOUBLE2("RTCC_SFP_LNG_PC2", PZSFPTAB.blocks[0].lng_pc2, PZSFPTAB.blocks[1].lng_pc2);
	SAVE_DOUBLE2("RTCC_SFP_PSI_LLS", PZSFPTAB.blocks[0].psi_lls, PZSFPTAB.blocks[1].psi_lls);
	SAVE_DOUBLE2("RTCC_SFP_RAD_LLS", PZSFPTAB.blocks[0].rad_lls, PZSFPTAB.blocks[1].rad_lls);
	SAVE_DOUBLE2("RTCC_SFP_T_LO", PZSFPTAB.blocks[0].T_lo, PZSFPTAB.blocks[1].T_lo);
	SAVE_DOUBLE2("RTCC_SFP_T_TE", PZSFPTAB.blocks[0].T_te, PZSFPTAB.blocks[1].T_te);

	SAVE_DOUBLE("RTCC_PZREAP_RRBIAS", PZREAP.RRBIAS);

	for (i = 0; i < 5; i++)
	{
		if (PZREAP.PTPSite[i] != "")
		{
			sprintf(Buffer, "%d %s %lf %lf", i, PZREAP.PTPSite[i].c_str(), PZREAP.PTPLatitude[i]*DEG, PZREAP.PTPLongitude[i]*DEG);
			oapiWriteScenario_string(scn, "RTCC_PZREAP_PTPSite", Buffer);
		}
	}
	for (i = 0; i < 5; i++)
	{
		if (PZREAP.ATPSite[i] != "")
		{
			int length = 0;

			length = sprintf(Buffer, "%d %s", i, PZREAP.ATPSite[i].c_str());

			for (int j = 0; j < 10; j++)
			{
				if (PZREAP.ATPCoordinates[i][j] >= 1e9)
				{
					length += sprintf(Buffer + length, " %lf", 1e10);
				}
				else
				{
					length += sprintf(Buffer + length, " %lf", PZREAP.ATPCoordinates[i][j] * DEG);
				}
			}
			oapiWriteScenario_string(scn, "RTCC_PZREAP_ATPSite", Buffer);
		}
	}

	if (EZLASITE.REF != -1)
	{
		SAVE_INT("RTCC_EZLASITE_REF", EZLASITE.REF);
	}
	for (i = 0;i < 12;i++)
	{
		if (EZLASITE.Data[i].code != "")
		{
			papiWriteScenario_Station(scn, "RTCC_EZLASITE", i, EZLASITE.Data[i]);
		}
	}

	SAVE_DOUBLE("RTCC_P30TIG", TimeofIgnition);
	SAVE_DOUBLE("RTCC_SplLat", SplashLatitude);
	SAVE_DOUBLE("RTCC_SplLng", SplashLongitude);
	SAVE_DOUBLE("RTCC_TEI", calcParams.TEI);
	SAVE_DOUBLE("RTCC_EI", calcParams.EI);
	SAVE_DOUBLE("RTCC_TLI", calcParams.TLI);
	SAVE_DOUBLE("RTCC_LOI", calcParams.LOI);
	SAVE_DOUBLE("RTCC_SEP", calcParams.SEP);
	SAVE_DOUBLE("RTCC_DOI", calcParams.DOI);
	SAVE_DOUBLE("RTCC_PDI", calcParams.PDI);
	SAVE_DOUBLE("RTCC_TLAND", CZTDTGTU.GETTD);
	SAVE_DOUBLE("RTCC_LSAzi", calcParams.LSAzi);
	SAVE_DOUBLE("RTCC_LSLat", BZLAND.lat[RTCC_LMPOS_BEST]);
	SAVE_DOUBLE("RTCC_LSLng", BZLAND.lng[RTCC_LMPOS_BEST]);
	SAVE_DOUBLE("RTCC_LSRadius", BZLAND.rad[RTCC_LMPOS_BEST]);
	SAVE_DOUBLE("RTCC_LunarLiftoff", calcParams.LunarLiftoff);
	SAVE_DOUBLE("RTCC_Insertion", calcParams.Insertion);
	SAVE_DOUBLE("RTCC_Phasing", calcParams.Phasing);
	SAVE_DOUBLE("RTCC_CSI", calcParams.CSI);
	SAVE_DOUBLE("RTCC_CDH", calcParams.CDH);
	SAVE_DOUBLE("RTCC_TPI", calcParams.TPI);
	SAVE_DOUBLE("RTCC_TIGSTORE1", calcParams.TIGSTORE1);

	// Strings
	// Vectors
	SAVE_V3("RTCC_DVLVLH", DeltaV_LVLH);
	SAVE_V3("RTCC_DVSTORE1", calcParams.DVSTORE1);
	// Matrizes
	SAVE_M3("RTCC_StoredREFSMMAT", calcParams.StoredREFSMMAT);
	SAVE_M3("PZMATCSM_EPH", PZMATCSM.EPH);
	SAVE_M3("PZMATCSM_G", PZMATCSM.G);
	SAVE_M3("PZMATCSM_GG", PZMATCSM.GG);
	SAVE_M3("PZMATLEM_EPH", PZMATLEM.EPH);
	SAVE_M3("PZMATLEM_G", PZMATLEM.G);
	SAVE_M3("PZMATLEM_GG", PZMATLEM.GG);
	SAVE_M3("GZLTRA_IU1_REFSMMAT", GZLTRA.IU1_REFSMMAT);
	for (i = 0;i < 12;i++)
	{
		if (EZJGMTX1.data[i].ID > 0)
		{
			papiWriteScenario_REFS(scn, "REFSMMAT", 1, i, EZJGMTX1.data[i]);
		}
	}
	for (i = 0;i < 12;i++)
	{
		if (EZJGMTX3.data[i].ID > 0)
		{
			papiWriteScenario_REFS(scn, "REFSMMAT", 3, i, EZJGMTX3.data[i]);
		}
	}
	// State vectors
	papiWriteScenario_SV(scn, "RTCC_SVSTORE1", calcParams.SVSTORE1);
	if (EZEPH1.EPHEM.Header.TUP > 0) SAVE_INT("EZEPH1_TUP", EZEPH1.EPHEM.Header.TUP);
	papiWriteScenario_SV(scn, "RTCC_MPTCM_ANCHOR", 9, EZANCHR1.AnchorVectors[9]);
	if (EZEPH2.EPHEM.Header.TUP > 0) SAVE_INT("EZEPH2_TUP", EZEPH2.EPHEM.Header.TUP);
	papiWriteScenario_SV(scn, "RTCC_MPTLM_ANCHOR", 9, EZANCHR3.AnchorVectors[9]);
	for (i = 0;i < 12;i++)
	{
		if (BZUSEVEC.data[i].ID > 0)
		{
			papiWriteScenario_SV(scn, "RTCC_BZUSEVEC", i, BZUSEVEC.data[i]);
		}
	}
	for (i = 0;i < 8;i++)
	{
		if (BZEVLVEC.data[i].ID > 0)
		{
			papiWriteScenario_SV(scn, "RTCC_BZEVLVEC", i, BZEVLVEC.data[i]);
		}
	}

	PZMPTCSM.SaveState(scn, "MPTCSM_BEGIN", "MPTCSM_END");
	PZMPTLEM.SaveState(scn, "MPTLEM_BEGIN", "MPTLEM_END");
	RZDBSC1.SaveState(scn, "RZDBSC1_BEGIN", "RZDBSC1_END");

	oapiWriteLine(scn, RTCC_END_STRING);
}

// Load State
void RTCC::LoadState(FILEHANDLE scn) {
	char Buff[128];
	char *line;
	int tmp = 0; // Used in boolean type loader
	std::string strtemp;
	double darrtemp[10];
	int inttemp, inttemp2;
	REFSMMATData refs;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, RTCC_END_STRING, sizeof(RTCC_END_STRING))) {
			break;
		}
		if (papiReadScenario_string(line, "RTCC_MISSIONFILE", Buff))
		{
			//For backwards compatibility. Remove the "Constants" at the end of the file
			int len = strlen(Buff);

			if (len > 9)
			{
				Buff[len - 10] = '\0';
			}

			SystemParametersFile.assign(Buff);
			TLIFile = SystemParametersFile + " TLI";
			SFPFile = SystemParametersFile + " SFP";
			SystemParametersFile += " Constants";

			LoadMissionFiles();
		}
		else if (papiReadScenario_string(line, "RTCC_MISSIONFILES", Buff))
		{
			//To make initial scenario loading easier. Afterwards scenarios save the individual file names
			SystemParametersFile.assign(Buff);
			TLIFile = SystemParametersFile + " TLI";
			SFPFile = SystemParametersFile + " SFP";
			SystemParametersFile += " Constants";

			LoadMissionFiles();
		}
		else if (papiReadScenario_string(line, "RTCC_SYSTEMPARAMETERSFILE", Buff))
		{
			SystemParametersFile.assign(Buff);
			LoadMissionConstantsFile(SystemParametersFile);
		}
		else if (papiReadScenario_string(line, "RTCC_TLIFILE", Buff))
		{
			TLIFile.assign(Buff);
			QMMBLD(TLIFile);
		}
		else if (papiReadScenario_string(line, "RTCC_SFPFILE", Buff))
		{
			SFPFile.assign(Buff);
			QMSEARCH(SFPFile);
		}
		LOAD_INT("RTCC_GZGENCSN_Year", GZGENCSN.Year);
		LOAD_INT("RTCC_GZGENCSN_RefDayOfYear", GZGENCSN.RefDayOfYear);
		LOAD_INT("RTCC_GZGENCSN_DaysInYear", GZGENCSN.DaysInYear);
		LOAD_INT("RTCC_GZGENCSN_MonthofLiftoff", GZGENCSN.MonthofLiftoff);
		if (papiReadScenario_int(line, "RTCC_GZGENCSN_DayofLiftoff", GZGENCSN.DayofLiftoff))
		{
			//Same as in ARCore, if the year is 0 we assume that the RTCC hasn't been initialized
			if (GZGENCSN.Year != 0)
			{
				LoadLaunchDaySpecificParameters(GZGENCSN.Year, GZGENCSN.MonthofLiftoff, GZGENCSN.DayofLiftoff);
			}
		}
		LOAD_INT("RTCC_GZGENCSN_DaysinMonthofLiftoff", GZGENCSN.DaysinMonthofLiftoff);
		LOAD_INT("RTCC_SFP_MODE", PZSFPTAB.blocks[1].mode);
		LOAD_DOUBLE("RTCC_GMTBASE",SystemParameters.GMTBASE);
		LOAD_DOUBLE("RTCC_MCGMTL", SystemParameters.MCGMTL);
		LOAD_DOUBLE("RTCC_MCGZSA", SystemParameters.MCGZSA);
		LOAD_DOUBLE("RTCC_MCGZSL", SystemParameters.MCGZSL);
		LOAD_DOUBLE("RTCC_MCGZSS", SystemParameters.MCGZSS);
		LOAD_DOUBLE("RTCC_MCLABN", SystemParameters.MCLABN);
		LOAD_DOUBLE("RTCC_MCLSBN", SystemParameters.MCLSBN);
		LOAD_DOUBLE("RTCC_MCLCBN", SystemParameters.MCLCBN);
		LOAD_DOUBLE("RTCC_MCGRAG", SystemParameters.MCGRAG);
		LOAD_DOUBLE("RTCC_MCGRIC", SystemParameters.MCGRIC);
		LOAD_DOUBLE("RTCC_MCGRIL", SystemParameters.MCGRIL);
		LOAD_DOUBLE("RTCC_MCGREF", SystemParameters.MCGREF);
		LOAD_DOUBLE("RTCC_MCLAMD", SystemParameters.MCLAMD);
		LOAD_DOUBLE("RTCC_MCTVEN", SystemParameters.MCTVEN);
		LOAD_DOUBLE("RTCC_MDVSTP_T4IG", SystemParameters.MDVSTP.T4IG);
		LOAD_DOUBLE("RTCC_MDVSTP_T4C", SystemParameters.MDVSTP.T4C);
		LOAD_INT("MGRTAG", SystemParameters.MGRTAG);

		LOAD_DOUBLE("RTCC_GZGENCSN_DKIELEVATIONANGLE", GZGENCSN.DKIElevationAngle);
		LOAD_DOUBLE("RTCC_GZGENCSN_DKITERMINALPHASEANGLE", GZGENCSN.DKITerminalPhaseAngle);
		LOAD_INT("RTCC_GZGENCSN_DKIPhaseAngleSetting", GZGENCSN.DKIPhaseAngleSetting);
		LOAD_DOUBLE("RTCC_GZGENCSN_TIDELTAH", GZGENCSN.TIDeltaH);
		LOAD_DOUBLE("RTCC_GZGENCSN_TIPHASEANGLE", GZGENCSN.TIPhaseAngle);
		LOAD_DOUBLE("RTCC_GZGENCSN_TIELEVATIONANGLE", GZGENCSN.TIElevationAngle);
		LOAD_DOUBLE("RTCC_GZGENCSN_TITRAVELANGLE", GZGENCSN.TITravelAngle);
		LOAD_DOUBLE("RTCC_GZGENCSN_TINSRNOMINALTIME", GZGENCSN.TINSRNominalTime);
		LOAD_DOUBLE("RTCC_GZGENCSN_TINSRNOMINALDELTAH", GZGENCSN.TINSRNominalDeltaH);
		LOAD_DOUBLE("RTCC_GZGENCSN_TINSRNOMINALPHASEANGLE", GZGENCSN.TINSRNominalPhaseAngle);
		LOAD_DOUBLE("RTCC_GZGENCSN_DKIDELTAH_NCC", GZGENCSN.DKIDeltaH_NCC);
		LOAD_DOUBLE("RTCC_GZGENCSN_DKIDELTAH_NSR", GZGENCSN.DKIDeltaH_NSR);
		LOAD_DOUBLE("RTCC_GZGENCSN_SPQDELTAH", GZGENCSN.SPQDeltaH);
		LOAD_DOUBLE("RTCC_GZGENCSN_SPQELEVATIONANGLE", GZGENCSN.SPQElevationAngle);
		LOAD_DOUBLE("RTCC_GZGENCSN_LDPPAzimuth", GZGENCSN.LDPPAzimuth);
		LOAD_DOUBLE("RTCC_GZGENCSN_LDPPHeightofPDI", GZGENCSN.LDPPHeightofPDI);
		LOAD_INT("RTCC_GZGENCSN_LDPPDwellOrbits", GZGENCSN.LDPPDwellOrbits);
		LOAD_BOOL("RTCC_GZGENCSN_LDPPPoweredDescentSimFlag", GZGENCSN.LDPPPoweredDescentSimFlag);
		LOAD_DOUBLE("RTCC_GZGENCSN_LDPPDescentFlightArc", GZGENCSN.LDPPDescentFlightArc);

		LOAD_INT("EZETVMED_SpaceDigVehID", EZETVMED.SpaceDigVehID);
		LOAD_INT("EZETVMED_SpaceDigCentralBody", EZETVMED.SpaceDigCentralBody);

		LOAD_V3("EZJGSTAR_STAR_392", EZJGSTAR[391]);
		LOAD_V3("EZJGSTAR_STAR_393", EZJGSTAR[392]);
		LOAD_V3("EZJGSTAR_STAR_394", EZJGSTAR[393]);
		LOAD_V3("EZJGSTAR_STAR_395", EZJGSTAR[394]);
		LOAD_V3("EZJGSTAR_STAR_396", EZJGSTAR[395]);
		LOAD_V3("EZJGSTAR_STAR_397", EZJGSTAR[396]);
		LOAD_V3("EZJGSTAR_STAR_398", EZJGSTAR[397]);
		LOAD_V3("EZJGSTAR_STAR_399", EZJGSTAR[398]);
		LOAD_V3("EZJGSTAR_STAR_400", EZJGSTAR[399]);

		LOAD_DOUBLE("PZLTRT_DT_Ins_TPI", PZLTRT.DT_Ins_TPI);
		LOAD_DOUBLE("PZLTRT_PoweredFlightArc", PZLTRT.PoweredFlightArc);
		LOAD_DOUBLE("PZLTRT_PoweredFlightTime", PZLTRT.PoweredFlightTime);

		LOAD_DOUBLE("RZC1RCNS_GLevel", RZC1RCNS.entry.GLevel);
		LOAD_DOUBLE("RZC1RCNS_GNInitialBank", RZC1RCNS.entry.GNInitialBank);

		LOAD_DOUBLE("RTCC_TLCCGET", PZMCCPLN.MidcourseGET);
		LOAD_DOUBLE("RTCC_TLCCVectorGET", PZMCCPLN.VectorGET);
		LOAD_DOUBLE("RTCC_TLCC_TLMIN", PZMCCPLN.TLMIN);
		LOAD_DOUBLE("RTCC_TLCC_TLMAX", PZMCCPLN.TLMAX);
		LOAD_DOUBLE("RTCC_TLCC_AZ_min", PZMCCPLN.AZ_min);
		LOAD_DOUBLE("RTCC_TLCC_AZ_max", PZMCCPLN.AZ_max);
		LOAD_DOUBLE("RTCC_TLCC_ETA1", PZMCCPLN.ETA1);
		LOAD_DOUBLE("RTCC_TLCC_REVS1", PZMCCPLN.REVS1);
		LOAD_DOUBLE("LOI_eta_1", PZLOIPLN.eta_1);
		LOAD_DOUBLE("LOI_REVS1", PZLOIPLN.REVS1);

		LOAD_DOUBLE2("RTCC_SFP_DPSI_LOI", PZSFPTAB.blocks[0].dpsi_loi, PZSFPTAB.blocks[1].dpsi_loi);
		LOAD_DOUBLE2("RTCC_SFP_DPSI_TEI", PZSFPTAB.blocks[0].dpsi_tei, PZSFPTAB.blocks[1].dpsi_tei);
		LOAD_DOUBLE2("RTCC_SFP_DT_LLS", PZSFPTAB.blocks[0].dt_lls, PZSFPTAB.blocks[1].dt_lls);
		LOAD_DOUBLE2("RTCC_SFP_DT_UPD_NOM", PZSFPTAB.blocks[0].dt_upd_nom, PZSFPTAB.blocks[1].dt_upd_nom);
		LOAD_DOUBLE2("RTCC_SFP_DV_TEI", PZSFPTAB.blocks[0].dv_tei, PZSFPTAB.blocks[1].dv_tei);
		LOAD_DOUBLE2("RTCC_SFP_GAMMA_LOI", PZSFPTAB.blocks[0].gamma_loi, PZSFPTAB.blocks[1].gamma_loi);
		LOAD_DOUBLE2("RTCC_SFP_GET_TLI", PZSFPTAB.blocks[0].GET_TLI, PZSFPTAB.blocks[1].GET_TLI);
		LOAD_DOUBLE2("RTCC_SFP_GMT_TIME_FLAG", PZSFPTAB.blocks[0].GMTTimeFlag, PZSFPTAB.blocks[1].GMTTimeFlag);
		LOAD_DOUBLE2("RTCC_SFP_GMT_ND", PZSFPTAB.blocks[0].GMT_nd, PZSFPTAB.blocks[1].GMT_nd);
		LOAD_DOUBLE2("RTCC_SFP_GMT_PC1", PZSFPTAB.blocks[0].GMT_pc1, PZSFPTAB.blocks[1].GMT_pc1);
		LOAD_DOUBLE2("RTCC_SFP_GMT_PC2", PZSFPTAB.blocks[0].GMT_pc2, PZSFPTAB.blocks[1].GMT_pc2);
		LOAD_DOUBLE2("RTCC_SFP_H_ND", PZSFPTAB.blocks[0].h_nd, PZSFPTAB.blocks[1].h_nd);
		LOAD_DOUBLE2("RTCC_SFP_H_PC1", PZSFPTAB.blocks[0].h_pc1, PZSFPTAB.blocks[1].h_pc1);
		LOAD_DOUBLE2("RTCC_SFP_H_PC2", PZSFPTAB.blocks[0].h_pc2, PZSFPTAB.blocks[1].h_pc2);
		LOAD_DOUBLE2("RTCC_SFP_INCL_FR", PZSFPTAB.blocks[0].incl_fr, PZSFPTAB.blocks[1].incl_fr);
		LOAD_DOUBLE2("RTCC_SFP_LAT_LLS", PZSFPTAB.blocks[0].lat_lls, PZSFPTAB.blocks[1].lat_lls);
		LOAD_DOUBLE2("RTCC_SFP_LAT_ND", PZSFPTAB.blocks[0].lat_nd, PZSFPTAB.blocks[1].lat_nd);
		LOAD_DOUBLE2("RTCC_SFP_LAT_PC1", PZSFPTAB.blocks[0].lat_pc1, PZSFPTAB.blocks[1].lat_pc1);
		LOAD_DOUBLE2("RTCC_SFP_LAT_PC2", PZSFPTAB.blocks[0].lat_pc2, PZSFPTAB.blocks[1].lat_pc2);
		LOAD_DOUBLE2("RTCC_SFP_LNG_LLS", PZSFPTAB.blocks[0].lng_lls, PZSFPTAB.blocks[1].lng_lls);
		LOAD_DOUBLE2("RTCC_SFP_LNG_ND", PZSFPTAB.blocks[0].lng_nd, PZSFPTAB.blocks[1].lng_nd);
		LOAD_DOUBLE2("RTCC_SFP_LNG_PC1", PZSFPTAB.blocks[0].lng_pc1, PZSFPTAB.blocks[1].lng_pc1);
		LOAD_DOUBLE2("RTCC_SFP_LNG_PC2", PZSFPTAB.blocks[0].lng_pc2, PZSFPTAB.blocks[1].lng_pc2);
		LOAD_DOUBLE2("RTCC_SFP_PSI_LLS", PZSFPTAB.blocks[0].psi_lls, PZSFPTAB.blocks[1].psi_lls);
		LOAD_DOUBLE2("RTCC_SFP_RAD_LLS", PZSFPTAB.blocks[0].rad_lls, PZSFPTAB.blocks[1].rad_lls);
		LOAD_DOUBLE2("RTCC_SFP_T_LO", PZSFPTAB.blocks[0].T_lo, PZSFPTAB.blocks[1].T_lo);
		LOAD_DOUBLE2("RTCC_SFP_T_TE", PZSFPTAB.blocks[0].T_te, PZSFPTAB.blocks[1].T_te);

		LOAD_DOUBLE("RTCC_PZREAP_RRBIAS", PZREAP.RRBIAS);
		if (papiReadConfigFile_PTPSite(line, "RTCC_PZREAP_PTPSite", strtemp, darrtemp, inttemp))
		{
			PZREAP.PTPSite[inttemp] = strtemp;
			PZREAP.PTPLatitude[inttemp] = darrtemp[0];
			PZREAP.PTPLongitude[inttemp] = darrtemp[1];
		}
		else if (papiReadConfigFile_ATPSite(line, "RTCC_PZREAP_ATPSite", strtemp, darrtemp, inttemp))
		{
			PZREAP.ATPSite[inttemp] = strtemp;
			for (int i = 0; i < 10; i++)
			{
				PZREAP.ATPCoordinates[inttemp][i] = darrtemp[i];
			}
		}

		LOAD_INT("RTCC_EZLASITE_REF", EZLASITE.REF);
		papiReadScenario_Station(line, "RTCC_EZLASITE", EZLASITE.Data);

		LOAD_DOUBLE("RTCC_P30TIG", TimeofIgnition);
		LOAD_DOUBLE("RTCC_SplLat", SplashLatitude);
		LOAD_DOUBLE("RTCC_SplLng", SplashLongitude);
		LOAD_DOUBLE("RTCC_TEI", calcParams.TEI);
		LOAD_DOUBLE("RTCC_EI", calcParams.EI);
		LOAD_DOUBLE("RTCC_TLI", calcParams.TLI);
		LOAD_DOUBLE("RTCC_LOI", calcParams.LOI);
		LOAD_DOUBLE("RTCC_SEP", calcParams.SEP);
		LOAD_DOUBLE("RTCC_DOI", calcParams.DOI);
		LOAD_DOUBLE("RTCC_PDI", calcParams.PDI);
		LOAD_DOUBLE("RTCC_TLAND", CZTDTGTU.GETTD);
		LOAD_DOUBLE("RTCC_LSAzi", calcParams.LSAzi);
		LOAD_DOUBLE("RTCC_LSLat", BZLAND.lat[RTCC_LMPOS_BEST]);
		LOAD_DOUBLE("RTCC_LSLng", BZLAND.lng[RTCC_LMPOS_BEST]);
		LOAD_DOUBLE("RTCC_LSRadius", BZLAND.rad[RTCC_LMPOS_BEST]);
		LOAD_DOUBLE("RTCC_LunarLiftoff", calcParams.LunarLiftoff);
		LOAD_DOUBLE("RTCC_Insertion", calcParams.Insertion);
		LOAD_DOUBLE("RTCC_Phasing", calcParams.Phasing);
		LOAD_DOUBLE("RTCC_CSI", calcParams.CSI);
		LOAD_DOUBLE("RTCC_CDH", calcParams.CDH);
		LOAD_DOUBLE("RTCC_TPI", calcParams.TPI);
		LOAD_DOUBLE("RTCC_TIGSTORE1", calcParams.TIGSTORE1);

		LOAD_V3("RTCC_DVLVLH", DeltaV_LVLH);
		LOAD_V3("RTCC_DVSTORE1", calcParams.DVSTORE1);
		LOAD_M3("RTCC_StoredREFSMMAT", calcParams.StoredREFSMMAT);
		LOAD_M3("PZMATCSM_EPH", PZMATCSM.EPH);
		LOAD_M3("PZMATCSM_G", PZMATCSM.G);
		LOAD_M3("PZMATCSM_GG", PZMATCSM.GG);
		LOAD_M3("PZMATLEM_EPH", PZMATLEM.EPH);
		LOAD_M3("PZMATLEM_G", PZMATLEM.G);
		LOAD_M3("PZMATLEM_GG", PZMATLEM.GG);
		LOAD_M3("GZLTRA_IU1_REFSMMAT", GZLTRA.IU1_REFSMMAT);
		if (papiReadScenario_REFS(line, "REFSMMAT", inttemp, inttemp2, refs))
		{
			if (inttemp == 1)
			{
				EZJGMTX1.data[inttemp2] = refs;
			}
			else
			{
				EZJGMTX3.data[inttemp2] = refs;
			}
		}
		papiReadScenario_SV(line, "RTCC_SVSTORE1", calcParams.SVSTORE1);
		LOAD_INT("EZEPH1_TUP", EZEPH1.EPHEM.Header.TUP);
		papiReadScenario_SV(line, "RTCC_MPTCM_ANCHOR", EZANCHR1.AnchorVectors);
		LOAD_INT("EZEPH2_TUP", EZEPH2.EPHEM.Header.TUP);
		papiReadScenario_SV(line, "RTCC_MPTLM_ANCHOR", EZANCHR3.AnchorVectors);
		papiReadScenario_SV(line, "RTCC_BZUSEVEC", BZUSEVEC.data);
		papiReadScenario_SV(line, "RTCC_BZEVLVEC", BZEVLVEC.data);

		if (!strnicmp(line, "MPTCSM_BEGIN", sizeof("MPTCSM_BEGIN"))) {
			PZMPTCSM.LoadState(scn, "MPTCSM_END");
		}
		else if (!strnicmp(line, "MPTLEM_BEGIN", sizeof("MPTLEM_BEGIN"))) {
			PZMPTLEM.LoadState(scn, "MPTLEM_END");
		}
		else if (!strnicmp(line, "RZDBSC1_BEGIN", sizeof("RZDBSC1_BEGIN"))) {
			RZDBSC1.LoadState(scn, "RZDBSC1_END");
		}

	}

	//PMSVCT might need to use RTCC GMT
	TimeUpdate();

	if (EZANCHR1.AnchorVectors[9].Vector.GMT != 0)
	{
		PMSVCT(4, RTCC_MPT_CSM, EZANCHR1.AnchorVectors[9]);
	}
	if (EZANCHR3.AnchorVectors[9].Vector.GMT != 0 || EZANCHR3.AnchorVectors[9].LandingSiteIndicator)
	{
		PMSVCT(4, RTCC_MPT_LM, EZANCHR3.AnchorVectors[9]);
	}
	//Update Mission Plan Table display
	PMDMPT();

	return;
}

void RTCC::SetManeuverData(double TIG, VECTOR3 DV)
{
	TimeofIgnition = (TIG - CalcGETBase())*24.0*3600.0;
	DeltaV_LVLH = DV;
}

int RTCC::SPSRCSDecision(double a, VECTOR3 dV_LVLH)
{
	double t;

	t = length(dV_LVLH) / a;

	if (t > 0.5)
	{
		return RTCC_ENGINETYPE_CSMSPS;
	}
	else
	{
		return RTCC_ENGINETYPE_CSMRCSPLUS4;
	}
}

SV2 RTCC::ExecuteManeuver(SV2 sv, double P30TIG, VECTOR3 dV_LVLH, int Thruster)
{
	SV sv0 = ConvertEphemDatatoSV(sv.sv, sv.Mass);
	SV sv1 = ExecuteManeuver(sv0, P30TIG, dV_LVLH, sv.AttachedMass, Thruster);

	SV2 sv2;

	sv2.sv = ConvertSVtoEphemData(sv1);
	sv2.Mass = sv1.mass;
	sv2.AttachedMass = sv.AttachedMass;

	return sv2;
}

SV RTCC::ExecuteManeuver(SV sv, double P30TIG, VECTOR3 dV_LVLH, double attachedMass, int Thruster)
{
	MATRIX3 Q_Xx;
	VECTOR3 V_G;

	return ExecuteManeuver(sv, P30TIG, dV_LVLH, attachedMass, Thruster, Q_Xx, V_G);
}

SV RTCC::ExecuteManeuver(SV sv, double P30TIG, VECTOR3 dV_LVLH, double attachedMass, int Thruster, MATRIX3 &Q_Xx, VECTOR3 &V_G)
{
	//INPUT:
	//vessel: vessel interface
	//P30TIG: Time of ignition in seconds relative to GETbase
	//dV_LVLH: DV Vector in LVLH coordinates
	//sv: state vector on trajectory before the maneuver, doesn't have to be at TIG
	//attachedMass: mass of the attached vessel, 0 if no vessel present
	//optional:
	//F: thrust in Newton
	//isp: specific impulse in m/s
	//OUTPUT:
	//sv3: state vector at cutoff

	double GETbase, t_go, theta_T, F, v_e, F_average, wdot, OnboardThrust;
	VECTOR3 UX, UY, UZ, DV, DV_P, DV_C;
	SV sv2, sv3;

	GETbase = CalcGETBase();
	EngineParametersTable(Thruster, F, wdot, OnboardThrust);
	v_e = F / wdot;

	if (Thruster == RTCC_ENGINETYPE_LMDPS)
	{
		double ManPADBurnTime, bt;
		int step;

		//Estimates for average thrust (relevant for finite burntime compensation), complete and variable burntime
		LMThrottleProgram(F, v_e, attachedMass + sv.mass, length(dV_LVLH), F_average, ManPADBurnTime, bt, step);
	}
	else
	{
		F_average = F;
	}

	sv2 = coast(sv, P30TIG - (sv.MJD - GETbase)*24.0*3600.0);

	UY = unit(crossp(sv2.V, sv2.R));
	UZ = unit(-sv2.R);
	UX = crossp(UY, UZ);
	Q_Xx = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	DV = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
	DV_P = UX*dV_LVLH.x + UZ*dV_LVLH.z;

	if (length(DV_P) != 0.0)
	{
		theta_T = length(crossp(sv2.R, sv2.V))*length(dV_LVLH)*(sv2.mass + attachedMass) / OrbMech::power(length(sv2.R), 2.0) / F;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + UY*dV_LVLH.y;
	}
	else
	{
		V_G = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
	}

	OrbMech::poweredflight(sv2.R, sv2.V, sv2.MJD, sv2.gravref, F_average, v_e, sv2.mass + attachedMass, V_G, sv3.R, sv3.V, sv3.mass, t_go);
	sv3.gravref = sv2.gravref;
	sv3.MJD = sv2.MJD + t_go / 24.0 / 3600.0;
	sv3.mass -= attachedMass;

	return sv3;
}

void RTCC::RTEMoonTargeting(RTEMoonOpt *opt, EntryResults *res)
{
	RTEMoon* teicalc;
	SV sv0, sv1, sv2;
	bool endi = false;
	double GETbase, EMSAlt, dt22, MJDguess, LMmass, TZMINI;
	VECTOR3 R05G, V05G;
	OBJHANDLE hEarth = oapiGetObjectByName("Earth");
	OBJHANDLE hMoon = oapiGetObjectByName("Moon");

	GETbase = CalcGETBase();
	EMSAlt = 297431.0*0.3048;

	sv0 = opt->RV_MCC;

	if (opt->TIGguess == 0.0)
	{
		//TEI targeting uses "now" as the initial guess
		MJDguess = sv0.MJD;

		if (opt->RevsTillTEI != 0)
		{
			double t_period, dt;
			t_period = OrbMech::period(sv0.R, sv0.V, OrbMech::mu_Moon);
			dt = t_period * (double)opt->RevsTillTEI;
			MJDguess += dt / 24.0 / 3600.0;
		}
	}
	else
	{
		MJDguess = GETbase + opt->TIGguess / 24.0 / 3600.0;
	}

	sv1 = coast(sv0, (MJDguess - sv0.MJD)*24.0*3600.0);

	if (opt->SMODE == 12 || opt->SMODE == 14 || opt->SMODE == 16)
	{
		sv2 = sv1;
	}
	else
	{
		double TIG = OrbMech::P29TimeOfLongitude(SystemParameters.MAT_J2000_BRCS, sv1.R, sv1.V, sv1.MJD, hMoon, 180.0*RAD);
		sv2 = coast(sv1, (TIG - sv1.MJD)*24.0*3600.0);
	}

	if (opt->returnspeed != -1)
	{
		double DT_TEI_EI;

		DT_TEI_EI = 62.0*3600.0;

		if (opt->returnspeed == 0)
		{
			DT_TEI_EI += 24.0*3600.0;
		}
		else if (opt->returnspeed == 2)
		{
			DT_TEI_EI -= 24.0*3600.0;
		}

		double t0 = OrbMech::GETfromMJD(sv2.MJD, GETbase);
		TZMINI = t0 + DT_TEI_EI;
	}
	else
	{
		TZMINI = opt->t_zmin;
	}

	double LINE[10];

	if (opt->entrylongmanual)
	{
		LINE[0] = PI05;
		LINE[1] = opt->EntryLng;
		LINE[2] = -PI05;
		LINE[3] = opt->EntryLng;
		LINE[4] = 1e10;
		LINE[5] = 1e10;
	}
	else
	{
		if (opt->ATPLine < 0 || opt->ATPLine>4)
		{
			return;
		}

		for (int i = 0;i < 10;i++)
		{
			LINE[i] = PZREAP.ATPCoordinates[opt->ATPLine][i];
		}
	}
	
	EphemerisData2 sv3;
	sv3.R = sv2.R;
	sv3.V = sv2.V;
	sv3.GMT = OrbMech::GETfromMJD(sv2.MJD, GetGMTBase());
	teicalc = new RTEMoon(this, sv3, GetGMTBase(), SystemParameters.MCLAMD);
	teicalc->ATP(LINE);
	teicalc->READ(opt->SMODE, PZREAP.IRMAX, PZREAP.VRMAX, PZREAP.RRBIAS, PZREAP.MOTION, PZREAP.HMINMC, 2, 0.3, PZREAP.DVMAX, 0.0, opt->Inclination, 1.0*1852.0, GMTfromGET(TZMINI), 0.0);

	endi = teicalc->MASTER();

	if (endi == false)
	{
		delete teicalc;
		return;
	}

	dt22 = OrbMech::time_radius(teicalc->R_EI, teicalc->V_EI, OrbMech::R_Earth + EMSAlt, -1, OrbMech::mu_Earth);
	OrbMech::rv_from_r0v0(teicalc->R_EI, teicalc->V_EI, dt22, R05G, V05G, OrbMech::mu_Earth); //Entry Interface to 0.05g

	res->latitude = teicalc->EntryLatcor;
	res->longitude = teicalc->EntryLngcor;
	res->P30TIG = GETfromGMT(teicalc->sv0.GMT);
	res->dV_LVLH = teicalc->Entry_DV;
	res->ReA = teicalc->EntryAng;
	res->GET400K = GETfromGMT(teicalc->t_R);
	res->GET05G = res->GET400K + dt22;
	res->RTGO = OrbMech::CMCEMSRangeToGo(SystemParameters.MAT_J2000_BRCS, R05G, OrbMech::MJDfromGET(res->GET05G, GETbase), res->latitude, res->longitude);
	res->VIO = length(V05G);
	res->precision = teicalc->precision;
	res->Incl = teicalc->ReturnInclination;
	res->FlybyAlt = teicalc->FlybyPeriAlt;
	res->solutionfound = true;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0;
	}

	SV sv_tig;

	sv_tig.gravref = hMoon;
	sv_tig.mass = sv0.mass;
	sv_tig.MJD = OrbMech::MJDfromGET(teicalc->sv0.GMT, GetGMTBase());
	sv_tig.R = teicalc->sv0.R;
	sv_tig.V = teicalc->sv0.V;

	PoweredFlightProcessor(sv_tig, 0.0, opt->enginetype, LMmass, teicalc->Vig_apo - teicalc->sv0.V, false, res->P30TIG, res->dV_LVLH, res->sv_preburn, res->sv_postburn);

	delete teicalc;
}

void RTCC::LunarOrbitMapUpdate(EphemerisData sv0, AP10MAPUPDATE &pad, double pm)
{
	double ttoLOS, ttoAOS, ttoSS, ttoSR, ttoPM, MJD;
	OBJHANDLE hEarth, hSun, gravref;
	double t_lng, GETbase;

	GETbase = CalcGETBase();
	MJD = OrbMech::MJDfromGET(sv0.GMT, GetGMTBase());
	gravref = GetGravref(sv0.RBI);

	hEarth = oapiGetObjectByName("Earth");
	hSun = oapiGetObjectByName("Sun");

	ttoLOS = OrbMech::sunrise(SystemParameters.MAT_J2000_BRCS, sv0.R, sv0.V, MJD, gravref, hEarth, 0, 0, true);
	ttoAOS = OrbMech::sunrise(SystemParameters.MAT_J2000_BRCS, sv0.R, sv0.V, MJD, gravref, hEarth, 1, 0, true);

	pad.LOSGET = (MJD - GETbase)*24.0*3600.0 + ttoLOS;
	pad.AOSGET = (MJD - GETbase)*24.0*3600.0 + ttoAOS;

	ttoSS = OrbMech::sunrise(SystemParameters.MAT_J2000_BRCS, sv0.R, sv0.V, MJD, gravref, hSun, 0, 0, true);
	ttoSR = OrbMech::sunrise(SystemParameters.MAT_J2000_BRCS, sv0.R, sv0.V, MJD, gravref, hSun, 1, 0, true);

	pad.SSGET = (MJD - GETbase)*24.0*3600.0 + ttoSS;
	pad.SRGET = (MJD - GETbase)*24.0*3600.0 + ttoSR;

	t_lng = OrbMech::P29TimeOfLongitude(SystemParameters.MAT_J2000_BRCS, sv0.R, sv0.V, MJD, gravref, pm);
	ttoPM = (t_lng - MJD)*24.0 * 3600.0;
	pad.PMGET = (MJD - GETbase)*24.0*3600.0 + ttoPM;
}

void RTCC::LandmarkTrackingPAD(LMARKTRKPADOpt *opt, AP11LMARKTRKPAD &pad)
{
	SV sv1;
	VECTOR3 R_P, u;
	double GETbase, dt1, dt2, MJDguess, sinl, gamma, r_0, LmkRange;

	GETbase = CalcGETBase();

	for (int i = 0;i < opt->entries;i++)
	{
		MJDguess = GETbase + opt->LmkTime[i] / 24.0 / 3600.0;
		sv1 = coast(opt->sv0, (MJDguess - opt->sv0.MJD)*24.0*3600.0);

		R_P = unit(_V(cos(opt->lng[i])*cos(opt->lat[i]), sin(opt->lng[i])*cos(opt->lat[i]), sin(opt->lat[i])))*(oapiGetSize(sv1.gravref) + opt->alt[i]);

		dt1 = OrbMech::findelev_gs(SystemParameters.AGCEpoch, SystemParameters.MAT_J2000_BRCS, sv1.R, sv1.V, R_P, MJDguess, 180.0*RAD, sv1.gravref, LmkRange);
		dt2 = OrbMech::findelev_gs(SystemParameters.AGCEpoch, SystemParameters.MAT_J2000_BRCS, sv1.R, sv1.V, R_P, MJDguess, 145.0*RAD, sv1.gravref, LmkRange);

		pad.T1[i] = dt1 + (MJDguess - GETbase) * 24.0 * 60.0 * 60.0;
		pad.T2[i] = dt2 + (MJDguess - GETbase) * 24.0 * 60.0 * 60.0;

		u = unit(R_P);
		sinl = u.z;

		if (sv1.gravref == hEarth)
		{
			double a, b, r_F;
			a = 6378166;
			b = 6356784;
			gamma = b * b / a / a;
			r_F = sqrt(b*b / (1.0 - (1.0 - b * b / a / a)*(1.0 - sinl * sinl)));
			r_0 = r_F;
		}
		else
		{
			gamma = 1.0;
			r_0 = OrbMech::R_Moon;
		}

		pad.Alt[i] = (length(R_P) - r_0) / 1852.0;
		pad.CRDist[i] = LmkRange / 1852.0;

		pad.Lat[i] = atan2(u.z, gamma*sqrt(u.x*u.x + u.y*u.y))*DEG;
		pad.Lng05[i] = opt->lng[i] / 2.0*DEG;
	}

	pad.entries = opt->entries;
}

SV RTCC::coast(SV sv0, double dt)
{
	SV sv1;
	OBJHANDLE gravout = NULL;

	OrbMech::oneclickcoast(SystemParameters.AGCEpoch, sv0.R, sv0.V, sv0.MJD, dt, sv1.R, sv1.V, sv0.gravref, gravout);
	sv1.gravref = gravout;
	sv1.mass = sv0.mass;
	sv1.MJD = sv0.MJD + dt / 24.0 / 3600.0;

	return sv1;
}

EphemerisData RTCC::coast(EphemerisData sv1, double dt)
{
	EphemerisData sv2;

	double MJD = OrbMech::MJDfromGET(sv1.GMT,SystemParameters.GMTBASE);

	OrbMech::oneclickcoast(SystemParameters.AGCEpoch, sv1.R, sv1.V, MJD, dt, sv2.R, sv2.V, sv1.RBI, sv2.RBI);

	sv2.GMT = sv1.GMT + dt;

	return sv2;
}

EphemerisData RTCC::coast(EphemerisData sv1, double dt, double Weight, double Area, double KFactor, bool Venting)
{
	EMMENIInputTable in;

	in.AnchorVector = sv1;
	in.Area = Area;
	in.CutoffIndicator = 1;
	in.DensityMultiplier = KFactor;
	if (dt >= 0)
	{
		in.IsForwardIntegration = 1.0;
	}
	else
	{
		in.IsForwardIntegration = -1.0;
	}
	in.MaxIntegTime = abs(dt);
	in.Weight = Weight;
	in.VentPerturbationFactor = Venting ? 1.0 : 0.0;
	EMMENI(in);
	return in.sv_cutoff;
}

EphemerisData RTCC::coast(EphemerisData sv1, double dt, int veh)
{
	MissionPlanTable *mpt = GetMPTPointer(veh);

	bool venting = (mpt->CommonBlock.ConfigCode[RTCC_CONFIG_S] == true);

	return coast(sv1, dt, mpt->TotalInitMass, mpt->ConfigurationArea, mpt->KFactor, venting);
}

void RTCC::GetTLIParameters(VECTOR3 &RIgn_global, VECTOR3 &VIgn_global, VECTOR3 &dV_LVLH, double &IgnMJD)
{
	VECTOR3 RIgn, VIgn;
	double GETbase;
	SV sv, sv2;

	GETbase = CalcGETBase();

	IgnMJD = GETbase + TimeofIgnition / 24.0 / 3600.0;

	sv = StateVectorCalc(calcParams.src);

	sv2 = coast(sv, (IgnMJD - sv.MJD)*24.0*3600.0);

	//RIgn = tmul(Rot, sv2.R);
	//VIgn = tmul(Rot, sv2.V);
	RIgn = sv2.R;
	VIgn = sv2.V;

	RIgn_global = _V(RIgn.x, RIgn.z, RIgn.y);
	VIgn_global = _V(VIgn.x, VIgn.z, VIgn.y);
	dV_LVLH = DeltaV_LVLH;
}

bool RTCC::REFSMMATDecision(VECTOR3 Att)
{
	if (cos(Att.z) > 0.5) //Yaw between 300� and 60�
	{
		return true;
	}

	return false;
}

int RTCC::PMMSPT(PMMSPTInput &in)
{
	//S-IVB TLI IGM Pre-Thrust Targeting Module

	//QUEID
	//32: Recompute S-IVB TLI IGM target parameters for an iterable S-IVB TLI maneuver when a trajectory update is being performed (called by PMSVCT)
	//33: Recompute S-IVB TLI IGM target parameters when iterable S-IVB TLI maneuver is being frozen (called by PMMFUD)
	//34: Compute S-IVB TLI hypersurface quantities for TLI planning study aid (called by PMMEPP)
	//36: Transfer S-IVB TLI maneuver from TLI planning study aid (called by PMMXFR)
	//37: Direct input S-IVB TLI maneuver (called by PMMXFR)
	//38: Confirm the S-IVB TLI maneuver (called by PMMXFR)
	//39: Continue processing of direct input of S-IVB TLI maneuver with only injection opportunity input (called by PMMXFR)
	EphemerisDataTable2 ephtab;
	EphemerisData2 sv_out;
	EMSMISSInputTable emsin;
	TLITargetingParametersTable *tlitab = NULL;
	SIVBTLIMatrixTable *ADRMAT;
	MATRIX3 R_true_mean, RMAT, M, N, A, EPH, BB, GG, B, G;
	VECTOR3 TargetVector, T_P, R, V, P, S, P_dot, S_dot, Hbar, H_apo, W, Sbar_1, Cbar_1, Omega_X, Omega_Y, Omega_Z;
	double T_TH, alpha_TSS, alpha_TS, beta, T_RP, f, R_N, KP0, KY0, T3_apo, T_ST, tau3R, T2, Vex2, Mdot2, DV_BR, tau2N, lambda, dt_ig, T, dt4, dt4_apo;
	double t_D, cos_sigma, C3, e_N, RA, DEC, theta_E, RR, SINB, COSB, COSATS, t, p, r, vv, C1, C2, p_dot, h, w, du_apo, c, theta, du, cos_psi, sin_psi;
	double i, X1, X2, theta_N, P_N, T_M, alpha_D_apo, P_RP, Y_RP, T3, tau3, A_Z, Azo, Azs, i_P, theta_N_P;
	int J, OrigTLIInd, CurTLIInd, LD, counter, GRP15, KX, err, n;
	unsigned ORER_out;

	if (in.QUEID == 39)
	{
		T_TH = in.GMT;
		goto RTCC_PMMSPT_3_1;
		//goto RTCC_PMMSPT_7_1;
	}
	if (in.QUEID != 38)
	{
		goto RTCC_PMMSPT_3_1;
	}
	//TLI confirmation
	in.CurMan->CommonBlock.TUP = 0;
	if (in.dt >= 0.0)
	{
		if (in.dt == 0.0)
		{
			in.CurMan->dt = -1.0;
		}
		else
		{
			in.CurMan->dt = in.dt;
		}
	}
	if (in.T_RP < 0.0)
	{
		goto RTCC_PMMSPT_19_1;
	}
	in.CurMan->GMTI = in.T_RP;
	T = in.CurMan->GMTI;
	if (T <= in.StartTimeLimit)
	{
		return 79;
	}
	if (in.CurMan->dt != -1.0)
	{
		T = T + in.CurMan->dt;
	}
	if (T >= in.EndTimeLimit)
	{
		return 79;
	}
	dt_ig = in.CurMan->GMT_1 - in.CurMan->GMTMAN;
	if (SystemParameters.MCTJD1 > dt_ig)
	{
		return 79;
	}
	in.CurMan->Word84 = dt_ig - SystemParameters.MCTJD1;
	goto RTCC_PMMSPT_19_1;
RTCC_PMMSPT_3_1:
	if (in.QUEID == 32 || in.QUEID == 33)
	{
		//Read in maneuver block
		T_RP = in.CurMan->GMTMAN;
		OrigTLIInd = in.CurMan->Word78i[1];
		J = abs(OrigTLIInd);
		goto RTCC_PMMSPT_4_1;
	}
	if (in.QUEID == 34)
	{
		//TBD: load data from study aid. Or not?
		goto RTCC_PMMSPT_3_2;
	}
	T_RP = in.T_RP;
	if (T_RP > 0)
	{
		OrigTLIInd = -in.InjOpp;
	}
	else
	{
	RTCC_PMMSPT_3_2:
		OrigTLIInd = in.InjOpp;
	}

	CurTLIInd = OrigTLIInd;
	J = in.InjOpp;
RTCC_PMMSPT_4_1:
	LD = GZGENCSN.RefDayOfYear;

	for (counter = 0; counter < 10; counter++)
	{
		if (LD == PZSTARGP.data[counter].Day)
		{
			tlitab = &PZSTARGP.data[counter];
			break;
		}
	}

	if (tlitab == NULL)
	{
		return 85;
	}
	T_ST = tlitab->T_ST[J - 1];
	alpha_TSS = tlitab->alpha_TS[J - 1];
	beta = tlitab->beta[J - 1];
	f = tlitab->f[J - 1];
	R_N = tlitab->R_N[J - 1];
	KP0 = tlitab->KP0[J - 1];
	KY0 = tlitab->KY0[J - 1];
	T3_apo = tlitab->T3_apo[J - 1];
	tau3R = tlitab->tau3R[J - 1];
	T2 = tlitab->T2[J - 1];
	Vex2 = tlitab->Vex2[J - 1];
	Mdot2 = tlitab->Mdot2[J - 1];
	DV_BR = tlitab->DV_BR[J - 1];
	tau2N = tlitab->tau2N[J - 1];

	if (OrigTLIInd > 0)
	{
		goto RTCC_PMMSPT_6_3;
	}
	else if (OrigTLIInd == 0)
	{
		goto RTCC_PMMSPT_8_2;
	}

	emsin.MaxIntegTime = T_RP - in.GMT;
	if (emsin.MaxIntegTime < 0)
	{
		emsin.IsForwardIntegration = -1.0;
		emsin.MaxIntegTime = abs(emsin.MaxIntegTime);
		goto RTCC_PMMSPT_6_2;
	}
	else if (emsin.MaxIntegTime == 0.0)
	{
		//Move Time, R, V into Aux Output Table
		goto RTCC_PMMSPT_8_2;
	}
	else
	{
		goto RTCC_PMMSPT_6_1;
	}
RTCC_PMMSPT_6_1:
	emsin.IsForwardIntegration = 1.0;
RTCC_PMMSPT_6_2:
	emsin.EphemerisBuildIndicator = false;
	goto RTCC_PMMSPT_7_2;
RTCC_PMMSPT_6_3:
	T_TH = SystemParameters.MDVSTP.T4C + GetGMTLO()*3600.0 + T_ST;
	if (in.QUEID != 37)
	{
		goto RTCC_PMMSPT_7_1;
	}
	if (T_TH < in.PresentGMT)
	{
		T_TH = in.PresentGMT;
	}
	if (in.ReplaceCode != 0)
	{
		goto RTCC_PMMSPT_7_1;
	}
	in.T_RP = T_TH;
	goto RTCC_PMMSPT_19_1;

RTCC_PMMSPT_7_1:
	emsin.EphemerisLeftLimitGMT = T_TH;
	emsin.EphemerisRightLimitGMT = T_TH + 1.2*3600.0;
	emsin.MinEphemDT = 0.0;
	emsin.MinNumEphemPoints = 9;
	emsin.ECIEphemerisIndicator = true;
	emsin.ECTEphemerisIndicator = false;
	emsin.MCIEphemerisIndicator = false;
	emsin.MCTEphemerisIndicator = false;
	emsin.EphemerisBuildIndicator = true;
	emsin.ECIEphemTableIndicator = &ephtab;
RTCC_PMMSPT_7_2:
	emsin.AnchorVector.R = in.R;
	emsin.AnchorVector.V = in.V;
	emsin.AnchorVector.GMT = in.GMT;
	emsin.AnchorVector.RBI = BODY_EARTH;
	emsin.ManCutoffIndicator = 2;
	emsin.CutoffIndicator = 1;
	emsin.ManeuverIndicator = false;
	emsin.VehicleCode = in.Table;
	emsin.DensityMultOverrideIndicator = false;
	emsin.ManTimesIndicator = NULL;
	emsin.AuxTableIndicator = NULL;
	emsin.IgnoreManueverNumber = 0;
	EMSMISS(&emsin);
	if (emsin.NIAuxOutputTable.ErrorCode)
	{
		return 72;
	}
RTCC_PMMSPT_8_2:
	lambda = SystemParameters.MCLGRA + GetGMTLO()*3600.0*OrbMech::w_Earth;

	if (ELVCNV(GetGMTLO()*3600.0, RTCC_COORDINATES_ECT, RTCC_COORDINATES_ECI, R_true_mean))
	{
		return 84;
	}

	RMAT = mul(R_true_mean, _M(cos(lambda), -sin(lambda), 0, sin(lambda), cos(lambda), 0, 0, 0, 1));
	M = mul(_M(1, 0, 0, 0, 0, -1, 0, 1, 0), OrbMech::tmat(RMAT));
	dt4 = (SystemParameters.MDVSTP.T4C - SystemParameters.MDVSTP.T4IG) - SystemParameters.MDVSTP.DT4N;

	if (abs(dt4) > SystemParameters.MDVSTP.DTLIM)
	{
		dt4_apo = SystemParameters.MDVSTP.DTLIM*dt4 / abs(dt4);
	}
	else
	{
		dt4_apo = dt4;
	}
	t_D = GetGMTLO()*3600.0 - tlitab->T_LO;
	
	GRP15 = PCMSP2(tlitab, J, t_D, cos_sigma, C3, e_N, RA, DEC);
	if (GRP15)
	{
		return GRP15;
	}
	TargetVector = _V(cos(RA)*cos(DEC), sin(RA)*cos(DEC), sin(DEC));
	theta_E = tlitab->theta_EO + tlitab->omega_E * t_D;
	N = mul(RMAT, _M(cos(theta_E), sin(theta_E), 0, -sin(theta_E), cos(theta_E), 0, 0, 0, 1));
	T_P = mul(N, TargetVector);
	R = ephtab.table[0].R;
	V = ephtab.table[0].V;
	RR = dotp(R, R);
	COSB = cos(beta);
	SINB = sin(beta);
	if (OrigTLIInd < 0)
	{
		goto RTCC_PMMSPT_13_2;
	}
	alpha_TS = alpha_TSS + tlitab->K_a1*dt4_apo + tlitab->K_a2*dt4_apo*dt4_apo;
	COSATS = cos(alpha_TS);
	t = T_TH;
	P = V * RR - R * dotp(R, V);
	p = length(P);
	r = length(R);
	vv = dotp(V, V);
	C1 = COSB / r;
	C2 = SINB / p;
	S = R * C1 + P * C2;
	P_dot = V * dotp(R, V) - R * vv;
	p_dot = length(P_dot);
	C2 = SINB / p;
	S_dot = V * C1 + P_dot * C2;
	if (dotp(S_dot, T_P) < 0 && dotp(S, T_P) <= COSATS)
	{
		T_RP = t;
		goto RTCC_PMMSPT_14_1;
	}
	KX = 1;
RTCC_PMMSPT_12_2:
	RR = dotp(R, R);
	Hbar = crossp(R, V);
	h = length(Hbar);
	H_apo = Hbar / h;
	W = crossp(H_apo, T_P);
	w = length(W);
	du_apo = acos(COSATS / w);
	c = dotp(H_apo, T_P);
	P = T_P - H_apo * c;
	theta = atan2(dotp(crossp(R, P), H_apo), dotp(R, P));
	if (KX == 1 && theta < 0 && abs(theta)>du_apo)
	{
		theta = theta + PI2;
	}
	du = theta + du_apo - beta;
	if (abs(du) > 0.0001 && KX < 8)
	{
		t = t + du * RR / h;
		KX++;
		err = ELVARY(ephtab, 8, t, true, sv_out, ORER_out);
		if (err == 16 || err == 32)
		{
			return 77;
		}
		R = sv_out.R;
		V = sv_out.V;
		goto RTCC_PMMSPT_12_2;
	}
	T_RP = t;
RTCC_PMMSPT_13_2:
	P = V * RR - R * dotp(R, V);
	p = length(P);
	r = length(R);
	C1 = COSB / r;
	C2 = SINB / p;
	S = R * C1 + P * C2;
RTCC_PMMSPT_14_1:
	cos_psi = dotp(S, T_P);
	sin_psi = sqrt(1.0 - cos_psi * cos_psi);
	Sbar_1 = (S*cos_psi-T_P) / sin_psi;
	Cbar_1 = crossp(Sbar_1, S);
	Omega_X = _V(M.m11, M.m12, M.m13);
	Omega_Y = _V(M.m21, M.m22, M.m23);
	Omega_Z = _V(M.m31, M.m32, M.m33);
	i = acos(dotp(Omega_Y, Cbar_1));
	X1 = dotp(Omega_Z, crossp(Cbar_1, Omega_Y));
	X2 = dotp(Omega_X, crossp(Cbar_1, Omega_Y));
	theta_N = atan2(X1, X2);
	if (theta_N < 0)
	{
		theta_N += PI2;
	}
	if (in.QUEID == 34)
	{
		goto RTCC_PMMSPT_21_1;
	}
	P_N = (OrbMech::mu_Earth / C3)*(e_N*e_N - 1.0);
	T_M = P_N / (1.0 - e_N * cos_sigma);
	alpha_D_apo = acos(cos_psi) + atan2(dotp(Sbar_1, crossp(Cbar_1, Omega_Y)), dotp(S, crossp(Cbar_1, Omega_Y)));
	in.CurMan->dV_inertial.x = i;
	in.CurMan->dV_inertial.y = theta_N;
	in.CurMan->dV_inertial.z = e_N;
	in.CurMan->dV_LVLH.x = C3;
	in.CurMan->dV_LVLH.y = alpha_D_apo;
	in.CurMan->dV_LVLH.z = f;
	in.CurMan->Word67d = R_N;
	in.CurMan->Word69 = T_M;
	goto RTCC_PMMSPT_15_2;
//RTCC_PMMSPT_15_1:
RTCC_PMMSPT_15_2:
	P_RP = KP0 + SystemParameters.MDVSTP.KP1 * dt4_apo + SystemParameters.MDVSTP.KP2 * dt4_apo*dt4_apo;
	Y_RP = KP0 + SystemParameters.MDVSTP.KY1 * dt4_apo + SystemParameters.MDVSTP.KY2 * dt4_apo*dt4_apo;
	T3 = T3_apo - tlitab->K_T3 * dt4_apo;
	tau3 = tau3R - dt4_apo;
	in.CurMan->Word73 = P_RP;
	in.CurMan->Word74 = Y_RP;
	in.CurMan->Word76 = T3;
	in.CurMan->Word77 = tau3;
	in.CurMan->Word78i[0] = CurTLIInd;
	in.CurMan->Word78i[1] = OrigTLIInd;
	in.CurMan->Word79 = T2;
	in.CurMan->Word80 = Vex2;
	in.CurMan->Word81 = Mdot2;
	in.CurMan->Word82 = DV_BR;
	in.CurMan->Word83 = tau2N;

	A_Z = 0.0;
	if (t_D < PZSTARGP.data[counter].t_DS1)
	{
		if (t_D < PZSTARGP.data[counter].t_DS0)
		{
			PMXSPT("PMMSPT", 98);
		}
		for (n = 0;n < 5;n++)
		{
			A_Z += PZSTARGP.data[counter].hx[0][n] * pow((t_D - PZSTARGP.data[counter].t_D1) / PZSTARGP.data[counter].t_SD1, n);
		}
	}
	else if (t_D < PZSTARGP.data[counter].t_DS2)
	{
		for (n = 0;n < 5;n++)
		{
			A_Z += PZSTARGP.data[counter].hx[1][n] * pow((t_D - PZSTARGP.data[counter].t_D2) / PZSTARGP.data[counter].t_SD2, n);
		}
	}
	else
	{
		if (t_D > PZSTARGP.data[counter].t_DS3)
		{
			PMXSPT("PMMSPT", 98);
		}
		for (n = 0;n < 5;n++)
		{
			A_Z += PZSTARGP.data[counter].hx[2][n] * pow((t_D - PZSTARGP.data[counter].t_D3) / PZSTARGP.data[counter].t_SD3, n);
		}
	}
	Azo = 72.0*RAD;
	Azs = 36.0*RAD;
	i_P = SystemParameters.MDVSTP.fx[0] + SystemParameters.MDVSTP.fx[1] * (A_Z - Azo) / Azs + SystemParameters.MDVSTP.fx[2] * pow((A_Z - Azo) / Azs, 2) + SystemParameters.MDVSTP.fx[3] * pow((A_Z - Azo) / Azs, 3)
		+ SystemParameters.MDVSTP.fx[4] * pow((A_Z - Azo) / Azs, 4) + SystemParameters.MDVSTP.fx[5] * pow((A_Z - Azo) / Azs, 5) + SystemParameters.MDVSTP.fx[6] * pow((A_Z - Azo) / Azs, 6);
	theta_N_P = SystemParameters.MDVSTP.gx[0] + SystemParameters.MDVSTP.gx[1] * (A_Z - Azo) / Azs + SystemParameters.MDVSTP.gx[2] * pow((A_Z - Azo) / Azs, 2) + SystemParameters.MDVSTP.gx[3] * pow((A_Z - Azo) / Azs, 3)
		+ SystemParameters.MDVSTP.gx[4] * pow((A_Z - Azo) / Azs, 4) + SystemParameters.MDVSTP.gx[5] * pow((A_Z - Azo) / Azs, 5) + SystemParameters.MDVSTP.gx[6] * pow((A_Z - Azo) / Azs, 6);

	A = _M(cos(SystemParameters.MDVSTP.PHIL), sin(SystemParameters.MDVSTP.PHIL)*sin(A_Z), -sin(SystemParameters.MDVSTP.PHIL)*cos(A_Z), -sin(SystemParameters.MDVSTP.PHIL), cos(SystemParameters.MDVSTP.PHIL)*sin(A_Z), -cos(SystemParameters.MDVSTP.PHIL)*cos(A_Z), 0, cos(A_Z), sin(A_Z));
	EPH = mul(OrbMech::tmat(A), M);
	BB = _M(cos(theta_N_P), 0, sin(theta_N_P), sin(theta_N_P)*sin(i_P), cos(i_P), -cos(theta_N_P)*sin(i_P), -sin(theta_N_P)*cos(i_P), sin(i_P), cos(theta_N_P)*cos(i_P));
	GG = mul(BB, A);
	B = _M(cos(theta_N), 0, sin(theta_N), sin(theta_N)*sin(i), cos(i), -cos(theta_N)*sin(i), -sin(theta_N)*cos(i), sin(i), cos(theta_N)*cos(i));
	G = mul(B, A);

	in.CurMan->GMTI = T_RP + SystemParameters.MDVSTP.DTIG;
	in.CurMan->Word84 = SystemParameters.MDVSTP.DTIG - SystemParameters.MCTJD1;
//RTCC_PMMSPT_18_1:
	if (in.Table == RTCC_MPT_CSM)
	{
		ADRMAT = &PZMATCSM;
	}
	else
	{
		ADRMAT = &PZMATLEM;
	}
	ADRMAT->EPH = EPH;
	ADRMAT->GG = GG;
	ADRMAT->G = G;

	if (in.QUEID != 36 && in.QUEID != 37 && in.QUEID != 39)
	{
		goto RTCC_PMMSPT_20_1;
	}
	if (T_RP <= in.StartTimeLimit || T_RP >= in.EndTimeLimit)
	{
		//Error
	RTCC_PMMSPT_18_2:
		return 79;
	}
	in.mpt->TimeToBeginManeuver[0] = in.mpt->TimeToEndManeuver[0] = T_RP;
	in.CurMan->CommonBlock.ConfigCode = in.CC;
	in.CurMan->CommonBlock.ConfigChangeInd = in.CCI;
	in.CurMan->AttitudeCode = in.AttitudeMode;
	in.CurMan->Thruster = in.ThrusterCode;
	in.CurMan->ConfigCodeBefore = in.CCMI;
	in.CurMan->TVC = in.TVC;
	in.CurMan->FrozenManeuverInd = false;
	in.CurMan->RefBodyInd = BODY_EARTH;
	in.CurMan->CoordSysInd = 0;
	in.CurMan->GMTMAN = T_RP;
	
	if (in.PrevMan)
	{
		in.CurMan->DockingAngle = in.PrevMan->DockingAngle;
	}
	else
	{
		in.CurMan->DockingAngle = in.mpt->DeltaDockingAngle;
	}
	in.CurMan->TrajDet[0] = in.CurMan->TrajDet[1] = in.CurMan->TrajDet[2] = 1;

RTCC_PMMSPT_19_1:
	return 0;
RTCC_PMMSPT_20_1:
	//TBD: TLI maneuver no 1?
	if (T_RP <= RTCCPresentTimeGMT())
	{
		goto RTCC_PMMSPT_18_2;
	}
	in.CurMan->Word78i[0] = OrigTLIInd;
	if (OrigTLIInd < 0)
	{
		if (in.QUEID == 32)
		{
			//Trajectory Update
			goto RTCC_PMMSPT_20_2;
		}
		else
		{
			//Freeze
			goto RTCC_PMMSPT_20_3;
		}
	}
	in.mpt->TimeToBeginManeuver[0] = in.CurMan->GMTMAN = T_RP;
	if (in.QUEID != 33)
	{
		//Trajectory Update
	RTCC_PMMSPT_20_2:
		//DTWRITE: Output maneuver block of MPT
		goto RTCC_PMMSPT_19_1;
	}
RTCC_PMMSPT_20_3:
	//DTWRITE: Output block 1 of MPT
	in.CurMan->FrozenManeuverInd = true;
	in.CurMan->StationIDFrozen = in.StationID;
	in.CurMan->GMTFrozen = in.GMT;
	in.CurMan->FrozenManeuverVector.R = R;
	in.CurMan->FrozenManeuverVector.V = V;
	in.CurMan->FrozenManeuverVector.GMT = T_RP;
	in.CurMan->FrozenManeuverVector.RBI = BODY_EARTH;
	goto RTCC_PMMSPT_20_2;
RTCC_PMMSPT_21_1:
	//Study Aid Input
	//Save data in study aid table
	double sigma = acos(cos_sigma);
	PZTTLIPL.T = T_P;
	PZTTLIPL.S = S;
	PZTTLIPL.R = R;
	PZTTLIPL.V = V;
	PZTTLIPL.TB6 = T_RP;
	PZTTLIPL.TIG = T_RP + SystemParameters.MDVSTP.DTIG;
	PZTTLIPL.i = i;
	PZTTLIPL.theta_N = theta_N;
	PZTTLIPL.sigma = sigma;
	PZTTLIPL.C3 = C3;
	goto RTCC_PMMSPT_19_1;
}

int RTCC::PCMSP2(TLITargetingParametersTable *tlitab, int J, double t_D, double &cos_sigma, double &C3, double &e_N, double &RA, double &DEC)
{
	int k = J - 1;
	int i = 0;
	double t_temp;
	bool direct = true;
	do
	{
		t_temp = tlitab->t_D[k][i];
		if (t_D == t_temp)
		{
			break;
		}
		else if (t_D < t_temp)
		{
			direct = false;
			break;
		}
		else
		{
			if (i >= 14)
			{
				return 85;
			}
			else
			{
				i++;
			}
		}
	} while (i < 15);

	if (i == 0 || direct)
	{
		cos_sigma = tlitab->cos_sigma[k][i];
		C3 = tlitab->C_3[k][i];
		e_N = tlitab->e_N[k][i];
		RA = tlitab->RA[k][i];
		DEC = tlitab->DEC[k][i];
		return 0;
	}

	double A = (t_D - tlitab->t_D[k][i - 1]) / (tlitab->t_D[k][i] - tlitab->t_D[k][i - 1]);
	cos_sigma = tlitab->cos_sigma[k][i - 1] + A * (tlitab->cos_sigma[k][i] - tlitab->cos_sigma[k][i - 1]);
	C3 = tlitab->C_3[k][i - 1] + A * (tlitab->C_3[k][i] - tlitab->C_3[k][i - 1]);
	e_N = tlitab->e_N[k][i - 1] + A * (tlitab->e_N[k][i] - tlitab->e_N[k][i - 1]);
	RA = tlitab->RA[k][i - 1] + A * (tlitab->RA[k][i] - tlitab->RA[k][i - 1]);
	DEC = tlitab->DEC[k][i - 1] + A * (tlitab->DEC[k][i] - tlitab->DEC[k][i - 1]);
	return 0;
}

void RTCC::EngineParametersTable(int enginetype, double &Thrust, double &WLR, double &OnboardThrust)
{
	switch (enginetype)
	{
	case RTCC_ENGINETYPE_CSMRCSPLUS2:
		Thrust = SystemParameters.MCTCT1;
		WLR = SystemParameters.MCTCW1;
		OnboardThrust = SystemParameters.MCTCT5;
		break;
	case RTCC_ENGINETYPE_CSMRCSPLUS4:
		Thrust = SystemParameters.MCTCT2;
		WLR = SystemParameters.MCTCW2;
		OnboardThrust = SystemParameters.MCTCT6;
		break;
	case RTCC_ENGINETYPE_CSMRCSMINUS2:
		Thrust = SystemParameters.MCTCT3;
		WLR = SystemParameters.MCTCW3;
		OnboardThrust = SystemParameters.MCTCT5;
		break;
	case RTCC_ENGINETYPE_CSMRCSMINUS4:
		Thrust = SystemParameters.MCTCT4;
		WLR = SystemParameters.MCTCW4;
		OnboardThrust = SystemParameters.MCTCT6;
		break;
	case RTCC_ENGINETYPE_LOX_DUMP:
		Thrust = OnboardThrust = 3300.0;
		WLR = Thrust / 157.0;
		break;
	case RTCC_ENGINETYPE_LMRCSPLUS2:
		Thrust = SystemParameters.MCTLT1;
		WLR = SystemParameters.MCTLW1;
		OnboardThrust = SystemParameters.MCTLT5;
		break;
	case RTCC_ENGINETYPE_LMRCSPLUS4:
		Thrust = SystemParameters.MCTLT2;
		WLR = SystemParameters.MCTLW2;
		OnboardThrust = SystemParameters.MCTLT6;
		break;
	case RTCC_ENGINETYPE_LMRCSMINUS2:
		Thrust = SystemParameters.MCTLT3;
		WLR = SystemParameters.MCTLW3;
		OnboardThrust = SystemParameters.MCTLT5;
		break;
	case RTCC_ENGINETYPE_LMRCSMINUS4:
		Thrust = SystemParameters.MCTLT4;
		WLR = SystemParameters.MCTLW4;
		OnboardThrust = SystemParameters.MCTLT6;
		break;
	case RTCC_ENGINETYPE_CSMSPS:
		Thrust = SystemParameters.MCTST1;
		WLR = SystemParameters.MCTSW1;
		OnboardThrust = SystemParameters.MCTST9;
		break;
	case RTCC_ENGINETYPE_LMAPS:
		Thrust = SystemParameters.MCTAT1;
		WLR = SystemParameters.MCTAW1;
		OnboardThrust = SystemParameters.MCTAT9;
		break;
	case RTCC_ENGINETYPE_LMDPS:
		Thrust = SystemParameters.MCTDT1;
		WLR = SystemParameters.MCTDW1;
		OnboardThrust = SystemParameters.MCTDT9;
		break;
	}
}

void RTCC::FiniteBurntimeCompensation(SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, SV &sv_tig, SV &sv_out, bool agc)
{
	//For CSM, LM or CSM/LM docked maneuvers
	//INPUT:
	//sv: State vector at impulsive TIG
	//attachedMass: mass of the attached vessel, 0 if no vessel present
	//DV: Delta velocity of impulsive thrusting maneuver
	//engine: see defintion of RTCC_ENGINETYPEs
	//
	//OUTPUT:
	//DV_imp: non-impulsive delta velocity
	//t_slip: time in seconds the maneuver has slipped to compensate for finite burntime; should always be negative
	//sv_out: complete state vector at actual cutoff

	VECTOR3 DV_imp_inert;
	double f_T, isp, F_average, OnboardThrust, wdot;

	sv_out.gravref = sv.gravref;

	if (engine == RTCC_ENGINETYPE_LMDPS)
	{
		double bt, bt_var;
		int step;

		EngineParametersTable(engine, f_T, wdot, OnboardThrust);
		isp = f_T / wdot;

		LMThrottleProgram(f_T, isp, sv.mass + attachedMass, length(DV), F_average, bt, bt_var, step);
	}
	else
	{
		EngineParametersTable(engine, f_T, wdot, OnboardThrust);
		isp = f_T / wdot;
		F_average = OnboardThrust;
	}

	OrbMech::impulsive(SystemParameters.AGCEpoch, sv.R, sv.V, sv.MJD, sv.gravref, F_average, isp, sv.mass + attachedMass, sv.R, sv.V + DV, DV_imp_inert, t_slip, sv_out.R, sv_out.V, sv_out.MJD, sv_out.mass);

	sv_tig = coast(sv, t_slip);

	if (agc)
	{
		DV_imp = PIEXDV(sv_tig.R, sv_tig.V, sv.mass + attachedMass, f_T, DV_imp_inert, false);
	}
	else
	{
		DV_imp = PIAEDV(DV_imp_inert, sv_tig.R, sv_tig.V, sv_tig.R, false);
	}

	sv_out.mass -= attachedMass;
}

void RTCC::FiniteBurntimeCompensation(SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, bool agc)
{
	SV sv_tig, sv_cut;

	FiniteBurntimeCompensation(sv, attachedMass, DV, engine, DV_imp, t_slip, sv_tig, sv_cut, agc);
}

int RTCC::PoweredFlightProcessor(PMMMPTInput in, double &GMT_TIG, VECTOR3 &dV_LVLH)
{
	MissionPlanTable mpt;
	MPTManeuver man;

	//Defaults
	in.Attitude = RTCC_ATTITUDE_PGNS_EXDV;
	in.LowerTimeLimit = 0.0;
	in.UpperTimeLimit = 10e70;
	in.CurrentManeuver = 1;
	in.DockingAngle = 0.0;
	in.mpt = &mpt;
	in.VehicleWeight = in.CSMWeight + in.LMWeight;

	int err = PMMMPT(in, man);

	GMT_TIG = man.GMTI;
	dV_LVLH = man.dV_LVLH;

	return err;
}

void RTCC::PoweredFlightProcessor(EphemerisData sv0, double mass, double GET_TIG_imp, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, bool agc)
{
	SV sv1;
	sv1 = ConvertEphemDatatoSV(sv0, mass);
	PoweredFlightProcessor(sv1, GET_TIG_imp, enginetype, attachedMass, DV, DVIsLVLH, GET_TIG, dV_LVLH, agc);
}

void RTCC::PoweredFlightProcessor(SV sv0, double GET_TIG_imp, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, bool agc)
{
	SV sv_pre, sv_post;

	PoweredFlightProcessor(sv0, GET_TIG_imp, enginetype, attachedMass, DV, DVIsLVLH, GET_TIG, dV_LVLH, sv_pre, sv_post, agc);
}

void RTCC::PoweredFlightProcessor(SV sv0, double GET_TIG_imp, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, SV &sv_pre, SV &sv_post, bool agc)
{
	SV sv_imp;
	MATRIX3 Q_Xx;
	VECTOR3 DeltaV;
	double GETbase, t_slip;

	GETbase = CalcGETBase();

	//If TIG set to 0 use input SV MJD as TIG
	if (GET_TIG_imp == 0.0)
	{
		sv_imp = sv0;
		GET_TIG_imp = OrbMech::GETfromMJD(sv_imp.MJD, GETbase);
	}
	else
	{
		sv_imp = coast(sv0, GET_TIG_imp - OrbMech::GETfromMJD(sv0.MJD, GETbase));
	}

	if (DVIsLVLH)
	{
		Q_Xx = OrbMech::LVLH_Matrix(sv_imp.R, sv_imp.V);
		DeltaV = tmul(Q_Xx, DV);
	}
	else
	{
		DeltaV = DV;
	}

	FiniteBurntimeCompensation(sv_imp, attachedMass, DeltaV, enginetype, dV_LVLH, t_slip, sv_pre, sv_post, agc);

	GET_TIG = GET_TIG_imp + t_slip;
}

double RTCC::GetDockedVesselMass(VESSEL *vessel)
{
	//INPUT:
	//vessel: primary vessel
	//OUTPUT:
	//mass of the docked vessel, 0 if no vessel docked

	DOCKHANDLE dock;
	OBJHANDLE hLM;
	VESSEL *lm;
	double LMmass;

	if (vessel->DockingStatus(0) == 1)
	{
		dock = vessel->GetDockHandle(0);
		hLM = vessel->GetDockStatus(dock);
		lm = oapiGetVesselInterface(hLM);

		LMmass = lm->GetMass();
	}
	else
	{
		LMmass = 0.0;
	}

	return LMmass;
}

double RTCC::FindOrbitalMidnight(SV sv, double t_TPI_guess)
{
	SV sv1;
	double GET_SV, dt, ttoMidnight;

	OBJHANDLE hSun = oapiGetObjectByName("Sun");

	GET_SV = OrbMech::GETfromMJD(sv.MJD, CalcGETBase());
	dt = t_TPI_guess - GET_SV;

	sv1 = coast(sv, dt);

	ttoMidnight = OrbMech::sunrise(SystemParameters.MAT_J2000_BRCS, sv1.R, sv1.V, sv1.MJD, sv1.gravref, hSun, 1, 1, false);
	return t_TPI_guess + ttoMidnight;
}

void RTCC::FindRadarAOSLOS(SV sv, double lat, double lng, double &GET_AOS, double &GET_LOS)
{
	VECTOR3 R_P;
	double LmkRange, dt1, dt2;

	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)))*oapiGetSize(sv.gravref);

	dt1 = OrbMech::findelev_gs(SystemParameters.AGCEpoch, SystemParameters.MAT_J2000_BRCS, sv.R, sv.V, R_P, sv.MJD, 175.0*RAD, sv.gravref, LmkRange);
	dt2 = OrbMech::findelev_gs(SystemParameters.AGCEpoch, SystemParameters.MAT_J2000_BRCS, sv.R, sv.V, R_P, sv.MJD, 5.0*RAD, sv.gravref, LmkRange);

	GET_AOS = OrbMech::GETfromMJD(sv.MJD, CalcGETBase()) + dt1;
	GET_LOS = OrbMech::GETfromMJD(sv.MJD, CalcGETBase()) + dt2;
}

void RTCC::FindRadarMidPass(SV sv, double lat, double lng, double &GET_Mid)
{
	VECTOR3 R_P;
	double LmkRange, dt;

	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)))*oapiGetSize(sv.gravref);

	dt = OrbMech::findelev_gs(SystemParameters.AGCEpoch, SystemParameters.MAT_J2000_BRCS, sv.R, sv.V, R_P, sv.MJD, 90.0*RAD, sv.gravref, LmkRange);

	GET_Mid = OrbMech::GETfromMJD(sv.MJD, CalcGETBase()) + dt;
}

double RTCC::FindOrbitalSunrise(SV sv, double t_sunrise_guess)
{
	SV sv1;
	double GET_SV, dt, ttoSunrise;

	OBJHANDLE hSun = oapiGetObjectByName("Sun");

	GET_SV = OrbMech::GETfromMJD(sv.MJD, CalcGETBase());
	dt = t_sunrise_guess - GET_SV;

	sv1 = coast(sv, dt);

	ttoSunrise = OrbMech::sunrise(SystemParameters.MAT_J2000_BRCS, sv1.R, sv1.V, sv1.MJD, sv1.gravref, hSun, true, false, false);
	return t_sunrise_guess + ttoSunrise;
}

VECTOR3 RTCC::PointAOTWithCSM(MATRIX3 REFSMMAT, EphemerisData sv, int AOTdetent, int star, double dockingangle)
{
	MATRIX3 Rot, C_LSM, R_ML, C_MSM;
	VECTOR3 s_REF, U_D, U_OAN, U_A, U_DL, U_AL, U_R, GA;
	double EL, AZ, A;

	s_REF = EZJGSTAR[star - 1];
	U_D = mul(REFSMMAT, s_REF);

	EL = 45.0*RAD;
	AZ = 60.0*RAD*((double)AOTdetent) - 120.0*RAD;
	
	//Vector in LM body coordinates
	U_OAN = _V(sin(EL), cos(EL)*sin(AZ), cos(EL)*cos(AZ));
	Rot = OrbMech::CSMBodyToLMBody(dockingangle);
	//Vector in CSM body coordinates
	U_A = tmul(Rot, U_OAN);

	C_LSM = OrbMech::tmat(OrbMech::CALCSMSC(_V(0, 0, 0)));
	U_DL = tmul(C_LSM, U_D);
	U_AL = U_A;
	U_R = -unit(crossp(U_DL, U_AL));
	A = acos(dotp(U_AL, U_DL));
	R_ML = OrbMech::ROTCOMP(U_R, A);
	C_MSM = mul(C_LSM, R_ML);
	GA = OrbMech::CALCGAR(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), OrbMech::tmat(C_MSM));

	return GA;
}

double LLWP_CURVE(double DV1, double DV2, double DV3, double DH1, double DH2, double DH3, double DVMAX)
{
	double K1, K2, K3, A, B, C, R1, R2, DH_MAX;

	K1 = DV1 / ((DH1 - DH2)*(DH1 - DH3));
	K2 = DV2 / ((DH2 - DH1)*(DH2 - DH3));
	K3 = DV3 / ((DH3 - DH1)*(DH3 - DH2));
	A = K1 + K2 + K3;
	B = -(K1*(DH3 + DH2) + K2 * (DH3 + DH1) + K3 * (DH2 + DH1));
	C = K1 * DH2*DH3 + K2 * DH1*DH3 + K3 * DH1*DH2 - DVMAX;
	R1 = (-B + sqrt(B*B - 4.0*A*C)) / (2.0*A);
	R2 = (-B - sqrt(B*B - 4.0*A*C)) / (2.0*A);
	if (R1 - R2 > 0)
	{
		DH_MAX = R1;
	}
	else
	{
		DH_MAX = R1;
	}
	return DH_MAX;
}

void RTCC::LLWP_PERHAP(AEGHeader Header, AEGDataBlock sv, double &RAP, double &RPE)
{
	AEGDataBlock sv_temp;
	double t0 = sv.TE;
	double dt = 20.0*60.0;
	double R[3], U[3];

	sv.TIMA = 0;
	R[0] = sv.R;
	U[0] = sv.U;
	sv.TE += 20.0*60.0;
	pmmlaeg.CALL(Header, sv, sv_temp);
	R[1] = sv_temp.R;
	U[1] = sv_temp.U;
	sv.TE += 20.0*60.0;
	pmmlaeg.CALL(Header, sv, sv_temp);
	R[2] = sv_temp.R;
	U[2] = sv_temp.U;
	PCHAPE(R[0], R[1], R[2], U[0], U[1], U[2], RAP, RPE);
}

void RTCC::LLWP_HMALIT(AEGHeader Header, AEGDataBlock *sv, AEGDataBlock *sv_temp, int M, int P, int I_CDH, double DH, double &dv_CSI, double &dv_CDH, double &t_CDH)
{
	VECTOR3 H, J, K, L, R_m, V_m, V_m_apo;
	double Pgamma, u_apo, P_v, dgamma, DV_K_apo, t_CSI, theta_k_apo, mu, R_apo, R_dot_apo, gamma_m, p_p, V_H, PP, V, R_p_dot, G_s, G_c, theta_k, g_apo, R_TK, R_R, DV_K;
	double R_CDH, U_CSI, DH_G, du;
	int K_orp, m, p;

	mu = OrbMech::mu_Moon;
	t_CSI = sv[0].TE;
	m = M - 1;
	p = P - 1;

	Pgamma = 0.0;
	u_apo = sv_temp[m].U;
	P_v = 0.0;
	dgamma = 0.0;
	DV_K_apo = 0.0;
	K_orp = 0;

	if (I_CDH > 0)
	{
		t_CDH = t_CSI + PI2 / sv[m].l_dot / 2.0*(double)(I_CDH);
	}
	else
	{
		if (sv[m].coe_osc.l - PI < 0)
		{

		}
		else
		{

		}
	}
	sv[m].TIMA = 0;
	sv[m].TE = t_CDH;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
	theta_k_apo = OrbMech::THETR(sv_temp[m].U, sv_temp[p].U, sv_temp[m].coe_osc.i, sv_temp[p].coe_osc.i, sv_temp[m].coe_osc.h, sv_temp[p].coe_osc.h);
	sv[m].TIMA = 0;
	sv[m].TE = t_CSI;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
	OrbMech::GIMKIC(sv_temp[m].coe_osc, mu, R_m, V_m);
	V_m_apo = V_m;
	K = unit(R_m);
	L = unit(V_m);
	H = unit(crossp(K, L));
	J = unit(crossp(K, H));
	R_apo = length(R_m);
	R_dot_apo = dotp(R_m, V_m) / R_apo;
	gamma_m = atan((R_dot_apo / length(V_m)) / sqrt(1.0 - pow(R_dot_apo / length(V_m), 2)));
	p_p = sv_temp[p].coe_osc.a*(1.0 - sv_temp[p].coe_osc.e*sv_temp[p].coe_osc.e);
	R_p_dot = sqrt(mu / p_p)*(sv_temp[p].coe_osc.e*cos(sv_temp[p].coe_osc.g)*sin(sv_temp[p].U) - sv_temp[p].coe_osc.e*sin(sv_temp[p].coe_osc.g)*cos(sv_temp[p].U));
	V_H = length(V_m)*cos(gamma_m);
	//RTCC Requirements had e_m instead of e_p
	PP = sv_temp[p].coe_osc.a*(1.0 - sv_temp[p].coe_osc.e*sv_temp[p].coe_osc.e);
	V = sqrt(mu*(2.0 / sv_temp[p].R - 1.0 / sv_temp[p].coe_osc.a));
	G_s = R_p_dot / V;
	G_c = sqrt(1.0 - G_s * G_s);
	theta_k = atan2(PP*G_s / G_c, P - sv_temp[p].R);
	g_apo = sv_temp[m].coe_osc.g;
	do
	{
		R_TK = PP / (1.0 + sv_temp[p].coe_osc.e*cos(theta_k_apo + theta_k + dgamma));
		R_R = R_TK - DH;
		DV_K = sqrt(2.0*mu / R_apo *(1.0 / (1.0 + R_apo / R_R))) - V_H;
		V_m = V_m_apo - J * DV_K;
		//Initialize
		sv[m].coe_osc = OrbMech::GIMIKC(R_m, V_m, mu);
		sv[m].ENTRY = 0;
		sv[m].TS = sv[m].TE = t_CSI;
		pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
		if (I_CDH > 0 || abs(DV_K_apo - DV_K) <= 1.0*0.3048)
		{
			break;
		}
		DV_K_apo = DV_K;
	} while (abs(DV_K_apo - DV_K) > 1.0*0.3048);
	R_CDH = R_R;
	dv_CSI = DV_K;
RTCC_LLWP_HMALIT_5_2:
	V_m = V_m_apo - J * dv_CSI;
	//Initialize
	sv[m].coe_osc = OrbMech::GIMIKC(R_m, V_m, mu);
	sv[m].ENTRY = 0;
	sv[m].TIMA = 0;
	sv[m].TS = sv[m].TE = t_CSI;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
	U_CSI = sv_temp[m].U;
	if (I_CDH > 0)
	{
		t_CDH = t_CSI + PI2 / sv[m].l_dot / 2.0*(double)(I_CDH);
	}
	else
	{
		sv[m].TIMA = 1;
		if (sv[m].coe_osc.l > PI)
		{
			sv[m].Item8 = 0.0;
			sv[m].Item10 = 1.0;
		}
		else
		{
			sv[m].Item8 = PI;
			sv[m].Item10 = 0.0;
		}
		
		pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
		du = sv_temp[m].U - U_CSI;
		if (K_orp <= 0 && du <= PI/6.0)
		{
			if (sv[m].coe_osc.l > PI)
			{
				sv[m].Item8 = PI;
				sv[m].Item10 = 1.0;
			}
			else
			{
				sv[m].Item8 = 0.0;
				sv[m].Item10 = 1.0;
			}
			K_orp = 1;
			pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
		}
		sv[m].Item10 = 0.0;
		t_CDH = sv_temp[m].TE;
	}
	sv[m].ENTRY = 0;
	sv[m].TIMA = 0;
	sv[m].TE = t_CDH;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
	//Bring passive vehicle to phase match
	sv[p].TIMA = 6;
	pmmlaeg.CALL(Header, sv[p], sv_temp[p]);
	DH_G = sv_temp[p].R - sv_temp[m].R;
	R_CDH = R_CDH + DH_G - DH;
	if (abs(DH_G - DH) <= 1.0)
	{
		goto RTCC_LLWP_HMALIT_8_1;
	}
	dv_CSI = sqrt(2.0*mu / (R_apo*(1.0 + R_apo / R_CDH))) - V_H;
	goto RTCC_LLWP_HMALIT_5_2;
RTCC_LLWP_HMALIT_8_1:

	VECTOR3 R_m_b, V_m_b, R_p, V_p;
	OrbMech::GIMKIC(sv_temp[m].coe_osc, mu, R_m_b, V_m_b);
	OrbMech::GIMKIC(sv_temp[p].coe_osc, mu, R_p, V_p);

	V_m_apo = OrbMech::CoellipticDV(R_m_b, R_p, V_p, mu);
	dv_CDH = length(V_m_apo - V_m_b);

	/*double a_a, V_a, l_dot_a, V_R_a, gamma_a, V_H_a, DV_R, DV_H, Pitch, Yaw, V_H_b;
	a_a = sv_temp[p].coe_osc.a - DH;
	V_a = sqrt(mu*(2.0 / sv_temp[m].R - 1.0 / a_a));
	l_dot_a = sqrt(mu / pow(a_a, 3));
	p_p = sv_temp[p].coe_osc.a*(1.0 - sv_temp[p].coe_osc.e*sv_temp[p].coe_osc.e);
	R_p_dot = sqrt(mu / p_p)*(sv_temp[p].coe_osc.e*cos(sv_temp[p].coe_osc.g)*sin(sv_temp[p].U) - sv_temp[p].coe_osc.e*sin(sv_temp[p].coe_osc.g)*cos(sv_temp[p].U));
	V_R_a = R_p_dot * l_dot_a / sv_temp[p].l_dot;
	gamma_a = asin(V_R_a / V_a);
	gamma_b = atan((V_R_b / length(V_m)) / sqrt(1.0 - pow(R_dot_apo / length(V_m), 2)));
	V_H_b = V_b * cos(gamma_b);
	V_H_a = V_a * cos(gamma_a);
	DV_H = V_H_a - V_H_b;
	DV_R = R_dot_apo - V_R_a;
	dv_CDH = sqrt(DV_H*DV_H + DV_R * DV_R);
	OrbMech::GIMKIC(sv_temp[m].coe_osc, mu, R_m, V_m);
	PCMVMR(R_m, V_m, R_m, V_m_apo, DV_H, DV_R, 0.0, -1, V_m_apo, Pitch, Yaw);*/
	//Initialize
	sv[m].coe_osc = OrbMech::GIMIKC(R_m_b, V_m_apo, mu);
	sv[m].ENTRY = 0;
	sv[m].TIMA = 0;
	sv[m].TS = sv[m].TE = t_CDH;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
}

void RTCC::PMMENS(VECTOR3 R, VECTOR3 V, double dt_pf, double y_s, double theta_PF, double h_bo, double V_H, double V_R, double GMT_LO, VECTOR3 R_LS, VECTOR3 &R_BO, VECTOR3 &V_BO, double &GMT_BO)
{
	VECTOR3 H, K, J, U_RLS;
	double gamma_BO, psi, delta, Gamma, xi, phi, dv_pc, v_BO;

	v_BO = sqrt(V_H*V_H + V_R * V_R);
	gamma_BO = asin(V_R / V_H);

	ELVCNV(R_LS, GMT_LO, 1, RTCC_COORDINATES_MCT, RTCC_COORDINATES_MCI, R_LS);

	U_RLS = unit(R_LS);
	H = unit(crossp(R, V));
	K = unit(crossp(H, U_RLS));
	J = unit(crossp(K, H));

	psi = asin(dotp(H, U_RLS));
	delta = asin(sin(psi) / cos(theta_PF));
	Gamma = asin(sin(theta_PF) / cos(psi));
	xi = abs(delta) - y_s;

	if (y_s <= 0)
	{
		dv_pc = 2.0*v_BO*sin(delta / 2.0);
	}
	else if (xi <= 0)
	{
		dv_pc = 0.0;
		Gamma = acos(cos(theta_PF) / cos(psi));
	}
	else
	{
		double acos_term = acos(cos(theta_PF) / cos(y_s));
		delta = abs(asin(sin(psi) / cos(acos_term))) - y_s;
		Gamma = acos(abs(sqrt(1.0 - pow(sin(acos_term) / cos(psi), 2))));
		dv_pc = 2.0*v_BO*sin(delta / 2.0);
	}

	phi = psi / abs(psi)*delta;

	MATRIX3 JKH1, JKH2;

	JKH1 = mul(_M(J.x, K.x, H.x, J.y, K.y, H.y, J.z, K.z, H.z), _M(cos(Gamma), -sin(Gamma), 0, sin(Gamma), cos(Gamma), 0, 0, 0, 1));
	if (xi > 0)
	{
		JKH1 = mul(JKH1, OrbMech::_MRy(phi));
	}

	JKH2 = mul(JKH1, OrbMech::_MRz(gamma_BO));
	R_BO = _V(JKH1.m11, JKH1.m21, JKH1.m31)*(length(R_LS) + h_bo);
	V_BO = _V(JKH2.m12, JKH2.m22, JKH2.m32)*v_BO;
	GMT_BO = GMT_LO + dt_pf;
}

//Lunar Launch Targeting from 68-FM-67
void RTCC::PMMLTR(AEGBlock sv_CSM, double T_LO, double V_H, double V_R, double h_BO, double t_PF, double P_FA, double Y_S, double r_LS, double lat_LS, double lng_LS, double &deltaw0, double &DR, double &deltaw, double &Yd, double &AZP)
{
	AEGDataBlock sv_temp, sv_L;
	VECTOR3 R_LS_sg, R_LSV, R_C, V_C, H_C, r_C, v_C, h_C, r_LSV, Q, c, R_LSP, r_LSP, R_BO, V_BO;
	double YSS, GMT_BO, DH, CXX, CYY, TRS, TS, phi_star, theta, DN, h_CA, h_CA_dot;
	int N, M;

	N = 1;
	M = 0;
	YSS = Y_S;
	Y_S = 0.0;

	sv_CSM.Data.TIMA = 0;
	sv_CSM.Data.TE = T_LO;
	pmmlaeg.CALL(sv_CSM.Header, sv_CSM.Data, sv_temp);
	if (N > 0)
	{
		goto RTCC_PMMLTR_3;
	}
	R_LS_sg = OrbMech::r_from_latlong(lat_LS, lng_LS, r_LS);
RTCC_PMMLTR_1:
	ELVCNV(R_LS_sg, T_LO, 1, RTCC_COORDINATES_MCT, RTCC_COORDINATES_MCI, R_LSV);
	OrbMech::GIMKIC(sv_temp.coe_osc, OrbMech::mu_Moon, R_C, V_C);
	r_C = unit(R_C);
	v_C = unit(V_C);
	H_C = crossp(r_C, v_C);
	h_C = unit(H_C);
RTCC_PMMLTR_2:
	r_LSV = unit(R_LSV);
	Q = crossp(r_LSV, h_C);
	c = unit(Q);
	R_LSP = crossp(h_C, c);
	r_LSP = unit(R_LSP);
	//Compute angle between landing site vector and its projection in CSM orbit plane
	theta = acos(dotp(r_LSP, r_LSV));
	if (M > 0)
	{
		Yd = r_LS * tan(theta);
		goto RTCC_PMMLTR_4;
	}
	DR = r_LS * tan(theta);
	M = 1;
RTCC_PMMLTR_3:
	sv_CSM.Data.TE = T_LO + t_PF;
	pmmlaeg.CALL(sv_CSM.Header, sv_CSM.Data, sv_temp);
	//Work on this
	h_CA = sv_temp.coe_mean.h;
	h_CA_dot = 0.0;
	PMMENS(R_C, V_C, t_PF, Y_S, P_FA, h_BO, V_H, V_R, T_LO, R_LS_sg, R_BO, V_BO, GMT_BO);
	sv_L.coe_osc = OrbMech::GIMIKC(R_BO, V_BO, OrbMech::mu_Moon);
	sv_L.TS = sv_L.TE = GMT_BO;
	pmmlaeg.CALL(sv_CSM.Header, sv_L, sv_temp);
	if (N <= 0)
	{
		R_LSV = R_BO;
		goto RTCC_PMMLTR_2;
	}
RTCC_PMMLTR_4:
	DH = sv_L.coe_mean.h - sv_CSM.Data.coe_mean.h;
	CXX = cos(sv_L.coe_mean.i)*cos(sv_CSM.Data.coe_mean.i) + sin(sv_L.coe_mean.i)*sin(sv_CSM.Data.coe_mean.i)*cos(DH);
	CYY = sqrt(1.0 - CXX * CXX);
	if (N > 0)
	{
		Y_S = YSS;
		N = 0;
		goto RTCC_PMMLTR_1;
	}
	deltaw = atan2(CXX, CYY);
	DN = h_CA;
	if (DN <= 0)
	{
		DN = DN + PI2;
	}
	DN = DN - PI;
	if (DN < 0)
	{
		DN = DN + PI;
	}
	TRS = DN / (OrbMech::w_Moon + h_CA_dot) + T_LO + t_PF;
	//TBD: TRS is in days and gets rounded to full second?
	TS = T_LO - TRS;
	phi_star = lng_LS - OrbMech::w_Moon*TS;
	AZP = atan2(-cos(sv_L.coe_mean.i)*cos(lat_LS) + sin(sv_CSM.Data.coe_mean.i)*sin(phi_star)*sin(lat_LS), sin(sv_CSM.Data.coe_mean.i)*cos(phi_star));
	if (AZP < 0)
	{
		AZP += PI2;
	}
}

void RTCC::LunarLaunchWindowProcessor(const LunarLiftoffTimeOpt &opt)
{
	//0 = CSM, 1 = LM
	AEGHeader Header;
	AEGDataBlock sv[2], sv_temp[2], sv_xx, elem_TPI;
	VECTOR3 R_LS, R_LS_sg, r_LS, R1, V1, r1, Q, h1, H1, v1, C, R_P, r_P, H_D, h_D, R_BO, V_BO, R_TPI, V_TPI, R_XX, V_XX, R_TI_C, V_TI_C, R_TI_T, V_TI_T;
	double mu, T_TPI, T_hole, t_ca, theta_CA, theta, dt, S1, S2, t_xx, T_INS, dt_B, t_CSI, t_D, U_R, t_R, r_apo, r_peri, H_S_apo, DH_MIN, DH_CRIT;
	double C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, P_mm, DH_u, theta_w, t_LOR, t_H, dv_CSI, dv_CDH, dv_TPI, dv_TPF, t_CDH, theta_TPI, dt_NSR, P_O, P_Q;
	double dt_max, B1, B2, B3, B4, DP, P_mm_apo, r_TPM, theta_s, f_TPI, C_R[3], C_V[3], DH_MAX, ddH_c, DH[10], DH_apo[10];
	int K, m, p, I_SRCH, L_DH, I, J, K_T, i, I_STOP2, M_stop, IPHAS;
	int N_arr[10];
	double T_TPI_arr[10], T_R_arr[10], DV_CSI_arr[10], DV_CDH_arr[10], T_CDH_arr[10], DV_TPI_arr[10], DV_TPF_arr[10], DV_T_arr[10], T_LO_arr[10], T_CSI_arr[10], T_INS_arr[10];
	TwoImpulseOpt tiopt;
	TwoImpulseResuls tires;

	//Reset number of solutions
	PZLRPT.plans = 0;

	tiopt.mode = 5;
	tiopt.sv_A.RBI = BODY_MOON;
	tiopt.sv_P.RBI = BODY_MOON;

	mu = OrbMech::mu_Moon;
	T_hole = opt.t_hole;
	m = opt.M - 1;
	p = opt.P - 1;
	dt_B = opt.DT_B;
	I_SRCH = opt.I_SRCH;
	L_DH = opt.L_DH;
	for (int ii = 0;ii < L_DH;ii++)
	{
		DH[ii] = opt.DH[ii];
	}

	Header.AEGInd = 1;
	Header.ErrorInd = 0;
	Header.NumBlocks = 1;
	sv[0].coe_osc = OrbMech::GIMIKC(opt.sv_CSM.R, opt.sv_CSM.V, mu);
	sv[0].Item7 =SystemParameters.GMTBASE;
	sv[0].TE = sv[0].TS = OrbMech::GETfromMJD(opt.sv_CSM.MJD,SystemParameters.GMTBASE);
	//Initialize
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);

	if (opt.I_TPI > 1)
	{
		T_TPI = opt.t_hole;
		T_hole = T_TPI - 1.5*PI2 / sv[0].l_dot;
		if (opt.I_CDH > 0)
		{
			T_hole = T_hole - PI2 / sv[0].l_dot*(double)(opt.I_CDH - 1);
		}
		if (opt.I_BURN == 1)
		{
			T_hole = T_hole - opt.DT_B;
		}
	}
	//Move to T_hole
	sv[0].TE = T_hole;
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	//Initialize at T_hole
	sv[0] = sv_temp[0];
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	//Move to LS longitude
	PMMTLC(Header, sv[0], sv_temp[0], opt.lng, K, 0);
	t_xx = sv_temp[0].TE;
	sv_xx = sv_temp[0];
	//Initialize
	pmmlaeg.CALL(Header, sv_xx, sv_temp[0]);
	OrbMech::GIMKIC(sv_xx.coe_osc, mu, R_XX, V_XX);
	t_ca = t_xx;
	R_LS_sg = OrbMech::r_from_latlong(opt.lat, opt.lng, opt.R_LLS);
RTCC_PMMLWP_2:
	ELVCNV(R_LS_sg, t_ca, 1, RTCC_COORDINATES_MCT, RTCC_COORDINATES_MCI, R_LS);
	r_LS = unit(R_LS);
	OrbMech::GIMKIC(sv_temp[0].coe_osc, mu, R1, V1);
	r1 = unit(R1);
	v1 = unit(V1);
	H1 = crossp(r1, v1);
	h1 = unit(H1);
	Q = crossp(r_LS, h1);
	C = unit(Q);
	R_P = crossp(h1, C);
	r_P = unit(R_P);
	theta_CA = acos(dotp(r_P, r1));
	H_D = crossp(r1, r_P);
	h_D = unit(H_D);
	S1 = h_D.z / abs(h_D.z);
	S2 = h1.z / abs(h1.z);
	theta = theta_CA * S1 / S2;
	dt = theta / sv[0].l_dot;
	if (abs(dt) > 0.1)
	{
		t_ca = t_ca + dt;
		sv[0].TE = t_ca;
		pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
		goto RTCC_PMMLWP_2;
	}
	//sv[0].TE = t_xx;
	//pmmlaeg.CALL(Header, sv[0], sv_temp);
	PMMENS(R_XX, V_XX, opt.dt_1, opt.Y_S, opt.theta_1, opt.h_BO, opt.v_LH, opt.v_LV, t_xx, R_LS_sg, R_BO, V_BO, T_INS);
	
	sv[1].coe_osc = OrbMech::GIMIKC(R_BO, V_BO, mu);
	sv[1].TE = sv[1].TS = T_INS;
	//Initialize
	pmmlaeg.CALL(Header, sv[1], sv_temp[1]);
	if (opt.I_BURN <= 1)
	{
		if (opt.I_BURN < 1)
		{
			dt_B = 1.0 / sv[m].l_dot*acos(sv[m].coe_mean.e - sv[m].coe_mean.e*sqrt(1.0 - pow(sv[m].coe_mean.e, 2)));
		}
		t_CSI = T_INS + dt_B;
	}
	else
	{
		sv[1].TIMA = 1;
		sv[1].Item8 = PI;
		pmmlaeg.CALL(Header, sv[1], sv_temp[1]);
		t_CSI = sv_temp[1].TE;
		if (opt.M <= 1)
		{
			t_CSI = t_CSI + opt.t_BASE;
		}
		dt_B = t_CSI - T_INS;
	}
	sv[0].TIMA = sv[1].TIMA = 0;
	sv[0].TE = sv[1].TE = t_CSI;
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	pmmlaeg.CALL(Header, sv[1], sv_temp[1]);
	int MM, I_LOOP, I_SELECT, I_STOP1, J_MOV, I_LAST, I_SAVE, I_CURVE;

	MM = 2;
	I_LOOP = 1;
	I_SELECT = 1;
	I_STOP1 = 1;
	J_MOV = 0;
	I_LAST = 0;
	I_SAVE = 1;
	I_CURVE = 1;

	if (opt.M < 2)
	{
		OrbMech::GIMKIC(sv_temp[0].coe_osc, mu, R_TPI, V_TPI);
		elem_TPI = sv_temp[0];
		I_SRCH = 0;
		goto RTCC_PMMLLWP_6_2;
	}
	t_D = t_CSI + PI2 / sv[1].l_dot / 2.0;
	if (opt.I_CDH > 0)
	{
		t_D = t_D + PI2 / sv[1].l_dot / 2.0*(double)(opt.I_CDH - 1);
	}
	sv[0].TE = t_D;
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	sv[0] = sv_temp[0];
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	if (opt.I_TPI > 1)
	{
		sv[0].TE = T_TPI;
		pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	}
	else
	{
		PMMTLC(Header, sv[0], sv_temp[0], opt.lng_TPI, K, 0);
		T_TPI = sv_temp[0].TE;
	}
	OrbMech::GIMKIC(sv_temp[0].coe_osc, mu, R_TPI, V_TPI);
	elem_TPI = sv_temp[0];
	U_R = sv_temp[0].U + opt.theta_F;
	sv[0].Item10 = 0.0;
	if (U_R >= PI2)
	{
		U_R -= PI2;
		sv[0].Item10 += 1.0;
	}
	sv[0].Item10 += trunc((sv_temp[0].TE - sv[0].TS) / (PI2 / sv[0].l_dot));
	if (sv[0].U > sv_temp[0].U)
	{
		sv[0].Item10 += 1.0;
	}
	sv[0].TIMA = 2;
	sv[0].Item8 = U_R;
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	t_R = sv_temp[0].TE;
	LLWP_PERHAP(Header, sv[p], r_apo, r_peri);
	H_S_apo = r_peri - opt.R_LLS;
	DH_MIN = H_S_apo - opt.H_S;
	DH_CRIT = sv_temp[0].R * (1.0 - 1.0 / cos(opt.E));
	K_T = 0;
	I = 1;
	J = 1;
	if (I_SRCH > 0)
	{
		DH_apo[J - 1] = DH_MIN;
		J = 2;
	}

RTCC_PMMLLWP_4:
	if (opt.DH[I - 1] >= DH_MIN || opt.DH[I-1]<=DH_CRIT)
	{
		if (I < L_DH)
		{
			goto RTCC_PMMLLWP_4;
		}
		if (K_T <= 0)
		{
			if (I_SRCH <= 0)
			{
				//error
				return;
			}
			L_DH = J;
			DH_apo[J - 1] = opt.DH_SRCH;
		}
		else
		{
			if (I_SRCH <= 0)
			{
				L_DH = J;
			}
			else
			{
				L_DH = J + 1;
				DH_apo[J - 1] = opt.DH_SRCH;
			}
		}
	}
	else
	{
		DH_apo[J - 1] = opt.DH[I - 1];
		if (I < L_DH)
		{
			K_T = 1;
			I++;
			J++;
			goto RTCC_PMMLLWP_4;
		}
		if (I_SRCH <= 0)
		{
			L_DH = J;
		}
		else
		{
			L_DH = J + 1;
			DH_apo[J - 1] = opt.DH_SRCH;
		}
	}
	i = 1;
	I_STOP2 = L_DH;
	while (i < L_DH)
	{
		DH[i - 1] = DH_apo[i - 1];
		i++;
	}
	P_mm = PI2 / sv[m].l_dot;
RTCC_PMMLLWP_6_2:
	C1 = sv_temp[p].R;
	C2 = sv_temp[m].R;
	C3 = PI2 / sv[p].l_dot;
	C4 = sv_temp[p].coe_osc.a;
	C5 = (opt.dt_1*sv[p].l_dot - opt.theta_1) / sv[p].l_dot;
	C6 = (C3 - PI2 / sv[m].l_dot)*dt_B / C3;
	DH_u = DH[I_SELECT - 1];
RTCC_PMMLLWP_7_7:
	IPHAS = 0;
	theta_w = PI05 - opt.E - asin((elem_TPI.R - DH_u)*cos(opt.E) / elem_TPI.R);
	do
	{
		theta_s = theta_w;
		IPHAS++;
		f_TPI = elem_TPI.U - elem_TPI.coe_osc.g - theta_w;
		r_TPM = elem_TPI.coe_osc.a*(1.0 - elem_TPI.coe_osc.e*elem_TPI.coe_osc.e) / (1.0 + elem_TPI.coe_osc.e*cos(f_TPI));
		theta_w = PI05 - opt.E - asin((elem_TPI.R - DH_u)*cos(opt.E) / elem_TPI.R);
	} while (abs(theta_w - theta_s) > 0.01*RAD && IPHAS < 10);

	C7 = (C2 + C1 - DH_u) / 2.0;
	C8 = 1.0 / C7 * sqrt(mu / C7);
	C9 = PI / C8;
	C10 = (C3 - 2.0*C9)*C9 / C3;
	C11 = theta_w / sv[p].l_dot;
	C12 = C4 - DH_u;
	C13 = 1.0 / C12 * sqrt(mu / C12);
	C14 = PI2 / C13;
	C15 = (C3 - C14)*C14 / (4.0*C3);
	t_LOR = t_xx - C5 + abs(C6) + abs(C10) + C15 - C11;
RTCC_PMMLLWP_8_8:
	sv[0] = sv_xx;
	sv[0].TE = t_LOR;
	sv[0].TIMA = 0;
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	OrbMech::GIMKIC(sv_temp[0].coe_osc, mu, R1, V1);
	PMMENS(R1, V1, opt.dt_1, opt.Y_S, opt.theta_1, opt.h_BO, opt.v_LH, opt.v_LV, t_LOR, R_LS_sg, R_BO, V_BO, T_INS);

	sv[1].coe_osc = OrbMech::GIMIKC(R_BO, V_BO, mu);
	sv[1].TE = sv[1].TS = T_INS;
	sv[1].ENTRY = 0;
	sv[1].TIMA = 0;
	//Initialize
	pmmlaeg.CALL(Header, sv[1], sv_temp[1]);

	if (opt.M <= 1)
	{
		t_H = t_CSI + PI2 / sv[m].l_dot / 2.0;
		if (opt.I_CDH > 0)
		{
			t_H = t_H + PI2 / sv[m].l_dot / 2.0*(double)(opt.I_CDH - 1);
		}
		if (opt.I_TPI > 1)
		{
			sv[p].TE = T_TPI;
			pmmlaeg.CALL(Header, sv[p], sv_temp[p]);
		}
		else
		{
			//Advance to t_H
			sv[p].TE = t_H;
			pmmlaeg.CALL(Header, sv[p], sv_temp[p]);
			//Initialize at t_H
			sv[p] = sv_temp[p];
			pmmlaeg.CALL(Header, sv[p], sv_temp[p]);
			//Find time of arrival at TPI
			PMMTLC(Header, sv[p], sv_temp[p], opt.lng_TPI, K, 0);
			T_TPI = sv_temp[p].TE;
		}
		OrbMech::GIMKIC(sv_temp[p].coe_osc, mu, R_TPI, V_TPI);
		elem_TPI = sv_temp[p];
		//Find LM time of arrival at U_R
		U_R = sv_temp[p].U + opt.theta_F;
		sv[p].Item10 = 0.0;
		if (U_R >= PI2)
		{
			U_R -= PI2;
			sv[p].Item10 += 1.0;
		}
		sv[p].Item10 += trunc((sv_temp[p].TE - sv[p].TS) / (PI2 / sv[p].l_dot));
		if (sv[p].U > sv_temp[p].U)
		{
			sv[p].Item10 += 1.0;
		}
		sv[p].TIMA = 2;
		sv[p].Item8 = U_R;
		pmmlaeg.CALL(Header, sv[p], sv_temp[p]);
		t_R = sv_temp[p].TE;

		IPHAS = 0;
		theta_w = PI05 - opt.E - asin((length(R_TPI) - DH_u)*cos(opt.E) / length(R_TPI));
		do
		{
			theta_s = theta_w;
			IPHAS++;
			f_TPI = elem_TPI.U - elem_TPI.coe_osc.g - theta_w;
			r_TPM = elem_TPI.coe_osc.a*(1.0 - elem_TPI.coe_osc.e*elem_TPI.coe_osc.e) / (1.0 + elem_TPI.coe_osc.e*cos(f_TPI));
			theta_w = PI05 - opt.E - asin((elem_TPI.R - DH_u)*cos(opt.E) / elem_TPI.R);
		} while (abs(theta_w - theta_s) > 0.01*RAD && IPHAS < 10);

	}
	if (DH_u == 0)
	{
		theta_w += opt.dTheta_OFF;
	}
	//Save output quantities
	N_arr[I_LOOP - 1] = 2;
	T_TPI_arr[I_LOOP - 1] = T_TPI;
	T_R_arr[I_LOOP - 1] = t_R;
	//Move both vehicles to time of insertion
	sv[0].TIMA = sv[1].TIMA = 0;
	sv[0].TE = sv[1].TE  = T_INS;
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	pmmlaeg.CALL(Header, sv[1], sv_temp[1]);
	theta = OrbMech::THETR(sv[p].U, sv[m].U, sv[p].coe_osc.i, sv[m].coe_osc.i, sv[p].coe_osc.h, sv[m].coe_osc.h);
	//TBD: Adjust mean anomaly
	if (opt.I_BURN <= 1)
	{
		if (opt.I_BURN < 1)
		{
			dt_B = 1.0 / sv[m].l_dot*acos(sv[m].coe_mean.e - sv[m].coe_mean.e*sqrt(1.0 - pow(sv[m].coe_mean.e, 2)));
		}
		t_CSI = T_INS + dt_B;
	}
	else
	{
		sv[1].TIMA = 1;
		sv[1].Item8 = PI;
		sv[1].Item10 = 0.0;
		pmmlaeg.CALL(Header, sv[1], sv_temp[1]);
		t_CSI = sv_temp[1].TE;
		if (opt.M <= 1)
		{
			t_CSI = t_CSI + opt.t_BASE;
		}
	}
	//Advance both to CSI
	sv[m].TIMA = sv[p].TIMA = 0;
	sv[m].TE = sv[p].TE = t_CSI;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
	pmmlaeg.CALL(Header, sv[p], sv_temp[p]);

	LLWP_HMALIT(Header, sv, sv_temp, opt.M, opt.P, opt.I_CDH, DH_u, dv_CSI, dv_CDH, t_CDH);
	//Advance both vehicles to TPI maneuver point
	sv[m].TE = T_TPI;
	sv[m].TIMA = 0;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
	sv[p].TE = T_TPI;
	sv[p].TIMA = 0;
	pmmlaeg.CALL(Header, sv[p], sv_temp[p]);
	//Phase angle at TPI
	theta_TPI = OrbMech::THETR(sv_temp[p].U, sv_temp[m].U, sv_temp[p].coe_osc.i, sv_temp[m].coe_osc.i, sv_temp[p].coe_osc.h, sv_temp[m].coe_osc.h);
	if (theta_TPI > PI)
	{
		theta_TPI -= PI2;
	}
	else if (theta_TPI < -PI)
	{
		theta_TPI += PI2;
	}

	dt_NSR = (theta_TPI - theta_w) / sv[m].l_dot;
	if (abs(dt_NSR) > 0.1)
	{
		if (opt.M > 1)
		{
			dt_NSR = -dt_NSR;
		}
		t_LOR = t_LOR + dt_NSR;
		goto RTCC_PMMLLWP_8_8;
	}
	P_O = PI2 / sv[p].l_dot;
	P_Q = PI2 / sv[m].l_dot;
	DV_CSI_arr[I_SELECT - 1] = dv_CSI;
	DV_CDH_arr[I_SELECT - 1] = dv_CDH;
	T_CDH_arr[I_LOOP - 1] = t_CDH;
	dv_TPI = 0.0;
	dv_TPF = 0.0;
	if (abs(DH_u) != 0.0)
	{
		//Call two impulse
		tiopt.T1 = T_TPI;
		tiopt.T2 = t_R;
		OrbMech::GIMKIC(sv_temp[m].coe_osc, mu, R_TI_C, V_TI_C);
		OrbMech::GIMKIC(sv_temp[p].coe_osc, mu, R_TI_T, V_TI_T);
		tiopt.sv_A.R = R_TI_C;
		tiopt.sv_A.V = V_TI_C;
		tiopt.sv_A.GMT = sv_temp[m].TE;
		tiopt.sv_P.R = R_TI_T;
		tiopt.sv_P.V = V_TI_T;
		tiopt.sv_P.GMT = sv_temp[p].TE;
		PMSTICN(tiopt, tires);
		dv_TPI = length(tires.dV);
		dv_TPF = length(tires.dV2);
	}
	DV_TPI_arr[I_SELECT - 1] = dv_TPI;
	DV_TPF_arr[I_SELECT - 1] = dv_TPF;
	DV_T_arr[I_SELECT - 1] = abs(dv_CSI) + dv_CDH + dv_TPI + dv_TPF;
	if (J_MOV > 0)
	{
		goto RTCC_PMMLLWP_20_16;
	}
	T_LO_arr[I_LOOP - 1] = t_LOR;
	T_INS_arr[I_LOOP - 1] = T_INS;
	T_CSI_arr[I_LOOP - 1] = t_CSI;
	dt_max = t_R - t_LOR;
	B1 = T_CSI_arr[I_LOOP - 1];
	B2 = T_CDH_arr[I_LOOP - 1];
	B3 = T_TPI_arr[I_LOOP - 1];
	B4 = T_R_arr[I_LOOP - 1];

	if (I_LAST > 0)
	{
		goto RTCC_PMMLLWP_18_17;
	}
	if (I_SELECT <= I_STOP1)
	{
		goto RTCC_PMMLLWP_17_18;
	}
	if (I_SELECT < I_STOP2 - 1)
	{
		goto RTCC_PMMLLWP_19_20;
	}
	else if (I_SELECT > I_STOP2 - 1)
	{
		goto RTCC_PMMLLWP_22_19;
	}
	if (I_SRCH > 0)
	{
		goto RTCC_PMMLLWP_20_22;
	}
	else
	{
		goto RTCC_PMMLLWP_19_20;
	}
RTCC_PMMLLWP_17_18:
	if (I_SRCH <= 0)
	{
		goto RTCC_PMMLLWP_19_20;
	}
	DP = P_O - P_mm;
	P_mm_apo = P_mm;
	M_stop = MM + (int)((opt.t_max - dt_max) / P_mm_apo);
RTCC_PMMLLWP_17_24:
	T_CSI_arr[I_LOOP - 1] = B1 + P_mm_apo * (double)(M_stop - 2);
	T_CDH_arr[I_LOOP - 1] = B2 + P_mm_apo * (double)(M_stop - 2);
	goto RTCC_PMMLLWP_18_25;
RTCC_PMMLLWP_18_17:
	if (DH_u > 0)
	{
		goto RTCC_PMMLLWP_22_19;
	}
	P_mm_apo = P_Q;
	DP = -(P_O - P_mm_apo);
	M_stop = MM + (int)((opt.t_max - dt_max) / P_mm_apo);
RTCC_PMMLLWP_18_26:
	T_CSI_arr[I_LOOP - 1] = B1 + DP * (double)(M_stop - 2);
	T_CDH_arr[I_LOOP - 1] = B2 + DP * (double)(M_stop - 2);
RTCC_PMMLLWP_18_25:
	T_LO_arr[I_LOOP - 1] = t_LOR + DP * (double)(M_stop - 2);
	T_TPI_arr[I_LOOP - 1] = B1 + P_mm_apo * (double)(M_stop - 2);
	T_R_arr[I_LOOP - 1] = B4 + P_mm_apo * (double)(M_stop - 2);
	N_arr[I_LOOP - 1] = M_stop;
	if (MM - M_stop != 0)
	{
		M_stop = 2;
		I_LOOP++;
		if (I_SELECT <= 1)
		{
			goto RTCC_PMMLLWP_17_24;
		}
		goto RTCC_PMMLLWP_18_26;
	}
RTCC_PMMLLWP_19_20:
	MM = 2;
	if (I_LAST > 0)
	{
		goto RTCC_PMMLLWP_22_19;
	}
	I_LOOP++;
	I_SELECT++;
	DH_u = DH[I_SELECT - 1];
	goto RTCC_PMMLLWP_7_7;
RTCC_PMMLLWP_20_22:
	J_MOV = 1;
	I_SELECT++;
	I_LOOP++;
	DH_u = DH[I_SELECT - 1];
	goto RTCC_PMMLLWP_7_7;
RTCC_PMMLLWP_20_16:
	C_R[I_CURVE - 1] = DH_u;
	C_V[I_CURVE - 1] = DV_T_arr[I_SELECT - 1];
	if (I_CURVE < opt.N_CURV)
	{
		I_CURVE++;
		DH_u = DH_u - opt.DH_SRCH;
		goto RTCC_PMMLLWP_7_7;
	}
	DH_MAX = LLWP_CURVE(C_V[0], C_V[1], C_V[2], C_R[0], C_R[1], C_R[2], opt.DV_MAX[m]);
	ddH_c = DH_MAX - DH_CRIT;
	DH[I_SELECT - 1] = DH_MAX;
	if (ddH_c < 0)
	{
		DH[I_SELECT - 1] = DH_CRIT;
	}
	DH_u = DH[I_SELECT - 1];
	I_LAST = 1;
	J_MOV = 0;
	goto RTCC_PMMLLWP_7_7;
RTCC_PMMLLWP_22_19:
	//Set up RET and RPT
	PZLRPT.plans = I_LOOP;
	PZLRPT.CSMSTAID = "";
	PZLRPT.CSM_GMTV = OrbMech::GETfromMJD(opt.sv_CSM.MJD,SystemParameters.GMTBASE);
	PZLRPT.CSM_GETV = GETfromGMT(PZLRPT.CSM_GMTV);
	if (opt.M == 2)
	{
		PZLRPT.ManVeh = "LM";
	}
	else
	{
		PZLRPT.ManVeh = "CSM";
	}
	PZLRPT.LSLat = opt.lat*DEG;
	PZLRPT.LSLng = opt.lng*DEG;
	PZLRPT.THT = GETfromGMT(opt.t_hole);
	PZLRPT.LMSTAID = "";
	PZLRPT.LM_GMTV = 0.0;
	PZLRPT.LM_GETV = 0.0;
	PZLRPT.DT_CSI = dt_B;
	PZLRPT.DV_MAX = opt.DV_MAX[m] / 0.3048;
	PZLRPT.WT = opt.theta_F*DEG;
	for (int i = 0;i < I_LOOP;i++)
	{
		PZLRPT.data[i].ID = i + 1;
		PZLRPT.data[i].N = N_arr[i];
		PZLRPT.data[i].DH = DH[i] / 1852.0;
		PZLRPT.data[i].GETLO = GETfromGMT(T_LO_arr[i]);
		PZLRPT.data[i].T_INS = GETfromGMT(T_INS_arr[i]);
		PZLRPT.data[i].T_CSI = GETfromGMT(T_CSI_arr[i]);
		PZLRPT.data[i].T_CDH = GETfromGMT(T_CDH_arr[i]);
		PZLRPT.data[i].T_TPI = GETfromGMT(T_TPI_arr[i]);
		PZLRPT.data[i].T_TPF = GETfromGMT(T_R_arr[i]);
		PZLRPT.data[i].DVCSI = DV_CSI_arr[i] / 0.3048;
		PZLRPT.data[i].DVCDH = DV_CDH_arr[i] / 0.3048;
		PZLRPT.data[i].DVTPI = DV_TPI_arr[i] / 0.3048;
		PZLRPT.data[i].DVTPF = DV_TPF_arr[i] / 0.3048;
		PZLRPT.data[i].DVT = DV_T_arr[i] / 0.3048;
	}
}

void RTCC::EntryUpdateCalc(SV sv0, double entryrange, bool highspeed, EntryResults *res)
{
	OBJHANDLE hEarth;
	VECTOR3 REI, VEI, UREI, R3, V3, R05G, V05G;
	double EntryInterface, RCON, dt2, MJD_EI, lambda, phi, GMT_l, vEI, t32, dt22, EMSAlt, t2;

	hEarth = oapiGetObjectByName("Earth");

	EntryInterface = 400000.0 * 0.3048;
	RCON = OrbMech::R_Earth + EntryInterface;
	if (highspeed)
	{
		EMSAlt = 297431.0*0.3048;
	}
	else
	{
		EMSAlt = 284643.0*0.3048;
	}

	dt2 = OrbMech::time_radius_integ(SystemParameters.AGCEpoch, sv0.R, sv0.V, sv0.MJD, RCON, -1, sv0.gravref, hEarth, REI, VEI);
	MJD_EI = sv0.MJD + dt2 / 24.0 / 3600.0;
	t2 = (sv0.MJD - CalcGETBase()) * 24.0 * 3600.0 + dt2;	//EI time in seconds from launch

	UREI = unit(REI);
	vEI = length(VEI);
	res->ReA = asin(dotp(UREI, VEI) / vEI);

	t32 = OrbMech::time_radius(REI, VEI, RCON - 30480.0, -1, OrbMech::mu_Earth);
	OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, OrbMech::mu_Earth);
	dt22 = OrbMech::time_radius(R3, V3, length(R3) - (300000.0 * 0.3048 - EMSAlt), -1, OrbMech::mu_Earth);
	OrbMech::rv_from_r0v0(R3, V3, dt22, R05G, V05G, OrbMech::mu_Earth);

	res->RTGO = EntryCalculations::LNDING(REI, VEI, OrbMech::GETfromMJD(MJD_EI, GetGMTBase()), SystemParameters.MCLAMD, 0.3, 2, entryrange, lambda, phi, GMT_l);

	res->latitude = phi;
	res->longitude = lambda;

	res->VIO = length(V05G);
	res->GET400K = t2;
	res->GET05G = t2 + t32 + dt22;
}

double RTCC::CalculateTPITimes(SV sv0, int tpimode, double t_TPI_guess, double dt_TPI_sunrise)
{
	//tpimode: 0 = TPI on time, 1 = TPI at orbital midnight, 2 = TPI at X minutes before sunrise
	SV sv_TPI_guess;
	OBJHANDLE hSun = oapiGetObjectByName("Sun");
	double t_TPI;

	
	if (tpimode == 0)
	{
		t_TPI = t_TPI_guess;
	}
	else if (tpimode == 1)
	{
		sv_TPI_guess = coast(sv0, t_TPI_guess - OrbMech::GETfromMJD(sv0.MJD, CalcGETBase()));
		double ttoMidnight;
		ttoMidnight = OrbMech::sunrise(SystemParameters.MAT_J2000_BRCS, sv_TPI_guess.R, sv_TPI_guess.V, sv_TPI_guess.MJD, sv_TPI_guess.gravref, hSun, 1, 1, false);
		t_TPI = t_TPI_guess + ttoMidnight;
	}
	else
	{
		SV sv_sunrise_guess;
		double ttoSunrise;

		sv_TPI_guess = coast(sv0, t_TPI_guess - OrbMech::GETfromMJD(sv0.MJD, CalcGETBase()));
		sv_sunrise_guess = coast(sv_TPI_guess, dt_TPI_sunrise);
		ttoSunrise = OrbMech::sunrise(SystemParameters.MAT_J2000_BRCS, sv_sunrise_guess.R, sv_sunrise_guess.V, sv_sunrise_guess.MJD, sv_sunrise_guess.gravref, hSun, 1, 0, false);
		t_TPI = t_TPI_guess + ttoSunrise;
	}

	return t_TPI;
}

//Newton-Raphson iteration for DKI
bool PCMIT1(int &C, double Y, double &X, double &X0, double &Y0, double DX1 = 2.0, int C_max = 20)
{
	double dx;

	if (C == 0 || abs(Y - Y0) == 0.0)
	{
		dx = DX1;
	}
	else
	{
		double P = (Y - Y0) / (X - X0);
		dx = Y / P;
	}
	C++;
	Y0 = Y;
	X0 = X;
	X = X - dx;

	if (C > C_max)
	{
		return true;
	}
	return false;
}

//Regula-Falsi iterator for DKI
void PCMIT2(int &I_PASS, double Y, double Y_apo, double Y_err, double &dx, double x, double &x_min, double &x_max, double k)
{
	if (I_PASS > 0)
	{
		dx = sign(Y_err)*k*(x_max - x_min);
		if (I_PASS == 2)
		{
			I_PASS = 0;
			return;
		}
		I_PASS = 0;
	}
	else
	{
		dx = Y_err * dx / (Y - Y_apo);
	}
	if (dx <= 0)
	{
		x_max = x;
		if (x + dx <= x_min)
		{
			dx = 0.5*(x_min - x);
		}
	}
	else
	{
		x_min = x;
		if (x + dx >= x_max)
		{
			dx = 0.5*(x_max - x);
		}
	}
}

//Coelliptic for DKI
void RTCC::PCMCEM(AEGHeader &h, AEGDataBlock &sv_M, AEGDataBlock &sv_I, double mu)
{
	AEGDataBlock sv_temp;
	double P_I, P_M, dh, VS, Rdot_I, Rdot_M, SKIP, DU, phi, VCF2;

	sv_M.TIMA = 0;
	sv_M.TE = sv_M.TS;
	PMMAEGS(h, sv_M, sv_temp);
	sv_I.TIMA = 6;
	PMMAEGS(h, sv_I, sv_temp);

	dh = sv_temp.R - sv_M.R;
	P_I = sv_temp.coe_osc.a*(1.0 - sv_temp.coe_osc.e*sv_temp.coe_osc.e);
	Rdot_I = sqrt(mu / P_I)*(sv_temp.coe_osc.e*cos(sv_temp.coe_osc.g)*sin(sv_temp.U) - sv_temp.coe_osc.e*sin(sv_temp.coe_osc.g)*cos(sv_temp.U));

	sv_M.coe_osc.a = sv_temp.coe_osc.a - dh;
	sv_M.coe_osc.e = sv_temp.coe_osc.a*sv_temp.coe_osc.e / sv_M.coe_osc.a;
	P_M = sv_M.coe_osc.a*(1.0 - sv_M.coe_osc.e*sv_M.coe_osc.e);
	VS = mu * (2.0 / sv_M.R - 1.0 / sv_M.coe_osc.a);
	Rdot_M = sqrt(abs(VS - mu * P_M / pow(sv_M.R, 2)));
	if (Rdot_I < 0)
	{
		Rdot_M = -Rdot_M;
	}
	SKIP = sqrt(sv_M.R*sv_M.R*P_M / mu)*Rdot_M;
	phi = atan2(SKIP, P_M - sv_M.R);
	sv_M.coe_osc.g = atan2(sin(sv_M.U - phi), cos(sv_M.U - phi));
	if (sv_M.coe_osc.g < 0)
	{
		sv_M.coe_osc.g += PI2;
	}
	phi = sv_M.U - sv_M.coe_osc.g;
	VCF2 = sqrt((1.0 - sv_M.coe_osc.e) / (1.0 + sv_M.coe_osc.e));
	DU = 2.0*atan2(VCF2*sin(phi / 2.0), cos(phi / 2.0));
	sv_M.coe_osc.l = DU - sv_M.coe_osc.e*sin(DU);
	if (sv_M.coe_osc.l < 0)
	{
		sv_M.coe_osc.l += PI2;
	}
	sv_M.ENTRY = 0;
}

void RTCC::PMMPHL(DKICommon &DKI, AEGHeader aegh, AEGDataBlock sv_I, double TXX, double &TTPI, double &TTPF)
{
	AEGDataBlock sv_temp;
	double TPIMIN, T, UWANT, DELTU, DELD, T_c, T_c_apo, T_N;
	//1 = want night, 2 = want day
	int ITP;
	int ICOU, ERR, ICOU2;

	TPIMIN = TXX + DKI.DTSR;
	T = TXX;

	if (DKI.K46 == 1)
	{
		//Input TPI time
		T = TTPI;
	}
	else if (DKI.K46 == 2)
	{
		//Input TPF time
		T = TTPF;
		sv_I.TIMA = 0;
		sv_I.TE = T;
		PMMAEGS(aegh, sv_I, sv_temp);
		UWANT = sv_temp.U - DKI.WT;
		ICOU = 0;
		do
		{
			DELTU = UWANT - sv_temp.U;
			if (abs(DELTU) <= DKI.DOS)
			{
				break;
			}
			ICOU++;
			if (ICOU > 5)
			{
				break;
			}
			T = T + DELTU / (sv_I.l_dot + sv_I.g_dot);
			sv_I.TIMA = 0;
			sv_I.TE = T;
			PMMAEGS(aegh, sv_I, sv_temp);
		} while (ICOU <= 5);
	}
	else
	{
		if (DKI.K46 == 3 || DKI.K46 == 5)
		{
			ITP = 1;
		}
		else
		{
			ITP = 2;
		}

		DELD = DKI.TLIT;
		DELD = DELD * 60.0;
		ICOU2 = 0;
		while (ICOU2 < 10)
		{
			ICOU2++;
			//Update inactive vehicle to T
			sv_I.TIMA = 0;
			sv_I.TE = T;
			PMMAEGS(aegh, sv_I, sv_temp);

			//Get next two environment changes
			//Positive return value = upcoming environment is daylight, negative = darkness
			PMMDAN(aegh, sv_temp, 2, ERR, T_c, T_c_apo);
			if (ERR) return;

			if (ITP == 1)
			{
				//Want night
				if (T_c < 0.0)
				{
					T_N = abs(T_c);
				}
				else
				{
					T_N = abs(T_c_apo);
				}
			}
			else
			{
				//Want day
				if (T_c > 0)
				{
					T_N = T_c;
				}
				else
				{
					T_N = T_c_apo;
				}
			}
			if (T_N + DELD > T + PI2 / sv_I.l_dot)
			{
				//Found condition one orbit too late
				T = T - PI / sv_I.l_dot;
				continue;
			}
			T = T_N + DELD;
			
			if (DKI.K46 == 5 || DKI.K46 == 6)
			{
				//Find TPI time from TPF
				sv_I.TIMA = 0;
				sv_I.TE = T;
				PMMAEGS(aegh, sv_I, sv_temp);
				UWANT = sv_temp.U - DKI.WT;
				ICOU = 0;
				do
				{
					DELTU = UWANT - sv_temp.U;
					if (DELTU < -PI)
					{
						DELTU += PI2;
					}
					if (abs(DELTU) <= DKI.DOS)
					{
						break;
					}
					ICOU++;
					if (ICOU > 5)
					{
						break;
					}
					T = T + DELTU / (sv_I.l_dot + sv_I.g_dot);
					sv_I.TIMA = 0;
					sv_I.TE = T;
					PMMAEGS(aegh, sv_I, sv_temp);
				} while (ICOU <= 5);
			}

			if (T <= TPIMIN)
			{
				T = TXX + PI / sv_I.l_dot;
				continue;
			}
			TTPI = T;
			break;
		}
	}
}

void RTCC::PMMITL(DKICommon &DKI, AEGHeader aegh, AEGDataBlock *sv, int J)
{
	//Maneuver selection flag
	//1 = dv is input (NC, NH), 2 = coelliptic
	int s_man[4];
	//Primary vehicle update flag
	//1 = update to a specified t_n, 2 = update through DT, 3 = update through n_rev (period), 4 = update through n_rev (maneuver line)
	int s_update[4];
	//either t, dt or n_rev
	double updval[4];
	//Constraint-iteration control flag
	// = -n: phasing constraint at maneuver point i, vary DV of maneuver n
	// = 0: no constraints at maneuver point i.
	// = n (0 < n < 10) a height constraint must be satisfied at maneuver point i; vary the DV of maneuver n
	// = nm (10 < nm < 100) both phasing and height constraints must be satisfied. Phasing constraint with maneuver n, height constraint with maneuver m.
	int s_term[5];
	//Phasing and height constraints
	double dh[5], dtheta[5];
	AEGDataBlock sv_temp;

	int ml, M, I, i;
	double mu;

	M = DKI.MV - 1;
	I = 1 - M;

	//Maneuver line of fractional period
	if (DKI.IHALF)
	{
		ml = 3;
	}
	else
	{
		ml = 4;
	}

	if (aegh.AEGInd == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}

	for (i = 0;i < 5;i++)
	{
		dh[i] = dtheta[i] = 0.0;
	}

	double N1, N2, N3;

	//Set up maneuver IDs
	switch (J)
	{
	case 1:
		DKI.ID[0] = "C1"; DKI.ID[1] = "H"; DKI.ID[2] = "SR";
		break;
	case 2:
		DKI.ID[0] = "H"; DKI.ID[1] = "C1"; DKI.ID[2] = "SR";
		break;
	case 3:
		DKI.ID[0] = "C1"; DKI.ID[1] = "SR";
		break;
	case 4:
		DKI.ID[0] = "H"; DKI.ID[1] = "SR";
		break;
	case 5:
		DKI.ID[0] = "SR";
		break;
	case 10:
		DKI.ID[0] = "C1"; DKI.ID[1] = "H"; DKI.ID[2] = "CC"; DKI.ID[3] = "SR";
		break;
	}

	//Set up maneuver sequence options
	switch (J)
	{
	case 1:
		//NC1, NH, NSR
	case 2:
		//NH, NC1, NSR
		s_man[0] = 1; s_man[1] = 1; s_man[2] = 2;
		s_update[0] = ml; s_update[1] = ml;
		s_term[0] = 0; s_term[1] = 0;
		if (J == 1)
		{
			N1 = DKI.NC1; N2 = DKI.NH;
			s_term[2] = 2;
			s_term[3] = -1;
		}
		else
		{
			N1 = DKI.NH; N2 = DKI.NC1;
			s_term[2] = 1;
			s_term[3] = -2;
		}
		N3 = DKI.NSR;
		updval[1] = N3 - N2;
		updval[2] = DKI.MI - N3;
		DKI.NOM = 3;
		break;
	case 3:
		//NC1, NSR
	case 4:
		//NH, NSR
		s_man[0] = 1; s_man[1] = 2;
		s_update[0] = ml;
		s_term[0] = 0;
		if (J == 3)
		{
			N1 = DKI.NC1; N2 = DKI.NSR;
			s_term[1] = 0; s_term[2] = -1;
		}
		else
		{
			N1 = DKI.NH; N2 = DKI.NSR;
			s_term[1] = 1; s_term[2] = 0;
		}
		N3 = N2;
		updval[1] = DKI.MI - N3;
		DKI.NOM = 2;
		break;
	case 5:
		//NSR
		s_man[0] = 2;
		N1 = DKI.NSR; N2 = N1; N3 = N2;
		updval[0] = DKI.MI - N3;
		DKI.NOM = 1;
		break;
	case 10:
		//NC1, NH, NCC, NSR (Skylab)
		s_man[0] = 1; s_man[1] = 1; s_man[2] = 1; s_man[3] = 2;
		s_update[0] = ml; s_update[1] = ml; s_update[2] = 2;
		s_term[0] = 0; s_term[1] = 0; s_term[2] = 2; s_term[3] = 3; s_term[4] = -1;
		N1 = DKI.NC1; N2 = DKI.NH; N3 = DKI.NCC;
		updval[1] = N3 - N2;
		updval[2] = DKI.dt_NCC_NSR;
		dh[2] = DKI.DHNCC;
		DKI.NOM = 4;
		break;
	default:
		return;
	}

	//Set up maneuver line between first two maneuvers
	updval[0] = N2 - N1;

	//NSR conditions for all maneuver types
	dh[DKI.NOM - 1] = DKI.DHSR;

	//TPI conditions for all maneuver types
	dtheta[DKI.NOM] = DKI.COSR;
	s_update[DKI.NOM - 1] = 1;
	updval[DKI.NOM - 1] = DKI.TTPI;

	bool converged = false;

	//Take state vector to first maneuver
	sv[M].ENTRY = 0;
	sv[M].TIMA = 0;
	sv[M].TE = DKI.TNAI;
	PMMAEGS(aegh, sv[M], sv[M]);

	if (DKI.ANAI != N1)
	{
		sv[M].TIMA = 3;
		sv[M].Item8 = sv[M].U;
		sv[M].Item9 = DKI.TNAI;
		sv[M].Item10 = N1 - DKI.ANAI;
		PMMAEGS(aegh, sv[M], sv[M]);
	}

	sv[I].ENTRY = 0;
	sv[I].TIMA = 0;
	sv[I].TE = sv[M].TE;
	PMMAEGS(aegh, sv[I], sv[I]);
	//Call again to initialize at TNA
	PMMAEGS(aegh, sv[I], sv_temp);

	//Save initial state vector
	DKI.sv_before[0] = sv[M];

	//Iteration variables
	i = 0;
	int C_H = 0, C_P = 0, C_MAX = 20;
	int i_cp, i_ch, i_proc, i_pass;
	bool i_reverse;
	VECTOR3 dv[4];
	double DELVX, DELVY, DELVZ, Pitch, Yaw;
	double e_P, e_H, e_P0, e_PERR, x_min, x_max, e_H0, dv_H0, ddv, dv_P0;
	double dv_max = 1000.0*0.3048, K_DV = 0.025;

	for (i = 0;i < 4;i++)
	{
		dv[i] = _V(0, 0, 0);
	}
	i = 0;
	e_P0 = 0.0;

	x_min = -dv_max;
	x_max = dv_max;
	i_pass = 2;

	//Initial guesses
	if (J == 1 || J == 10)
	{
		double DIFF, DP, COR, DEPA, T2, A2, V1, V2, DELAH, DEPH;

		//Calculate DH at N1
		sv[M].TIMA = 0;
		sv[M].TE = sv[M].TS;
		PMMAEGS(aegh, sv[M], sv_temp);
		sv[I].TIMA = 6;
		PMMAEGS(aegh, sv[I], sv_temp);

		DELAH = (sv_temp.Item8 - DKI.DHSR) / 2.0;
		DEPH = 3.0*PI*pow(sqrt(mu) / sv[M].l_dot, 1.0 / 3.0)*DELAH / sqrt(mu);

		DIFF = N3 - DKI.ANAI;
		//Period differential
		DP = PI2 / sv[I].l_dot - PI2 / sv[M].l_dot;
		//Correction in period
		COR = (DKI.theta_init - DKI.COSR) / sv[I].l_dot - DIFF*DP + DEPH * (N3 - N2);
		DEPA = COR / DIFF;
		T2 = PI2 / sv[M].l_dot - DEPA;
		A2 = pow(pow(T2 / PI2, 2)*mu, 1.0 / 3.0);
		V1 = sqrt(mu*(2.0 / sv[M].R - 1.0 / sv[M].coe_osc.a));
		V2 = sqrt(mu*(2.0 / sv[M].R - 1.0 / A2));
		dv[0].data[0] = V2 - V1;
		//sprintf(oapiDebugString(), "%lf", dv[0].data[0]);
	}

	while (converged == false)
	{
		sv[M] = DKI.sv_before[i];

		//Calculate DV
		if (s_man[i] == 1)
		{
			//NC or NH maneuver, just apply DV
			DELVX = dv[i].data[0]; DELVY = dv[i].data[1]; DELVZ = dv[i].data[2];
			//Apply maneuver DV
			PCMVMR(sv[M], sv[I], DELVX, DELVY, DELVZ, mu, Pitch, Yaw, -1);
		}
		else
		{
			//NSR

			//Compute coelliptic DV and elements
			PCMCEM(aegh, sv[M], sv[I], mu);
		}

		//Save state vector after maneuver
		DKI.sv_after[i] = sv[M];

		//Propagate to next point
		if (s_update[i] == 1)
		{
			sv[M].TIMA = 0;
			sv[M].TE = updval[i];
			PMMAEGS(aegh, sv[M], sv[M]);
		}
		else if (s_update[i] == 2)
		{
			sv[M].TIMA = 0;
			sv[M].TE = sv[M].TS + updval[i];
			PMMAEGS(aegh, sv[M], sv[M]);
		}
		else if (s_update[i] == 3)
		{
			sv[M].TIMA = 0;
			sv[M].TE = sv[M].TS + updval[i] * PI2 / sv[M].l_dot;
			PMMAEGS(aegh, sv[M], sv[M]);
		}
		else
		{
			sv[M].TIMA = 3;
			sv[M].Item8 = sv[M].U;
			sv[M].Item9 = sv[M].TS;
			sv[M].Item10 = updval[i];
			PMMAEGS(aegh, sv[M], sv[M]);
		}
		//Store SV at time of next maneuver
		DKI.sv_before[i + 1] = sv[M];

		//Calculate DH and phase angle, if needed
		if (s_term[i + 1] != 0)
		{
			sv[I].TIMA = 6;
			PMMAEGS(aegh, sv[I], sv_temp);
			if (s_term[i + 1] < 0 || s_term[i + 1] > 10)
			{
				e_P = sv_temp.Item10 - dtheta[i + 1];
			}
			if (s_term[i + 1] > 0)
			{
				e_H = sv_temp.Item8 - dh[i + 1];
			}
		}

		i_reverse = false;
		i++;
		if (s_term[i] == 0)
		{
			//No constraint
			i_proc = 2;
		}
		else if (s_term[i] < 0)
		{
			//Phasing constraint
			i_proc = 1;
		}
		else
		{
			i_cp = s_term[i] / 10;
			i_ch = s_term[i] - 10 * i_cp;
			if (i_ch < i_cp)
			{
				i_reverse = true;
				i_proc = 1;
			}
			else
			{
			PMMITL_7_2:
				if (abs(e_H) < DKI.DHT)
				{
					C_H = 0;
					if (s_term[i] < 10 || i_reverse)
					{
						i_proc = 2;
					}
					else
					{
						i_proc = 1;
					}
				}
				else
				{
					i = s_term[i] - 10 * (s_term[i] / 10) - 1;
					if (PCMIT1(C_H, e_H, dv[i].x, e_H0, dv_H0))
					{
						//Error, fail
						return;
					}
					else
					{
						continue;
					}
				}
			}
		}

		if (i_proc == 2)
		{
			if (i >= DKI.NOM)
			{
				//End, converged
				break;
			}
			else
			{
				continue;
			}
		}
		if (abs(e_P) < DKI.DOS || (i_pass == 0 && abs(ddv) < 0.1*0.3048))
		{
			C_P = 0;
			if (i_reverse)
			{
				i_pass = 2;
				x_min = -dv_max;
				x_max = dv_max;
				goto PMMITL_7_2;
			}
			if (i >= DKI.NOM)
			{
				//End, converged
				break;
			}
			else
			{
				continue;
			}
		}
		else
		{
			if (s_term[i] < 10)
			{
				i = abs(s_term[i]);
			}
			else
			{
				i = s_term[i] / 10;
			}
			i--;
			e_PERR = -e_P;
			PCMIT2(i_pass, e_P, e_P0, e_PERR, ddv, dv[i].x, x_min, x_max, K_DV);
			dv_P0 = dv[i].x;
			dv[i].x = dv[i].x + ddv;
			e_P0 = e_P;
			C_P++;
			if (C_P > C_MAX)
			{
				//Error
				return;
			}
			else
			{
				continue;
			}
		}
	}
}

bool RTCC::DockingInitiationProcessor(DKIOpt opt)
{
	DKICommon DKI;
	AEGBlock aeg_init;
	AEGHeader aegh;
	AEGDataBlock sv[2], sv_init[2], sv_temp;
	//Maneuvering vehicle
	int M;
	//Inactive vehicle
	int I;
	//Time of maneuver line
	double TNA;
	//Argument of latitude of initial maneuver line
	double UOCI;
	//Maneuver line
	double PUTNA;
	//Threshold time of TPI
	double TXX;
	double TSR1, TSI, P, ECF2, ESF2, TR, DNSR, mu, Elev, theta_TPI, theta_S, SONEL, RTPM, TTPF, R_E, r_per, PMIN, NHS;
	int ILOOP, i, block;
	bool failed, INH, store, recycle, end;

	PZDKIT.UpdatingIndicator = true;
	PZDKIT.NumSolutions = 0;

	//Convert to aeg format
	aeg_init = SVToAEG(opt.sv_CSM, 0.0, 1.0, 1.0); //TBD
	sv[0] = aeg_init.Data;
	aeg_init = SVToAEG(opt.sv_LM, 0.0, 1.0, 1.0); //TBD
	sv[1] = aeg_init.Data;
	aegh = aeg_init.Header;

	//Inputs
	DKI.MV = opt.MV;
	DKI.dt_NCC_NSR = opt.dt_NCC_NSR;
	PUTNA = opt.PUTNA;
	Elev = opt.Elev;
	DKI.COSR = opt.COSR;
	DKI.TLIT = opt.TIMLIT;
	DKI.K46 = opt.K46;
	DKI.DTSR = opt.DTSR;
	DKI.WT = opt.WT;
	DKI.DOS = opt.DOS;
	DKI.DHT = opt.DHT;
	DKI.TTPI = opt.TTPI;
	DKI.IHALF = opt.IHALF;
	DKI.DHNCC = opt.DHNCC;
	DKI.DHSR = opt.DHSR;
	DKI.NC1 = opt.NC1;
	DKI.NH = opt.NH;
	DKI.NCC = opt.NCC;
	DKI.NPC = opt.NPC;
	DKI.MI = opt.MI;

	if (opt.sv_CSM.RBI == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
		R_E = OrbMech::R_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		R_E = OrbMech::R_Moon;
	}
	PMIN = opt.PMIN + R_E;

	//For compatibility with AGC. Rendezvous from above is usually done with a elevation angle of 208.3�, but the DKI wants an angle from 0� to 180�
	//So both 28.3� or 208.3� can be input and give the same result. This operation is equivalent to what the routine QRDTPI in Skylark does.
	if (Elev > PI)
	{
		Elev = Elev - PI;
	}

	//Initial flag settings
	M = opt.MV - 1;
	I = 1 - M;
	block = 0;
	NHS = opt.NH;
	INH = false;

	//Initialze both vehicles
	sv[0].TIMA = 0;
	sv[0].TE = sv[0].TS;
	PMMAEGS(aegh, sv[0], sv_temp);
	sv[1].TIMA = 0;
	sv[1].TE = sv[1].TS;
	PMMAEGS(aegh, sv[1], sv_temp);

	//Find time of first maneuver
	if (opt.IPUTNA == 1)
	{
		//Input time
		TNA = opt.PUTTNA;
	}
	else if (opt.IPUTNA == 2)
	{
		//Apoapsis
		double INFO[10];
		PMMAPD(aegh, sv[M], 1, 0, INFO, &sv_temp, NULL);
		TNA = sv_temp.TE;
		//If no initial maneuver line was defined, use 1.0 for the apoapsis that was found
		if (PUTNA <= 0.0)
		{
			PUTNA = 1.0;
		}
	}
	else
	{
		//Apoapsis of inactive vehicle
		double INFO[10];
		PMMAPD(aegh, sv[I], 1, 0, INFO, &sv_temp, NULL);
		//Just to store the AEG block in the AEG
		sv_temp.TIMA = 0;
		sv_temp.TE = sv_temp.TS;
		PMMAEGS(aegh, sv_temp, sv_temp);
		//Phase match
		sv[M].TIMA = 6;
		PMMAEGS(aegh, sv[M], sv_temp);
		TNA = sv_temp.TE;
		//If no initial maneuver line was defined, use 1.0 for the apoapsis that was found
		if (PUTNA <= 0.0)
		{
			PUTNA = 1.0;
		}
	}

	//Update both vehicles to time TNA
	sv[I].TE = TNA;
	sv[I].TIMA = 0;
	PMMAEGS(aegh, sv[I], sv[I]);
	sv[M].TE = TNA;
	sv[M].TIMA = 0;
	PMMAEGS(aegh, sv[M], sv[M]);

	DKI.ANAI = PUTNA;
	UOCI = sv[M].U;
	DKI.TNAI = TNA;

	if (opt.I4)
	{
		//Skylab route, calculate estimate of NSR maneuver point

		//Semi-major axis on orbit between NCC and NSR
		double a_av = sv[I].coe_mean.a - (opt.DHNCC + opt.DHSR) / 2.0;
		//Orbital period of that orbit
		double P_av = PI2 * sqrt(pow(a_av, 3) / mu);
		DKI.NSR = opt.NCC + opt.dt_NCC_NSR / P_av;
	}
	else
	{
		//Take input
		DKI.NSR = opt.NSR;
	}

	//Calculate initial phase angle
	sv[I].TIMA = 4;
	PMMAEGS(aegh, sv[I], sv[I]);

	DKI.theta_init = sv[I].Item10;
	if (opt.KRAP != 0)
	{
		if (DKI.theta_init < 0)
		{
			DKI.theta_init += PI2;
		}
		if (opt.KRAP < 0)
		{
			DKI.theta_init = DKI.theta_init - PI2 - PI2 * (double)(abs(opt.KRAP) - 1);
		}
		else
		{
			DKI.theta_init = DKI.theta_init + PI2 * (double)(opt.KRAP - 1);
		}
	}

	//Save both vectors for future use
	sv_init[M] = sv[M];
	sv_init[I] = sv[I];

PMMDKI_2_1:

	//Take maneuvering vehicle to arrival at NSR
	sv[M].TIMA = 3;
	sv[M].Item8 = UOCI;
	sv[M].Item9 = TNA;
	sv[M].Item10 = DKI.NSR - DKI.ANAI;
	PMMAEGS(aegh, sv[M], sv[M]);

	//Take inactive vehicle to phase match at NSR
	sv[I].TIMA = 6;
	PMMAEGS(aegh, sv[I], sv[I]);

	TSR1 = sv[I].TE;

	sv[M].coe_osc.a = sv[I].coe_osc.a - opt.DHSR;
	sv[M].R = sv[I].R - opt.DHSR;
	sv[M].coe_osc.e = sv[I].coe_osc.a*sv[I].coe_osc.e / sv[M].coe_osc.a;
	P = sv[M].coe_osc.a*(1.0 - pow(sv[M].coe_osc.e, 2));
	ECF2 = (P - sv[M].R) / sv[M].R;
	ESF2 = sin(sv[I].f) / abs(sv[I].f)*sqrt(pow(sv[M].coe_osc.e, 2) - ECF2 * ECF2);
	sv[M].f = atan2(ESF2, ECF2);
	if (sv[M].f < 0)
	{
		sv[M].f += PI2;
	}
	sv[M].U = sv[I].U;
	TSI = TSR1;
	sv[M].coe_osc.g = atan2(sin(sv[M].U - sv[M].f), cos(sv[M].U - sv[M].f));
	if (sv[M].coe_osc.g < 0)
	{
		sv[M].coe_osc.g += PI2;
	}
	sv[M].coe_osc.l = sv[M].f - 2.0*sv[M].coe_osc.e*sin(sv[M].f) + 0.75*pow(sv[M].coe_osc.e, 2)*sin(2.0*sv[M].f);
	sv[M].ENTRY = 0;
	ILOOP = 0;
	TR = TSI;
	DNSR = (opt.MI - 1.0) - DKI.NSR;
	TXX = TSI;
	sv[M].l_dot = sqrt(mu) / pow(sv[M].coe_osc.a, 1.5);
	if (DNSR > 0.0)
	{
		TXX = TXX + DNSR * PI2 / sv[M].l_dot;
	}

	//Find TPI time
	PMMPHL(DKI, aegh, sv[I], TXX, DKI.TTPI, TTPF);

	SONEL = 1.0;
	if (Elev > PI05)
	{
		Elev = PI - Elev;
		SONEL = -1.0;
	}
	
	//Update inactive vector to TTPI
	sv[I].TIMA = 0;
	sv[I].TE = DKI.TTPI;
	PMMAEGS(aegh, sv[I], sv[I]);
	theta_TPI = SONEL * (PI05 - Elev - asin((sv[I].R - opt.DHSR)*cos(Elev) / sv[I].R));
	do
	{
		theta_S = theta_TPI;
		ILOOP++;
		sv[I].f = sv[I].U - sv[I].coe_osc.g - theta_TPI;
		RTPM = sv[I].coe_osc.a*(1.0 - pow(sv[I].coe_osc.e, 2)) / (1.0 + sv[I].coe_osc.e*cos(sv[I].f));
		theta_TPI = SONEL * (PI05 - Elev - asin((RTPM - opt.DHSR)*cos(Elev) / sv[I].R));
	} while (abs(theta_TPI - theta_S) > opt.DOS && ILOOP < 10);

	DKI.COSR = theta_TPI;

	//Restore
	sv[M] = sv_init[M];

	//Maneuver order options.
	//J = 1: NC1, NH, NSR
	//J = 2: NH, NC1, NSR
	//J = 3: NC1, NSR
	//J = 4: NH, NSR
	//J = 5: NSR
	//J = 10: NC1, NH, NCC, NSR (Skylab)
	int J;

	if (opt.I4)
	{
		J = 10;
	}
	else
	{
		if (opt.NC1 <= 0)
		{
			if (opt.NH <= 0)
			{
				J = 5;
			}
			else
			{
				J = 4;
			}
		}
		else
		{
			if (opt.NH <= 0)
			{
				J = 3;
			}
			else
			{
				if (opt.NC1 - opt.NH <= 0)
				{
					J = 1;
				}
				else
				{
					J = 2;
				}
			}
		}
	}

	PMMITL(DKI, aegh, sv, J);

	failed = false;
	//Minimum periapsis check
	for (i = 0;i < DKI.NOM;i++)
	{
		r_per = (1.0 - DKI.sv_after[i].coe_osc.e)*DKI.sv_after[i].coe_osc.a;
		if (r_per < PMIN)
		{
			failed = true;
			break;
		}
	}
	
	store = true;
	recycle = false;

	if (failed)
	{
		store = false;
		if (opt.I4 || opt.LNH == false || DKI.NSR - DKI.NH <= 1.0)
		{
			//Plan failed cannot reschedule NH
		}
		else
		{
			DKI.NH = DKI.NSR - 0.5;
			INH = true;
			recycle = true;
		}
	}

	if (store)
	{
		//Store
		PZDKIT.NumSolutions++;
		PZDKIT.Block[block].PlanStatus = 1;
		PZDKIT.Block[block].NumMan = DKI.NOM;
		PZDKIT.Block[block].Plan_M = (int)ceil(DKI.MI);
		PZDKIT.Block[block].NC1 = DKI.NC1;
		PZDKIT.Block[block].NH = DKI.NH;
		if (opt.I4)
		{
			PZDKIT.Block[block].NCC = DKI.NCC;
		}
		else
		{
			PZDKIT.Block[block].NCC = -1.0;
		}
		PZDKIT.Block[block].NSR = DKI.NSR;
		PZDKIT.Block[block].NPC = DKI.NPC;
		PZDKIT.Block[block].TTPI = DKI.TTPI;
		for (int i = 0;i < DKI.NOM;i++)
		{
			PZDKIT.Block[block].Display[i].Man_ID = DKI.ID[i];

			OrbMech::GIMKIC(DKI.sv_before[i].coe_osc, mu, PZDKIELM.Block[block].SV_before[i].R, PZDKIELM.Block[block].SV_before[i].V);
			OrbMech::GIMKIC(DKI.sv_after[i].coe_osc, mu, PZDKIELM.Block[block].SV_before[i].R, PZDKIELM.Block[block].V_after[i]);

			PZDKIELM.Block[block].SV_before[i].GMT = DKI.sv_before[i].TS;
			PZDKIELM.Block[block].SV_before[i].RBI = opt.sv_CSM.RBI;
			PZDKIT.Block[block].Display[0].ManGMT = PZDKIELM.Block[block].SV_before[i].GMT;

			if (M == 0)
			{
				PZDKIT.Block[block].Display[i].VEH = RTCC_MPT_CSM;
			}
			else
			{
				PZDKIT.Block[block].Display[i].VEH = RTCC_MPT_LM;
			}
		}


		//Calculate more display data
		MATRIX3 Q_Xx;
		double DH, Phase, HA, HP;

		for (i = 0;i < PZDKIT.Block[block].NumMan;i++)
		{
			PCPICK(aegh, DKI.sv_after[i], sv[I], DH, Phase, HA, HP);

			PZDKIT.Block[block].Display[i].DH = DH;
			PZDKIT.Block[block].Display[i].PhaseAngle = Phase;
			PZDKIT.Block[block].Display[i].HA = HA;
			PZDKIT.Block[block].Display[i].HP = HP;
			PZDKIT.Block[block].Display[i].ManGMT = DKI.sv_after[i].TS;
			Q_Xx = OrbMech::LVLH_Matrix(PZDKIELM.Block[block].SV_before[i].R, PZDKIELM.Block[block].SV_before[i].V);
			PZDKIT.Block[block].Display[i].DV_LVLH = mul(Q_Xx, PZDKIELM.Block[block].V_after[i] - PZDKIELM.Block[block].SV_before[i].V);
			PZDKIT.Block[block].Display[i].dv = length(PZDKIT.Block[block].Display[i].DV_LVLH);
			PZDKIT.Block[block].Display[i].Yaw = atan2(PZDKIT.Block[block].Display[i].DV_LVLH.y, PZDKIT.Block[block].Display[i].DV_LVLH.x);
			PZDKIT.Block[block].Display[i].Pitch = atan2(-PZDKIT.Block[block].Display[i].DV_LVLH.z, sqrt(pow(PZDKIT.Block[block].Display[i].DV_LVLH.x, 2) + pow(PZDKIT.Block[block].Display[i].DV_LVLH.y, 2)));
		}
		block++;
	}

	end = false;
	if (recycle == false)
	{
		if (INH)
		{
			DKI.NH = NHS;
		}
		if (opt.IDM > DKI.MI)
		{
			DKI.MI += 1.0;
			DKI.NCC += 1.0;
			DKI.NSR += 1.0;
			if (opt.MNH)
			{
				DKI.NH += 1.0;
				NHS = DKI.NH;
			}
		}
		else
		{
			end = true;
		}
	}

	if (end == false)
	{
		sv[M] = sv_init[M];
		sv[I] = sv_init[I];
		goto PMMDKI_2_1;
	}

	//We are done updating
	PZDKIT.UpdatingIndicator = false;

	//Recalculate displays
	PMDRPT();
	PMDRET();

	return true;
}

void RTCC::PMMDKI(SPQOpt &opt, SPQResults &res)
{
	double GETbase, t_CSI0, e_TPI, t_CSI, eps_TPI, c_TPI, e_TPIo, t_CSIo, p_TPI;
	int s_TPI;

	PZDKIT.UpdatingIndicator = true;
	res.err = 0;

	GETbase = CalcGETBase();

	p_TPI = c_TPI = 0.0;
	eps_TPI = 1.0;
	t_CSI = t_CSI0 = opt.t_CSI;

	do
	{
		opt.t_CSI = t_CSI;
		res.err = ConcentricRendezvousProcessor(opt, res);
		if (res.err)
		{
			//Time violation
			if (res.err == 3)
			{
				RTCCONLINEMON.TextBuffer[0] = "TIME";
				PMXSPT("PMMDKI", 92);
			}
			return;
		}

		if (!(opt.OptimumCSI && opt.K_CDH == 1 && opt.t_CSI > 0))
		{
			break;
		}

		e_TPI = res.t_TPI - opt.t_TPI;

		if (p_TPI == 0 || abs(e_TPI) >= eps_TPI)
		{
			OrbMech::ITER(c_TPI, s_TPI, e_TPI, p_TPI, t_CSI, e_TPIo, t_CSIo, 60.0);
		}
		//Iteration checking
		if (t_CSI > opt.t_CSI + opt.dt_CSI_Range)
		{
			//Warning message
			PMXSPT("PMMDKI", 101);
			t_CSI = opt.t_CSI + opt.dt_CSI_Range;
			break;
		}
		if (t_CSI < opt.t_CSI - opt.dt_CSI_Range)
		{
			//Warning message
			PMXSPT("PMMDKI", 101);
			t_CSI = opt.t_CSI - opt.dt_CSI_Range;
			break;
		}
		if (s_TPI == 1)
		{
			//Warning message
			PMXSPT("PMMDKI", 101);
			break;
		}
	} while (abs(e_TPI) >= eps_TPI);

	//Reset input CSI time
	opt.t_CSI = t_CSI0;

	//Plan is a SPQ
	PZDKIT.Block[0].PlanStatus = 2;

	//Save state vector and display data
	if (opt.t_CSI <= 0)
	{
		//Save CDH data
		PZDKIT.NumSolutions = 1;
		PZDKIT.Block[0].NumMan = 1;
		PZDKIELM.Block[0].SV_before[0] = ConvertSVtoEphemData(res.sv_C[0]);
		PZDKIELM.Block[0].V_after[0] = res.sv_C_apo[0].V;
		PZDKIT.Block[0].Display[0].Man_ID = "CDH";
		PZDKIT.Block[0].Display[0].VEH = opt.ChaserID;
	}
	else
	{
		//Save CSI and CDH data
		PZDKIT.NumSolutions = 1;
		PZDKIT.Block[0].NumMan = 2;

		PZDKIELM.Block[0].SV_before[0] = ConvertSVtoEphemData(res.sv_C[0]);
		PZDKIELM.Block[0].V_after[0] = res.sv_C_apo[0].V;
		PZDKIT.Block[0].Display[0].Man_ID = "CSI";
		PZDKIT.Block[0].Display[0].VEH = opt.ChaserID;

		PZDKIELM.Block[0].SV_before[1] = ConvertSVtoEphemData(res.sv_C[1]);
		PZDKIELM.Block[0].V_after[1] = res.sv_C_apo[1].V;
		PZDKIT.Block[0].Display[1].Man_ID = "CDH";
		PZDKIT.Block[0].Display[1].VEH = opt.ChaserID;

		//Calculate TPI and TPF maneuvers
		double t_TPI, t_TPF;
		res.err = PCTETR(res.sv_C_apo[1], res.sv_T[1], opt.WT, opt.E, t_TPI, t_TPF);

		if (res.err == 0)
		{
			//Coast to TPI
			res.sv_C[2] = coast(res.sv_C_apo[1], t_TPI - OrbMech::GETfromMJD(res.sv_C_apo[1].MJD, GETbase));
			res.sv_T[2] = coast(res.sv_T[1], t_TPI - OrbMech::GETfromMJD(res.sv_T[1].MJD, GETbase));

			//Call two-impulse processor
			LambertMan lam;
			TwoImpulseResuls lamres;

			lam.mode = 2;
			lam.T1 = t_TPI;
			lam.T2 = -1;
			lam.N = 0;
			lam.axis = RTCC_LAMBERT_MULTIAXIS;
			lam.Perturbation = RTCC_LAMBERT_PERTURBED;
			lam.sv_A = res.sv_C[2];
			lam.sv_P = res.sv_T[2];
			lam.TravelAngle = opt.WT;

			LambertTargeting(&lam, lamres);

			//Save post TPI state vector
			res.sv_C_apo[2] = res.sv_C[2];
			res.sv_C_apo[2].V = res.sv_C[2].V + lamres.dV;

			//Save TPI data
			PZDKIELM.Block[0].SV_before[2] = ConvertSVtoEphemData(res.sv_C[2]);
			PZDKIELM.Block[0].V_after[2] = res.sv_C_apo[2].V;
			PZDKIT.Block[0].Display[2].Man_ID = "TPI";
			PZDKIT.Block[0].Display[2].VEH = opt.ChaserID;
			PZDKIT.Block[0].NumMan++;

			//Coast to TPF
			res.sv_C[3] = coast(res.sv_C_apo[2], lamres.T2 - OrbMech::GETfromMJD(res.sv_C_apo[2].MJD, GETbase));
			res.sv_T[3] = coast(res.sv_T[2], lamres.T2 - OrbMech::GETfromMJD(res.sv_T[2].MJD, GETbase));

			//Save post TPF state vector
			res.sv_C_apo[3] = res.sv_C[3];
			res.sv_C_apo[3].V = res.sv_C[3].V + lamres.dV2;

			//Save TPF data
			PZDKIELM.Block[0].SV_before[3] = ConvertSVtoEphemData(res.sv_C[3]);
			PZDKIELM.Block[0].V_after[3] = res.sv_C_apo[3].V;
			PZDKIT.Block[0].Display[3].Man_ID = "TPF";
			PZDKIT.Block[0].Display[3].VEH = opt.ChaserID;
			PZDKIT.Block[0].NumMan++;
		}
	}

	//Calculate more display data
	SV sv_C, sv_T;
	MATRIX3 Q_Xx;
	double DH, Phase, HA, HP;
	for (int i = 0;i < PZDKIT.Block[0].NumMan;i++)
	{
		sv_C = ConvertEphemDatatoSV(PZDKIELM.Block[0].SV_before[i]);
		sv_C.V = PZDKIELM.Block[0].V_after[i];
		sv_T = res.sv_T[i];

		PCPICK(sv_C, sv_T, DH, Phase, HA, HP);

		PZDKIT.Block[0].Display[i].DH = DH;
		PZDKIT.Block[0].Display[i].PhaseAngle = Phase;
		PZDKIT.Block[0].Display[i].HA = HA;
		PZDKIT.Block[0].Display[i].HP = HP;
		PZDKIT.Block[0].Display[i].ManGMT = OrbMech::GETfromMJD(sv_C.MJD,SystemParameters.GMTBASE);
		Q_Xx = OrbMech::LVLH_Matrix(PZDKIELM.Block[0].SV_before[i].R, PZDKIELM.Block[0].SV_before[i].V);
		PZDKIT.Block[0].Display[i].DV_LVLH = mul(Q_Xx, PZDKIELM.Block[0].V_after[i] - PZDKIELM.Block[0].SV_before[i].V);
		PZDKIT.Block[0].Display[i].dv = length(PZDKIT.Block[0].Display[i].DV_LVLH);
		PZDKIT.Block[0].Display[i].Yaw = atan2(PZDKIT.Block[0].Display[i].DV_LVLH.y, PZDKIT.Block[0].Display[i].DV_LVLH.x);
		PZDKIT.Block[0].Display[i].Pitch = atan2(-PZDKIT.Block[0].Display[i].DV_LVLH.z, sqrt(pow(PZDKIT.Block[0].Display[i].DV_LVLH.x, 2) + pow(PZDKIT.Block[0].Display[i].DV_LVLH.y, 2)));
	}

	//We are done updating
	PZDKIT.UpdatingIndicator = false;

	//Recalculate display
	PMDRET();
}

void RTCC::PCPICK(AEGHeader header, AEGDataBlock sv_C, AEGDataBlock sv_T, double &DH, double &Phase, double &HA, double &HP)
{
	AEGDataBlock sv_temp;
	double dt, R[3], U[3], R_E, RA, RP;

	sv_C.TIMA = 0;
	sv_C.TE = sv_C.TS;
	PMMAEGS(header, sv_C, sv_temp);

	R[0] = sv_temp.R;
	U[0] = sv_temp.U;

	sv_T.TIMA = 6;
	PMMAEGS(header, sv_T, sv_temp);

	DH = sv_temp.Item8;
	Phase = sv_temp.Item10;

	if (header.AEGInd == BODY_EARTH)
	{
		dt = 15.0*60.0;
		R_E = OrbMech::R_Earth;
	}
	else
	{
		dt = 20.0*60.0;
		R_E = BZLAND.rad[RTCC_LMPOS_BEST];
	}

	sv_C.TE += dt;
	PMMAEGS(header, sv_C, sv_temp);

	R[1] = sv_temp.R;
	U[1] = sv_temp.U;

	sv_C.TE += dt;
	PMMAEGS(header, sv_C, sv_temp);

	R[2] = sv_temp.R;
	U[2] = sv_temp.U;

	PCHAPE(R[0], R[1], R[2], U[0], U[1], U[2], RA, RP);
	HA = RA - R_E;
	HP = RP - R_E;
}

void RTCC::PCPICK(SV sv_C, SV sv_T, double &DH, double &Phase, double &HA, double &HP)
{
	SV sv_TC;
	double mu, RA, RP, R_E, DT;
	Phase = OrbMech::PHSANG(sv_T.R, sv_T.V, sv_C.R);
	if (sv_C.gravref == hEarth)
	{
		mu = OrbMech::mu_Earth;
		R_E = OrbMech::R_Earth;
		DT = 15.0*60.0;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		R_E = BZLAND.rad[RTCC_LMPOS_BEST];
		DT = 20.0*60.0;
	}
	sv_TC = OrbMech::PositionMatch(SystemParameters.AGCEpoch, sv_T, sv_C, mu);
	DH = length(sv_TC.R) - length(sv_C.R);
	SV sv_CC[3];
	sv_CC[0] = sv_C;
	sv_CC[1] = coast(sv_CC[0], DT);
	sv_CC[2] = coast(sv_CC[1], DT);

	CELEMENTS elem;
	double R[3], U[3];

	for (int i = 0;i < 3;i++)
	{
		R[i] = length(sv_CC[i].R);
		elem = OrbMech::GIMIKC(sv_CC[i].R, sv_CC[i].V, mu);
		U[i] = OrbMech::MeanToTrueAnomaly(elem.l, elem.e) + elem.g;
		U[i] = fmod(U[i], PI2);
		if (U[i] < 0)
			U[i] += PI2;
	}
	PCHAPE(R[0], R[1], R[2], U[0], U[1], U[2], RA, RP);
	HA = RA - R_E;
	HP = RP - R_E;
}

void RTCC::PCHAPE(double R1, double R2, double R3, double U1, double U2, double U3, double &RAP, double &RPE)
{
	double alpha, beta, gamma, delta, arg1, arg2, U0, XR, RR;

	alpha = R1 - R3;
	delta = R1 - R2;
	arg1 = sin(U1) - sin(U2) - delta / alpha * (sin(U1) - sin(U3));
	arg2 = delta / alpha * (cos(U3) - cos(U1)) - (cos(U2) - cos(U1));
	U0 = atan2(arg1, arg2);
	beta = sin(U1 - U0);
	gamma = sin(U3 - U0);
	XR = alpha / (beta - gamma);
	RR = (R1 + R3 - XR * (beta + gamma)) / 2.0;
	XR = abs(XR);
	RAP = RR + XR;
	RPE = RR - XR;
}

void RTCC::PMMPNE(AEGHeader Header, AEGDataBlock sv_C, AEGDataBlock sv_T, double TREF, double FNPC, int KPC, int IPC, AEGDataBlock &SAVE, double &DI1, double &DH1)
{
	AEGDataBlock *sv_PC, *sv_NOPC;
	double TA, TB, U_L, U_U, mu;
	int ICT;

	if (Header.AEGInd == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}

	if (abs(KPC) == 2)
	{
		goto RTCC_PMMPNE_2_1;
	}

	ICT = 0;

	//SPQ
	if (KPC < 1)
	{
		TB = TREF + (FNPC - 1.0)*PI / sv_C.l_dot;
		sv_C.TIMA = 0;
		sv_C.TE = TB;
		PMMAEGS(Header, sv_C, sv_C);
		sv_T.TIMA = 6;
		PMMAEGS(Header, sv_T, sv_T);
		TA = sv_T.Item9;
	}
	//DKI
	else
	{
		//TBD
	}
	//Target does PC
	if (IPC < 0)
	{
		SAVE = sv_C;
		U_L = sv_C.U;
		sv_PC = &sv_T;
		sv_NOPC = &sv_C;
	}
	else
	{
		SAVE = sv_T;
		U_L = sv_T.U;
		sv_PC = &sv_C;
		sv_NOPC = &sv_T;
	}
	U_U = U_L + PI;
	if (U_U >= PI2)
	{
		U_U -= PI2;
	}
RTCC_PMMPNE_2_1:
	if (abs(KPC) == 3)
	{
		goto RTCC_PMMPNE_3_1;
	}
	double DP, DI, DH;
	DP = PI2 / sv_NOPC->l_dot - PI2 / sv_PC->l_dot;
	DI = sv_NOPC->coe_mean.i - sv_PC->coe_mean.i;
	DH = sv_NOPC->coe_mean.h - sv_PC->coe_mean.h;
	if (abs(DP) >= 1.0 && Header.AEGInd == BODY_EARTH)
	{
		DH += PI2 / sv_NOPC->l_dot*TA*(sv_NOPC->h_dot - sv_PC->h_dot) / DP;
	}
	if (abs(KPC) == 2)
	{
		DI1 += DI;
		DH1 += DH;
		return;
	}
	DI1 = DI;
	DH1 = DH;
RTCC_PMMPNE_3_1:
	double i_apo, h_apo, g_apo;
	if (IPC < 0)
	{
		i_apo = sv_T.coe_osc.i;
		h_apo = sv_T.coe_osc.h;
		g_apo = sv_T.coe_osc.g;
		sv_C = sv_T;
	}
	else
	{
		i_apo = sv_C.coe_osc.i;
		h_apo = sv_C.coe_osc.h;
		g_apo = sv_C.coe_osc.g;
		sv_T = sv_C;
	}
	sv_T.coe_osc.i = i_apo + DI;
	sv_T.coe_osc.h = h_apo + DH;
	if (sv_T.coe_osc.h >= PI2)
	{
		sv_T.coe_osc.h -= PI2;
	}
	sv_T.coe_osc.g = g_apo - 2.0*atan(tan(0.5*DH*sin((PI - sv_T.coe_osc.i + i_apo) / 2.0)) / sin(PI - sv_T.coe_osc.i + i_apo) / 2.0);
	if (sv_T.coe_osc.g >= PI2)
	{
		sv_T.coe_osc.g -= PI2;
	}
	else if (sv_T.coe_osc.g < 0)
	{
		sv_T.coe_osc.g += PI2;
	}
	sv_T.ENTRY = 0;
	//At this point PC vehicle is AEG block #1 (sv_C), phantom vehicle is AEG block #2 (sv_T)
	double cos_dw, DEN, U_CN, U_CN_apo, dw, DV_PC;
RTCC_PMMPNE_3_2:
	cos_dw = cos(sv_T.coe_osc.i)*cos(sv_C.coe_osc.i) + sin(sv_T.coe_osc.i)*sin(sv_C.coe_osc.i)*cos(sv_C.coe_osc.h - sv_T.coe_osc.h);
	DEN = cos_dw * cos(sv_C.coe_osc.i) - cos(sv_T.coe_osc.i);
	if (sv_T.coe_osc.h < sv_C.coe_osc.h)
	{
		DEN = -DEN;
	}
	U_CN = atan2(sin(sv_T.coe_osc.i)*sin(sv_C.coe_osc.i)*sin(abs(sv_C.coe_osc.h - sv_T.coe_osc.h)), DEN);
	if ((U_L > PI && U_U <= U_CN) || (U_L <= PI && U_U > U_CN))
	{
		U_CN += PI;
	}
	if (ICT != 0)
	{
		goto RTCC_PMMPNE_5_2;
	}
RTCC_PMMPNE_4_2:
	ICT++;
	if (ICT > 3)
	{
		goto RTCC_PMMPNE_6_1;
	}
	U_CN_apo = U_CN;
	//Take PC vehicle to U_CN and the phantom vehicle to position match
	sv_C.TIMA = 2;
	sv_C.Item8 = U_CN;
	PMMAEGS(Header, sv_C, sv_C);
	sv_T.TIMA = 6;
	PMMAEGS(Header, sv_T, sv_T);
	goto RTCC_PMMPNE_3_2;
RTCC_PMMPNE_5_1:
	if (abs(U_CN - U_CN_apo) > PI05 && abs(U_CN - U_CN_apo) <= PI && abs(U_CN - sv_C.U) <= PI && abs(U_CN - sv_C.U) >= PI05)
	{
		U_CN += PI;
		if (U_CN >= PI2)
		{
			U_CN -= PI2;
		}
	}
	goto RTCC_PMMPNE_4_2;
RTCC_PMMPNE_5_2:
	dw = acos(cos_dw);
	DV_PC = 2.0*sqrt(mu)*sin(dw / 2.0)*sqrt(2.0/sv_C.R-1.0/ sv_C.coe_osc.a);
	if (ICT != 1)
	{
		goto RTCC_PMMPNE_5_1;
	}
	if (DV_PC > 5.0*0.3048)
	{
		goto RTCC_PMMPNE_5_1;
	}
RTCC_PMMPNE_6_1:
	if (KPC > 0)
	{
		goto RTCC_PMMPNE_8_1;
	}
	double T_NPC, S, DV_Z, DV_H;
	T_NPC = sv_C.TE;
	VECTOR3 R_C, V_C, R_P, V_P, H_P, H_C, K;
	OrbMech::GIMKIC(sv_C.coe_osc, mu, R_C, V_C);
	OrbMech::GIMKIC(sv_T.coe_osc, mu, R_P, V_P);
	H_P = unit(crossp(R_P, V_P));
	H_C = unit(crossp(R_C, V_C));
	K = unit(crossp(H_P, R_C));
	S = dotp(H_C, K);
	DV_Z = S * DV_PC*cos(0.5*dw) / abs(S);
	DV_H = -DV_PC * sin(0.5*dw);
	//TBD: Output
	sv_T = SAVE;
	return;
RTCC_PMMPNE_8_1:
	//TBD
	return;
}

void RTCC::PCMVMR(AEGDataBlock &CHASER, AEGDataBlock &TARGET, double DELVX, double DELVY, double DELVZ, double mu, double &Pitch, double &Yaw, int I)
{
	VECTOR3 R_C, V_C, R_T, V_T, V_C_apo;
	OrbMech::GIMKIC(CHASER.coe_osc, mu, R_C, V_C);
	OrbMech::GIMKIC(TARGET.coe_osc, mu, R_T, V_T);

	PCMVMR(R_C, V_C, R_T, V_T, DELVX, DELVY, DELVZ, I, V_C_apo, Pitch, Yaw);

	CHASER.coe_osc = OrbMech::GIMIKC(R_C, V_C_apo, mu);
	CHASER.ENTRY = 0;
}

void RTCC::PCMVMR(VECTOR3 R_C, VECTOR3 V_C, VECTOR3 R_T, VECTOR3 V_T, double DELVX, double DELVY, double DELVZ, int I, VECTOR3 &V_C_apo, double &Pitch, double &Yaw)
{
	//I = 0: Pitch and yaw parallel to target, -1 = apply DV in-plane, 1 = apply DV parallel
	VECTOR3 H1, H2, J2, K1, K2;
	double DV_H, DV_R, DV_Z;

	DV_H = DELVX;
	DV_R = DELVY;
	DV_Z = DELVZ;

	H2 = unit(crossp(R_C, V_C));
	H1 = unit(crossp(R_T, V_T));
	K2 = unit(crossp(H2, R_C));
	K1 = unit(crossp(H1, R_C));
	J2 = unit(crossp(K2, H2));
	if (I >= 0)
	{
		double S = dotp(H2, K1);
		double ABP = S / abs(S)*acos(dotp(unit(K1), unit(K2)));
		DV_Z = DV_H * tan(ABP);
		Yaw = -ABP;
		Pitch = -atan2(DV_R, DV_H / cos(ABP));
		if (I <= 0)
		{
			return;
		}
	}
	V_C_apo = V_C + K2 * DV_H - J2 * DV_R + H2 * DV_Z;
}

int RTCC::PCTETR(SV sv_C, SV sv_T, double WT, double ESP, double &TESP, double &TR)
{
	SV sv_C1, sv_T1;
	double QL, DU, mu, C1, C2, C3, C4, r_T, r_C, QLI, DT, eps_dt, L_C_dot, L_T_dot;
	int I;
	bool err;

	eps_dt = 0.1;
	I = 1;
	sv_C1 = sv_C;
	sv_T1 = sv_T;
	if (sv_C1.gravref == hEarth)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}
	do
	{
		sv_T1 = OrbMech::PMMAEGS(SystemParameters.AGCEpoch, sv_T1, 0, sv_C1.MJD, err);
		if (err)
		{
			return -1;
		}
		QL = OrbMech::PHSANG(sv_T1.R, sv_T1.V, sv_C1.R);
		if (I == 1)
		{
			L_C_dot = OrbMech::GetMeanMotion(sv_C1.R, sv_C1.V, mu);
			L_T_dot = OrbMech::GetMeanMotion(sv_T1.R, sv_T1.V, mu);
			DU = L_C_dot - L_T_dot;
			if (DU*QL < 0)
			{
				QL += PI2;
			}
		}
		r_C = length(sv_C1.R) / OrbMech::R_Earth;
		r_T = length(sv_T1.R) / OrbMech::R_Earth;
		C1 = -pow(r_T / cos(ESP), 2);
		C2 = 2.0*r_T*r_C;
		C3 = -C1 - r_T * r_T - r_C * r_C;
		C4 = C2 * C2 - 4.0*C1*C3;
		if (C4 < 0)
		{
			return -1;
		}
		QLI = acos((-C2 - sqrt(C4)) / (2.0*C1));
		if (r_C > r_T)
		{
			QLI = -QLI;
		}
		DT = (QL - QLI) / DU;
		if (abs(DT) < eps_dt)
		{
			TESP = OrbMech::GETfromMJD(sv_C1.MJD, CalcGETBase());
			TR = TESP + WT / L_T_dot;
			return 0;
		}
		sv_C1 = coast(sv_C1, DT);
		I++;
	} while (I <= 25);
	return -1;
}

//Module PMMSPQ
int RTCC::ConcentricRendezvousProcessor(const SPQOpt &opt, SPQResults &res)
{
	SV sv_C_CSI, sv_T_CSI, sv_C_CDH, sv_C_TPI, sv_T_CDH, sv_C_CSI_apo, sv_T_TPI, sv_C_CDH_apo, sv_PC;
	CELEMENTS elem;
	MATRIX3 Q_Xx;
	VECTOR3 R_TJ, V_TJ, R_AF, R_AFD;
	double GETbase, dv_CSI, mu, t_CDH, t_TPI, DH, p_C, c_C, e_C, e_Co, dv_CSIo, V_Cb, V_CRb, gamma_C, V_CHb, a_T, a_C, r_T_dot, r_C_dot, T_TPF;
	double V_C_apo, gamma_C_apo, V_CHa, DV_H, DV_R, Pitch, Yaw;
	int s_C, PCMVMR_IND;
	bool err;

	GETbase = CalcGETBase();

	//Set up variables for the CSI iteration
	p_C = c_C = 0.0;
	s_C = 0;

	//In which orbit are we?
	if (opt.sv_A.gravref == hEarth)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}

	//Calculate the DV parallel to the target orbit if desired, and only if no plane change maneuver is scheduled
	if (opt.N_PC == false && opt.ParallelDVInd)
	{
		PCMVMR_IND = 1;
	}
	else
	{
		PCMVMR_IND = -1;
	}

	//TPI Definition
	if (opt.K_CDH == 0)
	{
		//TBD: Environment change and longitude crossing logic
		t_TPI = opt.t_TPI;
	}

	//CSI scheduled?
	if (opt.t_CSI > 0)
	{
		sv_C_CSI = coast(opt.sv_A, opt.t_CSI - OrbMech::GETfromMJD(opt.sv_A.MJD, GETbase));
		sv_T_CSI = coast(opt.sv_P, opt.t_CSI - OrbMech::GETfromMJD(opt.sv_P.MJD, GETbase));

		//Initial guess for CSI DV from GSOP
		VECTOR3 H1 = unit(crossp(crossp(sv_C_CSI.R, sv_C_CSI.V), sv_C_CSI.R));
		dv_CSI = sqrt(2.0*mu*length(sv_T_CSI.R)*1.0 / (length(sv_C_CSI.R)*(length(sv_T_CSI.R) + length(sv_C_CSI.R)))) - dotp(sv_C_CSI.V, H1);

		//Propagate target to TPI
		if (opt.K_CDH == 0)
		{
			sv_T_TPI = coast(sv_T_CSI, t_TPI - OrbMech::GETfromMJD(sv_T_CSI.MJD, GETbase));
		}
	}
	else
	{
		sv_C_CDH = coast(opt.sv_A, opt.t_CDH - OrbMech::GETfromMJD(opt.sv_A.MJD, GETbase));
		sv_T_CDH = coast(opt.sv_P, opt.t_CDH - OrbMech::GETfromMJD(opt.sv_P.MJD, GETbase));
	}

	//Here we return to the beginning of the loop
RTCC_PMMSPQ_A:
	if (opt.t_CSI > 0)
	{
		//CSI only calculations
		sv_C_CSI_apo = sv_C_CSI;
		//Apply maneuver
		PCMVMR(sv_C_CSI.R, sv_C_CSI.V, sv_T_CSI.R, sv_T_CSI.V, dv_CSI, 0.0, 0.0, PCMVMR_IND, sv_C_CSI_apo.V, Pitch, Yaw);

		//CDH at upcoming apsis (AEG)
		if (opt.I_CDH == 1 || opt.I_CDH == 3)
		{
			//true = CDH at upcoming apsis, false = CDH at angle from CSI
			bool apsissw;

			if (opt.I_CDH == 3)
			{
				//Don't use apsis option if angle option was requested
				apsissw = false;
			}
			else
			{
				//Otherwise decide based on eccentricity
				elem = OrbMech::GIMIKC(sv_C_CSI_apo.R, sv_C_CSI_apo.V, mu);
				if (elem.e < 0.0001)
				{
					apsissw = false;
				}
				else
				{
					apsissw = true;
				}
			}

			if (apsissw)
			{
				double l_c, DN;
				int N_CK;

				N_CK = 1;
				DN = 0.0;
				sv_C_CDH = sv_C_CSI_apo;
				elem = OrbMech::GIMIKC(sv_C_CDH.R, sv_C_CDH.V, mu);

				if (elem.l >= PI)
				{
					l_c = PI2;
				}
				else
				{
					l_c = PI;
				}

				do
				{
					sv_C_CDH = OrbMech::PMMAEGS(SystemParameters.AGCEpoch, sv_C_CDH, 1, l_c, err, DN);
					if (opt.N_CDH > N_CK)
					{
						N_CK++;
						elem = OrbMech::GIMIKC(sv_C_CDH.R, sv_C_CDH.V, mu);
						l_c = elem.l + PI;
						if (l_c > PI2)
						{
							l_c -= PI2;
							DN = 1.0;
						}
						else
						{
							DN = 0.0;
						}
					}
					else
					{
						break;
					}
				} while (opt.N_CDH >= N_CK);
				t_CDH = OrbMech::GETfromMJD(sv_C_CDH.MJD, GETbase);
			}
			else
			{
				double DU_D;

				if (opt.I_CDH == 1)
				{
					DU_D = PI * (double)opt.N_CDH;
				}
				else
				{
					DU_D = opt.DU_D;
				}
				double u_CSI, u_CDH, DN = 0;

				//Calculate argument of latitude at CSI
				elem = OrbMech::GIMIKC(sv_C_CSI_apo.R, sv_C_CSI_apo.V, mu);
				u_CSI = OrbMech::MeanToTrueAnomaly(elem.l, elem.e) + elem.g;
				u_CSI = fmod(u_CSI, PI2);
				if (u_CSI < 0)
					u_CSI += PI2;

				//Calculate required argument of latitude at CDH
				u_CDH = u_CSI + DU_D;
				while (u_CDH > PI2)
				{
					u_CDH -= PI2;
					DN += 1.0;
				}

				//Propagate to CDH
				sv_C_CDH = OrbMech::PMMAEGS(SystemParameters.AGCEpoch, sv_C_CSI_apo, 2, u_CDH, err, DN);
				t_CDH = OrbMech::GETfromMJD(sv_C_CDH.MJD, GETbase);
			}
		}
		//CDH on time
		else if (opt.I_CDH == 2)
		{
			t_CDH = opt.t_CDH;
			sv_C_CDH = coast(sv_C_CSI_apo, t_CDH - OrbMech::GETfromMJD(sv_C_CSI_apo.MJD, GETbase));
		}
		//CDH at next apsis (Keplerian)
		else
		{
			OELEMENTS coe;
			double V_R, t_P, t_CSI;
			
			t_CSI = OrbMech::GETfromMJD(sv_C_CSI_apo.MJD, GETbase);
			coe = OrbMech::coe_from_sv(sv_C_CSI_apo.R, sv_C_CSI_apo.V, mu);
			t_P = OrbMech::period(sv_C_CSI_apo.R, sv_C_CSI_apo.V, mu);
			V_R = dotp(sv_C_CSI_apo.R, sv_C_CSI_apo.V);

			if ((coe.e < 0.0001) || (V_R / length(sv_C_CSI_apo.R) < 7.0*0.3048))
			{
				t_CDH = t_CSI + t_P / 2.0*(double)opt.N_CDH;
			}
			else
			{
				double theta, dt;
				//Next apsis
				if (coe.TA > PI)
				{
					theta = PI2 - coe.TA;
				}
				else
				{
					theta = PI - coe.TA;
				}
				OrbMech::time_theta(sv_C_CSI_apo.R, sv_C_CSI_apo.V, theta, mu, dt);
				t_CDH = t_CSI + dt + t_P / 2.0*(double)(opt.N_CDH - 1);
			}
			sv_C_CDH = coast(sv_C_CSI_apo, t_CDH - t_CSI);
		}
		sv_T_CDH = coast(sv_T_CSI, t_CDH - OrbMech::GETfromMJD(sv_T_CSI.MJD, GETbase));
	}

	//Target to position (phase) match
	sv_PC = OrbMech::PositionMatch(SystemParameters.AGCEpoch, sv_T_CDH, sv_C_CDH, mu);
	//Calculate delta height
	DH = length(sv_PC.R) - length(sv_C_CDH.R);

	//For iteration on DH
	if (opt.t_CSI > 0 && opt.K_CDH == 1)
	{
		e_C = DH - opt.DH;
		if (abs(e_C) > 1.0)
		{
			OrbMech::ITER(c_C, s_C, e_C, p_C, dv_CSI, e_Co, dv_CSIo);
			goto RTCC_PMMSPQ_A;
		}
	}

	//CDH calculation
	V_Cb = length(sv_C_CDH.V);
	V_CRb = dotp(sv_C_CDH.R, sv_C_CDH.V) / length(sv_C_CDH.R);
	gamma_C = asin(V_CRb / V_Cb);
	V_CHb = V_Cb * cos(gamma_C);
	a_T = 1.0 / (2.0 / length(sv_PC.R) - dotp(sv_PC.V, sv_PC.V) / mu);
	a_C = a_T - DH;
	r_T_dot = dotp(sv_PC.R, sv_PC.V) / length(sv_PC.R);
	r_C_dot = r_T_dot * pow(a_T / a_C, 1.5);
	//Velocity after the maneuver
	V_C_apo = sqrt(mu*(2.0 / length(sv_C_CDH.R) - 1.0 / a_C));
	//Flight path angle after maneuver
	gamma_C_apo = asin(r_C_dot / V_C_apo);
	//Horizontal velocity after maneuver
	V_CHa = V_C_apo * cos(gamma_C_apo);
	DV_H = V_CHa - V_CHb;
	DV_R = V_CRb - r_C_dot;

	sv_C_CDH_apo = sv_C_CDH;
	PCMVMR(sv_C_CDH.R, sv_C_CDH.V, sv_T_CDH.R, sv_T_CDH.V, DV_H, DV_R, 0.0, PCMVMR_IND, sv_C_CDH_apo.V, Pitch, Yaw);

	if (opt.t_CSI <= 0)
	{
		//Final CDH comps
		res.DH = DH;
		Q_Xx = OrbMech::LVLH_Matrix(sv_C_CDH.R, sv_C_CDH.V);
		res.dV_CDH = mul(Q_Xx, sv_C_CDH_apo.V - sv_C_CDH.V);
		res.t_CDH = opt.t_CDH;
		res.t_TPI = 0.0;

		res.sv_C[0] = sv_C_CDH;
		res.sv_C_apo[0] = sv_C_CDH_apo;
		res.sv_T[0] = sv_T_CDH;
		
		return 0;
	}

	//Iteration on TPI time
	if (opt.K_CDH == 0)
	{
		sv_C_TPI = coast(sv_C_CDH_apo, t_TPI - t_CDH);
		OrbMech::QDRTPI(SystemParameters.AGCEpoch, sv_T_TPI.R, sv_T_TPI.V, sv_T_TPI.MJD, sv_T_TPI.gravref, mu, DH, opt.E, 1, R_TJ, V_TJ);
		R_AFD = R_TJ - unit(R_TJ)*DH;
		R_AF = OrbMech::PROJCT(R_TJ, V_TJ, sv_C_TPI.R);

		e_C = OrbMech::sign(dotp(crossp(R_AF, R_AFD), unit(crossp(R_TJ, V_TJ))))*acos(dotp(R_AFD / length(R_AFD), R_AF / length(R_AF)));
		if (abs(e_C) > 0.000004)
		{
			OrbMech::ITER(c_C, s_C, e_C, p_C, dv_CSI, e_Co, dv_CSIo);
			goto RTCC_PMMSPQ_A;
		}
	}
	else
	{
		//If iteration on DH was used, get TPI time now
		if (PCTETR(sv_C_CDH_apo, sv_T_CDH, opt.WT, opt.E, t_TPI, T_TPF))
		{
			return 94;
		}
	}

	if (t_CDH - opt.t_CSI < 10.0*60.0)
	{
		return 3;
	}
	if (t_TPI - t_CDH < 10.0*60.0)
	{
		return 3;
	}

	res.t_CSI = opt.t_CSI;
	res.dV_CSI = _V(dv_CSI, 0, 0);
	res.t_CDH = t_CDH;
	Q_Xx = OrbMech::LVLH_Matrix(sv_C_CDH.R, sv_C_CDH.V);
	res.dV_CDH = mul(Q_Xx, sv_C_CDH_apo.V - sv_C_CDH.V);
	res.t_TPI = t_TPI;
	res.DH = DH;

	//Save state vectors
	res.sv_C[0] = sv_C_CSI;
	res.sv_C_apo[0] = sv_C_CSI_apo;
	res.sv_T[0] = sv_T_CSI;
	res.sv_C[1] = sv_C_CDH;
	res.sv_C_apo[1] = sv_C_CDH_apo;
	res.sv_T[1] = sv_T_CDH;

	return 0;
}

VECTOR3 RTCC::HatchOpenThermalControl(double GMT, MATRIX3 REFSMMAT)
{
	MATRIX3 SMNB, MY, MX;
	VECTOR3 R_EM, V_EM, R_ES, R_SC, UX, UY, UZ, U_south;

	//Get sun vector
	PLEFEM(1, GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL);
	//Vector pointing south in ECI coordinates
	U_south = _V(0, 0, -1);
	//Vector pointing from Earth to Sun
	R_SC = -R_ES;

	//Matrix with -Z axis pointing at the sun, +X to Earth south
	UZ = unit(R_SC);
	UY = unit(crossp(UZ, U_south));
	UX = crossp(UY, UZ);

	SMNB = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	MY = OrbMech::_MRy(-15.0*RAD);
	MX = OrbMech::_MRx(-80.0*RAD);
	SMNB = mul(MX, mul(MY, SMNB));
	return OrbMech::CALCGAR(REFSMMAT, SMNB);
}

void RTCC::DockingAlignmentProcessor(DockAlignOpt &opt)
{
	//Option 1: LM REFSMMAT from CSM REFSMMAT, CSM attitude, docking angle and LM gimbal angles
	//Option 2: LM gimbal angles from CSM REFSMMAT, LM REFSMMAT, CSM gimbal angles and docking angle
	//Option 3: CSM gimbal angles from CSM REFSMMAT, LM REFSMMAT, LM gimbal angles and docking angle
	//Coordinate Systems:
	//Navigation Base (NB)
	//Stable Member (SM)
	//Basic Reference Coordinate System (BRCS)
	//REFSMMAT is BRCS to SM

	MATRIX3 M_NBCSM_NBLM;

	M_NBCSM_NBLM = OrbMech::CSMBodyToLMBody(opt.DockingAngle);

	if (opt.type == 1)
	{
		MATRIX3 M_SMCSM_NBCSM, M_SMLM_NBLM, M_BRCS_NBCSM, M_BRCS_NBLM, M_BRCS_SMLM;

		M_SMCSM_NBCSM = OrbMech::CALCSMSC(opt.CSMAngles);
		M_SMLM_NBLM = OrbMech::CALCSMSC(opt.LMAngles);
		M_BRCS_NBCSM = mul(M_SMCSM_NBCSM, opt.CSM_REFSMMAT);
		M_BRCS_NBLM = mul(M_NBCSM_NBLM, M_BRCS_NBCSM);
		M_BRCS_SMLM = mul(OrbMech::tmat(M_SMLM_NBLM), M_BRCS_NBLM);

		opt.LM_REFSMMAT = M_BRCS_SMLM;
	}
	else if (opt.type == 2)
	{
		MATRIX3 M_SMCSM_NBCSM, M_SMLM_SMCSM, M_SMCSM_NBLM, M_SMLM_NBLM;

		M_SMCSM_NBCSM = OrbMech::CALCSMSC(opt.CSMAngles);
		M_SMLM_SMCSM = mul(opt.CSM_REFSMMAT, OrbMech::tmat(opt.LM_REFSMMAT));
		M_SMCSM_NBLM = mul(M_NBCSM_NBLM, M_SMCSM_NBCSM);
		M_SMLM_NBLM = mul(M_SMCSM_NBLM, M_SMLM_SMCSM);

		opt.LMAngles = OrbMech::CALCGAR(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), M_SMLM_NBLM);
	}
	else if (opt.type == 3)
	{
		MATRIX3 M_SMLM_NBLM, M_SMCSM_NBCSM, M_SMCSM_SMLM, M_SMCSM_NBLM;

		M_SMLM_NBLM = OrbMech::CALCSMSC(opt.LMAngles);
		M_SMCSM_SMLM = mul(opt.LM_REFSMMAT, OrbMech::tmat(opt.CSM_REFSMMAT));
		M_SMCSM_NBLM = mul(M_SMLM_NBLM, M_SMCSM_SMLM);
		M_SMCSM_NBCSM = mul(OrbMech::tmat(M_NBCSM_NBLM), M_SMCSM_NBLM);

		opt.CSMAngles = OrbMech::CALCGAR(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), M_SMCSM_NBCSM);
	}
}

VECTOR3 RTCC::SkylabDockingAttitude(EphemerisData sv, MATRIX3 REFSMMAT, double DDA)
{
	MATRIX3 M_BRCS_SLB, M_BCSM_BSL, M_SMCSM_BCSM;
	VECTOR3 R_ES, h_ows, x_axis, y_axis, z_axis;

	//Get sun direction at state vector time
	PLEFEM(1, sv.GMT / 3600.0, 0, NULL, NULL, &R_ES, NULL);

	h_ows = unit(crossp(sv.R, sv.V));

	z_axis = unit(R_ES);
	x_axis = -unit(crossp(z_axis, h_ows));
	y_axis = crossp(z_axis, x_axis);

	M_BRCS_SLB = _M(x_axis.x, x_axis.y, x_axis.z, y_axis.x, y_axis.y, y_axis.z, z_axis.x, z_axis.y, z_axis.z); //BRCS to Skylab body
	M_BCSM_BSL = mul(OrbMech::_MRy(180.0*RAD), OrbMech::_MRx(-(35.0*RAD - DDA))); //CSM to Skylab body
	M_SMCSM_BCSM = mul(OrbMech::tmat(M_BCSM_BSL), mul(M_BRCS_SLB, OrbMech::tmat(REFSMMAT))); //SM CSM to BRCS, BRCS to Skylab body, Skylab body to CSM body

	return OrbMech::CALCGAR(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), M_SMCSM_BCSM);
}

AEGBlock RTCC::SVToAEG(EphemerisData sv, double Area, double Weight, double KFactor)
{
	AEGBlock aeg;

	EphemerisData sv1 = sv;

	if (sv.RBI == BODY_EARTH)
	{
		aeg.Data.ICSUBD = KFactor;
		aeg.Data.VehArea = Area;
		aeg.Data.Item7 = Weight;
	}

	aeg.Header.AEGInd = sv1.RBI;

	PIMCKC(sv1.R, sv1.V, sv1.RBI, aeg.Data.coe_osc.a, aeg.Data.coe_osc.e, aeg.Data.coe_osc.i, aeg.Data.coe_osc.l, aeg.Data.coe_osc.g, aeg.Data.coe_osc.h);

	aeg.Data.TS = aeg.Data.TE = sv1.GMT;
	aeg.Data.Item8 = aeg.Data.Item9 = aeg.Data.Item10 = 0.0;

	return aeg;
}

int RTCC::PMMAPD(AEGHeader Header, AEGDataBlock Z, int KAOP, int KE, double *INFO, AEGDataBlock *sv_A, AEGDataBlock *sv_P)
{
	//INPUTS:
	//Z: AEG block
	//KAOP: -1 = periapsis only, 0 = apoapsis and periapsis, 1 = apoapsis only
	//KE: < 0 for ECT, >= 0 for ECI (only needed for Earth)
	//OUTPUTS:
	//INFO: Array of size 10. Contains Time, radius, latitude, longitude, height of apogee and perigee
	AEGDataBlock Z_A, Z_P;

	for (int i = 0;i < 10;i++)
	{
		INFO[i] = 0.0;
	}

	//Decision logic for high vs. low e route
	if ((Z.ENTRY > 0 && Z.coe_mean.e > 0.005) || Z.coe_osc.e > 0.005)
	{
		//High e route

		//Propagate to periapsis (mean anomaly = 0)
		if (KAOP <= 0)
		{
			Z.TIMA = 1;
			Z.Item8 = 0.0;
			Z.Item10 = 1.0;
			PMMAEGS(Header, Z, Z_P);
			if (Header.ErrorInd)
			{
				//Error
				return 1;
			}
		}

		//Propagate to apoapsis (mean anomaly = PI)
		if (KAOP >= 0)
		{
			Z.TIMA = 1;
			Z.Item8 = PI;
			if (Z.coe_osc.l > PI)
			{
				Z.Item10 = 1.0;
			}
			else
			{
				Z.Item10 = 0.0;
			}
			PMMAEGS(Header, Z, Z_A);
			if (Header.ErrorInd)
			{
				//Error
				return 1;
			}
		}

		goto RTCC_PMMAPD_1_2;
	}

	if (Z.ENTRY == 0)
	{
		//Initialize
		PMMAEGS(Header, Z, Z);
		if (Header.ErrorInd)
		{
			//Error
			return 1;
		}
	}
	double dt;
	if (Header.AEGInd == BODY_EARTH)
	{
		dt = 15.0*60.0;
	}
	else
	{
		dt = 20.0*60.0;
	}
	double u[3], r[3];

	//Get three data sets of u and r
	u[0] = Z.U;
	r[0] = Z.R;

	Z.TE = Z.TS + dt;
	Z.TIMA = 0;
	PMMAEGS(Header, Z, Z_P);
	if (Header.ErrorInd)
	{
		return 1;
	}

	u[1] = Z_P.U;
	r[1] = Z_P.R;

	Z.TE += dt;
	PMMAEGS(Header, Z, Z_P);
	if (Header.ErrorInd)
	{
		//Error
		return 1;
	}

	u[2] = Z_P.U;
	r[2] = Z_P.R;

	//Calculate argument of latitude of periapsis and apoapsis
	double gamma, u0;
	gamma = (r[0] - r[1]) / (r[0] - r[2]);
	u0 = atan2(sin(u[0]) - sin(u[1]) - gamma * (sin(u[0]) - sin(u[2])), gamma*(cos(u[2]) - cos(u[0])) - cos(u[1]) + cos(u[0]));
	if (u0 < 0)
	{
		u0 += PI2;
	}
	double ux, uy;

	ux = u0 + PI05;
	uy = u0 - PI05;
	if (ux < 0)
	{
		ux += PI2;
	}
	if (uy < 0)
	{
		uy += PI2;
	}

	Z.TIMA = 2;
	Z.Item8 = ux;
	if (Z.U > ux)
	{
		Z.Item10 = 1.0;
	}
	else
	{
		Z.Item10 = 0.0;
	}
	PMMAEGS(Header, Z, Z_P);
	if (Header.ErrorInd)
	{
		//Error
		return 1;
	}

	Z.Item8 = uy;
	if (Z.U > uy)
	{
		Z.Item10 = 1.0;
	}
	else
	{
		Z.Item10 = 0.0;
	}
	PMMAEGS(Header, Z, Z_A);
	if (Header.ErrorInd)
	{
		//Error
		return 1;
	}

	//Check which one actually was periapsis. If it is wrong, switch them
	if (Z_P.R > Z_A.R)
	{
		AEGDataBlock temp;
		temp = Z_P;
		Z_P = Z_A;
		Z_A = temp;
	}

	//One-shot improvement of apogee and perigee
	/*double dang;

	dang = PI - Z_A.coe_osc.l;
	Z_A.TE = Z_A.TS + dang / Z_A.l_dot;
	Z_A.TIMA = 0;
	PMMAEGS(Header, Z_A, Z_A);

	dang = 0.0 - Z_P.coe_osc.l;
	if (dang > PI)
	{
		dang -= PI2;
	}
	Z_P.TE = Z_P.TS + dang / Z_P.l_dot;
	Z_P.TIMA = 0;
	PMMAEGS(Header, Z_P, Z_P);*/

RTCC_PMMAPD_1_2:
	double DELTA, ang;

	if (Header.AEGInd == BODY_EARTH && KE >= 0)
	{
		ang = SystemParameters.MCLAMD;
	}
	else
	{
		ang = 0.0;
	}

	double R_E;
	if (Header.AEGInd == BODY_EARTH)
	{
		R_E = OrbMech::R_Earth;
	}
	else
	{
		R_E = BZLAND.rad[RTCC_LMPOS_BEST];
	}

	if (KAOP >= 0)
	{
		double u_temp, i_temp, g_temp, h_temp, Z;
		if (Header.AEGInd == BODY_EARTH)
		{
			u_temp = Z_A.U;
			i_temp = Z_A.coe_osc.i;
			g_temp = Z_A.coe_osc.g;
			h_temp = Z_A.coe_osc.h;
			Z = 1.0;
		}
		else
		{
			MATRIX3 L;
			VECTOR3 P, W, P_apo, W_apo;
			PIVECT(Z_A.coe_osc.i, Z_A.coe_osc.g, Z_A.coe_osc.h, P, W);
			PLEFEM(5, Z_A.TS / 3600.0, 0, NULL, NULL, NULL, &L);
			P_apo = mul(L, P);
			W_apo = mul(L, W);
			PIVECT(P_apo, W_apo, i_temp, g_temp, h_temp);
			u_temp = g_temp + Z_A.f;
			if (u_temp > PI2)
			{
				u_temp -= PI2;
			}
			Z = 0.0;
		}

		INFO[0] = Z_A.TS;
		INFO[1] = Z_A.R;
		INFO[2] = asin(sin(u_temp)*sin(i_temp));
		DELTA = GLQATN(sin(u_temp)*cos(i_temp), cos(u_temp));
		INFO[3] = h_temp + DELTA - Z*(ang + Z_A.TS*OrbMech::w_Earth);
		INFO[3] = fmod(INFO[3], PI2);
		if (INFO[3] > PI)
		{
			INFO[3] -= PI2;
		}
		else if (INFO[3] <= -PI)
		{
			INFO[3] += PI2;
		}
		INFO[4] = Z_A.R - R_E;

		*sv_A = Z_A;
	}

	if (KAOP <= 0)
	{
		double u_temp, i_temp, g_temp, h_temp, Z;
		if (Header.AEGInd == BODY_EARTH)
		{
			u_temp = Z_P.U;
			i_temp = Z_P.coe_osc.i;
			g_temp = Z_P.coe_osc.g;
			h_temp = Z_P.coe_osc.h;
			Z = 1.0;
		}
		else
		{
			MATRIX3 L;
			VECTOR3 P, W, P_apo, W_apo;
			PIVECT(Z_P.coe_osc.i, Z_P.coe_osc.g, Z_P.coe_osc.h, P, W);
			PLEFEM(5, Z_P.TS / 3600.0, 0, NULL, NULL, NULL, &L);
			P_apo = mul(L, P);
			W_apo = mul(L, W);
			PIVECT(P_apo, W_apo, i_temp, g_temp, h_temp);
			u_temp = g_temp + Z_P.f;
			if (u_temp > PI2)
			{
				u_temp -= PI2;
			}
			Z = 0.0;
		}

		INFO[5] = Z_P.TS;
		INFO[6] = Z_P.R;
		INFO[7] = asin(sin(u_temp)*sin(i_temp));
		DELTA = atan2(sin(u_temp)*cos(i_temp), cos(u_temp));
		INFO[8] = h_temp + DELTA - Z*(ang + Z_P.TS*OrbMech::w_Earth);
		INFO[8] = fmod(INFO[8], PI2);
		if (INFO[8] > PI)
		{
			INFO[8] -= PI2;
		}
		else if (INFO[8] <= -PI)
		{
			INFO[8] += PI2;
		}
		INFO[9] = Z_P.R - R_E;

		*sv_P = Z_P;
	}

	return 0;
}

bool RTCC::GETEval2(double get)
{
	if (OrbMech::GETfromMJD(oapiGetSimMJD(), CalcGETBase()) > get)
	{
		return true;
	}

	return false;
}

void RTCC::MPTMassUpdate(VESSEL *vessel, MED_M50 &med1, MED_M55 &med2, MED_M49 &med3, bool docked)
{
	int vesseltype = 0;
	std::string cfg;
	double cmmass, lmmass, sivb_mass, lm_ascent_mass;
	cmmass = lmmass = sivb_mass = lm_ascent_mass = 0.0;

	double spsmass, csmrcsmass, sivbfuelmass, lmapsmass, lmrcsmass, lmdpsmass;
	spsmass = csmrcsmass = sivbfuelmass = lmapsmass = lmrcsmass = lmdpsmass = 0;

	if (vessel == NULL) return;

	if (utils::IsVessel(vessel, utils::Saturn))
	{
		vesseltype = 0;
	}
	else if (utils::IsVessel(vessel, utils::LEM))
	{
		vesseltype = 1;
	}
	else if (utils::IsVessel(vessel, utils::SIVB))
	{
		vesseltype = 2;
	}
	else
	{
		vesseltype = 3;
	}

	if (vesseltype == 3) return;

	if (vesseltype == 0)
	{
		Saturn *sat = (Saturn *)vessel;
		if (sat->GetStage() < CSM_LEM_STAGE)
		{
			csmrcsmass = 4.0*152.5;

			cmmass = sat->SM_FuelMass + sat->SM_EmptyMass + sat->CM_FuelMass + sat->CM_EmptyMass + csmrcsmass + 2.0*55.5;
			if (sat->SIVBPayload == PAYLOAD_LEM)
			{
				cfg = "CSL";

				lmrcsmass = 2.0*133.084001;
				lmapsmass = sat->LMAscentFuelMassKg;
				lmdpsmass = sat->LMDescentFuelMassKg;

				lmmass = lmdpsmass + lmapsmass + sat->LMDescentEmptyMassKg + sat->LMAscentEmptyMassKg + lmrcsmass;
				lm_ascent_mass = lmapsmass + sat->LMAscentEmptyMassKg + lmrcsmass;
			}
			else
			{
				cfg = "CS";
			}
			sivb_mass = vessel->GetMass() - cmmass - lmmass;

			spsmass = sat->SM_FuelMass;
			sivbfuelmass = sat->GetPropellantMass(sat->GetPropellantHandleByIndex(6));
		}
		else
		{
			cmmass = vessel->GetMass();
			if (docked && (lmmass = GetDockedVesselMass(vessel)))
			{
				DOCKHANDLE dock;
				OBJHANDLE hLM;
				VESSEL *lm;
				dock = vessel->GetDockHandle(0);
				hLM = vessel->GetDockStatus(dock);
				lm = oapiGetVesselInterface(hLM);

				LEM *lem = (LEM *)lm;
				lm_ascent_mass = lem->GetAscentStageMass();

				//TBD: Make this better
				if (lem->GetStage() < 2)
				{
					cfg = "CL";
					lmapsmass = lem->AscentFuelMassKg;
					lmdpsmass = lm->GetPropellantMass(lm->GetPropellantHandleByIndex(0));
				}
				else
				{
					cfg = "CA";
					lmapsmass = lm->GetPropellantMass(lm->GetPropellantHandleByIndex(0));
				}

				lmrcsmass = lm->GetPropellantMass(lm->GetPropellantHandleByIndex(1)) + lm->GetPropellantMass(lm->GetPropellantHandleByIndex(2));
			}
			else
			{
				cfg = "C";
			}

			if (sat->GetStage() < CM_STAGE)
			{
				spsmass = sat->GetPropellantMass(sat->GetPropellantHandleByIndex(0));
				csmrcsmass = sat->GetPropellantMass(sat->GetPropellantHandleByIndex(1)) + sat->GetPropellantMass(sat->GetPropellantHandleByIndex(2)) +
					sat->GetPropellantMass(sat->GetPropellantHandleByIndex(3)) + sat->GetPropellantMass(sat->GetPropellantHandleByIndex(4));
			}
		}
	}
	else if (vesseltype == 1)
	{
		LEM *lem = (LEM *)vessel;

		lmmass = vessel->GetMass();
		lm_ascent_mass = lem->GetAscentStageMass();

		if (lem->GetStage() < 2)
		{
			if (docked && (cmmass = GetDockedVesselMass(vessel)))
			{
				cfg = "CL";
			}
			else
			{
				cfg = "L";
			}
			lmapsmass = lem->AscentFuelMassKg;
			lmdpsmass = lem->GetPropellantMass(lem->GetPropellantHandleByIndex(0));
		}
		else
		{
			if (docked && (cmmass = GetDockedVesselMass(vessel)))
			{
				cfg = "CA";
			}
			else
			{
				cfg = "A";
			}
			lmapsmass = lem->GetPropellantMass(lem->GetPropellantHandleByIndex(0));
		}

		lmrcsmass = vessel->GetPropellantMass(vessel->GetPropellantHandleByIndex(1)) + vessel->GetPropellantMass(vessel->GetPropellantHandleByIndex(2));

		if (cfg[0] == 'C')
		{
			DOCKHANDLE dock;
			OBJHANDLE hCSM;
			VESSEL *csm;
			dock = vessel->GetDockHandle(0);
			hCSM = vessel->GetDockStatus(dock);
			csm = oapiGetVesselInterface(hCSM);

			Saturn *sat = (Saturn *)csm;

			if (sat->GetStage() < CM_STAGE)
			{
				spsmass = sat->GetPropellantMass(sat->GetPropellantHandleByIndex(0));
				csmrcsmass = sat->GetPropellantMass(sat->GetPropellantHandleByIndex(1)) + sat->GetPropellantMass(sat->GetPropellantHandleByIndex(2)) +
					sat->GetPropellantMass(sat->GetPropellantHandleByIndex(3)) + sat->GetPropellantMass(sat->GetPropellantHandleByIndex(4));
			}
		}
	}
	else
	{
		cfg = "S";
		sivb_mass = vessel->GetMass();
		sivbfuelmass = vessel->GetPropellantMass(vessel->GetPropellantHandleByIndex(0));
	}

	med2.ConfigCode = cfg;

	med1.CSMWT = cmmass;
	med1.LMWT = lmmass;
	med1.LMASCWT = lm_ascent_mass;
	med1.SIVBWT = sivb_mass;
	med1.WeightGET = GETfromGMT(RTCCPresentTimeGMT());

	med3.CSMRCSFuelRemaining = csmrcsmass;
	med3.LMAPSFuelRemaining = lmapsmass;
	med3.LMDPSFuelRemaining = lmdpsmass;
	med3.LMRCSFuelRemaining = lmrcsmass;
	med3.SIVBFuelRemaining = sivbfuelmass;
	med3.SPSFuelRemaining = spsmass;
}

VECTOR3 RTCC::LOICrewChartUpdateProcessor(EphemerisData sv0, MATRIX3 REFSMMAT, double p_EMP, double LOI_TIG, VECTOR3 dV_LVLH_LOI, double p_T, double y_T)
{
	EphemerisData sv_tig;
	MATRIX3 M_EMP, M_R, M, M_RTM;
	VECTOR3 X_B, UX, UY, UZ, IMUangles;
	double dt, headsswitch;

	headsswitch = -1.0;

	dt = GMTfromGET(LOI_TIG) - sv0.GMT;
	sv_tig = coast(sv0, dt);
	ELVCNV(sv_tig.GMT, RTCC_COORDINATES_MCI, RTCC_COORDINATES_EMP, M_EMP);
	X_B = tmul(M_EMP, _V(-sin(p_EMP), cos(p_EMP), 0));

	UX = X_B;
	UY = unit(crossp(X_B, sv_tig.R*headsswitch));
	UZ = unit(crossp(X_B, crossp(X_B, sv_tig.R*headsswitch)));

	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
	M_RTM = mul(OrbMech::tmat(M_R), M);
	IMUangles = OrbMech::CALCGAR(REFSMMAT, mul(OrbMech::tmat(M), M_R));

	return IMUangles;
}

bool RTCC::PoweredDescentProcessor(VECTOR3 R_LS, double TLAND, SV sv, RTCCNIAuxOutputTable &aux, EphemerisDataTable2 *E, SV &sv_PDI, SV &sv_land, double &dv)
{
	MATRIX3 Rot, REFSMMAT;
	DescentGuidance descguid;
	AscDescIntegrator integ;
	SV sv_IG, sv_D;
	VECTOR3 U_FDP, WI, W, R_LSP, U_FDP_abort, U_M;
	double GETbase, t_go, CR, t_PDI, t_UL, t_D, W_TD, T_DPS, isp, t_D_old;
	bool stop;

	bool LandFlag = false;
	t_UL = 7.9;
	dv = 0.0;

	GETbase = CalcGETBase();

	//Just for t_go and U_FDP, call to this function should be removed
	if (!PDIIgnitionAlgorithm(sv, R_LS, TLAND, sv_IG, t_go, CR, U_FDP, REFSMMAT))
	{
		return false;
	}
	//Use input SV instead
	sv_IG = sv;
	t_go = -t_go;
	t_PDI = OrbMech::GETfromMJD(sv_IG.MJD, GETbase);
	t_D = t_PDI - t_UL;
	t_D_old = t_D;
	sv_D = coast(sv_IG, -t_UL);
	sv_PDI = sv_IG;

	ELVCNV(OrbMech::GETfromMJD(sv_IG.MJD, GetGMTBase()), RTCC_COORDINATES_MCT, RTCC_COORDINATES_MCI, Rot);
	WI = mul(Rot, _V(0, 0, 1));
	W = mul(REFSMMAT, WI)*OrbMech::w_Moon;
	R_LSP = mul(REFSMMAT, mul(Rot, R_LS));
	descguid.Init(sv_IG.R, sv_IG.V, sv_IG.mass, t_PDI, REFSMMAT, R_LSP, t_PDI, W, t_go, &RTCCDescentTargets);
	W_TD = sv_IG.mass;
	U_FDP_abort = tmul(REFSMMAT, unit(U_FDP));

	stop = false;

	integ.Init(U_FDP_abort);

	do
	{
		descguid.Guidance(sv_D.R, sv_D.V, W_TD, t_D, U_FDP, t_go, T_DPS, isp);
		LandFlag = integ.Integration(sv_D.R, sv_D.V, W_TD, t_D, U_FDP, t_go, T_DPS, isp);
		dv += T_DPS / W_TD * (t_D - t_D_old);
		t_D_old = t_D;
	} while (LandFlag == false);

	sv_land.gravref = sv.gravref;
	sv_land.MJD = OrbMech::MJDfromGET(t_D, GETbase);

	ELVCNV(OrbMech::GETfromMJD(sv_land.MJD, GetGMTBase()), RTCC_COORDINATES_MCT, RTCC_COORDINATES_MCI, Rot);
	sv_land.mass = W_TD;
	sv_land.R = mul(Rot, R_LS);
	U_M = mul(Rot, _V(0, 0, 1));
	sv_land.V = crossp(U_M, sv_land.R)*OrbMech::w_Moon;

	return true;
}

void RTCC::LunarAscentProcessor(VECTOR3 R_LS, double m0, SV sv_CSM, double t_liftoff, double v_LH, double v_LV, double &theta, double &dt_asc, double &dv, SV &sv_IG, SV &sv_Ins)
{
	//Test
	AscentGuidance asc;
	AscDescIntegrator integ;
	SV sv_CSM_TIG, sv_ins;
	MATRIX3 Rot;
	VECTOR3 R0, V0, R, V, U_FDP, U_M, u, R0_proj;
	double t_go, Thrust, w_M, dt, t_total, isp, m1, t_total_old;
	bool stop = false;

	dv = 0.0;
	t_total = 0.0;
	dt = t_liftoff - OrbMech::GETfromMJD(sv_CSM.MJD, CalcGETBase());
	sv_CSM_TIG = coast(sv_CSM, dt);

	asc.Init(sv_CSM_TIG.R, sv_CSM_TIG.V, m0, length(R_LS), v_LH, v_LV);

	ELVCNV(OrbMech::GETfromMJD(sv_CSM_TIG.MJD, GetGMTBase()), RTCC_COORDINATES_MCT, RTCC_COORDINATES_MCI, Rot);

	R0 = mul(Rot, R_LS);
	w_M = OrbMech::w_Moon;
	U_M = mul(Rot, _V(0, 0, 1));
	V0 = crossp(U_M, R0)*w_M;

	sv_IG.gravref = sv_CSM.gravref;
	sv_IG.mass = m0;
	sv_IG.MJD = sv_CSM_TIG.MJD;
	sv_IG.R = R0;
	sv_IG.V = V0;

	R = R0;
	V = V0;
	m1 = m0;
	t_total_old = t_total;
	integ.Init(unit(R));

	while (stop == false)
	{
		asc.Guidance(R, V, m1, t_total, U_FDP, t_go, Thrust, isp);
		stop = integ.Integration(R, V, m1, t_total, U_FDP, t_go, Thrust, isp);
		dv += Thrust / m1 * (t_total - t_total_old);
		t_total_old = t_total;
	}

	u = unit(crossp(R, V));
	R0_proj = unit(R0 - u * dotp(R0, u))*length(R0);
	theta = acos(dotp(unit(R0_proj), unit(R)));
	dt_asc = t_total;

	sv_ins.R = R;
	sv_ins.V = V;
	sv_ins.mass = m1;
	sv_ins.gravref = sv_CSM_TIG.gravref;
	sv_ins.MJD = sv_CSM_TIG.MJD + dt_asc / 24.0 / 3600.0;
	sv_Ins = sv_ins;
}

bool RTCC::PDIIgnitionAlgorithm(SV sv, VECTOR3 R_LS, double TLAND, SV &sv_IG, double &t_go, double &CR, VECTOR3 &U_IG, MATRIX3 &REFSMMAT)
{
	SV sv_I;
	MATRIX3 C_GP, Rot;
	VECTOR3 U_FDP, dV_TrP, R_TG, V_TG, A_TG, R_LSI, R_LSP, W_I, W_P, R_P, G_P, V_P, R_G, V_SURFP, V_G, A_G, A_FDP;
	VECTOR3 C_XGP, C_YGP, C_ZGP, U_XSM, U_YSM, U_ZSM;
	double GETbase, GUIDDURN, AF_TRIM, DELTTRIM, TTT, t_pip, dt_I, FRAC;
	double v_IGG, r_IGXG, r_IGZG, K_X, K_Y, K_V;
	double J_TZG, A_TZG, V_TZG, R_TZG;
	double LEADTIME, w_M, t_2, t_I, PIPTIME, t_pipold, eps, dTTT, TTT_P, TEM, q;
	int n1, n2, COUNT_TTT;

	GETbase = CalcGETBase();

	GUIDDURN = 664.4;
	AF_TRIM = 0.350133;
	DELTTRIM = 26.0;
	n1 = 40;
	n2 = 2;
	COUNT_TTT = 0;
	U_FDP = dV_TrP = _V(0, 0, 0);
	C_GP = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	TTT = 0.0;
	t_pip = TLAND;
	dt_I = 1.0;
	FRAC = 43455.0;

	v_IGG = RTCCPDIIgnitionTargets.v_IGG;
	r_IGXG = RTCCPDIIgnitionTargets.r_IGXG;
	r_IGZG = RTCCPDIIgnitionTargets.r_IGZG;
	K_X = RTCCPDIIgnitionTargets.K_X;
	K_Y = RTCCPDIIgnitionTargets.K_Y;
	K_V = RTCCPDIIgnitionTargets.K_V;
	J_TZG = RTCCDescentTargets.JBRFGZ;
	A_TZG = RTCCDescentTargets.ABRFG.z;
	V_TZG = RTCCDescentTargets.VBRFG.z;
	R_TG = RTCCDescentTargets.RBRFG;
	R_TZG = RTCCDescentTargets.RBRFG.z;
	V_TG = RTCCDescentTargets.VBRFG;
	A_TG = RTCCDescentTargets.ABRFG;
	LEADTIME = 2.2;
	w_M = 2.66169948e-6;

	t_2 = OrbMech::GETfromMJD(sv.MJD, GETbase);

	ELVCNV(GMTfromGET(TLAND), RTCC_COORDINATES_MCT, RTCC_COORDINATES_MCI, Rot);
	R_LSI = mul(Rot, R_LS);

	U_XSM = unit(R_LSI);
	U_ZSM = unit(crossp(crossp(sv.R, sv.V), U_XSM));
	U_YSM = crossp(U_ZSM, U_XSM);
	REFSMMAT = _M(U_XSM.x, U_XSM.y, U_XSM.z, U_YSM.x, U_YSM.y, U_YSM.z, U_ZSM.x, U_ZSM.y, U_ZSM.z);

	R_LSP = mul(REFSMMAT, R_LSI);
	t_I = TLAND - GUIDDURN;
	W_I = mul(Rot, _V(0.0, 0.0, 1.0));
	W_P = mul(REFSMMAT, W_I)*w_M;
	sv_I = coast(sv, t_I - t_2);

	while (abs(dt_I) > 0.08 && n1>0)
	{
		PIPTIME = t_I;
		R_P = mul(REFSMMAT, sv_I.R);
		G_P = -R_P / pow(length(R_P), 3.0)*OrbMech::mu_Moon;

		n2 = 2;

		t_pipold = t_pip;
		t_pip = PIPTIME;
		t_2 = PIPTIME;
		R_LSP = unit(R_LSP + crossp(W_P, R_LSP)*(t_pip - t_pipold))*length(R_LS);
		TTT = TTT + t_pip - t_pipold;

		while (n2 > 0)
		{
			V_P = mul(REFSMMAT, sv_I.V) + dV_TrP;

			R_G = mul(C_GP, R_P - R_LSP);
			V_SURFP = V_P - crossp(W_P, R_P);
			V_G = mul(C_GP, V_SURFP);

			COUNT_TTT = 0;
			eps = abs(TTT / 128.0);
			do
			{
				dTTT = -(J_TZG*pow(TTT, 3.0) + 6.0*A_TZG*TTT*TTT + (18.0*V_TZG + 6.0*V_G.z)*TTT + 24.0*(R_TZG - R_G.z)) / (3.0*J_TZG*TTT*TTT + 12.0*A_TZG*TTT + 18.0*V_TZG + 6.0*V_G.z);
				TTT += dTTT;
				COUNT_TTT++;
			} while (abs(dTTT) > eps && COUNT_TTT < 8);

			if (COUNT_TTT == 8)
			{
				return false;
			}

			TTT_P = TTT + LEADTIME;
			A_G = (R_TG - R_G)*(-24.0*TTT_P / pow(TTT, 3.0) + 36.0*TTT_P*TTT_P / pow(TTT, 4.0));
			A_G += V_TG * (-18.0*TTT_P / TTT / TTT + 24.0*TTT_P*TTT_P / pow(TTT, 3.0));
			A_G += V_G * (-6.0*TTT_P / TTT / TTT + 12.0*TTT_P*TTT_P / pow(TTT, 3.0));
			A_G += A_TG * (6.0*TTT_P*TTT_P / TTT / TTT - 6.0*TTT_P / TTT + 1.0);

			A_FDP = tmul(C_GP, A_G) - G_P;
			//s = length(A_FDP);
			TEM = FRAC * FRAC / sv.mass / sv.mass - A_FDP.x*A_FDP.x - A_FDP.y*A_FDP.y;
			if (TEM < 0.0)
			{
				TEM = 0.0;
			}
			if (sqrt(TEM) + A_FDP.z < 0.0)
			{
				A_FDP.z = -sqrt(TEM);
			}
			U_FDP = A_FDP;

			C_XGP = unit(R_LSP);
			C_YGP = unit(crossp(unit(V_SURFP*TTT / 4.0 + R_LSP - R_P), R_LSP));
			C_ZGP = crossp(C_XGP, C_YGP);
			C_GP = _M(C_XGP.x, C_YGP.x, C_ZGP.x, C_XGP.y, C_YGP.y, C_ZGP.y, C_XGP.z, C_YGP.z, C_ZGP.z);

			dV_TrP = unit(U_FDP)*DELTTRIM*AF_TRIM;

			n2--;
		}

		n1--;
		q = K_X * (R_G.x - r_IGXG) + K_Y * R_G.y*R_G.y + R_G.z - r_IGZG + K_V * (length(V_G) - v_IGG);
		dt_I = -q / (V_G.z + K_X * V_G.x);
		t_I += dt_I;

		if (abs(dt_I) > 0.08)
		{
			sv_I = coast(sv_I, dt_I);
		}
	}

	if (n1 == 0)
	{
		return false;
	}

	sv_IG = coast(sv_I, -DELTTRIM);
	t_go = TTT;
	U_IG = U_FDP;
	CR = dotp(unit(crossp(V_P, R_P)), R_LSP);

	return true;
}

//Based on NTRS 19740072723
bool RTCC::PoweredDescentAbortProgram(PDAPOpt opt, PDAPResults &res)
{
	SPQOpt conopt;
	SPQResults conres;
	DescentGuidance descguid;
	AscentGuidance ascguid;
	AscDescIntegrator integ;
	SV sv_I_guess, sv_IG, sv_D, sv_CSM_Ins, sv_LM_Ins, sv_Abort, sv_CAN, sv_CAN_apo, sv_CSM_Abort;
	MATRIX3 Rot, Q_Xx, REFSMMAT;
	VECTOR3 U_FDP, WM, WI, W, R_LSP, U_FDP_abort;
	double GETbase, t_go, CR, t_PDI, t_D, t_UL, t_stage, W_TD, T_DPS, dt_abort, Z_D_dot, R_D_dot, W_TA, t, T, isp, t_Ins, TS, theta, r_Ins, A_Ins, H_a, t_CSI, DH_D;
	double SLOPE, dV_Inc, dh_apo, w_M, V_H_min, t_CAN, dt_CSI, R_a, R_a_apo, dt_CAN, theta_D, theta_apo, t_go_abort;
	int K_loop;
	bool K_stage;
	//false = CSI/CDH, true = Boost + CSI/CDH
	bool K3;
	bool LandFlag = false;
	bool InsertionFlag = false;
	bool stop = false;
	std::vector<double> t_Abort_Table;
	std::vector<double> dV_Abort_Table;
	std::vector<double> Phase_Table;
	std::vector<double> A_ins_Table;
	int i = 0;

	GETbase = CalcGETBase();

	K3 = false;
	w_M = 2.66169948e-6;
	t_UL = 7.9;
	Z_D_dot = 5650.0*0.3048;
	R_D_dot = 19.5*0.3048;
	DH_D = 15.0*1852.0;
	V_H_min = 5515.0*0.3048;
	dt_CSI = opt.dt_CSI;
	dt_CAN = opt.dt_CAN;
	conopt.E = 26.6*RAD;
	conopt.K_CDH = 0;
	conopt.t_TPI = opt.t_TPI;

	res.R_amin = length(opt.R_LS) + opt.h_amin;

	dt_abort = opt.dt_step;
	sv_I_guess = coast(opt.sv_A, opt.TLAND - OrbMech::GETfromMJD(opt.sv_A.MJD, GETbase));
	if (!PDIIgnitionAlgorithm(sv_I_guess, opt.R_LS, opt.TLAND, sv_IG, t_go, CR, U_FDP, REFSMMAT))
	{
		return false;
	}
	t_go = -t_go;
	t_go_abort = t_go;
	t_PDI = OrbMech::GETfromMJD(sv_IG.MJD, GETbase);
	t_D = t_PDI - t_UL;
	sv_Abort = coast(sv_IG, -t_UL);
	t_stage = t_PDI + opt.dt_stage;

	ELVCNV(OrbMech::GETfromMJD(sv_IG.MJD, GetGMTBase()), RTCC_COORDINATES_MCT, RTCC_COORDINATES_MCI, Rot);
	WI = mul(Rot, _V(0, 0, 1));
	W = mul(opt.REFSMMAT, WI)*w_M;
	R_LSP = mul(opt.REFSMMAT, mul(Rot, opt.R_LS));
	descguid.Init(sv_IG.R, sv_IG.V, opt.sv_A.mass, t_PDI, opt.REFSMMAT, R_LSP, t_PDI, W, t_go, &RTCCDescentTargets);
	W_TD = opt.sv_A.mass;
	U_FDP_abort = tmul(opt.REFSMMAT, unit(U_FDP));

	res.DEDA225 = (length(R_LSP) + 60000.0*0.3048 + length(R_LSP) + opt.h_amin) / 2.0;
	res.DEDA226 = 7031200.0*0.3048;

	do
	{
		stop = false;

		integ.Init(U_FDP_abort);
		sv_D = sv_Abort;
		t_go = t_go_abort;
		K_loop = 0;
		SLOPE = 2.0 / 1852.0*0.3048;

		do
		{
			if (LandFlag)
			{
				t_D = t_PDI + dt_abort;
				t_stage = 0.;
				break;
			}
			else
			{
				descguid.Guidance(sv_D.R, sv_D.V, W_TD, t_D, U_FDP, t_go, T_DPS, isp);
				LandFlag = integ.Integration(sv_D.R, sv_D.V, W_TD, t_D, U_FDP, t_go, T_DPS, isp);
				if (LandFlag)
				{
					t_D = t_PDI + dt_abort;
					t_stage = 0.;
					break;
				}
			}
		} while (t_D <= t_PDI + dt_abort);

		T = 43192.23;
		sv_Abort = sv_D;
		U_FDP_abort = U_FDP;
		t_go_abort = t_go;

		sv_CSM_Abort = coast(opt.sv_P, t_D - OrbMech::GETfromMJD(opt.sv_P.MJD, GETbase));
		WM = unit(crossp(sv_CSM_Abort.V, sv_CSM_Abort.R));
		TS = dotp(crossp(unit(sv_CSM_Abort.R), unit(sv_Abort.R)), WM);
		theta_D = OrbMech::sign(TS)*acos(dotp(unit(sv_CSM_Abort.R), unit(sv_Abort.R)));

		do
		{
			sv_D = sv_Abort;
			ascguid.Init(opt.sv_P.R, opt.sv_P.V, W_TD, length(opt.R_LS), Z_D_dot, R_D_dot, false);
			t = t_D;
			t_go = dt_abort;
			W_TA = W_TD;
			if (t >= t_stage || W_TA <= opt.W_TDRY)
			{
				t_go *= 2.0;
			}
			ascguid.SetTGO(t_go);
			K_stage = false;

			do
			{
				if (K_stage == false)
				{
					if (t >= t_stage || W_TA <= opt.W_TDRY)
					{
						K_stage = true;
						W_TA = opt.W_TAPS;
						ascguid.SetThrustParams(true);
					}
				}

				ascguid.Guidance(sv_D.R, sv_D.V, W_TA, t, U_FDP, t_go, T, isp);
				if (dotp(U_FDP, integ.GetCurrentTD()) < 0)
				{
					if (acos(dotp(integ.GetCurrentTD(), unit(sv_D.R))) > 30.0*RAD)
					{
						U_FDP = unit(sv_D.R);
					}
				}
				InsertionFlag = integ.Integration(sv_D.R, sv_D.V, W_TA, t, U_FDP, t_go, T, isp);
			} while (InsertionFlag == false);
			t_Ins = t;
			sv_LM_Ins = sv_D;
			sv_LM_Ins.mass = W_TA;
			sv_LM_Ins.MJD = OrbMech::MJDfromGET(t_Ins, GETbase);

			//Overwrite actual insertion velocity with desired one; gives more consistent results
			Q_Xx = OrbMech::LVLH_Matrix(sv_LM_Ins.R, sv_LM_Ins.V);
			sv_LM_Ins.V = tmul(Q_Xx, _V(Z_D_dot, 0, -R_D_dot));

			sv_CSM_Ins = coast(opt.sv_P, t_Ins - OrbMech::GETfromMJD(opt.sv_P.MJD, GETbase));
			WM = unit(crossp(sv_CSM_Ins.V, sv_CSM_Ins.R));
			TS = dotp(crossp(unit(sv_CSM_Ins.R), unit(sv_LM_Ins.R)), WM);
			theta = OrbMech::sign(TS)*acos(dotp(unit(sv_CSM_Ins.R), unit(sv_LM_Ins.R)));
			
			r_Ins = length(sv_D.R);
			A_Ins = OrbMech::mu_Moon * r_Ins / (2.0*OrbMech::mu_Moon - r_Ins * Z_D_dot*Z_D_dot);
			H_a = 2.0*A_Ins - r_Ins - length(opt.R_LS);
			R_a = 2.0*A_Ins - r_Ins;
			t_CSI = t_Ins + dt_CSI;

			if (opt.LongProfileFirst)
			{
				if (K3 == false)
				{
					t_CAN = t_Ins + dt_CAN;
					sv_CAN = coast(sv_LM_Ins, t_CAN - t_Ins);
					sv_CAN_apo = sv_CAN;
					sv_CAN_apo.V += tmul(OrbMech::LVLH_Matrix(sv_CAN.R, sv_CAN.V), _V(opt.dv_CAN, 0.0, 0.0));
					conopt.sv_A = sv_CAN_apo;
				}
				else
				{
					conopt.sv_A = sv_LM_Ins;
				}
			}
			else
			{
				if (K3 == false || dt_CAN <= 0.0 || opt.dv_CAN <= 0.0)
				{
					conopt.sv_A = sv_LM_Ins;
				}
				else
				{
					t_CAN = t_Ins + dt_CAN;
					sv_CAN = coast(sv_LM_Ins, t_CAN - t_Ins);
					sv_CAN_apo = sv_CAN;
					sv_CAN_apo.V += tmul(OrbMech::LVLH_Matrix(sv_CAN.R, sv_CAN.V), _V(opt.dv_CAN, 0.0, 0.0));
					conopt.sv_A = sv_CAN_apo;
				}
			}

			conopt.sv_P = sv_CSM_Ins;
			conopt.t_CSI = t_CSI;

			ConcentricRendezvousProcessor(conopt, conres);
			K_loop++;
			if (K_loop > 1)
			{
				SLOPE = dV_Inc / (conres.DH - dh_apo);
			}

			dV_Inc = SLOPE * (DH_D - conres.DH);
			Z_D_dot += dV_Inc;
			dh_apo = conres.DH;

		} while (abs(dV_Inc) > 0.1*0.3048 && K_loop < 10);

		if (opt.IsTwoSegment && R_a < res.R_amin)
		{
			if (K3 == false)
			{
				K_loop = 0;
				dt_CSI += opt.dt_2CSI;
				conopt.t_TPI += opt.dt_2TPI;
				res.Theta_LIM = theta_apo + (theta_D - theta_apo) / (R_a - R_a_apo)*(res.R_amin - R_a_apo);
				res.R_amin = length(opt.R_LS) + opt.h_2amin;
				if (dt_CAN >= dt_CSI)
				{
					dt_CAN = 0.0;
				}

				OrbMech::LinearLeastSquares(Phase_Table, A_ins_Table, res.K1, res.J1);
				res.DEDA227 = res.K1;
				res.DEDA224 = res.J1;
				Phase_Table.clear();
				A_ins_Table.clear();
				K3 = true;
			}
			else
			{
				OrbMech::LinearLeastSquares(Phase_Table, A_ins_Table, res.K2, res.J2);
				stop = true;
			}
		}
		else
		{
			t_Abort_Table.push_back(dt_abort);
			dV_Abort_Table.push_back(Z_D_dot);
			Phase_Table.push_back(theta);
			A_ins_Table.push_back(A_Ins);
			theta_apo = theta_D;
			dt_abort += opt.dt_step;
			R_a_apo = R_a;
		}

		if (!opt.IsTwoSegment && Z_D_dot <= V_H_min)
		{
			stop = true;
		}

	} while (stop == false);

	//If we use the two segment logic, then everything has already been calculated
	if (opt.IsTwoSegment) return true;

	//Apollo 11: First 4 solutions from T vs. V_H table for cubic function
	if (dV_Abort_Table.size() < 4)
	{
		return false;
	}

	double t_Abort[4] = { t_Abort_Table[0], t_Abort_Table[1] , t_Abort_Table[2] , t_Abort_Table[3] };
	double dV_Abort[4] = { dV_Abort_Table[0], dV_Abort_Table[1] , dV_Abort_Table[2] , dV_Abort_Table[3] };
	double coeff[4];
	OrbMech::CubicInterpolation(t_Abort, dV_Abort, coeff);

	res.ABTCOF1 = coeff[0];
	res.ABTCOF2 = coeff[1];
	res.ABTCOF3 = coeff[2];
	res.ABTCOF4 = coeff[3];

	OrbMech::LinearLeastSquares(Phase_Table, A_ins_Table, res.DEDA227, res.DEDA224);

	return true;
}

bool RTCC::LunarLiftoffTimePredictionDT(const LLTPOpt &opt, LunarLaunchTargetingTable &res)
{
	if (opt.sv_CSM.RBI != BODY_MOON) return false;

	TwoImpulseOpt lamman;
	TwoImpulseResuls lamres;
	EphemerisData sv_TH, sv_LS, sv_INS, sv_TPI, sv_TPF;
	VECTOR3 R_BO, V_BO, R_LS;
	double lng_LO, T_LO, T_LO_0, S, DV_Z0, DV_Z, T_TPI, T_INS, V_H;
	int i, k, I_max, AEGERR;

	lamman.mode = 5;
	sv_INS.RBI = sv_LS.RBI = BODY_MOON;

	//Advance CSM to threshold time
	sv_TH = coast(opt.sv_CSM, opt.T_TH - opt.sv_CSM.GMT);

	//Compute first guess of CSM longitude at liftoff time
	AEGBlock aeg;

	aeg = SVToAEG(sv_TH, 0.0, 1.0, 1.0);
	PMMAEGS(aeg.Header, aeg.Data, aeg.Data); //Initialize

	lng_LO = opt.lng_LS; //TBD

	//Find longitude
	PMMTLC(aeg.Header, aeg.Data, aeg.Data, lng_LO, AEGERR, 0);
	if (AEGERR)
	{
		return false;
	}

	//Bias by 1 minute from CSM over LS
	T_LO = aeg.Data.TE - 60.0;
	sv_LS = coast(sv_TH, T_LO - sv_TH.GMT);

	i = 0;
	k = 0;
	V_H = 5540.0*0.3048;
	I_max = 10;
	R_LS = OrbMech::r_from_latlong(opt.lat_LS, opt.lng_LS, opt.R_LS);

	T_TPI = T_LO + opt.dt_PF + opt.dt_INS_TPI;

	do
	{
		PMMENS(sv_LS.R, sv_LS.V, opt.dt_PF, opt.Y_S, opt.alpha_PF, opt.h_INS, V_H, opt.V_Z_NOM, T_LO, R_LS, R_BO, V_BO, T_INS);
		sv_INS.R = R_BO;
		sv_INS.V = V_BO;
		sv_INS.GMT = T_INS;
		if (k < 1)
		{
			lamman.T1 = T_INS;
			lamman.T2 = T_TPI;
			lamman.PhaseAngle = opt.dTheta_TPI;
			lamman.DH = opt.DH_TPI;
		}
		else
		{
			lamman.T1 = T_TPI;
			lamman.T2 = -1.0;
			lamman.WT = opt.WT;
			lamman.PhaseAngle = 0.0;
			lamman.DH = 0.0;
		}
		
		lamman.sv_A = coast(sv_INS, lamman.T1 - sv_INS.GMT);
		lamman.sv_P = coast(sv_LS, lamman.T1 - sv_LS.GMT);

		PMSTICN(lamman, lamres);
		if (lamres.SolutionFound == false)
		{
			return false;
		}
		DV_Z = lamres.dV_LVLH.z;

		if (k >= 1) break;
		if (abs(DV_Z) <= 0.1*0.3048)
		{
			k = 1;
			V_H = V_H + lamres.dV_LVLH.x;
			continue;
		}
		if (i > I_max)
		{
			return false;
		}
		i++;
		if (i == 1)
		{
			S = 0.455 / 0.3048;
		}
		else
		{
			S = (T_LO - T_LO_0) / (DV_Z - DV_Z0);
		}
		DV_Z0 = DV_Z;
		T_LO_0 = T_LO;
		T_LO = T_LO - S * DV_Z;
		T_TPI = T_TPI - S * DV_Z;
	} while (i <= I_max);

	res.GETLOR = GETfromGMT(T_LO);
	res.GET_TPI = GETfromGMT(T_TPI);
	res.GET_TPF = lamres.T2;
	res.DV_TPI = length(lamres.dV);
	res.DV_TPF = length(lamres.dV2);
	res.DV_TPI_LVLH = lamres.dV_LVLH;
	res.DV_TPF_LVLH = lamres.dV_LVLH2;
	res.VH = V_H;

	sv_TPI = coast(sv_INS, T_TPI - sv_INS.GMT);
	sv_TPI.V = sv_TPI.V + lamres.dV;
	sv_TPF = coast(sv_TPI, GMTfromGET(lamres.T2) - sv_TPI.GMT);
	sv_TPF.V = sv_TPF.V + lamres.dV2;
	
	double RA_TPI, RP_TPI, RA_TPF, RP_TPF, RA_T, RP_T;
	OrbMech::periapo(sv_TPI.R, sv_TPI.V, OrbMech::mu_Moon, RA_TPI, RP_TPI);
	OrbMech::periapo(sv_TPF.R, sv_TPF.V, OrbMech::mu_Moon, RA_TPF, RP_TPF);
	OrbMech::periapo(sv_LS.R, sv_LS.V, OrbMech::mu_Moon, RA_T, RP_T);

	res.HA_TPI = RA_TPI - BZLAND.rad[RTCC_LMPOS_BEST];
	res.HP_TPI = RP_TPI - BZLAND.rad[RTCC_LMPOS_BEST];
	res.HA_TPF = RA_TPF - BZLAND.rad[RTCC_LMPOS_BEST];
	res.HP_TPF = RP_TPF - BZLAND.rad[RTCC_LMPOS_BEST];
	res.HA_T = RA_T - BZLAND.rad[RTCC_LMPOS_BEST];
	res.HP_T = RP_T - BZLAND.rad[RTCC_LMPOS_BEST];

	return true;
}

void RTCC::papiWriteScenario_REFS(FILEHANDLE scn, char *item, int tab, int i, REFSMMATData in)
{
	char buffer[256];

	sprintf(buffer, "  %s %d %d %d %lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf", item, tab, i, in.ID, in.GMT, in.REFSMMAT.m11, in.REFSMMAT.m12, in.REFSMMAT.m13, in.REFSMMAT.m21, in.REFSMMAT.m22,
		in.REFSMMAT.m23, in.REFSMMAT.m31, in.REFSMMAT.m32, in.REFSMMAT.m33);
	oapiWriteLine(scn, buffer);
}

bool RTCC::papiReadScenario_REFS(char *line, char *item, int &tab, int &i, REFSMMATData &out)
{
	char buffer[256];

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			REFSMMATData v;
			if (sscanf(line, "%s %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", buffer, &tab, &i, &v.ID, &v.GMT, &v.REFSMMAT.m11, &v.REFSMMAT.m12, &v.REFSMMAT.m13, &v.REFSMMAT.m21, &v.REFSMMAT.m22,
				&v.REFSMMAT.m23, &v.REFSMMAT.m31, &v.REFSMMAT.m32, &v.REFSMMAT.m33) == 14) {
				out = v;
				return true;
			}
		}
	}
	return false;
}

void RTCC::PMXSPT(std::string source, int n)
{
	std::vector<std::string> message;
	std::string temp1, temp2, temp3;
	char Buffer[128];

	switch (n)
	{
	case 1:
		message.push_back("MANEUVER PRIOR TO PRESENT TIME.");
		break;
	case 2:
		message.push_back("MANEUVER TO BE REPLACED NOT IN THE MPT.");
		break;
	case 3:
		message.push_back("MANEUVER TO BE REPLACED OVERLAPS");
		message.push_back("ANOTHER MANEUVER.");
		break;
	case 4:
		message.push_back("MANEUVER PRIOR TO FROZEN MANEUVER.");
		break;
	case 5:
		message.push_back("MPT IS FULL - REQUESTED MANEUVER TRANSFER REJECTED.");
		break;
	case 6:
		message.push_back("INVALID CONFIGURATION CODE OR THRUSTER CODE - MPT UNCHANGED.");
		break;
	case 7:
		sprintf_s(Buffer, "PERIFOCUS CONVERGENCE FAILED FOR MANEUVER %d - FINAL SOLUTION ACCEPTED.", RTCCONLINEMON.IntBuffer[0]);
		message.push_back(Buffer);
		break;
	case 8:
		sprintf_s(Buffer, "AEG ERROR COMPUTING MANEUVER %d - NO SOLUTION AVAILABLE.", RTCCONLINEMON.IntBuffer[0]);
		message.push_back(Buffer);
		break;
	case 9:
		sprintf_s(Buffer, "PERIFOCUS ADJUST MANEUVER %d IS AN ILLEGAL REQUEST.", RTCCONLINEMON.IntBuffer[0]);
		message.push_back(Buffer);
		break;
	case 10:
		message.push_back("MANEUVER EXECUTED - CANNOT FREEZE");
		break;
	case 11:
		message.push_back("GOOD GPM MANEUVER.");
		break;
	case 12:
		message.push_back("ILLEGAL REQUEST. MANEUVER CANNOT BE PERFORMED IN THIS ORBIT - PROCESSING HALTED.");
		break;
	case 13:
		message.push_back("ILLEGAL REQUEST. MANEUVER CANNOT BE PERFORMED AT THIS POINT");
		message.push_back("IN ORBIT - PROCESSING HALTED.");
		break;
	case 14:
		message.push_back("AEG FAILED TO CONVERGE - PROCESSING CONTINUED.");
		break;
	case 15:
		message.push_back("UNRECOVERABLE AEG ERROR - PROCESSING HALTED");
		break;
	case 16:
		message.push_back("ESSENTIALLY CIRCULAR ORBIT. THRESHOLD HEIGHT RETURNED");
		break;
	case 17:
		message.push_back("UNRECOVERABLE AEG ERROR - PROCESSING HALTED");
		break;
	case 18:
		message.push_back("REQUESTED HEIGHT NOT IN ORBIT. NEAREST VALUE RETURNED.");
		break;
	case 19:
		message.push_back("FAILED TO CONVERGE ON REQUESTED HEIGHT");
		break;
	case 23:
		message.push_back("TIME ON M50 MED PRIOR TO END OF LAST EXECUTED");
		message.push_back("MANEUVER - MPT UNCHANGED");
		break;
	case 29:
		message.push_back("REQUESTED TWO-IMPULSE SOLUTION");
		message.push_back("NUMBER NOT AVAILABLE");
		break;
	case 30:
		message.push_back("FAILED TO CONVERGE ON ELEVATION ANGLE -");
		message.push_back("NO PLAN COMPUTED.");
		break;
	case 32:
		message.push_back("ILLEGAL ENTRY. CODE = 32");
		break;
	case 33:
		message.push_back("MPT REFLECTS REQUESTED CHANGES -");
		message.push_back("NO VECTOR AVAILABLE FOR " + RTCCONLINEMON.TextBuffer[0] + " TRAJECTORY UPDATE.");
		break;
	case 35:
		message.push_back("UNABLE TO MOVE VECTOR TO " + RTCCONLINEMON.TextBuffer[0] + " -");
		message.push_back("NO TRAJECTORY UPDATE.");
		break;
	case 36:
		message.push_back("EXECUTION VECTOR FOR MANEUVER");
		OnlinePrintTimeDDHHMMSS(RTCCONLINEMON.DoubleBuffer[0], temp1);
		message.push_back(RTCCONLINEMON.TextBuffer[0] + ", BEGIN TIME = " + temp1);
		OnlinePrintTimeDDHHMMSS(RTCCONLINEMON.DoubleBuffer[1], temp1);
		sprintf_s(Buffer, "%.1lf", RTCCONLINEMON.DoubleBuffer[2]);
		temp2.assign(Buffer);
		sprintf_s(Buffer, "%.1lf", RTCCONLINEMON.DoubleBuffer[3]);
		temp3.assign(Buffer);
		message.push_back("T = " + temp1 + " AREA = " + temp2 + " WEIGHT = " + temp3);
		sprintf_s(Buffer, "R = %.8lf %.8lf %.8lf", RTCCONLINEMON.VectorBuffer[0].x, RTCCONLINEMON.VectorBuffer[0].y, RTCCONLINEMON.VectorBuffer[0].z);
		temp1.assign(Buffer);
		message.push_back(temp1);
		sprintf_s(Buffer, "V = %.8lf %.8lf %.8lf", RTCCONLINEMON.VectorBuffer[1].x, RTCCONLINEMON.VectorBuffer[1].y, RTCCONLINEMON.VectorBuffer[1].z);
		temp1.assign(Buffer);
		message.push_back(temp1);
		break;
	case 37:
		message.push_back("DELETION TIME IS PRIOR TO A FROZEN MANEUVER - MPT UNCHANGED.");
		break;
	case 38:
		message.push_back("UNUSABLE MANEUVER FOR TRANSFER");
		break;
	case 39:
		message.push_back("UNABLE TO FETCH VECTOR FOR " + RTCCONLINEMON.TextBuffer[0]);
		message.push_back("MPT UNCHANGED");
		break;
	case 40:
		message.push_back("MANEUVER DOES NOT EXIST");
		break;
	case 41:
		message.push_back("INVALID VECTOR ID - CANNOT FREEZE");
		break;
	case 42:
		message.push_back("MANEUVER NOT EXECUTED - CANNOT DELETE AS HISTORY");
		break;
	case 43:
		message.push_back("MANEUVER EXECUTED - CANNOT DELETE AS FUTURE");
		break;
	case 44:
		message.push_back("MANEUVER PRIOR TO FROZEN VECTOR TIME - CANNOT FREEZE ON THIS VECTOR");
		break;
	case 45:
		message.push_back("MANEUVER NOT FROZEN - CANNOT UNFREEZE");
		break;
	case 46:
		message.push_back("MANEUVER EXECUTED - CANNOT UNFREEZE");
		break;
	case 47:
		message.push_back("MANEUVER FROZEN - CANNOT DELETE");
		break;
	case 48:
		message.push_back("ILLEGAL ENTRY");
		break;
	case 49:
		message.push_back("UNABLE TO FETCH VECTOR FOR RENDEZVOUS PLANNING REQUEST");
		break;
	case 50:
		sprintf_s(Buffer, "ERROR CODE %d FROM POWERED FLIGHT ITERATOR", RTCCONLINEMON.IntBuffer[0]);
		message.push_back(Buffer);
		message.push_back("MNVR TRANSFERRED USING BEST PARAMETERS AVAILABLE");
		break;
	case 51:
		message.push_back("UNRECOVERABLE ERROR FROM EMMENI - MPT UNCHANGED");
		break;
	case 52:
		message.push_back("FINITE BURN OF MNVR IS PRIOR TO PRESENT TIME OR");
		message.push_back("OVERLAPS ANOTHER MNVR - MPT UNCHANGED");
		break;
	case 53:
		message.push_back("REQUESTED MANEUVER IS NOT IN MPT - M58 MED IGNORE");
		break;
	case 54:
		message.push_back("REQUESTED MANEUVER IS EXECUTED - M58 MED IGNORE");
		break;
	case 55:
		message.push_back("REQUEST IS FOR TLI/ASC MANEUVER - M58 MED IGNORE");
		break;
	case 56:
		message.push_back("REQUESTED MANEUVER IS FROZEN - M58 MED IGNORE");
		break;
	case 57:
		message.push_back("UNABLE TO OBTAIN LIBRATION MATRIX. PROCESSING HALTED.");
		break;
	case 58:
		message.push_back("MANEUVER TO BE CONFIRMED HAS NOT BEEN EXECUTED - MPT UNCHANGED.");
		break;
	case 59:
		message.push_back("FAILURE TO CONVERGE. PLANS DELETED.");
		break;
	case 60:
		sprintf_s(Buffer, "ALL SUBSEQUENCE MANEUVERS TO %d MUST BE REPLACED/DELETED/CONFIRMED", RTCCONLINEMON.IntBuffer[0]);
		message.push_back(Buffer);
		message.push_back("IN CHRONOLOGICAL ORDER DUE TO THIS CONFIGURATION CHANGE.");
		break;
	case 61:
		sprintf_s(Buffer, "VECTOR FOR FREEZE REQUEST IS WITHIN RANGE OF MANEUVER %d", RTCCONLINEMON.IntBuffer[0]);
		message.push_back(Buffer);
		message.push_back("- CANNOT FREZE ON THIS VECTOR.");
		break;
	case 62:
		message.push_back("VECTOR FOR FREEZE REQUEST IS PRIOR TO FROZEN TLI -");
		message.push_back("CANNOT FREEZE POST_TLI MANEUVER ON THIS VECTOR.");
		break;
	case 63:
		sprintf_s(Buffer, "INVALID CONFIGURATION CODE (%d)", RTCCONLINEMON.IntBuffer[0]);
		message.push_back(Buffer);
		break;
	case 64:
		message.push_back("THE AMOUNT OF TIME TO UPDATE THE ELEMENTS EXCEEDS FOUR DAYS.");
		break;
	case 65:
		message.push_back("AEG/PIATSU ERROR - LONGITUDE OF ASCENDING NODE HAS BEEN");
		message.push_back("ZEROED FOR MANEUVER" + RTCCONLINEMON.TextBuffer[0]);
		break;
	case 66:
		message.push_back("MANEUVER " + RTCCONLINEMON.TextBuffer[0] + " OVERLAPS PREVIOUS MPT MANEUVER - WILL");
		message.push_back("BE SKIPPED BY INTEGRATOR.");
		break;
	case 67:
		message.push_back("ERROR FROM PMMDAN - DMT DAY/NIGHT INFORMATION ZEROED FOR MANEUVER " + RTCCONLINEMON.TextBuffer[0]);
		break;
	case 68:
		message.push_back("UNABLE TO ADVANCE TO SELENOGRAPHIC");
		message.push_back("ARGUMENT OF LATITUDE. PROCESSING HALTED.");
		break;
	case 69:
		message.push_back("UNABLE TO CONVERT VECTORS FROM MEAN TO TRUE.");
		break;
	case 70:
		message.push_back("NO FREEFLIGHT VECTOR AT IGNITION FOR MANEUVER " + RTCCONLINEMON.TextBuffer[0]);
		message.push_back("- EPHEMERIS VECTOR WILL BE USED.");
		break;
	case 71:
		message.push_back("DELETE MPT MANEUVERS PRIOR TO ENTERING M55 MED");
		break;
	case 72:
		message.push_back("ERROR FROM ENCKE N.I. UNABLE TO MOVE VECTOR TO THRESHOLD TIME");
		message.push_back("OR TO GENERATE SEARCH EPHEMERIS - PROCESSING TERMINATED.");
		break;
	case 73:
		message.push_back("MISSION PLAN TABLE IS BEING UPDATED.");
		break;
	case 74:
		message.push_back("CORRECTIVE COMBINATION SOLUTION REENTERED");
		message.push_back("FOR TRANSFER OR SINGLE SOLUTION REQUEST.");
		break;
	case 75:
		message.push_back("VECTOR FETCH TIME IS PRIOR TO TIME");
		message.push_back("OF LAST FROZEN MANEUVER.");
		break;
	case 76:
		message.push_back("UNRECOVERABLE AEG ERROR.");
		break;
	case 77:
		message.push_back("CALCULATED TIME OUTSIDE LIMITS OF IGNITION");
		message.push_back("SEARCH EPHEMERIS - PROCESSING TERMINATED.");
		break;
	case 78:
		message.push_back("MLD DATA TABLE HAS NOT BEEN INITIALIZED");
		break;
	case 79:
		message.push_back("INVALID IGNITION TIME FOR S-IVB TLI MANEUVER");
		message.push_back("- MPT UNCHANGED");
		break;
	case 81:
		message.push_back("FAILED TO CONVERGE.");
		break;
	case 82:
		message.push_back("REENTERED.");
		break;
	case 83:
		message.push_back("FAILED TO CONVERGE.");
		break;
	case 84:
		message.push_back("PRECESSION NUTATION MATRIX UNAVAILABLE -");
		message.push_back("PROCESSING TERMINATED");
		break;
	case 85:
		message.push_back("TARGET PARAMETERS UNAVAILABLE FOR");
		message.push_back("SPECIFIED LAUNCH DAY - PROCESSING TERMINATED");
		break;
	case 86:
		message.push_back("TARGET PARAMETERS UNAVAILABLE FOR");
		message.push_back("SPECIFIED INJECTION OPPORTUNITY - PROCESSING TERMINATED");
		break;
	case 92:
		message.push_back("CONSTRAINT " + RTCCONLINEMON.TextBuffer[0] + " VIOLATED IN");
		message.push_back("COELLIPTIC SEQUENCE.");
		break;
	case 98:
		message.push_back("DIFFERENCE BETWEEN ACTUAL AND NOMINAL");
		message.push_back("LIFTOFF TIME OUTSIDE LIMITS OF LAUNCH AZIMUTH POLYNOMIAL");
		break;
	case 101:
		message.push_back("SPQ PLAN FAILED TO CONVERGE ON OPTIMUM");
		message.push_back("CSI - PLAN RETAINED");
		break;
	case 102:
		message.push_back("NOMINAL TIME OF LIFTOFF FOR SPECIFIED LAUNCH DAY");
		message.push_back("UNAVAILABLE - PROCESSING TERMINATED.");
		break;
	case 108:
		message.push_back("ERROR RETURNED FROM PMMAPD - INVALID APOFOCUS / PERIFOCUS");
		break;
	case 120:
		message.push_back("UNABLE TO CONVERT SPHERICAL ELEMENTS TO R AND V VECTORS.");
		break;
	case 121:
		message.push_back("DIFFERENCE BETWEEN ACTUAL AND NOMINAL");
		message.push_back("LIFTOFF TIME OUTSIDE LIMITS OF LAUNCH AZIMUTH POLYNOMIAL");
		break;
	case 122:
		{
			int hh, mm;
			double ss;
			OrbMech::SStoHHMMSS(RTCCONLINEMON.DoubleBuffer[0], hh, mm, ss, 0.01);
			sprintf_s(Buffer, "LIFTOFF TIME(GMT) = 00/%02d/%02d/%05.2lf", hh, mm, ss);
			message.push_back(Buffer);
			sprintf_s(Buffer, "LAUNCH AZIMUTH (DEG) = %.6lf", RTCCONLINEMON.DoubleBuffer[1]);
			message.push_back(Buffer);
			OrbMech::SStoHHMMSS(RTCCONLINEMON.DoubleBuffer[2], hh, mm, ss, 0.01);
			sprintf_s(Buffer, "VECTOR TIME(HRS) = 00/%02d/%02d/%05.2lf CS = ECT", hh, mm, ss);
			message.push_back(Buffer);
			sprintf_s(Buffer, "   R(ER) = %.10lf %.10lf %.10lf", RTCCONLINEMON.VectorBuffer[0].x, RTCCONLINEMON.VectorBuffer[0].y, RTCCONLINEMON.VectorBuffer[0].z);
			message.push_back(Buffer);
			sprintf_s(Buffer, "V(ER/HR) = %.10lf %.10lf %.10lf", RTCCONLINEMON.VectorBuffer[1].x, RTCCONLINEMON.VectorBuffer[1].y, RTCCONLINEMON.VectorBuffer[1].z);
			message.push_back(Buffer);
		}
		break;
	case 124:
		message.push_back("COAST INTEGRATOR UNABLE TO OBTAIN PERICYNTHION POINT");
		message.push_back("FOR LOI REQUEST - LOI SOLUTIONS ARE UNOBTAINABLE");
		break;
	case 125:
		message.push_back("LUNAR APPROACH HYPERBOLA HAS");
		message.push_back("IMPACTING TRAJECTORY - LOI SOLUTIONS ARE UNOBTAINABLE");
		break;
	case 126:
		message.push_back("LUNAR APPROACH HYPERBOLIC PERICYNTHION GREATER THAN");
		message.push_back("REQUESTED LPO APOLUNE - LOI SOLUTIONS ARE UNOBTAINABLE");
		break;
	case 135:
		message.push_back("PMMAPD ERROR RETURN IN OBTAINING CURRENT");
		message.push_back("APOFOCUS/PERIFOCUS. PROCESSING HALTED.");
		break;
	case 136:
		message.push_back("PMMAPD ERROR RETURN IN OBTAINING RESULTANT");
		message.push_back("APOFOCUS/PERIFOCUS. PROCESSING HALTED.");
		break;
	case 137:
		message.push_back("RESULTANT ORBIT NON/ELLIPTICAL. PROCESSING HALTED.");
		break;
	case 200:
		message.push_back("ITERATION FAILURE, MVR TRANSFERRED");
		message.push_back("USING BEST PARAMETERS AVAILABLE");
		break;
	case 201:
		message.push_back(RTCCONLINEMON.TextBuffer[0]);
		break;
	default:
		return;
	}
	PMXSPT(source, message);
}

void RTCC::PMXSPT(std::string source, std::vector<std::string> message)
{
	OnlinePrint(source, message);
}

void RTCC::OnlinePrint(const std::string &source, const std::vector<std::string> &message)
{
	if (message.size() == 0)
	{
		return;
	}

	OnlineMonitorMessage data;
	std::string temp, temp2;

	double gmt;
	if (SystemParameters.GMTBASE == 0.0)
	{
		gmt = 0.0;
	}
	else
	{
		gmt = RTCCPresentTimeGMT();
	}
	OnlinePrintTimeDDHHMMSS(gmt, temp);
	data.message.push_back(temp + " )" + source + "( " + message[0]);
	for (unsigned i = 1;i < message.size();i++)
	{
		data.message.push_back(message[i]);
	}

	//Also write to text file
	if (RTCCONLINEMON.data.size() == 0)
	{
		rtccdebug.open("RTCCDebug.log", std::ofstream::trunc);
	}
	else
	{
		rtccdebug.open("RTCCDebug.log", std::ofstream::app);
	}

	RTCCONLINEMON.data.push_front(data);
	
	for (unsigned i = 0;i < data.message.size();i++)
	{
		rtccdebug << data.message[i] << endl;
	}
	rtccdebug.close();

	if (RTCCONLINEMON.data.size() >= 9)
	{
		RTCCONLINEMON.data.pop_back();
	}
}

void RTCC::GMSPRINT(std::string source, int n)
{
	std::vector<std::string> message;

	switch (n)
	{
	case 1:
		message.push_back("RECYCLE TO ORBIT L.S.");
		break;
	case 23:
		message.push_back("P32, ADD...STA ALREADY IN TABLE");
		break;
	case 24:
		message.push_back("P32, ADD...SORRY TABLE FULL");
		break;
	case 25:
		message.push_back("P32, MOD/DEL...STA NOT IN TABLE");
		break;
	case 26:
		message.push_back("LATE Q FROM HIGH-SPEED");
		break;
	case 27:
		message.push_back("INVALID P60 - IGNORED");
		break;
	case 28:
		message.push_back("INVALID ID - IGNORED");
		break;
	case 29:
		message.push_back("INVALID P81 - IGNORED");
		break;
	case 30:
		message.push_back("NO/GO GMSMED - S RTCHNGE");
		break;
	case 31:
		message.push_back("P13 CANNOT BE ROTATED TO ECI -- IGNORE");
		break;
	case 32:
		message.push_back("P32 E/M CONFLICT, RELEASE TABLE AND RE-ENTER MED");
		break;
	case 40:
		message.push_back("GMGPMED: P80 HAS INVALID DATE");
		break;
	case 46:
		message.push_back("P10, ..., TRAJ $ ALLOWED ONLY IN");
		message.push_back("NO PHASE, PRELCH, PRELCH2 (L.S.)");
		break;
	case 51:
		message.push_back("MED " + RTCCONLINEMON.TextBuffer[0]);
		break;
	default:
		return;
	}
	GMSPRINT(source, message);
}

void RTCC::GMSPRINT(std::string source, std::vector<std::string> message)
{
	OnlinePrint(source, message);
}

void RTCC::OnlinePrintTimeDDHHMMSS(double TIME_SEC, std::string &time)
{
	char Buffer[128];
	double days, hours, minutes, seconds;
	if (TIME_SEC < 0)
	{
		TIME_SEC = 0.0;
	}
	days = trunc(TIME_SEC / 86400.0);
	hours = trunc((TIME_SEC - 86400.0*days) / 3600.0);
	minutes = trunc((TIME_SEC - 86400.0 * days - 3600.0 * hours) / 60.0);
	seconds = TIME_SEC - 86400.0*days - 3600.0*hours - 60.0*minutes;

	sprintf(Buffer, "%02.0lf/%02.0lf/%02.0lf/%05.2lf", days, hours, minutes, seconds);
	time.assign(Buffer);
}

void RTCC::OnlinePrintTimeHHHMMSS(double TIME_SEC, std::string &time)
{
	char Buffer[128];
	double hours, minutes, seconds;
	if (TIME_SEC < 0)
	{
		TIME_SEC = 0.0;
	}
	hours = trunc(TIME_SEC / 3600.0);
	minutes = trunc((TIME_SEC - 3600.0 * hours) / 60.0);
	seconds = TIME_SEC - 3600.0*hours - 60.0*minutes;

	sprintf(Buffer, "%03.0lf/%02.0lf/%05.2lf", hours, minutes, seconds);
	time.assign(Buffer);
}

void RTCC::EMGGPCHR(double lat, double lng, double alt, int body, double GHA, StationData *stat)
{
	stat->lng = lng;
	stat->lat_geoc = stat->lat_geod = lat;
	stat->lng_iner = lng + GHA;
	OrbMech::normalizeAngle(stat->lng_iner);
	stat->sin_lng = sin(lng);
	stat->cos_lng = cos(lng);
	stat->sin_lat_geoc = stat->sin_lat_geod = sin(lat);
	stat->cos_lat_geoc = stat->cos_lat_geod = cos(lat);
	if (body == BODY_EARTH)
	{
		stat->R_E = OrbMech::R_Earth;
	}
	else
	{
		stat->R_E = OrbMech::R_Moon;
	}
	stat->H = alt;
	stat->R_S = stat->R_E + stat->H;
	stat->R_sin_dlat = stat->R_cos_dlat = 0.0;
	stat->R_E_cos_lat = stat->R_E*cos(stat->lat_geoc) + stat->H*cos(stat->lat_geod);
	stat->R_E_sin_lat = stat->R_E*sin(stat->lat_geoc) + stat->H*sin(stat->lat_geod);
}

void RTCC::EMMDYNEL(EphemerisData sv, TimeConstraintsTable &tab)
{
	VECTOR3 H, E, N, K;
	double mu, eps, v, r, lng, lat, fpa, azi, h, r_apo, r_peri, R_E;

	if (sv.RBI == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}

	OrbMech::rv_from_adbar(sv.R, sv.V, r, v, lng, lat, fpa, azi);
	eps = v * v / 2.0 - mu / r;
	H = crossp(sv.R, sv.V);
	E = crossp(sv.V, H) / mu - sv.R / r;
	K = _V(0, 0, 1);
	N = crossp(K, H);
	if (sv.RBI == BODY_EARTH)
	{
		R_E = OrbMech::R_Earth;
		lng -= OrbMech::w_Earth*sv.GMT;
		OrbMech::normalizeAngle(lng, false);
	}
	else
	{
		R_E = BZLAND.rad[RTCC_LMPOS_BEST];
	}
	h = r - R_E;

	tab.a = -mu / (2.0*eps);
	tab.AoP = acos2(dotp(unit(N), unit(E)));
	if (E.z < 0)
	{
		tab.AoP = PI2 - tab.AoP;
	}
	tab.azi = azi;
	tab.e = length(E);
	tab.gamma = PI05 - fpa;
	tab.h = h;
	tab.i = acos(H.z / length(H));
	tab.l = pow(length(H), 2) / mu;
	tab.lat = lat;
	tab.lng = lng;
	tab.RA = acos(N.x / length(N));
	if (N.y < 0)
	{
		tab.RA = PI2 - tab.RA;
	}
	tab.sv_present = sv;
	tab.T0 = OrbMech::period(sv.R, sv.V, mu);
	tab.TA = acos2(dotp(unit(E), unit(sv.R)));
	if (dotp(sv.R, sv.V) < 0)
	{
		tab.TA = PI2 - tab.TA;
	}
	if (tab.e < 1.0)
	{
		//Elliptical
		tab.MA = OrbMech::TrueToMeanAnomaly(tab.TA, tab.e);
	}
	else if (tab.e > 1.0)
	{
		//Hyperbolic
		double F;

		F = log((sqrt(tab.e + 1.0) + sqrt(tab.e - 1.0)*tan(tab.TA / 2.0)) / (sqrt(tab.e + 1.0) - sqrt(tab.e - 1.0)*tan(tab.TA / 2.0))); //Hyperbolic eccentric anomaly
		tab.MA = tab.e*sinh(F) - F;
	}
	else
	{
		//Parabolic
		tab.MA = 0.5*tan(tab.TA / 2.0) + 1.0 / 6.0*pow(tan(tab.TA / 2.0), 3);
	}

	tab.V = v;

	OrbMech::periapo(sv.R, sv.V, mu, r_apo, r_peri);
	tab.h_a = r_apo - R_E;
	tab.h_p = r_peri - R_E;
}

void RTCC::EMMRMD(int Veh1, int Veh2, double get, double dt, int refs, int axis, int ref_body, int mode, VECTOR3 Att, double PYRGET)
{
	double gmt_cur = RTCCPresentTimeGMT();
	double get_cur = GETfromGMT(gmt_cur);
	EZRMDT.error = "";
	if (get < get_cur)
	{
		EZRMDT.error = "MED OUTDATED";
		return;
	}
	double gmt = GMTfromGET(get);
	OrbitEphemerisTable *ChaTab, *TgtTab;
	MissionPlanTable *ChaMPT, *TgtMPT, *CSMMPT, *LMMPT;
	REFSMMATData refsdata;
	VECTOR3 X_P, Y_P, Z_P, X_B, Y_B, Z_B, X_B_selen, Y_B_selen, Z_B_selen;
	int CSI;

	CSMMPT = &PZMPTCSM;
	LMMPT = &PZMPTLEM;
	if (Veh1 == 1)
	{
		ChaTab = &EZEPH1;
		ChaMPT = &PZMPTCSM;
		refsdata = EZJGMTX1.data[refs - 1];
	}
	else
	{
		ChaTab = &EZEPH2;
		ChaMPT = &PZMPTLEM;
		refsdata = EZJGMTX3.data[refs - 1];
	}
	if (Veh2 == 1)
	{
		TgtTab = &EZEPH1;
		TgtMPT = &PZMPTCSM;
	}
	else
	{
		TgtTab = &EZEPH2;
		TgtMPT = &PZMPTLEM;
	}
	if (ChaTab->EPHEM.Header.TUP == 0 || TgtTab->EPHEM.Header.TUP == 0)
	{
		EZRMDT.error = "TIME NOT IN EPHEM";
		return;
	}
	if (gmt < ChaTab->EPHEM.Header.TL && gmt < TgtTab->EPHEM.Header.TL)
	{
		EZRMDT.error = "TIME NOT IN EPHEM";
		return;
	}
	if (ChaTab->EPHEM.Header.TUP < 0 || TgtTab->EPHEM.Header.TUP < 0)
	{
		EZRMDT.error = "UPDATE IN PROCESS";
		return;
	}
	if (ChaTab->EPHEM.Header.TUP != ChaMPT->CommonBlock.TUP)
	{
		EZRMDT.error = "INCONSISTENT TABLES";
		return;
	}
	if (TgtTab->EPHEM.Header.TUP != TgtMPT->CommonBlock.TUP)
	{
		EZRMDT.error = "INCONSISTENT TABLES";
		return;
	}
	if (mode == 2)
	{
		EphemerisDataTable2 EPHEM;
		ManeuverTimesTable MANTIMES;
		LunarStayTimesTable LUNSTAY;
		ELFECH(gmt, 1, 0, RTCC_MPT_LM, EPHEM, MANTIMES, LUNSTAY);

		if (gmt < LUNSTAY.LunarStayBeginGMT || gmt >LUNSTAY.LunarStayEndGMT)
		{
			EZRMDT.error = "TIME NOT WITHIN LUNAR STAY PERIOD";
			return;
		}
	}
	if (ref_body == BODY_EARTH)
	{
		CSI = 0;
	}
	else
	{
		CSI = 2;
	}
	EZRMDT.CSMSTAID = CSMMPT->StationID;
	EZRMDT.LMSTAID = LMMPT->StationID;
	EZRMDT.CSMGMTV = CSMMPT->GMTAV;
	EZRMDT.CSMGETV = GETfromGMT(EZRMDT.CSMGMTV);
	EZRMDT.LMGMTV = LMMPT->GMTAV;
	EZRMDT.LMGETV = GETfromGMT(EZRMDT.LMGMTV);
	if (mode == 1)
	{
		EZRMDT.Mode = '1';
		EZRMDT.PETorSH = "/PET";
		EZRMDT.YDotorT = "/Y";
	}
	else
	{
		EZRMDT.Mode = '2';
		EZRMDT.PETorSH = "/SH";
		EZRMDT.YDotorT = "/T";

		MATRIX3 M_B;
		double PYRGMT;

		PYRGMT = GMTfromGET(PYRGET);
		M_B = GLMRTM(refsdata.REFSMMAT, Att.x, 2, Att.z, 3, Att.y, 1);
		ELVCNV(_V(M_B.m11, M_B.m12, M_B.m13), PYRGMT, 0, 2, 3, X_B_selen);
		ELVCNV(_V(M_B.m21, M_B.m22, M_B.m23), PYRGMT, 0, 2, 3, Y_B_selen);
		ELVCNV(_V(M_B.m31, M_B.m32, M_B.m33), PYRGMT, 0, 2, 3, Z_B_selen);
	}
	char Buffer[7];

	FormatREFSMMATCode(refs, refsdata.ID, Buffer);
	EZRMDT.REFSMMAT.assign(Buffer);

	if (axis == 1)
	{
		EZRMDT.AXIS = 'X';
	}
	else
	{
		EZRMDT.AXIS = 'Z';
	}
	if (Veh1 == 1)
	{
		EZRMDT.Pitch = 'I';
		EZRMDT.Yaw = 'M';
		EZRMDT.Roll = 'O';
		EZRMDT.VEH = "CSM";
	}
	else
	{
		EZRMDT.Pitch = 'I';
		EZRMDT.Yaw = 'O';
		EZRMDT.Roll = 'M';
		EZRMDT.VEH = "LM";
	}
	EZRMDT.GETR = SystemParameters.MCGREF * 3600.0;
	X_P = _V(refsdata.REFSMMAT.m11, refsdata.REFSMMAT.m12, refsdata.REFSMMAT.m13);
	Y_P = _V(refsdata.REFSMMAT.m21, refsdata.REFSMMAT.m22, refsdata.REFSMMAT.m23);
	Z_P = _V(refsdata.REFSMMAT.m31, refsdata.REFSMMAT.m32, refsdata.REFSMMAT.m33);
	EZRMDT.solns = 0;
	ELVCTRInputTable intab;
	ELVCTROutputTable2 outtab;
	EphemerisData2 sv_cha, sv_tgt;
	double r_c, r_t, theta1, x, y, z, sin_phi, A, rho, rho_dot, E, P, Y, R, P_apo, Y_apo, R_apo;
	VECTOR3 R_c_u, R_t_u, H_c_u, H_t_u, Rho, Rho_dot;
	RelativeMotionDigitalsTableEntry empty;
	for (int i = 0;i < 11;i++)
	{
		EZRMDT.data[i] = empty;
	}
	for (int i = 0;i < 11;i++)
	{
		intab.GMT = gmt;
		intab.L = Veh1;
		ELVCTR(intab, outtab);
		if (outtab.ErrorCode)
		{
			gmt += dt;
			continue;
		}
		ELVCNV(outtab.SV, 0, CSI, sv_cha);
		sv_cha = outtab.SV;
		intab.GMT = gmt;
		intab.L = Veh2;
		ELVCTR(intab, outtab);
		if (outtab.ErrorCode)
		{
			gmt += dt;
			continue;
		}
		sv_tgt = outtab.SV;

		EZRMDT.data[EZRMDT.solns].GET = GETfromGMT(gmt);
		
		r_c = length(sv_cha.R);
		R_c_u = unit(sv_cha.R);
		H_c_u = unit(crossp(sv_cha.R, sv_cha.V));
		r_t = length(sv_tgt.R);
		R_t_u = unit(sv_tgt.R);
		H_t_u = unit(crossp(sv_tgt.R, sv_tgt.V));
		theta1 = atan2(dotp(H_t_u, crossp(R_t_u, R_c_u)), dotp(R_c_u, R_t_u));
		sin_phi = dotp(H_t_u, R_c_u);
		x = abs(r_t*theta1);
		y = abs(r_c*sin_phi);
		A = atan2(dotp(-H_c_u, R_t_u), dotp(H_c_u, crossp(R_c_u, R_t_u)));
		if (A < 0)
		{
			A += PI2;
		}
		Rho = sv_tgt.R - sv_cha.R;
		rho = length(Rho);
		if (rho < 1852.0*1e-10)
		{
			gmt += dt;
			EZRMDT.solns++;
			continue;
		}
		Rho_dot = sv_tgt.V - sv_cha.V;
		rho_dot = dotp(Rho, Rho_dot) / rho;
		E = asin(dotp(Rho, sv_cha.R) / rho / r_c);
		if (E < 0)
		{
			E += PI2;
		}
		z = abs(r_t - r_c);

		if (mode == 1)
		{
			EZRMDT.data[EZRMDT.solns].PETorShaft = abs(EZRMDT.data[EZRMDT.solns].GET - EZRMDT.GETR);
			EZRMDT.data[EZRMDT.solns].YdotorTrun = dotp(sv_cha.V, unit(crossp(sv_tgt.V, sv_tgt.R))) / 0.3048;
		}
		else
		{
			ELVCNV(X_B_selen, gmt, 0, 3, 2, X_B);
			ELVCNV(Y_B_selen, gmt, 0, 3, 2, Y_B);
			ELVCNV(Z_B_selen, gmt, 0, 3, 2, Z_B);

			VECTOR3 Rho_u;
			double Trun, Shaft;

			Rho_u = unit(Rho);
			Trun = asin(dotp(-Rho_u, Y_B));
			if (Trun < 0)
			{
				Trun += PI2;
			}
			Shaft = atan2(dotp(Rho_u, X_B), dotp(Rho_u, Z_B));
			if (Shaft < 0)
			{
				Shaft += PI2;
			}

			EZRMDT.data[EZRMDT.solns].PETorShaft = Shaft*DEG;
			EZRMDT.data[EZRMDT.solns].YdotorTrun = Trun*DEG;
		}

		EZRMDT.data[EZRMDT.solns].R = rho / 1852.0;
		EZRMDT.data[EZRMDT.solns].RDOT = rho_dot / 0.3048;
		EZRMDT.data[EZRMDT.solns].AZH = A * DEG;
		if (EZRMDT.data[EZRMDT.solns].AZH >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].AZH = 0.0;
		}
		EZRMDT.data[EZRMDT.solns].EL = E * DEG;
		EZRMDT.data[EZRMDT.solns].X = x / 1852.0;
		EZRMDT.data[EZRMDT.solns].Y = y / 1852.0;
		EZRMDT.data[EZRMDT.solns].Z = z / 1852.0;

		if (theta1 >= 0)
		{
			EZRMDT.data[EZRMDT.solns].XInd = 'L';
		}
		else
		{
			EZRMDT.data[EZRMDT.solns].XInd = 'T';
		}
		if (sin_phi >= 0)
		{
			EZRMDT.data[EZRMDT.solns].YInd = 'L';
		}
		else
		{
			EZRMDT.data[EZRMDT.solns].YInd = 'R';
		}
		if (r_t - r_c >= 0)
		{
			EZRMDT.data[EZRMDT.solns].ZInd = 'B';
		}
		else
		{
			EZRMDT.data[EZRMDT.solns].ZInd = 'A';
		}
		if (Veh1 == 1)
		{
			X_B = unit(Rho);
			//Y_B = unit(crossp(X_B, R_c_u));
			Y_B = unit(crossp(R_t_u, R_c_u));
			Z_B = crossp(X_B, Y_B);
			RLMPYR(X_P, Y_P, Z_P, X_B, Y_B, Z_B, P, Y, R);
			P_apo = P;
			Y_apo = Y;
			R_apo = R;
		}
		else
		{
			if (axis == 1)
			{
				X_B = unit(Rho);
				//Y_B = unit(crossp(X_B, R_c_u));
				Y_B = unit(crossp(R_t_u, R_c_u));
				Z_B = crossp(X_B, Y_B);
			}
			else
			{
				Z_B = unit(Rho);
				//Y_B = unit(crossp(Z_B, R_c_u));
				Y_B = unit(crossp(R_t_u,R_c_u));
				X_B = crossp(Y_B, Z_B);
			}
			EMGLMRAT(X_P, Y_P, Z_P, X_B, Y_B, Z_B, P, Y, R, P_apo, Y_apo, R_apo);
		}
		EZRMDT.data[EZRMDT.solns].Pitch = P * DEG;
		if (EZRMDT.data[EZRMDT.solns].Pitch >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].Pitch = 0.0;
		}
		EZRMDT.data[EZRMDT.solns].Yaw = Y * DEG;
		if (EZRMDT.data[EZRMDT.solns].Yaw >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].Yaw = 0.0;
		}
		EZRMDT.data[EZRMDT.solns].Roll = R * DEG;
		if (EZRMDT.data[EZRMDT.solns].Roll >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].Roll = 0.0;
		}
		EZRMDT.data[EZRMDT.solns].PB = P_apo * DEG;
		if (EZRMDT.data[EZRMDT.solns].PB >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].PB = 0.0;
		}
		EZRMDT.data[EZRMDT.solns].YB = Y_apo * DEG;
		if (EZRMDT.data[EZRMDT.solns].YB >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].YB = 0.0;
		}
		EZRMDT.data[EZRMDT.solns].RB = R_apo * DEG;
		if (EZRMDT.data[EZRMDT.solns].RB >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].RB = 0.0;
		}
		gmt += dt;
		EZRMDT.solns++;
	}
}

void RTCC::EMSTIME(int L, int ID)
{
	//Twelve-Second Time trap
	if (ID == 1)
	{
		ELVCTRInputTable in;
		ELVCTROutputTable2 out;
		TimeConstraintsTable *tab;

		if (L == 1)
		{
			tab = &EZTSCNS1;
		}
		else
		{
			tab = &EZTSCNS3;
		}

		in.GMT = RTCCPresentTimeGMT();
		in.L = L;

		ELVCTR(in, out);

		//Error code 2 is acceptable ("Order of interpolation performed less than order requested")
		if (out.ErrorCode > 2)
		{
			tab->TUP = 0;
			return;
		}

		EphemerisData2 sv_MCI, sv_I;
		ELVCNV(out.SV, 0, 2, sv_MCI);

		int coor1, coor2, RBI;
		if (length(sv_MCI.R) > 9.0*OrbMech::R_Earth)
		{
			sv_I = out.SV;
			RBI = BODY_EARTH;
			coor1 = 0;
			coor2 = 1;
		}
		else
		{
			sv_I = sv_MCI;
			RBI = BODY_MOON;
			coor1 = 2;
			coor2 = 3;
		}

		EphemerisData2 sv_true;
		int err = ELVCNV(sv_I, coor1, coor2, sv_true);
		if (err)
		{
			tab->TUP = 0;
			return;
		}

		EMMDYNEL(Eph2ToEph1(sv_true, RBI), *tab);

		tab->RevNum = CapeCrossingRev(L, sv_true.GMT);
		tab->TUP = out.TUP;
	}
}

void RTCC::EMSNAP(int L, int ID)
{
	//Trajectory Update
	if (ID == 1)
	{
		EMMDYNMC(L, 2);
	}
	//MPT update without trajectory update
	else if (ID == 2)
	{
		PMDMPT();
	}
	//Update MPT and DMT displays as a result of a fuel change or maneuver execution
	else if (ID == 7)
	{
		PMDMPT();
		PMDDMT(EZETVMED.DMT1Vehicle, EZETVMED.DMT1Number, EZETVMED.DMT1REFSMMATCode, EZETVMED.DMT1HeadsUpDownIndicator, DMTBuffer[0]);
		PMDDMT(EZETVMED.DMT2Vehicle, EZETVMED.DMT2Number, EZETVMED.DMT2REFSMMATCode, EZETVMED.DMT2HeadsUpDownIndicator, DMTBuffer[1]);
	}
	//Two-Impulse Multiple Solution Display
	else if (ID == 63)
	{
		PMDTIMP();
	}
}

void RTCC::EMMDYNMC(int L, int queid, int ind, double param)
{
	//Queue IDs:
	//1 = 12 second update
	//2 = Trajectory Update
	//3 = MED U12
	//4 = MED U13
	//5 = MED U14

	FIDOOrbitDigitals *tab;
	TimeConstraintsTable *tcontab;
	MissionPlanTable *mpt;
	OrbitEphemerisTable *ephtab;
	if (L == 1)
	{
		tab = &EZSAVCSM;
		tcontab = &EZTSCNS1;
		mpt = &PZMPTCSM;
		ephtab = &EZEPH1;
	}
	else
	{
		tab = &EZSAVLEM;
		tcontab = &EZTSCNS3;
		mpt = &PZMPTLEM;
		ephtab = &EZEPH2;
	}

	double CurGET, CurGMT, R_B;

	CurGMT = RTCCPresentTimeGMT();
	CurGET = GETfromGMT(CurGMT);
	tab->Error = 0;

	if (queid == 1 || queid == 2)
	{
		//TBD: move this
		EMSTIME(L, 1);
	}

	//For now don't even allow processing if time constraints table is invalid
	if (tcontab->TUP <= 0)
	{
		return;
	}

	if (tcontab->sv_present.RBI == BODY_EARTH)
	{
		R_B = OrbMech::R_Earth;
	}
	else
	{
		R_B = BZLAND.rad[RTCC_LMPOS_BEST];
	}

	if (queid == 1)
	{
		tab->REV = tcontab->RevNum;
		tab->GET = GETfromGMT(tcontab->sv_present.GMT);
		tab->TO = tcontab->T0;
		if (tcontab->sv_present.RBI == BODY_EARTH)
		{
			sprintf_s(tab->REF, "EARTH");
		}
		else
		{
			sprintf_s(tab->REF, "LUNAR");
		}

		if (mpt->LastExecutedManeuver == 0)
		{
			tab->ORBWT = mpt->TotalInitMass*1000.0*LBS;

		}
		else
		{
			tab->ORBWT = mpt->mantable[mpt->LastExecutedManeuver - 1].TotalMassAfter *1000.0*LBS;
		}
	}

	if (queid == 2)
	{
		sprintf_s(tab->STAID, tcontab->StationID.c_str());
		tab->GMTID = mpt->GMTAV;
		tab->GETID = GETfromGMT(mpt->GMTAV);
		if (L == 1)
		{
			sprintf_s(tab->VEHID, "CSM");
		}
		else
		{
			sprintf_s(tab->VEHID, "LEM");
		}
	}

	if (queid == 2 || (tcontab->lat > 0 && tab->PPP < 0))
	{
		EphemerisDataTable2 EPHEM;
		ManeuverTimesTable MANTIMES;
		LunarStayTimesTable LUNSTAY;
		ELFECH(CurGMT, 100, 1, L, EPHEM, MANTIMES, LUNSTAY);
		if (EPHEM.Header.NumVec < 9)
		{
			//Error
			return;
		}

		int RBI = DetermineSVBody(EPHEM.table[0]);

		int out;
		if (RBI == BODY_EARTH)
		{
			out = 1;
		}
		else
		{
			out = 3;
		}

		EphemerisDataTable2 EPHEM2;
		ELVCNV(EPHEM.table, 0, out, EPHEM2.table);
		EPHEM2.Header = EPHEM.Header;
		EPHEM2.Header.CSI = out;

		double lng;
		int err = RMMASCND(EPHEM2, MANTIMES, CurGMT, lng);
		if (err)
		{
			EMGPRINT("EMMDYNMC", 29);
			tab->LNPP = 0.0;
		}
		else
		{
			tab->LNPP = lng * DEG;
		}
	}

	if (queid == 1 || queid == 2)
	{
		tab->LPP = tcontab->lng*DEG;
		tab->PPP = tcontab->lat*DEG;
		tab->GETCC = GETfromGMT(CapeCrossingGMT(L, tcontab->RevNum + 1));
		tab->TAPP = tcontab->TA*DEG;
		tab->H = tcontab->h / 1852.0;
		tab->V = tcontab->V / 0.3048;
		tab->GAM = tcontab->gamma*DEG;
		tab->A = tcontab->a / 1852.0;
		tab->E = tcontab->e;
		tab->I = tcontab->i*DEG;

		if (queid == 2 || (CurGET > tab->GETA))
		{
			if (tcontab->e > 0.85)
			{
				tab->HA = 0.0;
				tab->PA = 0.0;
				tab->LA = 0.0;
				tab->GETA = 0.0;
			}
			else
			{
				AEGBlock aeg;
				AEGDataBlock sv_a;
				double INFO[10];

				PIMCKC(tcontab->sv_present.R, tcontab->sv_present.V, tcontab->sv_present.RBI, aeg.Data.coe_osc.a, aeg.Data.coe_osc.e, aeg.Data.coe_osc.i, aeg.Data.coe_osc.g, aeg.Data.coe_osc.h, aeg.Data.coe_osc.l);

				aeg.Header.AEGInd = tcontab->sv_present.RBI;
				aeg.Data.TS = aeg.Data.TE = tcontab->sv_present.GMT;

				PMMAPD(aeg.Header, aeg.Data, 1, 0, INFO, &sv_a, NULL);

				tab->HA = INFO[4] / 1852.0;
				tab->PA = INFO[2] * DEG;
				tab->LA = INFO[3] * DEG;
				tab->GETA = GETfromGMT(INFO[0]);
			}
		}

		if (queid == 2 || (CurGET > tab->GETP))
		{
			if (tcontab->e > 0.85)
			{
				tab->HP = 0.0;
				tab->PP = 0.0;
				tab->LP = 0.0;
				tab->GETP = 0.0;
			}
			else
			{
				AEGBlock aeg;
				AEGDataBlock sv_p;
				double INFO[10];

				PIMCKC(tcontab->sv_present.R, tcontab->sv_present.V, tcontab->sv_present.RBI, aeg.Data.coe_osc.a, aeg.Data.coe_osc.e, aeg.Data.coe_osc.i, aeg.Data.coe_osc.g, aeg.Data.coe_osc.h, aeg.Data.coe_osc.l);

				aeg.Header.AEGInd = tcontab->sv_present.RBI;
				aeg.Data.TS = aeg.Data.TE = tcontab->sv_present.GMT;

				PMMAPD(aeg.Header, aeg.Data, -1, 0, INFO, NULL, &sv_p);

				tab->HP = INFO[9] / 1852.0;
				tab->PP = INFO[7] * DEG;
				tab->LP = INFO[8] * DEG;
				tab->GETP = GETfromGMT(INFO[5]);
			}
		}
	}

	//Clear manual request data upon trajectory update
	if (queid == 2)
	{
		tab->HPR = 0.0;
		tab->PPR = 0.0;
		tab->LPR = 0.0;
		tab->GETPR = 0.0;
		tab->HAR = 0.0;
		tab->PAR = 0.0;
		tab->LAR = 0.0;
		tab->GETAR = 0.0;
		tab->REVL = 0;
		tab->GETL = 0.0;
		tab->L = 0.0;
	}

	//U12: Predicted apogee and perigee
	if (queid == 3)
	{
		EphemerisData sv_pred;
		if (ind == 1 || ind == 2)
		{
			double GET;
			if (ind == 1)
			{
				int rev = (int)param;
				GET = GETfromGMT(CapeCrossingGMT(L, rev));
				if (GET < 0)
				{
					return;
				}
			}
			else
			{
				GET = param;
			}
			ELVCTRInputTable intab;
			ELVCTROutputTable2 outtab;
			intab.GMT = GMTfromGET(GET);
			intab.L = L;
			ELVCTR(intab, outtab);
			if (outtab.ErrorCode)
			{
				return;
			}
			sv_pred = RotateSVToSOI(outtab.SV);
			tab->NV2 = outtab.ORER + 1;
		}
		else
		{
			unsigned man = (unsigned)param;
			if (man > mpt->mantable.size())
			{
				return;
			}
			sv_pred.R = mpt->mantable[man - 1].R_BO;
			sv_pred.V = mpt->mantable[man - 1].V_BO;
			sv_pred.GMT = mpt->mantable[man - 1].GMT_BO;
			sv_pred.RBI = mpt->mantable[man - 1].RefBodyInd;
			tab->NV2 = 0;
		}

		if (sv_pred.RBI == BODY_EARTH)
		{
			sprintf_s(tab->REFR, "EARTH");
		}
		else
		{
			sprintf_s(tab->REFR, "LUNAR");
		}

		tab->REVR = CapeCrossingRev(L, sv_pred.GMT);
		tab->GETBV = GETfromGMT(sv_pred.GMT);

		AEGBlock aeg;
		AEGDataBlock sv_a, sv_p;
		double INFO[10];

		PIMCKC(sv_pred.R, sv_pred.V, sv_pred.RBI, aeg.Data.coe_osc.a, aeg.Data.coe_osc.e, aeg.Data.coe_osc.i, aeg.Data.coe_osc.g, aeg.Data.coe_osc.h, aeg.Data.coe_osc.l);
		aeg.Header.AEGInd = sv_pred.RBI;

		PMMAPD(aeg.Header, aeg.Data, 0, 0, INFO, &sv_a, &sv_p);

		tab->HAR = INFO[4] / 1852.0;
		tab->PAR = INFO[2] * DEG;
		tab->LAR = INFO[3] * DEG;
		tab->GETAR = GETfromGMT(INFO[0]);

		tab->HPR = INFO[9] / 1852.0;
		tab->PPR = INFO[7] * DEG;
		tab->LPR = INFO[8] * DEG;
		tab->GETPR = GETfromGMT(INFO[5]);
	}
	//U13: Longitude crossing time
	else if (queid == 4)
	{
		int rev = ind;
		double lng = param;
		double GMT_guess = CapeCrossingGMT(L, rev);
		if (GMT_guess < 0)
		{
			//Error
			tab->Error = 5;
			return;
		}

		double GMT_cross, GMT_min, GMT_max; 

		GMT_min = GMT_guess - 5.0*60.0;
		GMT_max = GMT_guess + 2.5*3600.0;

		EphemerisDataTable2 EPHEM;
		ManeuverTimesTable MANTIMES;
		LunarStayTimesTable LUNSTAY;
		unsigned int NumVec;
		int TUP, err;

		err = ELNMVC(GMT_min, GMT_max, L, NumVec, TUP);
		if (err > 8)
		{
			//The revolution number on the U13 MED is valid. However, the longitude crossing request is outside the range of the ephemeris.
			tab->Error = 7;
			return;
		}
		err = ELFECH(GMT_min, NumVec, 0, L, EPHEM, MANTIMES, LUNSTAY);
		if (err)
		{
			tab->Error = 7;
			return;
		}

		//Convert to ECT/MCT
		EphemerisDataTable2 tab2;
		EphemerisData sv_out;
		EphemerisData2 sv_inter, sv_cross;
		int out;

		if (DetermineSVBody(EPHEM.table.front()) == BODY_EARTH)
		{
			out = 1;
		}
		else
		{
			out = 3;
		}

		ELVCNV(EPHEM.table, 0, out, tab2.table);
		tab2.Header = EPHEM.Header;
		tab2.Header.CSI = out;
		
		if (RLMTLC(tab2, ephtab->MANTIMES, lng, GMT_guess, GMT_cross, sv_cross))
		{
			tab->Error = 7;
			return;
		}
		tab->GETL = GETfromGMT(GMT_cross);
		tab->REVL = rev;
		tab->L = lng * DEG;
	}
	//U14: Compute longitude at given time
	else if (queid == 5)
	{
		double gmt = GMTfromGET(param);

		ELVCTRInputTable intab;
		ELVCTROutputTable2 outtab;

		intab.GMT = gmt;
		intab.L = L;

		ELVCTR(intab, outtab);
		if (outtab.ErrorCode)
		{
			return;
		}
		int out;
		if (DetermineSVBody(outtab.SV) == BODY_EARTH)
		{
			out = 1;
		}
		else
		{
			out = 3;
		}
		EphemerisData2 sv_true;
		ELVCNV(outtab.SV, 0, out, sv_true);
		double lat, lng;
		OrbMech::latlong_from_r(sv_true.R, lat, lng);
		tab->GETL = param;
		tab->L = lng * DEG;
	}
}

//Space Digitals

int RTCC::EMDSPACENoMPT(SV sv0, int queid, double gmt, double incl, double ascnode)
{
	//queid:
	//3 = MED Column 1
	//4 = MED Column 2
	//5 = MED Column 3

	EZSPACE.errormessage = "";

	EphemerisData sv = ConvertSVtoEphemData(sv0);

	//Generate display values for current state vector
	EZSPACE.GET = GETfromGMT(sv.GMT);
	EZSPACE.WEIGHT = sv0.mass * LBS*1000.0;
	EZSPACE.GMTV = 0.0;
	EZSPACE.GETV = EZSPACE.GETAxis = 0.0;

	if (sv.RBI == BODY_EARTH)
	{
		sprintf(EZSPACE.REF, "EARTH");
	}
	else
	{
		sprintf(EZSPACE.REF, "MOON");
	}

	TimeConstraintsTable newtab;
	EMMDYNEL(sv, newtab);

	EZSPACE.V = newtab.V / 0.3048;
	EZSPACE.GAM = newtab.gamma*DEG;
	EZSPACE.H = newtab.h / 1852.0;
	EZSPACE.PHI = newtab.lat*DEG;
	EZSPACE.LAM = newtab.lng*DEG;
	EZSPACE.PSI = newtab.azi*DEG;
	EZSPACE.ADA = newtab.TA*DEG;

	EZSPACE.GETR = EZSPACE.GET - SystemParameters.MCGREF * 3600.0;

	//Propagate to input time
	EMMENIInputTable emsin;
	emsin.AnchorVector = sv;
	emsin.MaxIntegTime = abs(gmt - sv.GMT);
	if (gmt - sv.GMT >= 0)
	{
		emsin.IsForwardIntegration = 1.0;
	}
	else
	{
		emsin.IsForwardIntegration = -1.0;
	}
	EMMENI(emsin);
	sv = emsin.sv_cutoff;

	//Column 1
	if (queid == 3)
	{
		EphemerisData sv_EMP, sv_true;
		double R_E, mu;

		EZSPACE.GETVector1 = GETfromGMT(sv.GMT);
		if (sv.RBI == BODY_EARTH)
		{
			sprintf(EZSPACE.REF1, "E");
			mu = OrbMech::mu_Earth;
		}
		else
		{
			sprintf(EZSPACE.REF1, "M");
			mu = OrbMech::mu_Moon;
		}

		EZSPACE.WT = sv0.mass * LBS*1000.0;

		int csi_out;
		if (sv.RBI == BODY_EARTH)
		{
			csi_out = 1;
			R_E = OrbMech::R_Earth;
		}
		else
		{
			csi_out = 3;
			R_E = BZLAND.rad[RTCC_LMPOS_BEST];
		}

		EZSPACE.H1 = EZSPACE.HS = EZSPACE.HO = (length(sv.R) - R_E) / 1852.0;
		ELVCNV(sv, 4, sv_EMP);
		ELVCNV(sv, csi_out, sv_true);

		EMMDYNEL(sv_EMP, newtab);
		EZSPACE.IEMP = newtab.i*DEG;

		EMMDYNEL(sv_true, newtab);
		EZSPACE.V1 = newtab.V / 0.3048;
		EZSPACE.PHI1 = EZSPACE.PHIO = newtab.lat * DEG;
		EZSPACE.LAM1 = newtab.lng*DEG;
		EZSPACE.GAM1 = newtab.gamma*DEG;
		EZSPACE.PSI1 = newtab.azi*DEG;
		EZSPACE.A1 = newtab.a / 1852.0;
		EZSPACE.L1 = (newtab.TA + newtab.AoP)*DEG;
		if (EZSPACE.L1 > 360.0)
		{
			EZSPACE.L1 -= 360.0;
		}
		EZSPACE.E1 = newtab.e;
		EZSPACE.I1 = newtab.i*DEG;

		if (newtab.e < 0.85)
		{
			AEGBlock aeg;
			AEGDataBlock sv_a, sv_p;
			double INFO[10];

			PIMCKC(sv.R, sv.V, sv.RBI, aeg.Data.coe_osc.a, aeg.Data.coe_osc.e, aeg.Data.coe_osc.i, aeg.Data.coe_osc.g, aeg.Data.coe_osc.h, aeg.Data.coe_osc.l);
			aeg.Header.AEGInd = sv.RBI;

			PMMAPD(aeg.Header, aeg.Data, 0, 0, INFO, &sv_a, &sv_p);

			EZSPACE.GETA = GETfromGMT(INFO[0]);
			EZSPACE.HA = INFO[4] / 1852.0;
			EZSPACE.HP = INFO[9] / 1852.0;
		}
		else
		{
			double peri, apo;
			OrbMech::periapo(sv.R, sv.V, mu, apo, peri);
			EZSPACE.HA = (apo - R_E) / 1852.0;
			if (EZSPACE.HA > 999999.9)
			{
				EZSPACE.HA = 999999.9;
			}
			EZSPACE.HP = (peri - R_E) / 1852.0;
			EZSPACE.GETA = 0.0;
		}
	}
	//Column 2
	else if (queid == 4)
	{
		//Null all values
		EZSPACE.GETSI = 0.0;
		EZSPACE.GETCA = 0.0;
		EZSPACE.VCA = 0.0;
		EZSPACE.HCA = 0.0;
		EZSPACE.PCA = 0.0;
		EZSPACE.LCA = 0.0;
		EZSPACE.PSICA = 0.0;
		EZSPACE.GETMN = 0.0;
		EZSPACE.HMN = 0.0;
		EZSPACE.PMN = 0.0;
		EZSPACE.LMN = 0.0;
		EZSPACE.DMN = 0.0;

		EZSPACE.GETVector2 = GETfromGMT(sv.GMT);

		EMMENIInputTable emsin;

		if (sv.RBI == BODY_EARTH)
		{
			//Try to find lunar sphere entry
			emsin.AnchorVector = sv;
			emsin.CutoffIndicator = 5;
			emsin.MaxIntegTime = 10.0*24.0*3600.0;

			EphemerisData sv_SI;
			EMMENI(emsin);
			sv_SI = emsin.sv_cutoff;
			if (emsin.TerminationCode == 5)
			{
				EZSPACE.GETSI = GETfromGMT(sv_SI.GMT);
			}
		}

		//Try to find perilune
		emsin.AnchorVector = sv;
		emsin.CutoffIndicator = 4;
		emsin.StopParamRefFrame = 1;
		emsin.MoonRelStopParam = 0.0;
		emsin.MaxIntegTime = 10.0*24.0*3600.0;
		EphemerisData sv1;
		EMMENI(emsin);
		sv1 = emsin.sv_cutoff;
		if (emsin.TerminationCode != 4)
		{
			return 0;
		}

		EphemerisData svtempout;
		TimeConstraintsTable newtab;

		ELVCNV(sv1, 3, svtempout);
		EMMDYNEL(svtempout, newtab);

		EZSPACE.GETCA = GETfromGMT(sv1.GMT);
		EZSPACE.VCA = newtab.V / 0.3048;
		EZSPACE.HCA = newtab.h / 1852.0;
		EZSPACE.PCA = newtab.lat * DEG;
		EZSPACE.LCA = newtab.lng * DEG;
		EZSPACE.PSICA = newtab.azi*DEG;
	}
	//Column 3
	else
	{
		//Null all values
		EZSPACE.GETSE = 0.0;
		EZSPACE.GETEI = 0.0;
		EZSPACE.VEI = 0.0;
		EZSPACE.GEI = 0.0;
		EZSPACE.PEI = 0.0;
		EZSPACE.LEI = 0.0;
		EZSPACE.PSIEI = 0.0;
		EZSPACE.GETVP = 0.0;
		EZSPACE.VVP = 0.0;
		EZSPACE.HVP = 0.0;
		EZSPACE.PVP = 0.0;
		EZSPACE.LVP = 0.0;
		EZSPACE.PSIVP = 0.0;
		EZSPACE.IE = 0.0;
		EZSPACE.LN = 0.0;

		EMMENIInputTable emsin;
		EZSPACE.GETVector3 = GETfromGMT(sv.GMT);

		//Find Moon SOI exit
		emsin.AnchorVector = sv;
		emsin.CutoffIndicator = 2;
		emsin.MoonRelStopParam = 9.0*OrbMech::R_Earth;
		emsin.StopParamRefFrame = 1;
		emsin.MaxIntegTime = 10.0*24.0*3600.0;
		EMMENI(emsin);
		if (emsin.TerminationCode == 2)
		{
			double fpa = dotp(unit(emsin.sv_cutoff.R), unit(emsin.sv_cutoff.V));
			if (fpa < 0)
			{
				//We found an Moon SOI entry
				//Try to find exit, first 1 hour into the future, then find next passage of SOI
				emsin.AnchorVector = emsin.sv_cutoff;
				emsin.CutoffIndicator = 1;
				emsin.MaxIntegTime = 1.0*3600.0;
				EMMENI(emsin);

				emsin.AnchorVector = emsin.sv_cutoff;
				emsin.CutoffIndicator = 2;
				emsin.MoonRelStopParam = 9.0*OrbMech::R_Earth;
				emsin.MaxIntegTime = 10.0*24.0*3600.0;
				EMMENI(emsin);
			}

			//Did we find the SOI radius?
			if (emsin.TerminationCode == 2)
			{
				EZSPACE.GETSE = GETfromGMT(emsin.sv_cutoff.GMT);
			}
		}

		//Find vacuum perigee
		emsin.AnchorVector = sv;
		emsin.CutoffIndicator = 4;
		emsin.StopParamRefFrame = 0;
		emsin.EarthRelStopParam = 0.0;
		emsin.MaxIntegTime = 10.0*24.0*3600.0;
		EMMENI(emsin);
		if (emsin.TerminationCode != 4)
		{
			return 0;
		}

		if (length(emsin.sv_cutoff.R) > 10.0*OrbMech::R_Earth)
		{
			return 0;
		}

		TimeConstraintsTable newtab;
		EphemerisData svtempout;

		EZSPACE.GETVP = GETfromGMT(emsin.sv_cutoff.GMT);

		ELVCNV(emsin.sv_cutoff, 1, svtempout);
		EMMDYNEL(svtempout, newtab);

		EZSPACE.VVP = newtab.V / 0.3048;
		EZSPACE.HVP = newtab.h / 1852.0;
		EZSPACE.PVP = newtab.lat * DEG;
		EZSPACE.LVP = newtab.lng * DEG;
		EZSPACE.PSIVP = newtab.azi*DEG;
		EZSPACE.IE = newtab.i*DEG;
		EZSPACE.LN = newtab.RA*DEG;

		//Have we passed EI?
		if (length(emsin.sv_cutoff.R) > OrbMech::R_Earth + 400000.0*0.3048)
		{
			return 0;
		}

		//Find EI
		emsin.AnchorVector = emsin.sv_cutoff;
		emsin.CutoffIndicator = 3;
		emsin.StopParamRefFrame = 0;
		emsin.EarthRelStopParam = 400000.0*0.3048;
		emsin.MaxIntegTime = 3600.0; //Should be enough
		emsin.IsForwardIntegration = -1.0;
		EMMENI(emsin);

		if (emsin.TerminationCode == 3)
		{
			EZSPACE.GETEI = GETfromGMT(emsin.sv_cutoff.GMT);

			ELVCNV(emsin.sv_cutoff, 1, svtempout);
			EMMDYNEL(svtempout, newtab);

			EZSPACE.VEI = newtab.V / 0.3048;
			EZSPACE.GEI = newtab.gamma*DEG;
			EZSPACE.PEI = newtab.lat * DEG;
			EZSPACE.LEI = newtab.lng * DEG;
			EZSPACE.PSIEI = newtab.azi*DEG;
		}
	}

	return 0;
}

int RTCC::EMDSPACE(int queid, int option, double val, double incl, double ascnode)
{
	//queid:
	//1 = Time cylce (12 sec)
	//2 = Trajectory Update
	//3 = MED Column 1
	//4 = MED Column 2
	//5 = MED Column 3
	//6 = MSK request
	//7 = Reinitialization

	EZSPACE.errormessage = "";

	if (EZETVMED.SpaceDigVehID < 0)
	{
		//Error: The display has not been initialized via MED U00
		EZSPACE.errormessage = "Error 1";
		return 1;
	}

	OrbitEphemerisTable *ephtab;
	MissionPlanTable *mpt;
	TimeConstraintsTable *tctab;

	if (EZETVMED.SpaceDigVehID == RTCC_MPT_CSM)
	{
		ephtab = &EZEPH1;
		mpt = &PZMPTCSM;
		tctab = &EZTSCNS1;
	}
	else
	{
		ephtab = &EZEPH2;
		mpt = &PZMPTLEM;
		tctab = &EZTSCNS3;
	}

	if (ephtab->EPHEM.Header.TUP == 0)
	{
		//Error: The trajectory for subject vehicle is not generated
		EZSPACE.errormessage = "Error 2";
		return 2;
	}
	if (ephtab->EPHEM.Header.TUP < 0)
	{
		//Error: The main ephemeris is in an update status
		EZSPACE.errormessage = "Error 5";
		return 5;
	}
	if (mpt->CommonBlock.TUP < 0 || mpt->CommonBlock.TUP != ephtab->EPHEM.Header.TUP)
	{
		//Error: The MPT is either in an update state or is inconsistent with the trajectory update number
		EZSPACE.errormessage = "Error 6";
		return 6;
	}

	if (queid == 1)
	{
		//TBD: Move this
		EMSTIME(EZETVMED.SpaceDigVehID, 1);

		if (tctab->TUP > 0)
		{
			EZSPACE.GET = GETfromGMT(tctab->sv_present.GMT);
		}
		else
		{
			EZSPACE.GET = 0.0;
		}
	}
	else
	{
		if (EZETVMED.SpaceDigVehID == 1)
		{
			sprintf_s(EZSPACE.VEHID, "CSM");
		}
		else
		{
			sprintf_s(EZSPACE.VEHID, "LEM");
		}
	}

	if (queid == 1 || queid == 2 || queid == 6 || queid == 7)
	{
		double cfg_weight;

		EZSPACE.TUP = ephtab->EPHEM.Header.TUP;
		sprintf_s(EZSPACE.VecID, mpt->StationID.c_str());
		if (tctab->TUP > 0)
		{
			if (mpt->LastExecutedManeuver == 0)
			{
				cfg_weight = mpt->TotalInitMass;

			}
			else
			{
				cfg_weight = mpt->mantable[mpt->LastExecutedManeuver - 1].TotalMassAfter;
			}
		}
		else
		{
			cfg_weight = 0.0;
		}
		EZSPACE.WEIGHT = cfg_weight * LBS*1000.0;
		EZSPACE.GMTV = mpt->GMTAV;
		EZSPACE.GETV = EZSPACE.GETAxis = GETfromGMT(mpt->GMTAV);

		if (tctab->TUP > 0)
		{
			if (tctab->sv_present.RBI == BODY_EARTH)
			{
				sprintf(EZSPACE.REF, "EARTH");
			}
			else
			{
				sprintf(EZSPACE.REF, "MOON");
			}
			EZSPACE.V = tctab->V / 0.3048;
			EZSPACE.GAM = tctab->gamma*DEG;
			EZSPACE.H = tctab->h / 1852.0;
			EZSPACE.PHI = tctab->lat*DEG;
			EZSPACE.LAM = tctab->lng*DEG;
			EZSPACE.PSI = tctab->azi*DEG;
			EZSPACE.ADA = tctab->TA*DEG;
		}
		else
		{
			EZSPACE.V = 0.0;
			EZSPACE.GAM = 0.0;
			EZSPACE.H = 0.0;
			EZSPACE.PHI = 0.0;
			EZSPACE.LAM = 0.0;
			EZSPACE.PSI = 0.0;
			EZSPACE.ADA = 0.0;
		}

		EZSPACE.GETR = EZSPACE.GET - SystemParameters.MCGREF * 3600.0;
	}
	else
	{
		EphemerisData sv;
		//GET
		if (option == 0)
		{
			double GMT = GMTfromGET(val);
			if (ELFECH(GMT, EZETVMED.SpaceDigVehID, sv))
			{
				if (GMT > ephtab->EPHEM.Header.TR)
				{
					//Error: The time on the U01 exceeds the start of the ephemeris
					EZSPACE.errormessage = "Error 3";
					return 3;
				}
				else
				{
					//Error: The time on the U01 precedes the start of the ephemeris
					EZSPACE.errormessage = "Error 4";
					return 4;
				}
			}
			if (queid == 3)
			{
				EMMENIInputTable emsin;
				emsin.AnchorVector = sv;
				emsin.MaxIntegTime = abs(GMT - sv.GMT);
				if (GMT - sv.GMT >= 0)
				{
					emsin.IsForwardIntegration = 1.0;
				}
				else
				{
					emsin.IsForwardIntegration = -1.0;
				}
				EMMENI(emsin);
				sv = emsin.sv_cutoff;
			}
		}
		//MNV
		else
		{
			unsigned mnv = (unsigned)val;
			if (mnv > mpt->mantable.size())
			{
				//Error: The maneuver number requested is greater than the nmber of maneuvers in the Mission Plan Table
				return 8;
			}
			sv.R = mpt->mantable[mnv - 1].R_BO;
			sv.V = mpt->mantable[mnv - 1].V_BO;
			sv.GMT = mpt->mantable[mnv - 1].GMT_BO;
			sv.RBI = mpt->mantable[mnv - 1].RefBodyInd;
		}

		//Column 1
		if (queid == 3)
		{
			EphemerisData sv_EMP, sv_true;
			TimeConstraintsTable newtab;
			double R_E, mu;

			EZSPACE.GETVector1 = GETfromGMT(sv.GMT);
			if (sv.RBI == BODY_EARTH)
			{
				sprintf(EZSPACE.REF1, "E");
				mu = OrbMech::mu_Earth;
			}
			else
			{
				sprintf(EZSPACE.REF1, "M");
				mu = OrbMech::mu_Moon;
			}
			
			PLAWDTInput pin;
			PLAWDTOutput pout;
			pin.T_UP = sv.GMT;
			pin.TableCode = EZETVMED.SpaceDigVehID;
			PLAWDT(pin, pout);
			EZSPACE.WT = pout.ConfigWeight * LBS*1000.0;

			int csi_out;
			if (sv.RBI == BODY_EARTH)
			{
				csi_out = 1;
				R_E = OrbMech::R_Earth;
			}
			else
			{
				csi_out = 3;
				R_E = BZLAND.rad[RTCC_LMPOS_BEST];
			}
			
			EZSPACE.H1 = EZSPACE.HS = EZSPACE.HO = (length(sv.R) - R_E) / 1852.0;
			ELVCNV(sv, 4, sv_EMP);
			ELVCNV(sv, csi_out, sv_true);
			
			EMMDYNEL(sv_EMP, newtab);
			EZSPACE.IEMP = newtab.i*DEG;

			EMMDYNEL(sv_true, newtab);
			EZSPACE.V1 = newtab.V / 0.3048;
			EZSPACE.PHI1 = EZSPACE.PHIO = newtab.lat * DEG;
			EZSPACE.LAM1 = newtab.lng*DEG;
			EZSPACE.GAM1 = newtab.gamma*DEG;
			EZSPACE.PSI1 = newtab.azi*DEG;
			EZSPACE.A1 = newtab.a / 1852.0;
			EZSPACE.L1 = (newtab.TA + newtab.AoP)*DEG;
			if (EZSPACE.L1 > 360.0)
			{
				EZSPACE.L1 -= 360.0;
			}
			EZSPACE.E1 = newtab.e;
			EZSPACE.I1 = newtab.i*DEG;

			if (newtab.e < 0.85)
			{
				AEGBlock aeg;
				AEGDataBlock sv_a, sv_p;
				double INFO[10];

				PIMCKC(sv.R, sv.V, sv.RBI, aeg.Data.coe_osc.a, aeg.Data.coe_osc.e, aeg.Data.coe_osc.i, aeg.Data.coe_osc.g, aeg.Data.coe_osc.h, aeg.Data.coe_osc.l);
				aeg.Header.AEGInd = sv.RBI;

				PMMAPD(aeg.Header, aeg.Data, 0, 0, INFO, &sv_a, &sv_p);

				EZSPACE.GETA = GETfromGMT(INFO[0]);
				EZSPACE.HA = INFO[4] / 1852.0;
				EZSPACE.HP = INFO[9] / 1852.0;
			}
			else
			{
				double peri, apo;
				OrbMech::periapo(sv.R, sv.V, mu, apo, peri);
				EZSPACE.HA = (apo - R_E) / 1852.0;
				if (EZSPACE.HA > 999999.9)
				{
					EZSPACE.HA = 999999.9;
				}
				EZSPACE.HP = (peri - R_E) / 1852.0;
				EZSPACE.GETA = 0.0;
			}
		}
		//Column 2
		else if (queid == 4)
		{
			//Null all values
			EZSPACE.GETSI = 0.0;
			EZSPACE.GETCA = 0.0;
			EZSPACE.VCA = 0.0;
			EZSPACE.HCA = 0.0;
			EZSPACE.PCA = 0.0;
			EZSPACE.LCA = 0.0;
			EZSPACE.PSICA = 0.0;
			EZSPACE.GETMN = 0.0;
			EZSPACE.HMN = 0.0;
			EZSPACE.PMN = 0.0;
			EZSPACE.LMN = 0.0;
			EZSPACE.DMN = 0.0;

			EZSPACE.GETVector2 = GETfromGMT(sv.GMT);

			EMMENIInputTable emsin;

			if (sv.RBI == BODY_EARTH)
			{
				//Try to find lunar sphere entry
				emsin.AnchorVector = sv;
				emsin.CutoffIndicator = 5;
				emsin.MaxIntegTime = 10.0*24.0*3600.0;

				EphemerisData sv_SI;
				EMMENI(emsin);
				sv_SI = emsin.sv_cutoff;
				if (emsin.TerminationCode == 5)
				{
					EZSPACE.GETSI = GETfromGMT(sv_SI.GMT);
				}
			}

			//Try to find perilune
			emsin.AnchorVector = sv;
			emsin.CutoffIndicator = 4;
			emsin.StopParamRefFrame = 1;
			emsin.MoonRelStopParam = 0.0;
			emsin.MaxIntegTime = 10.0*24.0*3600.0;
			EphemerisData sv1;
			EMMENI(emsin);
			sv1 = emsin.sv_cutoff;
			if (emsin.TerminationCode != 4)
			{
				return 0;
			}

			EphemerisData svtempout;
			TimeConstraintsTable newtab;

			ELVCNV(sv1, 3, svtempout);
			EMMDYNEL(svtempout, newtab);

			EZSPACE.GETCA = GETfromGMT(sv1.GMT);
			EZSPACE.VCA = newtab.V / 0.3048;
			EZSPACE.HCA = newtab.h / 1852.0;
			EZSPACE.PCA = newtab.lat * DEG;
			EZSPACE.LCA = newtab.lng * DEG;
			EZSPACE.PSICA = newtab.azi*DEG;
		}
		//Column 3
		else
		{
			//Null all values
			EZSPACE.GETSE = 0.0;
			EZSPACE.GETEI = 0.0;
			EZSPACE.VEI = 0.0;
			EZSPACE.GEI = 0.0;
			EZSPACE.PEI = 0.0;
			EZSPACE.LEI = 0.0;
			EZSPACE.PSIEI = 0.0;
			EZSPACE.GETVP = 0.0;
			EZSPACE.VVP = 0.0;
			EZSPACE.HVP = 0.0;
			EZSPACE.PVP = 0.0;
			EZSPACE.LVP = 0.0;
			EZSPACE.PSIVP = 0.0;
			EZSPACE.IE = 0.0;
			EZSPACE.LN = 0.0;

			EMMENIInputTable emsin;
			EZSPACE.GETVector3 = GETfromGMT(sv.GMT);			

			//Find Moon SOI exit
			emsin.AnchorVector = sv;
			emsin.CutoffIndicator = 2;
			emsin.MoonRelStopParam = 9.0*OrbMech::R_Earth;
			emsin.StopParamRefFrame = 1;
			emsin.MaxIntegTime = 10.0*24.0*3600.0;
			EMMENI(emsin);
			if (emsin.TerminationCode == 2)
			{
				double fpa = dotp(unit(emsin.sv_cutoff.R), unit(emsin.sv_cutoff.V));
				if (fpa < 0)
				{
					//We found an Moon SOI entry
					//Try to find exit, first 1 hour into the future, then find next passage of SOI
					emsin.AnchorVector = emsin.sv_cutoff;
					emsin.CutoffIndicator = 1;
					emsin.MaxIntegTime = 1.0*3600.0;
					EMMENI(emsin);

					emsin.AnchorVector = emsin.sv_cutoff;
					emsin.CutoffIndicator = 2;
					emsin.MoonRelStopParam = 9.0*OrbMech::R_Earth;
					emsin.MaxIntegTime = 10.0*24.0*3600.0;
					EMMENI(emsin);
				}

				//Did we find the SOI radius?
				if (emsin.TerminationCode == 2)
				{
					EZSPACE.GETSE = GETfromGMT(emsin.sv_cutoff.GMT);
				}
			}

			//Find vacuum perigee
			emsin.AnchorVector = sv;
			emsin.CutoffIndicator = 4;
			emsin.StopParamRefFrame = 0;
			emsin.EarthRelStopParam = 0.0;
			emsin.MaxIntegTime = 10.0*24.0*3600.0;
			EMMENI(emsin);
			if (emsin.TerminationCode != 4)
			{
				return 0;
			}

			if (length(emsin.sv_cutoff.R) > 10.0*OrbMech::R_Earth)
			{
				return 0;
			}

			TimeConstraintsTable newtab;
			EphemerisData svtempout;

			EZSPACE.GETVP = GETfromGMT(emsin.sv_cutoff.GMT);

			ELVCNV(emsin.sv_cutoff, 1, svtempout);
			EMMDYNEL(svtempout, newtab);

			EZSPACE.VVP = newtab.V / 0.3048;
			EZSPACE.HVP = newtab.h / 1852.0;
			EZSPACE.PVP = newtab.lat * DEG;
			EZSPACE.LVP = newtab.lng * DEG;
			EZSPACE.PSIVP = newtab.azi*DEG;
			EZSPACE.IE = newtab.i*DEG;
			EZSPACE.LN = newtab.RA*DEG;

			//Have we passed EI?
			if (length(emsin.sv_cutoff.R) > OrbMech::R_Earth + 400000.0*0.3048)
			{
				return 0;
			}

			//Find EI
			emsin.AnchorVector = emsin.sv_cutoff;
			emsin.CutoffIndicator = 3;
			emsin.StopParamRefFrame = 0;
			emsin.EarthRelStopParam = 400000.0*0.3048;
			emsin.MaxIntegTime = 3600.0; //Should be enough
			emsin.IsForwardIntegration = -1.0;
			EMMENI(emsin);

			if (emsin.TerminationCode == 3)
			{
				EZSPACE.GETEI = GETfromGMT(emsin.sv_cutoff.GMT);

				ELVCNV(emsin.sv_cutoff, 1, svtempout);
				EMMDYNEL(svtempout, newtab);

				EZSPACE.VEI = newtab.V / 0.3048;
				EZSPACE.GEI = newtab.gamma*DEG;
				EZSPACE.PEI = newtab.lat * DEG;
				EZSPACE.LEI = newtab.lng * DEG;
				EZSPACE.PSIEI = newtab.azi*DEG;
			}
		}
	}

	return 0;
}

void RTCC::EMSTAGEN(int L)
{
	EphemerisDataTable2 EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;

	//Get at least 10 hours worth of vectors. Or 200 vectors.
	double GMT_start, GMT_end;
	unsigned int NumVec;
	int TUP;
	GMT_start = RTCCPresentTimeGMT();
	GMT_end = GMT_start + 10.0*3600.0;

	ELNMVC(GMT_start, GMT_end, L, NumVec, TUP);
	if (NumVec < 200)
	{
		NumVec = 200;
	}
	ELFECH(GMT_start, NumVec, 1, L, EPHEM, MANTIMES, LUNSTAY);

	StationTable contact;

	for (int i = 0; i < SystemParameters.MKRBKS; i++)
	{
		if (SystemParameters.MGRTAG == 0 && GZSTCH[i].SiteType != 8) continue;

		contact.table.push_back(GZSTCH[i].data);
	}

	OrbitStationContactsTable *tab;
	if (L == 1)
	{
		tab = &EZSTACT1;
	}
	else
	{
		tab = &EZSTACT3;
	}

	if (GMT_start < EPHEM.Header.TL)
	{
		GMT_start = EPHEM.Header.TL;
		if (L == 1)
		{
			RTCCONLINEMON.TextBuffer[0] = "CSM";
			EMGPRINT("EMSTAGEN", 47);
		}
		else
		{
			RTCCONLINEMON.TextBuffer[0] = "LEM";
			EMGPRINT("EMSTAGEN", 47);
		}
	}

	//Convert vectors to ECT
	ELVCNV(EPHEM.table, 0, 1, EPHEM.table);
	EPHEM.Header.CSI = 1;
	//Generate station contacts
	EMGENGEN(EPHEM, MANTIMES, contact, BODY_EARTH, *tab, &LUNSTAY);

	RTCCONLINEMON.IntBuffer[0] = tab->Num;
	if (L == 1)
	{
		RTCCONLINEMON.TextBuffer[0] = "CSM";
	}
	else
	{
		RTCCONLINEMON.TextBuffer[0] = "LEM";
	}
	EMGPRINT("EMSTAGEN", 31);

	//Reset this, so next station contacts display updates immedately
	NextStationContactsBuffer.GET = -1;

	CapeCrossingTable *table;

	if (L == 1)
	{
		table = &EZCLEM;
	}
	else
	{
		table = &EZCCSM;
	}

	//Assign rev numbers to AOS times, if available
	if (table->NumRev == 0) return;

	for (int i = 0;i < 45;i++)
	{
		if (tab->Stations[i].GMTAOS <= 0.0) break;

		tab->Stations[i].REV = CapeCrossingRev(L, tab->Stations[i].GMTAOS);
	}
}

int RTCC::PMMMCD(PMMMCDInput in, MPTManeuver &man)
{
	EphemerisData sv_GMTI, sv_GMTI_other;
	VECTOR3 X_P, Y_P, Z_P, ExtDV, DV_A;
	double GMT_begin, P_G, Y_G, AL, BE, a1, a2, a3, b1, b2, b3, c1, c2, c3, SINP, SINY, SINR, COSP, COSY, COSR, d1, d2, d3, d4, Thrust;
	int TAIND, J;
	bool EXDVIND, Ind, TargetParamsInput;

	TargetParamsInput = true;
	ExtDV = _V(in.BurnParm75, in.BurnParm76, in.BurnParm77);

	J = 0;
	man.GMTI = in.GMTI;
	man.Thruster = in.Thruster;
	if (in.Thruster == RTCC_ENGINETYPE_CSMSPS || in.Thruster == RTCC_ENGINETYPE_LMAPS || in.Thruster == RTCC_ENGINETYPE_LMDPS)
	{
		double dt_ullage_overlap;

		if (in.dt_ullage == 0.0)
		{
			dt_ullage_overlap = 0.0;
		}
		else
		{
			if (in.Thruster == RTCC_ENGINETYPE_CSMSPS)
			{
				dt_ullage_overlap = SystemParameters.MCTSD9;
			}
			else if (in.Thruster == RTCC_ENGINETYPE_LMAPS)
			{
				dt_ullage_overlap = SystemParameters.MCTAD9;
			}
			else
			{
				dt_ullage_overlap = SystemParameters.MCTDD9;
			}
		}

		GMT_begin = in.GMTI - in.dt_ullage + dt_ullage_overlap;
		man.dt_ullage = in.dt_ullage;
	}
	else
	{
		GMT_begin = in.GMTI;
	}
	man.GMTMAN = GMT_begin;

	//TBD: Confirmation
	TAIND = 0;
	man.TrajDet[0] = 1;
	man.TrajDet[1] = 1;
	man.TrajDet[2] = 1;

	man.AttitudeCode = in.Attitude;
	man.CommonBlock.ConfigChangeInd = in.ConfigChangeInd;
	man.CommonBlock.ConfigCode = in.ConfigCodeAfter;
	man.TVC = in.TVC;
	man.DockingAngle = in.DockingAngle;
	man.ConfigCodeBefore = in.CCMI;

	if (in.BPIND == 5)
	{
		/*if (input)
		{

		}*/
		goto RTCC_PMMMCD_12_1;
	}
	else if (in.BPIND == 6 || in.BPIND == 3)
	{
		goto RTCC_PMMMCD_10_1;
	}
	else if (in.BPIND == 4)
	{
		goto RTCC_PMMMCD_14_2;
	}
	else if (in.BPIND == 1)
	{
		goto RTCC_PMMMCD_12_1;
	}
	else
	{
		if (in.FrozenManeuver)
		{
			goto RTCC_PMMMCD_5_1;
		}
		else
		{
			goto RTCC_PMMMCD_5_2;
		}
	}

RTCC_PMMMCD_5_1:
	J = 1;
	if (TargetParamsInput)
	{
		man.dV_LVLH = _V(in.BurnParm75, in.BurnParm76, in.BurnParm77);
	}
	else
	{

	}
	man.Word67i[0] = 0;
	if (in.Attitude == 4)
	{
		EXDVIND = true;
		goto RTCC_PMMMCD_6_1;
	}
	else
	{

	}
RTCC_PMMMCD_5_2:
	if (TargetParamsInput == false)
	{
		goto RTCC_PMMMCD_12_1;
	}
	man.Word67i[0] = 1;
	//TBD: Fix this
	//if (in.ID != 32)
	{
		goto RTCC_PMMMCD_7_2;
	}
RTCC_PMMMCD_5_3:
	if (in.Attitude > 4)
	{
		goto RTCC_PMMMCD_6_2;
	}
	PMMMCDCallEMSMISS(in.sv_anchor, in.GMTI, sv_GMTI);
RTCC_PMMMCD_B:
	EXDVIND = false;
RTCC_PMMMCD_6_1:
	Thrust = GetOnboardComputerThrust(in.Thruster);
	Ind = 0;
	ExtDV = PIEXDV(sv_GMTI.R, sv_GMTI.V, in.WTMI, Thrust, _V(in.BurnParm75, in.BurnParm76, in.BurnParm77), EXDVIND);
	goto RTCC_PMMMCD_7_3;
RTCC_PMMMCD_6_2:
	PMMMCDCallEMSMISS(in.sv_anchor, GMT_begin, sv_GMTI);
RTCC_PMMMCD_D:
	//Are CSM and LM docked?
	if (in.CCMI.to_ulong() == 13 || in.CCMI.to_ulong() == 5)
	{
		goto RTCC_PMMMCD_C;
	}
	PMMMCDCallEMSMISS(in.sv_other, GMT_begin, sv_GMTI_other);
RTCC_PMMMCD_C:
	Ind = J;
	ExtDV = PIAEDV(_V(in.BurnParm75, in.BurnParm76, in.BurnParm77), sv_GMTI_other.R, sv_GMTI_other.V, sv_GMTI.R, Ind);
RTCC_PMMMCD_7_3:
	if (J != 0)
	{
		man.dV_inertial = ExtDV;
	}
	goto RTCC_PMMMCD_12_1;
RTCC_PMMMCD_7_2:
	man.dV_LVLH = ExtDV;
	goto RTCC_PMMMCD_12_1;
RTCC_PMMMCD_10_1:
	if (in.ID == 32)
	{
		//Get REFSMMAT from inputs
		goto RTCC_PMMMCD_11_1;
	}
	MATRIX3 REFSMMAT;
	if (in.Thruster != RTCC_ENGINETYPE_LOX_DUMP)
	{
		if (in.TVC == 3)
		{
			//Get LM REFSMMAT
			REFSMMAT = EZJGMTX3.data[in.RefMatCode - 1].REFSMMAT;
		}
		else
		{
			//Get CSM REFSMMAT
			REFSMMAT = EZJGMTX1.data[in.RefMatCode - 1].REFSMMAT;
		}
	}
	else
	{
		///Get IU REFSMMAT
		REFSMMAT = GZLTRA.IU1_REFSMMAT;
	}
	X_P = _V(REFSMMAT.m11, REFSMMAT.m12, REFSMMAT.m13);
	Y_P = _V(REFSMMAT.m21, REFSMMAT.m22, REFSMMAT.m23);
	Z_P = _V(REFSMMAT.m31, REFSMMAT.m32, REFSMMAT.m33);
	//Obtain REFSMMAT
	if (in.Thruster != RTCC_ENGINETYPE_LOX_DUMP)
	{
		//REFSMMAT error
	}
RTCC_PMMMCD_11_1:
	if (in.BPIND == 5)
	{
		goto RTCC_PMMMCD_15_2;
	}
	if (in.BPIND > 5)
	{
		goto RTCC_PMMMCD_11_3;
	}
	if (in.BPIND < 3)
	{
		goto RTCC_PMMMCD_15_2;
	}
RTCC_PMMMCD_11_2:
	DV_A = X_P * in.BurnParm75 + Y_P * in.BurnParm76 + Z_P * in.BurnParm77;
	goto RTCC_PMMMCD_11_4;
RTCC_PMMMCD_11_3:
	//Residuals
	in.CoordinateInd = -1;
RTCC_PMMMCD_11_4:
	if (in.Attitude >= 4)
	{
		ExtDV = DV_A;
		if (in.FrozenManeuver == false)
		{
			man.Word67i[0] = 1;
			goto RTCC_PMMMCD_5_3;
		}

		man.dV_inertial = DV_A;
		man.Word67i[0] = 0;
		if (in.Attitude > 4)
		{
			goto RTCC_PMMMCD_D;
		}
		goto RTCC_PMMMCD_B;
	}
	double dv = length(DV_A);
	if (dv > 1e-10)
	{
		man.A_T = unit(DV_A);
	}
	else
	{
		man.A_T = _V(1, 0, 0);
	}
RTCC_PMMMCD_12_1:
	if (in.Thruster < 33)
	{
		if (in.BPIND == 1)
		{
			in.BurnParm73 = 0;
		}
	}
	else
	{
		man.UllageThrusterOpt = in.UllageThrusterOpt;
		//APS
		if (in.Thruster == 34)
		{
			TAIND = 1;
		}
		else
		{
			//DPS
			if (in.Thruster > 34)
			{
				man.DT_10PCT = in.DT_10PCT;
				man.DPSScaleFactor = in.DPSScaleFactor;
			}
			man.TrimAngleInd = TAIND - 1;
		}
	}
	if (in.BPIND < 2)
	{
		//DT input
		if (in.BurnParm72 < 0)
		{
			man.dt = in.BurnParm74;
		}
		else
		{
			if (in.BurnParm73 > 1)
			{
				//XBT
				if (TAIND != 0)
				{
					GetSystemGimbalAngles(in.Thruster, P_G, Y_G);
				}
				else
				{
					int IA = 1, IJ;
					double WTCXX, WTLXX, Thr, WDOT;
					if (in.CCMI[RTCC_CONFIG_D])
					{
						IJ = 0;
						if (in.mpt->mantable.size() == 0)
						{
							WTLXX = in.mpt->CommonBlock.LMAscentMass + in.mpt->CommonBlock.LMDescentMass;
						}
						else
						{
							WTLXX = in.mpt->mantable.back().CommonBlock.LMAscentMass + in.mpt->mantable.back().CommonBlock.LMDescentMass;
						}
					}
					else
					{
						IJ = 1;
						if (in.mpt->mantable.size() == 0)
						{
							WTLXX = in.mpt->CommonBlock.LMAscentMass;
						}
						else
						{
							WTLXX = in.mpt->mantable.back().CommonBlock.LMAscentMass;
						}
					}
					if (in.mpt->mantable.size() == 0)
					{
						WTCXX = in.mpt->CommonBlock.CSMMass;
					}
					else
					{
						WTCXX = in.mpt->mantable.back().CommonBlock.CSMMass;
					}
					unsigned IC = in.CCMI.to_ulong();
					GIMGBL(WTCXX, WTLXX, P_G, Y_G, Thr, WDOT, in.Thruster, IC, IA, IJ, in.DockingAngle);
				}
				man.dv = in.BurnParm72 / (cos(P_G)*cos(Y_G));
			}
			else
			{
				if (in.BurnParm73 == 1)
				{
					//DVC
					man.dv = -in.BurnParm72;
				}
				else
				{
					//MAG
					man.dv = in.BurnParm72;
				}
			}
		}
	}
	else
	{
		if (in.Attitude < 3)
		{
			man.dv = in.BurnParm72;
		}
	}

	if (in.CoordinateInd < 0)
	{
		man.HeadsUpDownInd = in.HeadsUpDownInd;
		goto RTCC_PMMMCD_17_1;
	}

	man.AttitudesInput = true;
	SINP = sin(in.Pitch);
	SINY = sin(in.Yaw);
	SINR = sin(in.Roll);
	COSP = cos(in.Pitch);
	COSY = cos(in.Yaw);
	COSR = cos(in.Roll);
	if (in.CoordinateInd > 0)
	{
		goto RTCC_PMMMCD_10_1;
	}
RTCC_PMMMCD_14_2:
	PMMMCDCallEMSMISS(in.sv_anchor, in.GMTI, sv_GMTI);
	Z_P = unit(-sv_GMTI.R);
	Y_P = -unit(crossp(sv_GMTI.R, sv_GMTI.V));
	X_P = crossp(Y_P, Z_P);
	if (in.BPIND == 4)
	{
		goto RTCC_PMMMCD_11_2;
	}
	AL = SINP * SINR;
	BE = SINP * COSR;
	a1 = COSY * COSP;
	a2 = SINY * COSP;
	a3 = -SINP;
	b1 = AL * COSY - SINY * COSR;
	b2 = AL * SINY + COSY * COSR;
	b3 = COSP * SINR;
	c1 = BE * COSY + SINY * SINR;
	c2 = BE * SINY - COSY * SINR;
	c3 = COSP * COSR;
	goto RTCC_PMMMCD_16_3;
RTCC_PMMMCD_15_2:
	if (in.TVC != 3)
	{
		d1 = SINY;
		d2 = SINR;
		d3 = COSY;
		d4 = COSR;
		goto RTCC_PMMMCD_16_4;
	}
	if (in.CoordinateInd == 1)
	{
		d1 = SINR;
		d2 = SINY;
		d3 = COSR;
		d4 = COSY;
		goto RTCC_PMMMCD_16_4;
	}
	AL = SINR * SINY;
	BE = COSR * SINY;
	a1 = COSR * COSP - AL * SINP;
	a2 = SINR * COSP;
	a3 = -COSR * SINP - AL * COSP;
	b1 = -SINR * COSP - AL * COSP;
	b2 = COSR * COSY;
	b3 = SINR * SINP - BE * COSP;
	c1 = COSY * SINP;
	c2 = SINY;
	c3 = COSY * COSP;
	goto RTCC_PMMMCD_16_3;
RTCC_PMMMCD_16_4:
	AL = COSP * d1;
	BE = SINP * d1;
	a1 = COSP * d3;
	a2 = d1;
	a3 = -SINP * d3;
	b1 = SINP * d2 - AL * d4;
	b2 = d3 * d4;
	b3 = COSP * d2 + BE * d4;
	c1 = SINP * d4 + AL * d2;
	c2 = -d3 * d2;
	c3 = COSP * d4 - BE * d2;
RTCC_PMMMCD_16_3:
	man.X_B = X_P * a1 + Y_P * a2 + Z_P * a3;
	man.Y_B = X_P * b1 + Y_P * b2 + Z_P * b3;
	man.Z_B = X_P * c1 + Y_P * c2 + Z_P * c3;

RTCC_PMMMCD_17_1:
	if (GMT_begin <= in.StartTimeLimit)
	{
		//Error
	}
	else if (GMT_begin + man.dt >= in.EndTimeLimit)
	{
		//Error
	}
	in.mpt->TimeToBeginManeuver[in.CurrentManeuver - 1U] = GMT_begin;
	in.mpt->TimeToEndManeuver[in.CurrentManeuver - 1U] = GMT_begin;
//RTCC_PMMMCD_17_2:
	return 0;
}

int RTCC::PMMMPT(PMMMPTInput in, MPTManeuver &man)
{
	EMMENIInputTable emsin;
	//Reference orbital elements (post maneuver)
	CELEMENTS ELA;
	EphemerisData sv_gmti, sv_gmti_other;
	VECTOR3 DV_LVLH;
	double WDI[6], TH[6], DELT[6], TIDPS, MASS, DELVB, T, GMTBB, GMTI, WAITA, DT, mu;
	int err = 0;

	int NPHASE = 1;

	T = DT = 0.0;

	if (MPTIsRCSThruster(in.Thruster))
	{
		in.DETU = 0.0;
		in.UT = false;
	}

	double TIMP = in.sv_before.GMT;

	man.HeadsUpDownInd = in.HeadsUpIndicator;
	man.TrajDet[0] = 1;
	man.TrajDet[1] = 1;
	man.TrajDet[2] = 1;
	man.DockingAngle = in.DockingAngle;
	man.Thruster = in.Thruster;
	man.AttitudeCode = in.Attitude;
	man.UllageThrusterOpt = in.UT;
	man.dt_ullage = in.DETU;
	man.ConfigCodeBefore = man.CommonBlock.ConfigCode = in.CONFIG;
	man.TVC = in.VC;
	man.IMPT = TIMP;
	if (in.Thruster == RTCC_ENGINETYPE_LMAPS)
	{
		man.TrimAngleInd = 1;
	}
	else
	{
		man.TrimAngleInd = in.TrimAngleInd;
	}

	if (in.sv_before.RBI == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}
	ELA = OrbMech::GIMIKC(in.sv_before.R, in.V_aft, mu);
	ELA.a /= OrbMech::R_Earth;
	VECTOR3 DV = in.V_aft - in.sv_before.V;
	double dv = length(DV);
	if (in.Attitude < 3)
	{
		man.dv = dv;
	}
	if (dv < 0.01*0.3048)
	{
		DV = _V(0, 0, 0);
		dv = 0.0;
		man.dt_ullage = 0.0;
		WAITA = in.VehicleWeight;
		GMTBB = TIMP;
		GMTI = TIMP;
		goto RTCC_PMMMPT_9_A;
	}

	if (in.Thruster <= 20)
	{
		if (in.Thruster == RTCC_ENGINETYPE_CSMRCSPLUS2)
		{
			WDI[0] = SystemParameters.MCTCW1;
			TH[0] = SystemParameters.MCTCT1;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_CSMRCSPLUS4)
		{
			WDI[0] = SystemParameters.MCTCW2;
			TH[0] = SystemParameters.MCTCT2;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_CSMRCSMINUS2)
		{
			WDI[0] = SystemParameters.MCTCW3;
			TH[0] = SystemParameters.MCTCT3;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_CSMRCSMINUS4)
		{
			WDI[0] = SystemParameters.MCTCW4;
			TH[0] = SystemParameters.MCTCT4;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_LMRCSPLUS2)
		{
			WDI[0] = SystemParameters.MCTLW1;
			TH[0] = SystemParameters.MCTLT1;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_LMRCSPLUS4)
		{
			WDI[0] = SystemParameters.MCTLW2;
			TH[0] = SystemParameters.MCTLT2;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_LMRCSMINUS2)
		{
			WDI[0] = SystemParameters.MCTLW3;
			TH[0] = SystemParameters.MCTLT3;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_LMRCSMINUS4)
		{
			WDI[0] = SystemParameters.MCTLW4;
			TH[0] = SystemParameters.MCTLT4;
		}
		goto RTCC_PMMMPT_8_A;
	}
	if (in.Thruster != RTCC_ENGINETYPE_CSMSPS)
	{
		goto RTCC_PMMMPT_5_A;
	}
	NPHASE = 4;
	if (in.UT)
	{
		WDI[0] = SystemParameters.MCTCW2;
		TH[0] = SystemParameters.MCTCT2;
	}
	else
	{
		WDI[0] = SystemParameters.MCTCW1;
		TH[0] = SystemParameters.MCTCT1;
	}
	DELT[0] = in.DETU - SystemParameters.MCTSD9;
	if (DELT[0] < 0)
	{
		DELT[0] = 0.0;
	}
	TH[1] = SystemParameters.MCTST2;
	TH[2] = SystemParameters.MCTST4;
	TH[3] = SystemParameters.MCTST4;
	WDI[1] = SystemParameters.MCTSW2;
	WDI[2] = SystemParameters.MCTSW4;
	WDI[3] = SystemParameters.MCTSW4;
	DELT[1] = SystemParameters.MCTSD2;
	DELT[2] = SystemParameters.MCTSD3;
	goto RTCC_PMMMPT_7_B;
RTCC_PMMMPT_5_A:
	if (in.UT)
	{
		WDI[0] = SystemParameters.MCTLW2;
		TH[0] = SystemParameters.MCTLT2;
	}
	else
	{
		WDI[0] = SystemParameters.MCTLW1;
		TH[0] = SystemParameters.MCTLT1;
	}
	if (in.Thruster == RTCC_ENGINETYPE_LMDPS)
	{
		goto RTCC_PMMMPT_6_A;
	}
	NPHASE = 4;
	DELT[0] = in.DETU - SystemParameters.MCTAD9;
	if (DELT[0] < 0)
	{
		DELT[0] = 0.0;
	}
	TH[1] = SystemParameters.MCTAT2;
	TH[2] = SystemParameters.MCTAT4;
	TH[3] = SystemParameters.MCTAT4;
	WDI[1] = SystemParameters.MCTAW2;
	WDI[2] = SystemParameters.MCTAW4;
	WDI[3] = SystemParameters.MCTAW4;
	DELT[1] = SystemParameters.MCTAD2;
	DELT[2] = SystemParameters.MCTAD3;
	goto RTCC_PMMMPT_7_B;
RTCC_PMMMPT_6_A:
	man.DPSScaleFactor = in.DPSScaleFactor;
	man.DT_10PCT = in.DT_10PCT;
	TH[1] = SystemParameters.MCTDT2;
	TH[2] = SystemParameters.MCTDT3;
	TH[3] = SystemParameters.MCTDT4;
	WDI[1] = SystemParameters.MCTDW2;
	WDI[2] = SystemParameters.MCTDW3;
	WDI[3] = SystemParameters.MCTDW4;
	DELT[0] = in.DETU - SystemParameters.MCTDD9;
	if (DELT[0] < 0)
	{
		DELT[0] = 0.0;
	}
	DELT[1] = SystemParameters.MCTDD2;
	DELT[2] = SystemParameters.MCTDD3;
	TIDPS = in.DT_10PCT;
	if (TIDPS < 0)
	{
		goto RTCC_PMMMPT_7_A;
	}
	else if (TIDPS == 0.0)
	{
		TIDPS = SystemParameters.MCTDD4;
	}
	MASS = in.VehicleWeight;
	DELVB = dv - SystemParameters.MCTAK4 / MASS;
	double TG = (DELVB / SystemParameters.MCTLT2)*MASS;
	if (TG <= SystemParameters.MCTDD6)
	{
		NPHASE = 4;
		goto RTCC_PMMMPT_7_B;
	}
RTCC_PMMMPT_7_A:
	DELT[3] = abs(TIDPS) - DELT[1] - DELT[2];
	NPHASE = 6;
	TH[4] = SystemParameters.MCTDT5 * in.DPSScaleFactor;
	TH[5] = SystemParameters.MCTDT6 * in.DPSScaleFactor;
	WDI[4] = SystemParameters.MCTDW5 * in.DPSScaleFactor;
	WDI[5] = SystemParameters.MCTDW6 * in.DPSScaleFactor;
	DELT[4] = SystemParameters.MCTDD5;
RTCC_PMMMPT_7_B:
	TH[1] = TH[0] + TH[1];
	WDI[1] = WDI[0] + WDI[1];
	if (in.Thruster != RTCC_ENGINETYPE_LMDPS)
	{
		TH[2] = TH[0] + TH[2];
		WDI[2] = WDI[0] + WDI[2];
	}
RTCC_PMMMPT_8_A:
	PCBBT(DELT, WDI, TH, in.VehicleWeight, TIMP, dv, NPHASE, T, GMTBB, GMTI, WAITA);
	DT = GMTI - GMTBB;
	if (in.IgnitionTimeOption)
	{
		GMTI = TIMP;
		GMTBB = GMTI - DT;
		if (in.Attitude != 3)
		{
			//Rotate DV through 1/2 the burn arc
			VECTOR3 X = unit(crossp(crossp(in.sv_before.R, in.sv_before.V), in.sv_before.R));
			VECTOR3 Y = unit(crossp(in.sv_before.V, in.sv_before.R));
			VECTOR3 Z = -unit(in.sv_before.R);
			double DV_X = dotp(DV, X);
			double DV_Y = dotp(DV, Y);
			double DV_Z = dotp(DV, Z);
			VECTOR3 DV_P = X * DV_X + Z * DV_Z;
			double theta_T = length(crossp(in.sv_before.R, in.sv_before.V))*length(DV)*in.VehicleWeight / (pow(length(in.sv_before.R), 2)*GetOnboardComputerThrust(in.Thruster));
			VECTOR3 DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, Y))*sin(theta_T / 2.0))*length(DV_P);
			DV = DV_C + Y * DV_Y;
		}
	}
	if (in.Attitude < 4)
	{
		man.A_T = DV / dv;
	}
RTCC_PMMMPT_9_A:
	if (in.Attitude == 3)
	{
		//TBD: Lambert
	}
	if (in.Attitude > 3)
	{
		man.FrozenManeuverInd = true;
	}
	//Go to iterate logic if it was requested and the predicted main engine burn time is at least 10 seconds
	if (in.IterationFlag && T - DT > 10.0)
	{
		goto RTCC_PMMMPT_12_A;
	}
RTCC_PMMMPT_10_A:
	man.FrozenManeuverInd = false;
	if (GMTBB < in.LowerTimeLimit)
	{
		return true;
	}
	if (in.Attitude <= 3)
	{
		goto RTCC_PMMMPT_11_B;
	}
	if (in.Attitude > 4)
	{
		goto RTCC_PMMMPT_11_A;
	}
	emsin.AnchorVector = in.sv_before;
	emsin.MaxIntegTime = abs(GMTI - TIMP);
	if (GMTI - TIMP >= 0.0)
	{
		emsin.IsForwardIntegration = 1.0;
	}
	else
	{
		emsin.IsForwardIntegration = -1.0;
	}
	EMMENI(emsin);
	sv_gmti = emsin.sv_cutoff;
	DV_LVLH = PIEXDV(sv_gmti.R, sv_gmti.V, in.VehicleWeight, GetOnboardComputerThrust(in.Thruster), DV, false);
	man.dV_inertial = DV;
	man.dV_LVLH = DV_LVLH;
	man.Word67i[0] = 1;
	goto RTCC_PMMMPT_11_B;
RTCC_PMMMPT_11_A:
	emsin.AnchorVector = in.sv_other;
	emsin.MaxIntegTime = abs(GMTBB - TIMP);
	if (GMTBB - TIMP >= 0.0)
	{
		emsin.IsForwardIntegration = 1.0;
	}
	else
	{
		emsin.IsForwardIntegration = -1.0;
	}
	EMMENI(emsin);
	sv_gmti_other = emsin.sv_cutoff;

	emsin.AnchorVector = in.sv_before;
	EMMENI(emsin);
	sv_gmti = emsin.sv_cutoff;

	DV_LVLH = PIAEDV(DV, sv_gmti_other.R, sv_gmti_other.V, sv_gmti.R, false);
	man.dV_LVLH = DV_LVLH;
	man.Word67i[0] = 1;
RTCC_PMMMPT_11_B:
	in.mpt->TimeToBeginManeuver[in.CurrentManeuver - 1] = in.mpt->TimeToEndManeuver[in.CurrentManeuver - 1] = GMTBB;
	man.GMTMAN = GMTBB;
	man.GMTI = GMTI;
	//WEIGHT = WAITA;
	if (in.VC == 1)
	{
		//Update CSM weight
	}
	else
	{
		//Update LM weight
	}
	return false;
RTCC_PMMMPT_12_A:
	PMMRKJInputArray integin;

	integin.A = in.VehicleArea;
	integin.DENSMULT = 1.0;
	integin.DOCKANG = man.DockingAngle;
	integin.DPSScale = man.DPSScaleFactor;
	integin.DTPS10 = man.DT_10PCT;
	integin.DTU = man.dt_ullage;
	integin.HeadsUpDownInd = man.HeadsUpDownInd;
	integin.IC = man.CommonBlock.ConfigCode.to_ulong();
	integin.KAUXOP = 1;
	integin.KEPHOP = 0;
	integin.KTRIMOP = man.TrimAngleInd;
	if (man.CommonBlock.ConfigCode[RTCC_CONFIG_D] == true)
	{
		integin.LMDESCJETT = 1e70;
	}
	else
	{
		integin.LMDESCJETT = 0;
	}
	integin.MANOP = 4;
	integin.ThrusterCode = man.Thruster;
	integin.TVC = man.TVC;
	integin.UllageOption = man.UllageThrusterOpt;
	integin.ExtDVCoordInd = false;
	integin.WDMULT = 1.0;
	integin.XB = man.X_B;
	integin.YB = man.Y_B;
	integin.ZB = man.Z_B;
	integin.CAPWT = in.VehicleWeight;
	integin.CSMWT = 0.0;
	integin.LMAWT = 0.0;
	integin.LMDWT = 0.0;
	integin.SIVBWT = 0.0;
	if (man.TVC == RTCC_MANVEHICLE_CSM)
	{
		integin.CSMWT = in.VehicleWeight;
	}
	else if (man.TVC == RTCC_MANVEHICLE_LM)
	{
		integin.LMDWT = in.VehicleWeight;
	}
	else
	{
		integin.SIVBWT = in.VehicleWeight;
	}

	int Ierr;
	RTCCNIAuxOutputTable aux;

	CSMLMPoweredFlightIntegration numin(this, integin, Ierr, NULL, &aux);

	double SUM;
	int ITCNT;

	SUM = 1.0e75;
	ITCNT = 0;
	VECTOR3 ATV;
	if (man.AttitudeCode >= 4)
	{
		ATV = DV / length(DV);
	}
	else
	{
		ATV = man.A_T;
	}
	EphemerisData sv_BO, sv_impt;
	VECTOR3 H, YT;
	double P, Y, PARM[6], SUMN, DA[6], DAS[6], X, LAMI, LAMP, WX[6], WY[6], EPS[6], SAVEX, DP[6], TIG, YV[6], DELP[6], LAML;
	int ICNT, i, ITMAX, J, IPI, ii, NIP, IPT;
	bool IP[6]; //Switches on/off independent variables (Pitch, Yaw, DV, TIG, Lambert time of flight, Lambert C)
	bool IC[6]; //Switch on/off dependent variables (orbital elements)
	bool IFLAG, DFLAG;
	CELEMENTS ELPRES, OSCELB;

	double **A, **XP;
	A = new double*[6];
	XP = new double*[6];
	for (i = 0;i < 6;i++)
	{
		A[i] = new double[6];
		XP[i] = new double[6];
		for (ii = 0;ii < 6;ii++)
		{
			A[i][ii] = 0.0;
			XP[i][ii] = 0.0;
		}
	}

	//Constants
	double WNULL[6] = { 0,0,0,0,0,0 };
	LAMI = pow(2, -28);
	LAML = 1.0;
	//If eccentricity is close to 1 then the semi-major axis "a" becomes very large (above 1 even negative, so a shouldn't be used near 1). So widen the tolerance on "a" near e = 1.
	if (abs(ELA.e - 1.0) < 0.001)
	{
		EPS[0] = abs(ELA.a) / (OrbMech::R_Earth*10.0);
	}
	else
	{
		EPS[0] = 1.0 / OrbMech::R_Earth;
	}
	EPS[1] = 0.0001; //Eccentricity
	EPS[2] = 0.00001; //Inclination
	EPS[3] = 0.00001; //Longitude of the ascending node
	EPS[4] = 0.00002; //Argument of periapsis

	WX[0] = 1.0;
	WX[1] = 1.0;
	WX[2] = 0.01;
	WX[3] = 0.01;
	WX[4] = 1.0;
	WX[5] = 1.0;
	for (i = 0;i < 5;i++)
	{
		WY[i] = pow(2, -40) / pow(EPS[i], 2);
	}
	ITMAX = 5;
	DP[0] = 0.001*RAD;
	DP[1] = 0.001*RAD;
	DP[2] = 0.1*0.3048;
	DP[3] = 0.01;
	IC[0] = IC[1] = IC[2] = IC[3] = IC[4] = true;
	IC[5] = false;

	//If eccentricity is smaller than 0.005 switch it off as iterating variable
	if (ELA.e < 0.005)
	{
		IC[4] = false;
	}

	PARM[3] = GMTBB;
	IP[3] = true;
	TIG = GMTBB;

	emsin.AnchorVector = in.sv_before;
	emsin.MaxIntegTime = abs(TIG - TIMP);
	if (TIG - TIMP >= 0.0)
	{
		emsin.IsForwardIntegration = 1.0;
	}
	else
	{
		emsin.IsForwardIntegration = -1.0;
	}
	EMMENI(emsin);
	sv_gmti = emsin.sv_cutoff;

	LAMP = LAMI * 10.0;
	NIP = 6;
	if (man.AttitudeCode == 3)
	{
		goto RTCC_PMMMPT_14_A;
	}
	IP[0] = true;
	IP[1] = true;
	IP[2] = true;
	IP[4] = false;
	IP[5] = false;
	PARM[0] = P = asin(ATV.z);
	if (abs(P - PI05) >= 0.0017)
	{
		PARM[1] = Y = atan2(ATV.y, ATV.x);
	}
	else
	{
		H = crossp(ATV, sv_gmti.R / OrbMech::R_Earth);
		if (length(H) >= 0.001)
		{
			YT = unit(H);
		}
		else
		{
			YT = unit(crossp(sv_gmti.V, sv_gmti.R));
		}
		Y = PARM[1] = atan2(YT.y, YT.x);
	}
	PARM[2] = dv;
	NIP = 4;
	goto RTCC_PMMMPT_14_B;
RTCC_PMMMPT_14_A:
	IP[0] = false;
	IP[1] = false;
	IP[2] = false;
	IP[4] = true;
	IP[5] = true;
	//TBD: Lambert
	goto RTCC_PMMMPT_14_B;
RTCC_PMMMPT_14_C:
	if (DFLAG)
	{
		for (i = 0;i < 6;i++)
		{
			DAS[i] = DA[i];
		}
	}
RTCC_PMMMPT_14_B:
	//Input parameters
	integin.sv0 = sv_gmti;
	ATV = _V(cos(PARM[1])*cos(PARM[0]), sin(PARM[1])*cos(PARM[0]), sin(PARM[0]));
	integin.VG = ATV * PARM[2];

	numin.PMMRKJ();
	sv_BO.R = aux.R_BO;
	sv_BO.V = aux.V_BO;
	sv_BO.GMT = aux.GMT_BO;
	sv_BO.RBI = aux.RBI;

	emsin.AnchorVector = sv_BO;
	emsin.MaxIntegTime = abs(TIMP - sv_BO.GMT);
	if (TIMP - sv_BO.GMT >= 0.0)
	{
		emsin.IsForwardIntegration = 1.0;
	}
	else
	{
		emsin.IsForwardIntegration = -1.0;
	}
	EMMENI(emsin);
	sv_impt = emsin.sv_cutoff;

	ELPRES = OrbMech::GIMIKC(sv_impt.R, sv_impt.V, mu);
	ELPRES.a /= OrbMech::R_Earth;
	SUMN = 0.0;
	IFLAG = false;
	ICNT = 0;
	for (i = 0;i < 6;i++)
	{
		if (IC[i])
		{
			DA[ICNT] = X = ELA.data[i] - ELPRES.data[i];
			ICNT++;
			SUMN = SUMN + WY[i] * X*X;
			if (abs(X) > EPS[i])
			{
				IFLAG = true;
			}
		}
	}
	if (IFLAG)
	{
		goto RTCC_PMMMPT_16_B;
	}
	if (ITCNT != 0)
	{
		//Move burn parameters from integrator input table to PMMMPT output area
		DV = integin.VG;
		GMTBB = integin.sv0.GMT;
		GMTI = GMTBB + DT;
	}
	goto RTCC_PMMMPT_10_A;
RTCC_PMMMPT_16_B:
	if (ITCNT != 0)
	{
		//Did fit improve?
		if (SUMN >= SUM)
		{
			//No
			goto RTCC_PMMMPT_20_A;
		}
	}
	if (LAMP > LAMI)
	{
		LAMP = LAMP / 10.0;
	}
	//Save burn parameters
	DFLAG = true;
	SUM = SUMN;
	ITCNT++;
	if (ITCNT > ITMAX)
	{
		//Put best burn parameters in PMMMPT output area - print message
		DV = integin.VG;
		GMTBB = integin.sv0.GMT;
		GMTI = GMTBB + DT;
		PMXSPT("PMMMPT", 200);
		goto RTCC_PMMMPT_10_A;
	}
	J = 0;
RTCC_PMMMPT_17_B:
	if (IP[J] == false)
	{
		goto RTCC_PMMMPT_19_B;
	}
	SAVEX = PARM[J];
	PARM[J] = PARM[J] + DP[J];
	if (TIG != PARM[3])
	{
		TIG = PARM[3];
		emsin.AnchorVector = in.sv_before;
		emsin.MaxIntegTime = abs(TIG - in.sv_before.GMT);
		if (TIG - in.sv_before.GMT >= 0.0)
		{
			emsin.IsForwardIntegration = 1.0;
		}
		else
		{
			emsin.IsForwardIntegration = -1.0;
		}
		EMMENI(emsin);
		sv_gmti = emsin.sv_cutoff;
	}
	//Update PMMRKJ input table
	integin.sv0 = sv_gmti;
	ATV = _V(cos(PARM[1])*cos(PARM[0]), sin(PARM[1])*cos(PARM[0]), sin(PARM[0]));
	integin.VG = ATV * PARM[2];
	numin.PMMRKJ();
	sv_BO.R = aux.R_BO;
	sv_BO.V = aux.V_BO;
	sv_BO.GMT = aux.GMT_BO;
	sv_BO.RBI = aux.RBI;

	emsin.AnchorVector = sv_BO;
	emsin.MaxIntegTime = abs(TIMP - sv_BO.GMT);
	if (TIMP - sv_BO.GMT >= 0.0)
	{
		emsin.IsForwardIntegration = 1.0;
	}
	else
	{
		emsin.IsForwardIntegration = -1.0;
	}
	EMMENI(emsin);
	sv_impt = emsin.sv_cutoff;

	OSCELB = OrbMech::GIMIKC(sv_impt.R, sv_impt.V, mu);
	OSCELB.a /= OrbMech::R_Earth;
	IPI = 0;
	for (ii = 0;ii < 6;ii++)
	{
		if (IC[ii])
		{
			XP[IPI][J] = (OSCELB.data[ii] - ELPRES.data[ii]) / DP[J];
			IPI++;
		}
	}
	PARM[J] = SAVEX;
RTCC_PMMMPT_19_B:
	J++;
	if (J <= 5)
	{
		goto RTCC_PMMMPT_17_B;
	}	
	PCMATO(A, YV, XP, DA, ICNT, NIP, WY, 0.0, WNULL);
	goto RTCC_PMMMPT_20_B;
RTCC_PMMMPT_20_A:
	//Reset independent variables
	LAMP = LAMP * 10.0;
	DFLAG = false;
	for (i = 0;i < 6;i++)
	{
		DA[i] = DAS[i];
	}
	if (LAMP > LAML)
	{
		//Put best burn parameters in PMMMPT output area - print message
		PMXSPT("PMMMPT", 200);
		goto RTCC_PMMMPT_10_A;
	}
RTCC_PMMMPT_20_B:
	for (i = 0;i < NIP;i++)
	{
		A[i][i] = A[i][i] + WX[i] * LAMP;
	}
	PCGAUS(A, YV, DELP, NIP, 0.0);
	IPT = 0;
	for (i = 0;i < 6;i++)
	{
		if (IP[i])
		{
			PARM[i] = PARM[i] + DELP[IPT];
			IPT++;
		}
	}
	TIG = PARM[3];
	emsin.AnchorVector = in.sv_before;
	emsin.MaxIntegTime = abs(TIG - in.sv_before.GMT);
	if (TIG - in.sv_before.GMT >= 0.0)
	{
		emsin.IsForwardIntegration = 1.0;
	}
	else
	{
		emsin.IsForwardIntegration = -1.0;
	}
	EMMENI(emsin);
	sv_gmti = emsin.sv_cutoff;
	goto RTCC_PMMMPT_14_C;

	delete[] A;
	delete[] XP;
}

int RTCC::PMMLAI(PMMLAIInput in, RTCCNIAuxOutputTable &aux, EphemerisDataTable2 *E)
{
	EphemerisData2 data;
	VECTOR3 R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.rad[RTCC_LMPOS_BEST]);
	double theta, dt_asc, dv;
	SV sv_IG, sv_Ins;
	LunarAscentProcessor(R_LS, in.m0, in.sv_CSM, GETfromGMT(in.t_liftoff), in.v_LH, in.v_LV, theta, dt_asc, dv, sv_IG, sv_Ins);

	if (E)
	{
		data.R = sv_IG.R;
		data.V = sv_IG.V;
		data.GMT = OrbMech::GETfromMJD(sv_IG.MJD, SystemParameters.GMTBASE);
		E->table.push_back(data);
	}

	aux.A_T = unit(sv_IG.R);
	aux.DT_B = (sv_Ins.MJD - sv_IG.MJD)*24.0*3600.0;
	aux.DT_TO = 0.0;
	aux.DV = dv;
	aux.DV_C = dv;
	aux.DV_cTO = 0.0;
	aux.DV_TO = 0.0;
	aux.DV_U = 0.0;
	aux.MainFuelUsed = sv_IG.mass - sv_Ins.mass;
	aux.P_G = 0.0;
	aux.RCSFuelUsed = 0.0;
	aux.CSI = 2;
	aux.RBI = BODY_MOON;
	aux.R_1 = sv_IG.R;
	aux.V_1 = sv_IG.V;
	aux.GMT_1 = OrbMech::GETfromMJD(sv_IG.MJD,SystemParameters.GMTBASE);
	aux.R_BI = sv_IG.R;
	aux.V_BI = sv_IG.V;
	aux.GMT_BI = OrbMech::GETfromMJD(sv_IG.MJD,SystemParameters.GMTBASE);
	aux.R_BO = sv_Ins.R;
	aux.V_BO = sv_Ins.V;
	aux.GMT_BO = OrbMech::GETfromMJD(sv_Ins.MJD,SystemParameters.GMTBASE);
	aux.sv_FF.R = sv_IG.R;
	aux.sv_FF.V = sv_IG.V;
	aux.sv_FF.GMT = OrbMech::GETfromMJD(sv_IG.MJD,SystemParameters.GMTBASE);
	if (sv_IG.gravref == hEarth)
	{
		aux.sv_FF.RBI = BODY_EARTH;
	}
	else
	{
		aux.sv_FF.RBI = BODY_MOON;
	}
	aux.V_G = aux.A_T*dv;
	aux.WTEND = sv_Ins.mass;
	aux.WTENGON = sv_IG.mass;
	aux.W_CSM = 0.0;
	aux.W_LMA = sv_IG.mass;
	aux.W_LMD = 0.0;
	aux.W_SIVB = 0.0;
	aux.X_B = _V(1, 0, 0);
	aux.Y_B = _V(0, 1, 0);
	aux.Y_G = 0.0;
	aux.Z_B = _V(0, 0, 1);

	if (E)
	{
		data.R = sv_Ins.R;
		data.V = sv_Ins.V;
		data.GMT = OrbMech::GETfromMJD(sv_Ins.MJD,SystemParameters.GMTBASE);
		E->table.push_back(data);

		//Header
		E->Header.CSI = 2;
		E->Header.NumVec = E->table.size();
		E->Header.TL = E->table.front().GMT;
		E->Header.TR = E->table.back().GMT;
	}

	return 0;
}

int RTCC::PMMLDI(PMMLDIInput in, RTCCNIAuxOutputTable &aux, EphemerisDataTable2 *E)
{
	EphemerisData2 data;
	SV sv_PDI, sv_land;
	double dv;
	VECTOR3 R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.rad[RTCC_LMPOS_BEST]);

	PoweredDescentProcessor(R_LS, in.TLAND, in.sv, aux, E, sv_PDI, sv_land, dv);

	if (E)
	{
		data.R = sv_PDI.R;
		data.V = sv_PDI.V;
		data.GMT = OrbMech::GETfromMJD(sv_PDI.MJD, SystemParameters.GMTBASE);
		E->table.push_back(data);
	}

	aux.A_T = _V(1, 0, 0);
	aux.DT_B = (sv_land.MJD - sv_PDI.MJD)*24.0*3600.0;
	aux.DT_TO = 0.0;
	aux.DV = dv;
	aux.DV_C = dv;
	aux.DV_cTO = 0.0;
	aux.DV_TO = 0.0;
	aux.DV_U = 0.0;
	aux.MainFuelUsed = sv_PDI.mass - sv_land.mass;
	aux.P_G = 0.0;
	aux.RCSFuelUsed = 0.0;
	aux.CSI = 2;
	aux.RBI = BODY_MOON;
	aux.R_1 = sv_PDI.R;
	aux.V_1 = sv_PDI.V;
	aux.GMT_1 = OrbMech::GETfromMJD(sv_PDI.MJD,SystemParameters.GMTBASE);
	aux.R_BI = sv_PDI.R;
	aux.V_BI = sv_PDI.V;
	aux.GMT_BI = OrbMech::GETfromMJD(sv_PDI.MJD,SystemParameters.GMTBASE);
	aux.R_BO = sv_land.R;
	aux.V_BO = sv_land.V;
	aux.GMT_BO = OrbMech::GETfromMJD(sv_land.MJD,SystemParameters.GMTBASE);
	aux.sv_FF.R = sv_PDI.R;
	aux.sv_FF.V = sv_PDI.V;
	aux.sv_FF.GMT = OrbMech::GETfromMJD(sv_PDI.MJD,SystemParameters.GMTBASE);
	if (sv_PDI.gravref == hEarth)
	{
		aux.sv_FF.RBI = BODY_EARTH;
	}
	else
	{
		aux.sv_FF.RBI = BODY_MOON;
	}
	aux.V_G = _V(1, 0, 0)*dv;
	aux.WTEND = sv_land.mass;
	aux.WTENGON = sv_PDI.mass;
	aux.W_CSM = 0.0;
	aux.W_LMA = in.W_LMA;
	aux.W_LMD = sv_PDI.mass - in.W_LMA;
	aux.W_SIVB = 0.0;
	aux.X_B = _V(1, 0, 0);
	aux.Y_B = _V(0, 1, 0);
	aux.Y_G = 0.0;
	aux.Z_B = _V(0, 0, 1);

	if (E)
	{
		data.R = sv_land.R;
		data.V = sv_land.V;
		data.GMT = OrbMech::GETfromMJD(sv_land.MJD,SystemParameters.GMTBASE);
		E->table.push_back(data);

		//Header
		E->Header.CSI = 2;
		E->Header.NumVec = E->table.size();
		E->Header.TL = E->table.front().GMT;
		E->Header.TR = E->table.back().GMT;
	}

	return 0;
}

int RTCC::PMMLDP(PMMLDPInput in, MPTManeuver &man)
{
	SV sv_IG;
	MATRIX3 REFSMMAT;
	VECTOR3 U_IG;
	double GMTBB, CR, t_go;

	VECTOR3 R_LS = OrbMech::r_from_latlong(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], BZLAND.rad[RTCC_LMPOS_BEST]);

	if (!PDIIgnitionAlgorithm(in.sv, R_LS, in.TLAND, sv_IG, t_go, CR, U_IG, REFSMMAT))
	{
		return 1;
	}

	GMTBB = OrbMech::GETfromMJD(sv_IG.MJD,SystemParameters.GMTBASE);

	man.CommonBlock.ConfigCode[RTCC_CONFIG_C] = false;
	man.CommonBlock.ConfigCode[RTCC_CONFIG_S] = false;
	man.CommonBlock.ConfigCode[RTCC_CONFIG_A] = true;
	man.CommonBlock.ConfigCode[RTCC_CONFIG_D] = true;
	man.AttitudeCode = RTCC_ATTITUDE_PGNS_DESCENT;
	man.Thruster = RTCC_ENGINETYPE_LMDPS;
	man.ConfigCodeBefore = man.CommonBlock.ConfigCode;
	man.TVC = RTCC_MANVEHICLE_LM;
	man.TrimAngleInd = in.TrimAngleInd - 1;
	man.HeadsUpDownInd = in.HeadsUpDownInd;
	if (in.CurrentManeuver > 0)
	{
		man.DockingAngle = in.mpt->mantable[in.CurrentManeuver - 1].DockingAngle;
	}
	else
	{
		man.DockingAngle = in.mpt->DeltaDockingAngle;
	}
	man.GMTMAN = GMTBB;// -7.5;
	man.GMTI = GMTBB;
	man.GMT_BO = GMTfromGET(in.TLAND);
	in.mpt->TimeToBeginManeuver[in.CurrentManeuver] = in.mpt->TimeToEndManeuver[in.CurrentManeuver] = GMTBB;

	return 0;
}

void RTCC::PMMCEN(EphemerisData sv, double tmin, double tmax, int opt, double endcond, double dir, EphemerisData &sv_out, int &ITS)
{
	//INPUT
	//sv: input state vector
	//tmin: minimum time to integrate before beginning to search for ending condition
	//tmax: maximum time to integrate for desired ending condition (not used for time option)
	//opt: type of stop indicator, 1 = time, 2 = flight path angle, 3 = radius
	//endcond: desired ending condition, for option 2 the sine of the flight path angle
	//dir: Search direction, +1.0 for forwards, -1.0 for backwards
	
	//OUTPUT
	//sv_out: output state vector
	//ITS: type of stop indicator (see opt)

	CoastIntegrator2 pmmcen(this);

	pmmcen.Propagate(sv.R, sv.V, sv.GMT, tmax, tmin, endcond, dir, sv.RBI, opt);

	sv_out.R = pmmcen.R2;
	sv_out.V = pmmcen.V2;
	sv_out.GMT = pmmcen.T2;
	sv_out.RBI = pmmcen.outplanet;
	ITS = pmmcen.ITS;
}

void RTCC::PMMFUD(int veh, unsigned man, int action, std::string StationID)
{
	bool tupind = false;
	MissionPlanTable *mpt = GetMPTPointer(veh);

	if (man > mpt->ManeuverNum)
	{
		//Error 40
		PMXSPT("PMMFUD", 40);
		return;
	}

	//Future Delete
	if (action == 0)
	{
		//Maneuver executed?
		if (man <= mpt->LastExecutedManeuver)
		{
			//Error 43
			PMXSPT("PMMFUD", 43);
			return;
		}
		//Maneuver frozen?
		if (mpt->mantable[man - 1].FrozenManeuverInd)
		{
			//Error 47
			PMXSPT("PMMFUD", 47);
			return;
		}

		while (mpt->mantable.size() >= man)
		{
			mpt->mantable.pop_back();
		}

		mpt->ManeuverNum = mpt->mantable.size();
		//TBD: Is TLI maneuver being deleted?
		//TBD: Is deorbit maneuver being deleted?
		//Is the next upcoming maneuver being deleted?
		if (man == mpt->LastExecutedManeuver + 1)
		{
			mpt->UpcomingManeuverGMT = 1e70;
		}
		mpt->CommonBlock.TUP = -mpt->CommonBlock.TUP;
		PMSVCT(8, veh);
	}
	//History Delete
	else if (action == 1)
	{
		//Are all maneuvers to be deleted executed?
		if (man > mpt->LastExecutedManeuver)
		{
			//Error 42
			PMXSPT("PMMFUD", 42);
			return;
		}

		if (mpt->LastExecutedManeuver > 0)
		{
			mpt->LastExecutedManeuver -= man;
		}
		if (mpt->LastFrozenManeuver > 0)
		{
			mpt->LastFrozenManeuver -= man;
		}

		for (unsigned i = man;i < mpt->ManeuverNum;i++)
		{
			mpt->TimeToBeginManeuver[i - man] = mpt->TimeToBeginManeuver[i];
			mpt->TimeToEndManeuver[i - man] = mpt->TimeToEndManeuver[i];
			mpt->AreaAfterManeuver[i - man] = mpt->AreaAfterManeuver[i];
			mpt->WeightAfterManeuver[i - man] = mpt->WeightAfterManeuver[i];
		}

		mpt->ManeuverNum -= man;
		mpt->DeltaDockingAngle = mpt->mantable[man - 1].DockingAngle;
		mpt->ConfigurationArea = mpt->mantable[man - 1].TotalAreaAfter;
		mpt->CommonBlock = mpt->mantable[man - 1].CommonBlock;
		//TBD: Compute and store new GET to begin venting

		for (unsigned i = 0;i < man;i++)
		{
			mpt->mantable.pop_front();
		}

		//Any remaining maneuvers?
		if (mpt->ManeuverNum > 0)
		{
			mpt->CommonBlock.TUP = -abs(mpt->CommonBlock.TUP);
		}
		//TBD: Move maneuvers up in the table here
		if (mpt->CommonBlock.TUP < 0)
		{
			mpt->CommonBlock.TUP = abs(mpt->CommonBlock.TUP);
		}
		EMSNAP(veh, 2);
	}
	//Freeze
	else if (action == 2)
	{
		//Maneuver executed?
		if (man <= mpt->LastExecutedManeuver)
		{
			//Error 10
			PMXSPT("PMMFUD", 10);
			return;
		}
		//Determine 1st maneuver to be frozen
		unsigned i = mpt->LastExecutedManeuver + 1;
		if (StationID != "")
		{
			goto RTCC_PMMFUD_9_1;
		}
		//Set trajectory number negative
		mpt->CommonBlock.TUP = -abs(mpt->CommonBlock.TUP);

		do
		{
			if (mpt->mantable[i - 1].AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
			{
				//Is this a re-freeze?
				if (mpt->mantable[i - 1].FrozenManeuverInd)
				{
					//Is the TLI maneuver iterable?
					if (mpt->mantable[i - 1].Word78i[1] != 0)
					{

					}
					else
					{

					}
				}
			}
			else
			{
				mpt->mantable[i - 1].FrozenManeuverVector.GMT = mpt->mantable[i - 1].GMTMAN;
				mpt->mantable[i - 1].FrozenManeuverVector.R = mpt->mantable[i - 1].R_1;
				mpt->mantable[i - 1].FrozenManeuverVector.V = mpt->mantable[i - 1].V_1;
				mpt->mantable[i - 1].FrozenManeuverVector.RBI = mpt->mantable[i - 1].RefBodyInd;
			}
			//External DV
			if (mpt->mantable[i - 1].AttitudeCode >= 4 && mpt->mantable[i - 1].AttitudeCode <= 5)
			{
				mpt->mantable[i - 1].Word67i[0] = 0;
			}
			mpt->mantable[i - 1].FrozenManeuverInd = true;
			i++;
		} while (i <= man);

		PMSVCT(8, veh);

	RTCC_PMMFUD_9_1:
		//Is an evaluation vector being requested?
		if (StationID.length() >= 4 && StationID[3] == 'E')
		{
			//Error 41
			PMXSPT("PMMFUD", 41);
			return;
		}
		EphemerisData sv_out;
		int err = BMSVPSVectorFetch(StationID, sv_out);
		//Error?
		if (err)
		{
			//Error 41
			PMXSPT("PMMFUD", 41);
			return;
		}
		//Is vector prior to maneuver?
		if (sv_out.GMT > mpt->mantable[i - 1].GMTMAN)
		{
			//No, Error
			PMXSPT("PMMFUD", 44);
			return;
		}
		//Is vector within a prior maneuver?
		//TBD
		//Must vector be moved through a frozen TLI?
		//TBD
		//Set trajectory number negative
		mpt->CommonBlock.TUP = -abs(mpt->CommonBlock.TUP);

		//Move anchor vector to GMTBB
		EMSMISSInputTable intab;

		intab.AnchorVector = sv_out;

		EMSMISS(&intab);
		//TBD
	}
	//Unfreeze
	else
	{
		//Maneuver executed?
		if (man <= mpt->LastExecutedManeuver)
		{
			//Error 46
			PMXSPT("PMMFUD", 46);
			return;
		}
		//Maneuver frozen?
		if (mpt->mantable[man - 1].FrozenManeuverInd == false)
		{
			//Error 45
			PMXSPT("PMMFUD", 45);
			return;
		}
		//Decrement input number by 1 and store as number of last frozen maneuver in MPT
		mpt->LastFrozenManeuver = man - 1;
		
		mpt->CommonBlock.TUP = -abs(mpt->CommonBlock.TUP);
		unsigned curman = man;
		while (curman <= mpt->ManeuverNum)
		{
			//Is this the TLI maneuver?
			if (mpt->mantable[curman - 1].AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
			{
				mpt->mantable[curman - 1].Word78i[0] = mpt->mantable[curman - 1].Word78i[1];
				tupind = true;
			}
			else if (mpt->mantable[curman - 1].AttitudeCode == RTCC_ATTITUDE_LAMBERT)
			{
				tupind = true;
			}
			//External DV
			else if (mpt->mantable[curman - 1].AttitudeCode > 3 && mpt->mantable[curman - 1].AttitudeCode < 6)
			{
				mpt->mantable[curman - 1].Word67i[0] = 1;
				tupind = true;
			}
			//IGM again??
			else if (mpt->mantable[curman - 1].AttitudeCode >= 6)
			{
				tupind = true;
			}
			mpt->mantable[curman - 1].FrozenManeuverInd = false;
			curman++;
		}
		if (tupind)
		{
			PMSVCT(8, veh);
		}
		else
		{
			mpt->CommonBlock.TUP = abs(mpt->CommonBlock.TUP);
		}
	}
}

void RTCC::PMMUDT(int L, unsigned man, int headsup, int trim)
{
	MissionPlanTable *tab;
	if (L == RTCC_MPT_CSM)
	{
		tab = &PZMPTCSM;
	}
	else
	{
		tab = &PZMPTLEM;
	}
	//Is requested maneuver in MPT?
	if (tab->mantable.size() < man)
	{
		//Error 53
		PMXSPT("PMMUDT", 53);
		return;
	}
	MPTManeuver *pMan = &tab->mantable[man - 1];
	//Has requested maneuver been executed?
	if (man <= tab->LastExecutedManeuver)
	{
		//Error 54
		PMXSPT("PMMUDT", 54);
		return;
	}
	//Is requested maneuver frozen?
	if (pMan->FrozenManeuverInd)
	{
		//Error 56
		PMXSPT("PMMUDT", 56);
		return;
	}
	if (pMan->AttitudeCode == RTCC_ATTITUDE_SIVB_IGM || pMan->AttitudeCode == RTCC_ATTITUDE_AGS_ASCENT || pMan->AttitudeCode == RTCC_ATTITUDE_PGNS_ASCENT)
	{
		//Error 55
		PMXSPT("PMMUDT", 55);
		return;
	}
	if (headsup >= 0)
	{
		if (headsup == 1)
		{
			pMan->HeadsUpDownInd = true;
		}
		else
		{
			pMan->HeadsUpDownInd = false;
		}
		pMan->AttitudesInput = false;
	}
	if (trim >= 0)
	{
		pMan->TrimAngleInd = trim - 1;
	}
	PMSVCT(8, L);
}

void RTCC::EMSTRAJ(StateVectorTableEntry sv, int L)
{
	MissionPlanTable *table;
	OrbitEphemerisTable *maineph;
	CapeCrossingTable *cctab;
	TimeConstraintsTable *tctab;
	StateVectorTableEntry sv1;

	double gmt;

	if (L == RTCC_MPT_CSM)
	{
		table = &PZMPTCSM;
		maineph = &EZEPH1;
		cctab = &EZCCSM;
		tctab = &EZTSCNS1;
	}
	else
	{
		table = &PZMPTLEM;
		maineph = &EZEPH2;
		cctab = &EZCLEM;
		tctab = &EZTSCNS3;
	}

	gmt = RTCCPresentTimeGMT();

	//Store as anchor vector
	EMGVECSTInput(L, sv);

	//Store Station ID
	tctab->StationID = sv.VectorCode;

	//Generate main ephemeris
	sv1 = sv;
	EMSEPH(2, sv1, L, gmt);
	if (sv.LandingSiteIndicator)
	{
		cctab->NumRev = 0;
		EMGPRINT("EMSTRAJ", 53);
	}
	else
	{
		//Generate cape crossing
		RMMEACC(L, sv.Vector.RBI, 0, CapeCrossingRev(L, gmt));

		//Orbital elements
		if (L == RTCC_MPT_CSM)
		{
			RTCCONLINEMON.TextBuffer[0] = "CSM";
		}
		else
		{
			RTCCONLINEMON.TextBuffer[0] = "LEM";
		}
		TimeConstraintsTable temptab;
		EMMDYNEL(sv.Vector, temptab);
		RTCCONLINEMON.IntBuffer[0] = maineph->EPHEM.Header.TUP;
		RTCCONLINEMON.DoubleBuffer[0] = temptab.a / 1852.0;
		RTCCONLINEMON.DoubleBuffer[1] = temptab.MA*DEG;
		RTCCONLINEMON.DoubleBuffer[2] = temptab.h_a / 1852.0;
		if (RTCCONLINEMON.DoubleBuffer[2] > 999999.0)
		{
			RTCCONLINEMON.DoubleBuffer[2] = 999999.0;
		}
		RTCCONLINEMON.DoubleBuffer[3] = temptab.e;
		RTCCONLINEMON.DoubleBuffer[4] = temptab.AoP*DEG;
		RTCCONLINEMON.DoubleBuffer[5] = temptab.h_p / 1852.0;
		RTCCONLINEMON.DoubleBuffer[6] = temptab.i*DEG;
		RTCCONLINEMON.DoubleBuffer[7] = temptab.RA*DEG;
		EMGPRINT("EMSTRAJ", 14);
	}
	//Generate station contacts
	EMSTAGEN(L);
	//Update displays
	EMSNAP(L, 1);
}

void RTCC::EMSEPH(int QUEID, StateVectorTableEntry &sv, int &L, double PresentGMT)
{
	//QUEID: 1 = Cutoff mode, 2 = Ephemeris mode
	//sv: Input and output state vector
	//L: 1 = CSM, 3 = LM. Set to a negative value as error indication

	EMSMISSInputTable InTable;
	EphemerisDataTable2 temptable;
	ManeuverTimesTable tempmantable;
	OrbitEphemerisTable *table;
	StateVectorTableEntry sv0;

	sv0 = sv;
	
	if (L == RTCC_MPT_CSM)
	{
		table = &EZEPH1;
	}
	else
	{
		table = &EZEPH2;
	}

	MissionPlanTable *mpt = GetMPTPointer(L);

	if (QUEID == 2)
	{
		//Lock ephemeris table
		table->EPHEM.Header.TUP = -abs(table->EPHEM.Header.TUP);
	}

	InTable.AnchorVector = sv0.Vector;
	InTable.EarthRelStopParam = 400000.0*0.3048; //400k feet EI altitude
	InTable.MoonRelStopParam = 5.0*1852.0;		 //5NM feet EI altitude
	InTable.CutoffIndicator = 3;
	InTable.ManCutoffIndicator = 1;
	InTable.VehicleCode = L;
	InTable.IgnoreManueverNumber = 0;
	InTable.landed = sv0.LandingSiteIndicator;
	InTable.ManeuverIndicator = true;
	InTable.StopParamRefFrame = 2;

	if (QUEID == 2)
	{
		InTable.MaxIntegTime = 10.0*24.0*3600.0;
		InTable.EphemerisBuildIndicator = true;
		InTable.MinEphemDT = 180.0;

		bool HighOrbit;

		if (sv0.LandingSiteIndicator)
		{
			HighOrbit = false;
		}
		else
		{

			double mu;

			if (sv0.Vector.RBI == BODY_EARTH)
			{
				mu = OrbMech::mu_Earth;
			}
			else
			{
				mu = OrbMech::mu_Moon;
			}

			OELEMENTS coe = OrbMech::coe_from_sv(sv0.Vector.R, sv0.Vector.V, mu);
			double SMA = OrbMech::GetSemiMajorAxis(sv0.Vector.R, sv0.Vector.V, mu);

			if (coe.e > SystemParameters.MCGECC)
			{
				HighOrbit = true;
			}
			else
			{
				if (SMA < 0 || SMA > SystemParameters.MCGSMA*OrbMech::R_Earth)
				{
					HighOrbit = true;
				}
				else
				{
					HighOrbit = false;
				}
			}
		}

		double DTMAX;

		if (HighOrbit)
		{
			DTMAX = 10.0*24.0*3600.0;
		}
		else
		{
			DTMAX = 48.0*3600.0;
		}

		InTable.EphemerisLeftLimitGMT = PresentGMT;
		InTable.EphemerisRightLimitGMT = PresentGMT + DTMAX;
		InTable.ECIEphemerisIndicator = true;
		InTable.ECIEphemTableIndicator = &temptable;
	}
	else
	{
		InTable.MaxIntegTime = PresentGMT - sv0.Vector.GMT;
		InTable.EphemerisBuildIndicator = false;
	}

	if (QUEID == 2)
	{
		table->EPHEM.table.clear();
		table->MANTIMES.Table.clear();
	}

	RTCCNIAuxOutputTable aux;
	InTable.AuxTableIndicator = &aux;
	if (QUEID == 2)
	{
		InTable.ManTimesIndicator = &tempmantable;
	}
	else
	{
		InTable.ManTimesIndicator = NULL;
	}

	//EI or kickout altitude for Earth and Moon
	bool IsKickout[2] = { false,false };
	//Has the ephemeris been completely generated?
	bool EphemerisFull = false;

	do
	{
		EMSMISS(&InTable);

		if (QUEID == 2)
		{
			//Store ephemeris
			double dt;
			for (unsigned i = 0;i < temptable.table.size();i++)
			{
				if (table->EPHEM.table.size() > 0)
				{
					dt = table->EPHEM.table.back().GMT - temptable.table[i].GMT;
					if (abs(dt) < 0.001)
					{
						continue;
					}
				}

				table->EPHEM.table.push_back(temptable.table[i]);
			}
			temptable.table.clear();

			//Lunar stay
			if (InTable.NIAuxOutputTable.LunarStayEndGMT > 0)
			{
				table->LUNRSTAY.LunarStayEndGMT = InTable.NIAuxOutputTable.LunarStayEndGMT;
			}
			if (InTable.NIAuxOutputTable.LunarStayBeginGMT > 0)
			{
				table->LUNRSTAY.LunarStayBeginGMT = InTable.NIAuxOutputTable.LunarStayBeginGMT;
			}
		}

		//Set up the next call
		InTable.AnchorVector = InTable.NIAuxOutputTable.sv_cutoff;
		InTable.landed = InTable.NIAuxOutputTable.landed;
		InTable.IgnoreManueverNumber = InTable.NIAuxOutputTable.ManeuverNumber;

		if (InTable.NIAuxOutputTable.TerminationCode == 7)
		{
			//NI ended with maneuver

			//Call DMT processor
			PMMDMT(L, InTable.NIAuxOutputTable.ManeuverNumber, InTable.AuxTableIndicator);

			if (QUEID == 2)
			{
				//Store maneuver
				table->MANTIMES.Table.push_back(tempmantable.Table[0]);
				tempmantable.Table.clear();

				if (EphemerisFull)
				{
					//No more ephemeris to be written

					//Any more maneuvers to simulate?
					if (InTable.NIAuxOutputTable.ManeuverNumber == mpt->ManeuverNum)
					{
						//No
						InTable.NIAuxOutputTable.TerminationCode = 1; //Set to termination on time
					}
				}
				else
				{
					//Write more ephemeris, starting at current time
					InTable.EphemerisLeftLimitGMT = InTable.AnchorVector.GMT;
				}
			}
			else
			{
				double dt = PresentGMT - InTable.AnchorVector.GMT;
				InTable.MaxIntegTime = abs(PresentGMT - InTable.AnchorVector.GMT);
				if (dt >= 0.0)
				{
					InTable.IsForwardIntegration = 1.0;
				}
				else
				{
					InTable.IsForwardIntegration = -1.0;
				}
			}
		}
		else
		{
			if (QUEID == 2)
			{
				if (InTable.NIAuxOutputTable.LunarStayEndGMT > 0)
				{
					table->LUNRSTAY.LunarStayEndGMT = InTable.NIAuxOutputTable.LunarStayEndGMT;
				}
				if (InTable.NIAuxOutputTable.LunarStayBeginGMT > 0)
				{
					table->LUNRSTAY.LunarStayBeginGMT = InTable.NIAuxOutputTable.LunarStayBeginGMT;
				}
			}

			//Terminated on altitude
			if (InTable.NIAuxOutputTable.TerminationCode == 3)
			{
				if (QUEID == 1)
				{
					//Unable to get state vector to present time, exit with an error indication
					L = -L;
					return;
				}

				//NI ended on altitude
				int num;

				num = InTable.NIAuxOutputTable.sv_cutoff.RBI;

				if (L == RTCC_MPT_CSM)
				{
					RTCCONLINEMON.TextBuffer[0] = "CSM";
				}
				else
				{
					RTCCONLINEMON.TextBuffer[0] = "LEM";
				}
				if (IsKickout[num] == true)
				{
					RTCCONLINEMON.TextBuffer[1] = "KO";
				}
				else
				{
					RTCCONLINEMON.TextBuffer[1] = "EI";
				}

				double get = GETfromGMT(InTable.NIAuxOutputTable.sv_cutoff.GMT);
				char Buff[64];
				format_time_rtcc(Buff, get);
				RTCCONLINEMON.TextBuffer[2].assign(Buff);

				double lat, lng, alt;
				GLSSAT(InTable.NIAuxOutputTable.sv_cutoff.R, InTable.NIAuxOutputTable.sv_cutoff.GMT, InTable.NIAuxOutputTable.sv_cutoff.RBI, lat, lng, alt);
				RTCCONLINEMON.DoubleBuffer[0] = lat * DEG;
				RTCCONLINEMON.DoubleBuffer[1] = lng * DEG;

				EMGPRINT("EMSEPH", 11);

				//Continue processing?
				if (IsKickout[num] == false)
				{
					InTable.NIAuxOutputTable.TerminationCode = 7; //To make it continue processing
					if (num == BODY_EARTH)
					{
						InTable.EarthRelStopParam = 50000.0*0.3048;
					}
					else
					{
						InTable.MoonRelStopParam = 0.0;
					}

					IsKickout[num] = true;
				}
			}
			else
			{
				//NI ended on time
				if (QUEID == 2)
				{
					//Enough ephemeris was written
					EphemerisFull = true;

					//More maneuvers to simulate
					bool MoreManeuvers;

					if (mpt->ManeuverNum > 0)
					{
						//Current time before begin time of last maneuver in MPT?
						if (InTable.AnchorVector.GMT < mpt->TimeToBeginManeuver[mpt->ManeuverNum - 1])
						{
							MoreManeuvers = true;
						}
						else
						{
							MoreManeuvers = false;
						}
					}
					else
					{
						MoreManeuvers = false;
					}

					if (MoreManeuvers)
					{
						//Yes
						InTable.EphemerisBuildIndicator = false;
						InTable.NIAuxOutputTable.TerminationCode = 7; //Continue processing
					}
				}
			}
		}
	} while (InTable.NIAuxOutputTable.TerminationCode == 7 && InTable.NIAuxOutputTable.ErrorCode == 0);

	//Unlock
	if (QUEID == 2)
	{
		//Unlock
		mpt->CommonBlock.TUP = abs(mpt->CommonBlock.TUP);
		table->EPHEM.Header.TUP = mpt->CommonBlock.TUP;
		//Write headers
		table->EPHEM.Header.NumVec = table->EPHEM.table.size();
		table->EPHEM.Header.Offset = 0;
		table->EPHEM.Header.TL = table->EPHEM.table.front().GMT;
		table->EPHEM.Header.TR = table->EPHEM.table.back().GMT;
		table->MANTIMES.TUP = table->EPHEM.Header.TUP;

		if (L == RTCC_MPT_CSM)
		{
			RTCCONLINEMON.TextBuffer[0] = "CSM";
		}
		else
		{
			RTCCONLINEMON.TextBuffer[0] = "LEM";
		}
		EMGPRINT("EMSEPH", 12);
		char Buff[64];
		format_time_rtcc(Buff, table->EPHEM.Header.TL);
		RTCCONLINEMON.TextBuffer[1].assign(Buff);
		format_time_rtcc(Buff, table->EPHEM.Header.TR);
		RTCCONLINEMON.TextBuffer[2].assign(Buff);
		EMGPRINT("EMSEPH", 15);

		//Update mission plan table display
		PMDMPT();
	}
	StateVectorTableEntry sv_out;
	sv_out.LandingSiteIndicator = InTable.NIAuxOutputTable.landed;
	sv_out.Vector = InTable.NIAuxOutputTable.sv_cutoff;
	sv_out.VectorCode = sv0.VectorCode;

	//Set output state vector as input state vector
	sv = sv_out;
}

void RTCC::EMSMISS(EMSMISSInputTable *in)
{
	RTCC_EMSMISS integrator(this);
	integrator.Call(in);
}

void RTCC::EMSLSF(EMSLSFInputTable &in)
{
	if (in.EphemerisRightLimitGMT < in.EphemerisLeftLimitGMT) return;

	EphemerisData2 sv_MCT, sv_temp;
	double T;
	bool EphemerisIndicators[4] = {in.ECIEphemerisIndicator, in.ECTEphemerisIndicator, in.MCIEphemerisIndicator, in.MCTEphemerisIndicator};
	EphemerisDataTable2 *pEphemerides[4] = { in.ECIEphemTableIndicator, in.ECTEphemTableIndicator, in.MCIEphemTableIndicator, in.MCTEphemTableIndicator };

	for (int i = 0;i < 4;i++)
	{
		if (EphemerisIndicators[i])
		{
			pEphemerides[i]->table.clear();
		}
	}

	T = in.EphemerisLeftLimitGMT;
	ELGLCV(BZLAND.lat[RTCC_LMPOS_BEST], BZLAND.lng[RTCC_LMPOS_BEST], sv_MCT.R, BZLAND.rad[RTCC_LMPOS_BEST]);
	sv_MCT.V = crossp(_V(0, 0, 1)*OrbMech::w_Moon, sv_MCT.R);

	int end = 0;

	while (end < 2)
	{
		sv_MCT.GMT = T;
		if (in.ECIEphemerisIndicator)
		{
			ELVCNV(sv_MCT, 3, 0, sv_temp);
			in.ECIEphemTableIndicator->table.push_back(sv_temp);
		}
		if (in.ECTEphemerisIndicator)
		{
			ELVCNV(sv_MCT, 3, 1, sv_temp);
			in.ECIEphemTableIndicator->table.push_back(sv_temp);
		}
		if (in.MCIEphemerisIndicator)
		{
			ELVCNV(sv_MCT, 3, 2, sv_temp);
			in.ECIEphemTableIndicator->table.push_back(sv_temp);
		}
		if (in.MCTEphemerisIndicator)
		{
			in.MCTEphemTableIndicator->table.push_back(sv_MCT);
		}

		T = T + in.LunarEphemDT;
		if (T >= in.EphemerisRightLimitGMT)
		{
			T = in.EphemerisRightLimitGMT;
			end++;
		}
	}
	for (int i = 0;i < 4;i++)
	{
		if (EphemerisIndicators[i])
		{
			pEphemerides[i]->Header.CSI = i;
			pEphemerides[i]->Header.NumVec = pEphemerides[i]->table.size();
			pEphemerides[i]->Header.Offset = 0;
			pEphemerides[i]->Header.Status = 0;
			pEphemerides[i]->Header.TL = pEphemerides[i]->table.front().GMT;
			pEphemerides[i]->Header.TR = pEphemerides[i]->table.back().GMT;
			pEphemerides[i]->Header.TUP = 0;
			pEphemerides[i]->Header.VEH = 0;
		}
	}
}

void RTCC::EMGVECSTInput(int L, StateVectorTableEntry sv)
{
	HistoryAnchorVectorTable *tab;
	if (L == RTCC_MPT_CSM)
	{
		tab = &EZANCHR1;
	}
	else
	{
		tab = &EZANCHR3;
	}

	//Move all SVs up by one place
	for (int i = 0;i < 9;i++)
	{
		tab->AnchorVectors[i] = tab->AnchorVectors[i + 1];
	}
	//Adjust number of anchor vectors
	if (tab->num < 10)
	{
		tab->num++;
	}
	//Make the last SV in table the current
	tab->AnchorVectors[9] = sv;
}

int RTCC::EMGVECSTOutput(int L, EphemerisData &sv)
{
	HistoryAnchorVectorTable *tab;
	if (L == RTCC_MPT_CSM)
	{
		tab = &EZANCHR1;
	}
	else
	{
		tab = &EZANCHR3;
	}

	if (tab->num < 1)
	{
		return 1;
	}

	sv = tab->AnchorVectors[9].Vector;

	return 0;
}

int RTCC::EMSFFV(double gmt, int L, EphemerisData &sv)
{
	MissionPlanTable *mpt = GetMPTPointer(L);

	//Is there any maneuver on the MPT?
	if (mpt->ManeuverNum > 0)
	{
		//Yes
		unsigned num = 1;
		bool stop = false;
		OrbitEphemerisTable *eph;

		if (L == RTCC_MPT_CSM)
		{
			eph = &EZEPH1;
		}
		else
		{
			eph = &EZEPH2;
		}

		//Find the maneuver before the input time
		do
		{
			if (gmt < mpt->TimeToBeginManeuver[num - 1])
			{
				//gmt is before this maneuver
				num--;
				stop = true;
			}
			else
			{
				if (mpt->ManeuverNum > num)
				{
					//Increment maneuver counter
					num++;
				}
				else
				{
					//gmt is after last maneuver
					stop = true;
				}
			}
		} while (stop == false);

		//Is gmt before first maneuver?
		if (num == 0)
		{
			//Normal fetch logic
			return ELFECH(gmt, L, sv);
		}
		else
		{
			bool UseBurnoutVec;

			//Within 3 minutes of maneuver time?
			if (gmt < mpt->TimeToEndManeuver[num - 1] + 3.0*60.0)
			{
				UseBurnoutVec = true;
			}
			//Previous maneuver is beyond ephemeris limit?
			else if (mpt->TimeToBeginManeuver[num - 1] > eph->EPHEM.Header.TR)
			{
				UseBurnoutVec = true;
			}
			else
			{
				UseBurnoutVec = false;
			}

			if (UseBurnoutVec)
			{
				//Use burnout vector
				sv.R = mpt->mantable[num - 1].R_BO;
				sv.V = mpt->mantable[num - 1].V_BO;
				sv.GMT = mpt->mantable[num - 1].GMT_BO;
				sv.RBI = mpt->mantable[num - 1].RefBodyInd;
				return 0;
			}
			else
			{
				//Normal fetch logic
				return ELFECH(gmt, L, sv);
			}
		}
	}
	else
	{
		//No, just try to fetch a state vector from the ephemeris
		return ELFECH(gmt, L, sv);
	}
}

void RTCC::EMMENI(EMMENIInputTable &in)
{
	EnckeFreeFlightIntegrator integ(this);
	integ.Propagate(in);
}

int RTCC::EMMXTR(double GMT, double rmag, double vmag, double rtasc, double decl, double fpav, double az, VECTOR3 &R, VECTOR3 &V)
{
	EphemerisData2 sv, sv_out;

	OrbMech::adbar_from_rv(rmag, vmag, rtasc + OrbMech::w_Earth*GMT, decl, fpav, az, sv.R, sv.V);
	sv.GMT = GMT;

	if (ELVCNV(sv, 1, 0, sv_out))
	{
		return 1;
	}

	R = sv_out.R;
	V = sv_out.V;

	return 0;
}

bool RTCC::MPTHasManeuvers(int L)
{
	if (L == RTCC_MPT_LM)
	{
		if (PZMPTLEM.mantable.size() > 0)
		{
			return true;
		}
	}
	else
	{
		if (PZMPTCSM.mantable.size() > 0)
		{
			return true;
		}
	}

	return false;
}

int RTCC::PMMWTC(int med)
{
	MPTVehicleDataBlock CommonBlock;
	MissionPlanTable *table;
	PLAWDTInput pin;
	PLAWDTOutput pout;
	double W, WDOT[6], TL[6], DVREM, FuelR[6];
	unsigned IBLK;
	int Thruster, TabInd;

	WDOT[0] = SystemParameters.MCTCW1;
	WDOT[1] = SystemParameters.MCTSW1;
	WDOT[2] = SystemParameters.MCTWAV;
	WDOT[3] = SystemParameters.MCTLW1;
	WDOT[4] = SystemParameters.MCTAW1;
	WDOT[5] = SystemParameters.MCTDW1;
	TL[0] = SystemParameters.MCTCT1;
	TL[1] = SystemParameters.MCTST1;
	TL[2] = SystemParameters.MCTSAV;
	TL[3] = SystemParameters.MCTLT1;
	TL[4] = SystemParameters.MCTAT1;
	TL[5] = SystemParameters.MCTDT1;

	//M49: Thruster Fuel Remaining
	if (med == 49)
	{
		double RCSFuelUsed, MainEngineFuelUsed, TotalMassAfter;

		TabInd = med_m49.Table;
		table = GetMPTPointer(TabInd);
		IBLK = table->LastExecutedManeuver + 1;
		double FuelC[6];

		FuelC[0] = med_m49.CSMRCSFuelRemaining;
		FuelC[1] = med_m49.SPSFuelRemaining;
		FuelC[2] = med_m49.SIVBFuelRemaining;
		FuelC[3] = med_m49.LMRCSFuelRemaining;
		FuelC[4] = med_m49.LMAPSFuelRemaining;
		FuelC[5] = med_m49.LMDPSFuelRemaining;
		int N = 0;
		int i;
	RTCC_PMMWTC_2:
		//Read IBLK of MPT
		if (IBLK == 1)
		{
			CommonBlock = table->CommonBlock;
		}
		else
		{
			CommonBlock = table->mantable[IBLK - 2].CommonBlock;
			Thruster = table->mantable[IBLK - 2].Thruster;
			RCSFuelUsed = table->mantable[IBLK - 2].RCSFuelUsed;
			MainEngineFuelUsed = table->mantable[IBLK - 2].MainEngineFuelUsed;
			TotalMassAfter = table->mantable[IBLK - 2].TotalMassAfter;
			DVREM = table->mantable[IBLK - 2].DVREM;
		}
		FuelR[0] = CommonBlock.CSMRCSFuelRemaining;
		FuelR[1] = CommonBlock.SPSFuelRemaining;
		FuelR[2] = CommonBlock.SIVBFuelRemaining;
		FuelR[3] = CommonBlock.LMRCSFuelRemaining;
		FuelR[4] = CommonBlock.LMAPSFuelRemaining;
		FuelR[5] = CommonBlock.LMDPSFuelRemaining;
		if (N == 0)
		{
			goto RTCC_PMMWTC_3;
		}
		if (CommonBlock.ConfigChangeInd == 0)
		{
			goto RTCC_PMMWTC_5;
		}
	RTCC_PMMWTC_3:
		if (CommonBlock.ConfigCode[RTCC_CONFIG_C] == false)
		{
			FuelC[0] = -1;
			FuelC[1] = -1;
		}
		if (CommonBlock.ConfigCode[RTCC_CONFIG_S] == false)
		{
			FuelC[2] = -1;
		}
		if (CommonBlock.ConfigCode[RTCC_CONFIG_A] == false)
		{
			FuelC[3] = -1;
			FuelC[4] = -1;
		}
		if (CommonBlock.ConfigCode[RTCC_CONFIG_D] == false)
		{
			FuelC[5] = -1;
		}
	RTCC_PMMWTC_5:
		i = 0;
	RTCC_PMMWTC_6:
		if (FuelC[i] < 0)
		{
			goto RTCC_PMMWTC_11;
		}
		FuelR[i] = FuelC[i];
		if (IBLK <= 1)
		{
			goto RTCC_PMMWTC_11;
		}
		if (N > 0)
		{
			goto RTCC_PMMWTC_7;
		}
		if (MPTIsPrimaryThruster(Thruster, i) == false)
		{
			goto RTCC_PMMWTC_11;
		}
		goto RTCC_PMMWTC_8;
	RTCC_PMMWTC_7:
		if (MPTIsPrimaryThruster(Thruster, i) == false)
		{
			goto RTCC_PMMWTC_9;
		}
		if (MPTIsRCSThruster(Thruster))
		{
			FuelR[i] = FuelR[i] - RCSFuelUsed;
		}
		else
		{
			FuelR[i] = FuelR[i] - MainEngineFuelUsed;
		}
	RTCC_PMMWTC_8:
		if (TotalMassAfter - FuelR[i] <= 0)
		{
			DVREM = 0.0;
		}
		else
		{
			DVREM = TL[i] / WDOT[i] * log(TotalMassAfter / (TotalMassAfter - FuelR[i]));
		}
		goto RTCC_PMMWTC_10;
	RTCC_PMMWTC_9:
		if (MPTIsUllageThruster(Thruster, i))
		{
			FuelR[i] = FuelR[i] - RCSFuelUsed;
		}
	RTCC_PMMWTC_10:
		FuelC[i] = FuelR[i];
	RTCC_PMMWTC_11:
		if (i < 5)
		{
			i++;
			goto RTCC_PMMWTC_6;
		}
		CommonBlock.CSMRCSFuelRemaining = FuelR[0];
		CommonBlock.SPSFuelRemaining = FuelR[1];
		CommonBlock.SIVBFuelRemaining = FuelR[2];
		CommonBlock.LMRCSFuelRemaining = FuelR[3];
		CommonBlock.LMAPSFuelRemaining = FuelR[4];
		CommonBlock.LMDPSFuelRemaining = FuelR[5];
		if (IBLK == 1)
		{
			table->CommonBlock = CommonBlock;
		}
		else
		{
			table->mantable[IBLK - 2].CommonBlock = CommonBlock;
			table->mantable[IBLK - 2].DVREM = DVREM;
		}
		if (IBLK - 1 < table->ManeuverNum)
		{
			N = 1;
			IBLK++;
			goto RTCC_PMMWTC_2;
		}
		//Update MPT and DMT displays
		EMSNAP(0, 7);
	}
	//M50: Vehicle Weight
	if (med == 50)
	{
		double WeightGMT, W, WTV[4];

		TabInd = med_m50.Table;
		table = GetMPTPointer(TabInd);
		IBLK = table->LastExecutedManeuver + 1;
		WeightGMT = GMTfromGET(med_m50.WeightGET);

		if (IBLK == 1)
		{
			CommonBlock = table->CommonBlock;
		}
		else
		{
			if (WeightGMT < table->mantable[IBLK - 2].GMT_BO)
			{
				PMXSPT("PMMWTC", 23);
				return 23;
			}
			CommonBlock = table->mantable[IBLK - 2].CommonBlock;
		}
		WTV[0] = CommonBlock.CSMMass;
		WTV[1] = CommonBlock.SIVBMass;
		WTV[2] = CommonBlock.LMAscentMass;
		WTV[3] = CommonBlock.LMDescentMass;

		W = 0.0;

		if (CommonBlock.ConfigCode[RTCC_CONFIG_C])
		{
			if (med_m50.CSMWT >= 0)
			{
				WTV[0] = med_m50.CSMWT;
			}
			W = W + WTV[0];
		}

		if (CommonBlock.ConfigCode[RTCC_CONFIG_S])
		{
			if (med_m50.SIVBWT >= 0)
			{
				WTV[1] = med_m50.SIVBWT;
			}
			W = W + WTV[1];
		}

		//Do we have an ascent stage?
		if (CommonBlock.ConfigCode[RTCC_CONFIG_A])
		{
			goto RTCC_PMMWTC_15;
		}
		if (CommonBlock.ConfigCode[RTCC_CONFIG_D])
		{
			if (med_m50.LMWT >= 0)
			{
				WTV[3] = med_m50.LMWT;
			}
			W = W + WTV[3];
		}
		goto RTCC_PMMWTC_17;
	RTCC_PMMWTC_15:
		//Ascent stage weight input?
		if (med_m50.LMASCWT >= 0)
		{
			//Set initial ascent stage weight
			WTV[2] = med_m50.LMASCWT;
		}
		//Total stage weight input?
		if (med_m50.LMWT < 0)
		{
			//Do we have also have a descent stage?
			if (CommonBlock.ConfigCode[RTCC_CONFIG_D])
			{
				goto RTCC_PMMWTC_15A;
			}
			else
			{
				goto RTCC_PMMWTC_16;
			}
		}
		//Do we have also have a descent stage?
		if (CommonBlock.ConfigCode[RTCC_CONFIG_D])
		{
			//Yes, calculate descent stage weight from total weight, minus ascent stage weight
			WTV[3] = med_m50.LMWT - WTV[2];
		RTCC_PMMWTC_15A:
			W = W + WTV[3];
		}
		else
		{
			//No, total weight equals ascent stage weight (can override input from LMASCWT earlier)
			WTV[3] = med_m50.LMWT;
		}
	RTCC_PMMWTC_16:
		W = W + WTV[2];
	RTCC_PMMWTC_17:
		CommonBlock.CSMMass = WTV[0];
		CommonBlock.SIVBMass = WTV[1];
		CommonBlock.LMAscentMass = WTV[2];
		CommonBlock.LMDescentMass = WTV[3];
		if (IBLK > 1)
		{
		RTCC_PMMWTC_17A:
			goto RTCC_PMMWTC_17B;
		}
		double t_aw = table->SIVBVentingBeginGET;
		if (t_aw >= med_m50.WeightGET)
		{
			goto RTCC_PMMWTC_19;
		}
		else
		{
			goto RTCC_PMMWTC_17A;
		}
	RTCC_PMMWTC_17B:
		//Compute expendables and venting for weight change
		pin.CSMArea = 0.0;
		pin.CSMWeight = 0.0;
		pin.KFactorOpt = false;
		pin.LMAscArea = 0.0;
		pin.LMAscWeight = 0.0;
		pin.LMDscArea = 0.0;
		pin.LMDscWeight = 0.0;
		pin.Num = CommonBlock.ConfigCode.to_ulong();
		pin.SIVBArea = 0.0;
		pin.SIVBWeight = 0.0;
		pin.TableCode = -TabInd;
		//Determine start of venting
		if (IBLK == 1)
		{
			//No maneuver executed, use actual start of venting
			pin.T_IN = table->SIVBVentingBeginGET;
			if (pin.T_IN < SystemParameters.MCGVEN)
			{
				pin.T_IN = SystemParameters.MCGVEN;
			}
			pin.T_IN += GetGMTLO()*3600.0;
		}
		else
		{
			//Calculate weight loss since last maneuver
			pin.T_IN = table->mantable[IBLK - 2].GMT_BO;
		}
		pin.T_UP = WeightGMT;
		pin.VentingOpt = true;
		PLAWDT(pin, pout);

		//Adjust total and vehicle weights for expendables
		W -= pout.ConfigWeight;
		CommonBlock.CSMMass -= pout.CSMWeight;
		CommonBlock.LMAscentMass -= pout.LMAscWeight;
		CommonBlock.LMDescentMass -= pout.LMDscWeight;
		CommonBlock.SIVBMass -= pout.SIVBWeight;

		if (IBLK <= 1)
		{
			goto RTCC_PMMWTC_19;
		}
		int I = MPTGetPrimaryThruster(table->mantable[IBLK - 2].Thruster);
		FuelR[0] = CommonBlock.CSMRCSFuelRemaining;
		FuelR[1] = CommonBlock.SPSFuelRemaining;
		FuelR[2] = CommonBlock.SIVBFuelRemaining;
		FuelR[3] = CommonBlock.LMRCSFuelRemaining;
		FuelR[4] = CommonBlock.LMAPSFuelRemaining;
		FuelR[5] = CommonBlock.LMDPSFuelRemaining;
		DVREM = TL[I] / WDOT[I] * log(table->mantable[IBLK - 2].TotalMassAfter/(table->mantable[IBLK - 2].TotalMassAfter- FuelR[I]));
		//DTWRITE: IBLK of MPT
		table->mantable[IBLK - 2].CommonBlock = CommonBlock;
		table->mantable[IBLK - 2].DVREM = DVREM;
		//DTREAD: MPT Header
		CommonBlock = table->CommonBlock;
	RTCC_PMMWTC_19:
		if (IBLK == 1)
		{
			table->TotalInitMass = W;
		}
		else
		{
			table->WeightAfterManeuver[IBLK - 2] = W;
		}
	RTCC_PMMWTC_20:
		CommonBlock.TUP = -CommonBlock.TUP;
		//DTWRITE: MPT Header
		table->CommonBlock = CommonBlock;

		//Set up input for PMSVCT
		PMSVCT(8, TabInd);
		return 0;
	}
	//Change Vehicle Area or K-Factor
	if (med == 51)
	{
		double Area, ARI[4], ARV[4];
		int I;

		TabInd = med_m51.Table;
		table = GetMPTPointer(TabInd);
		ARI[0] = med_m51.CSMArea;
		ARI[1] = med_m51.SIVBArea;
		ARI[2] = med_m51.LMAscentArea;
		ARI[3] = med_m51.LMDescentArea;
		IBLK = table->LastExecutedManeuver + 1;
		if (IBLK == 1)
		{
			CommonBlock = table->CommonBlock;
		}
		else
		{
			CommonBlock = table->mantable[IBLK - 2].CommonBlock;
		}
		ARV[0] = CommonBlock.CSMArea;
		ARV[1] = CommonBlock.SIVBArea;
		ARV[2] = CommonBlock.LMAscentArea;
		ARV[3] = CommonBlock.LMDescentArea;
		Area = 0;
		I = 1;
	RTCC_PMMWTC_22:
		if (CommonBlock.ConfigCode[I - 1])
		{
			if (ARI[I - 1] >= 0)
			{
				ARV[I - 1] = ARI[I - 1];
			}
			if (Area < ARV[I - 1])
			{
				Area = ARV[I - 1];
			}
		}
		if (I < 4)
		{
			I++;
			goto RTCC_PMMWTC_22;
		}
		CommonBlock.CSMArea = ARV[0];
		CommonBlock.SIVBArea = ARV[1];
		CommonBlock.LMAscentArea = ARV[2];
		CommonBlock.LMDescentArea = ARV[3];
		if (IBLK > 1)
		{
			table->mantable[IBLK - 2].CommonBlock = CommonBlock;
			CommonBlock = table->CommonBlock;
		}
		if (med_m51.KFactor >= -30)
		{
			table->KFactor = med_m51.KFactor;
		}
		if (IBLK > 1)
		{
			table->AreaAfterManeuver[IBLK - 2] = Area;
		}
		else
		{
			table->ConfigurationArea = Area;
		}
		goto RTCC_PMMWTC_20;
	}
	//Input initialization parameters
	if (med == 55)
	{
		TabInd = med_m55.Table;
		table = GetMPTPointer(TabInd);

		if (table->mantable.size() > 0)
		{
			PMXSPT("PMMWTC", 71);
			return 2;
		}

		if (med_m55.VentingGET >= 0.0)
		{
			table->SIVBVentingBeginGET = med_m55.VentingGET;
		}

		if (med_m55.DeltaDockingAngle >= -360.0)
		{
			table->DeltaDockingAngle = med_m55.DeltaDockingAngle;
		}

		std::bitset<4> cfg;
		MPTGetConfigFromString(med_m55.ConfigCode, cfg);

		if (cfg.to_ulong() > 0)
		{
			double arv[4], wtv[4], AREA;
			arv[0] = table->CommonBlock.CSMArea;
			arv[1] = table->CommonBlock.SIVBArea;
			arv[2] = table->CommonBlock.LMAscentArea;
			arv[3] = table->CommonBlock.LMDescentArea;
			wtv[0] = table->CommonBlock.CSMMass;
			wtv[1] = table->CommonBlock.SIVBMass;
			wtv[2] = table->CommonBlock.LMAscentMass;
			wtv[3] = table->CommonBlock.LMDescentMass;

			double arvs[4], wtvs[4];
			
			for (int i = 0;i < 4;i++)
			{
				arvs[i] = wtvs[i] = 0.0;
				if (table->CommonBlock.ConfigCode[i] == true)
				{
					wtvs[i] = wtv[i];
					arvs[i] = arv[i];
				}
			}

			table->CommonBlock.ConfigCode = cfg;
			AREA = 0.0;
			W = 0.0;
			for (int i = 0;i < 4;i++)
			{
				if (table->CommonBlock.ConfigCode[i] == true)
				{
					if (AREA < arvs[i])
					{
						AREA = arvs[i];
					}
					W = W + wtvs[i];
				}
				else
				{
					arvs[i] = 0.0;
					wtvs[i] = 0.0;
				}
			}
			table->CommonBlock.CSMArea = arvs[0];
			table->CommonBlock.CSMMass = wtvs[0];
			table->CommonBlock.SIVBArea = arvs[1];
			table->CommonBlock.SIVBMass = wtvs[1];
			table->CommonBlock.LMAscentArea = arvs[2];
			table->CommonBlock.LMAscentMass = wtvs[2];
			table->CommonBlock.LMDescentArea = arvs[3];
			table->CommonBlock.LMDescentMass = wtvs[3];
			table->TotalInitMass = W;
			table->ConfigurationArea = AREA;

			table->CommonBlock.TUP = -table->CommonBlock.TUP;

			//Set up input for PMSVCT
			PMSVCT(8, med_m55.Table);
		}
	}

	return 0;
}

void RTCC::PMSVCT(int QUEID, int L)
{
	//Convenient helper function for PMSVCT entries that don't require an input state vector. For QUEID = 7 and higher

	StateVectorTableEntry sv0;

	PMSVCT(QUEID, L, sv0);
}

void RTCC::PMSVCT(int QUEID, int L, StateVectorTableEntry sv0)
{
	//QUEID:
	//0 = Nominal Insertion
	//1 = Actual Insertion
	//2 = Restart
	//3 = Launch Override
	//4 = DC
	//5 = H.S. Radar
	//6 = Telemetry
	//7 = Maneuver Confirmation
	//8 = Maneuver Control (initiate trajectory update as a result of maneuver control action)
	//9 = Maneuver Control Special (initiate trajectory update as a result of maneuver control action that changes ephemeris prior to TLI)
	//17 = Ephemeris Extension

	OrbitEphemerisTable *maineph;
	MissionPlanTable *mpt;
	StateVectorTableEntry sv1;
	unsigned I, K;

	if (L == RTCC_MPT_CSM)
	{
		maineph = &EZEPH1;
		mpt = &PZMPTCSM;
	}
	else
	{
		maineph = &EZEPH2;
		mpt = &PZMPTLEM;
	}

	switch (QUEID)
	{
	case 0:
	case 1:
	case 3:
		goto RTCC_PMSVCT_3;
	case 2:
	case 4:
	case 5:
	case 6:
	case 7:
		goto RTCC_PMSVCT_4;
	case 8:
	case 9:
		goto RTCC_PMSVCT_15;
	case 17:
		goto RTCC_PMSVCT_14;
	}

	PMXSPT("PMSVCT", 48);
	return;

RTCC_PMSVCT_3:
	//TBD: Time of insertion = time of anchor vector
RTCC_PMSVCT_4:
	//New anchor vector
	if (sv0.LandingSiteIndicator)
	{
		mpt->GMTAV = RTCCPresentTimeGMT();
	}
	else
	{
		mpt->GMTAV = sv0.Vector.GMT;
	}

	//Station ID to MPT
	mpt->StationID.assign(sv0.VectorCode);
	K = 1;
	I = mpt->ManeuverNum;
RTCC_PMSVCT_5:
	if (K > I) goto RTCC_PMSVCT_8;
	if (mpt->GMTAV <= mpt->TimeToBeginManeuver[K - 1]) goto RTCC_PMSVCT_8;
	//Page 6
	if (mpt->GMTAV > mpt->TimeToEndManeuver[K - 1])
	{
		K++;
		goto RTCC_PMSVCT_5;
	}
	//Anchor vector is within maneuver
	mpt->TimeToEndManeuver[K - 1] = mpt->GMTAV;
	mpt->CommonBlock.TUP = -abs(mpt->CommonBlock.TUP);
	//DTWRITE MPT Header
	//DTREAD MPT mnvr descriptive block
	//Set maneuver burnout vector = anchor vector
	mpt->mantable[K - 1].GMT_BO = sv0.Vector.GMT;
	mpt->mantable[K - 1].R_BO = sv0.Vector.R;
	mpt->mantable[K - 1].V_BO = sv0.Vector.V;
	mpt->mantable[K - 1].CommonBlock.TUP = -abs(mpt->mantable[K - 1].CommonBlock.TUP);
	//DTWRITE descriptive maneuver block
	//Page 7
	//DTREAD MPT Header
RTCC_PMSVCT_8:
	bool tli = false;
	for (unsigned i = 0;i < mpt->mantable.size();i++)
	{
		if (mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_SIVB_IGM || mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_PGNS_DESCENT)
		{
			if (mpt->mantable[i].GMTMAN > sv0.Vector.GMT)
			{
				if (mpt->mantable[i].FrozenManeuverInd == false)
				{
					//EMSMISS and PMMSPT for TLI/PDI
					EMSMISSInputTable intab;
					intab.AnchorVector = sv0.Vector;
					intab.AuxTableIndicator = NULL;
					intab.CutoffIndicator = 1;
					intab.EphemerisBuildIndicator = false;
					intab.IgnoreManueverNumber = 0;
					intab.ManCutoffIndicator = 2;
					if (mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
					{
						intab.ManeuverIndicator = false;
					}
					else
					{
						intab.ManeuverIndicator = true;
					}
					intab.MaxIntegTime = mpt->mantable[i].GMTMAN - sv0.Vector.GMT - 1.0; //One second earlier so that EMSMISS isn't trying to simulate the maneuver
					intab.VehicleCode = L;
					EMSMISS(&intab);

					if (mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
					{
						if (intab.NIAuxOutputTable.ErrorCode)
						{
							RTCCONLINEMON.TextBuffer[0] = "TLI INITIATION";
							PMXSPT("PMSVCT", 35);
							return;
						}
						EphemerisData sv1 = intab.NIAuxOutputTable.sv_cutoff;

						PMMSPTInput intab2;
						intab2.QUEID = 32;
						intab2.GMT = sv1.GMT;
						intab2.R = sv1.R;
						intab2.V = sv1.V;
						intab2.CurMan = &mpt->mantable[i];
						intab2.Table = L;
						intab2.mpt = mpt;

						int err;
						if (err = PMMSPT(intab2))
						{
							PMXSPT("PMMSPT", err);
							EMSNAP(L, 2);
							return;
						}
					}
					else
					{
						if (intab.NIAuxOutputTable.ErrorCode)
						{
							RTCCONLINEMON.TextBuffer[0] = "POWERED DESCENT";
							PMXSPT("PMSVCT", 35);
							return;
						}
						EphemerisData sv1 = intab.NIAuxOutputTable.sv_cutoff;

						PMMLDPInput intab2;

						intab2.CurrentManeuver = i;
						intab2.HeadsUpDownInd = mpt->mantable[i].HeadsUpDownInd;
						intab2.mpt = mpt;
						intab2.sv = ConvertEphemDatatoSV(sv1);
						intab2.sv.mass = intab.NIAuxOutputTable.CutoffWeight;
						intab2.TLAND = GETfromGMT(mpt->mantable[i].GMT_BO);
						intab2.TrimAngleInd = mpt->mantable[i].TrimAngleInd;

						PMMLDP(intab2, mpt->mantable[i]);
					}
				}
			}
		}
	}
	if (QUEID == 0)
	{
		sv1 = sv0;
		goto RTCC_PMSVCT_12;
	}

	//Take state vector to current time
	sv1 = sv0;
	EMSEPH(1, sv1, L, RTCCPresentTimeGMT());
	if (L < 0)
	{
		RTCCONLINEMON.TextBuffer[0] = "PRESENT TIME";
		PMXSPT("PMSVCT", 35);
		return;
	}
RTCC_PMSVCT_12:
	mpt->CommonBlock.TUP++;
	mpt->CommonBlock.TUP = -mpt->CommonBlock.TUP;
	EMSTRAJ(sv1, L);
	return;
RTCC_PMSVCT_14:
	//TBD
	EMSNAP(L, 2);
	return;
RTCC_PMSVCT_15:
	//Trajectory update
	double T_P;
	EphemerisData sv;

	T_P = RTCCPresentTimeGMT();

	if (maineph->LUNRSTAY.LunarStayBeginGMT < T_P && T_P < maineph->LUNRSTAY.LunarStayEndGMT)
	{
		sv1.LandingSiteIndicator = true;
	}
	else
	{
		sv1.LandingSiteIndicator = false;
	}

	if (sv1.LandingSiteIndicator)
	{

	}
	else
	{
		double T_F, T_Left;

		if (maineph->EPHEM.table.size() > 0)
		{
			T_Left = maineph->EPHEM.table.front().GMT;
		}
		else
		{
			T_Left = 0.0;
		}


		if (T_P < T_Left)
		{
			T_F = T_Left;
		}
		else
		{
			T_F = T_P;
		}

		if (PMSVCTAuxVectorFetch(L, T_F, sv))
		{
			if (mpt->CommonBlock.TUP < 0)
			{
				mpt->CommonBlock.TUP = -mpt->CommonBlock.TUP;
			}

			if (L == RTCC_MPT_CSM)
			{
				RTCCONLINEMON.TextBuffer[0] = "CSM";
			}
			else
			{
				RTCCONLINEMON.TextBuffer[0] = "LEM";
			}
			PMXSPT("PMSVCT", 33);
			EMSNAP(L, 2);
			return;
		}
	}

	//TUP should be negative already
	if (mpt->CommonBlock.TUP > 0)
	{
		mpt->CommonBlock.TUP = -mpt->CommonBlock.TUP;
	}
	mpt->CommonBlock.TUP--;

	sv1.Vector = sv;
	sv1.VectorCode = mpt->StationID;
	EMSTRAJ(sv1, L);
}

int RTCC::PMSVEC(int L, double GMT, CELEMENTS &elem, double &KFactor, double &Area, double &Weight, std::string &StaID, int &RBI)
{
	double mu;
	MissionPlanTable *mpt = GetMPTPointer(L);
	EphemerisData sv;

	if (mpt->mantable.size() == 0)
	{
		Area = mpt->ConfigurationArea;
		Weight = mpt->TotalInitMass;
	}
	else
	{
		unsigned mancounter = 0;

		do
		{
			if (GMT < mpt->mantable[mancounter].GMTMAN)
			{
				break;
			}
			if (mancounter >= mpt->mantable.size() - 1)
			{
				break;
			}
		} while (mancounter < mpt->mantable.size());

		if (mancounter == 0U)
		{
			Area = mpt->ConfigurationArea;
			Weight = mpt->TotalInitMass;
		}
		else
		{
			Area = mpt->mantable[mancounter].TotalAreaAfter;
			Weight = mpt->mantable[mancounter].TotalMassAfter;
		}

		if (mancounter > 0U && GMT < mpt->mantable[mancounter - 1].GMT_BO + 3.0*60.0)
		{
			sv.R = mpt->mantable[mancounter - 1].R_BO;
			sv.V = mpt->mantable[mancounter - 1].V_BO;
			sv.GMT = mpt->mantable[mancounter - 1].GMT_BO;
			sv.RBI = mpt->mantable[mancounter - 1].RefBodyInd;

			goto PMSVEC_2_2;
		}
	}

	if (EMSFFV(GMT, L, sv))
	{
		PMXSPT("PMSVEC", 49);
		return 1;
	}
PMSVEC_2_2:

	if (sv.RBI == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}

	elem = OrbMech::GIMIKC(sv.R, sv.V, mu);
	KFactor = mpt->KFactor;
	StaID = mpt->StationID;

	return 0;
}

int RTCC::PMSVCTAuxVectorFetch(int L, double T_F, EphemerisData &sv)
{
	unsigned nbr_left;

	MissionPlanTable * mpt;
	EphemerisDataTable2 *maineph;
	if (L == RTCC_MPT_CSM)
	{
		maineph = &EZEPH1.EPHEM;
		mpt = &PZMPTCSM;
	}
	else
	{
		maineph = &EZEPH2.EPHEM;
		mpt = &PZMPTLEM;
	}

	if (mpt->mantable.size() == 0)
	{
		nbr_left = 1;
	}
	else
	{
		unsigned K = 0;
		do
		{
			if (T_F == mpt->TimeToEndManeuver[K])
			{
				nbr_left = 0;
				break;
			}
			else if (T_F > mpt->TimeToEndManeuver[K])
			{
				K++;
				if (K > mpt->mantable.size())
				{
					nbr_left = 1;
					break;
				}
			}
			else
			{
				if (T_F < mpt->TimeToBeginManeuver[K])
				{
					nbr_left = 1;
					break;
				}
				else
				{
					T_F = mpt->TimeToBeginManeuver[K];
					break;
				}
			}
		} while (K < mpt->mantable.size());
	}

	double T_right;
	if (maineph->table.size() > 0)
	{
		T_right = maineph->table.back().GMT;
	}
	else
	{
		T_right = 0.0;
	}

	if (T_F > T_right)
	{
		return 1;
	}

	EphemerisDataTable2 EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;
	int err = ELFECH(T_F, 1, nbr_left, L, EPHEM, MANTIMES, LUNSTAY);
	if (err)
	{
		return err;
	}
	sv = RotateSVToSOI(EPHEM.table[0]);

	return 0;
}

void RTCC::PMSEXE(int L, double gmt)
{
	double TMNVR, TMANVR;
	bool print = false;
	TMNVR = gmt;
	MissionPlanTable *tab = GetMPTPointer(L);
	unsigned i, j, k;
	i = tab->ManeuverNum;
	if (i == 0)
	{
		tab->LastExecutedManeuver = 0;
		tab->LastFrozenManeuver = 0;
		goto RTCC_PMSEXE_2_3;
	}
	j = 1;
RTCC_PMSEXE_A:
	if (TMNVR < tab->TimeToBeginManeuver[j - 1])
	{
		goto RTCC_PMSEXE_B;
	}
	j++;
	if (j <= i)
	{
		goto RTCC_PMSEXE_A;
	}
RTCC_PMSEXE_B:
	k = j - 1;
	if (k > tab->LastExecutedManeuver)
	{
		tab->LastExecutedManeuver = k;
		print = true;
	}
	if (k + 1 > i)
	{
	RTCC_PMSEXE_2_3:
		TMANVR = 1e70;
	}
	else
	{
		TMANVR = tab->TimeToBeginManeuver[k];
	}
	tab->UpcomingManeuverGMT = TMANVR;
	if (print == false)
	{
		return;
	}
	RTCCONLINEMON.TextBuffer[0] = tab->mantable[k - 1].code;
	RTCCONLINEMON.DoubleBuffer[0] = tab->mantable[k - 1].GMTMAN;
	RTCCONLINEMON.DoubleBuffer[1] = tab->mantable[k - 1].GMT_BO;
	RTCCONLINEMON.DoubleBuffer[2] = tab->mantable[k - 1].TotalAreaAfter;
	RTCCONLINEMON.DoubleBuffer[3] = tab->mantable[k - 1].TotalMassAfter*LBS*1000.0;
	RTCCONLINEMON.VectorBuffer[0] = tab->mantable[k - 1].R_BO / OrbMech::R_Earth;
	RTCCONLINEMON.VectorBuffer[1] = tab->mantable[k - 1].V_BO*3600.0 / OrbMech::R_Earth;
	PMXSPT("PMSEXE", 36);
	EMSNAP(L, 7);
}

bool RTCC::MEDTimeInputHHMMSS(std::string vec, double &hours)
{
	int hh, mm;
	double ss;
	bool pos = true;
	if (sscanf(vec.c_str(), "%d:%d:%lf", &hh, &mm, &ss) != 3)
	{
		return true;
	}
	if (vec[0] == '-')
	{
		pos = false;
		hh = abs(hh);
	}
	hours = (double)hh + (double)mm / 60.0 + ss / 3600.0;
	if (pos == false)
	{
		hours = -hours;
	}
	return false;
}

void RTCC::PMMPAD(AEGBlock sv, double mass, double THT, double dt, double H_P, int Thruster, double DPSScaleFactor)
{
	AEGDataBlock sv_temp, sv_a, sv_apo, sv_peri;
	double INFO[10], T_next, R_PD, beta, rho0, R_E, A, B, C, D, rho, R, r, rr, RR, DV1, DV2, DV, mu, V_b, X, r_dot_b, V_H_b, r_dot_a, V_H_a, V_a2, eacosEa, easinEa;
	double E_a, R_PC, R_PKEP, eps1, DR_PER_0, DR_PER_1, DR_PER_C, DDR_PER, dt_man, f_dot, DP, F, W_E;
	int KAOP, KE, man = 0, iter, iter_max, err;
	//0 = don't end, 1 = success, -1 = fail
	int endloop;

	PZPADDIS = PerigeeAdjustTable();

	//Initialize, if it isn't already
	PMMAEGS(sv.Header, sv.Data, sv_temp);

	T_next = THT;
	if (sv.Header.AEGInd == BODY_EARTH)
	{
		R_E = OrbMech::R_Earth;
		mu = OrbMech::mu_Earth;
	}
	else
	{
		R_E = OrbMech::R_Moon;
		mu = OrbMech::mu_Moon;
	}
	R_PD = R_E + H_P;
	beta = SystemParameters.MCGHZA;
	//0.1 NM tolerance
	eps1 = 185.2;
	iter_max = 10;

	W_E = mass;
	if (Thruster == RTCC_ENGINETYPE_CSMSPS)
	{
		F = SystemParameters.MCTST1;
	}
	else if (Thruster == RTCC_ENGINETYPE_LMAPS)
	{
		F = SystemParameters.MCTAT1;
	}
	else if (Thruster == RTCC_ENGINETYPE_LMDPS)
	{
		F = SystemParameters.MCTDT1*DPSScaleFactor;
	}
	else
	{
		F = SystemParameters.MCTLT2;
	}

	//Obtain present perigee based on input elements
	KAOP = 0;
	KE = 0;
	err = PMMAPD(sv.Header, sv.Data, KAOP, KE, INFO, &sv_temp, &sv_peri);

	if (err == 0)
	{
		PZPADDIS.H_A = INFO[4];
		PZPADDIS.H_P = INFO[9];
	}

	//Does present perifocus exceed desired?
	if (INFO[6] > R_PD)
	{
		//No maneuvers required
		PZPADDIS.Sol = 0;
		return;
	}

	PZPADDIS.T_thresh = THT;
	PZPADDIS.TimeStep = dt;

	do
	{
		//Advance either to the threshold time or the impulsive time of the next maneuver
		sv.Data.TIMA = 0;
		sv.Data.TE = T_next;
		PMMAEGS(sv.Header, sv.Data, sv_temp);

		//Does desired perigee exceed the current maneuver height?
		if (R_PD > sv_temp.R)
		{
			PZPADDIS.Man[man].TIG = -T_next;
		}
		else
		{
			//Compute the pitch and initialize iteration counter
			rho0 = beta - acos(R_E / sv_temp.R);
			iter = 0;

			rho = rho0;
			R = R_PD;

			r = sv_temp.R;
			V_b = sqrt(mu*(2.0 / r - 1.0 / sv_temp.coe_osc.a));
			X = sqrt(mu*sv_temp.coe_osc.a*(1.0 - sv_temp.coe_osc.e*sv_temp.coe_osc.e));
			r_dot_b = mu * sv_temp.coe_osc.e*sin(sv_temp.f) / X;
			V_H_b = X / r;

			endloop = 0;
			DV = 0.0;

			do
			{
				//Using original or corrected pitch, height of perigee desired (original or adjusted), and elements at T, compute the DV required
				rr = r * r;
				RR = R * R;
				A = rr*pow(cos(rho), 2) - RR;
				B = 2.0*V_H_b*cos(rho)*(rr - RR) - 2.0*RR*r_dot_b*sin(rho);
				C = rr * V_H_b*V_H_b - RR * V_b*V_b + 2.0*mu*R*(R - r) / r;
				D = B * B - 4.0*A*C;
				if (D < 0)
				{
					DV1 = DV2 = -1.0;
				}
				else
				{
					DV1 = (-B + sqrt(D)) / (2.0*A);
					DV2 = (-B - sqrt(D)) / (2.0*A);
				}

				//Is there a positive solution?
				if (DV1 > 0 || DV2 > 0)
				{
					//Are there two positive solutions?
					if (DV1 > 0 && DV2 > 0)
					{
						//Pick the smallest solution
						if (DV1 > DV2)
						{
							DV = DV2;
						}
						else
						{
							DV = DV1;
						}
					}
					else
					{
						//Pick the positive solution
						if (DV1 > 0)
						{
							DV = DV1;
						}
						else
						{
							DV = DV2;
						}
					}

					//Based on the DV required and pitch compute the elements after the maneuver
					sv_a = sv_temp;
					sv_a.ENTRY = 0;
					r_dot_a = r_dot_b + DV * sin(rho);
					V_H_a = V_H_b + DV * cos(rho);
					V_a2 = r_dot_a * r_dot_a + V_H_a * V_H_a;
					sv_a.coe_osc.a = mu * r / (2.0*mu - r * V_a2);
					eacosEa = (sv_a.coe_osc.a - r) / sv_a.coe_osc.a;
					easinEa = r_dot_a * r / sqrt(mu*sv_a.coe_osc.a);
					E_a = atan2(easinEa, eacosEa);
					if (E_a < 0)
					{
						E_a += PI2;
					}
					sv_a.coe_osc.e = sqrt(eacosEa*eacosEa + easinEa * easinEa);
					sv_a.coe_osc.l = E_a - easinEa;
					sv_a.f = atan2(sin(E_a)*(1.0 - sv_a.coe_osc.e*sv_a.coe_osc.e), cos(E_a) - sv_a.coe_osc.e);
					if (sv_a.f < 0)
					{
						sv_a.f += PI2;
					}
					sv_a.coe_osc.g = sv_temp.U - sv_a.f;
					if (sv_a.coe_osc.g < 0)
					{
						sv_a.coe_osc.g += PI2;
					}
					//With the elements after the maneuver, obtain perigee
					KAOP = -1;
					KE = 0;
					PMMAPD(sv.Header, sv_a, KAOP, KE, INFO, NULL, &sv_peri);

					//Is current perigee sufficiently close to the desired?
					R_PC = INFO[6];
					if (abs(R_PD - R_PC) < eps1)
					{
						//Good enough
						endloop = 1;
					}
					else
					{
						iter++;
						if (iter > iter_max)
						{
							endloop = 2;
						}
						else
						{
							if (iter == 1)
							{
								R_PKEP = sv_a.coe_osc.a*(1.0 - sv_a.coe_osc.e);
								R = R_PD + (R_PKEP - R_PC);
							}
							else if (iter == 2)
							{
								DR_PER_0 = R_PD - R_PC;
								DR_PER_C = DR_PER_0;
								R = R + DR_PER_C;
							}
							else
							{
								DR_PER_1 = R_PD - R_PC;
								DDR_PER = DR_PER_0 - DR_PER_1;
								DR_PER_C = (DR_PER_1 - DR_PER_0) / DDR_PER;
								DR_PER_0 = DR_PER_1;
								R = R + DR_PER_C;
							}
							//Using the thruster information compute DT based on DV, also burn initiate time from DT and T
							dt_man = DV / (F / W_E);
							f_dot = sqrt(mu*sv_a.coe_osc.a*(1.0 - sv_a.coe_osc.e*sv_a.coe_osc.e)) / rr;
							DP = dt_man / 2.0*f_dot;
							rho = rho0 + DP;
						}
					}
				}
				else
				{
					//No maneuver possible at this time
					PZPADDIS.Man[man].TIG = -T_next;
					endloop = -1;
				}
			} while (endloop == 0);

			if (endloop > 0)
			{
				//Obtain apogee height based on elements after the maneuver
				KAOP = 1;
				KE = 0;
				PMMAPD(sv.Header, sv_a, KAOP, KE, INFO, &sv_apo, NULL);

				//Store data
				PZPADDIS.Man[man].Pitch = beta + DP;
				PZPADDIS.Man[man].TIG = T_next - dt_man / 2.0;
				PZPADDIS.Man[man].DT = dt_man;
				PZPADDIS.Man[man].TA = sv_a.f;
				PZPADDIS.Man[man].H = r - R_E;
				PZPADDIS.Man[man].H_A = INFO[4];
				PZPADDIS.Man[man].DV = DV;
			}
		}
		//Have all maneuvers been computed?
		if (man == 5)
		{
			PZPADDIS.Sol = 1;
			return;
		}
		man++;
		T_next = T_next + dt;
	} while (man < 6);
}

void RTCC::PMDPAD()
{
	char Buffer[128];
	unsigned num = 0;

	MSK0050Buffer.clear();
	MSK0050Buffer.resize(64);

	if (PZPADDIS.Sol == 0)
	{
		sprintf(Buffer, "%.2lf", PZPADDIS.H_P / 1852.0);
		MSK0050Buffer[0].assign(Buffer);
		return;
	}

	sprintf(Buffer, "%.2lf", med_k28.H_P);
	MSK0050Buffer[num].assign(Buffer);
	num++;

	for (unsigned i = 0;i < 6;i++)
	{
		if (PZPADDIS.Man[i].TIG < 0)
		{
			OrbMech::format_time_HHMMSS(Buffer, abs(GETfromGMT(PZPADDIS.Man[i].TIG)));
			MSK0050Buffer[num].assign(Buffer);
			num += 6;
		}
		else
		{
			OrbMech::format_time_HHMMSS(Buffer, GETfromGMT(PZPADDIS.Man[i].TIG));
			MSK0050Buffer[num].assign(Buffer);
			num++;

			sprintf(Buffer, "%.0lf", PZPADDIS.Man[i].DV / 0.3048);
			MSK0050Buffer[num].assign(Buffer);
			num++;

			sprintf(Buffer, "%.1lf", PZPADDIS.Man[i].DT);
			MSK0050Buffer[num].assign(Buffer);
			num++;

			sprintf(Buffer, "%.0lf", PZPADDIS.Man[i].TA*DEG);
			MSK0050Buffer[num].assign(Buffer);
			num++;

			sprintf(Buffer, "%.0lf", PZPADDIS.Man[i].H / 1852.0);
			MSK0050Buffer[num].assign(Buffer);
			num++;

			sprintf(Buffer, "%.0lf", PZPADDIS.Man[i].H_A / 1852.0);
			MSK0050Buffer[num].assign(Buffer);
			num++;
		}
	}
}

void RTCC::PMMPAR(VECTOR3 RT, VECTOR3 VT, double TT)
{
	//IsCSMLaunch: true = LM launched first and launch targeting is for CSM

	LWPInputTable in;
	LWPGlobalConstants gl;
	LaunchWindowProcessor lwp(this);

	gl.mu = OrbMech::mu_Earth;
	gl.RREF = OrbMech::R_Earth;
	gl.w_E = OrbMech::w_Earth;

	in = PZSLVCON;
	in.RT = RT;
	in.VT = VT;
	in.TT = TT;

	lwp.SetGlobalConstants(gl);
	lwp.LWP(in);

	if (lwp.lwsum.LWPERROR) return;

	PZSLVTAR.GMTLO = lwp.lwsum.GMTLO;
	PZSLVTAR.TINS = lwp.lwsum.TINS;
	PZSLVTAR.GSTAR = lwp.rlott.GSTAR;
	PZSLVTAR.DN = lwp.lwsum.DN*DEG;
	PZSLVTAR.TPLANE = lwp.lwsum.TPLANE;
	PZSLVTAR.TYAW = lwp.rlott.TYAW;
	
	PZSLVTAR.TGRR = lwp.lwsum.GMTLO - in.DTGRR;
	PZSLVTAR.TGRR = PZSLVTAR.TGRR - trunc(PZSLVTAR.TGRR / 86400.0)*86400.0;
	PZSLVTAR.AZL = lwp.lwsum.AZL*DEG;
	PZSLVTAR.VIGM = lwp.lwsum.VIGM;
	PZSLVTAR.H = (lwp.lwsum.RIGM - OrbMech::R_Earth) / 1852.0;
	PZSLVTAR.AZP = 0.0; //TBD
	PZSLVTAR.RIGM = lwp.lwsum.RIGM;
	PZSLVTAR.GIGM = lwp.lwsum.GIGM*DEG;
	PZSLVTAR.IIGM = lwp.lwsum.IIGM*DEG;
	PZSLVTAR.TIGM = lwp.lwsum.TIGM*DEG;
	PZSLVTAR.TDIGM = lwp.lwsum.TDIGM*DEG;
	PZSLVTAR.DELNO = lwp.lwsum.DELNO*DEG;
	PZSLVTAR.DELNOD = lwp.rlott.DELNOD*DEG;
	PZSLVTAR.PA = lwp.lwsum.PA*DEG;
	PZSLVTAR.BIAS = PZSLVCON.BIAS;
	PZSLVTAR.LATLS = lwp.lwsum.LATLS*DEG;
	PZSLVTAR.LONGLS = lwp.lwsum.LONGLS*DEG;

	AEGHeader header;
	AEGDataBlock sv_apo, sv_peri;
	double INFO[10];
	PMMAPD(header, lwp.svtab.sv_T1, 0, -1, INFO, &sv_apo, &sv_peri);

	PZSLVTAR.HA_T = INFO[4] / 1852.0;
	PZSLVTAR.HP_T = INFO[9] / 1852.0;
	PZSLVTAR.I_T = lwp.svtab.sv_T1.coe_mean.i*DEG;
	PZSLVTAR.DN_T = lwp.svtab.sv_T1.coe_mean.h*DEG;

	PMMAPD(header, lwp.svtab.sv_C, 0, -1, INFO, &sv_apo, &sv_peri);

	PZSLVTAR.HA_C = INFO[4] / 1852.0;
	PZSLVTAR.HP_C = INFO[9] / 1852.0;
	PZSLVTAR.TA_C = lwp.svtab.sv_C.coe_osc.l*DEG;
	PZSLVTAR.DH = lwp.rlott.DH / 1852.0;
}

void RTCC::PMMIEV(int L, double T_L)
{
	TLITargetingParametersTable *tlitab = NULL;
	double T_D, A_Z, dt_EOI, lat_EOI, lng_EOI, azi_EOI, vel_EOI, fpa_EOI, rad_EOI;
	int day, counter, i, N;
	
	day = GZGENCSN.RefDayOfYear;

	for (counter = 0; counter < 10; counter++)
	{
		if (day == PZSTARGP.data[counter].Day)
		{
			tlitab = &PZSTARGP.data[counter];
			break;
		}
	}

	if (tlitab == NULL)
	{
		PMXSPT("PMMIEV", 102);
		return;
	}
	T_D = T_L - tlitab->T_LO;

	//Check if outside limits
	if (T_D < tlitab->t_DS0)
	{
		PMXSPT("PMMIEV", 121);
		return;
	}
	if (T_D < tlitab->t_DS1)
	{
		i = 0;
	}
	else if (T_D < tlitab->t_DS2)
	{
		i = 1;
	}
	else if (T_D <= tlitab->t_DS3)
	{
		i = 2;
	}
	else
	{
		PMXSPT("PMMIEV", 121);
		return;
	}

	//Compute launch azimuth
	A_Z = 0.0;

	switch (i)
	{
	case 0:
		for (N = 0; N < 5; N++)
		{
			A_Z += tlitab->hx[0][N] * pow((T_D - tlitab->t_D1) / tlitab->t_SD1, N);
		}
		break;
	case 1:
		for (N = 0; N < 5; N++)
		{
			A_Z += tlitab->hx[1][N] * pow((T_D - tlitab->t_D2) / tlitab->t_SD2, N);
		}
		break;
	default:
		for (N = 0; N < 5; N++)
		{
			A_Z += tlitab->hx[2][N] * pow((T_D - tlitab->t_D3) / tlitab->t_SD3, N);
		}
		break;
	}

	GZLTRA.Azimuth = A_Z;
	A_Z *= DEG;
	dt_EOI = SystemParameters.MDLIEV[0] + SystemParameters.MDLIEV[1] * A_Z + SystemParameters.MDLIEV[2] * pow(A_Z, 2) + SystemParameters.MDLIEV[3] * pow(A_Z, 3);
	lat_EOI = SystemParameters.MDLIEV[4] + SystemParameters.MDLIEV[5] * A_Z + SystemParameters.MDLIEV[6] * pow(A_Z, 2) + SystemParameters.MDLIEV[7] * pow(A_Z, 3);
	lng_EOI = SystemParameters.MDLIEV[8] + SystemParameters.MDLIEV[9] * A_Z + SystemParameters.MDLIEV[10] * pow(A_Z, 2) + SystemParameters.MDLIEV[11] * pow(A_Z, 3);
	azi_EOI = SystemParameters.MDLIEV[12] + SystemParameters.MDLIEV[13] * A_Z + SystemParameters.MDLIEV[14] * pow(A_Z, 2) + SystemParameters.MDLIEV[15] * pow(A_Z, 3);
	rad_EOI = SystemParameters.MDLEIC[0];
	fpa_EOI = SystemParameters.MDLEIC[1];
	vel_EOI = SystemParameters.MDLEIC[2];

	lat_EOI /= DEG;
	lng_EOI /= DEG;
	azi_EOI /= DEG;
	rad_EOI *= 1852.0;
	fpa_EOI /= DEG;
	vel_EOI *= 0.3048;

	VECTOR3 R, V;
	double GMT_EOI = T_L + dt_EOI;
	if (EMMXTR(GMT_EOI, rad_EOI, vel_EOI, lng_EOI, lat_EOI, fpa_EOI + PI05, azi_EOI, R, V))
	{
		PMXSPT("PMMIEV", 120);
		return;
	}
	//Print insertion vector
	RTCCONLINEMON.DoubleBuffer[0] = T_L;
	RTCCONLINEMON.DoubleBuffer[1] = A_Z;
	RTCCONLINEMON.DoubleBuffer[2] = GMT_EOI;
	RTCCONLINEMON.VectorBuffer[0] = R / OrbMech::R_Earth;
	RTCCONLINEMON.VectorBuffer[1] = V / OrbMech::R_Earth*3600.0;
	PMXSPT("PMMIEV", 122);

	//Write insertion vector in GZLTRA BLK (56, 62)
	GZLTRA.GMT_T = GMT_EOI;
	GZLTRA.R_T = R;
	GZLTRA.V_T = V;

	//ETMSCTRL for trajectory update
	StateVectorTableEntry sv0;

	sv0.VectorCode = "NOMINS";
	sv0.LandingSiteIndicator = false;
	sv0.Vector.R = R;
	sv0.Vector.V = V;
	sv0.Vector.GMT = GMT_EOI;
	sv0.Vector.RBI = BODY_EARTH;

	PMSVCT(0, L, sv0);
}

void RTCC::EMGENGEN(EphemerisDataTable2 &ephemeris, ManeuverTimesTable &MANTIMES, const StationTable &stationlist, int body, OrbitStationContactsTable &res, LunarStayTimesTable *LUNSTAY)
{
	std::vector<StationContact> acquisitions;
	StationContact current;
	StationContact empty;
	int err;

	if (stationlist.table.size() == 0) return;

	for (unsigned i = 0;i < stationlist.table.size();i++)
	{
		err = EMXING(ephemeris, MANTIMES, stationlist.table[i], body, acquisitions, LUNSTAY);

		if (err)
		{
			RTCCONLINEMON.IntBuffer[0] = err;
			if (ephemeris.Header.VEH == RTCC_MPT_CSM)
			{
				RTCCONLINEMON.TextBuffer[0] = "CSM";
			}
			else
			{
				RTCCONLINEMON.TextBuffer[0] = "LEM";
			}
			RTCCONLINEMON.TextBuffer[1] = stationlist.table[i].code;
			EMGPRINT("EMGENGEN", 55);
		}
	}

	//Sort
	std::sort(acquisitions.begin(), acquisitions.end());

	//Put first 45 in table
	res.Num = 0;
	for (unsigned i = 0;i < 45;i++)
	{
		if (i < acquisitions.size())
		{
			res.Stations[i] = acquisitions[i];
			res.Num++;
		}
		else
		{
			res.Stations[i] = empty;
		}
	}
}

bool RTCC::EMXINGLunarOccultation(EphemerisDataTable2 &ephemeris, ManeuverTimesTable &MANTIMES, double gmt, VECTOR3 R_S_equ, double &g_func, LunarStayTimesTable *LUNSTAY)
{
	ELVCTRInputTable interin;
	ELVCTROutputTable2 interout;

	interin.GMT = gmt;
	//Interpolate ephemeris
	ELVCTR(interin, interout, ephemeris, MANTIMES, LUNSTAY);
	if (interout.ErrorCode > 2) return true;
	//If radius is smaller than 50 er it can't be occulted by the Moon
	if (length(interout.SV.R) < 50.0*OrbMech::R_Earth)
	{
		g_func = 1000.0;
		return false;
	}

	VECTOR3 N, rho, r_M, rho1, rho_apo1;
	double sinang;
	EphemerisData2 sv_E, sv_ET;

	//Calculate elevation
	OrbMech::EMXINGElev(interout.SV.R, R_S_equ, N, rho, sinang);
	//Get moon state vector at interpolation time
	if (PLEFEM(4, interin.GMT / 3600.0, 0, &sv_E.R, &sv_E.V, NULL, NULL))
	{
		return true;
	}
	//Is spacecraft further away from the Earth than the Moon is?
	if (length(interout.SV.R) <= length(sv_E.R))
	{
		//No
		g_func = 1000.0;
		return false;
	}
	//Convert moon state vector to ECT
	sv_E.GMT = interin.GMT;
	if (ELVCNV(sv_E, 0, 1, sv_ET))
	{
		return true;
	}

	r_M = interout.SV.R - sv_ET.R;
	rho1 = crossp(unit(crossp(rho, r_M)), rho);
	rho_apo1 = unit(rho1);
	g_func = dotp(rho_apo1, r_M) - SystemParameters.MCSMLR;
	return false;
}

//Calculates station vector in ECT coordinates
VECTOR3 EMXING_Station_ECT(double GMT, double R_E_sin_lat, double R_E_cos_lat, double stat_lng)
{
	double lng = stat_lng + OrbMech::w_Earth*GMT;
	return _V(R_E_cos_lat*cos(lng), R_E_cos_lat*sin(lng), R_E_sin_lat);
}

int RTCC::EMXING(EphemerisDataTable2 &ephemeris, ManeuverTimesTable &MANTIMES, const StationData &station, int body, std::vector<StationContact> &acquisitions, LunarStayTimesTable *LUNSTAY)
{
	//Return codes: 0 = no error, 1 = mismatch between station and ephemeris type, 2 = interpolation error,  3 = convergence error

	if (ephemeris.table.size() == 0) return false;
	//Has to be ECT or MCT
	if (body == BODY_EARTH && ephemeris.Header.CSI != 1) return 1;
	if (body == BODY_MOON && ephemeris.Header.CSI != 3) return 1;

	ELVCTRInputTable interin;
	ELVCTROutputTable2 interout;
	EphemerisData2 svtemp, sv_AOS;
	StationContact current;
	VECTOR3 R_S_equ, R, rho, N, Ntemp, rhotemp, V;
	double GMT, sinang, GMT_AOS, LastGMT, LastSinang, GMT0, f, last_f, GMT_EMAX, sinangtemp, EMAX, GMT_LOS, R_E;
	unsigned iter = 0;
	int n, nmax;
	bool BestAvailableAOS, BestAvailableLOS, BestAvailableEMAX;

	if (body == BODY_EARTH)
	{
		R_E = OrbMech::R_Earth;
	}
	else
	{
		R_E = OrbMech::R_Moon;
	}

	GMT0 = ephemeris.table[0].GMT;

	//If coordinate system is MCT this only needs to be calculated once
	if (body == BODY_MOON) R_S_equ = _V(station.R_E_cos_lat*station.cos_lng, station.R_E_cos_lat*station.sin_lng, station.R_E_sin_lat);
	current.StationID = station.code;

EMXING_LOOP:

	BestAvailableAOS = false, BestAvailableLOS = false, BestAvailableEMAX = false;
	n = 0;
	nmax = 10;
	f = last_f = 0.0;

	//Find AOS
	while (ephemeris.table.size() > iter)
	{
		R = ephemeris.table[iter].R;
		V = ephemeris.table[iter].V;
		GMT = ephemeris.table[iter].GMT;

		if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(GMT, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
		OrbMech::EMXINGElev(R, R_S_equ, N, rho, sinang);
		f = OrbMech::EMXINGElevSlope(R, V, R_S_equ, body);

		//Elevation angle above 0, there is an AOS
		if (sinang >= 0) break;
		//Slope of the elevation curve has changed sign, there might be an AOS
		if (iter > 0 && f*last_f < 0)
		{
			GMT_EMAX = OrbMech::LinearInterpolation(f, GMT, last_f, LastGMT, 0.0);
			interin.GMT = GMT_EMAX;
			ELVCTR(interin, interout, ephemeris, MANTIMES, LUNSTAY);
			if (interout.ErrorCode)
			{
				return 2;
			}
			svtemp = interout.SV;

			if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(interin.GMT, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
			OrbMech::EMXINGElev(svtemp.R, R_S_equ, Ntemp, rhotemp, sinangtemp);
			//Elevation angle above 0, there is an AOS
			if (sinangtemp >= 0) break;
		}

		LastGMT = GMT;
		LastSinang = sinang;
		last_f = f;
		iter++;
	}

	if (iter == ephemeris.table.size())
	{
		//Out of ephemeris, no error
		return 0;
	}

	//Already in AOS
	if (iter == 0)
	{
		GMT_AOS = GMT;
		svtemp = ephemeris.table.front();
		BestAvailableAOS = true;
	}
	else
	{
		GMT_AOS = OrbMech::LinearInterpolation(sinang, GMT, LastSinang, LastGMT, 0.0);
		interin.GMT = GMT_AOS;
		ELVCTR(interin, interout, ephemeris, MANTIMES, LUNSTAY);
		if (interout.ErrorCode)
		{
			return 2;
		}
		svtemp = interout.SV;

		n = 0;

		while (abs(LastGMT - GMT_AOS) >= 3.0 && nmax > n)
		{
			R = svtemp.R;
			V = svtemp.V;
			GMT = svtemp.GMT;

			if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(GMT, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
			OrbMech::EMXINGElev(R, R_S_equ, N, rho, sinang);

			GMT_AOS = OrbMech::LinearInterpolation(sinang, GMT, LastSinang, LastGMT, 0.0);
			interin.GMT = GMT_AOS;
			ELVCTR(interin, interout, ephemeris, MANTIMES, LUNSTAY);
			if (interout.ErrorCode)
			{
				return 2;
			}
			svtemp = interout.SV;

			LastGMT = GMT;
			LastSinang = sinang;
			n++;
		}
	}

	sv_AOS = svtemp;

	//Find maximum elevation angle
	if (iter > 0) iter--;

	while (ephemeris.table.size() > iter)
	{
		R = ephemeris.table[iter].R;
		V = ephemeris.table[iter].V;
		GMT = ephemeris.table[iter].GMT;

		if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(GMT, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
		f = OrbMech::EMXINGElevSlope(R, V, R_S_equ, body);

		//EMAX before first SV in ephemeris
		if (iter == 0 && f < 0)
		{
			svtemp = ephemeris.table.front();
			GMT_EMAX = ephemeris.table.front().GMT;
			BestAvailableEMAX = true;
			break;
		}
		//Sign is changing, EMAX was passed
		if (f*last_f < 0)
		{
			GMT_EMAX = OrbMech::LinearInterpolation(f, GMT, last_f, LastGMT, 0.0);
			interin.GMT = GMT_EMAX;
			ELVCTR(interin, interout, ephemeris, MANTIMES, LUNSTAY);
			if (interout.ErrorCode)
			{
				return 2;
			}
			svtemp = interout.SV;

			LastGMT = GMT;
			last_f = f;
			break;
		}

		LastGMT = GMT;
		last_f = f;
		iter++;
	}

	if (BestAvailableEMAX == false)
	{
		//Out of SVs in the ephemeris, EMAX must be after end of ephemeris
		if (iter == ephemeris.table.size())
		{
			svtemp = ephemeris.table.back();
			GMT_EMAX = ephemeris.table.back().GMT;
			BestAvailableEMAX = true;
		}
		else
		{
			n = 0;

			while (abs(LastGMT - GMT_EMAX) >= 3.0 && nmax > n)
			{
				R = svtemp.R;
				V = svtemp.V;
				GMT = svtemp.GMT;

				if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(GMT, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
				f = OrbMech::EMXINGElevSlope(R, V, R_S_equ, body);

				GMT_EMAX = OrbMech::LinearInterpolation(f, GMT, last_f, LastGMT, 0.0);
				interin.GMT = GMT_EMAX;
				ELVCTR(interin, interout, ephemeris, MANTIMES, LUNSTAY);
				if (interout.ErrorCode)
				{
					return 2;
				}
				svtemp = interout.SV;

				LastGMT = GMT;
				last_f = f;
				n++;
			}
		}
	}

	if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(svtemp.GMT, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
	OrbMech::EMXINGElev(svtemp.R, R_S_equ, N, rho, sinang);
	EMAX = asin(sinang);

	//Find LOS
	if (iter > 0) iter--;

	while (ephemeris.table.size() > iter)
	{
		R = ephemeris.table[iter].R;
		V = ephemeris.table[iter].V;
		GMT = ephemeris.table[iter].GMT;

		if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(GMT, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
		OrbMech::EMXINGElev(R, R_S_equ, N, rho, sinang);
		f = OrbMech::EMXINGElevSlope(R, V, R_S_equ, body);

		//Elevation angle below 0, there is an LOS
		if (sinang < 0 && f < 0) break;

		LastGMT = GMT;
		LastSinang = sinang;
		iter++;
	}

	if (iter == ephemeris.table.size())
	{
		GMT_LOS = ephemeris.table.back().GMT;
		BestAvailableLOS = true;
	}
	else
	{
		GMT_LOS = OrbMech::LinearInterpolation(sinang, GMT, LastSinang, LastGMT, 0.0);
		interin.GMT = GMT_LOS;
		ELVCTR(interin, interout, ephemeris, MANTIMES, LUNSTAY);
		if (interout.ErrorCode)
		{
			return 2;
		}
		svtemp = interout.SV;

		n = 0;

		while (abs(LastGMT - GMT_LOS) >= 3.0 && nmax > n)
		{
			R = svtemp.R;
			V = svtemp.V;
			GMT = svtemp.GMT;

			if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(GMT, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
			OrbMech::EMXINGElev(R, R_S_equ, N, rho, sinang);

			GMT_LOS = OrbMech::LinearInterpolation(sinang, GMT, LastSinang, LastGMT, 0.0);
			interin.GMT = GMT_LOS;
			ELVCTR(interin, interout, ephemeris, MANTIMES, LUNSTAY);
			if (interout.ErrorCode)
			{
				return 2;
			}
			svtemp = interout.SV;

			LastGMT = GMT;
			LastSinang = sinang;
			n++;
		}
	}

	//Check if Moon occults AOS
	if (body == BODY_EARTH && ephemeris.Header.CSI == 1)
	{
		//Preliminary, write station contact data
		current.GMTAOS = GMT_AOS;
		current.GMTLOS = GMT_LOS;
		current.GMTEMAX = GMT_EMAX;
		current.MAXELEV = EMAX;
		current.BestAvailableAOS = BestAvailableAOS;
		current.BestAvailableLOS = BestAvailableLOS;
		current.BestAvailableEMAX = BestAvailableEMAX;

		double g_func, GMT_iter2, g_func_last, LastGMT_lunar, GMT_new;
		unsigned int iter2 = 0;

		GMT_iter2 = GMT_AOS;

		//Find first SV after AOS
		while (iter2 < (ephemeris.table.size() - 1) && ephemeris.table[iter2].GMT <= GMT_iter2)
		{
			iter2++;
		}

		//Look if already in AOS at horizon crossing
		if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(GMT_iter2, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
		EMXINGLunarOccultation(ephemeris, MANTIMES, GMT_iter2, R_S_equ, g_func, LUNSTAY);

		//Store so that we have any data
		LastGMT_lunar = GMT_iter2;
		g_func_last = g_func;

		//If we are in sight, go to logic for finding LOS
		if (g_func >= 0)
		{
			goto RTCC_EMXING_LUNAR_LOS1;
		}

	RTCC_EMXING_LUNAR_AOS1:

		g_func = -100.0;

		//Find AOS
		while (g_func < 0)
		{
			if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(GMT_iter2, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
			EMXINGLunarOccultation(ephemeris, MANTIMES, GMT_iter2, R_S_equ, g_func, LUNSTAY);
			if (g_func >= 0)
			{
				break;
			}
			LastGMT_lunar = GMT_iter2;
			g_func_last = g_func;
			GMT_iter2 = ephemeris.table[iter2].GMT;
			if (GMT_iter2 >= GMT_LOS)
			{
				//Found no more AOS, go to end
				goto RTCC_EMXING_END;
			}
			iter2++;
		}

		//Bracket AOS
		n = 0;
		GMT_iter2 = OrbMech::LinearInterpolation(g_func, GMT_iter2, g_func_last, LastGMT_lunar, 0.0);
		do
		{
			if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(GMT_iter2, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
			EMXINGLunarOccultation(ephemeris, MANTIMES, GMT_iter2, R_S_equ, g_func, LUNSTAY);
			GMT_new = GMT_iter2;
			GMT_iter2 = OrbMech::LinearInterpolation(g_func, GMT_iter2, g_func_last, LastGMT_lunar, 0.0);
			n++;
		} while (abs(GMT_new - GMT_iter2) > 3.0 && n < nmax);

		if (n >= nmax) return 3; //Didn't converge

		current.GMTAOS = GMT_iter2;
		current.BestAvailableAOS = true;

	RTCC_EMXING_LUNAR_LOS1:
		g_func = 100.0;
		//Find first SV in ephemeris after GMT_iter2
		while (iter2 < (ephemeris.table.size() - 1) && ephemeris.table[iter2].GMT <= GMT_iter2)
		{
			iter2++;
		}
		GMT_iter2 = ephemeris.table[iter2].GMT;
		while (g_func >= 0)
		{
			if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(GMT_iter2, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
			EMXINGLunarOccultation(ephemeris, MANTIMES, GMT_iter2, R_S_equ, g_func, LUNSTAY);
			if (g_func < 0)
			{
				break;
			}
			LastGMT_lunar = GMT_iter2;
			g_func_last = g_func;
			GMT_iter2 = ephemeris.table[iter2].GMT;
			if (GMT_iter2 >= GMT_LOS)
			{
				//Last state vector to consider
				GMT_iter2 = GMT_LOS;
				goto RTCC_EMXING_LUNAR_LOS3;
			}
			iter2++;
		}

		//Bracket LOS
		n = 0;
		GMT_iter2 = OrbMech::LinearInterpolation(g_func, GMT_iter2, g_func_last, LastGMT_lunar, 0.0);
		do
		{
			if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(GMT_iter2, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
			EMXINGLunarOccultation(ephemeris, MANTIMES, GMT_iter2, R_S_equ, g_func, LUNSTAY);
			GMT_new = GMT_iter2;
			GMT_iter2 = OrbMech::LinearInterpolation(g_func, GMT_iter2, g_func_last, LastGMT_lunar, 0.0);
			n++;
		} while (abs(GMT_new - GMT_iter2) > 3.0 && n < nmax);

		if (n >= nmax) return 3; //Didn't converge

		//If time of LOS (lunar occulation) is after actual LOS (station in sight)
		if (GMT_iter2 >= GMT_LOS)
		{
			GMT_iter2 = GMT_LOS;
		}

	RTCC_EMXING_LUNAR_LOS3:

		//Write station contact
		current.GMTLOS = GMT_iter2;
		if (current.GMTLOS >= GMT_LOS)
		{
			current.BestAvailableLOS = BestAvailableLOS;
		}
		else
		{
			current.BestAvailableLOS = true;
		}

		if (GMT_EMAX >= current.GMTAOS && GMT_EMAX <= current.GMTLOS)
		{
			//EMAX within AOS and LOS
			current.GMTEMAX = current.GMTLOS;
			current.MAXELEV = EMAX;
			current.BestAvailableEMAX = BestAvailableEMAX;
		}
		else
		{
			//Only best available EMAX
			current.BestAvailableEMAX = true;
			if (GMT_EMAX < current.GMTAOS)
			{
				current.GMTEMAX = current.GMTAOS;
			}
			else if (GMT_EMAX > current.GMTLOS)
			{
				current.GMTEMAX = current.GMTLOS;
			}
			//Calculate EMAX
			interin.GMT = current.GMTEMAX;
			ELVCTR(interin, interout, ephemeris, MANTIMES, LUNSTAY);
			if (body == BODY_EARTH) R_S_equ = EMXING_Station_ECT(interout.SV.GMT, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
			OrbMech::EMXINGElev(interout.SV.R, R_S_equ, N, rho, sinang);
			current.MAXELEV = asin(sinang);
		}
		
		acquisitions.push_back(current);

		//Find next ephemeris SV
		while (iter2 < (ephemeris.table.size() - 1) && ephemeris.table[iter2].GMT <= GMT_iter2)
		{
			iter2++;
		}
		GMT_iter2 = ephemeris.table[iter2].GMT;

		if (GMT_iter2 < GMT_LOS)
		{
			//Find next AOS
			goto RTCC_EMXING_LUNAR_AOS1;
		}
	}
	else
	{
		current.GMTAOS = GMT_AOS;
		current.GMTLOS = GMT_LOS;
		current.GMTEMAX = GMT_EMAX;
		current.MAXELEV = EMAX;
		current.BestAvailableAOS = BestAvailableAOS;
		current.BestAvailableLOS = BestAvailableLOS;
		current.BestAvailableEMAX = BestAvailableEMAX;

		acquisitions.push_back(current);
	}

RTCC_EMXING_END:

	if (iter < ephemeris.table.size() && acquisitions.size() <= 45)
	{
		goto EMXING_LOOP;
	}

	return 0;
}

void RTCC::EMDSTAC()
{
	double GMT = RTCCPresentTimeGMT();
	double GET = GETfromGMT(GMT);

	//Clear table
	for (unsigned l = 0;l < 2;l++)
	{
		for (unsigned k = 0;k < 6;k++)
		{
			NextStationContactsBuffer.STA[l][k] = "";
			NextStationContactsBuffer.GETHCA[l][k] = false;
			NextStationContactsBuffer.DTKLOS[l][k] = false;
			NextStationContactsBuffer.EMAX[l][k] = false;
			NextStationContactsBuffer.DTPASS[l][k] = 0.0;
			NextStationContactsBuffer.DTKH[l][k] = 0.0;
		}
	}

	OrbitStationContactsTable *tab;
	unsigned j;
	//Search through all station contacts
	for (unsigned l = 0;l < 2;l++)
	{
		if (l == 0)
		{
			tab = &EZSTACT1;
		}
		else
		{
			tab = &EZSTACT3;
		}

		j = 0;
		for (unsigned i = 0;i < 45;i++)
		{
			//If we got 6 contacts we are done
			if (j >= 6) break;
			//Skip over contacts from the past
			if (GMT > tab->Stations[i].GMTLOS)
			{
				continue;
			}
			//Found one
			NextStationContactsBuffer.STA[l][j] = tab->Stations[i].StationID;
			NextStationContactsBuffer.GETHCA[l][j] = GETfromGMT(tab->Stations[i].GMTAOS);
			NextStationContactsBuffer.DTKLOS[l][j] = 0.0;
			NextStationContactsBuffer.EMAX[l][j] = tab->Stations[i].MAXELEV*DEG;
			NextStationContactsBuffer.DTPASS[l][j] = tab->Stations[i].GMTLOS - tab->Stations[i].GMTAOS;
			NextStationContactsBuffer.DTKH[l][j] = 0.0;
			NextStationContactsBuffer.BestAvailableAOS[l][j] = tab->Stations[i].BestAvailableAOS;
			NextStationContactsBuffer.BestAvailableEMAX[l][j] = tab->Stations[i].BestAvailableEMAX;

			//If we are in AOS, modify some numbers accordingly
			if (GMT > tab->Stations[i].GMTAOS)
			{
				NextStationContactsBuffer.GETHCA[l][j] = GET;
				NextStationContactsBuffer.DTPASS[l][j] = tab->Stations[i].GMTLOS - GMT;
				NextStationContactsBuffer.BestAvailableAOS[l][j] = true;
			}
			if (GMT > tab->Stations[i].GMTEMAX)
			{
				NextStationContactsBuffer.BestAvailableEMAX[l][j] = true;
			}
			j++;
		}
	}
	//Save current GET for the 12 second update cycle
	NextStationContactsBuffer.GET = GET;
}

void RTCC::EMDLANDM(int L, double gmt, double dt, int ref)
{
	EphemerisData2 sv_glssat;
	ManeuverTimesTable mantimes;
	VECTOR3 R_stat;
	double GMT_Begin, GMT_End;
	unsigned i = 0, j = 0;
	int stat_body;

	GMT_Begin = gmt;
	GMT_End = gmt + dt;

	StationData station;
	StationTable contact;

	stat_body = EZLASITE.REF;
	if (stat_body < 0)
	{
		EZLANDU1.err = 6;
		return;
	}
	if (stat_body != ref)
	{
		EZLANDU1.err = 9;
		return;
	}
	for (int i = 0;i < 12;i++)
	{
		if (EZLASITE.Data[i].code == "") continue;
		station = EZLASITE.Data[i];
		contact.table.push_back(station);
	}

	int TUP, out;
	unsigned NumVec;
	OrbitEphemerisTable ephem;

	if (stat_body == BODY_EARTH)
	{
		out = 1;
	}
	else
	{
		out = 3;
	}

	//Get state vectors
	ELNMVC(GMT_Begin, GMT_End, RTCC_MPT_CSM, NumVec, TUP);
	ELFECH(GMT_Begin, NumVec, 1, RTCC_MPT_CSM, ephem.EPHEM, ephem.MANTIMES, ephem.LUNRSTAY);

	//Convert to ECT or MCT
	EphemerisDataTable2 ephem2;
	ELVCNV(ephem.EPHEM.table, 0, out, ephem2.table);
	ephem2.Header = ephem.EPHEM.Header;
	ephem2.Header.CSI = out;

	//Generate station contacts
	OrbitStationContactsTable res;
	EMGENGEN(ephem2, ephem.MANTIMES, contact, stat_body, res, &ephem.LUNRSTAY);

	LandmarkAcquisitionTable *tab = &EZLANDU1;
	*tab = LandmarkAcquisitionTable();

	ELVCTRInputTable intab;
	ELVCTROutputTable2 outtab;

	intab.L = RTCC_MPT_CSM;

	std::vector<int> maxcont;
	maxcont.resize(12);

	StationData tempstat;
	VECTOR3 p, r;
	double lambda, lat, lng, alt;

	int numcontacts = 0;
	for (int i = 0;i < 45;i++)
	{
		if (res.Stations[i].GMTAOS <= 0.0 || res.Stations[i].GMTAOS > GMT_End)
		{
			continue;
		}
		for (int j = 0;j < 12;j++)
		{
			if (contact.table[j].code == res.Stations[i].StationID)
			{
				tempstat = contact.table[j];

				if (maxcont[j] >= 5)
				{
					continue;
				}
				maxcont[j]++;
				break;
			}
		}

		tab->STAID[tab->pages - 1][numcontacts] = res.Stations[i].StationID;
		tab->GETAOS[tab->pages - 1][numcontacts] = GETfromGMT(res.Stations[i].GMTAOS);
		tab->GETCA[tab->pages - 1][numcontacts] = GETfromGMT(res.Stations[i].GMTEMAX);
		tab->GETLOS[tab->pages - 1][numcontacts] = GETfromGMT(res.Stations[i].GMTLOS);
		tab->BestAvailableAOS[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableAOS;
		tab->BestAvailableLOS[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableLOS;
		tab->BestAvailableCA[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableEMAX;

		intab.GMT = res.Stations[i].GMTEMAX;

		//Interpolate ECT or MCT vector
		ELVCTR(intab, outtab, ephem2, mantimes, NULL);
		sv_glssat = outtab.SV;

		//Calculate ground point in ECT or MCT coordinates
		if (stat_body == BODY_EARTH)
		{
			R_stat = EMXING_Station_ECT(intab.GMT, tempstat.R_E_sin_lat, tempstat.R_E_cos_lat, tempstat.lng);
		}
		else
		{
			R_stat = _V(tempstat.R_E_cos_lat*tempstat.cos_lng, tempstat.R_E_cos_lat*tempstat.sin_lng, tempstat.R_E_sin_lat);
		}

		p = unit(sv_glssat.R - R_stat);
		r = unit(sv_glssat.R);
		lambda = acos(dotp(p, r));

		GLSSAT(sv_glssat.R, sv_glssat.GMT, stat_body, lat, lng, alt);

		tab->h[tab->pages - 1][numcontacts] = alt / 1852.0;
		tab->Lambda[tab->pages - 1][numcontacts] = lambda*DEG;

		//Local sunrise/sunset (TBD)
		tab->GETSR[tab->pages - 1][numcontacts] = 0.0;
		tab->GETSS[tab->pages - 1][numcontacts] = 0.0;

		numcontacts++;
		if (numcontacts >= 20)
		{
			tab->pages++;
			numcontacts = 0;
		}
	}
	if (tab->pages == 1)
	{
		tab->numcontacts[0] = numcontacts;
		tab->numcontacts[1] = 0;
		tab->numcontacts[2] = 0;
	}
	else if (tab->pages == 2)
	{
		tab->numcontacts[0] = 20;
		tab->numcontacts[1] = numcontacts;
		tab->numcontacts[2] = 0;
	}
	else if (tab->pages == 3)
	{
		tab->numcontacts[0] = 20;
		tab->numcontacts[1] = 20;
		tab->numcontacts[2] = numcontacts;
	}

	tab->VectorID = EZTSCNS1.StationID;
	tab->TUN = ephem2.Header.TUP;
	tab->err = 0;
}

void RTCC::ECMEXP(EphemerisData sv, StationData *stat, int statbody, double &range, double &alt)
{
	EphemerisData sv_out;
	VECTOR3 Q, U_R;
	int out;

	U_R = EMXING_Station_ECT(sv.GMT, stat->R_E_sin_lat, stat->R_E_cos_lat, stat->lng);
	if (statbody == BODY_EARTH)
	{
		out = 1;
	}
	else
	{
		out = 3;
	}
	ELVCNV(sv, out, sv_out);
	
	Q = unit(crossp(sv_out.V, sv_out.R));
	range = abs(stat->R_S * asin(dotp(U_R, Q)));
	alt = length(sv_out.R) - stat->R_S;
}

void RTCC::EMDPESAD(int num, int veh, int ind, double vala, double valb, int body)
{
	StationContact empty;
	double GMT_Begin, GMT_End;
	unsigned i = 0, j = 0;
	int stat_body;

	if (ind == 1)
	{
		GMT_Begin = GMTfromGET(vala);
		GMT_End = GMTfromGET(valb);
	}
	else
	{
		GMT_Begin = CapeCrossingGMT(veh, (int)vala);
		if (GMT_Begin < 0)
		{
			GMT_Begin = CapeCrossingFirst(veh);
		}
		GMT_End = CapeCrossingGMT(veh, 1 + (int)valb);
		if (GMT_End < 0)
		{
			GMT_End = CapeCrossingLast(veh);
		}
	}

	if (GMT_End < 0)
	{
		return;
	}
	if (GMT_Begin < RTCCPresentTimeGMT())
	{
		GMT_Begin = RTCCPresentTimeGMT();
	}

	StationData station;
	StationTable contact;

	//Experimental site
	if (num == 3)
	{
		//TBD
		stat_body = EZEXSITE.REF;
		if (stat_body < 0)
		{
			return;
		}
		for (int i = 0;i < 12;i++)
		{
			if (EZEXSITE.Data[i].code == "") continue;
			station = EZEXSITE.Data[i];
			contact.table.push_back(station);
		}
	}
	//Predicted site
	else
	{
		stat_body = BODY_EARTH;
		for (int i = 0;i < SystemParameters.MKRBKS;i++)
		{
			if (SystemParameters.MGRTAG == 0 && GZSTCH[i].SiteType != 8) continue;

			contact.table.push_back(GZSTCH[i].data);
		}
	}

	int TUP, out;
	unsigned NumVec;
	OrbitEphemerisTable ephem;

	if (stat_body == BODY_EARTH)
	{
		out = 1;
	}
	else
	{
		out = 3;
	}

	ELNMVC(GMT_Begin, GMT_End, veh, NumVec, TUP);
	ELFECH(GMT_Begin, NumVec, 1, veh, ephem.EPHEM, ephem.MANTIMES, ephem.LUNRSTAY);

	EphemerisDataTable2 ephem2;
	ELVCNV(ephem.EPHEM.table, 0, out, ephem2.table);
	ephem2.Header = ephem.EPHEM.Header;
	ephem2.Header.CSI = out;

	if (ephem2.Header.NumVec < 9)
	{
		return;
	}

	OrbitStationContactsTable res;
	EMGENGEN(ephem2, ephem.MANTIMES, contact, stat_body, res, &ephem.LUNRSTAY);

	//Experimental site
	if (num == 3)
	{
		ExperimentalSiteAcquisitionTable *tab = &EZDPSAD2;

		ExperimentalSiteAcquisitionTable empty;
		*tab = empty;

		ELVCTRInputTable intab;
		ELVCTROutputTable2 outtab;
		StationData tempstat;
		double range, alt;

		intab.L = 1;

		int numcontacts = 0;
		for (int i = 0;i < 42;i++)
		{
			if (res.Stations[i].GMTAOS <= 0.0 || res.Stations[i].GMTAOS > GMT_End)
			{
				continue;
			}

			intab.GMT = res.Stations[i].GMTEMAX;
			ELVCTR(intab, outtab);

			for (unsigned j = 0;j < contact.table.size();j++)
			{
				if (contact.table[j].code == res.Stations[i].StationID)
				{
					tempstat = contact.table[j];
					break;
				}
			}
			ECMEXP(Eph2ToEph1(outtab.SV, BODY_EARTH), &tempstat, stat_body, range, alt);

			tab->REV[tab->pages - 1][numcontacts] = CapeCrossingRev(veh, res.Stations[i].GMTAOS);
			tab->STA[tab->pages - 1][numcontacts] = res.Stations[i].StationID;
			tab->GETAOS[tab->pages - 1][numcontacts] = GETfromGMT(res.Stations[i].GMTAOS);
			tab->GNDRNG[tab->pages - 1][numcontacts] = range / 1852.0;
			tab->ALT[tab->pages - 1][numcontacts] = alt / 1852.0;
			tab->ELMAX[tab->pages - 1][numcontacts] = res.Stations[i].MAXELEV*DEG;
			tab->GETCA[tab->pages - 1][numcontacts] = GETfromGMT(res.Stations[i].GMTEMAX);
			tab->GETLOS[tab->pages - 1][numcontacts] = GETfromGMT(res.Stations[i].GMTLOS);
			tab->BestAvailableAOS[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableAOS;
			tab->BestAvailableLOS[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableLOS;
			tab->BestAvailableEMAX[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableEMAX;
			numcontacts++;
			if (numcontacts >= 20)
			{
				tab->pages = 2;
				numcontacts = 0;
			}
		}
		if (tab->pages == 1)
		{
			tab->numcontacts[0] = numcontacts;
			tab->numcontacts[1] = 0;
		}
		else if (tab->pages == 2)
		{
			tab->numcontacts[0] = 20;
			tab->numcontacts[1] = numcontacts;
		}
	}
	//Predicted site
	else
	{
		PredictedSiteAcquisitionTable *tab;

		if (veh == 1)
		{
			if (num == 1)
			{
				tab = &EZACQ1;
			}
			else
			{
				tab = &EZDPSAD1;
			}
		}
		else
		{
			if (num == 1)
			{
				tab = &EZACQ3;
			}
			else
			{
				tab = &EZDPSAD3;
			}
		}

		PredictedSiteAcquisitionTable empty;
		*tab = empty;

		if (veh == 1)
		{
			tab->VEHICLE = "CSM";
		}
		else
		{
			tab->VEHICLE = "LEM";
		}

		int numcontacts = 0;
		for (int i = 0;i < 42;i++)
		{
			if (res.Stations[i].GMTAOS <= 0.0 || res.Stations[i].GMTAOS > GMT_End)
			{
				continue;
			}
			tab->STA[tab->pages - 1][numcontacts] = res.Stations[i].StationID;
			tab->GETHCA[tab->pages - 1][numcontacts] = GETfromGMT(res.Stations[i].GMTAOS);
			tab->GETHCD[tab->pages - 1][numcontacts] = GETfromGMT(res.Stations[i].GMTLOS);
			tab->ELMAX[tab->pages - 1][numcontacts] = res.Stations[i].MAXELEV*DEG;
			tab->REV[tab->pages - 1][numcontacts] = CapeCrossingRev(veh, res.Stations[i].GMTAOS);
			tab->BestAvailableAOS[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableAOS;
			tab->BestAvailableLOS[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableLOS;
			tab->BestAvailableEMAX[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableEMAX;
			numcontacts++;
			if (numcontacts >= 21)
			{
				tab->pages = 2;
				numcontacts = 0;
			}
		}
		if (tab->pages == 1)
		{
			tab->numcontacts[0] = numcontacts;
			tab->numcontacts[1] = 0;
		}
		else if (tab->pages == 2)
		{
			tab->numcontacts[0] = 21;
			tab->numcontacts[1] = numcontacts;
		}
	}
}

void RTCC::PMMAEGS(AEGHeader &header, AEGDataBlock &in, AEGDataBlock &out)
{
	if (header.AEGInd == BODY_EARTH)
	{
		pmmaeg.CALL(header, in, out);
	}
	else
	{
		pmmlaeg.CALL(header, in, out);
	}
}

void RTCC::PMMTLC(AEGHeader HEADER, AEGDataBlock AEGIN, AEGDataBlock &AEGOUT, double DESLAM, int &K, int INDVEC)
{
	//DESLAM: Desired longitude in radians
	//INDVEC: For Earth only. <0 for ECT, >= 0 for ECI

	//K: -1 = Unrecoverable AEG error, >0 = failure to converge, 0 = no error

	MATRIX3 Rot;
	double i_CB, g_CB, h_CB, u_CB, g_dot, h_dot, Z, DELTA, lambda_V, lambda_apo, w_CB, dlambda, DELTADOT, dt, u_CB_dot, mu_CB;

	if (HEADER.AEGInd == BODY_EARTH)
	{
		w_CB = OrbMech::w_Earth;
		mu_CB = OrbMech::mu_Earth;
	}
	else
	{
		w_CB = OrbMech::w_Moon;
		mu_CB = OrbMech::mu_Moon;
	}
	lambda_apo = 0.0;
	K = 1;
	//Move input to output
	AEGOUT = AEGIN;
	do
	{
		if (HEADER.AEGInd == BODY_EARTH)
		{
			if (INDVEC < 0)
			{
				lambda_apo = 0.0;
			}
			else
			{
				if (K == 1)
				{
					lambda_apo = SystemParameters.MCLAMD;
				}
			}
			i_CB = AEGOUT.coe_osc.i;
			g_CB = AEGOUT.coe_osc.g;
			h_CB = AEGOUT.coe_osc.h;
			u_CB = AEGOUT.U;
			g_dot = AEGOUT.g_dot;
			h_dot = AEGOUT.h_dot;
			Z = 1.0;
		}
		else
		{
			VECTOR3 P, W, P_apo, W_apo;
			double i_SG, g_SG, h_SG;

			PLEFEM(5, AEGOUT.TS / 3600.0, 0, NULL, NULL, NULL, &Rot);
			PIVECT(AEGOUT.coe_osc.i, AEGOUT.coe_osc.g, AEGOUT.coe_osc.h, P, W);
			P_apo = mul(Rot, P);
			W_apo = mul(Rot, W);
			PIVECT(P_apo, W_apo, i_SG, g_SG, h_SG);
			i_CB = i_SG;
			g_CB = g_SG;
			h_CB = h_SG;
			u_CB = AEGOUT.f + g_CB;
			if (u_CB >= PI2)
			{
				u_CB -= PI2;
			}
			g_dot = AEGIN.g_dot;
			h_dot = AEGIN.h_dot;
			Z = 0.0;
		}
		DELTA = atan2(sin(u_CB)*cos(i_CB), cos(u_CB));
		if (DELTA < 0)
		{
			DELTA += PI2;
		}
		lambda_V = h_CB + DELTA - Z * (lambda_apo + w_CB * AEGOUT.TE);
		lambda_V = fmod(lambda_V, PI2);
		if (lambda_V < 0)
		{
			lambda_V += PI2;
		}
		dlambda = DESLAM - lambda_V;
		if (K <= 1)
		{
			DELTADOT = AEGIN.l_dot + g_dot;
			if (i_CB > PI05)
			{
				DELTADOT = -DELTADOT;
				if (dlambda > 0)
				{
					dlambda = dlambda - PI2;
				}
			}
			else
			{
				if (dlambda < 0)
				{
					dlambda = dlambda + PI2;
				}
			}
		}
		else
		{
			u_CB_dot = sqrt(mu_CB*AEGOUT.coe_osc.a*(1.0 - pow(AEGOUT.coe_osc.e, 2))) / pow(AEGOUT.R, 2) + g_dot;
			DELTADOT = cos(i_CB)*u_CB_dot / (pow(cos(u_CB), 2) + pow(sin(u_CB), 2)*pow(cos(i_CB), 2));
			if (abs(dlambda) > PI)
			{
				if (dlambda > 0)
				{
					dlambda = dlambda - PI2;
				}
				else
				{
					dlambda = dlambda + PI2;
				}
			}
		}
		dt = dlambda / (h_dot + DELTADOT - w_CB);
		if (abs(dt) <= 0.01)
		{
			K = 0;
			return;
		}
		if (K > 5)
		{
			return;
		}
		K++;
		AEGIN.TE = AEGOUT.TE + dt;
		AEGIN.TIMA = 0;
		PMMAEGS(HEADER, AEGIN, AEGOUT);
		if (HEADER.ErrorInd)
		{
			K = -1;
			return;
		}
	} while (K < 6);
}

void RTCC::PMMDAN(AEGHeader Header, AEGDataBlock aeg, int IND, int &ERR, double &T_c, double &T_c_apo)
{
	AEGDataBlock sv_temp;
	VECTOR3 R_EM, V_EM, R_ES, R_S, R, V, H, N, N_apo;
	double MJD, r_S, mu, cos_theta, R_e, r, phi1, phi2, phi3, n, cos_phi1, sin_alpha, h, cos_eta, sin_eta, F, dt, S_T;
	int J, I_c;
	bool daylight;

	ERR = 0;
	J = 0;
	MJD = SystemParameters.GMTBASE + aeg.TS / 24.0 / 3600.0;
	PLEFEM(1, aeg.TS / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL);

	if (Header.AEGInd == BODY_EARTH)
	{
		R_S = R_ES;
		mu = OrbMech::mu_Earth;
		R_e = OrbMech::R_Earth;
	}
	else
	{
		R_S = R_ES - R_EM;
		mu = OrbMech::mu_Moon;
		R_e = BZLAND.rad[RTCC_LMPOS_BEST];
	}

	//Initialize
	aeg.TIMA = 0;
	aeg.TE = aeg.TS;
	PMMAEGS(Header, aeg, sv_temp);

	if (Header.ErrorInd != 0)
	{
		goto RTCC_PMMDAN_4_3;
	}
	
	OrbMech::GIMKIC(aeg.coe_osc, mu, R, V);
	r = length(R);
	r_S = length(R_ES);
	cos_theta = dotp(R, R_S) / r / r_S;
	if (cos_theta >= 0)
	{
		//Vehicle is in daylight
		daylight = true;
	}
	else if (r*sqrt(1.0 - cos_theta * cos_theta) >= R_e)
	{
		//Vehicle is in daylight
		daylight = true;
	}
	else
	{
		//Vehicle is in darkness
		daylight = false;
	}
	I_c = 0;
	T_c = sv_temp.TE;
RTCC_PMMDAN_2_2:
	if (I_c > 0)
	{
		OrbMech::GIMKIC(sv_temp.coe_osc, mu, R, V);
		r = length(R);
	}
	H = crossp(R, V);
	N = crossp(R_S, H);
	n = length(N);
	if (daylight)
	{
		n = -n;
	}
	N_apo = N / n;
	cos_phi1 = dotp(R, N_apo) / r;
	phi1 = acos(cos_phi1);
	sin_alpha = sqrt(1.0 - pow(R_e / r, 2));
	h = length(H);
	cos_eta = dotp(H, R_S) / h / r_S;
	sin_eta = sqrt(1.0 - cos_eta * cos_eta);
	if (sin_eta <= sin_alpha)
	{
		ERR = 1;
		goto RTCC_PMMDAN_4_3;
	}
	phi2 = asin(sin_alpha / sin_eta);
	F = R.x*N_apo.y - R.y*N_apo.x;
	if (Header.AEGInd == BODY_MOON)
	{
		F = -F;
	}
	if (daylight)
	{
		if (F >= 0)
		{
			phi3 = phi1 + phi2;
		}
		else
		{
			if (cos_phi1 > 0)
			{
				phi3 = phi2 - phi1;
			}
			else
			{
				phi3 = PI2 - phi1 + phi2;
			}
		}
	}
	else
	{
		if (F > 0)
		{
			phi3 = phi1 - phi2;
		}
		else
		{
			phi3 = -phi1 - phi2;
		}
	}
	dt = phi3 / (aeg.l_dot + aeg.g_dot);
	T_c = T_c + dt;
	if (abs(dt) > 0.00055*3600.0)
	{
		if (I_c >= 4)
		{
			ERR = 2;
			goto RTCC_PMMDAN_4_3;
		}
		I_c++;
	RTCC_PMMDAN_2_4:
		aeg.TE = T_c;
		PMMAEGS(Header, aeg, sv_temp);
		
		if (Header.ErrorInd != 0)
		{
			goto RTCC_PMMDAN_4_3;
		}
		goto RTCC_PMMDAN_2_2;
	}
	if (daylight)
	{
		T_c = -T_c;
	}
	if (IND == 1)
	{
		T_c_apo = 0.0;
		return;
	}
	if (J <= 0)
	{
		S_T = T_c;
		daylight = !daylight;
		if (daylight == false)
		{
			phi2 = -phi2;
		}
		phi3 = PI + 2.0*phi2;
		dt = phi3 / (aeg.l_dot + aeg.g_dot);
		T_c = abs(T_c) + dt;
		J = 1;
		I_c = 1;
		goto RTCC_PMMDAN_2_4;
	}
	else
	{
		T_c_apo = T_c;
		T_c = S_T;
		return;
	}

RTCC_PMMDAN_4_3:
	T_c = 0.0;
	T_c_apo = 0.0;
	return;
}

void RTCC::EMDCHECK(int veh, int opt, double param, double THTime, int ref, bool feet)
{
	EphemerisData sv_out, sv_conv, sv_inert;
	MissionPlanTable *table;
	OrbitEphemerisTable *ephem;
	ELVCTRInputTable interin;
	ELVCTROutputTable2 interout;

	EZCHECKDIS.ErrorMessage = "";

	if (veh == RTCC_MPT_LM)
	{
		table = &PZMPTLEM;
		ephem = &EZEPH2;
	}
	else
	{
		table = &PZMPTCSM;
		ephem = &EZEPH1;
	}

	if (table->CommonBlock.TUP == 0)
	{
		EZCHECKDIS.ErrorMessage = "Error 8";
		return;
	}
	else if (table->CommonBlock.TUP < 0)
	{
		EZCHECKDIS.ErrorMessage = "Error 2";
		return;
	}

	if (table->CommonBlock.TUP != ephem->EPHEM.Header.TUP)
	{
		EZCHECKDIS.ErrorMessage = "Error 3";
		return;
	}

	EZCHECKDIS.U_T = _V(-2, 0, 0);
	EZCHECKDIS.NV = 0;
	if (THTime > 0)
	{
		EZCHECKDIS.THT = THTime;
	}
	else
	{
		EZCHECKDIS.THT = 0.0;
	}
	EZCHECKDIS.K_Fac = table->KFactor;

	//Set option
	switch (opt)
	{
	case 1:
		sprintf(EZCHECKDIS.Option, "GMT");
		break;
	case 2:
		sprintf(EZCHECKDIS.Option, "GET");
		break;
	case 3:
		sprintf(EZCHECKDIS.Option, "MVI");
		break;
	case 4:
		sprintf(EZCHECKDIS.Option, "MVE");
		break;
	case 5:
		sprintf(EZCHECKDIS.Option, "RAD");
		break;
	case 6:
		sprintf(EZCHECKDIS.Option, "ALT");
		break;
	case 7:
		sprintf(EZCHECKDIS.Option, "FPA");
		break;
	}

	//All options except MVI and MVE
	if (opt == 1 || opt == 2 || opt == 5 || opt == 6 || opt == 7)
	{
		EphemerisData svtemp;
		double THGMT;
		bool hasThreshold = true;

		//Determine threshold time
		if (opt == 1 || opt == 2)
		{
			if (THTime > 0)
			{
				if (opt == 1)
				{
					THGMT = THTime;
				}
				else
				{
					THGMT = GMTfromGET(THTime);
				}
			}
			else
			{
				hasThreshold = false;

				if (opt == 1)
				{
					THGMT = param;
					
				}
				else if (opt == 2)
				{
					THGMT = GMTfromGET(param);
				}
			}
		}
		else
		{
			//Other options require threshold
			THGMT = GMTfromGET(THTime);
		}

		//Check ephemeris bounds. hasThreshold
		bool inbounds;
		if (THGMT >= ephem->EPHEM.Header.TL && THGMT <= ephem->EPHEM.Header.TR)
		{
			inbounds = true;
		}
		else
		{
			inbounds = false;
		}

		if (inbounds)
		{
			//If requested time is in the ephemeris span, interpolate
			interin.GMT = THGMT;
			interin.L = veh;
			ELVCTR(interin, interout);
			if (interout.ErrorCode > 2)
			{
				//Error: Input time is outside of ephemeris range
				EZCHECKDIS.ErrorMessage = "Error 4";
				return;
			}
			if (interout.VPI == 1)
			{
				//Error: Input time is during a maneuver
				EZCHECKDIS.ErrorMessage = "Error 5";
				return;
			}
			svtemp = RotateSVToSOI(interout.SV);

			EZCHECKDIS.NV = interout.ORER + 1;
		}
		else
		{
			//Otherwise, get closest free flight vector
			if (THGMT < ephem->EPHEM.Header.TL)
			{
				//Error: Input time is outside of ephemeris range
				EZCHECKDIS.ErrorMessage = "Error 4";
				return;
			}
			else
			{
				svtemp = RotateSVToSOI(ephem->EPHEM.table.back());
			}
		}

		//If no threshold and within ephemeris span, we already have our vector
		if (hasThreshold == false && inbounds)
		{
			sv_out = svtemp;
		}
		else
		{
			//Process further
			//Set up EMSMISS

			EMSMISSInputTable intab;

			intab.AnchorVector = svtemp;
			intab.landed = (svtemp.GMT >= ephem->LUNRSTAY.LunarStayBeginGMT) && (svtemp.GMT <= ephem->LUNRSTAY.LunarStayEndGMT);
			intab.DensityMultiplier = table->KFactor;
			intab.VehicleCode = veh;
			intab.DensityMultOverrideIndicator = true;
			intab.ManeuverIndicator = false;

			if (opt == 1 || opt == 2)
			{
				double GMT;

				if (opt == 1)
				{
					GMT = param;
				}
				else
				{
					GMT = GMTfromGET(param);
				}

				intab.MaxIntegTime = abs(GMT - svtemp.GMT);
				if (GMT - svtemp.GMT >= 0.0)
				{
					intab.IsForwardIntegration = 1.0;
				}
				else
				{
					intab.IsForwardIntegration = -1.0;
				}
			}
			else
			{
				if (inbounds == false)
				{
					//Not time options, state vector not from interpolation. So we still need to get to the threshold time

					intab.MaxIntegTime = abs(THGMT - svtemp.GMT);
					intab.CutoffIndicator = 1;
					if (THGMT - svtemp.GMT >= 0.0)
					{
						intab.IsForwardIntegration = 1.0;
					}
					else
					{
						intab.IsForwardIntegration = -1.0;
					}

					//Propagate
					EMSMISS(&intab);

					//Check for errors
					if (intab.CutoffIndicator != intab.NIAuxOutputTable.TerminationCode)
					{
						EZCHECKDIS.ErrorMessage = "Error 10";
						return;
					}

					svtemp = intab.NIAuxOutputTable.sv_cutoff;

					//Reset for forward search
					intab.IsForwardIntegration = 1.0;
					intab.AnchorVector = svtemp;
					intab.MaxIntegTime = 10.0*24.0*3600.0;
				}
			}

			if (opt == 5)
			{
				intab.CutoffIndicator = 2;
			}
			else if (opt == 6)
			{
				intab.CutoffIndicator = 3;
			}
			else if (opt == 7)
			{
				intab.CutoffIndicator = 4;
			}
			else
			{
				intab.CutoffIndicator = 1;
			}

			if (ref <= 1)
			{
				intab.StopParamRefFrame = 0;
				intab.EarthRelStopParam = param;
			}
			else
			{
				intab.StopParamRefFrame = 1;
				intab.MoonRelStopParam = param;
			}

			//Propagate
			EMSMISS(&intab);

			//Check for errors
			if (intab.CutoffIndicator != intab.NIAuxOutputTable.TerminationCode)
			{
				EZCHECKDIS.ErrorMessage = "Error 10";
				return;
			}
			//We have our vector
			sv_out = intab.NIAuxOutputTable.sv_cutoff;
		}
	}
	//Maneuver Initiation
	else if (opt == 3)
	{
		unsigned man = (unsigned)param;
		if (man <= 0 || table->mantable.size() < man)
		{
			EZCHECKDIS.ErrorMessage = "Error 7";
			return;
		}
		sv_out.R = table->mantable[man - 1].R_BI;
		sv_out.V = table->mantable[man - 1].V_BI;
		sv_out.GMT = table->mantable[man - 1].GMT_BI;
		sv_out.RBI = table->mantable[man - 1].RefBodyInd;

		EZCHECKDIS.U_T = table->mantable[man - 1].A_T;
	}
	//Maneuver cutoff
	else if (opt == 4)
	{
		unsigned man = (unsigned)param;

		if (man <= 0 || table->mantable.size() < man)
		{
			EZCHECKDIS.ErrorMessage = "Error 7";
			return;
		}
		sv_out.R = table->mantable[man - 1].R_BO;
		sv_out.V = table->mantable[man - 1].V_BO;
		sv_out.GMT = table->mantable[man - 1].GMT_BO;
		sv_out.RBI = table->mantable[man - 1].RefBodyInd;
	}

	if (veh == RTCC_MPT_LM)
	{
		sprintf_s(EZCHECKDIS.VEH, "LM");
	}
	else
	{
		sprintf_s(EZCHECKDIS.VEH, "CSM");
	}
	sprintf_s(EZCHECKDIS.VID, "%s", table->StationID.c_str());
	EZCHECKDIS.GET = GETfromGMT(sv_out.GMT);
	EZCHECKDIS.GMT = sv_out.GMT;

	sv_inert = sv_out;

	ELVCNV(sv_inert, ref, sv_conv);

	if (ref == 0)
	{
		sprintf(EZCHECKDIS.RF, "ECI");
	}
	else if (ref == 1)
	{
		sprintf(EZCHECKDIS.RF, "ECT");
	}
	else if (ref == 2)
	{
		sprintf(EZCHECKDIS.RF, "MCI");
	}
	else
	{
		sprintf(EZCHECKDIS.RF, "MCT");
	}

	//Display unit
	double CONVFACR, CONVFACV, R_E, mu;

	EZCHECKDIS.unit = feet;
	if (feet == 1)
	{
		CONVFACR = CONVFACV = 0.3048;
	}
	else
	{
		CONVFACR = SystemParameters.MCCMCU;
		CONVFACV = SystemParameters.MCCMCU / 3600.0;
	}

	if (ref < 2)
	{
		R_E = SystemParameters.MCECAP;
		mu = OrbMech::mu_Earth;
	}
	else
	{
		R_E = BZLAND.rad[RTCC_LMPOS_BEST];
		mu = OrbMech::mu_Moon;
	}

	EZCHECKDIS.Pos = sv_conv.R / CONVFACR;
	EZCHECKDIS.Vel = sv_conv.V / CONVFACV;

	double rmag, vmag, rtasc, decl, fpav, az;
	OrbMech::rv_from_adbar(sv_conv.R, sv_conv.V, rmag, vmag, rtasc, decl, fpav, az);

	EZCHECKDIS.V_i = vmag / 0.3048;
	EZCHECKDIS.gamma_i = 90.0 - fpav * DEG;
	EZCHECKDIS.psi = az * DEG;
	EZCHECKDIS.phi_c = 0.0;
	EZCHECKDIS.lambda = 0.0;
	EZCHECKDIS.h_s = (rmag - R_E) / 1852.0;
	EZCHECKDIS.R = rmag / 1852.0;

	if (ref < 2)
	{
		EZCHECKDIS.HOBlank = false;
		EZCHECKDIS.h_o_ft = (rmag - OrbMech::R_Earth) / 0.3048;
		if (EZCHECKDIS.h_o_ft > 9999999.0) EZCHECKDIS.h_o_ft = 9999999.0;
		EZCHECKDIS.h_o_NM = (rmag - OrbMech::R_Earth) / 1852.0;
		if (EZCHECKDIS.h_o_NM > 9999.99) EZCHECKDIS.h_o_NM = 9999.99;
	}
	else
	{
		EZCHECKDIS.HOBlank = true;
		EZCHECKDIS.h_o_ft = EZCHECKDIS.h_o_NM = 0.0;
	}

	OELEMENTS coe;
	coe = OrbMech::coe_from_sv(sv_conv.R, sv_conv.V, mu);

	if (coe.e >= 1)
	{
		EZCHECKDIS.a = 99999.99;
	}
	else
	{
		EZCHECKDIS.a = coe.h*coe.h / mu / (1.0 - coe.e*coe.e) / 1852.0;
	}
	if (EZCHECKDIS.a > 99999.99) EZCHECKDIS.a = 99999.99;
	EZCHECKDIS.e = coe.e;
	EZCHECKDIS.i = coe.i*DEG;
	EZCHECKDIS.omega_p = coe.w*DEG;
	EZCHECKDIS.Omega = coe.RA*DEG;
	EZCHECKDIS.nu = coe.TA*DEG;
	if (coe.e < 0.00001)
	{
		EZCHECKDIS.TABlank = true;
	}
	else
	{
		EZCHECKDIS.TABlank = false;
	}
	if (coe.e < 1.0)
	{
		EZCHECKDIS.MABlank = false;
		EZCHECKDIS.m = OrbMech::TrueToMeanAnomaly(coe.TA, coe.e)*DEG;
	}
	else
	{
		EZCHECKDIS.m = 0.0;
		EZCHECKDIS.MABlank = true;
	}

	if (coe.e < 0.85)
	{
		AEGBlock aeg;
		AEGDataBlock sv_a, sv_p;
		double INFO[10];

		aeg = SVToAEG(sv_inert, 0.0, 1.0, 1.0); //TBD

		PMMAPD(aeg.Header, aeg.Data, 0, 0, INFO, &sv_a, &sv_p);

		EZCHECKDIS.h_a = INFO[4] / 1852.0;
		EZCHECKDIS.h_p = INFO[9] / 1852.0;
	}
	else
	{
		EZCHECKDIS.h_p = pow(length(crossp(sv_inert.R, sv_inert.V)), 2) / (mu*(1.0 + coe.e)) - R_E;
		EZCHECKDIS.h_p = EZCHECKDIS.h_p / 1852.0;

		if (coe.e >= 1)
		{
			EZCHECKDIS.h_a = 9999.99;
		}
		else
		{
			EZCHECKDIS.h_a = pow(length(crossp(sv_inert.R, sv_inert.V)), 2) / (mu*(1.0 - coe.e)) - R_E;
			EZCHECKDIS.h_a = EZCHECKDIS.h_a / 1852.0;
		}
	}

	if (EZCHECKDIS.h_p > 9999.99) EZCHECKDIS.h_p = 9999.99;
	if (EZCHECKDIS.h_a > 9999.99) EZCHECKDIS.h_a = 9999.99;

	double lat, lng;
	OrbMech::latlong_from_r(sv_conv.R, lat, lng);
	EZCHECKDIS.phi_c = EZCHECKDIS.phi_D = lat;
	EZCHECKDIS.lambda = EZCHECKDIS.lambda_D = lng;

	if (ref == 0)
	{
		EZCHECKDIS.lambda_D -= SystemParameters.MCLAMD + sv_out.GMT*OrbMech::w_Earth;
	}
	else if (ref == 1)
	{
		EZCHECKDIS.lambda_D -= sv_out.GMT*OrbMech::w_Earth;
	}
	OrbMech::normalizeAngle(EZCHECKDIS.lambda);
	OrbMech::normalizeAngle(EZCHECKDIS.lambda_D);

	EZCHECKDIS.phi_c *= DEG;
	EZCHECKDIS.phi_D *= DEG;
	EZCHECKDIS.lambda *= DEG;
	EZCHECKDIS.lambda_D *= DEG;

	VECTOR3 R_EM;
	double LunarDeclination;

	//Convert a vector at the center of the Moon to ECI coordinates, thereby getting a lunar position vector in ECI
	int err = ELVCNV(_V(0, 0, 0), sv_inert.GMT, 1, 2, 0, R_EM);

	if (err)
	{
		LunarDeclination = 0.0;
	}
	else
	{
		LunarDeclination = asin(unit(R_EM).z);
	}
	EZCHECKDIS.deltaL = LunarDeclination * DEG;

	EZCHECKDIS.UpdateNo = table->CommonBlock.TUP;
	if (ephem->EPHEM.table.size() > 0)
	{
		EZCHECKDIS.EPHB = ephem->EPHEM.table.front().GMT;
		EZCHECKDIS.EPHE = ephem->EPHEM.table.back().GMT;
	}
	else
	{
		EZCHECKDIS.EPHB = 0.0;
		EZCHECKDIS.EPHE = 0.0;
	}
	EZCHECKDIS.LOC = SystemParameters.MCGMTL * 3600.0;
	EZCHECKDIS.GRRC = SystemParameters.MCGRAG * 3600.0;
	EZCHECKDIS.ZSC = SystemParameters.MCGZSA * 3600.0;
	EZCHECKDIS.GRRS = SystemParameters.MCGRIC * 3600.0;
	EZCHECKDIS.LOL = SystemParameters.MCGMTS * 3600.0;
	EZCHECKDIS.ZSL = SystemParameters.MCGZSL * 3600.0;
	EZCHECKDIS.ZSA = SystemParameters.MCGZSS * 3600.0;
	EZCHECKDIS.R_Day[0] = GZGENCSN.DayofLiftoff;
	EZCHECKDIS.R_Day[1] = GZGENCSN.MonthofLiftoff;
	EZCHECKDIS.R_Day[2] = GZGENCSN.Year;

	if (veh == RTCC_MPT_LM)
	{
		if (ephem->LUNRSTAY.LunarStayBeginGMT < 0)
		{
			EZCHECKDIS.LAL = 0.0;
		}
		else
		{
			EZCHECKDIS.LAL = ephem->LUNRSTAY.LunarStayBeginGMT;
		}

		if (ephem->LUNRSTAY.LunarStayEndGMT > 0 && ephem->LUNRSTAY.LunarStayEndGMT < 10e10)
		{
			EZCHECKDIS.LOL = ephem->LUNRSTAY.LunarStayEndGMT;
		}
		else
		{
			EZCHECKDIS.LOL = 0.0;
		}

		EZCHECKDIS.LSTBlank = false;
	}
	else
	{
		EZCHECKDIS.LAL = 0.0;
		EZCHECKDIS.LOL = 0.0;
		EZCHECKDIS.LSTBlank = true;
	}

	PLAWDTInput pin;
	PLAWDTOutput pout;
	pin.T_UP = sv_conv.GMT;
	pin.TableCode = veh;
	pin.VentingOpt = true;
	PLAWDT(pin, pout);

	MPTGetStringFromConfig(pout.CC, EZCHECKDIS.CFG);

	EZCHECKDIS.WT = pout.ConfigWeight / 0.45359237;
	EZCHECKDIS.WC = pout.CSMWeight / 0.45359237;
	EZCHECKDIS.WL = (pout.LMAscWeight + pout.LMDscWeight) / 0.45359237;
	EZCHECKDIS.A = pout.ConfigArea / 0.3048 / 0.3048;

	unsigned ii = 0;

	while (ii < table->ManeuverNum && sv_inert.GMT >= table->mantable[ii].GMT_BO)
	{
		ii++;
	}

	MPTVehicleDataBlock CommonBlock;
	if (ii == 0)
	{
		CommonBlock = table->CommonBlock;
	}
	else
	{
		CommonBlock = table->mantable[ii - 1].CommonBlock;
	}

	EZCHECKDIS.SPS = CommonBlock.SPSFuelRemaining / 0.45359237;
	EZCHECKDIS.DPS = CommonBlock.LMDPSFuelRemaining / 0.45359237;
	EZCHECKDIS.APS = CommonBlock.LMAPSFuelRemaining / 0.45359237;
	EZCHECKDIS.RCS_C = CommonBlock.CSMRCSFuelRemaining / 0.45359237;
	EZCHECKDIS.RCS_L = CommonBlock.LMRCSFuelRemaining / 0.45359237;
	EZCHECKDIS.J2 = CommonBlock.SIVBFuelRemaining / 0.45359237;

	return;
}

bool RTCC::MPTConfigSubset(const std::bitset<4> &CfgOld, const std::bitset<4> &CfgNew)
{
	//Can't be identical
	if (CfgOld == CfgNew)
	{
		return false;
	}
	//Can't be empty
	if (CfgNew.to_ulong() == 0)
	{
		return false;
	}
	for (unsigned i = 0;i < 4;i++)
	{
		//Does CfgNew have something that CfgOld didn't?
		if (CfgNew[i] == true && CfgOld[i] == false)
		{
			//Can't have that
			return false;
		}
	}

	//Otherwise ok
	return true;
}

void RTCC::MPTGetStringFromConfig(const std::bitset<4> &cfg, char *str)
{
	unsigned cfgint = cfg.to_ulong();

	switch (cfgint)
	{
	case 1:
		sprintf(str, "C");
		break;
	case 2:
		sprintf(str, "S");
		break;
	case 3:
		sprintf(str, "CS");
		break;
	case 4:
		sprintf(str, "A");
		break;
	case 5:
		sprintf(str, "CA");
		break;
	case 6:
		sprintf(str, "SA");
		break;
	case 7:
		sprintf(str, "CSA");
		break;
	case 8:
		sprintf(str, "D");
		break;
	case 9:
		sprintf(str, "CD");
		break;
	case 10:
		sprintf(str, "SD");
		break;
	case 11:
		sprintf(str, "CSD");
		break;
	case 12:
		sprintf(str, "L");
		break;
	case 13:
		sprintf(str, "CL");
		break;
	case 14:
		sprintf(str, "SL");
		break;
	case 15:
		sprintf(str, "CSL");
		break;
	default:
		sprintf(str, " ");
		break;
	}
}

void RTCC::MPTGetConfigFromString(const std::string &str, std::bitset<4> &cfg)
{
	cfg.reset();
	for (unsigned i = 0;i < str.length();i++)
	{
		if (str[i] == 'C')
		{
			cfg[RTCC_CONFIG_C] = true;
		}
		else if (str[i] == 'S')
		{
			cfg[RTCC_CONFIG_S] = true;
		}
		else if (str[i] == 'A')
		{
			cfg[RTCC_CONFIG_A] = true;
		}
		else if (str[i] == 'D')
		{
			cfg[RTCC_CONFIG_D] = true;
		}
		else if (str[i] == 'L')
		{
			cfg[RTCC_CONFIG_A] = true;
			cfg[RTCC_CONFIG_D] = true;
		}
	}
}

bool RTCC::MPTIsRCSThruster(int thruster)
{
	if (thruster == RTCC_ENGINETYPE_CSMRCSMINUS2 || thruster == RTCC_ENGINETYPE_CSMRCSMINUS4 || thruster == RTCC_ENGINETYPE_CSMRCSPLUS2 || thruster == RTCC_ENGINETYPE_CSMRCSPLUS4)
	{
		return true;
	}
	if (thruster == RTCC_ENGINETYPE_LMRCSMINUS2 || thruster == RTCC_ENGINETYPE_LMRCSMINUS4 || thruster == RTCC_ENGINETYPE_LMRCSPLUS2 || thruster == RTCC_ENGINETYPE_LMRCSPLUS4)
	{
		return true;
	}

	return false;
}

bool RTCC::MPTIsUllageThruster(int thruster, int i)
{
	switch (thruster)
	{
	case RTCC_ENGINETYPE_CSMSPS:
		if (i == 0)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_LMAPS:
		if (i == 3)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_LMDPS:
		if (i == 3)
		{
			return true;
		}
		break;
	}

	return false;
}

int RTCC::MPTGetPrimaryThruster(int thruster)
{
	switch (thruster)
	{
	case RTCC_ENGINETYPE_CSMSPS:
		return 1;
	case RTCC_ENGINETYPE_SIVB_MAIN:
		return 2;
	case RTCC_ENGINETYPE_LMRCSPLUS2:
	case RTCC_ENGINETYPE_LMRCSPLUS4:
	case RTCC_ENGINETYPE_LMRCSMINUS2:
	case RTCC_ENGINETYPE_LMRCSMINUS4:
		return 3;
	case RTCC_ENGINETYPE_LMAPS:
		return 4;
	case RTCC_ENGINETYPE_LMDPS:
		return 5;
	}

	return 0;
}

bool RTCC::MPTIsPrimaryThruster(int thruster, int i)
{
	switch (thruster)
	{
	case RTCC_ENGINETYPE_CSMRCSPLUS2:
	case RTCC_ENGINETYPE_CSMRCSPLUS4:
	case RTCC_ENGINETYPE_CSMRCSMINUS2:
	case RTCC_ENGINETYPE_CSMRCSMINUS4:
		if (i == 0)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_CSMSPS:
		if (i == 1)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_SIVB_MAIN:
		if (i == 2)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_LMRCSPLUS2:
	case RTCC_ENGINETYPE_LMRCSPLUS4:
	case RTCC_ENGINETYPE_LMRCSMINUS2:
	case RTCC_ENGINETYPE_LMRCSMINUS4:
		if (i == 3)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_LMAPS:
		if (i == 4)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_LMDPS:
		if (i == 5)
		{
			return true;
		}
		break;
	}

	return false;
}

MissionPlanTable* RTCC::GetMPTPointer(int L)
{
	if (L == RTCC_MPT_CSM)
	{
		return &PZMPTCSM;
	}
	else
	{
		return &PZMPTLEM;
	}
}

OBJHANDLE RTCC::GetGravref(int body)
{
	if (body == BODY_EARTH)
	{
		return hEarth;
	}

	return hMoon;
}

int RTCC::PMMXFR(int id, void *data)
{
	MissionPlanTable *mpt;
	//MPTManeuver *maneuver;
	double LowerLimit, UpperLimit, VectorFetchTime;
	int err, working_man, TVC;
	std::bitset<4> CCP, CC, CCMI;
	unsigned CurMan;
	bool LastManReplaceFlag;
	bool replace, dodelete;
	std::string code;

	//Save Logic
	//Setup input table for PMSVCT
	//Move MED inputs to proper location
	//Initialize flags
	working_man = 1;
	//Direct inputs (RTE, MPT, TLI)
	if (id == 32 || id == 33 || id == 37)
	{
		PMMXFRDirectInput *inp = static_cast<PMMXFRDirectInput*>(data);
		std::string purpose;
		double BurnParm72, BurnParm74, BurnParm75, BurnParm76, BurnParm77;
		int BurnParm73;
		int BPIND;

		//RTE
		if (id == 32)
		{
			if (inp->BurnParameterNumber == 1 || inp->BurnParameterNumber == 3)
			{
				RetrofireTransferTableEntry *temptab;

				if (inp->BurnParameterNumber == 1)
				{
					temptab = &RZRFTT.Primary;
				}
				else
				{
					temptab = &RZRFTT.Manual;
				}

				purpose = "TTF";
				inp->GMTI = temptab->GMTI;
				inp->ThrusterCode = temptab->Thruster;
				inp->dt_ullage = temptab->dt_ullage;
				inp->UllageThrusterOption = temptab->UllageThrusterOption;
				inp->HeadsUpDownIndicator = true; //TBD
				
				BurnParm75 = temptab->DeltaV.x;
				BurnParm76 = temptab->DeltaV.y;
				BurnParm77 = temptab->DeltaV.z;
			}
			else
			{
				int itemp;
				if (inp->BurnParameterNumber == 2)
				{
					itemp = 0;
				}
				else
				{
					itemp = 1;
				}

				RTEDigitalSolutionTable *rtetab = &PZREAP.RTEDTable[itemp];

				if (rtetab->ASTSolutionCode == "") return 1;

				purpose = "RTE";
				inp->GMTI = rtetab->GMTI;
				inp->ThrusterCode = rtetab->ThrusterCode;
				inp->dt_ullage = rtetab->dt_ullage;
				inp->DT10P = rtetab->dt_10PCT;
				inp->HeadsUpDownIndicator = rtetab->HeadsUpDownIndicator;
				if (rtetab->NumQuads == 4)
				{
					inp->UllageThrusterOption = true;
				}
				else
				{
					inp->UllageThrusterOption = false;
				}
				inp->ConfigurationChangeIndicator = rtetab->ConfigurationChangeIndicator;
				inp->EndConfiguration = rtetab->EndConfiguration;
				
				BurnParm75 = rtetab->DV_XDV.x;
				BurnParm76 = rtetab->DV_XDV.y;
				BurnParm77 = rtetab->DV_XDV.z;
			}
			BPIND = 2;
		}
		//Direct Input
		else if (id == 33)
		{
			purpose = "MAN";
			//Move in burn parameters from M40 MED
			if (inp->BurnParameterNumber == 1)
			{
				BurnParm72 = PZBURN.P1_DV;
				BurnParm73 = PZBURN.P1_DVIND;
				BurnParm74 = PZBURN.P1_DT;
			}
			else if (inp->BurnParameterNumber == 2)
			{
				BurnParm75 = PZBURN.P2_DV.x;
				BurnParm76 = PZBURN.P2_DV.y;
				BurnParm77 = PZBURN.P2_DV.z;
			}
			BPIND = inp->BurnParameterNumber;
		}
		//TLI
		else if (id == 37)
		{
			purpose = "TLI";

			PMMSPTInput tliin;

			tliin.QUEID = id;
			tliin.PresentGMT = RTCCPresentTimeGMT();
			tliin.ReplaceCode = 0;
			tliin.InjOpp = inp->BurnParameterNumber;
			tliin.Table = inp->TableCode;

			if (err = PMMSPT(tliin))
			{
				PMXSPT("PMMSPT", err);
				return 1;
			}
			inp->GMTI = tliin.T_RP;
			inp->ThrusterCode = RTCC_ENGINETYPE_SIVB_MAIN;
			inp->AttitudeCode = RTCC_ATTITUDE_SIVB_IGM;
			inp->ConfigurationChangeIndicator = RTCC_CONFIGCHANGE_NONE;
		}

		EphemerisData sv;

		if (inp->ReplaceCode > 0)
		{
			replace = true;
		}
		else
		{
			replace = false;
		}

		//Page 17
		//MPT header
		mpt = GetMPTPointer(inp->TableCode);
		//Check ground rules
		VectorFetchTime = inp->GMTI;
		err = PMMXFRGroundRules(mpt, inp->GMTI, inp->ReplaceCode, LastManReplaceFlag, LowerLimit, UpperLimit, CurMan, VectorFetchTime);
		if (err)
		{
			return 1;
		}
		//Format maneuver code
		err = PMMXFRFormatManeuverCode(inp->TableCode, inp->ThrusterCode, inp->AttitudeCode, CurMan, purpose, TVC, code);
		if (err)
		{
			return 1;
		}
		//Check configuration and thrust
		if (CurMan == 1)
		{
			CCP = mpt->CommonBlock.ConfigCode;
		}
		else
		{
			CCP = mpt->mantable.back().CommonBlock.ConfigCode;
		}
		CC = inp->EndConfiguration;
		err = PMMXFRCheckConfigThruster(true, inp->ConfigurationChangeIndicator, CCP, TVC, inp->ThrusterCode, CC, CCMI);
		if (err)
		{
			return 1;
		}
		if (replace && LastManReplaceFlag == false)
		{
			//TBD: Are config codes the same?
		}
		//TBD:  TLI maneuver with targets input
		if (PMMXFRFetchVector(VectorFetchTime, inp->TableCode, sv))
		{
			if (inp->TableCode == RTCC_MPT_CSM)
			{
				RTCCONLINEMON.TextBuffer[0] = "CSM";
			}
			else
			{
				RTCCONLINEMON.TextBuffer[0] = "LEM";
			}
			PMXSPT("PMMXFR", 39);
			return 39;
		}

		MPTManeuver man;
		man.code = code;

		//TBD: Is this a TLI maneuver?

		if (inp->AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
		{
			PMMSPTInput in;
			
			in.AttitudeMode = RTCC_ATTITUDE_SIVB_IGM;
			in.CC = CC.to_ulong();
			in.CCI = RTCC_CONFIGCHANGE_NONE;
			in.CCMI = CCMI.to_ulong();
			in.CurMan = &man;
			in.EndTimeLimit = UpperLimit;
			in.GMT = sv.GMT;
			in.InjOpp = inp->BurnParameterNumber;
			in.mpt = mpt;
			in.PresentGMT = RTCCPresentTimeGMT();
			if (mpt->mantable.size() == 0)
			{
				in.PrevMan = NULL;
			}
			else
			{
				in.PrevMan = &mpt->mantable.back();
			}
			in.QUEID = 39;
			in.R = sv.R;
			in.ReplaceCode = inp->ReplaceCode;
			in.StartTimeLimit = LowerLimit;
			in.Table = inp->TableCode;
			in.ThrusterCode = RTCC_ENGINETYPE_SIVB_MAIN;
			in.TVC = RTCC_MANVEHICLE_SIVB;
			in.T_RP = 0.0;
			in.V = sv.V;

			err = PMMSPT(in);
		}
		else
		{
			PMMMCDInput in;

			in.GMTI = inp->GMTI;
			in.Thruster = inp->ThrusterCode;
			in.dt_ullage = inp->dt_ullage;
			in.Attitude = inp->AttitudeCode;
			in.ConfigChangeInd = inp->ConfigurationChangeIndicator;
			in.ConfigCodeAfter = CC.to_ulong();
			in.TVC = TVC;
			in.DockingAngle = inp->DockingAngle;
			in.CCMI = CCMI;
			in.ID = id;
			in.UllageThrusterOpt = inp->UllageThrusterOption;
			in.DT_10PCT = inp->DT10P;
			in.DPSScaleFactor = inp->DPSScaleFactor;
			in.HeadsUpDownInd = inp->HeadsUpDownIndicator;
			in.CoordinateInd = inp->CoordinateIndicator;
			in.StartTimeLimit = LowerLimit;
			in.EndTimeLimit = UpperLimit;
			in.sv_anchor = sv;
			in.FrozenManeuver = false;
			in.CurrentManeuver = CurMan;
			in.mpt = mpt;
			in.BPIND = BPIND;
			in.BurnParm72 = BurnParm72;
			in.BurnParm73 = BurnParm73;
			in.BurnParm74 = BurnParm74;
			in.BurnParm75 = BurnParm75;
			in.BurnParm76 = BurnParm76;
			in.BurnParm77 = BurnParm77;
			in.Pitch = inp->Pitch;
			in.Yaw = inp->Yaw;
			in.Roll = inp->Roll;

			err = PMMMCD(in, man);
		}

		//PMMXFR_31_1
		if (err)
		{
			return 1;
		}
		
		//Either add a new maneuver or replace one
		if (CurMan > mpt->mantable.size())
		{
			mpt->mantable.push_back(man);
		}
		else
		{
			mpt->mantable[CurMan - 1] = man;
		}
		mpt->ManeuverNum = mpt->mantable.size();

		if (working_man == 1 && man.GMTI < mpt->UpcomingManeuverGMT)
		{
			PMSEXE(inp->TableCode, RTCCPresentTimeGMT());
		}

		PMSVCT(8, inp->TableCode);
	}
	//LOI/MCC, GPM, DKI, SPQ, Two-Impulse
	else if (id == 39 || id == 40 || id == 41 || id == 42)
	{
		std::string purpose;
		PMMMPTInput in;
		PMMXFR_Impulsive_Input *inp = static_cast<PMMXFR_Impulsive_Input*>(data);
		double GMTI;
		int num_man, plan;
		bool update_csm_eph = false, update_lm_eph = false;

		//Logic in MED decoder should prevent that both of these are true
		if (inp->ReplaceCode > 0)
		{
			replace = true;
		}
		else
		{
			replace = false;
		}
		if (inp->DeleteGMT > 0)
		{
			dodelete = true;
		}
		else
		{
			dodelete = false;
		}

		//DKI, SPQ and LDPP
		if (id == 41)
		{
			if (inp->Plan < 0)
			{
				num_man = 1;//PZLDPELM.num_man;
			}
			else if (inp->Plan == 0)
			{
				num_man = 1;//PZDKIT.Block[0].NumMan;
			}
			else
			{
				num_man = 1;//PZDKIT.Block[inp->Plan - 1].NumMan;
			}
		}
		else if (id == 42)
		{
			num_man = 2;
		}
		else
		{
			num_man = 1;
		}

		if (num_man < 1)
		{
			PMXSPT("PMMXFR", 38);
			return 38;
		}

	RTCC_PMMXFR_1:
		if (id == 39)
		{
			if (inp->Type == 0)
			{
				GMTI = PZMCCXFR.sv_man_bef[inp->Plan - 1].GMT;
				purpose = "MCC";
				plan = inp->Table;
			}
			else
			{
				GMTI = PZLRBELM.sv_man_bef[inp->Plan - 1].GMT;
				purpose = "LOI";
				plan = inp->Table;
			}
		}
		else if (id == 40)
		{
			GMTI = PZGPMELM.SV_before.GMT;
			purpose = PZGPMELM.code;
			plan = inp->Plan;
		}
		else if (id == 41)
		{
			if (inp->Plan < 0)
			{
				GMTI = PZLDPELM.sv_man_bef[0].GMT;
				purpose = PZLDPELM.code[0];
				plan = PZLDPELM.plan[0];
			}
			else
			{
				GMTI = PZDKIELM.Block[0].SV_before[0].GMT;
				purpose = PZDKIT.Block[0].Display[0].Man_ID;
				plan = PZDKIT.Block[0].Display[0].VEH;
			}
		}
		else if (id == 42)
		{
			GMTI = PZMYSAVE.SV_before[working_man - 1].GMT;
			purpose = PZMYSAVE.code[working_man - 1];
			plan = PZMYSAVE.plan[working_man - 1];
		}

		mpt = GetMPTPointer(plan);

		if (inp->DeleteGMT > 0 && working_man == 1)
		{
			//Delete all unfrozen maneuvers in this table after input time
			PMMXFRDeleteOption(plan, GMTI);
		}
		//Check ground rules
		err = PMMXFRGroundRules(mpt, GMTI, inp->ReplaceCode, LastManReplaceFlag, LowerLimit, UpperLimit, CurMan, VectorFetchTime);
		if (err)
		{
			return 1;
		}
		if (working_man == 1)
		{
			//Calculate number of previous maneuver, if applicable
			unsigned int prevman;
			if (inp->ReplaceCode > 0)
			{
				prevman = inp->ReplaceCode - 1;
			}
			else
			{
				prevman = mpt->ManeuverNum;
			}

			if (prevman > 0)
			{
				in.CSMWeight = 0.0;
				in.LMWeight = 0.0;
				in.VehicleWeight = mpt->mantable[prevman - 1].TotalMassAfter;
				in.VehicleArea = mpt->mantable[prevman - 1].TotalAreaAfter;
				in.DockingAngle = mpt->mantable[prevman - 1].DockingAngle;
			}
			else
			{
				in.CSMWeight = 0.0;
				in.LMWeight = 0.0;
				in.VehicleWeight = mpt->TotalInitMass;
				in.VehicleArea = mpt->ConfigurationArea;
				in.DockingAngle = mpt->DeltaDockingAngle;
			}
			if (replace)
			{
				//TBD
			}
		}
		//Format maneuver code
		err = PMMXFRFormatManeuverCode(plan, inp->Thruster[working_man - 1], inp->Attitude[working_man - 1], CurMan, purpose, TVC, code);
		//Check thruster
		if (mpt->mantable.size() == 0)
		{
			CCP = mpt->CommonBlock.ConfigCode;
		}
		else
		{
			CCP = mpt->mantable.back().CommonBlock.ConfigCode;
		}
		CC = CCMI = CCP;
		err = PMMXFRCheckConfigThruster(false, 0, CCP, TVC, inp->Thruster[working_man - 1], CC, CCMI);
		//Is attitude mode an AGS?
		if (inp->Attitude[working_man - 1] == 5)
		{
			if (EMSFFV(GMTI, 4 - plan, in.sv_other))
			{
				//TBD: Error message?
				return 1;
			}
		}
		in.Attitude = inp->Attitude[working_man - 1];
		in.CONFIG = CC.to_ulong();
		in.CurrentManeuver = CurMan;
		in.DETU = inp->dt_ullage[working_man - 1];
		in.DPSScaleFactor = inp->DPSScaleFactor[working_man - 1];
		in.DT_10PCT = inp->DT10P[working_man - 1];
		in.IgnitionTimeOption = inp->TimeFlag[working_man - 1];
		in.IterationFlag = inp->IterationFlag[working_man - 1];
		in.LowerTimeLimit = LowerLimit;
		in.mpt = mpt;
		if (id == 39)
		{
			if (inp->Type == 0)
			{
				in.sv_before = PZMCCXFR.sv_man_bef[inp->Plan - 1];
				in.V_aft = PZMCCXFR.V_man_after[inp->Plan - 1];
			}
			else
			{
				in.sv_before = PZLRBELM.sv_man_bef[inp->Plan - 1];
				in.V_aft = PZLRBELM.V_man_after[inp->Plan - 1];
			}
		}
		else if (id == 40)
		{
			in.sv_before = PZGPMELM.SV_before;
			in.V_aft = PZGPMELM.V_after;
		}
		else if (id == 41)
		{
			if (inp->Plan < 0)
			{
				in.sv_before = PZLDPELM.sv_man_bef[0];
				in.V_aft = PZLDPELM.V_man_after[0];
			}
			else
			{
				in.sv_before = PZDKIELM.Block[0].SV_before[0];
				in.V_aft = PZDKIELM.Block[0].V_after[0];
			}
		}
		else
		{
			in.sv_before = PZMYSAVE.SV_before[working_man - 1];
			in.V_aft = PZMYSAVE.V_after[working_man - 1];
		}
		in.Thruster = inp->Thruster[working_man - 1];
		in.UpperTimeLimit = UpperLimit;
		in.UT = inp->UllageThrusterOption[working_man - 1];
		in.VC = TVC;

		MPTManeuver man;

		man.code = code;
		if (PMMMPT(in, man))
		{
			return 1;
		}

		//Either add a new maneuver or replace one
		if (CurMan > mpt->mantable.size())
		{
			mpt->mantable.push_back(man);
		}
		else
		{
			mpt->mantable[CurMan - 1] = man;
		}
		mpt->ManeuverNum = mpt->mantable.size();

		if (working_man == 1 && man.GMTI < mpt->UpcomingManeuverGMT)
		{
			PMSEXE(plan, RTCCPresentTimeGMT());
		}

		if (plan == RTCC_MPT_CSM)
		{
			update_csm_eph = true;
		}
		else
		{
			update_lm_eph = true;
		}

		if (num_man > working_man)
		{
			working_man++;
			goto RTCC_PMMXFR_1;
		}

		if (update_csm_eph)
		{
			PMSVCT(8, RTCC_MPT_CSM);
		}
		if (update_lm_eph)
		{
			PMSVCT(8, RTCC_MPT_LM);
		}
	}
	//PDI
	else if (id == 43)
	{
		if (med_m86.ReplaceCode > 0)
		{
			replace = true;
		}
		else
		{
			replace = false;
		}
		//Store purpose code
		std::string purpose = "DSC";
		//MPT header
		mpt = GetMPTPointer(med_m86.Veh);

		PMMLDPInput in;
		MPTManeuver man;

		if (mpt->mantable.size() == 0)
		{
			CCP = mpt->CommonBlock.ConfigCode;
		}
		else
		{
			CCP = mpt->mantable.back().CommonBlock.ConfigCode;
		}
		//Do we have a full LM?
		if (!(CCP[RTCC_CONFIG_A] && CCP[RTCC_CONFIG_D]))
		{
			return 1;
		}

		in.CurrentManeuver = mpt->mantable.size();
		in.HeadsUpDownInd = med_m86.HeadsUp;
		in.mpt = mpt;
		if (mpt->mantable.size() == 0)
		{
			in.sv.R = EZANCHR3.AnchorVectors[9].Vector.R;
			in.sv.V = EZANCHR3.AnchorVectors[9].Vector.V;
			in.sv.MJD = OrbMech::MJDfromGET(EZANCHR3.AnchorVectors[9].Vector.GMT,SystemParameters.GMTBASE);
			in.sv.gravref = GetGravref(BODY_MOON);
			in.sv.mass = mpt->TotalInitMass;
		}
		else
		{
			in.sv.R = mpt->mantable.back().R_BO;
			in.sv.V = mpt->mantable.back().V_BO;
			in.sv.MJD = OrbMech::MJDfromGET(mpt->mantable.back().GMT_BO,SystemParameters.GMTBASE);
			in.sv.gravref = GetGravref(mpt->mantable.back().RefBodyInd);
			in.sv.mass = mpt->mantable.back().TotalMassAfter;
		}
		in.TLAND = med_m86.Time;
		in.TrimAngleInd = med_m86.TrimAngleIndicator;

		//Determine time
		PMMLDP(in, man);

		//Check ground rules
		err = PMMXFRGroundRules(mpt, man.GMTI, med_m86.ReplaceCode, LastManReplaceFlag, LowerLimit, UpperLimit, CurMan, VectorFetchTime);
		//Format maneuver code
		err = PMMXFRFormatManeuverCode(med_m86.Veh, RTCC_ENGINETYPE_LMDPS, RTCC_ATTITUDE_PGNS_DESCENT, CurMan, purpose, TVC, code);
		//Check configuration and thrust
		CC = CCP;
		err = PMMXFRCheckConfigThruster(true, 0, CCP, TVC, RTCC_ENGINETYPE_LMDPS, CC, CCMI);

		man.code = code;
		mpt->mantable.push_back(man);
		mpt->ManeuverNum = mpt->mantable.size();

		if (man.GMTI < mpt->UpcomingManeuverGMT)
		{
			PMSEXE(RTCC_MPT_LM, RTCCPresentTimeGMT());
		}

		PMSVCT(8, RTCC_MPT_LM);
	}
	//Ascent
	else if (id == 44)
	{
		if (med_m85.ReplaceCode > 0)
		{
			replace = true;
		}
		else
		{
			replace = false;
		}
		//Store purpose code
		std::string purpose = "ASC";
		//MPT header
		mpt = GetMPTPointer(med_m85.VEH);

		MPTManeuver man;

		if (mpt->mantable.size() == 0)
		{
			CCP = mpt->CommonBlock.ConfigCode;
		}
		else
		{
			CCP = mpt->mantable.back().CommonBlock.ConfigCode;
		}

		man.GMTI = GMTfromGET(JZLAI.t_launch);

		//Check ground rules
		err = PMMXFRGroundRules(mpt, man.GMTI, med_m85.ReplaceCode, LastManReplaceFlag, LowerLimit, UpperLimit, CurMan, VectorFetchTime);
		//Format maneuver code
		err = PMMXFRFormatManeuverCode(med_m85.VEH, RTCC_ENGINETYPE_LMAPS, RTCC_ATTITUDE_PGNS_ASCENT, CurMan, purpose, TVC, code);
		//Check configuration and thrust
		CC[RTCC_CONFIG_A] = true;
		err = PMMXFRCheckConfigThruster(true, RTCC_CONFIGCHANGE_UNDOCKING, CCP, TVC, RTCC_ENGINETYPE_LMAPS, CC, CCMI);

		man.code = code;
		man.GMTMAN = man.GMTI;
		man.dV_inertial.x = JZLAI.R_D;
		man.dV_inertial.y = JZLAI.Y_D;
		man.dV_LVLH.x = JZLAI.R_D_dot;
		man.dV_LVLH.y = JZLAI.Y_D_dot;
		man.dV_LVLH.z = JZLAI.Z_D_dot;
		man.CommonBlock.ConfigChangeInd = RTCC_CONFIGCHANGE_UNDOCKING;
		man.ConfigCodeBefore[RTCC_CONFIG_A] = true;
		man.ConfigCodeBefore[RTCC_CONFIG_D] = true;
		man.CommonBlock.ConfigCode = CC;
		man.Thruster = RTCC_ENGINETYPE_LMAPS;
		man.AttitudeCode = RTCC_ATTITUDE_PGNS_ASCENT;
		man.TVC = RTCC_MANVEHICLE_LM;
		mpt->mantable.push_back(man);
		mpt->TimeToBeginManeuver[CurMan - 1] = mpt->TimeToEndManeuver[CurMan - 1] = man.GMTMAN;
		mpt->ManeuverNum = mpt->mantable.size();

		if (man.GMTI < mpt->UpcomingManeuverGMT)
		{
			PMSEXE(RTCC_MPT_LM, RTCCPresentTimeGMT());
		}

		PMSVCT(8, RTCC_MPT_LM);
	}
	return 0;
}

int RTCC::PMMXFRGroundRules(MissionPlanTable * mpt, double GMTI, unsigned ReplaceMan, bool &LastManReplaceFlag, double &LowerLimit, double &UpperLimit, unsigned &CurMan, double &VectorFetchTime)
{
	double CurrentGMT;
	bool DeleteFlag = false;
	LastManReplaceFlag = false;

	CurrentGMT = RTCCPresentTimeGMT();

	if (ReplaceMan > 0)
	{
		CurMan = ReplaceMan;
		LastManReplaceFlag = true;

		//Is maneuver prior to present time?
		if (GMTI < CurrentGMT)
		{
			PMXSPT("PMMXFR", 1);
			return 1;
		}
		UpperLimit = 1e70;
		//Does maneuver to be replaced exist?
		if (ReplaceMan > mpt->mantable.size())
		{
			RTCCONLINEMON.IntBuffer[0] = (int)ReplaceMan;
			PMXSPT("PMMXFR", 2);
			return 2;
		}
		//Is this the last maneuver in MPT?
		if (ReplaceMan != mpt->mantable.size())
		{
			LastManReplaceFlag = false;
			if (GMTI > mpt->mantable[ReplaceMan].GMTMAN)
			{
				RTCCONLINEMON.IntBuffer[0] = (int)ReplaceMan;
				PMXSPT("PMMXFR", 3);
				return 3;
			}
		}
		//Set lower limit to end time of previous maneuver
		if (ReplaceMan > 1)
		{
			LowerLimit = mpt->mantable[ReplaceMan - 2].GMT_BO;
		}
		//Is this first maneuver in MPT?
		if (ReplaceMan == 1)
		{
			LowerLimit = CurrentGMT;
		}
		if (GMTI < LowerLimit)
		{
			RTCCONLINEMON.IntBuffer[0] = (int)ReplaceMan;
			PMXSPT("PMMXFR", 3);
			return 3;
		}
	}
	else
	{
		//Is maneuver prior to present time?
		if (GMTI < CurrentGMT)
		{
			PMXSPT("PMMXFR", 1);
			return 1;
		}
		if (mpt->mantable.size() == 0)
		{
			//Set lower limit to present time
			LowerLimit = CurrentGMT;
		}
		else
		{
			do
			{
				LowerLimit = mpt->mantable.back().GMT_BO;
				if (GMTI > LowerLimit)
				{
					break;
				}
				mpt->mantable.pop_back();
				DeleteFlag = true;
				if (mpt->mantable.size() == 0)
				{
					LowerLimit = CurrentGMT;
				}
			} while (mpt->mantable.size() > 0);
		}
		CurMan = mpt->mantable.size() + 1;
		if (CurMan >= mpt->MaxManeuverNum)
		{
			PMXSPT("PMMXFR", 5);
			return 5;
		}
		if (CurMan == 1 && DeleteFlag)
		{
			RTCCONLINEMON.IntBuffer[0] = (int)ReplaceMan;
			PMXSPT("PMMXFR", 3);
			return 3;
		}
		UpperLimit = 1e70;
	}

	//Store pointers and compute pointers to previous descriptive block
	if (ReplaceMan > 0)
	{
		if (mpt->mantable[ReplaceMan - 1].GMTMAN < GMTI)
		{
			//Set vector fetch time to time of original maneuver
			VectorFetchTime = mpt->mantable[ReplaceMan - 1].GMTMAN;
		}
	}

	//TBD: Set upper and lower limit times
	if (CurMan == 1)
	{
		return 0;
	}
	//TBD: Store descriptive block for PMMMCD, also store area and weight

	return 0;
}

int RTCC::PMMXFRFormatManeuverCode(int Table, int Thruster, int Attitude, unsigned Maneuver, std::string ID, int &TVC, std::string &code)
{
	char Veh, Thr, Guid, Tab;
	char Buffer[20];

	//Format all but purpose of maneuver code

	//Set MPT
	if (Table == RTCC_MPT_CSM)
	{
		Tab = 'C';
	}
	else
	{
		Tab = 'L';
	}

	//Set thrusting vehicle ID
	if (Thruster == RTCC_ENGINETYPE_LOX_DUMP || Thruster == RTCC_ENGINETYPE_SIVB_MAIN)
	{
		TVC = 2;
		Veh = 'S';
	}
	else if (Thruster == RTCC_ENGINETYPE_CSMSPS || Thruster == RTCC_ENGINETYPE_CSMRCSMINUS2 || Thruster == RTCC_ENGINETYPE_CSMRCSPLUS2 || Thruster == RTCC_ENGINETYPE_CSMRCSMINUS4 || Thruster == RTCC_ENGINETYPE_CSMRCSPLUS4)
	{
		TVC = 1;
		Veh = 'C';
	}
	else
	{
		TVC = 3;
		Veh = 'L';
	}

	if (Thruster == RTCC_ENGINETYPE_CSMSPS)
	{
		Thr = 'S';
	}
	else if (Thruster == RTCC_ENGINETYPE_LMDPS)
	{
		Thr = 'D';
	}
	else if (Thruster == RTCC_ENGINETYPE_LMAPS)
	{
		Thr = 'A';
	}
	else if (Thruster == RTCC_ENGINETYPE_LOX_DUMP || Thruster == RTCC_ENGINETYPE_SIVB_MAIN)
	{
		Thr = 'J';
	}
	else
	{
		Thr = 'R';
	}

	if (Attitude == RTCC_ATTITUDE_INERTIAL)
	{
		Guid = 'I';
	}
	else if (Attitude == RTCC_ATTITUDE_MANUAL)
	{
		Guid = 'M';
	}
	else if (Attitude == RTCC_ATTITUDE_LAMBERT)
	{
		Guid = 'L';
	}
	else if (Attitude == RTCC_ATTITUDE_PGNS_EXDV || Attitude == RTCC_ATTITUDE_PGNS_DESCENT || Attitude == RTCC_ATTITUDE_PGNS_ASCENT)
	{
		Guid = 'X';
	}
	else if (Attitude == RTCC_ATTITUDE_AGS_EXDV || Attitude == RTCC_ATTITUDE_AGS_ASCENT)
	{
		Guid = 'A';
	}
	else if (Attitude == RTCC_ATTITUDE_SIVB_IGM)
	{
		Guid = 'G';
	}
	else
	{
		Guid = 'U';
	}

	sprintf(Buffer, "%c%c%c%c%02d%s", Tab, Veh, Thr, Guid, Maneuver, ID.c_str());
	code.assign(Buffer);
	
	return 0;
}

int RTCC::PMMXFRCheckConfigThruster(bool CheckConfig, int CCI, const std::bitset<4> &CCP, int TVC, int Thruster, std::bitset<4> &CC, std::bitset<4> &CCMI)
{
	if (CheckConfig)
	{
		if (CCI > 1)
		{
			CCMI = CCP;
		}
		else
		{
			if (CCI < 1)
			{
				CC = CCP;
			}

			CCMI = CC;
			//Obtain docking angle from previous block
		}
	}
	if (TVC == 2)
	{
		if (!(CC[RTCC_CONFIG_S] && CCP[RTCC_CONFIG_S]))
		{
			PMXSPT("PMMXFR", 6);
			return 6;
		}
	}
	else
	{
		if (CCMI[RTCC_CONFIG_S] && (Thruster == RTCC_ENGINETYPE_LMAPS || Thruster == RTCC_ENGINETYPE_LMDPS || Thruster == RTCC_ENGINETYPE_CSMSPS))
		{
			PMXSPT("PMMXFR", 6);
			return 6;
		}
		if (TVC > 2)
		{
			if (Thruster == RTCC_ENGINETYPE_LMAPS && !(CC[RTCC_CONFIG_A] && CCP[RTCC_CONFIG_A]))
			{
				PMXSPT("PMMXFR", 6);
				return 6;
			}
			else if (Thruster == RTCC_ENGINETYPE_LMDPS && !(CC[RTCC_CONFIG_D] && CCP[RTCC_CONFIG_D]))
			{
				PMXSPT("PMMXFR", 6);
				return 6;
			}
		}
		else
		{
			if (!(CC[RTCC_CONFIG_C] && CCP[RTCC_CONFIG_C]))
			{
				PMXSPT("PMMXFR", 6);
				return 6;
			}
		}
	}

	//No configuration change
	if (CCI < 1)
	{
		return 0;
	}
	else if (CCI == 1)
	{
		if (CCP == CC)
		{
			PMXSPT("PMMXFR", 6);
			return 6;
		}
		else if (!MPTConfigSubset(CCP, CC))
		{
			PMXSPT("PMMXFR", 6);
			return 6;
		}
		else
		{
			return 0;
		}
	}

	//TBD: Docking

	return 0;
}

int RTCC::PMMXFRFetchVector(double GMTI, int L, EphemerisData &sv)
{
	EphemerisDataTable2 EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;

	int err2 = ELFECH(GMTI, 1, 1, L, EPHEM, MANTIMES, LUNSTAY);
	if (err2)
	{
		return 9;
	}
	if (LUNSTAY.LunarStayBeginGMT <= GMTI && LUNSTAY.LunarStayEndGMT >= GMTI)
	{
		return 9;
	}

	sv = RotateSVToSOI(EPHEM.table[0]);
	return 0;
}

int RTCC::PMMXFRFetchAnchorVector(int L, EphemerisData &sv)
{
	return EMGVECSTOutput(L, sv);
}

void RTCC::PMMXFRWeightAtInitiation(int CCI, int CCMI, double &weight)
{
	if (CCI != 1)
	{
		return;
	}

	//If undocking, determine weight
}

bool RTCC::PMMXFRDeleteOption(int L, double GMTI)
{
	MissionPlanTable *mpt = GetMPTPointer(L);

	if (mpt->ManeuverNum == 0)
	{
		return false;
	}
	do
	{
		if (GMTI <= mpt->TimeToEndManeuver[mpt->ManeuverNum - 1])
		{
			if (mpt->mantable[mpt->ManeuverNum - 1].FrozenManeuverInd)
			{
				PMXSPT("PMMXFR", 37);
				return true;
			}

			mpt->mantable.pop_back();
			mpt->ManeuverNum--;
		}
		else
		{
			break;
		}
	} while (mpt->ManeuverNum > 0);

	return false;
}

int RTCC::PMMMCDCallEMSMISS(EphemerisData sv0, double GMTI, EphemerisData &sv1)
{
	double dt = GMTI - sv0.GMT;
	sv1 = coast(sv0, dt);
	return 0;
}

double RTCC::RTCCPresentTimeGMT()
{
	return RTCC_GreenwichMeanTime;
}

double RTCC::RTCCMissionTime(int veh)
{
	//For MOCR clocks, PAMFD etc.
	if (veh == 1) return RTCC_CSM_GroundElapsedTime;
	return RTCC_LM_GroundElapsedTime;
}

double RTCC::GetOnboardComputerThrust(int thruster)
{
	if (thruster == RTCC_ENGINETYPE_CSMSPS)
	{
		return SystemParameters.MCTST9;
	}
	else if (thruster == RTCC_ENGINETYPE_LMAPS)
	{
		return SystemParameters.MCTAT9;
	}
	else if (thruster == RTCC_ENGINETYPE_LMDPS)
	{
		return SystemParameters.MCTDT9;
	}
	else if (thruster == RTCC_ENGINETYPE_CSMRCSPLUS2 || thruster == RTCC_ENGINETYPE_CSMRCSMINUS2)
	{
		return SystemParameters.MCTCT5;
	}
	else if (thruster == RTCC_ENGINETYPE_CSMRCSPLUS4 || thruster == RTCC_ENGINETYPE_CSMRCSMINUS4)
	{
		return SystemParameters.MCTCT6;
	}
	else if (thruster == RTCC_ENGINETYPE_LMRCSPLUS2 || thruster == RTCC_ENGINETYPE_LMRCSMINUS2)
	{
		return SystemParameters.MCTLT5;
	}
	else if (thruster == RTCC_ENGINETYPE_LMRCSPLUS4 || thruster == RTCC_ENGINETYPE_LMRCSMINUS4)
	{
		return SystemParameters.MCTLT6;
	}

	return 1e70;
}

void RTCC::GetSystemGimbalAngles(int thruster, double &P_G, double &Y_G) const
{
	if (thruster == RTCC_ENGINETYPE_CSMSPS)
	{
		P_G = SystemParameters.MCTSPG;
		Y_G = SystemParameters.MCTSYG;
	}
	else if (thruster == RTCC_ENGINETYPE_LMAPS)
	{
		P_G = SystemParameters.MCTAPG;
		Y_G = SystemParameters.MCTARG;
	}
	else if (thruster == RTCC_ENGINETYPE_LMDPS)
	{
		P_G = SystemParameters.MCTDPG;
		Y_G = SystemParameters.MCTDRG;
	}
	else
	{
		P_G = 0.0;
		Y_G = 0.0;
	}
}

void RTCC::PMDDMT(int MPT_ID, unsigned ManNo, int REFSMMAT_ID, bool HeadsUp, DetailedManeuverTable &res)
{
	if (MPT_ID == 0) return;

	res.error = "";

	MissionPlanTable *table, *othertable;
	MPTManeuver *man;
	int otherID;

	if (MPT_ID == RTCC_MPT_LM)
	{
		table = &PZMPTLEM;
		othertable = &PZMPTCSM;
		otherID = RTCC_MPT_CSM;
	}
	else
	{
		table = &PZMPTCSM;
		othertable = &PZMPTLEM;
		otherID = RTCC_MPT_LM;
	}

	if (ManNo > table->mantable.size())
	{
		res.error = "MANEUVER NOT AVAILABLE";
		return;
	}

	double mu;

	man = &table->mantable[ManNo - 1];

	sprintf_s(res.CODE, man->code.c_str());
	res.Attitude = man->AttitudeCode;
	res.GETI = GETfromGMT(man->GMTI);
	if (man->AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
	{
		res.PGNS_GETI = GETfromGMT(man->GMTMAN);
		res.AGS_GETI = res.CFP_GETI = res.Lam_GETI = 0.0;
	}
	else
	{
		res.AGS_GETI = res.CFP_GETI = res.Lam_GETI = res.PGNS_GETI = res.GETI;
	}
	res.GETR = SystemParameters.MCGREF * 3600.0;
	res.PETI = res.GETI - res.GETR;
	res.DVM = man->DV_M / 0.3048;
	res.DVREM = man->DVREM / 0.3048;
	res.DVC = man->DVC / 0.3048;

	res.DT_B = man->dt_BD;
	res.DT_U = man->dt_ullage;
	res.DT_TO = man->dt_TO;
	res.DV_TO = man->dv_TO / 0.3048;
	if (man->Thruster == RTCC_ENGINETYPE_CSMSPS)
	{
		res.DEL_P = (man->P_G - SystemParameters.MCTSPP)*DEG;
		res.DEL_Y = (man->Y_G - SystemParameters.MCTSYP)*DEG;
	}
	else if (man->Thruster == RTCC_ENGINETYPE_LMDPS)
	{
		ConvertDPSGimbalAnglesToTrim(man->P_G, man->Y_G, res.DEL_P, res.DEL_Y);

		res.DEL_P *= DEG;
		res.DEL_Y *= DEG;
	}
	else
	{
		res.DEL_P = man->P_G*DEG;
		res.DEL_Y = man->Y_G*DEG;
	}
	
	res.VF = man->V_F / 0.3048;
	res.VS = man->V_S / 0.3048;
	res.VD = man->V_D / 0.3048;
	res.H_BI = man->h_BI / 1852.0;
	res.P_BI = man->lat_BI*DEG;
	res.L_BI = man->lng_BI*DEG;
	res.F_BI = man->eta_BI*DEG;
	res.E = man->e_BO;
	res.I = man->i_BO*DEG;
	res.WP = man->g_BO*DEG;
	res.HA = man->h_a / 1852.0;
	if (res.HA > 99999.999)
	{
		res.HA = 99999.999;
	}
	res.HP = man->h_p / 1852.0;
	res.L_AN = man->lng_AN*DEG;

	res.P_LLS = 0.0;
	res.L_LLS = 0.0;
	res.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST] / 1852.0;

	res.PHASE = 0.0;
	res.PHASE_DOT = 0.0;
	res.WEDGE_ANG = 0.0;
	res.DH = 0.0;

	sprintf_s(res.PGNS_Veh, "");
	sprintf_s(res.AGS_Veh, "");

	if (man->AttitudeCode == 5)
	{
		res.PGNS_DV = _V(0, 0, 0);
		res.AGS_DV = man->dV_LVLH / 0.3048;
		sprintf_s(res.AGS_Veh, "LM");
	}
	else if (man->AttitudeCode == 4)
	{
		res.PGNS_DV = man->dV_LVLH / 0.3048;
		if (man->TVC == 3)
		{
			//Calc AGS DV, done further down
		}

		if (MPT_ID == RTCC_MPT_CSM)
		{
			sprintf_s(res.PGNS_Veh, "CSM");
		}
		else
		{
			sprintf_s(res.PGNS_Veh, "LM");
		}
	}
	else
	{
		res.PGNS_DV = _V(0, 0, 0);
		res.AGS_DV = _V(0, 0, 0);
	}

	if (MPT_ID == RTCC_MPT_CSM)
	{
		sprintf_s(res.C_STA_ID, table->StationID.c_str());
		res.C_GMTV = table->GMTAV;
		res.C_GETV = GETfromGMT(table->GMTAV);
	}
	else
	{
		sprintf_s(res.L_STA_ID, table->StationID.c_str());
		res.L_GMTV = table->GMTAV;
		res.L_GETV = GETfromGMT(table->GMTAV);
	}

	if (man->FrozenManeuverInd)
	{
		//TBD: frozen maneuver
	}
	else
	{
		res.X_GMTV = 0.0;
		sprintf(res.X_STA_ID, "");
	}

	if (man->RefBodyInd == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
		sprintf_s(res.REF, "EARTH");
	}
	else
	{
		mu = OrbMech::mu_Moon;
		sprintf_s(res.REF, "LUNAR");
	}

	bool UseOtherMPT;

	if (othertable->CommonBlock.TUP > 0)
	{
		UseOtherMPT = true;
	}
	else
	{
		UseOtherMPT = false;
	}

	VECTOR3 X_B, Y_B, Z_B, X_P, Y_P, Z_P;

	X_B = man->X_B;
	Y_B = man->Y_B;
	Z_B = man->Z_B;

	MATRIX3 REFSMMAT;
	bool RefsAvailable = true;

	if (REFSMMAT_ID == 100)
	{
		REFSMMAT = PIDREF(man->A_T, man->R_BI, man->V_BI, man->P_G, man->Y_G, HeadsUp);
		sprintf_s(res.REFSMMAT_Code, "DES");
	}
	else
	{
		REFSMMATData refdata;

		if (man->TVC == 3)
		{
			refdata = EZJGMTX3.data[REFSMMAT_ID - 1];
		}
		else
		{
			refdata = EZJGMTX1.data[REFSMMAT_ID - 1];
		}

		if (refdata.ID == 0)
		{
			RefsAvailable = false;
			res.error = "REFSMMAT NOT AVAILABLE";
			sprintf(res.REFSMMAT_Code, "");
		}
		else
		{
			REFSMMAT = refdata.REFSMMAT;
			FormatREFSMMATCode(REFSMMAT_ID, refdata.ID, res.REFSMMAT_Code);
		}
	}

	if (man->TVC == 3)
	{
		res.isCSMTV = false;
	}
	else
	{
		res.isCSMTV = true;
	}

	if (RefsAvailable)
	{
		X_P = _V(REFSMMAT.m11, REFSMMAT.m12, REFSMMAT.m13);
		Y_P = _V(REFSMMAT.m21, REFSMMAT.m22, REFSMMAT.m23);
		Z_P = _V(REFSMMAT.m31, REFSMMAT.m32, REFSMMAT.m33);

		double MG, OG, IG, C;

		MG = asin(dotp(Y_P, X_B));
		C = abs(MG);

		if (abs(C - PI05) < 0.0017)
		{
			OG = 0.0;
			IG = atan2(dotp(X_P, Z_B), dotp(Z_P, Z_B));
		}
		else
		{
			OG = atan2(-dotp(Z_B, Y_P), dotp(Y_B, Y_P));
			IG = atan2(-dotp(X_B, Z_P), dotp(X_B, X_P));
		}

		if (man->TVC == 3)
		{
			double Y, P, R;
			Y = asin(-cos(MG)*sin(OG));
			if (abs(sin(Y)) != 1.0)
			{
				R = atan2(sin(MG), cos(OG)*cos(MG));
				P = atan2(sin(IG)*cos(OG) + sin(MG)*sin(OG)*cos(IG), cos(OG)*cos(IG) - sin(MG)*sin(OG)*sin(IG));
			}
			else
			{
				P = 0.0;
				R = 0.0;
			}
			res.FDAIAtt = _V(R, P, Y);
		}
		else
		{
			res.FDAIAtt = _V(OG, IG, MG);
		}

		for (int i = 0;i < 3;i++)
		{
			res.FDAIAtt.data[i] *= DEG;
			if (res.FDAIAtt.data[i] < 0)
			{
				res.FDAIAtt.data[i] += 360.0;
			}
			if (res.FDAIAtt.data[i] >= 359.95)
			{
				res.FDAIAtt.data[i] = 0.0;
			}
		}
		res.IMUAtt = _V(OG, IG, MG);
		for (int i = 0;i < 3;i++)
		{
			res.IMUAtt.data[i] *= DEG;
			if (res.IMUAtt.data[i] < 0)
			{
				res.IMUAtt.data[i] += 360.0;
			}
			if (res.IMUAtt.data[i] >= 359.95)
			{
				res.IMUAtt.data[i] = 0.0;
			}
		}
	}
	else
	{
		res.FDAIAtt = _V(0, 0, 0);
		res.IMUAtt = _V(0, 0, 0);
	}

	VECTOR3 DV, V_G;

	if (man->AttitudeCode < 3 || man->AttitudeCode > 5)
	{
		DV = man->A_T*man->DV_M;
	}
	else
	{
		DV = man->dV_inertial;
	}

	if (RefsAvailable)
	{
		V_G.x = dotp(DV, X_P);
		V_G.y = dotp(DV, Y_P);
		V_G.z = dotp(DV, Z_P);

		res.VG = V_G / 0.3048;
	}
	else
	{
		res.VG = _V(0, 0, 0);
	}

	res.LVLHAtt.x = man->Y_H * DEG;
	res.LVLHAtt.y = man->P_H * DEG;
	res.LVLHAtt.z = man->R_H * DEG;

	if (UseOtherMPT)
	{
		ELVCTRInputTable interin;
		ELVCTROutputTable2 interout;
		EphemerisData sv_other;

		interin.L = 4 - MPT_ID;
		interin.GMT = man->GMT_BO;

		ELVCTR(interin, interout);

		if (interout.ErrorCode == 0)
		{
			sv_other = RotateSVToSOI(interout.SV);

			VECTOR3 R_m = man->R_BO;
			VECTOR3 V_m = man->V_BO;
			VECTOR3 H_m = crossp(R_m, V_m);

			VECTOR3 R_a = sv_other.R;
			VECTOR3 V_a = sv_other.V;
			VECTOR3 H_a = crossp(R_a, V_a);

			VECTOR3 N = crossp(H_a, H_m);
			double h_m = length(H_m);
			double h_a = length(H_a);
			double v_a_sq = dotp(V_a, V_a);
			double v_m_sq = dotp(V_m, V_m);
			double r_a = length(R_a);
			double r_m = length(R_m);
			double a_a = r_a / (2.0 - r_a * v_a_sq / mu);
			double a_m = r_m / (2.0 - r_m * v_m_sq / mu);

			if (man->AttitudeCode == 4 && man->TVC == 3)
			{
				res.AGS_DV = PIAEDV(DV, R_m, V_m, man->R_BI, false) / 0.3048;
			}

			if (a_a > 0 && a_m > 0)
			{
				double theta_dot = PI2 * (sqrt(pow(a_a, 3) / pow(a_m, 3)) - 1.0);
				double delta = asin(length(N) / h_m / h_a);

				res.PHASE_DOT = theta_dot * DEG;
				res.WEDGE_ANG = delta * DEG;

				EphemerisData sv_dh;
				CELEMENTS elem_a, elem_m;
				double h1, h2, i1, i2, u1, u2, gamma, Cg, Tg, alpha1, alpha2, beta, theta_R, n, dt, t_R;
				int i = 0;
				dt = 1.0;
				t_R = man->GMT_BO;

				sv_dh.RBI = man->RefBodyInd;

				while (i<10 && abs(dt)>0.01)
				{
					elem_a = OrbMech::GIMIKC(R_a, V_a, mu);
					elem_m = OrbMech::GIMIKC(R_m, V_m, mu);

					h1 = elem_m.h;
					h2 = elem_a.h;
					i1 = elem_m.i;
					i2 = elem_a.i;
					u1 = OrbMech::MeanToTrueAnomaly(elem_m.l, elem_m.e) + elem_m.g;
					if (u1 > PI2)
					{
						u1 -= PI2;
					}
					u2 = OrbMech::MeanToTrueAnomaly(elem_a.l, elem_a.e) + elem_a.g;
					if (u2 > PI2)
					{
						u2 -= PI2;
					}

					gamma = (h2 - h1) / 2.0;
					Cg = cos(gamma);
					Tg = sin(gamma) / Cg;
					alpha2 = (i2 + i1) / 2.0;
					alpha1 = alpha2 - i1;
					beta = 2.0*atan(cos(alpha2) / cos(alpha1)*Tg);
					theta_R = u1 - u2 + beta;
					if (theta_R > PI)
					{
						theta_R -= PI2;
					}
					if (theta_R < -PI)
					{
						theta_R += PI2;
					}
					if (i == 0)
					{
						res.PHASE = theta_R * DEG;
					}

					n = OrbMech::GetMeanMotion(R_a, V_a, mu);
					dt = theta_R / n;

					sv_dh.R = R_a;
					sv_dh.V = V_a;
					sv_dh.GMT = t_R;

					sv_dh = coast(sv_dh, dt);
					R_a = sv_dh.R;
					V_a = sv_dh.V;
					t_R = sv_dh.GMT;

					i++;
				}
				res.DH = (length(R_a) - length(R_m)) / 1852.0;
			}
		}
	}

	if (ManNo == 1)
	{
		res.WT = table->TotalInitMass / 0.45359237;
		res.WC = table->CommonBlock.CSMMass / 0.45359237;
		res.WL = (table->CommonBlock.LMAscentMass + table->CommonBlock.LMDescentMass) / 0.45359237;

		switch (man->Thruster)
		{
		case RTCC_ENGINETYPE_CSMSPS:
			res.WF = table->CommonBlock.SPSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_LMDPS:
			res.WF = table->CommonBlock.LMDPSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_LMAPS:
			res.WF = table->CommonBlock.LMAPSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_SIVB_MAIN:
			res.WF = table->CommonBlock.SIVBFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_CSMRCSMINUS2:
		case RTCC_ENGINETYPE_CSMRCSMINUS4:
		case RTCC_ENGINETYPE_CSMRCSPLUS2:
		case RTCC_ENGINETYPE_CSMRCSPLUS4:
			res.WF = table->CommonBlock.CSMRCSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_LMRCSMINUS2:
		case RTCC_ENGINETYPE_LMRCSMINUS4:
		case RTCC_ENGINETYPE_LMRCSPLUS2:
		case RTCC_ENGINETYPE_LMRCSPLUS4:
			res.WF = table->CommonBlock.LMRCSFuelRemaining / 0.45359237;
			break;
		default:
			res.WF = 0.0;
			break;
		}
	}
	else
	{
		res.WT = table->mantable[ManNo - 2].TotalMassAfter / 0.45359237;
		res.WC = table->mantable[ManNo - 2].CommonBlock.CSMMass / 0.45359237;
		res.WL = (table->mantable[ManNo - 2].CommonBlock.LMAscentMass + table->mantable[ManNo - 2].CommonBlock.LMDescentMass) / 0.45359237;
		switch (man->Thruster)
		{
		case RTCC_ENGINETYPE_CSMSPS:
			res.WF = table->mantable[ManNo - 2].CommonBlock.SPSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_LMDPS:
			res.WF = table->mantable[ManNo - 2].CommonBlock.LMDPSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_LMAPS:
			res.WF = table->mantable[ManNo - 2].CommonBlock.LMAPSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_SIVB_MAIN:
			res.WF = table->mantable[ManNo - 2].CommonBlock.SIVBFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_CSMRCSMINUS2:
		case RTCC_ENGINETYPE_CSMRCSMINUS4:
		case RTCC_ENGINETYPE_CSMRCSPLUS2:
		case RTCC_ENGINETYPE_CSMRCSPLUS4:
			res.WF = table->mantable[ManNo - 2].CommonBlock.CSMRCSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_LMRCSMINUS2:
		case RTCC_ENGINETYPE_LMRCSMINUS4:
		case RTCC_ENGINETYPE_LMRCSPLUS2:
		case RTCC_ENGINETYPE_LMRCSPLUS4:
			res.WF = table->mantable[ManNo - 2].CommonBlock.LMRCSFuelRemaining / 0.45359237;
			break;
		default:
			res.WF = 0.0;
			break;
		}
	}
}

void RTCC::PMDLDPP(const LDPPOptions &opt, const LDPPResults &res, LunarDescentPlanningTable &table)
{
	double lat, lng, GMTbase;

	for (int i = 0;i < 4;i++)
	{
		table.DV[i] = length(res.DeltaV_LVLH[i]) / 0.3048;
		table.DVVector[i] = res.DeltaV_LVLH[i] / 0.3048;
		table.GETIG[i] = res.T_M[i];
		table.GETTH[i] = opt.TH[i];
		table.MVR[i] = "";
		table.AC[i] = 0.0;
		table.HPC[i] = 0.0;
		table.LIG[i] = 0.0;
	}

	table.LAT_LLS = opt.Lat_LS*DEG;
	table.LONG_LLS = opt.Lng_LS*DEG;
	table.MODE = opt.MODE;

	GMTbase = floor(opt.GETbase);
	table.GMTV = OrbMech::GETfromMJD(opt.sv0.MJD, GMTbase);
	table.GETV = OrbMech::GETfromMJD(opt.sv0.MJD, opt.GETbase);

	if (opt.MODE == 1)
	{
		if (opt.IDO == -1)
		{
			table.MVR[0] = "PC";
			table.MVR[1] = "DOI";
		}
		else if (opt.IDO == 1)
		{
			table.MVR[0] = "ASP";
			table.MVR[1] = "CIA";
			table.MVR[2] = "DOI";
		}
		else
		{
			table.MVR[0] = "PCC";
			table.MVR[1] = "DOI";
		}
	}
	if (opt.MODE == 2)
	{
		if (opt.IDO == 0)
		{
			table.MVR[0] = "ASH";
			table.MVR[1] = "DOI";
		}
		else
		{
			table.MVR[0] = "CIR";
			table.MVR[1] = "DOI";
		}
	}
	else if (opt.MODE == 3)
	{
		table.MVR[0] = "ASH";
		table.MVR[1] = "CIA";
		table.MVR[2] = "DOI";
	}
	else if (opt.MODE == 4)
	{
		table.MVR[0] = "DOI";
	}
	else if (opt.MODE == 5)
	{
		if (opt.IDO == -1)
		{
			table.MVR[0] = "PC";
			table.MVR[1] = "HO1";
			table.MVR[2] = "HO2";
		}
		else if (opt.IDO == 0)
		{
			table.MVR[0] = "HO1";
			table.MVR[1] = "PC";
			table.MVR[2] = "HO2";
		}
		else
		{
			table.MVR[0] = "HO1";
			table.MVR[1] = "HO2";
			table.MVR[2] = "PC";
		}
		table.MVR[3] = "DOI";
	}
	else if (opt.MODE == 7)
	{
		table.MVR[0] = "PPC";
	}

	SV sv_ig;
	double r_apo, r_peri;

	sv_ig = opt.sv0;

	for (int i = 0;i < res.i;i++)
	{
		sv_ig = coast(sv_ig, res.T_M[i] - OrbMech::GETfromMJD(sv_ig.MJD, opt.GETbase));
		OrbMech::latlong_from_BRCS(SystemParameters.MAT_J2000_BRCS, sv_ig.R, sv_ig.MJD, sv_ig.gravref, lat, lng);
		table.LIG[i] = lng * DEG;
		sv_ig.V += tmul(OrbMech::LVLH_Matrix(sv_ig.R, sv_ig.V), res.DeltaV_LVLH[i]);

		OrbMech::periapo(sv_ig.R, sv_ig.V, OrbMech::mu_Moon, r_apo, r_peri); //TBD

		table.AC[i] = (r_apo - opt.R_LS) / 1852.0;
		table.HPC[i] = (r_peri - opt.R_LS) / 1852.0;
	}

	table.PD_GETTH = opt.T_PD;
	table.PD_GETIG = res.t_PDI;
	table.PD_GETTD = res.t_Land;

	if (opt.I_AZ == 0)
	{
		sprintf(table.DescAzMode, "OPT");
	}
	else
	{
		sprintf(table.DescAzMode, "DES");
	}
	table.DescAsc = res.azi*DEG;
	table.PD_ThetaIgn = opt.theta_D*DEG;
}

void RTCC::EMGLMRAT(VECTOR3 X_P, VECTOR3 Y_P, VECTOR3 Z_P, VECTOR3 X_B, VECTOR3 Y_B, VECTOR3 Z_B, double &Pitch, double &Yaw, double &Roll, double &PB, double &YB, double &RB)
{
	//X_P, Y_P, Z_P: Unit vectors of platform (stable member) axes in BRCS coordinates
	//X_B, Y_B, Z_B: Unit vectors of body (navigation base) axes in BRCS coordinates
	double dot, sin_YB;
	dot = dotp(X_B, Y_P);
	if (abs(dot) > 1.0 - 0.0017)
	{
		Yaw = 0.0;
		Pitch = atan2(dotp(Z_B, X_P), dotp(Z_B, Z_P));
	}
	else
	{
		Yaw = atan2(dotp(-Z_B, Y_P), dotp(Y_B, Y_P));
		Pitch = atan2(dotp(-X_B, Z_P), dotp(X_B, X_P));
	}
	Roll = asin(dot);
	PB = atan2(sin(Yaw)*sin(Roll)*cos(Pitch) + cos(Yaw)*sin(Pitch), -sin(Yaw)*sin(Roll)*sin(Pitch) + cos(Yaw)*cos(Pitch));
	sin_YB = -sin(Yaw)*cos(Roll);
	YB = asin(sin_YB);
	RB = atan2(tan(Roll), cos(Yaw));

	if (Roll < 0)
	{
		Roll += PI2;
	}
	if (Pitch < 0)
	{
		Pitch += PI2;
	}
	if (Yaw < 0)
	{
		Yaw += PI2;
	}
	if (RB < 0)
	{
		RB += PI2;
	}
	if (PB < 0)
	{
		PB += PI2;
	}
	if (YB < 0)
	{
		YB += PI2;
	}
}

int RTCC::RMMEACC(int L, int ref_frame, int ephem_type, int rev0)
{
	int rev, rev_max = 24;
	double lng_des, GMT_min, GMT_cross;

	OrbitEphemerisTable *ephemeris;
	CapeCrossingTable *cctab;

	if (ephem_type == 0)
	{
		if (L == RTCC_MPT_CSM)
		{
			ephemeris = &EZEPH1;
			cctab = &EZCCSM;
		}
		else
		{
			ephemeris = &EZEPH2;
			cctab = &EZCLEM;
		}
	}
	else
	{
		//TBD
		return 1;
	}

	//Reset table
	*cctab = CapeCrossingTable();

	//Get entire ephemeris and convert to ECT or MCT
	EphemerisDataTable2 EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;

	ELFECH(ephemeris->EPHEM.Header.TL, ephemeris->EPHEM.Header.NumVec, 0, L, EPHEM, MANTIMES, LUNSTAY);
	
	if (EPHEM.Header.NumVec < 9)
	{
		//Error
		return 1;
	}

	EphemerisDataTable2 EPHEM2;
	EphemerisData2 temp_out;
	VECTOR3 W_ES, V_true, h_true;
	double sign;
	int out;

	if (ref_frame == 0)
	{
		W_ES = _V(0, 0, 1)*OrbMech::w_Earth;
		sign = 1.0;
		out = 1;
	}
	else
	{
		W_ES = _V(0, 0, 1)*OrbMech::w_Moon;
		sign = -1.0;
		out = 3;
	}

	for (unsigned int j = 0;j < EPHEM.Header.NumVec;j++)
	{
		if (ELVCNV(EPHEM.table[j], 0, out, temp_out))
		{
			//Error
			return 1;
		}
		
		V_true = temp_out.V - crossp(W_ES, temp_out.R);
		h_true = crossp(unit(temp_out.R), unit(V_true));

		if (h_true.z*sign < 0.0)
		{
			//Ground track reversal
			break;
		}

		EPHEM2.table.push_back(temp_out);
	}

	if (EPHEM2.table.size() < 9)
	{
		//Error
		return 1;
	}

	//Write header
	EPHEM2.Header.CSI = out;
	EPHEM2.Header.NumVec = EPHEM2.table.size();
	EPHEM2.Header.Offset = 0;
	EPHEM2.Header.Status = 0;
	EPHEM2.Header.TL = EPHEM2.table.front().GMT;
	EPHEM2.Header.TR = EPHEM2.table.back().GMT;
	EPHEM2.Header.TUP = EPHEM.Header.TUP;
	EPHEM2.Header.VEH = EPHEM.Header.VEH;

	if (out == 1)
	{
		lng_des = -80.0*RAD;
	}
	else
	{
		lng_des = -180.0*RAD;
	}

	cctab->TUP = EPHEM2.Header.TUP;
	cctab->GMTEphemFirst = EPHEM2.table.front().GMT;
	cctab->GMTEphemLast = EPHEM2.table.back().GMT;

	GMT_min = EPHEM2.table.front().GMT;

	//Use current time as start of current rev
	rev = 0;
	cctab->GMTCross[rev] = GMT_min;

	EphemerisData2 sv_cross;

	while (rev < rev_max)
	{
		if (RLMTLC(EPHEM2, MANTIMES, lng_des, GMT_min, GMT_cross, sv_cross, &LUNSTAY))
		{
			break;
		}

		rev++;
		cctab->GMTCross[rev] = GMT_cross;

		GMT_min = GMT_cross + 3600.0;
	}

	cctab->NumRevFirst = rev0;
	cctab->NumRevLast = rev0 + rev;
	cctab->NumRev = rev + 1;
	cctab->ref_body = ref_frame;

	return 0;
}

int RTCC::RMMASCND(EphemerisDataTable2 &EPHEM, ManeuverTimesTable &MANTIMES, double GMT_min, double &lng_asc)
{
	if (EPHEM.Header.CSI != 1 && EPHEM.Header.CSI != 3) return 5;
	double GMT;

	ELVCTRInputTable intab;
	ELVCTROutputTable2 outtab;
	EphemerisData2 sv_true;
	double mu;

	if (EPHEM.Header.CSI == 1)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}

	GMT = GMT_min;
	intab.GMT = GMT;
	ELVCTR(intab, outtab, EPHEM, MANTIMES);
	if (outtab.ErrorCode)
	{
		return 1;
	}

	sv_true = outtab.SV;

	double u, du, a, n, dt;
	OELEMENTS coe;

	coe = OrbMech::coe_from_sv(sv_true.R, sv_true.V, mu);
	if (coe.e >= 1.0)
	{
		return 4;
	}
	u = coe.TA + coe.w;
	if (u > PI2)
	{
		u = u - PI2;
	}
	du = PI2 - u;
	a = OrbMech::GetSemiMajorAxis(sv_true.R, sv_true.V, mu);
	n = sqrt(mu / pow(a, 3));
	dt = du / n;
	GMT += dt;

	int i, imax;
	i = 0;
	imax = 10;

	while (abs(dt) >= 2.0 && i < imax)
	{
		intab.GMT = GMT;
		ELVCTR(intab, outtab, EPHEM, MANTIMES);
		if (outtab.ErrorCode)
		{
			return 1;
		}
		sv_true = outtab.SV;
		coe = OrbMech::coe_from_sv(sv_true.R, sv_true.V, mu);
		if (coe.e >= 1.0)
		{
			return 4;
		}
		u = coe.TA + coe.w;
		if (u >= PI2)
		{
			u = u - PI2;
		}
		if (abs(u) > PI)
		{
			if (u > 0)
			{
				du = u - PI2;
			}
			else
			{
				du = u + PI2;
			}
		}
		else
		{
			du = u;
		}
		n = coe.h / pow(length(sv_true.R), 2);
		dt = -du / n;
		GMT += dt;
		i++;
	}

	if (i >= imax)
	{
		lng_asc = u;
		return 3;
	}

	double lat;
	OrbMech::latlong_from_r(sv_true.R, lat, lng_asc);

	if (EPHEM.Header.CSI == 1)
	{
		lng_asc -= OrbMech::w_Earth*sv_true.GMT;
		OrbMech::normalizeAngle(lng_asc, false);
	}

	return 0;
}

int RTCC::EMMENV(EphemerisDataTable2 &ephemeris, ManeuverTimesTable &MANTIMES, double GMT_begin, int option, SunriseSunsetTable &table, VECTOR3 *u_inter)
{
	//option 0: Sun, 1: Moon, 2: Plane intersection Earth, 3: Plane intersection Moon
	ELVCTRInputTable interin;
	ELVCTROutputTable2 interout;
	SunriseSunsetData data;
	EphemerisData2 sv_cur;
	VECTOR3 R_EM, V_EM, R_ES, R_MS, s;
	double R_E, cos_theta, cos_theta_old, GMT_old, GMT;
	unsigned iter = 0, iter_start;
	bool sight, sight_old, rise;
	int dataset = 0;
	int counter = 0;
	int iter2 = 0;

	//Return error if no ephemeris is available
	if (ephemeris.table.size() == 0) return 1;

	//Return error if moonrise/moonset data is requested and the spacecraft is in lunar orbit
	if (option == 1 && ephemeris.Header.CSI != 0)
	{
		return 1;
	}

	//Find vector at starting GMT
	while (ephemeris.table[iter].GMT < GMT_begin)
	{
		iter++;
	}

	iter_start = iter;
	table.num = 0;

	//Find environment change
	while (iter < ephemeris.table.size())
	{
		sv_cur = ephemeris.table[iter];

		if (option >= 2)
		{
			EphemerisData2 sv_out;
			int out;
			//0 = ECI, 1 = ECT, 2 = MCI, 3 = MCT, 4 = EMP
			if (option == 2)
			{
				out = 0;
			}
			else
			{
				out = 2;
			}

			ELVCNV(sv_cur, ephemeris.Header.CSI, out, sv_out);
			cos_theta = dotp(sv_cur.R, *u_inter);
			sight = (cos_theta >= 0.0);
		}
		else
		{
			PLEFEM(1, sv_cur.GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL);
			if (option == 0)
			{
				if (ephemeris.Header.CSI == 0)
				{
					R_MS = R_ES;
					R_E = OrbMech::R_Earth;
				}
				else
				{
					R_MS = R_ES - R_EM;
					R_E = OrbMech::R_Moon;
				}
			}
			else
			{
				R_MS = R_EM;
				R_E = OrbMech::R_Earth;
			}

			sight = OrbMech::sight(sv_cur.R, R_MS, R_E);
			s = unit(R_MS);

			cos_theta = length(sv_cur.R - s * dotp(sv_cur.R, s)) - R_E;
		}

		
		if (iter == iter_start)
		{
			sight_old = sight;
		}
		if (sight != sight_old)
		{
			if (sight)
			{
				//sunrise
				rise = true;
			}
			else
			{
				//sunset
				rise = false;
			}

			//Environment change found

			iter2 = 0;

			while (abs(sv_cur.GMT - GMT_old) > 1.5)
			{
				GMT = sv_cur.GMT - cos_theta * (sv_cur.GMT - GMT_old) / (cos_theta - cos_theta_old);
				cos_theta_old = cos_theta;
				GMT_old = sv_cur.GMT;

				interin.GMT = GMT;
				ELVCTR(interin, interout, ephemeris, MANTIMES);
				if (interout.ErrorCode)
				{
					return 4;
				}
				sv_cur = interout.SV;

				cos_theta = length(sv_cur.R - s * dotp(sv_cur.R, s)) - R_E;
				iter2++;
				if (iter2 > 20)
				{
					return 2;
				}
			}

			if (rise)
			{
				data.GETSR = GETfromGMT(GMT);
			}
			else
			{
				data.GETSS = GETfromGMT(GMT);
			}

			VECTOR3 R_scal = unit(sv_cur.R)*R_E;
			cos_theta_old = length(R_scal - s * dotp(R_scal, s)) - R_E;
			GMT_old = sv_cur.GMT;
			GMT = GMT_old + 10.0;

			interin.GMT = GMT;
			ELVCTR(interin, interout, ephemeris, MANTIMES);
			if (interout.ErrorCode)
			{
				return 4;
			}
			sv_cur = interout.SV;

			R_scal = unit(sv_cur.R)*R_E;
			cos_theta = length(R_scal - s * dotp(R_scal, s)) - R_E;
			iter2 = 0;

			while (abs(sv_cur.GMT - GMT_old) > 1.5)
			{
				GMT = sv_cur.GMT - cos_theta * (sv_cur.GMT - GMT_old) / (cos_theta - cos_theta_old);
				cos_theta_old = cos_theta;
				GMT_old = sv_cur.GMT;

				interin.GMT = GMT;
				ELVCTR(interin, interout, ephemeris, MANTIMES);
				if (interout.ErrorCode)
				{
					return 4;
				}
				sv_cur = interout.SV;
				R_scal = unit(sv_cur.R)*R_E;
				cos_theta = length(R_scal - s * dotp(R_scal, s)) - R_E;
				iter2++;
				if (iter2 > 20)
				{
					return 2;
				}
			}

			if (rise)
			{
				data.GETTR = GETfromGMT(GMT);
			}
			else
			{
				data.GETTS = GETfromGMT(GMT);
			}
			dataset++;
			if (dataset >= 2)
			{
				dataset = 0;
				table.data[counter] = data;
				counter++;
			}
			if (counter >= 8)
			{
				break;
			}
		}

		sight_old = sight;
		cos_theta_old = cos_theta;
		GMT_old = sv_cur.GMT;
		iter++;
	}

	table.num = counter;

	return 0;
}

void RTCC::EMDSSEMD(int ind, double param)
{
	EZSSTAB.errormessage = "";

	if (PZMPTCSM.CommonBlock.TUP < 0)
	{
		EZSSTAB.errormessage = "TABLE BEING UPDATED";
		return;
	}

	if (PZMPTCSM.CommonBlock.TUP != EZCCSM.TUP)
	{
		EZSSTAB.errormessage = "INCONSISTENT TABLES";
		return;
	}

	double GMT_begin, get, Pitch, Yaw;

	if (ind == 1)
	{
		GMT_begin = GMTfromGET(param);
	}
	else
	{
		int rev = (int)param;
		if (rev < EZCCSM.NumRevFirst || rev > EZCCSM.NumRevLast)
		{
			return;
		}
		GMT_begin = CapeCrossingGMT(1, rev);
		if (GMT_begin < RTCCPresentTimeGMT())
		{
			GMT_begin = RTCCPresentTimeGMT();
		}
	}

	for (int i = 0;i < 8;i++)
	{
		EZSSTAB.data[i].REV = 0;
		EZSSTAB.data[i].GETTR = 0.0;
		EZSSTAB.data[i].GETSR = 0.0;
		EZSSTAB.data[i].theta_SR = 0.0;
		EZSSTAB.data[i].psi_SR = 0.0;
		EZSSTAB.data[i].GETTS = 0.0;
		EZSSTAB.data[i].GETSS = 0.0;
		EZSSTAB.data[i].theta_SS = 0.0;
		EZSSTAB.data[i].psi_SS = 0.0;
	}

	//Get entire ephemeris from GMT begin on
	EphemerisDataTable2 EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;
	ELFECH(GMT_begin, 10000, 1, RTCC_MPT_CSM, EPHEM, MANTIMES, LUNSTAY);
	if (EPHEM.Header.NumVec < 9)
	{
		EZSSTAB.errormessage = "DATA NOT AVAILABLE";
		return;
	}
	//Convert first state vector to MCI
	int out;
	if (DetermineSVBody(EPHEM.table[0]) == BODY_EARTH)
	{
		out = 0;
	}
	else
	{
		out = 2;
	}
	EphemerisDataTable2 EPHEM2;
	ELVCNV(EPHEM.table, 0, out, EPHEM2.table);
	EPHEM2.Header = EPHEM.Header;
	EPHEM2.Header.CSI = out;

	if (EMMENV(EPHEM2, MANTIMES, GMT_begin, 0, EZSSTAB))
	{
		return;
	}

	if (EZSSTAB.num == 0) return;
	if (EZSSTAB.num < 8)
	{
		EZSSTAB.errormessage = "END OF AVAILABLE DATA";
	}

	for (int i = 0;i < EZSSTAB.num;i++)
	{
		if (EZSSTAB.data[i].GETSR > EZSSTAB.data[i].GETSS)
		{
			get = EZSSTAB.data[i].GETSS;
		}
		else
		{
			get = EZSSTAB.data[i].GETSR;
		}

		EZSSTAB.data[i].REV = CapeCrossingRev(1, GMTfromGET(get));
		ECMPAY(EPHEM2, MANTIMES, GMTfromGET(EZSSTAB.data[i].GETSR), true, Pitch, Yaw);
		EZSSTAB.data[i].theta_SR = Pitch * DEG;
		EZSSTAB.data[i].psi_SR = Yaw * DEG;
		ECMPAY(EPHEM2, MANTIMES, GMTfromGET(EZSSTAB.data[i].GETSS), true, Pitch, Yaw);
		EZSSTAB.data[i].theta_SS = Pitch * DEG;
		EZSSTAB.data[i].psi_SS = Yaw * DEG;
	}
}

void RTCC::EMDSSMMD(int ind, double param)
{
	EZMMTAB.errormessage = "";

	if (PZMPTCSM.CommonBlock.TUP < 0)
	{
		EZMMTAB.errormessage = "TABLE BEING UPDATED";
		return;
	}

	if (PZMPTCSM.CommonBlock.TUP != EZCCSM.TUP)
	{
		EZMMTAB.errormessage = "INCONSISTENT TABLES";
		return;
	}

	double GMT_begin, get, Pitch, Yaw;

	if (ind == 1)
	{
		GMT_begin = GMTfromGET(param);
	}
	else
	{
		int rev = (int)param;
		if (rev < EZCCSM.NumRevFirst || rev > EZCCSM.NumRevLast)
		{
			return;
		}
		GMT_begin = CapeCrossingGMT(1, rev);
		if (GMT_begin < RTCCPresentTimeGMT())
		{
			GMT_begin = RTCCPresentTimeGMT();
		}
	}

	for (int i = 0;i < 8;i++)
	{
		EZMMTAB.data[i].REV = 0;
		EZMMTAB.data[i].GETTR = 0.0;
		EZMMTAB.data[i].GETSR = 0.0;
		EZMMTAB.data[i].theta_SR = 0.0;
		EZMMTAB.data[i].psi_SR = 0.0;
		EZMMTAB.data[i].GETTS = 0.0;
		EZMMTAB.data[i].GETSS = 0.0;
		EZMMTAB.data[i].theta_SS = 0.0;
		EZMMTAB.data[i].psi_SS = 0.0;
	}

	//Get entire ephemeris from GMT begin on
	EphemerisDataTable2 EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;
	ELFECH(GMT_begin, 10000, 1, RTCC_MPT_CSM, EPHEM, MANTIMES, LUNSTAY);
	if (EPHEM.Header.NumVec < 9)
	{
		EZMMTAB.errormessage = "DATA NOT AVAILABLE";
		return;
	}

	if (EMMENV(EPHEM, MANTIMES, GMT_begin, 1, EZMMTAB))
	{
		return;
	}

	if (EZMMTAB.num == 0) return;
	if (EZMMTAB.num < 8)
	{
		EZMMTAB.errormessage = "END OF AVAILABLE DATA";
	}

	for (int i = 0;i < EZMMTAB.num;i++)
	{
		if (EZMMTAB.data[i].GETSR > EZMMTAB.data[i].GETSS)
		{
			get = EZMMTAB.data[i].GETSS;
		}
		else
		{
			get = EZMMTAB.data[i].GETSR;
		}

		EZMMTAB.data[i].REV = CapeCrossingRev(1, GMTfromGET(get));
		ECMPAY(EPHEM, MANTIMES, GMTfromGET(EZMMTAB.data[i].GETSR), false, Pitch, Yaw);
		EZMMTAB.data[i].theta_SR = Pitch * DEG;
		EZMMTAB.data[i].psi_SR = Yaw * DEG;
		ECMPAY(EPHEM, MANTIMES, GMTfromGET(EZMMTAB.data[i].GETSS), false, Pitch, Yaw);
		EZMMTAB.data[i].theta_SS = Pitch * DEG;
		EZMMTAB.data[i].psi_SS = Yaw * DEG;
	}
}

int RTCC::CapeCrossingRev(int L, double GMT)
{
	CapeCrossingTable *table;
	if (L == RTCC_MPT_LM)
	{
		table = &EZCLEM;
	}
	else
	{
		table = &EZCCSM;
	}

	if (table->NumRev == 0)
	{
		return 1;
	}

	if (GMT < table->GMTCross[0])
	{
		return table->NumRevFirst - 1;
	}
	else if (GMT > table->GMTCross[table->NumRev - 1])
	{
		return table->NumRevLast;
	}

	for (int i = 0;i < table->NumRev;i++)
	{
		if (table->GMTCross[i] > GMT)
		{
			return i + table->NumRevFirst - 1;
		}
	}

	return 1;
}

double RTCC::CapeCrossingGMT(int L, int rev)
{
	CapeCrossingTable *table;
	if (L == RTCC_MPT_LM)
	{
		table = &EZCLEM;
	}
	else
	{
		table = &EZCCSM;
	}

	if (table->NumRev == 0)
	{
		return -1.0;
	}

	if (rev < table->NumRevFirst || rev >table->NumRevLast)
	{
		return -1.0;
	}

	return table->GMTCross[rev - table->NumRevFirst];
}

double RTCC::CapeCrossingFirst(int L)
{
	CapeCrossingTable *table;
	if (L == RTCC_MPT_LM)
	{
		table = &EZCLEM;
	}
	else
	{
		table = &EZCCSM;
	}

	if (table->NumRev == 0)
	{
		return -1.0;
	}

	return table->GMTCross[0];
}

double RTCC::CapeCrossingLast(int L)
{
	CapeCrossingTable *table;
	if (L == RTCC_MPT_LM)
	{
		table = &EZCLEM;
	}
	else
	{
		table = &EZCCSM;
	}

	if (table->NumRev == 0)
	{
		return -1.0;
	}

	return table->GMTCross[table->NumRev - 1];
}

void RTCC::ECMPAY(EphemerisDataTable2 &EPH, ManeuverTimesTable &MANTIMES, double GMT, bool sun, double &Pitch, double &Yaw)
{
	ELVCTRInputTable interin;
	ELVCTROutputTable2 interout;
	EphemerisData2 sv_out;
	VECTOR3 R_EM, V_EM, R_ES, R_VS, X_B, X_L, Y_L, Z_L;

	interin.GMT = GMT;
	ELVCTR(interin, interout, EPH, MANTIMES);
	if (interout.ErrorCode)
	{
		return;
	}
	sv_out = interout.SV;
	//Get sun/moon vectors
	PLEFEM(1, GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL);
	if (sun)
	{
		if (EPH.Header.CSI == 0)
		{
			R_VS = R_ES - sv_out.R;
		}
		else
		{
			R_VS = (R_ES - R_EM) - sv_out.R;
		}
	}
	else
	{
		if (EPH.Header.CSI == 0)
		{
			R_VS = R_EM - sv_out.R;
		}
		else
		{
			R_VS = -sv_out.R;
		}
	}

	X_B = unit(R_VS);
	Z_L = unit(-sv_out.R);
	Y_L = unit(crossp(sv_out.V, sv_out.R));
	X_L = crossp(Y_L, Z_L);

	Pitch = asin(dotp(-X_B, Z_L));
	Yaw = atan2(dotp(X_B, Y_L), dotp(X_B, X_L));
}

int RTCC::NewMPTTrajectory(int L, SV &sv0)
{
	if (MPTHasManeuvers(L))
	{
		MissionPlanTable *tab;
		if (L == RTCC_MPT_CSM)
		{
			tab = &PZMPTCSM;
		}
		else
		{
			tab = &PZMPTLEM;
		}

		sv0.R = tab->mantable.back().R_BO;
		sv0.V = tab->mantable.back().V_BO;
		sv0.MJD = OrbMech::MJDfromGET(tab->mantable.back().GMT_BO,SystemParameters.GMTBASE);
		if (tab->mantable.back().RefBodyInd == BODY_EARTH)
		{
			sv0.gravref = oapiGetObjectByName("Earth");
		}
		else
		{
			sv0.gravref = oapiGetObjectByName("Moon");
		}
		sv0.mass = tab->mantable.back().TotalMassAfter;
	}
	else
	{
		//SV at specified time
		double GMT = RTCCPresentTimeGMT();
		EphemerisData sv;
		if (ELFECH(GMT, L, sv))
		{
			return 1;
		}
		sv0.R = sv.R;
		sv0.V = sv.V;
		sv0.MJD = OrbMech::MJDfromGET(sv.GMT,SystemParameters.GMTBASE);
		sv0.gravref = GetGravref(sv.RBI);

		PLAWDTInput pin;
		PLAWDTOutput pout;
		pin.T_UP = GMT;
		pin.TableCode = L;
		PLAWDT(pin, pout);
		sv0.mass = pout.ConfigWeight;
	}

	return 0;
}

int RTCC::PMQREAP(const std::vector<TradeoffData> &TOdata)
{
	TradeoffData empty;
	unsigned i, j;

	//Clear tradeoff data
	RTETradeoffTable.curves = 0;
	for (i = 0;i < 10;i++)
	{
		RTETradeoffTable.NumInCurve[i] = 0;
		for (j = 0;j < 44;j++)
		{
			RTETradeoffTable.data[i][j] = empty;
		}
	}

	if (TOdata.size() == 0) return 1;

	double TZData[10];
	bool found;

	for (i = 0;i < TOdata.size();i++)
	{
		//Check if the data belongs to an already existing curve
		found = false;
		for (j = 0;j < RTETradeoffTable.curves;j++)
		{
			//Is data within an hour of already existing curve?
			if (abs(TZData[j] - TOdata[i].T_Z) < 1.0)
			{
				//Save for a closer estimate next time around
				TZData[j] = TOdata[i].T_Z;

				if (RTETradeoffTable.NumInCurve[j] < 44)
				{
					//Store data
					RTETradeoffTable.data[j][RTETradeoffTable.NumInCurve[j]] = TOdata[i];
					RTETradeoffTable.NumInCurve[j]++;
				}
				found = true;
				break;
			}
		}
		//Didn't find one, it's a new set of data
		if (found == false && RTETradeoffTable.curves < 10)
		{
			TZData[RTETradeoffTable.curves] = TOdata[i].T_Z;
			RTETradeoffTable.NumInCurve[RTETradeoffTable.curves] = 1;

			//Store data
			RTETradeoffTable.data[RTETradeoffTable.curves][0] = TOdata[i];

			RTETradeoffTable.curves++;
		}
	}

	return 0;
}

void RTCC::PMMREAP(int med)
{
	//Near Earth Tradeoff 
	if (med == 70 || med == 71)
	{
		EphemerisData sv0, sv1;

		if (EMSFFV(GMTfromGET(PZREAP.RTEVectorTime*3600.0), RTCC_MPT_CSM, sv0))
		{
			return;
		}

		EMSMISSInputTable in;
		EphemerisDataTable2 EPHEM;

		double dt = PZREAP.RTET0Min*3600.0 - GETfromGMT(sv0.GMT);

		in.AnchorVector = sv0;
		in.CutoffIndicator = 3;
		in.EarthRelStopParam = 400000.0*0.3048;
		in.IgnoreManueverNumber = 0;
		in.ManCutoffIndicator = 2;
		in.MaxIntegTime = abs(dt);
		if (dt < 0)
		{
			in.IsForwardIntegration = -1.0;
		}
		in.ManeuverIndicator = false;
		in.MoonRelStopParam = 0.0;
		in.StopParamRefFrame = 2;
		in.VehicleCode = RTCC_MPT_CSM;

		EMSMISS(&in);

		if (in.NIAuxOutputTable.TerminationCode != 1)
		{
			return;
		}

		sv1 = in.NIAuxOutputTable.sv_cutoff;

		double mu;

		if (sv1.RBI == BODY_EARTH)
		{
			mu = OrbMech::mu_Earth;
		}
		else
		{
			mu = OrbMech::mu_Moon;
		}

		//Calculate maximum time difference
		double T_P = OrbMech::period(sv1.R, sv1.V, mu);
		dt = (PZREAP.RTET0Max - PZREAP.RTET0Min)*3600.0;
		if (T_P < dt)
		{
			dt = T_P;
		}
		if (dt > 24.0*3600.0)
		{
			dt = 24.0*3600.0;
		}

		in.AnchorVector = sv1;
		in.EphemerisBuildIndicator = true;
		in.ECIEphemerisIndicator = true;
		in.EphemerisLeftLimitGMT = sv1.GMT;
		in.EphemerisRightLimitGMT = sv1.GMT + dt;
		in.ECIEphemTableIndicator = &EPHEM;
		in.IsForwardIntegration = 1.0;

		EMSMISS(&in);

		if (in.NIAuxOutputTable.ErrorCode)
		{
			return;
		}

		EphemerisDataTable2 EarthTable, MoonTable;
		EphemerisData2 sv_m;

		//Sort into Earth and Moon tables
		for (unsigned int ii = 0;ii < EPHEM.table.size();ii++)
		{
			if (ELVCNV(EPHEM.table[ii], 0, 2, sv_m))
			{
				return;
			}

			if (length(sv_m.R) < 9.0*OrbMech::R_Moon)
			{
				//Moon centered
				MoonTable.table.push_back(sv_m);
			}
			else
			{
				//Earth centered
				EarthTable.table.push_back(EPHEM.table[ii]);
			}
		}

		//Write headers
		if (EarthTable.table.size() > 0)
		{
			EarthTable.Header.CSI = 0;
			EarthTable.Header.NumVec = EarthTable.table.size();
			EarthTable.Header.Offset = EarthTable.Header.Status = 0;
			EarthTable.Header.TL = EarthTable.table.front().GMT;
			EarthTable.Header.TR = EarthTable.table.back().GMT;
			EarthTable.Header.TUP = 1;
			EarthTable.Header.VEH = 1;
		}

		if (MoonTable.table.size() > 0)
		{
			MoonTable.Header.CSI = 2;
			MoonTable.Header.NumVec = MoonTable.table.size();
			MoonTable.Header.Offset = MoonTable.Header.Status = 0;
			MoonTable.Header.TL = MoonTable.table.front().GMT;
			MoonTable.Header.TR = MoonTable.table.back().GMT;
			MoonTable.Header.TUP = 1;
			MoonTable.Header.VEH = 1;
		}

		int mode;
		if (PZREAP.RTEIsPTPSite)
		{
			mode = 2;
		}
		else
		{
			mode = 4;
		}

		std::vector<TradeoffData>todata;

		if (EarthTable.table.size() == 0) return;

		ConicRTEEarthNew rteproc(this, EarthTable.table);
		rteproc.READ(mode,SystemParameters.GMTBASE, PZREAP.TZMIN, PZREAP.TZMAX);
		rteproc.Init(PZREAP.DVMAX, PZREAP.EntryProfile, PZREAP.IRMAX, PZREAP.VRMAX, PZREAP.RRBIAS, PZREAP.TGTLN);

		std::vector<ATPData> line;
		ATPData data;

		double lat;

		if (mode == 4)
		{
			for (int i = 0;i < 5;i++)
			{
				lat = PZREAP.ATPCoordinates[PZREAP.RTESiteNum][i * 2];

				if (lat > 1e8)
				{
					break;
				}

				data.lat = lat;
				data.lng = PZREAP.ATPCoordinates[PZREAP.RTESiteNum][i * 2 + 1];

				line.push_back(data);
			}
		}

		if (line.size() == 0)
		{
			return;
		}

		rteproc.ATP(line);
		rteproc.MAIN();
		todata = rteproc.TOData;
		PMQREAP(todata);
		PMDTRDFF(med, PZREAP.RTETradeoffRemotePage);
	}
	//Generation of Abort Scan Table for unspecified area	
	else if (med == 75 || med == 76 || med == 77)
	{
		PMMREAST(med);
	}
	else if (med == 80)
	{
		PMMREDIG(true);
	}
}

bool RTCC::DetermineRTESite(std::string Site)
{
	bool found = false;

	for (int i = 0;i < 5;i++)
	{
		if (Site == PZREAP.ATPSite[i])
		{
			PZREAP.RTESite = Site;
			PZREAP.RTESiteNum = i;
			PZREAP.RTEIsPTPSite = false;
			found = true;
			break;
		}
		if (Site == PZREAP.PTPSite[i])
		{
			PZREAP.RTESite = Site;
			PZREAP.RTESiteNum = i;
			PZREAP.RTEIsPTPSite = true;
			found = true;
			break;
		}
	}
	return found;
}

void RTCC::PMMREAST(int med, EphemerisData *sv)
{
	//Check if any AST slots are available
	int i;
	bool astfree = false;
	for (i = 0;i < 7;i++)
	{
		if (PZREAP.AbortScanTableData[i].ASTCode == 0)
		{
			astfree = true;
			break;
		}
	}

	if (astfree == false)
	{
		return;
	}
	int ASTCode = i;

	//Convert MED, fetch state vector, propagate to TIG
	EphemerisData sv0, sv_abort;

	if (sv)
	{
		sv0 = *sv;
	}
	else
	{
		if (EMSFFV(PZREAP.RTEVectorTime*3600.0, RTCC_MPT_CSM, sv0))
		{
			return;
		}
	}

	EMMENIInputTable emmeniin;
	emmeniin.AnchorVector = sv0;
	emmeniin.MaxIntegTime = PZREAP.RTET0Min*3600.0 - sv0.GMT;
	if (emmeniin.MaxIntegTime > 0)
	{
		emmeniin.IsForwardIntegration = true;
	}
	else
	{
		emmeniin.IsForwardIntegration = false;
	}
	emmeniin.CutoffIndicator = 1;

	EMMENI(emmeniin);
	if (emmeniin.TerminationCode != 1)
	{
		return;
	}
	sv_abort = emmeniin.sv_cutoff;

	//Lunar search only for Moon reference
	if (med == 77 && sv_abort.RBI == BODY_EARTH)
	{
		return;
	}

	EphemerisData sv_ig, sv_r;
	ASTData AST;
	char typname[8];

	//Entry profile handling
	std::string EntryProfile;
	int EPI;

	EntryProfile = med_f75_f77.EntryProfile;

	if (PZREAP.TGTLN == 1)
	{
		EPI = 2;
	}
	else
	{
		if (EntryProfile == "HB1")
		{
			EPI = 1;
		}
		else
		{
			EPI = 0;
		}
	}

	//Here the logic diverts between Earth vs. Moon centered state vectors
	if (sv_abort.RBI == BODY_EARTH)
	{
		SV sv_abort2 = ConvertEphemDatatoSV(sv_abort);
		double dvmax, TZMINI;
		int critical;

		if (med == 75)
		{
			dvmax = med_f75.DVMAX*0.3048;
			if (med_f75.Type == "TCUA")
			{
				critical = 2;
				sprintf_s(typname, "TCUA");
			}
			else if (med_f75.Type == "FCUA")
			{
				critical = 3;
				sprintf_s(typname, "FCUA");
			}
			else
			{
				return;
			}
			TZMINI = 0.0;
		}
		else
		{
			critical = 1;
			dvmax = PZREAP.DVMAX*0.3048;
			if (PZREAP.RTEIsPTPSite)
			{
				sprintf_s(typname, "PTP");
			}
			else
			{
				sprintf_s(typname, "ATP");
			}
			
			TZMINI = PZREAP.RTETimeOfLanding*3600.0;
		}

		RTEEarth rte(this, sv_abort, GetGMTBase(), SystemParameters.MCLAMD, PZREAP.RTET0Min*3600.0, TZMINI, critical);
		rte.READ(PZREAP.RRBIAS, dvmax, EPI, PZREAP.VRMAX*0.3048);

		if (critical == 1)
		{
			double LINE[10];
			if (PZREAP.RTEIsPTPSite)
			{
				for (int i = 0;i < 5;i++)
				{
					LINE[2 * i] = 80.0*RAD - 40.0*RAD*(double)i;	//So table has 80, 40, 0, -40, -80 deg as latitude
					LINE[2 * i + 1] = PZREAP.PTPLongitude[PZREAP.RTESiteNum];
				}
			}
			else
			{
				for (int i = 0;i < 10;i++)
				{
					LINE[i] = PZREAP.ATPCoordinates[PZREAP.RTESiteNum][i];
				}
			}
			rte.ATP(LINE);
		}

		bool stop = false;
		while (!stop)
		{
			stop = rte.EntryIter();
		}

		sv_ig = sv_abort;
		sv_r.R = rte.R_r;
		sv_r.V = rte.V_r;
		sv_r.GMT = rte.t2;
		sv_r.RBI = BODY_EARTH;
		AST.lat_SPL = rte.EntryLatcor;
		AST.lng_SPL = rte.EntryLngcor;
		AST.SplashdownGMT = rte.t_Z;
		AST.DV = rte.DV;
		AST.h_PC = 0.0;
	}
	else
	{
		int SMODE;
		double Inclination, TZMINI, TZMAXI;

		Inclination = med_f75_f77.Inclination;

		if (med == 75)
		{
			if (med_f75.Type == "FCUA")
			{
				SMODE = 16;
				sprintf_s(typname, "FCUA");
			}
			else
			{
				return;
			}
			
			TZMINI = PZREAP.TZMIN*3600.0;
			TZMAXI = PZREAP.TZMAX*3600.0;

			//Convert to GMT
			TZMINI = GMTfromGET(TZMINI);
			TZMAXI = GMTfromGET(TZMAXI);
		}
		else if (med == 76)
		{
			SMODE = 14;
			if (PZREAP.RTEIsPTPSite)
			{
				sprintf_s(typname, "PTP");
			}
			else
			{
				sprintf_s(typname, "ATP");
			}
			TZMINI = PZREAP.RTETimeOfLanding*3600.0;
			TZMAXI = 0.0;
		}
		else
		{
			if (med_f77.Site == "FCUA")
			{
				SMODE = 36;
				sprintf_s(typname, "FCUA");
				TZMINI = PZREAP.TZMIN*3600.0;
				TZMAXI = PZREAP.TZMAX*3600.0;

				//Convert to GMT
				TZMINI = GMTfromGET(TZMINI);
				TZMAXI = GMTfromGET(TZMAXI);
			}
			else
			{
				SMODE = 34;
				if (PZREAP.RTEIsPTPSite)
				{
					sprintf_s(typname, "PTP");
				}
				else
				{
					sprintf_s(typname, "ATP");
				}
				TZMINI = PZREAP.RTETimeOfLanding*3600.0;
				TZMAXI = 0.0;
			}
		}

		EphemerisData2 sv_abort2;
		sv_abort2.R = sv_abort.R;
		sv_abort2.V = sv_abort.V;
		sv_abort2.GMT = sv_abort.GMT;
		RTEMoon rte(this, sv_abort2, GetGMTBase(), SystemParameters.MCLAMD);
		if (SMODE == 14 || SMODE == 34)
		{
			double LINE[10];
			if (PZREAP.RTEIsPTPSite)
			{
				for (int i = 0;i < 5;i++)
				{
					LINE[2 * i] = 80.0*RAD - 40.0*RAD*(double)i;	//So table has 80, 40, 0, -40, -80 deg as latitude
					LINE[2 * i + 1] = PZREAP.PTPLongitude[PZREAP.RTESiteNum];
				}
			}
			else
			{
				for (int i = 0;i < 10;i++)
				{
					LINE[i] = PZREAP.ATPCoordinates[PZREAP.RTESiteNum][i];
				}
			}
			rte.ATP(LINE);
		}

		rte.READ(SMODE, PZREAP.IRMAX, PZREAP.VRMAX, PZREAP.RRBIAS, PZREAP.MOTION, PZREAP.HMINMC, EPI, 0.3, PZREAP.DVMAX, 0.0, Inclination, 1.0*1852.0, TZMINI, TZMAXI);
		if (rte.MASTER() == false)
		{
			return;
		}

		sv_ig.R = rte.sv0.R;
		sv_ig.V = rte.sv0.V;
		sv_ig.GMT = rte.sv0.GMT;
		sv_ig.RBI = BODY_MOON;
		sv_r.R = rte.R_EI;
		sv_r.V = rte.V_EI;
		sv_r.GMT = rte.t_R;
		sv_r.RBI = BODY_EARTH;
		AST.lat_SPL = rte.EntryLatcor;
		AST.lng_SPL = rte.EntryLngcor;
		AST.SplashdownGMT = rte.t_Landing;
		AST.DV = rte.DV;
		AST.h_PC = rte.FlybyPeriAlt;
	}

	if (PZREAP.LastASTCode == 0)
	{
		AST.ASTCode = 101;
	}
	else
	{
		AST.ASTCode = PZREAP.LastASTCode + 1;
	}
	PZREAP.LastASTCode = AST.ASTCode;
	AST.StationID = PZMPTCSM.StationID; //TBD
	AST.AbortGMT = sv_ig.GMT;

	EphemerisData sv_r_ECT;
	ELVCNV(sv_r, 1, sv_r_ECT);
	OELEMENTS coe = OrbMech::coe_from_sv(sv_r_ECT.R, sv_r_ECT.V, OrbMech::mu_Earth);
	double r_r, v_r, lat_r, lng_r, gamma_r, azi_r;
	PICSSC(true, sv_r.R, sv_r.V, r_r, v_r, lat_r, lng_r, gamma_r, azi_r);
	AST.incl_EI = coe.i;
	if (azi_r >= -PI05 && azi_r <= PI05)
	{
		AST.incl_EI = -AST.incl_EI;
	}
	AST.dv = length(AST.DV);
	AST.v_EI = length(sv_r.V);
	AST.gamma_EI = gamma_r;
	AST.ReentryGMT = sv_r.GMT;
	AST.VectorGMT = sv0.GMT;

	char ref;
	char discr[8], ModeName[16];

	if (sv_abort.RBI == BODY_EARTH)
	{
		ref = 'E';
	}
	else
	{
		ref = 'L';
	}
	if (med == 77)
	{
		sprintf(discr, "S");
	}
	else
	{
		if (med != 75)
		{
			sprintf(discr, "D");
		}
		else
		{
			sprintf(discr, "");
		}
	}
	sprintf(ModeName, "%c%s%s", ref, discr, typname);
	AST.AbortMode.assign(ModeName);
	AST.ReentryMode = EPI;
	AST.MissDistance = 0.0;
	if (med == 76 || (med == 77 && med_f77.Site != "FCUA"))
	{
		AST.SiteID = PZREAP.RTESite;
	}
	else
	{
		AST.SiteID = "";
	}
	AST.GLevel = 0.0;
	AST.RRBIAS = PZREAP.RRBIAS;
	AST.sv_EI.R = sv_r.R;
	AST.sv_EI.V = sv_r.V;
	AST.sv_EI.GMT = sv_r.GMT;
	AST.sv_IG = sv_ig;
	PZREAP.AbortScanTableData[ASTCode] = AST;

	//Conic solutions
	/*VECTOR3 DV, R_r, V_r;
	double dvmax, T_r, tzmin, tzmax;
	//0 = TCUA, 1 = FCUA, 3 = PTP discrete, 5 = ATP discrete
	int Mode;

	//Determine inputs
	if (med == 75)
	{
		dvmax = med_f75.DVMAX;
		if (med_f75.Type == "TCUA")
		{
			Mode = 0;
		}
		else if (med_f75.Type == "FCUA")
		{
			Mode = 1;
		}
		else
		{
			return;
		}
		tzmin = GMTfromGET(PZREAP.TZMIN);
		tzmax = GMTfromGET(PZREAP.TZMAX);
	}
	else
	{
		dvmax = PZREAP.DVMAX;
		if (PZREAP.RTEIsPTPSite)
		{
			Mode = 3;
		}
		else
		{
			Mode = 5;
		}
		tzmin = PZREAP.RTETimeOfLanding - 12.0;
		tzmax = PZREAP.RTETimeOfLanding + 12.0;
	}

	if (sv_abort.RBI == BODY_EARTH)
	{
		std::vector<EphemerisData> SVArray;
		SVArray.push_back(sv_abort);
		ConicRTEEarthNew rteproc(this, SVArray);

		rteproc.READ(Mode, SystemParameters.GMTBASE, tzmin, tzmax);
		rteproc.Init(dvmax, PZREAP.EntryProfile, PZREAP.IRMAX, PZREAP.VRMAX, PZREAP.RRBIAS, PZREAP.TGTLN);

		if (Mode == 4 || Mode == 5)
		{
			std::vector<ATPData> line;
			ATPData data;
			double lat;
			for (int i = 0;i < 5;i++)
			{
				lat = PZREAP.ATPCoordinates[PZREAP.RTESiteNum][i * 2];

				if (lat > 1e8)
				{
					break;
				}

				data.lat = lat;
				data.lng = PZREAP.ATPCoordinates[PZREAP.RTESiteNum][i * 2 + 1];

				line.push_back(data);
			}

			if (line.size() == 0)
			{
				return;
			}

			rteproc.ATP(line);
		}

		rteproc.MAIN();

		if (rteproc.SolData.NOSOLN == 1)
		{
			return;
		}

		DV = rteproc.SolData.DV;
		T_r = rteproc.SolData.T_r;
		R_r = rteproc.SolData.RR;
		V_r = rteproc.SolData.U_r;
	}
	else
	{

	}

	double r, v1, v2, lat, lng, gamma1, gamma2, azi1, azi2;
	PICSSC(true, sv_abort.R, sv_abort.V, r, v1, lat, lng, gamma1, azi1);
	PICSSC(true, sv_abort.R, sv_abort.V + DV, r, v2, lat, lng, gamma2, azi2);

	EphemerisData2 XIN;
	ASTInput ARIN;
	ASTSettings IRIN;

	XIN.R = sv_abort.R;
	XIN.V = sv_abort.V;
	XIN.GMT = sv_abort.GMT;

	ARIN.dgamma = gamma2 - gamma1;
	ARIN.dpsi = azi2 - azi1;
	ARIN.dv = v2 - v1;
	ARIN.T_a = sv_abort.GMT;
	ARIN.T_r = T_r;

	EphemerisData2 sv_r, sv_r_ECT;
	sv_r.R = R_r;
	sv_r.V = V_r;
	sv_r.GMT = T_r;
	ELVCNV(sv_r, 0, 1, sv_r_ECT);
	PICSSC(true, sv_r_ECT.R, sv_r_ECT.V, r, v1, lat, lng, gamma1, azi1);

	ARIN.h_r_des = r - OrbMech::R_Earth;
	ARIN.lat_r_des = lat;
	ARIN.lng_r_des = lng;
	ARIN.azi_r_des = azi1;
	ARIN.dt_ar = ARIN.T_r - ARIN.T_a;
	ARIN.gamma_des = ARIN.gamma_stop = gamma1;
	ARIN.R_E_equ = ARIN.R_E_pole = OrbMech::R_Earth;
	ARIN.RRBIAS = PZREAP.RRBIAS;
	ARIN.w_E = OrbMech::w_Earth;
	//TBD: Reentry area and weight
	//TBD: PLA lat/lng
	ARIN.GMTBASE = GetGMTBase();
	
	if (Mode == 1)
	{
		IRIN.FuelCritical = true;
	}
	else
	{
		IRIN.FuelCritical = false;
	}
	IRIN.ReentryMode = 10;
	IRIN.Ref = sv_abort.RBI;
	IRIN.ReentryStop = 1;
	IRIN.EntryProfile = PZREAP.EntryProfile;
	IRIN.MissDistanceInd = 0;

	ASTData AST;
	int IER;

	//int IER, IPRT;
	PMMDAB(XIN, ARIN, IRIN, AST, IER, 0);

	if (IER)
	{
		return;
	}

	if (PZREAP.LastASTCode == 0)
	{
		AST.ASTCode = 101;
	}
	else
	{
		AST.ASTCode = PZREAP.LastASTCode + 1;
	}
	PZREAP.LastASTCode = AST.ASTCode;
	AST.StationID = PZMPTCSM.StationID; //TBD
	AST.VectorGMT = sv0.GMT;

	char ref;
	char discr[8], typname[8], ModeName[16];

	if (sv0.RBI == BODY_EARTH)
	{
		ref = 'E';
	}
	else
	{
		ref = 'L';
	}
	if (med == 77)
	{
		sprintf(discr, "S");
	}
	else
	{
		if (Mode == 3 || Mode == 5)
		{
			sprintf(discr, "D");
		}
		else
		{
			sprintf(discr, "");
		}
	}
	//TBD
	if (Mode == 0)
	{
		sprintf(typname, "TCUA");
	}
	else if (Mode == 1)
	{
		sprintf(typname, "FCUA");
	}
	else
	{
		sprintf(typname, "ATP");
	}
	sprintf(ModeName, "%c%s%s", ref, discr, typname);
	AST.AbortMode.assign(ModeName);
	if (Mode > 1)
	{
		AST.SiteID = PZREAP.RTESite;
	}
	else
	{
		AST.SiteID = "";
	}
	PZREAP.AbortScanTableData[ASTCode] = AST;

	//Debug
	VECTOR3 DV_LVLH1, DV_LVLH2;
	MATRIX3 Q_Xx = OrbMech::LVLH_Matrix(sv_abort.R, sv_abort.V);
	DV_LVLH1 = mul(Q_Xx, DV) / 0.3048;
	DV_LVLH2 = mul(Q_Xx, AST.DV) / 0.3048;
	sprintf(oapiDebugString(), "DV1: %lf %lf %lf DV2: %lf %lf %lf", DV_LVLH1.x, DV_LVLH1.y, DV_LVLH1.z, DV_LVLH2.x, DV_LVLH2.y, DV_LVLH2.z);*/
}

void RTCC::PMMREDIG(bool mpt)
{
	RTEDMEDData MED;
	RTEDASTData AST;
	RTEDSPMData SPM;
	RTEDigitalSolutionTable RID;

	char Buff[64];
	sprintf_s(Buff, 5, "%s", med_f80.ManeuverCode.c_str());
	double gmt;

	if (med_f80.ASTCode == 0)
	{
		EphemerisData sv_fetch, sv_LVLH;
		gmt = GMTfromGET(med_f81.IgnitionTime);
		//Manual
		if (mpt)
		{
			int err = EMSFFV(gmt, RTCC_MPT_CSM, sv_fetch);
			if (err) return;
		}
		else
		{
			sv_fetch = VEHDATABUF.sv;
		}

		//Propagate state vector to TIG
		EMMENIInputTable emmeniin;
		emmeniin.AnchorVector = sv_fetch;
		emmeniin.MaxIntegTime = gmt - sv_fetch.GMT;
		if (emmeniin.MaxIntegTime > 0)
		{
			emmeniin.IsForwardIntegration = true;
		}
		else
		{
			emmeniin.IsForwardIntegration = false;
		}
		emmeniin.CutoffIndicator = 1;

		EMMENI(emmeniin);
		if (emmeniin.TerminationCode != 1)
		{
			return;
		}
		AST.sv_TIG = emmeniin.sv_cutoff;
		AST.gamma_r_stop = 0.0;

		//Calculate DV vector
		int coord_out;

		if (med_f81.RefBody == BODY_EARTH)
		{
			coord_out = 0;
		}
		else
		{
			coord_out = 2;
		}
		ELVCNV(AST.sv_TIG, coord_out, sv_LVLH);
		MATRIX3 Q_Xx = OrbMech::LVLH_Matrix(sv_LVLH.R, sv_LVLH.V);
		AST.DV = tmul(Q_Xx, med_f81.XDV);
		AST.lat_tgt = med_f81.lat_tgt;
		AST.lng_tgt = med_f81.lng_tgt;

		MED.ManualEntry = true;
		RID.ASTSolutionCode = "MAN";
		RID.LandingSiteID = "";
		RID.VectorGET = sv_fetch.GMT;
	}
	else
	{
		//AST
		bool found = false;
		int i;
		for (i = 0;i < 7;i++)
		{
			if (med_f80.ASTCode == PZREAP.AbortScanTableData[i].ASTCode)
			{
				found = true;
				break;
			}
		}
		if (found == false)
		{
			return;
		}
		ASTData astin = PZREAP.AbortScanTableData[i];

		EphemerisData2 sv_EI_ECT;
		double r_r, v_r, lat_r, lng_r, gamma_r, azi_r;

		ELVCNV(astin.sv_EI, 0, 1, sv_EI_ECT);
		PICSSC(true, sv_EI_ECT.R, sv_EI_ECT.V, r_r, v_r, lat_r, lng_r, gamma_r, azi_r);

		AST.azi_r = azi_r;
		AST.dt_ar = astin.sv_EI.GMT - astin.sv_IG.GMT;
		AST.DV = astin.DV;
		AST.gamma_r = 0.0; //Not needed
		AST.gamma_r_stop = gamma_r;
		AST.h_r = r_r - OrbMech::R_Earth;
		AST.lat_tgt = astin.lat_SPL;
		AST.lng_tgt = astin.lng_SPL;
		AST.lat_r = lat_r;
		AST.lng_r = lng_r - OrbMech::w_Earth*sv_EI_ECT.GMT;
		OrbMech::normalizeAngle(AST.lng_r, false);
		AST.sv_TIG = astin.sv_IG;
		AST.sv_r = astin.sv_EI;

		gmt = astin.sv_IG.GMT;//astin.VectorGMT;//

		MED.ManualEntry = false;
		RID.ASTSolutionCode = astin.AbortMode;
		RID.LandingSiteID = astin.SiteID;
		RID.VectorGET = astin.VectorGMT;
	}

	double lmascmass, lmdscmass;
	std::bitset<4> oldconfig;

	if (mpt)
	{
		PLAWDTInput inp;
		PLAWDTOutput outp;

		inp.T_UP = gmt;
		inp.TableCode = RTCC_MPT_CSM;
		PLAWDT(inp, outp);

		SPM.CSMWeight = outp.CSMWeight;
		lmascmass = outp.LMAscWeight;
		lmdscmass = outp.LMDscWeight;
		oldconfig = outp.CC;
	}
	else
	{
		SPM.CSMWeight = VEHDATABUF.csmmass;
		lmascmass = VEHDATABUF.lmascmass;
		lmdscmass = VEHDATABUF.lmdscmass;
		MPTGetConfigFromString(VEHDATABUF.config, oldconfig);
	}

	//Save MED values
	MED.StoppingMode = 1;
	MED.HeadsUp = med_f80.HeadsUp;
	MED.TrimInd = med_f80.TrimAngleInd;
	MED.Column = med_f80.Column;

	//Check configuration
	if (RTEManeuverCodeLogic(Buff, lmascmass, lmdscmass, med_f80.NumQuads, MED.Thruster, MED.AttitudeMode, MED.ConfigCode, MED.ManVeh, SPM.LMWeight))
	{
		//Illegal maneuver code
		PZREAP.RTEDTable[MED.Column - 1].Error = 11;
		return;
	}
	std::bitset<4> newconfig;
	newconfig = MED.ConfigCode;

	if (oldconfig != newconfig && MPTConfigSubset(oldconfig, newconfig) == false)
	{
		//Not a valid configuration
		PZREAP.RTEDTable[MED.Column - 1].Error = 12;
		return;
	}

	if (MED.Thruster == RTCC_ENGINETYPE_CSMSPS || MED.Thruster == RTCC_ENGINETYPE_LMDPS)
	{
		if (med_f80.NumQuads == 4)
		{
			MED.UllageThrusters = true;
		}
		else if (med_f80.NumQuads == 2)
		{
			MED.UllageThrusters = false;
		}
		else
		{
			//Invalid number of ullage quads
			PZREAP.RTEDTable[MED.Column - 1].Error = 13;
			return;
		}

		if (MED.ManVeh == RTCC_MANVEHICLE_CSM)
		{
			if (MED.UllageThrusters)
			{
				SPM.UllageThrust = SystemParameters.MCTCT2;
				SPM.UllageWLR = SystemParameters.MCTCW2;
			}
			else
			{
				SPM.UllageThrust = SystemParameters.MCTCT1;
				SPM.UllageWLR = SystemParameters.MCTCW1;
			}
		}
		else
		{
			if (MED.UllageThrusters)
			{
				SPM.UllageThrust = SystemParameters.MCTLT2;
				SPM.UllageWLR = SystemParameters.MCTLW2;
			}
			else
			{
				SPM.UllageThrust = SystemParameters.MCTLT1;
				SPM.UllageWLR = SystemParameters.MCTLW1;
			}
		}
		SPM.dt_ullage = med_f80.UllageDT; //Minus overlap?
	}
	else
	{
		MED.UllageThrusters = false;
		SPM.dt_ullage = 0.0;
	}
	
	SPM.dt_10PCT = SystemParameters.MCTDD4;
	SPM.DockingAngle = med_f80.DockingAngle - 60.0*RAD;
	SPM.KFactor = PZMPTCSM.KFactor;
	SPM.CMWeight = 0.0; //TBD
	SPM.CMArea = 0.0;//TBD
	SPM.TDPS = 0.1; //TBD

	EngineParametersTable(MED.Thruster, SPM.MainEngineThrust, SPM.MainEngineWLR, SPM.ComputerThrust);
	if (MED.Thruster == RTCC_ENGINETYPE_LMDPS)
	{
		SPM.DPS10PCTThrust = SystemParameters.MCTDT4;
		SPM.DPS10PCTWLR = SystemParameters.MCTDW4;
	}
	else
	{
		SPM.DPS10PCTThrust = 0.0;
		SPM.DPS10PCTWLR = 0.0;
	}

	//Entry
	SPM.lng_T = 0.0;
	SPM.GNBankAngle = 0.0;
	if (med_f82.PrimaryEP == "HGN")
	{
		MED.PrimaryReentryMode = 3;
		SPM.GNBankAngle = med_f82.PrimaryInitialBank;
	}
	else if (med_f82.PrimaryEP == "HB1")
	{
		MED.PrimaryReentryMode = 10;
		SPM.BankAngle = med_f82.PrimaryInitialBank;
		SPM.lng_T = med_f82.PrimaryLongT;
	}
	else if (med_f82.PrimaryEP == "HB2")
	{
		MED.PrimaryReentryMode = 6;
		SPM.BankAngle = PI;
	}
	else if (med_f82.PrimaryEP == "HB3")
	{
		MED.PrimaryReentryMode = 1;
		SPM.BankAngle = 0.0;
	}
	else if (med_f82.PrimaryEP == "HB4")
	{
		MED.PrimaryReentryMode = 6;
		SPM.BankAngle = 0.0;
	}
	else if (med_f82.PrimaryEP == "HB5")
	{
		MED.PrimaryReentryMode = 2;
		SPM.BankAngle = 0.0;
	}
	else
	{
		//Invalid entry mode
		PZREAP.RTEDTable[MED.Column - 1].Error = 14;
		return;
	}

	//Reentry settings
	if (MED.PrimaryReentryMode == 3)
	{
		if (med_f82.BackupEP == "HB1")
		{
			MED.BackupReentryMode = 10;
			SPM.BankAngle = med_f82.BackupInitialBank;
			SPM.lng_T = med_f82.BackupLongT;
		}
		else if (med_f82.BackupEP == "HB2")
		{
			MED.BackupReentryMode = 6;
			SPM.BankAngle = PI;
		}
		else if (med_f82.BackupEP == "HB3")
		{
			MED.BackupReentryMode = 1;
			SPM.BankAngle = 0.0;
		}
		else if (med_f82.BackupEP == "HB4")
		{
			MED.BackupReentryMode = 6;
			SPM.BankAngle = 0.0;
		}
		else if (med_f82.BackupEP == "HB5")
		{
			MED.BackupReentryMode = 2;
			SPM.BankAngle = 0.0;
		}
		else
		{
			//Invalid entry mode
			PZREAP.RTEDTable[MED.Column - 1].Error = 14;
			return;
		}
	}
	else
	{
		MED.BackupReentryMode = 0;
	}
	SPM.GLevelConstant = PZREAP.GMAX;
	SPM.GLevelReentryPrim = med_f82.PrimaryGLIT;
	SPM.GLevelReentryBackup = med_f82.BackupGLIT;
	if (med_f82.PrimaryRollDirection == "N")
	{
		SPM.RollDirectionPrim = 1.0;
	}
	else
	{
		SPM.RollDirectionPrim = -1.0;
	}
	if (med_f82.BackupRollDirection == "N")
	{
		SPM.RollDirectionBackup = 1.0;
	}
	else
	{
		SPM.RollDirectionBackup = -1.0;
	}

	//REFSMMAT handling
	MATRIX3 RFS;
	int refsnum = EMGSTGENCode(med_f80.REFSMMAT.c_str());

	if (refsnum != -1)
	{
		MED.IRM = -1;
		if (EZJGMTX1.data[refsnum - 1].ID == 0)
		{
			//No REFSMMAT available
			PZREAP.RTEDTable[MED.Column - 1].Error = 10;
			return;
		}
		RFS = EZJGMTX1.data[refsnum - 1].REFSMMAT;
	}
	else
	{
		if (med_f80.REFSMMAT == "ROP")
		{
			MED.IRM = 2;
		}
		else if (med_f80.REFSMMAT == "ROY")
		{
			MED.IRM = 0;
		}
		else if (med_f80.REFSMMAT == "ROZ")
		{
			MED.IRM = 1;
		}
		else if (med_f80.REFSMMAT == "TEI")
		{
			MED.IRM = 3;
		}
		else
		{
			return;
		}
	}

	int IRED;
	PMMPAB(MED, AST, SPM, RFS, RID, IRED);

	if (IRED)
	{
		PZREAP.RTEDTable[MED.Column - 1].Error = IRED;
		return;
	}

	//Convert some GETs and PETs
	RID.PETReference = SystemParameters.MCGREF * 3600.0;
	RID.GETI = GETfromGMT(RID.GMTI);
	RID.PETI = RID.GETI - RID.PETReference;
	RID.RollPET = GETfromGMT(RID.RollPET)- RID.PETReference;
	RID.ReentryPET = GETfromGMT(RID.ReentryPET) - RID.PETReference;
	RID.ImpactGET_bu = GETfromGMT(RID.ImpactGET_bu);
	RID.VectorGET = GETfromGMT(RID.VectorGET);
	RID.ImpactGET_max_lift = GETfromGMT(RID.ImpactGET_max_lift);
	RID.ImpactGET_zero_lift = GETfromGMT(RID.ImpactGET_zero_lift);
	RID.RollGETBackup = GETfromGMT(RID.RollGETBackup);
	RID.MaxGLevelGETBackup = GETfromGMT(RID.MaxGLevelGETBackup);
	RID.ImpactGET_prim = GETfromGMT(RID.ImpactGET_prim);

	//Write some alphanumeric data
	RID.ThrusterCode = MED.Thruster;
	if (PZREAP.LastRTEDCode == 0)
	{
		RID.RTEDCode = "Z1";
	}
	else
	{
		RID.RTEDCode = "Z" + std::to_string(PZREAP.LastRTEDCode + 1);
	}
	PZREAP.LastRTEDCode++;
	RID.ManeuverCode = med_f80.ManeuverCode;
	if (refsnum != -1)
	{
		FormatREFSMMATCode(refsnum, EZJGMTX1.data[refsnum - 1].ID, Buff);
		RID.SpecifiedREFSMMAT.assign(Buff);
	}
	else
	{
		RID.SpecifiedREFSMMAT = med_f80.REFSMMAT;
	}
	RID.PrimaryReentryMode = med_f82.PrimaryEP;
	RID.BackupReentryMode = med_f82.BackupEP;
	RID.NumQuads = med_f80.NumQuads;
	RID.REFSMMAT = RFS;

	//Determine configuration change
	if (oldconfig == newconfig)
	{
		RID.ConfigurationChangeIndicator = RTCC_CONFIGCHANGE_NONE;
	}
	else
	{
		RID.ConfigurationChangeIndicator = RTCC_CONFIGCHANGE_UNDOCKING;
	}
	RID.EndConfiguration = newconfig.to_ulong();

	//Finally, write to PZREAP
	PZREAP.RTEDTable[MED.Column - 1] = RID;

	//External DV display
	RMDRXDV(true);
}

//Return to Earth Digital Supervisor
void RTCC::PMMPAB(const RTEDMEDData &MED, const RTEDASTData &AST, const RTEDSPMData &SPM, MATRIX3 &RFS, RTEDigitalSolutionTable &RID, int &IRED)
{
	PCMATCArray outarray;
	double dv;//, Tmp, dt_BU, Isp;

	//Prepare data for the trajectory computer
	outarray.IASD = 1;
	outarray.ISTP = MED.StoppingMode;
	outarray.gamma_stop = AST.gamma_r_stop;
	outarray.GMTBASE = GetGMTBase();
	if (AST.sv_TIG.RBI == BODY_EARTH)
	{
		outarray.h_pc_on = false;
	}
	else
	{
		outarray.h_pc_on = true;
	}
	outarray.Vec3.z = SPM.CSMWeight;
	outarray.LMWeight = SPM.LMWeight;
	outarray.DockingAngle = SPM.DockingAngle;
	outarray.dt_ullage = SPM.dt_ullage;
	outarray.DT_10PCT = SPM.dt_10PCT;
	outarray.AttitudeCode = MED.AttitudeMode;
	outarray.ThrusterCode = MED.Thruster;
	outarray.UllageCode = MED.UllageThrusters;
	outarray.ConfigCode = MED.ConfigCode;
	outarray.TVC = MED.ManVeh;
	outarray.HeadsUpDownInd = MED.HeadsUp;
	outarray.TrimAngleInd = MED.TrimInd;

	dv = length(AST.DV);

	//Manual entry?
	if (MED.ManualEntry == false)
	{
		PMMMPTInput in;
		MPTManeuver man;
		MissionPlanTable mpt;

		in.Attitude = MED.AttitudeMode;
		in.CONFIG = MED.ConfigCode;
		in.CSMWeight = SPM.CSMWeight;
		in.CurrentManeuver = 1;
		in.DETU = SPM.dt_ullage;
		in.DockingAngle = SPM.DockingAngle;
		in.DPSScaleFactor = SystemParameters.MCTDTF;
		in.DT_10PCT = SPM.dt_10PCT;
		in.HeadsUpIndicator = MED.HeadsUp;
		in.IgnitionTimeOption = false;
		in.IterationFlag = med_f80.Iterate;
		in.LMWeight = SPM.LMWeight;
		in.LowerTimeLimit = 0.0;
		in.mpt = &mpt;
		in.sv_before = AST.sv_TIG;
		in.Thruster = MED.Thruster;
		in.TrimAngleInd = MED.TrimInd;
		in.UpperTimeLimit = 1e10;
		in.UT = MED.UllageThrusters;
		in.VC = MED.ManVeh;
		in.VehicleArea = 0.0;
		in.VehicleWeight = SPM.CSMWeight + SPM.LMWeight;
		in.V_aft = AST.sv_TIG.V + AST.DV;

		if (PMMMPT(in, man))
		{
			//Error: "Forward iterator" failed to converge
			IRED = 6;
			return;
		}

		int ITS;
		EphemerisData sv_ig;
		PMMCEN(AST.sv_TIG, 0.0, 0.0, 1, man.GMTMAN - AST.sv_TIG.GMT, 1.0, sv_ig, ITS);
		if (ITS != 1)
		{
			//Error: Coast NI failed backing up to ignition time
			IRED = 2;
			return;
		}

		std::vector<double> var, arr;
		void * varPtr;
		bool mode = false;

		var.resize(5);
		arr.resize(8);

		var[0] = man.dV_inertial.x*3600.0 / OrbMech::R_Earth;
		var[1] = man.dV_inertial.y*3600.0 / OrbMech::R_Earth;
		var[2] = man.dV_inertial.z*3600.0 / OrbMech::R_Earth;
		var[3] = 0.0;
		var[4] = 0.0;

		outarray.R0 = sv_ig.R;
		outarray.V0 = sv_ig.V;
		outarray.T0 = sv_ig.GMT;
		outarray.REF = sv_ig.RBI;
		varPtr = &outarray;

		PCMATC(var, varPtr, arr, mode);

		//Cheat a little
		outarray.R_r = AST.sv_r.R;
		outarray.V_r = AST.sv_r.V;
		outarray.T_r = AST.sv_r.GMT;

		/*
		//No. SPS or DPS? (APS is not supported)
		if (MED.Thruster == RTCC_ENGINETYPE_CSMSPS || MED.Thruster == RTCC_ENGINETYPE_LMDPS)
		{
			//Estimate burn time for DPS or SPS
			double DV_me;
			double DT[3], W[3], WDOT[3], DV[3], T[3];

			DT[0] = SPM.dt_ullage;
			Tmp = SPM.UllageWLR*SPM.dt_ullage;
			W[0] = SPM.CSMWeight + SPM.LMWeight;
			DV[0] = -SPM.UllageThrust*log(1.0 - Tmp / W[0]) / SPM.UllageWLR;
			DV_me = dv - DV[0];
			W[1] = W[0] - Tmp;
			WDOT[0] = SPM.UllageWLR;
			T[0] = SPM.UllageThrust;
			if (DV_me <= 0)
			{
				//Error: DV of main engine <= 0
				IRED = 1;
				return;
			}
			int n;
			if (MED.Thruster == RTCC_ENGINETYPE_CSMSPS)
			{
				DV[1] = DV_me;
				Tmp = DV[1] * SPM.MainEngineWLR / SPM.MainEngineThrust;//DV[1] * SPM.MainEngineThrust / SPM.MainEngineWLR;
				//DT[1] = W[1] * (1.0 + (exp(-Tmp) - 1.0) / Tmp) / SPM.MainEngineWLR;
				DT[1] = W[1] / SPM.MainEngineWLR*(1.0 - exp(-Tmp));
				WDOT[1] = SPM.MainEngineWLR;
				T[1] = SPM.MainEngineThrust;
				n = 2;
				if (DT[1] < SPM.TDPS)
				{
					n = 1; //?
				}
			}
			else
			{
				DT[1] = SPM.dt_10PCT;
				Tmp = SPM.DPS10PCTWLR*SPM.dt_10PCT;
				DV[1] = -SPM.DPS10PCTThrust*log(1.0 - Tmp / W[1]) / SPM.DPS10PCTWLR;
				WDOT[1] = SPM.DPS10PCTWLR;
				T[1] = SPM.DPS10PCTThrust;
				DV[2] = DV_me - DV[1];
				W[2] = W[1] - Tmp;
				Tmp = DV[2] * SPM.MainEngineWLR / SPM.MainEngineThrust;//DV[2] * SPM.MainEngineThrust / SPM.MainEngineWLR;
				//DT[2] = W[2] * (1.0 + (exp(-Tmp) - 1.0) / Tmp) / Tmp;
				DT[2] = W[2] / SPM.MainEngineWLR*(1.0 - exp(-Tmp));
				WDOT[2] = SPM.MainEngineWLR;
				T[2] = SPM.MainEngineThrust;
				n = 3;
				if (DT[2] < SPM.TDPS)
				{
					n = 1; //?
				}
			}
			double An, T_apo, Ad, DDV;
			int i;

			An = 0.0;
			T_apo = 0.0;
			Ad = 0.0;
			i = 0;
			do
			{
				Isp = T[i] / WDOT[i];
				DDV = log((W[i] - WDOT[i] * DT[i]) / W[i]);
				An = An + Isp * (DT[i] + (W[i] + WDOT[i] * T_apo)*DDV / WDOT[i]);
				Ad = Ad + Isp * DDV;
				T_apo = T_apo + DT[i];
				i++;
			} while (i < n);
			dt_BU = An / Ad;
		}
		else
		{
			//RCS
			Isp = SPM.MainEngineThrust / SPM.MainEngineWLR;
			Tmp = dv / Isp;
			dt_BU = (SPM.CSMWeight + SPM.LMWeight)*Isp*(1.0 + (exp(-Tmp) - 1.0) / Tmp) / SPM.MainEngineThrust;
		}
		//Integrate state vector backwards through dt_BU
		int ITS;
		EphemerisData sv_ig;
		PMMCEN(AST.sv_TIG, 0.0, 10.0*24.0*3600.0, 1, -dt_BU, 1.0, sv_ig, ITS);
		if (ITS != 1)
		{
			//Error: Coast NI failed backing up to ignition time
			IRED = 2;
			return;
		}

		//Rotate DV through 1/2 the burn arc
		//VECTOR3 X = unit(crossp(crossp(sv_ig.R, sv_ig.V), sv_ig.R));
		//VECTOR3 Y = unit(crossp(sv_ig.V, sv_ig.R));
		//VECTOR3 Z = -unit(sv_ig.R);
		//double DV_X = dotp(AST.DV, X);
		//double DV_Y = dotp(AST.DV, Y);
		//double DV_Z = dotp(AST.DV, Z);
		//VECTOR3 DV_P = X * DV_X + Z * DV_Z;
		//double theta_T = length(crossp(sv_ig.R, sv_ig.V))*length(AST.DV)*(SPM.CSMWeight+SPM.LMWeight) / (pow(length(sv_ig.R), 2)*SPM.ComputerThrust);
		//VECTOR3 DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, Y))*sin(theta_T / 2.0))*length(DV_P);
		VECTOR3 DV = AST.DV;//DV_C + Y * DV_Y;

		//Set up inputs for forward iterator
		bool err;
		void *constPtr;
		outarray.R0 = sv_ig.R;
		outarray.V0 = sv_ig.V;
		outarray.T0 = sv_ig.GMT;
		outarray.REF = sv_ig.RBI;
		outarray.T_r = AST.sv_TIG.GMT + AST.dt_ar;
		constPtr = &outarray;

		bool PCMATCPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
		bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &PCMATCPointer;

		GenIterator::GeneralizedIteratorBlock block;

		block.IndVarSwitch[0] = true;
		block.IndVarSwitch[1] = true;
		block.IndVarSwitch[2] = true;
		block.IndVarSwitch[3] = true;
		block.IndVarGuess[0] = DV.x*3600.0 / OrbMech::R_Earth;
		block.IndVarGuess[1] = DV.y*3600.0 / OrbMech::R_Earth;
		block.IndVarGuess[2] = DV.z*3600.0 / OrbMech::R_Earth;
		block.IndVarGuess[3] = 0.0;
		block.IndVarGuess[4] = (AST.sv_TIG.GMT + AST.dt_ar) / 3600.0;
		block.IndVarStep[0] = pow(2, -19);
		block.IndVarStep[1] = pow(2, -19);
		block.IndVarStep[2] = pow(2, -19);
		block.IndVarStep[3] = pow(2, -19);
		block.IndVarWeight[0] = 1.0;
		block.IndVarWeight[1] = 1.0;
		block.IndVarWeight[2] = 1.0;
		block.IndVarWeight[3] = 1.0;

		block.DepVarSwitch[0] = true;
		block.DepVarSwitch[1] = true;
		block.DepVarSwitch[2] = true;
		block.DepVarSwitch[3] = true;
		block.DepVarSwitch[4] = true;
		if (MED.StoppingMode == -1)
		{
			block.DepVarSwitch[5] = true;
			block.DepVarWeight[5] = 1.0;
		}

		block.DepVarLowerLimit[0] = (AST.h_r - 400.0*0.3048) / OrbMech::R_Earth;
		block.DepVarLowerLimit[1] = AST.lat_r - 0.02*RAD;
		block.DepVarLowerLimit[2] = AST.lng_r - 0.02*RAD;
		block.DepVarLowerLimit[3] = AST.azi_r - 0.01*RAD;
		block.DepVarLowerLimit[4] = (AST.dt_ar - 0.2) / 3600.0;
		block.DepVarLowerLimit[5] = AST.gamma_r - 0.001*RAD;
		block.DepVarUpperLimit[0] = (AST.h_r + 400.0*0.3048) / OrbMech::R_Earth;
		block.DepVarUpperLimit[1] = AST.lat_r + 0.02*RAD;
		block.DepVarUpperLimit[2] = AST.lng_r + 0.02*RAD;
		block.DepVarUpperLimit[3] = AST.azi_r + 0.01*RAD;
		block.DepVarUpperLimit[4] = (AST.dt_ar + 0.2) / 3600.0;
		block.DepVarUpperLimit[5] = AST.gamma_r + 0.001*RAD;
		block.DepVarClass[0] = 1;
		block.DepVarClass[1] = 1;
		block.DepVarClass[2] = 1;
		block.DepVarClass[3] = 1;
		block.DepVarClass[4] = 1;//2;
		block.DepVarClass[5] = 1;
		//block.DepVarWeight[4] = 1.0;

		std::vector<double> result;
		std::vector<double> y_vals;
		err = GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);*/

		if (outarray.ErrInd)
		{
			if (outarray.ErrInd == 1)
			{
				//Error: Coast NI failed in PMMATC
				IRED = 8;
				return;
			}
			else
			{
				//Error: PMMRKJ failed in PMMATC
				IRED = 9;
				return;
			}
		}
	}
	else
	{
		//Manual entry
		std::vector<double> var, arr;
		void * varPtr;
		bool mode = false;

		var.resize(5);
		arr.resize(8);

		var[0] = AST.DV.x*3600.0 / OrbMech::R_Earth;
		var[1] = AST.DV.y*3600.0 / OrbMech::R_Earth;
		var[2] = AST.DV.z*3600.0 / OrbMech::R_Earth;
		var[3] = 0.0;
		var[4] = 0.0;

		outarray.R0 = AST.sv_TIG.R;
		outarray.V0 = AST.sv_TIG.V;
		outarray.T0 = AST.sv_TIG.GMT;
		outarray.REF = AST.sv_TIG.RBI;
		varPtr = &outarray;

		PCMATC(var, varPtr, arr, mode);

		if (outarray.ErrInd == 1)
		{
			//Error: Coast NI failed in PMMATC
			IRED = 8;
			return;
		}
		if (outarray.ErrInd == 2)
		{
			//Error: PMMRKJ failed in PMMATC
			IRED = 9;
			return;
		}
		if (length(outarray.R_r) > OrbMech::R_Earth + 400000.0*0.3048)
		{
			//Error: Manual entry didn't reenter
			IRED = 10;
			return;
		}

		//In manual mode the state vector was integrated to perigee, now go back up to 400k feet
		EphemerisData sv_peri, sv_r;
		int ITS;
		sv_peri.R = outarray.R_r;
		sv_peri.V = outarray.V_r;
		sv_peri.GMT = outarray.T_r;
		sv_peri.RBI = BODY_EARTH;
		PMMCEN(sv_peri, 0.0, 3600.0, 3, OrbMech::R_Earth + 400000.0*0.3048, -1.0, sv_r, ITS);
		if (ITS != 3)
		{
			//Error: Coast NI failed in PMMATC
			IRED = 8;
			return;
		}
		outarray.R_r = sv_r.R;
		outarray.V_r = sv_r.V;
		outarray.T_r = sv_r.GMT;
	}
	int ICC;
	PCRENT(outarray, MED, SPM, AST.lat_tgt, AST.lng_tgt, RID, ICC);

	if (ICC != 0)
	{
		//Error in conversion from ECI to ECT coordinates
		IRED = 7;
		return;
	}
	double mu;
	if (outarray.sv_CO.RBI == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}
	OELEMENTS coe = OrbMech::coe_from_sv(outarray.sv_CO.R, outarray.sv_CO.V, mu);
	double r_r, v_r, lat_r, lng_r, gamma_r, azi_r;
	EphemerisData2 sv_r, sv_r_ECT;
	sv_r.R = outarray.R_r;
	sv_r.V = outarray.V_r;
	sv_r.GMT = outarray.T_r;
	ELVCNV(sv_r, 0, 1, sv_r_ECT);
	PICSSC(true, sv_r_ECT.R, sv_r_ECT.V, r_r, v_r, lat_r, lng_r, gamma_r, azi_r);
	//Save a lot of data in the output table
	RID.VehicleWeight = outarray.W_on;
	RID.TrueAnomaly = coe.TA;
	RID.DVC = outarray.DVC;
	RID.dt = outarray.dt_ME + outarray.DT_TO;
	RID.dv = outarray.dv_total;
	RID.dt_ullage = outarray.dt_ullage;
	RID.PETI = outarray.GMT_BI;
	RID.GMTI = outarray.GMT_BI;
	RID.ReentryPET = outarray.T_r;
	RID.v_EI = v_r;
	RID.gamma_EI = gamma_r;
	RID.lat_EI = lat_r;
	RID.lng_EI = lng_r - OrbMech::w_Earth*sv_r_ECT.GMT;
	OrbMech::normalizeAngle(RID.lng_EI, false);
	RID.R_Y = outarray.R_Y;
	RID.R_Z = outarray.R_Z;
	VECTOR3 h = crossp(outarray.R_r, outarray.V_r);
	RID.Inclination = acos(h.z / length(h));

	VECTOR3 XSM, YSM, ZSM, XPH, YPH, ZPH;
	if (MED.IRM == -1)
	{
		//REFSMMAT was input
		XSM = _V(RFS.m11, RFS.m12, RFS.m13);
		YSM = _V(RFS.m21, RFS.m22, RFS.m23);
		ZSM = _V(RFS.m31, RFS.m32, RFS.m33);
	}
	else
	{
		//Burn related REFSMMAT
		VECTOR3 AT;
		double alpha, beta;

		AT = outarray.Vec1;
		alpha = outarray.R_Z;
		beta = outarray.R_Y;

		if (MED.IRM == 3)
		{
			alpha = -alpha;
			beta = -beta;
		}

		YPH = unit(crossp(AT, outarray.sv_BI.R));
		ZPH = unit(crossp(AT, YPH));
		XSM = AT * cos(alpha)*cos(beta) - YPH * sin(alpha)*cos(beta) + ZPH * sin(beta);
		YSM = AT * sin(alpha) + YPH * cos(alpha);
		ZSM = unit(crossp(XSM, YSM));

		if (MED.IRM < 2)
		{
			double LVO;

			LVO = -1.0;
			if (MED.IRM == 1)
			{
				LVO = 1.0;
			}
			XSM = -XSM;
			YSM = YSM * LVO;
			ZSM = -ZSM * LVO;
		}
	}

	if (MED.IRM >= 0)
	{
		//Save REFSMMAT as output if it was calculated
		RFS = _M(XSM.x, XSM.y, XSM.z, YSM.x, YSM.y, YSM.z, ZSM.x, ZSM.y, ZSM.z);
	}
	//LVLH attitude
	ZPH = unit(outarray.sv_BI.R);
	YPH = unit(crossp(outarray.sv_BI.V, outarray.sv_BI.R));
	XPH = unit(crossp(YPH, ZPH));
	double P, Y, R;
	P = asin(-dotp(ZPH, outarray.X_B));
	if (abs(P - PI05) <= 10e-8)
	{
		R = 0.0;
		Y = atan2(-dotp(XPH, outarray.X_B), dotp(YPH, outarray.Y_B));
	}
	else
	{
		Y = atan2(dotp(YPH, outarray.X_B), dotp(XPH, outarray.X_B));
		R = atan2(dotp(ZPH, outarray.Y_B), dotp(ZPH, outarray.Z_B));
	}
	RID.LVLHAtt.x = R;
	RID.LVLHAtt.y = P;
	RID.LVLHAtt.z = Y;
	//FDAI attitude
	double Mid, C, Out, In;
	Mid = asin(dotp(YSM, outarray.X_B));
	C = abs(Mid);
	if (Mid < 0)
	{
		Mid += PI2;
	}
	if (abs(C - PI05) < 10e-8)
	{
		Out = 0.0;
		In = atan2(dotp(XSM, outarray.Z_B), dotp(ZSM, outarray.Z_B));
	}
	else
	{
		Out = atan2(-dotp(YSM, outarray.Z_B), dotp(YSM, outarray.Y_B));
		In = atan2(-dotp(ZSM, outarray.X_B), dotp(XSM, outarray.X_B));
	}
	if (Out < 0)
	{
		Out += PI2;
	}
	if (In < 0)
	{
		In += PI2;
	}
	if (MED.ManVeh == RTCC_MANVEHICLE_CSM)
	{
		RID.FDAIAtt.x = Out;
		RID.FDAIAtt.y = In;
		RID.FDAIAtt.z = Mid;
	}
	else
	{
		Y = asin(-cos(Mid)*sin(Out));
		if (Y < 0)
		{
			Y += PI2;
		}
		if (sin(Y) == 1.0)
		{
			RID.FDAIAtt.x = 0.0;
			RID.FDAIAtt.y = 0.0;
			RID.FDAIAtt.z = Y;
		}
		else
		{
			P = asin(sin(In)*cos(Out) / cos(Y) + cos(In)*sin(Mid)*sin(Out) / cos(Y));
			R = asin(sin(Mid) / cos(Y));
			if (P < 0)
			{
				P += PI2;
			}
			if (R < 0)
			{
				R += PI2;
			}
			RID.FDAIAtt.x = R;
			RID.FDAIAtt.y = P;
			RID.FDAIAtt.z = Y;
		}
	}
	RID.IMUAtt.x = Out;
	RID.IMUAtt.y = In;
	RID.IMUAtt.z = Mid;
	//Output remaining elements
	RID.KFactor = PZMPTCSM.KFactor; //TBD
	RID.CMWeight = 0.0; //TBD
	RID.dt_10PCT = outarray.DT_10PCT;
	RID.A_T = outarray.Vec1;
	RID.X_B = outarray.X_B;
	RID.Y_B = outarray.Y_B;
	RID.Z_B = outarray.Z_B;
	RID.dv_TO = outarray.Vec3.y;
	RID.dt_TO = outarray.DT_TO;
	RID.PostAbortWeight = outarray.W_end;
	RID.R_BI = outarray.sv_BI.R;
	RID.V_BI = outarray.sv_BI.V;
	RID.h_a = outarray.h_a;
	RID.h_p = outarray.h_p;
	RID.R_BO = outarray.sv_CO.R;
	RID.V_BO = outarray.sv_CO.V;
	RID.GMT_BO = outarray.sv_CO.GMT;
	RID.HeadsUpDownIndicator = med_f80.HeadsUp;

	RID.DV_XDV = PIEXDV(outarray.sv_BI.R, outarray.sv_BI.V, SPM.CSMWeight + SPM.LMWeight, SPM.ComputerThrust, outarray.Vec1 *outarray.dv_total, false);
	IRED = 0;
}

void RTCC::PCRENT(PCMATCArray &FD, const RTEDMEDData &IMD, const RTEDSPMData &SPS, double PHIMP, double LIMP, RTEDigitalSolutionTable &RED, int &ICC)
{
	//Store some initial outputs
	RED.PrimaryReentryMode = IMD.PrimaryReentryMode;
	RED.lat_imp_tgt = PHIMP;
	RED.lng_imp_tgt = LIMP;

	//Null outputs
	RED.RollPET = 0.0;
	RED.LiftVectorOrientation = 0.0;
	RED.MaxGLevelBackup = 0.0;
	RED.MaxGLevelGETBackup = 0.0;
	RED.MaxGLevelGMT = 0.0;
	RED.MaxGLevelPrimary = 0.0;
	RED.ImpactGET_bu = 0.0;
	RED.ImpactGET_max_lift = 0.0;
	RED.ImpactGET_prim = 0.0;
	RED.ImpactGET_zero_lift = 0.0;
	RED.lat_imp_2nd_max = 0.0;
	RED.lat_imp_2nd_min = 0.0;
	RED.lat_imp_bu = 0.0;
	RED.lat_imp_prim = 0.0;
	RED.GLevelRoll = 0.0;
	RED.md_lat = 0.0;
	RED.md_lat_bu = 0.0;
	RED.md_lng = 0.0;
	RED.md_lng_bu = 0.0;

	EphemerisData2 sv_r, sv_r_ECT;
	sv_r.R = FD.R_r;
	sv_r.V = FD.V_r;
	sv_r.GMT = FD.T_r;
	if (ELVCNV(sv_r, 0, 1, sv_r_ECT))
	{
		ICC = 1;
		return;
	}

	RMMYNIInputTable inp;
	RMMYNIOutputTable outp;
	double g_level;

	inp.R0 = sv_r_ECT.R;
	inp.V0 = sv_r_ECT.V;
	inp.GMT0 = sv_r_ECT.GMT;

	//Primary reentry mode is constant g iteration?
	if (IMD.PrimaryReentryMode == 10 && SPS.lng_T < PI2)
	{
		RMMGIT(sv_r_ECT, SPS.lng_T);
		g_level = 0.0; //TBD: Output from RMMGIT
	}
	else
	{
		inp.lat_T = PHIMP;
		inp.lng_T = LIMP;
		inp.KSWCH = IMD.PrimaryReentryMode;
		if (IMD.PrimaryReentryMode == 3)
		{
			inp.K1 = SPS.GNBankAngle;
		}
		else
		{
			inp.K1 = SPS.BankAngle;
		}
		inp.g_c_BU = inp.g_c_GN = SPS.GLevelReentryPrim;
		inp.D0 = SPS.GLevelConstant*9.80665;
		inp.RLDIR = SPS.RollDirectionPrim;
		RMMYNI(inp, outp);
		g_level = SPS.GLevelConstant;
	}

	if (outp.IEND == 2)
	{
		if (IMD.ManualEntry)
		{
			goto RTCC_PCRENT_3D;
		}
	RTCC_PCRENT_1A:
		//Store primary outputs
		RED.RollPET = outp.t_gc;
		RED.LiftVectorOrientation = inp.K1;
		RED.MaxGLevelGMT = outp.t_gmax;
		RED.MaxGLevelPrimary = outp.gmax;
		RED.ImpactGET_prim = outp.t_drogue;
		RED.lat_imp_prim = outp.lat_IP;
		RED.lng_imp_prim = outp.lng_IP;
		RED.GLevelRoll = g_level;
		RED.md_lat = outp.lat_IP - PHIMP;
		if (RED.md_lat > PI)
		{
			RED.md_lat -= PI2;
		}
		else if (RED.md_lat < -PI)
		{
			RED.md_lat += PI2;
		}
		RED.md_lat *= OrbMech::R_Earth;
		RED.md_lng = outp.lng_IP - LIMP;
		if (RED.md_lng > PI)
		{
			RED.md_lng -= PI2;
		}
		else if (RED.md_lng < -PI)
		{
			RED.md_lng += PI2;
		}
		RED.md_lng *= OrbMech::R_Earth;
	}

	if (outp.IEND == 3)
	{
		//TBD: Skipout
	}
	if (IMD.PrimaryReentryMode != 3)
	{
		goto RTCC_PCRENT_3B;
	}
	//Backup reentry mode is constant g iteration?
	if (IMD.BackupReentryMode == 10 && SPS.lng_T < PI2)
	{
		RMMGIT(sv_r_ECT, SPS.lng_T);
	}
	else
	{
		inp.lat_T = PHIMP;
		inp.lng_T = LIMP;
		inp.KSWCH = IMD.BackupReentryMode;
		inp.K1 = SPS.BankAngle;
		inp.g_c_BU = inp.g_c_GN = SPS.GLevelReentryBackup;
		inp.D0 = SPS.GLevelConstant*9.80665;
		inp.RLDIR = SPS.RollDirectionBackup;
		RMMYNI(inp, outp);
	}
	if (outp.IEND == 2)
	{
		//Store outputs
		RED.lat_imp_bu = outp.lat_IP;
		RED.lng_imp_bu = outp.lng_IP;
		RED.ImpactGET_bu = outp.t_drogue;
		RED.md_lat_bu = outp.lat_IP - PHIMP;
		if (RED.md_lat_bu > PI)
		{
			RED.md_lat_bu -= PI2;
		}
		else if (RED.md_lat_bu < -PI)
		{
			RED.md_lat_bu += PI2;
		}
		RED.md_lat_bu *= OrbMech::R_Earth;
		RED.md_lng_bu = outp.lng_IP - LIMP;
		if (RED.md_lng_bu > PI)
		{
			RED.md_lng_bu -= PI2;
		}
		else if (RED.md_lng_bu < -PI)
		{
			RED.md_lng_bu += PI2;
		}
		RED.md_lng_bu *= OrbMech::R_Earth;
	}
RTCC_PCRENT_3B:
	ICC = 0;
	return;

RTCC_PCRENT_3D:
	double r_r, v_r, lat_r, lng_r, gamma_r, azi_r;
	//Why ELVCNV here?
	PICSSC(true, sv_r_ECT.R, sv_r_ECT.V, r_r, v_r, lat_r, lng_r, gamma_r, azi_r);
	//Store lat_r and lng_r
	goto RTCC_PCRENT_1A;
}

bool RTCC::PMMLRBTI(EphemerisData sv)
{
	EMMENIInputTable in;
	in.StopParamRefFrame = 1;
	in.MoonRelStopParam = 0.0;
	in.CutoffIndicator = 4;
	in.AnchorVector = sv;
	in.MaxIntegTime = 5.0*24.0*3600.0;

	EphemerisData sv2;
	EMMENI(in);

	if (in.TerminationCode != 4)
	{
		PMXSPT("PMMLRBTI", 124);
		return true;
	}

	sv2 = in.sv_cutoff;

	rtcc::LOIOptions opt;
	opt.SPH = sv2;
	opt.dh_bias = PZLOIPLN.dh_bias*1852.0;
	opt.DV_maxm = med_k18.DVMAXm*0.3048;
	opt.DV_maxp = med_k18.DVMAXp*0.3048;
	opt.DW = PZLOIPLN.DW*RAD;
	opt.eta1 = PZLOIPLN.eta_1*RAD;
	opt.GMTBASE =SystemParameters.GMTBASE;
	opt.HA_LLS = PZLOIPLN.HA_LLS*1852.0;
	opt.HP_LLS = PZLOIPLN.HP_LLS*1852.0;
	opt.HA_LPO = med_k18.HALOI1*1852.0;
	opt.HP_LPO = med_k18.HPLOI1*1852.0;
	opt.lat_LLS= BZLAND.lat[0];
	opt.lng_LLS = BZLAND.lng[0];
	opt.psi_DS = med_k18.psi_DS*RAD;
	opt.psi_mn = med_k18.psi_MN*RAD;
	opt.psi_mx = med_k18.psi_MX*RAD;
	opt.REVS1 = PZLOIPLN.REVS1;
	opt.REVS2 = PZLOIPLN.REVS2;
	opt.R_LLS = BZLAND.rad[RTCC_LMPOS_BEST];
	opt.usePlaneSolnForInterSoln = PZLOIPLN.PlaneSolnForInterSoln;

	double h_pc = length(opt.SPH.R) - opt.R_LLS;

	//Store h_pc for (mainly) the MCC
	PZLRBTI.h_pc = h_pc / 1852.0;

	if (h_pc > opt.HA_LPO)
	{
		PMXSPT("PMMLRBTI", 126);
		return true;
	}
	if (h_pc < 0.0)
	{
		PMXSPT("PMMLRBTI", 125);
		return true;
	}

	rtcc::LOITargeting loi(this, opt);
	loi.MAIN();

	//Write message with time at LLS
	char Buffer[100], Buffer2[100];
	OrbMech::format_time_HHMMSS(Buffer, GETfromGMT(loi.out.T_LLS));
	sprintf(Buffer2, "Time above the landing site is %s", Buffer);
	RTCCONLINEMON.TextBuffer[0].assign(Buffer2);
	PMXSPT("PMMLRBTI", 201);

	PMDLRBTI(opt, loi.out);

	for (int i = 0;i < 8;i++)
	{
		PZLRBELM.sv_man_bef[i].R = loi.out.data[i].R_LOI;
		PZLRBELM.sv_man_bef[i].V = loi.out.data[i].V_LOI;
		PZLRBELM.sv_man_bef[i].GMT = loi.out.data[i].GMT_LOI;
		PZLRBELM.sv_man_bef[i].RBI = BODY_MOON;
		PZLRBELM.V_man_after[i] = loi.out.data[i].V_LOI_apo;
	}
	return false;
}

void RTCC::PMDLRBTI(const rtcc::LOIOptions &opt, const rtcc::LOIOutputData &out)
{
	PZLRBTI.VectorGET = med_k18.VectorTime;
	PZLRBTI.lat_lls = BZLAND.lat[0] * DEG;
	PZLRBTI.lng_lls = BZLAND.lng[0] * DEG;
	PZLRBTI.R_lls = BZLAND.rad[RTCC_LMPOS_BEST] / 1852.0;
	PZLRBTI.REVS1 = opt.REVS1;
	PZLRBTI.REVS2 = opt.REVS2;
	PZLRBTI.DHBIAS = opt.dh_bias / 1852.0;
	PZLRBTI.AZ_LLS = opt.psi_DS*DEG;
	PZLRBTI.f_LLS = opt.DW*DEG;
	if (PZLRBTI.f_LLS < 0)
	{
		PZLRBTI.f_LLS += 360.0;
	}
	PZLRBTI.HALOI1 = opt.HA_LPO / 1852.0;
	PZLRBTI.HPLOI1 = opt.HP_LPO / 1852.0;
	PZLRBTI.HALOI2 = opt.HA_LLS / 1852.0;
	PZLRBTI.HPLOI2 = opt.HP_LLS / 1852.0;
	PZLRBTI.DVMAXp = opt.DV_maxp / 0.3048;
	PZLRBTI.DVMAXm = opt.DV_maxm / 0.3048;
	PZLRBTI.planesoln = opt.usePlaneSolnForInterSoln;
	PZLRBTI.RARPGT = opt.RARPGT / 1852.0;
	PZLRBTI.AZMN_f_ND = out.eta_MN*DEG;
	if (PZLRBTI.AZMN_f_ND < 0)
	{
		PZLRBTI.AZMN_f_ND += 360.0;
	}
	PZLRBTI.AZMX_f_ND = out.eta_MX*DEG;
	if (PZLRBTI.AZMX_f_ND < 0)
	{
		PZLRBTI.AZMX_f_ND += 360.0;
	}
	for (int i = 0;i < 8;i++)
	{
		if (out.data[i].GMT_LOI == 0.0)
		{
			PZLRBTI.sol[i].GETLOI = 0.0;
			PZLRBTI.sol[i].DVLOI1 = 0.0;
			PZLRBTI.sol[i].DVLOI2 = 0.0;
			PZLRBTI.sol[i].H_ND = 0.0;
			PZLRBTI.sol[i].f_ND_H = 0.0;
			PZLRBTI.sol[i].H_PC = 0.0;
			PZLRBTI.sol[i].Theta = 0.0;
			PZLRBTI.sol[i].f_ND_E = 0.0;
		}
		else
		{
			PZLRBTI.sol[i].GETLOI = GETfromGMT(out.data[i].GMT_LOI);
			PZLRBTI.sol[i].DVLOI1 = length(out.data[i].V_LOI_apo - out.data[i].V_LOI) / 0.3048;
			PZLRBTI.sol[i].DVLOI2 = out.data[i].display.dv_LOI2 / 0.3048;
			PZLRBTI.sol[i].H_ND = out.data[i].display.H_ND / 1852.0;
			PZLRBTI.sol[i].f_ND_H = out.data[i].display.eta_N*DEG;
			if (PZLRBTI.sol[i].f_ND_H < 0)
			{
				PZLRBTI.sol[i].f_ND_H += 360.0;
			}
			PZLRBTI.sol[i].H_PC = out.data[i].display.h_P / 1852.0;
			PZLRBTI.sol[i].Theta = out.data[i].display.theta*DEG;
			PZLRBTI.sol[i].f_ND_E = out.data[i].display.W_P*DEG;
			if (PZLRBTI.sol[i].f_ND_E < 0)
			{
				PZLRBTI.sol[i].f_ND_E += 360.0;
			}
		}
	}
}

bool RTCC::GMGMED(char *str)
{
	int i = 0;

	if (str[i] == '\0')
	{
		return false;
	}

	bool skipdata = false;
	char medtype;
	medtype = str[i];
	i++;

	std::string code;

	do
	{
		code.push_back(str[i]);
		i++;
		//End of the string?
		if (str[i] == ';')
		{
			skipdata = true;
			break;
		}
	} while (str[i] != ',');

	if (code.size() == 0)
	{
		return false;
	}

	i++;

	std::string word;
	std::vector<std::string> MEDSequence;

	if (skipdata == false)
	{
		do
		{
			if (str[i] == ',')
			{
				MEDSequence.push_back(word);
				word.clear();
			}
			else
			{
				word.push_back(str[i]);
			}
			i++;
			if (str[i] == '\0' || str[i] == ';')
			{
				MEDSequence.push_back(word);
			}
		} while (str[i] != '\0' && str[i] != ';');
	}

	RTCCONLINEMON.TextBuffer[0].assign(str);
	GMSPRINT("GMGMED", 51);

	int err;
	if (medtype == 'A')
	{
		err = EMGABMED(1, code, MEDSequence);
	}
	else if (medtype == 'B')
	{
		err = EMGABMED(2, code, MEDSequence);
	}
	else if (medtype == 'C')
	{
		err = CMRMEDIN(code, MEDSequence);
	}
	else if (medtype == 'G')
	{
		err = EMGABMED(3, code, MEDSequence);
	}
	else if (medtype == 'F')
	{
		err = PMQAFMED(code, MEDSequence);
	}
	else if (medtype == 'M')
	{
		err = PMMMED(code, MEDSequence);
	}
	else if (medtype == 'P')
	{
		err = GMSMED(code, MEDSequence);
	}
	else if (medtype == 'R')
	{
		err = RMRMED(code, MEDSequence);
	}
	else if (medtype == 'S')
	{
		err = BMQDCMED(code, MEDSequence);
	}
	else if (medtype == 'U')
	{
		err = EMGTVMED(code, MEDSequence);
	}
	else if (medtype == 'X')
	{
		err = GMSREMED(code, MEDSequence);
	}
	else
	{
		err = 3;
	}
	
	char Buffer[128];
	if (err == 0)
	{
		sprintf_s(Buffer, "%c%s OK", medtype, code.c_str());
		RTCCONLINEMON.TextBuffer[0].assign(Buffer);
		GMSPRINT("GMGMED", 51);
	}
	else
	{
		sprintf_s(Buffer, "%c%s ERR %d", medtype, code.c_str(), err);
		RTCCONLINEMON.TextBuffer[0].assign(Buffer);
		GMSPRINT("GMGMED", 51);
	}

	return true;
}

int RTCC::EMGABMED(int type, std::string med, std::vector<std::string> data)
{
	//A MEDs
	if (type == 1)
	{

	}
	//B MEDs
	else if (type == 2)
	{
		//Generate Station Contacts
		if (med == "03")
		{
			if (data.size() < 1)
			{
				return 1;
			}
			int veh;
			if (data[0] == "CSM")
			{
				veh = 1;
			}
			else if (data[0] == "LEM")
			{
				veh = 3;
			}
			else
			{
				return 1;
			}
			EMSTAGEN(veh);
		}
		//Suppress/unsuppress C-band station contacts
		else if (med == "04")
		{
			if (data.size() != 1)
			{
				return 1;
			}
			if (data[0] == "START")
			{
				SystemParameters.MGRTAG = 0;
				RTCCONLINEMON.TextBuffer[0] = "SUPPRESSED";
			}
			else if (data[0] == "STOP")
			{
				SystemParameters.MGRTAG = 1;
				RTCCONLINEMON.TextBuffer[0] = "UNSUPPRESSED";
			}
			else
			{
				return 2;
			}
			EMGPRINT("EMGABMED", 46);
		}
	}
	//G MEDs
	else if (type == 3)
	{
		//CSM/LEM REFSMMAT locker movement
		if (med == "00")
		{
			if (data.size() < 4)
			{
				return 1;
			}
			int L1, ID1, L2, ID2;
			if (data[0] == "CSM")
			{
				L1 = 1;
			}
			else if (data[0] == "LEM")
			{
				L1 = 3;
			}
			else
			{
				return 1;
			}
			ID1 = EMGSTGENCode(data[1].c_str());
			if (ID1 < 0)
			{
				return 1;
			}
			if (data[2] == "CSM")
			{
				L2 = 1;
			}
			else if (data[2] == "LEM")
			{
				L2 = 3;
			}
			else
			{
				return 1;
			}
			ID2 = EMGSTGENCode(data[3].c_str());
			if (ID2 < 0)
			{
				return 1;
			}
			double gmt, hh, mm, ss;
			if (data.size() < 5 || data[4] == "")
			{
				gmt = RTCCPresentTimeGMT();
			}
			else if (sscanf(data[4].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) == 3)
			{
				gmt = GMTfromGET(hh * 3600.0 + mm * 60.0 + ss);
			}
			else
			{
				return 1;
			}
			EMGSTGEN(2, L1, ID1, L2, ID2, gmt);
		}
		//Enter manual IMU matrix
		else if (med == "01")
		{
			if (data.size() < 1)
			{
				return 1;
			}
			int L;
			if (data[0] == "CSM")
			{
				L = 1;
			}
			else if (data[0] == "LEM")
			{
				L = 3;
			}
			else
			{
				return 1;
			}
			if (data.size() > 1)
			{
				if (data[1] != "")
				{
					int ID = EMGSTGENCode(data[1].c_str());
					if (ID < 0)
					{
						return 2;
					}
					EZGSTMED.G01_Type = ID;
				}
				if (data.size() > 2)
				{
					double val;
					for (unsigned int i = 0;i < 9;i++)
					{
						//MED doesn't have more data, break
						if (i + 3 > data.size()) break;
						//Entry is empty, continue
						if (data[i + 2] == "") continue;
						//Get value
						if (sscanf(data[i + 2].c_str(), "%lf", &val) != 1)
						{
							return 2;
						}
						//Check if between -1.0 and 1.0
						if (val > 1.0 || val < -1.0)
						{
							return 2;
						}
						//Save
						EZGSTMED.G01_REFSMMAT.data[i] = val;
					}
				}
			}
			EMGSTGEN(3, L, 0, 0, 0, RTCCPresentTimeGMT());
		}
		//COMPUTE AND SAVE LOCAL VERTICAL CSM/LM PLATFORM ALIGNMENT
		else if (med == "03")
		{
			if (data.size() < 3)
			{
				return 1;
			}
			int L;
			if (data[0] == "CSM")
			{
				L = 1;
			}
			else if (data[0] == "LEM")
			{
				L = 3;
			}
			else
			{
				return 1;
			}
			double gmt, hh, mm, ss;
			if (sscanf(data[1].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) == 3)
			{
				gmt = GMTfromGET(hh * 3600.0 + mm * 60.0 + ss);
			}
			else
			{
				return 2;
			}
			int body;
			if (data[2] == "E")
			{
				body = BODY_EARTH;
			}
			else if (data[2] == "M")
			{
				body = BODY_MOON;
			}
			else
			{
				return 2;
			}
			EMMGLCVP(L, gmt, body);
		}
		//Generate Guidance Optics Support Table
		else if (med == "10")
		{
			if (data.size() < 4)
			{
				return 1;
			}
			if (data[0] != "CSM")
			{
				return 2;
			}
			
			double get;
			if (data[1] == "")
			{
				get = 0.0;
			}
			else if (MEDTimeInputHHMMSS(data[1], get))
			{
				return 2;
			}
			unsigned star;
			if (data[2] == "")
			{
				star = 1;
			}
			else
			{
				if (sscanf(data[2].c_str(), "%o", &star) != 1)
				{
					return 2;
				}
			}
			int mtx1, mtx2, mtx3;
			if (data[3] == "")
			{
				mtx1 = 0;
			}
			else
			{
				mtx1 = EMGSTGENCode(data[3].c_str());
				if (mtx1 <= 0)
				{
					return 2;
				}
			}
			if (data.size() < 5 || data[4] == "")
			{
				mtx2 = 0;
			}
			else
			{
				mtx2 = EMGSTGENCode(data[4].c_str());
				if (mtx2 <= 0)
				{
					return 2;
				}
			}
			if (data.size() < 6 || data[5] == "")
			{
				mtx3 = 0;
			}
			else
			{
				mtx3 = EMGSTGENCode(data[5].c_str());
				if (mtx3 <= 0)
				{
					return 2;
				}
			}
			EZGSTMED.GMT = GMTfromGET(get*3600.0);
			EZGSTMED.StartingStar = star;
			EZGSTMED.MTX1 = mtx1;
			EZGSTMED.MTX2 = mtx2;
			EZGSTMED.MTX3 = mtx3;
			EMMGSTMP();
		}
		//Acquire and save CSM IMU matrix/optics
		else if (med == "11")
		{
			if (data.size() < 2)
			{
				return 1;
			}
			if (data[0] != "CSM")
			{
				return 2;
			}
			//Valid codes and what they mean:
			//TMH: REFSMMAT from high speed telemetry
			//TML: REFSMMAT from low speed telemetry
			//OST-T: Optics Support Table REFSMMAT using telemetry data
			//OST-M: Optics Support Table REFSMMAT using manual data
			//DMT: REFSMMAT for maneuver in LM MPT
			//DOS: REFSMMAT for maneuver in spacecraft setting
			//DOM: REFSMMAT for maneuver in manual column of deorbit targeting
			//OPH: Save optics data from high speed
			//OPL: Save optics data from low speed
			//ATH: Save attitude from high speed telemetry
			//ATL: Save attitude from low speed telemetry
			//REP: REFSMMAT in RTE primary column
			//REM: REFSMMAT in RTE manual column
			if (data[1] == "DMT")
			{
				if (data.size() < 5)
				{
					return 1;
				}
				unsigned man;
				if (sscanf(data[2].c_str(), "%d", &man) != 1)
				{
					return 2;
				}
				if (man < 1 || man > 15)
				{
					return 2;
				}
				int refs;

				if (data[3] == "DES")
				{
					refs = 100;
				}
				else
				{
					refs = EMGSTGENCode(data[3].c_str());
					if (refs < 0 || refs > 10)
					{
						return 2;
					}
				}
				bool headsup;
				if (data[4] == "U")
				{
					headsup = true;
				}
				else if (data[4] == "D")
				{
					headsup = false;
				}
				else
				{
					return 2;
				}
				EMSGSUPP(1, 5, refs, man, headsup);
			}
			else if (data[1] == "DOM")
			{
				EMSGSUPP(1, 6, 1);
			}
			else if (data[1] == "DOS")
			{
				EMSGSUPP(1, 6, 2);
			}
			else if (data[1] == "REP")
			{
				EMSGSUPP(1, 6, 3);
			}
			else if (data[1] == "REM")
			{
				EMSGSUPP(1, 6, 4);
			}
			else if (data[1] == "OST-M")
			{
				EMSGSUPP(1, 3, 1);
			}
		}
		//Enter CSM sextant optics and IMU attitudes
		else if (med == "12")
		{
			if (data.size() < 1)
			{
				return 1;
			}
			if (data[0] != "CSM")
			{
				return 2;
			}
			unsigned star, i, j;
			double shaft, trun;
			for (i = 0;i < 2;i++)
			{
				if (data.size() <= i * 3 + 1)
				{
					EMDGSUPP(0);
					return 0;
				}
				if (data[i * 3 + 1] != "")
				{
					if (sscanf(data[i * 3 + 1].c_str(), "%d", &star) == 1)
					{
						EZJGSTTB.SXT_STAR[i] = star;
					}
				}
				if (data.size() <= i * 3 + 2)
				{
					EMDGSUPP(0);
					return 0;
				}
				if (data[i * 3 + 2] != "")
				{
					if (sscanf(data[i * 3 + 2].c_str(), "%lf", &shaft) == 1)
					{
						shaft *= RAD;
						if (shaft >= 0.0 && shaft <= PI2)
						{
							EZJGSTTB.SXT_SFT_INP[i] = shaft;
						}
						else
						{
							return 2;
						}
					}
				}
				if (data.size() <= i * 3 + 3)
				{
					EMDGSUPP(0);
					return 0;
				}
				if (data[i * 3 + 3] != "")
				{
					if (sscanf(data[i * 3 + 3].c_str(), "%lf", &trun) == 1)
					{
						trun *= RAD;
						if (trun >= 0.0 && trun <= PI2)
						{
							EZJGSTTB.SXT_TRN_INP[i] = trun;
						}
						else
						{
							return 2;
						}
					}
				}
			}
			double att;
			for (i = 0;i < 2;i++)
			{
				for (j = 0;j < 3;j++)
				{
					if (data.size() <= i * 3 + j + 7)
					{
						EMDGSUPP(0);
						return 0;
					}
					if (data[i * 3 + j + 7] != "")
					{
						if (sscanf(data[i * 3 + j + 7].c_str(), "%lf", &att) == 1)
						{
							att *= RAD;
							if (att >= 0.0 && att <= PI2)
							{
								EZJGSTTB.Att[i].data[j] = att;
							}
							else
							{
								return 2;
							}
						}
					}
				}
			}
			EMDGSUPP(0);
		}
		//Enter target for GOST star catalog
		else if (med == "13")
		{
			if (data.size() < 3)
			{
				return 1;
			}

			unsigned int pos;

			if (sscanf(data[0].c_str(), "%d", &pos) != 1)
			{
				return 2;
			}
			if (pos < 392 || pos > 400)
			{
				return 2;
			}
			double ra, dec, deg, hours, minutes;

			if (sscanf(data[1].c_str(), "%lf:%lf:%lf", &deg, &hours, &minutes) != 3)
			{
				return 2;
			}
			ra = (deg + hours / 60.0 + minutes / 3600.0)*RAD;
			if (ra < 0.0 || ra > PI2)
			{
				return 2;
			}
			if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &deg, &hours, &minutes) != 3)
			{
				return 2;
			}
			dec = (abs(deg) + hours / 60.0 + minutes / 3600.0)*RAD;
			if (deg < 0.0)
			{
				dec = -dec;
			}
			if (dec < -PI05 || dec > PI05)
			{
				return 2;
			}
			EZJGSTAR[pos - 1] = OrbMech::r_from_latlong(dec, ra);
		}
		//Initialize MSK 229 display of special targets
		else if (med == "14")
		{
			if (data.size() < 2)
			{
				return 1;
			}
			int veh;
			if (data[0] == "CSM")
			{
				veh = 1;
			}
			else if (data[0] == "LEM")
			{
				veh = 3;
			}
			else
			{
				return 2;
			}
			unsigned star;
			if (data[1] == "")
			{
				star = 0;
			}
			else
			{
				if (sscanf(data[1].c_str(), "%d", &star) != 1)
				{
					return 2;
				}
				if (star < 1 || star > 400U)
				{
					return 2;
				}
			}
			int rb = 0;
			double lat = 0.0, lng = 0.0, height = 0.0, GMT = 0.0;
			if (star == 0)
			{
				if (data.size() < 4)
				{
					return 2;
				}
				double hours;
				if (MEDTimeInputHHMMSS(data[2], hours))
				{
					return 2;
				}
				GMT = GMTfromGET(hours*3600.0);
				if (data[3] == "S")
				{
					rb = 1;
				}
				else if (data[3] == "M")
				{
					rb = 2;
				}
				else if (data[3] == "E")
				{
					rb = 3;
				}
				else
				{
					return 2;
				}
				if (rb != 1)
				{
					if (data.size() < 7)
					{
						return 2;
					}
					if (sscanf(data[4].c_str(), "%lf", &lat) != 1)
					{
						return 2;
					}
					lat *= RAD;
					if (lat < -PI05 || lat > PI05)
					{
						return 2;
					}
					if (sscanf(data[5].c_str(), "%lf", &lng) != 1)
					{
						return 2;
					}
					lng *= RAD;
					if (lng < 0 || lng > PI2)
					{
						return 2;
					}
					if (sscanf(data[6].c_str(), "%lf", &height) != 1)
					{
						return 2;
					}
					height *= 0.3048;
				}
			}

			EZGSTMED.G14_Star = star;
			EZGSTMED.G14_Vehicle = veh;
			EZGSTMED.G14_height = height;
			EZGSTMED.G14_lat = lat;
			EZGSTMED.G14_lng = lng;
			EZGSTMED.G14_RB = rb;
			EZGSTMED.G14_GMT = GMT;
			EZGSTMED.MTX1 = 0;
			EZGSTMED.MTX2 = 0;
			EZGSTMED.MTX3 = 0;

			EMMGSTMP();
		}
		//Generate LEM Optics Support Table
		else if (med == "20")
		{
			if (EZJGSTBL.MODE != 0)
			{
				EMMGLMST(EZJGSTBL.MODE);
			}
		}
		//Acquire and save LEM IMU matrix/optics
		else if (med == "21")
		{
			if (data.size() < 2)
			{
				return 1;
			}
			if (data[0] != "LEM")
			{
				return 2;
			}
			//Valid codes and what they mean:
			//TMH: REFSMMAT from high speed telemetry
			//TML: REFSMMAT from low speed telemetry
			//OST-T: Optics Support Table REFSMMAT using telemetry data
			//OST-M: Optics Support Table REFSMMAT using manual data
			//DMT: REFSMMAT for maneuver in LM MPT
			//DOS: REFSMMAT for maneuver in spacecraft setting
			//DOM: REFSMMAT for maneuver in manual column of deorbit targeting
			//OPH: Save optics data from high speed
			//OPL: Save optics data from low speed
			//ATH: Save attitude from high speed telemetry
			//ATL: Save attitude from low speed telemetry
			//REP: REFSMMAT in RTE primary column
			//REM: REFSMMAT in RTE manual column
			if (data[1] == "LLD")
			{
				EMSLSUPP(1, 15);
			}
			else if (data[1] == "OST-M")
			{
				EMSLSUPP(1, 4);
			}
			else if (data[1] == "DMT")
			{
				if (data.size() < 5)
				{
					return 1;
				}
				unsigned man;
				if (sscanf(data[2].c_str(), "%d", &man) != 1)
				{
					return 2;
				}
				if (man < 1 || man > 15)
				{
					return 2;
				}
				int refs;
				
				if (data[3] == "DES")
				{
					refs = 100;
				}
				else
				{
					refs = EMGSTGENCode(data[3].c_str());
					if (refs < 0 || refs > 10)
					{
						return 2;
					}
				}
				bool headsup;
				if (data[4] == "U")
				{
					headsup = true;
				}
				else if (data[4] == "D")
				{
					headsup = false;
				}
				else
				{
					return 2;
				}
				EMSLSUPP(1, 5, refs, man, headsup);
			}
		}
		//Calculate DOK REFSMMAT
		else if (med == "23")
		{
			if (EZGSTMED.G23_Option < 1 || EZGSTMED.G23_Option>3)
			{
				return 2;
			}

			EMSLSUPP(3, EZGSTMED.G23_Option);
		}
	}
	
	return 0;
}

int RTCC::CMRMEDIN(std::string med, std::vector<std::string> data)
{
	//Landing Site Vector Update
	if (med == "06")
	{
		if (data.size() < 1)
		{
			return 1;
		}
		int VehicleType;
		if (data[0] == "CMC")
		{
			VehicleType = 1;
		}
		else if (data[0] == "LGC")
		{
			VehicleType = 2;
		}
		else
		{
			return 2;
		}
		CMMCMCLS(VehicleType);
	}
	//Initiate a DCS Time Increment Update
	else if (med == "07")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int VehicleType;
		if (data[0] == "CMC")
		{
			VehicleType = 1;
		}
		else if (data[0] == "LGC")
		{
			VehicleType = 2;
		}
		else
		{
			return 2;
		}

		double Inc;
		if (MEDTimeInputHHMMSS(data[1], Inc))
		{
			return 2;
		}
		if (abs(Inc) > 745.5)
		{
			return 2;
		}
		CMMTMEIN(VehicleType, Inc);
	}
	//Initiate Liftoff Time Update
	else if (med == "08")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int VehicleType;
		if (data[0] == "CMC")
		{
			VehicleType = 1;
		}
		else if (data[0] == "LGC")
		{
			VehicleType = 2;
		}
		else
		{
			return 2;
		}

		double Inc;
		if (MEDTimeInputHHMMSS(data[1], Inc))
		{
			return 2;
		}
		if (abs(Inc) > 745.5)
		{
			return 2;
		}
		CMMLIFTF(VehicleType, Inc);
	}
	//Initiate a CMC/LGC external delta-V update
	else if (med == "10")
	{
		if (data.size() != 3)
		{
			return 1;
		}
		int VehicleType;
		if (data[0] == "CMC")
		{
			VehicleType = 1;
		}
		else if (data[0] == "LGC")
		{
			VehicleType = 2;
		}
		else
		{
			return 2;
		}
		unsigned ManeuverNum;
		if (sscanf(data[1].c_str(), "%d", &ManeuverNum) != 1)
		{
			return 2;
		}
		if (ManeuverNum < 1 || ManeuverNum > 15)
		{
			return 2;
		}
		int L;
		if (data[2] == "CSM")
		{
			L = 1;
		}
		else if (data[2] == "LEM")
		{
			L = 3;
		}
		else
		{
			return 2;
		}
		MissionPlanTable *tab = GetMPTPointer(L);
		if (tab->mantable.size() < ManeuverNum)
		{
			return 2;
		}
		double TIG;
		VECTOR3 DV = tab->mantable[ManeuverNum - 1].dV_LVLH;
		if (VehicleType == 1)
		{
			TIG = tab->mantable[ManeuverNum - 1].GMTI - SystemParameters.MCGZSA * 3600.0;
			CMMAXTDV(TIG, DV, L, ManeuverNum);
		}
		else
		{
			TIG = tab->mantable[ManeuverNum - 1].GMTI - SystemParameters.MCGZSL * 3600.0;
			CMMLXTDV(TIG, DV, L, ManeuverNum);
		}
	}
	//Initiate a CMC/LGC REFSMMAT update
	else if (med == "12")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int Veh;
		if (data[0] == "CMC")
		{
			Veh = 1;
		}
		else if (data[0] == "LGC")
		{
			Veh = 3;
		}
		else
		{
			return 2;
		}
		int id = EMGSTGENCode(data[1].c_str());
		if (id < 0)
		{
			return 2;
		}
		int type;
		if (data[2] == "1")
		{
			type = 1;
		}
		else if (data[2] == "2")
		{
			type = 2;
		}
		else
		{
			return 2;
		}
		CMMRFMAT(Veh, id, type);
	}
	//Change and/or delete data from an erasable memory update
	else if (med == "EC")
	{
		if (data.size() < 2)
		{
			return 1;
		}

		int load, block;
		if (sscanf(data[0].c_str(), "%d", &load) != 1)
		{
			return 2;
		}
		switch (load)
		{
		case 18:
			block = 0;
			break;
		case 19:
			block = 1;
			break;
		case 38:
			block = 2;
			break;
		case 39:
			block = 3;
			break;
		default:
			return 2;
		}

		//TBD: Command sites

		if (data.size() > 5)
		{
			AGCErasableMemoryUpdateMakeupBlock *bl = &CZERAMEM.Blocks[block];

			int line;
			if (sscanf(data[5].c_str(), "%o", &line) != 1)
			{
				return 2;
			}
			if (line < 02)
			{
				return 2;
			}
			if (bl->IsVerb72)
			{
				if (line > 023) return 2;
			}
			else
			{
				if (line > 024) return 2;
			}

			std::vector<int> values;
			CMMERMEM(block, 3, line, values);
		}
	}
	//Generate an erasable memory command load using engineering units
	else if (med == "EE")
	{
		if (data.size() < 4)
		{
			return 1;
		}

		int load, block;
		if (sscanf(data[0].c_str(), "%d", &load) != 1)
		{
			return 2;
		}
		switch (load)
		{
		case 18:
			block = 0;
			break;
		case 19:
			block = 1;
			break;
		case 38:
			block = 2;
			break;
		case 39:
			block = 3;
			break;
		default:
			return 2;
		}

		int line;
		if (sscanf(data[1].c_str(), "%o", &line) != 1)
		{
			return 2;
		}

		int precision;
		if (sscanf(data[2].c_str(), "%d", &precision) != 1)
		{
			return 2;
		}

		if (precision < 1 || precision > 3)
		{
			return 2;
		}

		int maxline = 024 - precision + 1;

		AGCErasableMemoryUpdateMakeupBlock *bl = &CZERAMEM.Blocks[block];

		if (bl->IsVerb72)
		{
			maxline--;
		}

		double Magnitude;
		if (sscanf(data[3].c_str(), "%lf", &Magnitude) != 1)
		{
			return 2;
		}

		double Multiplier;

		if (data.size() < 5 || data[4] == "")
		{
			Multiplier = 1.0;
		}
		else
		{
			if (sscanf(data[4].c_str(), "%lf", &Multiplier) != 1)
			{
				return 2;
			}
		}

		double ScaleFactor;

		if (data.size() < 6 || data[5] == "")
		{
			ScaleFactor = 1.0;
		}
		else
		{
			if (sscanf(data[5].c_str(), "%lf", &ScaleFactor) != 1)
			{
				return 2;
			}
		}

		int PowerOfTwo;

		if (data.size() < 7 || data[6] == "")
		{
			PowerOfTwo = 0;
		}
		else
		{
			if (sscanf(data[5].c_str(), "%d", &PowerOfTwo) != 1)
			{
				return 2;
			}

			if (PowerOfTwo < -42 || PowerOfTwo > 42) return 2;
		}

		std::vector<int> values;

		if (bl->IsVerb72)
		{
			//Address

			int Address;

			if (data.size() < 8 || data[7] == "")
			{
				return 2;
			}

			if (sscanf(data[7].c_str(), "%o", &Address) != 1)
			{
				return 2;
			}

			if (Address < 0 || Address > 03777) return 2;

			values.push_back(Address);
		}

		//Convert to octal

		double number = Magnitude * Multiplier / ScaleFactor;

		if (precision == 1)
		{
			int value;

			value = OrbMech::SingleToBuffer(number, PowerOfTwo);

			values.push_back(value);
		}
		else if (precision == 2)
		{
			int value[2];
			OrbMech::DoubleToBuffer(number, PowerOfTwo, value[0], value[1]);

			values.push_back(value[0]);
			values.push_back(value[1]);
		}
		else
		{
			int value[3];

			OrbMech::TripleToBuffer(number, PowerOfTwo, value[0], value[1], value[2]);

			values.push_back(value[0]);
			values.push_back(value[1]);
			values.push_back(value[2]);
		}

		CMMERMEM(block, 1, line, values);
	}
	//Initialize erasable memory update makup buffer
	else if (med == "EI")
	{
		if (data.size() < 3)
		{
			return 1;
		}

		int load, block;
		if (sscanf(data[0].c_str(), "%d", &load) != 1)
		{
			return 2;
		}
		switch (load)
		{
		case 18:
			block = 0;
			break;
		case 19:
			block = 1;
			break;
		case 38:
			block = 2;
			break;
		case 39:
			block = 3;
			break;
		default:
			return 2;
		}

		//TBD: Command sites

		std::vector<int> values;
		if (data.size() < 4 || data[3] == "")
		{
			//V72
			values.push_back(0);
		}
		else
		{
			//V71
			int address;
			if (sscanf(data[3].c_str(), "%o", &address) != 1)
			{
				return 2;
			}

			if (address < 0 || address > 03777)
			{
				return 2;
			}
			values.push_back(address);
		}

		//Vehicle ID
		int VehID;
		if (data.size() < 5)
		{
			if (load == 18 || load == 19)
			{
				VehID = 4;
			}
			else
			{
				VehID = 3;
			}
		}
		else
		{
			if (sscanf(data[4].c_str(), "%d", &VehID) != 1)
			{
				return 2;
			}
		}

		CMMERMEM(block, 0, 0, values);
	}
	//Generate an erasable memory update with octal entries
	else if (med == "EO")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int load, block;
		if (sscanf(data[0].c_str(), "%d", &load) != 1)
		{
			return 2;
		}
		switch (load)
		{
		case 18:
			block = 0;
			break;
		case 19:
			block = 1;
			break;
		case 38:
			block = 2;
			break;
		case 39:
			block = 3;
			break;
		default:
			return 2;
		}

		AGCErasableMemoryUpdateMakeupBlock *bl = &CZERAMEM.Blocks[block];

		int line;
		if (sscanf(data[1].c_str(), "%o", &line) != 1)
		{
			return 2;
		}
		if (line < 02)
		{
			return 2;
		}
		if (bl->IsVerb72)
		{
			if (line > 023) return 2;
		}
		else
		{
			if (line > 024) return 2;
		}

		int value;
		if (sscanf(data[2].c_str(), "%o", &value) != 1)
		{
			return 2;
		}

		//Check for valid value or address
		bool IsAddress;
		if (bl->IsVerb72)
		{
			//Even lines are addresses
			IsAddress = (line % 2 == 0);
		}
		else
		{
			//Only line 2 is an address
			IsAddress = (line == 02);
		}
		if (value < 0)
		{
			return 2;
		}
		if (IsAddress)
		{
			if (value > 03777) return 2;
		}
		else
		{
			if (value > 077777) return 2;
		}

		std::vector<int> values;

		values.push_back(value);

		CMMERMEM(block, 2, line, values);
	}
	return 0;
}

int RTCC::PMQAFMED(std::string med)
{
	std::vector<std::string> data;
	return PMQAFMED(med, data);
}

int RTCC::PMQAFMED(std::string med, std::vector<std::string> data)
{
	//Initialize Azimuth Constraints for Midcourse Correction Planning
	if (med == "22")
	{
		double azmin, azmax;
		if (data.size() < 1 || data[0]== "")
		{
			azmin = -110.0*RAD;
		}
		else
		{
			if (sscanf(data[0].c_str(), "%lf", &azmin) != 1)
			{
				return 1;
			}
			if (azmin < -110.0)
			{
				return 1;
			}
			azmin *= RAD;
		}
		PZMCCPLN.AZ_min = azmin;
		if (data.size() < 2 || data[1] == "")
		{
			azmax = -70.0*RAD;
		}
		else
		{
			if (sscanf(data[1].c_str(), "%lf", &azmax) != 1)
			{
				return 1;
			}
			if (azmax > -70.0)
			{
				return 1;
			}
			azmax *= RAD;
		}
		PZMCCPLN.AZ_max = azmax;
	}
	//Initialization of translunar time (minimum and maximum) for midcourse correction planning
	else if (med == "23")
	{
		double hh, mm, ss, get;
		if (data.size() < 1 || data[0] == "")
		{
			PZMCCPLN.TLMIN = 0.0;
		}
		else
		{
			if (sscanf(data[0].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 1;
			}
			PZMCCPLN.TLMIN = hh + mm / 60.0 + ss / 3600.0;
		}
		if (data.size() < 2 || data[1] == "")
		{
			PZMCCPLN.TLMAX = 0.0;
		}
		else
		{
			if (sscanf(data[1].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 1;
			}
			get = hh + mm / 60.0 + ss / 3600.0;
			if (get == 0.0 || get > PZMCCPLN.TLMIN)
			{
				PZMCCPLN.TLMAX = get;
			}
		}
	}
	//Initalize gamma and reentry range for midcourse correction plans
	else if (med == "24")
	{
		if (data.size() < 1 || data[0] == "")
		{
			PZMCCPLN.gamma_reentry = -6.52*RAD;
		}
		else
		{
			double gamma;
			if (sscanf(data[0].c_str(), "%lf", &gamma) != 1)
			{
				return 1;
			}
			PZMCCPLN.gamma_reentry = gamma * RAD;
		}
		if (data.size() < 2 || data[1] == "")
		{
			PZMCCPLN.Reentry_range = 1285.0;
		}
		else
		{
			double range;
			if (sscanf(data[1].c_str(), "%lf", &range) != 1)
			{
				return 1;
			}
			PZMCCPLN.Reentry_range = range;
		}
	}
	//Delete column of midcourse correction display
	else if (med == "26")
	{
		if (data.size() < 1)
		{
			return 1;
		}
		int column;
		if (sscanf(data[0].c_str(), "%d", &column) != 1)
		{
			return 1;
		}
		if (column < 0 || column>4)
		{
			return 1;
		}
		if (column == 0)
		{
			for (int i = 0;i < 4;i++)
			{
				PZMCCDIS.data[i].Mode = 0;
			}
		}
		else
		{
			PZMCCDIS.data[column - 1].Mode = 0;
		}
	}
	//Specify pericynthion height limits for optimized MCC
	else if (med == "29")
	{
	if (data.size() < 1 || data[0] == "")
	{
		PZMCCPLN.H_PCYN_MIN = 40.0*1852.0;
	}
	else
	{
		double ht;
		if (sscanf(data[0].c_str(), "%lf", &ht) != 1)
		{
			return 1;
		}
		PZMCCPLN.H_PCYN_MIN = ht * 1852.0;;
	}
	if (data.size() < 2 || data[1] == "")
	{
		PZMCCPLN.H_PCYN_MAX = 5000.0*1852.0;
	}
	else
	{
		double ht;
		if (sscanf(data[1].c_str(), "%lf", &ht) != 1)
		{
			return 1;
		}
		PZMCCPLN.H_PCYN_MAX = ht * 1852.0;
	}
	}
	//Transfer midcourse correction plan to skeleton flight plan table
	else if (med == "30")
	{
	if (data.size() < 1)
	{
		return 1;
	}
	int column;

	if (sscanf(data[0].c_str(), "%d", &column) != 1)
	{
		return 1;
	}
	if (column < 1 || column > 4)
	{
		return 1;
	}
	if (PZMCCSFP.blocks[column - 1].mode <= 1 || PZMCCSFP.blocks[column - 1].mode >= 6)
	{
		return 1;
	}
	PZSFPTAB.blocks[1] = PZMCCSFP.blocks[column - 1];
	}
	//Alteration of Skeleton Flight Plan Table
	else if (med == "32")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int tab;
		if (sscanf(data[0].c_str(), "%d", &tab) != 1)
		{
			return 1;
		}
		if (tab < 1 || tab > 2)
		{
			return 1;
		}
		int item;
		if (sscanf(data[1].c_str(), "%d", &item) != 1)
		{
			return 1;
		}
		if (item < 1 || item > 26)
		{
			return 1;
		}
		TLMCCDataTable *table = &PZSFPTAB.blocks[tab - 1];
		double hh, mm, ss, val;
		int mode;
		switch (item)
		{
		case 1:
		case 3:
		case 7:
		case 11:
		case 12:
		case 18:
		case 19:
		case 26:
			if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 1;
			}
			val = hh * 3600.0 + mm * 60.0 + ss;
			switch (item)
			{
			case 1:
				table->GMTTimeFlag = val;
				break;
			case 3:
				table->GMT_pc1 = val;
				break;
			case 7:
				table->GMT_pc2 = val;
				break;
			case 11:
				table->GET_TLI = val;
				break;
			case 12:
				table->GMT_nd = val;
				break;
			case 18:
				table->T_lo = val;
				break;
			case 19:
				table->dt_lls = val;
				break;
			case 26:
				table->T_te = val;
				break;
			}
			break;
		case 2:
			if (sscanf(data[2].c_str(), "%d", &mode) != 1)
			{
				return 1;
			}
			table->mode = mode;
			break;
		case 4:
		case 5:
		case 6:
		case 8:
		case 9:
		case 10:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
			if (sscanf(data[2].c_str(), "%lf", &val) != 1)
			{
				return 1;
			}
			switch (item)
			{
			case 4:
				table->lat_pc1 = val * RAD;
				break;
			case 5:
				table->lng_pc1 = val * RAD;
				break;
			case 6:
				table->h_pc1 = val * 1852.0;
				break;
			case 8:
				table->lat_pc2 = val * RAD;
				break;
			case 9:
				table->lng_pc2 = val * RAD;
				break;
			case 10:
				table->h_pc2 = val * 1852.0;
				break;
			case 13:
				table->lat_nd = val * RAD;
				break;
			case 14:
				table->lng_nd = val * RAD;
				break;
			case 15:
				table->h_nd = val * 1852.0;
				break;
			case 16:
				table->dpsi_loi = val * RAD;
				break;
			case 17:
				table->gamma_loi = val * RAD;
				break;
			case 20:
				table->psi_lls = val * RAD;
				break;
			case 21:
				table->lat_lls = val * RAD;
				break;
			case 22:
				table->lng_lls = val * RAD;
				break;
			case 23:
				table->rad_lls = val * 1852.0;
				break;
			case 24:
				table->dpsi_tei = val * RAD;
				break;
			case 25:
				table->dv_tei = val * 0.3048;
				break;
			}
			break;
		}
	}
	//Interpolation for skeleton flight plan
	else if (med == "62")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int day;
		if (data[0] == "")
		{
			day = GZGENCSN.RefDayOfYear;
		}
		else if (sscanf(data[0].c_str(), "%d", &day) != 1)
		{
			return 1;
		}
		if (day < 1 || day > 366)
		{
			return 2;
		}
		int opp;
		if (sscanf(data[1].c_str(), "%d", &opp) != 1)
		{
			return 1;
		}
		if (opp < 1 || opp > 2)
		{
			return 2;
		}
		double azi;
		if (sscanf(data[2].c_str(), "%lf", &azi) != 1)
		{
			return 1;
		}
		if (azi < 72.0 || azi > 108.0)
		{
			return 2;
		}
		azi *= RAD;

		PZSFPTAB.Day = day;
		PZSFPTAB.Opportunity = opp;
		PZSFPTAB.Azimuth = azi;

		PMMSFPIN();
	}
	//Generation of near Earth tradeoff
	else if (med == "70")
	{
		bool found;
		//Check and find site
		found = DetermineRTESite(med_f70.Site);

		if (found == false)
		{
			return 1;
		}
		//Check vector time
		//TBD: T_V greater than present time
		PZREAP.RTEVectorTime = med_f70.T_V;

		//Check min abort time
		if (med_f70.T_omin < PZREAP.RTEVectorTime)
		{
			PZREAP.RTET0Min = PZREAP.RTEVectorTime;
		}
		else
		{
			PZREAP.RTET0Min = med_f70.T_omin;
		}

		//Check max abort time
		if (med_f70.T_omax == -1.0)
		{
			med_f70.T_omax = med_f70.T_omin + 24.0;
		}
		if (med_f70.T_omax < PZREAP.RTET0Min)
		{
			PZREAP.RTET0Max = PZREAP.RTET0Min;
		}
		else
		{
			PZREAP.RTET0Max = med_f70.T_omax;
		}

		//Check entry profile
		if (med_f70.EntryProfile == 2 && PZREAP.TGTLN == 1)
		{
			return 4;
		}
		PZREAP.EntryProfile = med_f70.EntryProfile;

		PMMREAP(70);
	}
	//Generation of remote Earth tradeoff
	else if (med == "71")
	{
		if (med_f71.Page < 1 || med_f71.Page > 5)
		{
			return 5;
		}
		PZREAP.RTETradeoffRemotePage = med_f71.Page;

		bool found = false;
		//Check and find site
		found = DetermineRTESite(med_f71.Site);

		if (found == false)
		{
			return 1;
		}
		//Check vector time
		//TBD: T_V greater than present time
		PZREAP.RTEVectorTime = med_f71.T_V;

		//Check min abort time
		if (med_f71.T_omin < PZREAP.RTEVectorTime)
		{
			PZREAP.RTET0Min = PZREAP.RTEVectorTime;
		}
		else
		{
			PZREAP.RTET0Min = med_f71.T_omin;
		}

		//Check max abort time
		if (med_f71.T_omax == -1.0)
		{
			med_f71.T_omax = med_f71.T_omin + 24.0;
		}
		if (med_f71.T_omax < PZREAP.RTET0Min)
		{
			PZREAP.RTET0Max = PZREAP.RTET0Min;
		}
		else
		{
			PZREAP.RTET0Max = med_f71.T_omax;
		}

		//Check entry profile
		if (med_f71.EntryProfile == 2 && PZREAP.TGTLN == 1)
		{
			return 4;
		}
		PZREAP.EntryProfile = med_f71.EntryProfile;

		PMMREAP(71);
	}
	//Abort Scan Table Generation for an unspecified area
	else if (med == "75")
	{
		//int type;

		if (med_f75.Type == "TCUA")
		{

		}
		else if (med_f75.Type == "FCUA")
		{

		}
		else
		{
			return 2;
		}

		PZREAP.RTEVectorTime = GMTfromGET(med_f75_f77.T_V) / 3600.0;
		PZREAP.RTET0Min = GMTfromGET(med_f75_f77.T_0_min) / 3600.0;

		if (PZREAP.TGTLN == 1)
		{
			PZREAP.EntryProfile = 2;
		}
		else
		{
			if (med_f75_f77.EntryProfile == "HB1")
			{
				PZREAP.EntryProfile = 1;
			}
			else
			{
				PZREAP.EntryProfile = 0;
			}
		}

		PMMREAP(75);
	}
	//Abort Scan Table generation for a specific site
	else if (med == "76")
	{
		bool found = DetermineRTESite(med_f76.Site);

		if (found == false)
		{
			return 2;
		}

		//Check vector time
		//TBD: T_V greater than present time
		PZREAP.RTEVectorTime = GMTfromGET(med_f75_f77.T_V) / 3600.0;
		PZREAP.RTET0Min = GMTfromGET(med_f75_f77.T_0_min) / 3600.0;
		PZREAP.RTETimeOfLanding = GMTfromGET(med_f75_f77.T_Z) / 3600.0;
		if (PZREAP.TGTLN == 1)
		{
			PZREAP.EntryProfile = 2;
		}
		else
		{
			if (med_f75_f77.EntryProfile == "HB1")
			{
				PZREAP.EntryProfile = 1;
			}
			else
			{
				PZREAP.EntryProfile = 0;
			}
		}
		PZREAP.RTEPTPMissDistance = med_f76.MissDistance;

		PMMREAP(76);
	}
	//AST lunar search generation for specific site or FCUA
	else if (med == "77")
	{
		if (med_f77.Site != "FCUA")
		{
			bool found = DetermineRTESite(med_f77.Site);

			if (found == false)
			{
				return 2;
			}
		}

		//Check vector time
		//TBD: T_V greater than present time
		PZREAP.RTEVectorTime = GMTfromGET(med_f75_f77.T_V) / 3600.0;
		PZREAP.RTET0Min = GMTfromGET(med_f75_f77.T_0_min) / 3600.0;
		PZREAP.RTETimeOfLanding = GMTfromGET(med_f75_f77.T_Z) / 3600.0;
		if (PZREAP.TGTLN == 1)
		{
			PZREAP.EntryProfile = 2;
		}
		else
		{
			if (med_f75_f77.EntryProfile == "HB1")
			{
				PZREAP.EntryProfile = 1;
			}
			else
			{
				PZREAP.EntryProfile = 0;
			}
		}
		PZREAP.RTEPTPMissDistance = med_f77.MissDistance;

		PMMREAP(77);
	}
	//Abort Scan Table delete
	else if (med == "79")
	{
		if (data.size() != 1)
		{
			return 1;
		}
		int row;
		if (sscanf(data[0].c_str(), "%d", &row) != 1)
		{
			return 2;
		}
		if (row < 0 || row > 7)
		{
			return 2;
		}
		if (row == 0)
		{
			for (int i = 0;i < 7;i++)
			{
				PZREAP.AbortScanTableData[i].ASTCode = 0;
			}
		}
		else
		{
			PZREAP.AbortScanTableData[row - 1].ASTCode = 0;
		}
	}
	//RTE digitals maneuver description
	else if (med == "80")
	{
		PMMREAP(80);
	}
	//Update the target table for return to Earth
	else if (med == "85")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int action;
		if (data[0] == "ADD")
		{
			action = 1;
		}
		else if (data[0] == "REPLACE")
		{
			action = 2;
		}
		else if (data[0] == "DELETE")
		{
			action = 3;
		}
		else
		{
			return 2;
		}

		int type;

		if (data[1] == "PTP")
		{
			type = 0;
		}
		else if (data[1] == "ATP")
		{
			type = 1;
		}
		else
		{
			return 2;
		}

		std::string Site = data[2];

		if (Site == "")
		{
			return 2;
		}

		unsigned i, j;

		if (action == 1)
		{
			//ADD

			if (data.size() < 6)
			{
				return 1;
			}

			//Site may not be in the target table and there must be an open space
			int space = -1;

			std::string *tab;

			if (type == 0)
			{
				tab = PZREAP.PTPSite;
			}
			else
			{
				tab = PZREAP.ATPSite;
			}

			//Can't have the same name twice
			for (i = 0; i < 5; i++)
			{
				if (tab[i] == Site)
				{
					return 2;
				}
			}

			//Find free spot
			for (i = 0; i < 5; i++)
			{
				if (tab[i] == "")
				{
					space = i;
					break;
				}
			}

			if (space == -1)
			{
				return 2;
			}

			double values[10];

			for (i = 0; i < 10; i++)
			{
				values[i] = 1e10;
			}

			double val;

			for (i = 4; i < 14; i++)
			{
				if (type == 0 && i >= 6) break;
				if (data.size() <= i) break;

				if (sscanf(data[i].c_str(), "%lf", &val) != 1)
				{
					return 2;
				}
				
				val *= RAD;

				if (i % 2 == 0)
				{
					//Latitude
					if (val > PI05 || val < -PI05)
					{
						return 2;
					}
				}
				else
				{
					//Longitude
					if (val > PI || val < -PI)
					{
						return 2;
					}
				}

				values[i - 4] = val;
			}
			
			if (type == 0)
			{
				//PTP
				PZREAP.PTPSite[space] = Site;
				PZREAP.PTPLatitude[space] = values[0];
				PZREAP.PTPLongitude[space] = values[1];
			}
			else
			{
				//ATP
				PZREAP.ATPSite[space] = Site;
				for (i = 0; i < 10; i++)
				{
					PZREAP.ATPCoordinates[space][i] = values[i];
				}
			}
		}
		else if (action == 2)
		{
			//REPLACE

			if (data.size() < 6)
			{
				return 1;
			}

			std::string *tab;

			if (type == 0)
			{
				tab = PZREAP.PTPSite;
			}
			else
			{
				tab = PZREAP.ATPSite;
			}

			int space = -1;

			//Find item to replace
			for (i = 0; i < 5; i++)
			{
				if (tab[i] == Site)
				{
					space = i;
					break;
				}
			}

			if (space == -1)
			{
				return 2;
			}

			//ATP number
			int Num = 0;

			if (type == 1)
			{
				if (sscanf(data[3].c_str(), "%d", &Num) != 1)
				{
					return 2;
				}
				if (Num < 1 || Num > 5)
				{
					return 2;
				}
			}

			double val, values[2];

			for (i = 0; i < 2; i++)
			{
				if (sscanf(data[i + 4].c_str(), "%lf", &val) != 1)
				{
					return 2;
				}

				val *= RAD;

				if (i % 2 == 0)
				{
					//Latitude
					if (val > PI05 || val < -PI05)
					{
						return 2;
					}
				}
				else
				{
					//Longitude
					if (val > PI || val < -PI)
					{
						return 2;
					}
				}

				values[i] = val;
			}

			if (type == 0)
			{
				//PTP
				PZREAP.PTPSite[space] = Site;
				PZREAP.PTPLatitude[space] = values[0];
				PZREAP.PTPLongitude[space] = values[1];
			}
			else
			{
				//ATP
				PZREAP.ATPSite[space] = Site;
				for (i = 0; i < 2; i++)
				{
					PZREAP.ATPCoordinates[space][i + 2 * (Num - 1)] = values[i];
				}
			}
		}
		else
		{
			//DELETE

			std::string *tab;

			if (type == 0)
			{
				tab = PZREAP.PTPSite;
			}
			else
			{
				tab = PZREAP.ATPSite;
			}

			//Find item to delete
			for (i = 0; i < 5; i++)
			{
				if (tab[i] == Site)
				{
					if (type == 0)
					{
						PZREAP.PTPSite[i] = "";
						PZREAP.PTPLatitude[i] = PZREAP.PTPLongitude[i] = 0.0;
					}
					else
					{
						PZREAP.ATPSite[i] = "";
						for (j = 0; j < 10; j++)
						{
							PZREAP.ATPCoordinates[i][j] = 1e10;
						}
					}
					break;
				}
			}
		}
	}
	//Update return to Earth constraints
	else if (med == "86")
	{
		if (med_f86.Constraint == "DVMAX")
		{
			PZREAP.DVMAX = med_f86.Value;
		}
		else if (med_f86.Constraint == "TZMIN")
		{
			PZREAP.TZMIN = med_f86.Value;
		}
		else if (med_f86.Constraint == "TZMAX")
		{
			PZREAP.TZMAX = med_f86.Value;
		}
		else if (med_f86.Constraint == "GMAX")
		{
			PZREAP.GMAX = med_f86.Value;
		}
		else if (med_f86.Constraint == "HMINMC")
		{
			PZREAP.HMINMC = med_f86.Value;
		}
		else if (med_f86.Constraint == "IRMAX")
		{
			PZREAP.IRMAX = med_f86.Value;
		}
		else if (med_f86.Constraint == "RRBIAS")
		{
			PZREAP.RRBIAS = med_f86.Value;
		}
		else if (med_f86.Constraint == "VRMAX")
		{
			PZREAP.VRMAX = med_f86.Value;
		}
	}
	//Update return to Earth constraints
	else if (med == "87")
	{
		if (med_f87.Constraint == "TGTLN")
		{
			if (med_f87.Value == "SHALLOW")
			{
				PZREAP.TGTLN = 0;
			}
			else if (med_f87.Value == "SHALLOW")
			{
				PZREAP.TGTLN = 1;
			}
		}
		else if (med_f87.Constraint == "MOTION")
		{
			if (med_f87.Value == "EITHER")
			{
				PZREAP.MOTION = 0;
			}
			else if (med_f87.Value == "DIRECT")
			{
				PZREAP.MOTION = 1;
			}
			else if (med_f87.Value == "CIRCUM")
			{
				PZREAP.MOTION = 2;
			}
		}
	}

	return 0;
}

void RTCC::PMKMED(std::string med)
{

}

int RTCC::PMMMED(std::string med, std::vector<std::string> data)
{
	//Input ignition and cutoff times of first S-IVB burn
	if (med == "03")
	{
		if (data.size() != 2)
		{
			return 1;
		}
		double T4IG, T4C;
		if (MEDTimeInputHHMMSS(data[0], T4IG))
		{
			return 2;
		}
		if (MEDTimeInputHHMMSS(data[1], T4C))
		{
			return 2;
		}
		if (T4C < T4IG)
		{
			return 2;
		}
		SystemParameters.MDVSTP.T4IG = T4IG;
		SystemParameters.MDVSTP.T4C = T4C;
	}
	else if (med == "40")
	{
		if (data.size() < 1)
		{
			return 1;
		}

		if (data[0] == "")
		{
			return 2;
		}
		if (data[0] == "P1")
		{
			double dv;
			if (data.size() < 2 || data[1] == "")
			{
				dv = -1;
			}
			else if (sscanf(data[1].c_str(), "%lf", &dv) != 1)
			{
				return 2;
			}
			int dvind;
			if (data.size() < 3 || data[2] == "" || data[2] == "MAG")
			{
				dvind = 0;
			}
			else if (data[2] == "DVC")
			{
				dvind = 1;
			}
			else if (data[2] == "XBT")
			{
				dvind = 2;
			}
			else
			{
				return 2;
			}

			double dt;
			if (data.size() < 4 || data[3] == "")
			{
				dt = 0.0;
			}
			else if (sscanf(data[3].c_str(), "%lf", &dt) != 1)
			{
				return 2;
			}
			if (dt < 0)
			{
				return 2;
			}

			PZBURN.P1_DV = dv * 0.3048;
			PZBURN.P1_DVIND = dvind;
			PZBURN.P1_DT = dt;
		}
		else if (data[0] == "P2")
		{
			if (data.size() < 4)
			{
				return 1;
			}

			VECTOR3 dv;
			if (sscanf(data[1].c_str(), "%lf", &dv.x) != 1)
			{
				return 2;
			}
			if (sscanf(data[2].c_str(), "%lf", &dv.y) != 1)
			{
				return 2;
			}
			if (sscanf(data[3].c_str(), "%lf", &dv.z) != 1)
			{
				return 2;
			}
			PZBURN.P2_DV = dv * 0.3048;
		}
		else if (data[0] == "P3")
		{
			VECTOR3 dv;
			if (data[1] == "")
			{
				return 2;
			}
			else if (sscanf(data[1].c_str(), "%lf", &dv.x) != 1)
			{
				return 2;
			}
			if (data[2] == "")
			{
				return 2;
			}
			else if (sscanf(data[2].c_str(), "%lf", &dv.y) != 1)
			{
				return 2;
			}
			if (data[3] == "")
			{
				return 2;
			}
			else if (sscanf(data[3].c_str(), "%lf", &dv.z) != 1)
			{
				return 2;
			}
			PZBURN.P3_DV = dv * 0.3048;
		}
		else if (data[0] == "P4")
		{
			VECTOR3 dv;
			if (data[1] == "")
			{
				return 2;
			}
			else if (sscanf(data[1].c_str(), "%lf", &dv.x) != 1)
			{
				return 2;
			}
			if (data[2] == "")
			{
				return 2;
			}
			else if (sscanf(data[2].c_str(), "%lf", &dv.y) != 1)
			{
				return 2;
			}
			if (data[3] == "")
			{
				return 2;
			}
			else if (sscanf(data[3].c_str(), "%lf", &dv.z) != 1)
			{
				return 2;
			}
			PZBURN.P4_DV = dv * 0.3048;
		}
	}
	//Change fuel remaining for specified thruster
	else if (med == "49")
	{
		if (data.size() < 1)
		{
			return 1;
		}

		if (data[0] == "CSM")
		{
			med_m49.Table = RTCC_MPT_CSM;
		}
		else if (data[0] == "LEM")
		{
			med_m49.Table = RTCC_MPT_LM;
		}
		else
		{
			return 2;
		}

		double temp;
		med_m49.SPSFuelRemaining = -1;
		if (data.size() > 1 && data[1] != "")
		{
			if (sscanf(data[1].c_str(), "%lf", &temp) == 1)
			{
				if (temp >= 0)
				{
					med_m49.SPSFuelRemaining = temp * 0.45359237;
				}
			}
		}
		med_m49.CSMRCSFuelRemaining = -1;
		if (data.size() > 2 && data[2] != "")
		{
			if (sscanf(data[2].c_str(), "%lf", &temp) == 1)
			{
				if (temp >= 0)
				{
					med_m49.CSMRCSFuelRemaining = temp * 0.45359237;
				}
			}
		}
		med_m49.SIVBFuelRemaining = -1;
		if (data.size() > 3 && data[3] != "")
		{
			if (sscanf(data[3].c_str(), "%lf", &temp) == 1)
			{
				if (temp >= 0)
				{
					med_m49.SIVBFuelRemaining = temp * 0.45359237;
				}
			}
		}
		med_m49.LMAPSFuelRemaining = -1;
		if (data.size() > 4 && data[4] != "")
		{
			if (sscanf(data[4].c_str(), "%lf", &temp) == 1)
			{
				if (temp >= 0)
				{
					med_m49.LMAPSFuelRemaining = temp * 0.45359237;
				}
			}
		}
		med_m49.LMRCSFuelRemaining = -1;
		if (data.size() > 5 && data[5] != "")
		{
			if (sscanf(data[5].c_str(), "%lf", &temp) == 1)
			{
				if (temp >= 0)
				{
					med_m49.LMRCSFuelRemaining = temp * 0.45359237;
				}
			}
		}
		med_m49.LMDPSFuelRemaining = -1;
		if (data.size() > 6 && data[6] != "")
		{
			if (sscanf(data[6].c_str(), "%lf", &temp) == 1)
			{
				if (temp >= 0)
				{
					med_m49.LMDPSFuelRemaining = temp * 0.45359237;
				}
			}
		}
		PMMWTC(49);
	}
	//Change vehicle cross-sectional area or K-factor
	else if (med == "51")
	{
		if (data.size() < 1)
		{
			return 1;
		}
		int tab;
		if (data[0] == "CSM")
		{
			tab = RTCC_MPT_CSM;
		}
		else if (data[0] == "LEM")
		{
			tab = RTCC_MPT_LM;
		}
		else
		{
			return 2;
		}
		med_m51.Table = tab;
		double temp;
		if (data.size() > 1 && data[1] != "" && sscanf(data[1].c_str(), "%lf", &temp) == 1)
		{
			med_m51.CSMArea = temp * 0.3048*0.3048;
		}
		else
		{
			med_m51.CSMArea = -1.0;
		}
		if (data.size() > 2 && data[2] != "" && sscanf(data[2].c_str(), "%lf", &temp) == 1)
		{
			med_m51.SIVBArea = temp * 0.3048*0.3048;
		}
		else
		{
			med_m51.SIVBArea = -1.0;
		}
		if (data.size() > 3 && data[3] != "" && sscanf(data[3].c_str(), "%lf", &temp) == 1)
		{
			med_m51.LMAscentArea = temp * 0.3048*0.3048;
		}
		else
		{
			med_m51.LMAscentArea = -1.0;
		}
		if (data.size() > 4 && data[4] != "" && sscanf(data[4].c_str(), "%lf", &temp) == 1)
		{
			med_m51.LMDescentArea = temp * 0.3048*0.3048;
		}
		else
		{
			med_m51.LMDescentArea = -1.0;
		}
		if (data.size() > 5 && data[5] != "" && sscanf(data[5].c_str(), "%lf", &temp) == 1)
		{
			med_m51.KFactor = temp;
		}
		else
		{
			med_m51.KFactor = -10000.0;
		}
		PMMWTC(51);
	}
	//Input initialization parameters
	else if (med == "55")
	{
		if (data.size() < 1)
		{
			return 1;
		}
		int mpt;
		if (data[0] == "CSM")
		{
			mpt = RTCC_MPT_CSM;
		}
		else if (data[0] == "LEM")
		{
			mpt = RTCC_MPT_LM;
		}
		else
		{
			return 2;
		}
		med_m55.Table = mpt;
		if (data.size() < 2 || data[1] == "")
		{
			med_m55.ConfigCode = "";
		}
		else
		{
			med_m55.ConfigCode = data[1];
		}
		if (data.size() < 3 || data[2] == "")
		{
			med_m55.VentingGET = -1.0;
		}
		else
		{
			double hrs;
			if (MEDTimeInputHHMMSS(data[2], hrs))
			{
				return 2;
			}
			med_m55.VentingGET = hrs * 3600.0;
		}
		if (data.size() < 4 || data[3] == "")
		{
			med_m55.DeltaDockingAngle = -720.0;
		}
		else
		{
			double ang;
			if (sscanf(data[3].c_str(), "%lf", &ang) != 1)
			{
				return 2;
			}
			med_m55.DeltaDockingAngle = ang * RAD;
		}
		PMMWTC(55);
	}
	//Change vehicle body orientation and trim angles for MPT maneuver
	else if (med == "58")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0] == "LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		unsigned man;
		if (sscanf(data[1].c_str(), "%d", &man) != 1)
		{
			return 2;
		}
		if (man < 1 || man>15)
		{
			return 2;
		}
		int headsup;
		if (data.size() < 3)
		{
			headsup = -1;
		}
		else if (data[2] == "U")
		{
			headsup = 1;
		}
		else if (data[2] == "D")
		{
			headsup = 0;
		}
		else
		{
			return 2;
		}
		int trim;
		if (data.size() < 4)
		{
			trim = -1;
		}
		else if (data[3] == "S")
		{
			trim = 2;
		}
		else if (data[3] == "C")
		{
			trim = 0;
		}
		else
		{
			return 2;
		}
		PMMUDT(veh, man, headsup, trim);
	}
	else if (med == "62")
	{
		int mpt, man, act;

		if (data.size() < 3)
		{
			return 1;
		}

		if (data[0] == "")
		{
			return 2;
		}
		else if (data[0] == "CSM")
		{
			mpt = 1;
		}
		else if (data[0] == "LEM")
		{
			mpt = 3;
		}
		else
		{
			return 2;
		}

		if (data[1] == "")
		{
			return 2;
		}
		if (sscanf(data[1].c_str(), "%d", &man) != 1)
		{
			return 2;
		}
		if (man < 1 || man > 15)
		{
			return 2;
		}

		if (data[2] == "")
		{
			return 2;
		}
		else if (data[2] == "F")
		{
			act = 2;
		}
		else if (data[2] == "U")
		{
			act = 3;
		}
		else if (data[2] == "D")
		{
			act = 0;
		}
		else if (data[2] == "DH")
		{
			act = 1;
		}
		else
		{
			return 2;
		}
		std::string StationID;
		if (data.size() <= 3)
		{
			StationID = "";
		}
		else
		{
			StationID = data[3];
		}

		PMMFUD(mpt, man, act, StationID);
	}
	//Transfer a GPM to the MPT
	else if (med == "65")
	{
		PMMXFR_Impulsive_Input inp;

		inp.Attitude[0] = med_m65.Attitude;
		inp.DeleteGMT = 0.0;
		inp.DPSScaleFactor[0] = med_m65.DPSThrustFactor;
		inp.DT10P[0] = med_m65.TenPercentDT;

		if (med_m65.UllageDT > 0 && med_m65.UllageDT <= 1)
		{
			return 2;
		}
		if (med_m65.UllageDT < 0)
		{
			inp.dt_ullage[0] = SystemParameters.MCTNDU;
		}
		else
		{
			inp.dt_ullage[0] = med_m65.UllageDT;
		}

		inp.IterationFlag[0] = med_m65.Iteration;
		inp.Plan = med_m65.Table;
		inp.ReplaceCode = med_m65.ReplaceCode;
		inp.Thruster[0] = med_m65.Thruster;
		inp.TimeFlag[0] = med_m65.TimeFlag;
		inp.UllageThrusterOption[0] = med_m65.UllageQuads;

		void *vPtr = &inp;
		return PMMXFR(40, vPtr);
	}
	//Direct Input to MPT
	else if (med == "66")
	{
		PMMXFRDirectInput inp;

		if (med_m66.Table != 1 && med_m66.Table != 3)
		{
			return 2;
		}
		inp.TableCode = med_m66.Table;

		if (med_m66.ReplaceCode < 0 || med_m66.ReplaceCode > 15)
		{
			return 2;
		}
		inp.ReplaceCode = med_m66.ReplaceCode;
		double GMT = GMTfromGET(med_m66.GETBI);
		if (GMT < RTCCPresentTimeGMT())
		{
			return 2;
		}
		inp.GMTI = GMT;
		inp.ThrusterCode = med_m66.Thruster;

		if (med_m66.Thruster == RTCC_ENGINETYPE_LOX_DUMP && med_m66.AttitudeOpt != RTCC_ATTITUDE_INERTIAL)
		{
			return 2;
		}
		if ((med_m66.AttitudeOpt == RTCC_ATTITUDE_PGNS_ASCENT || med_m66.AttitudeOpt == RTCC_ATTITUDE_AGS_ASCENT) && med_m66.Thruster != RTCC_ENGINETYPE_LMAPS)
		{
			return 2;
		}

		inp.AttitudeCode = med_m66.AttitudeOpt;

		if (med_m66.BurnParamNo == 1 && !(med_m66.AttitudeOpt == RTCC_ATTITUDE_INERTIAL || med_m66.AttitudeOpt == RTCC_ATTITUDE_MANUAL))
		{
			return 2;
		}
		if (med_m66.BurnParamNo == 2 && !(med_m66.AttitudeOpt == RTCC_ATTITUDE_PGNS_EXDV || med_m66.AttitudeOpt == RTCC_ATTITUDE_AGS_EXDV))
		{
			return 2;
		}
		if (med_m66.BurnParamNo == 3 || med_m66.BurnParamNo == 4)
		{
			if (!(med_m66.AttitudeOpt == RTCC_ATTITUDE_INERTIAL || med_m66.AttitudeOpt == RTCC_ATTITUDE_MANUAL || med_m66.AttitudeOpt == RTCC_ATTITUDE_PGNS_EXDV || med_m66.AttitudeOpt == RTCC_ATTITUDE_AGS_EXDV))
			{
				return 2;
			}
		}
		if (med_m66.BurnParamNo == 6 && !(med_m66.AttitudeOpt == RTCC_ATTITUDE_PGNS_ASCENT || med_m66.AttitudeOpt == RTCC_ATTITUDE_AGS_ASCENT))
		{
			return 2;
		}

		inp.BurnParameterNumber = med_m66.BurnParamNo;
		if (med_m66.BurnParamNo == 1)
		{
			inp.CoordinateIndicator = med_m66.CoordInd;
		}
		else
		{
			inp.CoordinateIndicator = -1;
		}
		inp.Pitch = med_m66.Att.x;
		inp.Yaw = med_m66.Att.y;
		inp.Roll = med_m66.Att.z;

		if (med_m66.UllageDT > 0 && med_m66.UllageDT <= 1)
		{
			return 2;
		}
		if (med_m66.UllageDT < 0)
		{
			inp.dt_ullage = SystemParameters.MCTNDU;
		}
		else
		{
			inp.dt_ullage = med_m66.UllageDT;
		}

		inp.UllageThrusterOption = med_m66.UllageQuads;
		inp.HeadsUpDownIndicator = med_m66.HeadsUp;

		if (med_m66.TenPercentDT == -1)
		{
			inp.DT10P = SystemParameters.MCTDD4;
		}
		else if (med_m66.TenPercentDT == 0)
		{
			inp.DT10P = -(SystemParameters.MCTDD2 + SystemParameters.MCTDD3);
		}
		else if (abs(med_m66.TenPercentDT) < (SystemParameters.MCTDD2 + SystemParameters.MCTDD3))
		{
			return 2;
		}
		else
		{
			inp.DT10P = med_m66.TenPercentDT;
		}

		if (med_m66.REFSMMATInd < 1 || med_m66.REFSMMATInd > 10)
		{
			inp.REFSMMATIndicator = RTCC_REFSMMAT_TYPE_CUR;
		}
		else
		{
			inp.REFSMMATIndicator = med_m66.REFSMMATInd;
		}

		if (med_m66.ConfigChangeInd < 0 || med_m66.ConfigChangeInd > 2)
		{
			inp.ConfigurationChangeIndicator = RTCC_CONFIGCHANGE_NONE;
		}
		else
		{
			inp.ConfigurationChangeIndicator = med_m66.ConfigChangeInd;
		}

		MPTGetConfigFromString(med_m66.FinalConfig, inp.EndConfiguration);
		if (inp.EndConfiguration.to_ulong() == 0)
		{
			return 2;
		}
		inp.DockingAngle = med_m66.DeltaDA*SystemParameters.MCCRPD;

		if (med_m66.DPSThrustFactor <= 0 || med_m66.DPSThrustFactor > 1)
		{
			return 2;
		}
		else if (med_m66.DPSThrustFactor == -1)
		{
			inp.DPSScaleFactor = SystemParameters.MCTDTF;
		}
		else
		{
			inp.DPSScaleFactor = med_m66.DPSThrustFactor;
		}

		if (med_m66.TrimAngleIndicator != 0 && med_m66.TrimAngleIndicator != 2)
		{
			inp.TrimAngleIndicator = 0;
		}
		else
		{
			inp.TrimAngleIndicator = med_m66.TrimAngleIndicator;
		}

		void *vPtr = &inp;

		return PMMXFR(33, vPtr);
	}
	//TLI Direct Input
	else if (med == "68")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0] == "LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		int opp;
		if (sscanf(data[1].c_str(), "%d", &opp) != 1)
		{
			return 2;
		}
		if (opp < 1 || opp > 2)
		{
			return 2;
		}

		PMMXFRDirectInput inp;

		inp.ReplaceCode = 0;
		inp.TableCode = veh;
		inp.BurnParameterNumber = opp;

		void *vPtr = &inp;
		return PMMXFR(37, vPtr);
	}
	//Transfer Descent Plan, DKI or SPQ
	else if (med == "70")
	{
		PMMXFR_Impulsive_Input inp;

		inp.Attitude[0] = med_m70.Attitude;
		if (med_m70.DeleteGET <= 0)
		{
			inp.DeleteGMT = 0.0;
		}
		else
		{
			inp.DeleteGMT = GMTfromGET(med_m70.DeleteGET);
		}
		inp.DPSScaleFactor[0] = med_m70.DPSThrustFactor;
		inp.DT10P[0] = med_m70.TenPercentDT;

		if (med_m70.UllageDT > 0 && med_m70.UllageDT <= 1)
		{
			return 2;
		}
		if (med_m70.UllageDT < 0)
		{
			inp.dt_ullage[0] = SystemParameters.MCTNDU;
		}
		else
		{
			inp.dt_ullage[0] = med_m70.UllageDT;
		}

		inp.IterationFlag[0] = med_m70.Iteration;
		inp.Plan = med_m70.Plan;
		inp.ReplaceCode = 0;
		inp.Thruster[0] = med_m70.Thruster;
		inp.TimeFlag[0] = med_m70.TimeFlag;
		inp.UllageThrusterOption[0] = med_m70.UllageQuads;

		void *vPtr = &inp;
		return PMMXFR(41, vPtr);
	}
	//Transfer two-impulse plan to MPT
	else if (med == "72")
	{
		PMMXFR_Impulsive_Input inp;

		inp.Attitude[0] = inp.Attitude[1] = med_m72.Attitude;
		if (med_m72.DeleteGET <= 0)
		{
			inp.DeleteGMT = 0.0;
		}
		else
		{
			inp.DeleteGMT = GMTfromGET(med_m72.DeleteGET);
		}
		inp.DPSScaleFactor[0] = inp.DPSScaleFactor[1] = med_m72.DPSThrustFactor;
		inp.DT10P[0] = inp.DT10P[1] = med_m72.TenPercentDT;

		if (med_m72.UllageDT > 0 && med_m72.UllageDT <= 1)
		{
			return 2;
		}
		if (med_m72.UllageDT < 0)
		{
			inp.dt_ullage[0] = SystemParameters.MCTNDU;
		}
		else
		{
			inp.dt_ullage[0] = med_m72.UllageDT;
		}
		inp.dt_ullage[1] = inp.dt_ullage[0];

		inp.IterationFlag[0] = inp.IterationFlag[1] = med_m72.Iteration;
		inp.Plan = med_m72.Plan;
		inp.ReplaceCode = 0;
		inp.Thruster[0] = inp.Thruster[1] = med_m72.Thruster;
		inp.TimeFlag[0] = inp.TimeFlag[1] = med_m72.TimeFlag;
		inp.UllageThrusterOption[0] = inp.UllageThrusterOption[1] = med_m72.UllageQuads;

		void *vPtr = &inp;
		return PMMXFR(42, vPtr);
	}
	//Transfer of planned Earth entry maneuver
	else if (med == "74")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0] == "LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		int ReplaceCode;
		if (data[1] == "")
		{
			ReplaceCode = 0;
		}
		else
		{
			if (sscanf(data[1].c_str(), "%d", &ReplaceCode) != 1)
			{
				return 2;
			}
			if (ReplaceCode < 1 || ReplaceCode > 15)
			{
				return 2;
			}
		}
		int type;
		if (data[2] == "SCS")
		{
			//TBD
			return 2;
		}
		else if (data[2] == "TTFP")
		{
			type = 1;
		}
		else if (data[2] == "TTFM")
		{
			type = 3;
		}
		else if (data[2] == "RTEP")
		{
			type = 2;
		}
		else if (data[2] == "RTEM")
		{
			type = 4;
		}
		else
		{
			return 2;
		}

		PMMXFRDirectInput inp;

		inp.AttitudeCode = RTCC_ATTITUDE_PGNS_EXDV;
		inp.BurnParameterNumber = type;
		inp.ConfigurationChangeIndicator = RTCC_CONFIGCHANGE_NONE; //TBD
		inp.CoordinateIndicator = -1;
		inp.DPSScaleFactor = SystemParameters.MCTDTF;
		inp.ReplaceCode = ReplaceCode;
		inp.TableCode = veh;
		inp.EndConfiguration = 0; //TBD

		void *vPtr = &inp;
		return PMMXFR(32, vPtr);
	}
	//LOI and MCC Transfer
	else if (med == "78")
	{
		PMMXFR_Impulsive_Input inp;

		inp.Table = med_m78.Table;
		inp.Plan = med_m78.ManeuverNumber;
		inp.Attitude[0] = med_m78.Attitude;
		inp.DeleteGMT = 0.0;
		inp.DPSScaleFactor[0] = med_m78.DPSThrustFactor;
		inp.DT10P[0] = med_m78.TenPercentDT;

		if (med_m78.UllageDT > 0 && med_m78.UllageDT <= 1)
		{
			return 2;
		}
		if (med_m78.UllageDT < 0)
		{
			inp.dt_ullage[0] = SystemParameters.MCTNDU;
		}
		else
		{
			inp.dt_ullage[0] = med_m78.UllageDT;
		}

		inp.IterationFlag[0] = med_m78.Iteration;
		inp.ReplaceCode = med_m78.ReplaceCode;
		if (med_m78.Type)
		{
			inp.Type = 1;
			if (inp.Plan < 1 || inp.Plan > 8)
			{
				return 2;
			}
		}
		else
		{
			inp.Type = 0;
			if (inp.Plan < 1 || inp.Plan > 4)
			{
				return 2;
			}
		}
		inp.Thruster[0] = med_m78.Thruster;
		inp.TimeFlag[0] = med_m78.TimeFlag;
		inp.UllageThrusterOption[0] = med_m78.UllageQuads;

		void *vPtr = &inp;
		return PMMXFR(39, vPtr);
	}
	//Transfer ascent maneuver to MPT from lunar targeting
	else if (med == "85")
	{
		void *vPtr = NULL;
		return PMMXFR(44, vPtr);
	}
	//Direct input of lunar descent maneuver
	else if (med == "86")
	{
		void *vPtr = NULL;
		return PMMXFR(43, vPtr);
	}
	return 0;
}

int RTCC::GMSMED(std::string med, std::vector<std::string> data)
{
	//Enter planned or actual liftoff time
	if (med == "10")
	{
		if (data.size() < 2 || data.size() > 3)
		{
			return 1;
		}
		int Veh;
		if (data[0] == "CSM")
		{
			Veh = 1;
		}
		else if (data[0] == "LEM")
		{
			Veh = 2;
		}
		else
		{
			return 2;
		}
		double hours;
		if (MEDTimeInputHHMMSS(data[1], hours))
		{
			return 2;
		}
		if (hours < 0)
		{
			return 2;
		}

		double gmtlocs = GLHTCS(hours);
		if (Veh == 1)
		{
			SystemParameters.MGLGMT = gmtlocs;
			SystemParameters.MCGMTL = hours;
		}
		else
		{
			SystemParameters.MGGGMT = gmtlocs;
			SystemParameters.MCGMTS = hours;
		}
		if (data.size() == 3)
		{
			if (data[2] == "" || data[2] == "NOTRAJ")
			{
				//Nothing
			}
			else if (data[2] == "TRAJ")
			{
				//Allowed mission phases: NPHASE, PRELCH, PRELCH2 L.S.
				if (SystemParameters.MGGPHS <= 1 || SystemParameters.MGGPHS == 11)
				{
					PMMIEV(Veh, hours*3600.0);
				}
				else
				{
					GMSPRINT("GMGPMED", 46);
					return 2;
				}
			}
			else
			{
				return 2;
			}
		}
	}
	//Enter planned or actual guidance reference release time and launch azimuth
	else if (med == "12")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 0;
			//If new REFSMMAT already has been established this MED is illegal
			if (EZJGMTX1.data[0].ID > 1)
			{
				return 1;
			}
		}
		else if (data[0] == "IU1")
		{
			veh = 1;
		}
		else if (data[0] == "IU2")
		{
			veh = 2;
		}

		double GMTGRR;
		if (MEDTimeInputHHMMSS(data[1], GMTGRR))
		{
			return 2;
		}
		if (GMTGRR < 0)
		{
			return 2;
		}
		double Azi;
		if (sscanf(data[2].c_str(), "%lf", &Azi) != 1)
		{
			return 2;
		}
		if (Azi < 35.0 || Azi > 120.0)
		{
			return 2;
		}

		SystemParameters.MCLABN = Azi *RAD;
		SystemParameters.MCLSBN = sin(SystemParameters.MCLABN);
		SystemParameters.MCLCBN = cos(SystemParameters.MCLABN);
		if (veh == 0)
		{
			SystemParameters.MCGRAG = GMTGRR;
		}
		else if (veh == 1)
		{
			SystemParameters.MCGRIC = GMTGRR;
		}
		else
		{
			SystemParameters.MCGRIL = GMTGRR;
		}

		LMMGRP(veh, GMTGRR*3600.0);
	}
	//Modify A & E used to determine integration limits
	else if (med == "30")
	{
		double val;
		if (data.size() > 0 && data[0] != "")
		{
			if (sscanf(data[0].c_str(), "%lf", &val) != 1)
			{
				return 2;
			}
			if (val < 0)
			{
				return 2;
			}
			SystemParameters.MCGSMA = val / SystemParameters.MCCNMC;
		}
		if (data.size() > 1 && data[1] != "")
		{
			if (sscanf(data[1].c_str(), "%lf", &val) != 1)
			{
				return 2;
			}
			if (val < 0)
			{
				return 2;
			}
			SystemParameters.MCGECC = val;
		}
	}
	//Initialize phase reference time - GET
	else if (med == "31")
	{
		if (data.size() != 1)
		{
			return 1;
		}
		int hh, mm;
		double ss;
		if (sscanf(data[0].c_str(), "%d:%d:%lf", &hh, &mm, &ss) != 3)
		{
			return 2;
		}
		double get = (double)hh + (double)mm / 60.0 + ss / 3600.0;
		if (get < 0)
		{
			return 2;
		}
		SystemParameters.MCGREF = get;
	}
	//Change venting multiplier and DT to end vent from TB7
	else if (med == "33")
	{
		if (data.size() != 2)
		{	
			return 1;
		}
		double val;
		if (sscanf(data[0].c_str(), "%lf", &val) != 1)
		{
			return 2;
		}
		if (val < 0.0)
		{
			return 2;
		}
		SystemParameters.MCTVEN = val;

		if (MEDTimeInputHHMMSS(data[1].c_str(), val))
		{
			return 2;
		}
		if (val < 0.0)
		{
			return 2;
		}
		SystemParameters.MCGVNT = val;
	}
	//Update coefficients of lift and drag for new center of gravity
	else if (med == "07")
	{
		return 1;
	}
	//Update GMTZS for specified vehicle
	else if (med == "15")
	{
		if (data.size() < 1)
		{
			return 1;
		}
		int Veh;
		if (data[0] == "AGC")
		{
			Veh = 1;
		}
		else if (data[0] == "LGC")
		{
			Veh = 2;
		}
		else if (data[0] == "AGS")
		{
			Veh = 3;
		}
		else
		{
			return 2;
		}
		double hours;
		if (Veh == 2)
		{
			if (data.size() > 1)
			{
				if (MEDTimeInputHHMMSS(data[1].c_str(), hours))
				{
					return 2;
				}
				SystemParameters.MCGZSL = hours;
			}
		}
		else if (Veh == 1)
		{
			if (data.size() > 1)
			{
				if (MEDTimeInputHHMMSS(data[1].c_str(), hours))
				{
					return 2;
				}
				SystemParameters.MCGZSA = hours;
			}
		}
		else
		{
			if (data.size() > 1 && data[1] != "")
			{
				if (MEDTimeInputHHMMSS(data[1].c_str(), hours))
				{
					return 2;
				}
			}
			else
			{
				hours = SystemParameters.MCGZSL;
			}
			double dt = 0.0;
			if (data.size() > 2 && data[2] != "")
			{
				if (MEDTimeInputHHMMSS(data[2].c_str(), dt))
				{
					return 2;
				}
			}
			SystemParameters.MCGZSS = hours + dt;
		}
	}
	//Phase initialization, transition, and recycle
	else if (med == "81")
	{
		//TBD
	}
	//Manually EOF and rewind log tape
	else if (med == "82")
	{
		//TBD
	}
	//Generate an ephemeris for one vehicle using a vector from the other vehicle
	else if (med == "16")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int veh1, veh2;
		if (data[0] == "CSM")
		{
			veh1 = 1;
		}
		else if (data[0] == "LEM")
		{
			veh1 = 3;
		}
		else
		{
			return 2;
		}
		if (data[1] == "CSM")
		{
			veh2 = 1;
		}
		else if (data[1] == "LEM")
		{
			veh2 = 3;
		}
		else
		{
			return 2;
		}
		double gmt;
		unsigned mnv;
		bool use_mnv;

		//Error: both GMT and maneuver used
		if (data.size() > 3 && data[2] != "" && data[3] != "")
		{
			return 2;
		}
		
		//Use maneuver
		if (data.size() > 3 && data[3] != "")
		{
			use_mnv = true;
			if (sscanf(data[3].c_str(), "%d", &mnv) != 1)
			{
				return 2;
			}
			if (mnv <= 0)
			{
				return 2;
			}
		}
		else
		{
			use_mnv = false;
			if (data[2] == "")
			{
				gmt = 0.0;
			}
			else
			{
				double hh, mm, ss;
				if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) == 3)
				{
					return 2;
				}
				gmt = hh * 3600.0 + mm * 60.0 + ss;
			}
		}
		EphemerisData sv0;
		std::string StationID;
		MissionPlanTable *mpt;
		bool landed;
		if (veh1 == RTCC_MPT_CSM)
		{
			mpt = &PZMPTCSM;
		}
		else
		{
			mpt = &PZMPTLEM;
		}
		StationID = mpt->StationID;
		if (use_mnv)
		{
			
			if (mpt->mantable.size() < mnv)
			{
				return 2;
			}
			sv0.R = mpt->mantable[mnv - 1].R_BO;
			sv0.V = mpt->mantable[mnv - 1].V_BO;
			sv0.GMT = mpt->mantable[mnv - 1].GMT_BO;
			sv0.RBI = mpt->mantable[mnv - 1].RefBodyInd;
			if (mpt->mantable[mnv - 1].AttitudeCode == RTCC_ATTITUDE_PGNS_DESCENT)
			{
				landed = true;
			}
			else
			{
				landed = false;
			}
		}
		else
		{
			if (gmt == 0)
			{
				gmt = RTCCPresentTimeGMT();
			}

			EphemerisDataTable2 EPHEM;
			ManeuverTimesTable MANTIMES;
			LunarStayTimesTable LUNSTAY;

			if (ELFECH(gmt, 1, 1, veh1, EPHEM, MANTIMES, LUNSTAY))
			{
				return 2;
			}
			sv0 = RotateSVToSOI(EPHEM.table[0]);
			if (gmt >= LUNSTAY.LunarStayBeginGMT && gmt <= LUNSTAY.LunarStayEndGMT)
			{
				landed = true;
			}
			else
			{
				landed = false;
			}
		}
		StateVectorTableEntry sv1;
		sv1.Vector = sv0;
		sv1.LandingSiteIndicator = landed;
		sv1.VectorCode = StationID;
		PMSVCT(4, veh2, sv1);
	}
	//Condition for Launch (Simulation)
	else if (med == "92")
	{
		//TBD
	}
	//Cape crossing table update and limit change
	else if (med == "17")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0] == "LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		int body;
		if (data[1] == "E")
		{
			body = BODY_EARTH;
		}
		else if (data[1] == "M")
		{
			body = BODY_MOON;
		}
		else
		{
			return 2;
		}
		int rev;
		if (data.size() < 3 || data[2] == "")
		{
			rev = 0;
		}
		else
		{
			if (sscanf(data[2].c_str(), "%d", &rev) != 1)
			{
				return 2;
			}
			if (rev <= 0)
			{
				return 2;
			}
		}
		double min_get, max_get, hh, mm, ss;
		if (data.size() < 4 || data[3] == "")
		{
			min_get = -1.0;
		}
		else
		{
			if (sscanf(data[3].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			min_get = hh * 3600.0 + mm * 60.0 + ss;
			if (min_get < 0)
			{
				return 2;
			}
		}
		if (data.size() < 5 || data[4] == "")
		{
			max_get = -1.0;
		}
		else
		{
			if (sscanf(data[4].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			max_get = hh * 3600.0 + mm * 60.0 + ss;
			if (max_get < 0)
			{
				return 2;
			}
		}
		
		CapeCrossingTable *table;

		if (veh == 1)
		{
			table = &EZCCSM;
		}
		else
		{
			table = &EZCLEM;
		}

		if (table->NumRev == 0) return 2;

		int delta = rev + 1 - table->NumRevFirst;

		table->NumRevFirst += delta;
		table->NumRevLast += delta;
	}
	//Update Fixed Ground Point Table and Landmark Acquisition Ground Point Table
	else if (med == "32")
	{
		if (data.size() < 4)
		{
			return 1;
		}
		int ActionCode;
		if (data[0] == "ADD")
		{
			ActionCode = 0;
		}
		else if (data[0] == "MOD")
		{
			ActionCode = 1;
		}
		else if (data[0] == "DEL")
		{
			ActionCode = 2;
		}
		else
		{
			return 2;
		}
		int BodyInd;
		if (data[1] == "E")
		{
			BodyInd = BODY_EARTH;
		}
		else if (data[1] == "M")
		{
			BodyInd = BODY_MOON;
		}
		int DataTableInd;
		if (data[2] == "EXST")
		{
			DataTableInd = 0;
		}
		else if (data[2] == "LDMK")
		{
			DataTableInd = 1;
		}
		else
		{
			return 2;
		}
		std::string STAID = data[3];
		if (STAID == "" || STAID.size() > 4)
		{
			return 2;
		}
		if (ActionCode == 2)
		{
			if (data.size() != 4)
			{
				return 1;
			}
		}
		else
		{
			if (data.size() != 8)
			{
				return 1;
			}
		}

		//Read rest of values
		double lat, lng, height;
		int HeightUnits;
		if (ActionCode != 2)
		{
			if (sscanf(data[4].c_str(), "%lf", &lat) != 1)
			{
				return 2;
			}
			lat *= RAD;
			if (sscanf(data[5].c_str(), "%lf", &lng) != 1)
			{
				return 2;
			}
			lng *= RAD;
			if (data[6] == "NM")
			{
				HeightUnits = 0;
			}
			else if (data[6] == "METR")
			{
				HeightUnits = 1;
			}
			else
			{
				return 2;
			}
			if (sscanf(data[7].c_str(), "%lf", &height) != 1)
			{
				return 2;
			}
			if (HeightUnits == 0)
			{
				height *= 1852.0;
			}
		}

		//Delete all logic
		if (ActionCode == 2 && STAID == "ALL")
		{
			if (DataTableInd == 0)
			{
				EZEXSITE.REF = -1;
			}
			else
			{
				EZLASITE.REF = -1;
			}

			for (int i = 0;i < 12;i++)
			{
				if (DataTableInd == 0)
				{
					EZEXSITE.Data[i].code = "";
				}
				else
				{
					EZLASITE.Data[i].code = "";
				}
			}
		}
		else
		{
			//Add
			if (ActionCode == 0)
			{
				if (DataTableInd == 0)
				{
					if (EZEXSITE.REF >= 0 && BodyInd != EZEXSITE.REF)
					{
						GMSPRINT("GMSMED", 32);
						return 2;
					}
				}
				else
				{
					if (EZLASITE.REF >= 0 && BodyInd != EZLASITE.REF)
					{
						GMSPRINT("GMSMED", 32);
						return 2;
					}
				}

				bool tablefull = true;
				int freeslot = -1;
				std::string tempname;
				for (int i = 0;i < 12;i++)
				{
					if (DataTableInd == 0)
					{
						tempname = EZEXSITE.Data[i].code;
					}
					else
					{
						tempname = EZLASITE.Data[i].code;
					}

					if (STAID == tempname)
					{
						GMSPRINT("GMSMED", 23);
						return 2;
					}
					if (tempname == "")
					{
						tablefull = false;
						if (freeslot == -1)
						{
							freeslot = i;
						}
					}
				}
				if (tablefull)
				{
					GMSPRINT("GMSMED", 24);
					return 2;
				}
				if (DataTableInd == 0)
				{
					if (freeslot == 0)
					{
						EZEXSITE.REF = BodyInd;
					}
					EZEXSITE.Data[freeslot].code = STAID;
					EMGGPCHR(lat, lng, height, BodyInd, 0.0, &EZEXSITE.Data[freeslot]);
				}
				else
				{
					if (freeslot == 0)
					{
						EZLASITE.REF = BodyInd;
					}
					EZLASITE.Data[freeslot].code = STAID;
					EMGGPCHR(lat, lng, height, BodyInd, 0.0, &EZLASITE.Data[freeslot]);
				}
			}
			//MOD
			else if (ActionCode == 1)
			{
				std::string tempname;
				int found = -1;
				for (int i = 0;i < 12;i++)
				{
					if (DataTableInd == 0)
					{
						tempname = EZEXSITE.Data[i].code;
					}
					else
					{
						tempname = EZLASITE.Data[i].code;
					}
					if (STAID == tempname)
					{
						found = i;
						break;
					}
				}
				if (found == -1)
				{
					GMSPRINT("GMSMED", 25);
					return 2;
				}
				if (DataTableInd == 0)
				{
					EMGGPCHR(lat, lng, height, BodyInd, 0.0, &EZEXSITE.Data[found]);
				}
				else
				{
					EMGGPCHR(lat, lng, height, BodyInd, 0.0, &EZLASITE.Data[found]);
				}
			}
			//DEL
			else
			{
				std::string tempname;
				int found = -1;
				for (int i = 0;i < 12;i++)
				{
					if (DataTableInd == 0)
					{
						tempname = EZEXSITE.Data[i].code;
					}
					else
					{
						tempname = EZLASITE.Data[i].code;
					}
					if (STAID == tempname)
					{
						found = i;
						break;
					}
				}
				if (found == -1)
				{
					GMSPRINT("GMSMED", 25);
					return 2;
				}
				if (DataTableInd == 0)
				{
					EZEXSITE.Data[found].code = "";
				}
				else
				{
					EZLASITE.Data[found].code = "";
				}
				//Is table now empty?
				bool empty = true;
				for (int i = 0;i < 12;i++)
				{
					if (DataTableInd == 0)
					{
						tempname = EZEXSITE.Data[i].code;
					}
					else
					{
						tempname = EZLASITE.Data[i].code;
					}
					if (tempname != "")
					{
						empty = false;
						break;
					}
				}
				if (empty)
				{
					if (DataTableInd == 0)
					{
						EZEXSITE.REF = -1;
					}
					else
					{
						EZLASITE.REF = -1;
					}
				}
			}
		}
	}
	//Enter vector
	else if (med == "13" || med == "14")
	{
		//TBD
	}
	//High speed processing control
	else if (med == "60")
	{
		//TBD
	}
	//Offsets and elevation angle for two-impulse solution
	else if (med == "51")
	{
		double val;
		if (data.size() > 0 && data[0] != "")
		{
			if (sscanf(data[0].c_str(), "%lf", &val) == 1)
			{
				GZGENCSN.TIDeltaH = val * 1852.0;
			}
		}
		if (data.size() > 1 && data[1] != "")
		{
			if (sscanf(data[1].c_str(), "%lf", &val) == 1)
			{
				GZGENCSN.TIPhaseAngle = val * RAD;
			}
		}
		if (data.size() > 2 && data[2] != "")
		{
			if (sscanf(data[2].c_str(), "%lf", &val) == 1)
			{
				GZGENCSN.TIElevationAngle = val * RAD;
			}
		}
		if (data.size() > 3 && data[3] != "")
		{
			if (sscanf(data[3].c_str(), "%lf", &val) == 1)
			{
				GZGENCSN.TITravelAngle = val * RAD;
			}
		}
	}
	//Two-impulse corrective combination nominals
	else if (med == "52")
	{
		double val;
		if (data.size() > 0 && data[0] != "")
		{
			double hh, mm, ss;
			if (sscanf(data[0].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) == 3)
			{
				val = hh * 3600.0 + mm * 60.0 + ss;
				GZGENCSN.TINSRNominalTime = val;
			}
		}
		if (data.size() > 1 && data[1] != "")
		{
			if (sscanf(data[1].c_str(), "%lf", &val) == 1)
			{
				GZGENCSN.TINSRNominalDeltaH = val * 1852.0;
			}
		}
		if (data.size() > 2 && data[2] != "")
		{
			if (sscanf(data[2].c_str(), "%lf", &val) == 1)
			{
				GZGENCSN.TINSRNominalPhaseAngle = val * RAD;
			}
		}
	}
	//Update pitch angle from horizon
	else if (med == "08")
	{
		if (data.size() != 1)
		{
			return 1;
		}
		double val;
		if (sscanf(data[0].c_str(), "%lf", &val) != 1)
		{
			return 2;
		}
		SystemParameters.MCGHZA = val * RAD;
	}
	//Initialize with mission type and day of year
	else if (med == "80")
	{
		if (data.size() < 5)
		{
			return 1;
		}
		int Num;
		if (sscanf(data[0].c_str(), "%d", &Num) != 1)
		{
			return 2;
		}
		if (Num != 1)
		{
			return 2;
		}

		bool found = false;
		for (unsigned i = 0;i < MHGVNM.tab.size();i++)
		{
			if (data[1] == MHGVNM.tab[i])
			{
				SystemParameters.MCGPRM = data[1];
				found = true;
			}
		}
		if (found == false)
		{
			return 2;
		}
		int Month, Day, Year;
		if (sscanf(data[2].c_str(), "%d", &Month) != 1)
		{
			return 2;
		}
		if (sscanf(data[3].c_str(), "%d", &Day) != 1)
		{
			return 2;
		}
		if (sscanf(data[4].c_str(), "%d", &Year) != 1)
		{
			return 2;
		}

		if (1960 <= Year && Year <= 1980)
		{
			GZGENCSN.Year = Year;
		}
		else
		{
			//Invalid date
			GMSPRINT("GMGPMED", 40);
			return 2;
		}
		if (1 <= Month && Month <= 12)
		{
			GZGENCSN.MonthofLiftoff = Month;
		}
		else
		{
			//Invalid date
			GMSPRINT("GMGPMED", 40);
			return 2;
		}
		if (1 <= Day && Day <= 31)
		{
			GZGENCSN.DayofLiftoff = Day;
		}
		else
		{
			//Invalid date
			GMSPRINT("GMGPMED", 40);
			return 2;
		}

		int m[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
		double Y_J = (double)GZGENCSN.Year + 0.001;
		double J = Y_J - 1960.0;
		int K = (int)(J / 4);
		J = floor(J);
		if (((int)J) == 4 * K)
		{
			m[1] = 29;
			GZGENCSN.DaysInYear = 366;
		}
		else
		{
			GZGENCSN.DaysInYear = 365;
		}
		double T = (double)GZGENCSN.DayofLiftoff + 0.5;
		J = floor(T);
		double T_apo = (double)GZGENCSN.MonthofLiftoff - 0.5;
		K = (int)T_apo;
		GZGENCSN.DaysinMonthofLiftoff = m[K];
		GZGENCSN.RefDayOfYear = (int)J;

		if (K != 0)
		{
			for (int i = 0;i < K;i++)
			{
				GZGENCSN.RefDayOfYear += m[i];
			}
		}

		double J_D = OrbMech::TJUDAT(Year, Month, Day);
		SystemParameters.GMTBASE = J_D - 2400000.5;

		SystemParameters.MCLAMD = PIGBHA();
		//TBD: Call EMLAMBNP
	}
	return 0;
}

int RTCC::GMSREMED(std::string med, std::vector<std::string> data)
{
	//Restart Tape
	if (med == "XX")
	{
		if (data.size() != 1)
		{
			return 1;
		}
		int type;
		if (data[0] == "B")
		{
			type = 1;
		}
		else if (data[0] == "R")
		{
			type = 2;
		}
		else
		{
			return 2;
		}

		GMLRESRT(type);
	}
	return 0;
}

void RTCC::GMLRESRT(int type)
{
	//RTCC saving
	if (type == 1)
	{

	}
	//RTCC loading
	else if (type == 2)
	{

	}
}

int RTCC::EMGTVMED(std::string med, std::vector<std::string> data)
{
	//Space Digitals Initialization
	if (med == "00")
	{
		if (data.size() < 1)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0]=="LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		int body;
		if (data.size() < 2 || data[1] == "" || data[1] == "E")
		{
			body = BODY_EARTH;
		}
		else if (data[1] == "M")
		{
			body = BODY_MOON;
		}
		else
		{
			return 2;
		}
		EZETVMED.SpaceDigVehID = veh;
		EZETVMED.SpaceDigCentralBody = body;

		EMDSPACE(7);
	}
	//Space Digitals
	else if (med == "01")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int column;
		if (sscanf(data[0].c_str(), "%d", &column) != 1)
		{
			return 2;
		}
		if (column < 1 || column > 3)
		{
			return 2;
		}
		int opt;
		if (data[1] == "GET")
		{
			opt = 0;
		}
		else if (data[1] == "MNV")
		{
			opt = 1;
		}
		else
		{
			return 2;
		}
		double val;
		if (opt == 0)
		{
			double hh, mm, ss;
			if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			val = hh * 3600.0 + mm * 60.0 + ss;
		}
		else
		{
			int man;
			if (sscanf(data[2].c_str(), "%d", &man) != 1)
			{
				return 2;
			}
			val = (double)man;
		}

		double incl = 0, ascnode = 0;
		if (column == 2)
		{
			if (data.size() < 5)
			{
				return 1;
			}
			if (sscanf(data[3].c_str(), "%lf", &incl) != 1)
			{
				return 2;
			}
			incl *= RAD;
			if (incl < 0 || incl > PI)
			{
				return 2;
			}
			if (sscanf(data[4].c_str(), "%lf", &ascnode) != 1)
			{
				return 2;
			}
			ascnode *= RAD;
		}
		int queid;
		if (column == 1)
		{
			queid = 3;
		}
		else if (column == 2)
		{
			queid = 4;
		}
		else
		{
			queid = 5;
		}

		EMDSPACE(queid, opt, val, incl, ascnode);
	}
	//Initiate Checkout Monitor Display
	else if (med == "02")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int tab;
		if (data[0] == "CSM")
		{
			tab = 1;
		}
		else if (data[0] == "LEM")
		{
			tab = 3;
		}
		else
		{
			return 2;
		}
		int opt;
		if (data[1] == "GMT")
		{
			opt = 1;
		}
		else if (data[1] == "GET")
		{
			opt = 2;
		}
		else if (data[1] == "MVI")
		{
			opt = 3;
		}
		else if (data[1] == "MVE")
		{
			opt = 4;
		}
		else if (data[1] == "RAD")
		{
			opt = 5;
		}
		else if (data[1] == "ALT")
		{
			opt = 6;
		}
		else if (data[1] == "FPA")
		{
			opt = 7;
		}
		else
		{
			return 2;
		}
		double param;
		if (opt == 1 || opt == 2)
		{
			double hh, mm, ss;
			if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			param = hh * 3600.0 + mm * 60.0 + ss;
		}
		else
		{
			if (sscanf(data[2].c_str(), "%lf", &param) != 1)
			{
				return 2;
			}
			if (opt == 5 || opt == 6)
			{
				param *= 1852.0;
			}
			else if (opt == 7)
			{
				param = sin(param*RAD);
			}
		}
		bool hasTHT;
		double THTime = -1.0;
		if (data.size() < 4 || data[3] == "")
		{
			hasTHT = false;
		}
		else
		{
			hasTHT = true;
		}
		//Illegal for MVI and MVE
		if (hasTHT && (opt == 3 || opt == 4))
		{
			return 2;
		}
		if (opt >= 5)
		{
			//Mandatory for RAD, ALT, FPA
			if (hasTHT == false)
			{
				return 2;
			}
			double hh, mm, ss;
			if (sscanf(data[3].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			THTime = hh * 3600.0 + mm * 60.0 + ss;
		}
		else if (opt <= 2)
		{
			if (hasTHT)
			{
				double hh, mm, ss;
				if (sscanf(data[3].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
				{
					return 2;
				}
				THTime = hh * 3600.0 + mm * 60.0 + ss;
			}
			else
			{
				THTime = -1.0;
			}
		}
		int ref;
		if (data.size() < 5 || data[4] == "" || data[4] == "ECI")
		{
			ref = 0;
		}
		else if (data[4] == "ECT")
		{
			ref = 1;
		}
		else if (data[4] == "MCI")
		{
			ref = 2;
		}
		else if (data[4] == "MCT")
		{
			ref = 3;
		}
		else
		{
			return 2;
		}
		bool feet;
		if (data.size() < 6 || data[5] == "")
		{
			feet = false;
		}
		else if (data[5] == "FT")
		{
			feet = true;
		}
		else
		{
			return 2;
		}
		EMDCHECK(tab, opt, param, THTime, ref, feet);
	}
	//Update Relative Motion Digital Display
	else if (med == "03")
	{
		if (data.size() < 7)
		{
			return 1;
		}
		int Veh1, Veh2;
		if (data[0] == "CSM")
		{
			Veh1 = 1;
		}
		else if (data[0] == "LEM")
		{
			Veh1 = 3;
		}
		else
		{
			return 2;
		}
		if (data[1] == "CSM")
		{
			Veh2 = 1;
		}
		else if (data[1] == "LEM")
		{
			Veh2 = 3;
		}
		else
		{
			return 2;
		}
		if (Veh1 == Veh2)
		{
			return 2;
		}
		double get;
		if (MEDTimeInputHHMMSS(data[2], get))
		{
			return 2;
		}
		if (get < 0)
		{
			return 2;
		}
		double dt;
		if (sscanf(data[3].c_str(), "%lf", &dt) != 1)
		{
			return 2;
		}
		if (dt<1.0 || dt>1800.0)
		{
			return 2;
		}
		int refs;
		if (data[4] == "")
		{
			refs = 1;
		}
		else
		{
			refs = EMGSTGENCode(data[4].c_str());
			if (refs < 0)
			{
				return 2;
			}
		}
		int axis;
		if (Veh1 == 1)
		{
			if (data[5] != "CX")
			{
				return 2;
			}
			axis = 1;
		}
		else
		{
			if (data[5] == "LX")
			{
				axis = 1;
			}
			else if (data[5] == "LZ")
			{
				axis = 2;
			}
			else
			{
				return 2;
			}
		}
		int ref_body;
		if (data[6] == "E")
		{
			ref_body = BODY_EARTH;
		}
		else if (data[6] == "M")
		{
			ref_body = BODY_MOON;
		}
		int mode = 1;
		if (data.size() > 7)
		{
			if (data[7] == "" || data[7] == "1")
			{
				mode = 1;
			}
			else if (data[7] == "2")
			{
				mode = 2;
			}
			//In Mode 2 the LM needs to be the chaser
			if (mode == 2 && Veh1 == 1)
			{
				return 2;
			}
			if (mode == 2 && data.size() != 11)
			{
				return 1;
			}
		}
		EMMRMD(Veh1, Veh2, get*3600.0, dt, refs, axis, ref_body, mode);
	}
	//Generate Rendezvous Evaluation Display
	else if (med == "06")
	{
		if (data.size() != 1)
		{
			return 1;
		}
		int plan;
		if (sscanf(data[0].c_str(), "%d", &plan) != 1)
		{
			return 2;
		}
		if (plan < 1 || plan > 7)
		{
			return 2;
		}
		EZETVMED.RETPlan = plan;
		PMDRET();
	}
	//Moonrise/Moonset Display
	else if (med == "07")
	{
		if (data.size() < 1)
		{
			return 1;
		}
		int ind;

		if (data[0] == "GET")
		{
			ind = 1;
		}
		else if (data[0] == "REV")
		{
			ind = 2;
		}
		else
		{
			return 2;
		}
		double param;
		if (ind == 1)
		{
			if (data.size() < 2)
			{
				param = 0.0;
			}
			double hh, mm, ss;
			if (sscanf(data[1].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			param = hh * 3600.0 + mm * 60.0 + ss;
		}
		else
		{
			if (data.size() < 2)
			{
				return 1;
			}
			int rev;
			if (sscanf(data[1].c_str(), "%d", &rev) != 1)
			{
				return 2;
			}
			if (rev > EZCCSM.NumRevLast)
			{
				return 2;
			}
			param = (double)rev;
		}
		EMDSSMMD(ind, param);
	}
	//Sunrise/Sunset Display
	else if (med == "08")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int ind;

		if (data[0] == "GET")
		{
			ind = 1;
		}
		else if (data[0] == "REV")
		{
			ind = 2;
		}
		else
		{
			return 2;
		}
		double param;
		if (ind == 1)
		{
			double hh, mm, ss;
			if (sscanf(data[1].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			param = hh * 3600.0 + mm * 60.0 + ss;
		}
		else
		{
			int rev;
			if (sscanf(data[1].c_str(), "%d", &rev) != 1)
			{
				return 2;
			}
			if (rev > EZCCSM.NumRevLast)
			{
				return 2;
			}
			param = (double)rev;
		}
		EMDSSEMD(ind, param);
	}
	//AGS Navigation Update
	else if (med == "10")
	{
		
	}
	//FDO Orbit Digitals: Predict apogee/perigee
	else if (med == "12")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int L;
		if (data[0] == "CSM")
		{
			L = 1;
		}
		else if (data[0] == "LEM")
		{
			L = 3;
		}
		else
		{
			return 2;
		}

		int ind;
		if (data[1] == "REV")
		{
			ind = 1;
		}
		else if (data[1] == "GET")
		{
			ind = 2;
		}
		else if (data[1] == "MNV")
		{
			ind = 3;
		}
		else
		{
			return 2;
		}
		double param;
		if (ind == 1 || ind == 3)
		{
			int rev;
			if (sscanf(data[2].c_str(), "%d", &rev) != 1)
			{
				return 2;
			}
			param = (double)rev;
		}
		else
		{
			double hh, mm, ss;
			if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			param = hh * 3600.0 + mm * 60.0 + ss;
		}

		EMMDYNMC(L, 3, ind, param);
	}
	//FDO Orbit Digitals: Longitude crossing time
	else if (med == "13")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int L;
		if (data[0] == "CSM")
		{
			L = 1;
		}
		else if (data[0] == "LEM")
		{
			L = 3;
		}
		else
		{
			return 2;
		}

		int ind;
		if (sscanf(data[1].c_str(), "%d", &ind) != 1)
		{
			return 2;
		}

		double lng;
		if (sscanf(data[2].c_str(), "%lf", &lng) != 1)
		{
			return 2;
		}
		EMMDYNMC(L, 4, ind, lng*RAD);
	}
	//FDO Orbit Digitals: Compute longitude at given time
	else if (med == "14")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int L;
		if (data[0] == "CSM")
		{
			L = 1;
		}
		else if (data[0] == "LEM")
		{
			L = 3;
		}
		else
		{
			return 2;
		}

		double hh, mm, ss;
		if (sscanf(data[1].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
		{
			return 2;
		}
		double param = hh * 3600.0 + mm * 60.0 + ss;
		EMMDYNMC(L, 5, 0, param);
	}
	//Predicted and experimental site acquisition
	else if (med == "15" || med == "16" || med == "55")
	{
		if (data.size() < 4)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0] == "LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		int ind;
		if (data[1] == "REV")
		{
			ind = 0;
		}
		else if (data[1] == "GET")
		{
			ind = 1;
		}
		else
		{
			return 2;
		}
		double vala, valb;
		if (ind == 0)
		{
			int revb, reve;

			if (sscanf(data[2].c_str(), "%d", &revb) != 1)
			{
				return 2;
			}
			if (revb <= 0)
			{
				return 2;
			}
			if (sscanf(data[3].c_str(), "%d", &reve) != 1)
			{
				return 2;
			}
			if (reve <= 0)
			{
				return 2;
			}
			vala = (double)revb;
			valb = (double)reve;
		}
		else
		{
			double hh, mm, ss;
			double stime, dt;
			if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			stime = hh * 3600.0 + mm * 60.0 + ss;
			if (sscanf(data[3].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			dt = hh * 3600.0 + mm * 60.0 + ss;
			if (dt < 0 || dt > 24.0*3600.0)
			{
				return 2;
			}
			vala = stime;
			valb = stime + dt;
		}
		int body = 0;
		if (data.size() >= 5)
		{
			if (data[4] == "E")
			{
				body = 0;
			}
			else if (data[4] == "M")
			{
				body = 2;
			}
			else
			{
				return 2;
			}
		}
		
		int num;
		if (med == "15")
		{
			num = 1;
		}
		else if (med == "16")
		{
			num = 3;
		}
		else
		{
			num = 2;
		}

		//TBD: Store in EZETVMED

		EMDPESAD(num, veh, ind, vala, valb, body);
	}
	//Landmark Acquisition Display
	else if (med == "17")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		if (data[0] != "CSM")
		{
			return 2;
		}
		int veh = 1;
		double hh, mm, ss, get, dt;
		if (sscanf(data[1].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
		{
			return 2;
		}
		get = hh * 3600.0 + mm * 60.0 + ss;
		if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
		{
			return 2;
		}
		dt = hh * 3600.0 + mm * 60.0 + ss;
		int ref;
		if (data.size() < 4 || data[3] == "" || data[3] == "E")
		{
			ref = BODY_EARTH;
		}
		else if (data[3] == "M")
		{
			ref = BODY_MOON;
		}
		else
		{
			return 2;
		}
		double gmt = GMTfromGET(get);
		EZETVMED.LandmarkVehID = veh;
		EZETVMED.LandmarkGMT = gmt;
		EZETVMED.LandmarkDT = dt;
		EZETVMED.LandmarkRef = ref;

		EMDLANDM(veh, gmt, dt, ref);
	}
	//Detailed Maneuver Table
	else if (med == "20")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0] == "LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		int num;
		if (sscanf(data[1].c_str(), "%d", &num) != 1)
		{
			return 2;
		}
		if (num < 1 || num>15)
		{
			return 2;
		}

		DetailedManeuverTable *dmt;
		int dmtnum;
		if (data.size() < 3 || data[2] == "" || data[2] == "54")
		{
			dmt = &DMTBuffer[0];
			dmtnum = 1;
		}
		else if (data[2] == "69")
		{
			dmt = &DMTBuffer[1];
			dmtnum = 2;
		}
		else
		{
			return 2;
		}

		int refscode;
		if (data.size() < 4 || data[3] == "")
		{
			refscode = 1;
		}
		else if (data[3] == "DES")
		{
			refscode = 100;
		}
		else
		{
			refscode = EMGSTGENCode(data[3].c_str());
			if (refscode < 0)
			{
				return 2;
			}
		}

		bool headsupdownind;
		if (refscode == 100)
		{
			if (data.size() < 5 || data[4] == "" || data[4] == "U")
			{
				headsupdownind = true;
			}
			else if (data[4] == "D")
			{
				headsupdownind = false;
			}
			else
			{
				return 2;
			}
		}

		//Save inputs
		if (dmtnum == 1)
		{
			EZETVMED.DMT1Vehicle = veh;
			EZETVMED.DMT1Number = num;
			EZETVMED.DMT1REFSMMATCode = refscode;
			EZETVMED.DMT1HeadsUpDownIndicator = headsupdownind;
		}
		else
		{
			EZETVMED.DMT2Vehicle = veh;
			EZETVMED.DMT2Number = num;
			EZETVMED.DMT2REFSMMATCode = refscode;
			EZETVMED.DMT2HeadsUpDownIndicator = headsupdownind;
		}

		PMDDMT(veh, num, refscode, headsupdownind, *dmt);
	}
	return 0;
}

EphemerisData RTCC::ConvertSVtoEphemData(SV sv)
{
	EphemerisData svnew;

	svnew.R = sv.R;
	svnew.V = sv.V;
	svnew.GMT = OrbMech::GETfromMJD(sv.MJD,SystemParameters.GMTBASE);
	if (sv.gravref == hEarth)
	{
		svnew.RBI = BODY_EARTH;
	}
	else
	{
		svnew.RBI = BODY_MOON;
	}
	return svnew;
}

SV RTCC::ConvertEphemDatatoSV(EphemerisData sv, double mass)
{
	SV svnew;

	svnew.R = sv.R;
	svnew.V = sv.V;
	svnew.MJD = OrbMech::MJDfromGET(sv.GMT,SystemParameters.GMTBASE);
	if (sv.RBI == BODY_EARTH)
	{
		svnew.gravref = hEarth;
	}
	else
	{
		svnew.gravref = hMoon;
	}
	svnew.mass = mass;

	return svnew;
}

double RTCC::PCDETA(double beta1, double beta2, double r1, double r2)
{
	double ctn_eta122 = (DCOTAN(beta1) + r1 / r2 * DCOTAN(beta2)) / (1 - r1 / r2);
	double cos_eta12 = (pow(ctn_eta122, 2) - 1.0) / (pow(ctn_eta122, 2) + 1.0);
	double eta12 = atan2((1.0 - cos_eta12)*ctn_eta122, cos_eta12);
	if (eta12 < 0)
	{
		eta12 += PI2;
	}
	return eta12;
}

double RTCC::DCOTAN(double ang)
{
	return 1.0 / tan(ang);
}

void RTCC::EMGSTGEN(int QUEID, int L1, int ID1, int L2, int ID2, double gmt, MATRIX3 *refs)
{
	//CSM/LEM REFSMMAT locker movement
	if (QUEID == 2)
	{
		REFSMMATLocker *tab1, *tab2;

		if (L1 == RTCC_MANVEHICLE_CSM)
		{
			tab1 = &EZJGMTX1;
		}
		else
		{
			tab1 = &EZJGMTX3;
		}
		if (L2 == RTCC_MANVEHICLE_CSM)
		{
			tab2 = &EZJGMTX1;
		}
		else
		{
			tab2 = &EZJGMTX3;
		}

		if (L1 == RTCC_MANVEHICLE_CSM)
		{
			if (ID1 >= RTCC_REFSMMAT_TYPE_AGS)
			{
				//Error
				return;
			}
			
		}
		else
		{
			
		}
		if (L2 == RTCC_MANVEHICLE_CSM)
		{
			if (!(ID2 == RTCC_REFSMMAT_TYPE_CUR || ID2 == RTCC_REFSMMAT_TYPE_PCR || ID2 == RTCC_REFSMMAT_TYPE_TLM || ID2 == RTCC_REFSMMAT_TYPE_MED || ID2 == RTCC_REFSMMAT_TYPE_LCV))
			{
				//Error
				return;
			}
		}
		else
		{
			if (ID2 == RTCC_REFSMMAT_TYPE_OST || ID2 == RTCC_REFSMMAT_TYPE_DMT || ID2 == RTCC_REFSMMAT_TYPE_DOK || ID2 == RTCC_REFSMMAT_TYPE_DOD || ID2 == RTCC_REFSMMAT_TYPE_LLD)
			{
				//Error
				return;
			}
		}

		if (tab1->data[ID1 - 1].ID == 0)
		{
			//ERROR: REFSMMAT NOT AVAILABLE
			EMGPRINT("EMGSTGEN", 1);
			return;
		}

		if (ID2 == RTCC_REFSMMAT_TYPE_CUR)
		{
			EMGSTSTM(L2, tab2->data[0].REFSMMAT, RTCC_REFSMMAT_TYPE_PCR, gmt);
		}
		EMGSTSTM(L2, tab1->data[ID1 - 1].REFSMMAT, ID2, gmt);
	}
	//Enter manual IMU matrix
	else if (QUEID == 3)
	{
		//Check allowed types
		switch (EZGSTMED.G01_Type)
		{
		case RTCC_REFSMMAT_TYPE_PCR:
		case RTCC_REFSMMAT_TYPE_TLM:
		case RTCC_REFSMMAT_TYPE_MED:
		case RTCC_REFSMMAT_TYPE_LCV:
			break;
		default:
			return;
		}

		//Check if orthogonal
		MATRIX3 Rot = mul(OrbMech::tmat(EZGSTMED.G01_REFSMMAT), EZGSTMED.G01_REFSMMAT);

		double tol = 1e-7;

		for (int i = 0;i < 9;i++)
		{
			if (i == 0 || i == 4 || i == 8)
			{
				if (abs(Rot.data[i] - 1.0) > tol) return;
			}
			else
			{
				if (abs(Rot.data[i]) > tol) return;
			}
		}

		MATRIX3 a = EZGSTMED.G01_REFSMMAT;
		EMGSTSTM(L1, a, EZGSTMED.G01_Type, gmt);
	}
}

void RTCC::EMGSTSTM(int L, MATRIX3 REFS, int id, double gmt)
{
	REFSMMATLocker *tab;
	if (L == RTCC_MANVEHICLE_CSM)
	{
		tab = &EZJGMTX1;
	}
	else
	{
		tab = &EZJGMTX3;
		//LM REFSMMATs start with code 501
		if (tab->data[id - 1].ID == 0)
		{
			tab->data[id - 1].ID = 500;
		}
	}

	tab->data[id - 1].ID++;
	tab->data[id - 1].REFSMMAT = REFS;
	tab->data[id - 1].GMT = gmt;

	char Buffer[128];
	char Buff1[4];
	char Buff2[32];
	char Buff3[4];

	EMGSTGENName(id, Buff1);
	OrbMech::format_time_HHMMSS(Buff2, GETfromGMT(gmt));
	if (L == 1)
	{
		sprintf_s(Buff3, "CSM");
	}
	else
	{
		sprintf_s(Buff3, "LEM");
	}

	sprintf_s(Buffer, "NEW IMU MATRIX %s%03d %s", Buff1, tab->data[id - 1].ID, Buff3);
	RTCCONLINEMON.TextBuffer[0].assign(Buffer);
	sprintf_s(Buffer, "GET = %s", Buff2);
	RTCCONLINEMON.TextBuffer[1].assign(Buffer);
	RTCCONLINEMON.MatrixBuffer = tab->data[id - 1].REFSMMAT;
	EMGPRINT("EMGSTSTM", 19);
}

void RTCC::FormatREFSMMATCode(int ID, int num, char *buff)
{
	char buff2[4];
	EMGSTGENName(ID, buff2);
	sprintf_s(buff, 7, "%s%03d", buff2, num);
}

void RTCC::EMGSTGENName(int ID, char *Buffer)
{
	switch (ID)
	{
	case RTCC_REFSMMAT_TYPE_CUR:
		sprintf_s(Buffer, 4, "CUR");
		break;
	case RTCC_REFSMMAT_TYPE_PCR:
		sprintf_s(Buffer, 4, "PCR");
		break;
	case RTCC_REFSMMAT_TYPE_TLM:
		sprintf_s(Buffer, 4, "TLM");
		break;
	case RTCC_REFSMMAT_TYPE_OST:
		sprintf_s(Buffer, 4, "OST");
		break;
	case RTCC_REFSMMAT_TYPE_MED:
		sprintf_s(Buffer, 4, "MED");
		break;
	case RTCC_REFSMMAT_TYPE_DMT:
		sprintf_s(Buffer, 4, "DMT");
		break;
	case RTCC_REFSMMAT_TYPE_DOD:
		sprintf_s(Buffer, 4, "DOD");
		break;
	case RTCC_REFSMMAT_TYPE_LCV:
		sprintf_s(Buffer, 4, "LCV");
		break;
	case RTCC_REFSMMAT_TYPE_AGS:
		sprintf_s(Buffer, 4, "AGS");
		break;
	case RTCC_REFSMMAT_TYPE_DOK:
		sprintf_s(Buffer, 4, "DOK");
		break;
	case RTCC_REFSMMAT_TYPE_LLA:
		sprintf_s(Buffer, 4, "LLA");
		break;
	case RTCC_REFSMMAT_TYPE_LLD:
		sprintf_s(Buffer, 4, "LLD");
		break;
	default:
		sprintf_s(Buffer, 4, "");
		break;
	}
}

int RTCC::EMGSTGENCode(const char *Buffer)
{
	if (!strcmp(Buffer, "CUR"))
	{
		return RTCC_REFSMMAT_TYPE_CUR;
	}
	else if (!strcmp(Buffer, "PCR"))
	{
		return RTCC_REFSMMAT_TYPE_PCR;
	}
	else if (!strcmp(Buffer, "TLM"))
	{
		return RTCC_REFSMMAT_TYPE_TLM;
	}
	else if (!strcmp(Buffer, "OST"))
	{
		return RTCC_REFSMMAT_TYPE_OST;
	}
	else if (!strcmp(Buffer, "MED"))
	{
		return RTCC_REFSMMAT_TYPE_MED;
	}
	else if (!strcmp(Buffer, "DMT"))
	{
		return RTCC_REFSMMAT_TYPE_DMT;
	}
	else if (!strcmp(Buffer, "DOD"))
	{
		return RTCC_REFSMMAT_TYPE_DOD;
	}
	else if (!strcmp(Buffer, "LCV"))
	{
		return RTCC_REFSMMAT_TYPE_LCV;
	}
	else if (!strcmp(Buffer, "AGS"))
	{
		return RTCC_REFSMMAT_TYPE_AGS;
	}
	else if (!strcmp(Buffer, "DOK"))
	{
		return RTCC_REFSMMAT_TYPE_DOK;
	}
	else if (!strcmp(Buffer, "LLA"))
	{
		return RTCC_REFSMMAT_TYPE_LLA;
	}
	else if (!strcmp(Buffer, "LLD"))
	{
		return RTCC_REFSMMAT_TYPE_LLD;
	}

	return -1;
}

void RTCC::EMGPRINT(std::string source, int i)
{
	std::vector<std::string> message;
	char Buffer[128];

	switch (i)
	{
	case 1:
		message.push_back("REFSMMAT NOT AVAILABLE");
		break;
	case 2:
		message.push_back("INVALID REQUEST");
		break;
	case 3:
		message.push_back("INSERTION VECTOR IS " + RTCCONLINEMON.TextBuffer[0]);
		break;
	case 4:
		message.push_back("HISTORY VECTOR IS UNAVAILABLE");
		break;
	case 11:
		message.push_back(RTCCONLINEMON.TextBuffer[0] + " BELOW " + RTCCONLINEMON.TextBuffer[1] + " ALTITUDE");
		if (RTCCONLINEMON.DoubleBuffer[0] >= 0)
		{
			sprintf_s(Buffer, "GET = %s LAT = %06.2lf N", RTCCONLINEMON.TextBuffer[2].c_str(), RTCCONLINEMON.DoubleBuffer[0]);
		}
		else
		{
			sprintf_s(Buffer, "GET = %s LAT = %06.2lf S", RTCCONLINEMON.TextBuffer[2].c_str(), abs(RTCCONLINEMON.DoubleBuffer[0]));
		}
		message.push_back(Buffer);
		if (RTCCONLINEMON.DoubleBuffer[1] >= 0)
		{
			sprintf_s(Buffer, "LON = %06.2lf E", RTCCONLINEMON.DoubleBuffer[1]);
		}
		else
		{
			sprintf_s(Buffer, "LON = %06.2lf W", abs(RTCCONLINEMON.DoubleBuffer[1]));
		}
		message.push_back(Buffer);
		break;
	case 12:
		message.push_back(RTCCONLINEMON.TextBuffer[0] + " EPHEMERIS UPDATE COMPLETED");
		break;
	case 13:
		message.push_back(RTCCONLINEMON.TextBuffer[0] + " MINIMUM EPHEMERIS WAS NOT GENERATED");
		break;
	case 14:
		sprintf_s(Buffer, "%s ORBITAL ELEM. FOR UPDATE NO. %d", RTCCONLINEMON.TextBuffer[0].c_str(), RTCCONLINEMON.IntBuffer[0]);
		message.push_back(Buffer);
		sprintf_s(Buffer, "A = %.1lf NM MEAN ANOMALY = %.2lf H(AP) = %.0lf", RTCCONLINEMON.DoubleBuffer[0], RTCCONLINEMON.DoubleBuffer[1], RTCCONLINEMON.DoubleBuffer[2]);
		message.push_back(Buffer);
		sprintf_s(Buffer, "E = %.3lf ARG. PERIGEE = %.2lf H(PER) = %.0lf", RTCCONLINEMON.DoubleBuffer[3], RTCCONLINEMON.DoubleBuffer[4], RTCCONLINEMON.DoubleBuffer[5]);
		message.push_back(Buffer);
		sprintf_s(Buffer, "I = %.2lf ARG ASCEND. NODE = %.2lf", RTCCONLINEMON.DoubleBuffer[6], RTCCONLINEMON.DoubleBuffer[7]);
		message.push_back(Buffer);
		break;
	case 15:
		message.push_back(RTCCONLINEMON.TextBuffer[0] + " EPHEMERIS LIMITS");
		message.push_back(RTCCONLINEMON.TextBuffer[1] + " TO " + RTCCONLINEMON.TextBuffer[2] + " GMT");
		break;
	case 17:
		message.push_back("ERROR RETURN FROM MANEUVER");
		sprintf_s(Buffer, "INTEGRATOR, ERROR CODE = %d", RTCCONLINEMON.IntBuffer[0]);
		message.push_back(Buffer);
		sprintf_s(Buffer, "MANEUVER NO. = %d MPT = %s", RTCCONLINEMON.IntBuffer[1], RTCCONLINEMON.TextBuffer[0].c_str());
		message.push_back(Buffer);
		break;
	case 19:
		message.push_back(RTCCONLINEMON.TextBuffer[0]);
		message.push_back(RTCCONLINEMON.TextBuffer[1]);
		sprintf_s(Buffer, "%+.7lf %+.7lf %+.7lf", RTCCONLINEMON.MatrixBuffer.m11, RTCCONLINEMON.MatrixBuffer.m12, RTCCONLINEMON.MatrixBuffer.m13);
		message.push_back(Buffer);
		sprintf_s(Buffer, "%+.7lf %+.7lf %+.7lf", RTCCONLINEMON.MatrixBuffer.m21, RTCCONLINEMON.MatrixBuffer.m22, RTCCONLINEMON.MatrixBuffer.m23);
		message.push_back(Buffer);
		sprintf_s(Buffer, "%+.7lf %+.7lf %+.7lf", RTCCONLINEMON.MatrixBuffer.m31, RTCCONLINEMON.MatrixBuffer.m32, RTCCONLINEMON.MatrixBuffer.m33);
		message.push_back(Buffer);
		break;
	case 20:
		message.push_back("ATTITUDE NOT AVAILABLE");
		break;
	case 21:
		message.push_back("MANEUVER NOT IN PLAN");
		break;
	case 22:
		message.push_back("DOD NOT VALID FOR SAVE");
		break;
	case 23:
		message.push_back("OPTICS NOT AVAILABLE");
		break;
	case 24:
		message.push_back("TLM DATA NOT VALID FOR SAVE");
		break;
	case 25:
		message.push_back("EPHEMERIS NOT AVAILABLE");
		break;
	case 26:
		message.push_back("RADIAL FLIGHT CONDITIONS");
		break;
	case 29:
		message.push_back("RMMASCND CONVERGENCE LIMIT");
		break;
	case 31:
		sprintf_s(Buffer, "%d %s STATION CONTACTS GENERATED", RTCCONLINEMON.IntBuffer[0], RTCCONLINEMON.TextBuffer[0].c_str());
		message.push_back(Buffer);
		break;
	case 32:
		message.push_back("UNRECOGNIZABLE ENTRY");
		break;
	case 33:
		//TBD: Error nn from EMXING for veh, sta
		break;
	case 34:
		//TBD: RTE digitals not valid for save
		break;
	case 35:
		//TBD: Error nn unable to generate veh station constants
		break;
	case 36:
		//TBD: EMDGAGSN: Vector order of interpolation has been changed from 8 to NN
		break;
	case 37:
		sprintf_s(Buffer, "TABLED STAR ARC - %.3lf DEGREES", RTCCONLINEMON.DoubleBuffer[0]);
		message.push_back(Buffer);
		sprintf_s(Buffer, "S/C SIGHTING ARC - %.3lf DEGREES", RTCCONLINEMON.DoubleBuffer[1]);
		message.push_back(Buffer);
		break;
	case 38:
		//TBD: CSM/LEM maneuver number 1-15 not in MPT
		break;
	case 39:
		message.push_back("OST REFSMMAT STAR OPTICS NOT VALID");
		break;
	case 46:
		message.push_back("C-BAND CONTACT GENERATION " + RTCCONLINEMON.TextBuffer[0]);
		break;
	case 47:
		message.push_back("ANCHOR TIME ADJUSTED FOR" + RTCCONLINEMON.TextBuffer[0] + "STATION CONTACTS");
		break;
	case 48:
		message.push_back("GRR MATRIX NOT AVAILABLE");
		break;
	case 53:
		message.push_back("ANCHOR VECTOR IS LUNAR SURFACE");
		message.push_back("NO ELEMENTS CALCULATED");
		break;
	case 55:
		sprintf_s(Buffer, "ERROR %d FROM EMXING", RTCCONLINEMON.IntBuffer[0]);
		message.push_back(Buffer);
		message.push_back("FOR " + RTCCONLINEMON.TextBuffer[0] + ", "  +RTCCONLINEMON.TextBuffer[1]);
		break;
	default:
		return;
	}
	EMGPRINT(source, message);
}

void RTCC::EMGPRINT(std::string source, std::vector<std::string> message)
{
	OnlinePrint(source, message);
}

void RTCC::EMMGLCVP(int L, double gmt, int body)
{
	EphemerisDataTable2 EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;
	int csiout;

	if (ELFECH(gmt, 16, 8, L, EPHEM, MANTIMES, LUNSTAY))
	{
		EMGPRINT("EMMGLCVP", 25);
		return;
	}

	if (body == BODY_EARTH)
	{
		csiout = 0;
	}
	else
	{
		csiout = 2;
	}

	ELVCNV(EPHEM.table, 0, csiout, EPHEM.table);
	EPHEM.Header.CSI = csiout;

	ELVCTRInputTable in;
	ELVCTROutputTable2 out;

	in.GMT = gmt;

	ELVCTR(in, out, EPHEM, MANTIMES, &LUNSTAY);

	if (out.ErrorCode)
	{
		EMGPRINT("EMMGLCVP", 25);
		return;
	}

	if (abs(dotp(unit(out.SV.R), unit(out.SV.V))) > 1.0 - 0.0017)
	{
		EMGPRINT("EMMGLCVP", 26);
		return;
	}

	MATRIX3 REFSMMAT;
	VECTOR3 X_SM, Y_SM, Z_SM;
	if (L == 1)
	{
		Z_SM = unit(-out.SV.R);
		Y_SM = unit(crossp(out.SV.V, out.SV.R));
		X_SM = crossp(Y_SM, Z_SM);
	}
	else
	{
		X_SM = unit(out.SV.R);
		Y_SM = unit(crossp(out.SV.V, out.SV.R));
		Z_SM = crossp(X_SM, Y_SM);
	}
	REFSMMAT = _M(X_SM.x, X_SM.y, X_SM.z, Y_SM.x, Y_SM.y, Y_SM.z, Z_SM.x, Z_SM.y, Z_SM.z);

	EMGSTSTM(L, REFSMMAT, RTCC_REFSMMAT_TYPE_LCV, gmt);
}

void RTCC::PCBBT(double *DELT, double *WDI, double *TU, double W, double TIMP, double DELV, int NPHASE, double &T, double &GMTBB, double &GMTI, double &WA)
{
	//INPUTS:
	//DELT: Array of phase times
	//WDI: Array of weight loss rates
	//TU: Array of thrust levels
	//W: Initial mass
	//TIMP: Impulsive maneuver time
	//DELV: Delta V of maneuver
	//NPHASE: Number of phases

	//OUTPUTS:
	//T: Total time of maneuver
	//GMTBB: Ullage on time
	//GMTI: Main engine on time
	//WA: Weight after maneuver

	double C, D, WD, TT, TH, WDOT, S, WF, A, B, DELVW, DETU;
	int I = 1;
	T = 0.0;
	C = 0.0;
	D = 0.0;
	WD = W;
	DELVW = DELV;
	DETU = DELT[0];
	if (NPHASE == 1)
	{
		DETU = 0.0;
		goto RTCC_PCBBT_2_B;
	}
RTCC_PCBBT_1_A:
	TT = DELT[I - 1];
	TH = TU[I - 1];
	WDOT = WDI[I - 1];
RTCC_PCBBT_1_B:
	S = TH / WDOT;
	WF = WD - WDOT * TT;
	A = S * log(WD / WF);
	B = (WD / WDOT + T)*A - S * TT;
	if (I >= NPHASE)
	{
		goto RTCC_PCBBT_2_C;
	}
	if (DELVW > A)
	{
		goto RTCC_PCBBT_2_C;
	}
RTCC_PCBBT_2_B:
	TH = TU[I - 1];
	WDOT = WDI[I - 1];
	TT = WD / WDOT * (1.0 - exp(-WDOT * DELVW / TH));
	I = NPHASE;
	goto RTCC_PCBBT_1_B;
RTCC_PCBBT_2_C:
	C = C + A;
	D = D + B;
	T = T + TT;
	DELVW = DELVW - A;
	WD = WF;
	I = I + 1;
	if (I < NPHASE)
	{
		goto RTCC_PCBBT_1_A;
	}
	else if (I == NPHASE)
	{
		goto RTCC_PCBBT_2_B;
	}
	GMTBB = TIMP - D / C;
	GMTI = GMTBB + DETU;
	WA = WD;
}

void RTCC::PCMATO(double **A, double *Y, double **B, double *X, int M, int N, double *W1, double lambda, double *W2)
{
	double SUM;
	int I, J, K;

	I = 0;
	do
	{
		J = 0;
		do
		{
			SUM = 0.0;
			K = 0;
			do
			{
				SUM = SUM + B[K][I] * B[K][J] * W1[K];
				K++;
			} while (K < M);
			A[I][J] = SUM;
			J++;
		} while (J <= I);
		I++;
	} while (I < N);
	I = 0;
	do
	{
		J = I + 1;
		do
		{
			A[I][J] = A[J][I];
			J++;
		} while (J < N);
		I++;
	} while (I < N - 1);
	I = 0;
	do
	{
		SUM = 0.0;
		K = 0;
		do
		{
			SUM = SUM + B[K][I] * X[K] * W1[K];
			K++;
		} while (K < M);
		Y[I] = SUM;
		I++;
	} while (I < N);
	I = 0;
	do
	{
		A[I][I] = A[I][I] + lambda * W2[I];
		I++;
	} while (I < N);
}

bool RTCC::PCGAUS(double **A, double *Y, double *X, int N, double eps)
{
	int *PP = new int[N + 1];

	if (OrbMech::LUPDecompose(A, N, 0.0, PP) == 0)
	{
		return true;
	}
	std::vector<double> dx;
	dx.resize(N);
	OrbMech::LUPSolve(A, PP, Y, N, dx);
	delete[] PP;

	for (int i = 0;i < N;i++)
	{
		X[i] = dx[i];
	}

	/*double AMAX, DUMI;
	int I, J, K, *IP, IMAX, JMAX, IDUM;
	IP = new int[N];

	for (I = 0;I < N;I++)
	{
		IP[I] = I;
	}
	for (K = 0;K < N;K++)
	{
		if (K != N - 1)
		{
			IMAX = K;
			AMAX = abs(A[K][K]);
			JMAX = K;
			I = 0;
			do
			{
				//Find maximum element
				J = 0;
				do
				{
					DUMI = abs(A[I][J]);
					if (AMAX < DUMI)
					{
						AMAX = DUMI;
						IMAX = I;
						JMAX = J;
					}
					J++;
				} while (J < N);
				I++;
			} while (I < N);
			if (IMAX != K)
			{
				//Interchange rows
				I = K;
				do
				{
					DUMI = A[IMAX][I];
					A[IMAX][I] = A[K][I];
					A[K][I] = DUMI;
					I++;
				} while (I < N);
				DUMI = Y[IMAX];
				Y[IMAX] = Y[K];
				Y[K] = DUMI;
			}
			if (JMAX != K)
			{
				//Interchange columns
				I = 0;
				do
				{
					DUMI = A[I][K];
					A[I][K] = A[I][JMAX];
					A[I][JMAX] = DUMI;
					I++;
				} while (I < N);
				IDUM = IP[K];
				IP[K] = IP[JMAX];
				IP[JMAX] = IDUM;
			}
		}
		if (abs(A[K][K]) <= eps)
		{
			//error
			return true;
		}

		for (I = K + 1;I < N;I++)
		{
			DUMI = A[I][K] / A[K][K];
			for (J = K + 1;J < N;J++)
			{
				A[I][J] = A[I][J] - DUMI * A[K][J];
			}
			Y[I] = Y[I] - DUMI * Y[K];
		}

		
		//Normalize row
		for (I = K + 1;I < N;I++)
		{
			A[K][I] = A[K][I] / A[K][K];
		}
		Y[K] = Y[K] / A[K][K];
		A[K][K] = 1.0;
		//Reduce matrix
		for (I = 0;I < N;I++)
		{
			if (I != K)
			{
				for (J = K + 1;J < N;J++)
				{
					A[I][J] = A[I][J] - A[I][K] * A[K][J];
				}
				Y[I] = Y[I] - A[I][K] * Y[K];
			}
		}
		
		K++;
	}

	I = 0;
	do
	{
		X[IP[I]] = Y[I];
		I++;
	} while (I < N);

	delete[] IP;
	*/
	return false;
}

void RTCC::PMMDAB(EphemerisData2 XIN, ASTInput ARIN, ASTSettings IRIN, ASTData &AST, int &IER, int IPRT)
{
	EphemerisData2 sv_r_ECT, sv_r_ECI;

	//Set up inputs for forward iterator
	bool err;
	void *constPtr;
	PCMATCArray outarray;
	outarray.R0 = XIN.R;
	outarray.V0 = XIN.V;
	outarray.T0 = XIN.GMT;
	outarray.REF = IRIN.Ref;
	outarray.IASD = 0;
	outarray.ISTP = IRIN.ReentryStop;
	outarray.gamma_stop = ARIN.gamma_stop;
	outarray.GMTBASE = ARIN.GMTBASE;
	if (IRIN.Ref == BODY_EARTH)
	{
		outarray.h_pc_on = false;
	}
	else
	{
		outarray.h_pc_on = true;
	}
	
	constPtr = &outarray;

	bool PCMATCPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &PCMATCPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	//block.IndVarSwitch[4] = true;
	block.IndVarGuess[0] = ARIN.dgamma;
	block.IndVarGuess[1] = ARIN.dpsi;
	block.IndVarGuess[2] = ARIN.dv*3600.0 / OrbMech::R_Earth;
	block.IndVarGuess[3] = 0.0;//ARIN.T_a / 3600.0;
	block.IndVarGuess[4] = ARIN.T_r / 3600.0;
	block.IndVarStep[0] = pow(2, -21);
	block.IndVarStep[1] = pow(2, -21);
	block.IndVarStep[2] = pow(2, -21);
	block.IndVarWeight[0] = 8.0;
	block.IndVarWeight[1] = 8.0;
	block.IndVarWeight[2] = 1.0;

	block.DepVarSwitch[0] = true;
	if (IRIN.FuelCritical == false)
	{
		block.DepVarSwitch[1] = true;
		block.DepVarSwitch[2] = true;
		block.DepVarSwitch[3] = true;
	}
	block.DepVarSwitch[4] = true;
	if (IRIN.ReentryStop == -1)
	{
		block.DepVarSwitch[5] = true;
		block.DepVarWeight[5] = 1.0;
	}
	if (IRIN.FuelCritical)
	{
		block.DepVarSwitch[7] = true;
	}
	block.DepVarLowerLimit[0] = (ARIN.h_r_des - 0.3*1852.0) / OrbMech::R_Earth;
	block.DepVarLowerLimit[1] = ARIN.lat_r_des - 0.1*RAD;
	block.DepVarLowerLimit[2] = ARIN.lng_r_des - 0.01*RAD;
	block.DepVarLowerLimit[3] = ARIN.azi_r_des - 2.0*RAD;
	block.DepVarLowerLimit[4] = (ARIN.T_r - ARIN.T_a) / 3600.0 - 12.0;
	block.DepVarLowerLimit[5] = ARIN.gamma_des - 0.001*RAD;
	block.DepVarLowerLimit[7] = 1.5;
	block.DepVarUpperLimit[0] = (ARIN.h_r_des + 0.3*1852.0) / OrbMech::R_Earth;
	block.DepVarUpperLimit[1] = ARIN.lat_r_des + 0.1*RAD;
	block.DepVarUpperLimit[2] = ARIN.lng_r_des + 0.01*RAD;
	block.DepVarUpperLimit[3] = ARIN.azi_r_des + 2.0*RAD;
	block.DepVarUpperLimit[4] = (ARIN.T_r - ARIN.T_a) / 3600.0 + 12.0;
	block.DepVarUpperLimit[5] = ARIN.gamma_des + 0.001*RAD;
	block.DepVarUpperLimit[7] = 1.5;
	block.DepVarClass[0] = 1;
	block.DepVarClass[1] = 1;
	block.DepVarClass[2] = 1;
	block.DepVarClass[3] = 2;
	block.DepVarClass[4] = 2;
	block.DepVarClass[5] = 1;
	block.DepVarClass[7] = 3;
	block.DepVarWeight[3] = 1.0;
	block.DepVarWeight[4] = 1.0;
	block.DepVarWeight[7] = 0.5;

	std::vector<double> result;
	std::vector<double> y_vals;
	err = GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
	if (err)
	{
		goto RTCC_PMMDAB_3B;
	}

	//TBD: Off target line?
	double GMT_L, lat_imp, lng_imp;
	sv_r_ECI.R = outarray.R_r;
	sv_r_ECI.V = outarray.V_r;
	sv_r_ECI.GMT = outarray.T_r;
	EntryCalculations::LNDING(sv_r_ECI.R, sv_r_ECI.V, sv_r_ECI.GMT, SystemParameters.MCLAMD, 0.3, IRIN.EntryProfile, ARIN.RRBIAS, lng_imp, lat_imp, GMT_L);

	double r_r, v_r, lat_r, lng_r, gamma_r, azi_r;
	PICSSC(true, outarray.R_r, outarray.V_r, r_r, v_r, lat_r, lng_r, gamma_r, azi_r);

	VECTOR3 h = crossp(outarray.R_r, outarray.V_r);
	AST.incl_EI = acos(h.z / length(h));

	AST.AbortGMT = XIN.GMT;
	AST.dv = length(outarray.Vec3);
	AST.DV = outarray.Vec3;
	if (outarray.h_pc_on)
	{
		AST.h_PC = y_vals[6] * OrbMech::R_Earth;
	}
	else
	{
		AST.h_PC = 0.0;
	}
	AST.v_EI = v_r;
	AST.gamma_EI = gamma_r;
	AST.ReentryGMT = outarray.T_r;
	AST.lat_SPL = lat_imp;
	AST.lng_SPL = lng_imp;
	AST.SplashdownGMT = GMT_L;
	AST.MissDistance = 0.0;
	AST.RRBIAS = ARIN.RRBIAS;
	AST.sv_EI.R = outarray.R_r;
	AST.sv_EI.V = outarray.V_r;
	AST.sv_EI.GMT = AST.ReentryGMT;
	AST.lat_TGT = 0.0;
	AST.lng_TGT = 0.0;
	AST.sv_IG.R = XIN.R;
	AST.sv_IG.V = XIN.V;
	AST.sv_IG.GMT = XIN.GMT;
	AST.sv_IG.RBI = IRIN.Ref;
	IER = 0;

	if (PZREAP.RTEIsPTPSite)
	{

	}
	return;
RTCC_PMMDAB_3B:
	IER = 1;
}

bool PCMATCPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	return ((RTCC*)data)->PCMATC(var, varPtr, arr, mode);
}

bool RTCC::PCMATC(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	//Independent variables:
	//0: dgamma_a, R_tx or DVX
	//1: dpsi_a, R_ty or DVY
	//2: dv_a, R_tz or DVZ
	//3: DT of abort
	//4: T of reentry
	//Dependent variables:
	//0: Height at reentry
	//1: Geodetic latitude at reentry
	//2: Longitude at reentry
	//3: Azimuth at reentry
	//4: T of reentry (should be time from abort to reentry)
	//5: Gamma at reentry
	//6: Height at pericynthion
	//7: Sigma

	OELEMENTS coe;
	double gamma_p, T_min, h_p = 0.0, m_cut, dt_ar, t_a;
	int stop_ind;
	//Pre abort
	EphemerisData sv0;
	//Actual abort time
	EphemerisData sv1;
	//After burn
	EphemerisData sv2;
	//At reentry
	EphemerisData sv_r;

	PCMATCArray *vars = static_cast<PCMATCArray*>(varPtr);

	vars->ErrInd = 0;

	sv0.R = vars->R0;
	sv0.V = vars->V0;
	sv0.GMT = vars->T0;
	sv0.RBI = vars->REF;
	if (var[3] == 0.0)
	{
		sv1 = sv0;
	}
	else
	{
		double dir;
		if (var[3] > 0)
		{
			dir = 1.0;
		}
		else
		{
			dir = -1.0;
		}
		PMMCEN(sv0, 0.0, 24.0*3600.0, 1, var[3] * 3600.0, dir, sv1, stop_ind);
	}
	t_a = sv1.GMT;
	if (vars->IASD == 1)
	{
		goto PCMATC_5A;
	}
	double B[9];
	B[0] = 0.0;
	B[1] = 0.0;
	B[2] = var[2] * OrbMech::R_Earth / 3600.0;
	B[3] = 0.0;
	B[4] = var[0];
	B[5] = var[1];
	B[6] = SystemParameters.MCTST1 / SystemParameters.MCTSW1; //TBD

	PIBURN(sv1.R, sv1.V, sv1.GMT, B, sv2.R, sv2.V, sv2.GMT);
	sv2.RBI = sv1.RBI;
	vars->Vec1 = sv2.R;
	vars->Vec2 = sv2.V;
	vars->Words77_78 = sv2.GMT;
	vars->Vec3 = sv2.V - sv1.V;
	m_cut = B[8];
PCMATC_1A:
	if (sv2.RBI == BODY_MOON)
	{
		goto PCMATC_2B;
	}
	if (vars->ISTP == -1)
	{
		dt_ar = var[4]*3600.0 - sv2.GMT;
		goto PCMATC_3A;
	}
	coe = OrbMech::coe_from_sv(sv2.R, sv2.V, OrbMech::mu_Earth);
	double a = OrbMech::GetSemiMajorAxis(sv2.R, sv2.V, OrbMech::mu_Earth);
	double gamma_min;
	if (coe.e > 1.0)
	{
		gamma_min = -PI05;
	}
	else
	{
		gamma_min = atan2(-coe.e, sqrt(1.0 - coe.e * coe.e));
	}
	if (gamma_min > vars->gamma_stop)
	{
		goto PCMATC_4C;
	}
	gamma_p = atan2(coe.e*sin(coe.TA), 1.0 + coe.e*cos(coe.TA));
	if (coe.e >= 1.0 && gamma_p >= 0.0)
	{
		goto PCMATC_4C;
	}
	double E = OrbMech::TrueToEccentricAnomaly(coe.TA, coe.e);
	if (E >= 3.0*PI05)
	{
		if (gamma_p > vars->gamma_stop)
		{
			goto PCMATC_4C;
		}
		T_min = 0.0;
	}
	else
	{
		T_min = 0.0;
		if (coe.e < 1.0)
		{
			T_min = sqrt(pow(a, 3) / OrbMech::mu_Earth)*((3.0*PI05 - E) + coe.e*(1.0 + sin(E)));
		}
	}
	goto PCMATC_3A;
PCMATC_2B:
	//Is H_PC switched on as a dependant variable?
	if (vars->h_pc_on == true)
	{
		double dir;
		EphemerisData sv_pc;

		gamma_p = asin(dotp(unit(sv2.R), unit(sv2.V)));
		if (gamma_p >= 0.0)
		{
			dir = -1.0;
		}
		else
		{
			dir = 1.0;
		}

		//Propagate from abort to pericynthion
		PMMCEN(sv2, 0.0, 10.0*24.0*3600.0, 2, 0.0, dir, sv_pc, stop_ind);
		h_p = length(sv_pc.R) - BZLAND.rad[RTCC_LMPOS_BEST];
		//If abort state vector was pre-pericynthion, use updated state vector
		if (dir > 0.0)
		{
			sv2 = sv_pc;
		}
	}
	T_min = 0.0;
	dt_ar = var[4] * 3600.0 - sv2.GMT;
PCMATC_3A:
	if (vars->ISTP == -1)
	{
		PMMCEN(sv2, 0.0, 10.0*24.0*3600.0, 1, dt_ar, 1.0, sv_r, stop_ind);
	}
	else
	{
		PMMCEN(sv2, T_min, 10.0*24.0*3600.0, 2, vars->gamma_stop, 1.0, sv_r, stop_ind);
	}

	if (sv_r.RBI == BODY_MOON || (vars->ISTP == 1 && stop_ind != 2))
	{
		vars->ErrInd = 1;
		goto PCMATC_4C;
	}
//PCMATC_3B:
	double r_a, r_p;
	OrbMech::periapo(sv_r.R, sv_r.V, OrbMech::mu_Earth, r_a, r_p);
	if (r_a >= 0)
	{
		vars->h_a = r_a - OrbMech::R_Earth;
	}
	else
	{
		vars->h_a = 0.0;
	}
	vars->h_p = r_p - OrbMech::R_Earth;

	MATRIX3 Rot;
	ELVCNV(sv_r.GMT, 1, 0, Rot);

	VECTOR3 R_r_equ = tmul(Rot, sv_r.R);
	VECTOR3 V_r_equ = tmul(Rot, sv_r.V);
	double h_r = length(sv_r.R) - OrbMech::R_Earth;
	double r_r, v_r, lat_r, ra_r, gamma_r, azi_r, lng_r, lambda_G;
	PICSSC(true, R_r_equ, V_r_equ, r_r, v_r, lat_r, ra_r, gamma_r, azi_r);
	double T_ar = sv_r.GMT - t_a;

	lambda_G = PIBSHA(sv_r.GMT / 3600.0);
	lng_r = ra_r - lambda_G;
	OrbMech::normalizeAngle(lng_r);

	arr[0] = h_r / OrbMech::R_Earth;
	arr[1] = lat_r;
	arr[2] = lng_r;
	arr[3] = azi_r;
	arr[4] = T_ar / 3600.0;
	arr[5] = gamma_r;
	arr[6] = h_p / OrbMech::R_Earth;
	arr[7] = m_cut;

	vars->R_r = sv_r.R;
	vars->V_r = sv_r.V;
	vars->T_r = sv_r.GMT;
	return false;
PCMATC_4C:
	return true;
PCMATC_5A:
	PMMRKJInputArray integin;

	integin.A = 0.0;
	integin.DENSMULT = 1.0;
	integin.DOCKANG = vars->DockingAngle;
	integin.DPSScale = SystemParameters.MCTDTF;
	integin.DTOUT = 10.0;
	integin.DTPS10 = vars->DT_10PCT;
	integin.DTU = vars->dt_ullage;
	integin.HeadsUpDownInd = vars->HeadsUpDownInd;
	integin.IC = vars->ConfigCode;
	integin.KAUXOP = 1;
	integin.KEPHOP = 0;
	integin.KTRIMOP = vars->TrimAngleInd;
	std::bitset<4> cfg = vars->ConfigCode;
	if (cfg[RTCC_CONFIG_D] == true)
	{
		integin.LMDESCJETT = 1e70;
	}
	else
	{
		integin.LMDESCJETT = 0;
	}
	integin.MANOP = vars->AttitudeCode;

	integin.sv0 = sv1;
	integin.ThrusterCode = vars->ThrusterCode;
	integin.TVC = vars->TVC;
	integin.UllageOption = vars->UllageCode;
	integin.ExtDVCoordInd = false;
	integin.VG = _V(var[0], var[1], var[2])*OrbMech::R_Earth / 3600.0;

	integin.WDMULT = 1.0;
	integin.CAPWT = vars->Vec3.z + vars->LMWeight;
	integin.CSMWT = vars->Vec3.z;
	integin.LMAWT = 0.0;
	integin.LMDWT = vars->LMWeight;

	int Ierr;

	RTCCNIAuxOutputTable AuxTable;
	CSMLMPoweredFlightIntegration numin(this, integin, Ierr, NULL, &AuxTable);
	numin.PMMRKJ();

	if (Ierr)
	{
		vars->ErrInd = 2;
		goto PCMATC_4C;
	}

	sv2.R = AuxTable.R_BO;
	sv2.V = AuxTable.V_BO;
	sv2.GMT = AuxTable.GMT_BO;
	sv2.RBI = sv1.RBI;

	vars->Vec1 = AuxTable.A_T;
	vars->Vec2 = AuxTable.R_BI;
	vars->Vec3.x = AuxTable.DV;
	vars->Vec3.y = AuxTable.DV_TO;
	vars->DT_TO = AuxTable.DT_TO;
	vars->X_B = AuxTable.X_B;
	vars->Y_B = AuxTable.Y_B;
	vars->Z_B = AuxTable.Z_B;
	vars->R_Y = AuxTable.P_G;
	vars->R_Z = AuxTable.Y_G;
	vars->DVC = AuxTable.DV_C;
	vars->sv_CO.R = AuxTable.R_BO;
	vars->sv_CO.V = AuxTable.V_BO;
	vars->sv_CO.GMT = AuxTable.GMT_BO;
	vars->sv_CO.RBI = AuxTable.RBI;
	vars->sv_BI = AuxTable.sv_FF;
	vars->dv_total = AuxTable.DV;
	vars->dt_ME = AuxTable.DT_B;
	vars->W_end = AuxTable.WTEND;
	vars->W_on = AuxTable.WTENGON;
	vars->GMT_BI = AuxTable.GMT_BI;
	vars->V_BI = AuxTable.V_BI;
	goto PCMATC_1A;
}

void RTCC::FDOLaunchAnalog1(EphemerisData sv)
{
	fdolaunchanalog1tab.LastUpdateTime = oapiGetSimTime();

	double xval = 0.0;
	double yval = 0.0;

	if (sv.RBI != BODY_EARTH)
	{
		return;
	}

	double r_apo, r_peri;
	OrbMech::periapo(sv.R, sv.V, OrbMech::mu_Earth, r_apo, r_peri);

	if (r_peri > OrbMech::R_Earth + 50.0*1852.0 || length(sv.R) < OrbMech::R_Earth + 100.0)
	{
		return;
	}

	double v_fps = length(sv.V) / 0.3048;
	double gamma_deg = asin(dotp(unit(sv.R), unit(sv.V)))*DEG;

	if (v_fps < 5500.0)
	{
		if (v_fps < 0)
		{
			v_fps = 0.0;
		}
		xval = v_fps / 6000.0;
		
		if (gamma_deg < 0.0)
		{
			gamma_deg = 0.0;
		}
		else if (gamma_deg > 40.0)
		{
			gamma_deg = 40.0;
		}
		yval = gamma_deg / 40.0;
	}
	else if (v_fps < 22000.0)
	{
		xval = (v_fps - 5000.0) / (23000.0 - 5000.0);

		if (gamma_deg > 35.0)
		{
			gamma_deg = 35.0;
		}
		else if (gamma_deg < -5.0)
		{
			gamma_deg = -5.0;
		}
		yval = (gamma_deg + 5.0) / (35.0 + 5.0);
	}
	else if (v_fps < 27000.0)
	{
		xval = (v_fps - 21000.0) / (27000.0 - 21000.0);

		if (gamma_deg > 2.0)
		{
			gamma_deg = 2.0;
		}
		else if (gamma_deg < -2.0)
		{
			gamma_deg = -2.0;
		}
		yval = (gamma_deg + 2.0) / (2.0 + 2.0);
	}
	else
	{
		return;
	}

	yval = 1.0 - yval;

	double xscalmin = 0.15;
	double xscalmax = 0.95;
	double yscalmin = 0.15;
	double yscalmax = 0.85;

	xval = xval * (xscalmax - xscalmin) + xscalmin;
	yval = yval * (yscalmax - yscalmin) + yscalmin;

	fdolaunchanalog1tab.XVal.push_back(xval);
	fdolaunchanalog1tab.YVal.push_back(yval);
}

void RTCC::FDOLaunchAnalog2(EphemerisData sv)
{
	fdolaunchanalog2tab.LastUpdateTime = oapiGetSimTime();

	double r_EI = OrbMech::R_Earth + 400000.0*0.3048;
	double xval = 0.0;
	double yval = 0.0;

	if (sv.RBI != BODY_EARTH)
	{
		return;
	}

	double r_apo, r_peri;
	OrbMech::periapo(sv.R, sv.V, OrbMech::mu_Earth, r_apo, r_peri);

	if (r_peri > r_EI || r_apo < r_EI)
	{
		return;
	}

	double dt = OrbMech::time_radius(sv.R, sv.V, r_EI, -1.0, OrbMech::mu_Earth);
	VECTOR3 R_EI, V_EI;
	OrbMech::rv_from_r0v0(sv.R, sv.V, dt, R_EI, V_EI, OrbMech::mu_Earth);

	double v_fps = length(V_EI) / 0.3048;
	double gamma_deg = asin(dotp(unit(R_EI), unit(V_EI)))*DEG;

	if (v_fps < 22750.0)
	{
		if (v_fps < 6000.0)
		{
			v_fps = 6000.0;
		}
		xval = (v_fps - 6000.0) / (23000.0 - 6000.0);

		if (gamma_deg > 0.0)
		{
			gamma_deg = 0.0;
		}
		else if (gamma_deg < -36.0)
		{
			gamma_deg = -36.0;
		}
		yval = -gamma_deg / 36.0;
	}
	else if (v_fps < 26250.0)
	{
		xval = (v_fps - 22500.0) / (26750.0 - 22500.0);

		if (gamma_deg > 0.0)
		{
			gamma_deg = 0.0;
		}
		else if (gamma_deg < -12.0)
		{
			gamma_deg = -12.0;
		}
		yval = -gamma_deg / 12.0;
	}
	else if (v_fps < 33500.0)
	{
		xval = (v_fps - 25000.0) / (33500.0 - 25000.0);

		if (gamma_deg > 0.0)
		{
			gamma_deg = 0.0;
		}
		else if (gamma_deg < -12.0)
		{
			gamma_deg = -12.0;
		}
		yval = -gamma_deg / 12.0;
	}
	else
	{
		return;
	}

	double xscalmin = 0.15;
	double xscalmax = 0.95;
	double yscalmin = 0.15;
	double yscalmax = 0.85;

	xval = xval * (xscalmax - xscalmin) + xscalmin;
	yval = yval * (yscalmax - yscalmin) + yscalmin;

	fdolaunchanalog2tab.XVal.push_back(xval);
	fdolaunchanalog2tab.YVal.push_back(yval);
}

void RTCC::PMDTRDFF(int med, unsigned page)
{
	//To figure out how to format the display, search for the largest DV and the maximum time difference

	unsigned i, j, p;
	double maxdv = 0;
	double MinTime = (RTETradeoffTable.data[0][0].T0 - SystemParameters.MCGMTL);
	double MaxTime = 0;

	for (i = 0;i < RTETradeoffTable.curves;i++)
	{
		for (j = 0;j < RTETradeoffTable.NumInCurve[i];j++)
		{
			if (RTETradeoffTable.data[i][j].DV > maxdv)
			{
				maxdv = RTETradeoffTable.data[i][j].DV;
			}
			if ((RTETradeoffTable.data[i][j].T0 - SystemParameters.MCGMTL) > MaxTime)
			{
				MaxTime = (RTETradeoffTable.data[i][j].T0 - SystemParameters.MCGMTL);
			}
		}
	}

	//Origin of x-axis should be at least below the minimum time
	double MinTimeLabel = floor(MinTime);
	//If the minimum time was very close to the previous hour, go back even one more hour
	if (MinTime - MinTimeLabel < 0.1)
	{
		MinTimeLabel -= 1.0;
	}
	//End of x-axis should be at least above the maximum time
	double MaxTimeLabel = ceil(MaxTime);
	//Difference between minimum and maximum time
	int DeltaTimeLabel = (int)(MaxTimeLabel - MinTimeLabel);
	//Enforce even number of hours on scale, so that the middle value is also a full hour
	if (DeltaTimeLabel % 2 != 0)
	{
		MaxTimeLabel += 1.0;
	}
	//If the maximum time was very close to the next hour, go up even one more hour
	else if (MaxTimeLabel - MaxTime < 0.1)
	{
		MaxTimeLabel += 1.0;
	}

	double MidTimeLabel = (MaxTimeLabel + MinTimeLabel) / 2.0;

	double MinDVLabel = 0;
	double MaxDVLabel = ceil(maxdv / 1000.0)*1000.0;
	int DeltaDVLabel = (int)((MaxDVLabel - MinDVLabel) / 1000.0);
	if (DeltaDVLabel % 2 != 0)
	{
		MaxDVLabel += 1000.0;
	}

	double MidDVLabel = (MaxDVLabel + MinDVLabel) / 2.0;

	double xscalmin = 0.1;
	double xscalmax = 0.95;
	double yscalmin = 0.1;
	double yscalmax = 0.85;
	double xval, yval;

	if (med == 70)
	{
		p = 0;
	}
	else
	{
		p = page - 1;
	}

	RTETradeoffTableBuffer[p].curves = RTETradeoffTable.curves;
	RTETradeoffTableBuffer[p].XLabels[0] = (int)MinTimeLabel;
	RTETradeoffTableBuffer[p].XLabels[1] = (int)MidTimeLabel;
	RTETradeoffTableBuffer[p].XLabels[2] = (int)MaxTimeLabel;
	RTETradeoffTableBuffer[p].YLabels[0] = (int)MinDVLabel / 1000;
	RTETradeoffTableBuffer[p].YLabels[1] = (int)MidDVLabel / 1000;
	RTETradeoffTableBuffer[p].YLabels[2] = (int)MaxDVLabel / 1000;

	//Set to -1, will not display the unused curves
	for (i = 0;i < 10;i++)
	{
		RTETradeoffTableBuffer[p].TZDisplay[i] = -1;
	}
	for (i = 0;i < RTETradeoffTable.curves;i++)
	{
		RTETradeoffTableBuffer[p].NumInCurve[i] = RTETradeoffTable.NumInCurve[i];

		//Rounded average of landing time
		RTETradeoffTableBuffer[p].TZDisplay[i] = (int)round(((RTETradeoffTable.data[i][0].T_Z - SystemParameters.MCGMTL) + (RTETradeoffTable.data[i][RTETradeoffTable.NumInCurve[i] - 1].T_Z - SystemParameters.MCGMTL)) / 2.0);

		for (j = 0;j < RTETradeoffTable.NumInCurve[i];j++)
		{
			xval = ((RTETradeoffTable.data[i][j].T0 - SystemParameters.MCGMTL) - MinTimeLabel) / (MaxTimeLabel - MinTimeLabel);
			yval = (RTETradeoffTable.data[i][j].DV - MinDVLabel) / (MaxDVLabel - MinDVLabel);

			xval = xval * (xscalmax - xscalmin) + xscalmin;
			yval = yval * (yscalmax - yscalmin) + yscalmin;
			yval = 1.0 - yval;

			RTETradeoffTableBuffer[p].xval[i][j] = xval;
			RTETradeoffTableBuffer[p].yval[i][j] = yval;
		}

		RTETradeoffTableBuffer[p].TZxval[i] = min(xscalmax, xval + 0.01);
		RTETradeoffTableBuffer[p].TZyval[i] = yval;
	}

	RTETradeoffTableBuffer[p].XAxisName = "Time of abort, hr";
	RTETradeoffTableBuffer[p].YAxisName = "Abort velocity, kfps";
	RTETradeoffTableBuffer[p].Site = PZREAP.RTESite;

	//If remote Earth tradeoff then we are done
	if (med == 71)
	{
		return;
	}

	//Now latitude and time of landing for near-Earth tradeoff

	//Unused for time of landing, same for latitude
	for (i = 0;i < 10;i++)
	{
		RTETradeoffTableBuffer[1].TZDisplay[i] = -1;
		RTETradeoffTableBuffer[2].TZDisplay[i] = RTETradeoffTableBuffer[0].TZDisplay[i];
	}

	for (i = 0;i < RTETradeoffTable.curves;i++)
	{
		for (j = 0;j < RTETradeoffTable.NumInCurve[i];j++)
		{
			//X-values are the same
			RTETradeoffTableBuffer[2].xval[i][j] = RTETradeoffTableBuffer[1].xval[i][j] = RTETradeoffTableBuffer[0].xval[i][j];
		}
	}

	double mintz = 10e10;
	double maxtz = 0;
	double minlat = 10e10;
	double maxlat = 0;

	//Find min and max values
	for (i = 0;i < RTETradeoffTable.curves;i++)
	{
		for (j = 0;j < RTETradeoffTable.NumInCurve[i];j++)
		{
			if ((RTETradeoffTable.data[i][j].T_Z - SystemParameters.MCGMTL) > maxtz)
			{
				maxtz = (RTETradeoffTable.data[i][j].T_Z - SystemParameters.MCGMTL);
			}
			if ((RTETradeoffTable.data[i][j].T_Z - SystemParameters.MCGMTL) < mintz)
			{
				mintz = (RTETradeoffTable.data[i][j].T_Z - SystemParameters.MCGMTL);
			}

			if (RTETradeoffTable.data[i][j].lat > maxlat)
			{
				maxlat = RTETradeoffTable.data[i][j].lat;
			}
			if (RTETradeoffTable.data[i][j].lat < minlat)
			{
				minlat = RTETradeoffTable.data[i][j].lat;
			}
		}
	}

	double MinTZLabel = floor(mintz);
	if (mintz - MinTZLabel < 0.1)
	{
		MinTZLabel -= 1.0;
	}
	double MaxTZLabel = ceil(maxtz);
	if (MaxTZLabel - maxtz < 0.1)
	{
		MaxTZLabel += 1.0;
	}

	int DeltaTZLabel = (int)(MaxTZLabel - MinTZLabel);
	if (DeltaTZLabel % 2 != 0)
	{
		MaxTZLabel += 1.0;
	}

	double MidTZLabel = (MaxTZLabel + MinTZLabel) / 2.0;

	RTETradeoffTableBuffer[1].curves = RTETradeoffTable.curves;
	RTETradeoffTableBuffer[1].XLabels[0] = (int)MinTimeLabel;
	RTETradeoffTableBuffer[1].XLabels[1] = (int)MidTimeLabel;
	RTETradeoffTableBuffer[1].XLabels[2] = (int)MaxTimeLabel;
	RTETradeoffTableBuffer[1].YLabels[0] = (int)MinTZLabel;
	RTETradeoffTableBuffer[1].YLabels[1] = (int)MidTZLabel;
	RTETradeoffTableBuffer[1].YLabels[2] = (int)MaxTZLabel;

	double MinlatLabel = floor(minlat);
	if (minlat - MinlatLabel < 0.1)
	{
		MinlatLabel -= 1.0;
	}
	double MaxlatLabel = ceil(maxlat);
	if (MaxlatLabel - maxlat < 0.1)
	{
		MaxlatLabel += 1.0;
	}

	int DeltalatLabel = (int)(MaxlatLabel - MinlatLabel);
	if (DeltalatLabel % 2 != 0)
	{
		MaxlatLabel += 1.0;
	}

	double MidlatLabel = (MaxlatLabel + MinlatLabel) / 2.0;

	RTETradeoffTableBuffer[2].curves = RTETradeoffTable.curves;
	RTETradeoffTableBuffer[2].XLabels[0] = (int)MinTimeLabel;
	RTETradeoffTableBuffer[2].XLabels[1] = (int)MidTimeLabel;
	RTETradeoffTableBuffer[2].XLabels[2] = (int)MaxTimeLabel;
	RTETradeoffTableBuffer[2].YLabels[0] = (int)MinlatLabel;
	RTETradeoffTableBuffer[2].YLabels[1] = (int)MidlatLabel;
	RTETradeoffTableBuffer[2].YLabels[2] = (int)MaxlatLabel;

	for (i = 0;i < RTETradeoffTable.curves;i++)
	{
		RTETradeoffTableBuffer[1].NumInCurve[i] = RTETradeoffTable.NumInCurve[i];
		RTETradeoffTableBuffer[2].NumInCurve[i] = RTETradeoffTable.NumInCurve[i];

		for (j = 0;j < RTETradeoffTable.NumInCurve[i];j++)
		{
			//Landing time
			yval = ((RTETradeoffTable.data[i][j].T_Z - SystemParameters.MCGMTL) - MinTZLabel) / (MaxTZLabel - MinTZLabel);

			yval = yval * (yscalmax - yscalmin) + yscalmin;
			yval = 1.0 - yval;

			RTETradeoffTableBuffer[1].yval[i][j] = yval;

			//Latitude
			yval = (RTETradeoffTable.data[i][j].lat - MinlatLabel) / (MaxlatLabel - MinlatLabel);

			yval = yval * (yscalmax - yscalmin) + yscalmin;
			yval = 1.0 - yval;

			RTETradeoffTableBuffer[2].yval[i][j] = yval;
		}

		RTETradeoffTableBuffer[2].TZxval[i] = RTETradeoffTableBuffer[0].TZxval[i];
		RTETradeoffTableBuffer[2].TZyval[i] = yval;
	}

	RTETradeoffTableBuffer[1].XAxisName = "Time of abort, hr";
	RTETradeoffTableBuffer[1].YAxisName = "Time of landing, hr";
	RTETradeoffTableBuffer[1].Site = PZREAP.RTESite;

	RTETradeoffTableBuffer[2].XAxisName = "Time of abort, hr";
	RTETradeoffTableBuffer[2].YAxisName = "Latitude of landing, deg";
	RTETradeoffTableBuffer[2].Site = PZREAP.RTESite;
}

//Mission Plan Table Display
void RTCC::PMDMPT()
{
	MPTDISPLAY.man.clear();

	char Buffer[100];
	double temp;
	unsigned csmnum = PZMPTCSM.mantable.size();
	unsigned lemnum = PZMPTLEM.mantable.size();
	unsigned i = 0, j = 0;

	MPTDISPLAY.CSMSTAID = PZMPTCSM.StationID;
	MPTDISPLAY.LEMSTAID = PZMPTLEM.StationID;

	if (PZMPTCSM.GMTAV == 0.0)
	{
		MPTDISPLAY.CSMGETAV = "";
	}
	else
	{
		temp = GETfromGMT(PZMPTCSM.GMTAV);
		OrbMech::format_time_HHMMSS(Buffer, temp);
		std::string strtemp(Buffer);
		MPTDISPLAY.CSMGETAV = strtemp;
	}

	if (PZMPTLEM.GMTAV == 0.0)
	{
		MPTDISPLAY.LEMGETAV = "";
	}
	else
	{
		temp = GETfromGMT(PZMPTLEM.GMTAV);
		OrbMech::format_time_HHMMSS(Buffer, temp);
		std::string strtemp(Buffer);
		MPTDISPLAY.LEMGETAV = strtemp;
	}

	if (csmnum == 0 && lemnum == 0) return;

	MPTManDisplay man;
	MPTManeuver *mptman;
	double mu, r, dt;
	bool isManeuverExecuted;

	while (i < csmnum || j < lemnum)
	{
		if (j >= lemnum || (i < csmnum && PZMPTCSM.mantable[i].GMT_BI < PZMPTLEM.mantable[j].GMT_BI))
		{
			mptman = &PZMPTCSM.mantable[i];
			isManeuverExecuted = (i + 1 <= PZMPTCSM.LastExecutedManeuver);
			i++;
		}
		else
		{
			mptman = &PZMPTLEM.mantable[j];
			isManeuverExecuted = (j + 1 <= PZMPTLEM.LastExecutedManeuver);
			j++;
		}

		man.DELTAV = mptman->DV_M / 0.3048;

		if (mptman->RefBodyInd == BODY_EARTH)
		{
			mu = OrbMech::mu_Earth;
			r = OrbMech::R_Earth;
		}
		else
		{
			mu = OrbMech::mu_Moon;
			r = BZLAND.rad[RTCC_LMPOS_BEST];
		}

		man.HA = mptman->h_a / 1852.0;
		if (man.HA > 9999.9)
		{
			man.HA = 9999.9;
		}
		man.HP = mptman->h_p / 1852.0;
		if (man.HP > 9999.9)
		{
			man.HP = 9999.9;
		}

		if (MPTDISPLAY.man.size() > 0)
		{
			dt = mptman->GMT_BI - temp;
			OrbMech::format_time_HHMMSS(Buffer, round(dt));
			std::string strtemp3(Buffer);
			man.DT = strtemp3;
		}

		temp = mptman->GMT_BO;

		OrbMech::format_time_HHMMSS(Buffer, round(GETfromGMT(mptman->GMT_BI)));
		std::string strtemp4(Buffer);
		man.GETBI = strtemp4;
		//Maneuver executed?
		if (isManeuverExecuted)
		{
			man.code = "E" + mptman->code;
		}
		//Maneuver frozen?
		else if (mptman->FrozenManeuverInd)
		{
			man.code = "F" + mptman->code;
		}
		else
		{
			man.code = mptman->code;
		}
		man.DVREM = mptman->DVREM / 0.3048;

		MPTDISPLAY.man.push_back(man);
	}
}

void RTCC::PMDTIMP()
{
	TwoImpMultDispBuffer = TwoImpulseMultipleSolutionDisplay();

	if (PZTIPREG.Solutions == 0)
	{
		if (PZTIPREG.Updating)
		{
			TwoImpMultDispBuffer.ErrorMessage = "TABLE BEING UPDATED";
			return;
		}
		else
		{
			TwoImpMultDispBuffer.ErrorMessage = "NO TWO IMPULSE PLANS AVAILABLE";
			return;
		}
	}

	TwoImpMultDispBuffer.CSMSTAID = PZTIPREG.CSMSTAID;
	TwoImpMultDispBuffer.LMSTAID = PZTIPREG.LMSTAID;
	if (PZTIPREG.MAN_VEH == 1)
	{
		TwoImpMultDispBuffer.MAN_VEH = "CSM";
	}
	else
	{
		TwoImpMultDispBuffer.MAN_VEH = "LM";
	}

	if (PZTIPREG.IVFLAG == 2)
	{
		TwoImpMultDispBuffer.GETFRZ = "2";
		TwoImpMultDispBuffer.GMTFRZ = "2";
		TwoImpMultDispBuffer.GETVAR = "1";
		TwoImpMultDispBuffer.GET1 = GETfromGMT(PZTIPREG.data[0].Time2);
		TwoImpMultDispBuffer.GMT1 = PZTIPREG.data[0].Time2;
	}
	else
	{
		TwoImpMultDispBuffer.GETFRZ = "1";
		TwoImpMultDispBuffer.GMTFRZ = "1";
		TwoImpMultDispBuffer.GETVAR = "2";
		TwoImpMultDispBuffer.GET1 = GETfromGMT(PZTIPREG.data[0].Time1);
		TwoImpMultDispBuffer.GMT1 = PZTIPREG.data[0].Time1;
	}

	if (PZTIPREG.IVFLAG == 0)
	{
		TwoImpMultDispBuffer.OPTION = "BOTH FIXED";
	}
	else if (PZTIPREG.IVFLAG == 1)
	{
		TwoImpMultDispBuffer.OPTION = "FIRST FIXED";
	}
	else
	{
		TwoImpMultDispBuffer.OPTION = "SECOND FIXED";
	}

	TwoImpMultDispBuffer.WT = GZGENCSN.TITravelAngle*DEG;
	TwoImpMultDispBuffer.PHASE = GZGENCSN.TIPhaseAngle*DEG;
	TwoImpMultDispBuffer.DH = GZGENCSN.TIDeltaH / 1852.0;

	TwoImpMultDispBuffer.Solutions = PZTIPREG.Solutions;
	TwoImpMultDispBuffer.showTPI = PZTIPREG.showTPI;
	for (int i = 0;i < PZTIPREG.Solutions;i++)
	{
		TwoImpMultDispBuffer.data[i].DELV1 = PZTIPREG.data[i].DELV1 / 0.3048;
		TwoImpMultDispBuffer.data[i].YAW1 = PZTIPREG.data[i].YAW1*DEG;
		TwoImpMultDispBuffer.data[i].PITCH1 = PZTIPREG.data[i].PITCH1*DEG;
		if (PZTIPREG.IVFLAG == 2)
		{
			TwoImpMultDispBuffer.data[i].Time2 = GETfromGMT(PZTIPREG.data[i].Time1);
		}
		else
		{
			TwoImpMultDispBuffer.data[i].Time2 = GETfromGMT(PZTIPREG.data[i].Time2);
		}
		TwoImpMultDispBuffer.data[i].DELV2 = PZTIPREG.data[i].DELV2 / 0.3048;
		TwoImpMultDispBuffer.data[i].YAW2 = PZTIPREG.data[i].YAW2*DEG;
		TwoImpMultDispBuffer.data[i].PITCH2 = PZTIPREG.data[i].PITCH2*DEG;
		if (TwoImpMultDispBuffer.showTPI)
		{
			TwoImpMultDispBuffer.data[i].T_TPI = GETfromGMT(PZTIPREG.data[i].T_TPI);
		}
		TwoImpMultDispBuffer.data[i].L = 'N';
		TwoImpMultDispBuffer.data[i].C = i + 1;
	}
}

void RTCC::PMDRET()
{
	PZREDT = RendezvousEvaluationDisplay();

	bool solns = false;
	for (int i = 0;i < 7;i++)
	{
		if (PZDKIT.Block[i].PlanStatus != 0)
		{
			solns = true;
			break;
		}
	}

	if (solns == false)
	{
		//"No Plans" message
		PZREDT.ErrorMessage = "No Plans";
		return;
	}

	int plan = EZETVMED.RETPlan;

	if (PZDKIT.Block[plan - 1].PlanStatus <= 0)
	{
		// "No Plans" message
		PZREDT.ErrorMessage = "No Plans";
		return;
	}
	DKIDataBlock *block = &PZDKIT.Block[plan - 1];
	PZREDT.ID = plan;
	if (block->PlanStatus == 1)
	{
		PZREDT.isDKI = true;
		PZREDT.M = block->Plan_M;
	}
	else
	{
		PZREDT.isDKI = false;
	}

	PZREDT.NumMans = block->NumMan;
	for (int i = 0;i < PZREDT.NumMans;i++)
	{
		PZREDT.GET[i] = GETfromGMT(block->Display[i].ManGMT);
		if (i > 0)
		{
			PZREDT.DT[i] = block->Display[i].ManGMT - block->Display[i-1].ManGMT;
		}
		PZREDT.DV[i] = block->Display[i].dv / 0.3048;
		if (block->Display[i].VEH == 1)
		{
			PZREDT.VEH[i] = "CSM";
		}
		else
		{
			PZREDT.VEH[i] = "LEM";
		}
		PZREDT.PURP[i] = block->Display[i].Man_ID;
		PZREDT.CODE[i] = 0.0;
		PZREDT.PHASE[i] = block->Display[i].PhaseAngle*DEG;
		PZREDT.HEIGHT[i] = block->Display[i].DH / 1852.0;
		PZREDT.HA[i] = block->Display[i].HA / 1852.0;
		PZREDT.HP[i] = block->Display[i].HP / 1852.0;
		PZREDT.Pitch[i] = block->Display[i].Pitch*DEG;
		PZREDT.Yaw[i] = block->Display[i].Yaw*DEG;
		PZREDT.DVVector[i] = block->Display[i].DV_LVLH / 0.3048;
	}
}

void RTCC::PMDRPT()
{
	PZRPDT = RendezvousPlanningDisplay();

	if (PZDKIT.Block[0].PlanStatus != 1)
	{
		PZRPDT.ErrorMessage = "No Plans";
		return;
	}
	if (PZDKIT.UpdatingIndicator)
	{
		PZRPDT.ErrorMessage = "Table being updated";
		return;
	}

	PZRPDT.plans = PZDKIT.NumSolutions;
	for (int i = 0;i < PZDKIT.NumSolutions;i++)
	{
		PZRPDT.data[i].ID = i + 1;
		PZRPDT.data[i].M = PZDKIT.Block[i].Plan_M;
		PZRPDT.data[i].NC1 = PZDKIT.Block[i].NC1;
		PZRPDT.data[i].NH = PZDKIT.Block[i].NH;
		PZRPDT.data[i].NSR = PZDKIT.Block[i].NSR;
		PZRPDT.data[i].NPC = PZDKIT.Block[i].NPC;
		PZRPDT.data[i].NCC = PZDKIT.Block[i].NCC;
		PZRPDT.data[i].GETTPI = GETfromGMT(PZDKIT.Block[i].TTPI);
	}
}

int RTCC::ThrusterNameToCode(std::string thruster)
{
	if (thruster == "C1") return 1;
	else if (thruster == "C2") return 2;
	else if (thruster == "C3") return 3;
	else if (thruster == "C4") return 4;
	else if (thruster == "BV") return 16;
	else if (thruster == "L1") return 17;
	else if (thruster == "L2") return 18;
	else if (thruster == "L3") return 19;
	else if (thruster == "L4") return 20;
	else if (thruster == "S") return 33;
	else if (thruster == "A") return 34;
	else if (thruster == "D") return 35;
	else if (thruster == "J") return 36;
	else if (thruster == "AU") return 37;

	return 0;
}

int RTCC::AttitudeNameToCode(std::string attitude)
{
	if (attitude == "I") return 1;
	else if (attitude == "M") return 2;
	else if (attitude == "L") return 3;
	else if (attitude == "EP") return 4;
	else if (attitude == "EA") return 5;
	else if (attitude == "AP") return 6;
	else if (attitude == "AA") return 7;

	return 0;
}

bool RTCC::RTEManeuverCodeLogic(char *code, double lmascmass, double lmdscmass, int UllageNum, int &thruster, int &AttMode, int &ConfigCode, int &ManVeh, double &lmmass)
{
	AttMode = RTCC_ATTITUDE_PGNS_EXDV;

	if (code[0] == 'C')
	{
		ManVeh = RTCC_MANVEHICLE_CSM;
		if (code[1] == 'S')
		{
			thruster = RTCC_ENGINETYPE_CSMSPS;
		}
		else if (code[1] == 'R')
		{
			if (UllageNum == 4)
			{
				thruster = RTCC_ENGINETYPE_CSMRCSPLUS4;
			}
			else if (UllageNum == 2)
			{
				thruster = RTCC_ENGINETYPE_CSMRCSPLUS2;
			}
			else if (UllageNum == -4)
			{
				thruster = RTCC_ENGINETYPE_CSMRCSMINUS4;
			}
			else if (UllageNum == -2)
			{
				thruster = RTCC_ENGINETYPE_CSMRCSMINUS2;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
		if (code[2] == 'D')
		{
			lmmass = lmascmass + lmdscmass;
			ConfigCode = 1 + 4 + 8;
		}
		else if (code[2] == 'A')
		{
			lmmass = lmascmass;
			ConfigCode = 1 + 4;
		}
		else if (code[2] == 'U')
		{
			lmmass = 0.0;
			ConfigCode = 1;
		}
		else
		{
			return true;
		}
	}
	else if (code[0] == 'L')
	{
		ManVeh = RTCC_MANVEHICLE_LM;
		if (code[1] == 'D')
		{
			thruster = RTCC_ENGINETYPE_LMDPS;
			if (code[2] == 'D')
			{
				lmmass = lmascmass + lmdscmass;
				ConfigCode = 1 + 4 + 8;
			}
			else
			{
				return true;
			}
		}
		else if (code[1] == 'R')
		{
			if (UllageNum == 4)
			{
				thruster = RTCC_ENGINETYPE_LMRCSPLUS4;
			}
			else if (UllageNum == 2)
			{
				thruster = RTCC_ENGINETYPE_LMRCSPLUS2;
			}
			else if (UllageNum == -4)
			{
				thruster = RTCC_ENGINETYPE_LMRCSMINUS4;
			}
			else if (UllageNum == -2)
			{
				thruster = RTCC_ENGINETYPE_LMRCSMINUS2;
			}
			else
			{
				return true;
			}
			if (code[2] == 'D')
			{
				lmmass = lmascmass + lmdscmass;
				ConfigCode = 1 + 4 + 8;
			}
			else if (code[2] == 'A')
			{
				lmmass = lmascmass;
				ConfigCode = 1 + 4;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}

	return false;
}

void RTCC::PMMDMT(int L, unsigned man, RTCCNIAuxOutputTable *aux)
{
	MissionPlanTable *mpt = GetMPTPointer(L);
	MPTManeuver *mptman;
	EphemerisData sv_FF;
	VECTOR3 V_G;
	
	if (mpt->mantable.size() < man)
	{
		//Error
		return;
	}
	mptman = &mpt->mantable[man - 1];

	mpt->TimeToEndManeuver[man - 1] = aux->GMT_BO;

	mptman->A_T = aux->A_T;
	mptman->R_BI = aux->R_BI;
	mptman->V_BI = aux->V_BI;
	mptman->GMT_BI = aux->GMT_BI;
	mptman->R_BO = aux->R_BO;
	mptman->V_BO = aux->V_BO;
	mptman->GMT_BO = aux->GMT_BO;
	mptman->R_1 = aux->R_1;
	mptman->V_1 = aux->V_1;
	mptman->GMT_1 = aux->GMT_1;
	mptman->CoordSysInd = aux->CSI;
	mptman->RefBodyInd = aux->RBI;
	mptman->DV_M = aux->DV;
	mptman->dt_BD = aux->DT_B;
	mptman->P_G = aux->P_G;
	mptman->Y_G = aux->Y_G;
	mptman->X_B = aux->X_B;
	mptman->Y_B = aux->Y_B;
	mptman->Z_B = aux->Z_B;
	mptman->dt_TO = aux->DT_TO;
	mptman->dv_TO = aux->DV_TO;
	mptman->MainEngineFuelUsed = aux->MainFuelUsed;
	mptman->RCSFuelUsed = aux->RCSFuelUsed;
	mptman->lng_AN = 0.0;
	//mptman->GMTFrozen = mpt->GMTAV;
	//mptman->StationIDFrozen = mpt->StationID;
	mptman->CommonBlock.TUP = abs(mpt->CommonBlock.TUP);

	double W_S_Prior, S_Fuel, WDOT, T, F;

	MPTVehicleDataBlock *CommonBlockBefore;

	if (man == 1)
	{
		CommonBlockBefore = &mpt->CommonBlock;
	}
	else
	{
		CommonBlockBefore = &mpt->mantable[man - 2].CommonBlock;
	}

	W_S_Prior = CommonBlockBefore->SIVBMass;
	S_Fuel = CommonBlockBefore->SIVBFuelRemaining;

	//S-IVB in configuration at init?
	if (mptman->ConfigCodeBefore[RTCC_CONFIG_S])
	{
		//Weight loss due to continuous venting
		double DW = W_S_Prior - aux->W_SIVB;
		//Subtract from S-IVB fuel remaining before maneuver
		S_Fuel = S_Fuel - DW;
		//S-IVB maneuver?
		if (mptman->TVC == 2)
		{
			mptman->CommonBlock.SIVBFuelRemaining = S_Fuel - aux->MainFuelUsed;

			T = SystemParameters.MCTSAV;
			WDOT = SystemParameters.MCTWAV;
			F = mptman->CommonBlock.SIVBFuelRemaining;

			mptman->CommonBlock.SPSFuelRemaining = CommonBlockBefore->SPSFuelRemaining;
			mptman->CommonBlock.CSMRCSFuelRemaining = CommonBlockBefore->CSMRCSFuelRemaining;
			mptman->CommonBlock.LMRCSFuelRemaining = CommonBlockBefore->LMRCSFuelRemaining;
			mptman->CommonBlock.LMAPSFuelRemaining = CommonBlockBefore->LMAPSFuelRemaining;
			mptman->CommonBlock.LMDPSFuelRemaining = CommonBlockBefore->LMDPSFuelRemaining;
		}
	}

	if (mptman->TVC != 2)
	{
		if (mptman->TVC == 1)
		{
			mptman->CommonBlock.SPSFuelRemaining = CommonBlockBefore->SPSFuelRemaining - aux->MainFuelUsed;
			mptman->CommonBlock.CSMRCSFuelRemaining = CommonBlockBefore->CSMRCSFuelRemaining - aux->RCSFuelUsed;
			mptman->CommonBlock.LMRCSFuelRemaining = CommonBlockBefore->LMRCSFuelRemaining;
			mptman->CommonBlock.LMAPSFuelRemaining = CommonBlockBefore->LMAPSFuelRemaining;
			mptman->CommonBlock.LMDPSFuelRemaining = CommonBlockBefore->LMDPSFuelRemaining;
		}
		else
		{
			mptman->CommonBlock.SPSFuelRemaining = CommonBlockBefore->SPSFuelRemaining;
			mptman->CommonBlock.CSMRCSFuelRemaining = CommonBlockBefore->CSMRCSFuelRemaining;
			mptman->CommonBlock.LMRCSFuelRemaining = CommonBlockBefore->LMRCSFuelRemaining - aux->RCSFuelUsed;
			if (mptman->Thruster == RTCC_ENGINETYPE_LMAPS)
			{
				mptman->CommonBlock.LMAPSFuelRemaining = CommonBlockBefore->LMAPSFuelRemaining - aux->MainFuelUsed;
				mptman->CommonBlock.LMDPSFuelRemaining = 0.0;
			}
			else if (mptman->Thruster == RTCC_ENGINETYPE_LMDPS)
			{
				mptman->CommonBlock.LMDPSFuelRemaining = CommonBlockBefore->LMDPSFuelRemaining - aux->MainFuelUsed;
				mptman->CommonBlock.LMAPSFuelRemaining = CommonBlockBefore->LMAPSFuelRemaining;
			}
			else
			{
				mptman->CommonBlock.LMDPSFuelRemaining = CommonBlockBefore->LMDPSFuelRemaining;
				mptman->CommonBlock.LMAPSFuelRemaining = CommonBlockBefore->LMAPSFuelRemaining;
			}
		}

		double OnboardThrust;
		EngineParametersTable(mptman->Thruster, T, WDOT, OnboardThrust);

		if (mptman->Thruster == RTCC_ENGINETYPE_CSMRCSPLUS2 || mptman->Thruster == RTCC_ENGINETYPE_CSMRCSMINUS2)
		{
			T = SystemParameters.MCTCT1;
			WDOT = SystemParameters.MCTCW1;
			F = mptman->CommonBlock.CSMRCSFuelRemaining;
		}
		else if (mptman->Thruster == RTCC_ENGINETYPE_CSMRCSPLUS4 || mptman->Thruster == RTCC_ENGINETYPE_CSMRCSMINUS4)
		{
			T = 2.0*SystemParameters.MCTCT1;
			WDOT = 2.0*SystemParameters.MCTCW1;
			F = mptman->CommonBlock.CSMRCSFuelRemaining;
		}
		else if (mptman->Thruster == RTCC_ENGINETYPE_LMRCSPLUS4 || mptman->Thruster == RTCC_ENGINETYPE_LMRCSMINUS4)
		{
			T = SystemParameters.MCTLT1;
			WDOT = SystemParameters.MCTLW1;
			F = mptman->CommonBlock.LMRCSFuelRemaining;
		}
		else if (mptman->Thruster == RTCC_ENGINETYPE_LMRCSPLUS4 || mptman->Thruster == RTCC_ENGINETYPE_LMRCSMINUS4)
		{
			T = 2.0*SystemParameters.MCTLT1;
			WDOT = 2.0*SystemParameters.MCTLW1;
			F = mptman->CommonBlock.LMRCSFuelRemaining;
		}
		else if (mptman->Thruster == RTCC_ENGINETYPE_CSMSPS)
		{
			T = SystemParameters.MCTST1;
			WDOT = SystemParameters.MCTSW1;
			F = mptman->CommonBlock.SPSFuelRemaining;
		}
		else if (mptman->Thruster == RTCC_ENGINETYPE_LMAPS)
		{
			T = SystemParameters.MCTAT1;
			WDOT = SystemParameters.MCTAW1;
			F = mptman->CommonBlock.LMAPSFuelRemaining;
		}
		else
		{
			T = SystemParameters.MCTDT1;
			WDOT = SystemParameters.MCTDW1;
			F = mptman->CommonBlock.LMDPSFuelRemaining;
		}
	}

	if (mptman->CommonBlock.ConfigChangeInd == RTCC_CONFIGCHANGE_DOCKING)
	{
		//TBD: Docking maneuver
	}

	//Mass Maintenance
	mptman->TotalMassAfter = 0.0;
	mptman->CommonBlock.CSMMass = 0.0;
	mptman->CommonBlock.LMAscentMass = 0.0;
	mptman->CommonBlock.LMDescentMass = 0.0;
	mptman->CommonBlock.SIVBMass = 0.0;
	mptman->TotalAreaAfter = 0.0;
	mptman->CommonBlock.CSMArea = 0.0;
	mptman->CommonBlock.LMAscentArea = 0.0;
	mptman->CommonBlock.LMDescentArea = 0.0;
	mptman->CommonBlock.SIVBArea = 0.0;
	if (mptman->CommonBlock.ConfigCode[RTCC_CONFIG_C])
	{
		//Was it an CSM engine?
		if (mptman->Thruster == RTCC_ENGINETYPE_CSMSPS || mptman->Thruster == RTCC_ENGINETYPE_CSMRCSMINUS2 || mptman->Thruster == RTCC_ENGINETYPE_CSMRCSMINUS4 || mptman->Thruster == RTCC_ENGINETYPE_CSMRCSPLUS2 || mptman->Thruster == RTCC_ENGINETYPE_CSMRCSPLUS4)
		{
			mptman->CommonBlock.CSMMass = aux->W_CSM - aux->MainFuelUsed - aux->RCSFuelUsed;
		}
		else
		{
			mptman->CommonBlock.CSMMass = aux->W_CSM;
		}
		mptman->TotalMassAfter += mptman->CommonBlock.CSMMass;

		mptman->CommonBlock.CSMArea = CommonBlockBefore->CSMArea;
		if (mptman->CommonBlock.CSMArea > mptman->TotalAreaAfter)
		{
			mptman->TotalAreaAfter = mptman->CommonBlock.CSMArea;
		}
	}
	if (mptman->CommonBlock.ConfigCode[RTCC_CONFIG_A])
	{
		//Was it an LM ascent stage engine?
		if (mptman->Thruster == RTCC_ENGINETYPE_LMAPS || mptman->Thruster == RTCC_ENGINETYPE_LMRCSMINUS2 || mptman->Thruster == RTCC_ENGINETYPE_LMRCSMINUS4 || mptman->Thruster == RTCC_ENGINETYPE_LMRCSPLUS2 || mptman->Thruster == RTCC_ENGINETYPE_LMRCSPLUS4)
		{
			mptman->CommonBlock.LMAscentMass = aux->W_LMA - aux->MainFuelUsed - aux->RCSFuelUsed;
		}
		else
		{
			mptman->CommonBlock.LMAscentMass = aux->W_LMA;
		}
		mptman->TotalMassAfter += mptman->CommonBlock.LMAscentMass;

		mptman->CommonBlock.LMAscentArea = CommonBlockBefore->LMAscentArea;
		if (mptman->CommonBlock.LMAscentArea > mptman->TotalAreaAfter)
		{
			mptman->TotalAreaAfter = mptman->CommonBlock.LMAscentArea;
		}
	}
	if (mptman->CommonBlock.ConfigCode[RTCC_CONFIG_D])
	{
		if (mptman->Thruster == RTCC_ENGINETYPE_LMDPS)
		{
			mptman->CommonBlock.LMAscentMass = aux->W_LMA - aux->RCSFuelUsed;
			mptman->CommonBlock.LMDescentMass = aux->W_LMD - aux->MainFuelUsed;
		}
		else
		{
			mptman->CommonBlock.LMDescentMass = aux->W_LMD;
		}
		
		mptman->TotalMassAfter += mptman->CommonBlock.LMDescentMass;

		mptman->CommonBlock.LMDescentArea = CommonBlockBefore->LMDescentArea;
		if (mptman->CommonBlock.LMDescentArea > mptman->TotalAreaAfter)
		{
			mptman->TotalAreaAfter = mptman->CommonBlock.LMDescentArea;
		}
	}
	if (mptman->CommonBlock.ConfigCode[RTCC_CONFIG_S])
	{
		if (mptman->Thruster == RTCC_ENGINETYPE_LOX_DUMP || mptman->Thruster == RTCC_ENGINETYPE_SIVB_MAIN)
		{
			mptman->CommonBlock.SIVBMass = aux->W_SIVB - aux->MainFuelUsed - aux->RCSFuelUsed;
		}
		else
		{
			mptman->CommonBlock.SIVBMass = aux->W_SIVB;
		}
		
		mptman->TotalMassAfter += mptman->CommonBlock.SIVBMass;

		mptman->CommonBlock.SIVBArea = CommonBlockBefore->SIVBArea;
		if (mptman->CommonBlock.SIVBArea > mptman->TotalAreaAfter)
		{
			mptman->TotalAreaAfter = mptman->CommonBlock.SIVBArea;
		}
	}

	mpt->WeightAfterManeuver[man - 1] = mptman->TotalMassAfter;
	mpt->AreaAfterManeuver[man - 1] = mptman->TotalAreaAfter;

	//To prevent NaNs
	if (mptman->TotalMassAfter - F <= 0.0)
	{
		mptman->DVREM = 0.0;
	}
	else
	{
		mptman->DVREM = T / WDOT * log(mptman->TotalMassAfter / (mptman->TotalMassAfter - F));
	}
	
	if (mptman->Thruster == RTCC_ENGINETYPE_LOX_DUMP)
	{
		mptman->DVC = mptman->DVXBT = aux->DV*cos(0.0);
	}
	else
	{
		mptman->DVC = aux->DV_C;
		mptman->DVXBT = aux->DV_C + aux->DV_cTO;
	}
	//Inertial, manual
	if (mptman->AttitudeCode < 3)
	{
		V_G = aux->A_T*aux->DV;
		sv_FF = aux->sv_FF;
		mptman->dV_inertial = V_G;
	}
	//Lambert, External DV, AGS
	else if (mptman->AttitudeCode != 6)
	{
		V_G = aux->V_G;
		sv_FF = aux->sv_FF;
		mptman->dV_inertial = V_G;
	}
	//IGM
	else
	{
		EMSMISSInputTable intab;
		double dt;

		intab.AnchorVector.R = aux->R_1;
		intab.AnchorVector.V = aux->V_1;
		intab.AnchorVector.GMT = aux->GMT_1;
		intab.AnchorVector.RBI = aux->RBI;
		intab.AuxTableIndicator = NULL;
		intab.CutoffIndicator = 1;
		intab.EphemerisBuildIndicator = false;
		intab.IgnoreManueverNumber = 0;
		intab.ManCutoffIndicator = 2;
		intab.ManeuverIndicator = false;
		dt = aux->GMT_BI - aux->GMT_1;
		intab.MaxIntegTime = abs(dt);
		if (dt < 0)
		{
			intab.IsForwardIntegration = -1.0;
		}
		intab.VehicleCode = L;

		EMSMISS(&intab);
		sv_FF = intab.NIAuxOutputTable.sv_cutoff;

		if (mptman->Word78i[0] != 0)
		{
			mptman->dV_inertial.z = aux->Word60;
			mptman->dV_LVLH.x = aux->Word61;
			mptman->dV_LVLH.y = aux->Word62;
			mptman->dV_LVLH.z = aux->Word63;
			mptman->Word67d = aux->Word64;
			mptman->Word68 = aux->Word65;
			mptman->Word69 = aux->Word66;
			mptman->Word70 = aux->Word67;
			mptman->Word71 = aux->Word68;
			mptman->Word72 = aux->Word69;
			mptman->Word78i[0] = 0;
		}
		V_G = aux->A_T*aux->DV;
	}
	VECTOR3 Z_PHV = -unit(sv_FF.R);
	VECTOR3 Y_PHV = unit(crossp(sv_FF.V, sv_FF.R));
	VECTOR3 X_PHV = crossp(Y_PHV, Z_PHV);
	mptman->P_H = asin(dotp(-Z_PHV, aux->X_B));
	double P_H_apo = abs(mptman->P_H);
	if (abs(P_H_apo - PI05) < 0.0017)
	{
		mptman->R_H = 0.0;
		mptman->Y_H = atan2(dotp(-X_PHV,aux->Y_B), dotp(Y_PHV,aux->Y_B));
	}
	else
	{
		mptman->Y_H = atan2(dotp(Y_PHV, aux->X_B), dotp(X_PHV, aux->X_B));
		mptman->R_H = atan2(dotp(Z_PHV, aux->Y_B), dotp(Z_PHV, aux->Z_B));
	}
	if (mptman->Y_H < 0)
	{
		mptman->Y_H += PI2;
	}
	if (mptman->R_H < 0)
	{
		mptman->R_H += PI2;
	}
	mptman->V_F = dotp(X_PHV, V_G);
	mptman->V_S = dotp(Y_PHV, V_G);
	mptman->V_D = dotp(Z_PHV, V_G);

	EphemerisData2 sv_BI_true, sv_BO_true;

	int in, out, err;

	if (aux->RBI == BODY_EARTH)
	{
		in = 0;
		out = 1;
	}
	else
	{
		in = 2;
		out = 3;
	}

	if (aux->GMT_BI == 0.0)
	{
		sv_BO_true.R = aux->R_BO;
		sv_BO_true.V = aux->V_BO;
		sv_BO_true.GMT = aux->GMT_BO;

		err = ELVCNV(sv_BO_true, in, out, sv_BO_true);

		sv_BI_true = sv_BO_true;
	}
	else
	{
		sv_BO_true.R = aux->R_BO;
		sv_BO_true.V = aux->V_BO;
		sv_BO_true.GMT = aux->GMT_BO;

		err = ELVCNV(sv_BO_true, in, out, sv_BO_true);

		if (err == 0)
		{
			sv_BI_true.R = aux->R_BI;
			sv_BI_true.V = aux->V_BI;
			sv_BI_true.GMT = aux->GMT_BI;

			err = ELVCNV(sv_BI_true, in, out, sv_BI_true);
		}
	}

	if (err)
	{
		PMXSPT("PMMDMT", 69);
	}
	else
	{
		double mu, R_E, K;

		if (aux->RBI == BODY_EARTH)
		{
			mu = OrbMech::mu_Earth;
			K = 1.0;
			R_E = OrbMech::R_Earth;

			double R_EE = length(sv_BI_true.R) / sqrt((sv_BI_true.R.x*sv_BI_true.R.x + sv_BI_true.R.y*sv_BI_true.R.y) / SystemParameters.MCEASQ + sv_BI_true.R.z*sv_BI_true.R.z / SystemParameters.MCEBSQ);
			mptman->h_BI = length(sv_BI_true.R) - R_EE;
			mptman->lat_BI = atan(SystemParameters.MCEBAS*sv_BI_true.R.z / sqrt(sv_BI_true.R.x*sv_BI_true.R.x + sv_BI_true.R.y*sv_BI_true.R.y));
		}
		else
		{
			mu = OrbMech::mu_Moon;
			K = 0.0;
			R_E = BZLAND.rad[RTCC_LMPOS_BEST];

			mptman->h_BI = length(sv_BI_true.R) - BZLAND.rad[RTCC_LMPOS_BEST];
			mptman->lat_BI = asin(sv_BI_true.R.z / sv_BI_true.R.x);
		}

		mptman->lng_BI = atan2(sv_BI_true.R.y, sv_BI_true.R.x) - K * OrbMech::w_Earth*sv_BI_true.GMT;
		while (mptman->lng_BI < 0)
		{
			mptman->lng_BI += PI2;
		}
		double h = length(crossp(sv_BI_true.R, sv_BI_true.V));
		mptman->eta_BI = atan2(h*dotp(sv_BI_true.R, sv_BI_true.V), h*h - mu * length(sv_BI_true.R));
		if (mptman->eta_BI < 0)
		{
			mptman->eta_BI += PI2;
		}

		AEGBlock aeg;

		PIMCKC(sv_BO_true.R, sv_BO_true.V, aux->RBI, aeg.Data.coe_osc.a, aeg.Data.coe_osc.e, aeg.Data.coe_osc.i, aeg.Data.coe_osc.l, aeg.Data.coe_osc.g, aeg.Data.coe_osc.h);
		mptman->e_BO = aeg.Data.coe_osc.e;
		mptman->i_BO = aeg.Data.coe_osc.i;
		mptman->g_BO = aeg.Data.coe_osc.g;

		//Set up AEG input
		if (aux->RBI == BODY_MOON) //TBD: Is this necessary?
		{
			PIMCKC(aux->R_BO, aux->V_BO, aux->RBI, aeg.Data.coe_osc.a, aeg.Data.coe_osc.e, aeg.Data.coe_osc.i, aeg.Data.coe_osc.l, aeg.Data.coe_osc.g, aeg.Data.coe_osc.h);
		}
		aeg.Data.TS = aeg.Data.TE = sv_BO_true.GMT;
		aeg.Header.AEGInd = aux->RBI;

		AEGDataBlock sv_A, sv_P;
		double INFO[10];

		if (PMMAPD(aeg.Header, aeg.Data, 0, -1, INFO, &sv_A, &sv_P))
		{
			PMXSPT("PMMDMT", 108);
		}
		else
		{
			mptman->GMT_a = INFO[0];
			mptman->GMT_p = INFO[5];

			if (INFO[1] != 0.0)
			{
				mptman->h_a = INFO[1] - R_E;
			}
			if (INFO[6] != 0.0)
			{
				mptman->h_p = INFO[6] - R_E;
				//TBD: V_P
			}
			double h_an;
			if (aux->RBI == BODY_EARTH)
			{
				aeg.Data.TIMA = 2;
				aeg.Data.Item8 = 0.0;
				aeg.Data.Item10 = 1.0;
				pmmaeg.CALL(aeg.Header, aeg.Data, aeg.Data);
				h_an = aeg.Data.coe_osc.h;
			}
			else
			{
				if (aeg.Data.coe_osc.e < 1.0)
				{
					double isg, gsg, hsg;
					PIATSU(aeg.Data, aeg.Data, isg, gsg, hsg);
					h_an = hsg;
				}
			}
			if (aeg.Header.ErrorInd)
			{
				RTCCONLINEMON.TextBuffer[0] = mptman->code;
				PMXSPT("PMMDMT", 65);
			}
			else
			{
				mptman->GMT_AN = aeg.Data.TE;
				mptman->lng_AN = h_an - K * OrbMech::w_Earth*mptman->GMT_AN;
			}
		}
	}
	//TBD: Day and night
}

double PIMDIR(double X, double *XI, double *YI, unsigned N)
{
	unsigned N1;

	N1 = N;

	if (N1 < 2)
	{
		return YI[N1 - 1];
	}
	if (N1 > 2)
	{
		for (N1 = 2; N1 < N; N1++)
		{
			if (X < XI[N1 - 1]) break;
		}
	}

	//TBD: More than just linear interpolation

	return (YI[N1 - 1] - YI[N1 - 2]) / (XI[N1 - 1] - XI[N1 - 2])*(X - XI[N1 - 2]) + YI[N1 - 2];
}

void RTCC::PMMSFPIN()
{
	unsigned i, j;
	int day, opportunity;
	double azimuth;

	//Read PZSFPTAB block 6
	day = PZSFPTAB.Day;
	opportunity = PZSFPTAB.Opportunity;
	azimuth = PZSFPTAB.Azimuth;

	if (PZMFPTAB.data.size() == 0)
	{
		//Error
		return;
	}

	//Can the day even be in the table?
	if (day < PZMFPTAB.data.front().day || day > PZMFPTAB.data.back().day)
	{
		//Error
		return;
	}

	//Find day in table
	bool found = false;

	for (i = 0; i < PZMFPTAB.data.size(); i++)
	{
		if (PZMFPTAB.data[i].day == day)
		{
			found = true;
			break;
		}
	}

	if (found == false)
	{
		//Error
		return;
	}

	MasterFlightPlanTableDay *pDay = &PZMFPTAB.data[i];

	//Find opportunity in table
	found = false;

	for (i = 0; i < pDay->data.size(); i++)
	{
		if (pDay->data[i].Opportunity == opportunity)
		{
			found = true;
			break;
		}
	}

	if (found == false)
	{
		//Error
		return;
	}

	TLMCCDataTable temptab;
	MasterFlightPlanTableOpportunity *pOpp = &pDay->data[i];
	unsigned N = pOpp->data.size();
	double *XARRAY = new double[N];
	double *YARRAY = new double[N];
	double X = azimuth;
	double P;

	for (j = 0; j < pOpp->data.size(); j++)
	{
		XARRAY[j] = pOpp->data[j].dAzimuth;
	}

	for (i = 0; i < 25; i++)
	{
		for (j = 0; j < pOpp->data.size(); j++)
		{
			YARRAY[j] = pOpp->data[j].data.data[i];
		}
		//Do interpolation
		P = PIMDIR(X, XARRAY, YARRAY, N);
		//Put in table
		temptab.data[i] = P;
	}

	//Copy to preflight table and store time
	temptab.GMTTimeFlag = RTCCPresentTimeGMT();
	PZSFPTAB.blocks[0] = temptab;

	//TBD: Store landing site coordinates?

	delete[] XARRAY;
	delete[] YARRAY;
}

//CMC External Delta-V Update Generator
void RTCC::CMMAXTDV(double GETIG, VECTOR3 DV_EXDV, int mpt, unsigned man)
{
	if (CZAXTRDV.LoadNumber == 0)
	{
		CZAXTRDV.LoadNumber = 1001;
	}
	else
	{
		CZAXTRDV.LoadNumber++;
	}
	CZAXTRDV.GenGET = GETfromGMT(RTCCPresentTimeGMT());

	CZAXTRDV.Octals[0] = 12;
	CZAXTRDV.Octals[1] = SystemParameters.MCCCEX;
	CZAXTRDV.Octals[2] = OrbMech::DoubleToBuffer(DV_EXDV.x / 100.0, 7, 1);
	CZAXTRDV.Octals[3] = OrbMech::DoubleToBuffer(DV_EXDV.x / 100.0, 7, 0);
	CZAXTRDV.Octals[4] = OrbMech::DoubleToBuffer(DV_EXDV.y / 100.0, 7, 1);
	CZAXTRDV.Octals[5] = OrbMech::DoubleToBuffer(DV_EXDV.y / 100.0, 7, 0);
	CZAXTRDV.Octals[6] = OrbMech::DoubleToBuffer(DV_EXDV.z / 100.0, 7, 1);
	CZAXTRDV.Octals[7] = OrbMech::DoubleToBuffer(DV_EXDV.z / 100.0, 7, 0);
	CZAXTRDV.Octals[8] = OrbMech::DoubleToBuffer(GETIG*100.0, 28, 1);
	CZAXTRDV.Octals[9] = OrbMech::DoubleToBuffer(GETIG*100.0, 28, 0);

	CZAXTRDV.GET = GETIG;
	CZAXTRDV.DV = DV_EXDV / 0.3048;

	if (mpt)
	{
		MissionPlanTable *m = GetMPTPointer(mpt);
		CZAXTRDV.ManeuverCode = m->mantable[man - 1].code;
		CZAXTRDV.GMTID = m->GMTAV;
		CZAXTRDV.StationID = m->StationID;
	}
	else
	{
		CZAXTRDV.ManeuverCode = "";
		CZAXTRDV.GMTID = 0.0;
		CZAXTRDV.StationID = "";
	}
	CMDAXTDV();
}

void RTCC::CMDAXTDV()
{

}

//LGC External Delta-V Update Generator
void RTCC::CMMLXTDV(double GETIG, VECTOR3 DV_EXDV, int mpt, unsigned man)
{
	if (CZLXTRDV.LoadNumber == 0)
	{
		CZLXTRDV.LoadNumber = 2201;
	}
	else
	{
		CZLXTRDV.LoadNumber++;
	}
	CZLXTRDV.GenGET = GETfromGMT(RTCCPresentTimeGMT());

	CZLXTRDV.Octals[0] = 12;
	CZLXTRDV.Octals[1] = SystemParameters.MCCLEX;
	CZLXTRDV.Octals[2] = OrbMech::DoubleToBuffer(DV_EXDV.x / 100.0, 7, 1);
	CZLXTRDV.Octals[3] = OrbMech::DoubleToBuffer(DV_EXDV.x / 100.0, 7, 0);
	CZLXTRDV.Octals[4] = OrbMech::DoubleToBuffer(DV_EXDV.y / 100.0, 7, 1);
	CZLXTRDV.Octals[5] = OrbMech::DoubleToBuffer(DV_EXDV.y / 100.0, 7, 0);
	CZLXTRDV.Octals[6] = OrbMech::DoubleToBuffer(DV_EXDV.z / 100.0, 7, 1);
	CZLXTRDV.Octals[7] = OrbMech::DoubleToBuffer(DV_EXDV.z / 100.0, 7, 0);
	CZLXTRDV.Octals[8] = OrbMech::DoubleToBuffer(GETIG*100.0, 28, 1);
	CZLXTRDV.Octals[9] = OrbMech::DoubleToBuffer(GETIG*100.0, 28, 0);

	CZLXTRDV.GET = GETIG;
	CZLXTRDV.DV = DV_EXDV / 0.3048;

	if (mpt)
	{
		MissionPlanTable *m = GetMPTPointer(mpt);
		CZLXTRDV.ManeuverCode = m->mantable[man - 1].code;
		CZLXTRDV.GMTID = m->GMTAV;
		CZLXTRDV.StationID = m->StationID;
	}
	else
	{
		CZLXTRDV.ManeuverCode = "";
		CZLXTRDV.GMTID = 0.0;
		CZLXTRDV.StationID = "";
	}
}

void RTCC::CMMRFMAT(int L, int id, int addr)
{
	REFSMMATUpdateMakeupTableBlock *block;
	if (L == 1)
	{
		block = &CZREFMAT.Block[0];
	}
	else
	{
		block = &CZREFMAT.Block[1];
	}

	block->error = "";

	REFSMMATData refs;
	if (L == 1)
	{
		refs = EZJGMTX1.data[id - 1];
	}
	else
	{
		refs = EZJGMTX3.data[id - 1];
	}
	if (refs.ID <= 0)
	{
		block->error = "REFSMMAT NOT AVAILABLE";
		return;
	}

	char buff[7];
	FormatREFSMMATCode(id, refs.ID, buff);
	block->MatrixID.assign(buff);

	MATRIX3 a = refs.REFSMMAT;
	block->REFSMMAT = a;

	//sprintf(oapiDebugString(), "%f, %f, %f, %f, %f, %f, %f, %f, %f", a.m11, a.m12, a.m13, a.m21, a.m22, a.m23, a.m31, a.m32, a.m33);

	block->Octals[0] = 24;
	if (addr == 1)
	{
		block->MatrixType = 1;
		if (L == 1)
		{
			block->Octals[1] = SystemParameters.MCCCRF;
		}
		else
		{
			block->Octals[1] = SystemParameters.MCCLRF;
		}
	}
	else
	{
		block->MatrixType = 2;
		if (L == 1)
		{
			block->Octals[1] = SystemParameters.MCCCXS;
		}
		else
		{
			block->Octals[1] = SystemParameters.MCCLXS;
		}
	}
	block->Octals[2] = OrbMech::DoubleToBuffer(a.m11, 1, 1);
	block->Octals[3] = OrbMech::DoubleToBuffer(a.m11, 1, 0);
	block->Octals[4] = OrbMech::DoubleToBuffer(a.m12, 1, 1);
	block->Octals[5] = OrbMech::DoubleToBuffer(a.m12, 1, 0);
	block->Octals[6] = OrbMech::DoubleToBuffer(a.m13, 1, 1);
	block->Octals[7] = OrbMech::DoubleToBuffer(a.m13, 1, 0);
	block->Octals[8] = OrbMech::DoubleToBuffer(a.m21, 1, 1);
	block->Octals[9] = OrbMech::DoubleToBuffer(a.m21, 1, 0);
	block->Octals[10] = OrbMech::DoubleToBuffer(a.m22, 1, 1);
	block->Octals[11] = OrbMech::DoubleToBuffer(a.m22, 1, 0);
	block->Octals[12] = OrbMech::DoubleToBuffer(a.m23, 1, 1);
	block->Octals[13] = OrbMech::DoubleToBuffer(a.m23, 1, 0);
	block->Octals[14] = OrbMech::DoubleToBuffer(a.m31, 1, 1);
	block->Octals[15] = OrbMech::DoubleToBuffer(a.m31, 1, 0);
	block->Octals[16] = OrbMech::DoubleToBuffer(a.m32, 1, 1);
	block->Octals[17] = OrbMech::DoubleToBuffer(a.m32, 1, 0);
	block->Octals[18] = OrbMech::DoubleToBuffer(a.m33, 1, 1);
	block->Octals[19] = OrbMech::DoubleToBuffer(a.m33, 1, 0);

	block->UpdateNo++;
}

int RTCC::BMQDCMED(std::string med, std::vector<std::string> data)
{
	//LM Position Vector Table (Should be in BMLMED instead)
	if (med == "72")
	{
		if (data.size() != 2)
		{
			return 1;
		}
		int id1;
		if (data[0] == "BEST")
		{
			id1 = 0;
		}
		else if (data[0] == "PNGCS")
		{
			id1 = 1;
		}
		else if (data[0] == "AGS")
		{
			id1 = 2;
		}
		else if (data[0] == "MED")
		{
			id1 = 3;
		}
		else
		{
			return 2;
		}
		int id2;
		if (data[1] == "PNGCS")
		{
			id2 = 1;
		}
		else if (data[1] == "AGS")
		{
			id2 = 2;
		}
		else if (data[1] == "MED")
		{
			id2 = 3;
		}
		else
		{
			return 2;
		}
		BZLAND.lat[id1] = BZLAND.lat[id2];
		BZLAND.lng[id1] = BZLAND.lng[id2];
		BZLAND.rad[id1] = BZLAND.rad[id2];
	}
	//Simulate vector control and DC PBI's
	else if (med == "99")
	{
		if (data.size() != 1)
		{
			return 1;
		}

		int code;
		if (sscanf(data[0].c_str(), "%d", &code) != 1)
		{
			return 2;
		}
		if (code >= 300 && code <= 335)
		{
			BMSVPS(0, code);
		}

		return 0;
	}
	return 1;
}

int RTCC::BMSVPSVectorFetch(const std::string &vecid, EphemerisData &sv_out)
{
	int i;

	//Search in evaluation vector table
	for (i = 0;i < 8;i++)
	{
		if (vecid == BZEVLVEC.data[i].VectorCode)
		{
			sv_out = BZEVLVEC.data[i].Vector;
			return 0;
		}
	}
	//Search in usable vector table
	for (i = 0;i < 12;i++)
	{
		if (vecid == BZUSEVEC.data[i].VectorCode)
		{
			sv_out = BZUSEVEC.data[i].Vector;
			return 0;
		}
	}

	return 1;
}

void RTCC::BMSVPS(int queid, int PBIID)
{
	//0 = Vector control queue
	//1 = DC control
	//2 = Terminate orbit high-speed processing
	//3 = S84 MED entry
	//4 = Vector fetch

	//PBI queue?
	if (queid <= 1)
	{
		goto RTCC_BMSVPS_1;
	}

	//PBI Queue
RTCC_BMSVPS_1:
	//To evaluation slot?
	if ((PBIID >= 300 && PBIID <= 303) || (PBIID >= 308 && PBIID <= 311))
	{
		int tabid;

		//Decode push button ID
		switch (PBIID)
		{
		case 300:
		case 301:
		case 302:
		case 303:
			tabid = PBIID - 300;
			break;
		case 308:
		case 309:
		case 310:
		case 311:
			tabid = PBIID - 304;
			break;
		}

		//Get telemetry vector from BZSTLM and save vector code
		EphemerisData sv;
		char Buff[5];
		switch (tabid)
		{
		case 0:
			sv = BZSTLM.HighSpeedCMCCSMVector;
			sprintf_s(Buff, "CCHE");
			break;
		case 1:
			sv = BZSTLM.HighSpeedLGCCSMVector;
			sprintf_s(Buff, "LCHE");
			break;
		case 2:
			sv = BZSTLM.HighSpeedAGSCSMVector;
			sprintf_s(Buff, "ACHE");
			break;
		case 3:
			sv = BZSTLM.HighSpeedIUVector;
			sprintf_s(Buff, "ICHE");
			break;
		case 4:
			sv = BZSTLM.HighSpeedCMCLEMVector;
			sprintf_s(Buff, "CLHE");
			break;
		case 5:
			sv = BZSTLM.HighSpeedLGCLEMVector;
			sprintf_s(Buff, "LLHE");
			break;
		case 6:
			sv = BZSTLM.HighSpeedAGSLEMVector;
			sprintf_s(Buff, "ALHE");
			break;
		case 7:
			sv = BZSTLM.HighSpeedIUVector;
			sprintf_s(Buff, "ILHE");
			break;
		}

		char Buff2[16];

		//Save in correct slot of evaluation table
		BZEVLVEC.data[tabid].Vector = sv;
		if (BZEVLVEC.data[tabid].ID < 0)
		{
			BZEVLVEC.data[tabid].ID = 1;
		}
		else
		{
			BZEVLVEC.data[tabid].ID++;
		}
		sprintf_s(Buff2, "%s%03d", Buff, BZEVLVEC.data[tabid].ID);
		BZEVLVEC.data[tabid].VectorCode.assign(Buff2);
	}
	//To usable slot?
	else if (PBIID >= 316 && PBIID <= 323)
	{
		int etabid, utabid;

		//Decode push button ID
		etabid = PBIID - 316;
		if (PBIID <= 319)
		{
			utabid = PBIID - 316;
		}
		else
		{
			utabid = PBIID - 314;
		}

		//Get Evaluation vector
		if (BZEVLVEC.data[etabid].ID < 0)
		{
			//Error
			return;
		}

		char Buff[5];
		switch (etabid)
		{
		case 0:
			sprintf_s(Buff, "CCHU");
			break;
		case 1:
			sprintf_s(Buff, "LCHU");
			break;
		case 2:
			sprintf_s(Buff, "ACHU");
			break;
		case 3:
			sprintf_s(Buff, "ICHU");
			break;
		case 4:
			sprintf_s(Buff, "CLHU");
			break;
		case 5:
			sprintf_s(Buff, "LLHU");
			break;
		case 6:
			sprintf_s(Buff, "ALHU");
			break;
		case 7:
			sprintf_s(Buff, "ILHU");
			break;
		}

		char Buff2[16];

		//Save in correct slot of usable vector table
		BZUSEVEC.data[utabid].Vector = BZEVLVEC.data[etabid].Vector;
		if (BZUSEVEC.data[utabid].ID < 0)
		{
			BZUSEVEC.data[utabid].ID = 1;
		}
		else
		{
			BZUSEVEC.data[utabid].ID++;
		}
		sprintf_s(Buff2, "%s%03d", Buff, BZUSEVEC.data[utabid].ID);
		BZUSEVEC.data[utabid].VectorCode.assign(Buff2);

		RTCCONLINEMON.TextBuffer[0] = BZUSEVEC.data[utabid].VectorCode;
		if (etabid <= 3)
		{
			RTCCONLINEMON.TextBuffer[1] = "CSM";
		}
		else
		{
			RTCCONLINEMON.TextBuffer[1] = "LEM";
		}
		BMGPRIME("BMSVPS", 42);
	}
	//To ephemeris update?
	else if (PBIID >= 324 && PBIID <= 335)
	{
		int id = PBIID - 324;

		//Get vector to cause ephemeris update
		if (BZUSEVEC.data[id].ID < 0)
		{
			//Error
			return;
		}

		int queid, L;
		switch (PBIID)
		{
		case 328:
		case 334:
			queid = 5;
			break;
		case 329:
		case 335:
			queid = 4;
			break;
		default:
			queid = 6;
			break;
		}
		if (PBIID <= 329)
		{
			L = RTCC_MPT_CSM;
		}
		else
		{
			L = RTCC_MPT_LM;
		}

		//Never use a landing site vector for the CSM ephemeris
		if (L == RTCC_MPT_CSM && BZUSEVEC.data[id].LandingSiteIndicator)
		{
			return;
		}

		PMSVCT(queid, L, BZUSEVEC.data[id]);
	}
}

void RTCC::BMSVEC()
{
	//If time is missing the time of the first vector is the assumed GMT and the first vector ID cannot be "EPHO" or "EPHR"
	if (med_s80.time == 0.0 && (med_s80.VID[0] == "EPHO" || med_s80.VID[0] == "EPHR"))
	{
		return;
	}

	OELEMENTS coe;
	double mu, r_apo, r_peri, R_E, r, v, lat, lng, gamma, azi, gmt;
	int numvec = 0;

	EMSMISSInputTable emsmissin;
	EphemerisData sv_comp[4];
	EphemerisData sv_final[4];

	//Reset
	BZCCANOE = VectorCompareTable();

	ELVCTRInputTable intab;
	ELVCTROutputTable2 outtab;

	//Get GMT for comparison
	if (med_s80.time > 0)
	{
		gmt = med_s80.time;
	}
	else if (med_s80.time < 0)
	{
		gmt = GMTfromGET(-med_s80.time);
	}
	else
	{
		//gmt gets set later
		gmt = 0.0;
	}

	for (int i = 0;i < 4;i++)
	{
		if (med_s80.VID[i] == "")
		{
			break;
		}
		//Are we requesting an ephemeris vector? 
		else if (med_s80.VID[i] == "EPHO")
		{
			intab.EphemerisType = 0;
			intab.GMT = gmt;
			intab.L = med_s80.VEH;

			ELVCTR(intab, outtab);

			//Error code 2 acceptable
			if (outtab.ErrorCode > 2)
			{
				BMGPRIME("BMSVEC", 10);
				BZCCANOE.error = true;
				goto RTCC_BMSVEC_1;
			}
			sv_comp[i] = RotateSVToSOI(outtab.SV);
			numvec++;
		}
		else
		{
			if (BMSVPSVectorFetch(med_s80.VID[i], sv_comp[i]))
			{
				RTCCONLINEMON.TextBuffer[0] = med_s80.VID[i];
				BMGPRIME("BMSVEC", 43);
				BZCCANOE.error = true;
				goto RTCC_BMSVEC_1;
			}
			numvec++;
		}
		if (gmt == 0.0)
		{
			gmt = sv_comp[i].GMT;
		}
	}

	BZCCANOE.NumVec = numvec;

	if (med_s80.REF == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
		R_E = OrbMech::R_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		R_E = BZLAND.rad[RTCC_LMPOS_BEST];
	}

	VECTOR3 U, V, W;

	emsmissin.AuxTableIndicator = NULL;
	emsmissin.CutoffIndicator = 1;
	emsmissin.DensityMultOverrideIndicator = false;
	emsmissin.DescentBurnIndicator = false;
	emsmissin.EphemerisBuildIndicator = false;
	emsmissin.IgnoreManueverNumber = 0;
	emsmissin.landed = false;
	emsmissin.ManCutoffIndicator = 2;
	if (med_s80.useMPT)
	{
		emsmissin.ManeuverIndicator = true;
	}
	else
	{
		emsmissin.ManeuverIndicator = false;
	}
	emsmissin.ManTimesIndicator = NULL;
	emsmissin.RefSwitchTabIndicator = NULL;
	emsmissin.useInputWeights = false;
	emsmissin.VehicleCode = med_s80.VEH;

	int csi_out;

	if (med_s80.REF == BODY_EARTH)
	{
		csi_out = 0;
	}
	else
	{
		csi_out = 2;
	}

	for (int i = 0;i < numvec;i++)
	{
		//EMSMISS settings
		emsmissin.AnchorVector = sv_comp[i];
		if (gmt - sv_comp[i].GMT >= 0.0)
		{
			emsmissin.IsForwardIntegration = 1.0;
		}
		else
		{
			emsmissin.IsForwardIntegration = -1.0;
		}
		emsmissin.MaxIntegTime = abs(gmt - sv_comp[i].GMT);
		
		//Propagate state vector
		EMSMISS(&emsmissin);

		//Convert to desired coordinate system
		ELVCNV(emsmissin.NIAuxOutputTable.sv_cutoff, csi_out, sv_final[i]);
		sv_final[i] = emsmissin.NIAuxOutputTable.sv_cutoff;

		if (i == 0)
		{
			U = unit(sv_final[i].R);
			W = unit(crossp(sv_final[i].R, sv_final[i].V));
			V = unit(crossp(-U, W));
		}

		BZCCANOE.data[i].GMT = sv_final[i].GMT;
		OrbMech::periapo(sv_final[i].R, sv_final[i].V, mu, r_apo, r_peri);
		coe = OrbMech::coe_from_sv(sv_final[i].R, sv_final[i].V, mu);
		PICSSC(true, sv_final[i].R, sv_final[i].V, r, v, lat, lng, gamma, azi);
		if (lng > PI)
		{
			lng -= PI2;
		}

		if (coe.e >= 1.0)
		{
			BZCCANOE.data[i].HA = 0.0;
		}
		else
		{
			BZCCANOE.data[i].HA = (r_apo - R_E);
		}
		BZCCANOE.data[i].HP = (r_peri - R_E);
		BZCCANOE.data[i].v = v;
		BZCCANOE.data[i].gamma = gamma;
		BZCCANOE.data[i].psi = azi;
		BZCCANOE.data[i].phi = lat;
		BZCCANOE.data[i].lambda = lng;
		BZCCANOE.data[i].h = (r - R_E);
		BZCCANOE.data[i].a = 1.0 / (2.0 / r - v * v / mu);
		BZCCANOE.data[i].e = coe.e;
		BZCCANOE.data[i].i = coe.i;
		BZCCANOE.data[i].theta_p = coe.w;
		BZCCANOE.data[i].Omega = coe.RA;
		BZCCANOE.data[i].nu = coe.TA;
		BZCCANOE.data[i].U = dotp(sv_final[i].R, U);
		BZCCANOE.data[i].V = dotp(sv_final[i].R, V);
		BZCCANOE.data[i].W = dotp(sv_final[i].R, W);
		BZCCANOE.data[i].U_dot = dotp(sv_final[i].V, U);
		BZCCANOE.data[i].V_dot = dotp(sv_final[i].V, V);
		BZCCANOE.data[i].W_dot = dotp(sv_final[i].V, W);
	}

RTCC_BMSVEC_1:
	BMDVEC();
}

void RTCC::BMDVEC()
{
	VectorCompareDisplayBuffer = VectorCompareDisplay();
	VectorCompareDisplayBuffer.error = "";

	if (BZCCANOE.error)
	{
		VectorCompareDisplayBuffer.error = "ERROR - REFER TO ONLINE";
		return;
	}

	VectorCompareDisplayBuffer.GMTR = GMTfromGET(SystemParameters.MCGREF);
	VectorCompareDisplayBuffer.PET = BZCCANOE.data[0].GMT - VectorCompareDisplayBuffer.GMTR;
	VectorCompareDisplayBuffer.NumVec = BZCCANOE.NumVec;

	for (int i = 0;i < BZCCANOE.NumVec;i++)
	{
		VectorCompareDisplayBuffer.data[i].GMT = BZCCANOE.data[i].GMT;

		if (BZCCANOE.data[i].e < 1.0)
		{
			VectorCompareDisplayBuffer.data[i].HA = BZCCANOE.data[i].HA / 1852.0;
			VectorCompareDisplayBuffer.showHA[i] = true;
		}
		else
		{
			VectorCompareDisplayBuffer.showHA[i] = false;
		}
		VectorCompareDisplayBuffer.data[i].HP = BZCCANOE.data[i].HP / 1852.0;
		VectorCompareDisplayBuffer.data[i].v = BZCCANOE.data[i].v / 0.3048;
		VectorCompareDisplayBuffer.data[i].gamma = BZCCANOE.data[i].gamma*DEG;
		VectorCompareDisplayBuffer.data[i].psi = BZCCANOE.data[i].psi*DEG;
		VectorCompareDisplayBuffer.data[i].phi = BZCCANOE.data[i].phi*DEG;
		VectorCompareDisplayBuffer.data[i].lambda = BZCCANOE.data[i].lambda*DEG;
		VectorCompareDisplayBuffer.data[i].h = BZCCANOE.data[i].h / 1852.0;
		VectorCompareDisplayBuffer.data[i].a = BZCCANOE.data[i].a / 1852.0;
		VectorCompareDisplayBuffer.data[i].e = BZCCANOE.data[i].e;
		VectorCompareDisplayBuffer.data[i].i = BZCCANOE.data[i].i*DEG;
		if (BZCCANOE.data[i].e >= 0.001)
		{
			VectorCompareDisplayBuffer.showWPAndTA[i] = true;
			VectorCompareDisplayBuffer.data[i].theta_p = BZCCANOE.data[i].theta_p*DEG;
			VectorCompareDisplayBuffer.data[i].nu = BZCCANOE.data[i].nu*DEG;
		}
		else
		{
			VectorCompareDisplayBuffer.showWPAndTA[i] = false;
		}
		VectorCompareDisplayBuffer.data[i].Omega = BZCCANOE.data[i].Omega*DEG;
		VectorCompareDisplayBuffer.data[i].U = BZCCANOE.data[i].U / 1852.0;
		VectorCompareDisplayBuffer.data[i].V = BZCCANOE.data[i].V / 1852.0;
		VectorCompareDisplayBuffer.data[i].W = BZCCANOE.data[i].W / 1852.0;
		VectorCompareDisplayBuffer.data[i].U_dot = BZCCANOE.data[i].U_dot / 0.3048;
		VectorCompareDisplayBuffer.data[i].V_dot = BZCCANOE.data[i].V_dot / 0.3048;
		VectorCompareDisplayBuffer.data[i].W_dot = BZCCANOE.data[i].W_dot / 0.3048;

		//Relative values
		if (i > 0)
		{
			if (VectorCompareDisplayBuffer.showHA[i])
			{
				VectorCompareDisplayBuffer.data[i].HA -= VectorCompareDisplayBuffer.data[0].HA;
			}
			VectorCompareDisplayBuffer.data[i].HP -= VectorCompareDisplayBuffer.data[0].HP;
			VectorCompareDisplayBuffer.data[i].v -= VectorCompareDisplayBuffer.data[0].v;
			VectorCompareDisplayBuffer.data[i].gamma -= VectorCompareDisplayBuffer.data[0].gamma;
			VectorCompareDisplayBuffer.data[i].psi -= VectorCompareDisplayBuffer.data[0].psi;
			VectorCompareDisplayBuffer.data[i].phi -= VectorCompareDisplayBuffer.data[0].phi;
			VectorCompareDisplayBuffer.data[i].lambda -= VectorCompareDisplayBuffer.data[0].lambda;
			VectorCompareDisplayBuffer.data[i].h -= VectorCompareDisplayBuffer.data[0].h;
			VectorCompareDisplayBuffer.data[i].a -= VectorCompareDisplayBuffer.data[0].a;
			VectorCompareDisplayBuffer.data[i].e -= VectorCompareDisplayBuffer.data[0].e;
			VectorCompareDisplayBuffer.data[i].i -= VectorCompareDisplayBuffer.data[0].i;
			if (VectorCompareDisplayBuffer.showWPAndTA[i])
			{
				VectorCompareDisplayBuffer.data[i].theta_p -= VectorCompareDisplayBuffer.data[0].theta_p;
				VectorCompareDisplayBuffer.data[i].nu -= VectorCompareDisplayBuffer.data[0].nu;
			}
			VectorCompareDisplayBuffer.data[i].Omega -= VectorCompareDisplayBuffer.data[0].Omega;
			VectorCompareDisplayBuffer.data[i].U -= VectorCompareDisplayBuffer.data[0].U;
			VectorCompareDisplayBuffer.data[i].V -= VectorCompareDisplayBuffer.data[0].V;
			VectorCompareDisplayBuffer.data[i].W -= VectorCompareDisplayBuffer.data[0].W;
			//Convert from NM to ft
			VectorCompareDisplayBuffer.data[i].U *= 1852.0 / 0.3048;
			VectorCompareDisplayBuffer.data[i].V *= 1852.0 / 0.3048;
			VectorCompareDisplayBuffer.data[i].W *= 1852.0 / 0.3048;
			VectorCompareDisplayBuffer.data[i].U_dot -= VectorCompareDisplayBuffer.data[0].U_dot;
			VectorCompareDisplayBuffer.data[i].V_dot -= VectorCompareDisplayBuffer.data[0].V_dot;
			VectorCompareDisplayBuffer.data[i].W_dot -= VectorCompareDisplayBuffer.data[0].W_dot;
		}
		//Limits
	}
}

void RTCC::BMDVPS()
{
	char Buff[64];
	double gmttemp;
	int i, j;
	MissionPlanTable *mpt;
	
	format_time_rtcc(Buff, RTCCPresentTimeGMT());
	VectorPanelSummaryBuffer.CurrentGMT.assign(Buff);
	for (i = 0;i < 2;i++)
	{
		if (i == 0)
		{
			mpt = &PZMPTCSM;
		}
		else
		{
			mpt = &PZMPTLEM;
		}
		if (mpt->GMTAV > 0)
		{
			VectorPanelSummaryBuffer.AnchorVectorID[i] = mpt->StationID;
			format_time_rtcc(Buff, mpt->GMTAV);
			VectorPanelSummaryBuffer.AnchorVectorGMT[i].assign(Buff);
		}
		else
		{
			VectorPanelSummaryBuffer.AnchorVectorID[i] = "";
			VectorPanelSummaryBuffer.AnchorVectorGMT[i] = "";
		}
		for (j = 0;j < 4;j++)
		{
			if (BZUSEVEC.data[6 * i + j].ID > 0)
			{
				VectorPanelSummaryBuffer.CompUsableID[i][j] = BZUSEVEC.data[6 * i + j].VectorCode;
				format_time_rtcc(Buff, BZUSEVEC.data[6 * i + j].Vector.GMT);
				VectorPanelSummaryBuffer.CompUsableGMT[i][j].assign(Buff);
			}
			else
			{
				VectorPanelSummaryBuffer.CompUsableID[i][j] = "";
				VectorPanelSummaryBuffer.CompUsableGMT[i][j] = "";
			}
			if (BZEVLVEC.data[4 * i + j].ID > 0)
			{
				VectorPanelSummaryBuffer.CompEvalID[i][j] = BZEVLVEC.data[4 * i + j].VectorCode;
				format_time_rtcc(Buff, BZEVLVEC.data[4 * i + j].Vector.GMT);
				VectorPanelSummaryBuffer.CompEvalGMT[i][j].assign(Buff);
			}
			else
			{
				VectorPanelSummaryBuffer.CompEvalID[i][j] = "";
				VectorPanelSummaryBuffer.CompEvalGMT[i][j] = "";
			}
			VectorPanelSummaryBuffer.CompTelemetryLowGMT[i][j] = ""; //TBD
		}
		if (BZUSEVEC.data[6 * i + 4].Vector.GMT > 0)
		{
			VectorPanelSummaryBuffer.HSRID[i] = BZUSEVEC.data[6 * i + 4].VectorCode;
			format_time_rtcc(Buff, BZUSEVEC.data[6 * i + 4].Vector.GMT);
			VectorPanelSummaryBuffer.HSRGMT[i].assign(Buff);
		}
		else
		{
			VectorPanelSummaryBuffer.HSRID[i] = "";
			VectorPanelSummaryBuffer.HSRGMT[i] = "";
		}
		if (BZUSEVEC.data[6 * i + 5].Vector.GMT > 0)
		{
			VectorPanelSummaryBuffer.DCID[i] = BZUSEVEC.data[6 * i + 5].VectorCode;
			format_time_rtcc(Buff, BZUSEVEC.data[6 * i + 5].Vector.GMT);
			VectorPanelSummaryBuffer.DCGMT[i].assign(Buff);
		}
		else
		{
			VectorPanelSummaryBuffer.DCID[i] = "";
			VectorPanelSummaryBuffer.DCGMT[i] = "";
		}
		if (mpt->LastExecutedManeuver > 0)
		{
			gmttemp = mpt->mantable[mpt->LastExecutedManeuver - 1].GMT_BI - mpt->mantable[mpt->LastExecutedManeuver - 1].dt_ullage + 1.0;
			format_time_rtcc(Buff, gmttemp);
			VectorPanelSummaryBuffer.LastManGMTUL[i].assign(Buff);
			format_time_rtcc(Buff, mpt->mantable[mpt->LastExecutedManeuver - 1].GMT_BO);
			VectorPanelSummaryBuffer.LastManGMTBO[i].assign(Buff);
		}
		else
		{
			VectorPanelSummaryBuffer.LastManGMTUL[i] = "";
			VectorPanelSummaryBuffer.LastManGMTBO[i] = "";
		}
		
	}
	if (BZSTLM.HighSpeedCMCCSMVector.GMT > 0)
	{
		format_time_rtcc(Buff, BZSTLM.HighSpeedCMCCSMVector.GMT);
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[0][0].assign(Buff);
	}
	else
	{
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[0][0] = "";
	}
	if (BZSTLM.HighSpeedLGCCSMVector.GMT > 0)
	{
		format_time_rtcc(Buff, BZSTLM.HighSpeedLGCCSMVector.GMT);
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[0][1].assign(Buff);
	}
	else
	{
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[0][1] = "";
	}
	if (BZSTLM.HighSpeedAGSCSMVector.GMT > 0)
	{
		format_time_rtcc(Buff, BZSTLM.HighSpeedAGSCSMVector.GMT);
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[0][2].assign(Buff);
	}
	else
	{
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[0][2] = "";
	}
	if (BZSTLM.HighSpeedIUVector.GMT > 0)
	{
		format_time_rtcc(Buff, BZSTLM.HighSpeedIUVector.GMT);
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[0][3].assign(Buff);
	}
	else
	{
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[0][3] = "";
	}
	if (BZSTLM.HighSpeedCMCLEMVector.GMT > 0)
	{
		format_time_rtcc(Buff, BZSTLM.HighSpeedCMCLEMVector.GMT);
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[1][0].assign(Buff);
	}
	else
	{
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[1][0] = "";
	}
	if (BZSTLM.HighSpeedLGCLEMVector.GMT > 0)
	{
		format_time_rtcc(Buff, BZSTLM.HighSpeedLGCLEMVector.GMT);
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[1][1].assign(Buff);
	}
	else
	{
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[1][1] = "";
	}
	if (BZSTLM.HighSpeedAGSLEMVector.GMT > 0)
	{
		format_time_rtcc(Buff, BZSTLM.HighSpeedAGSLEMVector.GMT);
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[1][2].assign(Buff);
	}
	else
	{
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[1][2] = "";
	}
	if (BZSTLM.HighSpeedIUVector.GMT > 0)
	{
		format_time_rtcc(Buff, BZSTLM.HighSpeedIUVector.GMT);
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[1][3].assign(Buff);
	}
	else
	{
		VectorPanelSummaryBuffer.CompTelemetryHighGMT[1][3] = "";
	}
}

void RTCC::BMGPRIME(std::string source, int n)
{
	std::vector<std::string> message;

	switch (n)
	{
	case 1:
		message.push_back("SPACECRAFT BELOW EI");
		break;
	case 10:
		message.push_back("INTERPOLATION ERROR");
		break;
	case 42:
		message.push_back(RTCCONLINEMON.TextBuffer[0] + " NOW A POSSIBLE " + RTCCONLINEMON.TextBuffer[1] + " ANCHOR VECTOR");
		break;
	case 43:
		message.push_back("CANNOT FIND VECTOR " + RTCCONLINEMON.TextBuffer[0]);
		break;
	}
	BMGPRIME(source, message);
}

void RTCC::BMGPRIME(std::string source, std::vector<std::string> message)
{
	OnlinePrint(source, message);
}

void RTCC::LMMGRP(int veh, double gmt)
{
	MATRIX3 REFS;
	double lat, lng, DLNG;

	if (veh == 2)
	{
		//IU2 (LM)
		lat = SystemParameters.MCLLLP[0];
		lng = SystemParameters.MCLLPL;
	}
	else
	{
		//CSM, IU1 (CSM)
		lat = SystemParameters.MCLLTP[0];
		lng = SystemParameters.MCLGRA;
	}

	DLNG = lng + SystemParameters.MCLAMD + SystemParameters.MCERTS * gmt / 3600.0;

	//CSM
	if (veh == 0)
	{
		REFS = GLMRTM(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), DLNG, 3, -PI05 - lat, 2, SystemParameters.MCLABN, 3);

		EZJGMTX1.data[0].ID = 0;
		EMGSTSTM(1, REFS, RTCC_REFSMMAT_TYPE_CUR, gmt);
	}
	//IU
	else
	{
		REFS = GLMRTM(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), DLNG, 3, -lat, 2, -SystemParameters.MCLABN, 1);

		if (veh == 1)
		{
			GZLTRA.IU1_REFSMMAT = REFS;
			RTCCONLINEMON.TextBuffer[0] = "IU1";
			RTCCONLINEMON.MatrixBuffer = GZLTRA.IU1_REFSMMAT;
		}
		else
		{
			GZLTRA.IU2_REFSMMAT = REFS;
			RTCCONLINEMON.TextBuffer[0] = "IU2";
			RTCCONLINEMON.MatrixBuffer = GZLTRA.IU2_REFSMMAT;
		}

		LMXPRNTR("LMMGRP", 10);
	}
}

void RTCC::LMXPRNTR(std::string source, int n)
{
	std::vector<std::string> message;
	char Buffer[128];

	switch (n)
	{
	case 1:
		message.push_back("LAUNCH IMPROPERLY CONDITIONED");
		break;
	case 10:
		message.push_back(RTCCONLINEMON.TextBuffer[0] + " REFSMMAT");
		sprintf_s(Buffer, "%+.7lf %+.7lf %+.7lf", RTCCONLINEMON.MatrixBuffer.m11, RTCCONLINEMON.MatrixBuffer.m12, RTCCONLINEMON.MatrixBuffer.m13);
		message.push_back(Buffer);
		sprintf_s(Buffer, "%+.7lf %+.7lf %+.7lf", RTCCONLINEMON.MatrixBuffer.m21, RTCCONLINEMON.MatrixBuffer.m22, RTCCONLINEMON.MatrixBuffer.m23);
		message.push_back(Buffer);
		sprintf_s(Buffer, "%+.7lf %+.7lf %+.7lf", RTCCONLINEMON.MatrixBuffer.m31, RTCCONLINEMON.MatrixBuffer.m32, RTCCONLINEMON.MatrixBuffer.m33);
		message.push_back(Buffer);
		break;
	case 11:
		message.push_back("VEHICLE OPTION INVALID - NO PLATFORM COMPUTED");
		break;
	}
	LMXPRNTR(source, message);
}

void RTCC::LMXPRNTR(std::string source, std::vector<std::string> message)
{
	OnlinePrint(source, message);
}

void RTCC::EMDAGSN(double GMT, int refs, int body)
{

}

void RTCC::EMSGSUPP(int QUEID, int refs, int refs2, unsigned man, bool headsup)
{
	//Initialize star table
	if (QUEID == 0)
	{
		ifstream startable(".\\Config\\ProjectApollo\\RTCC\\Star Table.txt");
		std::string line;
		VECTOR3 temp;
		unsigned i = 0;

		while (getline(startable, line))
		{
			sscanf(line.c_str(), "%lf %lf %lf", &temp.x, &temp.y, &temp.z);
			temp = mul(SystemParameters.MAT_J2000_BRCS, temp); //Star table in file is in ecliptic, convert to BRCS
			EZJGSTAR[i] = temp;
			i++;
		}
		startable.close();

		//Fill rest of table
		for (; i < 400U; i++)
		{
			EZJGSTAR[i] = _V(1, 0, 0);
		}
	}
	//Acquire and save CSM IMU matrix/optics
	if (QUEID == 1)
	{
		MATRIX3 REFSMMAT;
		double gmt;
		int type;

		//Telemetry (high-speed)
		if (refs == 1)
		{
			if (BZSTLM.CMCRefsPresent == false)
			{
				EMGPRINT("EMSGSUPP", 1);
				return;
			}

			REFSMMAT = BZSTLM.CMC_REFSMMAT;
			gmt = RTCCPresentTimeGMT();
			type = RTCC_REFSMMAT_TYPE_TLM;
		}
		//TBD: Telemetry (low-speed)
		//OST-M
		else if (refs == 3)
		{
			type = RTCC_REFSMMAT_TYPE_OST;

			MATRIX3 SMNB;
			VECTOR3 U_CB, U_CB_apo, U_CBA, U_CBB, U_CBA_apo, U_CBB_apo;

			//Check on optics data being input
			if (EZJGSTTB.SXT_STAR[0] == 0 || EZJGSTTB.SXT_STAR[1] == 0)
			{
				EMGPRINT("EMSGSUPP", 23);
				return;
			}

			for (int i = 0; i < 2; i++)
			{
				//Get star vector in BRCS
				U_CB = EZJGSTAR[EZJGSTTB.SXT_STAR[i] - 1];

				//Get star vector in SM
				SMNB = OrbMech::CALCSMSC(EZJGSTTB.Att[i]);
				U_CB_apo = tmul(SMNB, OrbMech::SXTNB(EZJGSTTB.SXT_TRN_INP[i], EZJGSTTB.SXT_SFT_INP[i]));

				if (i == 0)
				{
					U_CBA = U_CB;
					U_CBA_apo = U_CB_apo;
				}
				else
				{
					U_CBB = U_CB;
					U_CBB_apo = U_CB_apo;
				}
			}

			//Error check for vectors being too close to each other
			const double eps = 0.01*RAD;

			double arc1, arc2;

			arc1 = acos(dotp(U_CBA, U_CBB));
			arc2 = acos(dotp(U_CBA_apo, U_CBB_apo));

			if (arc1 < eps || arc2 < eps)
			{
				EMGPRINT("EMSGSUPP", 39);
				return;
			}

			REFSMMAT = OrbMech::AXISGEN(U_CBA_apo, U_CBB_apo, U_CBA, U_CBB);
			gmt = RTCCPresentTimeGMT();

			//Star arcs
			RTCCONLINEMON.DoubleBuffer[0] = arc1*DEG;
			RTCCONLINEMON.DoubleBuffer[1] = arc2*DEG;

			EMGPRINT("EMSGSUPP", 37);
		}
		//TBD: OST-T
		//DMT
		else if (refs == 5)
		{
			if (refs2 < 0)
			{
				EMGPRINT("EMSGSUPP", 1);
				return;
			}
			if (refs2 == 100)
			{
				//Desired REFSMMAT
				if (man < 0 || PZMPTCSM.ManeuverNum < man)
				{
					EMGPRINT("EMSGSUPP", 21);
					return;
				}

				MPTManeuver *mpt = &PZMPTCSM.mantable[man - 1];

				REFSMMAT = PIDREF(mpt->A_T, mpt->R_BI, mpt->V_BI, mpt->P_G, mpt->Y_G, headsup);
			}
			else
			{
				REFSMMATData data;
				data = EZJGMTX1.data[refs2 - 1];
				if (data.ID <= 0)
				{
					EMGPRINT("EMSGSUPP", 1);
					return;
				}
				REFSMMAT = data.REFSMMAT;
			}
			type = RTCC_REFSMMAT_TYPE_DMT;
			gmt = RTCCPresentTimeGMT();
		}
		//DOD and DOS
		else if (refs == 6)
		{
			if (refs2 == 1)
			{
				if (RZRFDP.data[2].Indicator != 0)
				{
					EMGPRINT("EMSGSUPP", 22);
					return;
				}
				REFSMMAT = RZRFDP.data[2].REFSMMAT;
				gmt = GMTfromGET(RZRFDP.data[2].GETI);
			}
			else if (refs2 == 2)
			{
				//TBD: Spacecraft setting matrix
				EMGPRINT("EMSGSUPP", 22);
				return;
			}
			else if (refs2 == 3)
			{
				if (PZREAP.RTEDTable[0].RTEDCode == "")
				{
					EMGPRINT("EMSGSUPP", 22);
					return;
				}
				REFSMMAT = PZREAP.RTEDTable[0].REFSMMAT;
				gmt = PZREAP.RTEDTable[0].GMTI;
			}
			else if (refs2 == 4)
			{
				if (PZREAP.RTEDTable[1].RTEDCode == "")
				{
					EMGPRINT("EMSGSUPP", 22);
					return;
				}
				REFSMMAT = PZREAP.RTEDTable[1].REFSMMAT;
				gmt = PZREAP.RTEDTable[1].GMTI;
			}
			else
			{
				return;
			}
			type = RTCC_REFSMMAT_TYPE_DOD;
		}
		else
		{
			EMGPRINT("EMSGSUPP", 1);
			return;
		}
		EMGSTSTM(1, REFSMMAT, type, gmt);
	}
}

void RTCC::EMSLSUPP(int QUEID, int refs, int refs2, unsigned man, bool headsup)
{
	//Acquire and save LEM IMU matrix/optics
	if (QUEID == 1)
	{
		//Telemetry (high-speed)
		if (refs == 1)
		{
			if (BZSTLM.LGCRefsPresent == false)
			{
				EMGPRINT("EMSLSUPP", 1);
				return;
			}
			EMGSTSTM(RTCC_MPT_LM, BZSTLM.LGC_REFSMMAT, RTCC_REFSMMAT_TYPE_TLM, RTCCPresentTimeGMT());
		}
		//OST-M
		else if (refs == 4)
		{
			if (EZJGSTBL.REFSMMATValid == false)
			{
				EMGPRINT("EMSLSUPP", 1);
				return;
			}
			EMGSTSTM(RTCC_MPT_LM, EZJGSTBL.StoredREFSMMAT, RTCC_REFSMMAT_TYPE_OST, RTCCPresentTimeGMT());
		}
		//DMT
		else if (refs == 5)
		{
			if (refs2 < 0)
			{
				EMGPRINT("EMSLSUPP", 1);
				return;
			}
			MATRIX3 REFSMMAT;
			if (refs2 == 100)
			{
				//Desired REFSMMAT
				if (man < 0 || PZMPTLEM.ManeuverNum < man)
				{
					EMGPRINT("EMSLSUPP", 21);
					return;
				}

				MPTManeuver *mpt = &PZMPTLEM.mantable[man - 1];

				REFSMMAT = PIDREF(mpt->A_T, mpt->R_BI, mpt->V_BI, mpt->P_G, mpt->Y_G, headsup);
			}
			else
			{
				REFSMMATData data;
				data = EZJGMTX3.data[refs2 - 1];
				if (data.ID <= 0)
				{
					EMGPRINT("EMSLSUPP", 1);
					return;
				}
				REFSMMAT = data.REFSMMAT;
			}
			EMGSTSTM(3, REFSMMAT, RTCC_REFSMMAT_TYPE_DMT, RTCCPresentTimeGMT());
		}
		//Lunar Launch Desired
		else if (refs == 15)
		{
			if (EZJGLSAD.LLDRefsPresent == false)
			{
				EMGPRINT("EMSLSUPP", 1);
				return;
			}
			EMGSTSTM(3, EZJGLSAD.LLD_REFSMMAT, RTCC_REFSMMAT_TYPE_LLD, RTCCPresentTimeGMT());
		}
	}
	//Calculate DOK REFSMMAT
	else if (QUEID == 3)
	{
		if (EZJGSTBL.REF1 <= 0)
		{
			return;
		}

		DockAlignOpt opt;
		REFSMMATData refdata;

		//Get CSM REFSMMAT data
		if (EZJGSTBL.REFSUSED <= 0)
		{
			return;
		}
		refdata = EZJGMTX1.data[EZJGSTBL.REFSUSED - 1];
		if (refdata.ID == 0)
		{
			//ERROR: REFSMMAT NOT AVAILABLE
			EMGPRINT("EMSLSUPP", 1);
			return;
		}

		opt.CSM_REFSMMAT = refdata.REFSMMAT;

		if (opt.type != 1)
		{
			//Get LM REFSMMAT data
			refdata = EZJGMTX3.data[EZJGSTBL.REF1 - 1];
			if (refdata.ID == 0)
			{
				//ERROR: REFSMMAT NOT AVAILABLE
				EMGPRINT("EMSLSUPP", 1);
				return;
			}
			opt.LM_REFSMMAT = refdata.REFSMMAT;
		}

		opt.DockingAngle = EZJGSTBL.DKAN;
		opt.CSMAngles = EZJGSTBL.CSM_ATT;
		opt.LMAngles = EZJGSTBL.LM_ATT;
		opt.type = refs;

		DockingAlignmentProcessor(opt);

		if (opt.type == 1)
		{
			//Save as DOK REFSMMAT
			EMGSTSTM(RTCC_MPT_LM, opt.LM_REFSMMAT, RTCC_REFSMMAT_TYPE_DOK, RTCCPresentTimeGMT());

			//Display on LOST
			EZJGSTBL.REF1 = RTCC_REFSMMAT_TYPE_DOK;
		}
		else if (opt.type == 2)
		{
			EZJGSTBL.LM_ATT = opt.LMAngles;
		}
		else
		{
			EZJGSTBL.CSM_ATT = opt.CSMAngles;
		}

		EMDGLMST();
	}
}

bool RTCC::EMMGSTCK(VECTOR3 u_star, VECTOR3 R, int body, VECTOR3 R_EM, VECTOR3 R_ES)
{
	//Function returns true, if star is occulted

	//Vectors from bodies to spacecraft
	VECTOR3 R_EV, R_MV, R_SV;
	if (body == BODY_EARTH)
	{
		R_EV = R;
		R_MV = -R_EM + R;
		R_SV = -R_ES + R;
	}
	else
	{
		R_EV = R_EM + R;
		R_MV = R;
		R_SV = -R_ES + R_EM + R;
	}

	//Earth occulation
	if (OrbMech::isnotocculted(u_star, R_EV, OrbMech::R_Earth, 0.0) == false) return true;
	//Moon occultation
	if (OrbMech::isnotocculted(u_star, R_MV, OrbMech::R_Moon, 0.0) == false) return true;
	//Sun occultation
	if (OrbMech::isnotocculted(u_star, R_SV, OrbMech::R_Sun, 0.0) == false) return true;

	return false;
}

void RTCC::EMMGSTMP()
{
	int err = 0;

	//Boresight/Scanning Telescope Data
	if (EZGSTMED.MTX1 > 0)
	{
		//Null stars
		unsigned i;

		for (i = 0;i < 2;i++)
		{
			EZJGSTTB.BS_S[i] = 0;
			EZJGSTTB.BS_DEC[i] = 0.0;
			EZJGSTTB.BS_RTASC[i] = 0.0;
			EZJGSTTB.BS_SPA[i] = 0.0;
			EZJGSTTB.BS_SXP[i] = 0.0;
		}

		//Get REFSMMAT
		REFSMMATData refs = EZJGMTX1.data[EZGSTMED.MTX1 - 1];
		if (refs.ID <= 0)
		{
			err = 4;
			EZJGSTTB.IRA = "ZZZZZZ";
			EMDGSUPP(err);
			return;
		}

		char Buff[7];

		FormatREFSMMATCode(EZGSTMED.MTX1, refs.ID, Buff);
		EZJGSTTB.IRA.assign(Buff);

		//Get state vector at input time
		ELVCTRInputTable intab;
		ELVCTROutputTable2 outtab;
		EphemerisData sv;

		intab.L = RTCC_MPT_CSM;
		intab.GMT = EZGSTMED.GMT;
		ELVCTR(intab, outtab);
		if (outtab.ErrorCode > 2)
		{
			err = 2;
			EMDGSUPP(err);
			return;
		}

		//Get ephemerides
		VECTOR3 R_EM, V_EM, R_ES;
		if (PLEFEM(1, EZGSTMED.GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL))
		{
			err = 2;
			EMDGSUPP(err);
			return;
		}

		MATRIX3 M_SMNB, RM;
		VECTOR3 u_SM, u_NB;
		double XSC, YSC, ZSC, SPA, SPX;

		sv = RotateSVToSOI(outtab.SV);
		RM = refs.REFSMMAT;
		M_SMNB = OrbMech::CALCSMSC(_V(EZJGSTTB.Att[0].z, EZJGSTTB.Att[0].x, EZJGSTTB.Att[0].y));

		VECTOR3 u;
		int num = 0;
		unsigned endstar;

		if (EZGSTMED.StartingStar <= 400U)
		{
			i = EZGSTMED.StartingStar - 1;
		}
		else
		{
			i = 0;
		}
		endstar = i;

		do
		{
			u = EZJGSTAR[i];

			//Occultation check
			if (i <= 396U)
			{
				if (EMMGSTCK(u, sv.R, sv.RBI, R_EM, R_ES))
				{
					i++;
					continue;
				}
			}

			u_SM = mul(RM, u);
			u_NB = mul(M_SMNB, u_SM);

			XSC = u_NB.x;
			YSC = u_NB.y;
			ZSC = u_NB.z;

			SPA = atan2(ZSC, XSC);
			SPX = asin(YSC);

			if (abs(SPX) <= 5.0*RAD)
			{
				if (SPA <= 31.5*RAD && SPA >= -10.0*RAD)
				{
					//Found one
					EZJGSTTB.BS_S[num] = i + 1;
					EZJGSTTB.BS_DEC[num] = acos(u.z);
					EZJGSTTB.BS_RTASC[num] = atan2(u.y, u.x);
					if (EZJGSTTB.BS_RTASC[num] < 0)
					{
						EZJGSTTB.BS_RTASC[num] += PI2;
					}
					EZJGSTTB.BS_SPA[num] = SPA;
					EZJGSTTB.BS_SXP[num] = SPX;
					num++;
					if (num >= 2)
					{
						break;
					}
				}
			}
			i++;
			if (i >= 400U)
			{
				i = 0;
			}
		} while (i != endstar);

		if (num < 2)
		{
			err = 3;
		}

		//TBD: Telescope
	}
	//Sextant Data
	if (EZGSTMED.MTX2 > 0)
	{
		REFSMMATData refs = EZJGMTX1.data[EZGSTMED.MTX2 - 1];
		if (refs.ID <= 0)
		{
			err = 4;
			EMDGSUPP(err);
			return;
		}
		//Null stars
		unsigned i;

		for (i = 0;i < 2;i++)
		{
			EZJGSTTB.SXT_STAR[0] = 0;
			EZJGSTTB.SXT_SFT_INP[i] = 0.0;
			EZJGSTTB.SXT_SFT_RTCC[i] = 0.0;
			EZJGSTTB.SXT_TRN_INP[i] = 0.0;
			EZJGSTTB.SXT_TRN_RTCC[i] = 0.0;
		}

		//Get state vector at input time
		ELVCTRInputTable intab;
		ELVCTROutputTable2 outtab;
		EphemerisData sv;

		intab.L = RTCC_MPT_CSM;
		intab.GMT = EZGSTMED.GMT;
		ELVCTR(intab, outtab);
		if (outtab.ErrorCode > 2)
		{
			err = 2;
			EMDGSUPP(err);
			return;
		}

		//Get ephemerides
		VECTOR3 R_EM, V_EM, R_ES;
		if (PLEFEM(1, EZGSTMED.GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL))
		{
			err = 2;
			EMDGSUPP(err);
			return;
		}

		MATRIX3 SMNB, RM, SBNB;
		VECTOR3 S_SM;
		double TA, SA, a;

		sv = RotateSVToSOI(outtab.SV);
		RM = refs.REFSMMAT;
		SMNB = OrbMech::CALCSMSC(_V(EZJGSTTB.Att[0].z, EZJGSTTB.Att[0].x, EZJGSTTB.Att[0].y));
		a = -0.5676353234;
		SBNB = _M(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));

		VECTOR3 u;
		int num = 0;
		unsigned endstar;

		if (EZGSTMED.StartingStar <= 400U)
		{
			i = EZGSTMED.StartingStar - 1;
		}
		else
		{
			i = 0;
		}
		endstar = i;

		do
		{
			u = EZJGSTAR[i];

			//Occultation check (but not for stars 398-400)
			if (i > 396U || !EMMGSTCK(u, sv.R, sv.RBI, R_EM, R_ES))
			{
				//Not occulated, check trunnion angle
				S_SM = mul(RM, u);
				OrbMech::CALCSXA(SMNB, S_SM, TA, SA);
				//Trunnion angle below 50�?
				if (TA <= 50.0*RAD)
				{
					//Found one
					EZJGSTTB.SXT_STAR[num] = i + 1;
					EZJGSTTB.SXT_SFT_RTCC[num] = SA;
					EZJGSTTB.SXT_TRN_RTCC[num] = TA;
					num++;
					//Found 2 stars, get out of here
					if (num >= 2)
					{
						break;
					}
				}
			}

			//Try next one
			i++;
			//Are we back at the start?
			if (i >= 400U)
			{
				//Continue from star 0 on
				i = 0;
			}
			//Have we reached the end?
		} while (i != endstar);
		//If it didn't find two stars, return error
		if (num < 2)
		{
			err = 3;
		}
	}
	//Matrix display
	if (EZGSTMED.MTX3 > 0)
	{
		REFSMMATData refs = EZJGMTX1.data[EZGSTMED.MTX3 - 1];
		if (refs.ID <= 0)
		{
			err = 4;
			EZJGSTTB.REFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
			EZJGSTTB.MAT = "";
		}
		else
		{
			char Buff[7];

			FormatREFSMMATCode(EZGSTMED.MTX3, refs.ID, Buff);
			EZJGSTTB.MAT.assign(Buff);
			EZJGSTTB.REFSMMAT = refs.REFSMMAT;
		}
	}
	//Unit vector
	if (EZGSTMED.G14_Star > 0)
	{
		EZJGSTTB.Landmark_GET = 0.0;
		VECTOR3 u = EZJGSTAR[EZGSTMED.G14_Star - 1];
		char buff[4];
		sprintf_s(buff, "%03d", EZGSTMED.G14_Star);
		EZJGSTTB.Landmark_SC.assign(buff);
		EZJGSTTB.Landmark_LOS = u;
		OrbMech::latlong_from_r(u, EZJGSTTB.Landmark_DEC, EZJGSTTB.Landmark_RA);
		if (EZJGSTTB.Landmark_RA < 0)
		{
			EZJGSTTB.Landmark_RA += PI2;
		}
	}
	//Landmark unit vector
	if (EZGSTMED.G14_RB > 0)
	{
		EZJGSTTB.Landmark_SC = "";
		EZJGSTTB.Landmark_LOS = _V(0, 0, 0);
		EZJGSTTB.Landmark_DEC = 0.0;
		EZJGSTTB.Landmark_RA = 0.0;

		VECTOR3 R_EM, V_EM, R_ES, R_BL, R_BV, R_VL, R_BL_equ;

		if (PLEFEM(1, EZGSTMED.G14_GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL))
		{
			err = 2;
		}
		else
		{
			if (EZGSTMED.G14_RB == 1)
			{
				//Point to center of sun
				R_BL = _V(0, 0, 0);
			}
			else
			{
				double r;
				int in, out;

				//Calculate ECT/MCT position vector of landmark
				if (EZGSTMED.G14_RB == 2)
				{
					r = EZGSTMED.G14_height + BZLAND.rad[RTCC_LMPOS_BEST];
					in = 3;
					out = 2;
					R_BL_equ = OrbMech::r_from_latlong(EZGSTMED.G14_lat, EZGSTMED.G14_lng, r);
				}
				else
				{
					r = EZGSTMED.G14_height + OrbMech::R_Earth;
					in = 1;
					out = 0;
					R_BL_equ = OrbMech::r_from_latlong(EZGSTMED.G14_lat, EZGSTMED.G14_lng + OrbMech::w_Earth*EZGSTMED.G14_GMT, r);
				}
				//Convert to ECI or MCI
				if (ELVCNV(R_BL_equ, EZGSTMED.G14_GMT, 1, in, out, R_BL))
				{
					err = 2;
				}
			}
			ELVCTRInputTable intab;
			ELVCTROutputTable2 outtab;

			intab.GMT = EZGSTMED.G14_GMT;
			intab.L = EZGSTMED.G14_Vehicle;

			ELVCTR(intab, outtab);
			if (outtab.ErrorCode)
			{
				err = 2;
			}
			else
			{
				if (DetermineSVBody(outtab.SV) == BODY_EARTH)
				{
					//Sun
					if (EZGSTMED.G14_RB == 1)
					{
						R_BV = outtab.SV.R - R_ES;
					}
					//Moon
					else if (EZGSTMED.G14_RB == 2)
					{
						R_BV = (outtab.SV.R) - R_EM;
					}
					//Earth
					else
					{
						R_BV = outtab.SV.R;
					}
				}
				else
				{
					//Sun
					if (EZGSTMED.G14_RB == 1)
					{
						R_BV = (outtab.SV.R + R_EM) - R_ES;
					}
					//Moon
					else if (EZGSTMED.G14_RB == 2)
					{
						R_BV = outtab.SV.R;
					}
					//Earth
					else
					{
						R_BV = outtab.SV.R + R_EM;
					}
				}
				R_VL = R_BL - R_BV;

				VECTOR3 u = unit(R_VL);
				EZJGSTTB.Landmark_SC = "";
				if (EZGSTMED.G14_Vehicle == 1)
				{
					EZJGSTTB.Landmark_SC += "C";
				}
				else
				{
					EZJGSTTB.Landmark_SC += "L";
				}
				//TBD
				EZJGSTTB.Landmark_SC += "1";
				if (EZGSTMED.G14_RB == 1)
				{
					EZJGSTTB.Landmark_SC += "S";
				}
				else if (EZGSTMED.G14_RB == 2)
				{
					EZJGSTTB.Landmark_SC += "M";
				}
				else
				{
					EZJGSTTB.Landmark_SC += "E";
				}
				EZJGSTTB.Landmark_GET = GETfromGMT(EZGSTMED.G14_GMT);
				EZJGSTTB.Landmark_LOS = u;
				OrbMech::latlong_from_r(u, EZJGSTTB.Landmark_DEC, EZJGSTTB.Landmark_RA);
				if (EZJGSTTB.Landmark_RA < 0)
				{
					EZJGSTTB.Landmark_RA += PI2;
				}
			}
		}
	}

	EMDGSUPP(err);
}

void RTCC::EMDGSUPP(int err)
{
	GOSTDisplayBuffer.err = "";

	if (err)
	{
		switch (err)
		{
		case 1:
			GOSTDisplayBuffer.err = "NO OPTICS THIS VEHICLE";
			break;
		case 2:
			GOSTDisplayBuffer.err = "EPHEMERIS NOT AVAILABLE";
			break;
		case 3:
			GOSTDisplayBuffer.err = "TWO STARS NOT AVAILABLE";
			break;
		case 4:
			GOSTDisplayBuffer.err = "MATRIX NOT AVAILABLE";
			break;
		case 5:
			GOSTDisplayBuffer.err = "MED INPUT REQUIRED";
			break;
		case 6:
			GOSTDisplayBuffer.err = "ATTITUDES NOT AVAILABLE";
			break;
		case 7:
			GOSTDisplayBuffer.err = "OPTICS NOT AVAILABLE";
			break;
		}
	}

	GOSTDisplayBuffer.data.MAT = EZJGSTTB.MAT;
	GOSTDisplayBuffer.data.REFSMMAT = EZJGSTTB.REFSMMAT;
	GOSTDisplayBuffer.data.Landmark_DEC = EZJGSTTB.Landmark_DEC * DEG*3600.0;
	GOSTDisplayBuffer.data.Landmark_RA = EZJGSTTB.Landmark_RA * DEG*3600.0;
	GOSTDisplayBuffer.data.Landmark_SC = EZJGSTTB.Landmark_SC;
	GOSTDisplayBuffer.data.Landmark_GET = EZJGSTTB.Landmark_GET;
	GOSTDisplayBuffer.data.Landmark_LOS = EZJGSTTB.Landmark_LOS;

	for (int i = 0;i < 2;i++)
	{
		GOSTDisplayBuffer.data.Att[i] = EZJGSTTB.Att[i] * DEG;
		GOSTDisplayBuffer.data.BS_S[i] = EZJGSTTB.BS_S[i];
		GOSTDisplayBuffer.data.BS_DEC[i] = EZJGSTTB.BS_DEC[i] * DEG*3600.0;
		GOSTDisplayBuffer.data.BS_RTASC[i] = EZJGSTTB.BS_RTASC[i] * DEG*3600.0;
		GOSTDisplayBuffer.data.BS_SPA[i] = EZJGSTTB.BS_SPA[i] * DEG;
		GOSTDisplayBuffer.data.BS_SXP[i] = EZJGSTTB.BS_SXP[i] * DEG;
		GOSTDisplayBuffer.data.SXT_STAR[i] = EZJGSTTB.SXT_STAR[i];
		GOSTDisplayBuffer.data.SXT_SFT_INP[i] = EZJGSTTB.SXT_SFT_INP[i] * DEG;
		GOSTDisplayBuffer.data.SXT_SFT_RTCC[i] = EZJGSTTB.SXT_SFT_RTCC[i] * DEG;
		GOSTDisplayBuffer.data.SXT_TRN_INP[i] = EZJGSTTB.SXT_TRN_INP[i] * DEG;
		GOSTDisplayBuffer.data.SXT_TRN_RTCC[i] = EZJGSTTB.SXT_TRN_RTCC[i] * DEG;
	}
}

void RTCC::EMMGLMST(int mode)
{
	EZJGSTBL.showRealign = false;

	if (mode == 1)
	{
		//Realign angles

		MATRIX3 REFSMMAT1, REFSMMAT2;

		if (EZJGSTBL.REF1 > 0)
		{
			REFSMMATData refs = EZJGMTX3.data[EZJGSTBL.REF1 - 1];
			if (refs.ID > 0)
			{
				REFSMMAT1 = refs.REFSMMAT;
			}
			else
			{
				EMDGLMST();
				return;
			}
		}
		else
		{
			EMDGLMST();
			return;
		}

		if (EZJGSTBL.REF2 > 0)
		{
			REFSMMATData refs = EZJGMTX3.data[EZJGSTBL.REF2 - 1];
			if (refs.ID > 0)
			{
				REFSMMAT2 = refs.REFSMMAT;
			}
			else
			{
				EMDGLMST();
				return;
			}
		}
		else
		{
			EMDGLMST();
			return;
		}

		VECTOR3 X_P, Y_P, Z_P, X_B, Y_B, Z_B;

		X_P = _V(REFSMMAT1.m11, REFSMMAT1.m12, REFSMMAT1.m13);
		Y_P = _V(REFSMMAT1.m21, REFSMMAT1.m22, REFSMMAT1.m23);
		Z_P = _V(REFSMMAT1.m31, REFSMMAT1.m32, REFSMMAT1.m33);
		X_B = _V(REFSMMAT2.m11, REFSMMAT2.m12, REFSMMAT2.m13);
		Y_B = _V(REFSMMAT2.m21, REFSMMAT2.m22, REFSMMAT2.m23);
		Z_B = _V(REFSMMAT2.m31, REFSMMAT2.m32, REFSMMAT2.m33);

		EZJGSTBL.showRealign = true;
		EMGLMRAT(X_P, Y_P, Z_P, X_B, Y_B, Z_B, EZJGSTBL.GIMB_ANG.y, EZJGSTBL.GIMB_ANG.x, EZJGSTBL.GIMB_ANG.z, EZJGSTBL.FDAI_ANG.y, EZJGSTBL.FDAI_ANG.z, EZJGSTBL.FDAI_ANG.x);
	}
	else if (mode == 2)
	{
		//DOK
		//TBD
	}
	else if (mode == 4)
	{
		//AGS

		//Star 1 is at center
		EZJGSTBL.A1_1 = EZJGSTBL.A2_1 = 0.0;
		//Has to be same detent
		EZJGSTBL.D2 = EZJGSTBL.D1;
		EZJGSTBL.StoredREFSMMAT = OrbMech::AGSStarAlignment(EZJGSTAR, EZJGSTBL.Att1, EZJGSTBL.Att2, EZJGSTBL.star1, EZJGSTBL.star2, EZJGSTBL.L2, EZJGSTBL.D1, EZJGSTBL.A1_2);
		EZJGSTBL.REFSMMATValid = true;
		//Save as OST (would actually be a second manual step)
		GMGMED("G21,LEM,OST-M;");
		//Show as MAT1
		EZJGSTBL.REF1 = RTCC_REFSMMAT_TYPE_OST;
		EZJGSTBL.REFSUSED = EZJGSTBL.REF1;
	}
	else if (mode == 7)
	{
		//CHK
		MATRIX3 REFSMMAT;
		double GMT;

		//Reset some data on the display
		EZJGSTBL.star1 = EZJGSTBL.star2 = 0;
		EZJGSTBL.A1_1 = EZJGSTBL.A1_2 = EZJGSTBL.A2_1 = EZJGSTBL.A2_2 = 0.0;

		//Get GMT of alignment
		EZJGSTBL.GETHORIZ = EZGSTMED.G20_HORIZGET;
		GMT = GMTfromGET(EZJGSTBL.GETHORIZ);

		ELVCTRInputTable in;
		ELVCTROutputTable2 out;

		in.EphemerisType = 0;
		in.GMT = GMT;
		in.L = RTCC_MPT_LM;

		ELVCTR(in, out);

		if (out.ErrorCode > 2)
		{
			EZJGSTBL.GETHORIZ = -1.0;
			EMDGLMST();
			return;
		}

		EphemerisData2 sv = out.SV;
		EphemerisData sv2 = RotateSVToSOI(sv);

		if (EZJGSTBL.REF1 > 0)
		{
			REFSMMATData refs = EZJGMTX3.data[EZJGSTBL.REF1 - 1];
			if (refs.ID > 0)
			{
				REFSMMAT = refs.REFSMMAT;
			}
			else
			{
				EMDGLMST();
				return;
			}
		}
		else
		{
			EMDGLMST();
			return;
		}

		EZJGSTBL.REFSUSED = EZJGSTBL.REF1;

		VECTOR3 R_ES, R_EM, V_EM;

		if (PLEFEM(1, sv2.GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL))
		{
			//Error
			EMDGLMST();
			return;
		}

		unsigned num = 0;
		bool found;

		//AOT
		double AOT_AZ, AOT_EL;

		//Get azimuth and elevation angles of AOT detent
		AOT_AZ = SystemParameters.MDGTCD[EZGSTMED.G20_AOT_Detent - 1];
		AOT_EL = SystemParameters.MDGETA[EZGSTMED.G20_AOT_Detent - 1];

		for (int i = 0; i < 2; i++)
		{
			found = false;

			while (num < 400U)
			{
				//Occultation check (but not for stars 398-400)
				if (num <= 396U)
				{
					if (EMMGSTCK(EZJGSTAR[num], sv2.R, sv2.RBI, R_EM, R_ES))
					{
						num++;
						continue;
					}
				}
				found = OrbMech::AOTcheckstar(EZJGSTAR[num], REFSMMAT, EZJGSTBL.StoredAttMED, AOT_AZ, AOT_EL);
				if (found) break;
				num++;
			}

			if (found)
			{
				if (i == 0)
				{
					EZJGSTBL.star1 = num + 1;
					OrbMech::AOTStarAcquisition(EZJGSTAR[num], REFSMMAT, EZJGSTBL.StoredAttMED, AOT_AZ, AOT_EL, EZJGSTBL.A1_1, EZJGSTBL.A2_1);
					num++;
				}
				else
				{
					EZJGSTBL.star2 = num + 1;
					OrbMech::AOTStarAcquisition(EZJGSTAR[num], REFSMMAT, EZJGSTBL.StoredAttMED, AOT_AZ, AOT_EL, EZJGSTBL.A1_2, EZJGSTBL.A2_2);
				}
			}
			else
			{
				if (i == 0)
				{
					EZJGSTBL.star1 = 0;
				}
				else
				{
					EZJGSTBL.star2 = 0;
				}
				break;
			}
		}

		//Set displayed detents and attitudes for star sightings to input
		EZJGSTBL.D1 = EZJGSTBL.D2 = EZGSTMED.G20_AOT_Detent;
		//And use +Y
		EZJGSTBL.L1 = EZJGSTBL.L2 = 2;
		EZJGSTBL.Att1 = EZJGSTBL.Att2 = EZJGSTBL.StoredAttMED;

		//COAS
		double EL, SXP;
		num = 0;
		found = false;

		for (int i = 0; i < 2; i++)
		{
			found = false;

			while (num < 400U)
			{
				//Occultation check (but not for stars 398-400)
				if (num <= 396U)
				{
					if (EMMGSTCK(EZJGSTAR[num], sv2.R, sv2.RBI, R_EM, R_ES))
					{
						num++;
						continue;
					}
				}
				found = OrbMech::LMCOASCheckStar(EZJGSTAR[num], REFSMMAT, EZJGSTBL.StoredAttMED, EZGSTMED.G20_COAS_Axis, EL, SXP);
				if (found) break;
				num++;
			}

			if (found)
			{
				if (i == 0)
				{
					EZJGSTBL.COAS_star1 = num;
					EZJGSTBL.COAS_EL1 = EL;
					EZJGSTBL.COAS_SXP1 = SXP;
					num++;
				}
				else
				{
					EZJGSTBL.COAS_star2 = num;
					EZJGSTBL.COAS_EL2 = EL;
					EZJGSTBL.COAS_SXP2 = SXP;
				}
			}
			else
			{
				if (i == 0)
				{
					EZJGSTBL.COAS_star1 = 0;
				}
				else
				{
					EZJGSTBL.COAS_star2 = 0;
				}
			}
		}

		EZJGSTBL.COAS_AXIS = EZGSTMED.G20_COAS_Axis;

		//TBD: Horizon
	}

	EMDGLMST();
}

void RTCC::EMDGLMST()
{
	char Buffer[128];
	LOSTDisplayBuffer.resize(81);

	//Item 1: VEH
	LOSTDisplayBuffer[0] = MHGVNM.tab[1];

	//Item 2: MODE
	switch (EZJGSTBL.MODE)
	{
	case 2: LOSTDisplayBuffer[1] = "DOK"; break;
	case 3: LOSTDisplayBuffer[1] = "FLT"; break;
	case 4: LOSTDisplayBuffer[1] = "AGS"; break;
	case 5: LOSTDisplayBuffer[1] = "LUN"; break;
	case 6: LOSTDisplayBuffer[1] = "MAT"; break;
	case 7: LOSTDisplayBuffer[1] = "CHK"; break;
	default: LOSTDisplayBuffer[1] = ""; break;
	}

	//Item 3: DKAN
	if (EZJGSTBL.MODE == 2)
	{
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.DKAN);
		LOSTDisplayBuffer[2].assign(Buffer);
	}
	else
	{
		LOSTDisplayBuffer[2] = "";
	}

	//Item 4-6: LM Docking Attitude
	if (EZJGSTBL.MODE == 2)
	{
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.LM_ATT.x*DEG);
		LOSTDisplayBuffer[3].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.LM_ATT.y*DEG);
		LOSTDisplayBuffer[4].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.LM_ATT.z*DEG);
		LOSTDisplayBuffer[5].assign(Buffer);
	}
	else
	{
		LOSTDisplayBuffer[3] = "";
		LOSTDisplayBuffer[4] = "";
		LOSTDisplayBuffer[5] = "";
	}


	//Item 7-9: LM Docking Attitude
	if (EZJGSTBL.MODE == 2)
	{
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.CSM_ATT.x*DEG);
		LOSTDisplayBuffer[6].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.CSM_ATT.y*DEG);
		LOSTDisplayBuffer[7].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.CSM_ATT.z*DEG);
		LOSTDisplayBuffer[8].assign(Buffer);
	}
	else
	{
		LOSTDisplayBuffer[6] = "";
		LOSTDisplayBuffer[7] = "";
		LOSTDisplayBuffer[8] = "";
	}

	//Item 10: Matrix used
	if (EZJGSTBL.MODE == 2 || EZJGSTBL.MODE == 4 || EZJGSTBL.MODE == 7)
	{
		if (EZJGSTBL.REFSUSED > 0)
		{
			REFSMMATData refs;
			
			if (EZJGSTBL.MODE == 2)
			{
				refs = EZJGMTX1.data[EZJGSTBL.REFSUSED - 1];
			}
			else
			{
				refs = EZJGMTX3.data[EZJGSTBL.REFSUSED - 1];
			}

			if (refs.ID > 0)
			{
				FormatREFSMMATCode(EZJGSTBL.REFSUSED, refs.ID, Buffer);
				LOSTDisplayBuffer[9].assign(Buffer);
			}
			else
			{
				LOSTDisplayBuffer[9] = "N/A";
			}
		}
		else
		{
			LOSTDisplayBuffer[9] = "N/A";
		}
	}
	else
	{
		LOSTDisplayBuffer[9] = "";
	}

	//Item 11: GETHORIZ
	if (EZJGSTBL.MODE == 7)
	{
		if (EZJGSTBL.GETHORIZ < 0.0)
		{
			LOSTDisplayBuffer[10] = "N/A";
		}
		else
		{
			OrbMech::format_time_HHMMSS(Buffer, EZJGSTBL.GETHORIZ);
			LOSTDisplayBuffer[10].assign(Buffer);
		}
	}
	else
	{
		LOSTDisplayBuffer[10] = "";
	}

	//Item 12: OGA
	if (EZJGSTBL.MODE == 7)
	{
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.OGA*DEG);
		LOSTDisplayBuffer[11].assign(Buffer);
	}
	else
	{
		LOSTDisplayBuffer[11] = "";
	}

	//Item 13-14: Stars 1-2
	if (EZJGSTBL.MODE == 3 || EZJGSTBL.MODE == 4 || EZJGSTBL.MODE == 5 || EZJGSTBL.MODE == 7)
	{
		if (EZJGSTBL.star1 > 0)
		{
			sprintf_s(Buffer, "%03d", EZJGSTBL.star1);
			LOSTDisplayBuffer[12].assign(Buffer);
		}
		else
		{
			LOSTDisplayBuffer[12] = "N/A";
		}
		if (EZJGSTBL.star2 > 0)
		{
			sprintf_s(Buffer, "%03d", EZJGSTBL.star2);
			LOSTDisplayBuffer[13].assign(Buffer);
		}
		else
		{
			LOSTDisplayBuffer[13] = "N/A";
		}
	}
	else
	{
		LOSTDisplayBuffer[12] = "";
		LOSTDisplayBuffer[13] = "";
	}

	//Item 15-18: RA and DEC
	if (EZJGSTBL.MODE == 7 && EZJGSTBL.star1 > 0)
	{
		double dec, ra;
		OrbMech::latlong_from_r(EZJGSTAR[EZJGSTBL.star1 - 1], dec, ra);
		sprintf_s(Buffer, "%06.2lf", ra*DEG);
		LOSTDisplayBuffer[14].assign(Buffer);
		sprintf_s(Buffer, "%+06.2lf", dec*DEG);
		LOSTDisplayBuffer[15].assign(Buffer);
	}
	else
	{
		LOSTDisplayBuffer[14] = LOSTDisplayBuffer[15] = "";
	}

	if (EZJGSTBL.MODE == 7 && EZJGSTBL.star2 > 0)
	{
		double dec, ra;
		OrbMech::latlong_from_r(EZJGSTAR[EZJGSTBL.star2 - 1], dec, ra);
		sprintf_s(Buffer, "%06.2lf", ra*DEG);
		LOSTDisplayBuffer[16].assign(Buffer);
		sprintf_s(Buffer, "%+06.2lf", dec*DEG);
		LOSTDisplayBuffer[17].assign(Buffer);
	}
	else
	{
		LOSTDisplayBuffer[16] = LOSTDisplayBuffer[17] = "";
	}

	//Item 19-24: Att 1 and 2
	if (EZJGSTBL.MODE == 4 || EZJGSTBL.MODE == 5 || EZJGSTBL.MODE == 7)
	{
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.Att1.x*DEG);
		LOSTDisplayBuffer[18].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.Att1.y*DEG);
		LOSTDisplayBuffer[19].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.Att1.z*DEG);
		LOSTDisplayBuffer[20].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.Att2.x*DEG);
		LOSTDisplayBuffer[21].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.Att2.y*DEG);
		LOSTDisplayBuffer[22].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.Att2.z*DEG);
		LOSTDisplayBuffer[23].assign(Buffer);
	}
	else
	{
		LOSTDisplayBuffer[18] = LOSTDisplayBuffer[19] = LOSTDisplayBuffer[20] = "";
		LOSTDisplayBuffer[21] = LOSTDisplayBuffer[22] = LOSTDisplayBuffer[23] = "";
	}

	//Item 25-26: Reticle line
	if (EZJGSTBL.MODE == 5 || EZJGSTBL.MODE == 7)
	{
		switch (EZJGSTBL.L1)
		{
		case 1: LOSTDisplayBuffer[24] = "-X"; break;
		case 2: LOSTDisplayBuffer[24] = "+Y"; break;
		case 3: LOSTDisplayBuffer[24] = "-Y"; break;
		default: LOSTDisplayBuffer[24] = "+X"; break;
		}
	}
	else
	{
		LOSTDisplayBuffer[24] = "";
	}

	if (EZJGSTBL.MODE == 4 || EZJGSTBL.MODE == 5 || EZJGSTBL.MODE == 7)
	{
		switch (EZJGSTBL.L2)
		{
		case 1: LOSTDisplayBuffer[25] = "-X"; break;
		case 2: LOSTDisplayBuffer[25] = "+Y"; break;
		case 3: LOSTDisplayBuffer[25] = "-Y"; break;
		default: LOSTDisplayBuffer[25] = "+X"; break;
		}
	}
	else
	{
		LOSTDisplayBuffer[25] = "";
	}

	//Item 27-28: A1
	//Item 29-30: A2
	if (EZJGSTBL.MODE == 5 || EZJGSTBL.MODE == 7)
	{
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.A1_1*DEG);
		LOSTDisplayBuffer[26].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.A2_1*DEG);
		LOSTDisplayBuffer[28].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.A2_2*DEG);
		LOSTDisplayBuffer[29].assign(Buffer);
	}
	else
	{
		LOSTDisplayBuffer[26] = LOSTDisplayBuffer[28] = LOSTDisplayBuffer[29] = "";
	}

	if (EZJGSTBL.MODE == 4 || EZJGSTBL.MODE == 5 || EZJGSTBL.MODE == 7)
	{
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.A1_2*DEG);
		LOSTDisplayBuffer[27].assign(Buffer);
	}
	else
	{
		LOSTDisplayBuffer[27] = "";
	}

	//Item 31-32: Detent
	if (EZJGSTBL.MODE == 4 || EZJGSTBL.MODE == 5 || EZJGSTBL.MODE == 7)
	{
		sprintf_s(Buffer, "%d", EZJGSTBL.D1);
		LOSTDisplayBuffer[30].assign(Buffer);

		sprintf_s(Buffer, "%d", EZJGSTBL.D2);
		LOSTDisplayBuffer[31].assign(Buffer);
	}
	else
	{
		LOSTDisplayBuffer[30] = LOSTDisplayBuffer[31] = "";
	}

	//Item 33-34: COAS stars
	//Item 35-36: COAS elevation
	//Item 37-38: COAS SXP
	//Item 39: COAS axis
	if (EZJGSTBL.MODE == 7)
	{
		if (EZJGSTBL.COAS_star1 != 0)
		{
			sprintf(Buffer, "%03d", EZJGSTBL.COAS_star1);
			LOSTDisplayBuffer[32].assign(Buffer);
			sprintf(Buffer, "%02.0lf", EZJGSTBL.COAS_EL1*DEG);
			LOSTDisplayBuffer[34].assign(Buffer);
			sprintf(Buffer, "%01.0lf", EZJGSTBL.COAS_SXP1*DEG);
			LOSTDisplayBuffer[36].assign(Buffer);
		}
		else
		{
			LOSTDisplayBuffer[32] = LOSTDisplayBuffer[34] = LOSTDisplayBuffer[36] = "";
		}

		if (EZJGSTBL.COAS_star2 != 0)
		{
			sprintf(Buffer, "%03d", EZJGSTBL.COAS_star2);
			LOSTDisplayBuffer[33].assign(Buffer);
			sprintf(Buffer, "%02.0lf", EZJGSTBL.COAS_EL2*DEG);
			LOSTDisplayBuffer[35].assign(Buffer);
			sprintf(Buffer, "%01.0lf", EZJGSTBL.COAS_SXP2*DEG);
			LOSTDisplayBuffer[37].assign(Buffer);
		}
		else
		{
			LOSTDisplayBuffer[33] = LOSTDisplayBuffer[35] = LOSTDisplayBuffer[37] = "";
		}

		if (EZJGSTBL.COAS_star1 == 0 && EZJGSTBL.COAS_star2 == 0)
		{
			LOSTDisplayBuffer[33] = "NO STARS AVAILABLE";
		}

		if (EZJGSTBL.COAS_AXIS == 1)
		{
			LOSTDisplayBuffer[38] = "X";
		}
		else
		{
			LOSTDisplayBuffer[38] = "Z";
		}
	}
	else
	{
		LOSTDisplayBuffer[32] = LOSTDisplayBuffer[33] = LOSTDisplayBuffer[34] = LOSTDisplayBuffer[35] = LOSTDisplayBuffer[36] = LOSTDisplayBuffer[37] = LOSTDisplayBuffer[38] = "";
	}
	//Item 40-41: Telemetry stars
	//Item 42-44: Telemetry unit vector 1
	//Item 45-47: Telemetry unit vector 2
	//Item 48-50: Realign attitude (IMU)
	//Item 51-53: Realign attitude (FDAI)
	if (EZJGSTBL.showRealign)
	{
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.GIMB_ANG.x*DEG);
		LOSTDisplayBuffer[47].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.GIMB_ANG.y*DEG);
		LOSTDisplayBuffer[48].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.GIMB_ANG.z*DEG);
		LOSTDisplayBuffer[49].assign(Buffer);

		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.FDAI_ANG.x*DEG);
		LOSTDisplayBuffer[50].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.FDAI_ANG.y*DEG);
		LOSTDisplayBuffer[51].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.FDAI_ANG.z*DEG);
		LOSTDisplayBuffer[52].assign(Buffer);
	}
	else
	{
		LOSTDisplayBuffer[47] = LOSTDisplayBuffer[48] = LOSTDisplayBuffer[49] = "";
		LOSTDisplayBuffer[50] = LOSTDisplayBuffer[51] = LOSTDisplayBuffer[52] = "";
	}
	//Item 54-56: Presently stored attitude (TLM)
	//Item 57-59: Presently stored attitude (MED)
	if (EZJGSTBL.MODE == 7)
	{
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.StoredAttMED.x*DEG);
		LOSTDisplayBuffer[56].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.StoredAttMED.y*DEG);
		LOSTDisplayBuffer[57].assign(Buffer);
		sprintf_s(Buffer, "%05.1lf", EZJGSTBL.StoredAttMED.z*DEG);
		LOSTDisplayBuffer[58].assign(Buffer);
	}
	else
	{
		LOSTDisplayBuffer[56] = LOSTDisplayBuffer[57] = LOSTDisplayBuffer[58] = "";
	}
	//Item 60: Matrix 1
	//Item 61: Matrix 1 GET
	//Item 62-70: Matrix 1
	if (EZJGSTBL.REF1 > 0)
	{
		REFSMMATData refs = EZJGMTX3.data[EZJGSTBL.REF1 - 1];
		if (refs.ID > 0)
		{
			FormatREFSMMATCode(EZJGSTBL.REF1, refs.ID, Buffer);
			LOSTDisplayBuffer[59].assign(Buffer);
			OrbMech::format_time_HHMMSS(Buffer, GETfromGMT(refs.GMT));
			LOSTDisplayBuffer[60].assign(Buffer);

			for (unsigned i = 0; i < 9; i++)
			{
				sprintf_s(Buffer, "%+010.7lf", refs.REFSMMAT.data[i]);
				LOSTDisplayBuffer[i + 61].assign(Buffer);
			}
		}
		else
		{
			LOSTDisplayBuffer[59] = "N/A";
			LOSTDisplayBuffer[60] = "";

			for (unsigned i = 0; i < 9; i++)
			{
				LOSTDisplayBuffer[i + 61] = "";
			}
		}
	}
	else
	{
		LOSTDisplayBuffer[59] = "N/A";

		for (unsigned i = 0; i < 10; i++)
		{
			LOSTDisplayBuffer[i + 60] = "";
		}
	}

	//Item 71: Matrix 2 ID
	//Item 72: Matrix 2 GET
	//Item 73-81: Matrix 2
	if (EZJGSTBL.REF2 > 0)
	{
		REFSMMATData refs = EZJGMTX3.data[EZJGSTBL.REF2 - 1];
		if (refs.ID > 0)
		{
			FormatREFSMMATCode(EZJGSTBL.REF2, refs.ID, Buffer);
			LOSTDisplayBuffer[70].assign(Buffer);
			OrbMech::format_time_HHMMSS(Buffer, GETfromGMT(refs.GMT));
			LOSTDisplayBuffer[71].assign(Buffer);

			for (unsigned i = 0; i < 9; i++)
			{
				sprintf_s(Buffer, "%+010.7lf", refs.REFSMMAT.data[i]);
				LOSTDisplayBuffer[i + 72].assign(Buffer);
			}
		}
		else
		{
			LOSTDisplayBuffer[70] = "N/A";
			LOSTDisplayBuffer[71] = "";

			for (unsigned i = 0; i < 9; i++)
			{
				LOSTDisplayBuffer[i + 72] = "";
			}
		}
	}
	else
	{
		LOSTDisplayBuffer[70] = "N/A";
		for (unsigned i = 0; i < 10; i++)
		{
			LOSTDisplayBuffer[i + 71] = "";
		}
	}
}

void RTCC::CMMSLVNAV(VECTOR3 R_ecl, VECTOR3 V_ecl, double GMT)
{
	CZNAVSLV.PosS = mul(GZLTRA.IU1_REFSMMAT, R_ecl);
	CZNAVSLV.DotS = mul(GZLTRA.IU1_REFSMMAT, V_ecl);
	CZNAVSLV.NUPTIM = GMT - SystemParameters.MCGRIC * 3600.0;
}

//CMC/LGC Landing Site Update Load Generator
void RTCC::CMMCMCLS(int veh)
{
	LandingSiteMakupBuffer *buf;
	unsigned int RLSAddr;
	if (veh == RTCC_MPT_CSM)
	{
		buf = &CZLSVECT.CSMLSUpdate;
		buf->LoadType = "06";
		RLSAddr = SystemParameters.MCCRLS;
	}
	else
	{
		buf = &CZLSVECT.LMLSUpdate;
		buf->LoadType = "26";
		RLSAddr = SystemParameters.MCLRLS;
	}

	if (buf->SequenceNumber == 0)
	{
		if (veh == RTCC_MPT_CSM)
		{
			buf->SequenceNumber = 600;
		}
		else
		{
			buf->SequenceNumber = 2600;
		}
	}

	buf->SequenceNumber++;

	buf->GETofGeneration = GETfromGMT(RTCCPresentTimeGMT());
	buf->lat = BZLAND.lat[RTCC_LMPOS_BEST];
	buf->lng = BZLAND.lng[RTCC_LMPOS_BEST];
	buf->rad = BZLAND.rad[RTCC_LMPOS_BEST];

	ELGLCV(buf->lat, buf->lng, buf->R_LS, buf->rad);

	buf->Octals[0] = 10;
	buf->Octals[1] = RLSAddr;
	buf->Octals[2] = OrbMech::DoubleToBuffer(buf->R_LS.x, 27, 1);
	buf->Octals[3] = OrbMech::DoubleToBuffer(buf->R_LS.x, 27, 0);
	buf->Octals[4] = OrbMech::DoubleToBuffer(buf->R_LS.y, 27, 1);
	buf->Octals[5] = OrbMech::DoubleToBuffer(buf->R_LS.y, 27, 0);
	buf->Octals[6] = OrbMech::DoubleToBuffer(buf->R_LS.z, 27, 1);
	buf->Octals[7] = OrbMech::DoubleToBuffer(buf->R_LS.z, 27, 0);
}

void RTCC::CMMDTGTU(double t_land)
{
	if (CZTDTGTU.SequenceNumber == 0)
	{
		CZTDTGTU.SequenceNumber = 2800;
	}

	CZTDTGTU.LoadType = "28";
	CZTDTGTU.SequenceNumber++;
	CZTDTGTU.GETofGeneration = GETfromGMT(RTCCPresentTimeGMT());

	CZTDTGTU.Octals[0] = 5;

	CZTDTGTU.Octals[1] = SystemParameters.MCLTTD;
	CZTDTGTU.Octals[3] = SystemParameters.MCLTTD + 1;

	CZTDTGTU.Octals[2] = OrbMech::DoubleToBuffer(t_land*100.0, 28, 1);
	CZTDTGTU.Octals[4] = OrbMech::DoubleToBuffer(t_land*100.0, 28, 0);
}

void RTCC::CMMRXTDV(int source, int column)
{
	//source: 1 = time-to-fire, 2 = Return to Earth
	//column: 1 = primary, 2 = manual

	double TIG, lat, lng;
	VECTOR3 DV;

	if (source == 1)
	{
		if (column == 1)
		{
			//TBD
			return;
		}
		else
		{
			if (RZRFDP.data[2].Indicator != 0)
			{
				//Error
				return;
			}
			TIG = RZRFTT.Manual.GMTI;
			DV = RZRFTT.Manual.DeltaV;
			lat = RZRFTT.Manual.entry.lat_T;
			lng = RZRFTT.Manual.entry.lng_T;
		}
	}
	else
	{
		RTEDigitalSolutionTable *tab;
		if (column == 1)
		{
			tab = &PZREAP.RTEDTable[0];
		}
		else
		{
			tab = &PZREAP.RTEDTable[1];
		}
		if (tab->RTEDCode == "")
		{
			//Error
			return;
		}
		TIG = tab->GMTI;
		DV = tab->DV_XDV;
		lat = tab->lat_imp_tgt;
		lng = tab->lng_imp_tgt;
	}

	if (CZREXTDV.SequenceNumber == 0)
	{
		CZREXTDV.SequenceNumber = 1300;
	}
	CZREXTDV.LoadType = "13";
	CZREXTDV.SequenceNumber++;

	//Convert TIG to CMC time
	TIG = TIG - GetCMCClockZero();

	CZREXTDV.Octals[0] = 16;
	CZREXTDV.Octals[1] = 3400;
	CZREXTDV.Octals[2] = OrbMech::DoubleToBuffer(lat / PI2, 0, 1);
	CZREXTDV.Octals[3] = OrbMech::DoubleToBuffer(lat / PI2, 0, 0);
	CZREXTDV.Octals[4] = OrbMech::DoubleToBuffer(lng / PI2, 0, 1);
	CZREXTDV.Octals[5] = OrbMech::DoubleToBuffer(lng / PI2, 0, 0);
	CZREXTDV.Octals[6] = OrbMech::DoubleToBuffer(DV.x / 100.0, 7, 1);
	CZREXTDV.Octals[7] = OrbMech::DoubleToBuffer(DV.x / 100.0, 7, 0);
	CZREXTDV.Octals[8] = OrbMech::DoubleToBuffer(DV.y / 100.0, 7, 1);
	CZREXTDV.Octals[9] = OrbMech::DoubleToBuffer(DV.y / 100.0, 7, 0);
	CZREXTDV.Octals[10] = OrbMech::DoubleToBuffer(DV.z / 100.0, 7, 1);
	CZREXTDV.Octals[11] = OrbMech::DoubleToBuffer(DV.z / 100.0, 7, 0);
	CZREXTDV.Octals[12] = OrbMech::DoubleToBuffer(TIG*100.0, 28, 1);
	CZREXTDV.Octals[13] = OrbMech::DoubleToBuffer(TIG*100.0, 28, 0);
	CZREXTDV.GETLoadGeneration = GETfromGMT(RTCCPresentTimeGMT());
	CZREXTDV.GET_TIG = TIG;
	CZREXTDV.DV = DV / 0.3048;
	CZREXTDV.Lat = lat * DEG;
	CZREXTDV.Lng = lng * DEG;
}

void RTCC::CMMENTRY(double lat, double lng)
{
	if (CZENTRY.SequenceNumber == 0)
	{
		CZENTRY.SequenceNumber = 1400;
	}
	CZENTRY.LoadType = "14";
	CZENTRY.SequenceNumber++;

	CZENTRY.Octals[0] = 6;
	CZENTRY.Octals[1] = 3400;
	CZENTRY.Octals[2] = OrbMech::DoubleToBuffer(lat / PI2, 0, 1);
	CZENTRY.Octals[3] = OrbMech::DoubleToBuffer(lat / PI2, 0, 0);
	CZENTRY.Octals[4] = OrbMech::DoubleToBuffer(lng / PI2, 0, 1);
	CZENTRY.Octals[5] = OrbMech::DoubleToBuffer(lng / PI2, 0, 0);
	CZENTRY.GETLoadGeneration = GETfromGMT(RTCCPresentTimeGMT());
	CZENTRY.Lat = lat * DEG;
	CZENTRY.Lng = lng * DEG;
}

void RTCC::CMMLIFTF(int L, double hrs)
{
	AGCLiftoffTimeUpdateMakeupTableBlock *block;

	if (L == 1)
	{
		block = &CZLIFTFF.Blocks[0];

		if (block->SequenceNumber == 0)
		{
			block->SequenceNumber = 800;
		}
		else
		{
			block->SequenceNumber++;
		}
	}
	else
	{
		block = &CZLIFTFF.Blocks[1];

		if (block->SequenceNumber == 0)
		{
			block->SequenceNumber = 2500;
		}
		else
		{
			block->SequenceNumber++;
		}
	}

	block->GETofGeneration = GETfromGMT(RTCCPresentTimeGMT());

	double centiseconds = hrs * 360000.0;
	block->Octals[0] = OrbMech::DoubleToBuffer(centiseconds, 28, 1);
	block->Octals[1] = OrbMech::DoubleToBuffer(centiseconds, 28, 0);
	block->TimeIncrement = hrs * 3600.0;
}

void RTCC::CMMTMEIN(int L, double hrs)
{
	AGCTimeIncrementMakeupTableBlock *block;

	if (L == 1)
	{
		block = &CZTMEINC.Blocks[0];

		if (block->SequenceNumber == 0)
		{
			block->SequenceNumber = 700;
		}
		else
		{
			block->SequenceNumber++;
		}
	}
	else
	{
		block = &CZTMEINC.Blocks[1];

		if (block->SequenceNumber == 0)
		{
			block->SequenceNumber = 2400;
		}
		else
		{
			block->SequenceNumber++;
		}
	}

	block->GETofGeneration = GETfromGMT(RTCCPresentTimeGMT());

	double centiseconds = hrs * 360000.0;
	block->Octals[0] = OrbMech::DoubleToBuffer(centiseconds, 28, 1);
	block->Octals[1] = OrbMech::DoubleToBuffer(centiseconds, 28, 0);
	block->TimeIncrement = hrs * 3600.0;
}

void RTCC::CMMERMEM(int blocknum, int med, int line, const std::vector<int> &data)
{
	//med: 0 = CEI (initialize), 1 = CEE (enter in engineering format), 2 = CEO (enter in octal format), 3 = CEC (change or delete)

	if (blocknum < 0 || blocknum > 3) return;

	const int maxdata = 19;

	unsigned i;

	AGCErasableMemoryUpdateMakeupBlock* block = &CZERAMEM.Blocks[blocknum];

	if (med == 0)
	{
		//Initialize
		block->GETofGeneration = GETfromGMT(RTCCPresentTimeGMT());

		if (data[0] == 0)
		{
			//V72
			block->IsVerb72 = true;
		}
		else if (data[0] >= 0 && data[0] <= 03777)
		{
			//V71
			block->IsVerb72 = false;

			block->Data[0].OctalData = data[0];
			block->Data[0].DataType = 0;
			block->Data[0].EndOfDataFlag = true;
			block->Data[0].EngineeringUnits = 0.0;
			block->Data[0].ValueLeastSignBit = 0.0;
		}
		else
		{
			//Error
			return;
		}

		if (block->SequenceNumber == 0)
		{
			switch (blocknum)
			{
			case 0:
				block->SequenceNumber = 1800;
				break;
			case 1:
				block->SequenceNumber = 1900;
				break;
			case 2:
				block->SequenceNumber = 3800;
				break;
			case 3:
				block->SequenceNumber = 3900;
				break;
			}
		}
		block->SequenceNumber++;

		int num;

		if (block->IsVerb72)
		{
			num = 0;
		}
		else
		{
			num = 1;
		}

		//Blank out everything
		for (i = num; i < maxdata; i++)
		{
			block->Data[i].OctalData = 0x8000;
			block->Data[i].DataType = 1;
			block->Data[i].EndOfDataFlag = true;
			block->Data[i].ValueLeastSignBit = 0.0;
			block->Data[i].EngineeringUnits = 0.0;
		}
	}
	else if (med == 1 || med == 2)
	{
		//Octal data
		unsigned num = line - 2;

		//Check if any numbers before num are not valid
		for (i = 0; i < num; i++)
		{
			if (block->Data[i].OctalData == 0x8000)
			{
				//Error
				return;
			}
		}

		for (i = 0; i < data.size(); i++)
		{
			block->Data[num].OctalData = data[i];
			block->Data[num].DataType = 1;
			block->Data[num].EndOfDataFlag = false;
			block->Data[num].ValueLeastSignBit = 0.0;
			block->Data[num].EngineeringUnits = 0.0;

			num++;

			if (num >= maxdata) break;
		}

		for (i = 0; i < num - 1; i++)
		{
			block->Data[i].EndOfDataFlag = false;
		}

		block->Index = num + 1;

		//Blank out rest
		for (i = num; i < maxdata; i++)
		{
			block->Data[i].OctalData = 0x8000;
			block->Data[i].DataType = 1;
			block->Data[i].EndOfDataFlag = true;
			block->Data[i].ValueLeastSignBit = 0.0;
			block->Data[i].EngineeringUnits = 0.0;
		}
	}
	else if (med == 3)
	{
		//Change or delete data

		unsigned num = line - 2;

		if (num > 0)
		{
			block->Data[num - 1].EndOfDataFlag = true;
		}

		block->Index = num + 1;

		//Blank out rest
		for (i = num; i < maxdata; i++)
		{
			block->Data[i].OctalData = 0x8000;
			block->Data[i].DataType = 1;
			block->Data[i].EndOfDataFlag = true;
			block->Data[i].ValueLeastSignBit = 0.0;
			block->Data[i].EngineeringUnits = 0.0;
		}
	}
}

void RTCC::QMEPHEM(int EPOCH, int YEAR, int MONTH, int DAY, double HOURS)
{
	double J_D = OrbMech::TJUDAT(YEAR, MONTH, DAY);
	double gmtbase = J_D - 2400000.5;
	QMGEPH(EPOCH, gmtbase, HOURS);
	QMPNREAD(gmtbase);
}

void RTCC::QMPNREAD(double gmtbase)
{
	MATRIX3 Rot, Rot2, E, Rot3;
	double dt;

	E = { 1,0,0, 0,1,0, 0,0,1 };
	Rot3 = SystemParameters.MAT_J2000_BRCS;

	//Table starts 5 days prior to launch
	double mjd = gmtbase - 5.0;
	EZNPMATX.mjd0 = mjd;
	for (int i = 0;i < 141;i++)
	{
		Rot = OrbMech::GetRotationMatrix(BODY_EARTH, mjd);
		Rot = MatrixRH_LH(Rot);
		dt = (mjd - gmtbase) * 24.0 * 3600.0;
		//Rotate from true coordinates back to midnight before launch. Then rotate to ecliptic coordinates
		Rot2 = mul(Rot, GLMRTM(E, OrbMech::w_Earth * dt, 3));
		//Lastly rotate to basic reference coordinate system
		EZNPMATX.Mat[i] = OrbMech::tmat(mul(Rot3, Rot2));
		//One matrix every 6 hours
		mjd = mjd + 0.25;
	}
}

bool RTCC::QMGEPH(int epoch, double gmtbase, double HOURS)
{
	MATRIX3 Rot_J_B, Rot_SG_ECL, R_LIB;
	VECTOR3 R_EM, R_ES, V_EM;
	double MJD, MoonPos[12], EarthPos[12];
	CELBODY *cMoon, *cEarth;

	cMoon = oapiGetCelbodyInterface(hMoon);
	cEarth = oapiGetCelbodyInterface(hEarth);

	//Store beginning time of ephemeris
	MDGSUN.MJD = gmtbase - 5.0;

	//Get matrix at epoch
	Rot_J_B = OrbMech::J2000EclToBRCS(epoch);

	for (int i = 0;i < 71;i++)
	{
		MJD = MDGSUN.MJD + 0.5*(double)(i);

		//Moon Ephemeris
		cMoon->clbkEphemeris(MJD, EPHEM_TRUEPOS, MoonPos);
		R_EM = _V(MoonPos[0], MoonPos[2], MoonPos[1]) / SystemParameters.MCCMCU;
		V_EM = _V(MoonPos[3], MoonPos[5], MoonPos[4]) / SystemParameters.MCCMCU*3600.0;
		R_EM = mul(Rot_J_B, R_EM);
		V_EM = mul(Rot_J_B, V_EM);

		//Sun Ephemeris
		cEarth->clbkEphemeris(MJD, EPHEM_TRUEPOS | EPHEM_TRUEVEL, EarthPos);
		R_ES = -OrbMech::Polar2Cartesian(EarthPos[2] * AU, EarthPos[1], EarthPos[0]) / SystemParameters.MCCMCU;
		R_ES = mul(Rot_J_B, R_ES);

		//Libration matrix
		Rot_SG_ECL = OrbMech::GetRotationMatrix(BODY_MOON, MJD);
		Rot_SG_ECL = MatrixRH_LH(Rot_SG_ECL);
		R_LIB = OrbMech::tmat(mul(Rot_J_B, Rot_SG_ECL));

		//Write data
		MDGSUN.data[i][0] = R_ES.x; MDGSUN.data[i][1] = R_ES.y; MDGSUN.data[i][2] = R_ES.z;
		MDGSUN.data[i][3] = R_EM.x; MDGSUN.data[i][4] = R_EM.y; MDGSUN.data[i][5] = R_EM.z;
		MDGSUN.data[i][6] = V_EM.x; MDGSUN.data[i][7] = V_EM.y; MDGSUN.data[i][8] = V_EM.z;
		MDGSUN.data[i][9] = R_LIB.m11; MDGSUN.data[i][10] = R_LIB.m12; MDGSUN.data[i][11] = R_LIB.m13;
		MDGSUN.data[i][12] = R_LIB.m21; MDGSUN.data[i][13] = R_LIB.m22; MDGSUN.data[i][14] = R_LIB.m23;
		MDGSUN.data[i][15] = R_LIB.m31; MDGSUN.data[i][16] = R_LIB.m32; MDGSUN.data[i][17] = R_LIB.m33;
	}

	//MCCBES stored as the time of midnight on launch day since beginning of the year
	//MCCBES = HOURS;
	return false;
}

bool RTCC::CalculateAGSKFactor(agc_t *agc, ags_t *aea, double &KFactor)
{
	//This function only works in a thread
	uint32_t tim_old, TA1, TA2;
	double simt;

	simt = oapiGetSimTime();
	tim_old = aea->Memory[0377];
	do
	{
		if (oapiGetSimTime() > simt + 3.0)
		{
			//Give up after 3 seconds
			return false;
		}
		TA1 = aea->Memory[0377];
	} while (tim_old == TA1 || TA1 == 0);
	//Quickly get data from AGC
	double t_agc = GetClockTimeFromAGC(agc) / 100.0;
	TA2 = aea->Memory[0353];
	double t_aea = 2.0*TA1 + pow(2, -16)*TA2;
	KFactor = t_agc - t_aea;

	return true;
}

void RTCC::RMMYNI(const RMMYNIInputTable &in, RMMYNIOutputTable &out)
{
	ReentryNumericalIntegrator integ(this);
	integ.Main(in, out);
}

void RTCC::RMMGIT(EphemerisData2 sv_EI, double lng_T)
{
	//sv_EI in ECT

	RMMYNIInputTable in;
	RMMYNIOutputTable out;
	double lng_min, lng_max;

	in.R0 = sv_EI.R;
	in.V0 = sv_EI.V;
	in.GMT0 = sv_EI.GMT;
	in.D0 = 2.0*9.80665;
	in.K1 = 0.0;
	in.K2 = 55.0*RAD;
	in.KSWCH = 10;

	RMMYNI(in, out);

	lng_min = out.lng_IP;

	in.D0 = 5.0*9.80665;

	RMMYNI(in, out);

	lng_max = out.lng_IP;
}

void RTCC::RMSDBMP(EphemerisData sv, double CSMmass)
{
	RetrofirePlanning plan(this);
	if (plan.RMSDBMP(sv, RZJCTTC.R32_GETI, RZJCTTC.R32_lat_T, RZJCTTC.R32_lng_T, CSMmass, PZMPTCSM.CommonBlock.CSMArea))
	{
		RZRFDP.data[2].Indicator = -1;
	}
	else
	{
		//External DV display
		RMDRXDV(false);
	}
}

void RTCC::RMDRTSD(EphemerisDataTable2 &tab, int opt, double val, double lng_des)
{
	if (tab.Header.CSI != BODY_EARTH) return;

	ManeuverTimesTable MANTIMES;
	EphemerisData2 sv_ECT;
	double GMT_cross, out, lng, GMT_guess, r_i, v_i, lat_i, lng_i, gamma_i, azi_i;
	int i;

	RZDRTSD.ErrorMessage = "";
	RZDRTSD.CurrentPage = 1;
	RZDRTSD.TotalNumEntries = 0;
	RZDRTSD.TotalNumPages = 1;
	for (i = 0;i < 40;i++)
	{
		RZDRTSD.table[i].DataIndicator = true;
	}
	i = 0;
	lng = lng_des;
	GMT_guess = val;

	//Convert to ECT
	EphemerisDataTable2 tab2;
	EphemerisData2 sv_out;
	EphemerisData2 sv_inter;

	ELVCNV(tab.table, 0, 1, tab2.table);
	tab2.Header = tab.Header;
	tab2.Header.CSI = 1;

	do
	{
		out = RLMTLC(tab2, MANTIMES, lng, GMT_guess, GMT_cross, sv_inter);
		if (out == -1)
		{
			//No convergence
			break;
		}
		RZDRTSD.table[i].DataIndicator = false;
		if (out == 0)
		{
			//Good data
			RZDRTSD.table[i].AlternateLongitudeIndicator = false;
		}
		else
		{
			//Not converged
			RZDRTSD.table[i].AlternateLongitudeIndicator = true;
		}

		PICSSC(true, sv_inter.R, sv_inter.V, r_i, v_i, lat_i, lng_i, gamma_i, azi_i);
		lng_i -= OrbMech::w_Earth*sv_inter.GMT;
		OrbMech::normalizeAngle(lng_i, false);

		RZDRTSD.table[i].Azimuth = azi_i*DEG;
		RZDRTSD.table[i].GET = GETfromGMT(GMT_cross);
		RZDRTSD.table[i].GMT = GMT_cross;
		RZDRTSD.table[i].Latitude = lat_i*DEG;
		RZDRTSD.table[i].Longitude = lng_i*DEG;
		RZDRTSD.table[i].Rev = CapeCrossingRev(RTCC_MPT_CSM, GMT_cross);

		GMT_guess = GMT_cross;
		lng += 1.0*RAD;
		if (lng > PI)
		{
			lng -= PI2;
		}

		i++;
	}
	while (i < 40);

	RZDRTSD.TotalNumEntries = i;
	if (RZDRTSD.TotalNumEntries > 30)
	{
		RZDRTSD.TotalNumPages = 4;
	}
	else if (RZDRTSD.TotalNumEntries > 20)
	{
		RZDRTSD.TotalNumPages = 3;
	}
	else if (RZDRTSD.TotalNumEntries > 10)
	{
		RZDRTSD.TotalNumPages = 2;
	}
}

int RTCC::RMRMED(std::string med, std::vector<std::string> data)
{
	//Direct transfer to spacecraft setting
	if (med == "65")
	{
		if (data.size() != 3)
		{
			return 1;
		}

		if (data[0] != "CSM")
		{
			return 2;
		}

		int entry;

		if (data[1] == "T")
		{
			int entry;

			if (data[2] == "P")
			{
				entry = 1;
			}
			else if (data[2] == "M")
			{
				entry = 2;
			}
			else
			{
				return 2;
			}

			RMSSCS(entry);
		}
		else if (data[1] == "R")
		{
			if (data[2] == "P")
			{
				entry = 3;
			}
			else if (data[2] == "M")
			{
				entry = 4;
			}
			else
			{
				return 2;
			}

			RMSSCS(entry);
		}
		else
		{
			return 2;
		}
	}

	return 0;
}

void RTCC::RMSSCS(int entry)
{
	double lat, lng;

	if (entry == 1 || entry == 2)
	{
		//Transfer TTF

		RetrofireTransferTableEntry *tab;
		RetrofireDisplayParametersTableData *tab2;

		if (entry == 1)
		{
			//Transfer TTF, Primary column data
			tab = &RZRFTT.Primary;
			tab2 = &RZRFDP.data[0];
		}
		else
		{
			//Transfer TTF, Manual column data
			tab = &RZRFTT.Manual;
			tab2 = &RZRFDP.data[2];
		}

		if (tab2->Indicator != 0)
		{
			std::vector<std::string> message;
			message.push_back("TTF TRANSFER TABLE IS NOT SET FOR S/C SETTING");
			OnlinePrint("RMSSCS", message);
			return;
		}

		lat = tab->entry.lat_T;
		lng = tab->entry.lng_T;
	}
	else if (entry == 3 || entry == 4)
	{
		//Transfer RTE

		RTEDigitalSolutionTable *tab;

		if (entry == 3)
		{
			//Transfer RTE, Primary column data
			tab = &PZREAP.RTEDTable[0];
		}
		else
		{
			//Transfer RTE, Manual column data
			tab = &PZREAP.RTEDTable[1];
		}

		if (tab->Error)
		{
			std::vector<std::string> message;
			message.push_back("RTE TRANSFER TABLE IS NOT SET FOR S/C SETTING");
			OnlinePrint("RMSSCS", message);
			return;
		}

		lat = tab->lat_imp_tgt;
		lng = tab->lng_imp_tgt;
	}
	else
	{
		//Manual input from R40, R66 and R67
	}

	RZDBSC1.Indicator = 0;
	RZDBSC1.lat_T = lat;
	RZDBSC1.lng_T = lng;
}

void RTCC::RTACFGuidanceOpticsSupportTable(RTACFGOSTInput in, RTACFGOSTOutput &out)
{
	//Propagate to GET
	EMMENIInputTable emmin;
	emmin.AnchorVector = in.sv;
	if (in.option == 4)
	{
		emmin.Area = PZMPTCSM.ConfigurationArea;
	}
	else
	{
		emmin.Area = PZMPTLEM.ConfigurationArea;
	}
	emmin.CutoffIndicator = 1;
	emmin.DensityMultiplier = 1.0;
	emmin.Weight = in.Weight;
	emmin.MaxIntegTime = GMTfromGET(in.get) - in.sv.GMT;
	if (emmin.MaxIntegTime > 0)
	{
		emmin.IsForwardIntegration = 1.0;
	}
	else
	{
		emmin.IsForwardIntegration = -1.0;
		emmin.MaxIntegTime = abs(emmin.MaxIntegTime);
	}
	EMMENI(emmin);

	//Compute REFSMMAT from two star IDs and shaft and trunnion angles (and attitude)
	if (in.option == 1)
	{
		//TBD
	}
	//Compute location of two stars in the scanning telescope at a specific attitude and IMU alignment
	else if (in.option == 2)
	{
		//TBD
	}
	//Compute shaft and trunnion angles for two input stars
	else if (in.option == 3)
	{
		//TBD
	}
	//Compute gimbal angles and LVLH pitch angle from spacecraft LVLH roll and yaw angle plus spacecraft pitch angle to the horizon
	else if (in.option == 4 || in.option == 14)
	{
		//Calculate LVLH pitch angle
		out.LVLHAtt.x = in.LVLHRoll;
		out.LVLHAtt.y = -in.WindowLine - acos(OrbMech::R_Earth / length(emmin.sv_cutoff.R));
		out.LVLHAtt.z = in.LVLHYaw;

		double SINP, SINY, SINR, COSP, COSY, COSR;
		SINP = sin(out.LVLHAtt.y);
		SINY = sin(in.LVLHYaw);
		SINR = sin(in.LVLHRoll);
		COSP = cos(out.LVLHAtt.y);
		COSY = cos(in.LVLHYaw);
		COSR = cos(in.LVLHRoll);

		VECTOR3 Z_P, Y_P, X_P;
		Z_P = -unit(emmin.sv_cutoff.R);
		Y_P = -unit(crossp(emmin.sv_cutoff.R, emmin.sv_cutoff.V));
		X_P = crossp(Y_P, Z_P);

		double AL, BE, a1, a2, a3, b1, b2, b3, c1, c2, c3;
		AL = SINP * SINR;
		BE = SINP * COSR;
		a1 = COSY * COSP;
		a2 = SINY * COSP;
		a3 = -SINP;
		b1 = AL * COSY - SINY * COSR;
		b2 = AL * SINY + COSY * COSR;
		b3 = COSP * SINR;
		c1 = BE * COSY + SINY * SINR;
		c2 = BE * SINY - COSY * SINR;
		c3 = COSP * COSR;

		VECTOR3 X_B, Y_B, Z_B;
		X_B = X_P * a1 + Y_P * a2 + Z_P * a3;
		Y_B = X_P * b1 + Y_P * b2 + Z_P * b3;
		Z_B = X_P * c1 + Y_P * c2 + Z_P * c3;

		//Calculate IMU angles
		X_P = _V(in.REFSMMAT.m11, in.REFSMMAT.m12, in.REFSMMAT.m13);
		Y_P = _V(in.REFSMMAT.m21, in.REFSMMAT.m22, in.REFSMMAT.m23);
		Z_P = _V(in.REFSMMAT.m31, in.REFSMMAT.m32, in.REFSMMAT.m33);

		out.IMUAtt.z = asin(dotp(Y_P, X_B));
		if (abs(abs(out.IMUAtt.z) - PI05) < 1e-8)
		{
			out.IMUAtt.x = 0.0;
			out.IMUAtt.y = atan2(dotp(X_P, Z_B), dotp(Z_P, Z_B));
		}
		else
		{
			out.IMUAtt.x = atan2(-dotp(Y_P, Z_B), dotp(Y_P, Y_B));
			out.IMUAtt.y = atan2(-dotp(Z_P, X_B), dotp(X_P, X_B));
		}

		if (out.IMUAtt.x < 0)
		{
			out.IMUAtt.x += PI2;
		}
		if (out.IMUAtt.y < 0)
		{
			out.IMUAtt.y += PI2;
		}
		if (out.IMUAtt.z < 0)
		{
			out.IMUAtt.z += PI2;
		}
	}
}

void RTCC::RMDRXDV(bool rte)
{
	RetrofireEXDV.data.resize(2);

	bool valid;
	for (unsigned i = 0;i < 2;i++)
	{
		RetrofireExternalDVDisplayData *tab = &RetrofireEXDV.data[i];

		if (rte)
		{
			valid = PZREAP.RTEDTable[i].RTEDCode != "";
		}
		else
		{
			if (i == 0)
			{
				valid = RZRFDP.data[0].Indicator == 0;
			}
			else
			{
				valid = RZRFDP.data[2].Indicator == 0;
			}
		}

		if (valid)
		{
			if (rte)
			{
				RTEDigitalSolutionTable *tab2 = &PZREAP.RTEDTable[i];

				tab->Indicator = true;
				tab->GETI = tab2->GETI;
				tab->DV = tab2->DV_XDV / 0.3048;

				if (tab2->ThrusterCode == RTCC_ENGINETYPE_CSMSPS)
				{
					tab->P_G = (tab2->R_Y - SystemParameters.MCTSPP)*DEG;
					tab->Y_G = (tab2->R_Z - SystemParameters.MCTSYP)*DEG;
				}
				else
				{
					tab->P_G = 0.0;
					tab->Y_G = 0.0;
				}

				tab->DT_TO = tab2->dt_TO;
				tab->DV_TO = tab2->dv_TO / 0.3048;
				tab->H_apo = tab2->h_a / 1852.0;
				tab->H_peri = tab2->h_p / 1852.0;
				tab->lat_IP = tab2->lat_imp_tgt*DEG;
				tab->lng_IP = tab2->lng_imp_tgt*DEG;
			}
			else
			{
				RetrofireDisplayParametersTableData *tab2;
				if (i == 0)
				{
					tab2 = &RZRFDP.data[0];
				}
				else
				{
					tab2 = &RZRFDP.data[2];
				}

				tab->Indicator = true;
				tab->GETI = tab2->GETI;
				tab->DV = tab2->VG_XDX;
				tab->P_G = tab2->P_G;
				tab->Y_G = tab2->Y_G;
				tab->DT_TO = tab2->DT_TO;
				tab->DV_TO = tab2->DV_TO;
				tab->H_apo = tab2->H_apo;
				tab->H_peri = tab2->H_peri;
				tab->lat_IP = tab2->lat_IP;
				tab->lng_IP = tab2->lng_IP;
			}
		}
		else
		{
			tab->Indicator = false;
			tab->GETI = 0.0;
			tab->DV = _V(0, 0, 0);
			tab->P_G = tab->Y_G = 0.0;
			tab->DT_TO = tab->DV_TO = 0.0;
			tab->H_apo = tab->H_peri = 0.0;
			tab->lat_IP = tab->lng_IP = 0.0;
		}
	}
}