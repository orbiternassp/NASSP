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

class IUUmbilicalInterface;
class IU;

class IUUmbilical
{
public:
	IUUmbilical(IUUmbilicalInterface *ml);
	~IUUmbilical();

	bool IsIUUmbilicalConnected() { return IUUmbilicalConnected; }

	void Connect(IU* iu);
	void Disconnect();
	//Called by IU during a pad abort. Technically doesn't disconnect IU umbilical
	virtual void AbortDisconnect();

	//From ML to SLV
	void SetEDSLiftoffEnableA();
	void SetEDSLiftoffEnableB();
	void EDSLiftoffEnableReset();
	void SwitchFCCPowerOn();
	void SwitchFCCPowerOff();
	void SwitchQBallPowerOn();
	void SwitchQBallPowerOff();
	bool AllSIEnginesRunning();
	bool IsEDSUnsafe();

	//From SLV to ML
	virtual bool ESEGetCommandVehicleLiftoffIndicationInhibit();
	virtual bool ESEGetSICOutboardEnginesCantInhibit();
	virtual bool ESEGetSICOutboardEnginesCantSimulate();
	virtual bool ESEGetAutoAbortInhibit();
	virtual bool ESEGetGSEOverrateSimulate();
	virtual bool ESEGetEDSPowerInhibit();
	virtual bool ESEPadAbortRequest();
	virtual bool ESEGetThrustOKIndicateEnableInhibitA();
	virtual bool ESEGetThrustOKIndicateEnableInhibitB();
	virtual bool ESEEDSLiftoffInhibitA();
	virtual bool ESEEDSLiftoffInhibitB();
	virtual bool ESEAutoAbortSimulate();
	virtual bool ESEGetSIBurnModeSubstitute();
	virtual bool ESEGetGuidanceReferenceRelease();
protected:
	IU* iu;
	IUUmbilicalInterface* IuUmb;

	bool IUUmbilicalConnected;
};