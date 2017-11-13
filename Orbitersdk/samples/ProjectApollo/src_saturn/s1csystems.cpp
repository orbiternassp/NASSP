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

	ThrustTimer = 0.0;
}

void F1Engine::Timestep(double simdt)
{
	if (th_f1 == NULL) return;

	//Thrust OK switch
	ThrustOK = vessel->GetThrusterLevel(th_f1) > 0.9;

	if (ProgrammedCutoff || EDSCutoff || GSECutoff || RSSCutoff)
	{
		EngineStop = true;
	}
	else
	{
		EngineStop = false;
	}

	if (EngineStop)
	{
		double tm_1, tm_2, tm_3, ThrustLevel;

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

		vessel->SetThrusterLevel(th_f1, ThrustLevel);
	}
	else if (EngineStart && !EngineRunning)
	{
		double tm_1, tm_2, tm_3, tm_4, ThrustLevel;

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
}

void F1Engine::SetThrusterDir(double beta_y, double beta_p)
{
	if (th_f1 == NULL) return;

	VECTOR3 f1vector;

	if (beta_y > 5.16*RAD)
	{
		f1vector.x = 5.16*RAD;
	}
	else if (beta_y < -5.16*RAD)
	{
		f1vector.x = -5.16*RAD;
	}
	else
	{
		f1vector.x = beta_y;
	}

	if (beta_p > 5.16*RAD)
	{
		f1vector.y = 5.16*RAD;
	}
	else if (beta_p < -5.16*RAD)
	{
		f1vector.y = -5.16*RAD;
	}
	else
	{
		f1vector.y = beta_p;
	}

	f1vector.z = 1.0;

	vessel->SetThrusterDir(th_f1, f1vector);
}

SICSystems::SICSystems(Saturn *v, THRUSTER_HANDLE *f1, PROPELLANT_HANDLE &f1prop, Sound &LaunchS, Sound &SShutS) :
	main_propellant(f1prop),
	f1engine1(v, f1[2]),
	f1engine2(v, f1[1]),
	f1engine3(v, f1[3]),
	f1engine4(v, f1[0]),
	f1engine5(v, f1[4]),
	f1engines(f1),
	SShutSound(SShutS),
	LaunchSound(LaunchS)
{
	vessel = v;

	for (int i = 0;i < 5;i++)
	{
		ThrustOK[i] = false;
		EarlySICutoff[i] = false;
		FirstStageFailureTime[i] = 0.0;
	}

	MultipleEngineCutoffEnabled = false;
	PropellantDepletionSensors = false;
	PointLevelSensorArmed = false;
	TwoAdjacentOutboardEnginesOutCutoff = false;
}

void SICSystems::Timestep(double simdt)
{
	f1engine1.Timestep(simdt);
	f1engine2.Timestep(simdt);
	f1engine3.Timestep(simdt);
	f1engine4.Timestep(simdt);
	f1engine5.Timestep(simdt);

	//Thrust OK
	ThrustOK[0] = f1engine1.GetThrustOK();
	ThrustOK[1] = f1engine2.GetThrustOK();
	ThrustOK[2] = f1engine3.GetThrustOK();
	ThrustOK[3] = f1engine4.GetThrustOK();
	ThrustOK[4] = f1engine5.GetThrustOK();

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

	//Failure code

	for (int i = 0;i < 5;i++)
	{
		if (EarlySICutoff[i] && (vessel->GetMissionTime() > FirstStageFailureTime[i]) && vessel->GetThrusterResource(f1engines[i]) != NULL)
		{
			vessel->SetThrusterResource(f1engines[i], NULL);
		}
	}
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
	if (n == 1)
	{
		f1engine1.SetEngineStart();
	}
	else if (n == 2)
	{
		f1engine2.SetEngineStart();
	}
	else if (n == 3)
	{
		f1engine3.SetEngineStart();
	}
	else if (n == 4)
	{
		f1engine4.SetEngineStart();
	}
	else if (n == 5)
	{
		f1engine5.SetEngineStart();
	}
}

void SICSystems::SetEDSCutoff()
{
	f1engine1.SetEDSCutoff();
	f1engine2.SetEDSCutoff();
	f1engine3.SetEDSCutoff();
	f1engine4.SetEDSCutoff();
	f1engine5.SetEDSCutoff();
}

void SICSystems::SetThrusterDir(int n, double beta_y, double beta_p)
{
	if (n == 1)
	{
		f1engine1.SetThrusterDir(beta_y, beta_p);
	}
	else if (n == 2)
	{
		f1engine2.SetThrusterDir(beta_y, beta_p);
	}
	else if (n == 3)
	{
		f1engine3.SetThrusterDir(beta_y, beta_p);
	}
	else if (n == 4)
	{
		f1engine4.SetThrusterDir(beta_y, beta_p);
	}
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

void SICSystems::GetThrustOK(bool *ok)
{
	ok = ThrustOK;
}

void SICSystems::SetEngineFailureParameters(bool *SICut, double *SICutTimes)
{
	for (int i = 0;i < 5;i++)
	{
		EarlySICutoff[i] = SICut[i];
		FirstStageFailureTime[i] = SICutTimes[i];
	}
}

void SICSystems::SwitchSelector(int channel)
{
	switch (channel)
	{
	case 0: //Liftoff (NOT A REAL SWITCH SELECTOR EVENT)
		vessel->SetStage(LAUNCH_STAGE_ONE);								// Switch to stage one
		vessel->SetContrailLevel(1.0);
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
		if (vessel->GetStage() == LAUNCH_STAGE_ONE)
		{
			// Drop old stage
			vessel->SeparateStage(LAUNCH_STAGE_TWO);
			vessel->SetStage(LAUNCH_STAGE_TWO);
			vessel->ActivateStagingVent();
		}
		break;
		break;
	case 16: //Inboard Engine Cutoff Backup
		InboardEngineCutoff();
		break;
	case 17: //Two Adjacent Outboard Engines Out Cutoff Enable
		TwoAdjacentOutboardEnginesOutCutoffEnable();
		break;
	case 19: //S-IC/S-II Separation (No. 2)
		if (vessel->GetStage() == LAUNCH_STAGE_ONE)
		{
			// Drop old stage
			vessel->SeparateStage(LAUNCH_STAGE_TWO);
			vessel->SetStage(LAUNCH_STAGE_TWO);
			vessel->ActivateStagingVent();
		}
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

		for (int i = 0;i < 4;i++) if (!ThrustOK[i]) return true;
	}

	return false;
}

bool SICSystems::GetInboardEngineOut()
{
	if (!ThrustOK[4]) return true;

	return false;
}

bool SICSystems::GetOutboardEngineOut()
{
	for (int i=0;i<4;i++) if (!ThrustOK[4]) return true;

	return false;
}