/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Stability & Control Systems definitions.
  The implementation is in satsystems.cpp

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

#include "DelayTimer.h"

class Saturn;

class AttitudeReference {

public:
	AttitudeReference();
	virtual void Init(VESSEL *v);
	virtual void Timestep(double simdt);
	virtual void SaveState(FILEHANDLE scn); 
	virtual void LoadState(char *line);
	VECTOR3 GetAttitude() { return Attitude; };
	VECTOR3 GetLastAttitude() { return LastAttitude; };
	void SetAttitude(VECTOR3 a);

	static MATRIX3 GetRotationMatrixX(double angle);
	static MATRIX3 GetRotationMatrixY(double angle);
	static MATRIX3 GetRotationMatrixZ(double angle);

protected:
	bool AttitudeInitialized;
	VESSEL *Vessel;
	VECTOR3 Attitude;
	VECTOR3 LastAttitude;

	struct {					// Orbiter's state
		VECTOR3 Attitude;
		MATRIX3 AttitudeReference;
	} OrbiterAttitude;

	void SetOrbiterAttitudeReference();
	MATRIX3 GetNavigationBaseToOrbiterLocalTransformation();
	MATRIX3 GetOrbiterLocalToNavigationBaseTransformation();
	VECTOR3 GetRotationAnglesXZY(MATRIX3 m);
};

// Body-Mounted Attitude Gyro

#define BMAG1_START_STRING	"BMAG1_BEGIN"
#define BMAG2_START_STRING	"BMAG2_BEGIN"
#define BMAG_END_STRING	    "BMAG_END"

class BMAG {
	
public: 
	BMAG();                                                                  // Cons
	void Init(int n, Saturn *v, e_object *dcbus, e_object *acbus, Boiler *h);		 // Initialization
	void Timestep(double simdt);                                             // Update function
	void SystemTimestep(double simdt);
	void Cage(int axis);
	void Uncage(int axis);
	VECTOR3 GetAttitudeError();												 // Attitude error when uncaged
	VECTOR3 GetRates() { return rates; };
	VECTOR3 IsUncaged() { return uncaged; };
	Boiler *GetHeater() { return heater; };
	bool IsPowered() { return powered; };
	void SetPower(bool dc, bool ac);
	double GetTempF();
	void SaveState(FILEHANDLE scn); 
	void LoadState(FILEHANDLE scn); 

protected:
	e_object *dc_source;                                                     // DC source for gyro heater
	e_object *ac_source;													 // 3-Phase AC source for gyro
	e_object *dc_bus;					  	          					     // DC source to use when powered
	e_object *ac_bus;														 // 3-Phase AC source to use when powered
	Boiler *heater;															 // Heat coldplates when powered
	double temperature;                                                         // Temperature
	VECTOR3 rates;                                                           // Detected rotation acceleration
	VECTOR3 uncaged;														 // 0 = caged, 1 = not caged (each axis)
	VECTOR3 errorAttitude;												     // Body attitude error when uncaged
	Saturn *sat;                                                             // Pointer to ship we're attached to
	bool powered;                                                            // Data valid flag.
	int number;																 // BMAG 1 or 2
};


// Gyro Display Coupler

#define GDC_START_STRING	"GDC_BEGIN"
#define GDC_END_STRING		"GDC_END"

class GDC {
	
public: // We use these inside a timestep, so everything is public to make data access as fast as possible.
	GDC();                          // Cons
	void Init(Saturn *v);		    // Initialization
	void Timestep(double simt);     // TimeStep
	void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn); // SaveState callback
	void LoadState(FILEHANDLE scn); // LoadState callback

	bool GetYawAttitudeSetInputEnable() { return A9K1; }
	bool GetPitchRollAttitudeSetInputEnable() { return A9K2; }

	double GetRollBodyMinusEulerError();
	double GetPitchBodyError();
	double GetYawBodyError();

	double GetRollEulerResolver();
	double GetPitchEulerResolver();
	double GetYawEulerResolver();

	VECTOR3 rates;					// Integrated Euler rotation rates
	Saturn *sat;
	// FDAI error needle data from CMC
	int fdai_err_ena;
	int fdai_err_x;
	int fdai_err_y;
	int fdai_err_z;

	friend class CSMcomputer; // Needs to write FDAI error indications, which are really not on the GDC, but meh.

