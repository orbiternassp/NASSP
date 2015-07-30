/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2005 Chris Knestrick

  Code to calculate the entry interface paramters

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
  *	Revision 1.1  2005/02/11 12:17:54  tschachim
  *	Initial version
  *	
  **************************************************************************/

#ifndef EI_H
#define EI_H

struct ENTRY_INTERFACE {
	bool InterfaceDefined;	// Indicates if the interface is defined.  This is used to
							// indicate the return status of CalcEntryInterface()

	double Angle;			// Angle between the velocity vector and the local horizon
	double TimeToGo;		// Time until entry interface
	VECTOR3 Pos, Vel;		// The predicted position and velocity vectors at entry interface
};

void CalcEntryInterface(ENTRY_INTERFACE &Interface);

#endif