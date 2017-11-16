/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

S-IB Systems

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

#include "s1bsystems.h"

H1Engine::H1Engine(VESSEL *v, THRUSTER_HANDLE &h1, bool cangimbal, double pcant, double ycant)
	:vessel(v), th_h1(h1)
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
	ThrustNotOKCutoff = false;

	ThrustTimer = 0.0;
	ThrustLevel = 0.0;

	EngineCanGimbal = cangimbal;
	pitchCant = pcant;
	yawCant = ycant;
}

void H1Engine::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);
	papiWriteScenario_bool(scn, "ENGINESTART", EngineStart);
	papiWriteScenario_bool(scn, "ENGINESTOP", EngineStop);
	papiWriteScenario_bool(scn, "PROGRAMMEDCUTOFF", ProgrammedCutoff);
	papiWriteScenario_bool(scn, "EDSCUTOFF", EDSCutoff);
	papiWriteScenario_bool(scn, "GSECUTOFF", GSECutoff);
	papiWriteScenario_bool(scn, "RSSCUTOFF", RSSCutoff);
	papiWriteScenario_bool(scn, "THRUSTNOTOKCUTOFF", ThrustNotOKCutoff);
	papiWriteScenario_bool(scn, "ENGINERUNNING", EngineRunning);
	papiWriteScenario_double(scn, "THRUSTTIMER", ThrustTimer);
	oapiWriteLine(scn, end_str);
}

void H1Engine::LoadState(FILEHANDLE scn, char *end_str) {
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
		papiReadScenario_bool(line, "THRUSTNOTOKCUTOFF", ThrustNotOKCutoff);
		papiReadScenario_bool(line, "ENGINERUNNING", EngineRunning);
		papiReadScenario_double(line, "THRUSTTIMER", ThrustTimer);
	}
}

