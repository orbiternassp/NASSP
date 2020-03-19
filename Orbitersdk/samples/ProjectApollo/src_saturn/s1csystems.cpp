/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

S-IC Systems

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

#include "Orbitersdk.h"

#include "soundlib.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "papi.h"
#include "TSMUmbilical.h"

#include "s1csystems.h"

F1Engine::F1Engine(VESSEL *v, THRUSTER_HANDLE &f1)
	:vessel(v), th_f1(f1)
{
	EngineStart = false;
	EngineStop = false;
	ProgrammedCutoff = false;
	EDSCutoff = false;
	GSECutoff = false;
	RSSCutoff = false;
	EngineRunning = false;
	ThrustOK = false;
	EngineFailed = false;

	ThrustTimer = 0.0;
	ThrustLevel = 0.0;

	pitchCmd = 0.0;
	pitchPos = 0.0;
	yawCmd = 0.0;
	yawPos = 0.0;
}

void F1Engine::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);
	papiWriteScenario_bool(scn, "ENGINESTART", EngineStart);
	papiWriteScenario_bool(scn, "ENGINESTOP", EngineStop);
	papiWriteScenario_bool(scn, "PROGRAMMEDCUTOFF", ProgrammedCutoff);
	papiWriteScenario_bool(scn, "EDSCUTOFF", EDSCutoff);
	papiWriteScenario_bool(scn, "GSECUTOFF", GSECutoff);
	papiWriteScenario_bool(scn, "RSSCUTOFF", RSSCutoff);
	papiWriteScenario_bool(scn, "ENGINERUNNING", EngineRunning);
	papiWriteScenario_double(scn, "THRUSTTIMER", ThrustTimer);
	papiWriteScenario_double(scn, "PITCHCMD", pitchCmd);
	papiWriteScenario_double(scn, "PITCHPOS", pitchPos);
	papiWriteScenario_double(scn, "YAWCMD", yawCmd);
	papiWriteScenario_double(scn, "YAWPOS", yawPos);
	oapiWriteLine(scn, end_str);
}

void F1Engine::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		papiReadScenario_bool(line, "ENGINESTART", EngineStart);
		papiReadScenario_bool(line, "ENGINESTOP", EngineStop);
		papiReadScenario_bool(line, "PROGRAMMEDCUTOFF", ProgrammedCutoff);
		papiReadScenario_bool(line, "EDSCUTOFF", EDSCutoff);
		papiReadScenario_bool(line, "GSECUTOFF", GSECutoff);
		papiReadScenario_bool(line, "RSSCUTOFF", RSSCutoff);
		papiReadScenario_bool(line, "ENGINERUNNING", EngineRunning);
		papiReadScenario_double(line, "THRUSTTIMER", ThrustTimer);
		papiReadScenario_double(line, "PITCHCMD", pitchCmd);
		papiReadScenario_double(line, "PITCHPOS", pitchPos);
		papiReadScenario_double(line, "YAWCMD", yawCmd);
		papiReadScenario_double(line, "YAWPOS", yawPos);
	}
}

