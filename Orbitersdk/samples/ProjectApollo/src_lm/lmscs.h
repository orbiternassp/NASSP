/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  LM Stabilization & Control System (and associated parts)

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

class LEM;

class ATCA {
	// ATTITUDE & TRANSLATION CONTROL ASSEMBLY
public:
	ATCA();								// Cons
	void Init(LEM *vessel);				// Init
	void Timestep(double simt);			// Timestep
	void ProcessLGC(int ch, int val);   // To process LGC commands

	LEM *lem;
	int lgc_err_x,lgc_err_y,lgc_err_z;	// LGC attitude error counters
	int lgc_err_ena;                    // LGC error counter enabled
	int jet_request[16];				// Jet request list
	int jet_last_request[16];			// Jet request list at last timestep
	double jet_start[16],jet_stop[16];  // RCS jet start/stop times
};

class DECA {
	// DESCENT ENGINE CONTROL ASSEMBLY
public:
	DECA();									// Cons
	void Init(LEM *vessel, e_object *dcbus);// Init
	void Timestep(double simt);				// Timestep
	void SystemTimestep(double simdt);		// System Timestep
	double GetCommandedThrust() { return dpsthrustcommand; }

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

protected:
	LEM *lem;
	e_object *dc_source;			     // DC source to use when powered
	bool powered;					 // Data valid flag.
	int pitchactuatorcommand, rollactuatorcommand;
	bool engOn, engOff;
	double dpsthrustcommand;
	double lgcAutoThrust;
};

class GASTA {
	//GIMBAL ANGLE SEQUENCING TRANSFORMATION ASSEMBLY
public:
	GASTA();
	void Init(LEM *s, e_object *dcsource, e_object *acsource, IMU* imu);
	void Timestep(double simt);				// Timestep
	void SystemTimestep(double simdt);		// System Timestep
	bool IsPowered();
	VECTOR3 GetTotalAttitude() { return gasta_att; }

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

protected:
	LEM *lem;
	e_object *dc_source;			     // DC source to use when powered
	e_object *ac_source;			     // AC source to use when powered
	IMU *imu;							 //Connected IMU
	VECTOR3 gasta_att;
};



