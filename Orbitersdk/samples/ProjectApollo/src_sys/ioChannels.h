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
#include <bitset>
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


typedef std::bitset<16> ChannelValue;

///
/// AGC output channel ten, used to control the DSKY lights.
///
/// \ingroup AGCIO
/// \brief AGC output channel 11.
///


enum ChannelValue11_Bits {
 ISSWarning=0,
 LightComputerActivity,		///< Turn on the Computer Activity light.
 LightUplink,					///< Turn on the Uplink Activity light.
 LightTempCaution,			///< Turn on the Temperature warning light.
 LightKbRel,					///< Turn on the Keyboard Release light.
 FlashVerbNoun,			///< Flash the Verb and Noun displays.
 LightOprErr,					///< Light the Operator Error light.
					///< Unused.
 TestConnectorOut=8,
 CautionReset,
//						///< Unused.
 EngineOn=12,					///< Turn the main engine on.
 EngineOff
};

///
/// \ingroup AGCIO
/// \brief CSM AGC output channel 12.
///

enum ChannelValue12_Bits {
	ZeroOpticsCDUs = 0,
	EnableOpticsCDUErrorCounters,
	// Unused.
	CoarseAlignEnable = 3,
	ZeroIMUCDUs,
	EnableIMUCDUErrorCounters,
	// Unused.
	TVCEnable = 7,						///< Enable thrust vector control.
	EnableSIVBTakeover,
	ZeroOptics,
	DisengageOpticsDAC,
	// Spare2							///< Unused.
	SIVBIgnitionSequenceStart = 12,		///< Start the SIVb ignition sequence for TLI.
	SIVBCutoff,						///< Shut down the SIVb.
	ISSTurnOnDelayComplete			///< Delay for ISS turnon has completed.
};



///
/// \ingroup AGCIO
/// \brief CSM AGC output channel 13.
///

enum ChannelValue13_Bits {
 RangeUnitSelectC = 0,
 RangeUnitSelectB,
 RangeUnitSelectA,
 RangeUnitActivity,
///< Unused.
 BlockInlink=6,
 DownlinkWordOrderCodeBit,
 ///< Unused.
 ///< Unused.
 TestAlarms=9,
 EnableStandby,				///< Enable AGC standby mode.
 ResetTrap31A,
 ResetTrap31B,
 ResetTrap32,
 EnableT6RUPT,
};

///
/// \ingroup AGCIO
/// \brief CSM AGC output channel 14.
///
enum ChannelValue14_Bits {
	OutLinkActivity = 0,				
	AltitudeRateSelect,					
	AltitudeMeterActivity,
	ThrustDrive,
	//Spare
	GyroEnable = 5,
	GyroSelectB,
	GyroSelectA,
	GyroSign,
	GyroActivity,
	DriveCDUS,
	DriveCDUT,
	DriveCDUZ,
	DriveCDUY,
	DriveCDUX
};

///
/// \ingroup AGCIO
/// \brief CSM AGC input channel 16.
///

enum ChannelValue16_Bits {
		NavDSKY1=0,				///< NAV DSKY
		NavDSKY2,				///< NAV DSKY
		NavDSKY3,				///< NAV DSKY
		NavDSKY4,				///< NAV DSKY
		NavDSKY5,				///< NAV DSKY
		Mark,					///< MARK key
		MarkReject				///< MARK REJECT key
}; 


///
/// \ingroup AGCIO
/// \brief CSM AGC input channel 30.
///

enum ChannelValue30_Bits {
	UllageThrust = 0,
	CMSMSeperate,				///< CM has seperated from the SM.
	SPSReady,					///< SPS is activated.
	SIVBSeperateAbort,
	LiftOff,						///< Liftoff has occured.
	GuidanceReferenceRelease,
	OpticsCDUFail,
//  Spare
	IMUOperate=8,					///< IMU is operating.
	SCControlOfSaturn,
	IMUCage,						///< IMU is caged.
	IMUCDUFail,
	IMUFail,
	ISSTurnOnRequest,
	TempInLimits
};


///
/// \ingroup AGCIO
/// \brief CSM AGC input channel 31.
///

enum ChannelValue31_Bits {
	    PlusPitchManualRotation=0,
		MinusPitchManualRotation,
		PlusYawManualRotation,
		MinusYawManualRotation,
		PlusRollManualRotation,
		MinusRollManualRotation,
		PlusXTranslation,
		MinusXTranslation,
		PlusYTranslation,
		MinusYTranslation,
		PlusZTranslation,
		MinusZTranslation,		
		HoldFunction,
		FreeFunction,
		GNAutopilotControl,
} ;


///
/// \ingroup AGCIO
/// \brief CSM AGC input channel 32.
///

enum ChannelValue32_Bits {
		PlusPitchMinImpulse=0,
		MinusPitchMinImpulse,
		PlusYawMinimumImpulse,
		MinusYawMinimumImpulse,
		PlusRollMinimumImpulse,
		MinusRollMinimumImpulse,
		// Spare 4					///< Unused.
		LMAttached=10,
		// Spare 2					///< Unused.
		Proceed=13
		//Spare 1					///< Unused.
} ;


///
/// \ingroup AGCIO
/// \brief CSM AGC input channel 33.
///

enum ChannelValue33_Bits {
        // Spare 1					///< Unused.
		RangeUnitDataGood = 1,
		// Spare 1					///< Unused.
		ZeroOptics_33 = 3,			///< Optics ZERO switch
		CMCControl,				    ///< Optics under CMC control
		// NotUsed 2				///< Unused.
		// Spare 2					///< Unused.
		BlockUplinkInput=9,
		UplinkTooFast,
		DownlinkTooFast,
		PIPAFail,
		AGCWarning,					///< AGC Internal use only.
		AGCOscillatorAlarm,
} ;

enum ChannelValue163_Bits {

	Ch163DSKYWarn = 0,				///< Turn on the CMC/LGC light.
	Ch163LightTemp = 3,				///< Turn on the Temperature light.
	Ch163LightKbRel,				///< Turn on the Keyboard Release light.
	Ch163FlashVerbNoun,				///< Flash the Verb and Noun displays.			
	Ch163LightOprErr,				///< Light the Operator Error light
	Ch163LightRestart,				///< Light the Restart light
	Ch163LightStandby,				///< Light the Standby light
	Ch163ELOff,						///< Switch off EL panel power

};

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

///
/// \brief Get AGC bank number from AGC flat address.
///
#define AGC_BANK(n) ((n) / 256)

///
/// \brief Get offset into bank from AGC flat address.
///
#define AGC_ADDR(n) ((n) & 0xff)
