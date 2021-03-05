/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007

  Environmental Control System

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
#include "LM_AscentStageResource.h"
#include "LM_VC_Resource.h"
#include "soundlib.h"
#include "toggleswitch.h"
#include "LEM.h"
#include "lm_ecs.h"

LEMCrewStatus::LEMCrewStatus(Sound &crewdeadsound) : crewDeadSound(crewdeadsound) {

	status = ECS_CREWSTATUS_OK;
	SuitPressureLowTime = 600;
	PressureLowTime = 600;
	SuitPressureHighTime = 3600;
	PressureHighTime = 3600;
	SuitTemperatureTime = 12 * 3600;
	TemperatureTime = 12 * 3600;
	CO2Time = 1800;
	accelerationTime = 10;
	lastVerticalVelocity = 0;

	lem = NULL;
	firstTimestepDone = false;
}

LEMCrewStatus::~LEMCrewStatus() {
}

void LEMCrewStatus::Init(LEM *s) {

	lem = s;
}

void LEMCrewStatus::Timestep(double simdt) {

	if (!firstTimestepDone) {
		// Dead at startup?
		if (status == ECS_CREWSTATUS_DEAD) {
			crewDeadSound.play();
		}
		firstTimestepDone = true;
	}

	if (!lem) return;

	LEMECSStatus ecs;
	lem->GetECSStatus(ecs);

	// Already dead?
	if (status == ECS_CREWSTATUS_DEAD) return;
	// No crew?
	if (!lem->Crewed || ecs.crewNumber == 0) return;

	status = ECS_CREWSTATUS_OK;

	// Suit/Cabin Pressure lower than 2.8 psi for 10 minutes
	if ((lem->ecs.GetECSSuitPSI() < 2.8) && (lem->CDRSuited->number + lem->LMPSuited->number > 0)) {
			if (SuitPressureLowTime <= 0) {
				status = ECS_CREWSTATUS_DEAD;
				crewDeadSound.play();
				return;
			} else {
				status = ECS_CREWSTATUS_CRITICAL;
				SuitPressureLowTime -= simdt;
			}
		} else {
		SuitPressureLowTime = 600;
	}

    if (lem->ecs.GetECSCabinPSI() < 2.8 && lem->CrewInCabin->number > 0) {
		if (PressureLowTime <= 0) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play();
			return;
		} else {
			status = ECS_CREWSTATUS_CRITICAL;
			PressureLowTime -= simdt;
		}
	} else {
		PressureLowTime = 600;
	}

	// Suit/Cabin Pressure higher than 22 psi for 1 hour
	if ((lem->ecs.GetECSSuitPSI() > 22) && (lem->CDRSuited->number + lem->LMPSuited->number > 0)) {
			if (SuitPressureHighTime <= 0) {
				status = ECS_CREWSTATUS_DEAD;
				crewDeadSound.play();
				return;
			} else {
				status = ECS_CREWSTATUS_CRITICAL;
				SuitPressureHighTime -= simdt;
			}
		} else {
		SuitPressureHighTime = 3600;
	}

	if (lem->ecs.GetECSCabinPSI() > 22 && lem->CrewInCabin->number > 0) {
		if (PressureHighTime <= 0) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play();
			return;
		} else {
			status = ECS_CREWSTATUS_CRITICAL;
			PressureHighTime -= simdt;
		}
	} else {
		PressureHighTime = 3600;
	}

	// Suit/Cabin temperature above about 45°C or below about 0°C for 12 hours
	if ((lem->ecs.GetSuitTempF() > 113 || lem->ecs.GetSuitTempF() < 32) && (lem->CDRSuited->number + lem->LMPSuited->number > 0)) {
			if (SuitTemperatureTime <= 0) {
				status = ECS_CREWSTATUS_DEAD;
				crewDeadSound.play();
				return;
			} else {
				status = ECS_CREWSTATUS_CRITICAL;
				SuitTemperatureTime -= simdt;
			}
		} else {
		SuitTemperatureTime = 12 * 3600;
	}
	// **Disabled for now until cabin temperatures are more stable
	/*if ((lem->ecs.GetCabinTempF() > 113 || lem->ecs.GetCabinTempF() < 32) && lem->CrewInCabin->number > 0) {
		if (TemperatureTime <= 0) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play();
			return;
		} else {
			status = ECS_CREWSTATUS_CRITICAL;
			TemperatureTime -= simdt;
		}
	} else {
		TemperatureTime = 12 * 3600;
	}*/

	// Suit/Cabin CO2 above 10 mmHg for 30 minutes
	if (lem->ecs.GetECSSensorCO2MMHg() > 10 && (lem->CrewInCabin->number > 0 || (lem->CDRSuited->number + lem->LMPSuited->number > 0))) {
		if (CO2Time <= 0) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play();
			return;
		}
		else {
			status = ECS_CREWSTATUS_CRITICAL;
			CO2Time -= simdt;
		}
	}
	else {
		CO2Time = 1800;
	}

	// Acceleration exceeds 12 g for 10 seconds
	VECTOR3 f, w;
	lem->GetForceVector(f);
	lem->GetWeightVector(w);
	if (length(f - w) / lem->GetMass() > 12. * G) {
		if (accelerationTime <= 0) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play();
			return;
		}
		else {
			status = ECS_CREWSTATUS_CRITICAL;
			accelerationTime -= simdt;
		}
	}
	else {
		accelerationTime = 10;
	}

	// Touchdown speed exceeds 15 m/s (= about 50 ft/s)
	if (lem->GroundContact()) {
		if (fabs(lastVerticalVelocity) > 15) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play();
			return;
		}
	}
	else {
		VECTOR3 v;
		lem->GetAirspeedVector(FRAME_HORIZON, v);
		lastVerticalVelocity = v.y;
	}
}

void LEMCrewStatus::LoadState(char *line) {

	sscanf(line + 10, "%d %lf %lf %lf %lf %lf %lf %lf %lf %lf", &status, &SuitPressureLowTime, &PressureLowTime, &SuitPressureHighTime,
		&PressureHighTime, &SuitTemperatureTime, &TemperatureTime, &CO2Time, &accelerationTime, &lastVerticalVelocity);
}

void LEMCrewStatus::SaveState(FILEHANDLE scn) {

	char buffer[1000];

	sprintf(buffer, "%d %lf %lf %lf %lf %lf %lf %lf %lf %lf", status, SuitPressureLowTime, PressureLowTime, SuitPressureHighTime,
		PressureHighTime, SuitTemperatureTime, TemperatureTime, CO2Time, accelerationTime, lastVerticalVelocity);
	oapiWriteScenario_string(scn, "CREWSTATUS", buffer);
}

LEMOverheadHatch::LEMOverheadHatch(Sound &opensound, Sound &closesound) :
	OpenSound(opensound), CloseSound(closesound)
{
	open = false;
	ovhdHatchHandle = NULL;
	lem = NULL;

	ovhdhatch_state.SetOperatingSpeed(0.2);
	anim_OvhdHatch = -1;
}

void LEMOverheadHatch::Init(LEM *l, ToggleSwitch *fhh)
{
	lem = l;
	ovhdHatchHandle = fhh;
}

void LEMOverheadHatch::DefineAnimations(UINT idx)
{
	// Overhead Hatch Animations
	ANIMATIONCOMPONENT_HANDLE ach_OvhdHatch;

	static UINT	meshgroup_OvhdHatch = AS_GRP_UpperHatch;
	static MGROUP_ROTATE mgt_OvhdHatch(idx, &meshgroup_OvhdHatch, 1, _V(0.00, 1.02873, -0.40544), _V(-1.0, 0.0, 0.0), (float)(-90.0*RAD));

	anim_OvhdHatch = lem->CreateAnimation(0.0);
	ach_OvhdHatch = lem->AddAnimationComponent(anim_OvhdHatch, 0.0f, 1.0f, &mgt_OvhdHatch);

	lem->SetAnimation(anim_OvhdHatch, ovhdhatch_state.State());
}

