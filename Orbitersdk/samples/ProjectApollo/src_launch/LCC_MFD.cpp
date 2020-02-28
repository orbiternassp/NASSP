/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  Launch Control Center MFD

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

#include "LCC_MFD.h"

LCC_MFD::LCC_MFD(DWORD w, DWORD h, VESSEL *v) :MFD2(w, h, v)
{
	pLCC = (LCC*)v;
	screen = 0;
	return;
}

LCC_MFD::~LCC_MFD(void)
{
	return;
}

int LCC_MFD::MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case OAPI_MSG_MFD_OPENED:
		return (int)(new LCC_MFD(LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}

bool LCC_MFD::Update(oapi::Sketchpad* skp)
{
	return true;
}

char* LCC_MFD::ButtonLabel(int bt)
{
	return m_buttonPages.ButtonLabel(bt);
}

int LCC_MFD::ButtonMenu(const MFDBUTTONMENU** menu) const
{
	return m_buttonPages.ButtonMenu(menu);
}
bool LCC_MFD::ConsumeKeyBuffered(DWORD key)
{
	return m_buttonPages.ConsumeKeyBuffered(this, key);
}

bool LCC_MFD::ConsumeButton(int bt, int event)
{
	return m_buttonPages.ConsumeButton(this, bt, event);
}

void LCC_MFD::menuSetSLCCDiscreteInput()
{
	bool SLCCDiscreteInputInput(void *id, char *str, void *data);
	oapiOpenInputBox("Discrete Input:", SLCCDiscreteInputInput, 0, 20, (void*)this);
}

bool SLCCDiscreteInputInput(void *id, char *str, void *data)
{
	int n, on;

	if (sscanf(str, "%d %d", &n, &on) == 2)
	{
		((LCC_MFD*)data)->set_SLCCDiscreteInput(n, on == 0 ? false : true);
		return true;
	}

	return false;
}

void LCC_MFD::set_SLCCDiscreteInput(int n, bool set)
{
	pLCC->SetDiscreteInput(n, set);
}