/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005



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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.3  2005/04/11 23:46:17  yogenfrutz
  *	added InPanel and Panel_ID
  *	
  *	Revision 1.2  2005/02/24 00:27:28  movieman523
  *	Revisions to make LEVA sounds work.
  *	
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

//
// Lem state settings from scenario file, passed from CSM.
//

typedef struct {

	double LandingLatitude;
	double LandingLongitude;
	double LandingAltitude;
	int MissionNo;
	double MissionTime;
	char language[64];
	bool Crewed;
	bool AutoSlow;
	int Realism;

} LemSettings;

class sat5_lmpkd : public VESSEL {

public:
	sat5_lmpkd(OBJHANDLE hObj, int fmodel);
	virtual ~sat5_lmpkd();

	void Init();
	void LoadStateEx (FILEHANDLE scn, void *vs);
	void SaveState (FILEHANDLE scn);
	int ConsumeDirectKey (const char *keystate);
	void PostStep(double simt, double simdt, double mjd);
	bool PanelMouseEvent (int id, int event, int mx, int my);
	bool PanelRedrawEvent (int id, int event, SURFHANDLE surf);
	bool LoadPanel (int id);
	bool LoadVC (int id);
	bool LoadGenericCockpit ();
	void PostCreation();
	void SetClassCaps (FILEHANDLE cfg);
	void SetStateEx(const void *status);
	void SetLmVesselDockStage();
	void SetLmVesselHoverStage();
	void SetLmAscentHoverStage();
	void StartAscent();
	virtual void SetLanderData(LemSettings &ls);

	PROPELLANT_HANDLE ph_Dsc, ph_Asc, ph_rcslm0,ph_rcslm1; // handles for propellant resources
	THRUSTER_HANDLE th_hover[2];               // handles for orbiter main engines,added 2 for "virtual engine"
	THRUSTER_HANDLE th_att_rot[24], th_att_lin[24];                 // handles for SPS engines
	THGROUP_HANDLE thg_hover;		          // handles for thruster groups

protected:
	void RedrawPanel_Thrust (SURFHANDLE surf);
	void ReleaseSurfaces ();
	void ResetThrusters();
	void SetRCS(PROPELLANT_HANDLE ph_prop);
	void AttitudeLaunch1();
	void SeparateStage (UINT stage);
	void InitPanel (int panel);
	void AddRCS_LM(double TRANZ);
	void AddRCS_LM2(double TRANZ);
	void AddRCS_LMH(double TRANZ);
	void AddRCS_LMH2(double TRANZ);
	void ToggleEVA();
	void SetupEVA();
	void SetView();
	void RedrawPanel_Horizon (SURFHANDLE surf);
	void SwitchClick();
	void CabinFanSound();
	void VoxSound();
	void ButtonClick();
	void GuardClick();
	void AbortFire();
	void InitPanel();
	void DoFirstTimestep();
	void LoadDefaultSounds();
	void GetDockStatus();

	bool CabinFansActive();
	bool AscentEngineArmed();
	bool AscentRCSArmed();

	//
	// Save/Load support functions.
	//

	int GetCSwitchState();
	void SetCSwitchState(int s);
	int GetSSwitchState();
	void SetSSwitchState(int s);
	int GetLPSwitchState();
	void SetLPSwitchState(int s);
	int GetRPSwitchState();
	void SetRPSwitchState(int s);

	SURFHANDLE srf[nsurf];  // handles for panel bitmaps

	double actualVEL;
	double actualALT;
	double actualFUEL;
	double aVAcc;
	double aVSpeed;
	double aHAcc;
	double ALTN1;
	double SPEEDN1;
	double VSPEEDN1;
	double aTime;
	double AtempP ;
	double AtempY ;
	double AtempR ;
	double MissionTime;

	//bool bAbort;
	bool RCS_Full;
	bool Eds;

	bool toggleRCS;

	bool Cswitch1;
	bool Cswitch2;
	bool Cswitch3;
	bool Cswitch4;
	bool Cswitch5;
	bool Cswitch6;
	bool Cswitch7;
	bool Cswitch8;
	bool Cswitch9;

	bool Sswitch1;
	bool Sswitch2;
	bool Sswitch3;
	bool Sswitch4;
	bool Sswitch5;
	bool Sswitch6;
	bool Sswitch7;
	bool Sswitch8;
	bool Sswitch9;