void LEMOverheadHatch::DefineAnimationsVC(UINT idx)
{
	// Overhead Hatch Animations
	ANIMATIONCOMPONENT_HANDLE ach_OvhdHatchVC;

	static UINT	meshgroup_OvhdHatchVC = VC_GRP_UpperHatch;
	static MGROUP_ROTATE mgt_OvhdHatchVC(idx, &meshgroup_OvhdHatchVC, 1, _V(0.00, 1.02873, -0.40544), _V(-1.0, 0.0, 0.0), (float)(-90.0*RAD));

	anim_OvhdHatchVC = lem->CreateAnimation(0.0);
	ach_OvhdHatchVC = lem->AddAnimationComponent(anim_OvhdHatchVC, 0.0f, 1.0f, &mgt_OvhdHatchVC);

	lem->SetAnimation(anim_OvhdHatchVC, ovhdhatch_state.State());
}

void LEMOverheadHatch::Timestep(double simdt)
{
	if (ovhdhatch_state.Process(simdt)) {
		lem->SetAnimation(anim_OvhdHatch, ovhdhatch_state.State());
		lem->SetAnimation(anim_OvhdHatchVC, ovhdhatch_state.State());
	}
}

void LEMOverheadHatch::Toggle()
{
	if (open == false)
	{
		if (ovhdHatchHandle->GetState() == 1)
		{
			open = true;
			OpenSound.play();
			lem->PanelRefreshOverheadHatch();
			ovhdhatch_state.Open();
			lem->DrogueVis();
		}
	}
	else
	{
		open = false;
		CloseSound.play();
		lem->PanelRefreshOverheadHatch();
		ovhdhatch_state.Close();
		lem->DrogueVis();
	}
}

void LEMOverheadHatch::LoadState(char *line) {

	int i1;
	double a, b;

	sscanf(line + 13, "%d %lf %lf", &i1, &a, &b);
	open = (i1 != 0);
	ovhdhatch_state.SetState(a, b);
}

void LEMOverheadHatch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i %lf %lf", (open ? 1 : 0), ovhdhatch_state.State(), ovhdhatch_state.Speed());
	oapiWriteScenario_string(scn, "OVERHEADHATCH", buffer);
}

LEMOVHDCabinReliefDumpValve::LEMOVHDCabinReliefDumpValve()
{
	cabinOVHDHatchValve = NULL;
	cabinOVHDHatchValveSwitch = NULL;
	ovhdHatch = NULL;
}

void LEMOVHDCabinReliefDumpValve::Init(h_Pipe *cohv, ThreePosSwitch *cohs, LEMOverheadHatch *oh)
{
	cabinOVHDHatchValve = cohv;
	cabinOVHDHatchValveSwitch = cohs;
	ovhdHatch = oh;
}

void LEMOVHDCabinReliefDumpValve::SystemTimestep(double simdt)
{
	if (!cabinOVHDHatchValve) return;

	// Valve in motion
	if (cabinOVHDHatchValve->in->pz) return;

	if (ovhdHatch->IsOpen())
	{
		cabinOVHDHatchValve->in->Open();
		cabinOVHDHatchValve->in->size = (float) 1000.0;	// no pressure in a few seconds
		cabinOVHDHatchValve->flowMax = 2000.0 / LBH;
	}
	else
	{
		cabinOVHDHatchValve->in->size = (float) 10.0;
		//DUMP
		if (cabinOVHDHatchValveSwitch->GetState() == 0)
		{
			cabinOVHDHatchValve->in->size = (float) 10.0;
			cabinOVHDHatchValve->flowMax = 666.0 / LBH;
			cabinOVHDHatchValve->in->Open();
		}
		//CLOSE
		else if (cabinOVHDHatchValveSwitch->GetState() == 2)
		{
			cabinOVHDHatchValve->in->Close();
		}
		//AUTO
		else if (cabinOVHDHatchValveSwitch->GetState() == 1)
		{
			double cabinpress = cabinOVHDHatchValve->in->parent->space.Press;

			if (cabinpress > 5.4 / PSI && cabinOVHDHatchValve->in->open == 0)
			{
				cabinOVHDHatchValve->in->Open();
			}
			else if (cabinpress < 5.25 / PSI && cabinOVHDHatchValve->in->open == 1)
			{
				cabinOVHDHatchValve->in->Close();
			}

			if (cabinOVHDHatchValve->in->open == 1)
			{
				if (cabinpress > 5.8 / PSI)
				{
					cabinOVHDHatchValve->flowMax = 666.0 / LBH;
				}
				else
				{
					//0 flow at 5.25 psi, full flow at 5.8 psi
					cabinOVHDHatchValve->flowMax = max(0.0001, (660.0 / LBH) * (1.81818*(cabinpress*PSI) - 9.54545));
				}
			}
		}
	}
}


LEMForwardHatch::LEMForwardHatch(Sound &opensound, Sound &closesound) :
	OpenSound(opensound), CloseSound(closesound)
{
	open = false;
	ForwardHatchHandle = NULL;
	lem = NULL;

	hatch_state.SetOperatingSpeed(0.2);
	anim_Hatch = -1;
}

void LEMForwardHatch::Init(LEM *l, ToggleSwitch *fhh)
{
	lem = l;
	ForwardHatchHandle = fhh;
}

void LEMForwardHatch::DefineAnimations(UINT idx)
{
	// Forward Hatch Animation
	ANIMATIONCOMPONENT_HANDLE	ach_Hatch;
	static UINT	meshgroup_Hatch = AS_GRP_FwdHatch;
	static MGROUP_ROTATE	mgt_Hatch(idx, &meshgroup_Hatch, 1, _V(0.39366, -0.57839, 1.63386), _V(0.0, 1.0, 0.0), (float)(-85.0*RAD));
	anim_Hatch = lem->CreateAnimation(0.0);
	ach_Hatch = lem->AddAnimationComponent(anim_Hatch, 0.0f, 1.0f, &mgt_Hatch);
	lem->SetAnimation(anim_Hatch, hatch_state.State());
}

void LEMForwardHatch::DefineAnimationsVC(UINT idx)
{
	// Forward Hatch Animation
	ANIMATIONCOMPONENT_HANDLE	ach_HatchVC;
	static UINT	meshgroup_HatchVC = VC_GRP_FwdHatch;
	static MGROUP_ROTATE	mgt_HatchVC(idx, &meshgroup_HatchVC, 1, _V(0.39366, -0.57839, 1.63386), _V(0.0, 1.0, 0.0), (float)(-85.0*RAD));
	anim_HatchVC = lem->CreateAnimation(0.0);
	ach_HatchVC = lem->AddAnimationComponent(anim_HatchVC, 0.0f, 1.0f, &mgt_HatchVC);
	lem->SetAnimation(anim_HatchVC, hatch_state.State());
}

void LEMForwardHatch::Timestep(double simdt)
{
	if (hatch_state.Process(simdt)) {
		lem->SetAnimation(anim_Hatch, hatch_state.State());
		lem->SetAnimation(anim_HatchVC, hatch_state.State());
	}
}

void LEMForwardHatch::Toggle()
{
	if (open == false)
	{
		if (ForwardHatchHandle->GetState() == 1)
		{
			open = true;
			OpenSound.play();
			lem->PanelRefreshForwardHatch();
			hatch_state.Open();
		}
	}
	else
	{
		open = false;
		CloseSound.play();
		lem->PanelRefreshForwardHatch();
		hatch_state.Close();
	}
}

void LEMForwardHatch::LoadState(char *line) {

	int i1;
	double a, b;

	sscanf(line + 13, "%d %lf %lf", &i1, &a, &b);
	open = (i1 != 0);
	hatch_state.SetState(a, b);
}

void LEMForwardHatch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i %lf %lf", (open ? 1 : 0), hatch_state.State(), hatch_state.Speed());
	oapiWriteScenario_string(scn, "FORWARDHATCH", buffer);
}

