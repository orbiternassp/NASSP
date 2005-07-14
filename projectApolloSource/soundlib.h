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
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

// X15 direct sound include to add landing sound
#ifdef DIRECTSOUNDENABLED
typedef DWORD *DWORD_PTR;
#include "dsound.h"
#endif



class SoundData {

public:
	SoundData();
	virtual ~SoundData();
	bool isValid();
	bool isPlaying();
	void play(int flags = NOLOOP, int volume = 255);
	void stop();
	void done();
	void setID(int num) { id = num; };
	void setSoundlibId(int num) { SoundlibId = num; };
	void setFileName(char *s);
	void AddRef() { refcount++; };
	void MakeValid() { valid = true; refcount = 0; };
	void MakeInvalid() { valid = false; };
	bool matches(char *s);

protected:
	char filename[256];
	int refcount;
	bool valid;
	int	id;
	int SoundlibId;
};

class Sound {

public:
	Sound();
	~Sound();
	Sound(Sound &s);
	bool isValid();
	bool isPlaying();
	void setFlags(int fl);
	void clearFlags(int fl);
	void play(int flags = NOLOOP, int volume = 255);
	void stop();
	void done();
	void SetSoundData(SoundData *s);

protected:
	int soundflags;
	bool valid;
	SoundData *sd;
};

#define SOUNDFLAG_1XORLESS		0x0001
#define SOUNDFLAG_1XONLY		0x0002

class SoundLib {

public:
	SoundLib();
	virtual ~SoundLib();

	void InitSoundLib(OBJHANDLE h, char *soundclass);
	void LoadSound(Sound &s, char *soundname, EXTENDEDPLAY extended = DEFAULT);
	void LoadMissionSound(Sound &s, char *soundname, char *genericname = NULL, EXTENDEDPLAY extended = DEFAULT);
	void LoadVesselSound(Sound &s, char *soundname, EXTENDEDPLAY extended = DEFAULT);
	void SetSoundLibMissionPath(char *mission);
	void SoundOptionOnOff(int option, int onoff);
	void SetLanguage(char *language);

	char basepath[256];
	char missionpath[256];


protected:

	SoundData *DoLoadSound(char *SoundPath, EXTENDEDPLAY extended);
	SoundData *CheckForMatch(char *s);
	int FindSlot();

//
// OrbiterSound currently supports 60 sounds. Note that zero is
// an invalid id and 60 is valid, so we actually allocate 61 entries
// here.
//

#define MAX_SOUNDS 60

	SoundData sounds[MAX_SOUNDS+1];

	char languagepath[256];
	bool OrbiterSoundActive;
	int SoundlibId;
};

// MODIF x15  managing landing sound

#ifdef DIRECTSOUNDENABLED
class SoundEvent {

public:
	SoundEvent();
	virtual ~SoundEvent();
	int  isValid();
	int  makeInvalid();

	bool AlreadyPlayed();
	int  play(SoundLib soundlib,
		      VESSEL   *vessel,
		      Sound    &s,
			  char * names,
		      double simcomputert,
			  double simt,
			  int mode,double timeremaining,double timeafterpdi,
		      int flags = NOLOOP, int volume = 255);
	int  Stop();
	int  Done();
    int  LoadMissionLandingSoundArray(SoundLib soundlib,char *soundname);
    int  InitDirectSound(SoundLib soundlib);
    int  PlaySound( char *filenames);
    int SoundEvent::IsPlaying();


	double altitude  ;
	int    mode      ;
	int    met       ;
	bool   played    ;
	char   filenames[255] ;
	double timetoignition;
	double timeafterignition;
	int    mandatory ;
	
	SoundLib soundlib;
	LPDIRECTSOUND8  m_pDS;
	LPDIRECTSOUNDBUFFER pDSBPrimary;
    LPDIRECTSOUNDBUFFER* apDSBuffer;

    VOID*   pDSLockedBuffer     ;
    DWORD   dwDSLockedBufferSize;


};
#endif