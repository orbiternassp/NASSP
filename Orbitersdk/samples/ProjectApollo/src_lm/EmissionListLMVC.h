#ifndef __EMISSIONLISTLMVC_H
#define __EMISSIONLISTLMVC_H

// Emissionlist for LMVC.msh

DWORD FloodLights_LMVC[] = {
	VC_MAT_ACA_t,
	VC_MAT_Alluminum,
	VC_MAT_ArmRest_t,
	VC_MAT_Ascent_Engine_Cover_t,
	VC_MAT_Bags_Shelf_t,
	VC_MAT_Cabin_Recirculation_Assembly_t,
	VC_MAT_CB_black_t,
	VC_MAT_CB_white_t,
	VC_MAT_COAS_1_t,
	VC_MAT_COAS_1ovhd_t,
	VC_MAT_COAS_Holder1_t,
	VC_MAT_COAS_Holder1ovhd_t,
	VC_MAT_COASglass,
	VC_MAT_COASglass2,
	VC_MAT_Colors_t,
	VC_MAT_DSKY_Frame_t,
	VC_MAT_ECS_Hoses_Slot_t,
	VC_MAT_ECS_Rotary_t,
	VC_MAT_ECS_t,
	VC_MAT_EVA_Ant_Handle_t,
	VC_MAT_FDAI_LM_t,
	VC_MAT_FwdHatch_t,
	VC_MAT_GlareShields,
	VC_MAT_LM_Hull_t,
	VC_MAT_LM_Interior_t,
	VC_MAT_LM_Window_Frames,
	VC_MAT_LM11_t,
	VC_MAT_LMVC_2_t,
	VC_MAT_LMVC_t,
	VC_MAT_Protective_Net_t,
	VC_MAT_RCS_t,
	VC_MAT_Redfoil_t,
	VC_MAT_Rotary_LM_t,
	VC_MAT_Suit_Flow_Control_t,
	VC_MAT_Suit_Flow_Hose_t,
	VC_MAT_Switch_Guards_P14_t,
	VC_MAT_Talkbacks_t,
	VC_MAT_TempColor,
	VC_MAT_UpperHatch_t,
	VC_MAT_upperhatch_vc_t,
	VC_MAT_Water_Dispenser_t,
	VC_MAT_WaterControlModule_t,
	VC_MAT_WindowsRim,
	VC_MAT_FDAI_ball_LM_t,
	VC_MAT_LMVC_Tapemeter_t,
	VC_MAT_MasterAlarm1,
	VC_MAT_MasterAlarm2,
	VC_MAT_DEDA_Display,
	VC_MAT_DEDA_Light,
	VC_MAT_Ordeal_Knob,

	// DSKY Lights
	VC_MAT_DSKY_LIGHTS_NO_ATT,
	VC_MAT_DSKY_LIGHTS_GIMBAL_LOCK,
	VC_MAT_DSKY_LIGHTS_KEY_REL,
	VC_MAT_DSKY_LIGHTS_OPR_ERR,
	VC_MAT_DSKY_LIGHTS_PROG,
	VC_MAT_DSKY_LIGHTS_RESTART,
	VC_MAT_DSKY_LIGHTS_STBY,
	VC_MAT_DSKY_LIGHTS_TEMP,
	VC_MAT_DSKY_LIGHTS_TRACKER,
	VC_MAT_DSKY_LIGHTS_UPLINK_ACTY,

	// CW Lights
	VC_MAT_Panel_01_CW_Lights_AGS,
	VC_MAT_Panel_01_CW_Lights_ASC_PRESS,
	VC_MAT_Panel_01_CW_Lights_CABIN,
	VC_MAT_Panel_01_CW_Lights_CES_AC,
	VC_MAT_Panel_01_CW_Lights_CES_DC,
	VC_MAT_Panel_01_CW_Lights_DC_BUS,
	VC_MAT_Panel_01_CW_Lights_DES_QTY,
	VC_MAT_Panel_01_CW_Lights_DES_REG,
	VC_MAT_Panel_01_CW_Lights_ISS,
	VC_MAT_Panel_01_CW_Lights_LGC,
	VC_MAT_Panel_01_CW_Lights_RCS_A_REG,
	VC_MAT_Panel_01_CW_Lights_RCS_B_REG,
	VC_MAT_Panel_01_CW_Lights_RCS_TCA,
	VC_MAT_Panel_01_CW_Lights_SUIT_FAN,
	VC_MAT_Panel_02_CW_Lights_ASC_HI_REG,
	VC_MAT_Panel_02_CW_Lights_ASC_QTY,
	VC_MAT_Panel_02_CW_Lights_BATTERY,
	VC_MAT_Panel_02_CW_Lights_CW_PWR,
	VC_MAT_Panel_02_CW_Lights_ECS,
	VC_MAT_Panel_02_CW_Lights_ED_RELAYS,
	VC_MAT_Panel_02_CW_Lights_ENG_GMBL,
	VC_MAT_Panel_02_CW_Lights_GLYCOL,
	VC_MAT_Panel_02_CW_Lights_HEATER,
	VC_MAT_Panel_02_CW_Lights_INVERTER,
	VC_MAT_Panel_02_CW_Lights_LDG_RDR,
	VC_MAT_Panel_02_CW_Lights_O2_QTY,
	VC_MAT_Panel_02_CW_Lights_PRE_AMPS,
	VC_MAT_Panel_02_CW_Lights_RCS,
	VC_MAT_Panel_02_CW_Lights_RNDZ_RDR,
	VC_MAT_Panel_02_CW_Lights_SBD_RCVR,
	VC_MAT_Panel_02_CW_Lights_WATER_QTY,

	//Tapemeter Lights
	VC_MAT_Panel1_Tapemeter_AltAltRate,
	VC_MAT_Panel1_Tapemeter_RangeRangeRate,
	VC_MAT_RCS_HE_PRESS_x10,

// Caution & Warning Lights Frame
	VC_MAT_CW_Frame
};

