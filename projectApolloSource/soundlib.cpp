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
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <string.h>
#include "OrbiterSoundSDK3.h"
#include "resource.h"

#include "soundlib.h"

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

void SoundData::play(int flags, int volume)

{
	if (valid) {
		PlayVesselWave3(SoundlibId, id, flags, volume);
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
	for (int i = 0; i <= MAX_SOUNDS; i++) {
		sounds[i].MakeInvalid();
	}

	SoundlibId = 0;
	OrbiterSoundActive = 0;
	missionpath[0] = 0;
	basepath[0] = 0;
	strcpy(languagepath, "English");
}

SoundLib::~SoundLib()

{
	//
	// Nothing for now.
	//
}

void SoundLib::InitSoundLib(OBJHANDLE h, char *soundclass)

{
	_snprintf(basepath, 255, "Sound\\%s", soundclass);

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

	for (i = 1; i <= MAX_SOUNDS; i++) {
		if (!(sounds[i].isValid()))
			return i;
	}

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

	char SoundPath[256];

	//
	// First check for a language-specific sound.
	//

	_snprintf(SoundPath, 255, "%s\\%s\\%s", basepath, languagepath, soundname);
	if (IsFile(SoundPath)) {
		s.SetSoundData(DoLoadSound(SoundPath, extended));
		return;
	}

	//
	// Then check for the generic file.
	//

	_snprintf(SoundPath, 255, "%s\\%s", basepath, soundname);
	if (IsFile(SoundPath)) {
		s.SetSoundData(DoLoadSound(SoundPath, extended));
		return;
	}

	//
	// Finally check the Vessel directory.
	//

	_snprintf(SoundPath, 255, "Vessel\\%s", soundname);
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

	//
	// First check for a language-specific sound.
	//

	_snprintf(SoundPath, 255, "%s\\%s\\%s\\%s", basepath, languagepath, missionpath, soundname);
	if (IsFile(SoundPath)) {
		s.SetSoundData(DoLoadSound(SoundPath, extended));
		return;
	}

	//
	// Then check for the generic file.
	//

	_snprintf(SoundPath, 255, "%s\\%s\\%s", basepath, missionpath, soundname);
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

	_snprintf(SoundPath, 255, "Vessel\\%s", soundname);
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

Sound::Sound()

{
	sd = 0;
	valid = false;
	soundflags = 0;
}

Sound::Sound(Sound &s)

{
	sd = s.sd;
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
		sd->play(flags, volume);
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
