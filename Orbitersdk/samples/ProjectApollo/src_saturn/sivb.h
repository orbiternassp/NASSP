/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: SIVb class

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

#if !defined(_PA_SIVB_H)
#define _PA_SIVB_H

#include "payload.h"

//
// Data structure passed from main vessel to SIVB to configure stage.
//

///
/// Flags structure indicating which of the SIVB settings are valid.
///
/// \brief SIVB settings flags.
/// \ingroup SepStageSettings
///
union SIVbSettingFlags
{
	struct {
		unsigned SIVB_SETTINGS_MASS:1; 			///< Mass settings are valid.
		unsigned SIVB_SETTINGS_FUEL:1;			///< Fuel mass settings are valid.
		unsigned SIVB_SETTINGS_GENERAL:1;		///< General settings (e.g. Mission Time) are valid.
		unsigned SIVB_SETTINGS_PAYLOAD:1;		///< Payload type settings are valid.
		unsigned SIVB_SETTINGS_ENGINES:1;		///< Engine settings are valid.
		unsigned SIVB_SETTINGS_PAYLOAD_INFO:1;	///< Detailed payload settings (e.g. LEM name/mass/PAD) are valid.
	};
	unsigned int word;						///< Set to zero to clear all flags.

	///
	/// \brief Constructor: clear all flags by default.
	///
	SIVbSettingFlags() { word = 0; };
};

///
/// Data structure passed from main vessel to SIVb to configure it after staging.
///
/// \brief SIVb setup structure.
/// \ingroup SepStageSettings
///
struct SIVBSettings 
{
	SIVbSettingFlags SettingsType;	///< Which settings are valid?

	int Payload;					///< Payload type.
	int VehicleNo;					///< Saturn vehicle number.

	double THRUST_VAC;				///< Vacuum thrust.
	double ISP_VAC;					///< Vacuum ISP.

	double MissionTime;				///< Current MET in seconds.
	double EmptyMass;				///< Empty mass in kg.
	double PayloadMass;				///< Payload mass in kg.
	double ApsFuelKg;				///< APS fuel in kg.
	double MainFuelKg;				///< Remaining fuel in kg.

	bool PanelsHinged;				///< Are SLA panels hinged?
	bool SaturnVStage;				///< Saturn V stage or Saturn 1b stage?
	bool LowRes;					///< Low-res meshes?

	double SLARotationLimit;		///< SLA rotation limit in degrees (usually 45.0).

	//
	// Payload settings.
	//

	double LMDescentFuelMassKg;		///< Mass of fuel in descent stage of LEM.
	double LMAscentFuelMassKg;		///< Mass of fuel in ascent stage of LEM.
	double LMDescentEmptyMassKg;	///< Empty mass of descent stage of LEM.
	double LMAscentEmptyMassKg;		///< Empty mass of ascent stage of LEM.
	char PayloadName[64];			///< Payload Name

	int LMPadCount;					///< Count of LM PAD data.
	unsigned int *LMPad;			///< LM PAD data.
	int AEAPadCount;				///< Count of AEA PAD data.
	unsigned int *AEAPad;			///< AEA PAD data.

	///
	/// LEM checklist file
	///
	char LEMCheck[100];

	SIVBSettings() { LMPad = 0; LMPadCount = 0; AEAPad = 0; AEAPadCount = 0; LEMCheck[0] = 0; };

	IU *iu_pointer;
};

class SIVB;

///
/// \ingroup Connectors
/// \brief SIVB class connector base class.
///
class SIVbConnector : public Connector
{
public:
	SIVbConnector();
	~SIVbConnector();

	void SetSIVb(SIVB *sat) { OurVessel = sat; };

protected:
	SIVB *OurVessel;
};

///
/// \ingroup Connectors
/// \brief SIVb to IU command connector type.
///
class SIVbToIUCommandConnector : public SIVbConnector
{
public:
	SIVbToIUCommandConnector();
	~SIVbToIUCommandConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

///
/// \ingroup Connectors
/// \brief CSM to SIVb command connector type.
///
class CSMToSIVBCommandConnector : public SIVbConnector
{
public:
	CSMToSIVBCommandConnector();
	~CSMToSIVBCommandConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);

	///
	/// \brief Tell CSM docking probe to ignore next docking event.
	///
	void SetIgnoreNextDockEvent();

