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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.3  2006/01/11 22:34:20  movieman523
  *	Wired Virtual AGC to RCS and SPS, and added some CMC guidance control switches.
  *	
  *	Revision 1.2  2005/08/19 13:46:56  tschachim
  *	Added ChannelValue177.
  *	
  *	Revision 1.1  2005/08/09 00:21:37  movieman523
  *	Copied I/O channel definitions from virtual apollo.
  *	
  **************************************************************************/

typedef union {
	struct {
		unsigned d:5;
		unsigned c:5;
		unsigned b:1;
		unsigned a:4;
	} Bits;
	unsigned int Value;
} ChannelValue10;

typedef union {
	struct {
		unsigned ISSWarning:1;
		unsigned LightComputerActivity:1;
		unsigned LightUplink:1;
		unsigned LightTempCaution:1;
		unsigned LightKbRel:1;
		unsigned FlashVerbNoun:1;
		unsigned LightOprErr:1;
		unsigned Spare:1;
		unsigned TestConnectorOut:1;
		unsigned CautionReset:1;
		unsigned Spare1:2;
		unsigned EngineOnOff:1;
	} Bits;
	int Value;
} ChannelValue11;

typedef union {
	struct {
		unsigned ZeroOpticsCDUs:1;
		unsigned EnableOpticsCDUErrorCounters:1;
		unsigned NotUsed:1;
		unsigned CoarseAlignEnable:1;
		unsigned ZeroIMUCDUs:1;
		unsigned EnableIMUCDUErrorCounters:1;
		unsigned Spare:1;
		unsigned TVCEnable:1;
		unsigned EnableSIVBTakeover:1;
		unsigned ZeroOptics:1;
		unsigned DisengageOpticsDAC:1;
		unsigned Spare2:1;
		unsigned SIVBIgnitionSequenceStart:1;
		unsigned SIVBCutoff:1;
		unsigned ISSTurnOnDelayComplete:1;
	} Bits;
	unsigned int Value;
} ChannelValue12;

typedef union {
	struct {
		unsigned RangeUnitSelectC:1;
		unsigned RangeUnitSelectB:1;
		unsigned RangeUnitSelectA:1;
		unsigned RangeUnitActivity:1;
		unsigned NotUsed1:1;
		unsigned BlockInlink:1;
		unsigned DownlinkWordOrderCodeBit:1;
		unsigned NotUsed2:1;
		unsigned Spare1:1;
		unsigned TestAlarms:1;
		unsigned EnableStandby:1;
		unsigned ResetTrap31A:1;
		unsigned ResetTrap31B:1;
		unsigned ResetTrap32:1;
		unsigned EnableT6RUPT:1;
	} Bits;
	unsigned int Value;
} ChannelValue13;

typedef union {
	struct {
		unsigned NotUsed1:1;
		unsigned Spare1:1;
		unsigned Spare2:1;
		unsigned Spare3:1;
		unsigned NotUsed2:1;		
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

typedef union {
	struct {
		unsigned UllageThrust:1;
		unsigned CMSMSeperate:1;
		unsigned SPSReady:1;
		unsigned SIVBSeperateAbort:1;
		unsigned LiftOff:1;
		unsigned GuidanceReferenceRelease:1;
		unsigned OpticsCDUFail:1;
		unsigned Spare:1;
		unsigned IMUOperate:1;
		unsigned SCControlOfSaturn:1;
		unsigned IMUCage:1;
		unsigned IMUCDUFail:1;
		unsigned IMUFail:1;
		unsigned ISSTurnOnRequest:1;
		unsigned TempInLimits:1;
	} Bits;
	unsigned int Value;
} ChannelValue30;

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

typedef union {
	struct {
		unsigned PlusPitchMinImpulse:1;
		unsigned MinusPitchMinImpulse:1;
		unsigned PlusYawMinimumImpulse:1;
		unsigned MinusYawMinimumImpulse:1;
		unsigned PlusRollMinimumImpulse:1;
		unsigned MinusRollMinimumImpulse:1;
		unsigned Spare:4;
		unsigned LMAttached:1;
		unsigned Spare1:2;
		unsigned Proceed:1;
		unsigned Spare2:1;
	} Bits;
	unsigned int Value;
} ChannelValue32;

typedef union {
	struct {
		unsigned Spare:1;
		unsigned RangeUnitDataGood:1;
		unsigned Spare1:1;
		unsigned ZeroOptics:1;
		unsigned CMCControl:1;
		unsigned NotUsed:2;
		unsigned Spare2:2;
		unsigned BlockUplinkInput:1;
		unsigned UplinkTooFast:1;
		unsigned DownlinkTooFast:1;
		unsigned PIPAFail:1;
		unsigned AGCWarning:1;
		unsigned AGCOscillatorAlarm:1;
	} Bits;
	unsigned int Value;
} ChannelValue33;

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

#define AGC_BANK(n) ((n) / 256)
#define AGC_ADDR(n) ((n) & 0xff)
