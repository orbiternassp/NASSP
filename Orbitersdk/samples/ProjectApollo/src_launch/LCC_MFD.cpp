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
	font = oapiCreateFont(w / 20, true, "Courier", FONT_NORMAL, 0);
	return;
}

LCC_MFD::~LCC_MFD(void)
{
	oapiReleaseFont(font);
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
	Title(skp, "Apollo LCC MFD");
	skp->SetFont(font);

	if (screen == 0)
	{
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(4 * W / 8, 2 * H / 14, "Checkout Programs", 17);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
	}

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

void LCC_MFD::menuLoadProgram()
{
	bool LoadCheckoutProgramInput(void *id, char *str, void *data);
	oapiOpenInputBox("Enter name of the checkout program:", LoadCheckoutProgramInput, 0, 20, (void*)this);
}

bool LoadCheckoutProgramInput(void *id, char *str, void *data)
{
	((LCC_MFD*)data)->LoadCheckoutProgram(str);
	return true;
}

void  LCC_MFD::LoadCheckoutProgram(const char *str)
{
	char str2[128];

	sprintf_s(str2, 128, "Config\\ProjectApollo\\Checkout\\%s.csv", str);

	pLCC->SLCCLoadProgram(str2);
}