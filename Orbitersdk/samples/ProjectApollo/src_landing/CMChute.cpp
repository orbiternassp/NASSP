/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2009

  CM Chute vessel

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
//############################################################################//
#define ORBITER_MODULE
// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>,<fstream> )
#include "orbitersdk.h"
//############################################################################//
#include "stdio.h"
#include "math.h"
#include "tracer.h"
#include "CMChute.h"
#include "papi.h"
char trace_file[]="ProjectApollo CMChute.log";

const double timprc[3]={1,6,5};
// States
#define STATE_CHUTE0   0
#define STATE_CHUTE1   1
#define STATE_CHUTE2   2
#define STATE_DEPLOYED 3
#define STATE_LANDED   4
// Modes
#define CH_WRONG  0
#define CH_MAIN   1
#define CH_DROGUE 2
//############################################################################//
DLLCLBK void InitModule(HINSTANCE hModule)               {}
DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel,int flightmodel){return new CMChute(hvessel,flightmodel);}
DLLCLBK void    ovcExit(VESSEL *vessel)                  {if(vessel)delete(CMChute*)vessel;}
//############################################################################//
CMChute::CMChute(OBJHANDLE hObj,int fmodel):VESSEL2(hObj,fmodel)
{
 mode=CH_WRONG;    
 char *classname;
 classname=GetClassName();

 if(!strnicmp(classname,"ProjectApollo/MainChute",14+9))mode=CH_MAIN;
 if(!strnicmp(classname,"ProjectApollo/DrogueChute",14+11))mode=CH_DROGUE;
 
 state=STATE_CHUTE0;
 for(int i=0;i<4;i++){anim[i]=0;proc[i]=1;}
 animLanding=0;
 procLanding=0;
}
CMChute::~CMChute(){}
//############################################################################//
void CMChute::clbkSetClassCaps(FILEHANDLE cfg)
{       
 if(mode==CH_WRONG)return;
 //<Why?>           
 ClearAirfoilDefinitions();
 ClearMeshes();
 ClearExhaustRefs();
 ClearAttExhaustRefs();
 //</Why?>
 
 SetEmptyMass(10);
 SetSize(15);
 SetFuelMass(0);
 SetPMI(_V(20,20,12));
 SetCW(1.0,1.5,1.4,1.4);
 SetRotDrag(_V(0.7,0.7,1.2));
 SetSurfaceFrictionCoeff(1,1);
 if(GetFlightModel()>=1){
  SetPitchMomentScale(-5e-3);
  SetYawMomentScale(-5e-3);
 }
 SetLiftCoeffFunc(0);
 
 if(mode==CH_MAIN){ 
  SetTouchdownPoints(_V(0,10,-2.5),_V(10,-10,-2.5),_V(-10,-10,-2.5));   
  SetCrossSections(_V(2.8,2.8,15.0));
 }else SetCrossSections(_V(2.8,2.8,10.0)); 

 //Drogue   
 if(mode==CH_DROGUE){
  VECTOR3 meshoffset=_V(0,0,11.2);
  meshindex[3]=AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Apollo_2chute"),&meshoffset);
  SetMeshVisibilityMode(meshindex[3],MESHVIS_ALWAYS);
  static MGROUP_SCALE chute(meshindex[3],NULL,0,_V(0,0,-10.912),_V(0.001,0.001,0.001)); 
  anim[3]=CreateAnimation(0.0);
  AddAnimationComponent(anim[3],0,1,&chute);
 }
 
 //Main 
 if(mode==CH_MAIN){
  VECTOR3 meshoffset1=_V(0,-0.25,3.9);
  VECTOR3 meshoffset2=_V(0,-1.38,23.3); 
  VECTOR3 meshoffset3=_V(0,-1.9,15.95);
  meshindex[0]=AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Apollo_3chuteEX"),&meshoffset1);
  meshindex[1]=AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Apollo_3chuteHD"),&meshoffset2);  
  meshindex[2]=AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Apollo_3chute")  ,&meshoffset3);
  SetMeshVisibilityMode(meshindex[0],MESHVIS_NEVER);
  SetMeshVisibilityMode(meshindex[1],MESHVIS_NEVER); 
  SetMeshVisibilityMode(meshindex[2],MESHVIS_NEVER);

  static MGROUP_SCALE chute1(meshindex[0],NULL,0,_V(0,0.25 ,-4.387 ),_V(0.2  ,0.2  ,0.001)); 
  static MGROUP_SCALE chute2(meshindex[1],NULL,0,_V(0,1.38 ,-23.026),_V(0.185,0.185,0.185)); 
  static MGROUP_SCALE chute3(meshindex[2],NULL,0,_V(0,1.908,-15.673),_V(0.65 ,0.65 ,1.2)); 
  anim[0]=CreateAnimation(0.0);   
  anim[1]=CreateAnimation(0.0);  
  anim[2]=CreateAnimation(0.0);
  AddAnimationComponent(anim[0],0,1,&chute1);
  AddAnimationComponent(anim[1],0,1,&chute2);
  AddAnimationComponent(anim[2],0,1,&chute3);

  static UINT landing_groups1[3]={4,5,6};
  static UINT landing_groups2[3]={1,2,3};
  static UINT landing_groups3[3]={7,8,9};
  static UINT landing_groups4[10]={0,1,2,3,4,5,6,7,8,9};
  static MGROUP_ROTATE landing1(meshindex[2],landing_groups1,3,_V(0,1.9,-12.5),_V( 1,0,0),(float)(0.38 * PI));
  static MGROUP_ROTATE landing2(meshindex[2],landing_groups2,3,_V(0,1.9,-12.5),_V( 0.5,0.8660254,0),(float)(-0.38 * PI));
  static MGROUP_ROTATE landing3(meshindex[2],landing_groups3,3,_V(0,1.9,-12.5),_V( -0.5,0.8660254,0),(float)(0.38 * PI));
  static MGROUP_SCALE  landing4(meshindex[2],landing_groups4,10,_V(0,1.9,-15.7), _V(1.0,1.0,0.1)); 
  static MGROUP_ROTATE landing5(meshindex[2],landing_groups1,3,_V(0,1.9,-15.5),_V( 1,0,0),(float)(0.03 * PI));
  static MGROUP_ROTATE landing6(meshindex[2],landing_groups2,3,_V(0,1.9,-15.5),_V( 0.5,0.8660254,0),(float)(-0.03 * PI));
  static MGROUP_ROTATE landing7(meshindex[2],landing_groups3,3,_V(0,1.9,-15.5),_V( -0.5,0.8660254,0),(float)(0.03 * PI));
  animLanding=CreateAnimation(0.0);
  AddAnimationComponent(animLanding,0,   0.45,&landing1);
  AddAnimationComponent(animLanding,0,   0.45,&landing2);
  AddAnimationComponent(animLanding,0,   0.45,&landing3);
  AddAnimationComponent(animLanding,0.45,0.9, &landing4);
  AddAnimationComponent(animLanding,0.9, 1,   &landing5);
  AddAnimationComponent(animLanding,0.9, 1,   &landing6);
  AddAnimationComponent(animLanding,0.9, 1,   &landing7);
 }
}
//############################################################################//
void CMChute::clbkPostCreation()
{            
 if(mode==CH_WRONG)return;
 for(int i=0;i<4;i++)SetAnimation(anim[i],proc[i]);
 SetAnimation(animLanding,procLanding);
}
//############################################################################//
void CMChute::clbkPreStep(double simt,double simdt,double mjd)
{      
 if(mode==CH_WRONG)return;
 if(mode==CH_MAIN){
  if(state<STATE_DEPLOYED){ 
   for(int i=0;i<3;i++)if(i==state)SetMeshVisibilityMode(meshindex[i],MESHVIS_ALWAYS);
                              else SetMeshVisibilityMode(meshindex[i],MESHVIS_NEVER);
   if(proc[state]>0){
    proc[state]=max(proc[state]-simdt/timprc[state],0);
    SetAnimation(anim[state],proc[state]);
   }else state++;
  }else if(state==STATE_DEPLOYED){
   if(GetAltitude()<2.3){
    if(procLanding<1){
     procLanding=min(procLanding+simdt/20,1);
     SetAnimation(animLanding,procLanding);
    }else state=STATE_LANDED;
   }
   SetMeshVisibilityMode(meshindex[0],MESHVIS_NEVER);
   SetMeshVisibilityMode(meshindex[1],MESHVIS_NEVER);
   SetMeshVisibilityMode(meshindex[2],MESHVIS_ALWAYS);
  }
 }else{
  if(proc[3]>0){
   proc[3]=max(proc[3]-simdt/3.0,0);
   SetAnimation(anim[3],proc[3]);
  }
 }
}
//############################################################################//
void CMChute::clbkLoadStateEx(FILEHANDLE scn,void *status)
{       
 if(mode==CH_WRONG)return;
 char *line;

 while(oapiReadScenario_nextline(scn,line)){
       if(!strnicmp(line,"STATE",5))       {sscanf(line+5 ,"%d" ,&state);}
  else if(!strnicmp(line,"PROC1",5))       {sscanf(line+5 ,"%lf",&proc[0]);}
  else if(!strnicmp(line,"PROC2",5))       {sscanf(line+5 ,"%lf",&proc[1]);}
  else if(!strnicmp(line,"PROC3",5))       {sscanf(line+5 ,"%lf",&proc[2]);}
  else if(!strnicmp(line,"PROC4",5))       {sscanf(line+5 ,"%lf",&proc[3]);}
  else if(!strnicmp(line,"PROCLANDING",11)){sscanf(line+11,"%lf",&procLanding);}   
  else ParseScenarioLineEx(line,status);
 }
}
//############################################################################//
void CMChute::clbkSaveState(FILEHANDLE scn)
{       
 if(mode==CH_WRONG)return;
 VESSEL2::clbkSaveState(scn);

 oapiWriteScenario_int   (scn,"STATE",state);
 papiWriteScenario_double(scn,"PROC1",proc[0]);
 papiWriteScenario_double(scn,"PROC2",proc[1]);
 papiWriteScenario_double(scn,"PROC3",proc[2]);
 papiWriteScenario_double(scn,"PROC4",proc[3]);
 papiWriteScenario_double(scn,"PROCLANDING",procLanding);
} 
//############################################################################//

