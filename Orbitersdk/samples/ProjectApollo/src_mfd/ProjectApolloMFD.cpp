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

// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"

#include "math.h"
#include "windows.h"
#include "nasspsound.h"
#include "soundlib.h"
#include "tracer.h"
#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "CSMcomputer.h"
#include "LEMcomputer.h"
#include "IMU.h"
#include "saturn.h"
#include "LEM.h"
#include "Crawler.h"
#include "MCCVessel.h"
#include "MCC.h"
#include "sivb.h"
#include "iu.h"
#include "papi.h"
#include "OrbMech.h"
#include "nassputils.h"
#include <stdio.h>

#include "MFDResource.h"
#include "ProjectApolloMFD.h"

#include <queue>

using namespace nassp;

// ==============================================================
// Global variables

static HINSTANCE g_hDLL;
static int g_MFDmode; // identifier for new MFD mode

#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

#define  UPLINK_SV				0


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
	VESSEL *uplinkVessel;
	int targetnumber;
	int iuUplinkType;
	int iuUplinkSwitSelStage;
	int iuUplinkSwitSelChannel;
	int iuUplinkResult;
	double iuUplinkTimebaseUpdateTime;
	double iuUplinkTIG;
	double iuUplinkDT;
	double iuUplinkPitch;
	double iuUplinkYaw;
	VECTOR3 iuUplinkGenManAtt;
	int iuUplinkGenManType;
	bool lmAlignType;	//true = same REFSMMAT; false = nominal alignments

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

	g_Data.prog = 0;
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
	g_Data.uplinkVessel = NULL;
	g_Data.targetnumber = -1;
	g_Data.iuUplinkType = 0;
	g_Data.iuUplinkSwitSelStage = 0;
	g_Data.iuUplinkSwitSelChannel = 1;
	g_Data.iuUplinkResult = 0;
	g_Data.lmAlignType = true;
	g_Data.iuUplinkTimebaseUpdateTime = 0.0;
	g_Data.iuUplinkTIG = 0.0;
	g_Data.iuUplinkDT = 0.0;
	g_Data.iuUplinkPitch = 0.0;
	g_Data.iuUplinkYaw = 0.0;
	g_Data.iuUplinkGenManAtt = _V(0, 0, 0);
	g_Data.iuUplinkGenManType = 0;
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
		case 'S': // 11-001-101 10-010-011 (code 23)
			cmdbuf[1] = 0315;
			cmdbuf[2] = 0223;
			break;
		case 'T': // 11-010-001 01-110-100 (code 24)
			cmdbuf[1] = 0321;
			cmdbuf[2] = 0164;
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

//LM Ascent Engine Arming Assembly (Apollo 9+10 only)
void uplink_aeaa_cmd(bool arm, bool set)
{
	//arm: true = APS arming, false = AGS guidance
	//set: true = set relays, false = reset relays
	unsigned char cmdbuf[8];

	//3 for LM, 4 for RTC A
	cmdbuf[0] = 034;
	cmdbuf[2] = 034;
	cmdbuf[4] = 034;
	cmdbuf[6] = 034;

	if (arm)
	{
		if (set)
		{
			cmdbuf[1] = 0;
			cmdbuf[3] = 2;
			cmdbuf[5] = 4;
			cmdbuf[7] = 6;
		}
		else
		{
			cmdbuf[1] = 1;
			cmdbuf[3] = 3;
			cmdbuf[5] = 5;
			cmdbuf[7] = 7;
		}
	}
	else
	{
		if (set)
		{
			cmdbuf[1] = 8;
			cmdbuf[3] = 10;
			cmdbuf[5] = 12;
			cmdbuf[7] = 14;
		}
		else
		{
			cmdbuf[1] = 9;
			cmdbuf[3] = 11;
			cmdbuf[5] = 13;
			cmdbuf[7] = 15;
		}
	}
	for (int i = 0; i < 8; i++) {
		g_Data.uplinkBuffer.push(cmdbuf[i]);
	}

	g_Data.uplinkDataReady = 3;
	g_Data.connStatus = 1;
}

void UplinkLMRTC(bool arm, bool set)
{
	if (g_Data.connStatus == 0) {
		int bytesRecv = SOCKET_ERROR;
		char addr[256];
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == INVALID_SOCKET) {
			g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "ERROR AT SOCKET(): %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(addr, "127.0.0.1");
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr);
		if (g_Data.uplinkLEM > 0) { clientService.sin_port = htons(14243); }
		else { clientService.sin_port = htons(14242); }
		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
			g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "FAILED TO CONNECT, ERROR %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "CONNECTED");
		g_Data.uplinkState = 0;
		uplink_aeaa_cmd(arm, set);
		g_Data.connStatus = 1;
	}
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
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == INVALID_SOCKET) {
			g_Data.updateClockReady = 0;
			sprintf(debugWinsock, "ERROR AT SOCKET(): %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(addr, "127.0.0.1");
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr);
		if (g_Data.uplinkLEM > 0) { clientService.sin_port = htons(14243); }
		else { clientService.sin_port = htons(14242); }
		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
			g_Data.updateClockReady = 0;
			sprintf(debugWinsock, "FAILED TO CONNECT, ERROR %ld", WSAGetLastError());
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
	}
	else if (g_Data.connStatus == 2) {
		// increment clock
		g_Data.updateClockReady = 0;
		char buffer[8];	
		double mt;
		if(g_Data.uplinkLEM > 0)
		{ 
			mt = g_Data.gorpVessel->GetMissionTime();
		}
		else
		{
			mt = g_Data.progVessel->GetMissionTime();
		}
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

void UplinkSunburstSuborbitalAbort()
{
	g_Data.uplinkDataReady = 2;

	if (g_Data.connStatus == 0)
	{
		int bytesRecv = SOCKET_ERROR;
		char addr[256];
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == INVALID_SOCKET) {
			g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "ERROR AT SOCKET(): %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(addr, "127.0.0.1");
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr);
		if (g_Data.uplinkLEM > 0) { clientService.sin_port = htons(14243); }
		else { clientService.sin_port = htons(14242); }
		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
			g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "FAILED TO CONNECT, ERROR %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "CONNECTED");
		g_Data.uplinkState = 0;

		send_agc_key('S');
		g_Data.connStatus = 1;
		g_Data.uplinkState = 0;
	}
}

void UplinkSunburstCOI()
{
	g_Data.uplinkDataReady = 2;

	if (g_Data.connStatus == 0)
	{
		int bytesRecv = SOCKET_ERROR;
		char addr[256];
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == INVALID_SOCKET) {
			g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "ERROR AT SOCKET(): %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(addr, "127.0.0.1");
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr);
		if (g_Data.uplinkLEM > 0) { clientService.sin_port = htons(14243); }
		else { clientService.sin_port = htons(14242); }
		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
			g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "FAILED TO CONNECT, ERROR %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "CONNECTED");
		g_Data.uplinkState = 0;

		send_agc_key('T');
		g_Data.connStatus = 1;
		g_Data.uplinkState = 0;
	}
}

