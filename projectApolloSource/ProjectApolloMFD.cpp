/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2006 

  Project Apollo MFD

  Project Apollo is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Project Apollo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Project Apollo; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  See http://nassp.sourceforge.net/license/ for more details.

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.25  2008/09/14 00:41:47  bluedragon8144
  *	fixed state vector calculation
  *	
  *	Revision 1.24  2008/09/09 17:21:45  tschachim
  *	Bugfixes GetStateVector
  *	
  *	Revision 1.23  2008/08/06 23:08:21  bluedragon8144
  *	Replaced obliquity constant with Orbiter's function.
  *	
  *	Revision 1.22  2008/06/01 08:05:42  jasonims
  *	correcting a naughty mistake...   my i is integer initialized
  *	
  *	Revision 1.21  2008/05/26 20:04:57  bluedragon8144
  *	Added clock update function to telemetry window.
  *	Moved state vector update function to start just before the values are uploaded, rather than calculating during the check.
  *	
  *	Revision 1.20  2008/05/03 23:27:37  tschachim
  *	warnings fixed
  *	
  *	Revision 1.19  2008/04/23 18:51:56  bluedragon8144
  *	Added telemetry window and vessel support for State Vector.  Cleaned up display.
  *	
  *	Revision 1.18  2008/04/18 21:28:15  bluedragon8144
  *	Added State Vector Update (beta) to GNC window
  *	
  *	Revision 1.17  2008/04/11 11:49:27  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *
  *	Revision 1.16  2007/12/30 15:14:21  lassombra
  *	Created new "dummy" mfd in the project apollo mfd plugin.  This plugin will now spawn both a checklist mfd and a regular mfd.  The Checklist MFD will be modified
  *	 to interface to the checklistpanel class.
  *	
  *	Revision 1.15  2007/12/19 02:54:26  lassombra
  *	Added function to move debug strings to the MFD.
  *	
  *	MFD can display, freeze, or clear the debug strings.
  *	
  *	Use (Saturn *)saturn->debugString() just as you would use oapiDebugString.
  *	
  *	oapiDebugString can be used to output important alerts to users still, but should remain clean otherwise.
  *	
  *	Also, redirected all the debug statements from the Telecom to the mfd.
  *	
  *	Revision 1.14  2007/12/17 15:09:13  lassombra
  *	Added function to alert Project Apollo's Configurator/MFD as to the identity of the Socket we are opening when we start the sim.  As a result, the configurator can now shutdown the socket when we close to launchpad, fixing the "TELECOM: BIND() FAILED" problem.  Also now, we have access to the socket in the MFD and can use it for various debugging purposes.
  *	
  *	Revision 1.13  2007/12/16 00:47:53  lassombra
  *	Removed ability to use buttons/keystrokes when using any ship but a saturn.
  *	
  *	Revision 1.12  2007/12/15 19:48:26  lassombra
  *	Added functionality to allow ProjectApollo MFD to get mission time from the Crawler as well as the Saturn.  The Crawler actually extracts the mission time from the Saturn, no updates to scenario files needed.
  *	
  *	Revision 1.11  2007/12/11 13:44:39  tschachim
  *	Bugfix, allow impulsive requests.
  *	
  *	Revision 1.10  2007/12/10 17:12:56  tschachim
  *	TLI burn fixes.
  *	ISS alarm in case the IMU is unpowered.
  *	
  *	Revision 1.9  2007/12/04 20:26:29  tschachim
  *	IMFD5 communication including a new TLI for the S-IVB IU.
  *	Additional CSM panels.
  *	
  *	Revision 1.8  2007/07/17 14:33:02  tschachim
  *	Added entry and post landing stuff.
  *	
  *	Revision 1.7  2007/06/06 15:02:10  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.6  2007/02/18 01:35:29  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.5  2006/12/19 15:55:55  tschachim
  *	ECS test stuff, bugfixes.
  *	
  *	Revision 1.4  2006/07/31 12:20:49  tschachim
  *	Bugfix
  *	
  *	Revision 1.3  2006/06/27 12:08:58  tschachim
  *	Bugfix
  *	
  *	Revision 1.2  2006/06/27 11:34:25  tschachim
  *	Added status screen.
  *	
  *	Revision 1.1  2006/06/23 11:56:48  tschachim
  *	New Project Apollo MFD for TLI burn control.
  *	
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"

#include "math.h"
#include "windows.h"
#include "nasspsound.h"
#include "OrbiterSoundSDK35.h"
#include "soundlib.h"
#include "tracer.h"
#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"
#include "saturn.h"
#include "Crawler.h"
#include "papi.h"
#include <stdio.h>

#include "MFDResource.h"
#include "ProjectApolloMFD.h"


// ==============================================================
// Global variables

HINSTANCE g_hDLL;
int g_MFDmode; // identifier for new MFD mode


#define PROG_NONE		0
#define PROG_GNC		1
#define PROG_ECS		2
#define PROG_IMFD		3
#define PROG_IMFDTLI	4
#define PROG_TELE		5
//This program displays info on the current telcom socket.  For debugging only.
#define PROG_SOCK		6		
#define PROG_DEBUG		7

#define PROGSTATE_NONE				0
#define PROGSTATE_TLI_START			1
#define PROGSTATE_TLI_REQUESTING	2
#define PROGSTATE_TLI_WAITING		3
#define PROGSTATE_TLI_RUNNING		4
#define PROGSTATE_TLI_ERROR			5

#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

// Time to ejection when the IU is programmed
#define IUSTARTTIME 900

static struct {  // global data storage
	int prog;	
	int progState;  
	Saturn *progVessel;
	double nextRequestTime;

	IMFD_BURN_DATA burnData;
	bool isRequesting;
	bool isRequestingManually;
	double requestMjd;
	char *errorMessage;
	int emem[17];
	int connStatus;
	int statevectorReady;
	int updateclockReady;
	int uplinkState;
	double missionTime;
	OBJHANDLE planet;
	VESSEL *vessel;
} g_Data;

static WSADATA wsaData;
static SOCKET m_socket;				
static sockaddr_in clientService;
static SOCKET close_Socket = INVALID_SOCKET;
static char debugString[100];
static char debugStringBuffer[100];
static char debugWinsock[100];

void ProjectApolloMFDopcDLLInit (HINSTANCE hDLL)
{
	static char *name = "Project Apollo";      // MFD mode name
	MFDMODESPEC spec;
	spec.name = name;
	spec.key = OAPI_KEY_A;					   // MFD mode selection key is obsolete
	spec.msgproc = ProjectApolloMFD::MsgProc;  // MFD mode callback function

	// Register the new MFD mode with Orbiter
	g_MFDmode = oapiRegisterMFDMode (spec);
	g_hDLL = hDLL;

	g_Data.prog = PROG_NONE;
	g_Data.progState = 0;
	g_Data.progVessel = NULL;
	g_Data.nextRequestTime = 0;

	ZeroMemory(&g_Data.burnData, sizeof(IMFD_BURN_DATA));
	g_Data.isRequesting = false;
	g_Data.isRequestingManually = false;
	g_Data.requestMjd = 0;
	g_Data.errorMessage = "";
	g_Data.connStatus = 0;
	g_Data.statevectorReady = 0;
	g_Data.updateclockReady = 0;
	g_Data.uplinkState = 0;
	//Init Emem
	for(int i = 0; i < 17; i++)
		g_Data.emem[i] = 0;
	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR ) {
		sprintf(debugWinsock,"Error at WSAStartup()");
	}
	else {
		sprintf(debugWinsock,"Disconnected");
	}
}

void ProjectApolloMFDopcDLLExit (HINSTANCE hDLL)
{
	// Unregister the custom MFD mode when the module is unloaded
	oapiUnregisterMFDMode (g_MFDmode);
}

