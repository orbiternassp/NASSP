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
  *	Revision 1.24  2012/08/09 18:55:44  tschachim
  *	State vector slot selection button instead of "focus automatic"
  *	
  *	Revision 1.23  2012/08/07 20:37:33  tschachim
  *	Removed the "Other SV" feature as it was buggy and obsolete, use the Change Source button instead.
  *	
  *	Revision 1.21  2012/01/14 22:31:32  tschachim
  *	Save EMS scroll
  *	
  *	Revision 1.20  2010/11/05 03:10:00  vrouleau
  *	Added CSM/LM Landing Site REFSMMAT Uplink to AGC. Contains hardcoded REFSMMAT per mission.
  *	
  *	Revision 1.19  2010/09/06 16:23:21  tschachim
  *	Changes for Orbiter 2010-P1
  *	
  *	Revision 1.18  2010/08/29 18:51:01  tschachim
  *	Display inclination
  *	
  *	Revision 1.17  2010/01/04 12:31:15  tschachim
  *	Improved Saturn IB launch autopilot, bugfixes
  *	
  *	Revision 1.16  2009/12/22 18:14:47  tschachim
  *	More bugfixes related to the prelaunch/launch checklists.
  *	
  *	Revision 1.15  2009/09/13 15:20:15  dseagrav
  *	LM Checkpoint Commit. Adds LM telemetry, fixed missing switch row init, initial LM support for PAMFD.
  *	
  *	Revision 1.14  2009/08/17 13:27:49  tschachim
  *	Enhancement of ChecklistMFD
  *	
  *	Revision 1.13  2009/07/11 18:59:51  tschachim
  *	Epoch corrections by Jarmo.
  *	
  *	Revision 1.12  2009/06/12 10:15:47  tschachim
  *	Added V37E00E at the end of clock update.
  *	
  *	Revision 1.11  2009/05/31 01:46:15  bluedragon8144
  *	fixed clock update to work with new queue function for uplinking to agc
  *	
  *	Revision 1.10  2009/05/07 11:49:13  tschachim
  *	Killrot, display cosmetics
  *	
  *	Revision 1.9  2009/05/05 19:07:21  bluedragon8144
  *	Fixed wrong change in IMFD TLI screen, fixed wrong variable assignments for uplinkData() tcp/ip errors
  *	
  *	Revision 1.8  2009/05/04 23:02:27  bluedragon8144
  *	Removed old P30, renamed External dV to P30 (thanks Tschachim), renamed Lambert to P31, removed time-dependent parts of the uplink functions (they are now queued), increased emem size to 24, moved P30 and P31 to the telemetry screen, added IMFD request button to telemetry screen
  *	
  *	Revision 1.7  2009/05/03 23:10:23  bluedragon8144
  *	Added better error handling on IMFD screen, added Lambert and External DV P27 cmc updates to IMFD screen
  *
  *	Revision 1.6  2009/05/01 18:39:37  bluedragon8144
  *	Added P30 Uplink Button to IMFD screen, added preliminary uplink code for P31 and ExtDV (to be added to Telemetry screen)
  *
  *	Revision 1.5  2009/05/01 17:05:36  bluedragon8144
  *	modified state vectors calculations to Jarmonik's calculations
  *
  *	Revision 1.4  2009/04/28 16:05:34  tschachim
  *	Queuing uplink data stream to prevent buffer issues.
  *	
  *	Revision 1.3  2009/03/16 19:42:31  tschachim
  *	New GetStateVector and bugfixes by Jarmo
  *	
  *	Revision 1.2  2009/03/03 15:37:53  bluedragon8144
  *	Fixed moon state vector angle
  *	
  *	Revision 1.1  2009/02/18 23:21:34  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.26  2008/09/15 19:52:42  tschachim
  *	Telemetry checklist layout.
  *	
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
#include "lemcomputer.h"
#include "IMU.h"
#include "lvimu.h"
#include "saturn.h"
#include "LEM.h"
#include "Crawler.h"
#include "papi.h"
#include <stdio.h>

#include "MFDResource.h"
#include "ProjectApolloMFD.h"

#include <queue>



// ==============================================================
// Global variables

static HINSTANCE g_hDLL;
static int g_MFDmode; // identifier for new MFD mode


#define PROG_NONE		0
#define PROG_GNC		1
#define PROG_ECS		2
#define PROG_IMFD		3
#define PROG_IMFDTLI	4
#define PROG_TELE		5
//This program displays info on the current telcom socket.  For debugging only.
#define PROG_SOCK		6		
#define PROG_DEBUG		7
// This screen pulls data from the CMC to be used for initializing the LGC
#define PROG_LGC		8

#define PROGSTATE_NONE				0
#define PROGSTATE_TLI_START			1
#define PROGSTATE_TLI_REQUESTING	2
#define PROGSTATE_TLI_WAITING		3
#define PROGSTATE_TLI_RUNNING		4
#define PROGSTATE_TLI_ERROR			5

#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

#define  UPLINK_SV				0
#define  UPLINK_P30				1
#define  UPLINK_P31				2
#define  UPLINK_REFSMMAT		3


