/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

MCC for Mission G (Header)

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

// MISSION STATES: MISSION G

//Ground liftoff time update to TLI Simulation
#define MST_G_INSERTION		10
//TLI Simulation to TLI+90 PAD
#define MST_G_EPO1			11
//TLI+90 Maneuver PAD to TLI+5h P37 PAD
#define MST_G_EPO2			12
#define MST_G_EPO3			13
#define MST_G_EPO4			14
#define MST_G_TRANSLUNAR1	20
#define MST_G_TRANSLUNAR2	21
#define MST_G_TRANSLUNAR3	22
#define MST_G_TRANSLUNAR4	23
#define MST_G_TRANSLUNAR5	24
#define MST_G_TRANSLUNAR6	25
#define MST_G_TRANSLUNAR_NO_MCC1_1	26
#define MST_G_TRANSLUNAR_NO_MCC1_2	27
#define MST_G_TRANSLUNAR7	28
#define MST_G_TRANSLUNAR8	29
#define MST_G_TRANSLUNAR9	30