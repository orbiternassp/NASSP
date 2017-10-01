/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Saturn Instrument Unit header

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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"

#include "soundlib.h"
#include "nasspsound.h"
#include "nasspdefs.h"

#include "ioChannels.h"
#include "toggleswitch.h"
#include "apolloguidance.h"

#include "connector.h"
#include "csmcomputer.h"

#include "saturn.h"
#include "sivb.h"
#include "papi.h"
#include "LVDC.h"


IU::IU()

{
	State = 0;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;
	TLICapable = false;
	FirstTimeStepDone = false;

	Crewed = true;

	commandConnector.SetIU(this);
	lvdc = NULL;
}

IU::~IU()

{
	if (lvdc != NULL)
	{
		delete lvdc;
		lvdc = NULL;
	}
}

void IU::SetMissionInfo(bool tlicapable, bool crewed)
{
	TLICapable = tlicapable;
	Crewed = crewed;
}

void IU::Timestep(double simt, double simdt, double mjd)

{
	if (lvdc != NULL) {
		lvdc->TimeStep(simt, simdt);
	}

	// Only SIVB in orbit for now
	if (lvCommandConnector.GetStage() != STAGE_ORBIT_SIVB) return;

	//
	// Update mission time.
	//
	MissionTime = simt;

	// Initialization
	if (!FirstTimeStepDone) {

		//
		// Disable the engines if we're waiting for
		// the user to start the TLI burn or if it's been done.
		//
		if (State <= 107 || State >= 202)	{
			lvCommandConnector.EnableDisableJ2(false);
		} else {
			lvCommandConnector.EnableDisableJ2(true);
		}
		FirstTimeStepDone = true;
		return;
	}

	// Switches to inhibit TLI
	bool XLunar = (commandConnector.TLIEnableSwitchState() == TOGGLESWITCH_UP);
	bool SIISIVBSep = (commandConnector.SIISIVbSwitchState() == TOGGLESWITCH_UP);

	//sprintf(oapiDebugString(), "TLIBurnState %d State %d IgnMJD %.12f tGO %f vG x %f y %f z %f l %f Th %f", TLIBurnState, State, GNC.Get_IgnMJD(), GNC.Get_tGO(), GNC.Get_vG().x, GNC.Get_vG().y, GNC.Get_vG().z, length(GNC.Get_vG()), lvCommandConnector.GetJ2ThrustLevel()); 
}

void IU::PostStep(double simt, double simdt, double mjd) {

}

void IU::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, IU_START_STRING);

	oapiWriteScenario_int(scn, "STATE", State);
	papiWriteScenario_double(scn, "NEXTMISSIONEVENTTIME", NextMissionEventTime);
	papiWriteScenario_double(scn, "LASTMISSIONEVENTTIME", LastMissionEventTime);

	oapiWriteLine(scn, IU_END_STRING);
}

void IU::LoadState(FILEHANDLE scn)

{
	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, IU_END_STRING, sizeof(IU_END_STRING)))
			return;

		if (papiReadScenario_int(line, "STATE", State)); 
		else if (papiReadScenario_double(line, "NEXTMISSIONEVENTTIME", NextMissionEventTime)); 
		else if (papiReadScenario_double(line, "LASTMISSIONEVENTTIME", LastMissionEventTime)) {};
	}
}

void IU::SaveLVDC(FILEHANDLE scn) {
	if (lvdc != NULL) { lvdc->SaveState(scn); }
}

void IU::ConnectToCSM(Connector *csmConnector)

{
	commandConnector.ConnectTo(csmConnector);
}

void IU::ConnectToMultiConnector(MultiConnector *csmConnector)

{
	csmConnector->AddTo(&commandConnector);
}

void IU::ConnectToLV(Connector *CommandConnector)

{
	lvCommandConnector.ConnectTo(CommandConnector);
}

void IU::ConnectLVDC()
{
	if (lvdc)
	{
		lvdc->Configure(&lvCommandConnector, &commandConnector);
	}
}

IUToCSMCommandConnector::IUToCSMCommandConnector()

{
	type = CSM_IU_COMMAND;
	ourIU = 0;
}

IUToCSMCommandConnector::~IUToCSMCommandConnector()

{
}

