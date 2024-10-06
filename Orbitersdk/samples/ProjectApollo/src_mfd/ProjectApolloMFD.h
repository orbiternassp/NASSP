/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2006 

  Project Apollo MFD

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

#ifndef __PROJECTAPOLLOMFD_H
#define __PROJECTAPOLLOMFD_H

#include "ProjectApolloMFDButtons.h"

class Saturn;
class LEM;
class Crawler;
class MCC;

///
/// \ingroup MFD
///
class ProjectApolloMFD: public MFD2 {
public:
	ProjectApolloMFD (DWORD w, DWORD h, VESSEL *vessel);
	~ProjectApolloMFD ();
	char *ButtonLabel (int bt);
	int ButtonMenu (const MFDBUTTONMENU **menu) const;
	bool ConsumeButton (int bt, int event);
	bool ConsumeKeyBuffered (DWORD key);
	bool Update (oapi::Sketchpad* skp);
	void GetStateVector (void);
	bool SetSource(char *rstr);
	bool SetReferencePlanet(char *rstr);
	bool SetCrewNumber (char *rstr);
	bool SetPrimECSTestHeaterPower (char *rstr);
	bool SetSecECSTestHeaterPower (char *rstr);
	bool SetSwitchSelectorChannel(char *rstr);
	bool SetTimebaseUpdate(char *rstr);
	void CalculateV42Angles(void);
	void menuPressEnterOnCMCLGC(void);
	void menuPressPROOnCMCLGC(void);
	void menuPressEnterOnDSKYDEDA(void);
	void menuCycleLMAlignType();
	bool SetImpactTIG(char *rstr);
	bool SetImpactBT(char *rstr);
	bool SetImpactPitch(char *rstr);
	bool SetImpactYaw(char *rstr);
	bool SetGeneralizedManeuverAttitude(char *rstr);
	bool SaturnSetFailureInput(unsigned n, int Condition, double ConditionValue);
	bool SaturnResetFailureInput(unsigned n);
	void SetRandomFailures(double FailureMultiplier);
	void SetAEAACommands(int arm, int set);

	void menuVoid();
	void menuSetMainPage();
	void menuSetGNCPage();
	void menuSetECSPage();
	void menuSetIUPage();
	void menuSetTELEPage();
	void menuSetLGCPage();
	void menuSetDebugPage();
	void menuSetFailuresPage();

	void menuKillRot();
	void menuSaveEMSScroll();
	void menuVAGCCoreDump();
	void menuSetCrewNumber();
	void menuSetCDRInSuit();
	void menuSetLMPInSuit();
	void menuStartEVA();
	void menuConnectCSMO2Hose();
	void menuSetPrimECSTestHeaterPower();
	void menuSetSecECSTestHeaterPower();
	void menuAbortUplink();
	void menuStateVectorUpdate();
	void menuClockUpdate();
	void menuSunburstSuborbitalAbort();
	void menuSunburstCOI();
	void menuSetSource();
	void menuSetReference();
	void menuSetSVSlot();
	void menuClearDebugLine();
	void menuFreezeDebugLine();
	void menuSetIUSource();
	void menuCycleIUUplinkType();
	void menuSetIUUplinkInp1();
	void menuSetIUUplinkInp2();
	void menuSetIUUplinkInp3();
	void menuSetIUUplinkInp4();
	void menuIUUplink();
	void menuCycleFailuresSubpage();
	void menuSetFailure();
	void menuResetFailure();
	void menuSetRandomFailures();
	void menuClearAllFailures();
	void menuAEAACommands();

	void WriteStatus (FILEHANDLE scn) const;
	void ReadStatus (FILEHANDLE scn);
	void StoreStatus (void) const;
	void RecallStatus (void);
	static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

protected:

	void GetCSM();

	SURFHANDLE hLogo;
	DWORD width;
	DWORD height;
	//We can link to the Saturn OR the Crawler.  Only the Saturn has full funcionality.  The Crawler can ONLY read the mission time.
	Saturn *saturn;
	Crawler *crawler;
	LEM *lem;
	MCC *mcc;
	int screen;
	bool debug_frozen;
	bool isSaturnV;
	unsigned FailureSubpage;

	static struct ScreenData {
		int screen;
	} screenData;

	const ProjectApolloMFDButtons m_buttonPages;
};

#endif // !__PROJECTAPOLLOMFD_H
