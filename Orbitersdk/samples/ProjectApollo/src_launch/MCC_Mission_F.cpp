/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

MCC sequencing for Mission F

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
#include "MCC_Mission_F.h"
#include "iu.h"

void MCC::MissionSequence_F()
{
	switch (MissionState) {
	case MST_F_INSERTION: //Ground liftoff time update to TLI Simulation
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(1.0*3600.0 + 40.0*60.0), 1, MST_F_EPO1);
		break;
	case MST_F_EPO1: //TLI Simulation to TLI+90 PAD
		UpdateMacro(UTP_NONE, PT_NONE, true, 2, MST_F_EPO2);
		break;
	case MST_F_EPO2: //TLI+90 Maneuver PAD to TLI+5h Maneuver PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 3.0*60.0, 3, MST_F_EPO3);
		break;
	case MST_F_EPO3: //TLI+5h P37 PAD to TLI PAD
		UpdateMacro(UTP_PADONLY, PT_P37PAD, SubStateTime > 3.0*60.0, 4, MST_F_EPO4);
		break;
	case MST_F_EPO4: //TLI PAD to TLI Evaluation
		UpdateMacro(UTP_PADONLY, PT_TLIPAD, rtcc->GETEval2(rtcc->calcParams.TLI + 18.0), 5, MST_F_TRANSLUNAR1);
		break;
	case MST_F_TRANSLUNAR1: //TLI Evaluation to Block Data 1
		UpdateMacro(UTP_NONE, PT_NONE, true, 6, MST_F_TRANSLUNAR2, scrubbed, rtcc->GETEval2(3.0*3600.0 + 10.0*60.0), MST_F_EPO1);
		break;
	case MST_F_TRANSLUNAR2:
		if (rtcc->GETEval2(rtcc->calcParams.TLI))
		{
			addMessage("TLI");
			MissionPhase = MMST_TL_COAST;
			setState(MST_F_TRANSLUNAR3);
		}
		else {
			break;
		}
		break;
	case MST_F_TRANSLUNAR3: //
		if (cm->stage == CSM_LEM_STAGE) {
			addMessage("SEPARATION");
			setState(MST_F_TRANSLUNAR4);
		}
		else {
			break;
		}
		break;
	case MST_F_TRANSLUNAR4: //
		if (rtcc->GETEval2(rtcc->calcParams.TLI + 55.0*60.0))
		{
			SlowIfDesired();
			setState(MST_F_TRANSLUNAR5);
		}
		break;
	case MST_F_TRANSLUNAR5: //Evasive Maneuver to TB8 enable
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TLI + 3600.0 + 30.0*60.0), 7, MST_F_TRANSLUNAR6);
		break;
	case MST_F_TRANSLUNAR6:  //TB8 enable to Block Data 1
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
			if (rtcc->GETEval2(rtcc->calcParams.TLI + 2.0*3600.0 + 30.0*60.0))
			{
				SlowIfDesired();
				setState(MST_F_TRANSLUNAR7);
			}
			break;
		}
		break;
	case MST_F_TRANSLUNAR7: //Block Data 1 to MCC-1 Calculation
		UpdateMacro(UTP_PADONLY, PT_P37PAD, rtcc->GETEval2(rtcc->calcParams.TLI + 3.0*3600.0 + 20.0*60.0), 8, MST_F_TRANSLUNAR8);
		break;
	case MST_F_TRANSLUNAR8: //MCC-1 Calculation to MCC-1 Update (or PTC REFSMMAT)
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(rtcc->calcParams.TLI + 7.0*3600.0 + 30.0*60.0), 11, MST_F_TRANSLUNAR9, scrubbed, rtcc->GETEval2(rtcc->calcParams.TLI + 4.0*3600.0 + 30.0*60.0), MST_F_TRANSLUNAR_NO_MCC1_1);
		break;
	case MST_F_TRANSLUNAR9: //MCC-1 Update to PTC REFSMMAT update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TLI + 9.0*3600.0 + 20.0*60.0), 12, MST_F_TRANSLUNAR10);
		break;
	case MST_F_TRANSLUNAR_NO_MCC1_1: //PTC REFSMMAT update to SV update (MCC-1 was scrubbed)
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.TLI + 7.0*3600.0 + 10.0*60.0), 10, MST_F_TRANSLUNAR_NO_MCC1_2);
		break;
	case MST_F_TRANSLUNAR_NO_MCC1_2: //SV update to Block Data 2 (MCC-1 was scrubbed)
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.TLI + 9.0*3600.0 + 30.0*60.0), 100, MST_F_TRANSLUNAR11);
		break;
	case MST_F_TRANSLUNAR10: //PTC REFSMMAT to Block Data 2
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.TLI + 9.0*3600.0 + 35.0*60.0), 10, MST_F_TRANSLUNAR11);
		break;
	case MST_F_TRANSLUNAR11: //Block Data 2 to MCC-2
		UpdateMacro(UTP_PADONLY, PT_P37PAD, rtcc->GETEval2(rtcc->calcParams.TLI + 23.0*3600.0), 9, MST_F_TRANSLUNAR12);
		break;
	case MST_F_TRANSLUNAR12: //MCC-2 to Lunar Flyby PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TLI + 30.0*3600.0 + 30.0*60.0), 13, MST_F_TRANSLUNAR13);
		break;
	case MST_F_TRANSLUNAR13: //Lunar Flyby PAD to State Vector update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TLI + 42.0*3600.0), 14, MST_F_TRANSLUNAR14);
		break;
	case MST_F_TRANSLUNAR14: //State Vector update to MCC-3 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.LOI - 23.0*3600.0 - 30.0*60.0), 100, MST_F_TRANSLUNAR15);
		break;
	case MST_F_TRANSLUNAR15: //MCC-3 update to MCC-4 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.LOI - 6.0*3600.0 - 30.0*60.0), 15, MST_F_TRANSLUNAR16);
		break;
	case MST_F_TRANSLUNAR16: //MCC-4 update to PC+2 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 16, MST_F_TRANSLUNAR17);
		break;
	case MST_F_TRANSLUNAR17: //PC+2 update to Preliminary LOI-1 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.LOI - 4.0*3600.0 - 30.0*60.0), 17, MST_F_TRANSLUNAR18);
		break;
	case MST_F_TRANSLUNAR18: //Preliminary LOI-1 update to TEI-1 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 5.0*60.0, 20, MST_F_TRANSLUNAR19);
		break;
	case MST_F_TRANSLUNAR19: //TEI-1 update to TEI-4 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 5.0*60.0, 30, MST_F_TRANSLUNAR20);
		break;
	case MST_F_TRANSLUNAR20: //TEI-4 update to Rev 1 Map Update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.LOI - 2.0*3600.0 - 15.0*60.0), 31, MST_F_TRANSLUNAR21);
		break;
	case MST_F_TRANSLUNAR21: //Rev 1 Map Update to LOI-1 update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, rtcc->GETEval2(rtcc->calcParams.LOI - 1.0*3600.0 - 30.0*60.0), 40, MST_F_TRANSLUNAR22);
		break;
	case MST_F_TRANSLUNAR22: //LOI-1 update to Rev 2 Map Update
		if (MissionPhase == MMST_TL_COAST && rtcc->GETEval2(rtcc->calcParams.LOI))
		{
			MissionPhase = MMST_LUNAR_ORBIT;
		}
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, MoonRev >= 1 && MoonRevTime > 30.0*60.0, 21, MST_F_LUNAR_ORBIT_LOI_DAY_1);
		break;
	case MST_F_LUNAR_ORBIT_LOI_DAY_1: //Rev 2 Map Update to LOI-2 update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 2 && MoonRevTime > 30.0*60.0, 41, MST_F_LUNAR_ORBIT_LOI_DAY_2);
		break;
	case MST_F_LUNAR_ORBIT_LOI_DAY_2: //LOI-2 update to TEI-5 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 22, MST_F_LUNAR_ORBIT_LOI_DAY_3);
		break;
	case MST_F_LUNAR_ORBIT_LOI_DAY_3: //TEI-5 update to Rev 3 Map Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 32, MST_F_LUNAR_ORBIT_LOI_DAY_4);
		break;
	case MST_F_LUNAR_ORBIT_LOI_DAY_4: //Rev 3 Map Update to landmark tracking rev 4 update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 3 && MoonRevTime > 45.0*60.0, 42, MST_F_LUNAR_ORBIT_LOI_DAY_5);
		break;
	case MST_F_LUNAR_ORBIT_LOI_DAY_5: //F-1 landmark tracking update to B-1 landmark tracking update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, SubStateTime > 3.0*60.0, 50, MST_F_LUNAR_ORBIT_LOI_DAY_6);
		break;
	case MST_F_LUNAR_ORBIT_LOI_DAY_6: //B-1 landmark tracking update to rev 4 map update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, SubStateTime > 3.0*60.0, 51, MST_F_LUNAR_ORBIT_LOI_DAY_7);
		break;
	case MST_F_LUNAR_ORBIT_LOI_DAY_7: //Rev 4 map update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 4 && MoonRevTime > 3600.0, 43, MST_F_LUNAR_ORBIT_LOI_DAY_8);
		break;
	case MST_F_LUNAR_ORBIT_LOI_DAY_8: //State vector update to state vector update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 5 && MoonRevTime > 30.0*60.0, 100, MST_F_LUNAR_ORBIT_LOI_DAY_9);
		break;
	case MST_F_LUNAR_ORBIT_LOI_DAY_9: //State vector update to TEI-10 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, true, 100, MST_F_LUNAR_ORBIT_LOI_DAY_10);
		break;
	case MST_F_LUNAR_ORBIT_LOI_DAY_10: //TEI-10 update to LLS-2 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, MoonRev >= 10 && MoonRevTime > 30.0*60.0, 33, MST_F_LUNAR_ORBIT_DOI_DAY_1);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_1: //LLS-2 update to LLS-2 track PAD
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, true, 60, MST_F_LUNAR_ORBIT_DOI_DAY_2);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_2: //LLS-2 track PAD to rev 11 map update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, SubStateTime > 3.0*60.0, 52, MST_F_LUNAR_ORBIT_DOI_DAY_3);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_3: //Rev 11 map update to CSM DAP update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 10 && MoonRevTime > 3600.0, 44, MST_F_LUNAR_ORBIT_DOI_DAY_4);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_4: //CSM DAP update to LM DAP Load PAD
		UpdateMacro(UTP_PADONLY, PT_AP10DAPDATA, MoonRev >= 11 && MoonRevTime > 30.0*60.0, 61, MST_F_LUNAR_ORBIT_DOI_DAY_5);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_5: //LM DAP Load PAD to LM IMU gyro torquing angle update
		UpdateMacro(UTP_PADONLY, PT_AP10DAPDATA, MoonRev >= 11 && MoonRevTime > 50.0*60.0, 62, MST_F_LUNAR_ORBIT_DOI_DAY_8);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_8: //LM IMU gyro torquing angle update to LGC activation update
		UpdateMacro(UTP_PADONLY, PT_TORQANG, MoonRev >= 11 && MoonRevTime > 60.0*60.0, 63, MST_F_LUNAR_ORBIT_DOI_DAY_9);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_9: //LGC activation update to separation update
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, MoonRev >= 11 && MoonRevTime > 65.0*60.0, 64, MST_F_LUNAR_ORBIT_DOI_DAY_10);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_10: //Separation update to AGS K Factor update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, MoonRev >= 11 && MoonRevTime > 70.0*60.0, 70, MST_F_LUNAR_ORBIT_DOI_DAY_11);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_11: //AGS K Factor update to DOI update
		UpdateMacro(UTP_PADONLY, PT_AP11AGSACT, MoonRev >= 12 && MoonRevTime > 30.0*60.0, 65, MST_F_LUNAR_ORBIT_DOI_DAY_12);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_12: //DOI update to Phasing update
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11LMMNV, SubStateTime > 3.0*60.0, 71, MST_F_LUNAR_ORBIT_DOI_DAY_13);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_13: //Phasing update to PDI Abort update
		UpdateMacro(UTP_PADONLY, PT_AP11LMMNV, SubStateTime > 3.0*60.0, 72, MST_F_LUNAR_ORBIT_DOI_DAY_14);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_14: //PDI Abort update to LGC CSM state vector update
		UpdateMacro(UTP_PADONLY, PT_AP11LMMNV, MoonRev >= 12 && MoonRevTime > 50.0*60.0, 74, MST_F_LUNAR_ORBIT_DOI_DAY_15);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_15: //LGC CSM state vector update to CMC CSM+LM state vector update
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, MoonRev >= 12 && MoonRevTime > 1.0*3600.0 + 10.0*60.0, 100, MST_F_LUNAR_ORBIT_DOI_DAY_16);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_16: //CMC CSM+LM state vector update to final phasing update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 13 && MoonRevTime > 40.0*60.0, 101, MST_F_LUNAR_ORBIT_DOI_DAY_17);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_17: //Final phasing update to CSM backup insertion update
		UpdateMacro(UTP_PADONLY, PT_AP11LMMNV, MoonRev >= 13 && MoonRevTime > 1.0*3600.0 + 20.0*60.0, 73, MST_F_LUNAR_ORBIT_DOI_DAY_18);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_18: //CSM backup insertion update to LM insertion update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, MoonRev >= 13 && MoonRevTime > 1.0*3600.0 + 25.0*60.0, 75, MST_F_LUNAR_ORBIT_DOI_DAY_19);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_19: //LM insertion update to CSM backup insertion update
		UpdateMacro(UTP_PADONLY, PT_AP11LMMNV, MoonRev >= 14 && MoonRevTime > 30.0*60.0, 77, MST_F_LUNAR_ORBIT_DOI_DAY_20);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_20: //CSM backup insertion update to LM insertion update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 3.0*60.0, 76, MST_F_LUNAR_ORBIT_DOI_DAY_21);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_21: //LM insertion update to CMC LM state vector update
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11LMMNV, MoonRev >= 14 && MoonRevTime > 1.0*3600.0 + 5.0*60.0, 78, MST_F_LUNAR_ORBIT_DOI_DAY_22);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_22: //CMC LM state vector update to CSI update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 1.0*60.0, 102, MST_F_LUNAR_ORBIT_DOI_DAY_23);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_23: //CSI update to APS depletion PAD update
		UpdateMacro(UTP_PADONLY, PT_AP10CSI, MoonRev >= 16 && MoonRevTime > 40.0*60.0 && cm->DockingStatus(0), 79, MST_F_LUNAR_ORBIT_DOI_DAY_24);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_24: //APS depletion PAD update to TEI-22 update
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11LMMNV, MoonRev >= 16 && MoonRevTime > 1.0*3600.0 + 15.0*60.0, 80, MST_F_LUNAR_ORBIT_DOI_DAY_25);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_25: //TEI-22 update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, MoonRev >= 17 && MoonRevTime > 1.0*3600.0 + 15.0*60.0, 34, MST_F_LUNAR_ORBIT_DOI_DAY_27);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_27: //State vector update to rev 22 map update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, true, 100, MST_F_LUNAR_ORBIT_DOI_DAY_28);
		break;
	case MST_F_LUNAR_ORBIT_DOI_DAY_28: //Rev 22 map update to rev 23 map update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 22 && MoonRevTime > 55.0*60.0, 45, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_1);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_1: //Rev 23 map update to TEI-23 update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, SubStateTime > 3.0*60.0, 46, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_2);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_2: //TEI-23 update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, true, 35, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_3);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_3: //State vector update to state vector update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 23 && MoonRevTime > 1.0*3600.0, 100, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_5);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_5: //State vector update to landmark tracking update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, true, 100, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_6);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_6: //Landmark tracking rev 24 update to TEI-24 update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, SubStateTime > 5.0*60.0, 53, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_7);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_7: //TEI-24 update to rev 24 map update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 3.0*60.0, 36, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_8);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_8: //Rev 24 map update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 24 && MoonRevTime > 1.0*3600.0 + 5.0*60.0, 47, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_9);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_9: //State vector update to TEI-25 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, true, 100, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_10);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_10: //TEI-25 update to landmark tracking rev 25 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 3.0*60.0, 37, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_11);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_11: //Landmark tracking rev 25 update to rev 25 map update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, SubStateTime > 5.0*60.0, 54, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_12);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_12: //Rev 25 map update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 25 && MoonRevTime > 1.0*3600.0 + 5.0*60.0, 48, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_13);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_13: //State vector update to TEI-26 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, true, 100, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_14);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_14: //TEI-26 update to landmark tracking rev 26 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 3.0*60.0, 38, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_15);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_15: //Landmark tracking rev 26 update to rev 26 map update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, SubStateTime > 5.0*60.0, 55, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_16);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_16: //Rev 26 map update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 26 && MoonRevTime > 1.0*3600.0 + 5.0*60.0, 49, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_17);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_17: //State vector update to TEI-27 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, true, 100, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_18);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_18: //TEI-27 update to landmark tracking rev 27 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 3.0*60.0, 39, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_19);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_19: //Landmark tracking rev 27 update to rev 27 map update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, SubStateTime > 5.0*60.0, 56, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_20);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_20: //Rev 27 map update to TEI-29 update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 27 && MoonRevTime > 40.0*60.0, 140, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_21);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_21: //TEI-29 update to rev 29 map update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 3.0*60.0, 130, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_22);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_22: //Rev 29 map update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 27 && MoonRevTime > 1.0*3600.0 + 5.0*60.0, 141, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_24);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_24: //State vector update to state vector update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 29 && MoonRevTime > 1.0*3600.0 + 10.0*60.0, 100, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_25);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_25: //State vector update to TEI-30 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, true, 100, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_26);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_26: //TEI-30 update to landmark tracking rev 30 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 3.0*60.0, 131, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_27);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_27: //Landmark tracking rev 30 update to rev 30 map update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, SubStateTime > 3.0*60.0, 57, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_28);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_28: //Rev 30 map update to preliminary TEI-31 update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 30 && MoonRevTime > 1.0*3600.0 + 15.0*60.0, 142, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_29);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_29: //Preliminary TEI-31 update to rev 31 map update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 3.0*60.0, 132, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_30);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_30: //Rev 31 map update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, SubStateTime > 3.0*60.0, 143, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_32);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_32: //State vector update to final TEI-31 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 31 && MoonRevTime > 35.0*60.0, 100, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_33);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_33: //Final TEI-31 update to TEI-32 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 3.0*60.0, 133, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_34);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_34: //TEI-32 update to TEI map update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 3.0*60.0, 134, MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_35);
		break;
	case MST_F_LUNAR_ORBIT_LMK_TRACK_DAY_35: //TEI map update to TEI
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, rtcc->GETEval2(rtcc->calcParams.TEI), 144, MST_F_TRANSEARTH_1);
		break;
	case MST_F_TRANSEARTH_1: //TEI to PTC REFSMMAT
		if (rtcc->GETEval2(rtcc->calcParams.TEI) && MissionPhase == MMST_LUNAR_ORBIT)
		{
			MissionPhase = MMST_TE_COAST;
		}
		if (rtcc->GETEval2(rtcc->calcParams.TEI + 35.0*60.0))
		{
			SlowIfDesired();
			setState(MST_F_TRANSEARTH_2);
		}
		break;
	case MST_F_TRANSEARTH_2: //PTC REFSMMAT to state vector update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.TEI + 2.0*3600.0 + 40.0*60.0), 10, MST_F_TRANSEARTH_3);
		break;
	case MST_F_TRANSEARTH_3: //State vector update to state vector update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.TEI + 10.0*3600.0 + 15.0*60.0), 100, MST_F_TRANSEARTH_4);
		break;
	case MST_F_TRANSEARTH_4: //State vector update to MCC-5 update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.TEI + 14.0*3600.0 + 10.0*60.0), 100, MST_F_TRANSEARTH_5);
		break;
	case MST_F_TRANSEARTH_5: //MCC-5 update to preliminary MCC-6 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TEI + 27.0*3600.0 + 20.0*60.0), 90, MST_F_TRANSEARTH_6);
		break;
	case MST_F_TRANSEARTH_6: //Preliminary MCC-6 update to Entry PAD update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 91, MST_F_TRANSEARTH_7);
		break;
	case MST_F_TRANSEARTH_7: //Entry PAD update to MCC-6 update
		UpdateMacro(UTP_PADONLY, PT_AP11ENT, rtcc->GETEval2(rtcc->calcParams.EI - 16.0*3600.0 - 45.0*60.0), 96, MST_F_TRANSEARTH_8);
		break;
	case MST_F_TRANSEARTH_8: //MCC-6 update to Entry PAD update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 92, MST_F_TRANSEARTH_9);
		break;
	case MST_F_TRANSEARTH_9: //Entry PAD update to MCC-7 decision update
		UpdateMacro(UTP_PADONLY, PT_AP11ENT, rtcc->GETEval2(rtcc->calcParams.EI - 5.0*3600.0 - 45.0*60.0), 97, MST_F_TRANSEARTH_10);
		break;
	case MST_F_TRANSEARTH_10: //MCC-7 decision update to MCC-7 update
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(rtcc->calcParams.EI - 4.5*3600.0), 93, MST_F_TRANSEARTH_11);
		break;
	case MST_F_TRANSEARTH_11: //MCC-7 update to Entry PAD update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 94, MST_F_TRANSEARTH_12);
		break;
	case MST_F_TRANSEARTH_12: //Entry PAD update to final entry update
		UpdateMacro(UTP_PADONLY, PT_AP11ENT, rtcc->GETEval2(rtcc->calcParams.EI - 1.0*3600.0), 98, MST_F_TRANSEARTH_13);
		break;
	case MST_F_TRANSEARTH_13: //Final entry update to CM/SM separation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11ENT, cm->stage == CM_STAGE, 99, MST_ENTRY);
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
	case MST_F_ABORT_ORBIT:
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