void H1Engine::Timestep(double simdt)
{
	if (th_h1 == NULL) return;

	//Thrust OK switch
	ThrustOK = vessel->GetThrusterLevel(th_h1) > 0.9 && !EngineFailed;

	if (ProgrammedCutoff || EDSCutoff || GSECutoff || RSSCutoff || ThrustNotOKCutoff)
	{
		EngineStop = true;
	}

	if (EngineStop)
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
				ThrustLevel = 1.0 - 3.35*(ThrustTimer - tm_1);
			}
			else
			{
				if (ThrustTimer < tm_3)
				{
					ThrustLevel = 0.33 - 0.825*(ThrustTimer - tm_2);
				}
				else
				{
					ThrustLevel = 0;
				}
			}
		}
		else
		{
			ThrustLevel = 1;
		}

		vessel->SetThrusterLevel(th_h1, ThrustLevel);
	}
	else if (EngineStart && !EngineRunning)
	{
		// SATURN IB ENGINE STARTUP
		// Engine startup was staggered 2-2-2-2, with engine 7+5 starting first, then 6+8, then 2+4, then 3+1

		// Engine 7+5 combustion chamber ignition was at T-2.998,  6+8 at T-2.898, 2+4 at T-2.798, 1+3 at T-2.698
		// The engines idled in low-range thrust (about 2.5% thrust) for about 0.3 seconds
		// and then rose to 93% thrust in 0.085 seconds.
		// The rise from 93 to 100 percent thrust took 0.75 second.
		// Total engine startup time was 1.9 seconds.

		// Source: Apollo 7 LV Flight Evaluation

		double tm_1, tm_2, tm_3, tm_4;

		ThrustTimer += simdt;

		tm_1 = -2.998 + 8.9;
		tm_2 = tm_1 + 0.3;  // Start of 1st rise
		tm_3 = tm_2 + 0.085; // Start of 2nd rise
		tm_4 = tm_3 + 0.75; // End of 2nd rise
		if (ThrustTimer >= tm_1) {
			// Light CC
			if (ThrustTimer < tm_2) {
				// Idle at 2.5% thrust
				ThrustLevel = 0.025;
			}
			else {
				if (ThrustTimer < tm_3) {
					// the actual rise is so fast that any 'smoothing' is pointless
					ThrustLevel = 0.93;
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

		vessel->SetThrusterLevel(th_h1, ThrustLevel);
	}
	else if (ThrustTimer > 0.0)
	{
		ThrustTimer = 0.0;
	}
}

void H1Engine::SetThrusterDir(double beta_y, double beta_p)
{
	if (th_h1 == NULL) return;
	if (!EngineCanGimbal) return;

	double pitch, yaw;
	VECTOR3 f1vector;

	if (beta_y > GIMBALLIMIT)
	{
		yaw = GIMBALLIMIT;
	}
	else if (beta_y < -GIMBALLIMIT)
	{
		yaw = -GIMBALLIMIT;
	}
	else
	{
		yaw = beta_y;
	}

	if (beta_p > GIMBALLIMIT)
	{
		pitch = GIMBALLIMIT;
	}
	else if (beta_p < -GIMBALLIMIT)
	{
		pitch = -GIMBALLIMIT;
	}
	else
	{
		pitch = beta_p;
	}

	f1vector = unit(_V(yaw + yawCant, pitch + pitchCant, 1.0));

	vessel->SetThrusterDir(th_h1, f1vector);
}

SIBSystems::SIBSystems(Saturn *v, THRUSTER_HANDLE *h1, PROPELLANT_HANDLE &h1prop, Sound &LaunchS, Sound &SShutS, double &contraillvl) :
	main_propellant(h1prop),
	h1engine1(v, h1[0], true, 2.45*RAD, 2.45*RAD),
	h1engine2(v, h1[1], true, 2.45*RAD, -2.45*RAD),
	h1engine3(v, h1[2], true, -2.45*RAD, -2.45*RAD),
	h1engine4(v, h1[3], true, -2.45*RAD, 2.45*RAD),
	h1engine5(v, h1[4], false, 0, 0),
	h1engine6(v, h1[5], false, 0, 0),
	h1engine7(v, h1[6], false, 0, 0),
	h1engine8(v, h1[7], false, 0, 0),
	SShutSound(SShutS),
	LaunchSound(LaunchS),
	contrailLevel(contraillvl)
{
	vessel = v;

	for (int i = 0;i < 8;i++)
	{
		ThrustOK[i] = false;
		EarlySICutoff[i] = false;
		FirstStageFailureTime[i] = 0.0;
	}

	SingleEngineCutoffEnabledLatch = false;
	SingleEngineCutoffInhibitRelay = false;
	MultipleEngineCutoffEnabledLatch1 = false;
	MultipleEngineCutoffEnabledLatch2 = false;
	FailInit = false;

	h1engines[0] = &h1engine1;
	h1engines[1] = &h1engine2;
	h1engines[2] = &h1engine3;
	h1engines[3] = &h1engine4;
	h1engines[4] = &h1engine5;
	h1engines[5] = &h1engine6;
	h1engines[6] = &h1engine7;
	h1engines[7] = &h1engine8;
}

void SIBSystems::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, SISYSTEMS_START_STRING);

	papiWriteScenario_bool(scn, "SINGLEENGINECUTOFFENABLEDLATCH", SingleEngineCutoffEnabledLatch);
	papiWriteScenario_bool(scn, "SINGLEENGINECUTOFFINHIBITRELAY", SingleEngineCutoffInhibitRelay);
	papiWriteScenario_bool(scn, "MULTIPLEENGINECUTOFFENABLEDLATCH1", MultipleEngineCutoffEnabledLatch1);
	papiWriteScenario_bool(scn, "MULTIPLEENGINECUTOFFENABLEDLATCH2", MultipleEngineCutoffEnabledLatch2);
	papiWriteScenario_bool(scn, "FAILINIT", FailInit);
	papiWriteScenario_boolarr(scn, "THRUSTOK", ThrustOK, 8);
	papiWriteScenario_boolarr(scn, "EARLYSICUTOFF", EarlySICutoff, 8);
	papiWriteScenario_doublearr(scn, "FIRSTSTAGEFAILURETIME", FirstStageFailureTime, 8);

	h1engine1.SaveState(scn, "ENGINE1_BEGIN", "ENGINE_END");
	h1engine2.SaveState(scn, "ENGINE2_BEGIN", "ENGINE_END");
	h1engine3.SaveState(scn, "ENGINE3_BEGIN", "ENGINE_END");
	h1engine4.SaveState(scn, "ENGINE4_BEGIN", "ENGINE_END");
	h1engine5.SaveState(scn, "ENGINE5_BEGIN", "ENGINE_END");
	h1engine6.SaveState(scn, "ENGINE6_BEGIN", "ENGINE_END");
	h1engine7.SaveState(scn, "ENGINE7_BEGIN", "ENGINE_END");
	h1engine8.SaveState(scn, "ENGINE8_BEGIN", "ENGINE_END");

	oapiWriteLine(scn, SISYSTEMS_END_STRING);
}