void StartIMFDRequest() {

	g_Data.isRequesting = true;
	if (!g_Data.progVessel->GetIMFDClient()->IsBurnDataRequesting())
		g_Data.progVessel->GetIMFDClient()->StartBurnDataRequests();
}

void StopIMFDRequest() {

	g_Data.isRequesting = false;
	if (!g_Data.isRequestingManually)
		g_Data.progVessel->GetIMFDClient()->StopBurnDataRequests();
}

void send_agc_key(char key)
	{
		int bytesXmit = SOCKET_ERROR;
		unsigned char cmdbuf[4];
		cmdbuf[0] = 043; // VA,SA
		switch(key) {
			case 'V': // 11-000-101 11-010-001										
				cmdbuf[1] = 0305;
				cmdbuf[2] = 0321;
				break;
			case 'N': // 11-111-100 00-011-111
				cmdbuf[1] = 0374;
				cmdbuf[2] = 0037;
				break;
			case 'E': // 11-110-000 01-111-100
				cmdbuf[1] = 0360;
				cmdbuf[2] = 0174;
				break;
			case 'R': // 11-001-001 10-110-010
				cmdbuf[1] = 0311;
				cmdbuf[2] = 0262;
				break;
			case 'C': // 11-111-000 00-111-110
				cmdbuf[1] = 0370;
				cmdbuf[2] = 0076;
				break;
			case 'K': // 11-100-100 11-011-001
				cmdbuf[1] = 0344;
				cmdbuf[2] = 0331;
				break;
			case '+': // 11-101-000 10-111-010
				cmdbuf[1] = 0350;
				cmdbuf[2] = 0272;
				break;
			case '-': // 11-101-100 10-011-011
				cmdbuf[1] = 0354;
				cmdbuf[2] = 0233;
				break;
			case '1': // 10-000-111 11-000-001
				cmdbuf[1] = 0207;
				cmdbuf[2] = 0301;
				break;
			case '2': // 10-001-011 10-100-010
				cmdbuf[1] = 0213;
				cmdbuf[2] = 0242;
				break;
			case '3': // 10-001-111 10-000-011
				cmdbuf[1] = 0217;
				cmdbuf[2] = 0203;
				break;
			case '4': // 10-010-011 01-100-100
				cmdbuf[1] = 0223;
				cmdbuf[2] = 0144;
				break;
			case '5': // 10-010-111 01-000-101
				cmdbuf[1] = 0227;
				cmdbuf[2] = 0105;
				break; 
			case '6': // 10-011-011 00-100-110
				cmdbuf[1] = 0233;
				cmdbuf[2] = 0046;
				break;
			case '7': // 10-011-111 00-000-111
				cmdbuf[1] = 0237;
				cmdbuf[2] = 0007;
				break;
			case '8': // 10-100-010 11-101-000
				cmdbuf[1] = 0242;
				cmdbuf[2] = 0350;
				break;
			case '9': // 10-100-110 11-001-001
				cmdbuf[1] = 0246;
				cmdbuf[2] = 0311;
				break;
			case '0': // 11-000-001 11-110-000
				cmdbuf[1] = 0301;
				cmdbuf[2] = 0360;
				break;
			}
 			bytesXmit = send(m_socket,(char *)cmdbuf,3,0);			
		}
void uplink_word(char * data)
{
	int i;
	for(i = 5; i > (int)strlen(data); i--) {
		send_agc_key('0');
	}
	for(i = 0; i < (int)strlen(data); i++) {
		send_agc_key(data[i]);
	}
	send_agc_key('E');
}

