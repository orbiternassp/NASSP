/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2006 

  Project Apollo MFD

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

#ifndef __PROJECTAPOLLOMFD_H
#define __PROJECTAPOLLOMFD_H

///
/// \ingroup MFD
///
class ProjectApolloMFD: public MFD {
public:
	ProjectApolloMFD (DWORD w, DWORD h, VESSEL *vessel);
	~ProjectApolloMFD ();
	char *ButtonLabel (int bt);
	int ButtonMenu (const MFDBUTTONMENU **menu) const;
	bool ConsumeButton (int bt, int event);
	bool ConsumeKeyBuffered (DWORD key);
	void Update (HDC hDC);
	void GetStateVector (void);
	bool SetSource(char *rstr);
	bool SetReferencePlanet(char *rstr);
	bool SetCrewNumber (char *rstr);
	bool SetPrimECSTestHeaterPower (char *rstr);
	bool SetSecECSTestHeaterPower (char *rstr);
	void CalculateV42Angles(void);

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
	LEM *lem;
	int screen;
	bool debug_frozen;

	static struct ScreenData {
		int screen;
	} screenData;
};

#endif // !__PROJECTAPOLLOMFD_H
