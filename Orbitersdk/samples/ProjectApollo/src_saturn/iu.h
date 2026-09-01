/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007

  Saturn Instrument Unit header

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

#if !defined(_PA_IU_H)
#define _PA_IU_H

#include "connector.h"
#include "LVIMU.h"
#include "LVDC.h"
#include "FCC.h"
#include "eds.h"
#include "LVDA.h"
#include "dcs.h"
#include "DelayTimer.h"
#include "IUControlDistributor.h"
#include "IUAuxiliaryPowerDistributor.h"
#include "IUControlSignalProcessor.h"

class SoundLib;
class IU;

///
/// \ingroup Connectors
/// \brief Message type to send to the CSM.
///
enum IUCSMMessageType
{
	IUCSM_SET_INPUT_CHANNEL_BIT,			///< Set an AGC input channel bit value.
	IUCSM_SET_SII_SEP_LIGHT,				///< Light or clear SII Sep light.
	IUCSM_SET_LV_RATE_LIGHT,
	IUCSM_SET_LV_GUID_LIGHT,
	IUCSM_SET_ENGINE_INDICATOR,				///< Set or clear an engine indicator.
	IUCSM_SET_ENGINE_INDICATORS,
	IUCSM_GET_ENGINE_INDICATOR,
	IUCSM_GET_TLI_INHIBIT,					///< Get TLI inhibit signal.
	IUCSM_GET_IU_UPTLM_ACCEPT,				///< Get IU UPTLM switch setting.
	IUCSM_GET_SIISIVB_DIRECT_STAGING,		///< Get S-II/S-IVB direct staging signal.
	IUCSM_GET_CMC_SIVB_TAKEOVER,			///< Get CMC S-IVB takeover signal.
	IUCSM_GET_CMC_SIVB_IGNITION,			///< Get CMC S-IVB ignition sequence start signal.
	IUCSM_GET_CMC_SIVB_CUTOFF,				///< Get CMC S-IVB cutoff signal.
	IUCSM_GET_LV_RATE_AUTO_SWITCH_STATE,
	IUCSM_GET_TWO_ENGINE_OUT_AUTO_SWITCH_STATE,
	IUCSM_GET_BECO_COMMAND,					///< Get Boost Engine Cutoff command from SECS.
	IUCSM_IS_EDS_BUS_POWERED,
	IUCSM_GET_AGC_ATTITUDE_ERROR,
	IUCSM_TLI_BEGUN,						///< Indicate for the event manager that the TLI burn has occured
	IUCSM_TLI_ENDED,						///< Indicate for the event manager that the TLI burn has ended
	IUCSM_IS_EDS_UNSAFE_A,
	IUCSM_IS_EDS_UNSAFE_B,

	CSMIU_SET_VESSEL_STATS,					///< Set the vessel stats in the IU.
	CSMIU_START_TLI_BURN,					///< Start the TLI burn.
	CSMIU_GET_LIFTOFF_CIRCUIT,
	CSMIU_GET_EDS_ABORT,					///< Set EDS abort signal.
	CSMIU_GET_LV_TANK_PRESSURE,
	CSMIU_GET_ABORT_LIGHT_SIGNAL,
	CSMIU_GET_QBALL_POWER,
	CSMIU_GET_QBALL_SIMULATE_CMD
};

///
/// \ingroup Connectors
/// \brief Message type to send to the launch vehicle.
///
enum IULVMessageType
{
	IULV_SEPARATE_STAGE,
	IULV_SET_STAGE,
	IULV_NOSECAP_JETTISON,
	IULV_DEPLOY_SLA_PANEL,

	IULV_GET_STAGE,							///< Get mission stage.
	IULV_GET_RELATIVE_POS,					///< Get relative position.
	IULV_GET_RELATIVE_VEL,					///< Get relative velocity.
	IULV_GET_INERTIAL_ACCEL,				///< Get inertial acceleration
	IULV_GET_ROTATIONMATRIX,				///< Get rotation matrix
	IULV_GET_ANGULARVEL,					///< Get angular velocity
	IULV_CSM_SEPARATION_SENSED,
	IULV_GET_VEHICLENO
};