protected:

	VECTOR3 Attitude;

	//RELAYS

	//Pitch Euler mode enable
	bool A2K1;
	//Pitch 0.05 config
	bool A2K2;
	//Pitch align enable
	bool A2K3;
	//Pitch rate 1 enable
	bool A2K4;

	//Roll Euler mode enable
	bool A3K1;
	//Roll 0.05G config
	bool A3K2;
	//Roll align enable
	bool A3K3;
	//GDC roll rate 1 enable
	bool A3K4;

	//Yaw Euler mode enable
	bool A4K1;
	//Yaw 0.05G configuration
	bool A4K2;
	//Yaw align enable
	bool A4K3;
	//GDC yaw rate 1 enable
	bool A4K4;

	//Yaw 0.05G Configuration
	bool A6K1;
	//EMS roll display on
	bool A6K2;

	//Resolver excitation
	bool A8K1;
	//EMS roll display on 
	bool A8K2;

	//Yaw attitude set input enable
	bool A9K1;
	//Pitch, roll attitude set input enable
	bool A9K2;
	//Secant function enable
	bool A9K3;

	//POWER FLAGS

	//Euler resolver power (Pitch, Roll)
	bool E0_505PR;
	//Euler resolver power (Yaw)
	bool E0_505Y;
};


// Attitude Set Control Panel

#define ASCP_START_STRING	"ASCP_BEGIN"
#define ASCP_END_STRING		"ASCP_END"

class ASCP {
	
public: // We use these inside a timestep, so everything is public to make data access as fast as possible.
	ASCP(Sound &clicksound);									   // Cons
	void Init(Saturn *vessel);									   // Initialization
	void TimeStep(double simdt);						           // Timestep
	bool PaintRollDisplay(SURFHANDLE surf, SURFHANDLE digits);     // Update panel image
	bool PaintPitchDisplay(SURFHANDLE surf, SURFHANDLE digits);    // Update panel image
	bool PaintYawDisplay(SURFHANDLE surf, SURFHANDLE digits);      // Update panel image
	void PaintRoll(SURFHANDLE surf, SURFHANDLE wheel);
	void PaintPitch(SURFHANDLE surf, SURFHANDLE wheel);
	void PaintYaw(SURFHANDLE surf, SURFHANDLE wheel);
	bool RollDisplayClicked();									   // Clicked
	bool PitchDisplayClicked();									   // Clicked
	bool YawDisplayClicked();									   // Clicked
	bool RollUpClick(int Event);								   // Clicked
	bool RollDnClick(int Event);
	bool PitchUpClick(int Event);								   // Clicked
	bool PitchDnClick(int Event);
	bool YawUpClick(int Event);									   // Clicked
	bool YawDnClick(int Event);
	bool RollClick(int Event, int mx, int my);
	bool PitchClick(int Event, int mx, int my);
	bool YawClick(int Event, int mx, int my);
	void SaveState(FILEHANDLE scn);                                // SaveState callback
	void LoadState(FILEHANDLE scn);                                // LoadState callback

	double GetRollEulerAttitudeSetError();
	double GetPitchEulerAttitudeSetError();
	double GetYawEulerAttitudeSetError();
	double GetRollEulerAttitudeSetInput();
	double GetPitchEulerAttitudeSetInput();
	double GetYawEulerAttitudeSetInput();

	int mousedowncounter;                                          // Mouse Down Counter
	int mousedownposition;
	double mousedownangle;

