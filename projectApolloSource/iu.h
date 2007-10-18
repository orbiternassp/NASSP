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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.8  2006/07/09 16:09:38  movieman523
  *	Added Prog 59 for SIVb venting.
  *	
  *	Revision 1.7  2006/07/09 00:07:07  movieman523
  *	Initial tidy-up of connector code.
  *	
  *	Revision 1.6  2006/07/07 19:44:58  movieman523
  *	First version of connector support.
  *	
  *	Revision 1.5  2006/06/25 21:19:45  movieman523
  *	Lots of Doxygen updates.
  *	
  *	Revision 1.4  2006/06/23 11:56:48  tschachim
  *	New Project Apollo MFD for TLI burn control.
  *	
  *	Revision 1.3  2006/04/25 13:39:15  tschachim
  *	Removed GetXXXSwitchState.
  *	
  *	Revision 1.2  2006/02/21 12:01:47  tschachim
  *	Moved TLI sequence to the IU.
  *	
  **************************************************************************/

#if !defined(_PA_IU_H)
#define _PA_IU_H

class SoundLib;
class IU;

///
/// \ingroup Connectors
/// \brief Message type to send to the CSM.
///
enum IUCSMMessageType
{
	IUCSM_SET_OUTPUT_CHANNEL,				///< Set an AGC output channel value.
	IUCSM_IS_VIRTUAL_AGC,					///< Check whether the CSM is running Virtual AGC.
	IUCSM_SET_SII_SEP_LIGHT,				///< Light or clear SII Sep light.
	IUCSM_SLOW_IF_DESIRED,					///< Slow if desired.
	IUCSM_SET_ENGINE_INDICATOR,				///< Set or clear an engine indicator.
	IUCSM_GET_SIISIVBSEP_SWITCH_STATE,		///< State of SII/SIVb Sep switch.
	IUCSM_GET_TLI_ENABLE_SWITCH_STATE,		///< State of TLI Enable switch.
	IUCSM_LOAD_TLI_SOUNDS,					///< Load sounds required for TLI burn.
	IUCSM_PLAY_COUNT_SOUND,					///< Play/stop countdown sound.
	IUCSM_PLAY_SECO_SOUND,					///< Play/stop SECO sound.
	IUCSM_PLAY_SEPS_SOUND,					///< Play/stop Seperation sound.
	IUCSM_PLAY_TLI_SOUND,					///< Play/stop TLI sound.
	IUCSM_PLAY_TLISTART_SOUND,				///< Play/stop TLI start sound.
	IUCSM_CLEAR_TLI_SOUNDS,					///< Unload the sounds required for the TLI burn.

	CSMIU_SET_VESSEL_STATS,					///< Set the vessel stats in the IU.
	CSMIU_START_TLI_BURN,					///< Start the TLI burn.
	CSMIU_IS_TLI_CAPABLE,					///< Is the IU TLI capable?
	CSMIU_CHANNEL_OUTPUT,					///< Process AGC channel output.
	CSMIU_GET_VESSEL_STATS,					///< Get vessel ISP and thrust.
	CSMIU_GET_VESSEL_MASS,					///< Get vessel mass.
	CSMIU_GET_VESSEL_FUEL,					///< Get vessel fuel.
};

///
/// \ingroup Connectors
/// \brief Message type to send to the launch vehicle.
///
enum IULVMessageType
{
	IULV_ENABLE_J2,							///< Enable the J2 engine.
	IULV_SET_J2_THRUST_LEVEL,				///< Set the J2 thrust level.
	IULV_SET_APS_THRUST_LEVEL,				///< Set APS thrust level.
	IULV_DEACTIVATE_NAVMODE,				///< Deactivate a navmode.
	IULV_ACTIVATE_NAVMODE,					///< Activate a navmode.
	IULV_ACTIVATE_S4RCS,					///< Activate the SIVb RCS.
	IULV_DEACTIVATE_S4RCS,					///< Deactivate the SIVb RCS.
	IULV_SET_ATTITUDE_LIN_LEVEL,			///< Set thruster levels.
	IULV_J2_DONE,							///< J2 is now done, turn it into a vent.

	IULV_GET_STAGE,							///< Get mission stage.
	IULV_GET_STATUS,						///< Get vessel status.
	IULV_GET_J2_THRUST_LEVEL,				///< Get the J2 engine thrust level.
	IULV_GET_ALTITUDE,						///< Get the current altitude.
	IULV_GET_PROPELLANT_MASS,				///< Get the propellant mass.
	IULV_GET_MAX_FUEL_MASS,					///< Get max fuel mass.
	IULV_GET_MASS,							///< Get the spacecraft mass.
	IULV_GET_GRAVITY_REF,					///< Get gravity reference.
	IULV_GET_RELATIVE_POS,					///< Get relative position.
	IULV_GET_RELATIVE_VEL,					///< Get relative velocity.
	IULV_GET_AP_DIST,						///< Get Ap Dist.
	IULV_GET_ELEMENTS,						///< Get orbital elements.
};

