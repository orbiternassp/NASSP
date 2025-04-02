#ifndef __EMISSIONLISTCMVC_H
#define __EMISSIONLISTCMVC_H

// Emissionlist for CMVC.msh

//
// Material Emissions Panel 8
//
DWORD IntegralLights_P8[] = {

	// TODO Material List
	VC_MAT_CMVCTex1_ILL_P8_t,
	VC_MAT_CMVCTex1_t,
	VC_MAT_CMVCTex2_ILL_P8_t,
	VC_MAT_CMVCTex2_t,
	VC_MAT_CMVCTex3_ILL_P8_t,
	VC_MAT_csm_cabin_press_panel_t,
	VC_MAT_csm_lower_equip_bay_t,
	VC_MAT_csm_right_cb_panels_t,
	VC_MAT_DSKY_Keys_t,
	VC_MAT_FDAI_Frame_t,
	VC_MAT_Meters_P1_t,
	VC_MAT_Meters_P2_P8_t,
	VC_MAT_Rotarys_P8_t
};

// Material Emissions Panel 8 without Emission Texture
DWORD IntergralLights_P8_NTex[] = {

	// TODO Material List
//	VC_MAT_CWLights_P8_t,
	VC_MAT_EMS_Scroll_Timer_P1_t,
	VC_MAT_FDAI_Ball_t,
	VC_MAT_AttitudeSetNumbers

};

// Material Emissions Panel 8 without Emission Texture
DWORD NumericLights_P8_NTex[] = {

	// TODO Material List
	VC_MAT_DSKY_P8_t,
	VC_MAT_EMS_DeltaV_Range_P8_t,
	VC_MAT_MissionTimer_P2
};

DWORD NumericLights_P8[] = {

	// TODO Material List
	VC_MAT_DSKY_P8_t
};