	///
	/// \brief Tell CSM docking probe to ignore next n docking events.
	/// \param n Number of events to ignore.
	///
	void SetIgnoreNextDockEvents(int n);

	///
	/// The CSM knows what the payload should do. This is difficult to get right as some
	/// of the data relates to the CSM which docks with the payload, and other data should
	/// be set by the booster which launches the SIVB into space. For now we'll get all the
	/// data from the docked CSM and we can figure out a better solution later.
	///
	/// \brief Get payload settings from CSM.
	/// \param p Payload settings structure.
	/// \return True if we got valid settings, false if not.
	///
	bool GetPayloadSettings(PayloadSettings &p);
};

///
/// This code simulates the seperated SIVb stage. Basically it simulates thrust decay if there is any fuel 
/// left, fires any retro rockets to push it away from the Saturn and then sits around waiting to be deleted.
///
/// \brief SIVb stage simulation.
/// \ingroup SepStages
///
class SIVB : public ProjectApolloConnectorVessel {

public:

	///
	/// Specifies the main state of the SIVb
	///
	/// \brief SIVb state.
	/// \ingroup SepStageSettings
	///
	enum SIVbState
	{
		SIVB_STATE_SETUP = -1,				///< SII is waiting for setup call.
		SIVB_STATE_WAITING					///< SII is idle after motor burnout.
	};

	///
	/// \ingroup ScenarioState
	/// \brief Main SIVB state-saving structure.
	///
	typedef union {
		struct {
			unsigned PanelsHinged:1;
			unsigned PanelsOpened:1;
			unsigned SaturnVStage:1;
			unsigned LowRes:1;
			unsigned J2IsActive:1;
			unsigned FuelVenting:1;
			unsigned Payloaddatatransfer:1;
		};
		unsigned long word;
	} MainState;

	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	SIVB (OBJHANDLE hObj, int fmodel);
	virtual ~SIVB();

	///
	/// \brief Orbiter state saving function.
	/// \param scn Scenario file to save to.
	///
	void clbkSaveState (FILEHANDLE scn);

	///
	/// \brief Orbiter timestep function.
	/// \param simt Current simulation time, in seconds since Orbiter was started.
	/// \param simdt Time in seconds since last timestep.
	/// \param mjd Current MJD.
	///
	void clbkPreStep(double simt, double simdt, double mjd);

	void clbkPostStep(double simt, double simdt, double mjd);

	///
	/// \brief Orbiter state loading function.
	/// \param scn Scenario file to load from.
	/// \param status Pointer to current vessel status.
	///
	void clbkLoadStateEx (FILEHANDLE scn, void *status);

	///
	/// \brief Orbiter class configuration function.
	/// \param cfg File to load configuration defaults from.
	///
	void clbkSetClassCaps (FILEHANDLE cfg);

	///
	/// \brief Orbiter dock state function.
	///
	void clbkDockEvent(int dock, OBJHANDLE connected);

	///
	/// Pass settings from the main DLL to the jettisoned SIVb. This call must be virtual 
	/// so it can be called from other DLLs without building in the LES code.
	/// \brief Setup jettisoned SIVb.
	/// \param state SIVb state settings.
	///
	virtual void SetState(SIVBSettings &state);

	///
	/// \brief Set thrust level of the J2 engine.
	/// \param thrust Thrust level from 0.0 to 1.0.
	///
	void SetJ2ThrustLevel(double thrust);

	///
	/// \brief Get thrust level of the J2 engine.
	/// \return Thrust level from 0.0 to 1.0.
	///
	double GetJ2ThrustLevel();

	///
	/// \brief Enable or disable the J2 engine.
	/// \param Enable Enable if true, disable if false.
	///
	void EnableDisableJ2(bool Enable);

	///
	/// \brief Get mission time.
	/// \return Mission time in seconds since launch.
	///
	double GetMissionTime();

	THRUSTER_HANDLE GetMainThruster(int n) { return th_main[n]; }
	THGROUP_HANDLE GetMainThrusterGroup() { return thg_main; }
	void SetSIVBThrusterDir(VECTOR3 &dir);
	void SetAPSThrusterLevel(int n, double level) { SetThrusterLevel(th_att_rot[n], level); }
	void SetAPSUllageThrusterLevel(int n, double level);

