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

class LVRG;
class IUToLVCommandConnector;
class IUToCSMCommandConnector;

class EDS
{
public:
	EDS(LVRG &rg);
	virtual void Timestep(double simdt) = 0;
	void Configure(IUToLVCommandConnector *lvCommandConn, IUToCSMCommandConnector *commandConn);

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	void SetTwoEngOutAutoAbortInhibit(bool set) { TwoEngOutAutoAbortInhibit = set; }
	void SetExcessiveRatesAutoAbortInhibit(bool set) { ExcessiveRatesAutoAbortInhibit = set; }
	void SetEngineOutIndicationA(bool set) { EngineOutIndicationA = set; }
	void SetEngineOutIndicationB(bool set) { EngineOutIndicationB = set; }
	void SetRateGyroSCIndicationSwitchA(bool set) { RateGyroSCIndicationSwitchA = set; }
	void SetRateGyroSCIndicationSwitchB(bool set) { RateGyroSCIndicationSwitchB = set; }
	void SetLVEnginesCutoffEnable(bool set) { LVEnginesCutoffEnable = set; }
protected:
	LVRG &lvrg;

	IUToLVCommandConnector *lvCommandConnector;

	IUToCSMCommandConnector *commandConnector;

	//Common Relays:
	
	//K29
	bool AutoAbortInitiate;
	//K43
	bool TwoEngOutAutoAbortDeactivate;
	//K46
	bool ExcessRatesAutoAbortDeactivatePY;
	//K47
	bool ExcessRatesAutoAbortDeactivateR;

	bool TwoEngOutAutoAbortInhibit;
	bool ExcessiveRatesAutoAbortInhibit;
	bool LVEnginesCutoffEnable;
	bool RateGyroSCIndicationSwitchA;
	bool RateGyroSCIndicationSwitchB;
	bool EngineOutIndicationA;
	bool EngineOutIndicationB;
};

class EDS1B : public EDS
{
public:
	EDS1B(LVRG &rg);
	void Timestep(double simdt);
};

class EDSSV : public EDS
{
public:
	EDSSV(LVRG &rg);
	void Timestep(double simdt);
};