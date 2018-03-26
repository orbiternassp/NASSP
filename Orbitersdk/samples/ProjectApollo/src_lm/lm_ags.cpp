/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  LM Abort Guidance System

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
#include "lmresource.h"

#include "yaAGS/aea_engine.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"

#include "LEM.h"
#include "tracer.h"
#include "papi.h"
#include "CollisionSDK/CollisionSDK.h"

#include "connector.h"

static char ThreeSpace[] = "   ";
static char SixSpace[] = "      ";
static int SegmentCount[] = {6, 2, 5, 5, 4, 5, 6, 3, 7, 5 };

// Abort Sensor Assembly
LEM_ASA::LEM_ASA()// : hsink("LEM-ASA-HSink",_vector3(0.013, 3.0, 0.03),0.03,0.04),
	//heater("LEM-ASA-Heater",1,NULL,15,20,0,272,274,&hsink)
{
	lem = NULL;

	PowerSwitch = 0;
	fastheater = 0;
	fineheater = 0;
	hsink = 0;
	asaHeat = 0;

	Initialized = false;
	Operate = false;
	PulsesSent = false;
	CurrentRotationMatrix = _M(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	EulerAngles = _V(0.0, 0.0, 0.0);
	RemainingDeltaVel = _V(0.0, 0.0, 0.0);
	LastWeightAcceleration = _V(0.0, 0.0, 0.0);
	LastGlobalVel = _V(0.0, 0.0, 0.0);
	LastSimDT = -1.0;
}

void LEM_ASA::Init(LEM *l, ThreePosSwitch *s, Boiler *fastht, Boiler *fineht, h_Radiator *hr, h_HeatLoad *asah) {
	lem = l;
	PowerSwitch = s;
	fastheater = fastht;
	fineheater = fineht;
	hsink = hr;
	asaHeat = asah;
	// Therm setup
	hsink->isolation = 0.0000001;
	hsink->Area = 975.0425;
	//hsink.mass = 9389.36206;
	//hsink.SetTemp(270);
	if (lem != NULL) {
		fastheater->WireTo(&lem->SCS_ASA_CB);
		fineheater->WireTo(&lem->SCS_ASA_CB);
		//lem->Panelsdk.AddHydraulic(&hsink);
		//lem->Panelsdk.AddElectrical(&heater, false);
	}
}

void LEM_ASA::TurnOn()
{
	Operate = true;
}

void LEM_ASA::TurnOff()
{
	Operate = false;
	PulsesSent = false;
	Initialized = false;
}

void LEM_ASA::Timestep(double simdt){
	if(lem == NULL){ return; }
	// AGS OFF  = ASA heaters active (OFF mode)
	// AGS STBY = ASA fully active   (WARMUP mode, becomes OPERATE mode when temp allows)
	// ASA OPR  = ASA fully active   (ditto)

	// ASA is 11.5x8x5.125 inches and weighs 20.7 pounds
	// ASA draws 74 watts operating? Need more info

	// ASA wants to stay at 120F.
	// Fast Warmup can get the ASA from 30F to 116F in 40 minutes.
	// Fast Warmup is active below 116F.
	// At 116F the Fine Warmup circuit takes over and gets to 120F and maintains it to within 0.2 degree F

	// sprintf(oapiDebugString(),"ASA Temp: %f AH %f",hsink.Temp,heater.pumping);

	// Do we have an ASA?
	if (lem->NoAEA) return;

	if (IsHeaterPowered())
	{
		if (fastheater->pumping)
		{
			fineheater->SetPumpOff();
		}
		else
		{
			fineheater->SetPumpAuto();
		}
	}

	if (!Operate) {
		if (IsPowered())
			TurnOn();
		else
			return;
	}
	else if (!IsPowered()) {
		TurnOff();
		return;
	}

	if (!PulsesSent)
	{
		EulerAngles = _V(0.0, 0.0, 0.0);
		RemainingDeltaVel = _V(0.0, 0.0, 0.0);
	}

	PulsesSent = false;

	//ATTITUDE
	MATRIX3 Rotnew, dRot;
	VECTOR3 dEulerAngles;

	lem->GetRotationMatrix(Rotnew);

	dRot = mul(transpose_matrix(CurrentRotationMatrix), Rotnew);
	dEulerAngles = MatrixToEuler(dRot);
	EulerAngles += _V(dEulerAngles.y, dEulerAngles.x, dEulerAngles.z);

	CurrentRotationMatrix = Rotnew;

	if (!Initialized) 
	{
		// Get current weight vector in vessel coordinates
		VECTOR3 w;
		lem->GetWeightVector(w);
		// Transform to Orbiter global and calculate weight acceleration
		w = mul(CurrentRotationMatrix, w) / lem->GetMass();

		//Orbiter 2016 hack
		if (length(w) == 0.0)
		{
			w = GetGravityVector();
		}

		LastWeightAcceleration = w;

		lem->GetGlobalVel(LastGlobalVel);

		LastSimDT = simdt;
		Initialized = true;
	}
	else
	{
		//ACCELERATION

		// Calculate accelerations
		VECTOR3 w, vel;
		lem->GetWeightVector(w);
		// Transform to Orbiter global and calculate accelerations
		w = mul(CurrentRotationMatrix, w) / lem->GetMass();

		//Orbiter 2016 hack
		if (length(w) == 0.0)
		{
			w = GetGravityVector();
		}

		lem->GetGlobalVel(vel);
		VECTOR3 dvel = (vel - LastGlobalVel) / LastSimDT;

		// Measurements with the 2006-P1 version showed that the average of the weight 
		// vector of this and the last step match the force vector while in free fall
		// The force vector matches the global velocity change of the last timestep exactly,
		// but isn't used because GetForceVector isn't working while docked
		VECTOR3 dw1 = w - dvel;
		VECTOR3 dw2 = LastWeightAcceleration - dvel;
		VECTOR3 accel = -(dw1 + dw2) / 2.0;
		LastWeightAcceleration = w;
		LastGlobalVel = vel;

		accel = mul(transpose_matrix(CurrentRotationMatrix), accel);

		RemainingDeltaVel.y += accel.x * LastSimDT;
		RemainingDeltaVel.x += accel.y * LastSimDT;
		RemainingDeltaVel.z += accel.z * LastSimDT;

		LastSimDT = simdt;
	}
}

VECTOR3 LEM_ASA::GetGravityVector()
{
	OBJHANDLE gravref = lem->GetGravityRef();
	OBJHANDLE hSun = oapiGetObjectByName("Sun");
	VECTOR3 R, U_R;
	lem->GetRelativePos(gravref, R);
	U_R = unit(R);
	double r = length(R);
	VECTOR3 R_S, U_R_S;
	lem->GetRelativePos(hSun, R_S);
	U_R_S = unit(R_S);
	double r_S = length(R_S);
	double mu = GGRAV * oapiGetMass(gravref);
	double mu_S = GGRAV * oapiGetMass(hSun);
	int jcount = oapiGetPlanetJCoeffCount(gravref);
	double JCoeff[5];
	for (int i = 0; i < jcount; i++)
	{
		JCoeff[i] = oapiGetPlanetJCoeff(gravref, i);
	}
	double R_E = oapiGetSize(gravref);

	VECTOR3 a_dP;

	a_dP = -U_R;

	if (jcount > 0)
	{
		MATRIX3 mat;
		VECTOR3 U_Z;
		double costheta, P2, P3;

		oapiGetPlanetObliquityMatrix(gravref, &mat);
		U_Z = mul(mat, _V(0, 1, 0));

		costheta = dotp(U_R, U_Z);

		P2 = 3.0 * costheta;
		P3 = 0.5*(15.0*costheta*costheta - 3.0);
		a_dP += (U_R*P3 - U_Z * P2)*JCoeff[0] * pow(R_E / r, 2.0);
		if (jcount > 1)
		{
			double P4;
			P4 = 1.0 / 3.0*(7.0*costheta*P3 - 4.0*P2);
			a_dP += (U_R*P4 - U_Z * P3)*JCoeff[1] * pow(R_E / r, 3.0);
			if (jcount > 2)
			{
				double P5;
				P5 = 0.25*(9.0*costheta*P4 - 5.0 * P3);
				a_dP += (U_R*P5 - U_Z * P4)*JCoeff[2] * pow(R_E / r, 4.0);
			}
		}
	}
	a_dP *= mu / pow(r, 2.0);
	a_dP -= U_R_S * mu_S / pow(r_S, 2.0);

	if (gravref == oapiGetObjectByName("Moon"))
	{
		OBJHANDLE hEarth = oapiGetObjectByName("Earth");

		VECTOR3 R_Ea, U_R_E;
		lem->GetRelativePos(hEarth, R_Ea);
		U_R_E = unit(R_Ea);
		double r_E = length(R_Ea);
		double mu_E = GGRAV * oapiGetMass(hEarth);

		a_dP -= U_R_E * mu_E / pow(r_E, 2.0);
	}

	return a_dP;
}

void LEM_ASA::SystemTimestep(double simdt)
{
	if (IsPowered())
	{
		lem->SCS_ASA_CB.DrawPower(41.1);
		asaHeat->GenerateHeat(41.1);
	}
}

void LEM_ASA::PulseTimestep(int* ASAPulses)
{
	int i;

	for (i = 0;i < 3;i++)
	{
		ASAPulses[i] = 32 + (int)(EulerAngles.data[i] * AttPulsesScal);

		if (ASAPulses[i] > 61)
		{
			ASAPulses[i] = 61;
		}
		else if (ASAPulses[i] < 3)
		{
			ASAPulses[i] = 3;
		}

		EulerAngles.data[i] -= (1.0 / AttPulsesScal)*(ASAPulses[i] - 32);
	}

	for (i = 3;i < 6;i++)
	{
		ASAPulses[i] = 32 + (int)(RemainingDeltaVel.data[i - 3] * AccPulsesScal);

		if (ASAPulses[i] > 61)
		{
			ASAPulses[i] = 61;
		}
		else if (ASAPulses[i] < 3)
		{
			ASAPulses[i] = 3;
		}

		RemainingDeltaVel.data[i - 3] -= (1.0 / AccPulsesScal)*(ASAPulses[i] - 32);
	}

	PulsesSent = true;
}

MATRIX3 LEM_ASA::transpose_matrix(MATRIX3 a)
{
	MATRIX3 b;

	b = _M(a.m11, a.m21, a.m31, a.m12, a.m22, a.m32, a.m13, a.m23, a.m33);
	return b;
}

VECTOR3 LEM_ASA::MatrixToEuler(MATRIX3 mat)
{
	return _V(atan2(mat.m23, mat.m33), -asin(mat.m13), atan2(mat.m12, mat.m11));
}

bool LEM_ASA::IsHeaterPowered()
{
	if (lem->SCS_ASA_CB.Voltage() < SP_MIN_DCVOLTAGE) { return false; }

	return true;
}

bool LEM_ASA::IsPowered()
{
	// Do we have an ASA?
	if (lem->NoAEA) return false;

	if (lem->SCS_ASA_CB.Voltage() < SP_MIN_DCVOLTAGE) { return false; }
	if (PowerSwitch) {
		if (PowerSwitch->IsDown()) { return false; }
	}

	return true;
}

double LEM_ASA::GetASATempF() {

	return KelvinToFahrenheit(hsink->GetTemp());

}

double LEM_ASA::GetASA12V() {
	if (IsPowered())
		return 12.0;
	else if (GetASATempF() > 145.0)
		return 0;
	else
		return 0;
}

double LEM_ASA::GetASA28V() {
	if (IsPowered())
		return 28.0;
	else
		return 0;
}

double LEM_ASA::GetASAFreq() {
	if (IsPowered() && (lem->CDR_SCS_AEA_CB.Voltage() > SP_MIN_DCVOLTAGE || lem->SCS_AEA_CB.Voltage() > SP_MIN_DCVOLTAGE))
		return 400.0;
	else
		return 0;
}

void LEM_ASA::SaveState(FILEHANDLE scn,char *start_str,char *end_str)
{
	oapiWriteLine(scn, start_str);

	papiWriteScenario_mx(scn, "CURRENTROTATIONMATRIX", CurrentRotationMatrix);
	papiWriteScenario_vec(scn, "EULERANGLES", EulerAngles);
	papiWriteScenario_vec(scn, "LASTWEIGHTACCELERATION", LastWeightAcceleration);
	papiWriteScenario_vec(scn, "LASTGLOBALVEL", LastGlobalVel);
	papiWriteScenario_vec(scn, "REMAININGDELTAVEL", RemainingDeltaVel);
	papiWriteScenario_double(scn, "LASTSIMDT", LastSimDT);
	papiWriteScenario_bool(scn, "INITIALIZED", Initialized);
	papiWriteScenario_bool(scn, "OPERATE", Operate);
	papiWriteScenario_bool(scn, "PULSESSENT", PulsesSent);

	oapiWriteLine(scn, end_str);
}

void LEM_ASA::LoadState(FILEHANDLE scn,char *end_str)
{
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, sizeof(end_str)))
			break;

		papiReadScenario_mat(line, "CURRENTROTATIONMATRIX", CurrentRotationMatrix);
		papiReadScenario_vec(line, "EULERANGLES", EulerAngles);
		papiReadScenario_vec(line, "LASTWEIGHTACCELERATION", LastWeightAcceleration);
		papiReadScenario_vec(line, "LASTGLOBALVEL", LastGlobalVel);
		papiReadScenario_vec(line, "REMAININGDELTAVEL", RemainingDeltaVel);
		papiReadScenario_double(line, "LASTSIMDT", LastSimDT);
		papiReadScenario_bool(line, "INITIALIZED", Initialized);
		papiReadScenario_bool(line, "OPERATE", Operate);
		papiReadScenario_bool(line, "PULSESSENT", PulsesSent);
	}
}

