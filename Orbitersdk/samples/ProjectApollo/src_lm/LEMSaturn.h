/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

LEM Saturn (Header)

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

class IU;
class SIBSystems;
class SIVBSystems;

class LEMSaturn;

class LEMSaturnConnector : public Connector
{
public:
	LEMSaturnConnector(LEMSaturn *l);
	~LEMSaturnConnector();

	void SetLEM(LEMSaturn *lem) { OurVessel = lem; };

protected:
	LEMSaturn * OurVessel;
};

class LEMSaturnToIUCommandConnector : public LEMSaturnConnector
{
public:
	LEMSaturnToIUCommandConnector(LEMSaturn *l);
	~LEMSaturnToIUCommandConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

class LEMSaturn : public LEM {
	friend class LVDC1B;
public:
	LEMSaturn(OBJHANDLE hObj, int fmodel);
	virtual ~LEMSaturn();

	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkLoadStateEx(FILEHANDLE scn, void *vs);

	void SeparateStage(UINT stage);
	void SetStage(int s);

	virtual void SetSIEngineStart(int eng);
	virtual void SetIUUmbilicalState(bool connect);

	void PlayCountSound(bool StartStop);
	void PlaySepsSound(bool StartStop);

	double GetJ2ThrustLevel();
	int GetStage() { return lemsat_stage; };
	void GetSIThrustOK(bool *ok);
	bool GetSIVBThrustOK();
	bool GetSIPropellantDepletionEngineCutoff();
	bool GetSIInboardEngineOut();
	bool GetSIOutboardEngineOut();
	bool GetSIBLowLevelSensorsDry();
	double GetFirstStageThrust() { return THRUST_FIRST_VAC; }
	void SwitchSelector(int item);
	void SISwitchSelector(int channel);
	void SIVBSwitchSelector(int channel);
	void SetAPSAttitudeEngine(int n, bool on);
	void SIEDSCutoff(bool cut);
	void SIVBEDSCutoff(bool cut);
	void SetSIThrusterDir(int n, double yaw, double pitch);
	void SetSIVBThrusterDir(double yaw, double pitch);
	void AddRCS_S4B();
	void ActivatePrelaunchVenting();
	void DeactivatePrelaunchVenting();
	void JettisonNosecap();
	void SetSLADeployCommand() { DeploySLACommand = true; }

protected:

	void initSaturn1b();
	void ClearThrusters();
	void CalculateStageMass();
	void SetFirstStage();
	void SetFirstStageMeshes(double offset);
	void SetFirstStageEngines();
	void SetSecondStage();
	void SetSecondStageMeshes(double offset);
	void SetSecondStageEngines();

	void CreateStageOne();
	void CreateSIVBStage(char *config, VESSELSTATUS &vs1);

	void CreateSIBSystems();
	void CreateSIVBSystems();
	void CreateIUSystems();

	void SetNosecapMesh();
	void SetupMeshes();
	void Saturn1bLoadMeshes();

	void GetApolloName(char *s);

	void SaveLEMSaturn(FILEHANDLE scn);
	void LoadLEMSaturn(FILEHANDLE scn);
	void LoadSIB(FILEHANDLE scn);
	void LoadSIVB(FILEHANDLE scn);
	void LoadIU(FILEHANDLE scn);
	void LoadLVDC(FILEHANDLE scn);

	int lemsat_stage;

	bool NosecapAttached;

	double ISP_FIRST_VAC, ISP_FIRST_SL, ISP_SECOND_VAC, ISP_SECOND_SL;
	double THRUST_FIRST_VAC, THRUST_SECOND_VAC;

	double SI_EmptyMass, SI_FuelMass;
	double SIVB_EmptyMass, SIVB_FuelMass;
	double SI_Mass, SIVB_Mass, LM_Mass;
	double Stage1Mass, Stage2Mass;

	double Offset1st;
	double TCPO;

	double contrailLevel;

	MESHHANDLE hStage1Mesh;
	MESHHANDLE hStage2Mesh;
	MESHHANDLE hInterstageMesh;
	MESHHANDLE hStageSLA1Mesh;
	MESHHANDLE hStageSLA2Mesh;
	MESHHANDLE hStageSLA3Mesh;
	MESHHANDLE hStageSLA4Mesh;

	// Mesh indexes
	int nosecapidx;
	int meshLM_1;
	int panelMesh1Saturn1b, panelMesh2Saturn1b, panelMesh3Saturn1b, panelMesh4Saturn1b;

	// Animation
	UINT panelAnim;
	double panelProc;
	double RotationLimit;
	bool DeploySLACommand;
	bool SLADeployed;

	//
	// Surfaces.
	//
	SURFHANDLE J2Tex;
	SURFHANDLE SIVBRCSTex;

	//
	// Vessel handles.
	//

	OBJHANDLE hstg1;
	OBJHANDLE habort;
	OBJHANDLE hs4bM;
	OBJHANDLE hNosecapVessel;

	//
	// Sounds
	//

	Sound LaunchS;
	Sound SShutS;
	Sound Scount;
	Sound SepS;

	THRUSTER_HANDLE th_1st[8], th_3rd[1], th_3rd_lox, th_ver[3];
	THRUSTER_HANDLE th_aps_rot[6];
	THGROUP_HANDLE thg_1st, thg_3rd, thg_ver;

	PROPELLANT_HANDLE ph_1st, ph_3rd, ph_ullage3;
	PROPELLANT_HANDLE ph_aps1, ph_aps2;

	PSTREAM_HANDLE prelaunchvent[3];

	IU* iu;
	SIBSystems *sib;
	SIVBSystems *sivb;

	Pyro SIBSIVBSepPyros;

	///
	/// \brief Connector from LM to IU.
	///
	LMToIUConnector iuCommandConnector;
	LEMSaturnToIUCommandConnector sivbCommandConnector;
};

extern void StageTransform(VESSEL *vessel, VESSELSTATUS *vs, VECTOR3 ofs, VECTOR3 vel);

const double STG0O = 0;
const double STG1O = 10.25;
const double STG2O = 8;
const VECTOR3 OFS_STAGE1 = { 0, 0, -14 };

#define SISYSTEMS_START_STRING		"SISYSTEMS_BEGIN"
#define SISYSTEMS_END_STRING		"SISYSTEMS_END"