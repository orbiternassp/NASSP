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
#define MST_G_TRANSLUNAR7	26
#define MST_G_TRANSLUNAR_NO_MCC1_1	27
#define MST_G_TRANSLUNAR_NO_MCC1_2	28
#define MST_G_TRANSLUNAR8	29
#define MST_G_TRANSLUNAR9	30
#define MST_G_TRANSLUNAR10	31
#define MST_G_TRANSLUNAR11	32
#define MST_G_TRANSLUNAR12	33
#define MST_G_TRANSLUNAR13	34
#define MST_G_TRANSLUNAR14	35
#define MST_G_TRANSLUNAR15	36
#define MST_G_TRANSLUNAR16	37
#define MST_G_TRANSLUNAR17	38
#define MST_G_TRANSLUNAR18	39
#define MST_G_LUNAR_ORBIT_LOI_DAY_1 100
#define MST_G_LUNAR_ORBIT_LOI_DAY_2 101
#define MST_G_LUNAR_ORBIT_LOI_DAY_3 102
#define MST_G_LUNAR_ORBIT_LOI_DAY_4 103
#define MST_G_LUNAR_ORBIT_LOI_DAY_5 104
#define MST_G_LUNAR_ORBIT_LOI_DAY_6 105
#define MST_G_LUNAR_ORBIT_PDI_DAY_1 110
#define MST_G_LUNAR_ORBIT_PDI_DAY_2 111
#define MST_G_LUNAR_ORBIT_PDI_DAY_3 112
#define MST_G_LUNAR_ORBIT_PDI_DAY_4 113
#define MST_G_LUNAR_ORBIT_PDI_DAY_5 114
#define MST_G_LUNAR_ORBIT_PDI_DAY_6 115
#define MST_G_LUNAR_ORBIT_PDI_DAY_7 116
#define MST_G_LUNAR_ORBIT_PDI_DAY_8 117
#define MST_G_LUNAR_ORBIT_PDI_DAY_9 118
#define MST_G_LUNAR_ORBIT_PDI_DAY_10 119
#define MST_G_LUNAR_ORBIT_PDI_DAY_11 120
#define MST_G_LUNAR_ORBIT_PDI_DAY_12 121
#define MST_G_LUNAR_ORBIT_PDI_DAY_13 122
#define MST_G_LUNAR_ORBIT_PDI_DAY_14 123
#define MST_G_LUNAR_ORBIT_PDI_DAY_15 124
#define MST_G_LUNAR_ORBIT_PRE_DOI_1 130
#define MST_G_LUNAR_ORBIT_PRE_DOI_2 131
#define MST_G_LUNAR_ORBIT_PRE_DOI_3 132
#define MST_G_LUNAR_ORBIT_PRE_DOI_4 133
#define MST_G_LUNAR_ORBIT_PRE_PDI_1 134
#define MST_G_LUNAR_ORBIT_PRE_LANDING_1 135
#define MST_G_LUNAR_ORBIT_POST_LANDING_1 140
#define MST_G_LUNAR_ORBIT_POST_LANDING_2 141
#define MST_G_LUNAR_ORBIT_POST_LANDING_3 142
#define MST_G_LUNAR_ORBIT_POST_LANDING_4 143
#define MST_G_LUNAR_ORBIT_POST_LANDING_5 144
#define MST_G_LUNAR_ORBIT_POST_LANDING_6 145
#define MST_G_LUNAR_ORBIT_POST_LANDING_7 146
#define MST_G_LUNAR_ORBIT_POST_LANDING_8 147
#define MST_G_LUNAR_ORBIT_NO_PLANE_CHANGE_1 148
#define MST_G_LUNAR_ORBIT_PLANE_CHANGE_1 149
#define MST_G_LUNAR_ORBIT_PLANE_CHANGE_2 150
#define MST_G_LUNAR_ORBIT_EVA_DAY_1 151
#define MST_G_LUNAR_ORBIT_EVA_DAY_2 152
#define MST_G_LUNAR_ORBIT_EVA_DAY_3 153
#define MST_G_LUNAR_ORBIT_EVA_DAY_4 154
#define MST_G_LUNAR_ORBIT_ASCENT_DAY_1 160
#define MST_G_LUNAR_ORBIT_ASCENT_DAY_2 161
#define MST_G_LUNAR_ORBIT_ASCENT_DAY_3 162
#define MST_G_LUNAR_ORBIT_ASCENT_DAY_4 163
#define MST_G_LUNAR_ORBIT_ASCENT_DAY_5 164
#define MST_G_LUNAR_ORBIT_ASCENT_DAY_6 165
#define MST_G_LUNAR_ORBIT_ASCENT_DAY_7 166
#define MST_G_LUNAR_ORBIT_ASCENT_DAY_8 167
#define MST_G_LUNAR_ORBIT_ASCENT_DAY_9 168
#define MST_G_LUNAR_ORBIT_ASCENT_DAY_10 169
#define MST_G_LUNAR_ORBIT_ASCENT_DAY_11 170
#define MST_G_LUNAR_ORBIT_ASCENT_DAY_12 171
#define MST_G_TRANSEARTH_1 200
#define MST_G_TRANSEARTH_2 201
#define MST_G_TRANSEARTH_3 202
#define MST_G_TRANSEARTH_4 203
#define MST_G_TRANSEARTH_5 204
#define MST_G_TRANSEARTH_6 205
#define MST_G_TRANSEARTH_7 206
#define MST_G_TRANSEARTH_8 207
#define MST_G_TRANSEARTH_9 208
#define MST_G_TRANSEARTH_10 209
#define MST_G_TRANSEARTH_11 210
#define MST_G_LUNAR_ORBIT_PRE_PDI2_1 300