/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Generic caution and warning system code.

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

#if !defined(_PA_CAUTIONWARNING_H)
#define _PA_CAUTIONWARNING_H

///
/// \ingroup InternalSystemsState
/// \brief CWS light test.
///
enum LightTestState
{
	CWS_TEST_LIGHTS_NONE = 0,		///< Disable light test.
	CWS_TEST_LIGHTS_LEFT = 1,		///< Test left-hand lights.
	CWS_TEST_LIGHTS_RIGHT = 2,		///< Test right-hand lights.
	CWS_TEST_LIGHTS_BOTH = 3,		///< Test both lights.
};

///
/// \ingroup InternalSystemsState
/// \brief CWS operation mode.
///
enum CWSOperationMode
{
	CWS_MODE_NORMAL = 0,			///< Normal Caution and Warning display mode.
	CWS_MODE_BOOST = 1,				///< Boost mode, which disables the left-hand Master Alarm light.
	CWS_MODE_ACK = 2				///< ACK mode, for use during sleep periods.
};

///
/// \ingroup InternalSystemsState
/// \brief CWS source.
///
enum CSWSource
{
	CWS_SOURCE_LEM = 0,				///< Display Caution and Warning alarms from the LEM.
	CWS_SOURCE_CSM = 0,				///< Display Caution and Warming alarms from the CSM.
	CWS_SOURCE_CM = 1,				///< Display Caution and Warning alarms from the CM.
};

///
/// \ingroup InternalSystemsState
/// \brief CWS power source.
///
enum CSWPowerSource
{
	CWS_POWER_SUPPLY_1 = 0,			///< Caution and Warning system is using power supply 1
	CWS_POWER_SUPPLY_2 = 1,			///< Caution and Warning system is using power supply 2
	CWS_POWER_NONE = 2,				///< Caution and Warning system is not powered.
};

///
/// \ingroup InternalSystemsState
/// \brief Master Alarm position to render.
///
enum CWSMasterAlarmPosition
{
	CWS_MASTERALARMPOSITION_NONE,
	CWS_MASTERALARMPOSITION_LEFT,	///< Render the left master alarm light.
	CWS_MASTERALARMPOSITION_RIGHT	///< Render the right master alarm light.
};

#include "powersource.h"

///
/// This is the base class for simulating the caution and warning system in the CSM and LEM.
/// \ingroup InternalSystems
/// \brief CWS simulation.
///
class CautionWarningSystem {

public:
	///
	/// \brief Constructor.
	/// \param mastersound Master Alarm sound.
	/// \param buttonsound Button click sound.
	/// \param p Panel SDK that we're connected to.
	///
	CautionWarningSystem(Sound &mastersound, Sound &buttonsound, PanelSDK &p);

	///
	/// \brief Destructor.
	///
	virtual ~CautionWarningSystem();

	///
	/// \brief Test caution and warning lights.
	/// \param state Which tset to perform.
	///
	virtual void LightTest(LightTestState state);

	///
	/// \brief Timestep update function.
	/// \param simt The current mission elapsed time in seconds.
	///
	virtual void TimeStep(double simt);

	///
	/// \brief Panel SDK timestep.
	/// \param simdt Time since last timestep in seconds.
	///
	virtual void SystemTimestep(double simdt); 

	///
	/// \brief Set the operating mode.
	/// \param mode Operating mode to set.
	///
	void SetMode(CWSOperationMode mode);

	///
	/// \brief Set the CWS source.
	/// \param source Set the source for the CWS warnings.
	///
	void SetSource(CSWSource source) { Source = source; };

	///
	/// \brief Set the power source for the CWS.
	/// \param bus The bus to use for power.
	///
	void SetPowerBus(CSWPowerSource bus);

	///
	/// \brief Turn the master alarm on or off.
	/// \param alarm True to enable alarm, false to disable it.
	///
	void SetMasterAlarm(bool alarm);

	///
	/// \brief Returns the master alarm state, on or off.
	///
	bool GetMasterAlarm() { return MasterAlarm; };

	///
	/// \brief Turn sounds on or off.
	/// \param sounds True to turn sounds on, false to disable them.
	///
	void SetPlaySounds(bool sounds) { PlaySounds = sounds; };

	///
	/// \brief Inhibit or de-inhibit the next Master Alarm.
	/// \param i True to inhibit next Master Alarm.
	///
	void SetInhibitNextMasterAlarm(bool i) { InhibitNextMasterAlarm = i; };

	///
	/// \brief Set vessel to monitor.
	/// \param v The vessel object to monitor.
	///
	void MonitorVessel(VESSEL *v) { OurVessel = v; };

	///
	/// Turn a light on or off. Lights 0-29 are on the left panel, lights
	/// 30-59 are on the right panel.
	///
	/// If the light wasn't on and is now being turned on, the Master Alarm
	/// will sound.
	///
	/// \brief Update light state.
	/// \param lightnum Light number to update.
	/// \param state True to turn light on, false to turn it off.
	///
	void SetLight(int lightnum, bool state);

	///
	/// \brief Force a light to fail, or fix it.
	/// \param lightnum Light number to update.
	/// \param failed True to make the light fail, false to fix it.
	///
	virtual void FailLight(int lightnum, bool failed);

	///
	/// \brief Check whether a light has failed.
	/// \param lightnum Light number to check.
	/// \return True if the light has failed.
	///
	bool IsFailed(int lightnum);

