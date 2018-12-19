/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

CSM Rotational Hand Controller (Header)

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

class RHC
{
public:
	RHC();
	void Timestep(int* rhc_pos, bool normdc, bool normac, bool dirdcab, bool dirdcredun);
protected:

	VECTOR3 deflection;
	VECTOR3 PropRate;
	bool PlusBO[3];
	bool MinusBO[3];
	bool PlusHO[3];
	bool MinusHO[3];
	bool PlusHO2[3];
	bool MinusHO2[3];
};