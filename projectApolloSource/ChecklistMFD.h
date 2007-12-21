/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2006 

  Checklist MFD header file

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

#ifndef __CHECKLISTMFD_H
#define __CHECKLISTMFD_H

#include "Connector.h"
#include "MFDConnector.h"

///
/// \ingroup MFD
///
class ChecklistMFD: public MFD {
public:
	ChecklistMFD (DWORD w, DWORD h, VESSEL *vessel);
	~ChecklistMFD ();
	char *ButtonLabel (int bt);
	int ButtonMenu (const MFDBUTTONMENU **menu) const;
	bool ConsumeButton (int bt, int event);
	bool ConsumeKeyBuffered (DWORD key);
	void Update (HDC hDC);
	bool SetCrewNumber (char *rstr);
	bool SetPrimECSTestHeaterPower (char *rstr);
	bool SetSecECSTestHeaterPower (char *rstr);
	
	void WriteStatus (FILEHANDLE scn) const;
	void ReadStatus (FILEHANDLE scn);
	void StoreStatus (void) const;
	void RecallStatus (void);
	
	static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

protected:
	HBITMAP hBmpLogo;
	DWORD width;
	DWORD height;
	//We can link to the Saturn OR the Crawler.  Only the Saturn has full funcionality.  The Crawler can ONLY read the mission time.
	Saturn *saturn;
	Crawler *crawler;
	int screen;
	bool debug_frozen;

	static struct ScreenData {
		int screen;
	} screenData;

	MFDConnector PanelConnector;
};

#endif // !__CHECKLISTMFD_H
