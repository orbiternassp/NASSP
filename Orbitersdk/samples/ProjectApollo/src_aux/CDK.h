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

#ifndef CDK_H
#define CDK_H

// Include other necessary files - user only needs to include this one
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Control.h"
#include "OrbiterMath.h"
// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"

// Prototypes for general functions
//void ScaleOutput(char *Buffer, double Value);
void PrintVector(VECTOR3 Vec);
void PrintAngleVector(VECTOR3 Vec);
void PrintInt(int x);
void PrintReal(double x);
void PrintReal(double x, double y);
void PrintReal(double x, double y, double z);
void PrintString(char *str);

#endif
