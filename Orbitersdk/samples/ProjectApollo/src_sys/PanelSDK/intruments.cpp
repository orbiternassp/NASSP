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
