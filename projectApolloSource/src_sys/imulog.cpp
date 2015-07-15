/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  IMU code

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
  *	Revision 1.2  2011/07/11 01:42:36  vrouleau
  *	- Removed AGC_SOCKET_ENABLED flag. Rework is needed to make this an optional feature instead of a conditional define. To many untested think exists in the socket version
  *	
  *	- Checkpoint commit on the LEM RR. If the RR as been slew to track the CSM , the auto mode will continue tracking it.
  *	
  *	Revision 1.1  2009/02/18 23:21:48  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.7  2008/04/11 11:49:37  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.6  2007/06/06 15:02:13  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.5  2007/04/25 18:48:58  tschachim
  *	New PIPA implementation.
  *	
  *	Revision 1.4  2005/10/19 11:28:18  tschachim
  *	Changed log file name.
  *	
  *	Revision 1.3  2005/08/30 14:53:00  spacex15
  *	Added conditionnally defined AGC_SOCKET_ENABLED to use an external socket connected virtual AGC
  *	
  *	Revision 1.2  2005/08/19 13:35:24  tschachim
  *	Logging enabled in debug configuration
  *	
  *	Revision 1.1  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "soundlib.h"

#include "nasspdefs.h"
#include "apolloguidance.h"
#include "dsky.h"

#include "ioChannels.h"
#include "IMU.h"

static FILE *logFile = NULL;
static long lastTime = -1;

char *intToBinaryString(char *buffer, int i);


void IMU::LogInit() 

{
#ifdef _DEBUG
	if (!logFile)
#ifdef SAT5_LMPKD_EXPORTS
		logFile = fopen("ProjectApollo LM IMU.log", "w");
#else
		logFile = fopen("ProjectApollo IMU.log", "w");
#endif
#endif
}

void IMU::LogState(int channel, char *device, int value) 

{
#ifdef _DEBUG
	struct _timeb tstruct;
	char *timeline;
	char buffer[100];	
	char buffer1[100];
	
	_ftime(&tstruct);
	timeline = ctime(&(tstruct.time));
	strcpy(buffer, timeline + 11);
	
	intToBinaryString(buffer1, value);

		fprintf(logFile, "%.8s.%03hu Ch %03o %s %s PIPA %o %o %o CDUCMD %o %o %o GYRO %o IMU %.2f %.2f %.2f\n", buffer, tstruct.millitm, channel, device, buffer1,
		    agc.GetErasable(0, RegPIPAX), 
		    agc.GetErasable(0, RegPIPAY), 
			agc.GetErasable(0, RegPIPAZ), 
			agc.GetErasable(0, RegCDUXCMD),
			agc.GetErasable(0, RegCDUYCMD),
			agc.GetErasable(0, RegCDUZCMD),
			//state->Erasable[0][RegCDUX],
			//state->Erasable[0][RegCDUY],
			//state->Erasable[0][RegCDUZ],			
			agc.GetErasable(0, RegGYROCTR),
			radToDeg(Gimbal.X),
			radToDeg(Gimbal.Y),
			radToDeg(Gimbal.Z));
			
	fflush(logFile);
#endif
}

void IMU::LogTimeStep(long simt) 

{
#ifdef _DEBUG
	struct _timeb tstruct;
	char *timeline;
	char buffer[100];	
	
	_ftime(&tstruct);
	timeline = ctime(&(tstruct.time));
	strcpy(buffer, timeline + 11);
		
	fprintf(logFile, "%.8s.%03hu TimeStep                   Orbiter %.2f %.2f %.2f   IMU %.2f %.2f %.2f\n", buffer, tstruct.millitm, 
			radToDeg(Orbiter.Attitude.X),
			radToDeg(Orbiter.Attitude.Y),
			radToDeg(Orbiter.Attitude.Z),
			radToDeg(Gimbal.X),
			radToDeg(Gimbal.Y),
			radToDeg(Gimbal.Z));

	fflush(logFile);
#endif
}


void IMU::LogVector(char* message, VECTOR3 v) 

{
#ifdef _DEBUG
	struct _timeb tstruct;
	char *timeline;
	char buffer[100];
	
	_ftime(&tstruct);
	timeline = ctime(&(tstruct.time));
	strcpy(buffer, timeline + 11);
		
	fprintf(logFile, "%.8s.%03hu %s Vector %f %f %f\n", buffer, tstruct.millitm, 
			message, v.x, v.y, v.z);
#endif
}

void IMU::LogMessage(char* s) 

{
#ifdef _DEBUG
	struct _timeb tstruct;
	char *timeline;
	char buffer[100];	
	
	_ftime(&tstruct);
	timeline = ctime(&(tstruct.time));
	strcpy(buffer, timeline + 11);
		
	fprintf(logFile, "%.8s.%03hu Message %s\n", buffer, tstruct.millitm, 
			s);	
#endif
}

char *intToBinaryString(char *buffer, int i) {

	unsigned int base = 0x4000; /* Assuming 15 bits */
	int pos = 0;

	while (base) {
		sprintf(&(buffer[pos]), "%c", (base & i) ? '1' : '0');
		base = base >> 1;
		pos++;
	}
	return(buffer);
}
