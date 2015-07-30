/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007

  IMFD5 communication support
  
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
#include "IMFD_Client.h"


IMFD_Client::IMFD_Client() {

	IMFD_MemoryMapping = NULL;
	IMFD_MemoryMapped = NULL;
	ZeroMemory(&ConstantConfiguration, sizeof(IMFDConstantConfiguration));
	VariableConfiguration.DataTimeStamp = 0;
	VariableConfiguration.GET = -1;
	Vessel = NULL;

	BurnRequestState = 0;
	LastBurnReadTime = 0;
	BurnDataValid = false;
	BurnFlags = 0;
}


IMFD_Client::~IMFD_Client() {

	Disconnect();
}
	
bool IMFD_Client::Connect() {

	IMFD_MemoryMapping = OpenFileMapping(FILE_MAP_WRITE, FALSE, IMFD_COM_AREA_NAME);
	if(IMFD_MemoryMapping) {
		IMFD_MemoryMapped = (IMFDCOMSET*)MapViewOfFile(IMFD_MemoryMapping, FILE_MAP_WRITE, 0, 0, 0);
		if(!IMFD_MemoryMapped) {
			CloseHandle(IMFD_MemoryMapping);
			IMFD_MemoryMapping = NULL;
		}
	}
	return (IMFD_MemoryMapped != NULL);
}

void IMFD_Client::Disconnect() {

	//
	// IMFD5 communication support
	//

	if (IMFD_MemoryMapping) {
		UnmapViewOfFile(IMFD_MemoryMapped);
		CloseHandle(IMFD_MemoryMapping);
		IMFD_MemoryMapping = NULL;
		IMFD_MemoryMapped = NULL;
	}
}

void IMFD_Client::SetConstantConfiguration(IMFDConstantConfiguration c) {

	memcpy(&ConstantConfiguration, &c, sizeof(IMFDConstantConfiguration));
}

void IMFD_Client::SetVariableConfiguration(IMFDVariableConfiguration c) {

	memcpy(&VariableConfiguration, &c, sizeof(IMFDVariableConfiguration));
}

void IMFD_Client::TimeStep() {

	if(IMFD_MemoryMapped) {
		if(IMFD_MemoryMapped->RequestToVessel == Vessel->GetHandle()) {
			if (IMFD_MemoryMapped->ReqVar) {
				IMFD_MemoryMapped->DataTimeStamp = VariableConfiguration.DataTimeStamp;
				IMFD_MemoryMapped->GET = VariableConfiguration.GET;
			}

			if(IMFD_MemoryMapped->ReqConst)	{
				strcpy(IMFD_MemoryMapped->LandingBase, ConstantConfiguration.LandingBase);
				strcpy(IMFD_MemoryMapped->TargetCelbody, ConstantConfiguration.TargetCelbody);
				IMFD_MemoryMapped->AZIMUTCOR = ConstantConfiguration.AZIMUTCOR;
				IMFD_MemoryMapped->MJDTLI = ConstantConfiguration.MJDTLI;
				IMFD_MemoryMapped->MJDLOI = ConstantConfiguration.MJDLOI;
				IMFD_MemoryMapped->MJDPEC = ConstantConfiguration.MJDPEC;
				IMFD_MemoryMapped->ALTPEC = ConstantConfiguration.ALTPEC;
				IMFD_MemoryMapped->MJDLDG = ConstantConfiguration.MJDLDG;
				IMFD_MemoryMapped->MJDSPL = ConstantConfiguration.MJDSPL;
				IMFD_MemoryMapped->SPL_lon = ConstantConfiguration.SPL_lon;
				IMFD_MemoryMapped->SPL_lat = ConstantConfiguration.SPL_lat;
				IMFD_MemoryMapped->ApolloP30 = ConstantConfiguration.ApolloP30Mode;
				/// \todo longitude and latitude seems to be mixed up 
				IMFD_MemoryMapped->OffsetLon = ConstantConfiguration.TLIOffsetLat; 
				IMFD_MemoryMapped->OffsetLat = ConstantConfiguration.TLIOffsetLon; 
				IMFD_MemoryMapped->OffsetRad = ConstantConfiguration.TLIOffsetRad; 
			}
			// ackowledge
			IMFD_MemoryMapped->ReqToDone = true;	
		}
	}

	// CODE TO REQUEST READ BURN STRUCT DATA FROM IMFD REACH EACH 0.5 SEC
	// ------------------------------------------------------------------
	// this code is amorced with BurnRequestState=1 and appropriate BurnFlags.
	// It will turn to idle automatically with BurnFlags=0; after having
	// sent a last request with BurnFlags=0 to stop IMFD burn computation.
	if(IMFD_MemoryMapped) {
		switch(BurnRequestState) {
		case 1:	// Launch burn data request
			if (IMFD_MemoryMapped->RequestFromVessel == NULL) {
				IMFD_MemoryMapped->RequestFromVessel = Vessel->GetHandle();	// initiate request
				IMFD_MemoryMapped->ReqFromDone = false;	// reset acknowledge
				IMFD_MemoryMapped->BurnFlags = BurnFlags;
				if (BurnFlags) {
					BurnRequestState++;
				} else {
					BurnRequestState = 4;
					BurnDataValid = false;
				}
			}
			break;

		case 2:	// wait acknowledge and read data
			if (IMFD_MemoryMapped->ReqFromDone) {	// answer arrived
				memcpy(&LastBurnData, &IMFD_MemoryMapped->Burn, sizeof(IMFD_BURN_DATA));
				BurnDataValid = true;
				IMFD_MemoryMapped->RequestFromVessel = NULL;
				LastBurnReadTime = GetTickCount();
				BurnRequestState++;
			}
			if (!BurnFlags) {
				BurnRequestState = 4;
				BurnDataValid = false;
			}
			break;

		case 3:	// wait before to do a further request
			if (GetTickCount() > LastBurnReadTime + 500)		// wait 0.5 sec
				BurnRequestState = 1;				// and reamorce
			break;

		case 4:	// return to idle after BurnFlags=0 request
			/// \todo But it does not arrive???
			// if (IMFD_MemoryMapped->ReqFromDone) {	// answer arrived
				IMFD_MemoryMapped->RequestFromVessel = NULL;
				BurnRequestState = 0;				// idle state
			// }
			break;
		}
	}	
}

bool IMFD_Client::StartBurnDataRequests() {

	if (BurnRequestState == 0) {
		BurnFlags = IPC_BURN|IPC_LVLH;
		BurnRequestState = 1;
		return true;
	}
	return false;
}

void IMFD_Client::StopBurnDataRequests() {

	BurnFlags = NULL;
}

