#include "Orbitersdk.h"
#include "ApolloRTCCMFD.h"
#include "iu.h"

COLORREF white_bright_col = RGB(240, 240, 240);
COLORREF white_darker_col = RGB(180, 180, 180);

char Buffer2[128];

// Repaint the MFD
bool ApolloRTCCMFD::Update(oapi::Sketchpad *skp)
{
	//No title for actual MOCR displays
	if (screen == 0)
	{
		Title(skp, "Apollo RTCC MFD");
	}
	skp->SetTextColor(GetDefaultColour(2)); //White
	skp->SetFont(font_menu);  //Lucida Console
	GetCharSize(skp, CW, CH);
	ResetMOCRDisplayCentered();

	double secs;
	int mm, hh;

	// Draws the MFD title

	// Add MFD display routines here.
	// Use the device context (hDC) for Windows GDI paint functions.

	switch (screen)
	{
	case 0:
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "Maneuver Targeting", 18);
		skp->Text(CW, 4 * H / 14, "Pre-Advisory Data", 17);
		skp->Text(CW, 6 * H / 14, "Utility", 7);
		skp->Text(CW, 8 * H / 14, "MPT Initialization", 18);
		skp->Text(CW, 10 * H / 14, "Mission Plan Table", 18);
		skp->Text(CW, 12 * H / 14, "Configuration", 13);

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		skp->Text(W - CW, 2 * H / 14, "Uplinks", 7);
		skp->Text(W - CW, 10 * H / 14, "MCC Displays", 12);
		break;
	case 1:
		if (subscreen == 0)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text(W / 2, CH / 2, "TI Multiple Solution (K30)", 26);
			skp->SetTextAlign(oapi::Sketchpad::LEFT);

			x = 1;  y = 3; dx = 7;
			Text(skp, x, marker + y, "*");
			x++;
			Text(skp, x, y, "VEH:");
			if (GC->rtcc->med_k30.Vehicle == 1) Text(skp, x + dx, y, "CSM");
			else Text(skp, x + dx, y, "LEM");
			y++;
			Text(skp, x, y, "IV:");
			if (GC->rtcc->med_k30.IVFlag == 0) Text(skp, x + dx, y, "0: Both Fixed");
			else if (GC->rtcc->med_k30.IVFlag == 1) Text(skp, x + dx, y, "1: First Fixed");
			else Text(skp, x + dx, y, "2: Second Fixed");
			y++;
			Text(skp, x, y, "CVT:");
			if (GC->MissionPlanningActive)
			{
				if (GC->rtcc->med_k30.ChaserVectorTime > 0) Text_GET_HHHMMSSCS(skp, x + dx, y, GC->rtcc->med_k30.ChaserVectorTime);
				else Text(skp, x + dx, y, "Present Time");
				y++;
			}
			else
			{
				if (GC->rtcc->med_k30.Vehicle == 1) PrintCSMVessel(Buffer);
				else PrintLMVessel(Buffer);
				Text(skp, x + dx, y, Buffer); y++;
			}
			Text(skp, x, y, "TVT:");
			if (GC->MissionPlanningActive)
			{
				if (GC->rtcc->med_k30.TargetVectorTime > 0) Text_GET_HHHMMSSCS(skp, x + dx, y, GC->rtcc->med_k30.TargetVectorTime);
				else Text(skp, x + dx, y, "Present Time");
				y++;
			}
			else
			{
				if (GC->rtcc->med_k30.Vehicle == 1) PrintLMVessel(Buffer);
				else PrintCSMVessel(Buffer);
				Text(skp, x + dx, y, Buffer); y++;
			}
			Text(skp, x, y, "T1:");
			if (GC->rtcc->med_k30.StartTime >= 0) Text_GET_HHHMMSSCS(skp, x + dx, y, GC->rtcc->med_k30.StartTime);
			else Text(skp, x + dx, y, "E = %.2f°", GC->rtcc->GZGENCSN.TIElevationAngle*DEG);
			y++;
			Text(skp, x, y, "T2:");
			if (GC->rtcc->med_k30.EndTime >= 0) Text_GET_HHHMMSSCS(skp, x + dx, y, GC->rtcc->med_k30.EndTime);
			else Text(skp, x + dx, y, "WT = %.2f°", GC->rtcc->GZGENCSN.TITravelAngle*DEG);
			if (GC->rtcc->med_k30.IVFlag != 0)
			{
				y++;
				Text(skp, x, y, "INC:");
				Text(skp, x + dx, y, "%.0lf s", GC->rtcc->med_k30.TimeStep); y++;
				Text(skp, x, y, "RAN:");
				Text(skp, x + dx, y, "%.0lf s", GC->rtcc->med_k30.TimeRange);
			}
			Text(skp, 1, 20, "OFFSETS:");
			Text(skp, 1, 21, "DEL H");
			Text(skp, 1, 22, "PHASE");
			Text(skp, 1, 23, "ELEV");
			Text(skp, 1, 24, "WT");
			Text(skp, 7, 21, "%.3f NM", GC->rtcc->GZGENCSN.TIDeltaH / 1852.0);
			Text(skp, 7, 22, "%.3f deg", GC->rtcc->GZGENCSN.TIPhaseAngle*DEG);
			Text(skp, 7, 23, "%.3f deg", GC->rtcc->GZGENCSN.TIElevationAngle*DEG);
			Text(skp, 7, 24, "%.3f deg", GC->rtcc->GZGENCSN.TITravelAngle*DEG);
		}
		else
		{
			SetMOCRFont(skp, 3, false);
			GetCharSize(skp, CW, CH);
			SetMOCRDisplayCentered(3);
			Text(skp, 14, 0, "TWO IMPULSE MULTIPLE SOLUTION");
			Text(skp, 51, 0, "0063");
			Text(skp, 1, 4, "LM STA ID");
			Text(skp, 1, 5, "LM GETTHS");
			Text(skp, 1, 6, "MAN VEH");
			Text(skp, 1, 7, "WT");
			Text(skp, 1, 8, "GET");
			Text(skp, 1, 9, "GMT");
			Text(skp, 34, 4, "CSM STA ID");
			Text(skp, 34, 5, "CSM GETTHS");
			Text(skp, 34, 6, "PHASE");
			Text(skp, 34, 7, "DEL H");
			Text(skp, 34, 8, "OPTION");
			Text(skp, 1, 11, "DEL V1   YAW  PITCH");
			Text(skp, 23, 11, "GET");
			Text(skp, 31, 11, "DEL V2");
			if (GC->rtcc->TwoImpMultDispBuffer.showTPI)
			{
				Text(skp, 42, 11, "TTPI");
			}
			else
			{
				Text(skp, 40, 11, "YAW");
				Text(skp, 45, 11, "PITCH");
			}
			Text(skp, 51, 11, "L");
			Text(skp, 55, 11, "C");
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			SetMOCRFont(skp, 3, true);
			Text(skp, 45, 26, GC->rtcc->TwoImpMultDispBuffer.ErrorMessage);
			Text(skp, 5, 8, std::string(1, GC->rtcc->TwoImpMultDispBuffer.GETFRZ));
			Text(skp, 5, 9, std::string(1, GC->rtcc->TwoImpMultDispBuffer.GMTFRZ));
			Text(skp, 27, 11, std::string(1, GC->rtcc->TwoImpMultDispBuffer.GETVAR));
			Text_GET_HHHMMSSC(skp, 22, 5, GC->rtcc->TwoImpMultDispBuffer.GETTH_LM);
			Text(skp, 22, 6, GC->rtcc->TwoImpMultDispBuffer.MAN_VEH);
			Text(skp, 22, 7, "%.3lf", GC->rtcc->TwoImpMultDispBuffer.WT);
			Text_GET_HHHMMSSC(skp, 22, 8, GC->rtcc->TwoImpMultDispBuffer.GET1);
			Text_GET_HHHMMSSC(skp, 22, 9, GC->rtcc->TwoImpMultDispBuffer.GMT1);
			Text_GET_HHHMMSSC(skp, 56, 5, GC->rtcc->TwoImpMultDispBuffer.GETTH_CSM);
			Text(skp, 56, 6, "%.4lf", GC->rtcc->TwoImpMultDispBuffer.PHASE);
			Text(skp, 56, 7, "%.2lf", GC->rtcc->TwoImpMultDispBuffer.DH);
			Text(skp, 56, 8, GC->rtcc->TwoImpMultDispBuffer.OPTION);
			Text(skp, 56, 9, GC->rtcc->TwoImpMultDispBuffer.MinutesUntil);

			for (int i = 0; i < GC->rtcc->TwoImpMultDispBuffer.Solutions; i++)
			{
				sprintf(Buffer, "%.1lf", GC->rtcc->TwoImpMultDispBuffer.data[i].DELV1);
				Text(skp, 7, 12 + i, Buffer);
				sprintf(Buffer, "%.1lf", GC->rtcc->TwoImpMultDispBuffer.data[i].YAW1);
				Text(skp, 14, 12 + i, Buffer);
				sprintf(Buffer, "%.1lf", GC->rtcc->TwoImpMultDispBuffer.data[i].PITCH1);
				Text(skp, 20, 12 + i, Buffer);
				GET_Display(Buffer, GC->rtcc->TwoImpMultDispBuffer.data[i].Time2, false);
				Text(skp, 30, 12 + i, Buffer);
				sprintf(Buffer, "%.1lf", GC->rtcc->TwoImpMultDispBuffer.data[i].DELV2);
				Text(skp, 37, 12 + i, Buffer);
				if (GC->rtcc->TwoImpMultDispBuffer.showTPI)
				{
					GET_Display(Buffer, GC->rtcc->TwoImpMultDispBuffer.data[i].T_TPI, false);
					Text(skp, 49, 12 + i, Buffer);
				}
				else
				{
					sprintf(Buffer, "%.1lf", GC->rtcc->TwoImpMultDispBuffer.data[i].YAW2);
					Text(skp, 44, 12 + i, Buffer);
					sprintf(Buffer, "%.1lf", GC->rtcc->TwoImpMultDispBuffer.data[i].PITCH2);
					Text(skp, 50, 12 + i, Buffer);
				}

				sprintf(Buffer, "%c", GC->rtcc->TwoImpMultDispBuffer.data[i].L);
				Text(skp, 52, 12 + i, Buffer);
				sprintf(Buffer, "%d", GC->rtcc->TwoImpMultDispBuffer.data[i].C);
				Text(skp, 56, 12 + i, Buffer);
			}
		}
		break;
	case 2:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Thrust and CG Tables", 20);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "Table:", 10);
		if (subscreen < 3)
		{
			//SPS, APS, DPS
			RTCCSystemParameters::ThrustTable *tab;
			if (subscreen == 0)
			{
				tab = &GC->rtcc->SystemParameters.MHTSTC; //SPS
				if (subsubscreen == 0) skp->Text(CW * 8, 2 * H / 14, "SPS (Page 1)", 12);
				else skp->Text(CW * 8, 2 * H / 14, "SPS (Page 2)", 12);
			}
			else if (subscreen == 1)
			{
				tab = &GC->rtcc->SystemParameters.MHTATC; //APS
				if (subsubscreen == 0) skp->Text(CW * 8, 2 * H / 14, "APS (Page 1)", 12);
				else skp->Text(CW * 8, 2 * H / 14, "APS (Page 2)", 12);
			}
			else
			{
				tab = &GC->rtcc->SystemParameters.MHTDTC; //DPS
				if (subsubscreen == 0) skp->Text(CW * 8, 2 * H / 14, "DPS (Page 1)", 12);
				else skp->Text(CW * 8, 2 * H / 14, "DPS (Page 2)", 12);
			}
			skp->Text(CW * 2, 6 * H / 28, "N  Weight Thrust  WLR", 21);
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			int j;
			if (subsubscreen == 1) j = 20;
			else j = 0;
			for (int i = 0; i < 20; i++)
			{
				if (i + j + 1 > tab->N) break;
				Text_Int(skp, CW * 4, (7 + i)*H / 28, "%02d", j + i + 1);
				Text_Double(skp, CW * 10, (7 + i) * H / 28, "%.0lf", tab->Weight[j + i] * LBS * 1000.0);
				Text_Double(skp, CW * 17, (7 + i) * H / 28, "%.0lf", tab->Thrust[j + i].x * LBF);
				Text_Double(skp, CW * 25, (7 + i) * H / 28, "%.3lf", tab->Thrust[j + i].y * LBS * 1000.0);
			}
		}
		else
		{
			//CSM, full LM, ascent stage
			RTCCSystemParameters::CGTable *tab;
			if (subscreen == 3)
			{
				tab = &GC->rtcc->SystemParameters.MHVCCG; //CSM
				if (subsubscreen == 0) skp->Text(CW * 8, 2 * H / 14, "CSM (Page 1)", 12);
				else skp->Text(CW * 8, 2 * H / 14, "CSM (Page 2)", 12);
			}
			else if (subscreen == 4)
			{
				tab = &GC->rtcc->SystemParameters.MHVLCG; //LM ascent + descent
				if (subsubscreen == 0) skp->Text(CW * 8, 2 * H / 14, "Full LM (Page 1)", 16);
				else skp->Text(CW * 8, 2 * H / 14, "Full LM (Page 2)", 16);
			}
			else
			{
				tab = &GC->rtcc->SystemParameters.MHVACG; //LM ascent
				if (subsubscreen == 0) skp->Text(CW * 8, 2 * H / 14, "Ascent Stage (Page 1)", 21);
				else skp->Text(CW * 8, 2 * H / 14, "Ascent Stage (Page 2)", 21);
			}
			skp->Text(CW * 2, 6 * H / 28, "N  Weight     X        Y        Z", 33);
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			int j;
			if (subsubscreen == 1) j = 20;
			else j = 0;
			for (int i = 0; i < 20; i++)
			{
				if (i + j + 1 > tab->N) break;
				Text_Int(skp, CW * 4, (7 + i)*H / 28, "%02d", j + i + 1);
				Text_Double(skp, CW * 10, (7 + i) * H / 28, "%.0lf", tab->Weight[j + i] * LBS * 1000.0);
				Text_Double(skp, CW * 20, (7 + i) * H / 28, "%.3lf", tab->CG[j + i].x / 0.0254);
				Text_Double(skp, CW * 29, (7 + i) * H / 28, "%.3lf", tab->CG[j + i].y / 0.0254);
				Text_Double(skp, CW * 38, (7 + i) * H / 28, "%.3lf", tab->CG[j + i].z / 0.0254);
			}
		}
		break;
	case 3:
		if (subscreen == 0)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text(W / 2, CH / 2, "Coelliptic", 10);
			skp->SetTextAlign(oapi::Sketchpad::LEFT);

			skp->Text(CW, 2 * H / 14, "SPQ Initialization", 18);

			if (GC->rtcc->med_k01.ChaserVehicle == 1)
			{
				skp->Text(CW, 4 * H / 14, "Chaser: CSM", 11);
				skp->Text(CW, 5 * H / 14, "Target: LEM", 11);
			}
			else
			{
				skp->Text(CW, 4 * H / 14, "Chaser: LEM", 11);
				skp->Text(CW, 5 * H / 14, "Target: CSM", 11);
			}

			if (GC->MissionPlanningActive)
			{
				if (GC->rtcc->med_k01.ChaserThresholdGET < 0)
				{
					sprintf_s(Buffer, "Present Time");
				}
				else
				{
					GET_Display(Buffer, GC->rtcc->med_k01.ChaserThresholdGET);
				}
			}
			else
			{
				if (GC->rtcc->med_k01.ChaserVehicle == 1)
				{
					PrintCSMVessel(Buffer);
				}
				else
				{
					PrintLMVessel(Buffer);
				}
			}
			skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));

			if (GC->MissionPlanningActive)
			{
				if (GC->rtcc->med_k01.TargetThresholdGET < 0)
				{
					sprintf_s(Buffer, "Present Time");
				}
				else
				{
					GET_Display(Buffer, GC->rtcc->med_k01.TargetThresholdGET);
				}
			}
			else
			{
				if (GC->rtcc->med_k01.ChaserVehicle == 1)
				{
					PrintLMVessel(Buffer);
				}
				else
				{
					PrintCSMVessel(Buffer);
				}
			}
			skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));

			if (G->SPQMode != 1)
			{
				if (G->SPQMode == 2)
				{
					skp->Text(CW, 10 * H / 14, "Optimum CSI", 11);
				}
				else
				{
					skp->Text(CW, 10 * H / 14, "CSI", 3);

					if (G->CDHtimemode == 0)
					{
						skp->Text(CW, 12 * H / 14, "Fixed TPI time", 14);
					}
					else if (G->CDHtimemode == 1)
					{
						skp->Text(CW, 12 * H / 14, "Fixed DH", 8);
					}
				}
			}
			else
			{
				skp->Text(CW, 10 * H / 14, "CDH", 3);

				if (G->CDHtimemode == 0)
				{
					skp->Text(CW, 12 * H / 14, "Fixed", 5);
				}
				else if (G->CDHtimemode == 1)
				{
					skp->Text(CW, 12 * H / 14, "Find GETI", 9);
				}
			}

			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			if (G->SPQMode != 1)
			{
				GET_Display(Buffer, GC->rtcc->med_k01.t_CSI);
				skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));
			}
			else
			{
				GET_Display(Buffer, G->CDHtime);
				skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));
			}
		}
		else
		{
			RendezvousEvaluationDisplay(skp);
		}
		break;
	case 4:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "General Purpose Maneuver", 24);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(CW / 2, CH * (marker + 3), "*", 1);
		skp->Text(CW * 2, CH * 2, "Code:", 5);
		GMPManeuverCodeName(Buffer, G->GMPManeuverCode);
		skp->Text(CW * 8, CH * 2, Buffer, strlen(Buffer));
		skp->Text(CW * 2, CH * 3, "VEH", 3);
		if (GC->rtcc->med_k20.Vehicle == RTCC_MPT_CSM)
		{
			skp->Text(CW * 6, CH * 3, "CSM", 3);
		}
		else
		{
			skp->Text(CW * 6, CH * 3, "LM", 2);
		}
		if (GC->MissionPlanningActive == false)
		{
			if (GC->rtcc->med_k20.Vehicle == RTCC_MPT_CSM)
			{
				PrintCSMVessel(Buffer, false);
			}
			else
			{
				PrintLMVessel(Buffer, false);
			}
			skp->Text(CW * 10, CH * 3, Buffer, strlen(Buffer));
		}
		skp->Text(CW * 2, CH * 4, "TYP", 3);
		GMPManeuverTypeName(Buffer, G->GMPManeuverType);
		skp->Text(CW * 6, CH * 4, Buffer, strlen(Buffer));
		skp->Text(CW * 2, CH * 5, "PNT", 3);
		GMPManeuverPointName(Buffer, G->GMPManeuverPoint);
		skp->Text(CW * 6, CH * 5, Buffer, strlen(Buffer));
		skp->Text(CW * 2, CH * 6, "GET", 3);
		GET_Display(Buffer, G->SPSGET, false);
		skp->Text(CW * 6, CH * 6, Buffer, strlen(Buffer));

		//Desired Maneuver Height
		if (G->GMPManeuverCode == RTCC_GMP_CRH || G->GMPManeuverCode == RTCC_GMP_HBH || G->GMPManeuverCode == RTCC_GMP_FCH || G->GMPManeuverCode == RTCC_GMP_CPH ||
			G->GMPManeuverCode == RTCC_GMP_CNH || G->GMPManeuverCode == RTCC_GMP_PCH || G->GMPManeuverCode == RTCC_GMP_NSH || G->GMPManeuverCode == RTCC_GMP_HOH)
		{
			skp->Text(CW * 2, CH * 7, "ALT", 3);
			sprintf(Buffer, "%.2f NM", G->GMPManeuverHeight / 1852.0);
			skp->Text(CW * 6, CH * 7, Buffer, strlen(Buffer));
		}
		//Desired Maneuver Longitude
		else if (G->GMPManeuverCode == RTCC_GMP_PCL || G->GMPManeuverCode == RTCC_GMP_CRL || G->GMPManeuverCode == RTCC_GMP_HOL || G->GMPManeuverCode == RTCC_GMP_NSL ||
			G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_NHL || G->GMPManeuverCode == RTCC_GMP_SAL || G->GMPManeuverCode == RTCC_GMP_PHL ||
			G->GMPManeuverCode == RTCC_GMP_CPL || G->GMPManeuverCode == RTCC_GMP_HBL || G->GMPManeuverCode == RTCC_GMP_CNL || G->GMPManeuverCode == RTCC_GMP_HNL ||
			G->GMPManeuverCode == RTCC_GMP_SAA || G->GMPManeuverCode == RTCC_GMP_HAS)
		{
			skp->Text(CW * 2, CH * 7, "LNG", 3);
			sprintf(Buffer, "%.2f°", G->GMPManeuverLongitude*DEG);
			skp->Text(CW * 6, CH * 7, Buffer, strlen(Buffer));
		}

		//Height Change
		if (G->GMPManeuverCode == RTCC_GMP_HOL || G->GMPManeuverCode == RTCC_GMP_HOT || G->GMPManeuverCode == RTCC_GMP_HAO || G->GMPManeuverCode == RTCC_GMP_HPO ||
			G->GMPManeuverCode == RTCC_GMP_HNL || G->GMPManeuverCode == RTCC_GMP_HNT || G->GMPManeuverCode == RTCC_GMP_HNA || G->GMPManeuverCode == RTCC_GMP_HNP ||
			G->GMPManeuverCode == RTCC_GMP_PHL || G->GMPManeuverCode == RTCC_GMP_PHT || G->GMPManeuverCode == RTCC_GMP_PHA || G->GMPManeuverCode == RTCC_GMP_PHP || G->GMPManeuverCode == RTCC_GMP_HOH)
		{
			skp->Text(CW * 2, CH * 8, "DH", 2);
			sprintf(Buffer, "%.2f NM", G->GMPHeightChange / 1852.0);
			skp->Text(CW * 6, CH * 8, Buffer, strlen(Buffer));
		}
		//Apoapsis Height
		else if (G->GMPManeuverCode == RTCC_GMP_HBT || G->GMPManeuverCode == RTCC_GMP_HBH || G->GMPManeuverCode == RTCC_GMP_HBO || G->GMPManeuverCode == RTCC_GMP_HBL ||
			G->GMPManeuverCode == RTCC_GMP_NHT || G->GMPManeuverCode == RTCC_GMP_NHL || G->GMPManeuverCode == RTCC_GMP_HAS)
		{
			skp->Text(CW * 2, CH * 8, "ApA", 3);
			sprintf(Buffer, "%.2f NM", G->GMPApogeeHeight / 1852.0);
			skp->Text(CW * 6, CH * 8, Buffer, strlen(Buffer));
		}
		//Delta V
		else if (G->GMPManeuverCode == RTCC_GMP_FCT || G->GMPManeuverCode == RTCC_GMP_FCA || G->GMPManeuverCode == RTCC_GMP_FCP || G->GMPManeuverCode == RTCC_GMP_FCE ||
			G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_FCH)
		{
			skp->Text(CW * 2, CH * 8, "DV", 2);
			sprintf(Buffer, "%.2f ft/s", G->GMPDeltaVInput / 0.3048);
			skp->Text(CW * 6, CH * 8, Buffer, strlen(Buffer));
		}
		//Apse line rotation
		else if (G->GMPManeuverCode == RTCC_GMP_SAT || G->GMPManeuverCode == RTCC_GMP_SAO || G->GMPManeuverCode == RTCC_GMP_SAL)
		{
			skp->Text(CW * 2, CH * 8, "ROT", 4);
			sprintf(Buffer, "%.2f°", G->GMPApseLineRotAngle*DEG);
			skp->Text(CW * 6, CH * 8, Buffer, strlen(Buffer));
		}

		//Wedge Angle
		if (G->GMPManeuverCode == RTCC_GMP_PCE || G->GMPManeuverCode == RTCC_GMP_PCL || G->GMPManeuverCode == RTCC_GMP_PCT || G->GMPManeuverCode == RTCC_GMP_PHL ||
			G->GMPManeuverCode == RTCC_GMP_PHT || G->GMPManeuverCode == RTCC_GMP_PHA || G->GMPManeuverCode == RTCC_GMP_PHP || G->GMPManeuverCode == RTCC_GMP_CPL ||
			G->GMPManeuverCode == RTCC_GMP_CPH || G->GMPManeuverCode == RTCC_GMP_CPT || G->GMPManeuverCode == RTCC_GMP_CPA || G->GMPManeuverCode == RTCC_GMP_CPP ||
			G->GMPManeuverCode == RTCC_GMP_PCH)
		{
			skp->Text(CW * 2, CH * 9, "DW", 2);
			sprintf(Buffer, "%.2f°", G->GMPWedgeAngle*DEG);
			skp->Text(CW * 6, CH * 9, Buffer, strlen(Buffer));
		}
		//Node Shift
		else if (G->GMPManeuverCode == RTCC_GMP_NST || G->GMPManeuverCode == RTCC_GMP_NSO || G->GMPManeuverCode == RTCC_GMP_NSH || G->GMPManeuverCode == RTCC_GMP_NSL ||
			G->GMPManeuverCode == RTCC_GMP_CNL || G->GMPManeuverCode == RTCC_GMP_CNH || G->GMPManeuverCode == RTCC_GMP_CNT || G->GMPManeuverCode == RTCC_GMP_CNA || G->GMPManeuverCode == RTCC_GMP_CNP ||
			G->GMPManeuverCode == RTCC_GMP_HNL || G->GMPManeuverCode == RTCC_GMP_HNT || G->GMPManeuverCode == RTCC_GMP_HNA || G->GMPManeuverCode == RTCC_GMP_HNP)
		{
			skp->Text(CW * 2, CH * 9, "DLN", 3);
			sprintf(Buffer, "%.2f°", G->GMPNodeShiftAngle*DEG);
			skp->Text(CW * 6, CH * 9, Buffer, strlen(Buffer));
		}
		//Periapsis Height
		else if (G->GMPManeuverCode == RTCC_GMP_HBT || G->GMPManeuverCode == RTCC_GMP_HBH || G->GMPManeuverCode == RTCC_GMP_HBO || G->GMPManeuverCode == RTCC_GMP_HBL ||
			G->GMPManeuverCode == RTCC_GMP_NHT || G->GMPManeuverCode == RTCC_GMP_NHL || G->GMPManeuverCode == RTCC_GMP_HAS)
		{
			skp->Text(CW * 2, CH * 9, "PeA", 3);
			sprintf(Buffer, "%.2f NM", G->GMPPerigeeHeight / 1852.0);
			skp->Text(CW * 6, CH * 9, Buffer, strlen(Buffer));
		}
		//Pitch
		else if (G->GMPManeuverCode == RTCC_GMP_FCT || G->GMPManeuverCode == RTCC_GMP_FCA || G->GMPManeuverCode == RTCC_GMP_FCP || G->GMPManeuverCode == RTCC_GMP_FCE ||
			G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_FCH)
		{
			skp->Text(CW * 2, CH * 9, "P", 1);
			sprintf(Buffer, "%.2f°", G->GMPPitch*DEG);
			skp->Text(CW * 6, CH * 9, Buffer, strlen(Buffer));
		}

		//Yaw
		if (G->GMPManeuverCode == RTCC_GMP_FCT || G->GMPManeuverCode == RTCC_GMP_FCA || G->GMPManeuverCode == RTCC_GMP_FCP || G->GMPManeuverCode == RTCC_GMP_FCE ||
			G->GMPManeuverCode == RTCC_GMP_FCL || G->GMPManeuverCode == RTCC_GMP_FCH)
		{
			skp->Text(CW * 2, CH * 10, "Y", 1);
			sprintf(Buffer, "%.2f°", G->GMPYaw*DEG);
			skp->Text(CW * 6, CH * 10, Buffer, strlen(Buffer));
		}
		//Node Shift
		else if (G->GMPManeuverCode == RTCC_GMP_NHT || G->GMPManeuverCode == RTCC_GMP_NHL)
		{
			skp->Text(CW * 2, CH * 10, "DLN", 3);
			sprintf(Buffer, "%.2f°", G->GMPNodeShiftAngle*DEG);
			skp->Text(CW * 6, CH * 10, Buffer, strlen(Buffer));
		}
		//Rev counter
		else if (G->GMPManeuverCode == RTCC_GMP_HAS)
		{
			skp->Text(CW * 2, CH * 10, "N", 1);
			sprintf(Buffer, "%d", G->GMPRevs);
			skp->Text(CW * 6, CH * 10, Buffer, strlen(Buffer));
		}
		if (GC->rtcc->PZGPMDIS.Err)
		{
			sprintf(Buffer, "Error: %d", GC->rtcc->PZGPMDIS.Err);
			skp->Text(CW * 3, CH * 21, Buffer, strlen(Buffer));
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);

		skp->Text(CW * 7, CH * 13, "GET A", 5);
		skp->Text(CW * 7, CH * 14, "HA", 2);
		skp->Text(CW * 7, CH * 15, "LONG A", 6);
		skp->Text(CW * 7, CH * 16, "LAT A", 5);
		skp->Text(CW * 7, CH * 17, "GET P", 5);
		skp->Text(CW * 7, CH * 18, "HP", 2);
		skp->Text(CW * 7, CH * 19, "LONG P", 6);
		skp->Text(CW * 7, CH * 20, "LAT P", 5);
		GET_Display(Buffer, GC->rtcc->PZGPMDIS.GET_A, false);
		skp->Text(CW * 18, CH * 13, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.1f", GC->rtcc->PZGPMDIS.HA / 1852.0);
		skp->Text(CW * 18, CH * 14, Buffer, strlen(Buffer));
		FormatLongitude(Buffer, GC->rtcc->PZGPMDIS.long_A*DEG);
		skp->Text(CW * 18, CH * 15, Buffer, strlen(Buffer));
		FormatLatitude(Buffer, GC->rtcc->PZGPMDIS.lat_A*DEG);
		skp->Text(CW * 18, CH * 16, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->rtcc->PZGPMDIS.GET_P, false);
		skp->Text(CW * 18, CH * 17, Buffer, strlen(Buffer));
		if (GC->rtcc->PZGPMDIS.ShowImpact)
		{
			sprintf(Buffer, "IMPACT");
		}
		else
		{
			sprintf(Buffer, "%.1f", GC->rtcc->PZGPMDIS.HP / 1852.0);
		}
		skp->Text(CW * 18, CH * 18, Buffer, strlen(Buffer));
		FormatLongitude(Buffer, GC->rtcc->PZGPMDIS.long_P*DEG);
		skp->Text(CW * 18, CH * 19, Buffer, strlen(Buffer));
		FormatLatitude(Buffer, GC->rtcc->PZGPMDIS.lat_P*DEG);
		skp->Text(CW * 18, CH * 20, Buffer, strlen(Buffer));

		skp->Text(W - CW * 11, CH * 5, "GETI", 4);
		skp->Text(W - CW * 11, CH * 6, "DEL V MAN", 9);
		skp->Text(W - CW * 11, CH * 7, "PIT MAN", 7);
		skp->Text(W - CW * 11, CH * 8, "YAW MAN", 7);
		skp->Text(W - CW * 11, CH * 9, "H MAN", 5);
		skp->Text(W - CW * 11, CH * 10, "LONG MAN", 8);
		skp->Text(W - CW * 11, CH * 11, "LAT MAN", 7);

		skp->Text(W - CW * 11, CH * 13, "A", 1);
		skp->Text(W - CW * 11, CH * 14, "E", 1);
		skp->Text(W - CW * 11, CH * 15, "I", 1);
		skp->Text(W - CW * 11, CH * 16, "NODE AN", 7);
		skp->Text(W - CW * 11, CH * 17, "DEL G", 5);

		skp->Text(W - CW * 11, CH * 19, "VX", 3);
		skp->Text(W - CW * 11, CH * 20, "VY", 3);
		skp->Text(W - CW * 11, CH * 21, "VZ", 3);

		GET_Display(Buffer, GC->rtcc->PZGPMDIS.GET_TIG, false);
		skp->Text(W - CW, CH * 5, Buffer, strlen(Buffer));
		AGC_Display(Buffer, length(GC->rtcc->PZGPMDIS.DV) / 0.3048);
		skp->Text(W - CW, CH * 6, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+.2f", GC->rtcc->PZGPMDIS.Pitch_Man*DEG);
		skp->Text(W - CW, CH * 7, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+.2f", GC->rtcc->PZGPMDIS.Yaw_Man*DEG);
		skp->Text(W - CW, CH * 8, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.1f", GC->rtcc->PZGPMDIS.H_Man / 1852.0);
		skp->Text(W - CW, CH * 9, Buffer, strlen(Buffer));
		FormatLongitude(Buffer, GC->rtcc->PZGPMDIS.long_Man*DEG);
		skp->Text(W - CW, CH * 10, Buffer, strlen(Buffer));
		FormatLatitude(Buffer, GC->rtcc->PZGPMDIS.lat_Man*DEG);
		skp->Text(W - CW, CH * 11, Buffer, strlen(Buffer));

		sprintf(Buffer, "%.1f", GC->rtcc->PZGPMDIS.A / 1852.0);
		skp->Text(W - CW, CH * 13, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.6f", GC->rtcc->PZGPMDIS.E);
		skp->Text(W - CW, CH * 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", GC->rtcc->PZGPMDIS.I*DEG);
		skp->Text(W - CW, CH * 15, Buffer, strlen(Buffer));
		FormatLongitude(Buffer, GC->rtcc->PZGPMDIS.Node_Ang*DEG);
		skp->Text(W - CW, CH * 16, Buffer, strlen(Buffer));
		FormatLongitude(Buffer, GC->rtcc->PZGPMDIS.Del_G*DEG);
		skp->Text(W - CW, CH * 17, Buffer, strlen(Buffer));

		AGC_Display(Buffer, GC->rtcc->PZGPMDIS.DV.x / 0.3048);
		skp->Text(W - CW, CH * 19, Buffer, strlen(Buffer));
		AGC_Display(Buffer, GC->rtcc->PZGPMDIS.DV.y / 0.3048);
		skp->Text(W - CW, CH * 20, Buffer, strlen(Buffer));
		AGC_Display(Buffer, GC->rtcc->PZGPMDIS.DV.z / 0.3048);
		skp->Text(W - CW, CH * 21, Buffer, strlen(Buffer));
		break;
	case 5:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "REFSMMAT", 8);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		CSMOrLMSelection(skp);

		if (G->REFSMMATopt == 0 || G->REFSMMATopt == 1) //P30 Maneuver
		{
			if (G->REFSMMATopt == 0)
			{
				if (G->REFSMMATHeadsUp)
				{
					skp->Text(CW, 4 * H / 14, "P30 (Heads up)", 14);
				}
				else
				{
					skp->Text(CW, 4 * H / 14, "P30 (Heads down)", 16);
				}
			}
			else
			{
				skp->Text(CW, 4 * H / 14, "P30 Retro", 9);
			}

			GET_Display(Buffer, G->P30TIG);
			skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			skp->Text(W - CW, 4 * H / 14, "DV Vector", 9);
			AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
			skp->Text(W - CW, 5 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
			skp->Text(W - CW, 6 * H / 14, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
			skp->Text(W - CW, 7 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->REFSMMATopt == 2)
		{
			skp->Text(CW, 4 * H / 14, "LVLH", 4);

			GET_Display(Buffer, G->REFSMMAT_LVLH_Time);
			skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->REFSMMATopt == 3)
		{
			skp->Text(CW, 4 * H / 14, "Lunar Entry", 11);
		}
		else if (G->REFSMMATopt == 4)
		{
			skp->Text(CW, 4 * H / 14, "Launch", 6);
		}
		else if (G->REFSMMATopt == 5 || G->REFSMMATopt == 8)
		{
			if (G->REFSMMATopt == 8)
			{
				skp->Text(CW, 4 * H / 14, "LS during TLC", 13);
			}
			else
			{
				skp->Text(CW, 4 * H / 14, "Landing Site", 12);

				if (GC->MissionPlanningActive == false && IsCSM == false)
				{
					PrintCSMVessel(Buffer);
					skp->Text(CW, 3 * H / 14, Buffer, strlen(Buffer));
				}
			}

			GET_Display(Buffer, GC->rtcc->CZTDTGTU.GETTD);
			skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));

			skp->Text(W - CW*10, 7 * H / 14, "Lat:", 4);
			sprintf(Buffer, "%.3f°", GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST] * DEG);
			skp->Text(W - CW*10, 8 * H / 14, Buffer, strlen(Buffer));
			skp->Text(W - CW*10, 9 * H / 14, "Lng:", 4);
			sprintf(Buffer, "%.3f°", GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST] * DEG);
			skp->Text(W - CW*10, 10 * H / 14, Buffer, strlen(Buffer));

			if (G->REFSMMATopt == 8)
			{
				skp->Text(W - CW*10, 11 * H / 14, "Azimuth:", 8);
				sprintf(Buffer, "%.3f°", GC->rtcc->med_k18.psi_DS);
				skp->Text(W - CW*10, 12 * H / 14, Buffer, strlen(Buffer));
			}
		}
		else if (G->REFSMMATopt == 6)
		{
			skp->Text(CW, 4 * H / 14, "PTC", 3);

			skp->Text(CW, 6 * H / 14, "Average TEI:", 12);
			sprintf(Buffer, "MJD %.4lf", GC->REFSMMAT_PTC_MJD);
			skp->Text(CW, 7 * H / 14, Buffer, strlen(Buffer));
		}
		else if (G->REFSMMATopt == 7)
		{
			skp->Text(CW, 4 * H / 14, "REFS from Att", 13);

			skp->Text(CW, 9 * H / 21, "Cur REFSMMAT:", 13);
			REFSMMATName(Buffer, G->REFSMMATcur);
			skp->Text(CW, 10 * H / 21, Buffer, strlen(Buffer));

			skp->Text(CW, 12 * H / 21, "Attitude:", 9);
			sprintf(Buffer, "%+07.2f R", G->VECangles.x*DEG);
			skp->Text(CW, 13 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.2f P", G->VECangles.y*DEG);
			skp->Text(CW, 14 * H / 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.2f Y", G->VECangles.z*DEG);
			skp->Text(CW, 15 * H / 21, Buffer, strlen(Buffer));
		}

		REFSMMATData *refsdata;
		if (IsCSM)
		{
			refsdata = &GC->rtcc->EZJGMTX1.data[0];
		}
		else
		{
			refsdata = &GC->rtcc->EZJGMTX3.data[0];
		}
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		GC->rtcc->FormatREFSMMATCode(RTCC_REFSMMAT_TYPE_CUR, refsdata->ID, Buffer);
		skp->Text(CW * 18, 3 * H / 14, Buffer, strlen(Buffer));

		for (int i = 0; i < 9; i++)
		{
			sprintf(Buffer, "%+f", refsdata->REFSMMAT.data[i]);
			skp->Text(CW * 17, (4 + i) * H / 14, Buffer, strlen(Buffer));
		}
		break;
	case 6:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Entry Options", 13);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "Tradeoff", 15);
		skp->Text(CW, 4 * H / 14, "Abort Scan Table", 16);
		skp->Text(CW, 6 * H / 14, "Return to Earth Digitals", 24);
		skp->Text(CW, 8 * H / 14, "Splashdown Update", 17);
		skp->Text(CW, 10 * H / 14, "RTE Constraints", 15);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		skp->Text(W - CW, 2 * H / 14, "RTED Manual Input", 17);
		skp->Text(W - CW, 4 * H / 14, "RTED Entry Profile", 18);
		break;
	case 7:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "AGS NAVIGATION UPDATES", 28);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(W - CW*5, CH / 2, "0277", 4);

		if (IsCSM)
		{
			skp->Text(CW, 2 * H / 14, "CSM", 3);
			PrintCSMVessel(Buffer);
		}
		else
		{
			skp->Text(CW, 2 * H / 14, "LM", 2);
			PrintLMVessel(Buffer);
		}
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		skp->Text(CW, 5 * H / 14, "REFSMMAT:", 9);
		GC->rtcc->FormatREFSMMATCode(GC->rtcc->EZETVMED.AGSNavUpdREFSMMAT, GC->rtcc->EZJGMTX3.data[GC->rtcc->EZETVMED.AGSNavUpdREFSMMAT - 1].ID, Buffer);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));

		skp->Text(CW * 8, 13 * H / 14, "AGS REFSMMAT:", 13);
		GC->rtcc->FormatREFSMMATCode(RTCC_REFSMMAT_TYPE_AGS, GC->rtcc->EZJGMTX3.data[RTCC_REFSMMAT_TYPE_AGS - 1].ID, Buffer);
		skp->Text(CW * 22, 13 * H / 14, Buffer, strlen(Buffer));

		GET_Display2(Buffer, GC->rtcc->GetAGSClockZero() - GC->rtcc->GetLGCClockZero());
		skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));

		if (IsBusy(G->subThreadStatus))
		{
			skp->Text(CW, 12 * H / 14, "Calculating...", 14);
		}
		else
		{
			skp->Text(CW, 12 * H / 14, "Calculate K-Factor", 18);
		}

		sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA240);
		skp->Text(CW * 22, CH * 5, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 5, "240", 3);
		sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA241);
		skp->Text(CW * 22, CH * 6, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 6, "241", 3);
		sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA242);
		skp->Text(CW * 22, CH * 7, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 7, "242", 3);

		sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA260);
		skp->Text(CW * 22, CH * 8, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 8, "260", 3);
		sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA261);
		skp->Text(CW * 22, CH * 9, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 9, "261", 3);
		sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA262);
		skp->Text(CW * 22, CH * 10, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 10, "262", 3);

		sprintf(Buffer, "%+07.1f", G->agssvpad.DEDA254);
		skp->Text(CW * 22, CH * 11, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 11, "254", 3);

		sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA244);
		skp->Text(CW * 22, CH * 12, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 12, "244", 3);
		sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA245);
		skp->Text(CW * 22, CH * 13, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 13, "245", 3);
		sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA246);
		skp->Text(CW * 22, CH * 14, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 14, "246", 3);


		sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA264);
		skp->Text(CW * 22, CH * 15, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 15, "264", 3);
		sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA265);
		skp->Text(CW * 22, CH * 16, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 16, "265", 3);
		sprintf(Buffer, "%+06.0f", G->agssvpad.DEDA266);
		skp->Text(CW * 22, CH * 17, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 17, "266", 3);

		sprintf(Buffer, "%+07.1f", G->agssvpad.DEDA272);
		skp->Text(CW * 22, CH * 18, Buffer, strlen(Buffer));
		skp->Text(CW * 33, CH * 18, "272", 3);
		break;
	case 8:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Config", 6);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "RTCC Files", 10);

		PrintCSMVessel(Buffer);
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));

		PrintLMVessel(Buffer);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));

		if (G->vesselisdocked)
		{
			skp->Text(CW, 8 * H / 14, "Docked", 6);
		}
		else
		{
			skp->Text(CW, 8 * H / 14, "Undocked", 8);
		}

		if (G->lemdescentstage)
		{
			skp->Text(CW, 10 * H / 14, "Full LM", 7);
		}
		else
		{
			skp->Text(CW, 10 * H / 14, "Ascent Stage", 12);
		}
		skp->Text(CW, 12 * H / 14, "Thrust and CG Tables", 20);

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		sprintf(Buffer, "%02d:%02d:%04d", GC->rtcc->GZGENCSN.DayofLiftoff, GC->rtcc->GZGENCSN.MonthofLiftoff, GC->rtcc->GZGENCSN.Year);
		skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));

		GET_Display2(Buffer, GC->rtcc->GetGMTLO()*3600.0);
		skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));

		sprintf(Buffer, "AGC Epoch: %d", GC->rtcc->SystemParameters.AGCEpoch);
		skp->Text(W - CW, 6 * H / 14, Buffer, strlen(Buffer));

		skp->Text(W - CW, 8 * H / 14, "Update Liftoff Time", 19);
		break;
	case 9:
		if (G->manpadopt == 0 || G->manpadopt == 1)
		{
			skp->SetPen(pen2);
			skp->Line(CW * 31 / 2, CH * 2, CW * 31 / 2, CH * 33 / 2);
			skp->Line(0, CH * 23 / 2, CW * 31 / 2, CH * 23 / 2);
			skp->Line(0, CH * 33 / 2, CW * 31 / 2, CH * 33 / 2);

			ThrusterName(Buffer, G->manpadenginetype);
			skp->Text(CW, CH * 4, Buffer, strlen(Buffer));

			if (G->HeadsUp)
			{
				skp->Text(CW, CH * 5, "Heads Up", 8);
			}
			else
			{
				skp->Text(CW, CH * 5, "Heads Down", 10);
			}

			PrintUllage(Buffer, G->manpadenginetype, G->manpad_ullage_opt, G->manpad_ullage_dt);
			skp->Text(CW, CH * 6, Buffer, strlen(Buffer));

			GET_Display2(Buffer, G->P30TIG);
			skp->Text(CW, CH * 7, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f", G->dV_LVLH.x / 0.3048);
			skp->Text(CW, CH * 8, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->dV_LVLH.y / 0.3048);
			skp->Text(CW, CH * 9, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->dV_LVLH.z / 0.3048);
			skp->Text(CW, CH * 10, Buffer, strlen(Buffer));

			skp->Text(CW, CH * 12, "Star Check:", 11);
			sprintf(Buffer,"%.0f min", -G->sxtstardtime / 60.0);
			skp->Text(CW, CH * 13, Buffer, strlen(Buffer));

			if (G->manpadopt == 0)
			{
				skp->SetTextAlign(oapi::Sketchpad::CENTER);
				skp->Text(W / 2, CH / 2, "P30 Maneuver", 12);
				skp->SetTextAlign(oapi::Sketchpad::LEFT);

				if (GC->MissionPlanningActive == false)
				{
					if (G->vesselisdocked == false)
					{
						skp->Text(CW, CH * 3, "CSM alone", 9);
					}
					else
					{
						skp->Text(CW, CH * 3, "CSM/LM", 6);
					}
				}
				else
				{
					skp->Text(CW, CH * 3, GC->manpad.remarks, strlen(GC->manpad.remarks));
				}

				skp->Text(CW, CH * 14, "Pref. Stars:", 12);
				switch (G->manpad_pref_GDC_stars)
				{
				case 0: sprintf(Buffer, "Deneb, Vega"); break;
				case 1: sprintf(Buffer, "Navi, Polaris"); break;
				case 2: sprintf(Buffer, "Acrux, Atria"); break;
				case 3: sprintf(Buffer, "Sirius, Rigel"); break;
				}
				skp->Text(CW, CH * 15, Buffer, strlen(Buffer));

				if (GC->MissionPlanningActive || G->vesselisdocked)
				{
					skp->Text(CW, CH * 17, "LM Weight:", 10);
					sprintf(Buffer, "%.0lf", GC->manpad.LMWeight);
					skp->Text(CW, CH * 18, Buffer, strlen(Buffer));
				}

				skp->Text(CW, CH * 20, "Set Stars:", 10);
				skp->Text(CW, CH * 21, GC->manpad.SetStars, strlen(GC->manpad.SetStars));

				sprintf(Buffer, "R %03.0f", OrbMech::round(GC->manpad.GDCangles.x));
				skp->Text(CW, CH * 22, Buffer, strlen(Buffer));
				sprintf(Buffer, "P %03.0f", OrbMech::round(GC->manpad.GDCangles.y));
				skp->Text(CW, CH * 23, Buffer, strlen(Buffer));
				sprintf(Buffer, "Y %03.0f", OrbMech::round(GC->manpad.GDCangles.z));
				skp->Text(CW, CH * 24, Buffer, strlen(Buffer));

				skp->Text(CW * 35, CH * 2, "N47", 3);
				skp->Text(CW * 35, CH * 3, "N48", 3);
				skp->Text(CW * 35, CH * 7, "N33", 3);
				skp->Text(CW * 35, CH * 8, "N81", 3);
				skp->Text(CW * 35, CH * 14, "N44", 3);

				sprintf(Buffer, "%+06.0f WGT", GC->manpad.Weight);
				skp->Text(CW * 17, CH * 2, Buffer, strlen(Buffer));
				if (strncmp(GC->manpad.PropGuid, "SPS", 3) == 0)
				{
					sprintf(Buffer, "%+07.2f PTRIM", GC->manpad.pTrim);
					skp->Text(CW * 17, CH * 3, Buffer, strlen(Buffer));
					sprintf(Buffer, "%+07.2f YTRIM", GC->manpad.yTrim);
					skp->Text(CW * 17, CH * 4, Buffer, strlen(Buffer));
				}
				else
				{
					skp->Text(CW * 17, CH * 3, "N/A     PTRIM", 13);
					skp->Text(CW * 17, CH * 4, "N/A     YTRIM", 13);
				}

				OrbMech::SStoHHMMSS(GC->manpad.GETI, hh, mm, secs, 0.01);

				sprintf(Buffer, "%+06d HRS GETI", hh);
				skp->Text(CW * 17, CH * 5, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+06d MIN", mm);
				skp->Text(CW * 17, CH * 6, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.2f SEC", secs);
				skp->Text(CW * 17, CH * 7, Buffer, strlen(Buffer));

				sprintf(Buffer, "%+07.1f DVX", GC->manpad.dV.x);
				skp->Text(CW * 17, CH * 8, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f DVY", GC->manpad.dV.y);
				skp->Text(CW * 17, CH * 9, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f DVZ", GC->manpad.dV.z);
				skp->Text(CW * 17, CH * 10, Buffer, strlen(Buffer));

				sprintf(Buffer, "XXX%03.0f R", GC->manpad.Att.x);
				skp->Text(CW * 17, CH * 11, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f P", GC->manpad.Att.y);
				skp->Text(CW * 17, CH * 12, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f Y", GC->manpad.Att.z);
				skp->Text(CW * 17, CH * 13, Buffer, strlen(Buffer));

				sprintf(Buffer, "%+07.1f HA", min(9999.9, GC->manpad.HA));
				skp->Text(CW * 17, CH * 14, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f HP", GC->manpad.HP);
				skp->Text(CW * 17, CH * 15, Buffer, strlen(Buffer));

				sprintf(Buffer, "%+07.1f VT", GC->manpad.Vt);
				skp->Text(CW * 17, CH * 16, Buffer, strlen(Buffer));

				OrbMech::SStoMMSS(GC->manpad.burntime, mm, secs);

				sprintf(Buffer, "XXX%d:%02.0f BT (MIN:SEC)", mm, secs);
				skp->Text(CW * 17, CH * 17, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f VC", GC->manpad.Vc);
				skp->Text(CW * 17, CH * 18, Buffer, strlen(Buffer));

				if (GC->manpad.Star == 0)
				{
					skp->Text(CW * 17, CH * 19, "N/A     SXTS", 12);
					skp->Text(CW * 17, CH * 20, "N/A     SFT", 11);
					skp->Text(CW * 17, CH * 21, "N/A     TRN", 11);
				}
				else
				{
					sprintf(Buffer, "XXXX%02d SXTS", GC->manpad.Star);
					skp->Text(CW * 17, CH * 19, Buffer, strlen(Buffer));
					sprintf(Buffer, "%+07.2f SFT", GC->manpad.Shaft);
					skp->Text(CW * 17, CH * 20, Buffer, strlen(Buffer));
					sprintf(Buffer, "%+07.3f TRN", GC->manpad.Trun);
					skp->Text(CW * 17, CH * 21, Buffer, strlen(Buffer));
				}
				if (GC->manpad.BSSStar == 0)
				{
					skp->Text(CW * 17, CH * 22, "N/A     BSS", 11);
					skp->Text(CW * 17, CH * 23, "N/A     SPA", 11);
					skp->Text(CW * 17, CH * 24, "N/A     SXP", 11);
				}
				else
				{
					sprintf(Buffer, "XXXX%02d BSS", GC->manpad.BSSStar);
					skp->Text(CW * 17, CH * 22, Buffer, strlen(Buffer));
					sprintf(Buffer, "%+07.2f SPA", GC->manpad.SPA);
					skp->Text(CW * 17, CH * 23, Buffer, strlen(Buffer));
					sprintf(Buffer, "%+07.3f SXP", GC->manpad.SXP);
					skp->Text(CW * 17, CH * 24, Buffer, strlen(Buffer));
				}
			}
			else
			{
				skp->SetTextAlign(oapi::Sketchpad::CENTER);
				skp->Text(W / 2, CH / 2, "P30 LM Maneuver", 15);
				skp->SetTextAlign(oapi::Sketchpad::LEFT);

				if (GC->MissionPlanningActive == false)
				{
					if (G->vesselisdocked == false)
					{
						skp->Text(CW, CH * 3, "LM alone", 9);
					}
					else
					{
						skp->Text(CW, CH * 3, "LM/CSM", 6);
					}
				}
				else
				{
					skp->Text(CW, CH * 3, GC->lmmanpad.remarks, strlen(GC->lmmanpad.remarks));
				}

				skp->Text(CW, CH * 17, "LM Weight:", 10);
				sprintf(Buffer, "%.0lf", GC->lmmanpad.LMWeight);
				skp->Text(CW, CH * 18, Buffer, strlen(Buffer));
				if (GC->MissionPlanningActive || G->vesselisdocked)
				{
					skp->Text(CW, CH * 19, "CSM Weight:", 11);
					sprintf(Buffer, "%.0lf", GC->lmmanpad.CSMWeight);
					skp->Text(CW, CH * 20, Buffer, strlen(Buffer));
				}

				OrbMech::SStoHHMMSS(GC->lmmanpad.GETI, hh, mm, secs, 0.01);

				sprintf(Buffer, "%+06d HRS GETI", hh);
				skp->Text(CW * 17, CH * 5, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+06d MIN", mm);
				skp->Text(CW * 17, CH * 6, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.2f SEC", secs);
				skp->Text(CW * 17, CH * 7, Buffer, strlen(Buffer));

				sprintf(Buffer, "%+07.1f DVX", GC->lmmanpad.dV.x);
				skp->Text(CW * 17, CH * 8, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f DVY", GC->lmmanpad.dV.y);
				skp->Text(CW * 17, CH * 9, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f DVZ", GC->lmmanpad.dV.z);
				skp->Text(CW * 17, CH * 10, Buffer, strlen(Buffer));

				sprintf(Buffer, "%+07.1f HA", min(9999.9, GC->lmmanpad.HA));
				skp->Text(CW * 17, CH * 11, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f HP", GC->lmmanpad.HP);
				skp->Text(CW * 17, CH * 12, Buffer, strlen(Buffer));

				sprintf(Buffer, "%+07.1f DVR", GC->lmmanpad.dVR);
				skp->Text(CW * 17, CH * 13, Buffer, strlen(Buffer));

				OrbMech::SStoMMSS(GC->lmmanpad.burntime, mm, secs);
				sprintf(Buffer, "XXX%d:%02.0f BT", mm, secs);
				skp->Text(CW * 17, CH * 14, Buffer, strlen(Buffer));

				sprintf(Buffer, "XXX%03.0f R", GC->lmmanpad.Att.x);
				skp->Text(CW * 17, CH * 15, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f P", GC->lmmanpad.Att.y);
				skp->Text(CW * 17, CH * 16, Buffer, strlen(Buffer));

				sprintf(Buffer, "%+07.1f DVX AGS N86", GC->lmmanpad.dV_AGS.x);
				skp->Text(CW * 17, CH * 17, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f DVY AGS", GC->lmmanpad.dV_AGS.y);
				skp->Text(CW * 17, CH * 18, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f DVZ AGS", GC->lmmanpad.dV_AGS.z);
				skp->Text(CW * 17, CH * 19, Buffer, strlen(Buffer));

				if (GC->lmmanpad.BSSStar == 0)
				{
					skp->Text(CW * 17, CH * 20, "N/A     BSS", 11);
					skp->Text(CW * 17, CH * 21, "N/A     SPA", 11);
					skp->Text(CW * 17, CH * 22, "N/A     SXP", 11);
				}
				else
				{
					sprintf(Buffer, "XXXX%02d BSS", GC->lmmanpad.BSSStar);
					skp->Text(CW * 17, CH * 20, Buffer, strlen(Buffer));
					sprintf(Buffer, "%+07.2f SPA", GC->lmmanpad.SPA);
					skp->Text(CW * 17, CH * 21, Buffer, strlen(Buffer));
					sprintf(Buffer, "%+07.3f SXP", GC->lmmanpad.SXP);
					skp->Text(CW * 17, CH * 22, Buffer, strlen(Buffer));
				}

				sprintf(Buffer, "IMU Attitude: %06.2lf %06.2lf %06.2lf", GC->lmmanpad.IMUAtt.x*DEG, GC->lmmanpad.IMUAtt.y*DEG, GC->lmmanpad.IMUAtt.z*DEG);
				skp->Text(CW, CH * 24, Buffer, strlen(Buffer));
			}
		}
		else if (G->manpadopt == 2)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text(W / 2, CH / 2, "Terminal Phase Initiate", 23);
			skp->SetTextAlign(oapi::Sketchpad::LEFT);

			GET_Display2(Buffer, G->P30TIG);
			skp->Text(CW, CH * 7, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f", G->dV_LVLH.x / 0.3048);
			skp->Text(CW, CH * 8, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->dV_LVLH.y / 0.3048);
			skp->Text(CW, CH * 9, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f", G->dV_LVLH.z / 0.3048);
			skp->Text(CW, CH * 10, Buffer, strlen(Buffer));

			OrbMech::SStoHHMMSS(GC->TPI_PAD.GETI, hh, mm, secs, 0.01);
			skp->Text(CW * 35, CH * 5, "N37", 3);
			sprintf(Buffer, "%+06d HRS GETI", hh);
			skp->Text(CW * 17, CH * 5, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+06d MIN", mm);
			skp->Text(CW * 17, CH * 6, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.2f SEC", secs);
			skp->Text(CW * 17, CH * 7, Buffer, strlen(Buffer));

			sprintf(Buffer, "%+07.1f DVX", GC->TPI_PAD.Vg.x);
			skp->Text(CW * 17, CH * 8, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVY", GC->TPI_PAD.Vg.y);
			skp->Text(CW * 17, CH * 9, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f DVZ", GC->TPI_PAD.Vg.z);
			skp->Text(CW * 17, CH * 10, Buffer, strlen(Buffer));

			if (GC->TPI_PAD.Backup_dV.x > 0)
			{
				sprintf(Buffer, "F%04.1f/%02.0f DVX LOS/BT", abs(GC->TPI_PAD.Backup_dV.x), GC->TPI_PAD.Backup_bT.x);
			}
			else
			{
				sprintf(Buffer, "A%04.1f/%02.0f DVX LOS/BT", abs(GC->TPI_PAD.Backup_dV.x), GC->TPI_PAD.Backup_bT.x);
			}
			skp->Text(CW * 17, CH * 11, Buffer, strlen(Buffer));
			if (GC->TPI_PAD.Backup_dV.y > 0)
			{
				sprintf(Buffer, "R%04.1f/%02.0f DVY LOS/BT", abs(GC->TPI_PAD.Backup_dV.y), GC->TPI_PAD.Backup_bT.y);
			}
			else
			{
				sprintf(Buffer, "L%04.1f/%02.0f DVY LOS/BT", abs(GC->TPI_PAD.Backup_dV.y), GC->TPI_PAD.Backup_bT.y);
			}
			skp->Text(CW * 17, CH * 12, Buffer, strlen(Buffer));
			if (GC->TPI_PAD.Backup_dV.z > 0)
			{
				sprintf(Buffer, "D%04.1f/%02.0f DVZ LOS/BT", abs(GC->TPI_PAD.Backup_dV.z), GC->TPI_PAD.Backup_bT.z);
			}
			else
			{
				sprintf(Buffer, "U%04.1f/%02.0f DVZ LOS/BT", abs(GC->TPI_PAD.Backup_dV.z), GC->TPI_PAD.Backup_bT.z);
			}
			skp->Text(CW * 17, CH * 13, Buffer, strlen(Buffer));
			sprintf(Buffer, "X%04.1f/%02.1f dH TPI/ddH", GC->TPI_PAD.dH_TPI, GC->TPI_PAD.dH_Max);
			skp->Text(CW * 17, CH * 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "X%06.2f R", GC->TPI_PAD.R);
			skp->Text(CW * 17, CH * 15, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.1f RDOT at TPI", GC->TPI_PAD.Rdot);
			skp->Text(CW * 17, CH * 16, Buffer, strlen(Buffer));
			sprintf(Buffer, "X%06.2f EL minus 5 min", GC->TPI_PAD.EL);
			skp->Text(CW * 17, CH * 17, Buffer, strlen(Buffer));
			sprintf(Buffer, "X%06.2f AZ", GC->TPI_PAD.AZ);
			skp->Text(CW * 17, CH * 18, Buffer, strlen(Buffer));
		}
		else if (G->manpadopt == 3)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text(W / 2, CH / 2, "TLI PAD", 7);
			skp->SetTextAlign(oapi::Sketchpad::LEFT);

			if (G->TLIPAD_StudyAid)
			{
				skp->Text(CW, 6 * H / 14, "TLI Processor", 13);
			}
			else
			{
				skp->Text(CW, 6 * H / 14, "Nominal", 7);
			}

			GET_Display(Buffer, GC->tlipad.TB6P);
			sprintf(Buffer, "%s TB6p", Buffer);
			skp->Text(CW * 17, CH * 5, Buffer, strlen(Buffer));

			sprintf(Buffer, "XXX%03.0f R", GC->tlipad.IgnATT.x);
			skp->Text(CW * 17, CH * 6, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f P", GC->tlipad.IgnATT.y);
			skp->Text(CW * 17, CH * 7, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f Y", GC->tlipad.IgnATT.z);
			skp->Text(CW * 17, CH * 8, Buffer, strlen(Buffer));

			OrbMech::SStoMMSS(GC->tlipad.BurnTime, mm, secs);
			sprintf(Buffer, "XXX%d:%02.0f BT", mm, secs);
			skp->Text(CW * 17, CH * 9, Buffer, strlen(Buffer));

			sprintf(Buffer, "%07.1f DVC", GC->tlipad.dVC);
			skp->Text(CW * 17, CH * 10, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+06.0f VI", GC->tlipad.VI);
			skp->Text(CW * 17, CH * 11, Buffer, strlen(Buffer));

			sprintf(Buffer, "XXX%03.0f R", GC->tlipad.SepATT.x);
			skp->Text(CW * 17, CH * 12, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f P SEP", GC->tlipad.SepATT.y);
			skp->Text(CW * 17, CH * 13, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f Y", GC->tlipad.SepATT.z);
			skp->Text(CW * 17, CH * 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "XXX%03.0f R", GC->tlipad.ExtATT.x);
			skp->Text(CW * 17, CH * 15, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f P EXTRACTION", GC->tlipad.ExtATT.y);
			skp->Text(CW * 17, CH * 16, Buffer, strlen(Buffer));
			sprintf(Buffer, "XXX%03.0f Y", GC->tlipad.ExtATT.z);
			skp->Text(CW * 17, CH * 17, Buffer, strlen(Buffer));
		}
		else
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text(W / 2, CH / 2, "PDI PAD", 7);
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			if (!G->PADSolGood)
			{
				skp->Text(W - CW, 2 * H / 14, "Calculation failed!", 19);
			}
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			if (G->HeadsUp)
			{
				skp->Text(CW, 6 * H / 14, "Heads Up", 8);
			}
			else
			{
				skp->Text(CW, 6 * H / 14, "Heads Down", 10);
			}
			
			skp->Text(CW * 15, CH * 21, "T_L:", 4);
			GET_Display(Buffer, GC->rtcc->CZTDTGTU.GETTD);
			skp->Text(CW * 20, CH * 21, Buffer, strlen(Buffer));

			skp->Text(CW * 15, CH * 22, "Lat:", 4);
			sprintf(Buffer, "%.3f°", GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST] * DEG);
			skp->Text(CW * 20, CH * 22, Buffer, strlen(Buffer));

			skp->Text(CW * 15, CH * 23, "Lng:", 4);
			sprintf(Buffer, "%.3f°", GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST] * DEG);
			skp->Text(CW * 20, CH * 23, Buffer, strlen(Buffer));

			skp->Text(CW * 15, CH * 24, "Rad:", 4);
			sprintf(Buffer, "%.2f NM", GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST] / 1852.0);
			skp->Text(CW * 20, CH * 24, Buffer, strlen(Buffer));

			OrbMech::SStoHHMMSS(GC->pdipad.GETI, hh, mm, secs, 0.01);

			skp->Text(CW * 15, CH * 5, "HRS      TIG", 12);
			sprintf(Buffer, "%+06d", hh);
			skp->Text(CW * 32, CH * 5, Buffer, strlen(Buffer));
			skp->Text(CW * 15, CH * 6, "MIN      PDI", 12);
			sprintf(Buffer, "%+06d", mm);
			skp->Text(CW * 32, CH * 6, Buffer, strlen(Buffer));
			skp->Text(CW * 15, CH * 7, "SEC", 3);
			sprintf(Buffer, "%+07.2f", secs);
			skp->Text(CW * 32, CH * 7, Buffer, strlen(Buffer));
			OrbMech::SStoMMSS(GC->pdipad.t_go, mm, secs);
			skp->Text(CW * 15, CH * 8, "TGO      N61", 12);
			sprintf(Buffer, "XX%02d:%02.0f", mm, secs);
			skp->Text(CW * 32, CH * 8, Buffer, strlen(Buffer));
			skp->Text(CW * 15, CH * 9, "CROSSRANGE", 10);
			sprintf(Buffer, "%07.1f", GC->pdipad.CR);
			skp->Text(CW * 32, CH * 9, Buffer, strlen(Buffer));
			skp->Text(CW * 15, CH * 10, "R        FDAI", 13);
			sprintf(Buffer, "XXX%03.0f", GC->pdipad.Att.x);
			skp->Text(CW * 32, CH * 10, Buffer, strlen(Buffer));
			skp->Text(CW * 15, CH * 11, "P        AT TIG", 15);
			sprintf(Buffer, "XXX%03.0f", GC->pdipad.Att.y);
			skp->Text(CW * 32, CH * 11, Buffer, strlen(Buffer));
			skp->Text(CW * 15, CH * 12, "Y", 1);
			sprintf(Buffer, "XXX%03.0f", GC->pdipad.Att.z);
			skp->Text(CW * 32, CH * 12, Buffer, strlen(Buffer));
			skp->Text(CW * 15, CH * 13, "DEDA 231 IF RQD", 15);
			sprintf(Buffer, "%+06.0f", GC->pdipad.DEDA231);
			skp->Text(CW * 32, CH * 13, Buffer, strlen(Buffer));
		}
		break;
	case 10:
		skp->SetFont(font_menu2);
		GetCharSize(skp, CW, CH);

			if (GC->entrypadopt == 0)
			{
				skp->Text(CW * 21, CH / 2, "Earth Entry PAD", 15);

				skp->Text(CW * 27, CH * 3, "PREBURN", 7);
				sprintf(Buffer, "XX%+05.1f dV TO", GC->earthentrypad.dVTO[0]);
				skp->Text(CW * 23, CH * 4, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f R 0.05G", GC->earthentrypad.Att400K[0].x);
				skp->Text(CW * 23, CH * 5, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f P 0.05G", GC->earthentrypad.Att400K[0].y);
				skp->Text(CW * 23, CH * 6, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f Y 0.05G", GC->earthentrypad.Att400K[0].z);
				skp->Text(CW * 23, CH * 7, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f RTGO .05G", GC->earthentrypad.RTGO[0]);
				skp->Text(CW * 23, CH * 8, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+06.0f VIO  .05G", GC->earthentrypad.VIO[0]);
				skp->Text(CW * 23, CH * 9, Buffer, strlen(Buffer));
				
				OrbMech::SStoMMSS(GC->earthentrypad.Ret05[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RET  .05G", mm, secs);
				skp->Text(CW * 23, CH * 10, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.2f LAT", GC->earthentrypad.Lat[0]);
				skp->Text(CW * 23, CH * 11, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.2f LONG", GC->earthentrypad.Lng[0]);
				skp->Text(CW * 23, CH * 12, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->earthentrypad.Ret2[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RET  .2G", mm, secs);
				skp->Text(CW * 23, CH * 13, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1lf DRE (55°)  N66", GC->earthentrypad.DRE[0]);
				skp->Text(CW * 23, CH * 14, Buffer, strlen(Buffer));
				sprintf(Buffer, "RR55/55 BANK AN");
				skp->Text(CW * 23, CH * 15, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->earthentrypad.RetRB[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RET RB", mm, secs);
				skp->Text(CW * 23, CH * 16, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->earthentrypad.RetBBO[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RETBBO", mm, secs);
				skp->Text(CW * 23, CH * 17, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->earthentrypad.RetEBO[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RETEBO", mm, secs);
				skp->Text(CW * 23, CH * 18, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->earthentrypad.RetDrog[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RETDROG", mm, secs);
				skp->Text(CW * 23, CH * 19, Buffer, strlen(Buffer));
				skp->Text(CW * 27, CH * 20, "POSTBURN", 8);
				sprintf(Buffer, "XXX%03.0f R 0.05G", GC->earthentrypad.PB_R400K[0]);
				skp->Text(CW * 23, CH * 21, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f RTGO .05G", GC->earthentrypad.PB_RTGO[0]);
				skp->Text(CW * 23, CH * 22, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+06.0f VIO  .05G", GC->earthentrypad.PB_VIO[0]);
				skp->Text(CW * 23, CH * 23, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->earthentrypad.PB_Ret05[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RET  .05G", mm, secs);
				skp->Text(CW * 23, CH * 24, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->earthentrypad.PB_Ret2[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RET  .2G", mm, secs);
				skp->Text(CW * 23, CH * 25, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1lf DRE +/- 100nm  N66", GC->earthentrypad.PB_DRE[0]);
				skp->Text(CW * 23, CH * 26, Buffer, strlen(Buffer));
				sprintf(Buffer, "RR55/55 BANK AN");
				skp->Text(CW * 23, CH * 27, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->earthentrypad.PB_RetRB[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RET RB", mm, secs);
				skp->Text(CW * 23, CH * 28, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->earthentrypad.PB_RetBBO[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RETBBO", mm, secs);
				skp->Text(CW * 23, CH * 29, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->earthentrypad.PB_RetEBO[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RETEBO", mm, secs);
				skp->Text(CW * 23, CH * 30, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->earthentrypad.PB_RetDrog[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RETDROG", mm, secs);
				skp->Text(CW * 23, CH * 31, Buffer, strlen(Buffer));

				skp->Text(CW, CH * 10, "Deorbit:", 8);
				ThrusterName(Buffer, G->manpadenginetype);
				skp->Text(CW, CH * 11, Buffer, strlen(Buffer));
				GET_Display(Buffer, G->P30TIG);
				skp->Text(CW, CH * 12, Buffer, strlen(Buffer));
				skp->Text(CW, CH * 13, "DVX", 3);
				skp->Text(CW, CH * 14, "DVY", 3);
				skp->Text(CW, CH * 15, "DVZ", 3);
				AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
				skp->Text(CW * 7, CH * 13, Buffer, strlen(Buffer));
				AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
				skp->Text(CW * 7, CH * 14, Buffer, strlen(Buffer));
				AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
				skp->Text(CW * 7, CH * 15, Buffer, strlen(Buffer));
				skp->Text(CW, CH * 16, "Initial Bank:", 13);
				sprintf(Buffer, "%+.2lf°", GC->rtcc->RZC1RCNS.entry.GNInitialBank*DEG);
				skp->Text(CW, CH * 17, Buffer, strlen(Buffer));
				skp->Text(CW, CH * 18, "G-Level:", 8);
				sprintf(Buffer, "%+.2lf", GC->rtcc->RZC1RCNS.entry.GLevel);
				skp->Text(CW, CH * 19, Buffer, strlen(Buffer));
				skp->Text(CW, CH * 20, "Splashdown:", 11);
				sprintf(Buffer, "Lat:  %+.2f°", GC->rtcc->RZDBSC1.lat_T*DEG);
				skp->Text(CW, CH * 21, Buffer, strlen(Buffer));
				sprintf(Buffer, "Long: %+.2f°", GC->rtcc->RZDBSC1.lng_T*DEG);
				skp->Text(CW, CH * 22, Buffer, strlen(Buffer));
			}
			else
			{
				skp->Text(CW * 21, CH / 2, "Lunar Entry PAD", 15);

				skp->Text(CW, 4 * H / 28, "Sxt Star Check at", 17);
				if (GC->EntryPADSxtStarCheckAttOpt)
				{
					skp->Text(CW, 5 * H / 28, "Entry Attitude", 14);
				}
				else
				{
					skp->Text(CW, 5 * H / 28, "Hor Check Attitude", 18);
				}
				if (G->entryrange != 0)
				{
					skp->Text(CW, 6 * H / 14, "Desired Range:", 14);
					sprintf(Buffer, "%.1f NM", G->entryrange);
					skp->Text(CW, 7 * H / 14, Buffer, strlen(Buffer));
				}

				sprintf(Buffer, "XXX%03.0f R 0.05G", GC->lunarentrypad.Att05[0].x);
				skp->Text(CW * 23, CH * 5, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f P 0.05G", GC->lunarentrypad.Att05[0].y);
				skp->Text(CW * 23, CH * 6, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f Y 0.05G", GC->lunarentrypad.Att05[0].z);
				skp->Text(CW * 23, CH * 7, Buffer, strlen(Buffer));
				GET_Display(Buffer, GC->lunarentrypad.GETHorCheck[0]);
				skp->Text(CW * 23, CH * 8, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%03.0f P HOR CK", GC->lunarentrypad.PitchHorCheck[0]);
				skp->Text(CW * 23, CH * 9, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.2f LAT", GC->lunarentrypad.Lat[0]);
				skp->Text(CW * 23, CH * 10, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.2f LONG", GC->lunarentrypad.Lng[0]);
				skp->Text(CW * 23, CH * 11, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%04.1f MAX G", GC->lunarentrypad.MaxG[0]);
				skp->Text(CW * 23, CH * 12, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+06.0f V400k", GC->lunarentrypad.V400K[0]);
				skp->Text(CW * 23, CH * 13, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.2f y400k", GC->lunarentrypad.Gamma400K[0]);
				skp->Text(CW * 23, CH * 14, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.1f RTGO .05G", GC->lunarentrypad.RTGO[0]);
				skp->Text(CW * 23, CH * 15, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+06.0f VIO  .05G", GC->lunarentrypad.VIO[0]);
				skp->Text(CW * 23, CH * 16, Buffer, strlen(Buffer));
				GET_Display(Buffer, GC->lunarentrypad.RRT[0]);
				sprintf(Buffer, "%s RRT", Buffer);
				skp->Text(CW * 23, CH * 17, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->lunarentrypad.RET05[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RET  .05G", mm, secs);
				skp->Text(CW * 23, CH * 18, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.2lf DL MAX", GC->lunarentrypad.DLMax[0]);
				skp->Text(CW * 23, CH * 19, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+07.2lf DL MIN", GC->lunarentrypad.DLMin[0]);
				skp->Text(CW * 23, CH * 20, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+06.0lf VL MAX", GC->lunarentrypad.VLMax[0]);
				skp->Text(CW * 23, CH * 21, Buffer, strlen(Buffer));
				sprintf(Buffer, "%+06.0lf VL MIN", GC->lunarentrypad.VLMin[0]);
				skp->Text(CW * 23, CH * 22, Buffer, strlen(Buffer));
				sprintf(Buffer, "XXX%04.2f DO", GC->lunarentrypad.DO[0]);
				skp->Text(CW * 23, CH * 23, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->lunarentrypad.RETVCirc[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RET V CIRC", mm, secs);
				skp->Text(CW * 23, CH * 24, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->lunarentrypad.RETBBO[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RETBBO", mm, secs);
				skp->Text(CW * 23, CH * 25, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->lunarentrypad.RETEBO[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RETEBO", mm, secs);
				skp->Text(CW * 23, CH * 26, Buffer, strlen(Buffer));
				OrbMech::SStoMMSS(GC->lunarentrypad.RETDRO[0], mm, secs);
				sprintf(Buffer, "XX%02d:%02.0f RETDRO", mm, secs);
				skp->Text(CW * 23, CH * 27, Buffer, strlen(Buffer));
				if (GC->lunarentrypad.SXTS[0] == 0)
				{
					skp->Text(CW * 23, CH * 28, "N/A     SXTS", 12);
					skp->Text(CW * 23, CH * 29, "N/A     SFT", 11);
					skp->Text(CW * 23, CH * 30, "N/A     TRN", 1);
				}
				else
				{
					sprintf(Buffer, "XXXX%02d SXTS", GC->lunarentrypad.SXTS[0]);
					skp->Text(CW * 23, CH * 28, Buffer, strlen(Buffer));
					sprintf(Buffer, "%+07.2f SFT", GC->lunarentrypad.SFT[0]);
					skp->Text(CW * 23, CH * 29, Buffer, strlen(Buffer));
					sprintf(Buffer, "%+07.3f TRN", GC->lunarentrypad.TRN[0]);
					skp->Text(CW * 23, CH * 30, Buffer, strlen(Buffer));
				}
				sprintf(Buffer, "XXXX%s LIFT VECTOR", GC->lunarentrypad.LiftVector[0]);
				skp->Text(CW * 23, CH * 31, Buffer, strlen(Buffer));

				skp->Text(CW, CH * 26, "Splashdown:", 11);
				sprintf(Buffer, "Lat:  %+.2f°", GC->rtcc->RZDBSC1.lat_T*DEG);
				skp->Text(CW, CH * 27, Buffer, strlen(Buffer));
				sprintf(Buffer, "Long: %+.2f°", GC->rtcc->RZDBSC1.lng_T*DEG);
				skp->Text(CW, CH * 28, Buffer, strlen(Buffer));
			}
		break;
	case 11:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Map Update", 10);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		CSMOrLMSelection(skp);
		GET_Display(Buffer, G->mapUpdateGET);
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		if (G->mappage == 0)
		{
			sprintf(Buffer, gsnames[G->mapgs]);
			skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
			GET_Display(Buffer2, G->GSAOSGET);
			sprintf(Buffer, "AOS %s", Buffer2);
			skp->Text(CW, 7 * H / 14, Buffer, strlen(Buffer));
			GET_Display(Buffer2, G->GSLOSGET);
			sprintf(Buffer, "LOS %s", Buffer2);
			skp->Text(CW, 9 * H / 14, Buffer, strlen(Buffer));

			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			skp->Text(W - CW, 4 * H / 14, "Earth", 5);
		}
		else
		{
			if (G->mapUpdatePM)
			{
				skp->Text(CW, 6 * H / 14, "Prime Meridian: 180°W", 21);
			}
			else
			{
				skp->Text(CW, 6 * H / 14, "Prime Meridian: 150°W", 21);
			}
			GET_Display(Buffer2, G->mapupdate.LOSGET);
			sprintf(Buffer, "LOS %s", Buffer2);
			skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
			GET_Display(Buffer2, G->mapupdate.SRGET);
			sprintf(Buffer, "SR  %s", Buffer2);
			skp->Text(CW, 9 * H / 14, Buffer, strlen(Buffer));
			GET_Display(Buffer2, G->mapupdate.PMGET);
			sprintf(Buffer, "PM  %s", Buffer2);
			skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
			GET_Display(Buffer2, G->mapupdate.AOSGET);
			sprintf(Buffer, "AOS %s", Buffer2);
			skp->Text(CW, 11 * H / 14, Buffer, strlen(Buffer));
			GET_Display(Buffer2, G->mapupdate.SSGET);
			sprintf(Buffer, "SS  %s", Buffer2);
			skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			skp->Text(W - CW, 4 * H / 14, "Moon", 4);
		}
		break;
	case 12:
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text(W / 2, CH / 2, "LOI Computation (K18)", 21);
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			skp->Text(CW, 2 * H / 14, "LOI Initialization", 18);
			if (GC->MissionPlanningActive)
			{
				GET_Display(Buffer, GC->rtcc->med_k18.VectorTime);
			}
			else
			{
				PrintCSMVessel(Buffer);
			}
			skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.1lf NM", GC->rtcc->med_k18.HALOI1);
			skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.1lf NM", GC->rtcc->med_k18.HPLOI1);
			skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.0lf ft/s", GC->rtcc->med_k18.DVMAXp);
			skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.0lf ft/s", GC->rtcc->med_k18.DVMAXm);
			skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			sprintf(Buffer, "%.1f°", GC->rtcc->med_k18.psi_MN);
			skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.1f°", GC->rtcc->med_k18.psi_DS);
			skp->Text(W - CW, 6 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.1f°", GC->rtcc->med_k18.psi_MX);
			skp->Text(W - CW, 8 * H / 14, Buffer, strlen(Buffer));
			break;
	case 13:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Landmark Tracking", 17);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		PrintCSMVessel(Buffer);
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		if (GC->LmkTime <= 0.0)
		{
			sprintf(Buffer, "Present Time");
		}
		else
		{
			GET_Display(Buffer, GC->LmkTime);
		}
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", GC->LmkElevation*DEG);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", GC->LmkLat*DEG);
		skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", GC->LmkLng*DEG);
		skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		
		GET_Display(Buffer2, GC->landmarkpad.T1[0]);
		sprintf(Buffer, "T1: %s (HOR)", Buffer2);
		skp->Text(CW * 16, CH * 15, Buffer, strlen(Buffer));
		GET_Display(Buffer2, GC->landmarkpad.T2[0]);
		sprintf(Buffer, "T2: %s (%.0lf°)", Buffer2, GC->LmkElevation*DEG);
		skp->Text(CW * 16, CH * 16, Buffer, strlen(Buffer));
		if (GC->landmarkpad.CRDist[0] > 0)
		{
			sprintf(Buffer, "%.1f NM North", GC->landmarkpad.CRDist[0]);
		}
		else
		{
			sprintf(Buffer, "%.1f NM South", abs(GC->landmarkpad.CRDist[0]));
		}
		skp->Text(CW * 16, CH * 17, Buffer, strlen(Buffer));
		skp->Text(CW * 16, CH * 18, "N89", 3);
		sprintf(Buffer, "Lat %+07.3f°", GC->landmarkpad.Lat[0]);
		skp->Text(CW * 16, CH * 19, Buffer, strlen(Buffer));
		sprintf(Buffer, "Long/2 %+07.3f°", GC->landmarkpad.Lng05[0]);
		skp->Text(CW * 16, CH * 20, Buffer, strlen(Buffer));
		sprintf(Buffer, "Alt %+07.2f NM", GC->landmarkpad.Alt[0]);
		skp->Text(CW * 16, CH * 21, Buffer, strlen(Buffer));
		break;
	case 14:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Maneuver Targeting", 18);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "Rendezvous", 10);
		skp->Text(CW, 4 * H / 14, "General Purpose Maneuver", 24);
		skp->Text(CW, 6 * H / 14, "Translunar", 10);
		skp->Text(CW, 8 * H / 14, "Lunar Insertion", 15);
		skp->Text(CW, 10 * H / 14, "Return to Earth", 15);
		skp->Text(CW, 12 * H / 14, "Deorbit", 7);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		skp->Text(W - CW, 2 * H / 14, "Descent Planning", 16);
		skp->Text(W - CW, 4 * H / 14, "LLWP", 4);
		skp->Text(W - CW, 6 * H / 14, "LLTP", 4);
		skp->Text(W - CW, 8 * H / 14, "Lunar Ascent", 12);
		skp->Text(W - CW, 10 * H / 14, "Perigee Adjust", 14);
		skp->Text(W - CW, 12 * H / 14, "Previous Page", 13);
		break;
	case 15:
		CSMOrLMSelection(skp);
		if (G->VECoption == 0)
		{
			skp->Text(CW, 4 * H / 14, "Point SC at body", 16);
		}
		else
		{
			skp->Text(CW, 4 * H / 14, "Open hatch thermal control", 26);
		}
		if (G->VECoption == 0)
		{
			if (G->VECbody != NULL)
			{
				oapiGetObjectName(G->VECbody, Buffer, 20);
				skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
			}
			switch (G->VECdirection)
			{
			case 0:
				skp->Text(CW, 8 * H / 14, "+X", 2);
				break;
			case 1:
				skp->Text(CW, 8 * H / 14, "-X", 2);
				break;
			case 2:
				skp->Text(CW, 8 * H / 14, "Optics", 6);
				break;
			case 3:
				skp->Text(CW, 8 * H / 14, "SIM Bay", 7);
				break;
			default:
				skp->Text(CW, 8 * H / 14, "Selectable", 10);
				sprintf(Buffer, "Y: %+07.2lf°", G->VECBodyVector.x*DEG);
				skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
				sprintf(Buffer, "P: %+07.2lf°", G->VECBodyVector.y*DEG);
				skp->Text(CW, 11 * H / 14, Buffer, strlen(Buffer));
				break;
			}
			sprintf(Buffer, "O: %+07.2lf°", G->VECBodyVector.z*DEG);
			skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			sprintf(Buffer, "%+07.2f R", G->VECangles.x*DEG);
			skp->Text(W - CW, 10 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.2f P", G->VECangles.y*DEG);
			skp->Text(W - CW, 11 * H / 14, Buffer, strlen(Buffer));
			sprintf(Buffer, "%+07.2f Y", G->VECangles.z*DEG);
			skp->Text(W - CW, 12 * H / 14, Buffer, strlen(Buffer));
		}
		break;
	case 16:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Computation for Lunar Descent Planning", 39);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "Init", 4);
		if (GC->rtcc->med_k16.Vehicle == RTCC_MPT_LM)
		{
			skp->Text(CW, 4 * H / 14, "LEM", 3);
		}
		else
		{
			skp->Text(CW, 4 * H / 14, "CSM", 3);
		}
		if (GC->MissionPlanningActive)
		{
			GET_Display(Buffer, GC->rtcc->med_k16.VectorTime, false);
		}
		else
		{
			if (GC->rtcc->med_k16.Vehicle == RTCC_MPT_LM)
			{
				PrintLMVessel(Buffer);
			}
			else
			{
				PrintCSMVessel(Buffer);
			}
		}
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->med_k16.Mode == 1)
		{
			skp->Text(CW, 8 * H / 14, "CSM Phase Change", 16);

			if (GC->rtcc->med_k16.Sequence == 1)
			{
				skp->Text(CW, 10 * H / 14, "1: PC, DOI", 10);
			}
			else if (GC->rtcc->med_k16.Sequence == 2)
			{
				skp->Text(CW, 10 * H / 14, "2: PCC, DOI", 11);
			}
			else if (GC->rtcc->med_k16.Sequence == 3)
			{
				skp->Text(CW, 10 * H / 14, "3: ASP, CIA, DOI", 16);
			}
			else if (GC->rtcc->med_k16.Sequence == 4)
			{
				skp->Text(CW, 10 * H / 14, "4: PCCH, DOI", 12);
			}
			else if (GC->rtcc->med_k16.Sequence == 5)
			{
				skp->Text(CW, 10 * H / 14, "5: PCCT, DOI", 12);
			}
		}
		else if (GC->rtcc->med_k16.Mode == 2)
		{
			skp->Text(CW, 8 * H / 14, "Single CSM Maneuver", 19);

			if (GC->rtcc->med_k16.Sequence == 2)
			{
				skp->Text(CW, 10 * H / 14, "2: ASH, DOI", 11);
			}
			else if (GC->rtcc->med_k16.Sequence == 3)
			{
				skp->Text(CW, 10 * H / 14, "3: CIR, DOI", 11);
			}
			else
			{
				sprintf(Buffer, "%d: Not Used", GC->rtcc->med_k16.Sequence);
				skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
			}
		}
		else if (GC->rtcc->med_k16.Mode == 3)
		{
			skp->Text(CW, 8 * H / 14, "Double CSM Maneuver", 19);

			if (GC->rtcc->med_k16.Sequence == 1)
			{
				skp->Text(CW, 10 * H / 14, "1: ASH at time, CIA, DOI", 24);
			}
			else if (GC->rtcc->med_k16.Sequence == 3)
			{
				skp->Text(CW, 10 * H / 14, "3: ASH at apsis, CIA, DOI", 25);
			}
			else
			{
				sprintf(Buffer, "%d: Not Used", GC->rtcc->med_k16.Sequence);
				skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
			}
		}
		else if (GC->rtcc->med_k16.Mode == 4)
		{
			skp->Text(CW, 8 * H / 14, "Descent Orbit Insertion", 23);
			switch (GC->rtcc->med_k16.Sequence)
			{
			case 1:
				skp->Text(CW, 10 * H / 14, "1: DOI only", 11);
				break;
			case 2:
				skp->Text(CW, 10 * H / 14, "2: DOI with plane change", 24);
				break;
			case 3:
				skp->Text(CW, 10 * H / 14, "3: Integrated DOI only", 22);
				break;
			case 4:
				skp->Text(CW, 10 * H / 14, "4: Integrated DOI with plane change", 35);
				break;
			default:
				sprintf(Buffer, "%d: Not Used", GC->rtcc->med_k16.Sequence);
				skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
				break;
			}
		}
		else if (GC->rtcc->med_k16.Mode == 5)
		{
			skp->Text(CW, 8 * H / 14, "Double Hohmann, PC", 28);

			if (GC->rtcc->med_k16.Sequence == 1)
			{
				skp->Text(CW, 10 * H / 14, "1: PC, HO1, HO2, DOI", 20);
			}
			else if (GC->rtcc->med_k16.Sequence == 2)
			{
				skp->Text(CW, 10 * H / 14, "2: HO1, PC, HO2, DOI", 20);
			}
			else if (GC->rtcc->med_k16.Sequence == 3)
			{
				skp->Text(CW, 10 * H / 14, "3: HO1, HO2, PC, DOI", 20);
			}
			else
			{
				sprintf(Buffer, "%d: Not Used", GC->rtcc->med_k16.Sequence);
				skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
			}
		}
		else if (GC->rtcc->med_k16.Mode == 6)
		{
			skp->Text(CW, 8 * H / 14, "LM Powered Descent", 18);
		}
		else if (GC->rtcc->med_k16.Mode == 7)
		{
			skp->Text(CW, 8 * H / 14, "CSM Prelaunch Plane Change", 26);
			skp->Text(CW, 10 * H / 14, "PPC", 3);
		}

		sprintf(Buffer, "%.3f NM", GC->rtcc->med_k16.DesiredHeight / 1852.0);
		skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		GET_Display(Buffer, GC->rtcc->med_k16.GETTH1, false);
		skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->rtcc->med_k16.GETTH2, false);
		skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->rtcc->med_k16.GETTH3, false);
		skp->Text(W - CW, 6 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->rtcc->med_k16.GETTH4, false);
		skp->Text(W - CW, 8 * H / 14, Buffer, strlen(Buffer));
		break;
	case 17:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Translunar Options", 18);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "TLI Processor", 13);
		skp->Text(CW, 4 * H / 14, "Midcourse Processor", 19);
		skp->Text(CW, 6 * H / 14, "Midcourse Constraints", 21);
		skp->Text(CW, 8 * H / 14, "Skeleton Flight Plan Table", 26);
		break;
	case 18:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Initialization for Lunar Descent Planning", 41);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (GC->rtcc->GZGENCSN.LDPPAzimuth != 0.0)
		{
			sprintf(Buffer, "%.3f°", GC->rtcc->GZGENCSN.LDPPAzimuth*DEG);
			skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(CW, 2 * H / 14, "Optimum Azimuth", 15);
		}
		sprintf(Buffer, "%.0f ft", GC->rtcc->GZGENCSN.LDPPHeightofPDI / 0.3048);
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->GZGENCSN.LDPPPoweredDescentSimFlag)
		{
			skp->Text(CW, 6 * H / 14, "Simulate descent (N/A)", 22);
		}
		else
		{
			skp->Text(CW, 6 * H / 14, "Do not simulate descent", 23);
		}
		if (GC->rtcc->GZGENCSN.LDPPTimeofPDI != 0.0)
		{
			GET_Display(Buffer, GC->rtcc->GZGENCSN.LDPPTimeofPDI);
		}
		else
		{
			sprintf(Buffer, "Calculate PDI time");
		}
		skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%d", GC->rtcc->GZGENCSN.LDPPDwellOrbits);
		skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		sprintf(Buffer, "%.2f min", GC->rtcc->GZGENCSN.LDPPDescentFlightTime / 60.0);
		skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f°", GC->rtcc->GZGENCSN.LDPPDescentFlightArc*DEG);
		skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f°", GC->rtcc->GZGENCSN.LDPPLandingSiteOffset*DEG);
		skp->Text(W - CW, 6 * H / 14, Buffer, strlen(Buffer));
		break;
	case 19:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Terrain Model", 13);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		sprintf(Buffer, "%.3f°", G->TMLat*DEG);
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", G->TMLng*DEG);
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", G->TMAzi*DEG);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.1f ft", G->TMDistance / 0.3048);
		skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.1f ft", G->TMStepSize / 0.3048);
		skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		skp->Text(CW * 25, 9 * H / 14, "LS Height:", 10);
		sprintf(Buffer, "%.2f NM", G->TMAlt / 1852.0);
		skp->Text(CW * 25, 10 * H / 14, Buffer, strlen(Buffer));
		break;
	case 20:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Pre-Advisory Data", 17);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "Maneuver PAD", 12);
		skp->Text(CW, 4 * H / 14, "Entry PAD", 9);
		skp->Text(CW, 6 * H / 14, "Landmark Tracking", 17);
		skp->Text(CW, 8 * H / 14, "Map Update", 10);
		skp->Text(CW, 10 * H / 14, "Nav Check PAD", 13);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		skp->Text(W - CW, 2 * H / 14, "DAP PAD", 7);
		skp->Text(W - CW, 4 * H / 14, "LM Ascent PAD", 13);
		skp->Text(W - CW, 6 * H / 14, "AGS SV PAD", 10);
		skp->Text(W - CW, 12 * H / 14, "Previous Page", 13);
		break;
	case 21:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Utility", 7);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "Landing Site", 12);
		skp->Text(CW, 4 * H / 14, "REFSMMAT", 8);
		skp->Text(CW, 6 * H / 14, "RTACF", 5);
		skp->Text(CW, 8 * H / 14, "IMU Parking Angles", 18);
		skp->Text(CW, 10 * H / 14, "Nodal Target Conversion", 23);
		skp->Text(CW, 12 * H / 14, "Descent Abort", 13);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		skp->Text(W - CW, 2 * H / 14, "Saturn IB LVDC", 14);
		skp->Text(W - CW, 4 * H / 14, "Saturn V LVDC", 13);
		skp->Text(W - CW, 6 * H / 14, "Terrain Model", 13);
		skp->Text(W - CW, 8 * H / 14, "Lunar Impact", 12);
		skp->Text(W - CW, 10 * H / 14, "Debug", 5);
		skp->Text(W - CW, 12 * H / 14, "Previous Page", 13);
		break;
	case 22:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Midcourse Correction Planning", 29);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		switch (GC->rtcc->PZMCCPLN.Mode)
		{
		case 1:
			sprintf_s(Buffer, "Option 1: Nodal Targeting");
			break;
		case 2:
			sprintf_s(Buffer, "Option 2: FR BAP, Fixed LPO, LS");
			break;
		case 3:
			sprintf_s(Buffer, "Option 3: FR BAP, Free LPO, LS");
			break;
		case 4:
			sprintf_s(Buffer, "Option 4: Non-FR BAP, Fixed LPO, LS");
			break;
		case 5:
			sprintf_s(Buffer, "Option 5: Non-FR BAP, Free LPO, LS");
			break;
		case 6:
			sprintf_s(Buffer, "Option 6: Circumlunar flyby, nominal");
			break;
		case 7:
			sprintf_s(Buffer, "Option 7: Circumlunar flyby, spec. H_pc");
			break;
		case 8:
			sprintf_s(Buffer, "Option 8: SPS flyby to spec. FR incl.");
			break;
		default:
			sprintf_s(Buffer, "Option 9: Fuel critical lunar flyby");
			break;
		}
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		if (GC->MissionPlanningActive)
		{
			GET_Display(Buffer, GC->rtcc->PZMCCPLN.VectorGET);
		}
		else
		{
			PrintCSMVessel(Buffer);
		}
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->rtcc->PZMCCPLN.MidcourseGET);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%d", GC->rtcc->PZMCCPLN.Column);
		skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->PZMCCPLN.Config)
		{
			skp->Text(CW, 10 * H / 14, "Docked", 6);
		}
		else
		{
			skp->Text(CW, 10 * H / 14, "Undocked", 8);
		}
		if (GC->rtcc->PZMCCPLN.SFPBlockNum == 1)
		{
			sprintf(Buffer, "1 (Preflight)");
		}
		else
		{
			sprintf(Buffer, "2 (Nominal Targets)");
		}
		skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (GC->rtcc->PZMCCPLN.Mode == 7)
		{
			sprintf(Buffer, "%.2f NM", GC->rtcc->PZMCCPLN.h_PC / 1852.0);
			skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
		}
		else if (GC->rtcc->PZMCCPLN.Mode >= 8)
		{
			if (GC->rtcc->PZMCCPLN.h_PC <= 0)
			{
				sprintf(Buffer, "Height from SFP");
			}
			else
			{
				sprintf(Buffer, "%.2f NM", GC->rtcc->PZMCCPLN.h_PC / 1852.0);
			}
			skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
		}
		if (GC->rtcc->PZMCCPLN.Mode >= 8)
		{
			sprintf(Buffer, "%.2f°", GC->rtcc->PZMCCPLN.incl_fr*DEG);
			skp->Text(W - CW, 6 * H / 14, Buffer, strlen(Buffer));
		}
		if (GC->rtcc->PZMCCPLN.Mode == 5)
		{
			if (GC->rtcc->PZMCCPLN.h_PC_mode5 < 0)
			{
				sprintf(Buffer, "Height from SFP");
			}
			else
			{
				sprintf(Buffer, "%.2f NM", GC->rtcc->PZMCCPLN.h_PC_mode5 / 1852.0);
			}
			skp->Text(W - CW, 8 * H / 14, Buffer, strlen(Buffer));
		}
		break;
	case 23:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Lunar Launch Window", 19);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "Initialization", 14);
		if (GC->rtcc->med_k15.Chaser == 1)
		{
			skp->Text(CW, 4 * H / 14, "Chaser: CSM", 11);
		}
		else
		{
			skp->Text(CW, 4 * H / 14, "Chaser: LM", 10);
		}
		if (GC->MissionPlanningActive)
		{
			GET_Display2(Buffer, GC->rtcc->med_k15.CSMVectorTime);
		}
		else
		{
			PrintCSMVessel(Buffer);
		}
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->med_k15.TPIDefinition == 1)
		{
			skp->Text(CW, 8 * H / 14, "TLO:", 4);
		}
		else
		{
			skp->Text(CW, 8 * H / 14, "TPI:", 4);
		}
		GET_Display(Buffer, GC->rtcc->med_k15.ThresholdTime, false);
		skp->Text(CW * 6, 8 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->med_k15.CSI_Flag == 0.0)
		{
			skp->Text(CW, 10 * H / 14, "CSI at 90 degrees from insertion", 32);
		}
		else if (GC->rtcc->med_k15.CSI_Flag < 0)
		{
			skp->Text(CW, 10 * H / 14, "CSI at LM apolune", 17);
		}
		else
		{
			sprintf(Buffer, "%.1lf min", GC->rtcc->med_k15.CSI_Flag / 60.0);
			skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		}
		if (GC->rtcc->med_k15.CDH_Flag == 0)
		{
			skp->Text(CW, 12 * H / 14, "CDH at apsis after CSI", 22);
		}
		else
		{
			sprintf(Buffer, "CDH %d half revs after CSI", GC->rtcc->med_k15.CDH_Flag);
			skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (GC->rtcc->med_k15.TPIDefinition == 1)
		{
			sprintf(Buffer, "TPI longitude: %.4lf°", GC->rtcc->med_k15.TPIValue*DEG);
			skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(W - CW, 4 * H / 14, "TPI at threshold time", 22);
		}
		if (GC->rtcc->med_k15.DeltaHTFlag > 0)
		{
			sprintf(Buffer, "Launch window with %d heights", GC->rtcc->med_k15.DeltaHTFlag);
			skp->Text(W - CW, 6 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(W - CW, 6 * H / 14, "Calc using input heights", 24);
			sprintf(Buffer, "%.2lf %.2lf %.2lf NM", GC->rtcc->med_k15.DH1 / 1852.0, GC->rtcc->med_k15.DH2 / 1852.0, GC->rtcc->med_k15.DH3 / 1852.0);
			skp->Text(W - CW, 8 * H / 14, Buffer, strlen(Buffer));
		}
		break;
	case 24:
	{
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		RTCC::AGCErasableMemoryUpdateMakeupBlock *block = &GC->rtcc->CZERAMEM.Blocks[subscreen];
		if (subscreen == 0)
		{
			skp->Text(CW * 12, CH / 2, "CMC ERASABLE MEMORY UPDATE A", 31);
			skp->Text(CW * 52, CH / 2, "0281", 4);
		}
		else if (subscreen == 1)
		{
			skp->Text(CW * 12, CH / 2, "CMC ERASABLE MEMORY UPDATE B", 31);
			skp->Text(CW * 52, CH / 2, "0282", 4);
		}
		else if (subscreen == 2)
		{
			skp->Text(CW * 12, CH / 2, "LGC ERASABLE MEMORY UPDATE A", 31);
			skp->Text(CW * 52, CH / 2, "0269", 4);
		}
		else
		{
			skp->Text(CW * 12, CH / 2, "LGC ERASABLE MEMORY UPDATE B", 31);
			skp->Text(CW * 52, CH / 2, "0275", 4);
		}
		skp->Text(CW * 22, CH * 27, G->EMPErrorMessage.c_str(), G->EMPErrorMessage.size());
		skp->Text(CW, 4 * H / 14, G->EMPFile.c_str(), G->EMPFile.size());
		sprintf(Buffer, "%d/%d", G->EMPUplinkNumber, G->EMPUplinkMaxNumber);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));

		skp->Text(CW * 10, CH * 4, "Description:", 12);
		skp->Text(CW * 23, CH * 4, G->EMPDescription.c_str(), G->EMPDescription.size());
		skp->Text(CW * 10, CH * 5, "Rope:", 5);
		skp->Text(CW * 23, CH * 5, G->EMPRope.c_str(), G->EMPRope.size());
		skp->Text(CW * 18, CH * 7, "OID", 3);
		skp->Text(CW * 28, CH * 7, "FCT", 3);
		if (block->IsVerb72)
		{
			skp->Text(CW * 38, CH * 7, "DSKY V72", 8);
		}
		else
		{
			skp->Text(CW * 38, CH * 7, "DSKY V71", 8);
		}
		for (int i = 1; i <= 024; i++)
		{
			sprintf(Buffer, "%02o", i);
			skp->Text(CW * 18, CH * (i + 7), Buffer, strlen(Buffer));
		}
		skp->Text(CW * 28, CH * 8, "INDEX", 5);
		sprintf(Buffer, "%05o", block->Index);
		skp->Text(CW * 38, CH * 8, Buffer, strlen(Buffer));
		for (int i = 0; i <= 8; i++)
		{
			skp->Text(CW * 28, CH * (9 + 2 * i), "ADD", 3);

			if (block->IsVerb72 == false) break;
		}
		for (int i = 0; i < 19; i++)
		{
			if (block->Data[i].OctalData != 0x8000)
			{
				sprintf(Buffer, "%05o", block->Data[i].OctalData);
				skp->Text(CW * 38, CH * (i + 9), Buffer, strlen(Buffer));
			}

			if (block->Data[i].EndOfDataFlag) break;
		}
	}
		break;
	case 25:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Nav Check PAD", 13);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		CSMOrLMSelection(skp);
		GET_Display2(Buffer, G->navcheckpad.NavChk[0]);
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.2f LAT", G->navcheckpad.lat[0]);
		skp->Text(CW * 20, CH * 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.2f LNG", G->navcheckpad.lng[0]);
		skp->Text(CW * 20, CH * 15, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f ALT", G->navcheckpad.alt[0]);
		skp->Text(CW * 20, CH * 16, Buffer, strlen(Buffer));
		break;
	case 26:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Deorbit", 7);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (GC->rtcc->RZJCTTC.R32_Code == 1)
		{
			skp->Text(CW, 2 * H / 14, "1: No Sep/Shaping", 17);
		}
		else
		{
			skp->Text(CW, 2 * H / 14, "2: With Sep/Shaping", 19);
		}
		GET_Display(Buffer, GC->rtcc->RZJCTTC.R32_GETI);
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->RZJCTTC.R32_lat_T <= -720.0*RAD)
		{
			sprintf(Buffer, "No latitude iteration");
		}
		else
		{
			sprintf(Buffer, "%f°", GC->rtcc->RZJCTTC.R32_lat_T*DEG);
		}
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f°", GC->rtcc->RZJCTTC.R32_lng_T*DEG);
		skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2lf NM", GC->rtcc->RZJCTTC.R32_MD);
		skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		skp->Text(W - CW, 2 * H / 14, "Retrofire Digitals", 18);
		skp->Text(W - CW, 4 * H / 14, "Retrofire External DV", 21);
		skp->Text(W - CW, 6 * H / 14, "Retrofire Separation", 20);
		break;
	case 27:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Return to Earth Digitals Inputs", 31);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (GC->rtcc->med_f80.Column == 1)
		{
			skp->Text(CW, 2 * H / 14, "Primary", 7);
		}
		else
		{
			skp->Text(CW, 2 * H / 14, "Manual", 6);
		}
		sprintf(Buffer, "%d", GC->rtcc->med_f80.ASTCode);
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%s", GC->rtcc->med_f80.REFSMMAT.c_str());
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%s", GC->rtcc->med_f80.ManeuverCode.c_str());
		skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->med_f80.ManeuverCode.size() > 2 && GC->rtcc->med_f80.ManeuverCode[1] == 'R')
		{
			sprintf(Buffer, "RCS Burn: %+d quads", GC->rtcc->med_f80.NumQuads);
		}
		else
		{
			sprintf(Buffer, "%+d quads %.1lf seconds", GC->rtcc->med_f80.NumQuads, GC->rtcc->med_f80.UllageDT);
		}
		skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->med_f80.TrimAngleInd == -1)
		{
			skp->Text(CW, 12 * H / 14, "Compute trim gimbals", 20);
		}
		else
		{
			skp->Text(CW, 12 * H / 14, "Use system parameter", 20);
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		sprintf(Buffer, "%.2lf°", GC->rtcc->med_f80.DockingAngle*DEG);
		skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->med_f80.HeadsUp)
		{
			skp->Text(W - CW, 6 * H / 14, "Heads Up", 8);
		}
		else
		{
			skp->Text(W - CW, 6 * H / 14, "Heads Down", 10);
		}
		if (GC->rtcc->med_f80.Iterate)
		{
			skp->Text(W - CW, 8 * H / 14, "Iterate", 7);
		}
		else
		{
			skp->Text(W - CW, 8 * H / 14, "Don't iterate", 13);
		}
		break;
	case 28:
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		skp->SetPen(pen2);
		Text(skp, 14, 0, "RETURN TO EARTH DIGITALS");
		Text(skp, 51, 0, "0363");
		Text(skp, 1, 1, "GETR");
		Text(skp, 17, 1, "CM WT");
		Text(skp, 31, 1, "K FAC");
		Text(skp, 42, 1, "STAID");
		Text(skp, 13, 2, "PRIMARY");
		Text(skp, 24, 2, "CODE");
		Text(skp, 35, 2, "MANUAL");
		Text(skp, 46, 2, "CODE");
		Line(skp, 25 * CW / 2, CH * 3, 25 * CW / 2, CH * 28);
		Line(skp, CW * 34, CH * 3, CW * 34, CH * 28);
		Text(skp, 1, 3, "STA ID  AM");
		Text(skp, 2, 4, "GETV");
		Text(skp, 2, 5, "AREA THR");
		Text(skp, 2, 6, "MATRIX WT");
		Text(skp, 2, 7, "TAA EP");
		Text(skp, 2, 8, "RL PL YL");
		Text(skp, 2, 9, "RO PI YM");
		Text(skp, 2, 10, "VC BT");
		Text(skp, 2, 11, "VT U DT");
		Text(skp, 2, 12, "PETI");
		Text(skp, 2, 13, "GETI");
		Text(skp, 2, 14, "GMTI");
		Text(skp, 1, 15, "BU PETIR LV");
		Text(skp, 3, 17, "GIR/GCON");
		Text(skp, 2, 18, "GMAX");
		Text(skp, 2, 19, "PETEI");
		Text(skp, 1, 20, "VEI GEI");
		Text(skp, 1, 21, "LAT LNG EI");
		Text(skp, 1, 22, "LAT LNG ML2");
		Text(skp, 1, 23, "LAT LNG T");
		Text(skp, 1, 24, "LAT LNG ZL2");
		Text(skp, 1, 25, "LAT LNG IPB");
		Text(skp, 1, 26, "GETL  MD");
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text_GET_HHHMMSS(skp, 15, 1, GC->rtcc->SystemParameters.MCGREF*3600.0);
		Text(skp, 30, 1, "%.1lf", GC->rtcc->SystemParameters.MCVCMW *LBS*1000.0);
		Text(skp, 41, 1, "%.2lf", 1.0); //TBD
		{
			RTEDigitalSolutionTable *tab;
			for (int i = 0; i < 2; i++)
			{
				tab = &GC->rtcc->PZREAP.RTEDTable[i];
				if (tab->Error)
				{
					sprintf_s(Buffer, "Error: %d", tab->Error);
					Text(skp, 32 + 22 * i, 13, Buffer);
				}
				if (tab->RTEDCode == "") continue;
				Text(skp, 33 + 22 * i, 2, tab->RTEDCode);
				Text(skp, 26 + 22 * i, 3, tab->StationID);
				Text(skp, 33 + 22 * i, 3, tab->ASTSolutionCode);
				Text_GET_HHHMMSS(skp, 27 + 22 * i, 4, tab->VectorGET);
				Text(skp, 21 + 22 * i, 5, tab->LandingSiteID);
				Text(skp, 28 + 22 * i, 5, tab->ManeuverCode);
				Text(skp, 23 + 22 * i, 6, tab->SpecifiedREFSMMAT);
				Text(skp, 29 + 22 * i, 6, "%.0lf", tab->VehicleWeight *LBS*1000.0);
				Text(skp, 23 + 22 * i, 7, "%.0lf", tab->TrueAnomaly*DEG);
				Text(skp, 27 + 22 * i, 7, tab->PrimaryReentryMode);
				Text(skp, 19 + 22 * i, 8, "%.1lf", tab->LVLHAtt.x*DEG);
				Text(skp, 25 + 22 * i, 8, "%.1lf", tab->LVLHAtt.y*DEG);
				Text(skp, 31 + 22 * i, 8, "%.1lf", tab->LVLHAtt.z*DEG);
				FormatIMUAngle1(Buffer, tab->IMUAtt.x); Text(skp, 19 + 22 * i, 9, Buffer);
				FormatIMUAngle1(Buffer, tab->IMUAtt.y); Text(skp, 25 + 22 * i, 9, Buffer);
				FormatIMUAngle1(Buffer, tab->IMUAtt.z); Text(skp, 31 + 22 * i, 9, Buffer);
				Text(skp, 22 + 22 * i, 10, "%.1lf", tab->DVC / 0.3048);
				Text_GET_MMSSC(skp, 31 + 22 * i, 10, tab->dt);
				Text(skp, 22 + 22 * i, 11, "%.1lf", tab->dv / 0.3048);
				Text(skp, 25 + 22 * i, 11, "%+d", tab->NumQuads);
				Text_GET_MMSS(skp, 31 + 22 * i, 11, tab->dt_ullage);
				Text_GET_HHHMMSS(skp, 27 + 22 * i, 12, tab->PETI);
				Text_GET_HHHMMSSCS(skp, 30 + 22 * i, 13, tab->GETI);
				Text_GET_HHHMMSSCS(skp, 30 + 22 * i, 14, tab->GMTI);
				Text(skp, 16 + 22 * i, 15, tab->BackupReentryMode);
				Text_GET_HHHMMSS(skp, 26 + 22 * i, 15, tab->RollPET);
				Text(skp, 33 + 22 * i, 15, "%.1lf", tab->LiftVectorOrientation*DEG);
				Text(skp, 23 + 22 * i, 17, "%.2lf", tab->GLevelRoll);
				Text(skp, 23 + 22 * i, 18, "%.2lf", tab->MaxGLevelPrimary);
				Text_GET_HHHMMSS(skp, 27 + 22 * i, 19, tab->ReentryPET);
				Text(skp, 23 + 22 * i, 20, "%.0lf", tab->v_EI / 0.3048);
				Text(skp, 30 + 22 * i, 20, "%.2lf", tab->gamma_EI*DEG);
				Text_Latitude(skp, 23 + 22 * i, 21, tab->lat_EI*DEG);
				Text_Longitude(skp, 31 + 22 * i, 21, tab->lng_EI*DEG);
				Text_Latitude(skp, 23 + 22 * i, 22, tab->lat_imp_2nd_max*DEG);
				Text_Longitude(skp, 31 + 22 * i, 22, tab->lng_imp_2nd_max*DEG);
				Text_Latitude(skp, 23 + 22 * i, 23, tab->lat_imp_tgt*DEG);
				Text_Longitude(skp, 31 + 22 * i, 23, tab->lng_imp_tgt*DEG);
				Text_Latitude(skp, 23 + 22 * i, 24, tab->lat_imp_2nd_min*DEG);
				Text_Longitude(skp, 31 + 22 * i, 24, tab->lng_imp_2nd_min*DEG);
				Text_Latitude(skp, 23 + 22 * i, 25, tab->lat_imp_bu*DEG);
				Text_Longitude(skp, 31 + 22 * i, 25, tab->lng_imp_bu*DEG);
				Text_GET_HHHMMSS(skp, 22 + 22 * i, 26, tab->ImpactGET_prim);
				if (tab->md_lat > 0)
				{
					sprintf_s(Buffer, "%.0lfN", tab->md_lat / 1852.0);
				}
				else
				{
					sprintf_s(Buffer, "%.0lfS", abs(tab->md_lat / 1852.0));
				}
				Text(skp, 29 + 22 * i, 26, Buffer);
				if (tab->md_lng > 0)
				{
					sprintf_s(Buffer, "%.0lfE", tab->md_lng / 1852.0);
				}
				else
				{
					sprintf_s(Buffer, "%.0lfW", abs(tab->md_lng / 1852.0));
				}
				Text(skp, 33 + 22 * i, 26, Buffer);
			}
		}
		break;
	case 29:
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 16, 0, "RETURN TO EARTH TARGET");
		Text(skp, 51, 0, "0366");
		Text(skp, 2, 4, "CONSTRAINTS");
		Text(skp, 2, 6, "DVMAX");
		Text(skp, 2, 8, "TZMIN");
		Text(skp, 2, 10, "TZMAX");
		Text(skp, 2, 12, "GMAX");
		Text(skp, 2, 14, "HMINMC");
		Text(skp, 2, 16, "IRMAX");
		Text(skp, 2, 18, "RRBIAS");
		Text(skp, 2, 20, "VRMAX");
		Text(skp, 2, 22, "MOTION");
		Text(skp, 2, 24, "TGTLN");
		Text(skp, 2, 26, "VECID");
		Text(skp, 35, 4, "ATP");
		Text(skp, 35, 18, "PTP");
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text(skp, 18, 6, "%.0f", GC->rtcc->PZREAP.DVMAX);
		Text_GET_HHHMMSS(skp, 18, 8, GC->rtcc->PZREAP.TZMIN*3600.0);
		Text_GET_HHHMMSS(skp, 18, 10, GC->rtcc->PZREAP.TZMAX*3600.0);
		Text(skp, 18, 12, "%.1f", GC->rtcc->PZREAP.GMAX);
		Text(skp, 18, 14, "%.1f", GC->rtcc->PZREAP.HMINMC);
		Text(skp, 18, 16, "%.2f", GC->rtcc->PZREAP.IRMAX);
		Text(skp, 18, 18, "%.0f", GC->rtcc->PZREAP.RRBIAS);
		Text(skp, 18, 20, "%.0f", GC->rtcc->PZREAP.VRMAX);
		if (GC->rtcc->PZREAP.MOTION == 0)
		{
			Text(skp, 18, 22, "EITHER");
		}
		else if (GC->rtcc->PZREAP.MOTION == 1)
		{
			Text(skp, 18, 22, "DIRECT");
		}
		else
		{
			Text(skp, 18, 22, "CIRCUM");
		}
		if (GC->rtcc->PZREAP.TGTLN == 0)
		{
			Text(skp, 18, 24, "SHALLOW", 7);
		}
		else
		{
			Text(skp, 18, 24, "STEEP", 5);
		}
		for (unsigned i = 0; i < 5; i++)
		{
			//If first element is not valid, skip this ATP
			if (GC->rtcc->PZREAP.ATPCoordinates[i][0] >= 1e9)
			{
				continue;
			}

			Text(skp, 26 + i * 7, 6, GC->rtcc->PZREAP.ATPSite[i]);
			for (unsigned j = 0; j < 5; j++)
			{
				//If current element isn't valid, skip the rest
				if (GC->rtcc->PZREAP.ATPCoordinates[i][2 * j] >= 1e9)
				{
					break;
				}

				Text(skp, 27 + i * 7, 7 + j * 2, "%.2f", GC->rtcc->PZREAP.ATPCoordinates[i][2 * j] * DEG);
				Text(skp, 27 + i * 7, 8 + j * 2, "%.2f", GC->rtcc->PZREAP.ATPCoordinates[i][2 * j + 1] * DEG);
			}
		}
		for (unsigned i = 0; i < 5; i++)
		{
			//If name is not valid, skip this PTP
			if (GC->rtcc->PZREAP.PTPSite[i] == "")
			{
				continue;
			}

			Text(skp, 26 + i * 7, 20, GC->rtcc->PZREAP.PTPSite[i]);
			Text(skp, 27 + i * 7, 21, "%.2f", GC->rtcc->PZREAP.PTPLatitude[i] * DEG);
			Text(skp, 27 + i * 7, 22, "%.2f", GC->rtcc->PZREAP.PTPLongitude[i] * DEG);
		}
		break;
	case 30:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Entry Update", 12);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		sprintf(Buffer, "Desired Range: %.1f NM", G->entryrange);
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "Lat:  %.2f °", GC->rtcc->RZDBSC1.lat_T*DEG);
		skp->Text(CW, 5 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "Long: %.2f °", GC->rtcc->RZDBSC1.lng_T*DEG);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		break;
	case 31:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "RTCC Files", 10);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "Constants:", 10);
		skp->Text(CW, 4 * H / 14, "TLI:", 4);
		skp->Text(CW, 6 * H / 14, "SFP:", 4);
		skp->Text(CW, 8 * H / 14, "Init:", 5);
		skp->Text(CW * 12, 2 * H / 14, GC->rtcc->SystemParametersFile.c_str(), GC->rtcc->SystemParametersFile.size());
		skp->Text(CW * 12, 4 * H / 14, GC->rtcc->TLIFile.c_str(), GC->rtcc->TLIFile.size());
		skp->Text(CW * 12, 6 * H / 14, GC->rtcc->SFPFile.c_str(), GC->rtcc->SFPFile.size());
		sprintf(Buffer, "%d-%02d-%02d Init", GC->rtcc->GZGENCSN.Year, GC->rtcc->GZGENCSN.MonthofLiftoff, GC->rtcc->GZGENCSN.DayofLiftoff);
		skp->Text(CW * 12, 8 * H / 14, Buffer, strlen(Buffer));
		switch (status)
		{
		case 1:
			skp->Text(CW * 12, 13 * H / 14, "File loaded!", 12);
			break;
		case 2:
			skp->Text(CW * 12, 13 * H / 14, "File not found!", 15);
			break;
		case 3:
			skp->Text(CW * 12, 13 * H / 14, "Error loading file!", 19);
			break;
		}
		break;
	case 32:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Rendezvous Processors", 22);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "Two Impulse Multiple Solution", 29);
		skp->Text(CW, 4 * H / 14, "Two Impulse Corrective Combination", 34);
		skp->Text(CW, 6 * H / 14, "Two Impulse Single Solution", 27);
		skp->Text(CW, 8 * H / 14, "Coelliptic Sequence Processor", 29);
		skp->Text(CW, 10 * H / 14, "Docking Initiation Processor", 28);
		skp->Text(CW, 12 * H / 14, "TPI Times", 9);
		break;
	case 33:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Docking Initiate", 16);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "TIG and TPI Definition", 22);
		skp->Text(CW, 4 * H / 14, "Init Parameters", 15);
		if (GC->rtcc->med_k00.NC1 > 0)
		{
			sprintf(Buffer, "%.2f", GC->rtcc->med_k00.NC1);
		}
		else
		{
			sprintf(Buffer, "No NC1 maneuver");
		}
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f", GC->rtcc->med_k00.NH);
		skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->med_k00.I4)
		{
			sprintf(Buffer, "NCC: %.2f", GC->rtcc->med_k00.NCC);
		}
		else
		{
			sprintf(Buffer, "%.2f", GC->rtcc->med_k00.NSR);
		}
		skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.0f", GC->rtcc->med_k00.MI);
		skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		sprintf(Buffer, "%d", GC->rtcc->med_k00.IDM);
		skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->med_k00.IDM > 0)
		{
			if (GC->rtcc->med_k00.MNH)
			{
				skp->Text(W - CW, 6 * H / 14, "Relative to NSR", 15);
			}
			else
			{
				skp->Text(W - CW, 6 * H / 14, "Same place", 10);
			}
		}
		if (GC->rtcc->med_k00.NPC > 0)
		{
			sprintf(Buffer, "%.2f", GC->rtcc->med_k00.NPC);
			skp->Text(W - CW, 8 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(W - CW, 8 * H / 14, "No NPC maneuver", 15);
		}
		break;
	case 34:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "DKI TIG and TPI Definition", 26);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (GC->rtcc->med_k00.ChaserVehicle == RTCC_MPT_CSM)
		{
			skp->Text(CW, 2 * H / 14, "CSM", 3);
		}
		else
		{
			skp->Text(CW, 2 * H / 14, "LM", 2);
		}
		if (GC->rtcc->med_k00.I4)
		{
			skp->Text(CW, 4 * H / 14, "Skylab Rendezvous", 17);
		}
		else
		{
			skp->Text(CW, 4 * H / 14, "Regular DKI", 11);
		}
		if (GC->rtcc->med_k10.MLDOption == 1)
		{
			skp->Text(CW, 6 * H / 14, "ML at input time", 16);

			GET_Display(Buffer, GC->rtcc->med_k10.MLDTime, false);
			skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			if (GC->rtcc->med_k10.MLDOption == 2)
			{
				skp->Text(CW, 6 * H / 14, "ML at chaser apoapsis", 21);
			}
			else
			{
				skp->Text(CW, 6 * H / 14, "ML at target apogee", 19);
			}
			skp->Text(CW, 7 * H / 14, "Threshold time:", 15);
			if (GC->rtcc->med_k10.MLDTime == 0.0)
			{
				sprintf(Buffer, "Current Time");
			}
			else
			{
				GET_Display(Buffer, GC->rtcc->med_k10.MLDTime, false);
			}
			skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		}
		sprintf(Buffer, "%.1lf", GC->rtcc->med_k10.MLDValue);
		skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%d", GC->rtcc->GZGENCSN.DKIPhaseAngleSetting);
		skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (GC->rtcc->GZGENCSN.DKI_TP_Definition == 0)
		{
			sprintf(Buffer, "Input TPI phase angle");
			skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));

			sprintf(Buffer, "%.1lf", GC->rtcc->GZGENCSN.DKI_TPDefinitionValue);
			skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			switch (GC->rtcc->GZGENCSN.DKI_TP_Definition)
			{
			case 1:
				sprintf(Buffer, "Input TPI time");
				break;
			case 2:
				sprintf(Buffer, "Input TPF time");
				break;
			case 3:
				sprintf(Buffer, "TPI at X minutes into night");
				break;
			case 4:
				sprintf(Buffer, "TPI at X minutes into day");
				break;
			case 5:
				sprintf(Buffer, "TPF at X minutes into night");
				break;
			case 6:
				sprintf(Buffer, "TPF at X minutes into day");
				break;
			default:
				sprintf(Buffer, "");
				break;
			}
			skp->Text(W - CW / 16, 2 * H / 14, Buffer, strlen(Buffer));

			if (GC->rtcc->GZGENCSN.DKI_TP_Definition == 1 || GC->rtcc->GZGENCSN.DKI_TP_Definition == 2)
			{
				GET_Display(Buffer, GC->rtcc->GZGENCSN.DKI_TPDefinitionValue, false);
				skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
			}
			else
			{
				sprintf(Buffer, "%.0lf min", GC->rtcc->GZGENCSN.DKI_TPDefinitionValue);
				skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
			}
		}
		if (GC->MissionPlanningActive)
		{
			//TBD: Chaser and target vector IDs
		}
		else
		{
			//TBD: Chaser vs. target instead of CSM and LM?
			PrintCSMVessel(Buffer);
			skp->Text(W - CW, 8 * H / 14, Buffer, strlen(Buffer));
			PrintLMVessel(Buffer);
			skp->Text(W - CW, 10 * H / 14, Buffer, strlen(Buffer));
		}
		break;
	case 35:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "DAP PAD", 7);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		CSMOrLMSelection(skp);
		if (G->vesselisdocked)
		{
			skp->Text(CW, 4 * H / 14, "Docked", 6);
		}
		else
		{
			skp->Text(CW, 4 * H / 14, "Undocked", 8);
		}
		if (G->lemdescentstage)
		{
			skp->Text(CW, 6 * H / 14, "Full LM", 7);
		}
		else
		{
			skp->Text(CW, 6 * H / 14, "Ascent Stage", 12);
		}
		sprintf(Buffer, "%+06.0f WT N47", G->DAP_PAD.ThisVehicleWeight);
		skp->Text(CW * 22, CH * 18, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+06.0f", G->DAP_PAD.OtherVehicleWeight);
		skp->Text(CW * 22, CH * 19, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.2f GMBL N48", G->DAP_PAD.PitchTrim);
		skp->Text(CW * 22, CH * 20, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.2f", G->DAP_PAD.YawTrim);
		skp->Text(CW * 22, CH * 21, Buffer, strlen(Buffer));
		break;
	case 36:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "LVDC", 4);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		sprintf(Buffer, "Launch Azimuth: %.4f°", G->LVDCLaunchAzimuth*DEG);
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		break;
	case 37:
		if (subscreen == 0)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text(W / 2, CH / 2, "TI Corrective Combination (K32)", 31);
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			x = 1;  y = 3; dx = 7;
			Text(skp, x, marker + y, "*");
			x++;
			Text(skp, x, y, "VEH:");
			if (GC->rtcc->med_k32.Vehicle == 1) Text(skp, x + dx, y, "CSM");
			else Text(skp, x + dx, y, "LEM");
			y++;
			Text(skp, x, y, "REQ:");
			if (GC->rtcc->med_k32.RequestIndicator == 0) Text(skp, x + dx, y, "0: Second maneuver time varied");
			else Text(skp, x + dx, y, "1: Second maneuver time fixed");
			y++;
			Text(skp, x, y, "CVT:");
			if (GC->MissionPlanningActive)
			{
				if (GC->rtcc->med_k32.ChaserVectorTime > 0) Text_GET_HHHMMSSCS(skp, x + dx, y, GC->rtcc->med_k32.ChaserVectorTime);
				else Text(skp, x + dx, y, "Present Time");
				y++;
			}
			else
			{
				if (GC->rtcc->med_k32.Vehicle == 1) PrintCSMVessel(Buffer);
				else PrintLMVessel(Buffer);
				Text(skp, x + dx, y, Buffer); y++;
			}
			Text(skp, x, y, "TVT:");
			if (GC->MissionPlanningActive)
			{
				if (GC->rtcc->med_k32.TargetVectorTime > 0) Text_GET_HHHMMSSCS(skp, x + dx, y, GC->rtcc->med_k32.TargetVectorTime);
				else Text(skp, x + dx, y, "Present Time");
				y++;
			}
			else
			{
				if (GC->rtcc->med_k32.Vehicle == 1) PrintLMVessel(Buffer);
				else PrintCSMVessel(Buffer);
				Text(skp, x + dx, y, Buffer); y++;
			}
			Text(skp, x, y, "NCC:");
			Text_GET_HHHMMSSCS(skp, x + dx, y, GC->rtcc->med_k32.T_NCC); y++;
			Text(skp, x, y, "DHMIN:");
			Text(skp, x + dx, y, "%.1lf NM", GC->rtcc->med_k32.DH_min); y++;
			Text(skp, x, y, "DHMAX:");
			Text(skp, x + dx, y, "%.1lf NM", GC->rtcc->med_k32.DH_max); y++;
			Text(skp, x, y, "DHINC:");
			Text(skp, x + dx, y, "%.1lf NM", GC->rtcc->med_k32.DH_inc); y++;
			Text(skp, x, y, "T2MIN:");
			Text_GET_HHHMMSSCS(skp, x + dx, y, GC->rtcc->med_k32.T2_min); y++;
			if (GC->rtcc->med_k32.RequestIndicator == 0)
			{
				Text(skp, x, y, "T2MAX:");
				Text_GET_HHHMMSSCS(skp, x + dx, y, GC->rtcc->med_k32.T2_max);
			}
			y++;
			Text(skp, x, y, "DT:");
			Text(skp, x + dx, y, "%.1lf min", GC->rtcc->med_k32.TimeStep); y++;
			if (GC->rtcc->med_k32.RequestIndicator == 1)
			{
				Text(skp, x, y, "SLIP:");
				Text(skp, x + dx, y, "%.1lf min", GC->rtcc->med_k32.dt_TPI_slip);
			}
			Text(skp, 1, 21, "NSR NOMINALS:");
			Text(skp, 1, 22, "GET");
			Text(skp, 1, 23, "DEL H");
			Text(skp, 1, 24, "PHASE");
			Text_GMT_HHHMMSSCS(skp, 7, 22, GC->rtcc->GZGENCSN.TINSRNominalTime);
			Text(skp, 7, 23, " %.2lf NM", GC->rtcc->GZGENCSN.TINSRNominalDeltaH / 1852.0);
			Text(skp, 7, 24, " %.2lf deg", GC->rtcc->GZGENCSN.TINSRNominalPhaseAngle*DEG);
		}
		else
		{
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			SetMOCRFont(skp, 3, false);
			GetCharSize(skp, CW, CH);
			SetMOCRDisplayCentered(3);
			Text(skp, 18, 0, "TWO IMPULSE DIGITALS");
			Text(skp, 52, 0, "0064");
			Text(skp, 0, 2, "LM STA ID");
			Text(skp, 0, 3, "LM GETTHS");
			Text(skp, 0, 4, "GETNCC");
			Text(skp, 0, 5, "GMTNCC");
			Text(skp, 36, 2, "CSM STA ID");
			Text(skp, 36, 3, "CSM GETTHS");
			Text(skp, 39, 4, "MAN VEH");
			Text(skp, 0, 7, "CODE  GETNSR    GMTNSR    DVT     DH     DP    DT  TSLIP");
			SetMOCRFont(skp, 3, true);
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			Text(skp, 19, 2, GC->rtcc->TwoImpCCDispBuffer.LMSTAID);
			Text_GET_HHHMMSS(skp, 19, 3, GC->rtcc->TwoImpCCDispBuffer.GETTH_LM);
			Text_GET_HHHMMSS(skp, 19, 4, GC->rtcc->TwoImpCCDispBuffer.GET_NCC);
			Text_GET_HHHMMSS(skp, 19, 5, GC->rtcc->TwoImpCCDispBuffer.GMT_NCC);
			Text(skp, 56, 2, GC->rtcc->TwoImpCCDispBuffer.CSMSTAID);
			Text_GET_HHHMMSS(skp, 56, 3, GC->rtcc->TwoImpCCDispBuffer.GETTH_CSM);
			Text(skp, 56, 4, GC->rtcc->TwoImpCCDispBuffer.MAN_VEH);
			for (int i = 0; i < GC->rtcc->TwoImpCCDispBuffer.Solutions; i++)
			{
				Text(skp, 3, 8 + i, "%d", GC->rtcc->TwoImpCCDispBuffer.data[i].Code);
				Text_GET_HHHMMSS(skp, 13, 8 + i, GC->rtcc->TwoImpCCDispBuffer.data[i].GET_NSR);
				Text_GET_HHHMMSS(skp, 23, 8 + i, GC->rtcc->TwoImpCCDispBuffer.data[i].GMT_NSR);
				Text(skp, 30, 8 + i, "%.1lf", GC->rtcc->TwoImpCCDispBuffer.data[i].DVT);
				Text(skp, 37, 8 + i, "%.2lf", GC->rtcc->TwoImpCCDispBuffer.data[i].DH);
				Text(skp, 44, 8 + i, "%.2lf", GC->rtcc->TwoImpCCDispBuffer.data[i].PhaseAngle);
				Text(skp, 50, 8 + i, "%.1lf", GC->rtcc->TwoImpCCDispBuffer.data[i].DT);
				Text(skp, 56, 8 + i, "%.1lf", GC->rtcc->TwoImpCCDispBuffer.data[i].TSLIP);
			}
			Text(skp, 45, 27, GC->rtcc->TwoImpCCDispBuffer.ErrorMessage);
		}
		break;
	case 38:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Lunar Ascent Processor", 22);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		GET_Display2(Buffer, G->t_LunarLiftoff);
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f ft/s", GC->rtcc->PZLTRT.InsertionHorizontalVelocity / 0.3048);
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+07.1f ft/s", GC->rtcc->PZLTRT.InsertionRadialVelocity / 0.3048);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));

		skp->Text(CW, CH * 15, "Cross range:", 12);
		sprintf(Buffer, "%.3f NM", G->LAP_CR / 1852.0);
		skp->Text(CW, CH * 16, Buffer, strlen(Buffer));
		skp->Text(CW, CH * 17, "Powered Flight Arc:", 19);
		sprintf(Buffer, "%.3f°", GC->rtcc->PZLTRT.PoweredFlightArc*DEG);
		skp->Text(CW, CH * 18, Buffer, strlen(Buffer));
		skp->Text(CW, CH * 19, "Powered Flight Time:", 20);
		sprintf(Buffer, "%.1f s", GC->rtcc->PZLTRT.PoweredFlightTime);
		skp->Text(CW, CH * 20, Buffer, strlen(Buffer));
		skp->Text(CW, CH * 21, "Insertion GET:", 14);
		{
			double get = GC->rtcc->GETfromGMT(GC->rtcc->JZLAI.sv_Insertion.GMT);
			if (get < 0)
			{
				get = 0.0;
			}
			GET_Display(Buffer, get, false);
			skp->Text(CW, CH * 22, Buffer, strlen(Buffer));
		}
		skp->Text(CW, CH * 23, "Phase Angle:", 12);
		sprintf(Buffer, "%.3f°", G->LAP_Phase*DEG);
		skp->Text(CW, CH * 24, Buffer, strlen(Buffer));
		skp->Text(W - CW * 17, CH * 18, "X", 1);
		skp->Text(W - CW * 17, CH * 19, "Y", 1);
		skp->Text(W - CW * 17, CH * 20, "Z", 1);
		skp->Text(W - CW * 17, CH * 21, "XD", 2);
		skp->Text(W - CW * 17, CH * 22, "YD", 2);
		skp->Text(W - CW * 17, CH * 23, "ZD", 2);
		skp->Text(W - CW * 17, CH * 24, "T", 1);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (!GC->MissionPlanningActive)
		{
			PrintLMVessel(Buffer);
			skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));
			PrintCSMVessel(Buffer);
			skp->Text(W - CW, 3 * H / 14, Buffer, strlen(Buffer));
		}
		sprintf(Buffer, "%f", GC->rtcc->JZLAI.sv_Insertion.R.x);
		skp->Text(W - CW, CH * 18, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", GC->rtcc->JZLAI.sv_Insertion.R.y);
		skp->Text(W - CW, CH * 19, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", GC->rtcc->JZLAI.sv_Insertion.R.z);
		skp->Text(W - CW, CH * 20, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", GC->rtcc->JZLAI.sv_Insertion.V.x);
		skp->Text(W - CW, CH * 21, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", GC->rtcc->JZLAI.sv_Insertion.V.y);
		skp->Text(W - CW, CH * 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", GC->rtcc->JZLAI.sv_Insertion.V.z);
		skp->Text(W - CW, CH * 23, Buffer, strlen(Buffer));
		sprintf(Buffer, "%f", GC->rtcc->JZLAI.sv_Insertion.GMT);
		skp->Text(W - CW, CH * 24, Buffer, strlen(Buffer));
		break;
	case 39:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "LM Ascent PAD", 13);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (G->AscentPADVersion == 1)
		{
			sprintf(Buffer, "Apollo 14-17");
		}
		else
		{
			sprintf(Buffer, "Apollo 11-13");
		}
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));

		OrbMech::SStoHHMMSS(G->t_LunarLiftoff, hh, mm, secs, 0.01);
		Text(skp, 12, 11, "%+06d HRS", hh);
		Text(skp, 12, 12, "%+06d MIN TIG", mm);
		Text(skp, 12, 13, "%+07.2f SEC", secs);
		Text(skp, 12, 14, "%+07.1f V (HOR)", GC->rtcc->PZLTRT.InsertionHorizontalVelocity / 0.3048);
		Text(skp, 12, 15, "%+07.1f V (VERT) N76", GC->rtcc->PZLTRT.InsertionRadialVelocity / 0.3048);
		Text(skp, 12, 16, "%+07.1f CROSSRANGE", G->lmascentpad.CR);
		Text(skp, 12, 17, "%+06d DEDA 047", G->lmascentpad.DEDA047);
		Text(skp, 12, 18, "%+06d DEDA 053", G->lmascentpad.DEDA053);
		if (G->AscentPADVersion == 1)
		{
			sprintf(Buffer, "%+06.0f DEDA 224/226", G->lmascentpad.DEDA225_226);
		}
		else
		{
			sprintf(Buffer, "%+06.0f DEDA 225/226", G->lmascentpad.DEDA225_226);
		}
		skp->Text(CW * 12, CH * 19, Buffer, strlen(Buffer));
		Text(skp, 12, 20, "%+06.0f DEDA 231", G->lmascentpad.DEDA231);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		PrintCSMVessel(Buffer);
		skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
		break;
	case 40:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Powered Descent Abort Program", 29);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		if (subscreen == 0)
		{
			x = 1;  y = 3; dx = 8;
			Text(skp, x, marker + y, "*");
			x++;
			Text(skp, x, y, "MIS:");
			if (GC->PDAPVersion == 0) Text(skp, x + dx, y, "Apollo 11");
			else if (GC->PDAPVersion == 1) Text(skp, x + dx, y, "Apollo 12");
			else Text(skp, x + dx, y, "Apollo 13+");
			y++;
			if (GC->PDAPVersion != 0)
			{
				Text(skp, x, y, "ENG:");
				if (GC->PDAPOptions.dt_stage != 0.0) Text(skp, x + dx, y, "DPS/APS");
				else Text(skp, x + dx, y, "APS");
			}
			y++;
			if (GC->MissionPlanningActive)
			{
				Text(skp, x, y, "CVT:");
				Text_GET_HHHMMSSCS(skp, x + dx, y, GC->PDAP_CSM_VectorTime);
			}
			else
			{
				Text(skp, x, y, "CSM:");
				PrintCSMVessel(Buffer, false);
				Text(skp, x + dx, y, Buffer);
			}
			y++;
			if (GC->MissionPlanningActive)
			{
				Text(skp, x, y, "LVT:");
				Text_GET_HHHMMSSCS(skp, x + dx, y, GC->PDAP_LM_VectorTime);
			}
			else
			{
				Text(skp, x, y, "LM:");
				PrintLMVessel(Buffer, false);
				Text(skp, x + dx, y, Buffer);
			}
			y++;
			Text(skp, x, y, "HAMIN:");
			Text(skp, x + dx, y, "%.1lf NM", GC->PDAPOptions.h_amin / 1852.0);
			y++;
			Text(skp, x, y, "DH:");
			Text(skp, x + dx, y, "%.1lf NM", GC->PDAPOptions.DH_D / 1852.0);
			y++;
			if (GC->PDAPVersion != 0)
			{
				Text(skp, x, y, "K4:");
				if (GC->PDAPOptions.K4) Text(skp, x + dx, y, "First segment phase angle limit");
				else Text(skp, x + dx, y, "First segment apolune limit");
				y++;
				if (GC->PDAPOptions.K4)
				{
					Text(skp, x, y, "PHA:");
					Text(skp, x + dx, y, "%.2lf deg", GC->PDAPOptions.theta_TARG*DEG);
				}
				y++;
			}
			else y += 2;
			Text(skp, x, y, "DTCAN:");
			Text(skp, x + dx, y, "%.1lf min", GC->PDAPOptions.dt_CAN / 60.0);
			y++;
			Text(skp, x, y, "DVCAN:");
			sprintf(Buffer, "%+.1lf %+.1lf %+.1lf", GC->PDAPOptions.DV_CAN.x / 0.3048, GC->PDAPOptions.DV_CAN.y / 0.3048, GC->PDAPOptions.DV_CAN.z / 0.3048);
			Text(skp, x + dx, y, Buffer);
			y++;
			Text(skp, x, y, "DTCSI:");
			Text(skp, x + dx, y, "%.1lf min", GC->PDAPOptions.dt_CSI / 60.0);
			y++;
			Text(skp, x, y, "TTPI:");
			Text_GET_HHHMMSSCS(skp, x + dx, y, GC->PDAPOptions.GMT_TPI);
			y++;
			if (GC->PDAPVersion != 0)
			{
				Text(skp, x, y, "DT2CAN:");
				Text(skp, x + dx, y, "%.1lf min", GC->PDAPOptions.dt_2CAN / 60.0);
				y++;
				Text(skp, x, y, "DV2CAN:");
				sprintf(Buffer, "%+.1lf %+.1lf %+.1lf", GC->PDAPOptions.DV_2CAN.x / 0.3048, GC->PDAPOptions.DV_2CAN.y / 0.3048, GC->PDAPOptions.DV_2CAN.z / 0.3048);
				Text(skp, x + dx, y, Buffer);
				y++;
				Text(skp, x, y, "DT2CSI:");
				Text(skp, x + dx, y, "%.1lf min", GC->PDAPOptions.dt_2CSI / 60.0);
				y++;
				Text(skp, x, y, "T2TPI:");
				Text_GET_HHHMMSSCS(skp, x + dx, y, GC->PDAPOptions.GMT_2TPI);
				y++;
			}
			else y += 4;
			Text(skp, x, y, "WTDRY:");
			Text(skp, x + dx, y, "%.1lf lbs", GC->PDAPOptions.W_TDRY / LBS2KG);
			y++;
			Text(skp, x, y, "WTAPS:");
			Text(skp, x + dx, y, "%.1lf lbs", GC->PDAPOptions.W_TAPS / LBS2KG);
			y++;

			skp->Text(W - CW * 15, CH * 19, "Landing Site:", 13);
			sprintf(Buffer, "%.3f°", GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST] * DEG);
			skp->Text(W - CW * 15, CH * 20, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.3f°", GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST] * DEG);
			skp->Text(W - CW * 15, CH * 21, Buffer, strlen(Buffer));
			sprintf(Buffer, "%.2f NM", GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST] / 1852.0);
			skp->Text(W - CW * 15, CH * 22, Buffer, strlen(Buffer));
			skp->Text(W - CW * 15, CH * 23, "TLAND:", 6);
			GET_Display(Buffer, GC->rtcc->CZTDTGTU.GETTD);
			skp->Text(W - CW * 15, CH * 24, Buffer, strlen(Buffer));
		}
		else
		{
			skp->SetFont(font_menu2);
			GetCharSize(skp, CW, CH);
			Text(skp, 17, 3, "PGNS COMMAND LOAD");
			if (GC->PDAPVersion == 0)
			{
				//Apollo 11
				Text(skp, 7, 5, "OCT1  OCT2  DIG. (M, CS)  DIG. (FT, S)");
				Text(skp, 1, 6, "DPS4");
				Text(skp, 1, 7, "DPS3");
				Text(skp, 1, 8, "DPS2");
				Text(skp, 1, 9, "DPS1");
				Text(skp, 1, 10, "APS4");
				Text(skp, 1, 11, "APS3");
				Text(skp, 1, 12, "APS2");
				Text(skp, 1, 13, "APS1");
				Text(skp, 1, 14, "VHMIN");

				for (int i = 0; i < 9; i++)
				{
					for (int j = 0; j < 2; j++)
					{
						Text(skp, 7 + j * 6, 6 + i, "%05d", GC->PDAP_UplinkData[i * 2 + j]);
					}
				}

				Text(skp, 19, 6, "%+e", GC->PDAPABTCOF[0] * pow(0.01, 4));
				Text(skp, 19, 7, "%+e", GC->PDAPABTCOF[1] * pow(0.01, 3));
				Text(skp, 19, 8, "%+e", GC->PDAPABTCOF[2] * pow(0.01, 2));
				Text(skp, 19, 9, "%+e", GC->PDAPABTCOF[3] * pow(0.01, 1));
				Text(skp, 19, 10, "%+e", GC->PDAPABTCOF[4] * pow(0.01, 4));
				Text(skp, 19, 11, "%+e", GC->PDAPABTCOF[5] * pow(0.01, 3));
				Text(skp, 19, 12, "%+e", GC->PDAPABTCOF[6] * pow(0.01, 2));
				Text(skp, 19, 13, "%+e", GC->PDAPABTCOF[7] * pow(0.01, 1));
				Text(skp, 19, 14, "%+e", GC->PDAP_V_hmin * pow(0.01, 1));

				Text(skp, 33, 6, "%+e", GC->PDAPABTCOF[0] / 0.3048);
				Text(skp, 33, 7, "%+e", GC->PDAPABTCOF[1] / 0.3048);
				Text(skp, 33, 8, "%+e", GC->PDAPABTCOF[2] / 0.3048);
				Text(skp, 33, 9, "%+e", GC->PDAPABTCOF[3] / 0.3048);
				Text(skp, 33, 10, "%+e", GC->PDAPABTCOF[4] / 0.3048);
				Text(skp, 33, 11, "%+e", GC->PDAPABTCOF[5] / 0.3048);
				Text(skp, 33, 12, "%+e", GC->PDAPABTCOF[6] / 0.3048);
				Text(skp, 33, 13, "%+e", GC->PDAPABTCOF[7] / 0.3048);
				Text(skp, 33, 14, "%+e", GC->PDAP_V_hmin / 0.3048);
			}
			else
			{
				Text(skp, 7, 5, "OCT1  OCT2     DIG");
				Text(skp, 1, 6, "J1");
				Text(skp, 1, 7, "K1");
				Text(skp, 1, 8, "J2");
				Text(skp, 1, 9, "K2");
				Text(skp, 1, 10, "THET");
				Text(skp, 1, 11, "RMIN");

				for (int i = 0; i < 6; i++)
				{
					for (int j = 0; j < 2; j++)
					{
						Text(skp, 7 + j * 6, 6 + i, "%05d", GC->PDAP_UplinkData[i * 2 + j]);
					}
				}

				Text(skp, 22, 6, "%.4f NM", GC->PDAP_J1 / 1852.0);
				Text(skp, 22, 7, "%.4f NM/DEG", GC->PDAP_K1 / 1852.0 / DEG);
				Text(skp, 22, 8, "%.4f NM", GC->PDAP_J2 / 1852.0);
				Text(skp, 22, 9, "%.4f NM/DEG", GC->PDAP_K2 / 1852.0 / DEG);
				Text(skp, 22, 10, "%.4f DEG", GC->PDAP_Theta_LIM*DEG);
				Text(skp, 22, 11, "%.4f NM", GC->PDAP_R_amin / 1852.0);
			}
			Text(skp, 18, 16, "AGS COMMAND LOAD");
			Text(skp, 7, 18, "ADD   DEDA     DIG");
			Text(skp, 1, 19, "J7");
			Text(skp, 7, 19, "224", 3);
			Text(skp, 12, 19, "%+06d", GC->DEDA224);
			Text(skp, 22, 19, "%.4f NM", GC->PDAP_J1 / 1852.0);
			Text(skp, 1, 20, "J8");
			Text(skp, 7, 20, "225", 3);
			Text(skp, 12, 20, "%+06d", GC->DEDA225);
			Text(skp, 22, 20, "%.4f NM", GC->PDAP_A_min / 1852.0);
			Text(skp, 7, 21, "226", 3);
			Text(skp, 12, 21, "%+06d", GC->DEDA226);
			if (GC->PDAPVersion != 2)
			{
				//FP6
				Text(skp, 1, 21, "J9");
				Text(skp, 22, 21, "%.4f NM", GC->PDAP_A_max / 1852.0);
				Text(skp, 1, 22, "K410");
				Text(skp, 7, 22, "227", 3);
				Text(skp, 12, 22, "%+06d", GC->DEDA227);
				Text(skp, 22, 22, "%.4f NM/DEG", GC->PDAP_K1 / 1852.0 / DEG);
			}
			else
			{
				//FP7+
				Text(skp, 1, 21, "J10");
				Text(skp, 22, 21, "%.4f NM", GC->PDAP_J2 / 1852.0);
				Text(skp, 1, 22, "J12");
				Text(skp, 7, 22, "305", 3);
				Text(skp, 12, 22, "%+06d", GC->DEDA305);
				Text(skp, 22, 22, "%.4f DEG", GC->PDAP_Theta_LIM*DEG);
				Text(skp, 1, 23, "K410");
				Text(skp, 7, 23, "662", 3);
				Text(skp, 12, 23, "%+06d", GC->DEDA662);
				Text(skp, 22, 23, "%.4f NM/DEG", GC->PDAP_K1 / 1852.0 / DEG);
				Text(skp, 1, 24, "J11");
				Text(skp, 7, 24, "673", 3);
				Text(skp, 12, 24, "%+06d", GC->DEDA673);
				Text(skp, 22, 24, "%.4f NM/DEG", GC->PDAP_K2 / 1852.0 / DEG);
			}

			if (IsBusy(G->subThreadStatus))
			{
				skp->Text(CW, CH * 31, "Calculating...", 14);
			}
			else if (GC->PDAP_ErrorCode)
			{
				switch (GC->PDAP_ErrorCode)
				{
				case 1:
					sprintf(Buffer, "Ignition algorithm failure");
					break;
				case 2:
					sprintf(Buffer, "CSI calculation failure");
					break;
				case 3:
					sprintf(Buffer, "Failure to converge on insertion velocity");
					break;
				case 4:
					sprintf(Buffer, "Insufficient data to calculate phase switch");
					break;
				case 5:
					sprintf(Buffer, "Insufficient data for curve fit");
					break;
				default:
					sprintf(Buffer, "");
					break;
				}
				skp->Text(CW, CH * 31, Buffer, strlen(Buffer));
			}
		}
		break;
	case 41:
	{
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);

		FIDOOrbitDigitals *tab;

		if (subscreen == 0)
		{
			Text(skp, 18, 0, "FDO ORBIT DIGITALS NO 1");
			Text(skp, 51, 0, "0046");
			tab = &GC->rtcc->EZSAVCSM;
			G->CycleFIDOOrbitDigitals1();
		}
		else
		{
			Text(skp, 18, 0, "FDO ORBIT DIGITALS NO 2");
			Text(skp, 51, 0, "0045");
			tab = &GC->rtcc->EZSAVLEM;
			G->CycleFIDOOrbitDigitals2();
		}
		Text(skp, 8, 2, "GET");
		Text(skp, 4, 3, "VEHICLE");
		Text(skp, 8, 4, "REV");
		Text(skp, 8, 5, "REF");
		Text(skp, 6, 6, "STAID");
		Text(skp, 6, 7, "GMTID");
		Text(skp, 6, 8, "GETID");
		Text(skp, 8, 9, "PET");
		Text(skp, 23, 9, "GETR");
		Text(skp, 10, 10, "H");
		Text(skp, 10, 11, "V");
		Text(skp, 8, 12, "GAM");
		Text(skp, 9, 14, "A");
		Text(skp, 9, 15, "E");
		Text(skp, 9, 16, "I");
		Text(skp, 9, 18, "HA");
		Text(skp, 9, 19, "PA");
		Text(skp, 9, 20, "LA");
		Text(skp, 7, 21, "GETA");
		Text(skp, 9, 23, "HP");
		Text(skp, 9, 24, "PP");
		Text(skp, 9, 25, "LP");
		Text(skp, 7, 26, "GETP");
		Text(skp, 23, 2, "NV");
		Text(skp, 22, 3, "LPP");
		Text(skp, 22, 4, "PPP");
		Text(skp, 20, 5, "GETCC");
		Text(skp, 22, 6, "TPP");
		Text(skp, 22, 7, "LAN");
		Text(skp, 38, 3, "REVL");
		Text(skp, 49, 3, "REF");
		Text(skp, 38, 4, "GETL");
		Text(skp, 41, 5, "L");
		Text(skp, 40, 6, "TO");
		Text(skp, 41, 7, "K");
		Text(skp, 37, 8, "ORBWT");
		Text(skp, 36, 10, "REQUESTED");
		Text(skp, 34, 11, "REV");
		Text(skp, 43, 11, "REF");
		Text(skp, 32, 12, "GETBV");
		Text(skp, 49, 12, "NV");
		Text(skp, 35, 13, "HA");
		Text(skp, 35, 14, "PA");
		Text(skp, 35, 15, "LA");
		Text(skp, 33, 16, "GETA");
		Text(skp, 35, 18, "HP");
		Text(skp, 35, 19, "PP");
		Text(skp, 35, 20, "LP");
		Text(skp, 33, 21, "GETP");
		Text(skp, 32, 23, "GETEI");
		Text(skp, 34, 24, "PEI");
		Text(skp, 34, 25, "LEI");
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text_GET_HHHMMSS(skp, 21, 2, tab->GET);
		Text(skp, 15, 3, tab->VEHID);
		Text(skp, 15, 4, "%03d", tab->REV);
		Text(skp, 15, 5, tab->REF1);
		Text(skp, 19, 6, tab->STAID);
		Text_GET_HHHMMSS(skp, 21, 7, tab->GMTID);
		Text_GET_HHHMMSS(skp, 21, 8, tab->GETID);
		Text_GET_HHHMMSS(skp, 21, 9, tab->PET);
		Text_GET_HHHMMSS(skp, 37, 9, tab->GETR);
		Text(skp, 20, 10, "%08.1f", tab->H);
		Text(skp, 17, 11, "%05.0f", tab->V);
		Text(skp, 18, 12, "%+06.2f", tab->GAM);
		Text(skp, 20, 14, "%08.1f", tab->A);
		Text(skp, 18, 15, "%06.4f", tab->E);
		Text(skp, 17, 16, "%05.2f", tab->I);

		if (tab->E < 1.0)
		{
			Text(skp, 20, 18, "%08.1f", tab->HA);
			if (tab->E > 0.0001)
			{
				FormatLatitude(Buffer, tab->PA);
				Text(skp, 19, 19, Buffer);
				FormatLongitude(Buffer, tab->LA);
				Text(skp, 19, 20, Buffer);
				Text_GET_HHHMMSS(skp, 21, 21, tab->GETA);
			}
		}
		Text(skp, 20, 23, "%08.1f", tab->HP);
		if (tab->E > 0.0001)
		{
			FormatLatitude(Buffer, tab->PP);
			Text(skp, 19, 24, Buffer);
			FormatLongitude(Buffer, tab->LP);
			Text(skp, 19, 25, Buffer);
			Text_GET_HHHMMSS(skp, 21, 26, tab->GETP);
		}

		Text(skp, 27, 2, "%d", tab->NV1);
		FormatLongitude(Buffer, tab->LPP);
		Text(skp, 33, 3, Buffer);
		FormatLatitude(Buffer, tab->PPP);
		Text(skp, 33, 4, Buffer);
		Text_GET_HHHMMSS(skp, 35, 5, tab->GETCC);
		if (tab->E > 0.0001)
		{
			Text(skp, 31, 6, "%05.1f", tab->TAPP);
		}
		FormatLongitude(Buffer, tab->LNPP);
		Text(skp, 33, 7, Buffer);
		Text(skp, 48, 3, "%05d", tab->REVL);
		Text(skp, 56, 3, tab->REF3);
		Text_GET_HHHMMSS(skp, 52, 4, tab->GETL);
		FormatLongitude(Buffer, tab->L, 3);
		Text(skp, 51, 5, Buffer);
		Text_GET_HHHMMSS(skp, 52, 6, tab->TO);
		Text(skp, 48, 7, "%05.1f", tab->K);
		Text(skp, 50, 8, "%07.1f", tab->ORBWT);
		Text(skp, 41, 11, "%03d", tab->REVR);
		Text(skp, 50, 11, tab->REF2);
		Text_GET_HHHMMSS(skp, 47, 12, tab->GETBV);
		Text(skp, 53, 12, "%d", tab->NV2);
		Text(skp, 46, 13, "%08.1f", tab->HAR);
		FormatLatitude(Buffer, tab->PAR);
		Text(skp, 45, 14, Buffer);
		FormatLongitude(Buffer, tab->LAR);
		Text(skp, 45, 15, Buffer);
		Text_GET_HHHMMSS(skp, 47, 16, tab->GETAR);
		Text(skp, 46, 18, "%08.1f", tab->HPR);
		FormatLatitude(Buffer, tab->PPR);
		Text(skp, 45, 19, Buffer);
		FormatLongitude(Buffer, tab->LPR);
		Text(skp, 45, 20, Buffer);
		Text_GET_HHHMMSS(skp, 47, 21, tab->GETPR);
		if (tab->Error > 0)
		{
			Text(skp, 29, 18, "*%d", tab->Error);
		}
	}
	break;
	case 42:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 25, 0, "TV GUIDE");
		Text(skp, 51, 0, "0001");
		Text(skp, 4, 27, "MSK     TITLE");
		Text(skp, 30, 27, "CH MSK     TITLE");
		x = 1; y = 2; dx = 54;
		Line(skp, (CW * x) / 2, (CH * y) / 2, (CW* (x + dx)) / 2, (CH*y) / 2); x += 58;
		Line(skp, (CW * x) / 2, (CH * y) / 2, (CW* (x + dx)) / 2, (CH*y) / 2);
		x = 1; dy = 52;
		Line(skp, (CW * x) / 2, (CH * y) / 2, (CW* x) / 2, (CH*(y + dy)) / 2); x += 6;
		Line(skp, (CW * x) / 2, (CH * y) / 2, (CW* x) / 2, (CH*(y + dy)) / 2); x += 10;
		Line(skp, (CW * x) / 2, (CH * y) / 2, (CW* x) / 2, (CH*(y + dy)) / 2); x += 26;
		Line(skp, (CW * x) / 2, (CH * y) / 2, (CW* x) / 2, (CH*(y + dy)) / 2); x += 12;
		Line(skp, (CW * x) / 2, (CH * y) / 2, (CW* x) / 2, (CH*(y + dy)) / 2); x += 4;
		Line(skp, (CW * x) / 2, (CH * y) / 2, (CW* x) / 2, (CH*(y + dy)) / 2); x += 6;
		Line(skp, (CW * x) / 2, (CH * y) / 2, (CW* x) / 2, (CH*(y + dy)) / 2); x += 10;
		Line(skp, (CW * x) / 2, (CH * y) / 2, (CW* x) / 2, (CH*(y + dy)) / 2); x += 26;
		Line(skp, (CW * x) / 2, (CH * y) / 2, (CW* x) / 2, (CH*(y + dy)) / 2); x += 12;
		Line(skp, (CW * x) / 2, (CH * y) / 2, (CW* x) / 2, (CH*(y + dy)) / 2);
		x = 4; y = 1;
		Text(skp, x, y, "0040 LAUNCH ANA 1"); y++;
		Text(skp, x, y, "0041 LAUNCH ANA 2"); y++;
		Text(skp, x, y, "0045 FDO ORB DIG2"); y++;
		Text(skp, x, y, "0046 FDO ORB DIG1"); y++;
		Text(skp, x, y, "0047 MISSION PLNG"); y++;
		Text(skp, x, y, "0048 GEN PURP MNV"); y++;
		Text(skp, x, y, "0050 PERIG ADJUST"); y++;
		Text(skp, x, y, "0054 DET MVR TAB1"); y++;
		Text(skp, x, y, "0055 CSM PSAT 1"); y++;
		Text(skp, x, y, "0056 LM PSAT 1"); y++;
		Text(skp, x, y, "0057 RDV PLAN TB"); y++;
		Text(skp, x, y, "0058 RDV EVAL TB"); y++;
		Text(skp, x, y, "0060 REL MOTION"); y++;
		Text(skp, x, y, "0069 DET MVR TAB2"); y++;
		Text(skp, x, y, "0078 LUN ORB INS"); y++;
		Text(skp, x, y, "0080 TLI PLANNING"); y++;
		Text(skp, x, y, "0082 SPACE DIGIT"); y++;
		Text(skp, x, y, "0086 LM DSCNT PLN"); y++;
		Text(skp, x, y, "0087 CSM PSAT 2"); y++;
		Text(skp, x, y, "0088 LM PSAT 2"); y++;
		Text(skp, x, y, "0229 GOST"); y++;
		Text(skp, x, y, "0232 ASCNT RDZ MN"); y++;
		Text(skp, x, y, "0233 SHRT RDZ MON"); y++;
		Text(skp, x, y, "0239 LOST"); y++;
		x += 29; y = 1;
		Text(skp, x, y, "0347 GRND TRK DIG"); y++;
		Text(skp, x, y, "0363 RET ERTH DIG"); y++;
		Text(skp, x, y, "0366 RET ERTH TRG"); y++;
		Text(skp, x, y, "1501 MOONRISE SET"); y++;
		Text(skp, x, y, "1502 SUNRISE SET"); y++;
		Text(skp, x, y, "1503 NXT STA CONT"); y++;
		Text(skp, x, y, "1505 REC ASC NODE"); y++;
		Text(skp, x, y, "1506 EXP SITE ACQ"); y++;
		Text(skp, x, y, "1508 LMK ACQ"); y++;
		Text(skp, x, y, "1590 VEC COMP TBL"); y++;
		Text(skp, x, y, "1591 VEC PANL SUM"); y++;
		Text(skp, x, y, "1597 SKEL FLT PLN"); y++;
		Text(skp, x, y, "1619 CHECKO MONIT"); y++;
		Text(skp, x, y, "1629 ONLINE MONIT"); y++;
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		//TBD: Channel numbers, these could confuse people right now which number to input
		/*
		x = 3;
		for (y = 1; y <= 26; y++)
		{
			Text(skp, x, y, "%d", y);
		}
		x += 29;
		for (y = 1; y <= 26; y++)
		{
			Text(skp, x, y, "%d", y + 26);
		}*/
		break;
	case 43:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		if (GC->MissionPlanningActive)
		{
			G->CycleSpaceDigitals();
		}
		Text(skp, 20, 0, "SPACE DIGITALS");
		Text(skp, 51, 0, "0082");
		Text(skp, 0, 1, "STA ID");
		Text(skp, 0, 2, "GMTV");
		Text(skp, 0, 4, "GET");
		Text(skp, 0, 5, "REF");
		Text(skp, 4, 6, "GET VECTOR 1");
		Text(skp, 0, 8, "REF");
		Text(skp, 11, 8, "WT");
		Text(skp, 0, 9, "AREA");
		Text(skp, 14, 9, "K");
		Text(skp, 0, 10, "GETA");
		Text(skp, 0, 11, "HA");
		Text(skp, 0, 12, "HP");
		Text(skp, 0, 13, "H");
		Text(skp, 0, 14, "V");
		Text(skp, 0, 15, "GAM");
		Text(skp, 0, 16, "PSI");
		Text(skp, 0, 17, "PS");
		Text(skp, 0, 18, "LS");
		Text(skp, 0, 19, "HS");
		Text(skp, 0, 20, "HO");
		Text(skp, 0, 21, "PO");
		Text(skp, 0, 22, "Iemp");
		Text(skp, 0, 23, "W");
		Text(skp, 0, 24, "OMG");
		Text(skp, 0, 25, "PRA");
		Text(skp, 13, 22, "A");
		Text(skp, 13, 23, "L");
		Text(skp, 13, 24, "E");
		Text(skp, 13, 25, "I");
		Text(skp, 18, 1, "WEIGHT");
		Text(skp, 17, 2, "GETV");
		Text(skp, 25, 6, "GET VECTOR 2");
		Text(skp, 23, 8, "GETSI");
		Text(skp, 23, 9, "GETCA");
		Text(skp, 24, 10, "VCA");
		Text(skp, 24, 11, "HCA");
		Text(skp, 24, 12, "PCA");
		Text(skp, 24, 13, "LCA");
		Text(skp, 23, 14, "PSICA");
		Text(skp, 23, 15, "GETMN");
		Text(skp, 24, 16, "HMN");
		Text(skp, 24, 17, "PMN");
		Text(skp, 24, 18, "LMN");
		Text(skp, 24, 19, "DMN");
		Text(skp, 38, 1, "GETR");
		Text(skp, 34, 2, "GET AXIS");
		Text(skp, 42, 6, "GET VECTOR 3");
		Text(skp, 40, 8, "GETSE");
		Text(skp, 40, 9, "GETEI");
		Text(skp, 41, 10, "VEI");
		Text(skp, 41, 11, "GEI");
		Text(skp, 41, 12, "PEI");
		Text(skp, 41, 13, "LEI");
		Text(skp, 40, 14, "PSIEI");
		Text(skp, 40, 15, "GETVP");
		Text(skp, 41, 16, "VVP");
		Text(skp, 41, 17, "HVP");
		Text(skp, 41, 18, "PVP");
		Text(skp, 41, 19, "LVP");
		Text(skp, 40, 20, "PSIVP");
		Text(skp, 43, 23, "IE");
		Text(skp, 43, 24, "LN");
		Text(skp, 14, 4, "V");
		Text(skp, 25, 4, "P");
		Text(skp, 35, 4, "h");
		Text(skp, 46, 4, "n");
		Text(skp, 14, 5, "G");
		Text(skp, 25, 5, "L");
		Text(skp, 35, 5, "PSI");
		Line(skp, 0, (CH * 7) / 2, CW * 56, (CH * 7) / 2);
		Line(skp, (CW * 45) / 2, (CH * 13) / 2, (CW * 45) / 2, CH * 28);
		Line(skp, (CW * 79) / 2, (CH * 13) / 2, (CW * 79) / 2, CH * 28);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text(skp, 19, 0, GC->rtcc->EZSPACE.VEHID);
		Text(skp, 52, 5, "%d", GC->rtcc->EZSPACE.TUP);
		Text(skp, 14, 1, GC->rtcc->EZSPACE.VecID);
		Text(skp, 33, 1, "%.1lf", GC->rtcc->EZSPACE.WEIGHT);
		Text_GET_HHHMMSS(skp, 14, 2, GC->rtcc->EZSPACE.GMTV);
		Text_GET_HHHMMSS(skp, 31, 2, GC->rtcc->EZSPACE.GETV);
		Text_GET_HHHMMSS(skp, 52, 2, GC->rtcc->EZSPACE.GETAxis);
		Text_GET_HHHMMSS(skp, 52, 1, GC->rtcc->EZSPACE.GETR);
		Text_GET_HHHMMSS(skp, 13, 4, GC->rtcc->EZSPACE.GET);
		Text(skp, 13, 5, GC->rtcc->EZSPACE.REF1);
		Text(skp, 23, 4, "%.1lf", GC->rtcc->EZSPACE.V);
		Text(skp, 23, 5, "%+.2lf", GC->rtcc->EZSPACE.GAM);
		Text(skp, 45, 4, "%.1lf", GC->rtcc->EZSPACE.H);
		Text_Latitude(skp, 34, 4, GC->rtcc->EZSPACE.PHI);
		Text_Longitude(skp, 34, 5, GC->rtcc->EZSPACE.LAM);
		Text(skp, 45, 5, "%.1lf", GC->rtcc->EZSPACE.PSI);
		Text(skp, 53, 4, "%.1lf", GC->rtcc->EZSPACE.ADA);
		Text_GET_HHHMMSSCS(skp, 17, 7, GC->rtcc->EZSPACE.GETVector1);
		Text(skp, 10, 8, GC->rtcc->EZSPACE.REF2);
		Text(skp, 22, 8, "%.1lf", GC->rtcc->EZSPACE.WT);
		Text(skp, 13, 9, "%.1lf", GC->rtcc->EZSPACE.AREA);
		Text(skp, 22, 9, "%+.1lf", GC->rtcc->EZSPACE.K);
		Text_GET_HHHMMSS(skp, 14, 10, GC->rtcc->EZSPACE.GETA);
		Text(skp, 14, 11, "%.3lf", GC->rtcc->EZSPACE.HA);
		Text(skp, 14, 12, "%+.3lf", GC->rtcc->EZSPACE.HP);
		Text(skp, 14, 13, "%.3lf", GC->rtcc->EZSPACE.H1);
		Text(skp, 12, 14, "%.3lf", GC->rtcc->EZSPACE.V1);
		Text(skp, 12, 15, "%+.3lf", GC->rtcc->EZSPACE.GAM1);
		Text(skp, 12, 16, "%.3lf", GC->rtcc->EZSPACE.PSI1);
		Text(skp, 12, 17, "%.5lf", abs(GC->rtcc->EZSPACE.PHI1));
		Text(skp, 13, 17, GC->rtcc->EZSPACE.PHI1 >= 0.0 ? "N" : "S");
		Text(skp, 12, 18, "%.5lf", abs(GC->rtcc->EZSPACE.LAM1));
		Text(skp, 13, 18, GC->rtcc->EZSPACE.LAM1 >= 0.0 ? "E" : "W");
		Text(skp, 12, 19, "%+.2lf", GC->rtcc->EZSPACE.HS);
		Text(skp, 12, 20, "%+.2lf", GC->rtcc->EZSPACE.HO);
		Text(skp, 11, 21, "%.5lf", abs(GC->rtcc->EZSPACE.PHIO));
		Text(skp, 12, 21, GC->rtcc->EZSPACE.PHIO >= 0.0 ? "N" : "S");
		Text(skp, 12, 22, "%+.3lf", GC->rtcc->EZSPACE.IEMP);
		Text(skp, 12, 23, "%.3lf", GC->rtcc->EZSPACE.W1);
		Text(skp, 12, 24, "%.3lf", GC->rtcc->EZSPACE.OMG);
		Text_Longitude(skp, 12, 25, GC->rtcc->EZSPACE.PRA);
		if (GC->rtcc->EZSPACE.A1 > 0)
		{
			Text(skp, 22, 22, "%.0lf", GC->rtcc->EZSPACE.A1);
		}
		Text(skp, 22, 23, "%.1lf", GC->rtcc->EZSPACE.L1);
		Text(skp, 22, 24, "%.4lf", GC->rtcc->EZSPACE.E1);
		Text(skp, 22, 25, "%.3lf", GC->rtcc->EZSPACE.I1);
		Text(skp, 13, 26, "%d", GC->rtcc->EZSPACE.TUN1);
		Text_GET_HHHMMSS(skp, 35, 7, GC->rtcc->EZSPACE.GETVector2);
		Text_GET_HHHMMSS(skp, 38, 8, GC->rtcc->EZSPACE.GETSI);
		Text_GET_HHHMMSS(skp, 38, 9, GC->rtcc->EZSPACE.GETCA);
		Text(skp, 38, 10, "%.2lf", GC->rtcc->EZSPACE.VCA);
		Text(skp, 38, 11, "%.2lf", GC->rtcc->EZSPACE.HCA);
		Text_Latitude(skp, 38, 12, GC->rtcc->EZSPACE.PCA);
		Text_Longitude(skp, 38, 13, GC->rtcc->EZSPACE.LCA);
		Text(skp, 38, 14, "%.3lf", GC->rtcc->EZSPACE.PSICA);
		Text_GET_HHHMMSS(skp, 38, 15, GC->rtcc->EZSPACE.GETMN);
		Text(skp, 38, 16, "%+.3lf", GC->rtcc->EZSPACE.HMN);
		Text_Latitude(skp, 38, 17, GC->rtcc->EZSPACE.PMN);
		Text_Longitude(skp, 38, 18, GC->rtcc->EZSPACE.LMN);
		Text(skp, 38, 19, "%+.3lf", GC->rtcc->EZSPACE.DMN);
		Text(skp, 33, 26, "%d", GC->rtcc->EZSPACE.TUN2);
		Text_GET_HHHMMSS(skp, 52, 7, GC->rtcc->EZSPACE.GETVector3);
		Text_GET_HHHMMSS(skp, 55, 8, GC->rtcc->EZSPACE.GETSE);
		Text_GET_HHHMMSS(skp, 55, 9, GC->rtcc->EZSPACE.GETEI);
		Text(skp, 55, 10, "%.2lf", GC->rtcc->EZSPACE.VEI);
		Text(skp, 55, 11, "%+.2lf", GC->rtcc->EZSPACE.GEI);
		Text_Latitude(skp, 55, 12, GC->rtcc->EZSPACE.PEI);
		Text_Longitude(skp, 55, 13, GC->rtcc->EZSPACE.LEI);
		Text(skp, 55, 14, "%.2lf", GC->rtcc->EZSPACE.PSIEI);
		Text_GET_HHHMMSS(skp, 55, 15, GC->rtcc->EZSPACE.GETVP);
		Text(skp, 55, 16, "%.2lf", GC->rtcc->EZSPACE.VVP);
		Text(skp, 55, 17, "%+.1lf", GC->rtcc->EZSPACE.HVP);
		Text_Latitude(skp, 55, 18, GC->rtcc->EZSPACE.PVP);
		Text_Longitude(skp, 55, 19, GC->rtcc->EZSPACE.LVP);
		Text(skp, 55, 20, "%.2lf", GC->rtcc->EZSPACE.PSIVP);
		Text(skp, 55, 23, "%.3lf", GC->rtcc->EZSPACE.IE);
		Text_Longitude(skp, 55, 24, GC->rtcc->EZSPACE.LN);
		Text(skp, 50, 26, "%d", GC->rtcc->EZSPACE.TUN2);
		Text(skp, 35, 23, GC->rtcc->EZSPACE.errormessage);
		break;
	case 44:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		if (GC->MissionPlanningActive)
		{
			skp->Text(CW, CH / 2, "Active", 6);
		}
		else
		{
			skp->Text(CW, CH / 2, "Inactive", 8);
		}
		Text(skp, 18, 0, "FDO MISSION PLAN TABLE");
		Text(skp, 51, 0, "0047");
		Text(skp, 4, 3, "CSM STA ID");
		Text(skp, 40, 3, "LM STA ID");
		Text(skp, 4, 4, "GETAV");
		Text(skp, 42, 4, "GETAV");
		Text(skp, 5, 6, "GETI");
		Text(skp, 15, 6, "DT");
		Text(skp, 22, 6, "DV");
		Text(skp, 27, 6, "DVRM");
		Text(skp, 35, 6, "HA");
		Text(skp, 42, 6, "HP");
		Text(skp, 50, 6, "CODE");
		Line(skp, 0, (CH * 11) / 2, CW * 56, (CH * 11) / 2);
		Line(skp, (CW * 25) / 2, (CH * 11) / 2, (CW * 25) / 2, CH * 28);
		Line(skp, (CW * 39) / 2, (CH * 11) / 2, (CW * 39) / 2, CH * 28);
		Line(skp, (CW * 53) / 2, (CH * 11) / 2, (CW * 53) / 2, CH * 28);
		Line(skp, (CW * 63) / 2, (CH * 11) / 2, (CW * 63) / 2, CH * 28);
		Line(skp, (CW * 93) / 2, (CH * 11) / 2, (CW * 93) / 2, CH * 28);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text(skp, 22, 3, GC->rtcc->MPTDISPLAY.CSMSTAID);
		Text(skp, 19, 4, GC->rtcc->MPTDISPLAY.CSMGETAV);
		Text(skp, 57, 3, GC->rtcc->MPTDISPLAY.LEMSTAID);
		Text(skp, 57, 4, GC->rtcc->MPTDISPLAY.LEMGETAV);
		for (unsigned i = 0; i < GC->rtcc->MPTDISPLAY.man.size(); i++)
		{
			Text_GET_HHHMMSSC(skp, 12, 8 + i, GC->rtcc->MPTDISPLAY.man[i].GETBI);
			Text(skp, 26, 8 + i, "%.1lf", GC->rtcc->MPTDISPLAY.man[i].DELTAV);
			Text(skp, 31, 8 + i, "%.0lf", GC->rtcc->MPTDISPLAY.man[i].DVREM);
			if (GC->rtcc->MPTDISPLAY.man[i].HA != 0.0)
				Text(skp, 39, 8 + i, "%.1f", GC->rtcc->MPTDISPLAY.man[i].HA);
			else
				Text(skp, 39, 8 + i, "ZZZZZZZ");
			if (GC->rtcc->MPTDISPLAY.man[i].HP != 0.0)
				Text(skp, 46, 8 + i, "%.1f", GC->rtcc->MPTDISPLAY.man[i].HP);
			else
				Text(skp, 46, 8 + i, "ZZZZZZZ");
			Text(skp, 56, 8 + i, GC->rtcc->MPTDISPLAY.man[i].code);
		}
		for (unsigned i = 1; i < GC->rtcc->MPTDISPLAY.man.size(); i++)
		{
			GET_Display_HHHMM(Buffer, GC->rtcc->MPTDISPLAY.man[i].DT);
			Text(skp, 19, 15 + i * 2, Buffer, 1, 2);
		}
		break;
	case 45:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 2, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(2);
		G->CycleNextStationContactsDisplay();
		if (GC->rtcc->SystemParameters.MGRTAG == 0)
		{
			skp->Text(CW, CH / 2, "Lunar", 5);
		}
		else
		{
			skp->Text(CW, CH / 2, "All", 3);
		}
		Text(skp, 22, 0, "NEXT STATION CONTACTS");
		Text(skp, 60, 0, "1503");
		Text(skp, 17, 2, "CSM");
		Text(skp, 27, 2, "GET");
		Text(skp, 49, 2, "LM");
		Text(skp, 9, 4, "GETHCA");
		Text(skp, 25, 4, "DTPASS");
		Text(skp, 40, 4, "GETHCA");
		Text(skp, 56, 4, "DTPASS");
		Text(skp, 2, 5, "STA");
		Text(skp, 19, 5, "EMAX");
		Text(skp, 33, 5, "STA");
		Text(skp, 50, 5, "EMAX");
		Text(skp, 9, 6, "DT KLOS");
		Text(skp, 25, 6, "DT KH");
		Text(skp, 40, 6, "DT KLOS");
		Text(skp, 56, 6, "DT KH");
		Text(skp, 7, 8, "HR MIN SEC");
		Text(skp, 19, 8, "DEG");
		Text(skp, 24, 8, "HR MN SC");
		Text(skp, 38, 8, "HR MIN SEC");
		Text(skp, 50, 8, "DEG");
		Text(skp, 55, 8, "HR MN SEC");
		Line(skp, CW / 2, (CH * 7) / 2, (CW * 129) / 2, (CH * 7) / 2);
		Line(skp, CW / 2, (CH * 15) / 2, (CW * 129) / 2, (CH * 15) / 2);
		Line(skp, CW / 2, (CH * 19) / 2, (CW * 129) / 2, (CH * 19) / 2);
		Line(skp, CW / 2, (CH * 7) / 2, CW / 2, CH * 32);
		Line(skp, (CW * 13) / 2, (CH * 7) / 2, (CW * 13) / 2, CH * 32);
		Line(skp, (CW * 37) / 2, (CH * 7) / 2, (CW * 37) / 2, CH * 32);
		Line(skp, (CW * 47) / 2, (CH * 7) / 2, (CW * 47) / 2, CH * 32);
		Line(skp, (CW * 65) / 2, (CH * 7) / 2, (CW * 65) / 2, CH * 32);
		Line(skp, (CW * 75) / 2, (CH * 7) / 2, (CW * 75) / 2, CH * 32);
		Line(skp, (CW * 99) / 2, (CH * 7) / 2, (CW * 99) / 2, CH * 32);
		Line(skp, (CW * 109) / 2, (CH * 7) / 2, (CW * 109) / 2, CH * 32);
		Line(skp, (CW * 129) / 2, (CH * 7) / 2, (CW * 129) / 2, CH * 32);
		Line(skp, CW * 9, (CH * 11) / 2, CW * 16, (CH * 11) / 2);
		Line(skp, CW * 25, (CH * 11) / 2, CW * 31, (CH * 11) / 2);
		Line(skp, CW * 40, (CH * 11) / 2, CW * 47, (CH * 11) / 2);
		Line(skp, CW * 56, (CH * 11) / 2, CW * 62, (CH * 11) / 2);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 2, true);
		Text_GET_HHHMMSS(skp, 40, 2, GC->rtcc->NextStationContactsBuffer.GET);
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				Text(skp, 6 + 31 * i, 10 + j * 3, GC->rtcc->NextStationContactsBuffer.STA[i][j]);
				if (GC->rtcc->NextStationContactsBuffer.BestAvailableAOS[i][j])
				{
					Text(skp, 8 + 31 * i, 10 + j * 3, "*");
				}
				Text_GET_HHHMMSS(skp, 17 + 31 * i, 10 + j * 3, GC->rtcc->NextStationContactsBuffer.GETHCA[i][j]);
				if (GC->rtcc->NextStationContactsBuffer.BestAvailableEMAX[i][j])
				{
					skp->Text((CW * (39 + 62 * i)) / 2 + WOFF, CH * (10 + j * 3) + HOFF, "*", 1);
				}
				sprintf_s(Buffer, "%.1f", GC->rtcc->NextStationContactsBuffer.EMAX[i][j]);
				skp->Text((CW * (47 + 62 * i)) / 2 + WOFF, CH * (10 + j * 3) + HOFF, Buffer, strlen(Buffer));
				Text_GET_HHMMSS(skp, 32 + 31 * i, 10 + j * 3, GC->rtcc->NextStationContactsBuffer.DTPASS[i][j]);
			}
		}
		break;
	case 46:
	case 72:
	case 73:
	case 74:
	{
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		PredictedSiteAcquisitionTable *tab;

		y = 3;
		Line(skp, 0, CH * y, CW * 56, CH * y);
		Line(skp, (CW * 9) / 2, CH*y, (CW * 9) / 2, CH * 28);
		Line(skp, CW * 10, CH*y, CW * 10, CH * 28);
		Line(skp, CW * 21, CH*y, CW * 21, CH * 28);
		Line(skp, CW * 32, CH*y, CW * 32, CH * 28);
		Line(skp, CW * 38, CH*y, CW * 38, CH * 28);
		Line(skp, (CW * 93) / 2, CH*y, (CW * 93) / 2, CH * 28);
		Line(skp, 0, CH * 6, CW * 56, CH * 6);
		Text(skp, 10, 0, "PREDICTED SITE ACQUISITION TABLE NO");
		Text(skp, 5, 1, "PAGE");
		Text(skp, 13, 1, "OF");
		Text(skp, 19, 1, "VEHICLE");
		Text(skp, 31, 1, "STA ID");
		switch (screen)
		{
		case 46:
			Text(skp, 46, 0, "1");
			Text(skp, 27, 1, "CSM");
			Text(skp, 52, 0, "0055");
			tab = &GC->rtcc->EZACQ1;
			break;
		case 72:
			Text(skp, 46, 0, "1");
			Text(skp, 27, 1, "LEM");
			Text(skp, 52, 0, "0056");
			tab = &GC->rtcc->EZACQ3;
			break;
		case 73:
			Text(skp, 46, 0, "2");
			Text(skp, 27, 1, "CSM");
			Text(skp, 52, 0, "0087");
			tab = &GC->rtcc->EZDPSAD1;
			break;
		case 74:
			Text(skp, 46, 0, "2");
			Text(skp, 27, 1, "LEM");
			Text(skp, 52, 0, "0088");
			tab = &GC->rtcc->EZDPSAD3;
			break;
		}
		Text(skp, 1, 4, "REV");
		Text(skp, 6, 4, "STA");
		Text(skp, 11, 7, "GETHCA", 1, 2);
		Text(skp, 11, 9, "HR:MN:SEC", 1, 2);
		Text(skp, 22, 7, "GETHCD", 1, 2);
		Text(skp, 22, 9, "HR:MN:SEC", 1, 2);
		Text(skp, 65, 4, "ELMAX", 2, 1);
		skp->SetFont(font_mocr2);
		Text(skp, 38, 3, "R=RDR,T=TLM");
		Text(skp, 38, 4, "V=VOICE");
		Text(skp, 38, 5, "C=CMD");
		skp->SetFont(font_mocr3);
		Text(skp, 48, 4, "ACQLOS");
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text(skp, 12, 1, "%d", tab->curpage);
		Text(skp, 18, 1, "%d", tab->pages);
		Text(skp, 45, 1, tab->STAID);
		for (unsigned i = 0; i < tab->numcontacts[tab->curpage - 1]; i++)
		{
			Text(skp, 4, 7 + i, "%d", tab->REV[tab->curpage - 1][i]);
			Text(skp, 9, 7 + i, tab->STA[tab->curpage - 1][i]);
			if (tab->BestAvailableAOS[tab->curpage - 1][i])
			{
				Text(skp, 11, 7 + i, "*");
			}
			Text_GET_HHHMMSS(skp, 20, 7 + i, tab->GETHCA[tab->curpage - 1][i]);
			if (tab->BestAvailableLOS[tab->curpage - 1][i])
			{
				Text(skp, 22, 7 + i, "*");
			}
			Text_GET_HHHMMSS(skp, 31, 7 + i, tab->GETHCD[tab->curpage - 1][i]);
			if (tab->BestAvailableEMAX[tab->curpage - 1][i])
			{
				Text(skp, 33, 7 + i, "*");
			}
			Text(skp, 37, 7 + i, "%.1lf", tab->ELMAX[tab->curpage - 1][i]);
		}
	}
	break;
	case 47:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Uplinks", 7);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->SetFont(font_menu2);
		GetCharSize(skp, CW, CH);
		y = 3;
		skp->Text(CW, CH * y, "00: CMC CSM Nav Update", 29); y++;
		skp->Text(CW, CH * y, "06: CMC Landing Site Vector", 27); y++;
		skp->Text(CW, CH * y, "07: CMC Time Increment", 22); y++;
		skp->Text(CW, CH * y, "08: CMC Liftoff Time Update", 27); y++;
		skp->Text(CW, CH * y, "09: CMC LM Nav Update", 28); y++;
		skp->Text(CW, CH * y, "10: CMC External DV", 19); y++;
		skp->Text(CW, CH * y, "12: CMC REFSMMAT Update", 23); y++;
		skp->Text(CW, CH * y, "13: CMC Retrofire External DV", 29); y++;
		skp->Text(CW, CH * y, "14: CMC Entry Update", 20); y++;
		skp->Text(CW, CH * y, "18: CMC Erasable Memory Update A", 32); y++;
		skp->Text(CW, CH * y, "19: CMC Erasable Memory Update B", 32); y++; y++;
		skp->Text(CW, CH * y, "20: LGC LM Nav Update", 28); y++;
		skp->Text(CW, CH * y, "21: LGC CSM Nav Update", 29); y++;
		skp->Text(CW, CH * y, "22: LGC External DV", 19); y++;
		skp->Text(CW, CH * y, "23: LGC REFSMMAT Update", 23); y++;
		skp->Text(CW, CH * y, "24: LGC Time Increment", 22); y++;
		skp->Text(CW, CH * y, "25: LGC Liftoff Time Update", 27); y++;
		skp->Text(CW, CH * y, "26: LGC Landing Site Vector", 27); y++;
		skp->Text(CW, CH * y, "28: LGC Descent Update", 22); y++;
		skp->Text(CW, CH * y, "38: LGC Erasable Memory Update A", 32); y++;
		skp->Text(CW, CH * y, "39: LGC Erasable Memory Update B", 32);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		y = 3;
		skp->Text(W - CW, CH * y, "49: LVDC Navigation Update", 26);
		break;
	case 48:
	{
		skp->SetPen(pen2);
		SetMOCRFont(skp, 4, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(4);
		RTCC::NavUpdateMakeupBuffer * tab;
		switch (subscreen)
		{
		case 0:
			Text(skp, 13, 0, "CMC CSM NAV UPDATE");
			Text(skp, 38, 0, "0276");
			tab = &GC->rtcc->CZNAVGEN.CMCCSMUpdate;
			break;
		case 1:
			Text(skp, 13, 0, "CMC LM NAV UPDATE");
			Text(skp, 38, 0, "0270");
			tab = &GC->rtcc->CZNAVGEN.CMCLEMUpdate;
			break;
		case 2:
			Text(skp, 13, 0, "LGC CSM NAV UPDATE");
			Text(skp, 38, 0, "0278");
			tab = &GC->rtcc->CZNAVGEN.LGCCSMUpdate;
			break;
		case 3:
			Text(skp, 13, 0, "LGC LM NAV UPDATE");
			Text(skp, 38, 0, "0279");
			tab = &GC->rtcc->CZNAVGEN.LGCLEMUpdate;
			break;
		}
		Text(skp, 1, 1, "LOAD NO");
		Text(skp, 15, 1, "GETGEN");
		Text(skp, 32, 1, "SITES");
		Text(skp, 1, 2, "STA ID");
		Text(skp, 16, 2, "REF");
		Text(skp, 27, 2, "GETSV");
		Text(skp, 3, 3, "OID");
		Text(skp, 10, 3, "FCT");
		Text(skp, 16, 3, "DSKY V71");
		Text(skp, 30, 3, "VECTOR");
		Text(skp, 9, 4, "INDEX");
		Text(skp, 10, 5, "ADD");
		Text(skp, 10, 6, "VID");
		Text(skp, 11, 7, "X");
		Text(skp, 11, 8, "X");
		Text(skp, 11, 9, "Y");
		Text(skp, 11, 10, "Y");
		Text(skp, 11, 11, "Z");
		Text(skp, 11, 12, "Z");
		Text(skp, 11, 13, "XD");
		Text(skp, 11, 14, "XD");
		Text(skp, 11, 15, "YD");
		Text(skp, 11, 16, "YD");
		Text(skp, 11, 17, "ZD");
		Text(skp, 11, 18, "ZD");
		Text(skp, 11, 19, "T");
		Text(skp, 11, 20, "T");
		for (y = 4; y < 21; y++)
		{
			Text(skp, 4, y, "%02o", y - 3);
		}
		Line(skp, CW, CH * 3, CW * 42, CH * 3);
		Line(skp, CW, CH * 4, CW * 42, CH * 4);
		Line(skp, (CW * 15) / 2, CH * 3, (CW * 15) / 2, CH * 21);
		Line(skp, (CW * 31) / 2, CH * 3, (CW * 31) / 2, CH * 21);
		Line(skp, (CW * 49) / 2, CH * 3, (CW * 49) / 2, CH * 21);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 4, false);
		Text(skp, 13, 1, "%04d", tab->SequenceNumber);
		Text_GET_HHHMMSS(skp, 31, 1, tab->GETofGeneration);
		if (GC->MissionPlanningActive)
		{
			Text(skp, 15, 2, tab->DCCode);
		}
		else
		{
			if (subscreen == 0 || subscreen == 2)
			{
				PrintCSMVessel(Buffer, false);
			}
			else
			{
				PrintLMVessel(Buffer, false);
			}
			sprintf(Buffer2, "%.7s", Buffer);
			Text(skp, 15, 2, Buffer2);
		}
		Text(skp, 25, 2, tab->sv.RBI == BODY_EARTH ? "EARTH" : "LUNAR");
		if (G->SVDesiredGET < 0)
		{
			sprintf(Buffer, "Present");
		}
		else
		{
			GET_Display(Buffer, G->SVDesiredGET, false);
		}
		Text(skp, 42, 2, Buffer);

		for (int i = 0; i < 021; i++)
		{
			Text(skp, 22, 4 + i, "%05d", tab->Octals[i]);
		}
		Text(skp, 40, 7, "%+.1f", tab->sv.R.x / 0.3048);
		Text(skp, 40, 9, "%+.1f", tab->sv.R.y / 0.3048);
		Text(skp, 40, 11, "%+.1f", tab->sv.R.z / 0.3048);
		Text(skp, 39, 13, "%+.2f", tab->sv.V.x / 0.3048);
		Text(skp, 39, 15, "%+.2f", tab->sv.V.y / 0.3048);
		Text(skp, 39, 17, "%+.2f", tab->sv.V.z / 0.3048);
		Text_GET_HHHMMSSCS(skp, 39, 19, tab->sv.GMT);
	}
	break;
	case 49:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CW / 2, "Landing Site Update", 19);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		PrintLMVessel(Buffer, false);
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST] * DEG);
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.3f°", GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST] * DEG);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%.2f NM", GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST] / 1852.0);
		skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		break;
	case 50:
	case 98:
	{
		skp->SetPen(pen2);
		SetMOCRFont(skp, 4, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(4);
		RTCC::LandingSiteMakupBuffer *tab;
		if (screen == 50)
		{
			Text(skp, 8, 0, "LANDING SITE UPDT TO CMC");
			Text(skp, 38, 0, "0293");
			tab = &GC->rtcc->CZLSVECT.CSMLSUpdate;
		}
		else
		{
			Text(skp, 8, 0, "LANDING SITE UPDT TO LGC");
			Text(skp, 38, 0, "0294");
			tab = &GC->rtcc->CZLSVECT.LMLSUpdate;
		}
		Text(skp, 1, 1, "LOAD NO");
		Text(skp, 15, 1, "GETGEN");
		Text(skp, 32, 1, "SITES");
		Text(skp, 1, 2, "LAT");
		Text(skp, 13, 2, "LNG");
		Text(skp, 26, 2, "RAD");
		Text(skp, 3, 3, "OID");
		Text(skp, 10, 3, "FCT");
		Text(skp, 16, 3, "DSKY V71");
		Text(skp, 30, 3, "VECTOR"); y = 4;
		Text(skp, 9, y, "INDEX"); y++;
		Text(skp, 10, y, "ADD"); y++;
		Text(skp, 11, y, "X"); y++;
		Text(skp, 11, y, "X"); y++;
		Text(skp, 11, y, "Y"); y++;
		Text(skp, 11, y, "Y"); y++;
		Text(skp, 11, y, "Z"); y++;
		Text(skp, 11, y, "Z");
		for (y = 4; y < 12; y++)
		{
			Text(skp, 4, y, "%02o", y - 3);
		}
		Line(skp, CW, CH * 3, CW * 42, CH * 3);
		Line(skp, CW, CH * 4, CW * 42, CH * 4);
		Line(skp, (CW * 15) / 2, CH * 3, (CW * 15) / 2, CH * 21);
		Line(skp, (CW * 31) / 2, CH * 3, (CW * 31) / 2, CH * 21);
		Line(skp, (CW * 49) / 2, CH * 3, (CW * 49) / 2, CH * 21);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 4, false);
		Text(skp, 13, 1, "%04d", tab->SequenceNumber);
		Text_GET_HHHMMSS(skp, 31, 1, tab->GETofGeneration);
		Text(skp, 11, 2, "%.3lf", abs(tab->lat) * DEG);
		Text(skp, 12, 2, tab->lat >= 0.0 ? "N" : "S");
		Text(skp, 24, 2, "%.3lf", abs(tab->lng) * DEG);
		Text(skp, 25, 2, tab->lng >= 0.0 ? "E" : "W");
		Text(skp, 37, 2, "%.3lf", tab->rad / 1852.0);
		for (int i = 0; i < 010; i++)
		{
			Text(skp, 22, 4 + i, "%05d", tab->Octals[i]);
		}
		Text(skp, 40, 6, "%+.1f", tab->R_LS.x / 0.3048);
		Text(skp, 40, 8, "%+.1f", tab->R_LS.y / 0.3048);
		Text(skp, 40, 10, "%+.1f", tab->R_LS.z / 0.3048);
	}
	break;
	case 51:
	case 102:
	{
		skp->SetPen(pen2);
		SetMOCRFont(skp, 4, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(4);
		RTCC::ExternalDVMakeupBuffer *tab;
		if (screen == 51)
		{
			Text(skp, 8, 0, "CMC EXTERNAL DV UPDATE");
			Text(skp, 38, 0, "0264");
			tab = &GC->rtcc->CZAXTRDV;
		}
		else
		{
			Text(skp, 8, 0, "LGC EXTERNAL DV UPDATE");
			Text(skp, 38, 0, "0280");
			tab = &GC->rtcc->CZLXTRDV;
		}
		Text(skp, 1, 1, "LOAD NO");
		Text(skp, 15, 1, "GETGEN");
		Text(skp, 32, 1, "SITES");
		Text(skp, 3, 3, "OID");
		Text(skp, 10, 3, "FCT");
		Text(skp, 16, 3, "DSKY V71");
		Text(skp, 30, 3, "DECIMAL"); y = 4;
		Text(skp, 9, y, "INDEX"); y++;
		Text(skp, 10, y, "ADD"); y++;
		Text(skp, 10, y, "VGX"); y++;
		Text(skp, 10, y, "VGX"); y++;
		Text(skp, 10, y, "VGY"); y++;
		Text(skp, 10, y, "VGY"); y++;
		Text(skp, 10, y, "VGZ"); y++;
		Text(skp, 10, y, "VGZ"); y++;
		Text(skp, 10, y, "TIGN"); y++;
		Text(skp, 10, y, "TIGN");
		for (y = 4; y < 14; y++)
		{
			Text(skp, 4, y, "%02o", y - 3);
		}
		Line(skp, CW, CH * 3, CW * 42, CH * 3);
		Line(skp, CW, CH * 4, CW * 42, CH * 4);
		Line(skp, (CW * 15) / 2, CH * 3, (CW * 15) / 2, CH * 21);
		Line(skp, (CW * 31) / 2, CH * 3, (CW * 31) / 2, CH * 21);
		Line(skp, (CW * 49) / 2, CH * 3, (CW * 49) / 2, CH * 21);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 4, true);
		Text(skp, 13, 1, "%04d", tab->LoadNumber);
		Text_GET_HHHMMSS(skp, 31, 1, tab->GenGET);
		for (int i = 0; i < 012; i++)
		{
			Text(skp, 22, 4 + i, "%05d", tab->Octals[i]);
		}
		if (GC->MissionPlanningActive)
		{
			Text(skp, 40, 6, "%+.1f", tab->DV.x);
			Text(skp, 40, 8, "%+.1f", tab->DV.y);
			Text(skp, 40, 10, "%+.1f", tab->DV.z);
			Text_GET_HHHMMSSCS(skp, 40, 12, tab->GET);
		}
		else
		{
			Text(skp, 40, 6, "%+.1f", G->dV_LVLH.x / 0.3048);
			Text(skp, 40, 8, "%+.1f", G->dV_LVLH.y / 0.3048);
			Text(skp, 40, 10, "%+.1f", G->dV_LVLH.z / 0.3048);
			Text_GET_HHHMMSSCS(skp, 40, 12, G->P30TIG);
		}
	}
	break;
	case 52:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 4, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(4);
		Text(skp, 2, 0, "CMC RETROFIRE EXTERNAL DV UPDATE");
		Text(skp, 38, 0, "0360");
		Text(skp, 1, 1, "LOAD NO");
		Text(skp, 15, 1, "GETGEN");
		Text(skp, 32, 1, "SITES");
		Text(skp, 3, 3, "OID");
		Text(skp, 10, 3, "FCT");
		Text(skp, 16, 3, "DSKY V71");
		Text(skp, 30, 3, "DECIMAL"); y = 4;
		Text(skp, 9, y, "INDEX"); y++;
		Text(skp, 10, y, "ADD"); y++;
		Text(skp, 10, y, "LAT"); y++;
		Text(skp, 10, y, "LAT"); y++;
		Text(skp, 10, y, "LNG"); y++;
		Text(skp, 10, y, "LNG"); y++;
		Text(skp, 10, y, "VGX"); y++;
		Text(skp, 10, y, "VGX"); y++;
		Text(skp, 10, y, "VGY"); y++;
		Text(skp, 10, y, "VGY"); y++;
		Text(skp, 10, y, "VGZ"); y++;
		Text(skp, 10, y, "VGZ"); y++;
		Text(skp, 10, y, "TIGN"); y++;
		Text(skp, 10, y, "TIGN");
		for (y = 4; y < 18; y++)
		{
			Text(skp, 4, y, "%02o", y - 3);
		}
		Line(skp, CW, CH * 3, CW * 42, CH * 3);
		Line(skp, CW, CH * 4, CW * 42, CH * 4);
		Line(skp, (CW * 15) / 2, CH * 3, (CW * 15) / 2, CH * 21);
		Line(skp, (CW * 31) / 2, CH * 3, (CW * 31) / 2, CH * 21);
		Line(skp, (CW * 49) / 2, CH * 3, (CW * 49) / 2, CH * 21);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 4, true);
		Text(skp, 13, 1, "%04d", GC->rtcc->CZREXTDV.SequenceNumber);
		Text_GET_HHHMMSS(skp, 31, 1, GC->rtcc->CZREXTDV.GETLoadGeneration);
		for (int i = 0; i < 016; i++)
		{
			Text(skp, 22, 4 + i, "%05d", GC->rtcc->CZREXTDV.Octals[i]);
		}
		Text(skp, 40, 6, "%+.3f", GC->rtcc->CZREXTDV.Lat);
		Text(skp, 40, 8, "%+.3f", GC->rtcc->CZREXTDV.Lng);
		Text(skp, 40, 10, "%+.1f", GC->rtcc->CZREXTDV.DV.x);
		Text(skp, 40, 12, "%+.1f", GC->rtcc->CZREXTDV.DV.y);
		Text(skp, 40, 14, "%+.1f", GC->rtcc->CZREXTDV.DV.z);
		Text_GET_HHHMMSSCS(skp, 40, 16, GC->rtcc->CZREXTDV.GET_TIG);
		break;
	case 53:
	case 94:
	{
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		RTCC::REFSMMATUpdateMakeupTableBlock *block;

		if (screen == 53)
		{
			block = &GC->rtcc->CZREFMAT.Block[0];
			Text(skp, 10, 0, "CMC REFSMMAT UPDATE");
			Text(skp, 52, 0, "0266");
		}
		else
		{
			block = &GC->rtcc->CZREFMAT.Block[1];
			Text(skp, 10, 0, "LGC REFSMMAT UPDATE");
			Text(skp, 52, 0, "0265");
		}
		Text(skp, 1, 1, "LOAD NO");
		Text(skp, 15, 1, "GETGEN");
		Text(skp, 32, 1, "SITES");
		Text(skp, 1, 2, "ID:");
		Text(skp, 12, 2, "MATRIX TYPE:");
		Text(skp, 3, 3, "OID");
		Text(skp, 10, 3, "FCT");
		Text(skp, 16, 3, "DSKY V71");
		Text(skp, 30, 3, "DECIMAL"); y = 4;
		Text(skp, 9, y, "INDEX"); y++;
		Text(skp, 10, y, "ADD"); y++;
		Text(skp, 10, y, "XIXE"); y++;
		Text(skp, 10, y, "XIXE"); y++;
		Text(skp, 10, y, "XIYE"); y++;
		Text(skp, 10, y, "XIYE"); y++;
		Text(skp, 10, y, "XIZE"); y++;
		Text(skp, 10, y, "XIZE"); y++;
		Text(skp, 10, y, "YIXE"); y++;
		Text(skp, 10, y, "YIXE"); y++;
		Text(skp, 10, y, "YIYE"); y++;
		Text(skp, 10, y, "YIYE"); y++;
		Text(skp, 10, y, "YIZE"); y++;
		Text(skp, 10, y, "YIZE"); y++;
		Text(skp, 10, y, "ZIXE"); y++;
		Text(skp, 10, y, "ZIXE"); y++;
		Text(skp, 10, y, "ZIYE"); y++;
		Text(skp, 10, y, "ZIYE"); y++;
		Text(skp, 10, y, "ZIZE"); y++;
		Text(skp, 10, y, "ZIZE"); y++;
		for (int i = 1; i <= 024; i++)
		{
			Text(skp, 4, i + 3, "%02o", i);

		}
		Line(skp, CW, CH * 3, CW * 42, CH * 3);
		Line(skp, CW, CH * 4, CW * 42, CH * 4);
		Line(skp, (CW * 15) / 2, CH * 3, (CW * 15) / 2, CH * 28);
		Line(skp, (CW * 31) / 2, CH * 3, (CW * 31) / 2, CH * 28);
		Line(skp, (CW * 49) / 2, CH * 3, (CW * 49) / 2, CH * 28);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text(skp, 13, 1, "%04d", block->UpdateNo);
		Text_GET_HHHMMSS(skp, 31, 1, block->GET);
		Text(skp, 11, 2, block->MatrixID);
		Text(skp, 32, 2, block->MatrixType == 2 ? "DESIRED" : "ACTUAL");
		for (int i = 0; i < 024; i++)
		{
			Text(skp, 22, 4 + i, "%05d", block->Octals[i]);
		}
		for (int i = 0; i < 9; i++)
		{
			Text(skp, 40, 6 + i * 2, "%+.8lf", block->REFSMMAT.data[i]);
		}
		Text(skp, 10, 26, block->error);
	}
	break;
	case 54:
	{
		int num = (GC->MissionPlanningActive ? subscreen : 0);

		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Two Impulse Transfer (MED M72)", 30);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		x = 1;  y = 3; dx = 7;
		Text(skp, x, marker + y, "*");
		x++;

		Text(skp, x, y, "TAB:");
		if (GC->rtcc->med_m72.Table == 1) Text(skp, x + dx, y, "Multiple Solution");
		else Text(skp, x + dx, y, "Corrective Combination");
		y++;
		Text(skp, x, y, "PLN:");
		Text(skp, x + dx, y, "%d", GC->rtcc->med_m72.Plan); y++;
		if (GC->MissionPlanningActive)
		{
			Text(skp, x, y, "DEL:");
			if (GC->rtcc->med_m72.DeleteGET > 0) Text_GET_HHHMMSSCS(skp, x + dx, y, GC->rtcc->med_m72.DeleteGET);
			else Text(skp, x + dx, y, "Do not delete");
		}
		y++;
		if (GC->MissionPlanningActive)
		{
			Text(skp, x, y, "MAN:");
			Text(skp, x + dx, y, "Inputs for maneuver %d", subscreen + 1);
		}
		y++;
		Text(skp, x, y, "THR:");
		ThrusterName(Buffer, GC->rtcc->med_m72.ManData[num].Thruster);
		Text(skp, x + dx, y, Buffer); y++;
		if (GC->MissionPlanningActive)
		{
			Text(skp, x, y, "ATT:");
			MPTAttitudeName(Buffer, GC->rtcc->med_m72.ManData[num].Attitude);
			Text(skp, x + dx, y, Buffer);
		}
		y++;
		Text(skp, x, y, "ULL:");
		PrintUllage(Buffer, GC->rtcc->med_m72.ManData[num].Thruster, GC->rtcc->med_m72.ManData[num].UllageQuads, GC->rtcc->med_m72.ManData[num].UllageDT);
		Text(skp, x + dx, y, Buffer); y++;
		Text(skp, x, y, "ITE:");
		if (GC->rtcc->med_m72.ManData[num].Iteration) Text(skp, x + dx, y, "Iterate");
		else Text(skp, x + dx, y, "Do not iterate");
		y++;
		if (GC->rtcc->med_m72.ManData[num].Thruster == RTCC_ENGINETYPE_LMDPS)
		{
			Text(skp, x, y, "10P:");
			Text(skp, x + dx, y, "%.1f s", GC->rtcc->med_m72.ManData[num].TenPercentDT); y++;
			Text(skp, x, y, "DPS:");
			Text(skp, x + dx, y, "%.3f", GC->rtcc->med_m72.ManData[num].DPSThrustFactor); y++;
		}
		else y += 2;
		Text(skp, x, y, "TIM:");
		if (GC->rtcc->med_m72.ManData[num].TimeFlag) Text(skp, x + dx, y, "Impulsive TIG");
		else Text(skp, x + dx, y, "Optimum TIG");
		y++;
		if (GC->MissionPlanningActive)
		{
			Text(skp, x, y, "SAV:");
			Text(skp, x + dx, y, "Use for all maneuvers");
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (GC->MissionPlanningActive == false)
		{
			GET_Display2(Buffer, G->P30TIG);
			skp->Text(W - CW, H - CH * 5, Buffer, strlen(Buffer));

			skp->Text(W - CW * 10, H - CH * 4, "DVX", 3);
			skp->Text(W - CW * 10, H - CH * 3, "DVY", 3);
			skp->Text(W - CW * 10, H - CH * 2, "DVZ", 3);

			AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
			skp->Text(W - CW, H - CH * 4, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
			skp->Text(W - CW, H - CH * 3, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
			skp->Text(W - CW, H - CH * 2, Buffer, strlen(Buffer));
		}
	}
		break;
	case 55:
	{
		int num = (GC->MissionPlanningActive ? subscreen : 0);

		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Transfer DKI, SPQ, or a Descent Plan", 36);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		x = 1;  y = 3; dx = 7;
		Text(skp, x, marker + y, "*");
		x++;
		Text(skp, x, y, "PLN:");
		if (GC->rtcc->med_m70.Plan == 0) Text(skp, x + dx, y, "SPQ");
		else if (GC->rtcc->med_m70.Plan < 0) Text(skp, x + dx, y, "Descent Plan");
		else Text(skp, x + dx, y, "DKI Plan %d", GC->rtcc->med_m70.Plan);
		y++;
		if (GC->MissionPlanningActive)
		{
			Text(skp, x, y, "DEL:");
			if (GC->rtcc->med_m70.DeleteGET > 0) Text_GET_HHHMMSSCS(skp, x + dx, y, GC->rtcc->med_m70.DeleteGET);
			else Text(skp, x + dx, y, "Do not delete");
		}
		y++;
		if (GC->MissionPlanningActive)
		{
			Text(skp, x, y, "MAN:");
			Text(skp, x + dx, y, "Inputs for maneuver %d", subscreen + 1);
		}
		y++;
		Text(skp, x, y, "THR:");
		ThrusterName(Buffer, GC->rtcc->med_m70.ManData[num].Thruster);
		Text(skp, x + dx, y, Buffer); y++;
		if (GC->MissionPlanningActive)
		{
			Text(skp, x, y, "ATT:");
			MPTAttitudeName(Buffer, GC->rtcc->med_m70.ManData[num].Attitude);
			Text(skp, x + dx, y, Buffer);
		}
		y++;
		Text(skp, x, y, "ULL:");
		PrintUllage(Buffer, GC->rtcc->med_m70.ManData[num].Thruster, GC->rtcc->med_m70.ManData[num].UllageQuads, GC->rtcc->med_m70.ManData[num].UllageDT);
		Text(skp, x + dx, y, Buffer); y++;
		Text(skp, x, y, "ITE:");
		if (GC->rtcc->med_m70.ManData[num].Iteration) Text(skp, x + dx, y, "Iterate");
		else Text(skp, x + dx, y, "Do not iterate");
		y++;
		if (GC->rtcc->med_m70.ManData[num].Thruster == RTCC_ENGINETYPE_LMDPS)
		{
			Text(skp, x, y, "10P:");
			Text(skp, x + dx, y, "%.1f s", GC->rtcc->med_m70.ManData[num].TenPercentDT); y++;
			Text(skp, x, y, "DPS:");
			Text(skp, x + dx, y, "%.3f", GC->rtcc->med_m70.ManData[num].DPSThrustFactor); y++;
		}
		else y += 2;
		Text(skp, x, y, "TIM:");
		if (GC->rtcc->med_m70.ManData[num].TimeFlag) Text(skp, x + dx, y, "Impulsive TIG");
		else Text(skp, x + dx, y, "Optimum TIG");
		y++;
		if (GC->MissionPlanningActive)
		{
			Text(skp, x, y, "SAV:");
			Text(skp, x + dx, y, "Use for all maneuvers");
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (GC->MissionPlanningActive == false)
		{
			GET_Display2(Buffer, G->P30TIG);
			skp->Text(W - CW, H - CH * 5, Buffer, strlen(Buffer));

			skp->Text(W - CW * 10, H - CH * 4, "DVX", 3);
			skp->Text(W - CW * 10, H - CH * 3, "DVY", 3);
			skp->Text(W - CW * 10, H - CH * 2, "DVZ", 3);

			AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
			skp->Text(W - CW, H - CH * 4, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
			skp->Text(W - CW, H - CH * 3, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
			skp->Text(W - CW, H - CH * 2, Buffer, strlen(Buffer));
		}
	}
		break;
	case 56:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Direct Input to MPT (MED M66)", 29);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (GC->rtcc->med_m66.Table == RTCC_MPT_CSM)
		{
			skp->Text(CW, 2 * H / 14, "CSM", 3);
		}
		else
		{
			skp->Text(CW, 2 * H / 14, "LM", 2);
		}
		if (GC->rtcc->med_m66.ReplaceCode == 0)
		{
			skp->Text(CW, 4 * H / 14, "Don't replace", 13);
		}
		else
		{
			sprintf_s(Buffer, "%d", GC->rtcc->med_m66.ReplaceCode);
			skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		}
		GET_Display3(Buffer, GC->rtcc->med_m66.GETBI);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		ThrusterName(Buffer, GC->rtcc->med_m66.Thruster);
		skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		MPTAttitudeName(Buffer, GC->rtcc->med_m66.AttitudeOpt);
		skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->med_m66.BurnParamNo == 1)
		{
			skp->Text(CW, 12 * H / 14, "P1: DV or DT", 16);
			if (GC->rtcc->PZBURN.P1_DVIND == 0)
			{
				sprintf(Buffer2, "MAG");
			}
			else if (GC->rtcc->PZBURN.P1_DVIND == 1)
			{
				sprintf(Buffer2, "DVC");
			}
			else
			{
				sprintf(Buffer2, "XBT");
			}
			sprintf(Buffer, "%.1f ft/s, %s, %.1f s", GC->rtcc->PZBURN.P1_DV / 0.3048, Buffer2, GC->rtcc->PZBURN.P1_DT);
			skp->Text(CW, 13 * H / 14, Buffer, strlen(Buffer));
		}
		else if (GC->rtcc->med_m66.BurnParamNo == 2)
		{
			skp->Text(CW, 12 * H / 14, "P2: DV Vector (AGC)", 19);

			sprintf(Buffer, "%+.1f %+.1f %+.1f", GC->rtcc->PZBURN.P2_DV.x / 0.3048, GC->rtcc->PZBURN.P2_DV.y / 0.3048, GC->rtcc->PZBURN.P2_DV.z / 0.3048);
			skp->Text(CW, 13 * H / 14, Buffer, strlen(Buffer));
		}
		else if (GC->rtcc->med_m66.BurnParamNo == 3)
		{
			skp->Text(CW, 12 * H / 14, "P3: DV Vector (IMU)", 19);

			sprintf(Buffer, "%+.1f %+.1f %+.1f", GC->rtcc->PZBURN.P3_DV.x / 0.3048, GC->rtcc->PZBURN.P3_DV.y / 0.3048, GC->rtcc->PZBURN.P3_DV.z / 0.3048);
			skp->Text(CW, 13 * H / 14, Buffer, strlen(Buffer));
		}
		else if (GC->rtcc->med_m66.BurnParamNo == 4)
		{
			skp->Text(CW, 12 * H / 14, "P4: DV Vector (LVLH)", 20);

			sprintf(Buffer, "%+.1f %+.1f %+.1f", GC->rtcc->PZBURN.P4_DV.x / 0.3048, GC->rtcc->PZBURN.P4_DV.y / 0.3048, GC->rtcc->PZBURN.P4_DV.z / 0.3048);
			skp->Text(CW, 13 * H / 14, Buffer, strlen(Buffer));
		}
		else if (GC->rtcc->med_m66.BurnParamNo == 5)
		{
			skp->Text(CW, 12 * H / 14, "P5: Lambert", 11);
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (GC->rtcc->med_m66.BurnParamNo == 1)
		{
			if (GC->rtcc->med_m66.CoordInd == 0)
			{
				skp->Text(W - CW, 2 * H / 14, "LVLH", 4);
			}
			else if (GC->rtcc->med_m66.CoordInd == 1)
			{
				skp->Text(W - CW, 2 * H / 14, "IMU", 4);
			}
			else
			{
				skp->Text(W - CW, 2 * H / 14, "FDAI", 4);
			}
			sprintf(Buffer, "%06.2f°P %06.2f°Y %06.2f°R", GC->rtcc->med_m66.Att.x*DEG, GC->rtcc->med_m66.Att.y*DEG, GC->rtcc->med_m66.Att.z*DEG);
			skp->Text(W - CW, 3 * H / 14, Buffer, strlen(Buffer));
		}
		if (GC->rtcc->med_m66.AttitudeOpt >= 3)
		{
			if (GC->rtcc->med_m66.HeadsUp)
			{
				skp->Text(W - CW, 4 * H / 14, "Heads Up", 8);
			}
			else
			{
				skp->Text(W - CW, 4 * H / 14, "Heads Down", 10);
			}
		}
		if (GC->rtcc->med_m66.Thruster == RTCC_ENGINETYPE_LMDPS)
		{
			sprintf_s(Buffer, "%.1lf s", GC->rtcc->med_m66.TenPercentDT);
			skp->Text(W - CW, 6 * H / 14, Buffer, strlen(Buffer));

			sprintf_s(Buffer, "%.3lf", GC->rtcc->med_m66.DPSThrustFactor);
			skp->Text(W - CW, 8 * H / 14, Buffer, strlen(Buffer));
		}
		skp->Text(W - CW, 10 * H / 14, "Page 1/2", 8);
		break;
	case 57:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "GPM Transfer (MED M65)", 22);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (GC->rtcc->med_m65.Table == 1)
		{
			skp->Text(CW, 2 * H / 14, "CSM", 3);
		}
		else
		{
			skp->Text(CW, 2 * H / 14, "LEM", 3);
		}
		if (GC->MissionPlanningActive)
		{
			if (GC->rtcc->med_m65.ReplaceCode == 0)
			{
				skp->Text(CW, 4 * H / 14, "Don't replace", 13);
			}
			else
			{
				sprintf_s(Buffer, "%d", GC->rtcc->med_m65.ReplaceCode);
				skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
			}
			MPTAttitudeName(Buffer, GC->rtcc->med_m65.Attitude);
			skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		}
		ThrusterName(Buffer, GC->rtcc->med_m65.Thruster);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		PrintUllage(Buffer, GC->rtcc->med_m65.Thruster, GC->rtcc->med_m65.UllageQuads, GC->rtcc->med_m65.UllageDT);
		skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (GC->rtcc->med_m65.Iteration)
		{
			skp->Text(W - CW, 2 * H / 14, "Iterate", 7);
		}
		else
		{
			skp->Text(W - CW, 2 * H / 14, "Don't iterate", 13);
		}

		if (GC->rtcc->med_m65.Thruster == RTCC_ENGINETYPE_LMDPS)
		{
			sprintf_s(Buffer, "%lf s", GC->rtcc->med_m65.TenPercentDT);
			skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));

			sprintf_s(Buffer, "%lf", GC->rtcc->med_m65.DPSThrustFactor);
			skp->Text(W - CW, 6 * H / 14, Buffer, strlen(Buffer));
		}
		if (GC->rtcc->med_m65.TimeFlag)
		{
			skp->Text(W - CW, 8 * H / 14, "Impulsive TIG", 13);
		}
		else
		{
			skp->Text(W - CW, 8 * H / 14, "Optimum TIG", 11);
		}
		if (GC->MissionPlanningActive == false)
		{
			GET_Display2(Buffer, G->P30TIG);
			skp->Text(W - CW, H - CH * 5, Buffer, strlen(Buffer));

			skp->Text(W - CW * 10, H - CH * 4, "DVX", 3);
			skp->Text(W - CW * 10, H - CH * 3, "DVY", 3);
			skp->Text(W - CW * 10, H - CH * 2, "DVZ", 3);

			AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
			skp->Text(W - CW, H - CH * 4, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
			skp->Text(W - CW, H - CH * 3, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
			skp->Text(W - CW, H - CH * 2, Buffer, strlen(Buffer));
		}
		break;
	case 58:
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 20, 0, "CHECKOUT MONITOR");
		Text(skp, 52, 0, "1619");
		Text(skp, 1, 1, "GET");
		Text(skp, 18, 1, "R-DAY");
		Text(skp, 33, 1, "K-FAC");
		Text(skp, 1, 2, "GMT");
		Text(skp, 18, 2, "VID");
		Text(skp, 30, 2, "RF");
		Text(skp, 37, 2, "A");
		Text(skp, 47, 2, "CFG");
		Text(skp, 4, 3, "VEH");
		Text(skp, 13, 3, "XT");
		Text(skp, 27, 3, "YT");
		Text(skp, 41, 3, "ZT");
		Text(skp, 44, 4, "OPTION");
		Text(skp, 2, 5, "X");
		Text(skp, 2, 7, "Y");
		Text(skp, 2, 9, "Z");
		Text(skp, 1, 11, "XD");
		Text(skp, 1, 13, "YD");
		Text(skp, 1, 15, "ZD");
		Text(skp, 18, 5, "VI");
		Text(skp, 18, 7, "GI");
		Text(skp, 18, 9, "AZ");
		Text(skp, 18, 11, "PS");
		Text(skp, 19, 13, "L");
		Text(skp, 18, 15, "HS");
		Text(skp, 32, 5, "a");
		Text(skp, 32, 7, "e");
		Text(skp, 32, 9, "i");
		Text(skp, 31, 11, "AP");
		Text(skp, 31, 13, "RA");
		Text(skp, 31, 15, "f");
		Text(skp, 47, 5, "NV");
		Text(skp, 44, 7, "WT");
		Text(skp, 44, 8, "WC");
		Text(skp, 44, 9, "SPS");
		Text(skp, 44, 10, "RCSC");
		Text(skp, 44, 11, "WL");
		Text(skp, 44, 12, "APS");
		Text(skp, 44, 13, "DPS");
		Text(skp, 44, 14, "RCSL");
		Text(skp, 44, 15, "J2");
		Text(skp, 1, 17, "LO/C");
		Text(skp, 1, 18, "GRR/C");
		Text(skp, 1, 19, "ZS/C");
		Text(skp, 1, 20, "GRR/S");
		Text(skp, 1, 21, "LO/L");
		Text(skp, 1, 22, "ZS/L");
		Text(skp, 1, 23, "ZS/A");
		Text(skp, 1, 24, "EPHB");
		Text(skp, 1, 25, "EPHE");
		Text(skp, 1, 26, "THT");
		Text(skp, 20, 17, "HA");
		Text(skp, 20, 18, "HP");
		Text(skp, 20, 19, "HO");
		Text(skp, 20, 20, "HO");
		Text(skp, 20, 21, "PD");
		Text(skp, 20, 22, "LD");
		Text(skp, 20, 23, "R");
		Text(skp, 34, 17, "m");
		Text(skp, 46, 17, "VM");
		Text(skp, 36, 18, "dL");
		Text(skp, 41, 24, "LSB");
		Text(skp, 41, 25, "LLS");
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text_GET_HHHMMSSCS(skp, 17, 1, GC->rtcc->EZCHECKDIS.GET);
		Text(skp, 26, 1, "%d", GC->rtcc->EZCHECKDIS.R_Day[0]);
		Text(skp, 29, 1, "%d", GC->rtcc->EZCHECKDIS.R_Day[1]);
		Text(skp, 32, 1, "%d", GC->rtcc->EZCHECKDIS.R_Day[2]);
		Text(skp, 43, 1, "%.2lf", GC->rtcc->EZCHECKDIS.K_Fac);
		Text_GET_HHHMMSSCS(skp, 17, 2, GC->rtcc->EZCHECKDIS.GMT);
		Text(skp, 29, 2, GC->rtcc->EZCHECKDIS.VID);
		Text(skp, 36, 2, GC->rtcc->EZCHECKDIS.RF);
		Text(skp, 46, 2, "%.2lf", GC->rtcc->EZCHECKDIS.A);
		Text(skp, 54, 2, GC->rtcc->EZCHECKDIS.CFG);
		Text(skp, 11, 3, GC->rtcc->EZCHECKDIS.VEH);
		if (GC->rtcc->EZCHECKDIS.U_T.x > -1)
		{
			Text(skp, 26, 3, "%.7lf", GC->rtcc->EZCHECKDIS.U_T.x);
			Text(skp, 40, 3, "%.7lf", GC->rtcc->EZCHECKDIS.U_T.y);
			Text(skp, 54, 3, "%.7lf", GC->rtcc->EZCHECKDIS.U_T.z);
		}
		Text(skp, 54, 4, GC->rtcc->EZCHECKDIS.Option);
		if (GC->rtcc->EZCHECKDIS.NV > 0) Text(skp, 51, 5, "%d", (int)GC->rtcc->EZCHECKDIS.NV);
		if (GC->rtcc->EZCHECKDIS.unit == 0)
		{
			Text(skp, 17, 5, "%+013.9lf", GC->rtcc->EZCHECKDIS.Pos.x);
			Text(skp, 17, 7, "%+013.9lf", GC->rtcc->EZCHECKDIS.Pos.y);
			Text(skp, 17, 9, "%+013.9lf", GC->rtcc->EZCHECKDIS.Pos.z);
			Text(skp, 17, 11, "%+013.9lf", GC->rtcc->EZCHECKDIS.Vel.x);
			Text(skp, 17, 13, "%+013.9lf", GC->rtcc->EZCHECKDIS.Vel.y);
			Text(skp, 17, 15, "%+013.9lf", GC->rtcc->EZCHECKDIS.Vel.z);
		}
		else
		{
			Text(skp, 17, 5, "%+012.0lf", GC->rtcc->EZCHECKDIS.Pos.x);
			Text(skp, 17, 7, "%+012.0lf", GC->rtcc->EZCHECKDIS.Pos.y);
			Text(skp, 17, 9, "%+012.0lf", GC->rtcc->EZCHECKDIS.Pos.z);
			Text(skp, 17, 11, "%+013.6lf", GC->rtcc->EZCHECKDIS.Vel.x);
			Text(skp, 17, 13, "%+013.6lf", GC->rtcc->EZCHECKDIS.Vel.y);
			Text(skp, 17, 15, "%+013.6lf", GC->rtcc->EZCHECKDIS.Vel.z);
		}
		Text_GET_HHHMMSSCS(skp, 19, 17, GC->rtcc->EZCHECKDIS.LOC);
		Text_GET_HHHMMSSCS(skp, 19, 18, GC->rtcc->EZCHECKDIS.GRRC);
		Text_GET_HHHMMSSCS(skp, 19, 19, GC->rtcc->EZCHECKDIS.ZSC);
		Text_GET_HHHMMSSCS(skp, 19, 20, GC->rtcc->EZCHECKDIS.GRRS);
		Text_GET_HHHMMSSCS(skp, 19, 21, GC->rtcc->EZCHECKDIS.LOL);
		Text_GET_HHHMMSSCS(skp, 19, 22, GC->rtcc->EZCHECKDIS.ZSL);
		Text_GET_HHHMMSSCS(skp, 19, 23, GC->rtcc->EZCHECKDIS.ZSA);
		Text_GET_HHHMMSSC(skp, 19, 24, GC->rtcc->EZCHECKDIS.EPHB);
		Text_GET_HHHMMSSC(skp, 19, 25, GC->rtcc->EZCHECKDIS.EPHE);
		if (GC->rtcc->EZCHECKDIS.THT > 0) Text_GET_HHHMMSSC(skp, 19, 26, GC->rtcc->EZCHECKDIS.THT);
		Text(skp, 30, 5, "%.3lf", GC->rtcc->EZCHECKDIS.V_i);
		Text(skp, 30, 7, "%+.5lf", GC->rtcc->EZCHECKDIS.gamma_i);
		Text(skp, 30, 9, "%.5lf", GC->rtcc->EZCHECKDIS.psi);
		Text_Latitude(skp, 30, 11, GC->rtcc->EZCHECKDIS.phi_c, 4);
		Text_Longitude(skp, 30, 13, GC->rtcc->EZCHECKDIS.lambda, 4);
		Text(skp, 30, 15, "%.3lf", GC->rtcc->EZCHECKDIS.h_s);
		Text(skp, 44, 5, "%.3lf", GC->rtcc->EZCHECKDIS.a);
		Text(skp, 41, 7, "%.5lf", GC->rtcc->EZCHECKDIS.e);
		Text(skp, 42, 9, "%.5lf", GC->rtcc->EZCHECKDIS.i);
		Text(skp, 43, 11, "%.5lf", GC->rtcc->EZCHECKDIS.omega_p);
		Text(skp, 43, 13, "%.5lf", GC->rtcc->EZCHECKDIS.Omega);
		if (GC->rtcc->EZCHECKDIS.TABlank == false) Text(skp, 43, 15, "%.6lf", GC->rtcc->EZCHECKDIS.nu);
		if (GC->rtcc->EZCHECKDIS.MABlank == false) Text(skp, 45, 17, "%.5lf", GC->rtcc->EZCHECKDIS.m);
		Text(skp, 56, 7, "%.1lf", GC->rtcc->EZCHECKDIS.WT);
		if (GC->rtcc->EZCHECKDIS.WC >= 0) Text(skp, 56, 8, "%.1lf", GC->rtcc->EZCHECKDIS.WC);
		if (GC->rtcc->EZCHECKDIS.SPS >= 0) Text(skp, 56, 9, "%.1lf", GC->rtcc->EZCHECKDIS.SPS);
		if (GC->rtcc->EZCHECKDIS.RCS_C >= 0) Text(skp, 56, 10, "%.1lf", GC->rtcc->EZCHECKDIS.RCS_C);
		if (GC->rtcc->EZCHECKDIS.WL >= 0) Text(skp, 56, 11, "%.1lf", GC->rtcc->EZCHECKDIS.WL);
		if (GC->rtcc->EZCHECKDIS.APS >= 0) Text(skp, 56, 12, "%.1lf", GC->rtcc->EZCHECKDIS.APS);
		if (GC->rtcc->EZCHECKDIS.DPS >= 0) Text(skp, 56, 13, "%.1lf", GC->rtcc->EZCHECKDIS.DPS);
		if (GC->rtcc->EZCHECKDIS.RCS_L >= 0) Text(skp, 56, 14, "%.1lf", GC->rtcc->EZCHECKDIS.RCS_L);
		if (GC->rtcc->EZCHECKDIS.J2 >= 0) Text(skp, 56, 15, "%.1lf", GC->rtcc->EZCHECKDIS.J2);
		Text(skp, 33, 17, "%.3lf", GC->rtcc->EZCHECKDIS.h_a);
		Text(skp, 34, 18, "%+.3lf", GC->rtcc->EZCHECKDIS.h_p);
		if (GC->rtcc->EZCHECKDIS.HOBlank == false)
		{
			Text(skp, 33, 19, "%.3lf", GC->rtcc->EZCHECKDIS.h_o_NM);
			Text(skp, 32, 20, "%.0lf", GC->rtcc->EZCHECKDIS.h_o_ft);
			Text_Latitude(skp, 32, 21, GC->rtcc->EZCHECKDIS.phi_D, 4);
			Text_Longitude(skp, 32, 22, GC->rtcc->EZCHECKDIS.lambda_D, 4);
		}
		Text(skp, 33, 23, "%.3lf", GC->rtcc->EZCHECKDIS.R);
		Text(skp, 48, 18, "%+.5lf", GC->rtcc->EZCHECKDIS.deltaL);
		if (GC->rtcc->EZCHECKDIS.LSTBlank == false)
		{
			Text_GET_HHHMMSSC(skp, 56, 24, GC->rtcc->EZCHECKDIS.LAL);
			Text_GET_HHHMMSSC(skp, 56, 25, GC->rtcc->EZCHECKDIS.LOL);
		}
		Text(skp, 14, 27, GC->rtcc->EZCHECKDIS.ErrorMessage);
		break;
	case 59:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "MPT INITIALIZATION", 18);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);

		skp->Text(CW, (marker + 7) * H / 16, "*", 1);

		if (GC->mptInitError == 1)
		{
			skp->Text(CW * 15, 13 * H / 14, "Update successful!", 18);
		}
		else if (GC->mptInitError == 2)
		{
			skp->Text(CW * 15, 13 * H / 14, "Update rejected!", 16);
		}

		if (GC->rtcc->med_m49.Table == RTCC_MPT_LM)
		{
			skp->Text(CW * 2, 2 * H / 14, "Table: LM", 9);
		}
		else
		{
			skp->Text(CW * 2, 2 * H / 14, "Table: CSM", 10);
		}
		x = 17;	y = 7;
		skp->Text(W - CW * x, 6 * H / 16, "MPT:", 4);
		if (G->mptinitmode == 0)
		{
			skp->Text(CW * 2, 4 * H / 14, "MED M49: Fuel Remaining", 23);
			if (GC->rtcc->med_m49.SPSFuelRemaining >= 0)
			{
				sprintf(Buffer, "SPS: %.0f lbs", GC->rtcc->med_m49.SPSFuelRemaining / 0.45359237);
			}
			else
			{
				sprintf(Buffer, "SPS: No Update");
			}
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			if (GC->rtcc->med_m49.CSMRCSFuelRemaining >= 0)
			{
				sprintf(Buffer, "CSM RCS: %.0f lbs", GC->rtcc->med_m49.CSMRCSFuelRemaining / 0.45359237);
			}
			else
			{
				sprintf(Buffer, "CSM RCS: No Update");
			}
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;

			if (GC->rtcc->med_m49.SIVBFuelRemaining >= 0)
			{
				sprintf(Buffer, "S-IVB: %.0f lbs", GC->rtcc->med_m49.SIVBFuelRemaining / 0.45359237);
			}
			else
			{
				sprintf(Buffer, "S-IVB: No Update");
			}
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			if (GC->rtcc->med_m49.LMAPSFuelRemaining >= 0)
			{
				sprintf(Buffer, "LM APS: %.0f lbs", GC->rtcc->med_m49.LMAPSFuelRemaining / 0.45359237);
			}
			else
			{
				sprintf(Buffer, "LM APS: No Update");
			}
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			if (GC->rtcc->med_m49.LMRCSFuelRemaining >= 0)
			{
				sprintf(Buffer, "LM RCS: %.0f lbs", GC->rtcc->med_m49.LMRCSFuelRemaining / 0.45359237);
			}
			else
			{
				sprintf(Buffer, "LM RCS: No Update");
			}
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			if (GC->rtcc->med_m49.LMDPSFuelRemaining >= 0)
			{
				sprintf(Buffer, "LM DPS: %.0f lbs", GC->rtcc->med_m49.LMDPSFuelRemaining / 0.45359237);
			}
			else
			{
				sprintf(Buffer, "LM DPS: No Update");
			}
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			MissionPlanTable *mpt = GC->rtcc->GetMPTPointer(GC->rtcc->med_m49.Table);
			y = 7;
			sprintf(Buffer, "%.0f lbs", mpt->CommonBlock.SPSFuelRemaining / 0.45359237);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "%.0f lbs", mpt->CommonBlock.CSMRCSFuelRemaining / 0.45359237);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "%.0f lbs", mpt->CommonBlock.SIVBFuelRemaining / 0.45359237);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "%.0f lbs", mpt->CommonBlock.LMAPSFuelRemaining / 0.45359237);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "%.0f lbs", mpt->CommonBlock.LMRCSFuelRemaining / 0.45359237);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "%.0f lbs", mpt->CommonBlock.LMDPSFuelRemaining / 0.45359237);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
		}
		else if (G->mptinitmode == 1)
		{
			skp->Text(CW * 2, 4 * H / 14, "MED M50: Weights", 16);

			if (GC->rtcc->med_m50.CSMWT >= 0)
			{
				sprintf(Buffer, "CSM: %.0f lbs", GC->rtcc->med_m50.CSMWT / 0.45359237);
			}
			else
			{
				sprintf(Buffer, "CSM: No Update");
			}
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;

			if (GC->rtcc->med_m50.SIVBWT >= 0)
			{
				sprintf(Buffer, "S-IVB: %.0f lbs", GC->rtcc->med_m50.SIVBWT / 0.45359237);
			}
			else
			{
				sprintf(Buffer, "S-IVB: No Update");
			}
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			if (GC->rtcc->med_m50.LMWT >= 0)
			{
				sprintf(Buffer, "LM Total: %.0f lbs", GC->rtcc->med_m50.LMWT / 0.45359237);
			}
			else
			{
				sprintf(Buffer, "LM Total: No Update");
			}
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			if (GC->rtcc->med_m50.LMASCWT >= 0)
			{
				sprintf(Buffer, "LM Ascent: %.0f lbs", GC->rtcc->med_m50.LMASCWT / 0.45359237);
			}
			else
			{
				sprintf(Buffer, "LM Ascent: No Update");
			}
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			GET_Display(Buffer, GC->rtcc->med_m50.WeightGET, true);
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			MissionPlanTable *mpt = GC->rtcc->GetMPTPointer(GC->rtcc->med_m50.Table);
			y = 7;
			sprintf(Buffer, "CSM %.0f lbs", mpt->CommonBlock.CSMMass / 0.45359237);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "S-IVB %.0f lbs", mpt->CommonBlock.SIVBMass / 0.45359237);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "LM ASC %.0f lbs", mpt->CommonBlock.LMAscentMass / 0.45359237);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "LM DSC %.0f lbs", mpt->CommonBlock.LMDescentMass / 0.45359237);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			skp->Text(W - CW * x, y * H / 16, "Total:", 6); y++;
			sprintf(Buffer, "%.0f lbs", mpt->TotalInitMass / 0.45359237);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
		}
		else if (G->mptinitmode == 2)
		{
			skp->Text(CW * 2, 4 * H / 14, "MED M51: Areas", 14);

			sprintf(Buffer, "CSM %.2f sq ft", GC->rtcc->med_m51.CSMArea / 0.3048 / 0.3048);
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "S-IVB %.2f sq ft", GC->rtcc->med_m51.SIVBArea / 0.3048 / 0.3048);
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "LM ASC %.2f sq ft", GC->rtcc->med_m51.LMAscentArea / 0.3048 / 0.3048);
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "LM DSC %.2f sq ft", GC->rtcc->med_m51.LMDescentArea / 0.3048 / 0.3048);
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "K-Factor %.2f", GC->rtcc->med_m51.KFactor);
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			MissionPlanTable *mpt = GC->rtcc->GetMPTPointer(GC->rtcc->med_m51.Table);
			y = 7;
			sprintf(Buffer, "%.2f sq ft", mpt->CommonBlock.CSMArea / 0.3048 / 0.3048);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "%.2f sq ft", mpt->CommonBlock.SIVBArea / 0.3048 / 0.3048);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "%.2f sq ft", mpt->CommonBlock.LMAscentArea / 0.3048 / 0.3048);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "%.2f sq ft", mpt->CommonBlock.LMDescentArea / 0.3048 / 0.3048);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "%.2f", mpt->KFactor);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			skp->Text(W - CW * x, y * H / 16, "Total:", 6); y++;
			sprintf(Buffer, "%.2f sq ft", mpt->ConfigurationArea / 0.3048 / 0.3048);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
		}
		else
		{
			skp->Text(CW * 2, 4 * H / 14, "MED M55: Configuration", 22);

			if (GC->rtcc->med_m55.ConfigCode == "")
			{
				sprintf(Buffer, "Config: None");
			}
			else
			{
				sprintf(Buffer, "Config: %s", GC->rtcc->med_m55.ConfigCode.c_str());
			}
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "T BV:");
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer));
			if (GC->rtcc->med_m55.VentingGET >= 0)
			{
				GET_Display(Buffer, GC->rtcc->med_m55.VentingGET);
			}
			else
			{
				sprintf(Buffer, "No Update");
			}
			skp->Text(CW * 8, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "DDA:");
			skp->Text(CW * 3, y * H / 16, Buffer, strlen(Buffer));
			if (GC->rtcc->med_m55.DeltaDockingAngle >= -360.0)
			{
				sprintf(Buffer, "%.2f°", GC->rtcc->med_m55.DeltaDockingAngle*DEG);
			}
			else
			{
				sprintf(Buffer, "No Update");
			}
			skp->Text(CW * 8, y * H / 16, Buffer, strlen(Buffer));

			MissionPlanTable *mpt = GC->rtcc->GetMPTPointer(GC->rtcc->med_m55.Table);
			y = 7;
			if (mpt->CommonBlock.ConfigCode == 0)
			{
				sprintf(Buffer, "None");
			}
			else
			{
				GC->rtcc->MPTGetStringFromConfig(mpt->CommonBlock.ConfigCode, Buffer);
			}
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			GET_Display(Buffer, mpt->SIVBVentingBeginGET);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
			sprintf(Buffer, "%.2f°", mpt->DeltaDockingAngle*DEG);
			skp->Text(W - CW * x, y * H / 16, Buffer, strlen(Buffer)); y++;
		}

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (GC->rtcc->med_m49.Table == RTCC_MPT_CSM)
		{
			PrintCSMVessel(Buffer);
		}
		else
		{
			PrintLMVessel(Buffer);
		}
		skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));
		break;
	case 60:
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 19, 0, "LM DESCENT PLANNING");
		Text(skp, 52, 0, "0086");
		Text(skp, 0, 1, "STA ID");
		Text(skp, 0, 2, "LM WT");
		Text(skp, 15, 1, "GMTV");
		Text(skp, 15, 2, "GETV");
		Text(skp, 32, 1, "MODE");
		Text(skp, 32, 2, "TL");
		Text(skp, 41, 1, "LAT LLS");
		Text(skp, 41, 2, "LNG LLS");
		Text(skp, 0, 7, "MVR/REV GETTH/GETIG LIG/DV HAC/HPC DEL/THPC DVX/DVY DVZ", 1, 2);
		Text(skp, 1, 17, "POWERED DESCENT");
		Text(skp, 1, 18, "THETA IGN");
		Text(skp, 1, 19, "PROP.REM");
		Text(skp, 18, 17, "GETTH");
		Text(skp, 18, 18, "GETIG");
		Text(skp, 18, 19, "GETTD");
		Text(skp, 36, 17, "MODE");
		Text(skp, 36, 18, "DESC AZ");
		Text(skp, 36, 19, "SN.LK.A");
		Text(skp, 18, 21, "GATE CONDITIONS");
		Text(skp, 10, 22, "PET  H DOT   H   RD GO  R GO    PH  TH LA PCT");
		Text(skp, 0, 23, "IGNITION");
		Text(skp, 0, 24, "THTL DWN");
		Text(skp, 1, 25, "HI GATE");
		Text(skp, 1, 26, "LO GATE");
		Text(skp, 2, 27, "HOVER");
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text(skp, 40, 1, "%d", GC->rtcc->PZLDPDIS.MODE);
		Text(skp, 13, 2, "%.1lf", GC->rtcc->PZLDPDIS.LMWT);
		Text_GET_HHHMMSSC(skp, 31, 1, GC->rtcc->PZLDPDIS.GMTV);
		Text_GET_HHHMMSSC(skp, 31, 2, GC->rtcc->PZLDPDIS.GETV);
		Text_Latitude(skp, 57, 1, GC->rtcc->PZLDPDIS.LAT_LLS, 2);
		Text_Longitude(skp, 57, 2, GC->rtcc->PZLDPDIS.LONG_LLS, 2);
		for (int i = 0; i < 4; i++)
		{
			Text(skp, 5, 5 + i * 3, GC->rtcc->PZLDPDIS.MVR[i]);
			Text_GET_HHHMMSSC(skp, 19, 5 + i * 3, GC->rtcc->PZLDPDIS.GETTH[i]);
			Text_GET_HHHMMSSC(skp, 19, 6 + i * 3, GC->rtcc->PZLDPDIS.GETIG[i]);
			Text_Longitude(skp, 27, 5 + i * 3, GC->rtcc->PZLDPDIS.LIG[i], 2);
			Text(skp, 27, 6 + i * 3, "%.2lf", GC->rtcc->PZLDPDIS.DV[i]);
			Text(skp, 34, 5 + i * 3, "%.2lf", GC->rtcc->PZLDPDIS.AC[i]);
			Text(skp, 34, 6 + i * 3, "%.2lf", GC->rtcc->PZLDPDIS.HPC[i]);
			Text(skp, 49, 5 + i * 3, "%+.1lf", GC->rtcc->PZLDPDIS.DVVector[i].x);
			Text(skp, 49, 6 + i * 3, "%+.1lf", GC->rtcc->PZLDPDIS.DVVector[i].y);
			Text(skp, 57, 5 + i * 3, "%+.1lf", GC->rtcc->PZLDPDIS.DVVector[i].z);
		}
		Text(skp, 16, 18, "%.2lf", GC->rtcc->PZLDPDIS.PD_ThetaIgn);
		Text_GET_HHHMMSSC(skp, 35, 17, GC->rtcc->PZLDPDIS.PD_GETTH);
		Text_GET_HHHMMSSC(skp, 35, 18, GC->rtcc->PZLDPDIS.PD_GETIG);
		Text_GET_HHHMMSSC(skp, 35, 19, GC->rtcc->PZLDPDIS.PD_GETTD);
		Text(skp, 51, 18, "%.3lf", GC->rtcc->PZLDPDIS.DescAsc);
		Text(skp, 51, 19, "%.3lf", GC->rtcc->PZLDPDIS.SN_LK_A);
		if (GC->rtcc->PZLDPDIS.error != 0)
		{
			Text(skp, 15, 27, "Error %d", GC->rtcc->PZLDPDIS.error);
		}
		break;
	case 61:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 4, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(4);
		Text(skp, 8, 0, "LGC DESCENT TARGET UPDATE");
		Text(skp, 38, 0, "0295");

		Text(skp, 1, 1, "LOAD NO");
		Text(skp, 15, 1, "GETGEN");
		Text(skp, 32, 1, "SITES");
		Text(skp, 1, 2, "TLAND");
		Text(skp, 3, 3, "OID");
		Text(skp, 10, 3, "FCT");
		Text(skp, 16, 3, "DSKY V72");
		y = 4;
		Text(skp, 9, y, "INDEX"); y++;
		Text(skp, 9, y, "ADD1"); y++;
		Text(skp, 9, y, "TLAND"); y++;
		Text(skp, 9, y, "ADD2"); y++;
		Text(skp, 9, y, "TLAND"); y++;
		for (y = 1; y <= 5; y++)
		{
			Text(skp, 4, y + 3, "%02o", y);
		}
		Line(skp, CW, CH * 3, CW * 42, CH * 3);
		Line(skp, CW, CH * 4, CW * 42, CH * 4);
		Line(skp, (CW * 15) / 2, CH * 3, (CW * 15) / 2, CH * 21);
		Line(skp, (CW * 31) / 2, CH * 3, (CW * 31) / 2, CH * 21);
		Line(skp, (CW * 49) / 2, CH * 3, (CW * 49) / 2, CH * 21);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 4, true);
		Text(skp, 13, 1, "%04d", GC->rtcc->CZTDTGTU.SequenceNumber);
		Text_GET_HHHMMSS(skp, 31, 1, GC->rtcc->CZTDTGTU.GETofGeneration);
		Text_GET_HHHMMSSCS(skp, 19, 2, GC->rtcc->CZTDTGTU.GETTD);
		for (int i = 0; i < 5; i++)
		{
			Text(skp, 22, 4 + i, "%05d", GC->rtcc->CZTDTGTU.Octals[i]);
		}
		break;
	case 62:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 2, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(2);
		Text(skp, 23, 0, "SUNRISE/SUNSET TIMES");
		Text(skp, 58, 0, "1502");
		Text(skp, 23, 2, "i");
		Text(skp, 36, 2, "LAN");
		//TBD
		Text(skp, 15, 7, "SUNRISE", 1, 2);
		Text(skp, 46, 7, "SUNSET", 1, 2);
		Text(skp, 1, 5, "REV", 2, 1);
		Text(skp, 6, 5, "GETTR");
		Text(skp, 16, 5, "GETSR");
		Text(skp, 24, 5, "PIT");
		Text(skp, 29, 5, "YAW");
		Text(skp, 37, 5, "GETTS");
		Text(skp, 47, 5, "GETSS");
		Text(skp, 55, 5, "PIT");
		Text(skp, 60, 5, "YAW");
		Text(skp, 1, 6, "NO");
		Text(skp, 4, 6, "HR MN SEC");
		Text(skp, 14, 6, "HR MN SEC");
		Text(skp, 24, 6, "DEG");
		Text(skp, 29, 6, "DEG");
		Text(skp, 35, 6, "HR MN SEC");
		Text(skp, 45, 6, "HR MN SEC");
		Text(skp, 55, 6, "DEG");
		Text(skp, 60, 6, "DEG");
		y = (CH * 13) / 4;
		dy = (CH * 19) / 4;
		Line(skp, 0, y, CW * 64, y);
		Line(skp, 0, dy, CW * 64, dy);
		Line(skp, 0, (CH * 15) / 2, CW * 64, (CH * 15) / 2);
		Line(skp, (CW * 7) / 2, y, (CW * 7) / 2, CH * 27);
		Line(skp, (CW * 27) / 2, dy, (CW * 27) / 2, CH * 27);
		Line(skp, (CW * 47) / 2, dy, (CW * 47) / 2, CH * 27);
		Line(skp, (CW * 55) / 2, dy, (CW * 55) / 2, CH * 27);
		Line(skp, (CW * 67) / 2, y, (CW * 67) / 2, CH * 27);
		Line(skp, (CW * 89) / 2, dy, (CW * 89) / 2, CH * 27);
		Line(skp, (CW * 109) / 2, dy, (CW * 109) / 2, CH * 27);
		Line(skp, (CW * 117) / 2, dy, (CW * 117) / 2, CH * 27);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 2, true);
		Text(skp, 40, 31, GC->rtcc->EZSSTAB.errormessage);
		for (int i = 0; i < 8; i++)
		{
			Text(skp, 3, 9 + i * 2, "%d", GC->rtcc->EZSSTAB.data[i].REV);
			if (GC->rtcc->EZSSTAB.data[i].BestAvailableGETTR) Text(skp, 4, 9 + i * 2, "*");
			Text_GET_HHHMMSS(skp, 13, 9 + i * 2, GC->rtcc->EZSSTAB.data[i].GETTR);
			if (GC->rtcc->EZSSTAB.data[i].BestAvailableGETSR) Text(skp, 14, 9 + i * 2, "*");
			Text_GET_HHHMMSS(skp, 23, 9 + i * 2, GC->rtcc->EZSSTAB.data[i].GETSR);
			Text(skp, 27, 9 + i * 2, "%+.0lf", GC->rtcc->EZSSTAB.data[i].theta_SR);
			Text(skp, 32, 9 + i * 2, "%+.0lf", GC->rtcc->EZSSTAB.data[i].psi_SR);

			if (GC->rtcc->EZSSTAB.data[i].BestAvailableGETTS) Text(skp, 35, 9 + i * 2, "*");
			Text_GET_HHHMMSS(skp, 44, 9 + i * 2, GC->rtcc->EZSSTAB.data[i].GETTS);
			if (GC->rtcc->EZSSTAB.data[i].BestAvailableGETSS) Text(skp, 45, 9 + i * 2, "*");
			Text_GET_HHHMMSS(skp, 54, 9 + i * 2, GC->rtcc->EZSSTAB.data[i].GETSS);
			Text(skp, 58, 9 + i * 2, "%+.0lf", GC->rtcc->EZSSTAB.data[i].theta_SS);
			Text(skp, 63, 9 + i * 2, "%+.0lf", GC->rtcc->EZSSTAB.data[i].psi_SS);
		}
		break;
	case 63:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 2, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(2);
		Text(skp, 21, 0, "MOONRISE/MOONSET TIMES");
		Text(skp, 58, 0, "1501");
		Text(skp, 23, 2, "i");
		Text(skp, 36, 2, "LAN");
		Text(skp, 15, 7, "MOONRISE", 1, 2);
		Text(skp, 46, 7, "MOONSET", 1, 2);
		Text(skp, 1, 5, "REV", 2, 1);
		Text(skp, 6, 5, "GETTR");
		Text(skp, 16, 5, "GETMR");
		Text(skp, 24, 5, "PIT");
		Text(skp, 29, 5, "YAW");
		Text(skp, 37, 5, "GETTS");
		Text(skp, 47, 5, "GETMS");
		Text(skp, 55, 5, "PIT");
		Text(skp, 60, 5, "YAW");
		Text(skp, 1, 6, "NO");
		Text(skp, 4, 6, "HR MN SEC");
		Text(skp, 14, 6, "HR MN SEC");
		Text(skp, 24, 6, "DEG");
		Text(skp, 29, 6, "DEG");
		Text(skp, 35, 6, "HR MN SEC");
		Text(skp, 45, 6, "HR MN SEC");
		Text(skp, 55, 6, "DEG");
		Text(skp, 60, 6, "DEG");
		y = (CH * 13) / 4;
		dy = (CH * 19) / 4;
		Line(skp, 0, y, CW * 64, y);
		Line(skp, 0, dy, CW * 64, dy);
		Line(skp, 0, (CH * 15) / 2, CW * 64, (CH * 15) / 2);
		Line(skp, (CW * 7) / 2, y, (CW * 7) / 2, CH * 27);
		Line(skp, (CW * 27) / 2, dy, (CW * 27) / 2, CH * 27);
		Line(skp, (CW * 47) / 2, dy, (CW * 47) / 2, CH * 27);
		Line(skp, (CW * 55) / 2, dy, (CW * 55) / 2, CH * 27);
		Line(skp, (CW * 67) / 2, y, (CW * 67) / 2, CH * 27);
		Line(skp, (CW * 89) / 2, dy, (CW * 89) / 2, CH * 27);
		Line(skp, (CW * 109) / 2, dy, (CW * 109) / 2, CH * 27);
		Line(skp, (CW * 117) / 2, dy, (CW * 117) / 2, CH * 27);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 2, true);
		Text(skp, 40, 31, GC->rtcc->EZMMTAB.errormessage);
		for (int i = 0; i < 8; i++)
		{
			Text(skp, 3, 9 + i * 2, "%d", GC->rtcc->EZMMTAB.data[i].REV);
			if (GC->rtcc->EZMMTAB.data[i].BestAvailableGETTR) Text(skp, 4, 9 + i * 2, "*");
			Text_GET_HHHMMSS(skp, 13, 9 + i * 2, GC->rtcc->EZMMTAB.data[i].GETTR);
			if (GC->rtcc->EZMMTAB.data[i].BestAvailableGETSR) Text(skp, 14, 9 + i * 2, "*");
			Text_GET_HHHMMSS(skp, 23, 9 + i * 2, GC->rtcc->EZMMTAB.data[i].GETSR);
			Text(skp, 27, 9 + i * 2, "%+.0lf", GC->rtcc->EZMMTAB.data[i].theta_SR);
			Text(skp, 32, 9 + i * 2, "%+.0lf", GC->rtcc->EZMMTAB.data[i].psi_SR);

			if (GC->rtcc->EZMMTAB.data[i].BestAvailableGETTS) Text(skp, 35, 9 + i * 2, "*");
			Text_GET_HHHMMSS(skp, 44, 9 + i * 2, GC->rtcc->EZMMTAB.data[i].GETTS);
			if (GC->rtcc->EZMMTAB.data[i].BestAvailableGETSS) Text(skp, 45, 9 + i * 2, "*");
			Text_GET_HHHMMSS(skp, 54, 9 + i * 2, GC->rtcc->EZMMTAB.data[i].GETSS);
			Text(skp, 58, 9 + i * 2, "%+.0lf", GC->rtcc->EZMMTAB.data[i].theta_SS);
			Text(skp, 63, 9 + i * 2, "%+.0lf", GC->rtcc->EZMMTAB.data[i].psi_SS);
		}
		break;
	case 64: //FDO Launch Analog No. 1
		if (oapiGetSimTime() > GC->rtcc->fdolaunchanalog1tab.LastUpdateTime + 0.5)
		{
			if (GC->rtcc->pCSM)
			{
				EphemerisData sv = GC->rtcc->StateVectorCalcEphem(GC->rtcc->pCSM);
				GC->rtcc->FDOLaunchAnalog1(sv);
			}
		}

		skp->SetPen(pen2);
		skp->SetFont(font_mocr3);
		GetCharSize(skp, CW, CH);
		Text(skp, 17, 0, "FDO LAUNCH ANALOG NO 1");
		Text(skp, 52, 0, "0040");

		skp->SetTextAlign(oapi::Sketchpad::CENTER, oapi::Sketchpad::BASELINE);
		skp->SetFont(font_mocr_plot);
		skp->Text((CW * 14) / 8, (CH * 91) / 20, "2.0", 3);
		skp->Text((CW * 35) / 8, (CH * 91) / 20, "35", 2);
		skp->Text((CW * 56) / 8, (CH * 91) / 20, "40", 2);

		skp->Text((CW * 14) / 8, (CH * 140) / 20, "1.5", 3);
		skp->Text((CW * 35) / 8, (CH * 140) / 20, "30", 2);
		skp->Text((CW * 56) / 8, (CH * 140) / 20, "35", 2);


		skp->Text((CW * 14) / 8, (CH * 189) / 20, "1.0", 3);
		skp->Text((CW * 35) / 8, (CH * 189) / 20, "25", 2);
		skp->Text((CW * 56) / 8, (CH * 189) / 20, "30", 2);

		skp->Text((CW * 35) / 8, (CH * 427) / 40, "(y)", 3);

		skp->Text((CW * 14) / 8, (CH * 238) / 20, ".5", 2);
		skp->Text((CW * 35) / 8, (CH * 238) / 20, "20", 2);
		skp->Text((CW * 56) / 8, (CH * 238) / 20, "25", 2);

		skp->Text((CW * 14) / 8, (CH * 287) / 20, "0", 1);
		skp->Text((CW * 35) / 8, (CH * 287) / 20, "15", 2);
		skp->Text((CW * 56) / 8, (CH * 287) / 20, "20", 2);

		skp->Text((CW * 14) / 8, (CH * 336) / 20, "-.5", 3);
		skp->Text((CW * 35) / 8, (CH * 336) / 20, "10", 2);
		skp->Text((CW * 56) / 8, (CH * 336) / 20, "15", 2);

		skp->Text((CW * 14) / 8, (CH * 385) / 20, "-1.0", 4);
		skp->Text((CW * 35) / 8, (CH * 385) / 20, "5", 1);
		skp->Text((CW * 56) / 8, (CH * 385) / 20, "10", 2);

		skp->Text((CW * 14) / 8, (CH * 434) / 20, "-1.5", 4);
		skp->Text((CW * 35) / 8, (CH * 434) / 20, "0", 1);
		skp->Text((CW * 56) / 8, (CH * 434) / 20, "5", 1);

		skp->Text((CW * 14) / 8, (CH * 483) / 20, "-2.0", 4);
		skp->Text((CW * 35) / 8, (CH * 483) / 20, "-5", 2);
		skp->Text((CW * 56) / 8, (CH * 483) / 20, "0", 1);

		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(3 * CW * 56 / 20, 21 * CH * 28 / 24, "0", 1);
		skp->Text(3 * CW * 56 / 20, 22 * CH * 28 / 24, "5", 1);
		skp->Text(3 * CW * 56 / 20, 23 * CH * 28 / 24, "21", 2);

		skp->Text(13 * CW * 56 / 60, 22 * CH * 28 / 24, "6.5", 3);

		skp->Text(17 * CW * 56 / 60, 21 * CH * 28 / 24, "1", 1);
		skp->Text(17 * CW * 56 / 60, 22 * CH * 28 / 24, "8", 1);
		skp->Text(17 * CW * 56 / 60, 23 * CH * 28 / 24, "22", 2);

		skp->Text(21 * CW * 56 / 60, 22 * CH * 28 / 24, "9.5", 3);

		skp->Text(25 * CW * 56 / 60, 21 * CH * 28 / 24, "2", 1);
		skp->Text(25 * CW * 56 / 60, 22 * CH * 28 / 24, "11", 2);
		skp->Text(25 * CW * 56 / 60, 23 * CH * 28 / 24, "23", 2);

		skp->Text(29 * CW * 56 / 60, 22 * CH * 28 / 24, "12.5", 4);
		skp->Text(29 * CW * 56 / 60, 23 * CH * 28 / 24, "(V)", 3);

		skp->Text(33 * CW * 56 / 60, 21 * CH * 28 / 24, "3", 1);
		skp->Text(33 * CW * 56 / 60, 22 * CH * 28 / 24, "14", 2);
		skp->Text(33 * CW * 56 / 60, 23 * CH * 28 / 24, "24", 2);

		skp->Text(37 * CW * 56 / 60, 22 * CH * 28 / 24, "15.5", 4);

		skp->Text(41 * CW * 56 / 60, 21 * CH * 28 / 24, "4", 1);
		skp->Text(41 * CW * 56 / 60, 22 * CH * 28 / 24, "17", 2);
		skp->Text(41 * CW * 56 / 60, 23 * CH * 28 / 24, "25", 2);

		skp->Text(45 * CW * 56 / 60, 22 * CH * 28 / 24, "18.5", 4);

		skp->Text(49 * CW * 56 / 60, 21 * CH * 28 / 24, "5", 1);
		skp->Text(49 * CW * 56 / 60, 22 * CH * 28 / 24, "20", 2);
		skp->Text(49 * CW * 56 / 60, 23 * CH * 28 / 24, "26", 2);

		skp->Text(53 * CW * 56 / 60, 22 * CH * 28 / 24, "21.5", 4);

		skp->Text(57 * CW * 56 / 60, 21 * CH * 28 / 24, "6", 1);
		skp->Text(57 * CW * 56 / 60, 22 * CH * 28 / 24, "23", 2);
		skp->Text(57 * CW * 56 / 60, 23 * CH * 28 / 24, "27", 2);

		Line(skp, CW * 56 * 3 / 20, CH * 28 * 3 / 20, CW * 56 * 19 / 20, CH * 28 * 3 / 20);
		Line(skp, CW * 56 * 3 / 20, CH * 28 * 3 / 20, CW * 56 * 3 / 20, CH * 28 * 17 / 20);
		Line(skp, CW * 56 * 3 / 20, CH * 28 * 17 / 20, CW * 56 * 19 / 20, CH * 28 * 17 / 20);
		Line(skp, CW * 56 * 19 / 20, CH * 28 * 3 / 20, CW * 56 * 19 / 20, CH * 28 * 17 / 20);

		for (unsigned i = 0; i < GC->rtcc->fdolaunchanalog1tab.XVal.size(); i++)
		{
			skp->Pixel((int)(GC->rtcc->fdolaunchanalog1tab.XVal[i] * CW * 56), (int)(GC->rtcc->fdolaunchanalog1tab.YVal[i] * CH * 28), 0x00FFFF);
		}
		break;
	case 65: //FDO Launch Analog No. 2
		if (oapiGetSimTime() > GC->rtcc->fdolaunchanalog2tab.LastUpdateTime + 0.5)
		{
			if (GC->rtcc->pCSM)
			{
				EphemerisData sv = GC->rtcc->StateVectorCalcEphem(GC->rtcc->pCSM);
				GC->rtcc->FDOLaunchAnalog2(sv);
			}
		}
		skp->SetPen(pen2);
		skp->SetFont(font_mocr3);
		GetCharSize(skp, CW, CH);
		Text(skp, 17, 0, "FDO LAUNCH ANALOG NO 2");
		Text(skp, 52, 0, "0041");
		skp->SetTextAlign(oapi::Sketchpad::CENTER, oapi::Sketchpad::BASELINE);
		skp->SetFont(font_mocr_plot);
		skp->Text(2 * CW * 56 / 64, 10 * CH * 28 / 60, "0", 1);
		skp->Text(5 * CW * 56 / 64, 10 * CH * 28 / 60, "0", 1);
		skp->Text(8 * CW * 56 / 64, 10 * CH * 28 / 60, "0", 1);

		skp->Text(2 * CW * 56 / 64, 17 * CH * 28 / 60, "-2", 2);
		skp->Text(5 * CW * 56 / 64, 17 * CH * 28 / 60, "-2", 2);
		skp->Text(8 * CW * 56 / 64, 17 * CH * 28 / 60, "-6", 2);

		skp->Text(2 * CW * 56 / 64, 24 * CH * 28 / 60, "-4", 2);
		skp->Text(5 * CW * 56 / 64, 24 * CH * 28 / 60, "-4", 2);
		skp->Text(8 * CW * 56 / 64, 24 * CH * 28 / 60, "-12", 3);

		skp->Text(5 * CW * 56 / 64, 55 * CH * 28 / 120, "(yEI)", 5);

		skp->Text(2 * CW * 56 / 64, 31 * CH * 28 / 60, "-6", 2);
		skp->Text(5 * CW * 56 / 64, 31 * CH * 28 / 60, "-6", 2);
		skp->Text(8 * CW * 56 / 64, 31 * CH * 28 / 60, "-18", 3);

		skp->Text(2 * CW * 56 / 64, 38 * CH * 28 / 60, "-8", 2);
		skp->Text(5 * CW * 56 / 64, 38 * CH * 28 / 60, "-8", 2);
		skp->Text(8 * CW * 56 / 64, 38 * CH * 28 / 60, "-24", 3);

		skp->Text(2 * CW * 56 / 64, 45 * CH * 28 / 60, "-10", 3);
		skp->Text(5 * CW * 56 / 64, 45 * CH * 28 / 60, "-10", 3);
		skp->Text(8 * CW * 56 / 64, 45 * CH * 28 / 60, "-30", 3);

		skp->Text(2 * CW * 56 / 64, 52 * CH * 28 / 60, "-12", 3);
		skp->Text(5 * CW * 56 / 64, 52 * CH * 28 / 60, "-12", 3);
		skp->Text(8 * CW * 56 / 64, 52 * CH * 28 / 60, "-36", 3);
		skp->SetTextAlign(oapi::Sketchpad::CENTER);

		skp->Text(3 * CW * 56 / 20, 21 * CH * 28 / 24, "6", 1);
		skp->Text(3 * CW * 56 / 20, 22 * CH * 28 / 24, "22.5", 4);
		skp->Text(3 * CW * 56 / 20, 23 * CH * 28 / 24, "25", 2);

		skp->Text(83 * CW * 56 / 340, 21 * CH * 28 / 24, "8", 1);
		skp->Text(83 * CW * 56 / 340, 22 * CH * 28 / 24, "23", 2);
		skp->Text(83 * CW * 56 / 340, 23 * CH * 28 / 24, "26", 2);

		skp->Text(115 * CW * 56 / 340, 21 * CH * 28 / 24, "10", 2);
		skp->Text(115 * CW * 56 / 340, 22 * CH * 28 / 24, "23.5", 4);
		skp->Text(115 * CW * 56 / 340, 23 * CH * 28 / 24, "27", 2);

		skp->Text(147 * CW * 56 / 340, 21 * CH * 28 / 24, "12", 2);
		skp->Text(147 * CW * 56 / 340, 22 * CH * 28 / 24, "24", 2);
		skp->Text(147 * CW * 56 / 340, 23 * CH * 28 / 24, "28", 2);

		skp->Text(163 * CW * 56 / 340, 22 * CH * 28 / 24, "V", 1);

		skp->Text(179 * CW * 56 / 340, 21 * CH * 28 / 24, "14", 2);
		skp->Text(179 * CW * 56 / 340, 22 * CH * 28 / 24, "24.5", 4);
		skp->Text(179 * CW * 56 / 340, 23 * CH * 28 / 24, "29", 2);

		skp->Text(211 * CW * 56 / 340, 21 * CH * 28 / 24, "16", 2);
		skp->Text(211 * CW * 56 / 340, 22 * CH * 28 / 24, "25", 2);
		skp->Text(211 * CW * 56 / 340, 23 * CH * 28 / 24, "30", 2);

		skp->Text(243 * CW * 56 / 340, 21 * CH * 28 / 24, "18", 2);
		skp->Text(243 * CW * 56 / 340, 22 * CH * 28 / 24, "25.5", 4);
		skp->Text(243 * CW * 56 / 340, 23 * CH * 28 / 24, "31", 2);

		skp->Text(275 * CW * 56 / 340, 21 * CH * 28 / 24, "20", 2);
		skp->Text(275 * CW * 56 / 340, 22 * CH * 28 / 24, "26", 2);
		skp->Text(275 * CW * 56 / 340, 23 * CH * 28 / 24, "32", 2);

		skp->Text(307 * CW * 56 / 340, 21 * CH * 28 / 24, "22", 2);
		skp->Text(307 * CW * 56 / 340, 22 * CH * 28 / 24, "26.5", 4);
		skp->Text(307 * CW * 56 / 340, 23 * CH * 28 / 24, "33", 2);

		skp->Text(323 * CW * 56 / 340, 21 * CH * 28 / 24, "23", 2);
		skp->Text(323 * CW * 56 / 340, 23 * CH * 28 / 24, "33.5", 4);

		Line(skp, CW * 56 * 3 / 20, CH * 28 * 3 / 20, CW * 56 * 19 / 20, CH * 28 * 3 / 20);
		Line(skp, CW * 56 * 3 / 20, CH * 28 * 3 / 20, CW * 56 * 3 / 20, CH * 28 * 17 / 20);
		Line(skp, CW * 56 * 3 / 20, CH * 28 * 17 / 20, CW * 56 * 19 / 20, CH * 28 * 17 / 20);
		Line(skp, CW * 56 * 19 / 20, CH * 28 * 3 / 20, CW * 56 * 19 / 20, CH * 28 * 17 / 20);

		for (unsigned i = 0; i < GC->rtcc->fdolaunchanalog2tab.XVal.size(); i++)
		{
			skp->Pixel((int)(GC->rtcc->fdolaunchanalog2tab.XVal[i] * CW * 56), (int)(GC->rtcc->fdolaunchanalog2tab.YVal[i] * CH * 28), 0x00FFFF);
		}
		break;
	case 66: //Return-to-Earth Tradeoff Display
		if (subscreen == 0)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text(W / 2, CH / 2, "RTE TRADE OFF INPUTS", 20);
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			if (G->RTETradeoffMode == 0)
			{
				skp->Text(CW, 2 * H / 14, "Near-Earth", 11);

				sprintf(Buffer, "%s", GC->rtcc->med_f70.Site.c_str());
				skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
				GET_Display(Buffer, GC->rtcc->med_f70.T_V *3600.0, false);
				skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
				GET_Display(Buffer, GC->rtcc->med_f70.T_omin*3600.0, false);
				skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
				GET_Display(Buffer, GC->rtcc->med_f70.T_omax*3600.0, false);
				skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
				skp->SetTextAlign(oapi::Sketchpad::RIGHT);
				if (GC->rtcc->med_f70.EntryProfile == 1)
				{
					skp->Text(W - CW, 10 * H / 14, "Constant G", 10);
				}
				else
				{
					skp->Text(W - CW, 10 * H / 14, "G&N", 3);
				}
			}
			else
			{
				skp->Text(CW, 2 * H / 14, "Remote-Earth", 13);

				sprintf(Buffer, "%d", GC->rtcc->med_f71.Page);
				skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
				sprintf(Buffer, "%s", GC->rtcc->med_f71.Site.c_str());
				skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
				GET_Display(Buffer, GC->rtcc->med_f71.T_V *3600.0, false);
				skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
				GET_Display(Buffer, GC->rtcc->med_f71.T_omin*3600.0, false);
				skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
				GET_Display(Buffer, GC->rtcc->med_f71.T_omax*3600.0, false);
				skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
				skp->SetTextAlign(oapi::Sketchpad::RIGHT);
				if (GC->rtcc->med_f71.EntryProfile == 1)
				{
					skp->Text(W - CW, 10 * H / 14, "Constant G", 10);
				}
				else
				{
					skp->Text(W - CW, 10 * H / 14, "G&N", 3);
				}
			}
		}
		else
		{
			skp->SetPen(pen2);
			skp->SetFont(font_mocr3);
			GetCharSize(skp, CW, CH);
			Text(skp, 16, 0, "RTE TRADE OFF DISPLAY");
			Text(skp, 52, 0, "0364");

			skp->SetTextAlign(oapi::Sketchpad::CENTER, oapi::Sketchpad::BASELINE);

			Line(skp, CW * 56 * 1 / 10, CH * 28 * 3 / 20, CW * 56 * 19 / 20, CH * 28 * 3 / 20);
			Line(skp, CW * 56 * 1 / 10, CH * 28 * 3 / 20, CW * 56 * 1 / 10, CH * 28 * 9 / 10);
			Line(skp, CW * 56 * 1 / 10, CH * 28 * 9 / 10, CW * 56 * 19 / 20, CH * 28 * 9 / 10);
			Line(skp, CW * 56 * 19 / 20, CH * 28 * 3 / 20, CW * 56 * 19 / 20, CH * 28 * 9 / 10);

			unsigned p = subscreen - 1;

			sprintf(Buffer, "%d", GC->rtcc->RTETradeoffTableBuffer[p].XLabels[0]);
			skp->Text(4 * CW * 56 / 40, 23 * CH * 28 / 24, Buffer, strlen(Buffer));
			sprintf(Buffer, "%d", GC->rtcc->RTETradeoffTableBuffer[p].XLabels[1]);
			skp->Text(21 * CW * 56 / 40, 23 * CH * 28 / 24, Buffer, strlen(Buffer));
			sprintf(Buffer, "%d", GC->rtcc->RTETradeoffTableBuffer[p].XLabels[2]);
			skp->Text(38 * CW * 56 / 40, 23 * CH * 28 / 24, Buffer, strlen(Buffer));

			sprintf(Buffer, "%d", GC->rtcc->RTETradeoffTableBuffer[p].YLabels[2]);
			skp->Text(CW * 4, 6 * CH * 28 / 40, Buffer, strlen(Buffer));
			sprintf(Buffer, "%d", GC->rtcc->RTETradeoffTableBuffer[p].YLabels[1]);
			skp->Text(CW * 4, 21 * CH * 28 / 40, Buffer, strlen(Buffer));
			sprintf(Buffer, "%d", GC->rtcc->RTETradeoffTableBuffer[p].YLabels[0]);
			skp->Text(CW * 4, 36 * CH * 28 / 40, Buffer, strlen(Buffer));

			sprintf(Buffer, "Site: %s", GC->rtcc->RTETradeoffTableBuffer[p].Site.c_str());
			skp->Text(12 * CW * 56 / 40, 23 * CH * 28 / 24, Buffer, strlen(Buffer));

			sprintf(Buffer, "%s", GC->rtcc->RTETradeoffTableBuffer[p].XAxisName.c_str());
			skp->Text(29 * CW * 56 / 40, 23 * CH * 28 / 24, Buffer, strlen(Buffer));

			skp->SetFont(font_mocr3_vert);
			sprintf(Buffer, "%s", GC->rtcc->RTETradeoffTableBuffer[p].YAxisName.c_str());
			skp->Text(CW, 14 * CH * 28 / 40, Buffer, strlen(Buffer));
			skp->SetFont(font_mocr3);

			skp->SetPen(pen);

			for (unsigned i = 0; i < GC->rtcc->RTETradeoffTableBuffer[p].curves; i++)
			{
				if (GC->rtcc->RTETradeoffTableBuffer[p].TZDisplay[i] >= 0)
				{
					sprintf(Buffer, "%d", GC->rtcc->RTETradeoffTableBuffer[p].TZDisplay[i]);
					skp->Text((int)(GC->rtcc->RTETradeoffTableBuffer[p].TZxval[i] * CW * 56), (int)(GC->rtcc->RTETradeoffTableBuffer[p].TZyval[i] * CH * 28), Buffer, strlen(Buffer));
				}

				for (unsigned j = 0; j < GC->rtcc->RTETradeoffTableBuffer[p].NumInCurve[i] - 1; j++)
				{
					Line(skp, (int)(GC->rtcc->RTETradeoffTableBuffer[p].xval[i][j] * CW * 56), (int)(GC->rtcc->RTETradeoffTableBuffer[p].yval[i][j] * CH * 28),
						(int)(GC->rtcc->RTETradeoffTableBuffer[p].xval[i][j + 1] * CW * 56), (int)(GC->rtcc->RTETradeoffTableBuffer[p].yval[i][j + 1] * CH * 28));
				}
			}
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		sprintf(Buffer, "%d/%d", subscreen, subscreenmax);
		skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));
		break;
	case 67: //Detailed Maneuver Table 1
	case 68: //Detailed Maneuver Table 2
	{
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		DetailedManeuverTable *tab;
		if (screen == 67)
		{
			Text(skp, 14, 0, "FDO DETAILED MANEUVER TABLE 1");
			Text(skp, 52, 0, "0054");
			tab = &GC->rtcc->DMTBuffer[0];
		}
		else
		{
			Text(skp, 14, 0, "FDO DETAILED MANEUVER TABLE 2");
			Text(skp, 52, 0, "0069");
			tab = &GC->rtcc->DMTBuffer[1];
		}
		Text(skp, 0, 1, "CSTAID");
		Text(skp, 0, 2, "GMTV");
		Text(skp, 0, 3, "GETV");
		Text(skp, 0, 4, "CODE");
		Text(skp, 15, 1, "LSTAID");
		Text(skp, 15, 2, "GMTV");
		Text(skp, 15, 3, "GETV");
		Text(skp, 20, 4, "REF");
		Text(skp, 31, 1, "STAID");
		Text(skp, 30, 2, "GMTV");
		Text(skp, 30, 3, "GETV");
		Text(skp, 30, 4, "GETR");
		Text(skp, 46, 1, "WT");
		Text(skp, 46, 2, "WC");
		Text(skp, 46, 3, "WL");
		Text(skp, 46, 4, "WF");
		Text(skp, 0, 5, "GETI");
		Text(skp, 0, 6, "PETI");
		Text(skp, 0, 7, "DVM");
		Text(skp, 0, 8, "DVRM");
		Text(skp, 0, 9, "DVC");
		Text(skp, 17, 5, "DTB");
		Text(skp, 17, 6, "DTU");
		Text(skp, 29, 5, "DTTO");
		Text(skp, 27, 6, "DVTO");
		Text(skp, 39, 5, "REFSMMAT");
		Text(skp, 48, 5, "DP");
		Text(skp, 48, 6, "DY");
		Text(skp, 13, 7, "VGX");
		Text(skp, 13, 8, "VGY");
		Text(skp, 13, 9, "VGZ");
		Text(skp, 28, 7, "O");
		Text(skp, 28, 8, "I");
		Text(skp, 28, 9, "M");
		Text(skp, 37, 7, "YB");
		Text(skp, 37, 8, "PB");
		Text(skp, 37, 9, "RB");
		Text(skp, 46, 7, "YH");
		Text(skp, 46, 8, "PH");
		Text(skp, 46, 9, "RH");
		Text(skp, 0, 10, "VF");
		Text(skp, 0, 11, "VS");
		Text(skp, 0, 12, "VD");
		TextW(skp, 0, 13, L"\u0394H");
		TextW(skp, 1, 14, L"\u03B8");
		TextW(skp, 1, 15, L"\u03B8");
		TextW(skp, 1, 15, L"\u02D9");
		TextW(skp, 1, 16, L"\u03B4");
		Text(skp, 0, 17, "YD");
		Text(skp, 12, 10, "HBI");
		TextW(skp, 12, 11, L"\u03C6BI");
		TextW(skp, 12, 12, L"\u03BBBI");
		TextW(skp, 12, 13, L"\u03BDBI");
		TextW(skp, 26, 10, L"h\u2090");
		TextW(skp, 26, 11, L"h\u209A");
		Text(skp, 25, 12, "LAN");
		Text(skp, 26, 13, "E");
		Text(skp, 26, 14, "I");
		TextW(skp, 26, 15, L"\u03C9\u209A");
		TextW(skp, 39, 10, L"V\u209A");
		TextW(skp, 39, 11, L"\u03B8\u209A");
		TextW(skp, 39, 12, L"\u03B4\u209A");
		TextW(skp, 39, 13, L"\u03C6LLS");
		TextW(skp, 39, 14, L"\u03BBLLS");
		Text(skp, 39, 15, "RLLS");
		Text(skp, 26, 18, "TARGETS");
		Line(skp, 0, (CH * 37) / 2, CW * 25, (CH * 37) / 2);
		Line(skp, CW * 34, (CH * 37) / 2, CW * 56, (CH * 37) / 2);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text(skp, 14, 1, tab->C_STA_ID);
		Text_GET_HHHMMSS(skp, 14, 2, tab->C_GMTV);
		Text_GET_HHHMMSS(skp, 14, 3, tab->C_GETV);
		Text(skp, 14, 4, tab->CODE);
		Text(skp, 29, 1, tab->L_STA_ID);
		Text_GET_HHHMMSS(skp, 29, 2, tab->L_GMTV);
		Text_GET_HHHMMSS(skp, 29, 3, tab->L_GETV);
		Text(skp, 29, 4, tab->REF);
		if (tab->X_VEH != ' ')
		{
			sprintf(Buffer, "%c", tab->X_VEH); Text(skp, 31, 1, Buffer);
			Text(skp, 44, 1, tab->X_STA_ID);
			Text_GET_HHHMMSS(skp, 44, 2, tab->X_GMTV);
			Text_GET_HHHMMSS(skp, 44, 3, tab->X_GETV);

		}
		Text_GET_HHHMMSS(skp, 44, 4, tab->GETR);
		if (tab->WT > 0.0) Text(skp, 56, 1, "%.1lf", tab->WT);
		if (tab->WC > 0.0) Text(skp, 56, 2, "%.1lf", tab->WC);
		if (tab->WL > 0.0) Text(skp, 56, 3, "%.1lf", tab->WL);
		if (tab->WF > 0.0) Text(skp, 56, 4, "%.1lf", tab->WF);
		Text_GET_HHHMMSSC(skp, 16, 5, tab->GETI);
		Text_GET_HHHMMSSC(skp, 16, 6, tab->PETI);
		Text(skp, 11, 7, "%.1lf", tab->DVM);
		Text(skp, 11, 8, "%.1lf", tab->DVREM);
		Text(skp, 11, 9, "%.1lf", tab->DVC);
		Text_GET_MMSSC(skp, 28, 5, tab->DT_B);
		Text(skp, 26, 6, "%.2lf", tab->DT_U);
		Text(skp, 24, 7, "%+.1lf", tab->VG.x);
		Text(skp, 24, 8, "%+.1lf", tab->VG.y);
		Text(skp, 24, 9, "%+.1lf", tab->VG.z);
		Text(skp, 38, 5, "%.2lf", tab->DT_TO);
		Text(skp, 38, 6, "%.2lf", tab->DV_TO);
		if (tab->isCSMTV)
		{
			Text(skp, 30, 7, "R");
			Text(skp, 30, 8, "P");
			Text(skp, 30, 9, "Y");
		}
		else
		{
			Text(skp, 30, 7, "Y");
			Text(skp, 30, 8, "P");
			Text(skp, 30, 9, "R");
		}
		Text(skp, 36, 7, "%.1lf", tab->IMUAtt.x);
		Text(skp, 36, 8, "%.1lf", tab->IMUAtt.y);
		Text(skp, 36, 9, "%.1lf", tab->IMUAtt.z);
		Text(skp, 46, 6, tab->REFSMMAT_Code);
		if (tab->isCSMTV == false)
		{
			Text(skp, 45, 7, "%.1lf", tab->FDAIAtt.x);
			Text(skp, 45, 8, "%.1lf", tab->FDAIAtt.y);
			Text(skp, 45, 9, "%.1lf", tab->FDAIAtt.z);
		}
		Text(skp, 56, 5, "%+.2lf", tab->DEL_P);
		Text(skp, 56, 6, "%+.2lf", tab->DEL_Y);
		Text(skp, 54, 7, "%.1lf", tab->LVLHAtt.x);
		Text(skp, 54, 8, "%.1lf", tab->LVLHAtt.y);
		Text(skp, 54, 9, "%.1lf", tab->LVLHAtt.z);
		Text(skp, 11, 10, "%.2lf", tab->VF);
		Text(skp, 11, 11, "%.2lf", tab->VS);
		Text(skp, 11, 12, "%.2lf", tab->VD);
		Text(skp, 11, 13, "%.2lf", tab->DH);
		Text(skp, 10, 14, "%.3lf", tab->PHASE);
		Text(skp, 9, 15, "%.2lf", tab->PHASE_DOT);
		Text(skp, 9, 16, "%.3lf", tab->WEDGE_ANG);
		Text(skp, 10, 17, "%.4lf", tab->YD);
		Text(skp, 24, 10, "%.1lf", tab->H_BI);
		Text_Latitude(skp, 23, 11, tab->P_BI);
		Text_Longitude(skp, 23, 12, tab->L_BI);
		Text(skp, 23, 13, "%.2lf", tab->F_BI);
		tab->HA < 0.0 ? Text(skp, 37, 10, "ZZZZZZZZ") : Text(skp, 37, 10, "%.1lf", tab->HA);
		Text(skp, 37, 11, "%.1lf", tab->HP);
		Text_Longitude(skp, 36, 12, tab->L_AN);
		Text(skp, 37, 13, "%.6lf", tab->E);
		Text(skp, 35, 14, "%.2lf", tab->I);
		Text(skp, 35, 15, "%.2lf", tab->WP);
		if (tab->Attitude == RTCC_ATTITUDE_SIVB_IGM)
		{
			Text(skp, 6, 19, "IU IGM");
			Text(skp, 3, 20, "TB6");
		}
		else
		{
			Text(skp, 4, 19, "PGNS");
			Text(skp, 6, 20, "EXT DV");
			Text(skp, 4, 21, "GETI");
			Text(skp, 3, 22, "VX");
			Text(skp, 3, 23, "VY");
			Text(skp, 3, 24, "VZ");
			Text(skp, 14, 20, tab->PGNS_Veh);
			Text(skp, 14, 22, "%.2lf", tab->PGNS_DV.x);
			Text(skp, 14, 23, "%.2lf", tab->PGNS_DV.y);
			Text(skp, 14, 24, "%.2lf", tab->PGNS_DV.z);
			Text(skp, 23, 19, "AGS");
			Text(skp, 26, 20, "EXT DV");
			Text(skp, 24, 21, "GETI");
			Text(skp, 23, 22, "VX");
			Text(skp, 23, 23, "VY");
			Text(skp, 23, 24, "VZ");
			Text(skp, 34, 20, tab->AGS_Veh);
			Text_GET_HHHMMSSCS(skp, 37, 21, tab->AGS_GETI);
			Text(skp, 34, 22, "%.2lf", tab->AGS_DV.x);
			Text(skp, 34, 23, "%.2lf", tab->AGS_DV.y);
			Text(skp, 34, 24, "%.2lf", tab->AGS_DV.z);
		}
		Text_GET_HHHMMSSCS(skp, 17, 21, tab->PGNS_GETI);
		Text(skp, 30, 26, tab->error);
	}
	break;
	case 69:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "SPQ Initialization (MED K06)", 28);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		Text_Double(skp, CW, 2 * H / 14, "%.1f NM", GC->rtcc->GZGENCSN.SPQDeltaH / 1852.0);
		Text_Double(skp, CW, 4 * H / 14, "%.2f°", GC->rtcc->GZGENCSN.SPQElevationAngle*DEG);
		Text_Double(skp, CW, 6 * H / 14, "%.2f°", GC->rtcc->GZGENCSN.SPQTerminalPhaseAngle*DEG);
		Text_Double(skp, CW, 8 * H / 14, "%.1f NM", GC->rtcc->GZGENCSN.SPQMinimumPerifocus / 1852.0);
		if (GC->rtcc->GZGENCSN.TPIDefinition == 1)
		{
			skp->Text(CW, 10 * H / 14, "Chaser Day/Night Time", 21);
			sprintf_s(Buffer, "%.1f min", GC->rtcc->GZGENCSN.TPIDefinitionValue / 60.0);
		}
		else if (GC->rtcc->GZGENCSN.TPIDefinition == 2)
		{
			skp->Text(CW, 10 * H / 14, "Chaser Longitude", 18);
			sprintf_s(Buffer, "%.1f°", GC->rtcc->GZGENCSN.TPIDefinitionValue*DEG);
		}
		else if (GC->rtcc->GZGENCSN.TPIDefinition == 3)
		{
			skp->Text(CW, 10 * H / 14, "On time", 7);
			GET_Display(Buffer, GC->rtcc->GZGENCSN.TPIDefinitionValue);
		}
		else if (GC->rtcc->GZGENCSN.TPIDefinition == 4)
		{
			skp->Text(CW, 10 * H / 14, "Target Day/Night Time", 21);
			sprintf_s(Buffer, "%.1f min", GC->rtcc->GZGENCSN.TPIDefinitionValue / 60.0);
		}
		else if (GC->rtcc->GZGENCSN.TPIDefinition == 5)
		{
			skp->Text(CW, 10 * H / 14, "Target Longitude", 16);
			sprintf_s(Buffer, "%.1f°", GC->rtcc->GZGENCSN.TPIDefinitionValue*DEG);
		}
		skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (GC->rtcc->med_k01.I_CDH == 1)
		{
			skp->Text(W - CW, 2 * H / 14, "CDH at upcoming apsis (AEG):", 28);
			sprintf_s(Buffer, "%d", GC->rtcc->med_k01.CDH_Apsis);
		}
		else if (GC->rtcc->med_k01.I_CDH == 2)
		{
			skp->Text(W - CW, 2 * H / 14, "CDH on time:", 12);
			GET_Display3(Buffer, GC->rtcc->med_k01.CDH_Time);
		}
		else if (GC->rtcc->med_k01.I_CDH == 3)
		{
			skp->Text(W - CW, 2 * H / 14, "Angle from CSI to CDH:", 22);
			sprintf_s(Buffer, "%.1lf°", GC->rtcc->med_k01.CDH_Angle*DEG);
		}
		else if (GC->rtcc->med_k01.I_CDH == 4)
		{
			skp->Text(W - CW, 2 * H / 14, "CDH at upcoming apsis (Kepler):", 31);
			sprintf_s(Buffer, "%d", GC->rtcc->med_k01.CDH_Apsis);
		}
		else
		{
			skp->Text(W - CW, 2 * H / 14, "CDH at N half-revs after CSI:", 29);
			sprintf_s(Buffer, "%d", GC->rtcc->med_k01.CDH_Apsis);
		}
		skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
		break;
	case 70:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "DKI Initialization (MED K05)", 28);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (GC->rtcc->med_k00.I4)
		{
			Text_Double(skp, CW, 2 * H / 14, "NCC DH: %.1f NM", GC->rtcc->GZGENCSN.DKIDeltaH_NCC / 1852.0);
		}
		Text_Double(skp, CW, 4 * H / 14, "NSR DH: %.1f NM", GC->rtcc->GZGENCSN.DKIDeltaH_NSR / 1852.0);
		Text_Double(skp, CW, 6 * H / 14, "%.2f°", GC->rtcc->GZGENCSN.DKIElevationAngle*DEG);
		Text_Double(skp, CW, 8 * H / 14, "%.2f°", GC->rtcc->GZGENCSN.DKITerminalPhaseAngle*DEG);
		Text_Double(skp, CW, 10 * H / 14, "%.1f NM", GC->rtcc->GZGENCSN.DKIMinPerigee / 1852.0);
		if (GC->rtcc->med_k00.I4)
		{
			Text_Double(skp, CW, 12 * H / 14, "%.1f min", GC->rtcc->med_k00.dt_NCC_NSR / 60.0);
		}
		break;
	case 71:
		if (subscreen == 0)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text(W / 2, CH / 2, "TI Single Solution (K31)", 24);
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			x = 1;  y = 3; dx = 7;
			Text(skp, x, marker + y, "*");
			x++;
			Text(skp, x, y, "TAB:");
			if (GC->rtcc->med_k31.TableIndicator == 1) Text(skp, x + dx, y, "Multiple Solution");
			else Text(skp, x + dx, y, "Corrective Combination");
			y++;
			Text(skp, x, y, "SOL:");
			Text(skp, x + dx, y, "%d", GC->rtcc->med_k31.PlanNumber); y++;
			Text(skp, x, y, "QUAD:");
			if (GC->rtcc->med_k31.UllageQuads) Text(skp, x + dx, y, "4 Quads");
			else Text(skp, x + dx, y, "2 Quads");
			y++;
			Text(skp, x, y, "LOS:");
			if (GC->rtcc->med_k31.LOSMode == 1) Text(skp, x + dx, y, "Target");
			else Text(skp, x + dx, y, "Horizon");
			y++;
			Text(skp, x, y, "PIT:");
			Text(skp, x + dx, y, "%.1lf deg", GC->rtcc->med_k31.DeltaPitch); y++;
			Text(skp, x, y, "DT:");
			Text(skp, x + dx, y, "%.1lf sec", GC->rtcc->med_k31.TimeStep);
		}
		else
		{
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			SetMOCRFont(skp, 2, false);
			GetCharSize(skp, CW, CH);
			SetMOCRDisplayCentered(2);
			Text(skp, 18, 0, "TWO IMPULSE SINGLE SOLUTION");
			Text(skp, 60, 0, "0065");
			Text(skp, 0, 2, "LM STA ID");
			Text(skp, 0, 3, "LM GETTHS");
			Text(skp, 2, 4, "MAN VEH");
			Text(skp, 2, 5, "MODE");
			Text(skp, 4, 6, "ID");
			Text(skp, 23, 5, "DTR");
			Text(skp, 23, 6, "WT");
			Text(skp, 44, 2, "CSM STA ID");
			Text(skp, 44, 3, "CSM GETTHS");
			Text(skp, 49, 4, "PHASE");
			Text(skp, 49, 5, "DEL H");
			Text(skp, 45, 6, "DEL PITCH");
			for (int i = 0; i < 2; i++)
			{
				Text(skp, 0, 8 + i * 11, "GET");
				Text(skp, 0, 9 + i * 11, "DV");
				Text(skp, 0, 10 + i * 11, "YAW");
				Text(skp, 0, 11 + i * 11, "PITCH");
				Text(skp, 14, 9 + i * 11, "VX");
				Text(skp, 14, 10 + i * 11, "VY");
				Text(skp, 14, 11 + i * 11, "VZ");
				Text(skp, 27, 8 + i * 11, "EHOR");
				Text(skp, 28, 10 + i * 11, "YAW");
				Text(skp, 26, 11 + i * 11, "PITCH");
				Text(skp, 41, 9 + i * 11, "XD");
				Text(skp, 41, 10 + i * 11, "YD");
				Text(skp, 41, 11 + i * 11, "ZD");
				Text(skp, 49, 8 + i * 11, "GMT");
				Text(skp, 53, 9 + i * 11, "DT");
				Text(skp, 53, 10 + i * 11, "DT");
				Text(skp, 53, 11 + i * 11, "DT");
				Text(skp, 10, 12 + i * 11, "MIN UNTIL");
				Text(skp, 34, 12 + i * 11, "HA");
				Text(skp, 53, 12 + i * 11, "HP");
				Text(skp, 0, 14 + i * 11, "   GET     TGT AZ TGT EL  RANGE   RDOT     X        Z       Y");
			}
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			SetMOCRFont(skp, 2, true);
			Text(skp, 19, 2, GC->rtcc->TwoImpSingleDispBuffer.LMSTAID);
			Text_GET_HHHMMSS(skp, 19, 3, GC->rtcc->TwoImpSingleDispBuffer.LM_GETTH);
			Text(skp, 64, 2, GC->rtcc->TwoImpSingleDispBuffer.CSMSTAID);
			Text_GET_HHHMMSS(skp, 64, 3, GC->rtcc->TwoImpSingleDispBuffer.CSM_GETTH);
			Text(skp, 16, 4, GC->rtcc->TwoImpSingleDispBuffer.MAN_VEH);
			Text(skp, 16, 5, GC->rtcc->TwoImpSingleDispBuffer.PointingMode);
			Text(skp, 14, 6, GC->rtcc->TwoImpSingleDispBuffer.TwoImpulseTableIndicator);
			Text(skp, 16, 6, "%d", GC->rtcc->TwoImpSingleDispBuffer.ID);
			Text_GET_HHHMMSS(skp, 36, 5, GC->rtcc->TwoImpSingleDispBuffer.DTR);
			Text(skp, 36, 6, "%.4lf", GC->rtcc->TwoImpSingleDispBuffer.WT);
			Text(skp, 64, 4, "%.4lf", GC->rtcc->TwoImpSingleDispBuffer.PHASE);
			Text(skp, 64, 5, "%.3lf", GC->rtcc->TwoImpSingleDispBuffer.DELH);
			Text(skp, 64, 6, "%.4lf", GC->rtcc->TwoImpSingleDispBuffer.DELPITCH);
			int k;
			for (int i = 0; i < 2; i++)
			{
				Text_GET_HHHMMSSC(skp, 15, 8 + i * 11, GC->rtcc->TwoImpSingleDispBuffer.man[i].GET);
				Text(skp, 38, 8 + i * 11, "%.2lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].E_HOR);
				Text_GET_HHHMMSSC(skp, 64, 8 + i * 11, GC->rtcc->TwoImpSingleDispBuffer.man[i].GMT);
				Text(skp, 13, 9 + i * 11, "%.2lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].DV);
				Text(skp, 13, 10 + i * 11, "%.2lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].Yaw);
				Text(skp, 13, 11 + i * 11, "%.2lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].Pitch);
				Text(skp, 24, 9 + i * 11, "%.1lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].DV_LVLH.x);
				Text(skp, 24, 10 + i * 11, "%.1lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].DV_LVLH.y);
				Text(skp, 24, 11 + i * 11, "%.1lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].DV_LVLH.z);
				Text(skp, 39, 10 + i * 11, "%.2lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].Yaw_LOS);
				Text(skp, 39, 11 + i * 11, "%.2lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].Pitch_LOS);
				Text(skp, 51, 9 + i * 11, "%.1lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].DV_LOS.x);
				Text(skp, 51, 10 + i * 11, "%.1lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].DV_LOS.y);
				Text(skp, 51, 11 + i * 11, "%.1lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].DV_LOS.z);
				Text_GET_MMSSC(skp, 63, 9 + i * 11, GC->rtcc->TwoImpSingleDispBuffer.man[i].BT_LOS.x);
				Text_GET_MMSSC(skp, 63, 10 + i * 11, GC->rtcc->TwoImpSingleDispBuffer.man[i].BT_LOS.y);
				Text_GET_MMSSC(skp, 63, 11 + i * 11, GC->rtcc->TwoImpSingleDispBuffer.man[i].BT_LOS.z);
				Text(skp, 64, 9 + i * 11, std::string(1, GC->rtcc->TwoImpSingleDispBuffer.man[i].BT_LOS_DIR[0]));
				Text(skp, 64, 10 + i * 11, std::string(1, GC->rtcc->TwoImpSingleDispBuffer.man[i].BT_LOS_DIR[1]));
				Text(skp, 64, 11 + i * 11, std::string(1, GC->rtcc->TwoImpSingleDispBuffer.man[i].BT_LOS_DIR[2]));
				Text(skp, 9, 12 + i * 11, "%.0lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].MinEnvironChange);
				Text(skp, 28, 12 + i * 11, GC->rtcc->TwoImpSingleDispBuffer.man[i].Condition);
				Text(skp, 45, 12 + i * 11, "%.3lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].HA);
				Text(skp, 64, 12 + i * 11, "%.3lf", GC->rtcc->TwoImpSingleDispBuffer.man[i].HP);
				for (int j = 0; j < (i == 0 ? 3 : 4); j++)
				{
					k = i * 3 + j;
					Text_GET_HHHMMSS(skp, 9, 15 + i * 11 + j, GC->rtcc->TwoImpSingleDispBuffer.app[k].GET);
					Text(skp, 16, 15 + i * 11 + j, "%.1lf", GC->rtcc->TwoImpSingleDispBuffer.app[k].TGT_AZ);
					Text(skp, 17, 15 + i * 11 + j, std::string(1, GC->rtcc->TwoImpSingleDispBuffer.app[k].TGT_AZ_DIR));
					Text(skp, 23, 15 + i * 11 + j, "%.1lf", GC->rtcc->TwoImpSingleDispBuffer.app[k].TGT_EL);
					Text(skp, 24, 15 + i * 11 + j, std::string(1, GC->rtcc->TwoImpSingleDispBuffer.app[k].TGT_EL_DIR));
					Text(skp, 31, 15 + i * 11 + j, "%.1lf", GC->rtcc->TwoImpSingleDispBuffer.app[k].RANGE);
					Text(skp, 38, 15 + i * 11 + j, "%.1lf", GC->rtcc->TwoImpSingleDispBuffer.app[k].RDOT);
					Text(skp, 46, 15 + i * 11 + j, "%.2lf", GC->rtcc->TwoImpSingleDispBuffer.app[k].OFF.x);
					Text(skp, 47, 15 + i * 11 + j, std::string(1, GC->rtcc->TwoImpSingleDispBuffer.app[k].X));
					Text(skp, 55, 15 + i * 11 + j, "%.2lf", GC->rtcc->TwoImpSingleDispBuffer.app[k].OFF.z);
					Text(skp, 56, 15 + i * 11 + j, std::string(1, GC->rtcc->TwoImpSingleDispBuffer.app[k].Z));
					Text(skp, 63, 15 + i * 11 + j, "%.2lf", GC->rtcc->TwoImpSingleDispBuffer.app[k].OFF.y);
					Text(skp, 64, 15 + i * 11 + j, std::string(1, GC->rtcc->TwoImpSingleDispBuffer.app[k].Y));
				}
			}
			Text(skp, 31, 50, GC->rtcc->TwoImpSingleDispBuffer.ErrorMessage);
		}
		break;
	case 75:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 2, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(2);
		Text(skp, 20, 0, "ON LINE MONITOR");
		Text(skp, 58, 0, "1629");
		{
			unsigned line = 0;

			for (unsigned i = 0; i < GC->rtcc->RTCCONLINEMON.data.size(); i++)
			{
				for (unsigned j = 0; j < GC->rtcc->RTCCONLINEMON.data[i].message.size(); j++)
				{
					sprintf(Buffer, GC->rtcc->RTCCONLINEMON.data[i].message[j].c_str());
					Text(skp, 1, 4 + line, Buffer);
					line++;
					if (line >= 28)
					{
						break;
					}
				}
				line++;
				if (line >= 28)
				{
					break;
				}
			}
		}
		break;
	case 76:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		if (GC->rtcc->med_m78.Type)
		{
			skp->Text(W / 2, CH / 2, "LOI Transfer (MED M78)", 22);
		}
		else
		{
			skp->Text(W / 2, CH / 2, "MCC Transfer (MED M78)", 22);
		}
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (GC->rtcc->med_m78.Table == 1)
		{
			skp->Text(CW, 2 * H / 14, "CSM", 3);
		}
		else
		{
			skp->Text(CW, 2 * H / 14, "LEM", 3);
		}
		if (GC->MissionPlanningActive)
		{
			if (GC->rtcc->med_m78.ReplaceCode == 0)
			{
				skp->Text(CW, 4 * H / 14, "Don't replace", 13);
			}
			else
			{
				Text_Int(skp, CW, 4 * H / 14, "%d", GC->rtcc->med_m78.ReplaceCode);
			}
		}
		Text_Int(skp, CW, 6 * H / 14, "%d", GC->rtcc->med_m78.ManeuverNumber);
		ThrusterName(Buffer, GC->rtcc->med_m78.ManData.Thruster);
		skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		if (GC->MissionPlanningActive)
		{
			MPTAttitudeName(Buffer, GC->rtcc->med_m78.ManData.Attitude);
			skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		}
		PrintUllage(Buffer, GC->rtcc->med_m78.ManData.Thruster, GC->rtcc->med_m78.ManData.UllageQuads, GC->rtcc->med_m78.ManData.UllageDT);
		skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (GC->rtcc->med_m78.ManData.Iteration)
		{
			skp->Text(W - CW, 2 * H / 14, "Iterate", 7);
		}
		else
		{
			skp->Text(W - CW, 2 * H / 14, "Don't iterate", 13);
		}
		if (GC->rtcc->med_m78.ManData.Thruster == RTCC_ENGINETYPE_LMDPS)
		{
			Text_Double(skp, W - CW, 4 * H / 14, "%.1lf s", GC->rtcc->med_m78.ManData.TenPercentDT);
			Text_Double(skp, W - CW, 6 * H / 14, "%.3lf", GC->rtcc->med_m78.ManData.DPSThrustFactor);
		}
		if (GC->rtcc->med_m78.ManData.TimeFlag)
		{
			skp->Text(W - CW, 8 * H / 14, "Impulsive TIG", 13);
		}
		else
		{
			skp->Text(W - CW, 8 * H / 14, "Optimum TIG", 11);
		}
		if (GC->MissionPlanningActive == false)
		{
			GET_Display2(Buffer, G->P30TIG);
			skp->Text(W - CW, H - CH * 5, Buffer, strlen(Buffer));

			skp->Text(W - CW * 10, H - CH * 4, "DVX", 3);
			skp->Text(W - CW * 10, H - CH * 3, "DVY", 3);
			skp->Text(W - CW * 10, H - CH * 2, "DVZ", 3);

			AGC_Display(Buffer, G->dV_LVLH.x / 0.3048);
			skp->Text(W - CW, H - CH * 4, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.y / 0.3048);
			skp->Text(W - CW, H - CH * 3, Buffer, strlen(Buffer));
			AGC_Display(Buffer, G->dV_LVLH.z / 0.3048);
			skp->Text(W - CW, H - CH * 2, Buffer, strlen(Buffer));
		}
		break;
	case 77:
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 15, 0, "SKELETON FLIGHT PLAN TABLE");
		Text(skp, 52, 0, "1597");
		{
			int i = GC->rtcc->PZSFPTAB.DisplayBlockNum - 1;
			x = 3; y = 1;
			Text(skp, x, y, "BLOCK NUMBER"); y++;
			Text(skp, x, y, "01-GMT TIME FLAG"); y++;
			Text(skp, x, y, "02-MODE", 7); y++;
			Text(skp, x, y, "03-GMT OF TLI PERICYN."); y++;
			Text(skp, x, y, "04-LAT. OF TLI PERICYN."); y++;
			Text(skp, x, y, "05-LONG. OF TLI PERICYN."); y++;
			Text(skp, x, y, "06-HEIGHT OF TLI PERICYN."); y++;
			Text(skp, x, y, "07-GMT OF LOI PERICYN."); y++;
			Text(skp, x, y, "08-LAT. OF LOI PERICYN."); y++;
			Text(skp, x, y, "09-LONG. OF LOI PERICYN."); y++;
			Text(skp, x, y, "10-HEIGHT OF LOI PERICYN."); y++;
			Text(skp, x, y, "11-GET OF TLI IGNITION"); y++;
			Text(skp, x, y, "12-GMT OF NODE"); y++;
			Text(skp, x, y, "13-LAT. OF NODE"); y++;
			Text(skp, x, y, "14-LONG. OF NODE"); y++;
			Text(skp, x, y, "15-HEIGHT OF NODE"); y++;
			Text(skp, x, y, "16-DELTA AZIMUTH OF LOI"); y++;
			Text(skp, x, y, "17-FLIGHT PATH ANGLE AT LOI"); y++;
			Text(skp, x, y, "18-DELTA TIME OF LPO"); y++;
			Text(skp, x, y, "19-DELTA TIME OF LLS"); y++;
			Text(skp, x, y, "20-AZIMUTH OF LLS"); y++;
			Text(skp, x, y, "21-LAT. OF LLS"); y++;
			Text(skp, x, y, "22-LONG. OF LLS"); y++;
			Text(skp, x, y, "23-RADIUS OF LLS"); y++;
			Text(skp, x, y, "24-DELTA AZIMUTH OF TEI"); y++;
			Text(skp, x, y, "25-DELTA V OF TEI"); y++;
			Text(skp, x, y, "26-DELTA TIME OF TEI"); y++;
			//Text(skp, x, y, "27-INCLINATION OF FREE RET.");y++;
			//Text(skp, x, y, "28-DELTA T OF UPDATED NOM.");
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			SetMOCRFont(skp, 3, true);
			x = 55; y = 1;
			if (i == 0)
			{
				Text(skp, x, y, "1 (PREFLIGHT)");
			}
			else
			{
				Text(skp, x, y, "2 (NOMINAL)");
			}
			y++;
			Text_GET_HHHMMSSCS(skp, x, y, GC->rtcc->PZSFPTAB.blocks[i].GMTTimeFlag); y++;
			Text(skp, x, y, "%d", GC->rtcc->PZSFPTAB.blocks[i].mode); y++;
			Text_GET_HHHMMSSCS(skp, x, y, GC->rtcc->PZSFPTAB.blocks[i].GMT_pc1); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].lat_pc1*DEG); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].lng_pc1*DEG); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].h_pc1 / 1852.0); y++;
			Text_GET_HHHMMSSCS(skp, x, y, GC->rtcc->PZSFPTAB.blocks[i].GMT_pc2); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].lat_pc2*DEG); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].lng_pc2*DEG); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].h_pc2 / 1852.0); y++;
			Text_GET_HHHMMSSCS(skp, x, y, GC->rtcc->PZSFPTAB.blocks[i].GET_TLI); y++;
			Text_GET_HHHMMSSCS(skp, x, y, GC->rtcc->PZSFPTAB.blocks[i].GMT_nd); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].lat_nd*DEG); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].lng_nd*DEG); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].h_nd / 1852.0); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].dpsi_loi*DEG); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].gamma_loi*DEG); y++;
			Text_GET_HHHMMSSCS(skp, x, y, GC->rtcc->PZSFPTAB.blocks[i].T_lo); y++;
			Text_GET_HHHMMSSCS(skp, x, y, GC->rtcc->PZSFPTAB.blocks[i].dt_lls); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].psi_lls*DEG); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].lat_lls*DEG); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].lng_lls*DEG); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].rad_lls / 1852.0); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].dpsi_tei*DEG); y++;
			Text(skp, x, y, "%.3lf", GC->rtcc->PZSFPTAB.blocks[i].dv_tei / 0.3048); y++;
			Text_GET_HHHMMSSCS(skp, x, y, GC->rtcc->PZSFPTAB.blocks[i].T_te);
		}
		break;
	case 78:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 18, 0, "MIDCOURSE TRADEOFF");
		Text(skp, 52, 0, "0079");
		x = 1; y = 1; dx = 8;
		Text(skp, x, y, "COLUMN"); x = 12;
		Text(skp, x, y, "1"); x += dx;
		Text(skp, x, y, "2"); x += dx;
		Text(skp, x, y, "3"); x += dx;
		Text(skp, x, y, "4"); x += dx;
		Text(skp, x, y, "5"); x += dx;
		Text(skp, x, y, "6");
		x = 1;  y = 3;
		Text(skp, x, y, "MODE"); y++;
		Text(skp, x, y, "RETURN", 6); y++;
		Text(skp, x, y, "AZ MIN", 6); y++;
		Text(skp, x, y, "AZ MAX", 6); y++;
		Text(skp, x, y, "WEIGHT", 6); y++;
		Text(skp, x, y, "GETMCC", 6); y++;
		Text(skp, x, y, "DV MCC", 6); y++;
		Text(skp, x, y, "YAW MCC", 7); y++;
		Text(skp, x, y, "H PYCN", 6); y++;
		Text(skp, x, y, "GET LOI", 7); y++;
		Text(skp, x, y, "DV LOI", 6); y++;
		Text(skp, x, y, "AZ ACT", 6); y++;
		Text(skp, x, y, "I FR", 4); y++;
		Text(skp, x, y, "I PR", 4); y++;
		Text(skp, x, y, "V EI", 4); y++;
		Text(skp, x, y, "G EI", 4); y++;
		Text(skp, x, y, "GETTEI", 6); y++;
		Text(skp, x, y, "DV TEI", 6); y++;
		Text(skp, x, y, "DV REM", 6); y++;
		Text(skp, x, y, "GET LC", 6); y++;
		Text(skp, x, y, "LAT IP", 6); y++;
		Text(skp, x, y, "LNG IP", 6); y++;
		Text(skp, x, y, "DV PC", 5);
		x = 17; y = (CH * 5) / 2; dx = 2 * 8;
		Line(skp, 0, y, CW * 56, y);
		Line(skp, (CW * x) / 2, y, (CW * x) / 2, CH * 28); x += dx;
		Line(skp, (CW * x) / 2, y, (CW * x) / 2, CH * 28); x += dx;
		Line(skp, (CW * x) / 2, y, (CW * x) / 2, CH * 28); x += dx;
		Line(skp, (CW * x) / 2, y, (CW * x) / 2, CH * 28); x += dx;
		Line(skp, (CW * x) / 2, y, (CW * x) / 2, CH * 28); x += dx;
		Line(skp, (CW * x) / 2, y, (CW * x) / 2, CH * 28); x += dx;
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		x = 16; dx = 8;
		for (int i = 0; i < 6; i++)
		{
			if (GC->rtcc->PZMCCDIS.data[i].Mode == 0) continue;
			y = 3;
			Text(skp, x + dx * i, y, "%d", GC->rtcc->PZMCCDIS.data[i].Mode); y++;
			if (GC->rtcc->PZMCCDIS.data[i].Return == 0)
			{
				sprintf_s(Buffer, "N/A");
			}
			else if (GC->rtcc->PZMCCDIS.data[i].Return == 1)
			{
				sprintf_s(Buffer, "NONFREE");
			}
			else
			{
				sprintf_s(Buffer, "FREE");
			}
			Text(skp, x + dx * i, y, Buffer); y++;
			Text(skp, x + dx * i, y, "%.3lf", GC->rtcc->PZMCCDIS.data[i].AZ_min*DEG); y++;
			Text(skp, x + dx * i, y, "%.3lf", GC->rtcc->PZMCCDIS.data[i].AZ_max*DEG); y++;
			Text(skp, x + dx * i, y, "%.0lf", (GC->rtcc->PZMCCDIS.data[i].CSMWT + GC->rtcc->PZMCCDIS.data[i].LMWT) / 0.45359237); y++;
			Text_GET_HHHMM(skp, x + dx * i, y, GC->rtcc->PZMCCDIS.data[i].GET_MCC); y++;
			Text(skp, x + dx * i, y, "%.1lf", length(GC->rtcc->PZMCCDIS.data[i].DV_MCC) / 0.3048); y++;
			Text(skp, x + dx * i, y, "%.3lf", GC->rtcc->PZMCCDIS.data[i].YAW_MCC*DEG); y++;
			Text(skp, x + dx * i, y, "%.3lf", GC->rtcc->PZMCCDIS.data[i].h_PC / 1852.0); y++;
			Text_GET_HHHMM(skp, x + dx * i, y, GC->rtcc->PZMCCDIS.data[i].GET_LOI); y++;
			Text(skp, x + dx * i, y, "%.1lf", length(GC->rtcc->PZMCCDIS.data[i].DV_LOI) / 0.3048); y++;
			Text(skp, x + dx * i, y, "%.3lf", GC->rtcc->PZMCCDIS.data[i].AZ_act*DEG); y++;
			Text(skp, x + dx * i, y, "%.3lf", GC->rtcc->PZMCCDIS.data[i].incl_fr*DEG); y++;
			Text(skp, x + dx * i, y, "%.3lf", GC->rtcc->PZMCCDIS.data[i].incl_pr*DEG); y++;
			Text(skp, x + dx * i, y, "%.1lf", GC->rtcc->PZMCCDIS.data[i].v_EI / 0.3048); y++;
			Text(skp, x + dx * i, y, "%.3lf", GC->rtcc->PZMCCDIS.data[i].gamma_EI*DEG); y++;
			Text_GET_HHHMM(skp, x + dx * i, y, GC->rtcc->PZMCCDIS.data[i].GET_TEI); y++;
			Text(skp, x + dx * i, y, "%.1lf", length(GC->rtcc->PZMCCDIS.data[i].DV_TEI) / 0.3048); y++;
			Text(skp, x + dx * i, y, "%.1lf", GC->rtcc->PZMCCDIS.data[i].DV_REM / 0.3048); y++;
			Text_GET_HHHMM(skp, x + dx * i, y, GC->rtcc->PZMCCDIS.data[i].GET_LC); y++;
			Text(skp, x + dx * i, y, "%.3lf", GC->rtcc->PZMCCDIS.data[i].lat_IP*DEG); y++;
			Text(skp, x + dx * i, y, "%.3lf", GC->rtcc->PZMCCDIS.data[i].lng_IP*DEG); y++;
			Text(skp, x + dx * i, y, "%.1lf", length(GC->rtcc->PZMCCDIS.data[i].DV_LOPC / 0.3048)); y++;
		}
		break;
	case 79:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "TLI PLANNING DISPLAY", 20);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, (marker + 3) * H / 22, "*", 1);
		skp->Text(CW * 2, 3 * H / 22, "IU:", 3);
		if (G->iuvessel == NULL)
		{
			sprintf_s(Buffer, 127, "No IU!");
		}
		else
		{
			sprintf_s(Buffer, 127, G->iuvessel->GetName());
		}
		skp->Text(CW * 10, 3 * H / 22, Buffer, strlen(Buffer));

		if (GC->MissionPlanningActive)
		{
			skp->Text(CW * 2, 4 * H / 22, "MPT:", 4);
			if (GC->rtcc->PZTLIPLN.mpt == RTCC_MPT_CSM)
			{
				sprintf(Buffer, "CSM");
			}
			else
			{
				sprintf(Buffer, "LEM");
			}
			skp->Text(CW * 10, 4 * H / 22, Buffer, strlen(Buffer));
			skp->Text(CW * 2, 5 * H / 22, "Vec:", 4);
			skp->Text(CW * 10, 5 * H / 22, GC->rtcc->PZTLIPLN.VectorType.c_str(), GC->rtcc->PZTLIPLN.VectorType.size());
		}
		skp->Text(CW * 2, 6 * H / 22, "Opp:", 4);
		Text_Int(skp, CW * 10, 6 * H / 22, "%d", GC->rtcc->PZTLIPLN.Opportunity);
		skp->Text(CW * 2, 7 * H / 22, "Mode:", 5);
		switch (GC->rtcc->PZTLIPLN.Mode)
		{
		case 2:
			skp->Text(CW * 10, 7 * H / 22, "Free Return", 11);
			break;
		case 3:
			skp->Text(CW * 10, 7 * H / 22, "Hybrid Ellipse", 14);
			break;
		case 4:
			skp->Text(CW * 10, 7 * H / 22, "Specified Apogee", 16);
			break;
		case 5:
			skp->Text(CW * 10, 7 * H / 22, "Non-Free Return", 15);
			break;
		case 6:
			skp->Text(CW * 10, 7 * H / 22, "External DV", 11);
			break;
		default:
			skp->Text(CW * 10, 7 * H / 22, "Hypersurface", 12);
			break;
		}
		if (GC->rtcc->PZTLIPLN.Mode != 1)
		{
			skp->Text(CW * 2, 8 * H / 22, "TIG:", 4);
			GET_Display(Buffer, GC->rtcc->PZTLIPLN.GET_TLI, false);
			skp->Text(CW * 10, 8 * H / 22, Buffer, strlen(Buffer));
		}

		if (GC->rtcc->PZTLIPLN.Mode == 3)
		{
			skp->Text(CW * 2, 9 * H / 22, "DV:", 3);
			sprintf_s(Buffer, "%.0lf ft/s", GC->rtcc->PZTLIPLN.dv_available);
			skp->Text(CW * 10, 9 * H / 22, Buffer, strlen(Buffer));
		}
		else if (GC->rtcc->PZTLIPLN.Mode == 4)
		{
			skp->Text(CW * 2, 9 * H / 22, "APO:", 4);

			sprintf_s(Buffer, "%.0lf NM", GC->rtcc->PZTLIPLN.h_ap);
			skp->Text(CW * 10, 9 * H / 22, Buffer, strlen(Buffer));
		}
		else if (GC->rtcc->PZTLIPLN.Mode == 6)
		{
			skp->Text(CW * 2, 9 * H / 22, "DV:", 4);
			AGC_Display(Buffer, GC->rtcc->PZTLIPLN.dV_LVLH.x / 0.3048);
			skp->Text(CW * 10, 9 * H / 22, Buffer, strlen(Buffer));
			AGC_Display(Buffer, GC->rtcc->PZTLIPLN.dV_LVLH.y / 0.3048);
			skp->Text(CW * 10, 10 * H / 22, Buffer, strlen(Buffer));
			AGC_Display(Buffer, GC->rtcc->PZTLIPLN.dV_LVLH.z / 0.3048);
			skp->Text(CW * 10, 11 * H / 22, Buffer, strlen(Buffer));
		}
		else if (GC->rtcc->PZTLIPLN.Mode == 2 || GC->rtcc->PZTLIPLN.Mode == 5)
		{
			skp->Text(CW * 2, 9 * H / 22, "Window:", 7);

			if (GC->rtcc->PZTLIPLN.IsPacficWindow)
			{
				skp->Text(CW * 10, 9 * H / 22, "Pacific", 14);
			}
			else
			{
				skp->Text(CW * 10, 9 * H / 22, "Atlantic", 15);
			}

			skp->Text(CW * 2, 10 * H / 22, "SFP:", 4);
			if (GC->rtcc->PZMCCPLN.SFPBlockNum == 1)
			{
				sprintf(Buffer, "1 (Preflight)");
			}
			else
			{
				sprintf(Buffer, "2 (Nominal)");
			}
			skp->Text(CW * 10, 10 * H / 22, Buffer, strlen(Buffer));

			if (GC->rtcc->PZTLIPLN.Mode == 2)
			{
				skp->Text(CW * 10, 12 * H / 22, "Pericynthion:", 13);
				sprintf_s(Buffer, "Lat %.3lf", GC->rtcc->PZSFPTAB.blocks[GC->rtcc->PZMCCPLN.SFPBlockNum - 1].lat_pc1 * DEG);
				skp->Text(CW * 10, 13 * H / 22, Buffer, strlen(Buffer));
				sprintf_s(Buffer, "Height %.3lf", GC->rtcc->PZSFPTAB.blocks[GC->rtcc->PZMCCPLN.SFPBlockNum - 1].h_pc1 / 1852.0);
				skp->Text(CW * 10, 14 * H / 22, Buffer, strlen(Buffer));
			}
			else
			{
				skp->Text(CW * 10, 12 * H / 22, "Node:", 5);
				GET_Display2(Buffer, GC->rtcc->PZSFPTAB.blocks[GC->rtcc->PZMCCPLN.SFPBlockNum - 1].GMT_nd);
				skp->Text(CW * 10, 13 * H / 22, Buffer, strlen(Buffer));
				sprintf_s(Buffer, "Lat %.3lf", GC->rtcc->PZSFPTAB.blocks[GC->rtcc->PZMCCPLN.SFPBlockNum - 1].lat_nd * DEG);
				skp->Text(CW * 10, 14 * H / 22, Buffer, strlen(Buffer));
				sprintf_s(Buffer, "Lng %.3lf", GC->rtcc->PZSFPTAB.blocks[GC->rtcc->PZMCCPLN.SFPBlockNum - 1].lng_nd * DEG);
				skp->Text(CW * 10, 15 * H / 22, Buffer, strlen(Buffer));
				sprintf_s(Buffer, "Height %.3lf", GC->rtcc->PZSFPTAB.blocks[GC->rtcc->PZMCCPLN.SFPBlockNum - 1].h_nd / 1852.0);
				skp->Text(CW * 10, 16 * H / 22, Buffer, strlen(Buffer));
			}
		}

		x = 18; y = 4;
		skp->Text(W - CW * x, y * H / 14, "GET RP", 6); y++;
		skp->Text(W - CW * x, y * H / 14, "GET TIG", 7); y++;
		skp->Text(W - CW * x, y * H / 14, "INCL", 4); y++;
		skp->Text(W - CW * x, y * H / 14, "DESC", 4); y++;
		skp->Text(W - CW * x, y * H / 14, "ECC", 3); y++;
		skp->Text(W - CW * x, y * H / 14, "C3", 2); y++;
		skp->Text(W - CW * x, y * H / 14, "ALPHA", 5); y++;
		skp->Text(W - CW * x, y * H / 14, "TA", 2); y++;
		skp->Text(W - CW * x, y * H / 14, "DV", 2); y++;
		skp->Text(W - CW * x, y * H / 14, "BT", 2);

		if (GC->rtcc->PZTTLIPL.DataIndicator == 1)
		{
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			y = 4;
			GET_Display(Buffer, GC->rtcc->PZTPDDIS.GET_TB6, false);
			skp->Text(W - CW, y * H / 14, Buffer, strlen(Buffer)); y++;
			GET_Display(Buffer, GC->rtcc->PZTPDDIS.GET_TIG, false);
			skp->Text(W - CW, y * H / 14, Buffer, strlen(Buffer)); y++;
			Text_Double(skp, W - CW, y * H / 14, "%.3lf", GC->rtcc->PZTTLIPL.elem.Inclination*DEG); y++;
			Text_Double(skp, W - CW, y * H / 14, "%.3lf", GC->rtcc->PZTTLIPL.elem.theta_N*DEG); y++;
			Text_Double(skp, W - CW, y * H / 14, "%.5lf", GC->rtcc->PZTTLIPL.elem.e); y++;
			Text_Double(skp, W - CW, y * H / 14, "%.3lf", GC->rtcc->PZTTLIPL.elem.C3 / pow(1852.0, 2)); y++;
			Text_Double(skp, W - CW, y * H / 14, "%.3lf", GC->rtcc->PZTTLIPL.elem.alpha_D*DEG); y++;
			Text_Double(skp, W - CW, y * H / 14, "%.3lf", GC->rtcc->PZTTLIPL.elem.f*DEG); y++;
			Text_Double(skp, W - CW, y * H / 14, "%.1lf", GC->rtcc->PZTPDDIS.dv_TLI); y++;
			OrbMech::SStoMMSS(GC->rtcc->PZTPDDIS.T_b, mm, secs);
			sprintf(Buffer, "%d:%02.0f", mm, secs);
			skp->Text(W - CW, y * H / 14, Buffer, strlen(Buffer));
		}
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		switch (G->iuUplinkResult)
		{
		case 1:
			sprintf(Buffer, "Uplink accepted!");
			break;
		case 2:
			sprintf(Buffer, "No vessel or IU!");
			break;
		case 3:
			sprintf(Buffer, "Uplink rejected!");
			break;
		case 4:
			sprintf(Buffer, "No TLI data!");
			break;
		default:
			sprintf(Buffer, "No Uplink");
			break;
		}
		skp->Text(CW * 15, H - CH, Buffer, strlen(Buffer));
		break;
	case 80:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "MIDCOURSE INPUTS AND CONSTANTS", 30);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->SetFont(font_menu2);
		GetCharSize(skp, CW, CH);
		skp->Text(CW, 4 * H / 28, "MED F22", 7);
		skp->Text(CW, 5 * H / 28, "Azimuth Constraints (Modes 3/5)", 31);
		Text_Double(skp, CW * 3, 6 * H / 28, "%.2lf°", GC->rtcc->PZMCCPLN.AZ_min*DEG);
		Text_Double(skp, CW * 3, 7 * H / 28, "%.2lf°", GC->rtcc->PZMCCPLN.AZ_max*DEG);
		skp->Text(CW, 9 * H / 28, "MED F23", 7);
		skp->Text(CW, 10 * H / 28, "Min/Max GET at Node (Modes 4/5)", 31);
		GET_Display(Buffer, GC->rtcc->PZMCCPLN.TLMIN*3600.0);
		skp->Text(CW * 3, 11 * H / 28, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->rtcc->PZMCCPLN.TLMAX*3600.0);
		skp->Text(CW * 3, 12 * H / 28, Buffer, strlen(Buffer));
		skp->Text(CW, 14 * H / 28, "MED F24", 7);
		skp->Text(CW, 15 * H / 28, "Gamma and Reentry Range", 23);
		sprintf_s(Buffer, "%.2lf°", GC->rtcc->PZMCCPLN.gamma_reentry*DEG);
		skp->Text(CW * 3, 16 * H / 28, Buffer, strlen(Buffer));
		sprintf_s(Buffer, "%.0lf NM", GC->rtcc->PZMCCPLN.Reentry_range);
		skp->Text(CW * 3, 17 * H / 28, Buffer, strlen(Buffer));
		skp->Text(CW, 19 * H / 28, "MED F29", 7);
		skp->Text(CW, 20 * H / 28, "Pericynthion height limits (Mode 9)", 35);
		sprintf_s(Buffer, "Min: %.0lf NM", GC->rtcc->PZMCCPLN.H_PCYN_MIN / 1852.0);
		skp->Text(CW * 3, 21 * H / 28, Buffer, strlen(Buffer));
		sprintf_s(Buffer, "Max: %.0lf NM", GC->rtcc->PZMCCPLN.H_PCYN_MAX / 1852.0);
		skp->Text(CW * 3, 22 * H / 28, Buffer, strlen(Buffer));
		skp->Text(CW, 24 * H / 28, "Latitude Bias (Modes 8/9)", 25);
		Text_Double(skp, CW * 3, 25 * H / 28, "%.1lf°", GC->rtcc->PZMCCPLN.LATBIAS*DEG);

		skp->Text(W - CW * 29, 4 * H / 28, "LOI/DOI Geometry (Modes 2/4)", 28);
		x = 18; y = 5;
		Text_Double(skp, W - CW * x, y * H / 28, "HALOI1 %.1lf", GC->rtcc->PZMCCPLN.H_A_LPO1 / 1852.0); y++;
		Text_Double(skp, W - CW * x, y * H / 28, "HPLOI1 %.1lf", GC->rtcc->PZMCCPLN.H_P_LPO1 / 1852.0); y++;
		Text_Double(skp, W - CW * x, y * H / 28, "HALOI2 %.1lf", GC->rtcc->PZMCCPLN.H_A_LPO2 / 1852.0); y++;
		Text_Double(skp, W - CW * x, y * H / 28, "HPLOI2 %.2lf", GC->rtcc->PZMCCPLN.H_P_LPO2 / 1852.0); y++;
		Text_Double(skp, W - CW * x, y * H / 28, "REVS1 %.2lf", GC->rtcc->PZMCCPLN.REVS1); y++;
		Text_Int(skp, W - CW * x, y * H / 28, "REVS2 %d", GC->rtcc->PZMCCPLN.REVS2); y++;
		Text_Double(skp, W - CW * x, y * H / 28, "SITEROT %.1lf°", GC->rtcc->PZMCCPLN.SITEROT*DEG); y++;
		Text_Double(skp, W - CW * x, y * H / 28, "ETA1 %.3lf°", GC->rtcc->PZMCCPLN.ETA1*DEG); y++; y++;
		skp->Text(W - CW * 27, y * H / 28, "Mission Constants", 17); y++;
		Text_Double(skp, W - CW * x, y * H / 28, "M %d", GC->rtcc->PZMCCPLN.LOPC_M); y++;
		Text_Double(skp, W - CW * x, y * H / 28, "N %d", GC->rtcc->PZMCCPLN.LOPC_N); y++;
		Text_Double(skp, W - CW * x, y * H / 28, "I PR MAX %.3lf°", GC->rtcc->PZMCCPLN.INCL_PR_MAX*DEG);
		break;
	case 81:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "NODAL TARGET CONVERSION", 30);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (G->NodeConvOpt)
		{
			skp->Text(CW, 2 * H / 14, "Selenographic to EMP", 20);
		}
		else
		{
			skp->Text(CW, 2 * H / 14, "EMP to Selenographic", 20);
		}
		GET_Display3(Buffer, G->NodeConvGET);
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		Text_Double(skp, CW, 6 * H / 14, "%.3lf°", G->NodeConvLat*DEG);
		Text_Double(skp, CW, 8 * H / 14, "%.3lf°", G->NodeConvLng*DEG);
		Text_Double(skp, CW, 10 * H / 14, "%.3lf NM", G->NodeConvHeight / 1852.0);
		skp->Text(W - CW * 18, 8 * H / 14, "Result:", 7);
		Text_Double(skp, W - CW * 18, 9 * H / 14, "%.3lf° Lat", G->NodeConvResLat*DEG);
		Text_Double(skp, W - CW * 18, 10 * H / 14, "%.3lf° Lng", G->NodeConvResLng*DEG);
		break;
	case 82:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "LOI Initialization", 18);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		Text_Double(skp, CW, 2 * H / 14, "%.1lf NM", GC->rtcc->PZLOIPLN.HA_LLS);
		Text_Double(skp, CW, 4 * H / 14, "%.2lf NM", GC->rtcc->PZLOIPLN.HP_LLS);
		Text_Double(skp, CW, 6 * H / 14, "%.1lf°", GC->rtcc->PZLOIPLN.DW);
		Text_Double(skp, CW, 8 * H / 14, "%.2lf", GC->rtcc->PZLOIPLN.REVS1);
		Text_Int(skp, CW, 10 * H / 14, "%d", GC->rtcc->PZLOIPLN.REVS2);
		Text_Double(skp, CW, 12 * H / 14, "%.1lf°", GC->rtcc->PZLOIPLN.eta_1);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		Text_Double(skp, W - CW, 2 * H / 14, "%.1lf NM", GC->rtcc->PZLOIPLN.dh_bias);
		if (GC->rtcc->PZLOIPLN.PlaneSolnForInterSoln)
		{
			skp->Text(W - CW, 4 * H / 14, "Plane solution", 18);
		}
		else
		{
			skp->Text(W - CW, 4 * H / 14, "Min theta solution", 18);
		}
		break;
	case 83:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 10, 0, "LUNAR ORBIT INSERTION PLANNING");
		Text(skp, 52, 0, "0078");
		Text(skp, 1, 1, "CSM STA");
		Text(skp, 0, 2, "GET VECTOR");
		Text(skp, 3, 4, "LAT LLS");
		Text(skp, 3, 5, "LNG LLS");
		Text(skp, 5, 6, "R LLS");
		Text(skp, 25, 4, "REVS1");
		Text(skp, 25, 5, "REVS2");
		Text(skp, 24, 6, "DHBIAS");
		Text(skp, 41, 4, "HALOI1");
		Text(skp, 41, 5, "HPLOI1");
		Text(skp, 41, 6, "HALOI2");
		Text(skp, 41, 7, "HPLOI2");
		Text(skp, 1, 9, "AZMN FND");
		Text(skp, 1, 10, "AZMX FND");
		Text(skp, 24, 9, "AZLLS");
		Text(skp, 25, 10, "FLLS");
		Text(skp, 41, 9, "DVMAX+");
		Text(skp, 41, 10, "DVMAX-");
		Text(skp, 41, 11, "RA-RP GT");
		Text(skp, 1, 13, "CODE");
		Text(skp, 7, 13, "GETLOI");
		Text(skp, 14, 13, "DVLOI1");
		Text(skp, 21, 13, "DVLOI2");
		Text(skp, 29, 13, "HND");
		Text(skp, 34, 13, "FND/H");
		Text(skp, 40, 13, "HPC");
		Text(skp, 44, 13, "THETA");
		Text(skp, 50, 13, "FND/E");
		Text(skp, 21, 15, "INTERSECTION");
		Text(skp, 23, 18, "COPLANAR");
		Text(skp, 23, 21, "MIN THETA");
		Text(skp, 25, 24, "PLANE");
		y = (CH * 25) / 2;
		dy = CH * 2;
		for (int i = 0; i < 5; i++)
		{
			Line(skp, CW / 2, y, (CW * 111) / 2, y);
			Line(skp, CW / 2, y + dy, (CW * 111) / 2, y + dy);
			Line(skp, CW / 2, y, CW / 2, y + dy);
			Line(skp, (CW * 13) / 2, y, (CW * 13) / 2, y + dy);
			Line(skp, (CW * 27) / 2, y, (CW * 27) / 2, y + dy);
			Line(skp, (CW * 41) / 2, y, (CW * 41) / 2, y + dy);
			Line(skp, (CW * 55) / 2, y, (CW * 55) / 2, y + dy);
			Line(skp, (CW * 67) / 2, y, (CW * 67) / 2, y + dy);
			Line(skp, (CW * 79) / 2, y, (CW * 79) / 2, y + dy);
			Line(skp, (CW * 87) / 2, y, (CW * 87) / 2, y + dy);
			Line(skp, (CW * 99) / 2, y, (CW * 99) / 2, y + dy);
			Line(skp, (CW * 111) / 2, y, (CW * 111) / 2, y + dy);

			if (i == 0)
			{
				y = (CH * 127) / 8;
			}
			else
			{
				y += (CH * 3);
			}
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text(skp, 18, 1, GC->rtcc->PZLRBTI.StaID);
		Text_GET_HHHMMSS(skp, 20, 2, GC->rtcc->PZLRBTI.VectorGET);
		Text_Latitude(skp, 20, 4, GC->rtcc->PZLRBTI.lat_lls);
		Text_Longitude(skp, 20, 5, GC->rtcc->PZLRBTI.lng_lls);
		Text(skp, 19, 6, "%.4lf", GC->rtcc->PZLRBTI.R_lls);
		if (GC->rtcc->PZLRBTI.planesoln)
		{
			Text(skp, 30, 7, "PLANE");
		}
		else
		{
			Text(skp, 34, 7, "MIN THETA");
		}
		Text(skp, 35, 4, "%.2lf", GC->rtcc->PZLRBTI.REVS1);
		Text(skp, 35, 5, "%d", GC->rtcc->PZLRBTI.REVS2);
		Text(skp, 35, 6, "%+.1lf", GC->rtcc->PZLRBTI.DHBIAS);
		Text(skp, 54, 4, "%.1lf", GC->rtcc->PZLRBTI.HALOI1);
		Text(skp, 54, 5, "%.1lf", GC->rtcc->PZLRBTI.HPLOI1);
		Text(skp, 54, 6, "%.1lf", GC->rtcc->PZLRBTI.HALOI2);
		Text(skp, 54, 7, "%.2lf", GC->rtcc->PZLRBTI.HPLOI2);
		Text(skp, 16, 9, "%.2lf", GC->rtcc->PZLRBTI.AZMN_f_ND);
		Text(skp, 16, 10, "%.2lf", GC->rtcc->PZLRBTI.AZMX_f_ND);
		Text(skp, 35, 9, "%.1lf", GC->rtcc->PZLRBTI.AZ_LLS);
		Text(skp, 35, 10, "%.1lf", GC->rtcc->PZLRBTI.f_LLS);
		Text(skp, 54, 9, "%.0lf", GC->rtcc->PZLRBTI.DVMAXp);
		Text(skp, 54, 10, "%.0lf", GC->rtcc->PZLRBTI.DVMAXm);
		Text(skp, 54, 11, "%.1lf", GC->rtcc->PZLRBTI.RARPGT);
		for (int i = 0; i < 4; i++)
		{
			Text(skp, 4, 16 + 3 * i, "%d", 2 * i + 1);
			Text(skp, 4, 17 + 3 * i, "%d", 2 * i + 2);
			Text_GET_HHHMM(skp, 13, 16 + 3 * i, GC->rtcc->PZLRBTI.sol[2 * i].GETLOI);
			Text_GET_HHHMM(skp, 13, 17 + 3 * i, GC->rtcc->PZLRBTI.sol[2 * i + 1].GETLOI);
			Text(skp, 19, 16 + 3 * i, "%.0lf", GC->rtcc->PZLRBTI.sol[2 * i].DVLOI1);
			Text(skp, 19, 17 + 3 * i, "%.0lf", GC->rtcc->PZLRBTI.sol[2 * i + 1].DVLOI1);
			Text(skp, 26, 16 + 3 * i, "%.0lf", GC->rtcc->PZLRBTI.sol[2 * i].DVLOI2);
			Text(skp, 26, 17 + 3 * i, "%.0lf", GC->rtcc->PZLRBTI.sol[2 * i + 1].DVLOI2);
			Text(skp, 33, 16 + 3 * i, "%.1lf", GC->rtcc->PZLRBTI.sol[2 * i].H_ND);
			Text(skp, 33, 17 + 3 * i, "%.1lf", GC->rtcc->PZLRBTI.sol[2 * i + 1].H_ND);
			Text(skp, 39, 16 + 3 * i, "%.1lf", GC->rtcc->PZLRBTI.sol[2 * i].f_ND_H);
			Text(skp, 39, 17 + 3 * i, "%.1lf", GC->rtcc->PZLRBTI.sol[2 * i + 1].f_ND_H);
			Text(skp, 43, 16 + 3 * i, "%.0lf", GC->rtcc->PZLRBTI.sol[2 * i].H_PC);
			Text(skp, 43, 17 + 3 * i, "%.0lf", GC->rtcc->PZLRBTI.sol[2 * i + 1].H_PC);
			Text(skp, 49, 16 + 3 * i, "%.2lf", GC->rtcc->PZLRBTI.sol[2 * i].Theta);
			Text(skp, 49, 17 + 3 * i, "%.2lf", GC->rtcc->PZLRBTI.sol[2 * i + 1].Theta);
			Text(skp, 55, 16 + 3 * i, "%.1lf", GC->rtcc->PZLRBTI.sol[2 * i].f_ND_E);
			Text(skp, 55, 17 + 3 * i, "%.1lf", GC->rtcc->PZLRBTI.sol[2 * i + 1].f_ND_E);
		}
		break;
	case 84:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "DIRECT INPUT TO MPT (MED M66)", 29);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (GC->rtcc->med_m66.UllageDT < 0)
		{
			sprintf_s(Buffer, "Default");
		}
		else
		{
			sprintf_s(Buffer, "%.0lfs Ullage DT", GC->rtcc->med_m66.UllageDT);
		}
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->med_m66.UllageQuads)
		{
			skp->Text(CW, 4 * H / 14, "4 Thrusters", 11);
		}
		else
		{
			skp->Text(CW, 4 * H / 14, "2 Thrusters", 11);
		}
		if (GC->rtcc->med_m66.BurnParamNo == 1 && GC->rtcc->med_m66.CoordInd >= 1)
		{
			skp->Text(CW, 6 * H / 14, "REFSMMAT: CUR", 13);
		}
		if (GC->rtcc->med_m66.ConfigChangeInd == 0)
		{
			skp->Text(CW, 8 * H / 14, "No change", 9);
		}
		else
		{
			if (GC->rtcc->med_m66.ConfigChangeInd == 1)
			{
				skp->Text(CW, 8 * H / 14, "Undocking", 9);
			}
			else if (GC->rtcc->med_m66.ConfigChangeInd == 2)
			{
				skp->Text(CW, 8 * H / 14, "Docking", 7);
			}

			sprintf_s(Buffer, GC->rtcc->med_m66.FinalConfig.c_str());
			skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		}
		Text_Double(skp, CW, 12 * H/14, "%+.1lf° Delta Docking Angle", GC->rtcc->med_m66.DeltaDA*DEG);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (GC->rtcc->med_m66.TrimAngleIndicator == 0)
		{
			skp->Text(W - CW, 2 * H / 14, "Compute Trim", 12);
		}
		else
		{
			skp->Text(W - CW, 2 * H / 14, "System Parameters", 17);
		}
		skp->Text(W - CW, 4 * H / 14, "Transfer to MPT", 15);
		skp->Text(W - CW, 10 * H / 14, "Page 2/2", 8);
		break;
	case 85:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 2, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(2);
		Text(skp, 20, 0, "EXPERIMENTAL SITE ACQUISITION");
		Text(skp, 60, 0, "1506");
		Text(skp, 11, 2, "STA ID");
		Text(skp, 39, 2, "PAGE NO");
		Text(skp, 2, 5, "REV");
		Text(skp, 7, 5, "STA");
		Text(skp, 27, 5, "GETAOS", 2, 1);
		Text(skp, 45, 5, "GND RNG", 2, 1);
		Text(skp, 65, 5, "ALT", 2, 1);
		Text(skp, 38, 5, "EMAX");
		Text(skp, 45, 5, "GETCA");
		Text(skp, 109, 5, "GETLOS", 2, 1);
		y = CH * 5;
		dy = CH * 32;
		Line(skp, (CW * 3) / 2, y, (CW * 3) / 2, dy);
		Line(skp, (CW * 11) / 2, y, (CW * 11) / 2, dy);
		Line(skp, (CW * 23) / 2, y, (CW * 23) / 2, dy);
		Line(skp, (CW * 43) / 2, y, (CW * 43) / 2, dy);
		Line(skp, (CW * 61) / 2, y, (CW * 61) / 2, dy);
		Line(skp, (CW * 75) / 2, y, (CW * 75) / 2, dy);
		Line(skp, (CW * 85) / 2, y, (CW * 85) / 2, dy);
		Line(skp, (CW * 105) / 2, y, (CW * 105) / 2, dy);
		Line(skp, (CW * 125) / 2, y, (CW * 125) / 2, dy);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 2, true);
		Text(skp, 25, 2, GC->rtcc->EZDPSAD2.STAID);
		if (GC->rtcc->EZDPSAD2.pages > 1) Text(skp, 49, 2, "%d", GC->rtcc->EZDPSAD2.curpage);
		{
			int j = GC->rtcc->EZDPSAD2.curpage - 1;
			for (unsigned i = 0; i < GC->rtcc->EZDPSAD2.numcontacts[j]; i++)
			{
				Text(skp, 5, 6 + i, "%d", GC->rtcc->EZDPSAD2.REV[j][i]);
				Text(skp, 11, 6 + i, GC->rtcc->EZDPSAD2.STA[j][i]);
				if (GC->rtcc->EZDPSAD2.BestAvailableAOS[j][i]) Text(skp, 12, 6 + i, "*");
				Text_GET_HHHMMSS(skp, 21, 6 + i, GC->rtcc->EZDPSAD2.GETAOS[j][i]);
				Text(skp, 30, 6 + i, "%+.2lf", GC->rtcc->EZDPSAD2.GNDRNG[j][i]);
				Text(skp, 37, 6 + i, "%.2lf", GC->rtcc->EZDPSAD2.ALT[j][i]);
				if (GC->rtcc->EZDPSAD2.BestAvailableEMAX[j][i]) Text(skp, 38, 6 + i, "*");
				Text(skp, 42, 6 + i, "%.1lf", GC->rtcc->EZDPSAD2.ELMAX[j][i]);
				Text_GET_HHHMMSS(skp, 52, 6 + i, GC->rtcc->EZDPSAD2.GETCA[j][i]);
				if (GC->rtcc->EZDPSAD2.BestAvailableLOS[j][i]) Text(skp, 53, 6 + i, "*");
				Text_GET_HHHMMSS(skp, 62, 6 + i, GC->rtcc->EZDPSAD2.GETLOS[j][i]);
			}
		}
		break;
	case 86:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 15, 0, "RELATIVE MOTION DIGITALS");
		Text(skp, 52, 0, "0060");
		Text(skp, 0, 1, "CSM STAID");
		Text(skp, 18, 1, "LM STAID");
		Text(skp, 35, 1, "GETR");
		Text(skp, 0, 2, "GMTV");
		Text(skp, 16, 2, "GMTV");
		Text(skp, 32, 2, "VEH");
		Text(skp, 43, 2, "AXIS AT TGT");
		Text(skp, 0, 3, "GETV");
		Text(skp, 16, 3, "GETV");
		Text(skp, 33, 3, "REFSMMAT NO");
		Text(skp, 0, 4, "MODE");
		Text(skp, 1, 5, "GET");
		Text(skp, 12, 5, "R");
		Text_Dot(skp, 17, 5);
		Text(skp, 17, 5, "R/PB");
		Text(skp, 23, 5, "AZ/YB");
		Text(skp, 29, 5, "EL/RB");
		Text(skp, 36, 5, "X/P");
		Text(skp, 44, 5, "Z/Y");
		Text(skp, 51, 5, "Y/R");
		Line(skp, CW * 7, (CH * 9) / 2, CW * 56, (CH * 9) / 2);
		y = (CH * 9) / 2;
		dy = (CH * 13) / 4;
		for (int i = 0; i < 11; i++)
		{
			Line(skp, (CW * 19) / 2, y, (CW * 19) / 2, y + dy);
			Line(skp, (CW * 33) / 2, y, (CW * 33) / 2, y + dy);
			Line(skp, (CW * 45) / 2, y, (CW * 45) / 2, y + dy);
			Line(skp, (CW * 57) / 2, y, (CW * 57) / 2, y + dy);
			Line(skp, (CW * 69) / 2, y, (CW * 69) / 2, y + dy);
			Line(skp, (CW * 85) / 2, y, (CW * 85) / 2, y + dy);
			Line(skp, (CW * 99) / 2, y, (CW * 99) / 2, y + dy);

			if (i == 0)
			{
				y = (CH * 33) / 4;
				dy = (CH * 3) / 2;
			}
			else
			{
				y += CH * 2;
			}
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text(skp, 17, 1, GC->rtcc->EZRMDT.CSMSTAID);
		Text(skp, 34, 1, GC->rtcc->EZRMDT.LMSTAID);
		Text_GET_HHHMMSS(skp, 49, 1, GC->rtcc->EZRMDT.GETR);
		Text_GET_HHHMMSS(skp, 14, 2, GC->rtcc->EZRMDT.CSMGMTV);
		Text_GET_HHHMMSS(skp, 30, 2, GC->rtcc->EZRMDT.LMGMTV);
		Text(skp, 39, 2, GC->rtcc->EZRMDT.VEH);
		Text(skp, 42, 2, std::string(1, GC->rtcc->EZRMDT.AXIS));
		Text_GET_HHHMMSS(skp, 14, 3, GC->rtcc->EZRMDT.CSMGETV);
		Text_GET_HHHMMSS(skp, 30, 3, GC->rtcc->EZRMDT.LMGETV);
		Text(skp, 51, 3, GC->rtcc->EZRMDT.REFSMMAT);
		Text(skp, 6, 4, std::string(1, GC->rtcc->EZRMDT.Mode));
		Text(skp, 8, 5, GC->rtcc->EZRMDT.PETorSH);
		Text(skp, 15, 5, GC->rtcc->EZRMDT.YDotorT);
		if (GC->rtcc->EZRMDT.Mode == '1')
		{
			Text_Dot(skp, 15, 5);
		}
		Text(skp, 40, 5, std::string(1, GC->rtcc->EZRMDT.Pitch));
		Text(skp, 48, 5, std::string(1, GC->rtcc->EZRMDT.Yaw));
		Text(skp, 55, 5, std::string(1, GC->rtcc->EZRMDT.Roll));
		Text(skp, 40, 27, GC->rtcc->EZRMDT.error);
		for (int i = 0; i < GC->rtcc->EZRMDT.solns; i++)
		{
			Text_GET_HHHMMSS(skp, 9, 6 + i * 2, GC->rtcc->EZRMDT.data[i].GET);
			if (GC->rtcc->EZRMDT.Mode == '1')
			{
				Text_GET_HHHMMSS(skp, 9, 7 + i * 2, abs(GC->rtcc->EZRMDT.data[i].PETorShaft)); //Negative PET's will be displayed as positive, says the IBM document
			}
			else
			{
				Text(skp, 9, 7 + i * 2, "%.1lf", GC->rtcc->EZRMDT.data[i].PETorShaft);
			}
			if (GC->rtcc->EZRMDT.data[i].R <= 9999.9)
			{
				Text(skp, 16, 6 + i * 2, "%.1lf", GC->rtcc->EZRMDT.data[i].R);
			}
			if (GC->rtcc->EZRMDT.Mode == '1')
			{
				if (abs(GC->rtcc->EZRMDT.data[i].YdotorTrun) <= 999.9)
				{
					Text(skp, 16, 7 + i * 2, "%+.1lf", GC->rtcc->EZRMDT.data[i].YdotorTrun);
				}
			}
			else
			{
				Text(skp, 16, 7 + i * 2, "%.1lf", GC->rtcc->EZRMDT.data[i].YdotorTrun);
			}
			if (abs(GC->rtcc->EZRMDT.data[i].RDOT) <= 999.9)
			{
				Text(skp, 22, 6 + i * 2, "%.1lf", GC->rtcc->EZRMDT.data[i].RDOT);
			}
			Text(skp, 22, 7 + i * 2, "%.1lf", GC->rtcc->EZRMDT.data[i].PB);
			Text(skp, 28, 6 + i * 2, "%.1lf", GC->rtcc->EZRMDT.data[i].AZH);
			Text(skp, 28, 7 + i * 2, "%.1lf", GC->rtcc->EZRMDT.data[i].YB);
			Text(skp, 34, 6 + i * 2, "%.1lf", GC->rtcc->EZRMDT.data[i].EL);
			Text(skp, 34, 7 + i * 2, "%.1lf", GC->rtcc->EZRMDT.data[i].RB);
			if (GC->rtcc->EZRMDT.Mode == '1' && abs(GC->rtcc->EZRMDT.data[i].X) <= 9999.9)
			{
				sprintf_s(Buffer, "%.1lf%c", GC->rtcc->EZRMDT.data[i].X, GC->rtcc->EZRMDT.data[i].XInd);
				Text(skp, 42, 6 + i * 2, Buffer);
			}
			Text(skp, 42, 7 + i * 2, "%.1lf", GC->rtcc->EZRMDT.data[i].Pitch);
			if (GC->rtcc->EZRMDT.Mode == '1' && abs(GC->rtcc->EZRMDT.data[i].Z) <= 9999.9)
			{
				sprintf_s(Buffer, "%.1lf%c", GC->rtcc->EZRMDT.data[i].Z, GC->rtcc->EZRMDT.data[i].ZInd);
				Text(skp, 49, 6 + i * 2, Buffer);
			}
			Text(skp, 49, 7 + i * 2, "%.1lf", GC->rtcc->EZRMDT.data[i].Yaw);
			if (GC->rtcc->EZRMDT.Mode == '1' && abs(GC->rtcc->EZRMDT.data[i].Y) <= 999.9)
			{
				sprintf_s(Buffer, "%.1lf%c", GC->rtcc->EZRMDT.data[i].Y, GC->rtcc->EZRMDT.data[i].YInd);
				Text(skp, 56, 6 + i * 2, Buffer);
			}
			Text(skp, 56, 7 + i * 2, "%.1lf", GC->rtcc->EZRMDT.data[i].Roll);
		}
		break;
	case 87:
		RendezvousEvaluationDisplay(skp);
		break;
	case 88:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Init for LM Targeting and Launch Window", 39);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		Text_Double(skp, CW, 2 * H / 14, "%.3lf°", GC->rtcc->PZLTRT.PoweredFlightArc*DEG);
		Text_Double(skp, CW, 4 * H / 14, "%.1lf s", GC->rtcc->PZLTRT.PoweredFlightTime);
		Text_Double(skp, CW, 6 * H / 14, "%.0lf ft", GC->rtcc->PZLTRT.InsertionHeight / 0.3048);
		Text_Double(skp, CW, 8 * H / 14, "%.1lf ft/s", GC->rtcc->PZLTRT.InsertionHorizontalVelocity / 0.3048);
		Text_Double(skp, CW, 10 * H / 14, "%.1lf ft/s", GC->rtcc->PZLTRT.InsertionRadialVelocity / 0.3048);
		Text_Double(skp, CW, 12 * H / 14, "%.2lf°", GC->rtcc->PZLTRT.YawSteerCap*DEG);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		Text_Double(skp, W - CW, 2 * H / 14, "%.2lf hrs", GC->rtcc->PZLTRT.MaxAscLifetime / 3600.0);
		Text_Double(skp, W - CW, 4 * H / 14, "%.2lf NM", GC->rtcc->PZLTRT.MinSafeHeight / 1852.0);
		Text_Double(skp, W - CW, 6 * H / 14, "%.2lf ft/s", GC->rtcc->PZLTRT.LMMaxDeltaV / 0.3048);
		Text_Double(skp, W - CW, 8 * H / 14, "%.2lf ft/s", GC->rtcc->PZLTRT.CSMMaxDeltaV / 0.3048);
		break;
	case 89:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Launch Window Initialization", 28);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "Targeting Parameters", 34);
		Text_Double(skp, CW, 4 * H / 14, "%.1lf min", GC->rtcc->PZLTRT.dt_bias / 60.0);
		Text_Double(skp, CW, 6 * H / 14, "%.2lf°", GC->rtcc->PZLTRT.ElevationAngle*DEG);
		Text_Double(skp, CW, 8 * H / 14, "%.2lf°", GC->rtcc->PZLTRT.TerminalPhaseTravelAngle*DEG);
		Text_Double(skp, CW, 10 * H / 14, "%.2lf NM", GC->rtcc->PZLTRT.TPF_Height_Offset / 1852.0);
		Text_Double(skp, CW, 12 * H / 14, "%.2lf°", GC->rtcc->PZLTRT.TPF_Phase_Offset*DEG);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		Text_Double(skp, W - CW, 2 * H / 14, "%.2lf NM", GC->rtcc->PZLTRT.Height_Diff_Begin / 1852.0);
		Text_Double(skp, W - CW, 4 * H / 14, "%.2lf NM", GC->rtcc->PZLTRT.Height_Diff_Incr / 1852.0);
		break;
	case 90:
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 15, 0, "LUNAR RENDEZVOUS PLAN TABLE");
		Text(skp, 52, 0, "0066");
		Text(skp, 0, 2, "CSM STA");
		Text(skp, 0, 3, "GMTV");
		Text(skp, 0, 4, "GETV");
		Text(skp, 0, 5, "MANVR VEH");
		Text(skp, 16, 2, "LM POSITION");
		TextW(skp, 16, 3, L"\u03C6LLS");
		TextW(skp, 16, 4, L"\u03BBLLS");
		Text(skp, 15, 5, "THT");
		Text(skp, 29, 2, "LM STA");
		Text(skp, 29, 3, "GMTV");
		Text(skp, 29, 4, "GETV");
		Text(skp, 29, 5, "DTCSI");
		Text(skp, 45, 2, "LMLIFE");
		Text(skp, 45, 3, "DVMAX");
		Text(skp, 45, 4, "MIN H");
		Text(skp, 45, 5, "WT");
		Text(skp, 0, 7, "ID");
		Text(skp, 3, 7, "M");
		Text(skp, 7, 7, "DH");
		Text(skp, 13, 7, "GETLO");
		Text(skp, 13, 8, "GETINS");
		Text(skp, 23, 7, "GETCSI");
		Text(skp, 23, 8, "DVCSI");
		Text(skp, 33, 7, "GETCDH");
		Text(skp, 33, 8, "DVCDH");
		Text(skp, 43, 7, "GETTPI");
		Text(skp, 43, 8, "DVTPI");
		Text(skp, 51, 7, "DVTPF");
		Text(skp, 52, 8, "DVT");
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text(skp, 15, 2, GC->rtcc->PZLRPT.CSMSTAID);
		Text_GET_HHHMMSS(skp, 14, 3, GC->rtcc->PZLRPT.CSM_GMTV);
		Text_GET_HHHMMSS(skp, 14, 4, GC->rtcc->PZLRPT.CSM_GETV);
		Text(skp, 14, 5, GC->rtcc->PZLRPT.ManVeh);
		Text_Latitude(skp, 28, 3, GC->rtcc->BZLAND.lat[0] * DEG);
		Text_Longitude(skp, 28, 4, GC->rtcc->BZLAND.lng[0] * DEG);
		Text_GET_HHHMMSS(skp, 28, 5, GC->rtcc->PZLRPT.THT);
		Text(skp, 44, 2, GC->rtcc->PZLRPT.LMSTAID);
		Text_GET_HHHMMSS(skp, 44, 3, GC->rtcc->PZLRPT.LM_GMTV);
		Text_GET_HHHMMSS(skp, 44, 4, GC->rtcc->PZLRPT.LM_GETV);
		if (GC->rtcc->PZLRPT.DT_CSI > 0.0) Text_GET_HHHMMSS(skp, 44, 5, GC->rtcc->PZLRPT.DT_CSI);
		Text(skp, 56, 2, "%.1lf", GC->rtcc->PZLRPT.LM_LIFETIME);
		Text(skp, 56, 3, "%.0lf", GC->rtcc->PZLRPT.DV_MAX);
		Text(skp, 56, 4, "%.1lf", GC->rtcc->PZLRPT.MINH);
		for (int i = 0; i < GC->rtcc->PZLRPT.plans; i++)
		{
			Text(skp, 2, 9 + 3 * i, "%d", GC->rtcc->PZLRPT.data[i].ID);
			Text(skp, 4, 9 + 3 * i, "%d", GC->rtcc->PZLRPT.data[i].N);
			Text(skp, 10, 9 + 3 * i, "%.1lf", GC->rtcc->PZLRPT.data[i].DH);
			Text_GET_HHHMMSS(skp, 20, 9 + 3 * i, GC->rtcc->PZLRPT.data[i].GETLO);
			Text_GET_HHHMMSS(skp, 20, 10 + 3 * i, GC->rtcc->PZLRPT.data[i].T_INS);
			Text_GET_HHHMMSS(skp, 30, 9 + 3 * i, GC->rtcc->PZLRPT.data[i].T_CSI);
			Text(skp, 30, 10 + 3 * i, "%.1lf", GC->rtcc->PZLRPT.data[i].DVCSI);
			Text_GET_HHHMMSS(skp, 40, 9 + 3 * i, GC->rtcc->PZLRPT.data[i].T_CDH);
			Text(skp, 40, 10 + 3 * i, "%.1lf", GC->rtcc->PZLRPT.data[i].DVCDH);
			Text_GET_HHHMMSS(skp, 50, 9 + 3 * i, GC->rtcc->PZLRPT.data[i].T_TPI);
			Text(skp, 50, 10 + 3 * i, "%.1lf", GC->rtcc->PZLRPT.data[i].DVTPI);
			Text(skp, 56, 9 + 3 * i, "%.1lf", GC->rtcc->PZLRPT.data[i].DVTPF);
			Text(skp, 56, 10 + 3 * i, "%.1lf", GC->rtcc->PZLRPT.data[i].DVT);
		}
		break;
	case 91:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 11, 0, "LUNAR LAUNCH TARGETING TABLE");
		Text(skp, 52, 0, "0077");
		Text(skp, 1, 1, "CSM STA ID");
		Text(skp, 1, 2, "GETTH");
		Text(skp, 1, 3, "GETV");
		Text(skp, 29, 1, "LM POSITION");
		TextW(skp, 41, 1, L"\u03C6LLS");
		TextW(skp, 41, 2, L"\u03BBLLS");
		Text(skp, 41, 3, "RLLS");
		Line(skp, 0, CH * 5, CW * 56, CH * 5);
		TextW(skp, 1, 6, L"PF\u2222");
		TextW(skp, 1, 7, L"PF\u0394T");
		Text(skp, 1, 8, "HINS");
		TextW(skp, 1, 9, L"\u1E58INS");
		Text(skp, 1, 10, "YSTEER");
		TextW(skp, 17, 6, L"\u0394T(INS-TPI)");
		TextW(skp, 17, 7, L"\u0394TNOM");
		TextW(skp, 17, 8, L"\u03B8INS");
		TextW(skp, 17, 9, L"\u03B8NOM");
		TextW(skp, 17, 10, L"\u03B8NOM");
		Text_Dot(skp, 17, 10);
		TextW(skp, 41, 6, L"\u0394HTPI");
		TextW(skp, 41, 7, L"\u0394\u03B8TPI");
		TextW(skp, 41, 8, L"\u03C9T");
		Text(skp, 41, 9, "HPMIN");
		TextW(skp, 41, 10, L"EL \u2222");
		Line(skp, 0, CH * 13, CW * 56, CH * 13);
		Text(skp, 1, 14, "GETLOR");
		Text(skp, 24, 14, "VH");
		Line(skp, 0, CH * 16, CW * 56, CH * 16);
		Text(skp, 1, 16, "CODE");
		Text(skp, 9, 16, "GET");
		TextW(skp, 18, 16, L"\u0394V");
		TextW(skp, 25, 16, L"\u0394VX");
		TextW(skp, 33, 16, L"\u0394VY");
		TextW(skp, 41, 16, L"\u0394VZ");
		Text(skp, 48, 16, "HA/HP");
		Line(skp, 0, (CH * 35) / 2, CW * 56, (CH * 35) / 2);
		Text(skp, 1, 18, "TPI");
		Text(skp, 1, 20, "TPF");
		Line(skp, 0, (CH * 45) / 2, CW * 56, (CH * 45) / 2);
		TextW(skp, 42, 24, L"H\u2090T");
		TextW(skp, 42, 25, L"H\u209AT");
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		if (GC->MissionPlanningActive)
		{
			Text(skp, 19, 1, GC->rtcc->PZLLTT.CSM_STA_ID);
		}
		else
		{
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			PrintCSMVessel(Buffer, false);
			Text(skp, 12, 1, Buffer);
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		}
		Text_GET_HHHMMSS(skp, 16, 2, GC->rtcc->med_k50.GETTH);
		Text_GET_HHHMMSS(skp, 16, 3, GC->rtcc->med_k50.GETV);
		Text(skp, 55, 1, "%+.4lf", GC->rtcc->BZLAND.lat[RTCC_LMPOS_BEST] * DEG);
		Text(skp, 55, 2, "%+.4lf", GC->rtcc->BZLAND.lng[RTCC_LMPOS_BEST] * DEG);
		Text(skp, 55, 3, "%.4lf", GC->rtcc->BZLAND.rad[RTCC_LMPOS_BEST] / 1852.0);
		Text(skp, 13, 6, "%.3lf", GC->rtcc->PZLTRT.PoweredFlightArc*DEG);
		Text(skp, 13, 7, "%.2lf", GC->rtcc->PZLTRT.PoweredFlightTime);
		Text(skp, 13, 8, "%.0lf", GC->rtcc->PZLTRT.InsertionHeight / 0.3048);
		Text(skp, 13, 9, "%+.1lf", GC->rtcc->PZLTRT.InsertionRadialVelocity / 0.3048);
		Text(skp, 13, 10, "%.2lf", GC->rtcc->PZLTRT.YawSteerCap*DEG);
		Text(skp, 35, 6, "%.2lf", GC->rtcc->PZLTRT.DT_Ins_TPI / 60.0);
		Text(skp, 35, 7, "%.2lf", GC->rtcc->PZLTRT.DT_Ins_TPI_NOM / 60.0);
		Text(skp, 55, 6, "%+.2lf", GC->rtcc->PZLTRT.DT_DH / 1852.0);
		Text(skp, 55, 7, "%+.2lf", GC->rtcc->PZLTRT.DT_Theta_i*DEG);
		Text(skp, 55, 8, "%.1lf", GC->rtcc->PZLTRT.TerminalPhaseTravelAngle*DEG);
		Text(skp, 55, 9, "%+.1lf", GC->rtcc->PZLTRT.MinSafeHeight / 1852.0);
		Text(skp, 55, 10, "%.1lf", GC->rtcc->PZLTRT.ElevationAngle*DEG);
		Text_GET_HHHMMSSCS(skp, 21, 14, GC->rtcc->PZLLTT.GETLOR);
		Text(skp, 34, 14, "%.1lf", GC->rtcc->PZLLTT.VH / 0.3048);
		Text_GET_HHHMMSS(skp, 15, 18, GC->rtcc->PZLLTT.GET_TPI);
		Text_GET_HHHMMSS(skp, 15, 20, GC->rtcc->PZLLTT.GET_TPF);
		Text(skp, 22, 18, "%.1lf", GC->rtcc->PZLLTT.DV_TPI / 0.3048);
		Text(skp, 22, 20, "%.1lf", GC->rtcc->PZLLTT.DV_TPF / 0.3048);
		Text(skp, 30, 18, "%+.1lf", GC->rtcc->PZLLTT.DV_TPI_LVLH.x / 0.3048);
		Text(skp, 30, 20, "%+.1lf", GC->rtcc->PZLLTT.DV_TPF_LVLH.x / 0.3048);
		Text(skp, 38, 18, "%+.1lf", GC->rtcc->PZLLTT.DV_TPI_LVLH.y / 0.3048);
		Text(skp, 38, 20, "%+.1lf", GC->rtcc->PZLLTT.DV_TPF_LVLH.y / 0.3048);
		Text(skp, 46, 18, "%+.1lf", GC->rtcc->PZLLTT.DV_TPI_LVLH.z / 0.3048);
		Text(skp, 46, 20, "%+.1lf", GC->rtcc->PZLLTT.DV_TPF_LVLH.z / 0.3048);
		Text(skp, 54, 18, "%.1lf", GC->rtcc->PZLLTT.HA_TPI / 1852.0);
		Text(skp, 54, 19, "%+.1lf", GC->rtcc->PZLLTT.HP_TPI / 1852.0);
		Text(skp, 54, 20, "%.1lf", GC->rtcc->PZLLTT.HA_TPF / 1852.0);
		Text(skp, 54, 21, "%+.1lf", GC->rtcc->PZLLTT.HP_TPF / 1852.0);
		Text(skp, 54, 24, "%.1lf", GC->rtcc->PZLLTT.HA_T / 1852.0);
		Text(skp, 54, 25, "%+.1lf", GC->rtcc->PZLLTT.HP_T / 1852.0);
		break;
	case 92:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "TPI TIMES", 9);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		PrintTargetVessel(Buffer);
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		if (G->TPI_Mode == 0)
		{
			skp->Text(CW, 4 * H / 14, "TPI on time", 11);
		}
		else if (G->TPI_Mode == 1)
		{
			skp->Text(CW, 4 * H / 14, "TPI at orbital midnight", 23);
		}
		else
		{
			skp->Text(CW, 4 * H / 14, "TPI at X min before sunrise:", 28);
		}
		if (G->TPI_Mode == 2)
		{
			sprintf_s(Buffer, "%.1f min", G->dt_TPI_sunrise / 60.0);
			skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		}
		GET_Display(Buffer, G->t_TPIguess, false);
		skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->t_TPI, false);
		skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		break;
	case 93:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 15, 0, "VECTOR COMPARISON DISPLAY");
		Text(skp, 52, 0, "1590");
		Text(skp, 3, 1, "VEHICLE");
		Text(skp, 15, 1, "GMT");
		Text(skp, 33, 1, "GET");
		Text(skp, 50, 1, "REF");
		TextW(skp, 2, 5, L"H\u2090");
		TextW(skp, 2, 6, L"H\u209A");
		Text(skp, 2, 7, "V");
		TextW(skp, 2, 8, L"\u03B3");
		TextW(skp, 2, 9, L"\u03C8");
		TextW(skp, 2, 10, L"\u03C6");
		TextW(skp, 2, 11, L"\u03BB");
		Text(skp, 2, 12, "h");
		Text(skp, 2, 14, "a");
		Text(skp, 2, 15, "e");
		Text(skp, 2, 16, "i");
		TextW(skp,  2, 17, L"\u03C9\u209A");
		TextW(skp, 2, 18, L"\u03A9");
		TextW(skp, 2, 19, L"\u03BD");
		TextW(skp, 2, 21, L"U");
		TextW(skp, 2, 22, L"V");
		TextW(skp, 2, 23, L"W");
		TextW(skp, 2, 24, L"U\u0307");
		TextW(skp, 2, 25, L"V\u0307");
		TextW(skp, 2, 26, L"W\u0307");
		x = 9; y = (CH * 5) / 2; dy = CH * 27;
		Line(skp, (CW * x) / 2, y, (CW * x) / 2, dy); x = 35;
		Line(skp, (CW * x) / 2, y, (CW * x) / 2, dy); x += 26;
		Line(skp, (CW * x) / 2, y, (CW * x) / 2, dy); x += 26;
		Line(skp, (CW * x) / 2, y, (CW * x) / 2, dy);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		GC->rtcc->med_s80.VEH == 1 ? Text(skp, 14, 1, "CSM") : Text(skp, 14, 1, "LEM");
		Text_GET_HHHMMSSCS(skp, 31, 1, GC->rtcc->VectorCompareDisplayBuffer.GMT);
		Text_GET_HHHMMSSCS(skp, 49, 1, GC->rtcc->VectorCompareDisplayBuffer.GET);
		GC->rtcc->med_s80.REF == BODY_EARTH ? Text(skp, 55, 1, "E") : Text(skp, 55, 1, "M");
		Text(skp, 40, 27, GC->rtcc->VectorCompareDisplayBuffer.error);
		for (int i = 0; i < 4; i++)
		{
			Text(skp, 14 + i * 13, 2, GC->rtcc->med_s80.VID[i]);
		}
		for (int i = 0; i < GC->rtcc->VectorCompareDisplayBuffer.NumVec; i++)
		{
			Text_GET_HHHMMSS(skp, 15 + i * 13, 3, GC->rtcc->VectorCompareDisplayBuffer.data[i].GMT);
			if (GC->rtcc->VectorCompareDisplayBuffer.showHA[i])
			{
				Text(skp, 15 + i * 13, 5, "%.3lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].HA);
			}
			Text(skp, 15 + i * 13, 6, "%.3lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].HP);
			Text(skp, 15 + i * 13, 7, "%.3lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].v);
			Text(skp, 15 + i * 13, 8, "%+.5lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].gamma);
			Text(skp, 15 + i * 13, 9, "%.5lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].psi);
			if (i == 0)
			{
				Text_Latitude(skp, 15 + i * 13, 10, GC->rtcc->VectorCompareDisplayBuffer.data[i].phi, 5);
			}
			else
			{
				Text(skp, 15 + i * 13, 10, "%+.5lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].phi);
			}
			if (i == 0)
			{
				Text_Longitude(skp, 16 + i * 13, 11, GC->rtcc->VectorCompareDisplayBuffer.data[i].lambda, 5);
			}
			else
			{
				Text(skp, 15 + i * 13, 11, "%+.5lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].lambda);
			}
			Text(skp, 15 + i * 13, 12, "%.3lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].h);
			Text(skp, 15 + i * 13, 14, "%.3lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].a);
			Text(skp, 15 + i * 13, 15, "%.5lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].e);
			Text(skp, 15 + i * 13, 16, "%.5lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].i);
			if (GC->rtcc->VectorCompareDisplayBuffer.showWPAndTA[i])
			{
				Text(skp, 15 + i * 13, 17, "%.5lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].theta_p);
				Text(skp, 15 + i * 13, 19, "%.5lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].nu);
			}
			Text(skp, 15 + i * 13, 18, "%.5lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].Omega);
			Text(skp, 15 + i * 13, 21, "%.2lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].U);
			Text(skp, 15 + i * 13, 22, "%.2lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].V);
			Text(skp, 15 + i * 13, 23, "%.2lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].W);
			Text(skp, 15 + i * 13, 24, "%.2lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].U_dot);
			Text(skp, 15 + i * 13, 25, "%.2lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].V_dot);
			Text(skp, 15 + i * 13, 26, "%.2lf", GC->rtcc->VectorCompareDisplayBuffer.data[i].W_dot);
		}
		break;
	case 95:
		SetMOCRFont(skp, 4, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(4);
		Text(skp, 5, 0, "GUIDANCE OPTICS SUPPORT TABLE");
		Text(skp, 37, 0, "0229");
		Text(skp, 2, 1, "CODE");
		TextW(skp, 16, 1, L"\u03B8\u2095");
		TextW(skp, 24, 1, L"\u03C8\u2095");
		TextW(skp, 32, 1, L"\u03C6\u2095");
		Text(skp, 1, 2, "GETAC");
		Text(skp, 19, 2, "IGA");
		Text(skp, 30, 2, "IRA");
		Text(skp, 1, 3, "SXT        SFT    TRN       SFT     TRN");
		Text(skp, 1, 4, "S1");
		Text(skp, 8, 4, "INP");
		Text(skp, 24, 4, "RTCC");
		Text(skp, 1, 5, "S2");
		Text(skp, 8, 5, "INP");
		Text(skp, 24, 5, "RTCC");
		Text(skp, 1, 6, "SCT");
		Text(skp, 0, 7, "S             M     R    RT ASC     DEC");
		Text(skp, 0, 8, "1");
		Text(skp, 4, 8, "SF");
		Text(skp, 0, 9, "2");
		Text(skp, 4, 9, "TR");
		Text(skp, 0, 11, "BORESIGHT SPA    SXP   RT ASC      DEC");
		Text(skp, 1, 12, "S1");
		Text(skp, 1, 13, "S2");
		Text(skp, 0, 14, "LOS  IX");
		Text(skp, 17, 14, "IY");
		Text(skp, 29, 14, "IZ");
		Text(skp, 0, 15, "GET");
		Text(skp, 13, 15, "SC");
		Text(skp, 19, 15, "RA");
		Text(skp, 30, 15, "DEC");
		Text(skp, 17, 16, "P");
		Text(skp, 26, 16, "Y");
		Text(skp, 35, 16, "R");
		Text(skp, 1, 17, "MAT");
		Text(skp, 17, 17, "P");
		Text(skp, 26, 17, "Y");
		Text(skp, 35, 17, "R");
		Text(skp, 0, 18, "X/XE");
		Text(skp, 14, 18, "Y/XE");
		Text(skp, 28, 18, "Z/XE");
		Text(skp, 0, 19, "X/YE");
		Text(skp, 14, 19, "Y/YE");
		Text(skp, 28, 19, "Z/YE");
		Text(skp, 0, 20, "X/ZE");
		Text(skp, 14, 20, "Y/ZE");
		Text(skp, 28, 20, "Z/ZE");
		SetMOCRFont(skp, 4, true);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		Text(skp, 30, 10, GC->rtcc->GOSTDisplayBuffer.err);
		Text(skp, 14, 1, GC->rtcc->GOSTDisplayBuffer.data.CODE);
		Text(skp, 23, 1, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.Att_H.x);
		Text(skp, 31, 1, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.Att_H.y);
		Text(skp, 40, 1, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.Att_H.z);
		Text_GET_HHHMMSS(skp, 16, 2, GC->rtcc->GOSTDisplayBuffer.data.GETAC);
		Text(skp, 28, 2, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.IGA);
		Text(skp, 40, 2, GC->rtcc->GOSTDisplayBuffer.data.IRA);
		Text(skp, 7, 4, "%03d", (int)GC->rtcc->GOSTDisplayBuffer.data.SXT_STAR[0]);
		Text(skp, 16, 4, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SXT_SFT_INP[0]);
		Text(skp, 23, 4, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SXT_TRN_INP[0]);
		Text(skp, 33, 4, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SXT_SFT_RTCC[0]);
		Text(skp, 40, 4, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SXT_TRN_RTCC[0]);
		Text(skp, 7, 5, "%03d", (int)GC->rtcc->GOSTDisplayBuffer.data.SXT_STAR[1]);
		Text(skp, 16, 5, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SXT_SFT_INP[1]);
		Text(skp, 23, 5, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SXT_TRN_INP[1]);
		Text(skp, 33, 5, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SXT_SFT_RTCC[1]);
		Text(skp, 40, 5, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SXT_TRN_RTCC[1]);
		Text(skp, 4, 8, "%03d", (int)GC->rtcc->GOSTDisplayBuffer.data.SCT_S[0]);
		Text(skp, 11, 8, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SCT_SF);
		Text(skp, 17, 8, "%+05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SCT_M[0]);
		Text(skp, 23, 8, "%+05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SCT_R[0]);
		Text_GET_HHMMSS(skp, 32, 8, GC->rtcc->GOSTDisplayBuffer.data.SCT_RTASC[0]);
		FormatDeclination(Buffer, GC->rtcc->GOSTDisplayBuffer.data.SCT_DEC[0]);
		Text(skp, 42, 8, Buffer);
		Text(skp, 4, 9, "%03d", (int)GC->rtcc->GOSTDisplayBuffer.data.SCT_S[1]);
		Text(skp, 11, 9, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SCT_TR);
		Text(skp, 17, 9, "%+05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SCT_M[1]);
		Text(skp, 23, 9, "%+05.1lf", GC->rtcc->GOSTDisplayBuffer.data.SCT_R[1]);
		Text_GET_HHMMSS(skp, 32, 9, GC->rtcc->GOSTDisplayBuffer.data.SCT_RTASC[1]);
		FormatDeclination(Buffer, GC->rtcc->GOSTDisplayBuffer.data.SCT_DEC[1]);
		Text(skp, 42, 9, Buffer);
		Text(skp, 7, 12, "%03d", (int)GC->rtcc->GOSTDisplayBuffer.data.BS_S[0]);
		Text(skp, 14, 12, "%+05.1lf", GC->rtcc->GOSTDisplayBuffer.data.BS_SPA[0]);
		Text(skp, 21, 12, "%+05.1lf", GC->rtcc->GOSTDisplayBuffer.data.BS_SXP[0]);
		Text_GET_HHMMSS(skp, 31, 12, GC->rtcc->GOSTDisplayBuffer.data.BS_RTASC[0]);
		FormatDeclination(Buffer, GC->rtcc->GOSTDisplayBuffer.data.BS_DEC[0]);
		Text(skp, 41, 12, Buffer);
		Text(skp, 7, 13, "%03d", (int)GC->rtcc->GOSTDisplayBuffer.data.BS_S[1]);
		Text(skp, 14, 13, "%+05.1lf", GC->rtcc->GOSTDisplayBuffer.data.BS_SPA[1]);
		Text(skp, 21, 13, "%+05.1lf", GC->rtcc->GOSTDisplayBuffer.data.BS_SXP[1]);
		Text_GET_HHMMSS(skp, 31, 13, GC->rtcc->GOSTDisplayBuffer.data.BS_RTASC[1]);
		FormatDeclination(Buffer, GC->rtcc->GOSTDisplayBuffer.data.BS_DEC[1]);
		Text(skp, 41, 13, Buffer);
		Text(skp, 15, 14, "%+.5lf", GC->rtcc->GOSTDisplayBuffer.data.Landmark_LOS.x);
		Text(skp, 27, 14, "%+.5lf", GC->rtcc->GOSTDisplayBuffer.data.Landmark_LOS.y);
		Text(skp, 39, 14, "%+.5lf", GC->rtcc->GOSTDisplayBuffer.data.Landmark_LOS.z);
		Text_GET_HHHMMSS(skp, 12, 15, GC->rtcc->GOSTDisplayBuffer.data.Landmark_GET);
		Text(skp, 18, 15, GC->rtcc->GOSTDisplayBuffer.data.Landmark_SC);
		Text_GET_HHMMSS(skp, 29, 15, GC->rtcc->GOSTDisplayBuffer.data.Landmark_RA);
		FormatDeclination(Buffer, GC->rtcc->GOSTDisplayBuffer.data.Landmark_DEC);
		Text(skp, 42, 15, Buffer);
		Text(skp, 24, 16, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.Att[0].x);
		Text(skp, 33, 16, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.Att[0].y);
		Text(skp, 42, 16, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.Att[0].z);
		Text(skp, 11, 17, GC->rtcc->GOSTDisplayBuffer.data.MAT);
		Text(skp, 24, 17, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.Att[1].x);
		Text(skp, 33, 17, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.Att[1].y);
		Text(skp, 42, 17, "%05.1lf", GC->rtcc->GOSTDisplayBuffer.data.Att[1].z);
		Text(skp, 13, 18, "%+.6lf", GC->rtcc->GOSTDisplayBuffer.data.REFSMMAT.m11);
		Text(skp, 27, 18, "%+.6lf", GC->rtcc->GOSTDisplayBuffer.data.REFSMMAT.m21);
		Text(skp, 42, 18, "%+.6lf", GC->rtcc->GOSTDisplayBuffer.data.REFSMMAT.m31);
		Text(skp, 13, 19, "%+.6lf", GC->rtcc->GOSTDisplayBuffer.data.REFSMMAT.m12);
		Text(skp, 27, 19, "%+.6lf", GC->rtcc->GOSTDisplayBuffer.data.REFSMMAT.m22);
		Text(skp, 42, 19, "%+.6lf", GC->rtcc->GOSTDisplayBuffer.data.REFSMMAT.m32);
		Text(skp, 13, 20, "%+.6lf", GC->rtcc->GOSTDisplayBuffer.data.REFSMMAT.m13);
		Text(skp, 27, 20, "%+.6lf", GC->rtcc->GOSTDisplayBuffer.data.REFSMMAT.m23);
		Text(skp, 42, 20, "%+.6lf", GC->rtcc->GOSTDisplayBuffer.data.REFSMMAT.m33);
		break;
	case 96:
		SetMOCRFont(skp, 4, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(4);
		Text(skp, 11, 0, "SLV NAVIGATION UPDATE");
		Text(skp, 37, 0, "0235");
		Text(skp, 1, 2, "LOAD NO");
		Text(skp, 14, 2, "GETGEN");
		Text(skp, 31, 2, "SITES");
		Text(skp, 1, 3, "STA ID");
		Text(skp, 27, 3, "GETSV");
		Text(skp, 1, 4, "GRR/S");
		Text(skp, 30, 4, "AZI");
		Text(skp, 1, 6, "FCT");
		Text(skp, 15, 6, "ENGLISH");
		Text(skp, 34, 6, "METRIC");
		Text(skp, 1, 8, "Z DOT");
		Text(skp, 1, 9, "X DOT");
		Text(skp, 1, 10, "Y DOT");
		Text(skp, 1, 11, "Z POS");
		Text(skp, 1, 12, "X POS");
		Text(skp, 1, 13, "Y POS");
		Text(skp, 1, 14, "TIME");
		PrintIUVessel(Buffer);
		skp->Text(CW, CH * 20, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 4, true);
		Text(skp, 13, 2, "%d", GC->rtcc->CZNAVSLV.SequenceNumber);
		Text_GET_HHHMMSS(skp, 30, 2, GC->rtcc->CZNAVSLV.GETLoadGeneration);
		Text(skp, 15, 3, GC->rtcc->CZNAVSLV.STAID);
		G->SVDesiredGET < 0 ? Text(skp, 42, 3, "PRESENT") : Text_GET_HHHMMSS(skp, 42, 3, G->SVDesiredGET);
		Text_GET_HHHMMSSCS(skp, 19, 4, GC->rtcc->GetIUClockZero());
		Text(skp, 42, 4, "%+.3lf", GC->rtcc->GetIULaunchAzimuth()*DEG);
		Text(skp, 24, 8, "%+.1lf", GC->rtcc->CZNAVSLV.DotS.z / 0.3048);
		Text(skp, 42, 8, "%+.1lf", GC->rtcc->CZNAVSLV.DotS.z);
		Text(skp, 24, 9, "%+.1lf", GC->rtcc->CZNAVSLV.DotS.x / 0.3048);
		Text(skp, 42, 9, "%+.1lf", GC->rtcc->CZNAVSLV.DotS.x);
		Text(skp, 24, 10, "%+.1lf", GC->rtcc->CZNAVSLV.DotS.y / 0.3048);
		Text(skp, 42, 10, "%+.1lf", GC->rtcc->CZNAVSLV.DotS.y);
		Text(skp, 24, 11, "%+.1lf", GC->rtcc->CZNAVSLV.PosS.z / 1852.0);
		Text(skp, 42, 11, "%+.1lf", GC->rtcc->CZNAVSLV.PosS.z / 1000.0);
		Text(skp, 24, 12, "%+.1lf", GC->rtcc->CZNAVSLV.PosS.x / 1852.0);
		Text(skp, 42, 12, "%+.1lf", GC->rtcc->CZNAVSLV.PosS.x / 1000.0);
		Text(skp, 24, 13, "%+.1lf", GC->rtcc->CZNAVSLV.PosS.y / 1852.0);
		Text(skp, 42, 13, "%+.1lf", GC->rtcc->CZNAVSLV.PosS.y / 1000.0);
		Text_GET_HHHMMSSCS(skp, 24, 14, GC->rtcc->CZNAVSLV.NUPTIM);
		Text(skp, 42, 14, "%+.4lf", GC->rtcc->CZNAVSLV.NUPTIM / 3600.0);
		switch (G->iuUplinkResult)
		{
		case 1:
			sprintf(Buffer, "Uplink accepted!");
			break;
		case 2:
			sprintf(Buffer, "No vessel or IU!");
			break;
		case 3:
			sprintf(Buffer, "Uplink rejected!");
			break;
		case 4:
			sprintf(Buffer, "No state vector!");
			break;
		default:
			sprintf(Buffer, "No Uplink");
			break;
		}
		skp->Text(W - CW, CH * 20, Buffer, strlen(Buffer));
		break;
	case 97:
		G->CycleVectorPanelSummary();
		skp->SetPen(pen2);
		SetMOCRFont(skp, 2, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(2);
		Text(skp, 23, 0, "VECTOR PANEL SUMMARY");
		Text(skp, 60, 0, "1591");
		Text(skp, 1, 2, "CSM ANCHOR");
		Text(skp, 27, 2, "CURRENT GMT");
		Text(skp, 42, 2, "LM ANCHOR");
		Text(skp, 4, 3, "GMTAV"); Text(skp, 44, 3, "GMTAV");
		Text(skp, 11, 5, "CSM VECTORS"); Text(skp, 42, 5, "LM VECTORS");
		Text(skp, 0, 7, "CMC     DT");
		Text(skp, 2, 8, "UV"); Text(skp, 2, 10, "EV"); Text(skp, 2, 12, "TH"); Text(skp, 2, 13, "TL");
		Text(skp, 0, 15, "LGC     DT");
		Text(skp, 2, 16, "UV"); Text(skp, 2, 18, "EV"); Text(skp, 2, 20, "TH"); Text(skp, 2, 21, "TL");
		Text(skp, 0, 23, "AGS     DT");
		Text(skp, 2, 24, "UV"); Text(skp, 2, 26, "EV"); Text(skp, 2, 28, "TH"); Text(skp, 2, 29, "TL");
		Text(skp, 16, 7, "IU      DT");
		Text(skp, 18, 8, "UV"); Text(skp, 18, 10, "EV"); Text(skp, 18, 12, "TH"); Text(skp, 18, 13, "TL");
		Text(skp, 16, 15, "HIGH SPEED RADAR");
		Text(skp, 18, 16, "DT"); Text(skp, 18, 17, "UV");
		Text(skp, 17, 20, "DC VECTORS");
		Text(skp, 18, 21, "DT"); Text(skp, 18, 22, "UV");
		Text(skp, 17, 25, "LAST EXECUTED"); Text(skp, 19, 26, "MANEUVER");
		Text(skp, 16, 27, "GMTUL"); Text(skp, 16, 28, "GMTTO");
		Text(skp, 33, 7, "CMC     DT");
		Text(skp, 34, 8, "UV"); Text(skp, 34, 10, "EV"); Text(skp, 34, 12, "TH"); Text(skp, 34, 13, "TL");
		Text(skp, 33, 15, "LGC     DT");
		Text(skp, 34, 16, "UV"); Text(skp, 34, 18, "EV"); Text(skp, 34, 20, "TH"); Text(skp, 34, 21, "TL");
		Text(skp, 33, 23, "AGS     DT");
		Text(skp, 34, 24, "UV"); Text(skp, 34, 26, "EV"); Text(skp, 34, 28, "TH"); Text(skp, 34, 29, "TL");
		Text(skp, 48, 7, "IU      DT");
		Text(skp, 50, 8, "UV"); Text(skp, 50, 10, "EV"); Text(skp, 50, 12, "TH"); Text(skp, 50, 13, "TL");
		Text(skp, 48, 15, "HIGH SPEED RADAR");
		Text(skp, 50, 16, "DT"); Text(skp, 50, 17, "UV");
		Text(skp, 49, 20, "DC VECTORS");
		Text(skp, 50, 21, "DT"); Text(skp, 50, 22, "UV");
		Text(skp, 49, 25, "LAST EXECUTED"); Text(skp, 51, 26, "MANEUVER");
		Text(skp, 48, 27, "GMTUL"); Text(skp, 48, 28, "GMTTO");
		Line(skp, 0, (CH * 9) / 2, CW * 64, (CH * 9) / 2);
		Line(skp, 0, (CH * 13) / 2, CW * 64, (CH * 13) / 2);
		Line(skp, 0, (CH * 29) / 2, CW * 64, (CH * 29) / 2);
		Line(skp, 0, (CH * 45) / 2, (CW * 31) / 2, (CH * 45) / 2);
		Line(skp, (CW * 31) / 2, (CH * 39) / 2, (CW * 65) / 2, (CH * 39) / 2);
		Line(skp, (CW * 31) / 2, (CH * 49) / 2, (CW * 65) / 2, (CH * 49) / 2);
		Line(skp, (CW * 65) / 2, (CH * 45) / 2, (CW * 95) / 2, (CH * 45) / 2);
		Line(skp, (CW * 95) / 2, (CH * 39) / 2, CW * 64, (CH * 39) / 2);
		Line(skp, (CW * 95) / 2, (CH * 49) / 2, CW * 64, (CH * 49) / 2);
		Line(skp, (CW * 31) / 2, (CH * 13) / 2, (CW * 31) / 2, CH * 32);
		Line(skp, (CW * 65) / 2, (CH * 13) / 2, (CW * 65) / 2, CH * 32);
		Line(skp, (CW * 95) / 2, (CH * 13) / 2, (CW * 95) / 2, CH * 32);
		SetMOCRFont(skp, 2, true);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		Text(skp, 19, 2, GC->rtcc->VectorPanelSummaryBuffer.AnchorVectorID[0]);
		Text(skp, 59, 2, GC->rtcc->VectorPanelSummaryBuffer.AnchorVectorID[1]);
		Text(skp, 19, 3, GC->rtcc->VectorPanelSummaryBuffer.AnchorVectorGMT[0]);
		Text(skp, 37, 3, GC->rtcc->VectorPanelSummaryBuffer.CurrentGMT);
		Text(skp, 59, 3, GC->rtcc->VectorPanelSummaryBuffer.AnchorVectorGMT[1]);
		x = 14; y = 8;
		for (int i = 0; i < 2; i++)
		{
			//CMC, LGC and AGS
			for (int j = 0; j < 3; j++)
			{
				Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.CompUsableID[i][j]); y++;
				Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.CompUsableGMT[i][j]); y++;
				Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.CompEvalID[i][j]); y++;
				Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.CompEvalGMT[i][j]); y++;
				Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.CompTelemetryHighGMT[i][j]); y++;
				Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.CompTelemetryLowGMT[i][j]); y += 3;
			}
			x = 46; y = 8;
		}
		x = 30; y = 8;
		for (int i = 0; i < 2; i++)
		{
			//IU
			Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.CompUsableID[i][3]); y++;
			Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.CompUsableGMT[i][3]); y++;
			Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.CompEvalID[i][3]); y++;
			Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.CompEvalGMT[i][3]); y++;
			Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.CompTelemetryHighGMT[i][3]); y++;
			Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.CompTelemetryLowGMT[i][3]); y += 4;
			//HSR
			Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.HSRID[i]); y++;
			Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.HSRGMT[i]); y += 4;
			//DC
			Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.DCID[i]); y++;
			Text(skp, x, y, GC->rtcc->VectorPanelSummaryBuffer.DCGMT[i]); y += 4;
			//Last Executed Maneuver
			Text(skp, x + 1, y, GC->rtcc->VectorPanelSummaryBuffer.LastManGMTUL[i]); y++;
			Text(skp, x + 1, y, GC->rtcc->VectorPanelSummaryBuffer.LastManGMTBO[i]);
			x = 62; y = 8;
		}
		break;
	case 99:
		//Ascent Rendezvous Monitor (Coelliptic)
		if (EnableCalculation)
		{
			skp->Text(CW, 2 * H / 14, "*", 1);

			if (GC->rtcc->RTCCPresentTimeGMT() > GC->rtcc->PZMARM.t_Calc_ARM + 2.0)
			{
				G->startSubthread(58);
			}
		}

		DFLBackgroundSlide(skp, 232, 4);
		DFLDynamicData(skp, 232, 4);
		break;
	case 100:
		//Ascent Rendezvous Monitor (Short)
		if (EnableCalculation)
		{
			skp->Text(CW, 2 * H / 14, "*", 1);

			if (GC->rtcc->RTCCPresentTimeGMT() > GC->rtcc->PZMARM.t_Calc_ShortARM + 2.0)
			{
				G->startSubthread(59);
			}
		}

		DFLBackgroundSlide(skp, 233, 4);
		DFLDynamicData(skp, 233, 4);
		break;
	case 101:
		if (subscreen == 0)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text(W / 2, CH / 2, "Groundtrack Digitals Input", 26);
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			x = 1;  y = 3; dx = 7;
			Text(skp, x, marker + y, "*");
			x++;
			Text(skp, x, y, "VEH:");
			if (GC->rtcc->EZETVMED.GrndTrkDigitalsVehID == 1) Text(skp, x + dx, y, "CSM");
			else Text(skp, x + dx, y, "LEM");
			y++;
			Text(skp, x, y, "OPT:");
			if (GC->rtcc->EZETVMED.GrndTrkDigitalsOption == 1) Text(skp, x + dx, y, "Rev");
			else Text(skp, x + dx, y, "Time");
			y++;
			if (GC->rtcc->EZETVMED.GrndTrkDigitalsOption == 2)
			{
				Text(skp, x, y, "TIME:");
				Text_GET_HHHMMSS(skp, x + dx, y, GC->rtcc->EZETVMED.GrndTrkDigitalsTime);
			}
			y++;
			Text(skp, x, y, "LNG:");
			Text(skp, x + dx, y, "%.2lf", GC->rtcc->EZETVMED.GrndTrkDigitalsLongitude*DEG);
			y++;
			if (GC->rtcc->EZETVMED.GrndTrkDigitalsOption == 1)
			{
				Text(skp, x, y, "REV:");
				Text(skp, x + dx, y, "%d", GC->rtcc->EZETVMED.GrndTrkDigitalsRev);
			}
			y++;
			Text(skp, x, y, "REF:");
			if (GC->rtcc->EZETVMED.GrndTrkDigitalsCoordinates == RTCC_COORDINATES_ECT) Text(skp, x + dx, y, "Earth");
			else Text(skp, x + dx, y, "Moon");
		}
		else
		{
			SetMOCRFont(skp, 3, false);
			GetCharSize(skp, CW, CH);
			SetMOCRDisplayCentered(3);
			Text(skp, 16, 0, "GROUNDTRACK DIGITALS");
			Text(skp, 52, 0, "0347");
			Text(skp, 2, 2, "VEH");
			Text(skp, 11, 2, "STA ID");
			Text(skp, 27, 2, "LNG");
			Text(skp, 40, 2, "REF");
			Text(skp, 50, 2, "/");
			Text(skp, 0, 4, " REV     GET        GMT      LAT      LNG      TAA");
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			SetMOCRFont(skp, 3, true);
			Text(skp, 9, 2, GC->rtcc->RZDGTD.VehicleName);
			Text(skp, 25, 2, GC->rtcc->RZDGTD.StationID);
			Text_Longitude(skp, 38, 2, GC->rtcc->RZDGTD.InputLongitude);
			Text(skp, 47, 2, GC->rtcc->RZDGTD.REF);
			Text(skp, 50, 2, "%d", GC->rtcc->RZDGTD.CurrentPage);
			Text(skp, 52, 2, "%d", GC->rtcc->RZDGTD.TotalNumPages);
			{
				int j = (GC->rtcc->RZDGTD.CurrentPage - 1) * 20;
				for (int i = 0; i < 20; i++)
				{
					if (GC->rtcc->RZDGTD.table[i + j].DataIndicator) break;
					Text(skp, 4, 6 + i, "%d", GC->rtcc->RZDGTD.table[i + j].Rev);
					Text_GET_HHHMMSS(skp, 15, 6 + i, GC->rtcc->RZDGTD.table[i + j].GET);
					Text_GET_HHHMMSS(skp, 26, 6 + i, GC->rtcc->RZDGTD.table[i + j].GMT);
					Text_Latitude(skp, 34, 6 + i, GC->rtcc->RZDGTD.table[i + j].Latitude);
					Text_Longitude(skp, 43, 6 + i, GC->rtcc->RZDGTD.table[i + j].Longitude);
					Text(skp, 51, 6 + i, "%.2lf", GC->rtcc->RZDGTD.table[i + j].TrueAnomaly);
				}
			}
			Text(skp, 50, 27, GC->rtcc->RZDGTD.ErrorMessage);
		}
		break;
	case 103:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Retrofire Constraints", 21);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		ThrusterName(Buffer, GC->rtcc->RZJCTTC.R31_Thruster);
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->RZJCTTC.R31_BurnMode == 1)
		{
			skp->Text(CW, 4 * H / 14, "DV", 2);
			Text_Double(skp, CW, 6 * H / 14, "DV = %.1lf", GC->rtcc->RZJCTTC.R31_dv / 0.3048);
		}
		else if (GC->rtcc->RZJCTTC.R31_BurnMode == 2)
		{
			skp->Text(CW, 4 * H / 14, "DT", 2);
			Text_Double(skp, CW, 6 * H / 14, "DT = %.1lf", GC->rtcc->RZJCTTC.R31_dt);
		}
		else
		{
			skp->Text(CW, 4 * H / 14, "V, Gamma", 8);
		}
		if (GC->rtcc->RZJCTTC.R31_AttitudeMode == 1)
		{
			skp->Text(CW, 8 * H / 14, "LVLH", 4);
			sprintf_s(Buffer, "%+.1lf %+.1lf %+.1lf", GC->rtcc->RZJCTTC.R31_LVLHAttitude.x*DEG, GC->rtcc->RZJCTTC.R31_LVLHAttitude.y*DEG, GC->rtcc->RZJCTTC.R31_LVLHAttitude.z*DEG);
			skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			skp->Text(CW, 8 * H / 14, "31.7° window line", 17);
		}
		PrintUllage(Buffer, GC->rtcc->RZJCTTC.R31_Thruster, GC->rtcc->RZJCTTC.R31_Use4UllageThrusters, GC->rtcc->RZJCTTC.R31_UllageTime);
		skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (GC->rtcc->RZJCTTC.R31_REFSMMAT < 9)
		{
			GC->rtcc->EMGSTGENName(GC->rtcc->RZJCTTC.R31_REFSMMAT, Buffer);
		}
		else
		{
			sprintf(Buffer, "DES");
		}
		skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->RZJCTTC.R31_GimbalIndicator == 1)
		{
			skp->Text(W - CW, 4 * H / 14, "Use System Parameters", 21);
		}
		else
		{
			skp->Text(W - CW, 4 * H / 14, "Compute Gimbal Trims", 20);
		}
		Text_Double(skp, W - CW, 6 * H / 14, "%+.1lf°", GC->rtcc->RZJCTTC.R31_InitialBankAngle*DEG);
		Text_Double(skp, W - CW, 8 * H / 14, "%.1lf gs", GC->rtcc->RZJCTTC.R31_GLevel);
		Text_Double(skp, W - CW, 10 * H / 14, "%+.1lf°", GC->rtcc->RZJCTTC.R31_FinalBankAngle*DEG);
		break;
	case 104:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 15, 0, "RETROFIRE DIGITALS");
		Text(skp, 52, 0, "0356");
		Text(skp, 1, 3, "AREA");
		Text(skp, 1, 4, "MATRIX");
		Text(skp, 1, 5, "WT TAA");
		Text(skp, 1, 6, "RLH PLH YLH");
		Text(skp, 1, 7, "RO PI YM");
		Text(skp, 1, 8, "VC BT");
		Text(skp, 1, 9, "VT U DT");
		Text(skp, 1, 10, "H");
		Text(skp, 1, 11, "GETI");
		Text(skp, 1, 12, "GMTI");
		Text(skp, 1, 13, "RET");
		Text(skp, 1, 14, "V EI GAM EI");
		Text(skp, 1, 15, "BA");
		Text(skp, 1, 16, "RETRB");
		Text(skp, 1, 17, "LAT ML LNG ML");
		Text(skp, 1, 18, "LAT T  LNG T");
		Text(skp, 1, 19, "LAT IP LNG IP");
		Text(skp, 1, 20, "LAT ZL LNG ZL");
		Text(skp, 1, 21, "DLAT   DLNG");
		Line(skp, CW * 22, CH * 3, CW * 22, CH * 23);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		{
			RetrofireDisplayParametersTableData *tab = &GC->rtcc->RZRFDP.data[2];
			if (tab->Indicator == 0)
			{
				Text(skp, 49, 4, tab->RefsID);
				Text(skp, 44, 5, "%.1lf", tab->CSMWeightRetro);
				Text(skp, 49, 5, "%.0lf", tab->TrueAnomalyRetro);
				sprintf_s(Buffer, "%.1lf %.1lf %.1lf", tab->Att_LVLH.x, tab->Att_LVLH.y, tab->Att_LVLH.z);
				Text(skp, 49, 6, Buffer);
				sprintf_s(Buffer, "%.1lf %.1lf %.1lf", tab->Att_IMU.x, tab->Att_IMU.y, tab->Att_IMU.z);
				Text(skp, 49, 7, Buffer);
				Text(skp, 40, 8, "%.1lf", tab->DVC);
				OrbMech::SStoMMSS(tab->BurnTime, mm, secs, 0.1);
				sprintf_s(Buffer, "%02d:%04.1lf", mm, secs);
				Text(skp, 49, 8, Buffer);
				Text(skp, 40, 9, "%.1lf", tab->DVT);
				Text(skp, 43, 9, "%+d", tab->UllageQuads);
				Text(skp, 49, 9, "%.1lf", tab->UllageDT);
				Text(skp, 49, 10, "%.1lf", tab->H_Retro);
				Text_GET_HHHMMSSCS(skp, 49, 11, tab->GETI);
				Text_GET_HHHMMSS(skp, 49, 12, tab->GMTI);
				OrbMech::SStoMMSS(tab->RET400k, mm, secs);
				sprintf_s(Buffer, "%d:%02.0lf", mm, secs);
				Text(skp, 49, 13, Buffer);
				Text(skp, 42, 14, "%.0lf", tab->V400k);
				Text(skp, 49, 14, "%.2lf", tab->Gamma400k);
				Text(skp, 49, 15, "%.1lf°", tab->BankAngle);
				OrbMech::SStoMMSS(tab->RETRB, mm, secs);
				sprintf_s(Buffer, "%d:%02.0lf", mm, secs);
				Text(skp, 49, 16, Buffer);
				Text_Latitude(skp, 40, 17, tab->lat_ML);
				Text_Longitude(skp, 49, 17, tab->lng_ML);
				Text_Latitude(skp, 40, 18, tab->lat_T);
				Text_Longitude(skp, 49, 18, tab->lng_T);
				Text_Latitude(skp, 40, 19, tab->lat_IP);
				Text_Longitude(skp, 49, 19, tab->lng_IP);
				Text_Latitude(skp, 40, 20, tab->lat_ZL);
				Text_Longitude(skp, 49, 20, tab->lng_ZL);
				sprintf_s(Buffer, "%.2lf %.2lf NM", tab->dlat_NM, tab->dlng_NM);
				Text(skp, 49, 21, Buffer);
			}
			else if (tab->Indicator == 1)
			{
				Text(skp, 49, 11, "NO DATA");
			}
			else
			{
				Text(skp, 49, 11, "ERROR - REFER TO ONLINE");
			}
		}
		break;
	case 105:
		SetMOCRFont(skp, 4, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(4);
		Text(skp, 10, 0, "RETROFIRE EXTERNAL DV");
		Text(skp, 38, 0, "0329");
		Text(skp, 16, 2, "PRIMARY");
		Text(skp, 33, 2, "MANUAL");
		Text(skp, 1, 4, "GETI");
		Text(skp, 1, 5, "VGX XDV");
		Text(skp, 1, 6, "VGY XDV");
		Text(skp, 1, 7, "VGZ XDV");
		Text(skp, 1, 8, "DEL P");
		Text(skp, 1, 9, "DEL Y");
		Text(skp, 1, 10, "DT TO");
		Text(skp, 1, 11, "DV TO");
		Text(skp, 1, 12, "HA");
		Text(skp, 1, 13, "HP");
		Text(skp, 1, 14, "LAT IP");
		Text(skp, 1, 15, "LNG IP");
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 4, true);
		if (GC->rtcc->RetrofireEXDV.data.size() > 0)
		{
			for (unsigned i = 0; i < 2; i++)
			{
				RTCC::RetrofireExternalDVDisplayData *tab = &GC->rtcc->RetrofireEXDV.data[i];
				if (tab->Indicator)
				{
					Text_GET_HHHMMSSCS(skp, 25 + i * 18, 4, tab->GETI);
					AGC_Display(Buffer, tab->DV.x);
					Text(skp, 25 + i * 18, 5, Buffer);
					AGC_Display(Buffer, tab->DV.y);
					Text(skp, 25 + i * 18, 6, Buffer);
					AGC_Display(Buffer, tab->DV.z);
					Text(skp, 25 + i * 18, 7, Buffer);
					Text(skp, 25 + i * 18, 8, "%+.2lf", tab->P_G);
					Text(skp, 25 + i * 18, 9, "%+.2lf", tab->Y_G);
					Text(skp, 25 + i * 18, 10, "%.2lf", tab->DT_TO);
					Text(skp, 25 + i * 18, 11, "%.2lf", tab->DV_TO);
					if (tab->H_apo < 99999.9) Text(skp, 25 + i * 18, 12, "%.1lf", tab->H_apo);
					Text(skp, 25 + i * 18, 13, "%.1lf", tab->H_peri);
					Text_Latitude(skp, 25 + i * 18, 14, tab->lat_IP);
					Text_Longitude(skp, 25 + i * 18, 15, tab->lng_IP);
				}
				else
				{
					Text(skp, 25 + i * 18, 7, "NO DATA");
				}
			}
		}
		break;
	case 106:
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 10, 0, "RECOVERY TARGET SELECTION");
		Text(skp, 52, 0, "1454");
		Text(skp, 52, 2, "/");
		Text(skp, 0, 4, "     REV     GET        GMT      LAT      LNG     AZI");
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text(skp, 52, 2, "%d", GC->rtcc->RZDRTSD.CurrentPage);
		Text(skp, 54, 2, "%d", GC->rtcc->RZDRTSD.TotalNumPages);
		{
			int j = (GC->rtcc->RZDRTSD.CurrentPage - 1) * 10;
			for (int i = 0; i < 10; i++)
			{
				if (GC->rtcc->RZDRTSD.table[i + j].DataIndicator) break;
				Text(skp, 3, 6 + i, "%d", i + j + 1);
				Text(skp, 8, 6 + i, "%d", GC->rtcc->RZDRTSD.table[i + j].Rev);
				Text_GET_HHHMMSS(skp, 19, 6 + i, GC->rtcc->RZDRTSD.table[i + j].GET);
				Text_GET_HHHMMSS(skp, 30, 6 + i, GC->rtcc->RZDRTSD.table[i + j].GMT);
				Text_Latitude(skp, 38, 6 + i, GC->rtcc->RZDRTSD.table[i + j].Latitude);
				Text_Longitude(skp, 47, 6 + i, GC->rtcc->RZDRTSD.table[i + j].Longitude);
				Text(skp, 55, 6 + i, "%.2lf", GC->rtcc->RZDRTSD.table[i + j].Azimuth);
			}
		}
		break;
	case 107:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Abort Scan Table Inputs", 23);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (G->RTEASTType == 75)
		{
			skp->Text(CW, 2 * H / 14, "Unspecified Area", 21);
		}
		else if (G->RTEASTType == 76)
		{
			skp->Text(CW, 2 * H / 14, "Specific Site", 13);
		}
		else
		{
			skp->Text(CW, 2 * H / 14, "Lunar Search", 12);
		}
		//4: Type TCUA, FCUA or Site
		//6: Vector time (all)
		//8: Abort time (F75 and F76), Min abort time (F77)
		//10: Delta V (F75), Max abort time (F77)
		//12: Landing time (F76 and F77)
		//4: Entry Profile
		//6: Miss Distance (F76 and F77, PTP only)
		//8: Inclination (F77)
		if (GC->MissionPlanningActive)
		{
			GET_Display(Buffer, GC->rtcc->med_f75_f77.T_V, false);
		}
		else
		{
			PrintCSMVessel(Buffer);
		}
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		if (G->RTEASTType == 77)
		{
			skp->Text(CW, 8 * H / 14, "MIN:", 4);
		}
		else
		{
			skp->Text(CW, 8 * H / 14, "T0:", 3);
		}
		GET_Display(Buffer, GC->rtcc->med_f75_f77.T_0_min, false);
		skp->Text(CW * 6, 8 * H / 14, Buffer, strlen(Buffer));
		if (G->RTEASTType == 75)
		{
			sprintf_s(Buffer, "%s", GC->rtcc->med_f75.Type.c_str());
			skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
			Text_Double(skp, CW, 10 * H / 14, "DVMAX: %.0lf ft/s", GC->rtcc->med_f75.DVMAX);
		}
		else if (G->RTEASTType == 76)
		{
			sprintf_s(Buffer, "%s", GC->rtcc->med_f76.Site.c_str());
			skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
			GET_Display(Buffer, GC->rtcc->med_f75_f77.T_Z, false);
			skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
		}
		else
		{
			sprintf_s(Buffer, "%s", GC->rtcc->med_f77.Site.c_str());
			skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
			skp->Text(CW, 10 * H / 14, "MAX:", 4);
			GET_Display(Buffer, GC->rtcc->med_f77.T_max, false);
			skp->Text(CW * 6, 10 * H / 14, Buffer, strlen(Buffer));
			if (GC->rtcc->med_f77.Site != "FCUA")
			{
				GET_Display(Buffer, GC->rtcc->med_f75_f77.T_Z, false);
				skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
			}
		}

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		sprintf_s(Buffer, "%s", GC->rtcc->med_f75_f77.EntryProfile.c_str());
		skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
		sprintf_s(Buffer, "%.2lf°", GC->rtcc->med_f75_f77.Inclination*DEG);
		skp->Text(W - CW, 8 * H / 14, Buffer, strlen(Buffer));
		break;
	case 108:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 20, 0, "ABORT SCAN TABLE");
		Text(skp, 52, 0, "0362");
		Text(skp, 0, 3, "CODE");
		Text(skp, 0, 4, "STA");
		Text(skp, 0, 5, "ID");
		Text(skp, 5, 3, "SITE");
		Text(skp, 6, 4, "AM");
		Text(skp, 13, 3, "GETI");
		Text(skp, 13, 4, "GETV");
		Text(skp, 22, 3, "DV");
		Text(skp, 22, 4, "INCL");
		Text(skp, 22, 5, "HPC");
		Text(skp, 29, 3, "VEI");
		Text(skp, 29, 4, "GEI");
		Text(skp, 36, 3, "GETEI");
		Text(skp, 36, 4, "GETL");
		Text(skp, 44, 3, "LAT IP");
		Text(skp, 44, 4, "LNG IP");
		Line(skp, 0, (CH * 5) / 2, CW * 56, (CH * 5) / 2);
		Line(skp, 0, (CH * 13) / 2, CW * 56, (CH * 13) / 2);
		Line(skp, (CW * 9) / 2, (CH * 5) / 2, (CW * 9) / 2, CH * 28);
		Line(skp, (CW * 21) / 2, (CH * 5) / 2, (CW * 21) / 2, CH * 28);
		Line(skp, (CW * 41) / 2, (CH * 5) / 2, (CW * 41) / 2, CH * 28);
		Line(skp, (CW * 55) / 2, (CH * 5) / 2, (CW * 55) / 2, CH * 28);
		Line(skp, (CW * 67) / 2, (CH * 5) / 2, (CW * 67) / 2, CH * 28);
		Line(skp, (CW * 87) / 2, (CH * 5) / 2, (CW * 87) / 2, CH * 28);
		Line(skp, (CW * 103) / 2, (CH * 5) / 2, (CW * 103) / 2, CH * 28);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		{
			ASTData *tab;
			for (int i = 0; i < 7; i++)
			{
				tab = &GC->rtcc->PZREAP.AbortScanTableData[i];
				if (tab->ASTCode == 0) continue;
				Text(skp, 4, 7 + 3 * i, "%d", tab->ASTCode);
				Text(skp, 10, 7 + 3 * i, tab->SiteID);
				Text(skp, 10, 8 + 3 * i, tab->AbortMode);
				Text_GET_HHHMMSS(skp, 20, 7 + 3 * i, GC->rtcc->GETfromGMT(tab->AbortGMT));
				Text_GET_HHHMMSS(skp, 20, 8 + 3 * i, GC->rtcc->GETfromGMT(tab->VectorGMT));
				if (tab->dv >= 304.8)
				{
					sprintf_s(Buffer, "%.0lf", tab->dv / 0.3048);
				}
				else if (tab->dv >= 30.48)
				{
					sprintf_s(Buffer, "%.1lf", tab->dv / 0.3048);
				}
				else
				{
					sprintf_s(Buffer, "%.2lf", tab->dv / 0.3048);
				}
				Text(skp, 27, 7 + 3 * i, Buffer);
				if (tab->incl_EI < 0)
				{
					sprintf(Buffer, "A%.2f", abs(tab->incl_EI*DEG));
				}
				else
				{
					sprintf(Buffer, "D%.2f", tab->incl_EI*DEG);
				}
				Text(skp, 27, 8 + 3 * i, Buffer);
				Text(skp, 27, 9 + 3 * i, "%.1lf", tab->h_PC / 1852.0);
				Text(skp, 33, 7 + 3 * i, "%.0lf", tab->v_EI / 0.3048);
				Text(skp, 33, 8 + 3 * i, "%.2lf", tab->gamma_EI*DEG);
				Text_GET_HHHMMSS(skp, 43, 7 + 3 * i, GC->rtcc->GETfromGMT(tab->ReentryGMT));
				Text_GET_HHHMMSS(skp, 43, 8 + 3 * i, GC->rtcc->GETfromGMT(tab->SplashdownGMT));
				Text_Latitude(skp, 51, 7 + 3 * i, tab->lat_SPL*DEG);
				Text_Longitude(skp, 51, 8 + 3 * i, tab->lng_SPL*DEG);
			}
		}
		break;
	case 109:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "RTE Digitals Manual Maneuver Input", 26);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (GC->MissionPlanningActive)
		{
			GET_Display(Buffer, GC->rtcc->med_f81.VectorTime, false);
			skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		}
		GET_Display(Buffer, GC->rtcc->med_f81.IgnitionTime, false);
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		Text_Double(skp, CW, 6 * H / 14, "%.2lf°", GC->rtcc->med_f81.lat_tgt*DEG);
		Text_Double(skp, CW, 8 * H / 14, "%.2lf°", GC->rtcc->med_f81.lng_tgt*DEG);
		if (GC->rtcc->med_f81.RefBody == BODY_EARTH)
		{
			skp->Text(CW, 10 * H / 14, "EARTH", 5);
		}
		else
		{
			skp->Text(CW, 10 * H / 14, "MOON", 4);
		}
		sprintf_s(Buffer, "%+.1lf %+.1lf %+.1lf", GC->rtcc->med_f81.XDV.x / 0.3048, GC->rtcc->med_f81.XDV.y / 0.3048, GC->rtcc->med_f81.XDV.z / 0.3048);
		skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		skp->Text(W - CW, 10 * H / 14, "Load splashdown target", 22);
		break;
	case 110:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "RTE Digitals Entry Profile", 26);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, GC->rtcc->med_f82.PrimaryEP.c_str(), GC->rtcc->med_f82.PrimaryEP.size());
		Text_Double(skp, CW, 4 * H / 14, "%.2lf°", GC->rtcc->med_f82.PrimaryInitialBank*DEG);
		Text_Double(skp, CW, 6 * H / 14, "%.2lf", GC->rtcc->med_f82.PrimaryGLIT);
		if (GC->rtcc->med_f82.PrimaryEP == "HB1")
		{
			skp->Text(CW, 8 * H / 14, GC->rtcc->med_f82.PrimaryRollDirection.c_str(), GC->rtcc->med_f82.PrimaryRollDirection.size());

			if (GC->rtcc->med_f82.PrimaryLongT > 999.9)
			{
				skp->Text(CW, 10 * H / 14, "Constant G", 10);
			}
			else
			{
				sprintf_s(Buffer, "%.2lf°", GC->rtcc->med_f82.PrimaryLongT*DEG);
				skp->Text(CW, 10 * H / 14, Buffer, strlen(Buffer));
			}
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		skp->Text(W - CW, 2 * H / 14, GC->rtcc->med_f82.BackupEP.c_str(), GC->rtcc->med_f82.BackupEP.size());
		Text_Double(skp, W - CW, 4 * H / 14, "%.2lf°", GC->rtcc->med_f82.BackupInitialBank*DEG);
		Text_Double(skp, W - CW, 6 * H / 14, "%.2lf", GC->rtcc->med_f82.BackupGLIT);
		if (GC->rtcc->med_f82.BackupEP == "HB1")
		{
			sprintf_s(Buffer, "%s", GC->rtcc->med_f82.PrimaryRollDirection.c_str());
			skp->Text(W - CW, 8 * H / 14, Buffer, strlen(Buffer));

			if (GC->rtcc->med_f82.BackupLongT > 999.9)
			{
				skp->Text(W - CW, 10 * H / 14, "Constant G", 10);
			}
			else
			{
				sprintf_s(Buffer, "%.2lf°", GC->rtcc->med_f82.BackupLongT*DEG);
				skp->Text(W - CW, 10 * H / 14, Buffer, strlen(Buffer));
			}
		}
		break;
	case 111:
	case 112:
	{
		int blocknum;
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		if (screen == 111)
		{
			skp->Text(W / 2, CH / 2, "CMC TIME INCREMENT UPDATE (MSK 340)", 35);
			blocknum = 0;
		}
		else
		{
			skp->Text(W / 2, CH / 2, "LGC TIME INCREMENT UPDATE (MSK 353)", 35);
			blocknum = 1;
		}
		RTCC::AGCTimeIncrementMakeupTableBlock *block = &GC->rtcc->CZTMEINC.Blocks[blocknum];
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 3 * H / 14, "RTCC TIME:", 10);
		GET_Display2(Buffer, G->RTCCClockTime[blocknum]);
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		skp->Text(CW, 5 * H / 14, "AGC TIME:", 9);
		GET_Display2(Buffer, G->AGCClockTime[blocknum]);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		skp->Text(CW, 7 * H / 14, "DELTA T:", 8);
		GET_Display2(Buffer, G->DeltaClockTime[blocknum]);
		skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		skp->Text(CW, 10 * H / 14, "AGC CLOCK ZERO:", 15);
		if (blocknum == 0)
		{
			GET_Display2(Buffer, GC->rtcc->GetCMCClockZero());
		}
		else
		{
			GET_Display2(Buffer, GC->rtcc->GetLGCClockZero());
		}
		skp->Text(CW, 11 * H / 14, Buffer, strlen(Buffer));
		x = 20;
		skp->Text(W - CW*x, H - CH * 12, "OID FCT   DSKY V73", 18);
		skp->Text(W - CW * x, H - CH * 10, "1", 1);
		skp->Text(W - CW * x, H - CH * 8, "2", 1);
		skp->Text(W - CW * (x - 4), H - CH * 10, "DELTAT", 6);
		skp->Text(W - CW * (x - 4), H - CH * 8, "DELTAT", 6);
		Text_Int(skp, W - CW * (x - 13), H - CH * 10, "%05d", block->Octals[0]);
		Text_Int(skp, W - CW * (x - 13), H - CH * 8, "%05d", block->Octals[1]);
		skp->Text(W - CW * x, H - CH * 4, "INCREMENT:", 10);
		GET_Display2(Buffer, block->TimeIncrement);
		skp->Text(W - CW * x, H - CH * 2, Buffer, strlen(Buffer));
	}
	break;
	case 113:
	case 114:
	{
		int blocknum;
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		if (screen == 113)
		{
			skp->Text(W / 2, CH / 2, "CMC LIFTOFF TIME UPDATE (MSK 341)", 33);
			blocknum = 0;
		}
		else
		{
			skp->Text(W / 2, CH / 2, "LGC LIFTOFF TIME UPDATE (MSK 339)", 33);
			blocknum = 1;
		}
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		RTCC::AGCLiftoffTimeUpdateMakeupTableBlock *block = &GC->rtcc->CZLIFTFF.Blocks[blocknum];
		skp->Text(CW, 3 * H / 14, "DESIRED LIFTOFF:", 16);
		GET_Display2(Buffer, G->DesiredRTCCLiftoffTime[blocknum]);
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));

		skp->Text(CW, 5 * H / 14, "AGC CLOCK ZERO:", 15);
		double lotime;
		if (blocknum == 0)
		{
			lotime = GC->rtcc->GetCMCClockZero();
		}
		else
		{
			lotime = GC->rtcc->GetLGCClockZero();
		}
		GET_Display2(Buffer, lotime);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		skp->Text(CW, 7 * H / 14, "DELTA T:", 8);
		GET_Display2(Buffer, G->DesiredRTCCLiftoffTime[blocknum] - lotime);
		skp->Text(CW, 8 * H / 14, Buffer, strlen(Buffer));
		skp->Text(CW, 10 * H / 14, "RTCC LIFTOFF:", 15);
		GET_Display2(Buffer, GC->rtcc->GetGMTLO()*3600.0);
		skp->Text(CW, 11 * H / 14, Buffer, strlen(Buffer));
		x = 20;
		skp->Text(W - CW * x, H - CH * 12, "OID FCT   DSKY V70", 18);
		skp->Text(W - CW * x, H - CH * 10, "1", 1);
		skp->Text(W - CW * x, H - CH * 8, "2", 1);
		skp->Text(W - CW * (x - 4), H - CH * 10, "DELTAT", 6);
		skp->Text(W - CW * (x - 4), H - CH * 8, "DELTAT", 6);
		Text_Int(skp, W - CW * (x - 13), H - CH * 10, "%05d", block->Octals[0]);
		Text_Int(skp, W - CW * (x - 13), H - CH * 8, "%05d", block->Octals[1]);
		skp->Text(W - CW * x, H - CH * 4, "INCREMENT:", 10);
		GET_Display2(Buffer, block->TimeIncrement);
		skp->Text(W - CW * x, H - CH * 2, Buffer, strlen(Buffer));
	}
	break;
	case 115:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "LUNAR TARGETING PROGRAM", 33);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->SetFont(font_menu3);
		GetCharSize(skp, CW, CH);
		x = 1;  y = 3; dx = 5;
		Text(skp, x, marker + y, "*");
		x++;
		Text(skp, x, y, "MOD:");
		if (G->LUNTAR_Input.mode == 0) Text(skp, x + dx, y, "Trajectory Evaluation");
		else if (G->LUNTAR_Input.mode == 1) Text(skp, x + dx, y, "Burn Evaluation");
		else Text(skp, x + dx, y, "Calculate Burn");
		y++;
		if (G->LUNTAR_Input.mode != 0)
		{
			Text(skp, x, y, "TIG:");
			GET_Display(skp, CW * (x + dx), CH * y, G->LUNTAR_Input.tig_guess, false); y++;
			Text(skp, x, y, "BT:");
			if (G->LUNTAR_Input.mode == 1 || G->LUNTAR_Input.bt_guess != 0.0) //Always show burn parameters for burn evaluation mode
			{
				Text_Double(skp, CW * (x + dx), CH * y, "%.1lf s", G->LUNTAR_Input.bt_guess); y++;
				Text(skp, x, y, "PIT:");
				Text_Double(skp, CW * (x + dx), CH * y, "%.2lf°", G->LUNTAR_Input.pitch_guess); y++;
				Text(skp, x, y, "YAW:");
				Text_Double(skp, CW * (x + dx), CH * y, "%.2lf°", G->LUNTAR_Input.yaw_guess); y++;
			}
			else
			{
				Text(skp, x + dx, y, "No initial guess"); y += 3;
			}
			if (G->LUNTAR_Input.mode == 2)
			{
				Text(skp, x, y, "LAT:");
				Text_Double(skp, CW * (x + dx), CH * y, "%.2lf°", G->LUNTAR_Input.lat_tgt); y++;
				Text(skp, x, y, "LNG:");
				Text_Double(skp, CW * (x + dx), CH * y, "%.2lf°", G->LUNTAR_Input.lng_tgt); y++;
				Text(skp, x, y, "OPT:");
				if (G->LUNTAR_Input.bOptimize) Text(skp, x + dx, y, "Optimize DV");
				else Text(skp, x + dx, y, "Desired impact time");
				y++;
				if (G->LUNTAR_Input.bOptimize == false)
				{
					Text(skp, x, y, "IMP:");
					GET_Display(skp, CW * (x + dx), CH * y, G->LUNTAR_Input.gmt_imp_tgt, false);
				}
			}
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		PrintIUVessel(Buffer);
		skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));
		skp->Text(W - CW, 9 * H / 20, "Burn Data:", 10);
		if (G->LUNTAR_Output.tig == 0.0 || G->LUNTAR_Output.bt == 0.0)
		{
			skp->Text(W - CW, 10 * H / 20, "No burn", 9);
		}
		else
		{
			Text_Double(skp, W - CW, 10 * H / 20, "TIG: TB8 + %.0lf s", G->LUNTAR_Output.tig);
			Text_Double(skp, W - CW, 11 * H / 20, "Burn time: %.1lf s", G->LUNTAR_Output.bt);
			Text_Double(skp, W - CW, 12 * H / 20, "Pitch: %.1lf°", G->LUNTAR_Output.pitch*DEG);
			Text_Double(skp, W - CW, 13 * H / 20, "Yaw: %.1lf°", G->LUNTAR_Output.yaw*DEG);
		}
		skp->Text(W - CW, 14 * H / 20, "Impact:", 7);
		GET_Display(skp, W - CW, 15 * H / 20, G->LUNTAR_Output.get_imp);
		Text_Double(skp, W - CW, 16 * H / 20, "Lat: %.2lf°", G->LUNTAR_Output.lat_imp*DEG);
		Text_Double(skp, W - CW, 17 * H / 20, "Lng: %.2lf°", G->LUNTAR_Output.lng_imp*DEG);
		Text_Double(skp, W - CW, 18 * H / 20, "FPA: %.2lf°", G->LUNTAR_Output.fpa_imp*DEG);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (G->LUNTAR_Output.err > 0)
		{
			if (G->LUNTAR_Output.err == 1)
			{
				sprintf_s(Buffer, "Initial guess did not impact. PC Alt: %.0lf NM", G->LUNTAR_Output.FlybyAlt);
			}
			else if (G->LUNTAR_Output.err == 2)
			{
				sprintf_s(Buffer, "Solution did not converge");
			}
			else if (G->LUNTAR_Output.err == 3)
			{
				sprintf_s(Buffer, "Timebase 8 not started");
			}
			skp->Text(CW, 19 * H / 20, Buffer, strlen(Buffer));
		}
		break;
	case 116:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 4, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(4);
		Text(skp, 10, 0, "RETROFIRE SEPARATION");
		Text(skp, 38, 0, "0355");
		Text(skp, 1, 2, "AREA");
		Text(skp, 1, 3, "MATRIX");
		Text(skp, 1, 4, "WT TAA");
		Text(skp, 1, 5, "RLH PLH YLH");
		Text(skp, 1, 6, "RO PI YM");
		Text(skp, 1, 7, "VC BT");
		Text(skp, 1, 8, "VT U DT");
		Text(skp, 1, 9, "H");
		Text(skp, 1, 10, "GETI");
		Text(skp, 1, 11, "GMTI");
		Text(skp, 1, 12, "HP");
		Text(skp, 1, 13, "EXT DV");
		Line(skp, CW * 14, CH * 2, CW * 14, CH * 21);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 4, true);
		{
			RetrofireDisplayParametersTableData *tab = &GC->rtcc->RZRFDP.data[2];

			if (tab->Indicator_Sep == 0)
			{
				Text(skp, 42, 3, tab->RefsID);
				Text(skp, 37, 4, "%.1lf", tab->CSMWeightSep);
				Text(skp, 42, 4, "%.0lf", tab->TrueAnomalySep);
				sprintf_s(Buffer, "%.1lf %.1lf %.1lf", tab->Att_LVLH_Sep.x, tab->Att_LVLH_Sep.y, tab->Att_LVLH_Sep.z);
				Text(skp, 42, 5, Buffer);
				sprintf_s(Buffer, "%.1lf %.1lf %.1lf", tab->Att_IMU_Sep.x, tab->Att_IMU_Sep.y, tab->Att_IMU_Sep.z);
				Text(skp, 42, 6, Buffer);
				Text(skp, 33, 7, "%.1lf", tab->DVC_Sep);
				OrbMech::SStoMMSS(tab->BurnTime_Sep, mm, secs, 0.1);
				sprintf_s(Buffer, "%02d:%04.1lf", mm, secs);
				Text(skp, 42, 7, Buffer);
				Text(skp, 33, 8, "%.1lf", tab->DVT_Sep);
				Text(skp, 36, 8, "%+d", tab->UllageQuads_Sep);
				Text(skp, 42, 8, "%.1lf", tab->UllageDT_Sep);
				Text(skp, 42, 9, "%.1lf", tab->H_Sep);
				Text_GET_HHHMMSSCS(skp, 42, 10, tab->GETI_Sep);
				Text_GET_HHHMMSS(skp, 42, 11, tab->GMTI_Sep);
				Text(skp, 42, 12, "%.1lf", tab->H_peri_sep);
				sprintf_s(Buffer, "%+07.1f %+07.1f %+07.1f", tab->VG_XDV_Sep.x, tab->VG_XDV_Sep.y, tab->VG_XDV_Sep.z);
				Text(skp, 42, 13, Buffer);
			}
			else if (tab->Indicator_Sep == 1)
			{
				Text(skp, 42, 10, "NO DATA");
			}
			else
			{
				Text(skp, 42, 10, "ERROR - REFER TO ONLINE");
			}
		}
		break;
	case 117:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Definition of Separation/Shaping Maneuver", 41);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (GC->rtcc->RZJCTTC.R30_GETI_SH > 0)
		{
			GET_Display2(Buffer, GC->rtcc->RZJCTTC.R30_GETI_SH);
		}
		else
		{
			sprintf(Buffer, "No Shaping Maneuver");
		}
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->RZJCTTC.R30_GETI_SH > 0)
		{
			sprintf(Buffer, "No Separation Maneuver");
		}
		else
		{
			sprintf(Buffer, "%.1lf min", GC->rtcc->RZJCTTC.R30_DeltaT_Sep / 60.0);
		}
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		ThrusterName(Buffer, GC->rtcc->RZJCTTC.R30_Thruster);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		Text_Double(skp, CW, 8 * H / 14, "%.1lf ft/s", GC->rtcc->RZJCTTC.R30_DeltaV / 0.3048);
		Text_Double(skp, CW, 10 * H / 14, "%.1lf s", GC->rtcc->RZJCTTC.R30_DeltaT);
		sprintf(Buffer, "%.2lf %.2lf %.2lf", GC->rtcc->RZJCTTC.R30_Att.x*DEG, GC->rtcc->RZJCTTC.R30_Att.y*DEG, GC->rtcc->RZJCTTC.R30_Att.z*DEG);
		skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		PrintUllage(Buffer, GC->rtcc->RZJCTTC.R30_Thruster, GC->rtcc->RZJCTTC.R30_Use4UllageThrusters, GC->rtcc->RZJCTTC.R30_Ullage_DT);
		skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));
		if (GC->rtcc->RZJCTTC.R30_Thruster == RTCC_ENGINETYPE_CSMSPS)
		{
			if (GC->rtcc->RZJCTTC.R30_GimbalIndicator == 1)
			{
				sprintf(Buffer, "Use System Parameters");
			}
			else
			{
				sprintf(Buffer, "Compute Gimbal Trims");
			}
			skp->Text(W - CW, 4 * H / 14, Buffer, strlen(Buffer));
		}
		break;
	case 118:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Retrofire Planning", 18);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "Separation/Shaping Constraints", 30);
		skp->Text(CW, 4 * H / 14, "Retrofire Constraints", 21);
		skp->Text(CW, 6 * H / 14, "Target Selection Display", 24);
		skp->Text(CW, 8 * H / 14, "Retrofire Maneuver", 18);
		break;
	case 119:
		SetMOCRFont(skp, 4, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(4);
		Text(skp, 10, 0, "CMC ENTRY UPDATE");
		Text(skp, 38, 0, "0343");
		Text(skp, 1, 2, "LOAD NO");
		Text(skp, 14, 2, "GETGEN");
		Text(skp, 31, 2, "SITES");
		Text(skp, 1, 3, "LAT");
		Text(skp, 19, 3, "LNG");
		Text(skp, 1, 5, "OID  FCT     DSKY V71   DECIMAL");
		Text(skp, 1, 7, "1    INDEX");
		Text(skp, 1, 8, "2    ADD");
		Text(skp, 1, 9, "3    LAT");
		Text(skp, 1, 10, "4    LAT");
		Text(skp, 1, 11, "5    LNG");
		Text(skp, 1, 12, "6    LNG");
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 4, true);
		Text(skp, 13, 2, "%04d", GC->rtcc->CZENTRY.SequenceNumber);
		Text_GET_HHHMMSS(skp, 30, 2, GC->rtcc->CZENTRY.GETLoadGeneration);
		Text(skp, 11, 3, "%+.2f", GC->rtcc->RZDBSC1.lat_T*DEG);
		Text(skp, 30, 3, "%+.2f", GC->rtcc->RZDBSC1.lng_T*DEG);
		for (int i = 0; i < 6; i++)
		{
			Text(skp, 20, 7 + i, "%05d", GC->rtcc->CZENTRY.Octals[i]);
		}
		Text(skp, 32, 9, "%+.2f", GC->rtcc->CZENTRY.Lat);
		Text(skp, 32, 11, "%+.2f", GC->rtcc->CZENTRY.Lng);
		break;
	case 120:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 2, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(2);
		Text(skp, 22, 0, "LANDMARK ACQUISITION");
		Text(skp, 60, 0, "1508");
		Text(skp, 1, 2, "STA ID");
		Text(skp, 36, 2, "PAGE");
		Text(skp, 44, 2, "OF");
		Text(skp, 1, 4, "STA    GETAOS     GETLOS      GETCA   LAM     H   GETSR  GETSS");
		Line(skp, (CW * 11) / 2, CH * 4, (CW * 11) / 2, CH * 31);
		Line(skp, (CW * 33) / 2, CH * 4, (CW * 33) / 2, CH * 31);
		Line(skp, (CW * 55) / 2, CH * 4, (CW * 55) / 2, CH * 31);
		Line(skp, (CW * 77) / 2, CH * 4, (CW * 77) / 2, CH * 31);
		Line(skp, (CW * 87) / 2, CH * 4, (CW * 87) / 2, CH * 31);
		Line(skp, (CW * 101) / 2, CH * 4, (CW * 101) / 2, CH * 31);
		Line(skp, (CW * 115) / 2, CH * 4, (CW * 115) / 2, CH * 31);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 2, true);
		Text(skp, 15, 2, GC->rtcc->EZLANDU1.VectorID);
		Text(skp, 43, 2, "%02d", GC->rtcc->EZLANDU1.curpage);
		Text(skp, 49, 2, "%02d", GC->rtcc->EZLANDU1.pages);
		for (unsigned i = 0; i < GC->rtcc->EZLANDU1.numcontacts[GC->rtcc->EZLANDU1.curpage - 1]; i++)
		{
			Text(skp, 5, 5 + i, GC->rtcc->EZLANDU1.STAID[GC->rtcc->EZLANDU1.curpage - 1][i]);
			if (GC->rtcc->EZLANDU1.BestAvailableAOS[GC->rtcc->EZLANDU1.curpage - 1][i]) Text(skp, 7, 5 + i, "*");
			Text_GET_HHHMMSS(skp, 16, 5 + i, GC->rtcc->EZLANDU1.GETAOS[GC->rtcc->EZLANDU1.curpage - 1][i]);
			if (GC->rtcc->EZLANDU1.BestAvailableLOS[GC->rtcc->EZLANDU1.curpage - 1][i]) Text(skp, 18, 5 + i, "*");
			Text_GET_HHHMMSS(skp, 27, 5 + i, GC->rtcc->EZLANDU1.GETLOS[GC->rtcc->EZLANDU1.curpage - 1][i]);
			if (GC->rtcc->EZLANDU1.BestAvailableCA[GC->rtcc->EZLANDU1.curpage - 1][i]) Text(skp, 29, 5 + i, "*");
			Text_GET_HHHMMSS(skp, 38, 5 + i, GC->rtcc->EZLANDU1.GETCA[GC->rtcc->EZLANDU1.curpage - 1][i]);
			Text(skp, 43, 5 + i, "%.1lf", GC->rtcc->EZLANDU1.Lambda[GC->rtcc->EZLANDU1.curpage - 1][i]);
			Text(skp, 50, 5 + i, "%.2lf", GC->rtcc->EZLANDU1.h[GC->rtcc->EZLANDU1.curpage - 1][i]);
			Text_GET_HHHMM(skp, 57, 5 + i, GC->rtcc->EZLANDU1.GETSR[GC->rtcc->EZLANDU1.curpage - 1][i]);
			Text_GET_HHHMM(skp, 64, 5 + i, GC->rtcc->EZLANDU1.GETSS[GC->rtcc->EZLANDU1.curpage - 1][i]);
		}
		if (GC->rtcc->EZLANDU1.err > 0)
		{
			Text(skp, 40, 31, "Error: %d", GC->rtcc->EZLANDU1.err);
		}
		break;
	case 121:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "LAUNCH WINDOW PROCESSOR INPUTS", 30);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->SetFont(font_menu2);
		GetCharSize(skp, CW, CH);
		skp->Text(CW, (marker + 3) * H / 22, "*", 1);
		x = 3; y = 3;
		if (subscreen == 0)
		{
			skp->Text(CW * x, y * H / 22, "PAD:", 4); y++;
			skp->Text(CW * x, y * H / 22, "TGT:", 4); y++;
			skp->Text(CW * x, y * H / 22, "LOT:", 4); y++;
			skp->Text(CW * x, y * H / 22, "CKFACT:", 7); y++;
			skp->Text(CW * x, y * H / 22, "CAREA:", 6); y++;
			skp->Text(CW * x, y * H / 22, "CWHT:", 5); y++;
			skp->Text(CW * x, y * H / 22, "NS:", 3); y++;
			skp->Text(CW * x, y * H / 22, "DAY:", 4); y++;
			skp->Text(CW * x, y * H / 22, "PFT:", 4); y++;
			skp->Text(CW * x, y * H / 22, "PFA:", 4); y++;
			skp->Text(CW * x, y * H / 22, "YSMAX:", 6); y++;
			skp->Text(CW * x, y * H / 22, "DTOPT:", 6); y++;
			skp->Text(CW * x, y * H / 22, "DTGRR:", 6); y++;
			skp->Text(CW * x, y * H / 22, "RINS:", 4); y++;
			skp->Text(CW * x, y * H / 22, "VINS:", 4); y++;
			skp->Text(CW * x, y * H / 22, "GAMINS:", 7); y++;
			skp->Text(CW * x, y * H / 22, "GMTLOR:", 7); y++;
			skp->Text(CW * x, y * H / 22, "OFFSET:", 7); y++;
			skp->Text(CW * x, y * H / 22, "BIAS:", 5);

			x = 11; y = 3;
			if (GC->rtcc->PZSLVCON.Pad == 1)
			{
				sprintf(Buffer, "CSM");
			}
			else
			{
				sprintf(Buffer, "LEM");
			}
			skp->Text(CW * x, y * H / 22, Buffer, strlen(Buffer)); y++;
			if (GC->MissionPlanningActive)
			{
				if (GC->rtcc->PZSLVCON.TargetVectorTime < 0.0)
				{
					sprintf(Buffer, "Present Time/After last MPT maneuver");
				}
				else
				{
					GET_Display2(Buffer, GC->rtcc->PZSLVCON.TargetVectorTime);
				}
			}
			else
			{
				if (G->Rendezvous_Target != NULL)
				{
					sprintf(Buffer, "%s", G->Rendezvous_Target->GetName());
				}
				else
				{
					sprintf(Buffer, "No Target!");
				}
			}
			skp->Text(CW * x, y * H / 22, Buffer, strlen(Buffer)); y++;
			switch (GC->rtcc->PZSLVCON.LOT)
			{
			case 1:
				sprintf(Buffer, "L/O on input time");
				break;
			case 2:
				sprintf(Buffer, "L/O with phase angle at insertion (OFFSET)");
				break;
			case 3:
				sprintf(Buffer, "Biased phase zero (GMTLOR)");
				break;
			case 4:
				sprintf(Buffer, "Biased phase zero (TPLANE)");
				break;
			case 5:
				sprintf(Buffer, "In-plane");
				break;
			case 6:
				sprintf(Buffer, "In-plane with nodal regression");
				break;
			default:
				sprintf(Buffer, "");
				break;
			}
			skp->Text(CW * x, y * H / 22, Buffer, strlen(Buffer)); y++;
			Text_Double(skp, CW * x, y * H / 22, "%.2lf", GC->rtcc->PZSLVCON.CKFACT); y++;
			Text_Double(skp, CW * x, y * H / 22, "%.1lf sq ft", GC->rtcc->PZSLVCON.CAREA / pow(0.3048, 2)); y++;
			Text_Double(skp, CW * x, y * H / 22, "%.1lf lbs", GC->rtcc->PZSLVCON.CWHT / LBS2KG); y++;
			if (GC->rtcc->PZSLVCON.NS == 0)
			{
				skp->Text(CW * x, y * H / 22, "Northerly", 9);
			}
			else
			{
				skp->Text(CW * x, y * H / 22, "Southerly", 9);
			}
			y++;
			Text_Int(skp, CW * x, y * H / 22, "%d", GC->rtcc->PZSLVCON.DAY); y++;
			Text_Double(skp, CW * x, y * H / 22, "%.1lf sec", GC->rtcc->PZSLVCON.PFT); y++;
			Text_Double(skp, CW * x, y * H / 22, "%.3lf deg", GC->rtcc->PZSLVCON.PFA*DEG); y++;
			Text_Double(skp, CW * x, y * H / 22, "%.1lf deg", GC->rtcc->PZSLVCON.YSMAX*DEG); y++;
			Text_Double(skp, CW * x, y * H / 22, "%.1lf sec", GC->rtcc->PZSLVCON.DTOPT); y++;
			Text_Double(skp, CW * x, y * H / 22, "%.1lf sec", GC->rtcc->PZSLVCON.DTGRR); y++;
			Text_Double(skp, CW * x, y * H / 22, "%.2lf m", GC->rtcc->PZSLVCON.RINS); y++;
			Text_Double(skp, CW * x, y * H / 22, "%.2lf m/s", GC->rtcc->PZSLVCON.VINS); y++;
			Text_Double(skp, CW * x, y * H / 22, "%+.3lf deg", GC->rtcc->PZSLVCON.GAMINS*DEG); y++;
			if (GC->rtcc->PZSLVCON.LOT <= 3)
			{
				GET_Display2(Buffer, GC->rtcc->PZSLVCON.GMTLOR);
				skp->Text(CW * x, y * H / 22, Buffer, strlen(Buffer));
			}
			y++;
			if (GC->rtcc->PZSLVCON.LOT == 2)
			{
				Text_Double(skp, CW * x, y * H / 22, "%+.3lf deg", GC->rtcc->PZSLVCON.OFFSET*DEG);
			}
			y++;
			if (GC->rtcc->PZSLVCON.LOT == 3 || GC->rtcc->PZSLVCON.LOT == 4)
			{
				Text_Double(skp, CW * x, y * H / 22, "%+.1lf sec", GC->rtcc->PZSLVCON.BIAS);
			}
			y++;
		}
		else
		{
			skp->Text(CW * x, y * H / 22, "TRANS:", 6); y++;
			skp->Text(CW * x, y * H / 22, "INSCO:", 6); y++;
			skp->Text(CW * x, y * H / 22, "DHW:", 4); y++;
			skp->Text(CW * x, y * H / 22, "DU:", 3); y++;
			skp->Text(CW * x, y * H / 22, "ANOM:", 5); y++;
			skp->Text(CW * x, y * H / 22, "DELNOF:", 7); y++;
			skp->Text(CW * x, y * H / 22, "DELNO:", 6); y++;
			skp->Text(CW * x, y * H / 22, "PHASE:", 6); y++;
			skp->Text(CW * x, y * H / 22, "LAZCOE:", 7);
			x = 11; y = 3;
			if (GC->rtcc->PZSLVCON.LOT == 5 || GC->rtcc->PZSLVCON.LOT == 6)
			{
				Text_Double(skp, CW * x, y * H / 22, "%+.1lf sec", GC->rtcc->PZSLVCON.TRANS);
			}
			y++;
			if (GC->rtcc->PZSLVCON.INSCO == 1)
			{
				strcpy(Buffer, "Input VINS, GAMINS, RINS");
			}
			else if (GC->rtcc->PZSLVCON.INSCO == 2)
			{
				strcpy(Buffer, "Input GAMINS, RINS and DH");
			}
			else
			{
				strcpy(Buffer, "Input GAMINS, RINS, ALT");
			}
			skp->Text(CW * x, y * H / 22, Buffer, strlen(Buffer)); y++;
			if (GC->rtcc->PZSLVCON.INSCO != 1)
			{
				Text_Double(skp, CW * x, y * H / 22, "%+.1lf NM", GC->rtcc->PZSLVCON.DHW / 1852.0); y++;
				Text_Double(skp, CW * x, y * H / 22, "%+.1lf deg", GC->rtcc->PZSLVCON.DU * DEG); y++;
				Text_Double(skp, CW * x, y * H / 22, "%+.1lf NM", GC->rtcc->PZSLVCON.ANOM / 1852.0); y++;
			}
			else
			{
				y += 3;
			}
			if (GC->rtcc->PZSLVCON.DELNOF)
			{
				Text_String(skp, CW * x, y * H / 22, "Compute DELNO"); y += 2;
			}
			else
			{
				Text_String(skp, CW * x, y * H / 22, "Input DELNO"); y++;
				Text_Double(skp, CW * x, y * H / 22, "%+.3lf deg", GC->rtcc->PZSLVCON.DELNO*DEG); y++;
			}
			if (GC->rtcc->PZSLVCON.NEGTIV == 2 && GC->rtcc->PZSLVCON.WRAP == 0)
			{
				strcpy(Buffer, "-90 to 90 deg");
			}
			else if (GC->rtcc->PZSLVCON.NEGTIV == 0 && GC->rtcc->PZSLVCON.WRAP == 0)
			{
				strcpy(Buffer, "90 to 270 deg");
			}
			else if (GC->rtcc->PZSLVCON.NEGTIV == 2 && GC->rtcc->PZSLVCON.WRAP == 1)
			{
				strcpy(Buffer, "270 to 450 deg");
			}
			else if (GC->rtcc->PZSLVCON.NEGTIV == 0 && GC->rtcc->PZSLVCON.WRAP == 1)
			{
				strcpy(Buffer, "450 to 630 deg");
			}
			else if (GC->rtcc->PZSLVCON.NEGTIV == 2 && GC->rtcc->PZSLVCON.WRAP == 2)
			{
				strcpy(Buffer, "630 to 720 deg");
			}
			else
			{
				strcpy(Buffer, "Invalid phase flags!");
			}
			skp->Text(CW * x, y * H / 22, Buffer, strlen(Buffer)); y++;
			Text_Double(skp, CW * x, y * H / 22, "%+.4lf", GC->rtcc->PZSLVCON.LAZCOE[0]); y++;
			Text_Double(skp, CW * x, y * H / 22, "%+.4lf", GC->rtcc->PZSLVCON.LAZCOE[1]); y++;
			Text_Double(skp, CW * x, y * H / 22, "%+.4lf", GC->rtcc->PZSLVCON.LAZCOE[2]); y++;
			Text_Double(skp, CW * x, y * H / 22, "%+.4lf", GC->rtcc->PZSLVCON.LAZCOE[3]); y++;
		}

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		Text_Int(skp, W - CW, 2 * H / 14, "%d/2", subscreen + 1);
		break;
	case 122:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "LAUNCH TARGETING", 16);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->SetFont(font_menu2);
		GetCharSize(skp, CW, CH);
		skp->Text(CW, 5 * H / 32, "GMTLO", 5);
		skp->Text(CW, 6 * H / 32, "TINS", 4);
		skp->Text(CW, 8 * H / 32, "GMTLO*", 6);
		skp->Text(CW, 9 * H / 32, "PFA", 3);
		skp->Text(CW, 10 * H / 32, "PFT", 3);
		skp->Text(CW, 11 * H / 32, "DN", 2);
		skp->Text(CW, 13 * H / 32, "TYAW", 4);
		skp->Text(CW, 14 * H / 32, "TPLANE", 6);
		skp->Text(CW, 16 * H / 32, "AZL", 3);
		skp->Text(CW, 17 * H / 32, "PAD", 3);
		skp->Text(CW, 18 * H / 32, "LATLS", 5);
		skp->Text(CW, 19 * H / 32, "LONGLS", 6);
		skp->Text(CW, 20 * H / 32, "GPAZ", 4);
		skp->Text(CW, 21 * H / 32, "YP", 2);
		skp->Text(CW, 23 * H / 32, "DELNO", 5);
		skp->Text(CW, 24 * H / 32, "DELNOD", 6);
		skp->Text(W - CW * 19, 5 * H / 32, "TGRR", 4);
		skp->Text(W - CW * 19, 6 * H / 32, "VIGM", 4);
		skp->Text(W - CW * 19, 8 * H / 32, "RIGM", 4);
		skp->Text(W - CW * 19, 10 * H / 32, "GIGM", 4);
		skp->Text(W - CW * 19, 11 * H / 32, "IIGM", 4);
		skp->Text(W - CW * 19, 12 * H / 32, "TIGM", 4);
		skp->Text(W - CW * 19, 13 * H / 32, "TDIGM", 5);
		skp->Text(W - CW * 19, 15 * H / 32, "LAUNCH ORBIT", 12);
		skp->Text(W - CW * 24, 17 * H / 32, "APOGEE", 6);
		skp->Text(W - CW * 24, 18 * H / 32, "PERIGEE", 7);
		skp->Text(W - CW * 24, 19 * H / 32, "T ANOMALY", 9);
		skp->Text(W - CW * 24, 20 * H / 32, "ALTITUDE", 8);
		skp->Text(W - CW * 24, 21 * H / 32, "DH", 2);
		skp->Text(W - CW * 24, 22 * H / 32, "TIME", 4);
		skp->Text(W - CW * 19, 24 * H / 32, "TARGET ORBIT", 12);
		skp->Text(W - CW * 24, 26 * H / 32, "APOGEE", 6);
		skp->Text(W - CW * 24, 27 * H / 32, "PERIGEE", 7);
		skp->Text(W - CW * 24, 28 * H / 32, "INCLINATION", 11);
		skp->Text(W - CW * 24, 29 * H / 32, "INS PHASE", 9);
		skp->Text(W - CW * 24, 30 * H / 32, "DN TARGET", 9);
		skp->Text(W - CW * 24, 31 * H / 32, "BIAS", 4);
		if (G->iuvessel == NULL)
		{
			sprintf_s(Buffer, 127, "No IU!");
		}
		else
		{
			sprintf_s(Buffer, 127, G->iuvessel->GetName());
		}
		skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
		switch (G->iuUplinkResult)
		{
		case 1:
			sprintf(Buffer, "Uplink accepted!");
			break;
		case 2:
			sprintf(Buffer, "Vessel has no IU!");
			break;
		case 3:
			sprintf(Buffer, "Uplink rejected!");
			break;
		case 4:
			sprintf(Buffer, "No targeting parameters!");
			break;
		default:
			sprintf(Buffer, "No Uplink");
			break;
		}
		skp->Text(CW, 13 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		x = 22;
		GET_Display2(Buffer, GC->rtcc->PZSLVTAR.GMTLO);
		skp->Text(CW * x, 5 * H / 32, Buffer, strlen(Buffer));
		GET_Display2(Buffer, GC->rtcc->PZSLVTAR.TINS);
		skp->Text(CW * x, 6 * H / 32, Buffer, strlen(Buffer));
		GET_Display2(Buffer, GC->rtcc->PZSLVTAR.GSTAR);
		skp->Text(CW * x, 8 * H / 32, Buffer, strlen(Buffer));
		Text_Double(skp, CW * x, 9 * H / 32,  "%.3lf", GC->rtcc->PZSLVCON.PFA*DEG);
		Text_Double(skp, CW * x, 10 * H / 32, "%.3lf", GC->rtcc->PZSLVCON.PFT);
		Text_Double(skp, CW * x, 11 * H / 32, "%+.3lf", GC->rtcc->PZSLVTAR.DN);
		GET_Display2(Buffer, GC->rtcc->PZSLVTAR.TYAW);
		skp->Text(CW * x, 13 * H / 32, Buffer, strlen(Buffer));
		GET_Display2(Buffer, GC->rtcc->PZSLVTAR.TPLANE);
		skp->Text(CW * x, 14 * H / 32, Buffer, strlen(Buffer));
		Text_Double(skp, CW * x, 16 * H / 32, "%.3lf", GC->rtcc->PZSLVTAR.AZL);
		if (GC->rtcc->PZSLVTAR.Pad == 1)
		{
			skp->Text(CW * x, 17 * H / 32, "CSM", 3);
		}
		else
		{
			skp->Text(CW * x, 17 * H / 32, "LEM", 3);
		}
		Text_Double(skp, CW * x, 18 * H / 32, "%+.3lf", GC->rtcc->PZSLVTAR.LATLS);
		Text_Double(skp, CW * x, 19 * H / 32, "%+.3lf", GC->rtcc->PZSLVTAR.LONGLS);
		Text_Double(skp, CW * x, 23 * H / 32, "%.8lf", GC->rtcc->PZSLVTAR.DELNO);
		Text_Double(skp, CW * x, 24 * H / 32, "%.8lf", GC->rtcc->PZSLVTAR.DELNOD);
		GET_Display2(Buffer, GC->rtcc->PZSLVTAR.TGRR);
		skp->Text(W - CW, 5 * H / 32, Buffer, strlen(Buffer));
		Text_Double(skp, W - CW, 6 * H / 32, "%.2lf", GC->rtcc->PZSLVTAR.VIGM);
		Text_Double(skp, W - CW, 7 * H / 32, "%.2lf", GC->rtcc->PZSLVTAR.VIGM / 0.3048);
		Text_Double(skp, W - CW, 8 * H / 32, "%.2lf", GC->rtcc->PZSLVTAR.RIGM);
		Text_Double(skp, W - CW, 9 * H / 32, "%.2lf", GC->rtcc->PZSLVTAR.RIGM / 0.3048);
		Text_Double(skp, W - CW, 10 * H / 32, "%.8lf", GC->rtcc->PZSLVTAR.GIGM);
		Text_Double(skp, W - CW, 11 * H / 32, "%.3lf", GC->rtcc->PZSLVTAR.IIGM);
		Text_Double(skp, W - CW, 12 * H / 32, "%.3lf", GC->rtcc->PZSLVTAR.TIGM);
		Text_Double(skp, W - CW, 13 * H / 32, "%+.7lf", GC->rtcc->PZSLVTAR.TDIGM);
		Text_Double(skp, W - CW, 17 * H / 32, "%.2lf", GC->rtcc->PZSLVTAR.HA_C);
		Text_Double(skp, W - CW, 18 * H / 32, "%.2lf", GC->rtcc->PZSLVTAR.HP_C);
		Text_Double(skp, W - CW, 19 * H / 32, "%+.2lf", GC->rtcc->PZSLVTAR.TA_C);
		Text_Double(skp, W - CW, 20 * H / 32, "%.2lf", GC->rtcc->PZSLVTAR.H);
		Text_Double(skp, W - CW, 21 * H / 32, "%.2lf", GC->rtcc->PZSLVTAR.DH);
		Text_Double(skp, W - CW, 22 * H / 32, "%E", GC->rtcc->PZSLVTAR.GMTLO);
		Text_Double(skp, W - CW, 26 * H / 32, "%.2lf", GC->rtcc->PZSLVTAR.HA_T);
		Text_Double(skp, W - CW, 27 * H / 32, "%.2lf", GC->rtcc->PZSLVTAR.HP_T);
		Text_Double(skp, W - CW, 28 * H / 32, "%.2lf", GC->rtcc->PZSLVTAR.I_T);
		Text_Double(skp, W - CW, 29 * H / 32, "%+.3lf", GC->rtcc->PZSLVTAR.PA);
		Text_Double(skp, W - CW, 30 * H / 32, "%+.3lf", GC->rtcc->PZSLVTAR.DN_T);
		Text_Double(skp, W - CW, 31 * H / 32, "%.3lf", GC->rtcc->PZSLVTAR.BIAS);
		break;
	case 123:
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 16, 0, "RENDEZVOUS PLANNING TABLE");
		Text(skp, 52, 0, "0057");
		Text(skp, 0, 2, "CSM STA");
		Text(skp, 0, 3, "GMTV");
		Text(skp, 0, 4, "GETV");
		Text(skp, 0, 5, "MVR VEH");
		Text(skp, 22, 2, "LM STA");
		Text(skp, 22, 3, "GMTV");
		Text(skp, 22, 4, "GETV");
		Text(skp, 44, 2, "DV MAX");
		Text(skp, 44, 3, "MIN H");
		Text(skp, 44, 4, "WT");
		Text(skp, 1, 7, "ID  M  DVCSM   DVLM  NC1   NH  NCC  NSR  NPC   GETTPI");
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		Text(skp, 40, 27, GC->rtcc->PZRPDT.ErrorMessage);
		for (int i = 0; i < GC->rtcc->PZRPDT.plans; i++)
		{
			Text(skp, 3, 9 + i * 2, "%d", GC->rtcc->PZRPDT.data[i].ID);
			Text(skp, 6, 9 + i * 2, "%d", GC->rtcc->PZRPDT.data[i].M);
			Text(skp, 13, 9 + i * 2, "%.1lf", GC->rtcc->PZRPDT.data[i].DV_CSM);
			Text(skp, 20, 9 + i * 2, "%.1lf", GC->rtcc->PZRPDT.data[i].DV_LM);
			Text(skp, 25, 9 + i * 2, "%.1lf", GC->rtcc->PZRPDT.data[i].NC1);
			Text(skp, 30, 9 + i * 2, "%.1lf", GC->rtcc->PZRPDT.data[i].NH);
			if (GC->rtcc->PZRPDT.data[i].NCC >= 0.0)
			{
				Text(skp, 35, 9 + i * 2, "%.1lf", GC->rtcc->PZRPDT.data[i].NCC);
			}
			Text(skp, 40, 9 + i * 2, "%.1lf", GC->rtcc->PZRPDT.data[i].NSR);
			if (GC->rtcc->PZRPDT.data[i].NPC >= 0.0)
			{
				Text(skp, 46, 9 + i * 2, "%.2lf", GC->rtcc->PZRPDT.data[i].NPC);
			}
			Text_GET_HHHMMSS(skp, 56, 9 + i * 2, GC->rtcc->PZRPDT.data[i].GETTPI);
		}
		break;
	case 124:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "PERIGEE ADJUST INPUTS (K28)", 27);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		if (GC->rtcc->med_k28.VEH == RTCC_MPT_CSM)
		{
			sprintf(Buffer, "CSM");
		}
		else
		{
			sprintf(Buffer, "LEM");
		}
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		if (GC->MissionPlanningActive)
		{
			GET_Display(Buffer, GC->rtcc->med_k28.VectorTime, false);
		}
		else
		{
			if (GC->rtcc->med_k28.VEH == RTCC_MPT_CSM)
			{
				PrintCSMVessel(Buffer, false);
			}
			else
			{
				PrintLMVessel(Buffer, false);
			}
		}
		skp->Text(CW, 4 * H / 14, Buffer, strlen(Buffer));
		GET_Display(Buffer, GC->rtcc->med_k28.ThresholdTime, false);
		skp->Text(CW, 6 * H / 14, Buffer, strlen(Buffer));
		Text_Double(skp, CW, 8 * H / 14, "%.0lf s", GC->rtcc->med_k28.TimeIncrement);
		Text_Double(skp, CW, 10 * H / 14, "%.0lf NM", GC->rtcc->med_k28.H_P);
		ThrusterName(Buffer, GC->rtcc->med_k28.Thruster);
		skp->Text(CW, 12 * H / 14, Buffer, strlen(Buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (GC->rtcc->med_k28.Thruster == RTCC_ENGINETYPE_LMDPS)
		{
			Text_Double(skp, W - CW, 4 * H / 14, "%.3lf", GC->rtcc->med_k28.DPSScaleFactor);
		}
		break;
	case 125:
		SetMOCRFont(skp, 4, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(4);
		Text(skp, 11, 0, "PERIGEE ADJUST TABLE");
		Text(skp, 38, 0, "0050");
		Text(skp, 5, 2, "HP DES");
		Text(skp, 5, 4, "TIG      DV    BT   TAA    H     HA");
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 4, true);
		if (GC->rtcc->MSK0050Buffer.size() != 0)
		{
			int j = 0;
			Text(skp, 19, 2, GC->rtcc->MSK0050Buffer[j]); j++;
			for (unsigned i = 0; i < 6; i++)
			{
				Text(skp, 11, 6 + i * 2, GC->rtcc->MSK0050Buffer[j]); j++;
				Text(skp, 18, 6 + i * 2, GC->rtcc->MSK0050Buffer[j]); j++;
				Text(skp, 24, 6 + i * 2, GC->rtcc->MSK0050Buffer[j]); j++;
				Text(skp, 28, 6 + i * 2, GC->rtcc->MSK0050Buffer[j]); j++;
				Text(skp, 35, 6 + i * 2, GC->rtcc->MSK0050Buffer[j]); j++;
				Text(skp, 42, 6 + i * 2, GC->rtcc->MSK0050Buffer[j]); j++;
			}
		}
		break;
	case 126:
		skp->SetPen(pen2);
		SetMOCRFont(skp, 3, false);
		GetCharSize(skp, CW, CH);
		SetMOCRDisplayCentered(3);
		Text(skp, 16, 0, "LM OPTICAL SUPPORT TABLE");
		Text(skp, 52, 0, "0239");
		Text(skp, 0, 1, "VEH");
		Text(skp, 10, 1, "MODE");
		Text(skp, 8, 2, "DKAN");
		Text(skp, 20, 1, "LM YO");
		Text(skp, 20, 2, "LM PI");
		Text(skp, 20, 3, "LM RM");
		Text(skp, 33, 1, "CSM RO");
		Text(skp, 33, 2, "CSM PI");
		Text(skp, 33, 3, "CSM YM");
		Text(skp, 0, 5, "MATRIX USED");
		Text(skp, 21, 5, "GETHORIZ");
		Text(skp, 43, 5, "OGA");
		Text(skp, 0, 6, "STAR   RA    DEC      Y     P     R   L   A1    A2   D");
		Text(skp, 0, 7, "1");
		Text(skp, 0, 8, "2");
		Text(skp, 40, 9, "COAS ANGLES");
		Text(skp, 3, 10, "STAR    IX        IY        IZ");
		Text(skp, 38, 10, "STAR  EL  SXP AXIS");
		Text(skp, 3, 11, "1");
		Text(skp, 3, 12, "2");
		Text(skp, 38, 11, "1");
		Text(skp, 38, 12, "2");
		Text(skp, 6, 14, "REALIGN ATTITUDES");
		Text(skp, 32, 14, "PRESENTLY STORED ATT");
		Text(skp, 0, 15, "GIMB ANG");
		Text(skp, 17, 15, "Y", 2, 1);
		Text(skp, 31, 15, "P", 2, 1);
		Text(skp, 45, 15, "R", 2, 1);
		Text(skp, 0, 16, "FDAI ANG");
		Text(skp, 17, 16, "R", 2, 1);
		Text(skp, 31, 16, "P", 2, 1);
		Text(skp, 45, 16, "Y", 2, 1);
		Text(skp, 30, 15, "TLM");
		Text(skp, 30, 16, "MED");
		Text(skp, 1, 18, "MATRIX 1");
		Text(skp, 30, 18, "MATGET");
		Text(skp, 3, 19, "X/XE");
		Text(skp, 22, 19, "X/YE");
		Text(skp, 40, 19, "X/ZE");
		Text(skp, 3, 20, "Y/XE");
		Text(skp, 22, 20, "Y/YE");
		Text(skp, 40, 20, "Y/ZE");
		Text(skp, 3, 21, "Z/XE");
		Text(skp, 22, 21, "Z/YE");
		Text(skp, 40, 21, "Z/ZE");
		Text(skp, 1, 23, "MATRIX 2");
		Text(skp, 30, 23, "MATGET");
		Text(skp, 3, 24, "X/XE");
		Text(skp, 22, 24, "X/YE");
		Text(skp, 40, 24, "X/ZE");
		Text(skp, 3, 25, "Y/XE");
		Text(skp, 22, 25, "Y/YE");
		Text(skp, 40, 25, "Y/ZE");
		Text(skp, 3, 26, "Z/XE");
		Text(skp, 22, 26, "Z/YE");
		Text(skp, 40, 26, "Z/ZE");
		Line(skp, 0, (CH * 9) / 2, CW * 56, (CH * 9) / 2);
		Line(skp, 0, (CH * 19) / 2, (CW * 75) / 2, (CH * 19) / 2);
		Line(skp, (CW * 75) / 2, (CH * 19) / 2, (CW * 75) / 2, (CH * 27) / 2);
		Line(skp, 0, (CH * 27) / 2, CW * 56, (CH * 27) / 2);
		Line(skp, (CW * 59) / 2, (CH * 27) / 2, (CW * 59) / 2, (CH * 35) / 2);
		Line(skp, 0, (CH * 35) / 2, CW * 56, (CH * 35) / 2);
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		SetMOCRFont(skp, 3, true);
		if (GC->rtcc->LOSTDisplayBuffer.size() < 81) return true;
		Text(skp, 8, 1, GC->rtcc->LOSTDisplayBuffer[0]); //VEH
		Text(skp, 18, 1, GC->rtcc->LOSTDisplayBuffer[1]); //MODE
		Text(skp, 18, 2, GC->rtcc->LOSTDisplayBuffer[2]); //DKAN
		Text(skp, 31, 1, GC->rtcc->LOSTDisplayBuffer[3]); //LM ATT X
		Text(skp, 31, 2, GC->rtcc->LOSTDisplayBuffer[4]); //LM ATT Y
		Text(skp, 31, 3, GC->rtcc->LOSTDisplayBuffer[5]); //LM ATT Z
		Text(skp, 45, 1, GC->rtcc->LOSTDisplayBuffer[6]); //CSM ATT X
		Text(skp, 45, 2, GC->rtcc->LOSTDisplayBuffer[7]); //CSM ATT Y
		Text(skp, 45, 3, GC->rtcc->LOSTDisplayBuffer[8]); //CSM ATT Z
		Text(skp, 18, 5, GC->rtcc->LOSTDisplayBuffer[9]); //MAT USED
		Text(skp, 41, 5, GC->rtcc->LOSTDisplayBuffer[10]); //GETHORIZ
		Text(skp, 52, 5, GC->rtcc->LOSTDisplayBuffer[11]); //OGA
		Text(skp, 4, 7, GC->rtcc->LOSTDisplayBuffer[12]); //Star 1
		Text(skp, 4, 8, GC->rtcc->LOSTDisplayBuffer[13]); //Star 2
		Text(skp, 11, 7, GC->rtcc->LOSTDisplayBuffer[14]); //RA star 1
		Text(skp, 18, 7, GC->rtcc->LOSTDisplayBuffer[15]); //DEC star 1
		Text(skp, 11, 8, GC->rtcc->LOSTDisplayBuffer[16]); //RA star 2
		Text(skp, 18, 8, GC->rtcc->LOSTDisplayBuffer[17]); //DEC star 2
		Text(skp, 25, 7, GC->rtcc->LOSTDisplayBuffer[18]); //Roll star 1
		Text(skp, 31, 7, GC->rtcc->LOSTDisplayBuffer[19]); //Pitch star 1
		Text(skp, 37, 7, GC->rtcc->LOSTDisplayBuffer[20]); //Yaw star 1
		Text(skp, 25, 8, GC->rtcc->LOSTDisplayBuffer[21]); //Roll star 2
		Text(skp, 31, 8, GC->rtcc->LOSTDisplayBuffer[22]); //Pitch star 2
		Text(skp, 37, 8, GC->rtcc->LOSTDisplayBuffer[23]); //Yaw star 2
		Text(skp, 39, 7, GC->rtcc->LOSTDisplayBuffer[24]); //L1
		Text(skp, 39, 8, GC->rtcc->LOSTDisplayBuffer[25]); //L2
		Text(skp, 46, 7, GC->rtcc->LOSTDisplayBuffer[26]); //A1 star 1
		Text(skp, 46, 8, GC->rtcc->LOSTDisplayBuffer[27]); //A1 star 2
		Text(skp, 52, 7, GC->rtcc->LOSTDisplayBuffer[28]); //A2 star 1
		Text(skp, 52, 8, GC->rtcc->LOSTDisplayBuffer[29]); //A2 star 2
		Text(skp, 54, 7, GC->rtcc->LOSTDisplayBuffer[30]); //Detent star 1
		Text(skp, 54, 8, GC->rtcc->LOSTDisplayBuffer[31]); //Detent star 2
		Text(skp, 42, 11, GC->rtcc->LOSTDisplayBuffer[32]); //COAS star 1
		Text(skp, 42, 12, GC->rtcc->LOSTDisplayBuffer[33]); //COAS star 2
		Text(skp, 46, 11, GC->rtcc->LOSTDisplayBuffer[34]); //COAS EL 1
		Text(skp, 46, 12, GC->rtcc->LOSTDisplayBuffer[35]); //COAS EL 2
		Text(skp, 49, 11, GC->rtcc->LOSTDisplayBuffer[36]); //COAS SXP 1
		Text(skp, 49, 12, GC->rtcc->LOSTDisplayBuffer[37]); //COAS SXP 2
		Text(skp, 54, 11, GC->rtcc->LOSTDisplayBuffer[38]); //COAS axis
		Text(skp, 7, 11, GC->rtcc->LOSTDisplayBuffer[39]); //Telemetry star 1
		Text(skp, 7, 12, GC->rtcc->LOSTDisplayBuffer[40]); //Telemetry star 2
		Text(skp, 17, 11, GC->rtcc->LOSTDisplayBuffer[41]); //Telemetry star unit vector 1
		Text(skp, 17, 12, GC->rtcc->LOSTDisplayBuffer[42]); //Telemetry star unit vector 1
		Text(skp, 27, 11, GC->rtcc->LOSTDisplayBuffer[43]); //Telemetry star unit vector 1
		Text(skp, 27, 12, GC->rtcc->LOSTDisplayBuffer[44]); //Telemetry star unit vector 2
		Text(skp, 37, 11, GC->rtcc->LOSTDisplayBuffer[45]); //Telemetry star unit vector 2
		Text(skp, 37, 12, GC->rtcc->LOSTDisplayBuffer[46]); //Telemetry star unit vector 2
		Text(skp, 15, 15, GC->rtcc->LOSTDisplayBuffer[47]); //Realign Attitude IMU
		Text(skp, 22, 15, GC->rtcc->LOSTDisplayBuffer[48]); //Realign Attitude IMU
		Text(skp, 29, 15, GC->rtcc->LOSTDisplayBuffer[49]); //Realign Attitude IMU
		Text(skp, 15, 16, GC->rtcc->LOSTDisplayBuffer[50]); //Realign Attitude FDAI
		Text(skp, 22, 16, GC->rtcc->LOSTDisplayBuffer[51]); //Realign Attitude FDAI
		Text(skp, 29, 16, GC->rtcc->LOSTDisplayBuffer[52]); //Realign Attitude FDAI
		Text(skp, 40, 15, GC->rtcc->LOSTDisplayBuffer[53]); //Presently Stored Attitude TLM
		Text(skp, 47, 15, GC->rtcc->LOSTDisplayBuffer[54]); //Presently Stored Attitude TLM
		Text(skp, 54, 15, GC->rtcc->LOSTDisplayBuffer[55]); //Presently Stored Attitude TLM
		Text(skp, 40, 16, GC->rtcc->LOSTDisplayBuffer[56]); //Presently Stored Attitude MED
		Text(skp, 47, 16, GC->rtcc->LOSTDisplayBuffer[57]); //Presently Stored Attitude MED
		Text(skp, 54, 16, GC->rtcc->LOSTDisplayBuffer[58]); //Presently Stored Attitude MED
		Text(skp, 17, 18, GC->rtcc->LOSTDisplayBuffer[59]); //MATRIX 1 ID
		Text(skp, 46, 18, GC->rtcc->LOSTDisplayBuffer[60]); //MATRIX 1 GET
		Text(skp, 17, 19, GC->rtcc->LOSTDisplayBuffer[61]); //MATRIX 1 X/XE
		Text(skp, 36, 19, GC->rtcc->LOSTDisplayBuffer[62]); //MATRIX 1 X/YE
		Text(skp, 54, 19, GC->rtcc->LOSTDisplayBuffer[63]); //MATRIX 1 X/ZE
		Text(skp, 17, 20, GC->rtcc->LOSTDisplayBuffer[64]); //MATRIX 1 Y/XE
		Text(skp, 36, 20, GC->rtcc->LOSTDisplayBuffer[65]); //MATRIX 1 Y/YE
		Text(skp, 54, 20, GC->rtcc->LOSTDisplayBuffer[66]); //MATRIX 1 Y/ZE
		Text(skp, 17, 21, GC->rtcc->LOSTDisplayBuffer[67]); //MATRIX 1 Z/XE
		Text(skp, 36, 21, GC->rtcc->LOSTDisplayBuffer[68]); //MATRIX 1 Z/YE
		Text(skp, 54, 21, GC->rtcc->LOSTDisplayBuffer[69]); //MATRIX 1 Z/ZE
		Text(skp, 17, 23, GC->rtcc->LOSTDisplayBuffer[70]); //MATRIX 2 ID
		Text(skp, 46, 23, GC->rtcc->LOSTDisplayBuffer[71]); //MATRIX 2 GET
		Text(skp, 17, 24, GC->rtcc->LOSTDisplayBuffer[72]); //MATRIX 2 X/XE
		Text(skp, 36, 24, GC->rtcc->LOSTDisplayBuffer[73]); //MATRIX 2 X/YE
		Text(skp, 54, 24, GC->rtcc->LOSTDisplayBuffer[74]); //MATRIX 2 X/ZE
		Text(skp, 17, 25, GC->rtcc->LOSTDisplayBuffer[75]); //MATRIX 2 Y/XE
		Text(skp, 36, 25, GC->rtcc->LOSTDisplayBuffer[76]); //MATRIX 2 Y/YE
		Text(skp, 54, 25, GC->rtcc->LOSTDisplayBuffer[77]); //MATRIX 2 Y/ZE
		Text(skp, 17, 26, GC->rtcc->LOSTDisplayBuffer[78]); //MATRIX 2 Z/XE
		Text(skp, 36, 26, GC->rtcc->LOSTDisplayBuffer[79]); //MATRIX 2 Z/YE
		Text(skp, 54, 26, GC->rtcc->LOSTDisplayBuffer[80]); //MATRIX 2 Z/ZE
		break;
	case 127:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "DEBUG", 5);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		CSMOrLMSelection(skp);
		if (IsCSM == false)
		{
			if (G->DebugLMComputer) skp->Text(CW, 4 * H / 14, "LGC", 3);
			else skp->Text(CW, 4 * H/ 14, "AGS", 3);
		}
		skp->Text(CW, 9 * H / 21, "Current REFSMMAT:", 17);
		{
			REFSMMATData *refsdata;
			if (IsCSM)
			{
				refsdata = &GC->rtcc->EZJGMTX1.data[0];
			}
			else
			{
				refsdata = &GC->rtcc->EZJGMTX3.data[0];
			}
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			GC->rtcc->FormatREFSMMATCode(RTCC_REFSMMAT_TYPE_CUR, refsdata->ID, Buffer);
			skp->Text(CW, 10 * H / 21, Buffer, strlen(Buffer));
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		skp->Text(W - CW, 5 * H / 14, "Misalignment:", 12);
		sprintf(Buffer, "%+.4lf°", G->DebugIMUTorquingAngles.x*DEG);
		skp->Text(W - CW, 6 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+.4lf°", G->DebugIMUTorquingAngles.y*DEG);
		skp->Text(W - CW, 7 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "%+.4lf°", G->DebugIMUTorquingAngles.z*DEG);
		skp->Text(W - CW, 8 * H / 14, Buffer, strlen(Buffer));
		break;
	case 128:
		AGOPDisplay(skp);
		break;
	case 129:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "Auxiliary Computing Facility", 28);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text(CW, 2 * H / 14, "AGOP", 4);
		skp->Text(CW, 4 * H / 14, "VECPOINT", 8);
		break;
	case 130:
		skp->SetFont(font_menu3);
		GetCharSize(skp, CW, CH);
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		if (GC->MEDInputData.size() != 0U && GC->MEDInputData.size() > ActiveMEDInputPage)
		{
			skp->Text(W / 2, CH / 2, GC->MEDInputData[ActiveMEDInputPage].Title.c_str(), GC->MEDInputData[ActiveMEDInputPage].Title.size());
			skp->SetTextAlign(oapi::Sketchpad::LEFT);

			skp->Text(CW, CH * (marker + 4), "*", 1);

			for (unsigned i = 0; i < GC->MEDInputData[ActiveMEDInputPage].table.size(); i++)
			{
				Text_String(skp, CW * 2, CH * (i + 4), GC->MEDInputData[ActiveMEDInputPage].table[i].Label);
				Text_String(skp, CW * 22, CH * (i + 4), GC->MEDInputData[ActiveMEDInputPage].table[i].Data);
				Text_String(skp, CW * 36, CH * (i + 4), GC->MEDInputData[ActiveMEDInputPage].table[i].Unit);
			}
		}
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		Text_String(skp, W / 2, H - CH, GC->rtcc->RTCCONLINEMON.LastMEDMessage);
		break;
	case 131:
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(W / 2, CH / 2, "IMU Parking Angles", 18);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		PrintLMVessel(Buffer, false);
		skp->Text(CW, 2 * H / 14, Buffer, strlen(Buffer));
		skp->Text(CW, 4 * H / 14, "Octal Values:", 13);
		for (int i = 0; i < 6; i++)
		{
			sprintf(Buffer, "%05o", G->GravVec[i]);
			skp->Text(CW, (5 + i) * H / 14, Buffer, strlen(Buffer));
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		skp->Text(W - CW, 5 * H / 14, "IMU Angles:", 11);
		sprintf(Buffer, "OGA: %+07.2lf°", G->IMUParkingAngles.x * DEG);
		skp->Text(W - CW, 6 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "IGA: %+07.2lf°", G->IMUParkingAngles.y * DEG);
		skp->Text(W - CW, 7 * H / 14, Buffer, strlen(Buffer));
		sprintf(Buffer, "MGA: %+07.2lf°", G->IMUParkingAngles.z * DEG);
		skp->Text(W - CW, 8 * H / 14, Buffer, strlen(Buffer));
		break;
	case 132:
		if (subscreen == 0)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text(W / 2, CH / 2, "Recovery Ascnd Node Input", 26);
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			x = 1;  y = 3; dx = 7;
			Text(skp, x, marker + y, "*");
			x++;
			Text(skp, x, y, "VEH:");
			if (GC->rtcc->EZETVMED.RecovAscNodeVehID == 1) Text(skp, x + dx, y, "CSM");
			else Text(skp, x + dx, y, "LEM");
			y++;
			Text(skp, x, y, "OPT:");
			if (GC->rtcc->EZETVMED.RecovAscNodeOption == 1) Text(skp, x + dx, y, "Revs");
			else Text(skp, x + dx, y, "Times");
			y++;
			if (GC->rtcc->EZETVMED.RecovAscNodeOption == 2)
			{
				Text(skp, x, y, "TIME1:");
				Text_GET_HHHMMSS(skp, x + dx, y, GC->rtcc->EZETVMED.RecovAscNodeBeginTime);
				y++;
				Text(skp, x, y, "TIME2:");
				Text_GET_HHHMMSS(skp, x + dx, y, GC->rtcc->EZETVMED.RecovAscNodeEndTime);
				y++;
			}
			else y += 2;
			if (GC->rtcc->EZETVMED.RecovAscNodeOption == 1)
			{
				Text(skp, x, y, "REV1:");
				Text(skp, x + dx, y, "%d", GC->rtcc->EZETVMED.RecovAscNodeBeginRev);
				y++;
				Text(skp, x, y, "REV2:");
				Text(skp, x + dx, y, "%d", GC->rtcc->EZETVMED.RecovAscNodeEndRev);
				y++;
			}
			else y += 2;
			Text(skp, x, y, "REF:");
			switch (GC->rtcc->EZETVMED.RecovAscNodeCoordinates)
			{
			case 1: Text(skp, x + dx, y, "ECT"); break;
			case 2: Text(skp, x + dx, y, "MCI"); break;
			case 3: Text(skp, x + dx, y, "MCT"); break;
			}
		}
		else
		{
			SetMOCRFont(skp, 4, false);
			GetCharSize(skp, CW, CH);
			SetMOCRDisplayCentered(4);
			Text(skp, 8, 0, "RECOVERY ASCENDING NODE");
			Text(skp, 38, 0, "1505");
			Text(skp, 2, 2, "VEH");
			Text(skp, 12, 2, "STA ID");
			Text(skp, 29, 2, "REF");
			Text(skp, 1, 4, "REV     GET        GMT       LNG     RAS");
			skp->SetTextAlign(oapi::Sketchpad::RIGHT);
			SetMOCRFont(skp, 4, true);
			Text(skp, 9, 2, GC->rtcc->RZASCND.VehicleName);
			Text(skp, 26, 2, GC->rtcc->RZASCND.StationID);
			Text(skp, 36, 2, GC->rtcc->RZASCND.REF);
			for (int i = 0; i < GC->rtcc->RZASCND.TotalNumEntries; i++)
			{
				Text(skp, 4, 5 + i, "%d", GC->rtcc->RZASCND.table[i].Rev);
				Text_GET_HHHMMSS(skp, 15, 5 + i, GC->rtcc->RZASCND.table[i].GET);
				Text_GET_HHHMMSS(skp, 26, 5 + i, GC->rtcc->RZASCND.table[i].GMT);
				Text_Longitude(skp, 35, 5 + i, GC->rtcc->RZASCND.table[i].Longitude, 2);
				Text_GET_HHMM(skp, 42, 5 + i, GC->rtcc->RZASCND.table[i].RightAscension);
			}
			Text(skp, 35, 20, GC->rtcc->RZASCND.ErrorMessage);
		}
		break;
	}

	return true;
}

void ApolloRTCCMFD::AGOPDisplay(oapi::Sketchpad* skp)
{
	skp->SetTextAlign(oapi::Sketchpad::CENTER);
	skp->Text(W / 2, CH / 2, "Apollo Generalized Optics Program", 33);
	skp->SetTextAlign(oapi::Sketchpad::LEFT);

	if (subscreen == 1)
	{
		skp->Text(CW, (marker + 3) * H / 22, "*", 1);

		switch (GC->AGOP_Option)
		{
		case 1:
			AGOPDisplayOption1(skp);
			break;
		case 2:
			AGOPDisplayOption2(skp);
			break;
		case 3:
			AGOPDisplayOption3(skp);
			break;
		case 4:
			AGOPDisplayOption4(skp);
			break;
		case 5:
			AGOPDisplayOption5(skp);
			break;
		case 6:
			AGOPDisplayOption6(skp);
			break;
		case 7:
			AGOPDisplayOption7(skp);
			break;
		case 8:
			skp->Text(CW * 2, 3 * H / 22, "8: Star Sighting Table", 22);
			break;
		case 9:
			skp->Text(CW * 2, 3 * H / 22, "9: Lunar Surface Alignment", 26);
			break;
		}
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		skp->Text(W - CW, 2 * H / 14, "1/2", 3);
	}
	else
	{
		skp->SetFont(font_mocr3);
		GetCharSize(skp, CW, CH);
		for (unsigned i = 0; i < GC->AGOP_Output.size(); i++)
		{
			Text(skp, 1, 8 + i, GC->AGOP_Output[i]);
			if (i >= 18) break;
		}

		skp->Text(CW * 10, H - CH, GC->AGOP_Error.c_str(), GC->AGOP_Error.size());
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		skp->Text(W - CW, 2 * H / 14, "2/2", 3);
	}
}

void ApolloRTCCMFD::AGOPDisplayOption1(oapi::Sketchpad*skp)
{
	skp->Text(CW * 2, 3 * H / 22, "1: Cislunar Navigation", 22);

	switch (GC->AGOP_Mode)
	{
	case 1:
		skp->Text(CW * 2, 4 * H / 22, "1: Star/Earth Horizon", 21);
		break;
	case 2:
		skp->Text(CW * 2, 4 * H / 22, "2: Star/Moon Horizon", 20);
		break;
	case 3:
		skp->Text(CW * 2, 4 * H / 22, "3: Star/Earth Landmark", 22);
		break;
	case 4:
		skp->Text(CW * 2, 4 * H / 22, "4: Star/Moon Landmark", 21);
		break;
	}

	skp->Text(CW * 2, 5 * H / 22, "Start:", 6);
	GET_Display(Buffer, GC->AGOP_StartTime);
	skp->Text(CW * 9, 5 * H / 22, Buffer, strlen(Buffer));

	skp->Text(CW * 2, 6 * H / 22, "Stop:", 5);
	GET_Display(Buffer, GC->AGOP_StopTime);
	skp->Text(CW * 9, 6 * H / 22, Buffer, strlen(Buffer));

	skp->Text(CW * 2, 7 * H / 22, "DT:", 3);
	sprintf(Buffer, "%.1lf min", GC->AGOP_TimeStep);
	skp->Text(CW * 9, 7 * H / 22, Buffer, strlen(Buffer));

	skp->Text(CW * 2, 8 * H / 22, "CSM REFSMMAT:", 13);
	GC->rtcc->EMGSTGENName(GC->AGOP_CSM_REFSMMAT, Buffer);
	skp->Text(CW * 16, 8 * H / 22, Buffer, strlen(Buffer));

	sprintf(Buffer, "Star: %d (Oct: %o)", GC->AGOP_Stars[0], GC->AGOP_Stars[0]);
	skp->Text(CW * 2, 10 * H / 22, Buffer, strlen(Buffer));

	if (GC->AGOP_Mode == 3 || GC->AGOP_Mode == 4)
	{
		sprintf(Buffer, "Lat: %.3lf°", GC->AGOP_Lat*DEG);
		skp->Text(CW * 2, 11 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "Lng: %.3lf°", GC->AGOP_Lng*DEG);
		skp->Text(CW * 2, 12 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "Alt: %.2lf NM", GC->AGOP_Alt / 1852.0);
		skp->Text(CW * 2, 13 * H / 22, Buffer, strlen(Buffer));
	}
}

void ApolloRTCCMFD::AGOPDisplayOption2(oapi::Sketchpad*skp)
{
	skp->Text(CW * 2, 3 * H / 22, "2: Reference Body", 17);

	switch (GC->AGOP_Mode)
	{
	case 1:
		skp->Text(CW * 2, 4 * H / 22, "1: General", 10);
		break;
	case 2:
		skp->Text(CW * 2, 4 * H / 22, "2: Center of Earth", 18);
		break;
	case 3:
		skp->Text(CW * 2, 4 * H / 22, "3: Center of Moon", 17);
		break;
	case 4:
		skp->Text(CW * 2, 4 * H / 22, "4: Center of Sun", 16);
		break;
	case 5:
		skp->Text(CW * 2, 4 * H / 22, "5: Earth Landmark", 17);
		break;
	case 6:
		skp->Text(CW * 2, 4 * H / 22, "6: Moon Landmark", 16);
		break;
	}

	skp->Text(CW * 2, 5 * H / 22, "Start:", 6);
	GET_Display(Buffer, GC->AGOP_StartTime);
	skp->Text(CW * 9, 5 * H / 22, Buffer, strlen(Buffer));

	//Time interval required for all modes except mode 1 (general)
	if (GC->AGOP_Mode != 1)
	{
		skp->Text(CW * 2, 6 * H / 22, "Stop:", 5);
		GET_Display(Buffer, GC->AGOP_StopTime);
		skp->Text(CW * 9, 6 * H / 22, Buffer, strlen(Buffer));

		skp->Text(CW * 2, 7 * H / 22, "DT:", 3);
		sprintf(Buffer, "%.1lf min", GC->AGOP_TimeStep);
		skp->Text(CW * 9, 7 * H / 22, Buffer, strlen(Buffer));
	}

	//Landmark modes
	if (GC->AGOP_Mode == 5 || GC->AGOP_Mode == 6)
	{
		sprintf(Buffer, "Lat: %.3lf°", GC->AGOP_Lat*DEG);
		skp->Text(CW * 2, 11 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "Lng: %.3lf°", GC->AGOP_Lng*DEG);
		skp->Text(CW * 2, 12 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "Alt: %.2lf NM", GC->AGOP_Alt / 1852.0);
		skp->Text(CW * 2, 13 * H / 22, Buffer, strlen(Buffer));
	}
}

void ApolloRTCCMFD::AGOPDisplayOption3(oapi::Sketchpad*skp)
{
	skp->Text(CW * 2, 3 * H / 22, "3: Star Catalog", 15);

	sprintf(Buffer, "Star: %d (Oct: %o)", GC->AGOP_Stars[0], GC->AGOP_Stars[0]);
	skp->Text(CW * 2, 10 * H / 22, Buffer, strlen(Buffer));
}

void ApolloRTCCMFD::AGOPDisplayOption4(oapi::Sketchpad*skp)
{
	skp->Text(CW * 2, 3 * H / 22, "4: Antenna Pointing", 19);

	switch (GC->AGOP_Mode)
	{
	case 1:
		skp->Text(CW * 2, 4 * H / 22, "1: S-Band Hi-Gain (Movable)", 27);
		break;
	case 2:
		skp->Text(CW * 2, 4 * H / 22, "2: S-Band Steerable (Movable)", 29);
		break;
	case 3:
		skp->Text(CW * 2, 4 * H / 22, "3: Rendezvous Radar (Movable)", 29);
		break;
	case 4:
		skp->Text(CW * 2, 4 * H / 22, "4: S-Band Hi-Gain (Fixed)", 25);
		break;
	case 5:
		skp->Text(CW * 2, 4 * H / 22, "5: S-Band Steerable (Fixed)", 27);
		break;
	case 6:
		skp->Text(CW * 2, 4 * H / 22, "6: Rendezvous Radar (Fixed)", 27);
		break;
	}

	skp->Text(CW * 2, 5 * H / 22, "Start:", 6);
	GET_Display(Buffer, GC->AGOP_StartTime);
	skp->Text(CW * 9, 5 * H / 22, Buffer, strlen(Buffer));

	skp->Text(CW * 2, 6 * H / 22, "Stop:", 5);
	GET_Display(Buffer, GC->AGOP_StopTime);
	skp->Text(CW * 9, 6 * H / 22, Buffer, strlen(Buffer));

	skp->Text(CW * 2, 7 * H / 22, "DT:", 3);
	sprintf(Buffer, "%.1lf min", GC->AGOP_TimeStep);
	skp->Text(CW * 9, 7 * H / 22, Buffer, strlen(Buffer));

	//CSM REFSMMAT required
	if (GC->AGOP_Mode == 1 || GC->AGOP_Mode == 4 || GC->AGOP_AttIsCSM)
	{
		skp->Text(CW * 2, 8 * H / 22, "CSM REFSMMAT:", 13);
		GC->rtcc->EMGSTGENName(GC->AGOP_CSM_REFSMMAT, Buffer);
		skp->Text(CW * 16, 8 * H / 22, Buffer, strlen(Buffer));
	}

	//LM REFSMMAT required
	if (GC->AGOP_Mode == 2 || GC->AGOP_Mode == 3 || GC->AGOP_Mode == 5 || GC->AGOP_Mode == 6 || !GC->AGOP_AttIsCSM)
	{
		skp->Text(CW * 2, 9 * H / 22, "LM REFSMMAT:", 12);
		GC->rtcc->EMGSTGENName(GC->AGOP_LM_REFSMMAT, Buffer);
		skp->Text(CW * 16, 9 * H / 22, Buffer, strlen(Buffer));
	}

	//Landmark
	sprintf(Buffer, "Lat: %.3lf°", GC->AGOP_Lat*DEG);
	skp->Text(CW * 2, 11 * H / 22, Buffer, strlen(Buffer));
	sprintf(Buffer, "Lng: %.3lf°", GC->AGOP_Lng*DEG);
	skp->Text(CW * 2, 12 * H / 22, Buffer, strlen(Buffer));
	sprintf(Buffer, "Alt: %.2lf NM", GC->AGOP_Alt / 1852.0);
	skp->Text(CW * 2, 13 * H / 22, Buffer, strlen(Buffer));

	if (GC->AGOP_Mode >= 4)
	{
		//Fixed antenna position
		if (GC->AGOP_HeadsUp)
		{
			skp->Text(CW * 2, 10 * H / 22, "Heads Up", 8);
		}
		else
		{
			skp->Text(CW * 2, 10 * H / 22, "Heads Down", 10);
		}

		sprintf(Buffer, "PCH: %+.2lf", GC->AGOP_AntennaPitch * DEG);
		skp->Text(CW * 2, 17 * H / 22, Buffer, strlen(Buffer));
		sprintf(Buffer, "YAW: %+.2lf", GC->AGOP_AntennaYaw * DEG);
		skp->Text(CW * 2, 18 * H / 22, Buffer, strlen(Buffer));
	}
	else
	{
		//Fixed attitude
		if (GC->AGOP_AttIsCSM)
		{
			skp->Text(CW * 2, 14 * H / 22, "CSM IMU:", 8);
		}
		else
		{
			skp->Text(CW * 2, 14 * H / 22, "LM IMU:", 7);
		}

		sprintf(Buffer, "%+07.2lf %+07.2lf %+07.2lf", GC->AGOP_Attitudes[0].data[0] * DEG, GC->AGOP_Attitudes[0].data[1] * DEG, GC->AGOP_Attitudes[0].data[2] * DEG);
		skp->Text(CW * 2, 15 * H / 22, Buffer, strlen(Buffer));
	}
}

void ApolloRTCCMFD::AGOPDisplayOption5(oapi::Sketchpad*skp)
{
	skp->Text(CW * 2, 3 * H / 22, "5: Passive Thermal Control", 26);

	skp->Text(CW * 2, 5 * H / 22, "Start:", 6);
	GET_Display(Buffer, GC->AGOP_StartTime);
	skp->Text(CW * 9, 5 * H / 22, Buffer, strlen(Buffer));

	skp->Text(CW * 2, 6 * H / 22, "Stop:", 5);
	GET_Display(Buffer, GC->AGOP_StopTime);
	skp->Text(CW * 9, 6 * H / 22, Buffer, strlen(Buffer));

	skp->Text(CW * 2, 7 * H / 22, "DT:", 3);
	sprintf(Buffer, "%.1lf min", GC->AGOP_TimeStep);
	skp->Text(CW * 9, 7 * H / 22, Buffer, strlen(Buffer));

	skp->Text(CW * 2, 8 * H / 22, "CSM REFSMMAT:", 13);
	GC->rtcc->EMGSTGENName(GC->AGOP_CSM_REFSMMAT, Buffer);
	skp->Text(CW * 16, 8 * H / 22, Buffer, strlen(Buffer));
}

void ApolloRTCCMFD::AGOPDisplayOption6(oapi::Sketchpad*skp)
{
	skp->Text(CW * 2, 3 * H / 22, "6: CSM Horizon Check", 20);

	if (GC->AGOP_Mode == 1)
	{
		skp->Text(CW * 2, 4 * H / 22, "1: Fwd Horizon", 14);
	}
	else
	{
		skp->Text(CW * 2, 4 * H / 22, "2: Aft Horizon", 14);
	}

	skp->Text(CW * 2, 5 * H / 22, "Start:", 6);
	GET_Display(Buffer, GC->AGOP_StartTime);
	skp->Text(CW * 9, 5 * H / 22, Buffer, strlen(Buffer));

	skp->Text(CW * 2, 6 * H / 22, "Stop:", 5);
	GET_Display(Buffer, GC->AGOP_StopTime);
	skp->Text(CW * 9, 6 * H / 22, Buffer, strlen(Buffer));

	skp->Text(CW * 2, 7 * H / 22, "DT:", 3);
	sprintf(Buffer, "%.1lf min", GC->AGOP_TimeStep);
	skp->Text(CW * 9, 7 * H / 22, Buffer, strlen(Buffer));

	if (GC->AGOP_HeadsUp)
	{
		skp->Text(CW * 2, 10 * H / 22, "Heads Up", 8);
	}
	else
	{
		skp->Text(CW * 2, 10 * H / 22, "Heads Down", 10);
	}
}

void ApolloRTCCMFD::AGOPDisplayOption7(oapi::Sketchpad*skp)
{
	skp->Text(CW * 2, 3 * H / 22, "7: Optical Support Table", 24);
	switch (GC->AGOP_Mode)
	{
	case 1:
		skp->Text(CW * 2, 4 * H / 22, "1: LM Horizon Check", 27);
		break;
	case 2:
		skp->Text(CW * 2, 4 * H / 22, "2: Alignment and Maneuver Check", 31);
		break;
	case 3:
		skp->Text(CW * 2, 4 * H / 22, "3: Compute REFSMMAT", 19);
		break;
	case 4:
		skp->Text(CW * 2, 4 * H / 22, "4: Docked Alignment", 19);
		break;
	case 5:
		skp->Text(CW * 2, 4 * H / 22, "5: Point AOT with CSM", 21);
		break;
	case 6:
		skp->Text(CW * 2, 4 * H / 22, "6: REFSMMAT to REFSMMAT", 23);
		break;
	}

	//Start time required for modes 1-2 and 5
	if (GC->AGOP_Mode <= 2 || GC->AGOP_Mode == 5)
	{
		skp->Text(CW * 2, 5 * H / 22, "Start:", 6);
		GET_Display(Buffer, GC->AGOP_StartTime);
		skp->Text(CW * 9, 5 * H / 22, Buffer, strlen(Buffer));

		//Stop time required for mode 2
		if (GC->AGOP_Mode == 2)
		{
			skp->Text(CW * 2, 6 * H / 22, "Stop:", 5);
			GET_Display(Buffer, GC->AGOP_StopTime);
			skp->Text(CW * 9, 6 * H / 22, Buffer, strlen(Buffer));
		}
	}

	if (GC->AGOP_Mode == 3)
	{
		//TBD: Star search or input stars
	}
	else if (GC->AGOP_Mode == 4)
	{
		switch (GC->AGOP_AdditionalOption)
		{
		case 0:
			skp->Text(CW * 2, 5 * H / 22, "0: Calc. LM REFSMMAT", 20);
			break;
		case 1:
			skp->Text(CW * 2, 5 * H / 22, "1: Calc. LM Gimbal Angles", 25);
			break;
		case 2:
			skp->Text(CW * 2, 5 * H / 22, "2: Calc. CSM Gimbal Angles", 26);
			break;
		case 3:
			skp->Text(CW * 2, 5 * H / 22, "3: Calc. CSM REFSMMAT", 21);
			break;
		}
	}

	//Logic to get required REFSMMATs
	bool GetCSMREFSMMAT, GetLMREFSMMAT;

	GetCSMREFSMMAT = GC->AGOP_CSM_REFSMMAT_Required();
	GetLMREFSMMAT = GC->AGOP_LM_REFSMMAT_Required();

	if (GetCSMREFSMMAT)
	{
		if (GC->AGOP_Mode == 6)
		{
			skp->Text(CW * 2, 8 * H / 22, "Current REFSMMAT:", 17);
		}
		else
		{
			skp->Text(CW * 2, 8 * H / 22, "CSM REFSMMAT:", 13);
		}

		GC->rtcc->EMGSTGENName(GC->AGOP_CSM_REFSMMAT, Buffer);
		skp->Text(CW * 22, 8 * H / 22, Buffer, strlen(Buffer));
	}

	if (GetLMREFSMMAT)
	{
		if (GC->AGOP_Mode == 6)
		{
			skp->Text(CW * 2, 9 * H / 22, "Preferred REFSMMAT:", 19);
		}
		else
		{
			skp->Text(CW * 2, 9 * H / 22, "LM REFSMMAT:", 12);
		}
		GC->rtcc->EMGSTGENName(GC->AGOP_LM_REFSMMAT, Buffer);
		skp->Text(CW * 22, 9 * H / 22, Buffer, strlen(Buffer));
	}

	if (GC->AGOP_Mode == 5)
	{
		sprintf(Buffer, "Star: %d (Oct: %o)", GC->AGOP_Stars[0], GC->AGOP_Stars[0]);
		skp->Text(CW * 2, 10 * H / 22, Buffer, strlen(Buffer));
	}
	else if (GC->AGOP_Mode == 3)
	{
		sprintf(Buffer, "Star 1: %d (Oct: %o), Star 2: %d (Oct: %o)", GC->AGOP_Stars[0], GC->AGOP_Stars[0], GC->AGOP_Stars[1], GC->AGOP_Stars[1]);
		skp->Text(CW * 2, 10 * H / 22, Buffer, strlen(Buffer));
	}

	//First attitude set
	if (GC->AGOP_Mode == 1)
	{
		//LM Horizon Check
		skp->Text(CW * 2, 14 * H / 22, "LM IMU:", 7);

		sprintf(Buffer, "%+07.2lf %+07.2lf %+07.2lf", GC->AGOP_Attitudes[0].data[0] * DEG, GC->AGOP_Attitudes[0].data[1] * DEG, GC->AGOP_Attitudes[0].data[2] * DEG);
		skp->Text(CW * 2, 15 * H / 22, Buffer, strlen(Buffer));
	}
	else if (GC->AGOP_Mode == 2 || GC->AGOP_Mode == 3 || GC->AGOP_Mode == 6)
	{
		//Alignment and Maneuver Check / Compute REFSMMAT / REFSMMAT to REFSMMAT
		if (GC->AGOP_AttIsCSM)
		{
			skp->Text(CW * 2, 14 * H / 22, "CSM IMU:", 8);
		}
		else
		{
			skp->Text(CW * 2, 14 * H / 22, "LM IMU:", 7);
		}

		sprintf(Buffer, "%+07.2lf %+07.2lf %+07.2lf", GC->AGOP_Attitudes[0].data[0] * DEG, GC->AGOP_Attitudes[0].data[1] * DEG, GC->AGOP_Attitudes[0].data[2] * DEG);
		skp->Text(CW * 2, 15 * H / 22, Buffer, strlen(Buffer));

		if (GC->AGOP_Mode == 3)
		{
			//Second attitude set for REFSMMAT computation
			sprintf(Buffer, "%+07.2lf %+07.2lf %+07.2lf", GC->AGOP_Attitudes[1].data[0] * DEG, GC->AGOP_Attitudes[1].data[1] * DEG, GC->AGOP_Attitudes[1].data[2] * DEG);
			skp->Text(CW * 2, 16 * H / 22, Buffer, strlen(Buffer));
		}
	}
	else if (GC->AGOP_Mode == 4)
	{
		//Docked Alignment
		skp->Text(CW * 2, 14 * H / 22, "Attitudes:", 10);

		if (GC->AGOP_AdditionalOption != 2)
		{
			sprintf(Buffer, "CSM: %+07.2lf %+07.2lf %+07.2lf", GC->AGOP_Attitudes[0].data[0] * DEG, GC->AGOP_Attitudes[0].data[1] * DEG, GC->AGOP_Attitudes[0].data[2] * DEG);
			skp->Text(CW * 2, 15 * H / 22, Buffer, strlen(Buffer));
		}

		if (GC->AGOP_AdditionalOption != 1)
		{
			sprintf(Buffer, "LM: %+07.2lf %+07.2lf %+07.2lf", GC->AGOP_Attitudes[1].data[0] * DEG, GC->AGOP_Attitudes[1].data[1] * DEG, GC->AGOP_Attitudes[1].data[2] * DEG);
			skp->Text(CW * 2, 16 * H / 22, Buffer, strlen(Buffer));
		}
	}

	//Instrument data for REFSMMAT calculation
	if (GC->AGOP_Mode == 3)
	{
		if (GC->AGOP_Instrument == 0)
		{
			sprintf(Buffer, "SFT: %+07.2lf° TRN: %+07.3lf°", GC->AGOP_InstrumentAngles1[0] * DEG, GC->AGOP_InstrumentAngles1[1] * DEG);
			skp->Text(CW * 2, 17 * H / 22, Buffer, strlen(Buffer));
			sprintf(Buffer, "SFT: %+07.2lf° TRN: %+07.3lf°", GC->AGOP_InstrumentAngles2[0] * DEG, GC->AGOP_InstrumentAngles2[1] * DEG);
			skp->Text(CW * 2, 18 * H / 22, Buffer, strlen(Buffer));
		}
		else if (GC->AGOP_Instrument == 1 || GC->AGOP_Instrument == 3)
		{
			sprintf(Buffer, "SPA: %+05.1lf° SXP: %+04.1lf°", GC->AGOP_InstrumentAngles1[0] * DEG, GC->AGOP_InstrumentAngles1[1] * DEG);
			skp->Text(CW * 2, 17 * H / 22, Buffer, strlen(Buffer));
			sprintf(Buffer, "SPA: %+05.1lf° SXP: %+04.1lf°", GC->AGOP_InstrumentAngles2[0] * DEG, GC->AGOP_InstrumentAngles2[1] * DEG);
			skp->Text(CW * 2, 18 * H / 22, Buffer, strlen(Buffer));
		}
		else
		{
			sprintf(Buffer, "A1: %+07.2lf° A2: %+07.2lf°", GC->AGOP_InstrumentAngles1[0] * DEG, GC->AGOP_InstrumentAngles1[1] * DEG);
			skp->Text(CW * 2, 17 * H / 22, Buffer, strlen(Buffer));
			sprintf(Buffer, "A1: %+07.2lf° A2: %+07.2lf°", GC->AGOP_InstrumentAngles2[0] * DEG, GC->AGOP_InstrumentAngles2[1] * DEG);
			skp->Text(CW * 2, 18 * H / 22, Buffer, strlen(Buffer));
		}
	}

	//Modes 2 and 3 need instrument specified
	if (GC->AGOP_Mode == 2 || GC->AGOP_Mode == 3)
	{
		skp->Text(CW * 2, 19 * H / 22, "Instrument:", 11);

		switch (GC->AGOP_Instrument)
		{
		case 0:
			skp->Text(CW * 14, 19 * H / 22, "Sextant", 7);
			break;
		case 1:
			skp->Text(CW * 14, 19 * H / 22, "LM COAS", 7);
			break;
		case 2:
			skp->Text(CW * 14, 19 * H / 22, "AOT", 3);
			break;
		case 3:
			skp->Text(CW * 14, 19 * H / 22, "CSM COAS", 8);
			break;
		}

		if (GC->AGOP_Instrument == 1)
		{
			if (GC->AGOP_LMCOASAxis)
			{
				skp->Text(CW * 2, 20 * H / 22, "Axis: +Z", 8);
			}
			else
			{
				skp->Text(CW * 2, 20 * H / 22, "Axis: +X", 8);
			}
		}
		else if (GC->AGOP_Instrument == 2)
		{
			sprintf(Buffer, "Detent: %d", GC->AGOP_LMAOTDetent);
			skp->Text(CW * 2, 20 * H / 22, Buffer, strlen(Buffer));
		}
	}
	else if (GC->AGOP_Mode == 5)
	{
		//Point AOT with CSM
		sprintf(Buffer, "Detent: %d", GC->AGOP_LMAOTDetent);
		skp->Text(CW * 2, 20 * H / 22, Buffer, strlen(Buffer));
	}
}

void ApolloRTCCMFD::AGOPDisplayOption8(oapi::Sketchpad*skp)
{
	//TBD
}

void ApolloRTCCMFD::AGOPDisplayOption9(oapi::Sketchpad*skp)
{
	//TBD
}

void ApolloRTCCMFD::RendezvousEvaluationDisplay(oapi::Sketchpad*skp)
{
	SetMOCRFont(skp, 3, false);
	GetCharSize(skp, CW, CH);
	SetMOCRDisplayCentered(3);
	Text(skp, 13, 0, "RENDEZVOUS EVALUATION DISPLAY");
	Text(skp, 52, 0, "0058");
	Text(skp, 15, 2, "ID");
	Text(skp, 35, 2, "M");
	Text(skp, 3, 4, "GET");
	Text(skp, 11, 4, "DT");
	Text(skp, 17, 4, "DV");
	Text(skp, 21, 4, "VEH");
	Text(skp, 25, 4, "ID");
	Text(skp, 29, 4, "PHASE");
	Text(skp, 38, 4, "DH");
	Text(skp, 45, 4, "HA");
	Text(skp, 52, 4, "HP");
	Text(skp, 5, 18, "PITCH     YAW      VX      VY      VZ");
	skp->SetTextAlign(oapi::Sketchpad::RIGHT);
	SetMOCRFont(skp, 3, true);
	Text(skp, 21, 2, "%d", GC->rtcc->PZREDT.ID);
	if (GC->rtcc->PZREDT.isDKI)
	{
		Text(skp, 40, 2, "%d", GC->rtcc->PZREDT.M);
	}
	Text(skp, 40, 27, GC->rtcc->PZREDT.ErrorMessage);
	for (int i = 0; i < GC->rtcc->PZREDT.NumMans; i++)
	{
		Text_GET_HHHMMSS(skp, 9, 5 + i * 2, GC->rtcc->PZREDT.GET[i]);
		if (i > 0)
		{
			Text_GET_MMSS(skp, 15, 4 + i * 2, GC->rtcc->PZREDT.DT[i]);
		}
		Text(skp, 21, 5 + i * 2, "%.1lf", GC->rtcc->PZREDT.DV[i]);
		Text(skp, 23, 5 + i * 2, GC->rtcc->PZREDT.VEH[i]);
		Text(skp, 27, 5 + i * 2, GC->rtcc->PZREDT.PURP[i]);
		Text(skp, 35, 5 + i * 2, "%.2lf", GC->rtcc->PZREDT.PHASE[i]);
		Text(skp, 42, 5 + i * 2, "%.1lf", GC->rtcc->PZREDT.HEIGHT[i]);
		Text(skp, 49, 5 + i * 2, "%.1lf", GC->rtcc->PZREDT.HA[i]);
		Text(skp, 56, 5 + i * 2, "%.1lf", GC->rtcc->PZREDT.HP[i]);
		Text(skp, 11, 19 + i, "%.3lf", GC->rtcc->PZREDT.Pitch[i]);
		Text(skp, 20, 19 + i, "%.3lf", GC->rtcc->PZREDT.Yaw[i]);
		Text(skp, 28, 19 + i, "%.1lf", GC->rtcc->PZREDT.DVVector[i].x);
		Text(skp, 36, 19 + i, "%.1lf", GC->rtcc->PZREDT.DVVector[i].y);
		Text(skp, 44, 19 + i, "%.1lf", GC->rtcc->PZREDT.DVVector[i].z);
	}
}

void ApolloRTCCMFD::CSMOrLMSelection(oapi::Sketchpad*skp)
{
	skp->SetTextAlign(oapi::Sketchpad::LEFT);
	if (IsCSM)
	{
		skp->Text(CW, 2 * H / 14, "CSM", 3);

		PrintCSMVessel(Buffer);
	}
	else
	{
		skp->Text(CW, 2 * H / 14, "LM", 2);

		PrintLMVessel(Buffer);
	}
	skp->SetTextAlign(oapi::Sketchpad::RIGHT);
	skp->Text(W - CW, 2 * H / 14, Buffer, strlen(Buffer));

	CSMOrLMSelectionErrorMessage(skp);
	skp->SetTextAlign(oapi::Sketchpad::LEFT);
}

void ApolloRTCCMFD::CSMOrLMSelectionErrorMessage(oapi::Sketchpad*skp)
{
	if (ErrorMessage == 1)
	{
		sprintf_s(Buffer, "No vessel selected!");
	}
	else if (ErrorMessage == 2)
	{
		sprintf_s(Buffer, "Wrong vessel type!");
	}
	else return;

	skp->SetTextAlign(oapi::Sketchpad::CENTER);
	skp->Text(W / 2, 21 * H / 22, Buffer, strlen(Buffer));
}

void ApolloRTCCMFD::PrintCSMVessel(char *Buffer, bool ShowCSM)
{
	if (GC->rtcc->pCSM != NULL)
	{
		if (ShowCSM)
		{
			sprintf_s(Buffer, 127, "CSM: %s", GC->rtcc->pCSM->GetName());
		}
		else
		{
			sprintf_s(Buffer, 127, GC->rtcc->pCSM->GetName());
		}
	}
	else
	{
		sprintf_s(Buffer, 127, "No CSM!");
	}
}

void ApolloRTCCMFD::PrintLMVessel(char *Buffer, bool ShowLM)
{
	if (GC->rtcc->pLM != NULL)
	{
		if (ShowLM)
		{
			sprintf_s(Buffer, 127, "LM: %s", GC->rtcc->pLM->GetName());
		}
		else
		{
			sprintf_s(Buffer, 127, GC->rtcc->pLM->GetName());
		}
	}
	else
	{
		sprintf_s(Buffer, 127, "No LM!");
	}
}

void ApolloRTCCMFD::PrintIUVessel(char *Buffer)
{
	if (G->iuvessel != NULL)
	{
		sprintf_s(Buffer, 127, "IU: %s", G->iuvessel->GetName());
	}
	else
	{
		sprintf_s(Buffer, 127, "No IU Vessel!");
	}
}

void ApolloRTCCMFD::PrintTargetVessel(char *Buffer)
{
	if (G->Rendezvous_Target != NULL)
	{
		sprintf_s(Buffer, 127, "TGT: %s", G->Rendezvous_Target->GetName());
	}
	else
	{
		sprintf_s(Buffer, 127, "No Target!");
	}
}

void ApolloRTCCMFD::PrintUllage(char *Buffer, int Thruster, bool Use4Jets, double Duration)
{
	if (Thruster == RTCC_ENGINETYPE_CSMSPS || Thruster == RTCC_ENGINETYPE_LMAPS || Thruster == RTCC_ENGINETYPE_LMDPS)
	{
		char Buff2[128];

		if (Use4Jets)
		{
			sprintf_s(Buff2, "4 Jets");
		}
		else
		{
			sprintf_s(Buff2, "2 Jets");
		}

		if (Duration < 0.0)
		{
			sprintf_s(Buffer, 127, "%s, default duration", Buff2);
		}
		else if (Duration > 1.0)
		{
			sprintf_s(Buffer, 127, "%s, %.1lfs", Buff2, Duration);
		}
		else if (Duration > 0.0)
		{
			sprintf_s(Buffer, 127, "Illegal ullage duration!");
		}
		else
		{
			sprintf_s(Buffer, 127, "No ullage");
		}
	}
	else
	{
		sprintf_s(Buffer, 127, "");
	}
}

void ApolloRTCCMFD::GetCharSize(oapi::Sketchpad*skp, int &CW, int &CH)
{
	DWORD charsize = skp->GetCharSize();
	CW = HIWORD(charsize);
	CH = LOWORD(charsize);
}

void ApolloRTCCMFD::SetMOCRFont(oapi::Sketchpad*skp, int size, bool dynamic)
{
	oapi::Font *temp = font_mocr1;

	if (dynamic)
	{
		switch (size)
		{
		case 1: temp = font_mocr1_dyn; break;
		case 2: temp = font_mocr2_dyn; break;
		case 3: temp = font_mocr3_dyn; break;
		case 4: temp = font_mocr4_dyn; break;
		case 5: temp = font_mocr5_dyn; break;
		}
	}
	else
	{
		switch (size)
		{
		case 1: temp = font_mocr1; break;
		case 2: temp = font_mocr2; break;
		case 3: temp = font_mocr3; break;
		case 4: temp = font_mocr4; break;
		case 5: temp = font_mocr5; break;
		}
	}
	skp->SetFont(temp);
	skp->SetTextColor(dynamic ? white_darker_col : white_bright_col); //White. Bright for static, dark for dynamic
}

void ApolloRTCCMFD::SetMOCRDisplayCentered(int size)
{
	//Call this before drawing a MOCR display that should appear in the center of the MFD
	int HH = 0, WW = 0;
	switch (size)
	{
	case 2:
		HH = 32;
		WW = 64;
		break;
	case 3:
		HH = 28;
		WW = 56;
		break;
	case 4:
		HH = 21;
		WW = 42;
		break;
	}

	WOFF = (W - CW * WW) / 2;
	HOFF = (H - CH * HH) / 2;
}

void ApolloRTCCMFD::ResetMOCRDisplayCentered()
{
	//Reset MOCR display offsets
	HOFF = WOFF = 0;
}