/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

Electrical Support Equipment for the S-IB Stage (Header)

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

#pragma once

class SCMUmbilical;
class LCCPadInterface;

class SIB_ESE
{
public:
	SIB_ESE(SCMUmbilical *TSMUmb, LCCPadInterface *p);

	void Timestep();
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	bool GetSIBThrustOKSimulate(int eng, int n) { return SIBThrustOKSimulate[eng - 1][n - 1]; }
protected:
	bool SIBThrustOKSimulate[8][3];

	SCMUmbilical *Umbilical;
	LCCPadInterface *Pad;
};