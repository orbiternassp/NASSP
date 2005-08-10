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
  **************************************************************************/

#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
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
#if DEBUG
	if (!logFile)
		logFile = fopen("VirtualApollo yaIMU.log", "w");
#endif
}

void IMU::LogState(int channel, char *device, int value) 

{
#if DEBUG
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

//	fprintf(logFile, "%.8s.%03hu Ch %03o %s %s\n", buffer, tstruct.millitm, channel, device, buffer1);
			
	fflush(logFile);
#endif
}

void IMU::LogTimeStep(long simt) 

{
#if DEBUG
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

void IMU::LogVector(char* message, IMU_Vector3 v) 

{
#if DEBUG
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
#if DEBUG
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


/*
void tsch_log1(FILE *file, int channel, char *device, char *value, int value1, int value2, int value3, int value4, int value5, int value6, int value7) {
	struct _timeb tstruct;
	char *timeline;
	char buffer[100];
	
	_ftime(&tstruct);
	timeline = ctime(&(tstruct.time));
	strcpy(buffer, timeline + 11);
	fprintf(file, "%.8s.%03hu Ch %03o %s %s %o %o %o %o %o %o %o\n", buffer, tstruct.millitm, 
		channel, device, value, value1, value2, value3, value4, value5, value6, value7);
	fflush(file);
}

void tsch_log(FILE *file, int channel, char *device, int value, int value1, int value2, int value3, int value4, int value5, int value6, int value7) {
	char buffer[100];

	tsch_log1(file, channel, device, tsch_intToBinaryString(buffer, value), value1, value2, value3, value4, value5, value6, value7);
}

void tsch_log(FILE *file, int channel, char *device, char *charValue, int value) {
	char buffer[500], buffer1[100];

	strcpy(buffer, tsch_intToBinaryString(buffer1, value));
	strcpy(buffer + strlen(buffer), " ");
	strcpy(buffer + strlen(buffer), charValue);

	tsch_log(file, channel, device, buffer); 
}

void tsch_addFlagToLogString(char *dest, int flag, char *toAdd) {
	
	char buffer[100];

	strcpy(buffer, toAdd);
	strcpy(buffer + strlen(buffer), " ");
	if (flag != 0) {
		_strupr(buffer);
	} else {
		_strlwr(buffer);
	}
	strcpy(dest + strlen(dest), buffer);
}

void tsch_addToLogString(char *dest, char *toAdd) {
	
	strcpy(dest + strlen(dest), toAdd);
}

*/

	
