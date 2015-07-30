/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER sound library.
  This code caches sounds for the OrbiterSound library so that
  you don't have to keep track of IDs.

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
  *	Revision 1.1  2009/02/18 23:21:48  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.4  2008/04/11 11:50:04  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.3  2007/06/06 15:02:22  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.2  2006/10/23 13:32:31  tschachim
  *	Removed warnings
  *	
  *	Revision 1.1  2006/06/24 15:05:40  movieman523
  *	New sound event files.
  *	
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <string.h>

#include "resource.h"

#include "soundlib.h"
#include "soundevents.h"

#include "tracer.h"
#include "nasspdefs.h"

// MODIF PG

//
// NONE OF THESE SHOULD BE STATIC VARIABLES! THESE SHOULD BE CLASS VARIABLES!
//
// And no fixed-size arrays either.
//
static int lastplayed = -1;
static int reallyplayed;
static int tobeplayed = -1;
static int nSoundsLoaded = 0;
#define MAX_SOUND_EVENT 256
static SoundEvent soundevents[MAX_SOUND_EVENT];
static int SoundEventLoaded = false;

// MODIF x15 to manage landing mission sound
SoundEvent::SoundEvent()

{
	altitude = -1.;
	mode     = -1 ;
	played   = false;
	filenames[0] = 0;
	lastplayed=-1;
    SoundEventLoaded =false;

	apDSBuffer = 0;
}

SoundEvent::~SoundEvent()

{
	//
	// Nothing to do.
	//
    Stop();
}

int SoundEvent::isValid()
{
	return(SoundEventLoaded);
}


int SoundEvent::makeInvalid()
{
	SoundEventLoaded = false;
	return(true);
}



int SoundEvent::play(SoundLib soundlib,
					 VESSEL   *Vessel,
					  char    *names ,
					  double  *offset,
					  int     *newbuffer,
					  double  simcomputert,
					  double  MissionTime,
					  int     mode,
					  double  timeremaining,
					  double  timeafterpdi,
					  double  timetoapproach,
					  int     flags,
					  int     volume)