	VECTOR3 output;												   // Output attitude
	Saturn *sat;												   // The spacecraft
	int msgcounter;
	double rolldisplay;
	double pitchdisplay;
	double yawdisplay;
	Sound &ClickSound;

protected:
	bool PaintDisplay(SURFHANDLE surf, SURFHANDLE digits, double value);
	double CalcRollEulerAttitudeSetError();
	double CalcPitchEulerAttitudeSetError();
	double CalcYawEulerAttitudeSetError();
};


// Electronic Display Assembly

#define EDA_START_STRING	"EDA_BEGIN"
#define EDA_END_STRING		"EDA_END"

class EDA {

public: // Same stuff about speed and I'm lazy too.
	EDA();															// Cons
	void Init(Saturn *vessel);										// Initialization
	void WireTo(e_object *ac1, e_object *ac2, e_object *dca, e_object *dcb);	// Wire to power sources
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn);                                // SaveState callback
	void LoadState(FILEHANDLE scn);                                // LoadState callback

	VECTOR3 GetFDAI1Attitude() { return FDAI1Attitude; }
	VECTOR3 GetFDAI2Attitude() { return FDAI2Attitude; }

	VECTOR3 GetFDAI1AttitudeRate() { return FDAI1AttitudeRate; }
	VECTOR3 GetFDAI2AttitudeRate() { return FDAI2AttitudeRate; }

	VECTOR3 GetFDAI1AttitudeError() { return FDAI1AttitudeError; }
	VECTOR3 GetFDAI2AttitudeError() { return FDAI2AttitudeError; }

	double GetConditionedPitchAttErr();
	double GetConditionedYawAttErr();
	double GetConditionedRollAttErr();

	double GetInstPitchAttRate();
	double GetInstYawAttRate();
	double GetInstRollAttRate();

	bool GetIMUtoAttSetRoll() { return A8K13; }
	bool GetIMUtoAttSetYaw() { return A8K15; }
	bool GetIMUtoAttSetPitch() { return A8K17; }

	bool GetGDCResolverExcitation() { return GDC118A8K1; }

