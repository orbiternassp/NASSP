/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Signal-Conditioning Electronics Assembly (Header)

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

//Solid State Switch
class SCEA_SolidStateSwitch
{
public:
	SCEA_SolidStateSwitch();
	void Reset();
	void SetState(bool closed);
	bool IsClosed() { return isClosed; };
protected:
	bool isClosed;
};

//Sub Assembly 501-1 (DC Amplifier)
class SCEA_SA_5011
{
public:
	SCEA_SA_5011();
	void Reset();
	double GetVoltage(int chan) { return Output[chan - 1]; }
	void SetOutput(int n, double val) { Output[n - 1] = val; }
protected:
	double Output[4];
};

//Sub Assembly 502-2 (Attenuator)
class SCEA_SA_5022
{
public:
	SCEA_SA_5022();
	void Reset();
	double GetVoltage(int chan) { return Output[chan - 1]; }
	void SetOutput(int n, double val) { Output[n - 1] = val; }
protected:
	double Output[4];
};

//Sub Assembly 503-2 (AC to DC converter)
class SCEA_SA_5032
{
public:
	SCEA_SA_5032();
	void Reset();
	double GetVoltage(int chan) { return Output[chan - 1]; }
	void SetOutput(int n, double val) { Output[n - 1] = val; }
protected:
	double Output[3];
};

//Sub Assembly 504-1 (Analog buffer)
class SCEA_SA_5041
{
public:
	SCEA_SA_5041();
	void Reset();
	double GetVoltage(int chan) { return Output[chan - 1]; }
	void SetOutput(int n, double val) { Output[n - 1] = val; }
protected:
	double Output[4];
};

//Sub Assembly 504-2 (Discrete buffer)
class SCEA_SA_5042
{
public:
	SCEA_SA_5042();
	void Reset();
	double GetVoltage(int chan) { return Output[chan - 1]; }
	void SetOutput(int n, double val) { Output[n - 1] = val; }
protected:
	double Output[10];
};

//Sub Assembly 504-3 (Discrete buffer)
class SCEA_SA_5043
{
public:
	SCEA_SA_5043();
	void Reset();
	double GetVoltage(int chan) { return Output[chan - 1]; }
	void SetOutput(int n, double val) { Output[n - 1] = val; }
protected:
	double Output[12];
};

//Sub Assembly 504-4 (Discrete buffer)
class SCEA_SA_5044
{
public:
	SCEA_SA_5044();
	void Reset();
	double GetVoltage(int chan) { return Output[chan - 1]; }
	SCEA_SolidStateSwitch* GetSwitch(int chan) { return &SolidStateSwitch[chan - 1]; }
	void SetOutput(int n, bool val);
protected:
	double Output[12];
	SCEA_SolidStateSwitch SolidStateSwitch[12];
};

//Sub Assembly 504-5 (Discrete buffer)
class SCEA_SA_5045
{
public:
	SCEA_SA_5045();
	void Reset();
	double GetVoltage(int chan) { return Output[chan - 1]; }
protected:
	double Output[12];
};

//Sub Assembly 505-1 (Frequency to DC converter)
class SCEA_SA_5051
{
public:
	SCEA_SA_5051();
	void Reset();
	double GetVoltage(int chan) { return Output[chan - 1]; }
protected:
	double Output[3];
};

//Sub Assembly 506-2/3 (Resistance to DC converter)
class SCEA_SA_5062
{
public:
	SCEA_SA_5062();
	void Reset();
	double GetVoltage(int chan) { return Output[chan - 1]; }
protected:
	double Output[4];
};

//Sub Assembly 507-1 (Phase demodulator)
class SCEA_SA_5071
{
public:
	SCEA_SA_5071();
	void Reset();
	double GetVoltage(int chan) { return Output[chan - 1]; }
protected:
	double Output[4];
};

class LEM;

class SCERA
{
public:
	SCERA();
	void Init(LEM *l, e_object *dc);
	virtual void Timestep() = 0;
	virtual void SystemTimestep(double simdt) = 0;
	virtual double GetVoltage(int sa, int chan) = 0;
	virtual SCEA_SolidStateSwitch* GetSwitch(int sa, int chan) = 0;
	virtual void Reset() = 0;
	double scale_data(double data, double low, double high);
	bool IsPowered();
protected:
	LEM *lem;
	e_object *dcpower;
	bool Operate;
};

//Signal Conditioning Electronic Replaceable Assembly 1 (ERA-1)
class SCERA1 : public SCERA
{
public:
	SCERA1();
	void Reset();
	void Timestep();
	void SystemTimestep(double simdt);
	double GetVoltage(int sa, int chan);
	SCEA_SolidStateSwitch* GetSwitch(int sa, int chan);
protected:
	SCEA_SA_5042 SA2;
	SCEA_SA_5042 SA3;
	SCEA_SA_5042 SA4;
	SCEA_SA_5041 SA5;
	SCEA_SA_5041 SA6;
	SCEA_SA_5041 SA7;
	SCEA_SA_5041 SA8;
	SCEA_SA_5062 SA9;
	SCEA_SA_5062 SA10;
	SCEA_SA_5042 SA11;
	SCEA_SA_5044 SA12;
	SCEA_SA_5044 SA13;
	SCEA_SA_5045 SA14;
	SCEA_SA_5022 SA15;
	SCEA_SA_5051 SA16;
	SCEA_SA_5032 SA17;
	SCEA_SA_5041 SA18;
	SCEA_SA_5041 SA19;
	SCEA_SA_5062 SA20;
	SCEA_SA_5062 SA21;
};

//Signal Conditioning Electronic Replaceable Assembly 2 (ERA-2)
class SCERA2 : public SCERA
{
public:
	SCERA2();
	void Reset();
	void Timestep();
	void SystemTimestep(double simdt);
	double GetVoltage(int sa, int chan);
	SCEA_SolidStateSwitch* GetSwitch(int sa, int chan);
protected:
	SCEA_SA_5043 SA2;
	SCEA_SA_5043 SA3;
	SCEA_SA_5044 SA4;
	SCEA_SA_5045 SA5;
	SCEA_SA_5062 SA6;
	SCEA_SA_5032 SA7;
	SCEA_SA_5011 SA8;
	SCEA_SA_5071 SA9;
	SCEA_SA_5071 SA10;
	SCEA_SA_5044 SA12;
	SCEA_SA_5045 SA13;
	SCEA_SA_5045 SA14;
	SCEA_SA_5022 SA15;
	SCEA_SA_5022 SA16;
	SCEA_SA_5022 SA17;
	SCEA_SA_5022 SA18;
	SCEA_SA_5022 SA19;
	SCEA_SA_5062 SA20;
	SCEA_SA_5062 SA21;
};