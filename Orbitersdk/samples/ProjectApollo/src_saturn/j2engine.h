/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

J-2 Engine (Header)

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

#pragma once

class J2Engine
{
public:
	J2Engine(VESSEL *v, THRUSTER_HANDLE &f1);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	void SetThrusterIsp(double isp) { vessel->SetThrusterIsp(th_j2, isp, isp); }
	void SetThrusterMax0(double thrust) { vessel->SetThrusterMax0(th_j2, thrust); }

	void SetEngineReadyBypass() { EngineReadyBypass = true; }
	void ResetEngineReadyBypass() { EngineReadyBypass = false; }
	void SetEngineStart() { EngineStart = true; }
	void ResetEngineStart() { EngineStart = false; }
	void SetLVDCEngineCutoff() { LVDCEngineStopRelay = true; }
	void ResetLVDCEnginesCutoff() { LVDCEngineStopRelay = false; }
	void SetEDSCutoff() { EDSCutoff = true; }
	void ResetEDSCutoff() { EDSCutoff = false; }
	void SetThrusterDir(double beta_y, double beta_p);
	void SetFailed() { EngineFailed = true; }

	bool GetThrustOK() { return ThrustOK; }
	bool GetFailed() { return EngineFailed; }
protected:
	THRUSTER_HANDLE &th_j2;
	VESSEL *vessel;

	bool EngineReady;
	bool EngineReadyBypass;
	bool EngineStart;
	bool EngineStop;
	bool LVDCEngineStopRelay;
	bool EDSCutoff;
	bool RSSCutoff;

	bool ThrustOK;
	bool EngineRunning;

	bool EngineFailed;

	double ThrustTimer;
	double ThrustLevel;
};