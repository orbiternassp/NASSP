//############################################################################//
//Lunosid collision detection interface calls
//Made by Artlav in 2005-2009
//############################################################################//
//Functions
void InitCollisionSDK();
//############################################################################//
void VSDisableCollisions(OBJHANDLE VesselHandle);
void VSEnableCollisions(OBJHANDLE VesselHandle);
void VSSetTouchdownPoints(OBJHANDLE VesselHandle,VECTOR3 pt1,VECTOR3 pt2,VECTOR3 pt3);
double VSGetATL(OBJHANDLE VesselHandle);
double VSGetElvLoc(OBJHANDLE VesselHandle,double lat,double lon,double alt);
double VSGetAbsElvLoc(char *PlanetName,double lat,double lon,double alt);
double VSGetAbsMaxElvLoc(char *PlanetName,double lat,double lon);
//############################################################################//

