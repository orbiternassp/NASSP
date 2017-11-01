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

void IU::Timestep(double misst, double simt, double simdt, double mjd)
{
	//
	// Update mission time.
	//
	MissionTime = misst;

	// Initialization
	if (!FirstTimeStepDone) {

		FirstTimeStepDone = true;
		return;
	}

	if (lvdc == NULL) return;

	lvimu.Timestep(mjd);
	lvrg.Timestep(simdt);

	if (lvdc->GetGuidanceReferenceFailure())
	{
		commandConnector.SetLVGuidLight();
	}
}

void IU::PostStep(double simt, double simdt, double mjd) {

}

void IU::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, IU_START_STRING);

	oapiWriteScenario_int(scn, "STATE", State);
	papiWriteScenario_double(scn, "NEXTMISSIONEVENTTIME", NextMissionEventTime);
	papiWriteScenario_double(scn, "LASTMISSIONEVENTTIME", LastMissionEventTime);

	SaveFCC(scn);
	SaveEDS(scn);
	
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
		else if (papiReadScenario_double(line, "LASTMISSIONEVENTTIME", LastMissionEventTime));
		else if (!strnicmp(line, "FCC_BEGIN", sizeof("FCC_BEGIN"))) {
			LoadFCC(scn);
		}
		else if (!strnicmp(line, "EDS_BEGIN", sizeof("EDS1_BEGIN"))) {
			LoadEDS(scn);
		}
	}
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

bool IU::GetSIPropellantDepletionEngineCutoff()
{
	int stage = lvCommandConnector.GetStage();
	if (stage != LAUNCH_STAGE_ONE) return false;

	if (lvCommandConnector.GetFuelMass() <= 0) return true;

	return false;
}

bool IU::SIBLowLevelSensorsDry()
{
	return false;
}

bool IU::GetSIIPropellantDepletionEngineCutoff()
{
	return false;
}

bool IU::GetSIVBEngineOut()
{
	int stage = lvCommandConnector.GetStage();
	if (stage != LAUNCH_STAGE_SIVB && stage != STAGE_ORBIT_SIVB) return false;

	double oetl = lvCommandConnector.GetSIVBThrustOK();
	if (oetl == 0) return true;

	return false;
}

void IU::DisconnectIU()
{
	lvCommandConnector.Disconnect();
	commandConnector.Disconnect();
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

bool IUToCSMCommandConnector::GetBECOCommand(bool IsSysA)
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_BECO_COMMAND;
	cm.val1.bValue = IsSysA;

	if (SendMessage(cm))
	{
		return cm.val2.bValue;
	}

	return false;
}

bool IUToCSMCommandConnector::IsEDSBusPowered(int eds)
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_IS_EDS_BUS_POWERED;
	cm.val1.iValue = eds;

	if (SendMessage(cm))
	{
		return cm.val2.bValue;
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

	case CSMIU_GET_LIFTOFF_CIRCUIT:
		if (ourIU)
		{
			if (m.val1.bValue)
			{
				m.val2.bValue = ourIU->GetEDS()->GetLiftoffCircuitA();
			}
			else
			{
				m.val2.bValue = ourIU->GetEDS()->GetLiftoffCircuitB();
			}
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

void IUToLVCommandConnector::SetSIThrusterLevel(int n, double level)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_SI_THRUSTER_LEVEL;
	cm.val1.iValue = n;
	cm.val2.dValue = level;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetSIIThrusterLevel(int n, double level)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_SII_THRUSTER_LEVEL;
	cm.val1.iValue = n;
	cm.val2.dValue = level;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetAPSThrusterLevel(int n, double level)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_APS_THRUSTER_LEVEL;
	cm.val1.iValue = n;
	cm.val2.dValue = level;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetAPSUllageThrusterLevel(int n, double level)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_APS_ULLAGE_THRUSTER_LEVEL;
	cm.val1.iValue = n;
	cm.val2.dValue = level;

	SendMessage(cm);
}

void IUToLVCommandConnector::ClearSIThrusterResource(int n)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_CLEAR_SI_THRUSTER_RESOURCE;
	cm.val1.iValue = n;

	SendMessage(cm);
}

void IUToLVCommandConnector::ClearSIIThrusterResource(int n)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_CLEAR_SII_THRUSTER_RESOURCE;
	cm.val1.iValue = n;

	SendMessage(cm);
}