void IUToCSMCommandConnector::SetAGCInputChannelBit(int channel, int bit, bool val)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_INPUT_CHANNEL_BIT;
	cm.val1.iValue = channel;
	cm.val2.iValue = bit;
	cm.val3.bValue = val;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SetAGCOutputChannel(int channel, int value)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_OUTPUT_CHANNEL;
	cm.val1.iValue = channel;
	cm.val2.iValue = value;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SetSIISep()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_SII_SEP_LIGHT;
	cm.val1.bValue = true;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearSIISep()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_SII_SEP_LIGHT;
	cm.val1.bValue = false;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SetLiftoffLight()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_LIFTOFF_LIGHT;
	cm.val1.bValue = true;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearLiftoffLight()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_LIFTOFF_LIGHT;
	cm.val1.bValue = false;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SetLVRateLight()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_LV_RATE_LIGHT;
	cm.val1.bValue = true;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearLVRateLight()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_LV_RATE_LIGHT;
	cm.val1.bValue = false;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SetLVGuidLight()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_LV_GUID_LIGHT;
	cm.val1.bValue = true;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearLVGuidLight()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_LV_GUID_LIGHT;
	cm.val1.bValue = false;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SetEDSAbort(int eds)
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_EDS_ABORT;
	cm.val1.iValue = eds;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SetEngineIndicator(int eng)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_ENGINE_INDICATOR;

	cm.val1.iValue = eng;
	cm.val2.bValue = true;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearEngineIndicator(int eng)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_ENGINE_INDICATOR;

	cm.val1.iValue = eng;
	cm.val2.bValue = false;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearEngineIndicators()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_ENGINE_INDICATORS;

	cm.val1.bValue = false;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SlowIfDesired()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SLOW_IF_DESIRED;

	SendMessage(cm);
}

bool IUToCSMCommandConnector::GetSIISepLight()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_SII_SEP_LIGHT;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool IUToCSMCommandConnector::GetEngineIndicator(int eng)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_ENGINE_INDICATOR;
	cm.val1.iValue = eng;

	if (SendMessage(cm))
	{
		return cm.val2.bValue;
	}

	return false;
}

int IUToCSMCommandConnector::TLIEnableSwitchState()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_TLI_ENABLE_SWITCH_STATE;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return -1;
}

int IUToCSMCommandConnector::SIISIVbSwitchState()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_SIISIVBSEP_SWITCH_STATE;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return -1;
}

int IUToCSMCommandConnector::LVGuidanceSwitchState()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_LV_GUIDANCE_SWITCH_STATE;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return -1;
}

int IUToCSMCommandConnector::EDSSwitchState()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_EDS_SWITCH_STATE;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return -1;
}

int IUToCSMCommandConnector::LVRateAutoSwitchState()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_LV_RATE_AUTO_SWITCH_STATE;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return -1;
}

int IUToCSMCommandConnector::TwoEngineOutAutoSwitchState()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_TWO_ENGINE_OUT_AUTO_SWITCH_STATE;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return -1;
}

bool IUToCSMCommandConnector::GetBECOSignal()
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_BECO_SIGNAL;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

int IUToCSMCommandConnector::GetAGCAttitudeError(int axis)
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_AGC_ATTITUDE_ERROR;
	cm.val1.iValue = axis;

	if (SendMessage(cm))
	{
		return cm.val2.iValue;
	}

	return 0;
}

bool IUToCSMCommandConnector::GetAGCInputChannelBit(int channel, int bit)
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_INPUT_CHANNEL_BIT;
	cm.val1.iValue = channel;
	cm.val2.iValue = bit;

	if (SendMessage(cm))
	{
		return cm.val3.bValue;
	}

	return false;
}

void IUToCSMCommandConnector::LoadTLISounds()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_LOAD_TLI_SOUNDS;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearTLISounds()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_CLEAR_TLI_SOUNDS;

	SendMessage(cm);
}

void IUToCSMCommandConnector::PlayCountSound(bool StartStop)

{
	PlayStopSound(IUCSM_PLAY_COUNT_SOUND, StartStop);
}

void IUToCSMCommandConnector::PlaySecoSound(bool StartStop)

{
	PlayStopSound(IUCSM_PLAY_SECO_SOUND, StartStop);
}

void IUToCSMCommandConnector::PlaySepsSound(bool StartStop)

{
	PlayStopSound(IUCSM_PLAY_SEPS_SOUND, StartStop);
}

void IUToCSMCommandConnector::PlayTLISound(bool StartStop)

{
	PlayStopSound(IUCSM_PLAY_TLI_SOUND, StartStop);
}

void IUToCSMCommandConnector::PlayTLIStartSound(bool StartStop)

{
	PlayStopSound(IUCSM_PLAY_TLISTART_SOUND, StartStop);
}

