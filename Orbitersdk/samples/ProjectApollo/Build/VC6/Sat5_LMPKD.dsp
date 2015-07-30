# Microsoft Developer Studio Project File - Name="SAT5_LMPKD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SAT5_LMPKD - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Sat5_LMPKD.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Sat5_LMPKD.mak" CFG="SAT5_LMPKD - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "SAT5_LMPKD - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SAT5_LMPKD - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SAT5_LMPKD - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release\Sat5_LMPKD"
# PROP BASE Intermediate_Dir "Release\Sat5_LMPKD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\Sat5_LMPKD"
# PROP Intermediate_Dir "Release\Sat5_LMPKD"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SAT5_LMPKD_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../../include" /I "../../src_aux" /I "../../src_sys" /I "../../src_mfd" /I "../../src_moon" /I "../../src_lm" /I "../../src_csm" /I "../../src_launch" /I "../../src_landing" /I "../../src_saturn" /I "../../../../../Sound/OrbiterSound_SDK/VESSELSOUND_SDK/ShuttlePB_project" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SAT5_LMPKD_EXPORTS" /D "DIRECTSOUNDENABLED" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"../../../../../Modules/ProjectApollo/Sat5_LMPKD.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ../../src_aux/dsound.lib PanelSDK.lib opengl32.lib glu32.lib winmm.lib ../../src_aux/dinput8.lib ../../src_aux/dxguid.lib orbiter.lib orbitersdk.lib OrbiterSoundSDK40.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib WS2_32.lib /nologo /dll /machine:I386 /nodefaultlib:"LIBCMT" /out:"../../../../../Modules/ProjectApollo/LEM.dll" /libpath:"../../src_sys/PanelSDK" /libpath:"../../../../lib" /libpath:"../../../../../Sound/OrbiterSound_SDK/VESSELSOUND_SDK/ShuttlePB_project"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "SAT5_LMPKD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug\Sat5_LMPKD"
# PROP BASE Intermediate_Dir "Debug\Sat5_LMPKD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\Sat5_LMPKD"
# PROP Intermediate_Dir "Debug\Sat5_LMPKD"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SAT5_LMPKD_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../../include" /I "../../src_aux" /I "../../src_sys" /I "../../src_mfd" /I "../../src_moon" /I "../../src_lm" /I "../../src_csm" /I "../../src_launch" /I "../../src_landing" /I "../../src_saturn" /I "../../../../../Sound/OrbiterSound_SDK/VESSELSOUND_SDK/ShuttlePB_project" /D "_DEBUG" /D "AGC_SOCKET_ENABLEDD" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SAT5_LMPKD_EXPORTS" /D "DIRECTSOUNDENABLED" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"../../../../../Modules/ProjectApollo/Sat5_LMPKD.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 "..\..\src_aux\dsound.lib" wsock32.lib PanelSDK.lib opengl32.lib glu32.lib winmm.lib ../../src_aux/dinput8.lib ../../src_aux/dxguid.lib orbiter.lib orbitersdk.lib OrbiterSoundSDK40.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib WS2_32.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"LIBCMT" /out:"../../../../../Modules/ProjectApollo/LEM.dll" /pdbtype:sept /libpath:"../../src_sys/PanelSDK" /libpath:"../../../../lib" /libpath:"../../../../../Sound/OrbiterSound_SDK/VESSELSOUND_SDK/ShuttlePB_project"

!ENDIF 

# Begin Target

# Name "SAT5_LMPKD - Win32 Release"
# Name "SAT5_LMPKD - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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

SOURCE=..\..\src_sys\dsky.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\dskyinterface.cpp
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

SOURCE=..\..\src_lm\LEM.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\lemautoascent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\lemautoland.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\LEMcomputer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\lemmesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\lempanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\lemswitches.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\lemsystems.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\lemvc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\lm_ags.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\lm_telecom.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\lmscs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_mfd\MFDconnector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\missiontimer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\payload.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\powersource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\soundevents.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\soundlib.cpp
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

# PROP Default_Filter "h;hpp;hxx;hm;inl"
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

SOURCE=..\..\src_csm\csmcautionwarning.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\CSMcomputer.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\dockingprobe.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\dsky.h
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

SOURCE=..\landervessel.h
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\LEM.h
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\LEMcomputer.h
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\lemswitches.h
# End Source File
# Begin Source File

SOURCE=..\..\src_moon\leva.h
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\lm_channels.h
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\lmscs.h
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

SOURCE=..\..\src_sys\powersource.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\pyro.h
# End Source File
# Begin Source File

SOURCE=..\..\src_aux\ProjectApolloConfigurator\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\satswitches.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\saturn.h
# End Source File
# Begin Source File

SOURCE=..\..\src_csm\secs.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\soundevents.h
# End Source File
# Begin Source File

SOURCE=..\..\src_sys\soundlib.h
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

SOURCE=..\Bitmaps\alignment_optical_telescope_view.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\Altimeter.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\Annonciators.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border23x20.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border29x29.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border32x160.bmp
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

SOURCE=..\Bitmaps\border38x52.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border39x38.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border46x75.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border50x158.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border55x111.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border70x70.bmp
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

SOURCE=..\Bitmaps\border90x90.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\border92x40.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\circuit_breaker.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\coas_lem1.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\coas_lem2.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\contact.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\digitaldisp.bmp
# End Source File
# Begin Source File

SOURCE=..\..\Bitmaps\digitaldisp_2.bmp
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

SOURCE=..\Bitmaps\ecs.bmp
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

SOURCE=..\Bitmaps\FDAIRoll.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\FDAIRollindicator.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\Gmeter.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\Horizon.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\Indicator.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\launchlight.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\lem_left_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\lem_left_window.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\lem_lpd_window.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\lem_main_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\lem_rendezvous_window.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\lem_right_panel.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\lem_right_window.bmp
# End Source File
# Begin Source File

SOURCE=..\..\Bitmaps\lem_stage_switch.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\LEMResources.rc
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\LemRotationalSwitch.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\lemswitch2.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\lemswitch3.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\lights2.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\LMAbortButton.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\LMDCAmps.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\LMDCVolts.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\LMMFDFrame.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\lv_eng.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\masteralarmbright.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\needle1.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\Pswitch.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\switch1.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\switch_lever.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\switchup.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\switchup3.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\threeposswitch.bmp
# End Source File
# Begin Source File

SOURCE=..\Bitmaps\Thrustmeter.bmp
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

SOURCE=..\..\src_sys\yaAGC\agc_utilities.c
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
# Begin Group "yaAGS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src_lm\yaAGS\aea_engine.c
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\yaAGS\aea_engine.h
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\yaAGS\aea_engine_init.c
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\yaAGS\OutputAPI_AGS.c
# End Source File
# Begin Source File

SOURCE=..\..\src_lm\yaAGS\yaAEA.h
# End Source File
# End Group
# End Target
# End Project
