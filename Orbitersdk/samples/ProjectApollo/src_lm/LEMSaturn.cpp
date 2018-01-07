/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

LEM Saturn

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

#include "Orbitersdk.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "lm_channels.h"

#include "iu.h"
#include "s1bsystems.h"
#include "sivbsystems.h"

#include "LEM.h"
#include "LEMSaturn.h"

static MESHHANDLE hSat1stg1;
static MESHHANDLE hSat1stg1low;
static MESHHANDLE hSat1intstg;
static MESHHANDLE hSat1intstglow;
static MESHHANDLE hSat1stg2;
static MESHHANDLE hSat1stg2low;
static MESHHANDLE hSat1stg21;
static MESHHANDLE hSat1stg22;
static MESHHANDLE hSat1stg23;
static MESHHANDLE hSat1stg24;
static MESHHANDLE hNosecap;
static MESHHANDLE hastp;
static MESHHANDLE hastp2;
static MESHHANDLE hCOAStarget;
static MESHHANDLE hlm_1;

static SURFHANDLE exhaust_tex;

PARTICLESTREAMSPEC s1b_exhaust = {
	0,		// flag
	3.2,	// size
	7000,	// rate
	180.0,	// velocity
	0.15,	// velocity distribution
	0.33,	// lifetime
	4.0,	// growthrate
	0.0,	// atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC srb_exhaust = {
	0,		// flag
	2.75,	// size
	2000,	// rate
	60.0,	// velocity
	0.1,	// velocity distribution
	0.4,	// lifetime
	2.0,	// growthrate
	0.0,	// atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC solid_exhaust = {
	0, 0.5, 250, 35.0, 0.1, 0.15, 0.5, 1.0,
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

// "staging vent" particle streams
static PARTICLESTREAMSPEC stagingvent_spec = {
	0,		// flag
	2.5,	// size
	100,	// rate
	10,	    // velocity
	2,		// velocity distribution
	2,		// lifetime
	2.0,	// growthrate
	0.5,    // atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.1, 0.1,
	PARTICLESTREAMSPEC::ATM_FLAT, 0.1, 0.1
};

// "fuel venting" particle streams
static PARTICLESTREAMSPEC fuel_venting_spec = {
	0,		// flag
	0.8,	// size
	30,		// rate
	2,	    // velocity
	0.5,    // velocity distribution
	20,		// lifetime
	0.15,	// growthrate
	0.5,    // atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.6, 0.6,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

// "prelaunch tank venting" particle streams
static PARTICLESTREAMSPEC prelaunchvent_spec = {
	0,		// flag
	0.4,	// size
	200,	// rate
	2,	    // velocity
	0.6,    // velocity distribution
	0.5,	// lifetime
	0.2,	// growthrate
	0.9,    // atmslowdown
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.1, 0.1,
	PARTICLESTREAMSPEC::ATM_FLAT, 0.1, 0.1
};

LEMSaturn::LEMSaturn(OBJHANDLE hObj, int fmodel) : LEM(hObj, fmodel)
{

}

LEMSaturn::~LEMSaturn()
{

}