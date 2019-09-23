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

// ==============================================================
// Global variables

ARoapiModule *g_coreMod;
int g_MFDmode; // identifier for new MFD mode
ARCore *GCoreData[32];
VESSEL *GCoreVessel[32];
AR_GCore *g_SC = NULL;      // points to the static core, root of all persistence
int nGutsUsed;
char Buffer[100];
bool initialised = false;

// ==============================================================
// MFD class implementation

// Constructor
ApolloRTCCMFD::ApolloRTCCMFD (DWORD w, DWORD h, VESSEL *vessel, UINT im)
: MFD2 (w, h, vessel)
{
	if (!g_SC) {
		g_SC = new AR_GCore(vessel);                     // First time only in this Orbiter session. Init the static core.
	}
	GC = g_SC;                                  // Make the ApolloRTCCMFD instance Global Core point to the static core. 

	//font = oapiCreateFont(w / 20, true, "Arial", FONT_NORMAL, 0);
	font = oapiCreateFont(w / 20, true, "Courier", FONT_NORMAL, 0);
	font2 = oapiCreateFont(w / 24, true, "Courier", FONT_NORMAL, 0);
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
		screen = 0;
		G = GCoreData[nGutsUsed];
		GCoreVessel[nGutsUsed] = vessel;
		nGutsUsed++;
	}

	marker = 0;
}

// Destructor
ApolloRTCCMFD::~ApolloRTCCMFD ()
{
	oapiReleaseFont (font);
	// Add MFD cleanup code here
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
	oapiWriteScenario_int(scn, "REFSMMATcur", G->REFSMMATcur);
	oapiWriteScenario_int(scn, "REFSMMATopt", G->REFSMMATopt);
	papiWriteScenario_double(scn, "REFSMMATTime", G->REFSMMATTime);
	oapiWriteScenario_int(scn, "REFSMMATupl", G->REFSMMATupl);
	papiWriteScenario_bool(scn, "REFSMMATHeadsUp", G->REFSMMATHeadsUp);
	papiWriteScenario_double(scn, "T1", G->T1);
	papiWriteScenario_double(scn, "T2", G->T2);
	papiWriteScenario_double(scn, "CDHTIME", G->CDHtime);
	papiWriteScenario_double(scn, "SPQTIG", G->SPQTIG);
	oapiWriteScenario_int(scn, "CDHTIMEMODE", G->CDHtimemode);
	papiWriteScenario_double(scn, "DH", G->DH);
	oapiWriteScenario_int(scn, "N", G->N);
	oapiWriteScenario_int(scn, "LAMBERTOPT", G->lambertopt);
	papiWriteScenario_bool(scn, "LAMBERTMULTI", G->lambertmultiaxis);
	oapiWriteScenario_int(scn, "TWOIMPULSEMODE", G->twoimpulsemode);
	papiWriteScenario_double(scn, "lambertelev", G->lambertelev);
	papiWriteScenario_vec(scn, "SPQDeltaV", G->SPQDeltaV);
	if (G->target != NULL)
	{
		sprintf(Buffer2, G->target->GetName());
		oapiWriteScenario_string(scn, "TARGET", Buffer2);
	}
	oapiWriteScenario_int(scn, "TARGETNUMBER", G->targetnumber);
	papiWriteScenario_vec(scn, "OFFVEC", G->offvec);
	papiWriteScenario_double(scn, "ANGDEG", G->angdeg);
	oapiWriteScenario_int(scn, "MISSION", GC->mission);
	papiWriteScenario_double(scn, "GETBASE", GC->GETbase);
	papiWriteScenario_double(scn, "LSLat", GC->LSLat);
	papiWriteScenario_double(scn, "LSLng", GC->LSLng);
	papiWriteScenario_double(scn, "LSAlt", GC->LSAlt);
	papiWriteScenario_double(scn, "TLAND", GC->t_Land);
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

	oapiWriteScenario_int(scn, "TLCCMANEUVER", G->TLCCmaneuver);
	papiWriteScenario_double(scn, "TLCCGET", G->TLCC_GET);
	papiWriteScenario_double(scn, "TLCCPeriGET", GC->TLCCPeriGET);
	papiWriteScenario_double(scn, "TLCCPeriGETcor", G->TLCCPeriGETcor);
	papiWriteScenario_double(scn, "TLCCReentryGET", G->TLCCReentryGET);
	papiWriteScenario_double(scn, "TLCCNodeLat", GC->TLCCNodeLat);
	papiWriteScenario_double(scn, "TLCCFreeReturnEMPLat", GC->TLCCFreeReturnEMPLat);
	papiWriteScenario_double(scn, "TLCCNonFreeReturnEMPLat", GC->TLCCNonFreeReturnEMPLat);
	papiWriteScenario_double(scn, "TLCCNodeLng", GC->TLCCNodeLng);
	papiWriteScenario_double(scn, "TLCCLAHPeriAlt", GC->TLCCLAHPeriAlt);
	papiWriteScenario_double(scn, "TLCCFlybyPeriAlt", GC->TLCCFlybyPeriAlt);
	papiWriteScenario_double(scn, "TLCCNodeAlt", GC->TLCCNodeAlt);
	papiWriteScenario_double(scn, "TLCCNodeGET", GC->TLCCNodeGET);
	papiWriteScenario_double(scn, "LOIapo", GC->LOIapo);
	papiWriteScenario_double(scn, "LOIperi", GC->LOIperi);
	papiWriteScenario_double(scn, "LOIazi", GC->LOIazi);
	papiWriteScenario_vec(scn, "TLCCDV", G->TLCC_dV_LVLH);
	papiWriteScenario_vec(scn, "LOIDV", G->LOI_dV_LVLH);
	papiWriteScenario_double(scn, "TLCCTIG", G->TLCC_TIG);
	papiWriteScenario_double(scn, "LOITIG", G->LOI_TIG);
	oapiWriteScenario_int(scn, "LOIEllipseRotation", GC->LOIEllipseRotation);
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

	oapiWriteScenario_int(scn, "DOI_N", GC->rtcc->med_k17.DwellOrbits);
	papiWriteScenario_double(scn, "LDPPGETTH1", GC->rtcc->med_k16.GETTH1);
	papiWriteScenario_double(scn, "LDPPGETTH2", GC->rtcc->med_k16.GETTH2);
	papiWriteScenario_double(scn, "LDPPGETTH3", GC->rtcc->med_k16.GETTH3);
	papiWriteScenario_double(scn, "LDPPGETTH4", GC->rtcc->med_k16.GETTH4);
	papiWriteScenario_double(scn, "DOI_PERIANG", GC->rtcc->med_k17.DescentFlightArc);
	papiWriteScenario_double(scn, "DOI_alt", GC->rtcc->med_k17.DescIgnHeight);

	papiWriteScenario_double(scn, "DKI_TIG", G->DKI_TIG);
	papiWriteScenario_double(scn, "t_Liftoff_guess", G->t_Liftoff_guess);
	papiWriteScenario_double(scn, "t_TPIguess", G->t_TPIguess);
	papiWriteScenario_double(scn, "DT_Ins_TPI", GC->DT_Ins_TPI);
	oapiWriteScenario_int(scn, "DKI_Profile", G->DKI_Profile);
	oapiWriteScenario_int(scn, "DKI_TPI_Mode", G->DKI_TPI_Mode);
	papiWriteScenario_bool(scn, "DKI_Maneuver_Line", G->DKI_Maneuver_Line);
	papiWriteScenario_bool(scn, "DKI_Radial_DV", G->DKI_Radial_DV);
	oapiWriteScenario_int(scn, "DKI_N_HC", G->DKI_N_HC);
	oapiWriteScenario_int(scn, "DKI_N_PB", G->DKI_N_PB);

	papiWriteScenario_double(scn, "AGSKFACTOR", G->AGSKFactor);

	oapiWriteScenario_int(scn, "MPTCM_InitConfigCode", GC->mptable.cmtable.InitConfigCode);
	papiWriteScenario_double(scn, "MPTCM_CSMInitMass", GC->mptable.cmtable.CSMInitMass);
	papiWriteScenario_double(scn, "MPTCM_LMInitMass", GC->mptable.cmtable.LMInitMass);
	papiWriteScenario_double(scn, "MPTCM_SIVBInitMass", GC->mptable.cmtable.SIVBInitMass);
	papiWriteScenario_double(scn, "MPTCM_TotalInitMass", GC->mptable.cmtable.TotalInitMass);
	oapiWriteScenario_int(scn, "MPTCM_number", GC->pCSMnumber);
	oapiWriteScenario_int(scn, "MPTCM_TUP", GC->mptable.cmtable.TUP);
	papiWriteScenario_SV(scn, "MPTCM_ANCHOR", GC->mptable.cmtable.AnchorVector);
	
	oapiWriteScenario_int(scn, "MPTLM_InitConfigCode", GC->mptable.lmtable.InitConfigCode);
	papiWriteScenario_double(scn, "MPTLM_CSMInitMass", GC->mptable.lmtable.CSMInitMass);
	papiWriteScenario_double(scn, "MPTLM_LMInitMass", GC->mptable.lmtable.LMInitMass);
	papiWriteScenario_double(scn, "MPTLM_SIVBInitMass", GC->mptable.lmtable.SIVBInitMass);
	papiWriteScenario_double(scn, "MPTLM_TotalInitMass", GC->mptable.lmtable.TotalInitMass);
	oapiWriteScenario_int(scn, "MPTLM_number", GC->pLMnumber);
	oapiWriteScenario_int(scn, "MPTLM_TUP", GC->mptable.lmtable.TUP);
	papiWriteScenario_SV(scn, "MPTLM_ANCHOR", GC->mptable.lmtable.AnchorVector);
}

