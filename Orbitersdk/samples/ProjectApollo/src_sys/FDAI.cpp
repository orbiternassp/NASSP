/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Rob Conley, Markus Joachim

  This class is used to display a FDAI on the panel.

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

// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>

#include "nasspdefs.h"

#include "PanelSDK/PanelSDK.h"
#include "PanelSDK/Internals/Esystems.h"

#include "FDAI.h"

FDAI::FDAI() {

	init = 0;
	ScrX = 0;
	ScrY = 0;

	target = _V(0, 0, 0);
	now = _V(0, 0, 0);
	lastRates = _V(0, 0, 0);
	lastErrors = _V(0, 0, 0);
	lastPaintAtt = _V(0, 0, 0);

	lastPaintTime = -1;
	LM_FDAI = false;
	DCSource = NULL;
	ACSource = NULL;
	noAC = false;
	vessel = NULL;
	hBallSurf = oapiCreateSurface(180, 180);

	penBlack = oapiCreatePen(1, 1, 0x000000);
	penWhite = oapiCreatePen(1, 1, 0xffffff);
	brushBlack = oapiCreateBrush(0x000000);
	brushWhite = oapiCreateBrush(0xffffff);
}

void FDAI::Init(VESSEL *v)
{
	vessel = v;
}

void FDAI::InitGL() {

	GLuint      PixelFormat;
	BITMAPINFOHEADER BIH;
	int iSize = sizeof(BITMAPINFOHEADER);
	BIH.biSize = iSize;
	BIH.biWidth = 180;				//size of the sphere is 180x180
	BIH.biHeight = 180;
	BIH.biPlanes = 1;
	BIH.biBitCount = 16;//default is 16.
	BIH.biCompression = BI_RGB;
	BIH.biSizeImage = 0;
	void* m_pBits;
	hDC2 = CreateCompatibleDC(NULL);//we make a new DC and DIbitmap for OpenGL to draw onto
	static  PIXELFORMATDESCRIPTOR pfd2;
	DescribePixelFormat(hDC2, 1, sizeof(PIXELFORMATDESCRIPTOR), &pfd2);//just get a random pixel format.. 
	BIH.biBitCount = pfd2.cColorBits;//to get the current bit depth.. !?
	hBMP = CreateDIBSection(hDC2, (BITMAPINFO*)&BIH, DIB_RGB_COLORS, &m_pBits, NULL, 0);
	hBMP_old = (HBITMAP)SelectObject(hDC2, hBMP);
	static  PIXELFORMATDESCRIPTOR pfd = {                             // pfd Tells Windows How We Want Things To Be
		sizeof(PIXELFORMATDESCRIPTOR),                              // Size Of This Pixel Format Descriptor
		1,                                                          // Version Number
		PFD_DRAW_TO_BITMAP |                                        // Format Must Support Bitmap Rendering
		PFD_SUPPORT_OPENGL |
		PFD_SUPPORT_GDI,											// Format Must Support OpenGL,                                           
		0,//        PFD_TYPE_RGBA,                                              // Request An RGBA Format
		16,															// Select Our Color Depth
		0, 0, 0, 0, 0, 0,                                           // Color Bits Ignored
		0,//1,                                                          // No Alpha Buffer
		0,                                                          // Shift Bit Ignored
		0,                                                          // No Accumulation Buffer
		0, 0, 0, 0,                                                 // Accumulation Bits Ignored
		0,//16,                                                         // 16Bit Z-Buffer (Depth Buffer)  
		0,                                                          // No Stencil Buffer
		0,                                                          // No Auxiliary Buffer
		0,//PFD_MAIN_PLANE,                                             // Main Drawing Layer
		0,                                                          // Reserved
		0, 0, 0                                                     // Layer Masks Ignored
	};
	pfd.cColorBits = pfd2.cColorBits;//same color depth needed.
	DWORD code;
	code = GetLastError();
	PixelFormat = ChoosePixelFormat(hDC2, &pfd);// now pretend we want a new format
	int ret;
	ret = SetPixelFormat(hDC2, PixelFormat, &pfd);
	code = GetLastError();
	hRC = wglCreateContext(hDC2);
	ret = wglMakeCurrent(hDC2, hRC);				//all standard OpenGL init so far

	//We load the texture
	int texture_index;
	if (LM_FDAI)
	{
		texture_index = LoadOGLBitmap("Textures\\ProjectApollo\\FDAI_Ball_LM.dds");
	}
	else
	{
		texture_index = LoadOGLBitmap("Textures\\ProjectApollo\\FDAI_Ball.dds");
	}
	if (texture_index > 0) glEnable(GL_TEXTURE_2D);

	glShadeModel(GL_SMOOTH);                        // Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);           // Panel Background color
	glClearDepth(1.0f);                             // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);                        // Enables Depth Testing
	glDepthFunc(GL_LESS);                           // The Type Of Depth Testing To Do
	glViewport(0, 0, 180, 180);                     // Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);                    // Select The Projection Matrix
	glLoadIdentity();                               // Reset The Projection Matrix
	gluPerspective(45.0f, 1.0, 1.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);                     // Select The Modelview Matrix          
	glLoadIdentity();                               // Reset The Projection Matrix

	//some ambiental setup
	GLfloat light_position[] = { -10.0,10.0,10.0,0.0 };
	GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_specular[] = { (float) 0.5, (float) 0.5, (float) 0.5, 1.0 };
	GLfloat mat_shin[] = { 5.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, mat_specular);
	glEnable(GL_LIGHT0);

	//defining our geometry and composing a display list;
	list_name = glGenLists(1);
	glNewList(list_name, GL_COMPILE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);             // Clear The Screen And The Depth Buffer        
	glColor3f(1.0, 1.0, 1.0);
	quadObj = gluNewQuadric();
	gluQuadricTexture(quadObj, GL_TRUE);
	gluSphere(quadObj, 12, 24, 24);
	glEndList();

	init = 1;		//that's it. If we made it so far, we can use OpenGL
}

