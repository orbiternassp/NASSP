/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

MCC sequencing for Mission D

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
#include "saturn.h"
#include "saturnv.h"
#include "LVDC.h"
#include "sivb.h"
#include "mcc.h"
#include "rtcc.h"
#include "MCC_Mission_D.h"
#include "iu.h"

void MCC::MissionSequence_D()
{
	switch (MissionState)
	{
	case MST_D_INSERTION:	//SV Update to Separation
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(3.0 * 60.0 * 60.0 + 15.0 * 60.0), 7, MST_D_DAY1SVUPDATE);
		break;
	case MST_D_DAY1SVUPDATE:
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(4 * 60 * 60 + 6 * 60), 2, MST_D_SEPARATION);
		break;
	case MST_D_SEPARATION:	//Separation to SPS-1

		switch (SubState) {
		case 0:
			if (cm->GetStage() >= CSM_LEM_STAGE)
			{
				setSubState(1);
			}
			break;
		case 1:

			if (SubStateTime > 5.0*60.0 && rtcc->GETEval2(4 * 60 * 60 + 25 * 60))
			{
				if (sivb == NULL)
				{
					VESSEL *v;
					OBJHANDLE hLV;
					hLV = oapiGetObjectByName(LVName);
					if (hLV != NULL)
					{
						v = oapiGetVesselInterface(hLV);

						if (!stricmp(v->GetClassName(), "ProjectApollo\\sat5stg3") ||
							!stricmp(v->GetClassName(), "ProjectApollo/sat5stg3")) {
							sivb = (SIVB *)v;
						}
					}
				}

				sivb->GetIU()->GetDCS()->Uplink(DCSUPLINK_REMOVE_INHIBIT_MANEUVER4, NULL);

				setSubState(2);
			}

			break;
		case 2:
			if (SubStateTime > 5.0*60.0)
			{
				sivb->GetIU()->GetDCS()->Uplink(DCSUPLINK_RESTART_MANEUVER_ENABLE, NULL);

				setSubState(3);
			}
			break;
		case 3:
			if (rtcc->GETEval2(4 * 60 * 60 + 50 * 60))
			{
				SlowIfDesired();
				setState(MST_D_DAY1STATE1);
			}
			break;
		}
		break;
	case MST_D_DAY1STATE1:	//SPS-1 to Daylight Star Check
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, rtcc->GETEval2(6 * 60 * 60 + 15 * 60), 10, MST_D_DAY1STATE2);
		break;
	case MST_D_DAY1STATE2: //Daylight Star Check to SV Update
		UpdateMacro(UTP_PADONLY, PT_STARCHKPAD, rtcc->GETEval2(7 * 60 * 60 + 20 * 60), 9, MST_D_DAY1STATE3);
		break;
	case MST_D_DAY1STATE3: //SV Update to Block Data 2
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(8 * 60 * 60 + 27 * 60), 2, MST_D_DAY1STATE4);
		break;
	case MST_D_DAY1STATE4: //Block Data 2 to Block Data 3
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(19 * 60 * 60 + 15 * 60), 11, MST_D_DAY2STATE1);
		break;
	case MST_D_DAY2STATE1: //Block Data 3 to SPS-2
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(20 * 60 * 60 + 37 * 60), 12, MST_D_DAY2STATE2);
		break;
	case MST_D_DAY2STATE2: //SPS-2 to SPS-3
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, rtcc->GETEval2(23 * 60 * 60 + 55 * 60), 13, MST_D_DAY2STATE3);
		break;
	case MST_D_DAY2STATE3: //SPS-3 to SPS-4
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, rtcc->GETEval2(26 * 60 * 60 + 50 * 60), 14, MST_D_DAY2STATE4);
		break;
	case MST_D_DAY2STATE4: //SPS-4 to SV Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, rtcc->GETEval2(28 * 60 * 60 + 50 * 60), 15, MST_D_DAY2STATE5);
		break;
	case MST_D_DAY2STATE5: //SV Update to Block Data 4
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(28 * 60 * 60 + 55 * 60), 2, MST_D_DAY2STATE6);
		break;
	case MST_D_DAY2STATE6: //Block Data 4 to Block Data 5
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(40 * 60 * 60 + 10 * 60), 16, MST_D_DAY3STATE1);
		break;
	case MST_D_DAY3STATE1: //Block Data 5 to CMC Docked DPS Burn Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(41 * 60 * 60 + 10 * 60), 17, MST_D_DAY3STATE2);
		break;
	case MST_D_DAY3STATE2: //CMC Docked DPS Burn Update to LM AOT STAR OBS PAD
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, true, 18, MST_D_DAY3STATE3);
		break;
	case MST_D_DAY3STATE3: //LM AOT STAR OBS PAD to Block Data 6
		UpdateMacro(UTP_PADONLY, PT_AP9AOTSTARPAD, rtcc->GETEval2(47 * 60 * 60 + 10 * 60), 19, MST_D_DAY3STATE5);
		break;
	case MST_D_DAY3STATE5: //Block Data 6 to CMC state vector updates
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(48 * 60 * 60), 20, MST_D_DAY3STATE6);
		break;
	case MST_D_DAY3STATE6: //CMC state vector updates to LGC Docked DPS Burn Update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(48 * 60 * 60 + 10 * 60), 3, MST_D_DAY3STATE7);
		break;
	case MST_D_DAY3STATE7: //LGC Docked DPS Burn Update to LGC Gyro Torquing Angles
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11LMMNV, SubStateTime > 3.0*60.0, 21, MST_D_DAY3STATE8);
		break;
	case MST_D_DAY3STATE8: //LGC Gyro Torquing Angles to LGC Gyro Torquing Angles
		UpdateMacro(UTP_PADONLY, PT_TORQANG, rtcc->GETEval2(49 * 60 * 60 + 5 * 60), 22, MST_D_DAY3STATE9);
		break;
	case MST_D_DAY3STATE9: //LGC Gyro Torquing Angles to SPS-5
		UpdateMacro(UTP_PADONLY, PT_TORQANG, rtcc->GETEval2(52 * 60 * 60 + 50 * 60), 22, MST_D_DAY3STATE10);
		break;
	case MST_D_DAY3STATE10: //SPS-5 to SV Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, rtcc->GETEval2(55 * 60 * 60 + 30 * 60), 23, MST_D_DAY3STATE11);
		break;
	case MST_D_DAY3STATE11: //SV Update to Block Data 7
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(56 * 60 * 60 + 40 * 60), 2, MST_D_DAY3STATE12);
		break;
	case MST_D_DAY3STATE12: //Block Data 7 to Block Data 8
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(67 * 60 * 60 + 30 * 60), 24, MST_D_DAY4STATE1);
		break;
	case MST_D_DAY4STATE1: //Block Data 8 to EVA REFSMMAT Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(69 * 60 * 60 + 55 * 60), 25, MST_D_DAY4STATE2);
		break;
	case MST_D_DAY4STATE2: //EVA REFSMMAT Update to state vector update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(77 * 60 * 60 + 45 * 60), 26, MST_D_DAY4STATE3);
		break;
	case MST_D_DAY4STATE3: //State vector update to Block Data 9
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(78 * 60 * 60 + 10 * 60), 2, MST_D_DAY4STATE4);
		break;
	case MST_D_DAY4STATE4: //Block Data 9 to Block Data 10
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(87 * 60 * 60 + 15 * 60), 27, MST_D_DAY5STATE1);
		break;
	case MST_D_DAY5STATE1: //Block Data 10 to CSM Rendezvous REFSMMAT update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(89 * 60 * 60 + 5 * 60), 28, MST_D_DAY5STATE2);
		break;
	case MST_D_DAY5STATE2: //CSM Rendezvous REFSMMAT update to LM DAP update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(91 * 60 * 60), 29, MST_D_DAY5STATE3);
		break;
	case MST_D_DAY5STATE3: //LM DAP update to LM Rendezvous REFSMMAT update
		UpdateMacro(UTP_PADONLY, PT_AP10DAPDATA, rtcc->GETEval2(91 * 60 * 60 + 10 * 60), 6, MST_D_DAY5STATE4);
		break;
	case MST_D_DAY5STATE4: //LM Rendezvous REFSMMAT update to gyro torquing angles update
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP7NAV, rtcc->GETEval2(91 * 60 * 60 + 15 * 60), 30, MST_D_DAY5STATE5);
		break;
	case MST_D_DAY5STATE5: //Gyro torquing angles update to Phasing update
		UpdateMacro(UTP_PADONLY, PT_TORQANG, rtcc->GETEval2(92 * 60 * 60 + 5 * 60), 31, MST_D_DAY5STATE6);
		break;
	case MST_D_DAY5STATE6: //Phasing update to TPI0 update
		UpdateMacro(UTP_PADONLY, PT_AP11LMMNV, rtcc->GETEval2(94 * 60 * 60 + 30 * 60), 32, MST_D_DAY5STATE7);
		break;
	case MST_D_DAY5STATE7: //TPI0 update to Insertion update
		UpdateMacro(UTP_PADONLY, PT_AP9LMTPI, rtcc->GETEval2(95 * 60 * 60 + 10 * 60), 33, MST_D_DAY5STATE8);
		break;
	case MST_D_DAY5STATE8: //Insertion update to CSI update
		UpdateMacro(UTP_PADONLY, PT_AP11LMMNV, rtcc->GETEval2(rtcc->calcParams.Insertion + 10.0*60.0), 34, MST_D_DAY5STATE9);
		break;
	case MST_D_DAY5STATE9: //CSI update to CDH update
		UpdateMacro(UTP_PADONLY, PT_AP10CSI, rtcc->GETEval2(rtcc->calcParams.CSI + 15.0*60.0), 35, MST_D_DAY5STATE10);
		break;
	case MST_D_DAY5STATE10: //CDH update to TPI update
		UpdateMacro(UTP_PADONLY, PT_AP9LMCDH, rtcc->GETEval2(rtcc->calcParams.CDH + 15.0*60.0), 36, MST_D_DAY5STATE11);
		break;
	case MST_D_DAY5STATE11: //TPI update to LM realign attitude update
		UpdateMacro(UTP_PADONLY, PT_AP9LMTPI, rtcc->GETEval2(99.0*3600.0 + 15.0*60.0), 37, MST_D_DAY5STATE12);
		break;
	case MST_D_DAY5STATE12: //LM realign attitude update to LM realign attitude update
		UpdateMacro(UTP_PADONLY, PT_AP9AOTSTARPAD, SubStateTime > 3.0*60.0, 38, MST_D_DAY5STATE13);
		break;
	case MST_D_DAY5STATE13: //LM realign attitude update to LM burn to depletion update
		UpdateMacro(UTP_PADONLY, PT_AP9AOTSTARPAD, rtcc->GETEval2(99.0*3600.0 + 55.0*60.0), 39, MST_D_DAY5STATE14);
		break;
	case MST_D_DAY5STATE14: //LM burn to depletion update to LM jettison attitude update
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11LMMNV, SubStateTime > 3.0*60.0, 40, MST_D_DAY5STATE15);
		break;
	case MST_D_DAY5STATE15: //LM jettison attitude update to Block Data 11
		UpdateMacro(UTP_PADONLY, PT_GENERIC, rtcc->GETEval2(100.0*3600.0 + 35.0*60.0), 41, MST_D_DAY5STATE16);
		break;
	case MST_D_DAY5STATE16: //Block Data 11 to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(103.0*3600.0 + 5.0*60.0), 42, MST_D_DAY5STATE17);
		break;
	case MST_D_DAY5STATE17: //State vector update to Block Data 12
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(104.0*3600.0 + 15.0*60.0), 2, MST_D_DAY5STATE18);
		break;
	case MST_D_DAY5STATE18: //Block Data 12 to Block Data 13
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(114.0*3600.0 + 55.0*60.0), 43, MST_D_DAY6STATE1);
		break;
	case MST_D_DAY6STATE1: //Block Data 13 to SPS-6 Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(119.0*3600.0 + 40.0*60.0), 44, MST_D_DAY6STATE2);
		break;
	case MST_D_DAY6STATE2: //SPS-6 Update to S065 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, rtcc->GETEval2(123.0*3600.0 + 30.0*60.0), 45, MST_D_DAY6STATE3);
		break;
	case MST_D_DAY6STATE3: //S065 Update to S065 Update
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, rtcc->GETEval2(125.0*3600.0 + 5.0*60.0), 46, MST_D_DAY6STATE4);
		break;
	case MST_D_DAY6STATE4: //S065 Update to state vector update
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, rtcc->GETEval2(126.0*3600.0 + 50.0*60.0), 47, MST_D_DAY6STATE5);
		break;
	case MST_D_DAY6STATE5: //State vector update to Block Data 14
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(128.0*3600.0 + 35.0*60.0), 2, MST_D_DAY6STATE6);
		break;
	case MST_D_DAY6STATE6: //Block Data 14 to Block Data 15
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(140.0*3600.0 + 5.0*60.0), 48, MST_D_DAY7STATE1);
		break;
	case MST_D_DAY7STATE1: //Block Data 15 to Landmark Tracking Align update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(141.0*3600.0 + 35.0*60.0), 49, MST_D_DAY7STATE2);
		break;
	case MST_D_DAY7STATE2: //Landmark Tracking Align update to Landmark Tracking Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_GENERIC, rtcc->GETEval2(142.0*3600.0 + 32.0*60.0), 50, MST_D_DAY7STATE3);
		break;
	case MST_D_DAY7STATE3: //Landmark tracking update to landmark tracking update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, rtcc->GETEval2(143.0*3600.0 + 15.0*60.0), 51, MST_D_DAY7STATE4);
		break;
	case MST_D_DAY7STATE4: //Landmark tracking update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, rtcc->GETEval2(144.0*3600.0 + 5.0*60.0), 52, MST_D_DAY7STATE5);
		break;
	case MST_D_DAY7STATE5: //State vector update to landmark tracking update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(144.0*3600.0 + 35.0*60.0), 2, MST_D_DAY7STATE6);
		break;
	case MST_D_DAY7STATE6: //Landmark tracking update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, rtcc->GETEval2(145.0*3600.0 + 35.0*60.0), 53, MST_D_DAY7STATE7);
		break;
	case MST_D_DAY7STATE7: //State vector update to landmark tracking update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(146.0*3600.0 + 5.0*60.0), 2, MST_D_DAY7STATE8);
		break;
	case MST_D_DAY7STATE8: //Landmark tracking update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, rtcc->GETEval2(147.0*3600.0 + 5.0*60.0), 54, MST_D_DAY7STATE9);
		break;
	case MST_D_DAY7STATE9: //State vector update to state vector update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(148.0*3600.0 + 50.0*60.0), 2, MST_D_DAY7STATE10);
		break;
	case MST_D_DAY7STATE10: //State vector update to Block Data 16
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(151.0*3600.0 + 30.0*60.0), 2, MST_D_DAY7STATE11);
		break;
	case MST_D_DAY7STATE11: //Block Data 16 to Block Data 17
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(162.0*3600.0 + 5.0*60.0), 55, MST_D_DAY8STATE1);
		break;
	case MST_D_DAY8STATE1: //Block Data 17 to SPS-7 Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(168.0*3600.0 + 7.0*60.0), 56, MST_D_DAY8STATE2);
		break;
	case MST_D_DAY8STATE2: //SPS-7 Update to S065 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, rtcc->GETEval2(170.0*3600.0 + 35.0*60.0), 57, MST_D_DAY8STATE3);
		break;
	case MST_D_DAY8STATE3: //S065 Update to S065 Update
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, rtcc->GETEval2(172.0*3600.0 + 20.0*60.0), 58, MST_D_DAY8STATE4);
		break;
	case MST_D_DAY8STATE4: //S065 Update to Block Data 18
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, SubStateTime > 3.0*60.0, 59, MST_D_DAY8STATE5);
		break;
	case MST_D_DAY8STATE5: //Block Data 18 to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(174.0*3600.0), 60, MST_D_DAY8STATE6);
		break;
	case MST_D_DAY8STATE6: //State vector update to Block Data 19
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(185.0*3600.0 + 10.0*60.0), 2, MST_D_DAY9STATE1);
		break;
	case MST_D_DAY9STATE1: //Block Data 18 to T Align Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(187.0*3600.0 + 30.0*60.0), 61, MST_D_DAY9STATE2);
		break;
	case MST_D_DAY9STATE2: //T Align Update to S065 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_GENERIC, rtcc->GETEval2(189.0*3600.0 + 50.0*60.0), 62, MST_D_DAY9STATE3);
		break;
	case MST_D_DAY9STATE3: //S065 Update to S065 Update
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, rtcc->GETEval2(191.0*3600.0 + 30.0*60.0), 63, MST_D_DAY9STATE4);
		break;
	case MST_D_DAY9STATE4: //S065 Update to HGA Test REFSMMAT
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, rtcc->GETEval2(192.0*3600.0 + 20.0*60.0), 64, MST_D_DAY9STATE5);
		break;
	case MST_D_DAY9STATE5: //HGA Test REFSMMAT to Block Data 20
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_GENERIC, rtcc->GETEval2(196.0*3600.0 + 45.0*60), 80, MST_D_DAY9STATE6);
		break;
	case MST_D_DAY9STATE6: //Block Data 20 to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(198.0*3600.0 + 30.0*60.0), 65, MST_D_DAY9STATE7);
		break;
	case MST_D_DAY9STATE7: //State vector update to Block Data 21
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(209.0*3600.0 + 50.0*60.0), 2, MST_D_DAY10STATE1);
		break;
	case MST_D_DAY10STATE1: //Block Data 21 to T Align Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(211.0*3600.0 + 28.0*60.0), 66, MST_D_DAY10STATE2);
		break;
	case MST_D_DAY10STATE2: //T Align Update to S065 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_GENERIC, rtcc->GETEval2(213.0*3600.0 + 50.0*60.0), 67, MST_D_DAY10STATE3);
		break;
	case MST_D_DAY10STATE3: //S065 Update to S065 Update
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, rtcc->GETEval2(215.0*3600.0 + 28.0*60.0), 68, MST_D_DAY10STATE4);
		break;
	case MST_D_DAY10STATE4: //S065 Update to state vector update
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, rtcc->GETEval2(217.0*3600.0), 69, MST_D_DAY10STATE5);
		break;
	case MST_D_DAY10STATE5: //State vector update to Block Data 22
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, rtcc->GETEval2(220.0*3600.0 + 48.0*60.0), 2, MST_D_DAY10STATE6);
		break;
	case MST_D_DAY10STATE6: //Block Data 22 to CSM/LM state vector update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(221.0*3600.0 + 5.0*60.0), 70, MST_D_DAY10STATE7);
		break;
	case MST_D_DAY10STATE7: //CSM/LM state vector update to Block Data 23
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(232.0*3600.0 + 25.0*60.0), 3, MST_D_DAY11STATE1);
		break;
	case MST_D_DAY11STATE1: //Block Data 23 to Deorbit Maneuver Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, rtcc->GETEval2(235.0*3600.0 + 15.0*60.0), 71, MST_D_DAY11STATE2);
		break;
	case MST_D_DAY11STATE2: //Deorbit Maneuver Update to Entry PAD Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, SubStateTime > 5.0*60.0, 72, MST_D_DAY11STATE3);
		break;
	case MST_D_DAY11STATE3: //Entry PAD to Entry PAD Postburn Update
		UpdateMacro(UTP_PADONLY, PT_AP7ENT, cm->GetStage() == CM_STAGE, 73, MST_ORBIT_ENTRY);
		break;
	case MST_ORBIT_ENTRY: //Entry PAD Postburn Update to EOM
		MissionPhase = MMST_ENTRY;
		UpdateMacro(UTP_PADONLY, PT_AP7ENT, cm->GetStage() == CM_ENTRY_STAGE_SEVEN, 74, MST_LANDING);
		break;
	}
}