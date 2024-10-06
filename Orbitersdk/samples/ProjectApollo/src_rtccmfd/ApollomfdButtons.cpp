#include "MFDButtonPage.hpp"
#include "ApollomfdButtons.h"
#include "ApolloRTCCMFD.h" 

ApolloRTCCMFDButtons::ApolloRTCCMFDButtons()
{
	static const MFDBUTTONMENU mnu0[] =
	{
		{ "Maneuver Targeting", 0, 'T' },
		{ "Pre-Advisory Data", 0, 'P' },
		{ "Utility", 0, 'U' },
		{ "MPT Init", 0, 'I' },
		{ "Mission Planning", 0, 'B' },
		{ "Configuration", 0, 'S' },

		{ "Uplinks", 0, 'U' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "MCC Displays", 0, 'M' },
		{ "", 0, ' ' },
	};

	RegisterPage(mnu0, sizeof(mnu0) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TAR", OAPI_KEY_T, &ApolloRTCCMFD::menuSetTargetingMenu);
	RegisterFunction("PAD", OAPI_KEY_P, &ApolloRTCCMFD::menuSetPADMenu);
	RegisterFunction("UTI", OAPI_KEY_U, &ApolloRTCCMFD::menuSetUtilityMenu);
	RegisterFunction("INIT", OAPI_KEY_I, &ApolloRTCCMFD::menuSetMPTInitPage);
	RegisterFunction("PLN", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMPTPage);
	RegisterFunction("CFG", OAPI_KEY_C, &ApolloRTCCMFD::menuSetConfigPage);

	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuSetUplinkMenu);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MCC", OAPI_KEY_M, &ApolloRTCCMFD::menuSetMCCDisplaysPage);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);


	static const MFDBUTTONMENU mnu1[] =
	{
		{ "Mode", 0, 'T' },
		{ "Target vehicle", 0, 'P' },
		{ "Chaser vector time", 0, 'M' },
		{ "Target vector time", 0, 'A' },
		{ "Time of 1st maneuver", 0, 'N' },
		{ "Time of 2nd maneuver", 0, 'G' },

		{ "Time increment", 0, 'V' },
		{ "Time range", 0, 'C' },
		{ "Set Offset", 0, 'O' },
		{ "Go to display", 0, 'U' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu1, sizeof(mnu1) / sizeof(MFDBUTTONMENU));

	RegisterFunction("OPT", OAPI_KEY_T, &ApolloRTCCMFD::menuCycleTwoImpulseOption);
	RegisterFunction("TGT", OAPI_KEY_P, &ApolloRTCCMFD::menuCycleK30Vehicle);
	RegisterFunction("CVT", OAPI_KEY_M, &ApolloRTCCMFD::menuTIChaserVectorTime);
	RegisterFunction("TVT", OAPI_KEY_A, &ApolloRTCCMFD::menuTITargetVectorTime);
	RegisterFunction("T1", OAPI_KEY_N, &ApolloRTCCMFD::t1dialogue);
	RegisterFunction("T2", OAPI_KEY_G, &ApolloRTCCMFD::t2dialogue);

	RegisterFunction("INC", OAPI_KEY_U, &ApolloRTCCMFD::menuTITimeIncrement);
	RegisterFunction("RAN", OAPI_KEY_V, &ApolloRTCCMFD::menuTITimeRange);
	RegisterFunction("OFF", OAPI_KEY_O, &ApolloRTCCMFD::TwoImpulseOffset);
	RegisterFunction("DIS", OAPI_KEY_C, &ApolloRTCCMFD::menuSetTIMultipleSolutionPage);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRendezvousPage);


	static const MFDBUTTONMENU mnu2[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Choose engine", 0, 'L' },
		{ "Last page", 0, 'L' },
	};

	RegisterPage(mnu2, sizeof(mnu2) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_X, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Y, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Z, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_B, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::lambertcalc);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("ENG", OAPI_KEY_L, &ApolloRTCCMFD::menuSetTITransferPage);
	RegisterFunction("BCK", OAPI_KEY_F, &ApolloRTCCMFD::menuSetLambertPage);


	static const MFDBUTTONMENU mnu3[] =
	{
		{ "SPQ Init", 0, 'I' },
		{ "Chaser/Target", 0, 'V' },
		{ "Chaser threshold", 0, 'F' },
		{ "Target threshold", 0, 'G' },
		{ "CSI or CDH", 0, 'M' },
		{ "Time Mode", 0, 'T' },

		{ "Maneuver Time", 0, 'M' },
		{ "", 0, ' ' },
		{ "Calculate burn", 0, 'C' },
		{ "Rendezvous Display", 0, 'D'},
		{ "Choose engine", 0, 'L' },
		{ "Previous page", 0, 'B' },
	};

	RegisterPage(mnu3, sizeof(mnu3) / sizeof(MFDBUTTONMENU));

	RegisterFunction("INI", OAPI_KEY_I, &ApolloRTCCMFD::menuSetSPQInitializationPage);
	RegisterFunction("VEH", OAPI_KEY_V, &ApolloRTCCMFD::menuCycleSPQChaser);
	RegisterFunction("CTH", OAPI_KEY_F, &ApolloRTCCMFD::menuSetSPQChaserThresholdTime);
	RegisterFunction("TTH", OAPI_KEY_G, &ApolloRTCCMFD::menuSetSPQTargetThresholdTime);
	RegisterFunction("MOD", OAPI_KEY_M, &ApolloRTCCMFD::menuCycleSPQMode);
	RegisterFunction("TIM", OAPI_KEY_T, &ApolloRTCCMFD::set_CDHtimemode);

	RegisterFunction("TIG", OAPI_KEY_N, &ApolloRTCCMFD::SPQtimedialogue);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::SPQcalc);
	RegisterFunction("DIS", OAPI_KEY_D, &ApolloRTCCMFD::menuSetRendezvousEvaluationDisplayPage);
	RegisterFunction("ENG", OAPI_KEY_L, &ApolloRTCCMFD::menuSetSPQorDKIRTransferPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRendezvousPage);


	static const MFDBUTTONMENU mnu4[] =
	{
		{ "Set input", 0, 'S' },
		{ "Previous Item", 0, 'P' },
		{ "Next Item", 0, 'N' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate", 0, 'C' },
		{ "Set vessel", 0, 'E' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Choose engine", 0, 'T' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu4, sizeof(mnu4) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SET", OAPI_KEY_S, &ApolloRTCCMFD::menuSetGMPInput);
	RegisterFunction("<<", OAPI_KEY_P, &ApolloRTCCMFD::menuCycleMarkerDown);
	RegisterFunction(">>", OAPI_KEY_N, &ApolloRTCCMFD::menuCycleMarkerUp);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::GPMPCalc);
	RegisterFunction("VES", OAPI_KEY_E, &ApolloRTCCMFD::menuGPMCycleVessel);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("ENG", OAPI_KEY_T, &ApolloRTCCMFD::menuSetGPMTransferPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu5[] =
	{
		{ "CSM or LM", 0, 'K' },
		{ "Option", 0, 'O' },
		{ "Time of Alignment", 0, 'G' },
		{ "Heads up/down for P30", 0, 'H' },
		{ "Attitude input", 0, 'A' },
		{ "REFSMMAT from AGC", 0, 'D' },
		

		{ "Select vessel", 0, 'S' },
		{ "Calculate", 0, 'C' },
		{ "Move REFSMMAT", 0, 'U' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu5, sizeof(mnu5) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VES", OAPI_KEY_K, &ApolloRTCCMFD::CycleCSMOrLMSelection);
	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::CycleREFSMMATopt);
	RegisterFunction("TIM", OAPI_KEY_T, &ApolloRTCCMFD::REFSMMATTimeDialogue);
	RegisterFunction("HEA", OAPI_KEY_H, &ApolloRTCCMFD::cycleREFSMMATHeadsUp);
	RegisterFunction("ATT", OAPI_KEY_A, &ApolloRTCCMFD::menuREFSMMATAtt);
	RegisterFunction("DWN", OAPI_KEY_D, &ApolloRTCCMFD::GetREFSMMATfromAGC);


	RegisterFunction("SEL", OAPI_KEY_S, &ApolloRTCCMFD::set_Vessel);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::calcREFSMMAT);
	RegisterFunction("G00", OAPI_KEY_U, &ApolloRTCCMFD::menuREFSMMATLockerMovement);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);

	static const MFDBUTTONMENU mnu6[] =
	{
		{ "RTE Tradeoff", 0, 'T' },
		{ "Abort Scan Table", 0, 'A' },
		{ "Return to Earth Digitals", 0, 'M' },
		{ "Splashdown Update", 0, 'S' },
		{ "RTE Constraints", 0, 'C' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu6, sizeof(mnu6) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TRD", OAPI_KEY_T, &ApolloRTCCMFD::menuSetRTETradeoffDisplayPage);
	RegisterFunction("AST", OAPI_KEY_A, &ApolloRTCCMFD::menuSetAbortScanTableInputPage);
	RegisterFunction("RTE", OAPI_KEY_M, &ApolloRTCCMFD::menuSetRTEDigitalsInputPage);
	RegisterFunction("SPL", OAPI_KEY_S, &ApolloRTCCMFD::menuSetEntryUpdatePage);
	RegisterFunction("CON", OAPI_KEY_C, &ApolloRTCCMFD::menuSetRTEConstraintsPage);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("MAN", OAPI_KEY_E, &ApolloRTCCMFD::menuSetRTEDManualManeuverInputPage);
	RegisterFunction("ENT", OAPI_KEY_V, &ApolloRTCCMFD::menuSetRTEDEntryProfilePage);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu7[] =
	{
		{ "State Vector Slot", 0, 'D' },
		{ "Set Target", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Set AGS K Factor", 0, 'A' },
		{ "Get AGS K Factor", 0, 'U' },

		{ "Calculate State Vector", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu7, sizeof(mnu7) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SLT", OAPI_KEY_D, &ApolloRTCCMFD::CycleCSMOrLMSelection);
	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_Vessel);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("AGS", OAPI_KEY_A, &ApolloRTCCMFD::menuSetAGSKFactor);
	RegisterFunction("KFA", OAPI_KEY_U, &ApolloRTCCMFD::menuGetAGSKFactor);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuAGSSVCalc);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);

	static const MFDBUTTONMENU mnu8[] =
	{
		{ "RTCC files", 0, 'G' },
		{ "Choose CSM", 0, 'D' },
		{ "Choose LM", 0, 'A' },
		{ "Vessel status", 0, 'V' },
		{ "LM stage", 0, 'T' },
		{ "Sextant Star Time", 0, 'S' },
		

		{ "Set launch day", 0, 'M' },
		{ "Set launch time", 0, 'K' },
		{ "", 0, ' ' },
		{ "Update liftoff time", 0, 'T' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu8, sizeof(mnu8) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MIS", OAPI_KEY_G, &ApolloRTCCMFD::menuSetRTCCFilesPage);
	RegisterFunction("CSM", OAPI_KEY_D, &ApolloRTCCMFD::set_CSMVessel);
	RegisterFunction("LM", OAPI_KEY_A, &ApolloRTCCMFD::set_LMVessel);
	RegisterFunction("TYP", OAPI_KEY_V, &ApolloRTCCMFD::menuChangeVesselStatus);
	RegisterFunction("STA", OAPI_KEY_T, &ApolloRTCCMFD::menuCycleLMStage);
	RegisterFunction("SXT", OAPI_KEY_S, &ApolloRTCCMFD::menusextantstartime);

	RegisterFunction("DAT", OAPI_KEY_M, &ApolloRTCCMFD::menuSetLaunchDate);
	RegisterFunction("TIM", OAPI_KEY_K, &ApolloRTCCMFD::menuSetLaunchTime);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPD", OAPI_KEY_U, &ApolloRTCCMFD::menuUpdateLiftoffTime);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu9[] =
	{
		{ "Maneuver Vehicle", 0, 'V' },
		{ "Maneuver Engine", 0, 'E' },
		{ "Heads-up/down", 0, 'H' },
		{ "Ullage option", 0, 'U' },
		{ "Manual TIG", 0, 'T' },
		{ "Manual DV", 0, 'D' },

		{ "Calculate PAD", 0, 'C' },
		{ "Switch Maneuver option", 0, 'O' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu9, sizeof(mnu9) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VEH", OAPI_KEY_V, &ApolloRTCCMFD::menuChangeVesselStatus);
	RegisterFunction("ENG", OAPI_KEY_E, &ApolloRTCCMFD::menuSwitchManPADEngine);
	RegisterFunction("HEA", OAPI_KEY_H, &ApolloRTCCMFD::menuSwitchHeadsUp);
	RegisterFunction("ULL", OAPI_KEY_U, &ApolloRTCCMFD::menuManPADUllage);
	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::menuManPADTIG);
	RegisterFunction("DV", OAPI_KEY_D, &ApolloRTCCMFD::menuManPADDV);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalcManPAD);
	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::menuSwitchManPADopt);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu10[] =
	{
		{ "Sextant star check", 0, 'H' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Downlink Entry Target", 0, 'W' },
		{ "", 0, ' ' },

		{ "Calculate Entry PAD", 0, 'C' },
		{ "Lunar/Earth Entry", 0, 'V' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu10, sizeof(mnu10) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SXT", OAPI_KEY_H, &ApolloRTCCMFD::menuCycleLunarEntryPADSxtOption);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("DWN", OAPI_KEY_W, &ApolloRTCCMFD::GetEntryTargetfromAGC);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalcEntryPAD);
	RegisterFunction("OPT", OAPI_KEY_V, &ApolloRTCCMFD::menuSwitchEntryPADOpt);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu11[] =
	{
		{ "GET for anchor vector", 0, 'G' },
		{ "Prime meridian", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Select CSM or LM", 0, 'E' },
		{ "Calc Map Update", 0, 'C' },
		{ "Earth or Moon", 0, 'V' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu11, sizeof(mnu11) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VES", OAPI_KEY_P, &ApolloRTCCMFD::CycleCSMOrLMSelection);
	RegisterFunction("GET", OAPI_KEY_G, &ApolloRTCCMFD::menuSetMapUpdateGET);
	RegisterFunction("PM", OAPI_KEY_D, &ApolloRTCCMFD::menuCycleMapUpdatePM);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("SEL", OAPI_KEY_E, &ApolloRTCCMFD::set_Vessel);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalcMapUpdate);
	RegisterFunction("MOD", OAPI_KEY_V, &ApolloRTCCMFD::menuSwitchMapUpdate);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu12[] =
	{
		{ "LOI init page", 0, 'I' },
		{ "Vessel or vector time", 0, 'V' },
		{ "LOI1 apolune", 0, 'A' },
		{ "LOI1 perilune", 0, 'P' },
		{ "Max DV for pos soln", 0, 'D' },
		{ "Max DV for neg soln", 0, 'E' },

		{ "Calculate maneuver", 0, 'C' },
		{ "Minimum azimuth", 0, 'M' },
		{ "Desired azimuth", 0, 'N' },
		{ "Maximum azimuth", 0, 'O' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu12, sizeof(mnu12) / sizeof(MFDBUTTONMENU));

	RegisterFunction("INI", OAPI_KEY_I, &ApolloRTCCMFD::menuSetLOIInitPage);
	RegisterFunction("VTI", OAPI_KEY_V, &ApolloRTCCMFD::menuSetLOIVectorTime);
	RegisterFunction("APO", OAPI_KEY_A, &ApolloRTCCMFD::menuSetLOIApo);
	RegisterFunction("PER", OAPI_KEY_P, &ApolloRTCCMFD::menuSetLOIPeri);
	RegisterFunction("DVP", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLOIMaxDVPos);
	RegisterFunction("DVN", OAPI_KEY_E, &ApolloRTCCMFD::menuSetLOIMaxDVNeg);

	RegisterFunction("DIS", OAPI_KEY_C, &ApolloRTCCMFD::menuSetLOIDisplayPage);
	RegisterFunction("AMN", OAPI_KEY_M, &ApolloRTCCMFD::menuSetLOIMinAzi);
	RegisterFunction("ADS", OAPI_KEY_N, &ApolloRTCCMFD::menuSetLOIDesiredAzi);
	RegisterFunction("AMX", OAPI_KEY_O, &ApolloRTCCMFD::menuSetLOIMaxAzi);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu13[] =
	{
		{ "Choose CSM", 0, 'E' },
		{ "T1 guess", 0, 'T' },
		{ "T2 elevation", 0, ' ' },
		{ "Landmark Latitude", 0, 'A' },
		{ "Landmark Longitude", 0, 'O' },
		{ "Load landing site coordinates", 0, 'D' },

		{ "Calculate PAD", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu13, sizeof(mnu13) / sizeof(MFDBUTTONMENU));

	RegisterFunction("CSM", OAPI_KEY_E, &ApolloRTCCMFD::set_CSMVessel);
	RegisterFunction("TIM", OAPI_KEY_T, &ApolloRTCCMFD::menuSetLmkTime);
	RegisterFunction("EL", OAPI_KEY_F, &ApolloRTCCMFD::menuSetLmkElevation);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuSetLmkLat);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::menuSetLmkLng);
	RegisterFunction("LLS", OAPI_KEY_D, &ApolloRTCCMFD::menuLmkUseLandingSite);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLmkPADCalc);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_J, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu14[] =
	{
		{ "Rendezvous Targeting", 0, 'R' },
		{ "Orbit Adjustment", 0, 'O' },
		{ "Translunar", 0, 'M' },
		{ "Lunar Orbit", 0, 'S' },
		{ "Return to Earth", 0, 'E' },
		{ "Deorbit", 0, 'T' },

		{ "DOI Targeting", 0, 'D' },
		{ "Lunar Launch Window", 0, 'W' },
		{ "Lunar Launch Targeting", 0, 'L' },
		{ "Lunar Ascent", 0, 'A' },
		{ "Perigee Adjust", 0, 'P' },
		{ "Back to main menu", 0, 'B' },
	};

	RegisterPage(mnu14, sizeof(mnu14) / sizeof(MFDBUTTONMENU));

	RegisterFunction("REN", OAPI_KEY_R, &ApolloRTCCMFD::menuSetRendezvousPage);
	RegisterFunction("ORB", OAPI_KEY_O, &ApolloRTCCMFD::menuSetOrbAdjPage);
	RegisterFunction("TL", OAPI_KEY_M, &ApolloRTCCMFD::menuTranslunarPage);
	RegisterFunction("LOI", OAPI_KEY_S, &ApolloRTCCMFD::menuSetLOIPage);
	RegisterFunction("ENT", OAPI_KEY_E, &ApolloRTCCMFD::menuSetReturnToEarthPage);
	RegisterFunction("DEO", OAPI_KEY_T, &ApolloRTCCMFD::menuSetRetrofireSubsystemPage);

	RegisterFunction("LDP", OAPI_KEY_D, &ApolloRTCCMFD::menuSetDescPlanCalcPage);
	RegisterFunction("LLW", OAPI_KEY_W, &ApolloRTCCMFD::menuSetLunarLiftoffPage);
	RegisterFunction("LLT", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLunarLaunchTargetingPage);
	RegisterFunction("ASC", OAPI_KEY_A, &ApolloRTCCMFD::menuSetLunarAscentPage);
	RegisterFunction("PAT", OAPI_KEY_P, &ApolloRTCCMFD::menuSetPerigeeAdjustInputPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu15[] =
	{
		{ "CSM or LM", 0, 'P' },
		{ "Object to point at", 0, 'P' },
		{ "Vessel axis", 0, 'D' },
		{ "Choose the direction", 0, 'D' },
		{ "Choose pitch and yaw", 0, 'E' },
		{ "Choose omicron angle", 0, 'F' },

		{ "Select Vessel", 0, 'T' },
		{ "Calculate attitude", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu15, sizeof(mnu15) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VES", OAPI_KEY_P, &ApolloRTCCMFD::CycleCSMOrLMSelection);
	RegisterFunction("OPT", OAPI_KEY_G, &ApolloRTCCMFD::cycleVECPOINTOpt);
	RegisterFunction("OBJ", OAPI_KEY_P, &ApolloRTCCMFD::vecbodydialogue);
	RegisterFunction("DIR", OAPI_KEY_D, &ApolloRTCCMFD::cycleVECDirOpt);
	RegisterFunction("PY", OAPI_KEY_E, &ApolloRTCCMFD::menuVECPOINTSelectAttitude);
	RegisterFunction("OMI", OAPI_KEY_F, &ApolloRTCCMFD::menuVECPOINTOmicron);

	RegisterFunction("SEL", OAPI_KEY_T, &ApolloRTCCMFD::set_Vessel);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuVECPOINTCalc);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_J, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRTACFPage);


	static const MFDBUTTONMENU mnu16[] =
	{
		{ "Init parameters", 0, 'I' },
		{ "Vehicle", 0, 'V' },
		{ "Vessel or vector time", 0, 'T' },
		{ "LDPP mode", 0, 'M' },
		{ "LDPP sequence", 0, 'S' },
		{ "Desired height", 0, 'H' },

		{ "Threshold time 1", 0, 'A' },
		{ "Threshold time 2", 0, 'C' },
		{ "Threshold time 3", 0, 'D' },
		{ "Threshold time 4", 0, 'E' },
		{ "LDPP display", 0, 'P' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu16, sizeof(mnu16) / sizeof(MFDBUTTONMENU));

	RegisterFunction("INI", OAPI_KEY_G, &ApolloRTCCMFD::menuSetDescPlanInitPage);
	RegisterFunction("VEH", OAPI_KEY_V, &ApolloRTCCMFD::cycleLDPPVehicle);
	RegisterFunction("VTI", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLDPPVectorTime);
	RegisterFunction("MOD", OAPI_KEY_M, &ApolloRTCCMFD::menuSetLDPPMode);
	RegisterFunction("SEQ", OAPI_KEY_S, &ApolloRTCCMFD::menuSetLDPPSequence);
	RegisterFunction("HEI", OAPI_KEY_H, &ApolloRTCCMFD::menuSetLDPPDesiredHeight);

	RegisterFunction("TH1", OAPI_KEY_A, &ApolloRTCCMFD::menuLDPPThresholdTime1);
	RegisterFunction("TH2", OAPI_KEY_B, &ApolloRTCCMFD::menuLDPPThresholdTime2);
	RegisterFunction("TH3", OAPI_KEY_D, &ApolloRTCCMFD::menuLDPPThresholdTime3);
	RegisterFunction("TH4", OAPI_KEY_E, &ApolloRTCCMFD::menuLDPPThresholdTime4);
	RegisterFunction("DPT", OAPI_KEY_C, &ApolloRTCCMFD::menuSetDescPlanTablePage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu17[] =
	{
		{ "TLI Processor", 0, 'M' },
		{ "Midcourse Processor", 0, 'G' },
		{ "Midcourse Constraints", 0, 'N' },
		{ "Go to SFP display", 0, 'V' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu17, sizeof(mnu17) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TLI", OAPI_KEY_M, &ApolloRTCCMFD::menuTLIPlanningPage);
	RegisterFunction("MCC", OAPI_KEY_G, &ApolloRTCCMFD::menuMidcoursePage);
	RegisterFunction("CON", OAPI_KEY_N, &ApolloRTCCMFD::menuSetMidcourseConstraintsPage);
	RegisterFunction("SFP", OAPI_KEY_V, &ApolloRTCCMFD::menuSetSkeletonFlightPlanPage);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu18[] =
	{
		{ "Approach azimuth", 0, 'A' },
		{ "Altitude at PDI", 0, 'H' },
		{ "Pwrd descent sim", 0, 'P' },
		{ "PDI time", 0, 'F' },
		{ "Orbits between DOI and PDI", 0, 'N' },
		{ "", 0, ' ' },

		{ "Descent Flight Time", 0, 'T' },
		{ "Descent Flight Arc", 0, 'C' },
		{ "Landing site offset", 0, 'N' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu18, sizeof(mnu18) / sizeof(MFDBUTTONMENU));

	RegisterFunction("AZI", OAPI_KEY_A, &ApolloRTCCMFD::menuSetLDPPAzimuth);
	RegisterFunction("HDP", OAPI_KEY_H, &ApolloRTCCMFD::menuSetLDPPDescIgnHeight);
	RegisterFunction("PDS", OAPI_KEY_P, &ApolloRTCCMFD::cycleLDPPPoweredDescSimFlag);
	RegisterFunction("PDI", OAPI_KEY_F, &ApolloRTCCMFD::menuSetLDPPPoweredDescTime);
	RegisterFunction("N", OAPI_KEY_N, &ApolloRTCCMFD::menuSetLDPPDwellOrbits);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("DFT", OAPI_KEY_T, &ApolloRTCCMFD::menuSetLDPPDescentFlightTime);
	RegisterFunction("DFA", OAPI_KEY_C, &ApolloRTCCMFD::menuSetLDPPDescentFlightArc);
	RegisterFunction("LSO", OAPI_KEY_N, &ApolloRTCCMFD::menuSetLDPPLandingSiteOffset);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetDescPlanCalcPage);


	static const MFDBUTTONMENU mnu19[] =
	{
		{ "Selen. latitude", 0, 'A' },
		{ "Selen. longitude", 0, 'O' },
		{ "Approach azimuth", 0, 'L' },
		{ "Distance", 0, ' ' },
		{ "Step Size", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "Generate terrain model", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu19, sizeof(mnu19) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuTMLat);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::menuTMLng);
	RegisterFunction("AZI", OAPI_KEY_L, &ApolloRTCCMFD::menuTMAzi);
	RegisterFunction("DIS", OAPI_KEY_B, &ApolloRTCCMFD::menuTMDistance);
	RegisterFunction("STE", OAPI_KEY_D, &ApolloRTCCMFD::menuTMStepSize);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTerrainModelCalc);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);
	

	static const MFDBUTTONMENU mnu20[] =
	{

		{ "Maneuver PAD", 0, 'M' },
		{ "Entry PAD", 0, 'E' },
		{ "Landmark Tracking", 0, 'L' },
		{ "Map Update", 0, 'U' },
		{ "Nav Check PAD", 0, 'N' },
		{ "", 0, ' ' },

		{ "DAP PAD", 0, 'A' },
		{ "LM Ascent PAD", 0, 'C' },
		{ "AGS SV PAD", 0, 'F' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu20, sizeof(mnu20) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAN", OAPI_KEY_M, &ApolloRTCCMFD::menuSetManPADPage);
	RegisterFunction("ENP", OAPI_KEY_E, &ApolloRTCCMFD::menuSetEntryPADPage);
	RegisterFunction("LMK", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLandmarkTrkPage);
	RegisterFunction("MAP", OAPI_KEY_U, &ApolloRTCCMFD::menuSetMapUpdatePage);
	RegisterFunction("NAV", OAPI_KEY_N, &ApolloRTCCMFD::menuSetNavCheckPADPage);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("DAP", OAPI_KEY_A, &ApolloRTCCMFD::menuSetDAPPADPage);
	RegisterFunction("ASC", OAPI_KEY_C, &ApolloRTCCMFD::menuSetLMAscentPADPage);
	RegisterFunction("AGS", OAPI_KEY_F, &ApolloRTCCMFD::menuSetAGSSVPage);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu21[] =
	{
		{ "Landing Site Update", 0, 'V' },
		{ "REFSMMAT", 0, 'R' },
		{ "Optics", 0, 'P' },
		{ "IMU Parking Angles", 0, 'E' },
		{ "Convert nodal target", 0, 'N' },
		{ "Descent abort", 0, 'A' },

		{ "Saturn IB Launch", 0, 'L' },
		{ "Saturn V Azimuth", 0, 'A' },
		{ "Terrain Model Generation", 0, 'T' },
		{ "Lunar Impact Targeting", 0, 'H' },
		{ "Debug", 0, 'I' },
		{ "Back to main menu", 0, 'B' }
	};

	RegisterPage(mnu21, sizeof(mnu21) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LS", OAPI_KEY_V, &ApolloRTCCMFD::menuSetLSUpdateMenu);
	RegisterFunction("REF", OAPI_KEY_R, &ApolloRTCCMFD::menuSetREFSMMATPage);
	RegisterFunction("OPT", OAPI_KEY_P, &ApolloRTCCMFD::menuSetRTACFPage);
	RegisterFunction("PRK", OAPI_KEY_E, &ApolloRTCCMFD::menuSetIMUParkingAnglesPage);
	RegisterFunction("NOD", OAPI_KEY_N, &ApolloRTCCMFD::menuSetNodalTargetConversionPage);
	RegisterFunction("ABO", OAPI_KEY_A, &ApolloRTCCMFD::menuSetPDAPPage);

	RegisterFunction("SIB", OAPI_KEY_L, &ApolloRTCCMFD::menuSetSaturnIBLVDCPage);
	RegisterFunction("SV", OAPI_KEY_A, &ApolloRTCCMFD::menuSetSaturnVLVDCPage);
	RegisterFunction("TER", OAPI_KEY_T, &ApolloRTCCMFD::menuSetTerrainModelPage);
	RegisterFunction("LUN", OAPI_KEY_H, &ApolloRTCCMFD::menuSetLunarTargetingProgramPage);
	RegisterFunction("DBG", OAPI_KEY_I, &ApolloRTCCMFD::menuSetDebugPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu22[] =
	{
		{ "Maneuver type", 0, 'M' },
		{ "Vessel or vector time", 0, 'V' },
		{ "Ignition time", 0, 'T' },
		{ "Column for results", 0, 'C' },
		{ "Docked or undocked", 0, 'D' },
		{ "Cycle DOI options", 0, 'S' },

		{ "Calculate maneuver", 0, 'O' },
		{ "Pericynthion height", 0, 'H' },
		{ "Return inclination", 0, 'I' },
		{ "PC height for mode 5", 0, 'E' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' }
	};

	RegisterPage(mnu22, sizeof(mnu22) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAN", OAPI_KEY_M, &ApolloRTCCMFD::menuSwitchTLCCManeuver);
	RegisterFunction("VTI", OAPI_KEY_V, &ApolloRTCCMFD::menuTLCCVectorTime);
	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::menuSetTLCCGET);
	RegisterFunction("COL", OAPI_KEY_C, &ApolloRTCCMFD::menuCycleTLCCColumnNumber);
	RegisterFunction("CFG", OAPI_KEY_D, &ApolloRTCCMFD::menuCycleTLCCConfiguration);
	RegisterFunction("SFP", OAPI_KEY_S, &ApolloRTCCMFD::menuCycleTLCCCSFPBlockNumber);

	RegisterFunction("MID", OAPI_KEY_O, &ApolloRTCCMFD::menuMidcourseTradeoffPage);
	RegisterFunction("HPC", OAPI_KEY_H, &ApolloRTCCMFD::menuSetTLCCAlt);
	RegisterFunction("INC", OAPI_KEY_I, &ApolloRTCCMFD::menuSetTLCCDesiredInclination);
	RegisterFunction("PC5", OAPI_KEY_E, &ApolloRTCCMFD::menuSetTLCCAltMode5);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuTranslunarPage);


	static const MFDBUTTONMENU mnu23[] =
	{
		{ "LLWP Init", 0, 'I' },
		{ "Chaser vehicle", 0, 'C' },
		{ "CSM or vector time", 0, 'V' },
		{ "Threshold time", 0, 'H' },
		{ "CSI option", 0, 'S' },
		{ "CDH option", 0, 'D' },

		{ "", 0, ' ' },
		{ "TPI option", 0, 'P' },
		{ "DH option", 0, 'A' },
		{ "Delta heights", 0, 'E' },
		{ "LLWP Display", 0, 'F' },
		{ "Back to menu", 0, 'B' }
	};

	RegisterPage(mnu23, sizeof(mnu23) / sizeof(MFDBUTTONMENU));

	RegisterFunction("INI", OAPI_KEY_I, &ApolloRTCCMFD::menuSetLLWPInitPage);
	RegisterFunction("CHA", OAPI_KEY_C, &ApolloRTCCMFD::menuCycleLLWPChaserOption);
	RegisterFunction("VTI", OAPI_KEY_V, &ApolloRTCCMFD::menuLLWPVectorTime);
	RegisterFunction("THT", OAPI_KEY_H, &ApolloRTCCMFD::menuSetLiftoffguess);
	RegisterFunction("CSI", OAPI_KEY_S, &ApolloRTCCMFD::menuSetLLWPCSIFlag);
	RegisterFunction("CDH", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLLWPCDHFlag);

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TPI", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("LLW", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("DH", OAPI_KEY_E, &ApolloRTCCMFD::menuSetLLWPDeltaHeights);
	RegisterFunction("DIS", OAPI_KEY_F, &ApolloRTCCMFD::menuSetLLWPDisplayPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu24[] =
	{
		{ "Enter file name", 0, 'F' },
		{ "Enter load number", 0, 'L' },
		{ "Initialize", 0, 'I' },
		{ "Edit octal", 0, 'G' },
		{ "Delete line", 0, 'D' },
		{ "", 0, ' ' },

		{ "Load EMP", 0, 'O' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink", 0, 'U' },
		{ "Back to menu", 0, 'B' }
	};

	RegisterPage(mnu24, sizeof(mnu24) / sizeof(MFDBUTTONMENU));

	RegisterFunction("FIL", OAPI_KEY_F, &ApolloRTCCMFD::menuSetEMPFileName);
	RegisterFunction("NUM", OAPI_KEY_L, &ApolloRTCCMFD::menuSetEMPUplinkNumber);
	RegisterFunction("INI", OAPI_KEY_I, &ApolloRTCCMFD::menuInitializeEMP);
	RegisterFunction("EDI", OAPI_KEY_E, &ApolloRTCCMFD::menuEditEMPOctal);
	RegisterFunction("DEL", OAPI_KEY_D, &ApolloRTCCMFD::menuDeleteEMPLine);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("LOA", OAPI_KEY_O, &ApolloRTCCMFD::menuLoadEMP);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuUplinkEMP);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu25[] =
	{
		{ "Choose vessel type", 0, 'P' },
		{ "GET of Nav Check", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Select CSM or LM", 0, 'E' },
		{ "Calculate PAD", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' }
	};

	RegisterPage(mnu25, sizeof(mnu25) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VES", OAPI_KEY_P, &ApolloRTCCMFD::CycleCSMOrLMSelection);
	RegisterFunction("GET", OAPI_KEY_G, &ApolloRTCCMFD::menuSetNavCheckGET);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("SEL", OAPI_KEY_E, &ApolloRTCCMFD::set_Vessel);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuNavCheckPADCalc);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu26[] =
	{
		{ "Target type", 0, 'L' },
		{ "Estimated TIG", 0, 'D' },
		{ "Target latitude", 0, 'O' },
		{ "Target longitude", 0, 'A' },
		{ "Miss distance", 0, 'M' },
		{ "", 0, ' ' },

		{ "Retrofire Digitals", 0, 'C' },
		{ "Retrofire External DV", 0, 'V' },
		{ "Retrofire Separation", 0, 'Q' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu26, sizeof(mnu26) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TYP", OAPI_KEY_L, &ApolloRTCCMFD::menuCycleRetrofireType);
	RegisterFunction("GET", OAPI_KEY_D, &ApolloRTCCMFD::menuRetrofireGETIDialogue);
	RegisterFunction("LAT", OAPI_KEY_O, &ApolloRTCCMFD::menuRetrofireLatDialogue);
	RegisterFunction("LNG", OAPI_KEY_A, &ApolloRTCCMFD::menuRetrofireLngDialogue);
	RegisterFunction("MD", OAPI_KEY_M, &ApolloRTCCMFD::menuSetRetrofireMissDistance);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("DIG", OAPI_KEY_C, &ApolloRTCCMFD::menuSetRetrofireDigitalsPage);
	RegisterFunction("XDV", OAPI_KEY_V, &ApolloRTCCMFD::menuSetRetrofireXDVPage);
	RegisterFunction("SEP", OAPI_KEY_Q, &ApolloRTCCMFD::menuSetRetrofireSeparationPage);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRetrofireSubsystemPage);


	static const MFDBUTTONMENU mnu27[] =
	{
		{ "Choose column", 0, 'C' },
		{ "AST Code", 0, 'A' },
		{ "REFSMMAT type", 0, 'R' },
		{ "Maneuver code", 0, 'M' },
		{ "Set ullage numbers", 0, 'U' },
		{ "Cycle gimbal trim option", 0, 'T' },

		{ "Go to display", 0, 'D' },
		{ "Docking angle", 0, 'O' },
		{ "Cycle heads up/down", 0, 'H' },
		{ "Iterate option", 0, 'I' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu27, sizeof(mnu27) / sizeof(MFDBUTTONMENU));

	RegisterFunction("COL", OAPI_KEY_C, &ApolloRTCCMFD::menuCycleRTEDColumn);
	RegisterFunction("AST", OAPI_KEY_A, &ApolloRTCCMFD::menuRTEDASTCodeDialogue);
	RegisterFunction("REF", OAPI_KEY_R, &ApolloRTCCMFD::menuRTED_REFSMMAT);
	RegisterFunction("MAN", OAPI_KEY_M, &ApolloRTCCMFD::menuSetRTEManeuverCode);
	RegisterFunction("ULL", OAPI_KEY_U, &ApolloRTCCMFD::menuSetRTEDUllage);
	RegisterFunction("TRM", OAPI_KEY_E, &ApolloRTCCMFD::menuCycleRTEDTrimAnglesOption);

	RegisterFunction("DIS", OAPI_KEY_D, &ApolloRTCCMFD::menuSetRTEDigitalsPage);
	RegisterFunction("DOC", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("HEA", OAPI_KEY_H, &ApolloRTCCMFD::menuCycleRTEDHeadsOption);
	RegisterFunction("ITE", OAPI_KEY_I, &ApolloRTCCMFD::menuCycleRTEDIterateOption);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetReturnToEarthPage);


	static const MFDBUTTONMENU mnu28[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "External DV", 0, 'I' },

		{ "Calculate solution", 0, 'C' },
		{ "Save as DOD REFSMMAT", 0, 'D' },
		{ "Make DOD the current REFSMMAT", 0, 'R' },
		{ "Save splashdown coordinates", 0, 'S' },
		{ "Transfer to MPT or PAD", 0, 'M' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu28, sizeof(mnu28) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("DV", OAPI_KEY_I, &ApolloRTCCMFD::menuSetRetrofireXDVPage);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuEntryCalc);
	RegisterFunction("REF", OAPI_KEY_D, &ApolloRTCCMFD::menuSaveRTEREFSMMAT);
	RegisterFunction("CUR", OAPI_KEY_R, &ApolloRTCCMFD::menuMakeDODREFSMMATCurrent);
	RegisterFunction("SPL", OAPI_KEY_S, &ApolloRTCCMFD::menuSaveSplashdownTarget);
	RegisterFunction("TRA", OAPI_KEY_M, &ApolloRTCCMFD::menuTransferRTEToMPT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRTEDigitalsInputPage);


	static const MFDBUTTONMENU mnu29[] =
	{
		{ "Constraints with number", 0, 'V' },
		{ "Constraints with text", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu29, sizeof(mnu29) / sizeof(MFDBUTTONMENU));

	RegisterFunction("CO1", OAPI_KEY_V, &ApolloRTCCMFD::menuSetRTEConstraintF86);
	RegisterFunction("CO2", OAPI_KEY_T, &ApolloRTCCMFD::menuSetRTEConstraintF87);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("ADD", OAPI_KEY_C, &ApolloRTCCMFD::menuAddRTESite);
	RegisterFunction("REP", OAPI_KEY_Q, &ApolloRTCCMFD::menuReplaceRTESite);
	RegisterFunction("DEL", OAPI_KEY_R, &ApolloRTCCMFD::menuDeleteRTESite);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetReturnToEarthPage);


	static const MFDBUTTONMENU mnu30[] =
	{
		{ "Entry Range", 0, 'R' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Update", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu30, sizeof(mnu30) / sizeof(MFDBUTTONMENU));

	RegisterFunction("RAN", OAPI_KEY_R, &ApolloRTCCMFD::EntryRangeDialogue);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuEntryUpdateCalc);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuSetEntryUplinkPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetReturnToEarthPage);


	static const MFDBUTTONMENU mnu31[] =
	{
		{ "Choose sys. parameters file", 0, 'T' },
		{ "Choose TLI file", 0, 'L' },
		{ "Choose SFP file", 0, 'O' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu31, sizeof(mnu31) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SYS", OAPI_KEY_T, &ApolloRTCCMFD::menuLoadSystemParametersFile);
	RegisterFunction("TLI", OAPI_KEY_L, &ApolloRTCCMFD::menuLoadTLIFile);
	RegisterFunction("SFP", OAPI_KEY_O, &ApolloRTCCMFD::menuLoadSFPFile);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetConfigPage);


	static const MFDBUTTONMENU mnu32[] =
	{
		{ "Lambert targeting", 0, 'L' },
		{ "CDH/NSR maneuver", 0, 'C' },
		{ "Docking initate", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to main menu", 0, 'B' },
	};

	RegisterPage(mnu32, sizeof(mnu32) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TI", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLambertPage);
	RegisterFunction("SPQ", OAPI_KEY_C, &ApolloRTCCMFD::menuSetSPQPage);
	RegisterFunction("DKI", OAPI_KEY_D, &ApolloRTCCMFD::menuSetDKIPage);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TPI", OAPI_KEY_A, &ApolloRTCCMFD::menuSetTPITimesPage);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu33[] =
	{
		{ "TIG/TPI options", 0, 'Q' },
		{ "Init parameters", 0, 'I' },
		{ "NC1 maneuver point", 0, 'T' },
		{ "NH maneuver point", 0, 'L' },
		{ "NSR maneuver point", 0, 'D' },
		{ "TPI maneuver point", 0, 'P' },

		{ "Go to displayr", 0, 'C' },
		{ "", 0, ' ' },
		{ "NPC maneuver point", 0, 'N' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu33, sizeof(mnu33) / sizeof(MFDBUTTONMENU));

	RegisterFunction("OPT", OAPI_KEY_Q, &ApolloRTCCMFD::menuSetDKIOptionsPage);
	RegisterFunction("INI", OAPI_KEY_I, &ApolloRTCCMFD::menuSetDKIInitializationPage);
	RegisterFunction("NC1", OAPI_KEY_T, &ApolloRTCCMFD::menuDKINC1Line);
	RegisterFunction("NH", OAPI_KEY_L, &ApolloRTCCMFD::menuDKINHLine);
	RegisterFunction("NSR", OAPI_KEY_D, &ApolloRTCCMFD::menuDKINSRLine);
	RegisterFunction("MI", OAPI_KEY_P, &ApolloRTCCMFD::menuDKIMILine);
	
	RegisterFunction("DIS", OAPI_KEY_C, &ApolloRTCCMFD::menuSetRendezvousPlanningDisplayPage);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("NPC", OAPI_KEY_N, &ApolloRTCCMFD::menuDKINPCLine);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRendezvousPage);


	static const MFDBUTTONMENU mnu34[] =
	{
		{ "Chaser vehicle", 0, 'C' },
		{ "Skylab or regular DKI", 0, 'P' },
		{ "Maneuver line definition", 0, 'R' },
		{ "Time of maneuver line", 0, 'O' },
		{ "Maneuver line value", 0, 'S' },
		{ "Phase angle setting", 0, 'M' },

		{ "Terminal phase definition", 0, 'E' },
		{ "Terminal phase definition value", 0, 'V' },
		{ "", 0, ' ' },
		{ "Choose CSM", 0, 'N' },
		{ "Choose LM", 0, 'U' },
		{ "Back to DKI page", 0, 'B' },
	};

	RegisterPage(mnu34, sizeof(mnu34) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VEH", OAPI_KEY_C, &ApolloRTCCMFD::menuCycleDKIChaserVehicle);
	RegisterFunction("PRO", OAPI_KEY_P, &ApolloRTCCMFD::menuCycleDKIProfile);
	RegisterFunction("MAN", OAPI_KEY_R, &ApolloRTCCMFD::menuCycleDKIManeuverLineDefinition);
	RegisterFunction("GET", OAPI_KEY_O, &ApolloRTCCMFD::menuDKITIG);
	RegisterFunction("ML", OAPI_KEY_S, &ApolloRTCCMFD::menuDKIManeuverLineValue);
	RegisterFunction("PHA", OAPI_KEY_M, &ApolloRTCCMFD::menuDKIInitialPhaseFlag);

	RegisterFunction("TPD", OAPI_KEY_E, &ApolloRTCCMFD::menuCycleDKITerminalPhaseOption);
	RegisterFunction("TPV", OAPI_KEY_V, &ApolloRTCCMFD::menuDKITerminalPhaseDefinitionValue);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("CSM", OAPI_KEY_N, &ApolloRTCCMFD::set_CSMVessel);
	RegisterFunction("LM", OAPI_KEY_U, &ApolloRTCCMFD::set_LMVessel);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetDKIPage);


	static const MFDBUTTONMENU mnu35[] =
	{
		{ "Choose vessel type", 0, 'P' },
		{ "Docked or undocked", 0, 'L' },
		{ "Full LM or ascent stage", 0, 'O' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Select CSM or LM", 0, 'E' },
		{ "Calculate DAP PAD", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu35, sizeof(mnu35) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VES", OAPI_KEY_P, &ApolloRTCCMFD::CycleCSMOrLMSelection);
	RegisterFunction("TYP", OAPI_KEY_L, &ApolloRTCCMFD::menuChangeVesselStatus);
	RegisterFunction("STA", OAPI_KEY_O, &ApolloRTCCMFD::menuCycleLMStage);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("SEL", OAPI_KEY_E, &ApolloRTCCMFD::set_Vessel);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuDAPPADCalc);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu36[] =
	{
		{ "Launch Azimuth", 0, 'A' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu36, sizeof(mnu36) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LAU", OAPI_KEY_A, &ApolloRTCCMFD::menuLaunchAzimuthCalc);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu37[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu37, sizeof(mnu37) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu38[] =
	{
		{ "Liftoff time", 0, 'L' },
		{ "Horizontal velocity", 0, 'H' },
		{ "Vertical velocity", 0, 'V' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate lunar ascent", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Transfer insertion SV to CMC uplink", 0, 'R' },
		{ "Transfer maneuver to MPT", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu38, sizeof(mnu38) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LTO", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLAPLiftoffTime);
	RegisterFunction("HOR", OAPI_KEY_H, &ApolloRTCCMFD::menuLunarLiftoffVHorInput);
	RegisterFunction("VER", OAPI_KEY_V, &ApolloRTCCMFD::menuLunarLiftoffVVertInput);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLAPCalc);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_R, &ApolloRTCCMFD::menuLunarLiftoffSaveInsertionSV);
	RegisterFunction("MPT", OAPI_KEY_U, &ApolloRTCCMFD::menuTransferPoweredAscentToMPT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu39[] =
	{
		{ "PAD version", 0, 'H' },
		{ "Liftoff time", 0, 'L' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Ascent PAD", 0, 'C' },
		{ "Set CSM", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu39, sizeof(mnu39) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VER", OAPI_KEY_H, &ApolloRTCCMFD::menuCycleAscentPADVersion);
	RegisterFunction("LTO", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLAPLiftoffTime);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuAscentPADCalc);
	RegisterFunction("CSM", OAPI_KEY_T, &ApolloRTCCMFD::set_CSMVessel);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu40[] =
	{
		{ "Mission type", 0, 'H' },
		{ "Abort engine", 0, 'E' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Descent Abort", 0, 'C' },
		{ "Set CSM", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu40, sizeof(mnu40) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MIS", OAPI_KEY_H, &ApolloRTCCMFD::menuCyclePDAPSegments);
	RegisterFunction("ENG", OAPI_KEY_E, &ApolloRTCCMFD::menuCyclePDAPEngine);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuPDAPCalc);
	RegisterFunction("CSM", OAPI_KEY_T, &ApolloRTCCMFD::set_CSMVessel);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuAP11AbortCoefUplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu41[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "Calculate longitude from GET", 0, 'G' },
		{ "Calculate GET from longitude", 0, 'L' },
		{ "Calc apo/peri values from GET", 0, 'R' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu41, sizeof(mnu41) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("GETL", OAPI_KEY_G, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETL);
	RegisterFunction("L", OAPI_KEY_L, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsL);
	RegisterFunction("GETR", OAPI_KEY_R, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETBV);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu42[] =
	{
		{ "Select display", 0, 'O' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu42, sizeof(mnu42) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MSK", OAPI_KEY_O, &ApolloRTCCMFD::menuMSKRequest);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu43[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Update state vector", 0, 'U' },
		{ "GET for Vector 1", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu43, sizeof(mnu43) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("UPD", OAPI_KEY_U, &ApolloRTCCMFD::menuSpaceDigitalsInit);
	RegisterFunction("GET", OAPI_KEY_G, &ApolloRTCCMFD::menuGenerateSpaceDigitals);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu44[] =
	{
		{ "Planning mode active", 0, 'A' },
		{ "", 0, ' ' },
		{ "Orientation change", 0, 'O' },
		{ "Delete maneuver", 0, 'D' },
		{ "Copy ephemeris", 0, 'H' },
		{ "Cape Crossing Init", 0, 'C' },

		{ "Direct Input", 0, 'I' },
		{ "Add TLI to MPT", 0, 'T' },
		{ "Add PDI to MPT", 0, 'P' },
		{ "Spherical coordinates", 0, 'S' },
		{ "Enter vector", 0, 'F' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu44, sizeof(mnu44) / sizeof(MFDBUTTONMENU));

	RegisterFunction("ACT", OAPI_KEY_A, &ApolloRTCCMFD::menuMPTCycleActive);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UDT", OAPI_KEY_O, &ApolloRTCCMFD::menuMPTVehicleOrientationChange);
	RegisterFunction("DEL", OAPI_KEY_D, &ApolloRTCCMFD::menuMPTDeleteManeuver);
	RegisterFunction("P16", OAPI_KEY_H, &ApolloRTCCMFD::menuMPTCopyEphemeris);
	RegisterFunction("P17", OAPI_KEY_C, &ApolloRTCCMFD::menuCapeCrossingInit);

	RegisterFunction("INP", OAPI_KEY_I, &ApolloRTCCMFD::menuSetMPTDirectInputPage);
	RegisterFunction("TLI", OAPI_KEY_T, &ApolloRTCCMFD::menuMPTTLIDirectInput);
	RegisterFunction("PDI", OAPI_KEY_P, &ApolloRTCCMFD::menuTransferPoweredDescentToMPT);
	RegisterFunction("P13", OAPI_KEY_S, &ApolloRTCCMFD::SetMEDInputPageP13);
	RegisterFunction("P14", OAPI_KEY_F, &ApolloRTCCMFD::SetMEDInputPageP14);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu45[] =
	{
		{ "All/lunar stations", 0, 'L' },
		{ "Generate contacts", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu45, sizeof(mnu45) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LUN", OAPI_KEY_L, &ApolloRTCCMFD::menuNextStationContactLunar);
	RegisterFunction("GEN", OAPI_KEY_G, &ApolloRTCCMFD::menuGenerateStationContacts);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu46[] =
	{
		{ "Cycle pages", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate contacts", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu46, sizeof(mnu46) / sizeof(MFDBUTTONMENU));

	RegisterFunction("PAG", OAPI_KEY_P, &ApolloRTCCMFD::CyclePredSiteAcqPage);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::PredSiteAcqCSM1Calc);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu47[] =
	{
		{ "Choose uplink page", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu47, sizeof(mnu47) / sizeof(MFDBUTTONMENU));

	RegisterFunction("DIS", OAPI_KEY_G, &ApolloRTCCMFD::menuUplinkDisplayRequest);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu48[] =
	{
		{ "Set Time", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Set Target", 0, 'T' },
		{ "Calculate State Vector", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Upload State Vector", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu48, sizeof(mnu48) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIM", OAPI_KEY_G, &ApolloRTCCMFD::menuSetSVTime);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_SVPageTarget);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuSVCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_L, &ApolloRTCCMFD::menuSVUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu49[] =
	{
		{ "Select LM", 0, 'T' },
		{ "Selen. latitude", 0, 'A' },
		{ "Selen. longitude", 0, 'O' },
		{ "Selen. radius", 0, 'L' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate LS Coordinates", 0, 'C' },
		{ "Revert to init. values", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu49, sizeof(mnu49) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LM", OAPI_KEY_T, &ApolloRTCCMFD::set_LMVessel);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuLSLat);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::menuLSLng);
	RegisterFunction("RAD", OAPI_KEY_L, &ApolloRTCCMFD::menuLSRadius);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLSCalc);
	RegisterFunction("REV", OAPI_KEY_D, &ApolloRTCCMFD::menuRevertRLSToPrelaunch);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu50[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate LS Uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink RLS", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu50, sizeof(mnu50) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCSMLSUplinkCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuCSMLSUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu51[] =
	{
		{ "Manual TIG", 0, 'T' },
		{ "Manual DV", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate P30 Uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink Ext DV", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu51, sizeof(mnu51) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::menuManPADTIG);
	RegisterFunction("DV", OAPI_KEY_D, &ApolloRTCCMFD::menuManPADDV);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuP30UplinkCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuP30Uplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu52[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate P30 Uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink Ext DV", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu52, sizeof(mnu52) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuRetrofireEXDVUplinkCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuRetrofireEXDVUplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu53[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink option", 0, 'K' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu53, sizeof(mnu53) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuREFSMMATUplinkCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::UploadREFSMMAT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu54[] =
	{
		{ "Maneuver table", 0, 'E' },
		{ "Plan number", 0, 'N' },
		{ "GET for deletion", 0, 'D' },
		{ "Select Thruster", 0, 'T' },
		{ "Attitude mode", 0, 'A' },
		{ "Ullage options", 0, 'U' },

		{ "Iteration flag", 0, 'I' },
		{ "DPS 10% time", 0, 'P' },
		{ "DPS scale factor", 0, 'S' },
		{ "Time flag", 0, 'F' },
		{ "Calc maneuver", 0, 'C' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu54, sizeof(mnu54) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TAB", OAPI_KEY_E, &ApolloRTCCMFD::menuCycleTITable);
	RegisterFunction("PLN", OAPI_KEY_N, &ApolloRTCCMFD::menuSetTIPlanNumber);
	RegisterFunction("DEL", OAPI_KEY_D, &ApolloRTCCMFD::menuTIDeleteGET);
	RegisterFunction("THR", OAPI_KEY_T, &ApolloRTCCMFD::menuChooseTIThruster);
	RegisterFunction("ATT", OAPI_KEY_A, &ApolloRTCCMFD::menuCycleTIAttitude);
	RegisterFunction("ULL", OAPI_KEY_U, &ApolloRTCCMFD::menuTIUllageOption);

	RegisterFunction("ITE", OAPI_KEY_I, &ApolloRTCCMFD::menuCycleTIIterationFlag);
	RegisterFunction("10P", OAPI_KEY_P, &ApolloRTCCMFD::menuTIDPSTenPercentTime);
	RegisterFunction("DPS", OAPI_KEY_S, &ApolloRTCCMFD::menuTIDPSScaleFactor);
	RegisterFunction("TIM", OAPI_KEY_F, &ApolloRTCCMFD::menuCycleTITimeFlag);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTransferTIToMPT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetLambertPage);


	static const MFDBUTTONMENU mnu55[] =
	{
		{ "GET for deletion", 0, 'D' },
		{ "Select Thruster", 0, 'T' },
		{ "Attitude option", 0, 'A' },
		{ "Ullage options", 0, 'E' },
		{ "Iteration flag", 0, 'I' },
		{ "Time flag", 0, 'G' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Transfer to MPT", 0, 'C' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu55, sizeof(mnu55) / sizeof(MFDBUTTONMENU));

	RegisterFunction("DEL", OAPI_KEY_D, &ApolloRTCCMFD::menuM70DeleteGET);
	RegisterFunction("THR", OAPI_KEY_T, &ApolloRTCCMFD::menuChooseSPQDKIThruster);
	RegisterFunction("ATT", OAPI_KEY_A, &ApolloRTCCMFD::menuM70CycleAttitude);
	RegisterFunction("ULL", OAPI_KEY_E, &ApolloRTCCMFD::menuM70UllageOption);
	RegisterFunction("ITE", OAPI_KEY_I, &ApolloRTCCMFD::menuM70CycleIterationFlag);
	RegisterFunction("TIM", OAPI_KEY_G, &ApolloRTCCMFD::menuM70CycleTimeFlag);
	
	RegisterFunction("10P", OAPI_KEY_F, &ApolloRTCCMFD::menuM70DPSTenPercentTime);
	RegisterFunction("DPS", OAPI_KEY_P, &ApolloRTCCMFD::menuM70DPSScaleFactor);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTransferSPQorDKIToMPT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuBackToSPQorDKIPage);


	static const MFDBUTTONMENU mnu56[] =
	{
		{ "CSM or LEM", 0, 'V' },
		{ "Input option", 0, 'R' },
		{ "Time of ignition", 0, 'I' },
		{ "Thruster", 0, 'T' },
		{ "Attitude mode", 0, 'G' },
		{ "Input attitude", 0, 'A' },

		{ "Choose thruster", 0, 'C' },
		{ "Heads up/down", 0, 'H' },
		{ "DPS DT at 10%", 0, 'P' },
		{ "DPS thrust", 0, 'F' },
		{ "Move to MPT", 0, 'M' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu56, sizeof(mnu56) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MPT", OAPI_KEY_V, &ApolloRTCCMFD::menuMPTDirectInputMPTCode);
	RegisterFunction("REP", OAPI_KEY_R, &ApolloRTCCMFD::menuMPTDirectInputReplaceCode);
	RegisterFunction("TIG", OAPI_KEY_I, &ApolloRTCCMFD::menuMPTDirectInputTIG);
	RegisterFunction("THR", OAPI_KEY_T, &ApolloRTCCMFD::menuChooseMPTDirectInputThruster);
	RegisterFunction("ATT", OAPI_KEY_G, &ApolloRTCCMFD::menuMPTDirectInputAttitude);
	RegisterFunction("BPA", OAPI_KEY_A, &ApolloRTCCMFD::menuMPTDirectInputBurnParameters);

	RegisterFunction("COO", OAPI_KEY_C, &ApolloRTCCMFD::menuMPTDirectInputCoord);
	RegisterFunction("HEA", OAPI_KEY_H, &ApolloRTCCMFD::menuMPTDirectInputHeadsUpDown);
	RegisterFunction("10P", OAPI_KEY_P, &ApolloRTCCMFD::menuMPTDirectInputDPSTenPercentTime);
	RegisterFunction("THR", OAPI_KEY_F, &ApolloRTCCMFD::menuMPTDirectInputDPSScaleFactor);
	RegisterFunction("PAG", OAPI_KEY_M, &ApolloRTCCMFD::menuMPTDirectInputSecondPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMPTPage);


	static const MFDBUTTONMENU mnu57[] =
	{
		{ "Table code", 0, 'V' },
		{ "Replace code", 0, 'R' },
		{ "Select thruster", 0, 'T' },
		{ "Attitude mode", 0, 'A' },
		{ "Ullage DT", 0, 'U' },
		{ "Ullage thrusters", 0, 'W' },

		{ "Iteration flag", 0, 'I' },
		{ "DPS 10% time", 0, 'P' },
		{ "DPS thrust scale", 0, 'S' },
		{ "Time flag", 0, 'F' },
		{ "Transfer to MPT", 0, 'C' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu57, sizeof(mnu57) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TAB", OAPI_KEY_V, &ApolloRTCCMFD::menuCycleGPMTable);
	RegisterFunction("REP", OAPI_KEY_R, &ApolloRTCCMFD::menuGPMReplaceCode);
	RegisterFunction("THR", OAPI_KEY_T, &ApolloRTCCMFD::menuChooseGPMThruster);
	RegisterFunction("ATT", OAPI_KEY_A, &ApolloRTCCMFD::menuCycleGPMAttitude);
	RegisterFunction("UDT", OAPI_KEY_U, &ApolloRTCCMFD::menuGPMUllageDT);
	RegisterFunction("UTH", OAPI_KEY_W, &ApolloRTCCMFD::menuGPMUllageThrusters);
	
	RegisterFunction("ITE", OAPI_KEY_I, &ApolloRTCCMFD::menuCycleGPMIterationFlag);
	RegisterFunction("DT", OAPI_KEY_P, &ApolloRTCCMFD::menuGPMDPSTenPercentDeltaT);
	RegisterFunction("PCT", OAPI_KEY_S, &ApolloRTCCMFD::menuGPMDPSThrustScaling);
	RegisterFunction("TIM", OAPI_KEY_F, &ApolloRTCCMFD::menuCycleGPMTimeFlag);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTransferGPMToMPT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetOrbAdjPage);


	static const MFDBUTTONMENU mnu58[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu58, sizeof(mnu58) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::CheckoutMonitorCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu59[] =
	{
		{ "Choose Table", 0, 'T' },
		{ "Choose MED", 0, 'E' },
		{ "Update MPT", 0, 'M' },
		{ "Previous Item", 0, 'P' },
		{ "Next Item", 0, 'N' },
		{ "Set input", 0, 'S' },

		{ "Select vehicle", 0, 'D' },
		{ "Auto update of parameters", 0, 'U' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Vector Panel Summary", 0, 'P' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu59, sizeof(mnu59) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TAB", OAPI_KEY_A, &ApolloRTCCMFD::menuCycleMPTTable);
	RegisterFunction("MED", OAPI_KEY_E, &ApolloRTCCMFD::menuCycleMPTMED);
	RegisterFunction("UPD", OAPI_KEY_M, &ApolloRTCCMFD::menuMPTUpdate);
	RegisterFunction("<<", OAPI_KEY_P, &ApolloRTCCMFD::menuCycleMarkerDown);
	RegisterFunction(">>", OAPI_KEY_N, &ApolloRTCCMFD::menuCycleMarkerUp);
	RegisterFunction("SET", OAPI_KEY_S, &ApolloRTCCMFD::menuSetMPTInitInput);

	RegisterFunction("TGT", OAPI_KEY_D, &ApolloRTCCMFD::menuMPTInitM50M55Vehicle);
	RegisterFunction("AUT", OAPI_KEY_U, &ApolloRTCCMFD::menuMPTInitAutoUpdate);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("VPS", OAPI_KEY_P, &ApolloRTCCMFD::menuVectorPanelSummaryPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu60[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate maneuver", 0, 'C' },
		{ "Save time of landing", 0, 'F' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Transfer to MPT", 0, 'U' },
		{ "Back to calc page", 0, 'B' },
	};

	RegisterPage(mnu60, sizeof(mnu60) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLDPPCalc);
	RegisterFunction("TLA", OAPI_KEY_F, &ApolloRTCCMFD::menuLDPPSaveTLAND);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MPT", OAPI_KEY_U, &ApolloRTCCMFD::menuSetSPQorDKIRTransferPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetDescPlanCalcPage);


	static const MFDBUTTONMENU mnu61[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink to LGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu61, sizeof(mnu61) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTLANDUplinkCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuTLANDUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu62[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate times", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu62, sizeof(mnu62) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuSunriseSunsetTimesCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu63[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate times", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu63, sizeof(mnu63) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuMoonriseMoonsetTimesCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu64[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu64, sizeof(mnu64) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu65[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu65, sizeof(mnu65) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu66[] =
	{
		{ "Choose Mode", 0, 'Q' },
		{ "Choose page for solution", 0, 'H' },
		{ "Choose landing site", 0, 'G' },
		{ "Vector time", 0, 'U' },
		{ "Minimum time", 0, 'V' },
		{ "Maximum time", 0, 'L' },

		{ "Cycle through pages", 0, 'P' },
		{ "Calc tradeoff display", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Entry profile", 0, 'E' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu66, sizeof(mnu66) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MOD", OAPI_KEY_Q, &ApolloRTCCMFD::menuSetRTETradeoffMode);
	RegisterFunction("REM", OAPI_KEY_H, &ApolloRTCCMFD::menuSetRTETradeoffRemoteEarthPage);
	RegisterFunction("SIT", OAPI_KEY_G, &ApolloRTCCMFD::menuSetRTETradeoffSite);
	RegisterFunction("TV", OAPI_KEY_U, &ApolloRTCCMFD::menuSetRTETradeoffVectorTime);
	RegisterFunction("MIN", OAPI_KEY_V, &ApolloRTCCMFD::menuSetRTETradeoffT0MinTime);
	RegisterFunction("MAX", OAPI_KEY_L, &ApolloRTCCMFD::menuSetRTETradeoffT0MaxTime);

	RegisterFunction("PAG", OAPI_KEY_P, &ApolloRTCCMFD::menuCycleRTETradeoffPage);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalcRTETradeoff);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("ENT", OAPI_KEY_E, &ApolloRTCCMFD::menuSetRTETradeoffEntryProfile);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetReturnToEarthPage);


	static const MFDBUTTONMENU mnu67[] =
	{
		{ "Generate DMT", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu67, sizeof(mnu67) / sizeof(MFDBUTTONMENU));

	RegisterFunction("U20", OAPI_KEY_G, &ApolloRTCCMFD::menuGenerateDMT);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu68[] =
	{
		{ "Generate DMT", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu68, sizeof(mnu68) / sizeof(MFDBUTTONMENU));

	RegisterFunction("U20", OAPI_KEY_G, &ApolloRTCCMFD::menuGenerateDMT);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu69[] =
	{
		{ "Coelliptic DH", 0, 'D' },
		{ "Elevation angle at TPI", 0, 'E' },
		{ "Angle between TPI and TPF", 0, 'W' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "TPI time", 0, 'Q' },

		{ "CDH option", 0, 'C' },
		{ "CDH value", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu69, sizeof(mnu69) / sizeof(MFDBUTTONMENU));

	RegisterFunction("DH", OAPI_KEY_D, &ApolloRTCCMFD::SPQDHdialogue);
	RegisterFunction("E", OAPI_KEY_E, &ApolloRTCCMFD::menuSetSPQElevation);
	RegisterFunction("WT", OAPI_KEY_W, &ApolloRTCCMFD::menuSetSPQTerminalPhaseAngle);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TPI", OAPI_KEY_Q, &ApolloRTCCMFD::menuSetSPQTPIDefinitionValue);

	RegisterFunction("CDH", OAPI_KEY_C, &ApolloRTCCMFD::menuCycleSPQCDHPoint);
	RegisterFunction("VAL", OAPI_KEY_D, &ApolloRTCCMFD::menuSPQCDHValue);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetSPQPage);


	static const MFDBUTTONMENU mnu70[] =
	{
		{ "DH at NCC", 0, 'V' },
		{ "DH at NSR", 0, 'D' },
		{ "Elevation angle at TPI", 0, 'E' },
		{ "Angle from TPI to TPF", 0, 'L' },
		{ "Minimum perigee", 0, 'H' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu70, sizeof(mnu70) / sizeof(MFDBUTTONMENU));

	RegisterFunction("DH1", OAPI_KEY_V, &ApolloRTCCMFD::menuDKINCCDHInput);
	RegisterFunction("DH2", OAPI_KEY_D, &ApolloRTCCMFD::menuDKINSRDHInput);
	RegisterFunction("E", OAPI_KEY_E, &ApolloRTCCMFD::menuSetDKIElevation);
	RegisterFunction("WT", OAPI_KEY_L, &ApolloRTCCMFD::menuSetDKITerminalPhaseAngle);
	RegisterFunction("MIN", OAPI_KEY_H, &ApolloRTCCMFD::menuSetDKIMinimumPerigee);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetDKIPage);


	static const MFDBUTTONMENU mnu71[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "Calculate longitude from GET", 0, 'G' },
		{ "Calculate GET from longitude", 0, 'L' },
		{ "Calc apo/peri values from GET", 0, 'R' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu71, sizeof(mnu71) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("GETL", OAPI_KEY_G, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETL);
	RegisterFunction("L", OAPI_KEY_L, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsL);
	RegisterFunction("GETR", OAPI_KEY_R, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETBV);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu72[] =
	{
		{ "Cycle pages", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate contacts", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu72, sizeof(mnu72) / sizeof(MFDBUTTONMENU));

	RegisterFunction("PAG", OAPI_KEY_P, &ApolloRTCCMFD::CyclePredSiteAcqPage);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::PredSiteAcqLM1Calc);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu73[] =
	{
		{ "Cycle pages", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate contacts", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu73, sizeof(mnu73) / sizeof(MFDBUTTONMENU));

	RegisterFunction("PAG", OAPI_KEY_P, &ApolloRTCCMFD::CyclePredSiteAcqPage);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::PredSiteAcqCSM2Calc);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu74[] =
	{
		{ "Cycle pages", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate contacts", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu74, sizeof(mnu74) / sizeof(MFDBUTTONMENU));

	RegisterFunction("PAG", OAPI_KEY_P, &ApolloRTCCMFD::CyclePredSiteAcqPage);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::PredSiteAcqLM2Calc);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu75[] =
	{
		{ "General MED Input", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu75, sizeof(mnu75) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MED", OAPI_KEY_G, &ApolloRTCCMFD::menuGeneralMEDRequest);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu76[] =
	{
		{ "Table code", 0, 'V' },
		{ "Replace code", 0, 'R' },
		{ "Maneuver number", 0, 'N' },
		{ "Select thruster", 0, 'T' },
		{ "Attitude mode", 0, 'A' },
		{ "Ullage thrusters and DT", 0, 'U' },

		{ "Iteration flag", 0, 'I' },
		{ "DPS 10% time", 0, 'P' },
		{ "DPS thrust scale", 0, 'S' },
		{ "Time flag", 0, 'F' },
		{ "Transfer to MPT", 0, 'C' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu76, sizeof(mnu76) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TAB", OAPI_KEY_V, &ApolloRTCCMFD::menuCycleLOIMCCTable);
	RegisterFunction("REP", OAPI_KEY_R, &ApolloRTCCMFD::menuLOIMCCReplaceCode);
	RegisterFunction("NUM", OAPI_KEY_N, &ApolloRTCCMFD::menuLOIMCCManeuverNumber);
	RegisterFunction("THR", OAPI_KEY_T, &ApolloRTCCMFD::menuChooseLOIMCCThruster);
	RegisterFunction("ATT", OAPI_KEY_A, &ApolloRTCCMFD::menuCycleLOIMCCAttitude);
	RegisterFunction("UDT", OAPI_KEY_U, &ApolloRTCCMFD::menuLOIMCCUllageThrustersDT);

	RegisterFunction("ITE", OAPI_KEY_I, &ApolloRTCCMFD::menuCycleLOIMCCIterationFlag);
	RegisterFunction("DT", OAPI_KEY_P, &ApolloRTCCMFD::menuLOIMCCDPSTenPercentDeltaT);
	RegisterFunction("PCT", OAPI_KEY_S, &ApolloRTCCMFD::menuLOIMCCDPSThrustScaling);
	RegisterFunction("TIM", OAPI_KEY_F, &ApolloRTCCMFD::menuCycleLOIMCCTimeFlag);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTransferLOIMCCtoMPT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuBacktoLOIorMCCPage);


	static const MFDBUTTONMENU mnu77[] =
	{
		{ "Cycle between SFP blocks", 0, 'D' },
		{ "Change entry in SFP", 0, 'S' },
		{ "Interpolation for SFP", 0, 'V' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu77, sizeof(mnu77) / sizeof(MFDBUTTONMENU));

	RegisterFunction("F31", OAPI_KEY_D, &ApolloRTCCMFD::menuCycleSFPDisplay);
	RegisterFunction("F32", OAPI_KEY_S, &ApolloRTCCMFD::menuAlterationSFPData);
	RegisterFunction("F62", OAPI_KEY_V, &ApolloRTCCMFD::menuInterpolateSFP);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuTranslunarPage);


	static const MFDBUTTONMENU mnu78[] =
	{
		{ "Transfer plan to SFP", 0, 'T' },
		{ "Delete column", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calc. maneuver", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Choose engine", 0, 'E' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu78, sizeof(mnu78) / sizeof(MFDBUTTONMENU));

	RegisterFunction("F30", OAPI_KEY_T, &ApolloRTCCMFD::menuTransferMCCPlanToSFP);
	RegisterFunction("F26", OAPI_KEY_D, &ApolloRTCCMFD::menuDeleteMidcourseColumn);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTLCCCalc);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("ENG", OAPI_KEY_E, &ApolloRTCCMFD::menuMCCTransferPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuMidcoursePage);


	static const MFDBUTTONMENU mnu79[] =
	{
		{ "Set input", 0, 'S' },
		{ "Previous Item", 0, 'P' },
		{ "Next Item", 0, 'N' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate solution", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Transfer to MPT", 0, 'S' },
		{ "Uplink to LVDC", 0, 'E' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu79, sizeof(mnu79) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SET", OAPI_KEY_S, &ApolloRTCCMFD::menuSetTLIProcessorInput);
	RegisterFunction("<<", OAPI_KEY_P, &ApolloRTCCMFD::menuCycleMarkerDown);
	RegisterFunction(">>", OAPI_KEY_N, &ApolloRTCCMFD::menuCycleMarkerUp);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTLIProcessorCalc);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MPT", OAPI_KEY_S, &ApolloRTCCMFD::SetMEDInputPageM75);
	RegisterFunction("UPL", OAPI_KEY_E, &ApolloRTCCMFD::menuSLVTLITargetingUplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuTranslunarPage);


	static const MFDBUTTONMENU mnu80[] =
	{
		{ "Azimuth constraints", 0, 'A' },
		{ "TLC times constraints", 0, 'T' },
		{ "Reentry constraints", 0, 'R' },
		{ "Height limits", 0, 'H' },
		{ "Latitude bias", 0, 'L' },
		{ "Max powered incl", 0, 'C' },

		{ "LOI ellipse heights", 0, 'K' },
		{ "DOI ellipse heights", 0, 'D' },
		{ "Revs in LOI/DOI ellipse", 0, 'O' },
		{ "True anomaly at PDI", 0, 'S' },
		{ "Revs for LOPC calc", 0, 'P' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu80, sizeof(mnu80) / sizeof(MFDBUTTONMENU));

	RegisterFunction("F22", OAPI_KEY_A, &ApolloRTCCMFD::menuSetTLMCCAzimuthConstraints);
	RegisterFunction("F23", OAPI_KEY_T, &ApolloRTCCMFD::menuSetTLMCCTLCTimesConstraints);
	RegisterFunction("F24", OAPI_KEY_R, &ApolloRTCCMFD::menuSetTLMCCReentryContraints);
	RegisterFunction("F29", OAPI_KEY_H, &ApolloRTCCMFD::menuSetTLMCCPericynthionHeightLimits);
	RegisterFunction("LAT", OAPI_KEY_L, &ApolloRTCCMFD::menuSetTLMCCLatitudeBias);
	RegisterFunction("INC", OAPI_KEY_C, &ApolloRTCCMFD::menuSetTLMCCMaxInclination);

	RegisterFunction("LOI", OAPI_KEY_K, &ApolloRTCCMFD::menuSetTLMCCLOIEllipseHeights);
	RegisterFunction("DOI", OAPI_KEY_D, &ApolloRTCCMFD::menuSetTLMCCDOIEllipseHeights);
	RegisterFunction("REV", OAPI_KEY_O, &ApolloRTCCMFD::menuSetTLMCCLOIDOIRevs);
	RegisterFunction("ROT", OAPI_KEY_S, &ApolloRTCCMFD::menuSetTLMCCLSRotation);
	RegisterFunction("PC", OAPI_KEY_P, &ApolloRTCCMFD::menuSetTLMCCLOPCRevs);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuTranslunarPage);


	static const MFDBUTTONMENU mnu81[] =
	{
		{ "Node conv. option", 0, 'O' },
		{ "Node conv.n GET", 0, 'G' },
		{ "Node conv. latitude", 0, 'A' },
		{ "Node conv. longitude", 0, 'L' },
		{ "Nodal height", 0, 'H' },
		{ "", 0, ' ' },

		{ "Convert", 0, 'C' },
		{ "Send node to SFP 2", 0, 'S' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu81, sizeof(mnu81) / sizeof(MFDBUTTONMENU));

	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::menuCycleNodeConvOption);
	RegisterFunction("GET", OAPI_KEY_G, &ApolloRTCCMFD::menuSetNodeConvGET);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuSetNodeConvLat);
	RegisterFunction("LNG", OAPI_KEY_L, &ApolloRTCCMFD::menuSetNodeConvLng);
	RegisterFunction("HE", OAPI_KEY_H, &ApolloRTCCMFD::menuSetNodeConvHeight);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuNodeConvCalc);
	RegisterFunction("SFP", OAPI_KEY_S, &ApolloRTCCMFD::menuSendNodeToSFP);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu82[] =
	{
		{ "Apolune at LLS", 0, 'A' },
		{ "Perilune at LLS", 0, 'P' },
		{ "Angle of perilune from LLS", 0, 'D' },
		{ "Revs in LPO1", 0, 'R' },
		{ "Revs in LPO2", 0, 'S' },
		{ "True anomaly at LOI", 0, 'E' },

		{ "Height bias at DOI", 0, 'H' },
		{ "Plane for inters. soln", 0, 'I' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu82, sizeof(mnu82) / sizeof(MFDBUTTONMENU));

	RegisterFunction("HA", OAPI_KEY_A, &ApolloRTCCMFD::menuSetLOI_HALLS);
	RegisterFunction("HP", OAPI_KEY_P, &ApolloRTCCMFD::menuSetLOI_HPLLS);
	RegisterFunction("DW", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLOIDW);
	RegisterFunction("R1", OAPI_KEY_R, &ApolloRTCCMFD::menuSetLOIRevs1);
	RegisterFunction("R2", OAPI_KEY_S, &ApolloRTCCMFD::menuSetLOIRevs2);
	RegisterFunction("ETA", OAPI_KEY_E, &ApolloRTCCMFD::menuSetLOIEta1);

	RegisterFunction("DHB", OAPI_KEY_H, &ApolloRTCCMFD::menuSetLOIDHBias);
	RegisterFunction("PLA", OAPI_KEY_I, &ApolloRTCCMFD::menuCycleLOIInterSolnFlag);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetLOIPage);


	static const MFDBUTTONMENU mnu83[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate LOI Solutions", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Choose engine", 0, 'E' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu83, sizeof(mnu83) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLOICalc);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("ENG", OAPI_KEY_E, &ApolloRTCCMFD::menuLOITransferPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetLOIPage);


	static const MFDBUTTONMENU mnu84[] =
	{
		{ "Choose ullage DT", 0, 'U' },
		{ "Choose ullage thrusters", 0, 'T' },
		{ "REFSMMAT", 0, 'R' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu84, sizeof(mnu84) / sizeof(MFDBUTTONMENU));

	RegisterFunction("UDT", OAPI_KEY_U, &ApolloRTCCMFD::menuMPTDirectInputUllageDT);
	RegisterFunction("UTH", OAPI_KEY_T, &ApolloRTCCMFD::menuMPTDirectInputUllageThrusters);
	RegisterFunction("REF", OAPI_KEY_R, &ApolloRTCCMFD::menuMPTDirectInputREFSMMAT);
	RegisterFunction("DOC", OAPI_KEY_D, &ApolloRTCCMFD::menuMPTDirectInputDock);
	RegisterFunction("CFG", OAPI_KEY_C, &ApolloRTCCMFD::menuMPTDirectInputFinalConfig);
	RegisterFunction("DDA", OAPI_KEY_D, &ApolloRTCCMFD::menuMPTDirectInputDeltaDockingAngle);

	RegisterFunction("TRM", OAPI_KEY_S, &ApolloRTCCMFD::menuMPTDirectInputTrimAngleInd);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuMPTDirectInputTransfer);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("PAG", OAPI_KEY_E, &ApolloRTCCMFD::menuSetMPTDirectInputPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMPTPage);


	static const MFDBUTTONMENU mnu85[] =
	{
		{ "Cycle pages", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate contacts", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu85, sizeof(mnu85) / sizeof(MFDBUTTONMENU));

	RegisterFunction("PAG", OAPI_KEY_P, &ApolloRTCCMFD::CycleExpSiteAcqPage);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::ExpSiteAcqLMCalc);
	RegisterFunction("ADD", OAPI_KEY_A, &ApolloRTCCMFD::GroundPointTableUpdate);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu86[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate RMD", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu86, sizeof(mnu86) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::RelativeMotionDigitalsCalc);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu87[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Choose plan", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu87, sizeof(mnu87) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("U06", OAPI_KEY_C, &ApolloRTCCMFD::menuChooseRETPlan);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRendezvousPage);


	static const MFDBUTTONMENU mnu88[] =
	{
		{ "Powered flight arc", 0, ' ' },
		{ "Powered flight time", 0, ' ' },
		{ "Insertion height", 0, ' ' },
		{ "Insertion Zdot", 0, 'L' },
		{ "Insertion Rdot", 0, 'h' },
		{ "Yaw steer capability", 0, ' ' },

		{ "Max asc stage life", 0, ' ' },
		{ "Min safe height", 0, ' ' },
		{ "LM Max DV", 0, ' ' },
		{ "CSM Max DV", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu88, sizeof(mnu88) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("VLH", OAPI_KEY_L, &ApolloRTCCMFD::menuLunarLiftoffVHorInput);
	RegisterFunction("VLV", OAPI_KEY_H, &ApolloRTCCMFD::menuLunarLiftoffVVertInput);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetLLWPInitPage);


	static const MFDBUTTONMENU mnu89[] =
	{
		{ "Targeting parameters", 0, 'P' },
		{ "CSM CSI time bias", 0, ' ' },
		{ "Elevation Angle", 0, 'E' },
		{ "TP transfer angle", 0, ' ' },
		{ "TPF height offset", 0, ' ' },
		{ "TPF phase offset", 0, ' ' },

		{ "LW scan begin", 0, ' ' },
		{ "LW scan increment", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu89, sizeof(mnu89) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TAR", OAPI_KEY_P, &ApolloRTCCMFD::menuSetLaunchTargetingInitPage);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("E", OAPI_KEY_E, &ApolloRTCCMFD::menuSetLLWPElevation);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetLunarLiftoffPage);


	static const MFDBUTTONMENU mnu90[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu90, sizeof(mnu90) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLunarLiftoffCalc);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetLunarLiftoffPage);


	static const MFDBUTTONMENU mnu91[] =
	{
		{ "Threshold time", 0, 'H' },
		{ "Vector time", 0, 'V' },
		{ "Radial velocity", 0, 'R' },
		{ "DT Ins. to TPI", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate solution", 0, 'C' },
		{ "Select CSM", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu91, sizeof(mnu91) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TTH", OAPI_KEY_H, &ApolloRTCCMFD::menuLLTPThresholdTime);
	RegisterFunction("VTI", OAPI_KEY_V, &ApolloRTCCMFD::menuLLTPVectorTime);
	RegisterFunction("RDO", OAPI_KEY_R, &ApolloRTCCMFD::menuLunarLiftoffVVertInput);
	RegisterFunction("DT", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLiftoffDT);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLLTPCalc);
	RegisterFunction("CSM", OAPI_KEY_A, &ApolloRTCCMFD::set_CSMVessel);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu92[] =
	{
		{ "Set CSM", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu92, sizeof(mnu92) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_TargetVessel);
	RegisterFunction("MOD", OAPI_KEY_Q, &ApolloRTCCMFD::menuCycleTPIMode);
	RegisterFunction("DT", OAPI_KEY_V, &ApolloRTCCMFD::TPIDTDialogue);
	RegisterFunction("TPI", OAPI_KEY_L, &ApolloRTCCMFD::menuSetTPIguess);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalculateTPITime);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRendezvousPage);


	static const MFDBUTTONMENU mnu93[] =
	{
		{ "Enter vector time", 0, 'T' },
		{ "Enter vehicle", 0, 'H' },
		{ "Column 1", 0, ' ' },
		{ "Column 2", 0, ' ' },
		{ "Column 3", 0, ' ' },
		{ "Column 4", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu93, sizeof(mnu93) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIM", OAPI_KEY_T, &ApolloRTCCMFD::menuVectorCompareTime);
	RegisterFunction("VEH", OAPI_KEY_H, &ApolloRTCCMFD::menuVectorCompareVehicle);
	RegisterFunction("V1", OAPI_KEY_Q, &ApolloRTCCMFD::menuVectorCompareColumn1);
	RegisterFunction("V2", OAPI_KEY_P, &ApolloRTCCMFD::menuVectorCompareColumn2);
	RegisterFunction("V3", OAPI_KEY_V, &ApolloRTCCMFD::menuVectorCompareColumn3);
	RegisterFunction("V4", OAPI_KEY_L, &ApolloRTCCMFD::menuVectorCompareColumn4);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuVectorCompareDisplayCalc);
	RegisterFunction("REF", OAPI_KEY_A, &ApolloRTCCMFD::menuVectorCompareReference);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu94[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink option", 0, 'K' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu94, sizeof(mnu94) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuREFSMMATUplinkCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::UploadREFSMMAT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu95[] =
	{
		{ "Display REFSMMAT", 0, 'T' },
		{ "Enter attitude", 0, 'D' },
		{ "Enter sextant data", 0, 'G' },
		{ "SCT/COAS calculation", 0, 'E' },
		{ "SXT calculation", 0, 'V' },
		{ "Enter star in catalog", 0, 'A' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Save OST REFSMMAT", 0, ' ' },
		{ "Show star vector", 0, 'S' },
		{ "Show landmark vector", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu95, sizeof(mnu95) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAT", OAPI_KEY_T, &ApolloRTCCMFD::menuGOSTDisplayREFSMMAT);
	RegisterFunction("ATT", OAPI_KEY_D, &ApolloRTCCMFD::menuGOSTEnterAttitude);
	RegisterFunction("ANG", OAPI_KEY_G, &ApolloRTCCMFD::menuGOSTEnterSXTData);
	RegisterFunction("BST", OAPI_KEY_E, &ApolloRTCCMFD::menuGOSTBoresightSCTCalc);
	RegisterFunction("SXT", OAPI_KEY_V, &ApolloRTCCMFD::menuGOSTSXTCalc);
	RegisterFunction("G13", OAPI_KEY_A, &ApolloRTCCMFD::menuGOSTEnterStarInCatalog);

	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("G11", OAPI_KEY_C, &ApolloRTCCMFD::menuSaveOSTREFSMMAT);
	RegisterFunction("UNI", OAPI_KEY_S, &ApolloRTCCMFD::menuGOSTShowStarVector);
	RegisterFunction("LMK", OAPI_KEY_U, &ApolloRTCCMFD::menuGOSTShowLandmarkVector);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu96[] =
	{
		{ "Set SV time", 0, 'G' },
		{ "Get GRR time from LVDC", 0, 'Q' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Set target", 0, 'T' },
		{ "Calculate SV", 0, 'C' },
		{ "Uplink SV to LVDC", 0, 'U' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu96, sizeof(mnu96) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIM", OAPI_KEY_G, &ApolloRTCCMFD::menuSetSVTime);
	RegisterFunction("GRR", OAPI_KEY_Q, &ApolloRTCCMFD::menuUpdateGRRTime);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_IUVessel);
	RegisterFunction("CLC", OAPI_KEY_A, &ApolloRTCCMFD::menuSLVNavigationUpdateCalc);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuSLVNavigationUpdateUplink);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu97[] =
	{
		{ "Get telemetry vectors", 0, 'T' },
		{ "Move to CSM evaluation table", 0, 'E' },
		{ "Move to CSM usable table", 0, 'U' },
		{ "CSM ground tracking vector", 0, 'G' },
		{ "CSM trajectory update", 0, 'C' },
		{ "", 0, ' ' },

		{ "General MED request", 0, 'M' },
		{ "Move to LM evaluation table", 0, 'F' },
		{ "Move to LM usable table", 0, 'V' },
		{ "LM ground tracking vector", 0, 'H' },
		{ "LM trajectory update", 0, 'L' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu97, sizeof(mnu97) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TLM", OAPI_KEY_T, &ApolloRTCCMFD::menuGetOnboardStateVectors);
	RegisterFunction("EV", OAPI_KEY_E, &ApolloRTCCMFD::menuMoveToEvalTableCSM);
	RegisterFunction("UV", OAPI_KEY_U, &ApolloRTCCMFD::menuMoveToUsableTableCSM);
	RegisterFunction("DC", OAPI_KEY_G, &ApolloRTCCMFD::menuMPTTrajectoryUpdateCSM);
	RegisterFunction("TUP", OAPI_KEY_C, &ApolloRTCCMFD::menuEphemerisUpdateCSM);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("MED", OAPI_KEY_M, &ApolloRTCCMFD::menuGeneralMEDRequest);
	RegisterFunction("EV", OAPI_KEY_F, &ApolloRTCCMFD::menuMoveToEvalTableLEM);
	RegisterFunction("UV", OAPI_KEY_V, &ApolloRTCCMFD::menuMoveToUsableTableLEM);
	RegisterFunction("DC", OAPI_KEY_H, &ApolloRTCCMFD::menuMPTTrajectoryUpdateLEM);
	RegisterFunction("TUP", OAPI_KEY_L, &ApolloRTCCMFD::menuEphemerisUpdateLEM);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMPTInitPage);


	static const MFDBUTTONMENU mnu98[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate LS Uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink RLS", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu98, sizeof(mnu98) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLMLSUplinkCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuLMLSUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);

	static const MFDBUTTONMENU mnu99[] =
	{
		{ "Enable ARM calculations", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to MED input", 0, 'B' },
	};

	RegisterPage(mnu99, sizeof(mnu99) / sizeof(MFDBUTTONMENU));

	RegisterFunction("ENA", OAPI_KEY_G, &ApolloRTCCMFD::CycleEnableCalculation);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuReturnToMEDInput);


	static const MFDBUTTONMENU mnu100[] =
	{
		{ "Enable ARM calculations", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to MED input", 0, 'B' },
	};

	RegisterPage(mnu100, sizeof(mnu100) / sizeof(MFDBUTTONMENU));

	RegisterFunction("ENA", OAPI_KEY_G, &ApolloRTCCMFD::CycleEnableCalculation);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuReturnToMEDInput);


	static const MFDBUTTONMENU mnu101[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
	};

	RegisterPage(mnu101, sizeof(mnu101) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_B, &ApolloRTCCMFD::menuVoid);


	static const MFDBUTTONMENU mnu102[] =
	{
		{ "Manual TIG", 0, 'T' },
		{ "Manual DV", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate P30 Uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink Ext DV", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu102, sizeof(mnu102) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::menuManPADTIG);
	RegisterFunction("DV", OAPI_KEY_D, &ApolloRTCCMFD::menuManPADDV);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuP30UplinkCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuP30Uplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu103[] =
	{
		{ "Choose the engine", 0, 'E' },
		{ "Choose burn mode", 0, 'M' },
		{ "Choose value for burn", 0, 'V' },
		{ "Choose attitude mode", 0, 'A' },
		{ "Choose attitude", 0, 'L' },
		{ "Choose ullage options", 0, 'U' },

		{ "", 0, ' ' },
		{ "Choose gimbal parameters", 0, 'G' },
		{ "Initial bank angle", 0, 'I' },
		{ "G level for bank manuever", 0, 'H' },
		{ "Final bank angle", 0, 'F' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu103, sizeof(mnu103) / sizeof(MFDBUTTONMENU));

	RegisterFunction("ENG", OAPI_KEY_E, &ApolloRTCCMFD::menuSwitchRetrofireEngine);
	RegisterFunction("MOD", OAPI_KEY_M, &ApolloRTCCMFD::menuSwitchRetrofireBurnMode);
	RegisterFunction("VAL", OAPI_KEY_V, &ApolloRTCCMFD::menuChooseRetrofireValue);
	RegisterFunction("ATT", OAPI_KEY_A, &ApolloRTCCMFD::menuSwitchRetrofireAttitudeMode);
	RegisterFunction("LVH", OAPI_KEY_L, &ApolloRTCCMFD::menuChooseRetrofireAttitude);
	RegisterFunction("ULL", OAPI_KEY_U, &ApolloRTCCMFD::menuChooseRetrofireUllage);

	RegisterFunction("MAT", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("GIM", OAPI_KEY_G, &ApolloRTCCMFD::menuSwitchRetrofireGimbalIndicator);
	RegisterFunction("K1", OAPI_KEY_I, &ApolloRTCCMFD::menuChooseRetrofireK1);
	RegisterFunction("GC", OAPI_KEY_H, &ApolloRTCCMFD::menuChooseRetrofireGs);
	RegisterFunction("K2", OAPI_KEY_F, &ApolloRTCCMFD::menuChooseRetrofireK2);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRetrofireSubsystemPage);


	static const MFDBUTTONMENU mnu104[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate deorbit solution", 0, 'C' },
		{ "Save maneuver for PAD or MPT", 0, 'F' },
		{ "Save as DOD REFSMMAT", 0, 'D' },
		{ "Make DOD the current REFSMMAT", 0, 'R' },
		{ "Go to online monitor", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu104, sizeof(mnu104) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuDeorbitCalc);
	RegisterFunction("PAD", OAPI_KEY_F, &ApolloRTCCMFD::menuTransferRTEToMPT);
	RegisterFunction("DOD", OAPI_KEY_D, &ApolloRTCCMFD::menuSaveDODREFSMMAT);
	RegisterFunction("CUR", OAPI_KEY_R, &ApolloRTCCMFD::menuMakeDODREFSMMATCurrent);
	RegisterFunction("ONL", OAPI_KEY_U, &ApolloRTCCMFD::menuSetOnlineMonitorPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetDeorbitPage);


	static const MFDBUTTONMENU mnu105[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to RTE", 0, 'U' },
		{ "Back to TTF", 0, 'B' },
	};

	RegisterPage(mnu105, sizeof(mnu105) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("RTE", OAPI_KEY_U, &ApolloRTCCMFD::menuSetRTEDigitalsPage);
	RegisterFunction("TTF", OAPI_KEY_B, &ApolloRTCCMFD::menuSetDeorbitPage);


	static const MFDBUTTONMENU mnu106[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate recovery targets", 0, 'C' },
		{ "Cycle through pages", 0, 'F' },
		{ "Save target", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu106, sizeof(mnu106) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuRecoveryTargetSelectionCalc);
	RegisterFunction("PAG", OAPI_KEY_F, &ApolloRTCCMFD::menuCycleRecoveryTargetSelectionPages);
	RegisterFunction("SEL", OAPI_KEY_P, &ApolloRTCCMFD::menuSelectRecoveryTarget);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRetrofireSubsystemPage);


	static const MFDBUTTONMENU mnu107[] =
	{
		{ "Cycle AST type", 0, 'T' },
		{ "Enter AST site or area", 0, 'E' },
		{ "CSM or vector time", 0, 'D' },
		{ "Enter abort time", 0, 'G' },
		{ "Max DV or Time", 0, 'A' },
		{ "Enter landing time", 0, 'V' },

		{ "Go to AST display", 0, 'C' },
		{ "Entry profile", 0, 'F' },
		{ "Max. miss distance", 0, 'P' },
		{ "Desired inclination", 0, 'S' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu107, sizeof(mnu107) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TYP", OAPI_KEY_T, &ApolloRTCCMFD::menuCycleASTType);
	RegisterFunction("SIT", OAPI_KEY_E, &ApolloRTCCMFD::menuSetASTSiteOrType);
	RegisterFunction("VTI", OAPI_KEY_D, &ApolloRTCCMFD::menuASTVectorTime);
	RegisterFunction("TIM", OAPI_KEY_G, &ApolloRTCCMFD::menuASTAbortTime);
	RegisterFunction("TDV", OAPI_KEY_A, &ApolloRTCCMFD::menuASTTMAXandDVInput);
	RegisterFunction("TZ", OAPI_KEY_V, &ApolloRTCCMFD::menuASTLandingTime);

	RegisterFunction("AST", OAPI_KEY_C, &ApolloRTCCMFD::menuSetAbortScanTablePage);
	RegisterFunction("ENT", OAPI_KEY_F, &ApolloRTCCMFD::menuASTEntryProfile);
	RegisterFunction("MD", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("INC", OAPI_KEY_S, &ApolloRTCCMFD::menuSetEntryDesiredInclination);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetReturnToEarthPage);


	static const MFDBUTTONMENU mnu108[] =
	{
		{ "Delete row", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Solution", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Go to RTE Digitals page", 0, 'R' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu108, sizeof(mnu108) / sizeof(MFDBUTTONMENU));

	RegisterFunction("DEL", OAPI_KEY_D, &ApolloRTCCMFD::menuDeleteASTRow);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuASTCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("RTE", OAPI_KEY_R, &ApolloRTCCMFD::menuSetRTEDigitalsInputPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetAbortScanTableInputPage);


	static const MFDBUTTONMENU mnu109[] =
	{
		{ "Enter vector time", 0, 'D' },
		{ "Enter ignition time", 0, 'E' },
		{ "Target latitude", 0, 'T' },
		{ "Target longitude", 0, 'G' },
		{ "Reference body", 0, 'A' },
		{ "LVLH Delta V vector", 0, 'L' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Load splashdown target", 0, 'L' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu109, sizeof(mnu109) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VTI", OAPI_KEY_D, &ApolloRTCCMFD::menuRTEDManualVectorTime);
	RegisterFunction("IGN", OAPI_KEY_E, &ApolloRTCCMFD::menuRTEDManualIgnitionTime);
	RegisterFunction("LAT", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("LNG", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("REF", OAPI_KEY_A, &ApolloRTCCMFD::menuCycleRTEDManualReference);
	RegisterFunction("DV", OAPI_KEY_L, &ApolloRTCCMFD::menuEnterRTEDManualDV);

	RegisterFunction("RTE", OAPI_KEY_C, &ApolloRTCCMFD::menuSetRTEDigitalsInputPage);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("SPL", OAPI_KEY_R, &ApolloRTCCMFD::LoadSplashdownTargetToRTEDManualInput);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetReturnToEarthPage);


	static const MFDBUTTONMENU mnu110[] =
	{
		{ "Prim. entry profile", 0, ' ' },
		{ "Prim. initial bank angle", 0, ' ' },
		{ "Prim. G level", 0, ' ' },
		{ "Prim. roll direction", 0, ' ' },
		{ "Prim. target long.", 0, ' ' },
		{ "", 0, ' ' },

		{ "B.U. entry profile", 0, ' ' },
		{ "B.U. initial bank angle", 0, ' ' },
		{ "B.U. G level", 0, ' ' },
		{ "B.U. roll direction", 0, ' ' },
		{ "B.U. target long.", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu110, sizeof(mnu110) / sizeof(MFDBUTTONMENU));

	RegisterFunction("EP", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BA", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("GLE", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("RDI", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("LNG", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("EP", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BA", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("GLE", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("RDI", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("LNG", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetReturnToEarthPage);

	static const MFDBUTTONMENU mnu111[] =
	{
		{ "Compare AGC and RTCC time", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Update liftoff time", 0, 'L' },
		{ "", 0, ' ' },

		{ "Input time increment", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink clock increment", 0, 'R' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu111, sizeof(mnu111) / sizeof(MFDBUTTONMENU));

	RegisterFunction("CLC", OAPI_KEY_D, &ApolloRTCCMFD::menuAGCTimeUpdateComparison);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPD", OAPI_KEY_L, &ApolloRTCCMFD::menuUpdateLiftoffTime);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("INP", OAPI_KEY_C, &ApolloRTCCMFD::menuAGCTimeUpdateCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_R, &ApolloRTCCMFD::menuAGCTimeUpdateUplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu112[] =
	{
		{ "Compare AGC and RTCC time", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Update liftoff time", 0, 'L' },
		{ "", 0, ' ' },

		{ "Input time increment", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink clock increment", 0, 'R' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu112, sizeof(mnu112) / sizeof(MFDBUTTONMENU));

	RegisterFunction("CLC", OAPI_KEY_D, &ApolloRTCCMFD::menuAGCTimeUpdateComparison);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPD", OAPI_KEY_L, &ApolloRTCCMFD::menuUpdateLiftoffTime);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("INP", OAPI_KEY_C, &ApolloRTCCMFD::menuAGCTimeUpdateCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_R, &ApolloRTCCMFD::menuAGCTimeUpdateUplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu113[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Update liftoff time", 0, 'L' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu113, sizeof(mnu113) / sizeof(MFDBUTTONMENU));

	RegisterFunction("CLC", OAPI_KEY_D, &ApolloRTCCMFD::menuAGCLiftoffTimeComparision);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPD", OAPI_KEY_L, &ApolloRTCCMFD::menuUpdateLiftoffTime);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("INP", OAPI_KEY_C, &ApolloRTCCMFD::menuAGCTimeUpdateCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_R, &ApolloRTCCMFD::menuAGCTimeUpdateUplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu114[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Update liftoff time", 0, 'L' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu114, sizeof(mnu114) / sizeof(MFDBUTTONMENU));

	RegisterFunction("CLC", OAPI_KEY_D, &ApolloRTCCMFD::menuAGCLiftoffTimeComparision);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPD", OAPI_KEY_L, &ApolloRTCCMFD::menuUpdateLiftoffTime);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("INP", OAPI_KEY_C, &ApolloRTCCMFD::menuAGCTimeUpdateCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_R, &ApolloRTCCMFD::menuAGCTimeUpdateUplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu115[] =
	{
		{ "Desired TIG", 0, 'E' },
		{ "Estimated burn time", 0, 'T' },
		{ "Estimated pitch", 0, 'G' },
		{ "Estimated yaw", 0, 'L' },
		{ "Impact latitude", 0, 'F' },
		{ "Impact longitude", 0, 'P' },

		{ "S-IVB vessel", 0, 'D' },
		{ "Calculate solution", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu115, sizeof(mnu115) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIG", OAPI_KEY_E, &ApolloRTCCMFD::LUNTAR_TIGInput);
	RegisterFunction("BT", OAPI_KEY_T, &ApolloRTCCMFD::LUNTAR_BTInput);
	RegisterFunction("PIT", OAPI_KEY_G, &ApolloRTCCMFD::LUNTAR_PitchInput);
	RegisterFunction("YAW", OAPI_KEY_L, &ApolloRTCCMFD::LUNTAR_YawInput);
	RegisterFunction("LAT", OAPI_KEY_F, &ApolloRTCCMFD::LUNTAR_LatInput);
	RegisterFunction("LNG", OAPI_KEY_P, &ApolloRTCCMFD::LUNTAR_LngInput);
	
	RegisterFunction("S4B", OAPI_KEY_D, &ApolloRTCCMFD::set_IUVessel);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::LUNTARCalc);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu116[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu116, sizeof(mnu116) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetDeorbitPage);


	static const MFDBUTTONMENU mnu117[] =
	{
		{ "GET of shaping maneuver", 0, 'S' },
		{ "Delta T of sep maneuver", 0, 'D' },
		{ "Thruster for sep/shaping", 0, 'T' },
		{ "DV of sep/shaping", 0, 'V' },
		{ "DT of sep/shaping", 0, 'C' },
		{ "LVLH attitude", 0, 'A' },

		{ "Ullage DT", 0, 'U' },
		{ "Ullage thrusters", 0, 'L' },
		{ "SPS gimbal angles", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu117, sizeof(mnu117) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SHA", OAPI_KEY_S, &ApolloRTCCMFD::menuRetroShapingGET);
	RegisterFunction("SEP", OAPI_KEY_D, &ApolloRTCCMFD::menuRetroSepDeltaTTIG);
	RegisterFunction("THR", OAPI_KEY_T, &ApolloRTCCMFD::menuRetroSepThruster);
	RegisterFunction("DV", OAPI_KEY_V, &ApolloRTCCMFD::menuRetroSepDeltaV);
	RegisterFunction("DT", OAPI_KEY_C, &ApolloRTCCMFD::menuRetroSepDeltaT);
	RegisterFunction("ATT", OAPI_KEY_A, &ApolloRTCCMFD::menuRetroSepAtt);

	RegisterFunction("UDT", OAPI_KEY_U, &ApolloRTCCMFD::menuRetroSepUllageDT);
	RegisterFunction("UTH", OAPI_KEY_L, &ApolloRTCCMFD::menuRetroSepUllageThrusters);
	RegisterFunction("GBL", OAPI_KEY_G, &ApolloRTCCMFD::menuRetroSepGimbalIndicator);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRetrofireSubsystemPage);


	static const MFDBUTTONMENU mnu118[] =
	{
		{ "Inputs for sep/shaping", 0, 'T' },
		{ "Retrofire constraints", 0, 'C' },
		{ "Target Selection", 0, 'L' },
		{ "Deorbit Maneuver", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu118, sizeof(mnu118) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SEP", OAPI_KEY_T, &ApolloRTCCMFD::menuSetRetrofireSeparationInputsPage);
	RegisterFunction("CON", OAPI_KEY_C, &ApolloRTCCMFD::menuSetRetrofireConstraintsPage);
	RegisterFunction("TAR", OAPI_KEY_L, &ApolloRTCCMFD::menuSetRetrofireTargetSelectionPage);
	RegisterFunction("DEO", OAPI_KEY_D, &ApolloRTCCMFD::menuSetDeorbitPage);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu119[] =
	{
		{ "Enter latitude", 0, 'T' },
		{ "Enter longitude", 0, 'L' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu119, sizeof(mnu119) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LAT", OAPI_KEY_T, &ApolloRTCCMFD::menuEnterSplashdownLat);
	RegisterFunction("LNG", OAPI_KEY_L, &ApolloRTCCMFD::menuEnterSplashdownLng);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuEntryUplinkCalc);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuEntryUpdateUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu120[] =
	{
		{ "Cycle pages", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate contacts", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu120, sizeof(mnu120) / sizeof(MFDBUTTONMENU));

	RegisterFunction("PAG", OAPI_KEY_P, &ApolloRTCCMFD::CycleLandmarkAcqDisplayPage);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::LandmarkAcqDisplayCalc);
	RegisterFunction("ADD", OAPI_KEY_A, &ApolloRTCCMFD::GroundPointTableUpdate);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu121[] =
	{
		{ "Liftoff time option", 0, 'O' },
		{ "Input liftoff time", 0, 'L' },
		{ "Radius of insertion", 0, 'R' },
		{ "Velocity of insertion", 0, 'V' },
		{ "Gamma of insertion", 0, 'G' },
		{ "Expected phase angle", 0, 'Q' },

		{ "Set Target", 0, 'T' },
		{ "Diff nod regr flag", 0, 'N' },
		{ "Input diff nod regr", 0, 'M' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu121, sizeof(mnu121) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LOT", OAPI_KEY_O, &ApolloRTCCMFD::menuLWPLiftoffTimeOption);
	RegisterFunction("TLO", OAPI_KEY_L, &ApolloRTCCMFD::menuLWPLiftoffTime);
	RegisterFunction("RINS", OAPI_KEY_R, &ApolloRTCCMFD::menuLWP_RINS);
	RegisterFunction("VINS", OAPI_KEY_V, &ApolloRTCCMFD::menuLWP_VINS);
	RegisterFunction("GINS", OAPI_KEY_G, &ApolloRTCCMFD::menuLWP_GAMINS);
	RegisterFunction("PHA", OAPI_KEY_Q, &ApolloRTCCMFD::menuLWP_PhaseFlags);

	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_TargetVessel);
	RegisterFunction("NOF", OAPI_KEY_N, &ApolloRTCCMFD::menuLWPCycleDELNOF);
	RegisterFunction("DNO", OAPI_KEY_M, &ApolloRTCCMFD::menuLWP_DELNO);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("DIS", OAPI_KEY_C, &ApolloRTCCMFD::menuSetLWPDisplayPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu122[] =
	{
		{ "CSM or LM pad", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Select IU vessel", 0, 'Q' },

		{ "Calculate LWP", 0, 'C' },
		{ "Insertion state vector to MPT", 0, 'A' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu122, sizeof(mnu122) / sizeof(MFDBUTTONMENU));

	RegisterFunction("PAD", OAPI_KEY_P, &ApolloRTCCMFD::menuSLVLaunchTargetingPad);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("IU", OAPI_KEY_Q, &ApolloRTCCMFD::set_IUVessel);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuSLVLaunchTargeting);
	RegisterFunction("MPT", OAPI_KEY_A, &ApolloRTCCMFD::menuSLVInsertionSVtoMPT);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuSLVLaunchUplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetSaturnIBLVDCPage);


	static const MFDBUTTONMENU mnu123[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate solution", 0, 'C' },
		{ "Go to eval display", 0, 'E' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Choose engine", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu123, sizeof(mnu123) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuDKICalc);
	RegisterFunction("EVA", OAPI_KEY_E, &ApolloRTCCMFD::menuSetRendezvousEvaluationDisplayPage);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TRA", OAPI_KEY_U, &ApolloRTCCMFD::menuSetSPQorDKIRTransferPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetDKIPage);


	static const MFDBUTTONMENU mnu124[] =
	{
		{ "Vehicle", 0, 'V' },
		{ "Vehicle or vector time", 0, 'W' },
		{ "Threshold time", 0, 'T' },
		{ "Time increment", 0, 'I' },
		{ "Perigee height", 0, 'P' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu124, sizeof(mnu124) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VEH", OAPI_KEY_V, &ApolloRTCCMFD::CyclePerigeeAdjustVehicle);
	RegisterFunction("VEC", OAPI_KEY_W, &ApolloRTCCMFD::menuPerigeeAdjustVectorTime);
	RegisterFunction("THT", OAPI_KEY_T, &ApolloRTCCMFD::menuPerigeeAdjustThresholdTime);
	RegisterFunction("INC", OAPI_KEY_I, &ApolloRTCCMFD::menuPerigeeAdjustTimeIncrement);
	RegisterFunction("HP", OAPI_KEY_P, &ApolloRTCCMFD::menuPerigeeAdjustHeight);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("DIS", OAPI_KEY_C, &ApolloRTCCMFD::menuSetPerigeeAdjustDisplayPage);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu125[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu125, sizeof(mnu125) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuPerigeeAdjustCalc);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPerigeeAdjustInputPage);


	static const MFDBUTTONMENU mnu126[] =
	{
		{ "Choose mode", 0, 'M' },
		{ "Enter first attitude", 0, 'Q' },
		{ "Enter second attitude", 0, 'P' },
		{ "CSM REFS for DOK", 0, 'H' },
		{ "REFSMMAT 1", 0, 'L' },
		{ "REFSMMAT 2", 0, 'V' },

		{ "Docking alignment", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Realign option", 0, 'R' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu126, sizeof(mnu126) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MOD", OAPI_KEY_M, &ApolloRTCCMFD::menuLOSTMode);
	RegisterFunction("AT1", OAPI_KEY_Q, &ApolloRTCCMFD::menuLOSTAttitude1);
	RegisterFunction("AT2", OAPI_KEY_P, &ApolloRTCCMFD::menuLOSTAttitude2);
	RegisterFunction("CRF", OAPI_KEY_H, &ApolloRTCCMFD::menuLOST_CSM_REFSMMAT);
	RegisterFunction("MA1", OAPI_KEY_L, &ApolloRTCCMFD::menuLOST_REFSMMAT1);
	RegisterFunction("MA2", OAPI_KEY_V, &ApolloRTCCMFD::menuLOST_REFSMMAT2);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalcLOST);
	RegisterFunction("OP1", OAPI_KEY_E, &ApolloRTCCMFD::menuLOSTOptics1);
	RegisterFunction("OP2", OAPI_KEY_Q, &ApolloRTCCMFD::menuLOSTOptics2);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("REA", OAPI_KEY_R, &ApolloRTCCMFD::menuLOSTRealign);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu127[] =
	{
		{ "Choose vessel type", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Select CSM or LM", 0, 'E' },
		{ "Calculate IMU error", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu127, sizeof(mnu127) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VES", OAPI_KEY_P, &ApolloRTCCMFD::CycleCSMOrLMSelection);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("SEL", OAPI_KEY_E, &ApolloRTCCMFD::set_Vessel);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalculateIMUComparison);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu128[] =
	{
		{ "Set item", 0, 'P' },
		{ "Next item", 0, 'Q' },
		{ "Previous item", 0, 'V' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Cycle Page", 0, 'E' },
		{ "Calculate", 0, 'C' },
		{ "", 0, ' ' },
		{ "Save REFSMMAT", 0, 'R' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu128, sizeof(mnu128) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SET", OAPI_KEY_P, &ApolloRTCCMFD::menuSetAGOPInput);
	RegisterFunction("<<", OAPI_KEY_Q, &ApolloRTCCMFD::menuCycleMarkerDown);
	RegisterFunction(">>", OAPI_KEY_V, &ApolloRTCCMFD::menuCycleMarkerUp);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("PAG", OAPI_KEY_E, &ApolloRTCCMFD::menuCycleAGOPPage);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuAGOPCalc);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("REF", OAPI_KEY_R, &ApolloRTCCMFD::menuAGOPSaveREFSMMAT);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRTACFPage);


	static const MFDBUTTONMENU mnu129[] =
	{
		{ "Generalized Optics", 0, 'P' },
		{ "Vector Pointing", 0, 'Q' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu129, sizeof(mnu129) / sizeof(MFDBUTTONMENU));

	RegisterFunction("AGO", OAPI_KEY_P, &ApolloRTCCMFD::menuSetAGOPPage);
	RegisterFunction("VEC", OAPI_KEY_Q, &ApolloRTCCMFD::menuSetVECPOINTPage);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu130[] =
	{
		{ "Set input", 0, 'S' },
		{ "Previous Item", 0, 'P' },
		{ "Next Item", 0, 'N' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate MED", 0, 'C' },
		{ "Go to display", 0, 'E' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu130, sizeof(mnu130) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SET", OAPI_KEY_S, &ApolloRTCCMFD::menuInputMEDData);
	RegisterFunction("<<", OAPI_KEY_P, &ApolloRTCCMFD::menuCycleMarkerDown);
	RegisterFunction(">>", OAPI_KEY_N, &ApolloRTCCMFD::menuCycleMarkerUp);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuMEDInputCalc);
	RegisterFunction("DIS", OAPI_KEY_E, &ApolloRTCCMFD::menuGenericGoToDisplay);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);

	static const MFDBUTTONMENU mnu131[] =
	{
		{ "Set LM vessel", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu131, sizeof(mnu131) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LM", OAPI_KEY_P, &ApolloRTCCMFD::set_LMVessel);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalculateIMUParkingAngles);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);
}

bool ApolloRTCCMFDButtons::SearchForKeysInOtherPages() const
{
	return false;
}