void UplinkStateVector(double simt)
{
	int bytesRecv = SOCKET_ERROR;
	char addr[256];
	char buffer[8];
	if(g_Data.connStatus == 0) {
		m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );	
		if ( m_socket == INVALID_SOCKET ) {
			sprintf(debugWinsock,"Error at socket(): %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(addr, "127.0.0.1");
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr);
		clientService.sin_port = htons( 14242 );
		if (connect( m_socket, (SOCKADDR*) &clientService, sizeof(clientService)) == SOCKET_ERROR) {
			sprintf(debugWinsock,"Failed to connect, Error %ld",WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "Connected");
		g_Data.connStatus = 1;
		g_Data.uplinkState = 0;
		send_agc_key('V');
		send_agc_key('7');
		send_agc_key('1');
		send_agc_key('E');
		g_Data.missionTime = simt;
	}
	else {
		if(simt > g_Data.missionTime + 1.0) {
			if(g_Data.uplinkState < 17) {
				sprintf(buffer, "%ld", g_Data.emem[g_Data.uplinkState]);
				uplink_word(buffer);
				g_Data.missionTime = simt;
				g_Data.uplinkState++;
			}
			else if(g_Data.uplinkState == 17){
				send_agc_key('V');
				send_agc_key('3');
				send_agc_key('3');
				send_agc_key('E');
				g_Data.missionTime = simt;
				g_Data.uplinkState++;
			}
			else {
				sprintf(debugWinsock, "Disconnected");
				g_Data.uplinkState = 0;
				
				// TODO TEST
				g_Data.statevectorReady = 0;
				//g_Data.statevectorReady = 1;
				
				g_Data.connStatus = 0;
				closesocket(m_socket);
			}
		}
	}
}

void UpdateClock(double simt)
{
	int bytesRecv = SOCKET_ERROR;
	char addr[256];
	char buffer[8];
	if(g_Data.connStatus == 0) {
		m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );	
		if ( m_socket == INVALID_SOCKET ) {
			sprintf(debugWinsock,"Error at socket(): %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(addr, "127.0.0.1");
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr);
		clientService.sin_port = htons( 14242 );
		if (connect( m_socket, (SOCKADDR*) &clientService, sizeof(clientService)) == SOCKET_ERROR) {
			sprintf(debugWinsock,"Failed to connect, Error %ld",WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "Connected");
		g_Data.connStatus = 1;
		g_Data.uplinkState = 0;
		send_agc_key('V');
		send_agc_key('2');
		send_agc_key('5');
		send_agc_key('N');
		send_agc_key('3');
		send_agc_key('6');
		send_agc_key('E');
		g_Data.missionTime = simt;
	}
	else {
		if(simt > g_Data.missionTime + 1.0) {
			if(g_Data.uplinkState == 0) {
				char sign = '+';
				double mt = 0;
				mt = g_Data.progVessel->GetMissionTime();
				mt += 1.0;
				int secs = abs((int) mt);
				int hours = (secs / 3600);
				secs -= (hours * 3600);
				int minutes = (secs / 60);
				secs -= 60 * minutes;
				secs *= 100;
				if (simt < 0)
					sign = '-';
				send_agc_key(sign);
				sprintf(buffer, "%ld", hours);
				uplink_word(buffer);
				send_agc_key(sign);
				sprintf(buffer, "%ld", minutes);
				uplink_word(buffer);
				send_agc_key(sign);
				sprintf(buffer, "%ld", secs);
				uplink_word(buffer);
				g_Data.missionTime = simt;
				g_Data.uplinkState++;
			}
			else if(g_Data.uplinkState == 1) {
				send_agc_key('V');
				send_agc_key('1');
				send_agc_key('6');
				send_agc_key('N');
				send_agc_key('6');
				send_agc_key('5');
				send_agc_key('E');
				g_Data.missionTime = simt;
				g_Data.uplinkState++;
			}
			else if(g_Data.uplinkState == 4) {
				send_agc_key('V');
				send_agc_key('3');
				send_agc_key('7');
				send_agc_key('E');
				send_agc_key('0');
				send_agc_key('0');
				send_agc_key('E');
				g_Data.missionTime = simt;
				g_Data.uplinkState++;
			}
			else if(g_Data.uplinkState == 5) {
				sprintf(debugWinsock, "Disconnected");
				g_Data.uplinkState = 0;
				g_Data.updateclockReady = 0;
				g_Data.connStatus = 0;
				closesocket(m_socket);
			}
			else {
				g_Data.missionTime = simt;
				g_Data.uplinkState++;
			}
		}
	}
}

void ProjectApolloMFDopcTimestep (double simt, double simdt, double mjd)
{

	// Recover if MFD was closed and TLI is in progress
	if (g_Data.progVessel) {
		if (g_Data.progVessel->GetIU()->IsTLIInProgress()) {
			if (g_Data.prog != PROG_IMFDTLI || g_Data.progState != PROGSTATE_TLI_RUNNING) {
				g_Data.prog = PROG_IMFDTLI;
				g_Data.progState = PROGSTATE_TLI_RUNNING;
				if (!g_Data.isRequesting) {
					StartIMFDRequest();
					g_Data.requestMjd = mjd;
				}
			}
   		}
	}
	if (g_Data.statevectorReady == 2) {
		UplinkStateVector(simt);
	}
	if (g_Data.updateclockReady == 2) {
		UpdateClock(simt);
	}
	
	if (g_Data.prog == PROG_IMFDTLI) {
		switch (g_Data.progState) {
		case PROGSTATE_TLI_START:
			StartIMFDRequest();
			g_Data.requestMjd = mjd;
			g_Data.progState = PROGSTATE_TLI_REQUESTING;
			break;

		case PROGSTATE_TLI_REQUESTING:
			if (g_Data.progVessel->GetIMFDClient()->IsBurnDataValid()) {
				g_Data.burnData = g_Data.progVessel->GetIMFDClient()->GetBurnData();
				StopIMFDRequest();
				if (!g_Data.burnData.p30mode || g_Data.burnData.impulsive) {
					g_Data.errorMessage ="IMFD not in Off-Axis, P30 Mode";
					g_Data.progState = PROGSTATE_TLI_ERROR;
				} else if ((g_Data.burnData.IgnMJD - mjd) * 24. * 3600. <= IUSTARTTIME) {
					g_Data.errorMessage ="Time to burn smaller than 900s";
					g_Data.progState = PROGSTATE_TLI_ERROR;
				} else {
					g_Data.progState = PROGSTATE_TLI_WAITING;
				}
			} else if ((mjd - g_Data.requestMjd) * 24. * 3600. > 10) {
				StopIMFDRequest();
				g_Data.errorMessage ="Request timeout";
				g_Data.progState = PROGSTATE_TLI_ERROR;
			}
			break;

		case PROGSTATE_TLI_WAITING:
			if ((g_Data.burnData.IgnMJD - mjd) * 24. * 3600. <= IUSTARTTIME) {
				if (g_Data.progVessel->GetIU()->StartTLIBurn(g_Data.burnData._RIgn, g_Data.burnData._VIgn, g_Data.burnData._dV_LVLH, g_Data.burnData.IgnMJD)) {
					g_Data.nextRequestTime = 500;
					g_Data.progState = PROGSTATE_TLI_RUNNING;
				} else {
					g_Data.errorMessage ="S-IVB start error";
					g_Data.progState = PROGSTATE_TLI_ERROR;
				}
			}
			break;

		case PROGSTATE_TLI_RUNNING:
			if (!g_Data.isRequesting) {
				if (g_Data.nextRequestTime > 0 && (g_Data.burnData.IgnMJD - mjd) * 24. * 3600. <= g_Data.nextRequestTime) {
					StartIMFDRequest();
					g_Data.requestMjd = mjd;
				}
			} else {
				if (g_Data.progVessel->GetIMFDClient()->IsBurnDataValid()) {
					g_Data.burnData = g_Data.progVessel->GetIMFDClient()->GetBurnData();
					StopIMFDRequest();
					if (g_Data.burnData.p30mode && !g_Data.burnData.impulsive && g_Data.burnData.IgnMJD > mjd) {
						g_Data.progVessel->GetIU()->StartTLIBurn(g_Data.burnData._RIgn, g_Data.burnData._VIgn, g_Data.burnData._dV_LVLH, g_Data.burnData.IgnMJD);
					}
				} else if ((mjd - g_Data.requestMjd) * 24. * 3600. > 2) {
					StopIMFDRequest();
				}
				if (!g_Data.isRequesting) {
					g_Data.nextRequestTime -= 100;
					if (g_Data.nextRequestTime <= 10) 
						g_Data.nextRequestTime = 0;
					else if (g_Data.nextRequestTime <= 100) 
						g_Data.nextRequestTime = 4;
				}
			}
			if (!g_Data.progVessel->GetIU()->IsTLIInProgress()) {
				g_Data.prog = PROG_NONE;
				g_Data.progState = PROGSTATE_NONE;
   			}
		}
	}
}

// ==============================================================
// MFD class implementation

// Constructor
ProjectApolloMFD::ProjectApolloMFD (DWORD w, DWORD h, VESSEL *vessel) : MFD (w, h, vessel)

{
	saturn = NULL;
	crawler = NULL;
	width = w;
	height = h;
	hBmpLogo = LoadBitmap(g_hDLL, MAKEINTRESOURCE (IDB_LOGO));
	screen = PROG_NONE;
	debug_frozen = false;
	char buffer[8];

	//We need to find out what type of vessel it is, so we check for the class name.
	//Saturns have different functions than Crawlers.  But we have methods for both.
	if (!stricmp(vessel->GetClassName(), "ProjectApollo\\Saturn5") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo/Saturn5") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo\\Saturn1b") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo/Saturn1b")) {
		saturn = (Saturn *)vessel;
		g_Data.progVessel = saturn;
		g_Data.vessel = vessel;
		oapiGetObjectName(saturn->GetGravityRef(), buffer, 8);
		if(strcmp(buffer,"Earth") == 0 || strcmp(buffer,"Moon") == 0 )
			g_Data.planet = saturn->GetGravityRef();
		else
			g_Data.planet = oapiGetGbodyByName("Earth");
	}
	else if (!stricmp(vessel->GetClassName(), "ProjectApollo\\Crawler") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo/Crawler"))  {
			crawler = (Crawler *)vessel;
			g_Data.planet = crawler->GetGravityRef();
	}
}

// Destructor
ProjectApolloMFD::~ProjectApolloMFD ()
{
	// Add MFD cleanup code here
}

// Return button labels
char *ProjectApolloMFD::ButtonLabel (int bt)
{
	// The labels for the buttons used by our MFD mode
	//Additional button added to labelNone for testing socket work, be SURE to remove it.
	//Additional button added at the bottom right of none for the debug string.
	static char *labelNone[12] = {"GNC", "ECS", "IMFD", "TELE","SOCK","","","","","","","DBG"};
	static char *labelGNC[6] = {"BCK", "DMP"};
	static char *labelECS[4] = {"BCK", "CRW", "PRM", "SEC"};
	static char *labelIMFDTliStop[3] = {"BCK", "REQ", "SIVB"};
	static char *labelIMFDTliRun[3] = {"BCK", "REQ", "STP"};
	static char *labelTELE[6] = {"BCK", "SV", "CLK", "", "SRC", "REF"};
	static char *labelSOCK[1] = {"BCK"};	
	static char *labelDEBUG[12] = {"","","","","","","","","","CLR","FRZ","BCK"};

	//If we are working with an unsupported vehicle, we don't want to return any button labels.
	if (!saturn) {
		return 0;
	}
	if (screen == PROG_GNC) {
		return (bt < 2 ? labelGNC[bt] : 0);
	}
	else if (screen == PROG_ECS) {
		return (bt < 4 ? labelECS[bt] : 0);
	}
	else if (screen == PROG_IMFD) {
		if (g_Data.progState == PROGSTATE_NONE)
			return (bt < 3 ? labelIMFDTliStop[bt] : 0);
		else
			return (bt < 3 ? labelIMFDTliRun[bt] : 0);
	}
	else if(screen == PROG_TELE) {
		return (bt < 6 ? labelTELE[bt] : 0);
	}
	else if (screen == PROG_SOCK) {
		return (bt < 1 ? labelSOCK[bt] : 0);
	}
	else if (screen == PROG_DEBUG) {
		return (bt < 12 ? labelDEBUG[bt] : 0);
	}
	return (bt < 12 ? labelNone[bt] : 0);
}

// Return button menus
int ProjectApolloMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	// The menu descriptions for the buttons used by our MFD mode
	static const MFDBUTTONMENU mnuNone[12] = {
		{"Guidance, Navigation & Control", 0, 'G'},
		{"Environmental Control System", 0, 'E'},
		{"IMFD Support", 0, 'I'},
		{"Telemetry",0,'T'},
		{"Socket info", 0, 'S'},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{"Debug String",0,'D'}
	};
	static const MFDBUTTONMENU mnuGNC[2] = {
		{"Back", 0, 'B'},
		{"Virtual AGC core dump", 0, 'D'}
	};
	static const MFDBUTTONMENU mnuECS[4] = {
		{"Back", 0, 'B'},
		{"Crew number", 0, 'C'},
		{"Primary coolant loop test heating", 0, 'P'},
		{"Secondary coolant loop test heating", 0, 'S'}
	};
	static const MFDBUTTONMENU mnuIMFDTliStop[3] = {
		{"Back", 0, 'B'},
		{"Toggle burn data requests", 0, 'R'},
		{"Start S-IVB burn", 0, 'S'}
	};
	static const MFDBUTTONMENU mnuIMFDTliRun[3] = {
		{"Back", 0, 'B'},
		{"Toggle burn data requests", 0, 'R'},
		{"Start S-IVB burn", 0, 'S'}
	};
	static const MFDBUTTONMENU mnuTELE[6] = {
		{"Back", 0, 'B'},
		{"State Vector Update", 0, 'U'},
		{"Clock Update",0,'C'},
		{0,0,0},
		{"Source",0,'S'},
		{"Reference Body", 0, 'R'}
	};
	//This menu set is just for the Socket program, remove before release.
	static const MFDBUTTONMENU mnuSOCK[1] = {
		{"Back", 0, 'B'}
	};
	static const MFDBUTTONMENU mnuDebug[12] = {
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{"Clear the Debug Line",0,'C'},
		{"Freeze debug line",0,'F'},
		{"Back",0,'B'}
	};
	// We don't want to display a menu if we are in an unsupported vessel.
	if (!saturn) {
		menu = 0;
		return 0;
	}

	if (screen == PROG_GNC) {
		if (menu) *menu = mnuGNC;
		return 6; 
	} else if (screen == PROG_ECS) {
		if (menu) *menu = mnuECS;
		return 4; 
	} else if (screen == PROG_IMFD) {
		if (g_Data.progState == PROGSTATE_NONE) {
			if (menu) *menu = mnuIMFDTliStop;
			return 3;
		} else {
			if (menu) *menu = mnuIMFDTliRun;
			return 3;
		}
	}	
	else if (screen == PROG_TELE) {
		if (menu) *menu = mnuTELE;
		return 6;
	}
	else if (screen == PROG_SOCK)
	{
		if (menu) *menu = mnuSOCK;
		return 1;
	}
	else if (screen == PROG_DEBUG)
	{
		if (menu) *menu = mnuDebug;
		return 12;
	}
	else {
		if (menu) *menu = mnuNone;
		return 12; 
	}
}

