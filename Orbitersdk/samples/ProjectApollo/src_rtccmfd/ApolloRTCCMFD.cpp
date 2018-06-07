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
#include "LVDC.h"
#include "iu.h"

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
	papiWriteScenario_double(scn, "SXTSTARDTIME", G->sxtstardtime);
	papiWriteScenario_mx(scn, "REFSMMAT", G->REFSMMAT);
	papiWriteScenario_intarr(scn, "REFSMMAToct", G->REFSMMAToct, 20);
	oapiWriteScenario_int(scn, "REFSMMATcur", G->REFSMMATcur);
	oapiWriteScenario_int(scn, "REFSMMATopt", G->REFSMMATopt);
	papiWriteScenario_double(scn, "REFSMMATTime", G->REFSMMATTime);
	oapiWriteScenario_int(scn, "REFSMMATupl", G->REFSMMATupl);
	papiWriteScenario_bool(scn, "REFSMMATdirect", G->REFSMMATdirect);
	papiWriteScenario_bool(scn, "REFSMMATHeadsUp", G->REFSMMATHeadsUp);
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
	oapiWriteScenario_int(scn, "TWOIMPULSEMODE", G->twoimpulsemode);
	papiWriteScenario_double(scn, "lambertelev", G->lambertelev);
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
	papiWriteScenario_double(scn, "LSAlt", G->LSAlt);
	papiWriteScenario_double(scn, "TLAND", G->t_Land);
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
	oapiWriteScenario_int(scn, "ENTRYCRITICAL", G->entrycritical);
	papiWriteScenario_double(scn, "ENTRYRANGE", G->entryrange);
	papiWriteScenario_bool(scn, "ENTRYNOMINAL", G->entrynominal);
	papiWriteScenario_bool(scn, "ENTRYLONGMANUAL", G->entrylongmanual);
	oapiWriteScenario_int(scn, "LANDINGZONE", G->landingzone);
	oapiWriteScenario_int(scn, "ENTRYPRECISION", G->entryprecision);

	papiWriteScenario_double(scn, "P37GET400K", G->P37GET400K);
	oapiWriteScenario_int(scn, "MAPPAGE", G->mappage);
	papiWriteScenario_bool(scn, "INHIBITUPLINK", G->inhibUplLOS);
	papiWriteScenario_double(scn, "APODESNM", G->apo_desnm);
	papiWriteScenario_double(scn, "PERIDESNM", G->peri_desnm);
	papiWriteScenario_double(scn, "INCDEG", G->incdeg);
	papiWriteScenario_double(scn, "SPSGET", G->SPSGET);
	papiWriteScenario_vec(scn, "OrbAdjDVX", G->OrbAdjDVX);

	oapiWriteScenario_int(scn, "LOIMANEUVER", G->LOImaneuver);
	oapiWriteScenario_int(scn, "TLCCMANEUVER", G->TLCCmaneuver);
	papiWriteScenario_double(scn, "TLCCGET", G->TLCC_GET);
	papiWriteScenario_double(scn, "TLCCPeriGET", G->TLCCPeriGET);
	papiWriteScenario_double(scn, "TLCCPeriGETcor", G->TLCCPeriGETcor);
	papiWriteScenario_double(scn, "TLCCReentryGET", G->TLCCReentryGET);
	papiWriteScenario_double(scn, "TLCCNodeLat", G->TLCCNodeLat);
	papiWriteScenario_double(scn, "TLCCFreeReturnEMPLat", G->TLCCFreeReturnEMPLat);
	papiWriteScenario_double(scn, "TLCCNonFreeReturnEMPLat", G->TLCCNonFreeReturnEMPLat);
	papiWriteScenario_double(scn, "TLCCNodeLng", G->TLCCNodeLng);
	papiWriteScenario_double(scn, "TLCCLAHPeriAlt", G->TLCCLAHPeriAlt);
	papiWriteScenario_double(scn, "TLCCFlybyPeriAlt", G->TLCCFlybyPeriAlt);
	papiWriteScenario_double(scn, "TLCCNodeAlt", G->TLCCNodeAlt);
	papiWriteScenario_double(scn, "TLCCNodeGET", G->TLCCNodeGET);
	papiWriteScenario_double(scn, "LOIapo", G->LOIapo);
	papiWriteScenario_double(scn, "LOIperi", G->LOIperi);
	papiWriteScenario_double(scn, "LOIazi", G->LOIazi);
	papiWriteScenario_vec(scn, "TLCCDV", G->TLCC_dV_LVLH);
	papiWriteScenario_vec(scn, "LOIDV", G->LOI_dV_LVLH);
	papiWriteScenario_double(scn, "TLCCTIG", G->TLCC_TIG);
	papiWriteScenario_double(scn, "LOITIG", G->LOI_TIG);
	papiWriteScenario_vec(scn, "R_TLI", G->R_TLI);
	papiWriteScenario_vec(scn, "V_TLI", G->V_TLI);

	papiWriteScenario_bool(scn, "SKYLABNPCOPTION", G->Skylab_NPCOption);
	papiWriteScenario_bool(scn, "SKYLABPCMANEUVER", G->Skylab_PCManeuver);
	oapiWriteScenario_int(scn, "SKYLABMANEUVER", G->Skylabmaneuver);
	papiWriteScenario_double(scn, "SKYLAB_N_C", G->Skylab_n_C);
	papiWriteScenario_double(scn, "SKYLAB_NC1", G->Skylab_t_NC1);
	papiWriteScenario_double(scn, "SKYLAB_NC2", G->Skylab_t_NC2);
	papiWriteScenario_double(scn, "SKYLAB_NCC", G->Skylab_t_NCC);
	papiWriteScenario_double(scn, "SKYLAB_NSR", G->Skylab_t_NSR);
	papiWriteScenario_double(scn, "t_TPI", G->t_TPI);
	//papiWriteScenario_double(scn, "SKYLAB_NPC", G->Skylab_t_NPC);
	papiWriteScenario_double(scn, "SKYLAB_DTTPM", G->Skylab_dt_TPM);
	papiWriteScenario_double(scn, "SKYLAB_E_L", G->Skylab_E_L);
	papiWriteScenario_double(scn, "SKYLAB_DH1", G->SkylabDH1);
	papiWriteScenario_double(scn, "SKYLAB_DH2", G->SkylabDH2);

	papiWriteScenario_bool(scn, "PCLANDED", G->PClanded);
	papiWriteScenario_double(scn, "PCALIGNGET", G->PCAlignGET);
	papiWriteScenario_double(scn, "PCEARLIESTGET", G->PCEarliestGET);
	papiWriteScenario_double(scn, "PCTIG", G->PC_TIG);
	papiWriteScenario_vec(scn, "PC_DV_LVLH", G->PC_dV_LVLH);

	oapiWriteScenario_int(scn, "DOI_N", G->DOI_N);
	papiWriteScenario_double(scn, "DOIGET", G->DOIGET);
	papiWriteScenario_double(scn, "DOI_PERIANG", G->DOI_PeriAng);

	papiWriteScenario_double(scn, "DKI_TIG", G->DKI_TIG);
	papiWriteScenario_double(scn, "t_TPIguess", G->t_TPIguess);
	oapiWriteScenario_int(scn, "DKI_Profile", G->DKI_Profile);
	oapiWriteScenario_int(scn, "DKI_TPI_Mode", G->DKI_TPI_Mode);
	papiWriteScenario_bool(scn, "DKI_Maneuver_Line", G->DKI_Maneuver_Line);
	papiWriteScenario_bool(scn, "DKI_Radial_DV", G->DKI_Radial_DV);
	oapiWriteScenario_int(scn, "DKI_N", G->DKI_N);

	papiWriteScenario_double(scn, "AGSKFACTOR", G->AGSKFactor);

}

void ApolloRTCCMFD::ReadStatus(FILEHANDLE scn)
{
	char *line;
	char Buffer2[100];
	bool istarget = false;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, "END_MFD", 7))
			return;

		//papiReadScenario_int(line, "SCREEN", G->screen);
		papiReadScenario_int(line, "VESSELTYPE", G->vesseltype);
		papiReadScenario_double(line, "SXTSTARDTIME", G->sxtstardtime);
		papiReadScenario_mat(line, "REFSMMAT", G->REFSMMAT);
		papiReadScenario_intarr(line, "REFSMMAToct", G->REFSMMAToct, 20);
		papiReadScenario_int(line, "REFSMMATcur", G->REFSMMATcur);
		papiReadScenario_int(line, "REFSMMATopt", G->REFSMMATopt);
		papiReadScenario_double(line, "REFSMMATTime", G->REFSMMATTime);
		papiReadScenario_int(line, "REFSMMATupl", G->REFSMMATupl);
		papiReadScenario_bool(line, "REFSMMATdirect", G->REFSMMATdirect);
		papiReadScenario_bool(line, "REFSMMATHeadsUp", G->REFSMMATHeadsUp);
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
		papiReadScenario_int(line, "TWOIMPULSEMODE", G->twoimpulsemode);
		papiReadScenario_double(line, "lambertelev", G->lambertelev);
		papiReadScenario_vec(line, "CDHdeltaV", G->CDHdeltaV);

		istarget = papiReadScenario_string(line, "TARGET", Buffer2);
		if (istarget)
		{
			OBJHANDLE obj;
			obj = oapiGetObjectByName(Buffer2);
			if (obj)
			{
				G->target = oapiGetVesselInterface(obj);
			}
			istarget = false;
		}

		papiReadScenario_int(line, "TARGETNUMBER", G->targetnumber);
		papiReadScenario_vec(line, "OFFVEC", G->offvec);
		papiReadScenario_double(line, "ANGDEG", G->angdeg);
		papiReadScenario_int(line, "MISSION", G->mission);
		papiReadScenario_double(line, "GETBASE", G->GETbase);
		papiReadScenario_double(line, "LSLat", G->LSLat);
		papiReadScenario_double(line, "LSLng", G->LSLng);
		papiReadScenario_double(line, "LSAlt", G->LSAlt);
		papiReadScenario_double(line, "TLAND", G->t_Land);
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
		papiReadScenario_int(line, "ENTRYCRITICAL", G->entrycritical);
		papiReadScenario_double(line, "ENTRYRANGE", G->entryrange);
		papiReadScenario_bool(line, "ENTRYNOMINAL", G->entrynominal);
		papiReadScenario_bool(line, "ENTRYLONGMANUAL", G->entrylongmanual);
		papiReadScenario_int(line, "LANDINGZONE", G->landingzone);
		papiReadScenario_int(line, "ENTRYPRECISION", G->entryprecision);

		papiReadScenario_double(line, "P37GET400K", G->P37GET400K);
		papiReadScenario_int(line, "MAPPAGE", G->mappage);
		papiReadScenario_bool(line, "INHIBITUPLINK", G->inhibUplLOS);
		papiReadScenario_double(line, "APODESNM", G->apo_desnm);
		papiReadScenario_double(line, "PERIDESNM", G->peri_desnm);
		papiReadScenario_double(line, "INCDEG", G->incdeg);
		papiReadScenario_double(line, "SPSGET", G->SPSGET);
		papiReadScenario_vec(line, "OrbAdjDVX", G->OrbAdjDVX);

		papiReadScenario_int(line, "LOIMANEUVER", G->LOImaneuver);
		papiReadScenario_int(line, "TLCCMANEUVER", G->TLCCmaneuver);
		papiReadScenario_double(line, "TLCCGET", G->TLCC_GET);
		papiReadScenario_double(line, "TLCCPeriGET", G->TLCCPeriGET);
		papiReadScenario_double(line, "TLCCPeriGETcor", G->TLCCPeriGETcor);
		papiReadScenario_double(line, "TLCCReentryGET", G->TLCCReentryGET);
		papiReadScenario_double(line, "TLCCNodeLat", G->TLCCNodeLat);
		papiReadScenario_double(line, "TLCCFreeReturnEMPLat", G->TLCCFreeReturnEMPLat);
		papiReadScenario_double(line, "TLCCNonFreeReturnEMPLat", G->TLCCNonFreeReturnEMPLat);
		papiReadScenario_double(line, "TLCCNodeLng", G->TLCCNodeLng);
		papiReadScenario_double(line, "TLCCLAHPeriAlt", G->TLCCLAHPeriAlt);
		papiReadScenario_double(line, "TLCCFlybyPeriAlt", G->TLCCFlybyPeriAlt);
		papiReadScenario_double(line, "TLCCNodeAlt", G->TLCCNodeAlt);
		papiReadScenario_double(line, "TLCCNodeGET", G->TLCCNodeGET);
		papiReadScenario_double(line, "LOIapo", G->LOIapo);
		papiReadScenario_double(line, "LOIperi", G->LOIperi);
		papiReadScenario_double(line, "LOIazi", G->LOIazi);
		papiReadScenario_vec(line, "TLCCDV", G->TLCC_dV_LVLH);
		papiReadScenario_vec(line, "LOIDV", G->LOI_dV_LVLH);
		papiReadScenario_double(line, "TLCCTIG", G->TLCC_TIG);
		papiReadScenario_double(line, "LOITIG", G->LOI_TIG);
		papiReadScenario_vec(line, "R_TLI", G->R_TLI);
		papiReadScenario_vec(line, "V_TLI", G->V_TLI);

		papiReadScenario_bool(line, "SKYLABNPCOPTION", G->Skylab_NPCOption);
		papiReadScenario_bool(line, "SKYLABPCMANEUVER", G->Skylab_PCManeuver);
		papiReadScenario_int(line, "SKYLABMANEUVER", G->Skylabmaneuver);
		papiReadScenario_double(line, "SKYLAB_N_C", G->Skylab_n_C);
		papiReadScenario_double(line, "SKYLAB_NC1", G->Skylab_t_NC1);
		papiReadScenario_double(line, "SKYLAB_NC2", G->Skylab_t_NC2);
		papiReadScenario_double(line, "SKYLAB_NCC", G->Skylab_t_NCC);
		papiReadScenario_double(line, "SKYLAB_NSR", G->Skylab_t_NSR);
		papiReadScenario_double(line, "t_TPI", G->t_TPI);
		//papiReadScenario_double(line, "SKYLAB_NPC", G->Skylab_t_NPC);
		papiReadScenario_double(line, "SKYLAB_DTTPM", G->Skylab_dt_TPM);
		papiReadScenario_double(line, "SKYLAB_E_L", G->Skylab_E_L);
		papiReadScenario_double(line, "SKYLAB_DH1", G->SkylabDH1);
		papiReadScenario_double(line, "SKYLAB_DH2", G->SkylabDH2);

		papiReadScenario_bool(line, "PCLANDED", G->PClanded);
		papiReadScenario_double(line, "PCALIGNGET", G->PCAlignGET);
		papiReadScenario_double(line, "PCEARLIESTGET", G->PCEarliestGET);
		papiReadScenario_double(line, "PCTIG", G->PC_TIG);
		papiReadScenario_vec(line, "PC_DV_LVLH", G->PC_dV_LVLH);

		papiReadScenario_int(line, "DOI_N", G->DOI_N);
		papiReadScenario_double(line, "DOIGET", G->DOIGET);
		papiReadScenario_double(line, "DOI_PERIANG", G->DOI_PeriAng);

		papiReadScenario_double(line, "DKI_TIG", G->DKI_TIG);
		papiReadScenario_double(line, "t_TPIguess", G->t_TPIguess);
		papiReadScenario_int(line, "DKI_Profile", G->DKI_Profile);
		papiReadScenario_int(line, "DKI_TPI_Mode", G->DKI_TPI_Mode);
		papiReadScenario_bool(line, "DKI_Maneuver_Line", G->DKI_Maneuver_Line);
		papiReadScenario_bool(line, "DKI_Radial_DV", G->DKI_Radial_DV);
		papiReadScenario_int(line, "DKI_N", G->DKI_N);

		papiReadScenario_double(line, "AGSKFACTOR", G->AGSKFactor);

		//G->coreButtons.SelectPage(this, G->screen);
	}
}

