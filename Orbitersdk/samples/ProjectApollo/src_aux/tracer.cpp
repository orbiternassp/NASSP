/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005



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
//############################################################################//

#include <stdio.h>
#include "tracer.h"
//############################################################################//
// Tracer class for simple debug output from modules. Feel free to steal
// this for any code you're working on yourself.
//############################################################################//
static int prefix_count = 0;
//############################################################################//
// Write the strings to a file. Note that we always open and close the
// file here... this will be slow, but it (almost) guarantees that it's
// been written to disk if the program later crashes. A hard lock could
// still potentially leave the data in the disk cach before it's been
// written, but those seem to be rare with Orbiter.
//############################################################################//
static void DoTrace(char *func,char *str)
{
	FILE *fp;
	static bool FirstTime=true;
	char *type="at";

	//Open for writing the first time, to delete any old data in the file.
	if(FirstTime){FirstTime=false;type="wt";}

	fp=fopen(trace_file,type);
	if(!fp)return;

	//Prefix with spaces to indent nicely.
	for(int i=0;i<prefix_count;i++)fputc(' ',fp);
	
	//Finally print the string.
	fprintf(fp,"%s: %s\n",func,str);
	fclose(fp);
}
//############################################################################//
     Tracer:: Tracer(char *s){prefix_count++;ThisFunc=s;DoTrace(s,"Starting");}
     Tracer::~Tracer()       {                          DoTrace(ThisFunc,"Done");prefix_count--;}
void Tracer::print(char *s)  {                          DoTrace(ThisFunc, s);}
//############################################################################//