protected:
	bool HasSigCondPower();
	bool IsPowered();
	double scale_data(double data);
	double inst_scale_rates(double data);
	void ResetRelays();
	void ResetTransistors();
	double NormalizeAngle(double ang);

	//Scaled -1 to 1
	VECTOR3 FDAI1AttitudeRate;
	VECTOR3 FDAI2AttitudeRate;
	VECTOR3 InstrAttitudeRate;
	//Simply in radians
	VECTOR3 FDAI1Attitude;
	VECTOR3 FDAI2Attitude;
	//Scaled -41 to 41 pixels
	VECTOR3 FDAI1AttitudeError;
	VECTOR3 FDAI2AttitudeError;
	VECTOR3 InstrAttitudeError;

	e_object *ac_source1;					  	          			// Power supply for FDAI 1 circuits
	e_object *ac_source2;											// Power supply for FDAI 2 circuits
	e_object *mna_source;					  	          			// Power supply for FDAI 1 circuits
	e_object *mnb_source;											// Power supply for FDAI 2 circuits

	Saturn *sat;

	//LOGIC SIGNAL

	//GDC Resolver Excitation
	bool GDC118A8K1;

	//RELAYS

	//FDAI No. 2 Attitude Enable
	bool A4K1;
	//FDAI No. 1 Attitude Motor Enable
	bool A4K2;
	//FDAI No. 1 Attitude Enable
	bool A4K3;
	//FDAI No. 2 Attitude Motor Enable
	bool A4K4;

	//FDAI No. 1 & No. 2 roll rate enable
	bool A5K1;
	//FDAI No. 1 and No. 2 yaw rate enable
	bool A5K2;
	//Pitch rate 1 enable
	bool A5K3;
	//FDAI rate and error source select
	bool A5K4;
	//FDAI rate and error source select
	bool A5K5;

	//IMU to FDAI No. 2 and GDC to FDAI No. 1 Enable
	bool A8K1;
	//IMU to FDAI No. 2 and GDC to FDAI No. 1 Enable
	bool A8K3;
	//IMU to FDAI No. 2 and GDC to FDAI No. 1 Enable
	bool A8K5;
	//IMU to FDAI No. 2 and GDC to FDAI No. 1 Enable
	bool A8K7;
	//IMU to FDAI No. 2 and GDC to FDAI No. 1 Enable
	bool A8K9;
	//IMU to FDAI No. 2 and GDC to FDAI No. 1 Enable
	bool A8K11;
	//IMU to Attitude Set Enable (Roll)
	bool A8K13;
	//IMU to Attitude Set Enable (Yaw)
	bool A8K15;
	//IMU to Attitude Set Enable (Pitch)
	bool A8K17;

	//FDAI No. 1 error enable
	bool A9K1;
	//FDAI No. 2 error enable
	bool A9K2;
	//FDAI No. 2 error enable
	bool A9K4;
	//FDAI rate and error source select
	bool A9K5;

	//LV Pressure Display Enable
	bool A11K1, A11K2, A11K3, A11K4, A11K5, A11K6;

	//TRANSISTORS

	//FDAI No. 1 Roll Gain Disable
	bool T1QS53;
	//FDAI No. 2 Roll Gain Disable
	bool T1QS54;
	//Attitude set to FDAI No. 1 Roll Error Disable
	bool T1QS55;
	//Attitude set to FDAI No. 2 Roll Error Disable
	bool T1QS56;
	//Roll error no. 1 to FDAI No. 1 disable
	bool T1QS57;
	//Roll error no. 1 to FDAI No. 2 disable
	bool T1QS58;
	//CDU roll error to FDAI No. 1 disable
	bool T1QS59;
	//Roll error to FDAI No. 2 disable, CDU
	bool T1QS60;
	//Roll rate 2 to FDAI No. 2 disable
	bool T1QS63;
	//Roll rate 1 to FDAI No. 1 disable
	bool T1QS64;
	//Roll rate 1 to FDAI No. 2 disable
	bool T1QS65;
	//FDAI No. 1 and No. 2 roll error scale factor low range enable
	bool T1QS67;
	//Roll rate 2 to FDAI No. 1 disable
	bool T1QS68;
	//Roll rate scale factor enable 5° or 50°
	bool T1QS71;
	//Roll rate scale factor enable 50°
	bool T1QS72;
	//GDC roll error to FDAI No. 1 and No. 2 disable
	bool T1QS73;
	//FDAI No. 1 50° scale factor disable
	bool T1QS75;
	//FDAI No. 2 50° scale factor disable
	bool T1QS76;
	//Attitude set/IMU gain disable
	bool T1QS78;

	//FDAI No. 1 Yaw Gain Disable
	bool T2QS53;
	//FDAI No. 2 Yaw Gain Disable
	bool T2QS54;
	//Attitude set to FDAI No. 1 yaw error disable
	bool T2QS55;
	//Attitude set to FDAI No. 2 yaw error disable
	bool T2QS56;
	//Yaw error no. 1 to FDAI No. 1 disable
	bool T2QS57;
	//Yaw error no. 1 to FDAI No. 2 disable
	bool T2QS58;
	//CDU yaw error to FDAI No. 1 disable
	bool T2QS59;
	//Yaw error to FDAI No. 2 disable, CDU
	bool T2QS60;
	//Roll rate 2 to FDAI No. 1 yaw rate cross coupling disable
	bool T2QS63;
	//Yaw rate 2 to FDAI No. 1 disable
	bool T2QS64;
	//Yaw rate 2 to FDAI No. 2 disable
	bool T2QS65;
	//Roll rate 2 to FDAI No. 2 yaw rate cross coupling disable
	bool T2QS66;
	//FDAI No. 1 yaw error scale factor low range enable
	bool T2QS67;
	//Roll rate 1 to FDAI No. 1 yaw rate cross coupling disable
	bool T2QS68;
	//Roll rate 1 to FDAI No. 2 yaw rate cross coupling disable
	bool T2QS69;
	//Yaw rate scale factor enable 5° or 10°
	bool T2QS71;
	//Yaw rate scale factor enable 10°
	bool T2QS72;
	//GDC yaw body attitude error to FDAI No. 1 disable
	bool T2QS73;
	//Yaw error to FDAI No. 2 disable
	bool T2QS74;
	//Yaw rate 1 to FDAI No. 1 disable
	bool T2QS76;
	//Yaw rate 1 to FDAI No. 2 disable
	bool T2QS77;

	//FDAI No. 1 Pitch Gain Disable
	bool T3QS53;
	//FDAI No. 2 Pitch Gain Disable
	bool T3QS54;
	//Attitude set to FDAI No. 1 pitch error disable
	bool T3QS55;
	//Attitude set to FDAI No. 2 pitch error disable
	bool T3QS56;
	//Pitch error no. 1 to FDAI No. 1 disable
	bool T3QS57;
	//Pitch error no. 1 to FDAI No. 2 disable
	bool T3QS58;
	//CDU pitch error to FDAI No. 1 disable
	bool T3QS59;
	//Pitch error to FDAI No. 2 disable, CDU
	bool T3QS60;
	//Pitch Rate 2 to FDAI No. 1 disable
	bool T3QS64;
	//Pitch Rate 2 to FDAI No. 2 disable
	bool T3QS65;
	//FDAI No. 1 pitch error scale factor low range enable
	bool T3QS67;
	//Pitch rate scale factor enable 5° or 10°
	bool T3QS71;
	//Pitch rate scale factor enable 10°
	bool T3QS72;
	//GDC pitch body attitude error to FDAI No. 1 disable
	bool T3QS73;
	//GDC pitch body attitude error to FDAI No. 2 disable
	bool T3QS74;
	//Pitch Rate 1 to FDAI No. 1 disable
	bool T3QS76;
	//Pitch Rate 1 to FDAI No. 2 disable
	bool T3QS77;
};


