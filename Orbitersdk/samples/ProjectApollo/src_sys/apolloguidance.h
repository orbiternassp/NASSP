/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Generic Apollo Guidance computer class which is hooked
  up to the DSKY to allow easy support for the CSM and LEM
  computers. This defines the interfaces that the DSKY will
  use to either computer, so only parts specific to the CSM
  or LEM will need to be written specially.

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
  *	Revision 1.7  2012/01/14 22:16:21  tschachim
  *	GN CWS lights
  *	
  *	Revision 1.6  2011/07/11 01:42:36  vrouleau
  *	- Removed AGC_SOCKET_ENABLED flag. Rework is needed to make this an optional feature instead of a conditional define. To many untested think exists in the socket version
  *	
  *	- Checkpoint commit on the LEM RR. If the RR as been slew to track the CSM , the auto mode will continue tracking it.
  *	
  *	Revision 1.5  2010/01/04 12:31:15  tschachim
  *	Improved Saturn IB launch autopilot, bugfixes
  *	
  *	Revision 1.4  2009/09/17 17:48:41  tschachim
  *	DSKY support and enhancements of ChecklistMFD / ChecklistController
  *	
  *	Revision 1.3  2009/09/02 18:26:46  vrouleau
  *	MultiThread support for vAGC
  *	
  *	Revision 1.2  2009/08/02 19:21:07  spacex15
  *	agc socket version reenabled
  *	
  *	Revision 1.1  2009/02/18 23:21:48  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.55  2008/05/02 16:46:16  tschachim
  *	Bugfixes uplink telemetry.
  *	
  *	Revision 1.54  2007/10/18 00:23:17  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.53  2007/06/06 15:02:10  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.52  2007/01/11 07:46:10  chode99
  *	Added NextControlTime variable (for use by entry guidance)
  *	
  *	Revision 1.51  2007/01/02 01:38:24  dseagrav
  *	Digital uplink and associated stuff.
  *	
  *	Revision 1.50  2006/12/17 04:35:24  dseagrav
  *	Telecom bugfixes, eliminate false error on client disconnect, vAGC now gets cycles by a different method, eliminated old and unused vAGC P11 debugging code that was eating up FPS on every timestep.
  *	
  *	Revision 1.49  2006/11/25 11:49:21  dseagrav
  *	Connect CM optics to vAGC. Does not work properly.
  *	
  *	Revision 1.48  2006/11/24 22:42:44  dseagrav
  *	Enable changing bits in AGC channel 33, enable LEB optics switch, enable tracker switch as optics status debug switch.
  *	
  *	Revision 1.47  2006/11/13 14:47:30  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.46  2006/07/26 15:42:02  tschachim
  *	Temporary fix of the lm landing autopilot until correct attitude control is ready.
  *	
  *	Revision 1.45  2006/07/03 18:37:00  movieman523
  *	Bunch more documentation.
  *	
  *	Revision 1.44  2006/07/01 23:49:13  movieman523
  *	Updated more documentation.
  *	
  *	Revision 1.43  2006/06/25 21:19:45  movieman523
  *	Lots of Doxygen updates.
  *	
  *	Revision 1.42  2006/06/24 15:40:06  movieman523
  *	Working on MET-driven audio playback. Also added initial Doxygen comments.
  *	
  *	Revision 1.41  2006/05/19 13:48:28  tschachim
  *	Fixed a lot of devices and power consumptions.
  *	DirectO2 valve added.
  *	
  *	Revision 1.40  2006/04/23 04:15:45  dseagrav
  *	LEM checkpoint commit. The LEM is not yet airworthy. Please be patient.
  *	
  *	Revision 1.39  2006/04/04 23:36:14  dseagrav
  *	Added the beginnings of the telecom subsystem.
  *	
  *	Revision 1.38  2006/03/12 01:13:28  dseagrav
  *	Added lots of SCS items and FDAI stuff.
  *	
  *	Revision 1.37  2006/02/27 00:57:48  dseagrav
  *	Added SPS thrust-vector control. Changes 20060225-20060226.
  *	
  *	Revision 1.36  2006/02/13 21:41:41  tschachim
  *	Fixes pad load.
  *	
  *	Revision 1.35  2006/02/02 21:36:17  lazyd
  *	Changed some routines to public.
  *	
  *	Revision 1.34  2006/01/14 20:58:15  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
  *	Revision 1.33  2006/01/12 20:02:42  movieman523
  *	Updated to new Virtual AGC.
  *	
  *	Revision 1.32  2006/01/11 22:34:20  movieman523
  *	Wired Virtual AGC to RCS and SPS, and added some CMC guidance control switches.
  *	
  *	Revision 1.31  2006/01/11 19:57:55  movieman523
  *	Load appropriate AGC binary file based on mission number.
  *	
  *	Revision 1.30  2006/01/09 21:56:44  movieman523
  *	Added support for LEM and CSM AGC PAD loads in scenario file.
  *	
  *	Revision 1.29  2005/12/02 19:47:19  movieman523
  *	Replaced most PowerSource code with e_object.
  *	
  *	Revision 1.28  2005/11/25 20:59:49  movieman523
  *	Added thrust decay for SIVb in TLI burn. Still needs tweaking.
  *	
  *	Revision 1.27  2005/11/17 21:04:52  movieman523
  *	IMU and AGC now start powered-down. Revised battery code, and wired up all batteries in CSM.
  *	
  *	Revision 1.26  2005/11/17 00:28:36  movieman523
  *	Wired in AGC circuit breakers.
  *	
  *	Revision 1.25  2005/11/16 20:21:39  movieman523
  *	CSM/LEM renaming changes.
  *	
  *	Revision 1.24  2005/10/03 15:53:41  lazyd
  *	Added P19
  *	
  *	Revision 1.23  2005/09/30 11:20:42  tschachim
  *	Turn on IMU also in non-Virtual AGC mode.
  *	
  *	Revision 1.22  2005/09/24 20:54:43  lazyd
  *	Added P16
  *	
  *	Revision 1.21  2005/09/22 22:27:40  movieman523
  *	Updated erasable memory display/edit functions in AGC.
  *	
  *	Revision 1.20  2005/09/20 22:32:05  lazyd
  *	Moved routines from autoland to here
  *	
  *	Revision 1.19  2005/08/30 14:53:00  spacex15
  *	Added conditionnally defined AGC_SOCKET_ENABLED to use an external socket connected virtual AGC
  *	
  *	Revision 1.18  2005/08/22 19:47:33  movieman523
  *	Fixed long timestep on startup, and added new Virtual AGC with EDRUPT fix.
  *	
  *	Revision 1.17  2005/08/19 13:55:35  tschachim
  *	Added Channel 13 handling
  *	
  *	Revision 1.16  2005/08/18 22:15:22  movieman523
  *	Wired up second DSKY, to accurately match the real hardware.
  *	
  *	Revision 1.15  2005/08/11 01:27:26  movieman523
  *	Added initial Virtual AGC support.
  *	
  *	Revision 1.14  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.13  2005/08/09 02:28:25  movieman523
  *	Complete rewrite of the DSKY code to make it work with the real AGC I/O channels. That should now mean we can just hook up the Virtual AGC and have it work (with a few tweaks).
  *	
  *	Revision 1.12  2005/08/08 22:32:49  movieman523
  *	First steps towards reimplementing the DSKY interface to use the same I/O channels as the real AGC/DSKY interface.
  *	
  *	Revision 1.11  2005/08/08 21:10:30  movieman523
  *	Fixed broken TLI program. LastAlt wasn't being set and that screwed up the burn end calculations.
  *	
  *	Revision 1.10  2005/08/06 01:12:52  movieman523
  *	Added initial I/O channel support for CSM, and added Realism setting for LEM AGC.
  *	
  *	Revision 1.9  2005/08/05 23:37:21  movieman523
  *	Added AGC I/O channel simulation to make integrating Virtual AGC easier.
  *	
  *	Revision 1.8  2005/08/01 21:46:18  lazyd
  *	Changed Program 33 to Program 37
  *	
  *	Revision 1.7  2005/07/09 18:30:57  lazyd
  *	Added argument to ComAttitude function
  *	
  *	Revision 1.6  2005/07/06 21:58:09  lazyd
  *	Added function defs for GetHoverAttitiude and ComAttitude
  *	
  *	Revision 1.5  2005/05/19 20:26:52  movieman523
  *	Rmaia's AGC 2.0 changes integrated: can't test properly as the LEM DSKY currently doesn't work!
  *	
  *	Revision 1.4  2005/04/30 23:09:15  movieman523
  *	Revised CSM banksums and apogee/perigee display to match the real AGC.
  *	
  *	Revision 1.3  2005/04/20 17:44:48  movieman523
  *	Added call to force restart of the AGC.
  *	
  *	Revision 1.2  2005/02/24 00:24:10  movieman523
  *	Added function to get Apollo mission number from AGC.
  *	
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

