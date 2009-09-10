/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2009

  LM Telecommunications Subssytems

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

// VHF system (and shared stuff)
class LM_VHF {
public:
	LM_VHF();
	void Init(LEM *vessel);	       // Initialization
	void TimeStep(double simt);        // TimeStep
	void SystemTimestep(double simdt); // System Timestep
	LEM *lem;					   // Ship we're installed in
};

// S-Band system
class LM_SBAND {
public:
	LM_SBAND();
	void Init(LEM *vessel);	       // Initialization
	void TimeStep(double simt);        // TimeStep
	void SystemTimestep(double simdt); // System Timestep
	LEM *lem;					   // Ship we're installed in
	int pa_mode_1,pa_mode_2;       // Power amplifier state
	double pa_timer_1,pa_timer_2;  // Tube heater timer
	int tc_mode_1,tc_mode_2;	   // Transciever state
	double tc_timer_1,tc_timer_2;  // Tube heater timer
};

// S-Band Steerable Antenna
class LEM_SteerableAnt{
public:
	LEM_SteerableAnt();
	void Init(LEM *s);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simdt);
	double GetAntennaTempF();

	LEM *lem;					// Pointer at LEM
	h_Radiator antenna;			// Antenna (loses heat into space)
	Boiler antheater;			// Antenna Heater (puts heat back into antenna)
};