// Abort Electronics Assembly
LEM_AEA::LEM_AEA(PanelSDK &p, LEM_DEDA &display) : DCPower(0, p), deda(display) {
	lem = NULL;	
	AEAInitialized = false;
	FlightProgram = 0;
	PowerSwitch = 0;
	aeaHeat = 0;
	secaeaHeat = 0;

	ASACycleCounter = 0;
	LastCycled = 0.0;
	for (int i = 0; i < MAX_OUTPUT_PORTS; i++)
		OutputPorts[i] = 0;

	sin_theta = 0.0;
	cos_theta = 0.0;
	sin_phi = 0.0;
	cos_phi = 0.0;
	sin_psi = 0.0;
	cos_psi = 0.0;
	AGSAttitudeError = _V(0.0, 0.0, 0.0);
	AGSLateralVelocity = 0.0;
	Altitude = 0.0;
	AltitudeRate = 0.0;

	//
	// Virtual AGS.
	//
	memset(&vags, 0, sizeof(vags));
	vags.ags_clientdata = this;
}

void LEM_AEA::Init(LEM *s, h_HeatLoad *aeah, h_HeatLoad *secaeah){
	lem = s;
	aeaHeat = aeah;
	secaeaHeat = secaeah;
}

void LEM_AEA::Timestep(double simt, double simdt){
	if(lem == NULL){ return; }

	if (!IsPowered()) return;

	int Delta, CycleCount = 0;

	int AsaPulses[6];

	if (LastCycled == 0) {					// Use simdt as difference if new run
		LastCycled = (simt - simdt);
	}

	long cycles = (long)((simt - LastCycled) / 0.0000009765625);						// Preserve the remainder

	while (CycleCount < cycles)
	{
		Delta = aea_engine(&vags);
		CycleCount += Delta;
		ASACycleCounter += Delta;

		//ASA cycle at 1 kHz
		if (ASACycleCounter >= 1024)
		{
			//TBD: This is just code for testing. Should be replaced by calling an AEA input channel function.

			lem->asa.PulseTimestep(AsaPulses);

			vags.InputPorts[IO_6002] += SignExtendAGS(AsaPulses[1]) * 0100;
			vags.InputPorts[IO_6002] &= 0377700;

			vags.InputPorts[IO_6004] += SignExtendAGS(AsaPulses[2]) * 0100;
			vags.InputPorts[IO_6004] &= 0377700;

			vags.InputPorts[IO_6010] += SignExtendAGS(AsaPulses[0]) * 0100;
			vags.InputPorts[IO_6010] &= 0377700;

			vags.InputPorts[IO_6020] += SignExtendAGS(AsaPulses[3]) * 0100;
			vags.InputPorts[IO_6020] &= 0377700;

			vags.InputPorts[IO_6040] += SignExtendAGS(AsaPulses[4]) * 0100;
			vags.InputPorts[IO_6040] &= 0377700;

			vags.InputPorts[IO_6100] += SignExtendAGS(AsaPulses[5]) * 0100;
			vags.InputPorts[IO_6100] &= 0377700;

			ASACycleCounter -= 1024;
		}
	}

	LastCycled += (0.0000009765625 * CycleCount);
}

