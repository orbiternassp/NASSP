/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

MCC for Mission C Prime (Header)

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

// MISSION STATES: MISSION C PRIME
#define MST_CP_INSERTION	10
// Ends at TLI prediction
#define MST_CP_EPO1			11
// Ends at TLI update
#define MST_CP_EPO2			12
#define MST_CP_EPO3			13
#define MST_CP_EPO4			14
// Ends at TLI BURN
#define MST_CP_TRANSLUNAR1	21
// Ends at separation
#define MST_CP_TRANSLUNAR2	22
//Ends at Block Data 1 Update
#define MST_CP_TRANSLUNAR3	23
//Ends at MCC1 Update
#define MST_CP_TRANSLUNAR4	24
//Ends at Block Data 2 Update
#define MST_CP_TRANSLUNAR5	25
//Ends at Block Data 3 Update
#define MST_CP_TRANSLUNAR6	26
//Ends at MCC2 Update
#define MST_CP_TRANSLUNAR7	27
//Ends at Block Data 4 Update
#define MST_CP_TRANSLUNAR8	28
//Ends at Block Data 5 Update
#define MST_CP_TRANSLUNAR9	29
//Ends at MCC3 Update
#define MST_CP_TRANSLUNAR10	30
//Ends at Block Data 6 Update
#define MST_CP_TRANSLUNAR11	31
//Ends at MCC4 Update
#define MST_CP_TRANSLUNAR12	32
//Ends at Preliminary LOI-1 Update
#define MST_CP_TRANSLUNAR13	33
//Ends at PC+2 Update
#define MST_CP_TRANSLUNAR14	34
//Ends at Fast PC+2 Update
#define MST_CP_TRANSLUNAR15	35
//Ends at Preliminary TEI-1 Update
#define MST_CP_TRANSLUNAR16	36
//Ends at Preliminary TEI-2 Update
#define MST_CP_TRANSLUNAR17	37
//Ends at Map Update
#define MST_CP_TRANSLUNAR18	38
//Ends at LOI-1 Update
#define MST_CP_TRANSLUNAR19	39
//Ends at LOI-2 Update

#define MST_CP_LUNAR_ORBIT1	101
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT2	102
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT3	103
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT4	104
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT5	105
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT6	106
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT7	107
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT8	108
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT9	109
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT10 110
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT11 111
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT12 112
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT13 113
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT14 114
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT15 115
#define MST_CP_LUNAR_ORBIT16 116
#define MST_CP_LUNAR_ORBIT17 117
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT18 118
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT19 119
// Ends at TEI BURN

#define MST_CP_TRANSEARTH1	201
//Ends at Entry REFSMMAT update
#define MST_CP_TRANSEARTH2	202
//Ends at MCC5 Update
#define MST_CP_TRANSEARTH3	203
//Ends at MCC6 Update
#define MST_CP_TRANSEARTH4	204
//Ends at Preliminary MCC7 Update
#define MST_CP_TRANSEARTH5	205
//Ends at Preliminary Entry Update
#define MST_CP_TRANSEARTH6	206
//Ends at MCC7 Update
#define MST_CP_TRANSEARTH7	207
//Ends at Entry Update
#define MST_CP_TRANSEARTH8	208
//Ends at Final Entry Update

// Ends at entry interface, goes to entry.
#define MST_CP_ABORT_ORBIT	300
// Abort from earth orbit, goes to MST_ORBIT_ENTRY
#define MST_CP_ABORT		301
// post-TLI abort, ends at abort burn (if any)
// goes to MST_CP_TRANSEARTH with an abort flag.