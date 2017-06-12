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
		{ "Configuration", 0, 'S' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
	};

	RegisterPage(mnu0, sizeof(mnu0) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TAR", OAPI_KEY_T, &ApolloRTCCMFD::menuSetTargetingMenu);
	RegisterFunction("PAD", OAPI_KEY_P, &ApolloRTCCMFD::menuSetPADMenu);
	RegisterFunction("UTI", OAPI_KEY_U, &ApolloRTCCMFD::menuSetUtilityMenu);
	RegisterFunction("CFG", OAPI_KEY_C, &ApolloRTCCMFD::menuSetConfigPage);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_B, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);

	static const MFDBUTTONMENU mnu1[] =
	{
		{ "", 0, ' ' },
		{ "Maneuver Time", 0, 'M' },
		{ "Arrival Time", 0, 'A' },
		{ "Number of revolutions", 0, 'N' },
		{ "LVLH maneuver axis", 0, 'U' },
		{ "Gravity sources", 0, 'G' },

		{ "Target Vessel", 0, 'V' },
		//{ "Offset Page", 0, 'P' },
		{ "Calculate burn", 0, 'C' },
		{ "Set Offset", 0, 'O' },
		{ "Set Phasing Angle", 0, 'P' },
		{ "Upload to AGC", 0, 'L' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu1, sizeof(mnu1) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("T1", OAPI_KEY_M, &ApolloRTCCMFD::t1dialogue);
	RegisterFunction("T2", OAPI_KEY_A, &ApolloRTCCMFD::t2dialogue);
	RegisterFunction("N", OAPI_KEY_N, &ApolloRTCCMFD::revdialogue);
	RegisterFunction("AXI", OAPI_KEY_U, &ApolloRTCCMFD::set_lambertaxis);
	RegisterFunction("SPH", OAPI_KEY_G, &ApolloRTCCMFD::set_spherical);
	//RegisterFunction("ORI", OAPI_KEY_O, &ApolloRTCCMFD::set_orient);

	RegisterFunction("TGT", OAPI_KEY_V, &ApolloRTCCMFD::set_target);
	//RegisterFunction("OFF", OAPI_KEY_D, &ApolloRTCCMFD::menuSetOffsetPage);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::lambertcalc);
	RegisterFunction("OFF", OAPI_KEY_O, &ApolloRTCCMFD::offvecdialogue);
	RegisterFunction("PHA", OAPI_KEY_P, &ApolloRTCCMFD::phasedialogue);
	RegisterFunction("UPL", OAPI_KEY_L, &ApolloRTCCMFD::menuP30Upload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);

	static const MFDBUTTONMENU mnu2[] =
	{
		{ "Choose the X offset:", 0, 'X' },
		{ "Choose the Y offset:", 0, 'Y' },
		{ "Choose the Z offset:", 0, 'Z' },
		{ "", 0, ' ' },
		{ "Phase angle", 0, 'P' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Last page", 0, 'L' },
	};

	RegisterPage(mnu2, sizeof(mnu2) / sizeof(MFDBUTTONMENU));

	RegisterFunction("XOF", OAPI_KEY_X, &ApolloRTCCMFD::xdialogue);
	RegisterFunction("YOF", OAPI_KEY_Y, &ApolloRTCCMFD::ydialogue);
	RegisterFunction("ZOF", OAPI_KEY_Z, &ApolloRTCCMFD::zdialogue);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("PHA", OAPI_KEY_P, &ApolloRTCCMFD::phasedialogue);
	RegisterFunction("", OAPI_KEY_B, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_F, &ApolloRTCCMFD::menuSetLambertPage);

	static const MFDBUTTONMENU mnu3[] =
	{
		{ "Time Mode", 0, 'T' },
		{ "Maneuver Time", 0, 'M' },
		{ "Coelliptic DH", 0, 'D' },
		{ "Corrected maneuver time", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Target Vessel", 0, 'V' },
		{ "Calculate burn", 0, 'C' },
		{ "", 0, 'D' },
		{ "", 0, ' ' },
		{ "Upload to AGC", 0, 'L' },
		{ "Previous page", 0, 'B' },
	};

	RegisterPage(mnu3, sizeof(mnu3) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIM", OAPI_KEY_T, &ApolloRTCCMFD::set_CDHtimemode);
	RegisterFunction("CDH", OAPI_KEY_M, &ApolloRTCCMFD::CDHtimedialogue);
	RegisterFunction("DH", OAPI_KEY_D, &ApolloRTCCMFD::DHdialogue);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("TGT", OAPI_KEY_V, &ApolloRTCCMFD::set_target);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::CDHcalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_L, &ApolloRTCCMFD::menuP30Upload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu4[] =
	{
		{ "GET of the Maneuver", 0, 'G' },
		{ "Apoapsis altitude", 0, 'A' },
		{ "Periapsis altitude", 0, 'P' },
		{ "Desired Inclination", 0, 'I' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "Calculate", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Upload to AGC", 0, 'L' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu4, sizeof(mnu4) / sizeof(MFDBUTTONMENU));

	RegisterFunction("GET", OAPI_KEY_T, &ApolloRTCCMFD::OrbAdjGETDialogue);
	RegisterFunction("APO", OAPI_KEY_A, &ApolloRTCCMFD::OrbAdjApoDialogue);
	RegisterFunction("PER", OAPI_KEY_P, &ApolloRTCCMFD::OrbAdjPeriDialogue);
	RegisterFunction("INC", OAPI_KEY_I, &ApolloRTCCMFD::OrbAdjIncDialogue);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::OrbAdjCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_L, &ApolloRTCCMFD::menuP30Upload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu5[] =
	{
		{ "Time of Alignment", 0, 'G' },
		{ "Uplink option", 0, 'K' },
		{ "Heads up/down for P30", 0, 'H' },
		{ "", 0, ' ' },
		{ "REFSMMAT from AGC", 0, 'D' },
		{ "Direct/MCC", 0, 'E' },

		{ "Option", 0, 'O' },
		{ "Calculate", 0, 'C' },
		{ "Upload", 0, 'U' },
		{ "LS Latitude", 0, 'A' },
		{ "LS Longitude", 0, 'L' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu5, sizeof(mnu5) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIM", OAPI_KEY_T, &ApolloRTCCMFD::REFSMMATTimeDialogue);
	RegisterFunction("TYP", OAPI_KEY_K, &ApolloRTCCMFD::cycleREFSMMATupl);
	RegisterFunction("HEA", OAPI_KEY_H, &ApolloRTCCMFD::cycleREFSMMATHeadsUp);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("DWN", OAPI_KEY_D, &ApolloRTCCMFD::GetREFSMMATfromAGC);
	RegisterFunction("MCC", OAPI_KEY_E, &ApolloRTCCMFD::menuREFSMMATdirect);

	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::CycleREFSMMATopt);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::calcREFSMMAT);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::UploadREFSMMAT);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuLSLat);
	RegisterFunction("LNG", OAPI_KEY_L, &ApolloRTCCMFD::menuLSLng);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);

	static const MFDBUTTONMENU mnu6[] =
	{
		{ "Est. Time of Ignition", 0, 'T' },
		{ "Manual longitude or zone", 0, 'L' },
		{ "Des. Landing Long", 0, 'O' },
		{ "Entry Angle", 0, 'A' },
		{ "", 0, ' ' },
		{ "Maneuver Type", 0, 'E' },

		{ "Calculate Entry", 0, 'C' },
		{ "Calculation Mode", 0, 'M' },
		{ "", 0, ' ' },
		{ "Entry Range", 0, 'R' },
		{ "Uplink to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu6, sizeof(mnu6) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::EntryTimeDialogue);
	RegisterFunction("LMO", OAPI_KEY_L, &ApolloRTCCMFD::EntryLongitudeModeDialogue);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::EntryLngDialogue);
	RegisterFunction("ANG", OAPI_KEY_A, &ApolloRTCCMFD::EntryAngDialogue);
	RegisterFunction("OPT", OAPI_KEY_D, &ApolloRTCCMFD::menuSwitchEntryNominal);
	RegisterFunction("TYP", OAPI_KEY_E, &ApolloRTCCMFD::menuSwitchCritical);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuEntryCalc);
	RegisterFunction("MOD", OAPI_KEY_V, &ApolloRTCCMFD::CycleEntryOpt);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("RAN", OAPI_KEY_R, &ApolloRTCCMFD::EntryRangeDialogue);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuEntryUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu7[] =
	{
		{ "Time Mode", 0, 'N' },
		{ "Set Time", 0, 'G' },
		{ "Reference Body", 0, 'U' },
		{ "Set Target", 0, 'T' },
		{ "State Vector Slot", 0, 'D' },
		{ "", 0, ' ' },

		{ "Choose Calculation Mode", 0, 'M' },
		{ "Calculate State Vector", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Upload State Vector", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu7, sizeof(mnu7) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MOD", OAPI_KEY_N, &ApolloRTCCMFD::menuCycleSVTimeMode);
	RegisterFunction("TIM", OAPI_KEY_G, &ApolloRTCCMFD::menuSetSVTime);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_svtarget);
	RegisterFunction("SLT", OAPI_KEY_D, &ApolloRTCCMFD::menuSwitchSVSlot);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("MOD", OAPI_KEY_M, &ApolloRTCCMFD::menuCycleSVMode);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuSVCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_L, &ApolloRTCCMFD::menuSVUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);

	static const MFDBUTTONMENU mnu8[] =
	{
		{ "Mission (GET base)", 0, 'G' },
		{ "Vessel type", 0, 'V' },
		{ "", 0, ' ' },
		{ "Sextant Star Time", 0, 'S' },
		{ "Uplinks in LOS", 0, 'U' },
		{ "", 0, ' ' },

		{ "Set launch MJD", 0, 'M' },
		{ "Set AGC Epoch", 0, 'E' },
		{ "Update liftoff time", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu8, sizeof(mnu8) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MIS", OAPI_KEY_G, &ApolloRTCCMFD::set_getbase);
	RegisterFunction("TYP", OAPI_KEY_V, &ApolloRTCCMFD::menuChangeVesselType);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("SXT", OAPI_KEY_S, &ApolloRTCCMFD::menusextantstartime);
	RegisterFunction("UPL", OAPI_KEY_A, &ApolloRTCCMFD::menuSwitchUplinkInhibit);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("MJD", OAPI_KEY_M, &ApolloRTCCMFD::menuSetLaunchMJD);
	RegisterFunction("EPO", OAPI_KEY_E, &ApolloRTCCMFD::menuSetAGCEpoch);
	RegisterFunction("TIM", OAPI_KEY_U, &ApolloRTCCMFD::menuUpdateLiftoffTime);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu9[] =
	{
		{ "Maneuver Vehicle", 0, 'V' },
		{ "Maneuver Engine", 0, 'E' },
		{ "Heads-up/down", 0, 'H' },
		{ "", 0, ' ' },
		{ "Manual TIG", 0, 'T' },
		{ "Manual DV", 0, 'D' },

		{ "Calculate PAD", 0, 'C' },
		{ "Switch Maneuver option", 0, 'O' },
		{ "", 0, ' ' },
		{ "Request burn data", 0, 'R' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu9, sizeof(mnu9) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VEH", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("ENG", OAPI_KEY_E, &ApolloRTCCMFD::menuSwitchManPADEngine);
	RegisterFunction("HEA", OAPI_KEY_H, &ApolloRTCCMFD::menuSwitchHeadsUp);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::menuManPADTIG);
	RegisterFunction("DV", OAPI_KEY_D, &ApolloRTCCMFD::menuManPADDV);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalcManPAD);
	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::menuSwitchManPADopt);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("REQ", OAPI_KEY_R, &ApolloRTCCMFD::menuRequestLTMFD);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu10[] =
	{
		{ "", 0, ' ' },
		{ "With/Without Midcourse", 0, 'D' },
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

	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MAN", OAPI_KEY_D, &ApolloRTCCMFD::menuSwitchEntryPADDirect);
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
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calc Map Update", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu11, sizeof(mnu11) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalcMapUpdate);
	RegisterFunction("MOD", OAPI_KEY_V, &ApolloRTCCMFD::menuSwitchMapUpdate);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu12[] =
	{
		{ "Maneuver type", 0, 'T' },
		{ "", 0, ' ' },
		{ "Time of landing", 0, 'P' },
		{ "Apocynthion", 0, 'H' },
		{ "Pericynthion", 0, 'N' },
		{ "Approach azimuth", 0, 'I' },

		{ "Calculate maneuver", 0, 'C' },
		{ "Selen. latitude", 0, 'A' },
		{ "Selen. longitude", 0, 'O' },
		{ "Selen. altitude", 0, 'L' },
		{ "Upload to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu12, sizeof(mnu12) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAN", OAPI_KEY_T, &ApolloRTCCMFD::menuSwitchLOIManeuver);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("GET", OAPI_KEY_P, &ApolloRTCCMFD::menuSetTLAND);
	RegisterFunction("APA", OAPI_KEY_H, &ApolloRTCCMFD::menuSetLOIApo);
	RegisterFunction("PEA", OAPI_KEY_N, &ApolloRTCCMFD::menuSetLOIPeri);
	RegisterFunction("AZI", OAPI_KEY_I, &ApolloRTCCMFD::menuSetLOIAzi);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLOICalc);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuLSLat);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::menuLSLng);
	RegisterFunction("ALT", OAPI_KEY_L, &ApolloRTCCMFD::menuLSAlt);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuP30Upload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu13[] =
	{
		{ "T1 guess", 0, 'T' },
		{ "Landmark Latitude", 0, 'A' },
		{ "Landmark Longitude", 0, 'O' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate PAD", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu13, sizeof(mnu13) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIM", OAPI_KEY_T, &ApolloRTCCMFD::menuSetLmkTime);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuSetLmkLat);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::menuSetLmkLng);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLmkPADCalc);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_J, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu14[] =
	{
		{ "Lambert Targeting", 0, 'L' },
		{ "CDH/NSR maneuver", 0, 'C' },
		{ "Orbit Adjustment", 0, 'O' },
		{ "Translunar", 0, 'M' },
		{ "Lunar Orbit", 0, 'S' },

		{ "Entry", 0, 'E' },
		{ "DOI Targeting", 0, 'D' },
		{ "Plane Change", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to main menu", 0, 'B' },
	};

	RegisterPage(mnu14, sizeof(mnu14) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LAM", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLambertPage);
	RegisterFunction("CDH", OAPI_KEY_C, &ApolloRTCCMFD::menuSetCDHPage);
	RegisterFunction("ORB", OAPI_KEY_O, &ApolloRTCCMFD::menuSetOrbAdjPage);
	RegisterFunction("MCC", OAPI_KEY_M, &ApolloRTCCMFD::menuTranslunarPage);
	RegisterFunction("LOI", OAPI_KEY_S, &ApolloRTCCMFD::menuSetLOIPage);
	RegisterFunction("ENT", OAPI_KEY_E, &ApolloRTCCMFD::menuSetEntryPage);

	RegisterFunction("DOI", OAPI_KEY_D, &ApolloRTCCMFD::menuSetDOIPage);
	RegisterFunction("PC", OAPI_KEY_P, &ApolloRTCCMFD::menuSetPCPage);
	RegisterFunction("LIF", OAPI_KEY_A, &ApolloRTCCMFD::menuSetLunarLiftoffPage);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu15[] =
	{
		{ "Pointing body", 0, 'P' },
		{ "Vessel axis", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate attitude", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu15, sizeof(mnu15) / sizeof(MFDBUTTONMENU));

	RegisterFunction("BOD", OAPI_KEY_P, &ApolloRTCCMFD::vecbodydialogue);
	RegisterFunction("DIR", OAPI_KEY_D, &ApolloRTCCMFD::cycleVECDirOpt);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuVECPOINTCalc);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_J, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu16[] =
	{
		{ "", 0, ' ' },
		{ "Maneuver GET", 0, 'G' },
		{ "Number of revolutions", 0, 'N' },
		{ "Selen. latitude", 0, 'A' },
		{ "Selen. longitude", 0, 'O' },
		{ "Selen. altitude", 0, 'L' },

		{ "Calculate maneuver", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Upload to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu16, sizeof(mnu16) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TIM", OAPI_KEY_G, &ApolloRTCCMFD::menuSetDOIGET);
	RegisterFunction("N", OAPI_KEY_N, &ApolloRTCCMFD::menuSetDOIRevs);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuLSLat);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::menuLSLng);
	RegisterFunction("ALT", OAPI_KEY_L, &ApolloRTCCMFD::menuLSAlt);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuDOICalc);
	RegisterFunction("TLA", OAPI_KEY_B, &ApolloRTCCMFD::menuTLANDUpload);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuP30Upload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu17[] =
	{
		{ "Maneuver Option", 0, 'M' },
		{ "Ground Elapsed Time", 0, 'G' },
		{ "Revs between NC1 and NC2", 0, 'N' },
		{ "DH at NCC", 0, 'H' },
		{ "DH at NSR", 0, 'I' },
		{ "Elevation angle at TPI", 0, 'E' },

		{ "Target Vessel", 0, 'V' },
		{ "Calculate maneuver", 0, 'C' },
		{ "Cycle PC Option for NCC1/2", 0, 'P' },
		{ "NC1 or NC2 Time for NPC", 0, 'T' },
		{ "Upload to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu17, sizeof(mnu17) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAN", OAPI_KEY_M, &ApolloRTCCMFD::menuSwitchSkylabManeuver);
	RegisterFunction("TIG", OAPI_KEY_G, &ApolloRTCCMFD::menuSetSkylabGET);
	RegisterFunction("NC", OAPI_KEY_N, &ApolloRTCCMFD::menuSetSkylabNC);
	RegisterFunction("DH1", OAPI_KEY_H, &ApolloRTCCMFD::menuSetSkylabDH1);
	RegisterFunction("DH2", OAPI_KEY_I, &ApolloRTCCMFD::menuSetSkylabDH2);
	RegisterFunction("EL", OAPI_KEY_E, &ApolloRTCCMFD::menuSetSkylabEL);

	RegisterFunction("TGT", OAPI_KEY_V, &ApolloRTCCMFD::set_target);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuSkylabCalc);
	RegisterFunction("NPC", OAPI_KEY_P, &ApolloRTCCMFD::menuCyclePlaneChange);
	RegisterFunction("PCM", OAPI_KEY_T, &ApolloRTCCMFD::menuCyclePCManeuver);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuP30Upload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu18[] =
	{
		{ "Maneuver GET", 0, 'G' },
		{ "Time of alignment", 0, 'D' },
		{ "Selen. latitude", 0, 'A' },
		{ "Selen. longitude", 0, 'O' },
		{ "Selen. altitude", 0, 'L' },
		{ "", 0, ' ' },

		{ "Target Vessel", 0, 'V' },
		{ "Calculate maneuver", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Upload to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu18, sizeof(mnu18) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIM", OAPI_KEY_G, &ApolloRTCCMFD::menuSetPCTIGguess);
	RegisterFunction("ALI", OAPI_KEY_D, &ApolloRTCCMFD::menuSetPCAlignGET);
	RegisterFunction("LAN", OAPI_KEY_N, &ApolloRTCCMFD::menuSetPCLanded);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuLSLat);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::menuLSLng);
	RegisterFunction("ALT", OAPI_KEY_L, &ApolloRTCCMFD::menuLSAlt);

	RegisterFunction("TGT", OAPI_KEY_V, &ApolloRTCCMFD::set_target);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuPCCalc);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuP30Upload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


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
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu20, sizeof(mnu20) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAN", OAPI_KEY_M, &ApolloRTCCMFD::menuSetManPADPage);
	RegisterFunction("ENP", OAPI_KEY_E, &ApolloRTCCMFD::menuSetEntryPADPage);
	RegisterFunction("LMK", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLandmarkTrkPage);
	RegisterFunction("MAP", OAPI_KEY_U, &ApolloRTCCMFD::menuSetMapUpdatePage);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu21[] =
	{
		{ "State Vector", 0, 'V' },
		{ "REFSMMAT", 0, 'R' },
		{ "Vector Pointing", 0, 'P' },
		{ "Erasable Memory Programs", 0, 'E' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "Skylab Rendezvous", 0, 'S' },
		{ "Terrain Model Generation", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to main menu", 0, 'B' }
	};

	RegisterPage(mnu21, sizeof(mnu21) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SV", OAPI_KEY_V, &ApolloRTCCMFD::menuSetSVPage);
	RegisterFunction("REF", OAPI_KEY_R, &ApolloRTCCMFD::menuSetREFSMMATPage);
	RegisterFunction("VEC", OAPI_KEY_P, &ApolloRTCCMFD::menuSetVECPOINTPage);
	RegisterFunction("EMP", OAPI_KEY_E, &ApolloRTCCMFD::menuSetEMPPage);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("SKY", OAPI_KEY_S, &ApolloRTCCMFD::menuSetSkylabPage);
	RegisterFunction("TER", OAPI_KEY_T, &ApolloRTCCMFD::menuSetTerrainModelPage);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu22[] =
	{
		{ "Maneuver type", 0, 'T' },
		{ "Maneuver GET", 0, 'G' },
		{ "Pericynthion GET", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate maneuver", 0, 'C' },
		{ "Selen. latitude", 0, 'A' },
		{ "Selen. longitude", 0, 'O' },
		{ "Selen. altitude", 0, 'L' },
		{ "Upload to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' }
	};

	RegisterPage(mnu22, sizeof(mnu22) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAN", OAPI_KEY_T, &ApolloRTCCMFD::menuSwitchTLCCManeuver);
	RegisterFunction("TIG", OAPI_KEY_G, &ApolloRTCCMFD::menuSetTLCCGET);
	RegisterFunction("GET", OAPI_KEY_P, &ApolloRTCCMFD::menuSetTLCCPeriGET);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTLCCCalc);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuSetTLCCLat);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::menuSetTLCCLng);
	RegisterFunction("ALT", OAPI_KEY_L, &ApolloRTCCMFD::menuSetTLCCAlt);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuP30Upload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu23[] =
	{
		{ "Guess for TPI time", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Liftoff Time Option", 0, 'O' },
		{ "Target Vessel", 0, 'V' },
		{ "Calculate liftoff times", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' }
	};

	RegisterPage(mnu23, sizeof(mnu23) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TPI", OAPI_KEY_T, &ApolloRTCCMFD::menuSetTPIguess);
	RegisterFunction("", OAPI_KEY_J, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::menuLunarLiftoffTimeOption);
	RegisterFunction("TGT", OAPI_KEY_V, &ApolloRTCCMFD::set_target);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLunarLiftoffCalc);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu24[] =
	{
		{ "P99", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Uplink Load Number", 0, 'O' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink EMP", 0, 'U' },
		{ "Back to menu", 0, 'B' }
	};

	RegisterPage(mnu24, sizeof(mnu24) / sizeof(MFDBUTTONMENU));

	RegisterFunction("P99", OAPI_KEY_T, &ApolloRTCCMFD::menuSetEMPUplinkP99);
	RegisterFunction("", OAPI_KEY_J, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("NUM", OAPI_KEY_O, &ApolloRTCCMFD::menuSetEMPUplinkNumber);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuEMPUplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);
}

bool ApolloRTCCMFDButtons::SearchForKeysInOtherPages() const
{
	return false;
}