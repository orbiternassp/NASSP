/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  IMU math helpers

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

#define IMUMATH_C

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <math.h>
#include <stdio.h>
#include "soundlib.h"

#include "nasspdefs.h"
#include "apolloguidance.h"
#include "dsky.h"

#include "ioChannels.h"
#include "IMU.h"

//
// These probably don't need to be part of the IMU class, but I've put them there
// for now to reduce namespace contagion.
//

double IMU::degToRad(double angle) {
	return angle * PI / 180.0;	
}

double IMU::radToDeg(double angle) {
	return angle * 180.0 / PI;	
}

double IMU::gyroPulsesToRad(int pulses) {
	return (((double)pulses) * TWO_PI) / 2097152.0;
}

int IMU::radToGyroPulses(double angle) {
	return (int)((angle * 2097152.0) / TWO_PI);
}

MATRIX3 IMU::getRotationMatrixX(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the X axis (Pitch)
	
	MATRIX3 RotMatrixX;
	
	RotMatrixX.m11 = 1;
	RotMatrixX.m12 = 0;
	RotMatrixX.m13 = 0;
	RotMatrixX.m21 = 0;
	RotMatrixX.m22 = cos(angle);
	RotMatrixX.m23 = -sin(angle);
	RotMatrixX.m31 = 0;
	RotMatrixX.m32 = sin(angle);
	RotMatrixX.m33 = cos(angle);
	
	return RotMatrixX;
}

MATRIX3 IMU::getRotationMatrixY(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the Y axis (Yaw)

	MATRIX3 RotMatrixY;
	
	RotMatrixY.m11 = cos(angle);
	RotMatrixY.m12 = 0;
	RotMatrixY.m13 = sin(angle);
	RotMatrixY.m21 = 0;
	RotMatrixY.m22 = 1;
	RotMatrixY.m23 = 0;
	RotMatrixY.m31 = -sin(angle);
	RotMatrixY.m32 = 0;
	RotMatrixY.m33 = cos(angle);
	
	return RotMatrixY;
}

MATRIX3 IMU::getRotationMatrixZ(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the Z axis (Roll)

	MATRIX3 RotMatrixZ;
	
	RotMatrixZ.m11 = cos(angle);
	RotMatrixZ.m12 = -sin(angle);
	RotMatrixZ.m13 = 0;
	RotMatrixZ.m21 = sin(angle);
	RotMatrixZ.m22 = cos(angle);
	RotMatrixZ.m23 = 0;
	RotMatrixZ.m31 = 0;
	RotMatrixZ.m32 = 0;
	RotMatrixZ.m33 = 1;
	
	return RotMatrixZ;	
}

VECTOR3 IMU::getRotationAnglesXZY(MATRIX3 m) {
	
	VECTOR3 v;
	
	v.z = asin(-m.m12);
	
	if (m.m11 * cos(v.z) > 0) {		  	
		v.y = atan(m.m13 / m.m11);
	} else {
		v.y = atan(m.m13 / m.m11) + PI;
	}
	
	if (m.m22 * cos(v.z) > 0) {
		v.x = atan(m.m32 / m.m22);
	} else {
		v.x = atan(m.m32 / m.m22) + PI;
	}
	return v;
}

VECTOR3 IMU::getRotationAnglesZYX(MATRIX3 m) {
	
	VECTOR3 v;
	
	v.y = asin(-m.m31);
	
	if (m.m33 * cos(v.y) > 0) {		  	
		v.x = atan(-m.m32 / m.m33);
	} else {
		v.x = atan(-m.m32 / m.m33) + PI;
	}
	
	if (m.m11 * cos(v.y) > 0) {
		v.z = atan(-m.m21 / m.m11);
	} else {
		v.z = atan(-m.m21 / m.m11) + PI;
	}
	return v;
}

MATRIX3 IMU::getNavigationBaseToOrbiterLocalTransformation() {
	
	MATRIX3 m;
	int i;
	
	for (i = 0; i < 9; i++) {
		m.data[i] = 0.0;
	}
	if(LEM){
		m.m12 = 1.0;	
		m.m21 = 1.0;
		m.m33 = 1.0;
	}else{
		m.m12 = 1.0;	
		m.m23 = -1.0;
		m.m31 = 1.0;
	}
	return m;
} 

MATRIX3 IMU::getOrbiterLocalToNavigationBaseTransformation() {
	
	MATRIX3 m;
	int i;
	
	for (i = 0; i < 9; i++) {
		m.data[i] = 0.0;
	}
	if(LEM){
		m.m12 = 1.0;
		m.m21 = 1.0;	
		m.m33 = 1.0;
	}else{
		m.m13 = 1.0;
		m.m21 = 1.0;	
		m.m32 = -1.0;
	}
	return m;
}
