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
typedef union {
	struct {
		unsigned ISSWarning:1;
		unsigned LightComputerActivity:1;
		unsigned LightUplink:1;
		unsigned LightTempCaution:1;
		unsigned LightKbRel:1;
		unsigned FlashVerbNoun:1;
		unsigned LightOprErr:1;
		unsigned TestConnectorOutA:1;
		unsigned TestConnectorOutB:1;
		unsigned CautionReset:1;
		unsigned Spare1:2;
		unsigned EngineOn:1;
		unsigned EngineOff:1;
	} Bits;
	int Value;
} LMChannelValue11;

// Channel 12, GN&CS DISCRETES
typedef union {
	struct {
		unsigned ZeroRRCDU:1;
		unsigned EnableRRCDUErrorCounter:1;
		unsigned HorizVelLowScale:1;
		unsigned CoarseAlignEnable:1;
		unsigned ZeroIMUCDUs:1;
		unsigned EnableIMUCDUErrorCounters:1;
		unsigned Spare:1;
		unsigned DIDEnable:1;
		unsigned PlusPitchTrim:1;
		unsigned MinusPitchTrim:1;
		unsigned PlusRollTrim:1;
		unsigned MinusRollTrim:1;
		unsigned LRPositionCommand:1;
		unsigned RRAutoTrackOrEnable:1;
		unsigned ISSTurnOnDelayComplete:1;
	} Bits;
	unsigned int Value;
} LMChannelValue12;

// Channel 13, LGC DISCRETES
typedef union {
	struct {
		unsigned RadarC:1;
		unsigned RadarB:1;
		unsigned RadarA:1;
		unsigned RadarActivity:1;
		unsigned InhibitUplink:1;
		unsigned BlockInlink:1;
		unsigned DownlinkWordOrderCodeBit:1;
		unsigned EnableRHCCounter:1;
		unsigned RHCRead:1;
		unsigned TestAlarms:1;
		unsigned EnableStandby:1;
		unsigned ResetTrap31A:1;
		unsigned ResetTrap31B:1;
		unsigned ResetTrap32:1;
		unsigned EnableT6RUPT:1;
	} Bits;
	unsigned int Value;
} LMChannelValue13;

// Channel 14, IMU DISCRETES
typedef union {
	struct {
		unsigned OutlinkActivity:1;
		unsigned AltitudeRate:1;
		unsigned AltitudeIndicator:1;
		unsigned ThrustIndicatorDrive:1;
		unsigned Spare:1;
		unsigned GyroEnable:1;
		unsigned GyroB:1;
		unsigned GyroA:1;
		unsigned GyroMinus:1;
		unsigned GyroActivity:1;
		unsigned ShaftAngleCDUDrive:1;
		unsigned TrunnionAngleCDUDrive:1;
		unsigned ZCDUDrive:1;
		unsigned YCDUDrive:1;
		unsigned XCDUDrive:1;
	} Bits;
	unsigned int Value;
} LMChannelValue14;

// Channel 15, MAIN DSKY

// Channel 16, NAVIGATION
typedef union {
	struct {
		unsigned Spare1:1;
		unsigned Spare2:1;
		unsigned MarkX:1;
		unsigned MarkY:1;
		unsigned MarkReject:1;
		unsigned PositiveDescent:1;
		unsigned NegativeDescent:1;
	} Bits;
	unsigned int Value;
} LMChannelValue16;

// Channel 30, GN&CS DISCRETES
typedef union {
	struct {
		unsigned ABORT:1;
		unsigned StageVerify:1;
		unsigned EngineArmed:1;
		unsigned ABORT_STAGE:1;
		unsigned AutomaticThrottle:1;
		unsigned DID:1;
		unsigned RRCDUFailure:1;
		unsigned spare:1;
		unsigned IMUOperate:1;
		unsigned GNControl:1;
		unsigned IMUCage:1;
		unsigned IMUCDUFailure:1;
		unsigned IMUFailure:1;
		unsigned ISSTurnOnRequest:1;
		unsigned TemperatureInLimits:1;
	} Bits;
	unsigned int Value;
} LMChannelValue30;

// Channel 31, Translation & Rotation
typedef union {
	struct {
		unsigned PlusElevation:1;
		unsigned MinusElevation:1;
		unsigned PlusYaw:1;
		unsigned MinusYaw:1;
		unsigned PlusAzimuth:1;
		unsigned MinusAzimuth:1;
		unsigned PlusX:1;
		unsigned MinusX:1;
		unsigned PlusY:1;
		unsigned MinusY:1;
		unsigned PlusZ:1;
		unsigned MinusZ:1;
		unsigned AttitudeHold:1;
		unsigned AutomaticStab:1;
		unsigned ACAOutOfDetent:1;
	} Bits;
	unsigned int Value;
} LMChannelValue31;

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

// Channel 33, Optics
typedef union {
	struct {
		unsigned Spare:1;
		unsigned RRPowerOnAuto:1;      // RR power on and RR mode = LGC
		unsigned RRRangeLowScale:1;
		unsigned RRDataGood:1;
		unsigned LRDataGood:1;
		unsigned LRPos1:1;
		unsigned LRPos2:1;
		unsigned LRVelocityDataGood:1;
		unsigned LRRangeLowScale:1;
		unsigned BlockUplink:1;
		unsigned UplinkTooFast:1;
		unsigned DownlinkTooFast:1;
		unsigned PIPAFailed:1;
		unsigned LGC:1;                // LGC Internal Malfunction
		unsigned OscillatorAlarm:1;
	} Bits;
	unsigned int Value;
} LMChannelValue33;

// Channel 34, Downlink 1
// Channel 35, Downlink 2
