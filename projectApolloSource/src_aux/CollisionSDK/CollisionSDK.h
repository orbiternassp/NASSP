//############################################################################//
//Collision system interface calls
//Made by Artlav in 2005-2009
//############################################################################//


#if !defined(_COLLISIONSDK_H)
#define _COLLISIONSDK_H

//Functions
void InitCollisionSDK();
//############################################################################//
void VSDisableCollisions(OBJHANDLE VesselHandle);
void VSEnableCollisions(OBJHANDLE VesselHandle,char *config_dir);
void VSSetTouchdownPoints(OBJHANDLE VesselHandle,VECTOR3 pt1,VECTOR3 pt2,VECTOR3 pt3);
double VSGetATL(OBJHANDLE VesselHandle);
double VSGetElvLoc(OBJHANDLE VesselHandle,double lat,double lon,double alt);
double VSGetAbsElvLoc(char *PlanetName,double lat,double lon,double alt);
double VSGetAbsMaxElvLoc(char *PlanetName,double lat,double lon);
//############################################################################//

#endif // _COLLISIONSDK_H