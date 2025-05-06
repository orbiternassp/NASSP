/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

MCC sequencing for Mission C

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
#include "mcc.h"
#include "rtcc.h"
#include "MCC_Mission_C.h"

void MCC::MissionSequence_C()
{
	switch (MissionState) {
	case MST_C_INSERTION: //Insertion to state vector uplink & LOX times
		UpdateMacro(UTP_NONE, PT_NONE, mcc_calcs.GETEval(56.0 * 60.0), 100, MST_C_DAY0STATE1);
		break;
	case MST_C_DAY0STATE1: //State vector uplink & LOX times to P52 Nav Star Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_GENERIC, mcc_calcs.GETEval(1.0 * 3600.0 + 50.0 * 60.0), 62, MST_C_DAY0STATE2);
		break;
	case MST_C_DAY0STATE2: //P52 Nav Star Update to SIVB Pitch & Inertial Times
		UpdateMacro(UTP_PADONLY, PT_GENERIC, mcc_calcs.GETEval(2.0 * 3600.0 + 20.0 * 60.0), 63, MST_C_DAY0STATE3);
		break;
	case MST_C_DAY0STATE3: //SIVB Pitch & Inertial Times to CSM/LV separation
		UpdateMacro(UTP_PADONLY, PT_GENERIC, cm->GetStage() == CSM_LEM_STAGE, 64, MST_C_DAY0STATE4);
		break;
	case MST_C_DAY0STATE4: //CSM/LV separation to phasing maneuver update
		switch (SubState) {
		case 0:
			addMessage("SEPARATION");
			SlowIfDesired();
			setSubState(1);
			break;
		case 1:
			if (mcc_calcs.GETEval(3.0 * 3600.0 + 5.0 * 60.0))
			{
				SlowIfDesired();
				setState(MST_C_DAY0STATE5);
			}
			break;
		}
		break;
	case MST_C_DAY0STATE5: //Phasing maneuver update to S-IVB navigation update
		UpdateMacro(UTP_PADONLY, PT_AP7MNV, mcc_calcs.GETEval(4.0 * 3600.0 + 31.0 * 60.0), 1, MST_C_DAY0STATE6);
		break;
	case MST_C_DAY0STATE6: // S-IVB navigation update to 6-4 Deorbit Maneuver update
		UpdateMacro(UTP_NONE, PT_NONE, mcc_calcs.GETEval(4.0 * 3600.0 + 45.0 * 60.0), 101, MST_C_DAY0STATE7);
		break;
	case MST_C_DAY0STATE7: // 6-4 Deorbit Maneuver update to Retro Orientation Test
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, SubStateTime > 3.0 * 60.0, 2, MST_C_DAY0STATE8);
		break;
	case MST_C_DAY0STATE8: // Retro Orientation Test update to Block Data 2
		UpdateMacro(UTP_PADONLY, PT_RETROORIENTATION, mcc_calcs.GETEval(10.0 * 3600.0 + 30.0 * 60.0), 102, MST_C_DAY0STATE9);
		break;
	case MST_C_DAY0STATE9: //Block Data 2 to 2nd Phasing Maneuver Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(14.0 * 3600.0 + 16.0 * 60.0), 3, MST_C_DAY0STATE10);
		break;
	case MST_C_DAY0STATE10: // 2nd Phasing Maneuver Update to Block Data 3
		UpdateMacro(UTP_PADONLY, PT_AP7MNV, mcc_calcs.GETEval(21.0 * 3600.0 + 50.0 * 60.0), 4, MST_C_DAY0STATE11);
		break;
	case MST_C_DAY0STATE11: // Block Data 3 to Preliminary NCC1 Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(22.0 * 3600.0 + 25.0 * 60.0), 5, MST_C_DAY0STATE12);
		break;
	case MST_C_DAY0STATE12: // Preliminary NCC1 Update to Final NCC1 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(25.0 * 3600.0 + 30.0 * 60.0), 6, MST_C_DAY1STATE1);
		break;
	case MST_C_DAY1STATE1: // Final NCC1 Update to NCC2 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(27.0 * 3600.0 + 5.0 * 60.0), 7, MST_C_DAY1STATE2);
		break;
	case MST_C_DAY1STATE2: //  NCC2 Update to NSR Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, SubStateTime > 5.0 * 60.0, 8, MST_C_DAY1STATE3, scrubbed, true, MST_C_DAY1STATE3);
		break;
	case MST_C_DAY1STATE3: // NSR Update to TPI Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(28.0 * 3600.0 + 50.0 * 60.0), 9, MST_C_DAY1STATE4);
		break;
	case MST_C_DAY1STATE4: // TPI Update to Final Separation Maneuver update
		UpdateMacro(UTP_PADONLY, PT_AP7TPI, mcc_calcs.GETEval(30.0 * 3600.0 + 10.0 * 60.0), 10, MST_C_DAY1STATE5);
		break;
	case MST_C_DAY1STATE5: // Final Separation Maneuver update to Block Data 4
		UpdateMacro(UTP_PADONLY, PT_AP7MNV, mcc_calcs.GETEval(30.0 * 3600.0 + 45.0 * 60.0), 11, MST_C_DAY1STATE6);
		break;
	case MST_C_DAY1STATE6: // Block Data 4 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(34.0 * 3600.0 + 45.0 * 60.0), 12, MST_C_DAY1STATE7);
		break;
	case MST_C_DAY1STATE7: // SV Update to P20 SV Update 1
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, mcc_calcs.GETEval(38.0 * 3600.0 + 35.0 * 60.0), 51, MST_C_DAY1STATE8);
		break;
	case MST_C_DAY1STATE8: // P20 SV Update 1 to Block Data 5
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(39.0 * 3600.0 + 50.0 * 60.0), 66, MST_C_DAY1STATE9);
		break;
	case MST_C_DAY1STATE9: // Block Data 5 to P20 SV Update 2
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(44.0 * 3600.0 + 40.0 * 60.0), 13, MST_C_DAY1STATE10);
		break;
	case MST_C_DAY1STATE10: // P20 SV Update 2 to Block Data 6
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(48.0 * 3600.0 + 45.0 * 60.0), 67, MST_C_DAY2STATE1);
		break;
	case MST_C_DAY2STATE1: // Block Data 6 to P20 SV Update 3
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(50.0 * 3600.0 + 20.0 * 60.0), 14, MST_C_DAY2STATE2);
		break;
	case MST_C_DAY2STATE2: // P20 SV Update 3 to SCT Star Count 1
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(51.0 * 3600.0 + 20.0 * 60.0), 68, MST_C_DAY2STATE3);
		break;
	case MST_C_DAY2STATE3: // SCT Star Count 1 to Block Data 7
		UpdateMacro(UTP_PADONLY, PT_AP7STRCNTPAD, mcc_calcs.GETEval(57.0 * 3600.0), 69, MST_C_DAY2STATE4);
		break;
	case MST_C_DAY2STATE4: // Block Data 7 to WMSR SV Nav Check
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(67.0 * 3600.0 + 50.0 * 60.0), 15, MST_C_DAY2STATE5);
		break;
	case MST_C_DAY2STATE5: // WMSR SV Nav Check to WMSR Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, SubStateTime > 5.0*60.0, 80, MST_C_DAY2STATE6);
		break;
	case MST_C_DAY2STATE6: // WMSR Update to Block Data 8
		UpdateMacro(UTP_PADONLY, PT_AP7WSMRPAD, mcc_calcs.GETEval(68.0 * 3600.0 + 35.0 * 60.0), 81, MST_C_DAY2STATE7);
		break;
	case MST_C_DAY2STATE7: // Block Data 8 to SPS-3
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(72.0 * 3600.0), 16, MST_C_DAY3STATE1);
		break;
	case MST_C_DAY3STATE1: // SPS-3 to Block Data 9
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(77 * 3600.0 + 5 * 60.0), 17, MST_C_DAY3STATE2);
		break;
	case MST_C_DAY3STATE2: // Block Data 9 to Block Data 10
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(85 * 3600.0 + 52 * 60.0), 18, MST_C_DAY3STATE3);
		break;
	case MST_C_DAY3STATE3: // Block Data 10 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(90 * 3600.0), 19, MST_C_DAY3STATE4);
		break;
	case MST_C_DAY3STATE4: // SV Update to SV Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(94 * 3600.0), 52, MST_C_DAY3STATE5);
		break;
	case MST_C_DAY3STATE5: // SV Update to Block Data 11
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(96 * 3600.0 + 21 * 60.0), 52, MST_C_DAY4STATE1);
		break;
	case MST_C_DAY4STATE1: // Block Data 11 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(97 * 3600.0 + 40 * 60.0), 20, MST_C_DAY4STATE2);
		break;
	case MST_C_DAY4STATE2: // SV Update to Block Data 12
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(105 * 3600.0 + 10 * 60.0), 52, MST_C_DAY4STATE3);
		break;
	case MST_C_DAY4STATE3: // Block Data 12 to Block Data 13
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(116 * 3600.0 + 3 * 60.0), 21, MST_C_DAY4STATE4);
		break;
	case MST_C_DAY4STATE4: // Block Data 13 to SPS-4
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(117 * 3600.0 + 30 * 60.0), 22, MST_C_DAY4STATE5);
		break;
	case MST_C_DAY4STATE5: // SPS-4 to landmark tracking update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(121 * 3600.0 + 6 * 60.0), 23, MST_C_DAY5STATE1);
		break;
	case MST_C_DAY5STATE1: // Landmark tracking update to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, mcc_calcs.GETEval(121 * 3600.0 + 36 * 60.0), 55, MST_C_DAY5STATE2);
		break;
	case MST_C_DAY5STATE2: // SV Update to SV Update
		UpdateMacro(UTP_PADONLY, PT_P27PAD, mcc_calcs.GETEval(123 * 3600.0 + 40 * 60.0), 54, MST_C_DAY5STATE3);
		break;
	case MST_C_DAY5STATE3: // SV Update to Block Data 14
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(125 * 3600.0 + 12 * 60.0), 52, MST_C_DAY5STATE4);
		break;
	case MST_C_DAY5STATE4: // Block Data 14 to Block Data 15
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(134 * 3600.0 + 48 * 60.0), 24, MST_C_DAY5STATE5);
		break;
	case MST_C_DAY5STATE5: // Block Data 15 to rev 90 landmark tracking update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(139 * 3600.0 + 40 * 60.0), 25, MST_C_DAY5STATE6);
		break;
	case MST_C_DAY5STATE6: // Rev 90 landmark tracking update to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, SubStateTime > 5.0*60.0, 56, MST_C_DAY5STATE7);
		break;
	case MST_C_DAY5STATE7: // SV Update to rev 91 landmark tracking
		UpdateMacro(UTP_PADONLY, PT_P27PAD, mcc_calcs.GETEval(143 * 3600.0), 54, MST_C_DAY5STATE8);
		break;
	case MST_C_DAY5STATE8: // Rev 91 landmark tracking update to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, mcc_calcs.GETEval(143 * 3600.0 + 20 * 60.0), 57, MST_C_DAY5STATE9);
		break;
	case MST_C_DAY5STATE9: // SV Update to Block Data 16
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(144 * 3600.0 + 11 * 60.0), 52, MST_C_DAY5STATE10);
		break;
	case MST_C_DAY5STATE10: // Block Data 16 to rev 92 landmark tracking update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(145 * 3600.0 + 30 * 60.0), 26, MST_C_DAY6STATE1);
		break;
	case MST_C_DAY6STATE1: // Rev 92 landmark tracking update to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, mcc_calcs.GETEval(147 * 3600.0 + 10 * 60.0), 58, MST_C_DAY6STATE2);
		break;
	case MST_C_DAY6STATE2: // SV Update to Block Data 17
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(153 * 3600.0 + 47 * 60.0), 52, MST_C_DAY6STATE3);
		break;
	case MST_C_DAY6STATE3: // Block Data 17 to SPS-5
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(161 * 3600.0 + 18 * 60.0), 27, MST_C_DAY6STATE4);
		break;
	case MST_C_DAY6STATE4: // SPS-5 to Block Data 18
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(161 * 3600.0 + 59 * 60.0), 28, MST_C_DAY6STATE5);
		break;
	case MST_C_DAY6STATE5: // Block Data 18 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(169 * 3600.0), 29, MST_C_DAY7STATE1);
		break;
	case MST_C_DAY7STATE1: // SV Update to Block Data 19
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(172 * 3600.0 + 42 * 60.0), 52, MST_C_DAY7STATE2);
		break;
	case MST_C_DAY7STATE2: // Block Data 19 to Block Data 20
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(180 * 3600.0 + 56 * 60.0), 30, MST_C_DAY7STATE3);
		break;
	case MST_C_DAY7STATE3: // Block Data 20 to Block Data 21
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(192 * 3600.0 + 17 * 60.0), 31, MST_C_DAY8STATE1);
		break;
	case MST_C_DAY8STATE1: // Block Data 21 to Block Data 22
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(201 * 3600.0 + 55 * 60.0), 32, MST_C_DAY8STATE2);
		break;
	case MST_C_DAY8STATE2: // Block Data 22 to SPS-6
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(205 * 3600.0 + 25 * 60.0), 33, MST_C_DAY8STATE3);
		break;
	case MST_C_DAY8STATE3: // SPS-6 to Block Data 23
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(210 * 3600.0 + 11 * 60.0), 34, MST_C_DAY8STATE4);
		break;
	case MST_C_DAY8STATE4: // Block Data 23 to rev 135 landmark tracking update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(212 * 3600.0 + 40 * 60.0), 35, MST_C_DAY8STATE5);
		break;
	case MST_C_DAY8STATE5: // Rev 135 landmark tracking update to SV PAD
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, mcc_calcs.GETEval(213 * 3600.0), 59, MST_C_DAY8STATE6);
		break;
	case MST_C_DAY8STATE6: // SV PAD to P27 PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(214 * 3600.0 + 10 * 60.0), 61, MST_C_DAY8STATE7);
		break;
	case MST_C_DAY8STATE7: //P27 PAD to rev 136 landmark tracking update
		UpdateMacro(UTP_PADONLY, PT_P27PAD, mcc_calcs.GETEval(215 * 3600.0), 36, MST_C_DAY8STATE8);
		break;
	case MST_C_DAY8STATE8: // Rev 136 landmark tracking update to SV PAD
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, mcc_calcs.GETEval(216 * 3600.0), 60, MST_C_DAY9STATE1);
		break;
	case MST_C_DAY9STATE1: //SV PAD to SV PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(217 * 3600.0 + 30 * 60.0), 52, MST_C_DAY9STATE2);
		break;
	case MST_C_DAY9STATE2: //SV PAD to Block Data 24
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(220 * 3600.0 + 43 * 60.0), 52, MST_C_DAY9STATE3);
		break;
	case MST_C_DAY9STATE3: // Block Data 24 to Block Data 25
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(230 * 3600.0 + 24 * 60.0), 37, MST_C_DAY9STATE4);
		break;
	case MST_C_DAY9STATE4: // Block Data 25 to SPS-7
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(233 * 3600.0 + 27 * 60.0), 38, MST_C_DAY9STATE5);
		break;
	case MST_C_DAY9STATE5: // SPS-7 to SV PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(240 * 3600.0 + 20 * 60.0), 39, MST_C_DAY10STATE1);
		break;
	case MST_C_DAY10STATE1: // SV PAD to Block Data 26
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(241 * 3600.0 + 39 * 60.0), 52, MST_C_DAY10STATE2);
		break;
	case MST_C_DAY10STATE2: // Block Data 26 to Block Data 27
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(248 * 3600.0 + 56 * 60.0), 40, MST_C_DAY10STATE3);
		break;
	case MST_C_DAY10STATE3: // Block Data 27 to SV PAD
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, mcc_calcs.GETEval(255 * 3600.0), 41, MST_C_DAY10STATE4);
		break;
	case MST_C_DAY10STATE4: // SV PAD to Deorbit Maneuver
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, mcc_calcs.GETEval(257 * 3600.0 + 20 * 60.0), 52, MST_C_DAY10STATE5);
		break;
	case MST_C_DAY10STATE5: // Deorbit Maneuver PAD to Entry PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, mcc_calcs.GETEval(257 * 3600.0 + 25 * 60.0), 42, MST_C_DAY10STATE6);
		break;
	case MST_C_DAY10STATE6:
		UpdateMacro(UTP_PADONLY, PT_AP7ENT, cm->GetStage() == CM_STAGE, 43, MST_ORBIT_ENTRY);
		break;
	case MST_ORBIT_ENTRY:
		switch (SubState) {
		case 0:
			MissionPhase = MMST_ENTRY;
			allocPad(6);// Allocate AP7 Entry Pad
			if (padForm != NULL) {
				// If success
				startSubthread(44, UTP_PADONLY); // Start subthread to fill PAD
			}
			else {
				// ERROR STATE
			}
			setSubState(1);
			// FALL INTO
		case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
			if (SubStateTime > 1 && padState > -1) {
				addMessage("You can has PAD");
				if (padAutoShow == true && padState == 0) { drawPad(); }
				setSubState(2);
			}
			break;
		case 2: // Await landing?
			if (cm->GetStage() == CM_ENTRY_STAGE_SEVEN)
			{
				SlowIfDesired();
				setState(MST_LANDING);
			}
			break;
		}
		break;
	case MST_C_ABORT:
	{
		if (AbortMode == 5) //Earth Orbit Abort
		{
			switch (SubState)
			{
			case 0:
				//Are we past the calculated TIG?
				if (cm->GetStage() >= CM_STAGE || mcc_calcs.GETEval(rtcc->TimeofIgnition))
				{
					setSubState(1); //No deorbit maneuver upcoming, just wait for CM/SM sep
				}
				else
				{
					setSubState(2);
				}
				break;
			case 1:
				if (cm->GetStage() >= CM_ENTRY_STAGE_SEVEN)
				{
					setState(MST_LANDING); //Deorbit maneuver upcoming, wait for Entry PAD
				}
				break;
			case 2:
				if (mcc_calcs.GETEval(rtcc->TimeofIgnition - 1.5*3600.0))
				{
					setState(MST_C_DAY10STATE6);
				}
				break;
			}
		}
	}
	break;
	}
}