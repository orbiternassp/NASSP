# Microsoft Developer Studio Project File - Name="Saturn1b" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Saturn1b - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Saturn1b.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Saturn1b.mak" CFG="Saturn1b - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Saturn1b - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Saturn1b - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Saturn1b - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release\Saturn1b"
# PROP BASE Intermediate_Dir "Release\Saturn1b"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\Saturn1b"
# PROP Intermediate_Dir "Release\Saturn1b"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../../include" /I "../../src_aux" /I "../../src_sys" /I "../../src_mfd" /I "../../src_moon" /I "../../src_lm" /I "../../src_csm" /I "../../src_launch" /I "../../src_landing" /I "../../src_saturn" /I "../../../../../Sound/OrbiterSound_SDK/VESSELSOUND_SDK/ShuttlePB_project" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Saturn1b.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 "..\..\src_aux\dsound.lib" winmm.lib PanelSDK.lib opengl32.lib glu32.lib ../../src_aux/dinput8.lib ../../src_aux/dxguid.lib WS2_32.lib orbiter.lib orbitersdk.lib OrbiterSoundSDK40.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"LIBCMT" /out:"../../../../../Modules/ProjectApollo/Saturn1b.dll" /libpath:"../../src_sys/PanelSDK" /libpath:"../../../../lib" /libpath:"../../../../../Sound/OrbiterSound_SDK/VESSELSOUND_SDK/ShuttlePB_project"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Saturn1b - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug\Saturn1b"
# PROP BASE Intermediate_Dir "Debug\Saturn1b"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\Saturn1b"
# PROP Intermediate_Dir "Debug\Saturn1b"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "../../../../include" /I "../../src_aux" /I "../../src_sys" /I "../../src_mfd" /I "../../src_moon" /I "../../src_lm" /I "../../src_csm" /I "../../src_launch" /I "../../src_landing" /I "../../src_saturn" /I "../../../../../Sound/OrbiterSound_SDK/VESSELSOUND_SDK/ShuttlePB_project" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Saturn1b.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 "..\..\src_aux\dsound.lib" winmm.lib PanelSDK.lib opengl32.lib glu32.lib ../../src_aux/dinput8.lib ../../src_aux/dxguid.lib WS2_32.lib orbiter.lib orbitersdk.lib OrbiterSoundSDK40.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBCMT" /out:"../../../../../Modules/ProjectApollo/Saturn1b.dll" /pdbtype:sept /libpath:"../../src_sys/PanelSDK" /libpath:"../../../../lib" /libpath:"../../../../../Sound/OrbiterSound_SDK/VESSELSOUND_SDK/ShuttlePB_project"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Saturn1b - Win32 Release"
# Name "Saturn1b - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=..\..\src_sys\apolloguidance.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_aux\BasicExcelVC6.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\cautionwarning.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\checklistController.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\checklistControllerHelpers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\connector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\csm_telecom.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\csmcautionwarning.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\CSMcomputer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\csmconnector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\csmrcs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\dockingprobe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\dsky.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\dskyinterface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\ecs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\FDAI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\imu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\imulog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\imumath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_saturn\iu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_saturn\lvimu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_launch\mcc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_mfd\MFDconnector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\missiontimer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\powersource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_aux\Prediction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\pyro.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_saturn\sat1ap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_saturn\sat1bmesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\satswitches.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\satsystems.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\saturn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_saturn\Saturn1b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_saturn\saturnap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\saturnmesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\saturnpanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\saturnvc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\scs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\secs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\soundevents.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\soundlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\sps.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\toggleswitch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_aux\tracer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\..\src_sys\apolloguidance.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\cautionwarning.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\checklistController.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\connector.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\csm_telecom.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\csmcautionwarning.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\CSMcomputer.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\csmconnector.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\dockingprobe.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\dsky.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\ecs.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\FDAI.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\IMU.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\ioChannels.h
# End Source File
# Begin Source File

SOURCE=..\..\src_saturn\iu.h
# End Source File
# Begin Source File

SOURCE=..\..\src_saturn\LES.h
# End Source File
# Begin Source File

SOURCE=..\..\src_saturn\LVIMU.h
# End Source File
# Begin Source File

SOURCE=..\..\src_launch\mcc.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\missiontimer.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\nasspdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\nasspsound.h
# End Source File
# Begin Source File

SOURCE=..\..\src_aux\OrbiterMath.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\papi.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\powersource.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\pyro.h
# End Source File
# Begin Source File

SOURCE=..\..\src_saturn\s1b.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\satswitches.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\saturn.h
# End Source File
# Begin Source File

SOURCE=..\..\src_saturn\Saturn1b.h
# End Source File
# Begin Source File

SOURCE=..\..\src_saturn\saturnv.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\scs.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\secs.h
# End Source File
# Begin Source File

SOURCE=..\..\src_saturn\sivb.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\sm.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\soundevents.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\soundlib.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\sps.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\toggleswitch.h
# End Source File
# Begin Source File

