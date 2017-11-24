/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2004-2005



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

class Sat5LMDSC : public VESSEL3 {

public:

	Sat5LMDSC(OBJHANDLE hObj, int fmodel);
	virtual ~Sat5LMDSC();
	void init();
	void Setup();
	virtual void SetState(int stage);
	void clbkSaveState(FILEHANDLE scn);
	void clbkLoadStateEx(FILEHANDLE scn, void *status);
	void clbkSetClassCaps(FILEHANDLE cfg);

protected:

	int state;

};