void LEM_AEA::SystemTimestep(double simdt)
{
	if (IsPowered())
	{
		DCPower.DrawPower(47.0);
		aeaHeat->GenerateHeat(21.25);
		secaeaHeat->GenerateHeat(21.25);
	}

	if (IsACPowered())
	{
		lem->AGS_AC_CB.DrawPower(3.45);
		aeaHeat->GenerateHeat(1.725);
		secaeaHeat->GenerateHeat(1.725);
	}
}

void LEM_AEA::ResetDEDAShiftIn()
{
	unsigned int mask = (1 << (DEDAShiftIn));

	int	data = vags.OutputPorts[IO_ODISCRETES];

	data |= mask;

	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

	vags.OutputPorts[IO_ODISCRETES] = data;
}

void LEM_AEA::ResetDEDAShiftOut()
{
	unsigned int mask = (1 << (DEDAShiftOut));

	int	data = vags.OutputPorts[IO_ODISCRETES];

	data |= mask;

	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

	vags.OutputPorts[IO_ODISCRETES] = data;
}

void LEM_AEA::SetInputPort(int port, int val)
{
	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

	vags.InputPorts[port] = val;
}

void LEM_AEA::SetInputPortBit(int port, int bit, bool val)
{
	unsigned int mask = (1 << (bit));

	int	data = vags.InputPorts[port];

	if (port < 0 || port > MAX_INPUT_PORTS)
		return;

	if (val) {
		data |= mask;
	}
	else {
		data &= ~mask;
	}

	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

	vags.InputPorts[port] = data;
}

