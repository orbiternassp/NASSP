/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2024

  Autosave functionality (Header)

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

#pragma once

#include <OrbiterAPI.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <direct.h>

// Reads config from Saturn5.launchpad.cfg. Call Update() from clbkPreStep.
class NASSPAutosave {
public:
	static void Update(OBJHANDLE vesselHandle, const char* vesselName, const char* missionName, double missionTime)
	{
		if (oapiGetFocusObject() != vesselHandle)
			return;

		State& s = GetState();

		if (!s.initialized) {
			LoadConfig(s);
			s.lastSaveTime = std::chrono::steady_clock::now();
			s.initialized = true;
		}

		if (!s.enabled)
			return;

		// Clear notification after timeout
		double simt = oapiGetSimTime();
		if (s.hNote && s.notificationEndTime > 0 && simt > s.notificationEndTime) {
			oapiAnnotationSetText(s.hNote, "");
			s.notificationEndTime = 0;
		}

		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - s.lastSaveTime);

		if (elapsed.count() >= s.intervalMinutes * 60) {
			DoSave(s, vesselName, missionName, missionTime);
			s.lastSaveTime = now;
		}
	}

private:
	struct State {
		bool enabled = false;
		int intervalMinutes = 10;
		bool showNotification = true;
		std::chrono::steady_clock::time_point lastSaveTime;
		bool initialized = false;
		double notificationEndTime = 0;
		NOTEHANDLE hNote = nullptr;
	};

	static State& GetState()
	{
		static State state;
		return state;
	}

	static void LoadConfig(State& s)
	{
		FILEHANDLE hFile = oapiOpenFile("ProjectApollo/Saturn5.launchpad.cfg", FILE_IN, CONFIG);
		if (!hFile) return;

		char *line;
		while (oapiReadScenario_nextline(hFile, line)) {
			if (!_strnicmp(line, "AUTOSAVE_ENABLED", 16)) {
				int val = 0;
				sscanf(line + 16, "%d", &val);
				s.enabled = (val != 0);
			}
			else if (!_strnicmp(line, "AUTOSAVE_INTERVAL", 17)) {
				sscanf(line + 17, "%d", &s.intervalMinutes);
			}
			else if (!_strnicmp(line, "AUTOSAVE_NOTIFICATION", 21)) {
				int val = 1;
				sscanf(line + 21, "%d", &val);
				s.showNotification = (val != 0);
			}
		}
		oapiCloseFile(hFile, FILE_IN);
	}

	static void DoSave(State& s, const char* vesselName, const char* missionName, double missionTime)
	{
		int totalSeconds = (int)missionTime;
		int hours = totalSeconds / 3600;
		int minutes = (totalSeconds % 3600) / 60;
		int seconds = totalSeconds % 60;

		const char* folderName = (missionName && missionName[0] != '\0') ? missionName : vesselName;

		char getStr[64];
		sprintf(getStr, "GET-%03d.%02d.%02d", hours, minutes, seconds);

		// oapiSaveScenario won't create directories
		char scenarioDir[512];
		sprintf(scenarioDir, "Scenarios\\Autosave");
		_mkdir(scenarioDir);
		sprintf(scenarioDir, "Scenarios\\Autosave\\%s", folderName);
		_mkdir(scenarioDir);

		char fullPath[512];
		sprintf(fullPath, "Autosave/%s/%s", folderName, getStr);

		char description[256];
		if (missionName && missionName[0] != '\0') {
			sprintf(description, "NASSP Autosave - %s", missionName);
		} else {
			sprintf(description, "NASSP Autosave");
		}

		oapiSaveScenario(fullPath, description);

		if (s.showNotification) {
			if (!s.hNote) {
				s.hNote = oapiCreateAnnotation(false, 0.65, _V(0.0, 1.0, 0.5));
				oapiAnnotationSetPos(s.hNote, 0.02, 0.15, 0.4, 0.2);
			}
			char msg[256];
			sprintf(msg, "Autosaved: %s/%s", folderName, getStr);
			oapiAnnotationSetText(s.hNote, msg);
			s.notificationEndTime = oapiGetSimTime() + 3.0;
		}
	}
};
