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
#define ORBITER_MODULE

#include "ApolloRTCCMFD.h"
#include "papi.h"

// ==============================================================
// Global variables

int g_MFDmode; // identifier for new MFD mode
ARCore *GCoreData[32];
OBJHANDLE GCoreVessel[32];
int nGutsUsed;
char Buffer[100];
bool initialised = false;

// ==============================================================
// API interface

DLLCLBK void InitModule (HINSTANCE hDLL)
{
	static char *name = "Apollo RTCC MFD";   // MFD mode name
	MFDMODESPECEX spec;
	spec.name = name;
	spec.key = OAPI_KEY_T;                // MFD mode selection key
	spec.context = NULL;
	spec.msgproc = ApolloRTCCMFD::MsgProc;  // MFD mode callback function

	// Register the new MFD mode with Orbiter
	g_MFDmode = oapiRegisterMFDMode (spec);
	nGutsUsed = 0;
}

DLLCLBK void ExitModule (HINSTANCE hDLL)
{
	// Unregister the custom MFD mode when the module is unloaded
	oapiUnregisterMFDMode (g_MFDmode);
}

// ==============================================================
// MFD class implementation

// Constructor
ApolloRTCCMFD::ApolloRTCCMFD (DWORD w, DWORD h, VESSEL *vessel)
: MFD2 (w, h, vessel)
{
	//font = oapiCreateFont(w / 20, true, "Arial", FONT_NORMAL, 0);
	font = oapiCreateFont(w / 20, true, "Courier", FONT_NORMAL, 0);
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
		GCoreData[nGutsUsed] = new ARCore(vessel);
		screen = 0;
		G = GCoreData[nGutsUsed];
		GCoreVessel[nGutsUsed] = vessel;
		nGutsUsed++;
	}
}

// Destructor
ApolloRTCCMFD::~ApolloRTCCMFD ()
{
	oapiReleaseFont (font);
	// Add MFD cleanup code here
}

DLLCLBK void opcPreStep(double SimT, double SimDT, double mjd) {
	//if (initialised) {
	for (int i = 0; i < nGutsUsed; i++)
	{
		GCoreData[i]->MinorCycle(SimT, SimDT, mjd);
	}
	//}
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
	char Buffer2[100];

	//oapiWriteScenario_int(scn, "SCREEN", G->screen);
	oapiWriteScenario_int(scn, "VESSELTYPE", G->vesseltype);
	papiWriteScenario_mx(scn, "REFSMMAT", G->REFSMMAT);
	papiWriteScenario_intarr(scn, "REFSMMAToct", G->REFSMMAToct, 20);
	oapiWriteScenario_int(scn, "REFSMMATcur", G->REFSMMATcur);
	oapiWriteScenario_int(scn, "REFSMMATopt", G->REFSMMATopt);
	papiWriteScenario_double(scn, "REFSMMATTime", G->REFSMMATTime);
	oapiWriteScenario_int(scn, "REFSMMATupl", G->REFSMMATupl);
	papiWriteScenario_bool(scn, "REFSMMATdirect", G->REFSMMATdirect);
	papiWriteScenario_double(scn, "T1", G->T1);
	papiWriteScenario_double(scn, "T2", G->T2);
	papiWriteScenario_double(scn, "CDHTIME", G->CDHtime);
	papiWriteScenario_double(scn, "CDHTIMEcor", G->CDHtime_cor);
	oapiWriteScenario_int(scn, "CDHTIMEMODE", G->CDHtimemode);
	papiWriteScenario_double(scn, "DH", G->DH);
	oapiWriteScenario_int(scn, "N", G->N);
	papiWriteScenario_vec(scn, "LambertdeltaV", G->LambertdeltaV);
	oapiWriteScenario_int(scn, "LAMBERTOPT", G->lambertopt);
	papiWriteScenario_bool(scn, "LAMBERTMULTI", G->lambertmultiaxis);
	papiWriteScenario_vec(scn, "CDHdeltaV", G->CDHdeltaV);
	if (G->target != NULL)
	{
		sprintf(Buffer2, G->target->GetName());
		oapiWriteScenario_string(scn, "TARGET", Buffer2);
	}
	oapiWriteScenario_int(scn, "TARGETNUMBER", G->targetnumber);
	papiWriteScenario_vec(scn, "OFFVEC", G->offvec);
	papiWriteScenario_double(scn, "ANGDEG", G->angdeg);
	oapiWriteScenario_int(scn, "MISSION", G->mission);
	papiWriteScenario_double(scn, "GETBASE", G->GETbase);
	papiWriteScenario_double(scn, "LSLat", G->LSLat);
	papiWriteScenario_double(scn, "LSLng", G->LSLng);
	papiWriteScenario_double(scn, "P30TIG", G->P30TIG);
	papiWriteScenario_vec(scn, "DV_LVLH", G->dV_LVLH);
	papiWriteScenario_double(scn, "ENTRYTIG", G->EntryTIG);
	papiWriteScenario_double(scn, "ENTRYLAT", G->EntryLat);
	papiWriteScenario_double(scn, "ENTRYLNG", G->EntryLng);
	papiWriteScenario_double(scn, "ENTRYTIGCOR", G->EntryTIGcor);
	papiWriteScenario_double(scn, "ENTRYLATCOR", G->EntryLatcor);
	papiWriteScenario_double(scn, "ENTRYLNGCOR", G->EntryLngcor);
	papiWriteScenario_double(scn, "ENTRYANG", G->EntryAng);
	papiWriteScenario_double(scn, "ENTRYANGCOR", G->EntryAngcor);
	papiWriteScenario_vec(scn, "ENTRYDV", G->Entry_DV);
	oapiWriteScenario_int(scn, "ENTRYCALCMODE", G->entrycalcmode);
	oapiWriteScenario_int(scn, "ENTRYCRITICAL", G->entrycritical);
	papiWriteScenario_double(scn, "ENTRYRANGE", G->entryrange);
	papiWriteScenario_bool(scn, "ENTRYNOMINAL", G->entrynominal);
	papiWriteScenario_bool(scn, "ENTRYLONGMANUAL", G->entrylongmanual);
	oapiWriteScenario_int(scn, "LANDINGZONE", G->landingzone);
	oapiWriteScenario_int(scn, "ENTRYPRECISION", G->entryprecision);

	oapiGetObjectName(G->maneuverplanet, Buffer2, 20);
	oapiWriteScenario_string(scn, "MANPLAN", Buffer2);
	papiWriteScenario_double(scn, "P37GET400K", G->P37GET400K);
	oapiWriteScenario_int(scn, "MAPPAGE", G->mappage);
	papiWriteScenario_bool(scn, "INHIBITUPLINK", G->inhibUplLOS);
	papiWriteScenario_double(scn, "APODESNM", G->apo_desnm);
	papiWriteScenario_double(scn, "PERIDESNM", G->peri_desnm);
	papiWriteScenario_double(scn, "INCDEG", G->incdeg);
	papiWriteScenario_double(scn, "SPSGET", G->SPSGET);
	papiWriteScenario_vec(scn, "OrbAdjDVX", G->OrbAdjDVX);

	oapiWriteScenario_int(scn, "LOIMANEUVER", G->LOImaneuver);
	papiWriteScenario_double(scn, "LOIGET", G->LOIGET);
	papiWriteScenario_double(scn, "LOIPeriGET", G->LOIPeriGET);
	papiWriteScenario_double(scn, "LOILat", G->LOILat);
	papiWriteScenario_double(scn, "LOILng", G->LOILng);
	papiWriteScenario_double(scn, "LOIapo", G->LOIapo);
	papiWriteScenario_double(scn, "LOIperi", G->LOIperi);
	papiWriteScenario_double(scn, "LOIinc", G->LOIinc);
	papiWriteScenario_vec(scn, "TLCCDV", G->TLCC_dV_LVLH);
	papiWriteScenario_vec(scn, "LOIDV", G->LOI_dV_LVLH);
	papiWriteScenario_double(scn, "TLCCTIG", G->TLCC_TIG);
	papiWriteScenario_double(scn, "LOITIG", G->LOI_TIG);
	papiWriteScenario_vec(scn, "R_TLI", G->R_TLI);
	papiWriteScenario_vec(scn, "V_TLI", G->V_TLI);
}

void ApolloRTCCMFD::ReadStatus(FILEHANDLE scn)
{
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		char Buffer2[100];
		bool istarget;

		//papiReadScenario_int(line, "SCREEN", G->screen);
		papiReadScenario_int(line, "VESSELTYPE", G->vesseltype);
		papiReadScenario_mat(line, "REFSMMAT", G->REFSMMAT);
		papiReadScenario_intarr(line, "REFSMMAToct", G->REFSMMAToct, 20);
		papiReadScenario_int(line, "REFSMMATcur", G->REFSMMATcur);
		papiReadScenario_int(line, "REFSMMATopt", G->REFSMMATopt);
		papiReadScenario_double(line, "REFSMMATTime", G->REFSMMATTime);
		papiReadScenario_int(line, "REFSMMATupl", G->REFSMMATupl);
		papiReadScenario_bool(line, "REFSMMATdirect", G->REFSMMATdirect);
		papiReadScenario_double(line, "T1", G->T1);
		papiReadScenario_double(line, "T2", G->T2);
		papiReadScenario_double(line, "CDHTIME", G->CDHtime);
		papiReadScenario_double(line, "CDHTIMEcor", G->CDHtime_cor);
		papiReadScenario_int(line, "CDHTIMEMODE", G->CDHtimemode);
		papiReadScenario_double(line, "DH", G->DH);
		papiReadScenario_int(line, "N", G->N);
		papiReadScenario_vec(line, "LambertdeltaV", G->LambertdeltaV);
		papiReadScenario_int(line, "LAMBERTOPT", G->lambertopt);
		papiReadScenario_bool(line, "LAMBERTMULTI", G->lambertmultiaxis);
		papiReadScenario_vec(line, "CDHdeltaV", G->CDHdeltaV);

		istarget = papiReadScenario_string(line, "TARGET", Buffer2);
		if (istarget)
		{
			G->target = oapiGetVesselInterface(oapiGetObjectByName(Buffer2));
		}

		papiReadScenario_int(line, "TARGETNUMBER", G->targetnumber);
		papiReadScenario_vec(line, "OFFVEC", G->offvec);
		papiReadScenario_double(line, "ANGDEG", G->angdeg);
		papiReadScenario_int(line, "MISSION", G->mission);
		papiReadScenario_double(line, "GETBASE", G->GETbase);
		papiReadScenario_double(line, "LSLat", G->LSLat);
		papiReadScenario_double(line, "LSLng", G->LSLng);
		papiReadScenario_double(line, "P30TIG", G->P30TIG);
		papiReadScenario_vec(line, "DV_LVLH", G->dV_LVLH);
		papiReadScenario_double(line, "ENTRYTIG", G->EntryTIG);
		papiReadScenario_double(line, "ENTRYLAT", G->EntryLat);
		papiReadScenario_double(line, "ENTRYLNG", G->EntryLng);
		papiReadScenario_double(line, "ENTRYTIGCOR", G->EntryTIGcor);
		papiReadScenario_double(line, "ENTRYLATCOR", G->EntryLatcor);
		papiReadScenario_double(line, "ENTRYLNGCOR", G->EntryLngcor);
		papiReadScenario_double(line, "ENTRYANG", G->EntryAng);
		papiReadScenario_double(line, "ENTRYANGCOR", G->EntryAngcor);
		papiReadScenario_vec(line, "ENTRYDV", G->Entry_DV);
		papiReadScenario_int(line, "ENTRYCALCMODE", G->entrycalcmode);
		papiReadScenario_int(line, "ENTRYCRITICAL", G->entrycritical);
		papiReadScenario_double(line, "ENTRYRANGE", G->entryrange);
		papiReadScenario_bool(line, "ENTRYNOMINAL", G->entrynominal);
		papiReadScenario_bool(line, "ENTRYLONGMANUAL", G->entrylongmanual);
		papiReadScenario_int(line, "LANDINGZONE", G->landingzone);
		papiReadScenario_int(line, "ENTRYPRECISION", G->entryprecision);

		papiReadScenario_string(line, "MANPLAN", Buffer2);
		G->maneuverplanet = oapiGetObjectByName(Buffer2);

		papiReadScenario_double(line, "P37GET400K", G->P37GET400K);
		papiReadScenario_int(line, "MAPPAGE", G->mappage);
		papiReadScenario_bool(line, "INHIBITUPLINK", G->inhibUplLOS);
		papiReadScenario_double(line, "APODESNM", G->apo_desnm);
		papiReadScenario_double(line, "PERIDESNM", G->peri_desnm);
		papiReadScenario_double(line, "INCDEG", G->incdeg);
		papiReadScenario_double(line, "SPSGET", G->SPSGET);
		papiReadScenario_vec(line, "OrbAdjDVX", G->OrbAdjDVX);

		papiReadScenario_int(line, "LOIMANEUVER", G->LOImaneuver);
		papiReadScenario_double(line, "LOIGET", G->LOIGET);
		papiReadScenario_double(line, "LOIPeriGET", G->LOIPeriGET);
		papiReadScenario_double(line, "LOILat", G->LOILat);
		papiReadScenario_double(line, "LOILng", G->LOILng);
		papiReadScenario_double(line, "LOIapo", G->LOIapo);
		papiReadScenario_double(line, "LOIperi", G->LOIperi);
		papiReadScenario_double(line, "LOIinc", G->LOIinc);
		papiReadScenario_vec(line, "TLCCDV", G->TLCC_dV_LVLH);
		papiReadScenario_vec(line, "LOIDV", G->LOI_dV_LVLH);
		papiReadScenario_double(line, "TLCCTIG", G->TLCC_TIG);
		papiReadScenario_double(line, "LOITIG", G->LOI_TIG);
		papiReadScenario_vec(line, "R_TLI", G->R_TLI);
		papiReadScenario_vec(line, "V_TLI", G->V_TLI);

		//G->coreButtons.SelectPage(this, G->screen);
	}
}