#if !defined(_PA_APOLLOGUIDANCE_H)
#define _PA_APOLLOGUIDANCE_H

class DSKY;
class IMU;
class PanelSDK;

#include "powersource.h"

#include "control.h"
#include "yaAGC/agc_engine.h"
#include "thread.h"
//
// Velocity in feet per second or meters per second?
//

///
/// \ingroup AGC
/// \brief Measurement units type.
/// Used to specify whether the C++ AGC is using Imperial or Metric units.
///
enum MeasurementUnits
{ 
	UnitImperial,	///< Use Imperial units for velocity, altitude, etc.
	UnitMetric		///< Use Metric units for velocity, altitude, etc.
};

///
/// \ingroup AGC
/// \brief AGC base class.
/// This is the generic base class for an AGC, from which we derive the LEM and CSM
/// computers.
///
class ApolloGuidance: public Runnable

{
public:
	///
	/// \brief Constructor.
	/// \param s Soundlib to use for playing sounds.
	/// \param display Primary DSKY interface.
	/// \param im Spacecraft Inertial Measurement Unit.
	/// \param p Panel SDK we're connected to.
	///
	ApolloGuidance(SoundLib &s, DSKY &display, IMU &im, PanelSDK &p);

	///
	/// \brief Destructor.
	///
	virtual ~ApolloGuidance();

	///
	/// \brief Start running a program.
	/// \param prog Program number (major mode) to start running.
	///
	void RunProgram(int prog);

	///
	/// This function is called when the user enters a verb and noun in order to determine
	/// whether the combination is valid. It's a pure virtual class because it will be defined in
	/// the derived classes: verbs and nouns for the CSM AGC aren't the same as the LEM AGC.
	/// \brief Validate a VERB/NOUN combination.
	/// \param verb Verb number to start running.
	/// \param noun Noun number to start running.
	/// \return True if the combination is valid.
	///
	virtual bool ValidateVerbNoun(int verb, int noun) = 0;

	///
	/// \brief Process a new verb and noun combination.
	/// \param verb The new Verb.
	/// \param noun The new Noun.
	///
	virtual void ProcessVerbNoun(int verb, int noun) = 0;

