/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER sound library.
  This code caches sounds for the OrbiterSound library so that
  you don't have to keep track of IDs.

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
  *	Revision 1.9  2006/05/17 19:04:13  movieman523
  *	Wired up both master volume switches.
  *	
  *	Revision 1.8  2006/01/09 19:26:03  tschachim
  *	More attempts to make code build on MS C++ 2005
  *	
  *	Revision 1.7  2005/11/26 16:30:50  movieman523
  *	Fixed retros and trying to fix TLI audio.
  *	
  *	Revision 1.6  2005/11/20 21:46:31  movieman523
  *	Added initial volume control support.
  *	
  *	Revision 1.5  2005/08/30 14:53:00  spacex15
  *	Added conditionnally defined AGC_SOCKET_ENABLED to use an external socket connected virtual AGC
  *	
  *	Revision 1.4  2005/08/03 10:44:33  spacex15
  *	improved audio landing synchro
  *	
  *	Revision 1.3  2005/07/15 07:58:54  spacex15
  *	fixed CTD short while after landing
  *	
  *	Revision 1.2  2005/07/14 10:06:14  spacex15
  *	Added full apollo11 landing sound
  *	initial release
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <string.h>

#include "OrbiterSoundSDK3.h"
#include "resource.h"

#include "soundlib.h"
#include "tracer.h"
#include "nasspdefs.h"

SoundData::SoundData()

{
	refcount = 0;
	valid = false;
	id = (-1);
	filename[0] = 0;
	SoundlibId = 0;
}

SoundData::~SoundData()

{
	//
	// Nothing to do.
	//
}

bool SoundData::isValid()

{
	if (id < 0)
		return false;

	//
	// If the sound is playing then we must still claim to
	// be valid. Note that for this to work we must ensure
	// that the sound id won't be reused until the sound has
	// gone invalid.
	//

	if (isPlaying())
		return true;

	return valid;
}

void SoundData::play(int flags, int libflags, int volume, int playvolume)

{
	if (valid) {
		PlayVesselWave3(SoundlibId, id, flags, playvolume);
		PlayVolume = playvolume;
		PlayFlags = flags;
		LibFlags = libflags;
		BaseVolume = volume;
	}
}

void SoundData::done()

{
	refcount--;
	if (refcount <= 0) {
		valid = false;
	}
}

void SoundData::stop()

{
	if (!isPlaying())
		return;

	StopVesselWave3(SoundlibId, id);
}

bool SoundData::isPlaying()

{
	if (id < 0)
		return false;

	return (IsPlaying3(SoundlibId, id) != 0);
}

bool SoundData::matches(char *s)

{
	if (!valid)
		return false;

	return (strcmp(s, filename) == 0);
}

void SoundData::setFileName(char *s)

{
	strncpy(filename, s, 255);
}

//
// Soundlib files.
//

SoundLib::SoundLib()

{
	int	i;

	for (i = 0; i <= MAX_SOUNDS; i++) {
		sounds[i].MakeInvalid();
	}

	SoundlibId = 0;
	OrbiterSoundActive = 0;
	missionpath[0] = 0;
	basepath[0] = 0;
	strcpy(languagepath, "English");
	NextSlot = 1;

	for (i = 0; i < N_VOLUMES; i++) {
		MasterVolume[i] = 100;
	}
}

SoundLib::~SoundLib()

{
	//
	// Nothing for now.
	//
}

void SoundLib::InitSoundLib(OBJHANDLE h, char *soundclass)

{
	_snprintf(basepath, 255, "Sound/%s", soundclass);

	SoundlibId = ConnectToOrbiterSoundDLL3(h);
	OrbiterSoundActive = (SoundlibId >= 0);
}

void SoundLib::SetSoundLibMissionPath(char *mission)

{
	strncpy(missionpath, mission, 255);
}

//
// access() doesn't seem to exist on Windows, so we have to do this
// manky thing instead, of opening and then closing.
//

static bool IsFile(char *path)

{
	FILE	*fp;

	fp = fopen(path, "rb");
	if (!fp)
		return false;

	fclose(fp);
	return true;
}

SoundData *SoundLib::CheckForMatch(char *s)

{
	int	i;

	for (i = 1; i <= MAX_SOUNDS; i++) {
		if (sounds[i].matches(s))
			return sounds + i;
	}

	return 0;
}

