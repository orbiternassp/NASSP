# Microsoft Developer Studio Project File - Name="FloatBag" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FloatBag - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "FloatBag.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "FloatBag.mak" CFG="FloatBag - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "FloatBag - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FloatBag - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FloatBag - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release\FloatBag"
# PROP BASE Intermediate_Dir "Release\FloatBag"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\FloatBag"
# PROP Intermediate_Dir "Release\FloatBag"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FloatBag_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../../include" /I "../../src_aux" /I "../../src_sys" /I "../../src_mfd" /I "../../src_moon" /I "../../src_lm" /I "../../src_csm" /I "../../src_launch" /I "../../src_landing" /I "../../src_saturn" /I "../../../../../Sound/OrbiterSound_SDK/VESSELSOUND_SDK/ShuttlePB_project" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FloatBag_EXPORTS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"../../../../../Modules/ProjectApollo/FloatBag.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 orbiter.lib orbitersdk.lib OrbiterSoundSDK40.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /machine:I386 /nodefaultlib:"LIBCMT" /out:"../../../../../Modules/ProjectApollo/FloatBag.dll" /libpath:"../../../../lib" /libpath:"../../../../../Sound/OrbiterSound_SDK/VESSELSOUND_SDK/ShuttlePB_project"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "FloatBag - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug\FloatBag"
# PROP BASE Intermediate_Dir "Debug\FloatBag"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\FloatBag"
# PROP Intermediate_Dir "Debug\FloatBag"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FloatBag_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../../include" /I "../../src_aux" /I "../../src_sys" /I "../../src_mfd" /I "../../src_moon" /I "../../src_lm" /I "../../src_csm" /I "../../src_launch" /I "../../src_landing" /I "../../src_saturn" /I "../../../../../Sound/OrbiterSound_SDK/VESSELSOUND_SDK/ShuttlePB_project" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FloatBag_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"../../../../../Modules/ProjectApollo/FloatBag.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 orbiter.lib orbitersdk.lib OrbiterSoundSDK40.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"LIBCMT" /nodefaultlib:"MSVCRT" /out:"../../../../../Modules/ProjectApollo/FloatBag.dll" /pdbtype:sept /libpath:"../../../../lib" /libpath:"../../../../../Sound/OrbiterSound_SDK/VESSELSOUND_SDK/ShuttlePB_project"

!ENDIF 

# Begin Target

# Name "FloatBag - Win32 Release"
# Name "FloatBag - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src_landing\FloatBag.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src_aux\tracer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src_landing\FloatBag.h
# End Source File
# Begin Source File

SOURCE=..\..\src_aux\tracer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "CollisionSDK"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src_aux\CollisionSDK\CollisionSDK.cpp
# End Source File
# End Group
# End Target
# End Project
