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
  *	Revision 1.11  2006/06/25 21:19:45  movieman523
  *	Lots of Doxygen updates.
  *	
  *	Revision 1.10  2006/06/24 15:40:06  movieman523
  *	Working on MET-driven audio playback. Also added initial Doxygen comments.
  *	
  *	Revision 1.9  2006/05/17 18:42:35  movieman523
  *	Partial fix for loading sound volume from scenario.
  *	
  *	Revision 1.8  2006/02/21 11:52:26  tschachim
  *	Fixes to make code build with MS C++ 2005
  *	
  *	Revision 1.7  2006/01/09 19:26:03  tschachim
  *	More attempts to make code build on MS C++ 2005
  *	
  *	Revision 1.6  2005/11/26 16:30:50  movieman523
  *	Fixed retros and trying to fix TLI audio.
  *	
  *	Revision 1.5  2005/11/20 21:50:40  movieman523
  *	Fixed LEM build break.
  *	
  *	Revision 1.4  2005/11/20 21:46:31  movieman523
  *	Added initial volume control support.
  *	
  *	Revision 1.3  2005/08/03 10:44:34  spacex15
  *	improved audio landing synchro
  *	
  *	Revision 1.2  2005/07/14 10:06:14  spacex15
  *	Added full apollo11 landing sound
  *	initial release
  *	
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

///
/// \defgroup Sound Audio support.
/// \brief Code to support audio playback, either directly through DirectSound indirectly
/// through OrbiterSound.
///

#ifndef SOUNDLIB_H
#define SOUNDLIB_H

///
/// \ingroup Sound
///
class SoundData {

public:
	SoundData();
	virtual ~SoundData();
	bool isValid();
	bool isPlaying();
	bool play(int flags, int libflags, int volume, int playvolume);
	void stop();
	void done();
	void setID(int num) { id = num; };
	void setSoundlibId(int num) { SoundlibId = num; };
	void setFileName(char *s);
	void AddRef() { refcount++; };
	void MakeValid() { valid = true; refcount = 0; };
	void MakeInvalid() { valid = false; };
	bool matches(char *s);
	int GetPlayFlags() { return PlayFlags; };
	int GetLibFlags() { return LibFlags; };
	int GetBaseVolume() { return BaseVolume; };
	char *GetFilename() { return filename; };

protected:

	char filename[256];
	int refcount;
	bool valid;

	int	id;
	int PlayVolume;
	int PlayFlags;
	int LibFlags;
	int BaseVolume;
	int SoundlibId;
};

class SoundLib;

///
/// \ingroup Sound
///
class Sound {

public:
	Sound();
	~Sound();
	Sound(Sound &s);
	bool isValid();
	bool isPlaying();
	void setFlags(int fl);
	void clearFlags(int fl);
	bool play(int flags = NOLOOP, int volume = 255);
	void stop();
	void done();
	void SetSoundData(SoundData *s);
	void SetSoundLib(SoundLib *s) { sl = s; };

protected:
	int soundflags;
	bool valid;
	SoundData *sd;
	SoundLib *sl;
};

#define SOUNDFLAG_1XORLESS		0x0001
#define SOUNDFLAG_1XONLY		0x0002

#define SOUNDFLAG_COMMS			0x0010

#define VOLUME_COMMS	1
#define VOLUME_COMMS2	2

///
/// The sound library class which wraps all calls to OrbiterSound into one simple
/// interface. Amongst other things it ensures that OrbiterSound is actually activated
/// before making any calls, saving the caller doing so.
///
/// \ingroup Sound
/// \brief Orbiter sound library.
///
class SoundLib {

public:
	SoundLib();
	virtual ~SoundLib();

	///
	/// \brief Initialise library and connection to Orbitersound.
	/// \param h Vessel handle to pass to Orbitersound.
	/// \param soundclass The 'class' of sound to use (e.g. 'ProjectApollo'). This is 
	/// appended to the sound path to find the right files.
	///
	void InitSoundLib(OBJHANDLE h, char *soundclass);
	void LoadSound(Sound &s, char *soundname, EXTENDEDPLAY extended = DEFAULT);
	void LoadMissionSound(Sound &s, char *soundname, char *genericname = NULL, EXTENDEDPLAY extended = DEFAULT);
	void LoadVesselSound(Sound &s, char *soundname, EXTENDEDPLAY extended = DEFAULT);