// Repaint the MFD
bool ApolloRTCCMFD::Update (oapi::Sketchpad *skp)
{
	Title (skp, "Apollo RTCC MFD");
	skp->SetFont(font);

	//VECTOR3 R_D, V_D,R_LSA, DV_DOI;
	//double t_D,SVMJD,t_E,h_DP,theta_F,t_F,mu,t_DOI, t_PDI, t_L,CR,lat,lng;

	/*G->vessel->GetRelativePos(G->gravref, R_D);
	G->vessel->GetRelativeVel(G->gravref, V_D);
	SVMJD = oapiGetSimMJD();
	t_D = (SVMJD - G->GETbase)*24.0*3600.0;
	t_E = 101.0*3600.0;
	lat = 0.713888889*RAD;
	lng = 23.707777778*RAD;
	R_LSA = _V(cos(lng)*cos(lat),sin(lat),sin(lng)*cos(lat))*oapiGetSize(G->gravref);
	h_DP = 50000.0*0.3048;
	theta_F = 15.0*RAD;
	t_F = 718.0;
	mu = GGRAV*oapiGetMass(G->gravref);
	G->mech->LunarLandingPrediction(R_D, V_D, t_D, t_E, R_LSA, h_DP, theta_F, t_F, G->gravref, G->GETbase, mu, t_DOI, t_PDI, t_L, DV_DOI, CR);
	*/
	// Draws the MFD title

	// Add MFD display routines here.
	// Use the device context (hDC) for Windows GDI paint functions.

	//sprintf(Buffer, "%d", G->screen);
	//skp->Text(7.5 * W / 8,(int)(0.5 * H / 14), Buffer, strlen(Buffer));

	if (screen == 0)
	{
		skp->Text(1 * W / 8, 2 * H / 14, "Lambert", 7);
		skp->Text(1 * W / 8, 4 * H / 14, "Coelliptic", 10);
		skp->Text(1 * W / 8, 6 * H / 14, "Orbit Adjustment", 16);
		skp->Text(1 * W / 8, 8 * H / 14, "REFSMMAT", 8);
		skp->Text(1 * W / 8, 10 * H / 14, "Entry", 5);
		skp->Text(1 * W / 8, 12 * H / 14, "Lunar Insertion", 15);

		skp->Text(5 * W / 8, 2 * H / 14, "State Vector", 12);
		skp->Text(5 * W / 8, 4 * H / 14, "Landmark Tracking", 17);
		skp->Text(5 * W / 8, 6 * H / 14, "Map Update", 10);
		skp->Text(5 * W / 8, 8 * H / 14, "Maneuver PAD", 12);
		skp->Text(5 * W / 8, 10 * H / 14, "Entry PAD", 9);
		skp->Text(5 * W / 8, 12 * H / 14, "Configuration", 13);
	}
	else if (screen == 1)
	{
		skp->Text(6 * W / 8, (int)(0.5 * H / 14), "Lambert", 7);

		skp->Text(1 * W / 8, 2 * H / 14, "GET", 3);

		GET_Display(Buffer, G->T1);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->T2);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%d", G->N);
		skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

		if (G->lambertmultiaxis)
		{
			skp->Text(1 * W / 8, 10 * H / 14, "Multi-Axis", 10);
		}
		else
		{
			skp->Text(1 * W / 8, 10 * H / 14, "X-Axis", 6);
		}

		skp->Text(5 * W / 8, 10 * H / 14, "DVX", 3);
		skp->Text(5 * W / 8, 11 * H / 14, "DVY", 3);
		skp->Text(5 * W / 8, 12 * H / 14, "DVZ", 3);

		AGC_Display(Buffer, G->LambertdeltaV.x / 0.3048);
		skp->Text(6 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->LambertdeltaV.y / 0.3048);
		skp->Text(6 * W / 8, 11 * H / 14, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->LambertdeltaV.z / 0.3048);
		skp->Text(6 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));

		/*if (G->orient == 0)
		{
			skp->Text(1 * W / 8, 12 * H / 14, "LVLH", 4);
		}
		else
		{
			skp->Text(1 * W / 8, 12 * H / 14, "P30", 3);
		}*/
		if (G->lambertopt == 0)
		{
			skp->Text(1 * W / 8, 12 * H / 14, "Spherical", 9);
		}
		else
		{
			skp->Text(1 * W / 8, 12 * H / 14, "Perturbed", 9);
		}

		if (G->target != NULL)
		{
			sprintf(Buffer, G->target->GetName());
			skp->Text(5 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		}

		sprintf(Buffer, "XOFF %f NM", G->offvec.x/1852.0);
		skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "YOFF %f NM", G->offvec.y/1852.0);
		skp->Text(5 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "ZOFF %f NM", G->offvec.z/1852.0);
		skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 2)
	{
		skp->Text(6 * W / 8, (int)(0.5 * H / 14), "Offset", 6);

		sprintf(Buffer, "%f °", G->angdeg);
		skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f NM", G->offvec.x/1852);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f NM", G->offvec.y / 1852);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f NM", G->offvec.z / 1852);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
	}

	else if (screen == 3)
	{
		skp->Text(6 * W / 8, (int)(0.5 * H / 14), "CDH", 3);

		if (G->CDHtimemode == 0)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Fixed", 5);
		}
		else if (G->CDHtimemode == 1)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Find GETI", 9);
		}

		GET_Display(Buffer, G->CDHtime);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f NM", G->DH);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		/*int hh,mm,ss;
		double mins, secs;

		hh = trunc(G->CDHtime_cor / 3600);//28
		mins = (G->CDHtime_cor/3600 - hh)*60;
		mm = trunc(mins);//0
		secs = (mins - mm) * 60; //31
		ss = trunc(secs);


		sprintf(Buffer, "%03d:%02d:%02d", hh, mm, ss);*/

		GET_Display(Buffer, G->CDHtime_cor);

		//sprintf(Buffer, "%f", CDHtime_cor);
		skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

		skp->Text(4 * W / 8, 6 * H / 14, "DX", 2);
		skp->Text(4 * W / 8, 8 * H / 14, "DY", 2);
		skp->Text(4 * W / 8, 10 * H / 14, "DZ", 2);

		AGC_Display(Buffer, G->CDHdeltaV.x / 0.3048);
		skp->Text(6 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->CDHdeltaV.y / 0.3048);
		skp->Text(6 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->CDHdeltaV.z / 0.3048);
		skp->Text(6 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		//skp->Text(1 * W / 8, 12 * H / 14, "P30", 3);

		if (G->target != NULL)
		{
			sprintf(Buffer, G->target->GetName());
			skp->Text(5 * W / 8, 2 * H / 12, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 4)
	{
		skp->Text(6 * W / 8,(int)(0.5 * H / 14), "Orbit", 5);

		GET_Display(Buffer, G->SPSGET);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%f NM", G->apo_desnm);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%f NM", G->peri_desnm);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%f °", G->incdeg);
		skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));


		skp->Text(5 * W / 8, 8 * H / 14, "DVX", 3);
		skp->Text(5 * W / 8, 9 * H / 14, "DVY", 3);
		skp->Text(5 * W / 8, 10 * H / 14, "DVZ", 3);
		skp->Text(5 * W / 8, 12 * H / 14, "DVT", 3);
		AGC_Display(Buffer, G->OrbAdjDVX.x / 0.3048);
		skp->Text(6 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->OrbAdjDVX.y / 0.3048);
		skp->Text(6 * W / 8, 9 * H / 14, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->OrbAdjDVX.z / 0.3048);
		skp->Text(6 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
		AGC_Display(Buffer, length(G->OrbAdjDVX) / 0.3048);
		skp->Text(6 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 5)
	{
		skp->Text(6 * W / 8,(int)(0.5 * H / 14), "REFSMMAT", 8);

		if (G->REFSMMATupl == 0)
		{
			skp->Text((int)(0.5 * W / 8), 4 * H / 14, "Desired REFSMMAT", 16);
		}
		else
		{
			skp->Text((int)(0.5 * W / 8), 4 * H / 14, "REFSMMAT", 8);
		}

		if (G->REFSMMATopt == 0) //P30 Maneuver
		{
			skp->Text(5 * W / 8, 2 * H / 14, "P30", 3);

			GET_Display(Buffer, G->P30TIG);
			skp->Text((int)(0.5 * W / 8), 2 * H / 14, Buffer, strlen(Buffer));
			
			skp->Text(5 * W / 8, 4 * H / 14, "DV Vector", 9);
			AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
			skp->Text(5 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
			skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
			skp->Text(5 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->REFSMMATopt == 1)//Retrofire
		{
			skp->Text(5 * W / 8, 2 * H / 14, "P30 Retro", 9);

			GET_Display(Buffer, G->P30TIG);
			skp->Text((int)(0.5 * W / 8), 2 * H / 14, Buffer, strlen(Buffer));
			
			skp->Text(5 * W / 8, 4 * H / 14, "DV Vector", 9);
			AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
			skp->Text(5 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
			skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
			skp->Text(5 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));

		}
		else if (G->REFSMMATopt == 2)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "LVLH", 4);

			GET_Display(Buffer, G->REFSMMATTime);
			skp->Text((int)(0.5 * W / 8), 2 * H / 14, Buffer, strlen(Buffer));

			if (G->REFSMMATdirect == true)
			{
				skp->Text((int)(0.5 * W / 8), 12 * H / 14, "Direct", 6);
			}
			else
			{
				skp->Text((int)(0.5 * W / 8), 12 * H / 14, "MCC", 3);
			}
		}
		else if (G->REFSMMATopt == 3)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "Lunar Entry", 11);

			if (G->REFSMMATdirect == true)
			{
				skp->Text((int)(0.5 * W / 8), 12 * H / 14, "Direct", 6);
			}
			else
			{
				skp->Text((int)(0.5 * W / 8), 12 * H / 14, "MCC", 3);
			}
		}
		else if (G->REFSMMATopt == 4)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "Launch", 6);

			if (G->mission == 0)
			{
				skp->Text((int)(0.5 * W / 8), 2 * H / 14, "Manual", 6);
			}
			else if (G->mission >= 7)
			{
				sprintf(Buffer, "Apollo %i", G->mission);
				skp->Text((int)(0.5 * W / 8), 2 * H / 14, Buffer, strlen(Buffer));
			}
		}
		else if (G->REFSMMATopt == 5)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "Landing Site", 12);

			GET_Display(Buffer, G->REFSMMATTime);
			skp->Text((int)(0.5 * W / 8), 2 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%f°", G->LSLat*DEG);
			skp->Text((int)(5.5 * W / 8), 8 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%f°", G->LSLng*DEG);
			skp->Text((int)(5.5 * W / 8), 10 * H / 14, Buffer, strlen(Buffer));

			if (G->REFSMMATdirect == true)
			{
				skp->Text((int)(0.5 * W / 8), 12 * H / 14, "Direct", 6);
			}
			else
			{
				skp->Text((int)(0.5 * W / 8), 12 * H / 14, "MCC", 3);
			}

		}
		else if (G->REFSMMATopt == 6)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "PTC", 3);

			GET_Display(Buffer, G->REFSMMATTime);
			skp->Text((int)(0.5 * W / 8), 2 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->REFSMMATopt == 7)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "LOI-2", 5);

			skp->Text(5 * W / 8, 6 * H / 21, "MCC", 3);
			GET_Display(Buffer, G->TLCC_TIG);
			skp->Text(5 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVX", G->TLCC_dV_LVLH.x / 0.3048);
			skp->Text(5 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVY", G->TLCC_dV_LVLH.y / 0.3048);
			skp->Text(5 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVZ", G->TLCC_dV_LVLH.z / 0.3048);
			skp->Text(5 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));

			skp->Text(5 * W / 8, 13 * H / 21, "LOI-1", 5);
			GET_Display(Buffer, G->LOI_TIG);
			skp->Text(5 * W / 8, 15 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVX", G->LOI_dV_LVLH.x / 0.3048);
			skp->Text(5 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVY", G->LOI_dV_LVLH.y / 0.3048);
			skp->Text(5 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVZ", G->LOI_dV_LVLH.z / 0.3048);
			skp->Text(5 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
		}

		for (int i = 0; i < 20; i++)
		{
			sprintf(Buffer, "%05d", G->REFSMMAToct[i]);
			skp->Text(4 * W / 8, (6+i) * H / 28, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 6)
	{
		if (G->entrycalcmode == 0)
		{
			skp->Text(6 * W / 8,(int)(0.5 * H / 14), "Entry", 5);

			GET_Display(Buffer, G->EntryTIG);
			skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

			if (G->entrycritical != 3)
			{
				if (G->entrylongmanual)
				{
					skp->Text(1 * W / 8, 4 * H / 14, "Manual", 6);
					sprintf(Buffer, "%f °", G->EntryLng*DEG);
					skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
				}
				else
				{
					skp->Text(1 * W / 8, 4 * H / 14, "Landing Zone", 12);
					if (G->landingzone == 0)
					{
						skp->Text(1 * W / 8, 6 * H / 14, "Mid Pacific", 11);
					}
					else if (G->landingzone == 1)
					{
						skp->Text(1 * W / 8, 6 * H / 14, "East Pacific", 12);
					}
					else if (G->landingzone == 2)
					{
						skp->Text(1 * W / 8, 6 * H / 14, "Atlantic Ocean", 14);
					}
					else if (G->landingzone == 3)
					{
						skp->Text(1 * W / 8, 6 * H / 14, "Indian Ocean", 12);
					}
					else if (G->landingzone == 4)
					{
						skp->Text(1 * W / 8, 6 * H / 14, "West Pacific", 12);
					}
				}
			}

			//sprintf(Buffer, "%f °", G->EntryLat*DEG);
			//skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
			
			sprintf(Buffer, "%f °", G->EntryAng*DEG);
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			if (G->entrycritical == 0)
			{
				skp->Text(1 * W / 8, 12 * H / 14, "Deorbit", 7);

				if (G->entrynominal)
				{
					skp->Text(1 * W / 8, 10 * H / 14, "Nominal", 7);
				}
				else
				{
					skp->Text(1 * W / 8, 10 * H / 14, "Min DV", 6);
				}
			}
			else if (G->entrycritical == 1)
			{
				skp->Text(1 * W / 8, 12 * H / 14, "Midcourse", 9);
			}
			else if (G->entrycritical == 2)
			{
				skp->Text(1 * W / 8, 12 * H / 14, "Abort", 5);
			}
			else
			{
				skp->Text(1 * W / 8, 12 * H / 14, "Corridor Control", 16);
			}

			if (G->entrycalcstate == 1)
			{
				skp->Text(5 * W / 8, 2 * H / 14, "Calculating...", 14);
			}
			else if (G->entrycalcstate == 0)
			{
				if (G->entryprecision == 0)
				{
					skp->Text(5 * W / 8, 2 * H / 14, "Conic Solution", 14);
				}
				else if (G->entryprecision == 1)
				{
					skp->Text(5 * W / 8, 2 * H / 14, "Precision Solution", 18);
				}
				else if (G->entryprecision == 2)
				{
					skp->Text(5 * W / 8, 2 * H / 14, "PeA=-30NM Solution", 18);
				}
				else if (G->entryprecision == 9)
				{
					skp->Text(5 * W / 8, 2 * H / 14, "Iteration failed", 16);
				}
			}

			GET_Display(Buffer, G->EntryTIGcor);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.3f° Lat", G->EntryLatcor*DEG);
			skp->Text(5 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.3f° Lng", G->EntryLngcor*DEG);
			skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.3f° ReA", G->EntryAngcor*DEG);
			skp->Text(5 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));
			GET_Display(Buffer, G->P37GET400K);
			sprintf(Buffer, "%s RRT", Buffer);
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			skp->Text(5 * W / 8, 10 * H / 14, "DVX", 3);
			skp->Text(5 * W / 8, 11 * H / 14, "DVY", 3);
			skp->Text(5 * W / 8, 12 * H / 14, "DVZ", 3);

			AGC_Display(Buffer, G->Entry_DV.x / 0.3048);
			skp->Text(6 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->Entry_DV.y / 0.3048);
			skp->Text(6 * W / 8, 11 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->Entry_DV.z / 0.3048);
			skp->Text(6 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));

			if (G->maneuverplanet == oapiGetObjectByName("Moon"))
			{
				skp->Text(6 * W / 8, 13 * H / 14, "Moon SOI", 8);
			}
		}
		else if (G->entrycalcmode == 1)
		{
			skp->Text(6 * W / 8,(int)(0.5 * H / 14), "Entry Update", 12);


			sprintf(Buffer, "Lat:  %f °", G->EntryLatcor*DEG);
			skp->Text(5 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "Long: %f °", G->EntryLngcor*DEG);
			skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "Desired Range: %.1f NM", G->entryrange);
			skp->Text(4 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "Actual Range:  %.1f NM", G->EntryRTGO);
			skp->Text(4 * W / 8, 9 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->entrycalcmode == 2)
		{
			skp->Text(6 * W / 8,(int)(0.5 * H / 14), "P37 Block Data", 14);

			GET_Display(Buffer, G->EntryTIG);
			skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

			if (G->entrylongmanual)
			{
				skp->Text(1 * W / 8, 4 * H / 14, "Manual", 6);
				sprintf(Buffer, "%f °", G->EntryLng*DEG);
				skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
			}
			else
			{
				skp->Text(1 * W / 8, 4 * H / 14, "Landing Zone", 12);
				if (G->landingzone == 0)
				{
					skp->Text(1 * W / 8, 6 * H / 14, "Mid Pacific", 11);
				}
				else if (G->landingzone == 1)
				{
					skp->Text(1 * W / 8, 6 * H / 14, "East Pacific", 12);
				}
				else if (G->landingzone == 2)
				{
					skp->Text(1 * W / 8, 6 * H / 14, "Atlantic Ocean", 14);
				}
				else if (G->landingzone == 3)
				{
					skp->Text(1 * W / 8, 6 * H / 14, "Indian Ocean", 12);
				}
				else if (G->landingzone == 4)
				{
					skp->Text(1 * W / 8, 6 * H / 14, "West Pacific", 12);
				}
			}

			//sprintf(Buffer, "%f °", G->EntryLat*DEG);
			//skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
			//sprintf(Buffer, "%f °", G->EntryLng*DEG);
			//skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%f °", G->EntryAng*DEG);
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			if (G->entrycalcstate == 1)
			{
				skp->Text(5 * W / 8, 2 * H / 14, "Calculating...", 14);
			}

			GET_Display(Buffer, G->EntryTIG);
			skp->Text(4 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "X%04.0f dVT", length(G->Entry_DV)/0.3048);
			skp->Text(4 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "X%+04.0f LONG", G->EntryLngcor*DEG);
			skp->Text(4 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));
			GET_Display(Buffer, G->P37GET400K);
			sprintf(Buffer, "%s 400K", Buffer);
			skp->Text(4 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		}
		else //TEI
		{
			skp->Text(6 * W / 8, (int)(0.5 * H / 14), "TEI", 3);

			double mu;
			VECTOR3 R, V, HH, E;
			OBJHANDLE hMoon = oapiGetObjectByName("Moon");
			mu = GGRAV*oapiGetMass(hMoon);
			G->vessel->GetRelativePos(hMoon, R);
			G->vessel->GetRelativeVel(hMoon, V);
			HH = crossp(R, V);
			E = crossp(V, HH) / mu - unit(R);

			if (G->TEItype != 2)
			{
				GET_Display(Buffer, G->EntryTIG);
				skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
			}

			if (G->entrylongmanual)
			{
				skp->Text(1 * W / 8, 4 * H / 14, "Manual", 6);
				sprintf(Buffer, "%f °", G->EntryLng*DEG);
				skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
			}
			else
			{
				skp->Text(1 * W / 8, 4 * H / 14, "Landing Zone", 12);
				if (G->landingzone == 0)
				{
					skp->Text(1 * W / 8, 6 * H / 14, "Mid Pacific", 11);
				}
				else if (G->landingzone == 1)
				{
					skp->Text(1 * W / 8, 6 * H / 14, "East Pacific", 12);
				}
				else if (G->landingzone == 2)
				{
					skp->Text(1 * W / 8, 6 * H / 14, "Atlantic Ocean", 14);
				}
				else if (G->landingzone == 3)
				{
					skp->Text(1 * W / 8, 6 * H / 14, "Indian Ocean", 12);
				}
				else if (G->landingzone == 4)
				{
					skp->Text(1 * W / 8, 6 * H / 14, "West Pacific", 12);
				}
			}

			if (G->returnspeed == 0)
			{
				skp->Text(1 * W / 8, 10 * H / 14, "Slow Return", 11);
			}
			else if(G->returnspeed == 1)
			{
				skp->Text(1 * W / 8, 10 * H / 14, "Normal Return", 13);
			}
			else if (G->returnspeed == 2)
			{
				skp->Text(1 * W / 8, 10 * H / 14, "Fast Return", 11);
			}

			G->TEIfail = false;

			if (G->TEItype == 0)
			{
				skp->Text(1 * W / 8, 12 * H / 14, "Trans Earth Injection", 21);

				if (length(E) > 1.0)
				{
					G->TEIfail = true;
					skp->Text(1 * W / 8, 8 * H / 14, "TEI not possible!", 17);
				}
			}
			else if (G->TEItype == 1)
			{
				skp->Text(1 * W / 8, 12 * H / 14, "Flyby", 5);

				if (length(E) < 1.0)
				{
					G->TEIfail = true;
					skp->Text(1 * W / 8, 8 * H / 14, "Flyby not possible!", 19);
				}
			}
			else if (G->TEItype == 2)
			{
				skp->Text(1 * W / 8, 12 * H / 14, "PC+2", 4);

				if (length(E) < 1.0)
				{
					G->TEIfail = true;
					skp->Text(1 * W / 8, 8 * H / 14, "PC+2 not possible!", 18);
				}
			}

			if (G->entrycalcstate == 1)
			{
				skp->Text(5 * W / 8, 2 * H / 14, "Calculating...", 14);
			}

			GET_Display(Buffer, G->EntryTIGcor);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.3f° Lat", G->EntryLatcor*DEG);
			skp->Text(5 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.3f° Lng", G->EntryLngcor*DEG);
			skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.3f° ReA", G->EntryAngcor*DEG);
			skp->Text(5 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));
			GET_Display(Buffer, G->P37GET400K);
			sprintf(Buffer, "%s RRT", Buffer);
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			skp->Text(5 * W / 8, 10 * H / 14, "DVX", 3);
			skp->Text(5 * W / 8, 11 * H / 14, "DVY", 3);
			skp->Text(5 * W / 8, 12 * H / 14, "DVZ", 3);
			skp->Text(5 * W / 8, 13 * H / 14, "DVT", 3);

			AGC_Display(Buffer, G->Entry_DV.x / 0.3048);
			skp->Text(6 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->Entry_DV.y / 0.3048);
			skp->Text(6 * W / 8, 11 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->Entry_DV.z / 0.3048);
			skp->Text(6 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, length(G->Entry_DV) / 0.3048);
			skp->Text(6 * W / 8, 13 * H / 14, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 7)
	{
		skp->Text(6 * W / 8,(int)(0.5 * H / 14), "State Vector", 12);

		if (!G->svtimemode)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Now", 3);
		}
		else
		{
			skp->Text(1 * W / 8, 2 * H / 14, "GET", 3);
			GET_Display(Buffer, G->BRCSGET);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		}

		oapiGetObjectName(G->gravref, Buffer, 20);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));


		if (G->svtarget != NULL)
		{
			sprintf(Buffer, G->svtarget->GetName());
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		}

		if (G->SVSlot)
		{
			skp->Text(1 * W / 8, 10 * H / 14, "CSM", 3);
		}
		else
		{
			skp->Text(1 * W / 8, 10 * H / 14, "LM", 2);
		}
		sprintf(Buffer, "%f", G->BRCSPos.x);
		skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", G->BRCSPos.y);
		skp->Text(5 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", G->BRCSPos.z);
		skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%f", G->BRCSVel.x);
		skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", G->BRCSVel.y);
		skp->Text(5 * W / 8, 9 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", G->BRCSVel.z);
		skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%f", G->BRCSGET);
		skp->Text(5 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 8)
	{
		skp->Text(6 * W / 8,(int)(0.5 * H / 14), "Config", 6);

		if (G->mission == 0)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Manual", 8);
		}
		else if (G->mission >= 7)
		{
			sprintf(Buffer, "Apollo %i", G->mission);
			skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		}
		sprintf(Buffer, "%f", G->GETbase);
		skp->Text(5 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		if (G->vesseltype == 0)
		{
			skp->Text(1 * W / 8, 4 * H / 14, "CSM", 3);
		}
		else if(G->vesseltype == 1)
		{
			skp->Text(1 * W / 8, 4 * H / 14, "CSM/LM docked", 13);
		}
		else if (G->vesseltype == 2)
		{
			skp->Text(1 * W / 8, 4 * H / 14, "LM", 3);
		}
		else
		{
			skp->Text(1 * W / 8, 4 * H / 14, "LM/CSM docked", 13);
		}

		oapiGetObjectName(G->gravref, Buffer, 20);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		skp->Text(1 * W / 8, 8 * H / 14, "Sxt/Star Check:", 15);
		sprintf(Buffer, "%.0f min", G->sxtstardtime);
		skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

		skp->Text(1 * W / 8, 10 * H / 14, "Uplink in LOS:", 14);

		if (G->inhibUplLOS)
		{
			skp->Text(5 * W / 8, 10 * H / 14, "Inhibit", 7);
		}
		else
		{
			skp->Text(5 * W / 8, 10 * H / 14, "Enabled", 7);
		}

		//skp->Text(1 * W / 8, 12 * H / 14, "DV Format:", 9);
		//skp->Text(5 * W / 8, 12 * H / 14, "AGC DSKY", 8);
	}
	else if (screen == 9)
	{
		if (G->g_Data.isRequesting)
		{
			skp->Text(6 * W / 8, 8 * H / 14, "Requesting...", 13);
		}

		if (G->manpadopt == 0)
		{
			skp->Text(5 * W / 8,(int)(0.5 * H / 14), "Maneuver PAD", 12);

			if (G->ManPADSPS == 0)
			{
				skp->Text((int)(0.5 * W / 8), 4 * H / 14, "SPS", 3);
			}
			else if (G->ManPADSPS == 1)
			{
				skp->Text((int)(0.5 * W / 8), 4 * H / 14, "RCS +X", 6);
			}
			else
			{
				skp->Text((int)(0.5 * W / 8), 4 * H / 14, "RCS -X", 6);
			}

			if (G->HeadsUp)
			{
				skp->Text((int)(0.5 * W / 8), 6 * H / 14, "Heads Up", 8);
			}
			else
			{
				skp->Text((int)(0.5 * W / 8), 6 * H / 14, "Heads Down", 10);
			}

			if (G->vesseltype == 0)
			{
				skp->Text((int)(0.5 * W / 8), 2 * H / 14, "CSM", 3);
			}
			else
			{
				skp->Text((int)(0.5 * W / 8), 2 * H / 14, "CSM/LM", 6);
			}

			skp->Text((int)(0.5 * W / 8), 8 * H / 14, "REFSMMAT:", 9);

			if (G->REFSMMATcur == 0)
			{
				skp->Text((int)(0.5 * W / 8), 9 * H / 14, "Preferred", 9);
			}
			else if (G->REFSMMATcur == 1)
			{
				skp->Text((int)(0.5 * W / 8), 9 * H / 14, "Retrofire", 9);
			}
			else if (G->REFSMMATcur == 2)
			{
				skp->Text((int)(0.5 * W / 8), 9 * H / 14, "Nominal", 7);
			}
			else if (G->REFSMMATcur == 3)
			{
				skp->Text((int)(0.5 * W / 8), 9 * H / 14, "Entry", 5);
			}
			else if (G->REFSMMATcur == 4)
			{
				skp->Text((int)(0.5 * W / 8), 9 * H / 14, "Launch", 6);
			}
			else if (G->REFSMMATcur == 5)
			{
				skp->Text((int)(0.5 * W / 8), 9 * H / 14, "Landing Site", 12);
			}
			else if (G->REFSMMATcur == 6)
			{
				skp->Text((int)(0.5 * W / 8), 9 * H / 14, "PTC", 3);
			}
			else if (G->REFSMMATcur == 7)
			{
				skp->Text((int)(0.5 * W / 8), 9 * H / 14, "LOI-2", 5);
			}

			if (G->vesseltype == 1)
			{
				sprintf(Buffer, "LM Weight: %5.0f", G->manpad.LMWeight);
				skp->Text((int)(0.5 * W / 8), 10 * H / 14, Buffer, strlen(Buffer));
			}

			skp->Text((int)(0.5 * W / 8), 18 * H / 23, "Set Stars:", 10);
			skp->Text((int)(0.5 * W / 8), 19 * H / 23, G->manpad.SetStars, strlen(G->manpad.SetStars));

			/*if (length(G->manpad.GDCangles) == 0.0)
			{
				skp->Text((int)(0.5 * W / 8), 19 * H / 23, "N/A", 3);
			}
			else
			{
				if (G->GDCset == 0)
				{
					skp->Text((int)(0.5 * W / 8), 19 * H / 23, "Vega, Deneb", 11);
				}
				else if (G->GDCset == 1)
				{
					skp->Text((int)(0.5 * W / 8), 19 * H / 23, "Navi, Polaris", 13);
				}
				else
				{
					skp->Text((int)(0.5 * W / 8), 19 * H / 23, "Acrux, Atria", 12);
				}
			}*/

				sprintf(Buffer, "R %03.0f", OrbMech::round(G->manpad.GDCangles.x));
				skp->Text((int)(0.5 * W / 8), 20 * H / 23, Buffer, strlen(Buffer));
				sprintf(Buffer, "P %03.0f", OrbMech::round(G->manpad.GDCangles.y));
				skp->Text((int)(0.5 * W / 8), 21 * H / 23, Buffer, strlen(Buffer));
				sprintf(Buffer, "Y %03.0f", OrbMech::round(G->manpad.GDCangles.z));
				skp->Text((int)(0.5 * W / 8), 22 * H / 23, Buffer, strlen(Buffer));

			int hh, mm;
			double secs;

			SStoHHMMSS(G->P30TIG, hh, mm, secs);

			skp->Text(7 * W / 8, 3 * H / 26, "N47", 3);
			skp->Text(7 * W / 8, 4 * H / 26, "N48", 3);
			skp->Text(7 * W / 8, 6 * H / 26, "N33", 3);
			skp->Text(7 * W / 8, 9 * H / 26, "N81", 3);
			skp->Text(7 * W / 8, 15 * H / 26, "N44", 3);

			sprintf(Buffer, "%+06.0f WGT", G->manpad.Weight);
			skp->Text((int)(3.5 * W / 8), 3 * H / 26, Buffer, strlen(Buffer));

			if (G->ManPADSPS == 0)
			{
				sprintf(Buffer, "%+07.2f PTRIM", G->manpad.pTrim);
				skp->Text((int)(3.5 * W / 8), 4 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.2f YTRIM", G->manpad.yTrim);
				skp->Text((int)(3.5 * W / 8), 5 * H / 26, Buffer, strlen(Buffer));
			}
			else
			{
				skp->Text((int)(3.5 * W / 8), 4 * H / 26, "N/A      PTRIM", 14);
				skp->Text((int)(3.5 * W / 8), 5 * H / 26, "N/A      YTRIM", 14);
			}

			sprintf(Buffer, "%+06d HRS GETI", hh);
			skp->Text((int)(3.5 * W / 8), 6 * H / 26, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+06d MIN", mm);
			skp->Text((int)(3.5 * W / 8), 7 * H / 26, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+06.0f SEC", secs * 100.0);
			skp->Text((int)(3.5 * W / 8), 8 * H / 26, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f DVX", G->dV_LVLH.x / 0.3048);
			skp->Text((int)(3.5 * W / 8), 9 * H / 26, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVY", G->dV_LVLH.y / 0.3048);
			skp->Text((int)(3.5 * W / 8), 10 * H / 26, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVZ", G->dV_LVLH.z / 0.3048);
			skp->Text((int)(3.5 * W / 8), 11 * H / 26, Buffer, strlen(Buffer));

			sprintf(Buffer, "XXX%03.0f R", G->manpad.Att.x);
			skp->Text((int)(3.5 * W / 8), 12 * H / 26, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f P", G->manpad.Att.y);
			skp->Text((int)(3.5 * W / 8), 13 * H / 26, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f Y", G->manpad.Att.z);
			skp->Text((int)(3.5 * W / 8), 14 * H / 26, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f HA", min(9999.9, G->manpad.HA));
			skp->Text((int)(3.5 * W / 8), 15 * H / 26, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f HP", G->manpad.HP);
			skp->Text((int)(3.5 * W / 8), 16 * H / 26, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f VT", length(G->dV_LVLH) / 0.3048);
			skp->Text((int)(3.5 * W / 8), 17 * H / 26, Buffer, strlen(Buffer));

			SStoHHMMSS(G->manpad.burntime, hh, mm, secs);

			sprintf(Buffer, "XXX%d:%02.0f BT (MIN:SEC)", mm, secs);
			skp->Text((int)(3.5 * W / 8), 18 * H / 26, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f VC", G->manpad.Vc);
			skp->Text((int)(3.5 * W / 8), 19 * H / 26, Buffer, strlen(Buffer));

			//skp->Text(4 * W / 8, 13 * H / 20, "SXT star check", 14);

			if (G->manpad.Star == 0)
			{
				sprintf(Buffer, "N/A     SXTS");
				skp->Text((int)(3.5 * W / 8), 20 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "N/A     SFT");
				skp->Text((int)(3.5 * W / 8), 21 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "N/A     TRN");
				skp->Text((int)(3.5 * W / 8), 22 * H / 26, Buffer, strlen(Buffer));
			}
			else
			{
				sprintf(Buffer, "XXXX%02d SXTS", G->manpad.Star);
				skp->Text((int)(3.5 * W / 8), 20 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.2f SFT", G->manpad.Shaft);
				skp->Text((int)(3.5 * W / 8), 21 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.3f TRN", G->manpad.Trun);
				skp->Text((int)(3.5 * W / 8), 22 * H / 26, Buffer, strlen(Buffer));
			}
			if (G->manpad.BSSStar == 0)
			{
				sprintf(Buffer, "N/A     BSS");
				skp->Text((int)(3.5 * W / 8), 23 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "N/A     SPA");
				skp->Text((int)(3.5 * W / 8), 24 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "N/A     SXP");
				skp->Text((int)(3.5 * W / 8), 25 * H / 26, Buffer, strlen(Buffer));
			}
			else
			{
				sprintf(Buffer, "XXXX%02d BSS", G->manpad.BSSStar);
				skp->Text((int)(3.5 * W / 8), 23 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.2f SPA", G->manpad.SPA);
				skp->Text((int)(3.5 * W / 8), 24 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.3f SXP", G->manpad.SXP);
				skp->Text((int)(3.5 * W / 8), 25 * H / 26, Buffer, strlen(Buffer));
			}
		}
		else if (G->manpadopt == 1)
		{
			skp->Text(4 * W / 8, (int)(0.5 * H / 14), "Terminal Phase Initiate", 23);

			int hh, mm; // ss;
			double secs;

			SStoHHMMSS(G->P30TIG, hh, mm, secs);

			skp->Text(7 * W / 8, 3 * H / 20, "N37", 3);

			sprintf(Buffer, "%+06d HRS GETI", hh);
			skp->Text(3 * W / 8, 3 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+06d MIN", mm);
			skp->Text(3 * W / 8, 4 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+06.0f SEC", secs * 100.0);
			skp->Text(3 * W / 8, 5 * H / 20, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f DVX", G->dV_LVLH.x / 0.3048);
			skp->Text(3 * W / 8, 6 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVY", G->dV_LVLH.y / 0.3048);
			skp->Text(3 * W / 8, 7 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVZ", G->dV_LVLH.z / 0.3048);
			skp->Text(3 * W / 8, 8 * H / 20, Buffer, strlen(Buffer));

			if (G->TPIPAD_dV_LOS.x > 0)
			{
				sprintf(Buffer, "F%04.1f/%02.0f DVX LOS/BT", abs(G->TPIPAD_dV_LOS.x),G->TPIPAD_BT.x);
			}
			else
			{
				sprintf(Buffer, "A%04.1f/%02.0f DVX LOS/BT", abs(G->TPIPAD_dV_LOS.x), G->TPIPAD_BT.x);
			}
			skp->Text(3 * W / 8, 9 * H / 20, Buffer, strlen(Buffer));
			if (G->TPIPAD_dV_LOS.y > 0)
			{
				sprintf(Buffer, "R%04.1f/%02.0f DVY LOS/BT", abs(G->TPIPAD_dV_LOS.y), G->TPIPAD_BT.y);
			}
			else
			{
				sprintf(Buffer, "L%04.1f/%02.0f DVY LOS/BT", abs(G->TPIPAD_dV_LOS.y), G->TPIPAD_BT.y);
			}
			skp->Text(3 * W / 8, 10 * H / 20, Buffer, strlen(Buffer));
			if (G->TPIPAD_dV_LOS.z > 0)
			{
				sprintf(Buffer, "D%04.1f/%02.0f DVZ LOS/BT", abs(G->TPIPAD_dV_LOS.z), G->TPIPAD_BT.z);
			}
			else
			{
				sprintf(Buffer, "U%04.1f/%02.0f DVZ LOS/BT", abs(G->TPIPAD_dV_LOS.z), G->TPIPAD_BT.z);
			}
			skp->Text(3 * W / 8, 11 * H / 20, Buffer, strlen(Buffer));

			sprintf(Buffer, "X%04.1f/%02.1f dH TPI/ddH", G->TPIPAD_dH, G->TPIPAD_ddH);
			skp->Text(3 * W / 8, 12 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "X%06.2f R", G->TPIPAD_R);
			skp->Text(3 * W / 8, 13 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f RDOT at TPI", G->TPIPAD_Rdot);
			skp->Text(3 * W / 8, 14 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "X%06.2f EL minus 5 min", G->TPIPAD_ELmin5);
			skp->Text(3 * W / 8, 15 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "X%06.2f AZ", G->TPIPAD_AZ);
			skp->Text(3 * W / 8, 16 * H / 20, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(4 * W / 8, (int)(0.5 * H / 14), "TLI PAD", 7);

			GET_Display(Buffer, G->tlipad.TB6P);
			sprintf(Buffer, "%s TB6p", Buffer);
			skp->Text(3 * W / 8, 3 * H / 20, Buffer, strlen(Buffer));

			sprintf(Buffer, "XXX%03.0f R", G->tlipad.IgnATT.x);
			skp->Text(3 * W / 8, 4 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f P", G->tlipad.IgnATT.y);
			skp->Text(3 * W / 8, 5 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f Y", G->tlipad.IgnATT.z);
			skp->Text(3 * W / 8, 6 * H / 20, Buffer, strlen(Buffer));

			double secs;
			int mm, hh;
			SStoHHMMSS(G->tlipad.BurnTime, hh, mm, secs);

			sprintf(Buffer, "XXX%d:%02.0f BT", mm, secs);
			skp->Text(3 * W / 8, 7 * H / 20, Buffer, strlen(Buffer));

			sprintf(Buffer, "%07.1f DVC", G->tlipad.dVC);
			skp->Text(3 * W / 8, 8 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+06.0f VI", G->tlipad.VI);
			skp->Text(3 * W / 8, 9 * H / 20, Buffer, strlen(Buffer));

			sprintf(Buffer, "XXX%03.0f R SEP", G->tlipad.SepATT.x);
			skp->Text(3 * W / 8, 10 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f P SEP", G->tlipad.SepATT.y);
			skp->Text(3 * W / 8, 11 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f Y SEP", G->tlipad.SepATT.z);
			skp->Text(3 * W / 8, 12 * H / 20, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 10)
	{

		if (G->entrypadopt == 0)
		{
			skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Earth Entry PAD", 15);
			skp->Text(4 * W / 8, 2 * H / 20, "PREBURN", 7);

			sprintf(Buffer, "XX%+05.1f dV TO", G->earthentrypad.dVTO[0]);
			skp->Text(3 * W / 8, 3 * H / 20, Buffer, strlen(Buffer));

			sprintf(Buffer, "XXX%03.0f R 0.05G", G->earthentrypad.Att400K[0].x);
			skp->Text(3 * W / 8, 4 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f P 0.05G", G->earthentrypad.Att400K[0].y);
			skp->Text(3 * W / 8, 5 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f Y 0.05G", G->earthentrypad.Att400K[0].z);
			skp->Text(3 * W / 8, 6 * H / 20, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f RTGO .05G", G->earthentrypad.RTGO[0]);
			skp->Text(3 * W / 8, 7 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+06.0f VIO  .05G", G->earthentrypad.VIO[0]);
			skp->Text(3 * W / 8, 8 * H / 20, Buffer, strlen(Buffer));

			double secs;
			int mm, hh;

			SStoHHMMSS(G->earthentrypad.Ret05[0], hh, mm, secs);

			sprintf(Buffer, "XX%02d:%02.0f RET  .05G", mm, secs);
			skp->Text(3 * W / 8, 9 * H / 20, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.2f LAT", G->earthentrypad.Lat[0]);
			skp->Text(3 * W / 8, 10 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.2f LONG", G->earthentrypad.Lng[0]);
			skp->Text(3 * W / 8, 11 * H / 20, Buffer, strlen(Buffer));

			skp->Text(4 * W / 8, 12 * H / 20, "POSTBURN", 8);

			sprintf(Buffer, "XXX%03.0f R 0.05G", G->earthentrypad.PB_R400K[0]);
			skp->Text(3 * W / 8, 13 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f RTGO .05G", G->earthentrypad.PB_RTGO[0]);
			skp->Text(3 * W / 8, 14 * H / 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+06.0f VIO  .05G", G->earthentrypad.PB_VIO[0]);
			skp->Text(3 * W / 8, 15 * H / 20, Buffer, strlen(Buffer));

			SStoHHMMSS(G->earthentrypad.PB_Ret05[0], hh, mm, secs);

			sprintf(Buffer, "XX%02d:%02.0f RET  .05G", mm, secs);
			skp->Text(3 * W / 8, 16 * H / 20, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Lunar Entry PAD", 15);

			if (G->EntryPADdirect)
			{
				skp->Text((int)(0.5 * W / 8), 4 * H / 14, "Direct", 6);
			}
			else
			{
				skp->Text((int)(0.5 * W / 8), 4 * H / 14, "MCC", 3);
			}

			if (G->entryrange != 0)
			{
				skp->Text((int)(0.5 * W / 8), 6 * H / 14, "Desired Range:", 14);
				sprintf(Buffer, "%.1f NM", G->entryrange);
				skp->Text((int)(0.5 * W / 8), 7 * H / 14, Buffer, strlen(Buffer));
			}

			sprintf(Buffer, "XXX%03.0f R 0.05G", G->lunarentrypad.Att05[0].x);
			skp->Text(3 * W / 8, 2 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f P 0.05G", G->lunarentrypad.Att05[0].y);
			skp->Text(3 * W / 8, 3 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f Y 0.05G", G->lunarentrypad.Att05[0].z);
			skp->Text(3 * W / 8, 4 * H / 21, Buffer, strlen(Buffer));

			GET_Display(Buffer, G->lunarentrypad.GETHorCheck[0]);
			skp->Text(3 * W / 8, 5 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f P HOR CK", G->lunarentrypad.PitchHorCheck[0]);
			skp->Text(3 * W / 8, 6 * H / 21, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.2f LAT", G->lunarentrypad.Lat[0]);
			skp->Text(3 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.2f LONG", G->lunarentrypad.Lng[0]);
			skp->Text(3 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));

			sprintf(Buffer, "XXX%04.1f MAX G", G->lunarentrypad.MaxG[0]);
			skp->Text(3 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+06.0f V400k", G->lunarentrypad.V400K[0]);
			skp->Text(3 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.2f y400k", G->lunarentrypad.Gamma400K[0]);
			skp->Text(3 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f RTGO .05G", G->lunarentrypad.RTGO[0]);
			skp->Text(3 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+06.0f VIO  .05G", G->lunarentrypad.VIO[0]);
			skp->Text(3 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));

			GET_Display(Buffer, G->lunarentrypad.RRT[0]);
			sprintf(Buffer, "%s RRT", Buffer);
			skp->Text(3 * W / 8, 14 * H / 21, Buffer, strlen(Buffer));

			double secs;
			int mm, hh;

			SStoHHMMSS(G->lunarentrypad.RET05[0], hh, mm, secs);

			sprintf(Buffer, "XX%02d:%02.0f RET  .05G", mm, secs);
			skp->Text(3 * W / 8, 15 * H / 21, Buffer, strlen(Buffer));

			sprintf(Buffer, "XXX%04.2f DO", G->lunarentrypad.DO[0]);
			skp->Text(3 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));

			if (G->lunarentrypad.SXTS[0] == 0)
			{
				sprintf(Buffer, "N/A     SXTS");
				skp->Text(3 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
				sprintf(Buffer, "N/A     SFT");
				skp->Text(3 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
				sprintf(Buffer, "N/A     TRN");
				skp->Text(3 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));
			}
			else
			{
				sprintf(Buffer, "XXXX%02d SXTS", G->lunarentrypad.SXTS[0]);
				skp->Text(3 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.2f SFT", G->lunarentrypad.SFT[0]);
				skp->Text(3 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.3f TRN", G->lunarentrypad.TRN[0]);
				skp->Text(3 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));
			}

			sprintf(Buffer, "XXXX%s LIFT VECTOR", G->lunarentrypad.LiftVector[0]);
			skp->Text(3 * W / 8, 20 * H / 21, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 11)
	{
		char Buffer2[100];

		skp->Text(6 * W / 8, (int)(0.5 * H / 14), "Map Update", 10);

		if (G->mappage == 0)
		{
			skp->Text(6 * W / 8, 4 * H / 14, "Earth", 5);

			sprintf(Buffer, gsnames[G->mapgs]);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer2, G->GSAOSGET);
			sprintf(Buffer, "AOS %s", Buffer2);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer2, G->GSLOSGET);
			sprintf(Buffer, "LOS %s", Buffer2);
			skp->Text(1 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->mappage == 1)
		{
			skp->Text(6 * W / 8, 4 * H / 14, "Moon", 4);

			GET_Display(Buffer2, G->LOSGET);
			sprintf(Buffer, "LOS %s", Buffer2);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer2, G->SRGET);
			sprintf(Buffer, "SR  %s", Buffer2);
			skp->Text(1 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer2, G->PMGET);
			sprintf(Buffer, "PM  %s", Buffer2);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer2, G->AOSGET);
			sprintf(Buffer, "AOS %s", Buffer2);
			skp->Text(1 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer2, G->SSGET);
			sprintf(Buffer, "SS  %s", Buffer2);
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 12)
	{
		skp->Text(6 * W / 8, (int)(0.5 * H / 14), "Lunar Insertion", 15);

		if (G->subThreadStatus == 2)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "Calculating...", 14);
		}

		if (G->LOImaneuver == 0)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "MCC", 3);

			GET_Display(Buffer, G->LOIGET);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer, G->LOIPeriGET);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.3f°", G->LOILat*DEG);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.3f°", G->LOILng*DEG);
			skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->LOIperi/1852.0);
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer, G->TLCC_TIG);
			skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			skp->Text(5 * W / 8, 17 * H / 21, "DVX", 3);
			skp->Text(5 * W / 8, 18 * H / 21, "DVY", 3);
			skp->Text(5 * W / 8, 19 * H / 21, "DVZ", 3);

			sprintf(Buffer, "%+07.1f", G->TLCC_dV_LVLH.x / 0.3048);
			skp->Text(6 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->TLCC_dV_LVLH.y / 0.3048);
			skp->Text(6 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->TLCC_dV_LVLH.z / 0.3048);
			skp->Text(6 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f", length(G->TLCC_dV_LVLH) / 0.3048);
			skp->Text(6 * W / 8, 20 * H / 21, Buffer, strlen(Buffer));

		}
		else if (G->LOImaneuver == 1)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "LOI-1 (w/ MCC)", 14);

			GET_Display(Buffer, G->LOIGET);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->LOIapo / 1852.0);
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->LOIperi / 1852.0);
			skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.3f°", G->LOIinc*DEG);
			skp->Text(1 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));

			skp->Text(5 * W / 8, 6 * H / 21, "MCC", 3);
			GET_Display(Buffer, G->TLCC_TIG);
			skp->Text(5 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVX", G->TLCC_dV_LVLH.x / 0.3048);
			skp->Text(5 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVY", G->TLCC_dV_LVLH.y / 0.3048);
			skp->Text(5 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVZ", G->TLCC_dV_LVLH.z / 0.3048);
			skp->Text(5 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));

			GET_Display(Buffer, G->LOI_TIG);
			skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f DVX", G->LOI_dV_LVLH.x / 0.3048);
			skp->Text(5 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVY", G->LOI_dV_LVLH.y / 0.3048);
			skp->Text(5 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVZ", G->LOI_dV_LVLH.z / 0.3048);
			skp->Text(5 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));
		}
		else if (G->LOImaneuver == 2)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "LOI-1 (w/o MCC)", 15);

			GET_Display(Buffer, G->LOIGET);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->LOIapo / 1852.0);
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->LOIperi / 1852.0);
			skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.3f°", G->LOIinc*DEG);
			skp->Text(1 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer, G->LOI_TIG);
			skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f DVX", G->LOI_dV_LVLH.x / 0.3048);
			skp->Text(5 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVY", G->LOI_dV_LVLH.y / 0.3048);
			skp->Text(5 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVZ", G->LOI_dV_LVLH.z / 0.3048);
			skp->Text(5 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));
		}
		else if (G->LOImaneuver == 3)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "LOI-2", 5);

			sprintf(Buffer, "%.2f NM", G->LOIperi / 1852.0);
			skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer, G->LOI_TIG);
			skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f DVX", G->LOI_dV_LVLH.x / 0.3048);
			skp->Text(5 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVY", G->LOI_dV_LVLH.y / 0.3048);
			skp->Text(5 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVZ", G->LOI_dV_LVLH.z / 0.3048);
			skp->Text(5 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));
		}
		else if (G->LOImaneuver == 4)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TLI", 3);

			GET_Display(Buffer, G->LOIGET);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer, G->LOIPeriGET);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.3f°", G->LOILat*DEG);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.3f°", G->LOILng*DEG);
			skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->LOIperi / 1852.0);
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer, G->TLCC_TIG);
			skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			skp->Text(5 * W / 8, 17 * H / 21, "DVX", 3);
			skp->Text(5 * W / 8, 18 * H / 21, "DVY", 3);
			skp->Text(5 * W / 8, 19 * H / 21, "DVZ", 3);

			sprintf(Buffer, "%+07.1f", G->TLCC_dV_LVLH.x / 0.3048);
			skp->Text(6 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->TLCC_dV_LVLH.y / 0.3048);
			skp->Text(6 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->TLCC_dV_LVLH.z / 0.3048);
			skp->Text(6 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f", length(G->TLCC_dV_LVLH) / 0.3048);
			skp->Text(6 * W / 8, 20 * H / 21, Buffer, strlen(Buffer));

		}
	}
	else if (screen == 13)
	{
		char Buffer2[100];
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Landmark Tracking", 17);

		GET_Display(Buffer, G->LmkTime);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", G->LmkLat*DEG);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", G->LmkLng*DEG);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		GET_Display(Buffer2, G->LmkT1);
		sprintf(Buffer, "T1: %s (HOR)", Buffer2);
		skp->Text(4 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer2, G->LmkT2);
		sprintf(Buffer, "T2: %s (35°)", Buffer2);
		skp->Text(4 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));

		if (G->LmkRange > 0)
		{
			sprintf(Buffer, "%.1f NM North", G->LmkRange / 1852.0);
		}
		else
		{
			sprintf(Buffer, "%.1f NM South", abs(G->LmkRange) / 1852.0);
		}
		
		skp->Text(4 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		skp->Text(4 * W / 8, 9 * H / 14, "N89", 3);
		sprintf(Buffer, "Lat %+07.3f°", G->LmkN89Lat*DEG);
		skp->Text(4 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "Long/2 %+07.3f°", G->LmkLng*DEG*0.5);
		skp->Text(4 * W / 8, 11 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "Alt %+07.2f NM", G->LmkN89Alt/1852.0);
		skp->Text(4 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
	}
	return true;
}

void ApolloRTCCMFD::menuEntryUpload()
{
	if (!G->inhibUplLOS || !G->vesselinLOS())
	{
		if (G->entrycalcmode == 0 || G->entrycalcmode == 3)
		{
			G->EntryUplink();
		}
		else
		{
			G->EntryUpdateUplink();
		}
	}
}

void ApolloRTCCMFD::menuP30Upload()
{
	if (screen == 12 && G->LOImaneuver == 4)
	{
		if (G->g_Data.progVessel->use_lvdc)
		{
			double T_RP, tb5start, mu, r, v, C3, inc, e, alpha_D, f, theta_N;
			VECTOR3 HH, E, K, N;
			SaturnV* testves;

			testves = (SaturnV*)G->g_Data.progVessel;

			tb5start = testves->lvdc->TB5 - 17.0;
			mu = testves->lvdc->mu;

			T_RP = G->P30TIG - tb5start - testves->lvdc->T_RG;
			r = length(G->R_TLI);
			v = length(G->V_TLI);
			C3 = v*v - 2.0*mu / r;
			HH = crossp(G->R_TLI, G->V_TLI);
			E = crossp(G->V_TLI, HH)/mu-unit(G->R_TLI);
			e = length(E);
			K = _V(0.0, 0.0, 1.0);
			N = crossp(HH, K);
			inc = acos(HH.z / length(HH));
			alpha_D = acos(dotp(N, E) / e / length(N));
			if (E.z < 0)
			{
				alpha_D = PI2 - alpha_D;
			}
			f = acos(dotp(E, G->R_TLI) / length(G->R_TLI) / length(E));
			theta_N = acos(N.x / length(N));
			if (N.y > 0)
			{
				theta_N = PI2 - theta_N;
			}
			theta_N -= -80.6041140*RAD;

			testves->lvdc->TU = true;
			testves->lvdc->TU10 = false;

			testves->lvdc->T_RP = T_RP;
			testves->lvdc->C_3 = C3;
			testves->lvdc->Inclination = inc;
			testves->lvdc->e = e;
			testves->lvdc->alpha_D = alpha_D;
			testves->lvdc->f = f;
			testves->lvdc->theta_N = theta_N;

		}
	}
	else if (!G->inhibUplLOS || !G->vesselinLOS())
	{
		G->P30Uplink();
	}
}

char* ApolloRTCCMFD::GET_Display(char* Buff, double time) //Display a time in the format hhh:mm:ss
{
	sprintf(Buff, "%03.0f:%02.0f:%02.0f GET", floor(time / 3600.0), floor(fmod(time, 3600.0) / 60.0), fmod(time, 60.0));
	//sprintf(Buff, "%03d:%02d:%02d", hh, mm, ss);
	return Buff;
}

char* ApolloRTCCMFD::AGC_Display(char* Buff, double vel)
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
	return Buff;
}

void ApolloRTCCMFD::CycleREFSMMATopt()
{
	if (G->REFSMMATopt < 7)
	{
		G->REFSMMATopt++;
	}
	else
	{
		G->REFSMMATopt = 0;
	}
}

void ApolloRTCCMFD::CycleEntryOpt()
{
	if (G->entrycalcmode < 3)
	{
		G->entrycalcmode++;
	}
	else
	{
		G->entrycalcmode = 0;
	}
}

void ApolloRTCCMFD::menuSetMenu()
{
	screen = 0;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetLambertPage()
{
	screen = 1;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetOffsetPage()
{
	screen = 2;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetCDHPage()
{
	screen = 3;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetOrbAdjPage()
{
	screen = 4;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetREFSMMATPage()
{
	screen = 5;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetEntryPage()
{
	screen = 6;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetSVPage()
{
	screen = 7;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetConfigPage()
{
	screen = 8;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetManPADPage()
{
	screen = 9;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetEntryPADPage()
{
	screen = 10;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetMapUpdatePage()
{
	screen = 11;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetLOIPage()
{
	screen = 12;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetLandmarkTrkPage()
{
	screen = 13;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuVoid(){}

void ApolloRTCCMFD::menuNextPage()
{
	if (screen < 2)
	{
		screen++;
	}
	else
	{
		screen = 0;
	}
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuLastPage()
{
	if (screen > 0)
	{
		screen--;
	}
	else
	{
		screen = 2;
	}
	coreButtons.SelectPage(this, screen);
}

// MFD message parser
int ApolloRTCCMFD::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		// Our new MFD mode has been selected, so we create the MFD and
		// return a pointer to it.
		return (int)(new ApolloRTCCMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}

void ApolloRTCCMFD::set_getbase()
{
	if (G->mission < 7)
	{
		G->mission = 7;
	}
	else if (G->mission < 17)
	{
		G->mission++;
	}
	else
	{
		G->mission = 0;
	}

	if (G->mission >= 7)
	{
		G->GETbase = LaunchMJD[G->mission-7];
	}
}

void ApolloRTCCMFD::OrbAdjCalc()
{
	G->OrbitAdjustCalc();
}

void ApolloRTCCMFD::menuManPADTIG()
{

	bool ManPADTIGInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for the maneuver (Format: hhh:mm:ss)", ManPADTIGInput, 0, 20, (void*)this);
}

bool ManPADTIGInput(void *id, char *str, void *data)
{
	int hh, mm, ss, ManPADTIG;

	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		ManPADTIG = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_ManPADTIG(ManPADTIG);

		return true;
		
	}
	return false;
}

void ApolloRTCCMFD::set_ManPADTIG(double ManPADTIG)
{
	G->P30TIG = ManPADTIG;
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

void ApolloRTCCMFD::t1dialogue()
{
	bool T1GETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for the maneuver (Format: hhh:mm:ss)", T1GETInput, 0, 20, (void*)this);
}

bool T1GETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	double elev;
	if (sscanf(str, "E=%lf", &elev) == 1)
		{
			((ApolloRTCCMFD*)data)->set_lambertelev(elev);
			return true;
		}
	else if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_t1(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_t1(double t1)
{
	this->G->T1 = t1;
}

void ApolloRTCCMFD::OrbAdjGETDialogue()
{
	bool OrbAdjGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for the maneuver (Format: hhh:mm:ss)", OrbAdjGETInput, 0, 20, (void*)this);
}

bool OrbAdjGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, SPSGET;
	if (strcmp(str, "PeT") == 0)
	{
		double pet;
		pet = ((ApolloRTCCMFD*)data)->timetoperi();
		((ApolloRTCCMFD*)data)->set_OrbAdjGET(pet);
		return true;
	}
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		SPSGET = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_OrbAdjGET(SPSGET);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_OrbAdjGET(double SPSGET)
{
	this->G->SPSGET = SPSGET;
}

void ApolloRTCCMFD::CDHtimedialogue()
{
	bool CDHGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for the CDH maneuver (Format: hhh:mm:ss)", CDHGETInput, 0, 20, (void*)this);
}

bool CDHGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, CDHtime;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		CDHtime = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_CDHtime(CDHtime);

		return true;
	}
	return false;
}

bool CDHtimeInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_CDHtime(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_CDHtime(double CDHtime)
{
	this->G->CDHtime = CDHtime;
}

void ApolloRTCCMFD::EntryTimeDialogue()
{
	if (!(G->TEItype == 2 && G->entrycalcmode == 3))
	{
		bool EntryGETInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the GET (Format: hhh:mm:ss)", EntryGETInput, 0, 20, (void*)this);
	}
}

bool EntryGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_EntryTime(t1time);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_EntryTime(double time)
{
	this->G->EntryTIG = time;
}

void ApolloRTCCMFD::EntryAngDialogue()
{
	if (G->entrycalcmode != 3)
	{
		bool EntryAngInput(void* id, char *str, void *data);
		oapiOpenInputBox("Entry FPA in degrees (°):", EntryAngInput, 0, 20, (void*)this);
	}
}

bool EntryAngInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_entryang(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_entryang(double ang)
{
	G->EntryAng = ang*RAD;
}

void ApolloRTCCMFD::EntryLatDialogue()
{
	bool EntryLatInput(void* id, char *str, void *data);
	oapiOpenInputBox("Latitude in degree (°):", EntryLatInput, 0, 20, (void*)this);
}

bool EntryLatInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_entrylat(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_entrylat(double lat)
{
	G->EntryLat = lat*RAD;
}

void ApolloRTCCMFD::EntryLngDialogue()
{
	if (G->entrycritical != 3)
	{
		if (G->entrylongmanual)
		{
			bool EntryLngInput(void* id, char *str, void *data);
			oapiOpenInputBox("Longitude in degree (°):", EntryLngInput, 0, 20, (void*)this);
		}
		else
		{
			if (G->landingzone < 4)
			{
				G->landingzone++;
			}
			else
			{
				G->landingzone = 0;
			}
		}
	}
}

bool EntryLngInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_entrylng(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_entrylng(double lng)
{
	G->EntryLng = lng*RAD;
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
	G->sxtstardtime = time;
}

void ApolloRTCCMFD::REFSMMATTimeDialogue()
{
	if (G->REFSMMATopt == 2 || G->REFSMMATopt == 5 || G->REFSMMATopt == 6)
	{
		bool REFSMMATGETInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the GET (Format: hhh:mm:ss)", REFSMMATGETInput, 0, 20, (void*)this);
	}
}

void ApolloRTCCMFD::UploadREFSMMAT()
{
	if (!G->inhibUplLOS || !G->vesselinLOS())
	{
		G->REFSMMATUplink();
	}
}

void ApolloRTCCMFD::set_REFSMMATTime(double time)
{
	this->G->REFSMMATTime = time;
}

double ApolloRTCCMFD::timetoperi()
{
	VECTOR3 R, V;
	double mu,pet, mjd;

	G->vessel->GetRelativePos(G->gravref, R);
	G->vessel->GetRelativeVel(G->gravref, V);
	mu = GGRAV*oapiGetMass(G->gravref);
	pet = OrbMech::timetoperi(R, V, mu);
	mjd = oapiTime2MJD(oapiGetSimTime() + pet);
	return (mjd - G->GETbase)*24.0*3600.0;
}

bool REFSMMATGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (strcmp(str, "PeT") == 0)
	{
		double pet;
		pet = ((ApolloRTCCMFD*)data)->timetoperi();
		((ApolloRTCCMFD*)data)->set_REFSMMATTime(pet);
		return true;
	}
	else if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_REFSMMATTime(t1time);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_lambertelev(double elev)
{
	G->lambertelev = elev*RAD;

	if (G->target == NULL)
	{
		return;
	}
	double mu, SVMJD, dt1;
	MATRIX3 obli;
	VECTOR3 RA0_orb, VA0_orb, RP0_orb, VP0_orb,RA0,VA0,RP0,VP0;

	mu = GGRAV*oapiGetMass(G->gravref);

	//if (IterStage == 0)
	//{

	oapiGetPlanetObliquityMatrix(G->gravref, &obli);//oapiGetRotationMatrix(gravref, &obli);

	G->vessel->GetRelativePos(G->gravref, RA0_orb);
	G->vessel->GetRelativeVel(G->gravref, VA0_orb);
	G->target->GetRelativePos(G->gravref, RP0_orb);
	G->target->GetRelativeVel(G->gravref, VP0_orb);

	//SVtime = oapiGetSimTime();
	SVMJD = oapiGetSimMJD();


	RA0_orb = mul(OrbMech::inverse(obli), RA0_orb);	//Calculates the equatorial state vector from the ecliptic state vector
	VA0_orb = mul(OrbMech::inverse(obli), VA0_orb);
	RP0_orb = mul(OrbMech::inverse(obli), RP0_orb);
	VP0_orb = mul(OrbMech::inverse(obli), VP0_orb);

	RA0 = _V(RA0_orb.x, RA0_orb.z, RA0_orb.y);	//The following equations use another coordinate system than Orbiter
	VA0 = _V(VA0_orb.x, VA0_orb.z, VA0_orb.y);
	RP0 = _V(RP0_orb.x, RP0_orb.z, RP0_orb.y);
	VP0 = _V(VP0_orb.x, VP0_orb.z, VP0_orb.y);

	dt1 = OrbMech::findelev(RA0, VA0, RP0, VP0, SVMJD, G->lambertelev, G->gravref);
	G->T1 = dt1 + (SVMJD - G->GETbase) * 24.0 * 60.0 * 60.0;
}

void ApolloRTCCMFD::calcREFSMMAT()
{
	G->REFSMMATCalc();
}

void ApolloRTCCMFD::OrbAdjApoDialogue()
{
	bool OrbAdjApoInput(void* id, char *str, void *data);
	oapiOpenInputBox("Apoapsis in NM:", OrbAdjApoInput, 0, 20, (void*)this);
}

void ApolloRTCCMFD::gravrefdialogue()
{
	bool GravrefInput(void* id, char *str, void *data);
	oapiOpenInputBox("Reference Body:", GravrefInput, 0, 20, (void*)this);
}

bool GravrefInput(void *id, char *str, void *data)
{
	if (oapiGetGbodyByName(str)!= NULL)
	{
		((ApolloRTCCMFD*)data)->set_gravref(oapiGetGbodyByName(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_gravref(OBJHANDLE body)
{
	G->gravref = body;
	//delete G->mech;
	//G->mech = new OrbMech(G->vessel, G->gravref);
}

void ApolloRTCCMFD::menuLSLat()
{
	if (G->REFSMMATopt == 5)
	{
		bool LSLatInput(void* id, char *str, void *data);
		oapiOpenInputBox("Latitude in degrees:", LSLatInput, 0, 20, (void*)this);
	}
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
	this->G->LSLat = lat*RAD;
}

void ApolloRTCCMFD::menuLSLng()
{
	if (G->REFSMMATopt == 5)
	{
		bool LSLngInput(void* id, char *str, void *data);
		oapiOpenInputBox("Longitude in degrees:", LSLngInput, 0, 20, (void*)this);
	}
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
	this->G->LSLng = lng*RAD;
}

bool OrbAdjApoInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_OrbAdjApo(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_OrbAdjApo(double apo)
{
	this->G->apo_desnm = apo;
}

void ApolloRTCCMFD::OrbAdjPeriDialogue()
{
	bool OrbAdjPeriInput(void* id, char *str, void *data);
	oapiOpenInputBox("Periapsis in NM:", OrbAdjPeriInput, 0, 20, (void*)this);
}

bool OrbAdjPeriInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_OrbAdjPeri(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_OrbAdjPeri(double peri)
{
	this->G->peri_desnm = peri;
}

void ApolloRTCCMFD::OrbAdjIncDialogue()
{
	bool OrbAdjIncInput(void* id, char *str, void *data);
	oapiOpenInputBox("Inclination in degrees:", OrbAdjIncInput, 0, 20, (void*)this);
}

bool OrbAdjIncInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_OrbAdjInc(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_OrbAdjInc(double inc)
{
	this->G->incdeg = inc;
}

void ApolloRTCCMFD::DHdialogue()
{
	bool DHInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the DH:", DHInput, 0, 20, (void*)this);
}

bool DHInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_DH(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_DH(double DH)
{
	this->G->DH = DH;
}

void ApolloRTCCMFD::phasedialogue()
{
	bool PhaseInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the phase angle:", PhaseInput, 0, 20, (void*)this);

}

bool PhaseInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->calcphaseoff(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::calcphaseoff(double angdeg)
{
	double epsilon,vel,r_p,mu,a,angrad,off;
	VECTOR3 R_P, V_P;
	OBJHANDLE gravref;

	this->G->angdeg = angdeg;

	if (G->target != NULL)
	{
		gravref = G->target->GetGravityRef();
		mu = GGRAV*oapiGetMass(gravref);
		G->target->GetRelativePos(gravref, R_P);
		G->target->GetRelativeVel(gravref, V_P);
		vel = length(V_P);
		r_p = length(R_P);
		epsilon = vel*vel / 2.0 - mu / r_p;
		a = -mu / (2.0 * epsilon);
		angrad = angdeg*RAD;
		off = a*angrad;

		G->offvec.x = off;
	}
}

void ApolloRTCCMFD::xdialogue()
{
	bool XoffInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the X offset:", XoffInput, 0, 20, (void*)this);

}

void ApolloRTCCMFD::ydialogue()
{
	bool YoffInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the Y offset:", YoffInput, 0, 20, (void*)this);

}

void ApolloRTCCMFD::zdialogue()
{
	bool ZoffInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the Z offset:", ZoffInput, 0, 20, (void*)this);

}

bool XoffInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_Xoff(atof(str));
		return true;
	}
	return false;
}

bool YoffInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_Yoff(atof(str));
		return true;
	}
	return false;
}

bool ZoffInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_Zoff(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_Xoff(double x)
{
	G->offvec.x = x;
}

void ApolloRTCCMFD::set_Yoff(double y)
{
	G->offvec.y = y;
}

void ApolloRTCCMFD::set_Zoff(double z)
{
	G->offvec.z = z;
}

void ApolloRTCCMFD::menuSetSVTime()
{
	if (G->svtimemode)
	{
		bool SVGETInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the GET for the state vector (Format: hhh:mm:ss)", SVGETInput, 0, 20, (void*)this);
	}
}

bool SVGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss;
	double SVtime;

	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		SVtime = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_SVtime(SVtime);

		return true;
	}
	else if (sscanf(str, "%d", &ss) == 1)
	{
		((ApolloRTCCMFD*)data)->set_SVtime(0);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SVtime(double SVtime)
{
	G->BRCSGET = SVtime;
}

void ApolloRTCCMFD::t2dialogue()
{
	bool T2GETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for the arrival (Format: hhh:mm:ss)", T2GETInput, 0, 20, (void*)this);
}



bool T2GETInput(void *id, char *str, void *data)
{
	int hh, mm, ss;
	double t2time,unival;
	char uni[10];
	if (sscanf(str, "T1+%lf%s", &t2time, &uni) == 2)
	{
		if (strcmp(uni, "min") == 0)
		{
			unival = 60.0;
		}
		else if (strcmp(uni, "h") == 0)
		{
			unival = 3600.0;
		}
		else if (strcmp(uni, "s") == 0)
		{
			unival = 1.0;
		}
		else
		{
			return false;
		}
		((ApolloRTCCMFD*)data)->set_t2(t2time*unival, false);
		return true;
	}
	else if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t2time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_t2(t2time, true);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_t2(double t2, bool t1dep)
{
	if (t1dep)
	{
		this->G->T2 = t2;
	}
	else
	{
		this->G->T2 = G->T1+t2;
	}
}

void ApolloRTCCMFD::revdialogue()
{
	bool RevInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the number of revolutions:", RevInput, 0, 20, (void*)this);
}

bool RevInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_rev(atoi(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_rev(int rev)
{
	this->G->N = rev;
}

void ApolloRTCCMFD::set_target()
{
	if (!G->SVSlot || screen != 7)
	{
		int vesselcount;

		vesselcount = oapiGetVesselCount();

		if (G->targetnumber < vesselcount - 1)
		{
			G->targetnumber++;
		}
		else
		{
			G->targetnumber = 0;
		}

		G->target = oapiGetVesselInterface(oapiGetVesselByIndex(G->targetnumber));
	}
}

void ApolloRTCCMFD::set_svtarget()
{
	int vesselcount;

	vesselcount = oapiGetVesselCount();

	if (G->svtargetnumber < vesselcount - 1)
	{
		G->svtargetnumber++;
	}
	else
	{
		G->svtargetnumber = 0;
	}

		G->svtarget = oapiGetVesselInterface(oapiGetVesselByIndex(G->svtargetnumber));
}

void ApolloRTCCMFD::CDHcalc()
{
	if (G->target != NULL)
	{
		G->CDHcalc();
	}
}

void ApolloRTCCMFD::lambertcalc()
{
	if (G->target != NULL)// && G->iterator == 0)
	{
		G->lambertcalc();
	}
}

void ApolloRTCCMFD::menuEntryCalc()
{
	double SVMJD;
	VECTOR3 R, V, R0B, V0B;
	MATRIX3 Rot;

	G->vessel->GetRelativePos(G->gravref, R);
	G->vessel->GetRelativeVel(G->gravref, V);
	SVMJD = oapiGetSimMJD();

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	R0B = mul(Rot, _V(R.x, R.z, R.y));
	V0B = mul(Rot, _V(V.x, V.z, V.y));

	if (G->entrycalcmode == 0)
	{
		if (G->entrylongmanual)
		{
			G->entry = new Entry(R0B, V0B, SVMJD, G->gravref, G->GETbase, G->EntryTIG, G->EntryAng, G->EntryLng, G->entrycritical, 0, G->entrynominal, G->entrylongmanual);
		}
		else
		{
			G->entry = new Entry(R0B, V0B, SVMJD, G->gravref, G->GETbase, G->EntryTIG, G->EntryAng, (double)G->landingzone, G->entrycritical, 0, G->entrynominal, G->entrylongmanual);

		}
		G->entrycalcstate = 1;// G->EntryCalc();
	}
	else if(G->entrycalcmode == 1)
	{
		G->entry = new Entry(R0B, V0B, SVMJD, G->gravref, G->GETbase, G->EntryTIG, G->EntryAng, G->EntryLng, G->entrycritical, G->entryrange, G->entrynominal, true);
		G->entrycalcstate = 2;// G->EntryUpdateCalc();
	}
	else if (G->entrycalcmode == 2)
	{
		G->entry = new Entry(R0B, V0B, SVMJD, G->gravref, G->GETbase, G->EntryTIG, G->EntryAng, G->EntryLng, 2, 0, 0, true);
		G->entrycalcstate = 1;// G->EntryCalc();
	}
	else
	{
		if (!G->TEIfail)
		{
			if (G->entrylongmanual)
			{
				G->teicalc = new TEI(R0B, V0B, SVMJD, G->gravref, G->GETbase + G->EntryTIG / 24.0 / 3600.0, G->EntryLng, G->entrylongmanual, G->returnspeed, G->TEItype);
			}
			else
			{
				G->teicalc = new TEI(R0B, V0B, SVMJD, G->gravref, G->GETbase + G->EntryTIG / 24.0 / 3600.0, (double)G->landingzone, G->entrylongmanual, G->returnspeed, G->TEItype);
			}
			G->entrycalcstate = 1;
		}
	}
}

void ApolloRTCCMFD::EntryRangeDialogue()
{
	if (G->entrycalcmode == 1)
	{
		bool EntryRangeInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the Entry Range in NM:", EntryRangeInput, 0, 20, (void*)this);
	}
}

bool EntryRangeInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_entryrange(atoi(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_entryrange(double range)
{
	G->entryrange = range;
}

void ApolloRTCCMFD::menuSVCalc()
{
	if (G->svtarget != NULL)
	{
		G->StateVectorCalc();
	}
}

void ApolloRTCCMFD::menuSwitchSVSlot()
{
	G->SVSlot = !G->SVSlot;
}

void ApolloRTCCMFD::menuSVUpload()
{
	if (!G->inhibUplLOS || !G->vesselinLOS())
	{
		G->StateVectorUplink();
	}
}

void ApolloRTCCMFD::set_spherical()
{
	if (G->lambertopt < 1)
	{
		G->lambertopt++;
		G->lambertmultiaxis = 1;
	}
	else
	{
		G->lambertopt = 0;
	}
}


void ApolloRTCCMFD::menuSwitchHeadsUp()
{
	if (G->manpadopt == 0)
	{
		G->HeadsUp = !G->HeadsUp;
	}
}

void ApolloRTCCMFD::menuCalcManPAD()
{
	if (G->manpadopt == 0)
	{
		G->ManeuverPAD();
	}
	else if (G->manpadopt == 1)
	{
		if (G->target != NULL)
		{
			G->TPIPAD();
		}
	}
	else
	{
		G->TLI_PAD();
	}
}

void ApolloRTCCMFD::menuCalcEntryPAD()
{
	if (length(G->dV_LVLH) != 0.0 || G->EntryPADdirect)
	{
		G->EntryPAD();
	}
}

void ApolloRTCCMFD::menuCalcMapUpdate()
{
	G->MapUpdate();
}

void ApolloRTCCMFD::menuSwitchCritical()
{
	if (G->entrycalcmode == 0)
	{
		if (G->entrycritical < 3)
		{
			G->entrycritical++;
		}
		else
		{
			G->entrycritical = 0;
		}
	}
	else if (G->entrycalcmode == 3)
	{
		if (G->TEItype < 2)
		{
			G->TEItype++;
		}
		else
		{
			G->TEItype = 0;
		}
	}
}

void ApolloRTCCMFD::menuSwitchEntryPADOpt()
{
	if (G->entrypadopt < 1)
	{
		G->entrypadopt++;
	}
	else
	{
		G->entrypadopt = 0;
		G->EntryPADdirect = false;
	}
}

void ApolloRTCCMFD::menuSwitchEntryPADDirect()
{
	if (G->entrypadopt == 1)
	{
		G->EntryPADdirect = !G->EntryPADdirect;
	}
}

void ApolloRTCCMFD::menuSwitchManPADEngine()
{
	if (G->manpadopt == 0)
	{
		if (G->ManPADSPS < 2)
		{
			G->ManPADSPS++;
		}
		else
		{
			G->ManPADSPS = 0;
		}
	}
}

void ApolloRTCCMFD::menuSwitchManPADopt()
{
	if (G->manpadopt < 2)
	{
		G->manpadopt++;
	}
	else
	{
		G->manpadopt = 0;
	}
}

void ApolloRTCCMFD::menuREFSMMATdirect()
{
	if (G->REFSMMATopt == 2 || G->REFSMMATopt == 3 || G->REFSMMATopt == 5)
	{
		G->REFSMMATdirect = !G->REFSMMATdirect;
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

void ApolloRTCCMFD::menuSwitchUplinkInhibit()
{
	G->inhibUplLOS = !G->inhibUplLOS;
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

void ApolloRTCCMFD::menuSetLaunchMJD()
{
	if (G->mission == 0)
	{
		bool LaunchMJDInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the launch MJD:", LaunchMJDInput, 0, 20, (void*)this);
	}
}

bool LaunchMJDInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_launchmjd(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_launchmjd(double mjd)
{
	this->G->GETbase = mjd;
}

void ApolloRTCCMFD::menuChangeVesselType()
{
	if (G->vesseltype < 3)
	{
		G->vesseltype++;
	}
	else
	{
		G->vesseltype = 0;
	}

	if (G->vesseltype < 2)
	{
		G->g_Data.uplinkLEM = 0;
	}
	else
	{
		G->g_Data.uplinkLEM = 1;
	}
}

void ApolloRTCCMFD::cycleREFSMMATupl()
{
	if (G->REFSMMATupl < 1)
	{
		G->REFSMMATupl++;
	}
	else
	{
		G->REFSMMATupl = 0;
	}
}

void ApolloRTCCMFD::offvecdialogue()
{
	bool OffVecInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the offset (x.x x.x x.x):", OffVecInput, 0, 20, (void*)this);
}

bool OffVecInput(void *id, char *str, void *data)
{
	double xoff, yoff, zoff;
	if (sscanf(str, "%lf %lf %lf", &xoff, &yoff, &zoff) == 3)
	{
		((ApolloRTCCMFD*)data)->set_offvec(_V(xoff*1852.0,yoff*1852.0,zoff*1852.0));
		return true;
	}
	else if (sscanf(str, "X=%lf", &xoff) == 1 || sscanf(str, "x=%lf", &xoff) == 1)
	{
		((ApolloRTCCMFD*)data)->set_Xoff(xoff*1852.0);
		return true;
	}
	else if (sscanf(str, "Y=%lf", &yoff) == 1 || sscanf(str, "y=%lf", &yoff) == 1)
	{
		((ApolloRTCCMFD*)data)->set_Yoff(yoff*1852.0);
		return true;
	}
	else if (sscanf(str, "Z=%lf", &zoff) == 1 || sscanf(str, "z=%lf", &zoff) == 1)
	{
		((ApolloRTCCMFD*)data)->set_Zoff(zoff*1852.0);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_offvec(VECTOR3 off)
{
	G->offvec = off;
}

void ApolloRTCCMFD::StoreStatus(void) const
{
	screenData.screen = screen;
	
}

void ApolloRTCCMFD::RecallStatus(void)
{
	screen = screenData.screen;
	coreButtons.SelectPage(this, screen);
}

ApolloRTCCMFD::ScreenData ApolloRTCCMFD::screenData = { 0 };

void ApolloRTCCMFD::menuCycleSVTimeMode()
{
	G->svtimemode = !G->svtimemode;
}

void ApolloRTCCMFD::GetREFSMMATfromAGC()
{
	if (G->vesseltype < 2)
	{
		saturn = (Saturn *)G->vessel;
		if (saturn->IsVirtualAGC() == FALSE)
		{

		}
		else
		{
			unsigned short REFSoct[20];
			REFSoct[2] = saturn->agc.vagc.Erasable[0][01735];
			REFSoct[3] = saturn->agc.vagc.Erasable[0][01736];
			REFSoct[4] = saturn->agc.vagc.Erasable[0][01737];
			REFSoct[5] = saturn->agc.vagc.Erasable[0][01740];
			REFSoct[6] = saturn->agc.vagc.Erasable[0][01741];
			REFSoct[7] = saturn->agc.vagc.Erasable[0][01742];
			REFSoct[8] = saturn->agc.vagc.Erasable[0][01743];
			REFSoct[9] = saturn->agc.vagc.Erasable[0][01744];
			REFSoct[10] = saturn->agc.vagc.Erasable[0][01745];
			REFSoct[11] = saturn->agc.vagc.Erasable[0][01746];
			REFSoct[12] = saturn->agc.vagc.Erasable[0][01747];
			REFSoct[13] = saturn->agc.vagc.Erasable[0][01750];
			REFSoct[14] = saturn->agc.vagc.Erasable[0][01751];
			REFSoct[15] = saturn->agc.vagc.Erasable[0][01752];
			REFSoct[16] = saturn->agc.vagc.Erasable[0][01753];
			REFSoct[17] = saturn->agc.vagc.Erasable[0][01754];
			REFSoct[18] = saturn->agc.vagc.Erasable[0][01755];
			REFSoct[19] = saturn->agc.vagc.Erasable[0][01756];

			for (int i = 2; i < 20; i++)
			{
				sprintf(Buffer, "%05o", REFSoct[i]);
				G->REFSMMAToct[i] = atoi(Buffer);
			}

			G->REFSMMAT.m11 = OrbMech::DecToDouble(REFSoct[2], REFSoct[3])*2.0;
			G->REFSMMAT.m12 = OrbMech::DecToDouble(REFSoct[4], REFSoct[5])*2.0;
			G->REFSMMAT.m13 = OrbMech::DecToDouble(REFSoct[6], REFSoct[7])*2.0;
			G->REFSMMAT.m21 = OrbMech::DecToDouble(REFSoct[8], REFSoct[9])*2.0;
			G->REFSMMAT.m22 = OrbMech::DecToDouble(REFSoct[10], REFSoct[11])*2.0;
			G->REFSMMAT.m23 = OrbMech::DecToDouble(REFSoct[12], REFSoct[13])*2.0;
			G->REFSMMAT.m31 = OrbMech::DecToDouble(REFSoct[14], REFSoct[15])*2.0;
			G->REFSMMAT.m32 = OrbMech::DecToDouble(REFSoct[16], REFSoct[17])*2.0;
			G->REFSMMAT.m33 = OrbMech::DecToDouble(REFSoct[18], REFSoct[19])*2.0;
			G->REFSMMATcur = G->REFSMMATopt;

			//sprintf(oapiDebugString(), "%f, %f, %f, %f, %f, %f, %f, %f, %f", G->REFSMMAT.m11, G->REFSMMAT.m12, G->REFSMMAT.m13, G->REFSMMAT.m21, G->REFSMMAT.m22, G->REFSMMAT.m23, G->REFSMMAT.m31, G->REFSMMAT.m32, G->REFSMMAT.m33);
		}
	}
}

void ApolloRTCCMFD::GetEntryTargetfromAGC()
{
	if (G->vesseltype < 2)
	{
		saturn = (Saturn *)G->vessel;
		//if (saturn->IsVirtualAGC() == FALSE)
		//{
		//
		//}
		//else
		//{
			unsigned short Entryoct[6];
			Entryoct[2] = saturn->agc.vagc.Erasable[0][03400];
			Entryoct[3] = saturn->agc.vagc.Erasable[0][03401];
			Entryoct[4] = saturn->agc.vagc.Erasable[0][03402];
			Entryoct[5] = saturn->agc.vagc.Erasable[0][03403];

			G->EntryLatcor = OrbMech::DecToDouble(Entryoct[2], Entryoct[3])*PI2;
			G->EntryLngcor = OrbMech::DecToDouble(Entryoct[4], Entryoct[5])*PI2;
			//G->EntryPADLat = G->EntryLatcor;
			//G->EntryPADLng = G->EntryLngcor;
		//}
	}
}

void ApolloRTCCMFD::set_lambertaxis()
{
	if (G->lambertopt == false)
	{
		G->lambertmultiaxis = !G->lambertmultiaxis;
	}
}

void ApolloRTCCMFD::menuSwitchEntryNominal()
{
	if (G->entrycritical == 0 && G->entrycalcmode == 0)
	{
		G->entrynominal = !G->entrynominal;
	}
	else if (G->entrycalcmode == 3)
	{
		if (G->returnspeed < 2)
		{
			G->returnspeed++;
		}
		else
		{
			G->returnspeed = 0;
		}
	}
}

void ApolloRTCCMFD::EntryLongitudeModeDialogue()
{
	if (G->entrycalcmode == 0 || G->entrycalcmode == 2 || G->entrycalcmode == 3)
	{
		if (G->entrycritical != 3)
		{
			G->entrylongmanual = !G->entrylongmanual;
		}
	}
}

void ApolloRTCCMFD::menuSwitchLOIManeuver()
{
	if (G->LOImaneuver < 4)
	{
		G->LOImaneuver++;
	}
	else
	{
		G->LOImaneuver = 0;
	}
}

void ApolloRTCCMFD::menuSetLOIGET()
{
	if (G->LOImaneuver == 0 || G->LOImaneuver == 1 || G->LOImaneuver == 2 || G->LOImaneuver == 4)
	{
		bool LOIGETInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the GET for the maneuver (Format: hhh:mm:ss)", LOIGETInput, 0, 20, (void*)this);
	}
}

bool LOIGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_LOIGET(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LOIGET(double time)
{
	G->LOIGET = time;
}

void ApolloRTCCMFD::menuSetLOIPeriGET()
{
	if (G->LOImaneuver == 0 || G->LOImaneuver == 4)
	{
		bool LOIPeriGETInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the Pericyntheon GET (Format: hhh:mm:ss)", LOIPeriGETInput, 0, 20, (void*)this);
	}
}

bool LOIPeriGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_LOIPeriGET(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LOIPeriGET(double time)
{
	G->LOIPeriGET = time;
}

void ApolloRTCCMFD::menuSetLOILat()
{
	if (G->LOImaneuver == 0 || G->LOImaneuver == 4)
	{
		bool LOILatInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the pericyntheon latitude:", LOILatInput, 0, 20, (void*)this);
	}
}

bool LOILatInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LOILat(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LOILat(double lat)
{
	this->G->LOILat = lat*RAD;
}

void ApolloRTCCMFD::menuSetLOILng()
{
	if (G->LOImaneuver == 0 || G->LOImaneuver == 4)
	{
		bool LOILngInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the pericyntheon longitude:", LOILngInput, 0, 20, (void*)this);
	}
}

bool LOILngInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LOILng(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LOILng(double lng)
{
	this->G->LOILng = lng*RAD;
}

void ApolloRTCCMFD::menuSetLOIAlt()
{
	bool LOIAltInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the pericyntheon altitude:", LOIAltInput, 0, 20, (void*)this);
}

bool LOIAltInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LOIAlt(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LOIAlt(double alt)
{
	this->G->LOIperi = alt*1852.0;
}

void ApolloRTCCMFD::menuSetLOIApo()
{
	if (G->LOImaneuver == 1 || G->LOImaneuver == 2)
	{
		bool LOIApoInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the apocyntheon altitude:", LOIApoInput, 0, 20, (void*)this);
	}
}

bool LOIApoInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LOIApo(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LOIApo(double alt)
{
	this->G->LOIapo = alt*1852.0;
}

void ApolloRTCCMFD::menuSetLOIInc()
{
	if (G->LOImaneuver == 1 || G->LOImaneuver == 2)
	{
		bool LOIIncInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the LOI inclination:", LOIIncInput, 0, 20, (void*)this);
	}
}

bool LOIIncInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LOIInc(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LOIInc(double inc)
{
	this->G->LOIinc = inc*RAD;
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
	bool LmkTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the guess for T1:", LmkTimeInput, 0, 20, (void*)this);
}

bool LmkTimeInput(void *id, char *str, void *data)
{
	int hh, mm, ss, Lmktime;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		Lmktime = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_LmkTime(Lmktime);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LmkTime(double time)
{
	this->G->LmkTime = time;
}

void ApolloRTCCMFD::menuSetLmkLat()
{
	bool LmkLatInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the landmark latitude:", LmkLatInput, 0, 20, (void*)this);
}

bool LmkLatInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LmkLat(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LmkLat(double lat)
{
	this->G->LmkLat = lat*RAD;
}

void ApolloRTCCMFD::menuSetLmkLng()
{
	bool LmkLngInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the landmark longitude:", LmkLngInput, 0, 20, (void*)this);
}

bool LmkLngInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LmkLng(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LmkLng(double lng)
{
	this->G->LmkLng = lng*RAD;
}

void ApolloRTCCMFD::menuRequestLTMFD()
{
	if (G->manpadopt == 0 || G->manpadopt == 2)
	{
		if (G->g_Data.isRequesting)
		{
			G->StopIMFDRequest();
		}
		else
		{
			G->StartIMFDRequest();
		}
	}
}

void ApolloRTCCMFD::SStoHHMMSS(double time, int &hours, int &minutes, double &seconds)
{
	double mins;
	hours = (int)trunc(time / 3600.0);
	mins = fmod(time / 60.0, 60.0);
	minutes = (int)trunc(mins);
	seconds = (mins - minutes) * 60.0;
}