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

  **************************************************************************/

#if !defined(_PA_APOLLOGUIDANCE_H)
#define _PA_APOLLOGUIDANCE_H

class DSKY;
class IMU;
class CDU;
class PanelSDK;

#include <bitset>
#include "powersource.h"

#include "control.h"
#include "yaAGC/agc_engine.h"
#include "thread.h"


typedef std::bitset<16> ChannelValue;
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
	ApolloGuidance(SoundLib &s, DSKY &display, IMU &im, CDU &sc, CDU &tc, PanelSDK &p);

	///
	/// \brief Destructor.
	///
	virtual ~ApolloGuidance();

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
	/// \brief Orbiter timestep processing.
	/// \param simt Mission time in seconds.
	/// \param simdt Time since last timestep.
	///
	virtual void Timestep(double simt, double simdt) = 0;
	virtual void SystemTimestep(double simdt); 

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
	/// \brief Force the AGC to restart.
	///
	void ForceRestart();

	///
	/// \brief Is the AGC in standby mode?
	///
	bool OnStandby() { return vagc.Standby; };

	///
	/// \brief Is the AGC out of reset?
	///
	bool OutOfReset();

	//
	// External event handlers.
	//

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
	virtual void SetInputChannelBit(int channel, int bit,bool val);

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
	virtual void SetOutputChannel(int channel, ChannelValue val);

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
	virtual void SetInputChannel(int channel, ChannelValue val);

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

	virtual void ProcessIMUCDUReadCount(int channel, int val);

	///
	/// \brief Triggers Virtual AGC core dump
	///
	void VirtualAGCCoreDump(char *fileName);

	//
	// Generally useful setup.
	//

	///
	/// Pass information about the mission to the AGC, which needs to know which vessels
	/// it's working with and the mission it's flying.
	///
	/// Amongst other things, we need to know the mission number so that we know which version
	/// of the Colossus software to load into the Virtual AGC.
	///
	/// \brief Set mission info in AGC.
	/// \param MissionNo Apollo mission number.
	/// \param OtherVessel Pointer to the LEM so that the CSM can track it for rendevouz.
	///
	virtual void SetMissionInfo(int MissionNo, char *OtherVessel = 0);

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
	// DSKY interface.
	//

	virtual void ProcessChannel5(ChannelValue val);
	virtual void ProcessChannel6(ChannelValue val);
	virtual void ProcessChannel10(ChannelValue val);
	virtual void ProcessChannel11Bit(int bit, bool val);
	virtual void ProcessChannel11(ChannelValue val);
	virtual void ProcessChannel13(ChannelValue val);
	virtual void ProcessChannel14(ChannelValue val);
	virtual void ProcessChannel140(ChannelValue val);
	virtual void ProcessChannel141(ChannelValue val);
	virtual void ProcessChannel142(ChannelValue val);
	virtual void ProcessChannel143(ChannelValue val);
	virtual void ProcessChannel163(ChannelValue val);
	virtual void ProcessIMUCDUErrorCount(int channel, ChannelValue val);
	public: virtual void GenerateHandrupt();
	public: virtual void GenerateDownrupt();
	public: virtual void GenerateUprupt();
    public: virtual void GenerateRadarupt();
	public: virtual bool IsUpruptActive();
	public: virtual int DoPINC(int16_t *Counter);
	public: virtual int DoPCDU(int16_t *Counter);
	public: virtual int DoMCDU(int16_t *Counter);
	public: virtual int DoDINC(int CounterNum, int16_t *Counter);

	//
	// Odds and ends.
	//

	bool SingleTimestepPrep(double simt, double simdt);
	bool SingleTimestep();
	bool GenericTimestep(double simt, double simdt);
	bool GenericReadMemory(unsigned int loc, int &val);
	void GenericWriteMemory(unsigned int loc, int val);

	int16_t ConvertDecimalToAGCOctal(double x, bool highByte);

	///
	/// \brief Are we running the reset program?
	///
	bool Reset;

	///
	/// \brief The primary DSKY attached to this AGC.
	///
	DSKY &dsky;
	DSKY *dsky2;

	///
	/// \brief The IMU attached to this AGC.
	///
	IMU &imu;

	CDU &tcdu;
	CDU &scdu;

	//
	// Program data.
	//

	///
	/// \brief Apollo mission number.
	///
	int ApolloNo;

	double LastTimestep;
	double LastCycled;
	double CurrentTimestep;

	bool isFirstTimestep;

	bool isLGC;
	///
	/// \brief Have we loaded the initial startup data into the Virtual AGC?
	///
	bool PadLoaded;


#define MAX_INPUT_CHANNELS	0200
#define MAX_OUTPUT_CHANNELS	0200

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

//
// Strings for state saving.
//

#define AGC_START_STRING	"AGC_BEGIN"		///< String to start AGC state dump in scenario file.
#define AGC_END_STRING		"AGC_END"		///< String to end AGC state dump in scenario file.

#define EMEM_ENTRIES	(8 * 0400)			///< Number of EMEM values to simulate

#endif // _PA_APOLLOGUIDANCE_H