{
	int kk;
	char buffers[255];
	double altitude;
	double vlon,vlat,vrad;


    double timetopdi;
	double timesincepdi;
	double deltaoffset =0.;

    if (!isValid())
		return(false);

	// is Sound still playing ?
	// if yes let it play

//   TRACESETUP("SOUNDEVENT PLAY");

	if(lastplayed >= 0)
	{
        if(soundevents[lastplayed].offset == -1)
		{
	        if (IsPlaying())
			{ 
            //		TRACE("EN TRAIN DE JOUER");
                return(false);
			}
		}
		else if (IsPlaying())
		{
			if (soundevents[lastplayed+1].offset > soundevents[lastplayed].offset)
			{
			    if(Finish((double) soundevents[lastplayed+1].offset - 0.1))
				{
				}
				else
				    return(false);
			}
		}
	}


//  no more sounds to be played just return

	if(soundevents[lastplayed+1].met == 0)
		return(false);


//	sprintf(buffers,"ENTREE PLAY MODE %d timeremaining %f timeafterpdi %f altitude %f",
//		mode,timeremaining,timeafterpdi,altitude);
//	TRACE(buffers);

	timetopdi = timeremaining  -(MissionTime - simcomputert);
	timesincepdi = timeafterpdi +(MissionTime - simcomputert);
	timetoapproach = timetoapproach -(MissionTime -simcomputert);
    if (mode == 0)
	{ 
/*		sprintf(buffers,"AVANT PDI IGNIT %f %f %f ",
					   timetopdi, MissionTime, simcomputert);
		sprintf(oapiDebugString(),"%s",buffers);
*/
	}	

	if (mode >= 1 && mode < 3)
	{ 
		OBJHANDLE hbody=Vessel->GetGravityRef();
		double bradius=oapiGetSize(hbody);
		Vessel->GetEquPos(vlon, vlat, vrad);
		double cgelev=Vessel->GetCOG_elev();
		altitude=vrad-bradius-cgelev;

							
/*		sprintf(buffers,"AFTER PDI : TIMEAFTER %f ALTITUDE %f",
			        timesincepdi,altitude);
		sprintf(oapiDebugString(),"%s",buffers);
*/
	}	

    kk = lastplayed+1;
	sprintf(buffers,"%f",soundevents[kk].altitude);
//	TRACE (buffers);
//	sprintf(buffers,"%f",altitude);
//	TRACE (buffers);

    // if mode = 1 BRAKING it's altitude 
	// if mode = 0 MONITOR it's time to ignition

	if (mode == 0)
    {
//		TRACE("TEST PDI IGNIT");
		sprintf(buffers,"TEST PDI IGNIT NEXTEVENT %d %f %s VERSUS %f",kk,
			        soundevents[kk].timetoignition,soundevents[kk].filenames,
					   timetopdi);
//		sprintf(oapiDebugString(),"%s",buffers);
//		TRACE(buffers);
        /* skip too old sounds */
		while (    (soundevents[kk].timetoignition > timetopdi)
			    && (soundevents[kk+1].timetoignition > timetopdi)
			  )
	        kk++;
        if (soundevents[kk].timetoignition < timetopdi)
			return 0;
		deltaoffset = soundevents[kk].timetoignition - timetopdi;
	}
	else if (mode == 1)
	{
//sprintf(buffers,"TIME AFTER PDI %f", timesincepdi);
//TRACE(buffers);
        
		if ((timeafterpdi < 160))
		{
//        TRACE("TIME AFTER");
		sprintf(buffers,"TIME AFTER %f %f",soundevents[kk].timeafterignition,
			                                                     timesincepdi);
//		TRACE(buffers);

		sprintf(buffers,"TEST AFTER PDI NEXTEVENT %d %f %s VERSUS %f",kk,
			        soundevents[kk].timeafterignition,soundevents[kk].filenames,
					   timesincepdi);
//		sprintf(oapiDebugString(),"%s",buffers);

		while (    (soundevents[kk].timeafterignition < timesincepdi)
			    && (soundevents[kk+1].timeafterignition < timesincepdi)
				&& (!soundevents[kk].mandatory)
			  )
	        kk++;

        if (soundevents[kk].timeafterignition>timesincepdi)
		    return 0;

		}
        else
		{
//        TRACE("TIME TO APPROACH");
		sprintf(buffers,"TIME TO APP %f %f",soundevents[kk].timetoapproach,
			                                                     timetoapproach);
//		TRACE(buffers);

		sprintf(buffers,"TEST TO APPROACH NEXTEVENT %d %f %s VERSUS %f",kk,
			        soundevents[kk].timetoapproach,soundevents[kk].filenames,
					   timetoapproach);
//		sprintf(oapiDebugString(),"%s",buffers);

		while (    (soundevents[kk].timetoapproach > timetoapproach)
			    && (soundevents[kk+1].timetoapproach > timetoapproach)
				&& (!soundevents[kk].mandatory)
			  )
	        kk++;

        if (soundevents[kk].timetoapproach<timetoapproach)
		    return 0;

		}
	}
	else if (mode == 2)
	{
//        TRACE("TEST ALTITUDE");
		sprintf(buffers,"ALT %d %f %f",mode,soundevents[kk].altitude,altitude);
//		TRACE(buffers);

//		sprintf(buffers,"TEST ALTI NEXTEVENT %d %f %s VERSUS %f MODE %d",kk,
//			        soundevents[kk].altitude,soundevents[kk].filenames,
//					   altitude,mode);
//		sprintf(oapiDebugString(),"%s",buffers);

        while (soundevents[kk].mode != mode)
		{
			kk++;
		}


		while (    (soundevents[kk].altitude > altitude)
			    && (soundevents[kk+1].altitude > altitude)
				&& (!soundevents[kk].mandatory)
			  )
	        kk++;

        if (soundevents[kk].altitude<altitude)
		    return 0;
	}
	else if (mode == 3)
	{
        if (soundevents[kk].met > MissionTime)
			return 0;
		deltaoffset = 0.0;
	}
	else return 0;
 

//	TRACE("ON VA JOUER ");
//	TRACE(soundevents[kk].filenames);
	

	strcpy(names,soundevents[kk].filenames);

	*offset = (double) soundevents[kk].offset + deltaoffset;

	*newbuffer = true;

	if(lastplayed >= 0)
	{
	    if (!strcmp(soundevents[kk].filenames,soundevents[lastplayed].filenames))
	        *newbuffer = false;
    }

	lastplayed = kk;

//	sprintf(buffers,"SON %d NEW %d OFFSET %f", kk, *newbuffer,*offset);
//	TRACE(buffers);

	return 1;
}


