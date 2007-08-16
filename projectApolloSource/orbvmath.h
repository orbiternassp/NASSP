#ifndef _ORBVMATH_H
#define _ORBVMATH_H

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>

void ovmCLRV(VECTOR3 &v)
{
	v.x = 0.0;
	v.y = 0.0;
	v.z = 0.0;
}

void ovmUNITV(VECTOR3 &v)
{
	v.x = 1.0;
	v.y = 1.0;
	v.z = 1.0;
}
void ovmSETV(VECTOR3 &v, double &s)
{
	v.x = s;
	v.y = s;
	v.z = s;
}

void ovmADDV(VECTOR3 &v, VECTOR3 &a, VECTOR3 &b)
{
	v.x = a.x + b.x;
	v.y = a.y + b.y;
	v.z = a.z + b.z;
}

void ovmSUBV(VECTOR3 &v, VECTOR3 &a, VECTOR3 &b)
{
	v.x = a.x - b.x;
	v.y = a.y - b.y;
	v.z = a.z - b.z;
}

void ovmMULVS(VECTOR3 &v, VECTOR3 &u, double &s)
{
	v.x = u.x * s;
	v.y = u.y * s;
	v.z = u.z * s;
}

void ovmDIVVS(VECTOR3 &v, VECTOR3 &u, double &s)
{
	v.x = u.x / s;
	v.y = u.y / s;
	v.z = u.z / s;
}

void ovmDOTVP(double &s, VECTOR3 &v, VECTOR3 &u)
{
	s = v.x*u.x + v.y*u.y + v.z*u.z;
}

void ovmMAGV(double &s, VECTOR3 &v)
{
	s = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

void ovmCROSSVP(VECTOR3 &v, VECTOR3 &a, VECTOR3 &b)
{
	v.x = a.y*b.z - a.z*b.y;
	v.y = a.z*b.x - a.x*b.z;
	v.z = a.x*b.y - a.y*b.x;
}

void ovmCLRM(MATRIX3 &p)
{
	int icnt;
	for (icnt = 0; icnt < 9; icnt++) {
		p.data[icnt] = 0.0;
	}
}

void ovmSETIM(MATRIX3 &p)
{
	int icnt, jcnt, indx;
	for (icnt = 0; icnt < 3; icnt++) {
		for (jcnt = 0; jcnt < 3; jcnt++) {
			indx = 3 * jcnt + icnt;
			if (icnt == jcnt) {
				p.data[indx] = 1.0;
			} else {
				p.data[indx] = 0.0;
			}
		}
	}
}

void ovmSETM(MATRIX3 &p, MATRIX3 &q)
{
	int icnt;
	for (icnt = 0; icnt < 9; icnt++) {
		p.data[icnt] = q.data[icnt];
	}
}

void ovmTRANSM(MATRIX3 &p, MATRIX3 &q)
{
	int icnt, jcnt, indx, indx2;
	for (icnt = 0; icnt < 3; icnt++) {
		for (jcnt = 0; jcnt < 3; jcnt++) {
			indx = 3 * jcnt + icnt;
			indx2 = 3 * icnt + jcnt;
			if (icnt == jcnt) {
				p.data[indx] = q.data[indx2];
			} else {
				p.data[indx] = q.data[indx2];
			}
		}
	}
}

void ovmADDM(MATRIX3 &p, MATRIX3 &q, MATRIX3 &r)
{
	int icnt;
	for (icnt = 0; icnt < 9; icnt++) {
		p.data[icnt] = q.data[icnt] + q.data[icnt];
	}
}

void ovmSUBM(MATRIX3 &p, MATRIX3 &q, MATRIX3 &r)
{
	int icnt;
	for (icnt = 0; icnt < 9; icnt++) {
		p.data[icnt] = q.data[icnt] - q.data[icnt];
	}
}

void ovmMULMS(MATRIX3 &p, MATRIX3 &q, double &s)
{
	int icnt;
	for (icnt = 0; icnt < 9; icnt++) {
		p.data[icnt] = q.data[icnt] * s;
	}
}

void ovmDIVMS(MATRIX3 &p, MATRIX3 &q, double &s)
{
	int icnt;
	for (icnt = 0; icnt < 9; icnt++) {
		p.data[icnt] = q.data[icnt] / s;
	}
}

void ovmMULMV(VECTOR3 &v, MATRIX3 &q, VECTOR3 &u)
{
	int icnt, jcnt, indx;
	double tmpv[3], tmpu[3];
	tmpu[0]=u.x;
	tmpu[1]=u.y;
	tmpu[2]=u.z;

	for (icnt = 0; icnt < 3; icnt++) {
		tmpv[icnt] = 0.0;
		for (jcnt = 0; jcnt < 3; jcnt++) {
			indx = 3 * jcnt + icnt;
			tmpv[icnt] += q.data[indx] * tmpu[jcnt];
		}
	}
	v.x=tmpv[0];
	v.y=tmpv[1];
	v.z=tmpv[2];
}
#endif
