/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  CSM Service Propulsion System

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

#if !defined(_PA_SPS_H)
#define _PA_SPS_H


class SPSPropellantSource : public PropellantSource {

public:
	SPSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p);
	virtual ~SPSPropellantSource();

	void Init(e_object *dc1, e_object *dc2, e_object *ac, h_Radiator *propline);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	double GetFuelPercent();
	double GetOxidPercent();
	double GetOxidUnbalanceLB();
	double GetPropellantPressurePSI() { return propellantPressurePSI; }
	double GetHeliumPressurePSI() { return heliumPressurePSI; }
	double GetNitrogenPressurePSI() { return nitrogenPressurePSI; }
	double GetPropellantLineTempF() { return KelvinToFahrenheit(propellantLine->GetTemp()); };
	bool IsHeliumValveAOpen() { return heliumValveAOpen; }
	bool IsHeliumValveBOpen() { return heliumValveBOpen; }
	bool IsOxidFlowValveMin();
	bool IsOxidFlowValveMax();
	void SPSTestSwitchToggled();
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

protected:
	bool IsGaugingPowered();

	double oxidMass;
	double primOxidFlowValve;
	double secOxidFlowValve;
	double primTestStatus;
	double primTestTimer;
	double auxTestStatus;
	double propellantMassToDisplay;
	double oxidMassToDisplay;
	double propellantMaxMassToDisplay;
	double propellantPressurePSI;
	double heliumPressurePSI;
	double nitrogenPressurePSI;
	double lastPropellantMassHeliumValvesClosed;
	bool heliumValveAOpen;
	bool heliumValveBOpen;

	PowerMerge DCPower;
	e_object *ACPower;
	h_Radiator *propellantLine;
	double propellantInitialized;
	double lastPropellantMass;
};

class SPSEngine {

public:
	SPSEngine(THRUSTER_HANDLE &sps);
	virtual ~SPSEngine();

	void Init(Saturn *s);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	double GetChamberPressurePSI();
	bool GetInjectorValves12Open() { return injectorValves12Open; };
	bool GetInjectorValves34Open() { return injectorValves34Open; };
	// Forcibly activate the SPS engine for unmanned control.
	void EnforceBurn(bool burn) { enforceBurn = burn; }
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

protected:
	bool injectorValves12Open;
	bool injectorValves34Open;
	bool enforceBurn;
	bool engineOnCommanded;

	Saturn *saturn;
	THRUSTER_HANDLE &spsThruster;
};



//
// Strings for state saving.
//

#define SPSPROPELLANT_START_STRING "SPSPROPELLANT_BEGIN"
#define SPSPROPELLANT_END_STRING   "SPSPROPELLANT_END"

#define SPSENGINE_START_STRING "SPSENGINE_BEGIN"
#define SPSENGINE_END_STRING   "SPSENGINE_END"

#endif
