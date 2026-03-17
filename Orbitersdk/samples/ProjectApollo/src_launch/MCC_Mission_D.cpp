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
#include "nassputils.h"

using namespace nassp;

void MCC::MissionSequence_D()
{
	switch (MissionState)
	{
	case MST_D_INSERTION:	//Insertion to SV Update
		UpdateMacro(UTP_NONE, PT_NONE, mcc_calcs.GETEval(1.0 * 3600.0 + 45.0 * 60.0), 1, MST_D_DAY1SVUPDATE1);
		break;
	case MST_D_DAY1SVUPDATE1:  //SV Update to SV Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(3.0 * 3600.0 + 15.0 * 60.0), 2, MST_D_DAY1SVUPDATE2);
		break;
	case MST_D_DAY1SVUPDATE2:  //SV Update to Separation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(4.0 * 3600.0 + 6.0 * 60.0), 3, MST_D_SEPARATION);
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

			if (SubStateTime > 5.0*60.0 && mcc_calcs.GETEval(4.0 * 3600.0 + 25.0 * 60.0))
			{
				if (sivb == NULL)
				{
					VESSEL *v;
					OBJHANDLE hLV;
					hLV = oapiGetVesselByName(LVName);
					if (hLV != NULL)
					{
						v = oapiGetVesselInterface(hLV);

						if (utils::IsVessel(v, utils::SaturnV_SIVB)) {
							sivb = (SIVB *)v;
						}
					}
				}

				sivb->GetIU()->GetDCS()->Uplink(DCSUPLINK_REMOVE_INHIBIT_MANEUVER4, NULL);

				setSubState(2);
			}

			break;
		case 2:
			if (SubStateTime > 5.0 * 60.0)
			{
				sivb->GetIU()->GetDCS()->Uplink(DCSUPLINK_RESTART_MANEUVER_ENABLE, NULL);

				setSubState(3);
			}
			break;
		case 3:
			if (mcc_calcs.GETEval(4.0 * 3600.0 + 50.0 * 60.0))
			{
				SlowIfDesired();
				setState(MST_D_DAY1STATE1);
			}
			break;
		}
		break;
	case MST_D_DAY1STATE1:	//SPS-1 to Daylight Star Check
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(6.0 * 3600.0 + 15.0 * 60.0), 10, MST_D_DAY1STATE2);
		break;
	case MST_D_DAY1STATE2: //Daylight Star Check to SV Update
		UpdateMacro(UTP_PADONLY, PT_STARCHKPAD, mcc_calcs.GETEval(7.0 * 3600 + 20.0 * 60.0), 9, MST_D_DAY1STATE3);
		break;
	case MST_D_DAY1STATE3: //SV Update to Block Data 2
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(8.0 * 3600 + 30.0 * 60.0), 90, MST_D_DAY1STATE4);
		break;
	case MST_D_DAY1STATE4: //Block Data 2 to Block Data 3
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(19.0 * 3600.0 + 15.0 * 60.0), 11, MST_D_DAY2STATE1);
		break;
	case MST_D_DAY2STATE1: //Block Data 3 to SPS-2 Calculation
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(20.0 * 3600.0 + 37.0 * 60.0), 12, MST_D_DAY2STATE2);
		break;
	case MST_D_DAY2STATE2: //SPS-2 Calculation to SPS-2 Update
		UpdateMacro(UTP_NONE, PT_NONE, true, 13, MST_D_DAY2STATE3);
		break;
	case MST_D_DAY2STATE3: //SPS-2 Update to SPS-3 Calculation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(22.0 * 3600.0 + 25.0 * 60.0), 110, MST_D_DAY2STATE4);
		break;
	case MST_D_DAY2STATE4: //SPS-3 Calculation to SPS-3 Update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, mcc_calcs.GETEval(23.0 * 3600.0 + 55.0 * 60.0), 14, MST_D_DAY2STATE5);
		break;
	case MST_D_DAY2STATE5: //SPS-3 Update to SPS-4 Calculation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(25.0 * 3600.0 + 30.0 * 60.0), 111, MST_D_DAY2STATE6);
		break;
	case MST_D_DAY2STATE6: //SPS-4 Calculation to SPS-4 Update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, mcc_calcs.GETEval(26.0 * 3600.0 + 50.0 * 60.0), 15, MST_D_DAY2STATE7);
		break;
	case MST_D_DAY2STATE7: //SPS-4 to SV Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(28 * 3600.0 + 50.0 * 60.0), 112, MST_D_DAY2STATE8);
		break;
	case MST_D_DAY2STATE8: //SV Update to Block Data 4
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(28.0 * 3600.0 + 55.0 * 60.0), 91, MST_D_DAY2STATE9);
		break;
	case MST_D_DAY2STATE9: //Block Data 4 to Block Data 5
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(40.0 * 3600.0 + 10.0 * 60.0), 16, MST_D_DAY3STATE1);
		break;
	case MST_D_DAY3STATE1: //Block Data 5 to CMC Docked DPS Burn Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(41.0 * 3600.0 + 10.0 * 60.0), 17, MST_D_DAY3STATE2);
		break;
	case MST_D_DAY3STATE2: //CMC Docked DPS Burn Update to LM AOT STAR OBS PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(41.0 * 3600.0 + 20.0 * 60.0), 18, MST_D_DAY3STATE3);
		break;
	case MST_D_DAY3STATE3: //LM AOT STAR OBS PAD to Block Data 6
		UpdateMacro(UTP_PADONLY, PT_AP9AOTSTARPAD, mcc_calcs.GETEval(47.0 * 3600.0 + 10.0 * 60.0), 19, MST_D_DAY3STATE5);
		break;
		// **Need SBand Steerable PAD here**
	case MST_D_DAY3STATE5: //Block Data 6 to LM DAP PAD
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(47.0 * 3600.0 + 50.0 * 60.0), 20, MST_D_DAY3STATE6);
		break;
	case MST_D_DAY3STATE6: //LM DAP PAD to CMC state vector updates
		UpdateMacro(UTP_PADONLY, PT_AP10DAPDATA, mcc_calcs.GETEval(48.0 * 3600.0), 8, MST_D_DAY3STATE7);
		break;
	case MST_D_DAY3STATE7: //CMC state vector updates to LGC Docked DPS Burn Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(48.0 * 3600.0 + 10.0 * 60.0), 21, MST_D_DAY3STATE8);
		break;
	case MST_D_DAY3STATE8: //LGC Docked DPS Burn Update to LGC Gyro Torquing Angles
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11LMMNV, SubStateTime > 3.0*60.0, 92, MST_D_DAY3STATE9);
		break;
	case MST_D_DAY3STATE9: //LGC Gyro Torquing Angles to LGC Gyro Torquing Angles
		UpdateMacro(UTP_PADONLY, PT_TORQANG, mcc_calcs.GETEval(49.0 * 3600.0 + 5.0 * 60.0), 22, MST_D_DAY3STATE10);
		break;
	case MST_D_DAY3STATE10: //LGC Gyro Torquing Angles to SPS-5 Calculation
		UpdateMacro(UTP_PADONLY, PT_TORQANG, mcc_calcs.GETEval(52.0 * 3600.0 + 50.0 * 60.0), 22, MST_D_DAY3STATE11);
		break;
	case MST_D_DAY3STATE11: //SPS-5 Calculation to SPS-5 Update
		UpdateMacro(UTP_NONE, PT_NONE, true, 23, MST_D_DAY3STATE12);
		break;
	case MST_D_DAY3STATE12: //SPS-5 Update to SV Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(55.0 * 3600.0 + 30.0 * 60.0), 113, MST_D_DAY3STATE13);
		break;
	case MST_D_DAY3STATE13: //SV Update to Block Data 7
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(56.0 * 3600.0 + 40.0 * 60.0), 93, MST_D_DAY3STATE14);
		break;
	case MST_D_DAY3STATE14: //Block Data 7 to Block Data 8
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(67.0 * 3600.0 + 30.0 * 60.0), 24, MST_D_DAY4STATE1);
		break;
	case MST_D_DAY4STATE1: //Block Data 8 to SCS Check
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(69.0 * 3600.0), 25, MST_D_DAY4STATE2);
		break;
	case MST_D_DAY4STATE2: //SCS Check to EVA REFSMMAT Update or Block Data 9 Flown
		UpdateMacro(UTP_NONE, PT_NONE, mcc_calcs.GETEval(69.0 * 3600.0 + 55.0 * 60.0), 150, MST_D_DAY4STATE3_NOMINAL, scrubbed, mcc_calcs.GETEval(76.0 * 3600.0 + 25.0 * 60.0), MST_D_DAY4STATE3_FLOWN);
		break;
		// Nominal EVA
	case MST_D_DAY4STATE3_NOMINAL: //EVA REFSMMAT Update to SV Update Nominal
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, mcc_calcs.GETEval(77.0 * 3600.0 + 45.0 * 60.0), 26, MST_D_DAY4STATE4_NOMINAL);
		break;
	case MST_D_DAY4STATE4_NOMINAL: //SV Update Nominal to Block Data 9 Nominal
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(78.0 * 3600.0 + 10.0 * 60.0), 95, MST_D_DAY4STATE5_NOMINAL);
		break;
	case MST_D_DAY4STATE5_NOMINAL: //Block Data 9 Nominal to Block Data 10
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(87.0 * 3600.0 + 15.0 * 60.0), 27, MST_D_DAY5STATE1);
		break;
		// Flown EVA
	case MST_D_DAY4STATE3_FLOWN: //Block Data 9 Flown to SV Update Flown
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(76.0 * 3600.0 + 35.0 * 60.0), 27, MST_D_DAY4STATE4_FLOWN);
		break;
	case MST_D_DAY4STATE4_FLOWN: //SV Update Flown to Block Data 10
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(87.0 * 3600.0 + 15.0 * 60.0), 94, MST_D_DAY5STATE1);
		break;
		// Continue normal timeline
	case MST_D_DAY5STATE1: //Block Data 10 to CSM Rendezvous DAP & REFSMMAT Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(89.0 * 3600.0 + 5.0 * 60.0), 28, MST_D_DAY5STATE2);
		break;
	case MST_D_DAY5STATE2: //CSM Rendezvous DAP & REFSMMAT Update to LM Rendezvous REFSMMAT Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP10DAPDATA, mcc_calcs.GETEval(91.0 * 3600.0 + 10.0 * 60.0), 29, MST_D_DAY5STATE3);
		break;
	case MST_D_DAY5STATE3: //LM Rendezvous REFSMMAT Update to LM Gyro Torquing Angles Update
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(91.0 * 3600.0 + 15.0 * 60.0), 30, MST_D_DAY5STATE4);
		break;
	case MST_D_DAY5STATE4: //LM Gyro torquing angles update to Phasing update
		UpdateMacro(UTP_PADONLY, PT_TORQANG, mcc_calcs.GETEval(92.0 * 3600.0), 31, MST_D_DAY5STATE5);
		break;
	case MST_D_DAY5STATE5: //Phasing update to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP11LMMNV, mcc_calcs.GETEval(92.0 * 3600.0 + 35.0 * 60.0), 32, MST_D_DAY5STATE6);
		break;
	case MST_D_DAY5STATE6: //SV Update to TPI0 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, mcc_calcs.GETEval(rtcc->TimeofIgnition - (35.0 * 60.0)), 97, MST_D_DAY5STATE7);
		break;
	case MST_D_DAY5STATE7: //TPI0 update to Insertion update
		UpdateMacro(UTP_PADONLY, PT_AP9LMTPI, mcc_calcs.GETEval(95.0 * 3600.0 + 10.0 * 60.0), 33, MST_D_DAY5STATE8);
		break;
	case MST_D_DAY5STATE8: //Insertion update to CSI update
		UpdateMacro(UTP_PADONLY, PT_AP11LMMNV, mcc_calcs.GETEval(rtcc->calcParams.Insertion + 10.0 * 60.0), 34, MST_D_DAY5STATE9);
		break;
	case MST_D_DAY5STATE9: //CSI update to CDH update
		UpdateMacro(UTP_PADONLY, PT_AP10CSI, mcc_calcs.GETEval(rtcc->calcParams.CSI + 15.0 * 60.0), 35, MST_D_DAY5STATE10);
		break;
	case MST_D_DAY5STATE10: //CDH update to TPI update
		UpdateMacro(UTP_PADONLY, PT_AP9LMCDH, mcc_calcs.GETEval(rtcc->calcParams.CDH + 15.0 * 60.0), 36, MST_D_DAY5STATE11);
		break;
	case MST_D_DAY5STATE11: //TPI update to LM realign attitude update
		UpdateMacro(UTP_PADONLY, PT_AP9LMTPI, mcc_calcs.GETEval(99.0 * 3600.0 + 15.0 * 60.0), 37, MST_D_DAY5STATE12);
		break;
	case MST_D_DAY5STATE12: //LM realign attitude update to LM realign attitude update
		UpdateMacro(UTP_PADONLY, PT_AP9AOTSTARPAD, SubStateTime > 3.0 * 60.0, 38, MST_D_DAY5STATE13);
		break;
	case MST_D_DAY5STATE13: //LM realign attitude update to LM burn to depletion update
		UpdateMacro(UTP_PADONLY, PT_AP9AOTSTARPAD, mcc_calcs.GETEval(99.0 * 3600.0 + 55.0 * 60.0), 39, MST_D_DAY5STATE14);
		break;
	case MST_D_DAY5STATE14: //LM burn to depletion update to LM jettison attitude update
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11LMMNV, SubStateTime > 3.0*60.0, 40, MST_D_DAY5STATE15);
		break;
	case MST_D_DAY5STATE15: //LM jettison attitude update to Block Data 11
		UpdateMacro(UTP_PADONLY, PT_GENERIC, mcc_calcs.GETEval(100.0*3600.0 + 35.0*60.0), 41, MST_D_DAY5STATE16);
		break;
	case MST_D_DAY5STATE16: //Block Data 11 to post jettison sep maneuver
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(101.0 * 3600.0 + 10.0 * 60.0), 42, MST_D_DAY5STATE17);
		break;
	case MST_D_DAY5STATE17: //Post jettison sep maneuver to AEAA ARM
		UpdateMacro(UTP_PADONLY, PT_GENERIC, mcc_calcs.GETEval(rtcc->TimeofIgnition - 10.0 * 60.0), 75, MST_D_DAY5STATE18);
		break;
	case MST_D_DAY5STATE18: //AEAA ARM to P42
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, SubStateTime > 30.0, 120, MST_D_DAY5STATE19); //Needs to be changed to AEAA ARM uplink
		break;
	case MST_D_DAY5STATE19: //P42 to LM DSKY Enter
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, SubStateTime > 10.0, 121, MST_D_DAY5STATE20);
		break;
	case MST_D_DAY5STATE20: //LM DSKY Enter to LM DSKY PRO
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, SubStateTime > 10.0, 122, MST_D_DAY5STATE21);
		break;
	case MST_D_DAY5STATE21: //LM DSKY PRO to LM DSKY PRO (ENG ON)
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, mcc_calcs.GETEval(rtcc->TimeofIgnition - 6.0), 123, MST_D_DAY5STATE22);
		break;
	case MST_D_DAY5STATE22: //LM DSKY PRO (ENG ON) to SV Update
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, mcc_calcs.GETEval(103.0 * 3600.0 + 5.0 * 60.0), 124, MST_D_DAY5STATE23);
		break;
	case MST_D_DAY5STATE23: //SV Update to Block Data 12
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(104.0 * 3600.0 + 15.0 * 60.0), 98, MST_D_DAY5STATE24);
		break;
	case MST_D_DAY5STATE24: //Block Data 12 to Block Data 13
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(114.0 * 3600.0 + 55.0 * 60.0), 43, MST_D_DAY6STATE1);
		break;
	case MST_D_DAY6STATE1: //Block Data 13 to SPS-6 Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(119.0 * 3600.0 + 40.0 * 60.0), 44, MST_D_DAY6STATE2);
		break;
	case MST_D_DAY6STATE2: //SPS-6 Update to S065 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(123.0 * 3600.0 + 30.0 * 60.0), 45, MST_D_DAY6STATE3);
		break;
	case MST_D_DAY6STATE3: //S065 Update to S065 Update
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, mcc_calcs.GETEval(125.0 * 3600.0 + 5.0 * 60.0), 46, MST_D_DAY6STATE4);
		break;
	case MST_D_DAY6STATE4: //S065 Update to SV Update
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, mcc_calcs.GETEval(126.0 * 3600.0 + 50.0 * 60.0), 47, MST_D_DAY6STATE5);
		break;
	case MST_D_DAY6STATE5: //SV Update to Block Data 14
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, mcc_calcs.GETEval(128.0 * 3600.0 + 35.0 * 60.0), 100, MST_D_DAY6STATE6);
		break;
	case MST_D_DAY6STATE6: //Block Data 14 to Block Data 15
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(140.0 * 3600.0 + 10.0 * 60.0), 48, MST_D_DAY7STATE1);
		break;
	case MST_D_DAY7STATE1: //Block Data 15 to Landmark Tracking Alignment
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(141.0 * 3600.0 + 35.0 * 60.0), 49, MST_D_DAY7STATE2);
		break;
	case MST_D_DAY7STATE2: //Landmark Tracking Alignment to Landmark Tracking Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(142.0 * 3600.0 + 35.0 * 60.0), 50, MST_D_DAY7STATE3);
		break;
	case MST_D_DAY7STATE3: //Landmark Tracking Update to Landmark Tracking Update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, mcc_calcs.GETEval(143.0 * 3600.0 + 15.0 * 60.0), 51, MST_D_DAY7STATE4);
		break;
	case MST_D_DAY7STATE4: //Landmark Tracking Update to Landmark SV Update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, mcc_calcs.GETEval(144.0 * 3600.0 + 5.0 * 60.0), 52, MST_D_DAY7STATE5);
		break;
	case MST_D_DAY7STATE5: //Landmark SV Update to Landmark Tracking Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(144.0 * 3600.0 + 35.0 * 60.0), 130, MST_D_DAY7STATE6);
		break;
	case MST_D_DAY7STATE6: //Landmark Tracking Update to Landmark SV Update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, mcc_calcs.GETEval(145.0 * 3600.0 + 35.0 * 60.0), 53, MST_D_DAY7STATE7);
		break;
	case MST_D_DAY7STATE7: //Landmark SV Update to Landmark Tracking Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(146.0 * 3600.0 + 5.0 * 60.0), 131, MST_D_DAY7STATE8);
		break;
	case MST_D_DAY7STATE8: //Landmark Tracking Update to Landmark SV Update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, mcc_calcs.GETEval(147.0 * 3600.0 + 5.0 * 60.0), 54, MST_D_DAY7STATE9);
		break;
	case MST_D_DAY7STATE9: //Landmark SV Update to SV Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(148.0 * 3600.0 + 50.0 * 60.0), 132, MST_D_DAY7STATE10);
		break;
	case MST_D_DAY7STATE10: //SV Update to Block Data 16
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(151.0 * 3600.0 + 30.0 * 60.0), 101, MST_D_DAY7STATE11);
		break;
	case MST_D_DAY7STATE11: //Block Data 16 to Block Data 17
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(162.0 * 3600.0 + 5.0 * 60.0), 55, MST_D_DAY8STATE1);
		break;
	case MST_D_DAY8STATE1: //Block Data 17 to SPS-7 Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(168.0 * 3600.0 + 10.0 * 60.0), 56, MST_D_DAY8STATE2);
		break;
	case MST_D_DAY8STATE2: //SPS-7 Update to S065 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(170.0 * 3600.0 + 35.0 * 60.0), 57, MST_D_DAY8STATE3);
		break;
	case MST_D_DAY8STATE3: //S065 Update to S065 Update
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, mcc_calcs.GETEval(172.0 * 3600.0 + 20.0 * 60.0), 58, MST_D_DAY8STATE4);
		break;
	case MST_D_DAY8STATE4: //S065 Update to Block Data 18
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, SubStateTime > 3.0 * 60.0, 59, MST_D_DAY8STATE5);
		break;
	case MST_D_DAY8STATE5: //Block Data 18 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(174.0 * 3600.0), 60, MST_D_DAY8STATE6);
		break;
	case MST_D_DAY8STATE6: //SV Update to Block Data 19
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(185.0 * 3600.0 + 10.0 * 60.0), 102, MST_D_DAY9STATE1);
		break;
	case MST_D_DAY9STATE1: //Block Data 18 to T-Align Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(187.0 * 3600.0 + 30.0 * 60.0), 61, MST_D_DAY9STATE2);
		break;
	case MST_D_DAY9STATE2: //T-Align Update to S065 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(189.0 * 3600.0 + 50.0 * 60.0), 62, MST_D_DAY9STATE3);
		break;
	case MST_D_DAY9STATE3: //S065 Update to S065 Update
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, mcc_calcs.GETEval(191.0 * 3600.0 + 35.0 * 60.0), 63, MST_D_DAY9STATE4);
		break;
	case MST_D_DAY9STATE4: //S065 Update to HGA Test REFSMMAT
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, mcc_calcs.GETEval(192.0 * 3600.0 + 25.0 * 60.0), 64, MST_D_DAY9STATE5);
		break;
	case MST_D_DAY9STATE5: //HGA Test REFSMMAT to Landmark Tracking Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_GENERIC, mcc_calcs.GETEval(193.0 * 3600.0 + 40.0 * 60), 80, MST_D_DAY9STATE6);
		break;
	case MST_D_DAY9STATE6: //Landmark Tracking Update to Backup GDC Align Update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, mcc_calcs.GETEval(195.0 * 3600.0 + 30.0 * 60), 76, MST_D_DAY9STATE7);
		break;
	case MST_D_DAY9STATE7: //Backup GDC Align Update to Block Data 20
		UpdateMacro(UTP_PADONLY, PT_GENERIC, mcc_calcs.GETEval(196.0 * 3600.0 + 45.0 * 60), 81, MST_D_DAY9STATE8);
		break;
	case MST_D_DAY9STATE8: //Block Data 20 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(198.0 * 3600.0 + 30.0 * 60.0), 65, MST_D_DAY9STATE9);
		break;
	case MST_D_DAY9STATE9: //SV Update to Block Data 21
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, mcc_calcs.GETEval(209.0 * 3600.0 + 50.0 * 60.0), 103, MST_D_DAY10STATE1);
		break;
	case MST_D_DAY10STATE1: //Block Data 21 to T-Align Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(211.0 * 3600.0 + 30.0 * 60.0), 66, MST_D_DAY10STATE2);
		break;
	case MST_D_DAY10STATE2: //T-Align Update to S065 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(213.0 * 3600.0 + 50.0 * 60.0), 67, MST_D_DAY10STATE3);
		break;
	case MST_D_DAY10STATE3: //S065 Update to S065 Update
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, mcc_calcs.GETEval(215.0 * 3600.0 + 30.0 * 60.0), 68, MST_D_DAY10STATE4);
		break;
	case MST_D_DAY10STATE4: //S065 Update to SV Update
		UpdateMacro(UTP_PADONLY, PT_S065UPDATE, mcc_calcs.GETEval(217.0 * 3600.0 + 25.0 * 60.0), 69, MST_D_DAY10STATE5);
		break;
	case MST_D_DAY10STATE5: //SV Update to Landmark Tracking Update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 2.0, 104, MST_D_DAY10STATE6);
		break;
	case MST_D_DAY10STATE6: //Landmark Tracking Update to Block Data 22
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, mcc_calcs.GETEval(220.0 * 3600.0 + 48.0 * 60.0), 77, MST_D_DAY10STATE7);
		break;
	case MST_D_DAY10STATE7: //Block Data 22 to CSM/LM state vector update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(221.0 * 3600.0 + 5.0 * 60.0), 70, MST_D_DAY10STATE8);
		break;
	case MST_D_DAY10STATE8: //CSM/LM state vector update to Block Data 23
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(233.0 * 3600.0 + 40.0 * 60.0), 105, MST_D_DAY11STATE1);
		break;
	case MST_D_DAY11STATE1: //Block Data 23 to Deorbit Maneuver Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(235.0 * 3600.0 + 15.0 * 60.0), 71, MST_D_DAY11STATE2);
		break;
	case MST_D_DAY11STATE2: //Deorbit Maneuver Update to Entry PAD Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, SubStateTime > 5.0 * 60.0, 72, MST_D_DAY11STATE3);
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