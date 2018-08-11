/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

MCC for Mission F (Header)

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

// MISSION STATES: MISSION F
#define MST_F_INSERTION		10
// Ends at TLI Update
#define MST_F_EPO1			11
// Ends at TLI BURN
#define MST_F_TRANSLUNAR1	21
// Ends at separation
#define MST_F_TRANSLUNAR2	22
// Ends at evasive maneuver update
#define MST_F_TRANSLUNAR3	23
// Ends at TB8 enable
#define MST_F_TRANSLUNAR4	24
// Ends at Block Data 1 update
#define MST_F_TRANSLUNAR5	25
// Ends at PTC REFSMMAT update
#define MST_F_TRANSLUNAR6	26
// Ends at MCC-1 update
#define MST_F_TRANSLUNAR7	27
// Ends at Block Data 2 update
#define MST_F_TRANSLUNAR8	28
// Ends at MCC-2 update
#define MST_F_TRANSLUNAR9	29
// Ends at Flyby PAD update
#define MST_F_TRANSLUNAR10	30
// Ends at State Vector Update
#define MST_F_TRANSLUNAR11	31
// Ends at MCC-3 update
#define MST_F_TRANSLUNAR12	32
// Ends at MCC-4 update
#define MST_F_TRANSLUNAR13	33
// Ends at PC+2 update
#define MST_F_TRANSLUNAR14	34
// Ends at Preliminary LOI-1 update
#define MST_F_TRANSLUNAR15	35
// Ends at TEI-1 update
#define MST_F_TRANSLUNAR16	36
// Ends at TEI-4 update
#define MST_F_TRANSLUNAR17	37
// Ends at rev 1 map update
#define MST_F_TRANSLUNAR18	38
// Ends at LOI-1 update
#define MST_F_TRANSLUNAR19	39
// Ends at rev 2 map update
#define MST_F_LUNAR_ORBIT_LOI_DAY_1		100
// Ends at LOI-2 update
#define MST_F_LUNAR_ORBIT_LOI_DAY_2		101
// Ends at TEI-5 update
#define MST_F_LUNAR_ORBIT_LOI_DAY_3		102
// Ends at rev 3 map update
#define MST_F_LUNAR_ORBIT_LOI_DAY_4		103
// Ends at F-1 landmark tracking rev 4 update
#define MST_F_LUNAR_ORBIT_LOI_DAY_5		104
// Ends at B-1 landmark tracking rev 4 update
#define MST_F_LUNAR_ORBIT_LOI_DAY_6		105
// Ends at rev 4 map update
#define MST_F_LUNAR_ORBIT_LOI_DAY_7		106
// Ends at state vector update
#define MST_F_LUNAR_ORBIT_LOI_DAY_8		107
// Ends at state vector update
#define MST_F_LUNAR_ORBIT_LOI_DAY_9		108
// Ends at TEI-10 update
#define MST_F_LUNAR_ORBIT_LOI_DAY_10	109
// Ends at LLS-2 update
#define MST_F_LUNAR_ORBIT_DOI_DAY_1		200
// Ends at LLS-2 track PAD update
#define MST_F_LUNAR_ORBIT_DOI_DAY_2		201
// Ends at rev 11 map update
#define MST_F_LUNAR_ORBIT_DOI_DAY_3		202
// Ends at CSM DAP update
#define MST_F_LUNAR_ORBIT_DOI_DAY_4		203
// Ends at LM DAP Load PAD update
#define MST_F_LUNAR_ORBIT_DOI_DAY_5		204
// Ends at LM map update
#define MST_F_LUNAR_ORBIT_DOI_DAY_6		205
// Ends at LM S-Band pointing angles update
#define MST_F_LUNAR_ORBIT_DOI_DAY_7		206
// Ends at LM IMU gyro torquing angle update
#define MST_F_LUNAR_ORBIT_DOI_DAY_8		207
// Ends at LGC activation update
#define MST_F_LUNAR_ORBIT_DOI_DAY_9		208
// Ends at separation update
#define MST_F_LUNAR_ORBIT_DOI_DAY_10	209
// Ends at AGS K Factor update
#define MST_F_LUNAR_ORBIT_DOI_DAY_11	210
// Ends at DOI update
#define MST_F_LUNAR_ORBIT_DOI_DAY_12	211
// Ends at PDI Abort update
#define MST_F_LUNAR_ORBIT_DOI_DAY_13	212
// Ends at Phasing update
#define MST_F_LUNAR_ORBIT_DOI_DAY_14	213
// Ends at LGC CSM state vector update
#define MST_F_LUNAR_ORBIT_DOI_DAY_15	214
// Ends at CMC CSM+LM state vector update
#define MST_F_LUNAR_ORBIT_DOI_DAY_16	215
// Ends at final phasing update
#define MST_F_LUNAR_ORBIT_DOI_DAY_17	216
// Ends at CSM backup insertion update
#define MST_F_LUNAR_ORBIT_DOI_DAY_18	217
// Ends at LM insertion update
#define MST_F_LUNAR_ORBIT_DOI_DAY_19	218
// Ends at CSM backup insertion update
#define MST_F_LUNAR_ORBIT_DOI_DAY_20	219
// Ends at LM insertion update
#define MST_F_LUNAR_ORBIT_DOI_DAY_21	220
// Ends at CMC LM state vector update
#define MST_F_LUNAR_ORBIT_DOI_DAY_22	221
// Ends at CSI update
#define MST_F_LUNAR_ORBIT_DOI_DAY_23	222
// Ends at APS depletion update
#define MST_F_LUNAR_ORBIT_DOI_DAY_24	223
// Ends at TEI-22 update
#define MST_F_LUNAR_ORBIT_DOI_DAY_25	224
// Ends at APS depletion burn
#define MST_F_LUNAR_ORBIT_DOI_DAY_26	225
// Ends at state vector update
#define MST_F_LUNAR_ORBIT_DOI_DAY_27	226
// Ends at rev 22 map update
#define MST_F_LUNAR_ORBIT_DOI_DAY_28	227
// Ends at LLS 2 photo PAD update
#define MST_F_LUNAR_ORBIT_DOI_DAY_29	228
// Ends at rev 23 map update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_1	300
// Ends at TEI-23 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_2	301
// Ends at state vector update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_3	302
// Ends at strip photo update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_4	303
// Ends at state vector update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_5	304
// Ends at landmark tracking rev 24 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_6	305
// Ends at TEI-24 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_7	306
// Ends at rev 24 map update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_8	307
// Ends at state vector update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_9	308
// Ends at TEI-25 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_10	309
// Ends at landmark tracking rev 25 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_11	310
// Ends at rev 25 map update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_12	311
// Ends at state vector update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_13	312
// Ends at TEI-26 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_14	313
// Ends at landmark tracking rev 26 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_15	314
// Ends at rev 26 map update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_16	315
// Ends at state vector update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_17	316
// Ends at TEI-27 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_18	317
// Ends at landmark tracking rev 27 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_19	318
// Ends at rev 27 map update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_20	319
// Ends at TEI-29 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_21	320
// Ends at rev 29 map update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_22	321
// Ends at LLS 3 photo update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_23	322
// Ends at state vector update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_24	323
// Ends at state vector update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_25	324
// Ends at TEI-30 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_26	325
// Ends at landmark tracking rev 30 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_27	326
// Ends at rev 30 map update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_28	327
// Ends at preliminary TEI-31 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_29	328
// Ends at rev 31 map update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_30	329
// Ends at strip photo update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_31	330
// Ends at state vector update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_32	331
// Ends at final TEI-31 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_33	332
// Ends at TEI-32 update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_34	333
// Ends at TEI map update
#define MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_35	334
// Ends at PTC REFSMMAT update
#define MST_F_TRANSEARTH_1	400
// Ends at state vector update
#define MST_F_TRANSEARTH_2	401
// Ends at state vector update
#define MST_F_TRANSEARTH_3	402
// Ends at MCC-5 update
#define MST_F_TRANSEARTH_4	403
// Ends at preliminary MCC-6 update
#define MST_F_TRANSEARTH_5	404
// Ends at Entry PAD update
#define MST_F_TRANSEARTH_6	405
// Ends at MCC-6 update
#define MST_F_TRANSEARTH_7	406
// Ends at Entry PAD update
#define MST_F_TRANSEARTH_8	407
// Ends at MCC-7 decision update
#define MST_F_TRANSEARTH_9	408
// Ends at MCC-7 update
#define MST_F_TRANSEARTH_10	409
// Ends at Entry PAD update
#define MST_F_TRANSEARTH_11	410
// Ends at final entry update
#define MST_F_TRANSEARTH_12	411
// Ends at CM/SM separation
#define MST_F_TRANSEARTH_13	412