void IUToLVCommandConnector::SIVBEDSCutoff(bool cut)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SIVB_EDS_CUTOFF;
	cm.val1.bValue = cut;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetSIThrusterDir(int n, VECTOR3 &dir)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_SI_THRUSTER_DIR;
	cm.val1.iValue = n;
	cm.val2.pValue = &dir;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetSIIThrusterDir(int n, VECTOR3 &dir)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_SII_THRUSTER_DIR;
	cm.val1.iValue = n;
	cm.val2.pValue = &dir;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetSIVBThrusterDir(VECTOR3 &dir)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_SIVB_THRUSTER_DIR;
	cm.val1.pValue = &dir;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetQBallPowerOff()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_QBALL_POWER_OFF;

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

void IUToLVCommandConnector::SISwitchSelector(int channel)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SI_SWITCH_SELECTOR;
	cm.val1.iValue = channel;

	SendMessage(cm);
}

void IUToLVCommandConnector::SIISwitchSelector(int channel)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SII_SWITCH_SELECTOR;
	cm.val1.iValue = channel;

	SendMessage(cm);
}

void IUToLVCommandConnector::SIVBSwitchSelector(int channel)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SIVB_SWITCH_SELECTOR;
	cm.val1.iValue = channel;

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

double IUToLVCommandConnector::GetSIPropellantMass()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SI_PROPELLANT_MASS;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
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

void IUToLVCommandConnector::GetGlobalOrientation(VECTOR3 &arot)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_GLOBAL_ORIENTATION;
	cm.val1.pValue = &arot;

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

double IUToLVCommandConnector::GetSIThrusterLevel(int n)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SI_THRUSTER_LEVEL;
	cm.val1.iValue = n;

	if (SendMessage(cm))
	{
		return cm.val2.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetSIIThrusterLevel(int n)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SII_THRUSTER_LEVEL;
	cm.val1.iValue = n;

	if (SendMessage(cm))
	{
		return cm.val2.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetSIVBThrustOK()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SIVB_THRUST_OK;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
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

bool IUToLVCommandConnector::CSMSeparationSensed()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_CSM_SEPARATION_SENSED;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

void IU::SaveLVDC(FILEHANDLE scn) {
	if (lvdc != NULL) {
		lvdc->SaveState(scn);
		lvimu.SaveState(scn);
	}
}

void IU::ControlDistributor(int stage, int channel)
{
	if (stage == SWITCH_SELECTOR_IU)
	{
		SwitchSelector(channel);
	}
	else if (stage == SWITCH_SELECTOR_SI)
	{
		lvCommandConnector.SISwitchSelector(channel);
	}
	else if (stage == SWITCH_SELECTOR_SII)
	{
		lvCommandConnector.SIISwitchSelector(channel);
	}
	else if (stage == SWITCH_SELECTOR_SIVB)
	{
		lvCommandConnector.SIVBSwitchSelector(channel);
	}
}

IU1B::IU1B() : fcc(lvrg), eds(lvrg)
{
	lvda.Init(this);
	eds.Init(this);
}

void IU1B::Timestep(double misst, double simt, double simdt, double mjd)
{
	IU::Timestep(misst, simt, simdt, mjd);

	if (lvdc != NULL) {
		eds.Timestep(simdt);
		lvdc->TimeStep(simt, simdt);
	}
	fcc.Timestep(simdt);

	//For now, enable the LV lights here
	if (MissionTime > -250.0 && MissionTime < -10.0)
	{
		eds.SetSIEngineOutIndicationA(true);
		eds.SetSIEngineOutIndicationB(true);
	}
}

bool IU1B::SIBLowLevelSensorsDry()
{
	if (lvCommandConnector.GetSIPropellantMass() <= 24000.0) return true;

	return false;
}

void IU1B::LoadLVDC(FILEHANDLE scn) {

	char *line;

	// If the LVDC does not yet exist, create it.
	if (lvdc == NULL) {
		lvdc = new LVDC1B(lvda);
		lvimu.Init();							// Initialize IMU
		lvrg.Init(&lvCommandConnector);			// LV Rate Gyro Package
		lvimu.SetVessel(&lvCommandConnector);	// set vessel pointer
		lvimu.CoarseAlignEnableFlag = false;	// Clobber this
		lvdc->Init(&lvCommandConnector);
		fcc.Init(this);
	}
	lvdc->LoadState(scn);

	if (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, LVIMU_START_STRING, sizeof(LVIMU_START_STRING))) {
			lvimu.LoadState(scn);
		}
	}
}