DWORD FloodLights_P8[] = {

	// TODO Material List
	VC_MAT_AdditionalParts_t,
//	VC_MAT_AttitudeSetNumbers,
	VC_MAT_Altimeter_Cover,
	VC_MAT_Sextant_Eyepiece,
	VC_MAT_Telescope_Eyepiece,
//	VC_MAT_CM_Switches,
	VC_MAT_Accelerometer_Cover,
	VC_MAT_DSKY_Glareshade,
	VC_MAT_MissionTimer_Glareshade,
	VC_MAT_EMSDeltaV_Glareshield,
	VC_MAT_Alluminum,
	VC_MAT_CB_black_t,
	VC_MAT_CM_VC_Color_1,
	VC_MAT_CM_VC_Color_2,
	VC_MAT_CM_VC_Color_3,
	VC_MAT_CM_VC_Color_4,
	VC_MAT_CM_VC_Color_6,
	VC_MAT_CM_VC_Color_7,
	VC_MAT_CMFloor_t,
	VC_MAT_CMVCDetTex1_t,
	VC_MAT_CMVCDetTex3_t,
	VC_MAT_CMVCDetTex4_t,
	VC_MAT_CMVCDetTex6_t,
	VC_MAT_CMVCTex1_ILL_P100_t,
	VC_MAT_CMVCTex1_ILL_P5_t,
	VC_MAT_CMVCTex1_ILL_P8_t,
	VC_MAT_CMVCTex1_t,
	VC_MAT_CMVCTex2_ILL_P100_t,
	VC_MAT_CMVCTex2_ILL_P5_t,
	VC_MAT_CMVCTex2_ILL_P8_t,
	VC_MAT_CMVCTex2_t,
	VC_MAT_CMVCTex3_ILL_P5_t,
	VC_MAT_CMVCTex3_ILL_P8_t,
	VC_MAT_CMVCTex3_t,
	VC_MAT_coas_t,
	VC_MAT_Colors_t,
	VC_MAT_csm_cabin_press_panel_t,
	VC_MAT_csm_lower_equip_bay_t,
	VC_MAT_csm_right_cb_panels_ILL_P5_t,
	VC_MAT_csm_right_cb_panels_ILL_P100_t,
	VC_MAT_csm_right_cb_panels_t,
	VC_MAT_DOR_t,
	VC_MAT_DSKY_Keys_LEB_t,
	VC_MAT_DSKY_Keys_t,
	VC_MAT_FDAI_Frame_t,
	VC_MAT_FwdHatch_t,
	VC_MAT_HolderRahmen_t,
	VC_MAT_HOPEN_t,
	VC_MAT_LMVC_t,
	VC_MAT_MAINVCTEX_t,
	VC_MAT_Meters_P1_t,
	VC_MAT_Meters_P2_P8_t,
	VC_MAT_Meters_P2_t,
	VC_MAT_Meters_P3_t,
	VC_MAT_Ordeal_Knob,
	VC_MAT_Ordeal_Lighting,
	VC_MAT_Rotarys_P100_t,
	VC_MAT_Rotarys_P5_t,
	VC_MAT_Rotarys_P8_t,
	VC_MAT_SCS_Rates_Card_t,
	VC_MAT_shutte1_t,
	VC_MAT_SideHatch_t,
	VC_MAT_SwitchCover_t,
	VC_MAT_Talkbacks_Frame_t,
	VC_MAT_Talkbacks_P2_t,
	VC_MAT_Talkbacks_P3_t,
	VC_MAT_MASTERALARM_PANEL1,
	VC_MAT_MASTERALARM_PANEL2,
	VC_MAT_ABORTLIGHT_PANEL1,
	VC_MAT_CMVC_LIFT_OFF_NO_ABORT,
	VC_MAT_MissionTimerLEB,
	VC_MAT_MissionTimer_P2,
	VC_MAT_Panel306,
	VC_MAT_EventTimerLEB,
	VC_MAT_csm_waste_disposal_rotary,
	VC_MAT_Panel382_Cover,

	// DSKY Panel 2 Lights Materials
	VC_MAT_DSKY_Lights_GIMBAL_LOCK,
	VC_MAT_DSKY_Lights_KEY_REL,
	VC_MAT_DSKY_Lights_NO_ATT,
	VC_MAT_DSKY_Lights_OPR_ERR,
	VC_MAT_DSKY_Lights_PROG,
	VC_MAT_DSKY_Lights_RESTART,	
	VC_MAT_DSKY_Lights_STBY,
	VC_MAT_DSKY_Lights_TEMP,
	VC_MAT_DSKY_Lights_TRACKER,
	VC_MAT_DSKY_Lights_UPLINK_ACTY,

	// DSKY LEB Lights Materials
	VC_MAT_DSKY_LIGHT_LEB_GIMBAL_LOCK,
	VC_MAT_DSKY_LIGHT_LEB_KEY_REL,
	VC_MAT_DSKY_LIGHT_LEB_NO_ATT,
	VC_MAT_DSKY_LIGHT_LEB_OPR_ERR,
	VC_MAT_DSKY_LIGHT_LEB_PROG,
	VC_MAT_DSKY_LIGHT_LEB_RESTART,
	VC_MAT_DSKY_LIGHT_LEB_STBY,
	VC_MAT_DSKY_LIGHT_LEB_TEMP,
	VC_MAT_DSKY_LIGHT_LEB_TRACKER,
	VC_MAT_DSKY_LIGHT_LEB_UPLINK_ACTY,

	// CW Lights Materials
	VC_MAT_CWLights_P8_t,
	VC_MAT_CW_Lights_P2_BMAG1_TEMP,
	VC_MAT_CW_Lights_P2_BMAG2_TEMP,
	VC_MAT_CW_Lights_P2_CO2_PPHI,
	VC_MAT_CW_Lights_P2_PITCH_GMBL1,
	VC_MAT_CW_Lights_P2_YAW_GMBL1,
	VC_MAT_CW_Lights_P2_PITCH_GMBL2,
	VC_MAT_CW_Lights_P2_YAW_GMBL2,
	VC_MAT_CW_Lights_P2_CRYO_PRESS,
	VC_MAT_CW_Lights_P2_GLYCOL_TEMP_LOW,
	VC_MAT_CW_Lights_P2_CM_RCS1,
	VC_MAT_CW_Lights_P2_CM_RCS2,
	VC_MAT_CW_Lights_P2_SM_RCS_A,
	VC_MAT_CW_Lights_P2_SM_RCS_B,
	VC_MAT_CW_Lights_P2_SM_RCS_C,
	VC_MAT_CW_Lights_P2_SM_RCS_D,
	VC_MAT_CW_Lights_P2_FC1,
	VC_MAT_CW_Lights_P2_FC2,
	VC_MAT_CW_Lights_P2_FC3,
	VC_MAT_CW_Lights_P2_INV1_TEMP_HI,
	VC_MAT_CW_Lights_P2_INV2_TEMP_HI,
	VC_MAT_CW_Lights_P2_INV3_TEMP_HI,
	VC_MAT_CW_Lights_P2_SPS_PRESS,
	VC_MAT_CW_Lights_P2_AC_BUS1,
	VC_MAT_CW_Lights_P2_AC_BUS2,
	VC_MAT_CW_Lights_P2_FC_BUS_DISCONNECT,
	VC_MAT_CW_Lights_P2_AC_BUS1_OVERLOAD,
	VC_MAT_CW_Lights_P2_AC_BUS2_OVERLOAD,
	VC_MAT_CW_Lights_P2_CW,
	VC_MAT_CW_Lights_P2_CREW_ALERT,
	VC_MAT_CW_Lights_P2_MN_BUS_A_UNDERVOLT,
	VC_MAT_CW_Lights_P2_MN_BUS_B_UNDERVOLT,
	VC_MAT_CW_Lights_P2_ISS,
	VC_MAT_CW_Lights_P2_CMC,
	VC_MAT_CW_Lights_P2_O2_FLOW_HI,
	VC_MAT_CW_Lights_P2_SUIT_COMPRESSOR,
	VC_MAT_CW_Lights_P2_SPS_FLANGE_TEMP_HI,

	// LV Engines
	VC_MAT_LVEngine_5_1,
	VC_MAT_LVEngine_5_1_1,
	VC_MAT_LVEngine_5_1_2,
	VC_MAT_LVEngine_5_2,
	VC_MAT_LVEngine_5_2_1,
	VC_MAT_LVEngine_5_2_2,
	VC_MAT_LVEngine_5_2_3,
	VC_MAT_LVEngine_5_3,
	VC_MAT_LVEngine_5_3_1,
	VC_MAT_LVEngine_5_3_2,
	VC_MAT_LVEngine_5_3_3,
	VC_MAT_LVEngine_5_4,
	VC_MAT_LVEngine_5_4_1,
	VC_MAT_LVEngine_5_4_2,
	VC_MAT_LVEngine_5_5,
	VC_MAT_LVEngine_5_5_1,
	VC_MAT_LVEngine_5_5_2,
	VC_MAT_LVEngine_5_5_3,
	VC_MAT_LVEngine_8_1,
	VC_MAT_LVEngine_8_2,
	VC_MAT_LVEngine_8_3,
	VC_MAT_LVEngine_8_4,
	VC_MAT_LVEngine_8_5,
	VC_MAT_LVEngine_8_6,
	VC_MAT_LVEngine_8_7,
	VC_MAT_LVEngine_8_8,

	VC_MAT_LV_ENG_LV_GUID,
	VC_MAT_LV_ENG_LV_RATE,
	VC_MAT_LV_ENG_SII_SEP,

	VC_MAT_LiftVectorindicatorBottom,
	VC_MAT_LiftVectorindicatorTop,

	VC_MAT_Panel1_05G,
	VC_MAT_Panel1_SPS_THRUST,

	// LEB Condition Lamps
	VC_MAT_LEB_ConditionLamp_CMC,
	VC_MAT_LEB_ConditionLamp_ISS,
	VC_MAT_LEB_ConditionLamp_PGNS,

	VC_MAT_MasterAlarm_LEB,

	VC_MAT_Panel_225_226_229_Guards
};

