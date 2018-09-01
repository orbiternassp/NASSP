/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

MCC sequencing for Mission G

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
#include "sivb.h"
#include "mcc.h"
#include "rtcc.h"
#include "MCC_Mission_G.h"
#include "iu.h"

void MCC::MissionSequence_G()
{
	switch (MissionState)
	{
	case MST_G_INSERTION: //Ground liftoff time update to TLI Simulation
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval(1.0*3600.0 + 30.0*60.0), 10, MST_G_EPO1);
		break;
	case MST_G_EPO1: //TLI Simulation to TLI+90 PAD
		UpdateMacro(UTP_NONE, PT_NONE, true, 11, MST_G_EPO2);
		break;
	case MST_G_EPO2: //TLI+90 Maneuver PAD to TLI+5h P37 PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 12, MST_G_EPO3);
		break;
	case MST_G_EPO3: //TLI+5h P37 PAD to TLI PAD
		UpdateMacro(UTP_PADONLY, PT_P37PAD, SubStateTime > 3.0*60.0, 13, MST_G_EPO4);
		break;
	case MST_G_EPO4: //TLI PAD to TLI Evaluation
		UpdateMacro(UTP_PADONLY, PT_TLIPAD, rtcc->GETEval(rtcc->calcParams.TLI + 18.0), 14, MST_G_TRANSLUNAR1);
		break;
	case MST_G_TRANSLUNAR1: //TLI Evaluation to Evasive Maneuver Update
		UpdateMacro(UTP_NONE, PT_NONE, true, 15, MST_G_TRANSLUNAR2, scrubbed, rtcc->GETEval(3.0*3600.0), MST_G_EPO1);
		break;
	case MST_G_TRANSLUNAR2:
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
			if (rtcc->GETEval(rtcc->calcParams.TLI + 3600.0 + 10.0*60.0))
			{
				SlowIfDesired();
				setState(MST_G_TRANSLUNAR3);
			}
		}
		break;
		}
		break;
	case MST_G_TRANSLUNAR3: //Evasive Maneuver Update to TB8 Enable
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TLI + 3600.0 + 30.0*60.0), 16, MST_G_TRANSLUNAR4);
		break;
	case MST_G_TRANSLUNAR4:  //TB8 enable to Block Data 1
		switch (SubState) {
		case 0:
		{
			if (cm->GetStage() >= CSM_LEM_STAGE)
			{
				setSubState(1);
			}
		}
		break;
		case 1:
		{
			if (SubStateTime > 2.0*60.0)
			{
				setSubState(2);
			}
		}
		break;
		case 2:
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

			sivb->GetIU()->dcs.Uplink(DCSUPLINK_TIMEBASE_8_ENABLE, NULL);
			setSubState(3);
		}
		break;
		case 3:
			if (rtcc->GETEval(rtcc->calcParams.TLI + 3.0*3600.0))
			{
				SlowIfDesired();
				setState(MST_G_TRANSLUNAR5);
			}
			break;
		}
		break;
	case MST_G_TRANSLUNAR5: //Block Data 1 to MCC-1 Calculation
		UpdateMacro(UTP_PADONLY, PT_P37PAD, rtcc->GETEval(rtcc->calcParams.TLI + 3.0*3600.0 + 20.0*60.0), 17, MST_G_TRANSLUNAR6);
		break;
	case MST_G_TRANSLUNAR6: //MCC-1 Calculation to MCC-1 update (or PTC REFSMMAT)
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval(rtcc->calcParams.TLI + 7.0*3600.0 + 10.0*60.0), 20, MST_G_TRANSLUNAR7, scrubbed, rtcc->GETEval(rtcc->calcParams.TLI + 4.0*3600.0), MST_G_TRANSLUNAR_NO_MCC1_1);
		break;
	case MST_G_TRANSLUNAR7: //MCC-1 update to PTC REFSMMAT update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TLI + 9.0*3600.0 + 5.0*60.0), 21, MST_G_TRANSLUNAR8);
		break;
	case MST_G_TRANSLUNAR_NO_MCC1_1: //PTC REFSMMAT update to SV update (MCC-1 was scrubbed)
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval(rtcc->calcParams.TLI + 7.0*3600.0 + 10.0*60.0), 19, MST_G_TRANSLUNAR_NO_MCC1_2);
		break;
	case MST_G_TRANSLUNAR_NO_MCC1_2: //SV update to Block Data 2 (MCC-1 was scrubbed)
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval(rtcc->calcParams.TLI + 9.0*3600.0 + 5.0*60.0), 1, MST_G_TRANSLUNAR9);
		break;
	case MST_G_TRANSLUNAR8: //PTC REFSMMAT update to Block Data 2
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, true, 19, MST_G_TRANSLUNAR9);
		break;
	case MST_G_TRANSLUNAR9: //Block Data 2 to MCC-2 update
		UpdateMacro(UTP_PADONLY, PT_P37PAD, rtcc->GETEval(rtcc->calcParams.TLI + 22.0*3600.0 + 40.0*60.0), 18, MST_G_TRANSLUNAR10);
		break;
	case MST_G_TRANSLUNAR10: //MCC-2 update to Lunar Flyby PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TLI + 32.0*3600.0 + 10.0*60.0), 22, MST_G_TRANSLUNAR11);
		break;
	case MST_G_TRANSLUNAR11: //Lunar Flyby PAD to MCC-3
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI - 24.0*3600.0 - 40.0*60.0), 23, MST_G_TRANSLUNAR12);
		break;
	case MST_G_TRANSLUNAR12: //MCC-3 update to MCC-4 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI - 6.0*3600.0 - 25.0*60.0), 24, MST_G_TRANSLUNAR13);
		break;
	case MST_G_TRANSLUNAR13: //MCC-4 update to PC+2 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 25, MST_G_TRANSLUNAR14);
		break;
	case MST_G_TRANSLUNAR14: //PC+2 update to LOI-1 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI - 2.0*3600.0 - 50.0*60.0), 26, MST_G_TRANSLUNAR15);
		break;
	case MST_G_TRANSLUNAR15: //LOI-1 update to TEI-1 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI - 2.0*3600.0 - 5.0*60.0), 30, MST_G_TRANSLUNAR16);
		break;
	case MST_G_TRANSLUNAR16: //TEI-1 update to TEI-4 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 5.0*60.0, 40, MST_G_TRANSLUNAR17);
		break;
	case MST_G_TRANSLUNAR17: //TEI-4 update to Rev 1 Map Update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI - 40.0*60.0), 41, MST_G_TRANSLUNAR18);
		break;
	case MST_G_TRANSLUNAR18: //Rev 1 Map Update to lunar orbit phase begin
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, rtcc->GETEval(rtcc->calcParams.LOI), 60, MST_G_LUNAR_ORBIT_LOI_DAY_1);
		break;
	case MST_G_LUNAR_ORBIT_LOI_DAY_1: //Lunar orbit phase begin
		switch (SubState) {
		case 0:
			MissionPhase = MMST_LUNAR_ORBIT;
			setSubState(1);
			break;
		case 1:
			if (MoonRev >= 2 && MoonRevTime > 35.0*60.0)
			{
				SlowIfDesired();
				setState(MST_G_LUNAR_ORBIT_LOI_DAY_2);
			}
			break;
		}
		break;
	case MST_G_LUNAR_ORBIT_LOI_DAY_2: //LOI-2 update to TEI-5 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 31, MST_G_LUNAR_ORBIT_LOI_DAY_3);
		break;
	case MST_G_LUNAR_ORBIT_LOI_DAY_3: //TEI-5 update to A-1 landmark tracking update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, MoonRev >= 3 && MoonRevTime > 50.0*60.0, 42, MST_G_LUNAR_ORBIT_LOI_DAY_4);
		break;
	case MST_G_LUNAR_ORBIT_LOI_DAY_4: //A-1 landmark tracking update to TEI-11 update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, MoonRev >= 4 && MoonRevTime > 1.0*3600.0 + 20.0*60.0, 61, MST_G_LUNAR_ORBIT_LOI_DAY_5);
		break;
	case MST_G_LUNAR_ORBIT_LOI_DAY_5: //TEI-11 update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, true, 43, MST_G_LUNAR_ORBIT_LOI_DAY_6);
		break;
	case MST_G_LUNAR_ORBIT_LOI_DAY_6: //State vector update to TEI-30 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 10 && MoonRevTime > 1.0*3600.0, 1, MST_G_LUNAR_ORBIT_PDI_DAY_1);
		break;
	case MST_G_LUNAR_ORBIT_PDI_DAY_1: //TEI-30 update to CMC LS Update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, MoonRev >= 11 && MoonRevTime > 40.0*60.0, 44, MST_G_LUNAR_ORBIT_PDI_DAY_2);
		break;
	case MST_G_LUNAR_ORBIT_PDI_DAY_2: //CMC LS Update and Lmk 130 Landmark Tracking PAD to CSM DAP Data
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11LMARKTRKPAD, SubStateTime > 3.0*60.0, 32, MST_G_LUNAR_ORBIT_PDI_DAY_4);
		break;
	case MST_G_LUNAR_ORBIT_PDI_DAY_4: //CSM DAP Data to LM Activation Data
		UpdateMacro(UTP_PADONLY, PT_AP10DAPDATA, MoonRev >= 12 && MoonRevTime > 30.0*60.0, 33, MST_G_LUNAR_ORBIT_PDI_DAY_7);
		break;
	case MST_G_LUNAR_ORBIT_PDI_DAY_7: //LM Activation Data to LGC activation update
		UpdateMacro(UTP_PADONLY, PT_LMACTDATA, MoonRev >= 12 && MoonRevTime > 65.0*60.0, 34, MST_G_LUNAR_ORBIT_PDI_DAY_9);
		break;
	case MST_G_LUNAR_ORBIT_PDI_DAY_9: //LGC activation update to AGC activation update
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, SubStateTime > 3.0*60.0, 35, MST_G_LUNAR_ORBIT_PDI_DAY_10);
		break;
	case MST_G_LUNAR_ORBIT_PDI_DAY_10: //AGC activation update to Separation maneuver update
		UpdateMacro(UTP_PADONLY, PT_AP11AGSACT, SubStateTime > 3.0*60.0, 36, MST_G_LUNAR_ORBIT_PDI_DAY_11);
		break;
	case MST_G_LUNAR_ORBIT_PDI_DAY_11: //Separation maneuver update to DOI update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, MoonRev >= 13 && MoonRevTime > 30.0*60.0, 37, MST_G_LUNAR_ORBIT_PDI_DAY_12);
		break;
	case MST_G_LUNAR_ORBIT_PDI_DAY_12: //DOI update to PDI PAD
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11LMMNV, SubStateTime > 3.0*60.0, 38, MST_G_LUNAR_ORBIT_PDI_DAY_13);
		break;
	case MST_G_LUNAR_ORBIT_PDI_DAY_13: //PDI PAD to PDI Abort PAD
		UpdateMacro(UTP_PADONLY, PT_AP11PDIPAD, SubStateTime > 3.0*60.0, 70, MST_G_LUNAR_ORBIT_PDI_DAY_14);
		break;
	case MST_G_LUNAR_ORBIT_PDI_DAY_14: //PDI Abort PAD to No PDI+12 PAD
		UpdateMacro(UTP_PADONLY, PT_PDIABORTPAD, SubStateTime > 3.0*60.0, 71, MST_G_LUNAR_ORBIT_PDI_DAY_15);
		break;
	case MST_G_LUNAR_ORBIT_PDI_DAY_15: //No PDI+12 PAD to Lunar Surface PAD 1
		UpdateMacro(UTP_PADONLY, PT_AP11LMMNV, rtcc->GETEval(rtcc->calcParams.SEP + 3.0*60.0) && SubStateTime > 3.0*60.0, 72, MST_G_LUNAR_ORBIT_PRE_DOI_1);
		break;
	case MST_G_LUNAR_ORBIT_PRE_DOI_1: //Lunar Surface PAD 1 to Lunar Surface PAD 2
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11T2ABORTPAD, SubStateTime > 3.0*60.0, 73, MST_G_LUNAR_ORBIT_PRE_DOI_2);
		break;
	case MST_G_LUNAR_ORBIT_PRE_DOI_2: //Lunar Surface PAD 2 to CSM Rescue PAD
		UpdateMacro(UTP_PADONLY, PT_AP11T3ABORTPAD, SubStateTime > 3.0*60.0, 74, MST_G_LUNAR_ORBIT_PRE_DOI_3);
		break;
	case MST_G_LUNAR_ORBIT_PRE_DOI_3: //CSM Rescue PAD to CSM P76 PADs
		UpdateMacro(UTP_PADONLY, PT_PDIABORTPAD, SubStateTime > 3.0*60.0, 75, MST_G_LUNAR_ORBIT_PRE_DOI_4);
		break;
	case MST_G_LUNAR_ORBIT_PRE_DOI_4: //CSM P76 PADs to DOI Evaluation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11P76PAD, rtcc->GETEval(rtcc->calcParams.DOI + 45.0*60.0), 76, MST_G_LUNAR_ORBIT_PRE_PDI_1);
		break;
	case MST_G_LUNAR_ORBIT_PRE_PDI_1: //DOI Evaluation to PDI Evaluation
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval(rtcc->calcParams.PDI + 2.0*60.0), 77, MST_G_LUNAR_ORBIT_PRE_LANDING_1);
		break;
	case MST_G_LUNAR_ORBIT_PRE_LANDING_1: //PDI Evaluation to landing confirmation
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->calcParams.tgt->GroundContact(), 78, MST_G_LUNAR_ORBIT_POST_LANDING_1);
		break;
	case MST_G_LUNAR_ORBIT_POST_LANDING_1: //Landing confirmation to T1
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval(rtcc->calcParams.PDI + 15.0*60.0), 79, MST_G_LUNAR_ORBIT_POST_LANDING_2);
		break;
	case MST_G_LUNAR_ORBIT_POST_LANDING_2: //T1 to T2
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval(rtcc->calcParams.PDI + 19.0*60.0 + 30.0), 80, MST_G_LUNAR_ORBIT_POST_LANDING_3);
		break;
	case MST_G_LUNAR_ORBIT_POST_LANDING_3: //T2 to LM Tracking PAD
		UpdateMacro(UTP_NONE, PT_NONE, MoonRev >= 14 && MoonRevTime > 90.0*60.0, 81, MST_G_LUNAR_ORBIT_POST_LANDING_4);
		break;
	case MST_G_LUNAR_ORBIT_POST_LANDING_4: //LM Tracking PAD to LM Ascent PAD
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, MoonRev >= 15 && MoonRevTime > 5.0*60.0, 63, MST_G_LUNAR_ORBIT_POST_LANDING_5);
		break;
	case MST_G_LUNAR_ORBIT_POST_LANDING_5: //LM Ascent PAD to CSI Data Card
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11LMASCPAD, SubStateTime > 3.0*60.0, 90, MST_G_LUNAR_ORBIT_POST_LANDING_6);
		break;
	case MST_G_LUNAR_ORBIT_POST_LANDING_6: //CSI Data Card to Liftoff Times Update 1
		UpdateMacro(UTP_PADONLY, PT_AP10CSI, MoonRev >= 15 && MoonRevTime > 40.0*60.0, 91, MST_G_LUNAR_ORBIT_POST_LANDING_7);
		break;
	case MST_G_LUNAR_ORBIT_POST_LANDING_7: //Liftoff Times Update 1 to Plane Change Evaluation
		UpdateMacro(UTP_PADONLY, PT_LIFTOFFTIMES, MoonRev >= 15 && MoonRevTime > 70.0*60.0, 92, MST_G_LUNAR_ORBIT_POST_LANDING_8);
		break;
	case MST_G_LUNAR_ORBIT_POST_LANDING_8: //Plane Change Evaluation to PC REFSMMAT or SV update (if PC scrubbed)
		UpdateMacro(UTP_NONE, PT_NONE, true, 93, MST_G_LUNAR_ORBIT_PLANE_CHANGE_1, scrubbed, MoonRev >= 16 && MoonRevTime > 40.0*60.0, MST_G_LUNAR_ORBIT_NO_PLANE_CHANGE_1);
		break;
	case MST_G_LUNAR_ORBIT_NO_PLANE_CHANGE_1: //SV update to LM Liftoff Times Update 2
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 16 && MoonRevTime > 95.0*60.0, 1, MST_G_LUNAR_ORBIT_EVA_DAY_1);
		break;
	case MST_G_LUNAR_ORBIT_PLANE_CHANGE_1: //PC REFSMMAT to PC Update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 16 && MoonRevTime > 40.0*60.0, 94, MST_G_LUNAR_ORBIT_PLANE_CHANGE_2);
		break;
	case MST_G_LUNAR_ORBIT_PLANE_CHANGE_2: //PC Update to Lunar Liftoff REFSMMAT Uplink
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, MoonRev >= 16 && MoonRevTime > 95.0*60.0, 95, MST_G_LUNAR_ORBIT_EVA_DAY_1);
		break;
	case MST_G_LUNAR_ORBIT_EVA_DAY_1: //Lunar Liftoff REFSMMAT Uplink to LM Liftoff Times Update 2
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 18 && MoonRevTime > 35.0*60.0, 96, MST_G_LUNAR_ORBIT_EVA_DAY_2);
		break;
	case MST_G_LUNAR_ORBIT_EVA_DAY_2: //LM Liftoff Times Update 2 to LM Acquisition Time update
		UpdateMacro(UTP_PADONLY, PT_LIFTOFFTIMES, MoonRev >= 19 && MoonRevTime > 60.0*60.0, 97, MST_G_LUNAR_ORBIT_EVA_DAY_3);
		break;
	case MST_G_LUNAR_ORBIT_EVA_DAY_3: //LM Acquisition Time update to LM Liftoff Time update 3
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, MoonRev >= 21 && MoonRevTime > 65.0*60.0, 64, MST_G_LUNAR_ORBIT_EVA_DAY_4);
		break;
	case MST_G_LUNAR_ORBIT_EVA_DAY_4: //LM Liftoff Times Update 3 to LGC CSM state vector update
		UpdateMacro(UTP_PADONLY, PT_LIFTOFFTIMES, MoonRev >= 24 && MoonRevTime > 35.0*60.0, 98, MST_G_LUNAR_ORBIT_ASCENT_DAY_1);
		break;
	case MST_G_LUNAR_ORBIT_ASCENT_DAY_1: //LGC CSM state vector update to Nominal Insertion targeting
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, MoonRev >= 24 && MoonRevTime > 70.0*60.0, 1, MST_G_LUNAR_ORBIT_ASCENT_DAY_2);
		break;
	case MST_G_LUNAR_ORBIT_ASCENT_DAY_2: //Nominal Insertion targeting to CMC State Vector uplinks
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, SubStateTime > 1.0*60.0, 100, MST_G_LUNAR_ORBIT_ASCENT_DAY_3);
		break;
	case MST_G_LUNAR_ORBIT_ASCENT_DAY_3: //CMC State Vector uplinks to LM Ascent PAD
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 1.0*60.0, 101, MST_G_LUNAR_ORBIT_ASCENT_DAY_4);
		break;
	case MST_G_LUNAR_ORBIT_ASCENT_DAY_4: //LM Ascent PAD to CSI Data Card
		UpdateMacro(UTP_PADONLY, PT_AP11LMASCPAD, SubStateTime > 5.0*60.0, 102, MST_G_LUNAR_ORBIT_ASCENT_DAY_5);
		break;
	case MST_G_LUNAR_ORBIT_ASCENT_DAY_5: //CSI Data Card to LM Liftoff Evaluation
		UpdateMacro(UTP_PADONLY, PT_AP10CSI, rtcc->GETEval(rtcc->calcParams.LunarLiftoff + 20.0), 103, MST_G_LUNAR_ORBIT_ASCENT_DAY_6);
		break;
	case MST_G_LUNAR_ORBIT_ASCENT_DAY_6: //LM Liftoff Evaluation to CMC LM State Vector update
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval(rtcc->calcParams.Insertion + 120.0), 104, MST_G_LUNAR_ORBIT_ASCENT_DAY_7, scrubbed, SubStateTime > 15.0*60.0, MST_G_LUNAR_ORBIT_ASCENT_DAY_2);
		break;
	case MST_G_LUNAR_ORBIT_ASCENT_DAY_7: //CMC LM State Vector update to CSM state vector update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 27 && MoonRevTime > 95.0*60.0, 2, MST_G_LUNAR_ORBIT_ASCENT_DAY_8);
		break;
	case MST_G_LUNAR_ORBIT_ASCENT_DAY_8: //CSM state vector update to Preliminary TEI-30 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 29 && MoonRevTime > 40.0*60.0, 1, MST_G_LUNAR_ORBIT_ASCENT_DAY_9);
		break;
	case MST_G_LUNAR_ORBIT_ASCENT_DAY_9: //Preliminary TEI-30 update to Final TEI-30 Update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, MoonRev >= 30 && MoonRevTime > 55.0*60.0, 45, MST_G_LUNAR_ORBIT_ASCENT_DAY_10);
		break;
	case MST_G_LUNAR_ORBIT_ASCENT_DAY_10: //Final TEI-30 Update to TEI-31 PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 46, MST_G_LUNAR_ORBIT_ASCENT_DAY_11);
		break;
	case MST_G_LUNAR_ORBIT_ASCENT_DAY_11: //TEI-31 PAD to TEI Evaluation
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TEI + 300.0), 47, MST_G_LUNAR_ORBIT_ASCENT_DAY_12);
		break;
	case MST_G_LUNAR_ORBIT_ASCENT_DAY_12: //TEI Evaluation to TEI
		UpdateMacro(UTP_NONE, PT_NONE, true, 105, MST_G_TRANSEARTH_1, scrubbed, MoonRevTime > 40.0*60.0, MST_G_LUNAR_ORBIT_ASCENT_DAY_10);
		break;
	case MST_G_TRANSEARTH_1: //TEI to PTC REFSMMAT
		switch (SubState)
		{
		case 0:
			MissionPhase = MMST_TE_COAST;
			setSubState(1);
			break;
		case 1:
			if (rtcc->GETEval(rtcc->calcParams.TEI + 20.0*60.0))
			{
				SlowIfDesired();
				setState(MST_G_TRANSEARTH_2);
			}
			break;
		}
		break;
	case MST_G_TRANSEARTH_2: //PTC REFSMMAT to MCC-5 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval(rtcc->calcParams.TEI + 13.0*3600.0 + 25.0*60.0), 19, MST_G_TRANSEARTH_3);
		break;
	case MST_G_TRANSEARTH_3: //MCC-5 update to preliminary MCC-6 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TEI + 24.0*3600.0 + 25.0*60.0), 110, MST_G_TRANSEARTH_4);
		break;
	case MST_G_TRANSEARTH_4: //Preliminary MCC-6 update to Entry PAD update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 111, MST_G_TRANSEARTH_5);
		break;
	case MST_G_TRANSEARTH_5: //Entry PAD update to MCC-6 update
		UpdateMacro(UTP_PADONLY, PT_AP11ENT, rtcc->GETEval(rtcc->calcParams.EI - 24.0*3600.0 - 10.0*60.0), 116, MST_G_TRANSEARTH_6);
		break;
	case MST_G_TRANSEARTH_6: //MCC-6 update to Entry PAD update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 112, MST_G_TRANSEARTH_7);
		break;
	case MST_G_TRANSEARTH_7: //Entry PAD update to MCC-7 decision update
		UpdateMacro(UTP_PADONLY, PT_AP11ENT, rtcc->GETEval(rtcc->calcParams.EI - 6.0*3600.0), 116, MST_G_TRANSEARTH_8);
		break;
	case MST_G_TRANSEARTH_8: //MCC-7 decision update to MCC-7 update
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval(rtcc->calcParams.EI - 4.0*3600.0 - 35.0*60.0), 113, MST_G_TRANSEARTH_9);
		break;
	case MST_G_TRANSEARTH_9: //MCC-7 update to Entry PAD update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 114, MST_G_TRANSEARTH_10);
		break;
	case MST_G_TRANSEARTH_10: //Entry PAD update to final entry update
		UpdateMacro(UTP_PADONLY, PT_AP11ENT, rtcc->GETEval(rtcc->calcParams.EI - 45.0*60.0), 117, MST_G_TRANSEARTH_11);
		break;
	case MST_G_TRANSEARTH_11: //Final entry update to CM/SM separation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11ENT, cm->stage == CM_STAGE, 118, MST_ENTRY);
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
		//Alternative sequences
	case MST_G_LUNAR_ORBIT_PRE_PDI2_1: //PDI2 PAD to
		UpdateMacro(UTP_PADONLY, PT_AP11PDIPAD, SubStateTime > 3.0*60.0, 170, MST_G_LUNAR_ORBIT_PDI_DAY_14);
		break;
	}
}