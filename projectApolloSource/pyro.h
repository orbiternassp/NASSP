/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: NASSP pyro simulation.

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
  **************************** Revision History ****************************/

#ifndef __PYRO_H
#define __PYRO_H

class Pyro : public e_object {

public:
	Pyro(char *i_name, PanelSDK &p);
	bool Blown() { return blown; };
	void SetBlown(bool b) { blown = b; };
	void DrawPower(double watts);
	void refresh(double dt);
	void UpdateFlow(double dt);

	void Load(char *line);
	void Save(FILEHANDLE scn);

protected:
	double energy;
	bool blown;
	PanelSDK &sdk;

};

#endif
