/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  Launch Control Center MFD Buttons

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
#include "LCCMFDButtons.h"

LCCMFDButtons::LCCMFDButtons()
{
	static const MFDBUTTONMENU mnu0[] =
	{
		{ "Discrete Input", 0, 'I' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
	};

	RegisterPage(mnu0, sizeof(mnu0) / sizeof(MFDBUTTONMENU));

	RegisterFunction("INP", OAPI_KEY_I, &LCC_MFD::menuSetSLCCDiscreteInput);
	RegisterFunction("LOA", OAPI_KEY_P, &LCC_MFD::menuLoadProgram);
	RegisterFunction("", OAPI_KEY_U, &LCC_MFD::menuVoid);
	RegisterFunction("", OAPI_KEY_M, &LCC_MFD::menuVoid);
	RegisterFunction("", OAPI_KEY_B, &LCC_MFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &LCC_MFD::menuVoid);

	RegisterFunction("", OAPI_KEY_U, &LCC_MFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &LCC_MFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &LCC_MFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &LCC_MFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &LCC_MFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &LCC_MFD::menuVoid);
}

bool LCCMFDButtons::SearchForKeysInOtherPages() const
{
	return false;
}