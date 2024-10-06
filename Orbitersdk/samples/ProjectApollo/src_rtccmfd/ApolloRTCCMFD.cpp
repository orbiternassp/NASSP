// ==============================================================
//                 ORBITER MODULE: DialogTemplate
//                  Part of the ORBITER SDK
//          Copyright (C) 2003-2010 Martin Schweiger
//                   All rights reserved
//
// ApolloRTCCMFD.cpp
//
// This module demonstrates how to build an Orbiter plugin which
// inserts a new MFD (multi-functional display) mode. The code
// is not very useful in itself, but it can be used as a starting
// point for your own MFD developments.
// ==============================================================

#define STRICT

#include "ApolloRTCCMFD.h"
#include "papi.h"
#include "LVDC.h"
#include "iu.h"
#include "ARoapiModule.h"
#include "TLMCC.h"
#include "nassputils.h"

using namespace nassp;

// ==============================================================
// Global variables

ARoapiModule *g_coreMod;
int g_MFDmode; // identifier for new MFD mode
ARCore *GCoreData[32];
VESSEL *GCoreVessel[32];
AR_GCore *g_SC = NULL;      // points to the static core, root of all persistence
int nGutsUsed;
bool initialised = false;
std::vector<RTCCMFDData> g_MFDData;

// ==============================================================
// MFD class implementation

// Constructor
ApolloRTCCMFD::ApolloRTCCMFD (DWORD w, DWORD h, VESSEL *vessel, UINT im)
: MFD2 (w, h, vessel)
{
	ID = im;
	screen = 0;
	subscreen = 0;

	if (!g_SC) {
		g_SC = new AR_GCore(vessel);                     // First time only in this Orbiter session. Init the static core.
	}
	GC = g_SC;                                  // Make the ApolloRTCCMFD instance Global Core point to the static core. 

	//font = oapiCreateFont(w / 20, true, "Arial", FONT_NORMAL, 0);
	font = oapiCreateFont(w / 20, true, "Courier", FONT_NORMAL, 0);
	font2 = oapiCreateFont(w * 2 / 51, true, "Courier", FONT_NORMAL, 0);
	font2vert = oapiCreateFont(w / 24, true, "Courier", FONT_NORMAL, 900);
	fonttest = oapiCreateFont(w / 32, false, "Courier New", FONT_NORMAL, 0);
	font3 = oapiCreateFont(w / 24, true, "Courier", FONT_NORMAL, 0);
	font4 = oapiCreateFont(w / 31, true, "Courier", FONT_NORMAL, 0);
	font5 = oapiCreateFont(w / 32, false, "Fixed", FONT_NORMAL, 0);

	pen = oapiCreatePen(1, 1, 0x00FFFF);
	pen2 = oapiCreatePen(1, 1, 0x00FFFFFF);
	bool found = false;
	for (int i = 0; i < nGutsUsed; i++) {
		if (i == 32){
			i = 0;
			GCoreVessel[i] = vessel;
		}
		if (GCoreVessel[i] == vessel)
		{
			found = true;
			G = GCoreData[i];
		}
	}
	if (!found)
	{
		GCoreData[nGutsUsed] = new ARCore(vessel, GC);
		G = GCoreData[nGutsUsed];
		GCoreVessel[nGutsUsed] = vessel;
		nGutsUsed++;
	}

	marker = 0;
	markermax = 0;
	IsCSM = true;
	EnableCalculation = false;
	ErrorMessage = false;

	LoadState();
}

// Destructor
ApolloRTCCMFD::~ApolloRTCCMFD ()
{
	// Add MFD cleanup code here
	oapiReleaseFont(font);
	oapiReleaseFont(font2);
	oapiReleaseFont(font2vert);
	oapiReleaseFont(fonttest);
	oapiReleaseFont(font3);
	oapiReleaseFont(font4);
	oapiReleaseFont(font5);
	oapiReleasePen(pen);
	oapiReleasePen(pen2);

	SaveState();
}

void ApolloRTCCMFD::SaveState()
{
	RTCCMFDData temp;

	temp.marker = marker;
	temp.markermax = markermax;
	temp.screen = screen;
	temp.subscreen = subscreen;
	temp.ID = ID;
	temp.MEDCode = MEDInputData.MEDCode;
	temp.IsCSM = IsCSM;
	temp.EnableCalculation = EnableCalculation;

	bool found = false;

	//Search for existing MFD data
	for (unsigned i = 0; i < g_MFDData.size(); i++)
	{
		if (g_MFDData[i].ID == ID)
		{
			//Found it, save in that place
			g_MFDData[i] = temp;
			found = true;
			break;
		}
	}

	if (!found)
	{
		//Found in array yet, add it
		g_MFDData.push_back(temp);
	}
}

void ApolloRTCCMFD::LoadState()
{
	//Load MFD data

	for (unsigned i = 0; i < g_MFDData.size(); i++)
	{
		if (g_MFDData[i].ID == ID)
		{
			//Found it, load data
			marker = g_MFDData[i].marker;
			markermax = g_MFDData[i].markermax;
			screen = g_MFDData[i].screen;
			subscreen = g_MFDData[i].subscreen;
			MEDInputData.MEDCode = g_MFDData[i].MEDCode;
			IsCSM = g_MFDData[i].IsCSM;
			EnableCalculation = g_MFDData[i].EnableCalculation;
			break;
		}
	}
}

// Return button labels
char *ApolloRTCCMFD::ButtonLabel (int bt)
{
	// The labels for the two buttons used by our MFD mode
	return coreButtons.ButtonLabel(bt);
}

// Return button menus
int ApolloRTCCMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	// The menu descriptions for the two buttons
	return  coreButtons.ButtonMenu(menu);
}

bool ApolloRTCCMFD::ConsumeButton(int bt, int event)
{
	return coreButtons.ConsumeButton(this, bt, event);
}

bool ApolloRTCCMFD::ConsumeKeyBuffered(DWORD key)
{
	return coreButtons.ConsumeKeyBuffered(this, key);
}

void ApolloRTCCMFD::WriteStatus(FILEHANDLE scn) const
{
	//oapiWriteScenario_int(scn, "SCREEN", G->screen);
	papiWriteScenario_bool(scn, "VESSELISDOCKED", G->vesselisdocked);
	papiWriteScenario_double(scn, "SXTSTARDTIME", G->sxtstardtime);
	oapiWriteScenario_int(scn, "REFSMMATcur", G->REFSMMATcur);
	oapiWriteScenario_int(scn, "REFSMMATopt", G->REFSMMATopt);
	papiWriteScenario_double(scn, "REFSMMAT_LVLH_Time", G->REFSMMAT_LVLH_Time);
	papiWriteScenario_bool(scn, "REFSMMATHeadsUp", G->REFSMMATHeadsUp);
	papiWriteScenario_double(scn, "T1", GC->rtcc->med_k30.StartTime);
	papiWriteScenario_double(scn, "T2", GC->rtcc->med_k30.EndTime);
	papiWriteScenario_double(scn, "CDHTIME", G->CDHtime);
	papiWriteScenario_double(scn, "SPQTIG", G->SPQTIG);
	oapiWriteScenario_int(scn, "CDHTIMEMODE", G->CDHtimemode);
	papiWriteScenario_vec(scn, "SPQDeltaV", G->SPQDeltaV);

	papiWriteScenario_double(scn, "P30TIG", G->P30TIG);
	papiWriteScenario_vec(scn, "DV_LVLH", G->dV_LVLH);
	papiWriteScenario_double(scn, "ENTRYRANGE", G->entryrange);
	oapiWriteScenario_int(scn, "LANDINGZONE", G->landingzone);
	oapiWriteScenario_int(scn, "ENTRYPRECISION", G->entryprecision);

	oapiWriteScenario_int(scn, "MAPPAGE", G->mappage);
	papiWriteScenario_double(scn, "GMPApogeeHeight", G->GMPApogeeHeight);
	papiWriteScenario_double(scn, "GMPPerigeeHeight", G->GMPPerigeeHeight);
	papiWriteScenario_double(scn, "GMPWedgeAngle", G->GMPWedgeAngle);
	papiWriteScenario_double(scn, "GMPManeuverHeight", G->GMPManeuverHeight);
	papiWriteScenario_double(scn, "GMPManeuverLongitude", G->GMPManeuverLongitude);
	papiWriteScenario_double(scn, "GMPHeightChange", G->GMPHeightChange);
	papiWriteScenario_double(scn, "GMPNodeShiftAngle", G->GMPNodeShiftAngle);
	papiWriteScenario_double(scn, "GMPDeltaVInput", G->GMPDeltaVInput);
	papiWriteScenario_double(scn, "GMPPitch", G->GMPPitch);
	papiWriteScenario_double(scn, "GMPYaw", G->GMPYaw);
	papiWriteScenario_double(scn, "GMPApseLineRotAngle", G->GMPApseLineRotAngle);
	oapiWriteScenario_int(scn, "GMPRevs", G->GMPRevs);
	oapiWriteScenario_int(scn, "GMPManeuverPoint", G->GMPManeuverPoint);
	oapiWriteScenario_int(scn, "GMPManeuverType", G->GMPManeuverType);
	oapiWriteScenario_int(scn, "GMPManeuverCode", G->GMPManeuverCode);
	papiWriteScenario_double(scn, "SPSGET", G->SPSGET);

	papiWriteScenario_double(scn, "t_TPI", G->t_TPI);

	papiWriteScenario_double(scn, "LDPPGETTH1", GC->rtcc->med_k16.GETTH1);
	papiWriteScenario_double(scn, "LDPPGETTH2", GC->rtcc->med_k16.GETTH2);
	papiWriteScenario_double(scn, "LDPPGETTH3", GC->rtcc->med_k16.GETTH3);
	papiWriteScenario_double(scn, "LDPPGETTH4", GC->rtcc->med_k16.GETTH4);

	papiWriteScenario_double(scn, "t_Liftoff_guess", G->t_LunarLiftoff);
	papiWriteScenario_double(scn, "t_TPIguess", G->t_TPIguess);

	papiWriteScenario_bool(scn, "MISSIONPLANNINGACTIVE", GC->MissionPlanningActive);
}

void ApolloRTCCMFD::ReadStatus(FILEHANDLE scn)
{
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, "END_MFD", 7))
			break;

		//papiReadScenario_int(line, "SCREEN", G->screen);
		papiReadScenario_bool(line, "VESSELISDOCKED", G->vesselisdocked);
		papiReadScenario_double(line, "SXTSTARDTIME", G->sxtstardtime);
		papiReadScenario_int(line, "REFSMMATcur", G->REFSMMATcur);
		papiReadScenario_int(line, "REFSMMATopt", G->REFSMMATopt);
		papiReadScenario_double(line, "REFSMMAT_LVLH_Time", G->REFSMMAT_LVLH_Time);
		papiReadScenario_bool(line, "REFSMMATHeadsUp", G->REFSMMATHeadsUp);
		papiReadScenario_double(line, "T1", GC->rtcc->med_k30.StartTime);
		papiReadScenario_double(line, "T2", GC->rtcc->med_k30.EndTime);
		papiReadScenario_double(line, "CDHTIME", G->CDHtime);
		papiReadScenario_double(line, "SPQTIG", G->SPQTIG);
		papiReadScenario_int(line, "CDHTIMEMODE", G->CDHtimemode);
		papiReadScenario_vec(line, "SPQDeltaV", G->SPQDeltaV);

		papiReadScenario_double(line, "P30TIG", G->P30TIG);
		papiReadScenario_vec(line, "DV_LVLH", G->dV_LVLH);
		papiReadScenario_double(line, "ENTRYRANGE", G->entryrange);
		papiReadScenario_int(line, "LANDINGZONE", G->landingzone);
		papiReadScenario_int(line, "ENTRYPRECISION", G->entryprecision);

		papiReadScenario_int(line, "MAPPAGE", G->mappage);
		papiReadScenario_double(line, "GMPApogeeHeight", G->GMPApogeeHeight);
		papiReadScenario_double(line, "GMPPerigeeHeight", G->GMPPerigeeHeight);
		papiReadScenario_double(line, "GMPWedgeAngle", G->GMPWedgeAngle);
		papiReadScenario_double(line, "GMPManeuverHeight", G->GMPManeuverHeight);
		papiReadScenario_double(line, "GMPManeuverLongitude", G->GMPManeuverLongitude);
		papiReadScenario_double(line, "GMPHeightChange", G->GMPHeightChange);
		papiReadScenario_double(line, "GMPNodeShiftAngle", G->GMPNodeShiftAngle);
		papiReadScenario_double(line, "GMPDeltaVInput", G->GMPDeltaVInput);
		papiReadScenario_double(line, "GMPPitch", G->GMPPitch);
		papiReadScenario_double(line, "GMPYaw", G->GMPYaw);
		papiReadScenario_double(line, "GMPApseLineRotAngle", G->GMPApseLineRotAngle);
		papiReadScenario_int(line, "GMPRevs", G->GMPRevs);
		papiReadScenario_int(line, "GMPManeuverPoint", G->GMPManeuverPoint);
		papiReadScenario_int(line, "GMPManeuverType", G->GMPManeuverType);
		papiReadScenario_int(line, "GMPManeuverCode", G->GMPManeuverCode);
		papiReadScenario_double(line, "SPSGET", G->SPSGET);

		papiReadScenario_double(line, "t_TPI", G->t_TPI);

		papiReadScenario_double(line, "LDPPGETTH1", GC->rtcc->med_k16.GETTH1);
		papiReadScenario_double(line, "LDPPGETTH2", GC->rtcc->med_k16.GETTH2);
		papiReadScenario_double(line, "LDPPGETTH3", GC->rtcc->med_k16.GETTH3);
		papiReadScenario_double(line, "LDPPGETTH4", GC->rtcc->med_k16.GETTH4);

		papiReadScenario_double(line, "t_Liftoff_guess", G->t_LunarLiftoff);
		papiReadScenario_double(line, "t_TPIguess", G->t_TPIguess);

		papiReadScenario_bool(line, "MISSIONPLANNINGACTIVE", GC->MissionPlanningActive);

		//G->coreButtons.SelectPage(this, G->screen);
	}
}

void ApolloRTCCMFD::Text(oapi::Sketchpad *skp, std::string message, int x, int y, int xmax, int ymax)
{
	skp->Text(x * W / xmax, y * H / ymax, message.c_str(), message.size());
}

void ApolloRTCCMFD::menuEntryUpdateUpload()
{
	G->EntryUpdateUplink();
}

void ApolloRTCCMFD::menuP30UplinkCalc()
{
	if (GC->MissionPlanningActive)
	{
		menuGeneralMEDRequest("Get TIG and DV from MPT. Format: C10, Vehicle Type (CMC or LGC), Maneuver Number (1-15), MPT Indicator (CSM or LEM);");
	}
	else
	{
		G->P30UplinkCalc(screen == 51);
	}
}

void ApolloRTCCMFD::menuP30Uplink()
{
	G->P30Uplink(screen == 51);
}

void ApolloRTCCMFD::menuRetrofireEXDVUplink()
{
	G->RetrofireEXDVUplink();
}

void ApolloRTCCMFD::menuRetrofireEXDVUplinkCalc()
{
	bool RetrofireEXDVUplinkCalcInput(void *id, char *str, void *data);
	oapiOpenInputBox("Enter source and column for uplink. Format: Source Column e.g. 'R P'. T for Time-to-Fire (Deorbit), R = for Return-to-Earth. P for Primary and M for Manual column.", RetrofireEXDVUplinkCalcInput, 0, 20, (void*)this);
}

bool RetrofireEXDVUplinkCalcInput(void *id, char *str, void *data)
{
	if (strlen(str) < 8)
	{
		return ((ApolloRTCCMFD*)data)->set_RetrofireEXDVUplinkCalc(str);
	}
	return false;
}

