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

#pragma once

#define VESIM_INPUTTYPE_BUTTON 1
#define VESIM_INPUTTYPE_AXIS   2

#define VESIM_MAX_INPUTS       1024
#define VESIM_MAX_INPUT_CONNS  16

#define VESIM_DEFAULT_AXIS_VALUE 32768

#define VESIM_EVTTYPE_BUTTON_ON  1
#define VESIM_EVTTYPE_BUTTON_OFF 2
#define VESIM_EVTTYPE_AXIS_CHANGED 2



class Vesim;

typedef  void (*CbInputChanged)(int inputID, int eventType, int newValue, void *pdata);

struct VesimInputDefinition {
	int id;
	char *name;
	int type;
	int defaultValue;
	bool notifyOnChange;
};

struct VesimDeviceInputConn {
	int deviceID;
	int subdeviceType;
	int subdeviceID;
	int modifiers;
	bool reverse;
};

class VesimInput {
private:
	int ID;
	std::string name;
	int type;
	int defaultValue;
	bool notifyOnChange;
	int nconns{ 0 };
	VesimDeviceInputConn conn[VESIM_MAX_INPUT_CONNS];

	int value;

public:
	VesimInput(int inputID, char *name, int type, int defaultValue, bool notifyOnChange) :
		ID(inputID),
		name(name),
		type(type),
		defaultValue(defaultValue),
		notifyOnChange(notifyOnChange),
		value(defaultValue)
	{};

	int VesimInput::addConnection(int deviceID, int subdeviceType, int subdeviceID, int modifiers, bool reverse);

	friend class VesimDevice;
	friend class Vesim;
};

class VesimDeviceAxis {
};

class VesimDevice {
private:
	std::string name;
	Vesim *parent;
	int type;
	std::vector<VesimDeviceAxis> axes;
	LPDIRECTINPUTDEVICE8 dx8_joystick {NULL};
	DIJOYSTATE2			 dx8_jstate;

	void poolDevice();

public:
	VesimDevice(Vesim* parent);
	VesimDevice(Vesim* parent, const char* deviceName, LPDIRECTINPUTDEVICE8 dx8_joystick);
	~VesimDevice();
	friend class Vesim;
};

class Vesim
{
private:
	CbInputChanged cbInputChanged;
	void *pCbData;
	std::vector<VesimInput> vinp;
	int inpid2idx[VESIM_MAX_INPUTS];
	int key2conn[256][2];

	std::vector<VesimDevice> vdev;
	std::vector<VesimDeviceInputConn> vconn;

	bool Vesim::connectDeviceToInput(int inputidx, int deviceID, int subdeviceType, int subdeviceID, bool reverse, int modifiers);
public:
	char* vesselStationName;
	LPDIRECTINPUT8 dx8ppv;

#ifdef _DEBUG
	FILE *out_file;
#endif
	Vesim(CbInputChanged cbInputChanged, void *pCbData);
	~Vesim();
	bool addInput(int inputID, char *inputName, int inputType, int defaultValue = VESIM_DEFAULT_AXIS_VALUE, bool notifyOnChange = false);
	bool addInput(VesimInputDefinition *vid);
	bool setupDevices(char* vesselStationName, LPDIRECTINPUT8 dx8ppv);
	int clbkConsumeBufferedKey(DWORD key, bool down, char *keystate);
	void poolDevices();
	int getInputValue(int inputID);

	friend BOOL CALLBACK VesimEnumJoysticksCB(const DIDEVICEINSTANCE* pdidInstance, VOID* pVesim);
};