	///
	/// \brief Get main propellant mass.
	/// \return Propellant mass in kg.
	///
	double GetSIVbPropellantMass();

	///
	/// \brief Get total mass, including docked vessels.
	/// \return Mass in kg.
	///
	double GetTotalMass();

	///
	/// \brief Set up engines as fuel venting thurster.
	///
	void SetVentingThruster();

	///
	/// \brief Set up active J2 engine.
	///
	void SetActiveJ2Thruster();

	///
	/// \brief Start venting.
	///
	void StartVenting();

	///
	/// \brief Stop venting.
	///
	void StopVenting();

	///
	/// \brief Is the SIVb venting fuel?
	/// \return True if venting.
	///
	bool IsVenting();

	///
	/// \brief Get main battery power.
	/// \return Power in joules.
	///
	double GetMainBatteryPower();

	///
	/// \brief Get main battery power drain.
	/// \return Power in joules.
	///
	double GetMainBatteryPowerDrain();

	///
	/// \brief Get main battery voltage.
	/// \return Voltage in volts.
	///
	double GetMainBatteryVoltage();

	///
	/// \brief Get main battery current.
	/// \return Current in amps.
	///
	double GetMainBatteryCurrent();

	///
	/// \brief Start payload separation.
	///
	void StartSeparationPyros();

	///
	/// \brief Stop payload separation.
	///
	void StopSeparationPyros();

protected:
	///
	/// PanelSDK functions as a interface between the
	/// actual System & Panel SDK and VESSEL class
	///
	/// Note that this must be defined early in the file, so it will be initialised
	/// before any other classes which rely on it at creation. Don't move it further
	/// down without good reason, or you're likely to crash!
	///
	/// \brief Panel SDK library.
	///
    PanelSDK Panelsdk;

	bool PanelSDKInitalised;

	///
	/// \brief Set SIVb state.
	///
	void SetS4b();

	///
	/// \brief Initialise SIVb state.
	///
	void InitS4b();

	///
	/// \brief Get the main state flags as a 32-bit value to save to the scenario file.
	/// \return 32-bit flags value.
	///
	int GetMainState();

	///
	/// \brief Set the main state flags from a 32-bit value loaded from the scenario file.
	/// \param s 32-bit flags value.
	///
	void SetMainState(int s);

	///
	/// \brief Get the spacecraft name based on the Apollo mission and vehicle number.
	/// \param s String to hold the spacecraft name.
	///
	void GetApolloName(char *s);
	void AddRCS_S4B();				///< Add RCS for SIVb control.
	void Boiloff();					///< Boil off some LOX/LH2 in orbit.

	bool PayloadIsDetachable();		///< Is the payload detachable?

	VECTOR3	mainExhaustPos;			///< Position of main thruster exhaust.

	int PayloadType;				///< Payload type.
	int MissionNo;					///< Apollo mission number.
	int VehicleNo;					///< Saturn vehicle number.
	SIVbState State;				///< Main stage state.

	double EmptyMass;				///< Empty mass in kg.
	double PayloadMass;				///< Payload mass in kg.
	double MainFuel;				///< Main fuel mass in kg.

	double MissionTime;				///< Current MET in seconds.
	double NextMissionEventTime;	///< Next event time for automated operation.
	double LastMissionEventTime;	///< Last event time.

	bool PanelsHinged;				///< SLA panels are hinged.
	bool PanelsOpened;				///< SLA Panels are open.
	bool SaturnVStage;				///< Stage from Saturn V.
	bool LowRes;					///< Using low-res meshes.
	bool J2IsActive;				///< Is the J2 active for burns?
	bool FuelVenting;				///< Is the SIVb venting fuel?

	double RotationLimit;			///< Panel rotation limit from 0.0 to 1.0 (1.0 = 180 degrees).
	double CurrentThrust;			///< Current thrust level (0.0 to 1.0).

	double THRUST_THIRD_VAC;		///< J2 engine thrust vacuum level in Newtons.
	double ISP_THIRD_VAC;			///< J2 engine ISP in vacuum.

	double LMDescentFuelMassKg;		///< Mass of fuel in descent stage of LEM.
	double LMAscentFuelMassKg;		///< Mass of fuel in ascent stage of LEM.
	double LMDescentEmptyMassKg;	///< Empty mass of descent stage of LEM.
	double LMAscentEmptyMassKg;		///< Empty mass of ascent stage of LEM.

