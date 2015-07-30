/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2005 Radu Poenaru

  System & Panel SDK (SPSDK)

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

#include "instruments.h"
#include <stdio.h>
#include <math.h>
#include < GL\gl.h >
#include < GL\glu.h >

instrument::instrument(int x, int y,Panel* i_parent) //basic constructor. Sets position and registers the instrument
{ ScrX=x;ScrY=y;parent=i_parent;
  parent->AddInstrument(this);
type=30; //void instrument
name[0]=0;
};


CText::CText(int x, int y, int lng, int dir,char *tt,Panel *i_parent):instrument(x,y,i_parent)
{
length=lng;
direction=dir;
strcpy(text,tt);
slen=strlen(text)*8; //length of text in pixels
}
void CText::SetColor(int red,int green,int blue)
{color[0]=red;color[1]=green;color[2]=blue;}
void CText::SetNoCol(int red,int green,int blue)
{nocol[0]=red;nocol[1]=green;nocol[2]=blue;};
void CText::RegisterMe(int index)
{
int size= slen;
if (size<length) size=length;
oapiRegisterPanelArea(index,_R(ScrX-size/2,ScrY,ScrX+size/2,ScrY+height),PANEL_REDRAW_USER,0,PANEL_MAP_BACKGROUND);
idx=index;
};

void CText::PaintMe()
{
  HDC hDC=oapiGetDC(parent->surf);
  SetBkMode(hDC,TRANSPARENT);

  if (parent->text_logic) {SetTextColor(hDC,RGB(color[0],color[1],color[2]));
						   SelectObject(hDC,parent->GDI_res->hPEN[ColPenIndex]);
							}
  else					   {SetTextColor(hDC,RGB(nocol[0],nocol[1],nocol[2]));
							 SelectObject(hDC,parent->GDI_res->hPEN[NoColPenIndex]);
							};
  SetTextAlign(hDC,TA_CENTER);
  SetBkMode(hDC,TRANSPARENT);
  SelectObject(hDC,parent->GDI_res->hFNT_Panel[FontIndex]);
  if (direction){
  MoveToEx(hDC,length-1, direction*7+5,NULL);
  LineTo(hDC,length-1,5);
  LineTo(hDC,length/2+slen/2,5);
  MoveToEx(hDC,length/2-slen/2, 5,NULL);
  LineTo(hDC,0,5);
  LineTo(hDC,0,direction*7+5);};
  TextOut(hDC,length/2,0,text, sizeof(char)*strlen(text));

 oapiReleaseDC(parent->surf,hDC);
};
void CText::SetLight()
{
	oapiTriggerPanelRedrawArea(parent->idx,idx);
};


Switch::Switch(int x, int y, int sx,int sy,int i_num_pos,int i_sp, int *i_SRC,Panel *i_parent):instrument(x,y,i_parent)
{ num_pos=i_num_pos;name[0]=0;
  size_x=sx;size_y=sy;
  spring=i_sp;if (num_pos<3) spring=0; //can't spring on 2 pos. only 3
  SRC=i_SRC;type=33; //switch
 if (SRC) pos=*SRC;
 else {SRC=&pos;pos=-1;};
};

void Switch::RegisterMe(int index)
{oapiRegisterPanelArea(index,_R(ScrX,ScrY,ScrX+size_x+1,ScrY+size_y+1),PANEL_REDRAW_MOUSE,15,PANEL_MAP_BACKGROUND);
 idx=index;
};
void Switch::PaintMe()
{ if (SRF_index){
	if (parent->panel_logic)
	oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],0,0,size_x-pos*size_x,size_y,size_x,size_y,0xFFFFFF);
	else
	oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],0,0,size_x-pos*size_x,0,size_x,size_y,0xFFFFFF);
}
};

void Switch::LBD(int x, int y)
{ if (pos<1)
		{pos+=4-num_pos;
		 if (SRC) *SRC=pos;
	   //  oapiTriggerPanelRedrawArea(parent->idx,idx);


				};
};
void Switch::RBD(int x, int y)
{ if (pos>-1) {pos-=4-num_pos;
			   if (SRC) *SRC=pos;
		   //    oapiTriggerPanelRedrawArea(parent->idx,idx);


				};
};
void Switch::BU()
{
	if (spring) {pos=0;
				if (SRC) *SRC=0;
	oapiTriggerPanelRedrawArea(parent->idx,idx);}
};
void Switch::SetLight()
{	oapiTriggerPanelRedrawArea(parent->idx,idx);}
void* Switch::GetComponent(char *component_name)
{
 if (!strnicmp (component_name, "SRCP",4 )) return &SRC;
 if (!strnicmp (component_name, "SRC",3 )) return SRC;

 return NULL;
}
CodeSwitch::CodeSwitch(int x, int y, int sx,int sy,int i_num_pos,int i_sp,
					    int i_code_index,int *i_SRC,Panel *i_parent):Switch(x,y,sx,sy,i_num_pos,i_sp,i_SRC,i_parent)
{code_index=i_code_index;};
void CodeSwitch::LBD(int x, int y)
{ if (pos<1)
		{pos+=4-num_pos;oapiTriggerPanelRedrawArea(parent->idx,idx);
		 *SRC=pos;};
 parent->CodeCallFromSwitch(code_index);
};

