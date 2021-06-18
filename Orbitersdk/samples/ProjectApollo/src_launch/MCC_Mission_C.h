/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

MCC for Mission C (Header)

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

// MISSION STATES: MISSION C
#define MST_C_INSERTION		10
// Ends at specified time, goes to
#define MST_C_SV_UPDATE1	11
#define MST_C_SEPARATION	12
// Ends at 1ST RDZ PHASING BURN, goes to
#define MST_C_DAY1STATE1	13
#define MST_C_DAY1STATE2	14
#define MST_C_DAY1STATE3	15
#define MST_C_DAY1STATE4	16
#define MST_C_DAY2STATE1	20
#define MST_C_DAY2STATE2	21
#define MST_C_DAY2STATE3	22
#define MST_C_DAY2STATE4	23
#define MST_C_DAY2STATE5	24
#define MST_C_DAY2STATE6	25
#define MST_C_DAY2STATE7	26
#define MST_C_DAY2STATE8	27
#define MST_C_DAY2STATE9	28
#define MST_C_DAY2STATE10	29
#define MST_C_DAY2STATE11	30
#define MST_C_DAY3STATE1	40
#define MST_C_DAY3STATE2	41
#define MST_C_DAY3STATE3	42
#define MST_C_DAY3STATE4	43
#define MST_C_DAY3STATE5	44
#define MST_C_DAY3STATE6	45
#define MST_C_DAY4STATE1	50
#define MST_C_DAY4STATE2	51
#define MST_C_DAY4STATE3	52
#define MST_C_DAY4STATE4	53
#define MST_C_DAY4STATE5	54
#define MST_C_DAY4STATE6	55
#define MST_C_DAY5STATE1	60
#define MST_C_DAY5STATE2	61
#define MST_C_DAY5STATE3	62
#define MST_C_DAY5STATE4	63
#define MST_C_DAY5STATE5	64
#define MST_C_DAY5STATE6	65
#define MST_C_DAY6STATE1	70
#define MST_C_DAY6STATE2	71
#define MST_C_DAY6STATE3	72
#define MST_C_DAY6STATE4	73
#define MST_C_DAY6STATE5	74
#define MST_C_DAY7STATE1	80
#define MST_C_DAY7STATE2	81
#define MST_C_DAY7STATE3	82
#define MST_C_DAY7STATE4	83
#define MST_C_DAY7STATE5	84
#define MST_C_DAY8STATE1	90
#define MST_C_DAY8STATE2	91
#define MST_C_DAY9STATE1	100
#define MST_C_DAY9STATE2	101
#define MST_C_DAY9STATE3	102
#define MST_C_DAY9STATE4	103
#define MST_C_DAY9STATE5	104
#define MST_C_DAY9STATE6	105
#define MST_C_DAY9STATE7	106
#define MST_C_DAY9STATE8	107
#define MST_C_DAY10STATE1	110
#define MST_C_DAY10STATE2	111
#define MST_C_DAY10STATE3	112
#define MST_C_DAY10STATE4	113
#define MST_C_DAY11STATE1	120
#define MST_C_DAY11STATE2	121
#define MST_C_DAY11STATE3	122
#define MST_C_DAY11STATE4	123
#define MST_C_ABORT			130
// Entered on abort from orbit, works like COAST13, goes to MST_ORBIT_ENTRY