//
// Material Emissions Panel 5
//
DWORD IntegralLights_P5[] = {

	// TODO Material List
	VC_MAT_CMVCTex1_ILL_P5_t,
	VC_MAT_CMVCTex2_ILL_P5_t,
	VC_MAT_CMVCTex3_ILL_P5_t,
	VC_MAT_csm_right_cb_panels_ILL_P5_t,
	VC_MAT_Meters_P2_t,
	VC_MAT_Meters_P3_t,
	VC_MAT_Rotarys_P5_t,
	VC_MAT_Talkbacks_P2_t,
	VC_MAT_Talkbacks_P3_t

};

DWORD FloodLights_P5[] = {

	// TODO Material List
	VC_MAT_CB_black_t,
	VC_MAT_CMVCTex2_t,
	VC_MAT_CMVCTex3_t,
	VC_MAT_Colors_t,
	VC_MAT_csm_right_cb_panels_t,
	VC_MAT_LMVC_t,
	VC_MAT_Rotarys_P5_t
};

//
// Material Emissions Panel 100 (LEB)
// Integral Lights controlled by Panel 100 Integral Rheostat
//
DWORD IntegralLights_P100[] = {

	// TODO Material List
	VC_MAT_CMVCTex1_ILL_P100_t,
	VC_MAT_CMVCTex2_ILL_P100_t,
	VC_MAT_csm_right_cb_panels_ILL_P100_t,
	VC_MAT_DSKY_Keys_LEB_t,
	VC_MAT_Rotarys_P100_t,
	VC_MAT_Panel306,
};