void CodeSwitch::RBD(int x, int y)
{if (pos>-1) {pos-=4-num_pos;oapiTriggerPanelRedrawArea(parent->idx,idx);
			   *SRC=pos;
				};
parent->CodeCallFromSwitch(code_index);
};

Rotational::Rotational(int x, int y, Panel *i_parent):instrument(x,y,i_parent)
{
 G=NULL;
 last_paint=0.0;
 local_blt=NULL;

};
void Rotational::SetCenter(int x, int y)
{needle_x=x;needle_y=y;}
void Rotational::SetNeedleSize(int w, int h)
{local_blt=oapiCreateSurface(2+2*h,2+2*h);
 board_size=h;
 TR[0].x=-w/2;TR[0].y=0;
 TR[1].x=+w/2;TR[1].y=0;
 TR[2].x=0;TR[2].y=h;
};
void Rotational::SetRange(double i_max_a,double i_max_v, double i_min_a,double i_min_v)
{
max_a=i_max_a/180.0*PI;max_v=i_max_v;
min_a=i_min_a/180.0*PI;min_v=i_min_v;
};
void Rotational::SetSize(int x, int y)
{size_x=x;size_y=y;
}
Rotational::~Rotational()
{if (local_blt)
	oapiDestroySurface(local_blt);
};
void Rotational::RegisterMe(int index)
{
 oapiRegisterPanelArea(index,_R(ScrX,ScrY,ScrX+size_x+1,ScrY+size_y+1),PANEL_REDRAW_ALWAYS,0,PANEL_MAP_BACKGROUND);
 idx=index;
};
void Rotational::PaintMe()
{
if (parent->panel_logic)
 oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],0,0,0,size_y,size_x,size_y);
else
 oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],0,0,0,0,size_x,size_y);

HDC hDC=oapiGetDC(local_blt);
SelectObject(hDC,parent->GDI_res->hBRUSH_TotalWhite);
SelectObject(hDC,parent->GDI_res->hPEN_NULL);
Rectangle(hDC,0,0,2*board_size+1,2*board_size+1);

double ang;
if (G)
  ang=(*G-min_v)/(max_v-min_v)*(max_a-min_a)+min_a;// angle in radians
if (last_paint>ang){if (last_paint-ang<0.01) last_paint=ang;
					else last_paint-=0.01;
					};
if (last_paint<ang) {if (ang-last_paint<0.01) last_paint=ang;
					else last_paint+=0.01;
					};

ang=last_paint;
POINT S[3];

for (int i=0;i<3;i++) // rotate the pointer by 'ang'
{
   S[i].x = (long)(board_size+ TR[i].x*cos(ang)-TR[i].y*sin(ang));
   S[i].y = (long)(board_size+ TR[i].x*sin(ang)+TR[i].y*cos(ang));
}
SelectObject(hDC,parent->GDI_res->hBRUSH_TotalBlack);
SelectObject(hDC,parent->GDI_res->hPEN_NULL);