int apolloLandSiteRefsmmat[7][18] = 
{
	// Apollo 11 - These values gotten from EMEM 1735 after P51 execution on CSM above landing site pointing up on Jul 20th 1969 20:17. Not the actual values
	{14417,31562, 11111, 14164 , 3727 , 27344, 77640, 53206, 71722, 53530, 16626, 36644, 11744, 33312, 64274, 40123, 73254, 77567},
	// Apollo 12
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	// Apollo 13 
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	// Apollo 14
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	// Apollo 15
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	// Apollo 16
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	// Apollo 17
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int apolloLandSiteLiftOffRefsmmat[7][18] = 
{
	// Apollo 11
	{14417,31562, 11111, 14164 , 3727 , 27344, 77640, 53206, 71722, 53530, 16626, 36644, 11744, 33312, 64274, 40123, 73254, 77567},
	// Apollo 12
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	// Apollo 13
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	// Apollo 14
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	// Apollo 15
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	// Apollo 16
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	// Apollo 17
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
// Time to ejection when the IU is programmed
#define IUSTARTTIME 900

static struct ProjectApolloMFDData {  // global data storage
	int prog;	
	int progState;  
	Saturn *progVessel;
	LEM *gorpVessel;

	double nextRequestTime;
	IMFD_BURN_DATA burnData;
	bool isRequesting;
	bool isRequestingManually;
	double requestMjd;
	bool hasError;
	char *errorMessage;

	int emem[24];
	int connStatus;
	int uplinkDataReady;
	int uplinkDataType;
	int updateClockReady;
	int uplinkState;
	int uplinkLEM;
	int uplinkSlot;
	queue<unsigned char> uplinkBuffer;
	double uplinkBufferSimt;
	OBJHANDLE planet;
	VESSEL *vessel;

	int killrot;
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
	MFDMODESPECEX spec;
	spec.name = name;
	spec.key = OAPI_KEY_A;					   // MFD mode selection key is obsolete
	spec.context = NULL;
	spec.msgproc = ProjectApolloMFD::MsgProc;  // MFD mode callback function

	// Register the new MFD mode with Orbiter
	g_MFDmode = oapiRegisterMFDMode (spec);
	g_hDLL = hDLL;

	g_Data.prog = PROG_NONE;
	g_Data.progState = 0;
	g_Data.progVessel = NULL;
	g_Data.gorpVessel = NULL;
	g_Data.nextRequestTime = 0;
	g_Data.uplinkLEM = 0;
	g_Data.uplinkSlot = 0;

	ZeroMemory(&g_Data.burnData, sizeof(IMFD_BURN_DATA));
	g_Data.isRequesting = false;
	g_Data.isRequestingManually = false;
	g_Data.requestMjd = 0;
	g_Data.hasError = false;
	g_Data.errorMessage = "";
	g_Data.connStatus = 0;
	g_Data.uplinkDataReady = 0;
	g_Data.uplinkDataType = 0;
	g_Data.updateClockReady = 0;
	g_Data.uplinkState = 0;
	//Init Emem
	for(int i = 0; i < 24; i++)
		g_Data.emem[i] = 0;
	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR ) {
		sprintf(debugWinsock,"ERROR AT WSAStartup()");
	}
	else {
		sprintf(debugWinsock,"DISCONNECTED");
	}
	g_Data.uplinkBufferSimt = 0;
	g_Data.killrot = 0;
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

int DoubleToBuffer(double x, double q, int m)
{
	int c=0, out=0, f=1;
	
	x = x * (268435456.0 / pow(2.0, fabs(q)));
	
	if (m) c = 0x3FFF & (((int)fabs(x))>>14);	// High word
	else   c = 0x3FFF & ((int)fabs(x));		// Low word
	
	if (x<0.0) c = 0x7FFF & (~c); // Polarity change
	
	while (c!=0) {
		out += (c&7) * f;
		f*=10;	c = c>>3;
	}
	return out;
}

void send_agc_key(char key)	{

	int bytesXmit = SOCKET_ERROR;
	unsigned char cmdbuf[4];

	if(g_Data.uplinkLEM > 0){
		cmdbuf[0] = 031; // VA,SA for LEM
	}else{
		cmdbuf[0] = 043; // VA,SA for CM
	}

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
	for (int i = 0; i < 3; i++) {
		g_Data.uplinkBuffer.push(cmdbuf[i]);
	}
}

void uplink_word(char *data)
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

void UplinkData()
{
	if (g_Data.connStatus == 0) {
		int bytesRecv = SOCKET_ERROR;
		char addr[256];
		char buffer[8];
		m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );	
		if ( m_socket == INVALID_SOCKET ) {
			g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock,"ERROR AT SOCKET(): %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(addr, "127.0.0.1");
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr);
		if(g_Data.uplinkLEM > 0){	clientService.sin_port = htons( 14243 ); }else{ clientService.sin_port = htons( 14242 ); }
		if (connect( m_socket, (SOCKADDR*) &clientService, sizeof(clientService)) == SOCKET_ERROR) {
			g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock,"FAILED TO CONNECT, ERROR %ld",WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "CONNECTED");
		g_Data.uplinkState = 0;
		send_agc_key('V');
		send_agc_key('7');
		send_agc_key('1');
		send_agc_key('E');
			
		int cnt2 = (g_Data.emem[0]/10);
		int cnt  = (g_Data.emem[0]-(cnt2*10)) + cnt2*8;
		
		while (g_Data.uplinkState < cnt && cnt<=20 && cnt>=3) {
			sprintf(buffer, "%ld", g_Data.emem[g_Data.uplinkState]);
			uplink_word(buffer);
			g_Data.uplinkState++;
		}
		send_agc_key('V');
		send_agc_key('3');
		send_agc_key('3');
		send_agc_key('E');
		g_Data.connStatus = 1;
		g_Data.uplinkState = 0;
	}
}

void UpdateClock()
{		
	if (g_Data.connStatus == 0)
	{
		int bytesRecv = SOCKET_ERROR;
		char addr[256];
		char buffer[8];
		m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );	
		if ( m_socket == INVALID_SOCKET ) {
			g_Data.updateClockReady = 0;
			sprintf(debugWinsock,"ERROR AT SOCKET(): %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(addr, "127.0.0.1");
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr);
		if(g_Data.uplinkLEM > 0){ clientService.sin_port = htons( 14243 ); }else{ clientService.sin_port = htons( 14242 ); }
		if (connect( m_socket, (SOCKADDR*) &clientService, sizeof(clientService)) == SOCKET_ERROR) {
			g_Data.updateClockReady = 0;
			sprintf(debugWinsock,"FAILED TO CONNECT, ERROR %ld",WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "CONNECTED");
		g_Data.uplinkState = 0;
		send_agc_key('V');
		send_agc_key('1');
		send_agc_key('6');
		send_agc_key('N');
		send_agc_key('3');
		send_agc_key('6');
		send_agc_key('E');
		send_agc_key('V');
		send_agc_key('2');
		send_agc_key('5');
		send_agc_key('E');
		// reset clock
		sprintf(buffer, "00000");		
		send_agc_key('+');
		uplink_word(buffer);
		send_agc_key('+');
		uplink_word(buffer);
		send_agc_key('+');
		uplink_word(buffer);		
		// Send until queue is empty, then continue 
		// with V55 clock update
		g_Data.connStatus = 2;
	} else if (g_Data.connStatus == 2) {
		// increment clock
		g_Data.updateClockReady = 0;
		char buffer[8];	
		double mt;
		if(g_Data.uplinkLEM > 0){ mt = g_Data.gorpVessel->GetMissionTime(); }else{ mt = g_Data.progVessel->GetMissionTime(); }
		char sign = '+';
		if (mt < 0)
			sign = '-';
		mt = abs(mt);
		int hours = ( (int) mt / 3600);
		mt -= 3600.0 * hours;
		int minutes = ( (int) mt / 60);
		mt -= 60.0 * minutes;
		int secs = (int) (mt * 100.0);
		send_agc_key('V');
		send_agc_key('5');
		send_agc_key('5');
		send_agc_key('E');	
		send_agc_key(sign);
		sprintf(buffer, "%ld", hours);
		uplink_word(buffer);
		send_agc_key(sign);
		sprintf(buffer, "%ld", minutes);
		uplink_word(buffer);
		send_agc_key(sign);
		sprintf(buffer, "%ld", secs);
		uplink_word(buffer);

		send_agc_key('V');
		send_agc_key('3');
		send_agc_key('7');
		send_agc_key('E');
		send_agc_key('0');
		send_agc_key('0');
		send_agc_key('E');
		// Send until queue empty, then reset uplinkDataReady to 0
		// and close the socket
		g_Data.connStatus = 1;
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

	if (g_Data.connStatus > 0 && g_Data.uplinkBuffer.size() > 0) {
		if (simt > g_Data.uplinkBufferSimt + 0.05) {
			unsigned char data = g_Data.uplinkBuffer.front();
			send(m_socket, (char *) &data, 1, 0);
			g_Data.uplinkBuffer.pop();
			g_Data.uplinkBufferSimt = simt;
		}
	} else if (g_Data.connStatus > 0 && g_Data.uplinkBuffer.size() == 0) {
		if (g_Data.connStatus == 1)	{
			sprintf(debugWinsock, "DISCONNECTED");
			g_Data.uplinkDataReady = 0;				
			g_Data.updateClockReady = 0;
			g_Data.connStatus = 0;
			closesocket(m_socket);
		} else if (g_Data.connStatus == 2 && g_Data.updateClockReady == 2) {
			UpdateClock();
		}
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

	if (g_Data.progVessel && g_Data.killrot) {
		g_Data.progVessel->SetAngularVel(_V(0, 0, 0));
	}

}

// ==============================================================
// MFD class implementation

// Constructor
ProjectApolloMFD::ProjectApolloMFD (DWORD w, DWORD h, VESSEL *vessel) : MFD (w, h, vessel)

{
	saturn = NULL;
	crawler = NULL;
	lem = NULL;
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
	else if (!stricmp(vessel->GetClassName(), "ProjectApollo\\LEM") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo/LEM")) {
			lem = (LEM *)vessel;
			g_Data.vessel = vessel;
			g_Data.gorpVessel = lem;
			oapiGetObjectName(lem->GetGravityRef(), buffer, 8);
			if(strcmp(buffer,"Earth") == 0 || strcmp(buffer,"Moon") == 0 )
				g_Data.planet = lem->GetGravityRef();
			else
				g_Data.planet = oapiGetGbodyByName("Earth");
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
	static char *labelNone[12] = {"GNC", "ECS", "IMFD", "TELE","LGC","","","","","","SOCK","DBG"};
	static char *labelGNC[4] = {"BCK", "KILR", "EMS", "DMP"};
	static char *labelECS[4] = {"BCK", "CRW", "PRM", "SEC"};
	static char *labelIMFDTliStop[3] = {"BCK", "REQ", "SIVB"};
	static char *labelIMFDTliRun[3] = {"BCK", "REQ", "STP"};
	static char *labelTELE[11] = {"BCK", "SV", "P30", "P31", "SRC", "REF", "REQ", "CLK", "LS", "", "SLT"};
	static char *labelSOCK[1] = {"BCK"};	
	static char *labelDEBUG[12] = {"","","","","","","","","","CLR","FRZ","BCK"};
	static char *labelLGC[1] = {"BCK"};

	//If we are working with an unsupported vehicle, we don't want to return any button labels.
	if (!saturn && !lem) {
		return 0;
	}
	if (screen == PROG_GNC) {
		return (bt < 4 ? labelGNC[bt] : 0);
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
		return (bt < 11 ? labelTELE[bt] : 0);
	}
	else if (screen == PROG_SOCK) {
		return (bt < 1 ? labelSOCK[bt] : 0);
	}
	else if (screen == PROG_DEBUG) {
		return (bt < 12 ? labelDEBUG[bt] : 0);
	}
	else if (screen == PROG_LGC) {
		return (bt < 1 ? labelLGC[bt] : 0);
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
		{"LGC Initialization Data",0,'L'},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{"Socket info", 0, 'S'},
		{"Debug String",0,'D'}
	};
	static const MFDBUTTONMENU mnuGNC[4] = {
		{"Back", 0, 'B'},
		{"Kill rotation", 0, 'K'},
		{"Save EMS scroll", 0, 'E'},
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
		{"Request Burn Data", 0, 'R'},
		{"Start S-IVB burn", 0, 'S'}
	};
	static const MFDBUTTONMENU mnuIMFDTliRun[3] = {
		{"Back", 0, 'B'},
		{"Request Burn Data", 0, 'R'},
		{"Start S-IVB burn", 0, 'S'}
	};
	static const MFDBUTTONMENU mnuTELE[11] = {
		{"Back", 0, 'B'},
		{"State Vector Update", 0, 'U'},
		{"P30 - Ext. DV Uplink", 0, 'D'},
		{"P31 - Lambert Aim Point Uplink", 0, 'L'},
		{"Change Source",0,'S'},
		{"Change Reference Body", 0, 'R'},
		{"Toggle burn data requests", 0, 'I'},
		{"Clock Update", 0, 'C'},
		{"REFSMMAT Upd.", 0, 'F'},
		{0,0,0},
		{"State Vector Slot", 0, 'T'}
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
	static const MFDBUTTONMENU mnuLGC[1] = {
		{"Back", 0, 'B'}
	};
	// We don't want to display a menu if we are in an unsupported vessel.
	if (!saturn && !lem) {
		menu = 0;
		return 0;
	}

	if (screen == PROG_GNC) {
		if (menu) *menu = mnuGNC;
		return 4; 
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
		return 11;
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
	else if (screen == PROG_LGC)
	{
		if (menu) *menu = mnuLGC;
		return 1;
	}
	else {
		if (menu) *menu = mnuNone;
		return 12; 
	}
}

bool ProjectApolloMFD::ConsumeKeyBuffered (DWORD key) 
{
	//We don't want to accept keyboard commands from the wrong vessels.
	if (!saturn && !lem)
		return false;

	if (screen == PROG_NONE) {
		if (key == OAPI_KEY_G) {
			screen = PROG_GNC;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_E && saturn != NULL) {
			// ECS is not supported for the LEM yet.
			screen = PROG_ECS;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_I && saturn != NULL) {
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
		} else if (key == OAPI_KEY_L) {
			screen = PROG_LGC;
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
		} else if (key == OAPI_KEY_K) {
			g_Data.killrot ? g_Data.killrot = 0 : g_Data.killrot = 1;				
			return true;
		} else if (key == OAPI_KEY_E) {
			if (saturn)
				saturn->SaveEMSScroll(); 			
			return true;
		} 		
	} else if (screen == PROG_TELE) {
		if (key == OAPI_KEY_B) {
			if(g_Data.uplinkDataReady == 0 && g_Data.updateClockReady == 0) {
				g_Data.hasError = false;
				screen = PROG_NONE;
				InvalidateDisplay();
				InvalidateButtons();
			} else {
				if(g_Data.uplinkDataReady == 1)
					g_Data.uplinkDataReady -= 1;
				if(g_Data.updateClockReady == 1)
					g_Data.updateClockReady -= 1;
			}
			return true;
		} else if (key == OAPI_KEY_U) {
			if (saturn || lem) {
				if (g_Data.uplinkDataReady == 0 && g_Data.updateClockReady == 0) {
					g_Data.uplinkDataReady = 1;
					g_Data.uplinkDataType = UPLINK_SV;
				}
				else if (g_Data.uplinkDataReady == 1 && g_Data.uplinkDataType == UPLINK_SV) {
					if(!saturn){ g_Data.uplinkLEM = 1; }else{ g_Data.uplinkLEM = 0; } // LEM flag
					GetStateVector();
				}
			}
			return true;
		} else if (key == OAPI_KEY_D) {
			if (saturn) {
				if (g_Data.uplinkDataReady == 0 && g_Data.updateClockReady == 0) {
					if( saturn->GetIMFDClient()->IsBurnDataValid()) {
						IMFD_BURN_DATA bd = saturn->GetIMFDClient()->GetBurnData();
						if (bd.DataMJD==0.0 || !bd.p30mode || bd.impulsive) {
							g_Data.hasError = true;
						} else {
							g_Data.hasError = false;							
							g_Data.uplinkDataReady = 1;
							g_Data.uplinkDataType = UPLINK_P30;
						}
					} else {
						g_Data.hasError = true;
					}
				} else if (g_Data.uplinkDataReady == 1 && g_Data.uplinkDataType == UPLINK_P30) {		
					IMFDP30Uplink();
				}
			}
			return true;
		} else if (key == OAPI_KEY_L) {
			if (saturn) {
				if (g_Data.uplinkDataReady == 0 && g_Data.updateClockReady == 0) {
					if( saturn->GetIMFDClient()->IsBurnDataValid()) {
						IMFD_BURN_DATA bd = saturn->GetIMFDClient()->GetBurnData();
						if (bd.DataMJD==0.0 || bd.LAP_MJD == 0 || !bd.p30mode || bd.impulsive) {
							g_Data.hasError = true;
						} else {
							g_Data.hasError = false;
							g_Data.uplinkDataReady = 1;							
							g_Data.uplinkDataType = UPLINK_P31;
						}
					} else {
						g_Data.hasError = true;
					}
				} else if (g_Data.uplinkDataReady == 1 && g_Data.uplinkDataType == UPLINK_P31) {
					IMFDP31Uplink();		
				}
			}
			return true;
		} else if (key == OAPI_KEY_I && !g_Data.isRequestingManually && saturn != NULL) {						
			if (!saturn->GetIMFDClient()->IsBurnDataRequesting()) {
				saturn->GetIMFDClient()->StartBurnDataRequests();
				g_Data.hasError = false;
				g_Data.isRequestingManually = true;
			}
		} else if (key == OAPI_KEY_I && g_Data.isRequestingManually && saturn != NULL) {
			if (!g_Data.isRequesting && g_Data.connStatus == 0) {
				saturn->GetIMFDClient()->StopBurnDataRequests();	
				g_Data.hasError = false;
				g_Data.isRequestingManually = false;
			}
		} else if (key == OAPI_KEY_C) {
			if (saturn || lem) {
				if (g_Data.updateClockReady == 0 && g_Data.uplinkDataReady == 0) {
					g_Data.updateClockReady = 1;
				}
				else if (g_Data.updateClockReady == 1) {
					g_Data.updateClockReady = 2;
					if(!saturn){ g_Data.uplinkLEM = 1; }else{ g_Data.uplinkLEM = 0; } // LEM flag
					UpdateClock();
				}
			}
			return true;
		} else if (key == OAPI_KEY_S) {
			if (g_Data.uplinkDataReady == 0) {
				bool SourceInput (void *id, char *str, void *data);
				oapiOpenInputBox("Set Source", SourceInput, 0, 20, (void*)this);
			}
			return true;
		} else if (key == OAPI_KEY_R) {
			if (g_Data.uplinkDataReady == 0) {
				bool ReferencePlanetInput (void *id, char *str, void *data);
				oapiOpenInputBox("Set Reference", ReferencePlanetInput, 0, 20, (void*)this);
			}
			return true;
		} else if (key == OAPI_KEY_F) {
			if (saturn || lem) {
				if (g_Data.uplinkDataReady == 0 && g_Data.updateClockReady == 0) {
					g_Data.uplinkDataReady = 1;
					g_Data.uplinkDataType = UPLINK_REFSMMAT;
				}
				else if (g_Data.uplinkDataReady == 1 && g_Data.uplinkDataType == UPLINK_REFSMMAT) {
					if(!saturn){ g_Data.uplinkLEM = 1; }else{ g_Data.uplinkLEM = 0; } // LEM flag
					UpLinkRefsmmat();
				}
			}
			return true;
		} else if (key == OAPI_KEY_T) {
			if (g_Data.uplinkDataReady == 0) {
				if (g_Data.uplinkSlot == 0) {
					g_Data.uplinkSlot = 1;
				} else {
					g_Data.uplinkSlot = 0;
				}
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
	else if (screen == PROG_LGC)
	{
		if (key == OAPI_KEY_B)
		{
			screen = PROG_NONE;
			InvalidateDisplay();
			InvalidateButtons();
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

	static const DWORD btkeyNone[12] = { OAPI_KEY_G, OAPI_KEY_E, OAPI_KEY_I, OAPI_KEY_T, OAPI_KEY_L, 0, 0, 0, 0, 0, OAPI_KEY_S, OAPI_KEY_D };
	static const DWORD btkeyGNC[4] = { OAPI_KEY_B, OAPI_KEY_K, OAPI_KEY_E, OAPI_KEY_D };
	static const DWORD btkeyECS[4] = { OAPI_KEY_B, OAPI_KEY_C, OAPI_KEY_P, OAPI_KEY_S };
	static const DWORD btkeyIMFD[3] = { OAPI_KEY_B, OAPI_KEY_R, OAPI_KEY_S };
	static const DWORD btkeyTELE[11] = { OAPI_KEY_B, OAPI_KEY_U, OAPI_KEY_D, OAPI_KEY_L, OAPI_KEY_S, OAPI_KEY_R, OAPI_KEY_I, OAPI_KEY_C, OAPI_KEY_F, 0, OAPI_KEY_T };
	static const DWORD btkeySock[1] = { OAPI_KEY_B };	
	static const DWORD btkeyDEBUG[12] = { 0,0,0,0,0,0,0,0,0,OAPI_KEY_C,OAPI_KEY_F,OAPI_KEY_B };
	static const DWORD btkeyLgc[1] = { OAPI_KEY_B };	

	if (screen == PROG_GNC) {
		if (bt < 4) return ConsumeKeyBuffered (btkeyGNC[bt]);
	} else if (screen == PROG_ECS) {
		if (bt < 4) return ConsumeKeyBuffered (btkeyECS[bt]);
	} else if (screen == PROG_IMFD) {
		if (bt < 3) return ConsumeKeyBuffered (btkeyIMFD[bt]);		
	} else if (screen == PROG_TELE) {
		if (bt < 11) return ConsumeKeyBuffered (btkeyTELE[bt]);
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
	else if (screen == PROG_LGC)
	{
		if (bt < 1) return ConsumeKeyBuffered (btkeyLgc[bt]);
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

	if (!saturn && !lem) {
		SetTextColor (hDC, RGB(255, 0, 0));
		TextOut(hDC, width / 2, (int) (height * 0.5), "Unsupported vessel", 18); 
		if (!crawler)
			return;
	}

	// Draw mission time
	SetTextColor (hDC, RGB(0, 255, 0));
	TextOut(hDC, width / 2, (int) (height * 0.1), "Ground Elapsed Time", 19);

	double mt = 0;
	if (saturn){ mt = saturn->GetMissionTime(); }
	if (crawler){ mt = crawler->GetMissionTime(); }
	if (lem){ mt = lem->GetMissionTime(); }

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
	if (!saturn && !lem)
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
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.7), "Inclination:", 12);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.8), "Latitude:", 9);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.85), "Longitude:", 10);

		OBJHANDLE planet;
		ELEMENTS elem;
		char planetName[255];
		VECTOR3 vel, hvel;
		double vvel = 0, apDist, peDist, lat, lon, radius;

		if (saturn) {
			planet = saturn->GetGravityRef();
			saturn->GetRelativeVel(planet, vel); 
			if (saturn->GetHorizonAirspeedVector(hvel)) {
				vvel = hvel.y * 3.2808399;
			}
			saturn->GetApDist(apDist);
			saturn->GetPeDist(peDist);
			saturn->GetEquPos(lon, lat, radius);
			saturn->GetElements(planet, elem, 0, 0, FRAME_EQU);
		} else if (lem) {
			planet = lem->GetGravityRef();
			lem->GetRelativeVel(planet, vel); 
			if (lem->GetHorizonAirspeedVector(hvel)) {
				vvel = hvel.y * 3.2808399;
			}
			lem->GetApDist(apDist);
			lem->GetPeDist(peDist);
			lem->GetEquPos(lon, lat, radius);
			lem->GetElements(planet, elem, 0, 0, FRAME_EQU);
		}

		oapiGetObjectName(planet, planetName, 16);
		if (strcmp(planetName, "Earth") == 0) {
			apDist -= 6.373338e6;
			peDist -= 6.373338e6;
		} else {
			apDist -= 1.73809e6;
			peDist -= 1.73809e6;
		}

		SetTextAlign (hDC, TA_RIGHT);
		sprintf(buffer, "%.0lf ft/s", length(vel) * 3.2808399);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.4), buffer, strlen(buffer));
		sprintf(buffer, "%.0lf ft/s", vvel);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.45), buffer, strlen(buffer));
		if(saturn){ sprintf(buffer, "%.1lf nm  ", saturn->GetAltitude() * 0.000539957); }
		if(lem){    sprintf(buffer, "%.1lf nm  ", lem->GetAltitude() * 0.000539957); }
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.5), buffer, strlen(buffer));
		sprintf(buffer, "%.1lf nm  ", apDist * 0.000539957);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.6), buffer, strlen(buffer));
		sprintf(buffer, "%.1lf nm  ", peDist * 0.000539957);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.65), buffer, strlen(buffer));
		sprintf(buffer, "%.2lf°   ", elem.i * DEG);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.7), buffer, strlen(buffer));
		sprintf(buffer, "%.2lf°   ", lat * DEG);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.8), buffer, strlen(buffer));
		sprintf(buffer, "%.2lf°   ", lon * DEG);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.85), buffer, strlen(buffer));

		if (g_Data.killrot) {
			SetTextColor (hDC, RGB(255, 0, 0));
			SetTextAlign (hDC, TA_CENTER);
			TextOut(hDC, width / 2, (int) (height * 0.9), "*** KILL ROTATION ACTIVE ***", 28);
		}

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
		if (g_Data.isRequestingManually && saturn->GetIMFDClient()->IsBurnDataValid()) {
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.35), "REQUESTING", 10);	
		} else if ( g_Data.isRequestingManually && !saturn->GetIMFDClient()->IsBurnDataValid()) {
			SetTextColor (hDC, RGB(255, 0, 0));
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.35), "NO DATA", 7);	
			SetTextColor (hDC, RGB(0, 255, 0));
		} else {
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.35), "NONE", 4);
		}
		if (saturn->GetIMFDClient()->IsBurnDataValid() && g_Data.isRequestingManually) {
			IMFD_BURN_DATA bd = saturn->GetIMFDClient()->GetBurnData();
			if (bd.p30mode && !bd.impulsive) {
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
		SetTextAlign (hDC, TA_LEFT);
		sprintf(buffer, "Telemetry: %s", debugWinsock);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.30), "Telemetry:", 10);
		TextOut(hDC, (int) (width * 0.6), (int) (height * 0.30), debugWinsock, strlen(debugWinsock));
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.35), "IMFD Burn Data:", 15);
		if (saturn){
			if ( g_Data.isRequestingManually && saturn->GetIMFDClient()->IsBurnDataValid()) {
				TextOut(hDC, (int) (width * 0.6), (int) (height * 0.35), "REQUESTING", 10);
			} else if ( g_Data.isRequestingManually && !saturn->GetIMFDClient()->IsBurnDataValid()) {
				SetTextColor (hDC, RGB(255, 0, 0));
				TextOut(hDC, (int) (width * 0.6), (int) (height * 0.35), "NO DATA", 7);	
				SetTextColor (hDC, RGB(0, 255, 0));
			} else {
				TextOut(hDC, (int) (width * 0.6), (int) (height * 0.35), "NONE", 4);
			}
		} else {
			TextOut(hDC, (int) (width * 0.6), (int) (height * 0.35), "N/A", 3);
		}
		if (g_Data.hasError) {
			if (saturn && saturn->GetIMFDClient()->IsBurnDataValid()) {
				IMFD_BURN_DATA bd = saturn->GetIMFDClient()->GetBurnData();				
				if (!bd.p30mode || bd.impulsive)
					g_Data.errorMessage ="IMFD not in Off-Axis, P30 Mode";
				else if (bd.LAP_MJD==0.0)
					g_Data.errorMessage ="IMFD does not have Lambert Data";
			} else {
				g_Data.errorMessage ="IMFD data not available";
			}
			SetTextAlign (hDC, TA_CENTER);
			SetTextColor (hDC, RGB(255, 0, 0));
			TextOut(hDC, width / 2, (int) (height * 0.45), "ERROR", 5);
			TextOut(hDC, width / 2, (int) (height * 0.5), g_Data.errorMessage, strlen(g_Data.errorMessage));
		} else if (g_Data.uplinkDataReady == 1 || g_Data.updateClockReady == 1) {
			if (lem) {
				SetTextAlign (hDC, TA_CENTER);
				sprintf(buffer, "Checklist");
				TextOut(hDC, width / 2, (int) (height * 0.45), buffer, strlen(buffer));
				SetTextAlign (hDC, TA_LEFT);
				sprintf(buffer, "LGC: IDLE (P00 DESIRED)");
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.55), buffer, strlen(buffer));
				sprintf(buffer, "P12: UPDATA LINK - DATA (down)");
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.60), buffer, strlen(buffer));
				sprintf(buffer, "P11: UP DATA LINK CB - IN");
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.65), buffer, strlen(buffer));
				SetTextAlign (hDC, TA_CENTER);				
			} else {
				SetTextAlign (hDC, TA_CENTER);
				sprintf(buffer, "Checklist");
				TextOut(hDC, width / 2, (int) (height * 0.45), buffer, strlen(buffer));
				SetTextAlign (hDC, TA_LEFT);
				sprintf(buffer, "DSKY - V37E 00E");
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.55), buffer, strlen(buffer));
				sprintf(buffer, "UPTLM CM - ACCEPT (up)   2, 122");
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.60), buffer, strlen(buffer));
				sprintf(buffer, "UP TLM - DATA (up)            3");
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.65), buffer, strlen(buffer));
				sprintf(buffer, "PCM BIT RATE - HIGH (up)      3");
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.7), buffer, strlen(buffer));
				SetTextAlign (hDC, TA_CENTER);
			}
			if (g_Data.uplinkDataReady == 1) {
				if (g_Data.uplinkDataType == UPLINK_SV)
					sprintf(buffer, "Press SV to start upload");
				else if (g_Data.uplinkDataType == UPLINK_P30)
					sprintf(buffer, "Press P30 to start upload");
				else if (g_Data.uplinkDataType == UPLINK_P31)
					sprintf(buffer, "Press P31 to start upload");
				else if (g_Data.uplinkDataType == UPLINK_REFSMMAT) {
					if( saturn )
						sprintf(buffer, "LS Desired REFSMMAT");
					else {
						if ( lem->GetAltitude() < 1 ) // Landed on the moon, Upload the Lift off REFSMMAT
							sprintf(buffer, "LiftOff REFSMMAT");
						else
							sprintf(buffer, "LS REFSMMAT");
					}
					TextOut(hDC, width / 2, (int) (height * 0.75), buffer, strlen(buffer));
					sprintf(buffer, "Press LS to start upload");
				}
			}
			else
				sprintf(buffer, "Press CLK to start upload");
			TextOut(hDC, width / 2, (int) (height * 0.8), buffer, strlen(buffer));
		}
		else if(g_Data.uplinkDataReady == 2) {
			double linepos = 0.4;
			SetTextAlign (hDC, TA_LEFT);
			sprintf(buffer, "304   %ld", g_Data.emem[0]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
			sprintf(buffer, "305   %ld", g_Data.emem[1]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
			sprintf(buffer, "306   %ld", g_Data.emem[2]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
			sprintf(buffer, "307   %ld", g_Data.emem[3]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
			sprintf(buffer, "310   %ld", g_Data.emem[4]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
			sprintf(buffer, "311   %ld", g_Data.emem[5]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
			sprintf(buffer, "312   %ld", g_Data.emem[6]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
			sprintf(buffer, "313   %ld", g_Data.emem[7]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
			sprintf(buffer, "314   %ld", g_Data.emem[8]);		
			TextOut(hDC, (int) (width * 0.1), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
			sprintf(buffer, "315   %ld", g_Data.emem[9]);
			TextOut(hDC, (int) (width * 0.1), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));		
			linepos = 0.4;
			if (g_Data.uplinkDataType != UPLINK_P30) {
				sprintf(buffer, "316   %ld", g_Data.emem[10]);
				TextOut(hDC, (int) (width * 0.55), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
				sprintf(buffer, "317   %ld", g_Data.emem[11]);
				TextOut(hDC, (int) (width * 0.55), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
				sprintf(buffer, "320   %ld", g_Data.emem[12]);
				TextOut(hDC, (int) (width * 0.55), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
			}
			if (g_Data.uplinkDataType >= UPLINK_SV) {
				sprintf(buffer, "321   %ld", g_Data.emem[13]);
				TextOut(hDC, (int) (width * 0.55), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
				sprintf(buffer, "322   %ld", g_Data.emem[14]);
				TextOut(hDC, (int) (width * 0.55), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
				sprintf(buffer, "323   %ld", g_Data.emem[15]);
				TextOut(hDC, (int) (width * 0.55), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
				sprintf(buffer, "324   %ld", g_Data.emem[16]);
				TextOut(hDC, (int) (width * 0.55), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
			}
			if (g_Data.uplinkDataType >= UPLINK_REFSMMAT) {
				sprintf(buffer, "325   %ld", g_Data.emem[17]);
				TextOut(hDC, (int) (width * 0.55), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
				sprintf(buffer, "326   %ld", g_Data.emem[18]);
				TextOut(hDC, (int) (width * 0.55), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
				sprintf(buffer, "327   %ld", g_Data.emem[19]);
				TextOut(hDC, (int) (width * 0.55), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));

			}
		}
		SetTextAlign (hDC, TA_LEFT);
		SetTextColor (hDC, RGB(128, 128, 128));
		oapiGetObjectName(g_Data.vessel->GetHandle(), buffer, 100);
		TextOut(hDC, (int) (width * 0.05), (int) (height * 0.95), buffer, strlen(buffer));
		SetTextAlign (hDC, TA_CENTER);
		oapiGetObjectName(g_Data.planet, buffer, 100);
		TextOut(hDC, (int) (width * 0.5), (int) (height * 0.95), buffer, strlen(buffer));
		SetTextAlign (hDC, TA_RIGHT);
		if (g_Data.uplinkSlot == 0) {
			sprintf(buffer, "This");
		} else {
			sprintf(buffer, "Other");
		}		
		TextOut(hDC, (int) (width * 0.95), (int) (height * 0.95), buffer, strlen(buffer));

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
	// Draw LGC Setup screen
	else if (screen == PROG_LGC) {
		OBJHANDLE object;
		VESSEL *vessel;
		TextOut(hDC, width / 2, (int) (height * 0.3), "LGC Docked Init Data", 20);
		// What's our status?
		if(saturn == NULL){
			// TextOut(hDC, width / 2, (int) (height * 0.4), "We are in the LM", 16);
			// We need to find the CM.
			// In all of the scenarios in which the LM is present and selectable, the CM is already separated from the S4B.
			object = oapiGetVesselByName("Gumdrop"); // A9
			if(object == NULL){
				object = oapiGetVesselByName("Charlie Brown"); // A10
			}
			if(object == NULL){
				object = oapiGetVesselByName("Columbia"); // A11
			}
			if(object == NULL){
				object = oapiGetVesselByName("Yankee Clipper"); // A12
			}
			if(object == NULL){
				object = oapiGetVesselByName("Odyssey"); // A13
			}
			if(object == NULL){
				object = oapiGetVesselByName("Kitty Hawk"); // A14
			}
			if(object == NULL){
				object = oapiGetVesselByName("Endeavour"); // A15
			}
			if(object == NULL){
				object = oapiGetVesselByName("Casper"); // A16
			}
			if(object == NULL){
				object = oapiGetVesselByName("America"); // A17
			}
			if(object != NULL){
				vessel = oapiGetVesselInterface(object);
				// If some jerk names the S4B a CM name instead this will probably screw up, but who would do that?
				if (!stricmp(vessel->GetClassName(), "ProjectApollo\\Saturn5") ||
					!stricmp(vessel->GetClassName(), "ProjectApollo/Saturn5") ||
					!stricmp(vessel->GetClassName(), "ProjectApollo\\Saturn1b") ||
					!stricmp(vessel->GetClassName(), "ProjectApollo/Saturn1b")) {
						saturn = (Saturn *)vessel;
						// Is this a vAGC CM?
						if(saturn->IsVirtualAGC() == FALSE){
							TextOut(hDC, width / 2, (int) (height * 0.4), "Only for Virtual AGC mode", 25);
						}else{
							// Yes
							VECTOR3 CMattitude,LMattitude;
							unsigned short tephem[3]; 
							// Obtain CM attitude.
							// It would be better to call GetTotalAttitude() but for some reason VC++ refuses to link it properly. Sigh.
							CMattitude.x = saturn->imu.Gimbal.X*DEG; // OUTER
							CMattitude.y = saturn->imu.Gimbal.Y*DEG; // INNER
							CMattitude.z = saturn->imu.Gimbal.Z*DEG; // MIDDLE
							// Docking tunnel angle is assumed to be zero.
							LMattitude.x = 300-CMattitude.x; if(LMattitude.x < 0){ LMattitude.x += 360; }
							LMattitude.y = 180+CMattitude.y; if(LMattitude.y > 360){ LMattitude.y -= 360; }
							LMattitude.z = 360-CMattitude.z; if(LMattitude.z < 0){ LMattitude.x += 360; }
							// We should obtain and print CSM time, but...
							// the update delay of the MFD makes time correction less than one second a pain at best, so we won't bother for now.
							// Just initialize from the mission timer.
							// Obtain TEPHEM
							tephem[0] = saturn->agc.vagc.Erasable[0][01706];
							tephem[1] = saturn->agc.vagc.Erasable[0][01707];
							tephem[2] = saturn->agc.vagc.Erasable[0][01710];
							sprintf(buffer,"TEPHEM: %05o %05o %05o",tephem[0],tephem[1],tephem[2]);
							TextOut(hDC, width / 2, (int) (height * 0.4), buffer, strlen(buffer));
							// Format gimbal angles and print them
							sprintf(buffer, "CSM O/I/M: %3.2f %3.2f %3.2f", CMattitude.x, CMattitude.y, CMattitude.z);
							TextOut(hDC, width / 2, (int) (height * 0.45), buffer, strlen(buffer));
							sprintf(buffer, "LM O/I/M: %3.2f %3.2f %3.2f", LMattitude.x, LMattitude.y, LMattitude.z);
							TextOut(hDC, width / 2, (int) (height * 0.5), buffer, strlen(buffer));

						}
						saturn = NULL; // Clobber
				}
			}
		}else{
			TextOut(hDC, width / 2, (int) (height * 0.4), "Do this from the LM", 19);
		}
		/*
		sprintf(buffer, "Socket: %i", close_Socket);
		TextOut(hDC, width / 2, (int) (height * 0.4), buffer, strlen(buffer));
		*/
	}

}

// =============================================================================================
// Convert from J2000 Ecliptic to Basic Reference Coordinate System (Earth Mean Equator of Date)
// param mjd - Modified Julian Date of the epoch of BRCS

MATRIX3 _MRx(double a) 
{
	double ca = cos(a), sa = sin(a);
	return _M(1.0, 0, 0, 0, ca, sa, 0, -sa, ca);
}

MATRIX3 _MRz(double a) 
{ 
	double ca = cos(a), sa = sin(a); 
	return _M(ca, sa, 0, -sa, ca, 0, 0, 0, 1.0);
}

MATRIX3 J2000EclToBRCS(double mjd)
{
	double t1 = (mjd - 51544.5) / 36525.0;
	double t2 = t1*t1;
	double t3 = t2*t1;

	t1 *= 4.848136811095359e-6;
	t2 *= 4.848136811095359e-6;
	t3 *= 4.848136811095359e-6;
	
	double i = 2004.3109*t1 - 0.42665*t2 - 0.041833*t3;
	double r = 2306.2181*t1 + 0.30188*t2 + 0.017998*t3;
	double L = 2306.2181*t1 + 1.09468*t2 + 0.018203*t3;

	double rot = -r - PI05;
	double lan = PI05 - L;
	double inc = i;
	double obl = 0.40909280422;

	return mul( mul(_MRz(rot),_MRx(inc)), mul(_MRz(lan),_MRx(-obl)) );
}

void ProjectApolloMFD::UpLinkRefsmmat ()
{
		g_Data.emem[0] = 24;
		// 
		// If in CSM or in LM landed, upload Desired RESFMMAT
		//
		if( saturn || (lem && ( lem->GetAltitude() < 1) ) ) {
				g_Data.emem[1] = 306; // XSMD	
				g_Data.uplinkDataType = UPLINK_REFSMMAT;
		} else {
			//
			// Upload REFSMMAT  ( LM powerup )
			g_Data.uplinkDataType = UPLINK_REFSMMAT;
			g_Data.emem[1] = 1733;	// Luminary99 REFSMMAT
		}

		if (lem && ( lem->GetAltitude() < 1) ) // In LEM and Landed on the moon, Upload the Lift off REFSMMAT
			for(int i = 0 ; i<18 ; i++)
				g_Data.emem[2+i] = apolloLandSiteLiftOffRefsmmat[saturn->GetApolloNo()-11][i];
		else
			for(int i = 0 ; i<18 ; i++)
				g_Data.emem[2+i] = apolloLandSiteRefsmmat[saturn->GetApolloNo()-11][i];
		g_Data.uplinkDataType = UPLINK_REFSMMAT;
		g_Data.uplinkDataReady = 2;
		UplinkData(); // Go for uplink
}

void ProjectApolloMFD::GetStateVector (void)
{
	double get;
	VECTOR3 pos, vel;

	if (saturn){ get = fabs(saturn->GetMissionTime()); }
	if (crawler){get = fabs(crawler->GetMissionTime()); }
	if (lem){    get = fabs(lem->GetMissionTime()); }

	g_Data.vessel->GetRelativePos(g_Data.planet, pos); 
	g_Data.vessel->GetRelativeVel(g_Data.planet, vel);
	
	OBJHANDLE hMoon  = oapiGetGbodyByName("Moon");
	OBJHANDLE hEarth = oapiGetGbodyByName("Earth");

	MATRIX3 Rot = J2000EclToBRCS(40222.525);
	
	pos = mul(Rot, _V(pos.x, pos.z, pos.y));
	vel = mul(Rot, _V(vel.x, vel.z, vel.y)) * 0.01;
	
	if (g_Data.planet==hMoon) {

		g_Data.emem[0] = 21;
		g_Data.emem[1] = 1501;	

		//if (g_Data.vessel->GetHandle()==oapiGetFocusObject()) 
		if (g_Data.uplinkSlot == 0)
			g_Data.emem[2] = 2;
		else 
			g_Data.emem[2] = 77775;	// Octal coded decimal
		
		g_Data.emem[3]  = DoubleToBuffer(pos.x, 27, 1);
		g_Data.emem[4]  = DoubleToBuffer(pos.x, 27, 0);
		g_Data.emem[5]  = DoubleToBuffer(pos.y, 27, 1);
		g_Data.emem[6]  = DoubleToBuffer(pos.y, 27, 0);
		g_Data.emem[7]  = DoubleToBuffer(pos.z, 27, 1);
		g_Data.emem[8]  = DoubleToBuffer(pos.z, 27, 0);
		g_Data.emem[9]  = DoubleToBuffer(vel.x, 5, 1);
		g_Data.emem[10] = DoubleToBuffer(vel.x, 5, 0);
		g_Data.emem[11] = DoubleToBuffer(vel.y, 5, 1);
		g_Data.emem[12] = DoubleToBuffer(vel.y, 5, 0);
		g_Data.emem[13] = DoubleToBuffer(vel.z, 5, 1);
		g_Data.emem[14] = DoubleToBuffer(vel.z, 5, 0);	
		g_Data.emem[15] = DoubleToBuffer(get*100.0, 28, 1);
		g_Data.emem[16] = DoubleToBuffer(get*100.0, 28, 0);

		g_Data.uplinkDataReady = 2;
		UplinkData(); // Go for uplink
		return;
	}

	if (g_Data.planet==hEarth) {

		g_Data.emem[0] = 21;
		g_Data.emem[1] = 1501;

		//if (g_Data.vessel->GetHandle()==oapiGetFocusObject()) 
		if (g_Data.uplinkSlot == 0)
			g_Data.emem[2] = 1;
		else 
			g_Data.emem[2] = 77776;	// Octal coded decimal
		
		g_Data.emem[3]  = DoubleToBuffer(pos.x, 29, 1);
		g_Data.emem[4]  = DoubleToBuffer(pos.x, 29, 0);
		g_Data.emem[5]  = DoubleToBuffer(pos.y, 29, 1);
		g_Data.emem[6]  = DoubleToBuffer(pos.y, 29, 0);
		g_Data.emem[7]  = DoubleToBuffer(pos.z, 29, 1);
		g_Data.emem[8]  = DoubleToBuffer(pos.z, 29, 0);
		g_Data.emem[9]  = DoubleToBuffer(vel.x, 7, 1);
		g_Data.emem[10] = DoubleToBuffer(vel.x, 7, 0);
		g_Data.emem[11] = DoubleToBuffer(vel.y, 7, 1);
		g_Data.emem[12] = DoubleToBuffer(vel.y, 7, 0);
		g_Data.emem[13] = DoubleToBuffer(vel.z, 7, 1);
		g_Data.emem[14] = DoubleToBuffer(vel.z, 7, 0);
		g_Data.emem[15] = DoubleToBuffer(get*100.0, 28, 1);
		g_Data.emem[16] = DoubleToBuffer(get*100.0, 28, 0);

		g_Data.uplinkDataReady = 2;
		UplinkData(); // Go for uplink
		return;
	}

	g_Data.uplinkDataReady = 0; // Abort uplink
}

void ProjectApolloMFD::IMFDP30Uplink(void)
{
	double get;
	IMFD_BURN_DATA bd;
	IMFD_BURN_DATA *pbd;

	if (g_Data.isRequestingManually) {
		bd = saturn->GetIMFDClient()->GetBurnData();
		pbd = &bd;
	}
	else pbd = &g_Data.burnData;
	
	if (pbd->DataMJD==0.0) {
		g_Data.uplinkDataReady = 0;		// Abort uplink
		return;
	}

	if (saturn){ get = fabs(saturn->GetMissionTime()); }
	if(crawler){ get = fabs(crawler->GetMissionTime()); }
	if(lem){     get = fabs(lem->GetMissionTime()); }

	double liftoff = oapiGetSimMJD()-get/86400.0;
	double getign  = (pbd->IgnMJD - liftoff) * 86400.0;

	g_Data.emem[0] = 12;
	g_Data.emem[1] = 3404;
	g_Data.emem[2] = DoubleToBuffer(pbd->_dV_LVLH.x/100.0, 7, 1);
	g_Data.emem[3] = DoubleToBuffer(pbd->_dV_LVLH.x/100.0, 7, 0);
	g_Data.emem[4] = DoubleToBuffer(pbd->_dV_LVLH.y/100.0, 7, 1);
	g_Data.emem[5] = DoubleToBuffer(pbd->_dV_LVLH.y/100.0, 7, 0);
	g_Data.emem[6] = DoubleToBuffer(pbd->_dV_LVLH.z/100.0, 7, 1);
	g_Data.emem[7] = DoubleToBuffer(pbd->_dV_LVLH.z/100.0, 7, 0);
	g_Data.emem[8] = DoubleToBuffer(getign*100.0, 28, 1);
	g_Data.emem[9] = DoubleToBuffer(getign*100.0, 28, 0);

	g_Data.uplinkDataReady = 2;
	UplinkData(); // Go for uplink
}

void ProjectApolloMFD::IMFDP31Uplink(void)
{
	double get;
	IMFD_BURN_DATA bd;
	IMFD_BURN_DATA *pbd;

	if (g_Data.isRequestingManually) {
		bd = saturn->GetIMFDClient()->GetBurnData();
		pbd = &bd;
	}
	else pbd = &g_Data.burnData;
	
	if (pbd->DataMJD==0.0 || pbd->LAP_MJD==0.0) {
		g_Data.uplinkDataReady = 0;		// Abort uplink
		return;
	}

	if (saturn){ get = fabs(saturn->GetMissionTime()); }
	if(crawler){ get = fabs(crawler->GetMissionTime()); }
	if(lem){     get = fabs(lem->GetMissionTime()); }

	double liftoff    = oapiGetSimMJD()-get/86400.0;
	double getign     = (pbd->IgnMJD - liftoff) * 86400.0;
	double delta      = (pbd->LAP_MJD - pbd->IgnMJD) * 86400.0;
	double ecsteer    = 0.9;  // Cross-product steering constant
	
	VECTOR3 _L  = pbd->_LAP;

	MATRIX3 Rot = J2000EclToBRCS(40222.525);

	_L = mul(Rot, _V(_L.x, _L.z, _L.y));

	g_Data.emem[0] = 15;
	g_Data.emem[1] = 3412;
	g_Data.emem[2] = DoubleToBuffer(getign*100.0, 28, 1);
	g_Data.emem[3] = DoubleToBuffer(getign*100.0, 28, 0);
	g_Data.emem[4] = DoubleToBuffer(_L.x, 29, 1);
	g_Data.emem[5] = DoubleToBuffer(_L.x, 29, 0);
	g_Data.emem[6] = DoubleToBuffer(_L.y, 29, 1);
	g_Data.emem[7] = DoubleToBuffer(_L.y, 29, 0);
	g_Data.emem[8] = DoubleToBuffer(_L.z, 29, 1);
	g_Data.emem[9] = DoubleToBuffer(_L.z, 29, 0);
	g_Data.emem[10] = DoubleToBuffer(delta*100.0, 28, 1);
	g_Data.emem[11] = DoubleToBuffer(delta*100.0, 28, 0);
	g_Data.emem[12] = DoubleToBuffer(ecsteer, 2, 1);

	g_Data.uplinkDataReady = 2;
	UplinkData(); // Go for uplink
}

void ProjectApolloMFD::WriteStatus (FILEHANDLE scn) const
{
	oapiWriteScenario_int(scn, "SCREEN", screen);
	oapiWriteScenario_int(scn, "PROGNO", g_Data.prog);
	oapiWriteScenario_int(scn, "PROGSTATE", g_Data.progState);
	if (g_Data.progVessel)
		oapiWriteScenario_string(scn, "PROGVESSEL", g_Data.progVessel->GetName());
	if (g_Data.gorpVessel)
		oapiWriteScenario_string(scn, "GORPVESSEL", g_Data.gorpVessel->GetName());
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
		if (!strnicmp (line, "GORPVESSEL", 10)) {
			sscanf (line + 10, "%s", name);
			OBJHANDLE h = oapiGetVesselByName(name);
			if (h != NULL)
				g_Data.gorpVessel = (LEM *) oapiGetVesselInterface(h);
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
