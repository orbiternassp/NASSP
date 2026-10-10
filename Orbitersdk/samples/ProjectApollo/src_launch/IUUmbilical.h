/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

IU Umbilical (Header)

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

#include "connector.h"

class IU_ESE;

//IU ESE to IU connector
class IUESEToIUCommandConnector : public Connector
{
public:
	IUESEToIUCommandConnector();
	~IUESEToIUCommandConnector();

	//IU to IU ESE
	bool ReceiveMessage(Connector *from, ConnectorMessage &m);

	//IU ESE to IU
	void SetEDSLiftoffEnableA();
	void SetEDSLiftoffEnableB();
	void EDSLiftoffEnableReset();
	void SwitchFCCPowerOn();
	void SwitchFCCPowerOff();
	void SwitchQBallPowerOn();
	void SwitchQBallPowerOff();
	void SetControlSignalProcessorPower(bool set);
	void EDSGroupNo1Reset();
	void EDSGroupNo2Reset();
	bool AllSIEnginesRunning();
	bool IsEDSUnsafeA();
	bool IsEDSUnsafeB();
	bool GetEDSSCCutoff1();
	bool GetEDSSCCutoff2();
	bool GetEDSSCCutoff3();
	bool GetEDSAutoAbortBus();
	bool GetEDSExcessiveRollRateIndication();
	bool GetEDSExcessivePitchYawRateIndication();
	bool GetLVDCOutputRegisterDiscrete(int bit);
	bool FCCPowerIsOn();
	void SwitchSelector(int stage, int channel);
	void LVDCPrepareToLaunch();
	bool GetSCCutoffEnabledA();
	bool GetSCCutoffEnabledB();
	bool GetLiftoffEnableA();
	bool GetLiftoffEnableB();
	void GetEDSAbortCommandToSC(bool *abort);

	void SetIU_ESE(IU_ESE *iu_ese) { ourIU_ESE = iu_ese; };
protected:
	bool GetEDSSCCutoff(int n);
	IU_ESE *ourIU_ESE;
};