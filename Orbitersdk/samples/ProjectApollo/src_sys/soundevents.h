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

  **************************************************************************/

#ifndef SOUNDEVENTS_H
#define SOUNDEVENTS_H

//
// Looks like VC6.0 doesn't know about DWORD_PTR.
//

#if _MSC_VER <= 1300
typedef unsigned long *DWORD_PTR;
#endif

// X15 direct sound include to add landing sound

#include "dsound.h"

// MODIF x15  managing landing sound

///
/// \ingroup Sound
///
class SoundEvent {

public:
	SoundEvent();
	virtual ~SoundEvent();
	int  isValid();
	int  makeInvalid();

	bool AlreadyPlayed();
	int  play(SoundLib soundlib,
		      VESSEL   *vessel,
			  char     *names,
			  double   *offset,
			  int      *newbuffer,
		      double   simcomputert,
			  double   MissionTime,
			  int mode,double timeremaining,double timeafterpdi,double timetoapproach,
		      int flags = NOLOOP, int volume = 255);
	int  Stop();
	int  Done();
    int  LoadMissionLandingSoundArray(SoundLib soundlib,char *soundname);
    int  LoadMissionTimeSoundArray(SoundLib soundlib, char *soundname, double MissionTime);
    int  InitDirectSound(SoundLib soundlib);
    int  PlaySound(char *filenames,int newbuffer,double offset);
    int	IsPlaying();
	int	Finish(double offsetfinish);

protected:

	double altitude  ;
	int    mode      ;
	double    met       ;
	bool   played    ;
	char   filenames[255] ;
	int    offset    ;
	double timetoignition;
	double timeafterignition;
	double timetoapproach;
	int    mandatory ;
	
	SoundLib soundlib;
	LPDIRECTSOUND8  m_pDS;
	LPDIRECTSOUNDBUFFER pDSBPrimary;
    LPDIRECTSOUNDBUFFER* apDSBuffer;

    VOID*   pDSLockedBuffer     ;
    DWORD   dwDSLockedBufferSize;


};

#endif // SOUNDEVENTS_H
