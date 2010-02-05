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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.5  2009/09/26 21:49:38  coussini
  *	This is Coussini MET (Mission Elapsed Time) for checklist MFD
  *	
  *	Revision 1.4  2009/09/26 18:57:07  coussini
  *	This is Coussini MET (Mission Elapsed Time) for checklist MFD
  *	
  *	Revision 1.3  2009/09/17 17:48:41  tschachim
  *	DSKY support and enhancements of ChecklistMFD / ChecklistController
  *	
  *	Revision 1.2  2009/08/17 13:27:49  tschachim
  *	Enhancement of ChecklistMFD
  *	
  *	Revision 1.1  2009/02/18 23:21:34  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.9  2008/12/05 13:05:47  tschachim
  *	New info screen by Coussini.
  *	
  *	Revision 1.8  2008/06/28 10:51:30  jasonims
  *	Updated ChecklistMFD interface to display event times.... only MISSION_TIME and LAST_ITEM_RELATIVE implemented.
  *	
  *	Revision 1.7  2008/06/17 16:39:07  tschachim
  *	Moved prime crew ingress to T-2h40min, bugfixes checklists.
  *	
  *	Revision 1.6  2008/04/11 11:49:26  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
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