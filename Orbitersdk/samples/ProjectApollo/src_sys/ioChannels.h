/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  I/O channel definitions for AGC.

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

///
/// AGC output channel ten, used to control the DSKY displays. Output values sent to
/// this channel can have numerous different results based on differnt combinations
/// of the a, b, c and d values.
///
/// \ingroup AGCIO
/// \brief AGC output channel 10.
///
typedef union {
	struct {
		unsigned d:5;						///< Typically second display digit.
		unsigned c:5;						///< Typically first display digit.
		unsigned b:1;						///< Typically positive or negative sign.
		unsigned a:4;						///< Indicates the kind of request.
	} Bits;
	unsigned int Value;
} ChannelValue10;

///
/// AGC output channel ten, used to control the DSKY lights.
///
/// \ingroup AGCIO
/// \brief AGC output channel 11.
///
typedef union {
	struct {
		unsigned ISSWarning:1;
		unsigned LightComputerActivity:1;		///< Turn on the Computer Activity light.
		unsigned LightUplink:1;					///< Turn on the Uplink Activity light.
		unsigned LightTempCaution:1;			///< Turn on the Temperature warning light.
		unsigned LightKbRel:1;					///< Turn on the Keyboard Release light.
		unsigned FlashVerbNoun:1;				///< Flash the Verb and Noun displays.
		unsigned LightOprErr:1;					///< Light the Operator Error light.
		unsigned Spare:1;						///< Unused.
		unsigned TestConnectorOut:1;
		unsigned CautionReset:1;
		unsigned Spare1:2;						///< Unused.
		unsigned EngineOnOff:1;					///< Turn the main engine on.
	} Bits;
	int Value;
} ChannelValue11;

///
/// \ingroup AGCIO
/// \brief CSM AGC output channel 12.
///
typedef union {
	struct {
		unsigned ZeroOpticsCDUs:1;
		unsigned EnableOpticsCDUErrorCounters:1;
		unsigned NotUsed:1;							///< Unused.
		unsigned CoarseAlignEnable:1;
		unsigned ZeroIMUCDUs:1;
		unsigned EnableIMUCDUErrorCounters:1;
		unsigned Spare:1;							///< Unused.
		unsigned TVCEnable:1;						///< Enable thrust vector control.
		unsigned EnableSIVBTakeover:1;
		unsigned ZeroOptics:1;
		unsigned DisengageOpticsDAC:1;
		unsigned Spare2:1;							///< Unused.
		unsigned SIVBIgnitionSequenceStart:1;		///< Start the SIVb ignition sequence for TLI.
		unsigned SIVBCutoff:1;						///< Shut down the SIVb.
		unsigned ISSTurnOnDelayComplete:1;			///< Delay for ISS turnon has completed.
	} Bits;
	unsigned int Value;
} ChannelValue12;

///
/// \ingroup AGCIO
/// \brief CSM AGC output channel 13.
///
typedef union {
	struct {
		unsigned RangeUnitSelectC:1;
		unsigned RangeUnitSelectB:1;
		unsigned RangeUnitSelectA:1;
		unsigned RangeUnitActivity:1;
		unsigned NotUsed1:1;					///< Unused.
		unsigned BlockInlink:1;
		unsigned DownlinkWordOrderCodeBit:1;
		unsigned NotUsed2:1;					///< Unused.
		unsigned Spare1:1;						///< Unused.
		unsigned TestAlarms:1;
		unsigned EnableStandby:1;				///< Enable AGC standby mode.
		unsigned ResetTrap31A:1;
		unsigned ResetTrap31B:1;
		unsigned ResetTrap32:1;
		unsigned EnableT6RUPT:1;
	} Bits;
	unsigned int Value;
} ChannelValue13;

///
/// \ingroup AGCIO
/// \brief CSM AGC output channel 14.
///
typedef union {
	struct {
		unsigned NotUsed1:1;				///< Unused.
		unsigned Spare1:1;					///< Unused.
		unsigned Spare2:1;					///< Unused.
		unsigned Spare3:1;					///< Unused.
		unsigned NotUsed2:1;				///< Unused.
		unsigned GyroEnable:1;
		unsigned GyroSelectB:1;
		unsigned GyroSelectA:1;
		unsigned GyroSign:1;
		unsigned GyroActivity:1;
		unsigned DriveCDUS:1;
		unsigned DriveCDUT:1;
		unsigned DriveCDUZ:1;
		unsigned DriveCDUY:1;
		unsigned DriveCDUX:1;
	} Bits;
	unsigned int Value;
} ChannelValue14;