int SoundEvent::LoadMissionLandingSoundArray(SoundLib soundlib,char *soundname)

{
	char	SoundPath[256];
	FILE *fp;
	char lines[255];
	int  indice = 0;
	char *buff;
	char *buff2;
	char  buffers[255];
	int   nchar;
	char hours[4];
	char mins[3];
	char secs[3];
	char basefilenames[255];
	char rootfilenames[255]; 
	int  numeroi;
	double delta,alt1,alt2;
    int  ii,jj,kk;
	bool done;
	int time1,time2;

	int type2 ;
	int oldnumeroi;
	double oldmet;
	
	if(SoundEventLoaded)
		return true;

 
	lastplayed =-1;
	TRACESETUP("LOAD MISSION SOUND ARRAY");

//	if (!OrbiterSoundActive)
//		return false;



	_snprintf(SoundPath, 255, "%s\\%s\\%s", soundlib.basepath,
		                                    soundlib.missionpath, soundname);
TRACE(SoundPath);

	fp = fopen(SoundPath, "r");
	if (!fp)
		return false;


	fgets(basefilenames,255,fp);

    type2 = (strncmp(basefilenames,"type2",5) == 0);

	if (type2)
    {
		oldnumeroi = 0;
	    fgets(basefilenames,255,fp);
	}


    buff2 = strchr(basefilenames,';');
	nchar = buff2 - basefilenames;
	memset(buffers,0,sizeof(buffers));
	strncpy(buffers,basefilenames,nchar);



 	_snprintf(rootfilenames, 255, "%s\\%s\\%s", soundlib.basepath,soundlib.missionpath, buffers);

	fgets(lines,255,fp);



	while (fgets(lines, 255, fp) != NULL)
    {
        buff = lines;


	    buff2 = strchr(buff,';');
	    nchar = buff2 - buff;
	    memset(buffers,0,sizeof(buffers));
	    strncpy(buffers,buff,nchar);



		/* first decode MET */
		memset(hours,0,sizeof(hours));
		strncpy(hours,buffers,3);
		soundevents[indice].met = 0;
		soundevents[indice].met += atoi(hours)*3600;

		memset(mins,0,sizeof(mins));
		strncpy(mins,buffers+4,2);
		soundevents[indice].met += atoi(mins)*60;

		memset(secs,0,sizeof(secs));
		strncpy(secs,buffers+7,2);
		soundevents[indice].met += atoi(secs);


		/* generate filenames */
		buff = buff2 + 1;
		buff2 = strchr(buff,';');
	    nchar = buff2 - buff;
	    memset(buffers,0,sizeof(buffers));
	    strncpy(buffers,buff,nchar);

		numeroi = atoi(buffers);

		if (type2)
		{
		    if (numeroi != oldnumeroi)
			{
				oldnumeroi = numeroi;
				oldmet = soundevents[indice].met;
                offset = 0;
			}
			else 
			{
				offset = (int) (soundevents[indice].met 
					      - oldmet);
			}
		}
		else offset = -1;

		soundevents[indice].offset = offset;

		strcpy(soundevents[indice].filenames,rootfilenames);
		sprintf(buffers,"%02d",numeroi);


		strcat(soundevents[indice].filenames,buffers);
		strcat(soundevents[indice].filenames,".wav");

		/* decode altitude */


		buff = buff2 + 1;
		buff2 = strchr(buff,';');
	    nchar = buff2 - buff;
	    memset(buffers,0,sizeof(buffers));

	    strncpy(buffers,buff,nchar);
		soundevents[indice].altitude = (atof(buffers)*0.308);
		if (soundevents[indice].altitude > 0.0)
		{
			// soundevents[indice].altitude += 5.1;
		}


		buff = buff2 +1;

		buff2 = strchr(buff,';');
	    nchar = buff2 - buff;
	    memset(buffers,0,sizeof(buffers));

	    strncpy(buffers,buff,nchar);
		if(buffers[0] == 'o')
		{
		    soundevents[indice].mode = 0;
		}
		else
		if(buffers[0] == 'b')
		{
		    soundevents[indice].mode = 1;
		}
		else 
		if(buffers[0] == 'a')
		{
		    soundevents[indice].mode = 2;
		}
        else
		{
		    soundevents[indice].mode = -1;
		}



		buff = buff2 +1;
	    buff2 = strchr(buff,';');
	    nchar = buff2 - buff;
	    memset(buffers,0,sizeof(buffers));
	    strncpy(buffers,buff,nchar);

		/* decode Ignition time*/
		memset(hours,0,sizeof(hours));
		strncpy(hours,buffers,3);
		soundevents[indice].timetoignition = 0;
		soundevents[indice].timetoignition += atoi(hours)*3600;

		memset(mins,0,sizeof(mins));
		strncpy(mins,buffers+4,2);
		soundevents[indice].timetoignition += atoi(mins)*60;

		memset(secs,0,sizeof(secs));
		strncpy(secs,buffers+7,2);
		soundevents[indice].timetoignition += atoi(secs);

		soundevents[indice].timetoignition -= soundevents[indice].met;
        if (soundevents[indice].timetoignition < 0.)
			soundevents[indice].timeafterignition = 
			   -soundevents[indice].timetoignition;
		else
			soundevents[indice].timeafterignition = 0.;


		buff = buff2 +1;

		buff2 = strchr(buff,';');
	    nchar = buff2 - buff;
	    memset(buffers,0,sizeof(buffers));

	    strncpy(buffers,buff,nchar);
		if(buffers[0] == 'm')
		{
		    soundevents[indice].mandatory = true;
		}
		else
		{
		    soundevents[indice].mandatory = false;
		}


		buff = buff2 +1;
	    buff2 = strchr(buff,';');
	    nchar = buff2 - buff;
	    memset(buffers,0,sizeof(buffers));
	    strncpy(buffers,buff,nchar);

		/* decode P64 time*/
		memset(hours,0,sizeof(hours));
		strncpy(hours,buffers,3);
		soundevents[indice].timetoapproach = 0;
		soundevents[indice].timetoapproach += atoi(hours)*3600;

		memset(mins,0,sizeof(mins));
		strncpy(mins,buffers+4,2);
		soundevents[indice].timetoapproach += atoi(mins)*60;

		memset(secs,0,sizeof(secs));
		strncpy(secs,buffers+7,2);
		soundevents[indice].timetoapproach += atoi(secs);

		soundevents[indice].timetoapproach -= soundevents[indice].met;

		sprintf(buffers,"LOADED %d %f %f %s ",indice,
                    soundevents[indice].timetoignition,
			        soundevents[indice].timeafterignition,
					soundevents[indice].filenames);
		TRACE(buffers);
		

		indice++;
	}

	fclose(fp);
    soundevents[indice].met = 0;

    /* now interpolate altitude information */

    kk = 0;
	done =false;
	while (!done)
	{
		alt1 = soundevents[kk].altitude;
		time1 = (int) soundevents[kk].met;
        jj = kk + 1;
	    while ((soundevents[jj].altitude == 0.0)&& (jj <indice))
	    {
	        jj++;
	    }

		alt2 = soundevents[jj].altitude;
		time2 = (int) soundevents[jj].met;

		ii = kk +1;
		while (ii < jj)
		{
            delta = ((double)soundevents[ii].met- (double)time1)
				      /((double)time2-(double)time1);
			soundevents[ii].altitude = alt1 + (delta*(alt2-alt1));
			ii++;
		}
		kk = jj;
		done = (kk >= indice);
	}

    SoundEventLoaded = true;
	return true;
	
}