// Reaction Jet Engine Control

#define RJEC_START_STRING	"RJEC_BEGIN"
#define RJEC_END_STRING		"RJEC_END"

class RJEC {
	
public: // Same stuff about speed and I'm lazy too.
	RJEC();															// Cons
	void Init(Saturn *vessel);										// Initialization
	void TimeStep(double simdt);                                    // Timestep
	void SystemTimestep(double simdt);

	bool GetThruster(int thruster);
	void SetThruster(int thruster,bool Active);                     // Set Thruster Level for CMC

	bool GetSPSEnableA() { return SPSEnableA; }
	bool GetSPSEnableB() { return SPSEnableB; }
	bool GetIGN2() { return IGN2; }
	
	bool GetDirectPitchActive() { return DirectPitchActive; }
	bool GetDirectYawActive()   { return DirectYawActive; }
	bool GetDirectRollActive()  { return DirectRollActive; }
	void SetDirectPitchActive(bool active) { DirectPitchActive = active; }
	void SetDirectYawActive(bool active)   { DirectYawActive = active; }
	void SetDirectRollActive(bool active)  { DirectRollActive = active; }

	void SaveState(FILEHANDLE scn);                                // SaveState callback
	void LoadState(FILEHANDLE scn);                                // LoadState callback

protected:
	DelayTimer engineOnDelayA;
	DelayTimer engineOnDelayB;
	DelayOffTimer engineOffDelay;

	bool ThrusterDemand[20];                                        // Set when this thruster is requested to fire
	bool DirectPitchActive, DirectYawActive, DirectRollActive;      // Direct axis fire notification
	bool SCSLatchUpA, SCSLatchUpB;
	bool SPSEnableA, SPSEnableB;
	bool IGN1, IGN2;												// SPS Active notification

	Saturn *sat;
	ThreePosSwitch *PoweredSwitch[20];                              // Set when power is drawn from this switch