FDAI::~FDAI() {

	if (init) {
		gluDeleteQuadric(quadObj);
		wglMakeCurrent(NULL, NULL);	//standard OpenGL release
		BOOL ret = wglDeleteContext(hRC);
		hRC = NULL;
		SelectObject(hDC2, hBMP_old);//remember to delete DC and bitmap memory we created
		DeleteObject(hBMP);
		DeleteDC(hDC2);
		hDC2 = 0;
	}
	oapiDestroySurface(hBallSurf);
	oapiReleasePen(penBlack);
	oapiReleasePen(penWhite);
	oapiReleaseBrush(brushBlack);
	oapiReleaseBrush(brushWhite);
}

void FDAI::RegisterMe(int index, int x, int y) {

	idx = index;
	ScrX = x;
	ScrY = y;
	oapiRegisterPanelArea(index, _R(ScrX, ScrY, ScrX + 245, ScrY + 245), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
}


void FDAI::SetAttitude(VECTOR3 attitude) {

	if (!IsPowered())
		return;

	target.y = -attitude.x;	// roll
	target.z = attitude.y;	// pitch
	target.x = attitude.z;	// yaw
}

void FDAI::RotateBall(double simdt) {

	double delta, deltamax;

	deltamax = 0.87*simdt; //About 50°/s

	delta = target.z - now.z;
	if (delta > deltamax) {
		if (delta > PI) {
			now.z += 2 * PI;
			RotateBall(simdt);
			return;
		}
		now.z += deltamax;

	}
	else if (delta < -deltamax) {
		if (delta < -PI) {
			now.z -= 2 * PI;
			RotateBall(simdt);
			return;
		}
		now.z -= deltamax;
	}
	else
		now.z += delta;

	delta = target.y - now.y;
	if (delta > deltamax) {
		if (delta > PI) {
			now.y += 2 * PI;
			RotateBall(simdt);
			return;
		}
		now.y += deltamax;

	}
	else if (delta < -deltamax) {
		if (delta < -PI) {
			now.y -= 2 * PI;
			RotateBall(simdt);
			return;
		}
		now.y -= deltamax;
	}
	else
		now.y += delta;

	delta = target.x - now.x;
	if (delta > deltamax) {
		if (delta > PI) {
			now.x += 2 * PI;
			RotateBall(simdt);
			return;
		}
		now.x += deltamax;

	}
	else if (delta < -deltamax) {
		if (delta < -PI) {
			now.x -= 2 * PI;
			RotateBall(simdt);
			return;
		}
		now.x -= deltamax;
	}
	else
		now.x += delta;
}

void FDAI::MoveBall2D()
{
	glLoadIdentity();
	gluLookAt(0.0, -35.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

	glRotatef(90.0, 0.0, 1.0, 0.0);

	glRotated(now.y / PI * 180.0, 0.0, 1.0, 0.0);	//attitude.x
	glRotated(now.x / PI * 180.0, 1.0, 0.0, 0.0);	//attitude.z
	glRotated(now.z / PI * 180.0, 0.0, 0.0, 1.0);	//attitude.y

	lastPaintAtt = now;
}

void FDAI::PaintMe(VECTOR3 rates, VECTOR3 errors, SURFHANDLE surf, SURFHANDLE hFDAI,
	SURFHANDLE hFDAIRoll, SURFHANDLE hFDAIOff, SURFHANDLE hFDAINeedles, int smooth) {
	if (!init) InitGL();

	// Don't do the OpenGL calculations every timestep
	if (smooth || lastPaintTime == -1 || ((length(lastPaintAtt - target) > 0.005 || oapiGetSysTime() > lastPaintTime + 2.0) && oapiGetSysTime() > lastPaintTime + 0.1)) {
		int ret = wglMakeCurrent(hDC2, hRC);
		MoveBall2D();
		glCallList(list_name);	//render
		glFlush();
		glFinish();

		lastPaintTime = oapiGetSysTime();

		HDC hDC = oapiGetDC(hBallSurf);
		BitBlt(hDC, 0, 0, 180, 180, hDC2, 0, 0, SRCCOPY);
		oapiReleaseDC(hBallSurf, hDC);
	}

	oapiBlt(surf, hBallSurf, 43, 43, 10, 10, 150, 150, SURF_PREDEF_CK);//then we bitblt onto the panel.

	// roll indicator
	double angle = -target.y;

	oapi::Sketchpad *skp = oapiGetSketchpad(surf);

	// Empirical values to draw the indicator
	const double trisize = 14.0;
	const double baseangle = 0.15;
	const double radius = 70;

	// Compute roll indicator vertices
	long xtip = 123 - (long)(radius * sin(angle));
	long ytip = 123 - (long)(radius * cos(angle));

	long xbl = 123 - (long)((radius - trisize) * sin(angle - baseangle));
	long ybl = 123 - (long)((radius - trisize) * cos(angle - baseangle));

	long xbr = 123 - (long)((radius - trisize) * sin(angle + baseangle));
	long ybr = 123 - (long)((radius - trisize) * cos(angle + baseangle));

	// Split it in 3
	oapi::IVECTOR2 tri1[3] = {
		{xtip, ytip},
		{xbl, ybl},
		{(long)(xbr * 0.3 + xbl * 0.7), (long)(ybr * 0.3 + ybl * 0.7)},
	};
	oapi::IVECTOR2 tri2[3] = {
		{xtip, ytip},
		{(long)(xbr * 0.3 + xbl * 0.7), (long)(ybr * 0.3 + ybl * 0.7)},
		{(long)(xbr * 0.7 + xbl * 0.3), (long)(ybr * 0.7 + ybl * 0.3)},
	};
	oapi::IVECTOR2 tri3[3] = {
		{xtip, ytip},
		{(long)(xbr * 0.7 + xbl * 0.3), (long)(ybr * 0.7 + ybl * 0.3)},
		{xbr, ybr},
	};


	skp->SetBrush(brushBlack);
	skp->SetPen(penBlack);
	skp->Polygon(tri2, 3);

	skp->SetBrush(brushWhite);
	skp->SetPen(penWhite);
	skp->Polygon(tri1, 3);
	skp->Polygon(tri3, 3);


	oapiReleaseSketchpad(skp);

	// frame-bitmaps
	// Was 13,13
	oapiBlt(surf, hFDAIRoll, 43, 43, 0, 0, 160, 160, SURF_PREDEF_CK);
	// Was 0,0
	oapiBlt(surf, hFDAI, 30, 30, 0, 0, 185, 183, SURF_PREDEF_CK);

	// Roll/Yaw X range is 54 - 180, 117 is center, 63 px per side. (the window is 245 pixels wide)
	// Full-Scale is 63. The max rates for roll are 1, 5, and 50 degrees per second.
	// This becomes 0.017453, 0.087265, and 0.87265 radians respectively.
	// 10 is 0.17453 radians and conversion factor 360.969460
	// Conversion rates are 3609.694608, 721.938921, and 72.193892
	// Offset = Conversion * Rate

	// For the LM FDAI, the width is 100 pixels (50 px/side), with roll/yaw center at 119
	// and pitch center at 120. 5 deg/s conversion factor is 572.967398, 25 deg/s is 114.593480.

	if (IsPowered())
		lastRates = rates;
	else
		rates = lastRates;

	int targetX, targetY, targetZ;

	if (!LM_FDAI)
	{
		//Input is scaled -1.0 to 1.0, this scales to 126 pixels
		targetX = (int)(117 - (63.0 * rates.z));
		targetY = (int)(117 + (63.0 * rates.x));
		targetZ = (int)(117 + (63.0 * rates.y));
	}
	else
	{
		//Input is scaled -1.0 to 1.0, this scales to 100 pixels
		targetX = (int)(119 - (50.0*rates.z));
		targetY = (int)(120 + (50.0*rates.x));
		targetZ = (int)(119 + (50.0*rates.y));
	}

	// Enforce Limits
	if (!LM_FDAI)
	{
		if (targetX > 180) { targetX = 180; }
		else { if (targetX < 54) { targetX = 54; } }
		if (targetY > 180) { targetY = 180; }
		else { if (targetY < 54) { targetY = 54; } }
		if (targetZ > 180) { targetZ = 180; }
		else { if (targetZ < 54) { targetZ = 54; } }
	}
	else
	{
		if (targetX > 169) { targetX = 169; }
		else { if (targetX < 69) { targetX = 69; } }
		if (targetY > 170) { targetY = 170; }
		else { if (targetY < 70) { targetY = 70; } }
		if (targetZ > 169) { targetZ = 169; }
		else { if (targetZ < 69) { targetZ = 69; } }
	}

	// 60-176

	if (LM_FDAI)
	{
		// Draw Roll-Rate Needle
		oapiBlt(surf, hFDAINeedles, targetX, 14, 10, 3, 7, 14, SURF_PREDEF_CK);
		// Draw Pitch-Rate Needle
		oapiBlt(surf, hFDAINeedles, 218, targetY, 17, 3, 14, 7, SURF_PREDEF_CK);
		// Draw Yaw-Rate Needle
		oapiBlt(surf, hFDAINeedles, targetZ, 218, 3, 3, 7, 14, SURF_PREDEF_CK);
	}
	else
	{
		// Draw Roll-Rate Needle
		oapiBlt(surf, hFDAINeedles, targetX, 4, 16, 3, 12, 11, SURF_PREDEF_CK);
		// Draw Pitch-Rate Needle
		oapiBlt(surf, hFDAINeedles, 225, targetY, 28, 3, 10, 12, SURF_PREDEF_CK);
		// Draw Yaw-Rate Needle
		oapiBlt(surf, hFDAINeedles, targetZ, 224, 4, 3, 12, 11, SURF_PREDEF_CK);
	}


	// Draw Roll-Error Needle
	// 122,42 is the center, 41 px left-right variance, 11 px up-down variance.
	// 0.268292 px down per pixel left.

	if (IsPowered())
		lastErrors = errors;
	else
		errors = lastErrors;
	if (!LM_FDAI)
	{
		targetY = (int)(fabs(errors.x) * 0.268292);
		oapiBlt(surf, hFDAINeedles, 122 + (int)errors.x, 42 + targetY, 0, 0, 2, 72 - targetY, SURF_PREDEF_CK);
		// Draw Pitch-Error Needle
		targetY = (int)(fabs(errors.y) * 0.268292);
		oapiBlt(surf, hFDAINeedles, 135, 122 + (int)errors.y, 4, 0, 69 - targetY, 2, SURF_PREDEF_CK);
		// Draw Yaw-Error Needle
		targetY = (int)(fabs(errors.z) * 0.268292);
		oapiBlt(surf, hFDAINeedles, 122 + (int)errors.z, 135, 0, 0, 2, 69 - targetY, SURF_PREDEF_CK);
	}
	else
	{
		targetY = (int)(fabs(errors.x) * 0.268292);
		oapiBlt(surf, hFDAINeedles, 122 + (int)errors.x, 54 + targetY, 0, 0, 2, 31 - targetY, SURF_PREDEF_CK);
		// Draw Pitch-Error Needle
		targetY = (int)(fabs(errors.y) * 0.268292);
		oapiBlt(surf, hFDAINeedles, 161, 122 + (int)errors.y, 3, 0, 31 - targetY, 2, SURF_PREDEF_CK);
		// Draw Yaw-Error Needle
		targetY = (int)(fabs(errors.z) * 0.268292);
		oapiBlt(surf, hFDAINeedles, 122 + (int)errors.z, 161, 0, 0, 2, 31 - targetY, SURF_PREDEF_CK);
	}


	// sprintf(oapiDebugString(),"FDAI: Rates %f %f %f, TGX %d",rates.x,rates.y,rates.z,targetX);
	// Off-flag (CSM FDAI doesn't have one... I think)
	if (LM_FDAI && !IsPowered())
		oapiBlt(surf, hFDAIOff, 31, 100, 0, 0, 13, 30, SURF_PREDEF_CK);
}

void FDAI::Timestep(double simt, double simdt)
{
	RotateBall(simdt);
}

void FDAI::SystemTimestep(double simdt) {

	// These numbers are adjusted for the CSM, I don't know if the LM is different.
	if (IsPowered()) {
		if (DCSource)
			DCSource->DrawPower(25.7); // Apollo 15 G&C checklist, page 44: Both FDAIs use 58W AC and DC alltogether
		if (ACSource)
			ACSource->DrawPower(3.3); // CSM Systems Handbook, Power distibution matrix
	}
}

int FDAI::LoadOGLBitmap(char *filename) {

	unsigned char *l_texture;
	int l_index, l_index2 = 0;
	FILE *file;
	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;
	RGBTRIPLE rgb;
	int num_texture = 1; //we only use one OGL texture ,so...


	if ((file = fopen(filename, "rb")) == NULL) return (-1);
	fread(&fileheader, sizeof(fileheader), 1, file);
	fseek(file, sizeof(fileheader), SEEK_SET);
	fread(&infoheader, sizeof(infoheader), 1, file);

	l_texture = (byte *)malloc(infoheader.biWidth * infoheader.biHeight * 4);
	memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);

	for (l_index = 0; l_index < infoheader.biWidth*infoheader.biHeight; l_index++)
	{
		fread(&rgb, sizeof(rgb), 1, file);

		l_texture[l_index2 + 0] = rgb.rgbtRed; // Red component
		l_texture[l_index2 + 1] = rgb.rgbtGreen; // Green component
		l_texture[l_index2 + 2] = rgb.rgbtBlue; // Blue component
		l_texture[l_index2 + 3] = 255; // Alpha value
		l_index2 += 4; // Go to the next position
	}

	fclose(file); // Closes the file stream

	glBindTexture(GL_TEXTURE_2D, num_texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, infoheader.biWidth, infoheader.biHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);
	free(l_texture);

	return (num_texture);
}

// Helper function for getting the minimum of 4 floats
float min4(float a, float b, float c, float d)
{
	if (a < b) {
		if (c < a) {
			if (d < c)
				return d;
			else
				return c;
		}
		else {
			if (d < a)
				return d;
			else
				return a;
		}
	}
	else {
		if (c < b) {
			if (d < c)
				return d;
			else
				return c;
		}
		else {
			if (d < b)
				return d;
			else
				return b;
		}
	}
}


// Helper function for getting the maximum of 4 floats
float max4(float a, float b, float c, float d)
{
	if (a > b) {
		if (c > a) {
			if (d > c)
				return d;
			else
				return c;
		}
		else {
			if (d > a)
				return d;
			else
				return a;
		}
	}
	else {
		if (c > b) {
			if (d > c)
				return d;
			else
				return c;
		}
		else {
			if (d > b)
				return d;
			else
				return b;
		}
	}
}

// The Representation of a 32 bit color table entry
#pragma pack(push)
#pragma pack(1)
typedef struct ssBGR {
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char pad;
} sBGR;

typedef sBGR *pBGR;
#pragma pack(pop)

// Returns the DI (Device Independent) bits of the Bitmap
// Here I use 32 bit since it's easy to adress in memory and no
// padding of the horizontal lines is required.
pBGR MyGetDibBits(HDC hdcSrc, HBITMAP hBmpSrc, int nx, int ny)
{
	BITMAPINFO bi;
	BOOL bRes;
	pBGR buf;

	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = nx;
	bi.bmiHeader.biHeight = -ny;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = nx * 4 * ny;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	buf = (pBGR)malloc(nx * 4 * ny);
	bRes = GetDIBits(hdcSrc, hBmpSrc, 0, ny, buf, &bi, DIB_RGB_COLORS);
	if (!bRes) {
		free(buf);
		buf = 0;
	}
	return buf;
}

bool FDAI::IsPowered()

{
	if (ACSource && DCSource) {
		if (ACSource->Voltage() > SP_MIN_ACVOLTAGE && DCSource->Voltage() > SP_MIN_DCVOLTAGE)
			return true;
	}

	/// \todo temporary hack for the LM
	if (noAC && DCSource) {
		if (DCSource->Voltage() > SP_MIN_DCVOLTAGE)
			return true;
	}
	return false;
}

void FDAI::SetLMmode()
{
	LM_FDAI = true;
}

void FDAI::AnimateFDAI(VECTOR3 rates, VECTOR3 errors, UINT animR, UINT animP, UINT animY, UINT errorR, UINT errorP, UINT errorY, UINT rateR, UINT rateP, UINT rateY)
{
	double fdai_proc[3];
	double rate_proc[3];

	// Drive FDAI ball
	fdai_proc[0] = now.y / PI2;
	fdai_proc[1] = now.z / PI2;
	fdai_proc[2] = now.x / PI2;
	if (fdai_proc[0] < 0) fdai_proc[0] += 1.0;
	if (fdai_proc[1] < 0) fdai_proc[1] += 1.0;
	if (fdai_proc[2] < 0) fdai_proc[2] += 1.0;
	vessel->SetAnimation(animY, fdai_proc[2]);
	vessel->SetAnimation(animR, fdai_proc[0]);
	vessel->SetAnimation(animP, fdai_proc[1]);

	// Drive error needles
	vessel->SetAnimation(errorR, (errors.x + 46) / 92);
	vessel->SetAnimation(errorP, (-errors.y + 46) / 92);
	vessel->SetAnimation(errorY, (errors.z + 46) / 92);

	// Drive rate needles
	rate_proc[0] = (-rates.z + 1) / 2;
	rate_proc[1] = (-rates.x + 1) / 2;
	rate_proc[2] = (rates.y + 1) / 2;
	if (rate_proc[0] < 0) rate_proc[0] = 0;
	if (rate_proc[1] < 0) rate_proc[1] = 0;
	if (rate_proc[2] < 0) rate_proc[2] = 0;
	if (rate_proc[0] > 1) rate_proc[0] = 1;
	if (rate_proc[1] > 1) rate_proc[1] = 1;
	if (rate_proc[2] > 1) rate_proc[2] = 1;
	vessel->SetAnimation(rateR, rate_proc[0]);
	vessel->SetAnimation(rateP, rate_proc[1]);
	vessel->SetAnimation(rateY, rate_proc[2]);
}

//
// Scenario state functions.
//


void FDAI::SaveState(FILEHANDLE scn, char *start_str, char *end_str)

{
	oapiWriteLine(scn, start_str);

	oapiWriteScenario_float(scn, "TGX", target.x);
	oapiWriteScenario_float(scn, "TGY", target.y);
	oapiWriteScenario_float(scn, "TGZ", target.z);
	oapiWriteScenario_float(scn, "NWX", now.x);
	oapiWriteScenario_float(scn, "NWY", now.y);
	oapiWriteScenario_float(scn, "NWZ", now.z);
	oapiWriteScenario_float(scn, "RATESX", lastRates.x);
	oapiWriteScenario_float(scn, "RATESY", lastRates.y);
	oapiWriteScenario_float(scn, "RATESZ", lastRates.z);
	oapiWriteScenario_float(scn, "ERRORSX", lastErrors.x);
	oapiWriteScenario_float(scn, "ERRORSY", lastErrors.y);
	oapiWriteScenario_float(scn, "ERRORSZ", lastErrors.z);

	oapiWriteLine(scn, end_str);
}

void FDAI::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	float flt = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp(line, "TGX", 3)) {
			sscanf(line + 3, "%f", &flt);
			target.x = flt;
		}
		else if (!strnicmp(line, "TGY", 3)) {
			sscanf(line + 3, "%f", &flt);
			target.y = flt;
		}
		else if (!strnicmp(line, "TGZ", 3)) {
			sscanf(line + 3, "%f", &flt);
			target.z = flt;
		}
		else if (!strnicmp(line, "NWX", 3)) {
			sscanf(line + 3, "%f", &flt);
			now.x = flt;
		}
		else if (!strnicmp(line, "NWY", 3)) {
			sscanf(line + 3, "%f", &flt);
			now.y = flt;
		}
		else if (!strnicmp(line, "NWZ", 3)) {
			sscanf(line + 3, "%f", &flt);
			now.z = flt;
		}
		else if (!strnicmp(line, "RATESX", 6)) {
			sscanf(line + 6, "%f", &flt);
			lastRates.x = flt;
		}
		else if (!strnicmp(line, "RATESY", 6)) {
			sscanf(line + 6, "%f", &flt);
			lastRates.y = flt;
		}
		else if (!strnicmp(line, "RATESZ", 6)) {
			sscanf(line + 6, "%f", &flt);
			lastRates.z = flt;
		}
		else if (!strnicmp(line, "ERRORSX", 7)) {
			sscanf(line + 7, "%f", &flt);
			lastErrors.x = flt;
		}
		else if (!strnicmp(line, "ERRORSY", 7)) {
			sscanf(line + 7, "%f", &flt);
			lastErrors.y = flt;
		}
		else if (!strnicmp(line, "ERRORSZ", 7)) {
			sscanf(line + 7, "%f", &flt);
			lastErrors.z = flt;
		}
	}
}