	///
	/// Validate program number for programs specific to a derived
	/// class of computer.
	///
	/// \brief Validate program number.
	/// \param prog Program number.
	/// \return True if the program is supported.
	///
	virtual bool ValidateProgram(int prog) = 0;
	virtual void DisplayNounData(int noun) = 0;

	///
	/// \brief Update program status for CSM or LEM programs when the PRO key is pressed.
	/// \param R1 Contents of DSKY register 1.
	/// \param R2 Contents of DSKY register 2.
	/// \param R3 Contents of DSKY register 3.
	///
	virtual void ProgPressed(int R1, int R2, int R3) = 0;

	///
	/// \brief Proceed in program without the user entering data, making a best guess.
	///
	virtual void ProceedNoData() = 0;

	///
	/// \brief Terminate the current program.
	///
	virtual void TerminateProgram() = 0;

	///
	/// \brief Get a flagword value.
	/// \param num Flagword number.
	/// \return Flagword value.
	///
	virtual unsigned int GetFlagWord(int num) = 0;

	///
	/// \brief Set a flagword value.
	/// \param num Flagword number.
	/// \param val Flagword value to set.
	///
	virtual void SetFlagWord(int num, unsigned int val) = 0;

	///
	/// \brief Read from simulated erasable memory.
	/// \param loc Erasable memory address.
	/// \param val Reference to return value.
	/// \return True if the read was from a valid address.
	///
	virtual bool ReadMemory(unsigned int loc, int &val) = 0;

	///
	/// \brief Write to simulated erasable memory.
	/// \param loc Erasable memory address.
	/// \param val Value to store in memory location.
	/// \return True if the write was to a valid address.
	///
	virtual void WriteMemory(unsigned int loc, int val) = 0;

	///
	/// RSet will clear the two-level alarm code stack, but not the 'most
	/// recent' alarm.
	///
	/// \brief RSET key was pressed.
	///
	void RSetPressed();

	///
	/// A new Verb and Noun have been entered. Validate them and raise an error if they can't
	/// be validated.
	///
	/// \brief Process Verb/Noun entry.
	/// \param verb New Verb.
	/// \param noun New Noun.
	///
	void VerbNounEntered(int verb, int noun);

	///
	/// \brief Validate common Verb and Noun combinations which are used in both CSM and LEM.
	/// \param verb New Verb.
	/// \param noun New Noun.
	///
	bool ValidateCommonVerbNoun(int verb, int noun);

	///
	/// \brief Validate a generic program used in both CSM and LEM.
	/// \param prog Program to validate.
	/// \return True if it's a valid program.
	///
	bool ValidateCommonProgram(int prog);
	bool DisplayCommonNounData(int noun);

	///
	/// \brief Update program status for generic programs when the PRO key is pressed.
	/// \param R1 Contents of DSKY register 1.
	/// \param R2 Contents of DSKY register 2.
	/// \param R3 Contents of DSKY register 3.
	///
	bool GenericProgPressed(int R1, int R2, int R3);
	void ProcessCommonVerbNoun(int verb, int noun);

	///
	/// \brief Proceed in generic program without the user entering data, making a best guess.
	///
	bool CommonProceedNoData();

	///
	/// \brief Orbiter timestep processing.
	/// \param simt Mission time in seconds.
	/// \param simdt Time since last timestep.
	///
	virtual void Timestep(double simt, double simdt) = 0;
	virtual void SystemTimestep(double simdt); 

	///
	/// \brief Pass information about the spacecraft to the AGC.
	/// \param ISP Main engine ISP.
	/// \param Thrust Main engine thrust.
	/// \param MainIsHover Is the main engine a hover engine?
	///
	void SetVesselStats(double ISP, double Thrust, bool MainIsHover);

	///
	/// \brief Set the vessel that the AGC is controlling.
	/// \param v The vessel this AGC controls.
	///
	void ControlVessel(VESSEL *v) { OurVessel = v; };

	///
	/// \brief Set the Apollo mission number for this spacecraft.
	/// \param flight The mission number.
	///
	void SetApolloNo(int flight) { ApolloNo = flight; };

	///
	/// \brief Get the Apollo mission number for this flight.
	/// \return Mission number.
	///
	int GetApolloNo() { return ApolloNo; };

	///
	/// \brief Set the desired landing site (Moon for LEM, Earth for CSM)
	/// \param latitude Latitude in degrees.
	/// \param longitude Longitude in degrees.
	/// \param altitude Altitude above surface in meters.
	///
	void SetDesiredLanding(double latitude, double longitude, double altitude);

	void ComAttitude(VECTOR3 &actatt, VECTOR3 &tgtatt, bool fast);
	VECTOR3 OrientAxis(VECTOR3 &vec, int axis, int ref, double gainFactor = 1.0);
	void OrbitParams(VECTOR3 &rpos, VECTOR3 &rvel, double &period, double &apo, double &tta, 
				 double &per, double &ttp);
	void EquToRel(double vlat, double vlon, double vrad, VECTOR3 &pos);
	void Lambert(VECTOR3 &stpos, VECTOR3 &renpos, double dt, double mu, 
						  VECTOR3 &v1, VECTOR3 &v2);
	void PredictPosVelVectors(const VECTOR3 &Pos, const VECTOR3 &Vel,
		double Mu, double Time, VECTOR3 &NewPos, VECTOR3 &NewVel, double &NewVelMag);

	///
	/// \brief Force the AGC to restart.
	///
	void ForceRestart();

	///
	/// \brief Start the computer up from standby.
	///
	void Startup();

	///
	/// \brief Go to standby.
	///
	void GoStandby();

