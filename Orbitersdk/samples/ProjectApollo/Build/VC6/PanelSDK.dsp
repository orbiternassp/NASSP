# Microsoft Developer Studio Project File - Name="PanelSDK" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=PanelSDK - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "PanelSDK.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "PanelSDK.mak" CFG="PanelSDK - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "PanelSDK - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "PanelSDK - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PanelSDK - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release\PanelSDK"
# PROP BASE Intermediate_Dir "Release\PanelSDK"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\PanelSDK"
# PROP Intermediate_Dir "Release\PanelSDK"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../../include" /I "../../src_aux" /I "../../src_sys" /I "../../src_mfd" /I "../../src_moon" /I "../../src_lm" /I "../../src_csm" /I "../../src_launch" /I "../../src_landing" /I "../../src_saturn" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"PanelSDK.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"PanelSDK.lib"

!ELSEIF  "$(CFG)" == "PanelSDK - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug\PanelSDK"
# PROP BASE Intermediate_Dir "Debug\PanelSDK"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\PanelSDK"
# PROP Intermediate_Dir "Debug\PanelSDK"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../../include" /I "../../src_aux" /I "../../src_sys" /I "../../src_mfd" /I "../../src_moon" /I "../../src_lm" /I "../../src_csm" /I "../../src_launch" /I "../../src_landing" /I "../../src_saturn" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"PanelSDK.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"PanelSDK.lib"

!ENDIF 

# Begin Target

# Name "PanelSDK - Win32 Release"
# Name "PanelSDK - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\oapichar.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\Vectors.cpp
# End Source File
# End Group
# Begin Group "Internals"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\Internals\esysparse.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\Internals\Esystems.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\Internals\Hsysparse.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\Internals\Hsystems.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\Internals\Thermal.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\build.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\intruments.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\Panel.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\PanelSDK.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\vsmgmt.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\BUILD.H
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\Internals\Esystems.h
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\Internals\Hsystems.h
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\instruments.h
# End Source File
# Begin Source File

SOURCE=.\..\..\src_aux\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\PanelSDK.h
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\Internals\Thermal.h
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\Vectors.h
# End Source File
# Begin Source File

SOURCE=.\..\..\src_sys\PanelSDK\vsmgmt.h
# End Source File
# End Group
# End Target
# End Project
