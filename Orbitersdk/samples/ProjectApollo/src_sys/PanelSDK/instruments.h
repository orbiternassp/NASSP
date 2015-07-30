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

#ifndef __INSTRUMENTS_H_
#define __INSTRUMENTS_H_

#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"
#include "matrix.h"

class Panel;

class instrument		// basic instrument template
{  public:
	instrument(int x, int y,Panel *i_parent);
	virtual ~instrument(){};
	int type;							//instrument registration number for run-time ID
	virtual void RegisterMe(int index){};//register instrument within Orbiter's api
	virtual void PaintMe(){};		//repaint the instrument
	virtual void RefreshMe(){};	//basic refresh()
	virtual void LBD(int x,int y) {};
	virtual void RBD(int x, int y) {};
	virtual void BU() {};
	virtual void SetLight(){};

	int ScrX;
    int ScrY;			//coords on screen
	int size_x,size_y;
    int idx;			//index on the panel list
	char name[25];

	int SRF_index;		//index to GDI res surface to use
	Panel *parent;		// pointer to parent panel
    virtual void* GetComponent(char *component_name){return NULL;};
};

class instrument_list	//list used by panels to keep instruments
{public:
   instrument *instance;
   instrument_list *next;
   instrument_list(){instance=NULL;next=NULL;};
};


class CText:public instrument		//<CLASS CTEXT>
{ public:

    CText(int x,int y, int lng,int dir,char *tt,Panel* i_parent);
	virtual ~CText(){};
	void SetColor(int red,int green,int blue);
	void SetNoCol(int red,int green, int blue);
	void RegisterMe(int index);
	void PaintMe();
	void SetLight();
	int light;
	int length;
	int direction;
	char text[25];
	int slen;
	int FontIndex;
	int ColPenIndex;
	int NoColPenIndex;
	int color[3];
	int nocol[3];
	int height;
};
class Switch: public instrument		//<CLASS SWITCH>
{  public:
	Switch(int x,int y,int sx,int sy,int i_num_pos,int i_sp, int *i_SRC,Panel* i_parent);
	virtual ~Switch(){};
	void RegisterMe(int index);
    void PaintMe();
	void RBD(int x, int y);
	void LBD(int x, int y);
	void BU();
	void SetLight();
	int pos;			//position of switch
    int num_pos;		//number of positions
	int spring;
	int *SRC;
	virtual void* GetComponent(char *component_name);

};
class CodeSwitch:public Switch		//CODE SWITCH
{ public:
    CodeSwitch(int x, int y, int sx,int sy,int i_num_pos, int i_sp,int i_code_index, int *i_SRC,Panel* i_parent);
	int code_index;
	void RBD(int x, int y);
	void LBD(int x, int y);
};

class Rotational:public instrument	//<CLASS
{ public:
	Rotational(int x, int y, Panel *i_parent);
	void SetNeedleSize(int w, int h);
	void SetCenter(int x,int y);
	void SetSize(int x, int y);
	void SetRange(double i_max_a,double i_max_v, double i_min_a,double i_min_v);

	int board_size;
	int needle_x,needle_y;
//	int needle_w,needle_h;
	double max_a,max_v,min_a,min_v;
	~Rotational();
	void RegisterMe(int index);
    void PaintMe();
	void RefreshMe();
	double last_paint;
	double *G;
	SURFHANDLE local_blt;	//tiny surface for pointer blt;
	POINT TR[3];
	virtual void* GetComponent(char *component_name);
};

class ADI:public instrument
{public:

   void *void_hpbuffer;
   int init;
   int sphere_resolution;		//more is better, but uses more memory..
   float sphere_vert[625][3];
   float sphere_norm[625][3];
   float sphere_tex[625][2];
   unsigned int sphere_index[1300];
   void *p_tex;
   int tri; //number of triangle strip
   int list_name; //we store the rendering into a display list
   double radius;

   float light_pos[3];
   float ambient[3];
   float color[3];
   float zoom;
   double *PITCH;
   double *YAW;
   double *ROLL;
   vector3 reference;
   matrix ref_rot;
   vector3 now;
   vector3 rates;

