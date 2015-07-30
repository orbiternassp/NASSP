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

#include "IMFD_IPC_com.h"

#if !defined(_IMFD_CLIENT_H)
#define _IMFD_CLIENT_H


typedef struct {
	char	LandingBase[256];	// Name of the landing base used for the mission.	
	char    TargetCelbody[64];	// Target Celbody for the Target Intercept program.
	double	AZIMUTCOR;			// Launch azimut correction (= launch azimuth - 90°), NULL if not available.
	double	MJDTLI;				// MJD of TLI burn, NULL if not available.
	double	MJDLOI;				// MJD of first LOI burn, NULL if not available.
	double	MJDPEC;				// MJD of free return pericynthion, NULL if not available.
	double	ALTPEC;				// Altitude of free return pericynthion, NULL if not available.
	double	MJDLDG;				// MJD of lunar landing, NULL if not available.
	double	MJDSPL;				// MJD of normal return entry interface, NULL if not available.
	double	SPL_lon;			// Normal return splashdown long in decimal deg, if MJDSPL valid.
	double	SPL_lat;			// Normal return splashdown lat in decimal deg, if MJDSPL valid.
	bool	ApolloP30Mode;		// When true will force LambertAP=ApolloP30 and Off-Axis guidance when available
	double  TLIOffsetLon;		// TLI target offset
	double  TLIOffsetLat;		// TLI target offset
	double  TLIOffsetRad;		// TLI target offset

} IMFDConstantConfiguration;

typedef struct {
	double	DataTimeStamp;	// Orbiter simt when the variable data was written into IPC.
	double	GET;			// Ground Elapsed Time in sec, equal to -1 before launch.
} IMFDVariableConfiguration;


class IMFD_Client {

public:
	IMFD_Client();
	virtual ~IMFD_Client();
	
	void SetVessel(VESSEL *v) { Vessel = v; };
	void SetConstantConfiguration(IMFDConstantConfiguration c);
	void SetVariableConfiguration(IMFDVariableConfiguration c);
	bool Connect();
	void Disconnect();
	void TimeStep();
	virtual bool StartBurnDataRequests();
	virtual void StopBurnDataRequests();
	bool IsBurnDataRequesting() { return BurnRequestState != 0; };
	bool IsBurnDataValid() { return BurnDataValid; };
	IMFD_BURN_DATA GetBurnData() { return LastBurnData; };

private:
	HANDLE      IMFD_MemoryMapping;
	IMFDCOMSET *IMFD_MemoryMapped;
	VESSEL *Vessel;
	IMFDConstantConfiguration ConstantConfiguration;
	IMFDVariableConfiguration VariableConfiguration;

	int BurnFlags;
	int BurnRequestState;
	bool BurnDataValid;
	DWORD LastBurnReadTime;	
	IMFD_BURN_DATA LastBurnData;
};

#endif