void LEM_AEA::SetOutputChannel(int Type, int Data)
{
	if (Type - MAX_INPUT_PORTS < 0 || Type - MAX_INPUT_PORTS >= MAX_OUTPUT_PORTS)
		return;

	OutputPorts[Type - MAX_INPUT_PORTS] = Data;

	switch (Type)
	{
		//sin theta
		//cos theta
		//sin phi
		//cos phi
		//sin psi
		//cos psi
	case 020:
	case 021:
	case 022:
	case 023:
	case 024:
	case 025:
		SetAGSAttitude(Type, Data);
		break;

	case 027:
		//DEDA
		deda.ProcessChannel27(Data);
		break;

		//E_X
		//E_Y
		//E_Z
	case 030:
	case 031:
	case 032:
		SetAGSAttitudeError(Type, Data);
		break;

	case 033:
		//Altitude, Altitude Rate
		SetAltitudeAltitudeRate(Data);
		break;

	case 034:
		//Lateral Velocity
		SetLateralVelocity(Data);
		break;

	case 040:
		//Output Discretes
		deda.ProcessChannel40(Data);
		break;
	}
}

unsigned int LEM_AEA::GetOutputChannel(int channel)

{
	if (channel < 0 || channel >= MAX_OUTPUT_PORTS)
		return 0;

	return OutputPorts[channel];
}

unsigned int LEM_AEA::GetInputChannel(int channel)

{
	if (channel < 0 || channel >= MAX_INPUT_PORTS)
		return 0;

	unsigned int val = vags.InputPorts[channel];

	return val;
}

void LEM_AEA::SetAGSAttitudeError(int Type, int Data)
{
	int DataVal;

	if (Data & 0400000) { // Negative
		DataVal = -((~Data) & 0777777);
		DataVal = -0400000 - DataVal;
	}
	else {
		DataVal = Data & 0777777;
	}

	double DataScaled = (double)DataVal*ATTITUDEERRORSCALEFACTOR;

	switch (Type)
	{
	case 030:
		//E_X
		AGSAttitudeError.x = DataScaled;
		break;

	case 031:
		//E_Y
		AGSAttitudeError.y = DataScaled;
		break;

	case 032:
		//E_Z
		AGSAttitudeError.z = DataScaled;
		break;
	}
}

void LEM_AEA::SetAGSAttitude(int Type, int Data)
{
	int DataVal;

	if (Data & 0400000) { // Negative
		DataVal = -((~Data) & 0777777);
		DataVal = -0400000 - DataVal;
	}
	else {
		DataVal = Data & 0777777;
	}

	double DataScaled = (double)DataVal*ATTITUDESCALEFACTOR;

	switch (Type)
	{
	case 020:
		//sin theta
		sin_theta = DataScaled;
		break;

	case 021:
		//cos theta
		cos_theta = DataScaled;
		break;

	case 022:
		//sin phi
		sin_phi = DataScaled;
		break;

	case 023:
		//cos phi
		cos_phi = DataScaled;
		break;

	case 024:
		//sin psi
		sin_psi = DataScaled;
		break;

	case 025:
		//cos psi
		cos_psi = DataScaled;
		break;
	}
}

void LEM_AEA::SetLateralVelocity(int Data)
{
	int DataVal;

	if (Data & 0400000) { // Negative
		DataVal = -((~Data) & 0777777);
		DataVal = -0400000 - DataVal;
	}
	else {
		DataVal = Data & 0777777;
	}

	AGSLateralVelocity = (double)DataVal*LATVELSCALEFACTOR;
}

void LEM_AEA::SetAltitudeAltitudeRate(int Data)
{
	int DataVal;

	AGSChannelValue40 val = GetOutputChannel(IO_ODISCRETES);

	if (val[AGSAltitude] == 0)
	{
		DataVal = Data & 0777777;

		Altitude = (double)DataVal*ALTSCALEFACTOR;
	}
	else if (val[AGSAltitudeRate] == 0)
	{
		if (Data & 0400000) { // Negative
			DataVal = -((~Data) & 0777777);
			DataVal = -0400000 - DataVal;
		}
		else {
			DataVal = Data & 0777777;
		}

		AltitudeRate = -(double)DataVal*ALTRATESCALEFACTOR;
	}
}

void LEM_AEA::SetPGNSIntegratorRegister(int channel, int val)
{
	int valx;

	if (channel == 032)
	{
		valx = SignExtendAGS(val) * 4;
		valx &= 0377774;
		SetInputPort(IO_2002, valx);
	}
	else if (channel == 033)
	{
		valx = SignExtendAGS(val) * 4;
		valx &= 0377774;
		SetInputPort(IO_2001, valx);
	}
	else if (channel == 034)
	{
		valx = SignExtendAGS(val) * 4;
		valx &= 0377774;
		SetInputPort(IO_2004, valx);
	}
}

VECTOR3 LEM_AEA::GetTotalAttitude()
{
	if (lem->AGS_AC_CB.Voltage() < SP_MIN_ACVOLTAGE)
	{
		return _V(0, 0, 0);
	}

	return _V(atan2(sin_theta, cos_theta), atan2(sin_psi, cos_psi), -atan2(sin_phi, cos_phi));
}

VECTOR3 LEM_AEA::GetAttitudeError()
{
	if (lem->AGS_AC_CB.Voltage() < SP_MIN_ACVOLTAGE)
	{
		return _V(0, 0, 0);
	}

	return _V(AGSAttitudeError.z, AGSAttitudeError.y, AGSAttitudeError.x);
}

double LEM_AEA::GetLateralVelocity()
{
	return AGSLateralVelocity;
}

double LEM_AEA::GetAltitude()
{
	return Altitude;
}

double LEM_AEA::GetAltitudeRate()
{
	return AltitudeRate;
}

void LEM_AEA::WireToBuses(e_object *a, e_object *b, ThreePosSwitch *s)

{
	DCPower.WireToBuses(a, b);
	PowerSwitch = s;
}

bool LEM_AEA::IsPowered()
{
	// Do we have an AEA?
	if (lem->NoAEA) return false;

	if (DCPower.Voltage() < SP_MIN_DCVOLTAGE) { return false; }
	if (PowerSwitch) {
		if (!PowerSwitch->IsUp()) { return false; }
	}

	return true;
}

bool LEM_AEA::IsACPowered()
{
	if (lem->AGS_AC_CB.Voltage() < SP_MIN_ACVOLTAGE) {	return false; }
	return true;
}

