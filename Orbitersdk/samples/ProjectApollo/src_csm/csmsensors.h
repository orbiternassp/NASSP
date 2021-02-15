/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2018

  CSM Specific Sensors (Header)

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

#include "PanelSDK/Internals/Esystems.h"
#include "PanelSDK/Internals/Hsystems.h"

class Saturn;

class CSMTankTransducer : public Transducer
{
public:
	CSMTankTransducer(char *i_name, double minIn, double maxIn);
	void Init(e_object *e, h_Tank *t);
protected:
	h_Tank *tank;
};

class CSMTankTempTransducer : public CSMTankTransducer
{
public:
	CSMTankTempTransducer(char *i_name, double minIn, double maxIn);
	double GetValue();
};

class CSMTankPressTransducer : public CSMTankTransducer
{
public:
	CSMTankPressTransducer(char *i_name, double minIn, double maxIn);
	double GetValue();
};

class CSMCO2PressTransducer : public CSMTankTransducer
{
public:
	CSMCO2PressTransducer(char *i_name, double minIn, double maxIn);
	double Voltage();
	double GetValue();
};

class CSMTankQuantityTransducer : public CSMTankTransducer
{
public:
	CSMTankQuantityTransducer(char *i_name, double minIn, double maxIn, double tm);
	double GetValue();
protected:
	double totalMass;
};

class CSMDeltaPressTransducer : public Transducer
{
public:
	CSMDeltaPressTransducer(char *i_name, double minIn, double maxIn);
	void Init(e_object *e, h_Tank *t1, h_Tank *t2);
protected:
	h_Tank *tank1;
	h_Tank *tank2;
};

class CSMDeltaPressINH2OTransducer : public CSMDeltaPressTransducer
{
public:
	CSMDeltaPressINH2OTransducer(char *i_name, double minIn, double maxIn);
	double GetValue();
};

class CSMDeltaPressPSITransducer : public CSMDeltaPressTransducer
{
public:
	CSMDeltaPressPSITransducer(char *i_name, double minIn, double maxIn);
	double GetValue();
};

class CSMEvaporatorTransducer : public Transducer
{
public:
	CSMEvaporatorTransducer(char *i_name, double minIn, double maxIn);
	void Init(e_object *e, h_Evaporator *ev);
protected:
	h_Evaporator *evap;
};

class CSMEvaporatorPressTransducer : public CSMEvaporatorTransducer
{
public:
	CSMEvaporatorPressTransducer(char *i_name, double minIn, double maxIn);
	double GetValue();
};

class CSMPipeFlowTransducer : public Transducer
{
public:
	CSMPipeFlowTransducer(char *i_name, double minIn, double maxIn);
	void Init(e_object *e, h_Pipe *p);
	double GetValue();
protected:
	h_Pipe *pipe;
};