	///
	/// The sound library can use a mission-specific path to find files in preference to the base path.
	/// This allows you to have a set of base files (e.g. a file for lunar touchdown) and then override
	/// them on a mission-by-mission basis.
	///
	/// \brief Set the mission-specific file path.
	/// \param mission Mission-specific string to append to base path to find files (e.g. 'Apollo11').
	///
	void SetSoundLibMissionPath(char *mission);

	///
	/// Turn an Orbitersound option on or off. See the Orbitersound header file for the
	/// appropriate definitions to pass to it.
	///
	/// \brief Control Orbitersound options.
	/// \param option Orbitersound option number.
	/// \param onoff Turn Orbitersound option on or off.
	///
	void SoundOptionOnOff(int option, int onoff);
	void SetLanguage(char *language);
	void SetVolume(int type, int percent);
	int GetSoundVolume(int flags, int volume);

	///
	/// \brief Base path to sound files.
	///
	char basepath[256];

	///
	/// \brief Path to mission-specific sound files.
	///
	char missionpath[256];

protected:

	SoundData *DoLoadSound(char *SoundPath, EXTENDEDPLAY extended);
	SoundData *CheckForMatch(char *s);
	int FindSlot();

#define N_VOLUMES	10

	int MasterVolume[N_VOLUMES];

///
/// OrbiterSound currently supports 60 sounds. Note that zero is
/// an invalid id and 60 is valid, so we actually allocate 61 entries
/// here.
///
#define MAX_SOUNDS 60

	SoundData sounds[MAX_SOUNDS+1];

	///
	/// \brief Path to language-specific sound files.
	///
	char languagepath[256];

	///
	/// \brief Is Orbitersound active? If not, we don't call it.
	///
	bool OrbiterSoundActive;

	///
	/// \brief Connection ID returned from initialising Orbitersound.
	///
	int SoundlibId;
	int NextSlot;
};

//
// Another evil nested include for now so we don't have to stick it everywhere in the
// code.
//

#include "soundevents.h"

//
// Timed sound sequencing.
//

///
/// \brief Single timed sound.
/// \ingroup Sound
///
class TimedSound
{
public:
	TimedSound();
	~TimedSound();

	///
	/// \brief Add a sound to the queue.
	///
	void AddToSoundQueue(TimedSound *prev);

	///
	/// \brief Remove this sound from the queue.
	///
	void RemoveFromQueue();

	///
	/// \brief Get the next sound in the queue.
	///
	TimedSound *GetNext() { return next; };

	///
	/// \brief Get the previous sound in the queue.
	///
	TimedSound *GetPrev() { return prev; };

	double GetPlayTime() { return triggerTime; };
	char *GetFilename() { return soundname; };
	bool IsMandatory() { return mandatory; };

	void SetPlayTime(double t) { triggerTime = t; };
	void SetFilename(char *s);
	void SetMandatory(bool m) { mandatory = m; };

protected:

	bool mandatory;
	double triggerTime;
	char soundname[256];

	TimedSound *next;
	TimedSound *prev;
};

///
/// \brief Manager code for timed playback.
/// \ingroup Sound
///
class TimedSoundManager
{
public:
	///
	/// \brief Constructor.
	/// \param s Sound library to use to play sounds.
	///
	TimedSoundManager(SoundLib &s);
	~TimedSoundManager();

	///
	/// \brief Timestep function to play sounds.
	/// \param simt Current mission time  in seconds.
	/// \param simdt Time in seconds since last timestep.
	/// \param autoslow Slow to 1.0x if time acceleration is higher when mandatory sound is played.
	///
	void Timestep(double simt, double simdt, bool autoslow);

	///
	/// \brief Load the sound information from a file.
	/// \param dataFile Name of data-file, found in the mission-specific sound directory.
	/// \param MissionTime Current mission-time, used to skip over earlier sounds.
	///
	void LoadFromFile(char *dataFile, double MissionTime);

protected:
	///
	/// \brief List of sounds that are launch-relative.
	///
	TimedSound *launchRelativeList;

	///
	/// \brief List of sounds that are re-entry relative.
	///
	TimedSound *reentryRelativeList;

	///
	/// \brief Current sound to play.
	///
	Sound currentSound;

	///
	/// \brief Did we load a sound to play?
	///
	bool SoundToPlay;

	///
	/// \brief Time to play sound.
	///
	double TimeToPlay;

	///
	/// \brief Is the next sound mandatory?
	///
	bool SoundIsMandatory;

	///
	/// \brief Any launch-relative sounds to play?
	///
	bool LaunchSoundsLoaded;

	///
	/// \brief Sound library to use for playback.
	///
	SoundLib &soundlib;
};

#endif // SOUNDLIB_H