DWORD IntegralLights_P100_NoTex[] = {
	VC_MAT_EventTimerLEB
};

DWORD FloodLights_P100[] = {

	// TODO Material List
	VC_MAT_csm_lower_equip_bay_t,
	VC_MAT_csm_right_cb_panels_t,
	VC_MAT_MAINVCTEX_t
};

DWORD NumericLights_P100[] = {

	// TODO Material List
	VC_MAT_DSKY_LEB_CMVCTex2_t,
	VC_MAT_MissionTimerLEB

};

//
// DSKY Panel 2 Lights
//
DWORD IntegralLights_DSKY_Lights[] = {
	VC_MAT_DSKY_Lights_GIMBAL_LOCK,
	VC_MAT_DSKY_Lights_KEY_REL,
	VC_MAT_DSKY_Lights_NO_ATT,
	VC_MAT_DSKY_Lights_OPR_ERR,
	VC_MAT_DSKY_Lights_PROG,
	VC_MAT_DSKY_Lights_RESTART,
	VC_MAT_DSKY_Lights_STBY,
	VC_MAT_DSKY_Lights_TEMP,
	VC_MAT_DSKY_Lights_TRACKER,
	VC_MAT_DSKY_Lights_UPLINK_ACTY,
};

DWORD FullLitMasterAlarm1[] = {
	VC_MAT_MASTERALARM_PANEL1
};

DWORD FullLitMasterAlarm2[] = {
	VC_MAT_MASTERALARM_PANEL2
};

DWORD FullLitAbort[]{
	VC_MAT_ABORTLIGHT_PANEL1
};

DWORD FullLitLiftOffNoAutoAbort[]{
	VC_MAT_CMVC_LIFT_OFF_NO_ABORT
};


//
// DSKY LEB Lights Materials
//
DWORD IntegralLights_DSKY_LEB_Lights[] = {
	VC_MAT_DSKY_LIGHT_LEB_GIMBAL_LOCK,
	VC_MAT_DSKY_LIGHT_LEB_KEY_REL,
	VC_MAT_DSKY_LIGHT_LEB_NO_ATT,
	VC_MAT_DSKY_LIGHT_LEB_OPR_ERR,
	VC_MAT_DSKY_LIGHT_LEB_PROG,
	VC_MAT_DSKY_LIGHT_LEB_RESTART,
	VC_MAT_DSKY_LIGHT_LEB_STBY,
	VC_MAT_DSKY_LIGHT_LEB_TEMP,
	VC_MAT_DSKY_LIGHT_LEB_TRACKER,
	VC_MAT_DSKY_LIGHT_LEB_UPLINK_ACTY,

};

