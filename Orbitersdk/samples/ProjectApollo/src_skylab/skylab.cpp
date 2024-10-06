/**************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2023 Matthew Hume


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

  See https://github.com/orbiternassp/NASSP/blob/Orbiter2016/NASSP-LICENSE.txt
  for more details.

 **************************************************************************/
#define ORBITER_MODULE
#include "skylab.h"

const double TACS_PROPELLANT_MASS = 646.8227; //1426 lbm
const double TACS_MAX_THRUST = 100.0*4.4482216152605; //100 lbf
const double TACS_SPECIFIC_IMPULSE = 790.86; //In m/s. Calculated from 115,000 lbf-sec maximum total impulse, other source says 66 (cold) to 78 (hot) seconds of specific impulse


Skylab::Skylab(OBJHANDLE hObj, int fmodel): ProjectApolloConnectorVessel(hObj, fmodel),
atmdc(this),
skylabanimations(this)
{
	csm = NULL;
}

Skylab::~Skylab() {

}

void Skylab::InitSkylab() {
	skylabmesh = oapiLoadMeshGlobal("ProjectApollo/Skylab1973/Skylab I");
	skylabmeshID = AddMesh(skylabmesh, &MeshOffset);
	SetMeshVisibilityMode(skylabmeshID, MESHVIS_ALWAYS);
	skylabanimations.DefineAnimations();

	visibilitySize = 31.1; //Tuned so Skylab disappears in the CSM optics at 400nm range

	if (oapiGetFocusObject() == GetHandle()) { SetSize(15); }
	else { SetSize(visibilitySize); }

	RegisterConnector(VIRTUAL_CONNECTOR_PORT, &skylab_vhf2csm_vhf_connector);
}

void Skylab::clbkPostCreation() {
	InitSkylab();
	ShiftCG(_V(0.066,0.6198,-6.1392)); //Initial CoM Relative to Vessel Coordinate System (Y,Z,X) in skylab coordinates
	skylabanimations.SetATMAnimationState(1.0);
	skylabanimations.SetATMArrayAnimationState(0, 1.0);
	skylabanimations.SetATMArrayAnimationState(1, 1.0);
	skylabanimations.SetATMArrayAnimationState(2, 1.0);
	skylabanimations.SetATMArrayAnimationState(3, 1.0);
	skylabanimations.SetATMArrayAnimationState(4, 1.0);
}

void Skylab::clbkPreStep(double simt, double simdt, double mjd)
{
	atmdc.Timestep();

	//Thrusters
	double max_thr = GetPropellantMass(ph_tacs) / GetPropellantMaxMass(ph_tacs); //Thrust is nearly a linear function of propellant pressure
	for (int i = 0; i < 6; i++)
	{
		SetThrusterLevel(th_tacs[i], atmdc.GetThrusterDemand(i) ? max_thr : 0.0);
	}

	//Communications

	if (skylab_vhf2csm_vhf_connector.connectedTo)
	{
		skylab_vhf2csm_vhf_connector.SendRF(296.8E6, 5, 10, 0, true);
	}
	else
	{
		skylab_vhf2csm_vhf_connector.ConnectTo(GetVesselConnector(csm, VIRTUAL_CONNECTOR_PORT, VHF_RNG));
	}

	//sprintf(oapiDebugString(), "size %0.1f", GetSize());
}

void Skylab::clbkSetClassCaps(FILEHANDLE cfg)
{
	VESSEL4::clbkSetClassCaps(cfg);
	ClearExhaustRefs();
	ClearAttExhaustRefs();
	ClearThrusterDefinitions();

	SetSize(15);
	//Mass
	double mass = 88474; //https://ntrs.nasa.gov/api/citations/19730025115/downloads/19730025115.pdf#page=189
	SetEmptyMass(mass);
	//Principal Moment of Inertia
	//https://ntrs.nasa.gov/api/citations/19770014164/downloads/19770014164.pdf
	double PMI_X = 7.93321E5 / mass;
	double PMI_Y = 3.767828E6 / mass;
	double PMI_Z = 3.694680E6 / mass;
	SetPMI(_V(PMI_Y, PMI_Z, PMI_X));
	
	//Rough Drag Properties
	SetCrossSections(_V(79.46, 79.46, 79.46)); //From Skylab Operational Databook
	SetCW(2.401*2,2.590*2,4.384*2,9.330*2); //From Skylab Operational Databook, oversimplification
	SetRotDrag(_V(0.7, 0.7, 1.2)); //complete fabrication...

	//Propellant and thrusters
	AddTACS();
}

void Skylab::clbkSaveState(FILEHANDLE scn)
{
	VESSEL4::clbkSaveState(scn);

	if (csm) oapiWriteScenario_string(scn, "ONAME", csm->GetName());

	atmdc.SaveState(scn);
}

void Skylab::clbkLoadStateEx(FILEHANDLE scn, void *vstatus)
{
	char *line;

	while (oapiReadScenario_nextline(scn, line))
	{
		if (!strnicmp(line, "ONAME", 5))
		{
			char temp[64];
			strncpy(temp, line + 6, 64);

			OBJHANDLE hVessel = oapiGetVesselByName(temp);
			if (hVessel != NULL) csm = oapiGetVesselInterface(hVessel);
		}
		else if (!strnicmp(line, ATMDC_START_STRING, sizeof(ATMDC_START_STRING)))
		{
			atmdc.LoadState(scn);
		}
		else
		{
			ParseScenarioLineEx(line, vstatus);
		}
	}

	if (oapiGetFocusObject() == GetHandle()) { SetSize(15); }
	else { SetSize(visibilitySize); }
}