enum IUSIVBMessageType
{
	IUSIVB_SET_APS_ATTITUDE_ENGINE,
	IUSIVB_SI_EDS_CUTOFF,
	IUSIVB_SII_EDS_CUTOFF,
	IUSIVB_SIVB_EDS_CUTOFF,
	IUSIVB_SET_SI_THRUSTER_DIR,				///< Set thruster direction.
	IUSIVB_SET_SII_THRUSTER_DIR,
	IUSIVB_SET_SIVB_THRUSTER_DIR,
	IUSIVB_SI_SWITCH_SELECTOR,
	IUSIVB_SII_SWITCH_SELECTOR,
	IUSIVB_SIVB_SWITCH_SELECTOR,
	IUSIVB_GET_SI_THRUST_OK,
	IUSIVB_GET_SII_THRUST_OK,
	IUSIVB_GET_SIVB_THRUST_OK,
	IUSIVB_GET_SI_PROPELLANT_DEPLETION_ENGINE_CUTOFF,
	IUSIVB_GET_SII_PROPELLANT_DEPLETION_ENGINE_CUTOFF,
	IUSIVB_GET_SIB_LOW_LEVEL_SENSORS_DRY,
	IUSIVB_GET_SI_INBOARD_ENGINE_OUT,
	IUSIVB_GET_SI_OUTBOARD_ENGINE_OUT,
	IUSIVB_GET_SII_FUEL_TANK_PRESSURE,
	IUSIVB_GET_SIVB_FUEL_TANK_PRESSURE,
	IUSIVB_GET_SIVB_LOX_TANK_PRESSURE,
	IUSIVB_GET_SIX_SIVB_NOT_SEPARATED,
	IUSIVB_GET_SIC_SII_NOT_SEPARATED,
	IUSIVB_GET_SII_INTERSTAGE_NOT_SEPARATED,
};

//IU connector class

class IUConnector : public Connector
{
public:
	IUConnector();
	virtual ~IUConnector();

	void SetIU(IU *iu) { ourIU = iu; };
protected:
	IU *ourIU;
};

///
/// \ingroup Connectors
/// \brief IU to CSM command connector.
///
class IUToCSMCommandConnector : public IUConnector
{
public:
	IUToCSMCommandConnector();
	~IUToCSMCommandConnector();

	void SetAGCInputChannelBit(int channel, int bit, bool val);
	void SetSIISep();
	void ClearSIISep();
	void SetEngineIndicator(int eng);
	void ClearEngineIndicator(int eng);
	void ClearEngineIndicators();
	void SetLVRateLight();
	void ClearLVRateLight();
	void SetLVGuidLight();
	void ClearLVGuidLight();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);

	bool GetCMCSIVBTakeover();
	bool GetCMCSIVBIgnitionSequenceStart();
	bool GetCMCSIVBCutoff();
	bool GetSIISIVbDirectStagingSignal();
	bool GetTLIInhibitSignal();
	bool GetIUUPTLMAccept();
	bool IsEDSUnsafeA();
	bool IsEDSUnsafeB();

	bool GetEngineIndicator(int eng);
	int LVRateAutoSwitchState();
	int TwoEngineOutAutoSwitchState();
	bool GetBECOCommand(bool IsSysA);
	int GetAGCAttitudeError(int axis);
	bool IsEDSBusPowered(int eds);

	void TLIBegun();
	void TLIEnded();	

};

// IU to S-IVB command connector (NEW)
class IUToSIVBCommandConnector : public IUConnector
{
public:
	IUToSIVBCommandConnector();
	~IUToSIVBCommandConnector();

	void SetAPSAttitudeEngine(int n, bool on);
	void SIEDSCutoff(bool cut);
	void SIIEDSCutoff(bool cut);
	void SIVBEDSCutoff(bool cut);
	void SetSIThrusterDir(int n, double yaw, double pitch);
	void SetSIIThrusterDir(int n, double yaw, double pitch);
	void SetSIVBThrusterDir(double yaw, double pitch);
	void SISwitchSelector(int channel);
	void SIISwitchSelector(int channel);
	void SIVBSwitchSelector(int channel);
	void GetSIThrustOK(bool *ok, int n);
	void GetSIIThrustOK(bool *ok);
	bool GetSIVBThrustOK();
	bool GetSIPropellantDepletionEngineCutoff();
	bool GetSIIPropellantDepletionEngineCutoff();
	bool GetSIBLowLevelSensorsDry();
	bool GetSIInboardEngineOut();
	bool GetSIOutboardEngineOut();
	double GetSIIFuelTankPressurePSI();
	double GetSIVBLOXTankPressurePSI();
	double GetSIVBFuelTankPressurePSI();
	bool SIXSIVBNotSeparated();
	bool SICSIINotSeparated();
	bool SIIInterstageNotSeparated();
};