DWORD IntegralLights_LMVC[] = {
	VC_MAT_LMVC_2_t,
	VC_MAT_LMVC_t,
	VC_MAT_Rotary_LM_t,
	VC_MAT_FDAI_LM_t,
	VC_MAT_Ordeal_Knob
//	VC_MAT_FDAI_ball_LM_t
};

DWORD MasterAlarm_NoTex[] = {
	VC_MAT_MasterAlarm1,
	VC_MAT_MasterAlarm2
};

DWORD DEDA_LightsFullLit[] = {
	VC_MAT_DEDA_Light
};


DWORD IntegralLights_LMVC_NoTex[] = {
//	VC_MAT_LMVC_2_t,
//	VC_MAT_LMVC_t,
//	VC_MAT_Rotary_LM_t,
//	VC_MAT_FDAI_LM_t,
	VC_MAT_LMVC_Tapemeter_t,
	VC_MAT_FDAI_ball_LM_t
};

DWORD NumericLights_LMVC[] = {
	VC_MAT_Panel4_DSKY,
	VC_MAT_Panel1_Numerics_Timer,
	VC_MAT_DEDA_Display,
/*	VC_MAT_L01_PwrFail_XpointerL,
	VC_MAT_L02_PwrFail_Thrust,
	VC_MAT_L03_PwrFail_DPSpress,
	VC_MAT_L04_PwrFail_RCSpress,
	VC_MAT_L05_PwrFail_RCSquan,
	VC_MAT_L06_PwrFail_ECSpress,
	VC_MAT_L07_PwrFail_Glycol,
	VC_MAT_L08_PwrFail_ECSquan,
	VC_MAT_L09_PwrFail_XpointerR,
	VC_MAT_L10_ContactLight1,
	VC_MAT_L11_ContactLight2,
	VC_MAT_L12_CompLight1_RRnottrack,
	VC_MAT_L13_CompLight2_Glycol,
	VC_MAT_L14_CompLight3_SuitFan,
	VC_MAT_L15_CompLight4_CO2,
	VC_MAT_L16_CompLight5_H2Osep,
	VC_MAT_L17_CompLight6_DCBus,
	VC_MAT_L18_CompLight7_BatFault,
	VC_MAT_L19_StageSeq_SysA,
	VC_MAT_L20_StageSeq_SysB,
	VC_MAT_L21_PwrFail_RangeRate
*/
};