void IU1B::SaveFCC(FILEHANDLE scn)
{
	fcc.SaveState(scn, "FCC_BEGIN", "FCC_END");
}

void IU1B::LoadFCC(FILEHANDLE scn)
{
	fcc.LoadState(scn, "FCC_END");
}

void IU1B::SaveEDS(FILEHANDLE scn)
{
	eds.SaveState(scn, "EDS_BEGIN", "EDS_END");
}

void IU1B::LoadEDS(FILEHANDLE scn)
{
	eds.LoadState(scn, "EDS_END");
}

void IU1B::SwitchSelector(int item)
{
	switch (item)
	{
	case 0:	//Liftoff (NOT A REAL SWITCH SELECTOR CHANNEL)
		fcc.SetGainSwitch(0);
		commandConnector.SetAGCInputChannelBit(030, LiftOff, true);
		break;
	case 1: //Q-Ball Power Off
		lvCommandConnector.SetQBallPowerOff();
		break;
	case 2: //Excess Rate (P,Y,R) Auto-Abort Inhibit and Switch Rate Gyro SC Indication "A"
		eds.SetExcessiveRatesAutoAbortInhibit(true);
		eds.SetRateGyroSCIndicationSwitchA(true);
		break;
	case 3: //S-IVB Engine EDS Cutoffs Disable
		eds.SetSIVBEngineOutIndicationA(false);
		eds.SetSIVBEngineOutIndicationB(false);
		eds.SetSIVBEngineCutoffDisabled();
		break;
	case 5: //Flight Control Computer S-IVB Burn Mode Off "B"
		fcc.SetSIVBBurnMode(false);
		break;
	case 6: //Flight Control Computer S-IVB Burn Mode On "B"
		fcc.SetStageSwitch(2);
		fcc.SetSIVBBurnMode(true);
		break;
	case 9: //S-IVB Engine Out Indication "A" Enable
		eds.SetSIVBEngineOutIndicationA(true);
		break;
	case 12: //Flight Control Computer S-IVB Burn Mode Off "A"
		fcc.SetSIVBBurnMode(false);
		break;
	case 15: //Excess Rate (P,Y,R) Auto-Abort Inhibit Enable
		break;
	case 16: //Auto-Abort Enable Relays Reset
		eds.ResetAutoAbortRelays();
		break;
	case 18: //S/C Control of Saturn Enable
		fcc.EnableSCControl();
		break;
	case 21: //Flight Control Computer Switch Point No. 2
		fcc.SetGainSwitch(2);
		break;
	case 22: //Flight Control Computer Switch Point No. 3
		fcc.SetGainSwitch(3);
		break;
	case 29: //S-IVB Engine Out Indication "B" Enable
		eds.SetSIVBEngineOutIndicationB(true);
		break;
	case 34: //Excess Rate (Roll) Auto-Abort Inhibit Enable
		break;
	case 35: //S-IB Two Engines Out Auto-Abort Inhibit
		eds.SetTwoEngOutAutoAbortInhibit(true);
		break;
	case 38: //Launch Vehicle Engines EDS Cutoff Enable
		eds.SetLVEnginesCutoffEnable(true);
		break;
	case 43: //Flight Control Computer Switch Point No. 1
		fcc.SetGainSwitch(1);
		break;
	case 50: //Excess Rate (Roll) Auto-Abort Inhibit and Switch Rate Gyro SC Indication "B"
		eds.SetExcessiveRatesAutoAbortInhibit(true);
		eds.SetRateGyroSCIndicationSwitchB(true);
		break;
	case 51: //S-IB Two Engines Out Auto-Abort Inhibit Enable
		break;
	case 53: //Flight Control Computer S-IVB Burn Mode On "A"
		fcc.SetStageSwitch(2);
		fcc.SetSIVBBurnMode(true);
		break;
	default:
		break;
	}
}

IUSV::IUSV() : fcc(lvrg), eds(lvrg)
{
	lvda.Init(this);
	eds.Init(this);
}