void SIBSystems::LoadState(FILEHANDLE scn) {
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, SISYSTEMS_END_STRING, sizeof(SISYSTEMS_END_STRING)))
			return;

		papiReadScenario_bool(line, "SINGLEENGINECUTOFFENABLEDLATCH", SingleEngineCutoffEnabledLatch);
		papiReadScenario_bool(line, "SINGLEENGINECUTOFFINHIBITRELAY", SingleEngineCutoffInhibitRelay);
		papiReadScenario_bool(line, "MULTIPLEENGINECUTOFFENABLED1LATCH", MultipleEngineCutoffEnabledLatch1);
		papiReadScenario_bool(line, "MULTIPLEENGINECUTOFFENABLEDLATCH2", MultipleEngineCutoffEnabledLatch2);
		papiReadScenario_bool(line, "FAILINIT", FailInit);
		papiReadScenario_boolarr(line, "THRUSTOK", ThrustOK, 8);
		papiReadScenario_boolarr(line, "EARLYSICUTOFF", EarlySICutoff, 8);
		papiReadScenario_doublearr(line, "FIRSTSTAGEFAILURETIME", FirstStageFailureTime, 8);

		if (!strnicmp(line, "ENGINE1_BEGIN", sizeof("ENGINE1_BEGIN"))) {
			h1engine1.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE2_BEGIN", sizeof("ENGINE2_BEGIN"))) {
			h1engine2.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE3_BEGIN", sizeof("ENGINE3_BEGIN"))) {
			h1engine3.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE4_BEGIN", sizeof("ENGINE4_BEGIN"))) {
			h1engine4.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE5_BEGIN", sizeof("ENGINE5_BEGIN"))) {
			h1engine5.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE6_BEGIN", sizeof("ENGINE6_BEGIN"))) {
			h1engine6.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE7_BEGIN", sizeof("ENGINE7_BEGIN"))) {
			h1engine7.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE8_BEGIN", sizeof("ENGINE8_BEGIN"))) {
			h1engine8.LoadState(scn, "ENGINE_END");
		}
	}
}

void SIBSystems::Timestep(double simdt)
{
	//Sanity check
	if (vessel->GetStage() >= LAUNCH_STAGE_TWO) return;

	h1engine1.Timestep(simdt);
	h1engine2.Timestep(simdt);
	h1engine3.Timestep(simdt);
	h1engine4.Timestep(simdt);
	h1engine5.Timestep(simdt);
	h1engine6.Timestep(simdt);
	h1engine7.Timestep(simdt);
	h1engine8.Timestep(simdt);

	//Thrust OK
	for (int i = 0;i < 5;i++)
	{
		ThrustOK[i] = h1engines[i]->GetThrustOK();
	}

	if (vessel->GetStage() == LAUNCH_STAGE_ONE)
	{
		Liftoff = true;
	}
	else
	{
		Liftoff = false;
	}

	if (PropellantLevelSensorsEnabledLatch)
	{
		PropellantLevelSensorsEnabledRelay = true;
	}
	else
	{
		PropellantLevelSensorsEnabledRelay = false;
	}

	if (PropellantLevelSensorsEnabledRelay)
	{
		K75 = true;
	}
	else
	{
		K75 = false;
	}

	//TBD: Fuel and LOX Level Sensor

	if (K76 || (!K75 && LOXLevelSensor))
	{
		K76 = true;
	}
	else
	{
		K76 = false;
	}

	if (K77 || (!K75 && FuelLevelSensor))
	{
		K77 = true;
	}
	else
	{
		K77 = false;
	}

	if (!MultipleEngineCutoffEnabledLatch1 && !MultipleEngineCutoffEnabledLatch2)
	{
		MultiEngineCutoffInhibitRelay = true;
		MultiEngineCutoffInhibitBusPowered = true;
	}
	else
	{
		MultiEngineCutoffInhibitRelay = false;
		MultiEngineCutoffInhibitBusPowered = false;
	}

	if (!SingleEngineCutoffEnabledLatch && Liftoff)
	{
		SingleEngineCutoffInhibitRelay = true;
	}
	else
	{
		SingleEngineCutoffInhibitRelay = false;
	}

	if ((EngineCutoffRelay[0] || EngineCutoffRelay[1] || EngineCutoffRelay[2] || EngineCutoffRelay[3] ||
		EngineCutoffRelay[4] || EngineCutoffRelay[5] || EngineCutoffRelay[6] || EngineCutoffRelay[7]) && 
		MultiEngineCutoffInhibitBusPowered)
	{
		SingleEngineFailedInhibitRelay = true;
	}
	else
	{
		SingleEngineFailedInhibitRelay = false;
	}

	if (InboardEnginesCutoffLatch)
	{
		InboardEnginesCutoffRelay = true;
	}
	else
	{
		InboardEnginesCutoffRelay = false;
	}

	if (LOXDepletionCutoffEnabledLatch)
	{
		LOXDepletionCutoffEnabledRelay = true;
	}
	else
	{
		LOXDepletionCutoffEnabledRelay = false;
	}

	if (OutboardEnginesCutoffLatch || (FuelDepletionCutoffEnabled && FuelDepletionSensors))
	{
		OutboardEnginesCutoffRelay = true;
	}
	else
	{
		OutboardEnginesCutoffRelay = false;
	}

	//Outboard Engines
	for (int i = 0;i < 4;i++)
	{
		if (((!SingleEngineFailedInhibitRelay && (!MultiEngineCutoffInhibitRelay || !SingleEngineCutoffInhibitRelay)) || EngineCutoffRelay[i]) && !h1engines[i]->GetThrustOK())
		{
			EngineCutoffRelay[i] = true;
		}
		else if (LOXDepletionCutoffEnabledRelay && OutboardEnginesCutoffRelay)
		{
			EngineCutoffRelay[i] = true;
		}
		else
		{
			EngineCutoffRelay[i] = false;
		}
	}

	//Inboard Engines
	for (int i = 4;i < 8;i++)
	{
		if (((!SingleEngineFailedInhibitRelay && !SingleEngineCutoffInhibitRelay) || EngineCutoffRelay[i]) && !h1engines[i]->GetThrustOK())
		{
			EngineCutoffRelay[i] = true;
		}
		else if (InboardEnginesCutoffRelay && PropellantLevelSensorsEnabledRelay)
		{
			EngineCutoffRelay[i] = true;
		}
		else
		{
			EngineCutoffRelay[i] = false;
		}
	}

	for (int i = 0;i < 8;i++)
	{
		if (EngineCutoffRelay[i])
		{
			h1engines[i]->SetThrustNotOKCutoff();
		}
	}

	//Failure code

	for (int i = 0;i < 8;i++)
	{
		if (EarlySICutoff[i] && (vessel->GetMissionTime() > FirstStageFailureTime[i]) && !h1engines[i]->GetFailed())
		{
			h1engines[i]->SetFailed();
		}
	}

	//Contrail Level

	contrailLevel = min(1.0, max(0.0, GetSumThrust()*(-vessel->GetAltitude(ALTMODE_GROUND) + 400.0) / 300.0));
}