//
// Generic time-based mission sound code.
//
// This uses only the MET.
//

int SoundEvent::LoadMissionTimeSoundArray(SoundLib soundlib, char *soundname, double MissionTime)

{
	char	SoundPath[256];
	FILE *fp;
	char lines[255];
	int  indice = 0;
	char *buff;
	char *buff2;
	char  buffers[255];
	int   nchar;
	char hours[4];
	char mins[3];
	char secs[3];
	char rootfilenames[255]; 

	if(SoundEventLoaded)
		return true;

	lastplayed =-1;
	TRACESETUP("LOAD MISSION SOUND ARRAY");

//	if (!OrbiterSoundActive)
//		return false;


	_snprintf(SoundPath, 255, "%s/%s/%s", soundlib.basepath,
		                                    soundlib.missionpath, soundname);
	TRACE(SoundPath);

	fp = fopen(SoundPath, "r");
	if (!fp)
		return false;

 	_snprintf(rootfilenames, 255, "%s/%s/", soundlib.basepath, soundlib.missionpath);

	while (fgets(lines, 255, fp) != NULL && lines[0])
    {
		if (lines[0] != '#' && indice < MAX_SOUND_EVENT)
		{
			buff = lines;
			bool negative = false;

			buff2 = strchr(buff,';');

			if (buff2)
			{
				nchar = buff2 - buff;
				memset(buffers,0,sizeof(buffers));

				//
				// Check for negative.
				//

				if (lines[0] == '-')
				{
					negative = true;
					buff++;
				}

				strncpy(buffers,buff,nchar);

				/* first decode MET */
				memset(hours,0,sizeof(hours));
				strncpy(hours,buffers,3);
				soundevents[indice].met = atoi(hours)*3600.0;

				memset(mins,0,sizeof(mins));
				strncpy(mins,buffers+4,2);
				soundevents[indice].met += atoi(mins)*60.0;

				memset(secs,0,sizeof(secs));
				strncpy(secs,buffers+7,2);
				soundevents[indice].met += (double) atoi(secs);

				if (negative)
					soundevents[indice].met = -soundevents[indice].met;

				/* generate filenames */
				buff = buff2 + 1;

				//
				// Find the end of the file-name. We have to do this both to
				// allow for comments, and because fgets() leaves a dangling
				// \n at the end of the line.
				//

				buff2 = buff;
				while (*buff2 && *buff2 != '\n' && *buff2 != '\r' && *buff2 != '#' && *buff2 != ' ' && *buff2 != '\t')
					buff2++;

				*buff2 = 0;

				strcpy(soundevents[indice].filenames, rootfilenames);

				strcat(soundevents[indice].filenames, buff);
				strcat(soundevents[indice].filenames, ".wav");

				soundevents[indice].altitude = 0.0;
				soundevents[indice].mode = 3;
				soundevents[indice].timetoignition = 0.0;
				soundevents[indice].mandatory = true;
				soundevents[indice].timetoapproach = 0.0;

				sprintf(buffers,"LOADED %d %f %f %s ",indice,
							soundevents[indice].timetoignition,
							soundevents[indice].timeafterignition,
							soundevents[indice].filenames);
				TRACE(buffers);
		
				//
				// Just reuse this slot if the MET is less than the mission time.
				//
				if (soundevents[indice].met >= MissionTime)
					indice++;
			}
		}
	}

	fclose(fp);
    soundevents[indice].met = MINUS_INFINITY;

	nSoundsLoaded = indice;

    SoundEventLoaded = true;
	return true;
}