void IUSV::Timestep(double misst, double simt, double simdt, double mjd)
{
	IU::Timestep(misst, simt, simdt, mjd);

	if (lvdc != NULL) {
		eds.Timestep(simdt);
		lvdc->TimeStep(simt, simdt);
	}
	fcc.Timestep(simdt);

	//For now, enable the LV lights here
	if (MissionTime > -250.0 && MissionTime < -10.0)
	{
		eds.SetSIEngineOutIndicationA(true);
		eds.SetSIEngineOutIndicationB(true);
	}
}

void IUSV::LoadLVDC(FILEHANDLE scn) {

	char *line;

	// If the LVDC does not yet exist, create it.
	if (lvdc == NULL) {
		lvdc = new LVDCSV(lvda);
		lvimu.Init();							// Initialize IMU
		lvrg.Init(&lvCommandConnector);			// LV Rate Gyro Package
		lvimu.SetVessel(&lvCommandConnector);	// set vessel pointer
		lvimu.CoarseAlignEnableFlag = false;	// Clobber this
		lvdc->Init(&lvCommandConnector);
		fcc.Init(this);
	}
	lvdc->LoadState(scn);

	if (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, LVIMU_START_STRING, sizeof(LVIMU_START_STRING))) {
			lvimu.LoadState(scn);
		}
	}
}

bool IUSV::GetSIIPropellantDepletionEngineCutoff()
{
	int stage = lvCommandConnector.GetStage();
	if (stage != LAUNCH_STAGE_TWO && stage != LAUNCH_STAGE_TWO_ISTG_JET) return false;

	double oetl = lvCommandConnector.GetSIIThrusterLevel(0) + lvCommandConnector.GetSIIThrusterLevel(1) + lvCommandConnector.GetSIIThrusterLevel(2) + lvCommandConnector.GetSIIThrusterLevel(3);
	if (oetl == 0) return true;

	return false;
}

void IUSV::SaveFCC(FILEHANDLE scn)
{
	fcc.SaveState(scn, "FCC_BEGIN", "FCC_END");
}

void IUSV::LoadFCC(FILEHANDLE scn)
{
	fcc.LoadState(scn, "FCC_END");
}

void IUSV::SaveEDS(FILEHANDLE scn)
{
	eds.SaveState(scn, "EDS_BEGIN", "EDS_END");
}

void IUSV::LoadEDS(FILEHANDLE scn)
{
	eds.LoadState(scn, "EDS_END");
}