bool LEM_AEA::GetTestModeFailure()
{
		if (!IsPowered())
			return false;
		AGSChannelValue40 agsval40;
		agsval40 = OutputPorts[IO_ODISCRETES];
		return ~agsval40[AGSTestModeFailure];
}

void LEM_AEA::InitVirtualAGS(char *binfile)

{
	aea_engine_init(&vags, binfile, NULL);
}

void LEM_AEA::SetMissionInfo(int MissionNo)
{
	if (AEAInitialized) { return; }

	if (MissionNo < 13)
	{
		FlightProgram = 6;
	}
	else
	{
		FlightProgram = 8;
	}

	SetFlightProgram(FlightProgram);
}

void LEM_AEA::SetFlightProgram(int FP)
{
	char *binfile;

	// Flight Program 6
	binfile = "Config/ProjectApollo/FP6.bin";

	if (FP == 8)	//Flight Program 8
	{
		binfile = "Config/ProjectApollo/FP8.bin";
	}

	InitVirtualAGS(binfile);

	AEAInitialized = true;
}

void LEM_AEA::PadLoad(unsigned int address, unsigned int value)
{
	WriteMemory(address, value);
}

void LEM_AEA::WriteMemory(unsigned int loc, int val)

{
	int bank;

	bank = (loc / 04000);

	if (bank == 0)
		vags.Memory[loc] = val;
	return;
}

bool LEM_AEA::ReadMemory(unsigned int loc, int &val)

{
	int bank;

	bank = (loc / 04000);

	if (bank == 0) {
		val = vags.Memory[loc];
		return true;
	}

	val = 0;
	return true;

}

void LEM_AEA::SaveState(FILEHANDLE scn,char *start_str,char *end_str)
{
	oapiWriteLine(scn, start_str);

	char fname[32], str[32], buffer[256];
	int i;
	int val;

	//Has to be first!
	oapiWriteScenario_int(scn, "FLIGHTPROGRAM", FlightProgram);

	for (i = 0; i < AEA_MEM_ENTRIES; i++) {
		if (ReadMemory(i, val) && (val != 0)) {
			sprintf(fname, "MEM%04o", i);
			sprintf(str, "%o", val);
			oapiWriteScenario_string(scn, fname, str);
		}
	}

	for (i = 0; i < MAX_INPUT_PORTS; i++) {
		val = GetInputChannel(i);
		if (val != 0) {
			sprintf(fname, "ICHAN%02d", i);
			oapiWriteScenario_int(scn, fname, val);
		}
	}

	for (i = 0; i < MAX_OUTPUT_PORTS; i++) {
		val = GetOutputChannel(i);
		if (val != 0) {
			sprintf(fname, "OCHAN%02d", i);
			oapiWriteScenario_int(scn, fname, val);
		}
	}

	oapiWriteScenario_int(scn, "PROGRAMCOUNTER", vags.ProgramCounter);
	oapiWriteScenario_int(scn, "ACCUMULATOR", vags.Accumulator);
	oapiWriteScenario_int(scn, "QUOTIENT", vags.Quotient);
	oapiWriteScenario_int(scn, "INDEX", vags.Index);
	oapiWriteScenario_int(scn, "OVERFLOW", vags.Overflow);
	oapiWriteScenario_int(scn, "HALT", vags.Halt);

	sprintf(buffer, "  CYCLECOUNTER %I64d", vags.CycleCounter);
	oapiWriteLine(scn, buffer);

	sprintf(buffer, "  NEXT20MSSIGNAL %I64d", vags.Next20msSignal);
	oapiWriteLine(scn, buffer);

	oapiWriteScenario_int(scn, "ASACYCLECOUNTER", ASACycleCounter);
	papiWriteScenario_double(scn, "SIN_THETA", sin_theta);
	papiWriteScenario_double(scn, "COS_THETA", cos_theta);
	papiWriteScenario_double(scn, "SIN_PHI", sin_phi);
	papiWriteScenario_double(scn, "COS_PHI", cos_phi);
	papiWriteScenario_double(scn, "SIN_PSI", sin_psi);
	papiWriteScenario_double(scn, "COS_PSI", cos_psi);
	papiWriteScenario_vec(scn, "ATTITUDEERROR", AGSAttitudeError);
	papiWriteScenario_double(scn, "LATERALVELOCITY", AGSLateralVelocity);
	papiWriteScenario_double(scn, "ALTITUDE", Altitude);
	papiWriteScenario_double(scn, "ALTITUDERATE", AltitudeRate);

	oapiWriteLine(scn, end_str);
}

void LEM_AEA::LoadState(FILEHANDLE scn,char *end_str)
{
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, sizeof(end_str)))
			break;

		//Has to be first!
		if (!strnicmp(line, "FLIGHTPROGRAM", 13)) {
			sscanf(line + 13, "%d", &FlightProgram);
			SetFlightProgram(FlightProgram);
		}
		else if (!strnicmp(line, "MEM", 3)) {
			int num, val;
			sscanf(line + 3, "%o", &num);
			sscanf(line + 8, "%o", &val);
			WriteMemory(num, val);
		}
		else if (!strnicmp(line, "ICHAN", 5)) {
			int num;
			unsigned int val;
			sscanf(line + 5, "%d", &num);
			sscanf(line + 8, "%d", &val);
			vags.InputPorts[num] = val;
		}
		else if (!strnicmp(line, "OCHAN", 5)) {
			int num;
			unsigned int val;
			sscanf(line + 5, "%d", &num);
			sscanf(line + 8, "%d", &val);
			OutputPorts[num] = val;
		}

		papiReadScenario_int(line, "PROGRAMCOUNTER", vags.ProgramCounter);
		papiReadScenario_int(line, "ACCUMULATOR", vags.Accumulator);
		papiReadScenario_int(line, "QUOTIENT", vags.Quotient);
		papiReadScenario_int(line, "INDEX", vags.Index);
		papiReadScenario_int(line, "OVERFLOW", vags.Overflow);
		papiReadScenario_int(line, "HALT", vags.Halt);

		if (!strnicmp(line, "CYCLECOUNTER", 12)) {
			sscanf(line + 12, "%I64d", &vags.CycleCounter);
		}
		else if (!strnicmp(line, "NEXT20MSSIGNAL", 14)) {
			sscanf(line + 14, "%I64d", &vags.Next20msSignal);
		}

		papiReadScenario_int(line, "ASACYCLECOUNTER", ASACycleCounter);
		papiReadScenario_double(line, "SIN_THETA", sin_theta);
		papiReadScenario_double(line, "COS_THETA", cos_theta);
		papiReadScenario_double(line, "SIN_PHI", sin_phi);
		papiReadScenario_double(line, "COS_PHI", cos_phi);
		papiReadScenario_double(line, "SIN_PSI", sin_psi);
		papiReadScenario_double(line, "COS_PSI", cos_psi);
		papiReadScenario_vec(line, "ATTITUDEERROR", AGSAttitudeError);
		papiReadScenario_double(line, "LATERALVELOCITY", AGSLateralVelocity);
		papiReadScenario_double(line, "ALTITUDE", Altitude);
		papiReadScenario_double(line, "ALTITUDERATE", AltitudeRate);
	}
}

