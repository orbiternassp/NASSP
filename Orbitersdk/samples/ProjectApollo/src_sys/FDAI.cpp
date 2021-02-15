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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"
#include <stdio.h>

#include "nasspdefs.h"

#include "PanelSDK/PanelSDK.h"
#include "PanelSDK/Internals/Esystems.h"

#include "FDAI.h"

// helper functions
HBITMAP RotateMemoryDC(HBITMAP hBmpSrc, HDC hdcSrc, int SrcX, int SrcY, float angle, HDC &hdcDst, int &dstX, int &dstY);
void DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, short xStart, short yStart, COLORREF cTransparentColor);

FDAI::FDAI() {

	init = 0;
	ScrX = 0;
	ScrY = 0;

	target = _V(0, 0, 0);
	now = _V(0, 0, 0);
	lastRates = _V(0, 0, 0);
	lastErrors = _V(0, 0, 0);

	lastPaintTime = -1;
	newRegistered = false;
	LM_FDAI = false;
	DCSource = NULL;
	ACSource = NULL;
	noAC = false;
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
}

void FDAI::RegisterMe(int index, int x, int y) {

	idx = index;
	ScrX = x;
	ScrY = y;
	oapiRegisterPanelArea(index, _R(ScrX, ScrY, ScrX + 245, ScrY + 245), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
	newRegistered = true;
}


void FDAI::SetAttitude(VECTOR3 attitude) {

	if (!IsPowered())
		return;

	target.y = -attitude.x;	// roll
	target.z = attitude.y;	// pitch
	target.x = attitude.z;	// yaw
}

void FDAI::MoveBall() {

	if (newRegistered) {
		now.x = target.x;
		now.y = target.y;
		now.z = target.z;

		newRegistered = false;

	}
	else {
		double delta;

		delta = target.z - now.z;
		if (delta > 0.05) {
			if (delta > PI) {
				now.z += 2 * PI;
				MoveBall();
				return;
			}
			now.z += 0.05;

		}
		else if (delta < -0.05) {
			if (delta < -PI) {
				now.z -= 2 * PI;
				MoveBall();
				return;
			}
			now.z -= 0.05;
		}
		else
			now.z += delta;

		delta = target.y - now.y;
		if (delta > 0.05) {
			if (delta > PI) {
				now.y += 2 * PI;
				MoveBall();
				return;
			}
			now.y += 0.05;

		}
		else if (delta < -0.05) {
			if (delta < -PI) {
				now.y -= 2 * PI;
				MoveBall();
				return;
			}
			now.y -= 0.05;
		}
		else
			now.y += delta;

		delta = target.x - now.x;
		if (delta > 0.05) {
			if (delta > PI) {
				now.x += 2 * PI;
				MoveBall();
				return;
			}
			now.x += 0.05;

		}
		else if (delta < -0.05) {
			if (delta < -PI) {
				now.x -= 2 * PI;
				MoveBall();
				return;
			}
			now.x -= 0.05;
		}
		else
			now.x += delta;
	}

	glLoadIdentity();
	gluLookAt(0.0, -35.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

	glRotatef(90.0, 0.0, 1.0, 0.0);

	glRotated(now.y / PI * 180.0, 0.0, 1.0, 0.0);	//attitude.x
	glRotated(now.x / PI * 180.0, 1.0, 0.0, 0.0);	//attitude.z
	glRotated(now.z / PI * 180.0, 0.0, 0.0, 1.0);	//attitude.y
}

void FDAI::PaintMe(VECTOR3 attitude, int no_att, VECTOR3 rates, VECTOR3 errors, SURFHANDLE surf, SURFHANDLE hFDAI,
	SURFHANDLE hFDAIRoll, SURFHANDLE hFDAIOff, SURFHANDLE hFDAINeedles, HBITMAP hBmpRoll, int smooth) {

	if (!init) InitGL();

	SetAttitude(attitude);

	// Don't do the OpenGL calculations every timestep
	if (smooth || lastPaintTime == -1 || ((length(now - target) > 0.005 || oapiGetSysTime() > lastPaintTime + 2.0) && oapiGetSysTime() > lastPaintTime + 0.1)) {
		int ret = wglMakeCurrent(hDC2, hRC);
		MoveBall();
		glCallList(list_name);	//render
		glFlush();
		glFinish();

		lastPaintTime = oapiGetSysTime();
	}

	HDC hDC = oapiGetDC(surf);
	BitBlt(hDC, 43, 43, 150, 150, hDC2, 10, 10, SRCCOPY);//then we bitblt onto the panel.

	// roll indicator
	HDC hDCRotate;
	int rotateX, rotateY;
	double angle = -target.y;

	HDC hDCTemp = CreateCompatibleDC(hDC);
	HBITMAP hBmpTemp = (HBITMAP)SelectObject(hDCTemp, hBmpRoll);

	HBITMAP hBmpRotate = RotateMemoryDC(hBmpRoll, hDCTemp, 20, 20, (float)(PI - angle), hDCRotate, rotateX, rotateY);
	HBITMAP hBmpXXX = CreateCompatibleBitmap(hDCRotate, rotateX, rotateY);
	SelectObject(hDCRotate, hBmpXXX);

	double radius = 62;
	// Was + 93 and 92
	int targetX = ((int)(sin(-angle) * radius)) + 123 - ((int)(rotateX / 2));
	int targetY = ((int)(-cos(-angle) * radius)) + 122 - ((int)(rotateY / 2));
	int targetZ = 0;

	DrawTransparentBitmap(hDC, hBmpRotate, targetX, targetY, 0x00FF00FF);

	DeleteObject(hBmpXXX);
	DeleteObject(hBmpTemp);
	DeleteObject(hBmpRotate);
	DeleteDC(hDCTemp);
	DeleteDC(hDCRotate);

	oapiReleaseDC(surf, hDC);

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

	if (!LM_FDAI)
	{
		//Input is scaled -1.0 to 1.0, this scales to 126 pixels
		targetX = (int)(117 + (63.0 * rates.z));
		targetY = (int)(117 - (63.0 * rates.x));
		targetZ = (int)(117 - (63.0 * rates.y));
	}
	else
	{
		//Input is scaled -1.0 to 1.0, this scales to 100 pixels
		targetX = (int)(119 + (50.0*rates.z));
		targetY = (int)(120 - (50.0*rates.x));
		targetZ = (int)(119 - (50.0*rates.y));
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
	if (LM_FDAI && (!IsPowered() || no_att != 0))
		oapiBlt(surf, hFDAIOff, 31, 100, 0, 0, 13, 30, SURF_PREDEF_CK);
}

void FDAI::Timestep(double simt, double simdt) {

	// Check for external view and disable smooth movement 
	if (oapiCameraMode() != CAM_COCKPIT)
		newRegistered = true;
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

void DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, short xStart,
	short yStart, COLORREF cTransparentColor) {

	BITMAP     bm;
	COLORREF   cColor;
	HBITMAP    bmAndBack, bmAndObject, bmAndMem, bmSave;
	HBITMAP    bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
	HDC        hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT      ptSize;

	hdcTemp = CreateCompatibleDC(hdc);
	SelectObject(hdcTemp, hBitmap);   // Select the bitmap

	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
	ptSize.x = bm.bmWidth;            // Get width of bitmap
	ptSize.y = bm.bmHeight;           // Get height of bitmap
	DPtoLP(hdcTemp, &ptSize, 1);      // Convert from device

									  // to logical points

	// Create some DCs to hold temporary data.
	hdcBack = CreateCompatibleDC(hdc);
	hdcObject = CreateCompatibleDC(hdc);
	hdcMem = CreateCompatibleDC(hdc);
	hdcSave = CreateCompatibleDC(hdc);

	// Create a bitmap for each DC. DCs are required for a number of
	// GDI functions.

	// Monochrome DC
	bmAndBack = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

	// Monochrome DC
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

	bmAndMem = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	bmSave = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

	// Each DC must select a bitmap object to store pixel data.
	bmBackOld = (HBITMAP)SelectObject(hdcBack, bmAndBack);
	bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
	bmMemOld = (HBITMAP)SelectObject(hdcMem, bmAndMem);
	bmSaveOld = (HBITMAP)SelectObject(hdcSave, bmSave);

	// Set proper mapping mode.
	SetMapMode(hdcTemp, GetMapMode(hdc));

	// Save the bitmap sent here, because it will be overwritten.
	BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	// Set the background color of the source DC to the color.
	// contained in the parts of the bitmap that should be transparent
	cColor = SetBkColor(hdcTemp, cTransparentColor);

	// Create the object mask for the bitmap by performing a BitBlt
	// from the source bitmap to a monochrome bitmap.
	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0,
		SRCCOPY);

	// Set the background color of the source DC back to the original
	// color.
	SetBkColor(hdcTemp, cColor);

	// Create the inverse of the object mask.
	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0,
		NOTSRCCOPY);

	// Copy the background of the main DC to the destination.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, xStart, yStart,
		SRCCOPY);

	// Mask out the places where the bitmap will be placed.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);

	// Mask out the transparent colored pixels on the bitmap.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);

	// XOR the bitmap with the background on the destination DC.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);

	// Copy the destination to the screen.
	BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, 0, 0,
		SRCCOPY);

	// Place the original bitmap back into the bitmap sent here.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

	// Delete the memory bitmaps.
	DeleteObject(SelectObject(hdcBack, bmBackOld));
	DeleteObject(SelectObject(hdcObject, bmObjectOld));
	DeleteObject(SelectObject(hdcMem, bmMemOld));
	DeleteObject(SelectObject(hdcSave, bmSaveOld));

	// Delete the memory DCs.
	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcSave);
	DeleteDC(hdcTemp);
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