int SoundEvent::InitDirectSound(SoundLib soundlib)
{

//    LPDIRECTSOUND8  m_pDS;
    HRESULT hr;
//    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

	TRACESETUP("INITDIRECTSOUND");

	   // Create IDirectSound using the primary sound device
    hr = DirectSoundCreate8( NULL, &m_pDS, NULL );
    if (hr != DS_OK)
	{
	    TRACE ("ERROR DIRECTSOUNDCREATE")
		this->makeInvalid();
		return (false);
	}
		else TRACE("DIRECTSOUND CREATE OK");

    // Set DirectSound coop level
    hr = m_pDS->SetCooperativeLevel( GetActiveWindow() , DSSCL_PRIORITY ) ;
    if (hr != DS_OK)
	    TRACE ("ERROR DIRECTSOUND SET COOP")
	else TRACE("DIRECTSOUND SETCOOP OK");

    // Get the primary buffer
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;

    hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL );
    if (hr != DS_OK)
	    TRACE ("ERROR DIRECTSOUND CREATE SOUND BUFFER")
	else TRACE("DIRECTSOUND CREATE SOUND BUFFER OK");

    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) );
    wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM;
    wfx.nChannels       = (WORD) 2;
    wfx.nSamplesPerSec  = (DWORD) 22050;
    wfx.wBitsPerSample  = (WORD) 16;
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);

    hr = pDSBPrimary->SetFormat(&wfx);
    if (hr != DS_OK)
	    TRACE ("ERROR DIRECTSOUND SET FORMAT")
	else TRACE("DIRECTSOUND SET FORMAT OK");

    return(true);
}