int SoundLib::FindSlot()

{
	int	i;

	//
	// Orbitersound doesn't seem to like reusing slots much, so we won't do that until we run out
	// of slots. That won't happen too often unless you try to fly an entire mission without
	// quitting Orbiter and reloading.
	//

	if (NextSlot <= MAX_SOUNDS) {
		return NextSlot++;
	}

	//
	// If we've used all slots, look for an empty one.
	//

	for (i = 1; i <= MAX_SOUNDS; i++) {
		if (!(sounds[i].isValid()))
			return i;
	}

	//
	// Uh-oh, we're doomed.
	//

	return -1;
}

SoundData *SoundLib::DoLoadSound(char *SoundPath, EXTENDEDPLAY extended)

{
	SoundData *s;

	//
	// If the sound already exists, return it.
	//

	s = CheckForMatch(SoundPath);
	if (s) {
		s->AddRef();
		return s;
	}

	int id = FindSlot();
	if (id < 0)
		return 0;

	s = sounds + id;

	//
	// So the file exists and we have a free slot. Try to load it.
	//

	if (RequestLoadVesselWave3(SoundlibId, id, SoundPath, extended) == 0)
		return 0;


	s->setSoundlibId(SoundlibId);
	s->setID(id);
	s->setFileName(SoundPath);
	s->MakeValid();
	s->AddRef();

	return s;
}

//
// This is used to load generic NCPP sound files. If they're not over-ridden by a file in the language-specific
// directory, they'll use the file from the NCPP directory. If that fails, they'll finally look in the Vessel
// directory.
//

void SoundLib::LoadSound(Sound &s, char *soundname, EXTENDEDPLAY extended)

{

	if (!OrbiterSoundActive) {
		s.SetSoundData(0);
		return;
	}

	s.SetSoundLib(this);

	char SoundPath[256];

	//
	// First check for a language-specific sound.
	//

	_snprintf(SoundPath, 255, "%s/%s/%s", basepath, languagepath, soundname);
	if (IsFile(SoundPath)) {
		s.SetSoundData(DoLoadSound(SoundPath, extended));
		return;
	}

	//
	// Then check for the generic file.
	//

	_snprintf(SoundPath, 255, "%s/%s", basepath, soundname);
	if (IsFile(SoundPath)) {
		s.SetSoundData(DoLoadSound(SoundPath, extended));
		return;
	}

	//
	// Finally check the Vessel directory.
	//

	_snprintf(SoundPath, 255, "Sound/Vessel/%s", soundname);
	if (IsFile(SoundPath)) {
		s.SetSoundData(DoLoadSound(SoundPath, extended));
		return;
	}

	s.SetSoundData(0);
}

//
// LoadMissionSound() looks for a mission-specific sound file. If it's not in the
// language-specific mission directory and not in the generic NCPP mission directory,
// then it will instead try to load the generic sound file if it's passed in.
//

void SoundLib::LoadMissionSound(Sound &s, char *soundname, char *genericname, EXTENDEDPLAY extended)

{
	char	SoundPath[256];

	if (!OrbiterSoundActive) {
		s.SetSoundData(0);
		return;
	}

	s.SetSoundLib(this);

	//
	// First check for a language-specific sound.
	//

	_snprintf(SoundPath, 255, "%s/%s/%s/%s", basepath, languagepath, missionpath, soundname);
	if (IsFile(SoundPath)) {
		s.SetSoundData(DoLoadSound(SoundPath, extended));
		return;
	}

	//
	// Then check for the generic file.
	//

	_snprintf(SoundPath, 255, "%s/%s/%s", basepath, missionpath, soundname);
	if (IsFile(SoundPath)) {
		s.SetSoundData(DoLoadSound(SoundPath, extended));
		return;
	}

	//
	// If none of the files exist and we're passed a generic alternative, try that.
	//

	if (genericname) {
		LoadSound(s, genericname, extended);
		return;
	}

	//
	// Nothing. Return a dummy.
	//

	s.SetSoundData(0);
}

//
// Load a sound from the generic Vessel directory. You shouldn't call this without a good reason.
//

void SoundLib::LoadVesselSound(Sound &s, char *soundname, EXTENDEDPLAY extended)

