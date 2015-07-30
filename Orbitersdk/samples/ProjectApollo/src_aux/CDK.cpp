/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Chris Knestrick

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

#if defined(_MSC_VER) && (_MSC_VER >= 1300 ) // Microsoft Visual Studio Version 2003 and higher
#define _CRT_SECURE_NO_DEPRECATE 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "CDK.h"


// Print out a vector to the debug string, with a precision of two decimal places
void PrintVector(VECTOR3 Vec)
{
	sprintf(oapiDebugString(), "%.3f  %.3f  %.3f", Vec.x, Vec.y, Vec.z);
}

// Same as above, except the vector contains angles
void PrintAngleVector(VECTOR3 Vec)
{
	sprintf(oapiDebugString(), "%.3f  %.3f  %.3f", Degree(Vec.x), Degree(Vec.y), Degree(Vec.z));
}

// Print an integer
void PrintInt(int x)
{
	sprintf(oapiDebugString(), "%d", x);
}

// Print a real (double)
void PrintReal(double x)
{
	sprintf(oapiDebugString(), "%.3f", x);
}

void PrintReal(double x, double y)
{
	sprintf(oapiDebugString(), "%.3f  %.3f", x, y);
}

void PrintReal(double x, double y, double z)
{
	sprintf(oapiDebugString(), "%.3f  %.3f  %.3f", x, y, z);
}

// Prints a string
void PrintString(char *str)
{
	sprintf(oapiDebugString(), "%s", str);
}