LEMFWDCabinReliefDumpValve::LEMFWDCabinReliefDumpValve()
{
	cabinFWDHatchValve = NULL;
	cabinFWDHatchValveSwitch = NULL;
	fwdHatch = NULL;
}

void LEMFWDCabinReliefDumpValve::Init(h_Pipe *cfv, ThreePosSwitch *cfvs, LEMForwardHatch *fh)
{
	cabinFWDHatchValve = cfv;
	cabinFWDHatchValveSwitch = cfvs;
	fwdHatch = fh;
}

void LEMFWDCabinReliefDumpValve::SystemTimestep(double simdt)
{
	if (!cabinFWDHatchValve) return;

	// Valve in motion
	if (cabinFWDHatchValve->in->pz) return;

	if (fwdHatch->IsOpen())
	{
		cabinFWDHatchValve->in->Open();
		cabinFWDHatchValve->in->size = (float) 1000.0;	// no pressure in a few seconds
		cabinFWDHatchValve->flowMax = 2000.0 / LBH;
	}
	else
	{
		cabinFWDHatchValve->in->size = (float) 10.;

		//DUMP
		if (cabinFWDHatchValveSwitch->GetState() == 0)
		{
			cabinFWDHatchValve->in->size = (float) 35.0;
			cabinFWDHatchValve->flowMax = 666.0 / LBH;
			cabinFWDHatchValve->in->Open();
		}
		//CLOSE
		else if (cabinFWDHatchValveSwitch->GetState() == 2)
		{
			cabinFWDHatchValve->in->Close();
		}
		//AUTO
		else if (cabinFWDHatchValveSwitch->GetState() == 1)
		{
			double cabinpress = cabinFWDHatchValve->in->parent->space.Press;

			if (cabinpress > 5.4 / PSI && cabinFWDHatchValve->in->open == 0)
			{
				cabinFWDHatchValve->in->Open();
			}
			else if (cabinpress < 5.25 / PSI && cabinFWDHatchValve->in->open == 1)
			{
				cabinFWDHatchValve->in->Close();
			}

			if (cabinFWDHatchValve->in->open == 1)
			{
				if (cabinpress > 5.8 / PSI)
				{
					cabinFWDHatchValve->flowMax = 666.0 / LBH;
				}
				else
				{
					//0 flow at 5.25 psi, full flow at 5.8 psi
					cabinFWDHatchValve->flowMax = max(0.0001, (660.0 / LBH) * (1.81818*(cabinpress*PSI) - 9.54545));
				}
			}
		}
	}
}

LEMSuitCircuitReliefValve::LEMSuitCircuitReliefValve()
{
	SuitCircuitReliefValve = NULL;
	SuitCircuitReliefValveSwitch = NULL;
}

void LEMSuitCircuitReliefValve::Init(h_Pipe *scrv, RotationalSwitch *scrvs)
{
	SuitCircuitReliefValve = scrv;
	SuitCircuitReliefValveSwitch = scrvs;
}

void LEMSuitCircuitReliefValve::SystemTimestep(double simdt)
{
	if (!SuitCircuitReliefValve) return;

	// Valve in motion
	if (SuitCircuitReliefValve->in->pz) return;

	//OPEN
	if (SuitCircuitReliefValveSwitch->GetState() == 0)
	{
		SuitCircuitReliefValve->flowMax = 7.8 / LBH;
		SuitCircuitReliefValve->in->Open();
	}
	//CLOSE
	else if (SuitCircuitReliefValveSwitch->GetState() == 2)
	{
		SuitCircuitReliefValve->in->Close();
	}
	//AUTO
	else if (SuitCircuitReliefValveSwitch->GetState() == 1)
	{
		double suitcircuitpress = SuitCircuitReliefValve->in->parent->space.Press;

		if (suitcircuitpress > 4.3 / PSI)
		{
			SuitCircuitReliefValve->in->Open();
		}
		else
		{
			SuitCircuitReliefValve->in->Close();
		}

		if (SuitCircuitReliefValve->in->open == 1)
		{
			if (suitcircuitpress > 4.7 / PSI)
			{
				SuitCircuitReliefValve->flowMax = 7.8 / LBH;
			}
			else
			{
				//0 flow at 4.3 psi, full flow at 4.7 psi
				SuitCircuitReliefValve->flowMax = max(0.0001, (7.8 / LBH) * (2.5*(suitcircuitpress*PSI) - 10.75));
			}
		}
	}
}

LEMPressureSwitch::LEMPressureSwitch()
{
	switchtank = NULL;
	maxpress = 0;
	minpress = 0;
	PressureSwitch = true;

}

void LEMPressureSwitch::Init(h_Tank *st, double max, double min)
{
	switchtank = st;
	maxpress = max;
	minpress = min;
}

void LEMPressureSwitch::SystemTimestep(double simdt)
{
	double press = switchtank->space.Press;

	if (press < minpress)
	{
		PressureSwitch = true;
	}
	else if (press > maxpress)
	{
		PressureSwitch = false;
	}
}

void LEMPressureSwitch::LoadState(char *line, int strlen)
{
	int i;

	sscanf(line + strlen + 1, "%i", &i);

	PressureSwitch = (i != 0);
}

void LEMPressureSwitch::SaveState(FILEHANDLE scn, char *name_str)
{
	char buffer[100];

	sprintf(buffer, "%d", PressureSwitch);
	oapiWriteScenario_string(scn, name_str, buffer);
}

LEMSuitIsolValve::LEMSuitIsolValve()
{
	lem = NULL;
	suitisolvlv = NULL;
	actuatorovrdswitch = NULL;

}

void LEMSuitIsolValve::Init(LEM *l, RotationalSwitch *scv, ToggleSwitch *ovrd)
{
	lem = l;
	suitisolvlv = scv;
	actuatorovrdswitch = ovrd;
}

void LEMSuitIsolValve::SystemTimestep(double simdt)
{
	if (!suitisolvlv) return;

	//Pressure Switch/Override Actuation (Suit Disconnect)
	if (suitisolvlv->GetState() == 0 && lem->ECS_SUIT_FLOW_CONT_CB.IsPowered() && (actuatorovrdswitch->GetState() == 1 || lem->SuitPressureSwitch.GetPressureSwitch() != 0))
	{
		suitisolvlv->SwitchTo(1); //Suit Disconnect
	}
}


LEMCabinRepressValve::LEMCabinRepressValve()
{
	lem = NULL;
	cabinRepressValve = NULL;
	cabinRepressValveSwitch = NULL;
	cabinRepressCB = NULL;
	pressRegulatorASwitch = NULL;
	pressRegulatorBSwitch = NULL;
	EmergencyCabinRepressRelay = false;
}

void LEMCabinRepressValve::Init(LEM *l, h_Pipe *crv, CircuitBrakerSwitch *crcb, RotationalSwitch *crvs, RotationalSwitch* pras, RotationalSwitch *prbs)
{
	lem = l;
	cabinRepressValve = crv;
	cabinRepressValveSwitch = crvs;
	cabinRepressCB = crcb;
	pressRegulatorASwitch = pras;
	pressRegulatorBSwitch = prbs;
}