void IUSV::SwitchSelector(int item)
{
	switch (item)
	{
	case 0:	//Liftoff (NOT A REAL SWITCH SELECTOR CHANNEL)
		fcc.SetGainSwitch(0);
		commandConnector.SetAGCInputChannelBit(030, LiftOff, true);
		break;
	case 1: //Q-Ball Power Off
		lvCommandConnector.SetQBallPowerOff();
		break;
	case 2: //Excess Rate (P,Y,R) Auto-Abort Inhibit and Switch Rate Gyro SC Indication "A"
		eds.SetExcessiveRatesAutoAbortInhibit(true);
		eds.SetRateGyroSCIndicationSwitchA(true);
		break;
	case 3: //Tape Recorder Playback Reverse Off
		break;
	case 4: //Flight Control Computer Switch Point No. 4
		fcc.SetGainSwitch(4);
		break;
	case 5: //Flight Control Computer Switch Point No. 6
		fcc.SetGainSwitch(6);
		break;
	case 9: //S-IVB Engine Out Indication "A" Enable
		eds.SetSIVBEngineOutIndicationA(true);
		break;
	case 11: //S-IVB Engine Out Indication "B" Enable
		eds.SetSIVBEngineOutIndicationB(true);
		break;
	case 12: //Flight Control Computer S-IVB Burn Mode Off "A"
		fcc.SetSIVBBurnMode(false);
		break;
	case 15: //Excess Rate (P,Y,R) Auto-Abort Inhibit Enable
		break;
	case 16: //Auto-Abort Enable Relays Reset
		eds.ResetAutoAbortRelays();
		break;
	case 17: //Tape Recorder Record Off
		break;
	case 18: //S-IVB Engine Out Indication "A" Enable Reset
		eds.SetSIVBEngineOutIndicationA(false);
		break;
	case 19: //Tape Recorder Playback Reverse On
		break;
	case 21: //Flight Control Computer Switch Pointer No. 2
		fcc.SetGainSwitch(2);
		break;
	case 22: //Flight Control Computer Switch Pointer No. 3
		fcc.SetGainSwitch(3);
		break;
	case 23: //Telemetry Calibrator Inflight Calibrate On
		break;
	case 24: //Telemetry Calibrator Inflight Calibrate Off
		break;
	case 26: //Flight Control Computer Switch Pointer No. 1
		fcc.SetGainSwitch(1);
		break;
	case 28: //S-II Engine Out Indication "A" Enable; S-II Aft Interstage Separation Indication "A" Enable
		eds.SetSIIEngineOutIndicationA(true);
		break;
	case 29: //S-IVB Engine EDS Cutoff No. 1 Disable
		eds.SetSIVBEngineCutoffDisabled();
		break;
	case 31: //Flight Control Computer Burn Mode On "A"
		fcc.SetStageSwitch(2);
		fcc.SetSIVBBurnMode(true);
		break;
	case 33: //Switch Engine Control to S-II and S-IC Outboard Engine Cant Off "A"
		fcc.SetStageSwitch(1);
		break;
	case 34: //Excess Rate (Roll) Auto-Abort Inhibit Enable
		break;
	case 35: //S-IC Two Engines Out Auto-Abort Inhibit
		eds.SetTwoEngOutAutoAbortInhibit(true);
		break;
	case 38: //Launch Vehicle Engines EDS Cutoff Enable
		eds.SetLVEnginesCutoffEnable(true);
		break;
	case 39: //Tape Recorder Record On
		break;
	case 43: // S-IVB Ullage Thrust Present Indication On
		commandConnector.SetAGCInputChannelBit(030, UllageThrust, true);
		break;
	case 44: //Flight Control Computer Switch Point No. 5
		fcc.SetGainSwitch(5);
		break;
	case 46: //S-IVB Ullage Thrust Present Indication Off
		commandConnector.SetAGCInputChannelBit(030, UllageThrust, false);
		break;
	case 48: //S-II Engine Out Indication "B" Enable; S-II Aft Interstage Separation Indication "B" Enable
		eds.SetSIIEngineOutIndicationB(true);
		break;
	case 50: //Excess Rate (Roll) Auto-Abort Inhibit and Switch Rate Gyro SC Indication "B"
		eds.SetExcessiveRatesAutoAbortInhibit(true);
		eds.SetRateGyroSCIndicationSwitchB(true);
		break;
	case 51: //S-IC Two Engines Out Auto-Abort Inhibit Enable
		break;
	case 53: //S-IVB Engine Out Indication "B" Enable Reset
		eds.SetSIVBEngineOutIndicationB(false);
		break;
	case 60: //PCM Coax Switch Low Gain Antenna
		break;
	case 62: //PCM Coax Switch High Gain Antenna
		break;
	case 63: //CCS Coax Switch High Gain Antenna
		break;
	case 65: //CCS Coax Switch Low Gain Antenna
		break;
	case 68: //S/C Control of Saturn Enable
		fcc.EnableSCControl();
		break;
	case 69: //S/C Control of Saturn Disable
		fcc.DisableSCControl();
		break;
	case 74: //Flight Control Computer Burn Mode On "B"
		fcc.SetStageSwitch(2);
		fcc.SetSIVBBurnMode(true);
		break;
	case 75: //Flight Control Computer S-IVB Burn Mode Off "B"
		fcc.SetSIVBBurnMode(false);
		break;
	case 80: //S-IVB Restart Alert On
		commandConnector.SetSIISep();
		break;
	case 81: //S-IVB Restart Alert Off
		commandConnector.ClearSIISep();
		break;
	case 82: //IU Command System Enable
		break;
	case 83: //S-IC Outboard Engines Cant On "A"
		break;
	case 84: //S-IC Outboard Engines Cant On "B"
		break;
	case 85: //S-IC Outboard Engines Cant On "C"
		break;
	case 86: //S-IC Outboard Engines Cant Off "B"
		break;
	case 106: //S-I RF Assembly Power Off
		break;
	case 107: //Water Coolant Valve Open
		break;
	case 108: //Water Coolant Valve Closed
		break;
	case 109: //Sensor Bias On
		break;
	case 110: //Cooling System Electronic Assembly Power Off
		break;
	default:
		break;
	}
}