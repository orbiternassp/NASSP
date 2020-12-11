/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2004-2020

Vessel Specific Input Manager

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

**************************** Revision History ****************************/

#define SECURE

#include <string>
#include <sstream>
#include "Orbitersdk.h"
#include "dinput.h"
#include "vesim.h"

#define VESIM_DEVICETYPE_KEYBOARD 1
#define VESIM_DEVICETYPE_JOYSTICK 2

#define VESIM_SUBDEVTYPE_AXIS   1
#define VESIM_SUBDEVTYPE_BUTTON 2
#define VESIM_SUBDEVTYPE_KEY    3


#define VESIM_DEVICE_AXIS_X       1
#define VESIM_DEVICE_AXIS_Y       2
#define VESIM_DEVICE_AXIS_Z       3
#define VESIM_DEVICE_AXIS_RX      4
#define VESIM_DEVICE_AXIS_RY      5
#define VESIM_DEVICE_AXIS_RZ      6
#define VESIM_DEVICE_AXIS_SLDR0   7
#define VESIM_DEVICE_AXIS_SLDR1   8

#define VESIM_MODIFIER_SHIFT   1
#define VESIM_MODIFIER_CONTROL 2
#define VESIM_MODIFIER_ALT     4
#define VESIM_MODIFIER_META    8

#define VESIM_CFG_EXT ".cfg"
#define VESIM_CFG_USER_EXT ".launchpad.cfg"
#define VESIM_CFG_GENERIC "GenericJoystick"

char *subdevnames[] = {
	"X", "Y", "Z", "RX", "RY", "RZ", "Slider 0", "Slider 1"
};

bool icomp(const char *s1, const char *s2) {
	//printf("icomp(\"%s\", \"%s\"\n", s1, s2);
	while (*s1 == ' ' && *s1 != 0) s1++;
	while (*s2 == ' ' && *s2 != 0) s2++;
	//printf("After trim c1: %c %hhx c2: %c %hhx\n", *s1, *s1, *s2, *s2);

	while (*s1 != 0 && *s2 != 0) {
		char c1 = *s1;
		char c2 = *s2;
		if (c1 >= 'A' && c1 <= 'Z') {
			c1 = c1 - 'A' + 'a';
		}
		if (c2 >= 'A' && c2 <= 'Z') {
			c2 = c2 - 'A' + 'a';
		}
		if (c1 != c2) return false;
		s1++;
		s2++;
		//printf("c1: %c %hhx c2: %c %hhx\n", c1, c1, c2, c2);
	}

	while (*s1 == ' ' && *s1 != 0) s1++;
	while (*s2 == ' ' && *s2 != 0) s2++;
	//printf("Trail trim c1: %c %hhx c2: %c %hhx\n", *s1, *s1, *s2, *s2);
	if (*s1 != *s2) return false;
	return true;
}

bool VesimInput::addConnection(int deviceID, int subdeviceType, int subdeviceID, int modifiers, bool reverse){
	if (nconns >= VESIM_MAX_INPUT_CONNS) return false;
	conn[nconns].deviceID = deviceID;
	conn[nconns].subdeviceID = subdeviceID;
	conn[nconns].subdeviceType = subdeviceType;
	conn[nconns].modifiers = modifiers;
	conn[nconns].reverse = reverse;
	nconns++;
	return true;
}

VesimDevice::VesimDevice(Vesim *parent, const char* deviceName) : parent(parent), name(deviceName) {
	type = VESIM_DEVICETYPE_KEYBOARD; 
};

VesimDevice::VesimDevice(Vesim *parent, const char* deviceName, LPDIRECTINPUTDEVICE8 dx8_joystick) : 
	parent(parent),  name(deviceName), dx8_joystick(dx8_joystick) {
	type = VESIM_DEVICETYPE_JOYSTICK;
	dx8_joystick->SetDataFormat(&c_dfDIJoystick2);
	//dx8_joystick->EnumObjects(VesimEnumAxesCB, this, DIDFT_AXIS | DIDFT_POV | DIDFT_BUTTON);
};