void LEMCabinRepressValve::SystemTimestep(double simdt)
{
	if (!cabinRepressValve) return;

	// Valve in motion
	if (cabinRepressValve->in->pz) return;

	EmergencyCabinRepressRelay = false;

	//MANUAL
	if (cabinRepressValveSwitch->GetState() == 0)
	{
		cabinRepressValve->flowMax = 480.0 / LBH;
		cabinRepressValve->in->Open();
	}
	//CLOSE
	else if (cabinRepressValveSwitch->GetState() == 2)
	{
		cabinRepressValve->in->Close();
	}
	//AUTO
	else if (cabinRepressValveSwitch->GetState() == 1)
	{
		cabinRepressValve->flowMax = 396.0 / LBH;

		bool repressinhibit = false;

		//Both in EGRESS
		if (pressRegulatorASwitch->GetState() == 0 && pressRegulatorBSwitch->GetState() == 0) repressinhibit = true;
		//One in EGRESS, other in CLOSE
		else if (pressRegulatorASwitch->GetState() == 0 && pressRegulatorBSwitch->GetState() == 3) repressinhibit = true;
		else if (pressRegulatorASwitch->GetState() == 3 && pressRegulatorBSwitch->GetState() == 0) repressinhibit = true;

		if (cabinRepressCB->IsPowered() && repressinhibit == false)
		{
			if (lem->CabinPressureSwitch.GetPressureSwitch() != 0 && cabinRepressValve->in->open == 0)
			{
				cabinRepressValve->in->Open();
			}
			else if (lem->CabinPressureSwitch.GetPressureSwitch() == 0 && cabinRepressValve->in->open == 1)
			{
				cabinRepressValve->in->Close();
			}
			if (cabinRepressValve->in->open)
			{
				EmergencyCabinRepressRelay = true;
			}
		}
		else
		{
			cabinRepressValve->in->Close();
		}
	}
}

LEMSuitCircuitPressureRegulator::LEMSuitCircuitPressureRegulator()
{
	pressRegulatorSwitch = NULL;
	pressRegulatorValve = NULL;
	suitCircuit = NULL;
}

void LEMSuitCircuitPressureRegulator::Init(h_Pipe *prv, h_Tank *sc, RotationalSwitch *prs)
{
	pressRegulatorSwitch = prs;
	pressRegulatorValve = prv;
	suitCircuit = sc;
}

void LEMSuitCircuitPressureRegulator::SystemTimestep(double simdt)
{
	
	if (!pressRegulatorValve) return;

	// Valve in motion
	if (pressRegulatorValve->in->pz) return;

	//Suit pressure
	double suitpress = suitCircuit->space.Press;

	//DIRECT O2
	if (pressRegulatorSwitch->GetState() == 2)
	{
		pressRegulatorValve->in->Open();
	}
	//CLOSE
	else if (pressRegulatorSwitch->GetState() == 3)
	{
		pressRegulatorValve->in->Close();
	}
	//EGRESS
	else if (pressRegulatorSwitch->GetState() == 0)
	{
		if (suitpress < 3.8 / PSI)
		{
			pressRegulatorValve->in->Open();
		}
		else
		{
			pressRegulatorValve->in->Close();
		}
	}
	//CABIN
	else if (pressRegulatorSwitch->GetState() == 1)
	{
		if (suitpress < 4.8 / PSI)
		{
			pressRegulatorValve->in->Open();
		}
		else
		{
			pressRegulatorValve->in->Close();
		}
	}
}

LEMSuitGasDiverter::LEMSuitGasDiverter()
{
	suitGasDiverterValve = NULL;
	cabin = NULL;
	suitGasDiverterSwitch = NULL;
	DivertVLVCB = NULL;
	pressRegulatorASwitch = NULL;
	pressRegulatorBSwitch = NULL;
}

void LEMSuitGasDiverter::Init(h_Tank *sgdv, h_Tank *cab, CircuitBrakerSwitch *sgds, CircuitBrakerSwitch *dvcb, RotationalSwitch* pras, RotationalSwitch *prbs)
{
	suitGasDiverterValve = sgdv;
	cabin = cab;
	suitGasDiverterSwitch = sgds;
	DivertVLVCB = dvcb;
	pressRegulatorASwitch = pras;
	pressRegulatorBSwitch = prbs;
}

void LEMSuitGasDiverter::SystemTimestep(double simdt)
{
	if (!suitGasDiverterValve) return;

	// Valve in motion
	if (suitGasDiverterValve->OUT_valve.pz) return;
	if (suitGasDiverterValve->OUT2_valve.pz) return;

	//Solenoid
	if (suitGasDiverterSwitch->GetState() == 1 && DivertVLVCB->IsPowered())
	{
		//Cabin pressure
		double cabinpress = cabin->space.Press;

		if (cabinpress < 4.0 / PSI || pressRegulatorASwitch->GetState() == 0 || pressRegulatorBSwitch->GetState() == 0)
		{
			suitGasDiverterSwitch->SwitchTo(0);
		}
	}

	//EGRESS
	if (suitGasDiverterSwitch->GetState() == 0)
	{
		suitGasDiverterValve->OUT_valve.Close();
		suitGasDiverterValve->OUT2_valve.Open();
	}
	//CABIN
	else
	{
		suitGasDiverterValve->OUT_valve.Open();
		suitGasDiverterValve->OUT2_valve.Close();
	}
}

LEMCO2CanisterSelect::LEMCO2CanisterSelect()
{
	PrimCO2Canister = NULL;
	SecCO2Canister = NULL;
	CO2CanisterSelectSwitch = NULL;
}

void LEMCO2CanisterSelect::Init(h_Tank *pco2, h_Tank *sco2, ToggleSwitch* co2s)
{
	PrimCO2Canister = pco2;
	SecCO2Canister = sco2;
	CO2CanisterSelectSwitch = co2s;
}

void LEMCO2CanisterSelect::SystemTimestep(double simdt)
{

	if (!PrimCO2Canister) return;

	if (!SecCO2Canister) return;

	// Valve in motion

	if (PrimCO2Canister->IN_valve.pz) return;

	//PRIM
	if (CO2CanisterSelectSwitch->GetState() == 1)
	{
		PrimCO2Canister->IN_valve.Open();
		PrimCO2Canister->OUT_valve.Open();
		SecCO2Canister->IN_valve.Close();
		SecCO2Canister->OUT_valve.Close();
	}
	//SEC
	else
	{
		PrimCO2Canister->IN_valve.Close();
		PrimCO2Canister->OUT_valve.Close();
		SecCO2Canister->IN_valve.Open();
		SecCO2Canister->OUT_valve.Open();
	}
}

LEMCO2CanisterVent::LEMCO2CanisterVent()
{
	CO2Canister = NULL;
	CO2CanisterVentSwitch = NULL;
}

void LEMCO2CanisterVent::Init(h_Tank *co2c, PushSwitch *co2vs)
{
	CO2Canister = co2c;
	CO2CanisterVentSwitch = co2vs;
}

void LEMCO2CanisterVent::SystemTimestep(double simdt)
{

	if (!CO2Canister) return;

	if (!CO2CanisterVentSwitch) return;

	// Valve in motion
	if (CO2Canister->OUT2_valve.pz) return;

	if (CO2CanisterVentSwitch->GetState() == 1)
	{
		CO2Canister->OUT2_valve.Open();
	}
	else
	{
		CO2Canister->OUT2_valve.Close();
	}
}

LEMCabinGasReturnValve::LEMCabinGasReturnValve()
{
	cabinGasReturnValve = NULL;
	cabinGasReturnValveSwitch = NULL;
}

void LEMCabinGasReturnValve::Init(h_Pipe * cgrv, RotationalSwitch *cgrvs)
{
	cabinGasReturnValve = cgrv;
	cabinGasReturnValveSwitch = cgrvs;
}

void LEMCabinGasReturnValve::SystemTimestep(double simdt)
{
	if (!cabinGasReturnValve) return;

	// Valve in motion
	if (cabinGasReturnValve->in->pz) return;

	//OPEN
	if (cabinGasReturnValveSwitch->GetState() == 0)
	{
		cabinGasReturnValve->in->Open();
	}
	//EGRESS
	else if (cabinGasReturnValveSwitch->GetState() == 2)
	{
		cabinGasReturnValve->in->Close();
	}
	//AUTO
	else if (cabinGasReturnValveSwitch->GetState() == 1)
	{
		if (cabinGasReturnValve->out->parent->space.Press > cabinGasReturnValve->in->parent->space.Press)
		{
			cabinGasReturnValve->in->Open();
		}
		else
		{
			cabinGasReturnValve->in->Close();
		}
	}
}

LEMWaterSeparationSelector::LEMWaterSeparationSelector()
{
	WaterSeparationSelectorValve = NULL;
	WaterSeparationSelectorSwitch = NULL;
}

