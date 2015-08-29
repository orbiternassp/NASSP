/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  I/O channel definitions for LM AGC.

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
#include <bitset>
// Channel 5, RCS PITCH & YAW
typedef union {
	struct {
		unsigned B4U:1;
		unsigned A4D:1;
		unsigned A3U:1;
		unsigned B3D:1;
		unsigned B2U:1;
		unsigned A2D:1;
		unsigned A1U:1;
		unsigned B1D:1;
	} Bits;
	int Value;
} LMChannelValue5;

// Channel 6, RCS ROLL
typedef union {
	struct {
		unsigned B3A:1;
		unsigned B4F:1;
		unsigned A1F:1;
		unsigned A2A:1;
		unsigned B2L:1;
		unsigned A3R:1;
		unsigned A4R:1;
		unsigned B1L:1;
	} Bits;
	int Value;
} LMChannelValue6;

typedef std::bitset<16> ChannelValue;

// Channel 10, DSKY
typedef union {
	struct {
		unsigned RelayBit1:1;
		unsigned RelayBit2:1;
		unsigned VELCaution:1;
		unsigned RelayBit4:1;
		unsigned ALTCaution:1;
		unsigned RelayBit6:1;
		unsigned RelayBit7:1;
		unsigned RelayBit8:1;
		unsigned RelayBit9:1;
		unsigned RelayBit10:1;
		unsigned RelayBit11:1;
		unsigned RelayAddr1:1;
		unsigned RelayAddr2:1;
		unsigned RelayAddr3:1;
		unsigned RelayAddr4:1;
	} Bits;
	int Value;
} LMChannelValue10;

// Channel 11, DSKY II
enum ChannelValue11_Bits {
	ISSWarning=0,
	LightComputerActivity,
	LightUplink,
	LightTempCaution,
	LightKbRel,
	FlashVerbNoun,
	LightOprErr,
	TestConnectorOutA,
	TestConnectorOutB,
	CautionReset,
	Spare1,
	EngineOn,
	EngineOff
};

// Channel 12, GN&CS DISCRETES
enum ChannelValue12_Bits {
	ZeroRRCDUs = 0,
	EnableRRCDUErrorCounter,
	// Unused.
	CoarseAlignEnable = 3,
	ZeroIMUCDUs,
	EnableIMUCDUErrorCounters,
	// Unused.
	DispayInertialData = 7,						///< Enable thrust vector control.
	PlusPitchVehicleMotion,
	MinusPitchVehicleMotion,
	PlusRollVehicleMotion,
	MinusRollVehicleMotion,
	LRPositionCommand,							
	RRAutoTrackOrEnable = 13,		///< Start the SIVb ignition sequence for TLI.
	ISSTurnOnDelayComplete			///< Delay for ISS turnon has completed.
};

// Channel 13, LGC DISCRETES
enum ChannelValue13_Bits {
	RadarC=0,
    RadarB,
    RadarA,
	RadarActivity,
	InhibitUplink,
	BlockInlink,
	DownlinkWordOrderCodeBit,
	EnableRHCCounter,
	RHCRead,
	TestAlarms,
	EnableStandby,
	ResetTrap31A,
	ResetTrap31B,
	ResetTrap32,
	EnableT6RUPT
};

// Channel 14, IMU DISCRETES
enum ChannelValue14_Bits {
	OutlinkActivity = 0,
    AltitudeRate,
	AltitudeIndicator,
	ThrustIndicatorDrive,
	Spare,
	GyroEnable,
	GyroB,
	GyroA,
	GyroMinus,
	GyroActivity,
	ShaftAngleCDUDrive,
	TrunnionAngleCDUDrive,
	ZCDUDrive,
	YCDUDrive,
	XCDUDrive
};

// Channel 15, MAIN DSKY

// Channel 16, NAVIGATION
enum ChannelValue16_Bits {
	// Spare 2				///< NAV DSKY
	MarkX = 2,				///< NAV DSKY
	MarkY,				///< NAV DSKY
	MarkReject_LM,				///< NAV DSKY
	DescendPlus,					///< MARK key
	DescendMinus				///< MARK REJECT key
};


// Channel 30, GN&CS DISCRETES
enum ChannelValue30_Bits {
	AbortWithDescentStage = 0,
	DescendStageAttached,
	EngineArmed,
	AbortWithAscentStage,
	AutoThrottle,						///< Liftoff has occured.
	DisplayInertialData,
	RendezVousCDUFail,
	// Spare
	IMUOperate = 8,					///< IMU is operating.
	GNControlOfSC,
	IMUCage,						///< IMU is caged.
	IMUCDUFailure,
	IMUFailure,
	ISSTurnOnRequest,
	TempInLimits
};

// Channel 31, Translation & Rotation
enum ChannelValue31_Bits {
	     PlusElevation = 0,
		 MinusElevation,
		 PlusYaw,
		 MinusYaw,
		 PlusAzimuth,
		 MinusAzimuth,
		 PlusX,
		 MinusX,
		 PlusY,
		 MinusY,
		 PlusZ,
		 MinusZ,
		 AttitudeHold,
		 AutomaticStab,
		 ACAOutOfDetent
};

// Channel 32, Impulse
typedef union {
	struct {
		unsigned A4D_A4R_Failed:1;
		unsigned A3U_A3R_Failed:1;
		unsigned B4U_B4F_Failed:1;
		unsigned B3D_B3A_Failed:1;
		unsigned B1D_B1L_Failed:1;
		unsigned A1U_A1F_Failed:1;
		unsigned B2U_B2L_Failed:1;
		unsigned A2D_A2A_Failed:1;
		unsigned GimbalOff:1;
		unsigned GimbalFail:1;
	} Bits;
	unsigned int Value;
} LMChannelValue32;


enum ChannelValue32_Bits {
	Thruster2_4_Disabled = 0,
	Thruster5_8_Disabled,
	Thruster1_3_Disabled,
	Thruster6_7_Disabled,
	Thruster14_16_Disabled,
	Thruster13_15_Disabled,
	Thruster9_12_Disabled,
	Thruster10_11_Disabled,
	DescentEngineGimbalsDisabled,
	ApparentDecscentEngineGimbalsFailed,
	// Spare 3 
	Proceed = 13
	// Spare
};

// Channel 33, Optics
enum ChannelValue33_Bits {
	//Spare
	RRPowerOnAuto = 1,      // RR power on and RR mode = LGC
	RRRangeLowScale,
	RRDataGood,
	LRDataGood,
	LRPos1,
	LRPos2,
	LRVelocityDataGood,
	LRRangeLowScale,
	BlockUplink,
	UplinkTooFast,
	DownlinkTooFast,
	PIPAFailed,
	LGC,                // LGC Internal Malfunction
	OscillatorAlarm
};

// Channel 34, Downlink 1
// Channel 35, Downlink 2
