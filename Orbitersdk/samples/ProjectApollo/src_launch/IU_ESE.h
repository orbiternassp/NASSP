/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

Electrical Support Equipment for the Instrument Unit (Header)

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

#pragma once

class LCCPadInterface;

#include "IUUmbilical.h"

#define LCC_EDS_MODE_OFF 0
#define LCC_EDS_MODE_MONITOR 1
#define LCC_EDS_MODE_TEST 2
#define LCC_EDS_MODE_LAUNCH 3

class IU_ESE
{
public:
	IU_ESE(LCCPadInterface *p);
	virtual ~IU_ESE();

	void Timestep(double MissionTime, double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	bool GetCommandVehicleLiftoffIndicationInhibit() { return CommandVehicleLiftoffIndicationInhibit; }
	bool GetExcessiveRollRateAutoAbortInhibit(int n);
	bool GetExcessivePitchYawRateAutoAbortInhibit(int n);
	bool GetTwoEngineOutAutoAbortInhibit(int n);
	bool GetOverrateSimulate(int n);
	bool GetThrustOKIndicateEnableInhibitA() { return ThrustOKIndicateEnableInhibitA; }
	bool GetThrustOKIndicateEnableInhibitB() { return ThrustOKIndicateEnableInhibitB; }
	bool GetEDSLiftoffInhibitA() { return EDSLiftoffInhibitA; }
	bool GetEDSLiftoffInhibitB() { return EDSLiftoffInhibitB; }
	bool GetEDSPadAbortRequest() { return PadAbortRequest; }
	bool GetEDSPowerInhibit() { return EDSPowerInhibit; }
	bool GetSIBurnModeSubstitute() { return SIBurnModeSubstitute; }
	bool GetGuidanceReferenceRelease() { return GuidanceReferenceRelease; }
	bool GetFCCPowerIsOn() { return FCCPowerIsOn; }
	bool GetQBallSimulateCmd() { return QBallSimulateCmd; }
	bool GetEDSAutoAbortSimulate(int n);
	bool GetEDSLVCutoffSimulate(int n);
	virtual bool GetSICOutboardEnginesCantInhibit() { return false; }
	virtual bool GetSICOutboardEnginesCantSimulate() { return false; }

	//IU ESE to ML
	bool GetOneEngineOutA() { return OneEngineOutA; }
	bool GetSCCutoffEnableA() { return SCCutoffEnableA; }
	bool GetSCCutoffEnableB() { return SCCutoffEnableB; }
	bool GetLiftoffReset() { return LiftoffReset; }
	bool GetLiftoffEnableA() { return LiftoffEnableA; }
	bool GetLiftoffEnableB() { return LiftoffEnableB; }
	bool GetEDSAbortCommandToSC(int n) { return EDSAbortCommandToSC[n]; }

	void SetGuidanceReferenceRelease(bool set) { GuidanceReferenceRelease = set; }
	void SetEDSPowerInhibit(bool set) { EDSPowerInhibit = set; }
	void SetEDSLiftoffInhibitA(bool set) { EDSLiftoffInhibitA = set; }
	void SetEDSLiftoffInhibitB(bool set) { EDSLiftoffInhibitB = set; }
	void SetEDSCutoffFromSC(int n, bool set) { EDSCutoffFromSC[n - 1] = set; }
	void SetThrustOKIndicateEnableInhibitA(bool set) { ThrustOKIndicateEnableInhibitA = set; }
	void SetThrustOKIndicateEnableInhibitB(bool set) { ThrustOKIndicateEnableInhibitB = set; }

	IUESEToIUCommandConnector* GetIUESEToIUCommandConnector() { return &iuESEToIUCommandConnector; }
protected:
	void SetEDSMode(int mode);

	bool CommandVehicleLiftoffIndicationInhibit;
	bool ExcessiveRollRateAutoAbortInhibit[3];
	bool ExcessivePitchYawRateAutoAbortInhibit[3];
	bool TwoEngineOutAutoAbortInhibit[3];
	//Roll 1-3, Pitch 1-3, Yaw 1-3
	bool OverrateSimulate[9];
	bool ThrustOKIndicateEnableInhibitA;
	bool ThrustOKIndicateEnableInhibitB;
	bool EDSLiftoffInhibitA;
	bool EDSLiftoffInhibitB;
	bool PadAbortRequest;
	bool EDSPowerInhibit;
	bool SIBurnModeSubstitute;
	bool GuidanceReferenceRelease;
	bool EDSCutoffFromSC[3];
	bool SwitchFCCPowerOn;
	bool SwitchFCCPowerOff;
	bool QBallSimulateCmd;
	bool EDSAutoAbortSimulate[6];
	bool EDSAbortCommandToSC[6];
	bool OneEngineOutA;
	bool SCCutoffEnableA;
	bool SCCutoffEnableB;
	bool LiftoffReset;
	bool LiftoffEnableA;
	bool LiftoffEnableB;

	//Signals from LV
	bool FCCPowerIsOn;

	bool EDSNotReady;
	bool InstrumentUnitReady;

	double LastMissionTime = 0.0;

	LCCPadInterface *Pad;
	IUESEToIUCommandConnector iuESEToIUCommandConnector;
};

class IUSV_ESE : public IU_ESE
{
public:
	IUSV_ESE(LCCPadInterface *p);

	bool GetSICOutboardEnginesCantInhibit() { return SICOutboardEnginesCantInhibit; }
	bool GetSICOutboardEnginesCantSimulate() { return SICOutboardEnginesCantSimulate; }

	void SetSICOutboardEnginesCantInhibit(bool set) { SICOutboardEnginesCantInhibit = set; }
	void SetSICOutboardEnginesCantSimulate(bool set) { SICOutboardEnginesCantSimulate = set; }
protected:
	bool SICOutboardEnginesCantInhibit;
	bool SICOutboardEnginesCantSimulate;
};