void F1Engine::Timestep(double simdt)
{
	if (th_f1 == NULL) return;

	//Thrust OK switch
	ThrustOK = vessel->GetThrusterLevel(th_f1) > 0.9 && !EngineFailed;

	if (ProgrammedCutoff || EDSCutoff || GSECutoff || RSSCutoff)
	{
		EngineStop = true;
	}

	if (EngineStop)
	{
		if (ThrustLevel > 0.0)
		{
			EngineStart = false;
			EngineRunning = false;

			double tm_1, tm_2, tm_3;

			ThrustTimer += simdt;

			tm_1 = 0.1;
			tm_2 = 0.3;
			tm_3 = 0.7;

			if (ThrustTimer >= tm_1)
			{
				if (ThrustTimer < tm_2)
				{
					ThrustLevel = min(ThrustLevel, 1.0 - 3.35*(ThrustTimer - tm_1));
				}
				else
				{
					if (ThrustTimer < tm_3)
					{
						ThrustLevel = min(ThrustLevel, 0.33 - 0.825*(ThrustTimer - tm_2));
					}
					else
					{
						ThrustLevel = 0;
					}
				}
			}
			else
			{
				ThrustLevel = min(ThrustLevel, 1);
			}

			vessel->SetThrusterLevel(th_f1, ThrustLevel);
		}
	}
	else if (EngineStart && !EngineRunning)
	{
		// SATURN V ENGINE STARTUP
		// Engine startup was staggered 1-2-2, with engine 5 starting first, then 1+3, then 2+4. 
		// This happened by the starter solenoid operating at T-6.585 for engine 5.

		// Engine 5 combustion chamber ignition was at T-3.315, engines 1+3 at T-3.035, and engines 2+4 at T-2.615
		// The engines idled in low-range thrust (about 2.5% thrust) for about 0.3 seconds
		// and then rose to 93% thrust in 0.85 seconds.
		// The rise from 93 to 100 percent thrust took 0.75 second.
		// Total engine startup time was 1.9 seconds.

		// Source: Apollo 8 LV Flight Evaluation

		double tm_1, tm_2, tm_3, tm_4;

		ThrustTimer += simdt;

		tm_1 = -3.315 + 8.9;
		tm_2 = tm_1 + 0.3;  // Start of 1st rise
		tm_3 = tm_2 + 0.85; // Start of 2nd rise
		tm_4 = tm_3 + 0.75; // End of 2nd rise
		if (ThrustTimer >= tm_1) {
			// Light CC
			if (ThrustTimer < tm_2) {
				// Idle at 2.5% thrust
				ThrustLevel = 0.025;
			}
			else {
				if (ThrustTimer < tm_3) {
					// Rise to 93% at a rate of 106 percent per second
					ThrustLevel = 0.025 + (1.06*(ThrustTimer - tm_2));
				}
				else {
					if (ThrustTimer < tm_4) {
						// Rise to 100% at a rate of 9 percent per second.
						ThrustLevel = 0.93 + (0.09*(ThrustTimer - tm_3));
					}
					else {
						// Hold 100%
						ThrustLevel = 1;
						EngineRunning = true;
					}
				}
			}
		}
		else
		{
			ThrustLevel = 0.0;
		}

		vessel->SetThrusterLevel(th_f1, ThrustLevel);
	}
	else if (ThrustTimer > 0.0)
	{
		ThrustTimer = 0.0;
	}

	//Gimbal timesteps
	ServoDrive(pitchPos, pitchCmd, 5.0*RAD, simdt);
	ServoDrive(yawPos, yawCmd, 5.0*RAD, simdt);

	vessel->SetThrusterDir(th_f1, _V(yawPos, pitchPos, 1));
}

void F1Engine::SetThrusterDir(double beta_y, double beta_p)
{
	if (th_f1 == NULL) return;

	if (beta_y > 5.16*RAD)
	{
		beta_y = 5.16*RAD;
	}
	else if (beta_y < -5.16*RAD)
	{
		beta_y = -5.16*RAD;
	}

	if (beta_p > 5.16*RAD)
	{
		beta_p = 5.16*RAD;
	}
	else if (beta_p < -5.16*RAD)
	{
		beta_p = -5.16*RAD;
	}

	pitchCmd = beta_p;
	yawCmd = beta_y;
}

void F1Engine::ServoDrive(double &Angle, double AngleCmd, double RateLimit, double simdt)
{
	double dposcmd, dpos;

	dposcmd = AngleCmd - Angle;

	if (abs(dposcmd) > RateLimit*simdt)
	{
		dpos = sign(AngleCmd - Angle)*RateLimit*simdt;
	}
	else
	{
		dpos = dposcmd;
	}
	Angle += dpos;
}

SICSystems::SICSystems(VESSEL *v, THRUSTER_HANDLE *f1, PROPELLANT_HANDLE &f1prop, Pyro &SIC_SII_Sep, Sound &LaunchS, Sound &SShutS, double &contraillvl) :
	main_propellant(f1prop),
	f1engine1(v, f1[2]),
	f1engine2(v, f1[1]),
	f1engine3(v, f1[3]),
	f1engine4(v, f1[0]),
	f1engine5(v, f1[4]),
	SShutSound(SShutS),
	LaunchSound(LaunchS),
	contrailLevel(contraillvl),
	SIC_SII_Separation_Pyros(SIC_SII_Sep)
{
	vessel = v;

	for (int i = 0;i < 5;i++)
	{
		EarlySICutoff[i] = false;
		FirstStageFailureTime[i] = 0.0;
	}
	for (int i = 0;i < 15;i++)
	{
		ThrustOK[i] = false;
	}

	MultipleEngineCutoffEnabled = false;
	PropellantDepletionSensors = false;
	PointLevelSensorArmed = false;
	TwoAdjacentOutboardEnginesOutCutoff = false;

	f1engines[0] = &f1engine1;
	f1engines[1] = &f1engine2;
	f1engines[2] = &f1engine3;
	f1engines[3] = &f1engine4;
	f1engines[4] = &f1engine5;
}