bool ProjectApolloMFD::ConsumeKeyBuffered (DWORD key) 
{
	//We don't want to accept keyboard commands from the wrong vessels.
	if (!saturn)
		return false;

	if (screen == PROG_NONE) {
		if (key == OAPI_KEY_G) {
			screen = PROG_GNC;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_E) {
			screen = PROG_ECS;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_I) {
			screen = PROG_IMFD;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_T) {
			screen = PROG_TELE;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_S) {
			screen = PROG_SOCK;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_D) {
			screen = PROG_DEBUG;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
	} else if (screen == PROG_GNC) {
		if (key == OAPI_KEY_B) {
			screen = PROG_NONE;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_D) {
			if (saturn)
				saturn->VirtualAGCCoreDump();
			return true;
		} 		
	} else if (screen == PROG_TELE) {
		if (key == OAPI_KEY_B) {
			if(g_Data.statevectorReady == 0 && g_Data.updateclockReady == 0) {
				screen = PROG_NONE;
				InvalidateDisplay();
				InvalidateButtons();
			} else {
				if(g_Data.statevectorReady == 1)
					g_Data.statevectorReady -= 1;
				if(g_Data.updateclockReady == 1)
					g_Data.updateclockReady -= 1;
			}
			return true;
		} else if (key == OAPI_KEY_U) {
			if (saturn) {
				if (g_Data.statevectorReady == 0 && g_Data.updateclockReady == 0) {
					// TODO TEST
					// GetStateVector();
					
					g_Data.statevectorReady = 1;
				}
				else if (g_Data.statevectorReady == 1) {				
					GetStateVector();
					g_Data.statevectorReady = 2;
				}
			}
			return true;
		} else if (key == OAPI_KEY_C) {
			if (saturn) {
				if (g_Data.updateclockReady == 0 && g_Data.statevectorReady == 0) {
					g_Data.updateclockReady = 1;
				}
				else if (g_Data.updateclockReady == 1) {
					g_Data.updateclockReady = 2;
				}
			}
			return true;
		} else if (key == OAPI_KEY_S) {
			if(g_Data.statevectorReady == 0) {
				bool SourceInput (void *id, char *str, void *data);
				oapiOpenInputBox("Set Source", SourceInput, 0, 20, (void*)this);
			}
			return true;
		} else if (key == OAPI_KEY_R) {
			if(g_Data.statevectorReady == 0) {
				bool ReferencePlanetInput (void *id, char *str, void *data);
				oapiOpenInputBox("Set Reference", ReferencePlanetInput, 0, 20, (void*)this);
			}
			return true;
		}
	} else if (screen == PROG_ECS) {
		if (key == OAPI_KEY_B) {
			screen = PROG_NONE;
			InvalidateDisplay();
			InvalidateButtons();
			return true;

		} else if (key == OAPI_KEY_C) {
			bool CrewNumberInput (void *id, char *str, void *data);
			oapiOpenInputBox ("Crew number [0-3]:", CrewNumberInput, 0, 20, (void*)this);			
			return true;
		
		} else if (key == OAPI_KEY_P) {
			bool PrimECSTestHeaterPowerInput (void *id, char *str, void *data);
			oapiOpenInputBox ("Primary coolant loop test heater power [-3000 to 3000 Watt]:", PrimECSTestHeaterPowerInput, 0, 20, (void*)this);			
			return true;

		} else if (key == OAPI_KEY_S) {
			bool SecECSTestHeaterPowerInput (void *id, char *str, void *data);
			oapiOpenInputBox ("Secondary coolant loop test heater power [-3000 to 3000 Watt]:", SecECSTestHeaterPowerInput, 0, 20, (void*)this);			
			return true;
		}
	} else if (screen == PROG_IMFD) {
		if (key == OAPI_KEY_B) {
			screen = PROG_NONE;
			InvalidateDisplay();
			InvalidateButtons();
			return true;

		} else if (key == OAPI_KEY_R && !g_Data.isRequestingManually) {						
			if (!saturn->GetIMFDClient()->IsBurnDataRequesting()) {
				saturn->GetIMFDClient()->StartBurnDataRequests();
			}
			g_Data.isRequestingManually = true;

		} else if (key == OAPI_KEY_R && g_Data.isRequestingManually) {
			if (!g_Data.isRequesting) {
				saturn->GetIMFDClient()->StopBurnDataRequests();
			}
			g_Data.isRequestingManually = false;
		
		} else if (key == OAPI_KEY_S && g_Data.progState == PROGSTATE_NONE) {
			g_Data.prog = PROG_IMFDTLI;
			g_Data.progState = PROGSTATE_TLI_START;
			g_Data.progVessel = saturn;
			InvalidateDisplay();
			InvalidateButtons();
			return true;

		} else if (key == OAPI_KEY_S && (g_Data.progState == PROGSTATE_TLI_WAITING || g_Data.progState == PROGSTATE_TLI_ERROR)) {
			g_Data.prog = PROG_NONE;
			g_Data.progState = PROGSTATE_NONE;
			g_Data.errorMessage = "";
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} 
	}
	//This program is for the socket, remove before release.
	else if (screen == PROG_SOCK)
	{
		if (key == OAPI_KEY_B)
		{
			screen = PROG_NONE;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
	}
	else if (screen == PROG_DEBUG)
	{
		if (key == OAPI_KEY_B)
		{
			screen = PROG_NONE;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		else if (key == OAPI_KEY_F)
		{
			if (debug_frozen)
				debug_frozen = false;
			else
				debug_frozen = true;
			return true;
		}
		else if (key == OAPI_KEY_C)
		{
			sprintf(debugString,"");
			return true;
		}
	}
	return false;
}

bool ProjectApolloMFD::ConsumeButton (int bt, int event)
{
	//We don't have to implement a test for the correct vessel here, as it checks this already in the consume key method, which we call anyways.
	//We only want to accept left mouse button clicks.
	if (!(event & PANEL_MOUSE_LBDOWN)) return false;

	static const DWORD btkeyNone[12] = { OAPI_KEY_G, OAPI_KEY_E, OAPI_KEY_I, OAPI_KEY_T, OAPI_KEY_S, 0, 0, 0, 0, 0, 0, OAPI_KEY_D };
	static const DWORD btkeyGNC[2] = { OAPI_KEY_B, OAPI_KEY_D };
	static const DWORD btkeyECS[4] = { OAPI_KEY_B, OAPI_KEY_C, OAPI_KEY_P, OAPI_KEY_S };
	static const DWORD btkeyIMFD[3] = { OAPI_KEY_B, OAPI_KEY_R, OAPI_KEY_S };
	static const DWORD btkeyTELE[6] = { OAPI_KEY_B, OAPI_KEY_U, OAPI_KEY_C, 0, OAPI_KEY_S, OAPI_KEY_R };
	static const DWORD btkeySock[1] = { OAPI_KEY_B };	
	static const DWORD btkeyDEBUG[12] = { 0,0,0,0,0,0,0,0,0,OAPI_KEY_C,OAPI_KEY_F,OAPI_KEY_B };

	if (screen == PROG_GNC) {
		if (bt < 2) return ConsumeKeyBuffered (btkeyGNC[bt]);
	} else if (screen == PROG_ECS) {
		if (bt < 4) return ConsumeKeyBuffered (btkeyECS[bt]);
	} else if (screen == PROG_IMFD) {
		if (bt < 3) return ConsumeKeyBuffered (btkeyIMFD[bt]);		
	} else if (screen == PROG_TELE) {
		if (bt < 6) return ConsumeKeyBuffered (btkeyTELE[bt]);
	}
	// This program is the socket data.  Remove before release.
	else if (screen == PROG_SOCK)
	{
		if (bt < 1) return ConsumeKeyBuffered (btkeySock[bt]);
	}
	else if (screen == PROG_DEBUG)
	{
		if (bt < 12) return ConsumeKeyBuffered (btkeyDEBUG[bt]);
	}
	else {		
		if (bt < 12) return ConsumeKeyBuffered (btkeyNone[bt]);
	}
	return false;
}

// Repaint the MFD
void ProjectApolloMFD::Update (HDC hDC)
{
	char buffer[100];

	HDC hDCTemp = CreateCompatibleDC(hDC);
	HBITMAP hBmpTemp = (HBITMAP) SelectObject(hDCTemp, hBmpLogo);
	StretchBlt(hDC, 1, 1, width - 2, height - 2, hDCTemp, 0, 0, 256, 256, SRCCOPY);
	DeleteObject(hBmpTemp);
	DeleteDC(hDCTemp);

	// Draws the MFD title
	Title (hDC, "Project Apollo");

	SelectDefaultFont(hDC, 0);
	SetBkMode (hDC, TRANSPARENT);
	SetTextAlign (hDC, TA_CENTER);

	if (!saturn) {
		SetTextColor (hDC, RGB(255, 0, 0));
		TextOut(hDC, width / 2, (int) (height * 0.5), "Unsupported vessel", 18);
		if (!crawler)
			return;
	}

	// Draw mission time
	SetTextColor (hDC, RGB(0, 255, 0));
	TextOut(hDC, width / 2, (int) (height * 0.1), "Ground Elapsed Time", 19);

	double mt = 0;
	if (!crawler)
		mt = saturn->GetMissionTime();
	else
		mt = crawler->GetMissionTime();
	int secs = abs((int) mt);
	int hours = (secs / 3600);
	secs -= (hours * 3600);
	int minutes = (secs / 60);
	secs -= 60 * minutes;
	if (mt < 0)
		sprintf(buffer, "-%d:%02d:%02d", hours, minutes, secs);
	else
		sprintf(buffer, "%d:%02d:%02d", hours, minutes, secs);
	TextOut(hDC, width / 2, (int) (height * 0.15), buffer, strlen(buffer));
	//If this is the crawler and not the actual Saturn, do NOTHING else!
	if (!saturn)
		return;

	SelectDefaultPen(hDC, 1);
	MoveToEx (hDC, (int) (width * 0.05), (int) (height * 0.25), 0);
	LineTo (hDC, (int) (width * 0.95), (int) (height * 0.25));

	// Draw GNC
	if (screen == PROG_GNC) {
		TextOut(hDC, width / 2, (int) (height * 0.3), "Guidance, Navigation & Control", 30);
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.4), "Velocity:", 9);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.45), "Vert. Velocity:", 15);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.5), "Altitude:", 9);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.6), "Apoapsis Alt.:", 14);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.65), "Periapsis Alt.:", 15);

		VECTOR3 vel, hvel;
		double vvel = 0, apDist, peDist;
		OBJHANDLE planet = saturn->GetGravityRef();
		saturn->GetRelativeVel(planet, vel); 
		if (saturn->GetHorizonAirspeedVector(hvel)) {
			vvel = hvel.y * 3.2808399;
		}
		saturn->GetApDist(apDist);
		saturn->GetPeDist(peDist);
		apDist -= 6.373338e6;
		peDist -= 6.373338e6;

		SetTextAlign (hDC, TA_RIGHT);
		sprintf(buffer, "%.0lfft/s", length(vel) * 3.2808399);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.4), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfft/s", vvel);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.45), buffer, strlen(buffer));
		sprintf(buffer, "%.1lfnm", saturn->GetAltitude() * 0.000539957);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.5), buffer, strlen(buffer));
		sprintf(buffer, "%.1lfnm", apDist * 0.000539957);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.6), buffer, strlen(buffer));
		sprintf(buffer, "%.1lfnm", peDist * 0.000539957);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.65), buffer, strlen(buffer));
	//Draw Socket details.
	}
	else if (screen == PROG_SOCK) {
		TextOut(hDC, width / 2, (int) (height * 0.3), "Socket details", 14);
		sprintf(buffer, "Socket: %i", close_Socket);
		TextOut(hDC, width / 2, (int) (height * 0.4), buffer, strlen(buffer));
	}
	// Draw ECS
	else if (screen == PROG_ECS) {
		TextOut(hDC, width / 2, (int) (height * 0.3), "Environmental Control System", 28);
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.4), "Crew status:", 12);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.45),  "Crew number:", 12);

		ECSStatus ecs;
		saturn->GetECSStatus(ecs);

		SetTextAlign (hDC, TA_CENTER);
		if (ecs.crewStatus == ECS_CREWSTATUS_OK) {
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.4), "OK", 2);	
		} else if (ecs.crewStatus == ECS_CREWSTATUS_CRITICAL) {
			SetTextColor (hDC, RGB(255, 255, 0));
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.4), "CRITICAL", 8);	
			SetTextColor (hDC, RGB(0, 255, 0));
		} else {
			SetTextColor (hDC, RGB(255, 0, 0));
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.4), "DEAD", 4);	
			SetTextColor (hDC, RGB(0, 255, 0));
		}

		sprintf(buffer, "%d", ecs.crewNumber);
		TextOut(hDC, (int) (width * 0.7), (int) (height * 0.45), buffer, strlen(buffer)); 

		TextOut(hDC, (int) (width * 0.5), (int) (height * 0.525), "Glycol Coolant Loops", 20);
		TextOut(hDC, (int) (width * 0.6), (int) (height * 0.6), "Prim.", 5);
		TextOut(hDC, (int) (width * 0.8), (int) (height * 0.6), "Sec.", 4);

		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.6), "Heating:", 8);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.65), "Actual:", 7);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.7), "Test:", 5);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.8), "Total:", 6);

		SetTextAlign (hDC, TA_CENTER);
		sprintf(buffer, "%.0lfW", ecs.PrimECSHeating);
		TextOut(hDC, (int) (width * 0.6), (int) (height * 0.65), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfW", ecs.PrimECSTestHeating);
		TextOut(hDC, (int) (width * 0.6), (int) (height * 0.7), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfW", ecs.PrimECSHeating + ecs.PrimECSTestHeating);
		TextOut(hDC, (int) (width * 0.6), (int) (height * 0.8), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfW", ecs.SecECSHeating);
		TextOut(hDC, (int) (width * 0.8), (int) (height * 0.65), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfW", ecs.SecECSTestHeating);
		TextOut(hDC, (int) (width * 0.8), (int) (height * 0.7), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfW", ecs.SecECSHeating + ecs.SecECSTestHeating);
		TextOut(hDC, (int) (width * 0.8), (int) (height * 0.8), buffer, strlen(buffer));

		MoveToEx (hDC, (int) (width * 0.5), (int) (height * 0.775), 0);
		LineTo (hDC, (int) (width * 0.9), (int) (height * 0.775));
	// Draw IMFD
	} else if (screen == PROG_IMFD) {
		TextOut(hDC, width / 2, (int) (height * 0.3), "IMFD Burn Data", 14);
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.35), "Status:", 7);
		SetTextAlign (hDC, TA_CENTER);
		if (g_Data.isRequestingManually) {
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.35), "REQUESTING", 10);	
		} else {
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.35), "NONE", 4);
		}
		if (saturn->GetIMFDClient()->IsBurnDataValid() && g_Data.isRequestingManually) {
			IMFD_BURN_DATA bd = saturn->GetIMFDClient()->GetBurnData();
			if (bd.p30mode || bd.impulsive) {
				SetTextAlign (hDC, TA_LEFT);
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.45), "GET Ignition:", 13);

				SetTextAlign (hDC, TA_RIGHT);				
				mt = ((bd.IgnMJD - oapiGetSimMJD()) * 24. * 3600.) + saturn->GetMissionTime();
				secs = abs((int) mt);
				hours = (secs / 3600);
				secs -= (hours * 3600);
				minutes = (secs / 60);
				secs -= 60 * minutes;
				if (mt < 0)
					sprintf(buffer, "-%d:%02d:%02d", hours, minutes, secs);
				else
					sprintf(buffer, "%d:%02d:%02d", hours, minutes, secs);
				TextOut(hDC, (int) (width * 0.9), (int) (height * 0.45), buffer, strlen(buffer));

				SetTextAlign (hDC, TA_LEFT);
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.5), "dV x:", 5);
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.55), "dV y:", 5);
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.6), "dV z:", 5);

				SetTextAlign (hDC, TA_RIGHT);				
				sprintf(buffer, "%.1lf ft/s", bd._dV_LVLH.x * FPS);
				TextOut(hDC, (int) (width * 0.9), (int) (height * 0.5), buffer, strlen(buffer));
				sprintf(buffer, "%.1lf ft/s", bd._dV_LVLH.y * FPS);
				TextOut(hDC, (int) (width * 0.9), (int) (height * 0.55), buffer, strlen(buffer));
				sprintf(buffer, "%.1lf ft/s", bd._dV_LVLH.z * FPS);
				TextOut(hDC, (int) (width * 0.9), (int) (height * 0.6), buffer, strlen(buffer));
			} else {
				SetTextAlign (hDC, TA_CENTER);
				SetTextColor (hDC, RGB(255, 0, 0));
				TextOut(hDC, (int) (width * 0.5), (int) (height * 0.5), "IMFD not in P30 Mode", 20);
				SetTextColor (hDC, RGB(0, 255, 0));
			}
		}

		SetTextAlign (hDC, TA_CENTER);
		TextOut(hDC, width / 2, (int) (height * 0.7), "S-IVB Burn Program", 18);
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.75), "Status:", 7);

		SetTextAlign (hDC, TA_CENTER);
		if (g_Data.progState == PROGSTATE_NONE) {
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.75), "IDLE", 4);

		} else if (g_Data.progState == PROGSTATE_TLI_REQUESTING) {
			SetTextColor (hDC, RGB(255, 255, 0));
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.75), "REQUESTING", 10);
			SetTextColor (hDC, RGB(0, 255, 0));

		} else if (g_Data.progState == PROGSTATE_TLI_WAITING) {
			SetTextColor (hDC, RGB(255, 255, 0));
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.75), "WAITING", 7);
			SetTextColor (hDC, RGB(0, 255, 0));

		} else if (g_Data.progState == PROGSTATE_TLI_RUNNING) {
			SetTextColor (hDC, RGB(255, 255, 0));
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.75), "RUNNING", 7);
			SetTextColor (hDC, RGB(0, 255, 0));

		} else if (g_Data.progState == PROGSTATE_TLI_ERROR) {
			SetTextColor (hDC, RGB(255, 0, 0));
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.75), "ERROR", 5);
			TextOut(hDC, (int) (width * 0.5), (int) (height * 0.8), g_Data.errorMessage, strlen(g_Data.errorMessage));
			SetTextColor (hDC, RGB(0, 255, 0));
		}
		if (g_Data.progState == PROGSTATE_TLI_WAITING || g_Data.progState == PROGSTATE_TLI_RUNNING) {
			SetTextAlign (hDC, TA_LEFT);
			if (g_Data.burnData.IgnMJD != 0) {
				if (oapiGetSimMJD() < g_Data.burnData.IgnMJD) {
					TextOut(hDC, (int) (width * 0.1), (int) (height * 0.8), "Time to Ignition:", 17);
					sprintf(buffer, "%.0lf s", (oapiGetSimMJD() - g_Data.burnData.IgnMJD) * 24. * 3600.);
				} else {
					TextOut(hDC, (int) (width * 0.1), (int) (height * 0.8), "Remaining Burn Time:", 20);
					sprintf(buffer, "%.0lf s", g_Data.burnData.BT - ((oapiGetSimMJD() - g_Data.burnData.IgnMJD) * 24. * 3600.));
				}
				SetTextAlign (hDC, TA_RIGHT);
				TextOut(hDC, (int) (width * 0.9), (int) (height * 0.8), buffer, strlen(buffer));
			}
		}
	}
	//Draw Telemetry
	else if (screen == PROG_TELE) {
		SetTextAlign (hDC, TA_CENTER);
		TextOut(hDC, width / 2, (int) (height * 0.3), "Telemetry", 9);
		sprintf(buffer, "Status: %s", debugWinsock);
		TextOut(hDC, width / 2, (int) (height * 0.35), buffer, strlen(buffer));
		if (g_Data.statevectorReady == 1 || g_Data.updateclockReady == 1) {
			sprintf(buffer, "Checklist");
			TextOut(hDC, width / 2, (int) (height * 0.45), buffer, strlen(buffer));
			SetTextAlign (hDC, TA_LEFT);
			sprintf(buffer, "DSKY - V37E 00E");
			TextOut(hDC, width * 0.1, (int) (height * 0.55), buffer, strlen(buffer));
			sprintf(buffer, "UPTLM CM - ACCEPT (up)   2, 122");
			TextOut(hDC, width * 0.1, (int) (height * 0.60), buffer, strlen(buffer));
			sprintf(buffer, "UP TLM - DATA (up)            3");
			TextOut(hDC, width * 0.1, (int) (height * 0.65), buffer, strlen(buffer));
			sprintf(buffer, "PCM BIT RATE - HIGH (up)      3");
			TextOut(hDC, width * 0.1, (int) (height * 0.7), buffer, strlen(buffer));
			SetTextAlign (hDC, TA_CENTER);
			if (g_Data.statevectorReady == 1) 
				sprintf(buffer, "Press SV to start upload");
			else
				sprintf(buffer, "Press CLK to start upload");
			TextOut(hDC, width / 2, (int) (height * 0.8), buffer, strlen(buffer));
		}
		else if(g_Data.statevectorReady == 2) {
			SetTextAlign (hDC, TA_LEFT);
			sprintf(buffer, "         %ld", g_Data.emem[0]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * 0.4), buffer, strlen(buffer));
			sprintf(buffer, "         %ld", g_Data.emem[1]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * 0.45), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1501 %ld", g_Data.emem[2]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * 0.5), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1502 %ld", g_Data.emem[3]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * 0.55), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1503 %ld", g_Data.emem[4]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * 0.6), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1504 %ld", g_Data.emem[5]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * 0.65), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1505 %ld", g_Data.emem[6]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * 0.7), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1506 %ld", g_Data.emem[7]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * 0.75), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1507 %ld", g_Data.emem[8]);		
			TextOut(hDC, (int) (width * 0.1), (int) (height * 0.8), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1510 %ld", g_Data.emem[9]);
			TextOut(hDC, (int) (width * 0.55), (int) (height * 0.4), buffer, strlen(buffer));		
			sprintf(buffer, "EMEM1511 %ld", g_Data.emem[10]);
			TextOut(hDC, (int) (width * 0.55), (int) (height * 0.45), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1512 %ld", g_Data.emem[11]);
			TextOut(hDC, (int) (width * 0.55), (int) (height * 0.5), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1513 %ld", g_Data.emem[12]);
			TextOut(hDC, (int) (width * 0.55), (int) (height * 0.55), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1514 %ld", g_Data.emem[13]);
			TextOut(hDC, (int) (width * 0.55), (int) (height * 0.6), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1515 %ld", g_Data.emem[14]);
			TextOut(hDC, (int) (width * 0.55), (int) (height * 0.65), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1516 %ld", g_Data.emem[15]);
			TextOut(hDC, (int) (width * 0.55), (int) (height * 0.7), buffer, strlen(buffer));
			sprintf(buffer, "EMEM1517 %ld", g_Data.emem[16]);
			TextOut(hDC, (int) (width * 0.55), (int) (height * 0.75), buffer, strlen(buffer));
		}
		SetTextAlign (hDC, TA_LEFT);
		SetTextColor (hDC, RGB(128, 128, 128));
		oapiGetObjectName(g_Data.vessel->GetHandle(), buffer, 100);
		TextOut(hDC, (int) (width * 0.05), (int) (height * 0.9), buffer, strlen(buffer));
		oapiGetObjectName(g_Data.planet, buffer, 100);
		TextOut(hDC, (int) (width * 0.05), (int) (height * 0.95), buffer, strlen(buffer));
	}
	else if (screen == PROG_DEBUG)
	{

		if ((strcmp(debugString,debugStringBuffer)!= 0) && (strlen(debugStringBuffer) != 0) && !debug_frozen)
		{
			strcpy(debugString, debugStringBuffer);
			sprintf(debugStringBuffer,"");
		}
		TextOut(hDC, width / 2, (int)(height * 0.3), "Debug Data",10);
		if (strlen(debugString) > 35)
		{
			int i = 0;
			double h = 0.4;
			bool done = false;
			while (!done)
			{
				if (strlen(&debugString[i]) > 35)
				{
					TextOut(hDC, width / 2, (int) (height * h), &debugString[i], 35);
					i = i + 35;
					h = h + 0.05;
				}
				else
				{
					TextOut(hDC, width / 2, (int) (height * h), &debugString[i], strlen(&debugString[i]));
					done = true;
				}
			}
		}
		else TextOut(hDC, width / 2, (int) (height * 0.4), debugString, strlen(debugString));
	}
}


void ProjectApolloMFD::GetStateVector (void)
{
	char buffer[16];
	char buffer2[16];
	double mt;
	if (!crawler)
		mt = saturn->GetMissionTime();
	else
		mt = crawler->GetMissionTime();
	MATRIX3 q1, q2, q3;
	VECTOR3 pos, vel;
	int n;
	double calc_1[7];
	double calc_2[7];
	double calc_3[7];
	g_Data.vessel->GetRelativePos(g_Data.planet, pos); 
	g_Data.vessel->GetRelativeVel(g_Data.planet, vel);
	pos = _V(pos.x, pos.z, -pos.y);
	vel = _V(vel.x, vel.z, vel.y);
	double get = fabs(mt);
	double a_angle = 180.0*RAD + oapiGetPlanetObliquity(g_Data.planet);
	double b_angle = 0.0*RAD;
	double y_angle = 180.0*RAD;
	q1 = _M(cos(b_angle), 0, -sin(b_angle), 0, 1, 0, sin(b_angle), 0, cos(b_angle));
	q2 = _M(cos(y_angle), sin(y_angle), 0, -sin(y_angle), cos(y_angle), 0, 0, 0, 1);
	q3 = _M(1, 0, 0, 0, cos(a_angle), sin(a_angle), 0, -sin(a_angle), cos(a_angle));
	q1 = mul(q1, q2);
	q1 = mul(q1, q3);	
	pos = tmul(q1, pos);
	a_angle = oapiGetPlanetObliquity(g_Data.planet);
	b_angle = 0.0*RAD;
	y_angle = 0.0*RAD;
	q1 = _M(cos(b_angle), 0, -sin(b_angle), 0, 1, 0, sin(b_angle), 0, cos(b_angle));
	q2 = _M(cos(y_angle), sin(y_angle), 0, -sin(y_angle), cos(y_angle), 0, 0, 0, 1);
	q3 = _M(1, 0, 0, 0, cos(a_angle), sin(a_angle), 0, -sin(a_angle), cos(a_angle));
	q1 = mul(q1, q2);
	q1 = mul(q1, q3);
	vel = tmul(q1, vel);
	oapiGetObjectName(g_Data.planet, buffer, 16);
	if(strcmp(buffer,"Earth") == 0) {
		g_Data.emem[0] = 21;
		g_Data.emem[1] = 1501;
		oapiGetObjectName(g_Data.vessel->GetHandle(), buffer, 16);
		oapiGetObjectName(oapiGetFocusObject(), buffer2, 16);
		if(strcmp(buffer, buffer2) == 0)
			g_Data.emem[2] = 1;
		else
			g_Data.emem[2] = 77776;
		calc_1[0] = -pos.x * pow(2.0, -29.0);
		calc_1[1] = pos.y * pow(2.0, -29.0);
		calc_1[2] = pos.z * pow(2.0, -29.0);
		calc_1[3] = (vel.x/100.0) * pow(2.0, -7.0);
		calc_1[4] = (vel.y/100.0) * pow(2.0, -7.0);
		calc_1[5] = (vel.z/100.0) * pow(2.0, -7.0);
		calc_1[6] = (get*100.0) * pow(2.0, -28.0);
	}
	else if(strcmp(buffer,"Moon") == 0) {
		g_Data.emem[0] = 21;
		g_Data.emem[1] = 1501;
		oapiGetObjectName(g_Data.vessel->GetHandle(), buffer, 16);
		oapiGetObjectName(oapiGetFocusObject(), buffer2, 16);
		if(strcmp(buffer, buffer2) == 0)
			g_Data.emem[2] = 2;
		else
			g_Data.emem[2] = 77775;
		calc_1[0] = -pos.x * pow(2.0, -27.0);
		calc_1[1] = pos.y * pow(2.0, -27.0);
		calc_1[2] = pos.z * pow(2.0, -27.0);
		calc_1[3] = (vel.x/100.0) * pow(2.0, -5.0);
		calc_1[4] = (vel.y/100.0) * pow(2.0, -5.0);
		calc_1[5] = (vel.z/100.0) * pow(2.0, -5.0);
		calc_1[6] = (get*100.0) * pow(2.0, -28.0); 
	}
	else {
		g_Data.emem[0] = 0;
		g_Data.emem[1] = 0;
		g_Data.emem[2] = 0;
		calc_1[0] = 0;
		calc_1[1] = 0;
		calc_1[2] = 0;
		calc_1[3] = 0;
		calc_1[4] = 0;
		calc_1[5] = 0;
		calc_1[6] = 0;
	}
	for(n = 0; n < 7; n++) {
		if(calc_1[n] >= 0.0)
			calc_2[n] = fmod(calc_1[n], pow(2.0, -14.0));
		else
			calc_2[n] = -fmod(-calc_1[n], pow(2.0, -14.0));
		calc_3[n] = calc_1[n]-calc_2[n];
	}
	for(n = 0; n < 14; n+=2) {
		g_Data.emem[n+3] = irDEC2OCT(calc_3[n/2]);
		g_Data.emem[n+4] = irDEC2OCT(calc_2[n/2]*pow(2.0, 14.0));
	}
}
int ProjectApolloMFD::DEC2OCT(int a)
{
	int base = 0;
	int factor = 1;
	while(a != 0){
		base += (a%8)*factor;
		factor *= 10;
		a /= 8;
	}
	return base;
}
int ProjectApolloMFD::irDEC2OCT(double a)
{
	int oct = 0;
	if(a < 0.0)	{
		a*=-1.0;
		int n = static_cast<int>(a*pow(2.0, 14.0)+0.5);
		int bin, dec;
		dec = 0;
		for(double i = 0.0; i < 15.0; i++)
		{
			bin = n%2;
			bin = bin*-1+1;
			dec += bin*(int)pow(2.0, i);
			n/=2;
		}
		oct = DEC2OCT(dec);
	}
	else
	{
		int n = static_cast<int>(a*pow(2.0, 14.0)+0.5);
		oct = DEC2OCT(n);
	}
	return oct;
}

void ProjectApolloMFD::WriteStatus (FILEHANDLE scn) const
{
	oapiWriteScenario_int(scn, "SCREEN", screen);
	oapiWriteScenario_int(scn, "PROGNO", g_Data.prog);
	oapiWriteScenario_int(scn, "PROGSTATE", g_Data.progState);
	if (g_Data.progVessel)
		oapiWriteScenario_string(scn, "PROGVESSEL", g_Data.progVessel->GetName());
	papiWriteScenario_double(scn, "NEXTREQUESTTIME", g_Data.nextRequestTime);
	papiWriteScenario_double(scn, "BURNDATA_IGNMJD", g_Data.burnData.IgnMJD);
	papiWriteScenario_double(scn, "BURNDATA_BT", g_Data.burnData.BT);
	papiWriteScenario_vec(scn, "BURNDATA_RIGN", g_Data.burnData._RIgn);
	papiWriteScenario_vec(scn, "BURNDATA_VIGN", g_Data.burnData._VIgn);
	papiWriteScenario_vec(scn, "BURNDATA_DVLVLH", g_Data.burnData._dV_LVLH);
}

void ProjectApolloMFD::ReadStatus (FILEHANDLE scn)
{
    char *line, name[100];

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, "END_MFD", 7))
			return;

		if (!strnicmp (line, "PROGVESSEL", 10)) {
			sscanf (line + 10, "%s", name);
			OBJHANDLE h = oapiGetVesselByName(name);
			if (h != NULL)
				g_Data.progVessel = (Saturn *) oapiGetVesselInterface(h);
		} 
		papiReadScenario_int(line, "SCREEN", screen);
		papiReadScenario_int(line, "PROGNO", g_Data.prog);
		papiReadScenario_int(line, "PROGSTATE", g_Data.progState);
		papiReadScenario_double(line, "NEXTREQUESTTIME", g_Data.nextRequestTime);
		papiReadScenario_double(line, "BURNDATA_IGNMJD", g_Data.burnData.IgnMJD);
		papiReadScenario_double(line, "BURNDATA_BT", g_Data.burnData.BT);
		papiReadScenario_vec(line, "BURNDATA_RIGN", g_Data.burnData._RIgn);
		papiReadScenario_vec(line, "BURNDATA_VIGN", g_Data.burnData._VIgn);
		papiReadScenario_vec(line, "BURNDATA_DVLVLH", g_Data.burnData._dV_LVLH);
	}
}

