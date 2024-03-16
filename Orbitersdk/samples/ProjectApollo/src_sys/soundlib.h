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

#ifndef SOUNDLIB_H
#define SOUNDLIB_H

class XRSound;

// This is the structure used by Set3dWaveParameters
typedef struct OS3DCONE {
    DWORD dwInsideConeAngle;
    DWORD dwOutsideConeAngle;
    VECTOR3 vConeOrientation;
    double lConeOutsideVolume;
} OS3DCONE;

// These are the keywords used by PlayVesselWave
#define NOLOOP	0
#define LOOP	1

// These are the keywords used by "RequestLoadWave***()"
typedef enum{
		DEFAULT,
		INTERNAL_ONLY,
		BOTHVIEW_FADED_CLOSE,
		BOTHVIEW_FADED_MEDIUM,
		BOTHVIEW_FADED_FAR,
		EXTERNAL_ONLY_FADED_CLOSE,
		EXTERNAL_ONLY_FADED_MEDIUM,
		EXTERNAL_ONLY_FADED_FAR,
		RADIO_SOUND,
}EXTENDEDPLAY;

/** END ORBITERSOUND CODE DUMP  **/

///
/// \ingroup Sound
///
class SoundData {

public:
	SoundData();
	virtual ~SoundData();
	bool isValid();
	bool isPlaying();
	bool play(int flags, int libflags, double volume, double playvolume, int frequency = NULL);
	void stop();
	void done();
	void setID(int num) { id = num; };
	void setSoundlib(XRSound *s) { Soundlib = s; };
	void setFileName(char *s);
	void AddRef() { refcount++; };
	void MakeValid() { valid = true; refcount = 0; };
	void MakeInvalid() { valid = false; };
	bool matches(char *s);
	int GetPlayFlags() { return PlayFlags; };
	int GetLibFlags() { return LibFlags; };
	double GetBaseVolume() { return BaseVolume; };
	char *GetFilename() { return filename; };

protected:

	char filename[256];
	int refcount;
	bool valid;

	int id;
	double PlayVolume;
	int PlayFlags;
	int LibFlags;
	double BaseVolume;
	XRSound *Soundlib;
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
	bool play(int flags = NOLOOP, double volume = 1.0, int frequency = NULL);
	void stop();
	void done();
	void SetSoundData(SoundData *s);
	void SetSoundLib(SoundLib *s) { sl = s; };

	Sound& Sound::operator=(const Sound &s);

protected:
	int soundflags;
	bool valid;
	SoundData *sd;
	SoundLib *sl;
};

class FadeInOutSound : public Sound {
public:
	bool play(double volume = 1.0);
	void stop();

	void setRiseTime(int seconds) { riseSlope = 1.0 / seconds; }
	void setFadeTime(int seconds) { fadeSlope = 1.0 / seconds; }

	void setFrequencyShift(int minFreq, int maxFreq) { fMin = minFreq; fMax = maxFreq; }
	void clearFrequencyShift() { fMin = fMax = NULL; }
	bool hasFrequencyShift() const { return fMin != NULL && fMax != NULL; }

private:
	double riseSlope     = 1.0 / 4.0; // [vol/sec]
	double fadeSlope     = 1.0 / 6.0; // [vol/sec]
	int fMin          =  3000;   // [Hz]
	int fMax          = 22050;   // [Hz]
	double currentVolume = -1.0;      // "lagging" volume level
};


#define SOUNDFLAG_1XORLESS		0x0001
#define SOUNDFLAG_1XONLY		0x0002

#define SOUNDFLAG_COMMS			0x0010

#define VOLUME_COMMS	1
#define VOLUME_COMMS2	2

