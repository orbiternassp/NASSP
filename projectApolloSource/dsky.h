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

class DSKY

{
public:

	DSKY(SoundLib &s, ApolloGuidance &computer, int IOChannel);
	virtual ~DSKY();

	void Init();
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

	void ProcessKeyPress(int mx, int my);
	void ProcessKeyRelease(int mx, int my);
	void RenderLights(SURFHANDLE surf, SURFHANDLE lights);
	void RenderData(SURFHANDLE surf, SURFHANDLE digits);
	void ProcessChannel10(int val);

	//
	// Helper functions.
	//

	void LightsOff();

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

protected:

	void SendKeyCode(int val);

	//
	// Lights.
	//

	void LightOprErrLight()		{ OprErrLight = true; };
	void ClearOprErrLight()		{ OprErrLight = false; };

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

	//
	// Local helper functions.
	//

	char ValueChar(unsigned val);
	void KeyClick();

	void DSKYLightBlt(SURFHANDLE surf, SURFHANDLE lights, int dstx, int dsty, bool lit);
	void RenderTwoDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str, bool Flash);
	void RenderSixDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str);
};

//
// Strings for state saving.
//

#define DSKY_START_STRING	"DSKY_BEGIN"
#define DSKY_END_STRING		"DSKY_END"


