/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2022

MCC sequencing for Mission H1

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
#include "MCC_Mission_H1.h"
#include "iu.h"

void MCC::MissionSequence_H1()
{
	switch (MissionState)
	{
	case MST_H1_INSERTION: //Ground liftoff time update to TLI Simulation
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(1.0*3600.0 + 30.0*60.0), 10, MST_H1_EPO1);
		break;
	case MST_H1_EPO1: //TLI Simulation to TLI+90 PAD
		UpdateMacro(UTP_NONE, PT_NONE, true, 11, MST_H1_EPO2);
		break;
	case MST_H1_EPO2: //TLI+90 Maneuver PAD to TLI+5h P37 PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 12, MST_H1_EPO3);
		break;
	case MST_H1_EPO3: //TLI+5h P37 PAD to TLI PAD
		UpdateMacro(UTP_PADONLY, PT_P37PAD, SubStateTime > 3.0*60.0, 13, MST_H1_EPO4);
		break;
	case MST_H1_EPO4: //TLI PAD to TLI Evaluation
		UpdateMacro(UTP_PADONLY, PT_TLIPAD, rtcc->GETEval2(rtcc->calcParams.TLI + 18.0), 14, MST_H1_TRANSLUNAR1);
		break;
	case MST_H1_TRANSLUNAR1: //TLI Evaluation to SIVB Evasive Maneuver
		UpdateMacro(UTP_NONE, PT_NONE, true, 15, MST_H1_TRANSLUNAR2, scrubbed, rtcc->GETEval2(3.0*3600.0), MST_H1_EPO1);
		break;
	case MST_H1_TRANSLUNAR2:
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
			if (rtcc->GETEval2(rtcc->calcParams.TLI + 3600.0 + 30.0*60.0))
			{
				SlowIfDesired();
				setState(MST_H1_TRANSLUNAR3);
			}
		}
		break;
		}
		break;
	case MST_H1_TRANSLUNAR3: //SIVB Evasive Maneuver to TB8 Enable
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

			sivb->GetIU()->GetDCS()->Uplink(DCSUPLINK_EVASIVE_MANEUVER_ENABLE, NULL);
			setSubState(3);
		}
		break;
		case 3:
			if (rtcc->GETEval2(rtcc->calcParams.TLI + 3600.0 + 51.0*60.0))
			{
				SlowIfDesired();
				setState(MST_H1_TRANSLUNAR4);
			}
			break;
		}
		break;
	case MST_H1_TRANSLUNAR4:  //TB8 enable to PTC REFSMMAT update
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

			sivb->GetIU()->GetDCS()->Uplink(DCSUPLINK_TIMEBASE_8_ENABLE, NULL);
			setSubState(3);
		}
		break;
		case 3:
			if (rtcc->GETEval2(rtcc->calcParams.TLI + 2.0*3600.0 + 29.0*60.0))
			{
				SlowIfDesired();
				setState(MST_H1_TRANSLUNAR5);
			}
			break;
		}
		break;
	case MST_H1_TRANSLUNAR5: //PTC REFSMMAT update to MCC-1 Evaluation
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 5.0*60.0, 18, MST_H1_TRANSLUNAR6);
		break;
	case MST_H1_TRANSLUNAR6: //MCC-1 Evaluation to Block Data 1
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(rtcc->calcParams.TLI + 2.0*3600.0 + 59.0*60.0), 19, MST_H1_TRANSLUNAR7);
		break;
	case MST_H1_TRANSLUNAR7: //Block Data 1 to MCC-1 update
		UpdateMacro(UTP_PADONLY, PT_P37PAD, rtcc->GETEval2(rtcc->calcParams.TLI + 7.0*3600.0 + 15.0*60.0), 16, MST_H1_TRANSLUNAR8);
		break;
	case MST_H1_TRANSLUNAR8: //MCC-1 update to MCC-2 Evaluation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TLI + 11.0*3600.0), 21, MST_H1_TRANSLUNAR9);
		break;
	case MST_H1_TRANSLUNAR9: //MCC-2 Evaluation to Block Data 2
		UpdateMacro(UTP_NONE, PT_NONE, SubStateTime > 5.0*60.0, 20, MST_H1_TRANSLUNAR10);
		break;
	case MST_H1_TRANSLUNAR10: //Block Data 2 to MCC-2 update
		UpdateMacro(UTP_PADONLY, PT_P37PAD, rtcc->GETEval2(rtcc->calcParams.TLI + 26.0*3600.0 + 27.0*60.0), 17, MST_H1_TRANSLUNAR11);
		break;
	case MST_H1_TRANSLUNAR11: //MCC-2 update to Lunar Flyby PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TLI + 31.0*3600.0 + 57.0*60.0), 22, MST_H1_TRANSLUNAR12);
		break;
	case MST_H1_TRANSLUNAR12: //Lunar Flyby PAD to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TLI + 39.0*3600.0 + 1.0*60.0), 23, MST_H1_TRANSLUNAR13);
		break;
	case MST_H1_TRANSLUNAR13: //SV Update to MCC-3
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.LOI - 23.5*3600.0), 5, MST_H1_TRANSLUNAR14);
		break;
	case MST_H1_TRANSLUNAR14: //MCC-3 update to MCC-4 Evaluation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.LOI - 6.5*3600.0), 24, MST_H1_TRANSLUNAR15);
		break;
	case MST_H1_TRANSLUNAR15: //MCC-4 Evaluation to MCC-4 update or SV update
		UpdateMacro(UTP_NONE, PT_NONE, SubStateTime > 5.0*60.0, 25, MST_H1_TRANSLUNAR16, scrubbed, rtcc->GETEval2(rtcc->calcParams.LOI - 4.5*3600.0), MST_H1_TRANSLUNAR_NO_MCC4_1);
		break;
	case MST_H1_TRANSLUNAR_NO_MCC4_1: //SV update to PC+2 update *No MCC-4 Timeline*
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 20.0*60.0, 5, MST_H1_TRANSLUNAR_NO_MCC4_2);
		break;
	case MST_H1_TRANSLUNAR_NO_MCC4_2: //PC+2 update to LOI-1 update (preliminary) *No MCC-4 Timeline*
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.LOI - 2.0*3600.0 - 20.0*60.0), 28, MST_H1_TRANSLUNAR_NO_MCC4_3);
		break;
	case MST_H1_TRANSLUNAR_NO_MCC4_3: //LOI-1 update (preliminary) to TEI-1 update *No MCC-4 Timeline*
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 29, MST_H1_TRANSLUNAR19);
		break;
	case MST_H1_TRANSLUNAR16: //MCC-4 update to PC+2 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 26, MST_H1_TRANSLUNAR17);
		break;
	case MST_H1_TRANSLUNAR17: //PC+2 update to LOI-1 update (preliminary)
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.LOI - 2.0*3600.0 - 20.0*60.0), 27, MST_H1_TRANSLUNAR18);
		break;
	case MST_H1_TRANSLUNAR18: //LOI-1 update (preliminary) to TEI-1 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 30, MST_H1_TRANSLUNAR19);
		break;
	case MST_H1_TRANSLUNAR19: //TEI-1 update to TEI-4 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 5.0*60.0, 40, MST_H1_TRANSLUNAR20);
		break;
	case MST_H1_TRANSLUNAR20: //TEI-4 update to Rev 1 Map Update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.LOI - 1.0*3600.0), 41, MST_H1_TRANSLUNAR21);
		break;
	case MST_H1_TRANSLUNAR21: //Rev 1 Map Update to LOI-1 update (final)
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, SubStateTime > 5.0*60.0, 60, MST_H1_TRANSLUNAR22);
		break;
	case MST_H1_TRANSLUNAR22: //LOI-1 update (final) to Lunar orbit phase begin
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.LOI), 30, MST_H1_LUNAR_ORBIT_LOI_DAY_1);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_1: //Lunar orbit phase begin
		switch (SubState) {
		case 0:
			MissionPhase = MMST_LUNAR_ORBIT;
			setSubState(1);
			break;
		case 1:
			if (MoonRev >= 2 && MoonRevTime > 35.0*60.0)
			{
				SlowIfDesired();
				setState(MST_H1_LUNAR_ORBIT_LOI_DAY_2);
			}
			break;
		}
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_2: //LOI-2 update to TEI-5 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 31, MST_H1_LUNAR_ORBIT_LOI_DAY_3);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_3: //TEI-5 update to H-1 landmark tracking update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, MoonRev >= 3 && MoonRevTime > 50.0*60.0, 42, MST_H1_LUNAR_ORBIT_LOI_DAY_4);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_4: //H-1 landmark tracking update to TEI-11 update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, MoonRev >= 4 && MoonRevTime > 1.0*3600.0 + 20.0*60.0, 61, MST_H1_LUNAR_ORBIT_LOI_DAY_5);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_5: //TEI-11 update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, true, 43, MST_H1_LUNAR_ORBIT_LOI_DAY_6);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_6: //State vector update to TEI-34 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 10 && MoonRevTime > 1.0*3600.0, 5, MST_H1_LUNAR_ORBIT_LOI_DAY_7);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_7: //TEI-34 update to CSM DAP PAD
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 5.0*60.0, 44, MST_H1_LUNAR_ORBIT_LOI_DAY_8);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_8: //CSM DAP PAD to LS Update
		UpdateMacro(UTP_PADONLY, PT_AP10DAPDATA, SubStateTime > 5.0*60.0, 7, MST_H1_LUNAR_ORBIT_PDI_DAY_1);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_1: //LS Update to Lmk 193 Landmark Tracking PAD
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 11 && MoonRevTime > 50.0*60.0, 32, MST_H1_LUNAR_ORBIT_PDI_DAY_2);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_2: //Lmk 193 Landmark Tracking PAD to CSM SV Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11LMARKTRKPAD, MoonRev >= 12 && MoonRevTime > 25.0*60.0, 62, MST_H1_LUNAR_ORBIT_PDI_DAY_4);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_4: //CSM SV Update to LM Activation Data
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 5.0*60.0, 5, MST_H1_LUNAR_ORBIT_PDI_DAY_7);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_7: //LM Activation Data to LGC activation update
		UpdateMacro(UTP_PADONLY, PT_LMACTDATA, SubStateTime > 5.0*60.0, 9, MST_H1_LUNAR_ORBIT_PDI_DAY_9);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_9: //LGC activation update to AGS activation update
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, SubStateTime > 3.0*60.0, 35, MST_H1_LUNAR_ORBIT_PDI_DAY_10);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_10: //AGS activation update to Separation maneuver update
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11AGSACT, SubStateTime > 3.0*60.0, 36, MST_H1_LUNAR_ORBIT_PDI_DAY_11);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_11: //Separation maneuver update to DOI update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, MoonRev >= 13 && MoonRevTime > 38.0*60.0, 37, MST_H1_LUNAR_ORBIT_PDI_DAY_12);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_12: //DOI update to PDI PAD
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11LMMNV, SubStateTime > 3.0*60.0, 38, MST_H1_LUNAR_ORBIT_PDI_DAY_13);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_13: //PDI PAD to PDI Abort PAD
		UpdateMacro(UTP_PADONLY, PT_AP11PDIPAD, SubStateTime > 3.0*60.0, 70, MST_H1_LUNAR_ORBIT_PDI_DAY_14);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_14: //PDI Abort PAD to No PDI+12 PAD
		UpdateMacro(UTP_PADONLY, PT_AP12PDIABORTPAD, SubStateTime > 3.0*60.0, 71, MST_H1_LUNAR_ORBIT_PDI_DAY_15);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_15: //No PDI+12 PAD to Lunar Surface PAD
		UpdateMacro(UTP_PADONLY, PT_AP11LMMNV, SubStateTime > 3.0*60.0, 72, MST_H1_LUNAR_ORBIT_PRE_DOI_1);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_DOI_1: //Lunar Surface PAD to P22 Acquistion time
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP12LUNSURFPAD, SubStateTime > 3.0*60.0, 73, MST_H1_LUNAR_ORBIT_PRE_DOI_2);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_DOI_2: //P22 Acquistion time to LM SV update
		UpdateMacro(UTP_PADONLY, PT_LMP22ACQPAD, SubStateTime > 10.0*60.0, 58, MST_H1_LUNAR_ORBIT_PRE_DOI_3);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_DOI_3: //LM SV update to CSM P76 PADs
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, true, 3, MST_H1_LUNAR_ORBIT_PRE_DOI_4);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_DOI_4: //CSM P76 PADs to DOI Evaluation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11P76PAD, rtcc->GETEval2(rtcc->calcParams.PDI - 21.0*60.0), 76, MST_H1_LUNAR_ORBIT_PRE_PDI_1);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_PDI_1: //DOI Evaluation to SV and RLS uplink
		UpdateMacro(UTP_NONE, PT_NONE, SubStateTime > 3.0*60.0, 77, MST_H1_LUNAR_ORBIT_PRE_PDI_2);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_PDI_2: //SV and RLS uplink to PDI Evaluation
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.PDI + 2.0*60.0), 75, MST_H1_LUNAR_ORBIT_PRE_LANDING_1);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_LANDING_1: //PDI Evaluation to landing confirmation or PDI Recycle
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->calcParams.tgt->GroundContact(), 78, MST_H1_LUNAR_ORBIT_POST_LANDING_1, scrubbed, SubStateTime > 3.0*60.0, MST_H1_LUNAR_ORBIT_NO_PDI);
		break;
	case MST_H1_LUNAR_ORBIT_NO_PDI: //PDI Recycle to PDI PAD
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11AGSACT, SubStateTime > 3.0*60.0, 170, MST_H1_LUNAR_ORBIT_PDI_DAY_13);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_1: //Landing confirmation to T1
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(rtcc->calcParams.PDI + 15.0*60.0), 79, MST_H1_LUNAR_ORBIT_POST_LANDING_2);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_2: //T1 to T2
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(rtcc->calcParams.PDI + 19.0*60.0 + 30.0), 80, MST_H1_LUNAR_ORBIT_POST_LANDING_3);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_3: //T2 to Landing Site Update
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->calcParams.PDI + 75.0*60.0, 81, MST_H1_LUNAR_ORBIT_POST_LANDING_4);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_4: //Landing Site Update to LM Tracking PAD
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, SubStateTime > 3.0*60.0, 74, MST_H1_LUNAR_ORBIT_POST_LANDING_5);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_5: //LM Tracking PAD to DAP Load
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11LMARKTRKPAD, rtcc->calcParams.PDI + 2.0*3600.0 + 10.0*60.0, 63, MST_H1_LUNAR_ORBIT_POST_LANDING_6);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_6: //DAP Load to Liftoff Times Update 1
		UpdateMacro(UTP_PADONLY, PT_AP10DAPDATA, SubStateTime > 3.0*60.0, 8, MST_H1_LUNAR_ORBIT_POST_LANDING_7);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_7: //Liftoff Times Update 1 to LM Tracking PAD
		UpdateMacro(UTP_PADONLY, PT_LIFTOFFTIMES, rtcc->calcParams.PDI + 3.0*3600.0 + 23.0*60.0, 92, MST_H1_LUNAR_ORBIT_POST_LANDING_8);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_8: //LM Tracking PAD to Plane Change Evaluation
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, MoonRev >= 18 && MoonRevTime > 52.0*60.0, 92, MST_H1_LUNAR_ORBIT_POST_LANDING_9);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_9: //Plane Change Evaluation to PC REFSMMAT or SV update (if PC scrubbed)
		UpdateMacro(UTP_NONE, PT_NONE, true, 93, MST_H1_LUNAR_ORBIT_PLANE_CHANGE_1, scrubbed, MoonRev >= 16 && MoonRevTime > 40.0*60.0, MST_H1_LUNAR_ORBIT_NO_PLANE_CHANGE_1);
		break;
	case MST_H1_LUNAR_ORBIT_NO_PLANE_CHANGE_1: //SV update to LM Liftoff Times Update 2
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 16 && MoonRevTime > 95.0*60.0, 1, MST_H1_LUNAR_ORBIT_EVA_DAY_1);
		break;
	case MST_H1_LUNAR_ORBIT_PLANE_CHANGE_1: //PC REFSMMAT to PC Update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 16 && MoonRevTime > 40.0*60.0, 94, MST_H1_LUNAR_ORBIT_PLANE_CHANGE_2);
		break;
	case MST_H1_LUNAR_ORBIT_PLANE_CHANGE_2: //PC Update to Lunar Liftoff REFSMMAT Uplink
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, MoonRev >= 16 && MoonRevTime > 95.0*60.0, 95, MST_H1_LUNAR_ORBIT_EVA_DAY_1);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_1: //Lunar Liftoff REFSMMAT Uplink to LM Liftoff Times Update 2
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 18 && MoonRevTime > 35.0*60.0, 96, MST_H1_LUNAR_ORBIT_EVA_DAY_2);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_2: //LM Liftoff Times Update 2 to LM Acquisition Time update
		UpdateMacro(UTP_PADONLY, PT_LIFTOFFTIMES, MoonRev >= 19 && MoonRevTime > 60.0*60.0, 97, MST_H1_LUNAR_ORBIT_EVA_DAY_3);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_3: //LM Acquisition Time update to LM Liftoff Time update 3
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, MoonRev >= 21 && MoonRevTime > 65.0*60.0, 64, MST_H1_LUNAR_ORBIT_EVA_DAY_4);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_4: //LM Liftoff Times Update 3 to LGC CSM state vector update
		UpdateMacro(UTP_PADONLY, PT_LIFTOFFTIMES, MoonRev >= 24 && MoonRevTime > 35.0*60.0, 98, MST_H1_LUNAR_ORBIT_ASCENT_DAY_1);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_1: //LGC CSM state vector update to Nominal Insertion targeting
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, MoonRev >= 24 && MoonRevTime > 70.0*60.0, 3, MST_H1_LUNAR_ORBIT_ASCENT_DAY_2);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_2: //Nominal Insertion targeting to CMC State Vector uplinks
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, SubStateTime > 1.0*60.0, 100, MST_H1_LUNAR_ORBIT_ASCENT_DAY_3);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_3: //CMC State Vector uplinks to LM Ascent PAD
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 1.0*60.0, 101, MST_H1_LUNAR_ORBIT_ASCENT_DAY_4);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_4: //LM Ascent PAD to CSI Data Card
		UpdateMacro(UTP_PADONLY, PT_AP11LMASCPAD, SubStateTime > 5.0*60.0, 102, MST_H1_LUNAR_ORBIT_ASCENT_DAY_5);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_5: //CSI Data Card to LM Liftoff Evaluation
		UpdateMacro(UTP_PADONLY, PT_AP10CSI, rtcc->GETEval2(rtcc->calcParams.LunarLiftoff + 20.0), 103, MST_H1_LUNAR_ORBIT_ASCENT_DAY_6);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_6: //LM Liftoff Evaluation to CMC LM State Vector update
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(rtcc->calcParams.Insertion + 120.0), 104, MST_H1_LUNAR_ORBIT_ASCENT_DAY_7, scrubbed, SubStateTime > 15.0*60.0, MST_H1_LUNAR_ORBIT_ASCENT_DAY_2);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_7: //CMC LM State Vector update to CSM state vector update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 27 && MoonRevTime > 95.0*60.0, 2, MST_H1_LUNAR_ORBIT_ASCENT_DAY_8);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_8: //CSM state vector update to Preliminary TEI-30 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 29 && MoonRevTime > 40.0*60.0, 1, MST_H1_LUNAR_ORBIT_ASCENT_DAY_9);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_9: //Preliminary TEI-30 update to Final TEI-30 Update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, MoonRev >= 30 && MoonRevTime > 55.0*60.0, 45, MST_H1_LUNAR_ORBIT_ASCENT_DAY_10);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_10: //Final TEI-30 Update to TEI-31 PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 46, MST_H1_LUNAR_ORBIT_ASCENT_DAY_11);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_11: //TEI-31 PAD to TEI Evaluation
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TEI + 300.0), 47, MST_H1_LUNAR_ORBIT_ASCENT_DAY_12);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_12: //TEI Evaluation to TEI
		UpdateMacro(UTP_NONE, PT_NONE, true, 105, MST_H1_TRANSEARTH_1, scrubbed, MoonRevTime > 40.0*60.0, MST_H1_LUNAR_ORBIT_ASCENT_DAY_10);
		break;
	case MST_H1_TRANSEARTH_1: //TEI to PTC REFSMMAT
		switch (SubState)
		{
		case 0:
			MissionPhase = MMST_TE_COAST;
			setSubState(1);
			break;
		case 1:
			if (rtcc->GETEval2(rtcc->calcParams.TEI + 20.0*60.0))
			{
				SlowIfDesired();
				setState(MST_H1_TRANSEARTH_2);
			}
			break;
		}
		break;
	case MST_H1_TRANSEARTH_2: //PTC REFSMMAT to MCC-5 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.TEI + 13.0*3600.0 + 25.0*60.0), 19, MST_H1_TRANSEARTH_3);
		break;
	case MST_H1_TRANSEARTH_3: //MCC-5 update to preliminary MCC-6 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TEI + 24.0*3600.0 + 25.0*60.0), 110, MST_H1_TRANSEARTH_4);
		break;
	case MST_H1_TRANSEARTH_4: //Preliminary MCC-6 update to Entry PAD update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 5.0*60.0, 111, MST_H1_TRANSEARTH_5);
		break;
	case MST_H1_TRANSEARTH_5: //Entry PAD update to MCC-6 update
		UpdateMacro(UTP_PADONLY, PT_AP11ENT, rtcc->GETEval2(rtcc->calcParams.EI - 24.0*3600.0 - 10.0*60.0), 116, MST_H1_TRANSEARTH_6);
		break;
	case MST_H1_TRANSEARTH_6: //MCC-6 update to Entry PAD update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 112, MST_H1_TRANSEARTH_7);
		break;
	case MST_H1_TRANSEARTH_7: //Entry PAD update to MCC-7 decision update
		UpdateMacro(UTP_PADONLY, PT_AP11ENT, rtcc->GETEval2(rtcc->calcParams.EI - 6.0*3600.0), 116, MST_H1_TRANSEARTH_8);
		break;
	case MST_H1_TRANSEARTH_8: //MCC-7 decision update to MCC-7 update
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(rtcc->calcParams.EI - 4.0*3600.0 - 35.0*60.0), 113, MST_H1_TRANSEARTH_9);
		break;
	case MST_H1_TRANSEARTH_9: //MCC-7 update to Entry PAD update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 114, MST_H1_TRANSEARTH_10);
		break;
	case MST_H1_TRANSEARTH_10: //Entry PAD update to final entry update
		UpdateMacro(UTP_PADONLY, PT_AP11ENT, rtcc->GETEval2(rtcc->calcParams.EI - 45.0*60.0), 117, MST_H1_TRANSEARTH_11);
		break;
	case MST_H1_TRANSEARTH_11: //Final entry update to CM/SM separation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11ENT, cm->GetStage() == CM_STAGE, 118, MST_ENTRY);
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
			if (cm->GetStage() == CM_ENTRY_STAGE_SEVEN)
			{
				setState(MST_LANDING);
			}
		}
		break;
		}
		break;
		//Alternative sequences
	case MST_H1_LUNAR_ORBIT_PRE_PDI2_1: //PDI2 PAD to
		UpdateMacro(UTP_PADONLY, PT_AP11PDIPAD, SubStateTime > 3.0*60.0, 170, MST_H1_LUNAR_ORBIT_PDI_DAY_14);
		break;
	case MST_H1_ABORT_ORBIT:
	{
		if (AbortMode == 5) //Earth Orbit Abort
		{
			if (cm->GetStage() == CM_ENTRY_STAGE_SEVEN)
			{
				setState(MST_LANDING);
			}
		}
	}
	break;
	}
}