void IUToCSMCommandConnector::PlayStopSound(IUCSMMessageType sound, bool StartStop)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = sound;
	cm.val1.bValue = StartStop;

	SendMessage(cm);
}
void IUToCSMCommandConnector::TLIBegun()
{
	ConnectorMessage cm;
	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_TLI_BEGUN;

	SendMessage(cm);
}

void IUToCSMCommandConnector::TLIEnded()
{
	ConnectorMessage cm;
	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_TLI_ENDED;

	SendMessage(cm);
}

//
// Process incoming messages from the CSM.
//

bool IUToCSMCommandConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	IUCSMMessageType messageType;

	messageType = (IUCSMMessageType) m.messageType;

	switch (messageType)
	{
	case CSMIU_SET_VESSEL_STATS:
		if (ourIU)
		{
			//ourIU->SetVesselStats(m.val1.dValue, m.val2.dValue);
		}
		return true;

	case CSMIU_START_TLI_BURN:
		if (ourIU)
		{
			//ourIU->StartTLIBurn(m.val1.vValue, m.val2.vValue, m.val3.vValue, m.val4.dValue);
		}
		return true;

	case CSMIU_IS_TLI_CAPABLE:
		if (ourIU)
		{
			//m.val1.bValue = ourIU->IsTLICapable();
			return true;
		}
		break;

	case CSMIU_CHANNEL_OUTPUT:
		if (ourIU)
		{
			//ourIU->ChannelOutput(m.val1.iValue, m.val2.iValue);
			return true;
		}
		break;

	case CSMIU_GET_VESSEL_STATS:
		if (ourIU)
		{
			//ourIU->GetVesselStats(m.val1.dValue, m.val2.dValue);
			return true;
		}
		break;

	case CSMIU_GET_VESSEL_MASS:
		if (ourIU)
		{
			//m.val1.dValue = ourIU->GetMass();
			return true;
		}
		break;

	case CSMIU_GET_VESSEL_FUEL:
		if (ourIU)
		{
			//m.val1.dValue = ourIU->GetFuelMass();
			return true;
		}
		break;
	}

	return false;
}

IUToLVCommandConnector::IUToLVCommandConnector()

{
	type = LV_IU_COMMAND;
}

IUToLVCommandConnector::~IUToLVCommandConnector()

{
}

void IUToLVCommandConnector::EnableDisableJ2(bool Enable)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_ENABLE_J2;
	cm.val1.bValue = Enable;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetJ2ThrustLevel(double thrust)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_J2_THRUST_LEVEL;
	cm.val1.dValue = thrust;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetVentingThruster()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_J2_DONE;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetThrusterLevel(THRUSTER_HANDLE th, double level)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_THRUSTER_LEVEL;
	cm.val1.pValue = th;
	cm.val2.dValue = level;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetThrusterGroupLevel(THGROUP_HANDLE thg, double level)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_THRUSTER_GROUP_LEVEL;
	cm.val1.pValue = thg;
	cm.val2.dValue = level;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetThrusterResource(THRUSTER_HANDLE th, PROPELLANT_HANDLE ph)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_THRUSTER_RESOURCE;
	cm.val1.pValue = th;
	cm.val2.pValue = ph;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetThrusterDir(THRUSTER_HANDLE th, VECTOR3 &dir)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_THRUSTER_DIR;
	cm.val1.pValue = th;
	cm.val2.pValue = &dir;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetAPSThrustLevel(double thrust)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_APS_THRUST_LEVEL;
	cm.val1.dValue = thrust;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetContrailLevel(double level)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_CONTRAIL_LEVEL;
	cm.val1.dValue = level;

	SendMessage(cm);
}

void IUToLVCommandConnector::SIVBBoiloff()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SIVB_BOILOFF;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetAttitudeLinLevel(int a1, int a2)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_ATTITUDE_LIN_LEVEL;
	cm.val1.iValue = a1;
	cm.val2.iValue = a2;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetAttitudeRotLevel (VECTOR3 th)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_ATTITUDE_ROT_LEVEL;
	cm.val1.vValue = th;

	SendMessage(cm);
}

void IUToLVCommandConnector::AddForce(VECTOR3 F, VECTOR3 r)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_ADD_FORCE;
	cm.val1.vValue = F;
	cm.val2.vValue = r;

	SendMessage(cm);
}

void IUToLVCommandConnector::ActivateNavmode(int mode)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_ACTIVATE_NAVMODE;
	cm.val1.iValue = mode;

	SendMessage(cm);
}

void IUToLVCommandConnector::DeactivateNavmode(int mode)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_DEACTIVATE_NAVMODE;
	cm.val1.iValue = mode;

	SendMessage(cm);
}