	bool RPswitch1;
	bool RPswitch2;
	bool RPswitch3;
	bool RPswitch4;
	bool RPswitch5;
	bool RPswitch6;
	bool RPswitch7;
	bool RPswitch8;
	bool RPswitch9;
	bool RPswitch10;
	bool RPswitch11;
	bool RPswitch12;
	bool RPswitch13;
	bool RPswitch14;
	bool RPswitch15;
	bool RPswitch16;
	bool RPswitch17;

	bool LPswitch1;
	bool LPswitch2;
	bool LPswitch3;
	bool LPswitch4;
	bool LPswitch5;
	bool LPswitch6;
	bool LPswitch7;
	bool SPSswitch;
	bool EDSswitch;

	bool DESHE1switch;
	bool DESHE2switch;

	bool ENGARMswitch;

	bool QUAD1switch;
	bool QUAD2switch;
	bool QUAD3switch;
	bool QUAD4switch;
	bool QUAD5switch;
	bool QUAD6switch;
	bool QUAD7switch;
	bool QUAD8switch;

	bool AFEED1switch;
	bool AFEED2switch;
	bool AFEED3switch;
	bool AFEED4switch;

	bool MSOV1switch;
	bool MSOV2switch;

	bool LDGswitch;

	bool ED1switch;
	bool ED2switch;
	bool ED4switch;
	bool ED5switch;
	bool ED6switch;

	bool ED7switch;
	bool ED8switch;
	bool ED9switch;

	bool GMBLswitch;

	bool ASCHE1switch;
	bool ASCHE2switch;

	bool RCSQ1switch;
	bool RCSQ2switch;
	bool RCSQ3switch;
	bool RCSQ4switch;

	bool ATT1switch;
	bool ATT2switch;
	bool ATT3switch;

	bool CRSFDswitch;

	bool CABFswitch;

	bool PTTswitch;

	bool RCSS1switch;
	bool RCSS2switch;
	bool RCSS3switch;
	bool RCSS4switch;

	bool X1switch;

	bool GUIDswitch;

	bool ALTswitch;

	bool RATE1switch;
	bool AT1switch;

	bool SHFTswitch;

	bool ETC1switch;
	bool ETC2switch;
	bool ETC3switch;
	bool ETC4switch;

	bool PMON1switch;
	bool PMON2switch;

	bool ACAPswitch;

	bool RATE2switch;
	bool AT2switch;

	bool DESEswitch;

	bool SLWRswitch;

	bool DBswitch;

	bool IMUCswitch;

	bool SPLswitch;

	bool X2switch;

	bool P41switch;
	bool P42switch;
	bool P43switch;
	bool P44switch;

	bool AUDswitch;
	bool RELswitch;

	bool CPswitch;

	bool HATCHswitch;

	bool EVAswitch;

	bool COASswitch;

	bool Abortswitch;

	bool FirstTimestep;

	bool LAUNCHIND[8];
	bool ABORT_IND;
	bool ENGIND[7];

	double countdown;

	bool high;
	bool bToggleHatch;
	bool bModeDocked;
	bool bModeHover;
	bool HatchOpen;
	bool bManualSeparate;
	bool ToggleEva;
	bool EVA_IP;

	bool lmpview;
	bool cdrview;
	bool startimer;
	//bool bABort;
	bool ContactOK;
	bool SoundsLoaded;

	bool Crewed;
	bool AutoSlow;

	OBJHANDLE hLEVA;
	OBJHANDLE hdsc;
	UINT stage;
	int status;

	int Realism;

	int SwitchFocusToLeva;

	DSKY dsky;
	LEMcomputer agc;

	bool InVC;
	bool InPanel; //yogen
	int  PanelId; //yogen

	SoundLib soundlib;

	Sound Sclick;
	Sound Bclick;
	Sound Gclick;
	Sound LunarAscent;
	Sound StageS;
	Sound S5P100;
	Sound Scontact;
	Sound SDMode;
	Sound SHMode;
	Sound SLEVA;
	Sound SAbort;
	Sound CabinFans;
	Sound Vox;
	Sound Afire;

	char AudioLanguage[64];
};

extern void LEMLoadMeshes();
extern void InitGParam(HINSTANCE hModule);
extern void FreeGParam();