void ApolloRTCCMFD::ReadStatus(FILEHANDLE scn)
{
	char *line;
	char Buffer2[100];
	bool istarget = false;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, "END_MFD", 7))
			break;

		//papiReadScenario_int(line, "SCREEN", G->screen);
		papiReadScenario_int(line, "VESSELTYPE", G->vesseltype);
		papiReadScenario_double(line, "SXTSTARDTIME", G->sxtstardtime);
		papiReadScenario_mat(line, "REFSMMAT", G->REFSMMAT);
		papiReadScenario_int(line, "REFSMMATcur", G->REFSMMATcur);
		papiReadScenario_int(line, "REFSMMATopt", G->REFSMMATopt);
		papiReadScenario_double(line, "REFSMMATTime", G->REFSMMATTime);
		papiReadScenario_int(line, "REFSMMATupl", G->REFSMMATupl);
		papiReadScenario_bool(line, "REFSMMATHeadsUp", G->REFSMMATHeadsUp);
		papiReadScenario_double(line, "T1", G->T1);
		papiReadScenario_double(line, "T2", G->T2);
		papiReadScenario_double(line, "CDHTIME", G->CDHtime);
		papiReadScenario_double(line, "SPQTIG", G->SPQTIG);
		papiReadScenario_int(line, "CDHTIMEMODE", G->CDHtimemode);
		papiReadScenario_double(line, "DH", G->DH);
		papiReadScenario_int(line, "N", G->N);
		papiReadScenario_int(line, "LAMBERTOPT", G->lambertopt);
		papiReadScenario_bool(line, "LAMBERTMULTI", G->lambertmultiaxis);
		papiReadScenario_int(line, "TWOIMPULSEMODE", G->twoimpulsemode);
		papiReadScenario_double(line, "lambertelev", G->lambertelev);
		papiReadScenario_vec(line, "SPQDeltaV", G->SPQDeltaV);

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
		papiReadScenario_int(line, "MISSION", GC->mission);
		papiReadScenario_double(line, "GETBASE", GC->GETbase);
		papiReadScenario_double(line, "LSLat", GC->LSLat);
		papiReadScenario_double(line, "LSLng", GC->LSLng);
		papiReadScenario_double(line, "LSAlt", GC->LSAlt);
		papiReadScenario_double(line, "TLAND", GC->t_Land);
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

		papiReadScenario_int(line, "TLCCMANEUVER", G->TLCCmaneuver);
		papiReadScenario_double(line, "TLCCGET", G->TLCC_GET);
		papiReadScenario_double(line, "TLCCPeriGET", GC->TLCCPeriGET);
		papiReadScenario_double(line, "TLCCPeriGETcor", G->TLCCPeriGETcor);
		papiReadScenario_double(line, "TLCCReentryGET", G->TLCCReentryGET);
		papiReadScenario_double(line, "TLCCNodeLat", GC->TLCCNodeLat);
		papiReadScenario_double(line, "TLCCFreeReturnEMPLat", GC->TLCCFreeReturnEMPLat);
		papiReadScenario_double(line, "TLCCNonFreeReturnEMPLat", GC->TLCCNonFreeReturnEMPLat);
		papiReadScenario_double(line, "TLCCNodeLng", GC->TLCCNodeLng);
		papiReadScenario_double(line, "TLCCLAHPeriAlt", GC->TLCCLAHPeriAlt);
		papiReadScenario_double(line, "TLCCFlybyPeriAlt", GC->TLCCFlybyPeriAlt);
		papiReadScenario_double(line, "TLCCNodeAlt", GC->TLCCNodeAlt);
		papiReadScenario_double(line, "TLCCNodeGET", GC->TLCCNodeGET);
		papiReadScenario_double(line, "LOIapo", GC->LOIapo);
		papiReadScenario_double(line, "LOIperi", GC->LOIperi);
		papiReadScenario_double(line, "LOIazi", GC->LOIazi);
		papiReadScenario_vec(line, "TLCCDV", G->TLCC_dV_LVLH);
		papiReadScenario_vec(line, "LOIDV", G->LOI_dV_LVLH);
		papiReadScenario_double(line, "TLCCTIG", G->TLCC_TIG);
		papiReadScenario_double(line, "LOITIG", G->LOI_TIG);
		papiReadScenario_int(line, "LOIEllipseRotation", GC->LOIEllipseRotation);
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

		papiReadScenario_int(line, "DOI_N", GC->rtcc->med_k17.DwellOrbits);
		papiReadScenario_double(line, "LDPPGETTH1", GC->rtcc->med_k16.GETTH1);
		papiReadScenario_double(line, "LDPPGETTH2", GC->rtcc->med_k16.GETTH2);
		papiReadScenario_double(line, "LDPPGETTH3", GC->rtcc->med_k16.GETTH3);
		papiReadScenario_double(line, "LDPPGETTH4", GC->rtcc->med_k16.GETTH4);
		papiReadScenario_double(line, "DOI_PERIANG", GC->rtcc->med_k17.DescentFlightArc);
		papiReadScenario_double(line, "DOI_alt", GC->rtcc->med_k17.DescIgnHeight);

		papiReadScenario_double(line, "DKI_TIG", G->DKI_TIG);
		papiReadScenario_double(line, "t_Liftoff_guess", G->t_Liftoff_guess);
		papiReadScenario_double(line, "t_TPIguess", G->t_TPIguess);
		papiReadScenario_double(line, "DT_Ins_TPI", GC->DT_Ins_TPI);
		papiReadScenario_int(line, "DKI_Profile", G->DKI_Profile);
		papiReadScenario_int(line, "DKI_TPI_Mode", G->DKI_TPI_Mode);
		papiReadScenario_bool(line, "DKI_Maneuver_Line", G->DKI_Maneuver_Line);
		papiReadScenario_bool(line, "DKI_Radial_DV", G->DKI_Radial_DV);
		papiReadScenario_int(line, "DKI_N_HC", G->DKI_N_HC);
		papiReadScenario_int(line, "DKI_N_PB", G->DKI_N_PB);

		papiReadScenario_double(line, "AGSKFACTOR", G->AGSKFactor);

		papiReadScenario_int(line, "MPTCM_InitConfigCode", GC->mptable.cmtable.InitConfigCode);
		papiReadScenario_double(line, "MPTCM_CSMInitMass", GC->mptable.cmtable.CSMInitMass);
		papiReadScenario_double(line, "MPTCM_LMInitMass", GC->mptable.cmtable.LMInitMass);
		papiReadScenario_double(line, "MPTCM_SIVBInitMass", GC->mptable.cmtable.SIVBInitMass);
		papiReadScenario_double(line, "MPTCM_TotalInitMass", GC->mptable.cmtable.TotalInitMass);
		papiReadScenario_int(line, "MPTCM_number", GC->pCSMnumber);
		papiReadScenario_int(line, "MPTCM_TUP", GC->mptable.cmtable.TUP);
		papiReadScenario_SV(line, "MPTCM_ANCHOR", GC->mptable.cmtable.AnchorVector);

		papiReadScenario_int(line, "MPTLM_InitConfigCode", GC->mptable.lmtable.InitConfigCode);
		papiReadScenario_double(line, "MPTLM_CSMInitMass", GC->mptable.lmtable.CSMInitMass);
		papiReadScenario_double(line, "MPTLM_LMInitMass", GC->mptable.lmtable.LMInitMass);
		papiReadScenario_double(line, "MPTLM_SIVBInitMass", GC->mptable.lmtable.SIVBInitMass);
		papiReadScenario_double(line, "MPTLM_TotalInitMass", GC->mptable.lmtable.TotalInitMass);
		papiReadScenario_int(line, "MPTLM_number", GC->pLMnumber);
		papiReadScenario_int(line, "MPTLM_TUP", GC->mptable.lmtable.TUP);
		papiReadScenario_SV(line, "MPTLM_ANCHOR", GC->mptable.lmtable.AnchorVector);

		//G->coreButtons.SelectPage(this, G->screen);
	}

	if (GC->pCSMnumber >= 0)
	{
		OBJHANDLE hVes = oapiGetVesselByIndex(GC->pCSMnumber);
		if (hVes)
		{
			GC->pCSM = oapiGetVesselInterface(hVes);
		}
	}
	if (GC->pLMnumber >= 0)
	{
		OBJHANDLE hVes = oapiGetVesselByIndex(GC->pLMnumber);
		if (hVes)
		{
			GC->pLM = oapiGetVesselInterface(hVes);
		}
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
		skp->Text(1 * W / 8, 8 * H / 14, "MCC Displays", 12);
		skp->Text(1 * W / 8, 10 * H / 14, "Mission Plan Table", 18);
		skp->Text(1 * W / 8, 12 * H / 14, "Configuration", 13);

		skp->Text(5 * W / 8, 2 * H / 14, "Uplinks", 7);
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

		if (G->lambertElevOpt == 0)
		{
			GET_Display(Buffer, G->T1);
		}
		else
		{
			sprintf(Buffer, "E = %.2f°", G->lambertelev*DEG);
		}
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		
		if (G->lambertTPFOpt == 0)
		{
			GET_Display(Buffer, G->T2);
		}
		else if (G->lambertTPFOpt == 1)
		{
			sprintf(Buffer, "DT = %.0f min", G->lambertDT / 60.0);
		}
		else
		{
			sprintf(Buffer, "WT = %.2f°", G->lambertWT*DEG);
		}
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

		GET_Display(Buffer, G->TwoImpulse_TIG);
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
		skp->Text(6 * W / 8, (int)(0.5 * H / 14), "Coelliptic", 10);

		if (G->SPQMode == 0)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "CSI", 3);

			if (G->CDHtimemode == 0)
			{
				skp->Text(1 * W / 8, 4 * H / 14, "Fixed TPI time", 14);
			}
			else if (G->CDHtimemode == 1)
			{
				skp->Text(1 * W / 8, 4 * H / 14, "Fixed DH", 8);
			}

			GET_Display(Buffer, G->CSItime);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(1 * W / 8, 2 * H / 14, "CDH", 3);

			if (G->CDHtimemode == 0)
			{
				skp->Text(1 * W / 8, 4 * H / 14, "Fixed", 5);
			}
			else if (G->CDHtimemode == 1)
			{
				skp->Text(1 * W / 8, 4 * H / 14, "Find GETI", 9);
			}

			GET_Display(Buffer, G->CDHtime);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
		}
		
		sprintf(Buffer, "%f NM", G->DH / 1852.0);
		skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.2f°", G->lambertelev*DEG);
		skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
		
		GET_Display(Buffer, G->t_TPI);
		skp->Text(1 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 6 * H / 21, "CDH:", 4);
		sprintf(Buffer, "%f NM", G->spqresults.DH / 1852.0);
		skp->Text(5 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
		if (G->SPQMode == 0)
		{
			AGC_Display(Buffer, G->spqresults.dV_CDH.x / 0.3048);
			skp->Text(5 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->spqresults.dV_CDH.y / 0.3048);
			skp->Text(5 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->spqresults.dV_CDH.z / 0.3048);
			skp->Text(5 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));
		}

		skp->Text(5 * W / 8, 11 * H / 21, "TPI:", 4);
		GET_Display(Buffer, G->spqresults.t_TPI);
		skp->Text(5 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));

		GET_Display(Buffer, G->SPQTIG);
		skp->Text(5 * W / 8, 15 * H / 21, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 16 * H / 21, "DX", 2);
		skp->Text(5 * W / 8, 17 * H / 21, "DY", 2);
		skp->Text(5 * W / 8, 18 * H / 21, "DZ", 2);

		AGC_Display(Buffer, G->SPQDeltaV.x / 0.3048);
		skp->Text(6 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->SPQDeltaV.y / 0.3048);
		skp->Text(6 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->SPQDeltaV.z / 0.3048);
		skp->Text(6 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));

		if (G->target != NULL)
		{
			sprintf(Buffer, G->target->GetName());
			skp->Text(5 * W / 8, 2 * H / 12, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 4)
	{
		skp->Text(4 * W / 8,(int)(0.5 * H / 14), "General Purpose Maneuver", 24);

		skp->Text(1 * W / 22, (marker + 4) * H / 22, "*", 1);

		skp->Text(2 * W / 22, 3 * H / 22, "Code:", 5);
		GMPManeuverCodeName(Buffer, G->GMPManeuverCode);
		skp->Text(5 * W / 22, 3 * H / 22, Buffer, strlen(Buffer));

		skp->Text(2 * W / 22, 4 * H / 22, "TYP", 3);
		GMPManeuverTypeName(Buffer, G->GMPManeuverType);
		skp->Text(4 * W / 22, 4 * H / 22, Buffer, strlen(Buffer));

		skp->Text(2 * W / 22, 5 * H / 22, "PNT", 3);
		GMPManeuverPointName(Buffer, G->GMPManeuverPoint);
		skp->Text(4 * W / 22, 5 * H / 22, Buffer, strlen(Buffer));

		skp->Text(2 * W / 22, 6 * H / 22, "GET", 3);
		GET_Display(Buffer, G->SPSGET, false);
		skp->Text(4 * W / 22, 6 * H / 22, Buffer, strlen(Buffer));

		skp->Text(2 * W / 22, 7 * H / 22, "REF", 3);

		if (G->OrbAdjAltRef == 0)
		{
			skp->Text(4 * W / 22, 7 * H / 22, "Mean rad", 8);
		}
		else
		{
			skp->Text(4 * W / 22, 7 * H / 22, "Pad/LS", 6);
		}

		//Desired Maneuver Height
		if (G->GMPManeuverCode == RTCC_GMP_CRH || G->GMPManeuverCode == RTCC_GMP_HBH || G->GMPManeuverCode == RTCC_GMP_FCH || G->GMPManeuverCode == RTCC_GMP_CPH ||
			G->GMPManeuverCode == RTCC_GMP_CNH || G->GMPManeuverCode == RTCC_GMP_PCH || G->GMPManeuverCode == RTCC_GMP_NSH || G->GMPManeuverCode == RTCC_GMP_HOH)
		{
			skp->Text(2 * W / 22, 8 * H / 22, "ALT", 3);
			sprintf(Buffer, "%.2f NM", G->GMPManeuverHeight / 1852.0);
			skp->Text(4 * W / 22, 8 * H / 22, Buffer, strlen(Buffer));
		}
		//Desired Maneuver Longitude
		else if (G->GMPManeuverCode == RTCC_GMP_PCL || G->GMPManeuverCode == RTCC_GMP_CRL || G->GMPManeuverCode == RTCC_GMP_HOL || G->GMPManeuverCode == RTCC_GMP_NSL ||
			G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_NHL || G->GMPManeuverCode == RTCC_GMP_SAL || G->GMPManeuverCode == RTCC_GMP_PHL ||
			G->GMPManeuverCode == RTCC_GMP_CPL || G->GMPManeuverCode == RTCC_GMP_HBL || G->GMPManeuverCode == RTCC_GMP_CNL || G->GMPManeuverCode == RTCC_GMP_HNL ||
			G->GMPManeuverCode == RTCC_GMP_SAA || G->GMPManeuverCode == RTCC_GMP_HAS)
		{
			skp->Text(2 * W / 22, 8 * H / 22, "LNG", 3);
			sprintf(Buffer, "%.2f°", G->GMPManeuverLongitude*DEG);
			skp->Text(4 * W / 22, 8 * H / 22, Buffer, strlen(Buffer));
		}

		//Height Change
		if (G->GMPManeuverCode == RTCC_GMP_HOL || G->GMPManeuverCode == RTCC_GMP_HOT || G->GMPManeuverCode == RTCC_GMP_HAO || G->GMPManeuverCode == RTCC_GMP_HPO ||
			G->GMPManeuverCode == RTCC_GMP_HNL || G->GMPManeuverCode == RTCC_GMP_HNT || G->GMPManeuverCode == RTCC_GMP_HNA || G->GMPManeuverCode == RTCC_GMP_HNP ||
			G->GMPManeuverCode == RTCC_GMP_PHL || G->GMPManeuverCode == RTCC_GMP_PHT || G->GMPManeuverCode == RTCC_GMP_PHA || G->GMPManeuverCode == RTCC_GMP_PHP)
		{
			skp->Text(2 * W / 22, 9 * H / 22, "DH", 2);
			sprintf(Buffer, "%.2f NM", G->GMPHeightChange / 1852.0);
			skp->Text(4 * W / 22, 9 * H / 22, Buffer, strlen(Buffer));
		}
		//Apoapsis Height
		else if (G->GMPManeuverCode == RTCC_GMP_HBT || G->GMPManeuverCode == RTCC_GMP_HBH || G->GMPManeuverCode == RTCC_GMP_HBO || G->GMPManeuverCode == RTCC_GMP_HBL ||
			G->GMPManeuverCode == RTCC_GMP_NHT || G->GMPManeuverCode == RTCC_GMP_NHL || G->GMPManeuverCode == RTCC_GMP_HAS)
		{
			skp->Text(2 * W / 22, 9 * H / 22, "ApA", 3);
			sprintf(Buffer, "%.2f NM", G->GMPApogeeHeight / 1852.0);
			skp->Text(4 * W / 22, 9 * H / 22, Buffer, strlen(Buffer));
		}
		//Delta V
		else if (G->GMPManeuverCode == RTCC_GMP_FCT || G->GMPManeuverCode == RTCC_GMP_FCA || G->GMPManeuverCode == RTCC_GMP_FCP || G->GMPManeuverCode == RTCC_GMP_FCE ||
			G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_FCH)
		{
			skp->Text(2 * W / 22, 9 * H / 22, "DV", 2);
			sprintf(Buffer, "%.2f ft/s", G->GMPDeltaVInput / 0.3048);
			skp->Text(4 * W / 22, 9 * H / 22, Buffer, strlen(Buffer));
		}
		//Apse line rotation
		else if (G->GMPManeuverCode == RTCC_GMP_SAT || G->GMPManeuverCode == RTCC_GMP_SAO || G->GMPManeuverCode == RTCC_GMP_SAL)
		{
			skp->Text(2 * W / 22, 9 * H / 22, "ROT", 4);
			sprintf(Buffer, "%.2f°", G->GMPApseLineRotAngle*DEG);
			skp->Text(4 * W / 22, 9 * H / 22, Buffer, strlen(Buffer));
		}

		//Wedge Angle
		if (G->GMPManeuverCode == RTCC_GMP_PCE || G->GMPManeuverCode == RTCC_GMP_PCL || G->GMPManeuverCode == RTCC_GMP_PCT || G->GMPManeuverCode == RTCC_GMP_PHL ||
			G->GMPManeuverCode == RTCC_GMP_PHT || G->GMPManeuverCode == RTCC_GMP_PHA || G->GMPManeuverCode == RTCC_GMP_PHP || G->GMPManeuverCode == RTCC_GMP_CPL ||
			G->GMPManeuverCode == RTCC_GMP_CPH || G->GMPManeuverCode == RTCC_GMP_CPT || G->GMPManeuverCode == RTCC_GMP_CPA || G->GMPManeuverCode == RTCC_GMP_CPP ||
			G->GMPManeuverCode == RTCC_GMP_PCH)
		{
			skp->Text(2 * W / 22, 10 * H / 22, "DW", 2);
			sprintf(Buffer, "%.2f°", G->GMPWedgeAngle*DEG);
			skp->Text(4 * W / 22, 10 * H / 22, Buffer, strlen(Buffer));
		}
		//Node Shift
		else if (G->GMPManeuverCode == RTCC_GMP_NST || G->GMPManeuverCode == RTCC_GMP_NSO || G->GMPManeuverCode == RTCC_GMP_NSH || G->GMPManeuverCode == RTCC_GMP_NSL ||
			G->GMPManeuverCode == RTCC_GMP_CNL || G->GMPManeuverCode == RTCC_GMP_CNH || G->GMPManeuverCode == RTCC_GMP_CNT ||
			G->GMPManeuverCode == RTCC_GMP_CNA || G->GMPManeuverCode == RTCC_GMP_CNP)
		{
			skp->Text(2 * W / 22, 10 * H / 22, "DLN", 3);
			sprintf(Buffer, "%.2f°", G->GMPNodeShiftAngle*DEG);
			skp->Text(4 * W / 22, 10 * H / 22, Buffer, strlen(Buffer));
		}
		//Periapsis Height
		else if (G->GMPManeuverCode == RTCC_GMP_HBT || G->GMPManeuverCode == RTCC_GMP_HBH || G->GMPManeuverCode == RTCC_GMP_HBO || G->GMPManeuverCode == RTCC_GMP_HBL ||
			G->GMPManeuverCode == RTCC_GMP_NHT || G->GMPManeuverCode == RTCC_GMP_NHL || G->GMPManeuverCode == RTCC_GMP_HAS)
		{
			skp->Text(2 * W / 22, 10 * H / 22, "PeA", 3);
			sprintf(Buffer, "%.2f NM", G->GMPPerigeeHeight / 1852.0);
			skp->Text(4 * W / 22, 10 * H / 22, Buffer, strlen(Buffer));
		}
		//Pitch
		else if (G->GMPManeuverCode == RTCC_GMP_FCT || G->GMPManeuverCode == RTCC_GMP_FCA || G->GMPManeuverCode == RTCC_GMP_FCP || G->GMPManeuverCode == RTCC_GMP_FCE ||
			G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_FCH)
		{
			skp->Text(2 * W / 22, 10 * H / 22, "P", 1);
			sprintf(Buffer, "%.2f°", G->GMPPitch*DEG);
			skp->Text(4 * W / 22, 10 * H / 22, Buffer, strlen(Buffer));
		}

		//Yaw
		if (G->GMPManeuverCode == RTCC_GMP_FCT || G->GMPManeuverCode == RTCC_GMP_FCA || G->GMPManeuverCode == RTCC_GMP_FCP || G->GMPManeuverCode == RTCC_GMP_FCE ||
			G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_FCH)
		{
			skp->Text(2 * W / 22, 11 * H / 22, "Y", 1);
			sprintf(Buffer, "%.2f°", G->GMPYaw*DEG);
			skp->Text(4 * W / 22, 11 * H / 22, Buffer, strlen(Buffer));
		}
		//Node Shift
		else if (G->GMPManeuverCode == RTCC_GMP_NHT || G->GMPManeuverCode == RTCC_GMP_NHL)
		{
			skp->Text(2 * W / 22, 11 * H / 22, "DLN", 3);
			sprintf(Buffer, "%.2f°", G->GMPNodeShiftAngle*DEG);
			skp->Text(4 * W / 22, 11 * H / 22, Buffer, strlen(Buffer));
		}
		//Rev counter
		else if (G->GMPManeuverCode == RTCC_GMP_HAS)
		{
			skp->Text(2 * W / 22, 11 * H / 22, "N", 1);
			sprintf(Buffer, "%d", G->GMPRevs);
			skp->Text(4 * W / 22, 11 * H / 22, Buffer, strlen(Buffer));
		}

		/*skp->Text(12 * W / 22, 6 * H / 22, "Number:", 7);
		sprintf(Buffer, "%d", G->GMPManeuverCode);
		skp->Text(16 * W / 22, 6 * H / 22, Buffer, strlen(Buffer));*/

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);

		skp->Text(4 * W / 22, 13 * H / 22, "GET A", 5);
		skp->Text(4 * W / 22, 14 * H / 22, "HA", 2);
		skp->Text(4 * W / 22, 15 * H / 22, "LONG A", 6);
		skp->Text(4 * W / 22, 16 * H / 22, "LAT A", 5);
		skp->Text(4 * W / 22, 17 * H / 22, "GET P", 5);
		skp->Text(4 * W / 22, 18 * H / 22, "HP", 2);
		skp->Text(4 * W / 22, 19 * H / 22, "LONG P", 6);
		skp->Text(4 * W / 22, 20 * H / 22, "LAT P", 5);

		GET_Display(Buffer, G->GMPResults.GET_A, false);
		skp->Text(10 * W / 22, 13 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f", G->GMPResults.HA / 1852.0);
		skp->Text(10 * W / 22, 14 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f°", G->GMPResults.long_A*DEG);
		skp->Text(10 * W / 22, 15 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f°", G->GMPResults.lat_A*DEG);
		skp->Text(10 * W / 22, 16 * H / 22, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->GMPResults.GET_P, false);
		skp->Text(10 * W / 22, 17 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f", G->GMPResults.HP / 1852.0);
		skp->Text(10 * W / 22, 18 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f°", G->GMPResults.long_P*DEG);
		skp->Text(10 * W / 22, 19 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f°", G->GMPResults.lat_P*DEG);
		skp->Text(10 * W / 22, 20 * H / 22, Buffer, strlen(Buffer));

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(12 * W / 22, 6 * H / 22, "Orbital Parameters:", 19);
		skp->Text(12 * W / 22, 7 * H / 22, "A", 1);
		skp->Text(12 * W / 22, 8 * H / 22, "E", 1);
		skp->Text(12 * W / 22, 9 * H / 22, "I", 1);
		skp->Text(12 * W / 22, 10 * H / 22, "NODE AN", 7);
		skp->Text(12 * W / 22, 11 * H / 22, "DEL G", 5);
		skp->Text(12 * W / 22, 12 * H / 22, "H MAN", 5);
		skp->Text(12 * W / 22, 13 * H / 22, "LONG MAN", 8);
		skp->Text(12 * W / 22, 14 * H / 22, "LAT MAN", 7);

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);

		sprintf(Buffer, "%.1f", G->GMPResults.A / 1852.0);
		skp->Text(20 * W / 22, 7 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.6f", G->GMPResults.E);
		skp->Text(20 * W / 22, 8 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", G->GMPResults.I*DEG);
		skp->Text(20 * W / 22, 9 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.1f°", G->GMPResults.Node_Ang*DEG);
		skp->Text(20 * W / 22, 10 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f°", G->GMPResults.Del_G*DEG);
		skp->Text(20 * W / 22, 11 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.1f", G->GMPResults.H_Man / 1852.0);
		skp->Text(20 * W / 22, 12 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f°", G->GMPResults.long_Man*DEG);
		skp->Text(20 * W / 22, 13 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f°", G->GMPResults.lat_Man*DEG);
		skp->Text(20 * W / 22, 14 * H / 22, Buffer, strlen(Buffer));

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		GET_Display(Buffer, G->GPM_TIG);
		skp->Text(5 * W / 8, 16 * H / 22, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 17 * H / 22, "DVX", 3);
		skp->Text(5 * W / 8, 18 * H / 22, "DVY", 3);
		skp->Text(5 * W / 8, 19 * H / 22, "DVZ", 3);
		skp->Text(5 * W / 8, 20 * H / 22, "DVT", 3);
		AGC_Display(Buffer, G->OrbAdjDVX.x / 0.3048);
		skp->Text(6 * W / 8, 17 * H / 22, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->OrbAdjDVX.y / 0.3048);
		skp->Text(6 * W / 8, 18 * H / 22, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->OrbAdjDVX.z / 0.3048);
		skp->Text(6 * W / 8, 19 * H / 22, Buffer, strlen(Buffer));
		AGC_Display(Buffer, length(G->OrbAdjDVX) / 0.3048);
		skp->Text(6 * W / 8, 20 * H / 22, Buffer, strlen(Buffer));
	}
	else if (screen == 5)
	{
		skp->Text(6 * W / 8,(int)(0.5 * H / 14), "REFSMMAT", 8);

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
			
			skp->Text(6 * W / 8, 4 * H / 14, "DV Vector", 9);
			AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
			skp->Text(6 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
			skp->Text(6 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
			skp->Text(6 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->REFSMMATopt == 1)//Retrofire
		{
			skp->Text(5 * W / 8, 2 * H / 14, "P30 Retro", 9);

			GET_Display(Buffer, G->P30TIG);
			skp->Text((int)(0.5 * W / 8), 2 * H / 14, Buffer, strlen(Buffer));
			
			skp->Text(6 * W / 8, 4 * H / 14, "DV Vector", 9);
			AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
			skp->Text(6 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
			skp->Text(6 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
			skp->Text(6 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));

		}
		else if (G->REFSMMATopt == 2)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "LVLH", 4);

			GET_Display(Buffer, G->REFSMMATTime);
			skp->Text((int)(0.5 * W / 8), 2 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->REFSMMATopt == 3)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "Lunar Entry", 11);
		}
		else if (G->REFSMMATopt == 4)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "Launch", 6);

			if (GC->mission == 0)
			{
				skp->Text((int)(0.5 * W / 8), 2 * H / 14, "Manual", 6);
			}
			else if (GC->mission >= 7)
			{
				sprintf(Buffer, "Apollo %i", GC->mission);
				skp->Text((int)(0.5 * W / 8), 2 * H / 14, Buffer, strlen(Buffer));
			}
		}
		else if (G->REFSMMATopt == 5 || G->REFSMMATopt == 8)
		{
			GET_Display(Buffer, GC->t_Land);
			skp->Text((int)(0.5 * W / 8), 2 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%f°", GC->LSLat*DEG);
			skp->Text((int)(5.5 * W / 8), 8 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%f°", GC->LSLng*DEG);
			skp->Text((int)(5.5 * W / 8), 10 * H / 14, Buffer, strlen(Buffer));

			if (G->REFSMMATopt == 8)
			{
				skp->Text(5 * W / 8, 2 * H / 14, "LS during TLC", 13);

				skp->Text((int)(5.5 * W / 8), 11 * H / 14, "Azimuth:", 8);
				sprintf(Buffer, "%f°", GC->LOIazi*DEG);
				skp->Text((int)(5.5 * W / 8), 12 * H / 14, Buffer, strlen(Buffer));
			}
			else
			{
				skp->Text(5 * W / 8, 2 * H / 14, "Landing Site", 12);
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

		for (int i = 0; i < 9; i++)
		{
			sprintf(Buffer, "%f", G->REFSMMAT.data[i]);
			skp->Text(7 * W / 16, (4 + i) * H / 14, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 6)
	{
		skp->Text(6 * W / 8, (int)(0.5 * H / 14), "Entry Options", 13);

		skp->Text(1 * W / 8, 2 * H / 14, "Deorbit Maneuver", 16);
		skp->Text(1 * W / 8, 4 * H / 14, "Return to Earth (Earth-centered)", 32);
		skp->Text(1 * W / 8, 6 * H / 14, "Return to Earth (Moon-centered)", 31);
		skp->Text(1 * W / 8, 8 * H / 14, "Splashdown Update", 17);
		skp->Text(1 * W / 8, 10 * H / 14, "RTE Constraints", 15);
	}
	else if (screen == 7)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(4 * W / 8, 1 * H / 14, "AGS NAVIGATION UPDATES (277)", 28);

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		if (G->SVSlot)
		{
			skp->Text((int)(0.5 * W / 8), 10 * H / 14, "CSM", 3);
		}
		else
		{
			skp->Text((int)(0.5 * W / 8), 10 * H / 14, "LM", 2);
		}

		if (G->svtarget != NULL)
		{
			sprintf(Buffer, G->svtarget->GetName());
			skp->Text((int)(0.5 * W / 8), 8 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text((int)(0.5 * W / 8), 8 * H / 14, "No Target!", 10);
		}

		skp->Text((int)(0.5 * W / 8), 5 * H / 14, "REFSMMAT:", 9);
		REFSMMATName(Buffer, G->REFSMMATcur);
		skp->Text((int)(0.5 * W / 8), 6 * H / 14, Buffer, strlen(Buffer));

		int hh, mm;
		double secs;

		SStoHHMMSS(G->AGSKFactor, hh, mm, secs);
		sprintf(Buffer, "%d:%02d:%05.2f GET", hh, mm, secs);
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
	else if (screen == 8)
	{
		skp->Text(6 * W / 8,(int)(0.5 * H / 14), "Config", 6);

		if (GC->mission == 0)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Manual", 8);
		}
		else if (GC->mission >= 7)
		{
			sprintf(Buffer, "Apollo %i", GC->mission);
			skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		}
		sprintf(Buffer, "Launch MJD: %f", GC->GETbase);
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
		sprintf(Buffer, "%.0f min", -G->sxtstardtime / 60.0);
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

				if (G->csmenginetype == 0)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "SPS", 3);
				}
				else if (G->csmenginetype == 1)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "2xRCS +X", 8);
				}
				else if (G->csmenginetype == 2)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "4xRCS +X", 8);
				}
				else if (G->csmenginetype == 3)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "2xRCS -X", 8);
				}
				else if (G->csmenginetype == 4)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "4xRCS -X", 8);
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

				if (G->csmenginetype == 0)
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

				if (G->lemenginetype == 1)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "DPS", 3);
				}
				else if (G->lemenginetype == 0)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "APS", 3);
				}
				else if (G->lemenginetype == 2)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "2xRCS +X", 8);
				}
				else if (G->lemenginetype == 3)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "4xRCS +X", 8);
				}
				else if (G->lemenginetype == 4)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "2xRCS -X", 8);
				}
				else if (G->lemenginetype == 5)
				{
					skp->Text((int)(0.5 * W / 8), 4 * H / 14, "4xRCS -X", 8);
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

				skp->Text(4 * W / 8, 15 * H / 20, "T_L:", 4);
				GET_Display(Buffer, GC->t_Land);
				skp->Text(5 * W / 8, 15 * H / 20, Buffer, strlen(Buffer));

				skp->Text(4 * W / 8, 16 * H / 20, "Lat:", 4);
				sprintf(Buffer, "%.3f°", GC->LSLat*DEG);
				skp->Text(5 * W / 8, 16 * H / 20, Buffer, strlen(Buffer));

				skp->Text(4 * W / 8, 17 * H / 20, "Lng:", 4);
				sprintf(Buffer, "%.3f°", GC->LSLng*DEG);
				skp->Text(5 * W / 8, 17 * H / 20, Buffer, strlen(Buffer));

				skp->Text(4 * W / 8, 18 * H / 20, "Alt:", 4);
				sprintf(Buffer, "%.2f NM", GC->LSAlt / 1852.0);
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

		GET_Display(Buffer, G->mapUpdateGET);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

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
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Lunar Insertion", 15);

		skp->Text(1 * W / 8, 2 * H / 14, "LOI-1", 5);

		if (G->subThreadStatus > 0)
		{
			skp->Text(5 * W / 8, 2 * H / 14, "Calculating...", 14);
		}

		if (G->LOIOption == 0)
		{
			skp->Text(1 * W / 8, 4 * H / 14, "Fixed LPO", 9);
		}
		else
		{
			skp->Text(1 * W / 8, 4 * H / 14, "LOI-1 at Pericynthion", 21);
		}

		GET_Display(Buffer, GC->t_Land);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.2f NM", GC->LOIapo / 1852.0);
		skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.2f NM", GC->LOIperi / 1852.0);
		skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		if (G->LOIOption == 0)
		{
			sprintf(Buffer, "%.3f°", GC->LOIazi*DEG);
			skp->Text(1 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
		}

		skp->Text(5 * W / 8, 6 * H / 21, "Landing site:", 13);
		sprintf(Buffer, "%.3f°", GC->LSLat*DEG);
		skp->Text(5 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", GC->LSLng*DEG);
		skp->Text(5 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f NM", GC->LSAlt / 1852.0);
		skp->Text(5 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));

		if (GC->LOIEllipseRotation == 0)
		{
			skp->Text(5 * W / 8, 8 * H / 14, "Min DV", 6);
		}
		else if (GC->LOIEllipseRotation == 1)
		{
			skp->Text(5 * W / 8, 8 * H / 14, "Solution 1", 10);
		}
		else if (GC->LOIEllipseRotation == 2)
		{
			skp->Text(5 * W / 8, 8 * H / 14, "Solution 2", 10);
		}

		GET_Display(Buffer, G->LOI_TIG);
		skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%+07.1f DVX", G->LOI_dV_LVLH.x / 0.3048);
		skp->Text(5 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f DVY", G->LOI_dV_LVLH.y / 0.3048);
		skp->Text(5 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f DVZ", G->LOI_dV_LVLH.z / 0.3048);
		skp->Text(5 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));
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
			sprintf(Buffer, "%.1f NM North", G->landmarkpad.CRDist[0]);
		}
		else
		{
			sprintf(Buffer, "%.1f NM South", abs(G->landmarkpad.CRDist[0]));
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

		skp->Text(1 * W / 16, 2 * H / 14, "Rendezvous", 10);
		skp->Text(1 * W / 16, 4 * H / 14, "General Purpose Maneuver", 24);
		skp->Text(1 * W / 16, 6 * H / 14, "Translunar", 10);
		skp->Text(1 * W / 16, 8 * H / 14, "Lunar Insertion", 15);
		skp->Text(1 * W / 16, 10 * H / 14, "Entry", 5);

		skp->Text(5 * W / 8, 2 * H / 14, "Descent Planning", 16);
		skp->Text(5 * W / 8, 6 * H / 14, "Lunar Liftoff", 13);
		skp->Text(5 * W / 8, 8 * H / 14, "Lunar Ascent", 12);
		skp->Text(5 * W / 8, 10 * H / 14, "Descent Abort", 13);
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
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(4 * W / 8, 1 * H / 14, "Computation for Lunar Descent Planning (K16)", 44);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(1 * W / 16, 2 * H / 14, "Init", 4);

		if (GC->MissionPlanningActive)
		{
			if (GC->rtcc->med_k16.Vehicle == 1)
			{
				skp->Text(1 * W / 16, 4 * H / 14, "LEM", 3);
			}
			else
			{
				skp->Text(1 * W / 16, 4 * H / 14, "CSM", 3);
			}

			GET_Display(Buffer, GC->rtcc->med_k16.VectorTime, false);
			skp->Text(1 * W / 16, 6 * H / 14, Buffer, strlen(Buffer));
		}

		if (GC->rtcc->med_k16.Mode == 1)
		{
			skp->Text(1 * W / 16, 8 * H / 14, "CSM Phase Change (N/A)", 22);

			if (GC->rtcc->med_k16.Sequence == 1)
			{
				skp->Text(1 * W / 16, 10 * H / 14, "1: PC, DOI (N/A)", 16);
			}
			else if (GC->rtcc->med_k16.Sequence == 2)
			{
				skp->Text(1 * W / 16, 10 * H / 14, "2: PCC, DOI (N/A)", 17);
			}
			else if (GC->rtcc->med_k16.Sequence == 3)
			{
				skp->Text(1 * W / 16, 10 * H / 14, "3: ASP, CIA, DOI (N/A)", 22);
			}
			else if (GC->rtcc->med_k16.Sequence == 4)
			{
				skp->Text(1 * W / 16, 10 * H / 14, "4: PCCH, DOI (N/A)", 18);
			}
			else if (GC->rtcc->med_k16.Sequence == 5)
			{
				skp->Text(1 * W / 16, 10 * H / 14, "5: PCCT, DOI (N/A)", 18);
			}
		}
		else if (GC->rtcc->med_k16.Mode == 2)
		{
			skp->Text(1 * W / 16, 8 * H / 14, "Single CSM Maneuver", 19);
		
			if (GC->rtcc->med_k16.Sequence == 2)
			{
				skp->Text(1 * W / 16, 10 * H / 14, "2: ASH, DOI (N/A)", 17);
			}
			else if (GC->rtcc->med_k16.Sequence == 3)
			{
				skp->Text(1 * W / 16, 10 * H / 14, "3: CIR, DOI", 11);
			}
			else
			{
				sprintf(Buffer, "%d: Not Used", GC->rtcc->med_k16.Sequence);
				skp->Text(1 * W / 16, 10 * H / 14, Buffer, strlen(Buffer));
			}
		}
		else if (GC->rtcc->med_k16.Mode == 3)
		{
			skp->Text(1 * W / 16, 8 * H / 14, "Double CSM Maneuver (N/A)", 25);

			if (GC->rtcc->med_k16.Sequence == 1)
			{
				skp->Text(1 * W / 16, 10 * H / 14, "2: ASH at time, CIA, DOI (N/A)", 30);
			}
			else if (GC->rtcc->med_k16.Sequence == 3)
			{
				skp->Text(1 * W / 16, 10 * H / 14, "3: ASH at apsis, CIA, DOI (N/A)", 31);
			}
			else
			{
				sprintf(Buffer, "%d: Not Used", GC->rtcc->med_k16.Sequence);
				skp->Text(1 * W / 16, 10 * H / 14, Buffer, strlen(Buffer));
			}
		}
		else if (GC->rtcc->med_k16.Mode == 4)
		{
			skp->Text(1 * W / 16, 8 * H / 14, "LM Maneuver Sequence", 20);
			skp->Text(1 * W / 16, 10 * H / 14, "DOI", 3);
		}
		else if (GC->rtcc->med_k16.Mode == 5)
		{
			skp->Text(1 * W / 16, 8 * H / 14, "Double Hohmann, PC (N/A)", 34);
		
			if (GC->rtcc->med_k16.Sequence == 1)
			{
				skp->Text(1 * W / 16, 10 * H / 14, "1: PC, HO1, HO2, DOI (N/A)", 26);
			}
			else if (GC->rtcc->med_k16.Sequence == 2)
			{
				skp->Text(1 * W / 16, 10 * H / 14, "2: HO1, PC, HO2, DOI (N/A)", 26);
			}
			else if (GC->rtcc->med_k16.Sequence == 3)
			{
				skp->Text(1 * W / 16, 10 * H / 14, "3: HO1, HO2, PC, DOI (N/A)", 26);
			}
			else
			{
				sprintf(Buffer, "%d: Not Used", GC->rtcc->med_k16.Sequence);
				skp->Text(1 * W / 16, 10 * H / 14, Buffer, strlen(Buffer));
			}
		}
		else if (GC->rtcc->med_k16.Mode == 6)
		{
			skp->Text(1 * W / 16, 8 * H / 14, "LM Powered Descent (N/A)", 24);
		}
		else if (GC->rtcc->med_k16.Mode == 7)
		{
			skp->Text(1 * W / 16, 8 * H / 14, "CSM Prelaunch Plane Change", 26);
			skp->Text(1 * W / 16, 10 * H / 14, "PPC", 3);
		}

		if (GC->rtcc->med_k16.Mode == 1)
		{
			sprintf(Buffer, "%.3f NM", GC->rtcc->med_k16.DesiredHeight / 1852.0);
			skp->Text(1 * W / 16, 12 * H / 14, Buffer, strlen(Buffer));
		}

		GET_Display(Buffer, GC->rtcc->med_k16.GETTH1, false);
		skp->Text(5 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->rtcc->med_k16.GETTH2, false);
		skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->rtcc->med_k16.GETTH3, false);
		skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->rtcc->med_k16.GETTH4, false);
		skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
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
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(4 * W / 8, 1 * H / 14, "Initialization for Lunar Descent Planning (K17)", 47);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		if (GC->rtcc->med_k17.Azimuth != 0.0)
		{
			sprintf(Buffer, "%.3f°", GC->rtcc->med_k17.Azimuth*DEG);
			skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Optimum Azimuth", 15);
		}

		sprintf(Buffer, "%.0f ft", GC->rtcc->med_k17.DescIgnHeight / 0.3048);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

		if (GC->rtcc->med_k17.PoweredDescSimFlag)
		{
			skp->Text(1 * W / 8, 6 * H / 14, "Simulate powered descent (N/A)", 30);
		}
		else
		{
			skp->Text(1 * W / 8, 6 * H / 14, "Do not simulate powered descent", 31);
		}

		GET_Display(Buffer, GC->rtcc->med_k17.PoweredDescTime);
		skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%d", GC->rtcc->med_k17.DwellOrbits);
		skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.2f min", GC->rtcc->med_k17.DescentFlightTime / 60.0);
		skp->Text(5 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.2f°", GC->rtcc->med_k17.DescentFlightArc*DEG);
		skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
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
		skp->Text(5 * W / 8, 4 * H / 14, "LM Ascent PAD", 13);
		skp->Text(5 * W / 8, 6 * H / 14, "AGS SV PAD", 10);
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

		skp->Text(1 * W / 8, 2 * H / 14, "Landing Site", 12);
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
			skp->Text(1 * W / 8, 2 * H / 14, "TLMCC Option 4: Non-FR BAP, Fixed LPO, LS", 41);
		}
		else if (G->TLCCmaneuver == 6)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TLMCC Option 5: Non-FR BAP, Free LPO, LS", 40);
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
			GET_Display(Buffer, GC->TLCCNodeGET);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.5f°", GC->TLCCNodeLat*DEG);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.5f°", GC->TLCCNodeLng*DEG);
			skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", GC->TLCCNodeAlt / 1852.0);
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->TLCCmaneuver == 1 || G->TLCCmaneuver == 3 || G->TLCCmaneuver == 4 || G->TLCCmaneuver == 7) //free return target display
		{
			GET_Display(Buffer, GC->TLCCPeriGET);
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

			sprintf(Buffer, "%.5f°", GC->TLCCFreeReturnEMPLat*DEG);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			if (G->TLCCmaneuver == 3 || G->TLCCmaneuver == 4)
			{
				sprintf(Buffer, "%.2f NM", GC->TLCCLAHPeriAlt / 1852.0);
			}
			else
			{
				sprintf(Buffer, "%.2f NM", GC->TLCCFlybyPeriAlt / 1852.0);
			}
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->TLCCmaneuver == 5 || G->TLCCmaneuver == 6)
		{
			skp->Text(1 * W / 8, 8 * H / 14, "Rev 2 Meridian Crossing:", 24);
			GET_Display(Buffer, G->TLCCRev2MeridianGET);
			skp->Text(1 * W / 8, 9 * H / 14, Buffer, strlen(Buffer));

			GET_Display(Buffer, GC->TLCCPeriGET);
			skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			skp->Text(1 * W / 8, 15 * H / 21, "LOI DV:", 7);
			sprintf(Buffer, "%+07.1f %+07.1f %+07.1f", G->LOI_dV_LVLH.x / 0.3048, G->LOI_dV_LVLH.y / 0.3048, G->LOI_dV_LVLH.z / 0.3048);
			skp->Text(1 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));

			if (GC->DOI_option == 1)
			{
				skp->Text(1 * W / 8, 17 * H / 21, "DOI DV:", 7);
				sprintf(Buffer, "%+07.1f %+07.1f %+07.1f", G->DOI_dV_LVLH.x / 0.3048, G->DOI_dV_LVLH.y / 0.3048, G->DOI_dV_LVLH.z / 0.3048);
				skp->Text(1 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
				skp->Text(1 * W / 8, 19 * H / 21, "Orbit after DOI:", 16);
				sprintf(Buffer, "%.1f x %.1f", G->TLCCPostDOIApoAlt / 1852.0, G->TLCCPostDOIPeriAlt / 1852.0);
				skp->Text(1 * W / 8, 20 * H / 21, Buffer, strlen(Buffer));
			}

			sprintf(Buffer, "%.5f°", GC->TLCCNonFreeReturnEMPLat*DEG);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", GC->TLCCLAHPeriAlt / 1852.0);
			skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->TLCCmaneuver == 8)
		{
			GET_Display(Buffer, GC->TLCCPeriGET);
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

			sprintf(Buffer, "%.5f°", GC->TLCCFreeReturnEMPLat*DEG);
			skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.2f NM", GC->TLCCFlybyPeriAlt / 1852.0);
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

		GET_Display(Buffer, G->t_Liftoff_guess);
		skp->Text((int)(0.5 * W / 8), 2 * H / 14, Buffer, strlen(Buffer));

		if (G->LunarLiftoffTimeOption == 0)
		{
			if (G->LunarLiftoffInsVelInput)
			{
				skp->Text((int)(0.5 * W / 8), 4 * H / 14, "Input Ins. Velocity", 19);
			}
			else
			{
				skp->Text((int)(0.5 * W / 8), 4 * H / 14, "Calc. Ins. Velocity", 19);
			}
		}

		skp->Text((int)(0.5 * W / 8), 8 * H / 21, "Rendezvous Schedule:", 20);

		skp->Text((int)(0.5 * W / 8), 9 * H / 21, "Launch:", 7);
		GET_Display(Buffer, G->LunarLiftoffRes.t_L);
		skp->Text(2 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));

		skp->Text((int)(0.5 * W / 8), 10 * H / 21, "Insertion:", 10);
		GET_Display(Buffer, G->LunarLiftoffRes.t_Ins);
		skp->Text(2 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));

		if (G->LunarLiftoffTimeOption == 0)
		{
			skp->Text((int)(0.5 * W / 8), 11 * H / 21, "T CSI:", 6);
			GET_Display(Buffer, G->LunarLiftoffRes.t_CSI);
			skp->Text(2 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));

			skp->Text((int)(0.5 * W / 8), 12 * H / 21, "DV CSI:", 7);
			sprintf(Buffer, "%.1f ft/s", G->LunarLiftoffRes.DV_CSI / 0.3048);
			skp->Text(2 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));

			skp->Text((int)(0.5 * W / 8), 13 * H / 21, "T CDH:", 6);
			GET_Display(Buffer, G->LunarLiftoffRes.t_CDH);
			skp->Text(2 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));

			skp->Text((int)(0.5 * W / 8), 14 * H / 21, "DV CDH:", 7);
			sprintf(Buffer, "%.1f ft/s", G->LunarLiftoffRes.DV_CDH / 0.3048);
			skp->Text(2 * W / 8, 14 * H / 21, Buffer, strlen(Buffer));
		}

		if (G->LunarLiftoffTimeOption == 0 || G->LunarLiftoffTimeOption == 1)
		{
			skp->Text((int)(0.5 * W / 8), 15 * H / 21, "T TPI:", 6);
			GET_Display(Buffer, G->LunarLiftoffRes.t_TPI);
			skp->Text(2 * W / 8, 15 * H / 21, Buffer, strlen(Buffer));

			skp->Text((int)(0.5 * W / 8), 16 * H / 21, "DV TPI:", 7);
			sprintf(Buffer, "%.1f ft/s", G->LunarLiftoffRes.DV_TPI / 0.3048);
			skp->Text(2 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));
		}

		skp->Text((int)(0.5 * W / 8), 17 * H / 21, "T TPF:", 6);
		GET_Display(Buffer, G->LunarLiftoffRes.t_TPF);
		skp->Text(2 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));

		skp->Text((int)(0.5 * W / 8), 18 * H / 21, "DV TPF:", 7);
		sprintf(Buffer, "%.1f ft/s", G->LunarLiftoffRes.DV_TPF / 0.3048);
		skp->Text(2 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));

		skp->Text((int)(0.5 * W / 8), 19 * H / 21, "DVT:", 4);
		sprintf(Buffer, "%.1f ft/s", G->LunarLiftoffRes.DV_T / 0.3048);
		skp->Text(2 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));

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
			sprintf(Buffer, "%.1f min", GC->DT_Ins_TPI / 60.0);
			skp->Text(5 * W / 8, 7 * H / 14, Buffer, strlen(Buffer));
		}

		skp->Text(5 * W / 8, 9 * H / 14, "Horizontal Velocity:", 20);
		sprintf(Buffer, "%+.1f ft/s", G->LunarLiftoffRes.v_LH / 0.3048);
		skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 11 * H / 14, "Vertical Velocity:", 18);
		sprintf(Buffer, "%+.1f ft/s", G->LunarLiftoffRes.v_LV / 0.3048);
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

		if (G->csmenginetype == 0)
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
		skp->Text(4 * W / 8, (int)(0.5 * H / 14), "Return to Earth (Moon)", 22);

		if (G->RTECalcMode == 1)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "ATP Search Option", 17);
		}
		else if (G->RTECalcMode == 2)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "ATP Discrete Option", 19);
		}
		else if (G->RTECalcMode == 3)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "UA Search Option", 16);
		}
		else if (G->RTECalcMode == 4)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "UA Discrete Option", 18);
		}

		GET_Display(Buffer, G->EntryTIG);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

		if (G->RTECalcMode == 0 || G->RTECalcMode == 1 || G->RTECalcMode == 2)
		{
			if (G->entrylongmanual)
			{
				skp->Text(1 * W / 8, 6 * H / 14, "Manual", 6);
				sprintf(Buffer, "%f °", G->EntryLng*DEG);
				skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
			}
			else
			{
				skp->Text(1 * W / 8, 6 * H / 14, "Landing Zone", 12);
				if (G->landingzone == 0)
				{
					skp->Text(1 * W / 8, 8 * H / 14, "Mid Pacific", 11);
				}
				else if (G->landingzone == 1)
				{
					skp->Text(1 * W / 8, 8 * H / 14, "East Pacific", 12);
				}
				else if (G->landingzone == 2)
				{
					skp->Text(1 * W / 8, 8 * H / 14, "Atlantic Ocean", 14);
				}
				else if (G->landingzone == 3)
				{
					skp->Text(1 * W / 8, 8 * H / 14, "Indian Ocean", 12);
				}
				else if (G->landingzone == 4)
				{
					skp->Text(1 * W / 8, 8 * H / 14, "West Pacific", 12);
				}
			}
		}

		GET_Display(Buffer, G->RTEReentryTime);
		skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		if (G->RTECalcMode == 0 || G->RTECalcMode == 1 || G->RTECalcMode == 2)
		{
			if (G->EntryDesiredInclination < 0)
			{
				sprintf(Buffer, "%.3f° A", abs(G->EntryDesiredInclination*DEG));
			}
			else if (G->EntryDesiredInclination > 0)
			{
				sprintf(Buffer, "%.3f° D", G->EntryDesiredInclination*DEG);
			}
			else
			{
				sprintf(Buffer, "Optimize DV");
			}
			skp->Text(1 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
		}

		if (G->subThreadStatus > 0)
		{
			skp->Text(5 * W / 8, 3 * H / 14, "Calculating...", 14);
		}
		else if (!G->TLCCSolGood)
		{
			skp->Text(5 * W / 8, 3 * H / 14, "Calculation Failed!", 19);
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

		skp->Text(5 * W / 8, 11 * H / 21, "Return Inclination:", 19);
		if (G->RTEReturnInclination < 0)
		{
			sprintf(Buffer, "%.3f° A", abs(G->RTEReturnInclination*DEG));
		}
		else
		{
			sprintf(Buffer, "%.3f° D", G->RTEReturnInclination*DEG);
		}
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
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "RTE Constraints", 15);

		sprintf(Buffer, "Max Return Inclination: %.3f°", GC->RTEMaxReturnInclination*DEG);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "Range Override: %.1f NM", GC->RTERangeOverrideNM);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "Max Reentry Speed: %.0f ft/s", G->RTEMaxReentrySpeed / 0.3048);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
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
		skp->Text(1 * W / 8, 8 * H / 14, "Skylab Rendezvous", 17);
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
		else if (G->DKI_Profile == 2)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Rescue-2 Sequence", 17);
		}
		else if(G->DKI_Profile == 3)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "TPI Time Only", 13);
		}
		else
		{
			skp->Text(1 * W / 8, 2 * H / 14, "High Dwell Sequence", 19);
		}

		if (G->DKI_Profile != 3)
		{
			GET_Display(Buffer, G->DKI_TIG);
			skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		}
		GET_Display(Buffer, G->t_TPIguess);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		if (G->DKI_Profile != 3)
		{
			sprintf(Buffer, "%.1f NM", G->DH / 1852.0);
			skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.2f°", G->lambertelev*DEG);
			skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));
		}

		if (G->target != NULL)
		{
			sprintf(Buffer, G->target->GetName());
			skp->Text(5 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		}

		if (G->DKI_Profile != 3)
		{
			skp->Text(5 * W / 8, 5 * H / 21, "Phasing:", 8);
			GET_Display(Buffer, G->DKI_TIG);
			skp->Text(5 * W / 8, 6 * H / 21, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f", G->DKI_DV.x / 0.3048);
			skp->Text(5 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->DKI_DV.y / 0.3048);
			skp->Text(5 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->DKI_DV.z / 0.3048);
			skp->Text(5 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));

			if (G->DKI_Profile == 1 || G->DKI_Profile == 4)
			{
				skp->Text(4 * W / 8, 13 * H / 21, "Boost:", 6);
				GET_Display(Buffer, G->dkiresult.t_Boost);
				skp->Text(5 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f ft/s", G->dkiresult.dv_Boost / 0.3048);
				skp->Text(5 * W / 8, 14 * H / 21, Buffer, strlen(Buffer));

				if (G->DKI_Profile == 1)
				{
					skp->Text(4 * W / 8, 15 * H / 21, "HAM:", 4);
					GET_Display(Buffer, G->dkiresult.t_HAM);
					skp->Text(5 * W / 8, 15 * H / 21, Buffer, strlen(Buffer));
				}
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
		}

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

		sprintf(Buffer, "%d", G->DKI_N_HC);
		skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%d", G->DKI_N_PB);
		skp->Text(1 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));

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
		if (G->AGCEphemOption == 0)
		{
			skp->Text(4 * W / 8, (int)(0.5 * H / 14), "AGC Ephemeris Generator", 23);

			skp->Text(1 * W / 8, 2 * H / 14, "Epoch of BRCS:", 14);
			skp->Text(1 * W / 8, 4 * H / 14, "TEphemZero:", 11);

			sprintf(Buffer, "%f", G->AGCEphemBRCSEpoch);
			skp->Text(4 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%f", G->AGCEphemTEphemZero);
			skp->Text(4 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(4 * W / 8, (int)(0.5 * H / 14), "AGC Correction Vectors", 23);

			skp->Text(1 * W / 8, 6 * H / 14, "TEPHEM:", 7);
			sprintf(Buffer, "%f", G->AGCEphemTEPHEM);
			skp->Text(4 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

			skp->Text(1 * W / 8, 10 * H / 14, "TLAND:", 6);
			GET_Display(Buffer, G->AGCEphemTLAND);
			skp->Text(4 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

			skp->Text(1 * W / 8, 12 * H / 14, "Mission:", 8);
			sprintf(Buffer, "%d", G->AGCEphemMission);
			skp->Text(4 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));

			if (G->AGCEphemIsCMC)
			{
				skp->Text(7 * W / 8, 6 * H / 14, "CMC", 3);
			}
			else
			{
				skp->Text(7 * W / 8, 6 * H / 14, "LGC", 3);
			}
		}

		
		skp->Text(1 * W / 8, 8 * H / 14, "TIMEM0:", 7);
		sprintf(Buffer, "%f", G->AGCEphemTIMEM0);
		skp->Text(4 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 38)
	{
		skp->Text(4 * W / 8, (int)(0.5 * H / 14), "Lunar Ascent Processor", 22);

		GET_Display(Buffer, G->LunarLiftoffRes.t_L);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%+07.1f ft/s", G->LunarLiftoffRes.v_LH / 0.3048);
		skp->Text(1 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%+07.1f ft/s", G->LunarLiftoffRes.v_LV / 0.3048);
		skp->Text(1 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.3f°", G->LAP_Theta*DEG);
		skp->Text(1 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.1f s", G->LAP_DT);
		skp->Text(1 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		if (!GC->MissionPlanningActive)
		{
			if (G->target != NULL)
			{
				sprintf(Buffer, G->target->GetName());
				skp->Text(5 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
			}
			else
			{
				skp->Text(5 * W / 8, 2 * H / 14, "No Target!", 10);
			}
		}

		sprintf(Buffer, "%f", G->LAP_SV_Insertion.R.x);
		skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", G->LAP_SV_Insertion.R.y);
		skp->Text(5 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", G->LAP_SV_Insertion.R.z);
		skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%f", G->LAP_SV_Insertion.V.x);
		skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", G->LAP_SV_Insertion.V.y);
		skp->Text(5 * W / 8, 9 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", G->LAP_SV_Insertion.V.z);
		skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%f", G->LAP_SV_Insertion.MJD);
		skp->Text(5 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 39)
	{
		skp->Text(4 * W / 8, (int)(0.5 * H / 14), "LM Ascent PAD", 13);

		if (G->target != NULL)
		{
			sprintf(Buffer, G->target->GetName());
			skp->Text((int)(5.5 * W / 8), 4 * H / 14, Buffer, strlen(Buffer));
		}

		int hh, mm;
		double secs;

		SStoHHMMSS(G->LunarLiftoffRes.t_L, hh, mm, secs);
		sprintf(Buffer, "%+06d HRS", hh);
		skp->Text(2 * W / 8, 6 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06d MIN TIG", mm);
		skp->Text(2 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06.0f SEC", secs * 100.0);
		skp->Text(2 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));

		sprintf(Buffer, "%+07.1f V (HOR)", G->LunarLiftoffRes.v_LH / 0.3048);
		skp->Text(2 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f V (VERT) N76", G->LunarLiftoffRes.v_LV / 0.3048);
		skp->Text(2 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f CROSSRANGE", G->lmascentpad.CR);
		skp->Text(2 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06d DEDA 047", G->lmascentpad.DEDA047);
		skp->Text(2 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06d DEDA 053", G->lmascentpad.DEDA053);
		skp->Text(2 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06.0f DEDA 225/226", G->lmascentpad.DEDA225_226);
		skp->Text(2 * W / 8, 14 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06.0f DEDA 231", G->lmascentpad.DEDA231);
		skp->Text(2 * W / 8, 15 * H / 21, Buffer, strlen(Buffer));

	}
	else if (screen == 40)
	{
		skp->Text(4 * W / 8, (int)(0.5 * H / 14), "Descent Abort", 13);

		if (G->subThreadStatus > 0)
		{
			skp->Text(5 * W / 8, 3 * H / 14, "Calculating...", 14);
		}
		else if (!G->PADSolGood)
		{
			skp->Text(5 * W / 8, 3 * H / 14, "Calculation failed!", 19);
		}

		if (G->PDAPEngine == 0)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "DPS/APS", 7);
		}
		else
		{
			skp->Text(1 * W / 8, 2 * H / 14, "APS", 7);
		}

		skp->Text(4 * W / 8, 3 * H / 21, "TPI:", 4);
		GET_Display(Buffer, G->t_TPI);
		skp->Text(5 * W / 8, 3 * H / 21, Buffer, strlen(Buffer));

		skp->Text(1 * W / 8, 5 * H / 21, "PGNS Coefficients:", 18);
		if (GC->mission <= 11)
		{
			sprintf(Buffer, "%e", G->PDAPABTCOF[0] / 0.3048);
			skp->Text(1 * W / 8, 6 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%e", G->PDAPABTCOF[1] / 0.3048);
			skp->Text(1 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%e", G->PDAPABTCOF[2] / 0.3048);
			skp->Text(1 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%f", G->PDAPABTCOF[3] / 0.3048);
			skp->Text(1 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%e", G->PDAPABTCOF[4] / 0.3048);
			skp->Text(1 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%e", G->PDAPABTCOF[5] / 0.3048);
			skp->Text(1 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%e", G->PDAPABTCOF[6] / 0.3048);
			skp->Text(1 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%f", G->PDAPABTCOF[7] / 0.3048);
			skp->Text(1 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(1 * W / 8, 6 * H / 21, "J1", 2);
			sprintf(Buffer, "%.4f NM", G->PDAP_J1 / 1852.0);
			skp->Text(2 * W / 8, 6 * H / 21, Buffer, strlen(Buffer));
			skp->Text(1 * W / 8, 7 * H / 21, "K1", 2);
			sprintf(Buffer, "%.4f NM/DEG", G->PDAP_K1 / 1852.0 / DEG);
			skp->Text(2 * W / 8, 7 * H / 21, Buffer, strlen(Buffer));
			skp->Text(1 * W / 8, 8 * H / 21, "J2", 2);
			sprintf(Buffer, "%.4f NM", G->PDAP_J2 / 1852.0);
			skp->Text(2 * W / 8, 8 * H / 21, Buffer, strlen(Buffer));
			skp->Text(1 * W / 8, 9 * H / 21, "K2", 2);
			sprintf(Buffer, "%.4f NM/DEG", G->PDAP_K2 / 1852.0 / DEG);
			skp->Text(2 * W / 8, 9 * H / 21, Buffer, strlen(Buffer));
			skp->Text(1 * W / 8, 10 * H / 21, "THET", 4);
			sprintf(Buffer, "%.4f°", G->PDAP_Theta_LIM*DEG);
			skp->Text(2 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));
			skp->Text(1 * W / 8, 11 * H / 21, "RMIN", 4);
			sprintf(Buffer, "%.4f NM", G->PDAP_R_amin / 1852.0);
			skp->Text(2 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));
		}

		skp->Text(1 * W / 8, 15 * H / 21, "AGS Coefficients:", 18);
		skp->Text(1 * W / 8, 16 * H / 21, "224", 3);
		sprintf(Buffer, "%+06.0f", G->DEDA224 / 0.3048 / 100.0);
		skp->Text(2 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));
		skp->Text(1 * W / 8, 17 * H / 21, "225", 3);
		sprintf(Buffer, "%+06.0f", G->DEDA225 / 0.3048 / 100.0);
		skp->Text(2 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
		skp->Text(1 * W / 8, 18 * H / 21, "226", 3);
		sprintf(Buffer, "%+06.0f", G->DEDA226 / 0.3048 / 100.0);
		skp->Text(2 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
		skp->Text(1 * W / 8, 19 * H / 21, "227", 3);
		sprintf(Buffer, "%+06d", G->DEDA227);
		skp->Text(2 * W / 8, 19 * H / 21, Buffer, strlen(Buffer));

		if (G->target != NULL)
		{
			sprintf(Buffer, G->target->GetName());
			skp->Text((int)(5.5 * W / 8), 4 * H / 14, Buffer, strlen(Buffer));
		}

		if (G->REFSMMATcur != 5 && G->REFSMMATcur != 8)
		{
			skp->Text(5 * W / 8, 6 * H / 14, "No LS REFSMMAT!", 15);
		}

		skp->Text(5 * W / 8, 15 * H / 21, "Landing Site:", 13);
		sprintf(Buffer, "%.3f°", GC->LSLat*DEG);
		skp->Text(5 * W / 8, 16 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", GC->LSLng*DEG);
		skp->Text(5 * W / 8, 17 * H / 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f NM", GC->LSAlt / 1852.0);
		skp->Text(5 * W / 8, 18 * H / 21, Buffer, strlen(Buffer));
		skp->Text(5 * W / 8, 19 * H / 21, "TLAND:", 6);
		GET_Display(Buffer, GC->t_Land);
		skp->Text(5 * W / 8, 20 * H / 21, Buffer, strlen(Buffer));
	}
	else if (screen == 41)
	{
		G->CycleFIDOOrbitDigitals();

		skp->Text(4 * W / 8, (int)(0.5 * H / 14), "FIDO ORBIT DIGITALS", 19);

		skp->SetFont(font2);

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);

		skp->Text(5 * W / 32, 3 * H / 28, "GET", 3);
		skp->Text(5 * W / 32, 4 * H / 28, "VEHICLE", 7);
		skp->Text(5 * W / 32, 5 * H / 28, "REV", 3);
		skp->Text(5 * W / 32, 6 * H / 28, "REF", 3);
		skp->Text(5 * W / 32, 7 * H / 28, "GMT ID", 6);
		skp->Text(5 * W / 32, 8 * H / 28, "GET ID", 6);
		skp->Text(5 * W / 32, 10 * H / 28, "H", 1);
		skp->Text(5 * W / 32, 11 * H / 28, "V", 1);
		skp->Text(5 * W / 32, 12 * H / 28, "GAM", 3);
		skp->Text(5 * W / 32, 14 * H / 28, "A", 1);
		skp->Text(5 * W / 32, 15 * H / 28, "E", 1);
		skp->Text(5 * W / 32, 16 * H / 28, "I", 1);
		skp->Text(5 * W / 32, 18 * H / 28, "HA", 2);
		skp->Text(5 * W / 32, 19 * H / 28, "PA", 2);
		skp->Text(5 * W / 32, 20 * H / 28, "LA", 2);
		skp->Text(5 * W / 32, 21 * H / 28, "GETA", 4);
		skp->Text(5 * W / 32, 23 * H / 28, "HP", 2);
		skp->Text(5 * W / 32, 24 * H / 28, "PP", 2);
		skp->Text(5 * W / 32, 25 * H / 28, "LP", 2);
		skp->Text(5 * W / 32, 26 * H / 28, "GETP", 4);

		skp->Text(15 * W / 32, 3 * H / 28, "LPP", 3);
		skp->Text(15 * W / 32, 4 * H / 28, "PPP", 3);
		skp->Text(15 * W / 32, 5 * H / 28, "GETCC", 5);
		skp->Text(15 * W / 32, 6 * H / 28, "TAPP", 4);
		skp->Text(15 * W / 32, 7 * H / 28, "LNPP", 3);

		skp->Text(25 * W / 32, 3 * H / 28, "REVL", 4);
		skp->Text(25 * W / 32, 4 * H / 28, "GETL", 4);
		skp->Text(25 * W / 32, 5 * H / 28, "L", 1);
		skp->Text(25 * W / 32, 6 * H / 28, "TO", 2);
		skp->Text(25 * W / 32, 7 * H / 28, "K", 1);
		skp->Text(25 * W / 32, 8 * H / 28, "ORBWT", 5);

		skp->Text(25 * W / 32, 10 * H / 28, "REQUESTED", 9);
		skp->Text(23 * W / 32, 11 * H / 28, "REF", 3);
		skp->Text(23 * W / 32, 12 * H / 28, "GETBV", 5);
		skp->Text(23 * W / 32, 13 * H / 28, "HA", 2);
		skp->Text(23 * W / 32, 14 * H / 28, "PA", 2);
		skp->Text(23 * W / 32, 15 * H / 28, "LA", 2);
		skp->Text(23 * W / 32, 16 * H / 28, "GETA", 4);
		skp->Text(23 * W / 32, 18 * H / 28, "HP", 2);
		skp->Text(23 * W / 32, 19 * H / 28, "PP", 2);
		skp->Text(23 * W / 32, 20 * H / 28, "LP", 2);
		skp->Text(23 * W / 32, 21 * H / 28, "GETP", 4);

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		GET_Display(Buffer, G->fidoorbit.GET, false);
		skp->Text(3 * W / 16, 3 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, G->vessel->GetName());
		skp->Text(3 * W / 16, 4 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%03d", G->fidoorbit.REV);
		skp->Text(3 * W / 16, 5 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, G->fidoorbit.REF);
		skp->Text(3 * W / 16, 6 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->fidoorbit.GMTID, false);
		skp->Text(3 * W / 16, 7 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->fidoorbit.GETID, false);
		skp->Text(3 * W / 16, 8 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%08.1f", G->fidoorbit.H);
		skp->Text(3 * W / 16, 10 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.0f", G->fidoorbit.V);
		skp->Text(3 * W / 16, 11 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06.2f", G->fidoorbit.GAM);
		skp->Text(3 * W / 16, 12 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%08.1f", G->fidoorbit.A);
		skp->Text(3 * W / 16, 14 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%06.4f", G->fidoorbit.E);
		skp->Text(3 * W / 16, 15 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.2f", G->fidoorbit.I);
		skp->Text(3 * W / 16, 16 * H / 28, Buffer, strlen(Buffer));

		if (G->fidoorbit.E < 1.0)
		{
			sprintf(Buffer, "%08.1f", G->fidoorbit.HA);
			skp->Text(3 * W / 16, 18 * H / 28, Buffer, strlen(Buffer));
			if (G->fidoorbit.E > 0.0001)
			{
				if (G->fidoorbit.PA > 0)
				{
					sprintf(Buffer, "%06.2f N", G->fidoorbit.PA);
				}
				else
				{
					sprintf(Buffer, "%06.2f S", abs(G->fidoorbit.PA));
				}
				skp->Text(3 * W / 16, 19 * H / 28, Buffer, strlen(Buffer));
				if (G->fidoorbit.LA > 0)
				{
					sprintf(Buffer, "%06.2f E", G->fidoorbit.LA);
				}
				else
				{
					sprintf(Buffer, "%06.2f W", abs(G->fidoorbit.LA));
				}
				skp->Text(3 * W / 16, 20 * H / 28, Buffer, strlen(Buffer));
				GET_Display(Buffer, G->fidoorbit.GETA, false);
				skp->Text(3 * W / 16, 21 * H / 28, Buffer, strlen(Buffer));
			}
		}

		sprintf(Buffer, "%08.1f", G->fidoorbit.HP);
		skp->Text(3 * W / 16, 23 * H / 28, Buffer, strlen(Buffer));
		if (G->fidoorbit.E > 0.0001)
		{
			if (G->fidoorbit.PP > 0)
			{
				sprintf(Buffer, "%06.2f N", G->fidoorbit.PP);
			}
			else
			{
				sprintf(Buffer, "%06.2f S", abs(G->fidoorbit.PP));
			}
			skp->Text(3 * W / 16, 24 * H / 28, Buffer, strlen(Buffer));
			if (G->fidoorbit.LP > 0)
			{
				sprintf(Buffer, "%06.2f E", G->fidoorbit.LP);
			}
			else
			{
				sprintf(Buffer, "%06.2f W", abs(G->fidoorbit.LP));
			}
			skp->Text(3 * W / 16, 25 * H / 28, Buffer, strlen(Buffer));
			GET_Display(Buffer, G->fidoorbit.GETP, false);
			skp->Text(3 * W / 16, 26 * H / 28, Buffer, strlen(Buffer));
		}

		if (G->fidoorbit.LPP > 0)
		{
			sprintf(Buffer, "%06.2f E", G->fidoorbit.LPP);
		}
		else
		{
			sprintf(Buffer, "%06.2f W", abs(G->fidoorbit.LPP));
		}
		skp->Text(8 * W / 16, 3 * H / 28, Buffer, strlen(Buffer));
		if (G->fidoorbit.PPP > 0)
		{
			sprintf(Buffer, "%06.2f N", G->fidoorbit.PPP);
		}
		else
		{
			sprintf(Buffer, "%06.2f S", abs(G->fidoorbit.PPP));
		}
		skp->Text(8 * W / 16, 4 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->fidoorbit.GETCC, false);
		skp->Text(8 * W / 16, 5 * H / 28, Buffer, strlen(Buffer));
		if (G->fidoorbit.E > 0.0001)
		{
			sprintf(Buffer, "%05.1f", G->fidoorbit.TAPP);
			skp->Text(8 * W / 16, 6 * H / 28, Buffer, strlen(Buffer));
		}
		if (G->fidoorbit.LNPP > 0)
		{
			sprintf(Buffer, "%06.2f E", G->fidoorbit.LNPP);
		}
		else
		{
			sprintf(Buffer, "%06.2f W", abs(G->fidoorbit.LNPP));
		}
		skp->Text(8 * W / 16, 7 * H / 28, Buffer, strlen(Buffer));

		sprintf(Buffer, "%04d", G->fidoorbit.REVL);
		skp->Text(13 * W / 16, 3 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->fidoorbit.GETL, false);
		skp->Text(13 * W / 16, 4 * H / 28, Buffer, strlen(Buffer));
		if (G->fidoorbit.L > 0)
		{
			sprintf(Buffer, "%06.2f E", G->fidoorbit.L);
		}
		else
		{
			sprintf(Buffer, "%06.2f W", abs(G->fidoorbit.L));
		}
		skp->Text(13 * W / 16, 5 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->fidoorbit.TO, false);
		skp->Text(13 * W / 16, 6 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.1f", G->fidoorbit.K);
		skp->Text(13 * W / 16, 7 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%07.1f", G->fidoorbit.ORBWT);
		skp->Text(13 * W / 16, 8 * H / 28, Buffer, strlen(Buffer));

		sprintf(Buffer, G->fidoorbit.REFR);
		skp->Text(12 * W / 16, 11 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->fidoorbit.GETBV, false);
		skp->Text(12 * W / 16, 12 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%08.1f", G->fidoorbit.HAR);
		skp->Text(12 * W / 16, 13 * H / 28, Buffer, strlen(Buffer));
		if (G->fidoorbit.PAR > 0)
		{
			sprintf(Buffer, "%06.2f N", G->fidoorbit.PAR);
		}
		else
		{
			sprintf(Buffer, "%06.2f S", abs(G->fidoorbit.PAR));
		}
		skp->Text(12 * W / 16, 14 * H / 28, Buffer, strlen(Buffer));
		if (G->fidoorbit.LAR > 0)
		{
			sprintf(Buffer, "%06.2f E", G->fidoorbit.LAR);
		}
		else
		{
			sprintf(Buffer, "%06.2f W", abs(G->fidoorbit.LAR));
		}
		skp->Text(12 * W / 16, 15 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->fidoorbit.GETAR, false);
		skp->Text(12 * W / 16, 16 * H / 28, Buffer, strlen(Buffer));

		sprintf(Buffer, "%08.1f", G->fidoorbit.HPR);
		skp->Text(12 * W / 16, 18 * H / 28, Buffer, strlen(Buffer));
		if (G->fidoorbit.PPR > 0)
		{
			sprintf(Buffer, "%06.2f N", G->fidoorbit.PPR);
		}
		else
		{
			sprintf(Buffer, "%06.2f S", abs(G->fidoorbit.PPR));
		}
		skp->Text(12 * W / 16, 19 * H / 28, Buffer, strlen(Buffer));
		if (G->fidoorbit.LPR > 0)
		{
			sprintf(Buffer, "%06.2f E", G->fidoorbit.LPR);
		}
		else
		{
			sprintf(Buffer, "%06.2f W", abs(G->fidoorbit.LPR));
		}
		skp->Text(12 * W / 16, 20 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->fidoorbit.GETPR, false);
		skp->Text(12 * W / 16, 21 * H / 28, Buffer, strlen(Buffer));
	}
	else if (screen == 42)
	{
		skp->Text(1 * W / 8, 2 * H / 14, "FIDO Orbit Digitals", 19);
		skp->Text(1 * W / 8, 4 * H / 14, "Space Digitals", 14);
		skp->Text(1 * W / 8, 6 * H / 14, "Next Station Contacts", 21);
		skp->Text(1 * W / 8, 8 * H / 14, "Predicted Site Acquisitions", 27);
		skp->Text(1 * W / 8, 10 * H / 14, "Checkout Monitor", 16);
		skp->Text(1 * W / 8, 12 * H / 14, "Sunrise/Sunset Times", 20);

		skp->Text(4 * W / 8, 2 * H / 14, "Moonrise/Moonset Times", 22);
	}
	else if (screen == 43)
	{
		G->CycleSpaceDigitals();

		if (GC->rtcc->med_u00.VEH == 1)
		{
			skp->Text(5 * W / 8, 1 * H / 64, "LEM SPACE DIGITALS", 18);
		}
		else
		{
			skp->Text(5 * W / 8, 1 * H / 64, "CSM SPACE DIGITALS", 18);
		}

		skp->SetFont(font2);

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(1 * W / 32, 2 * H / 28, "STA ID", 6);
		skp->Text(1 * W / 32, 3 * H / 28, "GMTV", 4);
		skp->Text(1 * W / 32, 5 * H / 28, "GET", 3);
		skp->Text(1 * W / 32, 6 * H / 28, "REF", 3);
		skp->Text(1 * W / 32, 8 * H / 28, "GET VECTOR 1", 12);
		skp->Text(1 * W / 32, 10 * H / 28, "REF", 3);
		skp->Text(11 * W / 64, 10 * H / 28, "WT", 2);
		skp->Text(1 * W / 32, 11 * H / 28, "AREA", 4);
		skp->Text(1 * W / 32, 12 * H / 28, "GETA", 4);
		skp->Text(1 * W / 32, 13 * H / 28, "HA", 2);
		skp->Text(1 * W / 32, 14 * H / 28, "HP", 2);
		skp->Text(1 * W / 32, 15 * H / 28, "H", 1);
		skp->Text(1 * W / 32, 16 * H / 28, "V", 1);
		skp->Text(1 * W / 32, 17 * H / 28, "GAM", 3);
		skp->Text(1 * W / 32, 18 * H / 28, "PSI", 3);
		skp->Text(1 * W / 32, 19 * H / 28, "PHI", 3);
		skp->Text(1 * W / 32, 20 * H / 28, "LAM", 3);
		skp->Text(1 * W / 32, 21 * H / 28, "HS", 2);
		skp->Text(1 * W / 32, 22 * H / 28, "HO", 2);
		skp->Text(1 * W / 32, 23 * H / 28, "PHIO", 4);
		skp->Text(1 * W / 32, 24 * H / 28, "IEMP", 4);
		skp->Text(1 * W / 32, 25 * H / 28, "W", 1);
		skp->Text(1 * W / 32, 26 * H / 28, "OMG", 3);
		skp->Text(1 * W / 32, 27 * H / 28, "PRA", 3);
		skp->Text(8 * W / 32, 24 * H / 28, "A", 1);
		skp->Text(8 * W / 32, 25 * H / 28, "L", 1);
		skp->Text(8 * W / 32, 26 * H / 28, "E", 1);
		skp->Text(8 * W / 32, 27 * H / 28, "I", 1);

		skp->Text(11 * W / 32, 2 * H / 28, "WEIGHT", 6);
		skp->Text(11 * W / 32, 3 * H / 28, "GETV", 4);
		skp->Text(11 * W / 32, 8 * H / 28, "GET VECTOR 2", 12);
		skp->Text(11 * W / 32, 10 * H / 28, "GETSI", 5);
		skp->Text(11 * W / 32, 11 * H / 28, "GETCA", 5);
		skp->Text(11 * W / 32, 12 * H / 28, "VCA", 3);
		skp->Text(11 * W / 32, 13 * H / 28, "HCA", 3);
		skp->Text(11 * W / 32, 14 * H / 28, "PCA", 3);
		skp->Text(11 * W / 32, 15 * H / 28, "LCA", 3);
		skp->Text(11 * W / 32, 16 * H / 28, "PSICA", 5);
		skp->Text(11 * W / 32, 17 * H / 28, "GETMN", 5);
		skp->Text(11 * W / 32, 18 * H / 28, "HMN", 3);
		skp->Text(11 * W / 32, 19 * H / 28, "PMN", 3);
		skp->Text(11 * W / 32, 20 * H / 28, "LMN", 3);
		skp->Text(11 * W / 32, 21 * H / 28, "DMN", 3);

		skp->Text(21 * W / 32, 3 * H / 28, "GET AXIS", 8);
		skp->Text(21 * W / 32, 8 * H / 28, "GET VECTOR 3", 12);
		skp->Text(21 * W / 32, 10 * H / 28, "GETSE", 5);
		skp->Text(21 * W / 32, 11 * H / 28, "GETEI", 5);
		skp->Text(21 * W / 32, 12 * H / 28, "VEI", 3);
		skp->Text(21 * W / 32, 13 * H / 28, "GEI", 3);
		skp->Text(21 * W / 32, 14 * H / 28, "PEI", 3);
		skp->Text(21 * W / 32, 15 * H / 28, "LEI", 3);
		skp->Text(21 * W / 32, 16 * H / 28, "PSIEI", 5);
		skp->Text(21 * W / 32, 17 * H / 28, "GETVP", 5);
		skp->Text(21 * W / 32, 18 * H / 28, "VVP", 3);
		skp->Text(21 * W / 32, 19 * H / 28, "HVP", 3);
		skp->Text(21 * W / 32, 20 * H / 28, "PVP", 3);
		skp->Text(21 * W / 32, 21 * H / 28, "LVP", 3);
		skp->Text(21 * W / 32, 22 * H / 28, "PSI VP", 6);
		skp->Text(21 * W / 32, 26 * H / 28, "IE", 2);
		skp->Text(21 * W / 32, 27 * H / 28, "LN", 2);

		skp->Text(9 * W / 32, 5 * H / 28, "V", 1);
		skp->Text(13 * W / 32, 5 * H / 28, "PHI", 3);
		skp->Text(20 * W / 32, 5 * H / 28, "H", 1);
		skp->Text(25 * W / 32, 5 * H / 28, "ADA", 3);

		skp->Text(8 * W / 32, 6 * H / 28, "GAM", 3);
		skp->Text(15 * W / 32, 6 * H / 28, "LAM", 3);
		skp->Text(23 * W / 32, 6 * H / 28, "PSI", 3);

		GET_Display(Buffer, GC->spacedigit.GMTV, false);
		skp->Text(4 * W / 32, 3 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->spacedigit.GET, false);
		skp->Text(4 * W / 32, 5 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, GC->spacedigit.REF);
		skp->Text(4 * W / 32, 6 * H / 28, Buffer, strlen(Buffer));

		sprintf(Buffer, "%07.1f", GC->spacedigit.WEIGHT);
		skp->Text(16 * W / 32, 2 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->spacedigit.GETV, false);
		skp->Text(14 * W / 32, 3 * H / 28, Buffer, strlen(Buffer));

		sprintf(Buffer, "%05.0f", GC->spacedigit.V);
		skp->Text(10 * W / 32, 5 * H / 28, Buffer, strlen(Buffer));
		if (GC->spacedigit.PHI > 0)
		{
			sprintf(Buffer, "%06.2f N", GC->spacedigit.PHI);
		}
		else
		{
			sprintf(Buffer, "%06.2f S", abs(GC->spacedigit.PHI));
		}
		skp->Text(15 * W / 32, 5 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%06.0f", GC->spacedigit.H);
		skp->Text(21 * W / 32, 5 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.2f°", GC->spacedigit.ADA);
		skp->Text(28 * W / 32, 5 * H / 28, Buffer, strlen(Buffer));

		sprintf(Buffer, "%+06.1f°", GC->spacedigit.GAM);
		skp->Text(11 * W / 32, 6 * H / 28, Buffer, strlen(Buffer));
		if (GC->spacedigit.LAM > 0)
		{
			sprintf(Buffer, "%06.2f E", GC->spacedigit.LAM);
		}
		else
		{
			sprintf(Buffer, "%06.2f W", abs(GC->spacedigit.LAM));
		}
		skp->Text(18 * W / 32, 6 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06.1f°", GC->spacedigit.PSI);
		skp->Text(25 * W / 32, 6 * H / 28, Buffer, strlen(Buffer));

		GET_Display(Buffer, GC->spacedigit.GETVector1, false);
		skp->Text(1 * W / 32, 9 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->spacedigit.GETVector2, false);
		skp->Text(11 * W / 32, 9 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->spacedigit.GETVector3, false);
		skp->Text(21 * W / 32, 9 * H / 28, Buffer, strlen(Buffer));

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);

		skp->Text(25 * W / 32, 2 * H / 28, "GETR", 4);

		sprintf(Buffer, GC->spacedigit.REF1);
		skp->Text(11 * W / 64, 10 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.0f", GC->spacedigit.WT);
		skp->Text(10 * W / 32, 10 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%08.1f", GC->spacedigit.HA);
		skp->Text(10 * W / 32, 13 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%08.1f", GC->spacedigit.HP);
		skp->Text(10 * W / 32, 14 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%08.1f", GC->spacedigit.H1);
		skp->Text(10 * W / 32, 15 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.0f", GC->spacedigit.V1);
		skp->Text(10 * W / 32, 16 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06.1f°", GC->spacedigit.GAM1);
		skp->Text(10 * W / 32, 17 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06.1f°", GC->spacedigit.PSI1);
		skp->Text(10 * W / 32, 18 * H / 28, Buffer, strlen(Buffer));
		if (GC->spacedigit.PHI1 > 0)
		{
			sprintf(Buffer, "%06.2f N", GC->spacedigit.PHI1);
		}
		else
		{
			sprintf(Buffer, "%06.2f S", abs(GC->spacedigit.PHI1));
		}
		skp->Text(10 * W / 32, 19 * H / 28, Buffer, strlen(Buffer));
		if (GC->spacedigit.LAM1 > 0)
		{
			sprintf(Buffer, "%06.2f E", GC->spacedigit.LAM1);
		}
		else
		{
			sprintf(Buffer, "%06.2f W", abs(GC->spacedigit.LAM1));
		}
		skp->Text(10 * W / 32, 20 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%08.1f", GC->spacedigit.HS);
		skp->Text(10 * W / 32, 21 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%08.1f", GC->spacedigit.HO);
		skp->Text(10 * W / 32, 22 * H / 28, Buffer, strlen(Buffer));
		if (GC->spacedigit.PHIO > 0)
		{
			sprintf(Buffer, "%06.2f N", GC->spacedigit.PHIO);
		}
		else
		{
			sprintf(Buffer, "%06.2f S", abs(GC->spacedigit.PHIO));
		}
		skp->Text(10 * W / 32, 23 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.2f°", GC->spacedigit.IEMP);
		skp->Text(7 * W / 32, 24 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.2f°", GC->spacedigit.W1);
		skp->Text(7 * W / 32, 25 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.2f°", GC->spacedigit.OMG);
		skp->Text(7 * W / 32, 26 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.2f°", GC->spacedigit.PRA);
		skp->Text(7 * W / 32, 27 * H / 28, Buffer, strlen(Buffer));

		if (GC->spacedigit.A1 > 0)
		{
			sprintf(Buffer, "%06.0f", GC->spacedigit.A1);
			skp->Text(13 * W / 32, 24 * H / 28, Buffer, strlen(Buffer));
		}
		sprintf(Buffer, "%05.2f°", GC->spacedigit.L1);
		skp->Text(13 * W / 32, 25 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.5f", GC->spacedigit.E1);
		skp->Text(13 * W / 32, 26 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.2f°", GC->spacedigit.I1);
		skp->Text(13 * W / 32, 27 * H / 28, Buffer, strlen(Buffer));

		GET_Display(Buffer, GC->spacedigit.GETSI, false);
		skp->Text(20 * W / 32, 10 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->spacedigit.GETCA, false);
		skp->Text(20 * W / 32, 11 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.0f", GC->spacedigit.VCA);
		skp->Text(20 * W / 32, 12 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%08.1f", GC->spacedigit.HCA);
		skp->Text(20 * W / 32, 13 * H / 28, Buffer, strlen(Buffer));
		if (GC->spacedigit.PCA > 0)
		{
			sprintf(Buffer, "%06.2f N", GC->spacedigit.PCA);
		}
		else
		{
			sprintf(Buffer, "%06.2f S", abs(GC->spacedigit.PCA));
		}
		skp->Text(20 * W / 32, 14 * H / 28, Buffer, strlen(Buffer));
		if (GC->spacedigit.LCA > 0)
		{
			sprintf(Buffer, "%06.2f E", GC->spacedigit.LCA);
		}
		else
		{
			sprintf(Buffer, "%06.2f W", abs(GC->spacedigit.LCA));
		}
		skp->Text(20 * W / 32, 15 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06.1f°", GC->spacedigit.PSICA);
		skp->Text(20 * W / 32, 16 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->spacedigit.GETMN, false);
		skp->Text(20 * W / 32, 17 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%08.1f", GC->spacedigit.HMN);
		skp->Text(20 * W / 32, 18 * H / 28, Buffer, strlen(Buffer));
		if (GC->spacedigit.PMN > 0)
		{
			sprintf(Buffer, "%06.2f N", GC->spacedigit.PMN);
		}
		else
		{
			sprintf(Buffer, "%06.2f S", abs(GC->spacedigit.PMN));
		}
		skp->Text(20 * W / 32, 19 * H / 28, Buffer, strlen(Buffer));
		if (GC->spacedigit.LMN > 0)
		{
			sprintf(Buffer, "%06.2f E", GC->spacedigit.LMN);
		}
		else
		{
			sprintf(Buffer, "%06.2f W", abs(GC->spacedigit.LMN));
		}
		skp->Text(20 * W / 32, 20 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.2f", GC->spacedigit.DMN);
		skp->Text(20 * W / 32, 21 * H / 28, Buffer, strlen(Buffer));


		GET_Display(Buffer, GC->spacedigit.GETSE, false);
		skp->Text(30 * W / 32, 10 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->spacedigit.GETEI, false);
		skp->Text(30 * W / 32, 11 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.0f", GC->spacedigit.VEI);
		skp->Text(30 * W / 32, 12 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06.1f°", GC->spacedigit.GEI);
		skp->Text(30 * W / 32, 13 * H / 28, Buffer, strlen(Buffer));
		if (GC->spacedigit.PEI > 0)
		{
			sprintf(Buffer, "%06.2f N", GC->spacedigit.PEI);
		}
		else
		{
			sprintf(Buffer, "%06.2f S", abs(GC->spacedigit.PEI));
		}
		skp->Text(30 * W / 32, 14 * H / 28, Buffer, strlen(Buffer));
		if (GC->spacedigit.LEI > 0)
		{
			sprintf(Buffer, "%06.2f E", GC->spacedigit.LEI);
		}
		else
		{
			sprintf(Buffer, "%06.2f W", abs(GC->spacedigit.LEI));
		}
		skp->Text(30 * W / 32, 15 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06.1f°", GC->spacedigit.PSIEI);
		skp->Text(30 * W / 32, 16 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->spacedigit.GETVP, false);
		skp->Text(30 * W / 32, 17 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%05.0f", GC->spacedigit.VVP);
		skp->Text(30 * W / 32, 18 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%08.1f", GC->spacedigit.HVP);
		skp->Text(30 * W / 32, 19 * H / 28, Buffer, strlen(Buffer));
		if (GC->spacedigit.PVP > 0)
		{
			sprintf(Buffer, "%06.2f N", GC->spacedigit.PVP);
		}
		else
		{
			sprintf(Buffer, "%06.2f S", abs(GC->spacedigit.PVP));
		}
		skp->Text(30 * W / 32, 20 * H / 28, Buffer, strlen(Buffer));
		if (GC->spacedigit.LVP > 0)
		{
			sprintf(Buffer, "%06.2f E", GC->spacedigit.LVP);
		}
		else
		{
			sprintf(Buffer, "%06.2f W", abs(GC->spacedigit.LVP));
		}
		skp->Text(30 * W / 32, 21 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06.1f°", GC->spacedigit.PSIVP);
		skp->Text(30 * W / 32, 22 * H / 28, Buffer, strlen(Buffer));

		sprintf(Buffer, "%05.2f°", GC->spacedigit.IE);
		skp->Text(30 * W / 32, 26 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%06.2f°", GC->spacedigit.LN);
		skp->Text(30 * W / 32, 27 * H / 28, Buffer, strlen(Buffer));
	}
	else if (screen == 44)
	{
		skp->Text(3 * W / 8, 1 * H / 14, "FIDO MISSION PLAN TABLE", 23);

		if (GC->MissionPlanningActive)
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Active", 6);
		}
		else
		{
			skp->Text(1 * W / 8, 2 * H / 14, "Inactive", 8);
		}

		skp->SetFont(font2);

		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(3 * W / 32, 6 * H / 28, "GETBI", 5);
		skp->Text(8 * W / 32, 6 * H / 28, "DT", 2);
		skp->Text(12 * W / 32, 6 * H / 28, "DELTAV", 6);
		skp->Text(16 * W / 32, 6 * H / 28, "DVREM", 5);
		skp->Text(20 * W / 32, 6 * H / 28, "HA", 2);
		skp->Text(24 * W / 32, 6 * H / 28, "HP", 2);
		skp->Text(29 * W / 32, 6 * H / 28, "CODE", 4);

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);

		for (unsigned i = 0;i < GC->mptable.fulltable.size();i++)
		{
			GET_Display(Buffer, OrbMech::GETfromMJD(GC->mptable.fulltable[i].BefMJD, GC->GETbase), false);
			skp->Text(5 * W / 32, (i * 2 + 7) * H / 28, Buffer, strlen(Buffer));

			sprintf(Buffer, "%07.1f", GC->mptable.fulltable[i].DV);
			skp->Text(14 * W / 32, (i * 2 + 7) * H / 28, Buffer, strlen(Buffer));

			sprintf(Buffer, "%07.1f", GC->mptable.fulltable[i].HA);
			skp->Text(22 * W / 32, (i * 2 + 7) * H / 28, Buffer, strlen(Buffer));

			sprintf(Buffer, "%07.1f", GC->mptable.fulltable[i].HP);
			skp->Text(26 * W / 32, (i * 2 + 7) * H / 28, Buffer, strlen(Buffer));

			sprintf(Buffer, GC->mptable.fulltable[i].code.c_str());
			skp->Text(31 * W / 32, (i * 2 + 7) * H / 28, Buffer, strlen(Buffer));
		}

		for (unsigned i = 1;i < GC->mptable.fulltable.size();i++)
		{
			GET_Display(Buffer, GC->mptable.fulltable[i].dt, false);
			skp->Text(10 * W / 32, (i * 2 + 6) * H / 28, Buffer, strlen(Buffer));
		}

	}
	else if (screen == 45)
	{
		G->CycleNextStationContactsDisplay();

		if (GC->rtcc->med_b04.FUNCTION)
		{
			skp->Text(1 * W / 16, 2 * H / 14, "Lunar", 5);
		}
		else
		{
			skp->Text(1 * W / 16, 2 * H / 14, "All", 3);
		}

		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(4 * W / 8, 3 * H / 28, "NEXT STATION CONTACTS", 21);

		skp->SetFont(font2);

		skp->Text(5 * W / 32, 7 * H / 28, "STATION", 7);
		skp->Text(5 * W / 32, 8 * H / 28, "CODE", 4);
		skp->Text(10 * W / 32, 7 * H / 28, "AOS", 3);
		skp->Text(10 * W / 32, 8 * H / 28, "GET", 3);
		skp->Text(16 * W / 32, 7 * H / 28, "LOS", 3);
		skp->Text(16 * W / 32, 8 * H / 28, "GET", 3);
		skp->Text(21 * W / 32, 7 * H / 28, "DELTA", 11);
		skp->Text(21 * W / 32, 8 * H / 28, "T", 3);
		skp->Text(26 * W / 32, 7 * H / 28, "MAX ELEV", 8);
		skp->Text(26 * W / 32, 8 * H / 28, "DEG", 3);

		for (unsigned i = 0;i < 6;i++)
		{
			skp->SetTextAlign(oapi::Sketchpad::LEFT);

			sprintf_s(Buffer, G->nextstatconttable.NextStations[i].StationID);
			skp->Text(4 * W / 32, (i + 10) * H / 28, Buffer, strlen(Buffer));

			skp->SetTextAlign(oapi::Sketchpad::CENTER);

			if (G->nextstatconttable.NextStations[i].BestAvailableAOS)
			{
				skp->Text(15 * W / 64, (i + 10) * H / 28, "*", 1);
			}
			GET_Display(Buffer, G->nextstatconttable.NextStations[i].GETAOS, false);
			skp->Text(10 * W / 32, (i + 10) * H / 28, Buffer, strlen(Buffer));

			if (G->nextstatconttable.NextStations[i].BestAvailableLOS)
			{
				skp->Text(27 * W / 64, (i + 10) * H / 28, "*", 1);
			}
			GET_Display(Buffer, G->nextstatconttable.NextStations[i].GETLOS, false);
			skp->Text(16 * W / 32, (i + 10) * H / 28, Buffer, strlen(Buffer));

			GET_Display(Buffer, G->nextstatconttable.NextStations[i].DELTAT, false);
			skp->Text(21 * W / 32, (i + 10) * H / 28, Buffer, strlen(Buffer));

			if (G->nextstatconttable.NextStations[i].BestAvailableEMAX)
			{
				skp->Text(25 * W / 32, (i + 10) * H / 28, "*", 1);
			}
			sprintf_s(Buffer, "%.0f", G->nextstatconttable.NextStations[i].MAXELEV);
			skp->Text(26 * W / 32, (i + 10) * H / 28, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 46)
	{
		GET_Display(Buffer, GC->rtcc->med_u15.PARAM1);
		skp->Text(1 * W / 16, 3 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->rtcc->med_u15.PARAM2);
		skp->Text(10 * W / 16, 3 * H / 28, Buffer, strlen(Buffer));

		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(4 * W / 8, 5 * H / 28, "PREDICTED SITE ACQUISITION", 26);

		skp->SetFont(font2);

		skp->Text(5 * W / 32, 7 * H / 28, "STATION", 7);
		skp->Text(5 * W / 32, 8 * H / 28, "CODE", 4);
		skp->Text(10 * W / 32, 7 * H / 28, "AOS", 3);
		skp->Text(10 * W / 32, 8 * H / 28, "GET", 3);
		skp->Text(16 * W / 32, 7 * H / 28, "LOS", 3);
		skp->Text(16 * W / 32, 8 * H / 28, "GET", 3);
		skp->Text(21 * W / 32, 7 * H / 28, "DELTA", 11);
		skp->Text(21 * W / 32, 8 * H / 28, "T", 3);
		skp->Text(26 * W / 32, 7 * H / 28, "MAX ELEV", 8);
		skp->Text(26 * W / 32, 8 * H / 28, "DEG", 3);

		for (unsigned i = 0;i < 12;i++)
		{
			skp->SetTextAlign(oapi::Sketchpad::LEFT);

			sprintf_s(Buffer, G->predsiteacqtable.Stations[i].StationID);
			skp->Text(4 * W / 32, (i + 10) * H / 28, Buffer, strlen(Buffer));

			skp->SetTextAlign(oapi::Sketchpad::CENTER);

			if (G->predsiteacqtable.Stations[i].BestAvailableAOS)
			{
				skp->Text(15 * W / 64, (i + 10) * H / 28, "*", 1);
			}
			GET_Display(Buffer, G->predsiteacqtable.Stations[i].GETAOS, false);
			skp->Text(10 * W / 32, (i + 10) * H / 28, Buffer, strlen(Buffer));

			if (G->predsiteacqtable.Stations[i].BestAvailableLOS)
			{
				skp->Text(27 * W / 64, (i + 10) * H / 28, "*", 1);
			}
			GET_Display(Buffer, G->predsiteacqtable.Stations[i].GETLOS, false);
			skp->Text(16 * W / 32, (i + 10) * H / 28, Buffer, strlen(Buffer));

			GET_Display(Buffer, G->predsiteacqtable.Stations[i].DELTAT, false);
			skp->Text(21 * W / 32, (i + 10) * H / 28, Buffer, strlen(Buffer));

			if (G->predsiteacqtable.Stations[i].BestAvailableEMAX)
			{
				skp->Text(25 * W / 32, (i + 10) * H / 28, "*", 1);
			}
			sprintf_s(Buffer, "%.0f", G->predsiteacqtable.Stations[i].MAXELEV);
			skp->Text(26 * W / 32, (i + 10) * H / 28, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 47)
	{
		skp->Text(1 * W / 8, 2 * H / 14, "State Vector Update", 19);
		skp->Text(1 * W / 8, 4 * H / 14, "Landing Site Vector", 19);
		skp->Text(1 * W / 8, 6 * H / 14, "External DV Update", 18);
		if (G->vesseltype < 2)
		{
			skp->Text(1 * W / 8, 8 * H / 14, "Retrofire EXDV Update", 21);
		}
		else
		{
			skp->Text(1 * W / 8, 8 * H / 14, "LGC Descent Update", 18);
		}
		skp->Text(1 * W / 8, 10 * H / 14, "REFSMMAT Update", 15);
	}
	else if (screen == 48)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		if (G->vesseltype < 2)
		{
			if (G->SVSlot)
			{
				skp->Text(4 * W / 8, 1 * H / 14, "CSM NAV UPDATE TO CMC (276)", 27);
			}
			else
			{
				skp->Text(4 * W / 8, 1 * H / 14, "LM NAV UPDATE TO CMC (270)", 26);
			}
		}
		else
		{
			if (G->SVSlot)
			{
				skp->Text(4 * W / 8, 1 * H / 14, "LGC CSM NAV UPDATE (278)", 24);
			}
			else
			{
				skp->Text(4 * W / 8, 1 * H / 14, "LGC LM NAV UPDATE (279)", 23);
			}
		}

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(1 * W / 8, 2 * H / 14, "GET", 3);
		skp->Text(1 * W / 8, 3 * H / 14, "REF", 3);

		GET_Display(Buffer, G->SVDesiredGET);
		skp->Text(2 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		if (G->UplinkSV.gravref == oapiGetObjectByName("Earth"))
		{
			skp->Text(2 * W / 8, 3 * H / 14, "Earth", 5);
		}
		else if (G->UplinkSV.gravref == oapiGetObjectByName("Moon"))
		{
			skp->Text(2 * W / 8, 3 * H / 14, "Moon", 4);
		}

		if (!GC->MissionPlanningActive && G->svtarget != NULL)
		{
			sprintf(Buffer, G->svtarget->GetName());
			skp->Text(5 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));
		}

		skp->Text(5 * W / 32, 9 * H / 28, "OID", 3);
		skp->Text(10 * W / 32, 9 * H / 28, "FCT", 3);
		skp->Text(15 * W / 32, 9 * H / 28, "DSKY V71", 8);
		skp->Text(22 * W / 32, 9 * H / 28, "VECTOR", 6);

		for (int i = 1;i <= 021;i++)
		{
			sprintf(Buffer, "%o", i);
			skp->Text(5 * W / 32, (i + 9) * H / 28, Buffer, strlen(Buffer));
		}

		skp->Text(10 * W / 32, 10 * H / 28, "INDEX", 5);
		skp->Text(10 * W / 32, 11 * H / 28, "ADD", 3);
		skp->Text(10 * W / 32, 12 * H / 28, "VID", 3);
		skp->Text(10 * W / 32, 13 * H / 28, "X", 1);
		skp->Text(10 * W / 32, 14 * H / 28, "X", 1);
		skp->Text(10 * W / 32, 15 * H / 28, "Y", 1);
		skp->Text(10 * W / 32, 16 * H / 28, "Y", 1);
		skp->Text(10 * W / 32, 17 * H / 28, "Z", 1);
		skp->Text(10 * W / 32, 18 * H / 28, "Z", 1);
		skp->Text(10 * W / 32, 19 * H / 28, "X-DOT", 5);
		skp->Text(10 * W / 32, 20 * H / 28, "X-DOT", 5);
		skp->Text(10 * W / 32, 21 * H / 28, "Y-DOT", 5);
		skp->Text(10 * W / 32, 22 * H / 28, "Y-DOT", 5);
		skp->Text(10 * W / 32, 23 * H / 28, "Z-DOT", 5);
		skp->Text(10 * W / 32, 24 * H / 28, "Z-DOT", 5);
		skp->Text(10 * W / 32, 25 * H / 28, "T", 1);
		skp->Text(10 * W / 32, 26 * H / 28, "T", 1);

		for (int i = 0;i < 021;i++)
		{
			sprintf(Buffer, "%05d", G->SVOctals[i]);
			skp->Text(15 * W / 32, (i + 10) * H / 28, Buffer, strlen(Buffer));
		}

		sprintf(Buffer, "%.1f", G->UplinkSV.R.x);
		skp->Text(22 * W / 32, 13 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.1f", G->UplinkSV.R.y);
		skp->Text(22 * W / 32, 15 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.1f", G->UplinkSV.R.z);
		skp->Text(22 * W / 32, 17 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.1f", G->UplinkSV.V.x);
		skp->Text(22 * W / 32, 19 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.1f", G->UplinkSV.V.y);
		skp->Text(22 * W / 32, 21 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.1f", G->UplinkSV.V.z);
		skp->Text(22 * W / 32, 23 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->UplinkSV.MJD, false);
		skp->Text(22 * W / 32, 25 * H / 28, Buffer, strlen(Buffer));
	}
	else if (screen == 49)
	{
		skp->Text(5 * W / 8, (int)(0.5 * H / 14), "Landing Site Update", 19);

		if (G->svtarget != NULL)
		{
			sprintf(Buffer, G->svtarget->GetName());
			skp->Text((int)(0.5 * W / 8), 8 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text((int)(0.5 * W / 8), 8 * H / 14, "No Target!", 10);
		}

		sprintf(Buffer, "%.3f°", GC->LSLat*DEG);
		skp->Text(5 * W / 8, 8 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.3f°", GC->LSLng*DEG);
		skp->Text(5 * W / 8, 10 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.2f NM", GC->LSAlt / 1852.0);
		skp->Text(5 * W / 8, 12 * H / 14, Buffer, strlen(Buffer));
	}
	else if (screen == 50)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		if (G->vesseltype < 2)
		{
			skp->Text(4 * W / 8, 2 * H / 14, "LANDING SITE UPDT TO CMC (293)", 30);
		}
		else
		{
			skp->Text(4 * W / 8, 2 * H / 14, "LANDING SITE UPDT TO LGC (294)", 30);
		}

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(1 * W / 8, 4 * H / 14, "LAT", 3);
		skp->Text(1 * W / 8, 5 * H / 14, "LNG", 3);
		sprintf(Buffer, "%.3f°", GC->LSLat*DEG);
		skp->Text(2 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", GC->LSLng*DEG);
		skp->Text(2 * W / 8, 5 * H / 14, Buffer, strlen(Buffer));

		skp->Text(4 * W / 8, 4 * H / 14, "ALT", 3);
		sprintf(Buffer, "%.4f NM", GC->LSAlt / 1852.0);
		skp->Text(5 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

		skp->Text(5 * W / 32, 13 * H / 28, "OID", 3);
		skp->Text(10 * W / 32, 13 * H / 28, "FCT", 3);
		skp->Text(15 * W / 32, 13 * H / 28, "DSKY V71", 8);
		skp->Text(22 * W / 32, 13 * H / 28, "VECTOR", 6);

		for (int i = 1;i <= 010;i++)
		{
			sprintf(Buffer, "%o", i);
			skp->Text(5 * W / 32, (i + 14) * H / 28, Buffer, strlen(Buffer));
		}

		skp->Text(10 * W / 32, 15 * H / 28, "INDEX", 5);
		skp->Text(10 * W / 32, 16 * H / 28, "ADD", 3);
		skp->Text(10 * W / 32, 17 * H / 28, "X", 1);
		skp->Text(10 * W / 32, 18 * H / 28, "X", 1);
		skp->Text(10 * W / 32, 19 * H / 28, "Y", 1);
		skp->Text(10 * W / 32, 20 * H / 28, "Y", 1);
		skp->Text(10 * W / 32, 21 * H / 28, "Z", 1);
		skp->Text(10 * W / 32, 22 * H / 28, "Z", 1);

		for (int i = 0;i < 010;i++)
		{
			sprintf(Buffer, "%05d", G->RLSOctals[i]);
			skp->Text(15 * W / 32, (i + 15) * H / 28, Buffer, strlen(Buffer));
		}

		sprintf(Buffer, "%.1f", G->RLSUplink.x);
		skp->Text(22 * W / 32, 17 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.1f", G->RLSUplink.y);
		skp->Text(22 * W / 32, 19 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.1f", G->RLSUplink.z);
		skp->Text(22 * W / 32, 21 * H / 28, Buffer, strlen(Buffer));
	}
	else if (screen == 51)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		if (G->vesseltype < 2)
		{
			skp->Text(4 * W / 8, 2 * H / 14, "CMC EXTERNAL DV UPDATE (264)", 28);
		}
		else
		{
			skp->Text(4 * W / 8, 2 * H / 14, "LGC EXTERNAL DV UPDATE (280)", 28);
		}

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(5 * W / 32, 8 * H / 28, "OID", 3);
		skp->Text(10 * W / 32, 8 * H / 28, "FCT", 3);
		skp->Text(15 * W / 32, 8 * H / 28, "DSKY V71", 8);

		for (int i = 1;i <= 012;i++)
		{
			sprintf(Buffer, "%o", i);
			skp->Text(5 * W / 32, (i + 9) * H / 28, Buffer, strlen(Buffer));
		}

		skp->Text(10 * W / 32, 10 * H / 28, "INDEX", 5);
		skp->Text(10 * W / 32, 11 * H / 28, "ADD", 3);
		skp->Text(10 * W / 32, 12 * H / 28, "VGX", 3);
		skp->Text(10 * W / 32, 13 * H / 28, "VGX", 3);
		skp->Text(10 * W / 32, 14 * H / 28, "VGY", 3);
		skp->Text(10 * W / 32, 15 * H / 28, "VGY", 3);
		skp->Text(10 * W / 32, 16 * H / 28, "VGZ", 3);
		skp->Text(10 * W / 32, 17 * H / 28, "VGZ", 3);
		skp->Text(10 * W / 32, 18 * H / 28, "TIGN", 4);
		skp->Text(10 * W / 32, 19 * H / 28, "TIGN", 4);

		for (int i = 0;i < 012;i++)
		{
			sprintf(Buffer, "%05d", G->P30Octals[i]);
			skp->Text(15 * W / 32, (i + 10) * H / 28, Buffer, strlen(Buffer));
		}

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);

		skp->Text(27 * W / 32, 8 * H / 28, "DECIMAL", 7);
		sprintf(Buffer, "%+07.1f", G->dV_LVLH.x / 0.3048);
		skp->Text(27 * W / 32, 12 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f", G->dV_LVLH.y / 0.3048);
		skp->Text(27 * W / 32, 14 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f", G->dV_LVLH.z / 0.3048);
		skp->Text(27 * W / 32, 16 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->P30TIG, false);
		skp->Text(27 * W / 32, 18 * H / 28, Buffer, strlen(Buffer));

	}
	else if (screen == 52)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(4 * W / 8, 2 * H / 14, "CMC RETROFIRE EXTERNAL DV UPDATE (360)", 38);

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(5 * W / 32, 8 * H / 28, "OID", 3);
		skp->Text(10 * W / 32, 8 * H / 28, "FCT", 3);
		skp->Text(15 * W / 32, 8 * H / 28, "DSKY V71", 8);

		for (int i = 1;i <= 016;i++)
		{
			sprintf(Buffer, "%o", i);
			skp->Text(5 * W / 32, (i + 9) * H / 28, Buffer, strlen(Buffer));
		}

		skp->Text(10 * W / 32, 10 * H / 28, "INDEX", 5);
		skp->Text(10 * W / 32, 11 * H / 28, "ADD", 3);
		skp->Text(10 * W / 32, 12 * H / 28, "LAT", 3);
		skp->Text(10 * W / 32, 13 * H / 28, "LAT", 3);
		skp->Text(10 * W / 32, 14 * H / 28, "LONG", 4);
		skp->Text(10 * W / 32, 15 * H / 28, "LONG", 4);
		skp->Text(10 * W / 32, 16 * H / 28, "VGX", 3);
		skp->Text(10 * W / 32, 17 * H / 28, "VGX", 3);
		skp->Text(10 * W / 32, 18 * H / 28, "VGY", 3);
		skp->Text(10 * W / 32, 19 * H / 28, "VGY", 3);
		skp->Text(10 * W / 32, 20 * H / 28, "VGZ", 3);
		skp->Text(10 * W / 32, 21 * H / 28, "VGZ", 3);
		skp->Text(10 * W / 32, 22 * H / 28, "TIGN", 4);
		skp->Text(10 * W / 32, 23 * H / 28, "TIGN", 4);

		for (int i = 0;i < 016;i++)
		{
			sprintf(Buffer, "%05d", G->RetrofireEXDVOctals[i]);
			skp->Text(15 * W / 32, (i + 10) * H / 28, Buffer, strlen(Buffer));
		}

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);

		skp->Text(27 * W / 32, 8 * H / 28, "DECIMAL", 7);
		sprintf(Buffer, "%+.2f°", G->EntryLatcor*DEG);
		skp->Text(27 * W / 32, 12 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+.2f°", G->EntryLngcor*DEG);
		skp->Text(27 * W / 32, 14 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f", G->dV_LVLH.x / 0.3048);
		skp->Text(27 * W / 32, 16 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f", G->dV_LVLH.y / 0.3048);
		skp->Text(27 * W / 32, 18 * H / 28, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f", G->dV_LVLH.z / 0.3048);
		skp->Text(27 * W / 32, 20 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, G->P30TIG, false);
		skp->Text(27 * W / 32, 22 * H / 28, Buffer, strlen(Buffer));

	}
	else if (screen == 53)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		if (G->vesseltype < 2)
		{
			if (G->REFSMMATupl == 0)
			{
				skp->Text(4 * W / 8, 1 * H / 14, "CMC DESIRED REFSMMAT UPDATE (266)", 33);
			}
			else
			{
				skp->Text(4 * W / 8, 1 * H / 14, "CMC REFSMMAT UPDATE (266)", 25);
			}
			
		}
		else
		{
			if (G->REFSMMATupl == 0)
			{
				skp->Text(4 * W / 8, 1 * H / 14, "LGC DESIRED REFSMMAT UPDATE (265)", 33);
			}
			else
			{
				skp->Text(4 * W / 8, 1 * H / 14, "LGC REFSMMAT UPDATE (265)", 25);
			}
		}

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(5 * W / 32, 4 * H / 28, "ID:", 3);
		REFSMMATName(Buffer, G->REFSMMATcur);
		skp->Text(7 * W / 32, 4 * H / 28, Buffer, strlen(Buffer));

		skp->Text(5 * W / 32, 6 * H / 28, "OID", 3);
		skp->Text(10 * W / 32, 6 * H / 28, "FCT", 3);
		skp->Text(15 * W / 32, 6 * H / 28, "DSKY V71", 8);

		for (int i = 1;i <= 024;i++)
		{
			sprintf(Buffer, "%o", i);
			skp->Text(5 * W / 32, (i + 7) * H / 28, Buffer, strlen(Buffer));
		}

		skp->Text(10 * W / 32, 8 * H / 28, "INDEX", 5);
		skp->Text(10 * W / 32, 9 * H / 28, "ADD", 3);
		skp->Text(10 * W / 32, 10 * H / 28, "XIXE", 4);
		skp->Text(10 * W / 32, 11 * H / 28, "XIXE", 4);
		skp->Text(10 * W / 32, 12 * H / 28, "XIYE", 4);
		skp->Text(10 * W / 32, 13 * H / 28, "XIYE", 4);
		skp->Text(10 * W / 32, 14 * H / 28, "XIZE", 4);
		skp->Text(10 * W / 32, 15 * H / 28, "XIZE", 4);
		skp->Text(10 * W / 32, 16 * H / 28, "YIXE", 4);
		skp->Text(10 * W / 32, 17 * H / 28, "YIXE", 4);
		skp->Text(10 * W / 32, 18 * H / 28, "YIYE", 4);
		skp->Text(10 * W / 32, 19 * H / 28, "YIYE", 4);
		skp->Text(10 * W / 32, 20 * H / 28, "YIZE", 4);
		skp->Text(10 * W / 32, 21 * H / 28, "YIZE", 4);
		skp->Text(10 * W / 32, 22 * H / 28, "ZIXE", 4);
		skp->Text(10 * W / 32, 23 * H / 28, "ZIXE", 4);
		skp->Text(10 * W / 32, 24 * H / 28, "ZIYE", 4);
		skp->Text(10 * W / 32, 25 * H / 28, "ZIYE", 4);
		skp->Text(10 * W / 32, 26 * H / 28, "ZIZE", 4);
		skp->Text(10 * W / 32, 27 * H / 28, "ZIZE", 4);

		for (int i = 0;i < 024;i++)
		{
			sprintf(Buffer, "%05d", G->REFSMMAToct[i]);
			skp->Text(15 * W / 32, (i + 8) * H / 28, Buffer, strlen(Buffer));
		}

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);

		for (int i = 0;i < 9;i++)
		{
			sprintf(Buffer, "%f", G->REFSMMAT_BRCS.data[i]);
			skp->Text(27 * W / 32, (i * 2 + 10) * H / 28, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 54)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(4 * W / 8, 1 * H / 14, "Two Impulse Transfer (MED M72)", 30);

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		ThrusterName(Buffer, GC->rtcc->med_m72.Thruster);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		GET_Display(Buffer, G->P30TIG);
		skp->Text(5 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 11 * H / 21, "DVX", 3);
		skp->Text(5 * W / 8, 12 * H / 21, "DVY", 3);
		skp->Text(5 * W / 8, 13 * H / 21, "DVZ", 3);

		AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
		skp->Text(6 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
		skp->Text(6 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
		skp->Text(6 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));
	}
	else if (screen == 55)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		if (GC->rtcc->med_m70.Plan == 0)
		{
			skp->Text(4 * W / 8, 1 * H / 14, "SPQ Transfer (MED M70)", 22);
		}
		else if (GC->rtcc->med_m70.Plan == 1)
		{
			skp->Text(4 * W / 8, 1 * H / 14, "DKI Transfer (MED M70)", 22);
		}
		else
		{
			skp->Text(4 * W / 8, 1 * H / 14, "Descent Plan Transfer (MED M70)", 31);
		}

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		ThrusterName(Buffer, GC->rtcc->med_m70.Thruster);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		GET_Display(Buffer, G->P30TIG);
		skp->Text(5 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 11 * H / 21, "DVX", 3);
		skp->Text(5 * W / 8, 12 * H / 21, "DVY", 3);
		skp->Text(5 * W / 8, 13 * H / 21, "DVZ", 3);

		AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
		skp->Text(6 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
		skp->Text(6 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
		skp->Text(6 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));
	}
	else if (screen == 56)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(4 * W / 8, 1 * H / 14, "DIRECT INPUT TO MPT (MED M66)", 29);

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		if (GC->rtcc->med_m66.Table == 1)
		{
			skp->Text(1 * W / 16, 2 * H / 14, "LM", 2);
		}
		else
		{
			skp->Text(1 * W / 16, 2 * H / 14, "CSM", 3);
		}

		sprintf(Buffer, GC->rtcc->med_m66.code.c_str());
		skp->Text(1 * W / 16, 4 * H / 14, Buffer, strlen(Buffer));

		if (GC->rtcc->med_m66.BurnParamNo == 1)
		{
			skp->Text(1 * W / 16, 6 * H / 14, "Specify DV or DT", 16);

			if (GC->rtcc->med_m40.P1_DV == 0.0)
			{
				sprintf(Buffer, "%.1f s", GC->rtcc->med_m40.P1_DT);
			}
			else
			{
				sprintf(Buffer, "%+.2f ft/s", GC->rtcc->med_m40.P1_DV / 0.3048);
			}
			skp->Text(1 * W / 16, 10 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%06.2f° %06.2f° %06.2f°", GC->rtcc->med_m66.Att.x*DEG, GC->rtcc->med_m66.Att.y*DEG, GC->rtcc->med_m66.Att.z*DEG);
			skp->Text(1 * W / 16, 12 * H / 14, Buffer, strlen(Buffer));

			if (GC->rtcc->med_m66.CoordInd == 0)
			{
				skp->Text(9 * W / 16, 12 * H / 14, "LVLH", 4);
			}
			else if (GC->rtcc->med_m66.CoordInd == 1)
			{
				skp->Text(9 * W / 16, 12 * H / 14, "IMU", 4);
			}
			else
			{
				skp->Text(9 * W / 16, 12 * H / 14, "FDAI", 4);
			}
		}
		else if (GC->rtcc->med_m66.BurnParamNo == 2)
		{
			skp->Text(1 * W / 16, 6 * H / 14, "DV Vector (P30)", 15);

			sprintf(Buffer, "%+.1f %+.1f %+.1f", GC->rtcc->med_m40.P2_DV.x / 0.3048, GC->rtcc->med_m40.P2_DV.y / 0.3048, GC->rtcc->med_m40.P2_DV.z / 0.3048);
			skp->Text(1 * W / 16, 10 * H / 14, Buffer, strlen(Buffer));
		}
		else if (GC->rtcc->med_m66.BurnParamNo == 3)
		{
			skp->Text(1 * W / 16, 6 * H / 14, "DV Vector (IMU)", 15);

			sprintf(Buffer, "%+.1f %+.1f %+.1f", GC->rtcc->med_m40.P3_DV.x / 0.3048, GC->rtcc->med_m40.P3_DV.y / 0.3048, GC->rtcc->med_m40.P3_DV.z / 0.3048);
			skp->Text(1 * W / 16, 10 * H / 14, Buffer, strlen(Buffer));
		}
		else if (GC->rtcc->med_m66.BurnParamNo == 4)
		{
			skp->Text(1 * W / 16, 6 * H / 14, "DV Vector (LVLH)", 16);

			sprintf(Buffer, "%+.1f %+.1f %+.1f", GC->rtcc->med_m40.P4_DV.x / 0.3048, GC->rtcc->med_m40.P4_DV.y / 0.3048, GC->rtcc->med_m40.P4_DV.z / 0.3048);
			skp->Text(1 * W / 16, 10 * H / 14, Buffer, strlen(Buffer));
		}
		else if (GC->rtcc->med_m66.BurnParamNo == 5)
		{
			skp->Text(1 * W / 16, 6 * H / 14, "Lambert", 7);
		}

		GET_Display(Buffer, GC->rtcc->med_m66.GETBI);
		skp->Text(1 * W / 16, 8 * H / 14, Buffer, strlen(Buffer));

		ThrusterName(Buffer, GC->rtcc->med_m66.Thruster);
		skp->Text(5 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		if (GC->rtcc->med_m66.ConfigChangeInd == 0)
		{
			skp->Text(5 * W / 8, 4 * H / 14, "No change", 9);
		}
		else
		{
			if (GC->rtcc->med_m66.ConfigChangeInd == 1)
			{
				skp->Text(5 * W / 8, 4 * H / 14, "Undocking", 9);
			}
			else if (GC->rtcc->med_m66.ConfigChangeInd == 2)
			{
				skp->Text(5 * W / 8, 4 * H / 14, "Docking", 7);
			}

			VehicleConfigName(Buffer, GC->rtcc->med_m66.FinalConfig);
			skp->Text(5 * W / 8, 6 * H / 14, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 57)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(4 * W / 8, 1 * H / 14, "GPM Transfer (MED M65)", 22);

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		ThrusterName(Buffer, GC->rtcc->med_m65.Thruster);
		skp->Text(1 * W / 8, 2 * H / 14, Buffer, strlen(Buffer));

		GET_Display(Buffer, G->P30TIG);
		skp->Text(5 * W / 8, 10 * H / 21, Buffer, strlen(Buffer));

		skp->Text(5 * W / 8, 11 * H / 21, "DVX", 3);
		skp->Text(5 * W / 8, 12 * H / 21, "DVY", 3);
		skp->Text(5 * W / 8, 13 * H / 21, "DVZ", 3);

		AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
		skp->Text(6 * W / 8, 11 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
		skp->Text(6 * W / 8, 12 * H / 21, Buffer, strlen(Buffer));
		AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
		skp->Text(6 * W / 8, 13 * H / 21, Buffer, strlen(Buffer));
	}
	else if (screen == 58)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(4 * W / 8, (int)(0.5 * H / 14), "CHECKOUT MONITOR", 16);

		skp->SetFont(font2);

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(1 * W / 32, 3 * H / 28, "GET", 3);
		GET_Display(Buffer, GC->checkmon.GET, false);
		skp->Text(4 * W / 32, 3 * H / 28, Buffer, strlen(Buffer));

		skp->Text(1 * W / 32, 4 * H / 28, "GMT", 3);
		GET_Display(Buffer, GC->checkmon.GMT, false);
		skp->Text(4 * W / 32, 4 * H / 28, Buffer, strlen(Buffer));

		skp->Text(1 * W / 32, 5 * H / 28, "VEH", 3);
		skp->Text(4 * W / 32, 5 * H / 28, GC->checkmon.VEH, 3);

		skp->Text(9 * W / 32, 3 * H / 28, "R-DAY", 5);
		skp->Text(10 * W / 32, 4 * H / 28, "VID", 3);
		skp->Text(7 * W / 32, 5 * H / 28, "XT", 2);
		skp->Text(18 * W / 32, 3 * H / 28, "K-FAC", 5);
		skp->Text(16 * W / 32, 4 * H / 28, "RF", 2);
		skp->Text(18 * W / 32, 4 * H / 28, GC->checkmon.RF, 3);
		skp->Text(14 * W / 32, 5 * H / 28, "YT", 2);
		skp->Text(21 * W / 32, 4 * H / 28, "A", 1);
		
		skp->Text(25 * W / 32, 4 * H / 28, "CFG", 3);
		skp->Text(21 * W / 32, 5 * H / 28, "ZT", 2);
		skp->Text(25 * W / 32, 6 * H / 28, "OPTION", 6);
		skp->Text(26 * W / 32, 7 * H / 28, "NV", 2);

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);

		skp->Text(31 * W / 32, 4 * H / 28, GC->checkmon.CFG, 3);
		skp->Text(31 * W / 32, 6 * H / 28, GC->checkmon.Option, 3);

		skp->Text(2 * W / 32, 7 * H / 28, "X", 1);
		skp->Text(2 * W / 32, 9 * H / 28, "Y", 1);
		skp->Text(2 * W / 32, 11 * H / 28, "Z", 1);
		skp->Text(2 * W / 32, 13 * H / 28, "DX", 2);
		skp->Text(2 * W / 32, 15 * H / 28, "DY", 2);
		skp->Text(2 * W / 32, 17 * H / 28, "DZ", 2);

		if (GC->checkmon.unit == 0)
		{
			sprintf(Buffer, "%+013.9f", GC->checkmon.Pos.x);
			skp->Text(10 * W / 32, 7 * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+013.9f", GC->checkmon.Pos.y);
			skp->Text(10 * W / 32, 9 * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+013.9f", GC->checkmon.Pos.z);
			skp->Text(10 * W / 32, 11 * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+013.9f", GC->checkmon.Vel.x);
			skp->Text(10 * W / 32, 13 * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+013.9f", GC->checkmon.Vel.y);
			skp->Text(10 * W / 32, 15 * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+013.9f", GC->checkmon.Vel.z);
			skp->Text(10 * W / 32, 17 * H / 28, Buffer, strlen(Buffer));
		}
		else
		{
			sprintf(Buffer, "%+012.0f", GC->checkmon.Pos.x);
			skp->Text(10 * W / 32, 7 * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+012.0f", GC->checkmon.Pos.y);
			skp->Text(10 * W / 32, 9 * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+012.0f", GC->checkmon.Pos.z);
			skp->Text(10 * W / 32, 11 * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+013.6f", GC->checkmon.Vel.x);
			skp->Text(10 * W / 32, 13 * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+013.6f", GC->checkmon.Vel.y);
			skp->Text(10 * W / 32, 15 * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+013.6f", GC->checkmon.Vel.z);
			skp->Text(10 * W / 32, 17 * H / 28, Buffer, strlen(Buffer));
		}

		skp->Text(4 * W / 32, 19 * H / 28, "LO/C", 4);
		GET_Display(Buffer, GC->checkmon.LOC, false);
		skp->Text(9 * W / 32, 19 * H / 28, Buffer, strlen(Buffer));
		skp->Text(4 * W / 32, 20 * H / 28, "GRR/C", 5);
		GET_Display(Buffer, GC->checkmon.GRRC, false);
		skp->Text(9 * W / 32, 20 * H / 28, Buffer, strlen(Buffer));
		skp->Text(4 * W / 32, 21 * H / 28, "ZS/C", 4);
		GET_Display(Buffer, GC->checkmon.ZSC, false);
		skp->Text(9 * W / 32, 21 * H / 28, Buffer, strlen(Buffer));
		skp->Text(4 * W / 32, 22 * H / 28, "GRR/S", 5);
		GET_Display(Buffer, GC->checkmon.GRRS, false);
		skp->Text(9 * W / 32, 22 * H / 28, Buffer, strlen(Buffer));
		skp->Text(4 * W / 32, 23 * H / 28, "ZS/L", 4);
		GET_Display(Buffer, GC->checkmon.ZSL, false);
		skp->Text(9 * W / 32, 23 * H / 28, Buffer, strlen(Buffer));
		skp->Text(4 * W / 32, 24 * H / 28, "ZS/A", 4);
		GET_Display(Buffer, GC->checkmon.ZSA, false);
		skp->Text(9 * W / 32, 24 * H / 28, Buffer, strlen(Buffer));
		skp->Text(4 * W / 32, 25 * H / 28, "EPHB", 4);
		GET_Display(Buffer, GC->checkmon.EPHB, false);
		skp->Text(9 * W / 32, 25 * H / 28, Buffer, strlen(Buffer));
		skp->Text(4 * W / 32, 26 * H / 28, "EPHE", 4);
		GET_Display(Buffer, GC->checkmon.EPHE, false);
		skp->Text(9 * W / 32, 26 * H / 28, Buffer, strlen(Buffer));
		skp->Text(4 * W / 32, 27 * H / 28, "THT", 4);
		GET_Display(Buffer, GC->checkmon.THT, false);
		skp->Text(9 * W / 32, 27 * H / 28, Buffer, strlen(Buffer));

		skp->Text(13 * W / 32, 7 * H / 28, "V", 1);
		sprintf(Buffer, "%07.1f", GC->checkmon.V_i);
		skp->Text(17 * W / 32, 7 * H / 28, Buffer, strlen(Buffer));
		skp->Text(13 * W / 32, 9 * H / 28, "PTH", 3);
		sprintf(Buffer, "%+07.3f", GC->checkmon.gamma_i);
		skp->Text(17 * W / 32, 9 * H / 28, Buffer, strlen(Buffer));
		skp->Text(13 * W / 32, 11 * H / 28, "AZ", 2);
		sprintf(Buffer, "%+07.3f", GC->checkmon.psi);
		skp->Text(17 * W / 32, 11 * H / 28, Buffer, strlen(Buffer));
		skp->Text(13 * W / 32, 13 * H / 28, "LATC", 4);
		sprintf(Buffer, "%+07.4f", GC->checkmon.phi_c);
		skp->Text(17 * W / 32, 13 * H / 28, Buffer, strlen(Buffer));
		skp->Text(13 * W / 32, 15 * H / 28, "LONC", 4);
		sprintf(Buffer, "%+07.3f", GC->checkmon.lambda);
		skp->Text(17 * W / 32, 15 * H / 28, Buffer, strlen(Buffer));
		skp->Text(12 * W / 32, 17 * H / 28, "HS", 2);
		sprintf(Buffer, "%07.2f", GC->checkmon.h_s);
		skp->Text(17 * W / 32, 17 * H / 28, Buffer, strlen(Buffer));

		skp->Text(19 * W / 32, 7 * H / 28, "A", 1);
		sprintf(Buffer, "%08.2f", GC->checkmon.a);
		skp->Text(24 * W / 32, 7 * H / 28, Buffer, strlen(Buffer));
		skp->Text(20 * W / 32, 9 * H / 28, "E", 1);
		sprintf(Buffer, "%07.5f", GC->checkmon.e);
		skp->Text(24 * W / 32, 9 * H / 28, Buffer, strlen(Buffer));
		skp->Text(20 * W / 32, 11 * H / 28, "I", 1);
		sprintf(Buffer, "%07.3f", GC->checkmon.i);
		skp->Text(24 * W / 32, 11 * H / 28, Buffer, strlen(Buffer));
		skp->Text(20 * W / 32, 13 * H / 28, "AP", 2);
		sprintf(Buffer, "%07.3f", GC->checkmon.omega_p);
		skp->Text(24 * W / 32, 13 * H / 28, Buffer, strlen(Buffer));
		skp->Text(20 * W / 32, 15 * H / 28, "RAN", 3);
		sprintf(Buffer, "%07.3f", GC->checkmon.Omega);
		skp->Text(24 * W / 32, 15 * H / 28, Buffer, strlen(Buffer));
		skp->Text(20 * W / 32, 17 * H / 28, "TA", 2);
		skp->Text(20 * W / 32, 19 * H / 28, "MA", 2);
		if (GC->checkmon.TABlank == false)
		{
			sprintf(Buffer, "%07.3f", GC->checkmon.nu);
			skp->Text(24 * W / 32, 17 * H / 28, Buffer, strlen(Buffer));
		}
		if (GC->checkmon.MABlank == false)
		{
			sprintf(Buffer, "%07.3f", GC->checkmon.m);
			skp->Text(24 * W / 32, 19 * H / 28, Buffer, strlen(Buffer));
		}
		skp->Text(27 * W / 32, 9 * H / 28, "WT", 2);
		sprintf(Buffer, "%06.0f", GC->checkmon.WT);
		skp->Text(31 * W / 32, 9 * H / 28, Buffer, strlen(Buffer));
		skp->Text(27 * W / 32, 10 * H / 28, "WC", 2);
		sprintf(Buffer, "%05.0f", GC->checkmon.WC);
		skp->Text(31 * W / 32, 10 * H / 28, Buffer, strlen(Buffer));
		skp->Text(27 * W / 32, 11 * H / 28, "SPS", 3);
		sprintf(Buffer, "%05.0f", GC->checkmon.SPS);
		skp->Text(31 * W / 32, 11 * H / 28, Buffer, strlen(Buffer));
		skp->Text(27 * W / 32, 12 * H / 28, "RCSC", 4);
		sprintf(Buffer, "%04.0f", GC->checkmon.RCS_C);
		skp->Text(31 * W / 32, 12 * H / 28, Buffer, strlen(Buffer));
		skp->Text(27 * W / 32, 13 * H / 28, "WL", 2);
		sprintf(Buffer, "%05.0f", GC->checkmon.WL);
		skp->Text(31 * W / 32, 13 * H / 28, Buffer, strlen(Buffer));
		skp->Text(27 * W / 32, 14 * H / 28, "APS", 3);
		sprintf(Buffer, "%05.0f", GC->checkmon.APS);
		skp->Text(31 * W / 32, 14 * H / 28, Buffer, strlen(Buffer));
		skp->Text(27 * W / 32, 15 * H / 28, "DPS", 3);
		sprintf(Buffer, "%05.0f", GC->checkmon.DPS);
		skp->Text(31 * W / 32, 15 * H / 28, Buffer, strlen(Buffer));
		skp->Text(27 * W / 32, 16 * H / 28, "RCSL", 4);
		sprintf(Buffer, "%04.0f", GC->checkmon.RCS_L);
		skp->Text(31 * W / 32, 16 * H / 28, Buffer, strlen(Buffer));
		skp->Text(27 * W / 32, 17 * H / 28, "J2", 2);
		sprintf(Buffer, "%05.0f", GC->checkmon.J2);
		skp->Text(31 * W / 32, 17 * H / 28, Buffer, strlen(Buffer));

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(10 * W / 32, 19 * H / 28, "HA", 2);
		sprintf(Buffer, "%07.2f", GC->checkmon.h_a);
		skp->Text(14 * W / 32, 19 * H / 28, Buffer, strlen(Buffer));
		skp->Text(10 * W / 32, 20 * H / 28, "HP", 2);
		sprintf(Buffer, "%07.2f", GC->checkmon.h_p);
		skp->Text(14 * W / 32, 20 * H / 28, Buffer, strlen(Buffer));
		skp->Text(10 * W / 32, 21 * H / 28, "HO", 2);
		skp->Text(10 * W / 32, 22 * H / 28, "HO", 2);
		if (GC->checkmon.HOBlank == false)
		{
			sprintf(Buffer, "%07.2f", GC->checkmon.h_o_NM);
			skp->Text(14 * W / 32, 21 * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%07.0f", GC->checkmon.h_o_ft);
			skp->Text(14 * W / 32, 22 * H / 28, Buffer, strlen(Buffer));
		}
		skp->Text(10 * W / 32, 23 * H / 28, "LATD", 4);
		sprintf(Buffer, "%+07.3f", GC->checkmon.phi_D);
		skp->Text(14 * W / 32, 23 * H / 28, Buffer, strlen(Buffer));
		skp->Text(10 * W / 32, 24 * H / 28, "LOND", 4);
		sprintf(Buffer, "%07.3f", GC->checkmon.lambda_D);
		skp->Text(14 * W / 32, 24 * H / 28, Buffer, strlen(Buffer));
		skp->Text(10 * W / 32, 25 * H / 28, "R", 1);
		sprintf(Buffer, "%+09.2f", GC->checkmon.R);
		skp->Text(14 * W / 32, 25 * H / 28, Buffer, strlen(Buffer));
		skp->Text(10 * W / 32, 26 * H / 28, "DECL", 4);
		sprintf(Buffer, "%+07.3f", GC->checkmon.deltaL);
		skp->Text(14 * W / 32, 26 * H / 28, Buffer, strlen(Buffer));
		skp->Text(10 * W / 32, 27 * H / 28, "LSB", 3);
		skp->Text(20 * W / 32, 27 * H / 28, "LLS", 3);
		if (GC->checkmon.LSTBlank == false)
		{
			GET_Display(Buffer, GC->checkmon.LAL, false);
			skp->Text(14 * W / 32, 27 * H / 28, Buffer, strlen(Buffer));
			GET_Display(Buffer, GC->checkmon.LOL, false);
			skp->Text(24 * W / 32, 27 * H / 28, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 59)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(5 * W / 8, 1 * H / 28, "MPT INITIALIZATION", 18);

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		if (GC->rtcc->med_m50.Table == 1)
		{
			skp->Text(1 * W / 16, 2 * H / 14, "LM", 2);
		}
		else
		{
			skp->Text(1 * W / 16, 2 * H / 14, "CSM", 3);
		}

		if (GC->rtcc->med_m50.Table == 1)
			{
				if (GC->pLM != NULL)
				{
					sprintf(Buffer, GC->pLM->GetName());
					skp->Text(1 * W / 16, 4 * H / 14, Buffer, strlen(Buffer));
				}
				else
				{
					skp->Text(1 * W / 16, 4 * H / 14, "None", 4);
				}
			}
		else
		{
			if (GC->pCSM != NULL)
			{
				sprintf(Buffer, GC->pCSM->GetName());
				skp->Text(1 * W / 16, 4 * H / 14, Buffer, strlen(Buffer));
			}
			else
			{
				skp->Text(1 * W / 16, 4 * H / 14, "None", 4);
			}
		}

		if (GC->rtcc->med_m50.CSMWT >= 0)
		{
			sprintf(Buffer, "%.0f lbs", GC->rtcc->med_m50.CSMWT / 0.45359237);
			skp->Text(1 * W / 16, 6 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(1 * W / 16, 6 * H / 14, "No Update", 9);
		}

		if (GC->rtcc->med_m50.SIVBWT >= 0)
		{
			sprintf(Buffer, "%.0f lbs", GC->rtcc->med_m50.SIVBWT / 0.45359237);
			skp->Text(1 * W / 16, 8 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(1 * W / 16, 8 * H / 14, "No Update", 9);
		}

		if (GC->rtcc->med_m50.LMWT >= 0)
		{
			sprintf(Buffer, "%.0f lbs", GC->rtcc->med_m50.LMWT / 0.45359237);
			skp->Text(1 * W / 16, 10 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(1 * W / 16, 10 * H / 14, "No Update", 9);
		}

		VehicleConfigName(Buffer, GC->rtcc->med_m55.ConfigCode);
		skp->Text(10 * W / 16, 2 * H / 14, Buffer, strlen(Buffer));

		if (GC->mptInitError == 1)
		{
			skp->Text(7 * W / 16, 13 * H / 14, "Mass update successful!", 23);
		}
		else if (GC->mptInitError == 2)
		{
			skp->Text(7 * W / 16, 13 * H / 14, "Mass update rejected!", 21);
		}
		else if (GC->mptInitError == 3)
		{
			skp->Text(7 * W / 16, 13 * H / 14, "Config update successful!", 25);
		}
		else if (GC->mptInitError == 4)
		{
			skp->Text(7 * W / 16, 13 * H / 14, "Config update rejected!", 23);
		}
		else if (GC->mptInitError == 5)
		{
			skp->Text(7 * W / 16, 13 * H / 14, "Trajectory update successful!", 29);
		}
		else if (GC->mptInitError == 6)
		{
			skp->Text(7 * W / 16, 13 * H / 14, "Trajectory update rejected!", 27);
		}
		
	}
	else if (screen == 60)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(4 * W / 8, 1 * H / 28, "LM DESCENT PLANNING (MSK 0086)", 30);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->SetFont(font2);

		skp->Text(1 * W / 32, 3 * H / 28, "STA ID", 6);
		skp->Text(1 * W / 32, 4 * H / 28, "LM WT", 5);
		sprintf(Buffer, "%07.1f", GC->descplantable.LMWT);
		skp->Text(5 * W / 32, 4 * H / 28, Buffer, strlen(Buffer));
		skp->Text(9 * W / 32, 3 * H / 28, "GMTV", 4);
		skp->Text(9 * W / 32, 4 * H / 28, "GETV", 4);
		skp->Text(17 * W / 32, 3 * H / 28, "MODE", 4);
		sprintf(Buffer, "%d", GC->descplantable.MODE);
		skp->Text(21 * W / 32, 3 * H / 28, Buffer, strlen(Buffer));
		skp->Text(17 * W / 32, 4 * H / 28, "TL", 2);

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);

		GET_Display(Buffer, GC->descplantable.GMTV, false);
		skp->Text(16 * W / 32, 3 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->descplantable.GETV, false);
		skp->Text(16 * W / 32, 4 * H / 28, Buffer, strlen(Buffer));

		skp->Text(26 * W / 32, 3 * H / 28, "LAT LLS", 7);
		skp->Text(26 * W / 32, 4 * H / 28, "LONG LLS", 8);

		if (GC->descplantable.LAT_LLS > 0)
		{
			sprintf(Buffer, "%05.2f°N", GC->descplantable.LAT_LLS);
		}
		else
		{
			sprintf(Buffer, "%05.2f°S", abs(GC->descplantable.LAT_LLS));
		}
		skp->Text(31 * W / 32, 3 * H / 28, Buffer, strlen(Buffer));
		if (GC->descplantable.LONG_LLS > 0)
		{
			sprintf(Buffer, "%06.2f°E", GC->descplantable.LONG_LLS);
		}
		else
		{
			sprintf(Buffer, "%06.2f°W", abs(GC->descplantable.LONG_LLS));
		}
		skp->Text(31 * W / 32, 4 * H / 28, Buffer, strlen(Buffer));

		sprintf(Buffer, "%07.3f°", GC->descplantable.DescAsc);
		skp->Text(30 * W / 32, 20 * H / 28, Buffer, strlen(Buffer));

		sprintf(Buffer, GC->descplantable.DescAzMode);
		skp->Text(27 * W / 32, 19 * H / 28, Buffer, strlen(Buffer));

		sprintf(Buffer, "%+06.2f", GC->descplantable.PD_ThetaIgn);
		skp->Text(10 * W / 32, 20 * H / 28, Buffer, strlen(Buffer));

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(1 * W / 64, 6 * H / 28, "MVR/REV", 7);
		skp->Text(5 * W / 32, 6 * H / 28, "GETTH/GETIG", 11);
		skp->Text(23 * W / 64, 6 * H / 28, "LIG/DV", 6);
		skp->Text(15 * W / 32, 6 * H / 28, "HAC/HPC", 7);
		skp->Text(20 * W / 32, 6 * H / 28, "DEL/THPC", 8);
		skp->Text(25 * W / 32, 6 * H / 28, "DVX/DVY", 7);
		skp->Text(30 * W / 32, 6 * H / 28, "DVZ", 3);

		for (int i = 0;i < 4;i++)
		{
			sprintf(Buffer, GC->descplantable.MVR[i].c_str());
			skp->Text(2 * W / 32, (7 + 3 * i) * H / 28, Buffer, strlen(Buffer));
			GET_Display(Buffer, GC->descplantable.GETTH[i], false);
			skp->Text(6 * W / 32, (7 + 3 * i) * H / 28, Buffer, strlen(Buffer));
			GET_Display(Buffer, GC->descplantable.GETIG[i], false);
			skp->Text(6 * W / 32, (8 + 3 * i) * H / 28, Buffer, strlen(Buffer));
			if (GC->descplantable.LIG[i] > 0)
			{
				sprintf(Buffer, "%06.2f°E", GC->descplantable.LIG[i]);
			}
			else
			{
				sprintf(Buffer, "%06.2f°W", abs(GC->descplantable.LIG[i]));
			}
			skp->Text(12 * W / 32, (7 + 3 * i) * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%06.2f", GC->descplantable.DV[i]);
			skp->Text(12 * W / 32, (8 + 3 * i) * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.2f", GC->descplantable.AC[i]);
			skp->Text(17 * W / 32, (7 + 3 * i) * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.2f", GC->descplantable.HPC[i]);
			skp->Text(17 * W / 32, (8 + 3 * i) * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+06.1f", GC->descplantable.DVVector[i].x);
			skp->Text(25 * W / 32, (7 + 3 * i) * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+06.1f", GC->descplantable.DVVector[i].y);
			skp->Text(25 * W / 32, (8 + 3 * i) * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+06.1f", GC->descplantable.DVVector[i].z);
			skp->Text(29 * W / 32, (7 + 3 * i) * H / 28, Buffer, strlen(Buffer));
		}

		skp->Text(1 * W / 32, 19 * H / 28, "POWERED DESCENT", 15);
		skp->Text(1 * W / 32, 20 * H / 28, "THETA IGN", 9);
		skp->Text(1 * W / 32, 21 * H / 28, "PROP. REM", 9);

		skp->Text(12 * W / 32, 19 * H / 28, "GETTH", 5);
		GET_Display(Buffer, GC->descplantable.PD_GETTH, false);
		skp->Text(16 * W / 32, 19 * H / 28, Buffer, strlen(Buffer));
		skp->Text(12 * W / 32, 20 * H / 28, "GETIG", 5);
		GET_Display(Buffer, GC->descplantable.PD_GETIG, false);
		skp->Text(16 * W / 32, 20 * H / 28, Buffer, strlen(Buffer));
		skp->Text(12 * W / 32, 21 * H / 28, "GETTD", 5);
		GET_Display(Buffer, GC->descplantable.PD_GETTD, false);
		skp->Text(16 * W / 32, 21 * H / 28, Buffer, strlen(Buffer));

		skp->Text(21 * W / 32, 19 * H / 28, "MODE", 4);
		skp->Text(21 * W / 32, 20 * H / 28, "DESC AZ", 7);
		skp->Text(21 * W / 32, 21 * H / 28, "SN.LK.A", 7);
	}
	else if (screen == 61)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(4 * W / 8, 2 * H / 14, "LGC DESCENT TARGET UPDATE (295)", 31);

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(1 * W / 8, 4 * H / 14, "TLAND:", 6);
		GET_Display(Buffer, GC->t_Land);
		skp->Text(3 * W / 8, 4 * H / 14, Buffer, strlen(Buffer));

		skp->Text(5 * W / 32, 13 * H / 28, "OID", 3);
		skp->Text(10 * W / 32, 13 * H / 28, "FCT", 3);
		skp->Text(15 * W / 32, 13 * H / 28, "DSKY V72", 8);

		for (int i = 1;i <= 5;i++)
		{
			sprintf(Buffer, "%o", i);
			skp->Text(5 * W / 32, (i + 14) * H / 28, Buffer, strlen(Buffer));
		}

		skp->Text(10 * W / 32, 15 * H / 28, "INDEX", 5);
		skp->Text(10 * W / 32, 16 * H / 28, "ADD1", 4);
		skp->Text(10 * W / 32, 17 * H / 28, "TLAND", 5);
		skp->Text(10 * W / 32, 18 * H / 28, "ADD2", 4);
		skp->Text(10 * W / 32, 19 * H / 28, "TLAND", 5);

		for (int i = 0;i < 5;i++)
		{
			sprintf(Buffer, "%05d", G->TLANDOctals[i]);
			skp->Text(15 * W / 32, (i + 15) * H / 28, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 62)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(4 * W / 8, 1 * H / 14, "SUNRISE/SUNSET TIMES (MSK 1502)", 31);

		skp->SetFont(font2);

		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(9 * W / 32, 4 * H / 28, "SUNRISE", 7);
		skp->Text(25 * W / 32, 4 * H / 28, "SUNSET", 6);

		skp->Text(1 * W / 32, 5 * H / 28, "REV", 3);
		skp->Text(1 * W / 32, 6 * H / 28, "NO", 2);

		skp->Text(9 * W / 64, 5 * H / 28, "GETTR", 5);
		skp->Text(9 * W / 64, 6 * H / 28, "HH:MM:SS", 8);

		skp->Text(19 * W / 64, 5 * H / 28, "GETSR", 5);
		skp->Text(19 * W / 64, 6 * H / 28, "HH:MM:SS", 8);

		skp->Text(27 * W / 64, 5 * H / 28, "PIT", 3);
		skp->Text(27 * W / 64, 6 * H / 28, "DEG", 3);

		skp->Text(32 * W / 64, 5 * H / 28, "YAW", 3);
		skp->Text(32 * W / 64, 6 * H / 28, "DEG", 3);

		skp->Text(39 * W / 64, 5 * H / 28, "GETTS", 5);
		skp->Text(39 * W / 64, 6 * H / 28, "HH:MM:SS", 8);

		skp->Text(49 * W / 64, 5 * H / 28, "GETSS", 5);
		skp->Text(49 * W / 64, 6 * H / 28, "HH:MM:SS", 8);

		skp->Text(56 * W / 64, 5 * H / 28, "PIT", 3);
		skp->Text(56 * W / 64, 6 * H / 28, "DEG", 3);

		skp->Text(61 * W / 64, 5 * H / 28, "YAW", 3);
		skp->Text(61 * W / 64, 6 * H / 28, "DEG", 3);

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		for (int i = 0;i < 8;i++)
		{
			sprintf(Buffer, "%02d", GC->rtcc->EZSSTAB.data[i].REV);
			skp->Text(1 * W / 32, (i + 8) * H / 28, Buffer, strlen(Buffer));

			GET_Display(Buffer, GC->rtcc->EZSSTAB.data[i].GETTR, false);
			skp->Text(3 * W / 32, (i + 8) * H / 28, Buffer, strlen(Buffer));
			GET_Display(Buffer, GC->rtcc->EZSSTAB.data[i].GETSR, false);
			skp->Text(8 * W / 32, (i + 8) * H / 28, Buffer, strlen(Buffer));

			sprintf(Buffer, "%03.0f", GC->rtcc->EZSSTAB.data[i].theta_SR);
			skp->Text(13 * W / 32, (i + 8) * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%03.0f", GC->rtcc->EZSSTAB.data[i].psi_SR);
			skp->Text(15 * W / 32, (i + 8) * H / 28, Buffer, strlen(Buffer));

			GET_Display(Buffer, GC->rtcc->EZSSTAB.data[i].GETTS, false);
			skp->Text(35 * W / 64, (i + 8) * H / 28, Buffer, strlen(Buffer));
			GET_Display(Buffer, GC->rtcc->EZSSTAB.data[i].GETSS, false);
			skp->Text(45 * W / 64, (i + 8) * H / 28, Buffer, strlen(Buffer));

			sprintf(Buffer, "%03.0f", GC->rtcc->EZSSTAB.data[i].theta_SS);
			skp->Text(55 * W / 64, (i + 8) * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%03.0f", GC->rtcc->EZSSTAB.data[i].psi_SS);
			skp->Text(59 * W / 64, (i + 8) * H / 28, Buffer, strlen(Buffer));
		}
	}
	else if (screen == 63)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(4 * W / 8, 1 * H / 14, "MOONRISE/MOONSET TIMES (MSK 1501)", 33);

		skp->SetFont(font2);

		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(9 * W / 32, 4 * H / 28, "MOONRISE", 8);
		skp->Text(25 * W / 32, 4 * H / 28, "MOONSET", 7);

		skp->Text(1 * W / 32, 5 * H / 28, "REV", 3);
		skp->Text(1 * W / 32, 6 * H / 28, "NO", 2);

		skp->Text(9 * W / 64, 5 * H / 28, "GETTR", 5);
		skp->Text(9 * W / 64, 6 * H / 28, "HH:MM:SS", 8);

		skp->Text(19 * W / 64, 5 * H / 28, "GETMR", 5);
		skp->Text(19 * W / 64, 6 * H / 28, "HH:MM:SS", 8);

		skp->Text(27 * W / 64, 5 * H / 28, "PIT", 3);
		skp->Text(27 * W / 64, 6 * H / 28, "DEG", 3);

		skp->Text(32 * W / 64, 5 * H / 28, "YAW", 3);
		skp->Text(32 * W / 64, 6 * H / 28, "DEG", 3);

		skp->Text(39 * W / 64, 5 * H / 28, "GETTS", 5);
		skp->Text(39 * W / 64, 6 * H / 28, "HH:MM:SS", 8);

		skp->Text(49 * W / 64, 5 * H / 28, "GETMS", 5);
		skp->Text(49 * W / 64, 6 * H / 28, "HH:MM:SS", 8);

		skp->Text(56 * W / 64, 5 * H / 28, "PIT", 3);
		skp->Text(56 * W / 64, 6 * H / 28, "DEG", 3);

		skp->Text(61 * W / 64, 5 * H / 28, "YAW", 3);
		skp->Text(61 * W / 64, 6 * H / 28, "DEG", 3);

		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		for (int i = 0;i < 8;i++)
		{
			sprintf(Buffer, "%02d", GC->rtcc->EZMMTAB.data[i].REV);
			skp->Text(1 * W / 32, (i + 8) * H / 28, Buffer, strlen(Buffer));

			GET_Display(Buffer, GC->rtcc->EZMMTAB.data[i].GETTR, false);
			skp->Text(3 * W / 32, (i + 8) * H / 28, Buffer, strlen(Buffer));
			GET_Display(Buffer, GC->rtcc->EZMMTAB.data[i].GETSR, false);
			skp->Text(8 * W / 32, (i + 8) * H / 28, Buffer, strlen(Buffer));

			sprintf(Buffer, "%03.0f", GC->rtcc->EZMMTAB.data[i].theta_SR);
			skp->Text(13 * W / 32, (i + 8) * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%03.0f", GC->rtcc->EZMMTAB.data[i].psi_SR);
			skp->Text(15 * W / 32, (i + 8) * H / 28, Buffer, strlen(Buffer));

			GET_Display(Buffer, GC->rtcc->EZMMTAB.data[i].GETTS, false);
			skp->Text(35 * W / 64, (i + 8) * H / 28, Buffer, strlen(Buffer));
			GET_Display(Buffer, GC->rtcc->EZMMTAB.data[i].GETSS, false);
			skp->Text(45 * W / 64, (i + 8) * H / 28, Buffer, strlen(Buffer));

			sprintf(Buffer, "%03.0f", GC->rtcc->EZMMTAB.data[i].theta_SS);
			skp->Text(55 * W / 64, (i + 8) * H / 28, Buffer, strlen(Buffer));
			sprintf(Buffer, "%03.0f", GC->rtcc->EZMMTAB.data[i].psi_SS);
			skp->Text(59 * W / 64, (i + 8) * H / 28, Buffer, strlen(Buffer));
		}
	}
	return true;
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
		LVDCSV *lvdc = (LVDCSV*)testves->iu->GetLVDC();

		coe = GC->rtcc->TLICutoffToLVDCParameters(G->R_TLI, G->V_TLI, GC->GETbase, G->P30TIG, lvdc->TB5, lvdc->mu, lvdc->T_RG);

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

void ApolloRTCCMFD::menuP30UplinkCalc()
{
	G->P30UplinkCalc();
}

void ApolloRTCCMFD::menuP30UplinkNew()
{
	if (!G->inhibUplLOS || !G->vesselinLOS())
	{
		G->P30UplinkNew();
	}
}

void ApolloRTCCMFD::menuRetrofireEXDVUplink()
{
	if (G->vesseltype < 2)
	{
		G->RetrofireEXDVUplink();
	}
}

void ApolloRTCCMFD::menuRetrofireEXDVUplinkCalc()
{
	if (G->vesseltype < 2)
	{
		G->RetrofireEXDVUplinkCalc();
	}
}

void ApolloRTCCMFD::menuTLANDUplinkCalc()
{
	G->TLANDUplinkCalc();
}

void ApolloRTCCMFD::menuTLANDUpload()
{
	G->TLANDUplink();
}

void ApolloRTCCMFD::GET_Display(char* Buff, double time, bool DispGET) //Display a time in the format hhh:mm:ss
{
	double time2 = round(time);
	if (DispGET)
	{
		sprintf(Buff, "%03.0f:%02.0f:%02.0f GET", floor(time2 / 3600.0), floor(fmod(time2, 3600.0) / 60.0), fmod(time2, 60.0));
	}
	else
	{
		sprintf(Buff, "%03.0f:%02.0f:%02.0f", floor(time2 / 3600.0), floor(fmod(time2, 3600.0) / 60.0), fmod(time2, 60.0));
	}
	//sprintf(Buff, "%03d:%02d:%02d", hh, mm, ss);
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

void ApolloRTCCMFD::ThrusterName(char *Buff, int n)
{
	if (n == RTCC_ENGINETYPE_SPS)
	{
		sprintf(Buff, "SPS");
	}
	else if (n == RTCC_ENGINETYPE_APS)
	{
		sprintf(Buff, "APS");
	}
	else if (n == RTCC_ENGINETYPE_DPS)
	{
		sprintf(Buff, "DPS");
	}
	else if (n == RTCC_ENGINETYPE_RCSPLUS2)
	{
		sprintf(Buff, "RCS +X (2 quads)");
	}
	else if (n == RTCC_ENGINETYPE_RCSPLUS4)
	{
		sprintf(Buff, "RCS +X (4 quads)");
	}
	else if (n == RTCC_ENGINETYPE_RCSMINUS2)
	{
		sprintf(Buff, "RCS -X (2 quads)");
	}
	else if (n == RTCC_ENGINETYPE_RCSMINUS4)
	{
		sprintf(Buff, "RCS -X (4 quads)");
	}
	else if (n == RTCC_ENGINETYPE_SIVB_APS)
	{
		sprintf(Buff, "S-IVB APS");
	}
	else if (n == RTCC_ENGINETYPE_LOX_DUMP)
	{
		sprintf(Buff, "S-IVB LOX");
	}
}

void ApolloRTCCMFD::VehicleConfigName(char *Buff, int n)
{
	if (n == RTCC_CONFIG_CSM)
	{
		sprintf(Buff, "CSM");
	}
	else if (n == RTCC_CONFIG_LM)
	{
		sprintf(Buff, "LM");
	}
	else if (n == RTCC_CONFIG_CSM_LM)
	{
		sprintf(Buff, "CSM+LM");
	}
	else if (n == RTCC_CONFIG_CSM_SIVB)
	{
		sprintf(Buff, "CSM+SIVB");
	}
	else if (n == RTCC_CONFIG_LM_SIVB)
	{
		sprintf(Buff, "LM+SIVB");
	}
	else if (n == RTCC_CONFIG_CSM_LM_SIVB)
	{
		sprintf(Buff, "CSM+LM+SIVB");
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

void ApolloRTCCMFD::menuSetAGSSVPage()
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

void ApolloRTCCMFD::menuSetDescPlanCalcPage()
{
	screen = 16;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetSkylabPage()
{
	screen = 17;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetDescPlanInitPage()
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

void ApolloRTCCMFD::menuSetRTEConstraintsPage()
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

void ApolloRTCCMFD::menuSetLunarAscentPage()
{
	screen = 38;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetLMAscentPADPage()
{
	screen = 39;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetPDAPPage()
{
	screen = 40;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetFIDOOrbitDigitalsPage()
{
	screen = 41;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetMCCDisplaysPage()
{
	screen = 42;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetSpaceDigitalsPage()
{
	screen = 43;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetMPTPage()
{
	screen = 44;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetNextStationContactsPage()
{
	screen = 45;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetPredSiteAcquisitionPage()
{
	screen = 46;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetUplinkMenu()
{
	screen = 47;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetStateVectorMenu()
{
	screen = 48;
	coreButtons.SelectPage(this, screen);

	G->SVDesiredGET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->GETbase);
}

void ApolloRTCCMFD::menuSetLSUpdateMenu()
{
	screen = 49;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetLSUplinkPage()
{
	screen = 50;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetP30UplinkPage()
{
	screen = 51;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetRetrofireEXDVUplinkPage()
{
	if (G->vesseltype < 2)
	{
		screen = 52;
		coreButtons.SelectPage(this, screen);
	}
	else
	{
		screen = 61;
		coreButtons.SelectPage(this, screen);
	}
}

void ApolloRTCCMFD::menuSetREFSMMATUplinkPage()
{
	screen = 53;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetTITransferPage()
{
	screen = 54;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetSPQorDKIRTransferPage()
{
	if (screen == 3)
	{
		GC->rtcc->med_m70.Plan = 0;
	}
	else if (screen == 33)
	{
		GC->rtcc->med_m70.Plan = 1;
	}
	else
	{
		GC->rtcc->med_m70.Plan = -1;
	}

	screen = 55;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetMPTDirectInputPage()
{
	screen = 56;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetGPMTransferPage()
{
	screen = 57;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetCheckoutMonitorPage()
{
	screen = 58;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetMPTInitPage()
{
	screen = 59;
	coreButtons.SelectPage(this, screen);

	GC->mptInitError = 0;
}

void ApolloRTCCMFD::menuSetDescPlanTablePage()
{
	screen = 60;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetSunriseSunsetTablePage()
{
	screen = 62;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuSetMoonriseMoonsetTablePage()
{
	screen = 63;
	coreButtons.SelectPage(this, screen);
}

void ApolloRTCCMFD::menuVoid() {}

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
		menuSetCDHPage();
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

void ApolloRTCCMFD::menuTransferGPMToMPT()
{
	G->TransferGPMToMPT();
}

void ApolloRTCCMFD::set_getbase()
{
	if (GC->mission < 7)
	{
		GC->mission = 7;
	}
	else if (GC->mission < 17)
	{
		GC->mission++;
	}
	else
	{
		GC->mission = 0;
	}

	if (GC->mission >= 7)
	{
		GC->GETbase = LaunchMJD[GC->mission-7];
	}
}

void ApolloRTCCMFD::menuMPTDirectInputVehicle()
{
	if (GC->rtcc->med_m66.Table < 2)
	{
		GC->rtcc->med_m66.Table++;
	}
	else
	{
		GC->rtcc->med_m66.Table = 1;
	}
}

void ApolloRTCCMFD::menuMPTDirectInputOption()
{
	if (GC->rtcc->med_m66.BurnParamNo < 5)
	{
		GC->rtcc->med_m66.BurnParamNo++;
	}
	else
	{
		GC->rtcc->med_m66.BurnParamNo = 1;
	}
}

void ApolloRTCCMFD::menuMPTDirectInputDV()
{
	if (GC->rtcc->med_m66.BurnParamNo == 1)
	{
		bool MPTDirectInputDTorDVInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the DT/DV for the maneuver (Format: DT/DV=X)", MPTDirectInputDTorDVInput, 0, 20, (void*)this);
	}
	else if (GC->rtcc->med_m66.BurnParamNo >= 2 && GC->rtcc->med_m66.BurnParamNo <= 4)
	{
		bool MPTDirectInputDVInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the DV for the maneuver (Format: DVX DVY DVZ)", MPTDirectInputDVInput, 0, 20, (void*)this);
	}
}

bool MPTDirectInputDVInput(void *id, char *str, void *data)
{
	VECTOR3 DV;

	if (sscanf(str, "%lf %lf %lf", &DV.x, &DV.y, &DV.z) == 3)
	{
		((ApolloRTCCMFD*)data)->set_MPTDirectInputDV(DV*0.3048);

		return true;

	}
	return false;
}

void ApolloRTCCMFD::set_MPTDirectInputDV(VECTOR3 DV)
{
	if (GC->rtcc->med_m66.BurnParamNo == 2)
	{
		GC->rtcc->med_m40.P2_DV = DV;
	}
	else if (GC->rtcc->med_m66.BurnParamNo == 3)
	{
		GC->rtcc->med_m40.P3_DV = DV;
	}
	else if (GC->rtcc->med_m66.BurnParamNo == 4)
	{
		GC->rtcc->med_m40.P4_DV = DV;
	}
}

bool MPTDirectInputDTorDVInput(void *id, char *str, void *data)
{
	double val;

	if (sscanf(str, "DT=%lf", &val) == 1)
	{
		((ApolloRTCCMFD*)data)->set_MPTDirectInputDTorDV(val, true);

		return true;
	}
	else if (sscanf(str, "DV=%lf", &val) == 1)
	{
		((ApolloRTCCMFD*)data)->set_MPTDirectInputDTorDV(val, false);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_MPTDirectInputDTorDV(double val, bool IsDT)
{
	if (IsDT)
	{
		GC->rtcc->med_m40.P1_DT = val;
	}
	else
	{
		GC->rtcc->med_m40.P1_DV = val * 0.3048;
	}
}

void ApolloRTCCMFD::menuMPTDirectInputAtt()
{
	if (GC->rtcc->med_m66.BurnParamNo == 1)
	{
		bool MPTDirectInputAttInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the attitude for the maneuver (Format: LVLH/IMU/FDAI=X Y Z)", MPTDirectInputAttInput, 0, 20, (void*)this);
	}
}

bool MPTDirectInputAttInput(void *id, char *str, void *data)
{
	VECTOR3 Att;

	if (sscanf(str, "LVLH=%lf %lf %lf", &Att.x, &Att.y, &Att.z) == 3)
	{
		((ApolloRTCCMFD*)data)->set_MPTDirectInputAtt(Att*RAD, 0);
		return true;
	}
	else if (sscanf(str, "IMU=%lf %lf %lf", &Att.x, &Att.y, &Att.z) == 3)
	{
		((ApolloRTCCMFD*)data)->set_MPTDirectInputAtt(Att*RAD, 1);
		return true;
	}
	else if (sscanf(str, "FDAI=%lf %lf %lf", &Att.x, &Att.y, &Att.z) == 3)
	{
		((ApolloRTCCMFD*)data)->set_MPTDirectInputAtt(Att*RAD, 2);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_MPTDirectInputAtt(VECTOR3 Att, int mode)
{
	GC->rtcc->med_m66.Att = Att;
	GC->rtcc->med_m66.CoordInd = mode;
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

void ApolloRTCCMFD::menuCycleGMPMarkerUp()
{
	if (marker >= 7)
	{
		marker = 0;
	}
	else
	{
		marker++;
	}
}

void ApolloRTCCMFD::menuCycleGMPMarkerDown()
{
	if (marker <= 0)
	{
		marker = 7;
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
		menuCycleGMPManeuverType();
	}
	else if (marker == 1)
	{
		menuCycleGMPManeuverPoint();
	}
	else if (marker == 2)
	{
		OrbAdjGETDialogue();
	}
	else if (marker == 3)
	{
		menuCycleOrbAdjAltRef();
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

void ApolloRTCCMFD::menuCycleOrbAdjAltRef()
{
	G->OrbAdjAltRef = !G->OrbAdjAltRef;
}

void ApolloRTCCMFD::GPMPCalc()
{
	if (G->GMPManeuverCode > 0)
	{
		G->GPMPCalc();
	}
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
	G->lambertElevOpt = 0;
	this->G->T1 = t1;
}

void ApolloRTCCMFD::set_t1_PDI(double dt)
{
	G->lambertElevOpt = 0;
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
	else if (strcmp(str, "PeT") == 0)
	{
		double pet;
		pet = ((ApolloRTCCMFD*)data)->timetoperi();
		((ApolloRTCCMFD*)data)->set_SPQtime(pet);
		return true;
	}
	else if (strcmp(str, "ApT") == 0)
	{
		double pet;
		pet = ((ApolloRTCCMFD*)data)->timetoapo();
		((ApolloRTCCMFD*)data)->set_SPQtime(pet);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SPQtime(double tig)
{
	if (G->SPQMode == 0)
	{
		this->G->CSItime = tig;
	}
	else
	{
		this->G->CDHtime = tig;
	}
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
				((ApolloRTCCMFD*)data)->set_EntryDesiredInclination(-inc);
				return true;
			}
			else if (strcmp(dir, "D") == 0)
			{
				((ApolloRTCCMFD*)data)->set_EntryDesiredInclination(inc);
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
	G->EntryDesiredInclination = inc * RAD;
}

void ApolloRTCCMFD::menuSetEntryMaxInclination()
{
	bool EntryMaxInclinationInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the maximum inclination:", EntryMaxInclinationInput, 0, 20, (void*)this);
}

bool EntryMaxInclinationInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_EntryMaxInclination(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_EntryMaxInclination(double inc)
{
	GC->RTEMaxReturnInclination = inc * RAD;
}

void ApolloRTCCMFD::menuSetEntryRangeOverride()
{
	bool EntryRangeOverrideInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the entry range in NM:", EntryRangeOverrideInput, 0, 20, (void*)this);
}

bool EntryRangeOverrideInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_EntryRangeOverride(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_EntryRangeOverride(double range)
{
	GC->RTERangeOverrideNM = range;
}

void ApolloRTCCMFD::menuSetMaxReentrySpeed()
{
	bool MaxReentrySpeedInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the entry range in ft/s:", MaxReentrySpeedInput, 0, 20, (void*)this);
}

bool MaxReentrySpeedInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_MaxReentrySpeed(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_MaxReentrySpeed(double vel)
{
	G->RTEMaxReentrySpeed = vel * 0.3048;
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

void ApolloRTCCMFD::menuCycleTIThruster()
{
	if (GC->rtcc->med_m72.Thruster < 6)
	{
		GC->rtcc->med_m72.Thruster++;
	}
	else
	{
		GC->rtcc->med_m72.Thruster = 0;
	}
}

void ApolloRTCCMFD::menuCycleSPQDKIThruster()
{
	if (GC->rtcc->med_m70.Thruster < 6)
	{
		GC->rtcc->med_m70.Thruster++;
	}
	else
	{
		GC->rtcc->med_m70.Thruster = 0;
	}
}

void ApolloRTCCMFD::menuCycleMPTDirectInputThruster()
{
	if (GC->rtcc->med_m66.Thruster < 8)
	{
		GC->rtcc->med_m66.Thruster++;
	}
	else
	{
		GC->rtcc->med_m66.Thruster = 0;
	}
}

void ApolloRTCCMFD::menuCycleGPMThruster()
{
	if (GC->rtcc->med_m65.Thruster < 6)
	{
		GC->rtcc->med_m65.Thruster++;
	}
	else
	{
		GC->rtcc->med_m65.Thruster = 0;
	}
}

void ApolloRTCCMFD::menuMPTDirectInputTransfer()
{
	G->MPTDirectInputCalc();
}

void ApolloRTCCMFD::menuTransferTIToMPT()
{
	G->TransferTIToMPT();
}

void ApolloRTCCMFD::menuMPTDirectInputTIG()
{
	bool MPTDirectInputTIGInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET (Format: hhh:mm:ss)", MPTDirectInputTIGInput, 0, 20, (void*)this);
}

bool MPTDirectInputTIGInput(void *id, char *str, void *data)
{
	int hh, mm, ss;
	double tig;
	
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		tig = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_MPTDirectInputTIG(tig);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_MPTDirectInputTIG(double tig)
{
	GC->rtcc->med_m66.GETBI = tig;
}

void ApolloRTCCMFD::menuMPTDirectInputCode()
{
	bool MPTDirectInputCodeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Code for Maneuver:", MPTDirectInputCodeInput, 0, 20, (void*)this);
}

bool MPTDirectInputCodeInput(void *id, char *str, void *data)
{
	((ApolloRTCCMFD*)data)->set_MPTDirectInputCode(str);

	return true;
}

void ApolloRTCCMFD::set_MPTDirectInputCode(const char *code)
{
	GC->rtcc->med_m66.code.assign(code);
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
	if (GC->rtcc->med_m66.FinalConfig < 5)
	{
		GC->rtcc->med_m66.FinalConfig++;
	}
	else
	{
		GC->rtcc->med_m66.FinalConfig = 0;
	}
}

void ApolloRTCCMFD::menuTransferPoweredAscentToMPT()
{
	G->TransferPoweredAscentToMPT();
}

void ApolloRTCCMFD::menuTransferPoweredDescentToMPT()
{
	G->TransferPoweredDescentToMPT();
}

void ApolloRTCCMFD::menuMPTInitM50M55Table()
{
	if (GC->rtcc->med_m50.Table == 1)
	{
		GC->rtcc->med_m50.Table = 2;
		GC->rtcc->med_m55.Table = 2;
	}
	else
	{
		GC->rtcc->med_m50.Table = 1;
		GC->rtcc->med_m55.Table = 1;
	}
}

void ApolloRTCCMFD::menuMPTInitM50CSMWT()
{

}

void ApolloRTCCMFD::menuMPTInitM50LMWT()
{

}

void ApolloRTCCMFD::menuMPTInitM50SIVBWT()
{

}

void ApolloRTCCMFD::menuMPTInitM55Config()
{
	if (GC->rtcc->med_m55.ConfigCode < RTCC_CONFIG_CSM_LM_SIVB)
	{
		GC->rtcc->med_m55.ConfigCode++;
	}
	else
	{
		GC->rtcc->med_m55.ConfigCode = 0;
	}
}

void ApolloRTCCMFD::menuMPTM50Update()
{
	if (GC->rtcc->PMMWTC(GC->mptable, 50))
	{
		GC->mptInitError = 2;
	}
	else
	{
		GC->mptInitError = 1;
	}
}

void ApolloRTCCMFD::menuMPTM55Update()
{
	if (GC->rtcc->PMMWTC(GC->mptable, 55))
	{
		GC->mptInitError = 4;
	}
	else
	{
		GC->mptInitError = 3;
	}
}

void ApolloRTCCMFD::menuMPTTrajectoryUpdate()
{
	if (GC->MPTTrajectoryUpdate())
	{
		GC->mptInitError = 6;
	}
	else
	{
		GC->mptInitError = 5;
	}
}

void ApolloRTCCMFD::menuMPTInitAutoUpdate()
{
	GC->MPTMassUpdate();
}

void ApolloRTCCMFD::menuMPTInitM50M55Vehicle()
{
	int vesselcount;

	vesselcount = oapiGetVesselCount();

	if (GC->rtcc->med_m50.Table == 1)
	{
		if (GC->pLMnumber < vesselcount - 1)
		{
			GC->pLMnumber++;
		}
		else
		{
			GC->pLMnumber = 0;
		}

		GC->pLM = oapiGetVesselInterface(oapiGetVesselByIndex(GC->pLMnumber));
	}
	else
	{
		if (GC->pCSMnumber < vesselcount - 1)
		{
			GC->pCSMnumber++;
		}
		else
		{
			GC->pCSMnumber = 0;
		}

		GC->pCSM = oapiGetVesselInterface(oapiGetVesselByIndex(GC->pCSMnumber));
	}
}

void ApolloRTCCMFD::CheckoutMonitorCalc()
{
	G->CheckoutMonitorCalc();
}

void ApolloRTCCMFD::menuCheckMonVehID()
{
	if (GC->rtcc->med_u02.VEH < 2)
	{
		GC->rtcc->med_u02.VEH++;
		sprintf(GC->checkmon.VEH, "CSM");
	}
	else
	{
		GC->rtcc->med_u02.VEH = 1;
		sprintf(GC->checkmon.VEH, "LEM");
	}
}

void ApolloRTCCMFD::menuCheckMonOptionID()
{
	if (GC->rtcc->med_u02.IND < 4)
	{
		GC->rtcc->med_u02.IND++;
	}
	else
	{
		GC->rtcc->med_u02.IND = 1;
	}

	if (GC->rtcc->med_u02.IND == 1)
	{
		sprintf(GC->checkmon.Option, "GMT");
	}
	else if (GC->rtcc->med_u02.IND == 2)
	{
		sprintf(GC->checkmon.Option, "GET");
	}
	else if (GC->rtcc->med_u02.IND == 3)
	{
		sprintf(GC->checkmon.Option, "MVI");
	}
	else if (GC->rtcc->med_u02.IND == 4)
	{
		sprintf(GC->checkmon.Option, "MVE");
	}
}

void ApolloRTCCMFD::menuCheckMonReference()
{
	if (GC->rtcc->med_u02.REF < 3)
	{
		GC->rtcc->med_u02.REF++;
	}
	else
	{
		GC->rtcc->med_u02.REF = 0;
	}

	if (GC->rtcc->med_u02.REF == 0)
	{
		sprintf(GC->checkmon.RF, "ECI");
	}
	else if (GC->rtcc->med_u02.REF == 1)
	{
		sprintf(GC->checkmon.RF, "ECT");
	}
	else if (GC->rtcc->med_u02.REF == 2)
	{
		sprintf(GC->checkmon.RF, "MCI");
	}
	else if (GC->rtcc->med_u02.REF == 3)
	{
		sprintf(GC->checkmon.RF, "MCT");
	}
}

void ApolloRTCCMFD::menuCheckMonFeet()
{
	if (GC->rtcc->med_u02.FT < 1)
	{
		GC->rtcc->med_u02.FT = 1;
		GC->checkmon.unit = 1;
	}
	else
	{
		GC->rtcc->med_u02.FT = 0;
		GC->checkmon.unit = 0;
	}
}

void ApolloRTCCMFD::menuCheckMonParameter()
{
	if (GC->rtcc->med_u02.IND == 1 || GC->rtcc->med_u02.IND == 2)
	{
		bool CheckMonTimeInput(void* id, char *str, void *data);
		oapiOpenInputBox("Input GET or GMT in HH:MM:SS.SS", CheckMonTimeInput, 0, 20, (void*)this);
	}
	else
	{
		bool CheckMonManInput(void* id, char *str, void *data);
		oapiOpenInputBox("Input Maneuver ID:", CheckMonManInput, 0, 20, (void*)this);
	}
}

bool CheckMonTimeInput(void* id, char *str, void *data)
{
	int hh, mm;
	double ss, tig;

	if (sscanf(str, "%d:%d:%lf", &hh, &mm, &ss) == 3)
	{
		tig = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_CheckMonTime(tig);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_CheckMonTime(double tig)
{
	GC->rtcc->med_u02.IND_val = tig;

	if (GC->rtcc->med_u02.IND == 1)
	{
		GC->checkmon.GMT = tig;
	}
	else
	{
		GC->checkmon.GET = tig;
	}
}

bool CheckMonManInput(void* id, char *str, void *data)
{
	int man;

	if (sscanf(str, "%d", &man) == 1)
	{
		((ApolloRTCCMFD*)data)->set_CheckMonMan(man);

		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_CheckMonMan(int man)
{
	GC->rtcc->med_u02.IND_man = man;
}

void ApolloRTCCMFD::menuCheckMonThresholdTime()
{

}

double ApolloRTCCMFD::timetoperi()
{
	VECTOR3 R, V;
	double mu,pet, mjd;
	OBJHANDLE gravref = GC->rtcc->AGCGravityRef(G->vessel);

	G->vessel->GetRelativePos(gravref, R);
	G->vessel->GetRelativeVel(gravref, V);
	mu = GGRAV*oapiGetMass(gravref);
	pet = OrbMech::timetoperi(R, V, mu);
	mjd = oapiTime2MJD(oapiGetSimTime() + pet);
	return (mjd - GC->GETbase)*24.0*3600.0;
}

double ApolloRTCCMFD::timetoapo()
{
	VECTOR3 R, V;
	double mu, pet, mjd;
	OBJHANDLE gravref = GC->rtcc->AGCGravityRef(G->vessel);

	G->vessel->GetRelativePos(gravref, R);
	G->vessel->GetRelativeVel(gravref, V);
	mu = GGRAV*oapiGetMass(gravref);
	pet = OrbMech::timetoapo(R, V, mu);
	mjd = oapiTime2MJD(oapiGetSimTime() + pet);
	return (mjd - GC->GETbase)*24.0*3600.0;
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
	G->lambertElevOpt = 1;
}

void ApolloRTCCMFD::calcREFSMMAT()
{
	G->REFSMMATCalc();
}

void ApolloRTCCMFD::menuSendREFSMMATToOtherVessel()
{
	if (G->target != NULL)
	{
		OBJHANDLE itertarget;
		ARCore *core;

		for (int i = 0;i < nGutsUsed;i++)
		{
			itertarget = GCoreVessel[i];

			if (G->target == itertarget)
			{
				core = GCoreData[i];

				core->REFSMMAT = G->REFSMMAT;
				core->REFSMMATcur = G->REFSMMATcur;
				core->REFSMMATopt = G->REFSMMATopt;

				for (int i = 0;i < 20;i++)
				{
					core->REFSMMAToct[i] = G->REFSMMAToct[i];
				}

				core->REFSMMAToct[1] = core->REFSMMATUplinkAddress();

				return;
			}
		}
	}
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
	this->GC->LSLat = lat*RAD;
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
	this->GC->LSLng = lng*RAD;
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
		G->GMPManeuverCode == RTCC_GMP_PHL || G->GMPManeuverCode == RTCC_GMP_PHT || G->GMPManeuverCode == RTCC_GMP_PHA || G->GMPManeuverCode == RTCC_GMP_PHP)
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
		G->GMPManeuverCode == RTCC_GMP_PHL || G->GMPManeuverCode == RTCC_GMP_PHT || G->GMPManeuverCode == RTCC_GMP_PHA || G->GMPManeuverCode == RTCC_GMP_PHP)
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
		G->GMPManeuverCode == RTCC_GMP_CNL || G->GMPManeuverCode == RTCC_GMP_CNH || G->GMPManeuverCode == RTCC_GMP_CNT || 
		G->GMPManeuverCode == RTCC_GMP_CNA || G->GMPManeuverCode == RTCC_GMP_CNP)
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
		G->GMPManeuverCode == RTCC_GMP_CNL || G->GMPManeuverCode == RTCC_GMP_CNH || G->GMPManeuverCode == RTCC_GMP_CNT ||
		G->GMPManeuverCode == RTCC_GMP_CNA || G->GMPManeuverCode == RTCC_GMP_CNP)
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
	bool SVGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for the state vector (Format: hhh:mm:ss)", SVGETInput, 0, 20, (void*)this);
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
	G->SVDesiredGET = SVtime;
}

void ApolloRTCCMFD::menuSetAGSKFactor()
{
	bool AGSKFactorInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the AGS time bias (Format: hhh:mm:ss)", AGSKFactorInput, 0, 20, (void*)this);
}

bool AGSKFactorInput(void *id, char *str, void *data)
{
	int hh, mm;
	double ss, get;

	if (sscanf(str, "%d:%d:%lf", &hh, &mm, &ss) == 3)
	{
		get = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_AGSKFactor(get);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_AGSKFactor(double time)
{
	G->AGSKFactor = time;
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
	if (sscanf(str, "DT=%lf%s", &t2time, &uni) == 2)
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
	else if (sscanf(str, "WT=%lf", &t2time) == 1)
	{
		((ApolloRTCCMFD*)data)->set_lambertWT(t2time);
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
		G->lambertTPFOpt = 0;
	}
	else
	{
		this->G->lambertDT = t2;
		G->lambertTPFOpt = 1;
	}
}

void ApolloRTCCMFD::set_lambertWT(double wt)
{
	G->lambertWT = wt * RAD;
	G->lambertTPFOpt = 2;
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

void ApolloRTCCMFD::menuMoonRTECalc()
{
	G->TLCCSolGood = true;
	G->MoonRTECalc();
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

void ApolloRTCCMFD::menuTransferRTEToMPT()
{
	G->TransferRTEToMPT();
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
		((ApolloRTCCMFD*)data)->set_entryrange(atof(str));
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
		if (!G->svtarget->GroundContact())
		{
			G->StateVectorCalc();
		}
	}
}


void ApolloRTCCMFD::menuAGSSVCalc()
{
	if (G->svtarget != NULL)
	{
		G->AGSStateVectorCalc();
	}
}

void ApolloRTCCMFD::menuLSCalc()
{
	if (G->svtarget != NULL)
	{
		G->LandingSiteUpdate();
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

void ApolloRTCCMFD::menuLSUplinkCalc()
{
	G->LSUplinkCalc();
}

void ApolloRTCCMFD::menuLSUpload()
{
	if (!G->inhibUplLOS || !G->vesselinLOS())
	{
		G->LandingSiteUplink();
	}
}

void ApolloRTCCMFD::menuREFSMMATUplinkCalc()
{
	G->REFSMMATUplinkCalc();
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
			G->TLI_PAD();
		}
		else
		{
			G->PDI_PAD();
		}
	}
}

void ApolloRTCCMFD::menuCalcEntryPAD()
{
	G->EntryPAD();
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

void ApolloRTCCMFD::menuSwitchEntryPADOpt()
{
	if (G->entrypadopt < 1)
	{
		G->entrypadopt++;
	}
	else
	{
		G->entrypadopt = 0;
	}
}

void ApolloRTCCMFD::menuSwitchManPADEngine()
{
	if (G->manpadopt == 0)
	{
		if (G->vesseltype < 2)
		{
			if (G->csmenginetype < 4)
			{
				G->csmenginetype++;
			}
			else
			{
				G->csmenginetype = 0;
			}
		}
		else
		{
			if (G->lemenginetype < 5)
			{
				G->lemenginetype++;
			}
			else
			{
				G->lemenginetype = 0;
			}
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
	bool MapUpdateGETInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for the anchor vector (Format: hhh:mm:ss)", MapUpdateGETInput, 0, 20, (void*)this);
}

bool MapUpdateGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_MapUpdateGET(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_MapUpdateGET(double time)
{
	G->mapUpdateGET = time;
}

void ApolloRTCCMFD::menuSwitchUplinkInhibit()
{
	G->inhibUplLOS = !G->inhibUplLOS;
}

void ApolloRTCCMFD::menuCycleSPQMode()
{
	if (G->SPQMode < 1)
	{
		G->SPQMode++;
	}
	else
	{
		G->SPQMode = 0;
	}
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
	if (GC->mission == 0)
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
	this->GC->GETbase = mjd;
}

void ApolloRTCCMFD::menuSetAGCEpoch()
{
	if (GC->mission == 0)
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

	if (GC->mission < 11)		//NBY 1968/1969
	{
		TEPHEM0 = 40038.;
	}
	else if (GC->mission < 14)	//NBY 1969/1970
	{
		TEPHEM0 = 40403.;
	}
	else if (GC->mission < 15)	//NBY 1970/1971
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
		GC->GETbase = (tephem / 8640000.) + TEPHEM0;
	}
	else
	{
		lem = (LEM *)G->vessel;

		double tephem = lem->agc.vagc.Erasable[0][01710] +
			lem->agc.vagc.Erasable[0][01707] * pow((double) 2., (double) 14.) +
			lem->agc.vagc.Erasable[0][01706] * pow((double) 2., (double) 28.);
		GC->GETbase = (tephem / 8640000.) + TEPHEM0;
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

	REFSMMATaddress = G->REFSMMATOctalAddress();

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

	//sprintf(oapiDebugString(), "%f, %f, %f, %f, %f, %f, %f, %f, %f", G->REFSMMAT.m11, G->REFSMMAT.m12, G->REFSMMAT.m13, G->REFSMMAT.m21, G->REFSMMAT.m22, G->REFSMMAT.m23, G->REFSMMAT.m31, G->REFSMMAT.m32, G->REFSMMAT.m33);

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
	if (G->csmenginetype < 4)
	{
		G->csmenginetype++;
	}
	else
	{
		G->csmenginetype = 0;
	}
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

void ApolloRTCCMFD::EntryLongitudeModeDialogue()
{
	G->entrylongmanual = !G->entrylongmanual;
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

void ApolloRTCCMFD::menuCycleLOIEllipseOption()
{
	if (GC->LOIEllipseRotation < 2)
	{
		GC->LOIEllipseRotation++;
	}
	else
	{
		GC->LOIEllipseRotation = 0;
	}
}

void ApolloRTCCMFD::menuTransferLOItoMPT()
{
	G->MPT_LOI_TLMCC_Flag = true;
	G->TransferLOIorMCCtoMPT();
}

void ApolloRTCCMFD::menuTransferTLCCtoMPT()
{
	G->MPT_LOI_TLMCC_Flag = false;
	G->TransferLOIorMCCtoMPT();
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
		GC->TLCCNodeGET = time;
	}
	else
	{
		GC->TLCCPeriGET = time;
	}
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
	GC->t_Land = time;
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
		GC->TLCCNodeLat = lat*RAD;
	}
	else if (G->TLCCmaneuver == 5 || G->TLCCmaneuver == 6)
	{
		GC->TLCCNonFreeReturnEMPLat = lat*RAD;
	}
	else
	{
		GC->TLCCFreeReturnEMPLat = lat*RAD;
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
	GC->TLCCNodeLng = lng*RAD;
}

void ApolloRTCCMFD::menuSetLOIApo()
{
	bool LOIApoInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the apocynthion altitude:", LOIApoInput, 0, 20, (void*)this);
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
	GC->LOIapo = alt*1852.0;
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
	GC->LOIperi = alt*1852.0;
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
		this->GC->TLCCNodeAlt = alt*1852.0;
	}
	else if (G->TLCCmaneuver == 1 || G->TLCCmaneuver == 7 || G->TLCCmaneuver == 8)
	{
		this->GC->TLCCFlybyPeriAlt = alt*1852.0;
	}
	else
	{
		this->GC->TLCCLAHPeriAlt = alt*1852.0;
	}
}

void ApolloRTCCMFD::menuSetLOIAzi()
{
	if (G->LOIOption == 0)
	{
		bool LOIAziInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the approach azimuth:", LOIAziInput, 0, 20, (void*)this);
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
	this->GC->LOIazi = azi*RAD;
}

void ApolloRTCCMFD::menuSetLOIGET()
{
	menuSetTLAND();
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

void ApolloRTCCMFD::menuSetLDPPVectorTime()
{
	if (GC->MissionPlanningActive)
	{
		bool LDPPVectorTimeInput(void *id, char *str, void *data);
		oapiOpenInputBox("Choose the MPT vector time (Format: hhh:mm:ss)", LDPPVectorTimeInput, 0, 20, (void*)this);
	}
}

bool LDPPVectorTimeInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_LDPPVectorTime(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LDPPVectorTime(double time)
{
	GC->rtcc->med_k16.VectorTime = time;
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
	GC->LSAlt = alt*1852.0;
}

void ApolloRTCCMFD::menuSetLDPPDwellOrbits()
{
	bool LDPPDwellOrbitsInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the number of revolutions:", LDPPDwellOrbitsInput, 0, 20, (void*)this);
}

bool LDPPDwellOrbitsInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LDPPDwellOrbits(atoi(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LDPPDwellOrbits(int N)
{
	GC->rtcc->med_k17.DwellOrbits = N;
}

void ApolloRTCCMFD::menuSetLDPPDescentFlightArc()
{
	bool LDPPDescentFlightArcInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the angle from perilune to landing site:", LDPPDescentFlightArcInput, 0, 20, (void*)this);
}

bool LDPPDescentFlightArcInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LDPPDescentFlightArc(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LDPPDescentFlightArc(double ang)
{
	GC->rtcc->med_k17.DescentFlightArc = ang*RAD;
}

void ApolloRTCCMFD::menuSetLDPPDescIgnHeight()
{
	bool LDPPDescIgnHeightInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the perilune altitude above the landing site:", LDPPDescIgnHeightInput, 0, 20, (void*)this);
}

bool LDPPDescIgnHeightInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LDPPDescIgnHeight(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LDPPDescIgnHeight(double alt)
{
	GC->rtcc->med_k17.DescIgnHeight = alt * 0.3048;
}

void ApolloRTCCMFD::cycleLDPPPoweredDescSimFlag()
{
	GC->rtcc->med_k17.PoweredDescSimFlag = !GC->rtcc->med_k17.PoweredDescSimFlag;
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

void ApolloRTCCMFD::menuLDPPCalc()
{
	G->LDPPalc();
	menuSetDescPlanTablePage();
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
		OBJHANDLE gravref = GC->rtcc->AGCGravityRef(G->vessel);

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
		G->t_TPI = dt1 + (SVMJD - GC->GETbase) * 24.0 * 60.0 * 60.0;
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

void ApolloRTCCMFD::menuSetLiftoffguess()
{
	bool LiftoffGuessInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET for liftoff (Format: hhh:mm:ss)", LiftoffGuessInput, 0, 20, (void*)this);
}

bool LiftoffGuessInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_Liftoffguess(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::menuCycleLunarLiftoffInsVelOption()
{
	G->LunarLiftoffInsVelInput = !G->LunarLiftoffInsVelInput;
}

void ApolloRTCCMFD::set_Liftoffguess(double time)
{
	G->t_Liftoff_guess = time;
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
	if (G->target != NULL && G->vesseltype > 1)
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
	GC->DT_Ins_TPI = dt * 60.0;
}

void ApolloRTCCMFD::menuLunarLiftoffVHorInput()
{
	if (G->LunarLiftoffInsVelInput && G->LunarLiftoffTimeOption == 0)
	{
		bool LunarLiftoffVHorInput(void* id, char *str, void *data);
		oapiOpenInputBox("Input horizontal velocity in ft/s:", LunarLiftoffVHorInput, 0, 20, (void*)this);
	}
}

bool LunarLiftoffVHorInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_LunarLiftoffVHorInput(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LunarLiftoffVHorInput(double v_lh)
{
	G->LunarLiftoffRes.v_LH = v_lh * 0.3048;
}

void ApolloRTCCMFD::menuLunarLiftoffVVertInput()
{
	if (G->LunarLiftoffInsVelInput && G->LunarLiftoffTimeOption == 0)
	{
		bool LunarLiftoffVVertInput(void* id, char *str, void *data);
		oapiOpenInputBox("Input vertical velocity in ft/s:", LunarLiftoffVVertInput, 0, 20, (void*)this);
	}
}

bool LunarLiftoffVVertInput(void *id, char *str, void *data)
{
	if (strlen(str) < 20)
	{
		((ApolloRTCCMFD*)data)->set_LunarLiftoffVVertInput(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LunarLiftoffVVertInput(double v_lv)
{
	G->LunarLiftoffRes.v_LV = v_lv * 0.3048;
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

void ApolloRTCCMFD::menuLAPCalc()
{
	if (GC->MissionPlanningActive || (G->target != NULL && G->vesseltype > 1))
	{
		G->LAPCalc();
	}
}

void ApolloRTCCMFD::menuSetLAPLiftoffTime()
{
	bool LAPLiftoffTimeInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the liftoff time (Format: hhh:mm:ss)", LAPLiftoffTimeInput, 0, 20, (void*)this);
}

bool LAPLiftoffTimeInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "PDI+%d:%d", &mm, &ss) == 2)
	{
		((ApolloRTCCMFD*)data)->set_LAPLiftoffTime_DT_PDI((double)mm * 60.0 + (double)ss);
		return true;
	}
	else if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_LAPLiftoffTime(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LAPLiftoffTime(double time)
{
	G->LunarLiftoffRes.t_L = time;
}

void ApolloRTCCMFD::set_LAPLiftoffTime_DT_PDI(double dt)
{
	G->LunarLiftoffRes.t_L = G->pdipad.GETI + dt;
}

void ApolloRTCCMFD::menuSetLAPHorVelocity()
{
	bool LAPHorVelocityInput(void* id, char *str, void *data);
	oapiOpenInputBox("Horizontal velocity in ft/s:", LAPHorVelocityInput, 0, 20, (void*)this);
}

bool LAPHorVelocityInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LAPHorVelocity(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LAPHorVelocity(double vel)
{
	this->G->LunarLiftoffRes.v_LH = vel * 0.3048;
}

void ApolloRTCCMFD::menuSetLAPVerVelocity()
{
	bool LAPVerVelocityInput(void* id, char *str, void *data);
	oapiOpenInputBox("Vertical velocity in ft/s:", LAPVerVelocityInput, 0, 20, (void*)this);
}

bool LAPVerVelocityInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_LAPVerVelocity(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LAPVerVelocity(double vel)
{
	this->G->LunarLiftoffRes.v_LV = vel * 0.3048;
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
	else if (strcmp(str, "ApT") == 0)
	{
		double apt;
		apt = ((ApolloRTCCMFD*)data)->timetoapo();
		((ApolloRTCCMFD*)data)->set_DKITIG(apt);
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
	if (G->DKI_Profile < 4)
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

void ApolloRTCCMFD::DKINHCDialogue()
{
	bool DKINHCInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the number of half-revs between CSI and CDH", DKINHCInput, 0, 20, (void*)this);
}

bool DKINHCInput(void *id, char *str, void *data)
{
	int N;
	if (sscanf(str, "%d", &N) == 1)
	{
		((ApolloRTCCMFD*)data)->set_DKINHC(N);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_DKINHC(int N)
{
	G->DKI_N_HC = N;
}

void ApolloRTCCMFD::DKINPBDialogue()
{
	bool DKINPBInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the number of half-revs between Phasing and Boost", DKINPBInput, 0, 20, (void*)this);
}

bool DKINPBInput(void *id, char *str, void *data)
{
	int N;
	if (sscanf(str, "%d", &N) == 1)
	{
		((ApolloRTCCMFD*)data)->set_DKINPB(N);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_DKINPB(int N)
{
	G->DKI_N_PB = N;
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

void ApolloRTCCMFD::menuCycleAGCEphemOpt()
{
	if (G->AGCEphemOption < 1)
	{
		G->AGCEphemOption++;
	}
	else
	{
		G->AGCEphemOption = 0;
	}
}

void ApolloRTCCMFD::menuCycleAGCEphemAGCType()
{
	G->AGCEphemIsCMC = !G->AGCEphemIsCMC;
}

void ApolloRTCCMFD::menuSetAGCEphemMission()
{
	bool AGCEphemMissionInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the mission number:", AGCEphemMissionInput, 0, 20, (void*)this);
}

bool AGCEphemMissionInput(void *id, char *str, void *data)
{
	int N;
	if (sscanf(str, "%d", &N) == 1)
	{
		((ApolloRTCCMFD*)data)->set_AGCEphemMission(N);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_AGCEphemMission(int ApolloNo)
{
	G->AGCEphemMission = ApolloNo;
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

void ApolloRTCCMFD::menuSetAGCEphemTEPHEM()
{
	bool AGCEphemTEPHEMInput(void* id, char *str, void *data);
	oapiOpenInputBox("MJD of launch:", AGCEphemTEPHEMInput, 0, 20, (void*)this);
}

bool AGCEphemTEPHEMInput(void *id, char *str, void *data)
{
	if (strlen(str)<20)
	{
		((ApolloRTCCMFD*)data)->set_AGCEphemTEPHEM(atof(str));
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_AGCEphemTEPHEM(double mjd)
{
	this->G->AGCEphemTEPHEM = mjd;
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


void ApolloRTCCMFD::menuSetAGCEphemTLAND()
{
	bool AGCEphemTLANDInput(void *id, char *str, void *data);
	oapiOpenInputBox("Choose the GET of lunar landing (Format: hhh:mm:ss)", AGCEphemTLANDInput, 0, 20, (void*)this);
}

bool AGCEphemTLANDInput(void *id, char *str, void *data)
{
	int hh, mm, ss, t1time;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		t1time = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_AGCEphemTLAND(t1time);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_AGCEphemTLAND(double get)
{
	G->AGCEphemTLAND = get;
}

void ApolloRTCCMFD::menuGenerateAGCEphemeris()
{
	if (G->AGCEphemOption == 0)
	{
		G->GenerateAGCEphemeris();
	}
	else
	{
		G->GenerateAGCCorrectionVectors();
	}
}

void ApolloRTCCMFD::menuAscentPADCalc()
{
	if (G->vesseltype > 1 && G->vessel->GroundContact() && G->target != NULL)
	{
		G->AscentPADCalc();
	}
}

void ApolloRTCCMFD::menuPDAPCalc()
{
	if (G->vesseltype > 1 && G->target != NULL)
	{
		G->PDAPCalc();
	}
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
	if (G->vesseltype > 1 && GC->mission == 11)
	{
		G->AP11AbortCoefUplink();
	}
}

void ApolloRTCCMFD::menuUpdateFIDOOrbitDigitals()
{
	G->UpdateFIDOOrbitDigitals();
}

void ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETL()
{
	bool FIDOOrbitDigitalsGETLInput(void* id, char *str, void *data);
	oapiOpenInputBox("GET of longitude (Format: hhh:mm:ss)", FIDOOrbitDigitalsGETLInput, 0, 20, (void*)this);
}

bool FIDOOrbitDigitalsGETLInput(void *id, char *str, void *data)
{
	int hh, mm, ss, getl;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		getl = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_FIDOOrbitDigitalsGETL(getl);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_FIDOOrbitDigitalsGETL(double getl)
{
	G->fidoorbit.GETL = getl;
	G->FIDOOrbitDigitalsCalculateLongitude();
}

void ApolloRTCCMFD::menuSetFIDOOrbitDigitalsL()
{
	bool FIDOOrbitDigitalsLInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: Revolution,desired longitude (Rev 0 to bypass check)", FIDOOrbitDigitalsLInput, 0, 20, (void*)this);
}

bool FIDOOrbitDigitalsLInput(void *id, char *str, void *data)
{
	int rev;
	double lng;

	if (sscanf(str, "%d,%lf", &rev, &lng) == 2)
	{
		((ApolloRTCCMFD*)data)->set_FIDOOrbitDigitalsL(rev, lng);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_FIDOOrbitDigitalsL(int rev, double lng)
{
	this->G->fidoorbit.REVL = rev;
	this->G->fidoorbit.L = lng;
	G->FIDOOrbitDigitalsCalculateGETL();
}

void ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETBV()
{
	bool FIDOOrbitDigitalsGETBVInput(void* id, char *str, void *data);
	oapiOpenInputBox("GET of requested vector (Format: hhh:mm:ss)", FIDOOrbitDigitalsGETBVInput, 0, 20, (void*)this);
}

bool FIDOOrbitDigitalsGETBVInput(void *id, char *str, void *data)
{
	int hh, mm, ss, getl;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		getl = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_FIDOOrbitDigitalsGETBV(getl);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_FIDOOrbitDigitalsGETBV(double getbv)
{
	G->fidoorbit.GETBV = getbv;
	G->FIDOOrbitDigitalsApoPeriRequest();
}

void ApolloRTCCMFD::menuSpaceDigitalsInit()
{
	bool SpaceDigitalsInitInput(void* id, char *str, void *data);
	oapiOpenInputBox("Initialization (Format: CSM or LEM,Earth or Moon", SpaceDigitalsInitInput, 0, 20, (void*)this);
}

bool SpaceDigitalsInitInput(void *id, char *str, void *data)
{
	char VEH[16], REF[16];
	int veh, ref;
	if (sscanf(str, " %[^,],%s", &VEH, &REF) == 2)
	{
		if (strcmp(VEH, "CSM") == 0)
		{
			veh = 2;
		}
		else if (strcmp(VEH, "LEM") == 0)
		{
			veh = 1;
		}
		else return false;

		if (strcmp(REF, "Earth") == 0)
		{
			ref = BODY_EARTH;
		}
		else if (strcmp(REF, "Moon") == 0)
		{
			ref = BODY_MOON;
		}
		else return false;

		((ApolloRTCCMFD*)data)->set_SpaceDigitalsInit(veh, ref);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SpaceDigitalsInit(int veh, int ref)
{
	GC->rtcc->med_u00.VEH = veh;
	GC->rtcc->med_u00.REF = ref;

	G->SpaceDigitalsInit();
}

void ApolloRTCCMFD::menuGenerateSpaceDigitals()
{
	bool GenerateSpaceDigitalsInput(void* id, char *str, void *data);
	oapiOpenInputBox("Generate Space Digitals (Format: Manual Col (1-3),Option Ind (GET or MNV),Parameter)", GenerateSpaceDigitalsInput, 0, 20, (void*)this);
}

bool GenerateSpaceDigitalsInput(void *id, char *str, void *data)
{
	char OptionInd[16];
	int ManualCol, hh, mm;
	int Mnv = 0;
	double ss, get;

	if (sscanf(str, "%d,%[^,],%d:%d:%lf", &ManualCol, &OptionInd, &hh, &mm, &ss) == 5)
	{
		get = ss + 60 * (mm + 60 * hh);

		if (strcmp(OptionInd, "GET")) return false;

		((ApolloRTCCMFD*)data)->set_GenerateSpaceDigitals(0, ManualCol, get, 0);
		return true;
	}
	else if (sscanf(str, "%d,%[^,],%d", &ManualCol, &OptionInd, &Mnv) == 3)
	{
		if (ManualCol < 1 || ManualCol > 3) return false;
		if (strcmp(OptionInd, "MNV")) return false;

		((ApolloRTCCMFD*)data)->set_GenerateSpaceDigitals(1, ManualCol, 0.0, Mnv);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_GenerateSpaceDigitals(int option, int column, double get, int mnv)
{
	GC->rtcc->med_u01.GET = get;
	GC->rtcc->med_u01.ManualCol = column;
	GC->rtcc->med_u01.MNV = mnv;
	GC->rtcc->med_u01.OptionInd = option;

	G->SpaceDigitalsGET();
}

void ApolloRTCCMFD::menuMPTCycleActive()
{
	GC->MissionPlanningActive = !GC->MissionPlanningActive;
}

void ApolloRTCCMFD::menuMPTDeleteManeuver()
{
	GC->rtcc->MPTDeleteManeuver(GC->mptable);
}

void ApolloRTCCMFD::menuMPTCopyEphemeris()
{
	bool MPTCopyEphemerisInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: OldVeh;NewVeh;GET;ManNum (LEM = 1, CSM = 2)", MPTCopyEphemerisInput, 0, 20, (void*)this);
}

bool MPTCopyEphemerisInput(void* id, char *str, void *data)
{
	int OldVeh, NewVeh, ManNum, hh, mm;
	double ss;

	if (sscanf(str, "%d;%d;;%d", &OldVeh, &NewVeh, &ManNum) == 3)
	{
		((ApolloRTCCMFD*)data)->set_MPTCopyEphemeris(OldVeh, NewVeh, 0, ManNum);
		return true;
	}
	else if (sscanf(str, "%d;%d;%d:%d:%lf;", &OldVeh, &NewVeh, &hh, &mm, &ss) == 5)
	{
		((ApolloRTCCMFD*)data)->set_MPTCopyEphemeris(OldVeh, NewVeh, hh*3600.0 + mm * 60.0 + ss, 0);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_MPTCopyEphemeris(int OldVeh, int NewVeh, double GET, int ManNum)
{
	GC->rtcc->med_p16.OldVeh = OldVeh;
	GC->rtcc->med_p16.NewVeh = NewVeh;
	GC->rtcc->med_p16.GMT = GET;
	GC->rtcc->med_p16.ManNum = ManNum;

	GC->rtcc->MPTCopyEphemeris(GC->mptable, GC->GETbase);
}

void ApolloRTCCMFD::menuMPTTLIDirectInput()
{
	G->MPTTLIDirectInput();
}

void ApolloRTCCMFD::menuNextStationContactLunar()
{
	GC->rtcc->med_b04.FUNCTION = !GC->rtcc->med_b04.FUNCTION;
}

void ApolloRTCCMFD::menuGenerateStationContacts()
{
	bool GenerateStationContactsInput(void* id, char *str, void *data);
	oapiOpenInputBox("Generate station contacts (Format: 1 = LEM, 2 = CSM):", GenerateStationContactsInput, 0, 20, (void*)this);
}

bool GenerateStationContactsInput(void* id, char *str, void *data)
{
	int veh;
	if (sscanf(str, "%d", &veh) == 1)
	{
		((ApolloRTCCMFD*)data)->set_GenerateStationContacts(veh);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_GenerateStationContacts(int VEH)
{
	GC->rtcc->med_b03.VEH = VEH;
	G->GenerateStationContacts();
}

void ApolloRTCCMFD::menuPredSiteAcqGET()
{
	bool PredSiteAcqGETInput(void* id, char *str, void *data);
	oapiOpenInputBox("GET to start search form station contacts (Format: hhh:mm:ss)", PredSiteAcqGETInput, 0, 20, (void*)this);
}

bool PredSiteAcqGETInput(void *id, char *str, void *data)
{
	int hh, mm, ss, get;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		get = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_PredSiteAcqGET(get);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_PredSiteAcqGET(double get)
{
	GC->rtcc->med_u15.PARAM1 = get;
}

void ApolloRTCCMFD::menuPredSiteAcqDT()
{
	bool PredSiteAcqDTInput(void* id, char *str, void *data);
	oapiOpenInputBox("Time interval for searching station contacts (Format: hhh:mm:ss)", PredSiteAcqDTInput, 0, 20, (void*)this);
}

bool PredSiteAcqDTInput(void *id, char *str, void *data)
{
	int hh, mm, ss, dt;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		dt = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_PredSiteAcqDT(dt);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_PredSiteAcqDT(double dt)
{
	GC->rtcc->med_u15.PARAM2 = dt;
}

void ApolloRTCCMFD::PredSiteAcqCalc()
{
	G->CalculatePredictedSiteAcquisitionDisplay();
}

void ApolloRTCCMFD::menuSetLDPPAzimuth()
{
	bool LDPPAzimuthInput(void* id, char *str, void *data);
	oapiOpenInputBox("Approach azimuth at the landing site (0 for optimal):", LDPPAzimuthInput, 0, 20, (void*)this);
}

bool LDPPAzimuthInput(void* id, char *str, void *data)
{
	double azi;
	if (sscanf(str, "%lf", &azi) == 1)
	{
		((ApolloRTCCMFD*)data)->set_LDPPAzimuth(azi);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LDPPAzimuth(double azi)
{
	GC->rtcc->med_k17.Azimuth = azi*RAD;
}

void ApolloRTCCMFD::menuSetLDPPPoweredDescTime()
{
	bool LDPPPoweredDescTimeInput(void* id, char *str, void *data);
	oapiOpenInputBox("Time for powered descent ignition (not available yet):", LDPPPoweredDescTimeInput, 0, 20, (void*)this);
}

bool LDPPPoweredDescTimeInput(void *id, char *str, void *data)
{
	int hh, mm, ss, dt;
	if (sscanf(str, "%d:%d:%d", &hh, &mm, &ss) == 3)
	{
		dt = ss + 60 * (mm + 60 * hh);
		((ApolloRTCCMFD*)data)->set_LDPPPoweredDescTime(dt);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LDPPPoweredDescTime(double pdi)
{
	GC->rtcc->med_k17.PoweredDescTime = pdi;
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
	}
	else if (thr == 2)
	{
		GC->rtcc->med_k16.GETTH2 = dt;
	}
	else if (thr == 3)
	{
		GC->rtcc->med_k16.GETTH3 = dt;
	}
	else if (thr == 4)
	{
		GC->rtcc->med_k16.GETTH4 = dt;
	}
}

void ApolloRTCCMFD::menuSetLDPPDescentFlightTime()
{
	bool LDPPDescentFlightTimeInput(void* id, char *str, void *data);
	oapiOpenInputBox("Descent flight time in minutes:", LDPPDescentFlightTimeInput, 0, 20, (void*)this);
}

bool LDPPDescentFlightTimeInput(void* id, char *str, void *data)
{
	double dt;
	if (sscanf(str, "%lf", &dt) == 1)
	{
		((ApolloRTCCMFD*)data)->set_LDPPDescentFlightTime(dt);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LDPPDescentFlightTime(double dt)
{
	GC->rtcc->med_k17.DescentFlightTime = dt * 60.0;
}

void ApolloRTCCMFD::cycleLDPPVehicle()
{
	if (GC->MissionPlanningActive)
	{
		if (GC->rtcc->med_k16.Vehicle < 2)
		{
			GC->rtcc->med_k16.Vehicle++;
		}
		else
		{
			GC->rtcc->med_k16.Vehicle = 1;
		}
	}
}

void ApolloRTCCMFD::menuSetLDPPDesiredHeight()
{
	bool LDPPDesiredHeightInput(void* id, char *str, void *data);
	oapiOpenInputBox("Desired height in NM:", LDPPDesiredHeightInput, 0, 20, (void*)this);
}

bool LDPPDesiredHeightInput(void* id, char *str, void *data)
{
	double alt;
	if (sscanf(str, "%lf", &alt) == 1)
	{
		((ApolloRTCCMFD*)data)->set_LDPPDescentFlightTime(alt);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_LDPPDesiredHeight(double alt)
{
	GC->rtcc->med_k16.DesiredHeight = alt / 1852.0;
}

void ApolloRTCCMFD::menuCycleDOIOption()
{
	if (GC->DOI_option < 1)
	{
		GC->DOI_option++;
	}
	else
	{
		GC->DOI_option = 0;
	}
}

void ApolloRTCCMFD::menuSunriseSunsetTimesCalc()
{
	bool SunriseSunsetTimesCalcInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: IND,PARAM (IND: REV = 0, GET = 1)", SunriseSunsetTimesCalcInput, 0, 20, (void*)this);
}

bool SunriseSunsetTimesCalcInput(void* id, char *str, void *data)
{
	int ind;
	double param;
	if (sscanf(str, "%d,%lf", &ind, &param) == 2)
	{
		((ApolloRTCCMFD*)data)->set_SunriseSunsetTimesInput(ind, param);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_SunriseSunsetTimesInput(int IND, double PARAM)
{
	GC->rtcc->med_u08.IND = IND;
	GC->rtcc->med_u08.PARM = PARAM;
	G->SunriseSunsetTimesCalc();
}

void ApolloRTCCMFD::menuMoonriseMoonsetTimesCalc()
{
	bool MoonriseMoonsetTimesCalcInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: IND,PARAM (IND: REV = 0, GET = 1)", MoonriseMoonsetTimesCalcInput, 0, 20, (void*)this);
}

bool MoonriseMoonsetTimesCalcInput(void* id, char *str, void *data)
{
	int ind;
	double param;
	if (sscanf(str, "%d,%lf", &ind, &param) == 2)
	{
		((ApolloRTCCMFD*)data)->set_MoonriseMoonsetTimesInput(ind, param);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_MoonriseMoonsetTimesInput(int IND, double PARAM)
{
	GC->rtcc->med_u07.IND = IND;
	GC->rtcc->med_u07.PARM = PARAM;
	G->MoonriseMoonsetTimesCalc();
}

void ApolloRTCCMFD::menuCapeCrossingInit()
{
	bool CapeCrossingInitInput(void* id, char *str, void *data);
	oapiOpenInputBox("Format: VEH,REV (1 = LEM, 3 = CSM)", CapeCrossingInitInput, 0, 20, (void*)this);
}

bool CapeCrossingInitInput(void* id, char *str, void *data)
{
	int veh, rev;
	if (sscanf(str, "%d,%d", &veh, &rev) == 2)
	{
		((ApolloRTCCMFD*)data)->set_CapeCrossingRev(veh, rev);
		return true;
	}
	return false;
}

void ApolloRTCCMFD::set_CapeCrossingRev(int veh, int rev)
{
	GC->rtcc->med_p17.VEH = veh;
	GC->rtcc->med_p17.REV = rev;
	G->CapeCrossingTableUpdate();
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

void ApolloRTCCMFD::SStoHHMMSS(double time, int &hours, int &minutes, double &seconds)
{
	double mins;
	hours = (int)trunc(time / 3600.0);
	mins = fmod(time / 60.0, 60.0);
	minutes = (int)trunc(mins);
	seconds = (mins - minutes) * 60.0;
}

void ApolloRTCCMFD::papiWriteScenario_SV(FILEHANDLE scn, char *item, MPTSV sv) {

	char buffer[256], name[16];

	if (sv.gravref)
	{
		oapiGetObjectName(sv.gravref, name, 16);
	}
	else
	{
		sprintf(name, "None");
	}

	sprintf(buffer, "  %s %s %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf", item, name, sv.MJD, sv.R.x, sv.R.y, sv.R.z, sv.V.x, sv.V.y, sv.V.z);
	oapiWriteLine(scn, buffer);
}

bool ApolloRTCCMFD::papiReadScenario_SV(char *line, char *item, MPTSV &sv)
{
	char buffer[256], name[16];

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			MPTSV v;
			if (sscanf(line, "%s %s %lf %lf %lf %lf %lf %lf %lf", buffer, name, &v.MJD, &v.R.x, &v.R.y, &v.R.z, &v.V.x, &v.V.y, &v.V.z) == 9) {
				v.gravref = oapiGetObjectByName(name);
				sv = v;
				return true;
			}
		}
	}
	return false;
}