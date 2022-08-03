/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2022

  Project Apollo Network Multiplexer

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

  See https://github.com/orbiternassp/NASSP/blob/Orbiter2016/NASSP-LICENSE.txt 
  for more details.

  **************************************************************************/

#ifndef PANETWORKMULTIPLEXER_H
#define PANETWORKMULTIPLEXER_H

#define ORBITER_MODULE
#define WIN32_LEAN_AND_MEAN //aparently this is needed for winsock2

#include "OrbiterSDK.h"
#include "thread"
#include "mutex"

#include <winsock2.h>



class ProjectApolloNetworkMultiplexer: public oapi::Module{
public:
	ProjectApolloNetworkMultiplexer(HINSTANCE HModule);
	~ProjectApolloNetworkMultiplexer();

private:

	uint8_t CSMTelemetryBuffer[1024];
	uint8_t LMTelemetryBuffer[1024];

	struct Ports {
		unsigned int CSM;
		unsigned int LM;
		unsigned int CSMPlayback;
		unsigned int LMPlayback;
	};

	Ports ports;

	enum ConnectType {
		CSMTelemetry,
		LMTelemetry,
		CSMPlayback,
		LMPlayback,
	};

	void ConnectToHost(ConnectType);

	void clbkPostStep(double simt, double simdt, double mjd);
};

#endif