///
/// \ingroup Connectors
/// \brief IU to LV command connector. (OLD)
///
class IUToLVCommandConnector : public Connector
{
public:
	IUToLVCommandConnector();
	~IUToLVCommandConnector();

	void SeparateStage(int stage);
	void SetStage(int stage);
	void JettisonNosecap();
	void DeploySLAPanel();

	int GetStage();
	void GetInertialAccel(VECTOR3 &a);
	void GetRotationMatrix(MATRIX3 &rot);
	void GetAngularVel(VECTOR3 &avel);
	int GetVehicleNo();

	void GetRelativePos(OBJHANDLE ref, VECTOR3 &v);
	void GetRelativeVel(OBJHANDLE ref, VECTOR3 &v);

	bool CSMSeparationSensed();
};

//IU to IU ESE command connector
class IUToIUESECommandConnector : public IUConnector
{
public:
	IUToIUESECommandConnector();
	~IUToIUESECommandConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);

	bool IsUmbilicalConnected();

	bool GetCommandVehicleLiftoffIndicationInhibit();
	bool GetExcessiveRollRateAutoAbortInhibit(int n);
	bool GetExcessivePitchYawRateAutoAbortInhibit(int n);
	bool GetTwoEngineOutAutoAbortInhibit(int n);
	bool GetGSEOverrateSimulate(int n);
	bool GetEDSPowerInhibit();
	bool PadAbortRequest();
	bool GetEngineThrustIndicationEnableInhibitA();
	bool GetEngineThrustIndicationEnableInhibitB();
	bool EDSLiftoffInhibitA();
	bool EDSLiftoffInhibitB();
	bool GetSIBurnModeSubstitute();
	bool GetGuidanceReferenceRelease();
	bool GetQBallSimulateCmd();
	bool GetEDSAutoAbortSimulate(int n);
	bool GetEDSLVCutoffSimulate(int n);
	bool GetSICOutboardEnginesCantInhibit();
	bool GetSICOutboardEnginesCantSimulate();
};

///
/// This class simulates the Saturn Instrument Unit, which flew the Saturn launch vehicle
/// prior to the CSM seperating from the SIVb.
///
/// \ingroup LVSystems
/// \brief Saturn IU simulation.
///
class IU {

public:
	IU();
	virtual ~IU();

	void SetMissionInfo(bool crewed, bool sccontpowered);

	virtual void ConnectToCSM(Connector *csmConnector);
	virtual void ConnectToLV(Connector *CommandConnector);
	virtual void ConnectToSIVB(Connector *CommandConnector);

	void DisconnectIU();

	///
	/// \brief Timestep function.
	/// \param simt The current Mission Elapsed Time in seconds from launch.
	/// \param simdt The time in seconds since the last timestep call.
	///
	virtual void Timestep(double simt, double simdt, double mjd);
	void LVDCTimestep(double simt, double simdt);
	virtual void SwitchSelector(int item) = 0;
	void PostStep(double simt, double simdt, double mjd);

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

	void SaveLVDC(FILEHANDLE scn);
	virtual void LoadLVDC(FILEHANDLE scn) = 0;

	bool GetSIPropellantDepletionEngineCutoff();
	virtual bool SIBLowLevelSensorsDry();
	virtual bool GetSIIPropellantDepletionEngineCutoff();
	bool GetSIInboardEngineOut();
	bool GetSIOutboardEngineOut();
	virtual bool GetSIIInboardEngineOut() { return false; }
	virtual bool GetSIIOutboardEngineOut() { return false; }
	virtual bool GetSIIEnginesOut();
	bool GetSCControlPoweredFlight() { return SCControlPoweredFlight; }
	VECTOR3 GetTheodoliteAlignment(double azimuth);

	virtual bool DCSUplink(int type, void *upl);

