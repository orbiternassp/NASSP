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

  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"

#include "math.h"
#include "windows.h"
#include "nasspsound.h"
#include "soundlib.h"
#include "tracer.h"
#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "lemcomputer.h"
#include "IMU.h"
#include "saturn.h"
#include "LEM.h"
#include "Crawler.h"
#include "papi.h"
#include "OrbMech.h"
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

#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

#define  UPLINK_SV				0


// Time to ejection when the IU is programmed
#define IUSTARTTIME 900

static struct ProjectApolloMFDData {  // global data storage
	int prog;	
	Saturn *progVessel;
	LEM *gorpVessel;

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

	VECTOR3 V42angles;

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
	g_Data.progVessel = NULL;
	g_Data.gorpVessel = NULL;
	g_Data.uplinkLEM = 0;
	g_Data.uplinkSlot = 0;

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
	g_Data.V42angles = _V(0, 0, 0);
	g_Data.killrot = 0;
}

void ProjectApolloMFDopcDLLExit (HINSTANCE hDLL)
{
	// Unregister the custom MFD mode when the module is unloaded
	oapiUnregisterMFDMode (g_MFDmode);
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

	if (g_Data.progVessel && g_Data.killrot && g_Data.progVessel == g_Data.vessel) {
		g_Data.progVessel->SetAngularVel(_V(0, 0, 0));
	}

	if (g_Data.gorpVessel && g_Data.killrot && g_Data.gorpVessel == g_Data.vessel) {
		g_Data.gorpVessel->SetAngularVel(_V(0, 0, 0));
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
	static char *labelTELE[11] = {"BCK", "SV", "", "", "SRC", "REF", "", "CLK", "", "", "SLT"};
	static char *labelSOCK[1] = {"BCK"};	
	static char *labelDEBUG[12] = {"","","","","","","","","","CLR","FRZ","BCK"};
	static char *labelLGC[5] = {"BCK", "", "", "", "V42"};

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
		return (bt < 3 ? labelIMFDTliStop[bt] : 0);
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
		return (bt < 5 ? labelLGC[bt] : 0);
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
		{ 0,0,0 },
		{ 0,0,0 },
		{"Change Source",0,'S'},
		{"Change Reference Body", 0, 'R'},
		{ 0,0,0 },
		{"Clock Update", 0, 'C'},
		{0,0,0},
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
	static const MFDBUTTONMENU mnuLGC[5] = {
		{"Back", 0, 'B'},
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ "Calculate V42 Angles", 0, 'F' }
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
		if (menu) *menu = mnuIMFDTliStop;
		return 3;
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
		return 5;
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
			else if (lem)
				lem->VirtualAGCCoreDump();
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

		} else if (key == OAPI_KEY_R) {						

		} else if (key == OAPI_KEY_S) {
			g_Data.prog = PROG_IMFDTLI;
			g_Data.progVessel = saturn;
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
		else if (key == OAPI_KEY_F)
		{
			CalculateV42Angles();
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
	static const DWORD btkeyTELE[11] = { OAPI_KEY_B, OAPI_KEY_U, 0, 0, OAPI_KEY_S, OAPI_KEY_R, 0, OAPI_KEY_C, 0, 0, OAPI_KEY_T };
	static const DWORD btkeySock[1] = { OAPI_KEY_B };	
	static const DWORD btkeyDEBUG[12] = { 0,0,0,0,0,0,0,0,0,OAPI_KEY_C,OAPI_KEY_F,OAPI_KEY_B };
	static const DWORD btkeyLgc[5] = { OAPI_KEY_B, 0, 0, 0, OAPI_KEY_F };

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
		if (bt < 5) return ConsumeKeyBuffered (btkeyLgc[bt]);
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

		if (saturn)
		{

			SetTextAlign(hDC, TA_LEFT);
			TextOut(hDC, (int)(width * 0.1), (int)(height * 0.4), "Crew status:", 12);
			TextOut(hDC, (int)(width * 0.1), (int)(height * 0.45), "Crew number:", 12);

			ECSStatus ecs;
			saturn->GetECSStatus(ecs);

			SetTextAlign(hDC, TA_CENTER);
			if (ecs.crewStatus == ECS_CREWSTATUS_OK) {
				TextOut(hDC, (int)(width * 0.7), (int)(height * 0.4), "OK", 2);
			}
			else if (ecs.crewStatus == ECS_CREWSTATUS_CRITICAL) {
				SetTextColor(hDC, RGB(255, 255, 0));
				TextOut(hDC, (int)(width * 0.7), (int)(height * 0.4), "CRITICAL", 8);
				SetTextColor(hDC, RGB(0, 255, 0));
			}
			else {
				SetTextColor(hDC, RGB(255, 0, 0));
				TextOut(hDC, (int)(width * 0.7), (int)(height * 0.4), "DEAD", 4);
				SetTextColor(hDC, RGB(0, 255, 0));
			}

			sprintf(buffer, "%d", ecs.crewNumber);
			TextOut(hDC, (int)(width * 0.7), (int)(height * 0.45), buffer, strlen(buffer));

			TextOut(hDC, (int)(width * 0.5), (int)(height * 0.525), "Glycol Coolant Loops", 20);
			TextOut(hDC, (int)(width * 0.6), (int)(height * 0.6), "Prim.", 5);
			TextOut(hDC, (int)(width * 0.8), (int)(height * 0.6), "Sec.", 4);

			SetTextAlign(hDC, TA_LEFT);
			TextOut(hDC, (int)(width * 0.1), (int)(height * 0.6), "Heating:", 8);
			TextOut(hDC, (int)(width * 0.1), (int)(height * 0.65), "Actual:", 7);
			TextOut(hDC, (int)(width * 0.1), (int)(height * 0.7), "Test:", 5);
			TextOut(hDC, (int)(width * 0.1), (int)(height * 0.8), "Total:", 6);

			SetTextAlign(hDC, TA_CENTER);
			sprintf(buffer, "%.0lfW", ecs.PrimECSHeating);
			TextOut(hDC, (int)(width * 0.6), (int)(height * 0.65), buffer, strlen(buffer));
			sprintf(buffer, "%.0lfW", ecs.PrimECSTestHeating);
			TextOut(hDC, (int)(width * 0.6), (int)(height * 0.7), buffer, strlen(buffer));
			sprintf(buffer, "%.0lfW", ecs.PrimECSHeating + ecs.PrimECSTestHeating);
			TextOut(hDC, (int)(width * 0.6), (int)(height * 0.8), buffer, strlen(buffer));
			sprintf(buffer, "%.0lfW", ecs.SecECSHeating);
			TextOut(hDC, (int)(width * 0.8), (int)(height * 0.65), buffer, strlen(buffer));
			sprintf(buffer, "%.0lfW", ecs.SecECSTestHeating);
			TextOut(hDC, (int)(width * 0.8), (int)(height * 0.7), buffer, strlen(buffer));
			sprintf(buffer, "%.0lfW", ecs.SecECSHeating + ecs.SecECSTestHeating);
			TextOut(hDC, (int)(width * 0.8), (int)(height * 0.8), buffer, strlen(buffer));

			MoveToEx(hDC, (int)(width * 0.5), (int)(height * 0.775), 0);
			LineTo(hDC, (int)(width * 0.9), (int)(height * 0.775));

		}
		else
		{
			TextOut(hDC, width / 2, (int)(height * 0.4), "LM ECS not implemented yet", 26);
		}
	// Draw IMFD
	} else if (screen == PROG_IMFD) {
		TextOut(hDC, width / 2, (int) (height * 0.3), "IMFD Burn Data", 14);
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.35), "Status:", 7);
		SetTextAlign (hDC, TA_CENTER);

		SetTextAlign (hDC, TA_CENTER);
		TextOut(hDC, width / 2, (int) (height * 0.7), "S-IVB Burn Program", 18);
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.75), "Status:", 7);

		SetTextAlign (hDC, TA_CENTER);
	}
	//Draw Telemetry
	else if (screen == PROG_TELE) {
		SetTextAlign (hDC, TA_LEFT);
		sprintf(buffer, "Telemetry: %s", debugWinsock);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.30), "Telemetry:", 10);
		TextOut(hDC, (int) (width * 0.6), (int) (height * 0.30), debugWinsock, strlen(debugWinsock));

		if (g_Data.uplinkDataReady == 1 || g_Data.updateClockReady == 1) {
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
			sprintf(buffer, "316   %ld", g_Data.emem[10]);
			TextOut(hDC, (int) (width * 0.55), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
			sprintf(buffer, "317   %ld", g_Data.emem[11]);
			TextOut(hDC, (int) (width * 0.55), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
			sprintf(buffer, "320   %ld", g_Data.emem[12]);
			TextOut(hDC, (int) (width * 0.55), (int) (height * (linepos+=0.05)), buffer, strlen(buffer));
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
			if (object == NULL) {
				object = oapiGetVesselByName("AS-504"); // A9
			}
			if(object == NULL){
				object = oapiGetVesselByName("Charlie Brown"); // A10
			}
			if (object == NULL) {
				object = oapiGetVesselByName("AS-505"); // A10
			}
			if(object == NULL){
				object = oapiGetVesselByName("Columbia"); // A11
			}
			if (object == NULL) {
				object = oapiGetVesselByName("AS-506"); // A11
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

						//Docked IMU Fine Alignment
						TextOut(hDC, width / 2, (int)(height * 0.6), "Docked IMU Fine Alignment", 25);

						sprintf(buffer, "V42: %+07.3f %+07.3f %+07.3f", g_Data.V42angles.x*DEG, g_Data.V42angles.y*DEG, g_Data.V42angles.z*DEG);
						TextOut(hDC, width / 2, (int)(height * 0.7), buffer, strlen(buffer));

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
	double obl = 0.4090928023;

	return mul( mul(_MRz(rot),_MRx(inc)), mul(_MRz(lan),_MRx(-obl)) );
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

	MATRIX3 Rot = J2000EclToBRCS(40221.525);
	
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

void ProjectApolloMFD::WriteStatus (FILEHANDLE scn) const
{
	oapiWriteScenario_int(scn, "SCREEN", screen);
	oapiWriteScenario_int(scn, "PROGNO", g_Data.prog);
	if (g_Data.progVessel)
		oapiWriteScenario_string(scn, "PROGVESSEL", g_Data.progVessel->GetName());
	if (g_Data.gorpVessel)
		oapiWriteScenario_string(scn, "GORPVESSEL", g_Data.gorpVessel->GetName());
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

void ProjectApolloMFD::CalculateV42Angles()
{

	OBJHANDLE object;
	VESSEL *vessel;

	object = oapiGetVesselByName("Gumdrop"); // A9
	if (object == NULL) {
		object = oapiGetVesselByName("AS-504"); // A9
	}
	if (object == NULL) {
		object = oapiGetVesselByName("Charlie Brown"); // A10
	}
	if (object == NULL) {
		object = oapiGetVesselByName("AS-505"); // A10
	}
	if (object == NULL) {
		object = oapiGetVesselByName("Columbia"); // A11
	}
	if (object == NULL) {
		object = oapiGetVesselByName("AS-506"); // A11
	}
	if (object == NULL) {
		object = oapiGetVesselByName("Yankee Clipper"); // A12
	}
	if (object == NULL) {
		object = oapiGetVesselByName("Odyssey"); // A13
	}
	if (object == NULL) {
		object = oapiGetVesselByName("Kitty Hawk"); // A14
	}
	if (object == NULL) {
		object = oapiGetVesselByName("Endeavour"); // A15
	}
	if (object == NULL) {
		object = oapiGetVesselByName("Casper"); // A16
	}
	if (object == NULL) {
		object = oapiGetVesselByName("America"); // A17
	}
	if (object != NULL) {
		vessel = oapiGetVesselInterface(object);
		// If some jerk names the S4B a CM name instead this will probably screw up, but who would do that?
		if (!stricmp(vessel->GetClassName(), "ProjectApollo\\Saturn5") ||
			!stricmp(vessel->GetClassName(), "ProjectApollo/Saturn5") ||
			!stricmp(vessel->GetClassName(), "ProjectApollo\\Saturn1b") ||
			!stricmp(vessel->GetClassName(), "ProjectApollo/Saturn1b")) {
			saturn = (Saturn *)vessel;

			if (saturn && lem)
			{
				VECTOR3 lmn20, csmn20;

				csmn20.x = saturn->imu.Gimbal.X;
				csmn20.y = saturn->imu.Gimbal.Y;
				csmn20.z = saturn->imu.Gimbal.Z;

				lmn20.x = lem->imu.Gimbal.X;
				lmn20.y = lem->imu.Gimbal.Y;
				lmn20.z = lem->imu.Gimbal.Z;

				g_Data.V42angles = OrbMech::finealignLMtoCSM(lmn20, csmn20);
			}
		}
	}
	saturn = NULL;
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