// RotateMemoryDC rotates a memory DC and returns the rotated DC as well as its dimensions
HBITMAP RotateMemoryDC(HBITMAP hBmpSrc, HDC hdcSrc, int SrcX, int SrcY, float angle, HDC &hdcDst, int &dstX, int &dstY)
{
	HBITMAP hBmpDst;
	float x1, x2, x3, x4, y1, y2, y3, y4, cA, sA;
	float CtX, CtY, orgX, orgY, divisor;
	int OfX, OfY;
	int stepX, stepY;
	int iorgX, iorgY;
	RECT rt;
	pBGR src, dst, dstLine;
	BITMAPINFO bi;

	// Rotate the bitmap around the center
	CtX = ((float)SrcX) / 2;
	CtY = ((float)SrcY) / 2;

	// First, calculate the destination positions for the four courners to get dstX and dstY
	cA = (float)cos(angle);
	sA = (float)sin(angle);

	x1 = CtX + (-CtX) * cA - (-CtY) * sA;
	x2 = CtX + (SrcX - CtX) * cA - (-CtY) * sA;
	x3 = CtX + (SrcX - CtX) * cA - (SrcY - CtY) * sA;
	x4 = CtX + (-CtX) * cA - (SrcY - CtY) * sA;

	y1 = CtY + (-CtY) * cA + (-CtX) * sA;
	y2 = CtY + (SrcY - CtY) * cA + (-CtX) * sA;
	y3 = CtY + (SrcY - CtY) * cA + (SrcX - CtX) * sA;
	y4 = CtY + (-CtY) * cA + (SrcX - CtX) * sA;

	OfX = ((int)floor(min4(x1, x2, x3, x4)));
	OfY = ((int)floor(min4(y1, y2, y3, y4)));

	dstX = ((int)ceil(max4(x1, x2, x3, x4))) - OfX;
	dstY = ((int)ceil(max4(y1, y2, y3, y4))) - OfY;

	// Create the new memory DC
	hdcDst = CreateCompatibleDC(hdcSrc);
	hBmpDst = CreateCompatibleBitmap(hdcSrc, dstX, dstY);
	SelectObject(hdcDst, hBmpDst);

	// Fill the new memory DC with the current Window color
	rt.left = 0;
	rt.top = 0;
	rt.right = dstX;
	rt.bottom = dstY;

	HBRUSH brush = CreateSolidBrush(0x00FF00FF);
	HBRUSH brushTemp = (HBRUSH)SelectObject(hdcDst, brush);
	FillRect(hdcDst, &rt, brush);
	SelectObject(hdcDst, brushTemp);
	DeleteObject(brush);

	// Get the bitmap bits for the source and destination
	src = MyGetDibBits(hdcSrc, hBmpSrc, SrcX, SrcY);
	dst = MyGetDibBits(hdcDst, hBmpDst, dstX, dstY);

	dstLine = dst;
	divisor = cA*cA + sA*sA;
	// Step through the destination bitmap
	for (stepY = 0; stepY < dstY; stepY++) {
		for (stepX = 0; stepX < dstX; stepX++) {
			// Calculate the source coordinate
			orgX = (cA * (((float)stepX + OfX) + CtX * (cA - 1)) + sA * (((float)stepY + OfY) + CtY * (sA - 1))) / divisor;
			orgY = CtY + (CtX - ((float)stepX + OfX)) * sA + cA *(((float)stepY + OfY) - CtY + (CtY - CtX) * sA);
			iorgX = (int)orgX;
			iorgY = (int)orgY;
			if ((iorgX >= 0) && (iorgY >= 0) && (iorgX < SrcX) && (iorgY < SrcY)) {
				// Inside the source bitmap -> copy the bits
				dstLine[dstX - stepX - 1] = src[iorgX + iorgY * SrcX];
			}
			else {
				// Outside the source -> set the color to magenta
				dstLine[dstX - stepX - 1].b = 255;
				dstLine[dstX - stepX - 1].g = 0;
				dstLine[dstX - stepX - 1].r = 255;
			}
		}
		dstLine = dstLine + dstX;
	}

	// Set the new Bitmap
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = dstX;
	bi.bmiHeader.biHeight = dstY;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = dstX * 4 * dstY;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;
	SetDIBits(hdcDst, hBmpDst, 0, dstY, dst, &bi, DIB_RGB_COLORS);

	// Free the color arrays
	free(src);
	free(dst);

	return hBmpDst;
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
