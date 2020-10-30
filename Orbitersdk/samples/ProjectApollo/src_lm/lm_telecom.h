/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2009

  LM Telecommunications Subssytems

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

/* PCM DOWN-TELEMETRY

	HBR FRAME:
	8 bit words, 128 words per frame.

	HEADER: (SAME FOR LBR, FRAME COUNT = 1)
	50D01A = SYNC 1						11111101   (Octal 375)
	50D01B = SYNC 2						11001010   (Octal 312)
	50D01C = SYNC 3						01101000   (Octal 150)
	50D01D = FRAME COUNT				00nnnnnn   (1 thru 50)

	... = SPECIAL (SEE SWITCH CASE)

	WORD	A/ALL		B			C			D			E
	1		50D01A	
	2		50D01B
	3		50D01C
	4		50D01D
	5		...
	6		200E1A
	7		200E1B
	8		100E01
	9		200A01
	10		200A02
	11		200A03
	12		200A04
	13		200A05
	14		200A06
	15		200A07
	16		100E02
	17		100A01
	18		100A02
	19		100A03
	20		100A04
	21		100A05
	22		100A06
	23		100A07
	24		50E001
	25		100A08
	26		100A09
	27		100A10
	28		100A11
	29		100A12
	30		100A13
	31		100A14
	32		50E002
	33		10D01A		10A008		10A018		10A028		10A037
	34		10D01B		10A009		10A019		10A029		10A038
	35		10D01C		10A010		10A020		10A030		10A039
	36		10D01D		10A011		10A021		10A031		10A040
	37		...
	38		200E1A
	39		200E1B
	40		100E03
	41		200A01
	42		200A02
	43		200A03
	44		200A04
	45		200A05
	46		200A06
	47		200A07
	48		100E04
	49		100A15
	50		100A16
	51		100A17
	52		100A18
	53		100A19
	54		100A20
	55		100A21
	56		100A22
	57		50A001
	58		50A002
	59		50A003
	60		50A004
	61		50A005
	62		50A006
	63		50A007
	64		50A008
	65		10A001		10A012		10A022		10A032		10A041
	66		10A002		10A013		10A023		10A033		10A042
	67		10A003		10A014		10A024		10A034		10A043
	68		10A004		10A015		10A025		10A035		10A044
	69		...
	70		200E1A
	71		200E1B
	72		100E01
	73		200A01
	74		200A02
	75		200A03
	76		200A04
	77		200A05
	78		200A06
	79		200A07
	80		100E02
	81		100A01
	82		100A02
	83		100A03
	84		100A04
	85		100A05
	86		100A06
	87		100A07
	88		50E003
	89		100A08
	90		100A09
	91		100A10
	92		100A11
	93		100A12
	94		100A13
	95		100A14
	96		50E004
	97		50D002
	98		...
	99		10A006		10A016		10A026		10A036		10A045
	100		...
	101		...	
	102		200E1A
	103		200E1B
	104		100E03
	105		200A01
	106		200A02
	107		200A03
	108		200A04
	109		200A05
	110		200A06
	111		200A07
	112		100E04
	113		100A15
	114		100A16
	115		100A17
	116		100A18
	117		100A19
	118		100A20
	119		100A21
	120		100A22
	121		50DS1A
	122		50DS1B
	123		50DS1C
	124		50DS1D
	125		50DS1E
	126		50DS2A
	127		50DS2B
	128		50DS2C
	*/

#define LTLM_A		1
#define LTLM_D		2
#define LTLM_DS		3
#define LTLM_E		4

class Saturn;

class LM_VHFAntenna
{
public:
	LM_VHFAntenna(VECTOR3 dir, double maximumGain);
	~LM_VHFAntenna();

	double getPolarGain(VECTOR3 target);
private:
	VECTOR3 pointingVector;
	double maxGain;
};

