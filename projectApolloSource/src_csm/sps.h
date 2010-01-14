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
  *	Revision 1.2  2009/12/17 17:47:18  tschachim
  *	New default checklist for ChecklistMFD together with a lot of related bugfixes and small enhancements.
  *	
  *	Revision 1.1  2009/02/18 23:20:56  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.7  2007/12/04 20:26:38  tschachim
  *	IMFD5 communication including a new TLI for the S-IVB IU.
  *	Additional CSM panels.
  *	
  *	Revision 1.6  2007/10/18 00:23:27  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.5  2007/06/06 15:02:22  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.4  2007/04/25 18:49:25  tschachim
  *	SCS dV mode.
  *	
  *	Revision 1.3  2007/01/22 15:48:19  tschachim
  *	SPS Thrust Vector Control, RHC power supply, THC clockwise switch, bugfixes.
  *	
  *	Revision 1.2  2006/11/20 16:38:27  tschachim
  *	Bugfix CWS CM/SM separation.
  *	
  *	Revision 1.1  2006/11/13 14:47:34  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  **************************************************************************/

#if !defined(_PA_SPS_H)
#define _PA_SPS_H

///
/// \ingroup Propulsion
///
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
	double GetPropellantLineTempF();
	bool IsHeliumValveAOpen() { return heliumValveAOpen; }
	bool IsHeliumValveBOpen() { return heliumValveBOpen; }
	bool IsOxidFlowValveMin();
	bool IsOxidFlowValveMax();
	void ResetOxidFlowValve() { primOxidFlowValve = 0; }

	void SPSTestSwitchToggled();
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	void CheckPropellantMass();

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
	double lastPropellantMassHeliumValvesClosed;
	bool heliumValveAOpen;
	bool heliumValveBOpen;

	PowerMerge DCPower;
	e_object *ACPower;
	h_Radiator *propellantLine;
	bool propellantInitialized;
	double lastPropellantMass;
	double propellantBuffer;
};

///
/// \ingroup Propulsion
///
class SPSGimbalActuator {

public:
	SPSGimbalActuator();
	virtual ~SPSGimbalActuator();

	void Init(Saturn *s, ThreePosSwitch *driveSwitch, ThreePosSwitch *m1Switch, ThreePosSwitch *m2Switch,
	          e_object *m1Source, e_object *m1StartSource, e_object *m2Source, e_object *m2StartSource,
			  ThumbwheelSwitch *tThumbwheel, ThreePosSwitch* modeSwitch);
	void Timestep(double simt, double simdt, double attitudeError, double attitudeRate, int rhcAxis);
	void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	double GetPosition() { return position; }
	void ChangeCMCPosition(double delta);
	void ZeroCMCPosition() { cmcPosition = 0; }

protected:
	bool IsSystem1Powered();
	bool IsSystem2Powered();
	void DrawSystem1Power();
	void DrawSystem2Power();

	double position;
	double commandedPosition;
	double cmcPosition;
	double scsPosition;
	double lastAttitudeError;
	int activeSystem;
	bool motor1Running;
	bool motor2Running;

	Saturn *saturn;
	ThreePosSwitch *tvcGimbalDriveSwitch, *gimbalMotor1Switch, *gimbalMotor2Switch, *scsTvcModeSwitch;
	e_object *motor1Source, *motor1StartSource, *motor2Source, *motor2StartSource;
	ThumbwheelSwitch *trimThumbwheel;
};

///
/// \ingroup Propulsion
///
class SPSEngine {

public:
	SPSEngine(THRUSTER_HANDLE &sps);
	virtual ~SPSEngine();

	void Init(Saturn *s);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	double GetChamberPressurePSI();
	bool IsThrustOn() { return thrustOn; };
	bool GetInjectorValves12Open() { return injectorValves12Open; };
	bool GetInjectorValves34Open() { return injectorValves34Open; };
	double GetNitrogenPressureAPSI() { return nitrogenPressureAPSI; };
	double GetNitrogenPressureBPSI() { return nitrogenPressureBPSI; };
	// Forcibly activate the SPS engine for unmanned control.
	void EnforceBurn(bool burn) { enforceBurn = burn; }
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	SPSGimbalActuator pitchGimbalActuator;
	SPSGimbalActuator yawGimbalActuator;
	bool cmcErrorCountersEnabled;

protected:
	bool thrustOn;
	bool injectorValves12Open;
	bool injectorValves34Open;
	bool enforceBurn;
	bool engineOnCommanded;
	double nitrogenPressureAPSI;
	double nitrogenPressureBPSI;

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

#define SPSGIMBALACTUATOR_PITCH_START_STRING "SPSGIMBALACTUATOR_PITCH_BEGIN"
#define SPSGIMBALACTUATOR_YAW_START_STRING "SPSGIMBALACTUATOR_YAW_BEGIN"
#define SPSGIMBALACTUATOR_END_STRING "SPSGIMBALACTUATOR_END"

#endif
