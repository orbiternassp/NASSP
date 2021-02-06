/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2020

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

#define VESIM_CFG_EXT ".cfg"
#define VESIM_CFG_USER_EXT ".launchpad.cfg"
#define VESIM_CFG_GENERIC "GenericJoystick"

char *subdevnames[] = {
	"X", "Y", "Z", "RX", "RY", "RZ", "Slider 0", "Slider 1"
};

char *povaxnames[] = {
	"0H", "0V", "1H", "1V", "2H", "2V", "3H", "3V"
};

struct KeyData{
	int oapiID;
	char *keyName;
};

#define KEYDATA_CNT 102
KeyData keydata[KEYDATA_CNT] = {
	{ OAPI_KEY_ESCAPE,         "Escape" },
	{ OAPI_KEY_1,              "1" },
	{ OAPI_KEY_2,              "2" },
	{ OAPI_KEY_3,              "3" },
	{ OAPI_KEY_4,              "4" },
	{ OAPI_KEY_5,              "5" },
	{ OAPI_KEY_6,              "6" },
	{ OAPI_KEY_7,              "7" },
	{ OAPI_KEY_8,              "8" },
	{ OAPI_KEY_9,              "9" },
	{ OAPI_KEY_0,              "0" },
	{ OAPI_KEY_MINUS,          "-" },
	{ OAPI_KEY_EQUALS,         "=" },
	{ OAPI_KEY_BACK,           "Backspace" },
	{ OAPI_KEY_TAB,            "Tab" },
	{ OAPI_KEY_Q,              "Q" },
	{ OAPI_KEY_W,              "W" },
	{ OAPI_KEY_E,              "E" },
	{ OAPI_KEY_R,              "R" },
	{ OAPI_KEY_T,              "T" },
	{ OAPI_KEY_Y,              "Y" },
	{ OAPI_KEY_U,              "U" },
	{ OAPI_KEY_I,              "I" },
	{ OAPI_KEY_O,              "O" },
	{ OAPI_KEY_P,              "P" },
	{ OAPI_KEY_LBRACKET,       "{" },
	{ OAPI_KEY_RBRACKET,       "}" },
	{ OAPI_KEY_RETURN,         "Enter" },
	{ OAPI_KEY_LCONTROL,       "LCtrl" },
	{ OAPI_KEY_A,              "A" },
	{ OAPI_KEY_S,              "S" },
	{ OAPI_KEY_D,              "D" },
	{ OAPI_KEY_F,              "F" },
	{ OAPI_KEY_G,              "G" },
	{ OAPI_KEY_H,              "H" },
	{ OAPI_KEY_J,              "J" },
	{ OAPI_KEY_K,              "K" },
	{ OAPI_KEY_L,              "L" },
	{ OAPI_KEY_SEMICOLON,      ";" },
	{ OAPI_KEY_APOSTROPHE,     "'" },
	{ OAPI_KEY_GRAVE,          "`" },
	{ OAPI_KEY_LSHIFT,         "LShift" },
	{ OAPI_KEY_BACKSLASH,      "\\" },
	{ OAPI_KEY_Z,              "Z" },
	{ OAPI_KEY_X,              "X" },
	{ OAPI_KEY_C,              "C" },
	{ OAPI_KEY_V,              "V" },
	{ OAPI_KEY_B,              "B" },
	{ OAPI_KEY_N,              "N" },
	{ OAPI_KEY_M,              "M" },
	{ OAPI_KEY_COMMA,          "," },
	{ OAPI_KEY_PERIOD,         "." },
	{ OAPI_KEY_SLASH,          "/" },
	{ OAPI_KEY_RSHIFT,         "RShift" },
	{ OAPI_KEY_MULTIPLY,       "Keypad *" },
	{ OAPI_KEY_LALT,           "LAlt" },
	{ OAPI_KEY_SPACE,          "Space" },
	{ OAPI_KEY_CAPITAL,        "Caps Lock" },
	{ OAPI_KEY_F1,             "F1" },
	{ OAPI_KEY_F2,             "F2" },
	{ OAPI_KEY_F3,             "F3" },
	{ OAPI_KEY_F4,             "F4" },
	{ OAPI_KEY_F5,             "F5" },
	{ OAPI_KEY_F6,             "F6" },
	{ OAPI_KEY_F7,             "F7" },
	{ OAPI_KEY_F8,             "F8" },
	{ OAPI_KEY_F9,             "F9" },
	{ OAPI_KEY_F10,            "F10" },
	{ OAPI_KEY_NUMLOCK,        "Num Lock" },
	{ OAPI_KEY_SCROLL,         "Scroll Lock" },
	{ OAPI_KEY_NUMPAD7,        "Keypad 7" },
	{ OAPI_KEY_NUMPAD8,        "Keypad 8" },
	{ OAPI_KEY_NUMPAD9,        "Keypad 9" },
	{ OAPI_KEY_SUBTRACT,       "Keypad -" },
	{ OAPI_KEY_NUMPAD4,        "Keypad 4" },
	{ OAPI_KEY_NUMPAD5,        "Keypad 5" },
	{ OAPI_KEY_NUMPAD6,        "Keypad 6" },
	{ OAPI_KEY_ADD,            "Keypad +" },
	{ OAPI_KEY_NUMPAD1,        "Keypad 1" },
	{ OAPI_KEY_NUMPAD2,        "Keypad 2" },
	{ OAPI_KEY_NUMPAD3,        "Keypad 3" },
	{ OAPI_KEY_NUMPAD0,        "Keypad 0" },
	{ OAPI_KEY_DECIMAL,        "Keypad ." },
	{ OAPI_KEY_OEM_102,        "OEM 102" },
	{ OAPI_KEY_F11,            "F11" },
	{ OAPI_KEY_F12,            "F12" },
	{ OAPI_KEY_NUMPADENTER,    "Keypad Enter" },
	{ OAPI_KEY_RCONTROL,       "RCtrl" },
	{ OAPI_KEY_DIVIDE,         "Keypad /" },
	{ OAPI_KEY_SYSRQ,          "SysReq" },
	{ OAPI_KEY_RALT,           "RAlt" },
	{ OAPI_KEY_PAUSE,          "Pause" },
	{ OAPI_KEY_HOME,           "Home" },
	{ OAPI_KEY_UP,             "Up" },
	{ OAPI_KEY_PRIOR,          "Page Up" },
	{ OAPI_KEY_LEFT,           "Left" },
	{ OAPI_KEY_RIGHT,          "Right" },
	{ OAPI_KEY_END,            "End" },
	{ OAPI_KEY_DOWN,           "Down" },
	{ OAPI_KEY_NEXT,           "Page Down" },
	{ OAPI_KEY_INSERT,         "Insert" },
	{ OAPI_KEY_DELETE,         "Delete" }
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

int VesimInput::addConnection(int deviceID, int subdeviceType, int subdeviceID, int modifiers, bool reverse){
	if (nconns >= VESIM_MAX_INPUT_CONNS) return -1;
	conn[nconns].deviceID = deviceID;
	conn[nconns].subdeviceID = subdeviceID;
	conn[nconns].subdeviceType = subdeviceType;
	conn[nconns].modifiers = modifiers;
	conn[nconns].reverse = reverse;
	conn[nconns].value = 0;
	return nconns++;
}

VesimDevice::VesimDevice(Vesim *parent) : parent(parent), name("Keyboard") {
	type = VESIM_DEVICETYPE_KEYBOARD; 
};

VesimDevice::VesimDevice(Vesim *parent, const char* deviceName, LPDIRECTINPUTDEVICE8 dx8_joystick) : 
	parent(parent),  name(deviceName), dx8_joystick(dx8_joystick) {
	type = VESIM_DEVICETYPE_JOYSTICK;
	dx8_joystick->SetDataFormat(&c_dfDIJoystick2);
};

void VesimDevice::poolDevice() {
	if (type == VESIM_DEVICETYPE_JOYSTICK) {
		HRESULT hr = dx8_joystick->Poll();
		if (FAILED(hr)) { // Did that work?
						  // Attempt to acquire the device
			hr = dx8_joystick->Acquire();
			if (FAILED(hr)) {
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

}

Vesim::Vesim(CbInputChanged cbInputChanged, void *pCbData) : cbInputChanged(cbInputChanged), pCbData(pCbData) {
	int i;
	for (i = 0; i < VESIM_MAX_INPUTS; i++)
		inpid2idx[i] = -1;
	for (i = 0; i < 256; i++) {
		key2conn[i][0] = -1;
		key2conn[i][1] = -1;
	}
	vdev.emplace_back(this);
#ifdef _DEBUG
	out_file = fopen("vesim.log", "wt");
	fprintf(out_file, "Vesim object created\n");
#endif
}

Vesim::~Vesim() {
	int ndev = vdev.size();
	for (int i = 0; i < ndev; i++) {
		VesimDevice *d = &vdev[i];
		if (d->type == VESIM_DEVICETYPE_JOYSTICK) {
			d->dx8_joystick->Unacquire();
			d->dx8_joystick->Release();
		}
	}
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
	fflush(ves->out_file);
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
#ifdef _DEBUG
	fprintf(out_file, "Setting up controller devices for vessel %s\n", vesselStationName);
	fflush(out_file);
#endif
	this->vesselStationName = vesselStationName;
	this->dx8ppv = dx8ppv;
	dx8ppv->EnumDevices(DI8DEVCLASS_GAMECTRL, VesimEnumJoysticksCB, this, DIEDFL_ATTACHEDONLY);
	int ndev = vdev.size();
	for (int devid = 0; devid < ndev; devid++) {
		std::string devicename = vdev[devid].name;
#ifdef _DEBUG
		fprintf(out_file, "Setting up device %s\n", devicename.c_str());
#endif
		std::string configdir = "Config\\ProjectApollo\\Vesim\\";
		int devtype = vdev[devid].type;

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
		if (!fdevcfg.is_open()) break;

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
							if (devtype == VESIM_DEVICETYPE_JOYSTICK) {
								if (token.length() > 5 && icomp(token.substr(0, 4).c_str(), "Axis")) {
									subdevtype = VESIM_SUBDEVTYPE_AXIS;
									std::string ssid = token.substr(4);
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
									catch (int e) {
										(void)e;
									}
								}
								else if (token.length() > 3 && icomp(token.substr(0, 3).c_str(), "POV")) {
									subdevtype = VESIM_SUBDEVTYPE_AXIS;
									std::string ssid = token.substr(3);
									const char * psid = ssid.c_str();
									for (int k = 0; k < 8; k++) {
										if (icomp(psid, povaxnames[k])) {
											subdevid = k + 16;
											break;
										}
									}
								}
							}
							else if (devtype == VESIM_DEVICETYPE_KEYBOARD) {
								subdevtype = VESIM_SUBDEVTYPE_KEY;
								for (int k = 0; k < KEYDATA_CNT; k++) {
									if (icomp(token.c_str(), keydata[k].keyName)) {
										subdevid = keydata[k].oapiID;
										break;
									}
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

					if (icomp(vinp[k].name.c_str(), inpname.c_str())) {
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
	int connidx= vinp[inputidx].addConnection(deviceID, subdeviceType, subdeviceID, modifiers, reverse);
	if (connidx >= 0) {
		if (subdeviceType == VESIM_SUBDEVTYPE_KEY) {
			key2conn[subdeviceID][0] = inputidx;
			key2conn[subdeviceID][1] = connidx;
		}
		return true;
	}
	return false;
}

int Vesim::clbkConsumeBufferedKey(DWORD key, bool down, char *keystate) {
	if (key >= 0 && key < 256) {
		int inpidx=key2conn[key][0];
#ifdef _DEBUG
		fprintf(out_file, "Key callback key:%d inpidx:%d \n", (int) key, inpidx);
		fflush(out_file);
#endif
		boolean isSet = false;
		int newValue = 0;
		if (inpidx >= 0) {
			VesimInput *inp = &vinp[inpidx];
			VesimDeviceInputConn *pconn = &inp->conn[key2conn[key][1]];
			if (down) {
				pconn->value = 1;
				isSet = true;
				if (inp->type == VESIM_INPUTTYPE_BUTTON) {
					newValue = pconn->reverse ? 0 : 1;
				}
				else newValue = pconn->reverse ? 0 : 65535;
			}
			else {
				pconn->value = 0;
				if (inp->type == VESIM_INPUTTYPE_BUTTON) {
					isSet = true;
					newValue = pconn->reverse ? 1 : 0;
				}
			}
			if (!isSet) newValue = inp->defaultValue;
			int oldValue = inp->value;
			inp->value = newValue;

#ifdef _DEBUG
			fprintf(out_file, "Value changed key:%d oldValue:%d newValue:%d\n", (int)key, oldValue, newValue);
			fflush(out_file);
#endif

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
	return 0;
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
					pconn->value = newValue;
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_Y:
					newValue = pdev->dx8_jstate.lY;
					pconn->value = newValue;
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_Z:
					newValue = pdev->dx8_jstate.lZ;
					pconn->value = newValue;
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_RX:
					newValue = pdev->dx8_jstate.lRx;
					pconn->value = newValue;
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_RY:
					newValue = pdev->dx8_jstate.lRy;
					pconn->value = newValue;
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_RZ:
					newValue = pdev->dx8_jstate.lRz;
					pconn->value = newValue;
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_SLDR0:
					newValue = pdev->dx8_jstate.rglSlider[0];
					pconn->value = newValue;
					isSet = true;
					break;
				case VESIM_DEVICE_AXIS_SLDR1:
					newValue = pdev->dx8_jstate.rglSlider[1];
					pconn->value = newValue;
					isSet = true;
					break;
				default: //It is a POV axis
					int povidx = pconn->subdeviceID-16;
					int povval = pdev->dx8_jstate.rgdwPOV[povidx>>1];					
					if ((povval & 0xFFFF) != 0xFFFF) {																
						double povcos = cos(PI*povval / 18000.0);
						double povsin = sin(PI*povval / 18000.0);
						double maxnorm = max(abs(povcos), abs(povsin));
						povcos = povcos / maxnorm;
						povsin = povsin / maxnorm;
						if (povidx & 1) {
							newValue = min((int)round(32768 - 327678.0*povcos), 65535);
						}
						else {
							newValue = min((int)round(32768 + 327678.0*povsin), 65535);
						}
						isSet = true;						
					}
					pconn->value = povval;
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
					pconn->value = 1;
					isSet = true;
					if (inp->type == VESIM_INPUTTYPE_BUTTON) {
						newValue = pconn->reverse ? 0 : 1;
					}
					else newValue = pconn->reverse?  0 : 65535;
				}
				else {
					pconn->value = 0;
					if (inp->type == VESIM_INPUTTYPE_BUTTON) {
						isSet = true;
						newValue = pconn->reverse ? 1 : 0;
					}
				}					
			}
			else if (pconn->subdeviceType == VESIM_SUBDEVTYPE_KEY) {
				if (pconn->value) {
					newValue = inp->value;
					isSet = true;
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
