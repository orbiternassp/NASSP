/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Saturn Emergency Detection System 602A5 (Header)

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

class IU;

class EDS
{
public:
	EDS(IU *iu);
	virtual ~EDS() {}
	virtual void Timestep(double simdt) = 0;
	void SetPlatformFailureParameters(bool PlatFail, double PlatFailTime);

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	//Switch Selector Functions
	void SetSIVBEngineOutIndicationA(bool set) { SIVBEngineOutIndicationA = set; }
	void SetSIVBEngineOutIndicationB(bool set) { SIVBEngineOutIndicationB = set; }
	void SetLVEnginesCutoffEnable1() { if (LVEnginesCutoffCommand2 == false) { LVEnginesCutoffEnable1 = true; } }
	void ResetAutoAbortRelays() { AutoAbortEnableRelayA = false; AutoAbortEnableRelayB = false; }
	void SetSIVBEngineCutoffDisabled() { SIVBEngineCutoffDisabled = true; }
	void SetSIVBRestartAlert(bool set) { SIVBRestartAlert = set; }
	void EnableSCControl() { SCControlEnableRelay = true; }
	void DisableSCControl() { SCControlEnableRelay = false; }
	void EnableCommandSystem() { IUCommandSystemEnable = true; }

	//GSE Reset Buses
	void ResetBus1();
	void ResetBus2();

	//To spacecraft
	bool GetLiftoffCircuitA() { return LiftoffA; }
	bool GetLiftoffCircuitB() { return LiftoffB; }
	bool GetEDSAbort(int n);
	virtual double GetLVTankPressure(int n);

	//To IU
	bool GetIULiftoff() { return IULiftoffRelay; }

	//To FCC
	bool GetSCControl();

	//To Control Distributor
	bool GetIUCommandSystemEnable();

	//GSE
	bool GetLiftoffEnableA() { return AutoAbortEnableRelayA; }
	bool GetLiftoffEnableB() { return AutoAbortEnableRelayB; }
	bool GetAutoAbort() { return AutoAbortBus; }
	void SetIUEDSBusPowered(bool set) { IUEDSBusPowered = set; }
protected:
	IU* iu;

	bool LVEnginesCutoffVote();

	//Buses:

	//+6D91-93
	bool IUEDSBusPowered;
	//+6D95
	bool AutoAbortBus;

	//Relays:
	
	//A4K1
	bool SCControlEnableRelay;
	//A4K6, A10K3, K55-1, K55-2
	bool LVAttRefFail;
	//A6K1, A7K1, A8K3 (K43-1 - K43-3)
	bool TwoEngOutAutoAbortDeactivate;
	//K46-K48 (K46-1 - K46-3)
	bool ExcessRatesAutoAbortDeactivatePY;
	//K52-K54 (K47-1 - K71-3)
	bool ExcessRatesAutoAbortDeactivateR;
	//K19 (K19-1)
	bool LVEnginesCutoffEnable1;
	//K29 (K19-2)
	bool LVEnginesCutoffEnable2;
	//K78-1 (K173)
	bool SIIEngineOutIndicationA;
	//K88-2 (K174)
	bool SIIEngineOutIndicationB;
	//K6 (K66)
	bool SIVBEngineOutIndicationA;
	//K91 (K167)
	bool SIVBEngineOutIndicationB;
	//K2 (K90)
	bool AutoAbortInhibitRelayA;
	//K3 (K91)
	bool AutoAbortInhibitRelayB;
	//K4 (K4)
	bool IULiftoffRelay;
	//K65 (K92)
	bool AutoAbortEnableRelayA;
	//K66 (K93)
	bool AutoAbortEnableRelayB;
	//K40 (K20-1)
	bool LVEnginesCutoffFromSC1;
	//K41-1/2 (K20-2/4)
	bool LVEnginesCutoffFromSC2;
	//K42 (K20-3)
	bool LVEnginesCutoffFromSC3;
	//K76 (K232)
	bool SIVBRestartAlert;
	//K77-1/2
	bool IUCommandSystemEnable;
	//K9-1/2, (K9-1/2)
	bool LVEnginesCutoffCommand1;
	//K98, K99 (K230, K231)
	bool LVEnginesCutoffCommand2;
	//K86-1/2 (K10-1/2)
	bool LVEnginesCutoffCommand3;
	bool SIVBEngineCutoffDisabled;

	//Signals
	bool LiftoffA;
	bool LiftoffB;
	bool EDSAbortSignal1;
	bool EDSAbortSignal2;
	bool EDSAbortSignal3;

	//Other
	bool SecondPlaneSeparationMonitor;
	bool SIEDSCutoff;
	bool SIIEDSCutoff;
	bool SIVBEDSCutoff;
	bool AttRefFailMonitor;

	//Common Saturn Failures
	bool PlatformFailure;
	double PlatformFailureTime;
};

class EDS1B : public EDS
{
public:
	EDS1B(IU *iu);
	void Timestep(double simdt);
	void LVIndicatorsOff();
	bool ThrustCommitEval();
protected:

	bool SIThrustNotOK[8];
	//Temporary variables, not relays
	bool ThrustOKSignal[8];
};

class EDSSV : public EDS
{
public:
	EDSSV(IU *iu);
	void Timestep(double simdt);
	void LVIndicatorsOff();
	bool ThrustCommitEval();
	void SetSIIEngineOutIndicationA() { SIIEngineOutIndicationA = true; }
	void SetSIIEngineOutIndicationB() { SIIEngineOutIndicationB = true; }
	double GetLVTankPressure(int n);
protected:
	//K21-1-5, K22-1-5
	bool SIThrustNotOK[5];
	// A11K1-5, A12K1-5
	bool SIIThrustNotOK[5];
	//Temporary variables, not relays
	bool ThrustOKSignal[5];
	//K69 (K223), K70 (K224)
	bool SIISIVBNotSeparated;

private:
	const int SIIEngInd[5] = { 2,4,1,3,5 };
};