SICSystems::~SICSystems()
{
	if (TSMUmb)
	{
		TSMUmb->AbortDisconnect();
	}
}

void SICSystems::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, SISYSTEMS_START_STRING);

	papiWriteScenario_bool(scn, "MULTIPLEENGINECUTOFFENABLED", MultipleEngineCutoffEnabled);
	papiWriteScenario_bool(scn, "PROPELLANTDEPLETIONSENSORS", PropellantDepletionSensors);
	papiWriteScenario_bool(scn, "POINTLEVELSENSORARMED", PointLevelSensorArmed);
	papiWriteScenario_bool(scn, "TWOADJACENTOUTBOARDENGINESOUTCUTOFF", TwoAdjacentOutboardEnginesOutCutoff);
	papiWriteScenario_boolarr(scn, "THRUSTOK", ThrustOK, 15);

	f1engine1.SaveState(scn, "ENGINE1_BEGIN", "ENGINE_END");
	f1engine2.SaveState(scn, "ENGINE2_BEGIN", "ENGINE_END");
	f1engine3.SaveState(scn, "ENGINE3_BEGIN", "ENGINE_END");
	f1engine4.SaveState(scn, "ENGINE4_BEGIN", "ENGINE_END");
	f1engine5.SaveState(scn, "ENGINE5_BEGIN", "ENGINE_END");

	oapiWriteLine(scn, SISYSTEMS_END_STRING);
}

void SICSystems::LoadState(FILEHANDLE scn) {
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, SISYSTEMS_END_STRING, sizeof(SISYSTEMS_END_STRING)))
			return;

		papiReadScenario_bool(line, "MULTIPLEENGINECUTOFFENABLED", MultipleEngineCutoffEnabled);
		papiReadScenario_bool(line, "PROPELLANTDEPLETIONSENSORS", PropellantDepletionSensors);
		papiReadScenario_bool(line, "POINTLEVELSENSORARMED", PointLevelSensorArmed);
		papiReadScenario_bool(line, "TWOADJACENTOUTBOARDENGINESOUTCUTOFF", TwoAdjacentOutboardEnginesOutCutoff);
		papiReadScenario_boolarr(line, "THRUSTOK", ThrustOK, 15);

		if (!strnicmp(line, "ENGINE1_BEGIN", sizeof("ENGINE1_BEGIN"))) {
			f1engine1.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE2_BEGIN", sizeof("ENGINE2_BEGIN"))) {
			f1engine2.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE3_BEGIN", sizeof("ENGINE3_BEGIN"))) {
			f1engine3.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE4_BEGIN", sizeof("ENGINE4_BEGIN"))) {
			f1engine4.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE5_BEGIN", sizeof("ENGINE5_BEGIN"))) {
			f1engine5.LoadState(scn, "ENGINE_END");
		}
	}
}

