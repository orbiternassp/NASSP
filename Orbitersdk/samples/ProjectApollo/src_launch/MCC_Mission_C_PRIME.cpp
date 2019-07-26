/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

MCC sequencing for Mission C Prime

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
#include "MCC_Mission_C_PRIME.h"
#include "iu.h"

void MCC::MissionSequence_C_Prime()
{
	switch (MissionState) {
	case MST_CP_INSERTION: //Ground liftoff time update to TLI Simulation
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval(1.0*3600.0 + 35.0*60.0), 1, MST_CP_EPO1);
		break;
	case MST_CP_EPO1: //TLI Simulation to TLI+90 PAD
		UpdateMacro(UTP_NONE, PT_NONE, true, 2, MST_CP_EPO2);
		break;
	case MST_CP_EPO2: //TLI+90 Maneuver PAD to TLI+5h Maneuver PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 3, MST_CP_EPO3);
		break;
	case MST_CP_EPO3: //TLI+5h P37 PAD to TLI PAD
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 5.0*60.0, 4, MST_CP_EPO4);
		break;
	case MST_CP_EPO4: //TLI PAD to TLI Evaluation
		UpdateMacro(UTP_PADONLY, PT_TLIPAD, rtcc->GETEval(rtcc->calcParams.TLI + 18.0), 5, MST_CP_TRANSLUNAR1);
		break;
	case MST_CP_TRANSLUNAR1: //TLI Evaluation to Block Data 1
		UpdateMacro(UTP_NONE, PT_NONE, true, 6, MST_CP_TRANSLUNAR2, scrubbed, rtcc->GETEval(3.0*3600.0 + 10.0*60.0), MST_CP_EPO1);
		break;
	case MST_CP_TRANSLUNAR2:
		switch (SubState) {
		case 0:
		{
			addMessage("TLI");
			MissionPhase = MMST_TL_COAST;
			setSubState(1);
		}
		break;
		case 1:
		{
			if (cm->stage == CSM_LEM_STAGE) {
				addMessage("SEPARATION");
				setSubState(2);
			}
		}
		break;
		case 2:
		{
			if (rtcc->GETEval(rtcc->calcParams.TLI + 2.0*3600.0 + 5.0*60.0))
			{
				SlowIfDesired();
				setState(MST_CP_TRANSLUNAR3);
			}
		}
		break;
		}
		break;
	case MST_CP_TRANSLUNAR3: //Block Data 1 to MCC1
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TLI + 5.0*3600.0), 10, MST_CP_TRANSLUNAR4);
		break;
	case MST_CP_TRANSLUNAR4: //MCC1 to Block Data 2
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TLI + 9.0*3600.0), 20, MST_CP_TRANSLUNAR5);
		break;
	case MST_CP_TRANSLUNAR5: //Block Data 2 to Block Data 3
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TLI + 22.0*3600.0), 11, MST_CP_TRANSLUNAR6);
		break;
	case MST_CP_TRANSLUNAR6: //Block Data 3 to MCC2
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TLI + 24.0*3600.0), 12, MST_CP_TRANSLUNAR7);
		break;
	case MST_CP_TRANSLUNAR7: //MCC2 to Block Data 4
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TLI + 32.0*3600.0), 21, MST_CP_TRANSLUNAR8);
		break;
	case MST_CP_TRANSLUNAR8: //Block Data 4 to Flyby
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TLI + 41.0*3600.0), 13, MST_CP_TRANSLUNAR9);
		break;
	case MST_CP_TRANSLUNAR9: //Flyby to Fast PC+2
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, StateTime > 5.0*60.0, 40, MST_CP_TRANSLUNAR10);
		break;
	case MST_CP_TRANSLUNAR10: //Fast PC+2 to MCC3
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI - 18.0*3600.0), 42, MST_CP_TRANSLUNAR11);
		break;
	case MST_CP_TRANSLUNAR11: //MCC3 to MCC4
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI - 9.5*3600.0), 22, MST_CP_TRANSLUNAR12);
		break;
	case MST_CP_TRANSLUNAR12: //MCC4 to PC+2
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, StateTime > 5.0*60.0, 23, MST_CP_TRANSLUNAR13);
		break;
	case MST_CP_TRANSLUNAR13: //PC+2 to Fast PC+2
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, StateTime > 5.0*60.0, 41, MST_CP_TRANSLUNAR14);
		break;
	case MST_CP_TRANSLUNAR14: //Fast PC+2 to Prel. LOI-1
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI - 1.0*3600.0 - 50.0*60.0), 42, MST_CP_TRANSLUNAR15);
		break;
	case MST_CP_TRANSLUNAR15: //Prel. LOI-1 to Prel. TEI-1
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, StateTime > 5.0*60.0, 30, MST_CP_TRANSLUNAR16);
		break;
	case MST_CP_TRANSLUNAR16: //Prel. TEI-1 to Prel. TEI-2
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, StateTime > 5.0*60.0, 50, MST_CP_TRANSLUNAR17);
		break;
	case MST_CP_TRANSLUNAR17: //Prel. TEI-2 to Map Update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, StateTime > 5.0*60.0, 51, MST_CP_TRANSLUNAR18);
		break;
	case MST_CP_TRANSLUNAR18: //Map Update to LOI-1
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, rtcc->GETEval(rtcc->calcParams.LOI - 1.0*3600.0 - 5.0*60.0), 60, MST_CP_TRANSLUNAR19);
		break;
	case MST_CP_TRANSLUNAR19: //LOI-1 to TEI-2
		if (MissionPhase == MMST_TL_COAST && rtcc->GETEval(rtcc->calcParams.LOI))
		{
			MissionPhase = MMST_LUNAR_ORBIT;
		}
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI + 1.0*3600.0 + 25.0*60.0 + 31.0), 31, MST_CP_LUNAR_ORBIT1);
		break;
	case MST_CP_LUNAR_ORBIT1: //TEI-2 to LOI-2
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI + 2.0*3600.0 + 30.0*60.0 + 31.0), 105, MST_CP_LUNAR_ORBIT2);
		break;
	case MST_CP_LUNAR_ORBIT2: //LOI-2 to TEI-3 Calc
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI + 3.0*3600.0 + 15.0*60.0 + 31.0), 102, MST_CP_LUNAR_ORBIT3);
		break;
	case MST_CP_LUNAR_ORBIT3: //TEI-3 Calc to TEI-4 Calc
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI + 5.0*3600.0 + 45.0*60.0 + 31.0), 106, MST_CP_LUNAR_ORBIT7);
		break;
	case MST_CP_LUNAR_ORBIT7: //TEI-4 Calc to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI + 7.0*3600.0 + 15.0*60.0 + 31.0), 107, MST_CP_LUNAR_ORBIT9);
		break;
	case MST_CP_LUNAR_ORBIT9: //SV Update to TEI-5 Calc
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval(rtcc->calcParams.LOI + 7.0*3600.0 + 30.0*60.0 + 31.0), 103, MST_CP_LUNAR_ORBIT10);
		break;
	case MST_CP_LUNAR_ORBIT10: //TEI-5 Calc to TEI-6 Calc
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI + 9.0*3600.0 + 15.0*60.0 + 31.0), 108, MST_CP_LUNAR_ORBIT11);
		break;
	case MST_CP_LUNAR_ORBIT11: //TEI-6 Calc to TEI-7 Calc
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI + 11.0*3600.0 + 10.0*60.0 + 31.0), 109, MST_CP_LUNAR_ORBIT12);
		break;
	case MST_CP_LUNAR_ORBIT12: //TEI-7 Calc to TEI-8 Calc
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI + 13.0*3600.0 + 10.0*60.0 + 31.0), 110, MST_CP_LUNAR_ORBIT13);
		break;
	case MST_CP_LUNAR_ORBIT13: //TEI-8 Calc to SV Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI + 15.0*3600.0 + 10.0*60.0 + 31.0), 111, MST_CP_LUNAR_ORBIT14);
		break;
	case MST_CP_LUNAR_ORBIT14: //SV Update to TEI-9 Calc
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval( rtcc->calcParams.LOI + 15.0*3600.0 + 25.0*60.0 + 31.0), 103, MST_CP_LUNAR_ORBIT15);
		break;
	case MST_CP_LUNAR_ORBIT15: // TEI-9 Calc to Prel. TEI-10 Calc
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI + 17.0*3600.0 + 10.0*60.0 + 31.0), 112, MST_CP_LUNAR_ORBIT17);
		break;
	case MST_CP_LUNAR_ORBIT17: //Prel. TEI-10 Calc to TEI-10 Calc
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI + 18.0*3600.0 + 35.0*60.0 + 31.0), 113, MST_CP_LUNAR_ORBIT18);
		break;
	case MST_CP_LUNAR_ORBIT18: //TEI-10 Calc to TEI-11 Calc
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, StateTime > 5.0*60.0, 200, MST_CP_LUNAR_ORBIT19);
		break;
	case MST_CP_LUNAR_ORBIT19: //TEI-11 Calc to TEI
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TEI), 201, MST_CP_TRANSEARTH1);
		break;
	case MST_CP_TRANSEARTH1: //TEI to ENTRY REFSMMAT
		if (rtcc->GETEval(rtcc->calcParams.TEI && MissionPhase == MMST_LUNAR_ORBIT))
		{
			MissionPhase = MMST_TE_COAST;
		}
		if (rtcc->GETEval(rtcc->calcParams.TEI + 45 * 60))
		{
			SlowIfDesired();
			setState(MST_CP_TRANSEARTH2);
		}
		break;
	case MST_CP_TRANSEARTH2: //ENTRY REFSMMAT to MCC5 Update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval(rtcc->calcParams.TEI + 13.5 * 3600.0), 202, MST_CP_TRANSEARTH3);
		break;
	case MST_CP_TRANSEARTH3: //MCC5 Update to MCC6 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TEI + 31.5 * 3600.0), 203, MST_CP_TRANSEARTH4, rtcc->calcParams.TEI + 37.0*3600.0 > rtcc->calcParams.EI - 2.0*3600.0, rtcc->GETEval(rtcc->calcParams.EI - 3.5 * 3600.0), MST_CP_TRANSEARTH6);
		break;
	case MST_CP_TRANSEARTH4: //MCC6 Update to Prel. MCC7 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.EI - 15.0 * 3600.0), 204, MST_CP_TRANSEARTH5, rtcc->calcParams.TEI + 34.0*3600.0 > rtcc->calcParams.EI - 15.0*3600.0, rtcc->GETEval(rtcc->calcParams.EI - 3.5 * 3600.0), MST_CP_TRANSEARTH6);
		break;
	case MST_CP_TRANSEARTH5: //Prel. MCC7 Update to MCC7 Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.EI - 3.5 * 3600.0), 205, MST_CP_TRANSEARTH6);
		break;
	case MST_CP_TRANSEARTH6: //MCC7 Update to Prel. Entry PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, StateTime > 5 * 60, 206, MST_CP_TRANSEARTH7);
		break;
	case MST_CP_TRANSEARTH7: //Prel. Entry PAD to Final Entry PAD
		UpdateMacro(UTP_PADONLY, PT_AP11ENT, rtcc->GETEval(rtcc->calcParams.EI - 45.0*60.0), 207, MST_CP_TRANSEARTH8);
		break;
	case MST_CP_TRANSEARTH8: //Final Entry PAD to Separation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11ENT, cm->stage == CM_STAGE, 208, MST_ENTRY);
		break;
	case MST_ENTRY:
		switch (SubState) {
		case 0:
		{
			MissionPhase = MMST_ENTRY;
			setSubState(1);
		}
		break;
		case 1:
		{
			if (cm->stage == CM_ENTRY_STAGE_SEVEN)
			{
				setState(MST_LANDING);
			}
		}
		break;
		}
		break;
	case MST_CP_ABORT_ORBIT:
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
	case MST_CP_ABORT:
		if (AbortMode == 6)	//Translunar Coast
		{
			switch (SubState) {
			case 0:
			{
				if (rtcc->GETEval(rtcc->calcParams.TEI))
				{
					setSubState(1);
				}
			}
			break;
			case 1:
				if (rtcc->AGCGravityRef(cm) == oapiGetObjectByName("Moon"))
				{
					setSubState(12);//Flyby
				}
				else if (rtcc->calcParams.TEI > rtcc->calcParams.EI - 12.0 * 60 * 60)
				{
					setSubState(2);//Skip directly to normal entry procedures
				}
				else
				{
					setSubState(3);	//Include another course correction
				}
				break;
			case 2:
			{
				if (rtcc->GETEval(rtcc->calcParams.EI - 3.5 * 60 * 60))
				{
					SlowIfDesired();
					setState(MST_CP_TRANSEARTH6);
				}
			}
			break;
			case 3:
			{
				if (rtcc->GETEval(rtcc->calcParams.TEI + 4.0 * 60 * 60))
				{
					SlowIfDesired();
					setSubState(4);
				}
			}
			break;

			case 4:
				allocPad(8); // Allocate AP7 Maneuver Pad
				if (padForm != NULL) {
					// If success
					startSubthread(300, UTP_PADWITHCMCUPLINK); // Start subthread to fill PAD
				}
				else {
					// ERROR STATE
				}
				setSubState(5);
				// FALL INTO
			case 5: // Await pad read-up time (however long it took to compute it and give it to capcom)
				if (SubStateTime > 1 && padState > -1) {
					if (scrubbed)
					{
						if (upMessage[0] != 0)
						{
							addMessage(upMessage);
						}
						freePad();
						scrubbed = false;
						setSubState(10);
					}
					else
					{

						addMessage("You can has PAD");
						if (padAutoShow == true && padState == 0) { drawPad(); }
						// Completed. We really should test for P00 and proceed since that would be visible to the ground.
						addMessage("Ready for uplink?");
						sprintf(PCOption_Text, "Ready for uplink");
						PCOption_Enabled = true;
						setSubState(6);
					}
				}
				break;
			case 6: // Awaiting user response
			case 7: // Negative response / not ready for uplink
				break;
			case 8: // Ready for uplink
				if (SubStateTime > 1 && padState > -1) {
					// The uplink should also be ready, so flush the uplink buffer to the CMC
					this->CM_uplink_buffer();
					// uplink_size = 0; // Reset
					PCOption_Enabled = false; // No longer needed
					if (upDescr[0] != 0)
					{
						addMessage(upDescr);
					}
					setSubState(9);
				}
				break;
			case 9: // Await uplink completion
				if (cm->pcm.mcc_size == 0) {
					addMessage("Uplink completed!");
					NCOption_Enabled = true;
					sprintf(NCOption_Text, "Repeat uplink");
					setSubState(10);
				}
				break;
			case 10: // Await burn
				if (rtcc->GETEval(rtcc->calcParams.EI - 3.5 * 60 * 60))
				{
					SlowIfDesired();
					setState(MST_CP_TRANSEARTH6);
				}
				break;
			case 11: //Repeat uplink
			{
				NCOption_Enabled = false;
				setSubState(4);
			}
			break;
			case 12:
			{
				//Wait for 10 minutes so the burn is over, then calculate Pericynthion time for return trajectory
				if (rtcc->GETEval(rtcc->calcParams.TEI + 10.0*60.0))
				{
					rtcc->calcParams.LOI = rtcc->PericynthionTime(cm);
					setSubState(13);
				}
			}
			case 13: //Flyby, go to nominal TEC procedures
			{
				if (rtcc->GETEval(rtcc->calcParams.LOI + 45.0*60.0) && rtcc->GETEval(rtcc->calcParams.TEI + 45.0*60.0))
				{
					rtcc->calcParams.TEI = rtcc->calcParams.LOI;
					setState(MST_CP_TRANSEARTH1);
				}
			}
			break;
			}
		}
		else if (AbortMode == 7) //Lunar Orbit
		{
			if (rtcc->GETEval(rtcc->calcParams.TEI))
			{
				setState(MST_CP_TRANSEARTH1);
			}
		}
		else if (AbortMode == 8)
		{
			//How to Abort?
		}
	}
}