	IUToCSMCommandConnector* GetCommandConnector() { return &commandConnector; }
	IUToLVCommandConnector* GetLVCommandConnector() { return &lvCommandConnector; }
	IUToSIVBCommandConnector* GetSIVBCommandConnector() { return &sivbCommandConnector; }
	IUToIUESECommandConnector* GetIUToIUESECommandConnector() { return &iuToIUESECommandConnector; }

	//Subsystem Access
	virtual EDS* GetEDS() = 0;
	virtual FCC* GetFCC() = 0;
	virtual IUControlDistributor *GetControlDistributor() = 0;
	virtual DelayTimer *GetEngineCutoffEnableTimer() = 0;
	virtual LVDC* GetLVDC() = 0;
	LVRG* GetLVRG() { return &lvrg; }
	LVDA* GetLVDA() { return &lvda; }
	LVIMU* GetLVIMU() { return &lvimu; }
	DCS* GetDCS() { return &dcs; }
	IUAuxiliaryPowerDistributor2 *GetAuxPowrDistr() { return &AuxiliaryPowerDistributor2; }
	IUControlSignalProcessor *GetContSigProc() { return &ControlSignalProcessor; }

protected:

	int State;

	//
	// Saturn stuff
	//

	bool Crewed;
	bool SCControlPoweredFlight;

	//Time keeping for LVDC
	double LastCycled;

	///
	/// \brief Connector to CSM.
	///
	IUToCSMCommandConnector commandConnector;

	///
	/// \brief Connector to launch vehicle.
	///
	IUToLVCommandConnector lvCommandConnector;

	//Connector to S-IVB
	IUToSIVBCommandConnector sivbCommandConnector;
	//Connector to IU UESE
	IUToIUESECommandConnector iuToIUESECommandConnector;

	//Subsystems:

	//603A29
	LVDA lvda;
	//603A19
	LVIMU lvimu;
	//602A23
	LVRG lvrg;
	DCS dcs;
	//601A33
	IUAuxiliaryPowerDistributor1 AuxiliaryPowerDistributor1;
	//602A34
	IUAuxiliaryPowerDistributor2 AuxiliaryPowerDistributor2;
	//601A24
	IUControlSignalProcessor ControlSignalProcessor;
};

class IU1B :public IU
{
public:
	IU1B();
	~IU1B();
	void Timestep(double simt, double simdt, double mjd);
	bool SIBLowLevelSensorsDry();
	void SwitchSelector(int item);
	void LoadLVDC(FILEHANDLE scn);

	//Subsystem access
	FCC* GetFCC() { return &fcc; }
	EDS* GetEDS() { return &eds; }
	IUControlDistributor *GetControlDistributor() { return &ControlDistributor; }
	DelayTimer *GetEngineCutoffEnableTimer() { return &EngineCutoffEnableTimer; }
	LVDC* GetLVDC() { return &lvdc; }

protected:
	//603A28
	LVDC1B lvdc;
	//602A27
	FCC1B fcc;
	//602A5
	EDS1B eds;
	IUControlDistributor1B ControlDistributor;
	//603A36
	DelayTimer EngineCutoffEnableTimer;
};

class IUSV :public IU
{
public:
	IUSV();
	~IUSV();
	void Timestep(double simt, double simdt, double mjd);
	bool GetSIIPropellantDepletionEngineCutoff();
	bool GetSIIEnginesOut();
	bool GetSIIInboardEngineOut();
	bool GetSIIOutboardEngineOut();
	void SwitchSelector(int item);
	void LoadLVDC(FILEHANDLE scn);

	//Subsystem access
	FCC* GetFCC() { return &fcc; }
	EDS* GetEDS() { return &eds; }
	IUControlDistributor *GetControlDistributor() { return &ControlDistributor; }
	DelayTimer *GetEngineCutoffEnableTimer() { return &EngineCutoffEnableTimer; }
	LVDC* GetLVDC() { return &lvdc; }

protected:
	//603A28
	LVDCSV lvdc;
	//602A27
	FCCSV fcc;
	//602A5
	EDSSV eds;
	//603A2
	IUControlDistributorSV ControlDistributor;
	//603A36
	DelayTimer EngineCutoffEnableTimer;
};

//
// Strings for state saving.
//

#define IU_START_STRING		"IU_BEGIN"
#define IU_END_STRING		"IU_END"

#endif