// Data Entry and Display Assembly

LEM_DEDA::LEM_DEDA(LEM *lm, SoundLib &s,LEM_AEA &computer) :  lem(lm), soundlib(s), ags(computer)
{
	Reset();
}

LEM_DEDA::~LEM_DEDA()
{
	//
	// Nothing for now.
	//
}

void LEM_DEDA::Init(e_object *powered)

{
	WireTo(powered);
	Reset();
	ResetKeyDown();
	FirstTimeStep = true;
}

void LEM_DEDA::Timestep(double simdt){
	if(lem == NULL){ return; }

	if(FirstTimeStep)
	{
		FirstTimeStep = false;
	    soundlib.LoadSound(Sclick, BUTTON_SOUND);
	}

	if (!IsPowered()) { return; }

	SetAddress();
	SetData();
}

void LEM_DEDA::ProcessChannel27(int val)
{
	if (State == 9)
	{
		State = 0;
	}
	ShiftRegister[State] = (val >> 13) & 017;
	State++;

	lem->aea.ResetDEDAShiftOut();
}

void LEM_DEDA::ProcessChannel40(AGSChannelValue40 val)
{
	if (val[DEDAShiftIn] == 0)
	{
		if (OprErrLit())
		{
			lem->aea.SetInputPort(IO_2200, 017 << 13);
		}
		else
		{
			lem->aea.SetInputPort(IO_2200, (ShiftRegister[0] & 017) << 13);

			for (int i = 0;i < 8;i++)
			{
				ShiftRegister[i] = ShiftRegister[i + 1];
			}
			ShiftRegister[8] = 017;
			if (State > 0)
			{
				State--;
			}
		}
		lem->aea.ResetDEDAShiftIn();
	}
}

void LEM_DEDA::SaveState(FILEHANDLE scn,char *start_str,char *end_str){
	oapiWriteLine(scn, start_str);

	papiWriteScenario_intarr(scn, "SHIFTREGISTER", ShiftRegister, 9);
	oapiWriteScenario_int(scn, "STATE", State);

	oapiWriteLine(scn, end_str);
}

void LEM_DEDA::LoadState(FILEHANDLE scn,char *end_str){
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, sizeof(end_str)))
			break;

		papiReadScenario_intarr(line, "SHIFTREGISTER", ShiftRegister, 9);
		papiReadScenario_int(line, "STATE", State);
	}
}

void LEM_DEDA::KeyClick()

{
	Sclick.play(NOLOOP, 255);
}

void LEM_DEDA::Reset()

{
	OprErrLight = false;
	LightsLit = 0;
	SegmentsLit = 0;
	State = 0;

	strcpy (Adr, ThreeSpace);
	strcpy (Data, SixSpace);

	for (int i = 0;i < 9;i++)
	{
		ShiftRegister[i] = 017;
	}
}

void LEM_DEDA::ResetKeyDown() 

{
	// Reset KeyDown-flags
	KeyDown_Plus = false;
	KeyDown_Minus = false;
	KeyDown_0 = false;
	KeyDown_1 = false;
	KeyDown_2 = false;
	KeyDown_3 = false;
	KeyDown_4 = false;
	KeyDown_5 = false;
	KeyDown_6 = false;
	KeyDown_7 = false;
	KeyDown_8 = false;
	KeyDown_9 = false;
	KeyDown_Clear = false;
	KeyDown_ReadOut = false;
	KeyDown_Enter = false;
	KeyDown_Hold = false;

	ags.SetInputPortBit(IO_2040, DEDAReadoutDiscrete, true);
	ags.SetInputPortBit(IO_2040, DEDAEnterDiscrete, true);
	ags.SetInputPortBit(IO_2040, DEDAHoldDiscrete, true);
	ags.SetInputPortBit(IO_2040, DEDAClearDiscrete, true);
}

void LEM_DEDA::SystemTimestep(double simdt)

{
	if (!IsPowered())
		return;
	
	// We will use a similar scan as the DSKY power consumption

	// The DSKY power consumption is a little bit hard to figure out. According 
	// to the Systems Handbook the complete interior lightning draws about 30W, so
	// we assume one DSKY draws 10W max, for now. We DO NOT rely on the render code to
	// track the number of lights that are lit, because during pause the still called render 
	// code causes wrong power loads
	//

	//
	// Check the lights.
	//

	SegmentsLit = 0;
	LightsLit = 0;
	if (OprErrLit()) LightsLit++;
	//
	// Check the segments
	//

	SegmentsLit += ThreeDigitDisplaySegmentsLit(Adr);
	SegmentsLit += SixDigitDisplaySegmentsLit(Data);

	// 10 lights with together max. 6W, 184 segments with together max. 4W  
	DrawPower((LightsLit * 0.6) + (SegmentsLit * 0.022));

	//sprintf(oapiDebugString(), "DSKY %f", (LightsLit * 0.6) + (SegmentsLit * 0.022));
}

int LEM_DEDA::ThreeDigitDisplaySegmentsLit(char *Str)

{
	int Curdigit, s = 0;

	if (Str[0] >= '0' && Str[0] <= '9') {
		Curdigit = Str[0] - '0';
		s += SegmentCount[Curdigit];
	}

	if (Str[1] >= '0' && Str[1] <= '9') {
		Curdigit = Str[1] - '0';
		s += SegmentCount[Curdigit];
	}

	if (Str[2] >= '0' && Str[2] <= '9') {
		Curdigit = Str[1] - '0';
		s += SegmentCount[Curdigit];
	}
	return s;
}