int SoundEvent::PlaySound(char *filenames,int newbuffer, double offset)
{
    HMMIO m_hmmio;
    HRESULT hr;

    TRACESETUP("PLAYSOUND");

	if(newbuffer)
	{

	m_hmmio = mmioOpen( filenames, NULL, MMIO_ALLOCBUF | MMIO_READ );

	if( NULL == m_hmmio )
	{
		TRACE ("DIRECT SOUND ERROR MMIOOPEN");
		return (false);
	}

    MMCKINFO        ckIn;           // chunk info. for general use.
    PCMWAVEFORMAT   pcmWaveFormat;  // Temp PCM structure to load in.

	WAVEFORMATEX* m_pwfx;        // Pointer to WAVEFORMATEX structure
    MMCKINFO      m_ck;          // Multimedia RIFF chunk
    MMCKINFO      m_ckRiff;      // Use in opening a WAVE file
    DWORD         m_dwSize;      // The size of the wave file
    MMIOINFO      m_mmioinfoOut;
    DWORD         m_dwFlags;

#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2
	
	
	   m_pwfx = NULL;

    if( ( 0 != mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) ) )
	{
        TRACE ("DIRECT SOUND ERROR MMIODESCEND");
		return (false);
	}

    // Check to make sure this is a valid wave file
    if( (m_ckRiff.ckid != FOURCC_RIFF) ||
        (m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
	{
		return(false);
        TRACE ("DIRECT SOUND ERROR MMIOFOURCC");
	}

    // Search the input file for for the 'fmt ' chunk.
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if( 0 != mmioDescend( m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK ) )
	{
        TRACE("DIRECT SOUND ERROR MMIDESCENT FIND CHUNK");
		return(false);
	}

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them
    if( ckIn.cksize < (LONG) sizeof(PCMWAVEFORMAT) )
	{
        TRACE ("DIRECT SOUND ERROR CHUNKSIZE");
		return(false);
	}

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if( mmioRead( m_hmmio, (HPSTR) &pcmWaveFormat,
                  sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat) )
	{
        TRACE ("DIRECT SOUND ERROR MMIOREAD");
		return(false);
	}

    // Allocate the waveformatex, but if its not pcm format, read the next
    // word, and thats how many extra bytes to allocate.
    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {
        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
        if( NULL == m_pwfx )
		{
            TRACE("DIRECT SOUND ERROR ALLOC");
			return(false);
		}

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = 0;
    }
    else
    {
        // Read in length of extra bytes.
        WORD cbExtraBytes = 0L;
        if( mmioRead( m_hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD) )
		{
            TRACE ("DIRECT SOUND ERROR M_HMMIO");
			return(false);
		}

        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) + cbExtraBytes ];
        if( NULL == m_pwfx )
		{
            TRACE("DIRECT SOUND ERREOR ALLOC 2");
			return(false);
		}

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = cbExtraBytes;

        // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
        if( mmioRead( m_hmmio, (CHAR*)(((BYTE*)&(m_pwfx->cbSize))+sizeof(WORD)),
                      cbExtraBytes ) != cbExtraBytes )
        {
            //SAFE_DELETE( m_pwfx );
            TRACE("DIRECT SOUND ERROR MMIOREAD2");
			return(false);
        }
    }

    // Ascend the input file out of the 'fmt ' chunk.
    if( 0 != mmioAscend( m_hmmio, &ckIn, 0 ) )
    {
        //SAFE_DELETE( m_pwfx );
        TRACE("DIRECT SOUND ERROR MMIOASCEND");
		return(false);
    }

	TRACE("DIRECTSOUND READMMIO OK");



    m_dwFlags = WAVEFILE_READ;

        if( m_dwFlags == WAVEFILE_READ )
        {
            // Seek to the data
            if( -1 == mmioSeek( m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC),
                            SEEK_SET ) )
			{
                TRACE ("DIRECT SOUND ERROR MMIOSEEK");
				return(false);
			}

            // Search the input file for the 'data' chunk.
            m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
            if( 0 != mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
			{
                 TRACE ("DIRECT SOUND ERROR MMIODESCEND");
				 return(false);
			}
        }
        else
        {
            // Create the 'data' chunk that holds the waveform samples.
            m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
            m_ck.cksize = 0;

            if( 0 != mmioCreateChunk( m_hmmio, &m_ck, 0 ) )
			{
                TRACE("DIRECT SOUND ERROR MMIOCREATECHUNK");
				return(false);
			}

            if( 0 != mmioGetInfo( m_hmmio, &m_mmioinfoOut, 0 ) )
			{
                TRACE ("DIRECT SOUND ERROR MMMIOGETINFO");
				return(false);
			}
		}
  

	TRACE("DIRECTSOUND RESETFILE OK");

 
    // After the reset, the size of the wav file is m_ck.cksize so store it now
    m_dwSize = m_ck.cksize;


    char buffers[80];

	TRACE ("DIRECTSOUND TAILLE BUFFER");
	sprintf(buffers,"%d", m_dwSize);
	TRACE (buffers);


    DWORD                dwDSBufferSize = NULL;



    apDSBuffer = new LPDIRECTSOUNDBUFFER[1];
    if( apDSBuffer == NULL )
    {
        TRACE("ERROR DIRECTSOUND NEW BUFFER");
		return(false);
    }

    // Make the DirectSound buffer the same size as the wav file
    dwDSBufferSize = m_dwSize;

    // Create the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can
    // be hardware accelerated
    DSBUFFERDESC dsbd2;
    ZeroMemory( &dsbd2, sizeof(DSBUFFERDESC) );
    dsbd2.dwSize          = sizeof(DSBUFFERDESC);
    dsbd2.dwFlags         = 0;
    dsbd2.dwBufferBytes   = dwDSBufferSize;
    dsbd2.guid3DAlgorithm = GUID_NULL;
    dsbd2.lpwfxFormat     = m_pwfx;

TRACE ("APPEL CREATE SOUND BUFFER");

    // DirectSound is only guarenteed to play PCM data.  Other
    // formats may or may not work depending the sound card driver.
    hr = m_pDS->CreateSoundBuffer( &dsbd2, &apDSBuffer[0], NULL );
    if (hr != DS_OK)
	    TRACE ("ERROR DIRECTSOUND CREATE SOUND BUFFER")
	else TRACE("DIRECTSOUND CREATE SOUND BUFFER OK");


	

    // Make sure we have focus, and we didn't just switch in from
    // an app which had a DirectSound device

//    hr = RestoreBuffer( &apDSBuffer[0], NULL );
//    if (hr != DS_OK)
//	    TRACE ("ERROR DIRECTSOUND RESTORE SOUND BUFFER")
//	else TRACE("DIRECTSOUND RESTORE SOUND BUFFER OK");



    // Lock the buffer down
    hr = apDSBuffer[0]->Lock( 0, m_dwSize,
                     &pDSLockedBuffer, &dwDSLockedBufferSize,
                     NULL, NULL, 0L );
    if (hr != DS_OK)
		 TRACE ("ERROR DIRECTSOUND LOCK")
	else TRACE ("DIRECTSOUND LOCK OK");

    // Reset the wave file to the beginning


    // Seek to the data
    if( -1 == mmioSeek( m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC),
                            SEEK_SET ) )
         TRACE ("ERROR DIRECTSOUND MMIOSEEK")
	else TRACE ("DIRECTSOUND MMIOSSEEK OK");

    // Search the input file for the 'data' chunk.
    m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if( 0 != mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
         TRACE ("ERROR DIRECTSOUND MMIODESCEND")
	else TRACE ("DIRECTSOUND MMIODESCEND OK");



    MMIOINFO mmioinfoIn; // current status of m_hmmio
    DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file


    dwWavDataRead = 0;

    if( 0 != mmioGetInfo( m_hmmio, &mmioinfoIn, 0 ) )
         TRACE ("ERROR DIRECTSOUND MMIOGETINFO")
	else TRACE ("DIRECTSOUND MMIOGETINFO OK");
        

    UINT cbDataIn = m_dwSize;
    if( cbDataIn > m_ck.cksize )
        cbDataIn = m_ck.cksize;

    m_ck.cksize -= cbDataIn;

    for( DWORD cT = 0; cT < cbDataIn; cT++ )
    {
            // Copy the bytes from the io to the buffer.
            if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
            {
                if( 0 != mmioAdvance( m_hmmio, &mmioinfoIn, MMIO_READ ) )
                         TRACE ("ERROR DIRECTSOUND MMIOADVANCE")
                  	else /* TRACE ("DIRECTSOUND MMIOADVANCE OK")*/;


                if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
                     TRACE ("ERROR DIRECTSOUND READ FIC")
	            else /* TRACE ("DIRECTSOUND READ FIC OK") */;
            }

            // Actual copy.
            *((BYTE*)pDSLockedBuffer+cT) = *((BYTE*)mmioinfoIn.pchNext);
            mmioinfoIn.pchNext++;
    }

    if( 0 != mmioSetInfo( m_hmmio, &mmioinfoIn, 0 ) )
           TRACE ("ERROR DIRECTSOUND MMIOSETINFO")
	  else TRACE ("DIRECTSOUND MMIOSETINFO OK");

    mmioClose( m_hmmio, 0 );

    dwWavDataRead = cbDataIn;
    } // end of newbuffer part