void LEMWaterSeparationSelector::Init(h_Tank *wssv, CircuitBrakerSwitch* wsss)
{
	WaterSeparationSelectorValve = wssv;
	WaterSeparationSelectorSwitch = wsss;
}

void LEMWaterSeparationSelector::SystemTimestep(double simdt)
{
	if (!WaterSeparationSelectorValve) return;

	// Valve in motion
	if (WaterSeparationSelectorValve->OUT_valve.pz) return;
	if (WaterSeparationSelectorValve->OUT2_valve.pz) return;

	//SEP1
	if (WaterSeparationSelectorSwitch->GetState())
	{
		WaterSeparationSelectorValve->OUT_valve.Open();
		WaterSeparationSelectorValve->OUT2_valve.Close();
	}
	//SEP2
	else
	{
		WaterSeparationSelectorValve->OUT_valve.Close();
		WaterSeparationSelectorValve->OUT2_valve.Open();
	}
}

LEMCabinFan::LEMCabinFan(FadeInOutSound &cabinfanS) : cabinfansound(cabinfanS)
{
	cabinFan1CB = NULL;
	cabinFanContCB = NULL;
	pressRegulatorASwitch = NULL;
	pressRegulatorBSwitch = NULL;
	cabinFan = NULL;
	cabinFanHeat = 0;
}

void LEMCabinFan::Init(CircuitBrakerSwitch *cf1cb, CircuitBrakerSwitch *cfccb, RotationalSwitch *pras, RotationalSwitch *prbs, Pump *cf, h_HeatLoad *cfh)
{
	cabinFan1CB = cf1cb;
	cabinFanContCB = cfccb;
	pressRegulatorASwitch = pras;
	pressRegulatorBSwitch = prbs;
	cabinFan = cf;
	cabinFanHeat = cfh;
}

void LEMCabinFan::SystemTimestep(double simdt)
{
	bool cabinFanSwitch;

	if (cabinFanContCB->IsPowered() && (pressRegulatorASwitch->GetState() == 0 || pressRegulatorBSwitch->GetState() == 0))
	{
		cabinFanContCB->DrawPower(1.1);
		cabinFanSwitch = true;
	}
	else
	{
		cabinFanSwitch = false;
	}

	if (cabinFan1CB->IsPowered() && !cabinFanSwitch)
	{
		cabinFan->SetPumpOn();
		CabinFanSound();
	}
	else
	{
		cabinFan->SetPumpOff();
		StopCabinFanSound();
	}

	if (cabinFan->pumping) {
		//cabinFanHeat->GenerateHeat(36.5);  //Not sure about this heat load, seems very high
		cabinFanHeat->GenerateHeat(18.2);  //Testing lower heat
	}
}

void LEMCabinFan::CabinFanSound()
{
	cabinfansound.play(200);
}

void LEMCabinFan::StopCabinFanSound()
{
	cabinfansound.stop();
}

LEMWaterTankSelect::LEMWaterTankSelect()
{
	WaterTankSelect = NULL;
	WaterTankSelectSwitch = NULL;
	SurgeTank = NULL;
}

void LEMWaterTankSelect::Init(h_Tank* wts, h_Tank *st, RotationalSwitch *wtss)
{
	WaterTankSelect = wts;
	WaterTankSelectSwitch = wtss;
	SurgeTank = st;
}

void LEMWaterTankSelect::SystemTimestep(double simdt)
{
	if (!WaterTankSelect) return;
	if (!SurgeTank) return;

	// Valve in motion
	if (WaterTankSelect->IN_valve.pz) return;
	if (SurgeTank->OUT2_valve.pz) return;

	//DES
	if (WaterTankSelectSwitch->GetState() == 0)
	{
		WaterTankSelect->IN_valve.Open();
		WaterTankSelect->OUT_valve.Open();
		WaterTankSelect->OUT2_valve.Close();
		SurgeTank->OUT_valve.Open();
		SurgeTank->OUT2_valve.Close();
	}
	//ASC
	else if (WaterTankSelectSwitch->GetState() == 1)
	{
		WaterTankSelect->IN_valve.Close();
		WaterTankSelect->OUT_valve.Open();
		WaterTankSelect->OUT2_valve.Open();
		SurgeTank->OUT_valve.Open();
		SurgeTank->OUT2_valve.Close();
	}
	//SEC
	else if (WaterTankSelectSwitch->GetState() == 2)
	{
		WaterTankSelect->IN_valve.Close();
		WaterTankSelect->OUT_valve.Close();
		WaterTankSelect->OUT2_valve.Open();
		SurgeTank->OUT_valve.Close();
		SurgeTank->OUT2_valve.Open();
	}
}

LEMPrimGlycolPumpController::LEMPrimGlycolPumpController()
{
	primGlycolAccumulatorTank = NULL;
	primGlycolPumpManifoldTank = NULL;
	glycolPump1CB = NULL;
	glycolPump2CB = NULL;
	glycolPumpAutoTransferCB = NULL;
	glycolRotary = NULL;
	glycolPump1 = NULL;
	glycolPump2 = NULL;
	glycolPump1Heat = 0;
	glycolPump2Heat = 0;

	GlycolAutoTransferRelay = false;
	GlycolPumpFailRelay = false;
	PressureSwitch = true;
	AutoTransferCounter = 0;
}

void LEMPrimGlycolPumpController::Init(h_Tank *pgat, h_Tank *pgpmt, Pump *gp1, Pump *gp2, RotationalSwitch *gr, CircuitBrakerSwitch *gp1cb, CircuitBrakerSwitch *gp2cb, CircuitBrakerSwitch *gpatcb, h_HeatLoad *gp1h, h_HeatLoad *gp2h)
{
	primGlycolAccumulatorTank = pgat;
	primGlycolPumpManifoldTank = pgpmt;
	glycolPump1 = gp1;
	glycolPump2 = gp2;
	glycolRotary = gr;
	glycolPump1CB = gp1cb;
	glycolPump2CB = gp2cb;
	glycolPumpAutoTransferCB = gpatcb;
	glycolPump1Heat = gp1h;
	glycolPump2Heat = gp2h;
}

void LEMPrimGlycolPumpController::SystemTimestep(double simdt)
{
	if (!primGlycolPumpManifoldTank || !primGlycolAccumulatorTank) return;

	double DPSensor = primGlycolPumpManifoldTank->space.Press - primGlycolAccumulatorTank->space.Press;

	if (PressureSwitch == false && DPSensor < 3.0 / PSI)
	{
		PressureSwitch = true;
	}
	else if (PressureSwitch == true && DPSensor > 7.0 / PSI)
	{
		PressureSwitch = false;
	}

	if (PressureSwitch && glycolRotary->GetState() == 1 && glycolPumpAutoTransferCB->IsPowered())
	{
		//To make this more stable with time acceleration and panel changes
		if (AutoTransferCounter > 20)
		{
			GlycolAutoTransferRelay = true;
		}
		else
		{
			AutoTransferCounter++;
		}
	}
	else if (glycolRotary->GetState() == 2 && glycolPumpAutoTransferCB->IsPowered())
	{
		GlycolAutoTransferRelay = false;
		AutoTransferCounter = 0;
	}
	else
	{
		AutoTransferCounter = 0;
	}

	if (GlycolAutoTransferRelay && glycolRotary->GetState() == 1 && glycolPump1CB->IsPowered())
	{
		GlycolPumpFailRelay = true;
	}
	else
	{
		GlycolPumpFailRelay = false;
	}

	//PUMP 1
	if (glycolRotary->GetState() == 1 && !GlycolAutoTransferRelay && glycolPump1CB->IsPowered())
	{
		glycolPump1->SetPumpOn();
		glycolPump1Heat->GenerateHeat(30.5);
	}
	else
	{
		glycolPump1->SetPumpOff();
	}

	//PUMP 2
	if ((glycolRotary->GetState() == 2 || GlycolAutoTransferRelay) && glycolPump2CB->IsPowered())
	{
		glycolPump2->SetPumpOn();
	}
	else
	{
		glycolPump2->SetPumpOff();
	}

	if (glycolPump1->pumping) {
		glycolPump1Heat->GenerateHeat(30.5);
	}

	if (glycolPump2->pumping) {
		glycolPump2Heat->GenerateHeat(30.5);
	}

	//sprintf(oapiDebugString(), "DP %f DPSwitch %d ATRelay %d Pump1 %d Pump2 %d", DPSensor*PSI, PressureSwitch, GlycolAutoTransferRelay, glycolPump1->h_pump, glycolPump2->h_pump);
}

