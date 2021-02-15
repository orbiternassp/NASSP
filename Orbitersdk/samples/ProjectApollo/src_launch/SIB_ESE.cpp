/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

Electrical Support Equipment for the S-IB Stage

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
#include "SIB_ESE.h"

SIB_ESE::SIB_ESE(SCMUmbilical *SCMUmb, LCCPadInterface *p)
{
	Umbilical = SCMUmb;
	Pad = p;

	for (int i = 0;i < 8;i++)
	{
		for (int j = 0;j < 3;j++)
		{
			SIBThrustOKSimulate[i][j] = false;
		}
	}
}

void SIB_ESE::Timestep()
{
	for (int i = 0;i < 5;i++)
	{
		for (int j = 0;j < 3;j++)
		{
			if (Pad->SLCCGetOutputSignal(778 + j + 3 * i))
			{
				SIBThrustOKSimulate[i][j] = true;
			}
			else
			{
				SIBThrustOKSimulate[i][j] = false;
			}
		}
	}

	if (Pad->SLCCGetOutputSignal(824))
		SIBThrustOKSimulate[5][0] = true;
	else
		SIBThrustOKSimulate[5][0] = false;

	if (Pad->SLCCGetOutputSignal(1032))
		SIBThrustOKSimulate[5][1] = true;
	else
		SIBThrustOKSimulate[5][1] = false;

	if (Pad->SLCCGetOutputSignal(1033))
		SIBThrustOKSimulate[5][2] = true;
	else
		SIBThrustOKSimulate[5][2] = false;

	if (Pad->SLCCGetOutputSignal(1034))
		SIBThrustOKSimulate[6][0] = true;
	else
		SIBThrustOKSimulate[6][0] = false;

	if (Pad->SLCCGetOutputSignal(1034))
		SIBThrustOKSimulate[6][1] = true;
	else
		SIBThrustOKSimulate[6][1] = false;

	if (Pad->SLCCGetOutputSignal(1052))
		SIBThrustOKSimulate[6][2] = true;
	else
		SIBThrustOKSimulate[6][2] = false;

	if (Pad->SLCCGetOutputSignal(1053))
		SIBThrustOKSimulate[7][0] = true;
	else
		SIBThrustOKSimulate[7][0] = false;

	if (Pad->SLCCGetOutputSignal(1054))
		SIBThrustOKSimulate[7][1] = true;
	else
		SIBThrustOKSimulate[7][1] = false;

	if (Pad->SLCCGetOutputSignal(1055))
		SIBThrustOKSimulate[7][2] = true;
	else
		SIBThrustOKSimulate[7][2] = false;
}

void SIB_ESE::SaveState(FILEHANDLE scn)
{

}

void SIB_ESE::LoadState(FILEHANDLE scn)
{

}