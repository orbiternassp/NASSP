/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2002-2005 Chris Knestrick

  General matrix/vector operations that I found useful

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
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

#ifndef CHRIS_MATRIX_H
#define CHRIS_MATRIX_H

void GetRotMatrixX(double Angle, MATRIX3 &RotMatrixX)
{
	RotMatrixX.m11 = 1;
	RotMatrixX.m12 = 0;
	RotMatrixX.m13 = 0;
	RotMatrixX.m21 = 0;
	RotMatrixX.m22 = cos(Angle);
	RotMatrixX.m23 = sin(Angle);
	RotMatrixX.m31 = 0;
	RotMatrixX.m32 = -sin(Angle);
	RotMatrixX.m33 = cos(Angle);
}

void GetRotMatrixY(double Angle, MATRIX3 &RotMatrixY)
{
	RotMatrixY.m11 = cos(Angle);
	RotMatrixY.m12 = 0;
	RotMatrixY.m13 = -sin(Angle);
	RotMatrixY.m21 = 0;
	RotMatrixY.m22 = 1;
	RotMatrixY.m23 = 0;
	RotMatrixY.m31 = sin(Angle);
	RotMatrixY.m32 = 0;
	RotMatrixY.m33 = cos(Angle);


}

void GetRotMatrixZ(double Angle, MATRIX3 &RotMatrixZ)
{
	RotMatrixZ.m11 = cos(Angle);
	RotMatrixZ.m12 = sin(Angle);
	RotMatrixZ.m13 = 0;
	RotMatrixZ.m21 = -sin(Angle);
	RotMatrixZ.m22 = cos(Angle);
	RotMatrixZ.m23 = 0;
	RotMatrixZ.m31 = 0;
	RotMatrixZ.m32 = 0;
	RotMatrixZ.m33 = 1;
}

void RotatePoint(const VECTOR3 &Initial, const MATRIX3 &RotMatrix, VECTOR3 &Result)
{

	Result.x =	(Initial.x * RotMatrix.m11)
				+ (Initial.y * RotMatrix.m12)
				+ (Initial.z * RotMatrix.m13);

	Result.y =	(Initial.x * RotMatrix.m21)
				+ (Initial.y * RotMatrix.m22)
				+ (Initial.z * RotMatrix.m23);

	Result.z =	(Initial.x * RotMatrix.m31)
				+ (Initial.y * RotMatrix.m32)
				+ (Initial.z * RotMatrix.m33);
}



// OK, I've also added some vector stuff
inline VECTOR3 operator/ (const VECTOR3 &a, double b)
{
	VECTOR3 c;
	c.x = a.x / b;
	c.y = a.y / b;
	c.z = a.z / b;

	return c;
}

inline VECTOR3 operator/ (const VECTOR3 &a, int b)
{
	VECTOR3 c;
	c.x = a.x / b;
	c.y = a.y / b;
	c.z = a.z / b;

	return c;
}

// Magnitude
inline double operator! (const VECTOR3 &a)
{
	return (sqrt(pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2)));
}

// Horizontal magnitude (z and x plane)
inline double HMag (const VECTOR3 &a)
{
	return (sqrt(pow(a.x, 2) + pow(a.z, 2)));
}


#endif