///
/// The sound library class which wraps all calls to XRSound into one simple
/// interface. Amongst other things it ensures that XRSound is actually activated
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
	/// \brief Initialise library and connection to XRSound.
	/// \param v Vessel pointer to pass to XRSound.
	/// \param soundclass The 'class' of sound to use (e.g. 'ProjectApollo'). This is 
	/// appended to the sound path to find the right files.
	///
	void InitSoundLib(VESSEL *v, char *soundclass);
	void LoadSound(Sound &s, char *soundname, EXTENDEDPLAY extended = DEFAULT);
	void LoadMissionSound(Sound &s, char *soundname, char *genericname = NULL, EXTENDEDPLAY extended = DEFAULT);
	void LoadVesselSound(Sound &s, char *soundname, EXTENDEDPLAY extended = DEFAULT);
	void LoadDefaultSound(Sound &s, char *soundname, EXTENDEDPLAY extended = DEFAULT);

	///
	/// The sound library can use a mission-specific path to find files in preference to the base path.
	/// This allows you to have a set of base files (e.g. a file for lunar touchdown) and then override
	/// them on a mission-by-mission basis.
	///
	/// \brief Set the mission-specific file path.
	/// \param mission Mission-specific string to append to base path to find files (e.g. 'Apollo11').
	///
	void SetSoundLibMissionPath(char *mission);
	void SetLanguage(char *language);
	void SetVolume(int type, double percent);
	double GetSoundVolume(int flags, double volume);
	bool IsXRSoundActive() { return XRSoundActive; };

protected:

	SoundData *DoLoadSound(char *SoundPath, EXTENDEDPLAY extended);
	SoundData *CheckForMatch(char *s);
	int FindSlot();

#define N_VOLUMES	10

	double MasterVolume[N_VOLUMES];

///
/// XRSound supports any number of sounds, starting at an ID of
/// 12000. If we've only used 60 sounds before though, no need
/// to change that here.
///
#define MAX_SOUNDS 60

	SoundData sounds[MAX_SOUNDS+1];

	///
	/// \brief Base path to sound files.
	///
	char basepath[256];

	///
	/// \brief Path to mission-specific sound files.
	///
	char missionpath[256];

	///
	/// \brief Path to language-specific sound files.
	///
	char languagepath[256];

	///
	/// \brief Is XRSound active? If not, we don't call it.
	///
	bool XRSoundActive;

	///
	/// \brief Handle for XRSound from CreateInstance
	///
	XRSound *Soundlib;
	int NextSlot;

	friend class TimedSound;
	friend class TimedSoundManager;
	friend class SoundEvent;
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
	/// \param prev Previous TimedSound in the queue (may be NULL).
	///
	void AddToSoundQueue(TimedSound *prev);

	///
	/// \brief Remove this sound from the queue.
	///
	void RemoveFromQueue();

	///
	/// \brief Get the next sound in the queue.
	/// \return Pointer to next sound.
	///
	TimedSound *GetNext() { return next; };

	///
	/// \brief Get the previous sound in the queue.
	/// \return Pointer to previous sound.
	///
	TimedSound *GetPrev() { return prev; };

	///
	/// \brief Get the time at which this should be played.
	/// \return Time to play the sound, relative to the appropriate event (launch, re-entry, etc).
	///
	double GetPlayTime() { return triggerTime; };

	///
	/// \brief Get the sound file name.
	/// \return Pointer to file name (without path)
	///
	char *GetFilename() { return soundname; };

	///
	/// \brief Is this mandatory to play?
	/// \return True if this should always be played.
	///
	bool IsMandatory() { return (priority > 8); };

	///
	/// \brief Is this purely informational, only to be played at low time acceleration?
	/// \return True if this should be skipped when time acceleration > 1.0.
	///
	bool IsInformational() { return (priority < 3); };

	///
	/// \brief Get the priority level.
	/// \return Priority level (0-9).
	///
	int	GetPriority() { return priority; };

	///
	/// \brief Set the time that the sound should be played.
	/// \param t Time to play relative to apropriate event.
	///
	void SetPlayTime(double t) { triggerTime = t; };

	///
	/// \brief Set the filename of the associated sound file.
	/// \param s File name without path.
	///
	void SetFilename(char *s);

	///
	/// \brief Set the priority of this sound.
	/// \param n Priority, from 0 to 9.
	///
	void SetPriority(int n);

protected:

	///
	/// \brief Sound priority.
	///
	int priority;

	///
	/// \brief Sound trigger time.
	///
	double triggerTime;

	///
	/// \brief Sound file name.
	///
	char soundname[256];

	///
	/// \brief Pointer to next sound in queue.
	///
	TimedSound *next;

	///
	/// \brief Pointer to previous sound in queue.
	///
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
	/// \brief Current sound playing.
	///
	Sound currentSound;

	///
	/// \brief Next sound to play.
	///
	Sound nextSound;

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
	/// \brief Is the next sound purely informational?
	///
	bool SoundIsInformational;

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
