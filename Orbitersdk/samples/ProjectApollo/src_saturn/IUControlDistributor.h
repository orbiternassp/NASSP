/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

IU Control Distributor (Header)

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
	void Timestep(double simdt);

	//Switch selector functions
	void SetSICEngineCantAOn() { SICEngineCantA = true; }
	void SetSICEngineCantB(bool set) { SICEngineCantB = set; }
	void SetSICEngineCantC(bool set) { SICEngineCantC = set; }
	void SetSIIBurnModeEngineCantOff();
	void SetSIVBBurnModeA(bool set) { IsSIVBBurnModeA = set; }
	void SetSIVBBurnModeB(bool set) { IsSIVBBurnModeB = set; }
	void SetFCCSwitchPoint1On() { SwitchPoint1 = true; }
	void SetFCCSwitchPoint2On() { SwitchPoint2 = true; }
	void SetFCCSwitchPoint3On() { SwitchPoint3 = true; }
	void SetFCCSwitchPoint4On() { SwitchPoint4 = true; }
	void SetFCCSwitchPoint5On() { SwitchPoint5 = true; }
	void SetFCCSwitchPoint6On() { SwitchPoint6 = true; }
	void SetFCCSwitchPoint7On() { SwitchPoint7 = true; }
	void SetFCCSwitchPoint8On() { SwitchPoint8 = true; }
	void SetFCCSwitchPoint9On() { SwitchPoint9 = true; }

	//GSE
	void ResetBus1();
	void ResetBus2();

	//For FCC
	//Signal C
	bool GetSIBurnMode();
	//Signal B
	bool GetSIIBurnMode();
	//Signal A
	bool GetSIVBBurnMode();
	//Signal E
	bool UseSICEngineCant();
	bool GetFCCSwitchPoint1() { return SwitchPoint1; }
	bool GetFCCSwitchPoint2() { return SwitchPoint2; }
	bool GetFCCSwitchPoint3() { return SwitchPoint3; }
	bool GetFCCSwitchPoint4() { return SwitchPoint4; }
	bool GetFCCSwitchPoint5() { return SwitchPoint5; }
	bool GetFCCSwitchPoint6() { return SwitchPoint6; }
	bool GetFCCSwitchPoint7() { return SwitchPoint7; }
	bool GetFCCSwitchPoint8() { return SwitchPoint8; }
	bool GetFCCSwitchPoint9() { return SwitchPoint9; }
protected:
	IU *iu;

	//Relays:

	//K4-1/2
	bool GSECommandVehicleLiftoffIndicationInhibit;
	//K23
	bool IsSIVBBurnModeA;
	//K24
	bool SwitchPoint1;
	//K25
	bool SwitchPoint2;
	//K26
	bool SwitchPoint3;
	//K27
	bool SwitchPoint4;
	//K28
	bool SwitchPoint5;
	//K34
	bool IsSIIBurnMode;
	//K36
	bool IsSIVBBurnModeB;
	//K53
	bool SICEngineCantA;
	//K54
	bool SICEngineCantB;
	//K56
	bool SICOutboardEnginesCantInhibit;
	//A7K2/3
	bool SIVBThrustNotOK;
	//A8K1
	bool SwitchPoint6;
	//A8K2
	bool SwitchPoint7;
	//A8K4
	bool SwitchPoint9;
	//A8K5
	bool SwitchPoint8;
	//A8K6
	bool SICEngineCantC;
};