/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2008

  Project Apollo Checklist MFD

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

#include "Connector.h"
#include "MFDConnector.h"

class ProjectApolloChecklistMFD: public MFD {
public:
	ProjectApolloChecklistMFD (DWORD w, DWORD h, VESSEL *vessel);
	~ProjectApolloChecklistMFD ();
	char *ButtonLabel (int bt);
	int ButtonMenu (const MFDBUTTONMENU **menu) const;
	bool ConsumeButton (int bt, int event);
	bool ConsumeKeyBuffered (DWORD key);
	void Update (HDC hDC);
	void WriteStatus (FILEHANDLE scn) const;
	void ReadStatus (FILEHANDLE scn);
	void StoreStatus (void) const;
	void RecallStatus (void);
	void substituteVariables(char *buffer,int buflen);

	static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

	MFDConnector conn;
	vector<ChecklistGroup> groups;
	DWORD width;
	DWORD height;

protected:
	int screen;

	bool MFDInit; // MFD has been initialized, including retrieving Available Checklists
	int NumChkLsts;
	bool ChkLstAutoOn;

	//We can link to the Saturn OR the Crawler.  Only the Saturn has full funcionality.  The Crawler can ONLY read the mission time.
	Saturn *saturn;
	Crawler *crawler;
	LEM *lem;

	int cnt;
	VECTOR3 temptime; //x is hr, y is min, z is sec
	std::string line;	

	int TopStep;
	int CurrentStep;
	int HiLghtdLine;
	bool HiLghtdLineDown;
	HBITMAP hBmpLogo;

	std::string DisplayChecklistMissionTime (ChecklistItem *item);
	bool bDisplayMET; // Coussini 
	std::string DisplayMissionElapsedTime (void); // Coussini
};