	///
	/// \brief Is the AGC in standby mode?
	///
	bool OnStandby() { return (!Yaagc && Standby); };

	///
	/// \brief Is the AGC out of reset?
	///
	bool OutOfReset();

	///
	/// Indicate that a checklist action is required by using Verb 20 Noun 25 and the checklist
	/// action number.
	///
	/// \brief Checklist action required.
	/// \param num Checklist action number.
	///
	void Checklist(int num);

	///
	/// \brief Blank all DSKY displays.
	///
	void BlankAll();

	//
	// Program helper functions.
	//

	///
	/// \brief Countdown display for the reset program.
	///
	void ResetCountdown();

	///
	/// \brief Terminate a generic program.
	///
	void TerminateCommonProgram();

	///
	/// \brief Abort the current program and raise a Program Alarm.
	/// \param ErrNo Program alarm to raise.
	///
	void AbortWithError(int ErrNo);

	///
	/// \brief Convert from velocity in meters per second to velocity in display units based on AGC setup.
	/// \param vel Velocity in meters per second.
	/// \return Velocity in display units (meters per second or feet per second as appropriate).
	///
	double DisplayVel(double vel);

	///
	/// \brief Convert from altitude in kilometers to altitude in display units based on AGC setup.
	/// \param alt Altitude in kilometers.
	/// \return Altitude in display units (kilometers or nautical miles as appropriate).
	///
	double DisplayAlt(double alt);

	///
	/// \brief Convert from velocity in display units to velocity in meters per second based on AGC setup.
	/// \param vel Velocity in display units (meters per second or feet per second as appropriate).
	/// \return Velocity in meters per second.
	///
	double GetVel(double vel);

	//
	// External event handlers.
	//

	///
	/// \brief Update spacecraft fuel level.
	/// \param fuel Fuel level.
	///
	void SetFuel(double fuel) { CurrentFuel = fuel; };

	///
	/// \brief Set spacecraft relative velocity.
	/// \param vel Velocity in meters per second.
	///
	void SetRVel(double vel) { CurrentRVel = vel; };

	///
	/// \brief Indicate that the engines have shutdown after launch.
	///
	void LaunchShutdown();

	///
	/// \brief Save AGC state to scenario file.
	/// \param scn Scenario file to save to.
	///
	void SaveState(FILEHANDLE scn);

	///
	/// \brief Load AGC state from scenario file.
	/// \param scn Scenario file to load from.
	///
	void LoadState(FILEHANDLE scn);

	//
	// I/O channels.
	//

	///
	/// Set or clear a bit in an AGC input channel. This is used to simulate the real hardware interface
	/// to the AGC, and is required to properly connect the Virtual AGC software.
	///
	/// This is a virtual function so it can he hooked by derived classes to update their
	/// state when the channel value changes. The default function here only supports the
	/// channels which are common to the CSM and LEM.
	///
	/// \brief Set input channel bit.
	/// \param channel Input channel to set.
	/// \param bit Bit number to update.
	/// \param val The bit value. True to set, false to clear.
	///
	virtual void SetInputChannelBit(int channel, int bit, bool val);

	///
	/// Set or clear a bit in an AGC output channel. This is used to simulate the real hardware interface
	/// to the AGC, and is required to properly connect the Virtual AGC software.
	///
	/// This is a virtual function so it can he hooked by derived classes to update their
	/// state when the channel value changes. The default function here only supports the
	/// channels which are common to the CSM and LEM.
	///
	/// Note that the AGC 'bit 1' is actually 'bit 0' in today's terminology, so bit numbers
	/// start at 1.
	///
	/// \brief Set output channel bit.
	/// \param channel Output channel to set.
	/// \param bit Bit number to update.
	/// \param val The bit value. True to set, false to clear.
	///
	virtual void SetOutputChannelBit(int channel, int bit, bool val);

	///
	/// Set the value of an AGC input channel. This is used to simulate the real hardware interface
	/// to the AGC, and is required to properly connect the Virtual AGC software.
	///
	/// This is a virtual function so it can he hooked by derived classes to update their
	/// state when the channel value changes. The default function here only supports the
	/// channels which are common to the CSM and LEM.
	///
	/// \brief Set output channel byte.
	/// \param channel Input channel to set.
	/// \param val Byte value to set in the channel.
	///
	virtual void SetOutputChannel(int channel, unsigned int val);

	///
	/// Get the specified bit from an output channel.
	///
	/// Note that the AGC 'bit 1' is actually 'bit 0' in today's terminology, so bit numbers
	/// start at 1.
	///
	/// \brief Get output channel bit.
	/// \param channel Output channel to read from.
	/// \param bit Bit number to read.
	/// \return True if bit is set.
	///
	virtual bool GetOutputChannelBit(int channel, int bit);

	///
	/// Get the value of an output channel.
	///
	/// \brief Get output channel value.
	/// \param channel Output channel to read from.
	/// \return Value of the output channel.
	///
	virtual unsigned int GetOutputChannel(int channel);

	///
	/// Set the value of an input channel.
	///
	/// \brief Set input channel value.
	/// \param channel Input channel to set.
	/// \param val Value to set in the input channel.
	///
	virtual void SetInputChannel(int channel, unsigned int val);

	///
	/// Get the specified bit from an input channel.
	///
	/// Note that the AGC 'bit 1' is actually 'bit 0' in today's terminology, so bit numbers
	/// start at 1.
	///
	/// \brief Get input channel bit.
	/// \param channel Input channel to read from.
	/// \param bit Bit number to read.
	/// \return True if bit is set.
	///
	bool GetInputChannelBit(int channel, int bit);