bool LEMPrimGlycolPumpController::GetGlycolPumpState(int i) {

	if (i == 1 && glycolPump1->pumping) {
		return true;
	}

	if (i == 2 && glycolPump2->pumping) {
		return true;
	}
	return false;
}

void LEMPrimGlycolPumpController::LoadState(char *line)
{
	int i, j, k;

	sscanf(line + 21, "%i %i %i", &i, &j, &k);

	PressureSwitch = (i != 0);
	GlycolAutoTransferRelay = (j != 0);
	GlycolPumpFailRelay = (k != 0);
}

void LEMPrimGlycolPumpController::SaveState(FILEHANDLE scn)
{
	char buffer[100];

	sprintf(buffer, "%d %d %d", PressureSwitch, GlycolAutoTransferRelay, GlycolPumpFailRelay);
	oapiWriteScenario_string(scn, "PRIMGLYPUMPCONTROLLER", buffer);
}

LEMSuitFanDPSensor::LEMSuitFanDPSensor()
{
	suitFanManifoldTank = NULL;
	suitCircuitHeatExchangerCoolingTank = NULL;
	suitFanDPCB = NULL;
	SuitFanFailRelay = false;
	PressureSwitch = false;
}

void LEMSuitFanDPSensor::Init(h_Tank *sfmt, h_Tank *schect, CircuitBrakerSwitch *sfdpcb)
{
	suitFanManifoldTank = sfmt;
	suitCircuitHeatExchangerCoolingTank = schect;
	suitFanDPCB = sfdpcb;
}

void LEMSuitFanDPSensor::SystemTimestep(double simdt)
{
	if (!suitFanManifoldTank || !suitCircuitHeatExchangerCoolingTank) return;

	double DPSensor = suitCircuitHeatExchangerCoolingTank->space.Press - suitFanManifoldTank->space.Press;

	if (PressureSwitch == false && DPSensor <=  6.0 / INH2O)
	{
		PressureSwitch = true;
	}
	else if (PressureSwitch == true && DPSensor >= 8.0 / INH2O)
	{
		PressureSwitch = false;
	}

	if (PressureSwitch && suitFanDPCB->IsPowered())
	{
		SuitFanFailRelay = true;
	}
	else
	{
		SuitFanFailRelay = false;
	}
}

void LEMSuitFanDPSensor::LoadState(char *line)
{
	int i, j;

	sscanf(line + 15, "%i %i", &i, &j);

	PressureSwitch = (i != 0);
	SuitFanFailRelay = (j != 0);
}

void LEMSuitFanDPSensor::SaveState(FILEHANDLE scn)
{
	char buffer[100];

	sprintf(buffer, "%d %d", PressureSwitch, SuitFanFailRelay);
	oapiWriteScenario_string(scn, "SUITFANDPSENSOR", buffer);
}

LEM_ECS::LEM_ECS(PanelSDK &p) : sdk(p)
{
	lem = NULL;
	// Initialize
	Asc_Oxygen1 = 0;
	Asc_Oxygen2 = 0;
	Des_Oxygen = 0;
	Des_OxygenPress = 0;
	Asc_Oxygen1Press = 0;
	Asc_Oxygen2Press = 0;
	//Des_Oxygen2 = 0; Using LM-8 Systems Handbook, only 1 DES O2 tank
	Asc_Water1 = 0;
	Asc_Water2 = 0;
	Des_Water = 0;
	Des_Water_Press = 0;
	//Des_Water2 = 0; Using LM-8 Systems Handbook, only 1 DES H2O tank
	Primary_CL_Glycol_Press = 0;						// Zero this, system will fill from accu
	Secondary_CL_Glycol_Press = 0;						// Zero this, system will fill from accu
	Primary_CL_Glycol_Temp = 0;							// 40 in the accu, 0 other side of the pump
	Secondary_CL_Glycol_Temp = 0;						// 40 in the accu, 0 other side of the pump
	Primary_Glycol_Accu = 0;							// Glycol Accumulator mass
	Primary_Glycol_Pump_Manifold = 0;					// Pump manifold mass
	Primary_Glycol_HXCooling = 0;						// HXCooling mass
	Primary_Glycol_Loop1 = 0;							// Loop 1 mass
	Primary_Glycol_WaterHX = 0;							// Water glycol HX mass
	Primary_Glycol_Loop2 = 0;							// Loop 2 mass
	Primary_Glycol_HXHeating = 0;						// HXHeating mass
	Primary_Glycol_EvapIn = 0;							// Evap inlet mass
	Primary_Glycol_EvapOut = 0;							// Evap outlet mass
	Primary_Glycol_AscCooling = 0;						// Ascent battery cooling mass
	Primary_Glycol_DesCooling = 0;						// Descent battery cooling mass
	Secondary_Glycol_Accu = 0;							// Glycol Accumulator mass
	Secondary_Glycol_Pump_Manifold = 0;					// Pump manifold mass
	Secondary_Glycol_Loop1 = 0;							// Loop 1 mass
	Secondary_Glycol_AscCooling = 0;					// Ascent battery cooling mass
	Secondary_Glycol_Loop2 = 0;							// Loop 2 mass
	Secondary_Glycol_EvapIn = 0;						// Evap inlet mass
	Secondary_Glycol_EvapOut = 0;						// Evap outlet mass
	Primary_Glycol_Accu_Press = 0;
	PLSS_O2_Fill_Press = 0;

	// Open valves as would be for IVT
	Des_O2 = 0;
	Des_H2O_To_PLSS = 0;
	Cabin_Repress = 0; // Auto
	// For simplicity's sake, we'll use a docked LM as it would be at IVT, at first docking the LM is empty!
	Cabin_Press = 0; Cabin_Temp = 0;
	Suit_Press = 0; Suit_Temp = 0;
	SuitCircuit_CO2 = 0; HX_CO2 = 0;
	Water_Sep1_RPM = 0; Water_Sep2_RPM = 0;
	Suit_Circuit_Relief = 0;
	Cabin_Gas_Return = 0;
	Asc_Water1Temp = 0; Asc_Water2Temp = 0; WB_Prim_Water_Temp = 0;
	WB_Prim_Gly_In_Temp = 0; WB_Prim_Gly_Out_Temp = 0;
}

void LEM_ECS::Init(LEM *s) {
	lem = s;
}
void LEM_ECS::Timestep(double simdt) {
	if (lem == NULL) { return; }
	// **** Atmosphere Revitalization Section ****
	// First, get air from the suits and/or the cabin into the system.
	// Second, remove oxygen for and add CO2 from the crew.
	// Third, remove CO2 from the air and simulate the reaction in the LiOH can
	// Fourth, use the fans to move the resulting air through the suits and/or the cabin.
	// Fifth, use the heat exchanger to move heat from the air to the HTS if enabled (emergency ops)
	// Sixth, use the water separators to remove water from the air and add it to the WMS and surge tank.
	// Seventh, use the OSCPCS to add pressure if required
	// Eighth, use the regenerative heat exchanger to add heat to the air if required
	// Ninth and optionally, simulate the system behavior if a PGA failure is detected.
	// Tenth, simulate the LCG water movement operation.

	// **** Oxygen Supply and Cabin Pressure Control Section ****
	// Simple, move air from tanks to the cabin as required, and move air from the cabin to space as required.

	// **** Water Management Section ****
	// Also relatively simple, move water from tanks to the HTS / crew / etc as required.

	// **** Heat Transport Section ****
	// First, operate pumps to move glycol/water through the loops.
	// Second, move heat from the equipment to the glycol.
	// Third, move heat from the glycol to the sublimators.
	// Fourth, vent steam from the sublimators overboard.
}

