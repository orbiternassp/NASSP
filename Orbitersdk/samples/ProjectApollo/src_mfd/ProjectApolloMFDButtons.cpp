#include "ProjectApolloMFD.h"
#include "ProjectApolloMFDButtons.h"

ProjectApolloMFDButtons::ProjectApolloMFDButtons()
{
	static const MFDBUTTONMENU mnuNone[12] = {
		{ "Guidance, Navigation & Control", 0, 'G' },
		{ "Environmental Control System", 0, 'E' },
		{ "IMFD Support", 0, 'I' },
		{ "Telemetry",0,'T' },
		{ "LGC Initialization Data",0,'L' },
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ "Debug String",0,'D' }
	};

	page.None = RegisterPage(mnuNone, sizeof(mnuNone) / sizeof(MFDBUTTONMENU));

	RegisterFunction("GNC", OAPI_KEY_G, &ProjectApolloMFD::menuSetGNCPage);
	RegisterFunction("ECS", OAPI_KEY_E, &ProjectApolloMFD::menuSetECSPage);
	RegisterFunction("IU", OAPI_KEY_I, &ProjectApolloMFD::menuSetIUPage);
	RegisterFunction("TELE", OAPI_KEY_T, &ProjectApolloMFD::menuSetTELEPage);
	RegisterFunction("LGC", OAPI_KEY_L, &ProjectApolloMFD::menuSetLGCPage);
	RegisterFunction("FAIL", OAPI_KEY_A, &ProjectApolloMFD::menuSetFailuresPage);

	RegisterFunction("", OAPI_KEY_B, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ProjectApolloMFD::menuVoid);
	RegisterFunction("DBG", OAPI_KEY_D, &ProjectApolloMFD::menuSetDebugPage);


	static const MFDBUTTONMENU mnuGNC[4] = {
		{ "Back", 0, 'B' },
		{ "Kill rotation", 0, 'K' },
		{ "Save EMS scroll", 0, 'E' },
		{ "Virtual AGC core dump", 0, 'D' }
	};

	page.GNC = RegisterPage(mnuGNC, sizeof(mnuGNC) / sizeof(MFDBUTTONMENU));

	RegisterFunction("BCK", OAPI_KEY_B, &ProjectApolloMFD::menuSetMainPage);
	RegisterFunction("KILR", OAPI_KEY_K, &ProjectApolloMFD::menuKillRot);
	RegisterFunction("EMS", OAPI_KEY_E, &ProjectApolloMFD::menuSaveEMSScroll);
	RegisterFunction("DMP", OAPI_KEY_D, &ProjectApolloMFD::menuVAGCCoreDump);


	static const MFDBUTTONMENU mnuECS[8] = {
		{ "Back", 0, 'B' },
		{ "Crew number", 0, 'C' },
		{ "CDR in suit", 0, 'D' },
		{ "LMP in suit", 0, 'L' },
		{ "Start EVA",0,'E' },
		{ "Connect CSM O2 Hose to LM",0,'H' },
		{ "Primary coolant loop test heating", 0, 'P' },
		{ "Secondary coolant loop test heating", 0, 'S' }
	};

	page.ECS = RegisterPage(mnuECS, sizeof(mnuECS) / sizeof(MFDBUTTONMENU));

	RegisterFunction("BCK", OAPI_KEY_B, &ProjectApolloMFD::menuSetMainPage);
	RegisterFunction("CRW", OAPI_KEY_C, &ProjectApolloMFD::menuSetCrewNumber);
	RegisterFunction("CDR", OAPI_KEY_D, &ProjectApolloMFD::menuSetCDRInSuit);
	RegisterFunction("LMP", OAPI_KEY_L, &ProjectApolloMFD::menuSetLMPInSuit);
	RegisterFunction("EVA", OAPI_KEY_E, &ProjectApolloMFD::menuStartEVA);
	RegisterFunction("HOS", OAPI_KEY_A, &ProjectApolloMFD::menuConnectCSMO2Hose);

	RegisterFunction("PRM", OAPI_KEY_P, &ProjectApolloMFD::menuSetPrimECSTestHeaterPower);
	RegisterFunction("SEC", OAPI_KEY_S, &ProjectApolloMFD::menuSetSecECSTestHeaterPower);


	static const MFDBUTTONMENU mnuIU[12] = {
		{ "Back", 0, 'B' },
		{ "Input data 1", 0, 'A' },
		{ "Input data 2",0,'C' },
		{ "Input data 3",0,'D' },
		{ "Input data 4", 0, 'R' },
		{ "Change Source",0,'S' },

		{ "IU uplink type",0,'T' },
		{ "Uplink to IU",0,'U' },
		{ "", 0,' ' },
		{ "", 0,' ' },
		{ "", 0,' ' },
		{ "", 0, ' '}
	};

	page.IU = RegisterPage(mnuIU, sizeof(mnuIU) / sizeof(MFDBUTTONMENU));

	RegisterFunction("BCK", OAPI_KEY_B, &ProjectApolloMFD::menuSetMainPage);
	RegisterFunction("IN1", OAPI_KEY_A, &ProjectApolloMFD::menuSetIUUplinkInp1);
	RegisterFunction("IN2", OAPI_KEY_C, &ProjectApolloMFD::menuSetIUUplinkInp2);
	RegisterFunction("IN3", OAPI_KEY_D, &ProjectApolloMFD::menuSetIUUplinkInp3);
	RegisterFunction("IN4", OAPI_KEY_R, &ProjectApolloMFD::menuSetIUUplinkInp4);
	RegisterFunction("SRC", OAPI_KEY_S, &ProjectApolloMFD::menuSetIUSource);

	RegisterFunction("TYP", OAPI_KEY_T, &ProjectApolloMFD::menuCycleIUUplinkType);
	RegisterFunction("UPL", OAPI_KEY_U, &ProjectApolloMFD::menuIUUplink);
	RegisterFunction("", OAPI_KEY_I, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Y, &ProjectApolloMFD::menuVoid);

	static const MFDBUTTONMENU mnuTELE[11] = {
		{ "Back", 0, 'B' },
		{ "State Vector Update", 0, 'U' },
		{ 0,0,0 },
		{ 0,0,0 },
		{ "Change Source",0,'S' },
		{ "Change Reference Body", 0, 'R' },
		{ "Ascent Engine Arming",0,'A' },
		{ "Clock Update", 0, 'C' },
		{ "Sunburst Suborbital Abort",0,'F' },
		{ "Sunburst COI",0,'G' },
		{ "State Vector Slot", 0, 'T' }
	};

	page.TELE = RegisterPage(mnuTELE, sizeof(mnuTELE) / sizeof(MFDBUTTONMENU));

	RegisterFunction("BCK", OAPI_KEY_B, &ProjectApolloMFD::menuAbortUplink);
	RegisterFunction("SV", OAPI_KEY_U, &ProjectApolloMFD::menuStateVectorUpdate);
	RegisterFunction("", OAPI_KEY_E, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ProjectApolloMFD::menuVoid);
	RegisterFunction("SRC", OAPI_KEY_S, &ProjectApolloMFD::menuSetSource);
	RegisterFunction("REF", OAPI_KEY_R, &ProjectApolloMFD::menuSetReference);

	RegisterFunction("AEAA", OAPI_KEY_A, &ProjectApolloMFD::menuAEAACommands);
	RegisterFunction("CLK", OAPI_KEY_C, &ProjectApolloMFD::menuClockUpdate);
	RegisterFunction("SAB", OAPI_KEY_F, &ProjectApolloMFD::menuSunburstSuborbitalAbort);
	RegisterFunction("COI", OAPI_KEY_G, &ProjectApolloMFD::menuSunburstCOI);
	RegisterFunction("SLT", OAPI_KEY_T, &ProjectApolloMFD::menuSetSVSlot);

	static const MFDBUTTONMENU mnuDebug[12] = {
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ "Clear the Debug Line",0,'C' },
		{ "Freeze debug line",0,'F' },
		{ "Back",0,'B' }
	};

	page.Debug = RegisterPage(mnuDebug, sizeof(mnuDebug) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_A, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ProjectApolloMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_J, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ProjectApolloMFD::menuVoid);
	RegisterFunction("CLR", OAPI_KEY_C, &ProjectApolloMFD::menuClearDebugLine);
	RegisterFunction("FRZ", OAPI_KEY_F, &ProjectApolloMFD::menuFreezeDebugLine);
	RegisterFunction("BCK", OAPI_KEY_B, &ProjectApolloMFD::menuSetMainPage);


	static const MFDBUTTONMENU mnuLGC[6] = {
		{ "Back", 0, 'B' },
		{ "Press ENTR on DSKY and DEDA",0,'D' },
		{ "Press ENTR on DSKY in CSM and LM",0,'E' },
		{ "Press PRO on DSKY in CSM and LM", 0, 'F' },
		{ "Calculate V42 Angles", 0, 'G' },
		{ "CSM and LM REFSMMAT types",0,'R' }
	};

	page.LGC = RegisterPage(mnuLGC, sizeof(mnuLGC) / sizeof(MFDBUTTONMENU));

	RegisterFunction("BCK", OAPI_KEY_B, &ProjectApolloMFD::menuSetMainPage);
	RegisterFunction("V47", OAPI_KEY_D, &ProjectApolloMFD::menuPressEnterOnDSKYDEDA);
	RegisterFunction("ENT", OAPI_KEY_E, &ProjectApolloMFD::menuPressEnterOnCMCLGC);
	RegisterFunction("PRO", OAPI_KEY_F, &ProjectApolloMFD::menuPressPROOnCMCLGC);
	RegisterFunction("V42", OAPI_KEY_G, &ProjectApolloMFD::CalculateV42Angles);
	RegisterFunction("", OAPI_KEY_A, &ProjectApolloMFD::menuVoid);
	RegisterFunction("REF", OAPI_KEY_R, &ProjectApolloMFD::menuCycleLMAlignType);


	static const MFDBUTTONMENU mnuFailures[12] = {
		{ "Set failure",0,'S' },
		{ "Clear failure",0,'D' },
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },

		{ "Cycle through pages",0,'P' },
		{ 0,0,0 },
		{ 0,0,0 },
		{ "Clear all failures",0,'C' },
		{ "Randomize failures",0,'R' },
		{ "Back",0,'B' }
	};

	page.Failures = RegisterPage(mnuFailures, sizeof(mnuFailures) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SET", OAPI_KEY_S, &ProjectApolloMFD::menuSetFailure);
	RegisterFunction("RES", OAPI_KEY_D, &ProjectApolloMFD::menuResetFailure);
	RegisterFunction("", OAPI_KEY_E, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ProjectApolloMFD::menuVoid);

	RegisterFunction("PAG", OAPI_KEY_P, &ProjectApolloMFD::menuCycleFailuresSubpage);
	RegisterFunction("", OAPI_KEY_J, &ProjectApolloMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ProjectApolloMFD::menuVoid);
	RegisterFunction("CLR", OAPI_KEY_C, &ProjectApolloMFD::menuClearAllFailures);
	RegisterFunction("RAN", OAPI_KEY_R, &ProjectApolloMFD::menuSetRandomFailures);
	RegisterFunction("BCK", OAPI_KEY_B, &ProjectApolloMFD::menuSetMainPage);
}

bool ProjectApolloMFDButtons::SearchForKeysInOtherPages() const
{
	return false;
}