///
/// \ingroup AGCIO
/// \brief CSM AGC input channel 16.
///
typedef union {
	struct {
		unsigned NavDSKY1:1;				///< NAV DSKY
		unsigned NavDSKY2:1;				///< NAV DSKY
		unsigned NavDSKY3:1;				///< NAV DSKY
		unsigned NavDSKY4:1;				///< NAV DSKY
		unsigned NavDSKY5:1;				///< NAV DSKY
		unsigned Mark:1;					///< MARK key
		unsigned MarkReject:1;				///< MARK REJECT key
	} Bits;
	unsigned int Value;
} ChannelValue16;

///
/// \ingroup AGCIO
/// \brief CSM AGC input channel 30.
///
typedef union {
	struct {
		unsigned UllageThrust:1;
		unsigned CMSMSeperate:1;				///< CM has seperated from the SM.
		unsigned SPSReady:1;					///< SPS is activated.
		unsigned SIVBSeperateAbort:1;
		unsigned LiftOff:1;						///< Liftoff has occured.
		unsigned GuidanceReferenceRelease:1;
		unsigned OpticsCDUFail:1;
		unsigned Spare:1;						///< Unused.
		unsigned IMUOperate:1;					///< IMU is operating.
		unsigned SCControlOfSaturn:1;
		unsigned IMUCage:1;						///< IMU is caged.
		unsigned IMUCDUFail:1;
		unsigned IMUFail:1;
		unsigned ISSTurnOnRequest:1;
		unsigned TempInLimits:1;
	} Bits;
	unsigned int Value;
} ChannelValue30;

///
/// \ingroup AGCIO
/// \brief CSM AGC input channel 31.
///
typedef union {
	struct {
		unsigned PlusPitchManualRotation:1;
		unsigned MinusPitchManualRotation:1;
		unsigned PlusYawManualRotation:1;
		unsigned MinusYawManualRotation:1;
		unsigned PlusRollManualRotation:1;
		unsigned MinusRollManualRotation:1;
		unsigned PlusXTranslation:1;
		unsigned MinusXTranslation:1;
		unsigned PlusYTranslation:1;
		unsigned MinusYTranslation:1;
		unsigned PlusZTranslation:1;
		unsigned MinusZTranslation:1;		
		unsigned HoldFunction:1;
		unsigned FreeFunction:1;
		unsigned GNAutopilotControl:1;
	} Bits;
	unsigned int Value;
} ChannelValue31;

///
/// \ingroup AGCIO
/// \brief CSM AGC input channel 32.
///
typedef union {
	struct {
		unsigned PlusPitchMinImpulse:1;
		unsigned MinusPitchMinImpulse:1;
		unsigned PlusYawMinimumImpulse:1;
		unsigned MinusYawMinimumImpulse:1;
		unsigned PlusRollMinimumImpulse:1;
		unsigned MinusRollMinimumImpulse:1;
		unsigned Spare:4;					///< Unused.
		unsigned LMAttached:1;
		unsigned Spare1:2;					///< Unused.
		unsigned Proceed:1;
		unsigned Spare2:1;					///< Unused.
	} Bits;
	unsigned int Value;
} ChannelValue32;

///
/// \ingroup AGCIO
/// \brief CSM AGC input channel 33.
///
typedef union {
	struct {
		unsigned Spare:1;					///< Unused.
		unsigned RangeUnitDataGood:1;
		unsigned Spare1:1;					///< Unused.
		unsigned ZeroOptics:1;				///< Optics ZERO switch
		unsigned CMCControl:1;				///< Optics under CMC control
		unsigned NotUsed:2;					///< Unused.
		unsigned Spare2:2;					///< Unused.
		unsigned BlockUplinkInput:1;
		unsigned UplinkTooFast:1;
		unsigned DownlinkTooFast:1;
		unsigned PIPAFail:1;
		unsigned AGCWarning:1;
		unsigned AGCOscillatorAlarm:1;
	} Bits;
	unsigned int Value;
} ChannelValue33;

///
/// \ingroup AGCIO
/// \brief AGC input channel 177.
///
typedef union {
	struct {
		unsigned GyroPulses:11;
		unsigned GyroEnable:1;
		unsigned GyroSelectB:1;
		unsigned GyroSelectA:1;
		unsigned GyroSign:1;
	} Bits;
	unsigned int Value;
} ChannelValue177;

//
// For now we'll also put AGC addresses in here.
//

//
// For Artemis 072, these are 03065 and 03066.
//

#define AGC_DAPDTR1			03066
#define AGC_DAPDTR2			03067

///
/// \brief Get AGC bank number from AGC flat address.
///
#define AGC_BANK(n) ((n) / 256)

///
/// \brief Get offset into bank from AGC flat address.
///
#define AGC_ADDR(n) ((n) & 0xff)
