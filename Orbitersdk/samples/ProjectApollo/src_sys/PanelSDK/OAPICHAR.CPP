/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2005 Radu Poenaru

  System & Panel SDK (SPSDK)

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
  *	Revision 1.1  2005/04/22 14:22:37  tschachim
  *	Initial version
  *	
  **************************************************************************/

int Compare(char* ln, char* trg)
{ if (ln)
   if (!strnicmp(ln,trg,strlen(trg)))
	    return 1;
return 0;
};

DWORD GetDWORDfromChar(char *name)
{
if (Compare(name,"OAPI_KEY_ESCAPE")) return		0x01;
else if (Compare(name,"OAPI_KEY_1")) return		0x02;
else if (Compare(name,"OAPI_KEY_2")) return		0x03;
else if (Compare(name,"OAPI_KEY_3")) return		0x04;
else if (Compare(name,"OAPI_KEY_4")) return		0x05;
else if (Compare(name,"OAPI_KEY_5")) return		0x06;
else if (Compare(name,"OAPI_KEY_6")) return		0x07;
else if (Compare(name,"OAPI_KEY_7")) return		0x08;
else if (Compare(name,"OAPI_KEY_9")) return		0x0A;
else if (Compare(name,"OAPI_KEY_0")) return		0x0B;
else if (Compare(name,"OAPI_KEY_MINUS")) return	0x0C; // on main keyboard
else if (Compare(name,"OAPI_KEY_EQUALS")) return			0x0D;
else if (Compare(name,"OAPI_KEY_BACK")) return			0x0E ; // backspace
else if (Compare(name,"OAPI_KEY_TAB")) return			0x0F;
else if (Compare(name,"OAPI_KEY_Q")) return				0x10;
else if (Compare(name,"OAPI_KEY_W")) return				0x11;
else if (Compare(name,"OAPI_KEY_E")) return				0x12;
else if (Compare(name,"OAPI_KEY_R")) return				0x13;
else if (Compare(name,"OAPI_KEY_T")) return				0x14;
else if (Compare(name,"OAPI_KEY_Y")) return				0x15;
else if (Compare(name,"OAPI_KEY_U")) return				0x16;
else if (Compare(name,"OAPI_KEY_I")) return				0x17;
else if (Compare(name,"OAPI_KEY_O")) return				0x18;
else if (Compare(name,"OAPI_KEY_P")) return				0x19;
else if (Compare(name,"OAPI_KEY_LBRACKET")) return		0x1A;
else if (Compare(name,"OAPI_KEY_RBRACKET")) return		0x1B;
else if (Compare(name,"OAPI_KEY_RETURN")) return			0x1C;  // Enter on main keyboard
else if (Compare(name,"OAPI_KEY_LCONTROL")) return		0x1D;
else if (Compare(name,"OAPI_KEY_A")) return				0x1E;
else if (Compare(name,"OAPI_KEY_S")) return				0x1F;
else if (Compare(name,"OAPI_KEY_D")) return				0x20;
else if (Compare(name,"OAPI_KEY_F")) return				0x21;
else if (Compare(name,"OAPI_KEY_G")) return				0x22;
else if (Compare(name,"OAPI_KEY_H")) return				0x23;
else if (Compare(name,"OAPI_KEY_J")) return				0x24;
else if (Compare(name,"OAPI_KEY_K")) return				0x25;
else if (Compare(name,"OAPI_KEY_L")) return				0x26;
else if (Compare(name,"OAPI_KEY_SEMICOLON")) return		0x27;
else if (Compare(name,"OAPI_KEY_APOSTROPHE")) return		0x28;
else if (Compare(name,"OAPI_KEY_GRAVE")) return			0x29;  // accent grave
else if (Compare(name,"OAPI_KEY_LSHIFT")) return			0x2A;
else if (Compare(name,"OAPI_KEY_BACKSLASH")) return		0x2B;
else if (Compare(name,"OAPI_KEY_Z")) return				0x2C;
else if (Compare(name,"OAPI_KEY_X")) return				0x2D;
else if (Compare(name,"OAPI_KEY_C")) return				0x2E;
else if (Compare(name,"OAPI_KEY_V")) return				0x2F;
else if (Compare(name,"OAPI_KEY_B")) return				0x30;
else if (Compare(name,"OAPI_KEY_N")) return				0x31;
else if (Compare(name,"OAPI_KEY_M")) return				0x32;
else if (Compare(name,"OAPI_KEY_COMMA")) return			0x33;
else if (Compare(name,"OAPI_KEY_PERIOD")) return			0x34;  // . on main keyboard
else if (Compare(name,"OAPI_KEY_SLASH")) return			0x35;  // / on main keyboard
else if (Compare(name,"OAPI_KEY_RSHIFT")) return			0x36;
else if (Compare(name,"OAPI_KEY_MULTIPLY")) return		0x37;  // * on numeric keypad
else if (Compare(name,"OAPI_KEY_LALT")) return			0x38;  // left Alt
else if (Compare(name,"OAPI_KEY_SPACE")) return			0x39;
else if (Compare(name,"OAPI_KEY_CAPITAL")) return	0x3A;  // caps lock key
else if (Compare(name,"OAPI_KEY_F1")) return				0x3B;
else if (Compare(name,"OAPI_KEY_F2")) return				0x3C;
else if (Compare(name,"OAPI_KEY_F3")) return				0x3D;
else if (Compare(name,"OAPI_KEY_F4")) return				0x3E;
else if (Compare(name,"OAPI_KEY_F5")) return				0x3F;
else if (Compare(name,"OAPI_KEY_F6")) return				0x40;
else if (Compare(name,"OAPI_KEY_F7")) return				0x41;
else if (Compare(name,"OAPI_KEY_F8")) return				0x42;
else if (Compare(name,"OAPI_KEY_F9")) return				0x43;
else if (Compare(name,"OAPI_KEY_F10")) return		0x44;
else if (Compare(name,"OAPI_KEY_NUMLOCK")) return		0x45;
else if (Compare(name,"OAPI_KEY_SCROLL")) return			0x46;  // Scroll lock
else if (Compare(name,"OAPI_KEY_NUMPAD7")) return		0x47;
else if (Compare(name,"OAPI_KEY_NUMPAD8")) return		0x48;
else if (Compare(name,"OAPI_KEY_NUMPAD9")) return		0x49;
else if (Compare(name,"OPAI_KEY_SUBTRACT")) return		0x4A ; // - on numeric keypad
else if (Compare(name,"OAPI_KEY_NUMPAD4")) return		0x4B;
else if (Compare(name,"OAPI_KEY_NUMPAD5")) return		0x4C;
else if (Compare(name,"OAPI_KEY_NUMPAD6")) return		0x4D;
else if (Compare(name,"OAPI_KEY_ADD")) return			0x4E ; // + on numeric keypad
else if (Compare(name,"OAPI_KEY_NUMPAD1")) return		0x4F;
else if (Compare(name,"OAPI_KEY_NUMPAD2")) return		0x50;
else if (Compare(name,"OAPI_KEY_NUMPAD3")) return		0x51;
else if (Compare(name,"OAPI_KEY_NUMPAD0")) return		0x52;
else if (Compare(name,"OAPI_KEY_DECIMAL")) return		0x53;  // . on numeric keypad
else if (Compare(name,"OAPI_KEY_OEM_102")) return	0x56;  // | < > on UK/German keyboards
else if (Compare(name,"OAPI_KEY_F11")) return			0x57;
else if (Compare(name,"OAPI_KEY_F12")) return			0x58;
else if (Compare(name,"OAPI_KEY_NUMPADENTER")) return	0x9C ;   // Enter on numeric keypad
else if (Compare(name,"OAPI_KEY_RCONTROL")) return		0x9D;
else if (Compare(name,"OAPI_KEY_DIVIDE")) return			0xB5;    // / on numeric keypad
else if (Compare(name,"OAPI_KEY_RALT")) return           0xB8;    // right Alt
return 0;
}