void IUToLVCommandConnector::SwitchSelector(int item)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SWITCH_SELECTOR;
	cm.val1.iValue = item;

	SendMessage(cm);
}

void IUToLVCommandConnector::SeparateStage(int stage)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SEPARATE_STAGE;
	cm.val1.iValue = stage;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetStage(int stage)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_STAGE;
	cm.val1.iValue = stage;

	SendMessage(cm);
}

void IUToLVCommandConnector::DeactivateS4RCS()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_DEACTIVATE_S4RCS;

	SendMessage(cm);
}

void IUToLVCommandConnector::ActivateS4RCS()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_ACTIVATE_S4RCS;

	SendMessage(cm);
}

void IUToLVCommandConnector::AddRCS_S4B()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_ADD_S4RCS;

	SendMessage(cm);
}

void IUToLVCommandConnector::DeactivatePrelaunchVenting()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_DEACTIVATE_PRELAUNCH_VENTING;

	SendMessage(cm);
}

void IUToLVCommandConnector::ActivatePrelaunchVenting()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_ACTIVATE_PRELAUNCH_VENTING;

	SendMessage(cm);
}

double IUToLVCommandConnector::GetMass()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_MASS;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetSize()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SIZE;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetMaxThrust(ENGINETYPE eng)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_MAXTHRUST;
	cm.val1.iValue = eng;

	if (SendMessage(cm))
	{
		return cm.val2.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetThrusterMax(THRUSTER_HANDLE th)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_THRUSTER_MAX;
	cm.val1.pValue = th;

	if (SendMessage(cm))
	{
		return cm.val2.dValue;
	}

	return 0.0;
}

PROPELLANT_HANDLE IUToLVCommandConnector::GetThrusterResource(THRUSTER_HANDLE th)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_THRUSTER_RESOURCE;
	cm.val1.pValue = th;

	if (SendMessage(cm))
	{
		return (PROPELLANT_HANDLE) cm.val2.pValue;
	}

	return 0;
}

int IUToLVCommandConnector::GetStage()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_STAGE;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return NULL_STAGE;
}

double IUToLVCommandConnector::GetJ2ThrustLevel()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_J2_THRUST_LEVEL;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetAltitude()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_ALTITUDE;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetSIVBPropellantMass()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SIVB_PROPELLANT_MASS;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetPropellantMass(PROPELLANT_HANDLE ph)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_PROPELLANT_MASS;
	cm.val1.pValue = ph;

	if (SendMessage(cm))
	{
		return cm.val2.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetMaxFuelMass()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_MAX_FUEL_MASS;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetFuelMass()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_FUEL_MASS;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

void IUToLVCommandConnector::GetStatus(VESSELSTATUS &status)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_STATUS;
	cm.val1.pValue = &status;

	SendMessage(cm);
}

void IUToLVCommandConnector::GetGlobalOrientation(VECTOR3 &arot)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_GLOBAL_ORIENTATION;
	cm.val1.pValue = &arot;

	SendMessage(cm);
}

void IUToLVCommandConnector::GetPMI(VECTOR3 &pmi)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_PMI;
	cm.val1.pValue = &pmi;

	SendMessage(cm);
}

OBJHANDLE IUToLVCommandConnector::GetGravityRef()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_GRAVITY_REF;

	if (SendMessage(cm))
	{
		return cm.val1.hValue;
	}

	return 0;
}

void IUToLVCommandConnector::GetApDist(double &d)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_AP_DIST;

	if (SendMessage(cm))
	{
		d = cm.val1.dValue;
		return;
	}

	d = 0.0;
}

void IUToLVCommandConnector::Local2Global(VECTOR3 &local, VECTOR3 &global)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_LOCAL2GLOBAL;
	cm.val1.pValue = &local;
	cm.val2.pValue = &global;

	SendMessage(cm);
}

void IUToLVCommandConnector::GetRelativePos(OBJHANDLE ref, VECTOR3 &v)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_RELATIVE_POS;
	cm.val1.hValue = ref;
	cm.val2.pValue = &v;

	SendMessage(cm);
}

void IUToLVCommandConnector::GetRelativeVel(OBJHANDLE ref, VECTOR3 &v)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_RELATIVE_VEL;
	cm.val1.hValue = ref;
	cm.val2.pValue = &v;

	SendMessage(cm);
}

void IUToLVCommandConnector::GetGlobalVel(VECTOR3 &v)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_GLOBAL_VEL;
	cm.val1.pValue = &v;

	SendMessage(cm);
}

