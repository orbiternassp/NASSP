/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007

  Project Apollo API utilities
  
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

#pragma warning(disable : 4996 ) 

#include "CollisionSDK/CollisionSDK.h"


// All inline functions and const variables should be static, see
// http://www.orbitersim.com/Forum/Default.aspx?g=posts&m=172439#172439

static inline void papiWriteScenario_bool(FILEHANDLE scn, char *item, bool b) {

	oapiWriteScenario_int(scn, item, (b ? 1 : 0));
}

static inline void papiWriteScenario_double(FILEHANDLE scn, char *item, double d) {

	char buffer[256];

	sprintf(buffer, "  %s %.12lf", item, d);
	oapiWriteLine(scn, buffer);
}

static inline void papiWriteScenario_vec(FILEHANDLE scn, char *item, VECTOR3 v) {

	char buffer[256];

	sprintf(buffer, "  %s %.12lf %.12lf %.12lf", item, v.x, v.y, v.z);
	oapiWriteLine(scn, buffer);
}

static inline void papiWriteScenario_mx(FILEHANDLE scn, char *item, MATRIX3 m) {

	char buffer[256];

	sprintf(buffer, "  %s %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf", item, m.m11, m.m12, m.m13, m.m21, m.m22, m.m23, m.m31, m.m32, m.m33 );
	oapiWriteLine(scn, buffer);
}

static inline bool papiReadScenario_bool(char *line, char *item, bool &b) {

	char buffer[256];
	int i = 0;

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp (buffer, item)) {
			if (sscanf(line, "%s %d", buffer, &i) == 2) {
				b = (i != 0);
				return true;
			}
		}
	}
	return false;
}

static inline bool papiReadScenario_int(char *line, char *item, int &i) {

	char buffer[256];
	int j;

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp (buffer, item)) {
			if (sscanf(line, "%s %d", buffer, &j) == 2) {
				i = j;
				return true;
			}
		}
	}
	return false;
}

static inline bool papiReadScenario_double(char *line, char *item, double &d) {

	char buffer[256];
	double e;

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			if (sscanf(line, "%s %lf", buffer, &e) == 2) {
				d = e;
				return true;
			}
		}
	}
	return false;
}

static inline bool papiReadScenario_vec(char *line, char *item, VECTOR3 &v) {

	char buffer[256];
	VECTOR3 w;

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp (buffer, item)) {
			if (sscanf(line, "%s %lf %lf %lf", buffer, &w.x, &w.y, &w.z) == 4) {
				v = w;
				return true;
			}
		}
	}
	return false;
}

static inline void papiCameraSetAperture(double fov) {

	// Artlav's camera aperture hack is working with the 20060929 version only
	int version = oapiGetOrbiterVersion();
	if (version != 60929) {
		oapiCameraSetAperture(fov);
		return;
	}

	double *pfov, *pfovx;
	double* d;
	void* p;
	DWORD a, *b, *c;

	p=&c;
	b=(DWORD*)p;
	*b=0x4F4990;
	a=*c;
	a+=0x2C8;

	p=&d;
	*(DWORD*)p=a;//0x381CD68;
	pfov=d;

	*(DWORD*)p=a+8;//0x381CD68;
	pfovx=d;

	oapiCameraSetAperture(fov);
	*pfov=fov; 
	*pfovx=(fov*1.06869);
}

static inline double papiCameraAperture() {

	// Artlav's camera aperture hack is working with the 20060929 version only
	int version = oapiGetOrbiterVersion();
	if (version != 60929) {
		return oapiCameraAperture();
	}

	double *pfov;
	double* d;
	void* p;
	DWORD a, *b, *c;

	p=&c;
	b=(DWORD*)p;
	*b=0x4F4990;
	a=*c;
	a+=0x2C8;

	p=&d;
	*(DWORD*)p=a;//0x381CD68;
	pfov=d;

	return *pfov;
}

static inline double papiGetAltitude(VESSEL *vessel) {

	char buffer[255];
	double lon, lat, rad;

	oapiGetObjectName(vessel->GetGravityRef(), buffer, 255);
	vessel->GetEquPos(lon, lat, rad);
	return vessel->GetAltitude() - VSGetAbsMaxElvLoc(buffer, lat, lon);
}

static inline void papiWriteScenario_intarr(FILEHANDLE scn, char *item, int *v, int len) {

	char buffer[256], buffer2[256];
	int s;
	sprintf(buffer, "  %s ", item);
	s = strlen(item) + 3;
	for (int i = 0; i < len; i++)
	{
		sprintf(buffer + s, "%d ", v[i]);
		sprintf(buffer2, "%d", v[i]);
		s += strlen(buffer2) + 1;
	}
	oapiWriteLine(scn, buffer);
}

static inline bool papiReadScenario_intarr(char *line, char *item, int *v, int len) {

	char buffer[256];
	int pos, cur;

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			sscanf(line, "%s %n", buffer, &pos);
			for (int i = 0; i < len; i++)
			{
				sscanf(line + pos, "%d %n", &v[i], &cur);
				pos += cur;
			}
		}
	}
	return false;
}

static inline bool papiReadScenario_string(char *line, char *item, char *i) {

	char buffer[256];

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			if (sscanf(line, "%s %s", buffer, i) == 2) {
				return true;
			}
		}
	}
	return false;
}

static inline bool papiReadScenario_mat(char *line, char *item, MATRIX3 &v) {

	char buffer[256];
	MATRIX3 w;

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			if (sscanf(line, "%s %lf %lf %lf %lf %lf %lf %lf %lf %lf", buffer, &w.m11, &w.m12, &w.m13, &w.m21, &w.m22, &w.m23, &w.m31, &w.m32, &w.m33) == 10) {
				v = w;
				return true;
			}
		}
	}
	return false;
}