//
// Caution & Warning Lights Left Panel
//
DWORD IntegralLights_CW_LeftLights[] = {
	VC_MAT_CW_Lights_P2_BMAG1_TEMP,
	VC_MAT_CW_Lights_P2_BMAG2_TEMP,
	VC_MAT_CW_Lights_P2_CO2_PPHI,
	VC_MAT_CW_Lights_P2_PITCH_GMBL1,
	VC_MAT_CW_Lights_P2_YAW_GMBL1,
	VC_MAT_CW_Lights_P2_PITCH_GMBL2,
	VC_MAT_CW_Lights_P2_YAW_GMBL2,
	VC_MAT_CW_Lights_P2_CRYO_PRESS,
	VC_MAT_CW_Lights_P2_GLYCOL_TEMP_LOW,
	VC_MAT_CW_Lights_P2_CM_RCS1,
	VC_MAT_CW_Lights_P2_CM_RCS2,
	VC_MAT_CW_Lights_P2_SM_RCS_A,
	VC_MAT_CW_Lights_P2_SM_RCS_B,
	VC_MAT_CW_Lights_P2_SM_RCS_C,
	VC_MAT_CW_Lights_P2_SM_RCS_D

};

//
// Caution & Warning Lights Right Panel
//
DWORD IntegralLights_CW_RightLights[] = {
	VC_MAT_CW_Lights_P2_SPS_FLANGE_TEMP_HI,
	VC_MAT_CW_Lights_P2_FC1,
	VC_MAT_CW_Lights_P2_FC2,
	VC_MAT_CW_Lights_P2_FC3,
	VC_MAT_CW_Lights_P2_INV1_TEMP_HI,
	VC_MAT_CW_Lights_P2_INV2_TEMP_HI,
	VC_MAT_CW_Lights_P2_INV3_TEMP_HI,
	VC_MAT_CW_Lights_P2_SPS_PRESS,
	VC_MAT_CW_Lights_P2_AC_BUS1,
	VC_MAT_CW_Lights_P2_AC_BUS2,
	VC_MAT_CW_Lights_P2_FC_BUS_DISCONNECT,
	VC_MAT_CW_Lights_P2_AC_BUS1_OVERLOAD,
	VC_MAT_CW_Lights_P2_AC_BUS2_OVERLOAD,
	VC_MAT_CW_Lights_P2_CMC,
	VC_MAT_CW_Lights_P2_CREW_ALERT,
	VC_MAT_CW_Lights_P2_MN_BUS_A_UNDERVOLT,
	VC_MAT_CW_Lights_P2_MN_BUS_B_UNDERVOLT,
	VC_MAT_CW_Lights_P2_ISS,
	VC_MAT_CW_Lights_P2_CW,
	VC_MAT_CW_Lights_P2_O2_FLOW_HI,
	VC_MAT_CW_Lights_P2_SUIT_COMPRESSOR

};

