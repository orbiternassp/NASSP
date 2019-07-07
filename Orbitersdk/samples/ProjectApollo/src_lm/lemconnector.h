/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

LM connector classes (Header)

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

class LEM;
class LEMcomputer;

class LEMConnector : public Connector
{
public:
	LEMConnector(LEM *l);
	~LEMConnector();

	void SetLEM(LEM *lem) { OurVessel = lem; };

protected:
	LEM *OurVessel;
};

class LEMECSConnector : public LEMConnector
{
public:
	LEMECSConnector(LEM *l);
	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

// ELECTRICAL
// LEM to CSM Power Connector
class LEMPowerConnector : public LEMConnector
{
public:
	LEMPowerConnector(LEM *l);
	int csm_power_latch;
	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

enum IULMMessageType
{
	IULM_SET_INPUT_CHANNEL_BIT,			///< Set an AGC input channel bit value.
	IULM_PLAY_COUNT_SOUND,				///< Play/stop countdown sound.
	IULM_PLAY_SEPS_SOUND,				///< Play/stop Seperation sound.
};

///
/// \ingroup Connectors
/// \brief LM to IU connector type.
///
class LMToIUConnector : public LEMConnector
{
public:
	LMToIUConnector(LEMcomputer &c, LEM *l);
	~LMToIUConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);

protected:
	LEMcomputer & agc;
};

///
/// \ingroup Connectors
/// \brief LM to S-IVB connector type.
///
class LMToSIVBConnector : public LEMConnector
{
public:
	LMToSIVBConnector(LEM *l);
	~LMToSIVBConnector();

	void StartSeparationPyros();
	void StopSeparationPyros();
};

//CSM to LM command connector

class LEMCommandConnector : public LEMConnector
{
public:
	LEMCommandConnector(LEM *l);
	~LEMCommandConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};