OBJHANDLE IUToLVCommandConnector::GetElements(ELEMENTS &el, double &mjd_ref)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_ELEMENTS;
	cm.val1.pValue = &el;

	if (SendMessage(cm))
	{
		mjd_ref = cm.val2.dValue;
		return cm.val3.hValue;
	}

	return 0;
}

bool IUToLVCommandConnector::GetWeightVector(VECTOR3 &w)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_WEIGHTVECTOR;
	cm.val1.pValue = &w;

	if (SendMessage(cm))
	{		
		return cm.val2.bValue; 
	}

	return false;
}

bool IUToLVCommandConnector::GetForceVector(VECTOR3 &f)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_FORCEVECTOR;
	cm.val1.pValue = &f;

	if (SendMessage(cm))
	{		
		return cm.val2.bValue; 
	}

	return false;
}

void IUToLVCommandConnector::GetRotationMatrix(MATRIX3 &rot)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_ROTATIONMATRIX;
	cm.val1.pValue = &rot;

	SendMessage(cm);
}

void IUToLVCommandConnector::GetAngularVel(VECTOR3 &avel)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_ANGULARVEL;
	cm.val1.pValue = &avel;

	SendMessage(cm);
}

double IUToLVCommandConnector::GetPitch()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_PITCH;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetBank()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_BANK;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetSlipAngle()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SLIP_ANGLE;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetMissionTime()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_MISSIONTIME;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

int IUToLVCommandConnector::GetApolloNo()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_APOLLONO;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return 0;
}

THRUSTER_HANDLE IUToLVCommandConnector::GetMainThruster(int n)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_MAIN_THRUSTER;
	cm.val1.iValue = n;

	if (SendMessage(cm))
	{
		return cm.val2.pValue;
	}

	return 0;
}

THRUSTER_HANDLE IUToLVCommandConnector::GetAPSThruster(int n)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_APS_THRUSTER;
	cm.val1.iValue = n;

	if (SendMessage(cm))
	{
		return cm.val2.pValue;
	}

	return 0;
}

THGROUP_HANDLE IUToLVCommandConnector::GetMainThrusterGroup()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_MAIN_THRUSTER_GROUP;

	if (SendMessage(cm))
	{
		return cm.val1.pValue;
	}

	return 0;
}

THGROUP_HANDLE IUToLVCommandConnector::GetVernierThrusterGroup()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_VERNIER_THRUSTER_GROUP;

	if (SendMessage(cm))
	{
		return cm.val1.pValue;
	}

	return 0;
}

THGROUP_HANDLE IUToLVCommandConnector::GetAPSThrusterGroup()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_APS_THRUSTER_GROUP;

	if (SendMessage(cm))
	{
		return cm.val1.pValue;
	}

	return 0;
}

double IUToLVCommandConnector::GetThrusterLevel(THRUSTER_HANDLE th)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_THRUSTER_LEVEL;
	cm.val1.pValue = th;

	if (SendMessage(cm))
	{
		return cm.val2.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetThrusterGroupLevel(THGROUP_HANDLE th)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_THRUSTER_GROUP_LEVEL;
	cm.val1.pValue = th;

	if (SendMessage(cm))
	{
		return cm.val2.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetFirstStageThrust()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_FIRST_STAGE_THRUST;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

PROPELLANT_HANDLE IUToLVCommandConnector::GetFirstStagePropellantHandle()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_FIRST_STAGE_PROPELLANT_HANDLE;

	if (SendMessage(cm))
	{
		return cm.val1.pValue;
	}

	return 0;
}

PROPELLANT_HANDLE IUToLVCommandConnector::GetThirdStagePropellantHandle()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_THIRD_STAGE_PROPELLANT_HANDLE;

	if (SendMessage(cm))
	{
		return cm.val1.pValue;
	}

	return 0;
}

IU1B::IU1B()
{

}

void IU1B::LoadLVDC(FILEHANDLE scn) {
	// If the LVDC does not yet exist, create it.
	if (lvdc == NULL) {
		lvdc = new LVDC1B;
		lvdc->Init(&lvCommandConnector, &commandConnector);

	}
	lvdc->LoadState(scn);
}

IUSV::IUSV()
{

}

void IUSV::LoadLVDC(FILEHANDLE scn) {
	// If the LVDC does not yet exist, create it.
	if (lvdc == NULL) {
		lvdc = new LVDCSV;
		lvdc->Init(&lvCommandConnector, &commandConnector);

	}
	lvdc->LoadState(scn);
}