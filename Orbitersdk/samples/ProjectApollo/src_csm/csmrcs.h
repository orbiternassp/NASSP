/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2008

  CSM Reaction Control System

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

#if !defined(_PA_CSMRCS_H)
#define _PA_CSMRCS_H

class SMRCSPropellantSource;

///
/// \ingroup Propulsion
///
class RCSValve {

public:
	virtual bool IsOpen() = 0;
};

///
/// \ingroup Propulsion
///
class SMRCSHeliumValve : public RCSValve {

public:
	SMRCSHeliumValve();
	void SetPropellantSource(SMRCSPropellantSource *p, bool sec);
	void SetState(bool open);
	bool IsOpen() { return isOpen; };
	void SwitchToggled(PanelSwitchItem *s);
	
protected:
	bool isOpen;
	bool isSec;
	SMRCSPropellantSource *propellant;
};

///
/// \ingroup Propulsion
///
class RCSPropellantValve : public RCSValve {

public:
	RCSPropellantValve();
	void SetState(bool open);
	bool IsOpen() { return isOpen; };
	
protected:
	bool isOpen;
};

///
/// \ingroup Propulsion
///
class SMRCSPropellantSource : public PropellantSource {

public:
	SMRCSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p);
	virtual ~SMRCSPropellantSource();

	void Init(THRUSTER_HANDLE *th, h_Radiator *p);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);

	double GetPackageTempF();
	double GetHeliumTempF();
	double GetHeliumPressurePSI();
	double GetPropellantPressurePSI() { return propellantPressurePSI; }
	double GetPropellantQuantityToDisplay() { return propellantQuantityToDisplay; }
	
	SMRCSHeliumValve *GetHeliumValve1() { return &heliumValve1; }
	SMRCSHeliumValve *GetHeliumValve2() { return &heliumValve2; }
	SMRCSHeliumValve *GetSecPropellantPressureValve() { return &secPropellantPressureValve; }
	RCSPropellantValve *GetPrimPropellantValve() { return &primPropellantValve; };
	RCSPropellantValve *GetSecPropellantValve() { return &secPropellantValve; };
	
	void PropellantSwitchToggled(PanelSwitchItem *s);
	void CheckHeliumValves();
	void CheckSecHeliumValve();

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	void CheckPropellantMass();

protected:
	void SetThrusters(PROPELLANT_HANDLE ph);

	double propellantBuffer;
	double lastPropellantMass;
	double primPropellantMass;
	double lastPrimPropellantMassHeliumValvesClosed;
	double lastSecPropellantMassHeliumValvesClosed;
	double propellantPressurePSI;
	double heliumPressurePSI;
	double propellantQuantityToDisplay;

	SMRCSHeliumValve heliumValve1;
	SMRCSHeliumValve heliumValve2;
	SMRCSHeliumValve secPropellantPressureValve;
	RCSPropellantValve primPropellantValve;
	RCSPropellantValve secPropellantValve;

	THRUSTER_HANDLE *thrusters;
	h_Radiator *package;
};

///
/// \ingroup Propulsion
///
class CMRCSPropellantSource : public PropellantSource {

public:
	CMRCSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p);
	virtual ~CMRCSPropellantSource();

	void Init(THRUSTER_HANDLE *th, h_Radiator *t, CMRCSPropellantSource *ic, e_object *pp, e_object *ppp);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);

	double GetHeliumTempF();
	double GetHeliumPressurePSI();
	double GetPropellantPressurePSI();
	RCSPropellantValve *GetPropellantValve() { return &propellantValve; };
	double *GetPurgeLevelRef(int i) { return &purgeLevel[i]; }	
	void PropellantSwitchToggled(PanelSwitchItem *s);
	void OpenHeliumValves();

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	void CheckPropellantMass();

protected:
	void OpenPurgeValves();
	void SetThrusters(PROPELLANT_HANDLE ph);
	void SetPurgeLevel(bool on, double simdt);

	double lastPropellantMass;
	double propellantBuffer;
	double heliumQuantity;
	RCSPropellantValve propellantValve;
	bool heliumValvesOpen;
	bool fuelInterconnectValvesOpen;
	bool oxidizerInterconnectValvesOpen;
	bool purgeValvesOpen;
	double purgeLevel[6]; 

	THRUSTER_HANDLE *thrusters;
	h_Radiator *heliumTank;
	CMRCSPropellantSource *interconnectedSystem;
	e_object *purgePower, *purgePyroPower;
};

//
// Strings for state saving.
//

#define SMRCSPROPELLANT_A_START_STRING "SMRCSPROPELLANT_A_BEGIN"
#define SMRCSPROPELLANT_B_START_STRING "SMRCSPROPELLANT_B_BEGIN"
#define SMRCSPROPELLANT_C_START_STRING "SMRCSPROPELLANT_C_BEGIN"
#define SMRCSPROPELLANT_D_START_STRING "SMRCSPROPELLANT_D_BEGIN"
#define SMRCSPROPELLANT_END_STRING     "SMRCSPROPELLANT_END"

#define CMRCSPROPELLANT_1_START_STRING "CMRCSPROPELLANT_1_BEGIN"
#define CMRCSPROPELLANT_2_START_STRING "CMRCSPROPELLANT_2_BEGIN"
#define CMRCSPROPELLANT_END_STRING     "CMRCSPROPELLANT_END"

#endif