void LEM_DEDA::RenderThreeDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str)

{
	int Curdigit;

	if (Str[0] >= '0' && Str[0] <= '9') {
		Curdigit = Str[0] - '0';
		oapiBlt(surf,digits,dstx+0,dsty,19*Curdigit,0,19,21);
	}

	if (Str[1] >= '0' && Str[1] <= '9') {
		Curdigit = Str[1] - '0';
		oapiBlt(surf,digits,dstx+20,dsty,19*Curdigit,0,19,21);
	}

	if (Str[2] >= '0' && Str[2] <= '9') {
		Curdigit = Str[2] - '0';
		oapiBlt(surf,digits,dstx+39,dsty,19*Curdigit,0,19,21);
	}
}

int LEM_DEDA::SixDigitDisplaySegmentsLit(char *Str)
{
	int	Curdigit;
	int i, s = 0;

	if (Str[0] == '-') 
		s += 1;
	else if (Str[0] == '+') 
		s += 2;

	for (i = 1; i < 6; i++) {
		if (Str[i] >= '0' && Str[i] <= '9') {
			Curdigit = Str[i] - '0';
			s += SegmentCount[Curdigit];
		}
	}
	return s;
}

void LEM_DEDA::RenderSixDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str)

{
	int	Curdigit;
	int i;

	if (Str[0] == '-') {
		oapiBlt(surf,digits,dstx+4,dsty,191,0,19,21);
	}
	else if (Str[0] == '+') {
		oapiBlt(surf,digits,dstx+4,dsty,210,0,19,21);
	}

	for (i = 1; i < 6; i++) {
		if (Str[i] >= '0' && Str[i] <= '9') {
			Curdigit = Str[i] - '0';
			oapiBlt(surf, digits, dstx + (20*i)+ 4, dsty, 19*Curdigit, 0, 19,21);
		}
		else {
//			oapiBlt(surf, digits, dstx + (10*i), dsty, 440, 6, 10, 15);
		}
	}
}


void LEM_DEDA::RenderAdr(SURFHANDLE surf, SURFHANDLE digits, int xOffset, int yOffset)

{
	if (!IsPowered())
		return;

	RenderThreeDigitDisplay(surf, digits, xOffset, yOffset, Adr);
}

void LEM_DEDA::RenderData(SURFHANDLE surf, SURFHANDLE digits, int xOffset, int yOffset)

{
	if (!IsPowered())
		return;

	//
	// Register contents.
	//

	RenderSixDigitDisplay(surf, digits, xOffset, yOffset, Data);
}

void LEM_DEDA::RenderKeys(SURFHANDLE surf, SURFHANDLE keys, int xOffset, int yOffset)

{
	DEDAKeyBlt(surf, keys, 1 + 44 * 0, 1,  44 * 0, 0,  KeyDown_Plus, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 0, 45, 44 * 0, 44, KeyDown_Minus, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 0, 90, 44 * 0, 88, KeyDown_0, xOffset, yOffset);

	DEDAKeyBlt(surf, keys, 1 + 44 * 1, 1,  44 * 1, 0,  KeyDown_7, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 1, 45, 44 * 1, 44, KeyDown_4, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 1, 90, 44 * 1, 88, KeyDown_1, xOffset, yOffset);

	DEDAKeyBlt(surf, keys, 1 + 44 * 2, 1,  44 * 2, 0,  KeyDown_8, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 2, 45, 44 * 2, 44, KeyDown_5, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 2, 90, 44 * 2, 88, KeyDown_2, xOffset, yOffset);

	DEDAKeyBlt(surf, keys, 1 + 44 * 3, 1,  44 * 3, 0,  KeyDown_9, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 3, 45, 44 * 3, 44, KeyDown_6, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 3, 90, 44 * 3, 88, KeyDown_3, xOffset, yOffset);

	DEDAKeyBlt(surf, keys, 1 + 44 * 4, 1,  44 * 4, 0,  KeyDown_Clear, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 4, 45, 44 * 4, 44, KeyDown_ReadOut, xOffset, yOffset);
    DEDAKeyBlt(surf, keys, 1 + 44 * 4, 90, 44 * 4, 88, KeyDown_Enter, xOffset, yOffset);

	DEDAKeyBlt(surf, keys, 1 + 44 * 2, 134, 44 * 2, 132, KeyDown_Hold, xOffset, yOffset);
}

void LEM_DEDA::DEDAKeyBlt(SURFHANDLE surf, SURFHANDLE keys, int dstx, int dsty, int srcx, int srcy, bool lit, int xOffset, int yOffset) 

{
	if (lit) {
		oapiBlt(surf, keys, dstx + xOffset, dsty + yOffset, srcx, srcy, 40, 40);
	}
	else {
		oapiBlt(surf, keys, dstx + xOffset, dsty + yOffset, srcx, srcy + 173, 40, 40);
	}
}


void LEM_DEDA::RenderOprErr(SURFHANDLE surf, SURFHANDLE lights)

{
	if (!IsPowered())
		return;

	//
	// Check the lights.
	//

	if (OprErrLit()) {
		oapiBlt(surf, lights, 0, 0, 46, 0, 45, 25);
	}
	else {
		oapiBlt(surf, lights, 0, 0, 0, 0, 45, 25);
	}

}


//
// Process a keypress based on the X and Y coords.
//

void LEM_DEDA::ProcessKeyPress(int mx, int my)