void VesimDevice::poolDevice() {
	if (type = VESIM_DEVICETYPE_JOYSTICK) {
		HRESULT hr = dx8_joystick->Poll();
		if (FAILED(hr)) { // Did that work?
						  // Attempt to acquire the device
			hr = dx8_joystick->Acquire();
			if (FAILED(hr)) {
				sprintf(oapiDebugString(), "DX8JS: Cannot aquire THC");
#ifdef _DEBUG
				fprintf(parent->out_file, "DX8JS: Cannot aquire %s\n", name.c_str());
#endif
			}
			else {
				hr = dx8_joystick->Poll();
			}
		}
		// Read data
		dx8_joystick->GetDeviceState(sizeof(dx8_jstate), &dx8_jstate);
	}
}

VesimDevice::~VesimDevice() {
	if (type = VESIM_DEVICETYPE_JOYSTICK) {
		dx8_joystick->Unacquire();
		dx8_joystick->Release();
	}
}
Vesim::Vesim(CbInputChanged cbInputChanged, void *pCbData) : cbInputChanged(cbInputChanged), pCbData(pCbData) {
	for (int i = 0; i < VESIM_MAX_INPUTS; i++)
		inpid2idx[i] = -1;
#ifdef _DEBUG
	out_file = fopen("vesim.log", "wt");
	fprintf(out_file, "Vesim object created\n", vesselStationName);
#endif
}

Vesim::~Vesim() {
#ifdef _DEBUG
	fclose(out_file);
#endif
}

BOOL CALLBACK VesimEnumJoysticksCB(const DIDEVICEINSTANCE* pdidInstance, VOID* pVesim)
{
	class Vesim* ves = (Vesim*)pVesim; // Pointer to us
	HRESULT hr;
	LPDIRECTINPUTDEVICE8 dx8_joystick;

#ifdef _DEBUG
	fprintf(ves->out_file, "Found stick instance: %s Prod name: %s\n", &pdidInstance->tszInstanceName[0], &pdidInstance->tszProductName[0]);
#endif
	// Obtain an interface to the enumerated joystick.
	hr = ves->dx8ppv->CreateDevice(pdidInstance->guidInstance, &dx8_joystick, NULL);

	if (FAILED(hr)) {              // Did that work?
		return DIENUM_CONTINUE;
	} // No, keep enumerating (if there's more)

	ves->vdev.emplace_back(ves, &pdidInstance->tszProductName[0], dx8_joystick);
	return DIENUM_CONTINUE; // and keep enumerating
}

std::string getDeviceInputFileName(char* vesselStationName, std::string deviceName, bool isUserCfg) {
	std::string ret(vesselStationName);
	ret.append(" - ");
	ret.append(deviceName);
	ret.append(isUserCfg ? VESIM_CFG_USER_EXT : VESIM_CFG_EXT);
	return ret;
}