void SICSystems::Timestep(double misst, double simdt)
{
	f1engine1.Timestep(simdt);
	f1engine2.Timestep(simdt);
	f1engine3.Timestep(simdt);
	f1engine4.Timestep(simdt);
	f1engine5.Timestep(simdt);

	//Thrust OK
	for (int i = 0;i < 5;i++)
	{
		for (int j = 0;j < 3;j++)
		{
			ThrustOK[i * 3 + j] = f1engines[i]->GetThrustOK() || ESEGetSICThrustOKSimulate(i + 1, j + 1);
		}
	}

	//Propellant Depletion
	if (PropellantLowLevel())
	{
		PropellantDepletionSensors = true;
	}
	else
	{
		PropellantDepletionSensors = false;
	}

	if (PropellantDepletionSensors)
	{
		OutboardEnginesCutoff();
	}

	if (TwoAdjacentOutboardEnginesOutCutoff)
	{
		if ((!f1engine1.GetThrustOK() && !f1engine2.GetThrustOK()) ||
			(!f1engine2.GetThrustOK() && !f1engine3.GetThrustOK()) ||
			(!f1engine3.GetThrustOK() && !f1engine4.GetThrustOK()) ||
			(!f1engine4.GetThrustOK() && !f1engine1.GetThrustOK()))
		{
			OutboardEnginesCutoff();
		}
	}

	if (MultipleEngineCutoffEnabled)
	{
		for (int i = 0;i < 5;i++)
		{
			if (f1engines[i]->GetThrustOK() == false) f1engines[i]->SetProgrammedEngineCutoff();
		}
	}

	//Failure code

	for (int i = 0;i < 5;i++)
	{
		if (EarlySICutoff[i] && (misst > FirstStageFailureTime[i]) && !f1engines[i]->GetFailed())
		{
			f1engines[i]->SetFailed();
		}
	}

	//Contrail Level

	contrailLevel = min(1.0, max(0.0, GetSumThrust()*(-vessel->GetAltitude(ALTMODE_GROUND) + 400.0) / 300.0));

	//sprintf(oapiDebugString(), "%f %f %f %f %f %f", contrailLevel, f1engine1.GetThrustLevel(), f1engine2.GetThrustLevel(), f1engine3.GetThrustLevel(), f1engine4.GetThrustLevel(), f1engine5.GetThrustLevel());
	//sprintf(oapiDebugString(), "%f %f %f %f %f", FirstStageFailureTime[0], FirstStageFailureTime[1], FirstStageFailureTime[2], FirstStageFailureTime[3], FirstStageFailureTime[4]);
}

bool SICSystems::PropellantLowLevel()
{
	if (PointLevelSensorArmed)
	{
		if (main_propellant)
		{
			if (vessel->GetPropellantMass(main_propellant) < 30000.0)
			{
				return true;
			}
		}
	}

	return false;
}

void SICSystems::InboardEngineCutoff()
{
	f1engine5.SetProgrammedEngineCutoff();
}

void SICSystems::OutboardEnginesCutoff()
{
	f1engine1.SetProgrammedEngineCutoff();
	f1engine2.SetProgrammedEngineCutoff();
	f1engine3.SetProgrammedEngineCutoff();
	f1engine4.SetProgrammedEngineCutoff();
}

void SICSystems::SetEngineStart(int n)
{
	if (n < 1 || n > 5) return;

	f1engines[n - 1]->SetEngineStart();
}

void SICSystems::SetThrusterDir(int n, double beta_y, double beta_p)
{
	if (n < 0 || n > 3) return;

	f1engines[n]->SetThrusterDir(beta_y, beta_p);
}

void SICSystems::EDSEnginesCutoff(bool cut)
{
	if (cut)
	{
		f1engine1.SetEDSCutoff();
		f1engine2.SetEDSCutoff();
		f1engine3.SetEDSCutoff();
		f1engine4.SetEDSCutoff();
		f1engine5.SetEDSCutoff();
	}
}

void SICSystems::GSEEnginesCutoff(bool cut)
{
	if (cut)
	{
		f1engine1.SetGSECutoff();
		f1engine2.SetGSECutoff();
		f1engine3.SetGSECutoff();
		f1engine4.SetGSECutoff();
		f1engine5.SetGSECutoff();
	}
}

void SICSystems::GetThrustOK(bool *ok)
{
	for (int i = 0;i < 15;i++)
	{
		ok[i] = ThrustOK[i];
	}
}

void SICSystems::SetEngineFailureParameters(bool *SICut, double *SICutTimes)
{
	for (int i = 0;i < 5;i++)
	{
		EarlySICutoff[i] = SICut[i];
		FirstStageFailureTime[i] = SICutTimes[i];
	}
}

void SICSystems::SetEngineFailureParameters(int n, double SICutTimes, bool fail)
{
	if (n < 1 || n > 5) return;

	EarlySICutoff[n - 1] = fail;
	FirstStageFailureTime[n - 1] = SICutTimes;
}

void SICSystems::GetEngineFailureParameters(int n, bool &fail, double &failtime)
{
	if (n < 1 || n > 5) return;

	fail = EarlySICutoff[n - 1];
	failtime = FirstStageFailureTime[n - 1];
}