// Repaint the MFD
bool ApolloRTCCMFD::Update (oapi::Sketchpad *skp)
{
	Title (skp, "Apollo RTCC MFD");
	skp->SetFont(font);

	/*OBJHANDLE hMoon = oapiGetObjectByName("Moon");
	double lat = 0.17*RAD;
	double lng = 173.57*RAD;
	double MJD = G->GETbase + OrbMech::HHMMSSToSS(75.0, 53.0, 35.0) / 24.0 / 3600.0;
	VECTOR3 R_selen = OrbMech::r_from_latlong(lat, lng);
	MATRIX3 Rot = OrbMech::GetRotationMatrix(hMoon, MJD);
	VECTOR3 R = rhmul(Rot, R_selen);
	MATRIX3 M_EMP = OrbMech::EMPMatrix(MJD);
	VECTOR3 R_EMP = mul(M_EMP, R);
	double lat_EMP, lng_EMP;
	OrbMech::latlong_from_r(R_EMP, lat_EMP, lng_EMP);

	sprintf(oapiDebugString(), "%f %f", lat_EMP*DEG, lng_EMP*DEG);*/

	// Draws the MFD title

	// Add MFD display routines here.
	// Use the device context (hDC) for Windows GDI paint functions.

	//sprintf(Buffer, "%d", G->screen);
	//skp->Text(7.5 * W / 8,(int)(0.5 * H / 14), Buffer, strlen(Buffer));

	if (screen == 0)
	{
		if (G->vesseltype < 2)
		{
			skp->Text(7 * W / 8, (int)(0.5 * H / 14), "CSM", 3);
		}
		else
		{
			skp->Text(7 * W / 8, (int)(0.5 * H / 14), "LM", 2);
		}

		skp->Text(1 * W / 8, 2 * H / 14, "Maneuver Targeting", 18);
		skp->Text(1 * W / 8, 4 * H / 14, "Pre-Advisory Data", 17);
		skp->Text(1 * W / 8, 6 * H / 14, "Utility", 7);
		skp->Text(1 * W / 8, 8 * H / 14, "Configuration", 13);
	}
	else if (screen == 1)
	{
		skp->Text(6 * W / 8, (int)(0.5 * H / 14), "Lambert", 7);

		if (G->twoimpulsemode == 0)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "General", 7);
		}
		else if (G->twoimpulsemode == 1)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "NCC/NSR", 7);
		}
		else
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TPI/TPF", 7);
		}

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

		if (G->twoimpulsemode == 0)
		{
			sprintf(Buffer, "XOFF %.3f NM", G->offvec.x / 1852.0);
			skp->Text(5 * W / 8, 6 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "YOFF %.3f NM", G->offvec.y / 1852.0);
			skp->Text(5 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "ZOFF %.3f NM", G->offvec.z / 1852.0);
			skp->Text(5 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));
		}
		else if (G->twoimpulsemode == 1)
		{
			skp->Text((int)(4.5 * H / 8), 6 * H / 21, "PHASE", 5);
			skp->Text((int)(4.5 * H / 8), 7 * H / 21, "DEL H", 5);
			sprintf(Buffer, "%.3f°", G->TwoImpulse_PhaseAngle*DEG);
			skp->Text(6 * W / 8, 6 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.3f NM", G->DH / 1852.0);
			skp->Text(6 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
		}

		GET_Display(Buffer, G->P30TIG);
		skp->Text(5 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 11 * H / 21, "DVX", 3);
		skp->Text(5 * W / 8, 12 * H / 21, "DVY", 3);
		skp->Text(5 * W / 8, 13 * H / 21, "DVZ", 3);

		AGC_Display(Buffer, G->LambertdeltaV.x / 0.3048);
		skp->Text(6 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->LambertdeltaV.y / 0.3048);
		skp->Text(6 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->LambertdeltaV.z / 0.3048);
		skp->Text(6 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));

		if (G->twoimpulsemode == 1)
		{
			skp->Text(5 * W / 8, 15 * H / 21, "Elevation Angle:", 16);
			sprintf(Buffer, "%.2f°", G->lambertelev*DEG);
			skp->Text(5 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));
			skp->Text(5 * W / 8, 17 * H / 21, "Actual TPI Time:", 16);
			GET_Display(Buffer, G->TwoImpulse_TPI);
			skp->Text(5 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
			skp->Text(5 * W / 8, 19 * H / 21, "Desired TPI Time:", 17);
			GET_Display(Buffer, G->t_TPI);
			skp->Text(5 * W / 8, 20 * H / 21, Buffer, strlen(Buffer));
		}
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
		sprintf(Buffer, "%f NM", G->DH / 1852.0);
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

		if (G->GMPType == 0)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Fixed TIG, specify inclination, apoapsis and periapsis", 54);
		}
		else if (G->GMPType == 1)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Fixed TIG, specify apoapsis altitude", 36);
		}
		else if (G->GMPType == 2)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Fixed TIG, specify periapsis altitude", 37);
		}
		else if (G->GMPType == 3)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Fixed TIG, circularize orbit", 28);
		}
		else if (G->GMPType == 4)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Circularize orbit at specified altitude", 39);
		}
		else if (G->GMPType == 5)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Rotate velocity vector, specify apoapsis", 40);
		}
		else if (G->GMPType == 6)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Rotate line of apsides at periapsis", 35);
		}
		else if (G->GMPType == 7)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Optimal node shift maneuver", 27);
		}

		GET_Display(Buffer, G->SPSGET);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

		if (G->GMPType == 0 || G->GMPType == 1 || G->GMPType == 5)
		{
			sprintf(Buffer, "%f NM", G->apo_desnm);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
		}

		if (G->GMPType == 0 || G->GMPType == 2 || G->GMPType == 4)
		{
			sprintf(Buffer, "%f NM", G->peri_desnm);
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		}

		if (G->GMPType == 0)
		{
			sprintf(Buffer, "%f °", G->incdeg);
			skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->GMPType == 5 || G->GMPType == 7)
		{
			sprintf(Buffer, "%f °", G->GMPRotationAngle*DEG);
			skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->GMPType == 6)
		{
			sprintf(Buffer, "%f °", G->GMPLongitude*DEG);
			skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
		}

		if (G->GMPType == 6)
		{
			sprintf(Buffer, "%d", G->GMPRevs);
			skp->Text(7 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			skp->Text(4 * W / 8, 5 * H / 14, "TOA:", 4);

			GET_Display(Buffer, G->GMPTOA);
			skp->Text(5 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));
		}

		if (G->OrbAdjAltRef == 0)
		{
			skp->Text(1 * W / 8, 12 * H / 14, "Mean radius", 11);
		}
		else
		{
			skp->Text(1 * W / 8, 12 * H / 14, "Launch Pad/Landing Site", 23);
		}

		GET_Display(Buffer, G->P30TIG);
		skp->Text(5 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));

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

		if (G->REFSMMATopt != 7 && G->REFSMMATopt != 8 && G->REFSMMATdirect == false)
		{
			GET_Display(Buffer, G->P30TIG);
			skp->Text(1 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVX", G->dV_LVLH.x / 0.3048);
			skp->Text(1 * W / 8, 14 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVY", G->dV_LVLH.y / 0.3048);
			skp->Text(1 * W / 8, 15 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVZ", G->dV_LVLH.z / 0.3048);
			skp->Text(1 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));
		}

		if (G->REFSMMATopt == 0) //P30 Maneuver
		{
			if (G->REFSMMATHeadsUp)
			{
				skp->Text(5 * W / 8, 2 * H / 14, "P30 (Heads up)", 14);
			}
			else
			{
				skp->Text(5 * W / 8, 2 * H / 14, "P30 (Heads down)", 16);
			}

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
		else if (G->REFSMMATopt == 5 || G->REFSMMATopt == 8)
		{
			GET_Display(Buffer, G->t_Land);
			skp->Text((int)(0.5 * W / 8), 2 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%f°", G->LSLat*DEG);
			skp->Text((int)(5.5 * W / 8), 8 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%f°", G->LSLng*DEG);
			skp->Text((int)(5.5 * W / 8), 10 * H / 14, Buffer, strlen(Buffer));

			if (G->REFSMMATopt == 8)
			{
				skp->Text(5 * W / 8, 2 * H / 14, "LS during TLC", 13);

				skp->Text((int)(5.5 * W / 8), 11 * H / 14, "Azimuth:", 8);
				sprintf(Buffer, "%f°", G->LOIazi*DEG);
				skp->Text((int)(5.5 * W / 8), 12 * H / 14, Buffer, strlen(Buffer));
			}
			else
			{
				skp->Text(5 * W / 8, 2 * H / 14, "Landing Site", 12);

				if (G->REFSMMATdirect == true)
				{
					skp->Text((int)(0.5 * W / 8), 12 * H / 14, "Direct", 6);
				}
				else
				{
					skp->Text((int)(0.5 * W / 8), 12 * H / 14, "MCC", 3);
				}
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

			skp->Text(1 * W / 8, 8 * H / 21, "MCC", 3);
			GET_Display(Buffer, G->TLCC_TIG);
			skp->Text(1 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVX", G->TLCC_dV_LVLH.x / 0.3048);
			skp->Text(1 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVY", G->TLCC_dV_LVLH.y / 0.3048);
			skp->Text(1 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVZ", G->TLCC_dV_LVLH.z / 0.3048);
			skp->Text(1 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));

			skp->Text(1 * W / 8, 14 * H / 21, "LOI-1", 5);
			GET_Display(Buffer, G->LOI_TIG);
			skp->Text(1 * W / 8, 15 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVX", G->LOI_dV_LVLH.x / 0.3048);
			skp->Text(1 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVY", G->LOI_dV_LVLH.y / 0.3048);
			skp->Text(1 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVZ", G->LOI_dV_LVLH.z / 0.3048);
			skp->Text(1 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
		}
		else if (G->REFSMMATopt == 9)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "REFS from Attitude", 18);

			skp->Text((int)(0.5 * W / 8), 9 * H / 21, "Current REFSMMAT:", 17);
			REFSMMATName(Buffer, G->REFSMMATcur);
			skp->Text((int)(0.5 * W / 8), 10 * H / 21, Buffer, strlen(Buffer));

			skp->Text((int)(0.5 * W / 8), 12 * H / 21, "Attitude:", 9);
			sprintf(Buffer, "%+07.2f R", G->VECangles.x*DEG);
			skp->Text((int)(0.5 * W / 8), 13 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.2f P", G->VECangles.y*DEG);
			skp->Text((int)(0.5 * W / 8), 14 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.2f Y", G->VECangles.z*DEG);
			skp->Text((int)(0.5 * W / 8), 15 * H / 21, Buffer, strlen(Buffer));
		}

		for (int i = 0; i < 20; i++)
		{
			sprintf(Buffer, "%05d", G->REFSMMAToct[i]);
			skp->Text(4 * W / 8, (6+i) * H / 28, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 6)
	{
		skp->Text(6 * W / 8, (int)(0.5 * H / 14), "Entry Options", 13);

		skp->Text(1 * W / 8, 2 * H / 14, "Deorbit Maneuver", 16);
		skp->Text(1 * W / 8, 4 * H / 14, "Return to Earth (Earth-centered)", 32);
		skp->Text(1 * W / 8, 6 * H / 14, "Return to Earth (Moon-centered)", 31);
		skp->Text(1 * W / 8, 8 * H / 14, "Transearth Injection", 20);
		skp->Text(1 * W / 8, 10 * H / 14, "Splashdown Update", 17);
	}
	else if (screen == 7)
	{
		if (G->svmode == 0)
		{
			skp->Text(5 * W / 8, (int)(0.5 * H / 14), "State Vector", 12);
		}
		else if (G->svmode == 1)
		{
			skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Landing Site Update", 19);
		}
		else if (G->svmode == 2)
		{
			skp->Text(4 * W / 8, (int)(0.5 * H / 14), "AGS State Vector Update", 23);
		}

		if (G->svmode == 0 || G->svmode == 2)
		{
			if (G->SVSlot)
			{
				skp->Text((int)(0.5 * W / 8), 10 * H / 14, "CSM", 3);
			}
			else
			{
				skp->Text((int)(0.5 * W / 8), 10 * H / 14, "LM", 2);
			}
		}

		if (G->svmode == 0)
		{
			if (!G->svtimemode)
			{
				skp->Text((int)(0.5 * W / 8), 2 * H / 14, "Now", 3);
			}
			else
			{
				skp->Text((int)(0.5 * W / 8), 2 * H / 14, "GET", 3);
				GET_Display(Buffer, G->J2000GET);
				skp->Text((int)(0.5 * W / 8), 4 * H / 14, Buffer, strlen(Buffer));
			}
		}

		if (G->svtarget != NULL)
		{
			sprintf(Buffer, G->svtarget->GetName());
			skp->Text((int)(0.5 * W / 8), 8 * H / 14, Buffer, strlen(Buffer));
		}

		if (G->svmode == 0)
		{
			sprintf(Buffer, "%f", G->J2000Pos.x);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%f", G->J2000Pos.y);
			skp->Text(5 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%f", G->J2000Pos.z);
			skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%f", G->J2000Vel.x);
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%f", G->J2000Vel.y);
			skp->Text(5 * W / 8, 9 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%f", G->J2000Vel.z);
			skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%f", G->J2000GET);
			skp->Text(5 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->svmode == 1)
		{
			sprintf(Buffer, "%.3f°", G->LSLat*DEG);
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.3f°", G->LSLng*DEG);
			skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->LSAlt / 1852.0);
			skp->Text(5 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->svmode == 2)
		{
			skp->Text((int)(0.5 * W / 8), 5 * H / 14, "REFSMMAT:", 9);
			REFSMMATName(Buffer, G->REFSMMATcur);
			skp->Text((int)(0.5 * W / 8), 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.1f h", G->AGSKFactor / 3600.0);
			skp->Text((int)(0.5 * W / 8), 12 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA240);
			skp->Text(4 * W / 8, 4 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 4 * H / 21, "240", 3);
			sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA241);
			skp->Text(4 * W / 8, 5 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 5 * H / 21, "241", 3);
			sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA242);
			skp->Text(4 * W / 8, 6 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 6 * H / 21, "242", 3);

			sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA260);
			skp->Text(4 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 7 * H / 21, "260", 3);
			sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA261);
			skp->Text(4 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 8 * H / 21, "261", 3);
			sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA262);
			skp->Text(4 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 9 * H / 21, "262", 3);

			sprintf(Buffer, "%+07.1f", G->agssvpad.DEDA254);
			skp->Text(4 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 10 * H / 21, "254", 3);

			sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA244);
			skp->Text(4 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 11 * H / 21, "244", 3);
			sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA245);
			skp->Text(4 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 12 * H / 21, "245", 3);
			sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA246);
			skp->Text(4 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 13 * H / 21, "246", 3);

			sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA264);
			skp->Text(4 * W / 8, 14 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 14 * H / 21, "264", 3);
			sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA265);
			skp->Text(4 * W / 8, 15 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 15 * H / 21, "265", 3);
			sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA266);
			skp->Text(4 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 16 * H / 21, "266", 3);

			sprintf(Buffer, "%+07.1f", G->agssvpad.DEDA272);
			skp->Text(4 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
			skp->Text(6 * W / 8, 17 * H / 21, "272", 3);
		}
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
		sprintf(Buffer, "Launch MJD: %f", G->GETbase);
		skp->Text(4 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "AGC Epoch: %f", G->AGCEpoch);
		skp->Text(4 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

		skp->Text(4 * W / 8, 6 * H / 14, "Update Liftoff MJD", 18);

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

		if (G->vesseltype >= 2)
		{
			if (G->lemdescentstage)
			{
				skp->Text(1 * W / 8, 6 * H / 14, "Descent Stage", 13);
			}
			else
			{
				skp->Text(1 * W / 8, 6 * H / 14, "Ascent Stage", 12);
			}
		}

		skp->Text(1 * W / 8, 8 * H / 14, "Sxt/Star Check:", 15);
		sprintf(Buffer, "%.0f min", G->sxtstardtime);
		skp->Text(4 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

		skp->Text(1 * W / 8, 10 * H / 14, "Uplink in LOS:", 14);

		if (G->inhibUplLOS)
		{
			skp->Text(4 * W / 8, 10 * H / 14, "Inhibit", 7);
		}
		else
		{
			skp->Text(4 * W / 8, 10 * H / 14, "Enabled", 7);
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
			if (G->HeadsUp)
			{
				skp->Text((int)(0.5 * W / 8), 6 * H / 14, "Heads Up", 8);
			}
			else
			{
				skp->Text((int)(0.5 * W / 8), 6 * H / 14, "Heads Down", 10);
			}

			skp->Text((int)(0.5 * W / 8), 8 * H / 14, "REFSMMAT:", 9);

			REFSMMATName(Buffer, G->REFSMMATcur);
			skp->Text((int)(0.5 * W / 8), 9 * H / 14, Buffer, strlen(Buffer));

			if (G->vesseltype < 2)
			{
				skp->Text(5 * W / 8, (int)(0.5 * H / 14), "P30 Maneuver", 12);

				if (G->vesseltype == 0)
				{
					skp->Text((int)(0.5 * W / 8), 2 * H / 14, "CSM", 3);
				}
				else
				{
					skp->Text((int)(0.5 * W / 8), 2 * H / 14, "CSM/LM", 6);
				}

				if (G->enginetype == RTCC_ENGINETYPE_SPSDPS)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "SPS", 3);
				}
				else if (G->enginetype == RTCC_ENGINETYPE_RCS && G->directiontype == RTCC_DIRECTIONTYPE_PLUSX)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "RCS +X", 6);
				}
				else if (G->enginetype == RTCC_ENGINETYPE_RCS && G->directiontype == RTCC_DIRECTIONTYPE_MINUSX)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "RCS -X", 6);
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

				if (G->enginetype == RTCC_ENGINETYPE_SPSDPS)
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
			else
			{
				skp->Text(5 * W / 8, (int)(0.5 * H / 14), "P30 LM Maneuver", 15);

				if (G->enginetype == 1 && G->lemdescentstage)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "DPS", 3);
				}
				else if (G->enginetype == 1 && !G->lemdescentstage)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "APS", 3);
				}
				else if (G->enginetype == 0 && G->directiontype == RTCC_DIRECTIONTYPE_PLUSX)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "RCS +X", 6);
				}
				else if (G->enginetype == 0 && G->directiontype == RTCC_DIRECTIONTYPE_MINUSX)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "RCS -X", 6);
				}

				if (G->vesseltype == 2)
				{
					skp->Text((int)(0.5 * W / 8), 2 * H / 14, "LM", 3);
				}
				else
				{
					skp->Text((int)(0.5 * W / 8), 2 * H / 14, "LM/CSM", 6);
				}

				sprintf(Buffer, "LM Weight: %5.0f", G->lmmanpad.LMWeight);
				skp->Text((int)(0.5 * W / 8), 10 * H / 14, Buffer, strlen(Buffer));

				if (G->vesseltype == 3)
				{
					sprintf(Buffer, "CSM Weight: %5.0f", G->lmmanpad.CSMWeight);
					skp->Text((int)(0.5 * W / 8), 11 * H / 14, Buffer, strlen(Buffer));
				}

				int hh, mm;
				double secs;

				SStoHHMMSS(G->P30TIG, hh, mm, secs);

				sprintf(Buffer, "%+06d HRS GETI", hh);
				skp->Text((int)(3.5 * W / 8), 5 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+06d MIN", mm);
				skp->Text((int)(3.5 * W / 8), 6 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+06.0f SEC", secs * 100.0);
				skp->Text((int)(3.5 * W / 8), 7 * H / 26, Buffer, strlen(Buffer));

				sprintf(Buffer, "%+07.1f DVX", G->dV_LVLH.x / 0.3048);
				skp->Text((int)(3.5 * W / 8), 8 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f DVY", G->dV_LVLH.y / 0.3048);
				skp->Text((int)(3.5 * W / 8), 9 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f DVZ", G->dV_LVLH.z / 0.3048);
				skp->Text((int)(3.5 * W / 8), 10 * H / 26, Buffer, strlen(Buffer));

				sprintf(Buffer, "%+07.1f HA", min(9999.9, G->lmmanpad.HA));
				skp->Text((int)(3.5 * W / 8), 11 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f HP", G->lmmanpad.HP);
				skp->Text((int)(3.5 * W / 8), 12 * H / 26, Buffer, strlen(Buffer));

				sprintf(Buffer, "%+07.1f DVR", length(G->dV_LVLH) / 0.3048);
				skp->Text((int)(3.5 * W / 8), 13 * H / 26, Buffer, strlen(Buffer));

				SStoHHMMSS(G->lmmanpad.burntime, hh, mm, secs);

				sprintf(Buffer, "XXX%d:%02.0f BT", mm, secs);
				skp->Text((int)(3.5 * W / 8), 14 * H / 26, Buffer, strlen(Buffer));

				sprintf(Buffer, "XXX%03.0f R", G->lmmanpad.Att.x);
				skp->Text((int)(3.5 * W / 8), 15 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f P", G->lmmanpad.Att.y);
				skp->Text((int)(3.5 * W / 8), 16 * H / 26, Buffer, strlen(Buffer));

				sprintf(Buffer, "%+07.1f DVX AGS N86", G->lmmanpad.dV_AGS.x);
				skp->Text((int)(3.5 * W / 8), 17 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f DVY AGS", G->lmmanpad.dV_AGS.y);
				skp->Text((int)(3.5 * W / 8), 18 * H / 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f DVZ AGS", G->lmmanpad.dV_AGS.z);
				skp->Text((int)(3.5 * W / 8), 19 * H / 26, Buffer, strlen(Buffer));

				if (G->lmmanpad.BSSStar == 0)
				{
					sprintf(Buffer, "N/A     BSS");
					skp->Text((int)(3.5 * W / 8), 20 * H / 26, Buffer, strlen(Buffer));
					sprintf(Buffer, "N/A     SPA");
					skp->Text((int)(3.5 * W / 8), 21 * H / 26, Buffer, strlen(Buffer));
					sprintf(Buffer, "N/A     SXP");
					skp->Text((int)(3.5 * W / 8), 22 * H / 26, Buffer, strlen(Buffer));
				}
				else
				{
					sprintf(Buffer, "XXXX%02d BSS", G->lmmanpad.BSSStar);
					skp->Text((int)(3.5 * W / 8), 20 * H / 26, Buffer, strlen(Buffer));
					sprintf(Buffer, "%+07.2f SPA", G->lmmanpad.SPA);
					skp->Text((int)(3.5 * W / 8), 21 * H / 26, Buffer, strlen(Buffer));
					sprintf(Buffer, "%+07.3f SXP", G->lmmanpad.SXP);
					skp->Text((int)(3.5 * W / 8), 22 * H / 26, Buffer, strlen(Buffer));
				}

				skp->Text((int)(0.5 * W / 8), 24 * H / 26, G->lmmanpad.remarks, strlen(G->lmmanpad.remarks));
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
				sprintf(Buffer, "F%04.1f/%02.0f DVX LOS/BT", abs(G->TPIPAD_dV_LOS.x), G->TPIPAD_BT.x);
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
			if (G->vesseltype < 2)
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

				sprintf(Buffer, "XXX%03.0f R", G->tlipad.SepATT.x);
				skp->Text(3 * W / 8, 10 * H / 20, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f P SEP", G->tlipad.SepATT.y);
				skp->Text(3 * W / 8, 11 * H / 20, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f Y", G->tlipad.SepATT.z);
				skp->Text(3 * W / 8, 12 * H / 20, Buffer, strlen(Buffer));

				sprintf(Buffer, "XXX%03.0f R", G->tlipad.ExtATT.x);
				skp->Text(3 * W / 8, 13 * H / 20, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f P EXTRACTION", G->tlipad.ExtATT.y);
				skp->Text(3 * W / 8, 14 * H / 20, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f Y", G->tlipad.ExtATT.z);
				skp->Text(3 * W / 8, 15 * H / 20, Buffer, strlen(Buffer));
			}
			else
			{
				skp->Text(5 * W / 8, (int)(0.5 * H / 14), "PDI PAD", 7);

				if (G->HeadsUp)
				{
					skp->Text((int)(0.5 * W / 8), 6 * H / 14, "Heads Up", 8);
				}
				else
				{
					skp->Text((int)(0.5 * W / 8), 6 * H / 14, "Heads Down", 10);
				}

				if (G->REFSMMATcur != 5 && G->REFSMMATcur != 8)
				{
					skp->Text((int)(0.5 * W / 8), 2 * H / 14, "No LS REFSMMAT!", 15);
				}

				if (!G->PDIPADdirect)
				{
					skp->Text((int)(0.5 * W / 8), 14 * H / 20, "DOI:", 4);

					GET_Display(Buffer, G->P30TIG);
					skp->Text((int)(0.5 * W / 8), 15 * H / 20, Buffer, strlen(Buffer));

					AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
					skp->Text((int)(0.5 * W / 8), 16 * H / 20, Buffer, strlen(Buffer));
					AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
					skp->Text((int)(0.5 * W / 8), 17 * H / 20, Buffer, strlen(Buffer));
					AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
					skp->Text((int)(0.5 * W / 8), 18 * H / 20, Buffer, strlen(Buffer));
				}

				skp->Text(4 * W / 8, 15 * H / 20, "T_L:", 4);
				GET_Display(Buffer, G->t_Land);
				skp->Text(5 * W / 8, 15 * H / 20, Buffer, strlen(Buffer));

				skp->Text(4 * W / 8, 16 * H / 20, "Lat:", 4);
				sprintf(Buffer, "%.3f°", G->LSLat*DEG);
				skp->Text(5 * W / 8, 16 * H / 20, Buffer, strlen(Buffer));

				skp->Text(4 * W / 8, 17 * H / 20, "Lng:", 4);
				sprintf(Buffer, "%.3f°", G->LSLng*DEG);
				skp->Text(5 * W / 8, 17 * H / 20, Buffer, strlen(Buffer));

				skp->Text(4 * W / 8, 18 * H / 20, "Alt:", 4);
				sprintf(Buffer, "%.2f NM", G->LSAlt / 1852.0);
				skp->Text(5 * W / 8, 18 * H / 20, Buffer, strlen(Buffer));

				if (!G->PADSolGood)
				{
					skp->Text(5 * W / 8, 2 * H / 14, "Calculation failed!", 19);
				}

				int hh, mm; // ss;
				double secs;

				SStoHHMMSS(G->pdipad.GETI, hh, mm, secs);

				skp->Text(3 * W / 8, 5 * H / 20, "HRS", 3);
				skp->Text((int)(4.5 * W / 8), 5 * H / 20, "TIG", 3);
				sprintf(Buffer, "%+06d", hh);
				skp->Text(6 * W / 8, 5 * H / 20, Buffer, strlen(Buffer));

				skp->Text(3 * W / 8, 6 * H / 20, "MIN", 3);
				skp->Text((int)(4.5 * W / 8), 6 * H / 20, "PDI", 3);
				sprintf(Buffer, "%+06d", mm);
				skp->Text(6 * W / 8, 6 * H / 20, Buffer, strlen(Buffer));

				skp->Text(3 * W / 8, 7 * H / 20, "SEC", 3);
				sprintf(Buffer, "%+06.0f", secs * 100.0);
				skp->Text(6 * W / 8, 7 * H / 20, Buffer, strlen(Buffer));

				SStoHHMMSS(G->pdipad.t_go, hh, mm, secs);
				skp->Text(3 * W / 8, 8 * H / 20, "TGO", 3);
				skp->Text((int)(4.5 * W / 8), 8 * H / 20, "N61", 3);
				sprintf(Buffer, "XX%02d:%02.0f", mm, secs);
				skp->Text(6 * W / 8, 8 * H / 20, Buffer, strlen(Buffer));

				skp->Text(3 * W / 8, 9 * H / 20, "CROSSRANGE", 10);
				sprintf(Buffer, "%07.1f", G->pdipad.CR);
				skp->Text(6 * W / 8, 9 * H / 20, Buffer, strlen(Buffer));

				skp->Text(3 * W / 8, 10 * H / 20, "R", 1);
				skp->Text((int)(4.5 * W / 8), 10 * H / 20, "FDAI", 4);
				sprintf(Buffer, "XXX%03.0f", G->pdipad.Att.x);
				skp->Text(6 * W / 8, 10 * H / 20, Buffer, strlen(Buffer));

				skp->Text(3 * W / 8, 11 * H / 20, "P", 1);
				skp->Text((int)(4.5 * W / 8), 11 * H / 20, "AT TIG", 6);
				sprintf(Buffer, "XXX%03.0f", G->pdipad.Att.y);
				skp->Text(6 * W / 8, 11 * H / 20, Buffer, strlen(Buffer));

				skp->Text(3 * W / 8, 12 * H / 20, "Y", 1);
				sprintf(Buffer, "XXX%03.0f", G->pdipad.Att.z);
				skp->Text(6 * W / 8, 12 * H / 20, Buffer, strlen(Buffer));

				skp->Text(3 * W / 8, 13 * H / 20, "DEDA 231 IF RQD", 15);
				sprintf(Buffer, "%+06.0f", G->pdipad.DEDA231);
				skp->Text(6 * W / 8, 13 * H / 20, Buffer, strlen(Buffer));
			}
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

			GET_Display(Buffer2, G->mapupdate.LOSGET);
			sprintf(Buffer, "LOS %s", Buffer2);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer2, G->mapupdate.SRGET);
			sprintf(Buffer, "SR  %s", Buffer2);
			skp->Text(1 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer2, G->mapupdate.PMGET);
			sprintf(Buffer, "PM  %s", Buffer2);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer2, G->mapupdate.AOSGET);
			sprintf(Buffer, "AOS %s", Buffer2);
			skp->Text(1 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer2, G->mapupdate.SSGET);
			sprintf(Buffer, "SS  %s", Buffer2);
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 12)
	{
		skp->Text(6 * W / 8, (int)(0.5 * H / 14), "Lunar Insertion", 15);

		if (G->LOImaneuver == 0 || G->LOImaneuver == 1)
		{
			if (G->LOImaneuver == 0)
			{
				skp->Text(1 * W / 8, 2 * H / 14, "LOI-1 (w/ MCC)", 14);

				if (G->subThreadStatus > 0)
				{
					skp->Text(5 * W / 8, 2 * H / 14, "Calculating...", 14);
				}
				else
				{
					skp->Text(5 * W / 8, 2 * H / 21, "MCC", 3);
					GET_Display(Buffer, G->TLCC_TIG);
					skp->Text(5 * W / 8, 3 * H / 21, Buffer, strlen(Buffer));

					sprintf(Buffer, "%+07.1f DVT", length(G->TLCC_dV_LVLH) / 0.3048);
					skp->Text(5 * W / 8, 4 * H / 21, Buffer, strlen(Buffer));
				}
			}
			else
			{
				skp->Text(1 * W / 8, 2 * H / 14, "LOI-1 (w/o MCC)", 15);

				if (G->subThreadStatus > 0)
				{
					skp->Text(5 * W / 8, 2 * H / 14, "Calculating...", 14);
				}
			}

			if (G->LOIOption == 0)
			{
				skp->Text(1 * W / 8, 4 * H / 14, "Fixed LPO", 9);
			}
			else
			{
				skp->Text(1 * W / 8, 4 * H / 14, "LOI-1 at Pericynthion", 21);
			}

			GET_Display(Buffer, G->t_Land);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->LOIapo / 1852.0);
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->LOIperi / 1852.0);
			skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			if (G->LOIOption == 0)
			{
				sprintf(Buffer, "%.3f°", G->LOIazi*DEG);
				skp->Text(1 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
			}

			sprintf(Buffer, "%.3f°", G->LSLat*DEG);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.3f°", G->LSLng*DEG);
			skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->LSAlt / 1852.0);
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

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
			skp->Text(1 * W / 8, 2 * H / 14, "LOI-2", 5);

			if (G->subThreadStatus > 0)
			{
				skp->Text(5 * W / 8, 2 * H / 14, "Calculating...", 14);
			}

			sprintf(Buffer, "%.2f NM", G->LOI2Alt / 1852.0);
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

		GET_Display(Buffer2, G->landmarkpad.T1[0]);
		sprintf(Buffer, "T1: %s (HOR)", Buffer2);
		skp->Text(4 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer2, G->landmarkpad.T2[0]);
		sprintf(Buffer, "T2: %s (35°)", Buffer2);
		skp->Text(4 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));

		if (G->landmarkpad.CRDist[0] > 0)
		{
			sprintf(Buffer, "%.1f NM North", G->landmarkpad.CRDist[0] / 1852.0);
		}
		else
		{
			sprintf(Buffer, "%.1f NM South", abs(G->landmarkpad.CRDist[0]) / 1852.0);
		}
		
		skp->Text(4 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		skp->Text(4 * W / 8, 9 * H / 14, "N89", 3);
		sprintf(Buffer, "Lat %+07.3f°", G->landmarkpad.Lat[0]);
		skp->Text(4 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "Long/2 %+07.3f°", G->landmarkpad.Lng05[0]);
		skp->Text(4 * W / 8, 11 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "Alt %+07.2f NM", G->landmarkpad.Alt[0]);
		skp->Text(4 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 14)
	{
		if (G->vesseltype < 2)
		{
			skp->Text(7 * W / 8, (int)(0.5 * H / 14), "CSM", 3);
		}
		else
		{
			skp->Text(7 * W / 8, (int)(0.5 * H / 14), "LM", 2);
		}

		skp->Text(1 * W / 8, 2 * H / 14, "Rendezvous", 10);
		skp->Text(1 * W / 8, 4 * H / 14, "Orbit Adjustment", 16);
		skp->Text(1 * W / 8, 6 * H / 14, "Translunar", 10);
		skp->Text(1 * W / 8, 8 * H / 14, "Lunar Insertion", 15);
		skp->Text(1 * W / 8, 10 * H / 14, "Entry", 5);

		skp->Text(5 * W / 8, 2 * H / 14, "DOI", 3);
		skp->Text(5 * W / 8, 4 * H / 14, "Plane Change", 12);
		skp->Text(5 * W / 8, 6 * H / 14, "Lunar Liftoff", 13);
		skp->Text(5 * W / 8, 12 * H / 14, "Previous Page", 13);
	}
	else if (screen == 15)
	{
		if (G->VECoption == 0)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Point SC at body", 16);
		}
		else
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Open hatch thermal control", 26);
		}

		if (G->VECoption == 0)
		{
			if (G->VECbody != NULL)
			{
				oapiGetObjectName(G->VECbody, Buffer, 20);
				skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
			}

			if (G->VECdirection == 0)
			{
				skp->Text(1 * W / 8, 6 * H / 14, "+X", 2);
			}
			else if (G->VECdirection == 1)
			{
				skp->Text(1 * W / 8, 6 * H / 14, "-X", 2);
			}
			else if (G->VECdirection == 2)
			{
				skp->Text(1 * W / 8, 6 * H / 14, "+Y", 2);
			}
			else if (G->VECdirection == 3)
			{
				skp->Text(1 * W / 8, 6 * H / 14, "-Y", 2);
			}
			else if (G->VECdirection == 4)
			{
				skp->Text(1 * W / 8, 6 * H / 14, "+Z", 2);
			}
			else if (G->VECdirection == 5)
			{
				skp->Text(1 * W / 8, 6 * H / 14, "-Z", 2);
			}
		}

		sprintf(Buffer, "%+07.2f R", G->VECangles.x*DEG);
		skp->Text(6 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.2f P", G->VECangles.y*DEG);
		skp->Text(6 * W / 8, 11 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.2f Y", G->VECangles.z*DEG);
		skp->Text(6 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 16)
	{
		if (G->DOI_option == 0)
		{
			skp->Text(5 * W / 8, (int)(0.5 * H / 14), "DOI from LPO", 12);
		}
		else
		{
			skp->Text(5 * W / 8, (int)(0.5 * H / 14), "DOI as LOI-2", 12);
		}

		GET_Display(Buffer, G->DOIGET);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%d", G->DOI_N);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.3f°", G->DOI_PeriAng*DEG);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.3f°", G->LSLat*DEG);
		skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.3f°", G->LSLng*DEG);
		skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
		
		sprintf(Buffer, "%.2f NM", G->LSAlt / 1852.0);
		skp->Text(1 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 6 * H / 14, "Uplink TLAND", 12);

		skp->Text(4 * W / 8, 11 * H / 21, "DOI:", 4);
		skp->Text(4 * W / 8, 12 * H / 21, "PDI:", 4);
		skp->Text(4 * W / 8, 13 * H / 21, "t_L:", 4);
		skp->Text(4 * W / 8, 14 * H / 21, "CR:", 3);

		GET_Display(Buffer, G->DOI_TIG);
		skp->Text(5 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));

		GET_Display(Buffer, G->DOI_t_PDI);
		skp->Text(5 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));

		GET_Display(Buffer, G->t_Land);
		skp->Text(5 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.1f NM", G->DOI_CR / 1852.0);
		skp->Text(5 * W / 8, 14 * H / 21, Buffer, strlen(Buffer));

		skp->Text(4 * W / 8, 16 * H / 21, "DVX", 3);
		skp->Text(4 * W / 8, 17 * H / 21, "DVY", 3);
		skp->Text(4 * W / 8, 18 * H / 21, "DVZ", 3);

		sprintf(Buffer, "%+07.1f", G->DOI_dV_LVLH.x / 0.3048);
		skp->Text(6 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f", G->DOI_dV_LVLH.y / 0.3048);
		skp->Text(6 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f", G->DOI_dV_LVLH.z / 0.3048);
		skp->Text(6 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));

		sprintf(Buffer, "%+07.1f", length(G->DOI_dV_LVLH) / 0.3048);
		skp->Text(6 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));
	}
	else if (screen == 17)
	{
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Skylab Rendezvous", 17);

		if (G->target != NULL)
		{
			sprintf(Buffer, G->target->GetName());
			skp->Text(5 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		}

		if (G->Skylabmaneuver != 0)
		{
			skp->Text(4 * W / 8, 16 * H / 21, "TIG", 3);
			GET_Display(Buffer, G->P30TIG);
			skp->Text(5 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));
			
			sprintf(Buffer, "%+07.1f", G->dV_LVLH.x / 0.3048);
			skp->Text(5 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->dV_LVLH.y / 0.3048);
			skp->Text(5 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->dV_LVLH.z / 0.3048);
			skp->Text(5 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));
		}

		if (G->Skylabmaneuver < 7)
		{
			skp->Text(4 * W / 8, 5 * H / 21, "TPI", 3);
			GET_Display(Buffer, G->t_TPI);
			skp->Text(5 * W / 8, 5 * H / 21, Buffer, strlen(Buffer));
		}

		if (!G->SkylabSolGood)
		{
			skp->Text(3 * W / 8, 7 * H / 14, "Calculation failed!", 19);
		}

		if (G->Skylabmaneuver == 0)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TPI Search", 10);

			GET_Display(Buffer, G->SkylabTPIGuess);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		}
		if (G->Skylabmaneuver == 1 || G->Skylabmaneuver == 2)
		{
			sprintf(Buffer, "%.1f NM", G->SkylabDH2 / 1852.0);
			skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f°", G->Skylab_E_L*DEG);
			skp->Text(1 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));

			skp->Text(4 * W / 8, 10 * H / 21, "NCC", 3);
			GET_Display(Buffer, G->Skylab_t_NCC);
			skp->Text(5 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f ft/s", G->Skylab_dv_NCC / 0.3048);
			skp->Text(5 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));

			skp->Text(4 * W / 8, 12 * H / 21, "NSR", 3);
			GET_Display(Buffer, G->Skylab_t_NSR);
			skp->Text(5 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f ft/s", length(G->Skylab_dV_NSR) / 0.3048);
			skp->Text(5 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));
		}
		if (G->Skylabmaneuver == 1)
		{
			if (G->Skylab_NPCOption)
			{
				skp->Text(1 * W / 8, 2 * H / 14, "NC1 with Plane Change", 21);
			}
			else
			{
				skp->Text(1 * W / 8, 2 * H / 14, "NC1", 3);
			}

			GET_Display(Buffer, G->Skylab_t_NC1);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
			
			sprintf(Buffer, "%.1f", G->Skylab_n_C);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.1f NM", G->SkylabDH1 / 1852.0);
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			skp->Text(4 * W / 8, 7 * H / 21, "NC2", 3);
			GET_Display(Buffer, G->Skylab_t_NC2);
			skp->Text(5 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f ft/s", G->Skylab_dv_NC2 / 0.3048);
			skp->Text(5 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "DH: %.1f NM", G->Skylab_dH_NC2 / 1852.0);
			skp->Text(5 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));

			skp->Text(4 * W / 8, 14 * H / 21, "DVT", 3);
			sprintf(Buffer, "%+07.1f ft/s", (length(G->dV_LVLH) + G->Skylab_dv_NC2 + G->Skylab_dv_NCC + length(G->Skylab_dV_NSR)) / 0.3048);
			skp->Text(5 * W / 8, 14 * H / 21, Buffer, strlen(Buffer));
		}
		if (G->Skylabmaneuver == 2)
		{
			if (G->Skylab_NPCOption)
			{
				skp->Text(1 * W / 8, 2 * H / 14, "NC2 with Plane Change", 21);
			}
			else
			{
				skp->Text(1 * W / 8, 2 * H / 14, "NC2", 3);
			}

			GET_Display(Buffer, G->Skylab_t_NC2);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			skp->Text(4 * W / 8, 14 * H / 21, "DVT", 3);
			sprintf(Buffer, "%+07.1f ft/s", (length(G->dV_LVLH) + G->Skylab_dv_NCC + length(G->Skylab_dV_NSR)) / 0.3048);
			skp->Text(5 * W / 8, 14 * H / 21, Buffer, strlen(Buffer));
		}
		if (G->Skylabmaneuver == 3)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "NCC", 3);

			GET_Display(Buffer, G->Skylab_t_NCC);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.1f NM", G->SkylabDH2 / 1852.0);
			skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f°", G->Skylab_E_L*DEG);
			skp->Text(1 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));

			skp->Text(4 * W / 8, 10 * H / 21, "NSR", 3);
			GET_Display(Buffer, G->Skylab_t_NSR);
			skp->Text(5 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->Skylab_dV_NSR.x / 0.3048);
			skp->Text(5 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->Skylab_dV_NSR.y / 0.3048);
			skp->Text(5 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->Skylab_dV_NSR.z / 0.3048);
			skp->Text(5 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));
		}
		if (G->Skylabmaneuver == 4)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "NSR", 3);

			GET_Display(Buffer, G->Skylab_t_NSR);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f°", G->Skylab_E_L*DEG);
			skp->Text(1 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
		}
		if (G->Skylabmaneuver == 5)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TPI", 3);

			skp->Text(1 * W / 8, 4 * H / 14, "Calculate TPI TIG", 17);

			sprintf(Buffer, "%.2f°", G->Skylab_E_L*DEG);
			skp->Text(1 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
		}
		if (G->Skylabmaneuver == 6)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TPM", 3);

			sprintf(Buffer, "DT = %.1f mins", G->Skylab_dt_TPM / 60.0);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		}
		if (G->Skylabmaneuver == 7)
		{
			if (G->Skylab_PCManeuver == 0)
			{
				skp->Text(1 * W / 8, 2 * H / 14, "NPC after NC1", 13);
			}
			else
			{
				skp->Text(1 * W / 8, 2 * H / 14, "NPC after NC2", 13);
			}
		}
	}
	else if (screen == 18)
	{
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Plane Change", 12);

		GET_Display(Buffer, G->PCEarliestGET);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->PCAlignGET);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

		if (G->PClanded)
		{
			skp->Text(1 * W / 8, 6 * H / 14, "LM on surface", 13);
		}
		else
		{
			skp->Text(1 * W / 8, 6 * H / 14, "Coordinates", 11);
		}

		if (G->target != NULL)
		{
			sprintf(Buffer, G->target->GetName());
			skp->Text(5 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		}

		if (!G->PClanded)
		{
			sprintf(Buffer, "%.3f°", G->LSLat*DEG);
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.3f°", G->LSLng*DEG);
			skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->LSAlt / 1852.0);
			skp->Text(1 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
		}

		GET_Display(Buffer, G->PC_TIG);
		skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 17 * H / 21, "DVX", 3);
		skp->Text(5 * W / 8, 18 * H / 21, "DVY", 3);
		skp->Text(5 * W / 8, 19 * H / 21, "DVZ", 3);

		sprintf(Buffer, "%+07.1f", G->PC_dV_LVLH.x / 0.3048);
		skp->Text(6 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f", G->PC_dV_LVLH.y / 0.3048);
		skp->Text(6 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f", G->PC_dV_LVLH.z / 0.3048);
		skp->Text(6 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));

		sprintf(Buffer, "%+07.1f", length(G->PC_dV_LVLH) / 0.3048);
		skp->Text(6 * W / 8, 20 * H / 21, Buffer, strlen(Buffer));
	}
	else if (screen == 19)
	{
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Terrain Model", 13);

		sprintf(Buffer, "%.3f°", G->TMLat*DEG);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.3f°", G->TMLng*DEG);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.3f°", G->TMAzi*DEG);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.1f ft", G->TMDistance / 0.3048);
		skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.1f ft", G->TMStepSize / 0.3048);
		skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 9 * H / 14, "LS Height:", 10);
		sprintf(Buffer, "%.2f NM", G->TMAlt / 1852.0);
		skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 20)
	{
		if (G->vesseltype < 2)
		{
			skp->Text(7 * W / 8, (int)(0.5 * H / 14), "CSM", 3);
		}
		else
		{
			skp->Text(7 * W / 8, (int)(0.5 * H / 14), "LM", 2);
		}
		
		skp->Text(1 * W / 8, 2 * H / 14, "Maneuver PAD", 12);
		skp->Text(1 * W / 8, 4 * H / 14, "Entry PAD", 9);
		skp->Text(1 * W / 8, 6 * H / 14, "Landmark Tracking", 17);
		skp->Text(1 * W / 8, 8 * H / 14, "Map Update", 10);
		skp->Text(1 * W / 8, 10 * H / 14, "Nav Check PAD", 13);
		skp->Text(1 * W / 8, 12 * H / 14, "P37 PAD", 7);

		skp->Text(5 * W / 8, 2 * H / 14, "DAP PAD", 7);
		skp->Text(5 * W / 8, 12 * H / 14, "Previous Page", 13);
	}
	else if (screen == 21)
	{
		if (G->vesseltype < 2)
		{
			skp->Text(7 * W / 8, (int)(0.5 * H / 14), "CSM", 3);
		}
		else
		{
			skp->Text(7 * W / 8, (int)(0.5 * H / 14), "LM", 2);
		}

		skp->Text(1 * W / 8, 2 * H / 14, "State Vector", 12);
		skp->Text(1 * W / 8, 4 * H / 14, "REFSMMAT", 8);
		skp->Text(1 * W / 8, 6 * H / 14, "VECPOINT", 8);
		skp->Text(1 * W / 8, 8 * H / 14, "Erasable Memory Programs", 24);

		skp->Text(5 * W / 8, 2 * H / 14, "LVDC", 4);
		skp->Text(5 * W / 8, 4 * H / 14, "Terrain Model", 13);
		skp->Text(5 * W / 8, 6 * H / 14, "AGC Ephemeris", 13);
		skp->Text(5 * W / 8, 12 * H / 14, "Previous Page", 13);
	}
	else if (screen == 22)
	{
		if (G->TLCCmaneuver == 0)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TLI (nodal)", 11);
		}
		else if (G->TLCCmaneuver == 1)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TLI (free return)", 17);
		}
		else if (G->TLCCmaneuver == 2)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TLMCC Option 1: Nodal Targeting", 31);
		}
		else if (G->TLCCmaneuver == 3)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TLMCC Option 2: FR BAP, Fixed LPO, LS", 37);
		}
		else if (G->TLCCmaneuver == 4)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TLMCC Option 3: FR BAP, Free LPO, LS", 36);
		}
		else if (G->TLCCmaneuver == 5)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TLMCC Option 4: Nonfree-return BAP, Fixed LPO, LS", 49);
		}
		else if (G->TLCCmaneuver == 6)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TLMCC Option 5: Nonfree-return BAP, Free LPO, LS", 48);
		}
		else if (G->TLCCmaneuver == 7)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TLMCC Option 6/7: Circumlunar free-return flyby", 47);
		}
		else
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TLMCC Option 8: SPS flyby to spec. FR inclination", 49);
		}
		
		if (G->TLCCmaneuver == 8)
		{
			if (G->subThreadStatus > 0)
			{
				sprintf(Buffer, "Iteration step %d/8...", G->TLCCIterationStep);
				skp->Text(5 * W / 8, 1 * H / 14, Buffer, strlen(Buffer));
			}
		}
		else
		{
			if (G->subThreadStatus > 0)
			{
				skp->Text(5 * W / 8, 1 * H / 14, "Calculating...", 14);
			}
		}

		if (!G->TLCCSolGood)
		{
			skp->Text(5 * W / 8, 1 * H / 14, "Calculation Failed!", 19);
		}

		GET_Display(Buffer, G->TLCC_GET);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

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

		//Nodal target display
		if (G->TLCCmaneuver == 0 || G->TLCCmaneuver == 2)
		{
			GET_Display(Buffer, G->TLCCNodeGET);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.5f°", G->TLCCNodeLat*DEG);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.5f°", G->TLCCNodeLng*DEG);
			skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->TLCCNodeAlt / 1852.0);
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->TLCCmaneuver == 1 || G->TLCCmaneuver == 3 || G->TLCCmaneuver == 4 || G->TLCCmaneuver == 7) //free return target display
		{
			GET_Display(Buffer, G->TLCCPeriGET);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			skp->Text(1 * W / 8, 11 * H / 21, "Pericynthion:", 13);
			GET_Display(Buffer, G->TLCCPeriGETcor);
			skp->Text(1 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));

			skp->Text(1 * W / 8, 13 * H / 21, "Reentry:", 8);
			GET_Display(Buffer, G->TLCCReentryGET);
			skp->Text(1 * W / 8, 14 * H / 21, Buffer, strlen(Buffer));

			skp->Text(1 * W / 8, 15 * H / 21, "FR Inclination:", 15);
			sprintf(Buffer, "%.3f°", G->TLCCFRIncl*DEG);
			skp->Text(1 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));

			skp->Text(1 * W / 8, 17 * H / 21, "Splashdown Latitude:", 20);
			sprintf(Buffer, "%.3f°", G->TLCCFRLat*DEG);
			skp->Text(1 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));

			skp->Text(1 * W / 8, 19 * H / 21, "Splashdown Longitude:", 21);
			sprintf(Buffer, "%.3f°", G->TLCCFRLng*DEG);
			skp->Text(1 * W / 8, 20 * H / 21, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.5f°", G->TLCCFreeReturnEMPLat*DEG);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			if (G->TLCCmaneuver == 3 || G->TLCCmaneuver == 4)
			{
				sprintf(Buffer, "%.2f NM", G->TLCCLAHPeriAlt / 1852.0);
			}
			else
			{
				sprintf(Buffer, "%.2f NM", G->TLCCFlybyPeriAlt / 1852.0);
			}
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->TLCCmaneuver == 5 || G->TLCCmaneuver == 6)
		{
			GET_Display(Buffer, G->TLCCPeriGET);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.5f°", G->TLCCNonFreeReturnEMPLat*DEG);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->TLCCLAHPeriAlt / 1852.0);
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->TLCCmaneuver == 8)
		{
			GET_Display(Buffer, G->TLCCPeriGET);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.3f°", G->TLCCFRDesiredInclination*DEG);
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			if (G->TLCCAscendingNode)
			{
				skp->Text(1 * W / 8, 10 * H / 14, "Ascending Node", 14);
			}
			else
			{
				skp->Text(1 * W / 8, 10 * H / 14, "Descending Node", 15);
			}

			sprintf(Buffer, "%.5f°", G->TLCCFreeReturnEMPLat*DEG);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", G->TLCCFlybyPeriAlt / 1852.0);
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

			skp->Text(1 * W / 8, 17 * H / 21, "Splashdown Latitude:", 20);
			sprintf(Buffer, "%.3f°", G->TLCCFRLat*DEG);
			skp->Text(1 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));

			skp->Text(1 * W / 8, 19 * H / 21, "Splashdown Longitude:", 21);
			sprintf(Buffer, "%.3f°", G->TLCCFRLng*DEG);
			skp->Text(1 * W / 8, 20 * H / 21, Buffer, strlen(Buffer));
		}

		if (G->TLCCmaneuver == 3 || G->TLCCmaneuver == 4 || G->TLCCmaneuver == 5 || G->TLCCmaneuver == 6 || G->TLCCmaneuver == 8)
		{
			sprintf(Buffer, "New Lat: %.5f°", G->TLCCEMPLatcor*DEG);
			skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 23)
	{
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Lunar Liftoff", 13);

		GET_Display(Buffer, G->t_TPIguess);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		skp->Text(1 * W / 8, 4 * H / 14, "Rendezvous Schedule:", 20);

		skp->Text(1 * W / 8, 8 * H / 21, "Launch Time:", 12);
		GET_Display(Buffer, G->LunarLiftoffTimes.t_L);
		skp->Text(1 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));

		skp->Text(1 * W / 8, 10 * H / 21, "Insertion:", 10);
		GET_Display(Buffer, G->LunarLiftoffTimes.t_Ins);
		skp->Text(1 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));

		if (G->LunarLiftoffTimeOption == 0)
		{
			skp->Text(1 * W / 8, 12 * H / 21, "CSI:", 4);
			GET_Display(Buffer, G->LunarLiftoffTimes.t_CSI);
			skp->Text(1 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));

			skp->Text(1 * W / 8, 14 * H / 21, "CDH:", 4);
			GET_Display(Buffer, G->LunarLiftoffTimes.t_CDH);
			skp->Text(1 * W / 8, 15 * H / 21, Buffer, strlen(Buffer));
		}

		if (G->LunarLiftoffTimeOption == 0 || G->LunarLiftoffTimeOption == 1)
		{
			skp->Text(1 * W / 8, 16 * H / 21, "TPI:", 4);
			GET_Display(Buffer, G->LunarLiftoffTimes.t_TPI);
			skp->Text(1 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
		}

		skp->Text(1 * W / 8, 18 * H / 21, "TPF:", 4);
		GET_Display(Buffer, G->LunarLiftoffTimes.t_TPF);
		skp->Text(1 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));

		if (G->LunarLiftoffTimeOption == 0)
		{
			skp->Text(4 * W / 8, 2 * H / 14, "Concentric Profile", 18);
		}
		else if (G->LunarLiftoffTimeOption == 1)
		{
			skp->Text(4 * W / 8, 2 * H / 14, "Direct Profile", 14);
		}
		else
		{
			skp->Text(4 * W / 8, 2 * H / 14, "Time Critical Profile", 21);
		}

		if (G->target != NULL)
		{
			sprintf(Buffer, G->target->GetName());
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		}

		if (G->LunarLiftoffTimeOption == 1)
		{
			skp->Text(5 * W / 8, 6 * H / 14, "DT Insertion-TPI:", 17);
			sprintf(Buffer, "%.1f min", G->DT_Ins_TPI / 60.0);
			skp->Text(5 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));
		}

		skp->Text(5 * W / 8, 9 * H / 14, "Horizontal Velocity:", 20);
		sprintf(Buffer, "%+.1f ft/s", G->LunarLiftoffTimes.v_LH / 0.3048);
		skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 11 * H / 14, "Vertical Velocity:", 18);
		sprintf(Buffer, "%+.1f ft/s", G->LunarLiftoffTimes.v_LV / 0.3048);
		skp->Text(5 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 24)
	{
		skp->Text(4 * W / 8, (int)(0.5 * H / 14), "Erasable Memory Programs", 24);

		skp->Text(1 * W / 8, 2 * H / 14, "Program 99", 10);

		sprintf(Buffer, "Uplink No. %d", G->EMPUplinkNumber);
		skp->Text(5 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

	}
	else if (screen == 25)
	{
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Nav Check PAD", 13);

		GET_Display(Buffer, G->navcheckpad.NavChk[0]);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%+07.2f LAT", G->navcheckpad.lat[0]);
		skp->Text(4 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%+07.2f LNG", G->navcheckpad.lng[0]);
		skp->Text(4 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%+07.1f ALT", G->navcheckpad.alt[0]);
		skp->Text(4 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 26)
	{
		skp->Text(6 * W / 8, (int)(0.5 * H / 14), "Deorbit", 7);

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

		sprintf(Buffer, "%f °", G->EntryAng*DEG);
		skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

		if (G->entrynominal)
		{
			skp->Text(1 * W / 8, 10 * H / 14, "Nominal", 7);
		}
		else
		{
			skp->Text(1 * W / 8, 10 * H / 14, "Min DV", 6);
		}

		if (G->enginetype == RTCC_ENGINETYPE_SPSDPS)
		{
			skp->Text(1 * W / 8, 12 * H / 14, "SPS Deorbit", 11);
		}
		else
		{
			skp->Text(1 * W / 8, 12 * H / 14, "SM RCS Deorbit", 14);
		}

		if (G->subThreadStatus > 0)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "Calculating...", 14);
		}
		else if (G->subThreadStatus == 0)
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
	}
	else if (screen == 27)
	{
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Return to Earth", 15);

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

		sprintf(Buffer, "%f °", G->EntryAng*DEG);
		skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

		if (G->entrycritical == 1)
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

		if (G->subThreadStatus > 0)
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

		AGC_Display(Buffer, G->Entry_DV.x / 0.3048);
		skp->Text(6 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->Entry_DV.y / 0.3048);
		skp->Text(6 * W / 8, 11 * H / 14, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->Entry_DV.z / 0.3048);
		skp->Text(6 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 28)
	{
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Return to Earth Flyby", 21);

		if (G->FlybyType != 1)
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
			skp->Text(1 * W / 8, 8 * H / 14, "Slow Return", 11);
		}
		else if (G->returnspeed == 1)
		{
			skp->Text(1 * W / 8, 8 * H / 14, "Normal Return", 13);
		}
		else if (G->returnspeed == 2)
		{
			skp->Text(1 * W / 8, 8 * H / 14, "Fast Return", 11);
		}

		sprintf(Buffer, "%.3f°", G->TLCCFRDesiredInclination*DEG);
		skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		if (G->TLCCAscendingNode)
		{
			skp->Text(1 * W / 8, 12 * H / 14, "Ascending Node", 14);
		}
		else
		{
			skp->Text(1 * W / 8, 12 * H / 14, "Descending Node", 15);
		}

		if (G->FlybyType == 0)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "Flyby", 5);
		}
		else if (G->FlybyType == 1)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "PC+2", 4);
		}

		if (G->subThreadStatus > 0)
		{
			skp->Text(5 * W / 8, 3 * H / 14, "Calculating...", 14);
		}

		sprintf(Buffer, "%.3f° Lat", G->EntryLatcor*DEG);
		skp->Text(5 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f° Lng", G->EntryLngcor*DEG);
		skp->Text(5 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.3f° ReA", G->EntryAngcor*DEG);
		skp->Text(5 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->P37GET400K);
		sprintf(Buffer, "%s RRT", Buffer);
		skp->Text(5 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 11 * H / 21, "FR Inclination:", 15);
		sprintf(Buffer, "%.3f°", G->TLCCFRIncl*DEG);
		skp->Text(5 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 13 * H / 21, "Flyby Altitude:", 15);
		sprintf(Buffer, "%.1f NM", G->FlybyPeriAlt / 1852.0);
		skp->Text(5 * W / 8, 14 * H / 21, Buffer, strlen(Buffer));

		GET_Display(Buffer, G->EntryTIGcor);
		sprintf(Buffer, "%s TIG", Buffer);
		skp->Text(5 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 17 * H / 21, "DVX", 3);
		skp->Text(5 * W / 8, 18 * H / 21, "DVY", 3);
		skp->Text(5 * W / 8, 19 * H / 21, "DVZ", 3);
		skp->Text(5 * W / 8, 20 * H / 21, "DVT", 3);

		AGC_Display(Buffer, G->Entry_DV.x / 0.3048);
		skp->Text(6 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->Entry_DV.y / 0.3048);
		skp->Text(6 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->Entry_DV.z / 0.3048);
		skp->Text(6 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, length(G->Entry_DV) / 0.3048);
		skp->Text(6 * W / 8, 20 * H / 21, Buffer, strlen(Buffer));
	}
	else if (screen == 29)
	{
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Transearth Injection", 20);

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

		if (G->returnspeed == 0)
		{
			skp->Text(1 * W / 8, 8 * H / 14, "Slow Return", 11);
		}
		else if (G->returnspeed == 1)
		{
			skp->Text(1 * W / 8, 8 * H / 14, "Normal Return", 13);
		}
		else if (G->returnspeed == 2)
		{
			skp->Text(1 * W / 8, 8 * H / 14, "Fast Return", 11);
		}

		sprintf(Buffer, "%.3f°", G->TLCCFRDesiredInclination*DEG);
		skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		if (G->TLCCAscendingNode)
		{
			skp->Text(1 * W / 8, 12 * H / 14, "Ascending Node", 14);
		}
		else
		{
			skp->Text(1 * W / 8, 12 * H / 14, "Descending Node", 15);
		}

		if (G->subThreadStatus > 0)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "Calculating...", 14);
		}

		sprintf(Buffer, "%.3f° Lat", G->EntryLatcor*DEG);
		skp->Text(5 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f° Lng", G->EntryLngcor*DEG);
		skp->Text(5 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.3f° ReA", G->EntryAngcor*DEG);
		skp->Text(5 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->P37GET400K);
		sprintf(Buffer, "%s RRT", Buffer);
		skp->Text(5 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 11 * H / 21, "FR Inclination:", 15);
		sprintf(Buffer, "%.3f°", G->TLCCFRIncl*DEG);
		skp->Text(5 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));

		GET_Display(Buffer, G->EntryTIGcor);
		sprintf(Buffer, "%s TIG", Buffer);
		skp->Text(5 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 17 * H / 21, "DVX", 3);
		skp->Text(5 * W / 8, 18 * H / 21, "DVY", 3);
		skp->Text(5 * W / 8, 19 * H / 21, "DVZ", 3);
		skp->Text(5 * W / 8, 20 * H / 21, "DVT", 3);

		AGC_Display(Buffer, G->Entry_DV.x / 0.3048);
		skp->Text(6 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->Entry_DV.y / 0.3048);
		skp->Text(6 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->Entry_DV.z / 0.3048);
		skp->Text(6 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, length(G->Entry_DV) / 0.3048);
		skp->Text(6 * W / 8, 20 * H / 21, Buffer, strlen(Buffer));
	}
	else if (screen == 30)
	{
		skp->Text(6 * W / 8, (int)(0.5 * H / 14), "Entry Update", 12);

		sprintf(Buffer, "Lat:  %f °", G->EntryLatcor*DEG);
		skp->Text(5 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "Long: %f °", G->EntryLngcor*DEG);
		skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "Desired Range: %.1f NM", G->entryrange);
		skp->Text(4 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "Actual Range:  %.1f NM", G->EntryRTGO);
		skp->Text(4 * W / 8, 9 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 31)
	{
		skp->Text(6 * W / 8,(int)(0.5 * H / 14), "P37 Block Data", 14);

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
	else if (screen == 32)
	{
		skp->Text(1 * W / 8, 2 * H / 14, "Lambert Targeting", 17);
		skp->Text(1 * W / 8, 4 * H / 14, "Coelliptic", 10);
		skp->Text(1 * W / 8, 6 * H / 14, "Docking Initiation Processor", 28);
		skp->Text(1 * W / 8, 8 * H / 14, "Skylab Rendezvous and TPI Search", 32);
	}
	else if (screen == 33)
	{
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Docking Initiate", 16);

		if (G->DKI_Profile == 0)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "CSI/CDH Sequence", 16);
		}
		else if(G->DKI_Profile == 1)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "HAM-CSI/CDH Sequence", 20);
		}
		else
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Rescue-2 Sequence", 17);
		}

		GET_Display(Buffer, G->DKI_TIG);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->t_TPIguess);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.1f NM", G->DH / 1852.0);
		skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f°", G->lambertelev*DEG);
		skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		if (G->target != NULL)
		{
			sprintf(Buffer, G->target->GetName());
			skp->Text(5 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		}

		skp->Text(5 * W / 8, 5 * H / 21, "Phasing:", 8);
		GET_Display(Buffer, G->P30TIG);
		skp->Text(5 * W / 8, 6 * H / 21, Buffer, strlen(Buffer));

		sprintf(Buffer, "%+07.1f", G->dV_LVLH.x / 0.3048);
		skp->Text(5 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f", G->dV_LVLH.y / 0.3048);
		skp->Text(5 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f", G->dV_LVLH.z / 0.3048);
		skp->Text(5 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));

		if (G->DKI_Profile == 1)
		{
			skp->Text(4 * W / 8, 13 * H / 21, "Boost:", 6);
			GET_Display(Buffer, G->dkiresult.t_Boost);
			skp->Text(5 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f ft/s", G->dkiresult.dv_Boost / 0.3048);
			skp->Text(5 * W / 8, 14 * H / 21, Buffer, strlen(Buffer));

			skp->Text(4 * W / 8, 15 * H / 21, "HAM:", 4);
			GET_Display(Buffer, G->dkiresult.t_HAM);
			skp->Text(5 * W / 8, 15 * H / 21, Buffer, strlen(Buffer));
		}

		skp->Text(4 * W / 8, 16 * H / 21, "CSI:", 4);
		GET_Display(Buffer, G->dkiresult.t_CSI);
		skp->Text(5 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f ft/s", G->dkiresult.dv_CSI / 0.3048);
		skp->Text(5 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));

		skp->Text(4 * W / 8, 18 * H / 21, "CDH:", 4);
		GET_Display(Buffer, G->dkiresult.t_CDH);
		skp->Text(5 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f ft/s", length(G->dkiresult.DV_CDH) / 0.3048);
		skp->Text(5 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));

		skp->Text(4 * W / 8, 20 * H / 21, "TPI:", 4);
		GET_Display(Buffer, G->dkiresult.t_TPI);
		skp->Text(5 * W / 8, 20 * H / 21, Buffer, strlen(Buffer));

	}
	else if (screen == 34)
	{
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "DKI Options", 11);

		if (G->DKI_Maneuver_Line)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Maneuver Line", 13);
		}
		else
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Specified DTs", 13);
		}

		if (G->DKI_Radial_DV)
		{
			skp->Text(1 * W / 8, 4 * H / 14, "-50 ft/s radial component", 25);
		}
		else
		{
			skp->Text(1 * W / 8, 4 * H / 14, "Horizontal maneuver", 19);
		}

		if (G->DKI_TPI_Mode == 0)
		{
			skp->Text(1 * W / 8, 6 * H / 14, "TPI on time", 11);
		}
		else if(G->DKI_TPI_Mode == 1)
		{
			skp->Text(1 * W / 8, 6 * H / 14, "TPI at orbital midnight", 23);
		}
		else
		{
			skp->Text(1 * W / 8, 6 * H / 14, "TPI at X min before sunrise:", 28);
		}

		if (G->DKI_TPI_Mode == 2)
		{
			sprintf(Buffer, "%.1f min", G->DKI_dt_TPI_sunrise / 60.0);
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		}

		sprintf(Buffer, "%d", G->DKI_N);
		skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		if (G->DKI_Maneuver_Line == false)
		{
			sprintf(Buffer, "%.1f min", G->DKI_dt_PBH / 60.0);
			skp->Text(6 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.1f min", G->DKI_dt_BHAM / 60.0);
			skp->Text(6 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.1f min", G->DKI_dt_HAMH / 60.0);
			skp->Text(6 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 35)
	{
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "DAP PAD", 7);

		sprintf(Buffer, "%+06.0f WT N47", G->DAP_PAD.ThisVehicleWeight);
		skp->Text((int)(3.5 * W / 8), 5 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06.0f", G->DAP_PAD.OtherVehicleWeight);
		skp->Text((int)(3.5 * W / 8), 6 * H / 21, Buffer, strlen(Buffer));

		sprintf(Buffer, "%+07.2f GMBL N48", G->DAP_PAD.PitchTrim);
		skp->Text((int)(3.5 * W / 8), 7 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.2f", G->DAP_PAD.YawTrim);
		skp->Text((int)(3.5 * W / 8), 8 * H / 21, Buffer, strlen(Buffer));
	}
	else if (screen == 36)
	{
		skp->Text(6 * W / 8, (int)(0.5 * H / 14), "LVDC", 4);

		sprintf(Buffer, "Launch Azimuth: %.4f°", G->LVDCLaunchAzimuth*DEG);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 37)
	{
		skp->Text(4 * W / 8, (int)(0.5 * H / 14), "AGC Ephemeris Generator", 23);

		skp->Text(1 * W / 8, 2 * H / 14, "Epoch of BRCS:", 14);
		skp->Text(1 * W / 8, 4 * H / 14, "TEphemZero:", 11);
		skp->Text(1 * W / 8, 6 * H / 14, "TIMEM0:", 7);

		sprintf(Buffer, "%f", G->AGCEphemBRCSEpoch);
		skp->Text(4 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", G->AGCEphemTEphemZero);
		skp->Text(4 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", G->AGCEphemTIMEM0);
		skp->Text(4 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
	}
	return true;
}

void ApolloRTCCMFD::menuEntryUpload()
{
	if (!G->inhibUplLOS || !G->vesselinLOS())
	{
		if (G->vesseltype < 2)
		{
			G->EntryUplink();
		}
		else //LM has no entry update
		{
			G->P30Uplink();
		}
	}
}

void ApolloRTCCMFD::menuEntryUpdateUpload()
{
	if (!G->inhibUplLOS || !G->vesselinLOS())
	{
		if (G->vesseltype < 2)
		{
			G->EntryUpdateUplink();
		}
	}
}

void ApolloRTCCMFD::menuP30Upload()
{
	if (screen == 22 && G->TLCCmaneuver < 2)
	{
		SevenParameterUpdate coe;
		SaturnV* testves;

		testves = (SaturnV*)G->g_Data.progVessel;
		LVDCSV *lvdc = (LVDCSV*)testves->iu->lvdc;

		coe = G->rtcc->TLICutoffToLVDCParameters(G->R_TLI, G->V_TLI, G->GETbase, G->P30TIG, lvdc->TB5, lvdc->mu, lvdc->T_RG);

		lvdc->TU = true;
		lvdc->TU10 = false;
		lvdc->GATE3 = false;

		lvdc->T_RP = coe.T_RP;
		lvdc->C_3 = coe.C3;
		lvdc->Inclination = coe.Inclination;
		lvdc->e = coe.e;
		lvdc->alpha_D = coe.alpha_D;
		lvdc->f = coe.f;
		lvdc->theta_N = coe.theta_N;
	}
	else if (!G->inhibUplLOS || !G->vesselinLOS())
	{
		G->P30Uplink();
	}
}

void ApolloRTCCMFD::menuTLANDUpload()
{
	G->TLANDUplink();
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

char* ApolloRTCCMFD::REFSMMATName(char* Buff, int n)
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
		sprintf(Buff, "LOI-2");
	}
	else if (n == 8)
	{
		sprintf(Buff, "Landing Site");
	}
	else if (n == 9)
	{
		sprintf(Buff, "REFS from Att");
	}
	else
	{
		sprintf(Buff, "Unknown Type");
	}
	return Buff;
}

void ApolloRTCCMFD::CycleREFSMMATopt()
{
	if (G->REFSMMATopt < 9)
	{
		G->REFSMMATopt++;
	}
	else
	{
		G->REFSMMATopt = 0;
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

void ApolloRTCCMFD::menuSetTargetingMenu()
{
	screen = 14;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetVECPOINTPage()
{
	screen = 15;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetDOIPage()
{
	screen = 16;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetSkylabPage()
{
	screen = 17;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetPCPage()
{
	screen = 18;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetTerrainModelPage()
{
	screen = 19;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetPADMenu()
{
	screen = 20;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetUtilityMenu()
{
	screen = 21;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuTranslunarPage()
{
	screen = 22;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetLunarLiftoffPage()
{
	screen = 23;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetEMPPage()
{
	screen = 24;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetNavCheckPADPage()
{
	screen = 25;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetDeorbitPage()
{
	screen = 26;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetEarthEntryPage()
{
	screen = 27;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetMoonEntryPage()
{
	screen = 28;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetTEIPage()
{
	screen = 29;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetEntryUpdatePage()
{
	screen = 30;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetP37PADPage()
{
	screen = 31;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetRendezvousPage()
{
	screen = 32;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetDKIPage()
{
	screen = 33;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetDKIOptionsPage()
{
	screen = 34;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetDAPPADPage()
{
	screen = 35;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetLVDCPage()
{
	screen = 36;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetAGCEphemerisPage()
{
	screen = 37;
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

void ApolloRTCCMFD::menuCycleOrbAdjOptions()
{
	if (G->GMPType >= 7)
	{
		G->GMPType = 0;
	}
	else
	{
		G->GMPType++;
	}
}

void ApolloRTCCMFD::menuCycleOrbAdjAltRef()
{
	G->OrbAdjAltRef = !G->OrbAdjAltRef;
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
	double elev, pdidt;
	if (sscanf(str, "E=%lf", &elev) == 1)
	{
		((ApolloRTCCMFD*)data)->set_lambertelev(elev);
		return true;
	}
	else if (sscanf(str, "PDI+%lf", &pdidt) == 1)
	{
		((ApolloRTCCMFD*)data)->set_t1_PDI(pdidt * 60.0);
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

void ApolloRTCCMFD::set_t1_PDI(double dt)
{
	G->T1 = G->pdipad.GETI + dt;
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

void ApolloRTCCMFD::OrbAdjRevDialogue()
{
	bool OrbAdjRevInput(void *id, char *str, void *data);
	oapiOpenInputBox("Number of revolutions:", OrbAdjRevInput, 0, 20, (void*)this);
}

bool OrbAdjRevInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_OrbAdjRevs(atoi(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_OrbAdjRevs(int N)
{
	G->GMPRevs = N;
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
	bool EntryGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET (Format: hhh:mm:ss)", EntryGETInput, 0, 20, (void*)this);
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
	bool EntryAngInput(void* id, char *str, void *data);
	oapiOpenInputBox("Entry FPA in degrees (°):", EntryAngInput, 0, 20, (void*)this);
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
	if (G->REFSMMATopt == 2 || G->REFSMMATopt == 6)
	{
		bool REFSMMATGETInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the GET (Format: hhh:mm:ss)", REFSMMATGETInput, 0, 20, (void*)this);
	}
	else if (G->REFSMMATopt == 5 || G->REFSMMATopt == 8)
	{
		menuSetTLAND();
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
	OBJHANDLE gravref = G->rtcc->AGCGravityRef(G->vessel);

	G->vessel->GetRelativePos(gravref, R);
	G->vessel->GetRelativeVel(gravref, V);
	mu = GGRAV*oapiGetMass(gravref);
	pet = OrbMech::timetoperi(R, V, mu);
	mjd = oapiTime2MJD(oapiGetSimTime() + pet);
	return (mjd - G->GETbase)*24.0*3600.0;
}

double ApolloRTCCMFD::timetoapo()
{
	VECTOR3 R, V;
	double mu, pet, mjd;
	OBJHANDLE gravref = G->rtcc->AGCGravityRef(G->vessel);

	G->vessel->GetRelativePos(gravref, R);
	G->vessel->GetRelativeVel(gravref, V);
	mu = GGRAV*oapiGetMass(gravref);
	pet = OrbMech::timetoapo(R, V, mu);
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
	OBJHANDLE gravref = G->rtcc->AGCGravityRef(G->vessel);

	G->lambertelev = elev*RAD;

	if (G->target == NULL)
	{
		return;
	}
	double mu, SVMJD, dt1;
	VECTOR3 RA0_orb, VA0_orb, RP0_orb, VP0_orb,RA0,VA0,RP0,VP0;

	mu = GGRAV*oapiGetMass(gravref);

	G->vessel->GetRelativePos(gravref, RA0_orb);
	G->vessel->GetRelativeVel(gravref, VA0_orb);
	G->target->GetRelativePos(gravref, RP0_orb);
	G->target->GetRelativeVel(gravref, VP0_orb);
	SVMJD = oapiGetSimMJD();

	RA0 = _V(RA0_orb.x, RA0_orb.z, RA0_orb.y);	//The following equations use another coordinate system than Orbiter
	VA0 = _V(VA0_orb.x, VA0_orb.z, VA0_orb.y);
	RP0 = _V(RP0_orb.x, RP0_orb.z, RP0_orb.y);
	VP0 = _V(VP0_orb.x, VP0_orb.z, VP0_orb.y);

	dt1 = OrbMech::findelev(RA0, VA0, RP0, VP0, SVMJD, G->lambertelev, gravref);
	G->T1 = dt1 + (SVMJD - G->GETbase) * 24.0 * 60.0 * 60.0;
}

void ApolloRTCCMFD::calcREFSMMAT()
{
	G->REFSMMATCalc();
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
	this->G->LSLat = lat*RAD;
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
	this->G->LSLng = lng*RAD;
}

void ApolloRTCCMFD::OrbAdjApoDialogue()
{
	if (G->GMPType == 0 || G->GMPType == 1 || G->GMPType == 5)
	{
		bool OrbAdjApoInput(void* id, char *str, void *data);
		oapiOpenInputBox("Apoapsis in NM:", OrbAdjApoInput, 0, 20, (void*)this);
	}
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
	if (G->GMPType == 0 || G->GMPType == 2 || G->GMPType == 4)
	{
		bool OrbAdjPeriInput(void* id, char *str, void *data);
		oapiOpenInputBox("Periapsis in NM:", OrbAdjPeriInput, 0, 20, (void*)this);
	}
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
	if (G->GMPType == 0)
	{
		bool OrbAdjIncInput(void* id, char *str, void *data);
		oapiOpenInputBox("Inclination in degrees:", OrbAdjIncInput, 0, 20, (void*)this);
	}
	else if (G->GMPType == 5 || G->GMPType == 7)
	{
		bool OrbAdjIncInput(void* id, char *str, void *data);
		oapiOpenInputBox("Rotation angle in degrees:", OrbAdjIncInput, 0, 20, (void*)this);
	}
	else if (G->GMPType == 6)
	{
		bool OrbAdjIncInput(void* id, char *str, void *data);
		oapiOpenInputBox("Longitude in degrees:", OrbAdjIncInput, 0, 20, (void*)this);
	}
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
	if (G->GMPType == 0)
	{
		this->G->incdeg = inc;
	}
	else if (G->GMPType == 5 || G->GMPType == 7)
	{
		this->G->GMPRotationAngle = inc * RAD;
	}
	else if (G->GMPType == 6)
	{
		this->G->GMPLongitude = inc*RAD;
	}
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
	this->G->DH = DH * 1852.0;
}

void ApolloRTCCMFD::phasedialogue()
{
	if (G->twoimpulsemode == 1)
	{
		bool PhaseInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the phase angle:", PhaseInput, 0, 20, (void*)this);
	}
}

bool PhaseInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_TIPhaseAngle(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TIPhaseAngle(double angdeg)
{
	G->TwoImpulse_PhaseAngle = angdeg * RAD;
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
	if (G->svmode == 0)
	{
		if (G->svtimemode)
		{
			bool SVGETInput(void *id, char *str, void *data);
			oapiOpenInputBox("Choose the GET for the state vector (Format: hhh:mm:ss)", SVGETInput, 0, 20, (void*)this);
		}
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
	G->J2000GET = SVtime;
}

void ApolloRTCCMFD::menuSetAGSKFactor()
{
	if (G->svmode == 2)
	{
		bool AGSKFactorInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the AGS time bias", AGSKFactorInput, 0, 20, (void*)this);
	}
}

bool AGSKFactorInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_AGSKFactor(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_AGSKFactor(double time)
{
	G->AGSKFactor = time*3600.0;
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

void ApolloRTCCMFD::menuTEICalc()
{
	G->TEICalc();
}

void ApolloRTCCMFD::menuRTEFlybyCalc()
{
	G->RTEFlybyCalc();
}

void ApolloRTCCMFD::menuDeorbitCalc()
{
	G->DeorbitCalc();
}

void ApolloRTCCMFD::menuEntryUpdateCalc()
{
	G->EntryUpdateCalc();
}

void ApolloRTCCMFD::menuEntryCalc()
{
	G->EntryCalc();
}

void ApolloRTCCMFD::EntryRangeDialogue()
{
	bool EntryRangeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the Entry Range in NM:", EntryRangeInput, 0, 20, (void*)this);
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
		if (G->svmode == 0)
		{
			if (!G->svtarget->GroundContact())
			{
				G->StateVectorCalc();
			}
		}
		else if (G->svmode == 1)
		{
			if (G->svtarget->GroundContact())
			{
				G->LandingSiteUpdate();
			}
		}
		else if (G->svmode == 2)
		{
			G->AGSStateVectorCalc();
		}
	}
}

void ApolloRTCCMFD::menuSwitchSVSlot()
{
	if (G->svmode == 0 || G->svmode == 2)
	{
		G->SVSlot = !G->SVSlot;
	}
}

void ApolloRTCCMFD::menuSVUpload()
{
	if (!G->inhibUplLOS || !G->vesselinLOS())
	{
		if (G->svmode == 0)
		{
			G->StateVectorUplink();
		}
		else if (G->svmode == 1)
		{
			G->LandingSiteUplink();
		}
	}
}

void ApolloRTCCMFD::menuCycleTwoImpulseOption()
{
	if (G->twoimpulsemode < 2)
	{
		G->twoimpulsemode++;
	}
	else
	{
		G->twoimpulsemode = 0;
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
	else if (G->manpadopt == 2 && G->vesseltype > 1)
	{
		G->HeadsUp = !G->HeadsUp;
	}
}

void ApolloRTCCMFD::menuManDirection()
{
	if (G->directiontype < 1)
	{
		G->directiontype++;
	}
	else
	{
		G->directiontype = 0;
	}
}

void ApolloRTCCMFD::menuSwitchPDIPADDirect()
{
	if (G->manpadopt == 2 && G->vesseltype > 1)
	{
		G->PDIPADdirect = !G->PDIPADdirect;
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
		if (G->vesseltype < 2)
		{
			TLIPADOpt opt;
			double T_TLI;

			SaturnV *SatV = (SaturnV*)G->g_Data.progVessel;
			LVDCSV *lvdc = (LVDCSV*)SatV->iu->lvdc;

			if (lvdc->TU)
			{
				G->TLI_PAD();
			}
			else
			{
				LVDCTLIparam tliparam;

				tliparam.alpha_TS = lvdc->alpha_TS;
				tliparam.Azimuth = lvdc->Azimuth;
				tliparam.beta = lvdc->beta;
				tliparam.cos_sigma = lvdc->cos_sigma;
				tliparam.C_3 = lvdc->C_3;
				tliparam.e_N = lvdc->e_N;
				tliparam.f = lvdc->f;
				tliparam.mu = lvdc->mu;
				tliparam.MX_A = lvdc->MX_A;
				tliparam.omega_E = lvdc->omega_E;
				tliparam.phi_L = lvdc->PHI;
				tliparam.R_N = lvdc->R_N;
				tliparam.TargetVector = lvdc->TargetVector;
				tliparam.TB5 = lvdc->TB5;
				tliparam.theta_EO = lvdc->theta_EO;
				tliparam.t_D = lvdc->t_D;
				tliparam.T_L = lvdc->T_L;
				tliparam.T_RG = lvdc->T_RG;
				tliparam.T_ST = lvdc->T_ST;

				G->rtcc->LVDCTLIPredict(tliparam, G->vessel, G->GETbase, G->dV_LVLH, G->P30TIG, G->R_TLI, G->V_TLI, T_TLI);

				opt.dV_LVLH = G->dV_LVLH;
				opt.GETbase = G->GETbase;
				opt.REFSMMAT = G->REFSMMAT;
				opt.TIG = G->P30TIG;
				opt.vessel = G->vessel;
				opt.SeparationAttitude = lvdc->XLunarAttitude;
				opt.TLI = T_TLI;
				opt.R_TLI = G->R_TLI;
				opt.V_TLI = G->V_TLI;
				opt.uselvdc = true;

				G->rtcc->TLI_PAD(&opt, G->tlipad);
			}
		}
		else
		{
			G->PDI_PAD();
		}
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
	if (G->entrycritical < 3)
	{
		G->entrycritical++;
	}
	else
	{
		G->entrycritical = 1;
	}
}

void ApolloRTCCMFD::menuSwitchFlybyType()
{
	if (G->FlybyType < 1)
	{
		G->FlybyType++;
	}
	else
	{
		G->FlybyType = 0;
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
		if (G->enginetype < 1)
		{
			G->enginetype++;
		}
		else
		{
			G->enginetype = 0;
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

void ApolloRTCCMFD::menuSetAGCEpoch()
{
	if (G->mission == 0)
	{
		bool AGCEpochInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the AGC Epoch:", AGCEpochInput, 0, 20, (void*)this);
	}
}

bool AGCEpochInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_AGCEpoch(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_AGCEpoch(double mjd)
{
	this->G->AGCEpoch = mjd;
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

		if (!stricmp(G->vessel->GetClassName(), "ProjectApollo\\LEM") ||
			!stricmp(G->vessel->GetClassName(), "ProjectApollo/LEM") ||
			!stricmp(G->vessel->GetClassName(), "ProjectApollo\\LEMSaturn") ||
			!stricmp(G->vessel->GetClassName(), "ProjectApollo/LEMSaturn")) {
			LEM *lem = (LEM *)G->vessel;
			if (lem->GetStage() < 2)
			{
				G->lemdescentstage = true;
			}
			else
			{
				G->lemdescentstage = false;
			}
		}
	}
}

void ApolloRTCCMFD::menuCycleLMStage()
{
	G->lemdescentstage = !G->lemdescentstage;
}

void ApolloRTCCMFD::menuUpdateLiftoffTime()
{
	double TEPHEM0;

	if (G->mission < 11)		//NBY 1968/1969
	{
		TEPHEM0 = 40038.;
	}
	else if (G->mission < 14)	//NBY 1969/1970
	{
		TEPHEM0 = 40403.;
	}
	else if (G->mission < 15)	//NBY 1970/1971
	{
		TEPHEM0 = 40768.;
	}
	else						//NBY 1971/1972
	{
		TEPHEM0 = 41133.;
	}

	if (G->vesseltype < 2)
	{
		saturn = (Saturn *)G->vessel;

		double tephem = saturn->agc.vagc.Erasable[0][01710] +
			saturn->agc.vagc.Erasable[0][01707] * pow((double) 2., (double) 14.) +
			saturn->agc.vagc.Erasable[0][01706] * pow((double) 2., (double) 28.);
		G->GETbase = (tephem / 8640000.) + TEPHEM0;
	}
	else
	{
		lem = (LEM *)G->vessel;

		double tephem = lem->agc.vagc.Erasable[0][01710] +
			lem->agc.vagc.Erasable[0][01707] * pow((double) 2., (double) 14.) +
			lem->agc.vagc.Erasable[0][01706] * pow((double) 2., (double) 28.);
		G->GETbase = (tephem / 8640000.) + TEPHEM0;
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

void ApolloRTCCMFD::cycleREFSMMATHeadsUp()
{
	if (G->REFSMMATopt == 0)
	{
		G->REFSMMATHeadsUp = !G->REFSMMATHeadsUp;
	}
}

void ApolloRTCCMFD::offvecdialogue()
{
	if (G->twoimpulsemode == 0)
	{
		bool OffVecInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the offset (x.x x.x x.x):", OffVecInput, 0, 20, (void*)this);
	}
	else if (G->twoimpulsemode == 1)
	{
		DHdialogue();
	}
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

void ApolloRTCCMFD::cycleVECDirOpt()
{
	if (G->VECdirection < 5)
	{
		G->VECdirection++;
	}
	else
	{
		G->VECdirection = 0;
	}
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
	if (oapiGetGbodyByName(str) != NULL)
	{
		((ApolloRTCCMFD*)data)->set_vecbody(oapiGetGbodyByName(str));
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
	G->VecPointCalc();
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

void ApolloRTCCMFD::menuCycleSVMode()
{
	if (G->svmode < 2)
	{
		G->svmode++;
	}
	else
	{
		G->svmode = 0;
	}
}

void ApolloRTCCMFD::menuCycleSVTimeMode()
{
	if (G->svmode == 0)
	{
		G->svtimemode = !G->svtimemode;
	}
}

void ApolloRTCCMFD::GetREFSMMATfromAGC()
{
	agc_t* vagc;

	if (G->vesseltype < 2)
	{
		saturn = (Saturn *)G->vessel;

		vagc = &saturn->agc.vagc;
	}
	else
	{
		lem = (LEM *)G->vessel;

		vagc = &lem->agc.vagc;
	}

	unsigned short REFSoct[20];
	int REFSMMATaddress;

	REFSMMATaddress = G->REFSMMAT_Address();

	REFSoct[2] = vagc->Erasable[0][REFSMMATaddress];
	REFSoct[3] = vagc->Erasable[0][REFSMMATaddress + 1];
	REFSoct[4] = vagc->Erasable[0][REFSMMATaddress + 2];
	REFSoct[5] = vagc->Erasable[0][REFSMMATaddress + 3];
	REFSoct[6] = vagc->Erasable[0][REFSMMATaddress + 4];
	REFSoct[7] = vagc->Erasable[0][REFSMMATaddress + 5];
	REFSoct[8] = vagc->Erasable[0][REFSMMATaddress + 6];
	REFSoct[9] = vagc->Erasable[0][REFSMMATaddress + 7];
	REFSoct[10] = vagc->Erasable[0][REFSMMATaddress + 8];
	REFSoct[11] = vagc->Erasable[0][REFSMMATaddress + 9];
	REFSoct[12] = vagc->Erasable[0][REFSMMATaddress + 10];
	REFSoct[13] = vagc->Erasable[0][REFSMMATaddress + 11];
	REFSoct[14] = vagc->Erasable[0][REFSMMATaddress + 12];
	REFSoct[15] = vagc->Erasable[0][REFSMMATaddress + 13];
	REFSoct[16] = vagc->Erasable[0][REFSMMATaddress + 14];
	REFSoct[17] = vagc->Erasable[0][REFSMMATaddress + 15];
	REFSoct[18] = vagc->Erasable[0][REFSMMATaddress + 16];
	REFSoct[19] = vagc->Erasable[0][REFSMMATaddress + 17];

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
	G->REFSMMAT = mul(G->REFSMMAT, OrbMech::J2000EclToBRCS(G->AGCEpoch));
	G->REFSMMATcur = G->REFSMMATopt;

	//sprintf(oapiDebugString(), "%f, %f, %f, %f, %f, %f, %f, %f, %f", G->REFSMMAT.m11, G->REFSMMAT.m12, G->REFSMMAT.m13, G->REFSMMAT.m21, G->REFSMMAT.m22, G->REFSMMAT.m23, G->REFSMMAT.m31, G->REFSMMAT.m32, G->REFSMMAT.m33);
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
	G->entrynominal = !G->entrynominal;
}

void ApolloRTCCMFD::menuSwitchDeorbitEngineOption()
{
	if (G->enginetype < 1)
	{
		G->enginetype++;
	}
	else
	{
		G->enginetype = 0;
	}
}

void ApolloRTCCMFD::menuSwitchReturnSpeed()
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

void ApolloRTCCMFD::EntryLongitudeModeDialogue()
{
	G->entrylongmanual = !G->entrylongmanual;
}

void ApolloRTCCMFD::menuSwitchLOIManeuver()
{
	if (G->LOImaneuver < 2)
	{
		G->LOImaneuver++;
	}
	else
	{
		G->LOImaneuver = 0;
	}
}

void ApolloRTCCMFD::menuSwitchLOIOption()
{
	if (G->LOIOption < 1)
	{
		G->LOIOption++;
	}
	else
	{
		G->LOIOption = 0;
	}
}

void ApolloRTCCMFD::menuSwitchTLCCManeuver()
{
	if (G->TLCCmaneuver < 8)
	{
		G->TLCCmaneuver++;
	}
	else
	{
		G->TLCCmaneuver = 0;
	}
}

void ApolloRTCCMFD::menuSetTLCCGET()
{
	bool TLCCGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for the maneuver (Format: hhh:mm:ss)", TLCCGETInput, 0, 20, (void*)this);
}

bool TLCCGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_TLCCGET(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLCCGET(double time)
{
	G->TLCC_GET = time;
}

void ApolloRTCCMFD::menuSetTLCCPeriGET()
{
	bool TLCCPeriGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the Pericyntheon GET (Format: hhh:mm:ss)", TLCCPeriGETInput, 0, 20, (void*)this);
}

void ApolloRTCCMFD::menuSetTLAND()
{
	bool TLandGETnput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the Time of Landing (Format: hhh:mm:ss)", TLandGETnput, 0, 20, (void*)this);
}

bool TLCCPeriGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_TLCCPeriGET(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLCCPeriGET(double time)
{
	if (G->TLCCmaneuver == 0 || G->TLCCmaneuver == 2)
	{
		G->TLCCNodeGET = time;
	}
	else
	{
		G->TLCCPeriGET = time;
	}
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
	G->t_Land = time;
}

void ApolloRTCCMFD::menuSetTLCCDesiredInclination()
{
	if (G->TLCCmaneuver == 8 || screen == 28 || screen == 29)
	{
		bool TLCCDesiredInclinationInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the desired inclination:", TLCCDesiredInclinationInput, 0, 20, (void*)this);
	}
}

bool TLCCDesiredInclinationInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_TLCCDesiredInclination(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLCCDesiredInclination(double inc)
{
	G->TLCCFRDesiredInclination = inc*RAD;
}

void ApolloRTCCMFD::menuSwitchTLCCAscendingNode()
{
	if (G->TLCCmaneuver == 8 || screen == 28 || screen == 29)
	{
		G->TLCCAscendingNode = !G->TLCCAscendingNode;
	}
}

void ApolloRTCCMFD::menuSetTLCCLat()
{
	bool TLCCLatInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the pericyntheon latitude:", TLCCLatInput, 0, 20, (void*)this);
}

bool TLCCLatInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_TLCCLat(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLCCLat(double lat)
{
	if (G->TLCCmaneuver == 0 || G->TLCCmaneuver == 2)
	{
		this->G->TLCCNodeLat = lat*RAD;
	}
	else if (G->TLCCmaneuver == 5 || G->TLCCmaneuver == 6)
	{
		this->G->TLCCNonFreeReturnEMPLat = lat*RAD;
	}
	else
	{
		this->G->TLCCFreeReturnEMPLat = lat*RAD;
	}
}

void ApolloRTCCMFD::menuSetTLCCLng()
{
	if (G->TLCCmaneuver == 0 || G->TLCCmaneuver == 2)
	{
		bool TLCCLngInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the pericyntheon longitude:", TLCCLngInput, 0, 20, (void*)this);
	}
}

bool TLCCLngInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_TLCCLng(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLCCLng(double lng)
{
	this->G->TLCCNodeLng = lng*RAD;
}

void ApolloRTCCMFD::menuSetLOIApo()
{
	if (G->LOImaneuver == 0 || G->LOImaneuver == 1)
	{
		bool LOIApoInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the apocynthion altitude:", LOIApoInput, 0, 20, (void*)this);
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

void ApolloRTCCMFD::menuSetLOIPeri()
{
	bool LOIPeriInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the perilune altitude:", LOIPeriInput, 0, 20, (void*)this);
}

bool LOIPeriInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LOIPeri(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LOIPeri(double alt)
{
	if (G->LOImaneuver == 2)
	{
		G->LOI2Alt = alt*1852.0;
	}
	else
	{
		G->LOIperi = alt*1852.0;
	}
}

void ApolloRTCCMFD::menuSetTLCCAlt()
{
	bool TLCCPeriInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the perilune altitude:", TLCCPeriInput, 0, 20, (void*)this);
}

bool TLCCPeriInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_TLCCAlt(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TLCCAlt(double alt)
{
	if (G->TLCCmaneuver == 0 || G->TLCCmaneuver == 2)
	{
		this->G->TLCCNodeAlt = alt*1852.0;
	}
	else if (G->TLCCmaneuver == 1 || G->TLCCmaneuver == 7 || G->TLCCmaneuver == 8)
	{
		this->G->TLCCFlybyPeriAlt = alt*1852.0;
	}
	else
	{
		this->G->TLCCLAHPeriAlt = alt*1852.0;
	}
}

void ApolloRTCCMFD::menuSetLOIAzi()
{
	if (G->LOImaneuver == 0 || G->LOImaneuver == 1)
	{
		if (G->LOIOption == 0)
		{
			bool LOIAziInput(void *id, char *str, void *data);
			oapiOpenInputBox("Choose the approach azimuth:", LOIAziInput, 0, 20, (void*)this);
		}
	}
}

bool LOIAziInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LOIAzi(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LOIAzi(double azi)
{
	this->G->LOIazi = azi*RAD;
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

void ApolloRTCCMFD::menuSetDOIGET()
{
	bool DOIGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for the maneuver (Format: hhh:mm:ss)", DOIGETInput, 0, 20, (void*)this);
}

bool DOIGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_DOIGET(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_DOIGET(double time)
{
	G->DOIGET = time;
}


void ApolloRTCCMFD::menuLSAlt()
{
	bool LSAltInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the landing site altitude:", LSAltInput, 0, 20, (void*)this);
}

bool LSAltInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LSAlt(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LSAlt(double alt)
{
	this->G->LSAlt = alt*1852.0;
}

void ApolloRTCCMFD::menuSetDOIRevs()
{
	bool DOIRevsInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the number of revolutions:", DOIRevsInput, 0, 20, (void*)this);
}

bool DOIRevsInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_DOIRevs(atoi(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_DOIRevs(int N)
{
	this->G->DOI_N = N;
}

void ApolloRTCCMFD::menuSetDOIPeriAng()
{
	bool DOIPeriAngInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the angle from perilune to landing site:", DOIPeriAngInput, 0, 20, (void*)this);
}

bool DOIPeriAngInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_DOIPeriAng(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_DOIPeriAng(double ang)
{
	this->G->DOI_PeriAng = ang*RAD;
}

void ApolloRTCCMFD::menuDOIOption()
{
	if (G->DOI_option < 1)
	{
		G->DOI_option++;
	}
	else
	{
		G->DOI_option = 0;
	}
}

void ApolloRTCCMFD::menuDOICalc()
{
	G->DOICalc();
}

void ApolloRTCCMFD::menuSwitchSkylabManeuver()
{
	if (G->Skylabmaneuver < 7)
	{
		G->Skylabmaneuver++;
	}
	else
	{
		G->Skylabmaneuver = 0;
	}
}

void ApolloRTCCMFD::menuCyclePlaneChange()
{
	if (G->Skylabmaneuver == 1 || G->Skylabmaneuver == 2)
	{
		G->Skylab_NPCOption = !G->Skylab_NPCOption;
	}
}

void ApolloRTCCMFD::menuCyclePCManeuver()
{
	if (G->Skylabmaneuver == 7)
	{
		G->Skylab_PCManeuver = !G->Skylab_PCManeuver;
	}
}

void ApolloRTCCMFD::menuSetSkylabGET()
{
	if (G->Skylabmaneuver == 5)
	{
		if (G->target == NULL)
		{
			return;
		}

		double mu, SVMJD, dt1;
		VECTOR3 RA0_orb, VA0_orb, RP0_orb, VP0_orb, RA0, VA0, RP0, VP0;
		OBJHANDLE gravref = G->rtcc->AGCGravityRef(G->vessel);

		mu = GGRAV*oapiGetMass(gravref);

		G->vessel->GetRelativePos(gravref, RA0_orb);
		G->vessel->GetRelativeVel(gravref, VA0_orb);
		G->target->GetRelativePos(gravref, RP0_orb);
		G->target->GetRelativeVel(gravref, VP0_orb);
		SVMJD = oapiGetSimMJD();

		RA0 = _V(RA0_orb.x, RA0_orb.z, RA0_orb.y);	//The following equations use another coordinate system than Orbiter
		VA0 = _V(VA0_orb.x, VA0_orb.z, VA0_orb.y);
		RP0 = _V(RP0_orb.x, RP0_orb.z, RP0_orb.y);
		VP0 = _V(VP0_orb.x, VP0_orb.z, VP0_orb.y);

		dt1 = OrbMech::findelev(RA0, VA0, RP0, VP0, SVMJD, G->Skylab_E_L, gravref);
		G->t_TPI = dt1 + (SVMJD - G->GETbase) * 24.0 * 60.0 * 60.0;
	}
	else if (G->Skylabmaneuver == 6)
	{
		bool SkylabDTTPMInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the relative time to TPI for the maneuver", SkylabDTTPMInput, 0, 20, (void*)this);
	}
	else
	{
		bool SkylabGETInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the GET for the maneuver (Format: hhh:mm:ss)", SkylabGETInput, 0, 20, (void*)this);
	}
}

bool SkylabGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "TPI=%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_t_TPI(t1time);
		return true;
	}
	else if (strcmp(str, "PeT") == 0)
	{
		double pet;
		pet = ((ApolloRTCCMFD*)data)->timetoperi();
		((ApolloRTCCMFD*)data)->set_SkylabGET(pet);
		return true;
	}
	else if (strcmp(str, "ApT") == 0)
	{
		double pet;
		pet = ((ApolloRTCCMFD*)data)->timetoapo();
		((ApolloRTCCMFD*)data)->set_SkylabGET(pet);
		return true;
	}
	else if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_SkylabGET(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SkylabGET(double time)
{
	if (G->Skylabmaneuver == 0)
	{
		G->SkylabTPIGuess = time;
	}
	else if (G->Skylabmaneuver == 1)
	{
		G->Skylab_t_NC1 = time;
	}
	else if (G->Skylabmaneuver == 2)
	{
		G->Skylab_t_NC2 = time;
	}
	else if (G->Skylabmaneuver == 3)
	{
		G->Skylab_t_NCC = time;
	}
	else if (G->Skylabmaneuver == 4)
	{
		G->Skylab_t_NSR = time;
	}
}

bool SkylabDTTPMInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_SkylabDTTPM(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SkylabDTTPM(double dt)
{
	this->G->Skylab_dt_TPM = dt*60.0;
}

void ApolloRTCCMFD::set_t_TPI(double time)
{
	G->t_TPI = time;
}

void ApolloRTCCMFD::menuSkylabCalc()
{
	if (G->target != NULL)
	{
		G->SkylabCalc();
	}
}

void ApolloRTCCMFD::menuSetSkylabNC()
{
	if (G->Skylabmaneuver == 1)
	{
		bool SkylabNCInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the number of revolutions:", SkylabNCInput, 0, 20, (void*)this);
	}
}

bool SkylabNCInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_SkylabNC(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SkylabNC(double N)
{
	this->G->Skylab_n_C = N;
}

void ApolloRTCCMFD::menuSetSkylabDH1()
{
	if (G->Skylabmaneuver == 1)
	{
		bool SkylabDH1Input(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the landing site altitude:", SkylabDH1Input, 0, 20, (void*)this);
	}
}

bool SkylabDH1Input(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_SkylabDH1(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SkylabDH1(double dh)
{
	this->G->SkylabDH1 = dh*1852.0;
}

void ApolloRTCCMFD::menuSetSkylabDH2()
{
	if (G->Skylabmaneuver == 1 || G->Skylabmaneuver == 2 || G->Skylabmaneuver == 3)
	{
		bool SkylabDH2Input(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the Delta Height at NSR:", SkylabDH2Input, 0, 20, (void*)this);
	}
}

bool SkylabDH2Input(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_SkylabDH2(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SkylabDH2(double dh)
{
	this->G->SkylabDH2 = dh*1852.0;
}

void ApolloRTCCMFD::menuSetSkylabEL()
{
	if (G->Skylabmaneuver > 0 && G->Skylabmaneuver <= 5)
	{
		bool SkylabELInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the elevation angle at TPI:", SkylabELInput, 0, 20, (void*)this);
	}
}

bool SkylabELInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_SkylabEL(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SkylabEL(double E_L)
{
	this->G->Skylab_E_L = E_L*RAD;
}

void ApolloRTCCMFD::menuPCCalc()
{
	if (G->target != NULL || !G->PClanded)
	{
		G->PCCalc();
	}
}

void ApolloRTCCMFD::menuSetPCTIGguess()
{
	bool PCGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for the maneuver (Format: hhh:mm:ss)", PCGETInput, 0, 20, (void*)this);
}

bool PCGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_PCTIGguess(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_PCTIGguess(double time)
{
	G->PCEarliestGET = time;
}

void ApolloRTCCMFD::menuSetPCAlignGET()
{
	bool PCAlignGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the alignment time (Format: hhh:mm:ss)", PCAlignGETInput, 0, 20, (void*)this);
}

bool PCAlignGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_PCAlignGET(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_PCAlignGET(double time)
{
	G->PCAlignGET = time;
}

void ApolloRTCCMFD::menuSetPCLanded()
{
	G->PClanded = !G->PClanded;
}

void ApolloRTCCMFD::menuSetTPIguess()
{
	bool TPIGuessInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for TPI (Format: hhh:mm:ss)", TPIGuessInput, 0, 20, (void*)this);
}

bool TPIGuessInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_TPIguess(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TPIguess(double time)
{
	G->t_TPIguess = time;
}

void ApolloRTCCMFD::menuTMLat()
{
	bool TMLatInput(void* id, char *str, void *data);
	oapiOpenInputBox("Latitude in degrees:", TMLatInput, 0, 20, (void*)this);
}

bool TMLatInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_TMLat(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TMLat(double lat)
{
	this->G->TMLat = lat*RAD;
}

void ApolloRTCCMFD::menuTMLng()
{
	bool TMLngInput(void* id, char *str, void *data);
	oapiOpenInputBox("Longitude in degrees:", TMLngInput, 0, 20, (void*)this);
}

bool TMLngInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_TMLng(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TMLng(double lng)
{
	this->G->TMLng = lng*RAD;
}

void ApolloRTCCMFD::menuTMAzi()
{
	bool TMAziInput(void* id, char *str, void *data);
	oapiOpenInputBox("Approach azimuth in degrees:", TMAziInput, 0, 20, (void*)this);
}

bool TMAziInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_TMAzi(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TMAzi(double azi)
{
	this->G->TMAzi = azi*RAD;
}

void ApolloRTCCMFD::menuTMDistance()
{
	bool TMDistanceInput(void* id, char *str, void *data);
	oapiOpenInputBox("Distance in feet:", TMDistanceInput, 0, 20, (void*)this);
}

bool TMDistanceInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_TMDistance(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TMDistance(double distance)
{
	this->G->TMDistance = distance*0.3048;
}

void ApolloRTCCMFD::menuTMStepSize()
{
	bool TMStepSizeInput(void* id, char *str, void *data);
	oapiOpenInputBox("Step size in feet:", TMStepSizeInput, 0, 20, (void*)this);
}

bool TMStepSizeInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_TMStepSize(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_TMStepSize(double step)
{
	this->G->TMStepSize = step*0.3048;
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

void ApolloRTCCMFD::menuLunarLiftoffCalc()
{
	if (G->target != NULL)
	{
		G->LunarLiftoffCalc();
	}
}

void ApolloRTCCMFD::menuLunarLiftoffTimeOption()
{
	if (G->LunarLiftoffTimeOption < 2)
	{
		G->LunarLiftoffTimeOption++;
	}
	else
	{
		G->LunarLiftoffTimeOption = 0;
	}
}

void ApolloRTCCMFD::menuSetLiftoffDT()
{
	if (G->LunarLiftoffTimeOption == 1)
	{
		bool LiftoffDTInput(void* id, char *str, void *data);
		oapiOpenInputBox("DT between insertion and TPI:", LiftoffDTInput, 0, 20, (void*)this);
	}
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
	G->DT_Ins_TPI = dt * 60.0;
}

void ApolloRTCCMFD::menuSetEMPUplinkP99()
{
	G->EMPUplinkNumber = 0;
	G->EMPUplinkType = 0;
}

void ApolloRTCCMFD::menuEMPUplink()
{
	if (G->EMPUplinkType == 0)
	{
		G->EMPP99Uplink(G->EMPUplinkNumber);
	}
}

void ApolloRTCCMFD::menuSetEMPUplinkNumber()
{
	if (G->EMPUplinkType == 0)
	{
		if (G->EMPUplinkNumber < 3)
		{
			G->EMPUplinkNumber++;
		}
		else
		{
			G->EMPUplinkNumber = 0;
		}
	}
}

void ApolloRTCCMFD::menuNavCheckPADCalc()
{
	G->NavCheckPAD();
}

void ApolloRTCCMFD::menuSetNavCheckGET()
{
	bool NavCheckGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for the Nav Check (Format: hhh:mm:ss)", NavCheckGETInput, 0, 20, (void*)this);
}

bool NavCheckGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_NavCheckGET(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_NavCheckGET(double time)
{
	G->navcheckpad.NavChk[0] = time;
}

void ApolloRTCCMFD::menuRequestLTMFD()
{
	if (G->vesseltype < 2)
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
}

void ApolloRTCCMFD::menuDKICalc()
{
	if (G->target != NULL)
	{
		G->DKICalc();
	}
}

void ApolloRTCCMFD::DKITIGDialogue()
{
	bool DKITIGInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the time of ignition (Format: hhh:mm:ss)", DKITIGInput, 0, 20, (void*)this);
}

bool DKITIGInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	double pdidt;
	if (strcmp(str, "PeT") == 0)
	{
		double pet;
		pet = ((ApolloRTCCMFD*)data)->timetoperi();
		((ApolloRTCCMFD*)data)->set_DKITIG(pet);
		return true;
	}
	else if (sscanf(str, "PDI+%lf", &pdidt) == 1)
	{
		((ApolloRTCCMFD*)data)->set_DKITIG_DT_PDI(pdidt * 60.0);
		return true;
	}
	else if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_DKITIG(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_DKITIG(double time)
{
	G->DKI_TIG = time;
}

void ApolloRTCCMFD::set_DKITIG_DT_PDI(double dt)
{
	G->DKI_TIG = G->pdipad.GETI + dt;
}

void ApolloRTCCMFD::menuCycleDKIProfile()
{
	if (G->DKI_Profile < 2)
	{
		G->DKI_Profile++;
	}
	else
	{
		G->DKI_Profile = 0;
	}
}

void ApolloRTCCMFD::menuCycleDKITPIMode()
{
	if (G->DKI_TPI_Mode < 2)
	{
		G->DKI_TPI_Mode++;
	}
	else
	{
		G->DKI_TPI_Mode = 0;
	}
}

void ApolloRTCCMFD::menuCycleDKIManeuverLine()
{
	G->DKI_Maneuver_Line = !G->DKI_Maneuver_Line;
}

void ApolloRTCCMFD::menuCycleDKIRadialComponent()
{
	G->DKI_Radial_DV = !G->DKI_Radial_DV;
}

void ApolloRTCCMFD::menuSetDKIElevation()
{
	bool DKIElevInput(void* id, char *str, void *data);
	oapiOpenInputBox("Elevation in degrees:", DKIElevInput, 0, 20, (void*)this);
}

bool DKIElevInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_DKIElevation(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_DKIElevation(double elev)
{
	this->G->lambertelev = elev * RAD;
}

void ApolloRTCCMFD::DKITPIDTDialogue()
{
	if (G->DKI_TPI_Mode == 2)
	{
		bool DKITPIDTInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the TPI time before sunrise in minutes", DKITPIDTInput, 0, 20, (void*)this);
	}
}

bool DKITPIDTInput(void *id, char *str, void *data)
{
	double dt;
	if (sscanf(str, "%lf", &dt) == 1)
	{
		((ApolloRTCCMFD*)data)->set_DKITPIDT(dt);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_DKITPIDT(double time)
{
	G->DKI_dt_TPI_sunrise = time * 60.0;
}

void ApolloRTCCMFD::DKINDialogue()
{
	bool DKINInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the number of half-revs between CSI and CDH", DKINInput, 0, 20, (void*)this);
}

bool DKINInput(void *id, char *str, void *data)
{
	int N;
	if (sscanf(str, "%d", &N) == 1)
	{
		((ApolloRTCCMFD*)data)->set_DKIN(N);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_DKIN(int N)
{
	G->DKI_N = N;
}

void ApolloRTCCMFD::menuDKIDeltaT1()
{
	if (G->DKI_Maneuver_Line == false)
	{
		bool DKIDT1Input(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the time between abort and Boost/CSI:", DKIDT1Input, 0, 20, (void*)this);
	}
}

bool DKIDT1Input(void *id, char *str, void *data)
{
	double dt;
	if (sscanf(str, "%lf", &dt) == 1)
	{
		((ApolloRTCCMFD*)data)->set_DKIDT1(dt);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_DKIDT1(double dt)
{
	G->DKI_dt_PBH = dt * 60.0;
}

void ApolloRTCCMFD::menuDKIDeltaT2()
{
	if (G->DKI_Maneuver_Line == false)
	{
		bool DKIDT2Input(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the time between boost and HAM:", DKIDT2Input, 0, 20, (void*)this);
	}
}

bool DKIDT2Input(void *id, char *str, void *data)
{
	double dt;
	if (sscanf(str, "%lf", &dt) == 1)
	{
		((ApolloRTCCMFD*)data)->set_DKIDT2(dt);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_DKIDT2(double dt)
{
	G->DKI_dt_BHAM = dt * 60.0;
}

void ApolloRTCCMFD::menuDKIDeltaT3()
{
	if (G->DKI_Maneuver_Line == false)
	{
		bool DKIDT3Input(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the time between HAM and CSI:", DKIDT3Input, 0, 20, (void*)this);
	}
}

bool DKIDT3Input(void *id, char *str, void *data)
{
	double dt;
	if (sscanf(str, "%lf", &dt) == 1)
	{
		((ApolloRTCCMFD*)data)->set_DKIDT3(dt);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_DKIDT3(double dt)
{
	G->DKI_dt_HAMH = dt * 60.0;
}

void ApolloRTCCMFD::menuDAPPADCalc()
{
	G->DAPPADCalc();
}

void ApolloRTCCMFD::menuLaunchAzimuthCalc()
{
	if (!stricmp(G->vessel->GetClassName(), "ProjectApollo\\Saturn5") ||
		!stricmp(G->vessel->GetClassName(), "ProjectApollo/Saturn5")) {

		SaturnV *SatV = (SaturnV*)G->vessel;
		if (SatV->iu)
		{
			LVDCSV *lvdc = (LVDCSV*)SatV->iu->lvdc;

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

void ApolloRTCCMFD::menuSetAGCEphemBRCSEpoch()
{
	bool AGCEphemBRCSEpochInput(void* id, char *str, void *data);
	oapiOpenInputBox("MJD of BRCS epoch:", AGCEphemBRCSEpochInput, 0, 20, (void*)this);
}

bool AGCEphemBRCSEpochInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_AGCEphemBRCSEpoch(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_AGCEphemBRCSEpoch(double mjd)
{
	this->G->AGCEphemBRCSEpoch = mjd;
}

void ApolloRTCCMFD::menuSetAGCEphemTEphemZero()
{
	bool AGCEphemTEphemZeroInput(void* id, char *str, void *data);
	oapiOpenInputBox("MJD of previous July 1st, midnight:", AGCEphemTEphemZeroInput, 0, 20, (void*)this);
}

bool AGCEphemTEphemZeroInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_AGCEphemTEphemZero(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_AGCEphemTEphemZero(double mjd)
{
	this->G->AGCEphemTEphemZero = mjd;
}

void ApolloRTCCMFD::menuSetAGCEphemTIMEM0()
{
	bool AGCEphemTIMEM0Input(void* id, char *str, void *data);
	oapiOpenInputBox("MJD of ephemeris time reference:", AGCEphemTIMEM0Input, 0, 20, (void*)this);
}

bool AGCEphemTIMEM0Input(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_AGCEphemTIMEM0(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_AGCEphemTIMEM0(double mjd)
{
	this->G->AGCEphemTIMEM0 = mjd;
}

void ApolloRTCCMFD::menuGenerateAGCEphemeris()
{
	G->GenerateAGCEphemeris();
}

void ApolloRTCCMFD::SStoHHMMSS(double time, int &hours, int &minutes, double &seconds)
{
	double mins;
	hours = (int)trunc(time / 3600.0);
	mins = fmod(time / 60.0, 60.0);
	minutes = (int)trunc(mins);
	seconds = (mins - minutes) * 60.0;
}