SOURCE=..\..\src_aux\tracer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\Bitmaps\Abort.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\ACVolts.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\allround.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\Altimeter.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\altimeter_2.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border116x116.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border118x118.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border150x80.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border160x32.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border17x36.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border200x80.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border23x20.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border23x23.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border29x29.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border31x31.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border32x160.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border33x43.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border34x29.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border34x31.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border34x33.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border34x34.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border34x39.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border34x61.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border36x17.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border38x38.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border38x52.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border39x38.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border45x36.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border46x75.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border47x47.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border48x48.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border50x158.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border55x111.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border57x57.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border58x58.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border65x65.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border70x70.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border72x109.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border72x72.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border75x64.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border78x78.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border84x84.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border87x111.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border90x90.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border92x40.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\cabin_press_guard_lever.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\cabin_press_lower_lever.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\cabin_press_upper_lever.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\circuit_breaker.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\circuit_breaker_yellow.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\CMMFDFrame.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\coas.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_accum_rotary.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_cabin_press_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_cabin_repress_valve.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_emergency_cabin_press_rotary.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_FootPrepWater_lever.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_glycol_rotary.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_glycol_to_radiators_knob.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_hatch_window.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_left_les_window_cover.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_left_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_left_rndz_window.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_lm_tunnel_vent_valve.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_lower_equip_bay.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_lower_equip_bay_center.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_lower_equip_bay_left.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_lower_equip_bay_right.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_lower_left_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_lower_main_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_main_left_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_main_middle_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_main_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_main_panel_les_window_cover.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_main_right_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_oxygen_surge_tank_valve.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_panel_101_dcvolts.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_press_relief_valve.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_right_cb_panels.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_right_les_window_cover.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_right_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_right_rndz_les_window_cover.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_right_rndz_window.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_secondary_cabin_temp_valve.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_selector_inlet_rotary.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_selector_outlet_rotary.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_sextant_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_sextant_panel_wide.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_suit_flow_control_lever.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_tank_valve.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_telescope_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_telescope_panel_wide.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\csm_waste_mgmt_rotary.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\cw_lights.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\DCAmps.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\dcvolts.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\digitaldisp.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\direct_o2_rotary.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\docking_switches.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\dsky_disp.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\dsky_keys.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\dsky_lights.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\ECSGlycolpump_rotary.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\ECSIndicator.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\ECSRotary.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\ems_button.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\ems_lights.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\EMS_RSIbkgrnd.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\EMSscroll_border.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\EMSscroll_LEO.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\event_timer.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\FDAI.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\FDAI_needles.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\FDAI_off_flag.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\FDAIPower_rotary.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\FDAIRoll.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\FDAIRollindicator.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\fonts_black.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\fonts_white.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\glycol_lever.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\gta_cover.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\guarded_switch20.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\Indicator.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\light_rotary.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\lv_eng.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\lv_eng_s1b.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\mark_buttons.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\masteralarmbright.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\MFDFrame.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\MFDPower.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\minimpulse_handcontroller.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\needle.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\optics_dsky.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\optics_handcontroller.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\ORDEAL_rotary.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\post_lndg_vlv_lever.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\Resources.rc
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\RotationalSwitch.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\SCSBMAGPower_rotary.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\SequencerSwitches.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\sps_injector_indicators.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\SPSmax_indicator.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\SPSmin_indicator.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\SuitCabinDeltaPMeter.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\suitreturn_lever.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\switch20.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\switch20_left.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\switch30.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\switch305_left.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\switch30_left.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\switch_lever.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\switch_lever_threepos.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\SwitchGuardPanel15.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\SwitchGuards.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\switchup.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\switchupsmall.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\thc.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\threeposswitch.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\threeposswitch20.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\threeposswitch20_left.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\threeposswitch30.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\threeposswitch305.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\threeposswitch305_left.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\threeposswitch30_left.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\threeposswitchsmall.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\Thrustmeter.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\thumbwheel_gpi_pitch.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\thumbwheel_gpi_yaw.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\thumbwheel_large_fonts.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\thumbwheel_large_fonts_inv.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\thumbwheel_small.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\thumbwheel_small_fonts.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\thumbwheel_small_fonts_diagonal.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\thumbwheel_small_fonts_diagonal_left.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\VHFAntenna_rotary.bmp
# End Source File
# End Group
# Begin Group "yaAGC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src_sys\yaAGC\agc_engine.c
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\yaAGC\agc_engine.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\yaAGC\agc_engine_init.c
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\yaAGC\Backtrace.c
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\yaAGC\random.c
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\yaAGC\rfopen.c
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\yaAGC\yaAGC.h
# End Source File
# End Group
# Begin Group "CollisionSDK"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src_aux\CollisionSDK\CollisionSDK.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_aux\CollisionSDK\CollisionSDK.h
# End Source File
# End Group
# Begin Group "IMFD"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src_aux\IMFD\IMFD_Client.cpp
# End Source File
# End Group
# End Target
# End Project