//
// Caution & Warning Lights 
//
DWORD IntegralLights_CW_Lights[] = {
	VC_MAT_CW_Lights_P2_BMAG1_TEMP,
	VC_MAT_CW_Lights_P2_BMAG2_TEMP,
	VC_MAT_NONE,
	VC_MAT_CW_Lights_P2_CO2_PPHI,
	VC_MAT_CW_Lights_P2_PITCH_GMBL1,
	VC_MAT_CW_Lights_P2_YAW_GMBL1,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_CW_Lights_P2_PITCH_GMBL2,
	VC_MAT_CW_Lights_P2_YAW_GMBL2,
	VC_MAT_CW_Lights_P2_CRYO_PRESS,
	VC_MAT_CW_Lights_P2_GLYCOL_TEMP_LOW,
	VC_MAT_CW_Lights_P2_CM_RCS1,
	VC_MAT_CW_Lights_P2_CM_RCS2,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_CW_Lights_P2_SM_RCS_A,
	VC_MAT_CW_Lights_P2_SM_RCS_B,
	VC_MAT_CW_Lights_P2_SM_RCS_C,
	VC_MAT_CW_Lights_P2_SM_RCS_D,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_CW_Lights_P2_SPS_FLANGE_TEMP_HI,
	VC_MAT_CW_Lights_P2_FC1,
	VC_MAT_CW_Lights_P2_FC2,
	VC_MAT_CW_Lights_P2_FC3,
	VC_MAT_NONE,
	VC_MAT_CW_Lights_P2_INV1_TEMP_HI,
	VC_MAT_CW_Lights_P2_INV2_TEMP_HI,
	VC_MAT_CW_Lights_P2_INV3_TEMP_HI,
	VC_MAT_CW_Lights_P2_SPS_PRESS,
	VC_MAT_NONE,
	VC_MAT_CW_Lights_P2_AC_BUS1,
	VC_MAT_CW_Lights_P2_AC_BUS2,
	VC_MAT_NONE,
	VC_MAT_CW_Lights_P2_FC_BUS_DISCONNECT,
	VC_MAT_CW_Lights_P2_AC_BUS1_OVERLOAD,
	VC_MAT_CW_Lights_P2_AC_BUS2_OVERLOAD,
	VC_MAT_CW_Lights_P2_CMC,
	VC_MAT_CW_Lights_P2_CREW_ALERT,
	VC_MAT_CW_Lights_P2_MN_BUS_A_UNDERVOLT,
	VC_MAT_CW_Lights_P2_MN_BUS_B_UNDERVOLT,
	VC_MAT_CW_Lights_P2_ISS,
	VC_MAT_CW_Lights_P2_CW,
	VC_MAT_CW_Lights_P2_O2_FLOW_HI,
	VC_MAT_CW_Lights_P2_SUIT_COMPRESSOR,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE
};

//
// Caution & Warning Lights 
//
DWORD IntegralLights_CW_Lights_CM[] = {
	VC_MAT_CW_Lights_P2_BMAG1_TEMP,
	VC_MAT_CW_Lights_P2_BMAG2_TEMP,
	VC_MAT_NONE,
	VC_MAT_CW_Lights_P2_CO2_PPHI,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_CW_Lights_P2_CM_RCS1,
	VC_MAT_CW_Lights_P2_CM_RCS2,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_CW_Lights_P2_INV1_TEMP_HI,
	VC_MAT_CW_Lights_P2_INV2_TEMP_HI,
	VC_MAT_CW_Lights_P2_INV3_TEMP_HI,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_CW_Lights_P2_AC_BUS1,
	VC_MAT_CW_Lights_P2_AC_BUS2,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_CW_Lights_P2_AC_BUS1_OVERLOAD,
	VC_MAT_CW_Lights_P2_AC_BUS2_OVERLOAD,
	VC_MAT_CW_Lights_P2_CMC,
	VC_MAT_CW_Lights_P2_CREW_ALERT,
	VC_MAT_CW_Lights_P2_MN_BUS_A_UNDERVOLT,
	VC_MAT_CW_Lights_P2_MN_BUS_B_UNDERVOLT,
	VC_MAT_CW_Lights_P2_ISS,
	VC_MAT_CW_Lights_P2_CW,
	VC_MAT_CW_Lights_P2_O2_FLOW_HI,
	VC_MAT_CW_Lights_P2_SUIT_COMPRESSOR,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE,
	VC_MAT_NONE
};

//
// EMS .05G Light
//
DWORD EMSPoint05GLight[] = {
	VC_MAT_Panel1_05G
};

//
// EMS SPS THRUST Light
//
DWORD EMSSPSThrustLight[] = {
	VC_MAT_Panel1_SPS_THRUST
};

//
// Ordeal
//
DWORD IntegralLights_CMVC_Ordeal[] = {
	VC_MAT_Ordeal_Knob,
	VC_MAT_Ordeal_Lighting
};

//
// LV Engines
//
DWORD LVEngine_5_1[] = {
	VC_MAT_LVEngine_5_1,
	VC_MAT_LVEngine_5_1_1,
	VC_MAT_LVEngine_5_1_2
};

DWORD LVEngine_5_2[] = {
	VC_MAT_LVEngine_5_2,
	VC_MAT_LVEngine_5_2_1,
	VC_MAT_LVEngine_5_2_2,
	VC_MAT_LVEngine_5_2_3
};

