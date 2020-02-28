/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  Launch Control Center MFD (Header)

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

#include "MFDAPI.h"
#include "LCC.h"
#include "LCCMFDButtons.h"

class LCC_MFD :public MFD2
{
private:
	LCC* pLCC;

public:
	LCC_MFD(DWORD w, DWORD h, VESSEL* v);
	~LCC_MFD(void);

	bool Update(oapi::Sketchpad* skp);
	char* ButtonLabel(int bt);
	int ButtonMenu(const MFDBUTTONMENU** menu) const;
	bool ConsumeKeyBuffered(DWORD key);
	bool ConsumeButton(int bt, int event);

	void menuVoid() {}
	void menuSetSLCCDiscreteInput();

	void set_SLCCDiscreteInput(int n, bool set);

	static int MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);
protected:
	const LCCMFDButtons m_buttonPages;

	int screen;
};