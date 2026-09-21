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

#ifndef __GROUNDELAPSEDTIMEMFD_H
#define __GROUNDELAPSEDTIMEMFD_H

#include <string>

class Saturn;
class Crawler;
class LEM;
class MCC;

///
/// A small, self-contained MFD that displays only the Ground Elapsed Time
/// (GET, also known as Mission Elapsed Time) of the currently focused
/// Apollo vessel.
///
/// \ingroup MFD
///
class GroundElapsedTimeMFD : public MFD2 {
public:
	GroundElapsedTimeMFD(DWORD w, DWORD h, VESSEL *vessel);
	~GroundElapsedTimeMFD();

	char *ButtonLabel(int bt);
	int ButtonMenu(const MFDBUTTONMENU **menu) const;
	bool ConsumeKeyBuffered(DWORD key);
	bool Update(oapi::Sketchpad *skp);

	static int MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

protected:
	std::string GetGroundElapsedTimeString(void) const;

	DWORD width;
	DWORD height;

	// We can read the Ground Elapsed Time from any of these vessel types.
	// Only one of them (at most) will be non-NULL for a given vessel.
	Saturn *saturn;
	Crawler *crawler;
	LEM *lem;
	MCC *mcc;
};

#endif // !__GROUNDELAPSEDTIMEMFD_H