void LEM_ECS::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {

}

void LEM_ECS::LoadState(FILEHANDLE scn, char *end_str) {

}

double LEM_ECS::DescentOxyTankPressurePSI() {
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;
	
	if (!Des_OxygenPress) {
		Des_OxygenPress = (double*)sdk.GetPointerByString("HYDRAULIC:DESO2TANK:PRESS");
	}
	return *Des_OxygenPress * PSI;
}

double LEM_ECS::AscentOxyTank1PressurePSI() {
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;
	
	if (!Asc_Oxygen1Press) {
		Asc_Oxygen1Press = (double*)sdk.GetPointerByString("HYDRAULIC:ASCO2TANK1:PRESS");
	}
	return *Asc_Oxygen1Press * PSI;
}

double LEM_ECS::AscentOxyTank2PressurePSI() {
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	if (!Asc_Oxygen2Press) {
		Asc_Oxygen2Press = (double*)sdk.GetPointerByString("HYDRAULIC:ASCO2TANK2:PRESS");
	}
	return *Asc_Oxygen2Press * PSI;
}

double LEM_ECS::DescentOxyTankQuantityLBS() {
	if (!Des_Oxygen) {
		Des_Oxygen = (double*)sdk.GetPointerByString("HYDRAULIC:DESO2TANK:MASS");
	}
	return *Des_Oxygen * LBS;	
}

double LEM_ECS::AscentOxyTank1QuantityLBS() {
	if (!Asc_Oxygen1) {
		Asc_Oxygen1 = (double*)sdk.GetPointerByString("HYDRAULIC:ASCO2TANK1:MASS");
	}
	return *Asc_Oxygen1 * LBS;	
}

double LEM_ECS::AscentOxyTank2QuantityLBS() {
	if (!Asc_Oxygen2) {
		Asc_Oxygen2 = (double*)sdk.GetPointerByString("HYDRAULIC:ASCO2TANK2:MASS");
	}
	return *Asc_Oxygen2 * LBS;	
}

double LEM_ECS::GetCabinPressurePSI() {
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	if (!Cabin_Press) {
		Cabin_Press = (double*)sdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	}
	return *Cabin_Press * PSI;
}

