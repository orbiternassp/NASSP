; NASSP Installer Script
; Author: harveywalker500
; FOR ANY ISSUES OR BUGS, OPEN AN ISSUE ON GITHUB OR CONTACT ME

; Includes MUI2
!include "MUI2.nsh"
!include "LogicLib.nsh"

; Custom Defines for information
!define NAME "Project Apollo - NASSP"
!define VERSION "8.0.0 Beta-Orbiter2016-2234"
!define SLUG "${NAME} v${VERSION}"

; Define installer name and output
name "${NAME}"
OutFile "${SLUG}.exe"
InstallDir "C:\OrbiterBeta"

; Define MUI2 settings
!define MUI_ABORTWARNING
!define MUI_ICON "icon.ico"
!define MUI_UNICON "icon.ico"

; Installer pages
!define MUI_WELCOMEFINISHPAGE_BITMAP "apolloLogo.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "apolloLogo.bmp"
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!define MUI_PAGE_CUSTOMFUNCTION_PRE excludeInstaller
!insertmacro MUI_PAGE_LICENSE "..\..\..\NASSP-LICENSE.txt"
!define MUI_PAGE_CUSTOMFUNCTION_PRE excludeD3D9
!insertmacro MUI_PAGE_LICENSE "D3D9-LICENSE.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!define MUI_FINISHPAGE_TEXT "Thank you for installing NASSP! Please note, in order to have sound in Orbiter, you will have to install OrbiterSound, which is a third party addon. You can find instructions on how to install it in the config."
!define MUI_FINISHPAGE_RUN "$INSTDIR\Orbiter_ng.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Launch Orbiter"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\Doc\Project Apollo - NASSP\configureInstructions.txt"
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Open Configuration Instructions"
!insertmacro MUI_PAGE_FINISH

; Language files
!insertmacro MUI_LANGUAGE "English"

Section "Project Apollo - NASSP" SecMain
    SetOutPath $INSTDIR
    SetOverwrite ifnewer
    File /r "..\..\..\*.*"
    ; Exclude the installer_scripts folder
    RMDIR /r "$INSTDIR\Config\ProjectApollo\InstallerScripts"
SectionEnd

Section "D3D9 Client" SecD3D9
    SetOutPath $INSTDIR
    SetOverwrite ifnewer
    File /r "D3D9Client\*.*"
SectionEnd

; Exclude license pages if the section is not selected
Function excludeInstaller
    ${Unless} ${SectionIsSelected} ${SecMain}
        Abort
    ${EndUnless}
FunctionEnd

Function excludeD3D9
    ${Unless} ${SectionIsSelected} ${SecD3D9}
        Abort
    ${EndUnless}
FunctionEnd