//  Unlock the buffer, we don't need it anymore.
//  apDSBuffer[0]->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );


    if (offset > 0.)
    {
         DWORD curplay,curwrite;
		 char buffers[80];

         apDSBuffer[0]->GetCurrentPosition(&curplay,&curwrite);
         sprintf(buffers, "POS CURPLAY AVT %d", curplay);
         TRACE(buffers);

         curplay = DWORD (offset * 8000.);
	     hr = apDSBuffer[0]->SetCurrentPosition(curplay);
         if (hr != DS_OK)
		     TRACE ("ERROR DIRECTSOUND SETCURRENT")
	     else TRACE ("DIRECTSOUND SETCURRENT OK");

         apDSBuffer[0]->GetCurrentPosition(&curplay,&curwrite);
         sprintf(buffers, "POS CURPLAY APS %d", curplay);
         TRACE(buffers);
	}


//    if (newbuffer)
        apDSBuffer[0]->Play( 0, 0, 0L );


    return(true);  
}

int SoundEvent::IsPlaying()
{
    DWORD dwStatus = 0;

	if (apDSBuffer == NULL)
		return (false);
    apDSBuffer[0]->GetStatus( &dwStatus );
    return( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );

}

int SoundEvent::Finish(double offsetfinish)
{

TRACESETUP("FINISH");

    DWORD curplay,curwrite;
	char buffers[80];

	if (apDSBuffer == NULL)
		return(false);


	apDSBuffer[0]->GetCurrentPosition(&curplay,&curwrite);
    sprintf(buffers, "POS CURPLAY  %f %f", curplay/8000.,offsetfinish);
    TRACE(buffers);


	if ((curplay/8000.) > offsetfinish)
	{
        apDSBuffer[0]->Stop();
		TRACE("STOPPED");
		return(true);
	}

	return(false);
}



int SoundEvent::Stop()
{
	if (apDSBuffer == NULL)
		return (false);
    apDSBuffer[0]->Stop();
    return(true);
}

int SoundEvent::Done()
{
	if (apDSBuffer == NULL)
		return (false);
    // Unlock the buffer, we don't need it anymore.
    apDSBuffer[0]->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );
    return(true);
}