bool Vesim::setupDevices(char* vesselStationName, LPDIRECTINPUT8 dx8ppv){
	this->vesselStationName = vesselStationName;
	this->dx8ppv = dx8ppv;
	dx8ppv->EnumDevices(DI8DEVCLASS_GAMECTRL, VesimEnumJoysticksCB, this, DIEDFL_ATTACHEDONLY);
	int ndev = vdev.size();
	for (int devid = 0; devid < ndev; devid++) {
		std::string devicename = vdev[devid].name;
		std::string configdir = "Config\\ProjectApollo\\Vesim\\";

		// Fallback config files
		// 
		// User prefered settings should use extension ".launchpad.cfg". Files with plain "cfg" extension
		// (without ".launchpad." are not to be changed by users but these could be used as templates 
		// for user settings. cfgnames[1] and cfgnames[2] are temporary fallbacks till the user-friendly
		// configurator will not be ready. The rationale is to avoid difficulties with joystick names when
		// manually creating user config files. Now it is enough to duplicate a vessel-specific generic
		// joystick configuration file with extension ".launchpad.cfg" and edit it.

		std::string cfgfnames[5];  
		cfgfnames[0] = configdir + getDeviceInputFileName(vesselStationName, devicename, true);
		cfgfnames[1] = configdir + getDeviceInputFileName(vesselStationName, VESIM_CFG_GENERIC, true);
		cfgfnames[2] = configdir + VESIM_CFG_GENERIC+VESIM_CFG_USER_EXT;
		cfgfnames[3] = configdir + getDeviceInputFileName(vesselStationName, devicename, false);
		cfgfnames[4] = configdir + getDeviceInputFileName(vesselStationName, VESIM_CFG_GENERIC, false);

		std::ifstream fdevcfg;
		for (int k = 0; k < 5; k++) {
			fdevcfg = std::ifstream(cfgfnames[k]);
#ifdef _DEBUG
			fprintf(out_file, "Attempting to open file %s\n", cfgfnames[k].c_str());
#endif
			if (fdevcfg.is_open()) break;
		}
		if (!fdevcfg.is_open()) return false;
#ifdef _DEBUG
		fflush(out_file);
#endif
		std::string line;
		int lineidx = 0;
		int colidx[] = { -1, -1, -1, -1, -1, -1, -1 };
		while (std::getline(fdevcfg, line))
		{
#ifdef _DEBUG
			fprintf(out_file, "Cfg Line:%s \n", line.c_str());
#endif
			int posidx = 0;
			std::istringstream ts (line);
			std::string token;
			std::string inpname;
			int inptype = -1;
			int subdevtype = -1;
			int subdevid = -1;
			bool isreverse = false;
			while (posidx<7 && std::getline(ts, token, '\t')) {
				if (lineidx == 0) {
					if (token == "INPUT")
						colidx[posidx] = 0;
					else if (token == "INPUTTYPE")
						colidx[posidx] = 1;
					else if (token == "SUBDEVID")
						colidx[posidx] = 2;
					else if (token == "REVERSE")
						colidx[posidx] = 3;
					else if (token == "MODIFIER")
						colidx[posidx] = 4;
				}
				else {
					const char *ptok = token.c_str();
					switch (colidx[posidx]) {
						case 0:
							inpname = token;
							break;
						case 1:
							if (icomp(ptok, "Axis")) inptype = VESIM_INPUTTYPE_AXIS;
							else if (icomp(ptok, "Button")) inptype = VESIM_INPUTTYPE_BUTTON;
							break;
						case 2:	
							if (token.length() > 5 && icomp(token.substr(0, 4).c_str(), "Axis")) {
								subdevtype = VESIM_SUBDEVTYPE_AXIS;
								std::string ssid = token.substr(5);
								const char * psid = ssid.c_str();
								for (int k = 0; k < 8; k++) {
									if (icomp(psid, subdevnames[k])) {
										subdevid = k + 1;
										break;
									}
								}
							}
							else if (token.length() > 7 && icomp(token.substr(0, 6).c_str(), "Button")) {
								subdevtype = VESIM_SUBDEVTYPE_BUTTON;
								try
								{
									subdevid = std::stoi(token.substr(6));
								}
								catch(int e) {
									(void)e;
								}
							}
							break;
						case 3:
							if (icomp(ptok, "True")) isreverse=true;						
							break;
						default:
							break;
					}
				}
				posidx++;
			}
#ifdef _DEBUG
			fprintf(out_file, "Connection  for input \"%s\" is defined as device %d (%s) subdev type:%d subdevid:%d reverse:%d\n", inpname.c_str(), devid, vdev[devid].name.c_str(), subdevtype, subdevid, isreverse);
#endif
			if (inpname.length() > 0 && subdevtype > 0 && subdevid >= 0) {
				for (size_t k = 0; k < vinp.size(); k++) {

					if (vinp[k].name == inpname) {
#ifdef _DEBUG
						fprintf(out_file, "Input %d (%s) connected to device %d (%s) subdev type:%d subdevid:%d reverse:%d\n", k, inpname.c_str(), devid, vdev[devid].name.c_str(), subdevtype, subdevid, isreverse);
#endif
						connectDeviceToInput(k, devid, subdevtype, subdevid, isreverse, 0);
						break;
					}
				}
			}
			lineidx++;
		}
		fdevcfg.close();
	}
#ifdef _DEBUG
	fflush(out_file);
#endif
	return true;
}

