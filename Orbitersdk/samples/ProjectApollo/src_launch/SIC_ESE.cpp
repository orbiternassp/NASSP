/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

Electrical Support Equipment for the S-IC Stage

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

#include "Orbitersdk.h"
#include "TSMUmbilical.h"
#include "LCCPadInterface.h"
#include "SIC_ESE.h"

SIC_ESE::SIC_ESE(TSMUmbilical *TSMUmb, LCCPadInterface *p)
{
	Umbilical = TSMUmb;
	Pad = p;

	for (int i = 0;i < 5;i++)
	{
		for (int j = 0;j < 3;j++)
		{
			SICThrustOKSimulate[i][j] = false;
		}
	}
}

void SIC_ESE::Timestep()
{
	for (int i = 0;i < 5;i++)
	{
		for (int j = 0;j < 3;j++)
		{
			if (Pad->SLCCGetOutputSignal(778 + j + 3 * i))
			{
				SICThrustOKSimulate[i][j] = true;
			}
			else
			{
				SICThrustOKSimulate[i][j] = false;
			}
		}
	}
}

void SIC_ESE::SaveState(FILEHANDLE scn)
{

}

void SIC_ESE::LoadState(FILEHANDLE scn)
{

}