Polygon(hDC,S,3);// then the pointer
oapiReleaseDC(local_blt,hDC);
//we need to find the blit point aswell
//double blt_x=33 - cos(ang)*20; //40-5-sin(angle)*radiu
//double blt_y=30 - sin(ang)*20;
oapiBlt(parent->surf,local_blt,needle_x-board_size,needle_y-board_size,0,0,2*board_size,2*board_size,0xFFFFFF);

};
void Rotational::RefreshMe()
{PaintMe(); //to refresh me is to paint me ;-)
};
void* Rotational::GetComponent(char *component_name)
{
 if (!strnicmp (component_name, "SRCP",4 )) return &G;
 if (!strnicmp (component_name, "SRC",3 )) return G;
 return NULL;
}
small_clock::small_clock(int x, int y,int i_size, double *i_val, Panel *i_parent):instrument(x,y, i_parent)
{ if (i_val) val=i_val;
  else val=&last_value;
size=i_size;
last_value=-1;
};
void small_clock::RegisterMe(int index)
{idx=index;
oapiRegisterPanelArea(index,_R(ScrX,ScrY,ScrX+size*size_x,ScrY+size_y),PANEL_REDRAW_ALWAYS,0,PANEL_MAP_CURRENT);
};
void small_clock::PaintMe()
{
char bit_blf[20];
char format_string[20];
sprintf(format_string,"%%%i.0f",size);
sprintf(bit_blf,format_string,last_value);
for (int i=0;i<size;i++)
{	if (bit_blf[i]==32)//space
			oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],i*size_x,0,0,0,size_x,size_y);	//put a 0 there
	else if (bit_blf[i]=='.')
			oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],i*size_x,0,0,0,size_x,size_y);	//put a 0 there;//comma

	else
	oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],i*size_x,0,(bit_blf[i]-48)*size_x,0,size_x,size_y);

}

}
void small_clock::RefreshMe()
{
    if (fabs(*val)>1e20) {last_value=-1;val=&last_value;PaintMe();};
	if (*val!=last_value)
	{last_value=*val;
	PaintMe();
	};
};
void* small_clock::GetComponent(char *component_name)
{
 if (!strnicmp (component_name, "SRCP",4 )) return &val;
 if (!strnicmp (component_name, "SRC",3 )) return val;

 return NULL;
}

int LoadOGLBitmap(char *filename)
{
   unsigned char *l_texture;
   int l_index, l_index2=0;
   FILE *file;
   BITMAPFILEHEADER fileheader;
   BITMAPINFOHEADER infoheader;
   RGBTRIPLE rgb;
   int num_texture=1; //we only use one OGL texture ,so...


   if( (file = fopen(filename, "rb"))==NULL) return (-1);
   fread(&fileheader, sizeof(fileheader), 1, file);
   fseek(file, sizeof(fileheader), SEEK_SET);
   fread(&infoheader, sizeof(infoheader), 1, file);

   l_texture = (byte *) malloc(infoheader.biWidth * infoheader.biHeight * 4);
   memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);

   for (l_index=0; l_index < infoheader.biWidth*infoheader.biHeight; l_index++)
   {
      fread(&rgb, sizeof(rgb), 1, file);

      l_texture[l_index2+0] = rgb.rgbtRed; // Red component
      l_texture[l_index2+1] = rgb.rgbtGreen; // Green component
      l_texture[l_index2+2] = rgb.rgbtBlue; // Blue component
      l_texture[l_index2+3] = 255; // Alpha value
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
};

