/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  LM Telecommunications Implementation

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
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "lmresource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "dsky.h"
#include "IMU.h"

#include "LEM.h"
#include "tracer.h"
#include "papi.h"
#include "CollisionSDK/CollisionSDK.h"

#include "connector.h"
#include "lm_channels.h"

// VHF System (and shared stuff)
LM_VHF::LM_VHF(){
	lem = NULL;
	conn_state = 0;
	uplink_state = 0; rx_offset = 0;
	wsk_error = 0;
	last_update = 0;
	last_rx = 0;
	pcm_rate_override = 0;
}

bool LM_VHF::registerSocket(SOCKET sock)
{
	HMODULE hpac = GetModuleHandle("modules\\startup\\ProjectApolloConfigurator.dll");
	if (hpac) {
		bool (__cdecl *regSocket1)(SOCKET);
		regSocket1 = (bool (__cdecl *)(SOCKET)) GetProcAddress(hpac,"pacDefineSocket");
		if (regSocket1)	{
			if (!regSocket1(sock))
				return false;
		} else {
			return false;
		}
	}
	return true;
}
void LM_VHF::Init(LEM *vessel){
	lem = vessel;
	conn_state = 0;
	uplink_state = 0; rx_offset = 0;
	wsk_error = 0;
	last_update = 0;
	last_rx = 0;
	word_addr = 0;
	pcm_rate_override = 0;
	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR ){
		sprintf(wsk_emsg,"LM-TELECOM: Error at WSAStartup()");
		wsk_error = 1;
		return;
	}
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( m_socket == INVALID_SOCKET ) {
		sprintf(wsk_emsg,"LM-TELECOM: Error at socket(): %ld", WSAGetLastError());
		WSACleanup();
		wsk_error = 1;
		return;
	}
	// Be nonblocking
	int iMode = 1; // 0 = BLOCKING, 1 = NONBLOCKING
	if(ioctlsocket(m_socket, FIONBIO, (u_long FAR*) &iMode) != 0){
		sprintf(wsk_emsg,"LM-TELECOM: ioctlsocket() failed: %ld", WSAGetLastError());
		wsk_error = 1;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	// Set up incoming options
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons( 14243 ); // CM on 14242, LM on 14243

	if ( ::bind( m_socket, (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) {
		sprintf(wsk_emsg,"LM-TELECOM: bind() failed: %ld", WSAGetLastError());
		wsk_error = 1;
		closesocket(m_socket);
		WSACleanup();
		return;
	}
	if ( listen( m_socket, 1 ) == SOCKET_ERROR ){
		wsk_error = 1;
		sprintf(wsk_emsg,"LM-TELECOM: listen() failed: %ld", WSAGetLastError());
		closesocket(m_socket);
		WSACleanup();
		return;
	}
	if(!registerSocket(m_socket))
	{
		sprintf(wsk_emsg,"LM-TELECOM: Failed to register socket %i for cleanup",m_socket);
		wsk_error = 1;
	}
	conn_state = 1; // INITIALIZED, LISTENING
	uplink_state = 0; rx_offset = 0;
}

void LM_VHF::SystemTimestep(double simdt) {
	if(lem == NULL){ return; } // Do nothing if not initialized
	// VHF XMTR A
	// Draws 30 watts of DC when transmitting and 3.5 watts when not.
	if(lem->COMM_VHF_XMTR_A_CB.Voltage() > 24){
		// For now, we'll just draw idle.
		if(lem->VHFAVoiceSwitch.GetState() != THREEPOSSWITCH_CENTER){
			lem->COMM_VHF_XMTR_A_CB.DrawPower(3.5);
		}
	}
	// VHF RCVR A
	// Draws 1.2 watts of DC when on
	if(lem->COMM_VHF_RCVR_A_CB.Voltage() > 24 && lem->VHFARcvrSwtich.GetState() == TOGGLESWITCH_UP){
		lem->COMM_VHF_RCVR_A_CB.DrawPower(1.2);	
	}
	// VHF XMTR B
	// Draws 28.9 watts of DC when transmitting voice and 31.7 watts when transmitting data.
	// Draws 3.5 watts when not transmitting.
	if(lem->COMM_VHF_XMTR_B_CB.Voltage() > 24){
		// For now, we'll just draw idle or data.
		if(lem->VHFBVoiceSwitch.GetState() == THREEPOSSWITCH_UP){
			lem->COMM_VHF_XMTR_B_CB.DrawPower(3.5); // Voice Mode
		}
		if(lem->VHFBVoiceSwitch.GetState() == THREEPOSSWITCH_DOWN){
			lem->COMM_VHF_XMTR_B_CB.DrawPower(31.7); // Data Mode
		}
	}
	// VHF RCVR B
	// Draws 1.2 watts of DC when on
	if(lem->COMM_VHF_RCVR_B_CB.Voltage() > 24 && lem->VHFBRcvrSwtich.GetState() == TOGGLESWITCH_UP){
		lem->COMM_VHF_RCVR_B_CB.DrawPower(1.2);	
	}
	// CDR and LMP Audio Centers
	if(lem->COMM_CDR_AUDIO_CB.Voltage() > 0){ lem->COMM_CDR_AUDIO_CB.DrawPower(4.8); }
	if(lem->COMM_SE_AUDIO_CB.Voltage() > 0){ lem->COMM_SE_AUDIO_CB.DrawPower(4.8); }
	// PMP
	if(lem->COMM_PMP_CB.Voltage() > 0){	lem->COMM_PMP_CB.DrawPower(4.3); }
	// ERAs
	if(lem->INST_SIG_CONDR_1_CB.Voltage() > 0){ lem->INST_SIG_CONDR_1_CB.DrawPower(16.04); }
	if(lem->INST_SIG_CONDR_2_CB.Voltage() > 0){ lem->INST_SIG_CONDR_2_CB.DrawPower(14.23); }
	// FIXME: Need current drain amount for INST_SIG_SENSOR_CB
	// PCMTEA
	if(lem->INST_PCMTEA_CB.Voltage() > 0){ lem->INST_PCMTEA_CB.DrawPower(11); }
}

void LM_VHF::TimeStep(double simt){
	// This stuff has to happen every timestep, regardless of system status.
	if(wsk_error != 0){
		sprintf(oapiDebugString(),"%s",wsk_emsg);
		// return;
	}
	// Allow IO to check for connections, etc
	/*
	if(conn_state != 2){
		last_update = simt; // Don't care about rate
		last_rx = simt;
		perform_io(simt);
		return;
	}
	*/

	// PCMTEA makes the datastream, but for now, don't care about voltage.
	// Otherwise we would abort here (I think)

	// Generate PCM datastream
	if(pcm_rate_override == 1 || (pcm_rate_override == 0 && lem->TLMBitrateSwitch.GetState() == TOGGLESWITCH_DOWN)){
		tx_size = (int)((simt - last_update) / 0.005);
		// sprintf(oapiDebugString(),"Need to send %d bytes",tx_size);
		if(tx_size > 0){
			last_update = simt;
			if(tx_size < 1024){
				tx_offset = 0;
				while(tx_offset < tx_size){
					generate_stream_lbr();
					tx_offset++;
				}
				perform_io(simt);
			}
		}
		return; // Don't waste time checking for HBR
	}
	if(pcm_rate_override == 2 || (pcm_rate_override == 0 && lem->TLMBitrateSwitch.GetState() == TOGGLESWITCH_UP)){
		tx_size = (int)((simt - last_update) / 0.00015625);
		// sprintf(oapiDebugString(),"Need to send %d bytes",tx_size);
		if(tx_size > 0){
			last_update = simt;
			if(tx_size < 1024){
				tx_offset = 0;
				while(tx_offset < tx_size){
					generate_stream_hbr();
					tx_offset++;
				}			
				perform_io(simt);
			}
		}
	}
}

// Scale data to 255 steps for transmission in the PCM datastream.
// This function will be called lots of times inside a timestep, so it should go
// as fast as possible!

unsigned char LM_VHF::scale_data(double data, double low, double high){
	double step = 0;
	
	// First eliminate cases outside of the scales
	if(data >= high){ return 0xFF; }
	if(data <= low){  return 0; }
	
	// Now figure step value
	step = ( ( high - low ) / 256.0);
	// and return result
	return static_cast<unsigned char>( ( ( data - low ) / step ) + 0.5 );
}

void LM_VHF::perform_io(double simt){
	// Do TCP IO
	switch(conn_state){
		case 0: // UNINITIALIZED
			break;
		case 1: // INITALIZED, LISTENING
			// Try to accept
			AcceptSocket = accept( m_socket, NULL, NULL );
			if(AcceptSocket != INVALID_SOCKET){
				conn_state = 2; // Accept this!
				wsk_error = 0; // For now
			}
			// Otherwise loop and try again.
			break;
		case 2: // CONNECTED			
			int bytesSent,bytesRecv;

			bytesSent = send(AcceptSocket, (char *)tx_data, tx_size, 0 );
			if(bytesSent == SOCKET_ERROR){
				long errnumber = WSAGetLastError();
				switch(errnumber){
					// KNOWN CODES that we can ignore
					case 10035: // Operation Would Block
						// We can ignore this entirely. It's not an error.
						break;

					case 10038: // Socket isn't a socket
					case 10053: // Software caused connection abort
					case 10054: // Connection reset by peer
						closesocket(AcceptSocket);
						conn_state = 1; // Accept another
						uplink_state = 0; rx_offset = 0;
						break;

					default:           // If unknown
						wsk_error = 1; // do this
						sprintf(wsk_emsg,"LM-TELECOM: send() failed: %ld",errnumber);
						closesocket(AcceptSocket);
						conn_state = 1; // Accept another
						uplink_state = 0; rx_offset = 0;
						break;					
				}
			}
			// Should we recieve?
			if (((simt - last_rx) / 0.005) < 1 || lem->agc.IsUpruptActive()) {			
				return; // No
			}
			last_rx = simt;
			bytesRecv = recv( AcceptSocket, (char *)(rx_data+rx_offset), 1, 0 );
			if(bytesRecv == SOCKET_ERROR){
				long errnumber = WSAGetLastError();
				switch(errnumber){
					// KNOWN CODES that we can ignore
					case 10035: // Operation Would Block
						// We can ignore this entirely. It's not an error.
						break;

					case 10053: // Software caused connection abort
					case 10038: // Socket isn't a socket
					case 10054: // Connection reset by peer
						closesocket(AcceptSocket);
						conn_state = 1; // Accept another
						uplink_state = 0; rx_offset = 0;
						break;

					default:           // If unknown
						wsk_error = 1; // do this
						sprintf(wsk_emsg,"LM-TELECOM: recv() failed: %ld",errnumber);
						closesocket(AcceptSocket);
						conn_state = 1; // Accept another
						uplink_state = 0; rx_offset = 0;
						break;					
				}
			}else{
				// FIXME: Check to make sure the up-data equipment is powered
				// Reject uplink if switch is not down.
				if(lem->Panel12UpdataLinkSwitch.GetState() != THREEPOSSWITCH_DOWN){
					return; // Discard the data
				}
				if(bytesRecv > 0){
					// Have Data
					switch(uplink_state){
						case 0: // NEW COMMAND START
							int va,sa;
							va = ((rx_data[rx_offset]&070)>>3);
							sa = rx_data[rx_offset]&07;
							// *** VEHICLE ADDRESS HARDCODED HERE *** (NASA DID THIS TOO)
							if(va != 03){ break; }
							switch(sa){
								case 0: // TEST
									rx_offset++; uplink_state=10;
									break;
								case 1: // LGC-UPDATA
									rx_offset++; uplink_state=20;
									break;
								default:
									sprintf(oapiDebugString(),"LM-UPLINK: UNKNOWN SYSTEM-ADDRESS %o",sa);
									break;
							}
							break;
						case 10: // TEST CMD
							rx_offset = 0; uplink_state = 0; break;

						case 20: // LGC UPLINK CMD
							// Expect another byte
							rx_offset++; uplink_state++; break;
						case 21: // LGC UPLINK
							{
								int lgc_uplink_wd = rx_data[rx_offset-1];
								lgc_uplink_wd <<= 8;
								lgc_uplink_wd |= rx_data[rx_offset];
								// Move to INLINK
								lem->agc.vagc.Erasable[0][045] = lgc_uplink_wd;
								// Cause UPRUPT
								lem->agc.GenerateUprupt();

								//sprintf(oapiDebugString(),"LGC UPLINK DATA %05o",cmc_uplink_wd);
								rx_offset = 0; uplink_state = 0;
							}
							break;
					}
				}
			}
			break;			
	}
}

void LM_VHF::generate_stream_hbr(){
	unsigned char data=0;
	// 128 words per frame, 50 frames pre second
	switch(word_addr){
		case 0: tx_data[tx_offset] = 0375;                // SYNC 1
			lem->agc.GenerateDownrupt();				  // And generate DOWNRUPT (We read the data out at 120)
			break;
		case 1: tx_data[tx_offset] = 0312; break;         // SYNC 2
		case 2: tx_data[tx_offset] = 0150; break;         // SYNC 3
		case 3: tx_data[tx_offset] = frame_addr+1; break; // SYNC 4 & FRAME COUNT
		case 4: // ** MAGIC WORD 0 **
		  switch(frame_addr){
		    case 0: tx_data[tx_offset] = measure(01,LTLM_D,0x001); break;
		    case 1: tx_data[tx_offset] = measure(01,LTLM_A,4); break;
		    case 2: tx_data[tx_offset] = measure(01,LTLM_A,8); break;
		    case 3: tx_data[tx_offset] = measure(01,LTLM_A,12); break;
		    case 4: tx_data[tx_offset] = measure(01,LTLM_A,16); break;
		    case 5: tx_data[tx_offset] = measure(01,LTLM_D,0x003); break;
		    case 6: tx_data[tx_offset] = measure(01,LTLM_A,23); break;
		    case 7: tx_data[tx_offset] = measure(01,LTLM_A,27); break;
		    case 8: tx_data[tx_offset] = measure(01,LTLM_A,31); break;
		    case 9: tx_data[tx_offset] = measure(01,LTLM_A,35); break;
		    case 10: tx_data[tx_offset] = measure(01,LTLM_D,0x005); break;
		    case 11: tx_data[tx_offset] = measure(01,LTLM_A,42); break;
		    case 12: tx_data[tx_offset] = measure(01,LTLM_A,46); break;
		    case 13: tx_data[tx_offset] = measure(01,LTLM_A,50); break;
		    case 14: tx_data[tx_offset] = measure(01,LTLM_A,54); break;
		    case 15: tx_data[tx_offset] = measure(01,LTLM_D,0x007); break;
		    case 16: tx_data[tx_offset] = measure(01,LTLM_A,61); break;
		    case 17: tx_data[tx_offset] = measure(01,LTLM_A,65); break;
		    case 18: tx_data[tx_offset] = measure(01,LTLM_A,69); break;
		    case 19: tx_data[tx_offset] = measure(01,LTLM_A,73); break;
		    case 20: tx_data[tx_offset] = measure(01,LTLM_D,0x009); break;
		    case 21: tx_data[tx_offset] = measure(01,LTLM_A,80); break;
		    case 22: tx_data[tx_offset] = measure(01,LTLM_A,84); break;
		    case 23: tx_data[tx_offset] = measure(01,LTLM_A,88); break;
		    case 24: tx_data[tx_offset] = measure(01,LTLM_A,92); break;
		    case 25: tx_data[tx_offset] = measure(01,LTLM_A,96); break;
		    case 26: tx_data[tx_offset] = measure(01,LTLM_A,100); break;
		    case 27: tx_data[tx_offset] = measure(01,LTLM_A,104); break;
		    case 28: tx_data[tx_offset] = measure(01,LTLM_A,108); break;
		    case 29: tx_data[tx_offset] = measure(01,LTLM_A,112); break;
		    case 30: tx_data[tx_offset] = measure(01,LTLM_A,116); break;
		    case 31: tx_data[tx_offset] = measure(01,LTLM_A,120); break;
		    case 32: tx_data[tx_offset] = measure(01,LTLM_A,124); break;
		    case 33: tx_data[tx_offset] = measure(01,LTLM_A,128); break;
		    case 34: tx_data[tx_offset] = measure(01,LTLM_A,132); break;
		    case 35: tx_data[tx_offset] = measure(01,LTLM_A,136); break;
		    case 36: tx_data[tx_offset] = measure(01,LTLM_A,140); break;
		    case 37: tx_data[tx_offset] = measure(01,LTLM_A,144); break;
		    case 38: tx_data[tx_offset] = measure(01,LTLM_A,148); break;
		    case 39: tx_data[tx_offset] = measure(01,LTLM_A,152); break;
		    case 40: tx_data[tx_offset] = measure(01,LTLM_A,156); break;
		    case 41: tx_data[tx_offset] = measure(01,LTLM_A,160); break;
		    case 42: tx_data[tx_offset] = measure(01,LTLM_A,164); break;
		    case 43: tx_data[tx_offset] = measure(01,LTLM_A,168); break;
		    case 44: tx_data[tx_offset] = measure(01,LTLM_A,172); break;
		    case 45: tx_data[tx_offset] = measure(01,LTLM_A,176); break;
		    case 46: tx_data[tx_offset] = measure(01,LTLM_A,180); break;
		    case 47: tx_data[tx_offset] = measure(01,LTLM_A,184); break;
		    case 48: tx_data[tx_offset] = measure(01,LTLM_A,188); break;
		    case 49: tx_data[tx_offset] = measure(01,LTLM_A,192); break;
		  }
		break;
        case 5: tx_data[tx_offset] = measure(200,LTLM_E,0x1A); break;
        case 6: tx_data[tx_offset] = measure(200,LTLM_E,0x1B); break;
        case 7: tx_data[tx_offset] = measure(100,LTLM_E,0x01); break;
        case 8: tx_data[tx_offset] = measure(200,LTLM_A,1); break;
        case 9: tx_data[tx_offset] = measure(200,LTLM_A,2); break;
        case 10: tx_data[tx_offset] = measure(200,LTLM_A,3); break;
        case 11: tx_data[tx_offset] = measure(200,LTLM_A,4); break;
        case 12: tx_data[tx_offset] = measure(200,LTLM_A,5); break;
        case 13: tx_data[tx_offset] = measure(200,LTLM_A,6); break;
        case 14: tx_data[tx_offset] = measure(200,LTLM_A,7); break;
        case 15: tx_data[tx_offset] = measure(100,LTLM_E,0x002); break;
        case 16: tx_data[tx_offset] = measure(100,LTLM_A,1); break;
        case 17: tx_data[tx_offset] = measure(100,LTLM_A,2); break;
        case 18: tx_data[tx_offset] = measure(100,LTLM_A,3); break;
        case 19: tx_data[tx_offset] = measure(100,LTLM_A,4); break;
        case 20: tx_data[tx_offset] = measure(100,LTLM_A,5); break;
        case 21: tx_data[tx_offset] = measure(100,LTLM_A,6); break;
        case 22: tx_data[tx_offset] = measure(100,LTLM_A,7); break;
        case 23: tx_data[tx_offset] = measure(50,LTLM_E,0x001); break;
        case 24: tx_data[tx_offset] = measure(100,LTLM_A,8); break;
        case 25: tx_data[tx_offset] = measure(100,LTLM_A,9); break;
        case 26: tx_data[tx_offset] = measure(100,LTLM_A,10); break;
        case 27: tx_data[tx_offset] = measure(100,LTLM_A,11); break;
        case 28: tx_data[tx_offset] = measure(100,LTLM_A,12); break;
        case 29: tx_data[tx_offset] = measure(100,LTLM_A,13); break;
        case 30: tx_data[tx_offset] = measure(100,LTLM_A,14); break;
        case 31: tx_data[tx_offset] = measure(50,LTLM_E,0x002); break;
		case 32:
		  switch(frame_count){
                    case 0: tx_data[tx_offset] = measure(10,LTLM_D,0x01A); break;
                    case 1: tx_data[tx_offset] = measure(10,LTLM_A,8); break;
                    case 2: tx_data[tx_offset] = measure(10,LTLM_A,18); break;
                    case 3: tx_data[tx_offset] = measure(10,LTLM_A,28); break;
                    case 4: tx_data[tx_offset] = measure(10,LTLM_A,37); break;
		  }
		  break;
		case 33:
		  switch(frame_count){
                    case 0: tx_data[tx_offset] = measure(10,LTLM_D,0x01B); break;
                    case 1: tx_data[tx_offset] = measure(10,LTLM_A,9); break;
                    case 2: tx_data[tx_offset] = measure(10,LTLM_A,19); break;
                    case 3: tx_data[tx_offset] = measure(10,LTLM_A,29); break;
                    case 4: tx_data[tx_offset] = measure(10,LTLM_A,38); break;
		  }
		  break;
		case 34:
		  switch(frame_count){
                    case 0: tx_data[tx_offset] = measure(10,LTLM_D,0x01C); break;
                    case 1: tx_data[tx_offset] = measure(10,LTLM_A,10); break;
                    case 2: tx_data[tx_offset] = measure(10,LTLM_A,20); break;
                    case 3: tx_data[tx_offset] = measure(10,LTLM_A,30); break;
                    case 4: tx_data[tx_offset] = measure(10,LTLM_A,39); break;
		  }
		  break;
		case 35:
		  switch(frame_count){
                    case 0: tx_data[tx_offset] = measure(10,LTLM_D,0x01D); break;
                    case 1: tx_data[tx_offset] = measure(10,LTLM_A,11); break;
                    case 2: tx_data[tx_offset] = measure(10,LTLM_A,21); break;
                    case 3: tx_data[tx_offset] = measure(10,LTLM_A,31); break;
                    case 4: tx_data[tx_offset] = measure(10,LTLM_A,40); break;
		  }
		  break;
		case 36: // ** MAGIC WORD 1 **
		  switch(frame_addr){
		    case 0: tx_data[tx_offset] = measure(01,LTLM_A,1); break;
		    case 1: tx_data[tx_offset] = measure(01,LTLM_A,5); break;
		    case 2: tx_data[tx_offset] = measure(01,LTLM_A,9); break;
		    case 3: tx_data[tx_offset] = measure(01,LTLM_A,13); break;
		    case 4: tx_data[tx_offset] = measure(01,LTLM_A,17); break;
		    case 5: tx_data[tx_offset] = measure(01,LTLM_A,20); break;
		    case 6: tx_data[tx_offset] = measure(01,LTLM_A,24); break;
		    case 7: tx_data[tx_offset] = measure(01,LTLM_A,28); break;
		    case 8: tx_data[tx_offset] = measure(01,LTLM_A,33); break;
		    case 9: tx_data[tx_offset] = measure(01,LTLM_A,36); break;
		    case 10: tx_data[tx_offset] = measure(01,LTLM_A,39); break;
		    case 11: tx_data[tx_offset] = measure(01,LTLM_A,43); break;
		    case 12: tx_data[tx_offset] = measure(01,LTLM_A,47); break;
		    case 13: tx_data[tx_offset] = measure(01,LTLM_A,51); break;
		    case 14: tx_data[tx_offset] = measure(01,LTLM_A,55); break;
		    case 15: tx_data[tx_offset] = measure(01,LTLM_A,58); break;
		    case 16: tx_data[tx_offset] = measure(01,LTLM_A,62); break;
		    case 17: tx_data[tx_offset] = measure(01,LTLM_A,66); break;
		    case 18: tx_data[tx_offset] = measure(01,LTLM_A,70); break;
		    case 19: tx_data[tx_offset] = measure(01,LTLM_A,74); break;
		    case 20: tx_data[tx_offset] = measure(01,LTLM_A,77); break;
		    case 21: tx_data[tx_offset] = measure(01,LTLM_A,81); break;
		    case 22: tx_data[tx_offset] = measure(01,LTLM_A,85); break;
		    case 23: tx_data[tx_offset] = measure(01,LTLM_A,89); break;
		    case 24: tx_data[tx_offset] = measure(01,LTLM_A,93); break;
		    case 25: tx_data[tx_offset] = measure(01,LTLM_A,97); break;
		    case 26: tx_data[tx_offset] = measure(01,LTLM_A,101); break;
		    case 27: tx_data[tx_offset] = measure(01,LTLM_A,105); break;
		    case 28: tx_data[tx_offset] = measure(01,LTLM_A,109); break;
		    case 29: tx_data[tx_offset] = measure(01,LTLM_A,113); break;
		    case 30: tx_data[tx_offset] = measure(01,LTLM_A,117); break;
		    case 31: tx_data[tx_offset] = measure(01,LTLM_A,121); break;
		    case 32: tx_data[tx_offset] = measure(01,LTLM_A,125); break;
		    case 33: tx_data[tx_offset] = measure(01,LTLM_A,129); break;
		    case 34: tx_data[tx_offset] = measure(01,LTLM_A,133); break;
		    case 35: tx_data[tx_offset] = measure(01,LTLM_A,137); break;
		    case 36: tx_data[tx_offset] = measure(01,LTLM_A,141); break;
		    case 37: tx_data[tx_offset] = measure(01,LTLM_A,145); break;
		    case 38: tx_data[tx_offset] = measure(01,LTLM_A,149); break;
		    case 39: tx_data[tx_offset] = measure(01,LTLM_A,153); break;
		    case 40: tx_data[tx_offset] = measure(01,LTLM_A,157); break;
		    case 41: tx_data[tx_offset] = measure(01,LTLM_A,161); break;
		    case 42: tx_data[tx_offset] = measure(01,LTLM_A,165); break;
		    case 43: tx_data[tx_offset] = measure(01,LTLM_A,169); break;
		    case 44: tx_data[tx_offset] = measure(01,LTLM_A,173); break;
		    case 45: tx_data[tx_offset] = measure(01,LTLM_A,177); break;
		    case 46: tx_data[tx_offset] = measure(01,LTLM_A,181); break;
		    case 47: tx_data[tx_offset] = measure(01,LTLM_A,185); break;
		    case 48: tx_data[tx_offset] = measure(01,LTLM_A,189); break;
		    case 49: tx_data[tx_offset] = measure(01,LTLM_A,193); break;
		  }
		break;
        case 37: tx_data[tx_offset] = measure(200,LTLM_E,0x1A); break;
        case 38: tx_data[tx_offset] = measure(200,LTLM_E,0x1B); break;
        case 39: tx_data[tx_offset] = measure(100,LTLM_E,0x03); break;
        case 40: tx_data[tx_offset] = measure(200,LTLM_A,1); break;
        case 41: tx_data[tx_offset] = measure(200,LTLM_A,2); break;
        case 42: tx_data[tx_offset] = measure(200,LTLM_A,3); break;
        case 43: tx_data[tx_offset] = measure(200,LTLM_A,4); break;
        case 44: tx_data[tx_offset] = measure(200,LTLM_A,5); break;
        case 45: tx_data[tx_offset] = measure(200,LTLM_A,6); break;
        case 46: tx_data[tx_offset] = measure(200,LTLM_A,7); break;
        case 47: tx_data[tx_offset] = measure(100,LTLM_E,0x04); break;
        case 48: tx_data[tx_offset] = measure(100,LTLM_A,15); break;
        case 49: tx_data[tx_offset] = measure(100,LTLM_A,16); break;
        case 50: tx_data[tx_offset] = measure(100,LTLM_A,17); break;
        case 51: tx_data[tx_offset] = measure(100,LTLM_A,18); break;
        case 52: tx_data[tx_offset] = measure(100,LTLM_A,19); break;
        case 53: tx_data[tx_offset] = measure(100,LTLM_A,20); break;
        case 54: tx_data[tx_offset] = measure(100,LTLM_A,21); break;
        case 55: tx_data[tx_offset] = measure(100,LTLM_A,22); break;
        case 56: tx_data[tx_offset] = measure(50,LTLM_A,1); break;
        case 57: tx_data[tx_offset] = measure(50,LTLM_A,2); break;
        case 58: tx_data[tx_offset] = measure(50,LTLM_A,3); break;
        case 59: tx_data[tx_offset] = measure(50,LTLM_A,4); break;
        case 60: tx_data[tx_offset] = measure(50,LTLM_A,5); break;
        case 61: tx_data[tx_offset] = measure(50,LTLM_A,6); break;
        case 62: tx_data[tx_offset] = measure(50,LTLM_A,7); break;
        case 63: tx_data[tx_offset] = measure(50,LTLM_A,8); break;
		case 64:
		  switch(frame_count){
                    case 0: tx_data[tx_offset] = measure(10,LTLM_A,1); break;
                    case 1: tx_data[tx_offset] = measure(10,LTLM_A,12); break;
                    case 2: tx_data[tx_offset] = measure(10,LTLM_A,22); break;
                    case 3: tx_data[tx_offset] = measure(10,LTLM_A,32); break;
                    case 4: tx_data[tx_offset] = measure(10,LTLM_A,41); break;
		  }
		  break;
		case 65:
		  switch(frame_count){
                    case 0: tx_data[tx_offset] = measure(10,LTLM_A,2); break;
                    case 1: tx_data[tx_offset] = measure(10,LTLM_A,13); break;
                    case 2: tx_data[tx_offset] = measure(10,LTLM_A,23); break;
                    case 3: tx_data[tx_offset] = measure(10,LTLM_A,33); break;
                    case 4: tx_data[tx_offset] = measure(10,LTLM_A,42); break;
		  }
		  break;
		case 66:
		  switch(frame_count){
                    case 0: tx_data[tx_offset] = measure(10,LTLM_A,3); break;
                    case 1: tx_data[tx_offset] = measure(10,LTLM_A,14); break;
                    case 2: tx_data[tx_offset] = measure(10,LTLM_A,24); break;
                    case 3: tx_data[tx_offset] = measure(10,LTLM_A,34); break;
                    case 4: tx_data[tx_offset] = measure(10,LTLM_A,43); break;
		  }
		  break;
		case 67:
		  switch(frame_count){
                    case 0: tx_data[tx_offset] = measure(10,LTLM_A,4); break;
                    case 1: tx_data[tx_offset] = measure(10,LTLM_A,15); break;
                    case 2: tx_data[tx_offset] = measure(10,LTLM_A,25); break;
                    case 3: tx_data[tx_offset] = measure(10,LTLM_A,35); break;
                    case 4: tx_data[tx_offset] = measure(10,LTLM_A,44); break;
		  }
		  break;
		case 68: // ** MAGIC WORD 2 **
		  switch(frame_addr){
		    case 0: tx_data[tx_offset] = measure(01,LTLM_A,2); break;
		    case 1: tx_data[tx_offset] = measure(01,LTLM_A,6); break;
		    case 2: tx_data[tx_offset] = measure(01,LTLM_A,10); break;
		    case 3: tx_data[tx_offset] = measure(01,LTLM_A,14); break;
		    case 4: tx_data[tx_offset] = measure(01,LTLM_A,18); break;
		    case 5: tx_data[tx_offset] = measure(01,LTLM_A,21); break;
		    case 6: tx_data[tx_offset] = measure(01,LTLM_A,25); break;
		    case 7: tx_data[tx_offset] = measure(01,LTLM_A,29); break;
		    case 8: tx_data[tx_offset] = measure(01,LTLM_A,33); break;
		    case 9: tx_data[tx_offset] = measure(01,LTLM_A,37); break;
		    case 10: tx_data[tx_offset] = measure(01,LTLM_A,40); break;
		    case 11: tx_data[tx_offset] = measure(01,LTLM_A,44); break;
		    case 12: tx_data[tx_offset] = measure(01,LTLM_A,48); break;
		    case 13: tx_data[tx_offset] = measure(01,LTLM_A,52); break;
		    case 14: tx_data[tx_offset] = measure(01,LTLM_A,56); break;
		    case 15: tx_data[tx_offset] = measure(01,LTLM_A,59); break;
		    case 16: tx_data[tx_offset] = measure(01,LTLM_A,63); break;
		    case 17: tx_data[tx_offset] = measure(01,LTLM_A,67); break;
		    case 18: tx_data[tx_offset] = measure(01,LTLM_A,71); break;
		    case 19: tx_data[tx_offset] = measure(01,LTLM_A,75); break;
		    case 20: tx_data[tx_offset] = measure(01,LTLM_A,78); break;
		    case 21: tx_data[tx_offset] = measure(01,LTLM_A,82); break;
		    case 22: tx_data[tx_offset] = measure(01,LTLM_A,86); break;
		    case 23: tx_data[tx_offset] = measure(01,LTLM_A,90); break;
		    case 24: tx_data[tx_offset] = measure(01,LTLM_A,94); break;
		    case 25: tx_data[tx_offset] = measure(01,LTLM_A,98); break;
		    case 26: tx_data[tx_offset] = measure(01,LTLM_A,102); break;
		    case 27: tx_data[tx_offset] = measure(01,LTLM_A,106); break;
		    case 28: tx_data[tx_offset] = measure(01,LTLM_A,110); break;
		    case 29: tx_data[tx_offset] = measure(01,LTLM_A,114); break;
		    case 30: tx_data[tx_offset] = measure(01,LTLM_A,118); break;
		    case 31: tx_data[tx_offset] = measure(01,LTLM_A,122); break;
		    case 32: tx_data[tx_offset] = measure(01,LTLM_A,126); break;
		    case 33: tx_data[tx_offset] = measure(01,LTLM_A,130); break;
		    case 34: tx_data[tx_offset] = measure(01,LTLM_A,134); break;
		    case 35: tx_data[tx_offset] = measure(01,LTLM_A,138); break;
		    case 36: tx_data[tx_offset] = measure(01,LTLM_A,142); break;
		    case 37: tx_data[tx_offset] = measure(01,LTLM_A,146); break;
		    case 38: tx_data[tx_offset] = measure(01,LTLM_A,150); break;
		    case 39: tx_data[tx_offset] = measure(01,LTLM_A,154); break;
		    case 40: tx_data[tx_offset] = measure(01,LTLM_A,158); break;
		    case 41: tx_data[tx_offset] = measure(01,LTLM_A,162); break;
		    case 42: tx_data[tx_offset] = measure(01,LTLM_A,166); break;
		    case 43: tx_data[tx_offset] = measure(01,LTLM_A,170); break;
		    case 44: tx_data[tx_offset] = measure(01,LTLM_A,174); break;
		    case 45: tx_data[tx_offset] = measure(01,LTLM_A,178); break;
		    case 46: tx_data[tx_offset] = measure(01,LTLM_A,182); break;
		    case 47: tx_data[tx_offset] = measure(01,LTLM_A,186); break;
		    case 48: tx_data[tx_offset] = measure(01,LTLM_A,190); break;
		    case 49: tx_data[tx_offset] = measure(01,LTLM_A,194); break;
		  }
		break;
        case 69: tx_data[tx_offset] = measure(200,LTLM_E,0x1A); break;
        case 70: tx_data[tx_offset] = measure(200,LTLM_E,0x1B); break;
        case 71: tx_data[tx_offset] = measure(100,LTLM_E,0x01); break;
        case 72: tx_data[tx_offset] = measure(200,LTLM_A,1); break;
        case 73: tx_data[tx_offset] = measure(200,LTLM_A,2); break;
        case 74: tx_data[tx_offset] = measure(200,LTLM_A,3); break;
        case 75: tx_data[tx_offset] = measure(200,LTLM_A,4); break;
        case 76: tx_data[tx_offset] = measure(200,LTLM_A,5); break;
        case 77: tx_data[tx_offset] = measure(200,LTLM_A,6); break;
        case 78: tx_data[tx_offset] = measure(200,LTLM_A,7); break;
        case 79: tx_data[tx_offset] = measure(100,LTLM_E,0x02); break;
        case 80: tx_data[tx_offset] = measure(100,LTLM_A,1); break;
        case 81: tx_data[tx_offset] = measure(100,LTLM_A,2); break;
        case 82: tx_data[tx_offset] = measure(100,LTLM_A,3); break;
        case 83: tx_data[tx_offset] = measure(100,LTLM_A,4); break;
        case 84: tx_data[tx_offset] = measure(100,LTLM_A,5); break;
        case 85: tx_data[tx_offset] = measure(100,LTLM_A,6); break;
        case 86: tx_data[tx_offset] = measure(100,LTLM_A,7); break;
        case 87: tx_data[tx_offset] = measure(50,LTLM_E,0x03); break;
        case 88: tx_data[tx_offset] = measure(100,LTLM_A,8); break;
        case 89: tx_data[tx_offset] = measure(100,LTLM_A,9); break;
        case 90: tx_data[tx_offset] = measure(100,LTLM_A,10); break;
        case 91: tx_data[tx_offset] = measure(100,LTLM_A,11); break;
        case 92: tx_data[tx_offset] = measure(100,LTLM_A,12); break;
        case 93: tx_data[tx_offset] = measure(100,LTLM_A,13); break;
        case 94: tx_data[tx_offset] = measure(100,LTLM_A,14); break;
        case 95: tx_data[tx_offset] = measure(50,LTLM_E,0x04); break;
        case 96: tx_data[tx_offset] = measure(50,LTLM_D,0x002); break;
		case 97: // ** MAGIC WORD 3 **
		  switch(frame_addr){
		    case 0: tx_data[tx_offset] = measure(10,LTLM_A,5); break;
		    case 1: tx_data[tx_offset] = measure(01,LTLM_E,0x001); break;
		    case 2: tx_data[tx_offset] = measure(01,LTLM_E,0x002); break;
		    case 3: tx_data[tx_offset] = measure(01,LTLM_E,0x003); break;
		    case 4: tx_data[tx_offset] = measure(01,LTLM_E,0x004); break;
		    case 5: tx_data[tx_offset] = measure(10,LTLM_A,5); break;
		    case 6: tx_data[tx_offset] = measure(01,LTLM_E,0x005); break;
		    case 7: tx_data[tx_offset] = measure(01,LTLM_E,0x006); break;
		    case 8: tx_data[tx_offset] = measure(01,LTLM_E,0x007); break;
		    case 9: tx_data[tx_offset] = measure(01,LTLM_E,0x008); break;
		    case 10: tx_data[tx_offset] = measure(10,LTLM_A,5); break;
		    case 11: tx_data[tx_offset] = measure(01,LTLM_E,0x009); break;
		    case 12: tx_data[tx_offset] = measure(01,LTLM_E,0x010); break;
		    case 13: tx_data[tx_offset] = measure(01,LTLM_E,0x011); break;
		    case 14: tx_data[tx_offset] = measure(01,LTLM_E,0x012); break;
		    case 15: tx_data[tx_offset] = measure(10,LTLM_A,5); break;
		    case 16: tx_data[tx_offset] = measure(01,LTLM_E,0x013); break;
		    case 17: tx_data[tx_offset] = measure(01,LTLM_E,0x014); break;
		    case 18: tx_data[tx_offset] = measure(01,LTLM_E,0x015); break;
		    case 19: tx_data[tx_offset] = measure(01,LTLM_E,0x016); break;
		    case 20: tx_data[tx_offset] = measure(10,LTLM_A,5); break;
		    case 21: tx_data[tx_offset] = measure(01,LTLM_E,0x17); break;
		    case 22: tx_data[tx_offset] = measure(01,LTLM_E,0x18); break;
		    case 23: tx_data[tx_offset] = measure(01,LTLM_E,0x19); break;
		    case 24: tx_data[tx_offset] = measure(01,LTLM_E,0x20); break;
		    case 25: tx_data[tx_offset] = measure(10,LTLM_A,5); break;
		    case 26: tx_data[tx_offset] = measure(01,LTLM_E,0x21); break;
		    case 27: tx_data[tx_offset] = measure(01,LTLM_E,0x22); break;
		    case 28: tx_data[tx_offset] = measure(01,LTLM_E,0x23); break;
		    case 29: tx_data[tx_offset] = measure(01,LTLM_E,0x25); break;
		    case 30: tx_data[tx_offset] = measure(10,LTLM_A,5); break;
		    case 31: tx_data[tx_offset] = measure(01,LTLM_E,0x26); break;
		    case 32: tx_data[tx_offset] = measure(01,LTLM_E,0x27); break;
		    case 33: tx_data[tx_offset] = measure(01,LTLM_E,0x28); break;
		    case 34: tx_data[tx_offset] = measure(01,LTLM_E,0x29); break;
		    case 35: tx_data[tx_offset] = measure(10,LTLM_A,5); break;
		    case 36: tx_data[tx_offset] = measure(01,LTLM_E,0x31); break;
		    case 37: tx_data[tx_offset] = measure(01,LTLM_E,0x32); break;
		    case 38: tx_data[tx_offset] = measure(01,LTLM_E,0x33); break;
		    case 39: tx_data[tx_offset] = measure(01,LTLM_E,0x35); break;
		    case 40: tx_data[tx_offset] = measure(10,LTLM_A,5); break;
		    case 41: tx_data[tx_offset] = measure(01,LTLM_E,0x36); break;
		    case 42: tx_data[tx_offset] = measure(01,LTLM_E,0x37); break;
		    case 43: tx_data[tx_offset] = measure(01,LTLM_E,0x38); break;
		    case 44: tx_data[tx_offset] = measure(01,LTLM_E,0x30); break;
		    case 45: tx_data[tx_offset] = measure(10,LTLM_A,5); break;
		    case 46: tx_data[tx_offset] = measure(01,LTLM_E,0x41); break;
		    case 47: tx_data[tx_offset] = measure(01,LTLM_E,0x42); break;
		    case 48: tx_data[tx_offset] = measure(01,LTLM_E,0x43); break;
		    case 49: tx_data[tx_offset] = measure(01,LTLM_E,0x45); break;
		  }
		break;
		case 98:
		  switch(frame_count){
                    case 0: tx_data[tx_offset] = measure(10,LTLM_A,6); break;
                    case 1: tx_data[tx_offset] = measure(10,LTLM_A,16); break;
                    case 2: tx_data[tx_offset] = measure(10,LTLM_A,26); break;
                    case 3: tx_data[tx_offset] = measure(10,LTLM_A,36); break;
                    case 4: tx_data[tx_offset] = measure(10,LTLM_A,45); break;
		  }
		  break;
		case 99: // ** MAGIC WORD 4 **
		  switch(frame_addr){
		    case 0: tx_data[tx_offset] = measure(10,LTLM_A,7); break;
		    case 1: tx_data[tx_offset] = measure(10,LTLM_A,17); break;
		    case 2: tx_data[tx_offset] = measure(10,LTLM_A,27); break;
		    case 3: tx_data[tx_offset] = measure(01,LTLM_D,0x002); break;
		    case 4: tx_data[tx_offset] = measure(10,LTLM_E,0x001); break;
		    case 5: tx_data[tx_offset] = measure(10,LTLM_A,7); break;
		    case 6: tx_data[tx_offset] = measure(10,LTLM_A,17); break;
		    case 7: tx_data[tx_offset] = measure(10,LTLM_A,27); break;
		    case 8: tx_data[tx_offset] = measure(01,LTLM_D,0x004); break;
		    case 9: tx_data[tx_offset] = measure(10,LTLM_E,0x001); break;
		    case 10: tx_data[tx_offset] = measure(10,LTLM_A,7); break;
		    case 11: tx_data[tx_offset] = measure(10,LTLM_A,17); break;
		    case 12: tx_data[tx_offset] = measure(10,LTLM_A,27); break;
		    case 13: tx_data[tx_offset] = measure(01,LTLM_D,0x006); break;
		    case 14: tx_data[tx_offset] = measure(10,LTLM_E,0x001); break;
		    case 15: tx_data[tx_offset] = measure(10,LTLM_A,7); break;
		    case 16: tx_data[tx_offset] = measure(10,LTLM_A,17); break;
		    case 17: tx_data[tx_offset] = measure(10,LTLM_A,27); break;
		    case 18: tx_data[tx_offset] = measure(01,LTLM_D,0x008); break;
		    case 19: tx_data[tx_offset] = measure(10,LTLM_E,0x001); break;
		    case 20: tx_data[tx_offset] = measure(10,LTLM_A,7); break;
		    case 21: tx_data[tx_offset] = measure(10,LTLM_A,17); break;
		    case 22: tx_data[tx_offset] = measure(10,LTLM_A,27); break;
		    case 23: tx_data[tx_offset] = measure(01,LTLM_D,0x10); break;
		    case 24: tx_data[tx_offset] = measure(10,LTLM_E,0x001); break;
		    case 25: tx_data[tx_offset] = measure(10,LTLM_A,7); break;
		    case 26: tx_data[tx_offset] = measure(10,LTLM_A,17); break;
		    case 27: tx_data[tx_offset] = measure(10,LTLM_A,27); break;
		    case 28: tx_data[tx_offset] = measure(01,LTLM_E,0x24); break;
		    case 29: tx_data[tx_offset] = measure(10,LTLM_E,0x001); break;
		    case 30: tx_data[tx_offset] = measure(10,LTLM_A,7); break;
		    case 31: tx_data[tx_offset] = measure(10,LTLM_A,17); break;
		    case 32: tx_data[tx_offset] = measure(10,LTLM_A,27); break;
		    case 33: tx_data[tx_offset] = measure(01,LTLM_E,0x29); break;
		    case 34: tx_data[tx_offset] = measure(10,LTLM_E,0x001); break;
		    case 35: tx_data[tx_offset] = measure(10,LTLM_A,7); break;
		    case 36: tx_data[tx_offset] = measure(10,LTLM_A,17); break;
		    case 37: tx_data[tx_offset] = measure(10,LTLM_A,27); break;
		    case 38: tx_data[tx_offset] = measure(01,LTLM_E,0x34); break;
		    case 39: tx_data[tx_offset] = measure(10,LTLM_E,0x001); break;
		    case 40: tx_data[tx_offset] = measure(10,LTLM_A,7); break;
		    case 41: tx_data[tx_offset] = measure(10,LTLM_A,17); break;
		    case 42: tx_data[tx_offset] = measure(10,LTLM_A,27); break;
		    case 43: tx_data[tx_offset] = measure(01,LTLM_E,0x39); break;
		    case 44: tx_data[tx_offset] = measure(10,LTLM_E,0x001); break;
		    case 45: tx_data[tx_offset] = measure(10,LTLM_A,7); break;
		    case 46: tx_data[tx_offset] = measure(10,LTLM_A,17); break;
		    case 47: tx_data[tx_offset] = measure(10,LTLM_A,27); break;
		    case 48: tx_data[tx_offset] = measure(01,LTLM_E,0x44); break;
		    case 49: tx_data[tx_offset] = measure(10,LTLM_E,0x001); break;
		  }
		break;
		case 100: // ** MAGIC WORD 5 **
		  switch(frame_addr){
		    case 0: tx_data[tx_offset] = measure(01,LTLM_A,3); break;
		    case 1: tx_data[tx_offset] = measure(01,LTLM_A,7); break;
		    case 2: tx_data[tx_offset] = measure(01,LTLM_A,11); break;
		    case 3: tx_data[tx_offset] = measure(01,LTLM_A,15); break;
		    case 4: tx_data[tx_offset] = measure(01,LTLM_A,19); break;
		    case 5: tx_data[tx_offset] = measure(01,LTLM_A,22); break;
		    case 6: tx_data[tx_offset] = measure(01,LTLM_A,26); break;
		    case 7: tx_data[tx_offset] = measure(01,LTLM_A,30); break;
		    case 8: tx_data[tx_offset] = measure(01,LTLM_A,34); break;
		    case 9: tx_data[tx_offset] = measure(01,LTLM_A,38); break;
		    case 10: tx_data[tx_offset] = measure(01,LTLM_A,41); break;
		    case 11: tx_data[tx_offset] = measure(01,LTLM_A,45); break;
		    case 12: tx_data[tx_offset] = measure(01,LTLM_A,49); break;
		    case 13: tx_data[tx_offset] = measure(01,LTLM_A,53); break;
		    case 14: tx_data[tx_offset] = measure(01,LTLM_A,57); break;
		    case 15: tx_data[tx_offset] = measure(01,LTLM_A,60); break;
		    case 16: tx_data[tx_offset] = measure(01,LTLM_A,64); break;
		    case 17: tx_data[tx_offset] = measure(01,LTLM_A,68); break;
		    case 18: tx_data[tx_offset] = measure(01,LTLM_A,72); break;
		    case 19: tx_data[tx_offset] = measure(01,LTLM_A,76); break;
		    case 20: tx_data[tx_offset] = measure(01,LTLM_A,79); break;
		    case 21: tx_data[tx_offset] = measure(01,LTLM_A,83); break;
		    case 22: tx_data[tx_offset] = measure(01,LTLM_A,87); break;
		    case 23: tx_data[tx_offset] = measure(01,LTLM_A,91); break;
		    case 24: tx_data[tx_offset] = measure(01,LTLM_A,95); break;
		    case 25: tx_data[tx_offset] = measure(01,LTLM_A,99); break;
		    case 26: tx_data[tx_offset] = measure(01,LTLM_A,103); break;
		    case 27: tx_data[tx_offset] = measure(01,LTLM_A,107); break;
		    case 28: tx_data[tx_offset] = measure(01,LTLM_A,111); break;
		    case 29: tx_data[tx_offset] = measure(01,LTLM_A,115); break;
		    case 30: tx_data[tx_offset] = measure(01,LTLM_A,119); break;
		    case 31: tx_data[tx_offset] = measure(01,LTLM_A,123); break;
		    case 32: tx_data[tx_offset] = measure(01,LTLM_A,127); break;
		    case 33: tx_data[tx_offset] = measure(01,LTLM_A,131); break;
		    case 34: tx_data[tx_offset] = measure(01,LTLM_A,135); break;
		    case 35: tx_data[tx_offset] = measure(01,LTLM_A,139); break;
		    case 36: tx_data[tx_offset] = measure(01,LTLM_A,143); break;
		    case 37: tx_data[tx_offset] = measure(01,LTLM_A,147); break;
		    case 38: tx_data[tx_offset] = measure(01,LTLM_A,151); break;
		    case 39: tx_data[tx_offset] = measure(01,LTLM_A,155); break;
		    case 40: tx_data[tx_offset] = measure(01,LTLM_A,159); break;
		    case 41: tx_data[tx_offset] = measure(01,LTLM_A,163); break;
		    case 42: tx_data[tx_offset] = measure(01,LTLM_A,167); break;
		    case 43: tx_data[tx_offset] = measure(01,LTLM_A,171); break;
		    case 44: tx_data[tx_offset] = measure(01,LTLM_A,175); break;
		    case 45: tx_data[tx_offset] = measure(01,LTLM_A,179); break;
		    case 46: tx_data[tx_offset] = measure(01,LTLM_A,183); break;
		    case 47: tx_data[tx_offset] = measure(01,LTLM_A,187); break;
		    case 48: tx_data[tx_offset] = measure(01,LTLM_A,191); break;
		    case 49: tx_data[tx_offset] = measure(01,LTLM_A,195); break;
		  }
		break;
        case 101: tx_data[tx_offset] = measure(200,LTLM_E,0x1A); break;
        case 102: tx_data[tx_offset] = measure(200,LTLM_E,0x1B); break;
        case 103: tx_data[tx_offset] = measure(100,LTLM_E,0x03); break;
        case 104: tx_data[tx_offset] = measure(200,LTLM_A,1); break;
        case 105: tx_data[tx_offset] = measure(200,LTLM_A,2); break;
        case 106: tx_data[tx_offset] = measure(200,LTLM_A,3); break;
        case 107: tx_data[tx_offset] = measure(200,LTLM_A,4); break;
        case 108: tx_data[tx_offset] = measure(200,LTLM_A,5); break;
        case 109: tx_data[tx_offset] = measure(200,LTLM_A,6); break;
        case 110: tx_data[tx_offset] = measure(200,LTLM_A,7); break;
        case 111: tx_data[tx_offset] = measure(100,LTLM_E,0x04); break;
        case 112: tx_data[tx_offset] = measure(100,LTLM_A,15); break;
        case 113: tx_data[tx_offset] = measure(100,LTLM_A,16); break;
        case 114: tx_data[tx_offset] = measure(100,LTLM_A,17); break;
        case 115: tx_data[tx_offset] = measure(100,LTLM_A,18); break;
        case 116: tx_data[tx_offset] = measure(100,LTLM_A,19); break;
        case 117: tx_data[tx_offset] = measure(100,LTLM_A,20); break;
        case 118: tx_data[tx_offset] = measure(100,LTLM_A,21); break;
        case 119: tx_data[tx_offset] = measure(100,LTLM_A,22); break;
        case 120: // 50DS1A
		{
			// DOWNRUPT needs time to get data on the bus, so it has to have happened BEFORE we get here!
			ChannelValue ch13;
			ch13 = lem->agc.GetOutputChannel(013);
			data = (lem->agc.GetOutputChannel(034) & 077400) >> 8;
			if (ch13[DownlinkWordOrderCodeBit]) { data |= 0200; } // WORD ORDER BIT
			/*
			sprintf(oapiDebugString(),"LGC DATA: %o (%lo %lo)",data,lem->agc.GetOutputChannel(034),
				lem->agc.GetOutputChannel(035));
			*/
			tx_data[tx_offset] = data;
			break;
		}
        case 121: // 50DS1B
			data = (lem->agc.GetOutputChannel(034)&0377);
			tx_data[tx_offset] = data; 
			break;
		case 122: // 50DS1C
			// PARITY OF CH 34 GOES IN TOP BIT HERE!
			data = (lem->agc.GetOutputChannel(035)&077400)>>8;
			tx_data[tx_offset] = data; 
			break;
        case 123: // 50DS1D
			data = (lem->agc.GetOutputChannel(035)&0377);
			tx_data[tx_offset] = data; 
			break;
        case 124: // 50DS1E
			// PARITY OF CH 35 GOES IN TOP BIT HERE!
			data = (lem->agc.GetOutputChannel(034)&077400)>>8;
			tx_data[tx_offset] = data; 
		case 125: // 50DS2A - AGS DATA
			tx_data[tx_offset] = 0; break;
        case 126: // 50DS2B - AGS DATA
			tx_data[tx_offset] = 0; break;
        case 127: // 50DS2C - AGS DATA
			tx_data[tx_offset] = 0; break;
		// JUST IN CASE
		default:
			tx_data[tx_offset] = 0;
			break;
	}
	word_addr++;
	if(word_addr > 127){
		word_addr = 0;
		frame_addr++;
		if(frame_addr > 49){
			frame_addr = 0;
		}
		frame_count++;
		if(frame_count > 4){
			frame_count = 0;
		}
	}
}

void LM_VHF::generate_stream_lbr(){
	unsigned char data=0;
	// 200 words per frame, 1 frame per second
	switch(word_addr){
		case 0: tx_data[tx_offset] = 0375; break; // SYNC 1
		case 1: tx_data[tx_offset] = 0312; break; // SYNC 2
		case 2: tx_data[tx_offset] = 0150; break; // SYNC 3
		case 3: tx_data[tx_offset] = 0001; break; // SYNC 4 & "FRAME COUNT"
		case 4: tx_data[tx_offset] = measure(01,LTLM_D,0x001); break;
		case 5: tx_data[tx_offset] = measure(01,LTLM_A,5); break;
		case 6: tx_data[tx_offset] = measure(01,LTLM_A,6); break;
		case 7: tx_data[tx_offset] = measure(01,LTLM_A,7); break;
		case 8: tx_data[tx_offset] = measure(01,LTLM_A,8); break;
		case 9: tx_data[tx_offset] = measure(01,LTLM_A,9); break;
		case 10: tx_data[tx_offset] = measure(01,LTLM_A,10); break;
		case 11: tx_data[tx_offset] = measure(01,LTLM_A,11); break;
		case 12: tx_data[tx_offset] = measure(01,LTLM_A,12); break;
		case 13: tx_data[tx_offset] = measure(01,LTLM_A,13); break;
		case 14: tx_data[tx_offset] = measure(01,LTLM_A,14); break;
		case 15: tx_data[tx_offset] = measure(01,LTLM_A,15); break;
		case 16: tx_data[tx_offset] = measure(01,LTLM_A,16); break;
		case 17: tx_data[tx_offset] = measure(01,LTLM_A,17); break;
		case 18: tx_data[tx_offset] = measure(01,LTLM_A,18); break;
		case 19: tx_data[tx_offset] = measure(01,LTLM_A,19); break;
        case 20: tx_data[tx_offset] = measure(01,LTLM_D,0x002); break;
        case 21: tx_data[tx_offset] = measure(01,LTLM_A,20); break;
        case 22: tx_data[tx_offset] = measure(01,LTLM_A,21); break;
        case 23: tx_data[tx_offset] = measure(01,LTLM_A,22); break;
        case 24: tx_data[tx_offset] = measure(01,LTLM_A,23); break;
        case 25: tx_data[tx_offset] = measure(01,LTLM_A,24); break;
        case 26: tx_data[tx_offset] = measure(01,LTLM_A,25); break;
        case 27: tx_data[tx_offset] = measure(01,LTLM_A,26); break;
        case 28: tx_data[tx_offset] = measure(01,LTLM_A,27); break;
        case 29: tx_data[tx_offset] = measure(01,LTLM_A,28); break;
        case 30: tx_data[tx_offset] = measure(01,LTLM_A,29); break;
        case 31: tx_data[tx_offset] = measure(01,LTLM_A,30); break;
        case 32: tx_data[tx_offset] = measure(01,LTLM_D,0x01A); break;
        case 33: tx_data[tx_offset] = measure(01,LTLM_D,0x01B); break;
        case 34: tx_data[tx_offset] = measure(01,LTLM_D,0x01C); break;
        case 35: tx_data[tx_offset] = measure(01,LTLM_D,0x01D); break;
        case 36: tx_data[tx_offset] = measure(01,LTLM_A,35); break;
        case 37: tx_data[tx_offset] = measure(01,LTLM_A,36); break;
        case 38: tx_data[tx_offset] = measure(01,LTLM_A,37); break;
        case 39: tx_data[tx_offset] = measure(01,LTLM_A,38); break;
        case 40: tx_data[tx_offset] = measure(01,LTLM_D,0x003); break;
        case 41: tx_data[tx_offset] = measure(01,LTLM_D,0x004); break;
        case 42: tx_data[tx_offset] = measure(50,LTLM_E,0x001); break;
        case 43: tx_data[tx_offset] = measure(50,LTLM_E,0x002); break;
        case 44: tx_data[tx_offset] = measure(01,LTLM_A,42); break;
        case 45: tx_data[tx_offset] = measure(10,LTLM_A,9); break;
        case 46: tx_data[tx_offset] = measure(10,LTLM_A,14); break;
        case 47: tx_data[tx_offset] = measure(10,LTLM_A,16); break;
        case 48: tx_data[tx_offset] = measure(1,LTLM_A,46); break;
        case 49: tx_data[tx_offset] = measure(10,LTLM_A,19); break;
        case 50: tx_data[tx_offset] = measure(10,LTLM_A,24); break;
        case 51: tx_data[tx_offset] = measure(10,LTLM_A,26); break;
        case 52: tx_data[tx_offset] = measure(1,LTLM_A,50); break;
        case 53: tx_data[tx_offset] = measure(10,LTLM_A,29); break;
        case 54: tx_data[tx_offset] = measure(10,LTLM_A,34); break;
        case 55: tx_data[tx_offset] = measure(10,LTLM_A,36); break;
        case 56: tx_data[tx_offset] = measure(1,LTLM_A,54); break;
        case 57: tx_data[tx_offset] = measure(10,LTLM_A,38); break;
        case 58: tx_data[tx_offset] = measure(10,LTLM_A,43); break;
        case 59: tx_data[tx_offset] = measure(10,LTLM_A,45); break;
        case 60: tx_data[tx_offset] = measure(1,LTLM_D,0x005); break;
        case 61: tx_data[tx_offset] = measure(1,LTLM_A,58); break;
        case 62: tx_data[tx_offset] = measure(1,LTLM_A,59); break;
        case 63: tx_data[tx_offset] = measure(1,LTLM_A,60); break;
        case 64: tx_data[tx_offset] = measure(1,LTLM_A,61); break;
        case 65: tx_data[tx_offset] = measure(1,LTLM_A,62); break;
        case 66: tx_data[tx_offset] = measure(1,LTLM_A,63); break;
        case 67: tx_data[tx_offset] = measure(1,LTLM_A,64); break;
        case 68: tx_data[tx_offset] = measure(1,LTLM_A,65); break;
        case 69: tx_data[tx_offset] = measure(1,LTLM_A,66); break;
        case 70: tx_data[tx_offset] = measure(1,LTLM_A,67); break;
        case 71: tx_data[tx_offset] = measure(1,LTLM_A,68); break;
        case 72: tx_data[tx_offset] = measure(1,LTLM_D,0x006); break;
        case 73: tx_data[tx_offset] = measure(1,LTLM_D,0x007); break;
        case 74: tx_data[tx_offset] = measure(1,LTLM_D,0x008); break;
        case 75: tx_data[tx_offset] = measure(1,LTLM_D,0x009); break;
        case 76: tx_data[tx_offset] = measure(1,LTLM_A,73); break;
        case 77: tx_data[tx_offset] = measure(1,LTLM_A,74); break;
        case 78: tx_data[tx_offset] = measure(1,LTLM_A,75); break;
        case 79: tx_data[tx_offset] = measure(1,LTLM_A,76); break;
        case 80: tx_data[tx_offset] = measure(1,LTLM_D,0x010); break;
        case 81: tx_data[tx_offset] = measure(1,LTLM_E,0x001); break;
        case 82: tx_data[tx_offset] = measure(1,LTLM_E,0x002); break;
        case 83: tx_data[tx_offset] = measure(1,LTLM_E,0x003); break;
        case 84: tx_data[tx_offset] = measure(1,LTLM_A,80); break;
        case 85: tx_data[tx_offset] = measure(1,LTLM_A,81); break;
        case 86: tx_data[tx_offset] = measure(1,LTLM_A,82); break;
        case 87: tx_data[tx_offset] = measure(1,LTLM_A,83); break;
        case 88: tx_data[tx_offset] = measure(1,LTLM_A,84); break;
        case 89: tx_data[tx_offset] = measure(1,LTLM_A,85); break;
        case 90: tx_data[tx_offset] = measure(1,LTLM_A,86); break;
        case 91: tx_data[tx_offset] = measure(1,LTLM_A,87); break;
        case 92: tx_data[tx_offset] = measure(1,LTLM_A,88); break;
        case 93: tx_data[tx_offset] = measure(1,LTLM_A,89); break;
        case 94: tx_data[tx_offset] = measure(1,LTLM_A,90); break;
        case 95: tx_data[tx_offset] = measure(1,LTLM_A,91); break;
        case 96: tx_data[tx_offset] = measure(1,LTLM_A,92); break;
        case 97: tx_data[tx_offset] = measure(1,LTLM_A,93); break;
        case 98: tx_data[tx_offset] = measure(1,LTLM_A,94); break;
        case 99: tx_data[tx_offset] = measure(1,LTLM_A,95); break;
        case 100: tx_data[tx_offset] = measure(1,LTLM_E,0x004); break;
        case 101: tx_data[tx_offset] = measure(1,LTLM_A,97); break;
        case 102: tx_data[tx_offset] = measure(1,LTLM_A,98); break;
        case 103: tx_data[tx_offset] = measure(1,LTLM_A,99); break;
        case 104: tx_data[tx_offset] = measure(1,LTLM_A,100); break;
        case 105: tx_data[tx_offset] = measure(1,LTLM_A,101); break;
        case 106: tx_data[tx_offset] = measure(1,LTLM_A,102); break;
        case 107: tx_data[tx_offset] = measure(1,LTLM_A,103); break;
        case 108: tx_data[tx_offset] = measure(1,LTLM_A,104); break;
        case 109: tx_data[tx_offset] = measure(1,LTLM_A,105); break;
        case 110: tx_data[tx_offset] = measure(1,LTLM_A,106); break;
        case 111: tx_data[tx_offset] = measure(1,LTLM_A,107); break;
        case 112: tx_data[tx_offset] = measure(1,LTLM_E,0x005); break;
        case 113: tx_data[tx_offset] = measure(1,LTLM_E,0x006); break;
        case 114: tx_data[tx_offset] = measure(1,LTLM_E,0x007); break;
        case 115: tx_data[tx_offset] = measure(1,LTLM_E,0x008); break;
        case 116: tx_data[tx_offset] = measure(1,LTLM_A,112); break;
        case 117: tx_data[tx_offset] = measure(1,LTLM_A,113); break;
        case 118: tx_data[tx_offset] = measure(1,LTLM_A,114); break;
        case 119: tx_data[tx_offset] = measure(1,LTLM_A,115); break;
        case 120: tx_data[tx_offset] = measure(1,LTLM_E,0x009); break;
        case 121: tx_data[tx_offset] = measure(1,LTLM_E,0x010); break;
        case 122: tx_data[tx_offset] = measure(1,LTLM_E,0x011); break;
        case 123: tx_data[tx_offset] = measure(1,LTLM_E,0x012); break;
        case 124: tx_data[tx_offset] = measure(1,LTLM_A,120); break;
        case 125: tx_data[tx_offset] = measure(1,LTLM_A,121); break;
        case 126: tx_data[tx_offset] = measure(1,LTLM_A,122); break;
        case 127: tx_data[tx_offset] = measure(1,LTLM_A,123); break;
        case 128: tx_data[tx_offset] = measure(1,LTLM_A,124); break;
        case 129: tx_data[tx_offset] = measure(1,LTLM_A,125); break;
        case 130: tx_data[tx_offset] = measure(1,LTLM_A,126); break;
        case 131: tx_data[tx_offset] = measure(1,LTLM_A,127); break;
        case 132: tx_data[tx_offset] = measure(1,LTLM_A,128); break;
        case 133: tx_data[tx_offset] = measure(1,LTLM_A,129); break;
        case 134: tx_data[tx_offset] = measure(1,LTLM_A,130); break;
        case 135: tx_data[tx_offset] = measure(1,LTLM_A,131); break;
        case 136: tx_data[tx_offset] = measure(1,LTLM_A,132); break;
        case 137: tx_data[tx_offset] = measure(1,LTLM_A,133); break;
        case 138: tx_data[tx_offset] = measure(1,LTLM_A,134); break;
        case 139: tx_data[tx_offset] = measure(1,LTLM_A,135); break;
        case 140: tx_data[tx_offset] = measure(1,LTLM_E,0x013); break;
        case 141: tx_data[tx_offset] = measure(1,LTLM_A,137); break;
        case 142: tx_data[tx_offset] = measure(1,LTLM_A,138); break;
        case 143: tx_data[tx_offset] = measure(1,LTLM_A,139); break;
        case 144: tx_data[tx_offset] = measure(1,LTLM_A,140); break;
        case 145: tx_data[tx_offset] = measure(1,LTLM_A,141); break;
        case 146: tx_data[tx_offset] = measure(1,LTLM_A,142); break;
        case 147: tx_data[tx_offset] = measure(1,LTLM_A,143); break;
        case 148: tx_data[tx_offset] = measure(1,LTLM_A,144); break;
        case 149: tx_data[tx_offset] = measure(1,LTLM_A,145); break;
        case 150: tx_data[tx_offset] = measure(1,LTLM_A,146); break;
        case 151: tx_data[tx_offset] = measure(1,LTLM_A,147); break;
        case 152: tx_data[tx_offset] = measure(1,LTLM_E,0x014); break;
        case 153: tx_data[tx_offset] = measure(1,LTLM_E,0x015); break;
        case 154: tx_data[tx_offset] = measure(1,LTLM_E,0x016); break;
        case 155: tx_data[tx_offset] = measure(1,LTLM_E,0x017); break;
        case 156: tx_data[tx_offset] = measure(1,LTLM_A,152); break;
        case 157: tx_data[tx_offset] = measure(1,LTLM_A,153); break;
        case 158: tx_data[tx_offset] = measure(1,LTLM_A,154); break;
        case 159: tx_data[tx_offset] = measure(1,LTLM_A,155); break;
        case 160: tx_data[tx_offset] = measure(1,LTLM_E,0x018); break;
        case 161: tx_data[tx_offset] = measure(1,LTLM_E,0x019); break;
        case 162: tx_data[tx_offset] = measure(1,LTLM_E,0x020); break;
        case 163: tx_data[tx_offset] = measure(1,LTLM_E,0x021); break;
        case 164: tx_data[tx_offset] = measure(1,LTLM_A,160); break;
        case 165: tx_data[tx_offset] = measure(1,LTLM_A,161); break;
        case 166: tx_data[tx_offset] = measure(1,LTLM_A,162); break;
        case 167: tx_data[tx_offset] = measure(1,LTLM_A,163); break;
        case 168: tx_data[tx_offset] = measure(1,LTLM_A,164); break;
        case 169: tx_data[tx_offset] = measure(1,LTLM_A,165); break;
        case 170: tx_data[tx_offset] = measure(1,LTLM_A,166); break;
        case 171: tx_data[tx_offset] = measure(1,LTLM_A,167); break;
        case 172: tx_data[tx_offset] = measure(1,LTLM_A,168); break;
        case 173: tx_data[tx_offset] = measure(1,LTLM_A,169); break;
        case 174: tx_data[tx_offset] = measure(1,LTLM_A,170); break;
        case 175: tx_data[tx_offset] = measure(1,LTLM_A,171); break;
        case 176: tx_data[tx_offset] = measure(1,LTLM_A,172); break;
        case 177: tx_data[tx_offset] = measure(1,LTLM_A,173); break;
        case 178: tx_data[tx_offset] = measure(1,LTLM_A,174); break;
        case 179: tx_data[tx_offset] = measure(1,LTLM_A,175); break;
        case 180: tx_data[tx_offset] = measure(1,LTLM_E,0x022); break;
        case 181: tx_data[tx_offset] = measure(1,LTLM_A,177); break;
        case 182: tx_data[tx_offset] = measure(1,LTLM_A,178); break;
        case 183: tx_data[tx_offset] = measure(1,LTLM_A,179); break;
        case 184: tx_data[tx_offset] = measure(1,LTLM_A,180); break;
        case 185: tx_data[tx_offset] = measure(1,LTLM_A,181); break;
        case 186: tx_data[tx_offset] = measure(1,LTLM_A,182); break;
        case 187: tx_data[tx_offset] = measure(1,LTLM_A,183); break;
        case 188: tx_data[tx_offset] = measure(1,LTLM_A,184); break;
        case 189: tx_data[tx_offset] = measure(1,LTLM_A,185); break;
        case 190: tx_data[tx_offset] = measure(1,LTLM_A,186); break;
        case 191: tx_data[tx_offset] = measure(1,LTLM_A,187); break;
        case 192: tx_data[tx_offset] = measure(1,LTLM_E,0x023); break;
        case 193: tx_data[tx_offset] = measure(1,LTLM_E,0x024); break;
        case 194: tx_data[tx_offset] = measure(1,LTLM_E,0x025); break;
        case 195: tx_data[tx_offset] = measure(1,LTLM_E,0x026); break;
        case 196: tx_data[tx_offset] = measure(1,LTLM_A,192); break;
        case 197: tx_data[tx_offset] = measure(1,LTLM_A,193); break;
        case 198: tx_data[tx_offset] = measure(1,LTLM_A,194); break;
        case 199: tx_data[tx_offset] = measure(1,LTLM_A,195); break;

		// JUST IN CASE
		default:
			tx_data[tx_offset] = 0;
			break;
	}
	word_addr++;
	if(word_addr > 199){
		word_addr = 0;
		frame_addr = 0;
		frame_count = 1;
	}
}

// Fetch a telemetry data item from its channel code
// FIXME: SCALE FACTORS NEED CHECKING AGAINST REAL DATA

unsigned char LM_VHF::measure(int channel, int type, int ccode){
	unsigned char rdata;
	switch(type){
		case LTLM_A:  // ANALOG
			switch(ccode){
				case 1: 
					if(channel == 1){return(0); } // DPS OX PRESS
					if(channel == 10){ return(0); } // X TRANS CMD
					if(channel == 50){ return(0); } // Y PIPA OUT IN O
					if(channel == 100){return(0); } // IG SVO ERR IN O
					if(channel == 200){return(0); } // DPS FUEL PRESS
				case 2: 
					if(channel == 1){ return(0); } // PLS TORQ REF (???)
					if(channel == 10){return(0); } // SBand ST PH ERR
					if(channel == 50){return(0); } // X PIPA OUT IN O
					if(channel == 200){return(0); } // DPS OX PRESS
				case 3: 
					if(channel == 1){return(0); } // DPS OX 1 TEMP
					if(channel == 100){ return(0); } // MG SVO ERR IN O					
					return(0); // Z PIPA OUT IN O
				case 4:  
					if(channel == 1){return(0); } // DPS FUEL 1 TEMP
					if(channel == 50){return(0); } // VAR INJ ACT POS
					if(channel == 200){return(0); } // DPS FUEL PRESS
					return(0); // P NO2 HE SUP 1
				case 5: 
					if(channel == 1){ return(0); } // IRIG SUSP 3.2 KC (???)
					if(channel == 10){ return(0); } // ROLL ERR CMD
					if(channel == 50){return(0); } // X PIPA OUT IN O
					if(channel == 200){return(0); } // DPS OX PRESS
				case 6: 
					if(channel == 1){ return(0); } // DPS HE PRESS
					if(channel == 200){return(0); } // DPS TCP
					return(0); // Y PIPA OUT IN O
				case 7:  
					if(channel == 200){ return(0); } // APS TCP
					if(channel == 10){ return(0); } // PITCH ATT ERR
					if(channel == 1){ return(0); } // DPS OX 2 QTY
					return(0); // MG RSVR OUT COS
				case 8: 
					if(channel == 1){ return(0); } // QUAD 4 TEMP
					if(channel == 50){return(0); } // VAR INJ ACT POS
					if(channel == 100){ return(0); } // DPS FUEL 1 QTY
					return(0); // Y TRANS CMD
				case 9:
					if(channel == 100){ return(0); } // IG SVO ERR IN O
					if(channel == 10){ return(0); } // MG RSVR OUT SIN
					return(scale_data(lem->Battery5->Voltage(),0,40)); // BAT 5 VOLT 
				case 10: // RR SHFT COS
					return(0);
				case 11: 
					if(channel == 100){ return(0); } // DPS OX 1 QTY
					if(channel == 1){ return(0); } // ROLL GDA POS
					return(0); // RR TRUN SIN
				case 12: 
					if(channel == 10){ return(0); } // MG RSVR OUT COS
					return(scale_data(0,0,1000)); // ASC 1 O2 PRESS
				case 13: 
					if(channel == 10){  return(0); } // ROLL ATT ERR
					if(channel == 100){ return(0); } // OG SVO ERR IN O
					return(0); // ASC 2 H20 TEMP (???)
				case 14: 
					if(channel == 1){ return(0); } // A FUEL MFLD PRESS
					return(0); // IG RSVR OUT SIN
				case 15: 
					if(channel == 1){ return(0); } // B FUEL MFLD PRESS
					return(0); // YAW ATT ERR
				case 16: 
					if(channel == 10){ return(0); } // IG RSVR OUT SIN
					if(channel == 100){ return(0); } // MG SVO ERR IN O
					return(scale_data(0,0,80)); // SEC GLY LOOP PRESS
				case 17: 
					if(channel == 10){ return(0); } // YAW ATT ERR
					return(scale_data(0,0,30)); // CO2 PARTIAL PRESS
				case 18: 
					if(channel == 1){ return(0); } // DPS FUEL PRESS
					return(0); // Z TRANS CMD
				case 19: 
					if(channel == 10){ return(0); } // RGA YAW RATE
					if(channel == 100){ return(0); } // OG SVO ERR IN O
					return(scale_data(0,0,80)); // GLY PUMP PRESS
				case 20: 
					if(channel == 10){ return(0); } // RR TRUN COS
					return(scale_data(0,0,100)); // ASC 1 H20 QTY
				case 21: 
					if(channel == 1){ return(0); } // A OX MFLD PRESS
					if(channel == 100){ return(0); } // DPS FUEL 2 QTY
					return(0); // AUTO THRUST CMD
				case 22: 
					if(channel == 10){ return(0); } // P NO2 HE SUP 2
					return(0); // PITCH GDA POS
				case 23: 
					if(channel == 10){ return(0); } // ROLL ATT ERR
					return(0); // 2.5 VDC TM BIAS
				case 24: 
					if(channel == 10){ return(0); } // OG RSVR OUT SIN
					return(scale_data(0,0,100)); // ASC 2 H20 QTY
				case 25: 
					if(channel == 10){ return(0); } // YAW ERR CMD
					return(scale_data(0,500,3600)); // H2O SEP RPM
				case 26: 
					if(channel == 10){ scale_data(85,0,100); } // CAL 85 PCT
					return(0); // YAW ATT ERR
				case 27: 
					if(channel == 10){ scale_data(15,0,100); } // CAL 15 PCT
					return(0); // ASC 2 H20 TEMP (???)
				case 28: 
					if(channel == 10){ return(0); } // OG RSVR OUT COS
					return(scale_data(0,0,1000)); // ASC 1 O2 PRESS
				case 29: 
					if(channel == 10){ return(0); } // RGA PITCH RATE
					return(scale_data(0,0,80)); // GLY PUMP PRESS
				case 30: 
					if(channel == 1){ return(0); } // DPS HE PRESS
					return(0); // PITCH GDA POS
				case 31: 
					if(channel == 1){ return(0); } // RR ANT TEMP
					return(0); // ROLL GDA POS
				case 32: // RR SHFT SIN
					return(0);
				case 33: 
					if(channel == 10){ return(0); } // PITCH ERR CMD
					if(lem->status < 2){ return(0); }else{ return(0); } // DES H20 PRESS (???)
				case 34: 
					if(channel == 1){ return(0); } // DPS OX 2 TEMP
					return(0); // RGA ROLL RATE
				case 35: 
					if(channel == 1){ return(0); } // QUAD 3 TEMP
					return(0); // PITCH ATT ERR
				case 36: 
					if(channel == 10){ scale_data(15,0,100); } // CAL 15 PCT
					return(scale_data(lem->Battery6->Voltage(),0,40)); // BAT 6 VOLT
				case 37: 
					if(channel == 1){ return(0); } // APS HE REG PRESS
					return(0); // ROLL ATT ERR
				case 38: 
					if(channel == 1){ return(0); } // B OX MFLD PRESS
					return(0); // RGA ROLL RATE
				case 39: // YAW ATT ERR
					return(0);
				case 40: 
					if(channel == 1){ return(0); } // DPS Start Tank Press
					return(0); // MG RSVR OUT SIN
				case 41: 
					if(channel == 10){ scale_data(85,0,100); } // CAL 85 PCT
					if(lem->status < 2){ return(scale_data(lem->Battery2->Voltage(),0,40)); }else{ return(0); } // BAT 2 VOLT
				case 42: 
					if(channel == 1){ return(0); } // DPS He Reg press
					return(0); // PITCH ATT ERR
				case 43: 
					if(channel == 10){ return(0); } // RGA YAW RATE
					return(scale_data(0,0,80)); // SEC GLY LOOP PRESS
				case 44: // B FUEL MFLD PRESS
					return(0);
				case 45: 
					if(channel == 1){ return(0); } // A FUEL MFLD PRESS
					return(0); // RGA PITCH RATE
				case 46: // CO2 PARTIAL PRESS
					return(scale_data(0,0,30));
				case 47: // DPS FUEL 1 TEMP
					return(0);
				case 48: // DPS OX 1 TEMP
					return(0);
				case 49: // PLS TORQ REF (???)
					return(0);
				case 50: // ASC 2 H20 QTY
					return(scale_data(0,0,100));
				case 51: // 2.5 VDC TM BIAS
					return(0);
				case 52: // B OX MFLD PRESS
					return(0);
				case 53: // A OX MFLD PRESS
					return(0);
				case 54: // ASC 1 H20 QTY
					return(scale_data(0,0,100));
				case 55: // RR ANT TEMP
					return(0);
				case 56: // DPS OX 2 TEMP
					return(0);
				case 57: // BAT 4 VOLT
					if(lem->status < 2){ return(scale_data(lem->Battery4->Voltage(),0,40)); }else{ return(0); }
				case 58: // DES H20 QTY
					if(lem->status < 2){ return(scale_data(0,0,100)); }else{ return(0); }
				case 59: // PRI GLY PUMP P
					return(scale_data(0,0,80));
				case 60: // PITCH ATT ERR
					return(0);
				case 61: // APS OX PRESS
					return(0);
				case 62: // APS FUEL PRESS
					return(0);
				case 63: // BAT 3 CUR
					if(lem->status < 2){ return(scale_data(lem->Battery3->Current(),0,120)); }else{ return(0); }
				case 64: // OG RSVR OUT COS
					return(0); 
				case 65: // ASA TEMP
					return(0);
				case 66: // RCS A FUEL TEMP
					return(0);
				case 67: // BAT 4 CUR
					if(lem->status < 2){ return(scale_data(lem->Battery4->Current(),0,120)); }else{ return(0); }
				case 68: // BAT 5 CUR
					return(scale_data(lem->Battery5->Current(),0,120));
				case 69: // DPS FUEL 2 QTY
					return(0);
				case 70: // REG OUT MANIFOLD
					return(0);
				case 71: // UNKNOWN, HBR
					return(0);
			    case 72: // BAT 1 VOLT
					if(lem->status < 2){ return(scale_data(lem->Battery1->Voltage(),0,40)); }else{ return(0); }
				case 73: // ASC 2 O2 PRESS
					return(scale_data(0,0,1000));
				case 74: // RCS B FUEL TEMP
					return(0);
				case 75: // BAT 6 CUR
					return(scale_data(lem->Battery6->Current(),0,120));
				case 76: // DPS TCP
					return(0);
				case 77: // DPS FUEL 1 QTY
					return(0);
				case 78: // UNKNOWN, HBR
					return(0);
				case 79: // BAT 3 VOLT
					if(lem->status < 2){ return(scale_data(lem->Battery3->Voltage(),0,40)); }else{ return(0); }
				case 80: // QUAD 2 TEMP
					return(0);
				case 81: // VHF B RX AGC
					return(0);
			    case 82: // AC BUS VOLT
					return(scale_data(lem->ACBusA.Voltage(),100,150));
				case 83: // CABIN PRESS
					return(scale_data(0,0,10));
				case 84: // QUAD 1 TEMP
					return(0);
				case 85: // PCM OSC FAIL 3
					return(0);
				case 86: // DPS TCP
					return(0);
				case 87: // BAT 6 CUR
					return(scale_data(lem->Battery6->Current(),0,120));
				case 88: // DES H20 QTY
					if(lem->status < 2){ return(scale_data(0,0,100)); }else{ return(0); }
				case 89: // ASA TEMP
					return(0);
				case 90: // O2 MANIFOLD PRESS (???)
					return(scale_data(0,0,1000));
				case 91: // BAT 1 CUR
					if(lem->status < 2){ return(scale_data(lem->Battery1->Current(),0,120)); }else{ return(0); }
				case 92: // APS FUEL PRESS
					return(0);
				case 93: // APS HE 2 PRESS
					return(0);
				case 94: // BAT 4 CUR
					if(lem->status < 2){ return(scale_data(lem->Battery4->Current(),0,120)); }else{ return(0); }
				case 95: // BAT 3 CUR
					if(lem->status < 2){ return(scale_data(lem->Battery3->Current(),0,120)); }else{ return(0); }
				case 96: // VHF B RX AGC
					return(0);
				case 97: // ASC 2 O2 PRESS
					return(scale_data(0,0,1000));
				case 98: // CABIN PRESS
					return(scale_data(0,0,10));
				case 99: // PRI GLY PUMP P
					return(scale_data(0,0,80));
				case 100: // APS HE 2 PRESS
					return(0);
				case 101: // DPS FUEL 2 QTY
					return(0);
				case 102: // BAT 5 CUR
					return(scale_data(lem->Battery5->Current(),0,120));
				case 103: // O2 MANIFOLD PRESS (???)
					return(scale_data(0,0,1000));
				case 104: // DPS FUEL 1 QTY
					return(0);
				case 105: // APS OX PRESS
					return(0);
				case 106: // BAT 1 CUR
					if(lem->status < 2){ return(scale_data(lem->Battery1->Current(),0,120)); }else{ return(0); }
				case 107: // S-BAND ANT TEMP
					return(0);
				case 108: // DPS He Reg press
					return(0); 
				case 109: // UNKNOWN, HBR
					return(0);
				case 110: // RCS A REG PRESS
					return(0);
				case 111: // UNKNOWN, HBR
					return(0);
				case 112: // RCS A REG PRESS
					return(0);
				case 113: // DPS BALL VLV TEMP
					return(0);
				case 114: // APS FUEL TEMP
					return(0);
				case 115: // ROLL ATT ERR
					return(0);
				case 116: // DPS OX 1 QTY
					return(0);
				case 117: // DPS FUEL 2 TEMP
					return(0);
				case 118: // UNKNOWN, HBR
					return(0);
				case 119: // RCS PROP B QTY
					return(0);
				case 120: // GLY TEMP
					return(scale_data(0,20,120));
				case 121: // IMU 28 VAC 800 (???)
					return(0);
				case 122: // BAT 2 CUR
					if(lem->status < 2){ return(scale_data(lem->Battery2->Current(),0,120)); }else{ return(0); }
				case 123: // PIPA TEMP
					return(0); 
				case 124: // RCS B HE PRESS
					return(0);
				case 125: // RCS A HE PRESS
					return(0);
				case 126: // CDR BUS VOLT
					return(scale_data(lem->CDRs28VBus.Voltage(),0,40));
				case 127: // "W/B GLY OUT TEMP" = Main Sublimator Outlet Temp
					return(scale_data(0,20,120));
				case 128: // CABIN TEMP
					return(scale_data(0,20,120));
				case 129: // APS OX TEMP
					return(0);
				case 130: // ECS SUIT PRESS
					return(scale_data(0,0,10));
				case 131: // DES O2 PRESS
					if(lem->status < 2){ return(scale_data(0,0,3000)); }else{ return(0); }
				case 132: // MAN THRUST CMD
					return(0);
				case 133: // DPS OX 2 QTY
					return(0); 
				case 134: // SE BUS VOLT
					return(scale_data(lem->LMPs28VBus.Voltage(),0,40));
				case 135: // ASC 1 H20 TEMP (???)
					return(0);
				case 136: // UNKNOWN, HBR
					return(0);
				case 137: // "W/B GLY IN TEMP" = Main Sublimator Inlet Temp
					return(scale_data(0,20,120));
				case 138: // RR ANT TEMP
					return(0);
				case 139: // YAW ERR CMD
					return(0);
				case 140: // RCS B REG PRESS
					return(0);
				case 141: // PRI H2O REG DP
					return(scale_data(0,0,2));
				case 142: // DES O2 PRESS
					if(lem->status < 2){ return(scale_data(0,0,3000)); }else{ return(0); }
				case 143: // SE BUS VOLT
					return(scale_data(lem->LMPs28VBus.Voltage(),0,40));
				case 144: // APS OX TEMP
					return(0);
				case 145: // RCS B HE PRESS
					return(0);
				case 146: // RCS PROP B QTY
					return(0);
				case 147: // ECS SUIT PRESS
					return(scale_data(0,0,10));
				case 148: // DPS FUEL 2 TEMP
					return(0);
				case 149: // LR ANT TEMP
					return(0);
				case 150: // UNKNOWN, HBR
					return(0);
				case 151: // RCS PROP A QTY
					return(0);
				case 152: // SBand RX SIG
					return(0);
				case 153: // APS HE 1 PRESS
					return(0);
				case 154: // "PRI W/B H20 TEMP" = Main Sublimator Inlet Water Temp
					return(scale_data(0,20,160));
				case 155: // PITCH ERR CMD
					return(0);
				case 156: // UNKNOWN, HBR
					return(0);
				case 157: // UNKNOWN, HBR
					return(0);
			    case 158: // AC BUS FREQ
					return(scale_data(400,380,420));
				case 159: // UNKNOWN, HBR
					return(0);
				case 160: // RCS A HE PRESS
					return(0);
				case 161: // SBand RX SIG
					return(0);
				case 162: // RCS PROP A QTY
					return(0);
				case 167: // BAT 2 CUR
					if(lem->status < 2){ return(scale_data(lem->Battery2->Current(),0,120)); }else{ return(0); }
				case 163: // "PRI W/B H20 TEMP" = Main Sublimator Inlet Water Temp
					return(scale_data(0,20,160));
				case 164: // LR ANT TEMP
					return(0);
				case 165: // DPS OX 1 QTY
					return(0); 
				case 166: // PIPA TEMP
					return(0); 
				case 168: // PRI H2O REG DP
					return(scale_data(0,0,2));
				case 169: // GLY TEMP
					return(scale_data(0,20,120));
				case 170: // ASC 1 H20 TEMP (???)
					return(0);
				case 171: // PCM OSC FAIL 2
					return(0);
				case 172: // ECS SUIT TEMP
					return(scale_data(0,20,120));
				case 173: // APS TCP
					return(0);
			    case 174: // AC BUS FREQ
					if(lem->ACBusB.Voltage() > 0){ return(scale_data(400,380,420)); } else{ return(0); }
				case 175: // ROLL ERR CMD
					return(0); 
				case 176: // DPS OX 1 QTY
					return(0); 
				case 177: // ECS SUIT TEMP
					return(scale_data(0,20,120));
				case 178: // APS TCP
					return(0);
				case 179: // APS HE 1 PRESS
					return(0);
				case 180: // RTG CASK SHIELD TEMP
					return(0);
				case 181: // UNKNOWN, LBR
					return(0);
				case 182: // UNKNOWN, LBR
					return(0);
				case 183: // F/H RLF PRESS (???)
					return(0);
				case 184: // UNKNOWN, LBR
					return(0);
				case 185: // SBand ST PH ERR
					return(0);
				case 186: // AUTO THRUST CMD
					return(0);
				case 187: // UNKNOWN, HBR
					return(0);
				case 188: // UNKNOWN, HBR
					return(0);
				case 189: // UNKNOWN, HBR
					return(0);
				case 190: // UNKNOWN, HBR
					return(0);
				case 191: // UNKNOWN, HBR
					return(0);
				case 192: // UNKNOWN, HBR
					return(0);
				case 193: // VAR INJ ACT POS
					return(0);
				case 194: // U/H RLF PRESS (???)
					return(0);
				case 195: // SBand XMTR PO
					return(0);
				default:
					sprintf(oapiDebugString(),"MEASURE: UNKNOWN A-%d",ccode);
					break;
			}
		break;
		case LTLM_D:  // DIGITAL
			switch(ccode){
				case 0x001: // FORMAT ID (???)
					return(0);
				case 0x002: // DUA STATUS (DIGITAL UPLINK ASSEMBLY)
					return(0);
				case 0x003: // UNKNOWN, LBR
					return(0);
				case 0x004: // UNKNOWN, LBR
					return(0);
				case 0x005: // UNKNOWN, LBR
					return(0);
				case 0x006: // UNKNOWN, LBR
					return(0);
				case 0x007: // UNKNOWN, LBR
					return(0);
				case 0x008: // UNKNOWN, LBR
					return(0);
				case 0x009: // UNKNOWN, LBR
					return(0);
				case 0x010: // UNKNOWN, LBR
					return(0);
				case 0x01A: // MET
					return(0);
				case 0x01B: // MET
					return(0);
				case 0x01C: // MET
					return(0);
				case 0x01D: // MET
					return(0);
				default:
					sprintf(oapiDebugString(),"MEASURE: UNKNOWN D-0x%x",ccode);
					break;
			}
		break;
		case LTLM_DS:  // DIGITAL SERIAL
			//switch(ccode){
			//	default:
					sprintf(oapiDebugString(),"MEASURE: UNKNOWN DS-0x%x",ccode);
			//		break;
			//}
		break;
		case LTLM_E:  // EVENT
			if(channel == 200){
				// Channel 100
				switch(ccode){
					case 0x01A: // RCS TCP nn
						// Bit 8 = 3S
						// Bit 7 = 3F
						// Bit 6 = 3D
						// Bit 5 = 3U
						// Bit 4 = 4S
						// Bit 3 = 4F
						// Bit 2 = 4D
						// Bit 1 = 4U
						return 0;
					case 0x01B: // RCS TCP nn
						// Bit 8 = 1S
						// Bit 7 = 1F
						// Bit 6 = 1D
						// Bit 5 = 1U
						// Bit 4 = 2S
						// Bit 3 = 2F
						// Bit 2 = 2D
						// Bit 1 = 2U
						return 0;
					default:
						sprintf(oapiDebugString(),"MEASURE: UNKNOWN 200-E-0x%x",ccode);
						break;
				}
			}
			if(channel == 100){
				// Channel 100
				switch(ccode){
					case 0x01: // RCS JET DRIVERS
					case 0x02:
						// Bit 8 = JD 1D OUTPUT
						// Bit 7 = JD 1U OUTPUT
						// Bit 6 = JD 2D OUTPUT
						// Bit 5 = JD 2U OUTPUT
						// Bit 4 = JD 3D OUTPUT
						// Bit 3 = JD 3U OUTPUT
						// Bit 2 = JD 4D OUTPUT
						// Bit 1 = JD 4U OUTPUT
						return 0;
					case 0x03: // UNKNOWN, HBR
						return 0;
					case 0x04: // MORE RCS JET DRIVERS
						// Bit 8 = JD 1S OUTPUT
						// Bit 7 = JD 1F OUTPUT
						// Bit 6 = JD 2S OUTPUT
						// Bit 5 = JD 2F OUTPUT
						// Bit 4 = JD 3S OUTPUT
						// Bit 3 = JD 3F OUTPUT
						// Bit 2 = JD 4S OUTPUT
						// Bit 1 = JD 4F OUTPUT
						return 0;
					default:
						sprintf(oapiDebugString(),"MEASURE: UNKNOWN 100-E-0x%x",ccode);
						break;
				}
			}
			if(channel == 50){
				// Channel 50
				switch(ccode){
					case 0x01: // ???
					case 0x02:
						// Bit 8 = APS On
						// Bit 7 = Abort Stage
						return 0;
					case 0x03: // UNKNOWN, HBR
					case 0x04:
						return 0;
					default:
						sprintf(oapiDebugString(),"MEASURE: UNKNOWN 50-E-0x%x",ccode);
						break;
				}
			}
			// Channel 1
			switch(ccode){
				case 0x01: // PNGS Statuses
				case 0x12: // PNGS Statuses
					// Bit 8 = LR Range Bad
					// Bit 7 = LR Vel Bad
					// Bit 6 = RR No Track
					return 0;
				case 0x02: // PNGS Statuses
				case 0x13: // PNGS Statuses
					// Bit 8 = LGC Warning
					// Bit 7 = ISS Warning
					return 0;
				case 0x03: // ECS Statuses
				case 0x14: // ECS Statuses
					// Bit 8 = CDR Suit Disc
					// Bit 7 = SE Suit Disc
					// Bit 6 = Selected Gly Level Low
					// Bit 5 = Repr Elec Open
					return 0;
				case 0x04: // ECS Statuses
				case 0x15: // ECS Statuses
					// Bit 8 = Cabin Ret Closed
					// Bit 7 = Cabin Ret Open
					// Bit 6 = Demand Reg A Closed
					// Bit 4 = Demand Reg B Closed
					// Bit 2 = Sec Gly Pump Fail
					return 0;
				case 0x05: // ECS Statuses
				case 0x16: // ECS Statuses
					// Bit 8 = Suit Fan 1 Malf
					// Bit 7 = Suit Fan 2 Malf
					// Bit 6 = Suit Rlf Closed
					// Bit 5 = Suit Rlf Open
					// Bit 4 = Suit Div Egress
					// Bit 3 = Sec CO2 Sel
					return 0;
				case 0x06: // PNGS Statuses
				case 0x17: // PNGS Statuses
					// Bit 8 = IMU STBY
					// Bit 7 = IMU OPR
					return 0;
				case 0x07: // IS Statuses
				case 0x18: // IS Statuses
					// Bit 8 = CES AC Power Fail
					// Bit 7 = CES DC Power Fail
					// Bit 6 = AGS Power Fail
					// Bit 5 = C&W Power Fail
					// Bit 4 = Master Alarm On
					// Bit 3 = EPS Battery Caution
					// Bit 1 = PCM Osc Fail 1
					return 0;
				case 0x08: // ???
				case 0x19: // ???
					// UNKNOWN - LBR
					return 0;
				case 0x09: // ???
				case 0x20: // ???
					// UNKNOWN - LBR
					return 0;
				case 0x10: // RCS Statuses
				case 0x21: // RCS Statuses
					// Bit 8 = SIG ASC Feed A OX Open
					// Bit 7 = SIG ASC Feed B OX Open
					// Bit 6 = RCS MAIN A CLOSED
					// Bit 5 = RCS MAIN B CLOSED
					// Bit 4 = ASC Feed A Open
					// Bit 3 = ASC Feed B Open
					// Bit 2 = A/B Crossfeed Open
					return 0;
				case 0x11: // EDS Statuses
				case 0x22: // EDS Statuses
					// Bit 8 = Abort Command
					// Bit 5 = ED Relay A K7-K15
					// Bit 4 = ED Relay B K7-K15
					// Bit 3 = ED Relay A K3-K6
					// Bit 2 = ED Relay B K3-K6
					return 0;
				case 0x23: // ???
					// Bit 8 = Auto On
					// Bit 7 = DPS On
					// Bit 6 = Pitch Trim Fail
					// Bit 5 = Roll Trim Fail
					// Bit 4 = AGS Selected
					return 0;
				case 0x24: // ???
					// Bit 8 = APS Fuel Low
					// Bit 7 = APS Ox Low
					return 0;
				case 0x25: // UNKNOWN, LBR
				case 0x26: // UNKNOWN, LBR
					return(0);
				case 0x27: // ???
					// Bit 8 = Roll Pulsed/Direct
					// Bit 7 = Pitch Pulsed/Direct
					// Bit 6 = Yaw Pulsed/Direct
					// Bit 5 = AGS Warmup
					// Bit 4 = AGS Standby
					return 0;
				case 0x28: // UNKNOWN, HBR
					return 0;
				case 0x29: // Battery Malfunction Flags
					rdata = 0;
					if(lem->status < 2){
						// Bits 8-5 = Bat 1-4 malf
					}
					// Bits 4,3 = bat 5,6 malf
					return rdata;
				case 0x30: // Descent Battery Status Flags
					if(lem->status < 2){
						rdata = 0;
						if(lem->ECA_1a.input == 1){ rdata |= 0x01; } // B1 HI tap
						if(lem->ECA_1a.input == 2){ rdata |= 0x02; } // B1 LO tap
						if(lem->ECA_1b.input == 1){ rdata |= 0x04; } // B2 HI tap
						if(lem->ECA_1b.input == 2){ rdata |= 0x08; } // B2 LO tap
						if(lem->ECA_2a.input == 1){ rdata |= 0x10; } // B3 HI tap
						if(lem->ECA_2a.input == 2){ rdata |= 0x20; } // B3 LO tap
						if(lem->ECA_2b.input == 1){ rdata |= 0x40; } // B4 HI tap
						if(lem->ECA_2b.input == 2){ rdata |= 0x80; } // B4 LO tap
						return rdata;
					}
					return 0;
				case 0x31: // ???
					// Bit 8 = Out Det
					// Bit 7 = Auto Off
					// Bit 6 = Engine Fire Override
					// Bit 5 = PNGS Mode Auto
					// Bit 4 = PNGS Mode Att Hold
					// Bit 3 = Unbalanced Couples
					// Bit 2 = AGS Mode Att Hold
					// Bit 1 = AGS Mode Auto
					return 0;
				case 0x32: // DPS Statuses
					// Bit 7 = DES Prop Lo
					// Bit 6 = PROP VLVS DEL P
					// Bit 5 = PROP VLVS DEL P
					return 0;
				case 0x33: // Ascent Battery Status Flags
					rdata = 0;
					if(lem->ECA_3b.input == 1){ rdata |= 0x01; } // B5 Backup
					if(lem->ECA_4a.input == 1){ rdata |= 0x02; } // B6 Normal
					if(lem->ECA_3a.input == 1){ rdata |= 0x04; } // B5 Normal
					if(lem->ECA_4b.input == 1){ rdata |= 0x08; } // B6 Backup
					return rdata;
				case 0x34: // UNKNOWN, HBR
					return 0;
				case 0x35: // UNKNOWN, HBR
					return 0;
				case 0x36: // UNKNOWN, HBR
					return 0;
				case 0x37: // UNKNOWN, HBR
					return 0;
				case 0x38: // UNKNOWN, HBR
					return 0;
				case 0x39: // UNKNOWN, HBR
					return 0;
				case 0x40: // ???
					// Bit 8 = Landing Gear Deploy
					return 0;
				case 0x41: // ???
					// Bit 8 = APS Arm
					// Bit 7 = DPS Arm
					// Bit 6 = Min Deadband
					// Bit 5 = X Trans Override
					return 0;
				case 0x42: // RCS Isolation valve xx closed
					// Bit 8 = 4A 
					// Bit 7 = 4B
					// Bit 6 = 3A
					// Bit 5 = 3B
					// Bit 4 = 2A
					// Bit 3 = 2B
					// Bit 2 = 1A
					// Bit 1 = 1B
					return 0;
				case 0x43: // ???
					// Bit 8 = APS He 1 Closed
					// Bit 7 = APS He 2 Closed
					return 0;
				case 0x44: // UNKNOWN, HBR
					return 0;
				case 0x45: // UNKNOWN, HBR
					return 0;
				default:
					sprintf(oapiDebugString(),"MEASURE: UNKNOWN E-0x%x",ccode);
					break;
			}
		break;
	}
	return (0);
}

// S-Band System
LM_SBAND::LM_SBAND(){
	lem = NULL;
	pa_mode_1 = 0; pa_timer_1 = 0;
	pa_mode_2 = 0; pa_timer_2 = 0;
	tc_mode_1 = 0; tc_timer_1 = 0;
	tc_mode_2 = 0; tc_timer_2 = 0;

}

void LM_SBAND::Init(LEM *vessel){
	lem = vessel;
}

void LM_SBAND::SystemTimestep(double simdt) {
	if(lem == NULL){ return; } // Do nothing if not initialized
	// SBand Transcievers take 30 seconds to warm up when turned on, or 5 seconds to change modulation modes.
	// SBand Power Amplifiers take 60 seconds to warm up when turned on, and 20 seconds to recycle if RF is interrupted.
	// SBand Primary Transciever
	// Pulls 36 watts operating.
	if(lem->SBandXCvrSelSwitch.GetState() == THREEPOSSWITCH_UP){
		if(tc_mode_1 == 0){ tc_mode_1 = 2; } // Start warming up
	}else{
		if(tc_mode_1 > 1){ tc_mode_1 = 1; } // Start cooling off
	}
	// SBand Secondary Transciever
	// Pulls 36 watts operating.
	if(lem->SBandXCvrSelSwitch.GetState() == THREEPOSSWITCH_DOWN){
		if(tc_mode_2 == 0){ tc_mode_2 = 2; } // Start warming up
	}else{
		if(tc_mode_2 > 1){ tc_mode_2 = 1; } // Start cooling off
	}	
	// SBand Primary PA
	// Pulls 72 watts operating
	if(lem->SBandPASelSwitch.GetState() == THREEPOSSWITCH_UP){
		if(pa_mode_1 == 0){ pa_mode_1 = 2; } // Start warming up
	}else{
		if(pa_mode_1 > 1){ pa_mode_1 = 1; } // Start cooling off
	}
	// SBand Secondary PA
	// Pulls 72 watts operating
	if(lem->SBandPASelSwitch.GetState() == THREEPOSSWITCH_DOWN){
		if(pa_mode_2 == 0){ pa_mode_2 = 2; } // Start warming up
	}else{
		if(pa_mode_2 > 1){ pa_mode_2 = 1; } // Start cooling off
	}
}

void LM_SBAND::TimeStep(double simt){
	if(lem == NULL){ return; } // Do nothing if not initialized
	switch(tc_mode_1){
		case 0: // OFF
			break;
		case 1: // SHUTTING DOWN
			if(tc_timer_1 == 0){
				tc_timer_1 = simt; // Initialize
			}
			// Wait one second for the tubes to cool
			if(simt > (tc_timer_1+1)){
				// Tubes are cooled, we can stop the timer.
				tc_timer_1 = 0; tc_mode_1 = 0;
			}
			break;
		case 2: // STARTING UP
			if(tc_timer_1 == 0){
				tc_timer_1 = simt; // Initialize
			}
			// Taking 30 seconds, warm up the tubes
			if(lem->COMM_PRIM_SBAND_XCVR_CB.Voltage() > 24){
				lem->COMM_PRIM_SBAND_XCVR_CB.DrawPower(12); // FIXME: I guessed at this number
			}else{
				// Power failed - Start over
				tc_timer_1 = simt; break;
			}
			if(simt > (tc_timer_1+30)){
				// Tubes are warm and we're ready to operate.
				tc_mode_1 = 3; tc_timer_1 = 0;
			}
			break;
		case 3: // OPERATING
			if(lem->COMM_PRIM_SBAND_XCVR_CB.Voltage() > 24){
				lem->COMM_PRIM_SBAND_XCVR_CB.DrawPower(36);
			}else{
				// Power failed - Start cooling
				if(tc_timer_1 == 0){
					tc_timer_1 = simt;
				}
				if(simt > (tc_timer_1+1)){ // After one second, shut down
					tc_mode_1 = 1; break;
				}
			}
			break;
	}
	switch(tc_mode_2){
		case 0: // OFF
			break;
		case 1: // SHUTTING DOWN
			if(tc_timer_2 == 0){
				tc_timer_2 = simt; // Initialize
			}
			// Wait one second for the tubes to cool
			if(simt > (tc_timer_2+1)){
				// Tubes are cooled, we can stop the timer.
				tc_timer_2 = 0; tc_mode_2 = 0;
			}
			break;
		case 2: // STARTING UP
			if(tc_timer_2 == 0){
				tc_timer_2 = simt; // Initialize
			}
			// Taking 30 seconds, warm up the tubes
			if(lem->COMM_SEC_SBAND_XCVR_CB.Voltage() > 24){
				lem->COMM_SEC_SBAND_XCVR_CB.DrawPower(12); // FIXME: I guessed at this number
			}else{
				// Power failed - Start over
				tc_timer_2 = simt; break;
			}
			if(simt > (tc_timer_2+30)){
				// Tubes are warm and we're ready to operate.
				tc_mode_2 = 3; tc_timer_2 = 0;
			}
			break;
		case 3: // OPERATING
			if(lem->COMM_SEC_SBAND_XCVR_CB.Voltage() > 24){
				lem->COMM_SEC_SBAND_XCVR_CB.DrawPower(36);
			}else{
				// Power failed - Start cooling
				if(tc_timer_2 == 0){
					tc_timer_2 = simt;
				}
				if(simt > (tc_timer_2+1)){ // After one second, shut down
					tc_mode_2 = 1; break;
				}
			}
			break;
	}
	// PAs
	switch(pa_mode_1){
		case 0: // OFF
			break;
		case 1: // SHUTTING DOWN
			if(pa_timer_1 == 0){
				pa_timer_1 = simt; // Initialize
			}
			// Wait one second for the tubes to cool
			if(simt > (pa_timer_1+1)){
				// Tubes are cooled, we can stop the timer.
				pa_timer_1 = 0; pa_mode_1 = 0;
			}
			break;
		case 2: // STARTING UP
			if(pa_timer_1 == 0){
				pa_timer_1 = simt; // Initialize
			}
			// Taking 60 seconds, warm up the tubes
			if(lem->COMM_PRIM_SBAND_PA_CB.Voltage() > 24){
				lem->COMM_PRIM_SBAND_PA_CB.DrawPower(12); // FIXME: I guessed at this number
			}else{
				// Power failed - Start over
				pa_timer_1 = simt; break;
			}
			if(simt > (pa_timer_1+60)){
				// Tubes are warm and we're ready to operate.
				pa_mode_1 = 3; pa_timer_1 = 0;
			}
			break;
		case 3: // OPERATING
			if(lem->COMM_PRIM_SBAND_PA_CB.Voltage() > 24){
				lem->COMM_PRIM_SBAND_PA_CB.DrawPower(72);
			}else{
				// Power failed - Start cooling
				if(pa_timer_1 == 0){
					pa_timer_1 = simt;
				}
				if(simt > (pa_timer_1+1)){ // After one second, shut down
					pa_mode_1 = 1; break;
				}
			}
			break;
	}
	switch(pa_mode_2){
		case 0: // OFF
			break;
		case 1: // SHUTTING DOWN
			if(pa_timer_2 == 0){
				pa_timer_2 = simt; // Initialize
			}
			// Wait one second for the tubes to cool
			if(simt > (pa_timer_2+1)){
				// Tubes are cooled, we can stop the timer.
				pa_timer_2 = 0; pa_mode_2 = 0;
			}
			break;
		case 2: // STARTING UP
			if(pa_timer_2 == 0){
				pa_timer_2 = simt; // Initialize
			}
			// Taking 60 seconds, warm up the tubes
			if(lem->COMM_SEC_SBAND_PA_CB.Voltage() > 24){
				lem->COMM_SEC_SBAND_PA_CB.DrawPower(12); // FIXME: I guessed at this number
			}else{
				// Power failed - Start over
				pa_timer_2 = simt; break;
			}
			if(simt > (pa_timer_2+60)){
				// Tubes are warm and we're ready to operate.
				pa_mode_2 = 3; pa_timer_2 = 0;
			}
			break;
		case 3: // OPERATING
			if(lem->COMM_SEC_SBAND_PA_CB.Voltage() > 24){
				lem->COMM_SEC_SBAND_PA_CB.DrawPower(72);
			}else{
				// Power failed - Start cooling
				if(pa_timer_2 == 0){
					pa_timer_2 = simt;
				}
				if(simt > (pa_timer_2+1)){ // After one second, shut down
					pa_mode_2 = 1; break;
				}
			}
			break;
	}
}

// S-Band Steerable Antenna
LEM_SteerableAnt::LEM_SteerableAnt()// : antenna("LEM-SBand-Steerable-Antenna",_vector3(0.013, 3.0, 0.03),0.03,0.04),
	//antheater("LEM-SBand-Steerable-Antenna-Heater",1,NULL,40,51.7,0,233.15,255,&antenna)
{
	lem = NULL;	
}

void LEM_SteerableAnt::Init(LEM *s, h_Radiator *an, Boiler *anheat){
	lem = s;
	// Set up antenna.
	// SBand antenna 51.7 watts to stay between -40F and 0F
	antenna = an;
	antheater = anheat;
	antenna->isolation = 1.0; 
	antenna->Area = 10783.0112; // Surface area of reflecting dish, probably good enough
	//antenna.mass = 10000;      // Probably the same as the RR antenna
	//antenna.SetTemp(233); 
	if(lem != NULL){
		antheater->WireTo(&lem->HTR_SBD_ANT_CB);
		//lem->Panelsdk.AddHydraulic(&antenna);
		//lem->Panelsdk.AddElectrical(&antheater,false);
		antheater->Enable();
		antheater->SetPumpAuto();
	}
}

void LEM_SteerableAnt::TimeStep(double simdt){
	if(lem == NULL){ return; }
	// Draw DC power from COMM_SBAND_ANT_CB to position.
	// Use 7.6 watts to move the antenna and 0.7 watts to maintain auto track.
	// Draw AC power from ???
	// Use 27.9 watts to move the antenna and 4.0 watts to maintain auto track.

	// sprintf(oapiDebugString(),"SBand Antenna Temp: %f AH %f",antenna.Temp,antheater.pumping);
}

void LEM_SteerableAnt::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_SteerableAnt::LoadState(FILEHANDLE scn,char *end_str){

}

double LEM_SteerableAnt::GetAntennaTempF(){

	return(0);
}
