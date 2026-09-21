/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2024

  Ground Elapsed Time MFD

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

#define STRICT
#define ORBITER_MODULE

// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"

#include "math.h"
#include "windows.h"
#include "nassputils.h"
#include "saturn.h"
#include "LEM.h"
#include "Crawler.h"
#include "mccvessel.h"
#include "mcc.h"

#include <stdio.h>
#include <string>

#include "GroundElapsedTimeMFD.h"

using namespace nassp;

static int g_hMFD;

DLLCLBK void InitModule(HINSTANCE hDLL)
{
	static char *name = "Ground Elapsed Time";
	MFDMODESPECEX spec;
	spec.key = OAPI_KEY_G;
	spec.name = name;
	spec.context = NULL;
	spec.msgproc = GroundElapsedTimeMFD::MsgProc;
	g_hMFD = oapiRegisterMFDMode(spec);
}

DLLCLBK void ExitModule(HINSTANCE hDLL)
{
	oapiUnregisterMFDMode(g_hMFD);
}

GroundElapsedTimeMFD::GroundElapsedTimeMFD(DWORD w, DWORD h, VESSEL *vessel) : MFD2(w, h, vessel)
{
	width = w;
	height = h;
	saturn = NULL;
	crawler = NULL;
	lem = NULL;
	mcc = NULL;

	// Find out what type of vessel is showing this MFD, so we know which
	// class to use to read the Ground Elapsed Time from.
	if (utils::IsVessel(vessel, utils::Saturn)) {
		saturn = (Saturn *)vessel;
	}
	else if (utils::IsVessel(vessel, utils::Crawler)) {
		crawler = (Crawler *)vessel;
	}
	else if (utils::IsVessel(vessel, utils::LEM)) {
		lem = (LEM *)vessel;
	}

	// The Mission Control vessel, if present, keeps its own copy of the
	// Ground Elapsed Time and is the most reliable source once it exists.
	OBJHANDLE hMCC = oapiGetVesselByName("MCC");
	if (hMCC != NULL) {
		VESSEL *pVessel = oapiGetVesselInterface(hMCC);
		if (pVessel && utils::IsVessel(pVessel, utils::MCC)) {
			MCCVessel *pMCCVessel = static_cast<MCCVessel *>(pVessel);
			if (pMCCVessel->mcc) {
				mcc = pMCCVessel->mcc;
			}
		}
	}
}

GroundElapsedTimeMFD::~GroundElapsedTimeMFD()
{
}

char *GroundElapsedTimeMFD::ButtonLabel(int bt)
{
	// No buttons; this MFD only ever shows one screen.
	return 0;
}

int GroundElapsedTimeMFD::ButtonMenu(const MFDBUTTONMENU **menu) const
{
	return 0;
}

bool GroundElapsedTimeMFD::ConsumeKeyBuffered(DWORD key)
{
	return false;
}

std::string GroundElapsedTimeMFD::GetGroundElapsedTimeString(void) const
{
	double get = 0.0;

	// MCC's Ground Elapsed Time is authoritative once available, but it
	// reads zero (or negative) before liftoff, so fall back to the
	// vessel's own clock in that case.
	if (mcc) {
		get = mcc->GetMissionTime();
	}

	if (get <= 0.0) {
		if (saturn) { get = saturn->GetMissionTime(); }
		else if (crawler) { get = crawler->GetMissionTime(); }
		else if (lem) { get = lem->GetMissionTime(); }
	}

	int secs = (int)fabs(get);
	int hours = secs / 3600;
	secs -= hours * 3600;
	int minutes = secs / 60;
	secs -= minutes * 60;

	char buf[32];
	sprintf_s(buf, sizeof(buf), "%s%03d:%02d:%02d", (get < 0.0) ? "-" : "", hours, minutes, secs);
	return std::string(buf);
}

bool GroundElapsedTimeMFD::Update(oapi::Sketchpad *skp)
{
	Title(skp, "Ground Elapsed Time");

	skp->SetFont(GetDefaultFont(0));
	skp->SetTextColor(RGB(0, 255, 0));
	skp->SetTextAlign(oapi::Sketchpad::CENTER);

	static char *label = "GET (HHH:MM:SS)";
	skp->Text((int)(width * 0.5), (int)(height * 0.35), label, strlen(label));

	std::string get = GetGroundElapsedTimeString();
	skp->Text((int)(width * 0.5), (int)(height * 0.50), get.c_str(), get.size());

	if (!saturn && !crawler && !lem) {
		static char *msg = "Unsupported vessel";
		skp->Text((int)(width * 0.5), (int)(height * 0.65), msg, strlen(msg));
	}

	return true;
}

int GroundElapsedTimeMFD::MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		return (int)(new GroundElapsedTimeMFD(LOWORD(wparam), HIWORD(wparam), (VESSEL *)lparam));
	}
	return 0;
}