DWORD LMVC_DSKY_Lights[] = {
	VC_MAT_DSKY_LIGHTS_NO_ATT,
	VC_MAT_DSKY_LIGHTS_GIMBAL_LOCK,
	VC_MAT_DSKY_LIGHTS_KEY_REL,
	VC_MAT_DSKY_LIGHTS_OPR_ERR,
	VC_MAT_DSKY_LIGHTS_PROG,
	VC_MAT_DSKY_LIGHTS_RESTART,
	VC_MAT_DSKY_LIGHTS_STBY,
	VC_MAT_DSKY_LIGHTS_TEMP,
	VC_MAT_DSKY_LIGHTS_TRACKER,
	VC_MAT_DSKY_LIGHTS_UPLINK_ACTY
};

DWORD LMVC_CW_Lights[5][8] = {
	{
		VC_MAT_NONE,
		VC_MAT_Panel_01_CW_Lights_CES_AC,
		VC_MAT_Panel_01_CW_Lights_RCS_TCA,
		VC_MAT_Panel_01_CW_Lights_CABIN,
		VC_MAT_Panel_02_CW_Lights_ASC_HI_REG,
		VC_MAT_Panel_02_CW_Lights_INVERTER,
		VC_MAT_Panel_02_CW_Lights_ED_RELAYS,
		VC_MAT_Panel_02_CW_Lights_ECS
	},

	{
		VC_MAT_Panel_01_CW_Lights_ASC_PRESS,
		VC_MAT_Panel_01_CW_Lights_CES_DC,
		VC_MAT_Panel_01_CW_Lights_RCS_A_REG,
		VC_MAT_Panel_01_CW_Lights_SUIT_FAN,
		VC_MAT_Panel_02_CW_Lights_ASC_QTY,
		VC_MAT_Panel_02_CW_Lights_BATTERY,
		VC_MAT_Panel_02_CW_Lights_RCS,
		VC_MAT_Panel_02_CW_Lights_O2_QTY
	},

	{
		VC_MAT_Panel_01_CW_Lights_DES_REG,
		VC_MAT_Panel_01_CW_Lights_AGS,
		VC_MAT_Panel_01_CW_Lights_RCS_B_REG,
		VC_MAT_NONE,
		VC_MAT_Panel_02_CW_Lights_ENG_GMBL,
		VC_MAT_Panel_02_CW_Lights_RNDZ_RDR,
		VC_MAT_Panel_02_CW_Lights_HEATER,
		VC_MAT_Panel_02_CW_Lights_GLYCOL
	},

	{
		VC_MAT_Panel_01_CW_Lights_DES_QTY,
		VC_MAT_Panel_01_CW_Lights_LGC,
		VC_MAT_Panel_01_CW_Lights_DC_BUS,
		VC_MAT_NONE,
		VC_MAT_NONE,
		VC_MAT_Panel_02_CW_Lights_LDG_RDR,
		VC_MAT_Panel_02_CW_Lights_CW_PWR,
		VC_MAT_Panel_02_CW_Lights_WATER_QTY
	},

	{
		VC_MAT_NONE,
		VC_MAT_Panel_01_CW_Lights_ISS,
		VC_MAT_NONE,
		VC_MAT_NONE,
		VC_MAT_NONE,
		VC_MAT_Panel_02_CW_Lights_PRE_AMPS,
		VC_MAT_NONE,
		VC_MAT_Panel_02_CW_Lights_SBD_RCVR,
	}
};

//Tapemeter Lights
DWORD Tapemeter_AltAltRate[] = {
	VC_MAT_Panel1_Tapemeter_AltAltRate
};

DWORD Tapemeter_RangeRangeRate[] = {
	VC_MAT_Panel1_Tapemeter_RangeRangeRate
};

DWORD RCS_Helium_Press_x10[] = {
	VC_MAT_RCS_HE_PRESS_x10
};


// External Meshes
DWORD FloodLights_XPointer_Shades[] = {
	0
};

DWORD FloodLights_WindowShades[] = {
	0
};

#endif // !__EMISSIONLISTCMVC