double LEM_ECS::GetSuitPressurePSI()
{
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;
	
	if (!Suit_Press) {
		Suit_Press = (double*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:PRESS");
	}
	return *Suit_Press * PSI;
}

double LEM_ECS::GetSensorCO2MMHg() {
	
	if (!lem->ECS_CO2_SENSOR_CB.IsPowered()) return 0.0;
	
	if (!SuitCircuit_CO2) {
		SuitCircuit_CO2 = (double*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:CO2_PPRESS");
	}
	if (!HX_CO2) {
		HX_CO2 = (double*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:CO2_PPRESS");
	}
	return ((*SuitCircuit_CO2 + *HX_CO2) / 2.0) * MMHG;
}

double LEM_ECS::DescentWaterTankQuantity() {
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;
	if (lem->stage > 1) return 0.0;

	if (!Des_Water) {
		Des_Water = (double*)sdk.GetPointerByString("HYDRAULIC:DESH2OTANK:MASS");
	}
	return (*Des_Water) / LM_DES_H2O_CAPACITY;
}

double LEM_ECS::AscentWaterTank1Quantity()
{
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	if (!Asc_Water1) {
		Asc_Water1 = (double*)sdk.GetPointerByString("HYDRAULIC:ASCH2OTANK1:MASS");
	}
	return (*Asc_Water1)/LM_ASC_H2O_CAPACITY;  
}

double LEM_ECS::AscentWaterTank2Quantity()
{
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	if (!Asc_Water2) {
		Asc_Water2 = (double*)sdk.GetPointerByString("HYDRAULIC:ASCH2OTANK2:MASS");
	}
	return (*Asc_Water2) / LM_ASC_H2O_CAPACITY;
}

double LEM_ECS::GetCabinTempF() {
	if (!Cabin_Temp) {
		Cabin_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:CABIN:TEMP");
	}
	return KelvinToFahrenheit(*Cabin_Temp);
}

double LEM_ECS::GetSuitTempF() {
	if (!Suit_Temp) {
		Suit_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:TEMP");
	}
	return KelvinToFahrenheit(*Suit_Temp);
}

double LEM_ECS::GetPrimaryGlycolPressure() {
	if (!Primary_CL_Glycol_Press) {
		Primary_CL_Glycol_Press = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLPUMPMANIFOLD:PRESS");
	}
	return *Primary_CL_Glycol_Press * PSI;
}

double LEM_ECS::GetPrimaryGlycolQuantity() {
	if (!Primary_Glycol_Accu) {
		Primary_Glycol_Accu = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:MASS");
	}
	if (!Primary_Glycol_Pump_Manifold) {
		Primary_Glycol_Pump_Manifold = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLPUMPMANIFOLD:MASS");
	}
	if (!Primary_Glycol_HXCooling) {
		Primary_Glycol_HXCooling = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXCOOLING:MASS");
	}
	if (!Primary_Glycol_Loop1) {
		Primary_Glycol_Loop1 = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP1:MASS");
	}
	if (!Primary_Glycol_WaterHX) {
		Primary_Glycol_WaterHX = (double*)sdk.GetPointerByString("HYDRAULIC:WATERGLYCOLHX:MASS");
	}
	if (!Primary_Glycol_Loop2) {
		Primary_Glycol_Loop2 = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP2:MASS");
	}
	if (!Primary_Glycol_HXHeating) {
		Primary_Glycol_HXHeating = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXHEATING:MASS");
	}
	if (!Primary_Glycol_EvapIn) {
		Primary_Glycol_EvapIn = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMEVAPINLET:MASS");
	}
	if (!Primary_Glycol_EvapOut) {
		Primary_Glycol_EvapOut = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMEVAPOUTLET:MASS");
	}
	if (!Primary_Glycol_AscCooling) {
		Primary_Glycol_AscCooling = (double*)sdk.GetPointerByString("HYDRAULIC:ASCBATCOOLING:MASS");
	}
	if (!Primary_Glycol_DesCooling) {
		Primary_Glycol_DesCooling = (double*)sdk.GetPointerByString("HYDRAULIC:DESBATCOOLING:MASS");
	}
	return (*Primary_Glycol_Accu + *Primary_Glycol_Pump_Manifold + *Primary_Glycol_HXCooling + *Primary_Glycol_Loop1 + *Primary_Glycol_WaterHX + *Primary_Glycol_Loop2 +
		*Primary_Glycol_HXHeating + *Primary_Glycol_EvapIn + *Primary_Glycol_EvapOut + *Primary_Glycol_AscCooling + *Primary_Glycol_DesCooling) * LBS;
}

double LEM_ECS::GetPrimaryGlycolTempF() {
	if (!Primary_CL_Glycol_Temp) {
		Primary_CL_Glycol_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:TEMP");
	}
	return KelvinToFahrenheit(*Primary_CL_Glycol_Temp);
}

double LEM_ECS::GetSecondaryGlycolPressure() {
	if (!Secondary_CL_Glycol_Press) {
		Secondary_CL_Glycol_Press = (double*)sdk.GetPointerByString("HYDRAULIC:SECGLYCOLPUMPFANMANIFOLD:PRESS");
	}
	return *Secondary_CL_Glycol_Press * PSI;
}

double LEM_ECS::GetSecondaryGlycolQuantity() {
	if (!Secondary_Glycol_Accu) {
		Secondary_Glycol_Accu = (double*)sdk.GetPointerByString("HYDRAULIC:SECGLYCOLACCUMULATOR:MASS");
	}
	if (!Secondary_Glycol_Pump_Manifold) {
		Secondary_Glycol_Pump_Manifold = (double*)sdk.GetPointerByString("HYDRAULIC:SECGLYCOLPUMPFANMANIFOLD:MASS");
	}
	if (!Secondary_Glycol_Loop1) {
		Secondary_Glycol_Loop1 = (double*)sdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP1:MASS");
	}
	if (!Secondary_Glycol_AscCooling) {
		Secondary_Glycol_AscCooling = (double*)sdk.GetPointerByString("HYDRAULIC:SECASCBATCOOLING:MASS");
	}
	if (!Secondary_Glycol_Loop2) {
		Secondary_Glycol_Loop2 = (double*)sdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP2:MASS");
	}
	if (!Secondary_Glycol_EvapIn) {
		Secondary_Glycol_EvapIn = (double*)sdk.GetPointerByString("HYDRAULIC:SECEVAPINLET:MASS");
	}
	if (!Secondary_Glycol_EvapOut) {
		Secondary_Glycol_EvapOut = (double*)sdk.GetPointerByString("HYDRAULIC:SECEVAPOUTLET:MASS");
	}
	return (*Secondary_Glycol_Accu + *Secondary_Glycol_Pump_Manifold + *Secondary_Glycol_Loop1 + *Secondary_Glycol_AscCooling + *Secondary_Glycol_Loop2
		+ *Secondary_Glycol_EvapIn + *Secondary_Glycol_EvapOut)* LBS;
}

double LEM_ECS::GetSecondaryGlycolTempF() {
	if (!Secondary_CL_Glycol_Temp) {
		Secondary_CL_Glycol_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:SECGLYCOLACCUMULATOR:TEMP");
	}
	return KelvinToFahrenheit(*Secondary_CL_Glycol_Temp);
}

double LEM_ECS::GetSelectedGlycolTempF()
{
	if (lem->GlycolRotary.GetState() == 0)
	{
		return GetSecondaryGlycolTempF();
	}

	return GetPrimaryGlycolTempF();
}

double LEM_ECS::GetSelectedGlycolPressure()
{
	if (lem->GlycolRotary.GetState() == 0)
	{
		return GetSecondaryGlycolPressure();
	}

	return GetPrimaryGlycolPressure();
}

double LEM_ECS::GetWaterSeparatorRPM()
{
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	if (!Water_Sep1_RPM) {
		Water_Sep1_RPM = (double*)sdk.GetPointerByString("HYDRAULIC:WATERSEP1:RPM");
	}
	if (!Water_Sep2_RPM) {
		Water_Sep2_RPM = (double*)sdk.GetPointerByString("HYDRAULIC:WATERSEP2:RPM");
	}

		if (*Water_Sep1_RPM > *Water_Sep2_RPM)
	{
			return (*Water_Sep1_RPM);
	}
		return (*Water_Sep2_RPM);
}

double LEM_ECS::GetAscWaterTank1TempF()
{
	if (!Asc_Water1Temp) {
		Asc_Water1Temp = (double*)sdk.GetPointerByString("HYDRAULIC:ASCH2OTANK1:TEMP");
	}
	return KelvinToFahrenheit(*Asc_Water1Temp);
}

double LEM_ECS::GetAscWaterTank2TempF()
{
	if (!Asc_Water2Temp) {
		Asc_Water2Temp = (double*)sdk.GetPointerByString("HYDRAULIC:ASCH2OTANK2:TEMP");
	}
	return KelvinToFahrenheit(*Asc_Water2Temp);
}

bool LEM_ECS::GetSuitFan1Failure()
{
	if (lem->SuitFanRotary.GetState() == 1)
	{
		if (lem->SuitFanDPSensor.GetSuitFanFail())
		{
			return true;
		}
	}

	return false;
}

bool LEM_ECS::GetSuitFan2Failure()
{
	if (lem->SuitFanRotary.GetState() == 2)
	{
		if (lem->SuitFanDPSensor.GetSuitFanFail())
		{
			return true;
		}
	}

	return false;
}

bool LEM_ECS::GetPrimGlycolLowLevel()
{
	if (lem->ecs.GetPrimaryGlycolQuantity() < 2.5)
	{
			return true;
	}

	return false;
}

bool LEM_ECS::GetSecGlycolLowLevel()
{
	if (lem->ecs.GetSecondaryGlycolQuantity() < 0.5)
	{
		return true;
	}

	return false;
}

bool LEM_ECS::IsSuitCircuitReliefValveOpen()
{
	if (!Suit_Circuit_Relief) {
		Suit_Circuit_Relief = (int*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:OUT2:ISOPEN");
	}
	if (Suit_Circuit_Relief)
	{
		return (*Suit_Circuit_Relief);
	}

	return false;
}

bool LEM_ECS::IsCabinGasReturnValveOpen()
{
	if (!Cabin_Gas_Return) {
		Cabin_Gas_Return = (int*)sdk.GetPointerByString("HYDRAULIC:CO2CANISTERMANIFOLD:LEAK:ISOPEN");
	}
	if (Cabin_Gas_Return)
	{
		return (*Cabin_Gas_Return);
	}

	return false;
}

bool LEM_ECS::GetGlycolPump2Failure()
{
	if (lem->GlycolRotary.GetState() != 0 && lem->PrimGlycolPumpController.GetPressureSwitch())
	{
		return true;
	}

	return false;
}

double LEM_ECS::GetPrimWBWaterInletTempF()
{
	if (!WB_Prim_Water_Temp) {
		WB_Prim_Water_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMWATERBOILER:TEMP");
	}
	return KelvinToFahrenheit(*WB_Prim_Water_Temp);
}

double LEM_ECS::GetPrimWBGlycolInletTempF()
{
	if (!WB_Prim_Gly_In_Temp) {
		WB_Prim_Gly_In_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMEVAPINLET:TEMP");
	}
	return KelvinToFahrenheit(*WB_Prim_Gly_In_Temp);
}

double LEM_ECS::GetPrimWBGlycolOutletTempF()
{
	if (!WB_Prim_Gly_Out_Temp) {
		WB_Prim_Gly_Out_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMEVAPOUTLET:TEMP");
	}
	return KelvinToFahrenheit(*WB_Prim_Gly_Out_Temp);
}

double LEM_ECS::GetPrimaryGlycolPumpDP()
{
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	if (!Primary_CL_Glycol_Press) {
		Primary_CL_Glycol_Press = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLPUMPMANIFOLD:PRESS");
	}
	if (!Primary_Glycol_Accu_Press) {
		Primary_Glycol_Accu_Press = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:PRESS");
	}
	return (*Primary_CL_Glycol_Press - *Primary_Glycol_Accu_Press)*PSI;
}

double LEM_ECS::GetPLSSFillPressurePSI()
{
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	if (!PLSS_O2_Fill_Press) {
		PLSS_O2_Fill_Press = (double*)sdk.GetPointerByString("HYDRAULIC:PLSSO2FILLVALVE:PRESS");
	}
	return (*PLSS_O2_Fill_Press)*PSI;
}

double LEM_ECS::DescentWaterTankPressure() {
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;
	if (lem->stage > 1) return 0.0;

	if (!Des_Water_Press) {
		Des_Water_Press = (double*)sdk.GetPointerByString("HYDRAULIC:DESH2OTANK:PRESS");
	}
	return (*Des_Water_Press)*PSI;
}
double LEM_ECS::GetECSSuitPSI() {

	if (!Suit_Press) {
		Suit_Press = (double*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:PRESS");
	}
	return *Suit_Press * PSI;
}

double LEM_ECS::GetECSCabinPSI() {

	if (!Cabin_Press) {
		Cabin_Press = (double*)sdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	}
	return *Cabin_Press * PSI;
}

double LEM_ECS::GetECSSensorCO2MMHg() {

	if (!SuitCircuit_CO2) {
		SuitCircuit_CO2 = (double*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:CO2_PPRESS");
	}
	if (!HX_CO2) {
		HX_CO2 = (double*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:CO2_PPRESS");
	}
	return ((*SuitCircuit_CO2 + *HX_CO2) / 2.0) * MMHG;
}