double SICSystems::GetSumThrust()
{
	return (f1engine1.GetThrustLevel() + f1engine2.GetThrustLevel() + f1engine3.GetThrustLevel() + f1engine4.GetThrustLevel() + f1engine5.GetThrustLevel()) / 5.0;
}

bool SICSystems::TripleVoting(bool vote1, bool vote2, bool vote3)
{
	int num = 0;
	if (vote1) num++;
	if (vote2) num++;
	if (vote3) num++;
	if (num >= 2) return true;
	return false;
}

void SICSystems::SwitchSelector(int channel)
{
	switch (channel)
	{
	case 0: //Liftoff (NOT A REAL SWITCH SELECTOR EVENT)
		if (LaunchSound.isValid() && !LaunchSound.isPlaying()) {			// And play launch sound			
			LaunchSound.play(NOLOOP, 255);
			LaunchSound.done();
		}
		break;
	case 1: //Telemeter Calibrate Off
		break;
	case 2: //Telemeter Calibrate On
		break;
	case 3: //Multiple Engine Cutoff Enable
		MultipleEngineCutoffEnable();
		break;
	case 4: //LOX Tank Strobe Lights Off
		break;
	case 5: //Fuel Pressurizing Valve No. 2 Open & Tape Recorder Record
		break;
	case 6: //Fuel Pressurizing Valve No. 3 Open
		break;
	case 7: //Fuel Pressurizing Valve No. 4 Open
		break;
	case 8: //Inboard Engine Cutoff
		InboardEngineCutoff();
		SShutSound.play(NOLOOP, 235);
		SShutSound.done();
		break;
	case 9: //Outboard Engines Cutoff Enable
		OutboardEnginesCutoffEnable();
		break;
	case 10: //Separation and Retro No. 1 EBW Firing Units Arm
		break;
	case 12: //Separation Camera On
		break;
	case 13: //Telemetry Measurement Switchover
		break;
	case 14: //Outboard Engines Cutoff Backup Enable
		OutboardEnginesCutoffEnable();
		break;
	case 15: //S-IC/S-II Separation (No. 1)
		SIC_SII_Separation_Pyros.SetBlown(true);
		break;
	case 16: //Inboard Engine Cutoff Backup
		InboardEngineCutoff();
		break;
	case 17: //Two Adjacent Outboard Engines Out Cutoff Enable
		TwoAdjacentOutboardEnginesOutCutoffEnable();
		break;
	case 19: //S-IC/S-II Separation (No. 2)
		SIC_SII_Separation_Pyros.SetBlown(true);
		break;
	case 20: //Separation and Retro No. 2 EBW Firing Units Arm
		break;
	default:
		break;
	}
}

bool SICSystems::GetPropellantDepletionEngineCutoff()
{
	if (PointLevelSensorArmed)
	{
		if (PropellantDepletionSensors) return true;
		if (!TripleVoting(ThrustOK[0], ThrustOK[1], ThrustOK[2]) && !TripleVoting(ThrustOK[3], ThrustOK[4], ThrustOK[5]) && !TripleVoting(ThrustOK[6], ThrustOK[7], ThrustOK[8]) && !TripleVoting(ThrustOK[9], ThrustOK[10], ThrustOK[11])) return true;
	}

	return false;
}

bool SICSystems::GetInboardEngineOut()
{
	if (!TripleVoting(ThrustOK[12], ThrustOK[13], ThrustOK[14])) return true;

	return false;
}

bool SICSystems::GetOutboardEngineOut()
{
	for (int i = 0;i < 4;i++) if (!TripleVoting(ThrustOK[3 * i], ThrustOK[3 * i + 1], ThrustOK[3 * i + 2])) return true;

	return false;
}

bool SICSystems::GetEngineStop()
{
	for (int i = 0;i < 5;i++) if (f1engines[i]->GetEngineStop()) return true;

	return false;
}

void SICSystems::ConnectUmbilical(TSMUmbilical *umb)
{
	TSMUmb = umb;
}

void SICSystems::DisconnectUmbilical()
{
	TSMUmb = NULL;
}

bool SICSystems::IsUmbilicalConnected()
{
	if (TSMUmb && TSMUmb->IsUmbilicalConnected()) return true;

	return false;
}

bool SICSystems::ESEGetSICThrustOKSimulate(int eng, int n)
{
	if (!IsUmbilicalConnected()) return false;

	return TSMUmb->ESEGetSICThrustOKSimulate(eng, n);
}