	///
	/// \brief Render the Master Alarm.
	/// \param surf Surface to render to.
	/// \param alarmLit Surface for lit alarm bitmap.
	/// \param position Which Master Alarm light we're rendering.
	///
	void RenderMasterAlarm(SURFHANDLE surf, SURFHANDLE alarmLit, SURFHANDLE border, CWSMasterAlarmPosition position);

	///
	/// \brief Render the lights.
	/// \param surf Surface to render to.
	/// \param lightsurf Surface with lit lights.
	/// \param leftpanel True when rendering left panel, false when rendering right panel.
	virtual void RenderLights(SURFHANDLE surf, SURFHANDLE lightsurf, bool leftpanel);

	///
	/// \brief Process a mouse click on the Master Alarm.
	/// \param event Mouse event.
	/// \return True if we processed the event.
	///
	bool CheckMasterAlarmMouseClick(int event);

	///
	/// \brief Save the CWS state to a scenario file.
	/// \param scn Scenario file to save to.
	///
	virtual void SaveState(FILEHANDLE scn);

	///
	/// \brief Load the CWS state from a scenario file.
	/// \param scn Scenario file to load from.
	///

	virtual void LoadState(FILEHANDLE scn) = 0;

	void LoadState(char *line);

	///
	/// \brief Is the CWS powered?
	/// \return True if we're connected to a suitable power source.
	///
	bool IsPowered();

	///
	/// \brief Are the CWS lights powered?
	/// \return True if the lights are connected to a suitable power source.
	///
	bool LightsPowered();

	///
	/// \brief Wire CWS to two buses.
	/// \param a Bus 1
	/// \param b Bus 2
	///
	void WireTo(e_object *a, e_object *b) { DCPower.WireToBuses(a, b); };

	///
	/// \brief Checklist helper function 
	///
	void PushMasterAlarm();

	///
	/// \brief What's the current light test state commanded by telemetry?
	///
	int UplinkTestState;

protected:

	///
	/// \brief What's the current light test state?
	///
	LightTestState TestState;

	///
	/// \brief Caution and Warning system operation mode.
	///
	CWSOperationMode Mode;

	///
	/// \brief Source for the CWS failure warnings.
	///
	CSWSource Source;

	///
	/// \brief Which power bus are we connected to?
	///
	CSWPowerSource PowerBus;

	///
	/// \brief Bit-field indicating which lights have failed on the left panel.
	///
	int LightsFailedLeft;

	///
	/// \brief Bit-field indicating which lights have failed on the right panel.
	///
	int LightsFailedRight;

	///
	/// \brief Are the sounds currently enabled?
	///
	bool PlaySounds;

	///
	/// \brief Is the Master Alarm light enabled?
	///
	bool MasterAlarmLightEnabled;

	///
	/// \brief Is the Master Alarm lit?
	///
	bool MasterAlarm;

	///
	/// \brief Is the user pressing the Master Alarm light?
	///
	bool MasterAlarmPressed;

	///
	/// \brief Should we inhibit the next master alarm?
	///
	bool InhibitNextMasterAlarm;

#define CWS_LIGHTS_PER_PANEL	30	///< Max number of lights on one panel. If more than 30 you'll need to change packing code.

	///
	/// Light states. You get 30 lights per panel, as that fits nicely into a 32-bit integer. In
	/// reality, neither CSM or LEM needs that many.
	///
	/// \brief Left-hand light states.
	///
	bool LeftLights[CWS_LIGHTS_PER_PANEL];

	///
	/// Light states. You get 30 lights per panel, as that fits nicely into a 32-bit integer. In
	/// reality, neither CSM or LEM needs that many.
	///
	/// \brief Right-hand light states.
	///
	bool RightLights[CWS_LIGHTS_PER_PANEL];

	//
	// These don't have to be saved.
	//

	///
	/// \brief Is the master alarm lit?
	///
	bool MasterAlarmLit;

	///
	/// \brief Sound to use for master alarm.
	///
	/// Note that we use a reference here, which must be set in the constructor and
	/// cannot be changed afterwards. This avoids having to use pointers in all the code
	/// which uses the sound.
	///
	Sound &MasterAlarmSound;

	///
	/// \brief Sound to use for button clicks.
	///
	/// Note that we use a reference here, which must be set in the constructor and
	/// cannot be changed afterwards. This avoids having to use pointers in all the code
	/// which uses the sound.
	///
	Sound &ButtonSound;

	///
	/// \brief The vessel we're attached to.
	///
	VESSEL *OurVessel;

	///
	/// \brief CWS power source.
	///
	PowerMerge DCPower;

	//
	// Helper functions.
	//

	///
	/// \brief Convert an array of light states to a 32-bit integer.
	/// This function converts an internal light state array into a 32-bit integer which can be
	/// saved in the scenario file.
	///
	/// \param LightState Array of light states.
	/// \return 32-bit packed array of states.
	///
	int GetLightStates(bool *LightState);

	///
	/// \brief Convert a 32-bit integer into an array of light states.
	/// This function takes a 32-bit integer which has been loaded from a scenario file, and
	/// expands it into an internal light state array.
	///
	/// \param LightState Array of light states, to be set from the packed array.
	/// \param state 32-bit packed array of light states to set in the array.
	///
	void SetLightStates(bool *LightState, int state);
};

//
// Strings for state saving.
//

#define CWS_START_STRING	"CWS_BEGIN"		///< String used to indicate start of CWS saved state in scenario.
#define CWS_END_STRING		"CWS_END"		///< String used to indicate end of CWS saved state in scenario.

#endif
