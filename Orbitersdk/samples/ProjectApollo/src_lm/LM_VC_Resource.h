// ========================================================
// Mesh resource file for LM_VC.msh
// Generated with meshc on Mon May 18 16:28:29 2020
// ========================================================

// Number of mesh groups:
#define VC_NGRP 99

// Number of materials:
#define VC_NMAT 26

// Number of textures:
#define VC_NTEX 5

// Named mesh groups:
#define VC_GRP_Drogue 0
#define VC_GRP_FDAIBall1_cdr 1
#define VC_GRP_FDAIBall1_lmp 2
#define VC_GRP_FDAIBall2_cdr 3
#define VC_GRP_FDAIBall2_lmp 4
#define VC_GRP_FDAIBall_cdr 5
#define VC_GRP_FDAIBall_lmp 6
#define VC_GRP_FDAI_cross_cdr 7
#define VC_GRP_FDAI_cross_lmp 8
#define VC_GRP_FDAI_pitcherror_cdr 9
#define VC_GRP_FDAI_pitcherror_lmp 10
#define VC_GRP_FDAI_rateP_cdr 11
#define VC_GRP_FDAI_rateP_lmp 12
#define VC_GRP_FDAI_rateR_cdr 13
#define VC_GRP_FDAI_rateR_lmp 14
#define VC_GRP_FDAI_rateY_cdr 15
#define VC_GRP_FDAI_rateY_lmp 16
#define VC_GRP_FDAI_rollerror_cdr 17
#define VC_GRP_FDAI_rollerror_lmp 18
#define VC_GRP_FDAI_yawerror_cdr 19
#define VC_GRP_FDAI_yawerror_lmp 20
#define VC_GRP_FDAIflag_cdr 21
#define VC_GRP_FDAIflag_lmp 22
#define VC_GRP_FDAIoverlay_cdr 23
#define VC_GRP_FDAIoverlay_lmp 24
#define VC_GRP_FwdHatch 25
#define VC_GRP_HatchRim 26
#define VC_GRP_HatchWall 27
#define VC_GRP_HullInterior 28
#define VC_GRP_HullMain 29
#define VC_GRP_HullSide 30
#define VC_GRP_HullTop 31
#define VC_GRP_L01_PwrFail_XpointerL 32
#define VC_GRP_L02_PwrFail_Thrust 33
#define VC_GRP_L03_PwrFail_Fuelpress 34
#define VC_GRP_L04_PwrFail_RCSpress 35
#define VC_GRP_L05_PwrFail_RCSquan 36
#define VC_GRP_L06_PwrFail_ECSpress 37
#define VC_GRP_L07_PwrFail_Glycol 38
#define VC_GRP_L08_PwrFail_ECSquan 39
#define VC_GRP_L09_PwrFail_XpointerR 40
#define VC_GRP_L10_ContactLight 41
#define VC_GRP_L11_ContactLight2 42
#define VC_GRP_L12_Comp_RRnotrack 43
#define VC_GRP_L13_CompLight_Glycol 44
#define VC_GRP_L14_CompLight_Suit_Fan 45
#define VC_GRP_L15_CompLight_CO2 46
#define VC_GRP_L16_CompLight_H2Osep 47
#define VC_GRP_LightsRail 48
#define VC_GRP_MasterAlarm1 49
#define VC_GRP_MasterAlarm2 50
#define VC_GRP_Needle_Radar 51
#define VC_GRP_Panel1 52
#define VC_GRP_Panel2 53
#define VC_GRP_Panel3 54
#define VC_GRP_Panel4 55
#define VC_GRP_RCSMounts 56
#define VC_GRP_RCSMounts2 57
#define VC_GRP_RCSMounts3 58
#define VC_GRP_RCSnozzles 59
#define VC_GRP_RCSnozzles2 60
#define VC_GRP_Rot_P3_01 61
#define VC_GRP_Rot_P3_02 62
#define VC_GRP_Rot_P3_03 63
#define VC_GRP_Rot_P3_04 64
#define VC_GRP_Rot_P3_05 65
#define VC_GRP_Sw_P3_01 66
#define VC_GRP_Sw_P3_02 67
#define VC_GRP_Sw_P3_03 68
#define VC_GRP_Sw_P3_04 69
#define VC_GRP_Sw_P3_05 70
#define VC_GRP_Sw_P3_06 71
#define VC_GRP_Sw_P3_07 72
#define VC_GRP_Sw_P3_08 73
#define VC_GRP_Sw_P3_09 74
#define VC_GRP_Sw_P3_10 75
#define VC_GRP_Sw_P3_11 76
#define VC_GRP_Sw_P3_12 77
#define VC_GRP_Sw_P3_13 78
#define VC_GRP_Sw_P3_14 79
#define VC_GRP_Sw_P3_15 80
#define VC_GRP_Sw_P3_16 81
#define VC_GRP_Sw_P3_17 82
#define VC_GRP_Sw_P3_18 83
#define VC_GRP_Sw_P3_19 84
#define VC_GRP_Sw_P3_20 85
#define VC_GRP_Sw_P3_21 86
#define VC_GRP_Sw_P3_22 87
#define VC_GRP_Sw_P3_23 88
#define VC_GRP_Sw_P3_24 89
#define VC_GRP_Sw_P3_25 90
#define VC_GRP_Sw_P3_26 91
#define VC_GRP_Tunnel 92
#define VC_GRP_UpperHatch 93
#define VC_GRP_Windows 94
#define VC_GRP_XpointerX_cdr 95
#define VC_GRP_XpointerX_lmp 96
#define VC_GRP_XpointerY_cdr 97
#define VC_GRP_XpointerY_lmp 98