bool ApolloRTCCMFD::set_RetrofireEXDVUplinkCalc(char *str)
{
	char str1, str2;
	if (sscanf(str, "%c %c", &str1, &str2) == 2)
	{
		G->RetrofireEXDVUplinkCalc(str1, str2);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuEntryUplinkCalc()
{
	G->EntryUplinkCalc();
}

void ApolloRTCCMFD::menuTLANDUplinkCalc()
{
	G->TLANDUplinkCalc();
}

void ApolloRTCCMFD::menuTLANDUpload()
{
	G->TLANDUplink();
}

void ApolloRTCCMFD::Angle_Display(char *Buff, double angle, bool DispPlus)
{
	double angle2 = abs(round(angle));
	if (angle >= 0)
	{
		if (DispPlus)
		{
			sprintf_s(Buff, 32, "+%03.0f:%02.0f:%02.0f", floor(angle2 / 3600.0), floor(fmod(angle2, 3600.0) / 60.0), fmod(angle2, 60.0));
		}
		else
		{
			sprintf_s(Buff, 32, "%03.0f:%02.0f:%02.0f", floor(angle2 / 3600.0), floor(fmod(angle2, 3600.0) / 60.0), fmod(angle2, 60.0));
		}
	}
	else
	{
		sprintf_s(Buff, 32, "-%03.0f:%02.0f:%02.0f", floor(angle2 / 3600.0), floor(fmod(angle2, 3600.0) / 60.0), fmod(angle2, 60.0));
	}
}

void ApolloRTCCMFD::GET_Display(char* Buff, double time, bool DispGET) //Display a time in the format hhh:mm:ss
{
	int hours, minutes;
	double seconds;

	OrbMech::SStoHHMMSS(time, hours, minutes, seconds);

	if (DispGET)
	{
		sprintf_s(Buff, 32, "%03d:%02d:%02.0f GET", hours, minutes, seconds);
	}
	else
	{
		sprintf_s(Buff, 32, "%03d:%02d:%02.0f", hours, minutes, seconds);
	}
}

void ApolloRTCCMFD::GMT_Display2(char* Buff, double time) const
{
	//Input value is GMT, display as GET like GET_Display2, but only if it is not zero
	if (time != 0.0)
	{
		GET_Display2(Buff, GC->rtcc->GETfromGMT(time));
	}
	else
	{
		GET_Display2(Buff, 0.0);
	}
}

void ApolloRTCCMFD::GET_Display2(char* Buff, double time) const //Display a time in the format hhh:mm:ss.ss
{
	bool pos = true;
	if (time < 0)
	{
		pos = false;
		time = abs(time);
	}

	int hours, minutes;
	double seconds;

	OrbMech::SStoHHMMSS(time, hours, minutes, seconds, 0.01);

	if (pos)
	{
		sprintf(Buff, "%03d:%02d:%05.2lf", hours, minutes, seconds);
	}
	else
	{
		sprintf(Buff, "-%03d:%02d:%05.2lf", hours, minutes, seconds);
	}
}

void ApolloRTCCMFD::GET_Display3(char* Buff, double time) //Display a time in the format hhh:mm:ss.s
{
	int hours, minutes;
	double seconds;

	OrbMech::SStoHHMMSS(time, hours, minutes, seconds, 0.1);

	sprintf(Buff, "%03d:%02d:%04.1f", hours, minutes, seconds);
}

//Format: HH:MM:SS
void ApolloRTCCMFD::GET_Display4(char* Buff, double time) //Display a time in the format hh:mm:ss
{
	int hours, minutes;
	double seconds;

	OrbMech::SStoHHMMSS(time, hours, minutes, seconds);

	sprintf(Buff, "%02d:%02d:%02.0f", hours, minutes, seconds);
}

//Format: HH:MM
void ApolloRTCCMFD::GET_Display_HHMM(char *Buff, double time)
{
	int hours, minutes;
	double seconds;

	OrbMech::SStoHHMMSS(time, hours, minutes, seconds, 60.0);

	sprintf(Buff, "%03d:%02d", hours, minutes);
}

void ApolloRTCCMFD::AGC_Display(char* Buff, double vel)
{
	//int velf;
	//velf = round(abs(vel));// *10.0));

	//if (vel >= 0)
	//{
		sprintf(Buff, "%+07.1f", vel);
	//}
	//else
	//{
	//	sprintf(Buff, "%+07.1f", vel);
	//}
}

void ApolloRTCCMFD::FormatLatitude(char * Buff, double lat)
{
	double iPart, fPart;
	fPart = modf(abs(lat), &iPart);

	//Rounding
	if (fPart*60.0 >= 59.5)
	{
		iPart += 1.0;
		fPart = 0.0;
	}

	if (lat >= 0)
	{
		sprintf_s(Buff, 64, "%02.0lf:%02.0lfN", iPart, fPart*60.0);
	}
	else
	{
		sprintf_s(Buff, 64, "%02.0lf:%02.0lfS", iPart, fPart*60.0);
	}
}

void ApolloRTCCMFD::FormatLongitude(char * Buff, double lng)
{
	while (lng >= 180.0)
	{
		lng -= 360.0;
	}
	while (lng < -180.0)
	{
		lng += 360.0;
	}

	double iPart, fPart;
	fPart = modf(abs(lng), &iPart);

	//Rounding
	if (fPart*60.0 >= 59.5)
	{
		iPart += 1.0;
		fPart = 0.0;
	}

	if (lng >= 0)
	{
		sprintf_s(Buff, 64, "%03.0lf:%02.0lfE", iPart, fPart*60.0);
	}
	else
	{
		sprintf_s(Buff, 64, "%03.0lf:%02.0lfW", iPart, fPart*60.0);
	}
}

void ApolloRTCCMFD::FormatIMUAngle0(char *Buff, double ang)
{
	ang *= DEG;
	if (ang >= 359.5)
	{
		ang = 0.0;
	}
	sprintf_s(Buff, 16, "%.0lf", ang);
}

void ApolloRTCCMFD::FormatIMUAngle1(char *Buff, double ang)
{
	ang *= DEG;
	if (ang >= 359.95)
	{
		ang = 0.0;
	}
	sprintf_s(Buff, 16, "%.1lf", ang);
}

void ApolloRTCCMFD::FormatIMUAngle2(char *Buff, double ang)
{
	ang *= DEG;
	if (ang >= 359.995)
	{
		ang = 0.0;
	}
	sprintf_s(Buff, 16, "%.2lf", ang);
}

void ApolloRTCCMFD::ThrusterName(char *Buff, int n)
{
	if (n == RTCC_ENGINETYPE_CSMSPS)
	{
		sprintf(Buff, "SPS");
	}
	else if (n == RTCC_ENGINETYPE_LMAPS)
	{
		sprintf(Buff, "APS");
	}
	else if (n == RTCC_ENGINETYPE_LMDPS)
	{
		sprintf(Buff, "DPS");
	}
	else if (n == RTCC_ENGINETYPE_CSMRCSPLUS2)
	{
		sprintf(Buff, "SM RCS 2+X");
	}
	else if (n == RTCC_ENGINETYPE_LMRCSPLUS2)
	{
		sprintf(Buff, "LM RCS 2+X");
	}
	else if (n == RTCC_ENGINETYPE_CSMRCSPLUS4)
	{
		sprintf(Buff, "SM RCS 4+X");
	}
	else if (n == RTCC_ENGINETYPE_LMRCSPLUS4)
	{
		sprintf(Buff, "LM RCS 4+X");
	}
	else if (n == RTCC_ENGINETYPE_CSMRCSMINUS2)
	{
		sprintf(Buff, "SM RCS 2-X");
	}
	else if (n == RTCC_ENGINETYPE_LMRCSMINUS2)
	{
		sprintf(Buff, "LM RCS 2-X");
	}
	else if (n == RTCC_ENGINETYPE_CSMRCSMINUS4)
	{
		sprintf(Buff, "SM RCS 4-X");
	}
	else if (n == RTCC_ENGINETYPE_LMRCSMINUS4)
	{
		sprintf(Buff, "LM RCS 4-X");
	}
	else if (n == RTCC_ENGINETYPE_LOX_DUMP)
	{
		sprintf(Buff, "S-IVB LOX");
	}
	else
	{
		sprintf(Buff, "");
	}
}

bool ApolloRTCCMFD::ThrusterType(std::string name, int &id)
{
	int temp = GC->rtcc->ThrusterNameToCode(name);
	if (temp > 0)
	{
		id = temp;
		return true;
	}
	return false;
}

void ApolloRTCCMFD::MPTAttitudeName(char *Buff, int n)
{
	if (n == RTCC_ATTITUDE_INERTIAL)
	{
		sprintf_s(Buff, 100, "Inertial");
	}
	else if (n == RTCC_ATTITUDE_MANUAL)
	{
		sprintf_s(Buff, 100, "Manual");
	}
	else if (n == RTCC_ATTITUDE_LAMBERT)
	{
		sprintf_s(Buff, 100, "Lambert (N/A)");
	}
	else if (n == RTCC_ATTITUDE_PGNS_EXDV)
	{
		sprintf_s(Buff, 100, "PGNS External DV");
	}
	else if (n == RTCC_ATTITUDE_AGS_EXDV)
	{
		sprintf_s(Buff, 100, "AGS External DV");
	}
	else
	{
		sprintf_s(Buff, 100, "");
	}
}

void ApolloRTCCMFD::REFSMMATName(char* Buff, int n)
{
	if (n == 0)
	{
		sprintf(Buff, "Preferred");
	}
	else if (n == 1)
	{
		sprintf(Buff, "Retrofire");
	}
	else if (n == 2)
	{
		sprintf(Buff, "Nominal");
	}
	else if (n == 3)
	{
		sprintf(Buff, "Entry");
	}
	else if (n == 4)
	{
		sprintf(Buff, "Launch");
	}
	else if (n == 5)
	{
		sprintf(Buff, "Landing Site");
	}
	else if (n == 6)
	{
		sprintf(Buff, "PTC");
	}
	else if (n == 7)
	{
		sprintf(Buff, "REFS from Att");
	}
	else if (n == 8)
	{
		sprintf(Buff, "Landing Site");
	}
	else
	{
		sprintf(Buff, "Unknown Type");
	}
}

void ApolloRTCCMFD::CycleREFSMMATopt()
{
	if (G->REFSMMATopt < 8)
	{
		G->REFSMMATopt++;
	}
	else
	{
		G->REFSMMATopt = 0;
	}
}

void ApolloRTCCMFD::SelectPage(int page)
{
	screen = page;
	coreButtons.SelectPage(this, screen);
	status = 0;
}

void ApolloRTCCMFD::menuSetMenu()
{
	SelectPage(0);
}

void ApolloRTCCMFD::menuSetLambertPage()
{
	SelectPage(1);
}

void ApolloRTCCMFD::menuSetTIMultipleSolutionPage()
{
	SelectPage(2);
}

void ApolloRTCCMFD::menuSetSPQPage()
{
	SelectPage(3);
}

void ApolloRTCCMFD::menuSetOrbAdjPage()
{
	marker = 0;
	markermax = 7;
	SelectPage(4);
}

void ApolloRTCCMFD::menuSetREFSMMATPage()
{
	SelectPage(5);
}

void ApolloRTCCMFD::menuSetReturnToEarthPage()
{
	SelectPage(6);
}

void ApolloRTCCMFD::menuSetAGSSVPage()
{
	SelectPage(7);
}

void ApolloRTCCMFD::menuSetConfigPage()
{
	SelectPage(8);
}

void ApolloRTCCMFD::menuSetManPADPage()
{
	SelectPage(9);
}

void ApolloRTCCMFD::menuSetEntryPADPage()
{
	SelectPage(10);
}

void ApolloRTCCMFD::menuSetMapUpdatePage()
{
	SelectPage(11);
}

void ApolloRTCCMFD::menuSetLOIPage()
{
	SelectPage(12);
}

void ApolloRTCCMFD::menuSetLandmarkTrkPage()
{
	SelectPage(13);
}

void ApolloRTCCMFD::menuSetTargetingMenu()
{
	SelectPage(14);
}

void ApolloRTCCMFD::menuSetVECPOINTPage()
{
	SelectPage(15);
}

void ApolloRTCCMFD::menuSetDescPlanCalcPage()
{
	SelectPage(16);
}

void ApolloRTCCMFD::menuTranslunarPage()
{
	SelectPage(17);
}

void ApolloRTCCMFD::menuSetDescPlanInitPage()
{
	SelectPage(18);
}

void ApolloRTCCMFD::menuSetTerrainModelPage()
{
	SelectPage(19);
}

void ApolloRTCCMFD::menuSetPADMenu()
{
	SelectPage(20);
}

void ApolloRTCCMFD::menuSetUtilityMenu()
{
	SelectPage(21);
}

void ApolloRTCCMFD::menuMidcoursePage()
{
	SelectPage(22);
}

void ApolloRTCCMFD::menuSetLunarLiftoffPage()
{
	SelectPage(23);
}

void ApolloRTCCMFD::menuSetNavCheckPADPage()
{
	SelectPage(25);
}

void ApolloRTCCMFD::menuSetDeorbitPage()
{
	SelectPage(26);
}

void ApolloRTCCMFD::menuSetRTEDigitalsInputPage()
{
	SelectPage(27);
}

void ApolloRTCCMFD::menuSetRTEDigitalsPage()
{
	SelectPage(28);
}

void ApolloRTCCMFD::menuSetRTEConstraintsPage()
{
	SelectPage(29);
}

void ApolloRTCCMFD::menuSetEntryUpdatePage()
{
	SelectPage(30);
}

void ApolloRTCCMFD::menuSetRTCCFilesPage()
{
	SelectPage(31);
}

void ApolloRTCCMFD::menuSetRendezvousPage()
{
	SelectPage(32);
}

void ApolloRTCCMFD::menuSetDKIPage()
{
	SelectPage(33);
}

void ApolloRTCCMFD::menuSetDKIOptionsPage()
{
	SelectPage(34);
}

void ApolloRTCCMFD::menuSetDAPPADPage()
{
	SelectPage(35);
}

void ApolloRTCCMFD::menuSetSaturnIBLVDCPage()
{
	SelectPage(121);
}

void ApolloRTCCMFD::menuSetSaturnVLVDCPage()
{
	SelectPage(36);
}

void ApolloRTCCMFD::menuSetLunarAscentPage()
{
	SelectPage(38);
}

void ApolloRTCCMFD::menuSetLMAscentPADPage()
{
	SelectPage(39);
}

void ApolloRTCCMFD::menuSetPDAPPage()
{
	SelectPage(40);
}

void ApolloRTCCMFD::menuSetFIDOOrbitDigitalsCSMPage()
{
	SelectPage(41);
}

void ApolloRTCCMFD::menuSetMCCDisplaysPage()
{
	SelectPage(42);
}

void ApolloRTCCMFD::menuSetSpaceDigitalsPage()
{
	SelectPage(43);

	//MSK request
	G->SpaceDigitalsMSKRequest();
}

void ApolloRTCCMFD::menuSetMPTPage()
{
	SelectPage(44);
}

void ApolloRTCCMFD::menuSetNextStationContactsPage()
{
	SelectPage(45);
}

void ApolloRTCCMFD::menuSetPredSiteAcquisitionCSM1Page()
{
	SelectPage(46);
}

void ApolloRTCCMFD::menuSetUplinkMenu()
{
	SelectPage(47);
}

void ApolloRTCCMFD::menuSetLSUpdateMenu()
{
	SelectPage(49);
}

void ApolloRTCCMFD::menuSetTITransferPage()
{
	SelectPage(54);
}

void ApolloRTCCMFD::menuSetSPQorDKIRTransferPage()
{
	if (screen == 3)
	{
		GC->rtcc->med_m70.Plan = 0;
	}
	else if (screen == 123)
	{
		GC->rtcc->med_m70.Plan = 1;
	}
	else
	{
		GC->rtcc->med_m70.Plan = -1;
	}

	SelectPage(55);
}

void ApolloRTCCMFD::menuSetMPTDirectInputPage()
{
	SelectPage(56);
}

void ApolloRTCCMFD::menuSetGPMTransferPage()
{
	SelectPage(57);
}

void ApolloRTCCMFD::menuSetCheckoutMonitorPage()
{
	SelectPage(58);
}

void ApolloRTCCMFD::menuSetMPTInitPage()
{
	SelectPage(59);

	GC->mptInitError = 0;
	marker = 0;
	markermax = 5;
}

void ApolloRTCCMFD::menuSetDescPlanTablePage()
{
	SelectPage(60);
}

void ApolloRTCCMFD::menuSetSunriseSunsetTablePage()
{
	SelectPage(62);
}

void ApolloRTCCMFD::menuSetMoonriseMoonsetTablePage()
{
	SelectPage(63);
}

void ApolloRTCCMFD::menuSetFIDOLaunchAnalogNo1Page()
{
	SelectPage(64);
}

void ApolloRTCCMFD::menuSetFIDOLaunchAnalogNo2Page()
{
	SelectPage(65);
}

void ApolloRTCCMFD::menuSetRTETradeoffDisplayPage()
{
	SelectPage(66);
}

void ApolloRTCCMFD::menuSetDetailedManeuverTableNo1Page()
{
	SelectPage(67);
}

void ApolloRTCCMFD::menuSetDetailedManeuverTableNo2Page()
{
	SelectPage(68);
}

void ApolloRTCCMFD::menuSetSPQInitializationPage()
{
	SelectPage(69);
}

void ApolloRTCCMFD::menuSetDKIInitializationPage()
{
	SelectPage(70);
}

void ApolloRTCCMFD::menuSetFIDOOrbitDigitalsLMPage()
{
	SelectPage(71);
}

void ApolloRTCCMFD::menuSetPredSiteAcquisitionLM1Page()
{
	SelectPage(72);
}

void ApolloRTCCMFD::menuSetPredSiteAcquisitionCSM2Page()
{
	SelectPage(73);
}

void ApolloRTCCMFD::menuSetPredSiteAcquisitionLM2Page()
{
	SelectPage(74);
}

void ApolloRTCCMFD::menuSetOnlineMonitorPage()
{
	SelectPage(75);
}

void ApolloRTCCMFD::menuLOITransferPage()
{
	GC->rtcc->med_m78.Type = true;
	SelectPage(76);
}

void ApolloRTCCMFD::menuMCCTransferPage()
{
	GC->rtcc->med_m78.Type = false;
	SelectPage(76);
}

void ApolloRTCCMFD::menuSetSkeletonFlightPlanPage()
{
	SelectPage(77);
}

void ApolloRTCCMFD::menuMidcourseTradeoffPage()
{
	SelectPage(78);
}

void ApolloRTCCMFD::menuTLIPlanningPage()
{
	marker = 0;
	markermax = 7;
	SelectPage(79);
}

void ApolloRTCCMFD::menuSetMidcourseConstraintsPage()
{
	SelectPage(80);
}

void ApolloRTCCMFD::menuSetNodalTargetConversionPage()
{
	SelectPage(81);
}

void ApolloRTCCMFD::menuSetLOIInitPage()
{
	SelectPage(82);
}

void ApolloRTCCMFD::menuSetLOIDisplayPage()
{
	SelectPage(83);
}

void ApolloRTCCMFD::menuMPTDirectInputSecondPage()
{
	SelectPage(84);
}

void ApolloRTCCMFD::menuSetExpSiteAcqPage()
{
	SelectPage(85);
}

void ApolloRTCCMFD::menuSetRelativeMotionDigitalsPage()
{
	SelectPage(86);
}

void ApolloRTCCMFD::menuSetRendezvousEvaluationDisplayPage()
{
	SelectPage(87);
}

void ApolloRTCCMFD::menuSetLaunchTargetingInitPage()
{
	SelectPage(88);
}

void ApolloRTCCMFD::menuSetLLWPInitPage()
{
	SelectPage(89);
}

void ApolloRTCCMFD::menuSetLLWPDisplayPage()
{
	SelectPage(90);
}

void ApolloRTCCMFD::menuSetLunarLaunchTargetingPage()
{
	SelectPage(91);
}

void ApolloRTCCMFD::menuSetTPITimesPage()
{
	SelectPage(92);
}

void ApolloRTCCMFD::menuSetVectorCompareDisplay()
{
	SelectPage(93);
}

void ApolloRTCCMFD::menuSetGuidanceOpticsSupportTablePage()
{
	SelectPage(95);
}

void ApolloRTCCMFD::menuVectorPanelSummaryPage()
{
	SelectPage(97);
}

void ApolloRTCCMFD::menuSetRetrofireConstraintsPage()
{
	SelectPage(103);
}

void ApolloRTCCMFD::menuSetRetrofireDigitalsPage()
{
	SelectPage(104);
}

void ApolloRTCCMFD::menuSetRetrofireXDVPage()
{
	SelectPage(105);
}

void ApolloRTCCMFD::menuSetRetrofireTargetSelectionPage()
{
	SelectPage(106);
}

void ApolloRTCCMFD::menuSetAbortScanTableInputPage()
{
	SelectPage(107);
}

void ApolloRTCCMFD::menuSetAbortScanTablePage()
{
	SelectPage(108);
}

void ApolloRTCCMFD::menuSetRTEDManualManeuverInputPage()
{
	SelectPage(109);
}

void ApolloRTCCMFD::menuSetRTEDEntryProfilePage()
{
	SelectPage(110);
}

void ApolloRTCCMFD::menuSetLunarTargetingProgramPage()
{
	SelectPage(115);
}

void ApolloRTCCMFD::menuSetRetrofireSeparationPage()
{
	SelectPage(116);
}

void ApolloRTCCMFD::menuSetRetrofireSeparationInputsPage()
{
	SelectPage(117);
}

void ApolloRTCCMFD::menuSetRetrofireSubsystemPage()
{
	SelectPage(118);
}

void ApolloRTCCMFD::menuSetEntryUplinkPage()
{
	SelectPage(119);
}

void ApolloRTCCMFD::menuSetLandmarkAcquisitionDisplayPage()
{
	SelectPage(120);
}

void ApolloRTCCMFD::menuSetLWPDisplayPage()
{
	SelectPage(122);
}

void ApolloRTCCMFD::menuSetRendezvousPlanningDisplayPage()
{
	SelectPage(123);
}

void ApolloRTCCMFD::menuSetPerigeeAdjustInputPage()
{
	SelectPage(124);
}

void ApolloRTCCMFD::menuSetPerigeeAdjustDisplayPage()
{
	SelectPage(125);
}

void ApolloRTCCMFD::menuSetLMOpticsSupportTablePage()
{
	SelectPage(126);
}

void ApolloRTCCMFD::menuSetDebugPage()
{
	SelectPage(127);
}

void ApolloRTCCMFD::menuSetAGOPPage()
{
	marker = 0;
	markermax = 17;
	SelectPage(128);
}

void ApolloRTCCMFD::menuSetRTACFPage()
{
	SelectPage(129);
}

void ApolloRTCCMFD::menuSetIMUParkingAnglesPage()
{
	SelectPage(131);
}

void ApolloRTCCMFD::menuPerigeeAdjustCalc()
{
	G->PerigeeAdjustCalc();
}

void ApolloRTCCMFD::CyclePerigeeAdjustVehicle()
{
	if (GC->rtcc->med_k28.VEH == RTCC_MPT_CSM)
	{
		GC->rtcc->med_k28.VEH = RTCC_MPT_LM;
	}
	else
	{
		GC->rtcc->med_k28.VEH = RTCC_MPT_CSM;
	}
}

void ApolloRTCCMFD::menuPerigeeAdjustVectorTime()
{
	if (GC->MissionPlanningActive)
	{
		GenericGETInput(&GC->rtcc->med_k28.VectorTime, "Enter desired vector GET (Format: HH:MM:SS)");
	}
	else
	{
		if (GC->rtcc->med_k28.VEH == RTCC_MPT_CSM)
		{
			set_CSMVessel();
		}
		else
		{
			set_LMVessel();
		}
	}
}

void ApolloRTCCMFD::menuPerigeeAdjustThresholdTime()
{
	GenericGETInput(&GC->rtcc->med_k28.ThresholdTime, "Enter desired threshold GET (Format: HH:MM:SS)");
}

void ApolloRTCCMFD::menuPerigeeAdjustTimeIncrement()
{
	GenericDoubleInput(&GC->rtcc->med_k28.TimeIncrement, "Time increment in seconds:", 1.0);
}

void ApolloRTCCMFD::menuPerigeeAdjustHeight()
{
	GenericDoubleInput(&GC->rtcc->med_k28.H_P, "Perigee height in nautical miles:", 1.0);
}

void ApolloRTCCMFD::menuCycleAGOPPage()
{
	if (GC->AGOP_Page == 1)
	{
		GC->AGOP_Page = 2;
	}
	else
	{
		GC->AGOP_Page = 1;
	}
}

void ApolloRTCCMFD::menuSetAGOPInput()
{
	switch (marker)
	{
	case 0: //Cycle option
		GC->AGOP_Mode = 1;
		GC->AGOP_AdditionalOption = 0;
		if (GC->AGOP_Option < 7)
		{
			GC->AGOP_Option++;
		}
		else
		{
			GC->AGOP_Option = 1;
		}
		break;
	case 1: //Cycle mode
		GC->AGOP_AdditionalOption = 0;
		switch (GC->AGOP_Option)
		{
		case 1:
			if (GC->AGOP_Mode < 4)
			{
				GC->AGOP_Mode++;
			}
			else
			{
				GC->AGOP_Mode = 1;
			}
			break;
		case 2:
		case 4:
			if (GC->AGOP_Mode < 6)
			{
				GC->AGOP_Mode++;
			}
			else
			{
				GC->AGOP_Mode = 1;
			}
			break;
		case 6:
			if (GC->AGOP_Mode < 2)
			{
				GC->AGOP_Mode++;
			}
			else
			{
				GC->AGOP_Mode = 1;
			}
			break;
		case 7:
			if (GC->AGOP_Mode < 6)
			{
				GC->AGOP_Mode++;
			}
			else
			{
				GC->AGOP_Mode = 1;
			}
			break;
		case 8:
			if (GC->AGOP_Mode < 5)
			{
				GC->AGOP_Mode++;
			}
			else
			{
				GC->AGOP_Mode = 1;
			}
			break;
		}
		break;
	case 2:
		if (GC->AGOP_Option == 7 && (GC->AGOP_Mode == 3 || GC->AGOP_Mode == 4))
		{
			//Additional option
			if (GC->AGOP_Mode == 3)
			{
				if (GC->AGOP_AdditionalOption < 1)
				{
					GC->AGOP_AdditionalOption++;
				}
				else
				{
					GC->AGOP_AdditionalOption = 0;
				}
			}
			else
			{
				if (GC->AGOP_AdditionalOption < 3)
				{
					GC->AGOP_AdditionalOption++;
				}
				else
				{
					GC->AGOP_AdditionalOption = 0;
				}
			}
		}
		else
		{
			//Input start time
			GenericGETInput(&GC->AGOP_StartTime, "Enter desired start GET (Format: HH:MM:SS)");
		}
		break;
	case 3: //Input stop time
		GenericGETInput(&GC->AGOP_StopTime, "Enter desired stop GET (Format: HH:MM:SS)");
		break;
	case 4: //Input time step
		GenericDoubleInput(&GC->AGOP_TimeStep, "Enter desired time step in minutes");
		break;
	case 5: //CSM REFSMMAT
		if (GC->AGOP_CSM_REFSMMAT < 11)
		{
			GC->AGOP_CSM_REFSMMAT++;
		}
		else
		{
			GC->AGOP_CSM_REFSMMAT = 1;
		}
		break;
	case 6: //LM REFSMMAT
		if (GC->AGOP_LM_REFSMMAT < 11)
		{
			GC->AGOP_LM_REFSMMAT++;
		}
		else
		{
			GC->AGOP_LM_REFSMMAT = 1;
		}
		break;
	case 7: //Input star
		if (GC->AGOP_Option == 1 || GC->AGOP_Option == 3 || (GC->AGOP_Option == 7 && GC->AGOP_Mode == 5))
		{
			GenericIntInput(&GC->AGOP_Stars[0], "Enter star ID in decimal format (1-400):", NULL, 1, 400);
		}
		else if (GC->AGOP_Option == 6)
		{
			GC->AGOP_HeadsUp = !GC->AGOP_HeadsUp;
		}
		else if (GC->AGOP_Option == 7 && GC->AGOP_Mode == 3)
		{
			GenericInt2Input(&GC->AGOP_Stars[0], &GC->AGOP_Stars[1], "Enter two star IDs in decimal format (1-400):", 1, 400, 1, 400, NULL);
		}
		break;
	case 8: //Landmark latitude
		if ((GC->AGOP_Option == 1 && (GC->AGOP_Mode == 3 || GC->AGOP_Mode == 4)) || GC->AGOP_Option == 4)
		{
			GenericDoubleInput(&GC->AGOP_Lat, "Enter landmark latitude in degrees:", RAD);
		}
		break;
	case 9: //Landmark longitude
		if ((GC->AGOP_Option == 1 && (GC->AGOP_Mode == 3 || GC->AGOP_Mode == 4)) || GC->AGOP_Option == 4)
		{
			GenericDoubleInput(&GC->AGOP_Lng, "Enter landmark longitude in degrees:", RAD);
		}
		break;
	case 10: //Landmark altitude
		if ((GC->AGOP_Option == 1 && (GC->AGOP_Mode == 3 || GC->AGOP_Mode == 4)) || GC->AGOP_Option == 4)
		{
			GenericDoubleInput(&GC->AGOP_Alt, "Enter landmark height in nautical miles:", 1852.0);
		}
		break;
	case 11: //CSM vs LM IMU
		if (GC->AGOP_Option == 4 || GC->AGOP_Option == 7)
		{
			GC->AGOP_AttIsCSM = !GC->AGOP_AttIsCSM;
		}
		break;
	case 12: //IMU Angles
		GenericVectorInput(&GC->AGOP_Attitudes[0], "Input IMU angles in degrees:", RAD, NULL);
		break;
	case 13: //IMU Angles (second set for option 7, mode 3)
		GenericVectorInput(&GC->AGOP_Attitudes[1], "Input IMU angles in degrees:", RAD, NULL);
		break;
	case 14: //Antenna Pitch
		if (GC->AGOP_Option == 7 && GC->AGOP_Mode == 3)
		{
			GenericDouble2Input(&GC->AGOP_InstrumentAngles1[0], &GC->AGOP_InstrumentAngles1[1], "Input instrument angles:", RAD, RAD);
		}
		else
		{
			GenericDoubleInput(&GC->AGOP_AntennaPitch, "Input antenna pitch angle in degrees:", RAD);
		}
		break;
	case 15: //Antenna Yaw
		if (GC->AGOP_Option == 7 && GC->AGOP_Mode == 3)
		{
			GenericDouble2Input(&GC->AGOP_InstrumentAngles2[0], &GC->AGOP_InstrumentAngles2[1], "Input instrument angles:", RAD, RAD);
		}
		else
		{
			GenericDoubleInput(&GC->AGOP_AntennaYaw, "Input antenna yaw angle in degrees:", RAD);
		}
		break;
	case 16: //Instrument
		if (GC->AGOP_Instrument < 3)
		{
			GC->AGOP_Instrument++;
		}
		else
		{
			GC->AGOP_Instrument = 0;
		}
		break;
	case 17: //LM COAS Axis
		if (GC->AGOP_Instrument == 1)
		{
			GC->AGOP_LMCOASAxis = !GC->AGOP_LMCOASAxis;
		}
		else if (GC->AGOP_Instrument == 2)
		{
			if (GC->AGOP_LMAOTDetent < 6)
			{
				GC->AGOP_LMAOTDetent++;
			}
			else
			{
				GC->AGOP_LMAOTDetent = 1;
			}
		}
		break;
	}
}

void ApolloRTCCMFD::menuAGOPCalc()
{
	GC->AGOP_Page = 2;
	G->startSubthread(51);
}

void ApolloRTCCMFD::menuAGOPSaveREFSMMAT()
{
	//If it is still zero then a REFSMMAT hasn't been saved yet
	if (GC->AGOP_REFSMMAT_Vehicle == 0) return;

	//Call RTCC function to save the REFSMMAT in the OST slot
	GC->rtcc->EMGSTSTM(GC->AGOP_REFSMMAT_Vehicle, GC->AGOP_REFSMMAT, RTCC_REFSMMAT_TYPE_OST, GC->rtcc->RTCCPresentTimeGMT());
}

void ApolloRTCCMFD::menuLWPLiftoffTimeOption()
{
	if (GC->rtcc->PZSLVCON.LOT < 6)
	{
		GC->rtcc->PZSLVCON.LOT++;
	}
	else
	{
		GC->rtcc->PZSLVCON.LOT = 1;
	}
}

void ApolloRTCCMFD::menuLWPLiftoffTime()
{
	GenericGETInput(&GC->rtcc->PZSLVCON.GMTLOR, "Enter desired GMT of liftoff (Format: HH:MM:SS)");
}

void ApolloRTCCMFD::LUNTAR_TIGInput()
{
	GenericGETInput(&G->LUNTAR_TIG, "Enter GET (Format: HH:MM:SS). Enter zero for trajectory evaluation (no maneuver):");
}

void ApolloRTCCMFD::menuLWP_RINS()
{
	GenericDoubleInput(&GC->rtcc->PZSLVCON.RINS, "Radius of insertion in meters:", 1.0);
}

void ApolloRTCCMFD::menuLWP_VINS()
{
	GenericDoubleInput(&GC->rtcc->PZSLVCON.VINS, "Velocity of insertion in m/s:", 1.0);
}

void ApolloRTCCMFD::menuLWP_GAMINS()
{
	GenericDoubleInput(&GC->rtcc->PZSLVCON.GAMINS, "Flight-path angle of insertion in degrees:", RAD);
}

void ApolloRTCCMFD::menuLWPCycleDELNOF()
{
	GC->rtcc->PZSLVCON.DELNOF = !GC->rtcc->PZSLVCON.DELNOF;
}

void ApolloRTCCMFD::menuLWP_DELNO()
{
	GenericDoubleInput(&GC->rtcc->PZSLVCON.DELNO, "Differential nodal regression in degrees:", RAD);
}

void ApolloRTCCMFD::menuLWP_PhaseFlags()
{
	if (GC->rtcc->PZSLVCON.NEGTIV == 2 && GC->rtcc->PZSLVCON.WRAP == 0)
	{
		GC->rtcc->PZSLVCON.NEGTIV = 0;
		GC->rtcc->PZSLVCON.WRAP = 0;
	}
	else if (GC->rtcc->PZSLVCON.NEGTIV == 0 && GC->rtcc->PZSLVCON.WRAP == 0)
	{
		GC->rtcc->PZSLVCON.NEGTIV = 2;
		GC->rtcc->PZSLVCON.WRAP = 1;
	}
	else if (GC->rtcc->PZSLVCON.NEGTIV == 2 && GC->rtcc->PZSLVCON.WRAP == 1)
	{
		GC->rtcc->PZSLVCON.NEGTIV = 0;
		GC->rtcc->PZSLVCON.WRAP = 1;
	}
	else if (GC->rtcc->PZSLVCON.NEGTIV == 0 && GC->rtcc->PZSLVCON.WRAP == 1)
	{
		GC->rtcc->PZSLVCON.NEGTIV = 2;
		GC->rtcc->PZSLVCON.WRAP = 2;
	}
	else
	{
		GC->rtcc->PZSLVCON.NEGTIV = 2;
		GC->rtcc->PZSLVCON.WRAP = 0;
	}
}

void ApolloRTCCMFD::LUNTAR_BTInput()
{
	GenericDoubleInput(&G->LUNTAR_bt_guess, "Enter burn time in seconds:", 1.0);
}

void ApolloRTCCMFD::LUNTAR_PitchInput()
{
	GenericDoubleInput(&G->LUNTAR_pitch_guess, "Enter estimated pitch in degrees:", RAD);
}

void ApolloRTCCMFD::LUNTAR_YawInput()
{
	GenericDoubleInput(&G->LUNTAR_yaw_guess, "Enter estimated yaw in degrees:", RAD);
}

void ApolloRTCCMFD::LUNTAR_LatInput()
{
	GenericDoubleInput(&G->LUNTAR_lat, "Enter desired impact latitude in degrees:", RAD);
}

void ApolloRTCCMFD::LUNTAR_LngInput()
{
	GenericDoubleInput(&G->LUNTAR_lng, "Enter desired impact longitude in degrees:", RAD);
}

void ApolloRTCCMFD::LUNTARCalc()
{
	G->LUNTARCalc();
}

void ApolloRTCCMFD::menuRetroShapingGET()
{
	GenericGETInput(&GC->rtcc->RZJCTTC.R30_GETI_SH, "Enter GET of shaping burn (Format: HH:MM:SS). Enter 0:0:0 for no shaping burn.");
}

void ApolloRTCCMFD::menuRetroSepDeltaTTIG()
{
	GenericDoubleInput(&GC->rtcc->RZJCTTC.R30_DeltaT_Sep, "Enter Delta T of separation maneuver in minutes:", 60.0);
}

void ApolloRTCCMFD::menuRetroSepThruster()
{
	bool ChooseRetroSepThrusterInput(void *id, char *str, void *data);
	oapiOpenInputBox("Thruster for the sep or shaping maneuver (Options: S or C1-C4)", ChooseRetroSepThrusterInput, 0, 20, (void*)this);
}

bool ChooseRetroSepThrusterInput(void *id, char *str, void *data)
{
	std::string th(str);
	return ((ApolloRTCCMFD*)data)->set_RetroSepThruster(th);
}

bool ApolloRTCCMFD::set_RetroSepThruster(std::string th)
{
	return ThrusterType(th, GC->rtcc->RZJCTTC.R30_Thruster);
}

void ApolloRTCCMFD::menuRetroSepDeltaT()
{
	GenericDoubleInput(&GC->rtcc->RZJCTTC.R30_DeltaT, "Enter burn time of separation maneuver in seconds (only choose DT or DV, not both):", 1.0);
}

void ApolloRTCCMFD::menuRetroSepDeltaV()
{
	GenericDoubleInput(&GC->rtcc->RZJCTTC.R30_DeltaV, "Enter Delta V of separation maneuver in ft/s (only choose DT or DV, not both):", 0.3048);
}

void ApolloRTCCMFD::menuRetroSepUllageDT()
{
	GenericDoubleInput(&GC->rtcc->RZJCTTC.R30_Ullage_DT, "Enter ullage time in seconds:", 1.0);
}

void ApolloRTCCMFD::menuRetroSepUllageThrusters()
{
	GC->rtcc->RZJCTTC.R30_Use4UllageThrusters = !GC->rtcc->RZJCTTC.R30_Use4UllageThrusters;
}

void ApolloRTCCMFD::menuRetroSepGimbalIndicator()
{
	if (GC->rtcc->RZJCTTC.R30_GimbalIndicator == 1)
	{
		GC->rtcc->RZJCTTC.R30_GimbalIndicator = -1;
	}
	else
	{
		GC->rtcc->RZJCTTC.R30_GimbalIndicator = 1;
	}
}

void ApolloRTCCMFD::menuRetroSepAtt()
{
	GenericVectorInput(&GC->rtcc->RZJCTTC.R30_Att, "Enter attitude of sep/shaping maneuver:", RAD);
}

void ApolloRTCCMFD::GenericGETInput(double *get, char *message)
{
	bool GenericGETInputBox(void *id, char *str, void *data);
	oapiOpenInputBox(message, GenericGETInputBox, 0, 25, (void*)(get));
}

bool GenericGETInputBox(void *id, char *str, void *data)
{
	double *get2 = static_cast<double*>(data);

	int hh, mm;
	double ss, get;

	if (sscanf(str, "%d:%d:%lf", &hh, &mm, &ss) == 3)
	{
		get = ss + 60 * (mm + 60 * hh);
		*get2 = get;
		
		return true;

	}
	return false;
}

void ApolloRTCCMFD::GenericDoubleInput(double *val, char *message, double factor)
{
	void *data2;

	tempData.dVal = val;
	tempData.factor = factor;
	data2 = &tempData;

	bool GenericDoubleInputBox(void *id, char *str, void *data);
	oapiOpenInputBox(message, GenericDoubleInputBox, 0, 25, data2);
}

bool GenericDoubleInputBox(void *id, char *str, void *data)
{
	RTCCMFDInputBoxData *arr = static_cast<RTCCMFDInputBoxData*>(data);
	double val;

	if (sscanf(str, "%lf", &val) == 1)
	{
		*arr->dVal = val * arr->factor;
		return true;
	}
	return false;
}

void ApolloRTCCMFD::GenericDouble2Input(double *val1, double *val2, char* message, double factor1, double factor2)
{
	void *data2;

	tempData.dVal = val1;
	tempData.dVal2 = val2;
	tempData.factor = factor1;
	tempData.factor2 = factor2;
	data2 = &tempData;

	bool GenericDouble2InputBox(void *id, char *str, void *data);
	oapiOpenInputBox(message, GenericDouble2InputBox, 0, 30, data2);
}

bool GenericDouble2InputBox(void *id, char *str, void *data)
{
	RTCCMFDInputBoxData *arr = static_cast<RTCCMFDInputBoxData*>(data);
	double val1, val2;

	if (sscanf(str, "%lf %lf", &val1, &val2) == 2)
	{
		*arr->dVal = val1 * arr->factor;
		*arr->dVal2 = val2 * arr->factor2;
		return true;
	}
	return false;
}

void ApolloRTCCMFD::GenericIntInput(int *val, char *message, void (ApolloRTCCMFD::*func)(void), int min, int max)
{
	void *data2;

	tempData.iVal = val;
	tempData.min1 = min;
	tempData.max1 = max;
	tempData.ptr = this;
	tempData.func = func;
	data2 = &tempData;

	bool GenericIntInputBox(void *id, char *str, void *data);
	oapiOpenInputBox(message, GenericIntInputBox, 0, 25, data2);
}

bool GenericIntInputBox(void *id, char *str, void *data)
{
	RTCCMFDInputBoxData *arr = static_cast<RTCCMFDInputBoxData*>(data);
	int val;

	if (sscanf(str, "%d", &val) == 1)
	{
		if (arr->max1 > arr->min1) //By default this check is disabled
		{
			if (val < arr->min1) return false;
			if (val > arr->max1) return false;
		}

		*arr->iVal = val;

		if (arr->func)
		{
			ApolloRTCCMFD *ptr = arr->ptr;
			void (ApolloRTCCMFD::*func)(void) = arr->func;

			(ptr->*func)();
		}
		return true;
	}
	return false;
}

void ApolloRTCCMFD::GenericInt2Input(int *val1, int *val2, char* message, int min1, int max1, int min2, int max2, void (ApolloRTCCMFD::*func)(void))
{
	void *data2;

	tempData.iVal = val1;
	tempData.iVal2 = val2;
	tempData.min1 = min1;
	tempData.max1 = max1;
	tempData.min2 = min2;
	tempData.max2 = max2;
	tempData.ptr = this;
	tempData.func = func;
	data2 = &tempData;

	bool GenericInt2InputBox(void *id, char *str, void *data);
	oapiOpenInputBox(message, GenericInt2InputBox, 0, 25, data2);
}

bool GenericInt2InputBox(void *id, char *str, void *data)
{
	RTCCMFDInputBoxData *arr = static_cast<RTCCMFDInputBoxData*>(data);
	int val1, val2;

	if (sscanf(str, "%d %d", &val1, &val2) == 2)
	{
		if (val1 < arr->min1) return false;
		if (val1 > arr->max1) return false;
		if (val2 < arr->min2) return false;
		if (val2 > arr->max2) return false;

		*arr->iVal = val1;
		*arr->iVal2 = val2;

		if (arr->func)
		{
			ApolloRTCCMFD *ptr = arr->ptr;
			void (ApolloRTCCMFD::*func)(void) = arr->func;

			(ptr->*func)();
		}
		return true;
	}
	return false;
}

void ApolloRTCCMFD::GenericVectorInput(VECTOR3 *val, char* message, double factor, void(ApolloRTCCMFD::*func)(void))
{
	void *data2;

	tempData.vVal = val;
	tempData.factor = factor;
	tempData.ptr = this;
	tempData.func = func;
	data2 = &tempData;

	bool GenericVectorInputBox(void *id, char *str, void *data);
	oapiOpenInputBox(message, GenericVectorInputBox, 0, 25, data2);
}

bool GenericVectorInputBox(void *id, char *str, void *data)
{
	RTCCMFDInputBoxData *arr = static_cast<RTCCMFDInputBoxData*>(data);
	double val1, val2, val3;

	if (sscanf(str, "%lf %lf %lf", &val1, &val2, &val3) == 3)
	{
		arr->vVal->x = val1 * arr->factor;
		arr->vVal->y = val2 * arr->factor;
		arr->vVal->z = val3 * arr->factor;

		if (arr->func)
		{
			ApolloRTCCMFD *ptr = arr->ptr;
			void (ApolloRTCCMFD::*func)(void) = arr->func;

			(ptr->*func)();
		}
		return true;
	}
	return false;
}

void ApolloRTCCMFD::GenericStringInput(std::string *val, char* message, void(ApolloRTCCMFD::*func)(void))
{
	void *data2;

	tempData.sVal = val;
	tempData.ptr = this;
	tempData.func = func;

	data2 = &tempData;

	bool GenericStringInputBox(void *id, char *str, void *data);
	oapiOpenInputBox(message, GenericStringInputBox, 0, 25, data2);
}

bool GenericStringInputBox(void *id, char *str, void *data)
{
	RTCCMFDInputBoxData *arr = static_cast<RTCCMFDInputBoxData*>(data);

	arr->sVal->assign(str);

	if (arr->func)
	{
		ApolloRTCCMFD *ptr = arr->ptr;
		void (ApolloRTCCMFD::*func)(void) = arr->func;

		(ptr->*func)();
	}

	return true;
}

void ApolloRTCCMFD::menuCycleRecoveryTargetSelectionPages()
{
	if (GC->rtcc->RZDRTSD.CurrentPage < GC->rtcc->RZDRTSD.TotalNumPages)
	{
		GC->rtcc->RZDRTSD.CurrentPage++;
	}
	else
	{
		GC->rtcc->RZDRTSD.CurrentPage = 1;
	}
}

void ApolloRTCCMFD::menuRecoveryTargetSelectionCalc()
{
	bool RecoveryTargetSelectionCalcInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose starting time and longitude (Format: HH:MM:SS XXX.X)", RecoveryTargetSelectionCalcInput, 0, 20, (void*)this);
}

bool RecoveryTargetSelectionCalcInput(void *id, char *str, void *data)
{
	int hh, mm;
	double ss, get, lng;

	if (sscanf(str, "%d:%d:%lf %lf", &hh, &mm, &ss, &lng) == 4)
	{
		get = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_RecoveryTargetSelectionCalc(get, lng);

		return true;

	}
	return false;
}

void ApolloRTCCMFD::set_RecoveryTargetSelectionCalc(double get, double lng)
{
	GC->rtcc->RZJCTTC.R20_GET = get;
	GC->rtcc->RZJCTTC.R20_lng = lng * RAD;
	G->RecoveryTargetSelectionCalc();
}

void ApolloRTCCMFD::menuSelectRecoveryTarget()
{
	bool SelectRecoveryTargetInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose recovery target (1-40):", SelectRecoveryTargetInput, 0, 20, (void*)this);
}

bool SelectRecoveryTargetInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		return ((ApolloRTCCMFD*)data)->set_RecoveryTarget(atoi(str));
	}
	return false;
}

bool ApolloRTCCMFD::set_RecoveryTarget(int num)
{
	if (num >= 1 && num <= 40)
	{
		if (GC->rtcc->RZDRTSD.table[num - 1].DataIndicator == false)
		{
			GC->rtcc->RZJCTTC.R32_lat_T = GC->rtcc->RZDRTSD.table[num - 1].Latitude*RAD;
			GC->rtcc->RZJCTTC.R32_lng_T = GC->rtcc->RZDRTSD.table[num - 1].Longitude*RAD;
			if (GC->rtcc->RZJCTTC.R31_Thruster == RTCC_ENGINETYPE_CSMSPS)
			{
				GC->rtcc->RZJCTTC.R32_GETI = GC->rtcc->RZDRTSD.table[num - 1].GET - 20.0*60.0;
			}
			else
			{
				GC->rtcc->RZJCTTC.R32_GETI = GC->rtcc->RZDRTSD.table[num - 1].GET - 30.0*60.0;
			}
			
			return true;
		}
	}
	return false;
}

void ApolloRTCCMFD::menuSaveDODREFSMMAT()
{
	GeneralMEDRequest("G11,CSM,DOM;");
}

void ApolloRTCCMFD::menuSaveRTEREFSMMAT()
{
	bool SaveRTEREFSMMATInput(void *id, char *str, void *data);
	oapiOpenInputBox("Save RTE REFSMMAT. P for primary or M for manual column.", SaveRTEREFSMMATInput, 0, 20, (void*)this);
}

bool SaveRTEREFSMMATInput(void *id, char *str, void *data)
{
	if (strlen(str) < 3)
	{
		char *code;
		if (str[0] == 'P')
		{
			code = "REP";
		}
		else if (str[0] == 'M')
		{
			code = "REM";
		}
		else
		{
			return false;
		}
		char Buff[128];
		sprintf(Buff, "G11,CSM,%s;", code);
		((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buff);

		return true;
	}

	return false;
}

void ApolloRTCCMFD::menuMakeDODREFSMMATCurrent()
{
	GeneralMEDRequest("G00,CSM,DOD,CSM,CUR;");
}

void ApolloRTCCMFD::menuSetRetrofireMissDistance()
{
	bool RetrofireMissDistanceInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the maximum miss distance in NM (0-1500):", RetrofireMissDistanceInput, 0, 20, (void*)this);
}

bool RetrofireMissDistanceInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		return ((ApolloRTCCMFD*)data)->set_RetrofireMissDistance(atof(str));
	}
	return false;
}

bool ApolloRTCCMFD::set_RetrofireMissDistance(double val)
{
	if (val >= 0 && val <= 1500)
	{
		GC->rtcc->RZJCTTC.R32_MD = val;
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuVoid() {}

void ApolloRTCCMFD::menuCycleRTETradeoffPage()
{
	if (subscreen < 5)
	{
		subscreen++;
	}
	else
	{
		subscreen = 0;
	}
}

void ApolloRTCCMFD::menuCalcRTETradeoff()
{
	G->RTETradeoffDisplayCalc();
}

void ApolloRTCCMFD::menuSetRTETradeoffSite()
{
	bool RTETradeoffSiteInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the landing site from target table:", RTETradeoffSiteInput, 0, 20, (void*)this);
}

bool RTETradeoffSiteInput(void *id, char *str, void *data)
{
	if (strlen(str) < 7)
	{
		std::string buf(str);
		((ApolloRTCCMFD*)data)->set_TradeoffSiteInput(buf);

		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_TradeoffSiteInput(const std::string &site)
{
	if (G->RTETradeoffMode == 0)
	{
		GC->rtcc->med_f70.Site = site;
	}
	else
	{
		GC->rtcc->med_f71.Site = site;
	}
}

void ApolloRTCCMFD::menuSetRTETradeoffRemoteEarthPage()
{
	bool RTETradeoffRemoteEarthPageInput(void *id, char *str, void *data);
	oapiOpenInputBox("Page for the Remote-Earth tradeoff display (1-5):", RTETradeoffRemoteEarthPageInput, 0, 20, (void*)this);
}

bool RTETradeoffRemoteEarthPageInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_RTETradeoffRemoteEarthPage(atoi(str));
		return true;

	}
	return false;
}

void ApolloRTCCMFD::set_RTETradeoffRemoteEarthPage(int page)
{
	GC->rtcc->med_f71.Page = page;
}

void ApolloRTCCMFD::menuSetRTETradeoffVectorTime()
{
	bool RTETradeoffVectorTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the vector time in GET (Format: hhh:mm:ss)", RTETradeoffVectorTimeInput, 0, 20, (void*)this);
}

bool RTETradeoffVectorTimeInput(void *id, char *str, void *data)
{
	int hh, mm, ss, get;

	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		get = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_RTETradeoffVectorTime(get);

		return true;

	}
	return false;
}

void ApolloRTCCMFD::set_RTETradeoffVectorTime(double tv)
{
	if (G->RTETradeoffMode == 0)
	{
		GC->rtcc->med_f70.T_V = tv / 3600.0;
	}
	else
	{
		GC->rtcc->med_f71.T_V = tv / 3600.0;
	}
}

void ApolloRTCCMFD::menuSetRTETradeoffT0MinTime()
{
	bool RTETradeoffT0MinTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the minimum abort time in GET (Format: hhh:mm:ss)", RTETradeoffT0MinTimeInput, 0, 20, (void*)this);
}

bool RTETradeoffT0MinTimeInput(void *id, char *str, void *data)
{
	int hh, mm, ss, get;

	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		get = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_RTETradeoffT0MinTime(get);

		return true;

	}
	return false;
}

void ApolloRTCCMFD::set_RTETradeoffT0MinTime(double get)
{
	if (G->RTETradeoffMode == 0)
	{
		GC->rtcc->med_f70.T_omin = get / 3600.0;
	}
	else
	{
		GC->rtcc->med_f71.T_omin = get / 3600.0;
	}
}

void ApolloRTCCMFD::menuSetRTETradeoffT0MaxTime()
{
	bool RTETradeoffT0MaxTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the maximum abort time in GET (Format: hhh:mm:ss)", RTETradeoffT0MaxTimeInput, 0, 20, (void*)this);
}

bool RTETradeoffT0MaxTimeInput(void *id, char *str, void *data)
{
	int hh, mm, ss, get;

	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		get = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_RTETradeoffT0MaxTime(get);

		return true;

	}
	return false;
}

void ApolloRTCCMFD::set_RTETradeoffT0MaxTime(double get)
{
	if (G->RTETradeoffMode == 0)
	{
		GC->rtcc->med_f70.T_omax = get / 3600.0;
	}
	else
	{
		GC->rtcc->med_f71.T_omax = get / 3600.0;
	}
}

void ApolloRTCCMFD::menuSetRTETradeoffEntryProfile()
{
	if (G->RTETradeoffMode == 0)
	{
		if (GC->rtcc->med_f70.EntryProfile < 2)
		{
			GC->rtcc->med_f70.EntryProfile++;
		}
		else
		{
			GC->rtcc->med_f70.EntryProfile = 1;
		}
	}
	else
	{
		if (GC->rtcc->med_f71.EntryProfile < 2)
		{
			GC->rtcc->med_f71.EntryProfile++;
		}
		else
		{
			GC->rtcc->med_f71.EntryProfile = 1;
		}
	}	
}

void ApolloRTCCMFD::menuSetRTETradeoffMode()
{
	if (G->RTETradeoffMode < 1)
	{
		G->RTETradeoffMode++;
	}
	else
	{
		G->RTETradeoffMode = 0;
	}
}

void ApolloRTCCMFD::menuCycleASTType()
{
	if (G->RTEASTType < 77)
	{
		G->RTEASTType++;
	}
	else
	{
		G->RTEASTType = 75;
	}
}

void ApolloRTCCMFD::menuSetASTSiteOrType()
{
	bool ASTSiteOrTypeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Unspecified area: Enter FCUA or TCUA for fuel/time critical return. Specific site: Enter name of site (e.g. MPL). Lunar Search: Site or FCUA.", ASTSiteOrTypeInput, 0, 20, (void*)this);
}

bool ASTSiteOrTypeInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->set_ASTSiteOrType(str);
	return true;
}

void ApolloRTCCMFD::set_ASTSiteOrType(char *site)
{
	if (G->RTEASTType == 75)
	{
		GC->rtcc->med_f75.Type.assign(site);
	}
	else if (G->RTEASTType == 76)
	{
		GC->rtcc->med_f76.Site.assign(site);
	}
	else
	{
		GC->rtcc->med_f77.Site.assign(site);
	}
}

void ApolloRTCCMFD::menuASTVectorTime()
{
	if (GC->MissionPlanningActive)
	{
		GenericGETInput(&GC->rtcc->med_f75_f77.T_V, "Choose the vector GET (Format: hhh:mm:ss)");
	}
	else
	{
		set_CSMVessel();
	}
}

void ApolloRTCCMFD::menuASTAbortTime()
{
	if (G->RTEASTType == 77)
	{
		GenericGETInput(&GC->rtcc->med_f75_f77.T_0_min, "Choose minimum abort time: (Format: hhh:mm:ss)");
	}
	else
	{
		GenericGETInput(&GC->rtcc->med_f75_f77.T_0_min, "Choose abort time: (Format: hhh:mm:ss)");
	}
}

void ApolloRTCCMFD::menuASTTMAXandDVInput()
{
	if (G->RTEASTType == 77)
	{
		GenericGETInput(&GC->rtcc->med_f77.T_max, "Choose maximum abort time: (Format: hhh:mm:ss)");
	}
	else
	{
		bool ASTTMAXandDVInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the maximum abort DV:", ASTTMAXandDVInput, 0, 25, (void*)this);
	}
}

bool ASTTMAXandDVInput(void *id, char *str, void *data)
{
	return ((ApolloRTCCMFD*)data)->set_ASTTMaxandDV(str);
}

bool ApolloRTCCMFD::set_ASTTMaxandDV(char *str)
{
	if (G->RTEASTType == 75)
	{
		double dv;
		if (sscanf(str, "%lf", &dv) == 1)
		{
			GC->rtcc->med_f75.DVMAX = dv;
			return true;
		}
	}
	return false;
}

void ApolloRTCCMFD::menuASTLandingTime()
{
	if (G->RTEASTType == 76 || G->RTEASTType == 77)
	{
		GenericGETInput(&GC->rtcc->med_f75_f77.T_Z, "Choose the landing time (Format: hhh:mm:ss)");
	}
}

void ApolloRTCCMFD::menuASTEntryProfile()
{
	bool ASTEntryProfileInput(void *id, char *str, void *data);
	oapiOpenInputBox("Enter HB1 for manual reentry or HGN for guided reentry. Only HB1 allowed for steep target line (HB1 default)", ASTEntryProfileInput, 0, 25, (void*)this);
}

bool ASTEntryProfileInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->set_ASTEntryProfile(str);
	return true;
}

void ApolloRTCCMFD::set_ASTEntryProfile(char *str)
{
	GC->rtcc->med_f75_f77.EntryProfile.assign(str);
}

void ApolloRTCCMFD::menuASTCalc()
{
	G->AbortScanTableCalc();
}

void ApolloRTCCMFD::menuDeleteASTRow()
{
	bool DeleteASTRowInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the row to delete (1-7). Enter 0 to delete all rows.", DeleteASTRowInput, 0, 25, (void*)this);
}

bool DeleteASTRowInput(void *id, char *str, void *data)
{
	char Buff[128];
	sprintf_s(Buff, "F79,%s;", str);
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buff);
	return true;
}

void ApolloRTCCMFD::menuRTEDManualVectorTime()
{
	if (GC->MissionPlanningActive)
	{
		bool RTEDManualVectorTimeInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the vector GET (Format: hhh:mm:ss)", RTEDManualVectorTimeInput, 0, 25, (void*)this);
	}
}