	void SetRCSState(int thruster, bool td, bool cm, int smquad, int smthruster, int cmthruster, ThreePosSwitch *s, bool lockout);
	bool IsThrusterPowered(ThreePosSwitch *s);
};


//ECA Integrator
class ECAIntegrator
{
public:
	ECAIntegrator(double g, double ulim, double llim);
	void Timestep(double input, double simdt);
	double GetState() { return state; }
	void Reset() { state = 0.0; }
protected:
	double state;
	double gain;
	double upperLimit;
	double lowerLimit;
};


// Electronic Control Assembly

class ECA {
	
public:
	ECA();
	void Init(Saturn *vessel);										// Initialization
	void TimeStep(double simdt);                                    // Timestep
	void SystemTimestep(double simdt);
	bool IsDCPowered();
	bool IsAC1Powered();
	bool IsAC2Powered();

	double GetPitchMTVCPosition() { return pitchMTVCPosition; }
	double GetYawMTVCPosition() { return yawMTVCPosition; }
	double GetPitchAutoTVCPosition() { return pitchAutoTVCPosition; }
	double GetYawAutoTVCPosition() { return yawAutoTVCPosition; }

	long thc_x,thc_y,thc_z;											// THC position
	int accel_roll_trigger;                                         // Joystick triggered roll thrust in RATE CMD mode
	int mnimp_roll_trigger;                                         // Joystick triggered roll thrust in MIN IMP mode
	int accel_pitch_trigger;                                        // Joystick triggered pitch thrust in RATE CMD mode
	int mnimp_pitch_trigger;                                        // Joystick triggered pitch thrust in MIN IMP mode
	int accel_yaw_trigger;                                          // Joystick triggered yaw thrust in RATE CMD mode
	int mnimp_yaw_trigger;                                          // Joystick triggered yaw thrust in MIN IMP mode
	Saturn *sat;
	VECTOR3 pseudorate;
protected:

	ECAIntegrator PitchMTVCIntegrator;
	ECAIntegrator YawMTVCIntegrator;
	ECAIntegrator PitchAutoTVCIntegrator;
	ECAIntegrator YawAutoTVCIntegrator;

	void ResetRelays();
	void ResetTransistors();

	double pitchMTVCPosition;
	double yawMTVCPosition;
	double pitchAutoTVCPosition;
	double yawAutoTVCPosition;

	//RELAYS

	//Roll Min Deadband Enable
	bool R1K22;
	//Roll High Rate Enable
	bool R1K25;

	//Yaw TVC Integrator Enable
	bool R2K11;
	//Yaw Min Deadband Enable
	bool R2K22;
	//Yaw High Rate Enable
	bool R2K25;
	//MTVC Yaw Rate 2 Enable
	bool R2K30;
	//MTVC Yaw Rate 1 Enable
	bool R2K31;

	//Pitch TVC Integrator Enable
	bool R3K11;
	//Pitch Min Deadband Enable
	bool R3K22;
	//Pitch High Rate Enable
	bool R3K25;
	//MTVC Pitch Rate 2 Enable
	bool R3K30;
	//MTVC Pitch Rate 1 Enable
	bool R3K31;

	//TRANSISTORS

	//Roll Attitude Signal No. 1 Enable
	bool T1QS21;
	//Roll High Rate Enable
	bool T1QS25;
	//Cross Coupling Enable
	bool T1QS26;
	//Roll Rate 1 Enable
	bool T1QS28;
	//Roll Rate 2 Enable
	bool T1QS29;
	//Roll RJC Rotational Control Proportional Enable
	bool T1QS32;
	//Roll Minimum Impulse or Accel Cmd Enable
	bool T1QS43;
	//Roll Pseudo Rate Disable
	bool T1QS44;

