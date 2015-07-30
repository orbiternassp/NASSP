/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2005 Chris Knestrick

  Code to solve the prediction problem - given the position and velocity
  vectors at some initial time, what will the position and velocity vectors
  be at specified time in the future. The method implemented here is
  described in Chapter 4 of "Fundementals of Astrodynamics" by Bate, Mueller,
  and White.

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

#ifndef PREDICTION_H
#define PREDICTION_H
// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "Elements.h"

void PredictPosVelVectors(const VECTOR3 &Pos, const VECTOR3 &Vel, double a, double Mu,
						  double Time, VECTOR3 &NewPos, VECTOR3 &NewVel);

#endif