ADI::ADI(int x,int y, Panel *i_parent):instrument(x,y,i_parent)
{
type= 44; //ADI ball
init=0;
radius=10;
sphere_resolution=25;	//let's try a 20..
int i;
float trad;
float Pi = (float) acos(-1.0);
int indx;


reference.x=0.0;reference.y=0.0;reference.z=0.0;//ecliptic is also first reference
now.x=0.37;now.y=-1.2;now.z=0.5;
float norm;
double a2 = 2.0 * acos(-1.0);
//we define the ADI sphere to be rendered by OpenGL
//we use a 6 by 12 sphere, with 84 vertices and 72 triangles
//bottom point
tri=0;//number of triangle strips
for (i=0;i<sphere_resolution;i++)
  for (int j=0;j<sphere_resolution;j++)
		{ trad = (float)(cos(-Pi/2+Pi/(sphere_resolution-1)*i)*radius);
		  indx=i*sphere_resolution+j;
		 sphere_vert[indx][0] = (float)(cos(a2/(sphere_resolution-1)*j)*trad);
		 sphere_vert[indx][1] = (float)(sin(a2/(sphere_resolution-1)*j)*trad);
		 sphere_vert[indx][2] = (float)(sin(-Pi/2+Pi/(sphere_resolution-1)*i)*radius);
		 sphere_tex[indx][0]=(float)j/(sphere_resolution-1);
		 sphere_tex[indx][1]=(float)i/(sphere_resolution-1);
		 norm = (float)(sqrt(pow(sphere_vert[indx][0],2) +
				             pow(sphere_vert[indx][1],2) +
				             pow(sphere_vert[indx][2],2)));
		 sphere_norm[indx][0]=sphere_vert[indx][0]/norm;
		 sphere_norm[indx][1]=sphere_vert[indx][1]/norm;
		 sphere_norm[indx][2]=sphere_vert[indx][2]/norm;
		 if (i>0) {
				sphere_index[tri++]=indx;
				sphere_index[tri++]=indx-sphere_resolution;

					};
		};


};
void ADI::InitGL(char *bitmapname)
{

GLuint      PixelFormat;
BITMAPINFOHEADER BIH;
int iSize=sizeof(BITMAPINFOHEADER);
BIH.biSize=iSize;
BIH.biWidth=size_x+20;				//size of the sphere is 160x160
BIH.biHeight=size_y+20;
BIH.biPlanes=1;
BIH.biBitCount=16;//default is 16.
BIH.biCompression=BI_RGB;
BIH.biSizeImage=0;
void* m_pBits;
hDC2=CreateCompatibleDC(NULL);//we make a new DC and DIbitmap for OpenGL to draw onto
static  PIXELFORMATDESCRIPTOR pfd2;
DescribePixelFormat(hDC2,1,sizeof(PIXELFORMATDESCRIPTOR),&pfd2);//just get a random pixel format..
BIH.biBitCount=pfd2.cColorBits;//to get the current bit depth.. !?
hBMP=CreateDIBSection(hDC2,(BITMAPINFO*)&BIH,DIB_RGB_COLORS,&m_pBits,NULL,0);
hBMP_old=(HBITMAP)SelectObject(hDC2,hBMP);

static  PIXELFORMATDESCRIPTOR pfd={                             // pfd Tells Windows How We Want Things To Be

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
pfd.cColorBits=pfd2.cColorBits;//same color depth needed.
DWORD code;
code=GetLastError();

PixelFormat=ChoosePixelFormat(hDC2,&pfd);// now pretend we want a new format
int ret;
ret=SetPixelFormat(hDC2,PixelFormat,&pfd);
code=GetLastError();
hRC=wglCreateContext(hDC2);
ret=wglMakeCurrent(hDC2,hRC);					//all standard OpenGL init so far

//We load the texture
int texture_index=LoadOGLBitmap(bitmapname+1);
if (texture_index>0) glEnable(GL_TEXTURE_2D);


glShadeModel(GL_SMOOTH);                        // Enable Smooth Shading
glClearColor(0.0f, 0.0f, 0.0f, 0.0f);           // Panel Background color
glClearDepth(1.0f);                             // Depth Buffer Setup
glEnable(GL_DEPTH_TEST);                        // Enables Depth Testing
glDepthFunc(GL_LESS);                           // The Type Of Depth Testing To Do
glViewport(0,0,size_x+20,size_y+20);                       // Reset The Current Viewport
glMatrixMode(GL_PROJECTION);                        // Select The Projection Matrix
glLoadIdentity();                                   // Reset The Projection Matrix
gluPerspective(45.0f,1.0,1.0f,1000.0f);
glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
glLoadIdentity();                                   // Reset The Projection Matrix

//some ambiental setup
GLfloat light_position[]={0.0,-10.0,10.0,0.0};
GLfloat light_diffuse[]={ambient[0],ambient[1],ambient[2],1.0};
GLfloat light_ambient[]={ambient[0],ambient[1],ambient[2],1.0};
GLfloat mat_specular[] = {(float) 0.3, (float) 0.3, (float) 0.3, 1.0};
GLfloat mat_shin[]={5.0};
glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
glEnable(GL_LIGHTING);
glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
glLightfv(GL_LIGHT0,GL_POSITION,light_position);
glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
glLightfv(GL_LIGHT0,GL_SPECULAR,mat_specular);
glEnable(GL_LIGHT0);


//defining our geometry and composing a display list;
glEnableClientState(GL_NORMAL_ARRAY);
if (texture_index>0) glEnableClientState(GL_TEXTURE_COORD_ARRAY);
glEnableClientState(GL_VERTEX_ARRAY);
glVertexPointer(3,GL_FLOAT,0,sphere_vert);
glNormalPointer(GL_FLOAT,0,sphere_norm);
if (texture_index>0) glTexCoordPointer(2,GL_FLOAT,0,sphere_tex);
list_name=glGenLists(1);
glNewList(list_name,GL_COMPILE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);             // Clear The Screen And The Depth Buffer
	glColor3f(1.0,1.0,1.0);
    glDrawElements(GL_TRIANGLE_STRIP,tri,GL_UNSIGNED_INT,sphere_index);
glEndList();

init=1;		//that's it. If we made it so far, we can use OpenGL
};

ADI::~ADI()
{
wglMakeCurrent(NULL,NULL);	//standard OpenGL release
wglDeleteContext(hRC);
hRC=NULL;
SelectObject(hDC2,hBMP_old);//remember to delete DC and bitmap memory we created
DeleteObject(hBMP);
DeleteDC(hDC2);

};

void ADI::SetZoom(double y)
{zoom = (float) -y;};
void ADI::SetAmbient(int red,int green,int blue)
{ambient[0] = (float)(red / 255.0); ambient[1] = (float)(green / 255.0); ambient[2] = (float)(blue / 255.0);};
void ADI::SetLight(int red,int green,int blue)
{color[0] = (float)(red / 255.0); color[1] = (float)(green / 255.0); color[2] = (float)(blue / 255.0);};
void ADI::RegisterMe(int index)
{oapiRegisterPanelArea(index,_R(ScrX,ScrY,ScrX+size_x,ScrY+size_y),PANEL_REDRAW_ALWAYS,PANEL_MOUSE_IGNORE,PANEL_MAP_CURRENT);
 idx=index;
};


/*
void ADI::SetEquatorial()
{ float Pi=acos(-1);
  float pitch=parent->v->GetPitch();
  double heading;
  oapiGetHeading(parent->v->GetHandle(),&heading);
  heading+=Pi/2;
  heading=2*Pi-heading;
  //heading+=Pi;
  double bank=-(parent->v->GetBank());
  target.x=heading;		//heading
  target.y=pitch;		//pitch
  target.z=bank;		//roll

}*/

void ADI::MoveBall()
{   float delta;
	float Pi = (float) acos(-1.0);
    over_rate=0.0;
    delta =- (float)((*ROLL) - now.z);
	if (delta>0.05) {if (delta>Pi) {now.z+=2*Pi;MoveBall();
									return;}
						now.z+=0.05;over_rate=1.;}
	else if (delta<-0.05){ if (delta<-Pi) {now.z-=2*Pi;MoveBall();
								return;}
						now.z-=0.05;over_rate=1.;}
	else now.z+=delta;

	delta = (float)(*PITCH - now.y);
	if (delta>0.05) {if (delta>Pi) {now.y+=2*Pi;MoveBall();
									return;}
					now.y+=0.05;over_rate=1.;}
	else if (delta<-0.05) {if (delta<-Pi) {now.y-=2*Pi;MoveBall();
								return;}
						now.y-=0.05;over_rate=1.;}
	else now.y+=delta;

	delta = (float)((2*Pi-(*YAW)+Pi/2)-now.x);
	if (delta>0.05) {if (delta>Pi) {now.x+=2*Pi;MoveBall();
									return;}
						now.x+=0.05;over_rate=1.;}
	else if (delta<-0.05) {if (delta<-Pi) {now.x-=2*Pi;MoveBall();
								return;}
						now.x-=0.05;over_rate=1.;}
	else now.x+=delta;

	glLoadIdentity();
	gluLookAt(0.0,zoom,0.0, 0.0,0.0,0.0,0.0,0.0,1.0);
	glRotatef((float)(-now.z/Pi*180.0),0.0,1.0,0.0);
	glRotatef((float)(now.y/Pi*180.0),1.0,0.0,0.0);
	glRotatef((float)(now.x/Pi*180.0),0.0,0.0,1.0);

}
void ADI::PaintMe()
{
	if (!init) InitGL("textures\\adi.dds");

	 MoveBall();
	//else {over_rate=1;};
	glCallList(list_name);	//render
	glFlush();
	glFinish();

HDC hDC=oapiGetDC(parent->surf);

BitBlt(hDC,0,0,size_x,size_y,hDC2,0,0,SRCCOPY);//then we bitblt onto the panel. wish there
oapiReleaseDC(parent->surf,hDC);		// was a faster way ...

oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],0,0,0,0,size_x,size_y,0x0);

}
void ADI::RefreshMe()
{
PaintMe();
}

