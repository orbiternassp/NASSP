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

	void Init(e_object *dc1, e_object *dc2, e_object *ac, h_Radiator *inj1, h_Radiator *inj2);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	double GetFuelPercent();
	double GetOxidPercent();
	double GetOxidUnbalanceLB();
	double GetPropellantPressurePSI() { return propellantPressurePSI; }
	double GetHeliumPressurePSI() { return heliumPressurePSI; }
	double GetInjectorFlangeTempF(int i);
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
	h_Radiator *OXInjectorFlange1;
	h_Radiator *FUInjectorFlange2;
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

	void Init(Saturn *s, ServoAmplifierModule *servoAmp, ThreePosSwitch *m1Switch, ThreePosSwitch *m2Switch,
	          e_object *m1Source, e_object *m1StartSource, e_object *m2Source, e_object *m2StartSource);
	void Timestep(double simdt);
	void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	double GetPosition() { return position; }
	double GetCommandedPosition() { return commandedPosition; }
	void CommandedPositionInc(double dangle) { commandedPosition += dangle; }
	bool HasGimbalMotorOverCurrent() { return false; }

protected:
	void GimbalTimestep(double simdt, double desPos);

	bool IsSystem1Powered();
	bool IsSystem2Powered();

	double position;
	double commandedPosition;
	bool motor1Running;
	bool motor2Running;

	Saturn *saturn;
	ThreePosSwitch *gimbalMotor1Switch, *gimbalMotor2Switch;
	e_object *motor1Source, *motor1StartSource, *motor2Source, *motor2StartSource;
	ServoAmplifierModule *servoAmplifier;
};

///
/// \ingroup Propulsion
///
class SPSEngine {

public:
	SPSEngine(THRUSTER_HANDLE &sps);
	virtual ~SPSEngine();

	void Init(Saturn *s, h_HeatLoad *h);
	void DefineAnimations(UINT idx);
	void DeleteAnimations();
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	double GetChamberPressurePSI();
	bool IsThrustOnA() { return thrustOnA; };
	bool IsThrustOnB() { return thrustOnB; };
	bool GetInjectorValves12Open() { return injectorValves12Open; };
	bool GetInjectorValves34Open() { return injectorValves34Open; };
	double GetNitrogenPressureAPSI() { return nitrogenPressureAPSI; };
	double GetNitrogenPressureBPSI() { return nitrogenPressureBPSI; };
	double GetInjectorValvePosition(int i);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	void clbkPostCreation();

	SPSGimbalActuator pitchGimbalActuator;
	SPSGimbalActuator yawGimbalActuator;
	h_HeatLoad *spsThrustHeat;

protected:
	bool thrustOnA, thrustOnB;
	bool injectorValves12Open;
	bool injectorValves34Open;
	double nitrogenPressureAPSI;
	double nitrogenPressureBPSI;
	double t_on;
	double t_off;
	double T_SPS;
	bool dualBoreStatus;

	Saturn *saturn;
	THRUSTER_HANDLE &spsThruster;

	// Animations
	UINT anim_SPSGimbalPitch, anim_SPSGimbalYaw;
	double spsgimbal_proc[2];
	double spsgimbal_proc_last[2];

	static const double tau16_D; //Thrust on delay (dual)
	static const double tau16_S; //Thrust on delay (single)
	static const double tau17_D; //Thrust off delay (dual)
	static const double tau17_S; //Thrust off delay (single)
	static const double tau18;
	static const double tau19; //End of thrust decay
	static const double tau20_D; //Thrust on slope (dual)
	static const double tau20_S; //Thrust on slope (single)
	static const double K1_D; //Dual bore
	static const double K1_S; //Single bore
	static const double K2;
	static const double K3_D; //Initial thrust of second decay phase (dual)
	static const double K3_S; //Initial thrust of second decay phase (single)

	double SPSThrustOnDelayDual(double t);
	double SPSThrustOffDelayDual(double t);
	double SPSThrustOnDelaySingle(double t);
	double SPSThrustOffDelaySingle(double t);
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