	//Yaw TVC Manual Cmd Enable
	bool T2QS1;
	//Yaw LM Off Gain Enable
	bool T2QS11;
	//Yaw LM On Gain Enable
	bool T2QS12;
	//Yaw Attitude Signal No. 1 Enable
	bool T2QS21;
	//Yaw High Rate Enable
	bool T2QS25;
	//Yaw Rate 1 Enable
	bool T2QS28;
	//Yaw Rate 2 Enable
	bool T2QS29;
	//Yaw MTVC Integrator Enable
	bool T2QS30;
	//Yaw RJC Prop Feedback Disable
	bool T2QS31;
	//Yaw RJC Rotational Control Proportional Enable
	bool T2QS32;
	//Yaw Minimum Impulse or Accel Cmd Enable
	bool T2QS43;
	//Yaw Pseudo Rate Disable
	bool T2QS44;

	//Pitch TVC Manual Cmd Enable
	bool T3QS1;
	//Pitch LM Off Gain Enable
	bool T3QS11;
	//Pitch LM On Gain Enable
	bool T3QS12;
	//Pitch Attitude Signal No. 1 Enable
	bool T3QS21;
	//Pitch High Rate Enable
	bool T3QS25;
	//Pitch Rate 1 Enable
	bool T3QS28;
	//Pitch Rate 2 Enable
	bool T3QS29;
	//Pitch MTVC Integrator Enable
	bool T3QS30;
	//Pitch RJC Prop Feedback Disable
	bool T3QS31;
	//Pitch RJC Rotational Control Proportional Enable
	bool T3QS32;
	//Pitch Minimum Impulse or Accel Cmd Enable
	bool T3QS43;
	//Pitch Pseudo Rate Disable
	bool T3QS44;
};

class ServoAmplifierModule
{
public:
	ServoAmplifierModule(bool *r1, bool *r2, bool *r3, bool *r4);
	void Init(Saturn *vessel);
	void SystemTimestep(double simdt);

	bool IsClutch1Powered();
	bool IsClutch2Powered();
protected:

	void DrawSystem1Power();
	void DrawSystem2Power();

	Saturn *sat;
	bool *relays[4];
};

//Thrust Vector Servo Amplifier Assembly

#define TVSA_START_STRING	"TVSA_BEGIN"
#define TVSA_END_STRING		"TVSA_END"

class TVSA
{
public:
	TVSA();
	void Init(Saturn *vessel);										// Initialization
	void TimeStep(double simdt);                                    // Timestep
	void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	void ChangeCMCPitchPosition(double delta);
	void ChangeCMCYawPosition(double delta);
	void ZeroCMCPosition() { cmcPitchPosition = 0; cmcYawPosition = 0; }
	void EnableCMCTVCErrorCounter() { cmcErrorCountersEnabled = true; }
	void DisableCMCTVCErrorCounter() { cmcErrorCountersEnabled = false; }
	bool IsCMCErrorCountersEnabled() { return cmcErrorCountersEnabled; }
	double GetPitchGimbalTrim();
	double GetYawGimbalTrim();
	double GetPitchGimbalPosition();
	double GetYawGimbalPosition();

	ServoAmplifierModule *GetPitchServoAmp() { return &pitchServoAmp; }
	ServoAmplifierModule *GetYawServoAmp() { return &yawServoAmp; }
protected:

	bool IsSystem1ACPowered();
	bool IsSystem2ACPowered();
	void DrawSystem1ACPower();
	void DrawSystem2ACPower();

	ServoAmplifierModule pitchServoAmp;
	ServoAmplifierModule yawServoAmp;

	bool cmcErrorCountersEnabled;
	double cmcPitchPosition;
	double cmcYawPosition;
	double pitchGimbalTrim1;
	double pitchGimbalTrim2;
	double yawGimbalTrim1;
	double yawGimbalTrim2;
	double pitchGimbalPosition1;
	double pitchGimbalPosition2;
	double yawGimbalPosition1;
	double yawGimbalPosition2;
	Saturn *sat;

	//RELAYS

