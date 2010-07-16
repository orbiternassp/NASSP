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
  *	Revision 1.2  2009/09/17 17:48:42  tschachim
  *	DSKY support and enhancements of ChecklistMFD / ChecklistController
  *	
  *	Revision 1.1  2009/02/18 23:21:48  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.17  2008/04/15 20:17:52  lassombra
  *	Implementing callbacks into DSKY for use by panel switch system (specifically geared towards checklist mfd compatibility
  *	
  *	Revision 1.16  2007/08/13 16:06:11  tschachim
  *	Moved bitmaps to subdirectory.
  *	New VAGC mission time pad load handling.
  *	New telescope and sextant panels.
  *	Fixed CSM/LV separation speed.
  *	
  *	Revision 1.15  2006/06/25 21:19:45  movieman523
  *	Lots of Doxygen updates.
  *	
  *	Revision 1.14  2006/06/24 15:40:06  movieman523
  *	Working on MET-driven audio playback. Also added initial Doxygen comments.
  *	
  *	Revision 1.13  2006/06/21 13:11:29  tschachim
  *	Bugfix power drawing.
  *	
  *	Revision 1.12  2006/06/10 14:36:44  movieman523
  *	Numerous changes. Lots of bug-fixes, new LES jettison code, lighting for guarded push switches and a partial rewrite of the Saturn 1b mesh code.
  *	
  *	Revision 1.11  2006/05/30 23:15:14  movieman523
  *	Mission timer and DSKY now need power to operate.
  *	
  *	Revision 1.10  2005/08/29 19:15:03  tschachim
  *	Rendering of the DSKY keys.
  *	
  *	Revision 1.9  2005/08/19 13:43:42  tschachim
  *	Channel 13 handling
  *	
  *	Revision 1.8  2005/08/18 22:15:22  movieman523
  *	Wired up second DSKY, to accurately match the real hardware.
  *	
  *	Revision 1.7  2005/08/18 20:54:16  movieman523
  *	Added Main Release switch and wired it up to the parachutes.
  *	
  *	Revision 1.6  2005/08/18 20:07:19  spacex15
  *	fixed click sound missing in lm dsky
  *	
  *	Revision 1.5  2005/08/18 00:22:53  movieman523
  *	Wired in CM Uplink switch, removed some old code, added initial support for second DSKY.
  *	
  *	Revision 1.4  2005/08/11 01:27:26  movieman523
  *	Added initial Virtual AGC support.
  *	
  *	Revision 1.3  2005/08/09 02:28:26  movieman523
  *	Complete rewrite of the DSKY code to make it work with the real AGC I/O channels. That should now mean we can just hook up the Virtual AGC and have it work (with a few tweaks).
  *	
  *	Revision 1.2  2005/08/08 22:32:49  movieman523
  *	First steps towards reimplementing the DSKY interface to use the same I/O channels as the real AGC/DSKY interface.
  *	
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

///
/// \ingroup DSKY
/// DSKY simulation.
/// \brief Generic DSKY simulation class.
///

#include "toggleswitch.h"
class DSKY : public e_object

{
public:

	DSKY(SoundLib &s, ApolloGuidance &computer, int IOChannel = 015);
	virtual ~DSKY();

	void Init(e_object *powered, RotationalSwitch *dimmer);
	void Reset();

	//
	// Light status.
	//

	bool UplinkLit()	{ return UplinkLight; };
	bool CompActyLit()	{ return CompActy; };
	bool NoAttLit()		{ return NoAttLight; };
	bool StbyLit()		{ return StbyLight; };
	bool KbRelLit()		{ return KbRelLight; };
	bool OprErrLit()	{ return OprErrLight; };
	bool TempLit()		{ return TempLight; };
	bool GimbalLockLit() { return GimbalLockLight; };
	bool ProgLit()		{ return ProgLight; };
	bool RestartLit()	{ return RestartLight; };
	bool TrackerLit()	{ return TrackerLight; };
	bool VelLit()		{ return VelLight; };
	bool AltLit()		{ return AltLight; };

	//
	// Set light status.
	//

	void LightStby()		{ StbyLight = true; };
	void LightRestart()		{ RestartLight = true; };

	void SetUplink(bool val)		{ UplinkLight = val; };
	void SetCompActy(bool val)		{ CompActy = val; };
	void SetNoAtt(bool val)			{ NoAttLight = val; };
	void SetStby(bool val)			{ StbyLight = val; };
	void SetKbRel(bool val)			{ KbRelLight = val; };
	void SetOprErr(bool val)		{ OprErrLight = val; };
	void SetTemp(bool val)			{ TempLight = val; };
	void SetGimbalLock(bool val)	{ GimbalLockLight = val; };
	void SetProg(bool val)			{ ProgLight = val; };
	void SetRestart(bool val)		{ RestartLight = val; };
	void SetTracker(bool val)		{ TrackerLight = val; };
	void SetVel(bool val)			{ VelLight = val; };
	void SetAlt(bool val)			{ AltLight = val; };

	void ClearStby()		{ StbyLight = false; };
	void ClearRestart()		{ RestartLight = false; };

	//
	// Flashing status.
	//

	void SetVerbDisplayFlashing()	{ VerbFlashing = true; };
	void SetNounDisplayFlashing()	{ NounFlashing = true; };

	void ClearVerbDisplayFlashing() { VerbFlashing = false; };
	void ClearNounDisplayFlashing() { NounFlashing = false; };

	//
	// Timestep to run programs.
	//

	void Timestep(double simt);
	void SystemTimestep(double simdt);

	//
	// Keypad interface.
	//

	void KeyRel();
	void VerbPressed();
	void NounPressed();
	void EnterPressed();
	void ClearPressed();
	void ResetPressed();
	void ProgPressed();
	void ProgReleased();
	void PlusPressed();
	void MinusPressed();
	void NumberPressed(int n);

	void VerbCallback(PanelSwitchItem* s);
	void NounCallback(PanelSwitchItem* s);
	void EnterCallback(PanelSwitchItem* s);
	void ClearCallback(PanelSwitchItem* s);
	void ResetCallback(PanelSwitchItem* s);
	void ProgCallback(PanelSwitchItem* s);
	void KeyRelCallback(PanelSwitchItem* s);
	void PlusCallback(PanelSwitchItem* s);
	void MinusCallback(PanelSwitchItem* s);
	void zeroCallback(PanelSwitchItem* s);
	void oneCallback(PanelSwitchItem* s);
	void twoCallback(PanelSwitchItem* s);
	void threeCallback(PanelSwitchItem* s);
	void fourCallback(PanelSwitchItem* s);
	void fiveCallback(PanelSwitchItem* s);
	void sixCallback(PanelSwitchItem* s);
	void sevenCallback(PanelSwitchItem* s);
	void eightCallback(PanelSwitchItem* s);
	void nineCallback(PanelSwitchItem* s);

	void ProcessKeyPress(int mx, int my);
	void ProcessKeyRelease(int mx, int my);
	void RenderLights(SURFHANDLE surf, SURFHANDLE lights, int xoffset = 0, int yoffset = 0, bool hasAltVel = true);
	void RenderData(SURFHANDLE surf, SURFHANDLE digits, SURFHANDLE disp, int xoffset = 0, int yoffset = 0);
	void RenderKeys(SURFHANDLE surf, SURFHANDLE keys, int xoffset = 0, int yoffset = 0);
	void ProcessChannel10(int val);
	void ProcessChannel13(int val);
	void ProcessChannel11Bit(int bit, bool val);
	void ProcessChannel11(int val);

	//
	// Helper functions.
	//

	void LightsOff();

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

protected:

	bool IsPowered();
	void SendKeyCode(int val);

	//
	// Lights.
	//

	void LightOprErrLight()		{ OprErrLight = true; };
	void ClearOprErrLight()		{ OprErrLight = false; };

	//
	// Light power consumption.
	//

	int LightsLit;
	int SegmentsLit;

	//
	// Lights state.
	//

	bool CompActy;
	bool UplinkLight;
	bool NoAttLight;
	bool StbyLight;
	bool KbRelLight;
	bool OprErrLight;
	bool TempLight;
	bool GimbalLockLight;
	bool ProgLight;
	bool RestartLight;
	bool TrackerLight;
	bool VelLight;
	bool AltLight;

	//
	// Keyboard state.
	//

	bool KbInUse;

	bool KeyDown_Verb;
	bool KeyDown_Noun;
	bool KeyDown_Plus;
	bool KeyDown_Minus;
	bool KeyDown_0;
	bool KeyDown_1;
	bool KeyDown_2;
	bool KeyDown_3;
	bool KeyDown_4;
	bool KeyDown_5;
	bool KeyDown_6;
	bool KeyDown_7;
	bool KeyDown_8;
	bool KeyDown_9;
	bool KeyDown_Clear;
	bool KeyDown_Prog;
	bool KeyDown_KeyRel;
	bool KeyDown_Enter;
	bool KeyDown_Reset;

	//
	// Current program state.
	//

	int CurrentVerb;
	int CurrentNoun;

	//
	// Internal variables.
	//

	char Prog[3];
	char Verb[3];
	char Noun[3];
	char R1[7];
	char R2[7];
	char R3[7];

	bool VerbFlashing;
	bool NounFlashing;

	//
	// FlashOn tracks whether flashing displays should currently be shown or
	// blanked out. LastFlashTime tracks when the state was updated.
	//

	bool FlashOn;
	double LastFlashTime;

	int	EnterPos;
	int EnterVal;

	//
	// AGC we're connected to.
	//

	ApolloGuidance &agc;

	//
	// I/O channel to use for key-codes.
	//

	int KeyCodeIOChannel;

	//
	// Sound library.
	//

	SoundLib &soundlib;
	Sound Sclick;

	bool FirstTimeStep;
	RotationalSwitch *DimmerRotationalSwitch;

	//
	// Local helper functions.
	//

	char ValueChar(unsigned val);
	void KeyClick();
	void ResetKeyDown();

	void DSKYLightBlt(SURFHANDLE surf, SURFHANDLE lights, int dstx, int dsty, bool lit, int xOffset, int yOffset);
	void DSKYKeyBlt(SURFHANDLE surf, SURFHANDLE keys, int dstx, int dsty, int srcx, int srcy, bool lit, int xOffset, int yOffset); 
	void RenderTwoDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str, bool Flash);
	void RenderSixDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str);
	int TwoDigitDisplaySegmentsLit(char *Str, bool Flash);
	int SixDigitDisplaySegmentsLit(char *Str);
};

//
// Strings for state saving.
//

#define DSKY_START_STRING	"DSKY_BEGIN"
#define DSKY_END_STRING		"DSKY_END"

#define DSKY2_START_STRING	"DSKY2_BEGIN"
#define DSKY2_END_STRING	"DSKY2_END"

