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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.2  2006/04/23 04:15:45  dseagrav
  *	LEM checkpoint commit. The LEM is not yet airworthy. Please be patient.
  *	
  *	Revision 1.1  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  **************************************************************************/

#define IMUMATH_C

#include <math.h>

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
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

IMU_Matrix3 IMU::getRotationMatrixX(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the X axis (Pitch)
	
	IMU_Matrix3 RotMatrixX;
	
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

IMU_Matrix3 IMU::getRotationMatrixY(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the Y axis (Yaw)

	IMU_Matrix3 RotMatrixY;
	
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

IMU_Matrix3 IMU::getRotationMatrixZ(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the Z axis (Roll)

	IMU_Matrix3 RotMatrixZ;
	
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

IMU_Matrix3 IMU::multiplyMatrix(IMU_Matrix3 a, IMU_Matrix3 b) {

	IMU_Matrix3 r;
	
	r.m11 = (a.m11 * b.m11) + (a.m12 * b.m21) + (a.m13 * b.m31);
	r.m12 = (a.m11 * b.m12) + (a.m12 * b.m22) + (a.m13 * b.m32);
	r.m13 = (a.m11 * b.m13) + (a.m12 * b.m23) + (a.m13 * b.m33);
	r.m21 = (a.m21 * b.m11) + (a.m22 * b.m21) + (a.m23 * b.m31);
	r.m22 = (a.m21 * b.m12) + (a.m22 * b.m22) + (a.m23 * b.m32);
	r.m23 = (a.m21 * b.m13) + (a.m22 * b.m23) + (a.m23 * b.m33);
	r.m31 = (a.m31 * b.m11) + (a.m32 * b.m21) + (a.m33 * b.m31);
	r.m32 = (a.m31 * b.m12) + (a.m32 * b.m22) + (a.m33 * b.m32);
	r.m33 = (a.m31 * b.m13) + (a.m32 * b.m23) + (a.m33 * b.m33);	
	return r;
}

IMU_Vector3 IMU::multiplyMatrixByVector(IMU_Matrix3 m, IMU_Vector3 v) {

	IMU_Vector3 r;

	r.x = (v.x * m.m11) + (v.y * m.m12) + (v.z * m.m13);
	r.y = (v.x * m.m21) + (v.y * m.m22) + (v.z * m.m23);
	r.z = (v.x * m.m31) + (v.y * m.m32) + (v.z * m.m33);

	return r;
}

IMU_Vector3 IMU::getRotationAnglesXZY(IMU_Matrix3 m) {
	
	IMU_Vector3 v;
	
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

IMU_Vector3 IMU::getRotationAnglesZYX(IMU_Matrix3 m) {
	
	IMU_Vector3 v;
	
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

IMU_Matrix3 IMU::getNavigationBaseToOrbiterLocalTransformation() {
	
	// This transformation assumes that spacecraft azimuth - orbiter heading = 180 deg
	IMU_Matrix3 m;
	int i;
	
	for (i = 0; i < 9; i++) {
		m.data[i] = 0.0;
	}
/*	m.m12 = -1.0;	
	m.m23 = 1.0;
	m.m31 = 1.0;
*/
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

IMU_Matrix3 IMU::getOrbiterLocalToNavigationBaseTransformation() {
	
	// This transformation assumes that spacecraft azimuth - orbiter heading = 180 deg
	IMU_Matrix3 m;
	int i;
	
	for (i = 0; i < 9; i++) {
		m.data[i] = 0.0;
	}
/*	m.m13 = 1.0;
	m.m21 = -1.0;	
	m.m32 = 1.0;
*/
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


IMU_Vector3 IMU::VECTOR3ToIMU_Vector3(VECTOR3 v) {

	IMU_Vector3 iv;

	iv.x = v.x;
	iv.y = v.y;
	iv.z = v.z;
	return iv;
}

VECTOR3 IMU::IMU_Vector3ToVECTOR3(IMU_Vector3 iv) {

	VECTOR3 v;

	v.x = iv.x;
	v.y = iv.y;
	v.z = iv.z;
	return v;
}

IMU_Matrix3 IMU::MATRIX3ToIMU_Matrix3(MATRIX3 m) {

	IMU_Matrix3 im;

	im.m11 = m.m11;
	im.m12 = m.m12;
	im.m13 = m.m13;
	im.m21 = m.m21;
	im.m22 = m.m22;
	im.m23 = m.m23;
	im.m31 = m.m31;
	im.m32 = m.m32;
	im.m33 = m.m33;
	return im;
}