   float over_rate;
   //some stuff for OpenGL
   HDC		   hDC2;
   HGLRC       hRC;
   HBITMAP	   hBMP;
   HBITMAP hBMP_old;
   ADI(int x,int y,Panel *i_parent);
   virtual ~ADI();

   void SetZoom(double y);
   void SetAmbient(int red,int green,int blue);
   void SetLight(int red,int green,int blue);
   void InitGL(char *bitmapname);
   void MoveBall();
   void RegisterMe(int index);
   void PaintMe();
   void RefreshMe();
   void SetLight();
   virtual void* GetComponent(char *component_name);
   SURFHANDLE local_blt;	//tiny surface for pointer blt; (attitude rates uses: max/med/min..)
   POINT TR[3];
};



class inst_MFD:public instrument
{public:
     int mfd_type;
    MFDSPEC mfdspecs;
	inst_MFD(int x, int y,int i_w,int i_h,int i_type, Panel *i_parent);
	void SetButtons(int b_width, int b_height,int left,int right,int yoff,int ydist);
	void SetColor(int r,int g,int b);
	void RegisterMe(int index);
	void PaintMe();
	void LBD(int x,int y);

	int red,green,blue;
	int width,height;
	int button_w;
	int button_h;
	int FontIndex;
};

class small_clock:public instrument
{
public:
	small_clock(int x, int y, int i_size,double *i_val, Panel *i_parent);
	void RegisterMe(int index);
	void PaintMe();
	void RefreshMe();

	double *val;
	double last_value;
	int size;
	virtual void* GetComponent(char *component_name);
};
class Linear:public instrument
{
public:
	Linear(int x, int y,double i_min,double i_max,double i_s,double i_b,double* i_src, Panel *i_paren);
    ~Linear();
	SURFHANDLE local_blt;
	double *SRC1;
	double scale;
	double bias;
	double max;
	double min;
    double max_x,max_y;
	double min_x,min_y;
	double angle;
	int point_x,point_y;
	int last_x,last_y;
	int needle_w,needle_h;
	void SetNeedleSize(int w, int h);
	void SetMax(double x,double y);
	void SetMin(double x,double y);
	void SetAngle(double ang);
	int last_light;
	int bkg_printed;
	double last1;
	int GetPixel1();
	void DoNeedle();
	void PrintBackground();
	void RegisterMe(int index);
	void PaintMe();
	void RefreshMe();
	void SetLight();
	virtual void* GetComponent(char *component_name);
};


class TB:public instrument
{public:
	TB(int x, int y, double *i_src,int i_type, Panel *i_parent);

	void RegisterMe(int index);
	void PaintMe();
	void RefreshMe();
	int IsOn();
	double *SRC;
	int type;
	int paint_on;
	double blink;
	double timer;
	int blink_on;
	virtual void* GetComponent(char *component_name);
};

class Rot:public instrument
{public:
	Rot(int x, int y,int i_p,int i_max,int i_min,Panel *i_parent);
	int max;
	int min;
	int c_pos;

	void RegisterMe(int index);
	void PaintMe();
	void RefreshMe();
	void RBD(int x, int y);
	void LBD(int x, int y);
	virtual void* GetComponent(char *component_name);
};

class RotEl:public instrument
{public:
    RotEl(double **i_SRC,double **i_TRG,int *i_set,int i_pos,int i_min,int i_max,Panel *i_parent);
	~RotEl();
	double **SRC;
	double **TRG;
	int *SET;
	int pos;
	int min;
	int max;
	void RegisterMe(int index);
	void PaintMe(){};
	void RefreshMe();

};




class Plot:public instrument
{

	int drawn;	//flag to signal that blanc has been drawn for no-arm
	double *x_axis;
	double *y_axis;
public:
	int strip_offset;
    int x_max,y_max;	//on screen
	int x_min,y_min;
	int x_scale_max;	//actual values
	int x_scale_min;
	int y_scale_max;
	int y_scale_min;
	int bug_x,bug_y;
	int trace;
	double x_resolution;	//in units/pixel