	//Pitch Servo No. 2 Engage
	bool A4K1;
	//Pitch Servo No. 2 Engage
	bool A4K2;
	//Pitch Servo No. 2 Engage
	bool A4K3;
	//Yaw Servo No. 2 Engage
	bool A4K4;
	//Yaw Servo No. 2 Engage
	bool A4K5;
	//Yaw Servo No. 2 Engage
	bool A4K6;
	//Pitch Servo No. 2 Engage
	bool A4K7;
	//Yaw Servo No. 2 Engage
	bool A4K8;

	//TRANSISTORS

	//Yaw SCS Gimbal Cmd Enable
	bool T2QS2;
	//Yaw TVC Gimbal Trim Enable
	bool T2QS3;

	//Pitch SCS Gimbal Cmd Enable
	bool T3QS2;
	//Pitch TVC Gimbal Trim Enable
	bool T3QS3;
};


// EMS

#define EMS_START_STRING	"EMS_BEGIN"
#define EMS_END_STRING		"EMS_END"
#define EMS_SCROLL_LENGTH_PX    2500   //Pixel length of bitmap
#define EMS_SCROLL_LENGTH_PY    145   //Pixel height of bitmap
#define EMS_RSI_CENTER_X        42     //Pixel center on bitmap
#define EMS_RSI_CENTER_Y        41     //Pixel center on bitmap

class EMS : public e_object {

public:
	EMS(PanelSDK &p);
	void Init(Saturn *vessel, e_object *a, e_object *b, RotationalSwitch *dimmer, e_object *c);
	void TimeStep(double MissionTime, double simdt);
	void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn);                                // SaveState callback
	void LoadState(FILEHANDLE scn);                                // LoadState callback

	double GetdVRangeCounter() { return dVRangeCounter; };
	POINT ScribePntArray[EMS_SCROLL_LENGTH_PX*3]; //Thrice the number of pixels in the scrolling direction.
	POINT RSITriangle[3];
	void SetRSIDeltaRotation(double dangle);
	int ScribePntCnt;
	int GetScrollOffset() { return ScribePntArray[ScribePntCnt-1].x-40; };
	int GetGScribe() { return GScribe; };
	void SwitchChanged();
	bool SPSThrustLight();
	bool pt05GLight();
	int LiftVectLight();
	bool IsOff();
	bool IsdVMode();
	bool WriteScrollToFile();
	bool IsDecimalPointBlanked();
	
protected:
	bool IsPowered();
	bool IsDisplayPowered();
	
	void AccelerometerTimeStep(double simdt);
	VECTOR3 GetGravityVector();
	double xacc, xaccG, constG;
	double vinert;

	int status;
	int SlewScribe; //pixels
	int GScribe; //pixels
	double ScrollPosition; //fractional pixels
	double MaxScrollPosition;
	bool dVInitialized;
	VECTOR3 lastWeight;
	VECTOR3 lastGlobalVel;
	double lastSimDT;
	double dVRangeCounter;
	double dVTestTime;

	bool switchchangereset;

	void RotateRSI(double simdt);
	double RSIRotation; // Current angle in radians.
	double RSITarget;   // Target angle in radians.

	bool pt05GLightOn;
	bool pt05GFailed;

	//Comparator Circuits
	bool pt05GComparator(double simdt);
	bool pt02GComparator(double simdt);
	bool Manual05GInit();
	bool InitialTrip;
	double OneSecTimer;
	short int VerifyCorridor();

	short int LiftVectLightOn; //1 is up light, -1 is down light

	//Threshold Circuits
	double TenSecTimer;
	bool ThresholdBreeched; // .05G Comparator has been tripped.
	double ThresholdBreechTime; // MissionTime that .05 Comparator is tripped
	bool CorridorEvaluated;

	double ScrollBitmapLength;
	double ScrollBitmapHeight;
	double ScrollScaling;

	PowerMerge DCPower;
	Saturn *sat;
	RotationalSwitch *DimmerRotationalSwitch;

	friend class SaturnEMSDvDisplay;
	friend class SaturnEMSScrollDisplay;
};

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp);
bool CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);
