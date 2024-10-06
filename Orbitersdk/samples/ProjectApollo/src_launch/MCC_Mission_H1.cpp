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
#include "nassputils.h"

using namespace nassp;

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
		UpdateMacro(UTP_PADONLY, PT_TLIPAD, rtcc->GETEval2(rtcc->calcParams.TLI + 18.0), 14, MST_H1_TRANSLUNAR_DAY1_1);
		break;
	case MST_H1_TRANSLUNAR_DAY1_1: //TLI Evaluation to SIVB Evasive Maneuver
		UpdateMacro(UTP_NONE, PT_NONE, true, 15, MST_H1_TRANSLUNAR_DAY1_2, scrubbed, rtcc->GETEval2(3.0*3600.0), MST_H1_EPO1);
		break;
	case MST_H1_TRANSLUNAR_DAY1_2:
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
			if (rtcc->GETEval2(rtcc->calcParams.TLI + 3600.0 + 20.0*60.0))  //1:20h from TLI cutoff
			{
				SlowIfDesired();
				setState(MST_H1_TRANSLUNAR_DAY1_3);
			}
		}
		break;
		}
		break;
	case MST_H1_TRANSLUNAR_DAY1_3: //SIVB Evasive Maneuver to TB8 Enable
		switch (SubState) {
		case 0:
		{
			if (cm->GetStage() >= CSM_LEM_STAGE) //Sanity check if CSM sep has happened
			{
				setSubState(1);
			}
		}
		break;
		case 1:
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

			if (sivb->DockingStatus(0) == 0) //Test for LM ejection
			{
				setSubState(2);
			}
		}
		break;
		case 2:
		{
			if (SubStateTime > 3.5*60.0) //Start yaw maneuver not earlier than 3.5 minutes after LM ejection
			{
				sivb->GetIU()->GetDCS()->Uplink(DCSUPLINK_EVASIVE_MANEUVER_ENABLE, NULL);
				setSubState(3);
			}
		}
		break;
		case 3:
			if (SubStateTime >= 8.0*60.0 && rtcc->GETEval2(rtcc->calcParams.TLI + 3600.0 + 31.0*60.0 + 40.0)) //11m40s after LM ejection. Not before 8 minutes after yaw maneuver command was sent
			{
				SlowIfDesired();
				setState(MST_H1_TRANSLUNAR_DAY1_4);
			}
			break;
		}
		break;
	case MST_H1_TRANSLUNAR_DAY1_4:  //TB8 enable to PTC REFSMMAT update
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

			sivb->GetIU()->GetDCS()->Uplink(DCSUPLINK_TIMEBASE_8_ENABLE, NULL);
			setSubState(2);
		}
		break;
		case 2:
			if (rtcc->GETEval2(rtcc->calcParams.TLI + 2.0*3600.0 + 29.0*60.0))
			{
				SlowIfDesired();
				setState(MST_H1_TRANSLUNAR_DAY1_5);
			}
			break;
		}
		break;
	case MST_H1_TRANSLUNAR_DAY1_5: //PTC REFSMMAT update to MCC-1 Evaluation
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 5.0*60.0, 18, MST_H1_TRANSLUNAR_DAY1_6);
		break;
	case MST_H1_TRANSLUNAR_DAY1_6: //MCC-1 Evaluation to Block Data 1
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(rtcc->calcParams.TLI + 2.0*3600.0 + 59.0*60.0), 19, MST_H1_TRANSLUNAR_DAY1_7);
		break;
	case MST_H1_TRANSLUNAR_DAY1_7: //Block Data 1 to PTC Quads Decision
		UpdateMacro(UTP_PADONLY, PT_P37PAD, rtcc->GETEval2(rtcc->calcParams.TLI + 4.0*3600.0 + 10.0*60.0), 16, MST_H1_TRANSLUNAR_DAY1_8);
		break;
	case MST_H1_TRANSLUNAR_DAY1_8: //PTC Quads Decision to MCC-1 update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, rtcc->GETEval2(rtcc->calcParams.TLI + 7.0*3600.0 + 15.0*60.0), 140, MST_H1_TRANSLUNAR_DAY1_9);
		break;
	case MST_H1_TRANSLUNAR_DAY1_9: //MCC-1 update to MCC-2 Evaluation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TLI + 11.0*3600.0 + 5.0*60.0), 21, MST_H1_TRANSLUNAR_DAY1_10);
		break;
	case MST_H1_TRANSLUNAR_DAY1_10: //MCC-2 Evaluation to Block Data 2
		UpdateMacro(UTP_NONE, PT_NONE, true, 20, MST_H1_TRANSLUNAR_DAY1_11);
		break;
	case MST_H1_TRANSLUNAR_DAY1_11: //Block Data 2 to PTC Quads Decision
		UpdateMacro(UTP_PADONLY, PT_P37PAD, rtcc->GETEval2(rtcc->calcParams.TLI + 13.5*3600.0), 17, MST_H1_TRANSLUNAR_DAY1_12);
		break;
	case MST_H1_TRANSLUNAR_DAY1_12: //PTC Quads Decision to MCC-2 update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, rtcc->GETEval2(rtcc->calcParams.TLI + 26.0*3600.0 + 27.0*60.0), 140, MST_H1_TRANSLUNAR_DAY2_1);
		break;
	case MST_H1_TRANSLUNAR_DAY2_1: //MCC-2 update to PTC Quads Decision
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TLI + 28.0*3600.0 + 7.0*60.0), 22, MST_H1_TRANSLUNAR_DAY2_2);
		break;
	case MST_H1_TRANSLUNAR_DAY2_2: //PTC Quads Decision to Lunar Flyby PAD
		UpdateMacro(UTP_PADONLY, PT_GENERIC, rtcc->GETEval2(rtcc->calcParams.TLI + 31.0*3600.0 + 57.0*60.0), 140, MST_H1_TRANSLUNAR_DAY2_3);
		break;
	case MST_H1_TRANSLUNAR_DAY2_3: //Lunar Flyby PAD to SV Update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TLI + 39.0*3600.0 + 1.0*60.0), 23, MST_H1_TRANSLUNAR_DAY2_4);
		break;
	case MST_H1_TRANSLUNAR_DAY2_4: //SV Update to MCC-3
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.LOI - 23.5*3600.0), 5, MST_H1_TRANSLUNAR_DAY3_1);
		break;
	case MST_H1_TRANSLUNAR_DAY3_1: //MCC-3 update to PTC Quads Decision
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.LOI - 21.8*3600.0), 24, MST_H1_TRANSLUNAR_DAY3_2, scrubbed, rtcc->GETEval2(rtcc->calcParams.LOI - 6.5*3600.0), MST_H1_TRANSLUNAR_DAY4_1);
		break;
	case MST_H1_TRANSLUNAR_DAY3_2: //PTC Quads Decision to MCC-4 Evaluation
		UpdateMacro(UTP_PADONLY, PT_GENERIC, rtcc->GETEval2(rtcc->calcParams.LOI - 6.5*3600.0), 140, MST_H1_TRANSLUNAR_DAY4_1);
		break;
	case MST_H1_TRANSLUNAR_DAY4_1: //MCC-4 Evaluation to MCC-4 update or SV update
		UpdateMacro(UTP_NONE, PT_NONE, SubStateTime > 5.0*60.0, 25, MST_H1_TRANSLUNAR_DAY4_2, scrubbed, rtcc->GETEval2(rtcc->calcParams.LOI - 4.5*3600.0), MST_H1_TRANSLUNAR_NO_MCC4_1);
		break;
	case MST_H1_TRANSLUNAR_NO_MCC4_1: //SV update to PC+2 update *No MCC-4 Timeline*
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 20.0*60.0, 5, MST_H1_TRANSLUNAR_NO_MCC4_2);
		break;
	case MST_H1_TRANSLUNAR_NO_MCC4_2: //PC+2 update to LOI-1 update (preliminary) *No MCC-4 Timeline*
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.LOI - 2.0*3600.0 - 20.0*60.0), 28, MST_H1_TRANSLUNAR_NO_MCC4_3);
		break;
	case MST_H1_TRANSLUNAR_NO_MCC4_3: //LOI-1 update (preliminary) to TEI-1 update *No MCC-4 Timeline*
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 29, MST_H1_TRANSLUNAR_DAY4_5);
		break;
	case MST_H1_TRANSLUNAR_DAY4_2: //MCC-4 update to PC+2 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 26, MST_H1_TRANSLUNAR_DAY4_3);
		break;
	case MST_H1_TRANSLUNAR_DAY4_3: //PC+2 update to LOI-1 update (preliminary)
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.LOI - 2.0*3600.0 - 20.0*60.0), 27, MST_H1_TRANSLUNAR_DAY4_4);
		break;
	case MST_H1_TRANSLUNAR_DAY4_4: //LOI-1 update (preliminary) to TEI-1 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 30, MST_H1_TRANSLUNAR_DAY4_5);
		break;
	case MST_H1_TRANSLUNAR_DAY4_5: //TEI-1 update to TEI-4 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 5.0*60.0, 40, MST_H1_TRANSLUNAR_DAY4_6);
		break;
	case MST_H1_TRANSLUNAR_DAY4_6: //TEI-4 update to Rev 1 Map Update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.LOI - 1.0*3600.0), 41, MST_H1_TRANSLUNAR_DAY4_7);
		break;
	case MST_H1_TRANSLUNAR_DAY4_7: //Rev 1 Map Update to LOI-1 update (final)
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, SubStateTime > 5.0*60.0, 60, MST_H1_TRANSLUNAR_DAY4_8);
		break;
	case MST_H1_TRANSLUNAR_DAY4_8: //LOI-1 update (final) to LOI-1 Evaluation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->TimeofIgnition + 7.0*60.0), 30, MST_H1_TRANSLUNAR_DAY4_9);
		break;
	case MST_H1_TRANSLUNAR_DAY4_9: //LOI-1 Evaluation to Lunar orbit phase begin or PC+2
		UpdateMacro(UTP_NONE, PT_NONE, true, 33, MST_H1_LUNAR_ORBIT_LOI_DAY_1, scrubbed, true, MST_H1_ABORT);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_1: //Lunar orbit phase begin
		switch (SubState) {
		case 0:
			MissionPhase = MMST_LUNAR_ORBIT;
			setSubState(1);
			break;
		case 1:
			if (MoonRev >= 1 && MoonRevTime > 65.0*60.0)
			{
				SlowIfDesired();
				setState(MST_H1_LUNAR_ORBIT_LOI_DAY_2);
			}
			break;
		}
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_2: //Rev 2 map update to TEI-5 update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 2 && MoonRevTime > 30.0*60.0, 600, MST_H1_LUNAR_ORBIT_LOI_DAY_3);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_3: //LOI-2 update to TEI-5 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 31, MST_H1_LUNAR_ORBIT_LOI_DAY_4);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_4: //TEI-5 update to rev 3 map update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 5.0*60.0, 42, MST_H1_LUNAR_ORBIT_LOI_DAY_5);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_5: //Rev 3 map update to H-1 landmark tracking update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 3 && MoonRevTime > 45.0*60.0, 600, MST_H1_LUNAR_ORBIT_LOI_DAY_6);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_6: //H-1 landmark tracking update to rev 4 map update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, SubStateTime > 5.0*60.0, 61, MST_H1_LUNAR_ORBIT_LOI_DAY_7);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_7: //Rev 4 map update to TEI-11 update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 4 && MoonRevTime > 1.0*3600.0 + 20.0*60.0, 600, MST_H1_LUNAR_ORBIT_LOI_DAY_8);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_8: //TEI-11 update to state vector update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, true, 43, MST_H1_LUNAR_ORBIT_LOI_DAY_9);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_9: //State vector update to rev 5 map update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 3.0*60.0, 5, MST_H1_LUNAR_ORBIT_LOI_DAY_10);
		break;
	case MST_H1_LUNAR_ORBIT_LOI_DAY_10: //Rev 5 map update to Fra Mauro Photography
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 10 && MoonRevTime > 45.0*60.0, 600, MST_H1_LUNAR_ORBIT_PDI_DAY_1);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_1: //Fra Mauro Photography to TEI-34 update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, MoonRev >= 10 && MoonRevTime > 1.0*3600.0, 602, MST_H1_LUNAR_ORBIT_PDI_DAY_2);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_2: //TEI-34 update to rev 11 map update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 5.0*60.0, 44, MST_H1_LUNAR_ORBIT_PDI_DAY_3);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_3: //Rev 11 map update to CSM DAP PAD
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, SubStateTime > 3.0*60.0, 600, MST_H1_LUNAR_ORBIT_PDI_DAY_4);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_4: //CSM DAP PAD to LS Update
		UpdateMacro(UTP_PADONLY, PT_AP10DAPDATA, SubStateTime > 5.0*60.0, 7, MST_H1_LUNAR_ORBIT_PDI_DAY_5);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_5: //LS Update to rev 12 map update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 11 && MoonRevTime > 30.0*60.0, 32, MST_H1_LUNAR_ORBIT_PDI_DAY_6);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_6: //Rev 12 map update to Lmk 193 Landmark Tracking PAD
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 11 && MoonRevTime > 50.0*60.0, 600, MST_H1_LUNAR_ORBIT_PDI_DAY_7);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_7: //Lmk 193 Landmark Tracking PAD to CSM SV Update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11LMARKTRKPAD, MoonRev >= 12 && MoonRevTime > 25.0*60.0, 62, MST_H1_LUNAR_ORBIT_PDI_DAY_8);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_8: //CSM SV Update to LM Activation Data
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 3.0*60.0, 5, MST_H1_LUNAR_ORBIT_PDI_DAY_9);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_9: //LM Activation Data to LGC activation update
		UpdateMacro(UTP_PADONLY, PT_LMACTDATA, true, 9, MST_H1_LUNAR_ORBIT_PDI_DAY_10);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_10: //LGC activation update to AGS activation update
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, SubStateTime > 5.0*60.0, 35, MST_H1_LUNAR_ORBIT_PDI_DAY_11);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_11: //AGS activation update to Separation maneuver update
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11AGSACT, SubStateTime > 5.0*60.0, 36, MST_H1_LUNAR_ORBIT_PDI_DAY_12);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_12: //Separation maneuver update to rev 13 map update
		UpdateMacro(UTP_PADONLY, PT_AP12SEPPAD, SubStateTime > 5.0*60.0, 37, MST_H1_LUNAR_ORBIT_PDI_DAY_13);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_13: //Rev 13 map update to DOI update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, rtcc->GETEval2(rtcc->calcParams.SEP - 25.0*60.0), 600, MST_H1_LUNAR_ORBIT_PDI_DAY_14);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_14: //DOI update to No PDI+12 PAD
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11LMMNV, SubStateTime > 3.0*60.0, 38, MST_H1_LUNAR_ORBIT_PDI_DAY_15);
		break;
		//Recycle from MST_H1_LUNAR_ORBIT_NO_PDI
	case MST_H1_LUNAR_ORBIT_PDI_DAY_15: //No PDI+12 PAD to PDI PAD
		UpdateMacro(UTP_PADONLY, PT_AP11LMMNV, SubStateTime > 3.0*60.0, 72, MST_H1_LUNAR_ORBIT_PDI_DAY_16);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_16: //PDI PAD to PDI Abort PAD
		UpdateMacro(UTP_PADONLY, PT_AP11PDIPAD, SubStateTime > 3.0*60.0, 70, MST_H1_LUNAR_ORBIT_PDI_DAY_17);
		break;
	case MST_H1_LUNAR_ORBIT_PDI_DAY_17: //PDI Abort PAD to Lunar Surface PAD
		UpdateMacro(UTP_PADONLY, PT_AP12PDIABORTPAD, SubStateTime > 3.0*60.0, 71, MST_H1_LUNAR_ORBIT_PRE_DOI_1);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_DOI_1: //Lunar Surface PAD to P22 Acquistion time
		UpdateMacro(UTP_PADONLY, PT_AP12LUNSURFPAD, SubStateTime > 3.0*60.0, 73, MST_H1_LUNAR_ORBIT_PRE_DOI_2);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_DOI_2: //P22 Acquistion time to LM SV update
		UpdateMacro(UTP_PADONLY, PT_LMP22ACQPAD, rtcc->GETEval2(rtcc->calcParams.SEP + 5.0*60.0), 58, MST_H1_LUNAR_ORBIT_PRE_DOI_3);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_DOI_3: //LM SV update to rev 14 map update
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, SubStateTime > 2.0*60.0, 400, MST_H1_LUNAR_ORBIT_PRE_DOI_4);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_DOI_4: //Rev 14 map update to rev 15 map update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP10MAPUPDATE, rtcc->GETEval2(rtcc->calcParams.PDI - 33.0*60.0), 76, MST_H1_LUNAR_ORBIT_PRE_PDI_1);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_PDI_1: //Rev 15 map update to DOI Evaluation
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, rtcc->GETEval2(rtcc->calcParams.PDI - 21.0*60.0), 600, MST_H1_LUNAR_ORBIT_PRE_PDI_2);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_PDI_2: //DOI Evaluation to SV and RLS uplink
		UpdateMacro(UTP_NONE, PT_NONE, SubStateTime > 3.0*60.0, 77, MST_H1_LUNAR_ORBIT_PRE_PDI_3);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_PDI_3: //SV and RLS uplink to PDI Evaluation
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.PDI + 2.0*60.0), 75, MST_H1_LUNAR_ORBIT_PRE_LANDING_1);
		break;
	case MST_H1_LUNAR_ORBIT_PRE_LANDING_1: //PDI Evaluation to landing confirmation or PDI Recycle
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->calcParams.tgt->GroundContact(), 78, MST_H1_LUNAR_ORBIT_POST_LANDING_1, scrubbed, SubStateTime > 3.0*60.0, MST_H1_LUNAR_ORBIT_NO_PDI);
		break;
	case MST_H1_LUNAR_ORBIT_NO_PDI: //PDI Recycle to No PDI+12 PAD
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11AGSACT, SubStateTime > 3.0*60.0, 170, MST_H1_LUNAR_ORBIT_PDI_DAY_15);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_1: //Landing confirmation to T1
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(rtcc->calcParams.PDI + 15.0*60.0), 79, MST_H1_LUNAR_ORBIT_POST_LANDING_2);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_2: //T1 to T2
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(rtcc->calcParams.PDI + 19.0*60.0 + 30.0), 80, MST_H1_LUNAR_ORBIT_POST_LANDING_3);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_3: //T2 to Landing Site Update
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(rtcc->calcParams.PDI + 75.0*60.0), 81, MST_H1_LUNAR_ORBIT_POST_LANDING_4);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_4: //Landing Site Update to Lmk 193 Landmark Tracking PAD
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_LMP22ACQPAD, SubStateTime > 8.0*60.0, 74, MST_H1_LUNAR_ORBIT_POST_LANDING_5);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_5: //Lmk 193 Landmark Tracking PAD to CSM SV update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, true, 63, MST_H1_LUNAR_ORBIT_POST_LANDING_6);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_6: //CSM SV update to rev 16 map update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 3.0*60.0, 1, MST_H1_LUNAR_ORBIT_POST_LANDING_7);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_7: //Rev 16 map update to DAP Load
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, rtcc->GETEval2(rtcc->calcParams.PDI + 2.0*3600.0 + 10.0*60.0), 600, MST_H1_LUNAR_ORBIT_POST_LANDING_8);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_8: //DAP Load to Liftoff Times Update 1
		UpdateMacro(UTP_PADONLY, PT_AP10DAPDATA, SubStateTime > 3.0*60.0, 8, MST_H1_LUNAR_ORBIT_POST_LANDING_9);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_9: //Liftoff Times Update 1 to LM Tracking PAD
		UpdateMacro(UTP_PADONLY, PT_LIFTOFFTIMES, rtcc->GETEval2(rtcc->calcParams.PDI + 3.0*3600.0 + 23.0*60.0), 85, MST_H1_LUNAR_ORBIT_POST_LANDING_10);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_10: //LM Tracking PAD to rev 17 map update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, SubStateTime > 3.0*60.0, 66, MST_H1_LUNAR_ORBIT_POST_LANDING_11);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_11: //Rev 17 map update to rev 18 map update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 17 && MoonRevTime > 65.0*60.0, 600, MST_H1_LUNAR_ORBIT_POST_LANDING_12);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_12: //Rev 18 map update to rev 19 map update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 18 && MoonRevTime > 30.0*60.0, 600, MST_H1_LUNAR_ORBIT_POST_LANDING_13);
		break;
	case MST_H1_LUNAR_ORBIT_POST_LANDING_13: //Rev 19 map update to PC-1 Evaluation
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 18 && MoonRevTime > 52.0*60.0, 600, MST_H1_LUNAR_ORBIT_PLANE_CHANGE_1);
		break;
	case MST_H1_LUNAR_ORBIT_PLANE_CHANGE_1: //PC-1 Evaluation to PC-1 Update or CMC CSM state vector update
		UpdateMacro(UTP_NONE, PT_NONE, true, 93, MST_H1_LUNAR_ORBIT_PLANE_CHANGE_2, scrubbed, MoonRev >= 19 && MoonRevTime > 50.0*60.0, MST_H1_LUNAR_ORBIT_NO_PLANE_CHANGE_1);
		break;
	case MST_H1_LUNAR_ORBIT_PLANE_CHANGE_2: //PC-1 Update to Liftoff Times Update 2
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->TimeofIgnition + 5*60.0), 94, MST_H1_LUNAR_ORBIT_EVA_DAY_1);
		break;
	case MST_H1_LUNAR_ORBIT_NO_PLANE_CHANGE_1: //CMC CSM state vector update to Liftoff Times Update 2
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 5.0*60.0, 1, MST_H1_LUNAR_ORBIT_EVA_DAY_1);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_1: //Liftoff Times Update 2 to rev 20 map update
		UpdateMacro(UTP_PADONLY, PT_LIFTOFFTIMES, SubStateTime > 5.0*60.0, 86, MST_H1_LUNAR_ORBIT_EVA_DAY_2);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_2: //Rev 20 map update to Lunar Liftoff REFSMMAT Uplink
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, true, 600, MST_H1_LUNAR_ORBIT_EVA_DAY_3);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_3: //Lunar Liftoff REFSMMAT Uplink to LM Liftoff Times Update 3
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 24 && MoonRevTime > 75.0*60.0, 96, MST_H1_LUNAR_ORBIT_EVA_DAY_4);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_4: //LM Liftoff Times Update 3 to rev 26 map update
		UpdateMacro(UTP_PADONLY, PT_LIFTOFFTIMES, MoonRev >= 25 && MoonRevTime > 60.0*60.0, 87, MST_H1_LUNAR_ORBIT_EVA_DAY_5);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_5: //Rev 26 map update to CMC CSM state vector update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, true, 600, MST_H1_LUNAR_ORBIT_EVA_DAY_6);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_6: //CMC CSM state vector update to rev 27 map update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 26 && MoonRevTime > 40.0*60.0, 1, MST_H1_LUNAR_ORBIT_EVA_DAY_7);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_7: //Rev 27 map update to LANSBERG A Tracking PAD
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, SubStateTime > 3.0*60.0, 600, MST_H1_LUNAR_ORBIT_EVA_DAY_8);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_8: //LANSBERG A Tracking PAD to S-158 PAD Rev 27
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, MoonRev >= 26 && MoonRevTime > 75.0*60.0, 64, MST_H1_LUNAR_ORBIT_EVA_DAY_9);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_9: //S-158 PAD Rev 27 PAD to rev 28 map update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, MoonRev >= 27 && MoonRevTime > 70.0*60.0, 603, MST_H1_LUNAR_ORBIT_EVA_DAY_10);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_10: //Rev 28 map update to S-158 PAD rev 28
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, SubStateTime > 3.0*60.0, 600, MST_H1_LUNAR_ORBIT_EVA_DAY_11);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_11: //S-158 PAD rev 28 to rev 29 map update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, MoonRev >= 28 && MoonRevTime > 25.0*60.0, 604, MST_H1_LUNAR_ORBIT_EVA_DAY_12);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_12: //Rev 29 map update to LM Liftoff Times Update 4
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 28 && MoonRevTime > 100.0*60.0, 600, MST_H1_LUNAR_ORBIT_EVA_DAY_13);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_13: //Liftoff Times Update 4 to P22 Acquistion time
		UpdateMacro(UTP_PADONLY, PT_LIFTOFFTIMES, SubStateTime > 2.0*60.0, 88, MST_H1_LUNAR_ORBIT_EVA_DAY_14);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_14: //P22 Acquistion time to CSM SV uplink
		UpdateMacro(UTP_PADONLY, PT_LMP22ACQPAD, MoonRev >= 29 && MoonRevTime > 22.0*60.00, 59, MST_H1_LUNAR_ORBIT_EVA_DAY_15);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_15: //CSM SV uplink to LM SV uplink
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 3.0*60.0, 1, MST_H1_LUNAR_ORBIT_EVA_DAY_16);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_16: //LM SV uplink to Lmk 193 Landmark Tracking PAD
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, SubStateTime > 3.0*60.0, 401, MST_H1_LUNAR_ORBIT_EVA_DAY_17);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_17: //Lmk 193 Landmark Tracking PAD to rev 30 map update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, SubStateTime > 3.0*60.0, 63, MST_H1_LUNAR_ORBIT_EVA_DAY_18);
		break;
	case MST_H1_LUNAR_ORBIT_EVA_DAY_18: //Rev 30 map update to Nominal Insertion targeting + CMC SV update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 29 && MoonRevTime > 75.0*60.0, 600, MST_H1_LUNAR_ORBIT_ASCENT_DAY_1);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_1: //Nominal Insertion targeting + CMC SV update to LM Ascent PAD
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.LunarLiftoff - 90.0*60.0), 100, MST_H1_LUNAR_ORBIT_ASCENT_DAY_2);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_2: //LM Ascent PAD to CSI Data Card
		UpdateMacro(UTP_PADONLY, PT_AP12LMASCPAD, SubStateTime > 5.0*60.0, 105, MST_H1_LUNAR_ORBIT_ASCENT_DAY_3);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_3: //CSI Data Card to LM Tracking PAD
		UpdateMacro(UTP_PADONLY, PT_AP10CSI, rtcc->GETEval2(rtcc->calcParams.LunarLiftoff - 45.0*60.0), 106, MST_H1_LUNAR_ORBIT_ASCENT_DAY_4);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_4: //LM Tracking PAD to CMC SV uplinks
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, true, 66, MST_H1_LUNAR_ORBIT_ASCENT_DAY_5);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_5: //CMC SV uplinks to LGC SV + RLS uplinks
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, SubStateTime > 5.0*60.0, 101, MST_H1_LUNAR_ORBIT_ASCENT_DAY_6);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_6: //LGC SV + RLS uplinks to LM Liftoff Evaluation
		UpdateMacro(UTP_LGCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.LunarLiftoff + 20.0), 102, MST_H1_LUNAR_ORBIT_ASCENT_DAY_7);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_7: //LM Liftoff Evaluation to CMC LM State Vector update
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(rtcc->calcParams.Insertion + 120.0), 107, MST_H1_LUNAR_ORBIT_ASCENT_DAY_8, scrubbed, SubStateTime > 15.0*60.0, MST_H1_LUNAR_ORBIT_ASCENT_DAY_1);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_8: //CMC LM State Vector update to DAP PAD
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, (rtcc->calcParams.src->DockingStatus(0) == 1) && rtcc->GETEval2(rtcc->calcParams.LunarLiftoff + 3.65*3600.0), 2, MST_H1_LUNAR_ORBIT_ASCENT_DAY_9);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_9: //DAP PAD to rev 33 map update
		UpdateMacro(UTP_PADONLY, PT_AP10DAPDATA, rtcc->GETEval2(rtcc->calcParams.LunarLiftoff + 4.0*3600.0), 700, MST_H1_LUNAR_ORBIT_ASCENT_DAY_10);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_10: //Rev 33 map update to SEP burn PAD
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, SubStateTime > 3.0*60.0, 600, MST_H1_LUNAR_ORBIT_ASCENT_DAY_11);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_11: //SEP burn PAD to Impact burn PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 3.0*60.0, 110, MST_H1_LUNAR_ORBIT_ASCENT_DAY_12);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_12: //Impact burn PAD to CSM P76 PAD
		UpdateMacro(UTP_PADWITHLGCUPLINK, PT_AP11LMMNV, rtcc->GETEval2(rtcc->calcParams.SEP - 50.0*60.0), 111, MST_H1_LUNAR_ORBIT_ASCENT_DAY_13);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_13: //CSM P76 PAD to rev 34 map update
		UpdateMacro(UTP_PADONLY, PT_AP11P76PAD, SubStateTime > 3.0*60.0, 112, MST_H1_LUNAR_ORBIT_ASCENT_DAY_14);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_14: //Rev 34 map update to DAP data
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, SubStateTime > 3.0*60.0, 600, MST_H1_LUNAR_ORBIT_ASCENT_DAY_15);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_15: //DAP data to P42 uplink
		UpdateMacro(UTP_PADONLY, PT_AP10DAPDATA, rtcc->GETEval2(rtcc->calcParams.SEP + 5.0*60.0), 7, MST_H1_LUNAR_ORBIT_ASCENT_DAY_16);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_16: //P42 uplink to LM DSKY Pro
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, SubStateTime > 20.0, 120, MST_H1_LUNAR_ORBIT_ASCENT_DAY_17);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_17: //LM DSKY Pro to LM DSKY Enter
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, SubStateTime > 20.0, 121, MST_H1_LUNAR_ORBIT_ASCENT_DAY_18);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_18: //LM DSKY Enter to TEI-39 PAD
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, rtcc->GETEval2(rtcc->TimeofIgnition - 10.0*60.0), 122, MST_H1_LUNAR_ORBIT_ASCENT_DAY_19);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_19: //TEI-39 PAD to LM command ullage off
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TIGSTORE1), 45, MST_H1_LUNAR_ORBIT_ASCENT_DAY_20);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_20: //LM command ullage off to LM impact prediction
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, rtcc->GETEval2(rtcc->calcParams.TIGSTORE1 + 5.0*60.0), 123, MST_H1_LUNAR_ORBIT_ASCENT_DAY_21);
		break;
	case MST_H1_LUNAR_ORBIT_ASCENT_DAY_21: //LM impact prediction to PC-2 Update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, MoonRev >= 38 && MoonRevTime > 50.0*60.0, 124, MST_H1_LUNAR_ORBIT_PC2_DAY_1);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_1: //PC-2 Update to Photography REFSMMAT calculation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 95, MST_H1_LUNAR_ORBIT_PC2_DAY_2);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_2: //Photography REFSMMAT calculation to rev 39 map update
		UpdateMacro(UTP_NONE, PT_NONE, true, 130, MST_H1_LUNAR_ORBIT_PC2_DAY_3);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_3: //Rev 39 map update to TEI-41 PAD
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, SubStateTime > 3.0*60.0, 600, MST_H1_LUNAR_ORBIT_PC2_DAY_4);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_4: //TEI-41 PAD to Lalande Photography
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval2(rtcc->TimeofIgnition + 5.0*60.0), 46, MST_H1_LUNAR_ORBIT_PC2_DAY_5);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_5: //Lalande Photography to Photography REFSMMAT Uplink
		UpdateMacro(UTP_PADONLY, PT_GENERIC, true, 605, MST_H1_LUNAR_ORBIT_PC2_DAY_6);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_6: //Photography REFSMMAT Uplink to stereo photo times
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 39 && MoonRevTime > 83.0*60.0, 131, MST_H1_LUNAR_ORBIT_PC2_DAY_7);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_7: //Stereo photo times to rev 40 map update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, SubStateTime > 3.0*60.0, 606, MST_H1_LUNAR_ORBIT_PC2_DAY_8);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_8: //Rev 40 map update to rev 41 map update
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 40 && MoonRevTime > 50.0*60.0, 600, MST_H1_LUNAR_ORBIT_PC2_DAY_9);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_9: //Rev 41 map update to TEI-43 PAD
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, SubStateTime > 3.0*60.0, 600, MST_H1_LUNAR_ORBIT_PC2_DAY_10);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_10: //TEI-43 PAD to Descartes photography
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, MoonRev >= 40 && MoonRevTime > 83.0*60.0, 47, MST_H1_LUNAR_ORBIT_PC2_DAY_11);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_11: //Descartes photography to Fra Mauro photography
		UpdateMacro(UTP_PADONLY, PT_GENERIC, SubStateTime > 3.0*60.0, 607, MST_H1_LUNAR_ORBIT_PC2_DAY_12);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_12: //Fra Mauro photography to rev 42 map update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, MoonRev >= 41 && MoonRevTime > 35.0*60.0, 608, MST_H1_LUNAR_ORBIT_PC2_DAY_13);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_13: //Rev 42 map update to Landmark tracking PAD
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 41 && MoonRevTime > 70.0*60.0, 600, MST_H1_LUNAR_ORBIT_PC2_DAY_14);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_14: //Landmark tracking PAD to CSM SV update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, true, 65, MST_H1_LUNAR_ORBIT_PC2_DAY_15);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_15: //CSM SV update to rev 43 map update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 42 && MoonRevTime > 30.0*60.0, 5, MST_H1_LUNAR_ORBIT_PC2_DAY_16);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_16: //Rev 43 map update to TEI-45 PAD (block data)
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, SubStateTime > 3.0*60.0, 600, MST_H1_LUNAR_ORBIT_PC2_DAY_17);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_17: //TEI-45 PAD (block data) to Landmark tracking PAD
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, MoonRev >= 42 && MoonRevTime > 75.0*60.0, 48, MST_H1_LUNAR_ORBIT_PC2_DAY_18);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_18: //Landmark tracking PAD to CSM SV update
		UpdateMacro(UTP_PADONLY, PT_AP11LMARKTRKPAD, SubStateTime > 10.0*60.0, 65, MST_H1_LUNAR_ORBIT_PC2_DAY_19);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_19: //CSM SV update to rev 44 map update
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 43 && MoonRevTime > 30.0*60.0, 5, MST_H1_LUNAR_ORBIT_PC2_DAY_20);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_20: //Rev 44 map update to stereo photo times
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, MoonRev >= 43 && MoonRevTime > 90.0*60.0, 600, MST_H1_LUNAR_ORBIT_PC2_DAY_21);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_21: //Stereo photo times to rev 45 map update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, MoonRev >= 44 && MoonRevTime > 35.0*60.0, 609, MST_H1_LUNAR_ORBIT_PC2_DAY_22);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_22: //Rev 45 map update to TEI-45 PAD (preliminary)
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, SubStateTime > 3.0*60.0, 600, MST_H1_LUNAR_ORBIT_PC2_DAY_23);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_23: //TEI-45 PAD (preliminary) to TEI REFSMMAT uplink
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, MoonRev >= 44 && MoonRevTime > 85.0*60.0, 49, MST_H1_LUNAR_ORBIT_PC2_DAY_24);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_24: //TEI REFSMMAT Uplink to TEI-45 PAD (final)
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, MoonRev >= 45 && MoonRevTime > 50.0*60.0, 132, MST_H1_LUNAR_ORBIT_PC2_DAY_25);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_25: //TEI-45 PAD (final) to TEI-46 PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 50, MST_H1_LUNAR_ORBIT_PC2_DAY_26);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_26: //TEI-46 PAD to rev 46 map update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 5.0*60.0, 51, MST_H1_LUNAR_ORBIT_PC2_DAY_27);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_27: //Rev 46 map update to TEI Evaluation
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, rtcc->GETEval2(rtcc->calcParams.TEI + 300.0), 601, MST_H1_LUNAR_ORBIT_PC2_DAY_28);
		break;
	case MST_H1_LUNAR_ORBIT_PC2_DAY_28: //TEI Evaluation to TEI
		UpdateMacro(UTP_NONE, PT_NONE, true, 200, MST_H1_TRANSEARTH_DAY1_1, scrubbed, rtcc->GETEval2(rtcc->calcParams.TEI + 30.0*60.0), MST_H1_LUNAR_ORBIT_PC2_DAY_26);
		break;
	case MST_H1_TRANSEARTH_DAY1_1: //TEI to PTC REFSMMAT
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
				setState(MST_H1_TRANSEARTH_DAY1_2);
			}
			break;
		}
		break;
	case MST_H1_TRANSEARTH_DAY1_2: //PTC REFSMMAT to PTC Quads Decision
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.TEI + 1.0*3600.0 + 10.0*60.0), 18, MST_H1_TRANSEARTH_DAY1_3);
		break;
	case MST_H1_TRANSEARTH_DAY1_3: //PTC Quads Decision to MCC-5 update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, rtcc->GETEval2(rtcc->calcParams.TEI + 12.0*3600.0), 140, MST_H1_TRANSEARTH_DAY2_1);
		break;
	case MST_H1_TRANSEARTH_DAY2_1: //MCC-5 update to PTC Quads Decision
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval2(rtcc->calcParams.TEI + 17.0*3600.0 + 40.0*60.0), 210, MST_H1_TRANSEARTH_DAY2_2);
		break;
	case MST_H1_TRANSEARTH_DAY2_2: //PTC Quads Decision to PTC Quads Decision
		UpdateMacro(UTP_PADONLY, PT_GENERIC, rtcc->GETEval2(rtcc->calcParams.TEI + 23.0*3600.0 + 40.0*60.0), 140, MST_H1_TRANSEARTH_DAY2_3);
		break;
	case MST_H1_TRANSEARTH_DAY2_3: //PTC Quads Decision to CSM SV update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, rtcc->GETEval2(rtcc->calcParams.EI - 36.0 * 3600.0), 140, MST_H1_TRANSEARTH_DAY3_1);
		break;
	case MST_H1_TRANSEARTH_DAY3_1: //CSM SV update to PTC Quads Decision
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval2(rtcc->calcParams.EI - (30.0 * 3600.0 + 20.0*60.0)), 5, MST_H1_TRANSEARTH_DAY3_2);
		break;
	case MST_H1_TRANSEARTH_DAY3_2: //PTC Quads Decision to MCC-6 update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, rtcc->GETEval2(rtcc->calcParams.EI - (23.0 * 3600.0 + 20.0 * 60.0)), 140, MST_H1_TRANSEARTH_DAY3_3);
		break;
	case MST_H1_TRANSEARTH_DAY3_3: //MCC-6 update to Entry PAD update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 212, MST_H1_TRANSEARTH_DAY3_4);
		break;
	case MST_H1_TRANSEARTH_DAY3_4: //Entry PAD update to PTC Quads Decision
		UpdateMacro(UTP_PADONLY, PT_AP11ENT, rtcc->GETEval2(rtcc->calcParams.EI - (21.0*3600.0 + 50.0*60.0)), 216, MST_H1_TRANSEARTH_DAY3_5);
		break;
	case MST_H1_TRANSEARTH_DAY3_5: //PTC Quads Decision to PTC Quads Decision
		UpdateMacro(UTP_PADONLY, PT_GENERIC, rtcc->GETEval2(rtcc->calcParams.EI - (20.0*3600.0 + 20.0*60.0)), 140, MST_H1_TRANSEARTH_DAY3_6);
		break;
	case MST_H1_TRANSEARTH_DAY3_6: //PTC Quads Decision to PTC Quads Decision
		UpdateMacro(UTP_PADONLY, PT_GENERIC, rtcc->GETEval2(rtcc->calcParams.EI - (7.0*3600.0 + 20.0*60.0)), 140, MST_H1_TRANSEARTH_DAY4_1);
		break;
	case MST_H1_TRANSEARTH_DAY4_1: //PTC Quads Decision to MCC-7 decision update
		UpdateMacro(UTP_PADONLY, PT_GENERIC, rtcc->GETEval2(rtcc->calcParams.EI - 6.0*3600.0), 140, MST_H1_TRANSEARTH_DAY4_2);
		break;
	case MST_H1_TRANSEARTH_DAY4_2: //MCC-7 decision update to MCC-7 update
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval2(rtcc->calcParams.EI - 5.0*3600.0), 213, MST_H1_TRANSEARTH_DAY4_3);
		break;
	case MST_H1_TRANSEARTH_DAY4_3: //MCC-7 update to Entry PAD update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 214, MST_H1_TRANSEARTH_DAY4_4);
		break;
	case MST_H1_TRANSEARTH_DAY4_4: //Entry PAD update to final entry update
		UpdateMacro(UTP_PADONLY, PT_AP11ENT, rtcc->GETEval2(rtcc->calcParams.EI - 45.0*60.0), 217, MST_H1_TRANSEARTH_DAY4_5);
		break;
	case MST_H1_TRANSEARTH_DAY4_5: //Final entry update to CM/SM separation
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11ENT, cm->GetStage() == CM_STAGE, 218, MST_ENTRY);
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
	case MST_H1_ABORT:
		if (AbortMode == 6)	//Translunar Coast
		{
			switch (SubState) {
			case 0:
			{
				if (rtcc->GETEval2(OrbMech::HHMMSSToSS(60, 0, 0)))
				{
					setSubState(13);//Flyby
				}
				else
				{
					if (rtcc->GETEval2(OrbMech::HHMMSSToSS(45, 0, 0))) //LO+60
					{
						rtcc->calcParams.TEI = OrbMech::HHMMSSToSS(60, 0, 0);
					}
					else if (rtcc->GETEval2(OrbMech::HHMMSSToSS(35, 0, 0))) //LO+45
					{
						rtcc->calcParams.TEI = OrbMech::HHMMSSToSS(45, 0, 0);
					}
					else if (rtcc->GETEval2(OrbMech::HHMMSSToSS(25, 0, 0))) //LO+35
					{
						rtcc->calcParams.TEI = OrbMech::HHMMSSToSS(35, 0, 0);
					}
					else if (rtcc->GETEval2(OrbMech::HHMMSSToSS(15, 0, 0))) //LO+25
					{
						rtcc->calcParams.TEI = OrbMech::HHMMSSToSS(25, 0, 0);
					}
					else if (rtcc->GETEval2(OrbMech::HHMMSSToSS(8, 0, 0))) //LO+15
					{
						rtcc->calcParams.TEI = OrbMech::HHMMSSToSS(15, 0, 0);
					}
					else if (rtcc->GETEval2(rtcc->calcParams.TLI + 90.0*60.0)) //LO+8
					{
						rtcc->calcParams.TEI = OrbMech::HHMMSSToSS(8, 0, 0);
					}
					else //TLI+90
					{
						rtcc->calcParams.TEI = rtcc->calcParams.TLI + 90.0*60.0;
					}

					setSubState(1);
				}
			}
			break;
			case 1:
			{
				if (rtcc->GETEval2(rtcc->calcParams.TEI + 10.0*60.0))
				{
					startSubthread(205, UTP_NONE); //Evaluate EI conditions
					setSubState(2);
				}
			}
			break;
			case 2:
				if (rtcc->calcParams.TEI > rtcc->calcParams.EI - 12.0 * 60 * 60)
				{
					setSubState(3);//Skip directly to normal entry procedures
				}
				else
				{
					setSubState(4);	//Include another course correction
				}
				break;
			case 3:
			{
				if (rtcc->GETEval2(rtcc->calcParams.EI - 4.0*3600.0 - 35.0*60.0))
				{
					SlowIfDesired();
					setState(MST_H1_TRANSEARTH_DAY4_3);
				}
			}
			break;
			case 4:
			{
				if (rtcc->GETEval2(rtcc->calcParams.TEI + 4.0 * 60 * 60))
				{
					SlowIfDesired();
					setSubState(5);
				}
			}
			break;

			case 5:
				allocPad(8); // Allocate AP11 Maneuver Pad
				if (padForm != NULL) {
					// If success
					startSubthread(300, UTP_PADWITHCMCUPLINK); // Start subthread to fill PAD
				}
				else {
					// ERROR STATE
				}
				setSubState(6);
				// FALL INTO
			case 6: // Await pad read-up time (however long it took to compute it and give it to capcom)
				if (SubStateTime > 1 && padState > -1) {
					if (scrubbed)
					{
						if (upMessage[0] != 0)
						{
							addMessage(upMessage);
						}
						freePad();
						scrubbed = false;
						setSubState(11);
					}
					else
					{

						addMessage("You can has PAD");
						if (padAutoShow == true && padState == 0) { drawPad(); }
						// Completed. We really should test for P00 and proceed since that would be visible to the ground.
						addMessage("Ready for uplink?");
						sprintf(PCOption_Text, "Ready for uplink");
						PCOption_Enabled = true;
						setSubState(7);
					}
				}
				break;
			case 7: // Awaiting user response
			case 8: // Negative response / not ready for uplink
				break;
			case 9: // Ready for uplink
				if (SubStateTime > 1 && padState > -1) {
					// The uplink should also be ready, so flush the uplink buffer to the CMC
					this->CM_uplink_buffer();
					// uplink_size = 0; // Reset
					PCOption_Enabled = false; // No longer needed
					if (upDescr[0] != 0)
					{
						addMessage(upDescr);
					}
					setSubState(10);
				}
				break;
			case 10: // Await uplink completion
				if (cm->pcm.mcc_size == 0) {
					addMessage("Uplink completed!");
					NCOption_Enabled = true;
					sprintf(NCOption_Text, "Repeat uplink");
					setSubState(11);
				}
				break;
			case 11: // Await burn
				if (rtcc->GETEval2(rtcc->calcParams.EI - 4.0*3600.0 - 35.0*60.0))
				{
					SlowIfDesired();
					setState(MST_H1_TRANSEARTH_DAY4_3);
				}
				break;
			case 12: //Repeat uplink
			{
				NCOption_Enabled = false;
				setSubState(5);
			}
			break;
			case 13:
			{
				//Wait until LOI + 2.5 hours, then calculate Pericynthion time and EI conditions for return trajectory
				if (rtcc->GETEval2(rtcc->calcParams.LOI + 2.5*3600.0))
				{
					EphemerisData sv = rtcc->StateVectorCalcEphem(rtcc->calcParams.src);
					double dt = OrbMech::timetoperi(sv.R, sv.V, OrbMech::mu_Moon);
					rtcc->calcParams.LOI = rtcc->GETfromGMT(sv.GMT + dt);

					startSubthread(205, UTP_NONE);
					setSubState(14);
				}
			}
			break;
			case 14: //Flyby, go to nominal TEC procedures
			{
				if (rtcc->calcParams.EI - rtcc->calcParams.LOI > 45.0*3600.0) //Is this a flyby or fast PC+2?
				{
					rtcc->calcParams.TEI = rtcc->calcParams.LOI;
					setState(MST_H1_TRANSEARTH_DAY1_1);
				}
				else
				{
					rtcc->calcParams.TEI = rtcc->calcParams.EI - 30.0*3600.0;
					setSubState(4); //Go to generic MCC. Not enough time for the full transearth timeline.
				}
			}
			break;
			}
		}
		else if (AbortMode == 7) //Lunar Orbit
		{
			switch (SubState) {
			case 0:
			{
				if (MoonRevTime > 900.0)
				{
					setSubState(1);
				}
			}
			break;
			case 1:
			{
				EphemerisData sv;
				OELEMENTS coe;

				sv = rtcc->StateVectorCalcEphem(rtcc->calcParams.src);

				coe = OrbMech::coe_from_sv(sv.R, sv.V, OrbMech::mu_Moon);

				if (coe.e > 0.7)
				{
					double dt = OrbMech::timetoperi(sv.R, sv.V, OrbMech::mu_Moon);
					rtcc->calcParams.TEI = rtcc->GETfromGMT(sv.GMT + dt);

					setState(MST_H1_TRANSEARTH_DAY1_1);
				}
				else
				{
					setSubState(2);
				}
			}
			break;
			case 2:
			{
				if (MoonRevTime < 100.0)
				{
					setSubState(0);
				}
			}
			break;
			}
		}
		else if (AbortMode == 8)
		{
			//How to Abort?
		}
	}
}