{
	KeyClick();

	if (mx > 2+0*44 && mx < 43+0*44) {

		if (my > 1 && my < 43) {
			KeyDown_Plus = true;
			if (OprErrLit() || !IsPowered())
					return;
			PlusPressed();
		}
		if (my > 44 && my < 88) {
			KeyDown_Minus = true;
			if (OprErrLit() || !IsPowered())
					return;
			MinusPressed();
		}
		if (my > 88 && my < 132) {
			KeyDown_0 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(0);
		}
	}

	if (mx > 2+1*43 && mx < 43+1*44) {
		if (my > 1 && my < 43) {
			KeyDown_7 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(7);
		}
		if (my > 44 && my < 88) {
			KeyDown_4 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(4);
		}
		if (my > 88 && my < 132) {
			KeyDown_1 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(1);
		}
	}

	if (mx > 2+2*44 && mx < 43+2*44) {
		if (my > 1 && my < 43) {
			KeyDown_8 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(8);
		}
		if (my > 44 && my < 88) {
			KeyDown_5 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(5);
		}
		if (my > 88 && my < 132) {
			KeyDown_2 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(2);
		}

		if (my > 132 && my < 176) {
			KeyDown_Hold = true;
			if (OprErrLit() || !IsPowered())
					return;
			HoldPressed();
		}
	}

	if (mx > 2+3*44 && mx < 43+3*44) {
		if (my > 1 && my < 43) {
			KeyDown_9 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(9);
		}
		if (my > 44 && my < 88) {
			KeyDown_6 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(6);
		}
		if (my > 88 && my < 132) {
			KeyDown_3 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(3);
		}
	}

	if (mx > 2+4*44 && mx < 43+4*44) {
		if (my > 1 && my < 43) {
			KeyDown_Clear = true;
//			ClearPressed();
		}
		if (my > 44 && my < 88) {
			KeyDown_ReadOut = true;
			if (OprErrLit() || !IsPowered())
					return;
			ReadOutPressed();
		}

		if (my > 88 && my < 132) {
			KeyDown_Enter = true;
			if (OprErrLit() || !IsPowered())
					return;
			EnterPressed();
		}
	}
}

void LEM_DEDA::ProcessKeyRelease(int mx, int my)

{
	if (mx > 2+4*44 && mx < 43+4*44) {
		if (my > 1 && my < 43) {
			ClearPressed();
		}
	}
	ResetKeyDown();
}

void LEM_DEDA::SendKeyCode(int val)

{
	//READOUT
	if (val == 10)
	{
		ags.SetInputPortBit(IO_2040, DEDAReadoutDiscrete, false);
	}
	//ENTR
	else if (val == 11)
	{
		ags.SetInputPortBit(IO_2040, DEDAEnterDiscrete, false);
	}
	//HOLD
	else if (val == 12)
	{
		ags.SetInputPortBit(IO_2040, DEDAHoldDiscrete, false);
	}
	//CLEAR
	else if (val == 13)
	{
		ags.SetInputPortBit(IO_2040, DEDAClearDiscrete, false);
	}
}

void LEM_DEDA::EnterPressed()
{
	KeyClick();

	if (State == 9)
	{
		SendKeyCode(11);
	}
	else
		SetOprErr(true);
}

void LEM_DEDA::ClearPressed()
{
	KeyClick();

	Reset();
	ResetKeyDown();
	SendKeyCode(13);
}

void LEM_DEDA::PlusPressed()
{
	KeyClick();

	if (State == 3){
		ShiftRegister[State] = 0;
		State++;
	} else 
		SetOprErr(true);
}

void LEM_DEDA::MinusPressed()
{
	KeyClick();

	if (State == 3){
		ShiftRegister[State] = 1;
		State++;
	} else 
		SetOprErr(true);
}

void LEM_DEDA::ReadOutPressed()
{
	KeyClick();

	if (State == 3){
		SendKeyCode(10);
	} else 
		SetOprErr(true);
}

void LEM_DEDA::HoldPressed()
{
	KeyClick();

	if (State == 3 || State == 9){
		SendKeyCode(12);
	} else 
		SetOprErr(true);
}

void LEM_DEDA::NumberPressed(int n)
{
	KeyClick();

	switch(State){
		case 0:
		case 1:
		case 2:
			if (n > 7){
				SetOprErr(true);
				return;
			}
			ShiftRegister[State] = n;
			State++;
			return;
		case 3:
			SetOprErr(true);
			return;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			ShiftRegister[State] = n;
			State++;
			return;
		case 9:
			SetOprErr(true);
			return;
	}
}

void LEM_DEDA::HoldCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_Hold = true;
		HoldPressed();
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::EnterCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_Enter = true;
		EnterPressed();
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::ClearCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_Clear = true;
		ClearPressed();
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::ReadOutCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_ReadOut = true;
		ReadOutPressed();
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::PlusCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_Plus = true;
		PlusPressed();
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::MinusCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_Minus = true;
		MinusPressed();
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::zeroCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_0 = true;
		NumberPressed(0);
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::oneCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_1 = true;
		NumberPressed(1);
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::twoCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_2 = true;
		NumberPressed(2);
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::threeCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_3 = true;
		NumberPressed(3);
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::fourCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_4 = true;
		NumberPressed(4);
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::fiveCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_5 = true;
		NumberPressed(5);
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::sixCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_6 = true;
		NumberPressed(6);
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::sevenCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_7 = true;
		NumberPressed(7);
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::eightCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_8 = true;
		NumberPressed(8);
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::nineCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_9 = true;
		NumberPressed(9);
	}
	else
	{
		ResetKeyDown();
	}
}

void LEM_DEDA::SetAddress()
{
	for (int i = 0;i < 3;i++)
	{
		Adr[i] = ValueChar(ShiftRegister[i]);
	}
}

void LEM_DEDA::SetData()
{
	Data[0] = ValueCharSign(ShiftRegister[3]);

	for (int i = 0;i < 5;i++)
	{
		Data[i + 1] = ValueChar(ShiftRegister[i + 4]);
	}
}

char LEM_DEDA::ValueCharSign(unsigned val)
{
	switch (val) {
	case 0:
		return '+';

	case 1:
		return '-';
	}
	return ' ';
}

char LEM_DEDA::ValueChar(unsigned val)

{
	switch (val) {
	case 0:
		return '0';

	case 1:
		return '1';

	case 2:
		return '2';

	case 3:
		return '3';

	case 4:
		return '4';

	case 5:
		return '5';

	case 6:
		return '6';

	case 7:
		return '7';

	case 8:
		return '8';

	case 9:
		return '9';
	}
	return ' ';
}

void
ChannelOutputAGS(ags_t * State, int Type, int Data)
{
	//
	// This will need to pass the data to the NASSP code.
	//

	LEM_AEA *ags;

	ags = (LEM_AEA *)State->ags_clientdata;
	ags->SetOutputChannel(Type, Data);
}