	///
	/// Get the value of an input channel.
	///
	/// \brief Get input channel value.
	/// \param channel Input channel to read from.
	/// \return Value of the input channel.
	///
	unsigned int GetInputChannel(int channel);

	//
	// Virtual AGC memory access.
	//

	///
	/// This function allows you read data back from the erasable memory in the Virtual AGC. If called on the
	/// C++ AGC it returns zero.
	///
	/// \brief Return the contents of an erasable memory location
	/// \param bank Memory bank to access.
	/// \param address Memory location within the bank to access.
	/// \return The value stored in the memory location.
	///
	int GetErasable(int bank, int address);

	///
	/// This function sets data in the erasable memory in the Virtual AGC. If called on the
	/// C++ AGC it does nothing.
	///
	/// \param bank Memory bank to access.
	/// \param address Memory location within the bank to access.
	/// \param value The value to store in the memory location.
	///
	void SetErasable(int bank, int address, int value);

	///
	/// Load a PAD value into the AGC. Used for initialising the LEM when created.
	///
	/// \brief Load PAD data into the AGC.
	/// \param address The EMEM address to update.
	/// \param value The value to story in the memory location.
	///
	void PadLoad(unsigned int address, unsigned int value);

	///
	/// \brief Proces PIPA pulses from the IMU.
	/// \param RegPIPA Register to update.
	/// \param pulses Number of pulses.
	///
	void PulsePIPA(int RegPIPA, int pulses);

	///
	/// \brief Is this a Virtual AGC?
	/// \return True for Virtual AGC, false for C++ AGC.
	///
	bool IsVirtualAGC() { return Yaagc; };

	///
	/// \brief Triggers Virtual AGC core dump
	///
	void VirtualAGCCoreDump(char *fileName);

	///
	/// \brief Set the Virtual AGC state.
	/// \param is_virtual True to make the AGC run with the Virtual AGC.
	///
	void SetVirtualAGC(bool is_virtual) { Yaagc = is_virtual; };

	//
	// Generally useful setup.
	//

	///
	/// Pass information about the mission to the AGC, which needs to know which vessels
	/// it's working with, the mission it's flying, and the realism level.
	///
	/// Amongst other things, we need to know the mission number so that we know which version
	/// of the Colossus software to load into the Virtual AGC.
	///
	/// \brief Set mission info in AGC.
	/// \param MissionNo Apollo mission number.
	/// \param RealismValue Current realism level.
	/// \param OtherVessel Pointer to the LEM so that the CSM can track it for rendevouz.
	///
	virtual void SetMissionInfo(int MissionNo, int RealismValue, char *OtherVessel = 0);

	///
	/// \brief Initialise the Virtual AGC.
	/// \param binfile Path to the file containing the AGC binary code.
	///
	virtual void InitVirtualAGC(char *binfile);

	//
	// Power supply.
	//

	///
	/// \brief Wire the AGC to external power buses.
	/// \param a Power bus 1.
	/// \param b Power bus 2.
	///
	void WirePower(e_object *a, e_object *b) { DCPower.WireToBuses(a, b); PowerConnected = true; };

	///
	/// \brief Is the AGC supplied with power.
	/// \return True if the AGC is powered.
	///
	bool IsPowered();

	void SetDSKY2(DSKY *d2) { dsky2 = d2; };

	///
	/// \brief alarm flags for CWS
	///
	bool GetProgAlarm() { return ProgAlarm; };
	bool GetGimbalLockAlarm() { return GimbalLockAlarm; };

protected:

	//
	// Various programs we can run.
	//

	///
	/// \brief Run the reset program.
	/// \param simt Current Mission Elapsed Time.
	///
	void ResetProg(double simt);

	///
	/// \brief Run Program 16.
	/// \param simt Current Mission Elapsed Time.
	///
	void Prog16(double simt);

	///
	/// \brief Process Pro key while running Program 16.
	/// \param R1 Value in DSKY R1 register.
	/// \param R2 Value in DSKY R2 register.
	/// \param R3 Value in DSKY R3 register.
	///
	void Prog16Pressed(int R1, int R2, int R3);

	///
	/// \brief Run Program 16.
	/// \param simt Current Mission Elapsed Time.
	///
	void Prog17(double simt);

	///
	/// \brief Process Pro key while running Program 17.
	/// \param R1 Value in DSKY R1 register.
	/// \param R2 Value in DSKY R2 register.
	/// \param R3 Value in DSKY R3 register.
	///
	void Prog17Pressed(int R1, int R2, int R3);

	///
	/// \brief Run Program 16.
	/// \param simt Current Mission Elapsed Time.
	///
	void Prog18(double simt);

	///
	/// \brief Process Pro key while running Program 18.
	/// \param R1 Value in DSKY R1 register.
	/// \param R2 Value in DSKY R2 register.
	/// \param R3 Value in DSKY R3 register.
	///
	void Prog18Pressed(int R1, int R2, int R3);

	///
	/// \brief Run Program 19.
	/// \param simt Current Mission Elapsed Time.
	///
	void Prog19(double simt);

	///
	/// \brief Process Pro key while running Program 19.
	/// \param R1 Value in DSKY R1 register.
	/// \param R2 Value in DSKY R2 register.
	/// \param R3 Value in DSKY R3 register.
	///
	void Prog19Pressed(int R1, int R2, int R3);

	///
	/// \brief Run Program 37.
	/// \param simt Current Mission Elapsed Time.
	///
	void Prog37(double simt);