bool Vesim::addInput(int inputID, char *inputName, int inputType, int defaultValue, bool notifyOnChange) {
	int currsize = vinp.size();
	if(currsize >= VESIM_MAX_INPUTS || inputID > VESIM_MAX_INPUTS || inputID < 0) return false;
	vinp.emplace_back(inputID, inputName, inputType, defaultValue, notifyOnChange);
	inpid2idx[inputID] = currsize;
#ifdef _DEBUG
	fprintf(out_file, "Input %d(%s) added with inputType %d, default value %d, notifyOnChange:%d\n", inputID, inputName, inputType, defaultValue, notifyOnChange);
	fflush(out_file);
#endif
	return true;
}

bool Vesim::addInput(VesimInputDefinition *vid) {
	return addInput(vid->id, vid->name, vid->type, vid->defaultValue, vid->notifyOnChange);
}

bool Vesim::connectDeviceToInput(int inputidx, int deviceID, int subdeviceType, int subdeviceID, bool reverse, int modifiers) {
	return vinp[inputidx].addConnection(deviceID, subdeviceType, subdeviceID, modifiers, reverse);
}

void  Vesim::poolDevices() {
	int ndev = vdev.size();
	for (int devid = 0; devid < ndev; devid++)
		vdev[devid].poolDevice();

	int ninp = vinp.size();
	for (int inpidx = 0; inpidx < ninp; inpidx++) {
		VesimInput *inp = &vinp[inpidx];
		int newValue=0;
		bool isSet = false;
		for (int i = 0; i < inp->nconns; i++) {
			VesimDeviceInputConn *pconn = &inp->conn[i];
			VesimDevice *pdev = &vdev[pconn->deviceID];
			if (pconn->subdeviceType == VESIM_SUBDEVTYPE_AXIS) {
				switch (pconn->subdeviceID) {
				case VESIM_DEVICE_AXIS_X:
					newValue = pdev->dx8_jstate.lX;
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_Y:
					newValue = pdev->dx8_jstate.lY;
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_Z:
					newValue = pdev->dx8_jstate.lZ;
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_RX:
					newValue = pdev->dx8_jstate.lRx;
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_RY:
					newValue = pdev->dx8_jstate.lRy;
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_RZ:
					newValue = pdev->dx8_jstate.lRz;
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_SLDR0:
					newValue = pdev->dx8_jstate.rglSlider[0];
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_SLDR1:
					newValue = pdev->dx8_jstate.rglSlider[1];
					isSet = true;
					break;
				default:
					break;
				}

				if (pconn->reverse)  newValue = 65535 - newValue;

				if (inp->type == VESIM_INPUTTYPE_BUTTON) {
					newValue = newValue > 60000 ? 1:0;
				}
			}
			else if (pconn->subdeviceType == VESIM_SUBDEVTYPE_BUTTON) {
				int sdid = pconn->subdeviceID;
				if (sdid >= 0 && sdid < 128 && pdev->dx8_jstate.rgbButtons[sdid] & 0x80) {
					isSet = true;
					if (inp->type == VESIM_INPUTTYPE_BUTTON) {
						newValue = pconn->reverse ? 0 : 1;
					}
					else newValue = pconn->reverse?  0 : 65535;
				}
				else if(inp->type == VESIM_INPUTTYPE_BUTTON){
					isSet = true;
					newValue = pconn->reverse ? 1 : 0;
				}					
			}
		}
		
		if (!isSet) newValue = inp->defaultValue;
		int oldValue = inp->value;
		inp->value=newValue;
		if (inp->notifyOnChange && (oldValue != inp->value) && cbInputChanged != NULL) {
			int eventType;
			if (inp->type == VESIM_INPUTTYPE_AXIS)
				eventType = VESIM_EVTTYPE_AXIS_CHANGED;
			else if (newValue)
				eventType = VESIM_EVTTYPE_BUTTON_ON;
			else
				eventType = VESIM_EVTTYPE_BUTTON_OFF;
			cbInputChanged(inp->ID, eventType, newValue, pCbData);
		}

		
	}
}

int Vesim::getInputValue(int inputID) {
	int inpsize = vinp.size();
	if (inputID < 0 || inputID >= VESIM_MAX_INPUTS) return 0;
	int iidx = inpid2idx[inputID];
	if (iidx < 0) return 0;
	return vinp[iidx].value;
}