bool ProjectApolloMFD::SetSource (char *rstr)
{
	OBJHANDLE vessel_obj = oapiGetVesselByName(rstr);
	if(vessel_obj != NULL)
	{
		g_Data.vessel = new VESSEL(vessel_obj,1);
		return true;
	}
	return false;
}

bool ProjectApolloMFD::SetReferencePlanet (char *rstr)
{
	if(stricmp(rstr, "Earth") == 0 || stricmp(rstr, "Moon") == 0)
	{
		g_Data.planet = oapiGetGbodyByName(rstr);
		return true;
	}
	return false;
}

bool ProjectApolloMFD::SetCrewNumber (char *rstr)
{
	int n;

	if (sscanf (rstr, "%d", &n) == 1 && n >= 0 && n <= 3) {
		if (saturn)
			saturn->SetCrewNumber(n);
		InvalidateDisplay();
		return true;
	}
	return false;
}

bool ProjectApolloMFD::SetPrimECSTestHeaterPower (char *rstr)
{
	double v;

	if (sscanf (rstr, "%lf", &v) == 1 && v >= -3000. && v <= 3000.) {
		if (saturn)
			saturn->SetPrimECSTestHeaterPowerW(v);
		InvalidateDisplay();
		return true;
	}
	return false;
}

bool ProjectApolloMFD::SetSecECSTestHeaterPower (char *rstr)
{
	double v;

	if (sscanf (rstr, "%lf", &v) == 1 && v >= -3000. && v <= 3000.) {
		if (saturn)
			saturn->SetSecECSTestHeaterPowerW(v);
		InvalidateDisplay();
		return true;
	}
	return false;
}

void ProjectApolloMFD::StoreStatus (void) const
{
	screenData.screen = screen;
}

void ProjectApolloMFD::RecallStatus (void)
{
	screen = screenData.screen;
}

// MFD message parser
int ProjectApolloMFD::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		// Our new MFD mode has been selected, so we create the MFD and
		// return a pointer to it.
		return (int)(new ProjectApolloMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}

bool SourceInput (void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetSource(str);
}

bool ReferencePlanetInput (void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetReferencePlanet(str);
}

bool CrewNumberInput (void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetCrewNumber(str);
}

bool PrimECSTestHeaterPowerInput (void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetPrimECSTestHeaterPower(str);
}

bool SecECSTestHeaterPowerInput (void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetSecECSTestHeaterPower(str);
}

ProjectApolloMFD::ScreenData ProjectApolloMFD::screenData = {PROG_NONE};


DLLCLBK bool pacDefineSocket(SOCKET sockettoclose)
{
	close_Socket = sockettoclose;
	return true;
}

DLLCLBK char *pacMFDGetDebugString()
{
	return debugStringBuffer;
}