// VHF system (and shared stuff)
class LM_VHF {
public:
	LM_VHF();
	void Init(LEM *vessel, h_HeatLoad *vhfh, h_HeatLoad *secvhfh, h_HeatLoad *pcmh, h_HeatLoad *secpcmh);	       // Initialization
	void Timestep(double simt);        // TimeStep
	void SystemTimestep(double simdt); // System Timestep
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);
	void RangingSignal(Saturn *sat, bool isAcquiring); //############################# DELETE ME ###############################

	const double freqXCVR_A = 296.8; //MHz;
	const double freqXCVR_B = 259.7; //MHz;

	LEM *lem;					   // Ship we're installed in
	VESSEL *csm;					//Pointer to CSM
	h_HeatLoad *VHFHeat;			//VHF Heat Load
	h_HeatLoad *VHFSECHeat;			//VHF Heat Load
	h_HeatLoad *PCMHeat;			//PCM Heat Load
	h_HeatLoad *PCMSECHeat;			//PCM Heat Load

	LM_VHFAntenna fwdInflightVHF;
	LM_VHFAntenna aftInflightVHF;
	LM_VHFAntenna evaVHF;

	bool receiveA;
	bool receiveB;
	bool transmitA;
	bool transmitB;
	bool isRanging;

	//void SetRCVDrfPropA(double freq, double pow, double gain, double phase, bool tone) { RCVDfreqRCVR_A = freq; RCVDpowRCVR_A = pow; RCVDgainRCVR_A = gain; RCVDPhaseRCVR_A = phase;  }; //not needed at the moment
	void SetRCVDrfPropB(double freq, double pow, double gain, double phase, bool tone) { RCVDfreqRCVR_B = freq; RCVDpowRCVR_B = pow; RCVDgainRCVR_B = gain; RCVDPhaseRCVR_B = phase; RCVDRangeTone = tone; }; 

	//Ranging RF properties
	double RangingRCVDfreq;
	double RangingRCVDpow;
	double RangingRCVDgain;
	double RangingRCVDPhase;

	// Winsock2
	WSADATA wsaData;				// Winsock subsystem data
	SOCKET m_socket;				// TCP socket
	sockaddr_in service;			// SOCKADDR_IN
	SOCKET AcceptSocket;			// Accept Socket
	int conn_state;                 // Connection State
	int uplink_state;               // Uplink State
	void perform_io(double simt);   // Get data from here to there
	void handle_uplink();			// Handle incoming data
	void generate_stream_lbr();     // Generate LBR datastream
	void generate_stream_hbr();     // Same for HBR datastream
	unsigned char scale_data(double data, double low, double high); // Scale data for PCM transmission
	unsigned char scale_scea(double data); // Scale preconditioned data from the SCEA for PCM transmission
	unsigned char measure(int channel, int type, int ccode);
	// Error control
	int wsk_error;                  // Winsock error
	char wsk_emsg[256];             // Winsock error message
	// PCM datastream management
	double last_update;				// simt of last update
	double last_rx;                 // simt of last uplink update
	int word_addr;                  // Word address of outgoing packet
	int frame_addr;                 // Frame address
	int frame_count;				// Frame counter
	int tx_size;                    // Number of words to send
	int tx_offset;                  // Offset to use
	int rx_offset;					// RX offset to use
	int mcc_offset;					// RX offset into MCC data block
	int mcc_size;					// Size of MCC data block
	int pcm_rate_override;          // Downtelemetry rate override
	unsigned char tx_data[1024];    // Characters to be transmitted
	unsigned char rx_data[1024];    // Characters recieved
	unsigned char mcc_data[2048];	// MCC-provided incoming data

	bool registerSocket(SOCKET sock);

	friend class MCC;				// Allow MCC to write directly to buffer

protected:

	//double RCVDfreqRCVR_A;
	//double RCVDpowRCVR_A;
	//double RCVDgainRCVR_A;
	//double RCVDPhaseRCVR_A;

	//Ranging RF properties
	double RCVDfreqRCVR_B;
	double RCVDpowRCVR_B;
	double RCVDgainRCVR_B;
	double RCVDPhaseRCVR_B;
	bool RCVDRangeTone;
};

// Generic S-Band Antenna
class LM_SBandAntenna
{
public:
	LM_SBandAntenna() { SignalStrength = 0.0; }
	double GetSignalStrength() { return SignalStrength; }
	double dBm2SignalStrength(double dBm);
protected:
	double SignalStrength;						// Signal Strength (0-100)
};

// S-Band system
class LM_SBAND {
public:
	LM_SBAND();
	void Init(LEM *vessel, h_HeatLoad *sbxh, h_HeatLoad *secsbxh, h_HeatLoad *sbph, h_HeatLoad *secsbph);	       // Initialization
	void Timestep(double simt);        // Timestep
	void SystemTimestep(double simdt); // System Timestep
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);

	LEM *lem;					   // Ship we're installed in
	h_HeatLoad *SBXHeat;			//XCVR Heat
	h_HeatLoad *SBXSECHeat;			//XCVR Heat
	h_HeatLoad *SBPHeat;			//PMP Heat
	h_HeatLoad *SBPSECHeat;			//PMP Heat
	int pa_mode_1,pa_mode_2;       // Power amplifier state
	double pa_timer_1,pa_timer_2;  // Tube heater timer
	int tc_mode_1,tc_mode_2;	   // Transciever state
	double tc_timer_1,tc_timer_2;  // Tube heater timer
	double rcvr_agc_voltage;		// Receiver AGC Voltage
	LM_SBandAntenna *ant;
};