void ProjectApolloMFDopcTimestep (double simt, double simdt, double mjd)
{
	if (g_Data.connStatus > 0 && g_Data.uplinkBuffer.size() > 0) {
		if (simt > g_Data.uplinkBufferSimt + 0.1) {
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
ProjectApolloMFD::ProjectApolloMFD (DWORD w, DWORD h, VESSEL *vessel) : MFD2 (w, h, vessel)

{
	saturn = NULL;
	isSaturnV = false;
	FailureSubpage = 0;
	crawler = NULL;
	lem = NULL;
	mcc = NULL;
	width = w;
	height = h;
	HBITMAP hBmpLogo = LoadBitmap(g_hDLL, MAKEINTRESOURCE (IDB_LOGO));
	hLogo = oapiCreateSurface(hBmpLogo);
	screen = 0;
	debug_frozen = false;
	char buffer[8];

	//We need to find out what type of vessel it is, so we check for the class name.
	//Saturns have different functions than Crawlers.  But we have methods for both.
	if (utils::IsVessel(vessel, utils::Saturn)) {
		saturn = (Saturn *)vessel;
		g_Data.progVessel = saturn;
		g_Data.vessel = vessel;
		oapiGetObjectName(saturn->GetGravityRef(), buffer, 8);
		if (strcmp(buffer, "Earth") == 0 || strcmp(buffer, "Moon") == 0)
			g_Data.planet = saturn->GetGravityRef();
		else
			g_Data.planet = oapiGetGbodyByName("Earth");

		if (utils::IsVessel(vessel, utils::SaturnIB))
		{
			isSaturnV = false;
		}
		else
		{
			isSaturnV = true;
		}
	}
	else if (utils::IsVessel(vessel, utils::Crawler))  {
			crawler = (Crawler *)vessel;
			g_Data.planet = crawler->GetGravityRef();
	}
	else if (utils::IsVessel(vessel, utils::LEM)) {
			lem = (LEM *)vessel;
			g_Data.vessel = vessel;
			g_Data.gorpVessel = lem;
			oapiGetObjectName(lem->GetGravityRef(), buffer, 8);
			if(strcmp(buffer,"Earth") == 0 || strcmp(buffer,"Moon") == 0 )
				g_Data.planet = lem->GetGravityRef();
			else
				g_Data.planet = oapiGetGbodyByName("Earth");
	}

	mcc = NULL;
	OBJHANDLE hMCC = oapiGetVesselByName("MCC");
	if (hMCC != NULL) {
		VESSEL* pVessel = oapiGetVesselInterface(hMCC);
		if (pVessel) {
			if (utils::IsVessel(pVessel, utils::MCC))
			{
				MCCVessel *pMCCVessel = static_cast<MCCVessel*>(pVessel);
				if (pMCCVessel->mcc)
				{
					mcc = pMCCVessel->mcc;
				}
			}
		}
	}
}

// Destructor
ProjectApolloMFD::~ProjectApolloMFD ()
{
	// Add MFD cleanup code here
	oapiDestroySurface(hLogo);
}

// Return button labels
char *ProjectApolloMFD::ButtonLabel (int bt)
{
	// The labels for the buttons used by our MFD mode
	//If we are working with an unsupported vehicle, we don't want to return any button labels.
	if (!saturn && !lem) {
		return 0;
	}

	return m_buttonPages.ButtonLabel(bt);
}

// Return button menus
int ProjectApolloMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	// The menu descriptions for the buttons used by our MFD mode
	// We don't want to display a menu if we are in an unsupported vessel.
	if (!saturn && !lem) {
		menu = 0;
		return 0;
	}

	return m_buttonPages.ButtonMenu(menu);
}

bool ProjectApolloMFD::ConsumeKeyBuffered (DWORD key) 
{
	//We don't want to accept keyboard commands from the wrong vessels.
	if (!saturn && !lem)
		return false;

	return m_buttonPages.ConsumeKeyBuffered(this, key);
}

bool ProjectApolloMFD::ConsumeButton (int bt, int event)
{
	return m_buttonPages.ConsumeButton(this, bt, event);
}

// Repaint the MFD
bool ProjectApolloMFD::Update (oapi::Sketchpad* skp)
{
	char buffer[100];

	SURFHANDLE s = skp->GetSurface();
	RECT src{ 0, 0, 255, 255 };
	RECT dst{ 1, 1, (LONG)width - 2, (LONG)height - 2 };
	oapiBlt(s, hLogo, &dst, &src);

	// Draws the MFD title
	Title(skp, "Project Apollo");

	skp->SetFont(GetDefaultFont(0));
	skp->SetBackgroundMode(oapi::Sketchpad::BK_TRANSPARENT);
	skp->SetTextAlign(oapi::Sketchpad::CENTER);

	if (!saturn && !lem) {
		skp->SetTextColor(RGB(255, 0, 0));
		skp->Text(width / 2, (int)(height * 0.5), "Unsupported vessel", 18);
		if (!crawler)
			return true;
	}

	// Draw mission time
	skp->SetTextColor (RGB(0, 255, 0));
	skp->Text(width / 2, (int) (height * 0.1), "Ground Elapsed Time", 19);

	double mt = 0;
	if (mcc)
	{
		mt = mcc->GetMissionTime();
	}
	
	if (mt <= 0.0) //Mission time from MCC might be nonsense before liftoff
	{
		if (saturn) { mt = saturn->GetMissionTime(); }
		if (crawler) { mt = crawler->GetMissionTime(); }
		if (lem) { mt = lem->GetMissionTime(); }
	}

	int secs = abs((int) mt);
	int hours = (secs / 3600);
	secs -= (hours * 3600);
	int minutes = (secs / 60);
	secs -= 60 * minutes;
	if (mt < 0)
		sprintf(buffer, "-%d:%02d:%02d", hours, minutes, secs);
	else
		sprintf(buffer, "%d:%02d:%02d", hours, minutes, secs);
	skp->Text(width / 2, (int)(height * 0.15), buffer, strlen(buffer));
	//If this is the crawler and not the actual Saturn, do NOTHING else!
	if (!saturn && !lem)
		return true;

	skp->SetPen(GetDefaultPen(0));
	skp->MoveTo((int)(width * 0.05), (int)(height * 0.25));
	skp->LineTo((int)(width * 0.95), (int)(height * 0.25));

	// Draw GNC
	if (screen == m_buttonPages.page.GNC) {
		skp->Text(width / 2, (int)(height * 0.3), "Guidance, Navigation & Control", 30);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text((int)(width * 0.1), (int)(height * 0.4), "Velocity:", 9);
		skp->Text((int)(width * 0.1), (int)(height * 0.45), "Vert. Velocity:", 15);
		skp->Text((int)(width * 0.1), (int)(height * 0.5), "Altitude:", 9);
		skp->Text((int)(width * 0.1), (int)(height * 0.6), "Apoapsis Alt.:", 14);
		skp->Text((int)(width * 0.1), (int)(height * 0.65), "Periapsis Alt.:", 15);
		skp->Text((int)(width * 0.1), (int)(height * 0.7), "Inclination:", 12);
		skp->Text((int)(width * 0.1), (int)(height * 0.8), "Latitude:", 9);
		skp->Text((int)(width * 0.1), (int)(height * 0.85), "Longitude:", 10);

		OBJHANDLE planet;
		ELEMENTS elem;
		char planetName[255];
		VECTOR3 vel, hvel;
		double vvel = 0, apDist, peDist, lat, lon, radius;

		if (saturn) {
			planet = saturn->GetGravityRef();
			saturn->GetRelativeVel(planet, vel);
			if (saturn->GetAirspeedVector(FRAME_HORIZON, hvel)) {
				vvel = hvel.y * 3.2808399;
			}
			saturn->GetApDist(apDist);
			saturn->GetPeDist(peDist);
			saturn->GetEquPos(lon, lat, radius);
			saturn->GetElements(planet, elem, 0, 0, FRAME_EQU);
		}
		else if (lem) {
			planet = lem->GetGravityRef();
			lem->GetRelativeVel(planet, vel);
			if (lem->GetAirspeedVector(FRAME_HORIZON, hvel)) {
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
		}
		else {
			apDist -= 1.73809e6;
			peDist -= 1.73809e6;
		}

		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		sprintf(buffer, "%.0lf ft/s", length(vel) * 3.2808399);
		skp->Text((int)(width * 0.9), (int)(height * 0.4), buffer, strlen(buffer));
		sprintf(buffer, "%.0lf ft/s", vvel);
		skp->Text((int)(width * 0.9), (int)(height * 0.45), buffer, strlen(buffer));
		if (saturn) { sprintf(buffer, "%.1lf nm  ", saturn->GetAltitude() * 0.000539957); }
		if (lem) { sprintf(buffer, "%.1lf nm  ", lem->GetAltitude() * 0.000539957); }
		skp->Text((int)(width * 0.9), (int)(height * 0.5), buffer, strlen(buffer));
		sprintf(buffer, "%.1lf nm  ", apDist * 0.000539957);
		skp->Text((int)(width * 0.9), (int)(height * 0.6), buffer, strlen(buffer));
		sprintf(buffer, "%.1lf nm  ", peDist * 0.000539957);
		skp->Text((int)(width * 0.9), (int)(height * 0.65), buffer, strlen(buffer));
		sprintf(buffer, "%.2lf°   ", elem.i * DEG);
		skp->Text((int)(width * 0.9), (int)(height * 0.7), buffer, strlen(buffer));
		sprintf(buffer, "%.2lf°   ", lat * DEG);
		skp->Text((int)(width * 0.9), (int)(height * 0.8), buffer, strlen(buffer));
		sprintf(buffer, "%.2lf°   ", lon * DEG);
		skp->Text((int)(width * 0.9), (int)(height * 0.85), buffer, strlen(buffer));

		if (g_Data.killrot) {
			skp->SetTextColor(RGB(255, 0, 0));
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text(width / 2, (int)(height * 0.9), "*** KILL ROTATION ACTIVE ***", 28);
		}
	}

	// Draw ECS
	else if (screen == m_buttonPages.page.ECS) {
		skp->Text(width / 2, (int)(height * 0.3), "Environmental Control System", 28);

		if (saturn)
		{

			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			skp->Text((int)(width * 0.1), (int)(height * 0.4), "Crew status:", 12);
			skp->Text((int)(width * 0.1), (int)(height * 0.45), "Crew number:", 12);

			ECSStatus ecs;
			saturn->GetECSStatus(ecs);

			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			if (ecs.crewStatus == ECS_CREWSTATUS_OK) {
				skp->Text((int)(width * 0.7), (int)(height * 0.4), "OK", 2);
			}
			else if (ecs.crewStatus == ECS_CREWSTATUS_CRITICAL) {
				skp->SetTextColor(RGB(255, 255, 0));
				skp->Text((int)(width * 0.7), (int)(height * 0.4), "CRITICAL", 8);
				skp->SetTextColor(RGB(0, 255, 0));
			}
			else {
				skp->SetTextColor(RGB(255, 0, 0));
				skp->Text((int)(width * 0.7), (int)(height * 0.4), "DEAD", 4);
				skp->SetTextColor(RGB(0, 255, 0));
			}

			sprintf(buffer, "%d", ecs.crewNumber);
			skp->Text((int)(width * 0.7), (int)(height * 0.45), buffer, strlen(buffer));

			skp->Text((int)(width * 0.5), (int)(height * 0.525), "Glycol Coolant Loops", 20);
			skp->Text((int)(width * 0.6), (int)(height * 0.6), "Prim.", 5);
			skp->Text((int)(width * 0.8), (int)(height * 0.6), "Sec.", 4);

			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			skp->Text((int)(width * 0.1), (int)(height * 0.6), "Heating:", 8);
			skp->Text((int)(width * 0.1), (int)(height * 0.65), "Actual:", 7);
			skp->Text((int)(width * 0.1), (int)(height * 0.7), "Test:", 5);
			skp->Text((int)(width * 0.1), (int)(height * 0.8), "Total:", 6);
			skp->Text((int)(width * 0.1), (int)(height * 0.9), "CSM O2 Hose:", 12);

			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			sprintf(buffer, "%.0lfW", ecs.PrimECSHeating);
			skp->Text((int)(width * 0.6), (int)(height * 0.65), buffer, strlen(buffer));
			sprintf(buffer, "%.0lfW", ecs.PrimECSTestHeating);
			skp->Text((int)(width * 0.6), (int)(height * 0.7), buffer, strlen(buffer));
			sprintf(buffer, "%.0lfW", ecs.PrimECSHeating + ecs.PrimECSTestHeating);
			skp->Text((int)(width * 0.6), (int)(height * 0.8), buffer, strlen(buffer));
			sprintf(buffer, "%.0lfW", ecs.SecECSHeating);
			skp->Text((int)(width * 0.8), (int)(height * 0.65), buffer, strlen(buffer));
			sprintf(buffer, "%.0lfW", ecs.SecECSTestHeating);
			skp->Text((int)(width * 0.8), (int)(height * 0.7), buffer, strlen(buffer));
			sprintf(buffer, "%.0lfW", ecs.SecECSHeating + ecs.SecECSTestHeating);
			skp->Text((int)(width * 0.8), (int)(height * 0.8), buffer, strlen(buffer));

			skp->MoveTo((int)(width * 0.5), (int)(height * 0.775));
			skp->LineTo((int)(width * 0.9), (int)(height * 0.775));

			if (ecs.CSMO2HoseConnected)
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.9), "Connected", 9);
			}
			else
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.9), "Disconnected", 12);
			}

		}
		else if (lem)
		{
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			skp->Text((int)(width * 0.1), (int)(height * 0.4), "Crew status:", 12);
			skp->Text((int)(width * 0.1), (int)(height * 0.45), "Crew number:", 12);
			skp->Text((int)(width * 0.1), (int)(height * 0.5), "CDR status:", 11);
			skp->Text((int)(width * 0.1), (int)(height * 0.55), "LMP status:", 11);

			LEMECSStatus ecs;
			lem->GetECSStatus(ecs);

			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			if (ecs.crewStatus == ECS_CREWSTATUS_OK) {
				skp->Text((int)(width * 0.7), (int)(height * 0.4), "OK", 2);
			}
			else if (ecs.crewStatus == ECS_CREWSTATUS_CRITICAL) {
				skp->SetTextColor(RGB(255, 255, 0));
				skp->Text((int)(width * 0.7), (int)(height * 0.4), "CRITICAL", 8);
				skp->SetTextColor(RGB(0, 255, 0));
			}
			else {
				skp->SetTextColor(RGB(255, 0, 0));
				skp->Text((int)(width * 0.7), (int)(height * 0.4), "DEAD", 4);
				skp->SetTextColor(RGB(0, 255, 0));
			}

			sprintf(buffer, "%d", ecs.crewNumber);
			skp->Text((int)(width * 0.7), (int)(height * 0.45), buffer, strlen(buffer));

			if (ecs.cdrStatus == 0)
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.5), "In Cabin", 8);
			}
			else if (ecs.cdrStatus == 1)
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.5), "In Suit", 7);
			}
			else if (ecs.cdrStatus == 2)
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.5), "EVA", 3);
			}
			else
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.5), "PLSS", 4);
			}


			if (ecs.lmpStatus == 0)
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.55), "In Cabin", 8);
			}
			else if (ecs.lmpStatus == 1)
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.55), "In Suit", 7);
			}
			else if (ecs.lmpStatus == 2)
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.55), "EVA", 3);
			}
			else
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.55), "PLSS", 4);
			}
		}
		else
		{
		skp->Text(width / 2, (int)(height * 0.4), "Unsupported vehicle", 19);
		}
	// Draw IMFD
	} else if (screen == m_buttonPages.page.IU) {
		skp->Text(width / 2, (int)(height * 0.3), "IU Uplink Data", 14);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text((int)(width * 0.1), (int)(height * 0.35), "Type:", 5);

		if (g_Data.iuUplinkType == DCSUPLINK_SWITCH_SELECTOR)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text((int)(width * 0.7), (int)(height * 0.35), "Switch Selector", 15);

			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			skp->Text((int)(width * 0.1), (int)(height * 0.45), "1: Stage:", 9);

			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			if (g_Data.iuUplinkSwitSelStage == 0)
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.45), "IU", 2);
			}
			else if (g_Data.iuUplinkSwitSelStage == 1)
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.45), "S-I", 3);
			}
			else if (g_Data.iuUplinkSwitSelStage == 2)
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.45), "S-II", 4);
			}
			else if (g_Data.iuUplinkSwitSelStage == 3)
			{
				skp->Text((int)(width * 0.7), (int)(height * 0.45), "S-IVB", 5);
			}

			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			skp->Text((int)(width * 0.1), (int)(height * 0.5), "2: Channel:", 11);

			sprintf(buffer, "%d", g_Data.iuUplinkSwitSelChannel);
			skp->Text((int)(width * 0.7), (int)(height * 0.5), buffer, strlen(buffer));
		}
		else if (g_Data.iuUplinkType == DCSUPLINK_TIMEBASE_UPDATE)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text((int)(width * 0.7), (int)(height * 0.35), "Timebase Update", 15);

			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			skp->Text((int)(width * 0.1), (int)(height * 0.45), "1: Delta T:", 11);

			sprintf(buffer, "%+.1f s", g_Data.iuUplinkTimebaseUpdateTime);
			skp->Text((int)(width * 0.7), (int)(height * 0.45), buffer, strlen(buffer));
		}
		else if (g_Data.iuUplinkType == DCSUPLINK_LM_ABORT)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text((int)(width * 0.7), (int)(height * 0.35), "LM Abort (Apollo 5)", 19);
		}
		else if (g_Data.iuUplinkType == DCSUPLINK_TDE_ENABLE)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text((int)(width * 0.7), (int)(height * 0.35), "TD&E Enable", 11);
		}
		else if (g_Data.iuUplinkType == DCSUPLINK_RESTART_MANEUVER_ENABLE)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text((int)(width * 0.7), (int)(height * 0.35), "Restart Maneuver Enable", 23);
		}
		else if (g_Data.iuUplinkType == DCSUPLINK_TIMEBASE_8_ENABLE)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text((int)(width * 0.7), (int)(height * 0.35), "Timebase 8 Enable", 17);
		}
		else if (g_Data.iuUplinkType == DCSUPLINK_EVASIVE_MANEUVER_ENABLE)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text((int)(width * 0.7), (int)(height * 0.35), "Evasive Yaw Mnvr Enable", 23);
		}
		else if (g_Data.iuUplinkType == DCSUPLINK_EXECUTE_COMM_MANEUVER)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text((int)(width * 0.7), (int)(height * 0.35), "Execute Comm Maneuver", 21);
		}
		else if (g_Data.iuUplinkType == DCSUPLINK_SIVBIU_LUNAR_IMPACT)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text((int)(width * 0.7), (int)(height * 0.35), "S-IVB/IU Lunar Impact", 21);

			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			skp->Text((int)(width * 0.1), (int)(height * 0.45), "1: TIG:", 7);
			skp->Text((int)(width * 0.1), (int)(height * 0.5), "2: BT:", 6);
			skp->Text((int)(width * 0.1), (int)(height * 0.55), "3: Pitch:", 9);
			skp->Text((int)(width * 0.1), (int)(height * 0.6), "4: Yaw:", 7);

			sprintf(buffer, "TB8+%.0f s", g_Data.iuUplinkTIG);
			skp->Text((int)(width * 0.7), (int)(height * 0.45), buffer, strlen(buffer));
			sprintf(buffer, "%.1f s", g_Data.iuUplinkDT);
			skp->Text((int)(width * 0.7), (int)(height * 0.5), buffer, strlen(buffer));
			sprintf(buffer, "%.01f°", g_Data.iuUplinkPitch*DEG);
			skp->Text((int)(width * 0.7), (int)(height * 0.55), buffer, strlen(buffer));
			sprintf(buffer, "%.01f°", g_Data.iuUplinkYaw*DEG);
			skp->Text((int)(width * 0.7), (int)(height * 0.6), buffer, strlen(buffer));
		}
		else if (g_Data.iuUplinkType == DCSUPLINK_REMOVE_INHIBIT_MANEUVER4)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text((int)(width * 0.7), (int)(height * 0.35), "Remove Inhibit Mnv. 4", 21);
		}
		else if (g_Data.iuUplinkType == DCSUPLINK_GENERALIZED_MANEUVER)
		{
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text((int)(width * 0.65), (int)(height * 0.35), "SIB Generalized Maneuver", 24);

			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			skp->Text((int)(width * 0.1), (int)(height * 0.45), "1: Type", 7);

			switch (g_Data.iuUplinkGenManType)
			{
			case 0:
				skp->Text((int)(width * 0.5), (int)(height * 0.45), "Local Reference", 15);
				break;
			case 1:
				skp->Text((int)(width * 0.5), (int)(height * 0.45), "Inertial Reference", 18);
				break;
			case 2:
				skp->Text((int)(width * 0.5), (int)(height * 0.45), "Return to Timeline", 18);
				break;
			case 3:
				skp->Text((int)(width * 0.5), (int)(height * 0.45), "Special Maneuver A", 18);
				break;
			case 4:
				skp->Text((int)(width * 0.5), (int)(height * 0.45), "Special Maneuver B", 18);
				break;
			}

			skp->Text((int)(width * 0.1), (int)(height * 0.5), "2: Time", 7);

			sprintf(buffer, "TB4+%.0f s", g_Data.iuUplinkTIG);
			skp->Text((int)(width * 0.5), (int)(height * 0.5), buffer, strlen(buffer));

			if (g_Data.iuUplinkGenManType == 0 || g_Data.iuUplinkGenManType == 1)
			{
				skp->Text((int)(width * 0.1), (int)(height * 0.55), "3: Attitude", 11);

				sprintf(buffer, "%.1f %.1f %.1f", g_Data.iuUplinkGenManAtt.x*DEG, g_Data.iuUplinkGenManAtt.y*DEG, g_Data.iuUplinkGenManAtt.z*DEG);
				skp->Text((int)(width * 0.5), (int)(height * 0.55), buffer, strlen(buffer));
			}
		}

		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(width / 2, (int)(height * 0.75), "IU Uplink Result", 16);
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text((int)(width * 0.1), (int)(height * 0.8), "Status:", 7);

		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		if (g_Data.iuUplinkResult == 1)
		{
			skp->Text((int)(width * 0.7), (int)(height * 0.8), "Uplink accepted", 15);
		}
		else if (g_Data.iuUplinkResult == 2)
		{
			skp->Text((int)(width * 0.7), (int)(height * 0.8), "Vessel has no IU", 16);
		}
		else if (g_Data.iuUplinkResult == 3)
		{
			skp->Text((int)(width * 0.7), (int)(height * 0.8), "Uplink rejected", 15);
		}

		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->SetTextColor(RGB(128, 128, 128));
		if (g_Data.uplinkVessel)
		{
			oapiGetObjectName(g_Data.uplinkVessel->GetHandle(), buffer, 100);
		}
		else
		{
			sprintf(buffer, "No Target!");
		}
		skp->Text((int)(width * 0.05), (int)(height * 0.95), buffer, strlen(buffer));
	}
	//Draw Telemetry
	else if (screen == m_buttonPages.page.TELE) {
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		sprintf(buffer, "Telemetry: %s", debugWinsock);
		skp->Text((int)(width * 0.1), (int)(height * 0.30), "Telemetry:", 10);
		skp->Text((int)(width * 0.6), (int)(height * 0.30), debugWinsock, strlen(debugWinsock));

		if (g_Data.uplinkDataReady == 1 || g_Data.updateClockReady == 1) {
			if (lem) {
				skp->SetTextAlign(oapi::Sketchpad::CENTER);
				sprintf(buffer, "Checklist");
				skp->Text(width / 2, (int)(height * 0.45), buffer, strlen(buffer));
				skp->SetTextAlign(oapi::Sketchpad::LEFT);
				sprintf(buffer, "LGC: IDLE (P00 DESIRED)");
				skp->Text((int)(width * 0.1), (int)(height * 0.55), buffer, strlen(buffer));
				sprintf(buffer, "P12: UPDATA LINK - DATA (down)");
				skp->Text((int)(width * 0.1), (int)(height * 0.60), buffer, strlen(buffer));
				sprintf(buffer, "P11: UP DATA LINK CB - IN");
				skp->Text((int)(width * 0.1), (int)(height * 0.65), buffer, strlen(buffer));
				skp->SetTextAlign(oapi::Sketchpad::CENTER);
			} else {
				skp->SetTextAlign(oapi::Sketchpad::CENTER);
				sprintf(buffer, "Checklist");
				skp->Text(width / 2, (int)(height * 0.45), buffer, strlen(buffer));
				skp->SetTextAlign(oapi::Sketchpad::LEFT);
				sprintf(buffer, "DSKY - V37E 00E");
				skp->Text((int)(width * 0.1), (int)(height * 0.55), buffer, strlen(buffer));
				sprintf(buffer, "UPTLM CM - ACCEPT (up)   2, 122");
				skp->Text((int)(width * 0.1), (int)(height * 0.60), buffer, strlen(buffer));
				sprintf(buffer, "UP TLM - DATA (up)            3");
				skp->Text((int)(width * 0.1), (int)(height * 0.65), buffer, strlen(buffer));
				sprintf(buffer, "PCM BIT RATE - HIGH (up)      3");
				skp->Text((int)(width * 0.1), (int)(height * 0.7), buffer, strlen(buffer));
				skp->SetTextAlign(oapi::Sketchpad::CENTER);
			}
			if (g_Data.uplinkDataReady == 1) {
				if (g_Data.uplinkDataType == UPLINK_SV)
					sprintf(buffer, "Press SV to start upload");
			}
			else
				sprintf(buffer, "Press CLK to start upload");
			skp->Text(width / 2, (int)(height * 0.8), buffer, strlen(buffer));
		}
		else if(g_Data.uplinkDataReady == 2) {
			double linepos = 0.4;
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			sprintf(buffer, "304   %ld", g_Data.emem[0]);
			skp->Text((int)(width * 0.1), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			sprintf(buffer, "305   %ld", g_Data.emem[1]);
			skp->Text((int)(width * 0.1), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			sprintf(buffer, "306   %ld", g_Data.emem[2]);
			skp->Text((int)(width * 0.1), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			sprintf(buffer, "307   %ld", g_Data.emem[3]);
			skp->Text((int)(width * 0.1), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			sprintf(buffer, "310   %ld", g_Data.emem[4]);
			skp->Text((int)(width * 0.1), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			sprintf(buffer, "311   %ld", g_Data.emem[5]);
			skp->Text((int)(width * 0.1), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			sprintf(buffer, "312   %ld", g_Data.emem[6]);
			skp->Text((int)(width * 0.1), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			sprintf(buffer, "313   %ld", g_Data.emem[7]);
			skp->Text((int)(width * 0.1), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			sprintf(buffer, "314   %ld", g_Data.emem[8]);
			skp->Text((int)(width * 0.1), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			sprintf(buffer, "315   %ld", g_Data.emem[9]);
			skp->Text((int)(width * 0.1), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			linepos = 0.4;
			sprintf(buffer, "316   %ld", g_Data.emem[10]);
			skp->Text((int)(width * 0.55), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			sprintf(buffer, "317   %ld", g_Data.emem[11]);
			skp->Text((int)(width * 0.55), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			sprintf(buffer, "320   %ld", g_Data.emem[12]);
			skp->Text((int)(width * 0.55), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			if (g_Data.uplinkDataType >= UPLINK_SV) {
				sprintf(buffer, "321   %ld", g_Data.emem[13]);
				skp->Text((int)(width * 0.55), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
				sprintf(buffer, "322   %ld", g_Data.emem[14]);
				skp->Text((int)(width * 0.55), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
				sprintf(buffer, "323   %ld", g_Data.emem[15]);
				skp->Text((int)(width * 0.55), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
				sprintf(buffer, "324   %ld", g_Data.emem[16]);
				skp->Text((int)(width * 0.55), (int)(height* (linepos += 0.05)), buffer, strlen(buffer));
			}
		}
		else if (g_Data.uplinkDataReady == 3)
		{
			skp->SetTextAlign(oapi::Sketchpad::LEFT);

			if (g_Data.uplinkBuffer.size() > 0)
			{
				sprintf(buffer, "Uplink word: %d", g_Data.uplinkBuffer.front());
				skp->Text((int)(width * 0.1), (int)(height * 0.4), buffer, strlen(buffer));
			}
		}
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->SetTextColor(RGB(128, 128, 128));
		oapiGetObjectName(g_Data.vessel->GetHandle(), buffer, 100);
		skp->Text((int)(width * 0.05), (int)(height * 0.95), buffer, strlen(buffer));
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		oapiGetObjectName(g_Data.planet, buffer, 100);
		skp->Text((int)(width * 0.5), (int)(height * 0.95), buffer, strlen(buffer));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		if (g_Data.uplinkSlot == 0) {
			sprintf(buffer, "This");
		} else {
			sprintf(buffer, "Other");
		}		
		skp->Text((int)(width * 0.95), (int)(height * 0.95), buffer, strlen(buffer));

	}
	else if (screen == m_buttonPages.page.Debug)
	{

		if ((strcmp(debugString,debugStringBuffer)!= 0) && (strlen(debugStringBuffer) != 0) && !debug_frozen)
		{
			strcpy(debugString, debugStringBuffer);
			sprintf(debugStringBuffer,"");
		}
		skp->Text(width / 2, (int)(height * 0.3), "Debug Data", 10);
		if (strlen(debugString) > 35)
		{
			int i = 0;
			double h = 0.4;
			bool done = false;
			while (!done)
			{
				if (strlen(&debugString[i]) > 35)
				{
					skp->Text(width / 2, (int)(height* h), &debugString[i], 35);
					i = i + 35;
					h = h + 0.05;
				}
				else
				{
					skp->Text(width / 2, (int)(height* h), &debugString[i], strlen(&debugString[i]));
					done = true;
				}
			}
		}
		else skp->Text(width / 2, (int)(height * 0.4), debugString, strlen(debugString));
	}
	// Draw LGC Setup screen
	else if (screen == m_buttonPages.page.LGC) {
		skp->Text(width / 2, (int)(height * 0.3), "LGC Docked Init Data", 20);
		// What's our status?
		if(saturn == NULL){
			// skp->Text(width / 2, (int) (height * 0.4), "We are in the LM", 16);

			// We need to find the CM.
			GetCSM();

			if (saturn) {
				VECTOR3 CMattitude, LMattitude;
				unsigned short tephem[3];
				// Obtain CM attitude.
				// It would be better to call GetTotalAttitude() but for some reason VC++ refuses to link it properly. Sigh.
				CMattitude.x = saturn->imu.Gimbal.X; // OUTER
				CMattitude.y = saturn->imu.Gimbal.Y; // INNER
				CMattitude.z = saturn->imu.Gimbal.Z; // MIDDLE
				// Docking tunnel angle is assumed to be zero.
				LMattitude = OrbMech::LMDockedCoarseAlignment(CMattitude, g_Data.lmAlignType);
				// We should obtain and print CSM time, but...
				// the update delay of the MFD makes time correction less than one second a pain at best, so we won't bother for now.
				// Just initialize from the mission timer.
				// Obtain TEPHEM
				tephem[0] = saturn->agc.vagc.Erasable[0][01706];
				tephem[1] = saturn->agc.vagc.Erasable[0][01707];
				tephem[2] = saturn->agc.vagc.Erasable[0][01710];
				sprintf(buffer, "TEPHEM: %05o %05o %05o", tephem[0], tephem[1], tephem[2]);
				skp->Text(width / 2, (int)(height * 0.4), buffer, strlen(buffer));
				// Format gimbal angles and print them
				sprintf(buffer, "CSM O/I/M: %+07.2f %+07.2f %+07.2f", CMattitude.x*DEG, CMattitude.y*DEG, CMattitude.z*DEG);
				skp->Text(width / 2, (int)(height * 0.45), buffer, strlen(buffer));
				sprintf(buffer, "LM O/I/M: %+07.2f %+07.2f %+07.2f", LMattitude.x*DEG, LMattitude.y*DEG, LMattitude.z*DEG);
				skp->Text(width / 2, (int)(height * 0.5), buffer, strlen(buffer));

				//Docked IMU Fine Alignment
				skp->Text(width / 2, (int)(height * 0.6), "Docked IMU Fine Alignment", 25);

				sprintf(buffer, "V42: %+07.3f %+07.3f %+07.3f", g_Data.V42angles.x*DEG, g_Data.V42angles.y*DEG, g_Data.V42angles.z*DEG);
				skp->Text(width / 2, (int)(height * 0.7), buffer, strlen(buffer));

				if (g_Data.lmAlignType)
				{
					skp->Text(width / 2, (int)(height * 0.85), "Alignment: Identical", 20);
				}
				else
				{
					skp->Text(width / 2, (int)(height * 0.85), "Alignment: LVLH", 15);
				}

				saturn = NULL; // Clobber
			}
			else {
				skp->Text(width / 2, (int)(height * 0.4), "CSM is not docked", 17);
			}
		}else{
			skp->Text(width / 2, (int)(height * 0.4), "Do this from the LM", 19);
		}
		/*
		sprintf(buffer, "Socket: %i", close_Socket);
		TextOut(hDC, width / 2, (int) (height * 0.4), buffer, strlen(buffer));
		*/
	}
	else if (screen == m_buttonPages.page.Failures)
	{
		skp->Text(width / 2, (int)(height * 0.3), "Failure Simulation", 18);

		if (saturn)
		{
			if (saturn->GetDamageModel() == false)
			{
				skp->Text(width / 2, (int)(height * 0.6), "Failures disabled in Orbiter Launchpad", 38);
			}
			else
			{
				skp->SetTextAlign(oapi::Sketchpad::LEFT);

				unsigned int offset = FailureSubpage * 10U; //Per per page
				unsigned j, len = 0;

				for (unsigned i = 0; i < 10; i++)
				{
					j = i + offset;

					if (j >= saturn->Failures.GetNumberOfMalfunctions()) break;

					len = sprintf(buffer, "%d: %s: ", j + 1, saturn->Failures.GetName(j).c_str());// , saturn->Failures.IsFailureArmed(j));

					if (saturn->Failures.IsFailureArmed(j))
					{
						switch (saturn->Failures.GetCondition(j))
						{
						case 0:
							len += sprintf(buffer + len, "MT+");
							break;
						case 1:
							len += sprintf(buffer + len, "SimT+");
							break;
						case 2:
							len += sprintf(buffer + len, "S-II+");
							break;
						case 3:
							len += sprintf(buffer + len, "S-IVB+");
							break;
						case 4:
							len += sprintf(buffer + len, "Ins+");
							break;
						case 5:
							len += sprintf(buffer + len, "TB6+");
							break;
						case 6:
							len += sprintf(buffer + len, "TLI+");
							break;
						}
						len += sprintf(buffer + len, " %.0lf s", saturn->Failures.GetConditionValue(j));
					}
					else
					{
						len += sprintf(buffer + len, "Off");
					}

					skp->Text(width / 16, (9 + i)*height / 20, buffer, strlen(buffer));
				}
			}
		}
		else
		{
			skp->Text(width / 2, (int)(height * 0.5), "Failures not supported!", 23);
		}
	}
	return true;
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
		{
			m_buttonPages.SelectPage(this, screen);
			return;
		}

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
		else if (lem)
			lem->SetCrewNumber(n);
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

bool ProjectApolloMFD::SetSwitchSelectorChannel(char *rstr)
{
	int n;

	if (sscanf(rstr, "%d", &n) == 1 && n >= 1 && n <= 112) {
		g_Data.iuUplinkSwitSelChannel = n;
		InvalidateDisplay();
		return true;
	}
	return false;
}

bool ProjectApolloMFD::SetTimebaseUpdate(char *rstr)
{
	double f;

	if (sscanf(rstr, "%lf", &f) == 1 && abs(f) > 4.0 && abs(f) < 124.0) {
		g_Data.iuUplinkTimebaseUpdateTime = f;
		InvalidateDisplay();
		return true;
	}
	return false;
}

bool ProjectApolloMFD::SaturnSetFailureInput(unsigned n, int Condition, double ConditionValue)
{
	if (saturn)
	{
		if (n >= 1 && n <= saturn->Failures.GetNumberOfMalfunctions())
		{
			saturn->Failures.ArmFailure(n - 1, Condition, ConditionValue);
			return true;
		}
	}

	return false;
}

bool ProjectApolloMFD::SaturnResetFailureInput(unsigned n)
{
	if (saturn)
	{
		if (n >= 1 && n <= saturn->Failures.GetNumberOfMalfunctions())
		{
			saturn->Failures.ResetFailureArm(n - 1);
			return true;
		}
	}
	return false;
}

void ProjectApolloMFD::SetRandomFailures(double FailureMultiplier)
{
	if (saturn == false) return;

	saturn->Failures.SetRandomFailures(FailureMultiplier);
}

void ProjectApolloMFD::SetAEAACommands(int arm, int set)
{
	g_Data.uplinkLEM = 1;
	UplinkLMRTC(arm == 1, set == 1);
}

void ProjectApolloMFD::GetCSM()
{
	//Get pointer to CSM vessel while in LM

	//Only do this in the LM
	if (lem == NULL) return;

	//Get dock handle
	DOCKHANDLE dock = lem->GetDockHandle(0);
	if (dock == NULL) return;

	//Get object handle
	OBJHANDLE object = lem->GetDockStatus(dock);

	VESSEL *vessel;

	if (object != NULL) {
		vessel = oapiGetVesselInterface(object);
		// If some jerk names the S4B a CM name instead this will probably screw up, but who would do that?
		if (utils::IsVessel(vessel, utils::Saturn)) {
			saturn = (Saturn *)vessel;
		}
	}
}

void ProjectApolloMFD::CalculateV42Angles()
{
	//Only do this in the LM
	if (lem == NULL) return;

	GetCSM();
	
	if (saturn && lem)
	{
		VECTOR3 lmn20, csmn20;

		csmn20.x = saturn->imu.Gimbal.X;
		csmn20.y = saturn->imu.Gimbal.Y;
		csmn20.z = saturn->imu.Gimbal.Z;

		lmn20.x = lem->imu.Gimbal.X;
		lmn20.y = lem->imu.Gimbal.Y;
		lmn20.z = lem->imu.Gimbal.Z;

		g_Data.V42angles = OrbMech::LMDockedFineAlignment(lmn20, csmn20, g_Data.lmAlignType);
	}

	saturn = NULL;
}

void ProjectApolloMFD::menuPressEnterOnDSKYDEDA()
{
	if (lem)
	{
		lem->DskySwitchEnter.SetState(true);
		lem->DedaSwitchEnter.SetState(true);
	}
}

void ProjectApolloMFD::menuPressEnterOnCMCLGC()
{
	//Only do this in the LM
	if (lem == NULL) return;

	GetCSM();

	if (lem && saturn)
	{
		lem->DskySwitchEnter.SetState(true);
		saturn->DskySwitchEnter.SetState(true);
	}

	saturn = NULL;
}

void ProjectApolloMFD::menuPressPROOnCMCLGC()
{
	//Only do this in the LM
	if (lem == NULL) return;

	GetCSM();

	if (lem && saturn)
	{
		lem->DskySwitchProceed.SetState(true);
		saturn->DskySwitchProceed.SetState(true);
	}

	saturn = NULL;
}

void ProjectApolloMFD::menuCycleLMAlignType()
{
	g_Data.lmAlignType = !g_Data.lmAlignType;
}

void ProjectApolloMFD::menuVoid(){}

void ProjectApolloMFD::menuSetMainPage()
{
	screen = m_buttonPages.page.None;
	m_buttonPages.SelectPage(this, screen);
}

void ProjectApolloMFD::menuSetGNCPage()
{
	screen = m_buttonPages.page.GNC;
	m_buttonPages.SelectPage(this, screen);
}

void ProjectApolloMFD::menuSetECSPage()
{
	if (saturn != NULL || lem != NULL)
	{
		screen = m_buttonPages.page.ECS;
		m_buttonPages.SelectPage(this, screen);
	}
}

void ProjectApolloMFD::menuSetIUPage()
{
	if (saturn != NULL || lem != NULL)
	{
		screen = m_buttonPages.page.IU;
		m_buttonPages.SelectPage(this, screen);
	}
}

void ProjectApolloMFD::menuSetTELEPage()
{
	screen = m_buttonPages.page.TELE;
	m_buttonPages.SelectPage(this, screen);
}

void ProjectApolloMFD::menuSetLGCPage()
{
	screen = m_buttonPages.page.LGC;
	m_buttonPages.SelectPage(this, screen);
}

void ProjectApolloMFD::menuSetFailuresPage()
{
	FailureSubpage = 0;

	screen = m_buttonPages.page.Failures;
	m_buttonPages.SelectPage(this, screen);
}

void ProjectApolloMFD::menuSetDebugPage()
{
	screen = m_buttonPages.page.Debug;
	m_buttonPages.SelectPage(this, screen);
}

void ProjectApolloMFD::menuKillRot()
{
	g_Data.killrot ? g_Data.killrot = 0 : g_Data.killrot = 1;
}

void ProjectApolloMFD::menuSaveEMSScroll()
{
	if (saturn)
		saturn->SaveEMSScroll();
}

void ProjectApolloMFD::menuVAGCCoreDump()
{
	if (saturn)
		saturn->VirtualAGCCoreDump();
	else if (lem)
		lem->VirtualAGCCoreDump();
}

void ProjectApolloMFD::menuSetCrewNumber()
{
	bool CrewNumberInput(void *id, char *str, void *data);
	oapiOpenInputBox("Crew number in cabin [0-3]:", CrewNumberInput, 0, 20, (void*)this);
}

void ProjectApolloMFD::menuSetCDRInSuit()
{
	if (lem)
	{
		lem->SetCDRInSuit();
	}
}

void ProjectApolloMFD::menuSetLMPInSuit()
{
	if (lem)
	{
		lem->SetLMPInSuit();
	}
}

void ProjectApolloMFD::menuStartEVA()
{
	if (lem)
	{
		lem->StartEVA();
	}
}

void ProjectApolloMFD::menuConnectCSMO2Hose()
{
	if (saturn)
	{
		ECSStatus ecs;
		saturn->GetECSStatus(ecs);

		if (ecs.CSMO2HoseConnected)
		{
			saturn->lemECSConnector.DisconnectCSMO2Hose();
		}
		else
		{
			if (saturn->ForwardHatch.IsOpen()) //TBD: Require LM hatch to be open as well
			{
				saturn->lemECSConnector.ConnectCSMO2Hose();
			}
		}
	}
}

void ProjectApolloMFD::menuSetPrimECSTestHeaterPower()
{
	if (saturn != NULL)
	{
		bool PrimECSTestHeaterPowerInput(void *id, char *str, void *data);
		oapiOpenInputBox("Primary coolant loop test heater power [-3000 to 3000 Watt]:", PrimECSTestHeaterPowerInput, 0, 20, (void*)this);
	}
}

void ProjectApolloMFD::menuSetSecECSTestHeaterPower()
{
	if (saturn != NULL)
	{
		bool SecECSTestHeaterPowerInput(void *id, char *str, void *data);
		oapiOpenInputBox("Secondary coolant loop test heater power [-3000 to 3000 Watt]:", SecECSTestHeaterPowerInput, 0, 20, (void*)this);
	}
}

void ProjectApolloMFD::menuAbortUplink()
{
	if (g_Data.uplinkDataReady == 0 && g_Data.updateClockReady == 0) {
		screen = m_buttonPages.page.None;
		m_buttonPages.SelectPage(this, screen);
	}
	else {
		if (g_Data.uplinkDataReady == 1)
			g_Data.uplinkDataReady -= 1;
		if (g_Data.updateClockReady == 1)
			g_Data.updateClockReady -= 1;
	}
}

void ProjectApolloMFD::menuStateVectorUpdate()
{
	if (saturn || lem) {
		if (g_Data.uplinkDataReady == 0 && g_Data.updateClockReady == 0) {
			g_Data.uplinkDataReady = 1;
			g_Data.uplinkDataType = UPLINK_SV;
		}
		else if (g_Data.uplinkDataReady == 1 && g_Data.uplinkDataType == UPLINK_SV) {
			if (!saturn) { g_Data.uplinkLEM = 1; }
			else { g_Data.uplinkLEM = 0; } // LEM flag
			GetStateVector();
		}
	}
}

void ProjectApolloMFD::menuClockUpdate()
{
	if (saturn || lem) {
		if (g_Data.updateClockReady == 0 && g_Data.uplinkDataReady == 0) {
			g_Data.updateClockReady = 1;
		}
		else if (g_Data.updateClockReady == 1) {
			g_Data.updateClockReady = 2;
			if (!saturn) { g_Data.uplinkLEM = 1; }
			else { g_Data.uplinkLEM = 0; } // LEM flag
			UpdateClock();
		}
	}
}

void ProjectApolloMFD::menuSunburstSuborbitalAbort()
{
	if (lem && lem->ApolloNo == 5) {
		g_Data.uplinkLEM = 1;
		UplinkSunburstSuborbitalAbort();
	}
}

void ProjectApolloMFD::menuSunburstCOI()
{
	if (lem && lem->ApolloNo == 5) {
		g_Data.uplinkLEM = 1;
		UplinkSunburstCOI();
	}
}

void ProjectApolloMFD::menuAEAACommands()
{
	if (g_Data.uplinkDataReady == 0) {
		bool AEAACommandsInput(void *id, char *str, void *data);
		oapiOpenInputBox("Ascent Engine Arming Assembly. Input: X X. First digit: 1 = Arm APS, 2 = AGS guidance control. Second digit: 1 = set, 2 = reset", AEAACommandsInput, 0, 20, (void*)this);
	}
}

void ProjectApolloMFD::menuSetSource()
{
	if (g_Data.uplinkDataReady == 0) {
		bool SourceInput(void *id, char *str, void *data);
		oapiOpenInputBox("Set Source", SourceInput, 0, 20, (void*)this);
	}
}

void ProjectApolloMFD::menuSetReference()
{
	if (g_Data.uplinkDataReady == 0) {
		bool ReferencePlanetInput(void *id, char *str, void *data);
		oapiOpenInputBox("Set Reference", ReferencePlanetInput, 0, 20, (void*)this);
	}
}

void ProjectApolloMFD::menuSetSVSlot()
{
	if (g_Data.uplinkDataReady == 0) {
		if (g_Data.uplinkSlot == 0) {
			g_Data.uplinkSlot = 1;
		}
		else {
			g_Data.uplinkSlot = 0;
		}
	}
}

void ProjectApolloMFD::menuClearDebugLine()
{
	sprintf(debugString, "");
}

void ProjectApolloMFD::menuFreezeDebugLine()
{
	if (debug_frozen)
		debug_frozen = false;
	else
		debug_frozen = true;
}

void ProjectApolloMFD::menuSetIUSource()
{
	int vesselcount;

	vesselcount = oapiGetVesselCount();

	if (g_Data.targetnumber < vesselcount - 1)
	{
		g_Data.targetnumber++;
	}
	else
	{
		g_Data.targetnumber = 0;
	}

	g_Data.uplinkVessel = oapiGetVesselInterface(oapiGetVesselByIndex(g_Data.targetnumber));
}

void ProjectApolloMFD::menuCycleIUUplinkType()
{
	if (g_Data.iuUplinkType < 10)
	{
		g_Data.iuUplinkType++;
	}
	else
	{
		g_Data.iuUplinkType = 0;
	}

	g_Data.iuUplinkResult = 0;
}

void ProjectApolloMFD::menuSetIUUplinkInp1()
{
	if (g_Data.iuUplinkType == DCSUPLINK_SWITCH_SELECTOR)
	{
		if (g_Data.iuUplinkSwitSelStage < 3)
		{
			g_Data.iuUplinkSwitSelStage++;
		}
		else
		{
			g_Data.iuUplinkSwitSelStage = 0;
		}
	}
	else if (g_Data.iuUplinkType == DCSUPLINK_TIMEBASE_UPDATE)
	{
		bool TimebaseUpdateInput(void *id, char *str, void *data);
		oapiOpenInputBox("Increment the current LVDC timebase time [4-124 seconds]:", TimebaseUpdateInput, 0, 20, (void*)this);
	}
	else if (g_Data.iuUplinkType == DCSUPLINK_SIVBIU_LUNAR_IMPACT)
	{
		bool ImpactTIGInput(void *id, char *str, void *data);
		oapiOpenInputBox("Time of ignition of S-IVB/IU impact burn:", ImpactTIGInput, 0, 20, (void*)this);
	}
	else if (g_Data.iuUplinkType == DCSUPLINK_GENERALIZED_MANEUVER)
	{
		if (g_Data.iuUplinkGenManType < 4)
		{
			g_Data.iuUplinkGenManType++;
		}
		else
		{
			g_Data.iuUplinkGenManType = 0;
		}
	}

	g_Data.iuUplinkResult = 0;
}

void ProjectApolloMFD::menuSetIUUplinkInp2()
{
	if (g_Data.iuUplinkType == DCSUPLINK_SWITCH_SELECTOR)
	{
		bool SwitchSelectorChannelInput(void *id, char *str, void *data);
		oapiOpenInputBox("Switch selector channel [1-112]:", SwitchSelectorChannelInput, 0, 20, (void*)this);
	}
	else if (g_Data.iuUplinkType == DCSUPLINK_SIVBIU_LUNAR_IMPACT)
	{
		bool ImpactBTInput(void *id, char *str, void *data);
		oapiOpenInputBox("Burntime of S-IVB/IU impact burn:", ImpactBTInput, 0, 20, (void*)this);
	}
	else if (g_Data.iuUplinkType == DCSUPLINK_GENERALIZED_MANEUVER)
	{
		bool ImpactTIGInput(void *id, char *str, void *data);
		oapiOpenInputBox("Time of maneuver in TB4:", ImpactTIGInput, 0, 20, (void*)this);
	}

	g_Data.iuUplinkResult = 0;
}

void ProjectApolloMFD::menuSetIUUplinkInp3()
{
	if (g_Data.iuUplinkType == DCSUPLINK_SIVBIU_LUNAR_IMPACT)
	{
		bool ImpactPitchInput(void *id, char *str, void *data);
		oapiOpenInputBox("Pitch of S-IVB/IU impact burn:", ImpactPitchInput, 0, 20, (void*)this);
	}
	else if (g_Data.iuUplinkType == DCSUPLINK_GENERALIZED_MANEUVER)
	{
		bool GeneralizedManeuverAttitudeInput(void *id, char *str, void *data);
		oapiOpenInputBox("Enter attitude in degrees:", GeneralizedManeuverAttitudeInput, 0, 20, (void*)this);
	}

	g_Data.iuUplinkResult = 0;
}

void ProjectApolloMFD::menuSetIUUplinkInp4()
{
	if (g_Data.iuUplinkType == DCSUPLINK_SIVBIU_LUNAR_IMPACT)
	{
		bool ImpactYawInput(void *id, char *str, void *data);
		oapiOpenInputBox("Yaw of S-IVB/IU impact burn:", ImpactYawInput, 0, 20, (void*)this);
	}

	g_Data.iuUplinkResult = 0;
}

bool ProjectApolloMFD::SetImpactTIG(char *rstr)
{
	double f;

	if (sscanf(rstr, "%lf", &f) == 1) {
		g_Data.iuUplinkTIG = f;
		InvalidateDisplay();
		return true;
	}
	return false;
}

bool ProjectApolloMFD::SetImpactBT(char *rstr)
{
	double f;

	if (sscanf(rstr, "%lf", &f) == 1) {
		g_Data.iuUplinkDT = f;
		InvalidateDisplay();
		return true;
	}
	return false;
}

bool ProjectApolloMFD::SetImpactPitch(char *rstr)
{
	double f;

	if (sscanf(rstr, "%lf", &f) == 1) {
		g_Data.iuUplinkPitch = f*RAD;
		InvalidateDisplay();
		return true;
	}
	return false;
}

bool ProjectApolloMFD::SetImpactYaw(char *rstr)
{
	double f;

	if (sscanf(rstr, "%lf", &f) == 1) {
		g_Data.iuUplinkYaw = f*RAD;
		InvalidateDisplay();
		return true;
	}
	return false;
}

bool  ProjectApolloMFD::SetGeneralizedManeuverAttitude(char *rstr)
{
	VECTOR3 Att;

	if (sscanf(rstr, "%lf %lf %lf", &Att.x, &Att.y, &Att.z) == 3) {
		g_Data.iuUplinkGenManAtt = Att * RAD;
		InvalidateDisplay();
		return true;
	}
	return false;
}

void ProjectApolloMFD::menuIUUplink()
{
	if (g_Data.uplinkVessel == NULL)
	{
		g_Data.iuUplinkResult = 2;
		return;
	}

	IU *iu = NULL;

	bool uplinkaccepted = false;

	if (utils::IsVessel(g_Data.uplinkVessel, utils::Saturn)) {
		Saturn *iuv = (Saturn *)g_Data.uplinkVessel;

		iu = iuv->GetIU();
	}
	else if (utils::IsVessel(g_Data.uplinkVessel, utils::SIVB))
	{
		SIVB *iuv = (SIVB *)g_Data.uplinkVessel;

		iu = iuv->GetIU();
	}

	if (iu == NULL)
	{
		g_Data.iuUplinkResult = 2;
		return;
	}

	void *uplink = NULL;

	switch (g_Data.iuUplinkType)
	{
	case DCSUPLINK_SWITCH_SELECTOR:
	{
		DCSSWITSEL upl;

		upl.channel = g_Data.iuUplinkSwitSelChannel;
		upl.stage = g_Data.iuUplinkSwitSelStage;

		uplink = &upl;
		uplinkaccepted = iu->DCSUplink(g_Data.iuUplinkType, uplink);
	}
	break;
	case DCSUPLINK_TIMEBASE_UPDATE:
	{
		DCSTBUPDATE upl;

		upl.dt = g_Data.iuUplinkTimebaseUpdateTime;

		uplink = &upl;
		uplinkaccepted = iu->DCSUplink(g_Data.iuUplinkType, uplink);
	}
	break;
	case DCSUPLINK_LM_ABORT:
	case DCSUPLINK_TDE_ENABLE:
	case DCSUPLINK_RESTART_MANEUVER_ENABLE:
	case DCSUPLINK_TIMEBASE_8_ENABLE:
	case DCSUPLINK_EVASIVE_MANEUVER_ENABLE:
	case DCSUPLINK_EXECUTE_COMM_MANEUVER:
	case DCSUPLINK_REMOVE_INHIBIT_MANEUVER4:
	{
		uplinkaccepted = iu->DCSUplink(g_Data.iuUplinkType, uplink);
	}
	break;
	case DCSUPLINK_SIVBIU_LUNAR_IMPACT:
	{
		DCSLUNARIMPACT upl;

		upl.tig = g_Data.iuUplinkTIG;
		upl.dt = g_Data.iuUplinkDT;
		upl.pitch = g_Data.iuUplinkPitch;
		upl.yaw = g_Data.iuUplinkYaw;

		uplink = &upl;
		uplinkaccepted = iu->DCSUplink(g_Data.iuUplinkType, uplink);
	}
	break;
	case DCSUPLINK_GENERALIZED_MANEUVER:
	{
		DCSGENMANEUVER upl;

		upl.T = g_Data.iuUplinkTIG;
		upl.X = g_Data.iuUplinkGenManAtt.x;
		upl.Y = g_Data.iuUplinkGenManAtt.y;
		upl.Z = g_Data.iuUplinkGenManAtt.z;

		switch (g_Data.iuUplinkGenManType)
		{
		case 0:
			upl.Type = 16;
			break;
		case 1:
			upl.Type = 8;
			break;
		case 2:
			upl.Type = 4;
			break;
		case 3:
			upl.Type = 2;
			break;
		case 4:
			upl.Type = 1;
			break;
		}

		uplink = &upl;
		uplinkaccepted = iu->DCSUplink(g_Data.iuUplinkType, uplink);
	}
	break;
	}

	if (uplinkaccepted)
	{
		g_Data.iuUplinkResult = 1;
	}
	else
	{
		g_Data.iuUplinkResult = 3;
	}
}

void ProjectApolloMFD::menuSetFailure()
{
	bool SaturnFailureInput(void *id, char *str, void *data);
	oapiOpenInputBox("Enter the number of failure type. Optional: condition for failure (0 = mission time, 1 = simulation time, 2+ vessel specific) and condition (time)", SaturnFailureInput, 0, 30, (void*)this);
}

void ProjectApolloMFD::menuResetFailure()
{
	bool SaturnResetFailureInput(void *id, char *str, void *data);
	oapiOpenInputBox("Enter the number of failure type to reset:", SaturnResetFailureInput, 0, 30, (void*)this);
}

void ProjectApolloMFD::menuSetRandomFailures()
{
	bool RandomFailuresInput(void *id, char *str, void *data);
	oapiOpenInputBox("Randomize failures, input multiplier (1.0 for standard failure rate):", RandomFailuresInput, 0, 20, (void*)this);
}

void ProjectApolloMFD::menuClearAllFailures()
{
	if (saturn) saturn->Failures.ClearAllFailures();
}

void ProjectApolloMFD::menuCycleFailuresSubpage()
{
	if (saturn)
	{
		unsigned num = saturn->Failures.GetNumberOfMalfunctions();
		if (num == 0) return;

		unsigned pages = (num - 1) / 10;

		if (FailureSubpage < pages)
		{
			FailureSubpage++;
		}
		else
		{
			FailureSubpage = 0;
		}
	}
}

void ProjectApolloMFD::StoreStatus (void) const
{
	screenData.screen = screen;
}

void ProjectApolloMFD::RecallStatus (void)
{
	screen = screenData.screen;
	m_buttonPages.SelectPage(this, screen);
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

bool SwitchSelectorChannelInput(void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetSwitchSelectorChannel(str);
}

bool TimebaseUpdateInput(void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetTimebaseUpdate(str);
}

bool ImpactTIGInput(void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetImpactTIG(str);
}

bool ImpactBTInput(void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetImpactBT(str);
}

bool ImpactPitchInput(void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetImpactPitch(str);
}

bool ImpactYawInput(void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetImpactYaw(str);
}

bool GeneralizedManeuverAttitudeInput(void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetGeneralizedManeuverAttitude(str);
}

bool SaturnFailureInput(void *id, char *str, void *data)
{
	unsigned type;
	int Condition = 1; //Simulation Time
	double ConditionValue = 0.0; //Immediately

	if (sscanf(str, "%d %d %lf", &type, &Condition, &ConditionValue) >= 1)
	{
		return ((ProjectApolloMFD*)data)->SaturnSetFailureInput(type, Condition, ConditionValue);
	}
	return false;
}

bool SaturnResetFailureInput(void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SaturnResetFailureInput(atoi(str));
}

bool RandomFailuresInput(void *id, char *str, void *data)
{
	double failmult;
	if (sscanf(str, "%lf", &failmult) == 1)
	{
		((ProjectApolloMFD*)data)->SetRandomFailures(failmult);
		return true;
	}
	return false;
}

bool AEAACommandsInput(void *id, char *str, void *data)
{
	int arm, set;
	if (sscanf(str, "%d %d", &arm, &set) == 2)
	{
		((ProjectApolloMFD*)data)->SetAEAACommands(arm, set);
		return true;
	}
	return false;
}

ProjectApolloMFD::ScreenData ProjectApolloMFD::screenData = {0};

DLLCLBK char *pacMFDGetDebugString()
{
	return debugStringBuffer;
}