void ADI::SetLight()
{ if (parent->panel_logic)
	{GLfloat light_diffuse[]={color[0],color[1],color[2]};
	 glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
	}
else
	{GLfloat light_diffuse[]={ambient[0],ambient[1],ambient[2]};
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
	};
};

void *ADI::GetComponent(char *component_name)
{
//if (!strnicmp (component_name, "FUNCTION_MODE",13 )) return &function_mode;
//if (!strnicmp (component_name, "ORB_ECL",7 )) return &orbital_ecliptic;
//if (!strnicmp (component_name, "REF",3 )) return &ref_handle;
 if (!strnicmp (component_name, "PITCHP",6 )) return &PITCH;
 if (!strnicmp (component_name, "PITCH",5 )) return PITCH;

 if (!strnicmp (component_name, "ROLLP",5 )) return &ROLL;
 if (!strnicmp (component_name, "ROLL",4 )) return ROLL;

 if (!strnicmp (component_name, "YAWP",4 )) return &YAW;
 if (!strnicmp (component_name, "YAW",3 )) return YAW;

return NULL;
}
Linear::Linear(int x, int y,double i_min,double i_max,double i_s,double i_b,double* i_src, Panel *i_parent):instrument(x,y,i_parent)
{
SRC1=i_src;
scale=i_s;bias=i_b;
max=i_max;min=i_min;
last_x=-1;last_y=-1;
last_light=0;bkg_printed=0;
last1=1e10;
local_blt=NULL;
}
Linear::~Linear()
{if (local_blt)
oapiDestroySurface(local_blt);
}
void Linear::SetAngle(double ang)
{angle=ang;
}
void Linear::DoNeedle()
{
local_blt=oapiCreateSurface(needle_h+3,needle_h+3);
POINT TR[3];
TR[0].x = (long)(-needle_h/2.0); TR[0].y = (long)(-needle_w/2.0);
TR[1].x = (long)(-needle_h/2.0); TR[1].y = (long)(needle_w/2.0);
TR[2].x = (long)(needle_h/2.0);  TR[2].y=0;
POINT S[3];
for (int i=0;i<3;i++) // rotate the pointer by 'ang'
{
   S[i].x = (long)(1+needle_h/2.0+ TR[i].x*cos(angle)-TR[i].y*sin(angle));
   S[i].y = (long)(1+needle_h/2.0+ TR[i].x*sin(angle)+TR[i].y*cos(angle));
}
HDC hDC=oapiGetDC(local_blt);
 //SelectObject(hDC,parent->GDI_res->hBRUSH_TotalWhite);
 //SelectObject(hDC,parent->GDI_res->hPEN_NULL);
 Rectangle(hDC,0,0,needle_h+2,needle_h+2);

 SelectObject(hDC,parent->GDI_res->hBRUSH_TotalBlack);
 SelectObject(hDC,parent->GDI_res->hPEN_NULL);
 Polygon(hDC,S,3);// draw the pointer
oapiReleaseDC(local_blt,hDC);
}
void Linear::SetNeedleSize(int w, int h)
{needle_h=h;
 needle_w=w;
};
void Linear::SetMax(double x,double y)
{max_x=x;max_y=y;};
void Linear::SetMin(double x,double y)
{min_x=x;min_y=y;};
void Linear::RegisterMe(int index)
{idx=index;
oapiRegisterPanelArea(index,_R(ScrX,ScrY,ScrX+size_x+1,ScrY+size_y+1),PANEL_REDRAW_ALWAYS,PANEL_MOUSE_IGNORE,PANEL_MAP_CURRENT);
};

