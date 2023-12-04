; --- Includes ---
!include "MUI2.nsh"

; Custom Defines for information
!define NAME "Project Apollo - NASSP"
!define APPFILE "NASSP Installer.exe"
!define VERSION "8.0.0 Beta-Orbiter2016-2173"
!define SLUG "${NAME} v${VERSION}"

; Define installer name and output
OutFile "${SLUG}.exe"
InstallDir "C:\OrbiterBeta"

; Define MUI2 settings
!define MUI_ABORTWARNING
!define MUI_ICON "icon.ico"
!define MUI_UNICON "icon.ico"

; Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\NASSP-LICENSE.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "$INSTDIR\Orbiter.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Launch Orbiter"
!insertmacro MUI_PAGE_FINISH

; Language files
!insertmacro MUI_LANGUAGE "English"

Section "Main Section" SecMain
    SetOutPath $INSTDIR
    SetOverwrite ifnewer
    File /r "..\*.*"
    ; Exclude the installer_scripts folder
    RMDIR /r "$INSTDIR\installer_scripts"
    RMDIR /r "$INSTDIR\.git"

SectionEnd