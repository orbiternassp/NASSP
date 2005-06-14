/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Crawler Transporter vessel

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

  **************************** Revision History ****************************
  *	$Log$
  **************************************************************************/

#include "Crawler.h"

HINSTANCE g_hDLL;
char trace_file[] = "ProjectApollo-Crawler-trace.txt";


DLLCLBK void InitModule(HINSTANCE hModule) {
	g_hDLL = hModule;
}


DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel) {

	return new Crawler(hvessel, flightmodel);
}


DLLCLBK void ovcExit(VESSEL *vessel) {

	if (vessel) delete (Crawler*)vessel;
}


Crawler::Crawler(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel) {

	velocity = 0;
	touchdownPointHeight = -0.0001;
	firstTimestepDone = false;

	keyAccelerate = false;
	keyBrake= false;
	keyLeft= false;
	keyRight= false;
	keyUp = false;
	keyDown = false;

	soundlib.InitSoundLib(hObj, SOUND_DIRECTORY);
	soundlib.LoadSound(soundEngine, "CrawlerEngine.wav", BOTHVIEW_FADED_MEDIUM);
}


Crawler::~Crawler() {
}


void Crawler::clbkSetClassCaps(FILEHANDLE cfg) {

	SetEmptyMass(2721);
	SetSize(40);
	SetPMI(_V(133, 189, 89));

	SetRotDrag (_V(0.7, 0.7, 1.2));	//???
	SetPitchMomentScale(0);
	SetBankMomentScale(0);
	SetLiftCoeffFunc(0); 

    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	VECTOR3 mesh_dir=_V(0,0,0);

    int icr = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Crawler"), &mesh_dir);
	SetMeshVisibilityMode(icr, MESHVIS_ALWAYS);
	SetTouchdownPoint();

	SetCameraOffset(_V(16.8, 5.2, 19.0));
}


void Crawler::clbkPreStep(double simt, double simdt, double mjd) {

	const double maxVelocity = 1.0e-6;
	double cap;
	VESSELSTATUS vs;

	if (!firstTimestepDone) DoFirstTimestep(); 

	GetStatus(vs);
	oapiGetHeading(GetHandle(), &cap);
	double lon = vs.vdata[0].x;
	double lat = vs.vdata[0].y;

	if (keyLeft) {
		vs.vdata[0].z = vs.vdata[0].z - 1.7 * simdt * PI / 180.0;
		if(vs.vdata[0].z <= -2.0 * PI) {
			vs.vdata[0].z = vs.vdata[0].z + 2.0 * PI;
		}
		keyLeft = false;
	}
	if (keyRight) {
		vs.vdata[0].z = vs.vdata[0].z + 1.7 * simdt * PI / 180.0;
		if(vs.vdata[0].z >= 2.0 * PI) {
			vs.vdata[0].z = vs.vdata[0].z - 2.0 * PI;
		}
		keyRight = false;
	}
	if (keyAccelerate) {
		velocity += 1.0e-7 * simdt;
		if (velocity > maxVelocity) velocity = maxVelocity;
		keyAccelerate = false;
	}
	if (keyBrake) {
		velocity -= 1.0e-7 * simdt;
		if (velocity < 0) velocity = 0;
		keyBrake = false;
	}

	lat = lat + cos(cap) * velocity * simdt;
	lon = lon + sin(cap) * velocity * simdt;

	vs.vdata[0].x = lon;
	vs.vdata[0].y = lat;
	DefSetState(&vs);

	if (keyUp) {
		touchdownPointHeight -= 0.5 * simdt;
		SetTouchdownPoint();
		keyUp = false;

	} else if (keyDown) {
		touchdownPointHeight += 0.5 * simdt;
		if (touchdownPointHeight > -0.0001) touchdownPointHeight = -0.0001;
		SetTouchdownPoint();
		keyDown = false;
	}

	if (velocity != 0) 
		soundEngine.play(LOOP, (int)(127.5 + 127.5 * velocity / maxVelocity));
	else
		soundEngine.stop();

	//sprintf(oapiDebugString(), "Velo %e Touchdown %f", velocity, touchdownPointHeight);
}


void Crawler::SetTouchdownPoint() {

	SetTouchdownPoints(_V( 0, touchdownPointHeight,  9), 
					   _V(-4, touchdownPointHeight, -9), 
					   _V( 4, touchdownPointHeight, -9));
}


void Crawler::DoFirstTimestep() {

	// Turn off pretty much everything that Orbitersound does by default.
	soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINRANDOMAMBIANCE, FALSE);
	soundlib.SoundOptionOnOff(PLAYRADIOATC, FALSE);

	firstTimestepDone = true;
}

void Crawler::clbkLoadStateEx(FILEHANDLE scn, void *status) {
	
	char *line;
	
	while (oapiReadScenario_nextline (scn, line)) {
		//if (!strnicmp (line, "MYPARAM", 7)) {
		//	sscanf (line+7, "%lf", &myparam);
		//} else {
			ParseScenarioLineEx (line, status);
		//}
	}
}

void Crawler::clbkSaveState(FILEHANDLE scn) {
	
	VESSEL2::clbkSaveState (scn);
	//oapiWriteScenario_float (scn, "MYPARAM", myparam);
}

int Crawler::clbkConsumeDirectKey(char *kstate) {

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0; 
	}

	int r = 0;
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD8)) {
		keyAccelerate = true;				
		r = 1;
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD2)) {
		keyBrake = true;				
		r = 1;
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD4)) {
		keyLeft = true;				
		r = 1;
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD6)) {
		keyRight = true;				
		r = 1;
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD9)) {
		keyUp = true;				
		r = 1;
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD3)) {
		keyDown = true;				
		r = 1;
	}
	return r;
}