/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Saturn Emergency Detection System (Header)

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
	void SetSIEngineOutIndicationA(bool set) { SIEngineOutIndicationA = set; }
	void SetSIEngineOutIndicationB(bool set) { SIEngineOutIndicationB = set; }
	void SetSIVBEngineOutIndicationA(bool set) { SIVBEngineOutIndicationA = set; }
	void SetSIVBEngineOutIndicationB(bool set) { SIVBEngineOutIndicationB = set; }
	void SetLVEnginesCutoffEnable() { LVEnginesCutoffEnable = true; }
	void ResetAutoAbortRelays() { AutoAbortEnableRelayA = false; AutoAbortEnableRelayB = false; }
	void SetSIVBEngineCutoffDisabled() { SIVBEngineCutoffDisabled = true; }
	void SetSIVBRestartAlert(bool set) { SIVBRestartAlert = set; }

	//GSE Reset Buses
	void ResetBus1();
	void ResetBus2();

	//To spacecraft
	bool GetLiftoffCircuitA() { return LiftoffA; }
	bool GetLiftoffCircuitB() { return LiftoffB; }
	bool GetEDSAbort(int n);

	//To IU
	bool GetIULiftoff() { return IULiftoffRelay; }

	//GSE
	bool GetLiftoffEnableA() { return AutoAbortEnableRelayA; }
	bool GetLiftoffEnableB() { return AutoAbortEnableRelayB; }
	bool GetAutoAbort() { return AutoAbortBus; }
protected:
	IU* iu;

	bool LVEnginesCutoffVote();

	//Buses:

	//+6D91-93
	bool IUEDSBusPowered;
	//+6D95
	bool AutoAbortBus;

	//EDS Distributor Relays:
	
	//A6K1, A7K1, A8K3 (K43-1 - K43-3)
	bool TwoEngOutAutoAbortDeactivate;
	//K46-K48 (K46-1 - K46-3)
	bool ExcessRatesAutoAbortDeactivatePY;
	//K52-K54 (K47-1 - K71-3)
	bool ExcessRatesAutoAbortDeactivateR;
	//K19 (K19-1)
	bool LVEnginesCutoffEnable;
	bool SIEngineOutIndicationA;
	bool SIEngineOutIndicationB;
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
	bool LVEnginesCutoff1;
	//K41-1/2 (K20-2/4)
	bool LVEnginesCutoff2;
	//K42 (K20-3)
	bool LVEnginesCutoff3;
	//K76 (K232)
	bool SIVBRestartAlert;
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

	bool SIThrustOK[8];
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
protected:
	bool SIThrustOK[5];
	bool SIIThrustOK[5];

private:
	const int SIIEngInd[5] = { 2,4,1,3,5 };
};