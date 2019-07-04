/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

IU Control Distributor 603A2 (Header)

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

class IUControlDistributor
{
public:
	IUControlDistributor(IU *iu);
	virtual ~IUControlDistributor() {}

	virtual void Timestep(double simdt);
	virtual void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	virtual void LoadState(FILEHANDLE scn, char *end_str) = 0;
	void LoadState(char *line);

	virtual bool GetSIBurnMode() = 0;
	virtual bool GetSIIBurnMode() { return false; }
	virtual bool UseSICEngineCant() { return false; }

	//Switch selector functions
	void SetFCCSwitchPoint1On() { SwitchPoint1to5[0] = true; }
	void SetFCCSwitchPoint2On() { SwitchPoint1to5[1] = true; }
	void SetFCCSwitchPoint3On() { SwitchPoint1to5[2] = true; }
	void SetFCCSwitchPoint4On() { SwitchPoint1to5[3] = true; }
	void SetFCCSwitchPoint5On() { SwitchPoint1to5[4] = true; }
	void SetSIVBBurnModeA(bool set) { IsSIVBBurnModeA = set; }
	void SetSIVBBurnModeB(bool set) { IsSIVBBurnModeB = set; }
	void SetTwoEngOutAutoAbortInhibit() { TwoEngOutAutoAbortInhibit = true; }
	void SetExcessiveRatePYRAutoAbortInhibitEnable(bool set) { ExcessiveRatePYRAutoAbortInhibitEnable = set; }
	void SetExcessiveRateRollAutoAbortInhibitEnable(bool set) { ExcessiveRateRollAutoAbortInhibitEnable = set; }
	void SetExcessiveRatePYRAutoAbortInhibit(bool set) { ExcessiveRatePYRAutoAbortInhibit = set; }
	void SetExcessiveRateRollAutoAbortInhibit(bool set) { ExcessiveRateRollAutoAbortInhibit = set; }
	void SetTwoEngOutAutoAbortInhibitEnable() { TwoEngOutAutoAbortInhibitEnable = true; }
	void SetQBallPower(bool set) { QBallPowerOn = set; }

	//For FCC
	bool GetSIVBBurnMode();
	bool GetFCCSwitchPoint1() { return SwitchPoint1to5[0]; }
	bool GetFCCSwitchPoint2() { return SwitchPoint1to5[1]; }
	bool GetFCCSwitchPoint3() { return SwitchPoint1to5[2]; }
	bool GetFCCSwitchPoint4() { return SwitchPoint1to5[3]; }
	bool GetFCCSwitchPoint5() { return SwitchPoint1to5[4]; }
	virtual bool GetFCCSwitchPoint6() { return false; }

	//For EDS
	bool GetTwoEnginesOutAutoAbortInhibit();
	bool GetExcessiveRatePYRAutoAbortInhibit();
	bool GetExcessiveRateRollAutoAbortInhibit();
	bool GetGSECommandVehicleLiftoffIndicationInhibit() { return GSECommandVehicleLiftoffIndicationInhibit; }

	//For LVDA
	void SwitchSelector(int stage, int channel);

	//For DCS
	bool GetIUCommandSystemEnable() { return IUCommandSystemEnable; }

	//For GSE
	void SetFCCPower(bool set) { FCCPowerOn = set; }
	void SetControlSignalProcessorPowerOn(bool set) { ControlSignalProcessorPowerOn = set; }
	virtual void ResetBus1();
	virtual void ResetBus2();
	bool GetEDSEnableLogic();
protected:
	IU *iu;

	//Relays:

	//K4-1/2
	bool GSECommandVehicleLiftoffIndicationInhibit;
	//K6
	bool FCCPowerOn;
	//K14
	bool QBallPowerOn;
	//K15 (K113)
	bool ExcessiveRatePYRAutoAbortInhibitEnable;
	//K16 (K40)
	bool ExcessiveRatePYRAutoAbortInhibit;
	//K17 (K114)
	bool ExcessiveRateRollAutoAbortInhibitEnable;
	//K18 (K41)
	bool ExcessiveRateRollAutoAbortInhibit;
	//K19 (K115)
	bool TwoEngOutAutoAbortInhibitEnable;
	//K20 (K42)
	bool TwoEngOutAutoAbortInhibit;
	//K23
	bool IsSIVBBurnModeA;
	//K24 to K28
	bool SwitchPoint1to5[5];
	//K36
	bool IsSIVBBurnModeB;
	//K51, K52
	bool IUCommandSystemEnable;
	//A7K2/3
	bool SIVBThrustNotOK;
	bool ControlSignalProcessorPowerOn;
};

class IUControlDistributor1B : public IUControlDistributor
{
public:
	IUControlDistributor1B(IU *iu);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	//For FCC
	//Signal C
	bool GetSIBurnMode();
};

class IUControlDistributorSV : public IUControlDistributor
{
public:
	IUControlDistributorSV(IU *iu);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	//Switch selector functions
	void SetSICEngineCantAOn() { SICEngineCantA = true; }
	void SetSICEngineCantB(bool set) { SICEngineCantB = set; }
	void SetSICEngineCantC(bool set) { SICEngineCantC = set; }
	void SetSIIBurnModeEngineCantOff();
	void SetFCCSwitchPoint6On() { SwitchPoint6to9[0] = true; }
	void SetFCCSwitchPoint7On() { SwitchPoint6to9[1] = true; }
	void SetFCCSwitchPoint8On() { SwitchPoint6to9[2] = true; }
	void SetFCCSwitchPoint9On() { SwitchPoint6to9[3] = true; }

	//For GSE
	void ResetBus1();
	void ResetBus2();

	//For FCC
	//Signal C
	bool GetSIBurnMode();
	//Signal B
	bool GetSIIBurnMode();
	//Signal E
	bool UseSICEngineCant();
	bool GetFCCSwitchPoint6() { return SwitchPoint6to9[0]; }
	bool GetFCCSwitchPoint7() { return SwitchPoint6to9[1]; }
	bool GetFCCSwitchPoint8() { return SwitchPoint6to9[2]; }
	bool GetFCCSwitchPoint9() { return SwitchPoint6to9[3]; }
protected:
	//Relays:

	//K34
	bool IsSIIBurnMode;
	//K53
	bool SICEngineCantA;
	//K54
	bool SICEngineCantB;
	//K56
	bool SICOutboardEnginesCantInhibit;
	//A8K1 - A8K2, A8K4 - A8K5
	bool SwitchPoint6to9[4];
	//A8K6
	bool SICEngineCantC;
};