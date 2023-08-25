/****************************************************************************
This file is part of Project Apollo - NASSP

MCC sequencing for Skylab missions

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

#include "mcc.h"
#include "rtcc.h"
#include "MCC_Mission_SL.h"

void MCC::MissionSequence_SL()
{
	switch (MissionState)
	{
	case MST_SL_PRELAUNCH: //Scenario start to targeting update
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(-95.0*60.0), 1, MST_SL_PRELAUNCH_TARGETING);
		break;
	case MST_SL_PRELAUNCH_TARGETING: //Targeting update to prelaunch
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(-16.0*60.0), 2, MST_1B_PRELAUNCH);
		break;
	case MST_SL_INSERTION: //Insertion to Rendezvous planning
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(30.0*60.0), 10, MST_SL_RENDEZVOUS_PLAN);
		break;
	case MST_SL_RENDEZVOUS_PLAN: //Rendezvous planning to NC1 preliminary update
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(1.0*3600.0 + 35.0*60.0), 11, MST_SL_PRELIM_NC1);
		break;
	case MST_SL_PRELIM_NC1: //NC1 preliminary update to NC1 final update
		UpdateMacro(UTP_NONE, PT_AP7MNV, rtcc->GETEval2(1.0 * 3600.0 + 55.0*60.0), 12, MST_SL_FINAL_NC1);
		break;
	case MST_SL_FINAL_NC1: //NC1 final update to NC2 preliminary update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, rtcc->GETEval2(3.0 * 3600.0 + 5.0*60.0), 13, MST_SL_PRELIM_NC2);
		break;
	case MST_SL_PRELIM_NC2: //NC2 preliminary update to NC2 final update
		UpdateMacro(UTP_NONE, PT_AP7MNV, rtcc->GETEval2(4.0 * 3600.0 + 15.0*60.0), 14, MST_SL_FINAL_NC2);
		break;
	case MST_SL_FINAL_NC2: //NC final update to NCC final update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, rtcc->GETEval2(5.0 * 3600.0 + 5.0*60.0), 15, MST_SL_FINAL_NCC);
		break;
	case MST_SL_FINAL_NCC: //NC final update to NCC final update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP7MNV, SubStateTime > 2.0*60.0, 16, MST_SL_FINAL_NSR);
		break;
	}
}