{
	char	SoundPath[256];

	if (!OrbiterSoundActive) {
		s.SetSoundData(0);
		return;
	}

	s.SetSoundLib(this);

	_snprintf(SoundPath, 255, "Vessel/%s", soundname);
	s.SetSoundData(DoLoadSound(SoundPath, extended));
}

void SoundLib::SoundOptionOnOff(int option, int onoff)

{
	SoundOptionOnOff3(SoundlibId, option, onoff);
}

void SoundLib::SetLanguage(char *language)

{
	strncpy(languagepath, language, 255);
}

//
// Adjust the volume passed in based on the master volume controls.
//

int SoundLib::GetSoundVolume(int flags, int volume)

{
	if (flags & SOUNDFLAG_COMMS) {
		//
		// There are two master volume controls, so use the
		// highest volume set.
		//
		int mv = MasterVolume[VOLUME_COMMS];
		if (MasterVolume[VOLUME_COMMS2] > mv)
			mv = MasterVolume[VOLUME_COMMS2];

		volume = (volume * mv) / 100;
	}

	return volume;
}

//
// Update volume.
//

void SoundLib::SetVolume(int type, int percent)

{
	//
	// We'll be nice if the volume is invalid.
	//

	if (percent > 100)
		percent = 100;
	if (percent < 0)
		percent = 0;

	//
	// Return if the type is invalid.
	//

	if (type < 0)
		return;
	if (type >= N_VOLUMES)
		return;

	//
	// Store volume as a percentage of max.
	//

	MasterVolume[type] = percent;

#if 0

	//
	// Try to update any sounds that are currently playing.
	//

	//
	// Unfortunately this doesn't work here, even though it does work for
	// the cabin fans... maybe only looped sounds let you change the volume?
	//

	int match_flags = 0;

	switch (type) {
	case VOLUME_COMMS:
		match_flags = SOUNDFLAG_COMMS;
		break;

	default:
		return;
	}

	for (int i = 1; i <= MAX_SOUNDS; i++) {
		if (sounds[i].isValid() && sounds[i].isPlaying()) {
			int libflags = sounds[i].GetLibFlags();
			if (libflags & match_flags) {
				int base_vol = sounds[i].GetBaseVolume();
				int playflags = sounds[i].GetPlayFlags();
				int vol = GetSoundVolume(libflags, base_vol);

				sounds[i].play(playflags, libflags, base_vol, vol);
			}
		}
	}
#endif
}

Sound::Sound()

{
	sd = 0;
	sl = 0;
	valid = false;
	soundflags = 0;
}

Sound::Sound(Sound &s)

{
	sd = s.sd;
	sl = s.sl;

	valid = s.valid;

	if (sd)
		sd->AddRef();
}

Sound::~Sound()

{
	if (sd && valid && sd->isValid()) {
		sd->done();
	}
}

void Sound::setFlags(int fl)

{
	soundflags |= fl;
}

void Sound::clearFlags(int fl)

{
	soundflags &= ~fl;
}

bool Sound::isValid()

{
	bool	v;

	v = valid;
	if (sd) {
		if (!sd->isValid())
			v = false;
	}

	return v;
}

bool Sound::isPlaying()

{
	if (!sd || !valid) {
		return false;
	}

	return sd->isPlaying();
}

void Sound::play(int flags, int volume)

{
	if (valid && sd && sd->isValid()) {
		if (soundflags & SOUNDFLAG_1XORLESS) {
			if (oapiGetTimeAcceleration() > 1.0)
				return;
		}
		if (soundflags & SOUNDFLAG_1XONLY) {
			double acc = oapiGetTimeAcceleration();
			if (acc > 1.01 || acc < 0.99)
				return;
		}

		int vol = volume;

		if (sl) {
			vol = sl->GetSoundVolume(soundflags, volume);
		}

		sd->play(flags, soundflags, volume, vol);
	}
}

void Sound::stop()

{
	if (valid && sd && sd->isValid())
		sd->stop();
}

void Sound::done()

{
	if (valid && sd && sd->isValid())
		sd->done();

	valid = false;
	sd = 0;
}

void Sound::SetSoundData(SoundData *s)

{
	if (valid && sd && sd->isValid())
		sd->done();

	sd = s;

	if (s) {
		valid = s->isValid();
	}
	else {
		valid = false;
	}
}