DWORD LVEngine_5_3[] = {
	VC_MAT_LVEngine_5_3,
	VC_MAT_LVEngine_5_3_1,
	VC_MAT_LVEngine_5_3_2,
	VC_MAT_LVEngine_5_3_3
};

DWORD LVEngine_5_4[] = {
	VC_MAT_LVEngine_5_4,
	VC_MAT_LVEngine_5_4_1,
	VC_MAT_LVEngine_5_4_2

};

DWORD LVEngine_5_5[] = {
	VC_MAT_LVEngine_5_5,
	VC_MAT_LVEngine_5_5_1,
	VC_MAT_LVEngine_5_5_2,
	VC_MAT_LVEngine_5_5_3
};

DWORD LVEngine_8_1[] = {
	VC_MAT_LVEngine_8_1,
	VC_MAT_LVEngine_5_1_2
};

DWORD LVEngine_8_2[] = {
	VC_MAT_LVEngine_8_2,
	VC_MAT_LVEngine_5_2_3
};

DWORD LVEngine_8_3[] = {
	VC_MAT_LVEngine_8_3,
	VC_MAT_LVEngine_5_3_1
};

DWORD LVEngine_8_4[] = {
	VC_MAT_LVEngine_8_4,
	VC_MAT_LVEngine_5_4_1
};

DWORD LVEngine_8_5[] = {
	VC_MAT_LVEngine_8_5,
	VC_MAT_LVEngine_5_4_2,
	VC_MAT_LVEngine_5_1_1,
	VC_MAT_LVEngine_5_5_2
};

DWORD LVEngine_8_6[] = {
	VC_MAT_LVEngine_8_6,
	VC_MAT_LVEngine_5_5_3,
	VC_MAT_LVEngine_5_2_2
};

DWORD LVEngine_8_7[] = {
	VC_MAT_LVEngine_8_7,
	VC_MAT_LVEngine_5_3_3,
	VC_MAT_LVEngine_5_2_1
};

DWORD LVEngine_8_8[] = {
	VC_MAT_LVEngine_8_8,
	VC_MAT_LVEngine_5_5_1,
	VC_MAT_LVEngine_5_3_2
};

DWORD LiftVectorIndUp[] = {
	VC_MAT_LiftVectorindicatorTop
};

DWORD LiftVectorIndDown[] = {
	VC_MAT_LiftVectorindicatorBottom
};

//
// External Meshes
//

// SeatsUnFolded.msh
DWORD CMVCSeatsUnFolded[] = {

	// TODO Material List
	VC_MAT_CM_VC_SeatsUnfolded_Couch_Frames_with_Tex,
	VC_MAT_CM_VC_SeatsUnfolded_Floodlights_with_Tex,
	VC_MAT_CM_VC_SeatsUnfolded_Floodlights_with_Tex2,
	VC_MAT_CM_VC_SeatsUnfolded_MTL_11,
	VC_MAT_CM_VC_SeatsUnfolded_MTL_7,
	VC_MAT_CM_VC_SeatsUnfolded_SeatWeave_with_Tex,
	VC_MAT_CM_VC_SeatsUnfolded_StdIntr_with_Tex,
	VC_MAT_CM_VC_SeatsUnfolded_StdIntr2
};

// SeatsFolded.msh
DWORD CMVCSeatsFolded[] = {

	// TODO Material List
	VC_MAT_CM_VC_SeatsFolded_Couch_Frames_with_Tex,
	VC_MAT_CM_VC_SeatsFolded_Floodlights_with_Tex,
	VC_MAT_CM_VC_SeatsFolded_Floodlights_with_Tex_2,
	VC_MAT_CM_VC_SeatsFolded_MTL_11,
	VC_MAT_CM_VC_SeatsFolded_MTL_7,
	VC_MAT_CM_VC_SeatsFolded_SeatWeave_with_Tex,
	VC_MAT_CM_VC_SeatsFolded_StdIntr2
};

//
// COAS
//
DWORD CMVC_COAS_CDR[] = {
	0
};

#endif // !__EMISSIONLISTCMVC