int  Linear::GetPixel1()
{
if (SRC1){
double x=((*SRC1)+bias)/scale; //actual value to display
x=(x-min)/(max-min);// clamped to a [0..1] value
if (x>1.0) x=1.0;
if (x<0.0) x=0.0;
if (x!= last1){
	last1=x;
	point_x = (int)(min_x+x*(max_x-min_x));
	point_y = (int)(min_y+x*(max_y-min_y));
	return 1;}
};
return 0;
}


void Linear::PrintBackground()
{
//first, copy the appropriate background,
 if (parent->panel_logic)
	oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],0,0,size_x,0,size_x,size_y);
 else
    oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],0,0,0,0,size_x,size_y);
 bkg_printed=1;
};
void Linear::PaintMe()
{
if (!local_blt) DoNeedle();
if (!bkg_printed)
PrintBackground();

GetPixel1();
if (parent->panel_logic)
oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],last_x,last_y,size_x+last_x,last_y,needle_h,needle_h);

else
oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],last_x,last_y,last_x,last_y,needle_h,needle_h);

//oapiBlt(parent->surf,local_blt,6,last1,1,0,9,9,0xFFFFFF);
oapiBlt(parent->surf,local_blt,point_x,point_y,1,1,needle_h,needle_h,0xFFFFFF);
last_x=point_x;
last_y=point_y;
};


void Linear::RefreshMe()
{
 if (GetPixel1())
		{bkg_printed=1;
		PaintMe();
		bkg_printed=0;
		}


}
void Linear::SetLight()
{	bkg_printed=0;
	oapiTriggerPanelRedrawArea(parent->idx,idx);

}
void* Linear::GetComponent(char *component_name)
{
    if (!strnicmp (component_name, "SRCP",4 )) return &SRC1;
	if (!strnicmp (component_name, "SRC",3 )) return SRC1;

 return NULL;
}


TB::TB(int x, int y, double *i_src,int i_type,Panel *i_parent):instrument(x,y,i_parent)
{SRC=i_src; type=i_type;paint_on=0;blink=0;timer=0;}