	///
	/// \brief Process Pro key while running Program 37.
	/// \param R1 Value in DSKY R1 register.
	/// \param R2 Value in DSKY R2 register.
	/// \param R3 Value in DSKY R3 register.
	///
	void Prog37Pressed(int R1, int R2, int R3);

	//
	// DSKY interface.
	//

	char ValueChar(unsigned val);
	unsigned CharValue(char val);

	void SetChannel10Lights(int bit, bool val);

	void ProcessInputChannel15(int val);
	void ProcessInputChannel30(int bit, bool val);
	void ProcessInputChannel32(int bit, bool val);

	virtual void ProcessChannel5(int val);
	virtual void ProcessChannel6(int val);
	virtual void ProcessChannel10(int val);
	virtual void ProcessChannel11Bit(int bit, bool val);
	virtual void ProcessChannel11(int val);
	virtual void ProcessChannel13(int val);
	virtual void ProcessChannel14(int val);
	virtual void ProcessChannel160(int val);
	virtual void ProcessChannel161(int val);
	virtual void ProcessIMUCDUErrorCount(int channel, unsigned int val);
	public: virtual void GenerateDownrupt();
	public: virtual void GenerateUprupt();
    public: virtual void GenerateRadarupt();
	public: virtual bool IsUpruptActive();
	public: virtual void SetCh33Switches(unsigned int val);
	public: unsigned int GetCh33Switches();
	public: virtual int DoPINC(int16_t *Counter);
	public: virtual int DoPCDU(int16_t *Counter);
	public: virtual int DoMCDU(int16_t *Counter);
	public: virtual int DoDINC(int CounterNum, int16_t *Counter);

	///
	/// \brief Set the Uplink Activity light on the DSKY.
	///
	void LightUplink();

	///
	/// \brief Clear the Uplink Activity light on the DSKY.
	///
	void ClearUplink();

	///
	/// \brief Set the Computer Activity light on the DSKY.
	///
	void LightCompActy();

	///
	/// \brief Clear the Computer Activity light on the DSKY.
	///
	void ClearCompActy();

	///
	/// \brief Set the temperature warning light on the DSKY.
	///
	void LightTemp();

	///
	/// \brief Clear the temperature warning light on the DSKY.
	///
	void ClearTemp();

	///
	/// \brief Set the Keyboard Release light on the DSKY.
	///
	void LightKbRel();

	///
	/// \brief Clear the Keyboard Release light on the DSKY.
	///
	void ClearKbRel();

	///
	/// \brief Set the Operator Error light on the DSKY.
	///
	void LightOprErr();

	///
	/// \brief Clear the Operator Error light on the DSKY.
	///
	void ClearOprErr();

	///
	/// \brief Set the No Attitude light on the DSKY.
	///
	void LightNoAtt();

	///
	/// \brief Clear the No Attitude light on the DSKY.
	///
	void ClearNoAtt();

	///
	/// \brief Set the Gimbal Lock light on the DSKY.
	///
	void LightGimbalLock();

	///
	/// \brief Clear the Gimbal Lock light on the DSKY.
	///
	void ClearGimbalLock();

	///
	/// \brief Set the Tracker light on the DSKY.
	///
	void LightTracker();

	///
	/// \brief Clear the Tracker light on the DSKY.
	///
	void ClearTracker();

	///
	/// \brief Set the Program Alarm light on the DSKY.
	///
	void LightProg();

	///
	/// \brief Clear the Program Alarm light on the DSKY.
	///
	void ClearProg();

	///
	/// \brief Set the Velocity light on the DSKY.
	///
	void LightVel();

	///
	/// \brief Clear the Velocity light on the DSKY.
	///
	void ClearVel();

	///
	/// \brief Set the Altitude light on the DSKY.
	///
	void LightAlt();

	///
	/// \brief Clear the Altitude light on the DSKY.
	///
	void ClearAlt();

	///
	/// \brief Flash the Verb and Noun indicators on the DSKY.
	///
	void SetVerbNounFlashing();

	///
	/// \brief Stop flashing the Verb and Noun indicators on the DSKY.
	///
	void ClearVerbNounFlashing();

	//
	// Odds and ends.
	//

	///
	/// Errors in the AGC are indicated by program alarm numbers. This function adds the alarm
	/// to the list of alarms occuring and lights the Program Alarm light on the DSKY.
	///
	/// \brief Raise a program alarm.
	/// \param AlarmNo The program alarm number.
	///
	void RaiseAlarm(int AlarmNo);

	void DoOrbitBurnCalcs(double simt);
	bool SingleTimestepPrep(double simt, double simdt);
	bool SingleTimestep();
	bool GenericTimestep(double simt, double simdt);
	bool GenericReadMemory(unsigned int loc, int &val);
	void GenericWriteMemory(unsigned int loc, int val);

	///
	/// This function displays a time on the DSKY in R1, R2 and R3 in the standard format used
	/// by the AGC (hours, minutes, seconds * 100).
	///
	/// \brief Display a time on the DSKY.
	/// \param t Time in seconds.
	///
	void DisplayTime(double t);

	///
	/// Flash Verb 37 on the DSKY to indicate that the user should switch to a new program. This
	/// will normally be called when one program has completed (e.g. after launch shutdown).
	///
	/// \brief Indicate thatthe user should switch to a new program.
	///
	void AwaitProgram();

	///
	/// Sets a verb and noun combination and flashes them on the DSKY.
	///
	/// \brief Set a Verb and Noun display and flash them.
	/// \param Verb Verb to display.
	/// \param Noun Noun to display.
	///
	void SetVerbNounAndFlash(int Verb, int Noun);

