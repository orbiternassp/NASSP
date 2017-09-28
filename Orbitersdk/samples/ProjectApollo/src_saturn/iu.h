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

class SoundLib;
class IU;

///
/// \ingroup Connectors
/// \brief Message type to send to the CSM.
///
enum IUCSMMessageType
{
	IUCSM_SET_OUTPUT_CHANNEL,				///< Set an AGC output channel value.
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
	IUCSM_TLI_BEGUN,						///< Indicate for the event manager that the TLI burn has occured
	IUCSM_TLI_ENDED,						///< Indicate for the event manager that the TLI burn has ended

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
	IULV_SET_ATTITUDE_ROT_LEVEL,			///< Set rotational thruster levels.
	IULV_ADD_FORCE,							///< Add force.
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
	IULV_GET_PMI,							///< Get PMI.
	IULV_GET_SIZE,							///< Get size.
	IULV_GET_MAXTHRUST,						///< Get max. thrust
	IULV_LOCAL2GLOBAL,						///< Local to global
	IULV_GET_WEIGHTVECTOR,					///< Get weight vector 
	IULV_GET_FORCEVECTOR,					///< Get force vector
	IULV_GET_ROTATIONMATRIX,				///< Get rotation matrix
	IULV_GET_GLOBAL_VEL,					///< Get global vel
	IULV_GET_PITCH,							///< Get local pitch
	IULV_GET_BANK,							///< Get local bank
	IULV_GET_SLIP_ANGLE,					///< Get local slip angle
	IULV_GET_ANGULARVEL,					///< Get angular velocity
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
	void TLIBegun();
	void TLIEnded();

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
	void SetAttitudeRotLevel (VECTOR3 th);
	void AddForce(VECTOR3 F, VECTOR3 r);

	int GetStage();
	double GetAltitude();
	double GetJ2ThrustLevel();
	double GetPropellantMass();
	double GetMass();
	double GetMaxFuelMass();
	void GetStatus(VESSELSTATUS &status);
	void GetPMI(VECTOR3 &pmi);
	double GetSize();
	double GetMaxThrust(ENGINETYPE eng);
	bool GetWeightVector(VECTOR3 &w);
	bool GetForceVector(VECTOR3 &f);
	void GetRotationMatrix(MATRIX3 &rot);
	void GetAngularVel(VECTOR3 &avel);

	void Local2Global(VECTOR3 &local, VECTOR3 &global);
	void GetApDist(double &d);

	void GetRelativePos(OBJHANDLE ref, VECTOR3 &v);
	void GetRelativeVel(OBJHANDLE ref, VECTOR3 &v);
	void GetGlobalVel(VECTOR3 &v);

	double GetPitch(void);
	double GetBank(void);
	double GetSlipAngle(void);

	OBJHANDLE GetElements(ELEMENTS &el, double &mjd_ref);
	OBJHANDLE GetGravityRef();
};

///
/// S-IVB IU GNC
///
/// \ingroup LVSystems
/// \brief S-IVB IU GNC.
///
class IUGNC {

public:
			 IUGNC();
			~IUGNC();

	void	Reset();

	void	PostStep(double sim_mjd, double dt);
	void	PreStep(double sim_mjd, double dt);

	void	Configure(IUToLVCommandConnector *lvc, int Ref);
	void	SetThrusterForce(double Force, double ISP, double TailOff);

	VECTOR3 Get_uTD();
	VECTOR3 Get_dV();
	double	Get_tGO();
	double	Get_IgnMJD() { return IgnMJD; }
	VECTOR3 Get_vG() { return _vG; }
	VECTOR3 Get_uTDInit() { return _uTDInit; }
	bool	IsEngineOn() { return engine; };
	
	bool	ActivateP30(VECTOR3 _rign, VECTOR3 _vign, VECTOR3 _dv, double IgnMJD);
	bool	ActivateP31(VECTOR3 _rign, VECTOR3 _vign, VECTOR3 _lap, double IgnMJD, double TgtMJD);

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

private:

	void    EngineOn();
	void    EngineOff();

	double	GetMass();
	VECTOR3 Lambert(VECTOR3 init, VECTOR3 rad, double time, double mu, double dm, VECTOR3 *tv=NULL);

	VECTOR3 create_vector(VECTOR3 normal, VECTOR3 zero, double angle);
	VECTOR3 GlobalToLV(VECTOR3 _in, VECTOR3 _pos, VECTOR3 _vel);
	VECTOR3 GlobalToP30_LVLH(VECTOR3 _in, VECTOR3 _r, VECTOR3 _v, double mass, double thrust);

	// Set by IU
	IUToLVCommandConnector *lvCommandConnector;
	double Thrust;
	double TailOff;
	double ISP;

	// Planet configuration
	int Ref;
	OBJHANDLE RefHandle;
	double RefMu;
	double Mass;

	// ** VECTORS **
	VECTOR3 _PIPA;
	VECTOR3 _uTD;
	VECTOR3 _vG;
	VECTOR3 _r1;
	VECTOR3 _v1;
	VECTOR3 _ri;
	VECTOR3 _vi;
	VECTOR3 _r2;
	VECTOR3 _lastWeight;
	VECTOR3 _uTDInit;

	//** FLAGS **
	bool ExtDV;
	bool ready;
	bool engine;

	// ** SCALARS **
	double tGO;
	double tBurn;
	double IgnMJD;
	double TInMJD;
	double tD;
	double CutMJD;
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
	
	void SetVesselStats(double ISP, double Thrust);
	void GetVesselStats(double &ISP, double &Thrust);
	void SetMissionInfo(bool tlicapable, bool crewed);

	///
	/// \brief Move to or hold attitude v in LVLH coordinates (by setting attitude thrusters, call each timestep)
	///
	void SetLVLHAttitude(VECTOR3 v);

	///
	/// \brief Hold attitude stored at first call, reset by SetLVLHAttitude (by setting attitude thrusters, call each timestep)
	///
	void HoldAttitude();

	///
	/// \brief Start TLI burn sequence or update burn data
	///
	virtual bool StartTLIBurn(VECTOR3 RIgn, VECTOR3 VIgn, VECTOR3 dV, double MJDIgn);

	///
	/// \brief Legacy support for Simple AGC P15, very unprecise and not recommended
	///
	void ChannelOutput(int address, int value);


	bool IsTLICapable() { return TLICapable; };
	bool IsTLIInProgress() { return (TLIBurnState != 0); }

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

	///
	/// \brief Timestep function.
	/// \param simt The current Mission Elapsed Time in seconds from launch.
	/// \param simdt The time in seconds since the last timestep call.
	///
	void Timestep(double simt, double simdt, double mjd);
	void PostStep(double simt, double simdt, double mjd);

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

protected:
	bool SIVBStart();
	void SIVBStop();
	void SIVBBoiloff();
	void TLIInhibit();
	VECTOR3 OrientAxis(VECTOR3 &vec, int axis, int ref, double gainFactor, VECTOR3 &vec2);

	int TLIBurnState;
	bool TLIBurnStart;
	bool TLIBurnDone;
	int State;
	double NextMissionEventTime;
	double LastMissionEventTime;
	bool FirstTimeStepDone;

	//
	// Saturn stuff
	//

	bool Crewed;
	bool TLICapable;
	double VesselISP;
	double VesselThrust;

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

	///
	/// \brief Guidance
	///
	IUGNC GNC;
	bool ExternalGNC;

	///
	/// \brief Attitude control
	///
	bool AttitudeHold;
	VECTOR3 AttitudeToHold;
	VECTOR3 AttitudeToHold2;
};

//
// Strings for state saving.
//

#define IU_START_STRING		"IU_BEGIN"
#define IU_END_STRING		"IU_END"

#define IUGNC_START_STRING	"IUGNC_BEGIN"
#define IUGNC_END_STRING	"IUGNC_END"

#endif