void TB::RegisterMe(int index)
{idx=index;
oapiRegisterPanelArea(index,_R(ScrX,ScrY,ScrX+size_x,ScrY+size_y),PANEL_REDRAW_ALWAYS,PANEL_MOUSE_IGNORE,PANEL_MAP_CURRENT);
};
void TB::PaintMe()
{
if ((IsOn())){
if (blink){
if (paint_on==1)
	oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],0,0,size_x,0,size_x,size_y);
else
    oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],0,0,2*size_x,0,size_x,size_y);

}else {

	oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],0,0,size_x,0,size_x,size_y);
	paint_on=1;
	}

}
else{
    oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],0,0,0,0,size_x,size_y);
	paint_on=0;blink_on=0;timer=0;
};

}
void TB::RefreshMe()
{
	if (IsOn())
		{

		if (blink)

		{ timer+=oapiGetSysStep();
		  if (timer>1/blink) //time's up
		  {	timer=0;
			paint_on+=1;if (paint_on>2) paint_on=1;
			PaintMe();
		  }
		}
		else if (!paint_on)
			PaintMe();
		}
	else if (paint_on) PaintMe();

}
int TB::IsOn()
{
if (SRC)
{if ((*SRC>0)&&(type==1))// LARGER_ZERO
	return 1;
 if ((*SRC<0)&&(type==2))// SMALLER_ZERO
	return 1;
 if ((*SRC)&&(type==3)) //NON_ZERO
	 return 1;
 if ((!*SRC)&&(type==4)) //ZERO
	 return 1;
 return 0;
}
return 0;
}
void* TB::GetComponent(char *component_name)
{
    if (!strnicmp (component_name, "SRCP",4 )) return &SRC;
	if (!strnicmp (component_name, "SRC",3 )) return SRC;

 return NULL;
}

Rot::Rot(int x, int y,int i_p,int i_max,int i_min,Panel *i_parent):instrument(x,y,i_parent)
{max=i_max;min=i_min;c_pos=i_p;}
void Rot::RegisterMe(int index)
{idx=index;
oapiRegisterPanelArea(index,_R(ScrX,ScrY,ScrX+size_x,ScrY+size_y),PANEL_REDRAW_MOUSE,15,PANEL_MAP_CURRENT);
};

void Rot::PaintMe()
{
oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],0,0,(c_pos-min)*size_x,0,size_x,size_y,0xFFFFFF);
}
void Rot::RefreshMe()
{PaintMe();}

void Rot::LBD(int x, int y)
{	if (c_pos>min ) c_pos--;
oapiTriggerPanelRedrawArea(parent->idx,idx);};
void Rot::RBD(int x, int y)
{ if (c_pos<max) c_pos++;
oapiTriggerPanelRedrawArea(parent->idx,idx);};

void *Rot::GetComponent(char *component_name)
{
if (!strnicmp (component_name, "SEL",3 )) return &c_pos;
return NULL;
}


RotEl::RotEl(double **i_SRC,double **i_TRG,int *i_set,int i_pos,int i_min,int i_max,Panel *i_parent):instrument(0,0,i_parent)
{name[0]=0;
SRC=i_SRC;TRG=i_TRG;SET=i_set;pos=i_pos;max=i_max;min=i_min;
}
RotEl::~RotEl()
{//if (TRG)
 //for (int i=max-min;i>min;i--)
//  delete *TRG;
};
void RotEl::RegisterMe(int index)
{idx=index;
oapiRegisterPanelArea(index,_R(0,0,1,1),PANEL_REDRAW_ALWAYS,PANEL_MOUSE_IGNORE,PANEL_MAP_NONE);
};
void RotEl::RefreshMe()
{
if (SET)
	if (((*SET)!=pos)&&(*SET>=min)&&(*SET<=max))	//changed,need to change src
	{
	  pos=*SET;
	  *SRC=TRG[pos-min];
		}

}

Plot::Plot(int x, int y, double *i_x,double *i_y,Panel *i_parent):instrument(x,y,i_parent)
{
armed=0;
drawn=0;
strip_offset=0;
x_axis=i_x;
y_axis=i_y;
pointer_y=0;
pointer_x=0;
};


void Plot::RegisterMe(int index)
{idx=index;
oapiRegisterPanelArea(index,_R(ScrX,ScrY,ScrX+size_x,ScrY+size_y),PANEL_REDRAW_ALWAYS,PANEL_MOUSE_IGNORE,PANEL_MAP_CURRENT);
};

