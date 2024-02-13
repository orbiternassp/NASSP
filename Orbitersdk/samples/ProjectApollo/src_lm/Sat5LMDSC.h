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

namespace mission
{
	class Mission;
};

class Sat5LMDSC : public VESSEL3 {

public:

	Sat5LMDSC(OBJHANDLE hObj, int fmodel);
	virtual ~Sat5LMDSC();
	void init();
	void Setup();
	void DefineAnimations(UINT idx);
	virtual void SetState(int stage, int mission);
	void clbkSaveState(FILEHANDLE scn);
	void clbkLoadStateEx(FILEHANDLE scn, void *status);
	void clbkSetClassCaps(FILEHANDLE cfg);
	void HideProbes();
	void HideDeflectors();
	void clbkVisualCreated(VISHANDLE vis, int refcount);
	void clbkVisualDestroyed(VISHANDLE vis, int refcount);

protected:

	int state; // 0 = no legs, 1 = gear retracted, 2 = gear extended w/probes, 3 = gear extended no probes
	int ApolloNo;
	THGROUP_HANDLE thg_sep;
	double ro1; // Distance from center of middle touchdown points
	double ro2; // Distance from center of lower (footpad) touchdown points
	UINT dscidx;
	DEVMESHHANDLE desstg_devmesh;

	UINT anim_Gear;
	MGROUP_TRANSFORM	*mgt_Leg[4], *mgt_Strut[4], *mgt_Downlock[4], *mgt_Probes1[3], *mgt_Probes2[3];

	mission::Mission* pMission;
};
