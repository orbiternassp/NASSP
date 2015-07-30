//############################################################################//
//Collision system interface calls
//Made by Artlav in 2005-2009
//############################################################################//
//To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>,<fstream> )
#include "Orbitersdk.h"
#include "CollisionSDK.h"
//############################################################################//
//Functions
void   (__stdcall *pVSSetTouchdownPoints)(OBJHANDLE VesselHandle,VECTOR3 pt1,VECTOR3 pt2,VECTOR3 pt3);
double (__stdcall *pVSGetATL)(OBJHANDLE VesselHandle);
double (__stdcall *pVSGetElvLoc)(OBJHANDLE VesselHandle,double lat,double lon,double alt);
double (__stdcall *pVSGetAbsElvLoc)(PCHAR PlanetName,double lat,double lon,double alt);
double (__stdcall *pVSGetAbsMaxElvLoc)(PCHAR PlanetName,double lat,double lon);
void   (__stdcall *pVSDisableCollisions)(OBJHANDLE VesselHandle);
void   (__stdcall *pVSEnableCollisions)(OBJHANDLE VesselHandle,char *config_dir);
DWORD  (__stdcall *pVSSetCollisionFlags)(OBJHANDLE VesselHandle,DWORD flags);
//############################################################################//
//Internal
HMODULE tcsdkhlib;
int SetErr=1;
#define colmod "modules\\collision.dll"
#define themod "Modules\\Plugin\\collision-core.dll"
//############################################################################//
void InitCollisionSDK()
{
 if(SetErr==0)return;

 tcsdkhlib=LoadLibrary(colmod);
 if(!tcsdkhlib){
  //Disabled for the moment to avoid annoyances
  //MessageBox(NULL,"Library modules\\lunosid.dll not found, please check your installation!","Error!",MB_OK|MB_ICONERROR); 
 }else{  
  pVSSetTouchdownPoints=(void  (__stdcall*)(OBJHANDLE VesselHandle,VECTOR3 pt1,VECTOR3 pt2,VECTOR3 pt3))GetProcAddress(tcsdkhlib,"VSSetTouchdownPoints");
  pVSGetATL=            (double(__stdcall*)(OBJHANDLE VesselHandle))                                    GetProcAddress(tcsdkhlib,"VSGetATL");
  pVSGetElvLoc=         (double(__stdcall*)(OBJHANDLE VesselHandle,double lat,double lon,double alt))   GetProcAddress(tcsdkhlib,"VSGetElvLoc");
  pVSGetAbsElvLoc=      (double(__stdcall*)(PCHAR PlanetName,double lat,double lon,double alt))         GetProcAddress(tcsdkhlib,"VSGetAbsElvLoc");
  pVSGetAbsMaxElvLoc=   (double(__stdcall*)(PCHAR PlanetName,double lat,double lon))                    GetProcAddress(tcsdkhlib,"VSGetAbsMaxElvLoc");
  pVSDisableCollisions= (void  (__stdcall*)(OBJHANDLE VesselHandle))                                    GetProcAddress(tcsdkhlib,"VSDisableCollisions");
  pVSEnableCollisions=  (void  (__stdcall*)(OBJHANDLE VesselHandle,char *config_dir))                   GetProcAddress(tcsdkhlib,"VSEnableCollisions");
  pVSSetCollisionFlags= (DWORD (__stdcall*)(OBJHANDLE VesselHandle,DWORD flags))                        GetProcAddress(tcsdkhlib,"VSSetCollisionFlags");

  if(pVSSetTouchdownPoints==NULL|| pVSGetATL==NULL||pVSGetElvLoc==NULL||pVSGetAbsElvLoc==NULL||pVSGetAbsMaxElvLoc==NULL||pVSDisableCollisions==NULL||pVSEnableCollisions==NULL||VSSetCollisionFlags==NULL){
   //Disabled for the moment to avoid annoyances
   //MessageBox(NULL,"Library modules\\lunosid.dll corrupt or wrong version, please check your installation!","Error!",MB_OK|MB_ICONERROR); 
  }else if(GetModuleHandle(themod))SetErr=0;
 }
}
//############################################################################//
void VSDisableCollisions (OBJHANDLE VesselHandle)                                    {if(SetErr)return;pVSDisableCollisions (VesselHandle);}
void VSEnableCollisions  (OBJHANDLE VesselHandle,char *config_dir)                   {if(SetErr)return;pVSEnableCollisions  (VesselHandle,config_dir);}
void VSSetTouchdownPoints(OBJHANDLE VesselHandle,VECTOR3 pt1,VECTOR3 pt2,VECTOR3 pt3){if(SetErr)return;pVSSetTouchdownPoints(VesselHandle,pt1,pt2,pt3);}
//############################################################################//
double VSGetATL           (OBJHANDLE VesselHandle)                                 {if(SetErr)return oapiGetVesselInterface(VesselHandle)->GetAltitude();return pVSGetATL(VesselHandle);}
double VSGetElvLoc        (OBJHANDLE VesselHandle,double lat,double lon,double alt){if(SetErr)return 0;return pVSGetElvLoc(VesselHandle,lat,lon,alt);}
DWORD  VSSetCollisionFlags(OBJHANDLE VesselHandle,DWORD flags)                     {if(SetErr)return 0;return pVSSetCollisionFlags(VesselHandle,flags);}
double VSGetAbsElvLoc     (char *PlanetName,double lat,double lon,double alt)      {if(SetErr)return 0;return pVSGetAbsElvLoc     (PlanetName,lat,lon,alt);}
double VSGetAbsMaxElvLoc  (PCHAR PlanetName,double lat,double lon)                 {if(SetErr)return 0;return pVSGetAbsMaxElvLoc  (PlanetName,lat,lon);}
//############################################################################//