	///
	/// Sets a verb and noun combination and shows them on the DSKY.
	///
	/// \brief Set a Verb and Noun display.
	/// \param Verb Verb to display.
	/// \param Noun Noun to display.
	///
	void SetVerbNoun(int Verb, int Noun);
	double NewVelocity_AorP(double Mu_Planet, double &Rapo, double &Rperi, double &Rnew);
	void KillAllThrusters();
	void OrientForOrbitBurn(double simt);
	void GetHoverAttitude( VECTOR3 &actatt);

	/// \todo Dirty Hack for the AGC++ attitude control, 
	/// remove this and use I/O channels and pulsed thrusters 
	/// identical to the VAGC instead
	virtual void SetAttitudeRotLevel(VECTOR3 level) = 0;

	///
	/// \brief Set the thrust level of the main engine.
	/// \param thrust Thrust level (0.0 to 1.0).
	///
	virtual void BurnMainEngine(double thrust);

	virtual void DisplayBankSum();
	void DisplayEMEM(unsigned int addr);
	virtual bool OrbitCalculationsValid() = 0;
	void DisplayOrbitCalculations();

	///
	/// Function to take a generic time from the R1, R2 and R3 registers in the standard AGC format
	/// (hours/minutes/seconds * 100), and put it in the BurnTime variable as a number of seconds after
	/// the current Mission Elapsed Time.
	///
	/// This way, for example, the user can enter a time for a burn to start, press Pro, and after
	/// calling UpdateBurnTime the MET for the start of the burn will be stored in BurnTime.
	///
	/// \brief Update the BurnTime based on the time in R1/R2/R3.
	/// \param R1 Partial time in hours.
	/// \param R2 Partial time in minutes.
	/// \param R3 Partial time in hundredths of seconds.
	///
	void UpdateBurnTime(int R1, int R2, int R3);
	int16_t ConvertDecimalToAGCOctal(double x, bool highByte);

	///
	/// \brief Is the AGC in power-saving Standby mode?
	///
	bool Standby;

	///
	/// \brief Are we running the reset program?
	///
	bool Reset;

	///
	/// \brief Are we in orbit or landed on a planet?
	///
	bool InOrbit;

	///
	/// Some generic autopilot code needs to know whether the main engine of the vessel is a normal
	/// inline engine (CSM) or a hover engine (LEM).
	///
	/// \brief Is the main thruster a hover thruster?
	///
	bool MainThrusterIsHover;

	//
	// DSKY interface.
	//

	int Chan10Flags;

	int Prog;
	bool ProgBlanked;
	int Verb;
	bool VerbBlanked;
	int Noun;
	bool NounBlanked;
	int R1;
	bool R1Blanked;
	bool R1Decimal;
	char R1Format[7];
	int R2;
	bool R2Blanked;
	bool R2Decimal;
	char R2Format[7];
	int R3;
	bool R3Blanked;
	bool R3Decimal;
	char R3Format[7];

	char TwoDigitEntry[2];
	char FiveDigitEntry[6];

	///
	/// \brief Temporary for transferring values to DSKY.
	///
	char RegStr[8];

	//
	// Data entry status.
	//

	///
	/// \brief User is entering a VERB.
	///
	bool EnteringVerb;

	///
	/// \brief User is entering a NOUN.
	///
	bool EnteringNoun;

	///
	/// \brief User is entering an octal value rather than decimal.
	///
	bool EnteringOctal;

	int	EnterPos;
	int EnterVal;
	int	EnteringData;
	int EnterCount;

	///
	/// When the user is entering a value in the DSKY, this tracks whether the value they're
	/// entering is positive or negative.
	///
	/// \brief Entering positive or negative data?
	///
	bool EnterPositive;

	bool KbInUse;


	//
	// DSKY interface functions.
	//

	void StartTwoDigitEntry();
	void StartFiveDigitEntry(bool octal);
	void ClearFiveDigitEntry();
	void UpdateTwoDigitEntry(int n);
	void UpdateFiveDigitEntry(int n);

	void TwoDigitDisplay(char *Str, int val, bool Blanked);
	void FiveDigitDisplay(char *Str, int val, bool Blanked, bool Decimal, char *Format);

	bool KBCheck();
	void SetR1(int val);
	void SetR2(int val);
	void SetR3(int val);
	void SetR1Octal(int val);
	void SetR2Octal(int val);
	void SetR3Octal(int val);
	void DisplayR1Octal() { R1Decimal = false; UpdateR1(); };
	void DisplayR2Octal() { R2Decimal = false; UpdateR2(); };
	void DisplayR3Octal() { R3Decimal = false; UpdateR3(); };
	void SetR1Format(char *fmt) { strncpy(R1Format, fmt, 7); UpdateR1(); };
	void SetR2Format(char *fmt) { strncpy(R2Format, fmt, 7); UpdateR2(); };
	void SetR3Format(char *fmt) { strncpy(R3Format, fmt, 7); UpdateR3(); };
	void SetProg(int val);
	void SetVerb(int val);
	void SetNoun(int val);

	void BlankData();
	void BlankR1();
	void BlankR2();
	void BlankR3();
	void UnBlankAll();
		
	void ReleaseKeyboard();
	void UpdateProg();
	void UpdateNoun();
	void UpdateVerb();
	void UpdateR1();
	void UpdateR2();
	void UpdateR3();
	void UpdateAll();
	void UpdateEntry();
	void KeyRel();
	void VerbPressed();
	void NounPressed();
	void ProgKeyPressed();
	void ResetPressed();
	void ClearPressed();
	void DataEntryR1();
	void DataEntryR1R2();
	void DataEntryR2();
	void DataEntryR3();