// S-Band Steerable Antenna
class LEM_SteerableAnt: public LM_SBandAntenna {
public:
	LEM_SteerableAnt();
	void Init(LEM *s, h_Radiator *an, Boiler *anheat);
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);
	void Timestep(double simdt);
	void SystemTimestep(double simdt);			// System Timestep
	void DefineAnimations(UINT idx);
	bool IsPowered();
	double GetAntennaTempF();
	double GetPitch() { return pitch*DEG; }
	double GetYaw() { return yaw*DEG; }
	VECTOR3 pitchYaw2GlobalVector(double pitch, double yaw);
	double LEM_SteerableAntGain;
	double LEM_SteerableAntFrequency;
	double LEM_SteerableAntWavelength;
	double Gain85ft;
	double Power85ft;

	LEM *lem;					// Pointer at LEM
	h_Radiator *antenna;			// Antenna (loses heat into space)
	Boiler *antheater;			// Antenna Heater (puts heat back into antenna)
protected:
	double pitch;
	double yaw;
	double HornSignalStrength[4];


	// Animations
	UINT anim_SBandPitch, anim_SBandYaw;
	double	sband_proc[2];
	double	sband_proc_last[2];

	bool moving;
	double hpbw_factor;

	const MATRIX3 NBSA = _M(cos(45.0*RAD), -sin(45.0*RAD), 0.0, sin(45.0*RAD), cos(45.0*RAD), 0.0, 0.0, 0.0, 1.0);
	const OBJHANDLE hMoon = oapiGetObjectByName("Moon");
	const OBJHANDLE hEarth = oapiGetObjectByName("Earth");
};

//S-Band Omnidirectional Antenna system

class LM_OMNI :public LM_SBandAntenna {
public:
	LM_OMNI(VECTOR3 dir);
	void Init(LEM *vessel);	// Initialization
	void Timestep();			// Timestep

	double OMNIWavelength;
	double OMNIFrequency;
	double Gain85ft;
	double Power85ft;
	double OMNI_Gain;
protected:
	LEM *lem;					// Ship we're installed in
	VECTOR3 direction;
	double hpbw_factor;			//Beamwidth factor
	OBJHANDLE hMoon;
	OBJHANDLE hEarth;
};

///
/// LM DSE holds 5,400 inches of tape (4 tracks, 2.5 hours each at 0.6 inches/second, making 21,600 inches of recordable tape)
///
class LM_DSEA : public e_object
{
	enum LM_DSEAState
	{
		STOPPED,			/// Tape is stopped
		STARTING_RECORD,	/// Tape is accelerating to play speed
		SLOWING_RECORD,		/// Tape is slowing to record speed
		RECORDING,			/// Tape is recording
		STOPPING,			/// Tape is stopping
	};

public:
	LM_DSEA();
	virtual ~LM_DSEA();

	void Init(LEM *l, h_HeatLoad *dseht);	       // Initialization

									   ///
									   /// \brief Tape motion indicator.
									   ///
	bool TapeMotion();

	///
	/// \brief Stop tape playing.
	///
	void Stop();

	///
	/// \brief Start tape recording.
	///
	void Record();

	bool RecordLogic();
	bool IsSWPowered();
	bool IsACPowered();
	bool IsPCMPowered();
	bool LMPVoiceXmit();
	bool CDRVoiceXmit();
	bool VoiceXmit();
	bool ICSPTT();
	bool VOXPTT();
	void SystemTimestep(double simdt);
	void Timestep(double simt, double simdt);

	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);

protected:
	LEM *lem;						    /// Ship we're installed in
	h_HeatLoad *DSEHeat;				/// Heatload
	double tapeSpeedInchesPerSecond;	/// Tape speed in inches per second.
	double desiredTapeSpeed;			/// Desired tape speed in inches per second.
	double tapeMotion;					/// Tape motion from 0.0 to 1.0.
	LM_DSEAState state;					/// Tape state.

	double lastEventTime;				/// Last event time.
};
