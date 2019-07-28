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
		{ "MCC Displays", 0, 'M' },
		{ "Mission Planning", 0, 'B' },
		{ "Configuration", 0, 'S' },

		{ "Uplinks", 0, 'U' },
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
	RegisterFunction("MCC", OAPI_KEY_M, &ApolloRTCCMFD::menuSetMCCDisplaysPage);
	RegisterFunction("PLN", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMPTPage);
	RegisterFunction("CFG", OAPI_KEY_C, &ApolloRTCCMFD::menuSetConfigPage);

	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuSetUplinkMenu);
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

	RegisterFunction("OPT", OAPI_KEY_T, &ApolloRTCCMFD::menuCycleTwoImpulseOption);
	RegisterFunction("T1", OAPI_KEY_M, &ApolloRTCCMFD::t1dialogue);
	RegisterFunction("T2", OAPI_KEY_A, &ApolloRTCCMFD::t2dialogue);
	RegisterFunction("N", OAPI_KEY_N, &ApolloRTCCMFD::revdialogue);
	RegisterFunction("AXI", OAPI_KEY_U, &ApolloRTCCMFD::set_lambertaxis);
	RegisterFunction("SPH", OAPI_KEY_G, &ApolloRTCCMFD::set_spherical);

	RegisterFunction("TGT", OAPI_KEY_V, &ApolloRTCCMFD::set_target);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::lambertcalc);
	RegisterFunction("OFF", OAPI_KEY_O, &ApolloRTCCMFD::offvecdialogue);
	RegisterFunction("PHA", OAPI_KEY_P, &ApolloRTCCMFD::phasedialogue);
	RegisterFunction("UPL", OAPI_KEY_L, &ApolloRTCCMFD::menuP30Upload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRendezvousPage);

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
		{ "CSI or CDH", 0, 'M' },
		{ "Time Mode", 0, 'T' },
		{ "Maneuver Time", 0, 'M' },
		{ "Coelliptic DH", 0, 'D' },
		{ "Elevation angle at TPI", 0, 'E' },
		{ "", 0, ' ' },

		{ "Target Vessel", 0, 'V' },
		{ "Calculate burn", 0, 'C' },
		{ "", 0, 'D' },
		{ "", 0, ' ' },
		{ "Upload to AGC", 0, 'L' },
		{ "Previous page", 0, 'B' },
	};

	RegisterPage(mnu3, sizeof(mnu3) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MOD", OAPI_KEY_M, &ApolloRTCCMFD::menuCycleSPQMode);
	RegisterFunction("TIM", OAPI_KEY_T, &ApolloRTCCMFD::set_CDHtimemode);
	RegisterFunction("TIG", OAPI_KEY_N, &ApolloRTCCMFD::SPQtimedialogue);
	RegisterFunction("DH", OAPI_KEY_D, &ApolloRTCCMFD::DHdialogue);
	RegisterFunction("E", OAPI_KEY_E, &ApolloRTCCMFD::menuSetDKIElevation);
	RegisterFunction("TPI", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("TGT", OAPI_KEY_V, &ApolloRTCCMFD::set_target);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::CDHcalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_L, &ApolloRTCCMFD::menuP30Upload);
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
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Upload to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu4, sizeof(mnu4) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SET", OAPI_KEY_S, &ApolloRTCCMFD::menuSetGMPInput);
	RegisterFunction("<<", OAPI_KEY_P, &ApolloRTCCMFD::menuCycleGMPMarkerDown);
	RegisterFunction(">>", OAPI_KEY_N, &ApolloRTCCMFD::menuCycleGMPMarkerUp);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::GPMPCalc);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuP30Upload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu5[] =
	{
		{ "Time of Alignment", 0, 'G' },
		{ "Uplink option", 0, 'K' },
		{ "Heads up/down for P30", 0, 'H' },
		{ "Send REFSMMAT to MFD of target", 0, 'S' },
		{ "REFSMMAT from AGC", 0, 'D' },
		{ "", 0, ' ' },

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
	RegisterFunction("SEN", OAPI_KEY_S, &ApolloRTCCMFD::menuSendREFSMMATToOtherVessel);
	RegisterFunction("DWN", OAPI_KEY_D, &ApolloRTCCMFD::GetREFSMMATfromAGC);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::CycleREFSMMATopt);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::calcREFSMMAT);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::UploadREFSMMAT);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuLSLat);
	RegisterFunction("LNG", OAPI_KEY_L, &ApolloRTCCMFD::menuLSLng);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);

	static const MFDBUTTONMENU mnu6[] =
	{
		{ "Deorbit Maneuver", 0, 'D' },
		{ "Return to Earth (Earth-centered)", 0, 'E' },
		{ "Return to Earth (Moon-centered)", 0, 'M' },
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

	RegisterFunction("DEO", OAPI_KEY_D, &ApolloRTCCMFD::menuSetDeorbitPage);
	RegisterFunction("RTE", OAPI_KEY_E, &ApolloRTCCMFD::menuSetEarthEntryPage);
	RegisterFunction("RTM", OAPI_KEY_M, &ApolloRTCCMFD::menuSetMoonEntryPage);
	RegisterFunction("SPL", OAPI_KEY_S, &ApolloRTCCMFD::menuSetEntryUpdatePage);
	RegisterFunction("CON", OAPI_KEY_C, &ApolloRTCCMFD::menuSetRTEConstraintsPage);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu7[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Set Target", 0, 'T' },
		{ "State Vector Slot", 0, 'D' },
		{ "AGS K Factor", 0, 'A' },

		{ "Calculate State Vector", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu7, sizeof(mnu7) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_svtarget);
	RegisterFunction("SLT", OAPI_KEY_D, &ApolloRTCCMFD::menuSwitchSVSlot);
	RegisterFunction("AGS", OAPI_KEY_A, &ApolloRTCCMFD::menuSetAGSKFactor);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuAGSSVCalc);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);

	static const MFDBUTTONMENU mnu8[] =
	{
		{ "Mission (GET base)", 0, 'G' },
		{ "Vessel type", 0, 'V' },
		{ "LM stage", 0, 'T' },
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
	RegisterFunction("STA", OAPI_KEY_T, &ApolloRTCCMFD::menuCycleLMStage);
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
		{ "RCS burn direction", 0, 'A' },
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
	RegisterFunction("DIR", OAPI_KEY_A, &ApolloRTCCMFD::menuManDirection);
	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::menuManPADTIG);
	RegisterFunction("DV", OAPI_KEY_D, &ApolloRTCCMFD::menuManPADDV);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalcManPAD);
	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::menuSwitchManPADopt);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("REQ", OAPI_KEY_R, &ApolloRTCCMFD::menuRequestLTMFD);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu10[] =
	{
		{ "", 0, ' ' },
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

	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
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

	RegisterFunction("GET", OAPI_KEY_G, &ApolloRTCCMFD::menuSetMapUpdateGET);
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
		{ "LOI option", 0, 'D' },
		{ "Time of landing", 0, 'P' },
		{ "Apocynthion", 0, 'H' },
		{ "Pericynthion", 0, 'N' },
		{ "Approach azimuth", 0, 'I' },

		{ "Calculate maneuver", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Dir. of ellipse rotation", 0, 'E' },
		{ "Upload to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu12, sizeof(mnu12) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAN", OAPI_KEY_T, &ApolloRTCCMFD::menuSwitchLOIManeuver);
	RegisterFunction("OPT", OAPI_KEY_D, &ApolloRTCCMFD::menuSwitchLOIOption);
	RegisterFunction("GET", OAPI_KEY_P, &ApolloRTCCMFD::menuSetLOIGET);
	RegisterFunction("APA", OAPI_KEY_H, &ApolloRTCCMFD::menuSetLOIApo);
	RegisterFunction("PEA", OAPI_KEY_N, &ApolloRTCCMFD::menuSetLOIPeri);
	RegisterFunction("AZI", OAPI_KEY_I, &ApolloRTCCMFD::menuSetLOIAzi);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLOICalc);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("ELI", OAPI_KEY_E, &ApolloRTCCMFD::menuCycleLOIEllipseOption);
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
		{ "Rendezvous Targeting", 0, 'R' },
		{ "Orbit Adjustment", 0, 'O' },
		{ "Translunar", 0, 'M' },
		{ "Lunar Orbit", 0, 'S' },
		{ "Entry", 0, 'E' },
		{ "", 0, ' ' },

		{ "DOI Targeting", 0, 'D' },
		{ "Plane Change", 0, 'P' },
		{ "Lunar Liftoff", 0, 'L' },
		{ "Lunar Ascent", 0, 'A' },
		{ "Descent Abort", 0, 'G' },
		{ "Back to main menu", 0, 'B' },
	};

	RegisterPage(mnu14, sizeof(mnu14) / sizeof(MFDBUTTONMENU));

	RegisterFunction("REN", OAPI_KEY_R, &ApolloRTCCMFD::menuSetRendezvousPage);
	RegisterFunction("ORB", OAPI_KEY_O, &ApolloRTCCMFD::menuSetOrbAdjPage);
	RegisterFunction("MCC", OAPI_KEY_M, &ApolloRTCCMFD::menuTranslunarPage);
	RegisterFunction("LOI", OAPI_KEY_S, &ApolloRTCCMFD::menuSetLOIPage);
	RegisterFunction("ENT", OAPI_KEY_E, &ApolloRTCCMFD::menuSetEntryPage);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("DOI", OAPI_KEY_D, &ApolloRTCCMFD::menuSetDOIPage);
	RegisterFunction("PC", OAPI_KEY_P, &ApolloRTCCMFD::menuSetPCPage);
	RegisterFunction("LIF", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLunarLiftoffPage);
	RegisterFunction("ASC", OAPI_KEY_A, &ApolloRTCCMFD::menuSetLunarAscentPage);
	RegisterFunction("ABO", OAPI_KEY_G, &ApolloRTCCMFD::menuSetPDAPPage);
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

	RegisterFunction("OPT", OAPI_KEY_G, &ApolloRTCCMFD::cycleVECPOINTOpt);
	RegisterFunction("BOD", OAPI_KEY_P, &ApolloRTCCMFD::vecbodydialogue);
	RegisterFunction("DIR", OAPI_KEY_D, &ApolloRTCCMFD::cycleVECDirOpt);
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
		{ "Maneuver GET", 0, 'G' },
		{ "Number of revolutions", 0, 'N' },
		{ "Perilune location", 0, ' ' },
		{ "Selen. latitude", 0, 'A' },
		{ "Selen. longitude", 0, 'O' },
		{ "Selen. altitude", 0, 'L' },

		{ "Calculate maneuver", 0, 'C' },
		{ "DOI Option", 0, 'M' },
		{ "Uplink TLAND", 0, 'T' },
		{ "", 0, ' ' },
		{ "Upload to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu16, sizeof(mnu16) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIM", OAPI_KEY_G, &ApolloRTCCMFD::menuSetDOIGET);
	RegisterFunction("N", OAPI_KEY_N, &ApolloRTCCMFD::menuSetDOIRevs);
	RegisterFunction("PER", OAPI_KEY_D, &ApolloRTCCMFD::menuSetDOIPeriAng);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuLSLat);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::menuLSLng);
	RegisterFunction("ALT", OAPI_KEY_L, &ApolloRTCCMFD::menuLSAlt);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuDOICalc);
	RegisterFunction("MOD", OAPI_KEY_M, &ApolloRTCCMFD::menuDOIOption);
	RegisterFunction("TLA", OAPI_KEY_T, &ApolloRTCCMFD::menuTLANDUpload);
	RegisterFunction("PEA", OAPI_KEY_I, &ApolloRTCCMFD::menuSetDOIPeriAlt);
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
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRendezvousPage);


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
		{ "Nav Check PAD", 0, 'N' },
		{ "P37 PAD", 0, 'P' },

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
	RegisterFunction("P37", OAPI_KEY_P, &ApolloRTCCMFD::menuSetP37PADPage);

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
		{ "Vector Pointing", 0, 'P' },
		{ "Erasable Memory Programs", 0, 'E' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "LVDC options", 0, 'L' },
		{ "Terrain Model Generation", 0, 'T' },
		{ "AGC Ephemeris Generator", 0, 'A' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to main menu", 0, 'B' }
	};

	RegisterPage(mnu21, sizeof(mnu21) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LS", OAPI_KEY_V, &ApolloRTCCMFD::menuSetLSUpdateMenu);
	RegisterFunction("REF", OAPI_KEY_R, &ApolloRTCCMFD::menuSetREFSMMATPage);
	RegisterFunction("VEC", OAPI_KEY_P, &ApolloRTCCMFD::menuSetVECPOINTPage);
	RegisterFunction("EMP", OAPI_KEY_E, &ApolloRTCCMFD::menuSetEMPPage);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("IU", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLVDCPage);
	RegisterFunction("TER", OAPI_KEY_T, &ApolloRTCCMFD::menuSetTerrainModelPage);
	RegisterFunction("EPH", OAPI_KEY_A, &ApolloRTCCMFD::menuSetAGCEphemerisPage);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu22[] =
	{
		{ "Maneuver type", 0, 'T' },
		{ "Maneuver GET", 0, 'G' },
		{ "Pericynthion GET", 0, 'G' },
		{ "FR Inclination", 0, 'I' },
		{ "Ascending or Descending Node", 0, 'N' },
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
	RegisterFunction("INC", OAPI_KEY_I, &ApolloRTCCMFD::menuSetTLCCDesiredInclination);
	RegisterFunction("ASC", OAPI_KEY_N, &ApolloRTCCMFD::menuSwitchTLCCAscendingNode);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTLCCCalc);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuSetTLCCLat);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::menuSetTLCCLng);
	RegisterFunction("ALT", OAPI_KEY_L, &ApolloRTCCMFD::menuSetTLCCAlt);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuP30Upload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu23[] =
	{
		{ "Earliest liftoff time", 0, 'T' },
		{ "Input or calc. insertion", 0, 'A' },
		{ "Horizontal velocity", 0, 'H' },
		{ "Vertical velocity", 0, 'V' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Liftoff Time Option", 0, 'O' },
		{ "Target Vessel", 0, 'V' },
		{ "Calculate liftoff times", 0, 'C' },
		{ "Time between Ins and TPI", 0, 'D' },
		{ "Input for VHOR/VERT", 0, 'V' },
		{ "Back to menu", 0, 'B' }
	};

	RegisterPage(mnu23, sizeof(mnu23) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TLO", OAPI_KEY_T, &ApolloRTCCMFD::menuSetLiftoffguess);
	RegisterFunction("VOP", OAPI_KEY_A, &ApolloRTCCMFD::menuCycleLunarLiftoffInsVelOption);
	RegisterFunction("VLH", OAPI_KEY_H, &ApolloRTCCMFD::menuLunarLiftoffVHorInput);
	RegisterFunction("VLV", OAPI_KEY_V, &ApolloRTCCMFD::menuLunarLiftoffVVertInput);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::menuLunarLiftoffTimeOption);
	RegisterFunction("TGT", OAPI_KEY_V, &ApolloRTCCMFD::set_target);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLunarLiftoffCalc);
	RegisterFunction("DT", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLiftoffDT);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
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


	static const MFDBUTTONMENU mnu25[] =
	{
		{ "GET of Nav Check", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate PAD", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' }
	};

	RegisterPage(mnu25, sizeof(mnu25) / sizeof(MFDBUTTONMENU));

	RegisterFunction("GET", OAPI_KEY_G, &ApolloRTCCMFD::menuSetNavCheckGET);
	RegisterFunction("", OAPI_KEY_J, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuNavCheckPADCalc);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu26[] =
	{
		{ "Est. Time of Ignition", 0, 'T' },
		{ "Manual longitude or zone", 0, 'L' },
		{ "Des. Landing Long", 0, 'O' },
		{ "Entry Angle", 0, 'A' },
		{ "Maneuver Type", 0, 'D' },
		{ "", 0, ' ' },

		{ "Calculate Entry", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu26, sizeof(mnu26) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::EntryTimeDialogue);
	RegisterFunction("LMO", OAPI_KEY_L, &ApolloRTCCMFD::EntryLongitudeModeDialogue);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::EntryLngDialogue);
	RegisterFunction("ANG", OAPI_KEY_A, &ApolloRTCCMFD::EntryAngDialogue);
	RegisterFunction("OPT", OAPI_KEY_D, &ApolloRTCCMFD::menuSwitchEntryNominal);
	RegisterFunction("ENG", OAPI_KEY_E, &ApolloRTCCMFD::menuSwitchDeorbitEngineOption);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuDeorbitCalc);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuEntryUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetEntryPage);


	static const MFDBUTTONMENU mnu27[] =
	{
		{ "Est. Time of Ignition", 0, 'T' },
		{ "Manual longitude or zone", 0, 'L' },
		{ "Des. Landing Long", 0, 'O' },
		{ "Entry Angle", 0, 'A' },
		{ "", 0, ' ' },
		{ "Maneuver Type", 0, 'E' },

		{ "Calculate Entry", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu27, sizeof(mnu27) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::EntryTimeDialogue);
	RegisterFunction("LMO", OAPI_KEY_L, &ApolloRTCCMFD::EntryLongitudeModeDialogue);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::EntryLngDialogue);
	RegisterFunction("ANG", OAPI_KEY_A, &ApolloRTCCMFD::EntryAngDialogue);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TYP", OAPI_KEY_E, &ApolloRTCCMFD::menuSwitchCritical);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuEntryCalc);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuEntryUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetEntryPage);


	static const MFDBUTTONMENU mnu28[] =
	{
		{ "Return-to-earth mode", 0, 'N' },
		{ "Est. Time of Ignition", 0, 'T' },
		{ "Manual longitude or zone", 0, 'L' },
		{ "Des. Landing Long", 0, 'O' },
		{ "Est. time of reentry", 0, 'S' },
		{ "FR Inclination", 0, 'I' },

		{ "Calculate Entry", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu28, sizeof(mnu28) / sizeof(MFDBUTTONMENU));

	RegisterFunction("OPT", OAPI_KEY_N, &ApolloRTCCMFD::CycleRTECalcMode);
	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::EntryTimeDialogue);
	RegisterFunction("LMO", OAPI_KEY_L, &ApolloRTCCMFD::EntryLongitudeModeDialogue);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::EntryLngDialogue);
	RegisterFunction("EIT", OAPI_KEY_S, &ApolloRTCCMFD::menuSetRTEReentryTime);
	RegisterFunction("INC", OAPI_KEY_I, &ApolloRTCCMFD::menuSetEntryDesiredInclination);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuMoonRTECalc);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuEntryUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetEntryPage);


	static const MFDBUTTONMENU mnu29[] =
	{
		{ "Max inclination", 0, 'V' },
		{ "Override entry range", 0, 'T' },
		{ "Max reentry speed", 0, 'L' },
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

	RegisterFunction("INM", OAPI_KEY_V, &ApolloRTCCMFD::menuSetEntryMaxInclination);
	RegisterFunction("RAN", OAPI_KEY_T, &ApolloRTCCMFD::menuSetEntryRangeOverride);
	RegisterFunction("SPE", OAPI_KEY_L, &ApolloRTCCMFD::menuSetMaxReentrySpeed);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetEntryPage);


	static const MFDBUTTONMENU mnu30[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Update", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Entry Range", 0, 'R' },
		{ "Uplink to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu30, sizeof(mnu30) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuEntryUpdateCalc);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("RAN", OAPI_KEY_R, &ApolloRTCCMFD::EntryRangeDialogue);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuEntryUpdateUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetEntryPage);


	static const MFDBUTTONMENU mnu31[] =
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

	RegisterPage(mnu31, sizeof(mnu31) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu32[] =
	{
		{ "Lambert targeting", 0, 'L' },
		{ "CDH/NSR maneuver", 0, 'C' },
		{ "Docking initate", 0, 'D' },
		{ "Skylab Rendezvous", 0, 'S' },
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

	RegisterFunction("LAM", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLambertPage);
	RegisterFunction("CDH", OAPI_KEY_C, &ApolloRTCCMFD::menuSetCDHPage);
	RegisterFunction("DKI", OAPI_KEY_D, &ApolloRTCCMFD::menuSetDKIPage);
	RegisterFunction("SKY", OAPI_KEY_S, &ApolloRTCCMFD::menuSetSkylabPage);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu33[] =
	{
		{ "Rendezvous profile", 0, 'P' },
		{ "Time of phasing maneuver", 0, 'T' },
		{ "Time of TPI maneuver", 0, 'L' },
		{ "Delta Height", 0, 'D' },
		{ "Elevation angle at TPI", 0, 'E' },
	{ "", 0, ' ' },

		{ "Target name", 0, 'V' },
		{ "Calculate maneuver", 0, 'C' },
		{ "DKI options page", 0, 'Q' },
		{ "", 0, ' ' },
		{ "Upload to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu33, sizeof(mnu33) / sizeof(MFDBUTTONMENU));

	RegisterFunction("PRO", OAPI_KEY_P, &ApolloRTCCMFD::menuCycleDKIProfile);
	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::DKITIGDialogue);
	RegisterFunction("TPI", OAPI_KEY_L, &ApolloRTCCMFD::menuSetTPIguess);
	RegisterFunction("DH", OAPI_KEY_D, &ApolloRTCCMFD::DHdialogue);
	RegisterFunction("E", OAPI_KEY_E, &ApolloRTCCMFD::menuSetDKIElevation);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("TGT", OAPI_KEY_V, &ApolloRTCCMFD::set_target);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuDKICalc);
	RegisterFunction("OPT", OAPI_KEY_Q, &ApolloRTCCMFD::menuSetDKIOptionsPage);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuP30Upload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRendezvousPage);


	static const MFDBUTTONMENU mnu34[] =
	{
		{ "Maneuver line or times", 0, 'L' },
		{ "Fixed radial component", 0, 'R' },
		{ "TPI mode", 0, 'O' },
		{ "TPI time before sunrise", 0, 'S' },
		{ "Half-revs between CSI and CDH", 0, 'N' },
		{ "Half-revs between Phasing and Boost", 0, 'M' },

		{ "Time between abort and Boost/CSI", 0, 'E' },
		{ "Time between Boost and HAM", 0, 'V' },
		{ "Time between HAM and CSI", 0, 'Q' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to DKI page", 0, 'B' },
	};

	RegisterPage(mnu34, sizeof(mnu34) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAN", OAPI_KEY_L, &ApolloRTCCMFD::menuCycleDKIManeuverLine);
	RegisterFunction("RAD", OAPI_KEY_R, &ApolloRTCCMFD::menuCycleDKIRadialComponent);
	RegisterFunction("TPI", OAPI_KEY_O, &ApolloRTCCMFD::menuCycleDKITPIMode);
	RegisterFunction("SUN", OAPI_KEY_S, &ApolloRTCCMFD::DKITPIDTDialogue);
	RegisterFunction("NHC", OAPI_KEY_N, &ApolloRTCCMFD::DKINHCDialogue);
	RegisterFunction("NPB", OAPI_KEY_M, &ApolloRTCCMFD::DKINPBDialogue);

	RegisterFunction("DT1", OAPI_KEY_E, &ApolloRTCCMFD::menuDKIDeltaT1);
	RegisterFunction("DT2", OAPI_KEY_V, &ApolloRTCCMFD::menuDKIDeltaT2);
	RegisterFunction("DT3", OAPI_KEY_Q, &ApolloRTCCMFD::menuDKIDeltaT3);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetDKIPage);


	static const MFDBUTTONMENU mnu35[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate DAP PAD", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu35, sizeof(mnu35) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuDAPPADCalc);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
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
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu37[] =
	{
		{ "Epoch of BRCS", 0, 'E' },
		{ "MJD of July 1st", 0, 'Z' },
		{ "MJD of launch", 0, 'L' },
		{ "MJD of mid-mission", 0, 'M' },
		{ "Time of landing", 0, 'T' },
		{ "Apollo mission", 0, 'N' },

		{ "Cycle options", 0, 'O' },
		{ "Generate file", 0, 'C' },
		{ "CMC or LGC", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu37, sizeof(mnu37) / sizeof(MFDBUTTONMENU));

	RegisterFunction("EPO", OAPI_KEY_A, &ApolloRTCCMFD::menuSetAGCEphemBRCSEpoch);
	RegisterFunction("ZER", OAPI_KEY_L, &ApolloRTCCMFD::menuSetAGCEphemTEphemZero);
	RegisterFunction("LAU", OAPI_KEY_L, &ApolloRTCCMFD::menuSetAGCEphemTEPHEM);
	RegisterFunction("MID", OAPI_KEY_M, &ApolloRTCCMFD::menuSetAGCEphemTIMEM0);
	RegisterFunction("LAN", OAPI_KEY_T, &ApolloRTCCMFD::menuSetAGCEphemTLAND);
	RegisterFunction("MIS", OAPI_KEY_N, &ApolloRTCCMFD::menuSetAGCEphemMission);

	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::menuCycleAGCEphemOpt);
	RegisterFunction("GEN", OAPI_KEY_C, &ApolloRTCCMFD::menuGenerateAGCEphemeris);
	RegisterFunction("AGC", OAPI_KEY_U, &ApolloRTCCMFD::menuCycleAGCEphemAGCType);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu38[] =
	{
		{ "Liftoff time", 0, 'T' },
		{ "Horizontal velocity", 0, 'H' },
		{ "Vertical velocity", 0, 'V' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate lunar ascent", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu38, sizeof(mnu38) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LTO", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLAPLiftoffTime);
	RegisterFunction("HOR", OAPI_KEY_H, &ApolloRTCCMFD::menuSetLAPHorVelocity);
	RegisterFunction("VER", OAPI_KEY_V, &ApolloRTCCMFD::menuSetLAPVerVelocity);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLAPCalc);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu39[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Ascent PAD", 0, 'C' },
		{ "Set target", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu39, sizeof(mnu39) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuAscentPADCalc);
	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_target);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu40[] =
	{
		{ "Abort engine", 0, 'E' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Descent Abort", 0, 'C' },
		{ "Set target", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu40, sizeof(mnu40) / sizeof(MFDBUTTONMENU));

	RegisterFunction("ENG", OAPI_KEY_E, &ApolloRTCCMFD::menuCyclePDAPEngine);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuPDAPCalc);
	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_target);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuAP11AbortCoefUplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu41[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Update state vector", 0, 'U' },
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

	RegisterFunction("UPD", OAPI_KEY_U, &ApolloRTCCMFD::menuUpdateFIDOOrbitDigitals);
	RegisterFunction("GETL", OAPI_KEY_G, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETL);
	RegisterFunction("L", OAPI_KEY_L, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsL);
	RegisterFunction("GETR", OAPI_KEY_R, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETBV);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu42[] =
	{
		{ "FIDO Orbit Digitals", 0, 'O' },
		{ "Space Digitals", 0, 'S' },
		{ "Next Station Contacts", 0, 'N' },
		{ "Predicted Site Acquisition", 0, 'P' },
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

	RegisterFunction("ORB", OAPI_KEY_O, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsPage);
	RegisterFunction("SPA", OAPI_KEY_S, &ApolloRTCCMFD::menuSetSpaceDigitalsPage);
	RegisterFunction("NSC", OAPI_KEY_N, &ApolloRTCCMFD::menuSetNextStationContactsPage);
	RegisterFunction("PSA", OAPI_KEY_P, &ApolloRTCCMFD::menuSetPredSiteAcquisitionPage);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

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

	RegisterFunction("UPD", OAPI_KEY_U, &ApolloRTCCMFD::menuUpdateSpaceDigitals);
	RegisterFunction("GET", OAPI_KEY_G, &ApolloRTCCMFD::menuSetSpaceDigitalsGET);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu44[] =
	{
		{ "Planning mode active", 0, 'P' },
		{ "Delete maneuver", 0, 'D' },
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

	RegisterPage(mnu44, sizeof(mnu44) / sizeof(MFDBUTTONMENU));

	RegisterFunction("ACT", OAPI_KEY_P, &ApolloRTCCMFD::menuMPTCycleActive);
	RegisterFunction("DEL", OAPI_KEY_D, &ApolloRTCCMFD::menuMPTDeleteManeuver);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu45[] =
	{
		{ "All/lunar stations", 0, 'L' },
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

	RegisterPage(mnu45, sizeof(mnu45) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LUN", OAPI_KEY_L, &ApolloRTCCMFD::menuNextStationContactLunar);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu46[] =
	{
		{ "Ground Elapsed Time", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Interval for contacts", 0, 'D' },
		{ "Calculate contacts", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu46, sizeof(mnu46) / sizeof(MFDBUTTONMENU));

	RegisterFunction("GET", OAPI_KEY_G, &ApolloRTCCMFD::menuPredSiteAcqGET);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("DT", OAPI_KEY_D, &ApolloRTCCMFD::menuPredSiteAcqDT);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::PredSiteAcqCalc);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu47[] =
	{
		{ "AGC State Vectors", 0, 'G' },
		{ "Landing Site Vector", 0, 'L' },
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

	RegisterFunction("SV", OAPI_KEY_G, &ApolloRTCCMFD::menuSetStateVectorMenu);
	RegisterFunction("RLS", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLSUplinkPage);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu48[] =
	{
		{ "Set Time", 0, 'G' },
		{ "State Vector Slot", 0, 'D' },
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
	RegisterFunction("SLT", OAPI_KEY_D, &ApolloRTCCMFD::menuSwitchSVSlot);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_svtarget);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuSVCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_L, &ApolloRTCCMFD::menuSVUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu49[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Set Target", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate LS Coordinates", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu49, sizeof(mnu49) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_svtarget);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLSCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
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

	RegisterPage(mnu49, sizeof(mnu49) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLSUplinkCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuLSUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);
}

bool ApolloRTCCMFDButtons::SearchForKeysInOtherPages() const
{
	return false;
}