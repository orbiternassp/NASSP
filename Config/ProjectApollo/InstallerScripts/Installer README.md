# Installer
The installer for NASSP was developed by harveywalker500 on github. Any issues with the installer can be directed to me :)

End users do not need to run the following instructions, as the pre-compiled installer should be released alongside the release itself. This guide is for those who want to compile the installer themselves.

## Prerequisites
- NSIS 
- Release version of Project Apollo - NASSP

## Instructions for compiling
1. Ensure that the InstallerScripts folder is in `NASSP/Config/ProjectApollo` and that D3D9 client is inside it. *The installer will not compile successfully if it is anywhere else!*
2. Open NSIS and select "Compile NSIS scripts".
3. Open the script using the dialog box and select the `installer.nsi` script. It should start compiling immediately, and take a few minutes.
4. Once the script has finished compiling, the installer executable can be found in the same folder the script is. It can then be moved and run anywhere on your computer.