void SIBSystems::SetThrusterDir(int n, double beta_y, double beta_p)
{
	if (n < 1 || n > 4) return;

	h1engines[n - 1]->SetThrusterDir(beta_y, beta_p);
}

double SIBSystems::GetSumThrust()
{
	double thrust = 0.0;

	for (int i = 0;i < 8;i++)
	{
		thrust += h1engines[i]->GetThrustLevel();
	}

	return thrust;
}

void SIBSystems::SwitchSelector(int channel)
{
	if (vessel->GetStage() > LAUNCH_STAGE_ONE) return;

	switch (channel)
	{
	case 0: //Liftoff (NOT A REAL SWITCH SELECTOR EVENT)
		vessel->SetStage(LAUNCH_STAGE_ONE);					// Switch to stage one
		if (LaunchSound.isValid() && !LaunchSound.isPlaying()) {	// And play launch sound
			LaunchSound.play(NOLOOP, 255);
			LaunchSound.done();
		}
		break;
	case 2: //Telemeter Calibration On
		break;
	case 15: //Multiple Engine Cutoff Enable No. 2
		SetMultipleEngineCutoffEnable2();
		break;
	case 16: //Multiple Engine Cutoff Enable No. 1
		SetMultipleEngineCutoffEnable1();
		break;
	case 18: //Outboard Engines Cutoff
		SetOutboardEnginesCutoff();
		break;
	case 23: //S-IB/S-IVB Separation On
		if (vessel->GetStage() == LAUNCH_STAGE_ONE)
		{
			vessel->SeparateStage(LAUNCH_STAGE_SIVB);
			vessel->SetStage(LAUNCH_STAGE_SIVB);
			vessel->AddRCS_S4B();
		}
		break;
	case 39: //Telemeter Calibration Off
		break;
	case 79: //FUel Depletion Cutoff Enable
		SetFuelDepletionCutoffEnable();
		break;
	case 97: //LOX Depletion Cutoff Enable
		SetLOXDepletionCutoffEnable();
		break;
	case 98: //Inboard Engines Cutoff
		SetInboardEnginesCutoff();
		SShutSound.play(NOLOOP, 235);
		SShutSound.done();
		break;
	case 100: //Single Engine Cutoff Enable
		SetSingleEngineCutoffEnable();
		break;
	case 104: //Propellant Level Sensors Enable
		SetPropellantLevelSensorsEnable();
		break;
	default:
		break;
	}
}