bool Skylab::clbkDrawHUD(int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad *skp)
{
	//Display scaling code by BrianJ
	int s = hps->H;
	double d = (s*0.00130208);

	int sw = ((hps->W));
	int lw = (int)(16 * sw / 1024);
	int lwoffset = sw - (18 * lw);
	int hlw = (int)(lw / 2);

	int roxl = 0;
	int royl = 0;

	double ds = s;
	double dsw = sw;
	double sc_ratio = ds / dsw;

	if (sc_ratio < 0.7284)
	{
		roxl = (lw * 10);
		royl = (int)(-88 * d);
	}

	int w0 = (int)(184 * d);
	int w1 = (int)(200 * d);

	skp->SetTextColor(0x0066FF66);

	skp->Text((10 + roxl), (w0 + royl), "Attitude Control Mode:", 22);

	char abuf[256];

	switch (atmdc.GetAttitudeControlMode())
	{
	case 0:
		sprintf(abuf, "Free Drift");
		break;
	case 1:
		sprintf(abuf, "Attitude Hold");
		break;
	case 2:
		sprintf(abuf, "Solar Inertial");
		break;
	case 3:
		sprintf(abuf, "Local Vertical (+VV)");
		break;
	case 4:
		sprintf(abuf, "Local Vertical (-VV)");
		break;
	case 5:
		sprintf(abuf, "Manual");
		break;
	default:
		sprintf(abuf, "None");
		break;
	}
	
	skp->Text((10 + roxl), (w1 + royl), abuf, strlen(abuf));

	return true;
}

int Skylab::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate)
{
	if (!down) return 0; //Only process keydown events

	if (KEYMOD_SHIFT(kstate))
	{

	}
	else if (KEYMOD_ALT(kstate))
	{

	}
	else if (KEYMOD_CONTROL(kstate))
	{

	}
	else { //unmodified keys
		switch (key)
		{
		case OAPI_KEY_A: //Attitude control mode
			{
				int state = atmdc.GetAttitudeControlMode();
	
				if (state < 5)
				{
				state++;
				}
				else
				{
					state = 0;
				}
				atmdc.SetAttitudeControlMode(state);
				return 1;
			}
		}
	}
	return 0;
}

void Skylab::clbkFocusChanged(bool getfocus, OBJHANDLE hNewVessel, OBJHANDLE hOldVessel)
{
	OBJHANDLE hSkylab = GetHandle();
	if (hNewVessel == hSkylab) { //Skylab gains focus

		bool fixCamera = false;
		if (oapiCameraInternal() == false) {
			fixCamera = true;
			oapiCameraAttach(hSkylab, 0);
		}

		SetSize(15);

		if (fixCamera == true) {
			oapiCameraAttach(hSkylab, 1);
		}
	}
	else if (hOldVessel == hSkylab) { //S-IVB loses focus
		SetSize(visibilitySize);
	}
}

void Skylab::clbkGetRadiationForce(const VECTOR3& mflux, VECTOR3& F, VECTOR3& pos)
{
	double size = 15;
	double cs = size * size;  // simplified cross section
	double albedo = 1.5;    // simplistic albedo (mixture of absorption, reflection)

	F = mflux * (cs * albedo);
	pos = _V(0, 0, 0);        // don't induce torque
}

void Skylab::AddTACS()
{
	ph_tacs = CreatePropellantResource(TACS_PROPELLANT_MASS);

	th_tacs[0] = CreateThruster(_V(0.028429, 3.257629, -19.582054), _V(-0.008727, -0.999962, -0.000000), TACS_MAX_THRUST, ph_tacs, TACS_SPECIFIC_IMPULSE);
	th_tacs[1] = CreateThruster(_V(0.172895, 3.374154, -19.975754), _V(-0.998690, 0.051174, -0.000000), TACS_MAX_THRUST, ph_tacs, TACS_SPECIFIC_IMPULSE);
	th_tacs[2] = CreateThruster(_V(-0.113982, 3.376658, -20.007504), _V(0.999431, -0.033737, -0.000000), TACS_MAX_THRUST, ph_tacs, TACS_SPECIFIC_IMPULSE);
	th_tacs[3] = CreateThruster(_V(-0.028429, -3.257629, -19.582054), _V(0.008727, 0.999962, -0.000000), TACS_MAX_THRUST, ph_tacs, TACS_SPECIFIC_IMPULSE);
	th_tacs[4] = CreateThruster(_V(0.113982, -3.376658, -19.975754), _V(-0.999431, 0.033737, -0.000000), TACS_MAX_THRUST, ph_tacs, TACS_SPECIFIC_IMPULSE);
	th_tacs[5] = CreateThruster(_V(-0.172895, -3.374154, -20.007504), _V(0.998690, -0.051174, -0.000000), TACS_MAX_THRUST, ph_tacs, TACS_SPECIFIC_IMPULSE);

	SURFHANDLE TACSTex = oapiRegisterExhaustTexture("ProjectApollo/exhaust_atrcs");

	for (int i = 0; i < 6; i++)
	{
		AddExhaust(th_tacs[i], 0.6, 0.078, TACSTex);
	}
}

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	VESSEL* skylab;
	skylab = new Skylab(hvessel, flightmodel);

	return skylab;
}


DLLCLBK void ovcExit(VESSEL* vessel)
{
	if (vessel) {
		delete (Skylab*)vessel;
	}
}


