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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"

class Mesh
{ public:
  Mesh();
  ~Mesh();
  MESHHANDLE h_mesh;
  VECTOR3 offset[10];
  char name[25];
  char mesh_name[25];
  double dry_weight;
  int min_stage;
  int max_stage;

  void Init();
  void Show(VESSEL *vs,int stage);
  void AddToList(Mesh * new_mesh);
  Mesh *next;
};

class Propellant
{  public:
	Propellant();
	~Propellant();
	PROPELLANT_HANDLE h_prop;
	char name[25];
	double total_weight;
	int min_stage;
	int max_stage;
	void Define(VESSEL * vs, int stage, int load);
	void AddToList(Propellant *new_p);
	PROPELLANT_HANDLE GetPhByName(char *p_name);
	Propellant *next;
};

class Thruster
{ public:
	Thruster();
	~Thruster();
   THRUSTER_HANDLE th;
   char propellant[25];
   VECTOR3 pos;
   VECTOR3 dir;
   double thr;
   double isp1,isp2;
   double width,length;
   int group;

   void Define(VESSEL *vs, Propellant *PH_list);
   Thruster *next;
   void AddToList(Thruster *new_th);
};

class Ejector
{public:
  char class_name[75];
   VECTOR3 offset;
   VECTOR3 rotation;
   Ejector *next;

	Ejector();
	~Ejector();
	void AddToList(Ejector* new_e);
	void Eject(VESSEL *vessel);
};

class DockPort
{public:
  VECTOR3 pos;
  VECTOR3 dir;
  VECTOR3 up;

  DockPort *next;
  DockPort();
  ~DockPort();

  void AddToList(DockPort* new_dp);
  void Define(VESSEL *vs);
};
class Stage
{ public:
  double size;
  VECTOR3 cw;
  VECTOR3 crosss;
  VECTOR3 rotdrag;
  VECTOR3 pmi;
  VECTOR3 camera_off;
  VECTOR3 touchd[3];
  VECTOR3 cgoffset;
  char wingname[25];
  Thruster *TH_list;
  Ejector *EJ_list;
  DockPort *DK_list;
  Stage();
  ~Stage();
  void SetStage(VESSEL *vs);
};

class KeyPress
{public:
   DWORD key;
   int trigger_type;	//1- separation , 2- animation
   int index;			//ie. which separation or which animation
   KeyPress *next;

   KeyPress();
   ~KeyPress();
   void AddToList(KeyPress *new_k);

};

class VesselMgmt
{public:
   Stage Stages[10];	      //up 10 staging options
   Mesh *Meshes;			  // actual meshes list
   Propellant *Propellants;	  //actual props lists
   KeyPress *DefinedKeys;
   int max_stages;
   VESSEL *vs;
   VesselMgmt();
   ~VesselMgmt();
   void Build(); //build it from files..
   void SetConfig( int config, int load);
   void Separation (int config);
   void GroupTHR(Thruster* TH_list);

   void BuildMeshElements();
   void BuildPropellants();
   void BuildStage(int stage);
};
