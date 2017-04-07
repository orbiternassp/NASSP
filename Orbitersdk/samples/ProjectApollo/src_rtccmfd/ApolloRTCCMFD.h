// ==============================================================
//                 ORBITER MODULE: DialogTemplate
//                  Part of the ORBITER SDK
//            Copyright (C) 2003 Martin Schweiger
//                   All rights reserved
//
// ApolloRTCCMFDe.h
//
// This module demonstrates how to build an Orbiter plugin which
// inserts a new MFD (multi-functional display) mode. The code
// is not very useful in itself, but it can be used as a starting
// point for your own MFD developments.
// ==============================================================

#ifndef __ApolloRTCCMFD_H
#define __ApolloRTCCMFD_H

#include "ARCore.h"
#include "soundlib.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "lemcomputer.h"
#include "IMU.h"
#include "saturn.h"
#include "saturnv.h"
#include "LEM.h"

class ApolloRTCCMFD: public MFD2 {
public:
	ApolloRTCCMFD (DWORD w, DWORD h, VESSEL *vessel);
	~ApolloRTCCMFD ();
	char *ButtonLabel (int bt);
	int ButtonMenu (const MFDBUTTONMENU **menu) const;
	bool Update (oapi::Sketchpad *skp);
	static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);
	bool ConsumeButton(int bt, int event);
	bool ConsumeKeyBuffered(DWORD key);
	void WriteStatus(FILEHANDLE scn) const;
	void ReadStatus(FILEHANDLE scn);
	void StoreStatus(void) const;
	void RecallStatus(void);

	void t1dialogue();
	void set_t1(double t1);
	void t2dialogue();
	void set_t2(double t2, bool t1dep);
	void CDHtimedialogue();
	void set_CDHtime(double CDHtime);
	void DHdialogue();
	void set_DH(double DH);
	void revdialogue();
	void set_rev(int rev);
	void set_target();
	//void set_offset();
	void xdialogue();
	void ydialogue();
	void zdialogue();
	void set_Xoff(double x);
	void set_Yoff(double y);
	void set_Zoff(double z);
	void menuVoid();
	void menuNextPage();
	void menuLastPage();
	void menuSetLambertPage();
	void menuSetCDHPage();
	void menuSetOffsetPage();
	void menuSetREFSMMATPage();
	void menuSetEntryPage();
	void menuSetSVPage();
	void menuSetMenu();
	void menuSetConfigPage();
	void menuSetOrbAdjPage();
	void menuSetMapUpdatePage();
	void REFSMMATTimeDialogue();
	void cycleREFSMMATHeadsUp();
	void set_REFSMMATTime(double time);
	void calcREFSMMAT();
	void OrbAdjApoDialogue();
	void set_OrbAdjApo(double apo);
	void OrbAdjPeriDialogue();
	void set_OrbAdjPeri(double peri);
	void OrbAdjIncDialogue();
	void set_OrbAdjInc(double inc);
	void OrbAdjGETDialogue();
	void set_OrbAdjGET(double SPSGET);
	void OrbAdjCalc();
	void phasedialogue();
	void set_getbase();
	void calcphaseoff(double angdeg);
	void CDHcalc();
	void lambertcalc();
	char* GET_Display(char * Buff, double time);
	char* AGC_Display(char * Buff, double time);
	void SStoHHMMSS(double time, int &hours, int &minutes, double &seconds);
	void gravrefdialogue();
	void set_gravref(OBJHANDLE body);
	double timetoperi();
	double timetoapo();
	void CycleREFSMMATopt();
	void UploadREFSMMAT();
	void menuP30Upload();
	void EntryAngDialogue();
	void set_entryang(double ang);
	void EntryTimeDialogue();
	void set_EntryTime(double time);
	void set_entrylat(double lat);
	void EntryLatDialogue();
	void set_entrylng(double lng);
	void EntryLngDialogue();
	void menuEntryCalc();
	void set_entryrange(double range);
	void EntryRangeDialogue();
	void menuSVCalc();
	void menuSwitchSVSlot();
	void menuSVUpload();
	void menuEntryUpload();
	void CycleEntryOpt();
	void set_spherical();
	void menuSwitchHeadsUp();
	void menuCalcManPAD();
	void menuSetManPADPage();
	void menuCalcEntryPAD();
	void menuSetEntryPADPage();
	void menuSwitchCritical();
	void menuSwitchEntryPADOpt();
	void menuSwitchEntryPADDirect();
	void menuManPADTIG();
	void set_ManPADTIG(double ManPADTIG);
	void menusextantstartime();
	void set_sextantstartime(double time);
	void menuManPADDV();
	void set_P30DV(VECTOR3 dv);
	void menuSwitchManPADEngine();
	void set_lambertelev(double elev);
	void menuSwitchManPADopt();
	void menuLSLat();
	void set_LSLat(double lat);
	void menuLSLng();
	void set_LSLng(double lng);
	void menuREFSMMATdirect();
	void menuSetSVTime();
	void set_SVtime(double SVtime);
	void menuCalcMapUpdate();
	void menuSwitchMapUpdate();
	void menuSwitchUplinkInhibit();
	void set_CDHtimemode();
	void menuSetLaunchMJD();
	void set_launchmjd(double mjd);
	void menuSetAGCEpoch();
	void set_AGCEpoch(double mjd);
	void menuChangeVesselType();
	void cycleREFSMMATupl();
	void set_svtarget();
	void offvecdialogue();
	void set_offvec(VECTOR3 off);
	void GetREFSMMATfromAGC();
	void GetEntryTargetfromAGC();
	void menuCycleSVTimeMode();
	void menuCycleSVMode();
	void set_lambertaxis();
	void menuSwitchEntryNominal();
	void EntryLongitudeModeDialogue();
	void menuSetLOIPage();
	void menuSwitchLOIManeuver();
	void menuSwitchTLCCManeuver();
	void menuSetTLCCGET();
	void set_TLCCGET(double time);
	void menuSetTLCCPeriGET();
	void set_TLCCPeriGET(double time);
	void menuSetTLCCLat();
	void set_TLCCLat(double lat);
	void menuSetTLCCLng();
	void set_TLCCLng(double lng);
	void menuSetTLCCAlt();
	void set_TLCCAlt(double alt);
	void menuSetLOIApo();
	void set_LOIApo(double alt);
	void menuSetLOIPeri();
	void set_LOIPeri(double alt);
	void menuSetLOIAzi();
	void set_LOIAzi(double inc);
	void menuSetTLAND();
	void menuLOICalc();
	void menuRequestLTMFD();
	void menuSetLandmarkTrkPage();
	void menuSetLmkTime();
	void set_LmkTime(double time);
	void menuSetLmkLat();
	void set_LmkLat(double lat);
	void menuSetLmkLng();
	void set_LmkLng(double lng);
	void menuLmkPADCalc();
	void menuSetTargetingMenu();
	void menuSetPADMenu();
	void menuSetUtilityMenu();
	void menuSetVECPOINTPage();
	void menuTranslunarPage();
	void cycleVECDirOpt();
	void vecbodydialogue();
	void set_vecbody(OBJHANDLE body);
	void menuVECPOINTCalc();
	void menuSetDOIGET();
	void set_DOIGET(double time);
	void menuLSAlt();
	void set_LSAlt(double alt);
	void menuSetDOIRevs();
	void set_DOIRevs(int N);
	void menuDOICalc();
	void menuSetDOIPage();
	void menuTLANDUpload();
	void menuSetSkylabPage();
	void menuSwitchSkylabManeuver();
	void menuSetSkylabGET();
	void set_SkylabGET(double time);
	void menuSkylabCalc();
	void menuSetSkylabNC();
	void set_SkylabNC(double N);
	void menuSetSkylabDH1();
	void set_SkylabDH1(double dh);
	void menuSetSkylabDH2();
	void set_SkylabDH2(double dh);
	void menuSetSkylabEL();
	void set_SkylabEL(double E_L);
	void set_SkylabTPI(double time);
	void menuCyclePlaneChange();
	void menuCyclePCManeuver();
	void set_SkylabDTTPM(double dt);
	void menuSetPCPage();
	void menuPCCalc();
	void menuSetPCTIGguess();
	void set_PCTIGguess(double time);
	void menuSetPCAlignGET();
	void set_PCAlignGET(double time);
	void menuSetPCLanded();
	void menuTMLat();
	void set_TMLat(double lat);
	void menuTMLng();
	void set_TMLng(double lng);
	void menuTMAzi();
	void set_TMAzi(double azi);
	void menuSetTerrainModelPage();
	void menuTMDistance();
	void set_TMDistance(double distance);
	void menuTMStepSize();
	void set_TMStepSize(double step);
	void menuTerrainModelCalc();
	void set_TLand(double time);
	void menuTLCCCalc();

protected:
	oapi::Font *font;
	Saturn *saturn;
	LEM *lem;
	int screen;
	static struct ScreenData {
		int screen;
	} screenData;
private:
	ARCore* G;
	ApolloRTCCMFDButtons coreButtons;	
};

#endif // !__ApolloRTCCMFD_H
