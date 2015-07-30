/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ML vessel

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
  *	Revision 1.4  2007/03/01 18:24:32  tschachim
  *	Fixed Saturn V assembly
  *	
  *	Revision 1.3  2006/06/26 19:05:36  movieman523
  *	More doxygen, made Lunar EVA a VESSEL2, made SM breakup, made LRV use VESSEL2 save/load functions.
  *	
  *	Revision 1.2  2006/05/19 13:48:28  tschachim
  *	Fixed a lot of devices and power consumptions.
  *	DirectO2 valve added.
  *	
  *	Revision 1.1  2006/04/25 14:00:22  tschachim
  *	New KSC.
  *	
  **************************************************************************/

///
/// \ingroup Ground
///
class ML: public VESSEL2 {

public:
	ML(OBJHANDLE hObj, int fmodel);
	virtual ~ML();

	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkPostCreation();
	void clbkLoadStateEx(FILEHANDLE scn, void *status);
	void clbkSaveState(FILEHANDLE scn);
	int clbkConsumeDirectKey(char *kstate);
	int clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkPostStep(double simt, double simdt, double mjd);

	virtual void SetVABBuildState();
	virtual void SetVABReadyState();
	virtual bool Detach();
	virtual bool Attach();
	virtual bool IsInVAB(); 


protected:
	bool firstTimestepDone;
	int meshindexML;
	bool moveToPad;
	bool moveToVab;
	bool moveLVToPad;
	double touchdownPointHeight;
	char LVName[256];
	SoundLib soundlib;
	OBJHANDLE hLV;
	int state;

	UINT craneAnim;
	UINT cmarmAnim;
	UINT s1cintertankarmAnim;
	UINT s1cforwardarmAnim;
	UINT swingarmAnim;
	UINT mastAnim;
	double craneProc;
	double cmarmProc;
	double s1cintertankarmProc;
	double s1cforwardarmProc;
	double swingarmProc;
	double mastProc;

	PSTREAM_HANDLE liftoffStream[2];
	double liftoffStreamLevel;

	void DoFirstTimestep();
	double GetDistanceTo(double lon, double lat);
	void SetTouchdownPointHeight(double height);
	void DefineAnimations();
};