///
/// \ingroup Connectors
/// \brief IU to CSM command connector.
///
class IUToCSMCommandConnector : public Connector
{
public:
	IUToCSMCommandConnector();
	~IUToCSMCommandConnector();

	void SetAGCOutputChannel(int channel, int val);
	void SetSIISep();
	void ClearSIISep();
	void SlowIfDesired();
	bool IsVirtualAGC();
	void SetEngineIndicator(int eng);
	void ClearEngineIndicator(int eng);

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);

	int SIISIVbSwitchState();
	int TLIEnableSwitchState();

	void SetIU(IU *iu) { ourIU = iu; };

	//
	// Sound functions.
	//

	void LoadTLISounds();
	void ClearTLISounds();

	void PlayCountSound(bool StartStop);
	void PlaySecoSound(bool StartStop);
	void PlaySepsSound(bool StartStop);
	void PlayTLISound(bool StartStop);
	void PlayTLIStartSound(bool StartStop);

protected:

	void PlayStopSound(IUCSMMessageType sound, bool StartStop);

	IU *ourIU;
};

///
/// \ingroup Connectors
/// \brief IU to LV command connector.
///
class IUToLVCommandConnector : public Connector
{
public:
	IUToLVCommandConnector();
	~IUToLVCommandConnector();

	void EnableDisableJ2(bool Enable);
	void SetJ2ThrustLevel(double thrust);
	void SetAPSThrustLevel(double thrust);
	void SetVentingThruster();

	void DeactivateNavmode(int mode);
	void ActivateNavmode(int mode);

	void DeactivateS4RCS();
	void ActivateS4RCS();

	void SetAttitudeLinLevel(int a1, int a2);

	int GetStage();
	double GetAltitude();
	double GetJ2ThrustLevel();
	double GetPropellantMass();
	double GetMass();
	double GetMaxFuelMass();
	void GetStatus(VESSELSTATUS &status);

	void GetApDist(double &d);

	void GetRelativePos(OBJHANDLE ref, VECTOR3 &v);
	void GetRelativeVel(OBJHANDLE ref, VECTOR3 &v);

	OBJHANDLE GetElements (ELEMENTS &el, double &mjd_ref);

	OBJHANDLE GetGravityRef();
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

	///
	/// \brief Timestep function.
	/// \param simt The current Mission Elapsed Time in seconds from launch.
	/// \param simdt The time in seconds since the last timestep call.
	///
	void Timestep(double simt, double simdt);
	void ChannelOutput(int address, int value);
	void SetVesselStats(double ISP, double Thrust);
	void GetVesselStats(double &ISP, double &Thrust);
	void SetMissionInfo(bool tlicapable, bool crewed, int realism, double sivbburnstart, double sivbapogee);
	bool IsTLICapable() { return TLICapable; };
	virtual bool StartTLIBurn(double timeToEjection, double dV);
	bool IsTLIInProgress() { return (TLIBurnState != 0); }
	double GetTLIBurnStartTime() { return TLIBurnStartTime; }
	double GetTLIBurnEndTime() { return TLIBurnEndTime; }

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

	///
	/// \brief Get vessel mass.
	/// \return The mass of the vessel we're controlling.
	///
	double GetMass();

	///
	/// \brief Get vessel fuel mass.
	/// \return The mass of fuel in the vessel we're controlling.
	///
	double GetFuelMass();

	virtual void ConnectToCSM(Connector *csmConnector);
	virtual void ConnectToMultiConnector(MultiConnector *csmConnector);
	virtual void ConnectToLV(Connector *CommandConnector);

protected:
	bool SIVBStart();
	void SIVBStop();
	void SIVBBoiloff();
	void TLIInhibit();
	bool DoTLICalcs();
	void UpdateTLICalcs();

	bool TLIBurnStart;
	bool TLIBurnDone;
	int State;
	double NextMissionEventTime;
	double LastMissionEventTime;
	bool FirstTimeStepDone;

	// TLI burn calculations
	int TLIBurnState;
	double TLIBurnTime;
	double TLIBurnStartTime;
	double TLIBurnEndTime;
	double TLIBurnDeltaV;
	double TLICutOffVel;
	double TLIThrustDecayDV;
	double TLILastAltitude;

	//
	// Saturn stuff
	//

	int Realism;
	bool Crewed;
	bool TLICapable;
	double VesselISP;
	double VesselThrust;

	//
	// SIVB burn info for unmanned flights.
	//

	bool SIVBBurn;
	double SIVBBurnStart;
	double SIVBApogee;

	///
	/// \brief Mission Elapsed Time, passed into the IU from the spacecraft.
	///
	double MissionTime;

	///
	/// \brief Connector to CSM.
	///
	IUToCSMCommandConnector commandConnector;

	///
	/// \brief Connector to launch vehicle.
	///
	IUToLVCommandConnector lvCommandConnector;
};

//
// Strings for state saving.
//

#define IU_START_STRING		"IU_BEGIN"
#define IU_END_STRING		"IU_END"

#endif