	//
	// LM PAD
	//

	int LMPadCount;					///< Count of LM PAD values.
	unsigned int *LMPad;			///< LM PAD load data.

	int LMPadLoadCount;
	int LMPadValueCount;

	int AEAPadCount;				///< Count of AEA PAD values.
	unsigned int *AEAPad;			///< AEA PAD load data.

	int AEAPadLoadCount;
	int AEAPadValueCount;

	char PayloadName[64];			///< Name of payload, if appropriate.

	bool Payloaddatatransfer;		///< Have we transferred data to the payload?

	///
	/// LEM checklist file
	///
	char LEMCheck[100];

	OBJHANDLE hs4b1;
	OBJHANDLE hs4b2;
	OBJHANDLE hs4b3;
	OBJHANDLE hs4b4;

	ATTACHMENTHANDLE hattDROGUE;

	///
	/// \brief Instrument Unit.
	///
	IU* iu;

	///
	/// \brief Connector from SIVb to CSM when docked.
	///
	MultiConnector SIVBToCSMConnector;

	///
	/// \brief Command connector from SIVb to IU.
	///
	SIVbToIUCommandConnector IUCommandConnector;

	///
	/// \brief Command connector from CSM to SIVb.
	///
	CSMToSIVBCommandConnector csmCommandConnector;

	PowerDrainConnectorObject SIVBToCSMPowerDrain;
	PowerDrainConnector SIVBToCSMPowerConnector;

	///
	/// \brief Handle of docked vessel.
	///
	DOCKHANDLE hDock;

	Battery *MainBattery;

	THRUSTER_HANDLE th_att_rot[10], th_main[1], th_att_lin[2];                 // handles for APS engines
	THGROUP_HANDLE thg_aps, thg_main, thg_sep, thg_sepPanel;
	PROPELLANT_HANDLE ph_aps, ph_main;

	UINT panelAnim;
	UINT panelAnimPlusX;
	double panelProc;
	double panelProcPlusX;
	int panelTimestepCount;
	int panelMesh1SaturnV, panelMesh2SaturnV, panelMesh3SaturnV, panelMesh4SaturnV;
	int panelMesh1SaturnVLow, panelMesh2SaturnVLow, panelMesh3SaturnVLow, panelMesh4SaturnVLow;
	int panelMesh1Saturn1b, panelMesh2Saturn1b, panelMesh3Saturn1b, panelMesh4Saturn1b;
	int meshSivbSaturnV, meshSivbSaturnVLow, meshSivbSaturn1b, meshSivbSaturn1bLow;
	int meshASTP_A, meshASTP_B, meshCOASTarget_A, meshCOASTarget_B, meshCOASTarget_C;
	int meshLMPKD, meshApollo8LTA, meshLTA_2r, meshLM_1;

	void HideAllMeshes();
};

///
/// \ingroup Connectors
/// \brief Message type to send from the CSM to the SIVb.
///
enum CSMSIVBMessageType
{
	CSMSIVB_GET_VESSEL_FUEL,				///< Get vessel fuel.
	CSMSIVB_GET_MAIN_BATTERY_POWER,			///< Get the main battery power level.
	CSMSIVB_GET_MAIN_BATTERY_ELECTRICS,		///< Get the main battery voltage and current.
	CSMSIVB_IS_VENTABLE,					///< Is this a ventable vessel?
	CSMSIVB_IS_VENTING,						///< Is the vessel venting fuel?
	CSMSIVB_START_VENTING,					///< Start fuel venting.
	CSMSIVB_STOP_VENTING,					///< Stop fuel venting.
	CSMSIVB_START_SEPARATION,				///< Start charging separation pyros.
	CSMSIVB_STOP_SEPARATION,				///< Stop charging separation pyros.
	SIVBCSM_IGNORE_DOCK_EVENT,				///< CSM docking probe should ignore next docking event (for payload creation)
	SIVBCSM_IGNORE_DOCK_EVENTS,				///< CSM docking probe should ignore next docking events (for payload creation)
	SIVBCSM_GET_PAYLOAD_SETTINGS,			///< Get the payload settings information from the CSM.
};

#endif // _PA_SIVB_H