void Plot::PaintMe()
{
int offs=120;
if ((armed)&(parent->panel_logic)) offs=0;
if (armed)
{
//HDC hDC=oapiGetDC(parent->GDI_res->hReentrySRF);
double x,y;
//get the position of the marker:

sprintf(oapiDebugString(),"%0.4f %0.4f",*x_axis,*y_axis);
x=(x_scale_min-(*x_axis))/x_resolution+x_min; //delta x in pixels
if (x<0) x=0;
y=((*y_axis)-y_scale_min)/(y_scale_max-y_scale_min); //[0..1] value
y=y_min+y*(y_max-y_min);	//delta y in pixles

if (x-strip_offset>x_max)
		strip_offset=(int)(x-x_max);

pointer_y_old=pointer_y;
pointer_x_old=pointer_x;
if (y>pointer_y) pointer_y+=0.2;
if (y<pointer_y) pointer_y-=0.2;
pointer_x=(int)x;
if (trace) //if we trace, then we blit directly to the surf
oapiBlt(parent->GDI_res->h_Surface[SRF_index],
		parent->GDI_res->h_Surface[SRF_bug_index],pointer_x,(int)pointer_y,0,0,bug_x,bug_y,0xFFFFFF);


}
oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_index],0,0,strip_offset,0,size_x,size_y);
oapiBlt(parent->surf,parent->GDI_res->h_Surface[SRF_mask_index],0,0,0,0,size_x,size_y,0x00);
}
void Plot::RefreshMe()
{
if (armed){
	PaintMe();
	drawn=0;
	return;};

if (!drawn)
{	PaintMe();
	drawn=1;
}

}
void *Plot::GetComponent(char *component_name)
{
if (!strnicmp (component_name, "ARM",3 )) return &armed;
if (!strnicmp (component_name, "XAXISP",6 )) return &x_axis;
if (!strnicmp (component_name, "XAXIS",5 )) return x_axis;
if (!strnicmp (component_name, "YAXISP",6 )) return &y_axis;
if (!strnicmp (component_name, "YAXIS",5 )) return y_axis;
return NULL;
}

inst_MFD::inst_MFD(int x, int y,int i_w,int i_h,int i_type, Panel *i_parent):instrument(x,y,i_parent)
{name[0]=0;
mfd_type=i_type; //MFD - user specified
width=i_w;
height=i_h;
}
void inst_MFD::SetButtons(int b_width, int b_height,int left,int right,int yoff,int ydist)
{mfdspecs.pos.left=ScrX+b_width;mfdspecs.pos.top=ScrY+b_height;
 mfdspecs.pos.right=ScrX+width+b_width;mfdspecs.pos.bottom=ScrY+height+b_height;
 mfdspecs.nbt_left=left;mfdspecs.nbt_right=right;
 mfdspecs.bt_yofs=yoff; mfdspecs.bt_ydist=ydist;
 button_w=b_width;
 button_h=b_height;
}
void inst_MFD::SetColor(int r,int g,int b)
{red=r,green=g;blue=b;}
void inst_MFD::RegisterMe(int index)
{idx=index;
oapiRegisterPanelArea(index,_R(ScrX,ScrY,ScrX+width+button_w*2,ScrY+height+2*button_h),PANEL_REDRAW_USER,PANEL_MOUSE_LBDOWN,PANEL_MAP_BACKGROUND);
oapiRegisterMFD(mfd_type,mfdspecs);
};
void inst_MFD::PaintMe()
{

HDC hDC=oapiGetDC(parent->surf);

SelectObject(hDC, parent->GDI_res->hFNT_Panel[FontIndex]);
SetTextColor (hDC, RGB(red,green,blue));
SetTextAlign (hDC, TA_CENTER);
SetBkMode (hDC, TRANSPARENT);

const char *label;
	for (int bt = 0; bt < mfdspecs.nbt_left+mfdspecs.nbt_right; bt++) {
		if (label = oapiMFDButtonLabel (mfd_type, bt))
			{if (bt<mfdspecs.nbt_left)
					TextOut (hDC, (int)(button_w/2.0), mfdspecs.bt_yofs+
					mfdspecs.bt_ydist*bt, label, strlen(label));
			else TextOut (hDC, (int)(width+button_w*1.5), mfdspecs.bt_yofs+
					mfdspecs.bt_ydist*(bt-6), label, strlen(label));}
	}
oapiReleaseDC(parent->surf,hDC);

};


void inst_MFD::LBD(int x, int y)
{ y-=mfdspecs.bt_yofs;
 int bt;

 bt=(int)(y/mfdspecs.bt_ydist);//which button
if ((bt>=0)&&(bt<=6)) {
	if (x<button_w)
	{					oapiProcessMFDButton(mfd_type,bt,PANEL_MOUSE_LBDOWN); return;}
	bt+=6;
	if (x>width+button_w)
	{					oapiProcessMFDButton(mfd_type,bt,PANEL_MOUSE_LBDOWN);return;}
}
int center=button_w+width/2;
if (y>height+button_h-mfdspecs.bt_yofs)
{	if (x<center-width/3) {oapiToggleMFD_on(mfd_type);return;}
	else if (x>center+width/3) {oapiSendMFDKey(mfd_type,OAPI_KEY_GRAVE);return;}
    else   {oapiSendMFDKey(mfd_type,OAPI_KEY_F1);return;};

	}

}
