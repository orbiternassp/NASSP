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

inline void papiWriteScenario_double(FILEHANDLE scn, char *item, double d) {

	char buffer[256];

	sprintf(buffer, "  %s %lf", item, d);
	oapiWriteLine(scn, buffer);
}

inline void papiCameraSetAperture(double fov) {

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

inline double papiCameraAperture() {

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