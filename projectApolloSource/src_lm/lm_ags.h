/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2009

  LM Abort Guidance System

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

// ABORT SENSOR ASSEMBLY (ASA)
class LEM_ASA{
public:
	LEM_ASA();							// Cons
	void Init(LEM *s); // Init
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simdt);
	LEM *lem;					// Pointer at LEM
	h_Radiator hsink;			// Case (Connected to primary coolant loop)
	Boiler heater;				// Heater
};

// ABORT ELECTRONICS ASSEMBLY (AEA)
class LEM_AEA{
public:
	LEM_AEA();							// Cons
	void Init(LEM *s); // Init
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simdt);
	LEM *lem;					// Pointer at LEM
};

// DATA ENTRY and DISPLAY ASSEMBLY (DEDA)
class LEM_DEDA{
public:
	LEM_DEDA();							// Cons
	void Init(LEM *s); // Init
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simdt);
	LEM *lem;					// Pointer at LEM
};

