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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.7  2010/11/05 03:10:00  vrouleau
  *	Added CSM/LM Landing Site REFSMMAT Uplink to AGC. Contains hardcoded REFSMMAT per mission.
  *	
  *	Revision 1.6  2009/09/13 15:20:15  dseagrav
  *	LM Checkpoint Commit. Adds LM telemetry, fixed missing switch row init, initial LM support for PAMFD.
  *	
  *	Revision 1.5  2009/05/31 01:47:30  bluedragon8144
  *	removed ClockUpdate() declaration
  *	
  *	Revision 1.4  2009/05/04 22:59:39  bluedragon8144
  *	Renamed external dv to P30, renamed lambert back to P31, added updateclock
  *	
  *	Revision 1.3  2009/05/03 23:09:28  bluedragon8144
  *	Changed P31 to Lambert, removed unused P30 function
  *	
  *	Revision 1.2  2009/05/01 17:06:03  bluedragon8144
  *	removed DEC2OCT and irDEC2OCT (not used anymore)
  *	
  *	Revision 1.1  2009/02/18 23:21:34  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.10  2008/04/23 18:50:25  bluedragon8144
  *	Added telemetry window and vessel support for State Vector.  Cleaned up display.
  *	
  *	Revision 1.9  2008/04/18 21:26:23  bluedragon8144
  *	Added State Vector Update (beta) to GNC window
  *	
  *	Revision 1.8  2007/12/19 02:54:26  lassombra
  *	Added function to move debug strings to the MFD.
  *
  *	MFD can display, freeze, or clear the debug strings.
  *
  *	Use (Saturn *)saturn->debugString() just as you would use oapiDebugString.
  *
  *	oapiDebugString can be used to output important alerts to users still, but should remain clean otherwise.
  *
  *	Also, redirected all the debug statements from the Telecom to the mfd.
  *
  *	Revision 1.7  2007/12/16 00:47:54  lassombra
  *	Removed ability to use buttons/keystrokes when using any ship but a saturn.
  *	
  *	Revision 1.6  2007/12/15 19:48:26  lassombra
  *	Added functionality to allow ProjectApollo MFD to get mission time from the Crawler as well as the Saturn.  The Crawler actually extracts the mission time from the Saturn, no updates to scenario files needed.
  *	
  *	Revision 1.5  2007/12/04 20:26:29  tschachim
  *	IMFD5 communication including a new TLI for the S-IVB IU.
  *	Additional CSM panels.
  *	
  *	Revision 1.4  2007/10/18 00:23:17  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.3  2006/12/19 15:55:55  tschachim
  *	ECS test stuff, bugfixes.
  *	
  *	Revision 1.2  2006/06/27 11:34:25  tschachim
  *	Added status screen.
  *	
  *	Revision 1.1  2006/06/23 11:56:48  tschachim
  *	New Project Apollo MFD for TLI burn control.
  *	
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
	void UpLinkRefsmmat (void);	
	void GetStateVector (void);	
	void GetOtherVehiculeStateVector (void);	
	void UploadStateVector (VESSEL *vessel,double get);	
	void IMFDP30Uplink (void);
	void IMFDP31Uplink (void);
	bool SetSource(char *rstr);
	bool SetReferencePlanet(char *rstr);
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
	LEM *lem;
	int screen;
	bool debug_frozen;

	static struct ScreenData {
		int screen;
	} screenData;
};

#endif // !__PROJECTAPOLLOMFD_H
