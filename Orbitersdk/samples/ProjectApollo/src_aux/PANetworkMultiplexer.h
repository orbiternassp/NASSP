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


#include "OrbiterSDK.h"

class ProjectApolloNetworkMultiplexer: public oapi::Module{
public:
	ProjectApolloNetworkMultiplexer(HINSTANCE HModule);
	~ProjectApolloNetworkMultiplexer();
	

};

#endif