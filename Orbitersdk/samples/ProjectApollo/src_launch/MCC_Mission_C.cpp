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
#include "MCC_Mission_C.h"

void MCC::MissionSequence_C()
{
	switch (MissionState) {
	case MST_C_INSERTION:
		// CALLOUTS TO MAKE:
		// Acknowledge CDR's SECO call
		// Acknowledge CDR's DSKY reports
		// GO FOR ORBIT
		// S4B SAFED
		// ORBIT APOGEE/PERIGEE IN (ROUND) NAUTICAL MILES
		// LOX dump time and expected dV
		// NAV uplink of some kind (state vector?) and GO for CDR to leave seat and/or unsuiting (?)
		// S4B Passivation complete (OK to terminate P47)
		// S4B takeover experiment
		// GO for pyro arm

		// Await separation.
		switch (SubState) {
		case 0:
			if (cm->GetMissionTime() > 56.0*60.0)
			{
				SlowIfDesired();
				setSubState(1);
			}
			break;
		case 1:
			startSubthread(50, UTP_CMCUPLINKONLY); // Start subthread to fill PAD
			setSubState(2);
			// FALL INTO
		case 2: // Await pad read-up time (however long it took to compute it and give it to capcom)
			if (SubStateTime > 1) {
				// Completed. We really should test for P00 and proceed since that would be visible to the ground.
				addMessage("Ready for uplink?");
				sprintf(PCOption_Text, "Ready for uplink");
				PCOption_Enabled = true;
				setSubState(3);
			}
			break;
		case 3: // Awaiting user response
		case 4: // Negative response / not ready for uplink
			break;
		case 5: // Ready for uplink
			if (SubStateTime > 1) {
				// The uplink should also be ready, so flush the uplink buffer to the CMC
				this->CM_uplink_buffer();
				// uplink_size = 0; // Reset
				PCOption_Enabled = false; // No longer needed
				if (upDescr[0] != 0)
				{
					addMessage(upDescr);
				}
				setSubState(6);
			}
			break;
		case 6: // Await uplink completion
			if (cm->pcm.mcc_size == 0) {
				addMessage("Uplink completed!");
				NCOption_Enabled = true;
				sprintf(NCOption_Text, "Repeat uplink");
				setSubState(7);
			}
			break;
		case 7: // Await separation
			if (cm->stage == CSM_LEM_STAGE) {
				addMessage("SEPARATION");
				setState(MST_C_SEPARATION);
			}
			else {
				break;
			}
			break;
		case 8: //Repeat uplink
		{
			NCOption_Enabled = false;
			setSubState(0);
		}
		break;
		}
		// FALL INTO
	case MST_C_SEPARATION:
		// Ends with 1ST RDZ PHASING BURN
		// The phasing burn was intended to place the spacecraft 76.5 n mi ahead of the S-IVB in 23 hours.
		if (cm->MissionTime > 3 * 60 * 60 + 5 * 60)
		{
			UpdateMacro(UTP_PADONLY, PT_AP7MNV, cm->MissionTime > 4 * 60 * 60 + 45 * 60, 1, MST_C_COAST1);
		}
		break;
	case MST_C_COAST1: // 6-4 Deorbit Maneuver update to Block Data 2
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, cm->MissionTime > 10 * 60 * 60 + 30 * 60, 2, MST_C_COAST2);
		break;
	case MST_C_COAST2: //Block Data 2 to 2nd Phasing Maneuver Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 14 * 60 * 60 + 16 * 60, 3, MST_C_COAST3);
		break;
	case MST_C_COAST3: // 2nd Phasing Maneuver Update to Block Data 3
		UpdateMacro(UTP_PADONLY, PT_AP7MNV, cm->MissionTime > 21 * 60 * 60 + 50 * 60, 4, MST_C_COAST4);
		break;
	case MST_C_COAST4: // Block Data 3 to Preliminary NCC1 Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 22 * 60 * 60 + 25 * 60, 5, MST_C_COAST5);
		break;
	case MST_C_COAST5: // Preliminary NCC1 Update to Final NCC1 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, cm->MissionTime > 25 * 60 * 60 + 30 * 60, 6, MST_C_COAST6);
		break;
	case MST_C_COAST6: // Final NCC1 Update to NCC2 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, cm->MissionTime > 27 * 60 * 60, 7, MST_C_COAST7);
		break;
	case MST_C_COAST7: //  NCC2 Update to NSR Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, cm->MissionTime > 27 * 60 * 60 + 32 * 60, 8, MST_C_COAST8);
		break;
	case MST_C_COAST8: // NSR Update to TPI Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, cm->MissionTime > 28 * 60 * 60 + 50 * 60, 9, MST_C_COAST9);
		break;
	case MST_C_COAST9: // TPI Update to Final Separation Maneuver update
		UpdateMacro(UTP_PADONLY, PT_AP7TPI, cm->MissionTime > 30 * 60 * 60 + 9 * 60, 10, MST_C_COAST10);
		break;
	case MST_C_COAST10: // Final Separation Maneuver update to Block Data 4
		UpdateMacro(UTP_PADONLY, PT_AP7MNV, cm->MissionTime > 30 * 60 * 60 + 54 * 60, 11, MST_C_COAST11);
		break;
	case MST_C_COAST11: // Block Data 4 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 34 * 60 * 60 + 45 * 60, 12, MST_C_COAST12);
		break;
	case MST_C_COAST12: // SV Update to Block Data 5
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 39 * 60 * 60 + 21 * 60, 53, MST_C_COAST13);
		break;
	case MST_C_COAST13: // Block Data 5 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 44 * 60 * 60, 13, MST_C_COAST14);
		break;
	case MST_C_COAST14: // SV Update to Block Data 6
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 49 * 60 * 60 + 12 * 60, 53, MST_C_COAST15);
		break;
	case MST_C_COAST15: // Block Data 6 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 50 * 60 * 60 + 15 * 60, 14, MST_C_COAST16);
		break;
	case MST_C_COAST16: // SV Update to Block Data 7
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 57 * 60 * 60 + 4 * 60, 53, MST_C_COAST17);
		break;
	case MST_C_COAST17: // Block Data 7 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 67 * 60 * 60 + 50 * 60, 15, MST_C_COAST18);
		break;
	case MST_C_COAST18: // SV Update to Block Data 8
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 68 * 60 * 60 + 37 * 60, 52, MST_C_COAST19);
		break;
	case MST_C_COAST19: // Block Data 8 to SPS-3
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 72 * 60 * 60, 16, MST_C_COAST20);
		break;
	case MST_C_COAST20: // SPS-3 to Block Data 9
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, cm->MissionTime > 77 * 60 * 60 + 5 * 60, 17, MST_C_COAST21);
		break;
	case MST_C_COAST21: // Block Data 9 to Block Data 10
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 85 * 60 * 60 + 52 * 60, 18, MST_C_COAST22);
		break;
	case MST_C_COAST22: // Block Data 10 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 90 * 60 * 60, 19, MST_C_COAST23);
		break;
	case MST_C_COAST23: // SV Update to SV Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 94 * 60 * 60, 52, MST_C_COAST24);
		break;
	case MST_C_COAST24: // SV Update to Block Data 11
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 96 * 60 * 60 + 21 * 60, 52, MST_C_COAST25);
		break;
	case MST_C_COAST25: // Block Data 11 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 97 * 60 * 60 + 40 * 60, 20, MST_C_COAST26);
		break;
	case MST_C_COAST26: // SV Update to Block Data 12
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 105 * 60 * 60 + 10 * 60, 52, MST_C_COAST27);
		break;
	case MST_C_COAST27: // Block Data 12 to Block Data 13
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 116 * 60 * 60 + 3 * 60, 21, MST_C_COAST28);
		break;
	case MST_C_COAST28: // Block Data 13 to SPS-4
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 117 * 60 * 60 + 30 * 60, 22, MST_C_COAST29);
		break;
	case MST_C_COAST29: // SPS-4 to SV Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, cm->MissionTime > 121 * 60 * 60 + 36 * 60, 23, MST_C_COAST30);
		break;
	case MST_C_COAST30: // SV Update to SV Update
		UpdateMacro(UTP_PADONLY, PT_P27PAD, cm->MissionTime > 123 * 60 * 60 + 40 * 60, 54, MST_C_COAST31);
		break;
	case MST_C_COAST31: // SV Update to Block Data 14
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 125 * 60 * 60 + 12 * 60, 52, MST_C_COAST32);
		break;
	case MST_C_COAST32: // Block Data 14 to Block Data 15
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 134 * 60 * 60 + 48 * 60, 24, MST_C_COAST33);
		break;
	case MST_C_COAST33: // Block Data 15 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 139 * 60 * 60 + 40 * 60, 25, MST_C_COAST34);
		break;
	case MST_C_COAST34: // SV Update to SV Update
		UpdateMacro(UTP_PADONLY, PT_P27PAD, cm->MissionTime > 143 * 60 * 60 + 20 * 60, 54, MST_C_COAST35);
		break;
	case MST_C_COAST35: // SV Update to Block Data 16
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 144 * 60 * 60 + 11 * 60, 52, MST_C_COAST36);
		break;
	case MST_C_COAST36: // Block Data 16 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 147 * 60 * 60 + 10 * 60, 26, MST_C_COAST37);
		break;
	case MST_C_COAST37: // SV Update to Block Data 17
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 153 * 60 * 60 + 47 * 60, 52, MST_C_COAST38);
		break;
	case MST_C_COAST38: // Block Data 17 to SPS-5
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 161 * 60 * 60 + 18 * 60, 27, MST_C_COAST39);
		break;
	case MST_C_COAST39: // SPS-5 to Block Data 18
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, cm->MissionTime > 161 * 60 * 60 + 59 * 60, 28, MST_C_COAST40);
		break;
	case MST_C_COAST40: // Block Data 18 to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 169 * 60 * 60, 29, MST_C_COAST41);
		break;
	case MST_C_COAST41: // SV Update to Block Data 19
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 172 * 60 * 60 + 42 * 60, 52, MST_C_COAST42);
		break;
	case MST_C_COAST42: // Block Data 19 to Block Data 20
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 180 * 60 * 60 + 56 * 60, 30, MST_C_COAST43);
		break;
	case MST_C_COAST43: // Block Data 20 to Block Data 21
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 192 * 60 * 60 + 17 * 60, 31, MST_C_COAST44);
		break;
	case MST_C_COAST44: // Block Data 21 to Block Data 22
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 201 * 60 * 60 + 55 * 60, 32, MST_C_COAST45);
		break;
	case MST_C_COAST45: // Block Data 22 to SPS-6
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 205 * 60 * 60 + 25 * 60, 33, MST_C_COAST46);
		break;
	case MST_C_COAST46: // SPS-6 to Block Data 23
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, cm->MissionTime > 210 * 60 * 60 + 11 * 60, 34, MST_C_COAST47);
		break;
	case MST_C_COAST47: // Block Data 23 to SV PAD
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 213 * 60 * 60, 35, MST_C_COAST48);
		break;
	case MST_C_COAST48: // SV PAD to P27 PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 214 * 60 * 60 + 10 * 60, 52, MST_C_COAST49);
		break;
	case MST_C_COAST49: //P27 PAD to SV PAD
		UpdateMacro(UTP_PADONLY, PT_P27PAD, cm->MissionTime > 216 * 60 * 60, 36, MST_C_COAST50);
		break;
	case MST_C_COAST50: //SV PAD to SV PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 217 * 60 * 60, 52, MST_C_COAST51);
		break;
	case MST_C_COAST51: //SV PAD to Block Data 24
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 220 * 60 * 60 + 43 * 60, 52, MST_C_COAST52);
		break;
	case MST_C_COAST52: // Block Data 24 to Block Data 25
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 230 * 60 * 60 + 24 * 60, 37, MST_C_COAST53);
		break;
	case MST_C_COAST53: // Block Data 25 to SPS-7
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 233 * 60 * 60 + 27 * 60, 38, MST_C_COAST54);
		break;
	case MST_C_COAST54: // SPS-7 to SV PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, cm->MissionTime > 240 * 60 * 60 + 20 * 60, 39, MST_C_COAST55);
		break;
	case MST_C_COAST55: // SV PAD to Block Data 26
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 241 * 60 * 60 + 39 * 60, 52, MST_C_COAST56);
		break;
	case MST_C_COAST56: // Block Data 26 to Block Data 27
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 248 * 60 * 60 + 56 * 60, 40, MST_C_COAST57);
		break;
	case MST_C_COAST57: // Block Data 27 to SV PAD
		UpdateMacro(UTP_PADONLY, PT_AP7BLK, cm->MissionTime > 255 * 60 * 60, 41, MST_C_COAST58);
		break;
	case MST_C_COAST58: // SV PAD to Deorbit Maneuver
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7NAV, cm->MissionTime > 257 * 60 * 60 + 20 * 60, 52, MST_C_COAST59);
		break;
	case MST_C_COAST59: // Deorbit Maneuver PAD to Entry PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, cm->MissionTime > 257 * 60 * 60 + 25 * 60, 42, MST_C_COAST60);
		break;
	case MST_C_COAST60:
		UpdateMacro(UTP_PADONLY, PT_AP7ENT, cm->stage == CM_STAGE, 43, MST_ORBIT_ENTRY);
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
			if (cm->stage == CM_ENTRY_STAGE_SEVEN)
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
			if (cm->stage == CM_ENTRY_STAGE_SEVEN)
			{
				setState(MST_LANDING);
			}
		}
	}
	break;
	}
}