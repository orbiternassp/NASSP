/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007

Float Bag vessel

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
  *	Revision 1.1  2007/07/17 14:33:13  tschachim
  *	Added entry and post landing stuff.
  *	
  **************************************************************************/

class FloatBag: public VESSEL2 {

public:
	FloatBag(OBJHANDLE hObj, int fmodel);
	virtual ~FloatBag();

	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkPostCreation();
	void clbkLoadStateEx(FILEHANDLE scn, void *status);
	void clbkSaveState(FILEHANDLE scn);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkPostStep(double simt, double simdt, double mjd);
	void clbkVisualCreated (VISHANDLE vis, int refcount);
	void clbkVisualDestroyed (VISHANDLE vis, int refcount);

	virtual void SetBagSize(int index, double size);
	virtual void ExtendBeacon() { extendBeacon = true; }
	virtual void SetBeaconLight(bool on, bool high);

protected:
	MESHHANDLE meshhandle;
	UINT anim1;  
	UINT anim2;  
	UINT anim3;  
	UINT animBeacon;  
			
	double proc1;
	double proc2;
	double proc3;
	double procBeacon;
	bool extendBeacon;
	BEACONLIGHTSPEC beaconLight;
};