	double  pointer_y,pointer_y_old;
	int pointer_x,pointer_x_old;
	int SRF_mask_index;
	int SRF_bug_index;

	Plot(int x, int y, double *i_x,double *i_y,Panel *i_parent);
	void RegisterMe(int index);
	void PaintMe();
	void RefreshMe();

	int armed;	//is it on/off.. etcc
	int light;	//we have a sep.. button for this ..???
	void* GetComponent(char *component_name);

};
//--------------------------------PANEL STUFF STARTS FROM HERE ----------------


class  GDI_resources //global holder of resources
{public:
	//these are always present
    HBRUSH hBRUSH_TotalBlack;
	HBRUSH hBRUSH_TotalWhite;
	HBRUSH hBRUSH_Gray;

	HPEN  hPEN_NULL,hPEN_Cyan,hPEN_Brown;
	HPEN  hPEN_Green;

	SURFHANDLE h_Surface[50]; //up to 50 resources can be defined
	HFONT hFNT_Panel[10];	//we can use up to 10 fonts on texts
    HPEN  hPEN[10];
	bool Panel_Resources_Loaded;
	int num_surfaces;
	int num_fonts;
	int num_brush;
	GDI_resources() {Panel_Resources_Loaded=0;num_surfaces=0;num_fonts=0;num_brush=0;};
	~GDI_resources();

};

class InstrumentDescriptor
{public:
	InstrumentDescriptor *next;
	char name[25];			// name of the instrument
    char class_name[25];	// name of the class it belongs to

	int p_int1,p_int2,p_int3;
	int p_int4,p_int5,p_int6;	//6 integer parameters
    int p_int7,p_int8;
	double p_double1,p_double2,p_double3,p_double4; //4 doubles
	double p_double5;

	int SurfaceIndex[4];		//up to 4 surface indexes
	int FontIndex;
	char param_string[40];
};
class CustomVariable
{	public:
	char name[25];			//the name of the var
	void *item;				//pointer to IT
	CustomVariable *next;	//these are chain-linked so we can have as many as we want
};
class PanelSDK;
class Panel
{
public:
   int Wdth,Hght;					//Width & Height of the panel;
   SURFHANDLE surf;					//surface pointer to the screened instrument
   char name[25];					//the name of the panel

   HINSTANCE hModule;				//handle to program instance // ??need this to load resource bitmaps.. ugh.. windows is idiot!!!
   HDC hDC;

   instrument_list *instruments;	//list of instruments

   int neighbours[4];
   int idx;							//index of panel and neighbours
   int mfd_idx[5];					//5 indexed of the eventual MFDs
   double transparent_color;
   int panel_attach;
   int text_logic;		//used by text allover the panel, etc
   int tl_handle;
   int pl_handle;
   int panel_logic;		//0-lights off, 1-lights on (used by the self-lighting instruments)
						//this requires DC power to work;

   int HUD_mode;		//off, horizon, orbital.
   int MFD_mode;		//-1 primary, 1 back-up
   VESSEL *v;			//pointer to parent vessel
   PanelSDK *parent;
   Panel();
   ~Panel();

   //some init functions
   HBITMAP MakeYourBackground(); //return's one's background image
   char BackgroundName[85];
   void AddInstrument(instrument* new_inst);
   instrument* GetInstrumentByName(char *inst_name);
   void* GetPointerByString(char *query);

   //Orbiter interaction
   void RegisterYourInstruments();
   void Paint(int index);	//all the instruments;
   void Refresh(int index);	//for instruments who need constant refresh;
   void LBD(int index,int x,int y);
   void RBD(int index,int x,int y);
   void BU(int index);
   void Load (FILEHANDLE scn);
   void Save (FILEHANDLE scn);
   void SetNeighbours();

	//GDI management
   GDI_resources *GDI_res;
   void SetGDI(GDI_resources *GDI);
   void GDI_Init_Resources();
   void GDI_Dealocate_Resources();


   void CodeCallFromSwitch(int index);	//some swithces need to run a code when activated
};
#include "PanelSDK.h"
#endif