bool RTEDManualVectorTimeInput(void *id, char *str, void *data)
{
	int hh, mm;
	double ss, get;
	if (sscanf(str, "%d:%d:%lf", &hh, &mm, &ss) == 3)
	{
		get = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_RTEDManualVectorTime(get);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_RTEDManualVectorTime(double get)
{
	GC->rtcc->med_f81.VectorTime = get;
}

void ApolloRTCCMFD::menuRTEDManualIgnitionTime()
{
	bool RTEDManualIgnitionTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the ignition GET (Format: hhh:mm:ss)", RTEDManualIgnitionTimeInput, 0, 25, (void*)this);
}

bool RTEDManualIgnitionTimeInput(void *id, char *str, void *data)
{
	int hh, mm;
	double ss, get;
	if (sscanf(str, "%d:%d:%lf", &hh, &mm, &ss) == 3)
	{
		get = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_RTEDManualIgnitionTime(get);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_RTEDManualIgnitionTime(double get)
{
	GC->rtcc->med_f81.IgnitionTime = get;
}

void ApolloRTCCMFD::menuCycleRTEDManualReference()
{
	if (GC->rtcc->med_f81.RefBody == BODY_EARTH)
	{
		GC->rtcc->med_f81.RefBody = BODY_MOON;
	}
	else
	{
		GC->rtcc->med_f81.RefBody = BODY_EARTH;
	}
}

void ApolloRTCCMFD::menuEnterRTEDManualDV()
{
	bool EnterRTEDManualDVInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the maneuver DV in LVLH coordinates:", EnterRTEDManualDVInput, 0, 25, (void*)this);
}

bool EnterRTEDManualDVInput(void *id, char *str, void *data)
{
	VECTOR3 DV;
	if (sscanf(str, "%lf %lf %lf", &DV.x, &DV.y, &DV.z) == 3)
	{
		((ApolloRTCCMFD*)data)->set_RTEDManualDV(DV);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_RTEDManualDV(VECTOR3 DV)
{
	GC->rtcc->med_f81.XDV = DV * 0.3048;
}

void ApolloRTCCMFD::menuTransferSPQorDKIToMPT()
{
	if (GC->rtcc->med_m70.Plan == 0)
	{
		G->TransferSPQToMPT();
	}
	else if (GC->rtcc->med_m70.Plan == 1)
	{
		G->TransferDKIToMPT();
	}
	else
	{
		G->TransferDescentPlanToMPT();
	}
}

void ApolloRTCCMFD::menuBackToSPQorDKIPage()
{
	if (GC->rtcc->med_m70.Plan == 0)
	{
		menuSetSPQPage();
	}
	else if (GC->rtcc->med_m70.Plan == 1)
	{
		menuSetDKIPage();
	}
	else
	{
		menuSetDescPlanTablePage();
	}
}

void ApolloRTCCMFD::menuBacktoLOIorMCCPage()
{
	if (GC->rtcc->med_m78.Type)
	{
		menuSetLOIDisplayPage();
	}
	else
	{
		menuMidcourseTradeoffPage();
	}
}

void ApolloRTCCMFD::menuTransferGPMToMPT()
{
	G->TransferGPMToMPT();
}

void ApolloRTCCMFD::menuLoadSystemParametersFile()
{
	bool LoadSystemParametersFileInput(void *id, char *str, void *data);
	oapiOpenInputBox("Enter name of systems parameters file:", LoadSystemParametersFileInput, 0, 20, (void*)this);
}

bool LoadSystemParametersFileInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->LoadSystemParametersFile(str);
	return true;
}

void ApolloRTCCMFD::LoadSystemParametersFile(char *file)
{
	if (GC->rtcc->LoadMissionConstantsFile(file))
	{
		GC->rtcc->SystemParametersFile.assign(file);
		status = 1;
	}
	else
	{
		status = 2;
	}
}

void ApolloRTCCMFD::menuLoadTLIFile()
{
	bool LoadTLIFileInput(void *id, char *str, void *data);
	oapiOpenInputBox("Enter name of TLI file:", LoadTLIFileInput, 0, 20, (void*)this);
}

bool LoadTLIFileInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->LoadTLIFile(str);
	return true;
}

void ApolloRTCCMFD::LoadTLIFile(char *file)
{
	int err = GC->rtcc->QMMBLD(file);

	if (err == 0)
	{
		GC->rtcc->TLIFile.assign(file);
		status = 1; //All good
	}
	else if (err == 1)
	{
		status = 2; //File not found
	}
	else
	{
		status = 3; //Other error
	}
}

void ApolloRTCCMFD::menuLoadSFPFile()
{
	bool LoadSFPFileInput(void *id, char *str, void *data);
	oapiOpenInputBox("Enter name of SFP file:", LoadSFPFileInput, 0, 20, (void*)this);
}

bool LoadSFPFileInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->LoadSFPFile(str);
	return true;
}

void ApolloRTCCMFD::LoadSFPFile(char *file)
{
	int err = GC->rtcc->QMSEARCH(file);

	if (err == 0)
	{
		GC->rtcc->SFPFile.assign(file);
		status = 1; //All good
	}
	else if (err == 1)
	{
		status = 2; //File not found
	}
	else
	{
		status = 3; //Other error
	}
}

void ApolloRTCCMFD::menuMPTDirectInputMPTCode()
{
	if (GC->rtcc->med_m66.Table == RTCC_MPT_CSM)
	{
		GC->rtcc->med_m66.Table = RTCC_MPT_LM;
	}
	else
	{
		GC->rtcc->med_m66.Table = RTCC_MPT_CSM;
	}
}

void ApolloRTCCMFD::menuMPTDirectInputReplaceCode()
{
	bool MPTDirectInputReplaceCodeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Enter maneuver to replace (0 for none):", MPTDirectInputReplaceCodeInput, 0, 20, (void*)this);
}

bool MPTDirectInputReplaceCodeInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->set_MPTDirectInputReplaceCode(atoi(str));
	return true;
}

void ApolloRTCCMFD::set_MPTDirectInputReplaceCode(unsigned n)
{
	GC->rtcc->med_m66.ReplaceCode = n;
}

void ApolloRTCCMFD::menuMPTDirectInputAttitude()
{
	if (GC->rtcc->med_m66.AttitudeOpt < RTCC_ATTITUDE_AGS_EXDV)
	{
		GC->rtcc->med_m66.AttitudeOpt++;
	}
	else
	{
		GC->rtcc->med_m66.AttitudeOpt = RTCC_ATTITUDE_INERTIAL;
	}
}

void ApolloRTCCMFD::menuMPTDirectInputBurnParameters()
{
	bool MPTDirectInputM40DataInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the burn parameter (P1-P7) or DV (Format: M40,PX,X,Y,Z;):", MPTDirectInputM40DataInput, 0, 30, (void*)this);
}

bool MPTDirectInputM40DataInput(void *id, char *str, void *data)
{
	return ((ApolloRTCCMFD*)data)->set_MPTDirectInputM40Data(str);
}

bool ApolloRTCCMFD::set_MPTDirectInputM40Data(char *str)
{
	if (strlen(str) == 2)
	{
		char buff = str[1];
		int parm = buff - '0';
		if (parm < 1 || parm > 7)
		{
			return false;
		}
		GC->rtcc->med_m66.BurnParamNo = parm;
		return true;
	}

	sprintf_s(GC->rtcc->RTCCMEDBUFFER, 256, str);
	G->GeneralMEDRequest();
	return true;
}

void ApolloRTCCMFD::menuMPTDirectInputCoord()
{
	if (GC->rtcc->med_m66.BurnParamNo == 1)
	{
		bool MPTDirectInputCoordInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the attitude for the maneuver (Format: LVLH/IMU/FDAI=X Y Z)", MPTDirectInputCoordInput, 0, 30, (void*)this);
	}
}

bool MPTDirectInputCoordInput(void *id, char *str, void *data)
{
	VECTOR3 Att;

	if (sscanf(str, "LVLH=%lf %lf %lf", &Att.x, &Att.y, &Att.z) == 3)
	{
		((ApolloRTCCMFD*)data)->set_MPTDirectInputCoord(Att*RAD, 0);
		return true;
	}
	else if (sscanf(str, "IMU=%lf %lf %lf", &Att.x, &Att.y, &Att.z) == 3)
	{
		((ApolloRTCCMFD*)data)->set_MPTDirectInputCoord(Att*RAD, 1);
		return true;
	}
	else if (sscanf(str, "FDAI=%lf %lf %lf", &Att.x, &Att.y, &Att.z) == 3)
	{
		((ApolloRTCCMFD*)data)->set_MPTDirectInputCoord(Att*RAD, 2);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_MPTDirectInputCoord(VECTOR3 Att, int mode)
{
	GC->rtcc->med_m66.Att = Att;
	GC->rtcc->med_m66.CoordInd = mode;
}

void ApolloRTCCMFD::menuMPTDirectInputHeadsUpDown()
{
	GC->rtcc->med_m66.HeadsUp = !GC->rtcc->med_m66.HeadsUp;
}

void ApolloRTCCMFD::menuMPTDirectInputDPSTenPercentTime()
{
	bool MPTDirectInputDPSTenPercentTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Delta T of 10% thrust for DPS (negative to ignore short burn test):", MPTDirectInputDPSTenPercentTimeInput, 0, 20, (void*)this);
}

bool MPTDirectInputDPSTenPercentTimeInput(void *id, char *str, void *data)
{
	double deltat;
	if (sscanf(str, "%lf", &deltat) == 1)
	{
		((ApolloRTCCMFD*)data)->set_MPTDirectInputDPSTenPercentTime(deltat);
		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_MPTDirectInputDPSTenPercentTime(double deltat)
{
	GC->rtcc->med_m66.TenPercentDT = deltat;
}

void ApolloRTCCMFD::menuMPTDirectInputDPSScaleFactor()
{
	bool MPTDirectInputDPSScaleFactorInput(void *id, char *str, void *data);
	oapiOpenInputBox("DPS thrust scaling factor (0 to 1):", MPTDirectInputDPSScaleFactorInput, 0, 20, (void*)this);
}

bool MPTDirectInputDPSScaleFactorInput(void *id, char *str, void *data)
{
	double scale;
	if (sscanf(str, "%lf", &scale) == 1)
	{
		((ApolloRTCCMFD*)data)->set_MPTDirectInputDPSScaleFactor(scale);
		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_MPTDirectInputDPSScaleFactor(double scale)
{
	GC->rtcc->med_m66.DPSThrustFactor = scale;
}

void ApolloRTCCMFD::menuMPTDirectInputUllageDT()
{
	bool MPTDirectInputUllageDTInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the ullage duration in seconds:", MPTDirectInputUllageDTInput, 0, 20, (void*)this);
}

bool MPTDirectInputUllageDTInput(void *id, char *str, void *data)
{
	double ss;
	if (sscanf(str, "%lf", &ss) == 1)
	{
		((ApolloRTCCMFD*)data)->set_MPTDirectInputUllageDT(ss);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_MPTDirectInputUllageDT(double dt)
{
	GC->rtcc->med_m66.UllageDT = dt;
}

void ApolloRTCCMFD::menuMPTDirectInputUllageThrusters()
{
	GC->rtcc->med_m66.UllageQuads = !GC->rtcc->med_m66.UllageQuads;
}

void ApolloRTCCMFD::menuMPTDirectInputREFSMMAT()
{
	//TBD
}

void ApolloRTCCMFD::menuMPTDirectInputDeltaDockingAngle()
{
	//TBD
}

void ApolloRTCCMFD::menuMPTDirectInputTrimAngleInd()
{
	if (GC->rtcc->med_m66.TrimAngleIndicator == 0)
	{
		GC->rtcc->med_m66.TrimAngleIndicator = 2;
	}
	else
	{
		GC->rtcc->med_m66.TrimAngleIndicator = 0;
	}
}

void ApolloRTCCMFD::menuCycleGMPManeuverVehicle()
{
	if (GC->rtcc->med_k20.Vehicle == 1)
	{
		GC->rtcc->med_k20.Vehicle = 3;
	}
	else
	{
		GC->rtcc->med_k20.Vehicle = 1;
	}
}

void ApolloRTCCMFD::menuCycleGMPManeuverPoint()
{
	if (G->GMPManeuverPoint >= 6)
	{
		G->GMPManeuverPoint = 0;
	}
	else
	{
		G->GMPManeuverPoint++;
	}

	G->DetermineGMPCode();
}

void ApolloRTCCMFD::menuCycleGMPManeuverType()
{
	if (G->GMPManeuverType >= 12)
	{
		G->GMPManeuverType = 0;
	}
	else
	{
		G->GMPManeuverType++;
	}

	G->DetermineGMPCode();
}

void ApolloRTCCMFD::menuCycleMarkerUp()
{
	if (marker >= markermax)
	{
		marker = 0;
	}
	else
	{
		marker++;
	}
}

void ApolloRTCCMFD::menuCycleMarkerDown()
{
	if (marker <= 0)
	{
		marker = markermax;
	}
	else
	{
		marker--;
	}
}

void ApolloRTCCMFD::menuSetGMPInput()
{
	if (marker == 0)
	{
		menuCycleGMPManeuverVehicle();
	}
	else if (marker == 1)
	{
		menuCycleGMPManeuverType();
	}
	else if (marker == 2)
	{
		menuCycleGMPManeuverPoint();
	}
	else if (marker == 3)
	{
		OrbAdjGETDialogue();
	}
	else if (marker == 4)
	{
		GMPInput1Dialogue();
	}
	else if (marker == 5)
	{
		GMPInput2Dialogue();
	}
	else if (marker == 6)
	{
		GMPInput3Dialogue();
	}
	else if (marker == 7)
	{
		GMPInput4Dialogue();
	}
}

void ApolloRTCCMFD::menuGPMCycleVessel()
{
	if (GC->rtcc->med_k20.Vehicle == 1)
	{
		set_CSMVessel();
	}
	else
	{
		set_LMVessel();
	}
}

void ApolloRTCCMFD::GPMPCalc()
{
	if (G->GMPManeuverCode > 0)
	{
		G->startSubthread(3);
	}
}

void ApolloRTCCMFD::menuManPADUllage()
{
	bool ManPADUllageOptionInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the number and duration of ullage (Format: number time)", ManPADUllageOptionInput, 0, 20, (void*)this);
}

bool ManPADUllageOptionInput(void *id, char *str, void *data)
{
	double ss;
	int num;
	if (sscanf(str, "%d %lf", &num, &ss) == 2)
	{
		((ApolloRTCCMFD*)data)->set_ManPADUllageOption(num, ss);
		return true;
	}
	return false;
}

bool ApolloRTCCMFD::set_ManPADUllageOption(int num, double dt)
{
	if (num == 2)
	{
		G->manpad_ullage_opt = false;
	}
	else if (num == 4)
	{
		G->manpad_ullage_opt = true;
	}
	else
	{
		return false;
	}

	G->manpad_ullage_dt = dt;
	return true;
}

void ApolloRTCCMFD::menuManPADTIG()
{
	GenericGETInput(&G->P30TIG, "Choose the GET for the maneuver (Format: hhh:mm:ss)");
}

void ApolloRTCCMFD::menuManPADDV()
{
	bool ManPADDVInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the DV for the maneuver (Format: +XX.X -XX.X +XX.X)", ManPADDVInput, 0, 20, (void*)this);
}

bool ManPADDVInput(void *id, char *str, void *data)
{
	int test;
	test = 0;
	for (unsigned int h = 0; h < strlen(str); h++)
	{
		if (str[h] == ' ')
		{
			test++;
		}
	}
	if (test != 2)
	{
		return false;
	}
	if (strlen(str) > 4)
	{
		char dvx[10], dvy[10], dvz[10];
		int i,j;
		for (i = 0; str[i]!=' '; i++);
		strncpy(dvx, str, i);
		for (j = i+1; str[j] != ' '; j++);
		strncpy(dvy, str+i+1, j - i - 1);
		//for (k = j+1; str[i] != '\0'; k++);
		strncpy(dvz, str + j+1, strlen(str)-j-1);
		((ApolloRTCCMFD*)data)->set_P30DV(_V(atof(dvx),atof(dvy),atof(dvz)));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_P30DV(VECTOR3 dv)
{
	G->dV_LVLH = dv*0.3048;
}

void ApolloRTCCMFD::menuREFSMMATAtt()
{
	bool REFSMMATAttInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the attitude for the REFSMMAT (Format: XX.X XX.X XX.X)", REFSMMATAttInput, 0, 20, (void*)this);
}

bool REFSMMATAttInput(void *id, char *str, void *data)
{
	VECTOR3 att;
	if (sscanf(str, "%lf %lf %lf", &att.x, &att.y, &att.z) == 3)
	{
		((ApolloRTCCMFD*)data)->set_REFSMMATAtt(att);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_REFSMMATAtt(VECTOR3 att)
{
	G->VECangles = att * RAD;
}

void ApolloRTCCMFD::menuTIChaserVectorTime()
{
	if (GC->MissionPlanningActive)
	{
		GenericGETInput(&GC->rtcc->med_k30.ChaserVectorTime, "Choose the chaser vector GET (Format: hhh:mm:ss), 0 or smaller for present time");
	}
	else
	{
		if (GC->rtcc->med_k30.Vehicle == 1)
		{
			set_CSMVessel();
		}
		else
		{
			set_LMVessel();
		}
	}
}

void ApolloRTCCMFD::menuTITargetVectorTime()
{
	if (GC->MissionPlanningActive)
	{
		GenericGETInput(&GC->rtcc->med_k30.TargetVectorTime, "Choose the target vector GET (Format: hhh:mm:ss), 0 or smaller for present time");
	}
	else
	{
		if (GC->rtcc->med_k30.Vehicle == 1)
		{
			set_LMVessel();
		}
		else
		{
			set_CSMVessel();
		}
	}
}

void ApolloRTCCMFD::menuTITimeIncrement()
{
	GenericDoubleInput(&GC->rtcc->med_k30.TimeStep, "Enter the time increment between the variable maneuver times:", 1.0);
}

void ApolloRTCCMFD::menuTITimeRange()
{
	GenericDoubleInput(&GC->rtcc->med_k30.TimeRange, "Enter the range of time for the variable maneuver times:", 1.0);
}

void ApolloRTCCMFD::t1dialogue()
{
	GenericGETInput(&GC->rtcc->med_k30.StartTime, "Choose the GET for the maneuver (Format: hhh:mm:ss), negative time for TPI search");
}

void ApolloRTCCMFD::t2dialogue()
{
	GenericGETInput(&GC->rtcc->med_k30.EndTime, "Choose the GET for the arrival (Format: hhh:mm:ss), negative time for TPF search");
}

void ApolloRTCCMFD::OrbAdjGETDialogue()
{
	GenericGETInput(&G->SPSGET, "Choose the GET for the maneuver (Format: hhh:mm:ss)");
}

void ApolloRTCCMFD::OrbAdjRevDialogue()
{
	GenericIntInput(&G->GMPRevs, "Number of revolutions:");
}

void ApolloRTCCMFD::menuSetSPQChaserThresholdTime()
{
	if (GC->MissionPlanningActive)
	{
		GenericGETInput(&GC->rtcc->med_k01.ChaserThresholdGET, "Choose the SPQ chaser threshold (Format: hhh:mm:ss)");
	}
	else
	{
		if (GC->rtcc->med_k01.ChaserVehicle == 1)
		{
			set_CSMVessel();
		}
		else
		{
			set_LMVessel();
		}
	}
}

void ApolloRTCCMFD::menuSetSPQTargetThresholdTime()
{
	if (GC->MissionPlanningActive)
	{
		GenericGETInput(&GC->rtcc->med_k01.TargetThresholdGET, "Choose the SPQ target threshold (Format: hhh:mm:ss)");
	}
	else
	{
		if (GC->rtcc->med_k01.ChaserVehicle == 1)
		{
			set_LMVessel();
		}
		else
		{
			set_CSMVessel();
		}
	}
}

void ApolloRTCCMFD::SPQtimedialogue()
{
	bool SPQGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for the maneuver (Format: hhh:mm:ss)", SPQGETInput, 0, 20, (void*)this);
}

bool SPQGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, tig;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		tig = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_SPQtime(tig);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SPQtime(double tig)
{
	if (G->SPQMode == 1)
	{
		this->G->CDHtime = tig;
	}
	else
	{
		this->G->CSItime = tig;
	}
}

void ApolloRTCCMFD::menuRTEDASTCodeDialogue()
{
	GenericIntInput(&GC->rtcc->med_f80.ASTCode, "Choose the AST code (enter 0 for manual entry):");
}

void ApolloRTCCMFD::menuRTED_REFSMMAT()
{
	bool RTED_REFSMMATInput(void* id, char *str, void *data);
	oapiOpenInputBox("REFSMMAT code either from RTCC table (CUR, DMT, PCR, DOD, TLM, LCV, OST, AGS, MED, DOK) or internally calculated (ROP, ROY, ROZ, TEI, DEI, REI). See manual for definitions.", RTED_REFSMMATInput, 0, 20, (void*)this);
}

bool RTED_REFSMMATInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->set_RTED_REFSMMAT(str);
	return true;
}

void ApolloRTCCMFD::set_RTED_REFSMMAT(char *str)
{
	GC->rtcc->med_f80.REFSMMAT.assign(str);
}

void ApolloRTCCMFD::menuSetRTEDUllage()
{
	bool SetRTEDUllageInput(void* id, char *str, void *data);
	oapiOpenInputBox("Set number of thrusters (All) and duration of ullage (SPS, DPS only). Format: Num Time", SetRTEDUllageInput, 0, 20, (void*)this);
}

bool SetRTEDUllageInput(void *id, char *str, void *data)
{
	double duration = 0.0;
	int num;
	if (sscanf(str, "%d %lf", &num, &duration) >= 1)
	{
		if (abs(num) == 2 || abs(num) == 4)
		{
			((ApolloRTCCMFD*)data)->set_RTEDUllage(num, duration);
			return true;
		}
	}
	return false;
}

void ApolloRTCCMFD::set_RTEDUllage(int num, double duration)
{
	GC->rtcc->med_f80.NumQuads = num;
	GC->rtcc->med_f80.UllageDT = duration;
}

void ApolloRTCCMFD::menuCycleRTEDTrimAnglesOption()
{
	if (GC->rtcc->med_f80.TrimAngleInd == 1)
	{
		GC->rtcc->med_f80.TrimAngleInd = -1;
	}
	else
	{
		GC->rtcc->med_f80.TrimAngleInd = 1;
	}
}

void ApolloRTCCMFD::menuCycleRTEDHeadsOption()
{
	GC->rtcc->med_f80.HeadsUp = !GC->rtcc->med_f80.HeadsUp;
}

void ApolloRTCCMFD::menuCycleRTEDIterateOption()
{
	GC->rtcc->med_f80.Iterate = !GC->rtcc->med_f80.Iterate;
}

void ApolloRTCCMFD::menuSetEntryDesiredInclination()
{
	bool EntryDesiredInclinationInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the desired inclination (e.g. 40A for asc, 40D for desc):", EntryDesiredInclinationInput, 0, 20, (void*)this);
}

bool EntryDesiredInclinationInput(void *id, char *str, void *data)
{
	double inc;
	char dir[32];
	if (strlen(str) < 20)
	{
		if (sscanf(str, "%lf%s", &inc, dir) == 2)
		{
			if (strcmp(dir, "A") == 0)
			{
				((ApolloRTCCMFD*)data)->set_EntryDesiredInclination(inc);
				return true;
			}
			else if (strcmp(dir, "D") == 0)
			{
				((ApolloRTCCMFD*)data)->set_EntryDesiredInclination(-inc);
				return true;
			}
		}
		else if (sscanf(str, "%lf", &inc) == 1)
		{
			((ApolloRTCCMFD*)data)->set_EntryDesiredInclination(inc);
			return true;
		}
	}
	return false;
}

void ApolloRTCCMFD::set_EntryDesiredInclination(double inc)
{
	GC->rtcc->med_f75_f77.Inclination = inc * RAD;
}

void ApolloRTCCMFD::menuSetRTEConstraintF86()
{
	bool RTEConstraintF86Input(void *id, char *str, void *data);
	oapiOpenInputBox("Enter the RTE constraint (Format: Constraint,Value)", RTEConstraintF86Input, 0, 20, (void*)this);
}

bool RTEConstraintF86Input(void *id, char *str, void *data)
{
	char buff[100];
	double val;

	if (sscanf(str, "%[^','],%lf", buff, &val) == 2)
	{
		std::string constr(buff);
		((ApolloRTCCMFD*)data)->set_RTEConstraintF86(constr, val);
		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_RTEConstraintF86(std::string constr, double value)
{
	GC->rtcc->med_f86.Constraint = constr;
	GC->rtcc->med_f86.Value = value;

	GC->rtcc->PMQAFMED("86");
}

void ApolloRTCCMFD::menuSetRTEConstraintF87()
{
	bool RTEConstraintF87Input(void *id, char *str, void *data);
	oapiOpenInputBox("Enter the RTE constraint (Format: Constraint,Value)", RTEConstraintF87Input, 0, 20, (void*)this);
}

bool RTEConstraintF87Input(void *id, char *str, void *data)
{
	char buff1[100], buff2[100];
	if (sscanf(str, "%[^','],%s", buff1, buff2) == 2)
	{
		std::string constr(buff1);
		std::string val(buff2);
		((ApolloRTCCMFD*)data)->set_RTEConstraintF87(constr, val);
		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_RTEConstraintF87(std::string constr, std::string value)
{
	GC->rtcc->med_f87.Constraint = constr;
	GC->rtcc->med_f87.Value = value;

	GC->rtcc->PMQAFMED("87");
}

void ApolloRTCCMFD::menuAddRTESite()
{
	bool AddRTESiteInput(void *id, char *str, void *data);
	oapiOpenInputBox("Add RTE Site. Format: Type Name Latitude Longitude. Type = PTP, ATP. Name = Name of Site. One pair of Lat/Lng for PTP, up to 5 for ATP", AddRTESiteInput, 0, 50, (void*)this);
}

bool AddRTESiteInput(void *id, char *str, void *data)
{
	double LatLng[10];
	char Buffer[256], buff1[100], buff2[100];
	int num;

	num = sscanf(str, "%s %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", buff1, buff2, &LatLng[0], &LatLng[1], &LatLng[2], &LatLng[3], &LatLng[4],
		&LatLng[5], &LatLng[6], &LatLng[7], &LatLng[8], &LatLng[9]);

	if (num >= 4)
	{
		int length = sprintf(Buffer, "F85,ADD,%s,%s,", buff1, buff2);

		for (int i = 0; i < num - 2; i++)
		{
			length += sprintf(Buffer + length, ",%lf", LatLng[i]);
		}
		length += sprintf(Buffer + length, ";");

		((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buffer);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuReplaceRTESite()
{
	bool ReplaceRTESiteInput(void *id, char *str, void *data);
	oapiOpenInputBox("Replace RTE Site.  Format: Type Site Number Latitude Longitude. Type = PTP, ATP. Site = Name of Site. Number (Skip for PTP, pair number for ATP)", ReplaceRTESiteInput, 0, 50, (void*)this);
}

bool ReplaceRTESiteInput(void *id, char *str, void *data)
{
	char Buffer[256], buff1[100], buff2[100];
	double LatLng[2];

	if (sscanf(str, "%s", buff1) == 1)
	{
		int type;

		if (strcmp(buff1, "PTP") == 0)
		{
			type = 0;
		}
		else if (strcmp(buff1, "ATP") == 0)
		{
			type = 1;
		}
		else return false;

		if (type == 0)
		{
			if (sscanf(str, "%s %s %lf %lf", buff1, buff2, &LatLng[0], &LatLng[1]) != 4)
			{
				return false;
			}

			sprintf(Buffer, "F85,REPLACE,PTP,%s,,%lf,%lf;", buff2, LatLng[0], LatLng[1]);
		}
		else
		{
			int pair;

			if (sscanf(str, "%s %s %d %lf %lf", buff1, buff2, &pair, &LatLng[0], &LatLng[1]) != 5)
			{
				return false;
			}

			sprintf(Buffer, "F85,REPLACE,ATP,%s,%d,%lf,%lf;", buff2, pair, LatLng[0], LatLng[1]);
		}

		((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buffer);
		return true;
	}


	return false;
}

void ApolloRTCCMFD::menuDeleteRTESite()
{
	bool DeleteRTESiteInput(void *id, char *str, void *data);
	oapiOpenInputBox("Delete RTE Site. Format: Type Name. Type = PTP, ATP. Name = Name of Site", DeleteRTESiteInput, 0, 20, (void*)this);
}

bool DeleteRTESiteInput(void *id, char *str, void *data)
{
	char Buffer[256], buff1[100], buff2[100];

	if (sscanf(str, "%s %s", buff1, buff2) == 2)
	{
		sprintf(Buffer, "F85,DELETE,%s,%s;", buff1, buff2);

		((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buffer);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::CycleRTECalcMode()
{
	if (G->RTECalcMode < 4)
	{
		G->RTECalcMode++;
	}
	else
	{
		G->RTECalcMode = 1;
	}
}

void ApolloRTCCMFD::menuSetRTEManeuverCode()
{
	bool RTEManeuverCodeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Maneuver code: 1st char = C(CSM), L(LEM), 2nd char = S(SPS), R(RCS), D(DPS), 3rd char = D(docked), A(docked with ascent stage), U(undocked), 4th char = X (External DV)", RTEManeuverCodeInput, 0, 20, (void*)this);
}

bool RTEManeuverCodeInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_RTEManeuverCode(str);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_RTEManeuverCode(char *code)
{
	GC->rtcc->med_f80.ManeuverCode.assign(code);
}

void ApolloRTCCMFD::menuCycleRTEDColumn()
{
	if (GC->rtcc->med_f80.Column < 2)
	{
		GC->rtcc->med_f80.Column++;
	}
	else
	{
		GC->rtcc->med_f80.Column = 1;
	}
}

void ApolloRTCCMFD::menusextantstartime()
{
	bool SextantStarTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Minutes before Maneuver", SextantStarTimeInput, 0, 20, (void*)this);
}

bool SextantStarTimeInput(void *id, char *str, void *data)
{
	if (strlen(str)<20 && atof(str) >= 0.0 && atof(str) <=60.0)
	{
		((ApolloRTCCMFD*)data)->set_sextantstartime(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_sextantstartime(double time)
{
	G->sxtstardtime = -time * 60.0;
}

void ApolloRTCCMFD::REFSMMATTimeDialogue()
{
	if (G->REFSMMATopt == 2)
	{
		bool REFSMMATGETInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the GET (Format: hhh:mm:ss)", REFSMMATGETInput, 0, 20, (void*)this);
	}
	else if (G->REFSMMATopt == 6)
	{
		GenericDoubleInput(&GC->REFSMMAT_PTC_MJD, "Enter MJD of average time of TEI:");
	}
	else if (G->REFSMMATopt == 5 || G->REFSMMATopt == 8)
	{
		menuSetTLAND();
	}
}

void ApolloRTCCMFD::UploadREFSMMAT()
{
	G->REFSMMATUplink(screen == 53);
}

void ApolloRTCCMFD::set_REFSMMATTime(double time)
{
	G->REFSMMAT_LVLH_Time = time;
}

void ApolloRTCCMFD::menuREFSMMATLockerMovement()
{
	menuGeneralMEDRequest("CSM/LEM REFSMMAT Locker Movement. Format: G00,Vehicle1,Matrix1,Vehicle2,Matrix2; (Codes: CUR, PCR, TLM, MED, LCV, OST, DMT, DOD, DOK, LLA, LLD)");
}

void ApolloRTCCMFD::menuCycleTITable()
{

}

void ApolloRTCCMFD::menuSetTIPlanNumber()
{
	bool TIPlanNumberInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose plan from multiple solution table (1-13):", TIPlanNumberInput, 0, 20, (void*)this);
}

bool TIPlanNumberInput(void *id, char *str, void *data)
{
	int num;

	if (sscanf(str, "%d", &num) == 1)
	{
		if (num < 1 || num > 13)
		{
			return false;
		}

		((ApolloRTCCMFD*)data)->set_TIPlanNumber(num);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TIPlanNumber(int plan)
{
	GC->rtcc->med_m72.Plan = plan;
}

void ApolloRTCCMFD::menuTIDeleteGET()
{
	bool TIDeleteGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Delete all maneuvers in both MPTs after GET (Format: hhh:mm:ss, or negative number for no delete)", TIDeleteGETInput, 0, 20, (void*)this);
}

bool TIDeleteGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss;
	double tig;

	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		tig = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_TIDeleteGET(tig);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TIDeleteGET(double get)
{
	GC->rtcc->med_m72.DeleteGET = get;
}

void ApolloRTCCMFD::menuChooseTIThruster()
{
	bool ChooseTIThrusterInput(void *id, char *str, void *data);
	oapiOpenInputBox("Thruster for the maneuver (Options: S, A, D, L1-L4, C1-C4)", ChooseTIThrusterInput, 0, 20, (void*)this);
}

bool ChooseTIThrusterInput(void *id, char *str, void *data)
{
	std::string th(str);
	return ((ApolloRTCCMFD*)data)->set_ChooseTIThruster(th);
}

bool ApolloRTCCMFD::set_ChooseTIThruster(std::string th)
{
	return ThrusterType(th, GC->rtcc->med_m72.Thruster);
}

void ApolloRTCCMFD::menuCycleTIAttitude()
{
	if (GC->rtcc->med_m72.Attitude < 5)
	{
		GC->rtcc->med_m72.Attitude++;
	}
	else
	{
		GC->rtcc->med_m72.Attitude = 1;
	}
}

void ApolloRTCCMFD::menuTIUllageOption()
{
	bool TIUllageOptionInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the number and duration of ullage (Format: number time)", TIUllageOptionInput, 0, 20, (void*)this);
}

bool TIUllageOptionInput(void *id, char *str, void *data)
{
	double ss;
	int num;
	if (sscanf(str, "%d %lf", &num, &ss) == 2)
	{
		((ApolloRTCCMFD*)data)->set_UllageOption(72, num, ss);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuM70UllageOption()
{
	bool M70UllageOptionInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the number and duration of ullage (Format: number time)", M70UllageOptionInput, 0, 20, (void*)this);
}

bool M70UllageOptionInput(void *id, char *str, void *data)
{
	double ss;
	int num;
	if (sscanf(str, "%d %lf", &num, &ss) == 2)
	{
		((ApolloRTCCMFD*)data)->set_UllageOption(70, num, ss);
		return true;
	}
	return false;
}

bool ApolloRTCCMFD::set_UllageOption(int med, int num, double dt)
{
	bool UllageQuads;
	double UllageDT;

	if (num == 2)
	{
		UllageQuads = false;
	}
	else if (num == 4)
	{
		UllageQuads = true;
	}
	else
	{
		return false;
	}

	UllageDT = dt;

	switch (med)
	{
	case 70:
		GC->rtcc->med_m70.UllageQuads = UllageQuads;
		GC->rtcc->med_m70.UllageDT = UllageDT;
		break;
	case 72:
		GC->rtcc->med_m72.UllageQuads = UllageQuads;
		GC->rtcc->med_m72.UllageDT = UllageDT;
		break;
	}
	return true;
}

void ApolloRTCCMFD::menuCycleTIIterationFlag()
{
	GC->rtcc->med_m72.Iteration = !GC->rtcc->med_m72.Iteration;
}

void ApolloRTCCMFD::menuCycleTITimeFlag()
{
	GC->rtcc->med_m72.TimeFlag = !GC->rtcc->med_m72.TimeFlag;
}

void ApolloRTCCMFD::menuTIDPSTenPercentTime()
{
	bool MPTTIDPSTenPercentTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Delta T of 10% thrust for DPS (negative to ignore short burn test):", MPTTIDPSTenPercentTimeInput, 0, 20, (void*)this);
}

bool MPTTIDPSTenPercentTimeInput(void *id, char *str, void *data)
{
	double deltat;
	if (sscanf(str, "%lf", &deltat) == 1)
	{
		((ApolloRTCCMFD*)data)->set_TIDPSTenPercentTime(deltat);
		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_TIDPSTenPercentTime(double deltat)
{
	GC->rtcc->med_m72.TenPercentDT = deltat;
}

void ApolloRTCCMFD::menuTIDPSScaleFactor()
{
	bool TIDPSScaleFactorInput(void *id, char *str, void *data);
	oapiOpenInputBox("DPS thrust scaling factor (0 to 1):", TIDPSScaleFactorInput, 0, 20, (void*)this);
}

bool TIDPSScaleFactorInput(void *id, char *str, void *data)
{
	double scale;
	if (sscanf(str, "%lf", &scale) == 1)
	{
		((ApolloRTCCMFD*)data)->set_TIDPSScaleFactor(scale);
		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_TIDPSScaleFactor(double scale)
{
	GC->rtcc->med_m72.DPSThrustFactor = scale;
}

void ApolloRTCCMFD::menuChooseSPQDKIThruster()
{
	bool ChooseSPQDKIThrusterInput(void *id, char *str, void *data);
	oapiOpenInputBox("Thruster for the maneuver (Options: S, A, D, L1-L4, C1-C4)", ChooseSPQDKIThrusterInput, 0, 20, (void*)this);
}

bool ChooseSPQDKIThrusterInput(void *id, char *str, void *data)
{
	std::string th(str);
	return ((ApolloRTCCMFD*)data)->set_ChooseSPQDKIThruster(th);
}

bool ApolloRTCCMFD::set_ChooseSPQDKIThruster(std::string th)
{
	return ThrusterType(th, GC->rtcc->med_m70.Thruster);
}

void ApolloRTCCMFD::menuM70DeleteGET()
{
	bool MPTM70DeleteGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Delete all maneuvers in both MPTs after GET (Format: hhh:mm:ss, or negative number for no delete)", MPTM70DeleteGETInput, 0, 20, (void*)this);
}

bool MPTM70DeleteGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss;
	double tig;

	if (sscanf(str, "%lf", &tig) == 1)
	{
		((ApolloRTCCMFD*)data)->set_MPTM70DeleteGET(tig);
		return true;
	}
	else if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		tig = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_MPTM70DeleteGET(tig);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_MPTM70DeleteGET(double get)
{
	GC->rtcc->med_m70.DeleteGET = get;
}

void ApolloRTCCMFD::menuM70CycleAttitude()
{
	if (GC->rtcc->med_m70.Attitude < 5)
	{
		GC->rtcc->med_m70.Attitude++;
	}
	else
	{
		GC->rtcc->med_m70.Attitude = 1;
	}
}

void ApolloRTCCMFD::menuM70CycleIterationFlag()
{
	GC->rtcc->med_m70.Iteration = !GC->rtcc->med_m70.Iteration;
}

void ApolloRTCCMFD::menuM70CycleTimeFlag()
{
	GC->rtcc->med_m70.TimeFlag = !GC->rtcc->med_m70.TimeFlag;
}

void ApolloRTCCMFD::menuM70DPSTenPercentTime()
{
	bool MPTM70DPSTenPercentTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Delta T of 10% thrust for DPS (negative to ignore short burn test):", MPTM70DPSTenPercentTimeInput, 0, 20, (void*)this);
}

bool MPTM70DPSTenPercentTimeInput(void *id, char *str, void *data)
{
	double deltat;
	if (sscanf(str, "%lf", &deltat) == 1)
	{
		((ApolloRTCCMFD*)data)->set_M70DPSTenPercentTime(deltat);
		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_M70DPSTenPercentTime(double deltat)
{
	GC->rtcc->med_m70.TenPercentDT = deltat;
}

void ApolloRTCCMFD::menuM70DPSScaleFactor()
{
	bool M70DPSScaleFactorInput(void *id, char *str, void *data);
	oapiOpenInputBox("DPS thrust scaling factor (0 to 1):", M70DPSScaleFactorInput, 0, 20, (void*)this);
}

bool M70DPSScaleFactorInput(void *id, char *str, void *data)
{
	double scale;
	if (sscanf(str, "%lf", &scale) == 1)
	{
		((ApolloRTCCMFD*)data)->set_M70DPSScaleFactor(scale);
		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_M70DPSScaleFactor(double scale)
{
	GC->rtcc->med_m70.DPSThrustFactor = scale;
}

void ApolloRTCCMFD::menuChooseMPTDirectInputThruster()
{
	bool ChooseMPTDirectInputThrusterInput(void *id, char *str, void *data);
	oapiOpenInputBox("Thruster for the maneuver (Options: S, A, D, L1-L4, C1-C4, BV)", ChooseMPTDirectInputThrusterInput, 0, 20, (void*)this);
}

bool ChooseMPTDirectInputThrusterInput(void *id, char *str, void *data)
{
	std::string th(str);
	return ((ApolloRTCCMFD*)data)->set_ChooseMPTDirectInputThruster(th);
}

bool ApolloRTCCMFD::set_ChooseMPTDirectInputThruster(std::string th)
{
	return ThrusterType(th, GC->rtcc->med_m66.Thruster);
}

void ApolloRTCCMFD::menuCycleGPMTable()
{
	if (GC->rtcc->med_m65.Table == 1)
	{
		GC->rtcc->med_m65.Table = 3;
	}
	else
	{
		GC->rtcc->med_m65.Table = 1;
	}
}

void ApolloRTCCMFD::menuGPMReplaceCode()
{
	bool GPMReplaceCodeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Enter maneuver to replace (0 for none):", GPMReplaceCodeInput, 0, 20, (void*)this);
}

bool GPMReplaceCodeInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->set_GPMReplaceCode(atoi(str));
	return true;
}

void ApolloRTCCMFD::set_GPMReplaceCode(unsigned n)
{
	GC->rtcc->med_m65.ReplaceCode = n;
}

void ApolloRTCCMFD::menuChooseGPMThruster()
{
	bool ChooseGPMThrusterInput(void *id, char *str, void *data);
	oapiOpenInputBox("Thruster for the maneuver (Options: S, A, D, L1-L4, C1-C4)", ChooseGPMThrusterInput, 0, 20, (void*)this);
}

bool ChooseGPMThrusterInput(void *id, char *str, void *data)
{
	std::string th(str);
	return ((ApolloRTCCMFD*)data)->set_ChooseGPMThruster(th);
}

bool ApolloRTCCMFD::set_ChooseGPMThruster(std::string th)
{
	return ThrusterType(th, GC->rtcc->med_m65.Thruster);
}

void ApolloRTCCMFD::menuCycleGPMAttitude()
{
	if (GC->rtcc->med_m65.Attitude < RTCC_ATTITUDE_AGS_EXDV)
	{
		GC->rtcc->med_m65.Attitude++;
	}
	else
	{
		GC->rtcc->med_m65.Attitude = RTCC_ATTITUDE_INERTIAL;
	}
}

void ApolloRTCCMFD::menuGPMUllageDT()
{
	bool GPMUllageDTInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the ullage duration in seconds (negative value for nominal):", GPMUllageDTInput, 0, 20, (void*)this);
}

bool GPMUllageDTInput(void *id, char *str, void *data)
{
	double ss;
	if (sscanf(str, "%lf", &ss) == 1)
	{
		((ApolloRTCCMFD*)data)->set_GPMUllageDT(ss);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_GPMUllageDT(double dt)
{
	GC->rtcc->med_m65.UllageDT = dt;
}

void ApolloRTCCMFD::menuGPMUllageThrusters()
{
	GC->rtcc->med_m65.UllageQuads = !GC->rtcc->med_m65.UllageQuads;
}

void ApolloRTCCMFD::menuCycleGPMIterationFlag()
{
	GC->rtcc->med_m65.Iteration = !GC->rtcc->med_m65.Iteration;
}

void ApolloRTCCMFD::menuGPMDPSTenPercentDeltaT()
{
	bool GPMDPSTenPercentDeltaTInput(void *id, char *str, void *data);
	oapiOpenInputBox("Delta T of 10% thrust for DPS (negative to ignore short burn test):", GPMDPSTenPercentDeltaTInput, 0, 20, (void*)this);
}

bool GPMDPSTenPercentDeltaTInput(void *id, char *str, void *data)
{
	double deltat;
	if (sscanf(str, "%lf", &deltat) == 1)
	{
		((ApolloRTCCMFD*)data)->set_GPMDPSTenPercentDeltaT(deltat);
		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_GPMDPSTenPercentDeltaT(double deltat)
{
	GC->rtcc->med_m65.TenPercentDT = deltat;
}

void ApolloRTCCMFD::menuGPMDPSThrustScaling()
{
	bool GPMDPSThrustScalingInput(void *id, char *str, void *data);
	oapiOpenInputBox("DPS thrust scaling factor (0 to 1):", GPMDPSThrustScalingInput, 0, 20, (void*)this);
}

bool GPMDPSThrustScalingInput(void *id, char *str, void *data)
{
	double scale;
	if (sscanf(str, "%lf", &scale) == 1)
	{
		((ApolloRTCCMFD*)data)->set_GPMDPSThrustScaling(scale);
		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_GPMDPSThrustScaling(double scale)
{
	GC->rtcc->med_m65.DPSThrustFactor = scale;
}

void ApolloRTCCMFD::menuCycleGPMTimeFlag()
{
	GC->rtcc->med_m65.TimeFlag = !GC->rtcc->med_m65.TimeFlag;
}

void ApolloRTCCMFD::menuMPTDirectInputTransfer()
{
	G->MPTDirectInputCalc();
}

void ApolloRTCCMFD::menuCycleLOIMCCTable()
{
	if (GC->rtcc->med_m78.Table == 1)
	{
		GC->rtcc->med_m78.Table = 3;
	}
	else
	{
		GC->rtcc->med_m78.Table = 1;
	}
}

void ApolloRTCCMFD::menuLOIMCCReplaceCode()
{
	bool LOIMCCReplaceCodeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Enter maneuver to replace (0 for none):", LOIMCCReplaceCodeInput, 0, 20, (void*)this);
}

bool LOIMCCReplaceCodeInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->set_LOIMCCReplaceCode(atoi(str));
	return true;
}

void ApolloRTCCMFD::set_LOIMCCReplaceCode(unsigned n)
{
	GC->rtcc->med_m78.ReplaceCode = n;
}

void ApolloRTCCMFD::menuChooseLOIMCCThruster()
{
	bool ChooseLOIMCCThrusterInput(void *id, char *str, void *data);
	oapiOpenInputBox("Thruster for the maneuver (Options: S, A, D, L1-L4, C1-C4)", ChooseLOIMCCThrusterInput, 0, 20, (void*)this);
}

bool ChooseLOIMCCThrusterInput(void *id, char *str, void *data)
{
	std::string th(str);
	return ((ApolloRTCCMFD*)data)->set_ChooseLOIMCCThruster(th);
}

bool ApolloRTCCMFD::set_ChooseLOIMCCThruster(std::string th)
{
	return ThrusterType(th, GC->rtcc->med_m78.Thruster);
}

void ApolloRTCCMFD::menuCycleLOIMCCAttitude()
{
	if (GC->rtcc->med_m78.Attitude < RTCC_ATTITUDE_AGS_EXDV)
	{
		GC->rtcc->med_m78.Attitude++;
	}
	else
	{
		GC->rtcc->med_m78.Attitude = RTCC_ATTITUDE_INERTIAL;
	}
}

void ApolloRTCCMFD::menuLOIMCCUllageThrustersDT()
{
	bool LOIMCCUllageThrustersDTInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the number and duration of ullage (Format: number time)", LOIMCCUllageThrustersDTInput, 0, 20, (void*)this);
}

bool LOIMCCUllageThrustersDTInput(void *id, char *str, void *data)
{
	double ss;
	int num;
	if (sscanf(str, "%d %lf", &num, &ss) == 2)
	{
		((ApolloRTCCMFD*)data)->set_LOIMCCUllageThrustersDT(num, ss);
		return true;
	}
	return false;
}

bool ApolloRTCCMFD::set_LOIMCCUllageThrustersDT(int num, double dt)
{
	if (num == 2)
	{
		GC->rtcc->med_m78.UllageQuads = false;
	}
	else if (num == 4)
	{
		GC->rtcc->med_m78.UllageQuads = true;
	}
	else
	{
		return false;
	}

	GC->rtcc->med_m78.UllageDT = dt;
	return true;
}

void ApolloRTCCMFD::menuLOIMCCManeuverNumber()
{
	bool LOIMCCManeuverNumberInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the maneuver number from LOI/MCC table:", LOIMCCManeuverNumberInput, 0, 20, (void*)this);
}

bool LOIMCCManeuverNumberInput(void *id, char *str, void *data)
{
	int num;
	if (sscanf(str, "%d", &num) == 1)
	{
		((ApolloRTCCMFD*)data)->set_LOIMCCManeuverNumber(num);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LOIMCCManeuverNumber(int num)
{
	GC->rtcc->med_m78.ManeuverNumber = num;
}

void ApolloRTCCMFD::menuCycleLOIMCCIterationFlag()
{
	GC->rtcc->med_m78.Iteration = !GC->rtcc->med_m78.Iteration;
}

void ApolloRTCCMFD::menuLOIMCCDPSTenPercentDeltaT()
{
	bool LOIMCCDPSTenPercentDeltaTInput(void *id, char *str, void *data);
	oapiOpenInputBox("Delta T of 10% thrust for DPS (negative to ignore short burn test):", LOIMCCDPSTenPercentDeltaTInput, 0, 20, (void*)this);
}

bool LOIMCCDPSTenPercentDeltaTInput(void *id, char *str, void *data)
{
	double deltat;
	if (sscanf(str, "%lf", &deltat) == 1)
	{
		((ApolloRTCCMFD*)data)->set_LOIMCCDPSTenPercentDeltaT(deltat);
		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_LOIMCCDPSTenPercentDeltaT(double deltat)
{
	GC->rtcc->med_m78.TenPercentDT = deltat;
}

void ApolloRTCCMFD::menuLOIMCCDPSThrustScaling()
{
	bool LOIMCCDPSThrustScalingInput(void *id, char *str, void *data);
	oapiOpenInputBox("DPS thrust scaling factor (0 to 1):", LOIMCCDPSThrustScalingInput, 0, 20, (void*)this);
}

bool LOIMCCDPSThrustScalingInput(void *id, char *str, void *data)
{
	double scale;
	if (sscanf(str, "%lf", &scale) == 1)
	{
		((ApolloRTCCMFD*)data)->set_LOIMCCDPSThrustScaling(scale);
		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_LOIMCCDPSThrustScaling(double scale)
{
	GC->rtcc->med_m78.DPSThrustFactor = scale;
}

void ApolloRTCCMFD::menuCycleLOIMCCTimeFlag()
{
	GC->rtcc->med_m78.TimeFlag = !GC->rtcc->med_m78.TimeFlag;
}

void ApolloRTCCMFD::menuTransferTIToMPT()
{
	G->TransferTIToMPT();
}

void ApolloRTCCMFD::menuMPTDirectInputTIG()
{
	GenericGETInput(&GC->rtcc->med_m66.GETBI, "Choose the GET (Format: hhh:mm:ss)");
}

void ApolloRTCCMFD::menuMPTDirectInputDock()
{
	if (GC->rtcc->med_m66.ConfigChangeInd < 2)
	{
		GC->rtcc->med_m66.ConfigChangeInd++;
	}
	else
	{
		GC->rtcc->med_m66.ConfigChangeInd = 0;
	}
}

void ApolloRTCCMFD::menuMPTDirectInputFinalConfig()
{
	bool MPTDirectInputFinalConfigInput(void* id, char *str, void *data);
	oapiOpenInputBox("Any combination of C (CSM), S (SIVB), L (Ascent+Descent), A (Ascent Only), D (Descent Only)", MPTDirectInputFinalConfigInput, 0, 20, (void*)this);
}

bool MPTDirectInputFinalConfigInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->set_MPTDirectInputFinalConfig(str);
	return true;
}

void ApolloRTCCMFD::set_MPTDirectInputFinalConfig(char *cfg)
{
	GC->rtcc->med_m66.FinalConfig.assign(cfg);
}

void ApolloRTCCMFD::menuTransferPoweredAscentToMPT()
{
	G->TransferPoweredAscentToMPT();
}

void ApolloRTCCMFD::menuTransferPoweredDescentToMPT()
{
	G->TransferPoweredDescentToMPT();
}

void ApolloRTCCMFD::menuCycleMPTTable()
{
	if (GC->rtcc->med_m49.Table == RTCC_MPT_CSM)
	{
		GC->rtcc->med_m49.Table = GC->rtcc->med_m50.Table = GC->rtcc->med_m51.Table = GC->rtcc->med_m55.Table = RTCC_MPT_LM;
	}
	else
	{
		GC->rtcc->med_m49.Table = GC->rtcc->med_m50.Table = GC->rtcc->med_m51.Table = GC->rtcc->med_m55.Table = RTCC_MPT_CSM;
	}
}

void ApolloRTCCMFD::menuCycleMPTMED()
{
	if (G->mptinitmode < 3)
	{
		G->mptinitmode++;
	}
	else
	{
		G->mptinitmode = 0;
	}
}

void ApolloRTCCMFD::menuSetMPTInitInput()
{
	switch (marker)
	{
	case 0: //Second option on MED
		switch (G->mptinitmode)
		{
		case 0: //M49: SPS Fuel Remaining
			GenericDoubleInput(&GC->rtcc->med_m49.SPSFuelRemaining, "Input SPS fuel in lbs (negative number for no update):", 0.45359237);
			break;
		case 1: //M50: CSM Weight
			GenericDoubleInput(&GC->rtcc->med_m50.CSMWT, "Input CSM mass in lbs (negative number for no update):", 0.45359237);
			break;
		case 2: //M51: CSM Area
			GenericDoubleInput(&GC->rtcc->med_m51.CSMArea, "Input CSM area in square feet (negative number for no update):", 0.3048*0.3048);
			break;
		case 3: //M55: Config
		{
			bool MPTInitM55ConfigInput(void* id, char *str, void *data);
			oapiOpenInputBox("Any combination of C (CSM), S (SIVB), L (Ascent+Descent), A (Ascent Only), D (Descent Only)", MPTInitM55ConfigInput, 0, 20, (void*)this);
		}
		break;
		}
		break;
	case 1: //Third option on MED
		switch (G->mptinitmode)
		{
		case 0: //M49: CSM RCS Fuel Remaining
			GenericDoubleInput(&GC->rtcc->med_m49.CSMRCSFuelRemaining, "Input CSM RCS fuel in lbs (negative number for no update):", 0.45359237);
			break;
		case 1: //M50: S-IVB Weight
			GenericDoubleInput(&GC->rtcc->med_m50.SIVBWT, "Input S-IVB stage mass in lbs (negative number for no update):", 0.45359237);
			break;
		case 2: //M51: S-IVB Area
			GenericDoubleInput(&GC->rtcc->med_m51.SIVBArea, "Input S-IVB area in square feet (negative number for no update):", 0.3048*0.3048);
			break;
		case 3: //M55: GET to start venting/initial weights
			GenericGETInput(&GC->rtcc->med_m55.VentingGET, "Time of initial weights/start of S-IVB venting (negative number for no update):");
			break;
		}
		break;
	case 2: //Fourth option on MED
		switch (G->mptinitmode)
		{
		case 0: //M49: S-IVB Fuel Remaining
			GenericDoubleInput(&GC->rtcc->med_m49.SIVBFuelRemaining, "Input S-IVB fuel in lbs (negative number for no update):", 0.45359237);
			break;
		case 1: //M50: LM Total Weight
			GenericDoubleInput(&GC->rtcc->med_m50.LMWT, "Input LM total mass in lbs (negative number for no update):", 0.45359237);
			break;
		case 2: //M51: LM Ascent Area
			GenericDoubleInput(&GC->rtcc->med_m51.LMAscentArea, "Input LM ascent stage area in square feet (negative number for no update):", 0.3048*0.3048);
			break;
		case 3: //M55: Delta Docking Angle
			GenericDoubleInput(&GC->rtcc->med_m55.DeltaDockingAngle, "Delta docking angle (smaller than -360° for no update):", RAD);
			break;
		}
		break;
	case 3: //Fifth option on MED
		switch (G->mptinitmode)
		{
		case 0: //M49: LM APS Fuel Remaining
			GenericDoubleInput(&GC->rtcc->med_m49.LMAPSFuelRemaining, "Input LM APS fuel in lbs (negative number for no update):", 0.45359237);
			break;
		case 1: //M50: LM Ascent Weight
			GenericDoubleInput(&GC->rtcc->med_m50.LMASCWT, "Input LM ascent stage mass in lbs (negative number for no update):", 0.45359237);
			break;
		case 2: //M51: LM Descent Area
			GenericDoubleInput(&GC->rtcc->med_m51.LMDescentArea, "Input LM descent stage area in square feet (negative number for no update):", 0.3048*0.3048);
			break;
		}
		break;
	case 4: //Sixth option on MED
		switch (G->mptinitmode)
		{
		case 0: //M49: LM RCS Fuel Remaining
			GenericDoubleInput(&GC->rtcc->med_m49.LMRCSFuelRemaining, "Input LM RCS fuel in lbs (negative number for no update):", 0.45359237);
			break;
		case 1: //M50: Time of weights
			GenericGETInput(&GC->rtcc->med_m50.WeightGET, "GET when weights are valid:");
			break;
		case 2: //M51: K-Factor
			GenericDoubleInput(&GC->rtcc->med_m51.KFactor, "Input K-Factor feet (-30 to +30):", 1.0);
			break;
		}
		break;
	case 5: //Seventh option on MED
		switch (G->mptinitmode)
		{
		case 0: //M49: LM DPS Fuel Remaining
			GenericDoubleInput(&GC->rtcc->med_m49.LMDPSFuelRemaining, "Input DPS fuel in lbs (negative number for no update):", 0.45359237);
			break;
		}
		break;
	}
}

bool MPTInitM55ConfigInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->set_MPTInitM55Config(str);
	return true;
}

void ApolloRTCCMFD::set_MPTInitM55Config(char *cfg)
{
	GC->rtcc->med_m55.ConfigCode.assign(cfg);
}

void ApolloRTCCMFD::menuMPTUpdate()
{
	int med;

	switch (G->mptinitmode)
	{
	case 0:
		med = 49;
		break;
	case 1:
		med = 50;
		break;
	case 2:
		med = 51;
		break;
	case 3:
		med = 55;
		break;
	default:
		return;
	}

	if (GC->rtcc->PMMWTC(med))
	{
		GC->mptInitError = 2;
	}
	else
	{
		GC->mptInitError = 1;
	}
}

void ApolloRTCCMFD::menuMPTTrajectoryUpdateCSM()
{
	bool DifferentialCorrectionSolutionCSMInput(void* id, char *str, void *data);
	oapiOpenInputBox("Choose vessel for the CSM ground tracking solution:", DifferentialCorrectionSolutionCSMInput, 0, 50, (void*)this);
}

bool DifferentialCorrectionSolutionCSMInput(void* id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		return ((ApolloRTCCMFD*)data)->set_DifferentialCorrectionSolution(str, true);
	}
	return false;
}

void ApolloRTCCMFD::menuMPTTrajectoryUpdateLEM()
{
	bool DifferentialCorrectionSolutionLEMInput(void* id, char *str, void *data);
	oapiOpenInputBox("Choose vessel for the LEM ground tracking solution:", DifferentialCorrectionSolutionLEMInput, 0, 50, (void*)this);
}

bool DifferentialCorrectionSolutionLEMInput(void* id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		return ((ApolloRTCCMFD*)data)->set_DifferentialCorrectionSolution(str, false);
	}
	return false;
}

bool ApolloRTCCMFD::set_DifferentialCorrectionSolution(char *str, bool csm)
{
	//To update display immediately
	GC->rtcc->VectorPanelSummaryBuffer.gmt = -10000000000000.0;

	OBJHANDLE hVessel = oapiGetVesselByName(str);
	if (hVessel)
	{
		VESSEL *v = oapiGetVesselInterface(hVessel);
		if (v)
		{
			if (GC->MPTTrajectoryUpdate(v, csm))
			{
				return false;
			}
			return true;
		}
	}
	return false;
}

void ApolloRTCCMFD::menuMoveToEvalTableCSM()
{
	bool MoveToEvalTableCSMInput(void* id, char *str, void *data);
	oapiOpenInputBox("Move CSM telemetry vector to evaluation vector table. Input: CMC, LGC, AGS or IU", MoveToEvalTableCSMInput, 0, 50, (void*)this);
}

bool MoveToEvalTableCSMInput(void* id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		std::string str(str);
		int code;
		if (str == "CMC")
		{
			code = 300;
		}
		else if (str == "LGC")
		{
			code = 301;
		}
		else if (str == "AGS")
		{
			code = 302;
		}
		else if (str == "IU")
		{
			code = 303;
		}
		else
		{
			return false;
		}

		((ApolloRTCCMFD*)data)->VectorControlPBI(code);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuMoveToEvalTableLEM()
{
	bool MoveToEvalTableLEMInput(void* id, char *str, void *data);
	oapiOpenInputBox("Move LEM telemetry vector to evaluation vector table. Input: CMC, LGC, AGS or IU", MoveToEvalTableLEMInput, 0, 50, (void*)this);
}

bool MoveToEvalTableLEMInput(void* id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		std::string str(str);
		int code;
		if (str == "CMC")
		{
			code = 308;
		}
		else if (str == "LGC")
		{
			code = 309;
		}
		else if (str == "AGS")
		{
			code = 310;
		}
		else if (str == "IU")
		{
			code = 311;
		}
		else
		{
			return false;
		}

		((ApolloRTCCMFD*)data)->VectorControlPBI(code);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuMoveToUsableTableCSM()
{
	bool MoveToUsableTableCSMInput(void* id, char *str, void *data);
	oapiOpenInputBox("Move CSM telemetry vector to usable vector table. Input: CMC, LGC, AGS or IU", MoveToUsableTableCSMInput, 0, 50, (void*)this);
}

bool MoveToUsableTableCSMInput(void* id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		std::string str(str);
		int code;
		if (str == "CMC")
		{
			code = 316;
		}
		else if (str == "LGC")
		{
			code = 317;
		}
		else if (str == "AGS")
		{
			code = 318;
		}
		else if (str == "IU")
		{
			code = 319;
		}
		else
		{
			return false;
		}

		((ApolloRTCCMFD*)data)->VectorControlPBI(code);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuMoveToUsableTableLEM()
{
	bool MoveToUsableTableLEMInput(void* id, char *str, void *data);
	oapiOpenInputBox("Move LEM telemetry vector to usable vector table. Input: CMC, LGC, AGS or IU", MoveToUsableTableLEMInput, 0, 50, (void*)this);
}

bool MoveToUsableTableLEMInput(void* id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		std::string str(str);
		int code;
		if (str == "CMC")
		{
			code = 320;
		}
		else if (str == "LGC")
		{
			code = 321;
		}
		else if (str == "AGS")
		{
			code = 322;
		}
		else if (str == "IU")
		{
			code = 323;
		}
		else
		{
			return false;
		}

		((ApolloRTCCMFD*)data)->VectorControlPBI(code);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuEphemerisUpdateCSM()
{
	bool EphemerisUpdateCSMInput(void* id, char *str, void *data);
	oapiOpenInputBox("Move CSM vector to ephemeris update. Input: CMC, LGC, AGS, IU, HSR or DC", EphemerisUpdateCSMInput, 0, 50, (void*)this);
}

bool EphemerisUpdateCSMInput(void* id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		std::string str(str);
		int code;
		if (str == "CMC")
		{
			code = 324;
		}
		else if (str == "LGC")
		{
			code = 325;
		}
		else if (str == "AGS")
		{
			code = 326;
		}
		else if (str == "IU")
		{
			code = 327;
		}
		else if (str == "HSR")
		{
			code = 328;
		}
		else if (str == "DC")
		{
			code = 329;
		}
		else
		{
			return false;
		}

		((ApolloRTCCMFD*)data)->VectorControlPBI(code);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuEphemerisUpdateLEM()
{
	bool EphemerisUpdateLEMInput(void* id, char *str, void *data);
	oapiOpenInputBox("Move LEM vector to ephemeris update. Input: CMC, LGC, AGS, IU, HSR or DC", EphemerisUpdateLEMInput, 0, 50, (void*)this);
}

bool EphemerisUpdateLEMInput(void* id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		std::string str(str);
		int code;
		if (str == "CMC")
		{
			code = 330;
		}
		else if (str == "LGC")
		{
			code = 331;
		}
		else if (str == "AGS")
		{
			code = 332;
		}
		else if (str == "IU")
		{
			code = 333;
		}
		else if (str == "HSR")
		{
			code = 334;
		}
		else if (str == "DC")
		{
			code = 335;
		}
		else
		{
			return false;
		}

		((ApolloRTCCMFD*)data)->VectorControlPBI(code);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::VectorControlPBI(int code)
{
	//To update display immediately
	GC->rtcc->VectorPanelSummaryBuffer.gmt = -10000000000000.0;

	GC->rtcc->BMSVPS(0, code);
}

void ApolloRTCCMFD::menuMPTInitAutoUpdate()
{
	if (G->mptinitmode != 2)
	{
		GC->MPTMassUpdate();
	}
}

void ApolloRTCCMFD::menuMPTInitM50M55Vehicle()
{
	if (GC->rtcc->med_m49.Table == RTCC_MPT_CSM)
	{
		set_CSMVessel();
	}
	else
	{
		set_LMVessel();
	}
}

void ApolloRTCCMFD::CheckoutMonitorCalc()
{
	bool CheckoutMonitorCalcInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: U02, CSM or LEM, Indicator (GMT,GET,MVI,MVE,RAD,ALT,FPA,LAT,LNG), Parameter, Threshold Time (opt.), Reference (ECI,ECT,MCI,MCT) (opt.), FT (opt.);", CheckoutMonitorCalcInput, 0, 50, (void*)this);
}

bool CheckoutMonitorCalcInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

bool REFSMMATGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_REFSMMATTime(t1time);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_lambertelev(double elev)
{
	GC->rtcc->GZGENCSN.TIElevationAngle = elev*RAD;
}

void ApolloRTCCMFD::calcREFSMMAT()
{
	G->startSubthread(4, IsCSM);
}

void ApolloRTCCMFD::menuLSLat()
{
	bool LSLatInput(void* id, char *str, void *data);
	oapiOpenInputBox("Latitude in degrees:", LSLatInput, 0, 20, (void*)this);
}

bool LSLatInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LSLat(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LSLat(double lat)
{
	this->GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST] = lat*RAD;
}

void ApolloRTCCMFD::menuLSLng()
{
	bool LSLngInput(void* id, char *str, void *data);
	oapiOpenInputBox("Longitude in degrees:", LSLngInput, 0, 20, (void*)this);
}

bool LSLngInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LSLng(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LSLng(double lng)
{
	this->GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST] = lng*RAD;
}

void ApolloRTCCMFD::GMPInput1Dialogue()
{
	bool GMPInput1Input(void* id, char *str, void *data);
	//Desired Maneuver Height
	if (G->GMPManeuverCode == RTCC_GMP_CRH || G->GMPManeuverCode == RTCC_GMP_HBH || G->GMPManeuverCode == RTCC_GMP_FCH || G->GMPManeuverCode == RTCC_GMP_CPH ||
		G->GMPManeuverCode == RTCC_GMP_CNH || G->GMPManeuverCode == RTCC_GMP_PCH || G->GMPManeuverCode == RTCC_GMP_NSH || G->GMPManeuverCode == RTCC_GMP_HOH)
	{
		oapiOpenInputBox("Maneuver height in NM:", GMPInput1Input, 0, 20, (void*)this);
	}
	//Desired Maneuver Longitude
	else if (G->GMPManeuverCode == RTCC_GMP_PCL || G->GMPManeuverCode == RTCC_GMP_CRL || G->GMPManeuverCode == RTCC_GMP_HOL || G->GMPManeuverCode == RTCC_GMP_NSL ||
		G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_NHL || G->GMPManeuverCode == RTCC_GMP_SAL || G->GMPManeuverCode == RTCC_GMP_PHL ||
		G->GMPManeuverCode == RTCC_GMP_CPL || G->GMPManeuverCode == RTCC_GMP_HBL || G->GMPManeuverCode == RTCC_GMP_CNL || G->GMPManeuverCode == RTCC_GMP_HNL ||
		G->GMPManeuverCode == RTCC_GMP_SAA || G->GMPManeuverCode == RTCC_GMP_HAS)
	{
		oapiOpenInputBox("Maneuver longitude in degrees:", GMPInput1Input, 0, 20, (void*)this);
	}
}

bool GMPInput1Input(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_GMPInput1(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_GMPInput1(double val)
{
	//Desired Maneuver Height
	if (G->GMPManeuverCode == RTCC_GMP_CRH || G->GMPManeuverCode == RTCC_GMP_HBH || G->GMPManeuverCode == RTCC_GMP_FCH || G->GMPManeuverCode == RTCC_GMP_CPH ||
		G->GMPManeuverCode == RTCC_GMP_CNH || G->GMPManeuverCode == RTCC_GMP_PCH || G->GMPManeuverCode == RTCC_GMP_NSH || G->GMPManeuverCode == RTCC_GMP_HOH)
	{
		G->GMPManeuverHeight = val * 1852.0;
	}
	//Desired Maneuver Longitude
	else if (G->GMPManeuverCode == RTCC_GMP_PCL || G->GMPManeuverCode == RTCC_GMP_CRL || G->GMPManeuverCode == RTCC_GMP_HOL || G->GMPManeuverCode == RTCC_GMP_NSL ||
		G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_NHL || G->GMPManeuverCode == RTCC_GMP_SAL || G->GMPManeuverCode == RTCC_GMP_PHL ||
		G->GMPManeuverCode == RTCC_GMP_CPL || G->GMPManeuverCode == RTCC_GMP_HBL || G->GMPManeuverCode == RTCC_GMP_CNL || G->GMPManeuverCode == RTCC_GMP_HNL ||
		G->GMPManeuverCode == RTCC_GMP_SAA || G->GMPManeuverCode == RTCC_GMP_HAS)
	{
		G->GMPManeuverLongitude = val * RAD;
	}
}

void ApolloRTCCMFD::GMPInput2Dialogue()
{
	bool GMPInput2Input(void* id, char *str, void *data);
	//Height Change
	if (G->GMPManeuverCode == RTCC_GMP_HOL || G->GMPManeuverCode == RTCC_GMP_HOT || G->GMPManeuverCode == RTCC_GMP_HAO || G->GMPManeuverCode == RTCC_GMP_HPO ||
		G->GMPManeuverCode == RTCC_GMP_HNL || G->GMPManeuverCode == RTCC_GMP_HNT || G->GMPManeuverCode == RTCC_GMP_HNA || G->GMPManeuverCode == RTCC_GMP_HNP ||
		G->GMPManeuverCode == RTCC_GMP_PHL || G->GMPManeuverCode == RTCC_GMP_PHT || G->GMPManeuverCode == RTCC_GMP_PHA || G->GMPManeuverCode == RTCC_GMP_PHP || G->GMPManeuverCode == RTCC_GMP_HOH)
	{
		oapiOpenInputBox("Height change in NM:", GMPInput2Input, 0, 20, (void*)this);
	}
	//Apoapsis Height
	else if (G->GMPManeuverCode == RTCC_GMP_HBT || G->GMPManeuverCode == RTCC_GMP_HBH || G->GMPManeuverCode == RTCC_GMP_HBO || G->GMPManeuverCode == RTCC_GMP_HBL ||
		G->GMPManeuverCode == RTCC_GMP_NHT || G->GMPManeuverCode == RTCC_GMP_NHL || G->GMPManeuverCode == RTCC_GMP_HAS)
	{
		oapiOpenInputBox("Apoapsis height in NM:", GMPInput2Input, 0, 20, (void*)this);
	}
	//Delta V
	else if (G->GMPManeuverCode == RTCC_GMP_FCT || G->GMPManeuverCode == RTCC_GMP_FCA || G->GMPManeuverCode == RTCC_GMP_FCP || G->GMPManeuverCode == RTCC_GMP_FCE ||
		G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_FCH)
	{
		oapiOpenInputBox("Delta V in ft/s:", GMPInput2Input, 0, 20, (void*)this);
	}
	//Apse line rotation
	else if (G->GMPManeuverCode == RTCC_GMP_SAT || G->GMPManeuverCode == RTCC_GMP_SAO || G->GMPManeuverCode == RTCC_GMP_SAL)
	{
		oapiOpenInputBox("Rotation angle in degrees:", GMPInput2Input, 0, 20, (void*)this);
	}
}

bool GMPInput2Input(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_GMPInput2(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_GMPInput2(double val)
{
	//Height Change
	if (G->GMPManeuverCode == RTCC_GMP_HOL || G->GMPManeuverCode == RTCC_GMP_HOT || G->GMPManeuverCode == RTCC_GMP_HAO || G->GMPManeuverCode == RTCC_GMP_HPO ||
		G->GMPManeuverCode == RTCC_GMP_HNL || G->GMPManeuverCode == RTCC_GMP_HNT || G->GMPManeuverCode == RTCC_GMP_HNA || G->GMPManeuverCode == RTCC_GMP_HNP ||
		G->GMPManeuverCode == RTCC_GMP_PHL || G->GMPManeuverCode == RTCC_GMP_PHT || G->GMPManeuverCode == RTCC_GMP_PHA || G->GMPManeuverCode == RTCC_GMP_PHP || G->GMPManeuverCode == RTCC_GMP_HOH)
	{
		G->GMPHeightChange = val * 1852.0;
	}
	//Apoapsis Height
	else if (G->GMPManeuverCode == RTCC_GMP_HBT || G->GMPManeuverCode == RTCC_GMP_HBH || G->GMPManeuverCode == RTCC_GMP_HBO || G->GMPManeuverCode == RTCC_GMP_HBL ||
		G->GMPManeuverCode == RTCC_GMP_NHT || G->GMPManeuverCode == RTCC_GMP_NHL || G->GMPManeuverCode == RTCC_GMP_HAS)
	{
		G->GMPApogeeHeight = val * 1852.0;
	}
	//Delta V
	else if (G->GMPManeuverCode == RTCC_GMP_FCT || G->GMPManeuverCode == RTCC_GMP_FCA || G->GMPManeuverCode == RTCC_GMP_FCP || G->GMPManeuverCode == RTCC_GMP_FCE ||
		G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_FCH)
	{
		G->GMPDeltaVInput = val * 0.3048;
	}
	//Apse line rotation
	else if (G->GMPManeuverCode == RTCC_GMP_SAT || G->GMPManeuverCode == RTCC_GMP_SAO || G->GMPManeuverCode == RTCC_GMP_SAL)
	{
		G->GMPApseLineRotAngle = val * RAD;
	}
}

void ApolloRTCCMFD::GMPInput3Dialogue()
{
	bool GMPInput3Input(void* id, char *str, void *data);

	//Wedge Angle
	if (G->GMPManeuverCode == RTCC_GMP_PCE || G->GMPManeuverCode == RTCC_GMP_PCL || G->GMPManeuverCode == RTCC_GMP_PCT || G->GMPManeuverCode == RTCC_GMP_PHL || 
		G->GMPManeuverCode == RTCC_GMP_PHT || G->GMPManeuverCode == RTCC_GMP_PHA || G->GMPManeuverCode == RTCC_GMP_PHP || G->GMPManeuverCode == RTCC_GMP_CPL || 
		G->GMPManeuverCode == RTCC_GMP_CPH || G->GMPManeuverCode == RTCC_GMP_CPT || G->GMPManeuverCode == RTCC_GMP_CPA || G->GMPManeuverCode == RTCC_GMP_CPP || 
		G->GMPManeuverCode == RTCC_GMP_PCH)
	{
		oapiOpenInputBox("Wedge in degrees:", GMPInput3Input, 0, 20, (void*)this);
	}
	//Node Shift
	else if (G->GMPManeuverCode == RTCC_GMP_NST || G->GMPManeuverCode == RTCC_GMP_NSO || G->GMPManeuverCode == RTCC_GMP_NSH || G->GMPManeuverCode == RTCC_GMP_NSL ||
		G->GMPManeuverCode == RTCC_GMP_CNL || G->GMPManeuverCode == RTCC_GMP_CNH || G->GMPManeuverCode == RTCC_GMP_CNT ||  G->GMPManeuverCode == RTCC_GMP_CNA || 
		G->GMPManeuverCode == RTCC_GMP_CNP || G->GMPManeuverCode == RTCC_GMP_HNL || G->GMPManeuverCode == RTCC_GMP_HNT || G->GMPManeuverCode == RTCC_GMP_HNA || G->GMPManeuverCode == RTCC_GMP_HNP)
	{
		oapiOpenInputBox("Node shift in degrees:", GMPInput3Input, 0, 20, (void*)this);
	}
	//Periapsis Height
	else if (G->GMPManeuverCode == RTCC_GMP_HBT || G->GMPManeuverCode == RTCC_GMP_HBH || G->GMPManeuverCode == RTCC_GMP_HBO || G->GMPManeuverCode == RTCC_GMP_HBL ||
		G->GMPManeuverCode == RTCC_GMP_NHT || G->GMPManeuverCode == RTCC_GMP_NHL || G->GMPManeuverCode == RTCC_GMP_HAS)
	{
		oapiOpenInputBox("Periapsis height in NM:", GMPInput3Input, 0, 20, (void*)this);
	}
	//Pitch
	else if (G->GMPManeuverCode == RTCC_GMP_FCT || G->GMPManeuverCode == RTCC_GMP_FCA || G->GMPManeuverCode == RTCC_GMP_FCP || G->GMPManeuverCode == RTCC_GMP_FCE ||
		G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_FCH)
	{
		oapiOpenInputBox("Pitch in degrees:", GMPInput3Input, 0, 20, (void*)this);
	}
}

bool GMPInput3Input(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_GMPInput3(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_GMPInput3(double val)
{
	//Wedge Angle
	if (G->GMPManeuverCode == RTCC_GMP_PCE || G->GMPManeuverCode == RTCC_GMP_PCL || G->GMPManeuverCode == RTCC_GMP_PCT || G->GMPManeuverCode == RTCC_GMP_PHL ||
		G->GMPManeuverCode == RTCC_GMP_PHT || G->GMPManeuverCode == RTCC_GMP_PHA || G->GMPManeuverCode == RTCC_GMP_PHP || G->GMPManeuverCode == RTCC_GMP_CPL ||
		G->GMPManeuverCode == RTCC_GMP_CPH || G->GMPManeuverCode == RTCC_GMP_CPT || G->GMPManeuverCode == RTCC_GMP_CPA || G->GMPManeuverCode == RTCC_GMP_CPP ||
		G->GMPManeuverCode == RTCC_GMP_PCH)
	{
		G->GMPWedgeAngle = val * RAD;
	}
	//Node Shift
	else if (G->GMPManeuverCode == RTCC_GMP_NST || G->GMPManeuverCode == RTCC_GMP_NSO || G->GMPManeuverCode == RTCC_GMP_NSH || G->GMPManeuverCode == RTCC_GMP_NSL ||
		G->GMPManeuverCode == RTCC_GMP_CNL || G->GMPManeuverCode == RTCC_GMP_CNH || G->GMPManeuverCode == RTCC_GMP_CNT || G->GMPManeuverCode == RTCC_GMP_CNA || G->GMPManeuverCode == RTCC_GMP_CNP || 
		G->GMPManeuverCode == RTCC_GMP_HNL || G->GMPManeuverCode == RTCC_GMP_HNT || G->GMPManeuverCode == RTCC_GMP_HNA || G->GMPManeuverCode == RTCC_GMP_HNP)
	{
		G->GMPNodeShiftAngle = val * RAD;
	}
	//Periapsis Height
	else if (G->GMPManeuverCode == RTCC_GMP_HBT || G->GMPManeuverCode == RTCC_GMP_HBH || G->GMPManeuverCode == RTCC_GMP_HBO || G->GMPManeuverCode == RTCC_GMP_HBL ||
		G->GMPManeuverCode == RTCC_GMP_NHT || G->GMPManeuverCode == RTCC_GMP_NHL || G->GMPManeuverCode == RTCC_GMP_HAS)
	{
		G->GMPPerigeeHeight = val * 1852.0;
	}
	//Pitch
	else if (G->GMPManeuverCode == RTCC_GMP_FCT || G->GMPManeuverCode == RTCC_GMP_FCA || G->GMPManeuverCode == RTCC_GMP_FCP || G->GMPManeuverCode == RTCC_GMP_FCE ||
		G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_FCH)
	{
		G->GMPPitch = val * RAD;
	}
}

void ApolloRTCCMFD::GMPInput4Dialogue()
{
	bool GMPInput4Input(void* id, char *str, void *data);

	//Yaw
	if (G->GMPManeuverCode == RTCC_GMP_FCT || G->GMPManeuverCode == RTCC_GMP_FCA || G->GMPManeuverCode == RTCC_GMP_FCP || G->GMPManeuverCode == RTCC_GMP_FCE ||
		G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_FCH)
	{
		oapiOpenInputBox("Yaw in degrees:", GMPInput4Input, 0, 20, (void*)this);
	}
	//Node Shift
	else if (G->GMPManeuverCode == RTCC_GMP_NHT || G->GMPManeuverCode == RTCC_GMP_NHL)
	{
		oapiOpenInputBox("Node shift in degrees:", GMPInput4Input, 0, 20, (void*)this);
	}
	//Rev counter
	else if (G->GMPManeuverCode == RTCC_GMP_HAS)
	{
		OrbAdjRevDialogue();
	}
}

bool GMPInput4Input(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_GMPInput4(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_GMPInput4(double val)
{
	if (G->GMPManeuverCode == RTCC_GMP_FCT || G->GMPManeuverCode == RTCC_GMP_FCA || G->GMPManeuverCode == RTCC_GMP_FCP || G->GMPManeuverCode == RTCC_GMP_FCE ||
		G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_FCH)
	{
		G->GMPYaw = val * RAD;
	}
	//Node Shift
	else if (G->GMPManeuverCode == RTCC_GMP_NHT || G->GMPManeuverCode == RTCC_GMP_NHL)
	{
		G->GMPNodeShiftAngle = val * RAD;
	}
}

void ApolloRTCCMFD::menuDKINSRDHInput()
{
	GenericDoubleInput(&GC->rtcc->GZGENCSN.DKIDeltaH_NSR, "Enter DH at NSR:", 1852.0);
}

void ApolloRTCCMFD::menuDKINCCDHInput()
{
	GenericDoubleInput(&GC->rtcc->GZGENCSN.DKIDeltaH_NCC, "Enter DH at NCC:", 1852.0);
}

void ApolloRTCCMFD::SPQDHdialogue()
{
	bool SPQDHInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the DH:", SPQDHInput, 0, 20, (void*)this);
}

bool SPQDHInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_SPQDH(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SPQDH(double DH)
{
	this->GC->rtcc->GZGENCSN.SPQDeltaH = DH * 1852.0;
}

void ApolloRTCCMFD::menuSetSVTime()
{
	bool SVGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for the state vector (Format: hhh:mm:ss). Negative value for present GET.", SVGETInput, 0, 20, (void*)this);
}

bool SVGETInput(void *id, char *str, void *data)
{
	int hh, mm;
	double ss, SVtime;

	if (sscanf(str, "%d:%d:%lf", &hh, &mm, &ss) == 3)
	{
		SVtime = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_SVtime(SVtime);

		return true;
	}
	else if (sscanf(str, "%lf", &ss) == 1)
	{
		((ApolloRTCCMFD*)data)->set_SVtime(0);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SVtime(double SVtime)
{
	G->SVDesiredGET = SVtime;
}

void ApolloRTCCMFD::menuUpdateGRRTime()
{
	G->UpdateGRRTime(G->iuvessel);
}

void ApolloRTCCMFD::menuSetAGSKFactor()
{
	bool AGSKFactorInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the AGS time bias (Format: hhh:mm:ss)", AGSKFactorInput, 0, 20, (void*)this);
}

bool AGSKFactorInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_AGSKFactor(str);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_AGSKFactor(char *str)
{
	char Buff[128];

	sprintf_s(Buff, "P15,AGS,,%s;", str);
	GeneralMEDRequest(Buff);
}

void ApolloRTCCMFD::menuGetAGSKFactor()
{
	G->GetAGSKFactor();
}

void ApolloRTCCMFD::menuCycleK30Vehicle()
{
	if (GC->rtcc->med_k30.Vehicle == 1)
	{
		GC->rtcc->med_k30.Vehicle = 3;
	}
	else
	{
		GC->rtcc->med_k30.Vehicle = 1;
	}
}

void ApolloRTCCMFD::menuSLVLaunchTargetingPad()
{
	if (GC->rtcc->PZSLVCON.Pad == 1)
	{
		GC->rtcc->PZSLVCON.Pad = 2;
	}
	else
	{
		GC->rtcc->PZSLVCON.Pad = 1;
	}
}

void ApolloRTCCMFD::menuSLVLaunchTargeting()
{
	G->SkylabSaturnIBLaunchCalc();
}

void ApolloRTCCMFD::menuSLVInsertionSVtoMPT()
{
	if (GC->MissionPlanningActive == false) return;
	if (GC->rtcc->GZLTRA.GMT_T == 0.0) return;

	StateVectorTableEntry sv0;
	int L;

	if (GC->rtcc->PZSLVCON.Pad == 1)
	{
		L = RTCC_MPT_CSM;
	}
	else
	{
		L = RTCC_MPT_LM;
	}

	sv0.VectorCode = "NOMINS";
	sv0.LandingSiteIndicator = false;
	sv0.Vector.R = GC->rtcc->GZLTRA.R_T;
	sv0.Vector.V = GC->rtcc->GZLTRA.V_T;
	sv0.Vector.GMT = GC->rtcc->GZLTRA.GMT_T;
	sv0.Vector.RBI = BODY_EARTH;

	GC->rtcc->PMSVCT(0, L, sv0);
}

void ApolloRTCCMFD::menuSLVLaunchUplink()
{
	G->SkylabSaturnIBLaunchUplink();
}

void ApolloRTCCMFD::menuSLVTLITargetingUplink()
{
	G->SaturnVTLITargetUplink();
}

void ApolloRTCCMFD::set_Vessel()
{
	if (IsCSM)
	{
		set_CSMVessel();
	}
	else
	{
		set_LMVessel();
	}
}

void ApolloRTCCMFD::set_CSMVessel()
{
	CycleThroughVessels(&GC->rtcc->pCSM);
}

void ApolloRTCCMFD::set_LMVessel()
{
	CycleThroughVessels(&GC->rtcc->pLM);
}

void ApolloRTCCMFD::set_IUVessel()
{
	CycleThroughVessels(&G->iuvessel);
}

void ApolloRTCCMFD::set_TargetVessel()
{
	CycleThroughVessels(&G->Rendezvous_Target);
}

void ApolloRTCCMFD::CycleThroughVessels(VESSEL **v) const
{
	VESSEL *pVessel;
	OBJHANDLE hVessel;
	int vesselcount, i;

	i = -1;
	vesselcount = oapiGetVesselCount();

	//Get current vessel number
	if (v != NULL)
	{
		for (i = 0; i < vesselcount; i++)
		{
			hVessel = oapiGetVesselByIndex(i);
			if (hVessel)
			{
				pVessel = oapiGetVesselInterface(hVessel);

				if (*v == pVessel) break;
			}
		}
	}

	if (i < vesselcount - 1)
	{
		i++;
	}
	else
	{
		i = 0;
	}

	*v = oapiGetVesselInterface(oapiGetVesselByIndex(i));
}

void ApolloRTCCMFD::SPQcalc()
{
	G->startSubthread(2);
}

void ApolloRTCCMFD::lambertcalc()
{
	G->startSubthread(1);
}

void ApolloRTCCMFD::menuDeorbitCalc()
{
	G->DeorbitCalc();
}

void ApolloRTCCMFD::menuCycleRetrofireType()
{
	if (GC->rtcc->RZJCTTC.R32_Code < 2)
	{
		GC->rtcc->RZJCTTC.R32_Code++;
	}
	else
	{
		GC->rtcc->RZJCTTC.R32_Code = 1;
	}
}

void ApolloRTCCMFD::menuRetrofireGETIDialogue()
{
	GenericGETInput(&GC->rtcc->RZJCTTC.R32_GETI, "Choose the GET (Format: hhh:mm:ss)");
}

void ApolloRTCCMFD::menuRetrofireLatDialogue()
{
	GenericDoubleInput(&GC->rtcc->RZJCTTC.R32_lat_T, "Latitude in degree (°), enter -720 or less for no iteration on latitude:", RAD);
}

void ApolloRTCCMFD::menuRetrofireLngDialogue()
{
	GenericDoubleInput(&GC->rtcc->RZJCTTC.R32_lng_T, "Longitude in degree (°):", RAD);
}

void ApolloRTCCMFD::menuSwitchRetrofireEngine()
{
	if (GC->rtcc->RZJCTTC.R31_Thruster < RTCC_ENGINETYPE_CSMRCSMINUS4)
	{
		GC->rtcc->RZJCTTC.R31_Thruster++;
	}
	else if (GC->rtcc->RZJCTTC.R31_Thruster == RTCC_ENGINETYPE_CSMRCSMINUS4)
	{
		GC->rtcc->RZJCTTC.R31_Thruster = RTCC_ENGINETYPE_CSMSPS;
	}
	else
	{
		GC->rtcc->RZJCTTC.R31_Thruster = RTCC_ENGINETYPE_CSMRCSPLUS2;
	}

	//BurnMode 3 is only compatible with SPS
	if (GC->rtcc->RZJCTTC.R31_Thruster != RTCC_ENGINETYPE_CSMSPS && GC->rtcc->RZJCTTC.R31_BurnMode == 3)
	{
		GC->rtcc->RZJCTTC.R31_BurnMode = 1;
	}
}

void ApolloRTCCMFD::menuSwitchRetrofireBurnMode()
{
	if (GC->rtcc->RZJCTTC.R31_BurnMode < 3)
	{
		GC->rtcc->RZJCTTC.R31_BurnMode++;
	}
	else
	{
		GC->rtcc->RZJCTTC.R31_BurnMode = 1;
	}

	//BurnMode 3 is only compatible with SPS
	if (GC->rtcc->RZJCTTC.R31_BurnMode == 3 && GC->rtcc->RZJCTTC.R31_Thruster != RTCC_ENGINETYPE_CSMSPS)
	{
		GC->rtcc->RZJCTTC.R31_BurnMode = 1;
	}
}

void ApolloRTCCMFD::menuChooseRetrofireValue()
{
	if (GC->rtcc->RZJCTTC.R31_BurnMode < 3)
	{
		bool ChooseRetrofireValueInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose retrofire DV in ft/s or DT in seconds:", ChooseRetrofireValueInput, 0, 20, (void*)this);
	}
}

bool ChooseRetrofireValueInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_RetrofireValue(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_RetrofireValue(double val)
{
	if (GC->rtcc->RZJCTTC.R31_BurnMode == 1)
	{
		GC->rtcc->RZJCTTC.R31_dv = val * 0.3048;
	}
	else
	{
		GC->rtcc->RZJCTTC.R31_dt = val;
	}
}

void ApolloRTCCMFD::menuSwitchRetrofireAttitudeMode()
{
	if (GC->rtcc->RZJCTTC.R31_AttitudeMode < 2)
	{
		GC->rtcc->RZJCTTC.R31_AttitudeMode++;
	}
	else
	{
		GC->rtcc->RZJCTTC.R31_AttitudeMode = 1;
	}
}

void ApolloRTCCMFD::menuSwitchRetrofireGimbalIndicator()
{
	GC->rtcc->RZJCTTC.R31_GimbalIndicator = -GC->rtcc->RZJCTTC.R31_GimbalIndicator;
}

void ApolloRTCCMFD::menuChooseRetrofireAttitude()
{
	if (GC->rtcc->RZJCTTC.R31_AttitudeMode == 1)
	{
		bool ChooseRetrofireAttitudeInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose LVLH burn attitude in roll, pitch, yaw:", ChooseRetrofireAttitudeInput, 0, 20, (void*)this);
	}
}

bool ChooseRetrofireAttitudeInput(void *id, char *str, void *data)
{
	VECTOR3 att;
	if (sscanf(str, "%lf %lf %lf", &att.x, &att.y, &att.z) == 3)
	{
		((ApolloRTCCMFD*)data)->set_RetrofireAttitude(att);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_RetrofireAttitude(VECTOR3 att)
{
	GC->rtcc->RZJCTTC.R31_LVLHAttitude = att * RAD;
}

void ApolloRTCCMFD::menuChooseRetrofireK1()
{
	bool ChooseRetrofireK1Input(void *id, char *str, void *data);
	oapiOpenInputBox("Choose initial bank angle during reentry:", ChooseRetrofireK1Input, 0, 20, (void*)this);
}

bool ChooseRetrofireK1Input(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_RetrofireK1(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_RetrofireK1(double val)
{
	GC->rtcc->RZJCTTC.R31_InitialBankAngle = val * RAD;
}

void ApolloRTCCMFD::menuChooseRetrofireGs()
{
	bool ChooseRetrofireGsInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose G-level for maneuver to final bank angle:", ChooseRetrofireGsInput, 0, 20, (void*)this);
}

bool ChooseRetrofireGsInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		return ((ApolloRTCCMFD*)data)->set_RetrofireGs(atof(str));
	}
	return false;
}

bool ApolloRTCCMFD::set_RetrofireGs(double val)
{
	if (val >= 0.0 && val <= 1.0)
	{
		GC->rtcc->RZJCTTC.R31_GLevel = val;
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuChooseRetrofireK2()
{
	bool ChooseRetrofireK2Input(void *id, char *str, void *data);
	oapiOpenInputBox("Choose final bank angle during reentry:", ChooseRetrofireK2Input, 0, 20, (void*)this);
}

bool ChooseRetrofireK2Input(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_RetrofireK2(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_RetrofireK2(double val)
{
	GC->rtcc->RZJCTTC.R31_FinalBankAngle = val * RAD;
}

void ApolloRTCCMFD::menuChooseRetrofireUllage()
{
	bool ChooseRetrofireUllageInput(void *id, char *str, void *data);
	oapiOpenInputBox("Number of ullage thrusters (2 or 4) and duration (e.g. '4 15'):", ChooseRetrofireUllageInput, 0, 20, (void*)this);
}

bool ChooseRetrofireUllageInput(void *id, char *str, void *data)
{
	int num;
	double dt;
	if (sscanf(str, "%d %lf", &num, &dt) == 2)
	{
		return ((ApolloRTCCMFD*)data)->set_RetrofireUllage(num, dt);
	}
	return false;
}

bool ApolloRTCCMFD::set_RetrofireUllage(int num, double dt)
{
	if ((num == 2 || num == 4) && (dt == 0.0 || dt >= 1.0))
	{
		if (num == 4)
		{
			GC->rtcc->RZJCTTC.R31_Use4UllageThrusters = true;
		}
		else
		{
			GC->rtcc->RZJCTTC.R31_Use4UllageThrusters = false;
		}
		GC->rtcc->RZJCTTC.R31_UllageTime = dt;

		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuEntryUpdateCalc()
{
	G->EntryUpdateCalc();
}

void ApolloRTCCMFD::menuEntryCalc()
{
	G->EntryCalc();
}

void ApolloRTCCMFD::menuSaveSplashdownTarget()
{
	bool SaveSplashdownTargetInput(void *id, char *str, void *data);
	oapiOpenInputBox("Save splashdown target. Enter P for Primary or M for Manual Column:", SaveSplashdownTargetInput, 0, 50, (void*)this);
}

bool SaveSplashdownTargetInput(void *id, char *str, void *data)
{
	return ((ApolloRTCCMFD*)data)->set_SaveSplashdownTarget(str);
}

bool ApolloRTCCMFD::set_SaveSplashdownTarget(char *str)
{
	if (str[0] == 'P')
	{
		if (GC->rtcc->PZREAP.RTEDTable[0].RTEDCode != "")
		{
			GC->rtcc->RZDBSC1.lat_T = GC->rtcc->PZREAP.RTEDTable[0].lat_imp_tgt;
			GC->rtcc->RZDBSC1.lng_T = GC->rtcc->PZREAP.RTEDTable[0].lng_imp_tgt;
			return true;
		}
	}
	else if (str[1] == 'M')
	{
		if (GC->rtcc->PZREAP.RTEDTable[1].RTEDCode != "")
		{
			GC->rtcc->RZDBSC1.lat_T = GC->rtcc->PZREAP.RTEDTable[1].lat_imp_tgt;
			GC->rtcc->RZDBSC1.lng_T = GC->rtcc->PZREAP.RTEDTable[1].lng_imp_tgt;
			return true;
		}
	}

	return false;
}

void ApolloRTCCMFD::LoadSplashdownTargetToRTEDManualInput()
{
	GC->rtcc->med_f81.lat_tgt = GC->rtcc->RZDBSC1.lat_T;
	GC->rtcc->med_f81.lng_tgt = GC->rtcc->RZDBSC1.lng_T;
}

void ApolloRTCCMFD::menuTransferRTEToMPT()
{
	if (GC->MissionPlanningActive)
	{
		bool TransferRTEMPTInput(void *id, char *str, void *data);
		oapiOpenInputBox("Format: M74,MPT (CSM or LEM), Replace Code (1-15 or missing), Maneuver Type (TTFM for deorbit, RTEP for RTE primary column, RTEM for manual column);", TransferRTEMPTInput, 0, 50, (void*)this);
	}
	else
	{
		bool TransferRTEInput(void *id, char *str, void *data);
		oapiOpenInputBox("Make burn solution available for Maneuver PAD etc. (enter RTEP for RTE primary column, RTEM for manual. TTFM for deorbit burn)", TransferRTEInput, 0, 50, (void*)this);
	}
}

bool TransferRTEInput(void *id, char *str, void *data)
{
	return ((ApolloRTCCMFD*)data)->set_RTESolution(str);
}

bool ApolloRTCCMFD::set_RTESolution(char *str)
{
	int i;

	if (strcmp(str, "RTEP") == 0)
	{
		i = 0;
	}
	else if (strcmp(str, "RTEM") == 0)
	{
		i = 1;
	}
	else if (strcmp(str, "TTFM") == 0)
	{
		i = 2;
	}
	else
	{
		return false;
	}

	if (i < 2)
	{
		RTEDigitalSolutionTable *tab = &GC->rtcc->PZREAP.RTEDTable[i];
		if (tab->RTEDCode != "")
		{
			G->P30TIG = tab->GETI;
			G->dV_LVLH = tab->DV_XDV;
			G->manpadenginetype = tab->ThrusterCode;
			G->manpad_ullage_dt = tab->dt_ullage;
			G->manpad_ullage_opt = tab->NumQuads == 4 ? true : false;
			G->HeadsUp = tab->HeadsUpDownIndicator;
		}
	}
	else
	{
		if (GC->rtcc->RZRFDP.data[2].Indicator == 0)
		{
			G->P30TIG = GC->rtcc->RZRFDP.data[2].GETI;
			G->dV_LVLH = GC->rtcc->RZRFTT.Manual.DeltaV;
			G->manpadenginetype = GC->rtcc->RZRFTT.Manual.Thruster;
			G->manpad_ullage_dt = GC->rtcc->RZRFTT.Manual.dt_ullage;
			G->manpad_ullage_opt = GC->rtcc->RZRFTT.Manual.UllageThrusterOption;
			G->HeadsUp = true;
		}
	}
	return true;
}

bool TransferRTEMPTInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::menuGeneralMEDRequest()
{
	menuGeneralMEDRequest("Manual Entry Device Input:");
}

void ApolloRTCCMFD::menuGeneralMEDRequest(char *message)
{
	bool GeneralMEDRequestInput(void *id, char *str, void *data);
	oapiOpenInputBox(message, GeneralMEDRequestInput, 0, 50, (void*)this);
}

bool GeneralMEDRequestInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::GeneralMEDRequest(char *str)
{
	sprintf_s(GC->rtcc->RTCCMEDBUFFER, 256, str);
	G->GeneralMEDRequest();
}

void ApolloRTCCMFD::EntryRangeDialogue()
{
	GenericDoubleInput(&G->entryrange, "Choose the Entry Range in NM:", 1.0);
}

void ApolloRTCCMFD::set_SVPageTarget()
{
	if (GC->MissionPlanningActive == false)
	{
		if (subscreen == 0 || subscreen == 2)
		{
			//CSM
			set_CSMVessel();
		}
		else
		{
			//LM
			set_LMVessel();
		}
	}
}

int AGCSVUplinkType(int subscreen)
{
	//Small utility function to convert
	if (subscreen == 0) return 0;
	else if (subscreen == 1) return 9;
	else if (subscreen == 2) return 21;
	else return 20;
}

void ApolloRTCCMFD::menuSVCalc()
{
	VESSEL *v;
	if (subscreen == 0 || subscreen == 2)
	{
		v = GC->rtcc->pCSM;
	}
	else
	{
		v = GC->rtcc->pLM;
	}

	if (GC->MissionPlanningActive || (v != NULL && !v->GroundContact()))
	{
		int type = AGCSVUplinkType(subscreen);
		G->StateVectorCalc(type);
	}
}


void ApolloRTCCMFD::menuAGSSVCalc()
{
	G->AGSStateVectorCalc(IsCSM);
}

void ApolloRTCCMFD::menuLSCalc()
{
	G->LandingSiteUpdate();
}

void ApolloRTCCMFD::menuRevertRLSToPrelaunch()
{
	GeneralMEDRequest("S72,BEST,MED;");
}

void ApolloRTCCMFD::menuSVUpload()
{
	int type = AGCSVUplinkType(subscreen);
	G->StateVectorUplink(type);
}

void ApolloRTCCMFD::menuCSMLSUplinkCalc()
{
	G->CSMLSUplinkCalc();
}

void ApolloRTCCMFD::menuLMLSUplinkCalc()
{
	G->LMLSUplinkCalc();
}

void ApolloRTCCMFD::menuCSMLSUpload()
{
	G->CSMLandingSiteUplink();
}

void ApolloRTCCMFD::menuLMLSUpload()
{
	G->LMLandingSiteUplink();
}

void ApolloRTCCMFD::menuREFSMMATUplinkCalc()
{
	if (GC->MissionPlanningActive)
	{
		bool REFSMMATUplinkCalcMPTInput(void *id, char *str, void *data);
		oapiOpenInputBox("Format: C12,VEH. COMPUTER,REFSMMAT,ADDRESS; VEH. COMPUTER = CMC, LGC. REFSMMAT = CUR, PCR, TLM, MED, LCV, OST, DMT, DOD, DOK, LLA, LLD. ADDRESS = 1 for actual, 2 for desired REFSMMAT", REFSMMATUplinkCalcMPTInput, 0, 20, (void*)this);
	}
	else
	{
		bool REFSMMATUplinkCalcInput(void *id, char *str, void *data);
		oapiOpenInputBox("Format: 1 for actual REFSMMAT, 2 for desired REFSMMAT", REFSMMATUplinkCalcInput, 0, 20, (void*)this);
	}
}

bool REFSMMATUplinkCalcInput(void *id, char *str, void *data)
{
	return ((ApolloRTCCMFD*)data)->REFSMMATUplinkCalc(str);
}

bool ApolloRTCCMFD::REFSMMATUplinkCalc(char *str)
{
	int type;
	if (sscanf(str, "%d", &type) == 1)
	{
		if (type >= 1 && type <= 2)
		{
			char veh[4];
			char str2[32];
			if (screen == 53)
			{
				sprintf_s(veh, "CMC");
			}
			else
			{
				sprintf_s(veh, "LGC");
			}
			sprintf_s(str2, 32, "C12,%s,CUR,%d;", veh, type);
			GeneralMEDRequest(str2);
			return true;
		}
	}
	return false;
}

bool REFSMMATUplinkCalcMPTInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::menuCycleTwoImpulseOption()
{
	if (GC->rtcc->med_k30.IVFlag < 2)
	{
		GC->rtcc->med_k30.IVFlag++;
	}
	else
	{
		GC->rtcc->med_k30.IVFlag = 0;
	}
}

void ApolloRTCCMFD::menuSwitchHeadsUp()
{
	if (G->manpadopt == 3)
	{
		G->TLIPAD_StudyAid = !G->TLIPAD_StudyAid;
	}
	else
	{
		G->HeadsUp = !G->HeadsUp;
	}
}

void ApolloRTCCMFD::menuCalcManPAD()
{
	switch (G->manpadopt)
	{
	case 0:
	case 1:
		if (GC->MissionPlanningActive)
		{
			bool ManPADMPTInput(void *id, char *str, void *data);
			oapiOpenInputBox("Enter MPT (CSM or LEM) and maneuver number (1-15):", ManPADMPTInput, 0, 20, (void*)this);
		}
		else
		{
			if (G->manpadopt == 0)
			{
				G->ManeuverPAD(true);
			}
			else
			{
				G->ManeuverPAD(false);
			}
		}
		break;
	case 2:
		G->TPIPAD();
		break;
	case 3:
		G->TLI_PAD();
		break;
	case 4:
		G->PDI_PAD();
		break;
	}
}

bool ManPADMPTInput(void *id, char *str, void *data)
{
	std::string mptname;
	char Buff[128];
	int mpt, num;
	if (sscanf(str, "%s %d", Buff, &num) == 2)
	{
		mptname.assign(Buff);
		if (mptname == "CSM")
		{
			mpt = RTCC_MPT_CSM;
		}
		else if (mptname == "LEM")
		{
			mpt = RTCC_MPT_LM;
		}
		else return false;

		if (num <= 0 || num > 15) return false;

		((ApolloRTCCMFD*)data)->set_ManPADMPTInput(mpt, num);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_ManPADMPTInput(int mpt, int num)
{
	G->ManPADMPT = mpt;
	G->ManPADMPTManeuver = num;

	if (G->manpadopt == 0)
	{
		G->ManeuverPAD(true);
	}
	else
	{
		G->ManeuverPAD(false);
	}
}

void ApolloRTCCMFD::menuCalcEntryPAD()
{
	G->EntryPAD();
}

void ApolloRTCCMFD::menuCalcMapUpdate()
{
	G->MapUpdate(IsCSM);
}

void ApolloRTCCMFD::menuSwitchEntryPADOpt()
{
	if (GC->entrypadopt < 1)
	{
		GC->entrypadopt++;
	}
	else
	{
		GC->entrypadopt = 0;
	}
}

void ApolloRTCCMFD::menuSwitchManPADEngine()
{
	CycleThrusterOption(G->manpadenginetype);
}

void ApolloRTCCMFD::CycleThrusterOption(int &thruster)
{
	if (thruster < RTCC_ENGINETYPE_CSMRCSMINUS4)
	{
		thruster++;
	}
	else if (thruster == RTCC_ENGINETYPE_CSMRCSMINUS4)
	{
		thruster = RTCC_ENGINETYPE_LOX_DUMP;
	}
	else if (thruster < RTCC_ENGINETYPE_LMRCSMINUS4)
	{
		thruster++;
	}
	else if (thruster == RTCC_ENGINETYPE_LMRCSMINUS4)
	{
		thruster = RTCC_ENGINETYPE_CSMSPS;
	}
	else if (thruster < RTCC_ENGINETYPE_LMDPS)
	{
		thruster++;
	}
	else
	{
		thruster = RTCC_ENGINETYPE_CSMRCSPLUS2;
	}
}

void ApolloRTCCMFD::menuSwitchManPADopt()
{
	if (G->manpadopt < 4)
	{
		G->manpadopt++;
	}
	else
	{
		G->manpadopt = 0;
	}
}

void ApolloRTCCMFD::menuSwitchMapUpdate()
{
	if (G->mappage < 1)
	{
		G->mappage++;
	}
	else
	{
		G->mappage = 0;
	}
}

void ApolloRTCCMFD::menuSetMapUpdateGET()
{
	GenericGETInput(&G->mapUpdateGET, "Choose the GET for the anchor vector (Format: hhh:mm:ss)");
}

void ApolloRTCCMFD::menuCycleMapUpdatePM()
{
	G->mapUpdatePM = !G->mapUpdatePM;
}

void ApolloRTCCMFD::menuCycleSPQMode()
{
	if (G->SPQMode < 2)
	{
		G->SPQMode++;
	}
	else
	{
		G->SPQMode = 0;
	}
}

void ApolloRTCCMFD::menuCycleSPQChaser()
{
	GC->rtcc->med_k01.ChaserVehicle = 4 - GC->rtcc->med_k01.ChaserVehicle;
}

void ApolloRTCCMFD::set_CDHtimemode()
{
	if (G->CDHtimemode < 1)
	{
		G->CDHtimemode++;
	}
	else
	{
		G->CDHtimemode = 0;
	}
}

void ApolloRTCCMFD::menuSetLaunchDate()
{
	bool LaunchDateInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the launch date (Format: day:month:year)", LaunchDateInput, 0, 20, (void*)this);
}

bool LaunchDateInput(void *id, char *str, void *data)
{
	int year, month, day;
	if (sscanf(str, "%d:%d:%d", &day, &month, &year) == 3)
	{
		((ApolloRTCCMFD*)data)->set_launchdate(year, month, day);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_launchdate(int year, int month, int day)
{
	char Buff[128];
	sprintf_s(Buff, "P80,1,CSM,%d,%d,%d;", month, day, year);
	GC->rtcc->GMGMED(Buff);

	GC->rtcc->LoadLaunchDaySpecificParameters(year, month, day);
}

void ApolloRTCCMFD::menuSetLaunchTime()
{
	bool LaunchTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the launch time (Format: HH:MM:SS.SS)", LaunchTimeInput, 0, 20, (void*)this);
}

bool LaunchTimeInput(void *id, char *str, void *data)
{
	int hours, minutes;
	double seconds;

	if (sscanf(str, "%d:%d:%lf", &hours, &minutes, &seconds) == 3)
	{
		((ApolloRTCCMFD*)data)->set_LaunchTime(hours, minutes, seconds);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LaunchTime(int hours, int minutes, double seconds)
{
	char Buff[128];
	sprintf_s(Buff, "P10,CSM,%d:%d:%.2lf;", hours, minutes, seconds);
	GC->rtcc->GMGMED(Buff);
}

void ApolloRTCCMFD::menuChangeVesselStatus()
{
	if (G->vesselisdocked == false)
	{
		G->vesselisdocked = true;
	}
	else
	{
		G->vesselisdocked = false;
	}
}

void ApolloRTCCMFD::menuCycleLMStage()
{
	G->lemdescentstage = !G->lemdescentstage;
}

void ApolloRTCCMFD::menuUpdateLiftoffTime()
{
	bool UpdateLiftoffTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Update RTCC liftoff time from CMC or LGC. Choose CMC or LGC:", UpdateLiftoffTimeInput, 0, 20, (void*)this);
}

bool UpdateLiftoffTimeInput(void *id, char *str, void *data)
{
	std::string comp(str);
	bool IsCMC;

	if (comp == "CMC")
	{
		IsCMC = true;
	}
	else if (comp == "LGC")
	{
		IsCMC = false;
	}
	else
	{
		return false;
	}

	return ((ApolloRTCCMFD*)data)->set_LiftoffTime(IsCMC);
}

bool ApolloRTCCMFD::set_LiftoffTime(bool IsCMC)
{
	agc_t *agc = G->GetAGCPointer(IsCMC);

	if (agc == NULL) return false;

	double LaunchMJD;

	//Get TEPHEM in centiseconds
	double tephem = GC->rtcc->GetTEPHEMFromAGC(agc, IsCMC);

	//Calculate MJD of TEPHEM
	LaunchMJD = (tephem / 8640000.) + GC->rtcc->SystemParameters.TEPHEM0;

	//Calculate MJD at midnight of launch day
	double GMTBase = floor(LaunchMJD);
	//Calculate GMT of TEPHEM in hours
	LaunchMJD = (LaunchMJD - GMTBase)*24.0;

	int hh, mm;
	double ss;
	OrbMech::SStoHHMMSS(LaunchMJD*3600.0, hh, mm, ss, 0.01);
	char Buff[128];
	//Update actual liftoff time
	sprintf_s(Buff, "P10,CSM,%d:%d:%.2lf;", hh, mm, ss);
	GC->rtcc->GMGMED(Buff);
	//Update GMT of zeroing CMC clock
	sprintf_s(Buff, "P15,AGC,%d:%d:%.2lf;", hh, mm, ss);
	GC->rtcc->GMGMED(Buff);
	//Update GMT of zeroing LGC clock
	sprintf_s(Buff, "P15,LGC,%d:%d:%.2lf;", hh, mm, ss);
	GC->rtcc->GMGMED(Buff);

	//Also update IU, if possible
	G->UpdateGRRTime(GC->rtcc->pCSM);

	return true;
}

void ApolloRTCCMFD::cycleREFSMMATHeadsUp()
{
	if (G->REFSMMATopt == 0)
	{
		G->REFSMMATHeadsUp = !G->REFSMMATHeadsUp;
	}
}

void ApolloRTCCMFD::TwoImpulseOffset()
{
	menuGeneralMEDRequest("Format: P51, Delta Height, Phase Angle, Elevation Angle, Travel Angle; (leave open for no update)");
}

void ApolloRTCCMFD::cycleVECDirOpt()
{
	if (G->VECdirection < 4)
	{
		G->VECdirection++;
	}
	else
	{
		G->VECdirection = 0;
	}
}

void ApolloRTCCMFD::menuVECPOINTSelectAttitude()
{
	if (G->VECdirection == 4)
	{
		GenericDouble2Input(&G->VECBodyVector.x, &G->VECBodyVector.y, "Select body yaw and pitch:", RAD, RAD);
	}
}

void ApolloRTCCMFD::menuVECPOINTOmicron()
{
	GenericDoubleInput(&G->VECBodyVector.z, "Select omicron (SEF = 180, BEF = 0):", RAD);
}

void ApolloRTCCMFD::cycleVECPOINTOpt()
{
	if (G->VECoption < 1)
	{
		G->VECoption++;
	}
	else
	{
		G->VECoption = 0;
	}
}

void ApolloRTCCMFD::vecbodydialogue()
{
	bool VECbodyInput(void* id, char *str, void *data);
	oapiOpenInputBox("Pointing Body:", VECbodyInput, 0, 20, (void*)this);
}

bool VECbodyInput(void *id, char *str, void *data)
{
	if (oapiGetObjectByName(str) != NULL)
	{
		((ApolloRTCCMFD*)data)->set_vecbody(oapiGetObjectByName(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_vecbody(OBJHANDLE body)
{
	G->VECbody = body;
}

void ApolloRTCCMFD::menuVECPOINTCalc()
{
	G->VecPointCalc(IsCSM);
}

void ApolloRTCCMFD::RecallStatus(void)
{
	//MFD data got reloaded in LoadState from the constructor, but resetting the MFD buttons crashes there. Do it here instead

	if (screen == 130)
	{
		//Special logic for MED input page
		SetMEDInputPage(MEDInputData.MEDCode);
	}
	else
	{
		SelectPage(screen);
	}
}

void ApolloRTCCMFD::GetREFSMMATfromAGC()
{
	agc_t *vagc = G->GetAGCPointer(IsCSM);
	if (vagc == NULL) return;

	MATRIX3 REFSMMAT = GC->rtcc->GetREFSMMATfromAGC(vagc, IsCSM);

	if (IsCSM)
	{
		GC->rtcc->BZSTLM.CMC_REFSMMAT = REFSMMAT;
		GC->rtcc->BZSTLM.CMCRefsPresent = true;
		GC->rtcc->EMSGSUPP(1, 1);
		GeneralMEDRequest("G00,CSM,TLM,CSM,CUR;");
	}
	else
	{
		GC->rtcc->BZSTLM.LGC_REFSMMAT = REFSMMAT;
		GC->rtcc->BZSTLM.LGCRefsPresent = true;
		GC->rtcc->EMSLSUPP(1, 1);
		GeneralMEDRequest("G00,LEM,TLM,LEM,CUR;");
	}

	G->REFSMMATcur = G->REFSMMATopt;

	//sprintf(oapiDebugString(), "%f, %f, %f, %f, %f, %f, %f, %f, %f", G->REFSMMAT.m11, G->REFSMMAT.m12, G->REFSMMAT.m13, G->REFSMMAT.m21, G->REFSMMAT.m22, G->REFSMMAT.m23, G->REFSMMAT.m31, G->REFSMMAT.m32, G->REFSMMAT.m33);
}

void ApolloRTCCMFD::menuCycleLunarEntryPADSxtOption()
{
	if (GC->entrypadopt == 1)
	{
		GC->EntryPADSxtStarCheckAttOpt = !GC->EntryPADSxtStarCheckAttOpt;
	}
}

void ApolloRTCCMFD::GetEntryTargetfromAGC()
{
	VESSEL *v = GC->rtcc->pCSM;

	if (v == NULL) return;

	if (utils::IsVessel(v, utils::Saturn) == false) return;

	unsigned short Entryoct[6];
	Entryoct[2] = saturn->agc.vagc.Erasable[0][03400];
	Entryoct[3] = saturn->agc.vagc.Erasable[0][03401];
	Entryoct[4] = saturn->agc.vagc.Erasable[0][03402];
	Entryoct[5] = saturn->agc.vagc.Erasable[0][03403];

	GC->rtcc->RZDBSC1.lat_T = OrbMech::DecToDouble(Entryoct[2], Entryoct[3])*PI2;
	GC->rtcc->RZDBSC1.lng_T = OrbMech::DecToDouble(Entryoct[4], Entryoct[5])*PI2;

}

void ApolloRTCCMFD::menuSetRTEReentryTime()
{
	bool RTEReentryTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the estimated reentry GET (Format: hhh:mm:ss)", RTEReentryTimeInput, 0, 20, (void*)this);
}

bool RTEReentryTimeInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_RTEReentryTime(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_RTEReentryTime(double t)
{
	G->RTEReentryTime = t;
}

void ApolloRTCCMFD::menuEnterSplashdownLat()
{
	GenericDoubleInput(&GC->rtcc->RZDBSC1.lat_T, "Choose the splashdown latitude:", RAD);
}

void ApolloRTCCMFD::menuEnterSplashdownLng()
{
	GenericDoubleInput(&GC->rtcc->RZDBSC1.lng_T, "Choose the splashdown longitude:", RAD);
}

void ApolloRTCCMFD::menuTransferLOIMCCtoMPT()
{
	G->TransferLOIorMCCtoMPT();
}

void ApolloRTCCMFD::menuTLCCVectorTime()
{
	if (GC->MissionPlanningActive)
	{
		GenericGETInput(&GC->rtcc->PZMCCPLN.VectorGET, "Choose the vector time for the maneuver (Format: hhh:mm:ss)");
	}
	else
	{
		set_CSMVessel();
	}
}

void ApolloRTCCMFD::menuCycleTLCCColumnNumber()
{
	if (GC->rtcc->PZMCCPLN.Column < 4)
	{
		GC->rtcc->PZMCCPLN.Column++;
	}
	else
	{
		GC->rtcc->PZMCCPLN.Column = 1;
	}
}

void ApolloRTCCMFD::menuCycleTLCCCSFPBlockNumber()
{
	if (GC->rtcc->PZMCCPLN.SFPBlockNum < 2)
	{
		GC->rtcc->PZMCCPLN.SFPBlockNum++;
	}
	else
	{
		GC->rtcc->PZMCCPLN.SFPBlockNum = 1;
	}
}

void ApolloRTCCMFD::menuCycleTLCCConfiguration()
{
	GC->rtcc->PZMCCPLN.Config = !GC->rtcc->PZMCCPLN.Config;
}

void ApolloRTCCMFD::menuSwitchTLCCManeuver()
{
	if (GC->rtcc->PZMCCPLN.Mode < 9)
	{
		GC->rtcc->PZMCCPLN.Mode++;
	}
	else
	{
		GC->rtcc->PZMCCPLN.Mode = 1;
	}
}

void ApolloRTCCMFD::menuSetTLCCGET()
{
	GenericGETInput(&GC->rtcc->PZMCCPLN.MidcourseGET, "Choose the GET for the maneuver (Format: hhh:mm:ss)");
}

void ApolloRTCCMFD::menuSetTLAND()
{
	bool TLandGETnput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the Time of Landing (Format: hhh:mm:ss)", TLandGETnput, 0, 20, (void*)this);
}

bool TLandGETnput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_TLand(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLand(double time)
{
	GC->rtcc->CZTDTGTU.GETTD = time;
}

void ApolloRTCCMFD::menuSetTLCCDesiredInclination()
{
	if (GC->rtcc->PZMCCPLN.Mode >= 8)
	{
		GenericDoubleInput(&GC->rtcc->PZMCCPLN.incl_fr, "Choose the desired return inclination (+ for ascending, - for descending, 0 for optimized mode 9):", RAD);
	}
}

void ApolloRTCCMFD::menuSetTLMCCAzimuthConstraints()
{
	char Buff[128];
	sprintf_s(Buff, "%.2lf,%.2lf", GC->rtcc->PZMCCPLN.AZ_min*DEG, GC->rtcc->PZMCCPLN.AZ_max*DEG);

	bool TLMCCAzimuthConstraintsInput(void *id, char *str, void *data);
	oapiOpenInputBox("Input Format: Minimum Azimuth,Maximum Azimuth (must be between -110 and -70°)", TLMCCAzimuthConstraintsInput, Buff, 20, (void*)this);
}

bool TLMCCAzimuthConstraintsInput(void *id, char *str, void *data)
{
	char Buff[128];
	sprintf_s(Buff, "F22,%s;", str);
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buff);
	return true;
}

void ApolloRTCCMFD::menuSetTLMCCTLCTimesConstraints()
{
	char Buff[128];
	int hh1, hh2, mm1, mm2;
	double ss1, ss2;

	OrbMech::SStoHHMMSS(GC->rtcc->PZMCCPLN.TLMIN*3600.0, hh1, mm1, ss1);
	OrbMech::SStoHHMMSS(GC->rtcc->PZMCCPLN.TLMAX*3600.0, hh2, mm2, ss2);

	sprintf_s(Buff, "%d:%d:%.0lf,%d:%d:%.0lf", hh1, mm1, ss1, hh2, mm2, ss2);

	bool TLMCCTLCTimesConstraintsInput(void *id, char *str, void *data);
	oapiOpenInputBox("Input Format: TLMIN,TLMAX (GET in HH:MM:SS, for no constraint)", TLMCCTLCTimesConstraintsInput, Buff, 40, (void*)this);
}

bool TLMCCTLCTimesConstraintsInput(void *id, char *str, void *data)
{
	char Buff[128];
	sprintf_s(Buff, "F23,%s;", str);
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buff);
	return true;
}

void ApolloRTCCMFD::menuSetTLMCCReentryContraints()
{
	bool TLMCCReentryContraintsInput(void *id, char *str, void *data);
	oapiOpenInputBox("Input Format: Flight Path Angle,Reentry Range", TLMCCReentryContraintsInput, "-6.52,1285", 20, (void*)this);
}

bool TLMCCReentryContraintsInput(void *id, char *str, void *data)
{
	char Buff[128];
	sprintf_s(Buff, "F24,%s;", str);
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buff);
	return true;
}

void ApolloRTCCMFD::menuSetTLMCCPericynthionHeightLimits()
{
	bool TLMCCPericynthionHeightLimitsInput(void *id, char *str, void *data);
	oapiOpenInputBox("Input Format: height minimum,height maximum", TLMCCPericynthionHeightLimitsInput, "40,5000", 40, (void*)this);
}

bool TLMCCPericynthionHeightLimitsInput(void *id, char *str, void *data)
{
	char Buff[128];
	sprintf_s(Buff, "F29,%s;", str);
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buff);
	return true;
}

void ApolloRTCCMFD::menuSetTLMCCLatitudeBias()
{
	bool TLMCCLatitudeBiasInput(void *id, char *str, void *data);
	oapiOpenInputBox("Latitude bias to find optimum mode 9 maneuver:", TLMCCLatitudeBiasInput, 0, 20, (void*)this);
}

bool TLMCCLatitudeBiasInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_TLMCCLatitudeBias(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLMCCLatitudeBias(double bias)
{
	GC->rtcc->PZMCCPLN.LATBIAS = bias * RAD;
}

void ApolloRTCCMFD::menuSetTLMCCMaxInclination()
{
	bool TLMCCMaxInclinationInput(void *id, char *str, void *data);
	oapiOpenInputBox("Maximum powered return inclination:", TLMCCMaxInclinationInput, 0, 20, (void*)this);
}

bool TLMCCMaxInclinationInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_TLMCCMaxInclination(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLMCCMaxInclination(double inc)
{
	GC->rtcc->PZMCCPLN.INCL_PR_MAX = inc * RAD;
}

void ApolloRTCCMFD::menuSetTLMCCLOIEllipseHeights()
{
	bool TLMCCLOIEllipseHeightsInput(void *id, char *str, void *data);
	oapiOpenInputBox("Apolune and perilune heights of the LOI ellipse (Format: HA HP)", TLMCCLOIEllipseHeightsInput, 0, 20, (void*)this);
}

bool TLMCCLOIEllipseHeightsInput(void *id, char *str, void *data)
{
	double ha, hp;

	if (sscanf(str, "%lf %lf", &ha, &hp) == 2)
	{
		((ApolloRTCCMFD*)data)->set_TLMCCLOIEllipseHeights(ha, hp);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLMCCLOIEllipseHeights(double ha, double hp)
{
	GC->rtcc->PZMCCPLN.H_A_LPO1 = ha * 1852.0;
	GC->rtcc->PZMCCPLN.H_P_LPO1 = hp * 1852.0;
}

void ApolloRTCCMFD::menuSetTLMCCDOIEllipseHeights()
{
	bool TLMCCDOIEllipseHeightsInput(void *id, char *str, void *data);
	oapiOpenInputBox("Apolune and perilune heights of the DOI ellipse (Format: HA HP)", TLMCCDOIEllipseHeightsInput, 0, 20, (void*)this);
}

bool TLMCCDOIEllipseHeightsInput(void *id, char *str, void *data)
{
	double ha, hp;

	if (sscanf(str, "%lf %lf", &ha, &hp) == 2)
	{
		((ApolloRTCCMFD*)data)->set_TLMCCDOIEllipseHeights(ha, hp);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLMCCDOIEllipseHeights(double ha, double hp)
{
	GC->rtcc->PZMCCPLN.H_A_LPO2 = ha * 1852.0;
	GC->rtcc->PZMCCPLN.H_P_LPO2 = hp * 1852.0;
}

void ApolloRTCCMFD::menuSetTLMCCLOIDOIRevs()
{
	bool TLMCCLOIDOIRevsInput(void *id, char *str, void *data);
	oapiOpenInputBox("Revolutions in LOI and DOI ellipses (Format: REVS1 REVS2)", TLMCCLOIDOIRevsInput, 0, 20, (void*)this);
}

bool TLMCCLOIDOIRevsInput(void *id, char *str, void *data)
{
	double revs1;
	int revs2;

	if (sscanf(str, "%lf %d", &revs1, &revs2) == 2)
	{
		((ApolloRTCCMFD*)data)->set_TLMCCLOIDOIRevs(revs1, revs2);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLMCCLOIDOIRevs(double revs1, int revs2)
{
	GC->rtcc->PZMCCPLN.REVS1 = revs1;
	GC->rtcc->PZMCCPLN.REVS2 = revs2;

	//Calculate new ETA1
	double R1;
	double DR1 = modf(GC->rtcc->PZMCCPLN.REVS1, &R1)*PI2;

	GC->rtcc->PZMCCPLN.ETA1 = 0.0 - DR1;
	if (GC->rtcc->PZMCCPLN.ETA1 < 0)
	{
		GC->rtcc->PZMCCPLN.ETA1 += PI2;
	}
}

void ApolloRTCCMFD::menuSetTLMCCLSRotation()
{
	bool TLMCCLSRotationInput(void *id, char *str, void *data);
	oapiOpenInputBox("Rotation of orbit at LS and estimate of true anomaly of LOI on ellipse (Format: SITEROT ETA)", TLMCCLSRotationInput, 0, 20, (void*)this);
}

bool TLMCCLSRotationInput(void *id, char *str, void *data)
{
	double rot, eta;

	if (sscanf(str, "%lf %lf", &rot, &eta) == 2)
	{
		((ApolloRTCCMFD*)data)->set_TLMCCLSRotation(rot, eta);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLMCCLSRotation(double rot, double eta)
{
	GC->rtcc->PZMCCPLN.SITEROT = rot * RAD;
	GC->rtcc->PZMCCPLN.ETA1 = eta * RAD;
}

void ApolloRTCCMFD::menuSetTLMCCLOPCRevs()
{
	bool TLMCCLOPCRevsInput(void *id, char *str, void *data);
	oapiOpenInputBox("Revolutions before and after LOPC (Format: m n)", TLMCCLOPCRevsInput, 0, 20, (void*)this);
}

bool TLMCCLOPCRevsInput(void *id, char *str, void *data)
{
	int m, n;

	if (sscanf(str, "%d %d", &m, &n) == 2)
	{
		((ApolloRTCCMFD*)data)->set_TLMCCLOPCRevs(m, n);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLMCCLOPCRevs(int m, int n)
{
	GC->rtcc->PZMCCPLN.LOPC_M = m;
	GC->rtcc->PZMCCPLN.LOPC_N = n;
}

void ApolloRTCCMFD::menuSetLOIVectorTime()
{
	if (GC->MissionPlanningActive)
	{
		GenericGETInput(&GC->rtcc->med_k18.VectorTime, "Choose the vector time for LOI computation:");
	}
	else
	{
		set_CSMVessel();
	}
}

void ApolloRTCCMFD::menuSetLOIApo()
{
	GenericDoubleInput(&GC->rtcc->med_k18.HALOI1, "Choose the apocynthion altitude:", 1.0);
}

void ApolloRTCCMFD::menuSetLOIPeri()
{
	GenericDoubleInput(&GC->rtcc->med_k18.HPLOI1, "Choose the perilune altitude:", 1.0);
}

void ApolloRTCCMFD::menuSetLOIMaxDVPos()
{
	GenericDoubleInput(&GC->rtcc->med_k18.DVMAXp, "Choose max DV for positive solution:", 1.0);
}

void ApolloRTCCMFD::menuSetLOIMaxDVNeg()
{
	GenericDoubleInput(&GC->rtcc->med_k18.DVMAXm, "Choose max DV for negative solution:", 1.0);
}

void ApolloRTCCMFD::menuSetLOI_HALLS()
{
	GenericDoubleInput(&GC->rtcc->PZLOIPLN.HA_LLS, "Choose the apolune altitude at the landing site:", 1.0);
}

void ApolloRTCCMFD::menuSetLOI_HPLLS()
{
	GenericDoubleInput(&GC->rtcc->PZLOIPLN.HP_LLS, "Choose the perilune altitude at the landing site:", 1.0);
}

void ApolloRTCCMFD::menuSetLOIDW()
{
	GenericDoubleInput(&GC->rtcc->PZLOIPLN.DW, "Angle of perilune from the landing site (negative if site is post-perilune):", 1.0);
}

void ApolloRTCCMFD::menuSetLOIDHBias()
{
	GenericDoubleInput(&GC->rtcc->PZLOIPLN.dh_bias, "Altitude bias of intersection solutions:", 1.0);
}

void ApolloRTCCMFD::menuSetLOIRevs1()
{
	GenericDoubleInput(&GC->rtcc->PZLOIPLN.REVS1, "Number of revolutions in first lunar orbit (may have fractional part):", 1.0);
}

void ApolloRTCCMFD::menuSetLOIRevs2()
{
	GenericIntInput(&GC->rtcc->PZLOIPLN.REVS2, "Number of revolutions in second lunar orbit:");
}

void ApolloRTCCMFD::menuCycleLOIInterSolnFlag()
{
	GC->rtcc->PZLOIPLN.PlaneSolnForInterSoln = !GC->rtcc->PZLOIPLN.PlaneSolnForInterSoln;
}

void ApolloRTCCMFD::menuSetLOIEta1()
{
	GenericDoubleInput(&GC->rtcc->PZLOIPLN.eta_1, "True anomaly on LPO-1 for transferring from hyperbola to LPO-1:", 1.0);
}

void ApolloRTCCMFD::menuSetTLCCAlt()
{
	GenericDoubleInput(&GC->rtcc->PZMCCPLN.h_PC, "Choose the perilune altitude (Either 0 or 50+ NM):", 1852.0);
}

void ApolloRTCCMFD::menuSetTLCCAltMode5()
{
	GenericDoubleInput(&GC->rtcc->PZMCCPLN.h_PC_mode5, "Choose the pericynthion height for mode 5 (negative number to use data table value):", 1852.0);
}

void ApolloRTCCMFD::menuSetLOIDesiredAzi()
{
	GenericDoubleInput(&GC->rtcc->med_k18.psi_DS, "Choose the desired approach azimuth:", 1.0);
}

void ApolloRTCCMFD::menuSetLOIMinAzi()
{
	GenericDoubleInput(&GC->rtcc->med_k18.psi_MN, "Choose the minimum approach azimuth:", 1.0);
}

void ApolloRTCCMFD::menuSetLOIMaxAzi()
{
	GenericDoubleInput(&GC->rtcc->med_k18.psi_MX, "Choose the maximum approach azimuth:", 1.0);
}

void ApolloRTCCMFD::menuLOICalc()
{
	G->LOICalc();
}

void ApolloRTCCMFD::menuLmkPADCalc()
{
	G->LmkCalc();
}

void ApolloRTCCMFD::menuSetLmkTime()
{
	GenericGETInput(&GC->LmkTime, "Choose the guess for T1:");
}

void ApolloRTCCMFD::menuSetLmkElevation()
{
	GenericDoubleInput(&GC->LmkElevation, "Choose the T2 elevation in degrees (0-90°):", RAD);
}

void ApolloRTCCMFD::menuSetLmkLat()
{
	GenericDoubleInput(&GC->LmkLat, "Choose the landmark latitude:", RAD);
}

void ApolloRTCCMFD::menuSetLmkLng()
{
	GenericDoubleInput(&GC->LmkLng, "Choose the landmark longitude:", RAD);
}

void ApolloRTCCMFD::menuLmkUseLandingSite()
{
	//Load RTCC stored landing site coordinates into input for P22 PAD
	GC->LmkLat = GC->rtcc->BZLAND.lat[0];
	GC->LmkLng = GC->rtcc->BZLAND.lng[0];
}

void ApolloRTCCMFD::menuSetLDPPVectorTime()
{
	if (GC->MissionPlanningActive)
	{
		GenericGETInput(&GC->rtcc->med_k16.VectorTime, "Choose the vector time (Format: hhh:mm:ss)");
	}
	else
	{
		if (GC->rtcc->med_k16.Vehicle == RTCC_MPT_LM)
		{
			set_LMVessel();
		}
		else
		{
			set_CSMVessel();
		}
	}
}

void ApolloRTCCMFD::menuLSRadius()
{
	GenericDoubleInput(&GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST], "Choose the landing site radius:", 1852.0);
}

void ApolloRTCCMFD::menuSetLDPPDwellOrbits()
{
	GenericIntInput(&GC->rtcc->GZGENCSN.LDPPDwellOrbits, "Choose the number of revolutions:");
}

void ApolloRTCCMFD::menuSetLDPPLandingSiteOffset()
{
	GenericDoubleInput(&GC->rtcc->GZGENCSN.LDPPLandingSiteOffset, "Choose the angle from perilune to landing site:", RAD);
}

void ApolloRTCCMFD::menuSetLDPPDescentFlightArc()
{
	GenericDoubleInput(&GC->rtcc->GZGENCSN.LDPPDescentFlightArc, "Choose the powered descent flight arc:", RAD);
}

void ApolloRTCCMFD::menuSetLDPPDescIgnHeight()
{
	GenericDoubleInput(&GC->rtcc->GZGENCSN.LDPPHeightofPDI, "Choose the perilune altitude above the landing site:", 0.3048);
}

void ApolloRTCCMFD::cycleLDPPPoweredDescSimFlag()
{
	GC->rtcc->GZGENCSN.LDPPPoweredDescentSimFlag = !GC->rtcc->GZGENCSN.LDPPPoweredDescentSimFlag;
}

void ApolloRTCCMFD::menuSetLDPPMode()
{
	if (GC->rtcc->med_k16.Mode < 7)
	{
		GC->rtcc->med_k16.Mode++;
	}
	else
	{
		GC->rtcc->med_k16.Mode = 1;
	}
	GC->rtcc->med_k16.Sequence = 1;
}

void ApolloRTCCMFD::menuSetLDPPSequence()
{
	if (GC->rtcc->med_k16.Sequence < 5)
	{
		GC->rtcc->med_k16.Sequence++;
	}
	else
	{
		GC->rtcc->med_k16.Sequence = 1;
	}
}

void ApolloRTCCMFD::menuLDPPSaveTLAND()
{
	if (GC->rtcc->PZLDPDIS.PD_GETTD != 0.0)
	{
		GC->rtcc->CZTDTGTU.GETTD = GC->rtcc->PZLDPDIS.PD_GETTD;
	}
}

void ApolloRTCCMFD::menuLDPPCalc()
{
	G->LDPPalc();
	menuSetDescPlanTablePage();
}

void ApolloRTCCMFD::set_t_TPI(double time)
{
	G->t_TPI = time;
}

void ApolloRTCCMFD::menuSetTPIguess()
{
	GenericGETInput(&G->t_TPIguess, "Choose the GET for TPI (Format: hhh:mm:ss)");
}

void ApolloRTCCMFD::menuCycleLLWPChaserOption()
{
	if (GC->rtcc->med_k15.Chaser == 1)
	{
		GC->rtcc->med_k15.Chaser = 3;
	}
	else
	{
		GC->rtcc->med_k15.Chaser = 1;
	}
}

void ApolloRTCCMFD::menuSetLiftoffguess()
{
	GenericGETInput(&GC->rtcc->med_k15.ThresholdTime, "Choose the threshold time (Format: hhh:mm:ss)");
}

void ApolloRTCCMFD::menuLLWPVectorTime()
{
	if (GC->MissionPlanningActive)
	{
		GenericGETInput(&GC->rtcc->med_k15.CSMVectorTime, "Choose the CSM vector time (Format: hhh:mm:ss)");
	}
	else
	{
		set_CSMVessel();
	}
}

void ApolloRTCCMFD::menuSetLLWPCSIFlag()
{
	GenericDoubleInput(&GC->rtcc->med_k15.CSI_Flag, "CSI Flag: 0: CSI done 90° from insertion, negative value: CSI done at LM apolune, positive value: CSI done at X mins after insertion", 60.0);
}

void ApolloRTCCMFD::menuSetLLWPCDHFlag()
{

}

void ApolloRTCCMFD::menuSetLLWPDeltaHeights()
{
	bool LLWPDeltaHeightsInput(void* id, char *str, void *data);
	oapiOpenInputBox("Input the three DHs to be calculated:", LLWPDeltaHeightsInput, 0, 20, (void*)this);
}

bool LLWPDeltaHeightsInput(void *id, char *str, void *data)
{
	double dh1, dh2, dh3;

	if (sscanf(str, "%lf %lf %lf", &dh1, &dh2, &dh3) == 3)
	{
		((ApolloRTCCMFD*)data)->set_LLWPDeltaHeights(dh1, dh2, dh3);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LLWPDeltaHeights(double dh1, double dh2, double dh3)
{
	GC->rtcc->med_k15.DH1 = dh1 * 1852.0;
	GC->rtcc->med_k15.DH2 = dh2 * 1852.0;
	GC->rtcc->med_k15.DH3 = dh3 * 1852.0;
}

void ApolloRTCCMFD::menuSetLLWPElevation()
{
	bool LLWPElevInput(void* id, char *str, void *data);
	oapiOpenInputBox("Elevation in degrees:", LLWPElevInput, 0, 20, (void*)this);
}

bool LLWPElevInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_LLWPElevation(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LLWPElevation(double elev)
{
	this->GC->rtcc->PZLTRT.ElevationAngle = elev * RAD;
}

void ApolloRTCCMFD::menuTMLat()
{
	GenericDoubleInput(&G->TMLat, "Latitude in degrees:", RAD);
}

void ApolloRTCCMFD::menuTMLng()
{
	GenericDoubleInput(&G->TMLng, "Longitude in degrees:", RAD);
}

void ApolloRTCCMFD::menuTMAzi()
{
	GenericDoubleInput(&G->TMAzi, "Approach azimuth in degrees:", RAD);
}

void ApolloRTCCMFD::menuTMDistance()
{
	GenericDoubleInput(&G->TMDistance, "Distance in feet:", 0.3048);
}

void ApolloRTCCMFD::menuTMStepSize()
{
	GenericDoubleInput(&G->TMStepSize, "Step size in feet:", 0.3048);
}

void ApolloRTCCMFD::menuTerrainModelCalc()
{
	G->TerrainModelCalc();
}

void ApolloRTCCMFD::menuTLCCCalc()
{
	G->TLCCSolGood = true;
	G->TLCCCalc();
}

void ApolloRTCCMFD::menuTLIProcessorCalc()
{
	G->TLCCSolGood = true;
	G->TLIProcessorCalc();
}

void ApolloRTCCMFD::menuSetTLIProcessorInput()
{
	switch (marker)
	{
	case 0:
		set_IUVessel();
		break;
	case 1:
		GC->rtcc->PZTLIPLN.mpt = 4 - GC->rtcc->PZTLIPLN.mpt;
		break;
	case 2:
		GenericStringInput(&GC->rtcc->PZTLIPLN.VectorType, "Vector type (CMC, LGC, AGS, IU, HSR, DC, ANC):");
		break;
	case 3:
		GC->rtcc->PZTLIPLN.Opportunity = 3 - GC->rtcc->PZTLIPLN.Opportunity;
		break;
	case 4:
		if (GC->rtcc->PZTLIPLN.Mode < 5)
		{
			GC->rtcc->PZTLIPLN.Mode++;
		}
		else
		{
			GC->rtcc->PZTLIPLN.Mode = 1;
		}
		break;
	case 5:
		GenericGETInput(&GC->rtcc->PZTLIPLN.GET_TLI, "Input time of ignition or threshold time. Format HH:MM:SS:");
		break;
	case 6:
		if (GC->rtcc->PZTLIPLN.Mode == 3)
		{
			GenericDoubleInput(&GC->rtcc->PZTLIPLN.dv_available, "Available Delta V for TLI (0 to 10000 ft/s):");
		}
		else if (GC->rtcc->PZTLIPLN.Mode == 4)
		{
			GenericDoubleInput(&GC->rtcc->PZTLIPLN.h_ap, "Input height of apogee (2700 to 7000 NM):");
		}
		else
		{
			GC->rtcc->PZTLIPLN.IsPacficWindow = !GC->rtcc->PZTLIPLN.IsPacficWindow;
		}
		break;
	case 7:
		menuCycleTLCCCSFPBlockNumber();
		break;
	}
}

void ApolloRTCCMFD::menuLunarLiftoffCalc()
{
	G->LunarLiftoffCalc();
}

void ApolloRTCCMFD::menuLLTPCalc()
{
	G->LunarLaunchTargetingCalc();
}

void ApolloRTCCMFD::menuSetLiftoffDT()
{
	bool LiftoffDTInput(void* id, char *str, void *data);
	oapiOpenInputBox("DT between insertion and TPI:", LiftoffDTInput, 0, 20, (void*)this);
}

bool LiftoffDTInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LiftoffDT(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LiftoffDT(double dt)
{
	GC->rtcc->PZLTRT.DT_Ins_TPI = dt * 60.0;
}

void ApolloRTCCMFD::menuLLTPThresholdTime()
{
	bool LLTPThresholdTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the threshold time for liftoff (Format: hhh:mm:ss)", LLTPThresholdTimeInput, 0, 20, (void*)this);
}

bool LLTPThresholdTimeInput(void *id, char *str, void *data)
{
	int hh, mm, ss, get;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		get = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_LLTPThresholdTime(get);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LLTPThresholdTime(double get)
{
	GC->rtcc->med_k50.GETTH = get;
}

void ApolloRTCCMFD::menuLLTPVectorTime()
{
	if (GC->MissionPlanningActive)
	{
		bool LLTPVectorTimeInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the vector time for liftoff (Format: hhh:mm:ss)", LLTPVectorTimeInput, 0, 20, (void*)this);
	}
}

bool LLTPVectorTimeInput(void *id, char *str, void *data)
{
	int hh, mm, ss, get;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		get = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_LLTPVectorTime(get);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LLTPVectorTime(double get)
{
	GC->rtcc->med_k50.GETV = get;
}

void ApolloRTCCMFD::menuLunarLiftoffVHorInput()
{
	GenericDoubleInput(&GC->rtcc->PZLTRT.InsertionHorizontalVelocity, "Input horizontal velocity in ft/s (LGC default is 5509.5):", 0.3048);
}

void ApolloRTCCMFD::menuLunarLiftoffVVertInput()
{
	GenericDoubleInput(&GC->rtcc->PZLTRT.InsertionRadialVelocity, "Input vertical velocity in ft/s (LGC default is 19.5):", 0.3048);
}

void ApolloRTCCMFD::menuLunarLiftoffSaveInsertionSV()
{
	GC->rtcc->CMMCMNAV(1, 3, GC->rtcc->JZLAI.sv_Insertion);
	SelectUplinkScreen(9); //Go to CMC LM state vector page
}

void ApolloRTCCMFD::menuSetEMPFileName()
{
	GenericStringInput(&G->EMPFile, "Enter EMP file name:", &ApolloRTCCMFD::ErasableMemoryFileRead);
}

void ApolloRTCCMFD::ErasableMemoryFileRead()
{
	G->ErasableMemoryFileRead();
}

void ApolloRTCCMFD::menuSetEMPUplinkNumber()
{
	if (G->EMPUplinkMaxNumber > 0)
	{
		GenericIntInput(&G->EMPUplinkNumber, "Enter load number:", NULL, 1, G->EMPUplinkMaxNumber);
	}
}

void ApolloRTCCMFD::menuInitializeEMP()
{
	bool InitializeEMPInputBox(void *id, char *str, void *data);
	oapiOpenInputBox("Enter starting address (V71) or leave blank (V72):", InitializeEMPInputBox, 0, 25, (void*)this);
}

bool InitializeEMPInputBox(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->set_EMPInit(str);
	return true;
}

void ApolloRTCCMFD::set_EMPInit(char *str)
{
	char Buffer[128];
	int load;

	if (subscreen == 0)
	{
		load = 18;
	}
	else if (subscreen == 1)
	{
		load = 19;
	}
	else if (subscreen == 2)
	{
		load = 38;
	}
	else
	{
		load = 39;
	}

	sprintf(Buffer, "CEI,%d,HSK,HSK,%s;", load, str);
	GeneralMEDRequest(Buffer);
}

void ApolloRTCCMFD::menuEditEMPOctal()
{
	bool EditEMPOctalInputBox(void *id, char *str, void *data);
	oapiOpenInputBox("Enter line and octal value:", EditEMPOctalInputBox, 0, 25, (void*)this);
}


bool EditEMPOctalInputBox(void *id, char *str, void *data)
{
	int line, value;

	if (sscanf(str, "%o %o", &line, &value) == 2)
	{
		((ApolloRTCCMFD*)data)->set_EMPOctal(line, value);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_EMPOctal(int line, int value)
{
	char Buffer[128];
	int load;

	if (subscreen == 0)
	{
		load = 18;
	}
	else if (subscreen == 1)
	{
		load = 19;
	}
	else if (subscreen == 2)
	{
		load = 38;
	}
	else
	{
		load = 39;
	}

	sprintf(Buffer, "CEO,%d,%o,%o;", load, line, value);
	GeneralMEDRequest(Buffer);
}

void ApolloRTCCMFD::menuDeleteEMPLine()
{
	bool DeleteEMPOctalInputBox(void *id, char *str, void *data);
	oapiOpenInputBox("Select line to delete from uplink:", DeleteEMPOctalInputBox, 0, 25, (void*)this);
}

bool DeleteEMPOctalInputBox(void *id, char *str, void *data)
{
	int line;

	if (sscanf(str, "%o", &line) == 1)
	{
		((ApolloRTCCMFD*)data)->set_EMPDelete(line);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_EMPDelete(int line)
{
	char Buffer[128];
	int load;

	if (subscreen == 0)
	{
		load = 18;
	}
	else if (subscreen == 1)
	{
		load = 19;
	}
	else if (subscreen == 2)
	{
		load = 38;
	}
	else
	{
		load = 39;
	}

	sprintf(Buffer, "CEC,%d,HSK,HSK,,,%o;", load, line);
	GeneralMEDRequest(Buffer);
}

void ApolloRTCCMFD::menuLoadEMP()
{
	G->ErasableMemoryFileLoad(subscreen);
}

void ApolloRTCCMFD::menuUplinkEMP()
{
	G->ErasableMemoryUpdateUplink(subscreen);
}

void ApolloRTCCMFD::menuNavCheckPADCalc()
{
	G->NavCheckPAD(IsCSM);
}

void ApolloRTCCMFD::menuSetNavCheckGET()
{
	GenericGETInput(&G->navcheckpad.NavChk[0], "Choose the GET for the Nav Check (Format: hhh:mm:ss)");
}

void ApolloRTCCMFD::menuDKINC1Line()
{
	GenericDoubleInput(&GC->rtcc->med_k00.NC1, "Enter NC1 maneuver line point:");
}

void ApolloRTCCMFD::menuDKINHLine()
{
	GenericDoubleInput(&GC->rtcc->med_k00.NH, "Enter NH maneuver line point:");
}

void  ApolloRTCCMFD::menuCycleDKIChaserVehicle()
{
	if (GC->rtcc->med_k00.ChaserVehicle == RTCC_MPT_CSM)
	{
		GC->rtcc->med_k00.ChaserVehicle = RTCC_MPT_LM;
	}
	else
	{
		GC->rtcc->med_k00.ChaserVehicle = RTCC_MPT_CSM;
	}
}

void ApolloRTCCMFD::menuDKICalc()
{
	G->startSubthread(19);
}

void ApolloRTCCMFD::menuLAPCalc()
{
	G->LAPCalc();
}

void ApolloRTCCMFD::menuSetLAPLiftoffTime()
{
	GenericGETInput(&G->t_LunarLiftoff, "Choose the liftoff time (Format: hhh:mm:ss)");
}

void ApolloRTCCMFD::menuDKINSRLine()
{
	if (GC->rtcc->med_k00.I4)
	{
		GenericDoubleInput(&GC->rtcc->med_k00.NCC, "Enter NCC maneuver line point:");
	}
	else
	{
		GenericDoubleInput(&GC->rtcc->med_k00.NSR, "Enter NSR maneuver line point:");
	}
}

void ApolloRTCCMFD::menuDKIMILine()
{
	GenericDoubleInput(&GC->rtcc->med_k00.MI, "Enter approximate TPI maneuver line point:");
}

void ApolloRTCCMFD::menuDKINPCLine()
{
	GenericDoubleInput(&GC->rtcc->med_k00.NPC, "Enter NPC maneuver line point:");
}

void ApolloRTCCMFD::menuCycleDKIManeuverLineDefinition()
{
	if (GC->rtcc->med_k10.MLDOption < 3)
	{
		GC->rtcc->med_k10.MLDOption++;
	}
	else
	{
		GC->rtcc->med_k10.MLDOption = 1;
	}
}

void ApolloRTCCMFD::menuCycleDKIProfile()
{
	GC->rtcc->med_k00.I4 = !GC->rtcc->med_k00.I4;
}

void ApolloRTCCMFD::menuDKITIG()
{
	GenericGETInput(&GC->rtcc->med_k10.MLDTime, "Enter time of ignition (Format: hhh:mm:ss)");
}

void ApolloRTCCMFD::menuSetSPQElevation()
{
	bool SPQElevInput(void* id, char *str, void *data);
	oapiOpenInputBox("Elevation in degrees:", SPQElevInput, 0, 20, (void*)this);
}

bool SPQElevInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_SPQElevation(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SPQElevation(double elev)
{
	this->GC->rtcc->GZGENCSN.SPQElevationAngle = elev * RAD;
}

void ApolloRTCCMFD::menuSetSPQTerminalPhaseAngle()
{
	bool SPQTerminalPhaseAngleInput(void* id, char *str, void *data);
	oapiOpenInputBox("Terminal phase angle in degrees:", SPQTerminalPhaseAngleInput, 0, 20, (void*)this);
}

bool SPQTerminalPhaseAngleInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_SPQTerminalPhaseAngle(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SPQTerminalPhaseAngle(double wt)
{
	this->GC->rtcc->GZGENCSN.SPQTerminalPhaseAngle = wt * RAD;
}

void ApolloRTCCMFD::menuSetSPQTPIDefinitionValue()
{
	bool SPQTPIDefinitionValueInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the TPI time (Format: hhh:mm:ss)", SPQTPIDefinitionValueInput, 0, 20, (void*)this);
}

bool SPQTPIDefinitionValueInput(void *id, char *str, void *data)
{
	double t1time;
	int hh, mm, ss;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_SPQTPIDefinitionValue(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SPQTPIDefinitionValue(double get)
{
	GC->rtcc->GZGENCSN.TPIDefinitionValue = get;
}

void ApolloRTCCMFD::menuCycleSPQCDHPoint()
{
	if (GC->rtcc->med_k01.I_CDH < 4)
	{
		GC->rtcc->med_k01.I_CDH++;
	}
	else
	{
		GC->rtcc->med_k01.I_CDH = 1;
	}
}

void ApolloRTCCMFD::menuSPQCDHValue()
{
	bool SPQCDHValueInput(void* id, char *str, void *data);
	if (GC->rtcc->med_k01.I_CDH == 3)
	{
		oapiOpenInputBox("Angle from CSI to CDH:", SPQCDHValueInput, 0, 20, (void*)this);
	}
	else if (GC->rtcc->med_k01.I_CDH == 2)
	{
		oapiOpenInputBox("GET of CDH:", SPQCDHValueInput, 0, 20, (void*)this);
	}
	else
	{
		oapiOpenInputBox("No. of apsis since CSI:", SPQCDHValueInput, 0, 20, (void*)this);
	}
}

bool SPQCDHValueInput(void* id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		return ((ApolloRTCCMFD*)data)->set_SPQCDHValue(str);
	}
	return false;
}

bool ApolloRTCCMFD::set_SPQCDHValue(char* val)
{
	if (GC->rtcc->med_k01.I_CDH == 3)
	{
		double angle;
		if (sscanf(val, "%lf", &angle) == 1)
		{
			GC->rtcc->med_k01.CDH_Angle = angle * RAD;
			return true;
		}
	}
	else if (GC->rtcc->med_k01.I_CDH == 2)
	{
		int hh, mm;
		double ss;
		if (sscanf(val, "%d:%d:%lf", &hh, &mm, &ss) == 3)
		{
			GC->rtcc->med_k01.CDH_Time = ss + 60 * (mm + 60 * hh);
			return true;
		}
	}
	else
	{
		int n;
		if (sscanf(val, "%d", &n) == 1)
		{
			GC->rtcc->med_k01.CDH_Apsis = n;
			return true;
		}
	}
	return false;
}

void ApolloRTCCMFD::menuSetDKIElevation()
{
	GenericDoubleInput(&GC->rtcc->GZGENCSN.DKIElevationAngle, "Elevation in degrees:", RAD);
}

void ApolloRTCCMFD::menuSetDKITerminalPhaseAngle()
{
	GenericDoubleInput(&GC->rtcc->GZGENCSN.DKITerminalPhaseAngle, "Terminal phase angle in degrees:", RAD);
}

void ApolloRTCCMFD::menuSetDKIMinimumPerigee()
{
	GenericDoubleInput(&GC->rtcc->GZGENCSN.DKIMinPerigee, "Minimum perigee in nautical miles:", 1852.0);
}

void ApolloRTCCMFD::menuDKIManeuverLineValue()
{
	GenericDoubleInput(&GC->rtcc->med_k10.MLDValue, "Choose the maneuver line at TIG:", 1.0);
}

void ApolloRTCCMFD::menuDKIInitialPhaseFlag()
{
	GenericIntInput(&GC->rtcc->GZGENCSN.DKIPhaseAngleSetting, "Control flag for initial phase angle wrapping. 0 = -180 to 180. 1 = 0 to 360. -1 = -360 to 0 and so on...");
}

void ApolloRTCCMFD::menuCycleDKITerminalPhaseOption()
{
	if (GC->rtcc->GZGENCSN.DKI_TP_Definition < 6)
	{
		GC->rtcc->GZGENCSN.DKI_TP_Definition++;
	}
	else
	{
		GC->rtcc->GZGENCSN.DKI_TP_Definition = 1; //TBD: Should be 0, but option 0 doesn't work
	}

	//Set to 0
	GC->rtcc->GZGENCSN.DKI_TPDefinitionValue = 0.0;
}

void ApolloRTCCMFD::menuDKITerminalPhaseDefinitionValue()
{
	switch (GC->rtcc->GZGENCSN.DKI_TP_Definition)
	{
	case 0: //TPI phase angle
		GenericDoubleInput(&GC->rtcc->GZGENCSN.DKI_TPDefinitionValue, "Choose the phase angle at TPI:", RAD);
		break;
	case 1: //TPI TIG input
		GenericGETInput(&GC->rtcc->GZGENCSN.DKI_TPDefinitionValue, "Choose the time of TPI (Format HHH:MM:SS)");
		break;
	case 2: //TPF TIG input
		GenericGETInput(&GC->rtcc->GZGENCSN.DKI_TPDefinitionValue, "Choose the time of TPF (Format HHH:MM:SS)");
		break;
	case 3: //TPI at X minutes into night
		GenericDoubleInput(&GC->rtcc->GZGENCSN.DKI_TPDefinitionValue, "TPI at X minutes into night:", 1.0);
		break;
	case 4: //TPI at X minutes into day
		GenericDoubleInput(&GC->rtcc->GZGENCSN.DKI_TPDefinitionValue, "TPI at X minutes into day:", 1.0);
		break;
	case 5: //TPF at X minutes into night
		GenericDoubleInput(&GC->rtcc->GZGENCSN.DKI_TPDefinitionValue, "TPF at X minutes into night:", 1.0);
		break;
	case 6: //TPF at X minutes into day
		GenericDoubleInput(&GC->rtcc->GZGENCSN.DKI_TPDefinitionValue, "TPF at X minutes into day:", 1.0);
		break;
	}
}

void ApolloRTCCMFD::menuCycleTPIMode()
{
	if (G->TPI_Mode < 2)
	{
		G->TPI_Mode++;
	}
	else
	{
		G->TPI_Mode = 0;
	}
}

void ApolloRTCCMFD::TPIDTDialogue()
{
	if (G->TPI_Mode == 2)
	{
		GenericDoubleInput(&G->dt_TPI_sunrise, "Choose the TPI time before sunrise in minutes", 60.0);
	}
}

void ApolloRTCCMFD::menuDAPPADCalc()
{
	G->DAPPADCalc(IsCSM);
}

void ApolloRTCCMFD::menuLaunchAzimuthCalc()
{
	VESSEL *v = GC->rtcc->pCSM;

	if (v == NULL) return;

	if (utils::IsVessel(v, utils::SaturnV)) {

		SaturnV *SatV = (SaturnV*)v;
		if (SatV->iu)
		{
			LVDCSV *lvdc = (LVDCSV*)SatV->iu->GetLVDC();

			double day = 0.0;
			double MJD_GRR = oapiGetSimMJD() - (SatV->GetMissionTime() + 17.0) / 24.0 / 3600.0;
			double T_L = modf(MJD_GRR, &day)*24.0*3600.0;
			double t_D = T_L - lvdc->T_LO;
			//t_D = TABLE15.target[tgt_index].t_D;

			//Azimuth determination
			if (t_D < lvdc->t_DS1)
			{
				G->LVDCLaunchAzimuth = lvdc->hx[0][0] + lvdc->hx[0][1] * ((t_D - lvdc->t_D1) / lvdc->t_SD1) + lvdc->hx[0][2] * pow((t_D - lvdc->t_D1) / lvdc->t_SD1, 2) + lvdc->hx[0][3] * pow((t_D - lvdc->t_D1) / lvdc->t_SD1, 3) + lvdc->hx[0][4] * pow((t_D - lvdc->t_D1) / lvdc->t_SD1, 4);
			}
			else if (lvdc->t_DS1 <= t_D && t_D < lvdc->t_DS2)
			{
				G->LVDCLaunchAzimuth = lvdc->hx[1][0] + lvdc->hx[1][1] * ((t_D - lvdc->t_D2) / lvdc->t_SD2) + lvdc->hx[1][2] * pow((t_D - lvdc->t_D2) / lvdc->t_SD2, 2) + lvdc->hx[1][3] * pow((t_D - lvdc->t_D2) / lvdc->t_SD2, 3) + lvdc->hx[1][4] * pow((t_D - lvdc->t_D2) / lvdc->t_SD2, 4);
			}
			else
			{
				G->LVDCLaunchAzimuth = lvdc->hx[2][0] + lvdc->hx[2][1] * ((t_D - lvdc->t_D3) / lvdc->t_SD3) + lvdc->hx[2][2] * pow((t_D - lvdc->t_D3) / lvdc->t_SD3, 2) + lvdc->hx[2][3] * pow((t_D - lvdc->t_D3) / lvdc->t_SD3, 3) + lvdc->hx[2][4] * pow((t_D - lvdc->t_D3) / lvdc->t_SD3, 4);
			}

			G->LVDCLaunchAzimuth *= RAD;
		}
	}
}

void ApolloRTCCMFD::menuAscentPADCalc()
{
	G->AscentPADCalc();
}

void ApolloRTCCMFD::menuCycleAscentPADVersion()
{
	if (G->AscentPADVersion < 1)
	{
		G->AscentPADVersion++;
	}
	else
	{
		G->AscentPADVersion = 0;
	}
}

void ApolloRTCCMFD::menuPDAPCalc()
{
	G->PDAPCalc();
}

void ApolloRTCCMFD::menuCyclePDAPSegments()
{
	G->PDAPTwoSegment = !G->PDAPTwoSegment;
}

void ApolloRTCCMFD::menuCyclePDAPEngine()
{
	if (G->PDAPEngine < 1)
	{
		G->PDAPEngine++;
	}
	else
	{
		G->PDAPEngine = 0;
	}
}

void ApolloRTCCMFD::menuAP11AbortCoefUplink()
{
	VESSEL *v = GC->rtcc->pLM;

	if (v == NULL || utils::IsVessel(v, utils::LEM) == false) return;

	if (G->PDAPTwoSegment == false)
	{
		G->AP11AbortCoefUplink();
	}
	else
	{
		G->AP12AbortCoefUplink();
	}
}

void ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETL()
{
	menuGeneralMEDRequest("Format: U14,CSM or LEM,Time in hhh:mm:ss;");
}

void ApolloRTCCMFD::menuSetFIDOOrbitDigitalsL()
{
	menuGeneralMEDRequest("Format: U13,CSM or LEM,Revolution,desired longitude;");
}

void ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETBV()
{
	menuGeneralMEDRequest("Format: U12, CSM or LEM,REV or GET or MNV,rev no/time or mnv no;");
}

void ApolloRTCCMFD::menuSpaceDigitalsInit()
{
	if (GC->MissionPlanningActive)
	{
		bool SpaceDigitalsInitInput(void* id, char *str, void *data);
		oapiOpenInputBox("Format: U00, CSM or LEM, E or M (optional);", SpaceDigitalsInitInput, 0, 20, (void*)this);
	}
	else
	{
		if (GC->rtcc->EZETVMED.SpaceDigVehID == RTCC_MPT_CSM)
		{
			GC->rtcc->EZETVMED.SpaceDigVehID = RTCC_MPT_LM;
		}
		else
		{
			GC->rtcc->EZETVMED.SpaceDigVehID = RTCC_MPT_CSM;
		}
	}
}

bool SpaceDigitalsInitInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::menuGenerateSpaceDigitals()
{
	if (GC->MissionPlanningActive)
	{
		bool GenerateSpaceDigitalsInput(void* id, char *str, void *data);
		oapiOpenInputBox("Generate Space Digitals, format: U01, column (1-3), option (GET or MNV), parameter (time or mnv number), Inclination (Col. 2), Long Ascending Node (Col. 2);", GenerateSpaceDigitalsInput, 0, 50, (void*)this);
	}
	else
	{
		bool GenerateSpaceDigitalsNoMPTInput(void* id, char *str, void *data);
		oapiOpenInputBox("Generate Space Digitals, format: Option GET (e.g. 1 100:00:00)", GenerateSpaceDigitalsNoMPTInput, 0, 50, (void*)this);
	}
}

bool GenerateSpaceDigitalsNoMPTInput(void* id, char *str, void *data)
{
	int opt, hh, mm;
	double ss;
	if (sscanf(str, "%d %d:%d:%lf", &opt, &hh, &mm, &ss) == 4)
	{
		if (opt < 1 || opt > 3)
		{
			return false;
		}

		double get = hh * 3600.0 + mm * 60.0 + ss;
		((ApolloRTCCMFD*)data)->set_SpaceDigitalsNoMPT(opt, get);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SpaceDigitalsNoMPT(int opt, double get)
{
	G->SpaceDigitalsOption = opt;
	G->SpaceDigitalsGET = get;
	G->GenerateSpaceDigitalsNoMPT();
}

bool GenerateSpaceDigitalsInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::menuMPTCycleActive()
{
	GC->MissionPlanningActive = !GC->MissionPlanningActive;
}

void ApolloRTCCMFD::menuMPTDeleteManeuver()
{
	bool MPTDeleteManeuverInput(void* id, char *str, void *data);
	oapiOpenInputBox("Freeze/unfreeze/delete maneuver in MPT (Format: M62,Table,Manv No.,Action,Vector;)", MPTDeleteManeuverInput, 0, 50, (void*)this);
}

bool MPTDeleteManeuverInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::menuMPTCopyEphemeris()
{
	bool MPTCopyEphemerisInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: P16, OldVeh, NewVeh, GMT, ManNum;", MPTCopyEphemerisInput, 0, 50, (void*)this);
}

bool MPTCopyEphemerisInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::menuMPTVehicleOrientationChange()
{
	bool MPTVehicleOrientationChangeInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: M58, CSM or LEM, maneuver number, U (Up) or D (Down), S or C, system param or trim angle computed (optional);", MPTVehicleOrientationChangeInput, 0, 50, (void*)this);
}

bool MPTVehicleOrientationChangeInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::menuMPTTLIDirectInput()
{
	bool MPTTLIDirectInputInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: M68, CSM or LEM, Opportunity (1 or 2);", MPTTLIDirectInputInput, 0, 50, (void*)this);
}

bool MPTTLIDirectInputInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->set_MPTTLIDirectInput(str);
	return true;
}

void ApolloRTCCMFD::set_MPTTLIDirectInput(char *str)
{
	sprintf_s(GC->rtcc->RTCCMEDBUFFER, 256, str);
	G->MPTTLIDirectInput();
}

void ApolloRTCCMFD::menuNextStationContactLunar()
{
	if (GC->rtcc->SystemParameters.MGRTAG == 1)
	{
		GeneralMEDRequest("B04,START;");
	}
	else
	{
		GeneralMEDRequest("B04,STOP;");
	}
}

void ApolloRTCCMFD::menuGenerateStationContacts()
{
	bool GenerateStationContactsInput(void* id, char *str, void *data);
	oapiOpenInputBox("Generate station contacts (Format: B03, CSM or LEM;)", GenerateStationContactsInput, 0, 20, (void*)this);
}

bool GenerateStationContactsInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::PredSiteAcqCSM1Calc()
{
	bool PredSiteAcqCSM1Input(void* id, char *str, void *data);
	oapiOpenInputBox("Format: U15, CSM, REV or GET, Begin GET or rev, Delta Time or end rev, ref body (optional);", PredSiteAcqCSM1Input, 0, 50, (void*)this);
}

bool PredSiteAcqCSM1Input(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::PredSiteAcqLM1Calc()
{
	bool PredSiteAcqLM1Input(void* id, char *str, void *data);
	oapiOpenInputBox("Format: U15, LEM, REV or GET, Begin GET or rev, Delta Time or end rev, ref body (optional);", PredSiteAcqLM1Input, 0, 50, (void*)this);
}

bool PredSiteAcqLM1Input(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::PredSiteAcqCSM2Calc()
{
	bool PredSiteAcqCSM2Input(void* id, char *str, void *data);
	oapiOpenInputBox("Format: U55, CSM, REV or GET, Begin GET or rev, Delta Time or end rev, ref body (optional);", PredSiteAcqCSM2Input, 0, 50, (void*)this);
}

bool PredSiteAcqCSM2Input(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::PredSiteAcqLM2Calc()
{
	bool PredSiteAcqLM2Input(void* id, char *str, void *data);
	oapiOpenInputBox("Format: U15, LEM, REV or GET, Begin GET or rev, Delta Time or end rev, ref body (optional);", PredSiteAcqLM2Input, 0, 50, (void*)this);
}

bool PredSiteAcqLM2Input(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::ExpSiteAcqLMCalc()
{
	bool ExpSiteAcqLMCalcInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: U16, CSM, REV or GET, Begin GET or rev, Delta Time or end rev, ref body (optional);", ExpSiteAcqLMCalcInput, 0, 50, (void*)this);
}

bool ExpSiteAcqLMCalcInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::LandmarkAcqDisplayCalc()
{
	bool LandmarkAcqDisplayCalcInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: U17, CSM, GET (threshold), Delta Time (0 to 24 hours), Ref Body (E or M);", LandmarkAcqDisplayCalcInput, 0, 50, (void*)this);
}

bool LandmarkAcqDisplayCalcInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::GroundPointTableUpdate()
{
	bool GroundPointTableUpdateInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: P32, Action Code (ADD,MOD,DEL),Earth/Moon Ind (E or M),Data Table Ind (EXST,LDMK),Station ID,Lat,Long,Height units (METR or NM),Height;", GroundPointTableUpdateInput, 0, 50, (void*)this);
}

bool GroundPointTableUpdateInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::CyclePredSiteAcqPage()
{
	PredictedSiteAcquisitionTable *tab;
	if (screen == 46)
	{
		tab = &GC->rtcc->EZACQ1;
	}
	else if (screen == 72)
	{
		tab = &GC->rtcc->EZACQ3;
	}
	else if (screen == 73)
	{
		tab = &GC->rtcc->EZDPSAD1;
	}
	else
	{
		tab = &GC->rtcc->EZDPSAD3;
	}

	if (tab->curpage < tab->pages)
	{
		tab->curpage++;
	}
	else
	{
		tab->curpage = 1;
	}
}

void ApolloRTCCMFD::CycleExpSiteAcqPage()
{
	if (GC->rtcc->EZDPSAD2.curpage < GC->rtcc->EZDPSAD2.pages)
	{
		GC->rtcc->EZDPSAD2.curpage++;
	}
	else
	{
		GC->rtcc->EZDPSAD2.curpage = 1;
	}
}

void ApolloRTCCMFD::CycleLandmarkAcqDisplayPage()
{
	if (GC->rtcc->EZLANDU1.curpage < GC->rtcc->EZLANDU1.pages)
	{
		GC->rtcc->EZLANDU1.curpage++;
	}
	else
	{
		GC->rtcc->EZLANDU1.curpage = 1;
	}
}

void ApolloRTCCMFD::RelativeMotionDigitalsCalc()
{
	bool RelativeMotionDigitalsCalcInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: U03,Chaser (CSM, LEM),Target (CSM, LEM),GET,Delta Time (1-1800s),REFSMMAT,AXIS (CX if CSM is Chaser, LX or LZ for LEM),Ref Body (E or M),optional: Mode (1 or 2),Pitch,Yaw,Roll,PYR GET;", RelativeMotionDigitalsCalcInput, 0, 50, (void*)this);
}

bool RelativeMotionDigitalsCalcInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::menuChooseRETPlan()
{
	bool ChooseRETPlanInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: U06,Plan Number (1-7);", ChooseRETPlanInput, 0, 50, (void*)this);
}

bool ChooseRETPlanInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::menuSetLDPPAzimuth()
{
	GenericDoubleInput(&GC->rtcc->GZGENCSN.LDPPAzimuth, "Approach azimuth at the landing site (0 for optimal):", RAD);
}

void ApolloRTCCMFD::menuSetLDPPPoweredDescTime()
{
	GenericGETInput(&GC->rtcc->GZGENCSN.LDPPTimeofPDI, "Time for powered descent ignition (0 to calculate it internally):");
}

void ApolloRTCCMFD::menuLDPPThresholdTime1()
{
	bool LDPPThresholdTime1Input(void* id, char *str, void *data);
	oapiOpenInputBox("Threshold time 1 (Format: hhh:mm:ss)", LDPPThresholdTime1Input, 0, 20, (void*)this);
}

bool LDPPThresholdTime1Input(void* id, char *str, void *data)
{
	int hh, mm, ss, dt;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		dt = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_LDPPThresholdTime(dt, 1);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuLDPPThresholdTime2()
{
	bool LDPPThresholdTime2Input(void* id, char *str, void *data);
	oapiOpenInputBox("Threshold time 2 (Format: hhh:mm:ss)", LDPPThresholdTime2Input, 0, 20, (void*)this);
}

bool LDPPThresholdTime2Input(void* id, char *str, void *data)
{
	int hh, mm, ss, dt;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		dt = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_LDPPThresholdTime(dt, 2);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuLDPPThresholdTime3()
{
	bool LDPPThresholdTime3Input(void* id, char *str, void *data);
	oapiOpenInputBox("Threshold time 3 (Format: hhh:mm:ss)", LDPPThresholdTime3Input, 0, 20, (void*)this);
}

bool LDPPThresholdTime3Input(void* id, char *str, void *data)
{
	int hh, mm, ss, dt;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		dt = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_LDPPThresholdTime(dt, 3);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuLDPPThresholdTime4()
{
	bool LDPPThresholdTime4Input(void* id, char *str, void *data);
	oapiOpenInputBox("Threshold time 4 (Format: hhh:mm:ss)", LDPPThresholdTime4Input, 0, 20, (void*)this);
}

bool LDPPThresholdTime4Input(void* id, char *str, void *data)
{
	int hh, mm, ss, dt;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		dt = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_LDPPThresholdTime(dt, 4);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LDPPThresholdTime(double dt, int thr)
{
	if (thr == 1)
	{
		GC->rtcc->med_k16.GETTH1 = dt;
		if (GC->rtcc->med_k16.GETTH2 < GC->rtcc->med_k16.GETTH1)
		{
			GC->rtcc->med_k16.GETTH2 = GC->rtcc->med_k16.GETTH1;
		}
		if (GC->rtcc->med_k16.GETTH3 < GC->rtcc->med_k16.GETTH2)
		{
			GC->rtcc->med_k16.GETTH3 = GC->rtcc->med_k16.GETTH2;
		}
		if (GC->rtcc->med_k16.GETTH4 < GC->rtcc->med_k16.GETTH3)
		{
			GC->rtcc->med_k16.GETTH4 = GC->rtcc->med_k16.GETTH3;
		}
	}
	else if (thr == 2)
	{
		GC->rtcc->med_k16.GETTH2 = dt;
		if (GC->rtcc->med_k16.GETTH3 < GC->rtcc->med_k16.GETTH2)
		{
			GC->rtcc->med_k16.GETTH3 = GC->rtcc->med_k16.GETTH2;
		}
		if (GC->rtcc->med_k16.GETTH4 < GC->rtcc->med_k16.GETTH3)
		{
			GC->rtcc->med_k16.GETTH4 = GC->rtcc->med_k16.GETTH3;
		}
	}
	else if (thr == 3)
	{
		GC->rtcc->med_k16.GETTH3 = dt;
		if (GC->rtcc->med_k16.GETTH4 < GC->rtcc->med_k16.GETTH3)
		{
			GC->rtcc->med_k16.GETTH4 = GC->rtcc->med_k16.GETTH3;
		}
	}
	else if (thr == 4)
	{
		GC->rtcc->med_k16.GETTH4 = dt;
	}
}

void ApolloRTCCMFD::menuSetLDPPDescentFlightTime()
{
	GenericDoubleInput(&GC->rtcc->GZGENCSN.LDPPDescentFlightTime, "Descent flight time in minutes:", 60.0);
}

void ApolloRTCCMFD::cycleLDPPVehicle()
{
	if (GC->rtcc->med_k16.Vehicle == RTCC_MPT_CSM)
	{
		GC->rtcc->med_k16.Vehicle = RTCC_MPT_LM;
	}
	else
	{
		GC->rtcc->med_k16.Vehicle = RTCC_MPT_CSM;
	}
}

void ApolloRTCCMFD::menuSetLDPPDesiredHeight()
{
	GenericDoubleInput(&GC->rtcc->med_k16.DesiredHeight, "Desired height in NM:", 1852.0);
}

void ApolloRTCCMFD::menuSunriseSunsetTimesCalc()
{
	menuGeneralMEDRequest("Format: U08,GET or REV,Time or Rev number;");
}

void ApolloRTCCMFD::menuMoonriseMoonsetTimesCalc()
{
	menuGeneralMEDRequest("Format: U07,GET or REV,Time or Rev number;");
}

void ApolloRTCCMFD::menuCapeCrossingInit()
{
	menuGeneralMEDRequest("Format: P17,Vehicle (CSM or LEM), E or M,Revolution;");
}

void ApolloRTCCMFD::menuGenerateDMT()
{
	menuGeneralMEDRequest("Format: U20,MPT ID,Maneuver No,MSK No (54 or 69),REFSMMAT,Heads Up/Down;");
}

void ApolloRTCCMFD::menuCycleSFPDisplay()
{
	if (GC->rtcc->PZSFPTAB.DisplayBlockNum < 2)
	{
		GC->rtcc->PZSFPTAB.DisplayBlockNum++;
	}
	else
	{
		GC->rtcc->PZSFPTAB.DisplayBlockNum = 1;
	}
}

void ApolloRTCCMFD::menuAlterationSFPData()
{
	bool AlterationSFPDataInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: F32, Block (1-2), Item (1-26), Value;", AlterationSFPDataInput, 0, 50, (void*)this);
}

bool AlterationSFPDataInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::menuTransferMCCPlanToSFP()
{
	bool TransferMCCPlanToSFPInput(void* id, char *str, void *data);
	oapiOpenInputBox("Column to move to SFP Block 2. Format: F30,Column Number;", TransferMCCPlanToSFPInput, 0, 50, (void*)this);
}

bool TransferMCCPlanToSFPInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::menuInterpolateSFP()
{
	bool InterpolateSFPInput(void* id, char *str, void *data);
	oapiOpenInputBox("Interplation for skeleton flight plan. Format: F62,Days (optional),Opportunity,Azimuth;", InterpolateSFPInput, "F62,,1,72.0;", 50, (void*)this);
}

bool InterpolateSFPInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::menuDeleteMidcourseColumn()
{
	bool DeleteMidcourseColumnInput(void* id, char *str, void *data);
	oapiOpenInputBox("Delete midcourse tradeoff column. Format: F26,Column; (0 for all)", DeleteMidcourseColumnInput, 0, 50, (void*)this);
}

bool DeleteMidcourseColumnInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->GeneralMEDRequest(str);
	return true;
}

void ApolloRTCCMFD::menuCycleNodeConvOption()
{
	G->NodeConvOpt = !G->NodeConvOpt;
}

void ApolloRTCCMFD::menuNodeConvCalc()
{
	G->NodeConvCalc();
}

void ApolloRTCCMFD::menuSendNodeToSFP()
{
	G->SendNodeToSFP();
}

void ApolloRTCCMFD::menuSetNodeConvGET()
{
	bool NodeConvGETInput(void* id, char *str, void *data);
	oapiOpenInputBox("Input GET at node (Format: HH:MM:SS)", NodeConvGETInput, 0, 20, (void*)this);
}

bool NodeConvGETInput(void* id, char *str, void *data)
{
	double hh, mm, ss, get;
	if (sscanf(str, "%lf:%lf:%lf", &hh, &mm, &ss) == 3)
	{
		get = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_NodeConvGET(get);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_NodeConvGET(double get)
{
	G->NodeConvGET = get;
}

void ApolloRTCCMFD::menuSetNodeConvLat()
{
	bool NodeConvLatInput(void* id, char *str, void *data);
	oapiOpenInputBox("Input latitude at node in degrees:", NodeConvLatInput, 0, 20, (void*)this);
}

bool NodeConvLatInput(void* id, char *str, void *data)
{
	double lat;
	if (sscanf(str, "%lf", &lat) == 1)
	{
		((ApolloRTCCMFD*)data)->set_NodeConvLat(lat);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_NodeConvLat(double lat)
{
	G->NodeConvLat = lat * RAD;
}

void ApolloRTCCMFD::menuSetNodeConvLng()
{
	bool NodeConvLngInput(void* id, char *str, void *data);
	oapiOpenInputBox("Input longitude at node in degrees:", NodeConvLngInput, 0, 20, (void*)this);
}

bool NodeConvLngInput(void* id, char *str, void *data)
{
	double lng;
	if (sscanf(str, "%lf", &lng) == 1)
	{
		((ApolloRTCCMFD*)data)->set_NodeConvLng(lng);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_NodeConvLng(double lng)
{
	G->NodeConvLng = lng * RAD;
}

void ApolloRTCCMFD::menuSetNodeConvHeight()
{
	bool NodeConvHeightInput(void* id, char *str, void *data);
	oapiOpenInputBox("Input height at node in NM:", NodeConvHeightInput, 0, 20, (void*)this);
}

bool NodeConvHeightInput(void* id, char *str, void *data)
{
	double height;
	if (sscanf(str, "%lf", &height) == 1)
	{
		((ApolloRTCCMFD*)data)->set_NodeConvHeight(height);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_NodeConvHeight(double height)
{
	G->NodeConvHeight = height * 1852.0;
}

void ApolloRTCCMFD::menuCalculateTPITime()
{
	G->CalculateTPITime();
}

void ApolloRTCCMFD::menuVectorCompareDisplayCalc()
{
	G->VectorCompareDisplayCalc();
}

void ApolloRTCCMFD::menuVectorCompareColumn1()
{
	bool VectorCompareColumn1Input(void* id, char *str, void *data);
	oapiOpenInputBox("Select vector for column 1. EPHO = Orbit Ephemeris, otherwise ID from Vector Panel Summary:", VectorCompareColumn1Input, 0, 20, (void*)this);
}

bool VectorCompareColumn1Input(void* id, char *str, void *data)
{
	if (strlen(str) < 8)
	{
		std::string buf(str);
		((ApolloRTCCMFD*)data)->set_VectorCompareColumn(buf, 1);

		return true;
	}

	return false;
}

void ApolloRTCCMFD::menuVectorCompareColumn2()
{
	bool VectorCompareColumn2Input(void* id, char *str, void *data);
	oapiOpenInputBox("Select vector for column 2. EPHO = Orbit Ephemeris, otherwise ID from Vector Panel Summary:", VectorCompareColumn2Input, 0, 20, (void*)this);
}

bool VectorCompareColumn2Input(void* id, char *str, void *data)
{
	if (strlen(str) < 8)
	{
		std::string buf(str);
		((ApolloRTCCMFD*)data)->set_VectorCompareColumn(buf, 2);

		return true;
	}

	return false;
}

void ApolloRTCCMFD::menuVectorCompareColumn3()
{
	bool VectorCompareColumn3Input(void* id, char *str, void *data);
	oapiOpenInputBox("Select vector for column 3. EPHO = Orbit Ephemeris, otherwise ID from Vector Panel Summary:", VectorCompareColumn3Input, 0, 20, (void*)this);
}

bool VectorCompareColumn3Input(void* id, char *str, void *data)
{
	if (strlen(str) < 8)
	{
		std::string buf(str);
		((ApolloRTCCMFD*)data)->set_VectorCompareColumn(buf, 3);

		return true;
	}

	return false;
}

void ApolloRTCCMFD::menuVectorCompareColumn4()
{
	bool VectorCompareColumn4Input(void* id, char *str, void *data);
	oapiOpenInputBox("Select vector for column 4. EPHO = Orbit Ephemeris, otherwise ID from Vector Panel Summary:", VectorCompareColumn4Input, 0, 20, (void*)this);
}

bool VectorCompareColumn4Input(void* id, char *str, void *data)
{
	if (strlen(str) < 8)
	{
		std::string buf(str);
		((ApolloRTCCMFD*)data)->set_VectorCompareColumn(buf, 4);

		return true;
	}

	return false;
}

void ApolloRTCCMFD::set_VectorCompareColumn(std::string vec, int col)
{
	GC->rtcc->med_s80.VID[col - 1] = vec;
}

void ApolloRTCCMFD::menuVectorCompareVehicle()
{
	if (GC->rtcc->med_s80.VEH == 1)
	{
		GC->rtcc->med_s80.VEH = 3;
	}
	else
	{
		GC->rtcc->med_s80.VEH = 1;
	}
}

void ApolloRTCCMFD::menuVectorCompareReference()
{
	if (GC->rtcc->med_s80.REF < 1)
	{
		GC->rtcc->med_s80.REF++;
	}
	else
	{
		GC->rtcc->med_s80.REF = 0;
	}
}

void ApolloRTCCMFD::menuVectorCompareTime()
{
	bool VectorCompareTimeInput(void* id, char *str, void *data);
	oapiOpenInputBox("Enter vector time. GMT if positive, GET if negative. Time of V1 vector if zero. (Format HH:MM:SS):", VectorCompareTimeInput, 0, 20, (void*)this);
}

bool VectorCompareTimeInput(void* id, char *str, void *data)
{
	int hh, mm, ss;
	double time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		time = ss + 60 * (mm + 60 * abs(hh));
		if (str[0] == '-')
		{
			time = -time;
		}
		((ApolloRTCCMFD*)data)->set_VectorCompareTime(time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_VectorCompareTime(double time)
{
	GC->rtcc->med_s80.time = time;
}

void ApolloRTCCMFD::menuGOSTDisplayREFSMMAT()
{
	bool GOSTDisplayREFSMMATInput(void* id, char *str, void *data);
	oapiOpenInputBox("Select REFSMMAT to display (CUR, PCR, TLM, OST, MED, DMT, DOD or LCV):", GOSTDisplayREFSMMATInput, 0, 20, (void*)this);
}

bool GOSTDisplayREFSMMATInput(void* id, char *str, void *data)
{
	std::string mat, med;

	mat.assign(str);
	med = "G10,CSM,,,,," + mat + ";";
	char Buff[64];
	sprintf_s(Buff, 64, med.c_str());

	((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buff);
	return true;
}

void ApolloRTCCMFD::menuGOSTBoresightSCTCalc()
{
	bool GOSTBoresightSCTCalcInput(void* id, char *str, void *data);
	oapiOpenInputBox("Boresight/SCT Data. Format: HHH:MM:SS NNN XXX (GET, Starting Star, REFSMMAT type)", GOSTBoresightSCTCalcInput, 0, 20, (void*)this);
}

bool GOSTBoresightSCTCalcInput(void* id, char *str, void *data)
{
	double ss;
	int hh, mm, star;
	char ref[16];

	if (sscanf(str, "%d:%d:%lf %d %15s", &hh, &mm, &ss, &star, ref) == 5)
	{
		std::string med, med2;
		med2.assign(ref);

		med = "G10,CSM," + std::to_string(hh) + ":" + std::to_string(mm) + ":" + std::to_string(ss) + "," + std::to_string(star) + "," + med2 + ",,;";
		char Buff[64];
		sprintf_s(Buff, 64, med.c_str());
		((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buff);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuGOSTSXTCalc()
{
	bool GOSTSXTCalcInput(void* id, char *str, void *data);
	oapiOpenInputBox("Sextant Data. Format: HHH:MM:SS NNN XXX (GET, Starting Star, REFSMMAT type)", GOSTSXTCalcInput, 0, 20, (void*)this);
}

bool GOSTSXTCalcInput(void* id, char *str, void *data)
{
	double ss;
	int hh, mm, star;
	char ref[16];

	if (sscanf(str, "%d:%d:%lf %d %15s", &hh, &mm, &ss, &star, &ref) == 5)
	{
		std::string med, med2;
		med2.assign(ref);

		med = "G10,CSM," + std::to_string(hh) + ":" + std::to_string(mm) + ":" + std::to_string(ss) + "," + std::to_string(star) + ",," + med2 + ",;";
		char Buff[64];
		sprintf_s(Buff, 64, med.c_str());
		((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buff);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuGOSTEnterAttitude()
{
	bool GOSTEnterAttitudeInput(void* id, char *str, void *data);
	oapiOpenInputBox("Enter attitude 1 or 2. Format: 1 or 2 P Y R", GOSTEnterAttitudeInput, 0, 20, (void*)this);
}

bool GOSTEnterAttitudeInput(void* id, char *str, void *data)
{
	int num;
	VECTOR3 Att;
	if (sscanf(str, "%d %lf %lf %lf", &num, &Att.x, &Att.y, &Att.z) == 4)
	{
		std::string med;

		med = "G12,CSM,,,,,,,";
		if (num == 2)
		{
			med += ",,,";
		}
		med += std::to_string(Att.x);
		med += ",";
		med += std::to_string(Att.y);
		med += ",";
		med += std::to_string(Att.z);
		med += ";";

		char Buff[64];
		sprintf_s(Buff, 64, med.c_str());

		((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buff);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuGOSTEnterSXTData()
{
	bool GOSTEnterSXTDataInput(void* id, char *str, void *data);
	oapiOpenInputBox("Enter sextant data 1 or 2. Format: 1/2 Star Shaft Trunnion", GOSTEnterSXTDataInput, 0, 20, (void*)this);
}

bool GOSTEnterSXTDataInput(void* id, char *str, void *data)
{
	int num;
	unsigned star;
	double SA, TA;
	if (sscanf(str, "%d %d %lf %lf", &num, &star, &SA, &TA) == 4)
	{
		std::string med;

		med = "G12,CSM,";
		if (num == 2)
		{
			med += ",,,";
		}
		med += std::to_string(star);
		med += ",";
		med += std::to_string(SA);
		med += ",";
		med += std::to_string(TA);
		med += ";";

		char Buff[64];
		sprintf_s(Buff, 64, med.c_str());

		((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buff);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuGOSTShowStarVector()
{
	bool GOSTShowStarVectorInput(void* id, char *str, void *data);
	oapiOpenInputBox("Enter number of star to display:", GOSTShowStarVectorInput, 0, 20, (void*)this);
}

bool GOSTShowStarVectorInput(void* id, char *str, void *data)
{
	unsigned star;
	if (sscanf(str, "%d", &star) == 1)
	{
		std::string med;
		med = "G14,CSM," + std::to_string(star) + ";";
		char Buff[64];
		sprintf_s(Buff, "%s", med.c_str());
		((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buff);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuGOSTShowLandmarkVector()
{
	bool GOSTShowLandmarkVectorInput(void* id, char *str, void *data);
	oapiOpenInputBox("Landmark vector. Format: VEHICLE (CSM or LEM) GET BODY (S, M or E) LAT LONG HEIGHT (in feet)", GOSTShowLandmarkVectorInput, 0, 40, (void*)this);
}

bool GOSTShowLandmarkVectorInput(void* id, char *str, void *data)
{
	int hh, mm;
	double ss, lat = 0.0, lng = 0.0, height = 0.0;
	char buff[16], body;
	if (sscanf(str, "%15s %d:%d:%lf %c %lf %lf %lf", buff, &hh, &mm, &ss, &body, &lat, &lng, &height) >= 3)
	{
		std::string med, med2;
		med2.assign(buff);

		med = "G14," + med2 + ",," + std::to_string(hh) + ":" + std::to_string(mm) + ":" + std::to_string(ss) + "," + body + "," + std::to_string(lat) + "," + std::to_string(lng) + "," + std::to_string(height) + ";";
		char Buff[64];
		sprintf_s(Buff, "%s", med.c_str());
		((ApolloRTCCMFD*)data)->GeneralMEDRequest(Buff);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuGOSTEnterStarInCatalog()
{
	menuGeneralMEDRequest("Enter target for star catalog. Input format: G13,Position (392 to 400),Right Ascension (hr:min:sec), Declination (deg:min:sec);");
}

void ApolloRTCCMFD::menuSaveOSTREFSMMAT()
{
	GeneralMEDRequest("G11,CSM,OST-M;");
}

void ApolloRTCCMFD::menuLOSTMode()
{
	bool LOSTModeInput(void* id, char *str, void *data);
	oapiOpenInputBox("Choose mode: DOK, FLT, AGS, LUN, MAT or CHK", LOSTModeInput, 0, 10, (void*)this);
}

bool LOSTModeInput(void* id, char *str, void *data)
{
	std::string Mode(str);
	int num;

	if (Mode == "DOK"){ num = 2;}
	else if (Mode == "FLT"){num = 3;}
	else if (Mode == "AGS"){num = 4;}
	else if (Mode == "LUN"){num = 5;}
	else if (Mode == "MAT"){num = 6;}
	else if (Mode == "CHK"){num = 7;}
	else return false;

	((ApolloRTCCMFD*)data)->set_LOSTMode(num);
	return true;
}

void ApolloRTCCMFD::set_LOSTMode(int mode)
{
	GC->rtcc->EZJGSTBL.MODE = mode;
	GC->rtcc->EMDGLMST();
}

void ApolloRTCCMFD::menuLOSTAttitude1()
{
	if (GC->rtcc->EZJGSTBL.MODE == 2)
	{
		GenericVectorInput(&GC->rtcc->EZJGSTBL.LM_ATT, "Enter LM attitude:", RAD, &ApolloRTCCMFD::UpdateLOSTDisplay);
	}
	else if (GC->rtcc->EZJGSTBL.MODE == 4)
	{
		GenericVectorInput(&GC->rtcc->EZJGSTBL.Att1, "Enter attitude for alignment:", RAD, &ApolloRTCCMFD::UpdateLOSTDisplay);
	}
	else if (GC->rtcc->EZJGSTBL.MODE == 7)
	{
		GenericVectorInput(&GC->rtcc->EZJGSTBL.StoredAttMED, "Enter attitude for star check:", RAD, &ApolloRTCCMFD::UpdateLOSTDisplay);
	}
}

void ApolloRTCCMFD::menuLOSTAttitude2()
{
	if (GC->rtcc->EZJGSTBL.MODE == 2)
	{
		GenericVectorInput(&GC->rtcc->EZJGSTBL.CSM_ATT, "Enter CSM attitude:", RAD, &ApolloRTCCMFD::UpdateLOSTDisplay);
	}
	else if (GC->rtcc->EZJGSTBL.MODE == 4)
	{
		GenericVectorInput(&GC->rtcc->EZJGSTBL.Att2, "Enter attitude for alignment:", RAD, &ApolloRTCCMFD::UpdateLOSTDisplay);
	}
}

void ApolloRTCCMFD::menuLOST_REFSMMAT1()
{
	bool LOST_REFSMMAT_Input1(void* id, char *str, void *data);
	oapiOpenInputBox("Enter LM REFSMMAT type (CUR, PCR, TLM, OST, MED, DMT, DOD, LCV, AGS, DOK, LLA, LLD):", LOST_REFSMMAT_Input1, "CUR", 10, (void*)this);
}

bool LOST_REFSMMAT_Input1(void* id, char *str, void *data)
{
	return ((ApolloRTCCMFD*)data)->set_LOST_REFSMMAT1(str);
}

bool ApolloRTCCMFD::set_LOST_REFSMMAT1(char *str)
{
	GC->rtcc->EZJGSTBL.REF1 = GC->rtcc->EMGSTGENCode(str);
	GC->rtcc->EMDGLMST();
	return true;
}

void ApolloRTCCMFD::menuLOST_REFSMMAT2()
{
	bool LOST_REFSMMAT_Input2(void* id, char *str, void *data);
	oapiOpenInputBox("Enter LM REFSMMAT type (CUR, PCR, TLM, OST, MED, DMT, DOD, LCV, AGS, DOK, LLA, LLD):", LOST_REFSMMAT_Input2, "CUR", 10, (void*)this);
}

bool LOST_REFSMMAT_Input2(void* id, char *str, void *data)
{
	return ((ApolloRTCCMFD*)data)->set_LOST_REFSMMAT2(str);
}

bool ApolloRTCCMFD::set_LOST_REFSMMAT2(char *str)
{
	GC->rtcc->EZJGSTBL.REF2 = GC->rtcc->EMGSTGENCode(str);
	GC->rtcc->EMDGLMST();
	return true;
}

void ApolloRTCCMFD::menuLOST_CSM_REFSMMAT()
{
	if (GC->rtcc->EZJGSTBL.MODE == 2)
	{
		bool LOST_CSM_REFSMMAT_Input(void* id, char *str, void *data);
		oapiOpenInputBox("Enter CSM REFSMMAT type (CUR, PCR, TLM, OST, MED, DMT, DOD, LCV):", LOST_CSM_REFSMMAT_Input, "CUR", 10, (void*)this);
	}
}

bool LOST_CSM_REFSMMAT_Input(void* id, char *str, void *data)
{
	return ((ApolloRTCCMFD*)data)->set_LOST_CSM_REFSMMAT(str);
}

bool ApolloRTCCMFD::set_LOST_CSM_REFSMMAT(char *str)
{
	GC->rtcc->EZJGSTBL.REFSUSED = GC->rtcc->EMGSTGENCode(str);
	GC->rtcc->EMDGLMST();
	return true;
}

void ApolloRTCCMFD::menuLOSTOptics1()
{
	if (GC->rtcc->EZJGSTBL.MODE == 4)
	{
		GenericInt2Input(&GC->rtcc->EZJGSTBL.star1, &GC->rtcc->EZJGSTBL.D1, "Enter star and detent. Format: Star Detent. Limits. Star: 1-400, Detent: 1-6", 1, 400, 1, 6, &ApolloRTCCMFD::UpdateLOSTDisplay);
	}
	else if (GC->rtcc->EZJGSTBL.MODE == 5)
	{
		//Star, detent, cursor angle, spirale angle
	}
}

void ApolloRTCCMFD::menuLOSTOptics2()
{
	if (GC->rtcc->EZJGSTBL.MODE == 4)
	{
		//Star, detent position, cursor angle
		bool LOST_AGS_Star2_Input(void* id, char *str, void *data);
		oapiOpenInputBox("Enter star, detent position and cursor angle. Format: Star Detent-Position Cursor-Angle. Limits. Star: 1-400. Detent position: PX, NX, PY, NY, Angle: 0-360", LOST_AGS_Star2_Input, 0, 30, (void*)this);
	}
	else if (GC->rtcc->EZJGSTBL.MODE == 5)
	{
		//Star, detent, cursor angle, spirale angle
	}
}

bool LOST_AGS_Star2_Input(void* id, char *str, void *data)
{
	double Angle;
	char Buff[3];
	int star;

	if (sscanf(str, "%d %2s %lf", &star, Buff, &Angle) == 3)
	{
		((ApolloRTCCMFD*)data)->set_LOST_AGS_Star2(star, Buff, Angle);
		return true;
	}
	return false;
}

bool ApolloRTCCMFD::set_LOST_AGS_Star2(int star, char *pos, double ang)
{
	int det;

	if (strcmp(pos, "PX") == 0) det = 0;
	else if (strcmp(pos, "NX") == 0) det = 1;
	else if (strcmp(pos, "PY") == 0) det = 2;
	else if (strcmp(pos, "NY") == 0) det = 3;
	else return false;

	if (star < 1 || star > 400) return false;
	if (ang < 0.0 || ang > 360.0) return false;

	GC->rtcc->EZJGSTBL.star2 = star;
	GC->rtcc->EZJGSTBL.L2 = det;
	GC->rtcc->EZJGSTBL.A1_2 = ang * RAD;
	GC->rtcc->EMDGLMST();
	return true;
}

void ApolloRTCCMFD::menuLOSTRealign()
{
	GC->rtcc->EMMGLMST(1);
}

void ApolloRTCCMFD::menuCalcLOST()
{
	if (GC->rtcc->EZJGSTBL.MODE == 2)
	{
		GenericIntInput(&GC->rtcc->EZGSTMED.G23_Option, "Docking Alignment. Option: 1 = Calculate LM REFSMMAT, 2 = LM gimbal angles, 3 = CSM gimbal angles", &ApolloRTCCMFD::CalculateLOSTDOKOption);
	}
	else if (GC->rtcc->EZJGSTBL.MODE == 4)
	{
		GeneralMEDRequest("G20;");
	}
	else if (GC->rtcc->EZJGSTBL.MODE == 7)
	{
		bool LOSTCheckModeInput(void* id, char *str, void *data);
		oapiOpenInputBox("Alignment check. Format: GET (HH:MM:SS) Detent (1-6) COAS axis (PX or PZ)", LOSTCheckModeInput, 0, 30, (void*)this);
	}
}

bool LOSTCheckModeInput(void* id, char *str, void *data)
{
	double hh, mm, ss, get;
	int Detent, COASAxis;
	char Buff[16];

	if (sscanf(str, "%lf:%lf:%lf %d %s", &hh, &mm, &ss, &Detent, Buff) == 5)
	{
		get = hh * 3600.0 + mm * 60.0 + ss;

		if (Detent < 1 || Detent > 6) return false;
		if (strcmp(Buff, "PX") == 0)
		{
			COASAxis = 1;
		}
		else if (strcmp(Buff, "PZ") == 0)
		{
			COASAxis = 2;
		}
		else return false;

		((ApolloRTCCMFD*)data)->set_LOSTCheckMode(get, Detent, COASAxis);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LOSTCheckMode(double get, int Detent, int COASAxis)
{
	GC->rtcc->EZGSTMED.G20_HORIZGET = get;
	GC->rtcc->EZGSTMED.G20_AOT_Detent = Detent;
	GC->rtcc->EZGSTMED.G20_COAS_Axis = COASAxis;

	GeneralMEDRequest("G20;");
}

void ApolloRTCCMFD::CalculateLOSTDOKOption()
{
	GeneralMEDRequest("G23;");
}

void ApolloRTCCMFD::UpdateLOSTDisplay()
{
	GC->rtcc->EMDGLMST();
}

void ApolloRTCCMFD::menuCalculateIMUComparison()
{
	ErrorMessage = G->menuCalculateIMUComparison(IsCSM);
}

void ApolloRTCCMFD::menuCalculateIMUParkingAngles()
{
	agc_t *vagc = G->GetAGCPointer(false);

	if (vagc == NULL) return;

	G->menuCalculateIMUParkingAngles(vagc);
}

void ApolloRTCCMFD::menuSLVNavigationUpdateCalc()
{
	G->SLVNavigationUpdateCalc();
}

void ApolloRTCCMFD::menuSLVNavigationUpdateUplink()
{
	G->SLVNavigationUpdateUplink();
}

void ApolloRTCCMFD::menuGetOnboardStateVectors()
{
	//To update display immediately
	GC->rtcc->VectorPanelSummaryBuffer.gmt = -10000000000000.0;

	G->GetStateVectorFromAGC(true, true);
	G->GetStateVectorFromAGC(true, false);
	G->GetStateVectorFromAGC(false, true);
	G->GetStateVectorFromAGC(false, false);
	G->GetStateVectorFromIU();
	G->GetStateVectorsFromAGS();
}

void ApolloRTCCMFD::menuAGCTimeUpdateComparison()
{
	int i;
	switch (screen)
	{
	case 111: //CMC
		i = 0;
		break;
	case 112: //LGC
		i = 1;
		break;
	default:
		return;
	}

	agc_t *agc;
	double ClockZero;

	if (i == 0)
	{
		//CMC
		VESSEL *v = GC->rtcc->pCSM;

		if (v == NULL || utils::IsVessel(v, utils::Saturn) == false)
		{
			return;
		}

		agc = &((Saturn *)v)->agc.vagc;
		ClockZero = GC->rtcc->GetCMCClockZero();
	}
	else
	{
		//LGC
		VESSEL *v = GC->rtcc->pLM;

		if (v == NULL || utils::IsVessel(v, utils::LEM) == false)
		{
			return;
		}

		agc = &((LEM *)v)->agc.vagc;
		ClockZero = GC->rtcc->GetLGCClockZero();
	}

	G->AGCClockTime[i] = GC->rtcc->GetClockTimeFromAGC(agc) / 100.0;
	double MJD = oapiGetSimMJD();
	double tephem = GC->rtcc->GetGMTBase() + ClockZero / 24.0 / 3600.0;
	G->RTCCClockTime[i] = (MJD - tephem)*24.0*3600.0;
	G->DeltaClockTime[i] = G->RTCCClockTime[i] - G->AGCClockTime[i];
}

void ApolloRTCCMFD::menuAGCLiftoffTimeComparision()
{
	bool AGCLiftoffTimeComparisionInput(void* id, char *str, void *data);
	oapiOpenInputBox("Enter desired liftoff time. Format: HHH:MM:SS.SS", AGCLiftoffTimeComparisionInput, 0, 20, (void*)this);
}

bool AGCLiftoffTimeComparisionInput(void* id, char *str, void *data)
{
	int hh, mm;
	double ss, tim;

	if (sscanf(str, "%d:%d:%lf", &hh, &mm, &ss) == 3)
	{
		tim = ss + mm * 60.0 + hh * 3600.0;
		((ApolloRTCCMFD*)data)->set_AGCLiftoffTimeComparision(tim);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_AGCLiftoffTimeComparision(double tim)
{
	int i;
	switch (screen)
	{
	case 113:
		i = 0;
		break;
	case 114:
		i = 1;
		break;
	default:
		return;
	}

	G->DesiredRTCCLiftoffTime[i] = tim;
}

void ApolloRTCCMFD::menuAGCTimeUpdateCalc()
{
	bool AGCTimeUpdateCalcInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose time increment (Format: hhh:mm:ss.ss)", AGCTimeUpdateCalcInput, 0, 20, (void*)this);
}

bool AGCTimeUpdateCalcInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->set_AGCTimeUpdateCalc(str);
	return true;
}

void ApolloRTCCMFD::set_AGCTimeUpdateCalc(char *dt)
{
	char MED[9];
	switch (screen)
	{
	case 111:
		sprintf(MED, "C07,CMC,");
		break;
	case 112:
		sprintf(MED, "C07,LGC,");
		break;
	case 113:
		sprintf(MED, "C08,CMC,");
		break;
	case 114:
		sprintf(MED, "C08,LGC,");
		break;
	default:
		return;
	}

	char FullMED[128];
	sprintf_s(FullMED, "%s%s;", MED, dt);
	GeneralMEDRequest(FullMED);
}

void ApolloRTCCMFD::menuAGCTimeUpdateUplink()
{
	switch (screen)
	{
	case 111:
		G->AGCClockIncrementUplink(true);
		break;
	case 112:
		G->AGCClockIncrementUplink(false);
		break;
	case 113:
		G->AGCLiftoffTimeIncrementUplink(true);
		break;
	case 114:
		G->AGCLiftoffTimeIncrementUplink(false);
		break;
	}
}

void ApolloRTCCMFD::menuUplinkDisplayRequest()
{
	bool UplinkDisplayRequestInput(void* id, char *str, void *data);
	oapiOpenInputBox("Select display by number:", UplinkDisplayRequestInput, 0, 20, (void*)this);
}

bool UplinkDisplayRequestInput(void* id, char *str, void *data)
{
	int num;
	if (sscanf(str, "%d", &num) == 1)
	{
		((ApolloRTCCMFD*)data)->SelectUplinkScreen(num);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::SelectUplinkScreen(int num)
{
	switch (num)
	{
	case 0: //CMC CSM State Vector
		subscreen = 0;
		screen = 48;
		break;
	case 6: //CMC Landing Site Vector
		screen = 50;
		break;
	case 7: //CMC Clock Increment
		screen = 111;
		break;
	case 8: //CMC Liftoff Time Update
		screen = 113;
		break;
	case 9: //CMC LM State Vector
		subscreen = 1;
		screen = 48;
		break;
	case 10: //CMC External DV Update
		screen = 51;
		break;
	case 12: //CMC REFSMMAT Update
		screen = 53;
		break;
	case 13: //CMC Retrofire External DV Update
		screen = 52;
		break;
	case 14: //CMC Entry Update
		screen = 119;
		break;
	case 18: //CMC Erasable Memory Update A
		screen = 24;
		subscreen = 0;
		break;
	case 19: //CMC Erasable Memory Update B
		screen = 24;
		subscreen = 1;
		break;
	case 20: //LGC LM State Vector
		subscreen = 3;
		screen = 48;
		break;
	case 21: //LGC CSM State Vector
		subscreen = 2;
		screen = 48;
		break;
	case 22: //LGC External DV Update
		screen = 102;
		break;
	case 23: //LGC REFSMMAT Update
		screen = 94;
		break;
	case 24: //LGC Clock Increment
		screen = 112;
		break;
	case 25: //LGC Liftoff Time Update
		screen = 114;
		break;
	case 26: //LGC Landing Site Vector
		screen = 98;
		break;
	case 28: //LGC Descent Update
		screen = 61;
		break;
	case 38: //LGC Erasable Memory Update A
		screen = 24;
		subscreen = 2;
		break;
	case 39: //LGC Erasable Memory Update B
		screen = 24;
		subscreen = 3;
		break;
	case 49: //LVDC Navigation Update
		screen = 96;
		break;
	default:
		return;
	}

	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuMSKRequest()
{
	bool MSKRequestInput(void* id, char *str, void *data);
	oapiOpenInputBox("Select display by number:", MSKRequestInput, 0, 20, (void*)this);
}

bool MSKRequestInput(void* id, char *str, void *data)
{
	int num;
	if (sscanf(str, "%d", &num) == 1)
	{
		((ApolloRTCCMFD*)data)->SelectMCCScreen(num);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::SelectMCCScreen(int num)
{
	switch (num)
	{
	case 1:
		menuSetMCCDisplaysPage();
		break;
	case 40:
		menuSetFIDOLaunchAnalogNo1Page();
		break;
	case 41:
		menuSetFIDOLaunchAnalogNo2Page();
		break;
	case 43:
		//FDO LAUNCH DIG NO 1
		break;
	case 45:
		menuSetFIDOOrbitDigitalsLMPage();
		break;
	case 46:
		menuSetFIDOOrbitDigitalsCSMPage();
		break;
	case 47:
		menuSetMPTPage();
		break;
	case 48:
		menuSetOrbAdjPage();
		break;
	case 54:
		menuSetDetailedManeuverTableNo1Page();
		break;
	case 55:
		menuSetPredSiteAcquisitionCSM1Page();
		break;
	case 56:
		menuSetPredSiteAcquisitionLM1Page();
		break;
	case 58:
		menuSetRendezvousEvaluationDisplayPage();
		break;
	case 60:
		menuSetRelativeMotionDigitalsPage();
		break;
	case 66:
		menuSetRendezvousPlanningDisplayPage();
		break;
	case 69:
		menuSetDetailedManeuverTableNo2Page();
		break;
	case 79:
		menuMidcourseTradeoffPage();
		break;
	case 82:
		menuSetSpaceDigitalsPage();
		break;
	case 86:
		menuSetDescPlanCalcPage();
		break;
	case 87:
		menuSetPredSiteAcquisitionCSM2Page();
		break;
	case 88:
		menuSetPredSiteAcquisitionLM2Page();
		break;
	case 229:
		menuSetGuidanceOpticsSupportTablePage();
		break;
	case 232:
		SetMEDInputPage("K19");
		break;
	case 233:
		SetMEDInputPage("K39");
		break;
	case 239:
		menuSetLMOpticsSupportTablePage();
		break;
	case 1501:
		menuSetMoonriseMoonsetTablePage();
		break;
	case 1502:
		menuSetSunriseSunsetTablePage();
		break;
	case 1503:
		menuSetNextStationContactsPage();
		break;
	case 1506:
		menuSetExpSiteAcqPage();
		break;
	case 1508:
		menuSetLandmarkAcquisitionDisplayPage();
		break;
	case 1590:
		menuSetVectorCompareDisplay();
		break;
	case 1591:
		menuVectorPanelSummaryPage();
		break;
	case 1597:
		menuSetSkeletonFlightPlanPage();
		break;
	case 1619:
		menuSetCheckoutMonitorPage();
		break;
	case 1629:
		menuSetOnlineMonitorPage();
		break;
	}
}

void ApolloRTCCMFD::GMPManeuverTypeName(char *buffer, int typ)
{
	switch (typ)
	{
	case 0:
		sprintf(buffer, "Plane Change");
		break;
	case 1:
		sprintf(buffer, "Circularization");
		break;
	case 2:
		sprintf(buffer, "Height Change");
		break;
	case 3:
		sprintf(buffer, "Node Shift");
		break;
	case 4:
		sprintf(buffer, "Apogee and Perigee Change");
		break;
	case 5:
		sprintf(buffer, "Input Maneuver");
		break;
	case 6:
		sprintf(buffer, "Apo/Peri Change + Node Shift");
		break;
	case 7:
		sprintf(buffer, "Shift Line-of-Apsides");
		break;
	case 8:
		sprintf(buffer, "Height + Plane Change");
		break;
	case 9:
		sprintf(buffer, "Circularization + Plane Change");
		break;
	case 10:
		sprintf(buffer, "Circularization + Node Shift");
		break;
	case 11:
		sprintf(buffer, "Height + Node Shift");
		break;
	case 12:
		sprintf(buffer, "Apo/Peri Change + Apsides Shift");
		break;
	default:
		sprintf(buffer, "");
		break;
	}
}

void ApolloRTCCMFD::GMPManeuverPointName(char *buffer, int point)
{
	switch (point)
	{
	case 0:
		sprintf(buffer, "Apoapsis");
		break;
	case 1:
		sprintf(buffer, "Equatorial crossing");
		break;
	case 2:
		sprintf(buffer, "Perigee");
		break;
	case 3:
		sprintf(buffer, "Longitude");
		break;
	case 4:
		sprintf(buffer, "Height");
		break;
	case 5:
		sprintf(buffer, "Time");
		break;
	case 6:
		sprintf(buffer, "Optimum");
		break;
	default:
		sprintf(buffer, "");
		break;
	}
}

void ApolloRTCCMFD::GMPManeuverCodeName(char *buffer, int code)
{
	switch (code)
	{
	case 0:
		sprintf(buffer, "No Valid Code");
		break;
	case RTCC_GMP_PCE:
		sprintf(buffer, "PCE");
		//sprintf(buffer, "PCE: Plane change at equatorial crossing (1)");
		break;
	case RTCC_GMP_PCL:
		sprintf(buffer, "PCL");
		//sprintf(buffer, "PCL: Plane change at specified longitude (2)");
		break;
	case RTCC_GMP_PCT:
		sprintf(buffer, "PCT");
		//sprintf(buffer, "PCT: Plane change at specified time (3)");
		break;
	case RTCC_GMP_CRL:
		sprintf(buffer, "CRL");
		//sprintf(buffer, "CRL: Circularization at specified longitude (4)");
		break;
	case RTCC_GMP_CRH:
		sprintf(buffer, "CRH");
		//sprintf(buffer, "CRH: Circularization at specified height (5)");
		break;
	case RTCC_GMP_HOL:
		sprintf(buffer, "HOL");
		//sprintf(buffer, "HOL: Height maneuver at specified longitude (6)");
		break;
	case RTCC_GMP_HOT:
		sprintf(buffer, "HOT");
		break;
	case RTCC_GMP_HAO:
		sprintf(buffer, "HAO");
		break;
	case RTCC_GMP_HPO:
		sprintf(buffer, "HPO");
		break;
	case RTCC_GMP_NST:
		sprintf(buffer, "NST");
		break;
	case RTCC_GMP_NSO:
		sprintf(buffer, "NSO");
		break;
	case RTCC_GMP_HBT:
		sprintf(buffer, "HBT");
		break;
	case RTCC_GMP_HBH:
		sprintf(buffer, "HBH");
		break;
	case RTCC_GMP_HBO:
		sprintf(buffer, "HBO");
		break;
	case RTCC_GMP_FCT:
		sprintf(buffer, "FCT");
		break;
	case RTCC_GMP_FCL:
		sprintf(buffer, "FCL");
		break;
	case RTCC_GMP_FCH:
		sprintf(buffer, "FCH");
		break;
	case RTCC_GMP_FCA:
		sprintf(buffer, "FCA");
		break;
	case RTCC_GMP_FCP:
		sprintf(buffer, "FCP");
		break;
	case RTCC_GMP_FCE:
		sprintf(buffer, "FCE");
		break;
	case RTCC_GMP_NHT:
		sprintf(buffer, "NHT");
		break;
	case RTCC_GMP_NHL:
		sprintf(buffer, "NHL");
		break;
	case RTCC_GMP_SAL:
		sprintf(buffer, "SAL");
		break;
	case RTCC_GMP_SAA:
		sprintf(buffer, "SAA");
		break;
	case RTCC_GMP_PHL:
		sprintf(buffer, "PHL");
		break;
	case RTCC_GMP_PHT:
		sprintf(buffer, "PHT");
		break;
	case RTCC_GMP_PHA:
		sprintf(buffer, "PHA");
		break;
	case RTCC_GMP_PHP:
		sprintf(buffer, "PHP");
		break;
	case RTCC_GMP_CPL:
		sprintf(buffer, "CPL");
		break;
	case RTCC_GMP_CPH:
		sprintf(buffer, "CPH");
		break;
	case RTCC_GMP_SAT:
		sprintf(buffer, "SAT");
		break;
	case RTCC_GMP_SAO:
		sprintf(buffer, "SAO");
		break;
	case RTCC_GMP_HBL:
		sprintf(buffer, "HBL");
		break;
	case RTCC_GMP_CNL:
		sprintf(buffer, "CNL");
		break;
	case RTCC_GMP_CNH:
		sprintf(buffer, "CNH");
		break;
	case RTCC_GMP_HNL:
		sprintf(buffer, "HNL");
		break;
	case RTCC_GMP_HNT:
		sprintf(buffer, "HNT");
		break;
	case RTCC_GMP_HNA:
		sprintf(buffer, "HNA");
		break;
	case RTCC_GMP_HNP:
		sprintf(buffer, "HNP");
		break;
	case RTCC_GMP_CRT:
		sprintf(buffer, "CRT");
		break;
	case RTCC_GMP_CRA:
		sprintf(buffer, "CRA");
		break;
	case RTCC_GMP_CRP:
		sprintf(buffer, "CRP");
		break;
	case RTCC_GMP_CPT:
		sprintf(buffer, "CPT");
		break;
	case RTCC_GMP_CPA:
		sprintf(buffer, "CPA");
		break;
	case RTCC_GMP_CPP:
		sprintf(buffer, "CPP");
		break;
	case RTCC_GMP_CNT:
		sprintf(buffer, "CNT");
		break;
	case RTCC_GMP_CNA:
		sprintf(buffer, "CNA");
		break;
	case RTCC_GMP_CNP:
		sprintf(buffer, "CNP");
		break;
	case RTCC_GMP_PCH:
		sprintf(buffer, "PCH");
		break;
	case RTCC_GMP_NSH:
		sprintf(buffer, "NSH");
		break;
	case RTCC_GMP_NSL:
		sprintf(buffer, "NSL");
		break;
	case RTCC_GMP_HOH:
		sprintf(buffer, "HOH");
		break;
	case RTCC_GMP_HAS:
		sprintf(buffer, "HAS");
		break;
	default:
		sprintf(buffer, "No Valid Code");
		break;
	}
}

void ApolloRTCCMFD::CycleCSMOrLMSelection()
{
	IsCSM = !IsCSM;
}

void ApolloRTCCMFD::CycleEnableCalculation()
{
	EnableCalculation = !EnableCalculation;
}

void ApolloRTCCMFD::SetMEDInputPageM75()
{
	SetMEDInputPage("M75");
}

void ApolloRTCCMFD::SetMEDInputPageP13()
{
	SetMEDInputPage("P13");
}

void ApolloRTCCMFD::SetMEDInputPageP14()
{
	SetMEDInputPage("P14");
}

void AddMEDInputTitle(MEDInputPage &MEDInputData, std::string MEDCode, std::string Title)
{
	MEDInputData.MEDCode = MEDCode;
	MEDInputData.Title = MEDCode + ": " + Title;
}

void AddMEDInput(std::vector<MEDInput> &table, std::string Label, std::string Description, std::string Data, std::string Unit)
{
	MEDInput temp;

	temp.Label = Label;
	temp.Description = Description;
	temp.Data = Data;
	temp.Unit = Unit;
	table.push_back(temp);
}

void ApolloRTCCMFD::SetMEDInputPage(std::string med)
{
	MEDInput temp;
	char Buff[128];

	MEDInputData.table.clear();

	if (med == "K19")
	{
		AddMEDInputTitle(MEDInputData, "K19", "Initialization for Ascent Rendezvous Monitor");

		sprintf(Buff, "%.1lf", GC->rtcc->PZMARM.WT*DEG);
		AddMEDInput(MEDInputData.table, "WT:", "Terminal phase travel angle (from TPI to TPF):", Buff, "degrees");
		sprintf(Buff, "%.1lf", GC->rtcc->PZMARM.E*DEG);
		AddMEDInput(MEDInputData.table, "E:", "Elevation Angle, chaser pitch to see target (0-90 degrees):", Buff, "degrees");
		sprintf(Buff, "%.1lf", GC->rtcc->PZMARM.CSIFlag);
		AddMEDInput(MEDInputData.table, "CSI:", "CSI Flag: If zero CSI performed at first apolune after Inser.; non-zero parm used as delta time from insertion to CSI", Buff, "minutes");
		sprintf(Buff, "%d", GC->rtcc->PZMARM.CDHIndicator);
		AddMEDInput(MEDInputData.table, "CDH:", "CDH Indicator: +N = N apsis crossings from CSI to CDH, -N = N/2 revs from CSI to CDH (N must be odd):", Buff, "");
		GMT_Display2(Buff, GC->rtcc->PZMARM.t_TPI_Coell);
		AddMEDInput(MEDInputData.table, "TPI:", "TPI time:", Buff, "GET");
		GMT_Display2(Buff, GC->rtcc->PZMARM.t_Ins);
		AddMEDInput(MEDInputData.table, "INS:", "Insertion time:", Buff, "GET");
		sprintf(Buff, "%.1lf", GC->rtcc->PZMARM.h_min / 1852.0);
		AddMEDInput(MEDInputData.table, "MIN:", "Minimum safe perilune", Buff, "NM");
		sprintf(Buff, "%.1lf", GC->rtcc->PZMARM.DH / 1852.0);
		AddMEDInput(MEDInputData.table, "DH:", "Desired Delta Height:", Buff, "NM");

		MEDInputData.display = 99;
	}
	else if (med == "K39")
	{
		AddMEDInputTitle(MEDInputData, "K39", "Initialization for Short ARM");

		if (GC->rtcc->PZMARM.ITWEAK)
		{
			GMT_Display2(Buff, GC->rtcc->PZMARM.t_tweak);
		}
		else
		{
			GET_Display2(Buff, -GC->rtcc->PZMARM.DT);
		}
		AddMEDInput(MEDInputData.table, "TW:", "Time of tweak (GET) or, if negative, time of tweak DT from insertion", Buff, "");

		if (GC->rtcc->PZMARM.ITPI)
		{
			GMT_Display2(Buff, GC->rtcc->PZMARM.t_TPI_Short);
		}
		else
		{
			GET_Display2(Buff, -GC->rtcc->PZMARM.DTPI);
		}
		AddMEDInput(MEDInputData.table, "TPI:", "Time of TPI (GET) or, if negative, time from insertion to TPI", Buff, "");

		sprintf(Buff, "%.2lf", GC->rtcc->PZMARM.DTHETA*DEG);
		AddMEDInput(MEDInputData.table, "DPH:", "Phase offset at TPI:", Buff, "degrees");
		sprintf(Buff, "%.1lf", GC->rtcc->PZMARM.DH / 1852.0);
		AddMEDInput(MEDInputData.table, "DH:", "Height offset at TPI:", Buff, "NM");
		sprintf(Buff, "%.1lf", GC->rtcc->PZMARM.WT*DEG);
		AddMEDInput(MEDInputData.table, "WT:", "Terminal phase travel angle (from TPI to TPF):", Buff, "degrees");
		GMT_Display2(Buff, GC->rtcc->PZMARM.t_Ins);
		AddMEDInput(MEDInputData.table, "INS:", "Insertion time:", Buff, "GET");
		sprintf(Buff, "%.0lf", GC->rtcc->PZMARM.IMUAngles.z*DEG);
		AddMEDInput(MEDInputData.table, "IMU R (M):", "IMU roll gimbal angle:", Buff, "degrees");
		sprintf(Buff, "%.0lf", GC->rtcc->PZMARM.IMUAngles.y*DEG);
		AddMEDInput(MEDInputData.table, "IMU P (I):", "IMU pitch gimbal angle:", Buff, "degrees");
		sprintf(Buff, "%.0lf", GC->rtcc->PZMARM.IMUAngles.x*DEG);
		AddMEDInput(MEDInputData.table, "IMU Y (O):", "IMU yaw gimbal angle:", Buff, "degrees");

		MEDInputData.display = 100;
	}
	else if (med == "M75")
	{
		AddMEDInputTitle(MEDInputData, "M75", "Transfer of TLI maneuver from study aid");

		AddMEDInput(MEDInputData.table, "VEH:", "Vehicle (CSM or LEM):", "CSM", "");
		AddMEDInput(MEDInputData.table, "REP:", "Replace code (1-15 or 0 if no replacement):", "0", "");
	}
	else if (med == "P13")
	{
		AddMEDInputTitle(MEDInputData, "P13", "Enter Vector in Spherical Coordinates");

		AddMEDInput(MEDInputData.table, "VEH:", "Vehicle (CSM or LEM):", "CSM", "");
		AddMEDInput(MEDInputData.table, "Velocity:", "Velocity in ft/s:", "0.0", "ft/s");
		AddMEDInput(MEDInputData.table, "Flight Path Angle:", "Flight Path Angle in degrees:", "0.0", "degrees");
		AddMEDInput(MEDInputData.table, "Azimuth:", "Azimuth in degrees:", "0.0", "degrees");
		AddMEDInput(MEDInputData.table, "Latitude:", "Geocentric latitude in degrees:", "0.0", "degrees");
		AddMEDInput(MEDInputData.table, "Longitude:", "Longitude in degrees:", "0.0", "degrees");
		AddMEDInput(MEDInputData.table, "Height:", "Height above spherical planet in nautical miles:", "0.0", "NM");
		AddMEDInput(MEDInputData.table, "Time:", "State vector time (format HHH:MM:SS.TH):", "000:00:00.00", "");
		AddMEDInput(MEDInputData.table, "Vector Action Code:", "Vector Action Code (L = Live Ephem, S = Static Ephem, G = put in targeting slot of GZLTRA only, B = put in GZLTRA & generate static ephem):", "L", "");
		AddMEDInput(MEDInputData.table, "Co-ord System Ind:", "Coordinate System Indicator (ECT or MCT):", "ECT", "");
	}
	else if (med == "P14")
	{
		AddMEDInputTitle(MEDInputData, "P14", "Initialize Trajectory with a Vector");

		AddMEDInput(MEDInputData.table, "VEH:", "Vehicle (CSM or LEM):", "CSM", "");
		AddMEDInput(MEDInputData.table, "X:", "Position in Earth radii:", "0.0", "ER");
		AddMEDInput(MEDInputData.table, "Y:", "Position in Earth radii:", "0.0", "ER");
		AddMEDInput(MEDInputData.table, "Z:", "Position in Earth radii:", "0.0", "ER");
		AddMEDInput(MEDInputData.table, "XDOT:", "Velocity in Earth radii per hour:", "0.0", "ER/hr");
		AddMEDInput(MEDInputData.table, "YDOT:", "Velocity in Earth radii per hour:", "0.0", "ER/hr");
		AddMEDInput(MEDInputData.table, "ZDOT:", "Velocity in Earth radii per hour:", "0.0", "ER/hr");
		AddMEDInput(MEDInputData.table, "Time:", "State vector time (format HHH:MM:SS.TH):", "000:00:00.00", "");
		AddMEDInput(MEDInputData.table, "Vector Action Code:", "Vector Action Code (L = Live Ephem, S = Static Ephem, G = put in targeting slot of GZLTRA only, B = put in GZLTRA & generate static ephem):", "L", "");
		AddMEDInput(MEDInputData.table, "Co-ord System Ind:", "Coordinate system of vector (ECI, ECT, MCI, MCT, EMP or PLUM)", "ECI", "");
	}
	else if (med == "S84")
	{
		AddMEDInputTitle(MEDInputData, "S84", "Vector Panel Summary Entry");

		AddMEDInput(MEDInputData.table, "VEH:", "Vehicle (CSM or LEM):", "CSM", "");
		AddMEDInput(MEDInputData.table, "X:", "Position in Earth radii:", "0.0", "ER");
		AddMEDInput(MEDInputData.table, "Y:", "Position in Earth radii:", "0.0", "ER");
		AddMEDInput(MEDInputData.table, "Z:", "Position in Earth radii:", "0.0", "ER");
		AddMEDInput(MEDInputData.table, "XDOT:", "Velocity in Earth radii per hour:", "0.0", "ER/hr");
		AddMEDInput(MEDInputData.table, "YDOT:", "Velocity in Earth radii per hour:", "0.0", "ER/hr");
		AddMEDInput(MEDInputData.table, "ZDOT:", "Velocity in Earth radii per hour:", "0.0", "ER/hr");
		AddMEDInput(MEDInputData.table, "GMT:", "GMT of vector (format HHH:MM:SS.TH):", "000:00:00.00", "GMT");
		AddMEDInput(MEDInputData.table, "Reference:", "Coordinate system of vector (ECI, ECT, MCI, MCT or EMP)", "ECI", "");
		AddMEDInput(MEDInputData.table, "Lunar Surf.:", "Lunar Surface Indicator (S if vector is on the surface of Moon):", "", "");
	}

	marker = 0;
	markermax = (int)MEDInputData.table.size();
	if (markermax > 0) markermax--;
	SelectPage(130);
}

void ApolloRTCCMFD::menuMEDInputCalc()
{
	if (MEDInputData.table.size() == 0U) return;

	std::string med;

	med = MEDInputData.MEDCode;

	for (unsigned i = 0; i < MEDInputData.table.size(); i++)
	{
		med += ",";
		med += MEDInputData.table[i].Data;
	}

	med += ";";

	sprintf_s(GC->rtcc->RTCCMEDBUFFER, 256, med.c_str());
	G->GeneralMEDRequest();
}

void ApolloRTCCMFD::menuInputMEDData()
{
	if (MEDInputData.table.size() <= (unsigned)marker) return;

	MEDInput data = MEDInputData.table[(unsigned)marker];

	char Buffer[256];
	sprintf(Buffer, "%s", data.Description.c_str());

	bool InputMEDDataInput(void* id, char *str, void *data);
	oapiOpenInputBox(Buffer, InputMEDDataInput, 0, 20, (void*)this);
}

bool InputMEDDataInput(void* id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->set_MEDData(str);
	return true;
}

void ApolloRTCCMFD::set_MEDData(char *str)
{
	MEDInput *data = &MEDInputData.table[(unsigned)marker];

	data->Data.assign(str);
}

void ApolloRTCCMFD::menuGenericGoToDisplay()
{
	if (MEDInputData.table.size() == 0U) return;

	if (MEDInputData.display >= 0)
	{
		SelectPage(MEDInputData.display);
	}
}

void ApolloRTCCMFD::menuReturnToMEDInput()
{
	if (MEDInputData.table.size() == 0U)
	{
		menuSetMenu();
	}
	else
	{
		SetMEDInputPage(MEDInputData.MEDCode);
	}
}

void ApolloRTCCMFD::DFLBackgroundSlide(oapi::Sketchpad *skp, unsigned display)
{
	skp->SetFont(font2);
	GC->DFLBackgroundSlide(skp, W, H, display);
}

void ApolloRTCCMFD::DFLDynamicData(oapi::Sketchpad *skp, unsigned display)
{
	skp->SetFont(font2);
	GC->rtcc->DynamicDisplayData.Print(skp, W, H, display);
}