	void DoSetR1(int val, bool decimal);
	void DoSetR2(int val, bool decimal);
	void DoSetR3(int val, bool decimal);

	void PlusPressed();
	void MinusPressed();
	void EnterPressed();
	void NumberPressed(int val);

	void LightsOff();

	//
	// Generic program flags. Higher
	// level code is responsible for
	// saving these in the Flag Words.
	//

	bool ProgFlag01;
	bool ProgFlag02;
	bool ProgFlag03;
	bool ProgFlag04;
	bool RetroFlag;
	bool BurnFlag;

	///
	/// \brief The primary DSKY attached to this AGC.
	///
	DSKY &dsky;
	DSKY *dsky2;

	///
	/// \brief The IMU attached to this AGC.
	///
	IMU &imu;

	int ProgRunning;
	int VerbRunning;
	int NounRunning;
	int ProgState;

	int Alarm01;
	int Alarm02;
	int Alarm03;

	unsigned int TIME1;

	MeasurementUnits DisplayUnits;

	//
	// Program data.
	//

	double LastAlt;

	//
	// Various program internal state variables.
	//

	int ResetCount;

	///
	/// \brief Apollo mission number.
	///
	int ApolloNo;

	///
	/// \brief Realism level.
	///
	int Realism;

	///
	/// \brief Are we running the Virtual AGC rather than the C++ AGC?
	///
	bool Yaagc;

	double ResetTime;
	double LastProgTime;
	double NextProgTime;
	double LastTimestep;
	double LastCycled;
	double CurrentTimestep;

	bool isFirstTimestep;

	bool isLGC;
	///
	/// \brief Have we loaded the initial startup data into the Virtual AGC?
	///
	bool PadLoaded;

	void GetPosVel();

	double CurrentVel;
	double CurrentRVel;
	double CurrentVelX;
	double CurrentVelY;
	double CurrentVelZ;
	double CurrentFuel;
	double CurrentAlt;
	double CurrentRoll;
	double CurrentPitch;
	double CurrentYaw;

	double TargetRoll;
	double TargetPitch;
	double TargetYaw;

	///
	/// \brief Desired Apogee for launch or orbit change.
	///
	double DesiredApogee;

	///
	/// \brief Desired Perigee for launch or orbit change.
	///
	double DesiredPerigee;

	///
	/// \brief Desired Azimuth for launch.
	///
	double DesiredAzimuth;

	///
	/// \brief Desired Inclination for launch.
	///
	double DesiredInclination;

	///
	/// \brief Desired latitude for landing (Earth for CSM or Moon for LEM).
	///
	double LandingLatitude;

	///
	/// \brief Desired longitude for landing (Earth for CSM or Moon for LEM).
	///
	double LandingLongitude;

	///
	/// \brief Desired altitude for landing on the Moon.
	///
	double LandingAltitude;
	double DeltaPitchRate;

	double LastVerb16Time;
	double NextEventTime;
	double LastEventTime;
	double NextControlTime;

	double BurnTime;
	double CutOffVel;
	double BurnStartTime;
	double BurnEndTime;
	double DesiredDeltaV;

	double MaxThrust;
	double VesselISP;

	///
	/// \brief Estimated delta-V for SIVb engine shutdown thrust.
	///
	double ThrustDecayDV;

	double DesiredDeltaVx;
	double DesiredDeltaVy;
	double DesiredDeltaVz;

	double DesiredPlaneChange;
	double DesiredLAN;

	unsigned int BankSumNum;
	unsigned int CurrentEMEMAddr;

#define MAX_INPUT_CHANNELS	0200
#define MAX_OUTPUT_CHANNELS	0200

	///
	/// \brief AGC input channel values.
	///
	unsigned int InputChannel[MAX_INPUT_CHANNELS + 1];

	///
	/// \brief AGC output channel values.
	///
	unsigned int OutputChannel[MAX_OUTPUT_CHANNELS + 1];

	//
	// Power supply.
	//

	PowerMerge DCPower;
	bool PowerConnected;

	///
	/// \brief The Vessel we're controlling.
	///
	VESSEL	*OurVessel;

	///
	/// \brief The sound library for the vessel we're controlling.
	///
	SoundLib &soundlib;

	///
	/// The rendevouz programs require the AGC to track another vessel which is the target
	/// for the rendevouz. We need to know the name of that vessel so we can look it up in
	/// Orbiter.
	/// 
	/// \brief The name of the 'other vessel' (e.g. CSM for LEM AGC, LEM for CSM AGC).
	///
	char OtherVesselName[64];

#ifdef _DEBUG
	FILE *out_file;
#endif


	
	///
	/// Virtual AGC state structure. We create this even if we don't use it, so we don't need
	/// to faff around with allocating and deleting memory.
	///
	/// We don't do much with this directly, but it has to be passed to the Virtual AGC calls
	/// so they can keep track of their state.
	/// \brief Virtual AGC state.
	///
	agc_t vagc;
	Mutex agcCycleMutex;
	Event timeStepEvent;
	double thread_simt;
	double thread_simdt;

	///
	/// \brief alarm flags for CWS
	///
	bool ProgAlarm;
	bool GimbalLockAlarm;
};

extern char TwoSpaceTwoFormat[];

//
// Strings for state saving.
//

#define AGC_START_STRING	"AGC_BEGIN"		///< String to start AGC state dump in scenario file.
#define AGC_END_STRING		"AGC_END"		///< String to end AGC state dump in scenario file.

#define EMEM_ENTRIES	(8 * 0400)			///< Number of EMEM values to simulate

#endif // _PA_APOLLOGUIDANCE_H
